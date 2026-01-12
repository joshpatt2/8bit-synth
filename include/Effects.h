#pragma once

#include "FxParams.h"
#include <vector>
#include <cstring>

class Reverb {
public:
    Reverb() : sampleRate(44100.0f) {
        // Initialize comb filters (using common comb filter sizes)
        combSizes = {1116, 1188, 1277, 1356};
        combBuffers.resize(4);
        combIndices.resize(4, 0);
        
        for (int i = 0; i < 4; i++) {
            combBuffers[i].resize(combSizes[i], 0.0f);
        }
        
        // Allpass filters (diffusers)
        allpassSizes = {556, 441, 341, 225};
        allpassBuffers.resize(4);
        allpassIndices.resize(4, 0);
        allpassFeedback.resize(4, 0.5f);
        
        for (int i = 0; i < 4; i++) {
            allpassBuffers[i].resize(allpassSizes[i], 0.0f);
        }
    }
    
    float process(float sample, const FxParams& params) {
        if (!params.reverbEnabled || params.reverbMix < 0.001f) {
            return sample;
        }
        
        // Calculate decay based on reverbTime
        float decay = 0.84f + (params.reverbTime * 0.15f);  // 0.84 to 0.99
        
        // Process through comb filters
        float combOut = 0.0f;
        for (int i = 0; i < 4; i++) {
            float& idx = combIndices[i];
            float& buffer = combBuffers[i][static_cast<int>(idx)];
            
            float filterOut = buffer * decay;
            combOut += filterOut;
            
            buffer = sample + (filterOut * 0.5f);
            idx = fmod(idx + 1.0f, static_cast<float>(combSizes[i]));
        }
        
        // Process through allpass filters for diffusion
        float allpassOut = combOut;
        for (int i = 0; i < 4; i++) {
            float& idx = allpassIndices[i];
            int intIdx = static_cast<int>(idx);
            float& buffer = allpassBuffers[i][intIdx];
            
            float bufferOut = buffer;
            float filterOut = bufferOut * allpassFeedback[i] + allpassOut;
            
            buffer = allpassOut + (bufferOut * -allpassFeedback[i]);
            allpassOut = filterOut;
            
            idx = fmod(idx + 1.0f, static_cast<float>(allpassSizes[i]));
        }
        
        // Dry/wet mix
        return sample * (1.0f - params.reverbMix) + allpassOut * params.reverbMix;
    }
    
private:
    float sampleRate;
    std::vector<int> combSizes;
    std::vector<std::vector<float>> combBuffers;
    std::vector<float> combIndices;
    
    std::vector<int> allpassSizes;
    std::vector<std::vector<float>> allpassBuffers;
    std::vector<float> allpassIndices;
    std::vector<float> allpassFeedback;
};

class Delay {
public:
    Delay() : sampleRate(44100.0f), maxDelayTime(1.0f) {
        // Maximum 1 second delay at 44100 Hz
        int maxSamples = static_cast<int>(sampleRate * maxDelayTime);
        delayBuffer.resize(maxSamples, 0.0f);
        readIndex = 0;
        writeIndex = 0;
    }
    
    float process(float sample, const FxParams& params) {
        if (!params.delayEnabled || params.delayMix < 0.001f) {
            return sample;
        }
        
        // Calculate delay samples
        float delaySamples = params.delayTime * sampleRate;
        if (delaySamples < 1.0f) delaySamples = 1.0f;
        
        int maxSamples = delayBuffer.size();
        readIndex = static_cast<int>(writeIndex - delaySamples + maxSamples) % maxSamples;
        
        // Read delayed sample
        float delayed = delayBuffer[readIndex];
        
        // Write new sample with feedback
        float feedback = params.delayFeedback;
        if (feedback > 0.9f) feedback = 0.9f;  // Cap to prevent explosion
        
        delayBuffer[writeIndex] = sample + (delayed * feedback);
        writeIndex = (writeIndex + 1) % maxSamples;
        
        // Dry/wet mix
        return sample * (1.0f - params.delayMix) + delayed * params.delayMix;
    }
    
private:
    float sampleRate;
    float maxDelayTime;
    std::vector<float> delayBuffer;
    int readIndex;
    int writeIndex;
};

class Effects {
public:
    static std::vector<float> process(const std::vector<float>& samples, const FxParams& params) {
        std::vector<float> output = samples;
        
        // Process through effects chain
        for (size_t i = 0; i < output.size(); i++) {
            // Apply delay first
            output[i] = delayProcessor.process(output[i], params);
            
            // Then reverb
            output[i] = reverbProcessor.process(output[i], params);
            
            // Soft clipping to prevent digital clipping
            if (output[i] > 1.0f) output[i] = 1.0f;
            if (output[i] < -1.0f) output[i] = -1.0f;
        }
        
        return output;
    }
    
    static void reset() {
        reverbProcessor = Reverb();
        delayProcessor = Delay();
    }
    
private:
    static Reverb reverbProcessor;
    static Delay delayProcessor;
};
