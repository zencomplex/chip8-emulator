cmake_minimum_required(VERSION 3.10)

project(Emulator VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(Emulator 
               src/emu.c
               )

target_include_directories(Emulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)