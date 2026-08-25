#include <stdio.h>
#include <SDL2/SDL.h>

#include "emu.h"

// Set all pixels to 0 (black)
void cls() {
    for (int i = 0; i < WIDTH*HEIGHT; i++) {
        Screen[i] = 0;
    }
}

// push to stack
void push(word address) {
    if (SP < STACK_SIZE) {
        Stack[SP] = address;
        SP++;
    }
}

// pop from stack
word pop() {
    word address;
    if (SP > 0) {
        address = Stack[SP - 1];
        SP--;
    }

    return address;
}

void chipInit() {

    // init memory
    for (int i = 0; i < RAM_SIZE; i++) {
        RAM[i] = 0;
    }

    // init stack
    for (int i = 0; i < STACK_SIZE; i++) {
        Stack[i] = 0;
    }

    // init registers
    for (int i = 0; i < 16; i++) {
        V[i] = 0;
    }

    DT = 0;
    ST = 0;
    SP = 0;
    I = 0;
    PC = LOAD_ADDRESS;

    cls();

    // load font
    for (int i = 0; i < 80; i++) {
        RAM[i] = fontSet[i];
    }
}

int loadROM(char *fname) {
    
    FILE *fp;
    long size = 0;
    int result = 0;

    // Open ROM and make sure it exists
    fp = fopen(fname, "rb");
    if (fp == NULL) {
        printf("ROM %s could not be opened!\n", fname);
        result = -1;
        goto EXIT;
    }

    // Get the size of the file
    fseek(fp, 0L, SEEK_END);
    size = (ftell(fp) < RAM_SIZE - LOAD_ADDRESS) ? ftell(fp) : RAM_SIZE - LOAD_ADDRESS;
    fseek(fp, 0L, SEEK_SET);

    // Make sure the ROM fits into memory
    if ((LOAD_ADDRESS + size) > RAM_SIZE) {
        fprintf(stderr, "ROM is too large to fit in memory\n");
        result = -1;
        goto EXIT;
    }

    // Read ROM bytes into memory
    size_t bytes_read = fread(&RAM[LOAD_ADDRESS], 1, size, fp);
    if (bytes_read != size) {
        fprintf(stderr, "Failed to read the complete ROM\n");
        result = -1;
        goto EXIT;
    }


EXIT:
    if (fp) {
        fclose(fp);
    }
    return result;
}

void drawPixel(SDL_Renderer *renderer, int x, int y) {
    for (int h = 0; h < SCALE; h++) {
        for (int w = 0; w < SCALE; w++) {
            SDL_RenderDrawPoint(renderer, (x * SCALE) + w, (y * SCALE) + h);
        }
    }
}

void renderScreen(SDL_Renderer *renderer) {
    for (int h = 0; h < HEIGHT; h++) {
        for (int w = 0; w < WIDTH; w++) {
            if (Screen[h * WIDTH + w] == 1) {
                drawPixel(renderer, w, h);
            }
        }
    }
}

// Fetch
word fetch() {

    word instruction;
    // Read 2-byte instruction PC is currently pointing at from memory
    instruction = RAM[PC] << 8;
    // short circuit if instruction is nonzero
    instruction = instruction | RAM[PC + 1];
    // Increment PC by 2 to fetch the next opcode
    PC += 2;

    return instruction;

}

