/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.7

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...

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

#include "JuceHeader.h"
#include "SliderPfm2.h"
//[/Headers]

#include "PanelEngine.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
static constexpr AlgoInformation algoInformation[] = {
		{ 3, 3, 1}, // ALGO1
		{ 3, 2, 2}, // ALGO2
		{ 4, 4, 1}, // ALGO3
		{ 4, 4, 2}, // ALGO4
		{ 4, 4, 1}, // ALGO5
		{ 4, 3, 3}, // ALGO6
		{ 6, 4, 3},  // ALGO7
		{ 6, 4, 2},  // ALGO8
		{ 6, 4, 2},  // ALGO9
		{ 6, 4, 2},   // ALG10
		{ 6, 4, 2},   // ALG11
		{ 6, 3, 3},   // ALG12
		{ 6, 4, 2},   // ALG13
		{ 6, 4, 2},   // ALG14
		{ 6, 4, 2},   // ALG15
		{ 6, 4, 2},   // ALG16
		{ 6, 5, 1},   // ALG17
		{ 6, 5, 1},   // ALG18
		{ 6, 4, 3},   // ALG19
		{ 6, 4, 3},   // ALG20
		{ 6, 4, 4},   // ALG21
		{ 6, 4, 4},   // ALG22
		{ 6, 3, 5},   // ALG23
		{ 6, 3, 3},   // ALG24
		{ 6, 2, 4},   // ALG25
		{ 6, 2, 4},   // ALG26
		{ 6, 0, 6},   // ALG27
		{ 6, 1, 5},   // ALG28
		{ 4, 3, 2},   // ALG29
		{ 4, 4, 2},   // ALG30
		{ 4, 3, 3},   // ALG31
		{ 4, 4, 1},   // ALG32
};

static constexpr int algoOpInformation[][NUMBER_OF_OPERATORS] = {
		{1,2,2,0,0,0}, // ALGO1
		{1,1,2,0,0,0}, // ALGO2
		{1,2,2,2,0,0}, // ALGO3
		{1,1,2,2,0,0}, // ALGO4
		{1,2,2,2,0,0}, // ALGO5
		{1,1,1,2,0,0}, // ALGO6
		{1,2,1,2,1,2}, // ALGO7
		{1,2,2,2,1,2}, // ALGO8
		{1,2,2,1,2,2}, // ALGO9
		{1,2,1,2,2,2}, // ALGO10
		{1,2,2,1,2,2}, // ALGO11
		{1,2,1,2,1,2}, // ALGO12
		{1,2,1,2,2,2}, // ALGO13
		{1,2,2,1,2,2}, // ALGO14
		{1,2,1,2,2,2}, // ALGO15
		{1,2,1,2,2,2}, // ALGO16
		{1,2,2,2,2,2}, // ALGO17
		{1,2,2,2,2,2}, // ALGO18
		{1,2,2,1,1,2}, // ALGO19
		{1,1,2,1,2,2}, // ALGO20
		{1,1,2,1,1,2}, // ALGO21
		{1,2,1,1,1,2}, // ALGO22
		{1,1,1,1,1,2}, // ALGO23
		{1,2,1,2,2,1}, // ALGO24
		{1,1,1,2,1,2}, // ALGO25
		{1,1,1,2,2,1}, // ALGO26
		{1,1,1,1,1,1}, // ALGO27
		{1,1,1,1,1,2}, // ALGO28
		{1,1,2,2,0,0}, // ALGO29
		{1,1,2,2,0,0}, // ALGO30
		{1,1,1,2,0,0}, // ALGO31
		{1,2,2,2,0,0}, // ALGO32
};

namespace
{
struct UnifiedAlgorithmEdge
{
	int index;
	int source;
	int destination;
	bool sync;
};

struct UnifiedAlgorithm
{
	int nodeCount;
	std::vector<UnifiedAlgorithmEdge> edges;
};

const UnifiedAlgorithm unifiedAlgorithms[] = {
	{ 3, {{1,2,1,false}, {2,3,1,false}, {3,3,2,false}, {6,3,3,false}} },
	{ 3, {{1,3,1,false}, {2,3,2,false}, {6,3,3,false}} },
	{ 4, {{1,2,1,false}, {2,3,1,false}, {3,4,1,false}, {4,3,4,false}, {6,3,3,false}} },
	{ 4, {{1,3,1,false}, {2,4,2,false}, {3,3,2,false}, {4,4,3,false}, {6,4,4,false}} },
	{ 4, {{1,2,1,false}, {2,3,2,false}, {3,4,3,false}, {4,4,2,false}, {6,4,4,false}} },
	{ 4, {{1,4,1,false}, {2,4,2,false}, {3,4,3,false}, {6,4,4,false}} },
	{ 6, {{1,2,1,false}, {2,4,3,false}, {3,6,5,false}, {4,4,6,false}, {6,4,4,false}} },
	{ 6, {{1,2,1,false}, {2,3,1,false}, {3,4,1,false}, {4,6,5,false}, {6,4,4,false}} },
	{ 6, {{1,2,1,false}, {2,3,1,false}, {3,5,4,false}, {4,6,5,false}, {6,6,6,false}} },
	{ 6, {{1,2,1,false}, {2,4,3,false}, {3,5,4,false}, {4,6,5,false}, {6,2,2,false}} },
	{ 6, {{1,2,1,false}, {2,3,2,false}, {3,5,4,false}, {4,6,5,false}, {6,6,6,false}} },
	{ 6, {{1,2,1,false}, {2,4,3,false}, {3,6,5,false}, {6,6,6,false}} },
	{ 6, {{1,2,1,false}, {2,4,3,false}, {3,5,3,false}, {4,6,5,false}, {6,4,4,false}} },
	{ 6, {{1,2,1,false}, {2,3,2,false}, {3,5,4,false}, {4,6,4,false}, {6,6,6,false}} },
	{ 6, {{1,2,1,false}, {2,4,3,false}, {3,5,3,false}, {4,6,3,false}, {6,6,6,false}} },
	{ 6, {{1,2,1,false}, {2,4,3,false}, {3,5,4,false}, {4,6,4,false}, {6,2,2,false}} },
	{ 6, {{1,2,1,false}, {2,3,1,false}, {3,4,3,false}, {4,5,1,false}, {5,6,5,false}, {6,2,2,false}} },
	{ 6, {{1,2,1,false}, {2,3,1,false}, {3,4,1,false}, {4,5,4,false}, {5,6,5,false}, {6,3,3,false}} },
	{ 6, {{1,2,1,false}, {2,3,2,false}, {3,6,4,false}, {4,6,5,false}, {6,6,6,false}} },
	{ 6, {{1,3,1,false}, {2,3,2,false}, {3,5,4,false}, {4,6,4,false}, {6,3,3,false}} },
	{ 6, {{1,3,1,false}, {2,3,2,false}, {3,6,4,false}, {4,6,5,false}, {6,3,3,false}} },
	{ 6, {{1,2,1,false}, {2,6,3,false}, {3,6,4,false}, {4,6,5,false}, {6,6,6,false}} },
	{ 6, {{1,6,3,false}, {2,6,4,false}, {3,6,5,false}, {6,6,6,false}} },
	{ 6, {{1,2,1,false}, {2,4,3,false}, {3,5,4,false}, {6,5,5,false}} },
	{ 6, {{1,4,3,false}, {2,6,5,false}, {6,6,6,false}} },
	{ 6, {{1,4,3,false}, {2,5,4,false}, {6,5,5,false}} },
	{ 6, {{6,6,6,false}} },
	{ 6, {{1,6,5,false}, {6,6,6,false}} },
	{ 4, {{1,3,1,true}, {2,4,2,false}, {3,4,3,false}, {6,4,4,false}} },
	{ 4, {{1,3,1,true}, {2,3,2,true}, {3,4,1,false}, {4,4,2,false}, {6,4,4,false}} },
	{ 4, {{1,4,1,true}, {2,4,2,true}, {3,4,3,true}} },
	{ 4, {{1,2,1,false}, {2,3,1,false}, {3,4,1,true}, {4,3,4,false}, {6,3,3,false}} }
};

const Colour modulationConnectionColours[] = {
	Colour(0xff55d6d9), Colour(0xffffb55a), Colour(0xffb783ff),
	Colour(0xffff7aa8), Colour(0xff8fd46a), Colour(0xff67a7ff)
};

class UnifiedAlgorithmDiagram final : public Component
{
public:
	void setAlgorithm(int newAlgorithm)
	{
		algorithm = jlimit(0, NUMBER_OF_ALGO - 1, newAlgorithm);
		repaint();
	}

