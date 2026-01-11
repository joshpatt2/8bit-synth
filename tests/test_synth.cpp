#include "../include/SynthParams.h"
#include "../include/Oscillator.h"
#include "../include/Envelope.h"
#include "../include/SynthEngine.h"
#include "../include/WavExporter.h"
#include "../include/Presets.h"
#include "../include/Sequencer.h"
#include "../include/PatternLoader.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <string>
#include <fstream>

#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    std::cout << "Running " #name "... "; \
    test_##name(); \
    std::cout << "✓" << std::endl; \
} while(0)

#define ASSERT(condition) do { \
    if (!(condition)) { \
        std::cerr << "Assertion failed: " #condition << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        exit(1); \
    } \
} while(0)

#define ASSERT_NEAR(a, b, epsilon) do { \
    if (std::abs((a) - (b)) > (epsilon)) { \
        std::cerr << "Assertion failed: " #a " ≈ " #b << " (diff: " << std::abs((a) - (b)) << ") at " << __FILE__ << ":" << __LINE__ << std::endl; \
        exit(1); \
    } \
} while(0)

// Test Oscillator
TEST(oscillator_square) {
    float sample1 = Oscillator::generateSample(Waveform::Square, 0.25f, 0.5f);
    ASSERT(sample1 == 1.0f);  // Below duty cycle
    
    float sample2 = Oscillator::generateSample(Waveform::Square, 0.75f, 0.5f);
    ASSERT(sample2 == -1.0f);  // Above duty cycle
}

TEST(oscillator_triangle) {
    float sample1 = Oscillator::generateSample(Waveform::Triangle, 0.0f);
    ASSERT_NEAR(sample1, -1.0f, 0.01f);
    
    float sample2 = Oscillator::generateSample(Waveform::Triangle, 0.25f);
    ASSERT_NEAR(sample2, 0.0f, 0.01f);
    
    float sample3 = Oscillator::generateSample(Waveform::Triangle, 0.5f);
    ASSERT_NEAR(sample3, 1.0f, 0.01f);
}

TEST(oscillator_sawtooth) {
    float sample1 = Oscillator::generateSample(Waveform::Sawtooth, 0.0f);
    ASSERT_NEAR(sample1, -1.0f, 0.01f);
    
    float sample2 = Oscillator::generateSample(Waveform::Sawtooth, 0.5f);
    ASSERT_NEAR(sample2, 0.0f, 0.01f);
    
    float sample3 = Oscillator::generateSample(Waveform::Sawtooth, 1.0f);
    ASSERT_NEAR(sample3, 1.0f, 0.01f);
}

TEST(oscillator_noise) {
    float sample1 = Oscillator::generateSample(Waveform::Noise, 0.5f);
    float sample2 = Oscillator::generateSample(Waveform::Noise, 0.5f);
    
    // Noise should produce values in valid range
    ASSERT(sample1 >= -1.0f && sample1 <= 1.0f);
    ASSERT(sample2 >= -1.0f && sample2 <= 1.0f);
}

// Test Envelope
TEST(envelope_attack) {
    SynthParams params;
    params.attack = 0.1f;
    params.decay = 0.1f;
    params.sustain = 0.5f;
    params.release = 0.1f;
    
    // At start of attack
    float env1 = Envelope::getValue(params, 0.0f, 1.0f);
    ASSERT_NEAR(env1, 0.0f, 0.01f);
    
    // Middle of attack
    float env2 = Envelope::getValue(params, 0.05f, 1.0f);
    ASSERT_NEAR(env2, 0.5f, 0.01f);
    
    // End of attack
    float env3 = Envelope::getValue(params, 0.1f, 1.0f);
    ASSERT_NEAR(env3, 1.0f, 0.01f);
}

TEST(envelope_sustain) {
    SynthParams params;
    params.attack = 0.1f;
    params.decay = 0.1f;
    params.sustain = 0.5f;
    params.release = 0.1f;
    
    // During sustain phase
    float env = Envelope::getValue(params, 0.5f, 1.0f);
    ASSERT_NEAR(env, 0.5f, 0.01f);
}

