#pragma once

#include <vector>
#include <string>

#include "model/SynthModel.h"
#include "model/UserAction.h"

/// View for sequencer UI.
/// Renders step grid, transport controls, and pattern editor.
/// Emits UserActions when user interacts with controls.
class SequencerView {
public:
    SequencerView() = default;
    
    /// Render sequencer UI and return any user actions
    /// @param state Current sequencer state
    /// @param embedded If true, renders without window wrapper for use in parent containers
    std::vector<UserAction> render(const SequencerState& state, bool embedded = false);
    
    /// Select a step for highlighting
    void setSelectedStep(int step) { selectedStep = step; }
    
    int getSelectedStep() const { return selectedStep; }
    
private:
    int selectedStep = -1;
};