	void paint(Graphics& g) override
	{
		const auto& diagram = unifiedAlgorithms[algorithm];
		auto area = getLocalBounds().toFloat().reduced(12.0f, 8.0f);
		const bool isVosim = algorithm >= 28;
		const Colour carrierFill(0xff17636a);
		const Colour carrierOutline(0xff5edbe0);
		const Colour modulatorFill(0xff463064);
		const Colour modulatorOutline(0xffb482ff);
		const Colour syncConnection(0xffc29aff);
		const Colour textColour(0xffedf4f7);

		if (isVosim)
		{
			g.setColour(syncConnection);
			g.setFont(Font(FontOptions(13.0f, Font::bold)
				.withMetricsKind(TypefaceMetricsKind::legacy)));
			g.drawText("VOSIM", area.removeFromTop(20.0f).toNearestInt(),
				Justification::centred, false);
		}

		std::array<int, NUMBER_OF_OPERATORS> levels{};
		for (int pass = 0; pass < diagram.nodeCount; ++pass)
			for (const auto& edge : diagram.edges)
				if (edge.source != edge.destination)
					levels[edge.source - 1] = jmax(levels[edge.source - 1],
						levels[edge.destination - 1] + 1);

		const auto maxLevel = *std::max_element(levels.begin(), levels.begin() + diagram.nodeCount);
		const bool topLevelHasFeedback = std::any_of(diagram.edges.begin(), diagram.edges.end(),
			[&](const UnifiedAlgorithmEdge& edge)
			{
				return edge.source == edge.destination && levels[edge.source - 1] == maxLevel;
			});
		int maxNodesOnLevel = 1;
		for (int level = 0; level <= maxLevel; ++level)
		{
			int nodesOnLevel = 0;
			for (int op = 0; op < diagram.nodeCount; ++op)
				if (levels[op] == level)
					++nodesOnLevel;
			maxNodesOnLevel = jmax(maxNodesOnLevel, nodesOnLevel);
		}
		const float nodeSize = jlimit(21.0f, 30.0f,
			jmin(area.getWidth() / (static_cast<float>(maxNodesOnLevel) + 2.2f),
				area.getHeight() / (static_cast<float>(maxLevel) + 2.8f)));
		const float outputSpace = 12.0f;
		const float top = area.getY() + nodeSize * (topLevelHasFeedback ? 1.45f : 0.6f);
		const float bottom = area.getBottom() - nodeSize * 0.5f - outputSpace;
		const float levelStep = maxLevel > 0 ? (bottom - top) / static_cast<float>(maxLevel) : 0.0f;
		std::array<Point<float>, NUMBER_OF_OPERATORS> centres{};

		for (int level = 0; level <= maxLevel; ++level)
		{
			std::array<int, NUMBER_OF_OPERATORS> operators{};
			int operatorCount = 0;
			for (int op = 0; op < diagram.nodeCount; ++op)
				if (levels[op] == level)
					operators[operatorCount++] = op;

			for (int i = 0; i < operatorCount; ++i)
			{
				const float x = area.getX() + area.getWidth()
					* static_cast<float>(i + 1) / static_cast<float>(operatorCount + 1);
				centres[operators[i]] = { x, bottom - level * levelStep };
			}
		}

		const auto pointOnNodeBoundary = [nodeSize](Point<float> centre, Point<float> towards)
		{
			const auto delta = towards - centre;
			const float xScale = std::abs(delta.x) > 0.001f
				? nodeSize * 0.5f / std::abs(delta.x) : std::numeric_limits<float>::max();
			const float yScale = std::abs(delta.y) > 0.001f
				? nodeSize * 0.5f / std::abs(delta.y) : std::numeric_limits<float>::max();
			return centre + delta * jmin(xScale, yScale);
		};

		for (const auto& edge : diagram.edges)
		{
			const auto source = centres[edge.source - 1];
			const auto destination = centres[edge.destination - 1];
			const auto connectionColour = modulationConnectionColours[
				jlimit(0, 5, edge.index - 1)];
			g.setColour(connectionColour);

			if (edge.source == edge.destination)
			{
				const float direction = area.getRight() - source.x >= source.x - area.getX()
					? 1.0f : -1.0f;
				const float sideX = source.x + direction * nodeSize * 0.5f;
				const float laneX = jlimit(area.getX() + 4.0f, area.getRight() - 4.0f,
					source.x + direction * nodeSize * 0.85f);
				const float loopTop = source.y - nodeSize * 1.05f;
				const float nodeTop = source.y - nodeSize * 0.5f;
				Path feedback;
				feedback.startNewSubPath(sideX, source.y);
				feedback.lineTo(laneX, source.y);
				feedback.lineTo(laneX, loopTop);
				feedback.lineTo(source.x, loopTop);
				g.strokePath(feedback, PathStrokeType(1.8f, PathStrokeType::mitered,
					PathStrokeType::butt));
				g.drawArrow(Line<float>(source.x, loopTop, source.x, nodeTop),
					1.8f, 6.0f, 5.0f);
			}
			else
			{
				const auto start = pointOnNodeBoundary(source, destination);
				const auto finish = pointOnNodeBoundary(destination, source);
				const Line<float> connection(start, finish);
				g.drawArrow(connection, 1.8f, 6.0f, 5.0f);

				if (edge.sync)
				{
					Path connectionPath;
					connectionPath.startNewSubPath(start);
					connectionPath.lineTo(finish);
					Path dashedSyncPath;
					const float dashLengths[] = { 3.5f, 2.5f };
					PathStrokeType(1.15f, PathStrokeType::curved,
						PathStrokeType::butt).createDashedStroke(dashedSyncPath,
							connectionPath, dashLengths, 2);
					g.setColour(syncConnection);
					g.fillPath(dashedSyncPath);
				}
			}
		}

		for (int op = 0; op < diagram.nodeCount; ++op)
		{
			const bool isCarrier = algoOpInformation[algorithm][op] == 1;
			const auto centre = centres[op];
			const Rectangle<float> bounds(centre.x - nodeSize * 0.5f,
				centre.y - nodeSize * 0.5f, nodeSize, nodeSize);
			g.setColour(isCarrier ? carrierFill : modulatorFill);
			g.fillRoundedRectangle(bounds, 4.0f);
			g.setColour(isCarrier ? carrierOutline : modulatorOutline);
			g.drawRoundedRectangle(bounds, 4.0f, 1.8f);
			g.setColour(textColour);
			g.setFont(Font(FontOptions(
				jlimit(12.0f, 14.0f, nodeSize * 0.48f), Font::bold)
				.withMetricsKind(TypefaceMetricsKind::legacy)));
			g.drawText(String(op + 1), bounds.toNearestInt(), Justification::centred, false);

			if (isCarrier)
			{
				const float outputTop = bounds.getBottom();
				g.setColour(carrierOutline);
				g.drawLine(centre.x, outputTop + 1.0f, centre.x, outputTop + 9.0f, 1.8f);
			}
		}
	}

private:
	int algorithm = 0;
};
}
//[/MiscUserDefs]

