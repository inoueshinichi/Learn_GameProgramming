#!/bin/sh
# Type a script or drag a script file from your workspace to insert its path.
echo "Run Script for copy file !!! by shinichi inoue"

if [ -d "$BUILD_DIR/Debug" ]; then
    cp "$SRCROOT"/../External/sdl2/"2.0.22"/lib/*.dylib $BUILD_DIR/Debug
fi

if [ -d "$BUILD_DIR/Release" ]; then
    cp "$SRCROOT"/../External/sdl2/"2.0.22"/lib/*.dylib $BUILD_DIR/Release
fi

