# PreenFM+ 4.0.6 — local verification, 2026-10-06

Source base: `8bdcfa1` (4.0.4). Release source is identified by tag
`v4.0.6-beta.1`. Firmware source was not modified by this work.

## Windows x64 Release

| Suite | Checks | Failures |
|---|---:|---:|
| MidiRoutingTests | 2,850 | 0 |
| EditorProtocolTests | 59 | 0 |
| MpeProcessorTests | 5,384 | 0 |
| Total | 8,293 | 0 |

CTest: all three suites passed. `git diff --check`: passed. Toolchain: MSVC
19.51 / Visual Studio 2026 Build Tools, Windows SDK 10.0.26100, CMake 4.4.2,
JUCE 9.0.0. Existing C4324 structure-padding warnings remain; no build errors.

The processor suite closes MIDI input/output ports before constructing any
processor. It uses the real queue serializer and synchronous drain helper;
capability/position/Store replies are simulated. No test preset was written to
hardware. Its selector probe is an explicit small firmware model, not execution
of the device firmware.

Coverage includes all channels/controllers for configuration filtering, ordinary
versus MPE expression routing, equal-pitch member notes, channel/mode changes,
note/sustain release and retry, actual queue exhaustion, bypass, device generations,
XML/legacy states, unchanged 242-parameter count/order, every editor NRPN entry
point, atomic Store disarm, fresh capability requirements and PreenFM3 pass-through.
GUI construction and minimum-size MPE-switch bounds were checked without opening
an external window.

Both VST3 and standalone are built as 4.0.6. Verify Windows FileVersion and
ProductVersion as well as the generated VST3 manifest; an incremental JUCE RC
generation dependency omission was discovered and fixed during this build.
VST3 component/controller class IDs remain unchanged.

## Not established by these results

- Actual Studio One delivery of MPE pitchbend, Channel Pressure and CC74.
- Standalone device selection and shutdown with physical held notes/sustain.
- Hardware Push/Pull round trip or Store persistence after power cycling.
- Timing/overflow of the hardware USB receive buffer; concurrent direct-DIN RPNs.
- Native VST3 Note Expression, CVIN layouts or non-Windows platforms.
- A new independent Claude review of the final 4.0.6 source. The supplied Claude
  report covered the earlier 4.0.5 prototype; it was used to drive these fixes.

Hardware/host validation is pending. Download ZIP hashes are supplied with the
release in `SHA256SUMS.txt`. The installed 4.0.4 plug-in was not overwritten.

## Commands

```powershell
cmake --preset windows-vs2026-x64
cmake --build build/windows-vs2026-x64 --config Release --target MidiRoutingTests EditorProtocolTests MpeProcessorTests PreenfmEditor_VST3 PreenfmEditor_Standalone --parallel 2
ctest --test-dir build/windows-vs2026-x64 -C Release --output-on-failure
```

See [MPE_EDITOR_IMPLEMENTATION.md](MPE_EDITOR_IMPLEMENTATION.md) for review-fix
dispositions, supported setup, safety boundaries and the hardware test checklist.
