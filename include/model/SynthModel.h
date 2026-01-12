#pragma once

#include <cstdint>
#include <string>
#include <vector>

// Pure data structures - no dependencies on SDL, ImGui, or audio frameworks

enum class Waveform {
    Square,
    Triangle,
    Sawtooth,
    Noise
};

struct FxParams {
    // Reverb
    float reverbTime = 0.5f;    // 0-2 seconds (decay time)
    float reverbMix = 0.3f;      // 0-1.0 (dry/wet blend)
    bool reverbEnabled = true;
    
    // Delay
    float delayTime = 0.3f;      // 0-1 second
    float delayFeedback = 0.4f;  // 0-0.9 (prevents exploding)
    float delayMix = 0.2f;       // 0-1.0 (dry/wet blend)
    bool delayEnabled = true;
};

struct SynthParams {
    // Waveform
    Waveform waveform = Waveform::Square;
    float dutyCycle = 0.5f;  // For square wave
    
    // Frequency
    float startFreq = 440.0f;  // Hz
    float endFreq = 440.0f;    // Hz
    float slideSpeed = 0.5f;   // 0-1, how fast frequency changes
    
    // ADSR Envelope
    float attack = 0.01f;   // seconds
    float decay = 0.1f;     // seconds
    float sustain = 0.5f;   // level 0-1
    float release = 0.2f;   // seconds
    
    // Duration
    float duration = 0.5f;  // seconds
    
    // Vibrato
    float vibratoFreq = 0.0f;   // Hz, 0 = off
    float vibratoDepth = 0.0f;  // 0-1
    
    // Effects
    FxParams effects;
    
    std::string name = "Untitled";
};

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

struct SequencerState {
    SequencerPattern pattern;
    std::vector<SoundSlot> slots;
    
    int currentStep = -1;  // Currently playing step (-1 = not playing)
    int selectedStep = -1; // User-selected step for editing
    bool isPlaying = false;
    
    SequencerState() {
        slots.resize(4);
        slots[0].name = "Kick";
        slots[1].name = "Snare";
        slots[2].name = "Hat";
        slots[3].name = "Blip";
    }
    
    float getStepDuration() const {
        // Duration of one step in seconds
        // 16 steps per bar, 4 beats per bar = 4 steps per beat
        float beatsPerSecond = pattern.bpm / 60.0f;
        float secondsPerBeat = 1.0f / beatsPerSecond;
        float stepsPerBeat = 4.0f;  // 16th notes
        return secondsPerBeat / stepsPerBeat;
    }
};

struct SongState {
    std::string name = "Untitled Song";
    std::vector<SequencerPattern> patterns;
    std::vector<SoundSlot> soundLibrary;
    bool modified = false;
};
