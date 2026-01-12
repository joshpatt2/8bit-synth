#include "controller/SongController.h"
#include "SongEngine.h"
#include "WavExporter.h"
#include "SynthEngine.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cctype>

SongController::SongController() {
    songState.name = "Untitled Song";
}

void SongController::handleAction(const UserAction& action) {
    switch (action.type) {
        case UserActionType::SaveSong:
            saveSong(action.filepath);
            break;
        case UserActionType::LoadSong:
            loadSong(action.filepath);
            break;
        case UserActionType::NewSong:
            newSong();
            break;
        case UserActionType::SavePattern:
            // Note: Save pattern requires synth and sequencer state from other controllers
            // This is handled in SongView
            break;
        case UserActionType::LoadPattern:
            // Note: Load pattern requires synth and sequencer state from other controllers
            // This is handled in SongView
            break;
        case UserActionType::DeletePattern:
            deletePattern(action.filepath);
            break;
        default:
            break;
    }
}

void SongController::addPatternToLibrary(const SequencerPattern& pattern) {
    songState.patterns.push_back(pattern);
    songState.modified = true;
}

void SongController::addPatternToArrangement(int patternIndex) {
    if (patternIndex >= 0 && patternIndex < static_cast<int>(songState.patterns.size())) {
        // This would require an arrangement vector in SongState
        // For now, this is a placeholder
        songState.modified = true;
    }
}

void SongController::removePatternFromArrangement(int arrangementIndex) {
    // Placeholder for arrangement management
    songState.modified = true;
}

std::string SongController::exportSong(const std::string& filepath, float bpm) {
    // Placeholder - would render all patterns in arrangement to single WAV
    return filepath;
}

std::string SongController::saveSong(const std::string& filepath) {
    // Placeholder - would serialize songState to file
    return filepath;
}

std::string SongController::loadSong(const std::string& filepath) {
    // Placeholder - would deserialize songState from file
    return filepath;
}

void SongController::newSong() {
    songState = SongState();
    songState.name = "Untitled Song";
    songState.modified = false;
}
bool SongController::savePattern(const std::string& patternName, const SynthParams& synthParams,
                                 const SequencerState& sequencerState) {
    // Create pattern file with current timestamp
    PatternFile pattern;
    pattern.name = patternName;
    pattern.version = "1.0";
    pattern.author = "User";
    pattern.synthParams = synthParams;
    pattern.sequencerState = sequencerState;
    
    // Generate ISO 8601 timestamp
    auto now = std::time(nullptr);
    auto tm = std::localtime(&now);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", tm);
    pattern.createdAt = std::string(buf);
    
    // Generate filepath
    std::string dir = patternSerializer.ensurePatternsDirectory();
    // Replace spaces and special chars with underscores for filename
    std::string safeName = patternName;
    for (auto& c : safeName) {
        if (!std::isalnum(c) && c != '-' && c != '_') {
            c = '_';
        }
    }
    std::string filepath = dir + "/" + safeName + ".8bp";
    
    return patternSerializer.savePattern(pattern, filepath);
}

bool SongController::loadPattern(const std::string& patternPath, SynthParams& synthParams,
                                 SequencerState& sequencerState) {
    PatternFile pattern = patternSerializer.loadPattern(patternPath);
    
    if (pattern.name.empty()) {
        return false;
    }
    
    synthParams = pattern.synthParams;
    sequencerState = pattern.sequencerState;
    return true;
}

bool SongController::deletePattern(const std::string& patternPath) {
    return patternSerializer.deletePattern(patternPath);
}

std::vector<PatternFile> SongController::getAvailablePatterns() const {
    return patternSerializer.listPatterns();
}