//==============================================================================
PanelEngine::PanelEngine ()
{
    //[Constructor_pre] You can add your own custom stuff here..
	envToCopy = -1;
	envSelected = 0;
	pfmType = TYPE_PREENFM2;
    //[/Constructor_pre]

    operatorGroup.reset (new juce::GroupComponent ("operator group",
                                                   juce::String()));
    addAndMakeVisible (operatorGroup.get());
    operatorGroup->setTextLabelPosition (juce::Justification::centredLeft);
    operatorGroup->setColour (juce::GroupComponent::outlineColourId, juce::Colour (0xff526a7c));
    operatorGroup->setColour (juce::GroupComponent::textColourId, juce::Colour (0xff749fad));

    mixerGroup.reset (new juce::GroupComponent ("mixer group",
                                                TRANS("Mixer")));
    addAndMakeVisible (mixerGroup.get());
    mixerGroup->setTextLabelPosition (juce::Justification::centredLeft);
    mixerGroup->setColour (juce::GroupComponent::outlineColourId, juce::Colour (0xff35aeb4));
    mixerGroup->setColour (juce::GroupComponent::textColourId, juce::Colour (0xff69cdd1));

    imGroup.reset (new juce::GroupComponent ("IM group",
                                             TRANS("Modulation indexes")));
    addAndMakeVisible (imGroup.get());
    imGroup->setTextLabelPosition (juce::Justification::centredLeft);
    imGroup->setColour (juce::GroupComponent::outlineColourId, juce::Colour (0xff7383c5));
    imGroup->setColour (juce::GroupComponent::textColourId, juce::Colour (0xff9da9e0));


    //[UserPreSize]
	for (int k = 0; k < NUMBER_OF_MIX; k++) {
		addAndMakeVisible((mixKnob[k] = std::make_unique<SliderPfm2>("Mix " + String(k + 1))).get());
		mixKnob[k]->setRange(0, 1, .01f);
		mixKnob[k]->setSliderStyle(Slider::RotaryVerticalDrag);
		mixKnob[k]->setTextBoxStyle(Slider::TextBoxBelow, false, 40, 16);
		mixKnob[k]->setDoubleClickReturnValue(true, 1.0f);
		mixKnob[k]->setValue(1.0f, dontSendNotification);
		mixKnob[k]->addListener(this);

		addAndMakeVisible((panKnob[k] = std::make_unique<SliderPfm2>("Pan " + String(k + 1))).get());
		panKnob[k]->setRange(-1, 1, .01f);
		panKnob[k]->setSliderStyle(Slider::LinearHorizontal);
		panKnob[k]->setTextBoxStyle(Slider::NoTextBox, false, 40, 20);
		panKnob[k]->setColour(Slider::thumbColourId, Colours::blue);
		panKnob[k]->setDoubleClickReturnValue(true, 0.0f);
		panKnob[k]->addListener(this);

		addAndMakeVisible((mixLabel[k] = std::make_unique<Label>(String("mix label ") + String(k + 1), String("Mix ") + String(k + 1))).get());
		mixLabel[k]->setJustificationType(Justification::centred);
	}

	for (int k = 0; k < NUMBER_OF_IM; k++) {
		if (k < (NUMBER_OF_IM - 1)) {
			addAndMakeVisible((IMNumber[k] = std::make_unique<Label>("IM Label" + String(k + 1), String("IM") + String(k + 1))).get());
		} else {
			addAndMakeVisible((IMNumber[k] = std::make_unique<Label>("IM Label" + String(k + 1), String("Feedback"))).get());
		}
		IMNumber[k]->setColour(Label::textColourId,
			modulationConnectionColours[jlimit(0, 5, k)]);

		addAndMakeVisible((IMKnob[k] = std::make_unique<SliderPfm2>("IM " + String(k + 1))).get());
		IMKnob[k]->setRange(0, k < (NUMBER_OF_IM - 1) ? 16 : 1, .01f);
		IMKnob[k]->setSliderStyle(Slider::RotaryVerticalDrag);
		IMKnob[k]->setTextBoxStyle(Slider::TextBoxLeft, false, 40, 16);
		IMKnob[k]->setDoubleClickReturnValue(true, 1.0f);
		IMKnob[k]->setValue(1.0f, dontSendNotification);
		IMKnob[k]->addListener(this);

		addAndMakeVisible((IMVelocityKnob[k] = std::make_unique<SliderPfm2>("IM Velocity " + String(k + 1))).get());
		IMVelocityKnob[k]->setRange(0, k < (NUMBER_OF_IM - 1) ? 16 : 1, .01f);
		IMVelocityKnob[k]->setSliderStyle(Slider::RotaryVerticalDrag);
		IMVelocityKnob[k]->setTextBoxStyle(Slider::TextBoxLeft, false, 40, 16);
		IMVelocityKnob[k]->setDoubleClickReturnValue(true, 0.0f);
		IMVelocityKnob[k]->setValue(0.0f, dontSendNotification);
		IMVelocityKnob[k]->addListener(this);
	}


	addAndMakeVisible((IMLabel = std::make_unique<Label>("IM Label", "Main")).get());
	IMLabel->setJustificationType(Justification::centredTop);
	addAndMakeVisible((IMVelocityLabel = std::make_unique<Label>("IM Velocity Label", "Velocity")).get());
	IMVelocityLabel->setJustificationType(Justification::centredTop);

	addAndMakeVisible((envCopyButton = std::make_unique<TextButton>("Copy")).get());
	envCopyButton->setColour(TextButton::buttonColourId, Colour::fromRGBA(150, 150, 150, 50));
	envCopyButton->setColour(TextButton::buttonOnColourId, Colour::fromRGBA(150, 150, 150, 150));
	envCopyButton->addListener(this);
	addAndMakeVisible((envPasteButton = std::make_unique<TextButton>("Paste")).get());
	envPasteButton->setColour(TextButton::buttonColourId, Colour::fromRGBA(150, 150, 150, 50));
	envPasteButton->setColour(TextButton::buttonOnColourId, Colour::fromRGBA(150, 150, 150, 150));
	envPasteButton->addListener(this);


	for (int k = 0; k < NUMBER_OF_OPERATORS; k++) {
		enveloppe[k] = std::make_unique<Enveloppe>();
		enveloppe[k]->setName("Op" + String(k + 1) + " Env");

		enveloppeButton[k] = std::make_unique<TextButton>("enveloppe button");
		enveloppeButton[k]->setButtonText("Op" + String(k + 1));

		enveloppeButton[k]->setClickingTogglesState(true);
		enveloppeButton[k]->setRadioGroupId(4242);
		enveloppeButton[k]->setConnectedEdges((k != 0 ? Button::ConnectedOnLeft : 0) | (k != NUMBER_OF_OPERATORS - 1 ? Button::ConnectedOnRight : 0));
		enveloppeButton[k]->addListener(this);
		addAndMakeVisible(enveloppeButton[k].get());

		opShape[k] = std::make_unique<ComboBox>("Op" + String(k + 1) + " Shape");
		opShape[k]->setJustificationType(Justification::centred);
		opShape[k]->setColour(ComboBox::buttonColourId, Colours::blue);
		opShape[k]->addItem("Off", 8);
		opShape[k]->addItem("Sin", 1);
		opShape[k]->addItem("Saw", 2);
		opShape[k]->addItem("Square", 3);
		opShape[k]->addItem("Sin^2", 4);
		opShape[k]->addItem("SinZero", 5);
		opShape[k]->addItem("SinPos", 6);
		opShape[k]->addItem("Rand/Noise", 7);
		opShape[k]->addItem("User 1", 9);
		opShape[k]->addItem("User 2", 10);
		opShape[k]->addItem("User 3", 11);
		opShape[k]->addItem("User 4", 12);
		opShape[k]->addItem("User 5", 13);
		opShape[k]->addItem("User 6", 14);
		opShape[k]->setSelectedId(1);
		opShape[k]->setScrollWheelEnabled(true);
		opShape[k]->setEditableText(false);
		opShape[k]->addListener(this);

		opFrequencyType[k] = std::make_unique<ComboBox>("Op" + String(k + 1) + " Freq Type");
		opFrequencyType[k]->setEditableText(false);
		opFrequencyType[k]->setJustificationType(Justification::centred);
		opFrequencyType[k]->setColour(ComboBox::buttonColourId, Colours::blue);
		opFrequencyType[k]->addItem("Keyboard", 1);
		opFrequencyType[k]->addItem("Fine tune Hertz", 3);
		opFrequencyType[k]->addItem("Fixed", 2);
		opFrequencyType[k]->setScrollWheelEnabled(true);
		opFrequencyType[k]->setSelectedId(1);
		opFrequencyType[k]->addListener(this);

		opFrequency[k] = std::make_unique<SliderPfm2Always2Decimals>("Op" + String(k + 1) + " Frequency");
		opFrequency[k]->setRange(0, 16, 1.0f / 12.0f);
		opFrequency[k]->setSliderStyle(Slider::RotaryVerticalDrag);
		opFrequency[k]->setTextBoxStyle(Slider::TextBoxBelow, false, 60, 16);
		opFrequency[k]->setDoubleClickReturnValue(true, 1.0f);
		opFrequency[k]->addListener(this);

		opFrequencyFineTune[k] = std::make_unique<SliderPfm2>("Op" + String(k + 1) + " Fine Tune");
		opFrequencyFineTune[k]->setRange(-9.0f, 9.0f, .01f);
		opFrequencyFineTune[k]->setSliderStyle(Slider::RotaryVerticalDrag);
		opFrequencyFineTune[k]->setTextBoxStyle(Slider::TextBoxBelow, false, 40, 16);
		opFrequencyFineTune[k]->setDoubleClickReturnValue(true, 0.0f);
		opFrequencyFineTune[k]->addListener(this);

		if (k == 0) {
			addAndMakeVisible(enveloppe[k].get());
			addAndMakeVisible(opShape[k].get());
			addAndMakeVisible(opFrequencyType[k].get());
			addAndMakeVisible(opFrequency[k].get());
			addAndMakeVisible(opFrequencyFineTune[k].get());
		}
		else {
			addChildComponent(enveloppe[k].get());
			addChildComponent(opShape[k].get());
			addChildComponent(opFrequencyType[k].get());
			addChildComponent(opFrequency[k].get());
			addChildComponent(opFrequencyFineTune[k].get());
		}
	}
	enveloppeButton[0]->setToggleState(true, sendNotification);

	addAndMakeVisible((opShapeLabel = std::make_unique<Label>("op shapelabel", "Shape")).get());
	opShapeLabel->setJustificationType(Justification::centredTop);

	addAndMakeVisible((opFrequencyTypeLabel = std::make_unique<Label>("op frequency type", "Follows")).get());
	opFrequencyTypeLabel->setJustificationType(Justification::centredTop);

	addAndMakeVisible((opFrequencyLabel = std::make_unique<Label>("op frequency label", "Frequency")).get());
	opFrequencyLabel->setJustificationType(Justification::centredTop);

	addAndMakeVisible((opFrequencyFineTuneLabel = std::make_unique<Label>("op frequency FT label", "Fine Tune")).get());
	opFrequencyFineTuneLabel->setJustificationType(Justification::centredTop);

	addAndMakeVisible((algoChooser = std::make_unique<SliderPfm2>("Algo")).get());
	algoChooser->setRange(1, NUMBER_OF_ALGO, 1);
	algoChooser->setTextBoxIsEditable(true);
	algoChooser->setSliderStyle(Slider::IncDecButtons);
	algoChooser->setTextBoxStyle(Slider::TextBoxAbove, false, 30, 16);
	algoChooser->setDoubleClickReturnValue(true, 1.0f);
	algoChooser->addListener(this);

	addAndMakeVisible((algoDrawableImage = std::make_unique<UnifiedAlgorithmDiagram>()).get());

	addAndMakeVisible((algoChooserLabel = std::make_unique<Label>("algo label", "Algo")).get());
	algoChooserLabel->setJustificationType(Justification::centredTop);

	addAndMakeVisible((velocityLabel = std::make_unique<Label>("velocity label", "Velocity")).get());
	velocityLabel->setJustificationType(Justification::centredTop);

	addAndMakeVisible((velocity = std::make_unique<SliderPfm2>("Velocity")).get());
	velocity->setRange(0, 16, 1);
	velocity->setSliderStyle(Slider::RotaryVerticalDrag);
	velocity->setTextBoxStyle(Slider::TextBoxAbove, false, 30, 16);
	velocity->setDoubleClickReturnValue(true, 1.0f);
	velocity->addListener(this);

	addAndMakeVisible((velocityLabel = std::make_unique<Label>("velocity label", "Velocity")).get());
	velocityLabel->setJustificationType(Justification::centredTop);

	addAndMakeVisible((velocity = std::make_unique<SliderPfm2>("Velocity")).get());
	velocity->setRange(0, 16, 1);
	velocity->setValue(12, dontSendNotification);
	velocity->setSliderStyle(Slider::RotaryVerticalDrag);
	velocity->setTextBoxStyle(Slider::TextBoxAbove, false, 30, 16);
	velocity->setDoubleClickReturnValue(true, 1.0f);
	velocity->addListener(this);

	addAndMakeVisible((voicesLabel = std::make_unique<Label>("voices label", "Voices")).get());
	voicesLabel->setJustificationType(Justification::centredTop);

	addAndMakeVisible((voices = std::make_unique<SliderPfm2>("Voices")).get());
	voices->setRange(0, 8, 1);
	voices->setSliderStyle(Slider::RotaryVerticalDrag);
	voices->setTextBoxStyle(Slider::TextBoxAbove, false, 30, 16);
	voices->setDoubleClickReturnValue(true, 3.0f);
	voices->addListener(this);

	playModePfm3 = std::make_unique<ComboBox>("Play Mode pfm3");
	playModePfm3->setJustificationType(Justification::centred);
	playModePfm3->setColour(ComboBox::buttonColourId, Colours::blue);
	playModePfm3->addItem("Mono", 1);
	playModePfm3->addItem("Poly", 2);
	playModePfm3->addItem("Unison", 3);
	playModePfm3->setSelectedId(2);
	playModePfm3->setScrollWheelEnabled(true);
	playModePfm3->setEditableText(false);
	playModePfm3->addListener(this);
	addAndMakeVisible(playModePfm3.get());

	playModePfm2 = std::make_unique<ComboBox>("Play Mode pfm2");
	playModePfm2->setJustificationType(Justification::centred);
	playModePfm2->setColour(ComboBox::buttonColourId, Colours::blue);
	playModePfm2->addItem("Poly", 1);
	playModePfm2->addItem("Unison", 2);
	playModePfm2->setSelectedId(1);
	playModePfm2->setScrollWheelEnabled(true);
	playModePfm2->setEditableText(false);
	playModePfm2->addListener(this);
	addAndMakeVisible(playModePfm2.get());


	addAndMakeVisible((glideLabel = std::make_unique<Label>("glide label", "Glide")).get());
	glideLabel->setJustificationType(Justification::centredTop);

	addAndMakeVisible((glide = std::make_unique<SliderPfm2>("Glide")).get());
	glide->setRange(0, 12, 1);
	glide->setSliderStyle(Slider::RotaryVerticalDrag);
	glide->setTextBoxStyle(Slider::TextBoxAbove, false, 30, 16);
	glide->setDoubleClickReturnValue(true, 3.0f);
	glide->setAlwaysOnTop(true);
	glide->addListener(this);

	addAndMakeVisible((glideTypeLabel = std::make_unique<Label>("glide type label", "Glide Type")).get());
	glideTypeLabel->setJustificationType(Justification::centredTop);

	glideType = std::make_unique<ComboBox>("Glide Type");
	glideType->setJustificationType(Justification::centred);
	glideType->setColour(ComboBox::buttonColourId, Colours::blue);
	glideType->addItem("Off", 1);
	glideType->addItem("Overlap", 2);
	glideType->addItem("Always", 3);
	glideType->setSelectedId(1);
	glideType->setScrollWheelEnabled(true);
	glideType->setEditableText(false);
	glideType->addListener(this);
	addAndMakeVisible(glideType.get());

	addAndMakeVisible((unisonSpreadLabel = std::make_unique<Label>("unison spread label", "Spread")).get());
	unisonSpreadLabel->setJustificationType(Justification::centredTop);

	addAndMakeVisible((unisonSpread = std::make_unique<SliderPfm2>("Unison Spread")).get());
	unisonSpread->setRange(0.0, 1.0, .01);
	unisonSpread->setSliderStyle(Slider::RotaryVerticalDrag);
	unisonSpread->setTextBoxStyle(Slider::TextBoxAbove, false, 50, 16);
	unisonSpread->setDoubleClickReturnValue(true, 3.0f);
	unisonSpread->addListener(this);

	addAndMakeVisible((unisonDetuneLabel = std::make_unique<Label>("Unison Detune label", "Detune")).get());
	unisonDetuneLabel->setJustificationType(Justification::centredTop);

	addAndMakeVisible((unisonDetune = std::make_unique<SliderPfm2>("Unison Detune")).get());
	unisonDetune->setRange(-1.0, 1.0, .01);
	unisonDetune->setSliderStyle(Slider::RotaryVerticalDrag);
	unisonDetune->setTextBoxStyle(Slider::TextBoxAbove, false, 50, 16);
	unisonDetune->setDoubleClickReturnValue(true, 3.0f);
	unisonDetune->addListener(this);


    //[/UserPreSize]

    setSize (900, 700);


    //[Constructor] You can add your own custom stuff here..
	eventsToAdd = nullptr;
	voices->setValue(4.0f, sendNotification);
	sliderValueChanged(voices.get());
	sliderValueChanged(algoChooser.get());
    //[/Constructor]
}

