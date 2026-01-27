/*
 * AmiArcadia libretro core
 *
 * Emulator for Signetics 2650 CPU-based systems:
 * - Arcadia 2001 / Interton VC 4000 / Elektor TV Games
 * - Zaccaria coin-ops (Astro Wars, Galaxia, Laser Battle)
 * - Malzak coin-ops
 *
 * Based on DroidArcadia by James Jacobs
 * https://amigan.1emu.net/releases/
 *
 * This file provides the libretro API. Platform layer comes from android.c
 * with JNI functions guarded by #ifndef LIBRETRO.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "libretro.h"

/* DroidArcadia headers - da.h has JNI guarded with #ifndef LIBRETRO */
#include "../DroidArcadia/app/src/main/cpp/da.h"

/* Extern declarations for variables/functions defined in android.c */
extern UWORD*     display;
extern UBYTE*     IOBuffer;
extern ASCREEN    screen[MAXBOXWIDTH][MAXBOXHEIGHT];
extern TEXT       fn_game[1024 + 1];
extern FLAG       guestplaying[TOTALCHANNELS];
extern UBYTE      hx[2], hy[2];
extern ULONG      frames, hinput[2], region, sound;
extern int        colourset, machine, memmap, whichgame;
extern int        console_start, console_a, console_b, console_reset;
extern int        SoundLength[TOTALCHANNELS];
extern SWORD      SoundBuffer[TOTALCHANNELS][SOUNDLENGTH * 2];
extern const UWORD pencolours[4][GUESTCOLOURS];
extern int        filesize;

extern void da_generate_crc32table(void);
extern void engine_reset(void);
extern int  parse_bytes(FLAG);
extern void uvi(void);
extern void newpvi(void);
extern void oldpvi(void);
extern void serialize_cos(void);
extern void play_any(int, float, int);

/* Serialization globals */
extern int serializemode;
extern int offset;

/* Memory arrays for RetroAchievements support */
extern UBYTE memory[32768];
extern UWORD mirror_r[32768];

/* Sound channel state (from android.c) */
extern UBYTE chan_volume[GUESTCHANNELS];
extern int   chan_status[TOTALCHANNELS];
extern float chan_hertz[GUESTCHANNELS];

/* Core options */
extern ULONG demultiplex;

/* ========================================================================= */
/* libretro interface                                                         */
/* ========================================================================= */

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;
static retro_log_printf_t log_cb;

/* Log helper - formats string then calls log_cb (works around callbacks that don't handle varargs) */
#define LOG_INFO(...) do { \
    if (log_cb) { \
        char _log_buf[256]; \
        snprintf(_log_buf, sizeof(_log_buf), __VA_ARGS__); \
        log_cb(RETRO_LOG_INFO, _log_buf); \
    } \
} while(0)

#define LOG_WARN(...) do { \
    if (log_cb) { \
        char _log_buf[256]; \
        snprintf(_log_buf, sizeof(_log_buf), __VA_ARGS__); \
        log_cb(RETRO_LOG_WARN, _log_buf); \
    } \
} while(0)

static UWORD framebuffer[MAXBOXWIDTH * MAXBOXHEIGHT];

/* Audio runs at 11025 Hz (matching DroidArcadia's samplerate) */
#define AUDIO_SAMPLE_RATE 11025
#define SAMPLES_PER_FRAME_PAL  221  /* 11025 / 50 = 220.5, round up */
#define SAMPLES_PER_FRAME_NTSC 184  /* 11025 / 60 = 183.75, round up */
static int16_t audio_buffer[SAMPLES_PER_FRAME_PAL * 2];

static bool game_loaded = false;
static int current_width = BOXWIDTH;
static int current_height = BOXHEIGHT;

/* For reloading unknown ROMs when machine option changes */
static UBYTE *rom_data = NULL;
static int rom_size = 0;
static int option_machine = ARCADIA;

/* Keyboard input state (bits match hinput layout) */
static ULONG keyboard_input = 0;

/*
 * Right analog stick maps to keypad keys 1-9 (excluding 5) in 8 directions:
 *   7 8 9      ↖ ↑ ↗
 *   4   6  =   ←   →
 *   1 2 3      ↙ ↓ ↘
 */
