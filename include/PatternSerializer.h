#pragma once

#include <string>
#include <vector>
#include <memory>
#include "model/PatternFile.h"

/// Handles JSON serialization/deserialization of pattern files.
/// Patterns are stored as .8bp files in ~/Documents/8bit-synth/patterns/
class PatternSerializer {
public:
    PatternSerializer();
    
    /// Save pattern to JSON file
    /// @param pattern The pattern to save
    /// @param filepath Full path to save to (e.g., ~/.../patterns/name.8bp)
    /// @return true if successful
    bool savePattern(const PatternFile& pattern, const std::string& filepath);
    
    /// Load pattern from JSON file
    /// @param filepath Full path to load from
    /// @return Loaded pattern, or empty pattern if failed
    PatternFile loadPattern(const std::string& filepath) const;
    
    /// Delete pattern file
    /// @param filepath Full path to delete
    /// @return true if successful
    bool deletePattern(const std::string& filepath);
    
    /// List all available patterns in the patterns directory
    /// @return Vector of PatternFile objects with minimal data (metadata only)
    std::vector<PatternFile> listPatterns() const;
    
    /// Ensure patterns directory exists, create if needed
    /// @return Directory path used
    std::string ensurePatternsDirectory();
    
private:
    std::string patternsDir;
    
    /// Get default patterns directory path
    std::string getPatternsDirectory() const;
    
    /// Convert PatternFile to JSON string
    std::string patternToJson(const PatternFile& pattern) const;
    
    /// Convert JSON string to PatternFile
    PatternFile jsonToPattern(const std::string& json, const std::string& filepath) const;
};
