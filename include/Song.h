#pragma once

#include "Sequencer.h"
#include <vector>

struct Song {
    std::vector<SequencerPattern> patterns;  // Pattern library
    std::vector<int> arrangement;             // Sequence of pattern indices
    std::string name = "Untitled Song";
    
    Song() {
        // Start with empty arrangement
    }
    
    void addPatternToArrangement(int patternIndex) {
        if (patternIndex >= 0 && patternIndex < static_cast<int>(patterns.size())) {
            arrangement.push_back(patternIndex);
        }
    }
    
    void removePatternFromArrangement(int positionInArrangement) {
        if (positionInArrangement >= 0 && positionInArrangement < static_cast<int>(arrangement.size())) {
            arrangement.erase(arrangement.begin() + positionInArrangement);
        }
    }
    
    float getTotalDuration(float bpm) const {
        if (arrangement.empty()) return 0.0f;
        
        float totalDuration = 0.0f;
        for (int patternIdx : arrangement) {
            if (patternIdx >= 0 && patternIdx < static_cast<int>(patterns.size())) {
                const auto& pattern = patterns[patternIdx];
                // Each step is 1/16th of a beat at the pattern's BPM
                float beatDuration = 60.0f / pattern.bpm;  // Seconds per beat
                float patternDuration = (pattern.numSteps / 4.0f) * beatDuration;
                totalDuration += patternDuration;
            }
        }
        return totalDuration;
    }
    
    int getTotalSteps() const {
        int totalSteps = 0;
        for (int patternIdx : arrangement) {
            if (patternIdx >= 0 && patternIdx < static_cast<int>(patterns.size())) {
                totalSteps += patterns[patternIdx].numSteps;
            }
        }
        return totalSteps;
    }
};
