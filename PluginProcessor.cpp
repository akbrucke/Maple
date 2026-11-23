#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>
#include <algorithm>
#include <complex>

MapleAudioProcessor::MapleAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout MapleAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    const juce::StringArray topologies { "Tube", "73 Class-A", "Vintage Tape", "FET" };
    const juce::StringArray eqPositions { "Pre-Stage 1", "Post-Stage 1", "Post-Stage 2", "Post-Stage 3" };
    const juce::StringArray osChoices { "2x", "4x", "8x" };

    // Input Trim: -4.0 dB default
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "inTrim", 1 }, "Input Trim",
        juce::NormalisableRange<float> (-18.0f, 18.0f, 0.1f), -4.0f, "dB"));

    // Stage 1: 73 Class-A (index 1) @ 25%
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "stage1Type", 1 }, "Stage 1 Type", topologies, 1));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "stage1Drive", 1 }, "Stage 1 Drive",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 25.0f, "%"));

    // Stage 2: Vintage Tape (index 2) @ 25%
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "stage2Type", 1 }, "Stage 2 Type", topologies, 2));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "stage2Drive", 1 }, "Stage 2 Drive",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 25.0f, "%"));

    // Stage 3: Tube (index 0) @ 25%
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "stage3Type", 1 }, "Stage 3 Type", topologies, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "stage3Drive", 1 }, "Stage 3 Drive",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 25.0f, "%"));

    // 2x Drive Range Toggle: Off
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "drive2x", 1 }, "2x Drive Range", false));

    // EQ Routing: Pre-Stage 1 (index 0), Flat @ 80 Hz and 8 kHz
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "eqPos", 1 }, "EQ Position", eqPositions, 0));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "lowFreq", 1 }, "Low Shelf Freq",
        juce::NormalisableRange<float> (20.0f, 400.0f, 1.0f, 0.5f), 80.0f, "Hz"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "lowGain", 1 }, "Low Shelf Gain",
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.05f), 0.0f, "dB"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "highFreq", 1 }, "High Shelf Freq",
        juce::NormalisableRange<float> (2000.0f, 20000.0f, 10.0f, 0.5f), 8000.0f, "Hz"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "highGain", 1 }, "High Shelf Gain",
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.05f), 0.0f, "dB"));

    // Oversampling: 4x (index 1)
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "oversampling", 1 }, "Oversampling", osChoices, 1));

    // Output Trim: +4.0 dB default
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "outTrim", 1 }, "Output Trim",
        juce::NormalisableRange<float> (-18.0f, 18.0f, 0.1f), 4.0f, "dB"));

    // Mix: 100% default
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "mix", 1 }, "Mix",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 100.0f, "%"));

    return { params.begin(), params.end() };
}

void MapleAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    baseSampleRate = sampleRate;
    maxBlockSize   = samplesPerBlock;

    for (int i = 0; i < 3; ++i)
    {
        oversamplers[static_cast<size_t> (i)] = std::make_unique<juce::dsp::Oversampling<float>> (
            2, i + 1, juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple, true);
        oversamplers[static_cast<size_t> (i)]->initProcessing (static_cast<size_t> (samplesPerBlock));
    }

    currentOsIndex = juce::jlimit (0, 2, static_cast<int> (apvts.getRawParameterValue ("oversampling")->load()));
    const int factor = 1 << (currentOsIndex + 1);
    const double osRate = baseSampleRate * static_cast<double> (factor);

    juce::dsp::ProcessSpec osSpec;
    osSpec.sampleRate       = osRate;
    osSpec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock * factor);
    osSpec.numChannels      = 2;

    lowShelfL.prepare (osSpec);
    lowShelfR.prepare (osSpec);
    highShelfL.prepare (osSpec);
    highShelfR.prepare (osSpec);
    lowShelfL.reset();
    lowShelfR.reset();
    highShelfL.reset();
    highShelfR.reset();

    lastLowFreq = lastHighFreq = -1.0f;
    lastLowGain = lastHighGain = -999.0f;
    lastFilterRate = -1.0;

    for (auto& st : stageStates)
        st.reset();

    const double rampSec = 0.015;
    inGainSmoothed.reset (osRate, rampSec);
    outGainSmoothed.reset (osRate, rampSec);
    mixSmoothed.reset (osRate, rampSec);
    for (auto& d : driveSmoothed)
        d.reset (osRate, rampSec);

    setLatencySamples (static_cast<int> (std::round (oversamplers[static_cast<size_t> (currentOsIndex)]->getLatencyInSamples())));
}

