#ifndef CHIP8_H
#define CHIP8_H

// Types
typedef unsigned char   byte;
typedef unsigned short  word;

// Opcodes
#define CLS         0x00E0 // Clear screen
#define JMP         0x1000 // Jump
#define RET         0x00EE // Return
#define SETVX       0x6000 // Set register VX
#define ADD         0x7000 // Add value to register VX
#define SETI        0xA000 // Set index register I
#define DRW         0xD000 // Display/draw

// Load Address
#define LOAD_ADDRESS    0x200

// Memory
#define RAM_SIZE        4096    // byte
#define STACK_SIZE      32      // word

// Display
#define WIDTH           64
#define HEIGHT          32

// Keyboard
byte Keyboard[16];

// Registers
byte V[16];         // General purpose registers
byte DT, ST;        // Delay and sound timer registers
byte SP;            // Stack pointer register
word PC;            // Program counter register
word I;             // Index register
#endif

