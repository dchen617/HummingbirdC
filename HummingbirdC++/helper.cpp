//
//  helper.cpp
//  HummingbirdC++
//
//  Created by David Chen on 4/17/17.
//  Copyright © 2017 David Chen. All rights reserved.
//

#include "helper.hpp"

#include <iostream>
#include <map>
#include <cmath>
#include <cfenv>

using namespace std;

class helper{
    string closest_note;
    int closest_octave;
    double base_frequency;
public:
    helper(string, int, double);
    
};

helper::helper(string a, int b, double c){
    closest_note = a;
    closest_octave = b;
    base_frequency = c;
}

map<string, double> generateKeys(){
    map<string, double> frequencies;
    frequencies["NA"] = 0.00;
    frequencies["C"] = 16.35;
    frequencies["C#"] = 17.32;
    frequencies["D"] = 18.35;
    frequencies["D#"] = 19.45;
    frequencies["E"] = 20.60;
    frequencies["F"] = 21.83;
    frequencies["F#"] = 23.12;
    frequencies["G"] = 24.50;
    frequencies["G#"] = 25.96;
    frequencies["A"] = 27.50;
    frequencies["A#"] = 29.14;
    frequencies["B"] = 30.87;
    return frequencies;
}

helper closestNote(int note){
    if (note<0){
        helper help("NA", 0, 0);
        return help;
    }
    
    int minimum_diff = -1;
    string closest_note = "NA";
    int closest_octave = 0;
    map<string, double> frequencies = generateKeys();
    for (map<string,double>::iterator it=frequencies.begin(); it!=frequencies.end(); ++it){
        if (it->first== "NA"){
            continue;
        }
        double roundedDiv = round(note/frequencies[it->first]);
        double realDiv =note/frequencies[it->first];
        double diff = abs(realDiv-roundedDiv);
        
        int octave = round(log2(diff));
        if (diff < minimum_diff || minimum_diff == -1){
            minimum_diff = diff;
            closest_octave = octave;
            closest_note = it->first;
        }
    }
    
    helper help(closest_note, closest_octave, frequencies[closest_note]);
    return help;
}

