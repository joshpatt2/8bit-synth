#pragma once

#include <string>
#include <vector>

#include "model/SynthModel.h"
#include "model/UserAction.h"

/// Controller for song composition and management.
/// Handles pattern library, song arrangement, and song export.
class SongController {
public:
    SongController();
    
    /// Handle a user action
    void handleAction(const UserAction& action);
    
    /// Get current song state
    const SongState& getState() const { return songState; }
    
    /// Get mutable song state
    SongState& getStateMutable() { return songState; }
    
    /// Add pattern to library
    void addPatternToLibrary(const SequencerPattern& pattern);
    
    /// Add pattern to song arrangement by index
    void addPatternToArrangement(int patternIndex);
    
    /// Remove pattern from arrangement
    void removePatternFromArrangement(int arrangementIndex);
    
    /// Export song as WAV file
    std::string exportSong(const std::string& filepath, float bpm = 120.0f);
    
    /// Save song to file
    std::string saveSong(const std::string& filepath);
    
    /// Load song from file
    std::string loadSong(const std::string& filepath);
    
    /// Create new song
    void newSong();
    
private:
    SongState songState;
};