void MapleAudioProcessor::releaseResources()
{
    for (auto& os : oversamplers)
        if (os)
            os->reset();
}

bool MapleAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet();
}

void MapleAudioProcessor::updateBaxandallFilters (double osRate, float lowFreq, float lowGainDb, float highFreq, float highGainDb)
{
    constexpr float baxQ = 0.55f;

    if (std::abs (osRate - lastFilterRate) > 1.0
        || std::abs (lowFreq - lastLowFreq) > 0.1f
        || std::abs (lowGainDb - lastLowGain) > 0.01f)
    {
        lastLowFreq = lowFreq;
        lastLowGain = lowGainDb;
        auto lowCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf (
            osRate, lowFreq, baxQ, juce::Decibels::decibelsToGain (lowGainDb));
        *lowShelfL.coefficients = *lowCoeffs;
        *lowShelfR.coefficients = *lowCoeffs;
    }

    if (std::abs (osRate - lastFilterRate) > 1.0
        || std::abs (highFreq - lastHighFreq) > 1.0f
        || std::abs (highGainDb - lastHighGain) > 0.01f)
    {
        lastHighFreq = highFreq;
        lastHighGain = highGainDb;
        auto highCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf (
            osRate, highFreq, baxQ, juce::Decibels::decibelsToGain (highGainDb));
        *highShelfL.coefficients = *highCoeffs;
        *highShelfR.coefficients = *highCoeffs;
    }

    lastFilterRate = osRate;
}

float MapleAudioProcessor::computeBaxandallMagnitudeDb (float freqHz,
                                                        float lowFreq, float lowGainDb,
                                                        float highFreq, float highGainDb)
{
    constexpr float baxQ = 0.55f;
    const double fs = 192000.0;

    auto evalBiquadDb = [&] (juce::dsp::IIR::Coefficients<float>::Ptr coeffs) -> float
    {
        const double w = 2.0 * juce::MathConstants<double>::pi * static_cast<double> (freqHz) / fs;
        const std::complex<double> z1 = std::polar (1.0, -w);
        const std::complex<double> z2 = std::polar (1.0, -2.0 * w);

        const auto* c = coeffs->getRawCoefficients();
        const std::complex<double> num = static_cast<double> (c[0]) + static_cast<double> (c[1]) * z1 + static_cast<double> (c[2]) * z2;
        const std::complex<double> den = 1.0 + static_cast<double> (c[3]) * z1 + static_cast<double> (c[4]) * z2;
        const double mag = std::abs (num / den);
        return static_cast<float> (20.0 * std::log10 (std::max (1.0e-6, mag)));
    };

    auto lowC  = juce::dsp::IIR::Coefficients<float>::makeLowShelf  (fs, lowFreq,  baxQ, juce::Decibels::decibelsToGain (lowGainDb));
    auto highC = juce::dsp::IIR::Coefficients<float>::makeHighShelf (fs, highFreq, baxQ, juce::Decibels::decibelsToGain (highGainDb));

    return evalBiquadDb (lowC) + evalBiquadDb (highC);
}

