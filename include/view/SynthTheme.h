#pragma once

#include "imgui.h"
#include <unordered_map>

/// Custom theme colors for the 8-bit Synthesizer
namespace SynthTheme {
    // Color palette
    constexpr ImU32 Background      = IM_COL32(26, 26, 46, 255);      // #1a1a2e
    constexpr ImU32 Surface         = IM_COL32(22, 33, 62, 255);      // #16213e
    constexpr ImU32 Primary         = IM_COL32(15, 52, 96, 255);      // #0f3460
    constexpr ImU32 Accent          = IM_COL32(233, 69, 96, 255);     // #e94560
    constexpr ImU32 AccentHover     = IM_COL32(255, 100, 120, 255);   // Lighter accent
    constexpr ImU32 AccentActive    = IM_COL32(200, 50, 80, 255);     // Darker accent
    constexpr ImU32 Text            = IM_COL32(234, 234, 234, 255);   // #eaeaea
    constexpr ImU32 TextDim         = IM_COL32(139, 139, 139, 255);   // #8b8b8b
    constexpr ImU32 Success         = IM_COL32(100, 255, 100, 255);   // Green for active
    constexpr ImU32 SliderTrack     = IM_COL32(40, 40, 60, 255);      // Dark track
    constexpr ImU32 SliderFill      = IM_COL32(233, 69, 96, 200);     // Accent with alpha
    
    // As ImVec4 for ImGui style colors
    inline ImVec4 ToVec4(ImU32 col) {
        return ImVec4(
            ((col >> 0) & 0xFF) / 255.0f,
            ((col >> 8) & 0xFF) / 255.0f,
            ((col >> 16) & 0xFF) / 255.0f,
            ((col >> 24) & 0xFF) / 255.0f
        );
    }
    
    /// Apply the custom theme to ImGui
    inline void Apply() {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;
        
        // Main colors
        colors[ImGuiCol_WindowBg]           = ImVec4(0.102f, 0.102f, 0.180f, 1.00f);  // #1a1a2e
        colors[ImGuiCol_ChildBg]            = ImVec4(0.086f, 0.129f, 0.243f, 1.00f);  // #16213e
        colors[ImGuiCol_PopupBg]            = ImVec4(0.086f, 0.129f, 0.243f, 0.95f);
        
        // Borders
        colors[ImGuiCol_Border]             = ImVec4(0.059f, 0.204f, 0.376f, 0.50f);  // #0f3460
        colors[ImGuiCol_BorderShadow]       = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        
        // Frame backgrounds (sliders, inputs)
        colors[ImGuiCol_FrameBg]            = ImVec4(0.157f, 0.157f, 0.235f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]     = ImVec4(0.200f, 0.200f, 0.300f, 1.00f);
        colors[ImGuiCol_FrameBgActive]      = ImVec4(0.059f, 0.204f, 0.376f, 1.00f);
        
        // Title bar
        colors[ImGuiCol_TitleBg]            = ImVec4(0.059f, 0.204f, 0.376f, 1.00f);
        colors[ImGuiCol_TitleBgActive]      = ImVec4(0.059f, 0.204f, 0.376f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]   = ImVec4(0.059f, 0.204f, 0.376f, 0.75f);
        
        // Buttons
        colors[ImGuiCol_Button]             = ImVec4(0.914f, 0.271f, 0.376f, 1.00f);  // #e94560
        colors[ImGuiCol_ButtonHovered]      = ImVec4(1.000f, 0.392f, 0.471f, 1.00f);
        colors[ImGuiCol_ButtonActive]       = ImVec4(0.784f, 0.196f, 0.314f, 1.00f);
        
        // Headers
        colors[ImGuiCol_Header]             = ImVec4(0.059f, 0.204f, 0.376f, 1.00f);
        colors[ImGuiCol_HeaderHovered]      = ImVec4(0.914f, 0.271f, 0.376f, 0.80f);
        colors[ImGuiCol_HeaderActive]       = ImVec4(0.914f, 0.271f, 0.376f, 1.00f);
        
        // Slider
        colors[ImGuiCol_SliderGrab]         = ImVec4(0.914f, 0.271f, 0.376f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]   = ImVec4(1.000f, 0.392f, 0.471f, 1.00f);
        
        // Checkbox
        colors[ImGuiCol_CheckMark]          = ImVec4(0.914f, 0.271f, 0.376f, 1.00f);
        
        // Separator
        colors[ImGuiCol_Separator]          = ImVec4(0.059f, 0.204f, 0.376f, 0.50f);
        colors[ImGuiCol_SeparatorHovered]   = ImVec4(0.914f, 0.271f, 0.376f, 0.78f);
        colors[ImGuiCol_SeparatorActive]    = ImVec4(0.914f, 0.271f, 0.376f, 1.00f);
        
        // Text
        colors[ImGuiCol_Text]               = ImVec4(0.918f, 0.918f, 0.918f, 1.00f);  // #eaeaea
        colors[ImGuiCol_TextDisabled]       = ImVec4(0.545f, 0.545f, 0.545f, 1.00f);  // #8b8b8b
        
        // Tab
        colors[ImGuiCol_Tab]                = ImVec4(0.059f, 0.204f, 0.376f, 0.86f);
        colors[ImGuiCol_TabHovered]         = ImVec4(0.914f, 0.271f, 0.376f, 0.80f);
        colors[ImGuiCol_TabSelected]        = ImVec4(0.914f, 0.271f, 0.376f, 1.00f);
        
        // Scrollbar
        colors[ImGuiCol_ScrollbarBg]        = ImVec4(0.102f, 0.102f, 0.180f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab]      = ImVec4(0.059f, 0.204f, 0.376f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.914f, 0.271f, 0.376f, 0.78f);
        colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.914f, 0.271f, 0.376f, 1.00f);
        
