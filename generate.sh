#!/bin/bash
set -e  # exit on error

# ----------------------------
# CONFIGURATION
# ----------------------------
BUILD_DIR="build"
BUILD_TYPE="Debug"
TARGET="$1"
ACTION="$2"

# Detect CPU cores for parallel build
JOBS=$(nproc)

# ----------------------------
# SETUP
# ----------------------------
# First-time configuration
if [ ! -d "$BUILD_DIR" ] || [ ! -f "$BUILD_DIR/Makefile" ]; then
    echo "[INFO] Configuring CMake for the first time..."
    cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
fi

# ----------------------------
# BUILD LOGIC
# ----------------------------
if [ -z "$TARGET" ]; then
    echo "[INFO] Building all targets..."
    cmake --build "$BUILD_DIR" -j"$JOBS"
else
    echo "[INFO] Building target: $TARGET"
    cmake --build "$BUILD_DIR" --target "$TARGET" -j"$JOBS"
fi

# ----------------------------
# OPTIONAL RUN STEP
# ----------------------------
if [ "$ACTION" == "run" ]; then
    echo "[INFO] Running $TARGET..."
    BIN_PATH="$BUILD_DIR/bin/${BUILD_TYPE}-$(uname)/$TARGET"

    if [ -x "$BIN_PATH" ]; then
        "$BIN_PATH"
    elif [ -x "$BUILD_DIR/$TARGET" ]; then
        "$BUILD_DIR/$TARGET"
    elif [ -x "$TARGET" ]; then
        "./$TARGET"
    else
        echo "[ERROR] Cannot find executable for $TARGET."
        exit 1
    fi
fi
