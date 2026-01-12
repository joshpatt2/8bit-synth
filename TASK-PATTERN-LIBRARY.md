# TASK: Pattern Library (JSON Import/Export)

**To:** Brent
**From:** J
**Date:** January 11, 2026
**Subject:** Save Your Work

---

## Brent.

Users are making sounds. They're tweaking parameters. They're building patterns in the sequencer.

Then they close the app and it's all gone.

**Fix that.**

---

## The Task

Build a pattern library system with JSON import/export.

Users should be able to:
1. Save their current synth settings + sequencer pattern to a file
2. Load a previously saved pattern
3. See a list of saved patterns
4. Delete patterns they don't want

---

## What Gets Saved

A "pattern" file should include:

### Synth Parameters
```json
{
  "synth": {
    "waveform": "square",
    "dutyCycle": 0.5,
    "startFreq": 440.0,
    "endFreq": 220.0,
    "slideSpeed": 0.5,
    "attack": 0.01,
    "decay": 0.1,
    "sustain": 0.7,
    "release": 0.3,
    "duration": 0.5,
    "effects": {
      "reverbTime": 0.5,
      "reverbMix": 0.3
    }
  }
}
```

### Sequencer State
```json
{
  "sequencer": {
    "bpm": 120,
    "numSteps": 16,
    "loop": true,
    "steps": [
      { "active": true, "soundSlot": 0 },
      { "active": false, "soundSlot": 0 },
      // ... 16 steps
    ],
    "slots": [
      { "name": "Kick", "enabled": true, "params": { /* SynthParams */ } },
      { "name": "Snare", "enabled": true, "params": { /* SynthParams */ } },
      // ... 4 slots
    ]
  }
}
```

### Metadata
```json
{
  "meta": {
    "name": "My Cool Pattern",
    "version": "1.0",
    "createdAt": "2026-01-11T15:30:00Z",
    "author": "Brent"
  }
}
```

---

## File Format

- Extension: `.8bp` (8-bit pattern) or `.json` — your call
- Location: `~/Documents/8bit-synth/patterns/` by default
- One file per pattern

---

## UI Requirements

**IMPORTANT:** Follow the [UI-STYLE-GUIDE.md](UI-STYLE-GUIDE.md) for all visual implementation.

### Location

Pattern management UI lives in the **Song Composer section** (bottom-right panel). This is thematically appropriate — song composition involves managing patterns — and the section is currently underutilized.

### Layout

```
┌─────────────────────────────────────────────────────────────┐
│  SONG COMPOSER                                              │
│                                                             │
│  Song Name: [Untitled Song________________]                 │
│                                                             │
│  ─────────────────────────────────────────────────────────  │
│                                                             │
│  PATTERNS                                                   │
│                                                             │
│  [💾 Save Pattern] [📂 Load Pattern]                        │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  BrentSignature.8bp              Jan 11    [🗑]     │   │
│  │  boss_battle.8bp                 Jan 10    [🗑]     │   │
│  │  game_over.8bp                   Jan 09    [🗑]     │   │
│  │  breakbeat.8bp                   Jan 08    [🗑]     │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  Current: boss_battle.8bp *                                 │
└─────────────────────────────────────────────────────────────┘
```

### Components

#### Save Pattern Button
```cpp
// Uses StyledButton from SynthTheme.h
if (StyledButton("💾 Save Pattern", ImVec2(130, 32))) {
    UserAction action;
    action.type = UserActionType::SavePattern;
    actions.push_back(action);
}
```
- Size: 130×32px
- Style: Accent background (matches Play/Random/Export)
- Triggers save modal

#### Load Pattern Button
```cpp
if (StyledButton("📂 Load Pattern", ImVec2(130, 32))) {
    UserAction action;
    action.type = UserActionType::LoadPattern;
    actions.push_back(action);
}
```
- Size: 130×32px
- Style: Accent background
- Opens pattern browser (if not already visible)

#### Pattern List
```
┌────────────────────────────────────────────────────────┐
│  [Selected] boss_battle.8bp         Jan 10    [🗑]    │ ← Selected row
│  breakbeat.8bp                      Jan 09    [🗑]    │ ← Normal row
│  game_over.8bp                      Jan 08    [🗑]    │
└────────────────────────────────────────────────────────┘
```

