#pragma once

#include <cstdint>
#include <random>
#include <chrono>
#include <string>

const unsigned int KEY_COUNT = 16;
const unsigned int MEMORY_SIZE = 4096;
const unsigned int REGISTER_COUNT = 16;
const unsigned int STACK_LEVELS = 16;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

class Chip8
{
public:
    Chip8();
    void loadROM(const std::string& filename);
    void setupTable();
    void cycle();
    bool drawFlag;
    uint8_t delayTimer; // Delay timer
    uint8_t soundTimer; // Sound timer
    uint8_t keypad[KEY_COUNT];
    uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT];

private:
    uint8_t memory[MEMORY_SIZE]; // Chip-8 has 4KB of memory
    uint8_t registers[REGISTER_COUNT]; // 16 general-purpose registers (V0 to VF)
    uint16_t index; // Index register (I)
    uint16_t pc; // Program counter (PC)
    uint16_t stack[STACK_LEVELS]; // Stack for subroutine calls
    uint8_t sp; // Stack pointer
    uint16_t opcode;

    std::default_random_engine randGen;
    std::uniform_int_distribution<unsigned int> randByte;

    //CLS
    void op_00E0();

    //RET
    void op_00EE();

    //JP address
    void op_1nnn();

    //CALL address
    void op_2nnn();

    //SE Vx, kk
    void op_3xkk();

    //SNE Vx, byte
    void op_4xkk();

    //SE Vx, Vy
    void op_5xy0();

    //LD Vx, byte
    void op_6xkk();

    //ADD Vx, byte
    void op_7xkk();

    //LD Vx, Vy
    void op_8xy0();

    //OR Vx, Vy
    void op_8xy1();

    //AND Vx, vy
    void op_8xy2();

    //XOR Vx, Vy
    void op_8xy3();

    //ADD Vx, Vy
    void op_8xy4();

    //SUB Vx, Vy
    void op_8xy5();

    //SHR Vx {, Vy}
    void op_8xy6();

    //SUBN Vx, Vy
    void op_8xy7();

    //SHL Vx {, Vy}
    void op_8xyE();

    //SNE Vx, Vy
    void op_9xy0();

    //LD I, addr
    void op_Annn();

    //Vp V0, addr
    void op_Bnnn();

    //RND Vx, byte
    void op_Cxkk();

    //DRW Vx, Vy, nibble
    void op_Dxyn();

    //SKP Vx
    void op_Ex9E();

    //SKNP Vx
    void op_ExA1();

    //LD Vx, DT
    void op_Fx07();

    //LD Vx, K
    void op_Fx0A();

    //LD DT, Vx
    void op_Fx15();

    //LD ST, Vx
    void op_Fx18();

    //ADD I, Vx
    void op_Fx1E();

    //LD F, Vx
    void op_Fx29();

    //LD B, Vx
    void op_Fx33();

    //LD [I], Vx
    void op_Fx55();

    //LD Vx, [I]
    void op_Fx65();

    void Table0();
    void Table8();
    void TableE();
    void TableF();

    typedef void (Chip8::*Chip8Func)();
    Chip8Func table[0xF + 1];
    Chip8Func table0[0xE + 1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];
};