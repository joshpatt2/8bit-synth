#pragma once

struct FxParams {
    // Reverb
    float reverbTime = 0.5f;    // 0-2 seconds (decay time)
    float reverbMix = 0.3f;      // 0-1.0 (dry/wet blend)
    bool reverbEnabled = true;
    
    // Delay
    float delayTime = 0.3f;      // 0-1 second
    float delayFeedback = 0.4f;  // 0-0.9 (prevents exploding)
    float delayMix = 0.2f;       // 0-1.0 (dry/wet blend)
    bool delayEnabled = true;
};
