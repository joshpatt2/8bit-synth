#include "view/SynthView.h"
#include "view/SynthTheme.h"
#include "imgui.h"
#include <unordered_map>
#include <cstdlib>
#include <algorithm>

// Animation state for smooth transitions
static std::unordered_map<ImGuiID, float> sliderAnimations;

/// Custom slider with filled track and value display
static bool PolishedSlider(const char* label, float* v, float v_min, float v_max, const char* format = "%.2f") {
    ImGui::PushID(label);
    
    ImGuiID id = ImGui::GetID("slider");
    float& animValue = sliderAnimations[id];
    
    // Smooth the displayed value for animation
    animValue += (*v - animValue) * 0.2f;
    
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    
    float sliderWidth = 180.0f;
    float sliderHeight = 6.0f;
    float labelWidth = 100.0f;
    float totalHeight = 24.0f;
    
    // Label
    ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + (totalHeight - ImGui::GetTextLineHeight()) * 0.5f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.00f));
    ImGui::Text("%s", label);
    ImGui::PopStyleColor();
    
    // Slider track position
    float trackX = pos.x + labelWidth;
    float trackY = pos.y + (totalHeight - sliderHeight) * 0.5f;
    
    // Track background (rounded)
    draw->AddRectFilled(
        ImVec2(trackX, trackY),
        ImVec2(trackX + sliderWidth, trackY + sliderHeight),
        IM_COL32(40, 40, 60, 255), 3.0f
    );
    
    // Filled portion
    float fillWidth = ((*v - v_min) / (v_max - v_min)) * sliderWidth;
    draw->AddRectFilled(
        ImVec2(trackX, trackY),
        ImVec2(trackX + fillWidth, trackY + sliderHeight),
        IM_COL32(233, 69, 96, 200), 3.0f
    );
    
    // Thumb
    float thumbX = trackX + fillWidth;
    float thumbRadius = 8.0f;
    
    // Invisible slider for interaction
    ImGui::SetCursorScreenPos(ImVec2(trackX, pos.y));
    ImGui::InvisibleButton("##slider", ImVec2(sliderWidth, totalHeight));
    
    bool hovered = ImGui::IsItemHovered();
    bool active = ImGui::IsItemActive();
    
    if (active) {
        float mouseX = ImGui::GetMousePos().x;
        float newValue = v_min + ((mouseX - trackX) / sliderWidth) * (v_max - v_min);
        *v = std::clamp(newValue, v_min, v_max);
    }
    
    // Thumb glow when hovered/active
    if (hovered || active) {
        draw->AddCircleFilled(ImVec2(thumbX, trackY + sliderHeight * 0.5f), thumbRadius + 4, IM_COL32(233, 69, 96, 60));
    }
    draw->AddCircleFilled(ImVec2(thumbX, trackY + sliderHeight * 0.5f), thumbRadius, IM_COL32(233, 69, 96, 255));
    draw->AddCircle(ImVec2(thumbX, trackY + sliderHeight * 0.5f), thumbRadius, IM_COL32(255, 255, 255, 100), 0, 1.5f);
    
    // Value display
    char valueBuf[32];
    snprintf(valueBuf, sizeof(valueBuf), format, *v);
    ImGui::SetCursorScreenPos(ImVec2(trackX + sliderWidth + 12, pos.y + (totalHeight - ImGui::GetTextLineHeight()) * 0.5f));
    ImGui::Text("%s", valueBuf);
    
    // Reset cursor for next item
    ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + totalHeight + 4));
    
    ImGui::PopID();
    return active;
}

/// Styled preset button
static bool PresetButton(const char* label) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.059f, 0.204f, 0.376f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.914f, 0.271f, 0.376f, 0.80f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.914f, 0.271f, 0.376f, 1.00f));
    
    ImVec2 size = ImVec2(100, 32);
    bool pressed = ImGui::Button(label, size);
    
    ImGui::PopStyleColor(3);
    return pressed;
}

/// Draw waveform visualization
static void DrawWaveformPreview(Waveform waveform, float dutyCycle, float width = 120.0f, float height = 40.0f) {
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    
    // Background
    draw->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), IM_COL32(22, 33, 62, 255), 4.0f);
    draw->AddRect(pos, ImVec2(pos.x + width, pos.y + height), IM_COL32(15, 52, 96, 255), 4.0f);
    
    float padding = 8.0f;
    float graphWidth = width - padding * 2;
    float graphHeight = height - padding * 2;
    float centerY = pos.y + height * 0.5f;
    
    // Draw waveform
    ImVec2 prevPoint(pos.x + padding, centerY);
    int segments = 60;
    
    for (int i = 0; i <= segments; i++) {
        float t = static_cast<float>(i) / segments;
        float sample = 0.0f;
        
        switch (waveform) {
            case Waveform::Square:
                sample = (t < dutyCycle) ? 1.0f : -1.0f;
                break;
            case Waveform::Triangle:
                sample = (t < 0.5f) ? (4.0f * t - 1.0f) : (3.0f - 4.0f * t);
                break;
            case Waveform::Sawtooth:
                sample = 2.0f * t - 1.0f;
                break;
            case Waveform::Noise:
                sample = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
                break;
        }
        
        ImVec2 point(
            pos.x + padding + t * graphWidth,
            centerY - sample * (graphHeight * 0.4f)
        );
        
        if (i > 0) {
            draw->AddLine(prevPoint, point, SynthTheme::Accent, 2.0f);
        }
        prevPoint = point;
    }
    
    // Zero line
    draw->AddLine(
        ImVec2(pos.x + padding, centerY),
        ImVec2(pos.x + width - padding, centerY),
        IM_COL32(100, 100, 120, 100), 1.0f
    );
    
    ImGui::Dummy(ImVec2(width, height));
}

