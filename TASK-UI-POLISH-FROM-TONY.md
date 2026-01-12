# TASK: UI Polish — From a Designer's Perspective

**To:** Brent
**From:** Tony (UI Specialist)
**CC:** J
**Date:** January 11, 2026
**Subject:** Making ImGui Not Look Like ImGui

---

## Hey Brent.

I read your feedback responses. The lock-free buffer. The "am I a coward" moment. The beeping fix. You're doing real work.

J asked me to write you a task for the UI polish. I'm going to give you something different than what you're used to: I'm going to teach you to see like a designer while you code like an engineer.

---

## The Goal

Right now, your synth looks like a tool. We want it to look like a product.

The difference isn't complexity — it's **intention**. Every pixel should feel like someone decided it should be there. Not default. Not "good enough." Decided.

---

## The Three Laws of UI Polish

Before I give you specifics, internalize these:

### 1. Hierarchy Is Everything

Your eye should know where to look first, second, third. Right now, everything in your UI has equal visual weight. That's chaos.

**Fix it with:**
- Size (bigger = more important)
- Color (brighter = more important)
- Space (more breathing room = more important)
- Contrast (higher contrast = more important)

### 2. Consistency Is Trust

If one button has 4px rounded corners and another has 8px, users feel something is "off" even if they can't name it. Their trust erodes.

**Fix it with:**
- One corner radius everywhere (I recommend 4px)
- One padding scale (8px base, multiples of 8)
- One accent color for all interactive elements
- One font family, limited sizes

### 3. Feedback Is Respect

When users interact with your UI, acknowledge them. Hover states. Press states. Transitions. Without feedback, the UI feels dead.

