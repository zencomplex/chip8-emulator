cmake_minimum_required(VERSION 3.10)

project(Emulator VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(Emulator 
               src/emu.c
               )

target_include_directories(Emulator PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)