PanelEngine::~PanelEngine()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    operatorGroup = nullptr;
    mixerGroup = nullptr;
    imGroup = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void PanelEngine::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..

    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff0b1117));

	const auto drawModule = [&g](Rectangle<float> bounds, Colour fill, Colour border)
	{
		g.setColour(fill);
		g.fillRoundedRectangle(bounds, 9.0f);
		g.setColour(border);
		g.drawRoundedRectangle(bounds, 9.0f, 1.0f);
	};

	drawModule({ static_cast<float>(proportionOfWidth(0.008f)),
		static_cast<float>(proportionOfHeight(0.006f)),
		static_cast<float>(proportionOfWidth(0.405f)),
		static_cast<float>(proportionOfHeight(0.305f)) },
		Colour(0xff102630), Colour(0xff315d69));

	drawModule({ static_cast<float>(proportionOfWidth(0.418f)),
		static_cast<float>(proportionOfHeight(0.006f)),
		static_cast<float>(proportionOfWidth(0.185f)),
		static_cast<float>(proportionOfHeight(0.305f)) },
		Colour(0xff111e2b), Colour(0xff344b60));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void PanelEngine::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    operatorGroup->setBounds (proportionOfWidth (0.0103f), proportionOfHeight (0.5008f), proportionOfWidth (0.9803f), proportionOfHeight (0.4900f));
    mixerGroup->setBounds (proportionOfWidth (0.0103f), proportionOfHeight (0.3122f), proportionOfWidth (0.5906f), proportionOfHeight (0.1853f));
    imGroup->setBounds (proportionOfWidth (0.6103f), proportionOfHeight (0.0000f), proportionOfWidth (0.3794f), proportionOfHeight (0.4958f));
    //[UserResized] Add your own custom resize handling here..

	envCopyButton->setBounds(proportionOfWidth(0.023f), proportionOfHeight(0.645f), proportionOfWidth(0.06f), proportionOfHeight(0.03f));
	envPasteButton->setBounds(proportionOfWidth(0.09f), proportionOfHeight(0.645f), proportionOfWidth(0.06f), proportionOfHeight(0.03f));

	// OPERATORS !
	for (int k = 0; k < NUMBER_OF_OPERATORS; k++) {
		enveloppe[k]->setBounds(proportionOfWidth(0.0200f), proportionOfHeight(0.67f), proportionOfWidth(0.9600f), proportionOfHeight(0.30f));
		enveloppeButton[k]->setBounds(proportionOfWidth(0.0100f) + 2 + 50 * k, proportionOfHeight(0.50f) + 9, 50, 26);
	}

	opShapeLabel->setBounds(proportionOfWidth(0.4f), proportionOfHeight(.53f), proportionOfWidth(0.12f), 20);
	opFrequencyTypeLabel->setBounds(proportionOfWidth(0.55f), proportionOfHeight(.53f), proportionOfWidth(0.12f), 20);
	opFrequencyLabel->setBounds(proportionOfWidth(0.7f), proportionOfHeight(.52f), proportionOfWidth(0.08f), 20);
	opFrequencyFineTuneLabel->setBounds(proportionOfWidth(0.83f), proportionOfHeight(.52f), proportionOfWidth(0.08f), 20);
	for (int k = 0; k < NUMBER_OF_OPERATORS; k++) {
		opShape[k]->setBounds(proportionOfWidth(0.4f), proportionOfHeight(.58f), proportionOfWidth(0.12f), 20);
		opFrequencyType[k]->setBounds(proportionOfWidth(0.55f), proportionOfHeight(.58f), proportionOfWidth(0.12f), 20);
		opFrequency[k]->setBounds(proportionOfWidth(0.70f), proportionOfHeight(.55f), proportionOfWidth(0.08f), proportionOfHeight(0.11f));
		opFrequencyFineTune[k]->setBounds(proportionOfWidth(0.83f), proportionOfHeight(.55f), proportionOfWidth(0.08f), proportionOfHeight(0.11f));
	}

	// MIX !
	for (int k = 0; k < NUMBER_OF_MIX; k++) {
		mixKnob[k]->setBounds(proportionOfWidth(0.09f * k + .04f), proportionOfHeight(0.34f), proportionOfWidth(0.08f), proportionOfHeight(0.09f));
		mixLabel[k]->setBounds(proportionOfWidth(0.09f * k + .04f), proportionOfHeight(0.435f), proportionOfWidth(0.08f), proportionOfHeight(0.025f));
		panKnob[k]->setBounds(proportionOfWidth(0.09f * k + .04f), proportionOfHeight(0.45f), proportionOfWidth(0.08f), proportionOfHeight(.043f));
	}

	int numberOfIMs = pfmType == TYPE_PREENFM2 ? 5 : 6;
	float imHeight = .32f / (numberOfIMs - 1);
	float imYSpace = .352f / (numberOfIMs - 1);
	for (int k = 0; k < numberOfIMs; k++) {
		IMNumber[k]->setBounds(proportionOfWidth(0.63f), proportionOfHeight(.055f + imYSpace * k), proportionOfWidth(0.055f), proportionOfHeight(imHeight));
		IMKnob[k]->setBounds(proportionOfWidth(0.69f), proportionOfHeight(.055f + imYSpace * k), proportionOfWidth(0.11f), proportionOfHeight(imHeight));
		IMVelocityKnob[k]->setBounds(proportionOfWidth(0.84f), proportionOfHeight(.055f + imYSpace * k), proportionOfWidth(0.11f), proportionOfHeight(imHeight));
	}

	IMLabel->setBounds(proportionOfWidth(0.69f), proportionOfHeight(.02f), proportionOfWidth(0.11f), proportionOfHeight(0.05f));
	IMVelocityLabel->setBounds(proportionOfWidth(0.84f), proportionOfHeight(.02f), proportionOfWidth(0.11f), proportionOfHeight(0.05f));

	algoChooser->setBounds(proportionOfWidth(0.03f), proportionOfHeight(.05f), proportionOfWidth(0.08f), proportionOfHeight(0.08f));
	algoChooserLabel->setBounds(proportionOfWidth(0.03f), proportionOfHeight(.02f), proportionOfWidth(0.08f), 40);

	if (pfmType == TYPE_PREENFM2) {

		voicesLabel->setBounds(proportionOfWidth(0.42f), proportionOfHeight(.01f), proportionOfWidth(0.08f), proportionOfHeight(0.09f));
		voices->setBounds(proportionOfWidth(0.42f), proportionOfHeight(.01f) + 22, proportionOfWidth(0.10f), proportionOfHeight(0.09f));

		glideLabel->setText("Glide", NotificationType::dontSendNotification);
		glideLabel->setBounds(proportionOfWidth(0.51f), proportionOfHeight(.01f), proportionOfWidth(0.08f), proportionOfHeight(0.09f));
		glide->setBounds(proportionOfWidth(0.51f), proportionOfHeight(.01f) + 22, proportionOfWidth(0.10f), proportionOfHeight(0.09f));


		playModePfm2->setBounds(proportionOfWidth(0.45f), proportionOfHeight(.15f), proportionOfWidth(0.12f), 22);

		unisonSpreadLabel->setBounds(proportionOfWidth(0.42f), proportionOfHeight(.2f), proportionOfWidth(0.10f), 40);
		unisonSpread->setBounds(proportionOfWidth(0.42f), proportionOfHeight(.2f) + 22, proportionOfWidth(0.10f), proportionOfHeight(0.09f));

		unisonDetuneLabel->setBounds(proportionOfWidth(0.51f), proportionOfHeight(.2f), proportionOfWidth(0.10f), 40);
		unisonDetune->setBounds(proportionOfWidth(0.51f), proportionOfHeight(.2f) + 22, proportionOfWidth(0.10f), proportionOfHeight(0.09f));
	}
	else {
		voicesLabel->setBounds(proportionOfWidth(0.45f), proportionOfHeight(.01f), proportionOfWidth(0.12f), 22);
		playModePfm3->setBounds(proportionOfWidth(0.45f), proportionOfHeight(.01f) + 22, proportionOfWidth(0.12f), 22);

		//

		glideTypeLabel->setBounds(proportionOfWidth(0.43f), proportionOfHeight(.08f), proportionOfWidth(0.08f), 22);
		glideType->setBounds(proportionOfWidth(0.43f), proportionOfHeight(.1f) + 22, proportionOfWidth(0.08f), 22);

		glideLabel->setText("Glide Speed", NotificationType::dontSendNotification);
		glideLabel->setBounds(proportionOfWidth(0.51f), proportionOfHeight(.08f), proportionOfWidth(0.10f), 40);
		glide->setBounds(proportionOfWidth(0.51f), proportionOfHeight(.08f) + 22, proportionOfWidth(0.10f), proportionOfHeight(0.09f));


		unisonSpreadLabel->setBounds(proportionOfWidth(0.42f), proportionOfHeight(.2f), proportionOfWidth(0.10f), 40);
		unisonSpread->setBounds(proportionOfWidth(0.42f), proportionOfHeight(.2f) + 22, proportionOfWidth(0.10f), proportionOfHeight(0.09f));

		unisonDetuneLabel->setBounds(proportionOfWidth(0.51f), proportionOfHeight(.2f), proportionOfWidth(0.10f), 40);
		unisonDetune->setBounds(proportionOfWidth(0.51f), proportionOfHeight(.2f) + 22, proportionOfWidth(0.10f), proportionOfHeight(0.09f));

	}

	velocity->setBounds(proportionOfWidth(0.03f), proportionOfHeight(.20f), proportionOfWidth(0.08f), proportionOfHeight(0.11f));
	velocityLabel->setBounds(proportionOfWidth(0.03f), proportionOfHeight(.17f), proportionOfWidth(0.08f), 40);


	resizeAlgoDrawableImage();

    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...