**Fix it with:**
- Hover: subtle brightness increase or border glow
- Press: slight darkening or scale reduction
- Active: clear visual distinction (the sequencer step that's playing)

---

## The Actual Task

### Phase 1: Theme Foundation (Day 1)

Create `ApplyCustomTheme()` function. Here's your palette:

```cpp
void ApplyCustomTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Backgrounds
    colors[ImGuiCol_WindowBg]           = ImVec4(0.10f, 0.10f, 0.14f, 1.00f);  // #1a1a24
    colors[ImGuiCol_ChildBg]            = ImVec4(0.09f, 0.09f, 0.12f, 1.00f);  // #161620
    colors[ImGuiCol_PopupBg]            = ImVec4(0.12f, 0.12f, 0.16f, 1.00f);  // #1e1e28

    // Headers
    colors[ImGuiCol_Header]             = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);  // #333845
    colors[ImGuiCol_HeaderHovered]      = ImVec4(0.26f, 0.29f, 0.34f, 1.00f);  // #434957
    colors[ImGuiCol_HeaderActive]       = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);

    // Buttons
    colors[ImGuiCol_Button]             = ImVec4(0.91f, 0.27f, 0.38f, 1.00f);  // #e94560 (accent)
    colors[ImGuiCol_ButtonHovered]      = ImVec4(0.95f, 0.35f, 0.45f, 1.00f);  // lighter
    colors[ImGuiCol_ButtonActive]       = ImVec4(0.80f, 0.20f, 0.30f, 1.00f);  // darker

    // Frame (sliders, inputs)
    colors[ImGuiCol_FrameBg]            = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);  // #262633
    colors[ImGuiCol_FrameBgHovered]     = ImVec4(0.20f, 0.20f, 0.26f, 1.00f);
    colors[ImGuiCol_FrameBgActive]      = ImVec4(0.25f, 0.25f, 0.32f, 1.00f);

    // Sliders
    colors[ImGuiCol_SliderGrab]         = ImVec4(0.91f, 0.27f, 0.38f, 1.00f);  // accent
    colors[ImGuiCol_SliderGrabActive]   = ImVec4(0.95f, 0.35f, 0.45f, 1.00f);

    // Text
    colors[ImGuiCol_Text]               = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);  // #ebebeb
    colors[ImGuiCol_TextDisabled]       = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);  // #8c8c8c

    // Borders
    colors[ImGuiCol_Border]             = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_BorderShadow]       = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Rounding & Spacing
    style.WindowRounding    = 6.0f;
    style.ChildRounding     = 4.0f;
    style.FrameRounding     = 4.0f;
    style.GrabRounding      = 4.0f;
    style.PopupRounding     = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.TabRounding       = 4.0f;

    style.WindowPadding     = ImVec2(16, 16);
    style.FramePadding      = ImVec2(8, 6);
    style.ItemSpacing       = ImVec2(8, 8);
    style.ItemInnerSpacing  = ImVec2(8, 4);
}
```

Call this once at startup after `ImGui::CreateContext()`.

---

### Phase 2: Typography (Day 1-2)

Download **Inter** from Google Fonts. It's designed for UIs.

```cpp
ImGuiIO& io = ImGui::GetIO();

// Load fonts at different sizes
ImFont* fontLarge  = io.Fonts->AddFontFromFileTTF("assets/fonts/Inter-SemiBold.ttf", 20.0f);
ImFont* fontNormal = io.Fonts->AddFontFromFileTTF("assets/fonts/Inter-Regular.ttf", 15.0f);
ImFont* fontSmall  = io.Fonts->AddFontFromFileTTF("assets/fonts/Inter-Regular.ttf", 12.0f);
```

Use them intentionally:
- `fontLarge` for section headers ("WAVEFORM", "ENVELOPE")
- `fontNormal` for control labels and values
- `fontSmall` for hints and secondary info

```cpp
ImGui::PushFont(fontLarge);
ImGui::Text("ENVELOPE");
ImGui::PopFont();
```

---

### Phase 3: Layout Structure (Day 2)

Your current layout is `SameLine()` soup. Let's add structure.

**Use child windows for grouping:**

```cpp
// Waveform section
ImGui::BeginChild("WaveformSection", ImVec2(0, 120), true);
ImGui::PushFont(fontLarge);
ImGui::Text("WAVEFORM");
ImGui::PopFont();
ImGui::Spacing();

// ... waveform controls ...

ImGui::EndChild();

ImGui::Spacing();  // breathing room

// Envelope section
ImGui::BeginChild("EnvelopeSection", ImVec2(0, 180), true);
// ...
```

The `true` parameter draws a border. With our theme, it's subtle — just enough to group.

**Spacing rhythm:**
- `ImGui::Spacing()` — 8px (one breath)
- `ImGui::Dummy(ImVec2(0, 16))` — 16px (section break)
- `ImGui::Separator()` — visual line (use sparingly)

---

### Phase 4: ADSR Visualizer (Day 2-3)

This is the centerpiece. When users drag envelope sliders, they see the shape update live.

```cpp
void DrawADSRVisualizer(ImDrawList* draw, ImVec2 pos, ImVec2 size,
                         float attack, float decay, float sustain, float release) {
    // Calculate key points
    float totalTime = attack + decay + 0.3f + release;  // 0.3 = sustain hold time for visualization
    float pixelsPerSecond = size.x / totalTime;

    float x0 = pos.x;                                        // Start
    float x1 = pos.x + attack * pixelsPerSecond;             // Peak (after attack)
    float x2 = x1 + decay * pixelsPerSecond;                 // Sustain start
    float x3 = x2 + 0.3f * pixelsPerSecond;                  // Sustain end
    float x4 = x3 + release * pixelsPerSecond;               // End

    float yBottom = pos.y + size.y;
    float yTop = pos.y;
    float ySustain = pos.y + size.y * (1.0f - sustain);

    // Draw filled area
    ImU32 fillColor = IM_COL32(233, 69, 96, 60);  // Accent with alpha
    draw->AddQuadFilled(
        ImVec2(x0, yBottom), ImVec2(x1, yTop),
        ImVec2(x2, ySustain), ImVec2(x0, yBottom), fillColor);
    draw->AddQuadFilled(
        ImVec2(x2, ySustain), ImVec2(x3, ySustain),
        ImVec2(x3, yBottom), ImVec2(x2, yBottom), fillColor);
    draw->AddTriangleFilled(
        ImVec2(x3, ySustain), ImVec2(x4, yBottom),
        ImVec2(x3, yBottom), fillColor);

    // Draw line
    ImU32 lineColor = IM_COL32(233, 69, 96, 255);  // Accent solid
    draw->AddLine(ImVec2(x0, yBottom), ImVec2(x1, yTop), lineColor, 2.0f);
    draw->AddLine(ImVec2(x1, yTop), ImVec2(x2, ySustain), lineColor, 2.0f);
    draw->AddLine(ImVec2(x2, ySustain), ImVec2(x3, ySustain), lineColor, 2.0f);
    draw->AddLine(ImVec2(x3, ySustain), ImVec2(x4, yBottom), lineColor, 2.0f);

    // Draw dots at key points
    draw->AddCircleFilled(ImVec2(x1, yTop), 4.0f, lineColor);
    draw->AddCircleFilled(ImVec2(x2, ySustain), 4.0f, lineColor);
    draw->AddCircleFilled(ImVec2(x3, ySustain), 4.0f, lineColor);
}
```

**Usage in SynthView:**

```cpp
ImVec2 vizPos = ImGui::GetCursorScreenPos();
ImVec2 vizSize = ImVec2(280, 80);
ImDrawList* draw = ImGui::GetWindowDrawList();

// Reserve space
ImGui::Dummy(vizSize);

// Draw background
draw->AddRectFilled(vizPos, ImVec2(vizPos.x + vizSize.x, vizPos.y + vizSize.y),
                    IM_COL32(22, 22, 32, 255), 4.0f);

// Draw ADSR
DrawADSRVisualizer(draw, vizPos, vizSize,
                   params.attack, params.decay, params.sustain, params.release);
```

This updates every frame. When the user drags Attack, the line moves. That's the "oh, that's nice" moment.

---

### Phase 5: Custom Slider Style (Day 3-4)

ImGui's default sliders are boring. Let's make them feel premium.

```cpp
bool StyledSlider(const char* label, float* value, float min, float max, const char* format) {
    ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 16.0f);
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.91f, 0.27f, 0.38f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1.0f, 0.35f, 0.45f, 1.0f));

    // Custom frame background
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.12f, 0.12f, 0.16f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.16f, 0.16f, 0.20f, 1.0f));

    bool changed = ImGui::SliderFloat(label, value, min, max, format);

    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar();

    return changed;
}
```

Or go further — draw the fill portion of the slider in the accent color:

```cpp
// After ImGui::SliderFloat, overlay a fill rect
ImVec2 sliderMin = ImGui::GetItemRectMin();
ImVec2 sliderMax = ImGui::GetItemRectMax();
float fillWidth = (sliderMax.x - sliderMin.x) * ((*value - min) / (max - min));

ImDrawList* draw = ImGui::GetWindowDrawList();
draw->AddRectFilled(sliderMin, ImVec2(sliderMin.x + fillWidth, sliderMax.y),
                    IM_COL32(233, 69, 96, 100), 4.0f);
```

---

### Phase 6: Value Display (Day 4)

Users want to see the actual numbers, not just slider positions.

**Option A:** Show value to the right of the slider
```cpp
ImGui::SliderFloat("##attack", &params.attack, 0.001f, 0.5f);
ImGui::SameLine();
ImGui::Text("%.3f s", params.attack);
```

**Option B:** Show value on hover (tooltip style)
```cpp
if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Attack: %.3f seconds", params.attack);
}
```

**Option C:** Editable value (click to type)
```cpp
ImGui::SetNextItemWidth(200);
if (ImGui::SliderFloat("Attack", &params.attack, 0.001f, 0.5f, "%.3f s")) {
    // Value changed via slider
}
ImGui::SameLine();
ImGui::SetNextItemWidth(60);
if (ImGui::InputFloat("##attack_input", &params.attack, 0, 0, "%.3f")) {
    // Value changed via input
    params.attack = std::clamp(params.attack, 0.001f, 0.5f);
}
```

Pick one and be consistent.

---

### Phase 7: One Hover Animation (Day 4-5)

Prove we can animate. Pick something simple.

**Button glow on hover:**

```cpp
// Track hover animation state per-button (or use a map)
static float buttonGlow = 0.0f;
float targetGlow = ImGui::IsItemHovered() ? 1.0f : 0.0f;
buttonGlow += (targetGlow - buttonGlow) * 0.15f;  // Smooth

// Draw glow behind button
if (buttonGlow > 0.01f) {
    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();
    ImU32 glowColor = IM_COL32(233, 69, 96, (int)(40 * buttonGlow));

    ImDrawList* draw = ImGui::GetWindowDrawList();
    draw->AddRectFilled(
        ImVec2(min.x - 4, min.y - 4),
        ImVec2(max.x + 4, max.y + 4),
        glowColor, 8.0f);
}
```

Subtle. Professional. Shows that the UI responds to the user.

---

## Deliverables

| Item | Description |
|------|-------------|
| `ApplyCustomTheme()` | Color palette + spacing + rounding |
| Font integration | Inter (or similar), 3 sizes |
| Layout refactor | Child windows, visual grouping, spacing rhythm |
| ADSR visualizer | Real-time envelope display |
| Styled sliders | Accent color, fill indicator |
| Value displays | Numbers visible alongside sliders |
| One animation | Hover glow or similar |
| Before/after screenshot | Prove the difference |

---

## The Bar

I've seen your code. You can write lock-free audio buffers. This is easier than that.

But it requires a different skill: **seeing**. Looking at the screen and noticing what feels wrong. Then fixing it. Then looking again.

Designers iterate visually. You'll iterate visually too.

Every time something looks "off," stop and ask:
- Is the spacing inconsistent?
- Is the hierarchy unclear?
- Is there feedback missing?
- Does this feel intentional or accidental?

If you can answer those questions, you can design.

---

## Timeline

**5 days.** J said 3-5. I'm giving you 5 because I want you to do it right, not fast.

Day 1: Theme + fonts
Day 2: Layout + sections
Day 3: ADSR visualizer
Day 4: Sliders + value display
Day 5: Animation + polish + screenshots

---

## One Last Thing

The ADSR visualizer is the "hero" of this POC. If users see that envelope line moving as they drag sliders, they'll trust the whole app more.

That's the psychology of UI: one moment of delight covers a lot of rough edges.

Make that moment happen.

---

**— Tony**

P.S. — You asked in your feedback: "What does great documentation look like?" This task is what great documentation looks like. Specific. Opinionated. Copy-pasteable code. Clear success criteria. Now go write UI like this.

P.P.S. — If you finish early and want extra credit, add a simple waveform visualizer that shows the current oscillator shape. Same approach as ADSR — just draw lines with ImDrawList. But nail ADSR first.