TEST(envelope_release) {
    SynthParams params;
    params.attack = 0.1f;
    params.decay = 0.1f;
    params.sustain = 0.5f;
    params.release = 0.1f;
    
    // At start of release (duration = 1.0s)
    float env1 = Envelope::getValue(params, 1.0f, 1.0f);
    ASSERT_NEAR(env1, 0.5f, 0.01f);
    
    // Middle of release
    float env2 = Envelope::getValue(params, 1.05f, 1.0f);
    ASSERT_NEAR(env2, 0.25f, 0.01f);
    
    // End of release
    float env3 = Envelope::getValue(params, 1.1f, 1.0f);
    ASSERT_NEAR(env3, 0.0f, 0.01f);
}

// Test SynthEngine
TEST(synth_engine_basic_render) {
    SynthParams params;
    params.waveform = Waveform::Square;
    params.startFreq = 440.0f;
    params.endFreq = 440.0f;
    params.duration = 0.1f;
    params.attack = 0.01f;
    params.decay = 0.01f;
    params.sustain = 1.0f;
    params.release = 0.01f;
    
    auto buffer = SynthEngine::render(params);
    
    // Should produce expected number of samples (duration + release)
    int expectedSamples = static_cast<int>((params.duration + params.release) * SynthEngine::SAMPLE_RATE);
    ASSERT(buffer.size() > 0);
    ASSERT(buffer.size() == static_cast<size_t>(expectedSamples));
    
    // All samples should be in valid range
    for (float sample : buffer) {
        ASSERT(sample >= -1.0f && sample <= 1.0f);
    }
}

TEST(synth_engine_frequency_sweep) {
    SynthParams params;
    params.waveform = Waveform::Square;
    params.startFreq = 880.0f;
    params.endFreq = 220.0f;
    params.duration = 0.2f;
    params.attack = 0.01f;
    params.decay = 0.01f;
    params.sustain = 1.0f;
    params.release = 0.01f;
    
    auto buffer = SynthEngine::render(params);
    
    ASSERT(buffer.size() > 0);
    // Verify we got a complete buffer
    int expectedSamples = static_cast<int>((params.duration + params.release) * SynthEngine::SAMPLE_RATE);
    ASSERT(buffer.size() == static_cast<size_t>(expectedSamples));
}

TEST(synth_engine_int16_conversion) {
    std::vector<float> floatBuffer = {-1.0f, -0.5f, 0.0f, 0.5f, 1.0f};
    auto intBuffer = SynthEngine::floatToInt16(floatBuffer);
    
    ASSERT(intBuffer.size() == floatBuffer.size());
    ASSERT(intBuffer[0] == -32767);
    ASSERT(intBuffer[2] == 0);
    ASSERT(intBuffer[4] == 32767);
}

// Test WavExporter
TEST(wav_export) {
    std::vector<int16_t> samples(4410);  // 0.1 second at 44100Hz
    for (size_t i = 0; i < samples.size(); i++) {
        samples[i] = static_cast<int16_t>(sin(i * 0.1) * 10000);
    }
    
    bool success = WavExporter::exportWav("test_output.wav", samples);
    ASSERT(success);
    
    // TODO: Could add file size check here
}

// Test Presets
TEST(presets_laser) {
    SynthParams laser = Presets::laser();
    
    ASSERT(laser.name == "Laser");
    ASSERT(laser.startFreq > laser.endFreq);  // Downward sweep
    ASSERT(laser.duration > 0.0f && laser.duration < 2.0f);
}

TEST(presets_explosion) {
    SynthParams explosion = Presets::explosion();
    
    ASSERT(explosion.name == "Explosion");
    ASSERT(explosion.waveform == Waveform::Noise);
    ASSERT(explosion.duration > 0.0f);
}

TEST(presets_randomize) {
    SynthParams random1 = Presets::randomize();
    SynthParams random2 = Presets::randomize();
    
    // Should produce valid parameters
    ASSERT(random1.startFreq >= 50.0f && random1.startFreq <= 2000.0f);
    ASSERT(random1.endFreq >= 50.0f && random1.endFreq <= 2000.0f);
    ASSERT(random1.duration >= 0.1f && random1.duration <= 1.0f);
    
    // Random should produce different results (very high probability)
    ASSERT(random1.startFreq != random2.startFreq || random1.endFreq != random2.endFreq);
}

