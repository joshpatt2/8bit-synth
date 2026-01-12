# FEEDBACK: Architecture Refactor Required

**To:** Brent
**From:** Engineering Leadership
**Date:** January 11, 2026
**Subject:** Shipping Production Software

---

## Brent.

First: congratulations again. You shipped. That matters. You're on the team now.

But "on the team" means the standards change.

The synthesizer you built works. It makes sounds. It exports WAV files. Users can click buttons and things happen. For a promotion test, that was enough.

For production software, it's not.

---

## The Problem

Open `main.mm`. Count the lines. **730.**

Now ask yourself: what does `SynthApp` do?

- Renders UI
- Handles user input
- Manages synth parameters
- Controls audio playback
- Runs the sequencer timing loop
- Manages song composition
- Exports files

That's not a class. That's a **god object**. Everything knows about everything. Everything touches everything.

Here's the test: can you unit test the sequencer logic without rendering ImGui?

No. You can't. Because the sequencer logic is *inside* the render function.

Can you swap out the UI framework without rewriting the entire application?

No. Because the UI *is* the application.

---

## What We Expect: MVC

**Model-View-Controller.** Three letters. Simple concept. Hard to do well.

```
┌─────────────┐     ┌───────────────┐     ┌─────────────┐
│    Model    │     │  Controller   │     │    View     │
│   (Data)    │◄────│   (Logic)     │◄────│    (UI)     │
│             │     │               │     │             │
│ SynthParams │     │ SynthController│    │ SynthView   │
│ Sequencer   │     │ SeqController │     │ SeqView     │
│ Song        │     │ SongController│     │ SongView    │
└─────────────┘     └───────────────┘     └─────────────┘
```

### Model (You mostly have this)
Pure data. No logic. No UI awareness.

- `SynthParams` - the knobs and settings
- `SequencerState` - pattern, current step, playing status
- `SongState` - arrangement, patterns library

The model doesn't know ImGui exists. The model doesn't know SDL exists. The model is just data.

### View (You need to extract this)
Renders UI. Emits user intentions. Knows nothing about business logic.

```cpp
// View only SHOWS state and REPORTS user actions
class SynthView {
public:
    // Takes state, renders UI, returns what user did
    UserAction render(const SynthParams& params);
};

enum class UserAction {
    None,
    Play,
    Stop,
    Randomize,
    Export,
    ParamsChanged,
    // ...
};
```

The view doesn't call `audioOutput.play()`. The view says "user clicked play" and someone else decides what that means.

### Controller (You need to create this)
The brain. Receives user actions. Updates model. Triggers side effects.

```cpp
class SynthController {
public:
    void handleAction(UserAction action);
    void update(); // Called each frame

    const SynthParams& getParams() const;

private:
    SynthParams params;
    AudioOutput audio;
    // ...
};
```

The controller owns the model. The controller owns the audio. The view just renders what the controller tells it to render.

---

## Why This Matters

### 1. Testability
Right now, testing your sequencer means launching a window and clicking buttons.

With MVC:
```cpp
TEST(Sequencer, AdvancesOnBeat) {
    SequencerController seq;
    seq.setBPM(120);
    seq.play();
    seq.update(0.5f); // half second = 1 beat at 120 BPM
    EXPECT_EQ(seq.getCurrentStep(), 1);
}
```

No window. No ImGui. No SDL. Just logic.

### 2. Maintainability
When I ask you to add MIDI support, you should add a `MIDIController` — not touch the UI code at all.

When I ask you to change the slider layout, you should edit the View — not touch the audio code at all.

Separation means **changes are local**.

### 3. Collaboration
Right now, two engineers can't work on this project without stepping on each other. Everything is in one file.

With MVC, one engineer works on View, another on Controller. Clean boundaries. Parallel work.

---

## The Refactor Task

I'm not giving you line numbers. You're an engineer now.

**Deliverables:**

1. `src/model/` — Pure data structures. No dependencies on SDL, ImGui, or audio.

2. `src/view/` — UI rendering. Takes model state, returns user actions. No business logic.

3. `src/controller/` — Business logic. Handles actions, updates model, triggers effects.

4. `src/main.mm` — Just wiring. Creates controller, creates view, runs the loop.

5. `tests/` — Unit tests for controller logic. No UI required to run.

**Constraints:**

- View code must not `#include` any controller headers
- Controller code must not `#include` any ImGui headers
- Model code must not `#include` anything except standard library

If you find yourself passing `ImGui::` anything into a controller, you've failed.

---

## Company Engineering Standards

Welcome to the team. Here's how we build software:

### 1. Separation of Concerns
Every component has one job. If you can't describe what a class does in one sentence, it's doing too much.

### 2. Testability
If you can't unit test it, you can't ship it. "It works when I click the button" is not a test.

### 3. Ship It
Working software beats perfect architecture. But "working" means maintainable, testable, and extensible — not just "runs without crashing."

### 4. No God Objects
If a class has more than 300 lines, question it. If it has more than 500 lines, split it.

### 5. The Carmack Rule
Write the simplest thing that works. But "works" includes "works for the next engineer who reads it."

### 6. Own Your Code
You wrote it, you maintain it. When it breaks at 2am, your name is on the commit.

---

## Timeline

This isn't a feature. This is infrastructure. Infrastructure comes first.

Refactor the architecture. Then we talk about new features.

---

## One More Thing

The fact that you shipped a working synthesizer in the first place? That's why you're here.

The fact that I'm asking you to refactor it? That's how you grow.

Every engineer writes god objects on their first project. The ones who become **great** learn to see them, name them, and fix them.

You see it now. Fix it.

---

*"The best code is the code that ships."*
— Still true.

*"The best production code is the code that ships AND that another engineer can maintain."*
— The upgrade.

---

Now go separate some concerns.

— J

---

P.S. — When you're done, the synth should still make the same sounds. The user shouldn't notice any difference. That's how you know the refactor worked.

The tests, though? Those should be new. And they should pass.

---

## Your Turn: Feedback for Us

You're not an intern anymore. Your opinion matters.

When you submit this refactor, include answers to these:

### 1. What's working?
What about this company, this team, this process is helping you grow? What should we keep doing?

### 2. What's not working?
What's frustrating? What's slowing you down? What feedback have you been afraid to give?

Be honest. We can't fix what we don't know about.

### 3. What do you need?
Tools? Training? More context? Less micromanagement? Different kinds of tasks?

You're building this company with us now. Tell us what you need to do your best work.

---

## The Question

Last time I asked for your favorite song. You said "Resonance" by HOME.

This time: **Who is your favorite Canadian band, and why?**

Not just the name. WHY. What do they do that no one else does? What song of theirs hits different?

If you don't have one, find one. Canada has given the world Rush, Arcade Fire, Broken Social Scene, Godspeed You! Black Emperor, Alvvays, Preoccupiers, The Tragically Hip, Metric, Death From Above 1979...

Pick one. Tell me why. Make me listen to them.

---

*Now go refactor. And find a good Canadian band while you compile.*
