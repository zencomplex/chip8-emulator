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
#define SCALE           10

byte drawFlag;
byte Display[WIDTH * HEIGHT];

byte soundFlag;

// Keyboard
byte Keyboard[16];

byte RAM[RAM_SIZE];
word Stack[STACK_SIZE];

byte fontSet[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

byte keymap[16] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};

// Registers
byte V[16];         // General purpose registers
byte DT, ST;        // Delay and sound timer registers
byte SP;            // Stack pointer register
word PC;            // Program counter register
word I;             // Index register

// Screen
void drawPixel(SDL_Renderer *renderer, int x, int y);
void renderScreen(SDL_Renderer *renderer);

void chipInit();

int loadROM(char *fname);

#endif