float MapleAudioProcessor::processTopologySample (float x, int type, float driveScaled, bool is2xMode,
                                                  double osRate, StageState& st, bool isLeft)
{
    if (driveScaled <= 0.0001f)
        return x;

    // 1x = 0.25x scale (up to ~2.1x internal drive); 2x = 0.85x scale (up to ~5.1x internal drive + extra bias push)
    const float preGain = 1.0f + driveScaled * 4.8f;
    const float driven  = x * preGain;
    float shaped = driven;

    if (type == 0)
    {
        // TUBE: Asymmetric triode grid conduction + warm even-order 2nd harmonics
        const float bias = (is2xMode ? 0.20f : 0.14f) * driveScaled;
        const float shifted = driven + bias;
        const float pos = shifted / (1.0f + 0.65f * std::abs (shifted));
        const float neg = std::tanh (shifted * 0.85f) / 0.85f;
        const float rawTube = (shifted >= 0.0f) ? pos : neg;
        const float biasOffset = bias / (1.0f + 0.65f * std::abs (bias));
        shaped = (rawTube - biasOffset) / preGain;
    }
    else if (type == 1)
    {
        // 73 CLASS-A: Transformer core low-end hysteresis + Class-A odd/even punch
        float& xfmrMem = isLeft ? st.xfmrMemL : st.xfmrMemR;
        const float lfCoeff = std::exp (-2.0f * juce::MathConstants<float>::pi * 45.0f / static_cast<float> (osRate));
        xfmrMem = lfCoeff * xfmrMem + (1.0f - lfCoeff) * driven;

        const float coreDrive = driven + (is2xMode ? 0.36f : 0.28f) * driveScaled * std::tanh (xfmrMem * 1.6f);
        const float asymEven  = 0.05f * driveScaled * (coreDrive * coreDrive) / (1.0f + std::abs (coreDrive));
        const float classA    = std::atan (coreDrive * 1.15f) / 1.15f + asymEven;
        shaped = classA / preGain;
    }
    else if (type == 2)
    {
        // VINTAGE TAPE: Magnetic flux tanh compression + HF head pre-emphasis/de-emphasis
        float& preMem = isLeft ? st.tapePreL : st.tapePreR;
        float& deMem  = isLeft ? st.tapeDeL  : st.tapeDeR;

        const float hfCoeff = std::exp (-2.0f * juce::MathConstants<float>::pi * 9500.0f / static_cast<float> (osRate));
        const float highPart = driven - preMem;
        preMem = hfCoeff * preMem + (1.0f - hfCoeff) * driven;

        const float fluxIn = driven + (0.38f * driveScaled) * highPart;
        const float fluxSat = std::tanh (fluxIn * 1.12f) / 1.12f;

        const float headCoeff = std::exp (-2.0f * juce::MathConstants<float>::pi
                                          * (24000.0f - 7500.0f * driveScaled) / static_cast<float> (osRate));
        deMem = headCoeff * deMem + (1.0f - headCoeff) * fluxSat;
        shaped = deMem / preGain;
    }
    else
    {
        // FET: Gritty Class-A FET output stage + small-core output transformer bite
        // Asymmetrical positive-swing FET pinch-off + cubic harmonic edge
        float& xfmrMem = isLeft ? st.xfmrMemL : st.xfmrMemR;
        const float lfCoeff = std::exp (-2.0f * juce::MathConstants<float>::pi * 75.0f / static_cast<float> (osRate));
        xfmrMem = lfCoeff * xfmrMem + (1.0f - lfCoeff) * driven;

        const float fetDrive = driven * (is2xMode ? 1.35f : 1.15f) + 0.18f * driveScaled * std::tanh (xfmrMem * 2.1f);
        float fetOut = 0.0f;
        if (fetDrive >= 0.0f)
        {
            // Harder asymmetrical positive rail saturation characteristic of Class-A FET comp
            const float clipped = std::tanh (fetDrive * 1.45f) / 1.45f;
            const float grit    = clipped - 0.12f * driveScaled * (clipped * clipped * clipped);
            fetOut = grit;
        }
        else
        {
            // Slightly softer negative swing with odd-harmonic edge
            const float neg = fetDrive / std::sqrt (1.0f + 1.35f * fetDrive * fetDrive);
            fetOut = neg;
        }
        shaped = fetOut / (preGain * (is2xMode ? 1.15f : 1.05f));
    }

    // Gentle 5 Hz DC blocker
    float& dcIn  = isLeft ? st.dcBlockInL  : st.dcBlockInR;
    float& dcOut = isLeft ? st.dcBlockOutL : st.dcBlockOutR;
    constexpr float dcR = 0.9997f;
    const float cleanOut = shaped - dcIn + dcR * dcOut;
    dcIn  = shaped;
    dcOut = cleanOut;

    return cleanOut;
}

void MapleAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();
    if (numChannels == 0 || numSamples == 0)
        return;

    const int requestedOs = juce::jlimit (0, 2, static_cast<int> (apvts.getRawParameterValue ("oversampling")->load()));
    if (requestedOs != currentOsIndex)
    {
        currentOsIndex = requestedOs;
        const int factor = 1 << (currentOsIndex + 1);
        const double osRate = baseSampleRate * static_cast<double> (factor);

        oversamplers[static_cast<size_t> (currentOsIndex)]->reset();
        setLatencySamples (static_cast<int> (std::round (
            oversamplers[static_cast<size_t> (currentOsIndex)]->getLatencyInSamples())));

        const double rampSec = 0.015;
        inGainSmoothed.reset (osRate, rampSec);
        outGainSmoothed.reset (osRate, rampSec);
        mixSmoothed.reset (osRate, rampSec);
        for (auto& d : driveSmoothed)
            d.reset (osRate, rampSec);
    }

    const int factor = 1 << (currentOsIndex + 1);
    const double osRate = baseSampleRate * static_cast<double> (factor);

    // 1x = 0.25x (subtle mastering sheen); 2x = 0.85x (3.4x deeper push into the non-linearities)
    const bool  is2xDrive  = apvts.getRawParameterValue ("drive2x")->load() > 0.5f;
    const float rangeScale = is2xDrive ? 0.85f : 0.25f;

    const float inTrimDb   = apvts.getRawParameterValue ("inTrim")->load();
    const int   st1Type    = static_cast<int> (apvts.getRawParameterValue ("stage1Type")->load());
    const float st1Drive   = (apvts.getRawParameterValue ("stage1Drive")->load() * 0.01f) * rangeScale;
    const int   st2Type    = static_cast<int> (apvts.getRawParameterValue ("stage2Type")->load());
    const float st2Drive   = (apvts.getRawParameterValue ("stage2Drive")->load() * 0.01f) * rangeScale;
    const int   st3Type    = static_cast<int> (apvts.getRawParameterValue ("stage3Type")->load());
    const float st3Drive   = (apvts.getRawParameterValue ("stage3Drive")->load() * 0.01f) * rangeScale;

    const int   eqPos      = static_cast<int> (apvts.getRawParameterValue ("eqPos")->load());
    const float lowFreq    = apvts.getRawParameterValue ("lowFreq")->load();
    const float lowGainDb  = apvts.getRawParameterValue ("lowGain")->load();
    const float highFreq   = apvts.getRawParameterValue ("highFreq")->load();
    const float highGainDb = apvts.getRawParameterValue ("highGain")->load();

    const float outTrimDb  = apvts.getRawParameterValue ("outTrim")->load();
    const float mixPct     = apvts.getRawParameterValue ("mix")->load() * 0.01f;

    inGainSmoothed.setTargetValue (juce::Decibels::decibelsToGain (inTrimDb));
    outGainSmoothed.setTargetValue (juce::Decibels::decibelsToGain (outTrimDb));
    mixSmoothed.setTargetValue (mixPct);
    driveSmoothed[0].setTargetValue (st1Drive);
    driveSmoothed[1].setTargetValue (st2Drive);
    driveSmoothed[2].setTargetValue (st3Drive);

    updateBaxandallFilters (osRate, lowFreq, lowGainDb, highFreq, highGainDb);

    float blockInPeak = 0.0f;
    for (int ch = 0; ch < numChannels; ++ch)
        blockInPeak = std::max (blockInPeak, buffer.getMagnitude (ch, 0, numSamples));
    meterInDb.store (juce::Decibels::gainToDecibels (blockInPeak, -100.0f));

    auto& activeOs = *oversamplers[static_cast<size_t> (currentOsIndex)];
    juce::dsp::AudioBlock<float> block (buffer);
    auto osBlock = activeOs.processSamplesUp (block);
    const int osSamples = static_cast<int> (osBlock.getNumSamples());

    float* chL = osBlock.getChannelPointer (0);
    float* chR = (numChannels > 1) ? osBlock.getChannelPointer (1) : chL;

    const bool eqActive = (std::abs (lowGainDb) > 0.01f || std::abs (highGainDb) > 0.01f);
    auto applyEqSample = [&] (float& sL, float& sR)
    {
        if (! eqActive)
            return;
        sL = highShelfL.processSample (lowShelfL.processSample (sL));
        if (numChannels > 1)
            sR = highShelfR.processSample (lowShelfR.processSample (sR));
    };

    for (int i = 0; i < osSamples; ++i)
    {
        const float curInGain  = inGainSmoothed.getNextValue();
        const float curOutGain = outGainSmoothed.getNextValue();
        const float curMix     = mixSmoothed.getNextValue();
        const float d1         = driveSmoothed[0].getNextValue();
        const float d2         = driveSmoothed[1].getNextValue();
        const float d3         = driveSmoothed[2].getNextValue();

        const float dryL = chL[i];
        const float dryR = chR[i];

        float wetL = dryL * curInGain;
        float wetR = dryR * curInGain;

        if (eqPos == 0)
            applyEqSample (wetL, wetR);

        wetL = processTopologySample (wetL, st1Type, d1, is2xDrive, osRate, stageStates[0], true);
        if (numChannels > 1)
            wetR = processTopologySample (wetR, st1Type, d1, is2xDrive, osRate, stageStates[0], false);

        if (eqPos == 1)
            applyEqSample (wetL, wetR);

        wetL = processTopologySample (wetL, st2Type, d2, is2xDrive, osRate, stageStates[1], true);
        if (numChannels > 1)
            wetR = processTopologySample (wetR, st2Type, d2, is2xDrive, osRate, stageStates[1], false);

        if (eqPos == 2)
            applyEqSample (wetL, wetR);

        wetL = processTopologySample (wetL, st3Type, d3, is2xDrive, osRate, stageStates[2], true);
        if (numChannels > 1)
            wetR = processTopologySample (wetR, st3Type, d3, is2xDrive, osRate, stageStates[2], false);

        if (eqPos == 3)
            applyEqSample (wetL, wetR);

        wetL *= curOutGain;
        wetR *= curOutGain;

        chL[i] = dryL + curMix * (wetL - dryL);
        if (numChannels > 1)
            chR[i] = dryR + curMix * (wetR - dryR);
    }

    activeOs.processSamplesDown (block);

    float blockOutPeak = 0.0f;
    for (int ch = 0; ch < numChannels; ++ch)
        blockOutPeak = std::max (blockOutPeak, buffer.getMagnitude (ch, 0, numSamples));
    meterOutDb.store (juce::Decibels::gainToDecibels (blockOutPeak, -100.0f));
}

juce::AudioProcessorEditor* MapleAudioProcessor::createEditor()
{
    return new MapleAudioProcessorEditor (*this);
}

void MapleAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void MapleAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MapleAudioProcessor();
}