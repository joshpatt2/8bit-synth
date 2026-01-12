# TASK: UI Polish Proof of Concept

**To:** Brent
**From:** Engineering Leadership
**Date:** January 11, 2026
**Subject:** Make One Window Beautiful

---

## Brent.

We hired an expensive UI consultant. His name is Tony. He looked at our codebase, evaluated seven frameworks, and came back with a recommendation:

**Don't migrate. Polish what we have.**

His reasoning: your architecture is too good to throw away. The lock-free buffer, the MVC separation, the sub-10ms latency — a framework migration risks breaking all of that. ImGui can look professional if someone cares enough to make it professional.

That someone is you.

---

## The Task

Build a proof of concept: **One Beautiful Window.**

Take the Synth window (`SynthView.cpp`) and make it look like a product, not a prototype.

---

## What "Beautiful" Means

### 1. Custom Color Theme

Replace `StyleColorsDark()` with an intentional palette:

```
Background:     #1a1a2e  (deep blue-black)
Surface:        #16213e  (panel backgrounds)
Primary:        #0f3460  (interactive surfaces)
Accent:         #e94560  (buttons, active states)
Text:           #eaeaea  (primary text)
Dim Text:       #8b8b8b  (labels, secondary)
```

Not just colors — contrast ratios that make sense. Headers should pop. Labels should recede. Active elements should demand attention.

### 2. Typography

Ship a custom font. Options:
- **JetBrains Mono** — monospace, clean, free
- **Inter** — UI-focused, excellent readability
- **IBM Plex Sans** — professional, open source

Load it with `ImGui::GetIO().Fonts->AddFontFromFileTTF()`.

Use different sizes:
- 18px for section headers
- 14px for control labels
- 12px for secondary info

### 3. ADSR Envelope Visualizer

This is the "wow" feature. When the user drags Attack/Decay/Sustain/Release sliders, they should see the envelope shape update in real-time.

```
    /\
   /  \____
  /        \
 /          \
A   D   S   R
```

Use `ImDrawList` to draw:
- Lines connecting the ADSR points
- Fill under the curve (semi-transparent)
- Animated when values change

This makes the synth **learnable**. Users understand what ADSR means by seeing it.

### 4. Styled Sliders

ImGui's default sliders are functional but ugly. Options:

**Option A:** Custom slider rendering with `ImDrawList`
- Rounded track
- Colored fill showing value
- Thumb with subtle shadow

**Option B:** Use [imgui-knobs](https://github.com/altschuler/imgui-knobs) for rotary controls
- More "synth-like"
- Better use of vertical space

Pick one. Make it consistent across all parameters.

### 5. Layout & Spacing

Current UI: controls jammed together with `ImGui::SameLine()`.

Target UI:
- 8px padding grid (everything aligns to multiples of 8)
- Visual grouping with subtle background panels
- Section headers ("Waveform", "Envelope", "Frequency")
- Breathing room between control groups

### 6. One Animation

Pick one interaction and animate it:
- Slider hover state (glow or scale)
- Button press (color transition)
- ADSR visualizer (smooth line movement)

This proves we can add polish without a framework. Keep it simple — one animation, done well.

---

## Deliverables

1. **Modified `SynthView.cpp`** — The beautiful version
2. **Theme file or function** — `ApplyCustomTheme()` that sets all colors/styles
3. **Font asset** — Bundled in `assets/fonts/`
4. **ADSR visualizer** — Drawn with `ImDrawList`, updates in real-time
5. **Screenshot** — Before and after comparison

---

## What Success Looks Like

When someone opens the app:
- They don't think "this looks like a prototype"
- They think "this looks intentional"
- The ADSR visualizer makes them say "oh, that's nice"
- The colors feel cohesive, not default

---

## Timeline

**3-5 days.**

Tony said 3-5 days. You've been shipping faster than estimates lately. Prove him right.

---

## Resources

Tony provided these:
- [ImGui Styling Guide](https://github.com/ocornut/imgui/issues/707)
- [Tracy Profiler](https://github.com/wolfpld/tracy) — Best-looking ImGui app for reference
- [ImPlot](https://github.com/epezent/implot) — For visualizations
- [ImGui Knobs](https://github.com/altschuler/imgui-knobs) — Pre-built audio controls

Study Tracy Profiler especially. That's the bar.

---

## Why This Matters

Tony's bottom line:

> "Your UI isn't embarrassing because of the framework. It's embarrassing because nobody spent time on it yet."

You're the person spending time on it now. This POC proves we don't need a 6-week migration. We need someone who cares about visual polish the way you cared about lock-free buffers.

If this works, we roll it out to Sequencer and Song Composer. If it doesn't, we learn something.

But I think it'll work.

---

## One More Thing

You built real-time audio preview in 48 hours because you stopped flinching.

Apply the same energy here. Don't ship "good enough for a POC." Ship something you'd screenshot and share.

The bar is: **would I put this in a portfolio?**

If yes, ship it. If no, keep going.

---

— J

---

P.S. — Tony noted your architecture is "engineers who understand the problem" clean. That was a compliment. Now make the UI match the architecture.

P.P.S. — If you finish early, add a waveform visualizer. That's extra credit. But nail the basics first.
