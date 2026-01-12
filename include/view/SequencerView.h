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
    std::vector<UserAction> render(const SequencerState& state);
    
    /// Select a step for highlighting
    void setSelectedStep(int step) { selectedStep = step; }
    
    int getSelectedStep() const { return selectedStep; }
    
private:
    int selectedStep = -1;
    int densityValue = 50;  // 0-100, default 50%
};
