# 8-Bit Synth Studio: UI Style Guide

**Version:** 1.0  
**Last Updated:** January 11, 2026  
**Framework:** Dear ImGui with SDL2  

---

## Design Philosophy

### Core Principles

1. **Audio First, UI Second**  
   The UI exists to serve the audio engine. Every design decision must preserve sub-10ms latency. If a visual flourish adds latency, cut it.

2. **Intentional, Not Default**  
   Nothing should look like ImGui's defaults. Every color, every spacing, every corner radius is a deliberate choice. If it looks like a debug UI, it's not done.

3. **Teach Through Visualization**  
   A synth UI should make audio concepts visible. The ADSR envelope visualizer isn't decoration—it teaches users what Attack, Decay, Sustain, and Release mean.

4. **One Window, One App**  
   No floating windows. No overlapping panels. One unified layout where everything has its place. Users should never hunt for a control.

5. **Polish Is Product**  
   Users judge audio quality by visual quality. A professional UI signals professional audio. An amateur UI undermines trust in the sound.

---

## Color Palette

### Primary Colors

| Name | Hex | RGB | Usage |
|------|-----|-----|-------|
| **Background** | `#1a1a2e` | `26, 26, 46` | Main window background |
| **Surface** | `#16213e` | `22, 33, 62` | Panels, child windows, cards |
| **Primary** | `#0f3460` | `15, 52, 96` | Interactive surfaces, borders |
| **Accent** | `#e94560` | `233, 69, 96` | Buttons, active states, highlights |
| **Text** | `#eaeaea` | `234, 234, 234` | Primary text |
| **Text Dim** | `#8b8b8b` | `139, 139, 139` | Labels, secondary text |

### Accent Variations

| State | Hex | Usage |
|-------|-----|-------|
| **Accent Hover** | `#ff6478` | Button hover states |
| **Accent Active** | `#c83250` | Button pressed states |
| **Success** | `#64ff64` | Playhead, active indicators |

### Usage Rules

- **Background** is only for the main window. Never use it for panels.
- **Surface** is for all contained areas (child windows, sections).
- **Accent** is reserved for interactive elements. Don't use it for decoration.
- **Text Dim** is for labels that describe controls, not for values.
- Never use pure white (`#ffffff`) or pure black (`#000000`).

---

## Typography

### Font

**Primary:** JetBrains Mono Regular, 14px  
**Fallback:** ImGui default font

JetBrains Mono is monospace, which aligns numeric values nicely in sliders and displays.

### Hierarchy

| Level | Size | Color | Usage |
|-------|------|-------|-------|
| **App Title** | 14px | Accent | "8-BIT SYNTH STUDIO" |
| **Section Header** | 14px | Accent + underline | "WAVEFORM", "ENVELOPE" |
| **Control Label** | 14px | Text Dim | "Attack", "Start Freq" |
| **Value Display** | 14px | Text | "0.100s", "440 Hz" |
| **Status Text** | 14px | Text Dim | "Real-time preview active" |

### Rules

- Section headers are ALL CAPS with accent color and underline.
- Control labels are sentence case, dim color.
- Never bold anything. The color hierarchy is sufficient.
- Emoji are allowed in titles only: 🎵, 🎲, 💾, ▶, ■

---

## Layout System

### Grid

All spacing is based on an **8px grid**:

- Window padding: 16px (2 units)
- Item spacing: 8px (1 unit)
- Section spacing: 16px (2 units)
- Group padding: 8px (1 unit)

### Structure

```
┌─────────────────────────────────────────────────────────────┐
│  Header Bar (title + status)                                │
├───────────────────────┬─────────────────────────────────────┤
│  Left Column (420px)  │  Right Column (flexible)            │
│                       │  ┌─────────────────────────────────┐│
│  [Synth Controls]     │  │  Top Section (Sequencer)        ││
│                       │  │  Height: 380px                  ││
│                       │  └─────────────────────────────────┘│
│                       │  ┌─────────────────────────────────┐│
│                       │  │  Bottom Section (Song)          ││
│                       │  │  Height: flexible               ││
│                       │  └─────────────────────────────────┘│
└───────────────────────┴─────────────────────────────────────┘
```

