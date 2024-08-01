#include "renderer.hpp"
#include <iostream>

Renderer::Renderer(int scale) : scale_(scale), quit_(false), currentTexture_(0) {
    SDL_Init(SDL_INIT_VIDEO);
    window_ = SDL_CreateWindow("Chip-8 Emulator", 
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                               VIDEO_WIDTH * scale, VIDEO_HEIGHT * scale, 
                               SDL_WINDOW_SHOWN);
    renderer_ = SDL_CreateRenderer(window_, -1, 
                                   SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    for (int i = 0; i < 3; ++i) {
        textures_[i] = SDL_CreateTexture(renderer_, 
                                         SDL_PIXELFORMAT_RGBA8888,
                                         SDL_TEXTUREACCESS_STREAMING,
                                         VIDEO_WIDTH, VIDEO_HEIGHT);
    }
}

Renderer::~Renderer() {
    for (auto& texture : textures_) {
        SDL_DestroyTexture(texture);
    }
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Renderer::update(const uint32_t* buffer) {
    SDL_UpdateTexture(textures_[currentTexture_], nullptr, buffer, VIDEO_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, textures_[currentTexture_], nullptr, nullptr);
    SDL_RenderPresent(renderer_);
    swapBuffers();
}

void Renderer::swapBuffers() {
    currentTexture_ = (currentTexture_ + 1) % 3;
}

void Renderer::handleInput(Chip8& chip8) {
    SDL_Event event;
    bool keypadChanged = false;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                std::cout << "Quit event received" << std::endl;
                quit_ = true;
                break;
            case SDL_KEYDOWN:
                std::cout << "Key pressed: " << SDL_GetKeyName(event.key.keysym.sym) << std::endl;
                keypadChanged = handleKeyEvent(event.key.keysym.sym, true, chip8);
                break;
            case SDL_KEYUP:
                std::cout << "Key released: " << SDL_GetKeyName(event.key.keysym.sym) << std::endl;
                keypadChanged = handleKeyEvent(event.key.keysym.sym, false, chip8);
                break;
            case SDL_WINDOWEVENT:
                handleWindowEvent(event.window);
                break;
            case SDL_SYSWMEVENT:
                std::cout << "System window manager event received" << std::endl;
                break;
            default:
                if (event.type >= SDL_USEREVENT && event.type < SDL_LASTEVENT) {
                    std::cout << "User-defined event received: " << (event.type - SDL_USEREVENT) << std::endl;
                } else {
                    std::cout << "Unhandled event type: " << event.type << std::endl;
                }
                break;
        }
    }
    
    if (keypadChanged) {
        std::cout << "Current CHIP-8 keypad state: ";
        for (int i = 0; i < 16; i++) {
            std::cout << (chip8.keypad[i] ? '1' : '0');
        }
        std::cout << std::endl;
    }
}

void Renderer::handleWindowEvent(const SDL_WindowEvent& windowEvent) {
    switch (windowEvent.event) {
        case SDL_WINDOWEVENT_SHOWN:
            std::cout << "Window shown" << std::endl;
            break;
        case SDL_WINDOWEVENT_HIDDEN:
            std::cout << "Window hidden" << std::endl;
            break;
        case SDL_WINDOWEVENT_EXPOSED:
            std::cout << "Window exposed" << std::endl;
            break;
        case SDL_WINDOWEVENT_MOVED:
            std::cout << "Window moved to " << windowEvent.data1 << "," << windowEvent.data2 << std::endl;
            break;
        case SDL_WINDOWEVENT_RESIZED:
            std::cout << "Window resized to " << windowEvent.data1 << "x" << windowEvent.data2 << std::endl;
            break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            std::cout << "Window size changed to " << windowEvent.data1 << "x" << windowEvent.data2 << std::endl;
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            std::cout << "Window minimized" << std::endl;
            break;
        case SDL_WINDOWEVENT_MAXIMIZED:
            std::cout << "Window maximized" << std::endl;
            break;
        case SDL_WINDOWEVENT_RESTORED:
            std::cout << "Window restored" << std::endl;
            break;
        case SDL_WINDOWEVENT_ENTER:
            std::cout << "Mouse entered window" << std::endl;
            break;
        case SDL_WINDOWEVENT_LEAVE:
            std::cout << "Mouse left window" << std::endl;
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            std::cout << "Window gained keyboard focus" << std::endl;
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            std::cout << "Window lost keyboard focus" << std::endl;
            break;
        case SDL_WINDOWEVENT_CLOSE:
            std::cout << "Window close requested" << std::endl;
            quit_ = true;
            break;
        default:
            std::cout << "Unhandled window event: " << windowEvent.event << std::endl;
            break;
    }
}

bool Renderer::handleKeyEvent(SDL_Keycode key, bool isPressed, Chip8& chip8) {
    int chipKey = -1;
    switch (key) {
        case SDLK_x: chipKey = 0x0; break;
        case SDLK_1: chipKey = 0x1; break;
        case SDLK_2: chipKey = 0x2; break;
        case SDLK_3: chipKey = 0x3; break;
        case SDLK_q: chipKey = 0x4; break;
        case SDLK_w: chipKey = 0x5; break;
        case SDLK_e: chipKey = 0x6; break;
        case SDLK_a: chipKey = 0x7; break;
        case SDLK_s: chipKey = 0x8; break;
        case SDLK_d: chipKey = 0x9; break;
        case SDLK_z: chipKey = 0xA; break;
        case SDLK_c: chipKey = 0xB; break;
        case SDLK_4: chipKey = 0xC; break;
        case SDLK_r: chipKey = 0xD; break;
        case SDLK_f: chipKey = 0xE; break;
        case SDLK_v: chipKey = 0xF; break;
        case SDLK_ESCAPE:
            quit_ = true;
            return false;
    }

    if (chipKey != -1) {
        chip8.keypad[chipKey] = isPressed ? 1 : 0;
        std::cout << "CHIP-8 key " << std::hex << chipKey << " " 
                  << (isPressed ? "pressed" : "released") << std::endl;
        return true;
    }

    return false;
}