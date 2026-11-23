#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <atomic>

class MapleAudioProcessor : public juce::AudioProcessor
{
public:
    MapleAudioProcessor();
    ~MapleAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts;

    // Lock-free telemetry for UI meters
    std::atomic<float> meterInDb  { -100.0f };
    std::atomic<float> meterOutDb { -100.0f };

    // Shared Baxandall magnitude calculation for accurate UI EQ plotting
    static float computeBaxandallMagnitudeDb (float freqHz,
                                              float lowFreq, float lowGainDb,
                                              float highFreq, float highGainDb);

private:
    std::array<std::unique_ptr<juce::dsp::Oversampling<float>>, 3> oversamplers;
    int currentOsIndex = 1; // Default 4x
    double baseSampleRate = 48000.0;
    int maxBlockSize = 512;

    // 2-Band Stereo Baxandall EQ Filters
    juce::dsp::IIR::Filter<float> lowShelfL, lowShelfR;
    juce::dsp::IIR::Filter<float> highShelfL, highShelfR;
    float lastLowFreq  = -1.0f, lastLowGain  = -999.0f;
    float lastHighFreq = -1.0f, lastHighGain = -999.0f;
    double lastFilterRate = -1.0;

    // Stateful analog stage memories
    struct StageState
    {
        float tapePreL = 0.0f, tapePreR = 0.0f;
        float tapeDeL  = 0.0f, tapeDeR  = 0.0f;
        float xfmrMemL = 0.0f, xfmrMemR = 0.0f;
        float dcBlockInL = 0.0f, dcBlockOutL = 0.0f;
        float dcBlockInR = 0.0f, dcBlockOutR = 0.0f;

        void reset()
        {
            tapePreL = tapePreR = 0.0f;
            tapeDeL  = tapeDeR  = 0.0f;
            xfmrMemL = xfmrMemR = 0.0f;
            dcBlockInL = dcBlockOutL = 0.0f;
            dcBlockInR = dcBlockOutR = 0.0f;
        }
    };
    std::array<StageState, 3> stageStates;

    // Parameter smoothers
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> inGainSmoothed;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> outGainSmoothed;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mixSmoothed;
    std::array<juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>, 3> driveSmoothed;

    void updateBaxandallFilters (double osRate, float lowFreq, float lowGainDb, float highFreq, float highGainDb);
    static float processTopologySample (float x, int type, float driveScaled, bool is2xMode,
                                        double osRate, StageState& st, bool isLeft);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapleAudioProcessor)
};