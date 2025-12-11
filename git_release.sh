#!/bin/bash
set -e

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Script used to generate Linux and Windows binaries for a git release  #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

TMPDIR=$(mktemp -d /tmp/simplic_release.XXXXXX)

# Linux
make -s clean
echo "Building for Linux"
make -s MODE=release CC=gcc BIN_NAME="x64-linux-simplic" > /dev/null 2>&1
mv build/release/x64-linux-simplic "$TMPDIR"

# Windows
make -s clean
echo "Building for Windows"
make -s MODE=release CC=x86_64-w64-mingw32-gcc BIN_NAME="x64-win-simplic" > /dev/null 2>&1

# Output
mv "$TMPDIR/x64-linux-simplic" build/release/
rm -rf "$TMPDIR"

echo "Release binaries saved in build/release"
