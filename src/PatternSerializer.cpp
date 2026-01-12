#include "PatternSerializer.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

using json = nlohmann::json;
namespace fs = std::filesystem;

PatternSerializer::PatternSerializer() {
    patternsDir = getPatternsDirectory();
}

std::string PatternSerializer::getPatternsDirectory() const {
    // Get home directory
    const char* home = std::getenv("HOME");
    if (!home) {
        home = "/tmp";
    }
    
    std::string dir = std::string(home) + "/Documents/8bit-synth/patterns";
    return dir;
}

std::string PatternSerializer::ensurePatternsDirectory() {
    std::string dir = getPatternsDirectory();
    
    try {
        if (!fs::exists(dir)) {
            fs::create_directories(dir);
            std::cout << "Created patterns directory: " << dir << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to create patterns directory: " << e.what() << std::endl;
    }
    
    return dir;
}

bool PatternSerializer::savePattern(const PatternFile& pattern, const std::string& filepath) {
    try {
        // Ensure directory exists
        ensurePatternsDirectory();
        
        std::string jsonStr = patternToJson(pattern);
        
        // Write to file
        std::ofstream file(filepath);
        if (!file) {
            std::cerr << "Failed to open file for writing: " << filepath << std::endl;
            return false;
        }
        
        file << jsonStr;
        file.close();
        
        std::cout << "Saved pattern to: " << filepath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving pattern: " << e.what() << std::endl;
        return false;
    }
}

PatternFile PatternSerializer::loadPattern(const std::string& filepath) const {
    PatternFile pattern;
    
    try {
        std::ifstream file(filepath);
        if (!file) {
            std::cerr << "Failed to open file for reading: " << filepath << std::endl;
            return pattern;
        }
        
        std::string jsonStr((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        return jsonToPattern(jsonStr, filepath);
    } catch (const std::exception& e) {
        std::cerr << "Error loading pattern: " << e.what() << std::endl;
        return pattern;
    }
}

bool PatternSerializer::deletePattern(const std::string& filepath) {
    try {
        if (fs::exists(filepath)) {
            fs::remove(filepath);
            std::cout << "Deleted pattern: " << filepath << std::endl;
            return true;
        } else {
            std::cerr << "File not found: " << filepath << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error deleting pattern: " << e.what() << std::endl;
        return false;
    }
}

std::vector<PatternFile> PatternSerializer::listPatterns() const {
    std::vector<PatternFile> patterns;
    
    try {
        std::string dir = getPatternsDirectory();
        
        if (!fs::exists(dir)) {
            return patterns;
        }
        
        // Iterate through .8bp files
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (entry.path().extension() == ".8bp") {
                PatternFile pattern = loadPattern(entry.path().string());
                if (!pattern.name.empty()) {
                    patterns.push_back(pattern);
                }
            }
        }
        
        // Sort by creation date (newest first)
        std::sort(patterns.begin(), patterns.end(),
                 [](const PatternFile& a, const PatternFile& b) {
                     return a.getCreatedTime() > b.getCreatedTime();
                 });
    } catch (const std::exception& e) {
        std::cerr << "Error listing patterns: " << e.what() << std::endl;
    }
    
    return patterns;
}

std::string PatternSerializer::patternToJson(const PatternFile& pattern) const {
    json j;
    
    // Metadata
    j["name"] = pattern.name;
    j["version"] = pattern.version;
    j["createdAt"] = pattern.createdAt;
    j["author"] = pattern.author;
    
    // Synth parameters
    j["synth"]["waveform"] = static_cast<int>(pattern.synthParams.waveform);
    j["synth"]["dutyCycle"] = pattern.synthParams.dutyCycle;
    j["synth"]["startFreq"] = pattern.synthParams.startFreq;
    j["synth"]["endFreq"] = pattern.synthParams.endFreq;
    j["synth"]["slideSpeed"] = pattern.synthParams.slideSpeed;
    j["synth"]["attack"] = pattern.synthParams.attack;
    j["synth"]["decay"] = pattern.synthParams.decay;
    j["synth"]["sustain"] = pattern.synthParams.sustain;
    j["synth"]["release"] = pattern.synthParams.release;
    j["synth"]["duration"] = pattern.synthParams.duration;
    j["synth"]["vibratoFreq"] = pattern.synthParams.vibratoFreq;
    j["synth"]["vibratoDepth"] = pattern.synthParams.vibratoDepth;
    
    // Effects (stored in synthParams.effects)
    j["effects"]["reverbTime"] = pattern.synthParams.effects.reverbTime;
    j["effects"]["reverbMix"] = pattern.synthParams.effects.reverbMix;
    j["effects"]["reverbEnabled"] = pattern.synthParams.effects.reverbEnabled;
    j["effects"]["delayTime"] = pattern.synthParams.effects.delayTime;
    j["effects"]["delayFeedback"] = pattern.synthParams.effects.delayFeedback;
    j["effects"]["delayMix"] = pattern.synthParams.effects.delayMix;
    j["effects"]["delayEnabled"] = pattern.synthParams.effects.delayEnabled;
    
    // Sequencer state
    j["sequencer"]["bpm"] = pattern.sequencerState.pattern.bpm;
    j["sequencer"]["numSteps"] = pattern.sequencerState.pattern.numSteps;
    j["sequencer"]["stepsData"] = json::array();
    
    for (const auto& step : pattern.sequencerState.pattern.steps) {
        json stepJson;
        stepJson["active"] = step.active;
        stepJson["soundSlot"] = step.soundSlot;
        j["sequencer"]["stepsData"].push_back(stepJson);
    }
    
    // Sequencer slots (sound library)
    j["sequencer"]["slots"] = json::array();
    for (const auto& slot : pattern.sequencerState.slots) {
        json slotJson;
        slotJson["name"] = slot.name;
        slotJson["enabled"] = slot.enabled;
        j["sequencer"]["slots"].push_back(slotJson);
    }
    
    return j.dump(2);
}

PatternFile PatternSerializer::jsonToPattern(const std::string& jsonStr, const std::string& filepath) const {
    PatternFile pattern;
    pattern.filepath = filepath;
    
    try {
        json j = json::parse(jsonStr);
        
        // Metadata
        pattern.name = j.value("name", "Untitled");
        pattern.version = j.value("version", "1.0");
        pattern.createdAt = j.value("createdAt", "");
        pattern.author = j.value("author", "User");
        
        // Synth parameters
        if (j.contains("synth")) {
            auto& synth = j["synth"];
            pattern.synthParams.waveform = static_cast<Waveform>(synth.value("waveform", 0));
            pattern.synthParams.dutyCycle = synth.value("dutyCycle", 0.5f);
            pattern.synthParams.startFreq = synth.value("startFreq", 440.0f);
            pattern.synthParams.endFreq = synth.value("endFreq", 440.0f);
            pattern.synthParams.slideSpeed = synth.value("slideSpeed", 0.0f);
            pattern.synthParams.attack = synth.value("attack", 0.01f);
            pattern.synthParams.decay = synth.value("decay", 0.1f);
            pattern.synthParams.sustain = synth.value("sustain", 0.7f);
            pattern.synthParams.release = synth.value("release", 0.1f);
            pattern.synthParams.duration = synth.value("duration", 0.5f);
            pattern.synthParams.vibratoFreq = synth.value("vibratoFreq", 0.0f);
            pattern.synthParams.vibratoDepth = synth.value("vibratoDepth", 0.0f);
        }
        
        // Effects
        if (j.contains("effects")) {
            auto& effects = j["effects"];
            pattern.synthParams.effects.reverbTime = effects.value("reverbTime", 0.5f);
            pattern.synthParams.effects.reverbMix = effects.value("reverbMix", 0.3f);
            pattern.synthParams.effects.reverbEnabled = effects.value("reverbEnabled", true);
            pattern.synthParams.effects.delayTime = effects.value("delayTime", 0.3f);
            pattern.synthParams.effects.delayFeedback = effects.value("delayFeedback", 0.4f);
            pattern.synthParams.effects.delayMix = effects.value("delayMix", 0.2f);
            pattern.synthParams.effects.delayEnabled = effects.value("delayEnabled", true);
        }
        
        // Sequencer state
        if (j.contains("sequencer")) {
            auto& seq = j["sequencer"];
            pattern.sequencerState.pattern.bpm = seq.value("bpm", 120.0f);
            pattern.sequencerState.pattern.numSteps = seq.value("numSteps", 16);
            
            // Load steps
            if (seq.contains("stepsData") && seq["stepsData"].is_array()) {
                pattern.sequencerState.pattern.steps.clear();
                for (const auto& stepJson : seq["stepsData"]) {
                    SequencerStep step;
                    step.active = stepJson.value("active", false);
                    step.soundSlot = stepJson.value("soundSlot", 0);
                    pattern.sequencerState.pattern.steps.push_back(step);
                }
            }
            
            // Load slots
            if (seq.contains("slots") && seq["slots"].is_array()) {
                pattern.sequencerState.slots.clear();
                for (const auto& slotJson : seq["slots"]) {
                    SoundSlot slot;
                    slot.name = slotJson.value("name", "");
                    slot.enabled = slotJson.value("enabled", true);
                    pattern.sequencerState.slots.push_back(slot);
                }
            }
        }
        
        return pattern;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return pattern;
    }
}

// Helper methods on PatternFile
std::string PatternFile::getShortDate() const {
    if (createdAt.empty()) return "";
    
    try {
        // Parse ISO 8601 string (YYYY-MM-DDTHH:MM:SS)
        std::tm tm = {};
        std::istringstream ss(createdAt);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        
        char buf[32];
        std::strftime(buf, sizeof(buf), "%b %d", &tm);
        return std::string(buf);
    } catch (...) {
        return "";
    }
}

std::time_t PatternFile::getCreatedTime() const {
    if (createdAt.empty()) return 0;
    
    try {
        std::tm tm = {};
        std::istringstream ss(createdAt);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        return std::mktime(&tm);
    } catch (...) {
        return 0;
    }
}
