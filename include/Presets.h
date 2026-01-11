#pragma once

#include "SynthParams.h"
#include <cstdlib>
#include <ctime>

class Presets {
public:
    static void init() {
        srand(static_cast<unsigned>(time(nullptr)));
    }
    
    static SynthParams laser() {
        SynthParams p;
        p.name = "Laser";
        p.waveform = Waveform::Square;
        p.startFreq = 800.0f;
        p.endFreq = 200.0f;
        p.slideSpeed = 0.3f;
        p.attack = 0.01f;
        p.decay = 0.05f;
        p.sustain = 0.4f;
        p.release = 0.15f;
        p.duration = 0.3f;
        p.dutyCycle = 0.5f;
        return p;
    }
    
    static SynthParams explosion() {
        SynthParams p;
        p.name = "Explosion";
        p.waveform = Waveform::Noise;
        p.startFreq = 1000.0f;
        p.endFreq = 50.0f;
        p.slideSpeed = 0.2f;
        p.attack = 0.01f;
        p.decay = 0.3f;
        p.sustain = 0.1f;
        p.release = 0.4f;
        p.duration = 0.6f;
        return p;
    }
    
    static SynthParams pickup() {
        SynthParams p;
        p.name = "Pickup";
        p.waveform = Waveform::Triangle;
        p.startFreq = 400.0f;
        p.endFreq = 800.0f;
        p.slideSpeed = 0.8f;
        p.attack = 0.01f;
        p.decay = 0.05f;
        p.sustain = 0.6f;
        p.release = 0.1f;
        p.duration = 0.2f;
        return p;
    }
    
    static SynthParams jump() {
        SynthParams p;
        p.name = "Jump";
        p.waveform = Waveform::Square;
        p.startFreq = 300.0f;
        p.endFreq = 600.0f;
        p.slideSpeed = 0.5f;
        p.attack = 0.01f;
        p.decay = 0.08f;
        p.sustain = 0.3f;
        p.release = 0.1f;
        p.duration = 0.25f;
        p.dutyCycle = 0.25f;
        return p;
    }
    
    static SynthParams hurt() {
        SynthParams p;
        p.name = "Hurt";
        p.waveform = Waveform::Sawtooth;
        p.startFreq = 500.0f;
        p.endFreq = 200.0f;
        p.slideSpeed = 0.4f;
        p.attack = 0.01f;
        p.decay = 0.05f;
        p.sustain = 0.5f;
        p.release = 0.2f;
        p.duration = 0.35f;
        p.vibratoFreq = 8.0f;
        p.vibratoDepth = 0.03f;
        return p;
    }
    
    static SynthParams powerup() {
        SynthParams p;
        p.name = "Powerup";
        p.waveform = Waveform::Square;
        p.startFreq = 200.0f;
        p.endFreq = 1200.0f;
        p.slideSpeed = 0.6f;
        p.attack = 0.02f;
        p.decay = 0.1f;
        p.sustain = 0.7f;
        p.release = 0.3f;
        p.duration = 0.5f;
        p.dutyCycle = 0.5f;
        return p;
    }

    // 808-style kick drum
    static SynthParams kick808() {
        SynthParams p;
        p.name = "808 Kick";
        p.waveform = Waveform::Triangle;  // Smooth, sine-like
        p.startFreq = 150.0f;             // Start with punch
        p.endFreq = 45.0f;                // Drop to sub-bass
        p.slideSpeed = 0.15f;             // Fast pitch drop
        p.attack = 0.005f;                // Punchy transient
        p.decay = 0.4f;                   // Long boom tail
        p.sustain = 0.0f;                 // Percussive, no sustain
        p.release = 0.3f;                 // Fade out smoothly
        p.duration = 0.5f;                // Let it breathe
        return p;
    }

    // Snappier 808 for faster tempos
    static SynthParams kick808Short() {
        SynthParams p;
        p.name = "808 Short";
        p.waveform = Waveform::Triangle;
        p.startFreq = 180.0f;             // Higher punch
        p.endFreq = 50.0f;
        p.slideSpeed = 0.1f;              // Even faster drop
        p.attack = 0.003f;
        p.decay = 0.2f;                   // Tighter
        p.sustain = 0.0f;
        p.release = 0.15f;
        p.duration = 0.25f;               // Snappy
        return p;
    }

    static SynthParams randomize() {
        SynthParams p;
        p.name = "Random";
        
        // Random waveform
        int waveformIndex = rand() % 4;
        p.waveform = static_cast<Waveform>(waveformIndex);
        
        // Random frequencies (50-2000 Hz)
        p.startFreq = 50.0f + static_cast<float>(rand() % 1950);
        p.endFreq = 50.0f + static_cast<float>(rand() % 1950);
        
        // Random slide speed
        p.slideSpeed = static_cast<float>(rand() % 100) / 100.0f;
        
        // Random envelope
        p.attack = 0.001f + static_cast<float>(rand() % 100) / 1000.0f;
        p.decay = 0.05f + static_cast<float>(rand() % 300) / 1000.0f;
        p.sustain = static_cast<float>(rand() % 100) / 100.0f;
        p.release = 0.05f + static_cast<float>(rand() % 400) / 1000.0f;
        
        // Random duration (0.1 - 1.0 seconds)
        p.duration = 0.1f + static_cast<float>(rand() % 90) / 100.0f;
        
        // Random duty cycle for square wave
        p.dutyCycle = 0.1f + static_cast<float>(rand() % 80) / 100.0f;
        
        // Sometimes add vibrato
        if (rand() % 3 == 0) {
            p.vibratoFreq = 3.0f + static_cast<float>(rand() % 12);
            p.vibratoDepth = 0.01f + static_cast<float>(rand() % 9) / 100.0f;
        }
        
        return p;
    }
};
