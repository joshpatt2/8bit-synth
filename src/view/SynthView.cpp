#include "view/SynthView.h"
#include "imgui.h"

std::vector<UserAction> SynthView::render(const SynthParams& params) {
    std::vector<UserAction> actions;
    
    ImGui::Begin("8-Bit Synthesizer", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    // Title
    ImGui::Text("🎵 Retro Sound Effect Generator");
    ImGui::Separator();
    
    // Waveform selection
    ImGui::Text("Waveform");
    const char* waveforms[] = { "Square", "Triangle", "Sawtooth", "Noise" };
    int currentWaveform = static_cast<int>(params.waveform);
    if (ImGui::Combo("##waveform", &currentWaveform, waveforms, 4)) {
        UserAction action;
        action.type = UserActionType::WaveformChanged;
        action.waveformValue = static_cast<Waveform>(currentWaveform);
        actions.push_back(action);
    }
    
    // Duty cycle (for square wave)
    if (params.waveform == Waveform::Square) {
        float dutyCycle = params.dutyCycle;
        if (ImGui::SliderFloat("Duty Cycle", &dutyCycle, 0.1f, 0.9f)) {
            UserAction action;
            action.type = UserActionType::DutyCycleChanged;
            action.floatValue = dutyCycle;
            actions.push_back(action);
        }
    }
    
    ImGui::Separator();
    
    // Frequency controls
    ImGui::Text("Frequency");
    float startFreq = params.startFreq;
    if (ImGui::SliderFloat("Start (Hz)", &startFreq, 50.0f, 2000.0f)) {
        UserAction action;
        action.type = UserActionType::StartFreqChanged;
        action.floatValue = startFreq;
        actions.push_back(action);
    }
    
    float endFreq = params.endFreq;
    if (ImGui::SliderFloat("End (Hz)", &endFreq, 50.0f, 2000.0f)) {
        UserAction action;
        action.type = UserActionType::EndFreqChanged;
        action.floatValue = endFreq;
        actions.push_back(action);
    }
    
    float slideSpeed = params.slideSpeed;
    if (ImGui::SliderFloat("Slide Speed", &slideSpeed, 0.0f, 1.0f)) {
        UserAction action;
        action.type = UserActionType::SlideSpeedChanged;
        action.floatValue = slideSpeed;
        actions.push_back(action);
    }
    
    ImGui::Separator();
    
    // Envelope controls
    ImGui::Text("Envelope (ADSR)");
    float attack = params.attack;
    if (ImGui::SliderFloat("Attack", &attack, 0.001f, 0.5f, "%.3f s")) {
        UserAction action;
        action.type = UserActionType::AttackChanged;
        action.floatValue = attack;
        actions.push_back(action);
    }
    
    float decay = params.decay;
    if (ImGui::SliderFloat("Decay", &decay, 0.01f, 1.0f, "%.3f s")) {
        UserAction action;
        action.type = UserActionType::DecayChanged;
        action.floatValue = decay;
        actions.push_back(action);
    }
    
    float sustain = params.sustain;
    if (ImGui::SliderFloat("Sustain", &sustain, 0.0f, 1.0f)) {
        UserAction action;
        action.type = UserActionType::SustainChanged;
        action.floatValue = sustain;
        actions.push_back(action);
    }
    
    float release = params.release;
    if (ImGui::SliderFloat("Release", &release, 0.01f, 2.0f, "%.3f s")) {
        UserAction action;
        action.type = UserActionType::ReleaseChanged;
        action.floatValue = release;
        actions.push_back(action);
    }
    
    ImGui::Separator();
    
    // Duration
    float duration = params.duration;
    if (ImGui::SliderFloat("Duration", &duration, 0.05f, 2.0f, "%.2f s")) {
        UserAction action;
        action.type = UserActionType::DurationChanged;
        action.floatValue = duration;
        actions.push_back(action);
    }
    
    ImGui::Separator();
    
    // Effects
    ImGui::Text("Effects");
    bool reverbEnabled = params.effects.reverbEnabled;
    if (ImGui::Checkbox("Reverb##enable", &reverbEnabled)) {
        // Note: View only reports user intention, controller handles it
    }
    
    ImGui::SameLine();
    float reverbTime = params.effects.reverbTime;
    if (ImGui::SliderFloat("Reverb Time", &reverbTime, 0.0f, 2.0f, "%.2f s")) {
        UserAction action;
        action.type = UserActionType::ReverbChanged;
        action.floatValue = reverbTime;
        actions.push_back(action);
    }
    
    float reverbMix = params.effects.reverbMix;
    if (ImGui::SliderFloat("Reverb Mix", &reverbMix, 0.0f, 1.0f, "%.2f")) {
        UserAction action;
        action.type = UserActionType::ReverbChanged;
        action.floatValue = reverbMix;
        actions.push_back(action);
    }
    
    ImGui::Separator();
    
    // Real-time preview toggle
    if (ImGui::Button("🔊 Real-Time Preview", ImVec2(150, 30))) {
        UserAction action;
        action.type = UserActionType::PlaySound;  // Reuse for now, will handle in controller
        actions.push_back(action);
    }
    
    ImGui::Text("(All sliders update audio in real-time when preview is active)");
    
    ImGui::Separator();
    
    // Control buttons
    if (ImGui::Button("Play", ImVec2(80, 30))) {
        UserAction action;
        action.type = UserActionType::PlaySound;
        actions.push_back(action);
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Randomize", ImVec2(80, 30))) {
        UserAction action;
        action.type = UserActionType::Randomize;
        actions.push_back(action);
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Export", ImVec2(80, 30))) {
        UserAction action;
        action.type = UserActionType::Export;
        action.filepath = "export.wav";
        actions.push_back(action);
    }
    
    ImGui::Separator();
    
    // Presets
    ImGui::Text("Presets");
    if (ImGui::Button("Laser", ImVec2(100, 0))) {
        UserAction action;
        action.type = UserActionType::PresetSelected;
        action.stringValue = "Laser";
        actions.push_back(action);
    }
    ImGui::SameLine();
    if (ImGui::Button("Jump", ImVec2(100, 0))) {
        UserAction action;
        action.type = UserActionType::PresetSelected;
        action.stringValue = "Jump";
        actions.push_back(action);
    }
    ImGui::SameLine();
    if (ImGui::Button("Hurt", ImVec2(100, 0))) {
        UserAction action;
        action.type = UserActionType::PresetSelected;
        action.stringValue = "Hurt";
        actions.push_back(action);
    }
    ImGui::SameLine();
    if (ImGui::Button("Powerup", ImVec2(100, 0))) {
        UserAction action;
        action.type = UserActionType::PresetSelected;
        action.stringValue = "Powerup";
        actions.push_back(action);
    }
    
    if (ImGui::Button("808 Kick", ImVec2(100, 0))) {
        UserAction action;
        action.type = UserActionType::PresetSelected;
        action.stringValue = "808 Kick";
        actions.push_back(action);
    }
    ImGui::SameLine();
    if (ImGui::Button("808 Short", ImVec2(100, 0))) {
        UserAction action;
        action.type = UserActionType::PresetSelected;
        action.stringValue = "808 Short";
        actions.push_back(action);
    }
    
    ImGui::Separator();
    
    // Status
    ImGui::Text("Status: Ready");
    if (!lastExportPath.empty()) {
        ImGui::Text("Last export: %s", lastExportPath.c_str());
    }
    
    ImGui::End();
    
    return actions;
}
