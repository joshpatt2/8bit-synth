#pragma once

#include "SynthParams.h"
#include <cmath>
#include <cstdlib>

class Oscillator {
public:
    // Generate a sample for the given phase (0-1) and waveform
    static float generateSample(Waveform waveform, float phase, float dutyCycle = 0.5f) {
        switch (waveform) {
            case Waveform::Square:
                return phase < dutyCycle ? 1.0f : -1.0f;
                
            case Waveform::Triangle:
                if (phase < 0.5f) {
                    return -1.0f + 4.0f * phase;  // Rise from -1 to 1
                } else {
                    return 3.0f - 4.0f * phase;   // Fall from 1 to -1
                }
                
            case Waveform::Sawtooth:
                return -1.0f + 2.0f * phase;  // Linear ramp from -1 to 1
                
            case Waveform::Noise:
                // Simple white noise
                return -1.0f + 2.0f * (static_cast<float>(rand()) / RAND_MAX);
        }
        return 0.0f;
    }
};
