# Modernization status

## Baseline

The original 3.1.4 project was generated with Projucer 6.0.7. Its Visual Studio
projects require the retired v142 toolset and a JUCE checkout at
`../JUCE_6.0/modules`. Those external assumptions make a clean build fail on a
current Visual Studio installation.

## Current build

- Product name: PreenFM+
- Project version: 4.0.3
- JUCE: 9.0.0, fetched and pinned by CMake
- Language level: C++17
- Windows toolchain: Visual Studio 2026 Build Tools 18.9.0, x64
- Build system: CMake 4.4.2 (Visual Studio 18 2026 generator)
- Formats: VST3 and Standalone
- Bundled VST3 SDK: 3.8.0 through JUCE

The migration replaces generated Projucer projects as the primary build with a
reproducible CMake configuration. A local JUCE checkout can be selected with
`-DJUCE_SOURCE_DIR=<path>` when an offline build is required.

## Application changes

- Migrated MIDI device discovery and opening from numeric indices to stable
  `MidiDeviceInfo` identifiers.
- Migrated host parameter notifications to
  `AudioProcessorParameter::sendValueChangedMessageToListeners`.
- Replaced the former `DrawableImage` component usage with `ImageComponent`,
  matching the JUCE 9 Drawable/Component split.
- Replaced deprecated `ScopedPointer` ownership in the maintained plugin GUI
  with `std::unique_ptr`, preserving member order and component lifetime.
- Updated deprecated JUCE font and notification calls while retaining the
  legacy font metrics used by the existing layout.
- Preserved the released VST3 parameter-ID behaviour. This should still be
  verified by loading sessions containing automation from version 3.1.4.
- Preserved the released VST3 component class ID explicitly during the PreenFM+
  rename, so hosts can continue to associate the new binary with existing
  sessions even though JUCE normally derives this ID partly from the name.
- Enforced a minimum editor size of 900 x 710 pixels, including when restoring
  an older session that stored an unusably small editor size.
- Added the four VOSIM algorithms (29-32) and the Brownian, Wandering and Flow
  LFO shapes from PreenFM3 v1.03 and the matching `pvig/preenfm2` VOSIM
  firmware branch.

## Correctness fixes

The 4.x work uncovered several defects inherited from the original editor. They
are called out explicitly so that maintainers of related PreenFM editors can
evaluate and reuse the fixes:

- Replaced the process-global parameter index counter with JUCE's per-processor
  parameter indices. Multiple simultaneous plugin instances no longer build
  invalid lookup tables.
- Corrected the MIDI-channel domain to 1-16 and sanitize old states and host
  input before filtering or sending MIDI.
- Repaired shared NRPN decoding for LFO frequency/sync, LFO key sync/time and
  step-sequencer BPM/sync. Partner routing now verifies the shared NRPN address,
  invalid sync IDs are normalized and unchanged partners are not redundantly
  reported as host automation.
- Limited hardware preset names to 12 printable ASCII characters and made the
  character stream tolerant of null padding and out-of-order delivery.
- Corrected the step sequencer's array deallocation and input bounds. Its 16
  positions and values 0-15 now match both PreenFM2 and PreenFM3 firmware; old
  out-of-range state values are clamped while loading.
- Corrected the arpeggiator direction and external step-sync ranges to match
  their actual GUI and firmware choices.
- Removed processor-global LookAndFeel state and moved restored editor state to
  JUCE's message thread, avoiding cross-instance and thread-affinity hazards.
- Forwarded host MIDI as raw bytes into the output queue, removing a possible
  allocation from the audio thread while preserving channel and system data.
- Made output and incoming-NRPN queue rejection counters visible in the editor,
  so overload or malformed-event loss is no longer silent.
- Declared integer-valued parameters as discrete host parameters and explicitly
  exposed the ten valid LFO external-sync steps.
- Fixed a missing return in `ListProperty::getFullPathName`, which previously
  invoked undefined behaviour.

The VOSIM diagram, algorithm-29 operator classification and new LFO-shape fixes
are 4.x integration corrections rather than defects in the original release.

## Validation performed

- Clean CMake configure with Visual Studio 2026
- Release x64 VST3 build
- Release x64 standalone build
- VST3 module manifest generation
- VST3 binary architecture and standalone launch smoke tests

Hardware MIDI exchange and loading in representative DAWs remain manual tests.

Version 4.0.2 adds a hardware-preset browser for native PreenFM patch banks.
The UI uses human-readable bank and preset positions 1-128, sends the firmware's
zero-based CC#32 and Program Change values, and requests a full NRPN dump after
the selected patch has loaded. The selected target is stored per plugin
instance.

