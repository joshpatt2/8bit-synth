#pragma once

#include <string>
#include <chrono>
#include "model/SynthModel.h"

/// Data structure representing a saved pattern file.
/// Contains synth parameters, sequencer state, and metadata.
struct PatternFile {
    /// Metadata
    std::string name;                    // "My Cool Pattern"
    std::string version = "1.0";         // Format version for future migration
    std::string createdAt;               // ISO 8601 timestamp
    std::string author = "User";         // Pattern author
    
    /// Audio content
    SynthParams synthParams;             // Current synth settings
    SequencerState sequencerState;       // Current sequencer pattern
    
    /// File path (not serialized, set when loading)
    std::string filepath;
    
    /// Get short date string for display (e.g., "Jan 11")
    std::string getShortDate() const;
    
    /// Get creation time as time_t for sorting
    std::time_t getCreatedTime() const;
};
