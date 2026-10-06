# PreenFM+ 4.0.7 Beta 1 — colour themes

Windows x64 VST3 and standalone pre-release. Experimental AI-assisted tAUREON
project; beta testers are welcome. Not a final production release.

## New

Five new selectable colour presets: **tAUREON, Graphite Amber, Arctic, Forest
and Plum**, plus the existing **Classic** style. Select them in the permanent
header. Your choice is remembered locally and has no effect on sound, MIDI,
hardware presets or DAW parameter state.

The complete editor uses the selected palette, including menus, envelopes
and sequencers. Functional operator colours remain consistent. Screenshots
and the palette gallery are refreshed in the repository:
[Colour themes](https://github.com/sscheidl/preenfm2-Editor/blob/master/docs/COLOUR_THEMES.md).

## Installation

Close the DAW/editor first. Extract the VST3 ZIP and copy the complete
`PreenFM+.vst3` folder into `C:\Program Files\Common Files\VST3`, replacing
the previous version. The standalone ZIP contains `PreenFM+.exe`.
Both binaries report version 4.0.7. ZIP SHA-256 hashes are supplied separately.

## Validation and limitations

Windows builds and offline regression/theme tests run before publication.
The theme tests render all three pages in all six themes at minimum and larger
sizes, check text contrast/selector overlap and confirm no MIDI or synth
parameter changes. New hardware and Studio One validation remains pending;
an independent theme review is requested but not yet completed.

All 4.0.6 MPE limitations remain: configure the hardware/controller MPE zone
and bend range manually; editor channel must match the MPE manager/timbre.
PreenFM2 raw host configuration CC6/38/96–101 is blocked for safety. Native
VST3 Note Expression and CVIN matrix layouts are not supported. See
[MPE setup](https://github.com/sscheidl/preenfm2-Editor/blob/master/docs/MPE_EDITOR_IMPLEMENTATION.md).
