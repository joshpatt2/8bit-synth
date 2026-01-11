#!/bin/bash

# Download Dear ImGui
echo "Downloading Dear ImGui..."

cd "$(dirname "$0")"

if [ ! -d "external" ]; then
    mkdir -p external
fi

cd external

if [ -d "imgui" ]; then
    echo "ImGui already exists, updating..."
    cd imgui
    git pull
else
    echo "Cloning ImGui..."
    git clone https://github.com/ocornut/imgui.git
fi

echo "ImGui setup complete!"
