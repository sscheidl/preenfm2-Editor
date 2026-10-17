# PreenFM+ 4.0.5 — Review des MPE-Routings

Review vom 2026-10-06 durch Claude (Cloud-Sitzung mit Dateizugriff auf beide
Verzeichnisse). Es wurde kein Quellcode geändert und nichts veröffentlicht. Dieser
Bericht ist die einzige neue Datei.

## 1. Entscheidung

**Nächster Hardware-/Studio-One-Test: Go, mit Auflagen.**
Routing, Notenbuchführung, XML-Zustand und die Bytefolge der Editor-NRPNs tun in der
vorgesehenen Konfiguration, was der Auftrag verlangt. Ich habe keinen Fehler gefunden,
der den Test in dieser Konfiguration sinnlos oder gefährlich macht. Auflagen:

1. Nur die einfache Konfiguration testen: MPE-Instrument N, dessen MIDI-Kanal = MPE-Master
   (Manager-Kanal der Zone) = Kanal im Editor. Zone an der Hardware zuerst einschalten
   und prüfen, **dann** den MPE-Schalter im Editor setzen.
2. Die Arpeggiator-Zeile der Hardware als Kontrollanzeige benutzen: Steht nach einer
   Editoraktion `Clk = Ext` und `Dire = ShUD`, dann erreichen die RPN-Null-Bytes die
   normale CC-Verarbeitung (Befund P1-1) und die Konfiguration stimmt nicht.
3. Vor dem ersten Store die SD-Karte sichern. Store nur in den Wegwerf-Slot, und nicht
   in dem Moment, in dem der Controller eingesteckt, neu konfiguriert oder die
   Wiedergabe gestartet wird (Befund P1-2).
4. Firmware-Build mit Commit `1107a62` verwenden (USB-PolyAT), siehe P3-3.

**Öffentliche Veröffentlichung: No-Go.** Offen sind zwei P1-Befunde, die einen
gespeicherten Patch verändern bzw. einen fremden Slot überschreiben können, ein
bestätigter Verlust der Aufräumnachrichten beim Schließen (P2-1) und die noch völlig
ungeprüfte Zustellung durch Studio One (P2-4).

## 2. Geprüfter Stand

| | |
|---|---|
| Editor | Zweig `codex/mpe-routing`, `HEAD` = `fork/master` = `8bdcfa185d5dfe3690fcd201315a8375b5f7e01e` |
| Änderungen | vollständig uncommitted: 8 geänderte Dateien (92 Zeilen hinzu, 23 entfernt), dazu ungetrackt `MidiPerformanceRouter.h`, `MidiRoutingTests.cpp`, `MpeProcessorTests.cpp`, `docs/MPE_EDITOR_IMPLEMENTATION.md`, der Auftrag |
| Firmware | `feature/full-mpe` = `374be62f05ead717b1fa674abbd97a2deac643d8`, inhaltlich sauber |
| JUCE | 9.0.0 = `f8f8864172464b9adf9eba6101e1f784838d1597` (lokal in `build/.../_deps/juce-src` und für meinen Build frisch von GitHub, gleicher Commit) |

SHA-256-Anfänge der geprüften Dateien, zum Abgleich:

```
41ebd5578593  Plugin/Source/MidiPerformanceRouter.h
65cfef764da0  Plugin/Source/PluginProcessor.cpp
1af54f4ca050  Plugin/Source/PluginProcessor.h
cf9246dea688  Plugin/Source/Pfm2MidiDevice.cpp
c5727a2aceee  Plugin/Source/Pfm2MidiDevice.h
fd8c40ce9f14  Plugin/Source/UI/MainTabs.cpp
50d45333aad8  Plugin/Tests/MidiRoutingTests.cpp
c8073cf7dc2f  Plugin/Tests/MpeProcessorTests.cpp
c1582816c7b5  CMakeLists.txt
9e6cb3e1d313  docs/MPE_EDITOR_IMPLEMENTATION.md
```

`docs/MPE_EDITOR_IMPLEMENTATION.md` habe ich zuletzt erneut geprüft: unverändert seit
18:02:58 UTC, gleicher Hash wie beim ersten Lesen.

Hinweis zum Diff: Über die Linux-Brücke meldet `git diff --stat fork/master` 159
Dateien, weil der Arbeitsbaum CRLF enthält. Mit `-c core.autocrlf=true
-c core.fileMode=false` bleiben genau die 8 oben genannten Dateien. Unter Windows
sollte der Diff ohne Zusatz so aussehen.

## 3. Befunde

Kennzeichnung: **bestätigt** = am vorliegenden Code nachvollzogen und, wo angegeben,
durch einen Lauf belegt. **Plausibel** = folgt aus dem Code, Eintritt hängt von
Hardware, Host oder Timing ab und wurde nicht beobachtet. Kein Befund wurde an Hardware
oder in Studio One geprüft.