void PanelEngine::resizeAlgoDrawableImage() {
	float left = (float)proportionOfWidth(0.12f);
	float top = (float)proportionOfHeight(0.007f);
	float width = (float)proportionOfWidth(0.32f);
	float height = (float)proportionOfHeight(0.31f);

	juce::Rectangle<float> rect = juce::Rectangle<float>(left, top, width, height);
	algoDrawableImage->setBounds(rect.toNearestInt());

}

void PanelEngine::newAlgo(int algoNumber) {
	algoNumber = jlimit(0, NUMBER_OF_ALGO - 1, algoNumber);
	int numberOfMixer = algoInformation[algoNumber].mix;
	for (int m = 0; m < 6; m++) {
		bool enable = m < numberOfMixer;
		enableComponent(mixKnob[m].get(), enable);
		enableComponent(panKnob[m].get(), enable);
		enableComponent(mixLabel[m].get(), enable);
	}

	int numberOfIM = algoInformation[algoNumber].im;
	for (int im = 0; im < 5; im++) {
		bool enable = im < numberOfIM;
		enableComponent(IMKnob[im].get(), enable);
		enableComponent(IMVelocityKnob[im].get(), enable);
		enableComponent(IMNumber[im].get(), enable);
	}
	int numberOfOp = algoInformation[algoNumber].osc;
	for (int o = 0; o < 6; o++) {
		if (o < numberOfOp) {
			enveloppeButton[o]->setEnabled(true);
		}
		else {
			enveloppeButton[o]->setEnabled(false);
			if (enveloppeButton[o]->getToggleState()) {
				enveloppeButton[0]->setToggleState(true, sendNotification);
			}
		}
		// Only carrier operators have loop
		enveloppe[o]->setOperatorType(algoOpInformation[algoNumber][o]);
	}
	static_cast<UnifiedAlgorithmDiagram*>(algoDrawableImage.get())->setAlgorithm(algoNumber);
	resizeAlgoDrawableImage();
}