static const int analog_keypad_map[8] = {
    (1 << 6),  /* up-left     → key 7 (bit 6) */
    (1 << 7),  /* up          → key 8 (bit 7) */
    (1 << 8),  /* up-right    → key 9 (bit 8) */
    (1 << 5),  /* right       → key 6 (bit 5) */
    (1 << 2),  /* down-right  → key 3 (bit 2) */
    (1 << 1),  /* down        → key 2 (bit 1) */
    (1 << 0),  /* down-left   → key 1 (bit 0) */
    (1 << 3),  /* left        → key 4 (bit 3) */
};

#define ANALOG_DEADZONE 8192
#define PI 3.14159265358979323846

/* ========================================================================= */
/* libretro API implementation                                                */
/* ========================================================================= */

/*
 * Keyboard callback for desktop play and overlays.
 * Handles both number row (RETROK_0-9) and numeric keypad (RETROK_KP0-9).
 * Overlays typically use retrok_keypad* for numeric pad buttons.
 */
static void keyboard_cb(bool down, unsigned keycode, uint32_t character, uint16_t mods)
{
    (void)character;
    (void)mods;

    ULONG bit = 0;
    switch (keycode)
    {
    /* Number row (physical keyboard) */
    case RETROK_1: bit = (1 << 0);  break;
    case RETROK_2: bit = (1 << 1);  break;
    case RETROK_3: bit = (1 << 2);  break;
    case RETROK_4: bit = (1 << 3);  break;
    case RETROK_5: bit = (1 << 4);  break;
    case RETROK_6: bit = (1 << 5);  break;
    case RETROK_7: bit = (1 << 6);  break;
    case RETROK_8: bit = (1 << 7);  break;
    case RETROK_9: bit = (1 << 8);  break;
    case RETROK_0: bit = (1 << 10); break;

    /* Numeric keypad (overlays use these) */
    case RETROK_KP1: bit = (1 << 0);  break;
    case RETROK_KP2: bit = (1 << 1);  break;
    case RETROK_KP3: bit = (1 << 2);  break;
    case RETROK_KP4: bit = (1 << 3);  break;
    case RETROK_KP5: bit = (1 << 4);  break;
    case RETROK_KP6: bit = (1 << 5);  break;
    case RETROK_KP7: bit = (1 << 6);  break;
    case RETROK_KP8: bit = (1 << 7);  break;
    case RETROK_KP9: bit = (1 << 8);  break;
    case RETROK_KP0: bit = (1 << 10); break;

    /* Clear: [ or * or keypad multiply */
    case RETROK_LEFTBRACKET:  bit = (1 << 9);  break;
    case RETROK_ASTERISK:     bit = (1 << 9);  break;
    case RETROK_KP_MULTIPLY:  bit = (1 << 9);  break;

    /* Enter: ] or # or keypad enter */
    case RETROK_RIGHTBRACKET: bit = (1 << 11); break;
    case RETROK_HASH:         bit = (1 << 11); break;
    case RETROK_KP_ENTER:     bit = (1 << 11); break;

    default: return;
    }

    if (down)
        keyboard_input |= bit;
    else
        keyboard_input &= ~bit;
}

void retro_init(void)
{
    struct retro_log_callback logging;

    if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
        log_cb = logging.log;
    else
        log_cb = NULL;

    /* Initialize display pointer - normally done by JNI setframebuffer() */
    display = framebuffer;

    da_generate_crc32table();

    machine = ARCADIA;
    memmap = MEMMAP_ARCADIA;

    if (log_cb)
        log_cb(RETRO_LOG_INFO, "AmiArcadia: retro_init() complete\n");
}

void retro_deinit(void)
{
    game_loaded = false;
    if (rom_data)
    {
        free(rom_data);
        rom_data = NULL;
    }
    rom_size = 0;
}

unsigned retro_api_version(void) { return RETRO_API_VERSION; }

void retro_set_controller_port_device(unsigned port, unsigned device) { }

