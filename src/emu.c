#include <stdio.h>
#include <SDL2/SDL.h>

#include "emu.h"

// Set all pixels to 0 (black)
void cls() {
    for (int i = 0; i < WIDTH*HEIGHT; i++) {
        Display[i] = 0;
    }
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
            if (Display[h * WIDTH + w] == 1) {
                drawPixel(renderer, w, h);
            }
        }
    }
}


int main(int argc, char** argv) {

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
                loadROM(event.drop.file);
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