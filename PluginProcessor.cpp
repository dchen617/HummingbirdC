/*
 ==============================================================================
 
Parts of the file were auto-generated! It contains the basic framework code for a JUCE plugin processor.
 Additons can be added to the fucntion not implemented. See .h file for function explainations

 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
/** Our audio processing class directly inherets from the JUCE AudioProcessor with the addition of tempo
 ** This class interfaces with the JUCE classes and allow audio processing
 ** We override some existing implementations and virtual functions
 ** See .h file for function explainations
 */
HummingbirdAudioProcessor::HummingbirdAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", AudioChannelSet::stereo(), true)
#endif
                  )
#endif
{
}

HummingbirdAudioProcessor::~HummingbirdAudioProcessor()
{
}

//==============================================================================
const String HummingbirdAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

//bool to determine if the plguin uses midi input
bool HummingbirdAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

//bool to determine if the plguin uses midi output
bool HummingbirdAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

double HummingbirdAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HummingbirdAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int HummingbirdAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HummingbirdAudioProcessor::setCurrentProgram (int index)
{
}

const String HummingbirdAudioProcessor::getProgramName (int index)
{
    return String();
}

void HummingbirdAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void HummingbirdAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void HummingbirdAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HummingbirdAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;
    
    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    
    return true;
#endif
}
#endif

void HummingbirdAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);
        
        // ..do something to the data...
    }
}

//==============================================================================
bool HummingbirdAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* HummingbirdAudioProcessor::createEditor()
{
    return new HummingbirdAudioProcessorEditor (*this);
}

//==============================================================================
void HummingbirdAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void HummingbirdAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HummingbirdAudioProcessor();
}
