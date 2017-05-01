/*
 ==============================================================================
 
 Parts of the file were auto-generated! It contains the basic framework code for a JUCE plugin processor.
 Additons can be added to the fucntion not implemented.
 
 ==============================================================================
 */

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/** Our audio processing class directly inherets from the JUCE AudioProcessor with the addition of tempo
 ** This class interfaces with the JUCE classes and allow audio processing
 ** We override some existing implementations and virtual functions
 */
class HummingbirdAudioProcessor  : public AudioProcessor
{
public:
    float tempo;
    
    //Using default constructor and destructor
    HummingbirdAudioProcessor();
    ~HummingbirdAudioProcessor();
    
    //Called before playback starts, to let the filter prepare itself.
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    //Called after playback has stopped, to let the filter free up any resources it no longer needs.
    void releaseResources() override;
    
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
    
    //Renders the next block.
    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;
    
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    //==============================================================================
    const String getName() const override;
    
    //Returns true if the processor wants midi messages.
    bool acceptsMidi() const override;
    //Returns true if the processor produces midi messages.
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;
    
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    
    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HummingbirdAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
