# PreenFM+ 4.0.8 Beta 1 — theme review fixes

Experimental Windows x64 VST3 and standalone pre-release; beta testers welcome.

Addresses all five P3 findings from the independent 4.0.7 theme review:
safe custom-colour lookup, stronger Arctic contrast, effective explicit colour
overrides, themed Store/MIDI dialogs, and expanded tests through the actual UI
selector, name editor, DAW state and two independent processors.

tAUREON is now distinctly near-black/warm charcoal with gold highlights.
**Graphite Rose** replaces Amber with a muted red accent. Saved theme indices
remain compatible. The other palettes and functional operator colours remain.

Windows Debug and Release automated tests pass (10,394 checks per configuration);
Debug ThemeTests capture zero JUCE assertions. Updated screenshots use the actual
editor components. New hardware/Studio One workflow checks and independent
review of these fixes remain pending; this is not a final production release.

Close the host before replacing the complete `PreenFM+.vst3` bundle in
`C:\Program Files\Common Files\VST3`. The standalone ZIP contains `PreenFM+.exe`.
Both report 4.0.8. All MPE safety/setup requirements and limitations from 4.0.6
remain unchanged. No firmware or MIDI routing/protocol changes were made.
