# PreenFM+ 4.0.6 Beta 1

Experimental Windows x64 VST3 and standalone pre-release. Beta testers are
welcome. This build adds opt-in MIDI-channel MPE routing and addresses safety
findings from an independent review of the unreleased 4.0.5 prototype.

**New hardware / Studio One verification is pending.** The earlier 4.0.4 beta
and matching firmware have been exercised on hardware, but those results do not
validate the new MPE routing. This is not a final or production-certified build.

## Changes

- Saved MPE switch preserves member channels for notes, pitchbend, Channel
  Pressure and CC74. Normal mode retains single-channel performance routing.
- Removed unsafe RPN-Null prefixes that could alter PreenFM2 arpeggiator settings.
- Isolated host RPN/NRPN configuration from editor commands and added a warning.
- Atomic Store now disarms its command address to prevent an unintended second
  Store from later bare Data Entry. Snapshot/Store exclusivity is retained.
- Queue drain on final shutdown, tracked note/sustain release on bypass and
  destruction, retry after rejected releases, and fresh protocol detection after
  channel/model/MPE changes.
- Expanded routing, protocol, real-processor and GUI-construction regression
  tests. JUCE 9.0.0, C++17 and the original VST3/host-parameter identities remain.

## Required setup and limitations

1. Use matching VOSIM/MPE developer firmware. USB PolyAT needs the firmware fix
   `1107a62` or a descendant containing it. Firmware sources/binaries are not
   changed by this editor release.
2. Enable Lower-Zone MPE on hardware first. Editor control channel must equal
   both the MPE manager and the configured channel of the MPE timbre.
3. Configure the hardware/controller zone and pitchbend range manually to match.
   For PreenFM2, host CC6/38/96-101 configuration is blocked in both routing modes;
   controller RPN0/6 auto-configuration therefore does not reach the hardware
   through this editor. Editor-generated NRPN editing still works normally.
4. Do not configure another direct MIDI/DIN controller during editor Push/Store.
   Finish its RPN setup with RPN Null before editor use. Concurrent external
   configuration needs a firmware-level selector change and is not supported.
5. PolyAT uses the master or ordinary non-member channel; per-member MPE pressure
   uses Channel Pressure. CVIN firmware matrix layouts are not supported.
6. Native VST3 Note Expression is not implemented. Actual channel/expression
   delivery depends on the host. Studio One verification of this build is pending.

**Back up important banks; first test Store on a disposable slot.** Check the
target and reload after a power cycle. Neither passing local tests nor a MIDI
queue acknowledgement proves successful persistent hardware storage.

## Install and test

- VST3: close the DAW, unpack `PreenFM+.vst3` into
  `C:\Program Files\Common Files\VST3\` or your configured VST3 directory;
  preserve the bundle's `Contents` directory and rescan/restart the DAW.
- Standalone: unpack `PreenFM+.exe` into its own folder and choose the PreenFM MIDI
  input/output. Test it separately for device selection and application shutdown;
  successful standalone behaviour does not establish VST3 host delivery.
- `SHA256SUMS.txt` records hashes of both downloadable ZIPs.

Start with note/expression routing and unchanged arp settings, then Push/Pull,
and only then Store. See
[the complete setup/test checklist](https://github.com/sscheidl/preenfm2-Editor/blob/master/docs/MPE_EDITOR_IMPLEMENTATION.md).
Report Windows/DAW/firmware versions, MIDI channels, reproduction steps and raw
MIDI output. The existing 4.0.4-beta.1 release remains available for comparison.

Development is a tAUREON experiment in human-directed collaboration with Codex
and Claude Code. See the repository's AI-development notice and upstream credits.