void retro_get_system_info(struct retro_system_info *info)
{
    memset(info, 0, sizeof(*info));
    info->library_name     = "AmiArcadia";
    info->library_version  = INTEGERVERSION;
    info->need_fullpath    = false;
    info->valid_extensions = "bin|tvc";
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
    switch (machine)
    {
    case ARCADIA:
    case INTERTON:
    case ELEKTOR:
        current_width = BOXWIDTH;
        current_height = (region == REGION_NTSC) ? 226 : BOXHEIGHT;
        break;
    case ZACCARIA:
    case MALZAK:
        current_width = COINOP_BOXWIDTH;
        current_height = COINOP_BOXHEIGHT;
        break;
    default:
        current_width = BOXWIDTH;
        current_height = BOXHEIGHT;
        break;
    }

    info->timing.fps            = (region == REGION_NTSC) ? 60.0 : 50.0;
    info->timing.sample_rate    = AUDIO_SAMPLE_RATE;
    info->geometry.base_width   = current_width;
    info->geometry.base_height  = current_height;
    info->geometry.max_width    = MAXBOXWIDTH;
    info->geometry.max_height   = MAXBOXHEIGHT;
    info->geometry.aspect_ratio = (float)current_width / (float)current_height;
}

void retro_set_environment(retro_environment_t cb)
{
    environ_cb = cb;

    bool no_game = false;
    cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_game);

    struct retro_variable vars[] = {
        { "amiarcadia_machine", "Machine (for unknown ROMs); Arcadia|Interton" },
        { "amiarcadia_region", "Region; PAL|NTSC" },
        { "amiarcadia_demultiplex", "Sprite Demultiplexing (reduce flicker); enabled|disabled" },
        { NULL, NULL },
    };
    cb(RETRO_ENVIRONMENT_SET_VARIABLES, vars);

    /* Register keyboard callback for desktop play */
    struct retro_keyboard_callback kb = { keyboard_cb };
    cb(RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK, &kb);
}

void retro_set_audio_sample(retro_audio_sample_t cb) { audio_cb = cb; }
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) { audio_batch_cb = cb; }
void retro_set_input_poll(retro_input_poll_t cb) { input_poll_cb = cb; }
void retro_set_input_state(retro_input_state_t cb) { input_state_cb = cb; }
void retro_set_video_refresh(retro_video_refresh_t cb) { video_cb = cb; }

static void setup_memory_maps(void);

static void reload_with_machine(int new_machine)
{
    if (!rom_data || rom_size == 0)
        return;

    machine = new_machine;
    option_machine = new_machine;

    IOBuffer = malloc(rom_size);
    if (!IOBuffer)
        return;

    memcpy(IOBuffer, rom_data, rom_size);
    filesize = rom_size;

    int rc = parse_bytes(TRUE);
    free(IOBuffer);
    IOBuffer = NULL;

    LOG_INFO("AmiArcadia: Reloaded with machine=%d, rc=%d, game=%d\n",
             machine, rc, whichgame);

    /* Reinitialize display */
    int x, y;
    for (y = 0; y < MAXBOXHEIGHT; y++)
        for (x = 0; x < MAXBOXWIDTH; x++)
        {
            screen[x][y] = GREY1;
            framebuffer[(y * MAXBOXWIDTH) + x] = pencolours[colourset][GREY1];
        }

    /* Update memory maps for RetroAchievements */
    setup_memory_maps();
}

/*
 * Set up memory maps for RetroAchievements.
 * Matches WinArcadia's RA_InstallMemoryBank() layout exactly.
 */
