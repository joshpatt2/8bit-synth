#pragma once

#include "SynthParams.h"
#include <vector>
#include <string>

struct SequencerStep {
    bool active = false;
    int soundSlot = 0;  // Which sound to trigger
};

struct SequencerPattern {
    std::vector<SequencerStep> steps;
    int numSteps = 16;  // 1 bar = 16 steps
    float bpm = 120.0f;
    bool loop = true;
    
    SequencerPattern() {
        steps.resize(32);  // Max 2 bars
    }
};

struct SoundSlot {
    SynthParams params;
    std::string name;
    bool enabled = true;
};

class Sequencer {
public:
    Sequencer() {
        // Initialize with some default sounds
        slots.resize(4);
        slots[0].name = "Kick";
        slots[1].name = "Snare";
        slots[2].name = "Hat";
        slots[3].name = "Blip";
    }
    
    SequencerPattern pattern;
    std::vector<SoundSlot> slots;
    
    int currentStep = -1;  // Currently playing step
    int selectedStep = -1; // User-selected step for editing
    bool isPlaying = false;
    
    float getStepDuration() const {
        // Duration of one step in seconds
        // 16 steps per bar, 4 beats per bar = 4 steps per beat
        float beatsPerSecond = pattern.bpm / 60.0f;
        float secondsPerBeat = 1.0f / beatsPerSecond;
        float stepsPerBeat = 4.0f;  // 16th notes
        return secondsPerBeat / stepsPerBeat;
    }
};