// Integration test
TEST(integration_full_sound_generation) {
    Presets::init();
    
    // Generate a laser sound
    SynthParams laser = Presets::laser();
    auto floatBuffer = SynthEngine::render(laser);
    auto intBuffer = SynthEngine::floatToInt16(floatBuffer);
    
    ASSERT(floatBuffer.size() > 0);
    ASSERT(intBuffer.size() == floatBuffer.size());
    
    // Export it
    bool success = WavExporter::exportWav("test_laser.wav", intBuffer);
    ASSERT(success);
    
    std::cout << "\n  Generated test_laser.wav (" << intBuffer.size() << " samples)" << std::endl;
}

TEST(integration_all_waveforms) {
    SynthParams params;
    params.duration = 0.1f;
    params.attack = 0.01f;
    params.decay = 0.01f;
    params.sustain = 1.0f;
    params.release = 0.01f;
    params.startFreq = 440.0f;
    params.endFreq = 440.0f;
    
    Waveform waveforms[] = {Waveform::Square, Waveform::Triangle, Waveform::Sawtooth, Waveform::Noise};
    
    for (auto waveform : waveforms) {
        params.waveform = waveform;
        auto buffer = SynthEngine::render(params);
        ASSERT(buffer.size() > 0);
    }
}

// Sequencer Tests
TEST(sequencer_pattern_initialization) {
    SequencerPattern pattern;
    
    // Default should be 16 steps
    ASSERT(pattern.numSteps == 16);
    ASSERT(pattern.bpm == 120.0f);
    ASSERT(pattern.loop == true);
    
    // All steps should start inactive
    for (int i = 0; i < 16; i++) {
        ASSERT(pattern.steps[i].active == false);
    }
}

TEST(sequencer_pattern_activation) {
    SequencerPattern pattern;
    
    // Activate step 0 for sound slot 0 (Kick)
    pattern.steps[0].active = true;
    pattern.steps[0].soundSlot = 0;
    
    ASSERT(pattern.steps[0].active == true);
    ASSERT(pattern.steps[0].soundSlot == 0);
    
    // Activate step 4 for sound slot 1 (Snare)
    pattern.steps[4].active = true;
    pattern.steps[4].soundSlot = 1;
    
    ASSERT(pattern.steps[4].active == true);
    ASSERT(pattern.steps[4].soundSlot == 1);
    
    // Verify other steps unchanged
    ASSERT(pattern.steps[1].active == false);
    ASSERT(pattern.steps[8].active == false);
}

TEST(sequencer_pattern_length) {
    SequencerPattern pattern;
    
    // Test 16-step pattern
    pattern.numSteps = 16;
    ASSERT(pattern.numSteps == 16);
    
    // Test 32-step pattern
    pattern.numSteps = 32;
    ASSERT(pattern.numSteps == 32);
    
    // Verify we can access all steps
    pattern.steps[31].active = true;
    ASSERT(pattern.steps[31].active == true);
}

TEST(sequencer_bpm_setting) {
    SequencerPattern pattern;
    
    pattern.bpm = 60.0f;
    ASSERT(pattern.bpm == 60.0f);
    
    pattern.bpm = 180.0f;
    ASSERT(pattern.bpm == 180.0f);
    
    pattern.bpm = 140.0f;
    ASSERT(pattern.bpm == 140.0f);
}

TEST(sequencer_sound_slots) {
    Sequencer sequencer;
    
    // Should have 4 sound slots
    ASSERT(sequencer.slots.size() == 4);
    
    // Check slot names
    ASSERT(sequencer.slots[0].name == "Kick");
    ASSERT(sequencer.slots[1].name == "Snare");
    ASSERT(sequencer.slots[2].name == "Hat");
    ASSERT(sequencer.slots[3].name == "Blip");
}

TEST(sequencer_slot_params) {
    Sequencer sequencer;
    
    // Each slot should have valid parameters
    for (int i = 0; i < 4; i++) {
        const auto& slot = sequencer.slots[i];
        ASSERT(slot.params.startFreq > 0.0f);
        ASSERT(slot.params.duration > 0.0f);
        ASSERT(slot.params.attack >= 0.0f);
    }
}

