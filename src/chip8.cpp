#include "chip8.hpp"
#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>
#include <random>
#include <chrono>

const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int START_ADDRESS = 0x200;

uint8_t fontSet[FONTSET_SIZE] = 
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0 in a 5x5 sprite, 1 represents pixel to lit up.
        0x20, 0x60, 0x20, 0x20, 0x70,  // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
        0xF0, 0x80, 0xF0, 0x80, 0x80   // F
    };

void Chip8::setupTable() {
    // Initialize main table
    // The main table is used to determine the general category of the opcode
    // based on the first nibble (4 bits) of the opcode.
    // Each entry in the main table points to the corresponding function or sub-table.
    table[0x0] = &Chip8::Table0;  // Opcodes starting with 0x0 are handled by Table0
    table[0x1] = &Chip8::op_1nnn; // Opcodes starting with 0x1 are handled by op_1nnn
    table[0x2] = &Chip8::op_2nnn; // Opcodes starting with 0x2 are handled by op_2nnn
    table[0x3] = &Chip8::op_3xkk; // Opcodes starting with 0x3 are handled by op_3xkk
    table[0x4] = &Chip8::op_4xkk; // Opcodes starting with 0x4 are handled by op_4xkk
    table[0x5] = &Chip8::op_5xy0; // Opcodes starting with 0x5 are handled by op_5xy0
    table[0x6] = &Chip8::op_6xkk; // Opcodes starting with 0x6 are handled by op_6xkk
    table[0x7] = &Chip8::op_7xkk; // Opcodes starting with 0x7 are handled by op_7xkk
    table[0x8] = &Chip8::Table8;  // Opcodes starting with 0x8 are handled by Table8
    table[0x9] = &Chip8::op_9xy0; // Opcodes starting with 0x9 are handled by op_9xy0
    table[0xA] = &Chip8::op_Annn; // Opcodes starting with 0xA are handled by op_Annn
    table[0xB] = &Chip8::op_Bnnn; // Opcodes starting with 0xB are handled by op_Bnnn
    table[0xC] = &Chip8::op_Cxkk; // Opcodes starting with 0xC are handled by op_Cxkk
    table[0xD] = &Chip8::op_Dxyn; // Opcodes starting with 0xD are handled by op_Dxyn
    table[0xE] = &Chip8::TableE;  // Opcodes starting with 0xE are handled by TableE
    table[0xF] = &Chip8::TableF;  // Opcodes starting with 0xF are handled by TableF

    // Initialize table0
    // Table0 is used to handle opcodes starting with 0x0.
    // The last nibble of the opcode determines the specific instruction.
    table0[0x0] = &Chip8::op_00E0; // Opcode 0x00E0 is handled by op_00E0
    table0[0xE] = &Chip8::op_00EE; // Opcode 0x00EE is handled by op_00EE

    // Initialize table8
    // Table8 is used to handle opcodes starting with 0x8.
    // The last nibble of the opcode determines the specific instruction.
    table8[0x0] = &Chip8::op_8xy0; // Opcode 0x8xy0 is handled by op_8xy0
    table8[0x1] = &Chip8::op_8xy1; // Opcode 0x8xy1 is handled by op_8xy1
    table8[0x2] = &Chip8::op_8xy2; // Opcode 0x8xy2 is handled by op_8xy2
    table8[0x3] = &Chip8::op_8xy3; // Opcode 0x8xy3 is handled by op_8xy3
    table8[0x4] = &Chip8::op_8xy4; // Opcode 0x8xy4 is handled by op_8xy4
    table8[0x5] = &Chip8::op_8xy5; // Opcode 0x8xy5 is handled by op_8xy5
    table8[0x6] = &Chip8::op_8xy6; // Opcode 0x8xy6 is handled by op_8xy6
    table8[0x7] = &Chip8::op_8xy7; // Opcode 0x8xy7 is handled by op_8xy7
    table8[0xE] = &Chip8::op_8xyE; // Opcode 0x8xyE is handled by op_8xyE

    // Initialize tableE
    // TableE is used to handle opcodes starting with 0xE.
    // The last byte of the opcode determines the specific instruction.
    tableE[0x1] = &Chip8::op_ExA1; // Opcode 0xExA1 is handled by op_ExA1
    tableE[0xE] = &Chip8::op_Ex9E; // Opcode 0xEx9E is handled by op_Ex9E

    // Initialize tableF
    // TableF is used to handle opcodes starting with 0xF.
    // The last byte of the opcode determines the specific instruction.
    tableF[0x07] = &Chip8::op_Fx07; // Opcode 0xFx07 is handled by op_Fx07
    tableF[0x0A] = &Chip8::op_Fx0A; // Opcode 0xFx0A is handled by op_Fx0A
    tableF[0x15] = &Chip8::op_Fx15; // Opcode 0xFx15 is handled by op_Fx15
    tableF[0x18] = &Chip8::op_Fx18; // Opcode 0xFx18 is handled by op_Fx18
    tableF[0x1E] = &Chip8::op_Fx1E; // Opcode 0xFx1E is handled by op_Fx1E
    tableF[0x29] = &Chip8::op_Fx29; // Opcode 0xFx29 is handled by op_Fx29
    tableF[0x33] = &Chip8::op_Fx33; // Opcode 0xFx33 is handled by op_Fx33
    tableF[0x55] = &Chip8::op_Fx55; // Opcode 0xFx55 is handled by op_Fx55
    tableF[0x65] = &Chip8::op_Fx65; // Opcode 0xFx65 is handled by op_Fx65
}

