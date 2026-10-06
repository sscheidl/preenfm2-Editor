/* Copyright 2026 tAUREON. SPDX-License-Identifier: GPL-3.0-or-later */
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "UI/PreenLookAndFeel.h"
#include <iostream>
#include <vector>

struct ThemeTestAccess
{
    static std::unique_ptr<PropertiesFile> swapPreferences(preenfmLookAndFeel& look,
        std::unique_ptr<PropertiesFile> replacement)
    {
        auto original = std::move(look.appearanceSettings);
        look.appearanceSettings = std::move(replacement);
        return original;
    }
};

struct MidiRoutingTestAccess
{
    static void offline(Pfm2MidiDevice& device) { device.stopThread(2000); device.resetDevices(); }
    static int drain(Pfm2MidiDevice& device)
    {
        int count = 0;
        device.drainOutputQueue([&](const Pfm2MidiDevice::OutputEvent&) { ++count; });
        return count;
    }
};

namespace
{
struct AssertionLog final : Logger
{
    Logger* previous = Logger::getCurrentLogger();
    std::atomic<int> assertions { 0 };
    AssertionLog() { Logger::setCurrentLogger(this); }
    ~AssertionLog() override { Logger::setCurrentLogger(previous); }
    void logMessage(const String& message) override
    {
        if (message.contains("JUCE Assertion failure"))
        {
            ++assertions;
            std::cerr << message << '\n';
        }
    }
};
struct ScopedThemePreferences
{
    preenfmLookAndFeel& look;
    File file;
    std::unique_ptr<PropertiesFile> original;
    explicit ScopedThemePreferences(preenfmLookAndFeel& l) : look(l),
        file(File::getCurrentWorkingDirectory().getNonexistentChildFile("theme-ui-test", ".appearance", false))
    {
        PropertiesFile::Options options;
        options.applicationName = "PreenFM+";
        options.millisecondsBeforeSaving = 0;
        original = ThemeTestAccess::swapPreferences(look, std::make_unique<PropertiesFile>(file, options));
    }
    ~ScopedThemePreferences()
    {
        auto temporary = ThemeTestAccess::swapPreferences(look, std::move(original));
        temporary.reset();
        file.deleteFile();
    }
};
int checks = 0, failures = 0;
void check(bool result, const char* message)
{
    ++checks;
    if (!result) { ++failures; std::cerr << "FAIL: " << message << '\n'; }
}
template<class T> T* find(Component& root, const String& name = {})
{
    if (auto* result = dynamic_cast<T*>(&root))
        if (name.isEmpty() || root.getName() == name) return result;
    for (auto* child : root.getChildren())
        if (auto* result = find<T>(*child, name)) return result;
    return nullptr;
}
double luminance(Colour c)
{
    const auto linear = [](double value)
    { return value <= 0.04045 ? value / 12.92 : std::pow((value + 0.055) / 1.055, 2.4); };
    return 0.2126 * linear(c.getFloatRed()) + 0.7152 * linear(c.getFloatGreen())
        + 0.0722 * linear(c.getFloatBlue());
}
double contrast(Colour a, Colour b)
{
    const auto x = luminance(a), y = luminance(b);
    return (std::max(x, y) + 0.05) / (std::min(x, y) + 0.05);
}
}

