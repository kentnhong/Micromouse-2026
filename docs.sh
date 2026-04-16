#!/bin/bash
# Bash script to generate Doxygen documentation
DOXYFILE="$(dirname "$0")/../Doxyfile"

if ! command -v doxygen &> /dev/null; then
    echo "Doxygen is not installed or not in PATH."
    exit 1
fi

echo "Generating Doxygen documentation..."
doxygen "$DOXYFILE"
echo "Documentation generated in .docs/doxygen/html"
