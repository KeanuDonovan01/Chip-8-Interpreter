# CHIP-8 Interpreter in C++
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Project Overview
This project implements a CHIP-8 interpreter in C++. CHIP-8 is an interpreted programming language developed in the 1970s, primarily used on 8-bit microcomputers. This interpreter allows you to run CHIP-8 ROMs on modern systems.

## Features
- Full implementation of CHIP-8 instruction set
- Support for keyboard input
- ROM loading from file

## Dependencies
- SDL2 (for graphics and input handling)
- C++17 compatible compiler
- CMake (for build system)

## CHIP-8 Architecture
The CHIP-8 system includes:
- Memory: 4KB (4096 bytes)
- Registers: 16 8-bit registers (V0 to VF)
- Stack: 16 16-bit values
- Input: 16-key hexadecimal keypad
- Display: 64x32 pixel monochrome display
- Two timers: delay timer and sound timer

## License
This project is licensed under the MIT License - see the [LICENSE](https://opensource.org/licenses/MIT) file for details.

## Author
Keanu Donovan

## Acknowledgments
- CHIP-8 Technical Reference
- SDL2 library developers
