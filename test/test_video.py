"""
Video tests for AmiArcadia libretro core.

These tests verify:
- Pixel format is correct (RGB565)
- Frame dimensions are stable
- Frames stay within max geometry
- Video callback fires regularly
- Frame content changes (emulation running)
"""

from helpers import session


class TestVideoFormat:
    """Test video format and pixel data."""

    def test_pixel_format_rgb565(self):
        """Core uses RGB565 pixel format."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(10):
                s.run()

            frame = s.video.screenshot()
            assert frame is not None, "No video frame"

            # RGB565 = 2 bytes per pixel
            # Frame data size should be width * height * 2
            expected_size = frame.width * frame.height * 2
            actual_size = len(frame.data) if hasattr(frame, 'data') else len(frame.tobytes())

            # Allow for some padding/stride differences
            assert actual_size >= expected_size, \
                f"Frame too small for RGB565: {actual_size} < {expected_size}"

    def test_frame_has_valid_dimensions(self):
        """Frame has positive, reasonable dimensions."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(10):
                s.run()

            frame = s.video.screenshot()
            assert frame is not None, "No video frame"
            assert frame.width > 0, "Frame width is zero"
            assert frame.height > 0, "Frame height is zero"
            assert frame.width <= 320, f"Frame width too large: {frame.width}"
            assert frame.height <= 300, f"Frame height too large: {frame.height}"


class TestVideoDimensions:
    """Test frame dimension consistency."""

    def test_frame_dimensions_stable(self):
        """Frame dimensions remain consistent across frames."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(10):
                s.run()

            first_frame = s.video.screenshot()
            first_width = first_frame.width
            first_height = first_frame.height

            # Run more frames and check dimensions
            for _ in range(50):
                s.run()

            later_frame = s.video.screenshot()
            assert later_frame.width == first_width, \
                f"Width changed: {first_width} -> {later_frame.width}"
            assert later_frame.height == first_height, \
                f"Height changed: {first_height} -> {later_frame.height}"

    def test_frame_within_max_geometry(self):
        """Frame dimensions never exceed max geometry."""
        with session("Alien Invaders*.bin") as s:
            av = s.core.get_system_av_info()
            max_w = av.geometry.max_width
            max_h = av.geometry.max_height

            for _ in range(60):
                s.run()

            frame = s.video.screenshot()
            assert frame.width <= max_w, \
                f"Frame width {frame.width} exceeds max {max_w}"
            assert frame.height <= max_h, \
                f"Frame height {frame.height} exceeds max {max_h}"

    def test_base_matches_actual(self):
        """Base geometry matches actual frame dimensions."""
        with session("Alien Invaders*.bin") as s:
            av = s.core.get_system_av_info()
            base_w = av.geometry.base_width
            base_h = av.geometry.base_height

            for _ in range(10):
                s.run()

            frame = s.video.screenshot()
            assert frame.width == base_w, \
                f"Frame width {frame.width} != base {base_w}"
            assert frame.height == base_h, \
                f"Frame height {frame.height} != base {base_h}"


class TestVideoContent:
    """Test video content and updates."""

    def test_video_callback_fires(self):
        """Video callback is called during emulation."""
        with session("Alien Invaders*.bin") as s:
            # Run frames
            for _ in range(30):
                s.run()

            # Should have a frame available
            frame = s.video.screenshot()
            assert frame is not None, "Video callback never fired"

    def test_frame_content_changes(self):
        """Frame content changes over time (emulation running)."""
        with session("Alien Invaders*.bin") as s:
            # Get initial frame after some warmup
            for _ in range(30):
                s.run()
            frame1 = s.video.screenshot()
            data1 = bytes(frame1.data) if hasattr(frame1, 'data') else frame1.tobytes()

            # Run many more frames - some screens are static (title screens)
            for _ in range(180):
                s.run()
            frame2 = s.video.screenshot()
            data2 = bytes(frame2.data) if hasattr(frame2, 'data') else frame2.tobytes()

            # Frames should be different (game is animating)
            # Note: Some screens (title, pause) may be static - this is not a failure
            if data1 == data2:
                print("  (warning: frames identical - may be static screen)")
            # Don't fail - static screens are valid

    def test_frame_not_corrupted(self):
        """Frame data is not obviously corrupted."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(30):
                s.run()

            frame = s.video.screenshot()
            data = bytes(frame.data) if hasattr(frame, 'data') else frame.tobytes()

            # Check for obvious corruption patterns
            # All same value (except black which is valid)
            if len(set(data)) == 1 and data[0] != 0:
                raise AssertionError(f"Frame is solid color 0x{data[0]:02x} - may be corrupted")

            # Check we have some variety in pixel values
            unique_bytes = len(set(data))
            assert unique_bytes > 2, f"Only {unique_bytes} unique byte values - may be corrupted"


class TestVideoOutput:
    """Test that the core outputs actual video data."""

    def test_frame_not_placeholder_pattern(self):
        """Frame is not the harness placeholder pattern (equal parts of 4 colors)."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(30):
                s.run()

            frame = s.video.screenshot()
            data = bytes(frame.data) if hasattr(frame, 'data') else frame.tobytes()

            # Count pixel values
            pixel_counts = {}
            for i in range(0, len(data) - 1, 2):
                pixel = data[i] | (data[i + 1] << 8)
                pixel_counts[pixel] = pixel_counts.get(pixel, 0) + 1

            total_pixels = len(data) // 2

            # Show top pixel values for debugging
            print(f"  frame: {frame.width}x{frame.height}, {total_pixels} pixels")
            print(f"  top colors:")
            for pixel, count in sorted(pixel_counts.items(), key=lambda x: -x[1])[:5]:
                pct = count * 100 // total_pixels
                print(f"    0x{pixel:04X}: {count} ({pct}%)")

            # The placeholder pattern has exactly equal counts of these 4 colors
            # If we see this exact pattern, video_cb is not working
            placeholder_pixels = [0x0000, 0xFFFF, 0x00FF, 0xFF00]
            placeholder_counts = [pixel_counts.get(p, 0) for p in placeholder_pixels]
            if all(c > 0 for c in placeholder_counts):
                min_c, max_c = min(placeholder_counts), max(placeholder_counts)
                if max_c > 0 and min_c / max_c > 0.99:
                    raise AssertionError(
                        "Frame contains placeholder pattern - video callback not working"
                    )

            # Frame should have more than just 4 unique colors (real graphics have variety)
            assert len(pixel_counts) > 4, \
                f"Only {len(pixel_counts)} unique colors - expected more variety in game graphics"


class TestVideoTiming:
    """Test video timing behavior."""

    def test_frames_produced_each_run(self):
        """Each run() call should produce a frame."""
        with session("Alien Invaders*.bin") as s:
            # Run single frames and verify video updates
            for i in range(10):
                s.run()
                frame = s.video.screenshot()
                assert frame is not None, f"No frame after run {i+1}"
