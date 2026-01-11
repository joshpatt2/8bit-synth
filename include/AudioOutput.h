#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <cstdint>

class AudioOutput {
public:
    AudioOutput() : deviceId(0) {
        SDL_AudioSpec spec;
        spec.freq = 44100;
        spec.format = AUDIO_S16SYS;
        spec.channels = 1;
        spec.samples = 2048;
        spec.callback = nullptr;  // Using queue mode
        
        deviceId = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, 0);
        if (deviceId == 0) {
            SDL_Log("Failed to open audio device: %s", SDL_GetError());
        }
    }
    
    ~AudioOutput() {
        if (deviceId != 0) {
            SDL_CloseAudioDevice(deviceId);
        }
    }
    
    void play(const std::vector<int16_t>& samples) {
        if (deviceId == 0) return;
        
        // Clear any existing audio
        SDL_ClearQueuedAudio(deviceId);
        
        // Queue new audio
        SDL_QueueAudio(deviceId, samples.data(), samples.size() * sizeof(int16_t));
        
        // Start playback
        SDL_PauseAudioDevice(deviceId, 0);
    }
    
    void stop() {
        if (deviceId != 0) {
            SDL_PauseAudioDevice(deviceId, 1);
            SDL_ClearQueuedAudio(deviceId);
        }
    }
    
    bool isPlaying() const {
        if (deviceId == 0) return false;
        return SDL_GetQueuedAudioSize(deviceId) > 0;
    }
    
private:
    SDL_AudioDeviceID deviceId;
};
