#!/bin/bash

# Quick build script for 8-bit synthesizer

set -e

cd "$(dirname "$0")"

echo "🎵 Building 8-Bit Synthesizer..."

# Setup ImGui if not present
if [ ! -d "external/imgui" ]; then
    echo "ImGui not found, downloading..."
    ./setup_imgui.sh
fi

# Build
cd build
cmake ..
make -j$(sysctl -n hw.ncpu)

echo "✅ Build complete!"
echo "Run with: ./8bit-synth"
