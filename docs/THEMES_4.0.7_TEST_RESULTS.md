# PreenFM+ 4.0.7 theme validation

Date: 2026-10-06. Windows x64, Visual Studio 2026/MSVC, JUCE 9.0.0,
CMake Release build. VST3 DLL, standalone EXE and VST3 manifest report 4.0.7.

## Automated checks

| Suite | Checks | Failures |
| --- | ---: | ---: |
| EditorProtocolTests | 59 | 0 |
| MidiRoutingTests | 2,850 | 0 |
| MpeProcessorTests | 5,384 | 0 |
| ThemeTests | 1,797 | 0 |
| Total | 10,090 | 0 |

ThemeTests cover all six palettes, all three tabs, minimum 900×778 and larger
1500×950 sizes, selector bounds/overlap, inherited palette, text/status contrast,
unchanged synth parameters and MPE flag, and an empty MIDI queue after theme
changes. A temporary, isolated preference file verifies save/reopen and
independent existing LookAndFeel instances; it is removed afterward. The tests
do not alter the user's saved appearance.

GUI tests stop the MIDI worker and close ports before constructing processors;
queued commands are discarded, never sent to hardware. Existing protocol and
performance regression suites also pass. Firmware and MIDI protocol code are
unchanged in this release.

The optional screenshot-export run produces actual GUI captures. The gallery
contains the five new Engine palettes and tAUREON Modulation/Arp & Filter pages.
tAUREON's three pages and Arctic Engine were visually inspected. A snapshot is
not equivalent to a real host or physical-display test.

## Remaining manual review

- Use the header selector in Studio One and standalone; close/reopen to confirm
  the saved preference, and check an already open second instance stays independent.
- Inspect open dropdowns, editable preset name, disabled controls, hover/focus
  states and all pages at minimum size and non-default DPI settings.
- Confirm familiar MIDI/hardware editing still works on the installed binary.
- Independent Claude theme review is requested, not completed yet.

No new Studio One/hardware validation is claimed. Existing alignment-padding
warnings C4324 remain; the build succeeds without new source-level diagnostics.