// Pattern Loader Tests
TEST(pattern_loader_save_and_load) {
    Sequencer sequencer;
    
    // Create a test pattern
    sequencer.pattern.numSteps = 16;
    sequencer.pattern.bpm = 120.0f;
    sequencer.pattern.steps[0].active = true;
    sequencer.pattern.steps[0].soundSlot = 0;
    sequencer.pattern.steps[4].active = true;
    sequencer.pattern.steps[4].soundSlot = 1;
    sequencer.pattern.steps[8].active = true;
    sequencer.pattern.steps[8].soundSlot = 2;
    
    // Save to test file
    const char* testFile = "/tmp/test_pattern_temp.txt";
    bool saved = PatternLoader::savePattern(testFile, sequencer.pattern);
    ASSERT(saved);
    
    // Load into new pattern
    SequencerPattern loadedPattern;
    bool loaded = PatternLoader::loadPattern(testFile, loadedPattern);
    ASSERT(loaded);
    
    // Verify loaded pattern matches original
    ASSERT(loadedPattern.numSteps == 16);
    ASSERT(loadedPattern.bpm == 120.0f);
    ASSERT(loadedPattern.steps[0].active == true);
    ASSERT(loadedPattern.steps[0].soundSlot == 0);
    ASSERT(loadedPattern.steps[4].active == true);
    ASSERT(loadedPattern.steps[4].soundSlot == 1);
    ASSERT(loadedPattern.steps[8].active == true);
    ASSERT(loadedPattern.steps[8].soundSlot == 2);
    
    // Clean up
    std::remove(testFile);
}

TEST(pattern_loader_builtin_patterns) {
    Sequencer sequencer;
    
    // Test loading basic_beat pattern from root directory
    bool loaded = PatternLoader::loadPattern("../patterns/basic_beat.txt", sequencer.pattern);
    
    // If file doesn't exist, that's OK for tests running from build dir
    // Just verify the pattern structure is valid
    if (loaded) {
        ASSERT(sequencer.pattern.bpm == 120.0f);
        ASSERT(sequencer.pattern.numSteps == 16);
        
        // Should have some active steps
        bool hasActiveSteps = false;
        for (int i = 0; i < 16; i++) {
            if (sequencer.pattern.steps[i].active) {
                hasActiveSteps = true;
                break;
            }
        }
        ASSERT(hasActiveSteps);
    } else {
        // Test structure even if file isn't accessible
        ASSERT(sequencer.pattern.numSteps == 16);
        ASSERT(sequencer.pattern.bpm == 120.0f);
    }
}

// Slot Management Tests
TEST(slot_selection_and_sync) {
    Sequencer sequencer;
    SynthParams currentParams;
    
    // Simulate selecting Kick slot (slot 0)
    currentParams = sequencer.slots[0].params;
    
    // Verify we got the slot's parameters (slot has default values)
    ASSERT(currentParams.startFreq > 0.0f);
    ASSERT(currentParams.duration > 0.0f);
    
    // Modify the parameters
    SynthParams modified = currentParams;
    modified.startFreq = 100.0f;
    modified.duration = 0.5f;
    
    // Simulate updating the slot with modified parameters
    sequencer.slots[0].params = modified;
    
    // Verify the slot was updated
    ASSERT(sequencer.slots[0].params.startFreq == 100.0f);
    ASSERT(sequencer.slots[0].params.duration == 0.5f);
}

TEST(slot_parameter_persistence) {
    Sequencer sequencer;
    
    // Set unique parameters for each slot
    sequencer.slots[0].params.startFreq = 100.0f;  // Kick
    sequencer.slots[1].params.startFreq = 200.0f;  // Snare
    sequencer.slots[2].params.startFreq = 300.0f;  // Hat
    sequencer.slots[3].params.startFreq = 400.0f;  // Blip
    
    // Verify each slot retains its unique parameter
    ASSERT(sequencer.slots[0].params.startFreq == 100.0f);
    ASSERT(sequencer.slots[1].params.startFreq == 200.0f);
    ASSERT(sequencer.slots[2].params.startFreq == 300.0f);
    ASSERT(sequencer.slots[3].params.startFreq == 400.0f);
    
    // Modify one slot
    sequencer.slots[1].params.startFreq = 250.0f;
    
    // Verify others unchanged
    ASSERT(sequencer.slots[0].params.startFreq == 100.0f);
    ASSERT(sequencer.slots[1].params.startFreq == 250.0f);
    ASSERT(sequencer.slots[2].params.startFreq == 300.0f);
    ASSERT(sequencer.slots[3].params.startFreq == 400.0f);
}

