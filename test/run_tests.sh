#!/bin/bash
# Run all tests for AmiArcadia libretro core

cd "$(dirname "$0")"
source venv/bin/activate

# Run pytest with verbose output
# -v for verbose, -x to stop on first failure (optional)
pytest -v "$@"
