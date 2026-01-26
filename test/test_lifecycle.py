"""
Lifecycle tests for AmiArcadia libretro core.

These tests verify:
- Load/unload cycles work correctly
- Multiple resets don't crash
- Extended runtime is stable
- Resource cleanup works
"""

from helpers import session, require_rom, CORE_PATH
from libretro import defaults


class TestLifecycle:
    """Test core lifecycle operations."""

    def test_load_unload_cycle(self):
        """Load ROM, run, unload, repeat without crash."""
        rom = require_rom("Alien Invaders*.bin")

        for cycle in range(3):
            with defaults(str(CORE_PATH)).with_content(str(rom)).build() as s:
                for _ in range(30):
                    s.run()
            # Session closed, resources should be freed

    def test_multiple_resets(self):
        """Reset multiple times in a row without crash."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(30):
                s.run()

            # Reset 10 times
            for i in range(10):
                s.core.reset()
                # Run a few frames after each reset
                for _ in range(10):
                    s.run()

    def test_reset_immediately_after_load(self):
        """Reset immediately after loading doesn't crash."""
        with session("Alien Invaders*.bin") as s:
            s.core.reset()
            for _ in range(30):
                s.run()

    def test_run_extended(self):
        """Run 600 frames (10 seconds) without crash or issues."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(600):
                s.run()

            # Verify still functioning
            frame = s.video.screenshot()
            assert frame is not None, "No video after extended run"

    def test_run_very_extended(self):
        """Run 3600 frames (1 minute) for stability."""
        with session("Alien Invaders*.bin") as s:
            for i in range(3600):
                s.run()

            # Verify still functioning
            frame = s.video.screenshot()
            assert frame is not None, "No video after very extended run"

    def test_rapid_reset_cycle(self):
        """Rapid reset/run cycles don't leak or crash."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(50):
                s.core.reset()
                s.run()
                s.run()


class TestResourceManagement:
    """Test resource allocation and cleanup."""

    def test_repeated_sessions(self):
        """Multiple sequential sessions don't leak resources."""
        rom = require_rom("Alien Invaders*.bin")

        # Create and destroy 10 sessions
        for _ in range(10):
            with defaults(str(CORE_PATH)).with_content(str(rom)).build() as s:
                for _ in range(10):
                    s.run()

    def test_av_info_stable_across_frames(self):
        """AV info remains consistent during emulation."""
        with session("Alien Invaders*.bin") as s:
            av1 = s.core.get_system_av_info()

            for _ in range(100):
                s.run()

            av2 = s.core.get_system_av_info()

            assert av1.geometry.base_width == av2.geometry.base_width
            assert av1.geometry.base_height == av2.geometry.base_height
            assert av1.timing.fps == av2.timing.fps
            assert av1.timing.sample_rate == av2.timing.sample_rate