Version 4.0.3 connects the editor to the PreenFM2 firmware `3.00 alpha` editor
remote protocol, protocol version 1. The protocol occupies NRPN page 4
(parameter numbers 512-639), a range no earlier firmware or editor uses, so an
older firmware simply ignores the requests and answers nothing.

- **Capability detection.** The editor sends a capability query after choosing a
  MIDI device and when the preset browser opens, and offers an explicit
  re-check. Store and the position query are enabled only after the firmware
  has confirmed protocol version 1 and the matching capability bits. Support is
  never inferred from a firmware version string. On timeout the editor reports
  that the protocol is unavailable; Load and Pull keep working.
- **Position query.** A load now asks where the hardware actually is and only
  pulls when the reported bank and preset match the request. A missing bank is
  therefore reported instead of silently looking like a successful load. Bank
  type must be 0, `VALID` must be 1 and both values must be inside the firmware
  limits; `VALID = 0` is treated as an unknown position, not an error, and does
  not overwrite the browser target.
- **Store is an atomic output batch.** The firmware writes its *live edit
  buffer*, so a store is only correct if nothing modifies that buffer between
  the first byte of the pushed snapshot and the store request. Serialising only
  the page-4 request is not enough: the snapshot itself consists of ordinary
  NRPNs, and the shared output queue accepts events from every producer.
  `Pfm2MidiDevice::queueNrpnBatch()` therefore reserves a contiguous run of
  queue cells with a single compare-and-swap on the enqueue position. Competing
  producers are never blocked - they simply receive a later position, so their
  events are emitted either completely before or completely after the batch.
  Either the whole run is reserved or nothing is enqueued, so a rejected batch
  cannot leave a half-transmitted patch on the wire. The parameter values are
  snapshotted before the reservation, so host automation cannot alter the patch
  while it is being sent.
- **Two separate concerns.** Response correlation (who may complete a
  transaction) and MIDI exclusivity (what may be interleaved on the wire) are
  deliberately not merged into one global lock. The first is a monotonic
  transaction token in the shared device; the second is the batch reservation
  above. Nothing in either path blocks the audio thread: `processBlock()` uses
  the same non-blocking single-cell enqueue as before and drops on overflow.
- **Transaction identity.** The claim is a monotonically increasing token
  rather than a raw `this` pointer, and it is not re-entrant. Only the holder
  of the exact token can release it, the processor destructor always releases,
  and a late answer belonging to an older generation can never complete a newer
  transaction.
- **Device generation.** Every queued event carries the device generation it
  was created for. A device change bumps the generation, so events built for
  the previous device are discarded instead of being replayed into whatever is
  opened next, and a store that was in flight is marked as unknown.
- **Honest outcomes.** A store counts as successful only when the echoed target
  matches the request and a following status 0 arrives. A timeout is latched as
  an *unknown* outcome, not a failure: the firmware writes before it answers, so
  the slot may already have been written. Store stays disabled and nothing is
  retried automatically until an explicit position resync. Status 4 likewise
  does not claim that nothing was written, because `savePreenFMPatch()` writes
  payload and padding in two steps. Status 0 is reported as "the firmware saved
  its edit buffer", which is what it proves - a full readback would be needed to
  prove that every preceding NRPN arrived.
- **Bank limit.** Regular patch banks are limited to 1-64, matching the
  firmware's `NUMBEROFPREENFMBANKS`, and not to the theoretical width of CC32.
  Old saved states holding bank 65-128 are clamped to 64, never wrapped to 1.

Required hardware settings:

| setting | needed for |
|---|---|
| `Receives: NRPN` or `CC & NRPN` | every editor-protocol request; with `None` or `CC` the firmware answers nothing |
| `Program change: Yes` | loading a preset |
| `USB MIDI: In/Out` | receiving answers over USB; with `Off` or `In` they only reach the DIN output |
| a dedicated timbre MIDI channel | Store; the firmware refuses with status 3 unless the channel maps to exactly one timbre |

The `Send:` setting has no effect on this protocol; responses are emitted
regardless.

## Remaining work

- Replace the legacy modal MIDI-device chooser with an asynchronous dialog, then
  remove `JUCE_MODAL_LOOPS_PERMITTED`.
- Migrate the separate legacy `Host` bank-management application. The current
  Standalone target is JUCE's wrapper around the plugin editor.
- Test automation/session compatibility in current DAWs and MIDI I/O with real
  preenfm2/preenfm3 hardware.
- Add CI builds and packaging for supported operating systems.

## Licensing note

The editor source files state GPL v3 or later. JUCE 9 modules are dual-licensed
under AGPLv3 or a commercial JUCE licence. Distribution of this open build must
therefore comply with the applicable AGPLv3 requirements; a closed-source build
requires an appropriate commercial JUCE licence. The bundled Exo font retains
its SIL Open Font License.
