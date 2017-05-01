/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
HummingbirdAudioProcessorEditor::HummingbirdAudioProcessorEditor (HummingbirdAudioProcessor& p)
: AudioProcessorEditor (&p), processor (p),
keyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard),
startTime(Time::getMillisecondCounterHiRes() * 0.001)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    // setSize (200, 200);
    
    midiFile = MidiFile();
    trackSequence = MidiMessageSequence();
    
    startRecordingButton.setButtonText ("Record");
    startRecordingButton.addListener(this);
    AudioProcessorEditor::addAndMakeVisible(startRecordingButton);
    startRecordingButton.setBounds(10, 30, 190, 30);
    
    recordingLabel.setColour(Label::backgroundColourId, Colours::white);
    recordingLabel.setColour(Label::textColourId, Colours::red);
    recordingLabel.setJustificationType(Justification::centred);
    AudioProcessorEditor::addAndMakeVisible(recordingLabel);
    recordingLabel.setBounds(220, 30, 70, 30);
    
    AudioProcessorEditor::setSize(600,490);
    
    // these define the parameters of our slider object
    tempoMeter.setSliderStyle (Slider::LinearHorizontal);
    tempoMeter.setRange(60.0, 240.0, 1.0);
    tempoMeter.setTextBoxStyle (Slider::NoTextBox, false, 200, 100);
    // tempoMeter.setPopupDisplayEnabled (true, AudioProcessorEditor::this);
    tempoMeter.setTextValueSuffix (" BPM");
    tempoMeter.setValue(60.0);
    processor.tempo = 60.0;
    
    tempoMeter.setBounds(410, 30, 150, 30);
    
    //// this function adds the slider to the editor
    AudioProcessorEditor::addAndMakeVisible(&tempoMeter);
    tempoMeter.addListener (this);
    
    //initialize the metronome
    Metronome metronome();
    microphone = new Microphone(&midiMessagesBox);
    
    AudioProcessorEditor::addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);
    keyboardComponent.setBounds(30, 80, 550, 100);
    
    //setting the attributes of the messageBox
    AudioProcessorEditor::addAndMakeVisible(midiMessagesBox);
    midiMessagesBox.setMultiLine(true);
    midiMessagesBox.setReturnKeyStartsNewLine(true);
    midiMessagesBox.setReadOnly(true);
    midiMessagesBox.setScrollbarsShown(true);
    midiMessagesBox.setCaretVisible(false);
    midiMessagesBox.setPopupMenuEnabled(true);
    midiMessagesBox.setColour(TextEditor::backgroundColourId, Colour(0x32ffffff));
    midiMessagesBox.setColour(TextEditor::outlineColourId, Colour(0x1c000000));
    midiMessagesBox.setColour(TextEditor::shadowColourId, Colour(0x16000000));
    midiMessagesBox.setBounds(30, 190, 550, 300);
    
    AudioProcessorEditor::setSize(600, 400);
    
    
    
}

HummingbirdAudioProcessorEditor::~HummingbirdAudioProcessorEditor()
{
    startRecordingButton.removeListener(this);
    keyboardState.removeListener(this);
}

//==============================================================================
//Basic UI generation
void HummingbirdAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);
    
    g.setColour (Colours::black);
    g.setFont (15.0f);
    g.drawFittedText ("Hummingbird", 0, 0, AudioProcessorEditor::getWidth(), 30, Justification::centred, 1);
}

void HummingbirdAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // tempoMeter.setBounds (40, 30, 20, getHeight() - 60);
}

//This function starts the recording on botton click
void HummingbirdAudioProcessorEditor::buttonClicked (Button* button)
{
    if (button == &startRecordingButton)
    {
        if (isRecording == true) {
            isRecording = false;
            const String labelString = "";
            recordingLabel.setText(labelString, dontSendNotification);
            stopTimer();
            midiFile.addTrack(trackSequence);
            
            File myFile = File::createFileWithoutCheckingPath("testFile.mid");
            FileOutputStream myStream(myFile);
            midiFile.writeTo(myStream);
        }
        else {
            isRecording = true;
            const String labelString = "Recording";
            recordingLabel.setText(labelString, dontSendNotification);
            startTimer(1000*60 / tempoMeter.getValue());
        }
    }
}

//This function allows the slider to set the tempo of the recording
void HummingbirdAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    processor.tempo = tempoMeter.getValue();
    stopTimer();
    startTimer(1000 * 60.0 / tempoMeter.getValue());
}

//callback function
void HummingbirdAudioProcessorEditor::timerCallback() {
    if (!isRecording) return;
    recordingLabel.setText((String)tempoMeter.getValue() + " opened", dontSendNotification);
    metronome.prepareToPlay(441, metronome.reader->sampleRate);
    metronome.transportSource.stop();
    metronome.transportSource.setPosition(0.0);
    handleNoteOn( (&keyboardState), 0, 120%(int)(tempoMeter.getValue()), 127);
    metronome.transportSource.start();
}



