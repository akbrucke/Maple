#include "PluginEditor.h"
#include <cmath>

namespace MaplePalette
{
    const juce::Colour bgBark        = juce::Colour::fromString ("FF131011");
    const juce::Colour panelBark     = juce::Colour::fromString ("FF1B1618");
    const juce::Colour borderBark    = juce::Colour::fromString ("FF302629");
    const juce::Colour gridLine      = juce::Colour::fromString ("FF261E21");
    const juce::Colour textIvory     = juce::Colour::fromString ("FFF3ECE8");
    const juce::Colour textMuted     = juce::Colour::fromString ("FFA39195");
    const juce::Colour mapleCrimson  = juce::Colour::fromString ("FFE11D48");
    const juce::Colour mapleScarlet  = juce::Colour::fromString ("FFFB7185");
    const juce::Colour mapleAmber    = juce::Colour::fromString ("FFF59E0B");
    const juce::Colour mapleVermil   = juce::Colour::fromString ("FFEA580C");
    const juce::Colour mapleMoss     = juce::Colour::fromString ("FF34D399");
}

juce::Font MapleLookAndFeel::getGaramondFont (float height, bool isBold)
{
    static const juce::Typeface::Ptr garamondTypeface = juce::Typeface::createSystemTypefaceFor (
        BinaryData::EBGaramond_ttf, BinaryData::EBGaramond_ttfSize);

    auto options = juce::FontOptions (garamondTypeface).withHeight (height);
    if (isBold)
        options = options.withStyle ("Bold");

    return juce::Font (options);
}

juce::Font MapleLookAndFeel::getInterFont (float height, bool isBold)
{
    static const juce::Typeface::Ptr interTypeface = juce::Typeface::createSystemTypefaceFor (
        BinaryData::Inter_ttf, BinaryData::Inter_ttfSize);

    auto options = juce::FontOptions (interTypeface).withHeight (height);
    if (isBold)
        options = options.withStyle ("Bold");

    return juce::Font (options);
}

MapleLookAndFeel::MapleLookAndFeel()
{
    setColour (juce::Label::textColourId, MaplePalette::textIvory);
    setColour (juce::TextEditor::backgroundColourId, MaplePalette::bgBark);
    setColour (juce::TextEditor::textColourId, MaplePalette::textIvory);
    setColour (juce::TextEditor::outlineColourId, MaplePalette::mapleCrimson);
    setColour (juce::PopupMenu::backgroundColourId, MaplePalette::panelBark);
    setColour (juce::PopupMenu::textColourId, MaplePalette::textIvory);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, MaplePalette::mapleCrimson.withAlpha (0.35f));
    setColour (juce::PopupMenu::highlightedTextColourId, MaplePalette::textIvory);
}

void MapleLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float rotaryStartAngle,
                                         float rotaryEndAngle, juce::Slider& slider)
{
    const float radius  = std::min (width, height) * 0.5f - 5.0f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float angle   = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    const auto accent = slider.findColour (juce::Slider::rotarySliderFillColourId);

    g.setColour (MaplePalette::bgBark);
    g.fillEllipse (centreX - radius + 4.0f, centreY - radius + 4.0f,
                   (radius - 4.0f) * 2.0f, (radius - 4.0f) * 2.0f);

    juce::Path bgArc;
    bgArc.addCentredArc (centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (MaplePalette::borderBark.brighter (0.25f));
    g.strokePath (bgArc, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        juce::Path valArc;
        valArc.addCentredArc (centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
        g.setColour (accent);
        g.strokePath (valArc, juce::PathStrokeType (3.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    juce::Path pointer;
    const float innerR = radius * 0.35f;
    const float outerR = radius - 3.5f;
    pointer.startNewSubPath (centreX + innerR * std::sin (angle), centreY - innerR * std::cos (angle));
    pointer.lineTo          (centreX + outerR * std::sin (angle), centreY - outerR * std::cos (angle));

    g.setColour (MaplePalette::textIvory);
    g.strokePath (pointer, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void MapleLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool,
                                     int, int, int, int, juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<int> (0, 0, width, height).toFloat().reduced (0.5f);
    g.setColour (MaplePalette::bgBark);
    g.fillRoundedRectangle (bounds, 5.0f);

    g.setColour (box.hasKeyboardFocus (true) ? MaplePalette::mapleCrimson : MaplePalette::borderBark.brighter (0.2f));
    g.drawRoundedRectangle (bounds, 5.0f, 1.1f);

    const float arrowX = width - 14.0f;
    const float arrowY = height * 0.5f;
    juce::Path p;
    p.startNewSubPath (arrowX - 3.5f, arrowY - 1.5f);
    p.lineTo          (arrowX,        arrowY + 2.0f);
    p.lineTo          (arrowX + 3.5f, arrowY - 1.5f);
    g.setColour (MaplePalette::textMuted);
    g.strokePath (p, juce::PathStrokeType (1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void MapleLookAndFeel::positionComboBoxText (juce::ComboBox& box, juce::Label& label)
{
    label.setBounds (6, 0, box.getWidth() - 22, box.getHeight());
    label.setFont (getInterFont (11.5f, true));
    label.setJustificationType (juce::Justification::centredLeft);
}

void MapleLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                         bool shouldDrawButtonAsHighlighted, bool)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (0.5f);
    const bool on = button.getToggleState();

    g.setColour (on ? MaplePalette::mapleCrimson.withAlpha (0.28f) : MaplePalette::bgBark);
    g.fillRoundedRectangle (bounds, 5.0f);

    g.setColour (on ? MaplePalette::mapleScarlet
                    : (shouldDrawButtonAsHighlighted ? MaplePalette::textMuted : MaplePalette::borderBark.brighter (0.2f)));
    g.drawRoundedRectangle (bounds, 5.0f, on ? 1.3f : 1.1f);

    g.setColour (on ? MaplePalette::textIvory : MaplePalette::textMuted);
    g.setFont (MapleLookAndFeel::getInterFont (10.5f, true));
    g.drawText (button.getButtonText(), bounds, juce::Justification::centred);
}

//==============================================================================
MapleKnob::MapleKnob (juce::AudioProcessorValueTreeState& apvts,
                      const juce::String& paramID,
                      const juce::String& labelText,
                      const juce::String& unitText,
                      juce::Colour accentColour,
                      bool isPrimary)
    : accent (accentColour), unit (unitText), primary (isPrimary)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    slider.setColour (juce::Slider::rotarySliderFillColourId, accent);
    slider.setMouseDragSensitivity (160);
    slider.onValueChange = [this] { updateValueText(); };
    slider.shiftCallback = [this] (double delta, bool isStart)
    {
        if (onShiftDrag)
            onShiftDrag (delta, isStart);
    };
    addAndMakeVisible (slider);

    nameLabel.setText (labelText, juce::dontSendNotification);
    nameLabel.setJustificationType (juce::Justification::centred);
    nameLabel.setFont (MapleLookAndFeel::getInterFont (primary ? 11.0f : 10.5f, true));
    nameLabel.setColour (juce::Label::textColourId, MaplePalette::textMuted);
    addAndMakeVisible (nameLabel);

    valueLabel.setJustificationType (juce::Justification::centred);
    valueLabel.setFont (MapleLookAndFeel::getInterFont (primary ? 12.0f : 11.5f, true));
    valueLabel.setColour (juce::Label::textColourId, MaplePalette::textIvory);
    valueLabel.setEditable (false, true, false);
    valueLabel.onTextChange = [this]
    {
        const double typed = valueLabel.getText().getDoubleValue();
        slider.setValue (typed, juce::sendNotificationSync);
        updateValueText();
    };
    addAndMakeVisible (valueLabel);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, paramID, slider);
    updateValueText();
}

void MapleKnob::updateValueText()
{
    const double val = slider.getValue();
    if (unit == "Hz" && val >= 1000.0)
    {
        valueLabel.setText (juce::String (val / 1000.0, 2) + " kHz", juce::dontSendNotification);
        return;
    }

    int decimals = 1;
    if (unit == "Hz" || unit == "%")
        decimals = 0;
    else if (unit == "dB")
        decimals = 2;

    juce::String txt (val, decimals);
    if (unit == "dB" && val > 0.005)
        txt = "+" + txt;

    valueLabel.setText (txt + " " + unit, juce::dontSendNotification);
}

void MapleKnob::paint (juce::Graphics&) {}

void MapleKnob::resized()
{
    auto b = getLocalBounds();
    nameLabel.setBounds (b.removeFromTop (15));
    valueLabel.setBounds (b.removeFromBottom (17));
    slider.setBounds (b.reduced (2));
}

//==============================================================================
MapleStageCard::MapleStageCard (juce::AudioProcessorValueTreeState& apvts,
                                int,
                                const juce::String& title,
                                const juce::String& typeParamID,
                                const juce::String& driveParamID)
    : driveKnob (apvts, driveParamID, "DRIVE", "%", MaplePalette::mapleCrimson, true),
      stageTitle (title)
{
    typeSelector.addItemList ({ "Tube", "73 Class-A", "Vintage Tape", "FET" }, 1);
    typeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        apvts, typeParamID, typeSelector);
    addAndMakeVisible (typeSelector);
    addAndMakeVisible (driveKnob);
}

void MapleStageCard::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setColour (MaplePalette::panelBark);
    g.fillRoundedRectangle (b, 8.0f);
    g.setColour (MaplePalette::borderBark);
    g.drawRoundedRectangle (b, 8.0f, 1.0f);

    g.setColour (MaplePalette::mapleScarlet);
    g.setFont (MapleLookAndFeel::getInterFont (12.0f, true));
    g.drawText (stageTitle, b.reduced (10, 6).removeFromTop (14), juce::Justification::centredLeft);
}

void MapleStageCard::resized()
{
    auto b = getLocalBounds().reduced (10, 8);
    b.removeFromTop (16);
    typeSelector.setBounds (b.removeFromTop (24));
    b.removeFromTop (6);
    driveKnob.setBounds (b);
}

//==============================================================================
MapleAudioProcessorEditor::MapleAudioProcessorEditor (MapleAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      stage1Card   (p.apvts, 0, "STAGE 1", "stage1Type", "stage1Drive"),
      stage2Card   (p.apvts, 1, "STAGE 2", "stage2Type", "stage2Drive"),
      stage3Card   (p.apvts, 2, "STAGE 3", "stage3Type", "stage3Drive"),
      lowFreqKnob  (p.apvts, "lowFreq",  "LOW FREQ",  "Hz", MaplePalette::mapleAmber, false),
      lowGainKnob  (p.apvts, "lowGain",  "LOW GAIN",  "dB", MaplePalette::mapleAmber, true),
      highFreqKnob (p.apvts, "highFreq", "HIGH FREQ", "Hz", MaplePalette::mapleAmber, false),
      highGainKnob (p.apvts, "highGain", "HIGH GAIN", "dB", MaplePalette::mapleAmber, true),
      inTrimKnob   (p.apvts, "inTrim",   "INPUT",     "dB", MaplePalette::mapleMoss,  false),
      outTrimKnob  (p.apvts, "outTrim",  "OUTPUT",    "dB", MaplePalette::mapleMoss,  false),
      mixKnob      (p.apvts, "mix",      "MIX",       "%",  MaplePalette::mapleScarlet, false)
{
    setLookAndFeel (&mapleLnf);

    addAndMakeVisible (stage1Card);
    addAndMakeVisible (stage2Card);
    addAndMakeVisible (stage3Card);

    stage1Card.driveKnob.onShiftDrag = [this] (double d, bool s) { handleShiftDriveLink (0, d, s); };
    stage2Card.driveKnob.onShiftDrag = [this] (double d, bool s) { handleShiftDriveLink (1, d, s); };
    stage3Card.driveKnob.onShiftDrag = [this] (double d, bool s) { handleShiftDriveLink (2, d, s); };

    for (auto* k : { &lowFreqKnob, &lowGainKnob, &highFreqKnob, &highGainKnob,
                     &inTrimKnob, &outTrimKnob, &mixKnob })
    {
        addAndMakeVisible (k);
    }

    drive2xAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        p.apvts, "drive2x", drive2xBtn);
    addAndMakeVisible (drive2xBtn);

    osSelector.addItemList ({ "2x OS", "4x OS", "8x OS" }, 1);
    osAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        p.apvts, "oversampling", osSelector);
    addAndMakeVisible (osSelector);

    setResizable (true, true);
    setResizeLimits (820, 480, 1440, 860);
    setSize (940, 540);

    startTimerHz (60);
}

MapleAudioProcessorEditor::~MapleAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void MapleAudioProcessorEditor::handleShiftDriveLink (int sourceStageIdx, double delta, bool isStart)
{
    const char* ids[3] = { "stage1Drive", "stage2Drive", "stage3Drive" };
    if (isStart)
    {
        for (int i = 0; i < 3; ++i)
            shiftBaseDrives[static_cast<size_t> (i)] = audioProcessor.apvts.getRawParameterValue (ids[i])->load();
        return;
    }

    for (int i = 0; i < 3; ++i)
    {
        if (i == sourceStageIdx)
            continue;

        if (auto* param = audioProcessor.apvts.getParameter (ids[i]))
        {
            const float target = juce::jlimit (0.0f, 100.0f,
                                               shiftBaseDrives[static_cast<size_t> (i)] + static_cast<float> (delta));
            param->setValueNotifyingHost (param->convertTo0to1 (target));
        }
    }
}

float MapleAudioProcessorEditor::getTotalDriveNormalized() const
{
    const float d1 = audioProcessor.apvts.getRawParameterValue ("stage1Drive")->load();
    const float d2 = audioProcessor.apvts.getRawParameterValue ("stage2Drive")->load();
    const float d3 = audioProcessor.apvts.getRawParameterValue ("stage3Drive")->load();
    const bool  is2x = audioProcessor.apvts.getRawParameterValue ("drive2x")->load() > 0.5f;

    // Combined percentage of all 3 gain knobs (0.0 to 1.0), scaled upward slightly when 2X DRIVE is engaged
    const float avgDrive = (d1 + d2 + d3) / 300.0f;
    return juce::jlimit (0.0f, 1.0f, is2x ? std::min (1.0f, avgDrive * 1.35f + 0.12f) : avgDrive);
}

void MapleAudioProcessorEditor::timerCallback()
{
    const float rawIn  = audioProcessor.meterInDb.load();
    const float rawOut = audioProcessor.meterOutDb.load();

    smoothInDb  = (rawIn > smoothInDb)   ? rawIn  : (smoothInDb  * 0.84f + rawIn  * 0.16f);
    smoothOutDb = (rawOut > smoothOutDb) ? rawOut : (smoothOutDb * 0.84f + rawOut * 0.16f);

    repaint();
}

void MapleAudioProcessorEditor::mouseDown (const juce::MouseEvent& e)
{
    const auto pt = e.getPosition();
    for (int i = 0; i < 4; ++i)
    {
        if (nodeBounds[static_cast<size_t> (i)].contains (pt))
        {
            if (auto* param = audioProcessor.apvts.getParameter ("eqPos"))
            {
                param->setValueNotifyingHost (param->convertTo0to1 (static_cast<float> (i)));
                repaint();
            }
            return;
        }
    }
}

void MapleAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced (14);

    auto header = bounds.removeFromTop (28);
    osSelector.setBounds (header.removeFromRight (82).reduced (0, 2));
    header.removeFromRight (8);
    drive2xBtn.setBounds (header.removeFromRight (76).reduced (0, 2));
    bounds.removeFromTop (8);

    auto bottomDock = bounds.removeFromBottom (164);
    bounds.removeFromBottom (10);

    meterArea = bounds.removeFromRight (78);
    bounds.removeFromRight (10);
    eqCurveArea = bounds;

    auto stagesZone = bottomDock.removeFromLeft (static_cast<int> (bottomDock.getWidth() * 0.48f));
    bottomDock.removeFromLeft (10);
    auto eqAndIoZone = bottomDock;

    const int stageW = (stagesZone.getWidth() - 16) / 3;
    stage1Card.setBounds (stagesZone.removeFromLeft (stageW));
    stagesZone.removeFromLeft (8);
    stage2Card.setBounds (stagesZone.removeFromLeft (stageW));
    stagesZone.removeFromLeft (8);
    stage3Card.setBounds (stagesZone);

    auto innerRight = eqAndIoZone.reduced (10, 8);
    innerRight.removeFromTop (36);
    innerRight.removeFromTop (6);

    const int knobW = innerRight.getWidth() / 7;
    lowFreqKnob.setBounds  (innerRight.removeFromLeft (knobW).reduced (2));
    lowGainKnob.setBounds  (innerRight.removeFromLeft (knobW).reduced (2));
    highFreqKnob.setBounds (innerRight.removeFromLeft (knobW).reduced (2));
    highGainKnob.setBounds (innerRight.removeFromLeft (knobW).reduced (2));
    inTrimKnob.setBounds   (innerRight.removeFromLeft (knobW).reduced (2));
    outTrimKnob.setBounds  (innerRight.removeFromLeft (knobW).reduced (2));
    mixKnob.setBounds      (innerRight.reduced (2));
}

void MapleAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (MaplePalette::bgBark);

    auto header = getLocalBounds().reduced (14).removeFromTop (28);
    g.setColour (MaplePalette::mapleScarlet);
    g.setFont (MapleLookAndFeel::getGaramondFont (24.0f, false));
    g.drawText ("MAPLE", header.removeFromLeft (82), juce::Justification::centredLeft);

    g.setColour (MaplePalette::textMuted);
    g.setFont (MapleLookAndFeel::getInterFont (11.0f, false));
    g.drawText ("HARMONIC MASTERING CONSOLE  |  SHIFT-DRAG LINKS STAGE DRIVES",
                header, juce::Justification::centredLeft);

    auto bottomDock = getLocalBounds().reduced (14).removeFromBottom (164);
    bottomDock.removeFromLeft (static_cast<int> (bottomDock.getWidth() * 0.48f) + 10);

    g.setColour (MaplePalette::panelBark);
    g.fillRoundedRectangle (bottomDock.toFloat(), 8.0f);
    g.setColour (MaplePalette::borderBark);
    g.drawRoundedRectangle (bottomDock.toFloat(), 8.0f, 1.0f);

    drawSignalFlowNodes (g, bottomDock.reduced (10, 8).removeFromTop (34));
    drawEqCurve (g, eqCurveArea);
    drawMeters (g, meterArea);
}

