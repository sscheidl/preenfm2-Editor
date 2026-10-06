# Eng begrenzter Review: PreenFM+ 4.0.7 Themes

Prüfe ausschließlich die Theme-Erweiterung gegenüber Tag `v4.0.6-beta.1`.
Keine allgemeinen Refactorings, keine Firmwareänderungen, kein Quellcode ändern.

Schwerpunktdateien: `Plugin/Source/UI/PreenTheme.h`, `PreenLookAndFeel.*`,
`MainTabs.*`, `PanelEngine.*`, die Farbänderungen in `PanelModulation.cpp`,
`PanelArpAndFilter.cpp`, `Enveloppe*.cpp`, `StepSequencer.cpp`, außerdem
`PluginEditor.cpp` und `Plugin/Tests/ThemeTests.cpp`.

Prüfe nur:
1. Alle sechs Themes schalten vollständig um, auch Arctic, Popup-Menüs,
   editierbarer Presetname und dynamische Kindkomponenten. Kontraste bleiben lesbar.
2. Speicherung ist lokal und unabhängig von Synth-Preset/DAW-State; zwei Instanzen
   teilen keine mutable LookAndFeel-Instanz. Kein MIDI oder Parameterwechsel beim Umschalten.
3. Auswahl passt bei Mindestgröße ohne Überlappung. Keine neuen Lifetime-/Callback-Probleme.
4. ThemeTests decken diese Aussagen sinnvoll ab; führe sie und die vorhandenen
   drei Regressionstests offline aus. Keine Hardware-MIDI-Ausgabe, keinen Store senden.

Ausgabe: `CLAUDE_THEME_REVIEW.md` im Projektstamm. Nur konkrete Befunde mit
Datei/Zeile, Schweregrad und kleinstem Fixvorschlag; sonst „keine Befunde“.
Nicht ausgeführte Tests klar nennen. Maximal eine Seite. Kein Git-Push/Release.
