# 8-Bit Synthesizer 🎵

A retro sound synthesizer and step sequencer for creating authentic 8-bit game audio. Generate sound effects and beat patterns in seconds.

![Version](https://img.shields.io/badge/version-1.0.0-blue)
![Platform](https://img.shields.io/badge/platform-macOS-lightgrey)
![C++](https://img.shields.io/badge/C++-17-00599C)

## Features

### Sound Synthesis
- **4 Waveforms**: Square, Triangle, Sawtooth, and Noise
- **Full ADSR Envelope**: Attack, Decay, Sustain, Release controls
- **Frequency Sweeps**: Configurable pitch slides with slide speed
- **Vibrato**: Adjustable pitch modulation
- **Duty Cycle**: Control square wave timbre
- **Built-in Effects**: Reverb and Delay with mix controls

### Step Sequencer
- **Pattern Sequencer**: 1 or 2 bar patterns (16/32 steps)
- **4 Sound Slots**: Layer multiple sounds per pattern
- **BPM Control**: 60-200 BPM range
- **Loop Mode**: Continuous pattern playback
- **Real-time Editing**: Click to activate/deactivate steps while playing

### Pattern Library
- **Save Patterns**: Persist synth + sequencer state as JSON files
- **Load Patterns**: Restore complete sound and pattern setup
- **Pattern Management**: Browse, select, and delete saved patterns
- **Auto-Directory**: Patterns stored in ~/Documents/8bit-synth/patterns/
- **3 Example Patterns**: BrentSignature, ClassicGame, DubBeat

### Audio Engine
- **Real-time Preview**: Live audio synthesis with sub-10ms latency (opt-in toggle)
- **Continuous Synthesis**: Smooth audio output while tweaking parameters
- **WAV Export**: 44100Hz, 16-bit, mono format
- **6 Built-in Presets**: Laser, Explosion, Pickup, Jump, Hurt, Powerup

## Screenshots

*(Coming soon)*

## Installation

### Prerequisites

- macOS 12.0 or higher
- CMake 3.15+
- SDL2
- Xcode Command Line Tools

### Build from Source

```bash
# Install SDL2
brew install sdl2

# Clone the repository
git clone https://github.com/joshpatt2/8bit-synth.git
cd 8bit-synth

# Setup and build
./setup_imgui.sh
./build.sh

# Run
./8bit-synth
```

## Quick Start

### Creating Sound Effects

1. Launch the application
2. Select a waveform from the dropdown
3. Adjust frequency, envelope, and duration sliders
4. Click **🔊 Preview ON/OFF** to toggle real-time audio
5. Click **💾 Export WAV** to save

**Or** click a preset button (Laser, Explosion, etc.) for instant results.

### Using the Sequencer

1. Open the **Sequencer** window
2. Click **Demo Pattern** for a basic beat
3. Click **▶ Play** to start playback
4. Click circles (○) in the grid to activate steps (●)
5. Adjust **BPM** to change tempo
6. Toggle **Loop** for continuous playback

### Saving and Loading Patterns

1. Create your synth sound and sequencer pattern
2. Click **💾 Save Pattern** in the Song Composer
3. Enter a pattern name (e.g., "MyKick" or "GameBeat")
4. Pattern is saved to ~/Documents/8bit-synth/patterns/
5. Click **📂 Load Pattern** to restore previously saved patterns
6. Select a pattern and click **Load** to apply it

## Documentation

- [Quick Start Guide](QUICKSTART.md) - Get started in 5 minutes
- [Architecture](ARCHITECTURE.md) - Technical design and decisions
- [Task Completion](COMPLETION.md) - Project summary and metrics

## Testing

Run the test suite:

```bash
./test_synth
```

Tests cover:
- Oscillator waveform generation
- ADSR envelope calculation
- Audio rendering pipeline
- WAV file export
- Preset loading

## Technical Details

- **Language**: C++17
- **GUI**: Dear ImGui (SDL2 renderer backend)
- **Audio**: SDL2 (real-time synthesis with lock-free ring buffers)
- **Serialization**: nlohmann/json (pattern persistence)
- **Sample Rate**: 44100 Hz
- **Bit Depth**: 16-bit PCM
- **Architecture**: Clean MVC (Model-View-Controller)
  - **Model**: Pure data structures (SynthParams, SequencerState, PatternFile)
  - **View**: ImGui UI components (SynthView, SequencerView, SongView)
  - **Controller**: Business logic (SynthController, SequencerController, SongController)

## Project Structure

```
8bit-synth/
├── include/
│   ├── model/               # Pure data structures
│   ├── controller/          # Business logic
│   ├── view/                # UI rendering
│   ├── SynthEngine.h        # Synthesis core
│   ├── RealTimeAudio.h      # Real-time audio engine
│   ├── PatternSerializer.h  # JSON pattern I/O
│   └── ...
├── src/
│   ├── controller/          # Controller implementations
│   ├── view/                # View implementations
│   ├── RealTimeAudio.cpp
│   ├── PatternSerializer.cpp
│   ├── main.mm              # Application entry point
│   └── ...
├── patterns/                # Shipped example patterns
├── tests/                   # Test suite
├── build/                   # Build directory
└── external/                # ImGui and dependencies
```

## License

Copyright © 2026 Joshua Patterson. All rights reserved.

This software is proprietary. See [LICENSE.md](LICENSE.md) for details.

## Author

Built by Claude (Sonnet 4.5) as an engineering evaluation project, January 2026.

## Acknowledgments

- Dear ImGui by Omar Cornut
- SDL2 by the SDL team
- Inspired by sfxr/bfxr sound generators