Kein P0.

### P1-1 — RPN-Null hängt am Editor-Schalter, nicht am Zustand der Hardware

*Bestätigt am Code beider Seiten und per Modellrechnung; nicht an Hardware.*

- Editor: `PluginProcessor.h:236-238` (`shouldClearRpnForEditor()` = Schalter an und
  Typ PreenFM2), Bytefolge `MidiPerformanceRouter.h:105-109`, jedes Batch-Element
  `Pfm2MidiDevice.cpp:521`.
- Firmware: CC 100/101 werden nur auf Kanälen der aktiven Zone als RPN geschluckt
  (`MidiDecoder.cpp:242-247`, `468-479`). Überall sonst sind sie `CC_ARP_CLOCK` und
  `CC_ARP_DIRECTION` (`854-861`), aktiv bei `Receives` mit CC-Anteil; Werkseinstellung
  ist `CC & NRPN` (`SynthState.cpp:749`), MPE ist ab Werk aus (`:759`). Der Wert 127
  wird auf das Maximum begrenzt: `Clk = Ext`, `Dire = ShUD` (`SynthState.cpp:117,119`).

**Auslöser:** Editor-Schalter an und eines davon: MPE an der Hardware aus, Firmware ohne
MPE, oder der Editorkanal liegt außerhalb der Zone (zweite Instanz für ein anderes
Timbre). Naheliegend ist der Fall, dass der Schalter im DAW-Projekt gespeichert ist, die
MPE-Einstellung der Hardware aber nicht.

**Folge:** Jede Editor-NRPN stellt den Arpeggiator dieses Timbres auf externen Takt.
Ohne MIDI-Clock bleibt das Timbre danach stumm. Das passiert bei jeder Parameteränderung,
bei jeder Automation, beim Öffnen des Editors (Capability-Abfrage) und beim Laden der
Session (247 NRPNs). Beim Store steht das Präfix auch unmittelbar vor dem Store-Befehl,
also nach den Arp-Werten des Batches: **Der Slot wird mit `Clk = Ext`, `Dire = ShUD`
geschrieben**, unabhängig vom Editorzustand.

Spiegelfall, ebenfalls aus dem Code: Schalter aus, Zone an, auf dem Master-Kanal steht
noch eine RPN-Auswahl (aus einer zweiten Instanz oder vom direkt angeschlossenen
Controller). Dann schluckt die Firmware CC 6/38 der Editor-NRPN. Bei stehender RPN 6
setzt der Wert-MSB die Member-Anzahl der Zone (`MidiDecoder.cpp:520-542`); der Parameter
kommt nicht an.

**Kleinster sinnvoller Korrekturvorschlag**

1. Im Batch nur das erste Element mit Präfix senden: `Pfm2MidiDevice.cpp:521`
   → `cell.event.clearRpn = clearRpn && index == 0;`. Innerhalb des zusammenhängenden
   Batches kann über diesen Port keine neue RPN-Auswahl entstehen, und die Arp-Elemente
   des Batches stellen die Werte vor dem Store wieder richtig. `MpeProcessorTests.cpp:119-120`
   muss dann angepasst werden. Das beseitigt den gespeicherten Schaden und spart ein
   Drittel der Bytes.
2. Für alle anderen NRPNs das Präfix nur senden, wenn über diesen Port tatsächlich eine
   RPN-Auswahl auf dem Kanal hinausging. Das lässt sich ohne neue Synchronisation im
   Worker entscheiden (`sendOutputEvent`, einziger Konsument): pro Kanal merken, ob
   zuletzt CC 100/101 ungleich 127 gesendet wurde; bei einer Editor-NRPN mit gesetztem
   Flag nur dann 101/100 = 127 voranstellen, danach zurücksetzen; bei Gerätewechsel
   löschen. Eine RPN, die die Hardware auf anderem Weg erreicht (DIN), deckt das nicht
   ab; dafür genügt ein einmaliges Präfix je Gerätegeneration und Kanal.
3. Tooltip und Doku: „Nur einschalten, wenn die Zone an der Hardware aktiv ist und der
   Editorkanal der MPE-Master ist."

### P1-2 — Das Präfix kann Controller-RPN-Daten verwaisen lassen; die Store-Adresse bleibt scharf

*Mechanismus bestätigt am Code, durch den Firmware-eigenen Test und per Modellrechnung.
Eintritt plausibel: Timing über eine Blockgrenze bzw. hostabhängig.*

- Der Router reicht CC 101/100/6/38 unverändert und einzeln in die Queue
  (`MidiPerformanceRouter.h:44`, `PluginProcessor.cpp:631-633`). Editor-NRPNs anderer
  Threads können zwischen zwei dieser Events landen.
