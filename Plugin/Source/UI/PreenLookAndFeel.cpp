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

namespace
{
const Colour background(0xff0b1117);
const Colour surface(0xff111a24);
const Colour surfaceRaised(0xff182532);
const Colour outline(0xff314353);
const Colour textPrimary(0xffedf4f7);
const Colour textMuted(0xff9db0bd);
const Colour accent(0xff35c2c8);
const Colour accentBright(0xff5edbe0);
}

preenfmLookAndFeel::preenfmLookAndFeel() : LookAndFeel_V4() {
	setUsingNativeAlertWindows(false);
	setColourScheme(
		{ background, surface, surfaceRaised, outline, textPrimary,
			accent, textPrimary, accent, textPrimary });

	setColour(Label::textColourId, textPrimary);
	setColour(TextButton::buttonColourId, surfaceRaised);
	setColour(TextButton::buttonOnColourId, accent.darker(0.25f));
	setColour(TextButton::textColourOffId, textPrimary);
	setColour(TextButton::textColourOnId, Colours::white);
	setColour(ComboBox::backgroundColourId, surfaceRaised);
	setColour(ComboBox::textColourId, textPrimary);
	setColour(ComboBox::outlineColourId, outline);
	setColour(ComboBox::arrowColourId, accentBright);
	setColour(Slider::textBoxTextColourId, textPrimary);
	setColour(Slider::textBoxBackgroundColourId, surface);
	setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
	setColour(GroupComponent::outlineColourId, outline);
	setColour(GroupComponent::textColourId, textMuted);
	setColour(TabbedComponent::backgroundColourId, background);
	setColour(TabbedComponent::outlineColourId, Colours::transparentBlack);
	setColour(TabbedButtonBar::tabTextColourId, textMuted);
	setColour(TabbedButtonBar::frontTextColourId, textPrimary);
	setColour(PopupMenu::backgroundColourId, surfaceRaised);
	setColour(PopupMenu::textColourId, textPrimary);
	setColour(PopupMenu::highlightedBackgroundColourId, accent.darker(0.35f));
	setColour(PopupMenu::highlightedTextColourId, Colours::white);
	setColour(HyperlinkButton::textColourId, accentBright);
}

Font preenfmLookAndFeel::getTextButtonFont(TextButton&, int buttonHeight)
{
	return Font(jmin(15.0f, buttonHeight * 0.58f), Font::plain);
}

void preenfmLookAndFeel::drawButtonBackground(Graphics& g, Button& button,
	const Colour& backgroundColour, bool isHighlighted, bool isDown)
{
	auto bounds = button.getLocalBounds().toFloat().reduced(0.75f);
	auto fill = backgroundColour.isTransparent() ? surfaceRaised : backgroundColour;

	if (button.getToggleState())
		fill = accent.darker(0.35f);
	if (isDown)
		fill = fill.darker(0.18f);
	else if (isHighlighted)
		fill = fill.brighter(0.10f);

	g.setColour(fill.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.45f));
	g.fillRoundedRectangle(bounds, 5.0f);
	g.setColour((button.getToggleState() ? accentBright : outline)
		.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.4f));
	g.drawRoundedRectangle(bounds, 5.0f, button.getToggleState() ? 1.5f : 1.0f);
}

void preenfmLookAndFeel::drawGroupComponentOutline(Graphics& g, int width, int height,
	const String& label, const Justification&, GroupComponent& group)
{
	const auto bounds = Rectangle<float>(0.5f, 8.0f,
		static_cast<float>(width) - 1.0f, static_cast<float>(height) - 8.5f);
	g.setColour(surface.withAlpha(0.42f));
	g.fillRoundedRectangle(bounds, 8.0f);
	g.setColour(group.findColour(GroupComponent::outlineColourId));
	g.drawRoundedRectangle(bounds, 8.0f, 1.0f);

	if (label.isNotEmpty())
	{
		g.setFont(Font(13.0f, Font::bold));
		const auto textWidth = jmin(width - 24,
			GlyphArrangement::getStringWidthInt(g.getCurrentFont(), label) + 14);
		g.setColour(background);
		g.fillRect(10, 0, textWidth, 18);
		g.setColour(group.findColour(GroupComponent::textColourId));
		g.drawText(label, 16, 0, textWidth - 8, 18, Justification::centredLeft, true);
	}
}

void preenfmLookAndFeel::drawTabButton(TabBarButton& button, Graphics& g,
	bool isMouseOver, bool isMouseDown)
{
	auto bounds = button.getLocalBounds().toFloat().reduced(2.0f, 3.0f);
	auto fill = button.isFrontTab() ? accent.darker(0.55f) : surface;
	if (isMouseDown)
		fill = fill.darker(0.15f);
	else if (isMouseOver)
		fill = fill.brighter(0.08f);

	g.setColour(fill);
	g.fillRoundedRectangle(bounds, 6.0f);
	if (button.isFrontTab())
	{
		g.setColour(accentBright);
		g.fillRoundedRectangle(bounds.removeFromBottom(2.5f), 1.25f);
	}

	drawTabButtonText(button, g, isMouseOver, isMouseDown);
}



void preenfmLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
	const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
	const auto track = outline;
	const auto fill = slider.isEnabled() ? accentBright : textMuted.withAlpha(0.45f);

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

		g.setColour(surfaceRaised);
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

	const auto track = outline;
	const auto fill = slider.isEnabled() ? accentBright : textMuted.withAlpha(0.45f);


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
	juce::Point<float> minPoint, maxPoint;
	juce::Point<float> midPoint = (endPoint + startPoint) / 2;

	const auto kx = slider.isHorizontal() ? sliderPos : (x + width * 0.5f);
	const auto ky = slider.isHorizontal() ? (y + height * 0.5f) : sliderPos;

	minPoint = startPoint;
	maxPoint = { kx, ky };

	valueTrack.startNewSubPath(midPoint);
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
	return Font(jmin(14.0f, box.getHeight() * 0.85f));
}


void preenfmLookAndFeel::positionComboBoxText(ComboBox& box, Label& label)
{
	label.setBounds(1, 1,
		box.getWidth() - 18,
		box.getHeight() - 2);

	label.setFont(getComboBoxFont(box));
}