        // Style adjustments
        style.WindowRounding    = 8.0f;
        style.ChildRounding     = 6.0f;
        style.FrameRounding     = 4.0f;
        style.GrabRounding      = 4.0f;
        style.PopupRounding     = 4.0f;
        style.ScrollbarRounding = 4.0f;
        style.TabRounding       = 4.0f;
        
        style.WindowPadding     = ImVec2(16.0f, 16.0f);
        style.FramePadding      = ImVec2(8.0f, 4.0f);
        style.ItemSpacing       = ImVec2(8.0f, 8.0f);
        style.ItemInnerSpacing  = ImVec2(8.0f, 4.0f);
        
        style.WindowBorderSize  = 1.0f;
        style.FrameBorderSize   = 0.0f;
        style.PopupBorderSize   = 1.0f;
        
        style.ScrollbarSize     = 12.0f;
        style.GrabMinSize       = 10.0f;
    }
}

/// Draw a custom styled slider with filled track
inline bool StyledSliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.2f") {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.157f, 0.157f, 0.235f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.200f, 0.200f, 0.300f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.200f, 0.200f, 0.300f, 1.00f));
    
    bool changed = ImGui::SliderFloat(label, v, v_min, v_max, format);
    
    ImGui::PopStyleColor(3);
    return changed;
}