void PanelEngine::sliderValueChanged(Slider* sliderThatWasMoved)
{
	//[UsersliderValueChanged_Pre]
	sliderValueChanged(sliderThatWasMoved, true);
	//[/UsersliderValueChanged_Pre]

	//[UsersliderValueChanged_Post]
	//[/UsersliderValueChanged_Post]
}

void PanelEngine::sliderValueChanged(Slider* sliderThatWasMoved, bool fromPluginUI)
{
	// Update the value if the change comes from the UI
	if (fromPluginUI) {
		AudioProcessorParameter* parameterReady = parameterMap[sliderThatWasMoved->getName()];
		if (parameterReady != nullptr) {
			float value = (float)sliderThatWasMoved->getValue();
			static_cast<MidifiedFloatParameter*>(parameterReady)->setRealValue(value);
		}
	}
	if (sliderThatWasMoved == algoChooser.get()) {
		newAlgo((int)(algoChooser->getValue() - 1));
	} else if (sliderThatWasMoved == voices.get()) {

		enableComponent(playModePfm2.get(), voices->getValue() > 1);

		bool unison = playModePfm2->getSelectedId() == 2;

		enableComponent(unisonSpreadLabel.get(), voices->getValue() > 1 && unison);
		enableComponent(unisonSpread.get(), voices->getValue() > 1 && unison);
		enableComponent(unisonDetuneLabel.get(), voices->getValue() > 1 && unison);
		enableComponent(unisonDetune.get(), voices->getValue() > 1 && unison);

		enableComponent(glideLabel.get(), voices->getValue() == 1 || unison);
		enableComponent(glide.get(), voices->getValue() == 1 || unison);
	}
}

