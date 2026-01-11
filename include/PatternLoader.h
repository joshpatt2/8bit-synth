#pragma once

#include "Sequencer.h"
#include <fstream>
#include <sstream>
#include <string>

class PatternLoader {
public:
    static bool loadPattern(const std::string& filename, SequencerPattern& pattern) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Clear existing pattern
        for (auto& step : pattern.steps) {
            step.active = false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Parse BPM
            if (line.find("BPM:") == 0) {
                pattern.bpm = std::stof(line.substr(4));
                continue;
            }
            
            // Parse STEPS
            if (line.find("STEPS:") == 0) {
                pattern.numSteps = std::stoi(line.substr(6));
                continue;
            }
            
            // Parse step:slot pattern (e.g., "4:1")
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                try {
                    int step = std::stoi(line.substr(0, colonPos));
                    int slot = std::stoi(line.substr(colonPos + 1));
                    
                    if (step >= 0 && step < 32 && slot >= 0 && slot < 4) {
                        pattern.steps[step].active = true;
                        pattern.steps[step].soundSlot = slot;
                    }
                } catch (...) {
                    // Skip malformed lines
                    continue;
                }
            }
        }
        
        file.close();
        return true;
    }
    
    static bool savePattern(const std::string& filename, const SequencerPattern& pattern) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# Saved Pattern\n";
        file << "BPM: " << pattern.bpm << "\n";
        file << "STEPS: " << pattern.numSteps << "\n\n";
        
        for (int i = 0; i < pattern.numSteps; i++) {
            if (pattern.steps[i].active) {
                file << i << ":" << pattern.steps[i].soundSlot << "\n";
            }
        }
        
        file.close();
        return true;
    }
};
