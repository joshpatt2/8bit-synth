#pragma once

#include "Song.h"
#include "SynthEngine.h"
#include <vector>

class SongEngine {
public:
    static std::vector<float> renderSong(const Song& song, const std::vector<SynthParams>& soundSlots) {
        if (song.arrangement.empty()) {
            return std::vector<float>();
        }
        
        std::vector<float> songBuffer;
        
        // Render each pattern in the arrangement
        for (int patternIdx : song.arrangement) {
            if (patternIdx < 0 || patternIdx >= static_cast<int>(song.patterns.size())) {
                continue;
            }
            
            const SequencerPattern& pattern = song.patterns[patternIdx];
            
            // Calculate pattern duration
            float beatDuration = 60.0f / pattern.bpm;
            float stepDuration = beatDuration / 4.0f;  // 4 steps per beat
            float patternDuration = stepDuration * pattern.numSteps;
            int numSamples = static_cast<int>(patternDuration * 44100.0f);
            
            std::vector<float> patternBuffer(numSamples, 0.0f);
            
            // Render all active steps in this pattern
            for (int step = 0; step < pattern.numSteps; step++) {
                const SequencerStep& stepData = pattern.steps[step];
                
                if (stepData.active && stepData.soundSlot >= 0 && stepData.soundSlot < static_cast<int>(soundSlots.size())) {
                    // Render this sound
                    auto sound = SynthEngine::render(soundSlots[stepData.soundSlot]);
                    
                    // Mix into pattern buffer at the correct time
                    int startSample = static_cast<int>(step * stepDuration * 44100.0f);
                    mixIntoBuffer(patternBuffer, sound, startSample);
                }
            }
            
            // Append pattern to song
            songBuffer.insert(songBuffer.end(), patternBuffer.begin(), patternBuffer.end());
        }
        
        // Normalize to prevent clipping
        normalizeBuffer(songBuffer);
        
        return songBuffer;
    }
    
private:
    static void mixIntoBuffer(std::vector<float>& buffer, const std::vector<float>& sound, int startSample) {
        for (size_t i = 0; i < sound.size() && startSample + i < static_cast<int>(buffer.size()); i++) {
            buffer[startSample + i] += sound[i];
        }
    }
    
    static void normalizeBuffer(std::vector<float>& buffer) {
        // Find peak
        float peak = 0.0f;
        for (float sample : buffer) {
            if (std::abs(sample) > peak) {
                peak = std::abs(sample);
            }
        }
        
        // Normalize if clipping
        if (peak > 1.0f) {
            float scale = 0.95f / peak;
            for (float& sample : buffer) {
                sample *= scale;
            }
        }
    }
};
