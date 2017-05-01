#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <aubio/aubio.h>
#include <iostream>
#include <fstream>
#include "helper.hpp"
#include <cmath>


//==============================================================================
/** Our Microphone Class is a subclass of the AudioAppComponent class that is used by JUCE
 ** Microphone configures the microphone for recording
 ** and specifies the input and output like bitrate, chunksize
 ** This class is also responsible for the pitch detection and midi note storage
 */
class Microphone : public AudioAppComponent {
public:
    MidiOutput* midiDevice;
    fvec_t *input;
    fvec_t *output;
    aubio_pitch_t* detector;
    int success;
    int CHUNK_SIZE = 1024;
    int BITRATE = 16100;
    std::vector<int> midi_nums;
    
    std::string CURRENT_NOTE;
    int CURRENT_OCTAVE;
    
    Microphone(Microphone& m) {
        
    }
    
    Microphone() {
        
    }
    
    //Sets base line for pitch detection and silences
    Microphone(TextEditor* t)
    {
        midiDevice = MidiOutput::openDevice(0);
        messageBox = t;
        setAudioChannels(1, 0);
        
        input = new_fvec(CHUNK_SIZE/4);
        output = new_fvec(1);
        detector = new_aubio_pitch("default", CHUNK_SIZE, CHUNK_SIZE/4, BITRATE);
        
        success = aubio_pitch_set_unit(detector, "Hz");
        if (success != 0) {
            postMessageToList(MidiMessage(0, 60, float(127)),
                              "Pitch Detection Unit not set properly");
            return;
        }
        
        success = aubio_pitch_set_silence(detector, -40);
        if (success != 0) {
            postMessageToList(MidiMessage(0, 60, float(127)),
                              "Silence not set properly");
            return;
        }
        
    }
    
    ~Microphone()
    {
        shutdownAudio();
    }
    
    void sendMicMessage() {
        MidiMessage m(0, 60, float(127));
        postMessageToList(m, "Mic check 1 2 1 2");
    }
    
    void prepareToPlay(int /*samplesPerBlockExpected*/, double /*sampleRate*/) override
    {
    }
    
    void releaseResources() override
    {
    }
    
    
    //Each audio block is analyzed and run through pitch detection
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
    {
        
        AudioIODevice* device = deviceManager.getCurrentAudioDevice();
        const BigInteger activeInputChannels = device->getActiveInputChannels();
        const BigInteger activeOutputChannels = device->getActiveOutputChannels();
        const int maxInputChannels = activeInputChannels.getHighestBit() + 1;
        const int maxOutputChannels = activeOutputChannels.getHighestBit() + 1;
        
        float* inBuffer = new float[CHUNK_SIZE/4];
        String message = "Entering getNextAudioBlock";
        std::ofstream myfile;
        myfile.open("log.txt", std::ios_base::app);
        
        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            inBuffer = (float*)bufferToFill.buffer->getReadPointer(
                                                                   channel,
                                                                   bufferToFill.startSample);
            
            // Perform Aubio Pitch Detection on buffer
            input->data = inBuffer;
            aubio_pitch_do(detector, input, output);
            
            // output isn't set?
            smpl_t freq = fvec_get_sample(output, 0);
            if (freq > 0) {
                myfile << "Freq: " << freq << std::endl;
                }
                
                helper help = closestNote(freq);
                
                // Get closest note given frequency
                double base = help.getBaseFrequency();
                int octave = help.getClosestOctave();
                CURRENT_NOTE = help.getClosestNote();
                CURRENT_OCTAVE = octave;
                
                if (base == 0) {
                    continue;
                }
                
                // Convert Note Number to MIDI input
                double detected_freq = base*(pow(2, octave));
                float note_number;
                if (detected_freq > 0) {
                    note_number = 12 * log2(detected_freq / 440.0) + 69;
                }
                else {
                    myfile << "wadfh" << std::endl;
                    note_number = 0;
                }
                int midi_num = (int)round(note_number);
                
                // Send message to virtual MIDI instrument if pitch is changing
                if (midi_nums.size() == 0) {
                    MidiMessage m(MidiMessage::noteOn(0x90, midi_num, (uint8)127));
                    midiDevice->sendMessageNow(m);
                    postMessageToList(m, " Voice Detection.");
                    
                    midi_nums.push_back(midi_num);
                }
                else if (midi_nums.size() > 0 && midi_nums[midi_nums.size() - 1] != midi_num) {
                    MidiMessage noteToggleOffMessage(MidiMessage::noteOff(0x80, midi_nums[midi_nums.size() - 1], (uint8)127));
                    midiDevice->sendMessageNow(noteToggleOffMessage);
                    
                    MidiMessage noteToggleOnMessage(MidiMessage::noteOn(0x90, midi_num, (uint8)127));
                    midiDevice->sendMessageNow(noteToggleOnMessage);
                    postMessageToList(noteToggleOnMessage, " New Note Freq: "+std::to_string(freq));
                    
                    midi_nums.push_back(midi_num);
                }
                }
                myfile.close();
                }