static void setup_memory_maps(void)
{
    struct retro_memory_descriptor descs[2];
    struct retro_memory_map mmap;

    memset(descs, 0, sizeof(descs));

    switch (machine)
    {
    case ARCADIA:
        /* Arcadia: $1800-$1AFF (768 bytes) */
        descs[0].ptr    = memory;
        descs[0].start  = 0x1800;
        descs[0].len    = 0x300;
        mmap.descriptors = descs;
        mmap.num_descriptors = 1;
        break;

    case INTERTON:
        /* Interton: $1800-$1BFF (1024 bytes) + $1E00-$1FFF (512 bytes) */
        descs[0].ptr    = memory;
        descs[0].start  = 0x1800;
        descs[0].len    = 0x400;
        descs[1].ptr    = memory;
        descs[1].start  = 0x1E00;
        descs[1].len    = 0x200;
        mmap.descriptors = descs;
        mmap.num_descriptors = 2;
        break;

    case ELEKTOR:
        /* Elektor: $0800-$1FFF (6144 bytes) */
        descs[0].ptr    = memory;
        descs[0].start  = 0x0800;
        descs[0].len    = 0x1800;
        mmap.descriptors = descs;
        mmap.num_descriptors = 1;
        break;

    default:
        return;
    }

    environ_cb(RETRO_ENVIRONMENT_SET_MEMORY_MAPS, &mmap);
}

static void check_variables(void)
{
    struct retro_variable var = {0};

    var.key = "amiarcadia_region";
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        if (strcmp(var.value, "NTSC") == 0)
            region = REGION_NTSC;
        else
            region = REGION_PAL;
    }

    var.key = "amiarcadia_machine";
    var.value = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        int new_machine = ARCADIA;
        if (strcmp(var.value, "Interton") == 0)
            new_machine = INTERTON;

        /* Only reload if machine changed AND ROM is unknown */
        if (new_machine != option_machine && whichgame == -1 && game_loaded)
        {
            reload_with_machine(new_machine);
        }
        option_machine = new_machine;
    }

    var.key = "amiarcadia_demultiplex";
    var.value = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
    {
        demultiplex = (strcmp(var.value, "enabled") == 0) ? TRUE : FALSE;
    }
}

bool retro_load_game(const struct retro_game_info *info)
{
    enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;
    if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
    {
        if (log_cb)
            log_cb(RETRO_LOG_ERROR, "AmiArcadia: RGB565 not supported\n");
        return false;
    }

    check_variables();

    if (!info || !info->data || info->size == 0)
    {
        if (log_cb)
            log_cb(RETRO_LOG_ERROR, "AmiArcadia: No ROM data\n");
        return false;
    }

    /* Store ROM data for potential reload if machine option changes */
    rom_size = (int)info->size;
    rom_data = malloc(rom_size);
    if (!rom_data)
    {
        if (log_cb)
            log_cb(RETRO_LOG_ERROR, "AmiArcadia: malloc failed\n");
        return false;
    }
    memcpy(rom_data, info->data, rom_size);

    IOBuffer = malloc(rom_size);
    if (!IOBuffer)
    {
        free(rom_data);
        rom_data = NULL;
        if (log_cb)
            log_cb(RETRO_LOG_ERROR, "AmiArcadia: malloc failed\n");
        return false;
    }

    memcpy(IOBuffer, rom_data, rom_size);
    filesize = rom_size;

    if (info->path)
        strncpy(fn_game, info->path, sizeof(fn_game) - 1);
    else
        strcpy(fn_game, "game.bin");
    fn_game[sizeof(fn_game) - 1] = '\0';

    /* Set machine from option before parsing (for unknown ROMs) */
    machine = option_machine;

    int rc = parse_bytes(TRUE);
    free(IOBuffer);
    IOBuffer = NULL;

    if (rc != 0)
        LOG_WARN("AmiArcadia: parse_bytes returned %d\n", rc);

    /* Initialize display */
    int x, y;
    for (y = 0; y < MAXBOXHEIGHT; y++)
        for (x = 0; x < MAXBOXWIDTH; x++)
        {
            screen[x][y] = GREY1;
            framebuffer[(y * MAXBOXWIDTH) + x] = pencolours[colourset][GREY1];
        }

    for (int i = 0; i < TOTALCHANNELS; i++)
        guestplaying[i] = FALSE;

    /* Set up input descriptors */
    struct retro_input_descriptor desc[] = {
        /* Player 1 */
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "D-Pad Left (Paddle)" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "D-Pad Up (Paddle)" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "D-Pad Down (Paddle)" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "D-Pad Right (Paddle)" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      "Keypad 1" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      "Keypad 2 (Fire)" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      "Keypad 4" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      "Keypad 5" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,      "Keypad 0" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2,     "Keypad Enter" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3,     "Keypad 6" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3,     "Keypad Clear" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,      "Console B (Coin)" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2,     "Console Reset" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Console A" },
        { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  "Console Start" },
        { 0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,  RETRO_DEVICE_ID_ANALOG_X, "Paddle X" },
        { 0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,  RETRO_DEVICE_ID_ANALOG_Y, "Paddle Y" },
        { 0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X, "Keypad [1-9]" },
        { 0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y, "Keypad [1-9]" },
        /* Player 2 */
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "D-Pad Left (Paddle)" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "D-Pad Up (Paddle)" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "D-Pad Down (Paddle)" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "D-Pad Right (Paddle)" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      "Keypad 1" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      "Keypad 2 (Fire)" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      "Keypad 4" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      "Keypad 5" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,      "Keypad 0" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2,     "Keypad Enter" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3,     "Keypad 6" },
        { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3,     "Keypad Clear" },
        { 1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,  RETRO_DEVICE_ID_ANALOG_X, "Paddle X" },
        { 1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,  RETRO_DEVICE_ID_ANALOG_Y, "Paddle Y" },
        { 1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X, "Keypad [1-9]" },
        { 1, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y, "Keypad [1-9]" },
        { 0 },
    };
    environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

    /* Set up memory maps for RetroAchievements */
    setup_memory_maps();

    game_loaded = true;

    LOG_INFO("AmiArcadia: Loaded, machine=%d, game=%d\n", machine, whichgame);

    return true;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
    return false;
}

