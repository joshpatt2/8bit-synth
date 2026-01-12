#pragma once

#include <chrono>
#include <vector>
#include <string>

#include "model/SynthModel.h"
#include "model/UserAction.h"
#include "SynthEngine.h"
#include "AudioOutput.h"
#include "Presets.h"

/// Controller for synthesizer sound generation and playback.
/// Handles user actions from the view and updates model state.
/// Manages audio rendering and playback.
class SynthController {
public:
    SynthController();
    
    /// Handle a user action
    void handleAction(const UserAction& action);
    
    /// Get current synth parameters (const reference to model)
    const SynthParams& getParams() const { return params; }
    
    /// Get mutable parameters for editing
    SynthParams& getParamsMutable() { return params; }
    
    /// Check if audio is currently playing
    bool isAudioPlaying() const { return audioOutput.isPlaying(); }
    
    /// Play current sound
    void playSound();
    
    /// Stop current sound
    void stopSound();
    
    /// Export current sound to WAV file
    std::string exportToWav(const std::string& filepath);
    
    /// Get list of available presets
    static std::vector<std::string> getPresetNames();
    
    /// Load a preset by name
    void loadPreset(const std::string& name);
    
    /// Randomize current parameters
    void randomizeParams();
    
private:
    SynthParams params;
    AudioOutput audioOutput;
    
    /// Helper: Apply a parameter change and re-render if needed
    void updateParam(const UserAction& action);
};
