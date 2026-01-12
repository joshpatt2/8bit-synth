#include "view/SongView.h"
#include "view/SynthTheme.h"
#include "imgui.h"

std::vector<UserAction> SongView::render(const SongState& state, bool embedded) {
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
    
    // Pattern library info
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.00f));
    ImGui::Text("Patterns in Library:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("%zu", state.patterns.size());
    
    if (state.patterns.empty()) {
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.914f, 0.271f, 0.376f, 0.80f));
        ImGui::Text("No patterns yet - create one in sequencer");
        ImGui::PopStyleColor();
    }
    
    ImGui::Spacing();
    
    // Song status
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.00f));
    ImGui::Text("Current Song:");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("%s", state.name.c_str());
    if (state.modified) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.914f, 0.271f, 0.376f, 1.0f), "*");
    }
    
    if (!embedded) {
        ImGui::End();
    }
    
    return actions;
}
