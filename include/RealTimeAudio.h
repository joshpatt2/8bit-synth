#pragma once

#include <SDL2/SDL.h>
#include <array>
#include <atomic>
#include <cstring>

#include "SynthParams.h"
#include "SynthEngine.h"

/// Lock-free ring buffer for thread-safe parameter updates.
/// UI thread writes pending parameter changes, audio callback reads atomically.
class ParameterUpdateBuffer {
public:
    static constexpr int BUFFER_SIZE = 512;  // Max parameter updates per audio frame
    
    struct Update {
        SynthParams params;
        bool valid = false;
    };
    
    /// Write a parameter update from UI thread (non-blocking)
    void writeUpdate(const SynthParams& params) {
        Update& slot = updates[writeIndex % BUFFER_SIZE];
        slot.params = params;
        slot.valid = true;
        writeIndex++;
    }
    
    /// Read the latest parameter update in audio callback
    bool readLatestUpdate(SynthParams& outParams) {
        int readIdx = readIndex;
        bool foundUpdate = false;
        
        while (readIdx < writeIndex) {
            Update& slot = updates[readIdx % BUFFER_SIZE];
            if (slot.valid) {
                outParams = slot.params;
                slot.valid = false;
                foundUpdate = true;
            }
            readIdx++;
        }
        
        readIndex = readIdx;
        return foundUpdate;
    }
    
private:
    std::array<Update, BUFFER_SIZE> updates;
    std::atomic<int> writeIndex{0};
    int readIndex = 0;
};

/// Real-time audio synthesizer with parameter smoothing and instant feedback.
class RealTimeAudio {
public:
    static constexpr int SAMPLE_RATE = 44100;
    static constexpr int BUFFER_SIZE = 2048;
    
    RealTimeAudio();
    ~RealTimeAudio();
    
    /// Start real-time audio playback
    void start();
    
    /// Stop real-time audio playback
    void stop();
    
    /// Queue a parameter update (called from UI thread)
    void updateParameters(const SynthParams& params);
    
    /// Get current audio device ID
    SDL_AudioDeviceID getDeviceId() const { return deviceId; }
    
    /// Check if audio is running
    bool isRunning() const { return running; }
    
private:
    SDL_AudioDeviceID deviceId = 0;
    SDL_AudioSpec desiredSpec, obtainedSpec;
    std::atomic<bool> running{false};
    
    ParameterUpdateBuffer paramBuffer;
    
    // Current synth state (accessed only from audio callback)
    SynthParams currentParams;
    float phase = 0.0f;
    
    // Parameter smoothing
    SynthParams targetParams;
    float attackSmoothing = 0.0f;
    float decaySmoothing = 0.0f;
    float sustainSmoothing = 0.0f;
    float releaseSmoothing = 0.0f;
    float freqSmoothing = 0.0f;
    
    static constexpr float SMOOTHING_RATE = 0.005f;  // Smooth over ~200ms
    
    /// Audio callback (called from audio thread)
    static void audioCallback(void* userdata, Uint8* stream, int len);
    
    /// Process one buffer of audio
    void processAudio(float* outputBuffer, int numSamples);
    
    /// Smooth a parameter toward its target value
    void smoothParameter(float& current, float target);
};
