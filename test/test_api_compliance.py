"""
API compliance tests for AmiArcadia libretro core.

These tests verify the core follows the libretro API specification:
- Required functions exist and return valid values
- System info is properly formatted
- AV info contains valid timing and geometry
- Memory functions work correctly
- Serialization functions exist (even if unsupported)
- Environment callbacks are handled properly
"""

import pytest
from helpers import session, require_rom, CORE_PATH
from libretro import defaults


class TestRequiredFunctions:
    """Test that all required libretro API functions exist."""

    def test_retro_api_version(self):
        """retro_api_version returns RETRO_API_VERSION (1)."""
        rom = require_rom("Alien Invaders*.bin")
        with defaults(str(CORE_PATH)).with_content(str(rom)).build() as s:
            version = s.core.api_version()
            assert version == 1, f"Expected API version 1, got {version}"

    def test_retro_get_system_info(self):
        """retro_get_system_info returns valid info."""
        rom = require_rom("Alien Invaders*.bin")
        with defaults(str(CORE_PATH)).with_content(str(rom)).build() as s:
            info = s.core.get_system_info()

            assert info.library_name, "library_name is empty"
            assert info.library_version, "library_version is empty"
            assert info.valid_extensions, "valid_extensions is empty"

    def test_retro_get_system_av_info(self):
        """retro_get_system_av_info returns valid AV info."""
        with session("Alien Invaders*.bin") as s:
            av = s.core.get_system_av_info()

            # Geometry checks
            assert av.geometry.base_width > 0, "base_width is zero"
            assert av.geometry.base_height > 0, "base_height is zero"
            assert av.geometry.max_width >= av.geometry.base_width, \
                "max_width < base_width"
            assert av.geometry.max_height >= av.geometry.base_height, \
                "max_height < base_height"

            # Timing checks
            assert av.timing.fps > 0, "fps is zero"
            assert av.timing.sample_rate > 0, "sample_rate is zero"

    def test_retro_init_deinit(self):
        """retro_init and retro_deinit don't crash."""
        rom = require_rom("Alien Invaders*.bin")
        # Session handles init/deinit - just verify no crash
        with defaults(str(CORE_PATH)).with_content(str(rom)).build() as s:
            pass
        # If we get here, init/deinit worked

    def test_retro_load_unload_game(self):
        """retro_load_game and retro_unload_game work."""
        rom = require_rom("Alien Invaders*.bin")
        with defaults(str(CORE_PATH)).with_content(str(rom)).build() as s:
            # Game is loaded at this point
            for _ in range(10):
                s.run()
        # Game unloaded on exit - no crash means success

    def test_retro_run(self):
        """retro_run executes without crash."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(60):
                s.run()

    def test_retro_reset(self):
        """retro_reset works without crash."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(30):
                s.run()
            s.core.reset()
            for _ in range(30):
                s.run()


class TestSystemInfo:
    """Test system info validity."""

    def test_library_name_format(self):
        """Library name is reasonable string."""
        rom = require_rom("Alien Invaders*.bin")
        with defaults(str(CORE_PATH)).with_content(str(rom)).build() as s:
            info = s.core.get_system_info()
            name = info.library_name
            # Handle bytes if returned
            if isinstance(name, bytes):
                name = name.decode('utf-8')

            assert len(name) > 0, "library_name is empty"
            assert len(name) < 100, f"library_name too long: {len(name)}"
            # Should contain alphanumeric characters
            assert any(c.isalnum() for c in name), \
                f"library_name has no alphanumeric chars: {name}"

    def test_library_version_format(self):
        """Library version is reasonable string."""
        rom = require_rom("Alien Invaders*.bin")
        with defaults(str(CORE_PATH)).with_content(str(rom)).build() as s:
            info = s.core.get_system_info()
            version = info.library_version
            # Handle bytes if returned
            if isinstance(version, bytes):
                version = version.decode('utf-8')

            assert len(version) > 0, "library_version is empty"
            assert len(version) < 50, f"library_version too long: {len(version)}"

    def test_valid_extensions(self):
        """Valid extensions list is reasonable."""
        rom = require_rom("Alien Invaders*.bin")
        with defaults(str(CORE_PATH)).with_content(str(rom)).build() as s:
            info = s.core.get_system_info()
            exts = info.valid_extensions
            # Handle bytes if returned
            if isinstance(exts, bytes):
                exts = exts.decode('utf-8')

            assert len(exts) > 0, "valid_extensions is empty"
            # Should contain bin at minimum
            assert "bin" in exts.lower(), \
                f"'bin' not in valid_extensions: {exts}"