private:
    TextEditor* messageBox;
    double startTime;
    
    //inner class is a member of the Microphone class
    //Used for midi note processing
    class IncomingMessageCallback : public CallbackMessage
    {
    public:
        IncomingMessageCallback(Microphone* o, const MidiMessage& m, const String& s)
        : owner(o), message(m), source(s)
        {}
        
        void messageCallback() override
        {
            if (owner != nullptr)
                owner->addMessageToList(message, source);
        }
        
        Component::SafePointer<Microphone> owner;
        MidiMessage message;
        String source;
    };
    
    void addMessageToList(const MidiMessage& message, const String& source)
    {
        const double time = message.getTimeStamp() - startTime;
        
        const int hours = ((int)(time / 3600.0)) % 24;
        const int minutes = ((int)(time / 60.0)) % 60;
        const int seconds = ((int)time) % 60;
        const int millis = ((int)(time * 1000.0)) % 1000;
        
        const String timecode(String::formatted("%02d:%02d:%02d.%03d",
                                                hours,
                                                minutes,
                                                seconds,
                                                millis));
        
        const String description(getMidiMessageDescription(message));
        
        const String midiMessageString(timecode + "  -  " + description + " (" + source + ")"); // [7]
        logMessage(midiMessageString);
    }
    
    void postMessageToList(const MidiMessage& message, const String& source)
    {
        (new IncomingMessageCallback(this, message, source))->post();
    }
    
    static String getMidiMessageDescription(const MidiMessage& m)
    {
        if (m.isNoteOn())           return "Note on " + MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
        if (m.isNoteOff())          return "Note off " + MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
        if (m.isProgramChange())    return "Program change " + String(m.getProgramChangeNumber());
        if (m.isPitchWheel())       return "Pitch wheel " + String(m.getPitchWheelValue());
        if (m.isAftertouch())       return "After touch " + MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3) + ": " + String(m.getAfterTouchValue());
        if (m.isChannelPressure())  return "Channel pressure " + String(m.getChannelPressureValue());
        if (m.isAllNotesOff())      return "All notes off";
        if (m.isAllSoundOff())      return "All sound off";
        if (m.isMetaEvent())        return "Meta event";
        
        if (m.isController())
        {
            String name(MidiMessage::getControllerName(m.getControllerNumber()));
            
            if (name.isEmpty())
                name = "[" + String(m.getControllerNumber()) + "]";
            
            return "Controller " + name + ": " + String(m.getControllerValue());
        }
        
        return String::toHexString(m.getRawData(), m.getRawDataSize());
    }
    
    void logMessage(const String& m)
    {
        messageBox->moveCaretToEnd();
        messageBox->insertTextAtCaret(m + newLine);
    }
};
                
                
//==============================================================================
/** Our Metronome Class is also a subclass of the AudioAppComponent class that is used by JUCE
** Microphone creates a metronome when the VST is activated allowing for a more accurate tempo
*/
class Metronome : public AudioAppComponent {
public:
    AudioFormatReader* reader;
    Metronome(Metronome& m){
        
    }
    Metronome() {
        setAudioChannels(0, 2);
        formatManager.registerBasicFormats();
        File file("metronome.wav");
        reader = formatManager.createReaderFor(file);
        if (reader != nullptr)
        {
            ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource(reader, true); // [11]
            transportSource.setSource(newSource, 0, nullptr, reader->sampleRate);                         // [12]                                                                  // [13]
            readerSource = newSource.release();                                                            // [14]
        }
    }
    ~Metronome() {
        shutdownAudio();
    }
    
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override {
        transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    }
    void releaseResources() override {
        transportSource.releaseResources();
        
    }
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override {
        if (readerSource == nullptr)
        {
            bufferToFill.clearActiveBufferRegion();
            return;
        }
        
        transportSource.getNextAudioBlock(bufferToFill);
    }
    