void MapleAudioProcessorEditor::drawSignalFlowNodes (juce::Graphics& g, juce::Rectangle<int> area)
{
    const int activePos = static_cast<int> (audioProcessor.apvts.getRawParameterValue ("eqPos")->load());
    const juce::StringArray labels { "EQ > STG 1", "STG 1 > EQ > STG 2", "STG 2 > EQ > STG 3", "STG 3 > EQ" };

    g.setColour (MaplePalette::borderBark.brighter (0.3f));
    g.drawHorizontalLine (area.getCentreY(), static_cast<float> (area.getX() + 12), static_cast<float> (area.getRight() - 12));

    const int gap = 6;
    const int pillW = (area.getWidth() - gap * 3) / 4;

    for (int i = 0; i < 4; ++i)
    {
        auto pill = area.withX (area.getX() + i * (pillW + gap)).withWidth (pillW).reduced (0, 3);
        nodeBounds[static_cast<size_t> (i)] = pill;

        const bool selected = (i == activePos);
        g.setColour (selected ? MaplePalette::mapleCrimson.withAlpha (0.28f) : MaplePalette::bgBark);
        g.fillRoundedRectangle (pill.toFloat(), 5.0f);

        g.setColour (selected ? MaplePalette::mapleScarlet : MaplePalette::borderBark.brighter (0.2f));
        g.drawRoundedRectangle (pill.toFloat(), 5.0f, selected ? 1.4f : 1.0f);

        g.setColour (selected ? MaplePalette::textIvory : MaplePalette::textMuted);
        g.setFont (MapleLookAndFeel::getInterFont (9.5f, selected));;
        g.drawText (labels[i], pill, juce::Justification::centred);
    }
}

