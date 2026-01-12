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
    std::vector<UserAction> render(const SongState& state);
    
private:
    char songNameBuffer[256] = "Untitled Song";
};