Implementation:
```cpp
ImGui::BeginChild("PatternList", ImVec2(-1, 150), true);

for (const auto& pattern : patterns) {
    bool isSelected = (pattern.name == currentPattern);
    
    if (isSelected) {
        ImGui::PushStyleColor(ImGuiCol_Header, SynthTheme::ToVec4(SynthTheme::Accent));
    }
    
    if (ImGui::Selectable(pattern.name.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)) {
        // Load this pattern
        UserAction action;
        action.type = UserActionType::LoadPattern;
        action.stringValue = pattern.path;
        actions.push_back(action);
    }
    
    if (isSelected) {
        ImGui::PopStyleColor();
    }
    
    // Date column
    ImGui::SameLine(200);
    ImGui::TextColored(ImVec4(0.545f, 0.545f, 0.545f, 1.0f), "%s", pattern.dateShort.c_str());
    
    // Delete button
    ImGui::SameLine(280);
    ImGui::PushID(pattern.name.c_str());
    if (ImGui::SmallButton("🗑")) {
        UserAction action;
        action.type = UserActionType::DeletePattern;
        action.stringValue = pattern.path;
        actions.push_back(action);
    }
    ImGui::PopID();
}

ImGui::EndChild();
```

Colors:
- List background: Surface (`#16213e`)
- Selected row: Accent (`#e94560`)
- Filename text: Text (`#eaeaea`)
- Date text: Text Dim (`#8b8b8b`)
- Delete button: Primary background, Accent on hover

#### Save Modal (Overlay)

When user clicks "Save Pattern", show an overlay modal:

```
┌──────────────────────────────────────────┐
│  SAVE PATTERN                            │
│                                          │
│  Pattern Name:                           │
│  [________________________________]      │
│                                          │
│  [Cancel]              [💾 Save]         │
└──────────────────────────────────────────┘
```

Implementation:
```cpp
// Modal state
static bool showSaveModal = false;
static char patternNameBuffer[256] = "";

// Trigger modal
if (saveButtonPressed) {
    showSaveModal = true;
    memset(patternNameBuffer, 0, sizeof(patternNameBuffer));
}

// Render modal
if (showSaveModal) {
    // Dim background
    ImDrawList* draw = ImGui::GetForegroundDrawList();
    draw->AddRectFilled(ImVec2(0, 0), io.DisplaySize, IM_COL32(0, 0, 0, 150));
    
    // Center modal
    ImVec2 modalSize(350, 150);
    ImVec2 modalPos((io.DisplaySize.x - modalSize.x) * 0.5f,
                    (io.DisplaySize.y - modalSize.y) * 0.5f);
    
    ImGui::SetNextWindowPos(modalPos);
    ImGui::SetNextWindowSize(modalSize);
    ImGui::Begin("##SaveModal", nullptr, 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove);
    
    SectionHeader("SAVE PATTERN");
    
    ImGui::Text("Pattern Name:");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##PatternName", patternNameBuffer, sizeof(patternNameBuffer));
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Cancel button (secondary style)
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.059f, 0.204f, 0.376f, 1.00f));
    if (ImGui::Button("Cancel", ImVec2(100, 32))) {
        showSaveModal = false;
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine(modalSize.x - 116);  // Right-align save button
    
    if (StyledButton("💾 Save", ImVec2(100, 32))) {
        if (strlen(patternNameBuffer) > 0) {
            UserAction action;
            action.type = UserActionType::SavePattern;
            action.stringValue = patternNameBuffer;
            actions.push_back(action);
            showSaveModal = false;
        }
    }
    
    ImGui::End();
}
```

Modal colors:
- Background overlay: Black at 60% opacity
- Modal window: Background (`#1a1a2e`)
- Border: Primary (`#0f3460`)

#### Delete Confirmation

When user clicks delete (🗑), show inline confirmation:

```
┌────────────────────────────────────────────────────────┐
│  Delete "boss_battle.8bp"?    [Yes] [No]              │
└────────────────────────────────────────────────────────┘
```

Or use a similar modal pattern. Keep it simple — don't over-engineer.

#### Status/Feedback

Show current pattern and modified state:

```cpp
ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.0f));
ImGui::Text("Current:");
ImGui::PopStyleColor();
ImGui::SameLine();
ImGui::Text("%s", currentPatternName.c_str());
if (isModified) {
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.914f, 0.271f, 0.376f, 1.0f), "*");
}
```

#### Error States

Errors display as temporary toast-style notifications at the bottom of the pattern list:

```cpp
// Error state
static std::string errorMessage;
static float errorTimer = 0.0f;

// Show error
if (!errorMessage.empty()) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.914f, 0.271f, 0.376f, 1.0f));
    ImGui::Text("⚠ %s", errorMessage.c_str());
    ImGui::PopStyleColor();
    
    errorTimer -= ImGui::GetIO().DeltaTime;
    if (errorTimer <= 0) {
        errorMessage.clear();
    }
}

// Set error (from controller)
void showError(const std::string& msg) {
    errorMessage = msg;
    errorTimer = 3.0f;  // Show for 3 seconds
}
```

Error messages:
- "Could not save pattern" — file write failed
- "Could not load pattern" — file read or parse failed
- "Pattern not found" — file was deleted externally
- "Invalid pattern file" — JSON parse error

### User Actions

Add these to `UserAction.h`:

```cpp
enum class UserActionType {
    // ... existing actions ...
    
    // Pattern management
    SavePattern,      // stringValue = pattern name (for new) or empty (overwrite current)
    LoadPattern,      // stringValue = pattern path
    DeletePattern,    // stringValue = pattern path
    NewPattern,       // Reset to defaults
};
```

### Empty State

When no patterns exist:

```
┌─────────────────────────────────────────────────────┐
│                                                     │
│        No saved patterns yet.                       │
│        Click "Save Pattern" to create one.          │
│                                                     │
└─────────────────────────────────────────────────────┘
```

Text color: Text Dim, centered.

### Keyboard Shortcuts (Nice to Have)

- `Cmd+S` / `Ctrl+S` — Save pattern (opens modal)
- `Cmd+O` / `Ctrl+O` — Focus pattern list
- `Enter` in pattern list — Load selected
- `Delete` / `Backspace` in pattern list — Delete selected (with confirmation)
- `Escape` — Close modal

---

## UI Requirements (Original)

### Save Dialog
- Button in UI: "💾 Save Pattern"
- Prompt for pattern name
- Save to patterns directory
- Show confirmation: "Saved: MyPattern.8bp"

### Load Dialog
- Button in UI: "📂 Load Pattern"
- Show list of available patterns (from patterns directory)
- Click to load
- Replace current synth + sequencer state

### Pattern Browser (Optional but Nice)
- List view showing saved patterns
- Pattern name, date created
- Delete button per pattern
- Double-click to load

---

## Architecture

Keep it clean. You know MVC now.

### Model Layer
```cpp
// include/model/PatternFile.h
struct PatternFile {
    std::string name;
    std::string version;
    std::string createdAt;
    std::string author;

    SynthParams synthParams;
    SequencerState sequencerState;
};
```

### Serialization Layer
```cpp
// include/PatternSerializer.h
class PatternSerializer {
public:
    static std::string toJson(const PatternFile& pattern);
    static PatternFile fromJson(const std::string& json);

    static bool saveToFile(const PatternFile& pattern, const std::string& path);
    static std::optional<PatternFile> loadFromFile(const std::string& path);

    static std::vector<std::string> listPatterns(const std::string& directory);
};
```

### Controller Integration
```cpp
// In SynthController or new PatternController
void savePattern(const std::string& name);
void loadPattern(const std::string& path);
std::vector<std::string> getAvailablePatterns();
```

### View Layer
- Save/Load buttons emit `UserAction::SavePattern` / `UserAction::LoadPattern`
- Pattern browser is a new view component or modal

---

## JSON Library

Use [nlohmann/json](https://github.com/nlohmann/json). It's header-only, well-documented, and standard.

```cpp
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// Serialize
json j;
j["synth"]["waveform"] = "square";
j["synth"]["attack"] = params.attack;
std::string output = j.dump(2);  // Pretty print

// Deserialize
json j = json::parse(jsonString);
params.attack = j["synth"]["attack"];
```

Add it to `external/` or use CMake FetchContent.

---

## Schema Versioning

Include a version field. Future you will thank present you.

```json
{
  "meta": {
    "version": "1.0"
  }
}
```

When loading:
- If version matches: load normally
- If version is older: migrate or warn
- If version is newer: warn "created with newer version"

Don't over-engineer this now. Just include the field so you can handle it later.

---

## Error Handling

- File doesn't exist: Show error, don't crash
- Invalid JSON: Show error, don't crash
- Missing fields: Use defaults, warn in console
- Permission denied: Show error, suggest different location

---

## Deliverables

1. **PatternFile struct** — Data model for saved patterns
2. **PatternSerializer** — JSON serialization/deserialization
3. **File I/O** — Save to disk, load from disk, list directory
4. **UI integration** — Save/Load buttons in SynthView or new PatternView
5. **Default patterns directory** — Created on first save if doesn't exist
6. **At least 2 example patterns** — Ship with the app so users see how it works

---

## Acceptance Criteria

- [ ] User can save current synth + sequencer state to a named file
- [ ] User can load a saved pattern and see synth + sequencer update
- [ ] User can see list of saved patterns
- [ ] Patterns persist after app restart
- [ ] Invalid files don't crash the app
- [ ] Version field exists in saved files

---

## Timeline

**1 week.**

This is plumbing, not rocket science. JSON serialization, file I/O, UI buttons. You've done harder things.

---

## Why This Matters

Right now, every session starts from zero. Users can't:
- Build a library of sounds
- Share patterns with teammates
- Version control their audio assets
- Come back tomorrow and continue where they left off

This feature turns the synth from a toy into a tool.

---

## One More Thing

Your roadmap said this enables the future pattern marketplace. You're right. But don't build the marketplace now. Build the file format. Build the save/load. Ship it.

The marketplace comes later. The foundation comes now.

---

— J

---

P.S. — When you're done, save a pattern called "BrentSignature.8bp" with your favorite sound. Ship it as a default pattern. Put your name on it.
