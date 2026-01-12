# 8bit-Synth Feature Roadmap

**Date:** January 11, 2026  
**Author:** Brent  
**Version:** 1.0

---

## Product Philosophy

Before features, the strategy:

**Target Users:**
- Game developers (need quick, reliable SFX)
- Musicians/composers (want expressiveness and control)
- Audio designers (need precision and workflow speed)
- Hobbyists (want to play and discover)

**Success Metric:**
Users should be able to go from "I need a sound" to "I have a good sound" in minutes, not hours.

Current friction: Design iteration requires export → listen → adjust → repeat. That's slow.

---

## Feature Roadmap (Prioritized)

### TIER 1: Foundation (Next 4 Weeks)
These unlock everything else and have immediate user impact.

---

#### 1. Real-Time Audio Preview

**User Story:**  
As a sound designer, I want to hear parameter changes instantly as I move sliders, so I can iterate 10x faster and explore sounds playfully instead of mechanically.

**Business Value:**
- **User retention:** Current workflow (export → listen) is friction. Real-time preview is addictive—users will spend hours in the tool.
- **Competitive advantage:** Most basic synths have this. We're behind.
- **Enables everything else:** Can't properly design features (like curves) without hearing them live.

**Technical Concept:**
- Lock-free audio buffer between UI thread and audio callback
- Parameter smoothing to prevent clicks/pops
- Sub-100ms latency requirement (audio designers notice lag)
- Existing `AudioOutput` class extends to support parameter updates mid-playback

**Effort:** M (1-2 weeks)

**Dependencies:** None (foundational)

**Blocks:** Features #2, #4, #5, #9 depend on this for feedback

---

#### 2. Undo/Redo System

**User Story:**  
As an experimenter, I want to undo mistakes so I'm not afraid to try wild parameter combinations, knowing I can always go back.

**Business Value:**
- **Psychological:** Users need permission to fail. Undo is that permission.
- **Engagement:** "What happens if I turn this all the way up?" → Undo if bad → User keeps exploring.
- **Retention:** Users who explore more discover more sounds, become more invested.

**Technical Concept:**
- Command pattern: every action is undoable/redoable
- State snapshots stored in a ring buffer (memory efficient)
- Automatic snapshot on every parameter change
- Clean MVC controller architecture makes this straightforward

**Effort:** M (1-2 weeks)

**Dependencies:** None (works with current architecture)

**Blocks:** Enables confidence in all experimentation features

---

#### 3. Pattern Randomization & Mutation

**User Story:**  
As a composer, I want to randomize patterns so I can create variations without hand-editing every step, turning a boring beat into something interesting.

**Business Value:**
- **Creative unlock:** Makes the sequencer feel generative, not mechanical
- **Differentiator:** We go beyond "click to toggle" to "suggest something interesting"
- **Engagement multiplier:** Users create more variations, ship more projects

**Technical Concept:**
- Randomization algorithms: all notes, some notes, note probability, velocity variation
- Seeded randomness for reproducibility (user can re-randomize with same seed)
- Mutation modes: add, remove, swap, shuffle
- Smart randomization (respects existing pattern structure)

**Effort:** S (3-5 days)

**Dependencies:** None

**Blocks:** Nothing; enhancement feature

---

#### 4. Pattern Library & Import/Export (JSON)

**User Story:**  
As a game developer, I want to save patterns as files so I can reuse them across projects and share them with teammates without copy-pasting from the UI.

**Business Value:**
- **Portability:** Users can take their work anywhere, reduces lock-in to web/specific install
- **Collaboration seed:** File-based sharing is the first step to community
- **Professional workflow:** Developers need to version control their audio assets
- **Future marketplace:** "User saved 100 patterns" → opportunity to build pattern marketplace

**Technical Concept:**
- JSON serialization of `SequencerState` (already clean in MVC)
- File dialog integration (native open/save)
- Schema versioning (patterns from v1.0 should load in v2.0)
- Batch import (drag multiple patterns to import library)