void PanelEngine::comboBoxChanged(ComboBox* comboBoxThatHasChanged) {
	comboBoxChanged(comboBoxThatHasChanged, true);
}

void PanelEngine::comboBoxChanged(ComboBox* comboBoxThatHasChanged, bool fromPluginUI) {

	if (comboBoxThatHasChanged == playModePfm3.get()) {
		bool glideEnable = playModePfm3->getSelectedId() != 2;

		enableComponent(glideTypeLabel.get(), glideEnable);
		enableComponent(glideType.get(), glideEnable);

		if (pfmType == TYPE_PREENFM3) {
			comboBoxChanged(glideType.get(), false);
		}
		else {
			sliderValueChanged(voices.get(), false);
		}

		bool spreadEnable = playModePfm3->getSelectedId() >= 3;

		enableComponent(unisonSpreadLabel.get(), spreadEnable);
		enableComponent(unisonSpread.get(), spreadEnable);

		enableComponent(unisonDetuneLabel.get(), spreadEnable);
		enableComponent(unisonDetune.get(), spreadEnable);

	}
	else if (comboBoxThatHasChanged == playModePfm2.get()) {

		bool unison = playModePfm2->getSelectedId()== 2;

		enableComponent(unisonSpreadLabel.get(), unison);
		enableComponent(unisonSpread.get(), unison);

		enableComponent(unisonDetuneLabel.get(), unison);
		enableComponent(unisonDetune.get(), unison);

		enableComponent(glideLabel.get(), voices->getValue() == 1 || unison);
		enableComponent(glide.get(), voices->getValue() == 1 || unison);
	}
	else if (comboBoxThatHasChanged == glideType.get()) {
		bool glideEnable = (glideType->getSelectedId() != 1) && (playModePfm3->getSelectedId() != 2);
		enableComponent(glideLabel.get(), glideEnable);
		enableComponent(glide.get(), glideEnable);
	}


	// Update the value if the change comes from the UI
	if (fromPluginUI) {
		AudioProcessorParameter * parameterReady = parameterMap[comboBoxThatHasChanged->getName()];
		if (parameterReady != nullptr) {
			float value = (float)comboBoxThatHasChanged->getSelectedId();
			static_cast<MidifiedFloatParameter*>(parameterReady)->setRealValue(value);
		}
	}
}

void PanelEngine::buttonClicked(Button* buttonThatWasClicked)
{
	//[UserbuttonClicked_Pre]
	bool enveloppeButtonClicked = false;
	for (int k = 0; k < NUMBER_OF_OPERATORS; k++) {
		if (buttonThatWasClicked == enveloppeButton[k].get()) {
			enveloppeButtonClicked = true;
		}
	}
	if (enveloppeButtonClicked) {
		for (int k = 0; k < NUMBER_OF_OPERATORS; k++) {
			if (buttonThatWasClicked == enveloppeButton[k].get()) {
				envSelected = k;
				enveloppe[k]->setVisible(true);
				opShape[k]->setVisible(true);
				opFrequencyType[k]->setVisible(true);
				opFrequency[k]->setVisible(true);
				opFrequencyFineTune[k]->setVisible(true);
			}
			else {
				enveloppe[k]->setVisible(false);
				opShape[k]->setVisible(false);
				opFrequencyType[k]->setVisible(false);
				opFrequency[k]->setVisible(false);
				opFrequencyFineTune[k]->setVisible(false);
			}
		}
	}

	if (buttonThatWasClicked == envPasteButton.get()) {
		if (envToCopy >= 0) {
			for (int k = 0; k < enveloppe[envToCopy]->getNumberOfPoints(); k++) {
				enveloppe[envSelected]->setX(k, enveloppe[envToCopy]->getX(k));
				enveloppe[envSelected]->setY(k, enveloppe[envToCopy]->getY(k));
			}
			// Point 0 is not a real point.
			for (int k = 1; k < enveloppe[envToCopy]->getNumberOfPoints(); k++) {
				enveloppe[envSelected]->notifyObservers(k, true);
				enveloppe[envSelected]->notifyObservers(k, false);
			}
			enveloppe[envSelected]->repaint();
		}
	}
	else if (buttonThatWasClicked == envCopyButton.get()) {
		envToCopy = envSelected;
		envPasteButton->setButtonText("Paste " + String(envToCopy + 1));
	}

	//[/UserbuttonClicked_Pre]


	//[UserbuttonClicked_Post]
	//[/UserbuttonClicked_Post]
}



