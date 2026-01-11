#include "../include/SynthParams.h"
#include "../include/Oscillator.h"
#include "../include/Envelope.h"
#include "../include/SynthEngine.h"
#include "../include/WavExporter.h"
#include "../include/Presets.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <string>

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
    
    std::cout << "\n✅ All tests passed!" << std::endl;
    std::cout << "\nGenerated test files:" << std::endl;
    std::cout << "  - test_output.wav" << std::endl;
    std::cout << "  - test_laser.wav" << std::endl;
    
    return 0;
}