- Die NRPN-Adresse der Firmware bleibt nach einem Seite-4-Befehl stehen. Ein frisches
  Paar CC 6 + CC 38 löst den Befehl erneut aus (`MidiDecoder.cpp:906-913`, `1177-1192`).
  Die Firmware testet das als gewollt: `test/host/protocol_sim_test.py:559-570`
  („value bytes may be resent alone", zweiter Slot wird geschrieben).
- Der Store-Befehl ist das letzte Batch-Element (`PluginProcessor.cpp:956-958`).

**Auslöser und Folge**

| Folge auf der Leitung | Wirkung |
|---|---|
| `CC101 CC100` · Editor-NRPN für Parameter P · `CC6=48 CC38=0` | P erhält den Rohwert 6144. Tritt ein, wenn eine RPN über zwei Audioblöcke verteilt ankommt und dazwischen automatisiert wird: JUCE wendet Parameteränderungen vor `processBlock` an. |
| `CC101 CC100` · Store-Batch · `CC6=n CC38=m` | Zweiter Store in Bank n+1, Preset m+1, sofern die Bank existiert. Der Editor ist dann schon im Leerlauf und zeigt nichts an (`PluginProcessor.cpp:1191-1193`). |
| Store · später nacktes `CC6 CC38` auf dem Editorkanal | Wie oben. Besteht schon in 4.0.4, wo der Normalmodus jede Data-Entry-Nachricht auf den Editorkanal umschreibt. MPE-Controller senden RPN 0 und RPN 6 regelmäßig, dadurch wird es wahrscheinlicher. |

Unter VST3 kommt hinzu, dass CC 101, 100, 6 und 38 vier getrennte Parameter sind. JUCE
übernimmt die Reihenfolge der Host-Queues (`juce_audio_plugin_client_VST3.cpp:3523-3531`),
bei gleicher Sample-Position ist sie hostbestimmt. Liefert Studio One die Datenbytes vor
der Auswahl, ist die dritte Zeile der Normalfall statt eines Zufalls. Ob Studio One RPNs
überhaupt durchreicht, ist ungeprüft.

**Kleinster sinnvoller Korrekturvorschlag**

1. Adresse nach dem Store parken: in `buildStoreBatch` nach dem Store-Element ein
   weiteres anhängen, `{ PREENFM_EDITOR_NRPN_LAST, 0 }` (Seite 4, LSB 127). Seite 4 mit LSB ≥ 64
   ignoriert die Firmware ausdrücklich (`MidiDecoder.cpp:1193-1200`), ältere Firmware
   ignoriert Seite 4 ganz. Eine Zeile, schützt auch gegen Nachrichten über DIN.
2. Besitzregel im Worker: Data Entry (CC 6/38/96/97) aus dem Performance-Strom nur
   weiterreichen, wenn die letzte Adressauswahl (CC 98–101) auf diesem Kanal ebenfalls
   aus dem Performance-Strom kam, nicht vom Editor. Etwa 15 Zeilen in `sendOutputEvent`,
   einthreadig. Wer die Controller-RPN nicht verlieren will, wiederholt stattdessen die
   gemerkte Auswahl vor dem Datenbyte.

Zur Auftragsfrage „Können fremde RPN-Nachrichten in einen Store-Batch geraten?": In den
Batch nicht, die Zellen sind zusammenhängend reserviert (`Pfm2MidiDevice.cpp:446-527`).
Um ihn herum schon, und genau das ist der Fall der zweiten Tabellenzeile.

### P2-1 — Aufräumnachrichten gehen verloren, wenn die letzte Instanz geschlossen wird

*Bestätigt durch Lauf mit dem unveränderten Produktionscode.*

`releaseResources()` stellt Note-Offs nur in die Queue (`PluginProcessor.cpp:602-610`).
Der Destruktor des gemeinsamen Geräts ruft `stopThread(2000)` und schließt die Ports
(`Pfm2MidiDevice.cpp:94-98`). Der Worker verlässt seine Schleife beim Exit-Signal, ohne
die Queue zu leeren (`286-302`), und pollt nur alle 2 ms.

Messung (8 gehaltene Noten, je 200 Durchläufe):

```
stopThread 0 ms nach releaseResources(): 200 von 200 Durchläufen, 1600 von 1600 Note-Offs nicht gesendet
stopThread 1 ms nach releaseResources(): 193 von 200 Durchläufen, 1543 von 1600
stopThread 5 ms nach releaseResources():   0 von 200
```

**Auslöser:** Entfernen der letzten Instanz, Schließen des Projekts oder der
Standalone-Anwendung mit gehaltenen Noten oder Sustain. JUCE ruft `releaseResources()`
in `terminate()` unmittelbar vor der Zerstörung (`juce_audio_plugin_client_VST3.cpp:2632-2636`).
Mit weiteren lebenden Instanzen bleibt das Gerät bestehen und die Queue läuft leer.

**Korrektur:** In `~Pfm2MidiDevice()` nach `stopThread` und vor `resetDevices()` die
Queue synchron abarbeiten (`while (dequeueOutputEvent(e)) sendOutputEvent(e);`).

### P2-2 — „Transportende" ist nicht verdrahtet

*Bestätigt an der JUCE-Quelle und per Lauf.*

`releaseResources()` wird von JUCE bei `setActive(false)` und `terminate()` gerufen
(`juce_audio_plugin_client_VST3.cpp:2707-2713`, `2632-2636`), nicht beim Stoppen des
Transports. `CHANGELOG.md` („playback shutdown") und das Testlabel
`MpeProcessorTests.cpp:78` („transport shutdown") beschreiben etwas anderes als
implementiert ist. Beim Stopp hängt das Aufräumen allein an den Note-Offs des Hosts.

Dazu gehören zwei Lücken derselben Art:

- Ein Moduswechsel wirkt erst im nächsten `processBlock` (Lauf: nichts gesendet, bis
  `processBlock` oder `releaseResources()` läuft). Ruht die Audio-Engine oder ist die
  Instanz suspendiert, bleiben die Noten so lange stehen.
- Im Host-Bypass ruft JUCE `processBlockBypassed` (`:3731`). Das ist nicht überschrieben;
  gehaltene Noten bleiben hängen. Vorbestehend.

**Korrektur:** Aussagen in Changelog, Doku und Testlabel auf „Deaktivierung der Instanz"
ändern. Optional `processBlockBypassed` überschreiben und dort `release` rufen. Ein
automatisches Freigeben beim Transportstopp würde live gespielte Noten abschneiden und
ist deshalb keine Empfehlung.

### P2-3 — Steuerkanal und Antwortkanal passen nur in einer Belegung zusammen

*Bestätigt am Code und per Lauf; nicht an Hardware.*

Die Firmware leitet den Master-Kanal ausschließlich an das MPE-Timbre
(`MidiDecoder.cpp:261-263`), Member-Kanäle nehmen außer CC 74 keine Controller an
(`603-612`). Antworten und der Patch-Dump gehen auf dem konfigurierten Kanal des Timbres
hinaus (`1068-1076`, `966-970`). Der Editor wertet nur seinen eigenen Kanal aus
(`PluginProcessor.cpp:2033-2035`).

| Editorkanal | Befehle kommen an | Antworten kommen an |
|---|---|---|
| Master = Kanal des MPE-Timbres | ja | ja |
| Master, Timbre-Kanal anders | ja, beim MPE-Timbre | nein: „Protocol not detected", Pull leer |
| Timbre-Kanal, liegt als Member in der Zone | nein | — |
| Timbre-Kanal außerhalb der Zone | ja | ja, aber Arp-Schaden nach P1-1 |

Mit den Werkskanälen 1–4 und Zone 1 + 2..16 funktioniert also nur „MPE inst = 1". Der
Tooltip „use the manager/timbre channel" (`MainTabs.cpp:118-119`, `386-387`) liest sich,
als wären beide gleichwertig.

Verschärfend: Der Protokollstatus überlebt einen Kanalwechsel
(`PluginProcessor.cpp:1622-1629`, `1915-1921`). Im Lauf blieb „Store supported" nach dem
Wechsel von Kanal 1 auf 5 bestehen und der Store wurde auf Kanal 5 ohne neue Abfrage
angenommen. In der zweiten Tabellenzeile heißt das: Die Hardware schreibt, das Echo wird
verworfen, der Editor meldet nach 6 s „Store result UNKNOWN". Vorbestehend, durch MPE
leichter zu erreichen.

**Korrektur:** Tooltip und Doku: „Editorkanal = MPE-Master = MIDI-Kanal des
MPE-Instruments". Bei Kanalwechsel und beim Umschalten des MPE-Schalters den
Protokollstatus auf `unknown` setzen und neu abfragen.

### P2-4 — Host-Zustellung unter VST3 ist unbelegt; `supportsMPE()` bewirkt dort nichts

*Bestätigt an der JUCE-Quelle; Verhalten von Studio One ungeprüft.*

- `supportsMPE()` wird in JUCE 9.0.0 nur von AU, AUv3 und VST2 ausgewertet
  (`juce_audio_plugin_client_AU_1.mm:609`, `_AUv3.mm:416`, `_VST2.cpp:1820`). Der
  VST3-Wrapper fragt es nicht ab. Die Prüfung `MpeProcessorTests.cpp:62` belegt für die
  gebauten Formate nichts.
- Was `processBlock` unter VST3 erreicht: Note On/Off und Poly Pressure als Events mit
  Kanal (`juce_VST3Common.h:1513-1526`). Channel Pressure, Pitchbend und alle CCs nur
  dann, wenn der Host sie über `IMidiMapping` als Parameter des jeweiligen Kanals schickt
  (`juce_audio_plugin_client_VST3.cpp:1255-1265`, `3545-3560`). Note-Expression-Events
  werden verworfen (`juce_VST3Common.h:1534-1538`).
- Controller desselben Zeitpunkts erscheinen vor den Noten (Parameter werden vor den
  Events eingelesen, `:3591-3595`). Für MPE ist das günstig. Die Reihenfolge der CCs
  untereinander ist hostbestimmt, siehe P1-2.

Die Doku sagt das im Kern bereits (`docs/MPE_EDITOR_IMPLEMENTATION.md:63-67`). Die
Changelog-Zeile „Preserved … RPN bytes" gilt nur für das, was `processBlock` erreicht.
Externe Angaben zum Stand von Studio One widersprechen sich je nach Version (siehe
Quellen); maßgeblich ist der Test mit einem MIDI-Monitor am Ausgang des Editors.

### P2-5 — Push, Store und Session-Load sind im MPE-Modus 50 % länger

*Zahlen gemessen; Auswirkung an der Hardware plausibel, nicht geprüft.*

| | NRPNs | Bytes normal | Bytes MPE |
|---|---|---|---|
| Push / Session-Load | 247 | 2964 | 4446 |
| Store-Batch | 248 | 2976 | 4464 |

Die Firmware nimmt USB-MIDI in einen 200-Byte-Ring ohne Überlaufprüfung auf
(`PreenFM_irq.cpp:27`, `utils/RingBuffer.h:36-39`) und gibt den Endpunkt sofort wieder
frei (`usbd_midi_core.c:225-229`). Ob die Reserve bei 4.0.4 knapp oder groß war, weiß
ich nicht. Die Vorschläge aus P1-1 bringen die Länge fast auf den alten Wert zurück.
Im Hardwaretest: Push im MPE-Modus, danach Pull, Werte vergleichen.

### P3-1 — Verhalten bei voller Ausgabequeue

*Bestätigt per Lauf.* Vorbestehend gleichwertig: 4.0.4 verlor Nachrichten ebenfalls.

- Ein abgelehntes Note-Off wird nicht nachgeholt. `forward()` meldet `false`, der
  Aufrufer ignoriert es (`PluginProcessor.cpp:632`). Die Note bleibt im Router vermerkt
  und klingt, bis ein Moduswechsel oder `releaseResources()` sie freigibt.
- Schlägt die Freigabe bei einem Routingwechsel fehl, wird der gesamte Block verworfen
  (`PluginProcessor.cpp:629-634`), auch unbeteiligte Noten und Controller. Die Freigabe
  selbst wird im nächsten Block korrekt nachgeholt.

### P3-2 — Gerätegeneration

*Bestätigt per Lauf.* Wird im Dialog dasselbe Gerät erneut gewählt, steigt die Generation
und der Router vergisst gehaltene Noten und Sustain (`MidiPerformanceRouter.h:19-25`).
Es wird nichts an das neue Gerät gesendet, wie verlangt; die Noten am alten Gerät bleiben
aber stehen. Das ist die im Auftrag gewählte Seite der Abwägung und sollte als Grenze in
der Doku stehen.

Der Router ist nicht synchronisiert. `releaseResources()` und `processBlock` dürfen nach
VST3-Vertrag nicht gleichzeitig laufen; ein Host, der das verletzt, erzeugt ein Datenrennen
auf dem Notenfeld.

### P3-3 — PolyAT in der Zone und Stand der Firmware-Referenz

*Bestätigt am Code.*

- Poly Pressure auf einem Member-Kanal ignoriert die Firmware (`MidiDecoder.cpp:613-616`).
  Auf dem Master-Kanal wirkt sie über die Tonhöhe auf alle Stimmen dieser Note. Der
  Editor reicht PolyAT auf allen Kanälen unverändert durch; `MidiRoutingTests.cpp:39`
  und der Testplan Schritt 3 lesen sich, als sei PolyAT je Member-Note wirksam. Im
  Testplan sollte stehen: PolyAT auf dem Master-Kanal oder außerhalb der Zone, Press
  auf den Member-Kanälen über Channel Pressure.
- Die genannte Referenz `374be62` kennt im USB-Empfang kein Poly Pressure: In
  `usbd_midi_core.c:236-241` fehlt `case 0xa`. Die Korrektur ist `1107a62` auf
  `fix/usb-poly-pressure`, kein Vorfahr von `feature/full-mpe`. `src/midi` und `src/synth`
  sind zwischen beiden Ständen identisch, die Analyse oben gilt also für beide. Für den
  Test über USB braucht es den späteren Build.

### P3-4 — Lücken in den Tests

- Nicht geprüft werden die NRPN-Pfade Parameteränderung, Automation, Push, Pull, Position
  und Load. Ich habe sie mit einer eigenen Probe gegen den echten Prozessor geprüft: alle
  tragen im MPE-Modus das Präfix, im Normalmodus keiner (Abschnitt 5).
- Gerätewechsel, Kanalwechsel und Queue-Überlauf sind nur am Router mit Attrappe geprüft,
  nicht am Prozessor.
- `MpeProcessorTests.cpp:93-94` vergleicht Parameternamen vor und nach dem Restore im
  selben Build, nicht gegen die Basis. Gegen die Basis habe ich per Diff geprüft: Der
  Konstruktor (`PluginProcessor.cpp` bis Zeile 524), `MidifiedFloatParameter.*` und
  `PreenNrpn.h` sind unverändert, die erste Änderung liegt in `releaseResources` ab
  Zeile 604. Anzahl im aktuellen Stand: 242. Parameterliste und IDs sind damit unverändert; `MpeEnabled` ist
  nur ein XML-Attribut.
- `friend struct MidiRoutingTestAccess` steht in zwei Produktionsheadern.

### P3-5 — Kleinigkeiten

- `setMpeEnabled` meldet dem Host keine Zustandsänderung (`updateHostDisplay` mit
  `withNonParameterStateChanged`) und ruft `requestEditorStateUpdate()`, das den Schalter
  gar nicht aktualisiert (`PluginProcessor.cpp:612-616`).
- Der Routingmodus ist Teil des Plugin-Zustands. Ein Host-Preset aus 4.0.4 schaltet MPE
  aus, eines aus 4.0.5 gegebenenfalls ein. Das sollte dokumentiert sein.
- Die Sperre des Schalters (`MainTabs.cpp:263`) deckt Capability-Abfrage und Pull nicht
  ab. Folgenlos, weil das Flag beim Einreihen feststeht.
- `MidiPerformanceRouter.h` fehlt in `Plugin/pfm2Plugin.jucer`. UI-Texte nennen
  „firmware 3.00 alpha", die Firmware heißt inzwischen 3.10 alpha.
- Firmware-Builds mit `CVIN` verschieben die Matrix-ID von MPE-Slide von 19 auf 23
  (`Common.h:478-484`); der Editor zeigt dann den falschen Eintrag. Betrifft nur diese
  Builds.

## 4. Antworten auf die Review-Fragen

| Frage | Ergebnis |
|---|---|
| Zwei Noten, auch gleiche Tonhöhe auf zwei Member-Kanälen, unabhängig Press, Slide, Glide | Editor: ja, Bytes und Kanäle bleiben durch Prozessor und Queue erhalten (`MpeProcessorTests.cpp:64-71`, von mir ausgeführt). Firmware: eine Stimme je Member-Kanal, Channel Pressure, CC 74 und Pitchbend wirken je Stimme (`MidiDecoder.cpp:588-612`, `Timbre.cpp:5666-5689`). PolyAT nicht, siehe P3-3. |
| Note On/Off beim Umschalten, Velocity 0, Überlappung, Sustain, Überlauf, Retry | Router korrekt, Tests bestehen. Grenzen in P3-1. |
| Stop/Start, Reset/Restore, Schließen, Gerätewechsel, mehrere Instanzen | P2-1, P2-2, P3-2. Instanzen teilen Port, Queue und Transaktions-Token; Router, Modus und Protokollstatus sind je Instanz getrennt. Zwei Instanzen mit derselben Note auf demselben Kanal schneiden sich gegenseitig ab; das Präfix entscheidet jede Instanz für sich (P1-1, Spiegelfall). |
| clearRpn in jedem NRPN-Pfad | Ja, alle fünf Aufrufstellen (`PluginProcessor.cpp:733, 837, 1947, 2018` und der Batch `:1050`), im Gerät bis zur Ausgabe durchgereicht (`Pfm2MidiDevice.cpp:202, 243, 279, 521`). Per Lauf bestätigt. |
| Fremde RPN im Store-Batch, RPN vor Automation | P1-2. |
| Manager- und Timbre-Kanal verschieden | P2-3. |
| VST3, Studio One, `supportsMPE()` | P2-4. `supportsMPE()` reicht nicht und wirkt unter VST3 gar nicht. |
| Normales MIDI, PreenFM3, alte States | Im Normalmodus sind die weitergeleiteten Bytes identisch zu 4.0.4 (`queueMidiMessage` mit Kanal-Override wie zuvor); neu ist dort nur die Freigabe gehaltener Noten beim Kanalwechsel. PreenFM3 erhält nie ein Präfix. Fehlt `MpeEnabled`, bleibt der Normalmodus. Parameter unverändert (P3-4). |
| Echtzeitfähigkeit | Im Audio-Callback kein Heap, kein Lock, keine GUI. Schleifen begrenzt (16 × 128 Felder, je Feld höchstens so viele Durchläufe wie gezählte Note-Ons). `mpeEnabled`, `pfmType`, `currentMidiChannel` atomar. Automation reiht NRPNs lock-frei auf dem Audio-Thread ein. Einschränkung: P3-2, fehlende Synchronisation des Routers. |

## 5. Ausgeführte Befehle und Ergebnisse

**Vorhandene Suiten, von mir gebaut und ausgeführt (Linux, nicht das Windows-Preset).**
Quellen unverändert aus dem Arbeitsbaum, Hashes wie in Abschnitt 2.

```
# auf dem Rechner des Auftraggebers, Linux-Umgebung, g++ 11.4
g++ -std=c++17 -O2 -Wall -Wextra Plugin/Tests/MidiRoutingTests.cpp      ->  797 checks, 0 failures
g++ -std=c++17 -O2 -Wall -Wextra Plugin/Tests/EditorProtocolTests.cpp   ->   59 checks, 0 failures
g++ ... -fsanitize=address,undefined Plugin/Tests/MidiRoutingTests.cpp  ->  797 checks, 0 failures, keine Meldung

# Cloud-Arbeitsbereich, g++ 13.3, CMake 3.28, Ninja, JUCE 9.0.0 (f8f8864)
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DJUCE_SOURCE_DIR=<JUCE 9.0.0>
cmake --build build --target EditorProtocolTests MidiRoutingTests MpeProcessorTests
xvfb-run ctest --output-on-failure
  EditorProtocolTests   59 checks, 0 failures            Passed
  MidiRoutingTests      797 checks, 0 failures           Passed
  MpeProcessorTests     523 processor checks, 0 failures Passed
```

**Windows-Preset: nicht von mir ausgeführt.** Aus dieser Sitzung lässt sich auf dem
Rechner keine PowerShell- oder MSVC-Sitzung starten. Vorgefunden habe ich
`build/windows-vs2026-x64/Testing/Temporary/LastTest.log` vom 06.10., 20:01 MESZ mit
denselben drei Ergebnissen (59, 797, 523, alle bestanden). Die Testprogramme sind jünger
als ihre Quellen (`MpeProcessorTests.exe` 18:01:24 UTC, Quelle 18:00:16 UTC), das Protokoll
gehört also zum aktuellen Stand. Das VST3-Manifest in `out/4.0.5-dev` nennt 4.0.5.
`git diff --check` gegen `fork/master`: ohne Meldung.

**Eigene Proben, außerhalb des Repositories.** Sie liegen nur im Arbeitsbereich dieser
Sitzung und sind auf Wunsch lieferbar.

1. Prozessor-Probe, gegen den unveränderten Plugin-Code gelinkt, liest die echte
   Ausgabequeue. Kein MIDI-Port offen.

   ```
   host parameters: 242
   Editor-MPE-Schalter aus:  Automation 1 NRPN / 0 mit Präfix, UI 1/0, Push 247/0, Pull 1/0,
                             Position 1/0, Load 1/0, Store 248/0, Session-Restore 247/0
   Editor-MPE-Schalter an:   Automation 1/1, UI 1/1, Push 247/247, Pull 1/1,
                             Position 1/1, Load 1/1, Store 248/248, Session-Restore 247/247
   Ende des Store-Batches:   [B0 65 7F] [B0 64 7F] [B0 63 04] [B0 62 02] [B0 06 01] [B0 26 02]
   Store nach Kanalwechsel 1 -> 5 ohne neue Abfrage: supported=yes, angenommen=yes, Status B4
   Queue voll (2048), Routingwechsel offen: Block erzeugt nichts; nächster Block [81 3C 00]
   Moduswechsel mit gehaltener Note ohne processBlock: nichts; releaseResources(): [82 3E 00]
   ```
   Dazu die Messung aus P2-1.

2. Router-Probe mit dem unveränderten `MidiPerformanceRouter.h`, mit ASan und UBSan:
   abgelehntes Note-Off wird nicht wiederholt; Generationswechsel 7 → 8 sendet nichts;
   RPN 0 von Kanal 6 verlässt den Normalmodus als `[B0 65 00] [B0 64 00] [B0 06 30] [B0 26 00]`;
   nacktes CC 6/38 passiert im MPE-Modus unverändert; teilweise fehlgeschlagene Freigabe
   wird beim zweiten `configure` vollständig nachgeholt.

3. Modellrechnung der Firmware in Python. Das ist **meine Nachbildung** der in den
   Befunden zitierten Stellen aus `MidiDecoder.cpp`, nicht die Firmware. Sie prüft nur,
   dass die beschriebenen Folgen aus dem gelesenen Code folgen. 12 Szenarien, alle wie
   beschrieben, darunter: Zone an und Editorkanal = Master → NRPN kommt an, Arp unberührt;
   Hardware-MPE aus → ein Edit ergibt `clock=2, direction=11`, der Store schreibt genau
   das; RPN um den Store-Batch geteilt → Stores `(1,9)` und `(48,0)`; mit den beiden
   Vorschlägen (ein Präfix am Batchanfang, Parken auf Seite 4/LSB 127) genau ein Store
   und unveränderte Arp-Werte.

## 6. Nicht geprüft

- Keine Hardware. Kein Studio One, kein anderer Host. Kein Start der Standalone-Anwendung
  oder des VST3.
- Kein Build und kein Testlauf unter Windows/MSVC durch mich (siehe Abschnitt 5). Die
  Linux-Läufe belegen die Logik, nicht das Windows-Artefakt.
- Timing der echten MIDI-Ausgabe (WinMM, USB), Verhalten der Firmware unter der längeren
  Bytefolge (P2-5), tatsächliche Reihenfolge und Vollständigkeit der Host-Controller (P2-4).
- `Timbre.cpp` und `Common.h` der Firmware nur in den zitierten Abschnitten gelesen, nicht
  vollständig. Der Firmware-Bericht `MPE_IMPLEMENTATION_REPORT.md` nur am Anfang; meine
  Aussagen stützen sich auf den Quellcode.
- Die `AGENTS.md` im Playground liegt außerhalb der beiden freigegebenen Ordner und war
  für mich nicht lesbar. Ich habe mich an die Vorgaben des Auftrags gehalten.
- macOS, AU, Projucer-Builds und der Ordner `Host/`.

Wenn nichts Weiteres gefunden wurde, heißt das nicht „fehlerfrei": Die Aussagen reichen
so weit wie diese Liste es zulässt.

Zur Transparenz: Zwei `git status`-Aufrufe zu Beginn haben in beiden Repositories den
Stat-Cache des Index aufgefrischt (`.git/index`, 18:28 UTC). Inhalt, Staging und
Arbeitsbaum sind unverändert, `git diff --cached` ist in beiden leer. Danach lief Git
nur noch mit `GIT_OPTIONAL_LOCKS=0`.

## 7. Ergänzungen für den Hardware-/Studio-One-Test

Zusätzlich zum Plan in `docs/MPE_EDITOR_IMPLEMENTATION.md`:

1. MIDI-Monitor zwischen Editor und Hardware (oder Loopback-Port als Editor-Ausgang).
   Je Member-Kanal prüfen, ob Pitchbend, Channel Pressure und CC 74 ankommen, und in
   welcher Reihenfolge eine RPN 0 und eine RPN 6 des Controllers eintreffen (P2-4, P1-2).
2. Gegenprobe zu P1-1: MPE an der Hardware aus, Editor-Schalter an, einen Parameter
   bewegen. Erwartung nach diesem Bericht: `Clk = Ext`, `Dire = ShUD`. Danach korrekt
   konfigurieren und wiederholen: Arp unverändert.
3. Push im MPE-Modus, anschließend Pull, Werte vergleichen (P2-5).
4. Nach einem Store in den Wegwerf-Slot die Nachbarschaft kontrollieren: Preset 1 der
   Bänke, deren Nummer um eins über dem Bend-Bereich bzw. der Member-Anzahl des
   Controllers liegt (P1-2).
5. Letzte Instanz mit gehaltener Note und mit gehaltenem Sustain schließen; dasselbe mit
   der Standalone-Anwendung (P2-1). Transportstopp mit gehaltenen Sequenznoten, Bypass
   mit gehaltener Note (P2-2).
6. PolyAT auf dem Master-Kanal und außerhalb der Zone testen, nicht auf Member-Kanälen
   (P3-3).

## Quellen zu Studio One

- [Sound On Sound: Studio One — Working With MPE](https://www.soundonsound.com/techniques/studio-one-working-mpe) — nennt für den damaligen Stand MPE nur für VST2.
- [ROLI Support: MPE in Studio One](https://support.roli.com/en/support/solutions/articles/36000441309) — nennt VST3-Unterstützung ab Studio One 5.3 und die Option „Enable MPE".

Keine der beiden Quellen sagt, auf welchem Weg ein JUCE-VST3 ohne Note Expression die
Kanaldaten erhält.
