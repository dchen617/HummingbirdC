//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Tue Jan  8 10:08:15 PST 2002
// Last Modified: Mon Feb  9 21:24:41 PST 2015 Updated for C++11.
// Filename:      ...sig/doc/examples/all/createmidifile/createmidifile.cpp
// Syntax:        C++
//
// Description:   Demonstration of how to create a Multi-track MIDI file.
//
//
//Modified by David Chen 4/10/17

#include "MidiFile.h"
#include <iostream>

using namespace std;

typedef unsigned char uchar;

///////////////////////////////////////////////////////////////////////////

void makeMidi(int melody[], int mrhythm[], string filename){
    MidiFile outputfile;        // create an empty MIDI file with one track
    outputfile.absoluteTicks();  // time information stored as absolute time
    // (will be coverted to delta time when written)
    outputfile.addTrack(1);     // Add another two tracks to the MIDI file
    vector<uchar> midievent;     // temporary storage for MIDI events
    midievent.resize(3);        // set the size of the array to 3 bytes
    int tpq = 120;              // default value in MIDI file is 48
    outputfile.setTicksPerQuarterNote(tpq);
    
    // data to write to MIDI file: (60 = middle C)
    // C5 C  G G A A G-  F F  E  E  D D C-
    
    // store a melody line in track 1 (track 0 left empty for conductor info)
    int i=0;
    int actiontime = 0;      // temporary storage for MIDI event time
    midievent[2] = 64;       // store attack/release velocity for note command
    while (melody[i] >= 0) {
        midievent[0] = 0x90;     // store a note on command (MIDI channel 1)
        midievent[1] = melody[i];
        outputfile.addEvent(1, actiontime, midievent);
        actiontime += tpq * mrhythm[i];
        midievent[0] = 0x80;     // store a note on command (MIDI channel 1)
        outputfile.addEvent(1, actiontime, midievent);
        i++;
    }
    
    outputfile.sortTracks();         // make sure data is in correct order
    outputfile.write(filename+".mid"); // write Standard MIDI File twinkle.mid
}

int main(int argc, char** argv) {
    int melody[50]  = {72,72,79,79,81,81,79,77,77,76,76,74,74,72,-1};
    int mrhythm[50] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1};
    string filename = "twinkle2";
    makeMidi(melody, mrhythm, filename);
    return 0;
}


/*  FUNCTIONS available in the MidiFile class:

void absoluteTime(void);
   Set the time information to absolute time.
int addEvent(int aTrack, int aTime, vector<uchar>& midiData);
   Add an event to the end of a MIDI track.
int addTrack(void);
   Add an empty track to the MIDI file.
int addTrack(int count);
   Add a number of empty tracks to the MIDI file.
void deltaTime(void);
   Set the time information to delta time.
void deleteTrack(int aTrack);
   remove a track from the MIDI file.
void erase(void);
   Empty the contents of the MIDI file, leaving one track with no data.
MFEvent& getEvent(int aTrack, int anIndex);
   Return a MIDI event from the Track.
int getTimeState(void);
   Indicates if the timestate is TIME_STATE_ABSOLUTE or TIME_STATE_DELTA.
int getTrackState(void);
   Indicates if the tracks are being processed as multiple tracks or
   as a single track.
int getTicksPerQuarterNote(void);
   Returns the ticks per quarter note value from the MIDI file.
int getTrackCount(void);
   Returns the number of tracks in the MIDI file.
int getNumTracks(void);
   Alias for getTrackCount();
int getNumEvents(int aTrack);
   Return the number of events present in the given track.
void joinTracks(void);
   Merge all tracks together into one track.  This function is reversable,
   unlike mergeTracks().
void mergeTracks(int aTrack1, int aTrack2);
   Combine the two tracks into a single track stored in Track1.  Track2
   is deleted.
int read(char* aFile);
   Read the contents of a MIDI file into the MidiFile class data structure
void setTicksPerQuarterNote    (int ticks);
   Set the MIDI file's ticks per quarter note information
void sortTrack(vector<MFEvent>& trackData);
   If in absolute time, sort particular track into correct time order.
void sortTracks(void);
   If in absolute time, sort tracks into correct time order.

*/



