#!/bin/bash
# Bash script to generate Doxygen documentation
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
DOXYFILE="$SCRIPT_DIR/Doxyfile"

if ! command -v doxygen &> /dev/null; then
    echo "Doxygen is not installed or not in PATH."
    exit 1
fi

echo "Generating Doxygen documentation..."
(
    cd "$REPO_ROOT" || exit 1
    doxygen "$DOXYFILE"
)
echo "Documentation generated in docs/doxygen/html"
