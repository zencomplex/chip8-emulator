#!/bin/bash

set -e # Exit immediately if any command fails

BUILD_TYPE="Release"
CLEAN_BUILD=false
BUILD_DIR="build"

# Parse arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --clean|-c) CLEAN_BUILD=true shift ;;
        --debug|-d) BUILD_TYPE="Debug"; shift ;;
        --release|-r) BUILD_TYPE="Release"; shift ;;
        *) echo "Unknown option: $1"; exit 1 ;;
    esac
done

if [ "$CLEAN_BUILD" = true ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

echo "Configuring CMake ($BUILD_TYPE)..."
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=$BUILD_TYPE

echo "Compiling project..."
cmake --build "$BUILD_DIR" -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
