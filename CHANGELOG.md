# Changelog

This project follows semantic versioning.

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
