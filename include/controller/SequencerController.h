#pragma once

#include <chrono>
#include <vector>

#include "model/SynthModel.h"
#include "model/UserAction.h"
#include "SynthEngine.h"
#include "AudioOutput.h"

/// Controller for sequencer playback and pattern editing.
/// Handles sequencer timing, step triggering, and pattern management.
class SequencerController {
public:
    SequencerController();
    
    /// Handle a user action
    void handleAction(const UserAction& action);
    
    /// Update sequencer state (called each frame to advance sequencer)
    void update();
    
    /// Get current sequencer state (const reference to model)
    const SequencerState& getState() const { return sequencerState; }
    
    /// Get mutable sequencer state for editing
    SequencerState& getStateMutable() { return sequencerState; }
    
    /// Check if sequencer is currently playing
    bool isPlaying() const { return sequencerState.isPlaying; }
    
    /// Start playback
    void play();
    
    /// Stop playback
    void stop();
    
    /// Toggle a step on/off
    void toggleStep(int stepIndex, int slotIndex = 0);
    
    /// Clear all steps in pattern
    void clearPattern();
    
    /// Select a step for editing
    void selectStep(int stepIndex);
    
    /// Select a sound slot
    void selectSlot(int slotIndex);
    
    /// Update a slot's parameters
    void updateSlotParams(int slotIndex, const SynthParams& params);
    
    /// Get the currently selected slot's parameters
    const SynthParams& getSelectedSlotParams() const;
    
    SynthParams& getSelectedSlotParamsMutable();
    
    /// Randomization modes
    enum class RandomizeMode {
        All,           // Randomly toggle every step
        Some,          // Add notes to ~30% of empty steps
        Remove,        // Remove notes from ~50% of active steps
        Shuffle,       // Shuffle active steps randomly
        Density,       // Increase density (add notes to existing pattern)
        Slots          // Randomize which sound slot plays (keep pattern structure)
    };
    
    /// Randomize pattern with specified mode
    void randomizePattern(RandomizeMode mode, int density = 50);
    
private:
    SequencerState sequencerState;
    AudioOutput audioOutput;
    int selectedSlotIndex = 0;
    
    std::chrono::steady_clock::time_point lastStepTime;
    
    /// Trigger sounds for the current step
    void triggerStep();
};
