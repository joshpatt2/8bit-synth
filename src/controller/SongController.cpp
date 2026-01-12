#include "controller/SongController.h"
#include "SongEngine.h"
#include "WavExporter.h"
#include "SynthEngine.h"

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
