# Changelog

This project follows semantic versioning.

## 4.0.3 (in development)

- Replaced the modal hardware-preset callout with a permanent two-row header,
  keeping bank, preset, Load, Store, Position and protocol status visible on all
  editor pages. The redundant Presets and manual Protocol buttons were removed.
- Reordered the Arp & Filter page so the filter appears before the arpeggiator,
  following the more familiar synthesizer signal-flow workflow.
- Added explicit upstream/VOSIM credits and the tAUREON experimental
  AI-assisted development notice.
- **Store is now an atomic output batch.** The frozen patch snapshot, the twelve
  name letters and the store request reserve one contiguous run of queue cells
  with a single compare-and-swap. Competing producers - other plugin instances,
  host automation, forwarded host MIDI - keep enqueueing without blocking, but
  their events can only be emitted completely before or completely after the
  batch, never inside it. Previously a Program Change from another instance
  could replace the hardware edit buffer in the middle of a push, after which
  the firmware stored a hybrid patch and correctly answered status 0.
- The store snapshot is frozen before anything is sent, so host automation
  cannot change parameter values while the batch is being transmitted.
- Load claims the transaction *before* sending Bank Select and Program Change.
  If the claim fails, nothing mutating is sent and no legacy success is faked.
- Replaced the raw-pointer transaction owner with a monotonic token, and made
  the claim non-re-entrant. A late answer from an older generation can no
  longer complete a newer transaction, and a partial completion can no longer
  release the shared claim early.
- The processor destructor now releases its transaction claim. An instance
  dying with an open claim used to block the protocol for every other instance.
- Added a device generation. Queued events carry the generation they were built
  for and are discarded after a device change instead of being replayed into
  the newly opened instrument; an affected store is marked as unknown.
- A store timeout is now latched as an unknown outcome: the firmware writes
  before it answers, so the slot may already have been written. Store stays
  disabled, nothing is retried automatically, and an explicit position resync
  is required to clear the state.
- Firmware status 4 no longer claims that nothing was written. `savePreenFMPatch()`
  writes payload and padding separately, so the slot can already be changed.
- Status 0 is reported as "firmware saved its edit buffer, verify by reloading"
  rather than as proof that every preceding NRPN arrived.
- `sendOutputEvent()` no longer discards events silently when no device is open;
  undelivered events are counted separately from dropped ones.
- Editor page-4 NRPNs are consumed after dispatch, matching the firmware, so a
  second lone CC38 cannot repeat a response. Ordinary parameter pages keep
  their historic behaviour.
- Made all protocol timeout comparisons wrap-safe.
- Added support for the PreenFM2 firmware `3.00 alpha` editor remote protocol
  (protocol version 1) on NRPN page 4.
- Added capability detection. Store and the position query stay disabled until
  the firmware has confirmed a supported protocol version and the matching
  capability bits; support is never inferred from a firmware version string.
- Added a hardware position query. A load now confirms the reported bank and
  preset before pulling, so a missing bank is reported instead of appearing to
  have loaded. Firmware without the protocol keeps the previous delayed pull.
- Added direct Store of the current patch into a hardware bank/preset slot. The
  patch is pushed first, then stored; success requires the echoed target to
  match the request and a following status 0. All six firmware status codes are
  reported with bank, preset and their technical meaning.
- Added an overwrite confirmation before Store, because the firmware writes
  immediately and has no confirmation of its own.
- Serialised editor-protocol transactions across plugin instances. The wire
  protocol carries no request id, so a competing request is refused instead of
  being completed by the wrong instance.
- Limited regular PreenFM patch banks to 1-64, matching the firmware's
  `NUMBEROFPREENFMBANKS`. Older saved states holding bank 65-128 are clamped to
  64 rather than wrapping onto bank 1.
- Editor-protocol responses are intercepted before the normal parameter lookup,
  so they can never be mistaken for synth parameters, preset-name letters or
  step-sequencer values.

## 4.0.2 (in development)

- Added a hardware-preset browser for native PreenFM banks and presets 1-128.
- Added previous/next audition controls using Bank Select and Program Change.
- Automatically requests a full parameter dump after a selected hardware
  preset has loaded.
- Stores the selected hardware bank and preset target per plugin instance.
- Prepared the Store control but leaves it disabled until the matching VOSIM
  firmware protocol and its acknowledgement/error responses are verified.

## 4.0.1

- Fixed MIDI-channel state and output handling so only channels 1-16 are used.
- Fixed the carrier layout for algorithm 29 and several vector-diagram layout
  and scaling issues.
- Fixed incoming shared NRPN handling for LFO frequency/sync, LFO key sync/time
  and step-sequencer BPM/external sync.
- Hardened shared-NRPN partner routing, normalized invalid LFO sync IDs and
  avoided redundant host automation notifications for unchanged partners.
- Limited preset names to 12 printable ASCII characters, matching the hardware.
- Made character-by-character preset-name reception robust against embedded
  nulls and out-of-order characters.
- Fixed the step-sequencer's firmware-compatible 0-15 value range and rendering.
- Corrected out-of-range values while restoring old states. Changing the 32 step
  parameters from 0-16 to 0-15 intentionally changes their normalized host
  automation mapping.
- Kept MIDI output active while the device chooser is open and serialized the
  actual device switch.
- Removed process-global LookAndFeel state and moved editor state restoration
  onto the JUCE message thread.
- Corrected the arpeggiator direction range to the firmware's 12 choices. This
  intentionally changes normalized host automation for that discrete parameter.
- Added arpeggiator pattern 26 (`User 4`). Expanding this parameter from 25 to
  26 choices intentionally changes its normalized host automation mapping.
- Replaced the process-global parameter index counter with JUCE's per-processor
  indices, preventing lookup-table collisions between concurrent instances.
- Forwarded host MIDI from the audio thread as raw bytes instead of constructing
  potentially allocating `MidiMessage` objects. Channel voice messages still
  follow the selected output channel; system and SysEx messages remain intact.
- Made rejected MIDI/NRPN queue events visible as `Midi !` in the editor header,
  with separate incoming and outgoing counts in the tooltip.
- Declared integer-valued host parameters as discrete. LFO external sync reports
  its ten valid values explicitly rather than the 91 integers in its raw range.
- Fixed additional null checks, invalid lookup routing and a missing return that
  could previously invoke undefined behaviour.
- Replaced deprecated JUCE GUI ownership pointers with `std::unique_ptr` and
  removed legacy compiler warnings without changing component lifetime or font
  metrics.

## 4.0.0

- Requires PreenFM firmware with the VOSIM extension; users of older firmware
  should continue to use a 3.x editor.
- Renamed the editor to PreenFM+.
- Migrated the maintained build to JUCE 9.0.0 and CMake.
- Added Visual Studio 2026 and x64 VST3/Standalone builds.
- Modernized MIDI-device and JUCE APIs while explicitly retaining the released
  VST3 component class ID.
- Added a 900 x 710 minimum editor size to prevent broken compact layouts.
- Added algorithms 29-32 with vector VOSIM diagrams for the corresponding
  PreenFM2 VOSIM and PreenFM3 firmware versions.
- Added the Brownian, Wandering and Flow LFO shapes supported by those firmware
  versions.
