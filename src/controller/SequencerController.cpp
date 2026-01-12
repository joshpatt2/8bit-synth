#include "controller/SequencerController.h"

SequencerController::SequencerController() {
    lastStepTime = std::chrono::steady_clock::now();
}

void SequencerController::handleAction(const UserAction& action) {
    switch (action.type) {
        case UserActionType::SequencerPlay:
            play();
            break;
        case UserActionType::SequencerStop:
            stop();
            break;
        case UserActionType::SequencerStepToggled:
            toggleStep(action.intValue, selectedSlotIndex);
            break;
        case UserActionType::SequencerStepSelected:
            selectStep(action.intValue);
            break;
        case UserActionType::SequencerBPMChanged:
            sequencerState.pattern.bpm = action.floatValue;
            break;
        case UserActionType::SequencerSlotSelected:
            selectSlot(action.intValue);
            break;
        case UserActionType::SequencerClear:
            clearPattern();
            break;
        case UserActionType::SequencerRandomizeAll:
            randomizePattern(RandomizeMode::All);
            break;
        case UserActionType::SequencerRandomizeSome:
            randomizePattern(RandomizeMode::Some, 30);
            break;
        case UserActionType::SequencerRandomizeRemove:
            randomizePattern(RandomizeMode::Remove, 50);
            break;
        case UserActionType::SequencerRandomizeShuffle:
            randomizePattern(RandomizeMode::Shuffle);
            break;
        case UserActionType::SequencerRandomizeDensity:
            randomizePattern(RandomizeMode::Density, action.intValue);
            break;
        case UserActionType::SequencerRandomizeSlots:
            randomizePattern(RandomizeMode::Slots);
            break;
        default:
            break;
    }
}

void SequencerController::update() {
    if (!sequencerState.isPlaying) return;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<float>(now - lastStepTime).count();
    float stepDuration = sequencerState.getStepDuration();
    
    if (elapsed >= stepDuration) {
        lastStepTime = now;
        
        // Advance step
        sequencerState.currentStep++;
        if (sequencerState.currentStep >= sequencerState.pattern.numSteps) {
            if (sequencerState.pattern.loop) {
                sequencerState.currentStep = 0;
            } else {
                sequencerState.isPlaying = false;
                sequencerState.currentStep = -1;
                return;
            }
        }
        
        triggerStep();
    }
}

void SequencerController::play() {
    sequencerState.isPlaying = true;
    sequencerState.currentStep = -1;
    lastStepTime = std::chrono::steady_clock::now();
}

void SequencerController::stop() {
    sequencerState.isPlaying = false;
    sequencerState.currentStep = -1;
}

void SequencerController::toggleStep(int stepIndex, int slotIndex) {
    if (stepIndex >= 0 && stepIndex < sequencerState.pattern.numSteps) {
        auto& step = sequencerState.pattern.steps[stepIndex];
        if (!step.active) {
            step.active = true;
            step.soundSlot = slotIndex;
        } else {
            step.active = false;
        }
    }
}

void SequencerController::clearPattern() {
    for (auto& step : sequencerState.pattern.steps) {
        step.active = false;
    }
}

void SequencerController::selectStep(int stepIndex) {
    if (stepIndex >= 0 && stepIndex < sequencerState.pattern.numSteps) {
        sequencerState.selectedStep = stepIndex;
    }
}

void SequencerController::selectSlot(int slotIndex) {
    if (slotIndex >= 0 && slotIndex < static_cast<int>(sequencerState.slots.size())) {
        selectedSlotIndex = slotIndex;
    }
}

void SequencerController::updateSlotParams(int slotIndex, const SynthParams& params) {
    if (slotIndex >= 0 && slotIndex < static_cast<int>(sequencerState.slots.size())) {
        sequencerState.slots[slotIndex].params = params;
    }
}

const SynthParams& SequencerController::getSelectedSlotParams() const {
    return sequencerState.slots[selectedSlotIndex].params;
}

SynthParams& SequencerController::getSelectedSlotParamsMutable() {
    return sequencerState.slots[selectedSlotIndex].params;
}

void SequencerController::triggerStep() {
    auto& step = sequencerState.pattern.steps[sequencerState.currentStep];
    if (step.active && step.soundSlot < static_cast<int>(sequencerState.slots.size())) {
        auto& slot = sequencerState.slots[step.soundSlot];
        if (slot.enabled) {
            auto floatBuffer = SynthEngine::render(slot.params);
            auto intBuffer = SynthEngine::floatToInt16(floatBuffer);
            audioOutput.play(intBuffer);
        }
    }
}

void SequencerController::randomizePattern(RandomizeMode mode, int density) {
    // Clamp density to 0-100
    if (density < 0) density = 0;
    if (density > 100) density = 100;
    
    auto& pattern = sequencerState.pattern;
    
    switch (mode) {
        case RandomizeMode::All: {
            // Randomly toggle every step (50% chance each)
            for (auto& step : pattern.steps) {
                step.active = (rand() % 2 == 0);
                if (step.active) {
                    step.soundSlot = rand() % sequencerState.slots.size();
                }
            }
            break;
        }
        
        case RandomizeMode::Some: {
            // Add notes to ~density% of empty steps
            for (auto& step : pattern.steps) {
                if (!step.active && (rand() % 100) < density) {
                    step.active = true;
                    step.soundSlot = rand() % sequencerState.slots.size();
                }
            }
            break;
        }
        
        case RandomizeMode::Remove: {
            // Remove notes from ~density% of active steps
            for (auto& step : pattern.steps) {
                if (step.active && (rand() % 100) < density) {
                    step.active = false;
                }
            }
            break;
        }
        
        case RandomizeMode::Shuffle: {
            // Shuffle positions of active steps
            std::vector<int> activeIndices;
            for (int i = 0; i < pattern.numSteps; i++) {
                if (pattern.steps[i].active) {
                    activeIndices.push_back(i);
                }
            }
            
            // Clear pattern
            for (int i = 0; i < pattern.numSteps; i++) {
                pattern.steps[i].active = false;
            }
            
            // Randomize active indices and re-place
            for (int i = 0; i < static_cast<int>(activeIndices.size()); i++) {
                int randomPos = rand() % pattern.numSteps;
                
                // Find next empty slot (avoid collisions)
                while (pattern.steps[randomPos].active && randomPos < pattern.numSteps) {
                    randomPos = (randomPos + 1) % pattern.numSteps;
                }
                
                pattern.steps[randomPos].active = true;
                pattern.steps[randomPos].soundSlot = rand() % sequencerState.slots.size();
            }
            break;
        }
        
        case RandomizeMode::Density: {
            // Increase density by adding notes (proportional to density parameter)
            int targetActive = (pattern.numSteps * density) / 100;
            int currentActive = 0;
            
            for (int i = 0; i < pattern.numSteps; i++) {
                if (pattern.steps[i].active) currentActive++;
            }
            
            int toAdd = targetActive - currentActive;
            int attempts = 0;
            
            while (toAdd > 0 && attempts < pattern.numSteps * 2) {
                int pos = rand() % pattern.numSteps;
                if (!pattern.steps[pos].active) {
                    pattern.steps[pos].active = true;
                    pattern.steps[pos].soundSlot = rand() % sequencerState.slots.size();
                    toAdd--;
                }
                attempts++;
            }
            break;
        }
        
        case RandomizeMode::Slots: {
            // Keep pattern structure, randomize which slots play
            for (auto& step : pattern.steps) {
                if (step.active) {
                    step.soundSlot = rand() % sequencerState.slots.size();
                }
            }
            break;
        }
    }
}