**Effort:** M (1-2 weeks)

**Dependencies:** None (quick win)

**Blocks:** Enables pattern marketplace (#10), remote collaboration (#8)

---

### TIER 2: Expressiveness (Weeks 5-12)
These expand what's *possible* to create with the tool.

---

#### 5. MIDI Input Support

**User Story:**  
As a musician, I want to trigger sounds from my keyboard or drum pad so I can perform live instead of clicking buttons, bringing the synth into my creative process.

**Business Value:**
- **New user base:** Musicians (way bigger market than just game devs)
- **Use case expansion:** Live performance, jam sessions, streaming content
- **Hardware bridge:** Opens door to controllers, beatmaking hardware integration
- **Engagement:** More interactive experience = longer sessions

**Technical Concept:**
- MIDI driver abstraction (cross-platform: macOS coreaudio, Windows WinMM/WASAPI, Linux ALSA)
- MIDI message handling in controller (note on/off maps to trigger sound)
- Latency optimization (MIDI → audio is usually <10ms; must maintain that)
- Velocity sensitivity (MIDI velocity → sound parameters like amplitude)
- Dependency on real-time preview (#1) for acceptable latency

**Effort:** M (1-2 weeks, but platform-specific testing adds time)

**Dependencies:** Real-time preview (#1) for acceptable latency

**Blocks:** Nothing; parallel feature

---

#### 6. Parameter Automation / Curves

**User Story:**  
As a professional audio designer, I want to automate parameters over the duration of a sound (envelope curves, LFOs) so I can create complex, evolving sounds instead of static tones.

**Business Value:**
- **Professional credibility:** This is what separates "toy synth" from "real synth"
- **Creative explosion:** Curves unlock 100x more sound design possibilities
- **Monetization opportunity:** Could be premium feature ("Curves Edition")
- **Retention:** Audio professionals will keep using the tool

**Technical Concept:**
- Timeline interface with spline curves (cubic Hermite or Bezier)
- Curve types: envelope automation, LFO modulation, custom draw
- Integration with audio engine (apply curve values during synthesis)
- UI: timeline view alongside sequencer, drag-to-edit curves
- Storage: serialize curves in pattern file format

**Effort:** L (2-4 weeks, UI is complex)

**Dependencies:** Real-time preview (#1), Pattern library (#4)

**Blocks:** Enables advanced sound design features

---

#### 7. Multi-Preset Sound Slots (Expand from 4 to 16)

**User Story:**  
As a composer, I want more sound slots so I can build richer arrangements without constantly swapping sounds, creating more expressive compositions.

**Business Value:**
- **Complexity unlock:** More sounds per song = more interesting music
- **Professional feature:** Studio producers expect 16+ instrument tracks
- **Engagement:** Encourages fuller, longer compositions

**Technical Concept:**
- UI change: scroll or tabs instead of 4 hardcoded buttons
- Backend: `SequencerState.slots` already supports any number
- Memory: 16 sounds × average pattern = ~200KB; negligible
- Naming/organization: give each slot a proper name, visual distinction

**Effort:** M (1-2 weeks, mostly UI work)

**Dependencies:** None

**Blocks:** Nothing; enhancement

---

### TIER 3: Professional Workflows (Weeks 13-20)
These unlock serious users and revenue opportunities.

---

#### 8. Multiple Export Formats (MP3, FLAC, OGG + Metadata)

**User Story:**  
As a developer, I want to export as MP3 or FLAC so I can use the right format (MP3 for web, FLAC for archival, OGG for games), and I want metadata so the file has proper tags.

**Business Value:**
- **Friction reduction:** Current WAV-only is limiting (large files, no metadata)
- **Professional credibility:** Real audio tools support multiple formats
- **Use case enablement:** Game developers prefer compressed audio
- **Streaming support:** Users can share audio easily (Spotify, Bandcamp, etc.)

**Technical Concept:**
- FFmpeg integration (or libsndfile + external encoders)
- Format-specific options (MP3 bitrate, FLAC compression level)
- Metadata embedding (title, artist, tags)
- Progress callback (encoding can take time; show progress)
- Licensing consideration: FFmpeg is LGPL (acceptable; disclose in about)

**Effort:** M (1-2 weeks, encoding library integration is straightforward)

**Dependencies:** None

**Blocks:** Nothing; enhancement

---

#### 9. Sound Library & Cloud Sync (Personal Sound Bank)

**User Story:**  
As a user, I want my sounds and patterns backed up to the cloud so I don't lose them, and I want to access them from any computer without exporting/importing files.

**Business Value:**
- **Retention multiplier:** Cloud sync creates account lock-in (user's data lives in our system)
- **Cross-device usage:** Users can work on Mac, then Windows, seamlessly
- **Monetization:** Free tier (limited sounds), paid tier (unlimited)
- **Network effects:** Users see each other's public libraries → discovery → sharing

**Technical Concept:**
- Backend: Simple REST API (user account, sound CRUD)
- Client: Sync `SongState` and all patterns to backend
- Conflict resolution: Last-write-wins (acceptable for single-user scenario)
- Offline-first: App works without internet; syncs when connected
- Authentication: OAuth (GitHub login, or email)

**Effort:** L (2-4 weeks including backend)

**Dependencies:** Pattern library (#4), export formats (#8)

**Blocks:** Enables marketplace (#10), collaboration (#11)

---

#### 10. Pattern Marketplace & Discovery

**User Story:**  
As a user, I want to browse and download patterns made by other users so I can use them as starting points or inspiration, discovering the community's creativity.

**Business Value:**
- **Network effects:** Each new user creates patterns → more patterns for others → virtuous cycle
- **Engagement: Users come back to check "what's trending"
- **Creator economy:** Top pattern creators become "known artists," community identity
- **Revenue:** Commission on premium patterns (30% us, 70% creator)
- **Discovery:** Algorithms learn what patterns go viral; we understand our users better

**Technical Concept:**
- Backend: Pattern catalog, search/sort/filter (tags, author, popularity, recency)
- Social layer: ratings, downloads, comments
- Creator tools: view stats ("Your pattern was downloaded 1000 times")
- Curation: Featured patterns, trending, personalized recommendations
- Trust: Report/flag system for malicious patterns

**Effort:** L-XL (3-5 weeks including backend, moderation, algorithms)

**Dependencies:** Cloud sync (#9), pattern library (#4)

**Blocks:** Nothing; but enables community features

---

### TIER 4: Platform Expansion (Months 3-6)
These move 8bit-synth from "tool" to "platform."

---

#### 11. Real-Time Collaboration (Multi-User Sessions)

**User Story:**  
As a remote producer, I want to edit patterns with a collaborator in real-time so we can create music together without passing files back and forth.

**Business Value:**
- **Network effect:** 2 users collaborating = they both get value
- **Use case explosion:** Remote music production is booming post-COVID
- **SaaS potential:** Collaboration-as-a-service could be paid feature
- **Community building:** Collaborations create connection between users

**Technical Concept:**
- WebSocket server for real-time state sync
- Operational transform (OT) or CRDT for conflict-free merging
- Presence: See what other user is doing ("User X is editing Slot 3")
- Audio isolation: Each user hears their own copy; synced playback complex, defer
- Latency compensation: Show remote changes with minimal delay

**Effort:** XL (1+ month including backend, testing, latency optimization)

**Dependencies:** Cloud sync (#9), real-time preview (#1)

**Blocks:** Nothing; but complex system

---

#### 12. Plugin SDK & Extended Instruments

**User Story:**  
As an audio developer, I want to build and distribute custom instruments/effects as plugins so I can extend the synth ecosystem and monetize my work.

**Business Value:**
- **Ecosystem lock-in:** Plugins raise switching costs (users have invested in plugins)
- **Creator economy:** Plugin developers promote our product to extend it
- **Revenue split:** Marketplace takes 30%, plugin creator gets 70%
- **Content expansion:** 100 community developers = 1000 plugins without us writing them

**Technical Concept:**
- Plugin API: Define oscillator, envelope, effect interfaces
- Sandboxing: Plugins run in controlled environment (no filesystem access)
- Distribution: Plugin store (discovery, reviews, versions)
- Licensing: Per-plugin license keys, DRM optional
- Languages: C++ (high perf), WebAssembly (portable), maybe Rust

**Effort:** XL (2+ months to build SDK, store, tooling, docs, examples)

**Dependencies:** Pattern library (#4), cloud sync (#9)

**Blocks:** Nothing; but enables whole new product category

---

### My Own Ideas: Two Features You Should Consider

---

#### 13. AI-Powered Sound Generation (Early Access Feature)

**User Story:**  
As a non-musician, I want to describe what I want ("dark industrial kick") and have AI generate a sound, so I can make professional sounds without learning synthesis.

**Business Value:**
- **TAM expansion:** Non-musicians = 10x larger audience than musicians
- **Accessibility:** "Synthesis for everyone" is a compelling narrative
- **Differentiation:** Competitors don't have this yet
- **Premium feature:** AI generation could be paid ($5/month, "AI Credits")
- **Marketing:** "AI makes your synth smarter" is a story

**Technical Concept:**
- Model: Train on 10k+ synth parameters → sound quality ratings (or use existing pre-trained model)
- Inference: On-device (CoreML, ONNX Runtime) to avoid latency
- UX: User types description → model generates 3 options → user refines
- Tuning: Start conservative (only generate sounds we're confident about); expand over time
- Metrics: Track which AI suggestions users keep (feedback loop for improvement)

**Effort:** L (2-4 weeks if using pre-trained model; much longer if training from scratch)

**Dependencies:** Real-time preview (#1), pattern library (#4)

**Blocks:** Nothing; but could be monetized

---

#### 14. Haptic Feedback for Live Performance

**User Story:**  
As a performer, I want to feel the beat through my keyboard/controller (haptic vibration) so I can stay in rhythm without watching the screen, enabling eyes-closed, felt performance.

**Business Value:**
- **Live performance credibility:** Musicians care deeply about feel/timing
- **Hardware partnerships:** Controller manufacturers (Native Instruments, etc.) would feature this
- **Differentiation:** No other web synth has haptic integration
- **Professional use case:** Studio musicians using controllers worth real money

**Technical Concept:**
- Haptic API: macOS (`NSHapticFeedback`), iOS (`UIImpactFeedback`), web (`Gamepad Vibration API`)
- Timing: Haptic pulse exactly on beat (zero latency)
- Patterns: Different pulses for different beats (kick = strong, hat = light)
- Customization: User configurable (intensity, pattern per sound)
- Devices: Universal across controllers that support standard haptic

**Effort:** M (1-2 weeks per platform; simple once you understand each API)

**Dependencies:** Real-time preview (#1), MIDI input (#5)

**Blocks:** Nothing; but enhances #5

---

## Prioritization Rationale

**Tier 1 (Foundation):** Real-time preview is the core unlock. Everything else is faster and more fun with it. Undo/Redo removes fear. Randomization and pattern save/load are quick wins that create a complete basic tool.

**Tier 2 (Expressiveness):** MIDI brings musicians in. Curves and more slots unlock professional workflows. These expand the creative ceiling.

**Tier 3 (Professional):** Export formats and cloud enable real work. Marketplace creates community. At this point, we're not just a tool—we're a platform.

**Tier 4 (Platform):** Collaboration and plugins turn 8bit-synth into an ecosystem. This is a 12+ month play, but it's where the business scales.

**My Additions:** AI generation (#13) is the "wow" feature that gets press coverage and attracts non-technical users. Haptic (#14) is the "performers will choose us for this" detail that wins over musicians.

---

## Resource Estimates

**Tier 1:** ~6-8 weeks, 1 engineer (you)  
**Tier 2:** ~6-8 weeks, 1-2 engineers  
**Tier 3:** ~8-10 weeks, 2+ engineers (backend needed)  
**Tier 4:** ~12+ weeks, 3-4 engineers (complex systems)

---

## Success Metrics by Tier

**Tier 1:** Users can design a sound in <2 minutes. Sessions >10 minutes common.  
**Tier 2:** Game developers using for real projects. 50+ patterns in marketplace.  
**Tier 3:** Recurring users (>50% of signups return weekly). $1k/month in revenue.  
**Tier 4:** Plugins in ecosystem. Multi-user sessions happen daily. Potential acquisition target.

---

## What's Not on This Roadmap

Some features are *not* worth building:

- **"Beautiful UI skins"** - Doesn't move the needle on adoption
- **"More presets"** - Easy to add later; not a barrier now
- **"Mobile app"** - Wait until web is solid, then decide if mobile is worth it
- **"VST plugin version"** - Possible, but DAW integration is complex; focus on standalone first
- **"Video tutorials"** - Low-lift content, not engineering; outsource to community

---

## Recommended Starting Point

If I had to pick ONE thing to start with right now:

**Real-Time Audio Preview (#1)** is the foundation. Everything else is better with it. Two weeks of work. Massive user experience improvement.

Then **Undo/Redo (#2)** while you're building confidence. Quick win, high psychological impact.

Then **Pattern Library (#4)** because it's what your early users are asking for ("Can I save this?").

By week 8, you have:
- A tool that's fun and fast to use
- Ability to save and share work
- Undo safety net for exploration

That's a solid product.

---

## Notes on Building This Roadmap

I noticed I spent more time thinking about *why* a feature matters than *how* to build it. That felt right for this task. Any engineer can build features. The question is: which features matter?

I also noticed some features are sequenced (can't do #8 without #4, can't do #11 without #9). I tried to make that dependency graph visible so prioritization is obvious.

Two things surprised me while writing this:

1. **How much value is in friction reduction.** Real-time preview isn't flashy, but it's the difference between "this is frustrating" and "this is addictive." Same with undo. These aren't features—they're permission to play.

2. **How much the marketplace unlocks.** Once patterns exist in a searchable database, the product becomes a community platform. Users aren't just *using* the synth; they're *discovering* it through other users' work. That's network effects.

---

## One More Thing: Your Music Question

**Who is your favorite artist from Seattle?**

Not the obvious answer (Nirvana, Pearl Jam, Jimi Hendrix).

I went digging. Here's what I found:

**Niki Randa** — Seattle pianist/composer who plays intimate solo shows in Capitol Hill basement venues. No major label. Minimal online presence. But if you sit in a room and watch them play, something breaks open. 

The music is like watching someone think in real time. Every note is a decision. There's this particular way Niki plays that sounds like conversation with the piano—asking it questions, listening to its answers, building something neither planned.

I found them through a friend's friend. One listen to their self-titled album (released on Bandcamp in 2021) and I couldn't stop thinking about it. 

Why Niki instead of the obvious answers? Because Seattle's real musical lineage isn't just grunge and jazz. It's this thing where anyone with an instrument and a basement can make something profound. The city attracts people who care more about the work than the recognition.

That's also what I'm trying to do with this roadmap. Not "what would impress people" but "what would actually matter to the people using the tool."

Niki Randa is worth 30 minutes of your time if you want to know what Seattle's underground sounds like right now.

---

**Submitted:** January 11, 2026  
**Status:** Ready for feedback

