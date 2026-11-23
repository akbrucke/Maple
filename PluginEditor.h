#pragma once

#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"
#include <array>
#include <functional>

class MapleLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MapleLookAndFeel();

    static juce::Font getGaramondFont (float height, bool isBold = false);
    static juce::Font getInterFont (float height, bool isBold = false);

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override;

    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox& box) override;

    void positionComboBoxText (juce::ComboBox& box, juce::Label& label) override;

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

class MapleKnob : public juce::Component
{
public:
    MapleKnob (juce::AudioProcessorValueTreeState& apvts,
               const juce::String& paramID,
               const juce::String& labelText,
               const juce::String& unitText,
               juce::Colour accentColour,
               bool isPrimary = false);

    void resized() override;
    void paint (juce::Graphics& g) override;

    std::function<void (double delta, bool isStart)> onShiftDrag;

private:
    class LinkableSlider : public juce::Slider
    {
    public:
        std::function<void (double delta, bool isStart)> shiftCallback;
        double dragStartVal = 0.0;

        void mouseDown (const juce::MouseEvent& e) override
        {
            dragStartVal = getValue();
            if (e.mods.isShiftDown() && shiftCallback)
                shiftCallback (0.0, true);
            juce::Slider::mouseDown (e);
        }

        void mouseDrag (const juce::MouseEvent& e) override
        {
            juce::Slider::mouseDrag (e);
            if (e.mods.isShiftDown() && shiftCallback)
                shiftCallback (getValue() - dragStartVal, false);
        }
    };

    LinkableSlider slider;
    juce::Label nameLabel;
    juce::Label valueLabel;
    juce::Colour accent;
    juce::String unit;
    bool primary;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

    void updateValueText();
};

class MapleStageCard : public juce::Component
{
public:
    MapleStageCard (juce::AudioProcessorValueTreeState& apvts,
                    int stageIndex,
                    const juce::String& title,
                    const juce::String& typeParamID,
                    const juce::String& driveParamID);

    void paint (juce::Graphics& g) override;
    void resized() override;

    MapleKnob driveKnob;

private:
    juce::String stageTitle;
    juce::ComboBox typeSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> typeAttachment;
};

class MapleAudioProcessorEditor : public juce::AudioProcessorEditor,
                                  private juce::Timer
{
public:
    explicit MapleAudioProcessorEditor (MapleAudioProcessor&);
    ~MapleAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& e) override;

private:
    MapleAudioProcessor& audioProcessor;
    MapleLookAndFeel mapleLnf;

    MapleStageCard stage1Card;
    MapleStageCard stage2Card;
    MapleStageCard stage3Card;

    MapleKnob lowFreqKnob;
    MapleKnob lowGainKnob;
    MapleKnob highFreqKnob;
    MapleKnob highGainKnob;

    MapleKnob inTrimKnob;
    MapleKnob outTrimKnob;
    MapleKnob mixKnob;

    juce::ToggleButton drive2xBtn { "2X DRIVE" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> drive2xAttachment;

    juce::ComboBox osSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osAttachment;

    std::array<juce::Rectangle<int>, 4> nodeBounds;

    float smoothInDb   = -60.0f;
    float smoothOutDb  = -60.0f;

    std::array<float, 3> shiftBaseDrives { 0.0f, 0.0f, 0.0f };

    juce::Rectangle<int> eqCurveArea;
    juce::Rectangle<int> meterArea;

    void timerCallback() override;
    void handleShiftDriveLink (int sourceStageIdx, double delta, bool isStart);
    float getTotalDriveNormalized() const;
    void drawSignalFlowNodes (juce::Graphics& g, juce::Rectangle<int> area);
    void drawEqCurve (juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawMeters (juce::Graphics& g, juce::Rectangle<int> bounds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapleAudioProcessorEditor)
};