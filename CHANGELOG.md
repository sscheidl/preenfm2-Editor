# Changelog

This project follows semantic versioning.

## 4.0.0

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
