#!/bin/bash
# Package the Orbital Simulation release for distribution (MSYS2/MinGW)

echo "==============================================="
echo "Packaging Orbital Simulation for Release"
echo "==============================================="

# Get version or use default
VERSION="1.0.0"
RELEASE_NAME="orbital-simulation-v${VERSION}-windows"

# Create release directory
mkdir -p releases
rm -rf "releases/${RELEASE_NAME}"
mkdir -p "releases/${RELEASE_NAME}"

echo ""
echo "Copying files to release directory..."

# Copy executable and dependencies
cp build/orbital_sim.exe "releases/${RELEASE_NAME}/" 2>/dev/null
cp build/*.dll "releases/${RELEASE_NAME}/" 2>/dev/null
cp build/CascadiaCode.ttf "releases/${RELEASE_NAME}/" 2>/dev/null
cp build/planet_data.csv "releases/${RELEASE_NAME}/" 2>/dev/null

# Copy license if it exists
if [ -f "LICENSE" ]; then
    cp LICENSE "releases/${RELEASE_NAME}/"
fi

echo ""
echo "Creating ZIP archive..."

# Create ZIP
cd releases
zip -r "${RELEASE_NAME}.zip" "${RELEASE_NAME}"

if [ $? -eq 0 ]; then
    echo ""
    echo "==============================================="
    echo "Release package created successfully!"
    echo "==============================================="
    echo ""
    echo "Location: releases/${RELEASE_NAME}.zip"
    echo "Folder:   releases/${RELEASE_NAME}/"
    echo ""
    echo "You can now upload this ZIP to GitHub Releases!"
    echo "==============================================="
else
    echo ""
    echo "ZIP creation failed. Files are available in: releases/${RELEASE_NAME}/"
    echo "You may need to install zip: pacman -S zip"
fi

cd ..
