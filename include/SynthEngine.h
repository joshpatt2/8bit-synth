#pragma once

#include "SynthParams.h"
#include "Oscillator.h"
#include "Envelope.h"
#include "Effects.h"
#include <vector>
#include <cmath>

class SynthEngine {
public:
    static constexpr int SAMPLE_RATE = 44100;
    
    // Render a complete sound effect to a buffer
    static std::vector<float> render(const SynthParams& params) {
        // Calculate total duration including release
        float totalDuration = params.duration + params.release;
        int numSamples = static_cast<int>(totalDuration * SAMPLE_RATE);
        
        std::vector<float> buffer(numSamples);
        
        float phase = 0.0f;
        float currentFreq = params.startFreq;
        
        for (int i = 0; i < numSamples; i++) {
            float time = static_cast<float>(i) / SAMPLE_RATE;
            
            // Frequency sweep (linear interpolation)
            float freqProgress = time / params.duration;
            if (freqProgress > 1.0f) freqProgress = 1.0f;
            
            // Apply slide speed to the interpolation
            float slideT = std::pow(freqProgress, 1.0f / (params.slideSpeed + 0.1f));
            currentFreq = params.startFreq + (params.endFreq - params.startFreq) * slideT;
            
            // Add vibrato if enabled
            float vibratoOffset = 0.0f;
            if (params.vibratoFreq > 0.0f) {
                vibratoOffset = std::sin(time * params.vibratoFreq * 2.0f * M_PI) 
                              * params.vibratoDepth * currentFreq;
            }
            float finalFreq = currentFreq + vibratoOffset;
            
            // Generate waveform sample
            float sample = Oscillator::generateSample(params.waveform, phase, params.dutyCycle);
            
            // Apply envelope
            float envelope = Envelope::getValue(params, time, params.duration);
            sample *= envelope;
            
            // Advance phase
            phase += finalFreq / SAMPLE_RATE;
            phase = fmod(phase, 1.0f);  // Wrap phase to 0-1
            
            buffer[i] = sample;
        }
        
        // Apply effects processing
        Effects::reset();
        buffer = Effects::process(buffer, params.effects);
        
        return buffer;
    }
    
    // Convert float samples (-1 to 1) to 16-bit PCM
    static std::vector<int16_t> floatToInt16(const std::vector<float>& floatBuffer) {
        std::vector<int16_t> intBuffer(floatBuffer.size());
        for (size_t i = 0; i < floatBuffer.size(); i++) {
            // Clamp and convert
            float sample = floatBuffer[i];
            if (sample > 1.0f) sample = 1.0f;
            if (sample < -1.0f) sample = -1.0f;
            intBuffer[i] = static_cast<int16_t>(sample * 32767.0f);
        }
        return intBuffer;
    }
};
