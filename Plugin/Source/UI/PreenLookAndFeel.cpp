/*
* Copyright 2017 Xavier Hosxe
*
* Author: Xavier Hosxe (xavier <dot> hosxe
*                      (at) g m a i l <dot> com)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "PreenLookAndFeel.h"

preenfmLookAndFeel::preenfmLookAndFeel(const File& preferenceFile) : LookAndFeel_V4() {
	setUsingNativeAlertWindows(false);
	PropertiesFile::Options options;
	options.applicationName = "PreenFM+";
	options.filenameSuffix = "appearance";
	options.folderName = "tAUREON";
	options.osxLibrarySubFolder = "Application Support";
	options.millisecondsBeforeSaving = 0;
	appearanceSettings = preferenceFile == File{}
		? std::make_unique<PropertiesFile>(options)
		: std::make_unique<PropertiesFile>(preferenceFile, options);
	setTheme(appearanceSettings->getIntValue("theme", 0), false);
}

void preenfmLookAndFeel::setTheme(int index, bool persist)
{
	themeIndex = jlimit(0, PreenTheme::count - 1, index);
	palette = PreenTheme::palette(themeIndex);
	// Apply the JUCE scheme first: it also sets editor/toggle/alert colour IDs.
	setColourScheme(
		{ palette.background, palette.surface, palette.surfaceRaised, palette.outline, palette.textPrimary,
			palette.accent, palette.textPrimary, palette.accent, palette.textPrimary });
	using namespace PreenTheme;
	setColour(backgroundId, palette.background);
	setColour(surfaceId, palette.surface);
	setColour(raisedId, palette.surfaceRaised);
	setColour(outlineId, palette.outline);
	setColour(textId, palette.textPrimary);
	setColour(mutedId, palette.textMuted);
	setColour(accentId, palette.accent);
	setColour(brightId, palette.accentBright);
	setColour(warningId, palette.warning);
	setColour(TextEditor::backgroundColourId, palette.surfaceRaised);
	setColour(TextEditor::textColourId, palette.textPrimary);
	setColour(TextEditor::highlightColourId, palette.accent);
	setColour(TextEditor::highlightedTextColourId, palette.background);
	setColour(ToggleButton::textColourId, palette.textPrimary);
	setColour(ToggleButton::tickColourId, palette.accentBright);
	setColour(ToggleButton::tickDisabledColourId, palette.textMuted);
	setColour(AlertWindow::backgroundColourId, palette.surface);
	setColour(AlertWindow::textColourId, palette.textPrimary);
	if (persist && appearanceSettings != nullptr)
	{
		appearanceSettings->setValue("theme", themeIndex);
		appearanceSettings->saveIfNeeded();
	}
	setColour(Label::textColourId, palette.textPrimary);
	setColour(TextButton::buttonColourId, palette.surfaceRaised);
	setColour(TextButton::buttonOnColourId, palette.accent);
	setColour(TextButton::textColourOffId, palette.textPrimary);
	setColour(TextButton::textColourOnId, palette.background);
	setColour(ComboBox::backgroundColourId, palette.surfaceRaised);
	setColour(ComboBox::textColourId, palette.textPrimary);
	setColour(ComboBox::outlineColourId, palette.outline);
	setColour(ComboBox::arrowColourId, palette.accentBright);
	setColour(Slider::textBoxTextColourId, palette.textPrimary);
	setColour(Slider::textBoxBackgroundColourId, palette.surface);
	setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
	setColour(GroupComponent::outlineColourId, palette.outline);
	setColour(GroupComponent::textColourId, palette.textMuted);
	setColour(TabbedComponent::backgroundColourId, palette.background);
	setColour(TabbedComponent::outlineColourId, Colours::transparentBlack);
	setColour(TabbedButtonBar::tabTextColourId, palette.textMuted);
	setColour(TabbedButtonBar::frontTextColourId, palette.textPrimary);
	setColour(PopupMenu::backgroundColourId, palette.surfaceRaised);
	setColour(PopupMenu::textColourId, palette.textPrimary);
	setColour(PopupMenu::highlightedBackgroundColourId, palette.accent);
	setColour(PopupMenu::highlightedTextColourId, palette.background);
	setColour(HyperlinkButton::textColourId, palette.accentBright);
}

Font preenfmLookAndFeel::getTextButtonFont(TextButton&, int buttonHeight)
{
	return Font(FontOptions(jmin(15.0f, buttonHeight * 0.58f), Font::plain)
		.withMetricsKind(TypefaceMetricsKind::legacy));
}

int preenfmLookAndFeel::getTabButtonBestWidth(TabBarButton& button, int tabDepth)
{
	return LookAndFeel_V4::getTabButtonBestWidth(button, tabDepth) + 18;
}

void preenfmLookAndFeel::drawButtonBackground(Graphics& g, Button& button,
	const Colour& backgroundColour, bool isHighlighted, bool isDown)
{
	auto bounds = button.getLocalBounds().toFloat().reduced(0.75f);
	auto fill = backgroundColour.isTransparent() ? palette.surfaceRaised : backgroundColour;

	if (button.getToggleState())
		fill = palette.accent;
	if (isDown)
		fill = fill.darker(0.18f);
	else if (isHighlighted)
		fill = button.getToggleState() && palette.background.getBrightness() > 0.6f
			? fill.darker(0.10f) : fill.brighter(0.10f);

	g.setColour(fill.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.45f));
	g.fillRoundedRectangle(bounds, 5.0f);
	g.setColour((button.getToggleState() ? palette.accentBright : palette.outline)
		.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.4f));
	g.drawRoundedRectangle(bounds, 5.0f, button.getToggleState() ? 1.5f : 1.0f);
}

void preenfmLookAndFeel::drawGroupComponentOutline(Graphics& g, int width, int height,
	const String& label, const Justification&, GroupComponent& group)
{
	const auto bounds = Rectangle<float>(0.5f, 8.0f,
		static_cast<float>(width) - 1.0f, static_cast<float>(height) - 8.5f);
	const auto border = palette.outline.interpolatedWith(palette.accent, 0.12f);
	const auto tone = static_cast<unsigned int>(group.getText().hashCode()) % 4;
	g.setColour(palette.surface.interpolatedWith(palette.surfaceRaised, 0.12f + 0.13f * tone));
	g.fillRoundedRectangle(bounds, 8.0f);
	g.setColour(border);
	g.drawRoundedRectangle(bounds, 8.0f, 1.0f);

	if (label.isNotEmpty())
	{
		g.setFont(Font(FontOptions(13.0f, Font::bold)
			.withMetricsKind(TypefaceMetricsKind::legacy)));
		const auto textWidth = jmin(width - 24,
			GlyphArrangement::getStringWidthInt(g.getCurrentFont(), label) + 14);
		g.setColour(palette.background);
		g.fillRect(10, 0, textWidth, 18);
		g.setColour(palette.textMuted);
		g.drawText(label, 16, 0, textWidth - 8, 18, Justification::centredLeft, true);
	}
}

void preenfmLookAndFeel::drawTabButton(TabBarButton& button, Graphics& g,
	bool isMouseOver, bool isMouseDown)
{
	auto bounds = button.getLocalBounds().toFloat().reduced(5.0f, 4.0f);
	auto fill = palette.surface.interpolatedWith(palette.surfaceRaised,
		0.25f + 0.25f * jlimit(0, 2, button.getIndex()));
	if (button.isFrontTab())
		fill = fill.brighter(0.16f);
	if (isMouseDown)
		fill = fill.darker(0.15f);
	else if (isMouseOver)
		fill = fill.brighter(0.08f);

	g.setColour(fill);
	g.fillRoundedRectangle(bounds, 6.0f);
	g.setColour((button.isFrontTab() ? palette.accentBright : palette.outline).withAlpha(0.9f));
	g.drawRoundedRectangle(bounds, 6.0f, button.isFrontTab() ? 1.4f : 1.0f);

	drawTabButtonText(button, g, isMouseOver, isMouseDown);
}



void preenfmLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
	const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
	const auto track = palette.outline;
	const auto fill = slider.isEnabled() ? palette.accentBright : palette.textMuted.withAlpha(0.45f);

	const auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(5);

	auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
	const auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
	auto lineW = jmin(4.0f, radius * 0.5f);
	auto arcRadius = radius - lineW * 0.5f;

	Path backgroundArc;
	backgroundArc.addCentredArc(bounds.getCentreX(),
		bounds.getCentreY(),
		arcRadius,
		arcRadius,
		0.0f,
		rotaryStartAngle,
		rotaryEndAngle,
		true);

	g.setColour(track);
	g.strokePath(backgroundArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::butt));

	if (slider.isEnabled())
	{
		Path valueArc;
		valueArc.addCentredArc(bounds.getCentreX(),
			bounds.getCentreY(),
			arcRadius,
			arcRadius,
			0.0f,
			rotaryStartAngle,
			toAngle,
			true);

		g.setColour(fill);
		g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::butt));

        const juce::Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - float_Pi * 0.5f),
			bounds.getCentreY() + arcRadius * std::sin(toAngle - float_Pi * 0.5f));

		g.setColour(palette.surfaceRaised);
		g.fillEllipse(bounds.withSizeKeepingCentre(radius * 1.18f, radius * 1.18f));
		g.setColour(fill);
		g.drawLine(bounds.getCentreX(), bounds.getCentreY(), thumbPoint.getX(), thumbPoint.getY(), 2.0f);
	}
}


void preenfmLookAndFeel::drawLinearSlider(Graphics& g, int x, int y, int width, int height,
	float sliderPos,
	float minSliderPos,
	float maxSliderPos,
	const Slider::SliderStyle style, Slider& slider)
{
	ignoreUnused(minSliderPos, maxSliderPos, style);

	const auto track = palette.outline;
	const auto fill = slider.isEnabled() ? palette.accentBright : palette.textMuted.withAlpha(0.45f);


	const auto trackWidth = jmin(4.0f, slider.isHorizontal() ? height * 0.25f : width * 0.25f);

	const juce::Point<float> startPoint(slider.isHorizontal() ? x : x + width * 0.5f,
		slider.isHorizontal() ? y + height * 0.5f : height + y);

	const juce::Point<float> endPoint(slider.isHorizontal() ? width + x : startPoint.x,
		slider.isHorizontal() ? startPoint.y : y);

	Path backgroundTrack;
	backgroundTrack.startNewSubPath(startPoint);
	backgroundTrack.lineTo(endPoint);

	g.setColour(track);
	g.strokePath(backgroundTrack, PathStrokeType(trackWidth, PathStrokeType::curved, PathStrokeType::butt));

	Path valueTrack;

	const auto kx = slider.isHorizontal() ? sliderPos : (x + width * 0.5f);
	const auto ky = slider.isHorizontal() ? (y + height * 0.5f) : sliderPos;
	const juce::Point<float> maxPoint { kx, ky };
	juce::Point<float> valueOrigin = startPoint;

	if (slider.getMinimum() < 0.0 && slider.getMaximum() > 0.0) {
		const float zeroProportion = static_cast<float>(
			slider.valueToProportionOfLength(0.0));
		valueOrigin = startPoint + (endPoint - startPoint) * zeroProportion;
	}

	valueTrack.startNewSubPath(valueOrigin);
	valueTrack.lineTo(maxPoint);

	float knobWidth = trackWidth;
	float knobHeight = trackWidth;
	if (slider.isHorizontal()) {
		knobHeight *= 3.0f;
	}
	else {
		knobWidth *= 3.0f;

	}

	g.setColour(fill);
	g.strokePath(valueTrack, PathStrokeType(trackWidth, PathStrokeType::curved, PathStrokeType::butt));
	g.fillRoundedRectangle(Rectangle<float>(knobWidth, knobHeight).withCentre(maxPoint), 2.0f);
}


void preenfmLookAndFeel::drawComboBox(Graphics& g, int width, int height, bool,
	int, int, int, int, ComboBox& box)
{
	const auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 5.0f;
	const Rectangle<int> boxBounds(0, 0, width, height);

	g.setColour(box.findColour(ComboBox::backgroundColourId));
	g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

	g.setColour(box.findColour(ComboBox::outlineColourId));
	g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize,
		box.hasKeyboardFocus(true) ? 1.5f : 1.0f);

	Rectangle<int> arrowZone(width - 18, 0, 15, height);
	Path path;
	path.startNewSubPath(arrowZone.getX() + 3.0f, arrowZone.getCentreY() - 2.0f);
	path.lineTo(static_cast<float> (arrowZone.getCentreX()), arrowZone.getCentreY() + 3.0f);
	path.lineTo(arrowZone.getRight() - 3.0f, arrowZone.getCentreY() - 2.0f);

	g.setColour(box.findColour(ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
	g.strokePath(path, PathStrokeType(1.5f));
}


Font preenfmLookAndFeel::getComboBoxFont(ComboBox& box)
{
	return Font(FontOptions(jmin(14.0f, box.getHeight() * 0.85f))
		.withMetricsKind(TypefaceMetricsKind::legacy));
}


void preenfmLookAndFeel::positionComboBoxText(ComboBox& box, Label& label)
{
	label.setBounds(1, 1,
		box.getWidth() - 18,
		box.getHeight() - 2);

	label.setFont(getComboBoxFont(box));
}





