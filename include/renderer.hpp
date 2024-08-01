#include <SDL.h>
#include "chip8.hpp"
#include <array>

class Renderer {
public:
    Renderer(int scale);
    ~Renderer();
    void update(const uint32_t* buffer);
    void handleInput(Chip8& chip8);
    bool quit() const { return quit_; }

private:
    SDL_Window* window_;       // Pointer to the SDL window
    SDL_Renderer* renderer_;   // Pointer to the SDL renderer
    std::array<SDL_Texture*, 3> textures_;  // Array of three textures for triple buffering
    int currentTexture_;       // Index of the current texture being drawn to
    int scale_;                // Scale factor for the window size
    bool quit_;                // Flag to indicate if the application should quit

    bool handleKeyEvent(SDL_Keycode key, bool isPressed, Chip8& chip8);
    void handleWindowEvent(const SDL_WindowEvent& windowEvent);
    void swapBuffers();        // Method to cycle through the textures
};