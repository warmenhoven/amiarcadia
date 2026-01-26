# AmiArcadia libretro core

A libretro core for emulating Signetics 2650 CPU-based systems, based on [DroidArcadia](https://amigan.1emu.net/releases/) by James Jacobs.

## Supported Systems

### Home Consoles
- **Emerson Arcadia 2001** - and compatible systems (Bandai, Grandstand, etc.)
- **Interton VC 4000** - and compatible systems (Voltmace, Rowtron, etc.)
- **Elektor TV Games Computer**

### Arcade (Coin-op)
- **Zaccaria** - Astro Wars, Galaxia, Laser Battle, Lazarian
- **Malzak** - Malzak I & II

## Building

```bash
# Build for current platform
make

# Build with debug symbols
make DEBUG=1

# Clean build artifacts
make clean
```

The compiled core will be named `amiarcadia_libretro.dylib` (macOS), `amiarcadia_libretro.so` (Linux), or `amiarcadia_libretro.dll` (Windows).

## Features

- **Save States** - Full save/load state support using the COS format
- **RetroAchievements** - Memory exposed for achievement tracking
- **Core Options** - Machine selection, region (PAL/NTSC), sprite demultiplexing
- **Two Player Support** - Full two-player input with analog paddle support
- **Keyboard Input** - Desktop keyboard support for keypad overlay

## Core Options

| Option | Values | Description |
|--------|--------|-------------|
| Machine | Arcadia, Interton | Machine type for unknown ROMs (known ROMs auto-detect) |
| Region | PAL, NTSC | Video timing (PAL=50Hz, NTSC=60Hz) |
| Sprite Demultiplexing | enabled, disabled | Reduces sprite flicker in games that multiplex sprites |

## Input Mapping

### Controller (RetroPad)

| Button | Function |
|--------|----------|
| D-Pad | Paddle/Joystick movement |
| Left Analog | Paddle (analog) |
| B | Keypad 1 |
| A | Keypad 2 (Fire) |
| Y | Keypad 4 |
| X | Keypad 5 |
| R | Keypad 0 |
| R2 | Keypad Enter |
| R3 | Keypad 6 |
| L3 | Keypad Clear |
| Right Analog | Keypad 1-9 (8-directional) |
| Start | Console Start |
| Select | Console A |
| L | Console B (Coin for arcade) |
| L2 | Console Reset |

### Keyboard (Player 1)

| Key | Function |
|-----|----------|
| 0-9 | Keypad 0-9 |
| Numpad 0-9 | Keypad 0-9 |
| [ or * | Keypad Clear |
| ] or # | Keypad Enter |

## ROM Detection

The core automatically identifies known ROMs by CRC32 and configures the correct machine type, memory map, and game-specific settings. For unknown ROMs, use the "Machine" core option to select between Arcadia and Interton modes.

Supported ROM extensions: `.bin`, `.tvc`

## Testing

Tests use the [libretro.py](https://pypi.org/project/libretro.py/) library and require Python 3.11 or 3.12.

```bash
# Setup (one-time)
cd test
python -m venv venv
source venv/bin/activate
pip install libretro.py pytest

# Run all tests
./run_tests.sh

# Run specific test file
pytest -v test_smoke.py
```

ROMs should be placed in an `Emerson - Arcadia 2001/` directory accessible to the tests.

## Credits

- **James Jacobs** - Original WinArcadia/DroidArcadia emulator
- **libretro team** - libretro API and RetroArch frontend

## License

Non-commercial use only. See the original [AmiArcadia/WinArcadia](https://amigan.1emu.net/releases/).

## Links

- [AmiArcadia Homepage](https://amigan.1emu.net/releases/)
- [libretro Documentation](https://docs.libretro.com/)
- [RetroArch](https://www.retroarch.com/)
