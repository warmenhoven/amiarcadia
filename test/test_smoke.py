"""
Smoke tests for AmiArcadia libretro core.

These tests verify basic functionality:
- Core loads and initializes
- ROM loading works
- Emulation runs without crashing
- Video output is generated
"""

from helpers import session, CORE_PATH


class TestCoreBasics:
    """Test core loading and basic info."""

    def test_core_exists(self):
        """Core dylib exists."""
        assert CORE_PATH.exists(), f"Core not found at {CORE_PATH}"

    def test_core_loads(self):
        """Core loads without error."""
        with session("Alien Invaders*.bin") as s:
            assert s is not None

    def test_api_version(self):
        """Core returns valid API version."""
        with session("Alien Invaders*.bin") as s:
            version = s.core.api_version()
            assert version == 1, f"Expected API version 1, got {version}"

    def test_system_info(self):
        """Core returns valid system info."""
        with session("Alien Invaders*.bin") as s:
            info = s.core.get_system_info()
            assert info.library_name == b"AmiArcadia"
            assert info.library_version  # Should have a version string
            assert b"bin" in info.valid_extensions.lower()


class TestRomLoading:
    """Test ROM loading functionality."""

    def test_load_arcadia_rom(self):
        """Load an Arcadia 2001 ROM."""
        with session("Alien Invaders*.bin") as s:
            assert s is not None

    def test_av_info_after_load(self):
        """AV info is valid after loading ROM."""
        with session("Alien Invaders*.bin") as s:
            av = s.core.get_system_av_info()
            assert av.geometry.base_width == 164
            assert av.geometry.base_height in (226, 269)  # NTSC or PAL
            assert av.geometry.max_width == 240
            assert av.geometry.max_height == 269
            assert av.timing.fps in (50.0, 60.0)  # PAL or NTSC
            assert av.timing.sample_rate > 0


class TestEmulation:
    """Test emulation execution."""

    def test_game_actually_loaded(self):
        """Verify game ROM is actually loaded, not just broken/wiped state.

        This catches regressions where the ROM gets wiped after loading
        (e.g., by calling setmemmap() after parse_bytes()). The broken
        state shows a distinctive pattern: ~50% bright green, ~50% black.
        A properly loaded game has a dominant background color (>70%).
        """
        with session("Alien Invaders*.bin") as s:
            # Run frames to let the game initialize
            for _ in range(60):
                s.run()

            frame = s.video.screenshot()
            assert frame is not None, "No video frame"

            frame_bytes = bytes(frame.data) if hasattr(frame, 'data') else frame.tobytes()

            # Count pixel colors
            colors = {}
            for i in range(0, len(frame_bytes), 2):
                pixel = frame_bytes[i] | (frame_bytes[i+1] << 8)
                colors[pixel] = colors.get(pixel, 0) + 1

            total_pixels = len(frame_bytes) // 2

            # Check 1: A loaded game has a dominant background color (>70% of pixels)
            # The broken state is ~50% green, ~50% black (no dominant color)
            most_common_count = max(colors.values())
            dominant_pct = most_common_count / total_pixels
            assert dominant_pct > 0.70, \
                f"No dominant background color ({dominant_pct:.1%}) - ROM may not have loaded"

            # Check 2: Black should not dominate (broken state has ~48% black)
            black_count = colors.get(0, 0)
            black_pct = black_count / total_pixels
            assert black_pct < 0.30, \
                f"Too much black ({black_pct:.1%}) - ROM may have been wiped"

    def test_run_frames(self):
        """Run multiple frames without crashing."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(60):
                s.run()

    def test_video_output(self):
        """Video frames are generated."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(10):
                s.run()
            frame = s.video.screenshot()
            assert frame is not None, "No video frame captured"
            assert frame.width == 164
            assert frame.height in (226, 269)

    def test_frame_not_all_black(self):
        """Video frame has non-black pixels."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(30):
                s.run()
            frame = s.video.screenshot()
            assert frame is not None
            frame_bytes = bytes(frame.data) if hasattr(frame, 'data') else frame.tobytes()
            has_content = any(b != 0 for b in frame_bytes)
            assert has_content, "Frame appears to be all black"

    def test_reset(self):
        """Reset doesn't crash."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(30):
                s.run()
            s.core.reset()
            for _ in range(30):
                s.run()