void retro_unload_game(void)
{
    game_loaded = false;
    if (rom_data)
    {
        free(rom_data);
        rom_data = NULL;
    }
    rom_size = 0;
}

unsigned retro_get_region(void)
{
    return (region == REGION_NTSC) ? RETRO_REGION_NTSC : RETRO_REGION_PAL;
}

void retro_reset(void)
{
    engine_reset();
}

static void update_input(void)
{
    static bool prev_start = false, prev_select = false;

    ULONG p_input[2] = {0, 0};
    int p_x[2] = {128, 128};
    int p_y[2] = {128, 128};

    input_poll_cb();

    /*
     * Keypad mapping (hinput bits 0-11 = keypad keys 1-9, Cl, 0, En):
     *
     * Face buttons:        B=1, A=2, Y=4, X=5
     * Shoulder/stick:      R=0, R2=Enter, R3=6, L3=Clear
     * Right analog stick:  8-directional keypad (keys 1-4, 6-9)
     * Keyboard (P1 only):  0-9, [=Clear, ]=Enter
     */
    for (int p = 0; p < 2; p++)
    {
        /* Face buttons */
        if (input_state_cb(p, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B))  p_input[p] |= (1 << 0);  /* Key 1 */
        if (input_state_cb(p, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A))  p_input[p] |= (1 << 1);  /* Key 2 */
        if (input_state_cb(p, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y))  p_input[p] |= (1 << 3);  /* Key 4 */
        if (input_state_cb(p, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X))  p_input[p] |= (1 << 4);  /* Key 5 */

        /* Shoulder/stick buttons */
        if (input_state_cb(p, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R))  p_input[p] |= (1 << 10); /* Key 0 */
        if (input_state_cb(p, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2)) p_input[p] |= (1 << 11); /* Enter */
        if (input_state_cb(p, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3)) p_input[p] |= (1 << 5);  /* Key 6 */
        if (input_state_cb(p, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3)) p_input[p] |= (1 << 9);  /* Clear */

        /* Right analog stick → 8-directional keypad (keys 1-4, 6-9) */
        int ax = input_state_cb(p, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X);
        int ay = input_state_cb(p, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y);
        if (ax < -ANALOG_DEADZONE || ax > ANALOG_DEADZONE || ay < -ANALOG_DEADZONE || ay > ANALOG_DEADZONE)
        {
            double angle = atan2((double)ay, (double)ax);
            int dir = (int)((angle + PI) / (2.0 * PI) * 8.0 + 0.5) % 8;
            dir = (dir + 5) % 8;  /* Rotate so up-left is 0 */
            p_input[p] |= analog_keypad_map[dir];
        }

        /* D-pad → paddle position (digital: 0/128/255) */
        bool up    = input_state_cb(p, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP);
        bool down  = input_state_cb(p, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN);
        bool left  = input_state_cb(p, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT);
        bool right = input_state_cb(p, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT);

        if (left)       p_x[p] = 0;
        else if (right) p_x[p] = 255;

        if (up)         p_y[p] = 0;
        else if (down)  p_y[p] = 255;

        /* Left analog stick overrides d-pad for paddle */
        ax = input_state_cb(p, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);
        ay = input_state_cb(p, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);
        if (ax < -ANALOG_DEADZONE || ax > ANALOG_DEADZONE || ay < -ANALOG_DEADZONE || ay > ANALOG_DEADZONE)
        {
            p_x[p] = ((ax + 32768) * 255) / 65535;
            p_y[p] = ((ay + 32768) * 255) / 65535;
        }
    }

    /* Console keys - P1 only (active for multiple frames when pressed) */
    bool cur_start = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START);
    bool cur_select = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT);

    if (cur_start && !prev_start)   console_start = 16;
    if (cur_select && !prev_select) console_a = 16;
    if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L))  console_b = 5;
    if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2)) console_reset = 5;

    prev_start = cur_start;
    prev_select = cur_select;

    /* Write to emulator (keyboard input only goes to P1) */
    hinput[0] = p_input[0] | keyboard_input;
    hinput[1] = p_input[1];
    hx[0] = (UBYTE)p_x[0];
    hy[0] = (UBYTE)p_y[0];
    hx[1] = (UBYTE)p_x[1];
    hy[1] = (UBYTE)p_y[1];
}

