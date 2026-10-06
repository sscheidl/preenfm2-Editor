# PreenFM+ 4.0.6 — MPE implementation and review fixes

## Status and scope

Base: editor 4.0.4, commit `8bdcfa1`. The unreleased 4.0.5 prototype
was reviewed by Claude on 2026-10-06. This 4.0.6 revision addresses that review.
Hardware/Studio One tests of the new binary are still pending. Earlier successful
hardware tests belong to 4.0.4 and to the firmware, not automatically to 4.0.6.

The software is experimental AI-assisted development, not a final release.
Firmware is unchanged by this editor fix. Use the MPE firmware with USB PolyAT
support (fix `1107a62` or a descendant containing it), not the older
`feature/full-mpe` reference `374be62` alone.

## Routing and required hardware setup

The saved MPE switch preserves note and expression channels. Normal mode remaps
them to the editor's selected channel. All 242 host parameters retain their
identity and order. MPE is a non-parameter XML attribute; old host presets/sessions
without it switch back to normal mode. Host dirty-state notification is sent for
a user mode change, not while restoring that attribute.

For PreenFM2, host performance RPN/NRPN configuration is deliberately blocked:
CC 6, 38, 96, 97, 98, 99, 100 and 101. This applies on all channels in both routing
modes. Notes, PolyAT, Channel Pressure, pitchbend, CC74 and sustain still pass.
A shared counter and header warning make suppression visible. Editor-generated
NRPNs use a separate path and are unaffected; PreenFM3 keeps legacy raw CC routing.

Configure the hardware MPE zone and bend range manually to match the controller.
Do not depend on a controller's RPN 0/6 setup being forwarded by the editor.
The supported control configuration is:

`editor channel = MPE manager channel = configured MIDI channel of the MPE timbre`.

Activate the hardware zone first. Member channels carry per-voice pitchbend,
Channel Pressure and CC74. PolyAT is meaningful on the manager or an ordinary
non-member channel; this firmware ignores PolyAT on member channels. Pressure
uses matrix source After touch (10); slide uses CC74 (19) in non-CVIN builds.
CVIN firmware shifts source IDs and is not supported by this editor matrix list.

## Safety changes and review disposition

- **P1-1:** Removed every editor RPN-Null prefix. Protocol v1 cannot report whether
  the addressed channel is actually in an active MPE zone; sending CC100/101
  blindly would change legacy arpeggiator controls. The editor never sends those
  controllers merely because its MPE flag is enabled. Host configuration is
  isolated instead of guessed from controller history.
- **P1-2:** Host configuration/data-entry messages cannot cross into the editor's
  selected NRPN. Every atomic Store batch ends with harmless page 4 / LSB 127.
  This disarms the Store address before later bare CC6/38 can retrigger it.
  The disarm is in the same queue reservation, not a separately enqueued command.
- **P2-1:** Processor destruction releases its tracked notes/sustain. Device
  destruction stops the output worker and drains the remaining queue before
  invalidating/closing the ports.
- **P2-2:** Bypass releases tracked notes/sustain and discards incoming performance
  MIDI. Documentation describes instance deactivation, not transport stop.
  Transport stop still relies on host Note-Offs so live notes are not cut off.
  A routing change takes effect at the next audio callback (or deactivation);
  changing the switch alone while an engine is suspended is not a panic button.
- **P2-3:** Channel, model and MPE changes invalidate capabilities immediately.
  Message-thread handling resets position/protocol state, abandons open
  transactions (Store outcome becomes unknown), and starts a fresh query.
  Store is blocked before the new context is confirmed. Capability and delayed
  Load/Pull also disable the header's MPE switch.
- **P2-4:** Still requires the user's host test. JUCE 9's VST3 wrapper does not
  use supportsMPE() to negotiate channel delivery. Notes/Poly Pressure and
  channel-mapped controllers must actually reach processBlock. Native VST3
  Note Expression is not implemented; local routing tests do not establish it.
- **P2-5:** No repeated RPN prefixes. Push/restore use the old 2,964-byte sequence.
  Store is 2,988 bytes, only one 12-byte disarm command more than the old 2,976.
- **P3-1:** Rejected owned Note-Offs and sustain-off are retried on later callbacks.
  Failed routing cleanup remains pending. Under overflow the current performance
  block can still be dropped; no unbounded audio-thread waiting is introduced.
- **P3-2:** A new device generation discards old ownership and queued events.
  This intentionally does not send old notes to a replacement device. Release
  keys/pedal before reconnecting, including reselecting the same physical port.
  Shared hardware channels/pitches do not have exclusive per-instance ownership.
- **P3-3/4/5:** Corrected firmware/PolyAT test assumptions, expanded real-processor
  tests to all NRPN paths, added Projucer header entry, version-independent protocol
  UI labels, host dirty-state notification and explicit matrix/host limitations.
  The friend test access has no exported runtime control interface.

## External MIDI and unavoidable limits

The editor cannot inspect or serialize another controller connected directly
to the hardware DIN/USB input. Such a controller must finish its RPN setup with
RPN Null before editor use, and must not send configuration during Push/Store.
Otherwise the firmware can consume editor data as RPN values. Supporting concurrent
external configuration robustly requires a firmware selector/protocol change;
this editor-only release does not silently modify the hardware firmware.

The final Store disarm protects against later bare data bytes, not arbitrary
explicit Store commands sent by another application, nor another physical input
interleaving during the snapshot. Back up banks and use a disposable Store slot.

Delivery still depends on open ports and finite queue capacity. MIDI output uses
the existing asynchronous worker, not sample-accurate timestamps. No software
cleanup guarantees delivery to unplugged or unresponsive hardware.

## Local verification

Windows x64 Release build and test results are recorded in
[MPE_4.0.6_TEST_RESULTS.md](MPE_4.0.6_TEST_RESULTS.md).
Tests close the MIDI ports before constructing processors: no test Note-On or
Store is sent to physical hardware. They use the real queue serializer and
simulate firmware responses. The small selector model tests disarm semantics
but is not firmware execution. The editor-construction/layout smoke has no
visible external window.

Run:

```powershell
cmake --preset windows-vs2026-x64
cmake --build --preset windows-vs2026-x64-release --target EditorProtocolTests MidiRoutingTests MpeProcessorTests
ctest --test-dir build/windows-vs2026-x64 -C Release --output-on-failure
cmake --build --preset windows-vs2026-x64-release
```

## Hardware / Studio One test order

1. Back up banks. Configure Lower Zone, matching manager/timbre/editor channel,
   member channels and bend range. Receives must permit NRPN.
2. Enable editor MPE. Hold two notes on channels 2 and 3; vary pitchbend,
   Channel Pressure and CC74 independently. Repeat at equal pitch and release
   separately. Use an output MIDI monitor to check actual host channel delivery.
3. Test PolyAT on the manager or a normal non-member channel, not member channels.
4. Save/reopen the session; check the MPE flag. Check normal mode, channel change,
   bypass, instance deactivation and closing the last instance with held sustain.
5. Check Capability/Position, Push then Pull and compare parameter values.
   Arpeggiator clock/direction must not change when editor MPE is enabled but
   hardware MPE is disabled. This counter-test does not require Store.
6. Store only into a disposable slot. Verify target, name and parameters after
   power cycling. Confirm no other slot changed. Never use a valuable bank for
   the first Store test; do not simultaneously configure a direct MIDI controller.
