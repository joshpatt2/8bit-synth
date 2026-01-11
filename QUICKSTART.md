# Quick Start Guide

## First Time Setup (5 minutes)

```bash
# 1. Install SDL2 if you haven't
brew install sdl2

# 2. Clone or navigate to the project
cd /path/to/8bit-synth

# 3. Download ImGui and build
./setup_imgui.sh
./build.sh

# 4. Run!
./8bit-synth
```

## Daily Use

```bash
# Just run it
./8bit-synth
```

## Making a Sound Effect in 30 Seconds

1. **Launch** the app (`./8bit-synth`)
2. **Click** a preset button (try "Laser")
3. **Click** the "▶ Play" button to hear it
4. **Tweak** any sliders to customize (try lowering End Frequency)
5. **Click** "💾 Export WAV" to save
6. **Done!** Your `sfx_*.wav` file is ready to use

## Workflow Tips

### Want a specific sound?

**Laser beam:**
- Waveform: Square
- Start: 800 Hz, End: 200 Hz
- Attack: 0.01s, Release: 0.15s

**Explosion:**
- Waveform: Noise
- Start: 1000 Hz, End: 50 Hz
- Longer decay and release

**Coin pickup:**
- Waveform: Triangle
- Start: 400 Hz, End: 800 Hz (upward sweep!)
- Short and punchy

**Jump:**
- Waveform: Square with low duty cycle (0.25)
- Start: 300 Hz, End: 600 Hz
- Quick attack

### No idea what you want?

1. Click "🎲 Randomize" a few times
2. When you hear something interesting, tweak it
3. Presets are your friends - start there and modify

### Understanding the Controls

**Waveform**: The "color" of the sound
- Square = classic beep/bleep
- Triangle = smoother, bass-like
- Sawtooth = buzzy, aggressive
- Noise = static-like, good for explosions

**Start/End Frequency**: The pitch sweep
- Higher = higher pitch
- Start > End = downward sweep (most retro sounds)
- End > Start = upward sweep (pickups, jumps)

**ADSR** (The envelope):
- Attack = how fast it fades in
- Decay = how fast it drops after attack
- Sustain = the held volume level
- Release = how fast it fades out

**Keep these short** for retro sounds (under 0.2s usually).

## File Output

WAV files are saved in the current directory as `sfx_TIMESTAMP.wav`

They're:
- 44100 Hz (standard game audio)
- 16-bit (high quality)
- Mono (saves space)
- Ready to import into Unity, Godot, GameMaker, etc.

## Keyboard Shortcuts

None yet! This is a mouse-driven tool. Could add shortcuts if needed.

## Troubleshooting

**App won't launch:**
- Check you have SDL2: `brew list sdl2`
- Rebuild: `cd build && make clean && make`

**No sound:**
- Check your volume
- Make sure you clicked "Play" after tweaking
- Try a preset first to confirm audio works

**Can't find exported file:**
- It's in the directory where you ran the app
- Look for `sfx_*.wav`

**UI is tiny/huge:**
- ImGui scales with DPI automatically
- If it's wrong, that's a macOS Retina issue (rare)

## What's Next?

You have a working synthesizer. Use it to make sounds for your game.

**Want to modify the code?**
- Read [ARCHITECTURE.md](ARCHITECTURE.md) first
- The synth engine is in `include/`
- The UI is in `src/main.mm`

**Want more presets?**
- Edit `include/Presets.h`
- Add a new function that returns a `SynthParams`
- Add a button in `main.mm` that calls it

**Want to save custom presets?**
- That's not implemented yet
- You'd need to serialize `SynthParams` to JSON
- Left as an exercise (or just copy the values you like)

---

**You're ready. Go make some noise.**
