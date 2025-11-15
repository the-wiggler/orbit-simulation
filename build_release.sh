#!/bin/bash
# Build script for creating a Windows release of Orbital Simulation (MSYS2/MinGW)

echo "==============================================="
echo "Building Orbital Simulation - Release Build"
echo "==============================================="

# Create build directory if it doesn't exist
mkdir -p build

# Navigate to build directory
cd build

echo ""
echo "Configuring CMake..."
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed!"
    exit 1
fi

echo ""
echo "Building project..."
cmake --build . --config Release

if [ $? -ne 0 ]; then
    echo "ERROR: Build failed!"
    exit 1
fi

echo ""
echo "==============================================="
echo "Build successful!"
echo "==============================================="
echo ""
echo "Your distributable files are in: build/"
echo ""
echo "Files ready for distribution:"
echo "  - orbital_sim.exe"
echo "  - SDL3.dll"
echo "  - SDL3_ttf.dll"
echo "  - CascadiaCode.ttf"
echo "  - planet_data.csv"
echo ""
echo "You can zip these files together for release!"
echo "==============================================="

cd ..