int main(int argc, char** argv)
{
    AssertionLog assertionLog;
    ScopedJuceInitialiser_GUI juceInit;
    Component orphan;
    check(PreenTheme::colour(orphan, PreenTheme::backgroundId).isTransparent(),
        "unattached components do not query unspecified JUCE colours");
    const auto preferences = File::getCurrentWorkingDirectory().getNonexistentChildFile(
        "theme-test-preferences", ".appearance", false);
    {
        preenfmLookAndFeel first(preferences);
        first.setTheme(2);
        preenfmLookAndFeel second(preferences);
        check(second.getTheme() == 2, "new instance restores saved local theme");
        first.setTheme(5);
        check(second.getTheme() == 2, "already open instances keep independent palettes");
        preenfmLookAndFeel reopened(preferences);
        check(reopened.getTheme() == 5, "reopening uses last saved theme");
    }
    check(preferences.deleteFile(), "isolated test preferences removed");
    SharedResourcePointer<Pfm2MidiDevice> device;
    MidiRoutingTestAccess::offline(*device);
    Pfm2AudioProcessor processor;
    auto* look = dynamic_cast<preenfmLookAndFeel*>(processor.getEditorLookAndFeel());
    check(look != nullptr, "per-instance LookAndFeel exists");
    if (look == nullptr) return 1;
    const int originalTheme = look->getTheme();
    ScopedThemePreferences isolatedPreferences(*look);
    std::unique_ptr<AudioProcessorEditor> editor(processor.createEditor());
    // JUCE's actual Label editor requires a showing peer for keyboard focus.
    // Keep a temporary, non-interactive peer off screen; do not suppress its assertions.
    editor->setTopLeftPosition(-20000, -20000);
    editor->addToDesktop(ComponentPeer::windowIsTemporary | ComponentPeer::windowIgnoresKeyPresses
        | ComponentPeer::windowIgnoresMouseClicks);
    editor->setVisible(true);
    auto* themeCombo = find<ComboBox>(*editor, "Editor theme");
    auto* tabs = find<TabbedComponent>(*editor);
    check(themeCombo != nullptr && tabs != nullptr, "theme selector and tabs exist");
    if (themeCombo == nullptr || tabs == nullptr) return 1;
    check(themeCombo->getNumItems() == PreenTheme::count, "all six themes listed");
    Pfm2AudioProcessor secondProcessor;
    auto* secondLook = dynamic_cast<preenfmLookAndFeel*>(secondProcessor.getEditorLookAndFeel());
    check(secondLook != nullptr && secondLook != look, "two processors have separate LookAndFeel instances");
    if (secondLook == nullptr) return 1;
    const int secondTheme = secondLook->getTheme();
    std::unique_ptr<AudioProcessorEditor> secondEditor(secondProcessor.createEditor());
    auto* secondCombo = find<ComboBox>(*secondEditor, "Editor theme");
    check(secondCombo != nullptr, "second editor has its own selector");
    MemoryBlock secondState;
    secondProcessor.getStateInformation(secondState);
    std::vector<float> parameters;
    for (auto* parameter : processor.getParameters()) parameters.push_back(parameter->getValue());
    const bool mpe = processor.isMpeEnabled();
    MidiRoutingTestAccess::drain(*device);
    // Force a different initial selection so every subsequent switch invokes
    // the real UI callback. Only the isolated preference file is written.
    themeCombo->setSelectedId(PreenTheme::count, sendNotificationSync);
    File screenshots;
    if (argc > 1)
    {
        screenshots = File(String::fromUTF8(argv[1]));
        check(screenshots.createDirectory().wasOk(), "screenshot directory available");
    }
    for (int theme = 0; theme < PreenTheme::count; ++theme)
    {
        MemoryBlock beforeSwitch, afterSwitch;
        processor.getStateInformation(beforeSwitch);
        themeCombo->setSelectedId(theme + 1, sendNotificationSync);
        processor.getStateInformation(afterSwitch);
        check(look->getTheme() == theme, "real selector callback changes palette");
        check(beforeSwitch == afterSwitch, "real theme selection leaves DAW state byte-identical");
        check(secondLook->getTheme() == secondTheme, "second processor palette stays independent");
        check(secondCombo != nullptr && secondCombo->getSelectedId() == secondTheme + 1,
            "second editor selector stays independent");
        MemoryBlock secondAfter;
        secondProcessor.getStateInformation(secondAfter);
        check(secondAfter == secondState, "second processor state stays byte-identical");
        check(themeCombo->getSelectedId() == theme + 1, "selector reflects active theme");
        const auto p = PreenTheme::palette(theme);
        check(look->findColour(PopupMenu::backgroundColourId) == p.surfaceRaised, "popup uses selected surface");
        check(look->findColour(PopupMenu::highlightedBackgroundColourId) == p.accent, "popup highlight uses accent");
        check(look->findColour(PopupMenu::highlightedTextColourId) == p.background, "popup highlight text uses background");
        check(look->findColour(TextEditor::highlightColourId) == p.accent, "preset name selection uses accent");
        check(look->findColour(TextEditor::highlightedTextColourId) == p.background, "selected preset name uses intended text");
        check(look->findColour(ToggleButton::tickColourId) == p.accentBright, "tick override survives scheme update");
        check(look->findColour(AlertWindow::backgroundColourId) == p.surface, "alert override survives scheme update");
        {
            AlertWindow confirmation("Theme test", "Offline: no hardware operation", MessageBoxIconType::WarningIcon);
            editor->addChildComponent(&confirmation);
            check(&confirmation.getLookAndFeel() == look
                && confirmation.findColour(AlertWindow::backgroundColourId) == p.surface,
                "embedded confirmation inherits the editor theme");
            editor->removeChildComponent(&confirmation);
            preenfmLookAndFeel dialogLook(isolatedPreferences.file);
            dialogLook.setTheme(theme, false);
            AlertWindow midiDialog("MIDI theme test", "Offline", MessageBoxIconType::QuestionIcon);
            midiDialog.setLookAndFeel(&dialogLook);
            midiDialog.addComboBox("Input", { "None" }, "Input");
            check(midiDialog.findColour(AlertWindow::backgroundColourId) == p.surface
                && midiDialog.getComboBoxComponent("Input")->findColour(ComboBox::backgroundColourId) == p.surfaceRaised,
                "dialog-owned palette themes the MIDI window and controls");
        }
        check(contrast(p.background, p.accent) >= 4.5, "selected text on accent meets contrast");
        const auto hoverAccent = p.background.getBrightness() > 0.6f ? p.accent.darker(0.10f) : p.accent.brighter(0.10f);
        check(contrast(p.background, hoverAccent) >= 4.5, "selected-button hover meets text contrast");
        auto* presetName = find<Label>(*editor, "preset name label");
        check(presetName != nullptr, "editable preset name exists");
        if (presetName != nullptr)
        {
            presetName->showEditor();
            auto* textEditor = presetName->getCurrentTextEditor();
            check(textEditor != nullptr && textEditor->findColour(TextEditor::textColourId) == p.textPrimary,
                "actual preset name editor inherits theme");
            presetName->hideEditor(true); // Discard; never rename/send NRPN in a GUI test.
        }
        for (const auto bg : { p.background, p.surface, p.surfaceRaised })
        {
            check(contrast(p.textPrimary, bg) >= 4.5, "primary text has sufficient contrast");
            check(contrast(p.textMuted, bg) >= 4.5, "secondary text has sufficient contrast");
            check(contrast(p.warning, bg) >= 4.5, "operation status is readable");
        }
        for (int size = 0; size < 2; ++size)
        {
            editor->setSize(size == 0 ? Pfm2AudioProcessorEditor::minimumWidth : 1500,
                size == 0 ? Pfm2AudioProcessorEditor::minimumHeight : 950);
            check(themeCombo->getParentComponent()->getLocalBounds().contains(themeCombo->getBounds()),
                "selector fits header at both sizes");
            for (auto* sibling : themeCombo->getParentComponent()->getChildren())
                if (sibling != themeCombo && sibling->isVisible())
                    check(!sibling->getBounds().intersects(themeCombo->getBounds()), "selector does not overlap controls");
            for (int tab = 0; tab < tabs->getNumTabs(); ++tab)
            {
                tabs->setCurrentTabIndex(tab);
                check(tabs->getTabBackgroundColour(tab) == p.background, "tab background tracks theme");
                if (tab == 0)
                {
                    for (int im = 1; im <= 6; ++im)
                    {
                        auto* label = find<Label>(*editor, "IM Label" + String(im));
                        check(label != nullptr, "IM label exists");
                        if (label != nullptr)
                            check(contrast(label->findColour(Label::textColourId), p.surface) >= 4.5,
                                "actual IM label meets contrast, including Arctic");
                    }
                    check(contrast(PreenTheme::carrierOutline(*tabs->getTabContentComponent(tab)), p.surface) >= 3.0,
                        "carrier outline/output visible on selected background");
                }
                const auto image = editor->createComponentSnapshot(editor->getLocalBounds());
                check(image.isValid(), "every theme/tab renders at both sizes");
                check(tabs->getTabContentComponent(tab)->findColour(PreenTheme::backgroundId, true) == p.background,
                    "tab inherits active palette");
                if (size == 1 && argc > 1)
                {
                    auto file = screenshots.getChildFile("theme-" + String(theme) + "-tab-" + String(tab) + ".png");
                    auto stream = file.createOutputStream();
                    if (stream != nullptr) { stream->setPosition(0); stream->truncate(); }
                    check(stream != nullptr && PNGImageFormat().writeImageToStream(image, *stream), "GUI screenshot exported");
                }
            }
        }
        check(MidiRoutingTestAccess::drain(*device) == 0, "appearance changes emit no MIDI");
        size_t index = 0;
        for (auto* parameter : processor.getParameters())
            check(parameter->getValue() == parameters[index++], "theme leaves synth parameters unchanged");
        check(processor.isMpeEnabled() == mpe, "theme leaves performance routing unchanged");
    }
    look->setTheme(-1, false); check(look->getTheme() == 0, "negative theme safely clamped");
    look->setTheme(999, false); check(look->getTheme() == PreenTheme::count - 1, "large theme safely clamped");
    look->setTheme(originalTheme, false);
    secondEditor.reset();
    editor.reset();
    MidiRoutingTestAccess::drain(*device);
    check(assertionLog.assertions.load() == 0, "no JUCE assertions during construction, switching or editor teardown");
    std::cout << checks << " theme checks, " << failures << " failures\n";
    return failures == 0 ? 0 : 1;
}
