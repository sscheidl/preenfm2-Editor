# Theme-Review PreenFM+ 4.0.7 (gegen `v4.0.6-beta.1`)

Stand: Quellen vom 06.10.2026, 22:28 Uhr; `Plugin/` und `CMakeLists.txt` sind identisch mit Commit `5195653` (Tag `v4.0.7-beta.1`) und HEAD `6cca3e6`. Von mir: Quellcode nicht verändert, kein Push, kein Release.

**Ergebnis: kein P0 bis P2.** Die Aussagen 1 bis 3 halten. Fünf P3-Befunde; T1 betrifft nur Debug-Builds, T3 hat keine sichtbare Wirkung.

## Befunde

**T1 · P3 · Farbabfrage ohne Theme-LookAndFeel.** `PreenTheme.h:47`; Aufrufer `PanelEngine.cpp:401`, `:695`, `MainTabs.cpp:175`, `:526`.
`findColour(id, true)` landet bei elternlosen Komponenten im JUCE-Standard-LookAndFeel: im Konstruktor (`:401`), bei nicht sichtbaren Tabs (JUCE ruft `lookAndFeelChanged()` auch dort) und beim Schließen (`PluginEditor.cpp:46`). Debug: `jassertfalse` in `juce_LookAndFeel.cpp:94`, in ThemeTests 52-mal, in MpeProcessorTests 28-mal. Release: Rückgabe Schwarz. Beim Wechsel auf Arctic mit offenem Modulation-Tab bekommt IM1 `#55D6D9` statt `#338183`; JUCE korrigiert das beim Tabwechsel, sichtbar wird es nicht.
Fix: `return c.getLookAndFeel().isColourSpecified(id) ? c.findColour(id, true) : Colour();`. In einer Arbeitskopie geprüft: 0 Assertions, Tests grün.

**T2 · P3 · Kontrast in Arctic.** `PanelEngine.cpp:179`, `:197`.
IM1, IM2 und IM5 gemessen 4,2 bis 4,3:1 (ThemeTests verlangen für Text 4,5). Carrier-Rahmen und Ausgangsstrich `#5EDBE0` auf hellem Grund rechnerisch 1,6:1.
Fix: `darker(0.9f)`, gemessen dann mindestens 5,3:1. Für `carrierOutline` im hellen Theme `0xff17636a`.

**T3 · P3 · Wirkungslose Farbzuweisungen.** `PreenLookAndFeel.cpp:49–57`.
`setColourScheme()` in Zeile 63 überschreibt alle neun, in jedem Theme; fünf erhalten andere Werte als beabsichtigt. Es gilt stattdessen: Selektion in Akzent 40 % mit Primärtext (Presetname in Arctic 7,2:1), Häkchen in Textfarbe. Lesbar.
Fix: Zeilen 49–57 löschen (Aussehen bleibt) oder Zeilen 63–65 vor Zeile 40 ziehen.

**T4 · P3 · Dialoge folgen dem Theme nicht.** `MainTabs.cpp:272`, `Pfm2MidiDevice.cpp:295`.
Die Store-Rückfrage zeichnet mit dem JUCE-Standard (`#263238`, weißer Text), in Arctic also dunkel. Der MIDI-Dialog ebenso (nur am Code geprüft). Lesbar.
Fix: `AlertWindow::showAsync(MessageBoxOptions::makeOptionsOkCancel(…, nullptr).withParentComponent(getTopLevelComponent()), …)`. Geprüft: Dialog im Editor zentriert, Theme-Farben, Rückgabe 1/0 unverändert. Alternative: als Ausnahme dokumentieren.

**T5 · P3 · Lücken in ThemeTests.** `ThemeTests.cpp:90–91`, `:93–99`.
- Die Auswahl selbst (`MainTabs.cpp:130–142`) läuft nie; der Test setzt das Theme direkt.
- Popup-Farben, Presetname-Editor, IM-Labels und Tab-Hintergrund werden nicht geprüft; T1 und T3 fielen deshalb nicht auf.
- Kontrast nur für drei Palettenfarben, nicht für IM-Farben oder Hintergrund auf Akzent (Arctic 4,8:1, Hover 3,9:1). DAW-State und ein zweiter Prozessor fehlen.

Fix, je eine `check`-Zeile: `getTabBackgroundColour(i)`, IM-Label-Farbe in Arctic, `PopupMenu::*` und `TextEditor::highlightColourId` am LookAndFeel, `getStateInformation` vor und nach dem Wechsel, zweiter `Pfm2AudioProcessor`.

## Ohne Befund

Mit eigener Probe über die echte Auswahl geprüft: alle sechs Themes schalten um, Popup-Farben folgen; 0 MIDI-Events, 0 geänderte Parameter, Plugin-State bytegleich; zwei Instanzen bleiben unabhängig, eine neue Instanz übernimmt die letzte Wahl. Bei 900 px liegt die Auswahl bei x = 764 bis 888, die übrige Kopfzeile endet bei 662. Am Code gelesen: über T1 hinaus keine neuen Lifetime- oder Callback-Probleme.

## Tests

EditorProtocolTests 59, MidiRoutingTests 2.850, MpeProcessorTests 5.384, ThemeTests 1.797 Checks, jeweils 0 Fehler. Ausgeführt unter Linux (GCC 13, JUCE 9.0.0, Xvfb), Debug und Release, ohne MIDI-Geräte, kein Store. Debug mit GCC baut nur mit `-fpermissive` (`PluginProcessor.cpp:1495`, außerhalb des Auftrags).

**Nicht ausgeführt:** Windows-Build und Windows-Tests (das vorgefundene CTest-Protokoll von 22:32 Uhr zeigt dieselben Zahlen), Host und Studio One, geöffnete Popups und Dialoge am echten Bildschirm, andere DPI-Stufen, Speicherpfad unter Windows.