/*
 * Process pending sound channel requests.
 * Called at end of frame to convert chan_status[] requests into actual audio.
 */
static void process_audio_channels(void)
{
    for (int i = 0; i < TOTALCHANNELS; i++)
    {
        if (sound)
        {
            if (chan_status[i] == CHAN_PLAY)
            {
                if (i >= GUESTCHANNELS)
                    play_any(i, 0, 0);
                else
                    play_any(i, chan_hertz[i], chan_volume[i]);
                chan_status[i] = CHAN_OK;
            }
            else if (chan_status[i] == CHAN_PLAYTHENSTOP)
            {
                if (i >= GUESTCHANNELS)
                    play_any(i, 0, 0);
                else
                    play_any(i, chan_hertz[i], chan_volume[i]);
                chan_status[i] = CHAN_STOP;
            }
            else if (chan_status[i] == CHAN_STOP)
            {
                guestplaying[i] = FALSE;
                chan_status[i] = CHAN_OK;
            }
        }
    }
}

void retro_run(void)
{
    bool updated = false;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
        check_variables();

    update_input();

    /* Run one frame */
    switch (machine)
    {
    case ARCADIA:
        uvi();
        break;
    case INTERTON:
    case ELEKTOR:
        newpvi();
        break;
    case ZACCARIA:
    case MALZAK:
        oldpvi();
        break;
    }

    frames++;

    /* Process pending sound channel requests (generates audio into SoundBuffer) */
    process_audio_channels();

    video_cb(framebuffer, current_width, current_height, MAXBOXWIDTH * sizeof(UWORD));

    /* Audio - mix all active channels */
    int samples_per_frame = (region == REGION_NTSC) ? SAMPLES_PER_FRAME_NTSC : SAMPLES_PER_FRAME_PAL;

    if (sound)
    {
        /* Clear buffer */
        memset(audio_buffer, 0, samples_per_frame * 2 * sizeof(int16_t));

        /* Mix all active channels */
        for (int ch = 0; ch < TOTALCHANNELS; ch++)
        {
            if (!guestplaying[ch])
                continue;

            int len = SoundLength[ch];
            if (len <= 0)
                continue;

            /* Mix this channel into the buffer (stereo) */
            for (int i = 0; i < samples_per_frame; i++)
            {
                /* Loop the channel's sound buffer */
                int src_idx = i % len;
                int32_t sample = SoundBuffer[ch][src_idx];

                /* Mix into stereo buffer (same on both channels) */
                int32_t left = audio_buffer[i * 2] + sample;
                int32_t right = audio_buffer[i * 2 + 1] + sample;

                /* Clamp to int16 range */
                if (left > 32767) left = 32767;
                if (left < -32768) left = -32768;
                if (right > 32767) right = 32767;
                if (right < -32768) right = -32768;

                audio_buffer[i * 2] = (int16_t)left;
                audio_buffer[i * 2 + 1] = (int16_t)right;
            }
        }
    }
    else
    {
        memset(audio_buffer, 0, samples_per_frame * 2 * sizeof(int16_t));
    }

    audio_batch_cb(audio_buffer, samples_per_frame);
}

