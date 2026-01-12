#include "controller/SynthController.h"
#include "WavExporter.h"

SynthController::SynthController() {
    Presets::init();
    params = Presets::laser();
}

void SynthController::handleAction(const UserAction& action) {
    switch (action.type) {
        case UserActionType::PlaySound:
            playSound();
            break;
        case UserActionType::StopSound:
            stopSound();
            break;
        case UserActionType::Export:
            exportToWav(action.filepath);
            break;
        case UserActionType::PresetSelected:
            loadPreset(action.stringValue);
            break;
        case UserActionType::Randomize:
            randomizeParams();
            break;
        case UserActionType::WaveformChanged:
            params.waveform = action.waveformValue;
            break;
        case UserActionType::DutyCycleChanged:
            params.dutyCycle = action.floatValue;
            break;
        case UserActionType::StartFreqChanged:
            params.startFreq = action.floatValue;
            break;
        case UserActionType::EndFreqChanged:
            params.endFreq = action.floatValue;
            break;
        case UserActionType::SlideSpeedChanged:
            params.slideSpeed = action.floatValue;
            break;
        case UserActionType::AttackChanged:
            params.attack = action.floatValue;
            break;
        case UserActionType::DecayChanged:
            params.decay = action.floatValue;
            break;
        case UserActionType::SustainChanged:
            params.sustain = action.floatValue;
            break;
        case UserActionType::ReleaseChanged:
            params.release = action.floatValue;
            break;
        case UserActionType::DurationChanged:
            params.duration = action.floatValue;
            break;
        case UserActionType::VibratoFreqChanged:
            params.vibratoFreq = action.floatValue;
            break;
        case UserActionType::VibratoDepthChanged:
            params.vibratoDepth = action.floatValue;
            break;
        case UserActionType::DistortionChanged:
            params.effects.reverbMix = action.floatValue;
            break;
        case UserActionType::FilterChanged:
            params.effects.delayMix = action.floatValue;
            break;
        case UserActionType::ReverbChanged:
            params.effects.reverbTime = action.floatValue;
            break;
        default:
            break;
    }
}

void SynthController::playSound() {
    auto floatBuffer = SynthEngine::render(params);
    auto intBuffer = SynthEngine::floatToInt16(floatBuffer);
    audioOutput.play(intBuffer);
}

void SynthController::stopSound() {
    // Currently AudioOutput doesn't have explicit stop
    // This would be implemented if needed
}

std::string SynthController::exportToWav(const std::string& filepath) {
    auto floatBuffer = SynthEngine::render(params);
    auto intBuffer = SynthEngine::floatToInt16(floatBuffer);
    WavExporter::exportWav(filepath, intBuffer);
    return filepath;
}

std::vector<std::string> SynthController::getPresetNames() {
    return {
        "Laser",
        "Jump",
        "Hurt",
        "Powerup",
        "808 Kick",
        "808 Short"
    };
}

void SynthController::loadPreset(const std::string& name) {
    if (name == "Laser") {
        params = Presets::laser();
    } else if (name == "Jump") {
        params = Presets::jump();
    } else if (name == "Hurt") {
        params = Presets::hurt();
    } else if (name == "Powerup") {
        params = Presets::powerup();
    } else if (name == "808 Kick") {
        params = Presets::kick808();
    } else if (name == "808 Short") {
        params = Presets::kick808Short();
    }
}

void SynthController::randomizeParams() {
    params.waveform = static_cast<Waveform>(rand() % 4);
    params.dutyCycle = 0.2f + (rand() % 100) / 100.0f * 0.6f;
    params.startFreq = 50.0f + (rand() % 100) / 100.0f * 1950.0f;
    params.endFreq = 50.0f + (rand() % 100) / 100.0f * 1950.0f;
    params.slideSpeed = (rand() % 100) / 100.0f;
    params.attack = 0.001f + (rand() % 100) / 100.0f * 0.499f;
    params.decay = 0.01f + (rand() % 100) / 100.0f * 0.99f;
    params.sustain = (rand() % 100) / 100.0f;
    params.release = 0.01f + (rand() % 100) / 100.0f * 1.99f;
}