std::vector<UserAction> SynthView::render(const SynthParams& params, bool embedded) {
    std::vector<UserAction> actions;
    
    if (!embedded) {
        ImGui::SetNextWindowSize(ImVec2(480, 0), ImGuiCond_FirstUseEver);
        ImGui::Begin("8-Bit Synthesizer", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    }
    
    // Title with icon (only if not embedded - parent handles title)
    if (!embedded) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.914f, 0.271f, 0.376f, 1.00f));
        ImGui::Text("🎵 8-BIT SYNTHESIZER");
        ImGui::PopStyleColor();
        ImGui::Spacing();
        ImGui::Spacing();
    }
    
    // ═══════════════════════════════════════════════════════════════
    // WAVEFORM SECTION
    // ═══════════════════════════════════════════════════════════════
    SectionHeader("WAVEFORM");
    
    ImGui::BeginGroup();
    {
        // Waveform selector
        const char* waveforms[] = { "Square", "Triangle", "Sawtooth", "Noise" };
        int currentWaveform = static_cast<int>(params.waveform);
        
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.059f, 0.204f, 0.376f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.100f, 0.250f, 0.420f, 1.00f));
        ImGui::SetNextItemWidth(140);
        if (ImGui::Combo("##waveform", &currentWaveform, waveforms, 4)) {
            UserAction action;
            action.type = UserActionType::WaveformChanged;
            action.waveformValue = static_cast<Waveform>(currentWaveform);
            actions.push_back(action);
        }
        ImGui::PopStyleColor(2);
        
        ImGui::SameLine(0, 24);
        DrawWaveformPreview(params.waveform, params.dutyCycle);
    }
    ImGui::EndGroup();
    
    // Duty cycle (for square wave only)
    if (params.waveform == Waveform::Square) {
        ImGui::Spacing();
        float dutyCycle = params.dutyCycle;
        if (PolishedSlider("Duty Cycle", &dutyCycle, 0.1f, 0.9f, "%.0f%%")) {
            UserAction action;
            action.type = UserActionType::DutyCycleChanged;
            action.floatValue = dutyCycle;
            actions.push_back(action);
        }
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // ═══════════════════════════════════════════════════════════════
    // ENVELOPE SECTION (ADSR)
    // ═══════════════════════════════════════════════════════════════
    SectionHeader("ENVELOPE");
    
    // ADSR Visualizer
    DrawADSREnvelope(params.attack, params.decay, params.sustain, params.release, 280.0f, 80.0f);
    
    ImGui::Spacing();
    
    // ADSR Sliders
    float attack = params.attack;
    if (PolishedSlider("Attack", &attack, 0.001f, 0.5f, "%.3fs")) {
        UserAction action;
        action.type = UserActionType::AttackChanged;
        action.floatValue = attack;
        actions.push_back(action);
    }
    
    float decay = params.decay;
    if (PolishedSlider("Decay", &decay, 0.01f, 1.0f, "%.3fs")) {
        UserAction action;
        action.type = UserActionType::DecayChanged;
        action.floatValue = decay;
        actions.push_back(action);
    }
    
    float sustain = params.sustain;
    if (PolishedSlider("Sustain", &sustain, 0.0f, 1.0f, "%.0f%%")) {
        UserAction action;
        action.type = UserActionType::SustainChanged;
        action.floatValue = sustain;
        actions.push_back(action);
    }
    
    float release = params.release;
    if (PolishedSlider("Release", &release, 0.01f, 2.0f, "%.3fs")) {
        UserAction action;
        action.type = UserActionType::ReleaseChanged;
        action.floatValue = release;
        actions.push_back(action);
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // ═══════════════════════════════════════════════════════════════
    // FREQUENCY SECTION
    // ═══════════════════════════════════════════════════════════════
    SectionHeader("FREQUENCY");
    
    float startFreq = params.startFreq;
    if (PolishedSlider("Start", &startFreq, 50.0f, 2000.0f, "%.0f Hz")) {
        UserAction action;
        action.type = UserActionType::StartFreqChanged;
        action.floatValue = startFreq;
        actions.push_back(action);
    }
    
    float endFreq = params.endFreq;
    if (PolishedSlider("End", &endFreq, 50.0f, 2000.0f, "%.0f Hz")) {
        UserAction action;
        action.type = UserActionType::EndFreqChanged;
        action.floatValue = endFreq;
        actions.push_back(action);
    }
    
    float slideSpeed = params.slideSpeed;
    if (PolishedSlider("Slide", &slideSpeed, 0.0f, 1.0f, "%.0f%%")) {
        UserAction action;
        action.type = UserActionType::SlideSpeedChanged;
        action.floatValue = slideSpeed;
        actions.push_back(action);
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // ═══════════════════════════════════════════════════════════════
    // EFFECTS SECTION
    // ═══════════════════════════════════════════════════════════════
    SectionHeader("EFFECTS");
    
    float reverbTime = params.effects.reverbTime;
    if (PolishedSlider("Reverb", &reverbTime, 0.0f, 2.0f, "%.2fs")) {
        UserAction action;
        action.type = UserActionType::ReverbChanged;
        action.floatValue = reverbTime;
        actions.push_back(action);
    }
    
    float reverbMix = params.effects.reverbMix;
    if (PolishedSlider("Mix", &reverbMix, 0.0f, 1.0f, "%.0f%%")) {
        UserAction action;
        action.type = UserActionType::ReverbChanged;
        action.floatValue = reverbMix;
        actions.push_back(action);
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // ═══════════════════════════════════════════════════════════════
    // DURATION
    // ═══════════════════════════════════════════════════════════════
    SectionHeader("DURATION");
    
    float duration = params.duration;
    if (PolishedSlider("Length", &duration, 0.05f, 2.0f, "%.2fs")) {
        UserAction action;
        action.type = UserActionType::DurationChanged;
        action.floatValue = duration;
        actions.push_back(action);
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // ═══════════════════════════════════════════════════════════════
    // CONTROL BUTTONS
    // ═══════════════════════════════════════════════════════════════
    
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 8));
    
    // Main action buttons
    if (StyledButton("▶ Play", ImVec2(100, 36))) {
        UserAction action;
        action.type = UserActionType::PlaySound;
        actions.push_back(action);
    }
    
    ImGui::SameLine();
    if (StyledButton("🎲 Random", ImVec2(100, 36))) {
        UserAction action;
        action.type = UserActionType::Randomize;
        actions.push_back(action);
    }
    
    ImGui::SameLine();
    if (StyledButton("💾 Export", ImVec2(100, 36))) {
        UserAction action;
        action.type = UserActionType::Export;
        action.filepath = "export.wav";
        actions.push_back(action);
    }
    
    ImGui::PopStyleVar();
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Real-time preview toggle
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 8));
    
    const char* previewLabel = previewEnabled ? "🔊 Preview: ON" : "🔇 Preview: OFF";
    ImVec4 previewColor = previewEnabled ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, previewColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(previewColor.x * 1.2f, previewColor.y * 1.2f, previewColor.z * 1.2f, 1.0f));
    
    if (ImGui::Button(previewLabel, ImVec2(200, 28))) {
        previewEnabled = !previewEnabled;
        UserAction action;
        action.type = UserActionType::PreviewToggled;
        action.intValue = previewEnabled ? 1 : 0;
        actions.push_back(action);
    }
    
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // ═══════════════════════════════════════════════════════════════
    // PRESETS
    // ═══════════════════════════════════════════════════════════════
    SectionHeader("PRESETS");
    
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));
    
    // Row 1
    if (PresetButton("Laser")) {
        UserAction action;
        action.type = UserActionType::PresetSelected;
        action.stringValue = "Laser";
        actions.push_back(action);
    }
    ImGui::SameLine();
    if (PresetButton("Jump")) {
        UserAction action;
        action.type = UserActionType::PresetSelected;
        action.stringValue = "Jump";
        actions.push_back(action);
    }
    ImGui::SameLine();
    if (PresetButton("Hurt")) {
        UserAction action;
        action.type = UserActionType::PresetSelected;
        action.stringValue = "Hurt";
        actions.push_back(action);
    }
    ImGui::SameLine();
    if (PresetButton("Powerup")) {
        UserAction action;
        action.type = UserActionType::PresetSelected;
        action.stringValue = "Powerup";
        actions.push_back(action);
    }
    
    // Row 2
    if (PresetButton("808 Kick")) {
        UserAction action;
        action.type = UserActionType::PresetSelected;
        action.stringValue = "808 Kick";
        actions.push_back(action);
    }
    ImGui::SameLine();
    if (PresetButton("808 Short")) {
        UserAction action;
        action.type = UserActionType::PresetSelected;
        action.stringValue = "808 Short";
        actions.push_back(action);
    }
    
    ImGui::PopStyleVar();
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Status (only if not embedded)
    if (!embedded) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.00f));
        if (previewEnabled) {
            ImGui::Text("Preview: ON • All parameters update live");
        } else {
            ImGui::Text("Preview: OFF • Use 'Play' button or activate Preview");
        }
        if (!lastExportPath.empty()) {
            ImGui::Text("Last export: %s", lastExportPath.c_str());
        }
        ImGui::PopStyleColor();
    }
    
    if (!embedded) {
        ImGui::End();
    }
    
    return actions;
}
