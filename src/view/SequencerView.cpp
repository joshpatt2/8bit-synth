#include "view/SequencerView.h"
#include "imgui.h"
#include "PatternLoader.h"

std::vector<UserAction> SequencerView::render(const SequencerState& state) {
    std::vector<UserAction> actions;
    
    ImGui::Begin("Sequencer", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    // Transport controls
    if (state.isPlaying) {
        if (ImGui::Button("■ Stop", ImVec2(100, 30))) {
            UserAction action;
            action.type = UserActionType::SequencerStop;
            actions.push_back(action);
        }
    } else {
        if (ImGui::Button("▶ Play", ImVec2(100, 30))) {
            UserAction action;
            action.type = UserActionType::SequencerPlay;
            actions.push_back(action);
        }
    }
    
    ImGui::SameLine();
    bool loop = state.pattern.loop;
    if (ImGui::Checkbox("Loop", &loop)) {
        // Loop changes would need to be handled in controller
    }
    
    // Tempo and pattern length
    float bpm = state.pattern.bpm;
    if (ImGui::SliderFloat("BPM", &bpm, 60.0f, 200.0f, "%.0f")) {
        UserAction action;
        action.type = UserActionType::SequencerBPMChanged;
        action.floatValue = bpm;
        actions.push_back(action);
    }
    
    ImGui::Separator();
    
    // Sound slot selector
    ImGui::Text("Sound Slots:");
    for (int i = 0; i < static_cast<int>(state.slots.size()); i++) {
        bool isSelected = (selectedStep >= 0);
        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.8f, 1.0f));
        }
        
        if (ImGui::Button(state.slots[i].name.c_str(), ImVec2(100, 0))) {
            UserAction action;
            action.type = UserActionType::SequencerSlotSelected;
            action.intValue = i;
            actions.push_back(action);
        }
        
        if (isSelected) {
            ImGui::PopStyleColor();
        }
        
        if (i < 3) ImGui::SameLine();
    }
    
    ImGui::Separator();
    
    // Step grid
    ImGui::Text("Step Grid:");
    
    for (int slot = 0; slot < static_cast<int>(state.slots.size()); slot++) {
        ImGui::PushID(slot);
        
        ImGui::Text("%-6s", state.slots[slot].name.c_str());
        ImGui::SameLine();
        
        for (int step = 0; step < state.pattern.numSteps; step++) {
            if (step > 0 && step % 4 == 0) {
                ImGui::SameLine();
                ImGui::Text("|");
            }
            
            ImGui::SameLine();
            ImGui::PushID(step);
            
            const auto& stepData = state.pattern.steps[step];
            bool isActive = stepData.active && stepData.soundSlot == slot;
            bool isCurrent = (state.currentStep == step && state.isPlaying);
            bool isSelected = (selectedStep == step);
            
            // Style the button based on state
            if (isCurrent) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.9f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
            } else if (isSelected) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.7f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.8f, 0.3f, 1.0f));
            } else if (isActive) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.9f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
            }
            
            const char* glyph = isActive ? "✓" : "·";
            if (isCurrent) glyph = "►";
            if (isSelected && !isCurrent) glyph = "◆";
            
            if (ImGui::SmallButton(glyph)) {
                UserAction action;
                action.type = UserActionType::SequencerStepToggled;
                action.intValue = step;
                actions.push_back(action);
                
                selectedStep = step;
            }
            
            ImGui::PopStyleColor(2);
            ImGui::PopID();
        }
        
        ImGui::PopID();
        ImGui::NewLine();
    }
    
    ImGui::Separator();
    
    // Clear pattern button
    if (ImGui::Button("Clear Pattern")) {
        UserAction action;
        action.type = UserActionType::SequencerClear;
        actions.push_back(action);
    }
    
    ImGui::End();
    
    return actions;
}