Chip8::Chip8()
    : randGen(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())),
      randByte(0, 255)
{
    pc = START_ADDRESS;
    sp = 0;
    opcode = 0;
    index = 0;
    delayTimer = 0;
    soundTimer = 0;

    // Clear display, stack, registers, and memory
    memset(video, 0, sizeof(video));
    memset(stack, 0, sizeof(stack));
    memset(registers, 0, sizeof(registers));
    memset(memory, 0, sizeof(memory));
    memset(keypad, 0, sizeof(keypad));  // Initialize keypad to all zeros

    // Load fonts into memory
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
        memory[FONTSET_START_ADDRESS + i] = fontSet[i];
    }

    // Initialize the random number generator
    randGen.seed(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    // The seed is based on the current time to ensure different random sequences each run

    // Set up the opcode function pointer tables
    setupTable();

    // Clear the display
    memset(video, 0, sizeof(video));
}

void Chip8::loadROM(const std::string& filename) {
    // Open ROM in binary to ensure the computer reads the machine code 
    std::ifstream file(filename, std::ios::binary); // creates an std::ifstream object

    // Check if file was succesfully opened,if file objects evalutes to false, error message is printed to stander error stream
    if (!file) {
        std::cerr << "Failed to open ROM file: "<<filename<<std::endl;
        return;
    }
    //Retrieves size of file
    file.seekg(0, std::ios::end); //moves file position indicator to the end
    std::streampos filesize = file.tellg(); //retrieves current position
    file.seekg(0, std::ios::beg); //moves pi to begining

    // Check if ROM is too large to fit in memory
    if (START_ADDRESS + filesize > MEMORY_SIZE) {
        std::cerr << "Error: ROM file too large. Maximum size is "
                  << (MEMORY_SIZE - START_ADDRESS) << " bytes." << std::endl;
        file.close();
        return;
    }

    //Creates memory buffer equalto file size
    std::vector<uint8_t> buffer(filesize); //Creates a vector with a size equal to filesize, 8-bit
    file.read(reinterpret_cast<char*>(buffer.data()), filesize); //reads the rom data, buffer.data is point is cast to char* using reinterpret_cast to match the type of the file

    for (size_t i=0; i<filesize; ++i) {
        memory[START_ADDRESS + i] = buffer[i];
    } //Loop iterates over each byte in hte buffer and copies it into chip-8 memory, rom data to memory
    file.close(); //Closes file
}

void Chip8::cycle() {
    // Fetch the opcode from memory
    // The opcode is 2 bytes (16 bits) long, so we need to combine two bytes from memory
    // The first byte is shifted left by 8 bits and then ORed with the second byte
    opcode = (memory[pc] << 8) | memory[pc + 1];

    // Increment the program counter to point to the next instruction
    // Since each opcode is 2 bytes long, we increment the PC by 2
    pc += 2;

    // Extract the first nibble of the opcode to determine the instruction category
    // We do this by shifting the opcode right by 12 bits (3 nibbles) and then masking with 0xF
    // This gives us the first nibble of the opcode
    uint8_t instruction = (opcode & 0xF000u) >> 12u;

    // Execute the opcode using the function pointer table
    // We use the first nibble of the opcode as an index into the 'table' array
    // The function pointer stored at that index is then invoked using the ((*this).*(...))() syntax
    ((*this).*(table[instruction]))();

    // Note: Timers are decremented in main.cpp at 60Hz, not here
}

