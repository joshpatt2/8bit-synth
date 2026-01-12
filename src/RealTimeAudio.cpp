#include "RealTimeAudio.h"
#include "Oscillator.h"
#include "Envelope.h"
#include <cmath>

RealTimeAudio::RealTimeAudio() {
    // Setup desired audio format
    SDL_zero(desiredSpec);
    desiredSpec.freq = SAMPLE_RATE;
    desiredSpec.format = AUDIO_F32SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = BUFFER_SIZE;
    desiredSpec.callback = audioCallback;
    desiredSpec.userdata = this;
    
    // Initialize params
    currentParams = SynthParams();
    targetParams = SynthParams();
}

RealTimeAudio::~RealTimeAudio() {
    stop();
}

void RealTimeAudio::start() {
    if (deviceId != 0) return;  // Already running
    
    deviceId = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, &obtainedSpec, 0);
    if (deviceId == 0) {
        SDL_Log("Failed to open audio device: %s", SDL_GetError());
        return;
    }
    
    running = true;
    SDL_PauseAudioDevice(deviceId, 0);  // Start playback
}

void RealTimeAudio::stop() {
    if (deviceId == 0) return;
    
    running = false;
    SDL_PauseAudioDevice(deviceId, 1);
    SDL_CloseAudioDevice(deviceId);
    deviceId = 0;
}

void RealTimeAudio::updateParameters(const SynthParams& params) {
    paramBuffer.writeUpdate(params);
}

void RealTimeAudio::audioCallback(void* userdata, Uint8* stream, int len) {
    RealTimeAudio* self = static_cast<RealTimeAudio*>(userdata);
    float* floatStream = reinterpret_cast<float*>(stream);
    int numSamples = len / sizeof(float);
    
    self->processAudio(floatStream, numSamples);
}

void RealTimeAudio::processAudio(float* outputBuffer, int numSamples) {
    // Check for new parameter updates
    SynthParams newParams;
    if (paramBuffer.readLatestUpdate(newParams)) {
        targetParams = newParams;
    }
    
    // Generate samples
    for (int i = 0; i < numSamples; i++) {
        // Smooth parameters toward target
        smoothParameter(currentParams.attack, targetParams.attack);
        smoothParameter(currentParams.decay, targetParams.decay);
        smoothParameter(currentParams.sustain, targetParams.sustain);
        smoothParameter(currentParams.release, targetParams.release);
        smoothParameter(currentParams.startFreq, targetParams.startFreq);
        
        // Waveform can change instantly
        if (currentParams.waveform != targetParams.waveform) {
            currentParams.waveform = targetParams.waveform;
        }
        
        // Generate waveform sample
        float sample = Oscillator::generateSample(currentParams.waveform, phase, currentParams.dutyCycle);
        
        // Apply simple envelope (attack/sustain state)
        // For continuous play, we treat the synthesizer as always "playing"
        float envelope = 0.5f + (currentParams.sustain * 0.5f);  // Sustain level
        sample *= envelope;
        
        // Advance phase
        phase += currentParams.startFreq / SAMPLE_RATE;
        if (phase >= 1.0f) {
            phase -= 1.0f;
        }
        
        outputBuffer[i] = sample;
    }
}

void RealTimeAudio::smoothParameter(float& current, float target) {
    if (current == target) return;
    
    float diff = target - current;
    if (diff > 0) {
        current += diff * SMOOTHING_RATE;
        if (current >= target) current = target;
    } else {
        current += diff * SMOOTHING_RATE;
        if (current <= target) current = target;
    }
}
