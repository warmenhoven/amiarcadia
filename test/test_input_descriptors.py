"""
Input descriptor tests for AmiArcadia libretro core.

These tests verify:
- Input descriptors are provided for both players
- All expected buttons are mapped
- Analog inputs are defined
- Descriptor format is valid
"""

import pytest
from helpers import session


class TestInputDescriptors:
    """Test input descriptor registration."""

    def test_input_descriptors_exist(self):
        """Core provides input descriptors."""
        with session("Alien Invaders*.bin") as s:
            descriptors = s.input_descriptors

            assert descriptors is not None, "No input descriptors"
            assert len(descriptors) > 0, "Empty input descriptors"

    def test_all_descriptors_have_descriptions(self):
        """All descriptors have description strings."""
        with session("Alien Invaders*.bin") as s:
            descriptors = s.input_descriptors
            assert descriptors is not None

            assert all(d.description for d in descriptors), \
                "Some descriptors missing descriptions"


class TestPlayer1Descriptors:
    """Test Player 1 input descriptor coverage."""

    def test_player1_has_descriptors(self):
        """Player 1 (port 0) has input descriptors."""
        with session("Alien Invaders*.bin") as s:
            descriptors = s.input_descriptors
            assert descriptors is not None

            p1_descs = [d for d in descriptors if d.port == 0]
            assert len(p1_descs) > 0, "No Player 1 descriptors"

    def test_player1_has_dpad(self):
        """Player 1 has D-Pad descriptors."""
        with session("Alien Invaders*.bin") as s:
            descriptors = s.input_descriptors
            assert descriptors is not None

            # RETRO_DEVICE_JOYPAD = 1
            # D-Pad IDs: UP=4, DOWN=5, LEFT=6, RIGHT=7
            p1_joypad = [d for d in descriptors if d.port == 0 and d.device == 1]
            dpad_ids = {4, 5, 6, 7}
            found_ids = {d.id for d in p1_joypad}

            missing = dpad_ids - found_ids
            assert not missing, f"Missing D-Pad descriptors for IDs: {missing}"

    def test_player1_has_buttons(self):
        """Player 1 has button descriptors."""
        with session("Alien Invaders*.bin") as s:
            descriptors = s.input_descriptors
            assert descriptors is not None

            # RETRO_DEVICE_JOYPAD = 1
            p1_joypad = [d for d in descriptors if d.port == 0 and d.device == 1]

            # Should have at least D-Pad (4) + some buttons
            assert len(p1_joypad) >= 6, \
                f"Expected at least 6 joypad descriptors, got {len(p1_joypad)}"

    def test_player1_has_analog(self):
        """Player 1 has analog stick descriptors."""
        with session("Alien Invaders*.bin") as s:
            descriptors = s.input_descriptors
            assert descriptors is not None

            # RETRO_DEVICE_ANALOG = 5
            p1_analog = [d for d in descriptors if d.port == 0 and d.device == 5]

            assert len(p1_analog) >= 2, \
                f"Expected at least 2 analog descriptors, got {len(p1_analog)}"


class TestPlayer2Descriptors:
    """Test Player 2 input descriptor coverage."""

    def test_player2_has_descriptors(self):
        """Player 2 (port 1) has input descriptors."""
        with session("Alien Invaders*.bin") as s:
            descriptors = s.input_descriptors
            assert descriptors is not None

            p2_descs = [d for d in descriptors if d.port == 1]
            assert len(p2_descs) > 0, "No Player 2 descriptors"

    def test_player2_has_dpad(self):
        """Player 2 has D-Pad descriptors."""
        with session("Alien Invaders*.bin") as s:
            descriptors = s.input_descriptors
            assert descriptors is not None

            # RETRO_DEVICE_JOYPAD = 1
            p2_joypad = [d for d in descriptors if d.port == 1 and d.device == 1]
            dpad_ids = {4, 5, 6, 7}
            found_ids = {d.id for d in p2_joypad}

            missing = dpad_ids - found_ids
            assert not missing, f"Missing P2 D-Pad descriptors for IDs: {missing}"

    def test_player2_has_buttons(self):
        """Player 2 has button descriptors."""
        with session("Alien Invaders*.bin") as s:
            descriptors = s.input_descriptors
            assert descriptors is not None

            p2_joypad = [d for d in descriptors if d.port == 1 and d.device == 1]

            # Player 2 should have at least D-Pad + fire buttons
            assert len(p2_joypad) >= 5, \
                f"Expected at least 5 joypad descriptors for P2, got {len(p2_joypad)}"

    def test_player2_has_analog(self):
        """Player 2 has analog stick descriptors."""
        with session("Alien Invaders*.bin") as s:
            descriptors = s.input_descriptors
            assert descriptors is not None

            # RETRO_DEVICE_ANALOG = 5
            p2_analog = [d for d in descriptors if d.port == 1 and d.device == 5]

            assert len(p2_analog) >= 2, \
                f"Expected at least 2 analog descriptors for P2, got {len(p2_analog)}"


class TestDescriptorFormat:
    """Test input descriptor format validity."""

    def test_descriptors_valid_ports(self):
        """All descriptors have valid port numbers (0 or 1)."""
        with session("Alien Invaders*.bin") as s:
            descriptors = s.input_descriptors
            assert descriptors is not None

            for desc in descriptors:
                assert desc.port in (0, 1), \
                    f"Invalid port {desc.port}, expected 0 or 1"

    def test_descriptors_valid_devices(self):
        """All descriptors have valid device types."""
        with session("Alien Invaders*.bin") as s:
            descriptors = s.input_descriptors
            assert descriptors is not None

            # RETRO_DEVICE_JOYPAD=1, RETRO_DEVICE_ANALOG=5
            valid_devices = {1, 5}

            for desc in descriptors:
                assert desc.device in valid_devices, \
                    f"Unexpected device type {desc.device}"

    def test_descriptor_count_reasonable(self):
        """Total descriptor count is reasonable."""
        with session("Alien Invaders*.bin") as s:
            descriptors = s.input_descriptors
            assert descriptors is not None

            # We expect at least 20 descriptors (P1 + P2 basic controls)
            # and no more than 100 (reasonable upper bound)
            assert 20 <= len(descriptors) <= 100, \
                f"Descriptor count {len(descriptors)} seems unreasonable"


class TestSpecificMappings:
    """Test specific input mappings for Arcadia controllers."""

    def test_fire_button_mapped(self):
        """Fire button (A) is mapped for both players."""
        with session("Alien Invaders*.bin") as s:
            descriptors = s.input_descriptors
            assert descriptors is not None

            # RETRO_DEVICE_ID_JOYPAD_A = 8
            p1_a = [d for d in descriptors if d.port == 0 and d.device == 1 and d.id == 8]
            p2_a = [d for d in descriptors if d.port == 1 and d.device == 1 and d.id == 8]

            assert len(p1_a) > 0, "P1 A button not mapped"
            assert len(p2_a) > 0, "P2 A button not mapped"

    def test_start_button_mapped(self):
        """Start button is mapped for player 1."""
        with session("Alien Invaders*.bin") as s:
            descriptors = s.input_descriptors
            assert descriptors is not None

            # RETRO_DEVICE_ID_JOYPAD_START = 3
            p1_start = [d for d in descriptors if d.port == 0 and d.device == 1 and d.id == 3]

            assert len(p1_start) > 0, "P1 Start button not mapped"
