"""
Pytest configuration and shared fixtures for AmiArcadia libretro core tests.
"""

import pytest
from contextlib import contextmanager
from pathlib import Path

from libretro import defaults

# Paths relative to this file
TEST_DIR = Path(__file__).parent
REPO_DIR = TEST_DIR.parent
CORE_PATH = REPO_DIR / "amiarcadia_libretro.dylib"
ROM_DIR = REPO_DIR / "Emerson - Arcadia 2001"


def find_rom(pattern, rom_dir=None):
    """Find ROM matching glob pattern."""
    search_dir = rom_dir or ROM_DIR
    if not search_dir.exists():
        return None
    matches = list(search_dir.glob(pattern))
    return matches[0] if matches else None


def require_rom(pattern, rom_dir=None):
    """Find ROM or skip test if not found."""
    rom = find_rom(pattern, rom_dir)
    if not rom:
        pytest.skip(f"ROM not found: {pattern}")
    return rom


@contextmanager
def session(rom_pattern, rom_dir=None):
    """Create test session with ROM."""
    rom = require_rom(rom_pattern, rom_dir)
    with defaults(str(CORE_PATH)).with_content(str(rom)).build() as s:
        yield s


@pytest.fixture
def arcadia_session():
    """Pytest fixture providing a session with an Arcadia ROM."""
    with session("Alien Invaders*.bin") as s:
        yield s
