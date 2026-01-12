#pragma once

#include <vector>
#include <string>

#include "model/SynthModel.h"
#include "model/UserAction.h"

/// View for synthesizer UI.
/// Renders synth parameters and effects controls.
/// Emits UserActions when user interacts with controls.
class SynthView {
public:
    SynthView() = default;
    
    /// Render synth UI and return any user actions
    std::vector<UserAction> render(const SynthParams& params);
    
    /// Get last played sound info
    const std::string& getLastExportPath() const { return lastExportPath; }
    
    void setLastExportPath(const std::string& path) { lastExportPath = path; }
    
private:
    std::string lastExportPath;
};
