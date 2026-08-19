# Modernization status

## Baseline

The original 3.1.4 project was generated with Projucer 6.0.7. Its Visual Studio
projects require the retired v142 toolset and a JUCE checkout at
`../JUCE_6.0/modules`. Those external assumptions make a clean build fail on a
current Visual Studio installation.

## Current build

- Product name: PreenFM+
- Project version: 4.0.1
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
