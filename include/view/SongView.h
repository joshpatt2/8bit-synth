#pragma once

#include <vector>
#include <string>

#include "model/SynthModel.h"
#include "model/UserAction.h"
#include "model/PatternFile.h"

/// View for song composer UI.
/// Renders pattern library, song arrangement, and export controls.
/// Emits UserActions when user interacts with controls.
class SongView {
public:
    SongView() = default;
    
    /// Render song composer UI and return any user actions
    /// @param state Current song state
    /// @param embedded If true, renders without window wrapper for use in parent containers
    std::vector<UserAction> render(const SongState& state, bool embedded = false);
    
    /// Update available patterns for display
    void setAvailablePatterns(const std::vector<PatternFile>& patterns);
    
private:
    char songNameBuffer[256] = "Untitled Song";
    char patternNameBuffer[256] = "";
    
    // Pattern UI state
    std::vector<PatternFile> availablePatterns;
    int selectedPatternIndex = -1;
    
    bool showSavePatternDialog = false;
    bool showLoadPatternDialog = false;
    bool showDeleteConfirm = false;
    
    void renderPatternLibrarySection();
    void renderSavePatternDialog(std::vector<UserAction>& actions);
    void renderLoadPatternDialog(std::vector<UserAction>& actions);
    void renderDeleteConfirmDialog(std::vector<UserAction>& actions);
};
