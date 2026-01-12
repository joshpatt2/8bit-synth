#include <SDL2/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "controller/SynthController.h"
#include "controller/SequencerController.h"
#include "controller/SongController.h"
#include "view/SynthView.h"
#include "view/SequencerView.h"
#include "view/SongView.h"
#include "view/SynthTheme.h"

#include <vector>
#include <string>

/// Main application orchestrator.
/// Sets up MVC architecture: Controllers manage state, Views render UI and emit actions.
class SynthApp {
public:
    SynthApp()
        : synthController(),
          sequencerController(),
          songController(),
          synthView(),
          sequencerView(),
          songView() {
        // Real-time preview disabled by default. User toggles via Preview button.
    }
    
    ~SynthApp() {
        // Stop any active audio on shutdown
        synthController.stopRealtimePreview();
    }
    
    /// Process a frame: render UI and handle actions
    void update() {
        // Get window size for full-screen layout
        ImGuiIO& io = ImGui::GetIO();
        
        // Main window covering the entire viewport
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("8-Bit Synth Studio", nullptr, 
            ImGuiWindowFlags_NoTitleBar | 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus);
        
        // Header bar
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.914f, 0.271f, 0.376f, 1.00f));
        ImGui::Text("🎵 8-BIT SYNTH STUDIO");
        ImGui::PopStyleColor();
        ImGui::SameLine(io.DisplaySize.x - 200);
        ImGui::TextColored(ImVec4(0.545f, 0.545f, 0.545f, 1.0f), "Real-time Preview Active");
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // Calculate layout dimensions
        float availableWidth = io.DisplaySize.x - 32;  // Padding
        float leftColumnWidth = 420;
        float rightColumnWidth = availableWidth - leftColumnWidth - 16;
        float topRowHeight = 380;
        
        // === LEFT COLUMN: Synth Controls ===
        ImGui::BeginChild("LeftColumn", ImVec2(leftColumnWidth, -1), false);
        {
            auto synthActions = renderSynthSection(synthController.getParams());
            for (const auto& action : synthActions) {
                synthController.handleAction(action);
                if (action.type == UserActionType::Export) {
                    std::string path = synthController.exportToWav("export.wav");
                    synthView.setLastExportPath(path);
                }
            }
        }
        ImGui::EndChild();
        
        ImGui::SameLine(0, 16);
        
        // === RIGHT COLUMN: Sequencer + Song ===
        ImGui::BeginChild("RightColumn", ImVec2(rightColumnWidth, -1), false);
        {
            // Sequencer section (top)
            ImGui::BeginChild("SequencerSection", ImVec2(-1, topRowHeight), true);
            {
                auto sequencerActions = renderSequencerSection(sequencerController.getState());
                for (const auto& action : sequencerActions) {
                    sequencerController.handleAction(action);
                }
            }
            ImGui::EndChild();
            
            ImGui::Spacing();
            
            // Song section (bottom)
            ImGui::BeginChild("SongSection", ImVec2(-1, -1), true);
            {
                auto songActions = renderSongSection(songController.getState());
                for (const auto& action : songActions) {
                    if (action.type == UserActionType::SavePattern) {
                        // Get current synth and sequencer state and save pattern
                        songController.savePattern(action.stringValue, 
                                                  synthController.getParams(),
                                                  sequencerController.getState());
                    } else if (action.type == UserActionType::LoadPattern) {
                        // Load pattern and update synth and sequencer
                        SynthParams newSynthParams;
                        SequencerState newSequencerState;
                        
                        if (songController.loadPattern(action.filepath, newSynthParams, newSequencerState)) {
                            synthController.getParamsMutable() = newSynthParams;
                            sequencerController.setState(newSequencerState);
                        }
                    } else {
                        songController.handleAction(action);
                    }
                }
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();
        
        ImGui::End();
        
        // Update sequencer timing logic
        sequencerController.update();
    }
    
private:
    /// Render synth controls section (embedded, no window chrome)
    std::vector<UserAction> renderSynthSection(const SynthParams& params) {
        return synthView.render(params, true);  // embedded = true
    }
    
    /// Render sequencer section (embedded)
    std::vector<UserAction> renderSequencerSection(const SequencerState& state) {
        return sequencerView.render(state, true);  // embedded = true
    }
    
    /// Render song section (embedded)
    std::vector<UserAction> renderSongSection(const SongState& state) {
        // Update pattern list before rendering
        songView.setAvailablePatterns(songController.getAvailablePatterns());
        return songView.render(state, true);  // embedded = true
    }
    
    SynthController synthController;
    SequencerController sequencerController;
    SongController songController;
    
    SynthView synthView;
    SequencerView sequencerView;
    SongView songView;
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
        "8-Bit Synth Studio",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1200,
        800,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );
    
    if (!window) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    // Setup SDL Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED
    );
    
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
    
    // Load custom font (try multiple paths)
    ImFont* font = io.Fonts->AddFontFromFileTTF("assets/fonts/JetBrainsMono-Regular.ttf", 14.0f);
    if (!font) {
        font = io.Fonts->AddFontFromFileTTF("../assets/fonts/JetBrainsMono-Regular.ttf", 14.0f);
    }
    if (!font) {
        // Fall back to default font
        io.Fonts->AddFontDefault();
    }
    
    // Apply custom theme
    SynthTheme::Apply();
    
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    
    // Create application
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
        
        // Update application state and render UI
        app.update();
        
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
