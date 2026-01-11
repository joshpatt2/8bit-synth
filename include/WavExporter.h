#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <cstdint>

class WavExporter {
public:
    static bool exportWav(const std::string& filename, 
                         const std::vector<int16_t>& samples, 
                         int sampleRate = 44100) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // WAV header
        uint32_t dataSize = samples.size() * sizeof(int16_t);
        uint32_t fileSize = 36 + dataSize;
        
        // RIFF header
        file.write("RIFF", 4);
        file.write(reinterpret_cast<const char*>(&fileSize), 4);
        file.write("WAVE", 4);
        
        // fmt chunk
        file.write("fmt ", 4);
        uint32_t fmtSize = 16;
        uint16_t audioFormat = 1;  // PCM
        uint16_t numChannels = 1;  // Mono
        uint32_t sampleRateValue = sampleRate;
        uint32_t byteRate = sampleRate * numChannels * sizeof(int16_t);
        uint16_t blockAlign = numChannels * sizeof(int16_t);
        uint16_t bitsPerSample = 16;
        
        file.write(reinterpret_cast<const char*>(&fmtSize), 4);
        file.write(reinterpret_cast<const char*>(&audioFormat), 2);
        file.write(reinterpret_cast<const char*>(&numChannels), 2);
        file.write(reinterpret_cast<const char*>(&sampleRateValue), 4);
        file.write(reinterpret_cast<const char*>(&byteRate), 4);
        file.write(reinterpret_cast<const char*>(&blockAlign), 2);
        file.write(reinterpret_cast<const char*>(&bitsPerSample), 2);
        
        // data chunk
        file.write("data", 4);
        file.write(reinterpret_cast<const char*>(&dataSize), 4);
        file.write(reinterpret_cast<const char*>(samples.data()), dataSize);
        
        file.close();
        return true;
    }
};