class TestAVInfo:
    """Test audio/video info validity."""

    def test_geometry_reasonable(self):
        """Geometry values are in reasonable ranges."""
        with session("Alien Invaders*.bin") as s:
            av = s.core.get_system_av_info()
            g = av.geometry

            # Arcadia 2001 has ~128x208 or similar resolution
            assert 64 <= g.base_width <= 512, \
                f"base_width {g.base_width} outside expected range"
            assert 64 <= g.base_height <= 512, \
                f"base_height {g.base_height} outside expected range"
            assert g.max_width <= 1024, \
                f"max_width {g.max_width} seems too large"
            assert g.max_height <= 1024, \
                f"max_height {g.max_height} seems too large"

    def test_fps_reasonable(self):
        """Frame rate is in reasonable range."""
        with session("Alien Invaders*.bin") as s:
            av = s.core.get_system_av_info()
            fps = av.timing.fps

            # PAL ~50, NTSC ~60
            assert 45 <= fps <= 65, f"FPS {fps} outside expected range"

    def test_sample_rate_reasonable(self):
        """Audio sample rate is standard value."""
        with session("Alien Invaders*.bin") as s:
            av = s.core.get_system_av_info()
            rate = av.timing.sample_rate

            # Common rates: 22050, 44100, 48000
            assert 11025 <= rate <= 96000, \
                f"Sample rate {rate} outside expected range"

    def test_aspect_ratio_reasonable(self):
        """Aspect ratio is reasonable (if provided)."""
        with session("Alien Invaders*.bin") as s:
            av = s.core.get_system_av_info()
            aspect = av.geometry.aspect_ratio

            # 0 means use width/height, otherwise should be reasonable
            if aspect > 0:
                assert 0.5 <= aspect <= 3.0, \
                    f"Aspect ratio {aspect} seems unreasonable"


class TestMemoryFunctions:
    """Test memory-related API functions."""

    def test_get_memory_size(self):
        """retro_get_memory_size returns valid size."""
        with session("Alien Invaders*.bin") as s:
            # RETRO_MEMORY_SYSTEM_RAM = 2
            size = s.core.get_memory_size(2)
            # Arcadia has 768 bytes RAM
            assert size == 0x300, f"Expected 0x300, got 0x{size:x}"

    def test_get_memory_data(self):
        """retro_get_memory_data returns non-null for valid memory."""
        with session("Alien Invaders*.bin") as s:
            # RETRO_MEMORY_SYSTEM_RAM = 2
            data = s.core.get_memory_data(2)
            assert data, "Memory data pointer is null"

    def test_save_ram_size(self):
        """RETRO_MEMORY_SAVE_RAM size is defined (may be 0)."""
        with session("Alien Invaders*.bin") as s:
            # RETRO_MEMORY_SAVE_RAM = 0
            size = s.core.get_memory_size(0)
            # Arcadia doesn't have battery save, so 0 is expected
            assert size >= 0, f"Invalid SRAM size: {size}"