void Chip8::Table0()
{
	((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8()
{
	((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE()
{
	((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF()
{
	((*this).*(tableF[opcode & 0x00FFu]))();
}

void Chip8::op_00E0() {
    memset(video, 0, sizeof(video)); //Clear the display
}

void Chip8::op_00EE() {
    // opcode: 00EE
    // Description: Return from a subroutine
    // The interpreter sets the program counter to the address at the top of the stack,
    // then subtracts 1 from the stack pointer.

    // Check for stack underflow before decrementing the stack pointer
    if (sp == 0) {
        std::cerr << "Error: Stack underflow" << std::endl;
        return;
    }

    // Set the program counter to the address at the top of the stack
    pc = stack[sp];

    // Decrement the stack pointer
    --sp;
}

void Chip8::op_1nnn() {
    // Opcode: 1nnn
    // Description: Jump to location nnn
    // The interpreter sets the program counter to nnn

    // Extract the address from the opcode
    // The opcode has the format: 1nnn
    // where 1 is the opcode identifier and nnn is a 12-bit memory address
    // To extract the address, we perform a bitwise AND operation between the opcode and 0x0FFF
    // 0x0FFF has the binary representation 0000111111111111, which isolates the lower 12 bits of the opcode
    uint16_t address = opcode & 0x0FFF;

    // Set the program counter to the extracted address
    // This effectively jumps the execution of the program to the specified memory location
    pc = address;

    // Execution continues from the new memory location specified by the updated program counter
}

void Chip8::op_2nnn() {
    // opcode: 2nnn
    // Description: Call subroutine at address nnn
    // The interpreter increments the stack pointer, then puts the current PC on the top of the stack.
    // The PC is then set to nnn.

    // Check for stack overflow before incrementing the stack pointer
    if (sp >= STACK_LEVELS - 1) {
        std::cerr << "Error: Stack overflow" << std::endl;
        return;
    }

    // Increment the stack pointer
    ++sp;

    // Push the current program counter onto the stack
    // This allows the emulator to remember the return address after the subroutine finishes
    stack[sp] = pc;

    // Extract the address from the opcode
    // The opcode has the format: 2nnn
    // where 2 is the opcode identifier and nnn is a 12-bit memory address
    uint16_t address = opcode & 0x0FFF;

    // Set the program counter to the extracted address
    // This effectively jumps the execution to the subroutine at memory address nnn
    pc = address;
}

void Chip8::op_3xkk() {
    // Opcode: 3xkk
    // Description: Skip next instruction if Vx = kk
    // The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2

    // Extract the value of Vx from the opcode
    // The opcode has the format: 3xkk
    // where 3 is the opcode identifier, x is a 4-bit value representing the index of Vx, and kk is an 8-bit constant value
    // To extract the index of Vx, we perform a bitwise AND operation between the opcode and 0x0F00, then right shift by 8 bits
    // This returns a 4 bit number (up to 15)
    uint8_t Vx = (opcode & 0x0F00) >> 8; 

    // Extract the constant byte kk from the opcode
    // To extract kk, we perform a bitwise AND operation between the opcode and 0x00FF
    uint8_t kk = opcode & 0x00FF;

    //Compare the value of register Vx with constant byte kk
    if (registers[Vx] == kk) {
        //If register Vx == kk, increase pc by 2
        pc +=2;
    }
}

void Chip8::op_4xkk() {
    //Opcode: 4xkk
    //skips next instruction if vx != kk
    //Intrepreter compares register vx to kk, and if they are not equal, increments the program counter by 2.
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    uint8_t kk = opcode & 0x00FF;

    //Compare the value of register Vx with constant byte kk
    if (registers[Vx] != kk) {
        //If register Vx == kk, increase pc by 2
        pc +=2;
    }
}


void Chip8::op_5xy0() {
    //opcode: 5xy0
    //skips next instruction if Vx = Vy

    uint8_t Vx = (opcode & 0x0F00) >> 8; //Extracts the third bit and right shifts it 8 bits

    uint8_t Vy = (opcode & 0x00F0) >> 4; //Extracts the second bit and right shifts it 4 bits

    //Compare the value of register Vx with register Vy
    if (registers[Vx] == registers[Vy]) {
        //If register Vx == kk, increase pc by 2
        pc +=2;
    }
}

void Chip8::op_6xkk() {
    //opcode: 6xkk
    // Set Vx == kk
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    uint8_t kk = opcode & 0x00FF;

    registers[Vx] = kk; // Puts value of kk in to register Vx
}

void Chip8::op_7xkk() {
    //opcode: 7xkk
    //Adds value of kk to value of regisster vx V, stores in Vx
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    uint8_t kk = opcode & 0x00FF;

    registers[Vx] += kk;
}

void Chip8::op_8xy0() {
    //opcode: 8xy0
    //Stores value of register Vy in register Vx
    uint8_t Vx = (opcode & 0x0F00) >> 8; //Extracts the third bit and right shifts it 8 bits

    uint8_t Vy = (opcode & 0x00F0) >> 4; //Extracts the second bit and right shifts it 4 bits

    registers[Vx] = registers[Vy];
}

void Chip8::op_8xy1() {
    //opcode: 8xy1
    //Performs a bitwise OR on values of Vx and Vy, stores the result in VX
    uint8_t Vx = (opcode & 0x0F00) >> 8; //Extracts the third 4-bit and right shifts it 8 bits

    uint8_t Vy = (opcode & 0x00F0) >> 4; //Extracts the second bit and right shifts it 4 bits

    registers[Vx] |= registers[Vy]; // Bitwise OR on values Vx and Vy
}

void Chip8::op_8xy2() {
    //opcode: 8xy2
    //Performs a bitwise AND on the values of Vx and Vy and stores the result in Vx
    uint8_t Vx = (opcode & 0x0F00) >> 8; //Extracts the third bit and right shifts it 8 bits

    uint8_t Vy = (opcode & 0x00F0) >> 4; //Extracts the second bit and right shifts it 4 bits

    registers[Vx] &= registers[Vy]; // Bitwiise AND

}

void Chip8::op_8xy3() {
    //opcode: 8xy3
    //Performs bitwise XOR on values of registers Vx and Vy, replace that value with value in Vx
    uint8_t Vx = (opcode & 0x0F00) >> 8; //Extracts the third bit and right shifts it 8 bits

    uint8_t Vy = (opcode & 0x00F0) >> 4; //Extracts the second bit and right shifts it 4 bits

    registers[Vx] ^= registers[Vy]; //Bitwise XOR
}

void Chip8::op_8xy4() {
    //opcode: 8xy4
    //The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
    uint8_t Vx = (opcode & 0x0F00) >> 8; //Extracts the third bit and right shifts it 8 bits

    uint8_t Vy = (opcode & 0x00F0) >> 4; //Extracts the second bit and right shifts it 4 bits

    uint16_t sum = registers[Vx] + registers[Vy];

    // Set the carry flag in VF
    registers[0xF] = (sum > 255) ? 1 : 0;

    registers[Vx] = sum & 0xFF; // Stores lowest 8 bit of sum in Vx
}
 
void Chip8::op_8xy5() {
    //opcode: 8xy5
    // If value of registers Vx > Vy, Register VF is set to 1, otherwise 0. Then Vx is subtraced from Vy and the rsult is stored in VX
    uint8_t Vx = (opcode & 0x0F00) >> 8; //Extracts the third bit and right shifts it 8 bits

    uint8_t Vy = (opcode & 0x00F0) >> 4; //Extracts the second bit and right shifts it 4 bits

    registers[0xF] = (registers[Vx] >= registers[Vy]) ? 1 : 0;

    registers[Vx] -= registers[Vy];

}

void Chip8::op_8xy6() {
    //opcode: 8xy6
    //If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    // Check the least significant bit of Vx using a bitwise AND operation with 0x1
    // If the least significant bit is 1, set VF to 1, otherwise set VF to 0
    registers[0xF] = (registers[Vx] & 0x1) != 0 ? 1 : 0;

    // Perform a logical right shift on Vx by 1 bit, effectively dividing Vx by 2
    registers[Vx] >>= 1;

}

void Chip8::op_8xy7() {
    //opcode: 8xy7
    //If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.

    uint8_t Vx = (opcode & 0x0F00) >> 8; //Extracts the third bit and right shifts it 8 bits

    uint8_t Vy = (opcode & 0x00F0) >> 4; //Extracts the second bit and right shifts it 4 bits

    registers[0xF] = (registers[Vy] > registers[Vx]) ? 1 : 0; //If Vy>Vx, set register F to 1, else 0

    registers[Vx] = registers[Vy] - registers[Vx]; //Vx is substracted from Vy

}

void Chip8::op_8xyE() {
    //opcode: 8xyE
    //If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save MSB in VF
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

	registers[Vx] <<= 1;

}

void Chip8::op_9xy0() {
    //opcode: 9xy0
    //The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.

    uint8_t Vx = (opcode & 0x0F00) >> 8; //Extracts the third bit and right shifts it 8 bits

    uint8_t Vy = (opcode & 0x00F0) >> 4; //Extracts the second bit and right shifts it 4 bits

    pc += (registers[Vx] != registers[Vy]) ? 2 : 0;
}

void Chip8::op_Annn() {
    //opcode: Ann
    //Values of register I is set to nnn
    uint16_t address = opcode & 0x0FFF;

    index = address;

}

void Chip8::op_Bnnn() {
    //opcode: Bnn
    //The program counter is set to nnn plus the value of V0.
    uint16_t address = opcode & 0x0FFF;

    pc = registers[0] + address;

}

void Chip8::op_Cxkk() {
    //opcode: cxkk
    //The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx.
    uint8_t kk = opcode & 0x00FF; // Calculate value of kk

    uint8_t Vx = (opcode & 0x0F00) >> 8; //Extracts the third bit and right shifts it 8 bits

    registers[Vx] = kk & randByte(randGen);

}

void Chip8::op_Dxyn() {
    // Extract the X and Y coordinates from the opcode
    uint8_t Vx = (opcode & 0x0F00) >> 8;  // X-coordinate register index
    uint8_t Vy = (opcode & 0x00F0) >> 4;  // Y-coordinate register index
    uint8_t height = opcode & 0x000F;     // Height of the sprite

    // Get the actual X and Y coordinates from the registers
    // Use modulo to ensure the coordinates wrap around the screen
    uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

    // Reset the collision flag (VF) to 0
    registers[0xF] = 0;

    // Iterate over each row of the sprite
    for (unsigned int row = 0; row < height; ++row) {
        // Get the current byte of the sprite data
        uint8_t spriteByte = memory[index + row];

        // Iterate over each bit (pixel) in the current byte
        for (unsigned int col = 0; col < 8; ++col) {
            // Extract the current pixel from the sprite byte
            // We use a bit mask that starts at 10000000 and shifts right
            uint8_t spritePixel = spriteByte & (0x80 >> col);
            
            // Check if the current pixel is within the screen bounds
            if (xPos + col < VIDEO_WIDTH && yPos + row < VIDEO_HEIGHT) {
                // Calculate the address of the screen pixel
                uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

                // If the sprite pixel is on (1)
                if (spritePixel) {
                    // Check for collision
                    // If the screen pixel is already on (white), set VF to 1
                    if (*screenPixel == 0xFFFFFFFF) {
                        registers[0xF] = 1;
                    }

                    // XOR the screen pixel
                    // This will flip the pixel: off->on or on->off
                    *screenPixel ^= 0xFFFFFFFF;
                }
            }
            // Note: Pixels outside the screen bounds are simply ignored (clipped)
        }
    }

    // Set the draw flag to indicate the screen needs updating
    drawFlag = true;
}

void Chip8::op_Ex9E() {
    //opcode: ex9e
    //Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.

    // Extract the value of Vx from the opcode
    // Vx is the index of the register containing the key value to check
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    // Get the key value from the Vx register
    uint8_t key = registers[Vx];

    // Bounds check: ensure key is within valid range (0-15)
    if (key >= KEY_COUNT) {
        std::cerr << "Error: Invalid key value " << (int)key << " in op_Ex9E" << std::endl;
        return;
    }

    // Check if the key corresponding to the value in Vx is currently pressed
    if (keypad[key]) {
        // If the key is pressed, skip the next instruction by increasing the program counter (PC) by 2
        // The PC is normally incremented by 2 after each instruction cycle
        // By incrementing it by 2 here, we effectively skip the next instruction
        pc += 2;
    }
}

void Chip8::op_ExA1() {
    //opcode: exa1
    //Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.

    // Extract the value of Vx from the opcode
    // Vx is the index of the register containing the key value to check
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    // Get the key value from the Vx register
    uint8_t key = registers[Vx];

    // Bounds check: ensure key is within valid range (0-15)
    if (key >= KEY_COUNT) {
        std::cerr << "Error: Invalid key value " << (int)key << " in op_ExA1" << std::endl;
        return;
    }

    // Check if the key corresponding to the value in Vx is currently not pressed
    if (!keypad[key]) {
        // If the key is not pressed, skip the next instruction by increasing the program counter (PC) by 2
        // The PC is normally incremented by 2 after each instruction cycle
        // By incrementing it by 2 here, we effectively skip the next instruction
        pc += 2;
    }
}

void Chip8::op_Fx07() {
    //opcode: fx07
    //The value of DT is placed into Vx.

    // Extract the value of Vx from the opcode
    // Vx is the index of the register to store the delay timer value
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    // Set the value of register Vx to the current value of the delay timer
    registers[Vx] = delayTimer;

}

void Chip8::op_Fx0A() {
    //opcode: fx0a
    //All execution stops until a key is pressed, then the value of that key is stored in Vx.

    // Extract the value of Vx from the opcode
    // Vx is the index of the register to store the pressed key value
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    // Flag to track if a key is pressed
    bool keyPressed = false;

    // Iterate through all the keys in the keypad
    for (int i = 0; i < 16; ++i) {
        // Check if the current key is pressed
        if (keypad[i]) {
            // Store the value of the pressed key in register Vx
            registers[Vx] = i;
            keyPressed = true;
            break;
        }
    }

    // If no key is pressed, decrement the program counter by 2
    // This will cause the interpreter to stay on the same instruction until a key is pressed
    if (!keyPressed) {
        pc -= 2;
    }
}

void Chip8::op_Fx15() {
    //opcode fx15
    //DT is set equal to the value of Vx.

    uint8_t Vx = (opcode & 0x0F00) >> 8;

    delayTimer = registers[Vx];
}

void Chip8::op_Fx18() {
    //opcode: fx18
    //St is set equal to the value of Vx
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    soundTimer = registers[Vx];

}

void Chip8::op_Fx1E() {
    //opcode: fx1e
    //The values of I and Vx are added, and the results are stored in I.
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    index += registers[Vx];

}

void Chip8::op_Fx29() {
    //opcode: fx29
    //The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx.
    uint8_t Vx = (opcode &0x0F00) >> 8;

    // Get the hexadecimal digit value from register Vx
    uint8_t digit = registers[Vx];

    // Bounds check: only hex digits 0-F are valid
    if (digit > 0xF) {
        std::cerr << "Error: Invalid digit " << (int)digit << " in op_Fx29. Only 0-F are valid." << std::endl;
        return;
    }

    // Set the index register I to the memory location of the sprite for the digit
    // Each sprite occupies 5 bytes in memory, so we multiply the digit by 5
    // to get the memory offset from the start of the font set
    index = FONTSET_START_ADDRESS + (5 * digit);

}

void Chip8::op_Fx33() {
    //opcode: fx33
    //The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.

    // Extract the value of Vx from the opcode
    // Vx is the index of the register containing the value to be converted to BCD
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    // Get the value from register Vx
    uint8_t value = registers[Vx];

    // Store the hundreds digit in memory location I
    memory[index] = value / 100;

    // Store the tens digit in memory location I+1
    memory[index + 1] = (value / 10) % 10;

    // Store the ones digit in memory location I+2
    memory[index + 2] = value % 10;

}

void Chip8::op_Fx55() {
    //opcode: fx55
    //The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.

    uint8_t Vx = (opcode & 0x0F00) >>8;

    for (int i = 0; i <= Vx; i++) {
        memory[index + i] = registers[i];
    }

    // Increment the index register I by Vx + 1
    index += Vx + 1;
}
void Chip8::op_Fx65() {
    //opcode: fx55
    //The interpreter reads values from memory starting at location I into registers V0 through Vx.

    uint8_t Vx = (opcode & 0x0F00) >>8;

    // Load the values from memory starting at address I into registers V0 through Vx
    for (int i = 0; i <= Vx; ++i) {
        registers[i] = memory[index + i];
    }

    // Increment the index register I by Vx + 1
    index += Vx + 1;

}





