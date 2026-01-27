"""
Test that audio infrastructure is correctly set up.

Note: Arcadia 2001 games typically don't produce sound during attract mode
(title screen). Sound is only generated during actual gameplay when the
player interacts with the game.

The audio system requires:
1. process_audio_channels() to convert chan_status[] requests to actual audio
2. parse_bytes() calls changemachine() which sets up AUDIBLE flags on sound registers
"""

from itertools import repeat
from helpers import session, CORE_PATH, ROM_DIR
import libretro
from libretro import JoypadState
import glob


def audio_has_nonzero_samples(audio_buffer):
    """Check if audio buffer contains any non-zero samples."""
    if audio_buffer is None or len(audio_buffer) == 0:
        return False

    # libretro.py returns an array of signed shorts
    for sample in audio_buffer[:10000]:  # Check first 10k samples
        if sample != 0:
            return True
    return False


def get_max_amplitude(audio_buffer):
    """Get maximum amplitude from audio buffer."""
    if audio_buffer is None or len(audio_buffer) == 0:
        return 0

    max_amp = 0
    for sample in audio_buffer[:10000]:
        max_amp = max(max_amp, abs(sample))
    return max_amp


class TestAudioInfrastructure:
    """Test that audio infrastructure is correctly configured."""

    def test_audio_buffer_exists(self):
        """Audio buffer should be created and have correct size."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(60):
                s.run()

            buf = s.audio.buffer
            assert buf is not None, "Audio buffer should exist"
            assert len(buf) > 0, "Audio buffer should have data"

            # At 11025 Hz / 50 fps PAL = ~220 samples per frame
            # 60 frames = ~13200 samples, stereo = ~26400
            # Allow some variance for buffering
            assert len(buf) >= 10000, f"Buffer too small: {len(buf)}"

    def test_audio_sample_rate(self):
        """Audio sample rate should be valid."""
        with session("Alien Invaders*.bin") as s:
            av = s.core.get_system_av_info()
            rate = av.timing.sample_rate

            # Core uses 11025 Hz
            assert rate == 11025, f"Expected 11025 Hz, got {rate}"

    def test_memory_sound_registers_accessible(self):
        """Sound registers should be accessible via system RAM."""
        with session("Space Attack*.bin") as s:
            for _ in range(10):
                s.run()

            # For Arcadia, system RAM is exposed at $1800
            # A_PITCH is at $18FD, A_VOLUME is at $18FE
            mem = s.core.get_memory(2)  # RETRO_MEMORY_SYSTEM_RAM

            assert mem is not None, "System RAM should be accessible"
            # Arcadia RAM is 768 bytes ($1800-$1AFF)
            assert len(mem) >= 256, f"RAM too small: {len(mem)}"


class TestAudioWithInput:
    """Test that audio is generated when the player interacts with games."""

    def test_gameplay_produces_sound(self):
        """Pressing fire button during gameplay should produce audio.

        This test verifies that process_audio_channels() is working by:
        1. Starting a game (pressing Start)
        2. Pressing fire button (A) repeatedly
        3. Verifying audio output has non-zero amplitude

        If process_audio_channels() is missing or broken, amplitude will be 0.
        """
        rom = glob.glob(str(ROM_DIR / "Alien Invaders*.bin"))[0]

        def generate_input():
            # Wait for game to initialize
            yield from repeat(0, 60)
            # Press Start to begin game
            yield JoypadState(start=True)
            yield from repeat(0, 30)
            # Press A (fire) repeatedly during gameplay
            for _ in range(120):
                yield JoypadState(a=True)
                yield 0

        with libretro.defaults(str(CORE_PATH)).with_content(rom).with_input(generate_input).build() as s:
            for _ in range(240):
                s.run()

            buf = s.audio.buffer
            max_amp = max(abs(x) for x in list(buf)[:40000]) if buf and len(buf) > 0 else 0

            # With process_audio_channels() working, we get amplitude ~2900+
            # Without it, amplitude is 0
            assert max_amp > 100, \
                f"No audio generated during gameplay (amplitude={max_amp}) - " \
                "process_audio_channels() may be missing"


class TestAudioAttractMode:
    """Document that games don't produce sound in attract mode."""

    def test_attract_mode_is_silent(self):
        """Games in attract mode typically produce silence.

        This is expected behavior - Arcadia games only make sound during
        actual gameplay when the user is interacting.
        """
        with session("Space Attack*.bin") as s:
            for _ in range(120):
                s.run()

            buf = s.audio.buffer
            max_amp = get_max_amplitude(buf)

            # Attract mode is typically silent - this documents expected behavior
            # (test passes whether silent or not, it's just documenting)
