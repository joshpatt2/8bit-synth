#include "view/SongView.h"
#include "view/SynthTheme.h"
#include "imgui.h"

void SongView::setAvailablePatterns(const std::vector<PatternFile>& patterns) {
    availablePatterns = patterns;
    selectedPatternIndex = -1;
}

std::vector<UserAction> SongView::render(const SongState& /* state */, bool embedded) {
    std::vector<UserAction> actions;
    
    if (!embedded) {
        ImGui::Begin("Song Composer", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    }
    
    SectionHeader("SONG COMPOSER");
    
    // Song name input
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.00f));
    ImGui::Text("Song Name:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200);
    ImGui::InputText("##SongName", songNameBuffer, sizeof(songNameBuffer));
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    renderPatternLibrarySection();
    
    // Modals
    if (showSavePatternDialog) {
        renderSavePatternDialog(actions);
    }
    if (showLoadPatternDialog) {
        renderLoadPatternDialog(actions);
    }
    if (showDeleteConfirm) {
        renderDeleteConfirmDialog(actions);
    }
    
    if (!embedded) {
        ImGui::End();
    }
    
    return actions;
}

void SongView::renderPatternLibrarySection() {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.00f));
    ImGui::Text("Pattern Library:");
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    
    // Save and Load buttons
    if (ImGui::Button("💾 Save Pattern", ImVec2(120, 0))) {
        showSavePatternDialog = true;
        memset(patternNameBuffer, 0, sizeof(patternNameBuffer));
    }
    
    ImGui::SameLine();
    if (ImGui::Button("📂 Load Pattern", ImVec2(120, 0))) {
        showLoadPatternDialog = true;
        selectedPatternIndex = -1;
    }
    
    ImGui::Spacing();
    
    // Pattern list
    if (!availablePatterns.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.00f));
        ImGui::Text("Available Patterns (%zu):", availablePatterns.size());
        ImGui::PopStyleColor();
        
        // Display patterns in a table-like format
        for (size_t i = 0; i < availablePatterns.size(); ++i) {
            const auto& pattern = availablePatterns[i];
            
            bool isSelected = (selectedPatternIndex == static_cast<int>(i));
            if (ImGui::Selectable(pattern.name.c_str(), isSelected, ImGuiSelectableFlags_DontClosePopups)) {
                selectedPatternIndex = static_cast<int>(i);
            }
            
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s - %s", pattern.author.c_str(), pattern.getShortDate().c_str());
            }
        }
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.914f, 0.271f, 0.376f, 0.80f));
        ImGui::Text("No patterns saved yet");
        ImGui::PopStyleColor();
    }
}

void SongView::renderSavePatternDialog(std::vector<UserAction>& actions) {
    ImGui::OpenPopup("Save Pattern");
    
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    
    if (ImGui::BeginPopupModal("Save Pattern", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Pattern Name:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(250);
        ImGui::InputText("##PatternName", patternNameBuffer, sizeof(patternNameBuffer));
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        if (ImGui::Button("Save", ImVec2(120, 0))) {
            if (strlen(patternNameBuffer) > 0) {
                UserAction action;
                action.type = UserActionType::SavePattern;
                action.stringValue = patternNameBuffer;
                actions.push_back(action);
                
                showSavePatternDialog = false;
                ImGui::CloseCurrentPopup();
            }
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            showSavePatternDialog = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

void SongView::renderLoadPatternDialog(std::vector<UserAction>& actions) {
    ImGui::OpenPopup("Load Pattern");
    
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    
    if (ImGui::BeginPopupModal("Load Pattern", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Select a pattern to load:");
        ImGui::Spacing();
        
        if (!availablePatterns.empty()) {
            for (size_t i = 0; i < availablePatterns.size(); ++i) {
                const auto& pattern = availablePatterns[i];
                bool isSelected = (selectedPatternIndex == static_cast<int>(i));
                
                if (ImGui::Selectable(pattern.name.c_str(), isSelected)) {
                    selectedPatternIndex = static_cast<int>(i);
                }
            }
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.914f, 0.271f, 0.376f, 0.80f));
            ImGui::Text("No patterns available");
            ImGui::PopStyleColor();
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        bool canLoad = selectedPatternIndex >= 0 && selectedPatternIndex < static_cast<int>(availablePatterns.size());
        if (!canLoad) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }
        
        if (ImGui::Button("Load", ImVec2(120, 0))) {
            if (canLoad) {
                UserAction action;
                action.type = UserActionType::LoadPattern;
                action.filepath = availablePatterns[selectedPatternIndex].filepath;
                actions.push_back(action);
                
                showLoadPatternDialog = false;
                ImGui::CloseCurrentPopup();
            }
        }
        
        if (!canLoad) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Delete", ImVec2(120, 0)) && canLoad) {
            showDeleteConfirm = true;
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Close", ImVec2(120, 0))) {
            showLoadPatternDialog = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

void SongView::renderDeleteConfirmDialog(std::vector<UserAction>& actions) {
    ImGui::OpenPopup("Delete Pattern?");
    
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    
    if (ImGui::BeginPopupModal("Delete Pattern?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        bool canDelete = selectedPatternIndex >= 0 && selectedPatternIndex < static_cast<int>(availablePatterns.size());
        
        if (canDelete) {
            ImGui::Text("Delete '%s'?", availablePatterns[selectedPatternIndex].name.c_str());
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        if (ImGui::Button("Delete", ImVec2(120, 0)) && canDelete) {
            UserAction action;
            action.type = UserActionType::DeletePattern;
            action.filepath = availablePatterns[selectedPatternIndex].filepath;
            actions.push_back(action);
            
            showDeleteConfirm = false;
            showLoadPatternDialog = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            showDeleteConfirm = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}
