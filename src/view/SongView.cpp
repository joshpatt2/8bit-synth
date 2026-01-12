#include "view/SongView.h"
#include "imgui.h"

std::vector<UserAction> SongView::render(const SongState& state) {
    std::vector<UserAction> actions;
    
    ImGui::Begin("Song Composer", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    ImGui::Text("🎼 Song Mode");
    ImGui::Separator();
    
    // Song name input
    ImGui::InputText("Song Name", songNameBuffer, sizeof(songNameBuffer));
    
    ImGui::Separator();
    ImGui::Text("Patterns in Library: %zu", state.patterns.size());
    
    if (state.patterns.empty()) {
        ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.3f, 1.0f), "No patterns yet - create one in sequencer");
    }
    
    ImGui::Separator();
    
    // Song info
    ImGui::Text("Song: %s", state.name.c_str());
    if (state.modified) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "*");
    }
    
    ImGui::End();
    
    return actions;
}
