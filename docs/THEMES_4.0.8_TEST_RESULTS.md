# PreenFM+ 4.0.8 theme-review fixes

Windows x64, Visual Studio 2026/MSVC, JUCE 9.0.0, 2026-10-06.

## Changes against 4.0.7

- **T1:** `PreenTheme::colour` checks the LookAndFeel before querying a custom
  ID. Unattached/hidden components and teardown safely return transparent.
- **T2:** Arctic IM colours use `darker(0.9f)`; its carrier outline/output is
  dark teal. Enabled IM text is checked at ≥4.5:1 and carrier visibility ≥3:1.
- **T3:** JUCE's colour scheme is set before explicit editor/toggle/alert
  overrides. The intended values are tested, not just palette constants.
- **T4:** Store confirmation is parented to the actual plugin editor (also in
  standalone), retaining the safe callback and Store=1/Cancel=0 semantics.
  MIDI dialog parent/theme is passed through the UI call only. Its LookAndFeel
  is owned locally, avoiding a borrowed editor palette in the shared MIDI device.
  Port selection, routing and hardware operations are unchanged.
- **T5:** Tests exercise the real selector with isolated preferences, byte-equal
  serialized DAW state, a second processor/editor, actual name-editor colours,
  menu/tick/alert overrides, tab colours, IM labels and selected/hover contrast.
- tAUREON now uses near-black/warm charcoal with gold; Graphite Rose replaces
  Amber with muted red. Existing preference indices are preserved.

## Automated results

| Suite | Checks per configuration | Failures |
| --- | ---: | ---: |
| EditorProtocolTests | 59 | 0 |
| MidiRoutingTests | 2,850 | 0 |
| MpeProcessorTests | 5,384 | 0 |
| ThemeTests | 2,101 | 0 |
| Total | 10,394 | 0 |

Both Debug and Release suites are run. Debug defines `JUCE_LOG_ASSERTIONS=1`;
ThemeTests capture assertion logs and check zero during construction, real
selection, all tabs and editor teardown. The first expanded Debug run exposed
a test-harness focus assertion (editing a label without a showing peer), not
a theme-colour assertion. The harness now uses a temporary non-interactive peer
off screen; assertions are not suppressed. The corrected Debug run passes.

The optional Release screenshot-export run passes 2,120 checks and generates
18 actual GUI captures, all themes × all tabs. MIDI ports/worker are shut down
before processor construction; no hardware messages or Store are sent. Isolated
preference files are removed and the user's actual preference file is untouched.

## Limits

Dialog palette inheritance/controls are tested with real AlertWindow components;
the interactive Store-confirmation/port-selection workflow still needs manual
host verification. No real Store, MIDI port switch or hardware test is claimed.
Different DPI, multiple monitors and user focus interactions remain manual tests.
The new patch has not yet received a second independent review.

Pre-existing MSVC padding warnings C4324 remain. Debug also shows legacy pointer
truncation diagnostics C4311/C4302 in the old DBG line at PluginProcessor.cpp:1495;
that diagnostic-only old code was outside this theme task and remains unchanged.