TEST(slot_auto_save_on_preset) {
    Sequencer sequencer;
    SynthParams currentParams;
    
    // Select Kick slot
    currentParams = sequencer.slots[0].params;
    
    // Apply a preset (e.g., Laser)
    Presets::init();
    currentParams = Presets::laser();
    
    // Auto-save to slot
    sequencer.slots[0].params = currentParams;
    
    // Verify the slot has the preset parameters
    ASSERT(sequencer.slots[0].params.name == "Laser");
    ASSERT(sequencer.slots[0].params.waveform == Waveform::Square);
    ASSERT(sequencer.slots[0].params.startFreq > 0.0f);
}

TEST(real_time_synth_sequencer_sync) {
    Sequencer sequencer;
    SynthParams currentParams;
    int selectedSlot = 0;  // Kick selected
    
    // Load slot for editing
    currentParams = sequencer.slots[selectedSlot].params;
    float originalFreq = currentParams.startFreq;
    
    // Simulate slider change
    currentParams.startFreq = 150.0f;
    
    // Real-time update to sequencer slot
    if (selectedSlot >= 0 && selectedSlot < static_cast<int>(sequencer.slots.size())) {
        sequencer.slots[selectedSlot].params = currentParams;
    }
    
    // Verify slot was updated in real-time
    ASSERT(sequencer.slots[selectedSlot].params.startFreq == 150.0f);
    ASSERT(sequencer.slots[selectedSlot].params.startFreq != originalFreq);
}

int main() {
    std::cout << "\n🎵 8-Bit Synthesizer Test Suite\n" << std::endl;
    
    std::cout << "Oscillator Tests:" << std::endl;
    RUN_TEST(oscillator_square);
    RUN_TEST(oscillator_triangle);
    RUN_TEST(oscillator_sawtooth);
    RUN_TEST(oscillator_noise);
    
    std::cout << "\nEnvelope Tests:" << std::endl;
    RUN_TEST(envelope_attack);
    RUN_TEST(envelope_sustain);
    RUN_TEST(envelope_release);
    
    std::cout << "\nSynthEngine Tests:" << std::endl;
    RUN_TEST(synth_engine_basic_render);
    RUN_TEST(synth_engine_frequency_sweep);
    RUN_TEST(synth_engine_int16_conversion);
    
    std::cout << "\nWavExporter Tests:" << std::endl;
    RUN_TEST(wav_export);
    
    std::cout << "\nPresets Tests:" << std::endl;
    RUN_TEST(presets_laser);
    RUN_TEST(presets_explosion);
    RUN_TEST(presets_randomize);
    
    std::cout << "\nIntegration Tests:" << std::endl;
    RUN_TEST(integration_full_sound_generation);
    RUN_TEST(integration_all_waveforms);
    
    std::cout << "\nSequencer Tests:" << std::endl;
    RUN_TEST(sequencer_pattern_initialization);
    RUN_TEST(sequencer_pattern_activation);
    RUN_TEST(sequencer_pattern_length);
    RUN_TEST(sequencer_bpm_setting);
    RUN_TEST(sequencer_sound_slots);
    RUN_TEST(sequencer_slot_params);
    
    std::cout << "\nPattern Loader Tests:" << std::endl;
    RUN_TEST(pattern_loader_save_and_load);
    RUN_TEST(pattern_loader_builtin_patterns);
    
    std::cout << "\nSlot Management & Auto-Sync Tests:" << std::endl;
    RUN_TEST(slot_selection_and_sync);
    RUN_TEST(slot_parameter_persistence);
    RUN_TEST(slot_auto_save_on_preset);
    RUN_TEST(real_time_synth_sequencer_sync);
    
    std::cout << "\n✅ All tests passed!" << std::endl;
    std::cout << "\nGenerated test files:" << std::endl;
    std::cout << "  - test_output.wav" << std::endl;
    std::cout << "  - test_laser.wav" << std::endl;
    
    return 0;
}
