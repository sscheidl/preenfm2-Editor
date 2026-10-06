/* Copyright 2026 tAUREON. SPDX-License-Identifier: GPL-3.0-or-later */
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "UI/PreenLookAndFeel.h"
#include <iostream>
#include <vector>

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
    ScopedJuceInitialiser_GUI juceInit;
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
    std::unique_ptr<AudioProcessorEditor> editor(processor.createEditor());
    auto* themeCombo = find<ComboBox>(*editor, "Editor theme");
    auto* tabs = find<TabbedComponent>(*editor);
    check(themeCombo != nullptr && tabs != nullptr, "theme selector and tabs exist");
    if (themeCombo == nullptr || tabs == nullptr) return 1;
    check(themeCombo->getNumItems() == PreenTheme::count, "all six themes listed");
    std::vector<float> parameters;
    for (auto* parameter : processor.getParameters()) parameters.push_back(parameter->getValue());
    const bool mpe = processor.isMpeEnabled();
    MidiRoutingTestAccess::drain(*device);
    File screenshots;
    if (argc > 1)
    {
        screenshots = File(String::fromUTF8(argv[1]));
        check(screenshots.createDirectory().wasOk(), "screenshot directory available");
    }
    for (int theme = 0; theme < PreenTheme::count; ++theme)
    {
        look->setTheme(theme, false); // Tests never change the user's saved appearance.
        editor->sendLookAndFeelChange();
        check(themeCombo->getSelectedId() == theme + 1, "selector reflects active theme");
        const auto p = PreenTheme::palette(theme);
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
    editor.reset();
    MidiRoutingTestAccess::drain(*device);
    std::cout << checks << " theme checks, " << failures << " failures\n";
    return failures == 0 ? 0 : 1;
}
