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
    /// @param params Current synth parameters
    /// @param embedded If true, renders without window wrapper for use in parent containers
    std::vector<UserAction> render(const SynthParams& params, bool embedded = false);
    
    /// Get last played sound info
    const std::string& getLastExportPath() const { return lastExportPath; }
    
    void setLastExportPath(const std::string& path) { lastExportPath = path; }
    
    bool isPreviewEnabled() const { return previewEnabled; }
    
private:
    std::string lastExportPath;
    bool previewEnabled = false;
};