void MapleAudioProcessorEditor::drawEqCurve (juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto b = bounds.toFloat();
    g.setColour (MaplePalette::panelBark);
    g.fillRoundedRectangle (b, 8.0f);

    // Total drive percentage across all 3 stages drives the Japanese maple color shift
    const float driveNorm = getTotalDriveNormalized();

    const auto leftCol  = MaplePalette::mapleAmber.interpolatedWith (MaplePalette::mapleVermil,  driveNorm);
    const auto midCol   = MaplePalette::mapleVermil.interpolatedWith (MaplePalette::mapleCrimson, driveNorm);
    const auto rightCol = MaplePalette::mapleAmber.interpolatedWith (MaplePalette::mapleScarlet, driveNorm);

    g.setColour (MaplePalette::borderBark.interpolatedWith (midCol, driveNorm * 0.65f));
    g.drawRoundedRectangle (b, 8.0f, 1.0f + driveNorm * 0.9f);

    auto plot = b.reduced (26.0f, 22.0f);
    constexpr float minDb = -15.0f;
    constexpr float maxDb = 15.0f;
    constexpr float spanDb = maxDb - minDb;

    const float logMin = std::log10 (20.0f);
    const float logMax = std::log10 (20000.0f);

    auto freqToX = [&] (float f)
    {
        const float norm = (std::log10 (juce::jlimit (20.0f, 20000.0f, f)) - logMin) / (logMax - logMin);
        return plot.getX() + norm * plot.getWidth();
    };
    auto dbToY = [&] (float db)
    {
        return plot.getBottom() - ((db - minDb) / spanDb) * plot.getHeight();
    };

    g.setFont (MapleLookAndFeel::getInterFont (9.5f));
    for (float f : { 30.0f, 60.0f, 100.0f, 250.0f, 500.0f, 1000.0f, 2500.0f, 5000.0f, 10000.0f, 16000.0f })
    {
        const float gx = freqToX (f);
        g.setColour (MaplePalette::gridLine);
        g.drawVerticalLine (static_cast<int> (gx), plot.getY(), plot.getBottom());

        juce::String lbl = (f >= 1000.0f) ? (juce::String (static_cast<int> (f / 1000.0f)) + "k")
                                        : juce::String (static_cast<int> (f));
        g.setColour (MaplePalette::textMuted.withAlpha (0.7f));
        g.drawText (lbl, static_cast<int> (gx - 14), static_cast<int> (plot.getBottom() + 2), 28, 12,
                    juce::Justification::centred);
    }

    // Horizontal dB lines (-12, -6, 0, +6, +12 dB)
    for (float db : { -12.0f, -6.0f, 0.0f, 6.0f, 12.0f })
    {
        const float gy = dbToY (db);
        g.setColour (db == 0.0f ? MaplePalette::textMuted.withAlpha (0.35f) : MaplePalette::gridLine);
        g.drawHorizontalLine (static_cast<int> (gy), plot.getX(), plot.getRight());
    }

    const float lowFreq  = audioProcessor.apvts.getRawParameterValue ("lowFreq")->load();
    const float lowGain  = audioProcessor.apvts.getRawParameterValue ("lowGain")->load();
    const float highFreq = audioProcessor.apvts.getRawParameterValue ("highFreq")->load();
    const float highGain = audioProcessor.apvts.getRawParameterValue ("highGain")->load();

    juce::Path eqPath;
    juce::Path fillPath;
    const float zeroY = dbToY (0.0f);
    fillPath.startNewSubPath (plot.getX(), zeroY);

    const int steps = 160;
    for (int i = 0; i <= steps; ++i)
    {
        const float norm = static_cast<float> (i) / steps;
        const float freq = std::pow (10.0f, logMin + norm * (logMax - logMin));
        const float magDb = juce::jlimit (minDb, maxDb,
                                          MapleAudioProcessor::computeBaxandallMagnitudeDb (
                                              freq, lowFreq, lowGain, highFreq, highGain));
        const float px = plot.getX() + norm * plot.getWidth();
        const float py = dbToY (magDb);

        if (i == 0)
            eqPath.startNewSubPath (px, py);
        else
            eqPath.lineTo (px, py);

        fillPath.lineTo (px, py);
    }

    fillPath.lineTo (plot.getRight(), zeroY);
    fillPath.closeSubPath();

    // Multi-stop horizontal ColourGradient that shifts from autumn gold -> vermilion -> maple crimson
    juce::ColourGradient fillGrad (leftCol.withAlpha (0.12f + driveNorm * 0.24f), plot.getX(), zeroY,
                                   rightCol.withAlpha (0.12f + driveNorm * 0.24f), plot.getRight(), zeroY, false);
    fillGrad.addColour (0.5, midCol.withAlpha (0.16f + driveNorm * 0.28f));
    g.setGradientFill (fillGrad);
    g.fillPath (fillPath);

    juce::ColourGradient strokeGrad (leftCol, plot.getX(), zeroY,
                                     rightCol, plot.getRight(), zeroY, false);
    strokeGrad.addColour (0.5, midCol);

    if (driveNorm > 0.01f)
    {
        juce::ColourGradient glowGrad (leftCol.withAlpha (driveNorm * 0.38f), plot.getX(), zeroY,
                                       rightCol.withAlpha (driveNorm * 0.38f), plot.getRight(), zeroY, false);
        glowGrad.addColour (0.5, midCol.withAlpha (driveNorm * 0.45f));
        g.setGradientFill (glowGrad);
        g.strokePath (eqPath, juce::PathStrokeType (6.0f + driveNorm * 6.0f,
                                                    juce::PathStrokeType::curved,
                                                    juce::PathStrokeType::rounded));
    }

    g.setGradientFill (strokeGrad);
    g.strokePath (eqPath, juce::PathStrokeType (2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    g.setColour (MaplePalette::textMuted);
    g.setFont (MapleLookAndFeel::getInterFont (10.0f, true));
    g.drawText ("BAXANDALL RESPONSE", b.reduced (12, 6).removeFromTop (14), juce::Justification::centredLeft);

    const float rawTotalPct = (audioProcessor.apvts.getRawParameterValue ("stage1Drive")->load()
                             + audioProcessor.apvts.getRawParameterValue ("stage2Drive")->load()
                             + audioProcessor.apvts.getRawParameterValue ("stage3Drive")->load()) / 3.0f;
    g.setColour (midCol);
    g.drawText ("TOTAL DRIVE: " + juce::String (static_cast<int> (std::round (rawTotalPct))) + "%",
                b.reduced (12, 6).removeFromTop (14), juce::Justification::centredRight);
}

void MapleAudioProcessorEditor::drawMeters (juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto b = bounds.toFloat();
    g.setColour (MaplePalette::panelBark);
    g.fillRoundedRectangle (b, 8.0f);
    g.setColour (MaplePalette::borderBark);
    g.drawRoundedRectangle (b, 8.0f, 1.0f);

    auto inner = b.reduced (10.0f, 12.0f);
    const float colW = inner.getWidth() / 2.0f;

    auto drawBar = [&] (juce::Rectangle<float> col, const juce::String& title, float valDb, juce::Colour fill)
    {
        g.setColour (MaplePalette::textMuted);
        g.setFont (MapleLookAndFeel::getInterFont (10.0f, true));
        g.drawText (title, col.removeFromTop (14.0f), juce::Justification::centred);

        auto numRect = col.removeFromBottom (16.0f);
        g.setColour (MaplePalette::textIvory);
        g.setFont (MapleLookAndFeel::getInterFont (9.5f, true));
        g.drawText (valDb <= -59.5f ? "-inf" : juce::String (valDb, 1), numRect, juce::Justification::centred);

        auto bar = col.reduced (6.0f, 4.0f);
        g.setColour (MaplePalette::bgBark);
        g.fillRoundedRectangle (bar, 3.0f);

        const float norm = juce::jlimit (0.0f, 1.0f, (valDb + 60.0f) / 66.0f);
        const float h = bar.getHeight() * norm;
        g.setColour (fill);
        g.fillRoundedRectangle (bar.withTop (bar.getBottom() - h), 3.0f);
    };

    drawBar (inner.removeFromLeft (colW), "IN",  smoothInDb,  MaplePalette::mapleMoss);
    drawBar (inner,                       "OUT", smoothOutDb, MaplePalette::mapleScarlet);
}