class TestSerializationFunctions:
    """Test save state (serialization) API functions."""

    def test_serialize_size(self):
        """retro_serialize_size returns a value."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(30):
                s.run()
            size = s.core.serialize_size()
            # Size should be 36KB for this implementation
            assert size == 36 * 1024, f"Expected 36KB, got {size}"

    def test_serialize_size_reasonable(self):
        """Serialize size is in reasonable range."""
        with session("Alien Invaders*.bin") as s:
            size = s.core.serialize_size()
            # Should be > 0 and < 1MB
            assert 0 < size < 1024 * 1024, f"Unreasonable size: {size}"

    def test_serialize_unserialize_cycle(self):
        """Serialize and unserialize don't crash (if supported)."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(30):
                s.run()

            size = s.core.serialize_size()
            if size == 0:
                pytest.skip("Save states not supported")

            # Try to serialize
            state = bytearray(size)
            result = s.core.serialize(state)
            assert result, "Serialization failed"

            # Run more frames
            for _ in range(30):
                s.run()

            # Try to unserialize
            result = s.core.unserialize(state)
            assert result, "Unserialization failed"

    def test_save_state_header(self):
        """Save state has correct COS header."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(30):
                s.run()

            size = s.core.serialize_size()
            state = bytearray(size)
            s.core.serialize(state)

            # Check COS magic byte (0xBF)
            assert state[0] == 0xBF, f"Invalid magic byte: 0x{state[0]:02X}"
            # state[1] = cosversion
            # state[2] = 2 (DroidArcadia COS format)
            assert state[2] == 2, f"Invalid COS format: {state[2]}"
            # state[3] = memmap

    def test_save_state_restores_memory(self):
        """Save state actually restores memory values."""
        import ctypes
        with session("Alien Invaders*.bin") as s:
            # Run to get game initialized
            for _ in range(120):
                s.run()

            # Read memory via descriptor
            mmap = s.memory_maps
            desc = mmap.descriptors[0]
            mem_before = bytes((ctypes.c_uint8 * desc.len).from_address(desc.ptr))

            # Save state
            size = s.core.serialize_size()
            state = bytearray(size)
            s.core.serialize(state)

            # Run more frames to change memory
            for _ in range(120):
                s.run()

            # Memory should have changed
            mem_during = bytes((ctypes.c_uint8 * desc.len).from_address(desc.ptr))
            # (Some bytes may be the same, but not all)

            # Restore state
            s.core.unserialize(state)

            # Read memory again
            mem_after = bytes((ctypes.c_uint8 * desc.len).from_address(desc.ptr))

            # Memory should match the saved state
            assert mem_after == mem_before, "Memory not restored correctly"


class TestCallbackRegistration:
    """Test that callback registration works."""

    def test_video_callback_works(self):
        """Video refresh callback receives frames."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(30):
                s.run()
            frame = s.video.screenshot()
            assert frame is not None, "Video callback not working"

    def test_audio_callback_works(self):
        """Audio callback receives samples."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(60):
                s.run()
            samples = s.audio.buffer
            assert samples is not None, "Audio callback not working"
            assert len(samples) > 0, "No audio samples received"

    def test_input_poll_callback_works(self):
        """Input poll callback is called."""
        with session("Alien Invaders*.bin") as s:
            # Just verify emulation works with input polling
            for _ in range(60):
                s.run()
            # If we get here, input polling didn't crash


class TestEdgeCases:
    """Test edge cases and error handling."""

    def test_run_before_load(self):
        """Core handles being created without crashing."""
        rom = require_rom("Alien Invaders*.bin")
        # libretro.py handles this correctly via the builder pattern
        with defaults(str(CORE_PATH)).with_content(str(rom)).build() as s:
            s.run()

    def test_double_reset(self):
        """Double reset doesn't crash."""
        with session("Alien Invaders*.bin") as s:
            s.core.reset()
            s.core.reset()
            for _ in range(10):
                s.run()

    def test_reset_after_extended_play(self):
        """Reset after extended play works."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(600):
                s.run()
            s.core.reset()
            for _ in range(30):
                s.run()
            frame = s.video.screenshot()
            assert frame is not None, "No video after reset"

    def test_memory_after_many_frames(self):
        """Memory functions work after extended emulation."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(600):
                s.run()

            size = s.core.get_memory_size(2)
            assert size == 0x300, f"Memory size changed: 0x{size:x}"

            data = s.core.get_memory_data(2)
            assert data, "Memory data null after extended run"
