#pragma once

#include <string>
#include "model/SynthModel.h"

enum class UserActionType {
    None,
    
    // Synth parameter changes
    WaveformChanged,
    DutyCycleChanged,
    StartFreqChanged,
    EndFreqChanged,
    SlideSpeedChanged,
    AttackChanged,
    DecayChanged,
    SustainChanged,
    ReleaseChanged,
    DurationChanged,
    VibratoFreqChanged,
    VibratoDepthChanged,
    
    // Effects
    DistortionChanged,
    FilterChanged,
    ReverbChanged,
    
    // Sound control
    PlaySound,
    StopSound,
    Randomize,
    Export,
    
    // Preset selection
    PresetSelected,
    
    // Sequencer
    SequencerPlay,
    SequencerStop,
    SequencerStepToggled,
    SequencerStepSelected,
    SequencerBPMChanged,
    SequencerSlotSelected,
    SequencerSlotNameChanged,
    SequencerClear,
    
    // Song management
    SaveSong,
    LoadSong,
    NewSong,
};

struct UserAction {
    UserActionType type = UserActionType::None;
    
    // Generic float parameter
    float floatValue = 0.0f;
    
    // Generic int parameter
    int intValue = 0;
    
    // Generic string parameter
    std::string stringValue;
    
    // For parameter-based actions, which param was changed
    Waveform waveformValue = Waveform::Square;
    
    // Filename for export/save/load
    std::string filepath;
};