void PanelEngine::buildParameters() {
	updateSliderFromParameter(algoChooser.get());
	updateSliderFromParameter(velocity.get());
	updateSliderFromParameter(voices.get());
	updateSliderFromParameter(glide.get());

	// pfm3
	updateComboFromParameter(playModePfm3.get());
	updateComboFromParameter(glideType.get());
	updateSliderFromParameter(unisonDetune.get());
	updateSliderFromParameter(unisonSpread.get());

	// pfm2
	updateComboFromParameter(playModePfm2.get());

	for (int k = 0; k < NUMBER_OF_MIX; k++) {
		updateSliderFromParameter(mixKnob[k].get());
		updateSliderFromParameter(panKnob[k].get());
	}
	for (int k = 0; k < NUMBER_OF_IM; k++) {
		updateSliderFromParameter(IMKnob[k].get());
		updateSliderFromParameter(IMVelocityKnob[k].get());
	}
	for (int k = 0; k < NUMBER_OF_OPERATORS; k++) {
		updateComboFromParameter(opShape[k].get());
		updateComboFromParameter(opFrequencyType[k].get());
		updateSliderFromParameter(opFrequency[k].get());
		updateSliderFromParameter(opFrequencyFineTune[k].get());
	}

	// To fill map with all points
	updateUIEnveloppe("");

	for (int k = 0; k < NUMBER_OF_OPERATORS; k++) {
		// Let listen to enveloppe
		enveloppe[k]->addListener((EnveloppeListener*)this);
	}

}

void PanelEngine::updateUIEnveloppe(String paramName) {
	const char** pointName = enveloppe[0]->getPointSuffix();
	for (int k = 0; k < NUMBER_OF_OPERATORS; k++) {
		String pString = enveloppe[k]->getName();

		for (int p = 2; p < enveloppe[k]->getNumberOfPoints() * 2; p++) {
			String name = pString + String(pointName[p - 2]);

			MidifiedFloatParameter* param = checkParamExistence(name);

			if (param == nullptr || (paramName.length() > 0 && name != String(paramName))) {
				continue;
			}

			// And let's update the value and update the UI Without sending modification !!!
			// No modification : we dont want sliderValueChanged to be called in the different panels

			if ((p & 0x1) == 0) {
				if (param->getRealValue() != enveloppe[k]->getX(p / 2)) {
					enveloppe[k]->setX(p / 2, param->getRealValue());
					enveloppe[k]->repaint();

				}
			}
			else {
				if (param->getRealValue() != enveloppe[k]->getY(p / 2)) {
					enveloppe[k]->setY(p / 2, param->getRealValue());
					enveloppe[k]->repaint();
				}
			}
		}
	}
}

bool PanelEngine::containsThisParameterAsEnveloppe(String name) {
	return (name.startsWith("Op") && name.indexOf(" Env") == 3);
}


void PanelEngine::updateComboFromParameter_hook(ComboBox* combo) {
	comboBoxChanged(combo, false);
}

void PanelEngine::updateSliderFromParameter_hook(Slider* slider) {
	sliderValueChanged(slider, false);
}

void PanelEngine::sliderDragStarted(Slider* slider) {
	AudioProcessorParameter * param = parameterMap[slider->getName()];
	if (param != nullptr) {
		param->beginChangeGesture();
	}
}
void PanelEngine::sliderDragEnded(Slider* slider) {
	AudioProcessorParameter * param = parameterMap[slider->getName()];
	if (param != nullptr) {
		param->endChangeGesture();
	}
}


 void PanelEngine::setPfmType(int type) {
	pfmType = type;

	bool isPreenfm2 = pfmType == TYPE_PREENFM2;
	algoChooser->setRange(1, NUMBER_OF_ALGO, 1);
	hideTotallyComponent(IMKnob[5].get(), isPreenfm2);
	hideTotallyComponent(IMNumber[5].get(), isPreenfm2);
	hideTotallyComponent(IMVelocityKnob[5].get(), isPreenfm2);

	hideTotallyComponent(glideTypeLabel.get(), isPreenfm2);
	hideTotallyComponent(glideType.get(), isPreenfm2);

	hideTotallyComponent(voices.get(), !isPreenfm2);

	hideTotallyComponent(playModePfm2.get(), !isPreenfm2);
	hideTotallyComponent(playModePfm3.get(), isPreenfm2);



	if (isPreenfm2) {
		voicesLabel->setText("Voices", NotificationType::dontSendNotification);
		sliderValueChanged(voices.get(), false);

	} else 	{
		voicesLabel->setText("Play mode", NotificationType::dontSendNotification);
		comboBoxChanged(playModePfm3.get(), false);
	}

	newAlgo((int)(algoChooser->getValue() - 1));
 }


 void PanelEngine::enableComponent(Component* comp, bool enable) {
	 comp->setEnabled(enable);
	 comp->setAlpha(enable ? 1.0f : 0.4f);
}

 void PanelEngine::hideTotallyComponent(Component* comp, bool hide) {
	 comp->setVisible(!hide);
 }


//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PanelEngine" componentName=""
                 parentClasses="public Component, public Slider::Listener, public Button::Listener, public ComboBox::Listener, public PanelOfComponents"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="900"
                 initialHeight="700">
  <BACKGROUND backgroundColour="62934">
    <RECT pos="50% 1% 50% 60%" fill=" radial: 80% 5%, 51% 5%, 0=ff125368, 1=ff083543"
          hasStroke="0"/>
    <ROUNDRECT pos="14.119% 0.654% 27.2% 30.937%" cornerSize="10.0" fill="solid: ff125468"
               hasStroke="1" stroke="1.5, mitered, butt" strokeColour="solid: ff749fad"/>
  </BACKGROUND>
  <GROUPCOMPONENT name="operator group" id="3a99a017e94aaaf5" memberName="operatorGroup"
                  virtualName="" explicitFocusOrder="0" pos="1.03% 50.083% 98.026% 48.998%"
                  outlinecol="ff749fad" textcol="ff749fad" title="" textpos="33"/>
  <GROUPCOMPONENT name="mixer group" id="a41fc3891a2af464" memberName="mixerGroup"
                  virtualName="" explicitFocusOrder="0" pos="1.03% 31.219% 59.056% 18.531%"
                  outlinecol="ff749fad" textcol="ff749fad" title="Mixer" textpos="33"/>
  <GROUPCOMPONENT name="IM group" id="249d6ec6feb3696f" memberName="imGroup" virtualName=""
                  explicitFocusOrder="0" pos="61.03% 0% 37.94% 49.583%" outlinecol="ff749fad"
                  textcol="ff749fad" title="Modulation indexes" textpos="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