### Column Rules

- **Left column** is fixed width (420px) for synth controls.
- **Right column** fills remaining space.
- Never let content overflow. If it doesn't fit, the layout is wrong.

---

## Components

### Section Header

```cpp
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
        IM_COL32(233, 69, 96, 255), 2.0f
    );
    ImGui::Spacing();
}
```

Use for: Major UI sections (WAVEFORM, ENVELOPE, FREQUENCY, etc.)

### Styled Slider

Custom sliders with:
- Rounded track (dark)
- Filled portion (accent)
- Circular thumb with hover glow
- Label on left (100px), slider in middle (180px), value on right

```
[Label      ] ━━━━━○━━━━━━━━ [Value]
```

### Styled Button

Primary action buttons:
- Background: Accent
- Hover: Accent Hover (with subtle glow)
- Active: Accent Active
- Size: 100×36px for main actions, 80×28px for secondary

```cpp
if (StyledButton("▶ Play", ImVec2(100, 36))) { ... }
```

### Preset Button

Secondary buttons for presets:
- Background: Primary
- Hover: Accent (80% opacity)
- Size: 100×32px

```cpp
if (PresetButton("Laser")) { ... }
```

### Sequencer Grid Cell

- Size: 24×24px
- Spacing: 4px between cells, 8px between beats
- Colors:
  - Inactive: `#28283c` (dark)
  - Active: Accent
  - Playhead: Success (green)

### ADSR Visualizer

- Size: 280×80px
- Background: Surface with Primary border
- Lines: Accent, 2px stroke
- Fill: Accent at 24% opacity
- Points: 4px circles at envelope vertices

---

## Patterns

### DO: Embedded Views

Views should support an `embedded` parameter:

```cpp
std::vector<UserAction> render(const SynthParams& params, bool embedded = false);
```

When `embedded = true`:
- Skip `ImGui::Begin()` / `ImGui::End()`
- Skip window-specific titles
- Render directly into parent container

This allows views to work both standalone (for testing) and embedded (in production).

### DO: Action-Based Updates

Views emit `UserAction` objects, they don't modify state directly:

```cpp
if (ImGui::SliderFloat(...)) {
    UserAction action;
    action.type = UserActionType::AttackChanged;
    action.floatValue = newValue;
    actions.push_back(action);
}
return actions;
```

Controllers handle the actions. This maintains MVC separation.

### DO: Custom Rendering with ImDrawList

For polished controls, use `ImDrawList` instead of default widgets:

```cpp
ImDrawList* draw = ImGui::GetWindowDrawList();
ImVec2 pos = ImGui::GetCursorScreenPos();

// Draw custom control
draw->AddRectFilled(pos, endPos, color, rounding);

// Handle interaction with invisible button
ImGui::InvisibleButton("##control", size);
if (ImGui::IsItemActive()) { ... }
```

### DO: Smooth Animations

For value changes that should animate:

```cpp
static std::unordered_map<ImGuiID, float> animations;
float& anim = animations[ImGui::GetID("control")];
anim += (targetValue - anim) * 0.15f;  // Smooth interpolation
```

### DON'T: Use Default ImGui Styling

Never do this:
```cpp
ImGui::StyleColorsDark();  // NO
```

Always apply custom theme:
```cpp
SynthTheme::Apply();  // YES
```

### DON'T: Create Floating Windows

Never do this:
```cpp
ImGui::Begin("My Panel");  // NO - creates floating window
```

Use child containers instead:
```cpp
ImGui::BeginChild("MyPanel", size, true);  // YES - embedded panel
```

### DON'T: Use Raw ImGui Widgets in Visible UI

Never do this for user-facing controls:
```cpp
ImGui::SliderFloat("Attack", &value, 0, 1);  // NO - looks default
```

Use styled versions:
```cpp
PolishedSlider("Attack", &value, 0, 1, "%.3fs");  // YES - matches theme
```

### DON'T: Hardcode Colors

Never do this:
```cpp
ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.3f, 0.4f, 1.0f));  // NO
```

