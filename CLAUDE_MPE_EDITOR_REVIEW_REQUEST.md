# Auftrag an Claude Code: PreenFM+ 4.0.6 MPE-Fixes nachprüfen

## Ziel

Führe eine unabhängige, gründliche Review der neuen MPE-Erweiterung des Editors
durch. Prüfe den tatsächlich vorliegenden Code und die Tests; übernimm die
Behauptungen des Implementierungsberichts nicht ungeprüft. Schreibe den Bericht
als `CLAUDE_MPE_EDITOR_REVIEW_4_0_6.md` in den Editor-Projektstamm. Bewahre den
ursprünglichen 4.0.5-Bericht unverändert. Bearbeite keinen
Quellcode und veröffentliche nichts auf GitHub.

Editor-Verzeichnis:
`D:\Eigene Dateien\Eigene Dokumente\Playground\Preen FM VST3 Editor`

Firmware-Verzeichnis:
`D:\Eigene Dateien\Eigene Dokumente\Playground\preenfm2-vosim-firmware-project\preenfm2-vosim-editor-firmware`

Editor-Arbeitszweig: `codex/mpe-routing`. Basis: Commit `8bdcfa1` (Editor 4.0.4).
Nach dem Push kann `fork/master` bereits 4.0.6 enthalten. Ermittle den Diff mit
`git diff 8bdcfa1` und
berücksichtige auch neue, noch ungetrackte Dateien. Firmware-Referenz:
`feature/full-mpe`, bei Prüfung Commit `374be62`. Bewahre vorhandene lokale
Änderungen und beachte die AGENTS.md im Playground.

## Hintergrund und Sollverhalten

Der Benutzer bestätigt einen erfolgreichen Hardwaretest der Firmware mit
Polyphonic Key Pressure und Lower-Zone MPE. Der bisherige Editor 4.0.4 schreibt
alle Performance-MIDI-Nachrichten auf einen Ausgangskanal um und verliert damit
die MPE-Notenzuordnung. Die Firmware behält das Patchformat und das
Editor-Remote-Protokoll Version 1 bei.

Die neue Version 4.0.6 soll:

1. Im normalen Modus weiter auf den ausgewählten MIDI-Kanal umschreiben.
2. Im MPE-Modus sämtliche Performance-Kanäle und Datenbytes erhalten:
   Note On/Off, PolyAT, Channel Pressure, Pitchbend, CC74 und Sustain.
   RPN/NRPN-Hostkonfiguration (CC6/38/96-101) wird für PreenFM2 absichtlich
   blockiert. MPE-Zone/Bend-Bereich werden an Hardware und Controller eingestellt.
3. Editorbefehle weiterhin auf dem gewählten Steuerkanal senden. Hardware-MPE
   wird separat am Gerät konfiguriert. Steuerkanal und Antwortkanal müssen
   zusammenpassen; ein MPE-Member-Kanal nimmt keine Editorbefehle entgegen.
4. Den MPE-Modus im Pluginzustand speichern; alte Sessions ohne neues Attribut
   erhalten den bisherigen normalen Modus. Parameterzahl, IDs und Reihenfolge
   dürfen keine bestehende Automation beschädigen.
5. Bei Moduswechsel, normalem Kanalwechsel, Bypass und Instanz-Deaktivierung weitergeleitete
   gehaltene Noten/Sustain auf ihren bisherigen Kanälen bereinigen. Keine
   pauschale Panic über alle Hardware-Timbres. Bei Gerätewechsel dürfen alte
   Bereinigungsnachrichten nicht an ein neues Gerät gesendet werden.
6. Keine RPN-Null-Präfixe ausgeben. PreenFM2-Hostkonfiguration wird vom
   Ausgabe-Worker isoliert. Store endet atomar auf Seite 4 / LSB 127, um die
   Store-Adresse zu entschärfen. Prüfe auch die Grenzen bei einem externen DIN-
   Controller: dessen RPN-Zustand kann der Editor nicht kontrollieren.

PolyAT verwendet in dieser PreenFM2-Firmware die vorhandene Matrixquelle
`After touch` (ID 10), MPE-Slide die vorhandene Quelle CC74 (ID 19). Die separate
Quelle `Polyphonic Aftertouch` (ID 21) im Editor gehört zum PreenFM3.

## Besonders wichtige Dateien

- `Plugin/Source/MidiPerformanceRouter.h`: Kanalrouting, Noten/Sustain-Buchführung,
  Moduswechsel, Gerätegeneration und Editor-NRPN-Bytefolge.
- `Plugin/Source/PluginProcessor.cpp` und `.h`: processBlock, releaseResources,
  Host-MPE-Unterstützung, Atomics, XML-Speicherung/Wiederherstellung und ALLE
  Aufrufer von queueNrpn/queueNrpnBatch.
- `Plugin/Source/Pfm2MidiDevice.cpp` und `.h`: protectPreenfmProtocol-Flag über
  Queue/Dequeue, tatsächliche Ausgabe, Shutdown-Drain, Generationen, Queueüberlauf
  und Batch-Kontiguität.
