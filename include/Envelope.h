#pragma once

#include "SynthParams.h"

class Envelope {
public:
    // Get envelope value (0-1) at a given time in the sound
    static float getValue(const SynthParams& params, float time, float duration) {
        float totalTime = params.attack + params.decay;
        float releaseStart = duration;
        
        if (time < params.attack) {
            // Attack phase: ramp up from 0 to 1
            return time / params.attack;
        } 
        else if (time < totalTime) {
            // Decay phase: drop from 1 to sustain level
            float decayTime = time - params.attack;
            float decayProgress = decayTime / params.decay;
            return 1.0f - (1.0f - params.sustain) * decayProgress;
        }
        else if (time < releaseStart) {
            // Sustain phase: hold at sustain level
            return params.sustain;
        }
        else {
            // Release phase: fade from sustain to 0
            float releaseTime = time - releaseStart;
            if (releaseTime >= params.release) {
                return 0.0f;
            }
            float releaseProgress = releaseTime / params.release;
            return params.sustain * (1.0f - releaseProgress);
        }
    }
};