Use theme constants:
```cpp
ImGui::PushStyleColor(ImGuiCol_Button, SynthTheme::ToVec4(SynthTheme::Accent));  // YES
```

### DON'T: Block the Audio Thread

Never do file I/O, network calls, or heavy computation in the render loop. The UI runs at 60fps and shares time with audio processing.

---

## File Organization

```
include/view/
├── SynthTheme.h      # Colors, theme application, styled components
├── SynthView.h       # Synth panel view
├── SequencerView.h   # Sequencer panel view
└── SongView.h        # Song composer view

src/view/
├── SynthView.cpp     # Synth panel implementation
├── SequencerView.cpp # Sequencer implementation
└── SongView.cpp      # Song composer implementation

assets/fonts/
└── JetBrainsMono-Regular.ttf
```

### Where Code Lives

| Code Type | Location |
|-----------|----------|
| Color definitions | `SynthTheme.h` (namespace) |
| Theme application | `SynthTheme::Apply()` |
| Reusable components | `SynthTheme.h` (inline functions) |
| View rendering | `*View.cpp` |
| Layout orchestration | `main.mm` |

---

## Performance Guidelines

### Frame Budget

At 60fps, each frame has 16.67ms. The UI should use less than 2ms, leaving headroom for audio.

### Measurement

```cpp
// In debug builds, track frame time
float frameTime = ImGui::GetIO().DeltaTime * 1000.0f;
if (frameTime > 4.0f) {
    SDL_Log("Slow frame: %.2fms", frameTime);
}
```

### Optimization Rules

1. **Avoid allocations in render loop.** Use static buffers for temporary strings.
2. **Don't rebuild font atlas every frame.** Load fonts once at startup.
3. **Limit draw calls.** Batch similar operations.
4. **Use `##` labels** to avoid string hashing overhead: `"##slider"` not `"Attack Slider"`.

---

## Accessibility (Future)

ImGui has limited accessibility support. When we address this:

1. **High contrast mode** — Increase color contrast ratios.
2. **Keyboard navigation** — Already partially supported via `ImGuiConfigFlags_NavEnableKeyboard`.
3. **Screen reader support** — Not available in ImGui. Would require framework migration.

For now, focus on visual clarity and keyboard support.

---

## Review Checklist

Before merging UI changes:

- [ ] No default ImGui colors visible
- [ ] All spacing follows 8px grid
- [ ] Section headers use `SectionHeader()` function
- [ ] Buttons use `StyledButton()` or `PresetButton()`
- [ ] Sliders use `PolishedSlider()` for user-facing controls
- [ ] No floating windows (except debug/dev tools)
- [ ] Actions are emitted, not state mutations
- [ ] Frame time stays under 2ms
- [ ] Builds without warnings

---

## Examples

### Adding a New Control Section

```cpp
// In SynthView.cpp render()

ImGui::Spacing();
ImGui::Spacing();

SectionHeader("NEW SECTION");

float myValue = params.myValue;
if (PolishedSlider("My Control", &myValue, 0.0f, 1.0f, "%.2f")) {
    UserAction action;
    action.type = UserActionType::MyValueChanged;
    action.floatValue = myValue;
    actions.push_back(action);
}
```

### Adding a New Visualizer

```cpp
// In SynthTheme.h

inline void DrawMyVisualizer(float value, float width = 200.0f, float height = 60.0f) {
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    
    // Background
    draw->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), 
                        SynthTheme::Surface, 4.0f);
    draw->AddRect(pos, ImVec2(pos.x + width, pos.y + height), 
                  SynthTheme::Primary, 4.0f);
    
    // Custom visualization...
    
    // Reserve space
    ImGui::Dummy(ImVec2(width, height));
}
```

---

## References

- [ImGui Styling Guide](https://github.com/ocornut/imgui/issues/707)
- [Tracy Profiler](https://github.com/wolfpld/tracy) — Best-in-class ImGui styling example
- [ImPlot](https://github.com/epezent/implot) — Visualization patterns

---

*"The UI isn't done when it works. It's done when it feels inevitable."*
