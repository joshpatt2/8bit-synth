#pragma once

// For backward compatibility, include from model
#include "model/SynthModel.h"

// Re-export for existing code that includes this
using ::SequencerStep;
using ::SequencerPattern;
using ::SoundSlot;
using ::SequencerState;

// Legacy alias for backward compatibility
class Sequencer : public SequencerState {
public:
    Sequencer() : SequencerState() {}
};