/*
 * Save state size - 36KB covers all machine types with margin.
 * Based on DroidArcadia's buffer allocation in csavecos().
 */
#define SAVESTATE_SIZE (36 * KILOBYTE)

size_t retro_serialize_size(void)
{
    return SAVESTATE_SIZE;
}

bool retro_serialize(void *data, size_t size)
{
    if (!game_loaded || size < SAVESTATE_SIZE)
        return false;

    IOBuffer = malloc(SAVESTATE_SIZE);
    if (!IOBuffer)
        return false;

    serializemode = SERIALIZE_WRITE;
    serialize_cos();

    /* Copy the actual used portion */
    memcpy(data, IOBuffer, offset);

    free(IOBuffer);
    IOBuffer = NULL;

    return true;
}

bool retro_unserialize(const void *data, size_t size)
{
    const UBYTE *src = (const UBYTE *)data;

    if (!game_loaded || size < 4)
        return false;

    /* Validate COS header magic byte */
    if (src[0] != 0xBF)
    {
        LOG_WARN("AmiArcadia: Invalid save state magic (got 0x%02X)\n", src[0]);
        return false;
    }

    /* Verify memmap matches (src[3] is the saved memmap) */
    if (src[3] != memmap)
    {
        LOG_WARN("AmiArcadia: Save state memmap mismatch (%d vs %d)\n", src[3], memmap);
        return false;
    }

    IOBuffer = malloc(size);
    if (!IOBuffer)
        return false;

    memcpy(IOBuffer, data, size);
    filesize = (int)size;
    serializemode = SERIALIZE_READ;
    serialize_cos();

    free(IOBuffer);
    IOBuffer = NULL;

    return true;
}
/*
 * RetroAchievements memory exposure - matches WinArcadia's RA_InstallMemoryBank() layout:
 *
 * Arcadia:  $1800-$1AFF (768 bytes = 0x300) - single bank
 * Interton: $1800-$1BFF (1024 bytes = 0x400) via mirror_r + $1E00-$1FFF (512 bytes = 0x200)
 *           Note: Interton uses address mirroring which we can't fully replicate with a
 *           simple pointer, so we expose $1800-$1BFF directly (works for most cases)
 * Elektor:  $0800-$1FFF (6144 bytes = 0x1800)
 */
void *retro_get_memory_data(unsigned id)
{
    if (id != RETRO_MEMORY_SYSTEM_RAM)
        return NULL;

    switch (machine)
    {
    case ARCADIA:
        return &memory[0x1800];
    case INTERTON:
        return &memory[0x1800];
    case ELEKTOR:
        return &memory[0x0800];
    default:
        return NULL;
    }
}

size_t retro_get_memory_size(unsigned id)
{
    if (id != RETRO_MEMORY_SYSTEM_RAM)
        return 0;

    switch (machine)
    {
    case ARCADIA:
        return 0x300;  /* $1800-$1AFF */
    case INTERTON:
        return 0x400;  /* $1800-$1BFF (primary bank only) */
    case ELEKTOR:
        return 0x1800; /* $0800-$1FFF */
    default:
        return 0;
    }
}
void retro_cheat_reset(void) { }
void retro_cheat_set(unsigned index, bool enabled, const char *code) { }
