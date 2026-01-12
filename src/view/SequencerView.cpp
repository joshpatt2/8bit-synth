#include "view/SequencerView.h"
#include "view/SynthTheme.h"
#include "imgui.h"
#include "PatternLoader.h"

std::vector<UserAction> SequencerView::render(const SequencerState& state, bool embedded) {
    std::vector<UserAction> actions;
    
    if (!embedded) {
        ImGui::Begin("Sequencer", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    }
    
    // Section header
    SectionHeader("SEQUENCER");
    
    // Transport controls
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.914f, 0.271f, 0.376f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.392f, 0.471f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.784f, 0.196f, 0.314f, 1.00f));
    
    if (state.isPlaying) {
        if (ImGui::Button("■ Stop", ImVec2(100, 32))) {
            UserAction action;
            action.type = UserActionType::SequencerStop;
            actions.push_back(action);
        }
    } else {
        if (ImGui::Button("▶ Play", ImVec2(100, 32))) {
            UserAction action;
            action.type = UserActionType::SequencerPlay;
            actions.push_back(action);
        }
    }
    ImGui::PopStyleColor(3);
    
    ImGui::SameLine();
    bool loop = state.pattern.loop;
    if (ImGui::Checkbox("Loop", &loop)) {
        // Loop changes would need to be handled in controller
    }
    
    ImGui::SameLine(0, 32);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.00f));
    ImGui::Text("BPM:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    
    // Styled BPM slider
    float bpm = state.pattern.bpm;
    ImGui::SetNextItemWidth(120);
    if (ImGui::SliderFloat("##BPM", &bpm, 60.0f, 200.0f, "%.0f")) {
        UserAction action;
        action.type = UserActionType::SequencerBPMChanged;
        action.floatValue = bpm;
        actions.push_back(action);
    }
    
    ImGui::Spacing();
    
    // Sound slot selector with styled buttons
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.00f));
    ImGui::Text("Sound Slots:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    
    for (int i = 0; i < static_cast<int>(state.slots.size()); i++) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.059f, 0.204f, 0.376f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.914f, 0.271f, 0.376f, 0.80f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.914f, 0.271f, 0.376f, 1.00f));
        
        if (ImGui::Button(state.slots[i].name.c_str(), ImVec2(80, 28))) {
            UserAction action;
            action.type = UserActionType::SequencerSlotSelected;
            action.intValue = i;
            actions.push_back(action);
        }
        ImGui::PopStyleColor(3);
        
        if (i < 3) ImGui::SameLine();
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Step grid
    float cellSize = 24.0f;
    float cellSpacing = 4.0f;
    float labelWidth = 60.0f;
    float beatSeparator = 8.0f;
    
    for (int slot = 0; slot < static_cast<int>(state.slots.size()); slot++) {
        ImGui::PushID(slot);
        
        // Row label
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.00f));
        ImGui::Text("%-6s", state.slots[slot].name.c_str());
        ImGui::PopStyleColor();
        ImGui::SameLine(labelWidth);
        
        for (int step = 0; step < state.pattern.numSteps; step++) {
            // Add beat separator
            if (step > 0 && step % 4 == 0) {
                ImGui::SameLine(0, beatSeparator);
            } else if (step > 0) {
                ImGui::SameLine(0, cellSpacing);
            }
            
            ImGui::PushID(step);
            
            const auto& stepData = state.pattern.steps[step];
            bool isActive = stepData.active && stepData.soundSlot == slot;
            bool isCurrent = (state.currentStep == step && state.isPlaying);
            
            // Determine cell color
            ImVec4 cellColor;
            if (isCurrent) {
                cellColor = ImVec4(0.392f, 1.0f, 0.392f, 1.0f);  // Green playhead
            } else if (isActive) {
                cellColor = ImVec4(0.914f, 0.271f, 0.376f, 1.0f);  // Accent
            } else {
                cellColor = ImVec4(0.157f, 0.157f, 0.235f, 1.0f);  // Dark
            }
            
            ImGui::PushStyleColor(ImGuiCol_Button, cellColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(cellColor.x + 0.1f, cellColor.y + 0.1f, cellColor.z + 0.1f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(cellColor.x - 0.1f, cellColor.y - 0.1f, cellColor.z - 0.1f, 1.0f));
            
            if (ImGui::Button("##cell", ImVec2(cellSize, cellSize))) {
                UserAction action;
                action.type = UserActionType::SequencerStepToggled;
                action.intValue = step;
                actions.push_back(action);
                selectedStep = step;
            }
            
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }
        
        ImGui::PopID();
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Clear pattern button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.059f, 0.204f, 0.376f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.914f, 0.271f, 0.376f, 0.80f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.914f, 0.271f, 0.376f, 1.00f));
    if (ImGui::Button("Clear Pattern", ImVec2(120, 28))) {
        UserAction action;
        action.type = UserActionType::SequencerClear;
        actions.push_back(action);
    }
    ImGui::PopStyleColor(3);
    
    if (!embedded) {
        ImGui::End();
    }
    
    return actions;
}
