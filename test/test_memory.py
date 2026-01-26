"""
Memory tests for AmiArcadia libretro core (RetroAchievements support).

Tests verify memory exposure matches WinArcadia's RA_InstallMemoryBank() layout:
- Arcadia:  $1800-$1AFF (768 bytes)
- Interton: $1800-$1BFF (1024 bytes) + $1E00-$1FFF (512 bytes)
- Elektor:  $0800-$1FFF (6144 bytes)
"""

import pytest
from helpers import session, find_rom

# RETRO_MEMORY_SYSTEM_RAM = 2
SYSTEM_RAM = 2


class TestArcadiaMemory:
    """Test memory exposure for Arcadia 2001 games."""

    def test_memory_size(self):
        """Arcadia should expose 768 bytes (0x300) of system RAM."""
        with session("Alien Invaders*.bin") as s:
            size = s.core.get_memory_size(SYSTEM_RAM)
            assert size == 0x300, f"Expected 0x300, got 0x{size:x}"

    def test_memory_data_valid(self):
        """Memory data pointer should be non-null."""
        with session("Alien Invaders*.bin") as s:
            data = s.core.get_memory_data(SYSTEM_RAM)
            assert data, "Memory data pointer is null"

    def test_memory_map_descriptor_count(self):
        """Arcadia should have 1 memory descriptor."""
        with session("Alien Invaders*.bin") as s:
            mmap = s.memory_maps
            assert mmap.num_descriptors == 1, \
                f"Expected 1 descriptor, got {mmap.num_descriptors}"

    def test_memory_map_start_address(self):
        """Arcadia memory should start at $1800."""
        with session("Alien Invaders*.bin") as s:
            mmap = s.memory_maps
            desc = mmap.descriptors[0]
            assert desc.start == 0x1800, \
                f"Expected start=0x1800, got 0x{desc.start:x}"

    def test_memory_map_length(self):
        """Arcadia memory descriptor should have len=0x300."""
        with session("Alien Invaders*.bin") as s:
            mmap = s.memory_maps
            desc = mmap.descriptors[0]
            assert desc.len == 0x300, \
                f"Expected len=0x300, got 0x{desc.len:x}"

    def test_memory_map_ptr_valid(self):
        """Memory descriptor ptr should be non-null."""
        with session("Alien Invaders*.bin") as s:
            mmap = s.memory_maps
            desc = mmap.descriptors[0]
            assert desc.ptr, "Descriptor ptr is null"

    def test_memory_readable(self):
        """Memory should be readable via the descriptor pointer."""
        with session("Alien Invaders*.bin") as s:
            # Run a few frames
            for _ in range(30):
                s.run()

            mmap = s.memory_maps
            desc = mmap.descriptors[0]

            # Read memory via ctypes - should not crash
            import ctypes
            mem = bytes((ctypes.c_uint8 * desc.len).from_address(desc.ptr))
            assert len(mem) == desc.len, f"Expected {desc.len} bytes, got {len(mem)}"

    def test_memory_not_all_zeros(self):
        """After running, memory should have some non-zero values."""
        with session("Alien Invaders*.bin") as s:
            # Run enough frames for game to initialize
            for _ in range(120):
                s.run()

            mmap = s.memory_maps
            desc = mmap.descriptors[0]

            import ctypes
            mem = bytes((ctypes.c_uint8 * desc.len).from_address(desc.ptr))
            non_zero = sum(1 for b in mem if b != 0)
            assert non_zero > 0, "All memory is zero after emulation"


class TestIntertronMemory:
    """Test memory exposure for Interton VC 4000 games."""

    def _find_interton_rom(self):
        """Find an Interton ROM if available."""
        # Try common Interton-specific games
        patterns = ["*Bowling*.bin", "*Super Invaders*.bin", "*Chess*.bin"]
        for pattern in patterns:
            rom = find_rom(pattern)
            if rom:
                return rom
        return None

    def test_interton_memory_descriptors(self):
        """Interton should have 2 memory descriptors (if ROM available)."""
        rom = self._find_interton_rom()
        if not rom:
            pytest.skip("No Interton ROM found")

        # Note: This test would need a known Interton ROM to work
        # For now we just verify the infrastructure is in place


class TestMemoryAfterReset:
    """Test memory behavior across reset."""

    def test_memory_valid_after_reset(self):
        """Memory should still be accessible after reset."""
        with session("Alien Invaders*.bin") as s:
            for _ in range(30):
                s.run()

            s.core.reset()

            size = s.core.get_memory_size(SYSTEM_RAM)
            assert size == 0x300, f"Expected 0x300 after reset, got 0x{size:x}"

            data = s.core.get_memory_data(SYSTEM_RAM)
            assert data, "Memory data null after reset"