- `Plugin/Source/UI/MainTabs.cpp`: MPE-Schalter im permanenten Header, Timer,
  Wiederherstellung, Tooltip und Verhalten bei laufenden Store-Transaktionen.
- `Plugin/Tests/MidiRoutingTests.cpp` und ggf. `MpeProcessorTests.cpp`:
  Nachweis der realen Produktionspfade, nicht nur Wunschverhalten.
- `CMakeLists.txt`, `Plugin/pfm2Plugin.jucer`, `CHANGELOG.md`,
  `docs/MPE_EDITOR_IMPLEMENTATION.md`: Versionierung, Build, Claims und Grenzen.
- Firmware: `src/midi/MidiDecoder.cpp/.h`, `src/synth/Timbre.cpp/.h`,
  `src/synth/Common.h`, `docs/MPE_IMPLEMENTATION_REPORT.md`.

## Review-Fragen und Tests

- Können zwei gleichzeitig gehaltene Noten (auch dieselbe Tonhöhe auf zwei
  Member-Kanälen) unabhängig Press, Slide und Glide bekommen?
- Bleiben Note On und Note Off beim Umschalten zusammengehörig? Prüfe
  Velocity-zero Note On, überlappende Noten, Sustain, Queueüberlauf und Retry.
- Prüfe Stop/Start, Reset/Restore, Schließen der Instanz, Gerätewechsel und
  mehrere Plugin-Instanzen. Unterscheide gemeinsam genutzten Port und getrennte
  Instanzzustände. Benenne bestehende Grenzen ausdrücklich.
- Wird Hostkonfiguration auf allen PreenFM2-Kanälen isoliert, ohne Noten,
  Pressure, CC74 oder Sustain zu blockieren? Bleibt PreenFM3 unverändert?
  Erhalten Presetname, Push/Pull, Capability, Position und Store dieselben
  unpräfixierten NRPNs? Kann die Store-Entschärfung vor/innerhalb des Snapshots
  ausgegeben werden oder fehlen? Prüfe getrennte RPN-Teilfolgen über Audioblöcke,
  Queueüberlauf und tatsächliche Grenzen bei externem DIN.
- Firmware-Antworten kommen auf dem konfigurierten Timbre-Kanal zurück.
  Manager- und Timbre-Kanal können verschieden sein: Prüfe die Folgen für
  Editor-Antwortfilter, Store/Position und Push/Pull. Keine unbelegte pauschale
  Kompatibilitätszusage für alle Kanalbelegungen.
- Prüfe VST3/Studio-One-Hostanmeldung und JUCE-9-MIDI-Konvertierung: Welche
  Ausdrucksdaten werden als MIDI-Ereignisse an processBlock geliefert? Reicht
  supportsMPE() für die behauptete Hostunterstützung tatsächlich aus?
- Prüfe, dass normales MIDI, PreenFM3 und alte State-Dateien unverändert nutzbar
  bleiben. Vergleiche Parameter-IDs/-Anzahl mit der Basis, nicht nur Versionen.
- Bewerte Echtzeitfähigkeit: kein Heap/Lock/GUI im Audio-Callback, begrenzte
  Schleifen und korrektes Threadmodell für Setter, Restore und Shutdown.

Führe die verfügbaren Tests selbst aus, soweit lokal möglich:

```powershell
cmake --preset windows-vs2026-x64
cmake --build --preset windows-vs2026-x64-release --target EditorProtocolTests MidiRoutingTests MpeProcessorTests
ctest --test-dir build/windows-vs2026-x64 -C Release --output-on-failure
cmake --build --preset windows-vs2026-x64-release
```

Das `MpeProcessorTests`-Target liegt vor und muss mitgeprüft werden.
Hardware/Studio-One-Tests nur bei tatsächlich verfügbarem, ausdrücklich
adressiertem Testaufbau; keine Bank überschreiben. Fehlende Hardwareprüfung
transparent als offene Prüfung aufführen.

## Bericht

Gib zuerst ein begründetes Go/No-Go für den nächsten Hardware-/Studio-One-Test
an, getrennt von einer Freigabe zur öffentlichen Veröffentlichung. Liste
Befunde nach P0/P1/P2/P3 mit Datei/Zeile, Auslöser, tatsächlicher Folge und
kleinstem sinnvollem Korrekturvorschlag. Unterscheide bestätigte Fehler,
plausible Risiken und nicht geprüfte Punkte. Nenne die ausgeführten Befehle,
Ergebnisse und exakten geprüften Commits/Diff. Wenn nichts gefunden wird,
schreibe nicht "fehlerfrei", sondern benenne die Grenzen der Prüfung.

Lies zuletzt `docs/MPE_EDITOR_IMPLEMENTATION.md` für den finalen lokalen
Teststand. Diese Datei kann während der Umsetzung ergänzt werden.
