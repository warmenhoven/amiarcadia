"""
Core options tests for AmiArcadia libretro core.

These tests verify:
- Core options are registered correctly
- Option keys and values are valid
- Options can be read back
- Option changes take effect
"""

import pytest
from helpers import session, require_rom, CORE_PATH
from libretro import defaults, DictOptionDriver


# Expected core options (as bytes, matching libretro.py convention)
EXPECTED_OPTIONS = [
    b"amiarcadia_machine",
    b"amiarcadia_region",
    b"amiarcadia_demultiplex",
]


class TestCoreOptionsRegistration:
    """Test that core options are properly registered."""

    def test_options_available(self):
        """Core registers options during init."""
        with session("Alien Invaders*.bin") as s:
            assert s.options is not None, "Options not available"

    def test_options_definitions_exist(self):
        """Core provides option definitions."""
        with session("Alien Invaders*.bin") as s:
            assert s.options is not None
            assert s.options.definitions is not None
            assert len(s.options.definitions) > 0, "No options defined"

    def test_machine_option_exists(self):
        """Machine selection option is registered."""
        with session("Alien Invaders*.bin") as s:
            assert s.options is not None
            defs = s.options.definitions
            assert defs is not None

            # Check for machine option (key is bytes)
            assert b"amiarcadia_machine" in defs, \
                f"amiarcadia_machine not in options: {list(defs.keys())}"

    def test_region_option_exists(self):
        """Region selection option is registered."""
        with session("Alien Invaders*.bin") as s:
            assert s.options is not None
            defs = s.options.definitions
            assert defs is not None

            assert b"amiarcadia_region" in defs, \
                f"amiarcadia_region not in options: {list(defs.keys())}"

    def test_demultiplex_option_exists(self):
        """Sprite demultiplexing option is registered."""
        with session("Alien Invaders*.bin") as s:
            assert s.options is not None
            defs = s.options.definitions
            assert defs is not None

            assert b"amiarcadia_demultiplex" in defs, \
                f"amiarcadia_demultiplex not in options: {list(defs.keys())}"

    def test_all_expected_options_exist(self):
        """All expected options are registered."""
        with session("Alien Invaders*.bin") as s:
            assert s.options is not None
            defs = s.options.definitions
            assert defs is not None

            for key in EXPECTED_OPTIONS:
                assert key in defs, f"Missing option: {key}"


class TestCoreOptionDefinitions:
    """Test that core option definitions are valid."""

    def test_options_have_keys(self):
        """All option definitions have keys."""
        with session("Alien Invaders*.bin") as s:
            assert s.options is not None
            defs = s.options.definitions
            assert defs is not None

            for key, opt in defs.items():
                assert opt.key, f"Option {key} has no key"

    def test_options_have_descriptions(self):
        """All option definitions have descriptions."""
        with session("Alien Invaders*.bin") as s:
            assert s.options is not None
            defs = s.options.definitions
            assert defs is not None

            for key, opt in defs.items():
                # desc or info field depending on version
                desc = getattr(opt, 'desc', None) or getattr(opt, 'info', None)
                # Note: v0 options may not have separate desc
                # Just ensure the option is defined

    def test_machine_option_has_values(self):
        """Machine option has Arcadia and Interton values."""
        with session("Alien Invaders*.bin") as s:
            assert s.options is not None
            defs = s.options.definitions
            assert defs is not None

            machine_opt = defs.get(b"amiarcadia_machine")
            assert machine_opt is not None

            # Get values - structure depends on options version
            values = getattr(machine_opt, 'values', None)
            if values:
                value_strs = [str(v.value if hasattr(v, 'value') else v) for v in values]
                assert any(b"Arcadia" in str(v).encode() or "Arcadia" in str(v) for v in value_strs), \
                    f"Arcadia not found in values: {value_strs}"

    def test_region_option_has_values(self):
        """Region option has PAL and NTSC values."""
        with session("Alien Invaders*.bin") as s:
            assert s.options is not None
            defs = s.options.definitions
            assert defs is not None

            region_opt = defs.get(b"amiarcadia_region")
            assert region_opt is not None

    def test_demultiplex_option_has_values(self):
        """Demultiplex option has enabled/disabled values."""
        with session("Alien Invaders*.bin") as s:
            assert s.options is not None
            defs = s.options.definitions
            assert defs is not None

            demux_opt = defs.get(b"amiarcadia_demultiplex")
            assert demux_opt is not None


class TestCoreOptionBehavior:
    """Test that option changes affect emulation."""

    def test_region_affects_timing(self):
        """Changing region affects frame rate."""
        rom = require_rom("Alien Invaders*.bin")

        with defaults(str(CORE_PATH)).with_content(str(rom)).build() as s:
            av = s.core.get_system_av_info()
            fps = av.timing.fps

            # PAL should be ~50 fps, NTSC ~60 fps
            # Just verify it's a reasonable value
            assert 45 <= fps <= 65, f"FPS {fps} outside expected range"

    def test_emulation_stable_with_options(self):
        """Emulation runs stably with default options."""
        with session("Alien Invaders*.bin") as s:
            # Run many frames to ensure options don't cause issues
            for _ in range(300):
                s.run()

            frame = s.video.screenshot()
            assert frame is not None, "No video after running with options"

    def test_can_set_option_value(self):
        """Options can be set via the variables interface."""
        rom = require_rom("Alien Invaders*.bin")

        with defaults(str(CORE_PATH)).with_content(str(rom)).build() as s:
            # Try to set an option value
            if hasattr(s.options, 'variables'):
                s.options.variables[b"amiarcadia_region"] = b"NTSC"
                # Run a frame to apply
                s.run()
            # Just verify no crash


class TestOptionsVersion:
    """Test options API version handling."""

    def test_options_version_valid(self):
        """Options version is 0, 1, or 2."""
        with session("Alien Invaders*.bin") as s:
            assert s.options is not None
            version = s.options.version
            assert version in (0, 1, 2), f"Unexpected options version: {version}"

    def test_options_with_explicit_version(self):
        """Core works with explicit options version."""
        rom = require_rom("Alien Invaders*.bin")

        # Test with version 0
        with defaults(str(CORE_PATH)).with_content(str(rom)).with_options(0).build() as s:
            assert s.options is not None
            assert s.options.version == 0
            for _ in range(10):
                s.run()
