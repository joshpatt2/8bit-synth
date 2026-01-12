#pragma once

#include <vector>
#include <string>

#include "model/SynthModel.h"
#include "model/UserAction.h"

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
    
private:
    char songNameBuffer[256] = "Untitled Song";
};