/// Draw ADSR envelope visualization
inline void DrawADSREnvelope(float attack, float decay, float sustain, float release, float width = 200.0f, float height = 80.0f) {
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    
    // Background panel
    draw->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), SynthTheme::Surface, 4.0f);
    draw->AddRect(pos, ImVec2(pos.x + width, pos.y + height), SynthTheme::Primary, 4.0f);
    
    // Calculate envelope points
    float padding = 8.0f;
    float graphWidth = width - padding * 2;
    float graphHeight = height - padding * 2;
    
    // Normalize times (attack, decay, release are in seconds, sustain is a level)
    float totalTime = attack + decay + release + 0.3f; // 0.3 for sustain hold visualization
    float attackEnd = (attack / totalTime) * graphWidth;
    float decayEnd = attackEnd + (decay / totalTime) * graphWidth;
    float sustainEnd = decayEnd + (0.3f / totalTime) * graphWidth;
    float releaseEnd = graphWidth;
    
    // Points
    ImVec2 p0(pos.x + padding, pos.y + height - padding);                           // Start (0,0)
    ImVec2 p1(pos.x + padding + attackEnd, pos.y + padding);                        // Peak (attack)
    ImVec2 p2(pos.x + padding + decayEnd, pos.y + padding + graphHeight * (1.0f - sustain));  // Sustain level
    ImVec2 p3(pos.x + padding + sustainEnd, pos.y + padding + graphHeight * (1.0f - sustain)); // Sustain hold
    ImVec2 p4(pos.x + padding + releaseEnd, pos.y + height - padding);              // End (release)
    
    // Fill under curve (semi-transparent accent)
    ImVec2 fillPoints[6] = {p0, p1, p2, p3, p4, ImVec2(p4.x, p0.y)};
    draw->AddConvexPolyFilled(fillPoints, 5, IM_COL32(233, 69, 96, 60));
    
    // Draw the envelope line
    draw->AddLine(p0, p1, SynthTheme::Accent, 2.0f);  // Attack
    draw->AddLine(p1, p2, SynthTheme::Accent, 2.0f);  // Decay
    draw->AddLine(p2, p3, SynthTheme::Accent, 2.0f);  // Sustain hold
    draw->AddLine(p3, p4, SynthTheme::Accent, 2.0f);  // Release
    
    // Draw points
    draw->AddCircleFilled(p0, 4.0f, SynthTheme::Text);
    draw->AddCircleFilled(p1, 4.0f, SynthTheme::Accent);
    draw->AddCircleFilled(p2, 4.0f, SynthTheme::Accent);
    draw->AddCircleFilled(p3, 4.0f, SynthTheme::Accent);
    draw->AddCircleFilled(p4, 4.0f, SynthTheme::Text);
    
    // Labels
    ImGui::SetCursorScreenPos(ImVec2(pos.x + padding, pos.y + height + 2));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.00f));
    ImGui::Text("A     D     S     R");
    ImGui::PopStyleColor();
    
    // Reserve space for the widget
    ImGui::Dummy(ImVec2(width, height + 20));
}

/// Section header with underline
inline void SectionHeader(const char* label) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.914f, 0.271f, 0.376f, 1.00f));
    ImGui::Text("%s", label);
    ImGui::PopStyleColor();
    
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* draw = ImGui::GetWindowDrawList();
    float textWidth = ImGui::CalcTextSize(label).x;
    draw->AddLine(
        ImVec2(pos.x, pos.y - 2),
        ImVec2(pos.x + textWidth, pos.y - 2),
        SynthTheme::Accent, 2.0f
    );
    ImGui::Spacing();
}

/// Styled button with hover animation
inline bool StyledButton(const char* label, ImVec2 size = ImVec2(0, 0)) {
    static std::unordered_map<ImGuiID, float> hoverAnimations;
    
    ImGuiID id = ImGui::GetID(label);
    float& anim = hoverAnimations[id];
    
    // Default size
    if (size.x == 0) size.x = ImGui::CalcTextSize(label).x + 24;
    if (size.y == 0) size.y = 32;
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.914f, 0.271f, 0.376f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.392f, 0.471f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.784f, 0.196f, 0.314f, 1.00f));
    
    bool pressed = ImGui::Button(label, size);
    
    // Update animation state after button (for next frame)
    float targetAnim = ImGui::IsItemHovered() ? 1.0f : 0.0f;
    anim += (targetAnim - anim) * 0.15f;
    
    ImGui::PopStyleColor(3);
    
    return pressed;
}

/// Panel with background
inline void BeginPanel(const char* id, float width = 0) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.086f, 0.129f, 0.243f, 0.50f));
    ImGui::BeginChild(id, ImVec2(width, 0), ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));
}

inline void EndPanel() {
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::Spacing();
}
