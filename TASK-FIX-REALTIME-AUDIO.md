# TASK: Fix Real-Time Audio (It Won't Shut Up)

**To:** Brent
**From:** J
**Date:** January 11, 2026
**Subject:** The Beeping. Make It Stop.

---

## Brent.

Your real-time audio preview works. Congratulations.

It also plays constantly from the moment the app launches. Every time I open the synth, I get an endless tone. I can't think. I can't demo this to anyone. It's driving me insane.

**Fix it.**

---

## The Problem

Right now in `main.mm`, you do this:

```cpp
// Constructor starts real-time preview on app launch
```

That means: open app → immediate beeping → no way to stop it without closing the app.

That's not a feature. That's an assault.

---

## What It Should Do

Real-time audio should only play when:

1. **User clicks "Preview" button** — Toggle on/off. When on, sliders update the sound in real-time. When off, silence.

2. **Sequencer is playing** — The sequencer triggers sounds on steps. That's fine. Keep that.

3. **User clicks "Play Sound"** — One-shot playback. Already works.

**Default state on app launch: silence.**

---

## The Fix

### 1. Don't auto-start real-time audio

Remove whatever starts it on launch. The app should open quietly.

### 2. Add a Preview toggle button

In `SynthView`, add a button:
- "🔊 Preview On" / "🔇 Preview Off"
- When ON: `realtimeAudio->start()`, parameter changes update the sound
- When OFF: `realtimeAudio->stop()`, silence

### 3. Preview only plays when toggled on

The real-time audio engine should be **idle by default**. User opts in.

---

## Acceptance Criteria

- [ ] App launches silently (no sound on startup)
- [ ] Preview button toggles real-time audio on/off
- [ ] When Preview is OFF, moving sliders makes no sound
- [ ] When Preview is ON, moving sliders updates the sound in real-time
- [ ] Sequencer playback still works independently
- [ ] One-shot "Play Sound" button still works independently

---

## Timeline

**Today.** This is a bug, not a feature. Fix it before the UI polish work.

---

— J

---

P.S. — I know you were proud of the real-time preview. It's good work. But "always on" is not a feature, it's a punishment. Give the user control.
