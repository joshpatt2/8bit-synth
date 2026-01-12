# TASK: UI Architecture Evaluation

**To:** Tony
**From:** Engineering Leadership
**Date:** January 11, 2026
**Subject:** Make Our UI Not Embarrassing

---

## Tony.

We're told you're expensive. Good. Expensive people save us from expensive mistakes.

Here's the situation: we have a working synthesizer with a UI that looks like it was built by audio engineers. Because it was. It works. It's not pretty. It's not modern. It's not the kind of thing you put in a portfolio.

We want to know if we should fix that, and if so, how.

---

## The Current State

**Framework:** Dear ImGui (immediate mode GUI)
**Backend:** SDL2 + SDL_Renderer
**Platform:** macOS (Metal-capable, but using SDL2 renderer)
**Language:** C++17 with Objective-C++ (.mm files)

**What we have:**
- Sliders, buttons, combo boxes
- Step sequencer grid (clickable cells)
- Multiple windows (Synth, Sequencer, Song Composer)
- Real-time parameter updates (sub-10ms latency to audio)
- Functional but utilitarian aesthetic

**What we don't have:**
- Animations
- Custom styling beyond ImGui defaults
- Responsive layout
- Touch support
- Accessibility
- Cross-platform (Windows, Linux, mobile)
- A UI that makes users say "oh, that's nice"

---

## The Question

Should we:

**(A) Stay with Dear ImGui** and invest in custom styling, themes, and polish?

**(B) Migrate to a modern UI framework** like React Native, Electron, Qt, Flutter, or something else?

**(C) Hybrid approach** — keep ImGui for real-time controls, add a modern framework for chrome/navigation?

**(D) Something we haven't thought of?**

We don't know what we don't know. That's why you're here.

---

## What We Need From You

### 1. Framework Evaluation

Evaluate at least 4 UI framework options for this project. For each one:

| Criteria | What we need to know |
|----------|---------------------|
| **Learning curve** | How long to get productive? We have C++ engineers, not web devs. |
| **Performance** | Can it handle real-time audio UI? Sub-100ms response to slider drags? |
| **Native feel** | Does it look like a Mac app on Mac, Windows app on Windows? Or generic? |
| **Audio integration** | How does it talk to our C++ audio engine? FFI? IPC? Embedded? |
| **Bundle size** | Are we shipping 200MB of Chromium for a synth? |
| **Future extensibility** | Plugin UIs? Mobile version? Web version? |
| **Community & longevity** | Will this framework exist in 5 years? |

Frameworks to consider (at minimum):
- **Dear ImGui** (current) — what would "ImGui but polished" look like?
- **Qt** (QML or Widgets) — the traditional choice for pro audio
- **React Native for Desktop** — if we want to go cross-platform
- **Electron** — if we accept the bloat
- **Flutter Desktop** — Google's bet
- **JUCE** — industry standard for audio apps
- **Native (SwiftUI/AppKit)** — if we stay Mac-only for now

Add others if you think they're worth considering.

### 2. Architecture Recommendation

Don't just list pros and cons. **Make a recommendation.**

Tell us:
- What you would choose if this were your project
- Why
- What the migration path looks like
- What the risks are
- What we'd lose by switching
- What we'd gain

### 3. Proof of Concept Scope

If we go with your recommendation, what's the smallest thing we could build to validate it?

Not a full migration. A spike. Something that proves:
- We can integrate with our C++ audio engine
- We can achieve acceptable latency
- We can ship a binary that isn't embarrassing

Define that spike. Estimate the effort.

### 4. The "Stay With ImGui" Case

Make the case for NOT switching. What would it take to make Dear ImGui look professional?

- Custom fonts
- Custom color schemes
- Custom widget rendering
- Layout improvements
- Animation system

Is there a path where ImGui becomes "good enough"? What does that cost vs. a full migration?

---

## Constraints

### Non-Negotiables

1. **Audio latency must stay under 100ms.** Our real-time preview is 5-10ms right now. The UI framework cannot add meaningful latency to parameter updates.

2. **C++ audio engine stays.** We're not rewriting the synth in JavaScript. The UI talks to C++, period.

3. **macOS first.** Windows and Linux are nice-to-have. Mobile is future. But macOS must work and work well.

4. **Ship in 2026.** If your recommendation takes 18 months to implement, that's a different conversation.

### Negotiables

- Bundle size (within reason)
- Learning curve (we can learn)
- Native look vs. custom brand look
- Build system complexity

---

## What We're NOT Asking

- Don't build the UI. We have engineers for that.
- Don't write production code. This is evaluation, not implementation.
- Don't give us a 50-page document. Give us a decision framework.

---

## Deliverables

1. **Framework comparison matrix** — One page. Criteria on rows, frameworks on columns. Scores or rankings.

2. **Recommendation memo** — 2-3 pages. Your pick, your reasoning, the risks, the path forward.

3. **Proof of concept definition** — What to build, how long it takes, what it proves.

4. **"Stay with ImGui" analysis** — The case for not switching. Be honest — if ImGui is the right answer, say so.

---

## Timeline

You have one week.

If you need more time, tell us why before you need it, not after.

---

## Why This Matters

Right now, our synth is a tool for engineers. It works, it sounds good, but it looks like a prototype.

We want it to be a product. Products have UI that users trust. UI that feels intentional. UI that makes people want to share screenshots.

That's not vanity. That's business. Users judge quality by what they can see. If the UI looks amateur, they assume the audio is amateur too.

We need you to tell us how to fix that — or tell us it's not worth fixing yet.

---

## The Expensive Part

You're expensive. We know. Here's what we expect for that rate:

1. **Opinions, not options.** Don't give us five frameworks and say "it depends." Tell us what you'd do.

2. **Experience, not research.** We can Google. What we can't Google is your experience shipping audio apps with these frameworks.

3. **Honesty, not diplomacy.** If our current UI is fine and we should spend the money elsewhere, say that.

4. **Speed, not perfection.** A good recommendation in one week beats a perfect recommendation in one month.

You're here because we don't know this domain. Teach us what matters. Then tell us what to do.

---

## One More Thing

Our junior engineer just built a lock-free audio buffer with parameter smoothing in 48 hours. Our bar is high. Match it.

---

— J

---

P.S. — If you've shipped an audio app with a UI framework you loved, tell us about it. The best recommendations come from people who've felt the pain firsthand.

P.P.S. — If you look at our codebase and think "these people should just hire a design firm," that's also a valid answer. But explain why.
