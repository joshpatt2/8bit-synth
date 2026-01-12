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