// Decode
// X: The second nibble. Used to look up one of the 16 registers (VX) from V0 through VF.
// Y: The third nibble. Also used to look up one of the 16 registers (VY) from V0 through VF.
// N: The fourth nibble. A 4-bit number.
// NN: The second byte (third and fourth nibbles). An 8-bit immediate number.
// NNN: The second, third and fourth nibbles. A 12-bit immediate memory address.
void decode(word instruction) {
    word data;
    byte x;
    byte y;

    switch (instruction & 0xF000) {
        // jump: (1NNN) set PC to address
        case JMP:
            PC = instruction & 0x0FFF;
            break;

        // call: push PC to stack and jump to address
        case CALL:
            push(PC);
            PC = instruction & 0x0FFF;
            break;

        // skip if equal: Skip if Vx = byte
        case SE:
            x = (instruction & 0x0F00) >> 8;
            data = instruction & 0x00FF;
            if (V[x] == data) {
                PC += 2;
            }
            break;

        // skip if not equal: Skip if Vx != byte
        case SNE:
            x = (instruction & 0x0F00) >> 8;
            data = instruction & 0x00FF;
            if (V[x] != data) {
                PC += 2;
            }
            break;

        // skip if reqisters are equal: Skip if Vx = Vy
        case SER:
            x = (instruction & 0x0F00) >> 8;
            y = (instruction & 0x00F0) >> 4;
            if (V[x] == V[y]) {
                PC += 2;
            }
            break;

        // setvx: (6XNN) simply set the register VX to the value in the second byte
        case SETVX:
            x = (instruction & 0x0F00) >> 8;
            data = instruction & 0x00FF;
            V[x] = data;
            break;

        // add: (7XNN) add value to register VX
        case ADD:
            x = (instruction & 0x0F00) >> 8;
            data = instruction & 0x00FF;
            V[x] += data;
            break;

        // seti: (ANNN) set I to value
        case SETI:
            data = instruction & 0x0FFF;
            I = data;
            break;
        
        // random number generator
        case RND:
            x = (instruction & 0x0F00) >> 8;
            data = instruction & 0x00FF;

            V[x] = ((byte)rand()) & data;
            break;

        // draw screen (DXYN) starting at V[x], V[y]
        case DRW:
            x = (instruction & 0x0F00) >> 8;
            y = (instruction & 0x00F0) >> 4;

            byte n = (instruction & 0x000F);
            byte row;
            V[15] = 0;
            drawFlag = 1;

            for (int iy = 0; iy < n; iy++) {
                row = RAM[I + iy];

                for (int ix = 0; ix < 8; ix++) {
                    if (row & (0x80 >> ix)) {
                        // collision
                        if (Screen[(V[x] + ix + (V[y] + iy) * WIDTH)]) {
                            V[15] = 1;
                        }
                        // no collision
                        Screen[(V[x] + ix + (V[y] + iy) * WIDTH)] ^= 1;
                    }
                }
            }
            break;

        default:
            printf("Unknown instruction %x\n", instruction);

            
        }
    }



void execute(word instruction) {
    if (instruction == CLS) {
        cls();
    } else if (instruction == RET) {
        word address;
        // pop address off stack
        address = pop();
        // 
        if (address != -1) {
            PC = address;
        }
    } else if (instruction == NOP) {

    } else {
        decode(instruction);
    }
}

void emulate() {
    execute(fetch());
    if (DT > 0) {
        DT--;
    }
    if (ST > 0) {
        ST--;
        soundFlag = 1;
    }
}

void debug_print() {
    for (int i = 0; i < RAM_SIZE; i++) {
        if (RAM[i] != 0) {
            printf("%d %d\n", i, RAM[i]);
        }
    }

    printf("\n\nStack\n");
    for (int i = 0; i < STACK_SIZE; i++) {
        printf("%d ", Stack[i]);
    }

    printf("\n\nScreen\n");
    for(int i = 0; i < WIDTH * HEIGHT; i++) {
        printf("%d ", Screen[i]);
    }

    printf("\n\nRegisters\n");
    for (int i = 0; i < 16; i++) {
        printf("V%X %x\n", i, V[i]);
    }

    printf("\nDT %d\n", DT);
    printf("ST %d\n", ST);
    printf("PC %d\n", PC);
    printf("SP %d\n", SP);
    printf("I %d\n", I);
}


int main(int argc, char** argv) {

    int result = 0;

    chipInit();
    
    if (argc > 1 && loadROM(argv[1]) == -1)
    {
        fprintf(stderr, "Unable to open ROM. %s", SDL_GetError());
        exit(-1);
    }

    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1)
    {
        fprintf(stderr, "Initialization failed. %s", SDL_GetError());
        exit(-1);
    }

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    window = SDL_CreateWindow("Chip 8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH*SCALE, HEIGHT*SCALE, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    // main loop
    for (;;) {

        emulate();

        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break;
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    break;
                }

                for (int i = 0; i < 16; i++) {
                     if (event.key.keysym.sym == keymap[i]) {
                        Keyboard[i] = 1;
                    }
                } 
            }

            // Reset keys on release
            if (event.type == SDL_KEYUP) {
                for (int i = 0; i < 16; i++) {
                    if (event.key.keysym.sym == keymap[i]) {
                        Keyboard[i] = 0;
                    }
                }
            }

            if (event.type == SDL_DROPFILE) {
                chipInit();
                result = loadROM(event.drop.file);
                if (result == -1) {
                    exit(-1);
                }
            }
        }
        else {
            if (drawFlag) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                renderScreen(renderer);

                SDL_RenderPresent(renderer);
                drawFlag = 0;
            }

            SDL_UpdateWindowSurface(window);
        }
    }
    SDL_Quit();

    return 0;

}