    AudioFormatManager formatManager;
    ScopedPointer<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
};
                
                class HummingbirdAudioProcessorEditor  :
                public virtual AudioProcessorEditor,
                private Slider::Listener,
                public Button::Listener,
                private Timer,
                private MidiInputCallback,
                private MidiKeyboardStateListener
                
                {
                public:
                    HummingbirdAudioProcessorEditor (HummingbirdAudioProcessor&);
                    ~HummingbirdAudioProcessorEditor();
                    
                    //==============================================================================
                    void paint (Graphics&) override;
                    void resized() override;
                    void buttonClicked (Button* button) override;
                    void sliderValueChanged (Slider* slider);
                    void timerCallback() override;
                    
                    MidiKeyboardState keyboardState;            // [5]
                    MidiKeyboardComponent keyboardComponent;    // [6]
                    
                    TextEditor midiMessagesBox;
                    double startTime;
                    
                    
                    
                private:
                    Metronome metronome;
                    Microphone* microphone;
                    ImageButton recordingLight;
                    Slider tempoMeter;
                    TextButton startRecordingButton;
                    Label recordingLabel;
                    
                    MidiFile midiFile;
                    MidiMessageSequence trackSequence;
                    
                    bool isRecording = false; 
                    
                    // This reference is provided as a quick way for your editor to
                    // access the processor object that created it.
                    HummingbirdAudioProcessor& processor;
                    
                    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HummingbirdAudioProcessorEditor)
                    
                    
                    // These methods handle callbacks from the midi device + on-screen keyboard..
                    void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message) override
                    {
                        bool isAddingFromMidiInput(false);
                        const ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
                        keyboardState.processNextMidiEvent(message);
                        postMessageToList(message, source->getName());
                    }
                    
                    void handleNoteOn(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
                    {
                        MidiMessage m(MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity));
                        m.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
                        MidiOutput* device = MidiOutput::openDevice(0);
                        device->sendMessageNow(m);
                        postMessageToList(m, "Voice data sent to "+MidiOutput::getDevices()[0]);
                        microphone->sendMicMessage();
                        
                        trackSequence.addEvent(m, Time::getMillisecondCounterHiRes());
                    }
                    
                    void handleNoteOff(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/) override
                    {
                        MidiMessage m(MidiMessage::noteOff(midiChannel, midiNoteNumber));
                        m.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
                        postMessageToList(m, "Voice");
                        
                        trackSequence.addEvent(m, Time::getMillisecondCounterHiRes());
                    }
                    
                    static String getMidiMessageDescription(const MidiMessage& m)
                    {
                        if (m.isNoteOn())           return "Note on " + MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
                        if (m.isNoteOff())          return "Note off " + MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
                        if (m.isProgramChange())    return "Program change " + String(m.getProgramChangeNumber());
                        if (m.isPitchWheel())       return "Pitch wheel " + String(m.getPitchWheelValue());
                        if (m.isAftertouch())       return "After touch " + MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3) + ": " + String(m.getAfterTouchValue());
                        if (m.isChannelPressure())  return "Channel pressure " + String(m.getChannelPressureValue());
                        if (m.isAllNotesOff())      return "All notes off";
                        if (m.isAllSoundOff())      return "All sound off";
                        if (m.isMetaEvent())        return "Meta event";
                        
                        if (m.isController())
                        {
                            String name(MidiMessage::getControllerName(m.getControllerNumber()));
                            
                            if (name.isEmpty())
                                name = "[" + String(m.getControllerNumber()) + "]";
                            
                            return "Controller " + name + ": " + String(m.getControllerValue());
                        }
                        
                        return String::toHexString(m.getRawData(), m.getRawDataSize());
                    }
                    
                    void logMessage(const String& m)
                    {
                        midiMessagesBox.moveCaretToEnd();
                        midiMessagesBox.insertTextAtCaret(m + newLine);
                    }
                    
                    // This is used to dispach an incoming message to the message thread
                    class IncomingMessageCallback : public CallbackMessage
                    {
                    public:
                        IncomingMessageCallback(HummingbirdAudioProcessorEditor* o, const MidiMessage& m, const String& s)
                        : owner(o), message(m), source(s)
                        {}
                        
                        void messageCallback() override
                        {
                            if (owner != nullptr)
                                owner->addMessageToList(message, source);
                        }
                        
                        Component::SafePointer<HummingbirdAudioProcessorEditor> owner;
                        MidiMessage message;
                        String source;
                    };
                    
                    void postMessageToList(const MidiMessage& message, const String& source)
                    {
                        (new IncomingMessageCallback(this, message, source))->post();
                    }
                    
                    void addMessageToList(const MidiMessage& message, const String& source)
                    {
                        const double time = message.getTimeStamp() - startTime;
                        
                        const int hours = ((int)(time / 3600.0)) % 24;
                        const int minutes = ((int)(time / 60.0)) % 60;
                        const int seconds = ((int)time) % 60;
                        const int millis = ((int)(time * 1000.0)) % 1000;
                        
                        const String timecode(String::formatted("%02d:%02d:%02d.%03d",
                                                                hours,
                                                                minutes,
                                                                seconds,
                                                                millis));
                        
                        const String description(getMidiMessageDescription(message));
                        
                        const String midiMessageString(timecode + "  -  " + description + " (" + source + ")"); // [7]
                        logMessage(midiMessageString);
                    }
                };
                
                
#endif  // PLUGINEDITOR_H_INCLUDED
