#pragma once

#include <cstdint>
#include <string>
#include "FxParams.h"

enum class Waveform {
    Square,
    Triangle,
    Sawtooth,
    Noise
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
    
    // Stretch features
    float vibratoFreq = 0.0f;   // Hz, 0 = off
    float vibratoDepth = 0.0f;  // 0-1
    
    // Effects
    FxParams effects;
    
    std::string name = "Untitled";
};
