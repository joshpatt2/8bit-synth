#include <SDL2/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "SynthParams.h"
#include "SynthEngine.h"
#include "AudioOutput.h"
#include "WavExporter.h"
#include "Presets.h"
#include "Sequencer.h"
#include "PatternLoader.h"
#include "Song.h"
#include "SongEngine.h"

#include <vector>
#include <string>
#include <chrono>

class SynthApp {
public:
    SynthApp() {
        Presets::init();
        currentParams = Presets::laser();
        
        // Initialize sequencer sounds
        setupSequencerSounds();
        
        lastStepTime = std::chrono::steady_clock::now();
        selectedSlot = -1;  // No slot selected by default
    }
    
    void renderUI() {
        ImGui::Begin("8-Bit Synthesizer", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        
        // Title
        ImGui::Text("🎵 Retro Sound Effect Generator");
        
        // Show which slot is being edited, if any
        if (selectedSlot >= 0 && selectedSlot < static_cast<int>(sequencer.slots.size())) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), " - Editing: %s", 
                             sequencer.slots[selectedSlot].name.c_str());
        }
        
        ImGui::Separator();
        
        // Waveform selection
        ImGui::Text("Waveform");
        const char* waveforms[] = { "Square", "Triangle", "Sawtooth", "Noise" };
        int currentWaveform = static_cast<int>(currentParams.waveform);
        if (ImGui::Combo("##waveform", &currentWaveform, waveforms, 4)) {
            currentParams.waveform = static_cast<Waveform>(currentWaveform);
            updateSelectedSlot();
        }
        
        // Duty cycle (for square wave)
        if (currentParams.waveform == Waveform::Square) {
            if (ImGui::SliderFloat("Duty Cycle", &currentParams.dutyCycle, 0.1f, 0.9f)) {
                updateSelectedSlot();
            }
        }
        
        ImGui::Separator();
        
        // Frequency controls
        ImGui::Text("Frequency");
        if (ImGui::SliderFloat("Start (Hz)", &currentParams.startFreq, 50.0f, 2000.0f)) {
            updateSelectedSlot();
        }
        if (ImGui::SliderFloat("End (Hz)", &currentParams.endFreq, 50.0f, 2000.0f)) {
            updateSelectedSlot();
        }
        if (ImGui::SliderFloat("Slide Speed", &currentParams.slideSpeed, 0.0f, 1.0f)) {
            updateSelectedSlot();
        }
        
        ImGui::Separator();
        
        // Envelope controls
        ImGui::Text("Envelope (ADSR)");
        if (ImGui::SliderFloat("Attack", &currentParams.attack, 0.001f, 0.5f, "%.3f s")) {
            updateSelectedSlot();
        }
        if (ImGui::SliderFloat("Decay", &currentParams.decay, 0.01f, 1.0f, "%.3f s")) {
            updateSelectedSlot();
        }
        if (ImGui::SliderFloat("Sustain", &currentParams.sustain, 0.0f, 1.0f)) {
            updateSelectedSlot();
        }
        if (ImGui::SliderFloat("Release", &currentParams.release, 0.01f, 2.0f, "%.3f s")) {
            updateSelectedSlot();
        }
        
        ImGui::Separator();
        
        // Duration
        if (ImGui::SliderFloat("Duration", &currentParams.duration, 0.05f, 2.0f, "%.2f s")) {
            updateSelectedSlot();
        }
        
        ImGui::Separator();
        
        // Vibrato (stretch feature)
        ImGui::Text("Vibrato");
        if (ImGui::SliderFloat("Vibrato Freq", &currentParams.vibratoFreq, 0.0f, 20.0f, "%.1f Hz")) {
            updateSelectedSlot();
        }
        if (currentParams.vibratoFreq > 0.0f) {
            if (ImGui::SliderFloat("Vibrato Depth", &currentParams.vibratoDepth, 0.0f, 0.2f)) {
                updateSelectedSlot();
            }
        }
        
        ImGui::Separator();
        
        // Effects
        ImGui::Text("Effects");
        
        // Delay
        if (ImGui::Checkbox("##delay_enable", &currentParams.effects.delayEnabled)) {
            updateSelectedSlot();
        }
        ImGui::SameLine();
        if (ImGui::SliderFloat("Delay Time", &currentParams.effects.delayTime, 0.0f, 1.0f, "%.2f s")) {
            updateSelectedSlot();
        }
        if (ImGui::SliderFloat("Delay Feedback", &currentParams.effects.delayFeedback, 0.0f, 0.9f, "%.2f")) {
            updateSelectedSlot();
        }
        if (ImGui::SliderFloat("Delay Mix", &currentParams.effects.delayMix, 0.0f, 1.0f, "%.2f")) {
            updateSelectedSlot();
        }
        
        ImGui::Separator();
        
        // Reverb
        if (ImGui::Checkbox("##reverb_enable", &currentParams.effects.reverbEnabled)) {
            updateSelectedSlot();
        }
        ImGui::SameLine();
        if (ImGui::SliderFloat("Reverb Time", &currentParams.effects.reverbTime, 0.0f, 2.0f, "%.2f s")) {
            updateSelectedSlot();
        }
        if (ImGui::SliderFloat("Reverb Mix", &currentParams.effects.reverbMix, 0.0f, 1.0f, "%.2f")) {
            updateSelectedSlot();
        }
        
        ImGui::Separator();
        
        // Action buttons
        ImGui::Text("Actions");
        
        if (ImGui::Button("▶ Play", ImVec2(100, 30))) {
            playSound();
        }
        
        ImGui::SameLine();
        if (ImGui::Button("■ Stop", ImVec2(100, 30))) {
            audioOutput.stop();
        }
        
        ImGui::SameLine();
        if (ImGui::Button("🎲 Randomize", ImVec2(120, 30))) {
            currentParams = Presets::randomize();
        }
        
        if (ImGui::Button("💾 Export WAV", ImVec2(120, 30))) {
            exportWav();
        }
        
        // Save to sequencer slot button
        if (selectedSlot >= 0) {
            ImGui::SameLine();
            if (ImGui::Button("💾 Save to Slot", ImVec2(120, 30))) {
                updateSelectedSlot();
                SDL_Log("Updated: %s", sequencer.slots[selectedSlot].name.c_str());
            }
        }
        
        ImGui::Separator();
        
        // Presets
        ImGui::Text("Presets");
        
        if (ImGui::Button("Laser", ImVec2(100, 0))) {
            currentParams = Presets::laser();
            if (selectedSlot >= 0) updateSelectedSlot();
        }
        ImGui::SameLine();
        if (ImGui::Button("Explosion", ImVec2(100, 0))) {
            currentParams = Presets::explosion();
            if (selectedSlot >= 0) updateSelectedSlot();
        }
        ImGui::SameLine();
        if (ImGui::Button("Pickup", ImVec2(100, 0))) {
            currentParams = Presets::pickup();
            if (selectedSlot >= 0) updateSelectedSlot();
        }
        
        if (ImGui::Button("Jump", ImVec2(100, 0))) {
            currentParams = Presets::jump();
            if (selectedSlot >= 0) updateSelectedSlot();
        }
        ImGui::SameLine();
        if (ImGui::Button("Hurt", ImVec2(100, 0))) {
            currentParams = Presets::hurt();
            if (selectedSlot >= 0) updateSelectedSlot();
        }
        ImGui::SameLine();
        if (ImGui::Button("Powerup", ImVec2(100, 0))) {
            currentParams = Presets::powerup();
            if (selectedSlot >= 0) updateSelectedSlot();
        }

        // Drum presets
        if (ImGui::Button("808 Kick", ImVec2(100, 0))) {
            currentParams = Presets::kick808();
        }
        ImGui::SameLine();
        if (ImGui::Button("808 Short", ImVec2(100, 0))) {
            currentParams = Presets::kick808Short();
        }

        ImGui::Separator();
        
        // Status
        if (audioOutput.isPlaying()) {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "● Playing...");
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "○ Ready");
        }
        
        if (!lastExportPath.empty()) {
            ImGui::Text("Last export: %s", lastExportPath.c_str());
        }
        
        ImGui::End();
    }
    
    void playSound() {
        auto floatBuffer = SynthEngine::render(currentParams);
        auto intBuffer = SynthEngine::floatToInt16(floatBuffer);
        audioOutput.play(intBuffer);
    }
    
    void exportWav() {
        auto floatBuffer = SynthEngine::render(currentParams);
        auto intBuffer = SynthEngine::floatToInt16(floatBuffer);
        
        // Generate filename with timestamp
        time_t now = time(nullptr);
        char filename[256];
        snprintf(filename, sizeof(filename), "sfx_%ld.wav", now);
        
        if (WavExporter::exportWav(filename, intBuffer)) {
            lastExportPath = filename;
            SDL_Log("Exported: %s", filename);
        } else {
            SDL_Log("Export failed!");
        }
    }
    
    void setupSequencerSounds() {
        // Kick
        sequencer.slots[0].params = Presets::explosion();
        sequencer.slots[0].params.duration = 0.15f;
        sequencer.slots[0].params.startFreq = 120.0f;
        sequencer.slots[0].params.endFreq = 40.0f;
        sequencer.slots[0].name = "Kick";
        
        // Snare
        sequencer.slots[1].params = Presets::explosion();
        sequencer.slots[1].params.duration = 0.1f;
        sequencer.slots[1].params.startFreq = 400.0f;
        sequencer.slots[1].params.endFreq = 200.0f;
        sequencer.slots[1].name = "Snare";
        
        // Hi-hat
        sequencer.slots[2].params.waveform = Waveform::Noise;
        sequencer.slots[2].params.duration = 0.05f;
        sequencer.slots[2].params.attack = 0.001f;
        sequencer.slots[2].params.release = 0.02f;
        sequencer.slots[2].params.sustain = 0.3f;
        sequencer.slots[2].name = "Hat";
        
        // Blip
        sequencer.slots[3].params = Presets::pickup();
        sequencer.slots[3].params.duration = 0.08f;
        sequencer.slots[3].name = "Blip";
    }
    
    void updateSequencer() {
        if (!sequencer.isPlaying) return;
        
        auto now = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration<float>(now - lastStepTime).count();
        float stepDuration = sequencer.getStepDuration();
        
        if (elapsed >= stepDuration) {
            lastStepTime = now;
            
            // Advance step
            sequencer.currentStep++;
            if (sequencer.currentStep >= sequencer.pattern.numSteps) {
                if (sequencer.pattern.loop) {
                    sequencer.currentStep = 0;
                } else {
                    sequencer.isPlaying = false;
                    sequencer.currentStep = -1;
                    return;
                }
            }
            
            // Trigger sounds for this step
            auto& step = sequencer.pattern.steps[sequencer.currentStep];
            if (step.active && step.soundSlot < static_cast<int>(sequencer.slots.size())) {
                auto& slot = sequencer.slots[step.soundSlot];
                if (slot.enabled) {
                    auto floatBuffer = SynthEngine::render(slot.params);
                    auto intBuffer = SynthEngine::floatToInt16(floatBuffer);
                    audioOutput.play(intBuffer);
                }
            }
        }
    }
    
    void renderSequencerUI() {
        ImGui::Begin("Sequencer", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        
        ImGui::Text("🎹 Step Sequencer");
        ImGui::Separator();
        
        // Transport controls
        if (sequencer.isPlaying) {
            if (ImGui::Button("■ Stop", ImVec2(100, 30))) {
                sequencer.isPlaying = false;
                sequencer.currentStep = -1;
            }
        } else {
            if (ImGui::Button("▶ Play", ImVec2(100, 30))) {
                sequencer.isPlaying = true;
                sequencer.currentStep = -1;
                lastStepTime = std::chrono::steady_clock::now();
            }
        }
        
        ImGui::SameLine();
        ImGui::Checkbox("Loop", &sequencer.pattern.loop);
        
        // Tempo and pattern length
        ImGui::SliderFloat("BPM", &sequencer.pattern.bpm, 60.0f, 200.0f, "%.0f");
        
        const char* patternLengths[] = { "1 Bar (16 steps)", "2 Bars (32 steps)" };
        int currentLength = (sequencer.pattern.numSteps == 16) ? 0 : 1;
        if (ImGui::Combo("Pattern Length", &currentLength, patternLengths, 2)) {
            sequencer.pattern.numSteps = (currentLength == 0) ? 16 : 32;
        }
        
        ImGui::Separator();
        
        // Show selected step info
        if (sequencer.selectedStep >= 0 && sequencer.selectedStep < sequencer.pattern.numSteps) {
            ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.2f, 1.0f), "Selected Step: %d", sequencer.selectedStep);
            auto& step = sequencer.pattern.steps[sequencer.selectedStep];
            if (step.active) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.3f, 1.0f), "(Slot: %s)", sequencer.slots[step.soundSlot].name.c_str());
            }
        } else {
            ImGui::TextDisabled("No step selected");
        }
        
        ImGui::Separator();
        
        // Sound slot editor
        ImGui::Text("Sound Slots:");
        for (int i = 0; i < 4; i++) {
            bool isSelected = (selectedSlot == i);
            if (isSelected) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.8f, 1.0f));
            }
            
            if (ImGui::Button(sequencer.slots[i].name.c_str(), ImVec2(100, 0))) {
                selectSlot(i);
            }
            
            if (isSelected) {
                ImGui::PopStyleColor();
            }
            
            if (i < 3) ImGui::SameLine();
        }
        
        ImGui::Separator();
        
        // Step grid
        ImGui::Text("Step Grid:");
        
        for (int slot = 0; slot < 4; slot++) {
            ImGui::PushID(slot);
            
            ImGui::Text("%-6s", sequencer.slots[slot].name.c_str());
            ImGui::SameLine();
            
            for (int step = 0; step < sequencer.pattern.numSteps; step++) {
                if (step > 0 && step % 4 == 0) {
                    ImGui::SameLine();
                    ImGui::Text("|");
                }
                
                ImGui::SameLine();
                ImGui::PushID(step);
                
                auto& stepData = sequencer.pattern.steps[step];
                bool isActive = stepData.active && stepData.soundSlot == slot;
                bool isCurrent = (sequencer.currentStep == step && sequencer.isPlaying);
                bool isSelected = (sequencer.selectedStep == step);
                
                // Style the button based on state
                if (isCurrent) {
                    // Currently playing - bright green with outline
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.9f, 0.2f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
                } else if (isSelected) {
                    // Selected step - bright orange/gold
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.7f, 0.2f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.8f, 0.3f, 1.0f));
                } else if (isActive) {
                    // Active step - medium blue
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 0.9f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));
                } else {
                    // Inactive step - dark gray
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
                }
                
                const char* glyph = isActive ? "✓" : "·";
                if (isCurrent) glyph = "►";
                if (isSelected && !isCurrent) glyph = "◆";
                
                if (ImGui::SmallButton(glyph)) {
                    // Toggle active state
                    if (isActive) {
                        stepData.active = false;
                    } else {
                        stepData.active = true;
                        stepData.soundSlot = slot;
                    }
                    // Always set as selected when clicked
                    sequencer.selectedStep = step;
                }
                
                ImGui::PopStyleColor(2);
                
                ImGui::PopID();
            }
            
            ImGui::PopID();
            ImGui::NewLine();
        }
        
        ImGui::Separator();
        
        // Clear pattern button
        if (ImGui::Button("Clear Pattern")) {
            for (auto& step : sequencer.pattern.steps) {
                step.active = false;
            }
        }
        
        ImGui::Separator();
        ImGui::Text("Load Demo Pattern:");
        
        if (ImGui::Button("Basic Beat", ImVec2(120, 0))) {
            PatternLoader::loadPattern("patterns/basic_beat.txt", sequencer.pattern);
        }
        ImGui::SameLine();
        if (ImGui::Button("Breakbeat", ImVec2(120, 0))) {
            PatternLoader::loadPattern("patterns/breakbeat.txt", sequencer.pattern);
        }
        
        if (ImGui::Button("Game Over", ImVec2(120, 0))) {
            PatternLoader::loadPattern("patterns/game_over.txt", sequencer.pattern);
        }
        ImGui::SameLine();
        if (ImGui::Button("Boss Battle", ImVec2(120, 0))) {
            PatternLoader::loadPattern("patterns/boss_battle.txt", sequencer.pattern);
        }
        
        ImGui::Separator();
        
        if (ImGui::Button("Save Pattern...")) {
            time_t now = time(nullptr);
            char filename[256];
            snprintf(filename, sizeof(filename), "patterns/saved_%ld.txt", now);
            if (PatternLoader::savePattern(filename, sequencer.pattern)) {
                SDL_Log("Pattern saved: %s", filename);
            }
        }
        
        ImGui::End();
    }
    
    void renderSongUI() {
        ImGui::Begin("Song Composer", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        
        ImGui::Text("🎼 Song Mode");
        ImGui::Separator();
        
        // Song name input
        static char songName[256] = "Untitled Song";
        ImGui::InputText("Song Name", songName, sizeof(songName));
        song.name = std::string(songName);
        
        ImGui::Separator();
        
        // Pattern library
        ImGui::Text("Pattern Library:");
        if (ImGui::Button("+ Add Current Pattern", ImVec2(200, 0))) {
            SequencerPattern newPattern = sequencer.pattern;
            song.patterns.push_back(newPattern);
            SDL_Log("Pattern added to library (total: %zu)", song.patterns.size());
        }
        
        ImGui::Separator();
        
        // Current pattern display
        ImGui::Text("Patterns in Library: %zu", song.patterns.size());
        for (size_t i = 0; i < song.patterns.size(); i++) {
            ImGui::PushID(static_cast<int>(i));
            
            ImGui::Text("Pattern %zu: %d steps @ %.0f BPM", i, song.patterns[i].numSteps, song.patterns[i].bpm);
            ImGui::SameLine();
            
            if (ImGui::SmallButton("→ Add to Song")) {
                song.addPatternToArrangement(i);
                SDL_Log("Pattern %zu added to arrangement", i);
            }
            
            ImGui::PopID();
        }
        
        ImGui::Separator();
        
        // Song arrangement timeline
        ImGui::Text("Song Arrangement (%zu patterns)", song.arrangement.size());
        
        if (song.arrangement.empty()) {
            ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.3f, 1.0f), "Empty - add patterns to create song");
        } else {
            float totalDuration = song.getTotalDuration(120.0f);
            ImGui::Text("Total Duration: %.1f seconds", totalDuration);
            
            // Display arrangement
            for (size_t i = 0; i < song.arrangement.size(); i++) {
                ImGui::PushID(static_cast<int>(i));
                
                int patternIdx = song.arrangement[i];
                ImGui::Text("[%zu] Pattern %d (%d steps)", i, patternIdx, song.patterns[patternIdx].numSteps);
                
                ImGui::SameLine();
                if (ImGui::SmallButton("×")) {
                    song.removePatternFromArrangement(i);
                    SDL_Log("Pattern removed from arrangement at position %zu", i);
                }
                
                ImGui::PopID();
            }
        }
        
        ImGui::Separator();
        
        // Song export
        if (!song.arrangement.empty()) {
            if (ImGui::Button("🎵 Export Song as WAV", ImVec2(200, 30))) {
                exportSong();
            }
        } else {
            ImGui::TextDisabled("(Add patterns to enable export)");
        }
        
        ImGui::End();
    }
    
    void selectSlot(int slotIndex) {
        if (slotIndex >= 0 && slotIndex < static_cast<int>(sequencer.slots.size())) {
            selectedSlot = slotIndex;
            currentParams = sequencer.slots[slotIndex].params;
        }
    }
    
    void updateSelectedSlot() {
        if (selectedSlot >= 0 && selectedSlot < static_cast<int>(sequencer.slots.size())) {
            sequencer.slots[selectedSlot].params = currentParams;
        }
    }
    
    void exportSong() {
        // Render the entire song
        std::vector<SynthParams> soundSlots;
        for (const auto& slot : sequencer.slots) {
            soundSlots.push_back(slot.params);
        }
        
        auto songBuffer = SongEngine::renderSong(song, soundSlots);
        auto intBuffer = SynthEngine::floatToInt16(songBuffer);
        
        // Generate filename with timestamp
        time_t now = time(nullptr);
        char filename[256];
        snprintf(filename, sizeof(filename), "sfx_song_%ld.wav", now);
        
        if (WavExporter::exportWav(filename, intBuffer)) {
            SDL_Log("Song exported: %s (%zu samples)", filename, intBuffer.size());
            lastExportPath = filename;
        } else {
            SDL_Log("Failed to export song");
        }
    }
    
private:
    SynthParams currentParams;
    AudioOutput audioOutput;
    std::string lastExportPath;
    Sequencer sequencer;
    Song song;
    std::chrono::steady_clock::time_point lastStepTime;
    int selectedSlot;
};

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }
    
    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "8-Bit Synthesizer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        700,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );
    
    if (!window) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    // Setup SDL Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Setup style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    
    // Create app
    SynthApp app;
    
    // Main loop
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_WINDOWEVENT && 
                event.window.event == SDL_WINDOWEVENT_CLOSE) {
                running = false;
            }
        }
        
        // Start ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        // Update sequencer logic
        app.updateSequencer();
        
        // Render our UI
        app.renderUI();
        app.renderSequencerUI();
        app.renderSongUI();
        
        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, 25, 25, 30, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }
    
    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
