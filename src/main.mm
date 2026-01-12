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
          songView() {}
    
    /// Process a frame: render UI and handle actions
    void update() {
        // Render all views and collect actions
        auto synthActions = synthView.render(synthController.getParams());
        auto sequencerActions = sequencerView.render(sequencerController.getState());
        auto songActions = songView.render(songController.getState());
        
        // Handle synth actions
        for (const auto& action : synthActions) {
            synthController.handleAction(action);
            
            // Special case: when editing sequencer slot, update it
            if (action.type == UserActionType::Export) {
                std::string path = synthController.exportToWav("export.wav");
                synthView.setLastExportPath(path);
            }
        }
        
        // Handle sequencer actions
        for (const auto& action : sequencerActions) {
            sequencerController.handleAction(action);
        }
        
        // Update sequencer timing logic
        sequencerController.update();
        
        // Handle song actions
        for (const auto& action : songActions) {
            songController.handleAction(action);
        }
    }
    
private:
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
    
    // Setup style
    ImGui::StyleColorsDark();
    
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
