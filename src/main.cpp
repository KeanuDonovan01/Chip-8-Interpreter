#include "chip8.hpp"
#include "renderer.hpp"
#include <chrono>
#include <iostream>
#include <thread>

// Set the clock speed of the CHIP-8 CPU
// This value determines how many CPU cycles are executed per second
// Adjust this value to change the overall speed of the emulation
const int CHIP8_CLOCK_SPEED = 500; // Hz

int main(int argc, char** argv)
{
    // Check if the correct number of command-line arguments are provided
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    // Parse command-line arguments
    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    const char* romFilename = argv[3];

    // Initialize the renderer and CHIP-8 emulator
    Renderer renderer(videoScale);
    Chip8 chip8;
    chip8.loadROM(romFilename);

    // Get the current time as the starting point for our timing calculations
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    // Calculate time intervals for frame updates and CPU cycles
    // frameInterval is set to 1/60 second for 60 FPS
    const std::chrono::duration<double> frameInterval(1.0 / 60.0);
    // cycleInterval is set based on the defined CHIP8_CLOCK_SPEED
    const std::chrono::duration<double> cycleInterval(1.0 / CHIP8_CLOCK_SPEED);

    // Main emulation loop
    while (!renderer.quit())
    {
        // Get the current time at the start of each loop iteration
        auto currentTime = std::chrono::high_resolution_clock::now();

        // Handle user input
        renderer.handleInput(chip8);

        // CPU cycle loop: Run as many CPU cycles as necessary based on elapsed time
        while (currentTime - lastCycleTime >= cycleInterval)
        {
            // Execute one CPU cycle
            chip8.cycle();

            // Update the last cycle time
            // We use duration_cast to ensure we're adding the exact cycle interval
            lastCycleTime += std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(cycleInterval);

            // If a draw operation occurred during this cycle, exit the CPU loop
            // This allows us to update the screen immediately when necessary
            if (chip8.drawFlag)
            {
                break;
            }
        }

        // Frame update: Check if it's time to update the screen (60 times per second)
        if (currentTime - lastFrameTime >= frameInterval)
        {
            // Only update the screen if the draw flag is set
            if (chip8.drawFlag)
            {
                renderer.update(chip8.video);
                chip8.drawFlag = false;
            }

            // Update the last frame time
            // Again, we use duration_cast to ensure we're adding the exact frame interval
            lastFrameTime += std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(frameInterval);

            // Update CHIP-8 timers
            // These timers should decrement at 60Hz, which is why we update them here
            if (chip8.delayTimer > 0)
            {
                --chip8.delayTimer;
            }

            if (chip8.soundTimer > 0)
            {
                if (chip8.soundTimer == 1)
                {
                    // Emit a beep sound when the sound timer reaches 1
                    // In this case, we just print "BEEP!" to the console
                    std::cout << "BEEP!" << std::endl;
                }
                --chip8.soundTimer;
            }
        }

        // Sleep for a short time to prevent the loop from consuming 100% CPU
        // This allows other processes to run and reduces power consumption
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}