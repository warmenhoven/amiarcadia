#define DECIMALVERSION       "36.1"
#define INTEGERVERSION       "36.10"
#define MAJORVERSION         0x36
#define MINORVERSION         0x10
#define VERSIONSTRING        "\0$VER: AmiArcadia " INTEGERVERSION " (29.3.2026)" // d.m.yyyy format
#define RELEASEDATE          "29-03-26" // dd-mm-yy format. Year *must* be only 2 digits!
#define LONGDATE             "29 March 2026" // full month and year
#define COPYRIGHT            "� 2006-2026 James Jacobs of Amigan Software"
// VERSIONSTRING needs a leading NUL for OS4 to correctly find it
#define NEWCONFIGVERSION     "36.1" // V36.1+
#define OLDCONFIGVERSION     "36.0" // V36.0 beta-36.04
#define VERYOLDCONFIGVERSION "35.9" // V35.9
#define RELEASING
// comment this out during development

// #define USEMV
// whether to use millivolts instead of milligauss for magnetic X/Y/Z in
// vector magnetometer subwindow

#define SCALEVOLUME
/* whether to reduce volume of recorded WAVs, etc. according to host volume:
wa:
 if SCALEVOLUME,\
  always plays at full waveOutSetVolume()
  plays and generates quiet sounds by scaling waveform to host volume
 otherwise,
  uses waveOutSetVolume() on played sound to apply host volume
  always generates loud WAVs regardless of host volume
aa:
 if SCALEVOLUME,
  uses Paula/AHI volume controls on played sound to apply host volume
  generates quiet sounds by scaling waveform to host volume
 otherwise,
  uses Paula/AHI volume controls on played sound to apply host volume
  always generates loud WAVs regardless of host volume
*/

#define BLACKBORDER
// whether to colour unused areas (ie. in full-screen mode without
// "stretch to fit") as black (otherwise grey)
#ifdef BLACKBORDER
    #define BORDERPEN BLACK
#else
    #define BORDERPEN GREY1
#endif

// #define THOLIN
// whether to use Tholin's instruction categories instead of the Elektor disassembler ones

// #define VERBOSE
// enable this for diagnostic output re. non-sound

// #define DEBUGSOUND
// enable this for diagnostic output re. sound

// #define DEBUGSMUS
// whether to log SMUS events to the output window

// #define LOGNETWORK
// enable this to log netplay bytes sent and received

// #define CHECKBLOCKS
// whether to support checking a game against a list of stored BCCs, via Ctrl+Shift+D
// (WinArcadia only)

// #define SHOWRUMBLE
// whether to visibly show rumble by flashing the side of the screen

// #define BASICMODE
// whether to prefer memmap "E" for games which are compatible with either memmap
// but have extra features on a deluxe TVGC (otherwise prefers memmap "F").

// #define DEBUGPSU
// whether to log writes to the PSU by game code

#define EMULATE_CPU
#define EMULATE_GFX
#define EMULATE_XVI
// whether to disable parts of the emulation for benchmarking purposes
// (only currently implemented for WinArcadia (not AmiArcadia, WinInterton, etc.))

#define SETPATHS
// whether non-game load/saves via ASL should set path_foo accordingly

#define LOWERCAPS
// whether to show "caps" on status bar when Caps Lock is off (otherwise blank)

#define VERBOSEDISKREADS
#define VERBOSEDISKWRITES
#define VERBOSEDISKCONTROL
// if these are not defined, you won't get the corresponding output

// #define GAMER
// whether to build an experimental "gamer edition", omitting tracing,
// tape support, etc. for increased speed (not yet fully implemented)

// #define QUICK
// whether to ignore some page wrapping issues

// #define TRUECOLOUR
// whether to save PNG/APNG/MNG files as 24-bit

// #define MNGPLAY
// OS4 MNGPlay ("2001-07-08" version) has a bug (expects BGR instead of RGB format).
// So MNGPLAY is a workaround for the bug.
// Also you must run MNGPlay with the undocumented 24 switch, eg.: MNGPlay foo.mng 24

// #define BENCHMARK_GFX
// to just test speed of graphics operations
// #define BENCHMARK_EMU
// to just test speed of CPU+XVI emulation
// #define BENCHMARK_CPU
// to just test speed of CPU emulation

// #define OPCOLOURS
// (Arcadia/Interton/Elektor only)
// whether to show on-screen what instructions are being executed and when.

#define WIDEINSTRUCTOR
// whether Instructor 50 display should be widened and italicized
// (like the real machine)

#define BIGTYPERIGHT
// whether to double the size of the guest screen (to 128*28) (otherwise 64*14)
#ifdef BIGTYPERIGHT
    #define TR_YY 2
#else
    #define TR_YY 1
#endif

#define REPEATRATE 10 // keys repeat every 10 frames (ie. 5 times per second for PAL)
// for WinArcadia, at least, it is also limited by the host key repeat settings (in Windows Control Panel)

// #define MALZAK_885KHZ (too glitchy)
#define MALZAK_1180KHZ
// define one (only!) of these (for master PAL clock divided by 4 or 3 respectively)

// #define SHOWCHARSET
// if you want to examine the character generator PROM contents
// (BINBUG, CD2650, PHUNSY, ZACCARIA only)

// #define TOURNAMENTHISCORES
// whether high scores should be cleared to Basque Tournament records (otherwise to zero)

// #define ADVENTURETRAINER
// whether to enable unlimited time (oil) and level skip (teleport) cheats for (BINBUG) Microbyte Adventure

// #define KEYCLICKS
// whether to make a click for every keypress (currently only implemented for PIPBUG and BINBUG)

#define PAPERTAPESPROCKETS
// whether to show the sprocket column on the papertape

#define PAPERTAPEMAX (32 * KILOBYTE)

#define RESETTIMEOUT timeoutat = frames + ((region == REGION_PAL) ? 20 : 24)
// 400 ms (20/50ths or 24/60ths of a sec)

// Defined by AmigaOS are: IMPORT, UBYTE, UWORD, SWORD, ULONG, LONG, TEXT, STRPTR.
#define TRANSIENT              auto   /* automatic variables */
#define MODULE                 static /* external static (file-scope) */
#define PERSIST                static /* internal static (function-scope) */
#define FAST                   static /* internal static (function-scope) which could be autos if we didn't care about speed */
#define DISCARD                (void) /* discarded return values */
#define elif                   else if
#define acase                  break; case
#define adefault               break; default
#define EXPORT
#define ROUNDTO4(x)            ((((x) +  3) >> 2) << 2)
#define ROUNDTO16(x)           ((((x) + 15) >> 4) << 4)
#define EOS                    '\0' // end of string (ie. ASCII NUL)
#define ENDOF(x)               &x[strlen((const char*) x)]
#ifdef AMIGA
    #define HWND               struct Window*
#endif

#ifndef __amigaos4__
    #define REG(x)     register __ ## x
    #define UNUSED
    #ifdef __MORPHOS__
        #define ASM
        #define SAVEDS
    #else
        #define ASM    __asm
    #endif
#endif

// #define EXIT_SUCCESS  0
#define EXIT_WARNING     5
#define EXIT_ERROR      10
// #define EXIT_FAILURE 20

#define CODE           'C'
#define DATA           'D'
#define LOWPTR         'D' // low  (less significant) byte of a pointer
#define HIGHPTR        'P' // high (more significant) byte of a pointer
#define POINTER        'P'
#define UNKNOWN        'U'
#define STRING         DATA

#define WATCH_NONE 0
#define WATCH_SOME 1 // only watch writes that change the contents
#define WATCH_ALL  2 // watch all writes

#ifdef AMIGA
#define MAXSAMPLERATE  11025
#endif
#ifdef WIN32
#define MAXSAMPLERATE  44100
#endif
#define SOUNDLENGTH    (MAXSAMPLERATE * 2) // enough for two seconds (we only use just over one second though)
#define SILENCEBUFSIZE 44100 // this could be set to any length really

// These only affect tape recording. Always define one (only!) of them.
// #define SAVE_HZ_11025
// #define SAVE_HZ_22050
#define SAVE_HZ_44100

#ifdef SAVE_HZ_11025
   #define SAVE_HZ      11025
   #define SAVE_HZ_SANE 0x0C
#endif
#ifdef SAVE_HZ_22050
   #define SAVE_HZ      22050
   #define SAVE_HZ_SANE 0x0D
#endif
#ifdef SAVE_HZ_44100
   #define SAVE_HZ      44100
   #define SAVE_HZ_SANE 0x0E
#endif

#define CHAN_OK               0
#define CHAN_PLAY             1
#define CHAN_STOP             2
#define CHAN_PLAYTHENSTOP     3

#define ARCADIAGLYPHS        59 // for sidebar

#define MAX_ADDR          32767
#define ALLREDEFINEGADS      60
#define CPL_MIN             192
#define INVALIDRASTER     (-99)
#define QUEUESIZE         (MAX_VDUROWS * MAX_VDUCOLUMNS * 2)

#define MAX_STATESIZE      (667 * KILOBYTE)
// TWIN   uses up to 667 K ( 48K of RAM + 2*308K floppy disks + 3 K for header, CPU state, etc.)
// PHUNSY uses up to 301?K (300K of RAM                       + 1?K for header, CPU state, etc.)

#define PRINTERX        (5+2)
#define EUYPRINTERX        2
#define EUYPRINTERY     (7+5)
#define EUYCOLUMNS        32
#define EUYROWS           32 // arbitrary (paper roll is 60m long continuous)
#define EUYWIDTH        (2 + (   PRINTERX * EUYCOLUMNS) + 2) // 2+224+2=228
#define EUYHEIGHT       (     EUYPRINTERY * EUYROWS        ) //         384
#define EUY_PAPER      GREY1

#define C306XMARGIN       29 // per side
#define C306PRINTERX       4
#define C306PRINTERY    (7+4)
#define C306ROWS         117
#define C306HEIGHT_FULL 1287
#define C306HEIGHT_VIEW  462 // 4    " (or  7    ")
#define C306COLUMNS      132 // 8    "+0.5"=8.5  ". 924+58=982 pixels total
#define C306WIDTH_FULL     ((C306COLUMNS * PRINTERX) + (C306XMARGIN * 2))
#ifdef WIN32
    #define C306WIDTH_VIEW C306WIDTH_FULL
#endif
#ifdef AMIGA
    #define C306WIDTH_VIEW 512
#endif
#define PRINTERWIDTH_FULL   C306WIDTH_FULL   // widest
#define PRINTERWIDTH_VIEW   C306WIDTH_VIEW   // widest
#define PRINTERHEIGHT_FULL  C306HEIGHT_FULL  // tallest
#define PRINTERHEIGHT_VIEW  C306HEIGHT_VIEW  // tallest
#define PRINTERROWS         C306ROWS         // widest
#define PRINTERCOLUMNS      C306COLUMNS      // widest
#define C306C_PAPER         WHITE

/* Our EUY   scale is  37.6' pixels per cm   (about 95-96 pixels per inch)
   Our C306C scale is 115.5  pixels per inch horizontally
                  and   ?    pixels per inch vertically

C306C:
 Margin of 1/4" ( 28.875 pixels) on each side, * 2 =  57.75 pixels (1/2")
             8" (924     pixels) for the rest      = 981.75 pixels wide
 4"   would be 462    pixels tall and provide 42    rows.
 5.5" would be 635.25 pixels tall and provide 57.75 rows.
 6.2" would be 726    pixels tall and provide 66    rows.
 8"   would be 924    pixels tall and provide 84    rows.
 11"  would be 1270.5 pixels tall and provide 115.5 rows.
         We do 1287   pixels tall to  provide 117   rows. */

// for BINBUG
#define    BINBUG_USERBLOCKSIZE 254 // to exclude block headers when showing byte sizes to user
// #define BINBUG_USERBLOCKSIZE 256 // to include block headers when showing byte sizes to user

// #define BINBUG_TRACKS  37
#define BINBUG_TRACKS     40

#define BINBUG_SECTORS     10
#define BINBUG_BLOCKSIZE  256
#define BINBUG_DISKBLOCKS (BINBUG_TRACKS     * BINBUG_SECTORS  ) //    40 *  10 =     400
#define BINBUG_DISKSIZE   (BINBUG_DISKBLOCKS * BINBUG_BLOCKSIZE) //   400 * 256 = 102,400 = 100   K
#define BINBUG_TRACKSIZE  (BINBUG_SECTORS    * BINBUG_BLOCKSIZE) //    10 * 256 =   2,560 =   2.5 K
#define DISKSIZE_USER     (BINBUG_DISKBLOCKS * BINBUG_USERBLOCKSIZE)

#define CD2650_TRACKS      35
#define CD2650_SECTORS      9
#define CD2650_BLOCKSIZE  256
#define CD2650_DISKBLOCKS (CD2650_TRACKS     * CD2650_SECTORS  ) //    35 *   9 =     315
#define CD2650_TRACKSIZE  (CD2650_SECTORS    * CD2650_BLOCKSIZE) //     9 * 256 =   2,304 =   2.25K
#define CD2650_DISKSIZE   (CD2650_DISKBLOCKS * CD2650_BLOCKSIZE) //   315 * 256 =  80,640 =  78.75K

#define TWIN_TRACKS        77
#define TWIN_SECTORS       32 // per track
#define TWIN_BLOCKSIZE    128
#define TWIN_DISKBLOCKS   (TWIN_TRACKS       * TWIN_SECTORS    ) //    77 *  32 =   2,464
#define TWIN_TRACKSIZE    (TWIN_SECTORS      * TWIN_BLOCKSIZE  ) //    32 * 128 =   4,096 =   4   K
#define TWIN_DISKSIZE     (TWIN_DISKBLOCKS   * TWIN_BLOCKSIZE  ) // 2,464 * 128 = 315,392 = 308   K

#define CLUSTERS_MAX      BINBUG_DISKBLOCKS // 400 (vs. 304 for TWIN)
#define DRIVES_MAX        4

#define BAM_LOST          0
#define BAM_DIR           1
#define BAM_FILE          2
#define BAM_FREE          3
#define DRIVEMODE_IDLE    0 // must be 0!
#define DRIVEMODE_READING 1
#define DRIVEMODE_WRITING 2

#define PERIPH_PRINTER      0 // EA printer
#define PERIPH_LINEARISATIE 1
#define PERIPH_MAGNETOMETER 2
#define PERIPH_FURNACE      3

#define DIGITLEDS          13 // PIPBUG is 4+3+3+3

// for Pong
#define TONE_BOUNCE     488                    // B4
#define TONE_BAT        976                    // B5
#define TONE_SCORE      (retune ? 1952 : 1950) // B6

#define PONGXMARGIN       1 // must be >= 1 (because of invisible collision pixels at columns 26 and 100)! In double-width "pong pixels"
#define PONGYMARGIN       5 // must be >= 0!

#define ROBOTSPEED_INFINITE 4 // 0=off, 1..3=that many per frame, 4=infinite

// for Type-right
#define LOWBEEP        1800 // Hz                 A6 or A#6
#define HIGHBEEP       2350 // Hz                 D7

#define FIRSTGUESTTONE    0
#define TONE_1STXVI       0 // 1+ PVI machines, Arcadia, PIPBUG, CD2650, PHUNSY, Pong, Type-right
#define TONE_2NDXVI       1 // 2+ PVI machines (Galaxia, Laser Battle, Lazarian, Malzak), machine bell
#define TONE_3RDXVI       2 // 3+ PVI machines (Galaxia, Laser Battle, Lazarian), printer bell
#define TONE_1STLB1       3 // Laser Battle/Lazarian
#define TONE_A1           3 // Expanded Elektor (and Galaxia)
#define TONE_1STLB2       4 // Laser Battle/Lazarian
#define TONE_B1           4 // Expanded Elektor (and Galaxia)
#define TONE_2NDLB1       5 // Laser Battle/Lazarian
#define TONE_C1           5 // Expanded Elektor
#define TONE_2NDLB2       6 // Laser Battle/Lazarian
#define TONE_A2           6 // Expanded Elektor
#define TONE_B2           7 // Expanded Elektor
#define TONE_C2           8 // Expanded Elektor
#define LASTGUESTTONE     8
#define TONECHANNELS     (LASTGUESTTONE - FIRSTGUESTTONE + 1)
#define FIRSTGUESTNOISE   9
#define NOISE_1STXVI      9 // Arcadia, Interton, expanded Elektor
#define NOISE_A1         10 // Expanded Elektor
#define NOISE_B1         11 // Expanded Elektor
#define NOISE_C1         12 // Expanded Elektor
#define NOISE_A2         13 // Expanded Elektor
#define NOISE_B2         14 // Expanded Elektor
#define NOISE_C2         15 // Expanded Elektor
#define LASTGUESTNOISE   15
#define NOISECHANNELS    (LASTGUESTNOISE - FIRSTGUESTNOISE + 1)
#define GUESTCHANNELS    16
#define SAMPLE_EXPLOSION  0 // 16 Interton, expanded Elektor
#define SAMPLE_SHOOT      1 // 17 Astro Wars, Galaxia
#define SAMPLE_BANG       2 // 18 Astro Wars, Galaxia
#define SAMPLE_COIN       3 // 19 coin-ops
#define SAMPLE_PRINTER    4 // 20 PIPBUG, BINBUG, TWIN, CD2650
#define SAMPLE_CLICK      5 // 21 Interton, expanded Elektor
#define SAMPLE_SPIN       6 // 22 BINBUG, TWIN, CD2650
#define SAMPLE_STEP       7 // 23 BINBUG, TWIN, CD2650
#define SAMPLE_GRIND      8 // 24 BINBUG, TWIN, CD2650
#define SAMPLE_PUNCH      9 // 25 PIPBUG, BINBUG, TWIN
#define SAMPLES          10 //    counting from 1
#define TOTALCHANNELS    (GUESTCHANNELS + SAMPLES)

#ifdef AMIGA // it's already built-in on IBM-PC
    #define MAX_PATH    512 // OS3 supports about 255 chars for each of path and filename
#endif

#ifndef INVALID_SOCKET
    #define INVALID_SOCKET (-1)
#endif

#define HISTORYLINES            20
#define PROJECTSIZE           (((MAX_VDUCOLUMNS + 2) * MAX_VDUROWS) + 15 + 1) // for PIPBUG
// max. size of PHUNSY screen clipboard text is ((64+2)*32)+1 plus (6+1+2) for LEDs
// max. size of printer output is PRINTERROWS * (PRINTERCOLUMNS + 3)
#define SPRITENAMELENGTH      (40 + 1)
#define FRIENDLYLENGTH        (LABELLIMIT + 8 + 1) // enough for "$1234 (FOO)"

#define VARIANT_8550_TENNIS      0
#define VARIANT_8550_SOCCER      1 // aka Hockey
#define VARIANT_8550_HANDICAP    2
#define VARIANT_8550_SQUASH      3 // aka Handball
#define VARIANT_8550_PRACTICE    4
#define VARIANT_8550_RIFLE1      5
#define VARIANT_8550_RIFLE2      6
#define VARIANT_8600_TENNIS      7
#define VARIANT_8600_HOCKEY      8
#define VARIANT_8600_SOCCER      9
#define VARIANT_8600_SQUASH     10
#define VARIANT_8600_PRACTICE   11
#define VARIANT_8600_GRIDBALL   12
#define VARIANT_8600_BASKETBALL 13
#define VARIANT_8600_BBPRACTICE 14
#define VARIANT_8600_TARGET1    15
#define VARIANT_8600_TARGET2    16
#define VARIANTS                17

#define SERVING_AUTO          0
#define SERVING_MANUAL        1

#define PONGMACHINE_1976      0
#define PONGMACHINE_1977      1
#define PONGMACHINE_8550      2

#define KILOBYTE           1024
#define ONE_MILLION     1000000
#define ONE_BILLION  1000000000

#define SI50_MINIWIDTH_SML   72
#define SI50_MINIHEIGHT_SML   9
#define SI50_MINIWIDTH_BIG  144
#define SI50_MINIHEIGHT_BIG  18
#define MIKIT_MINIWIDTH     108 // 9*12
#define MIKIT_MINIHEIGHT     18
#define SELBST_MINIWIDTH    108 // 9*12
#define SELBST_MINIHEIGHT    18
#define TYPERIGHT_MINIWIDTH  64
#define TYPERIGHT_MINIHEIGHT 14
#define PHUNSY_MINIWIDTH    112
#define PHUNSY_MINIHEIGHT    18
#define SPLITDISTANCE        18
#define MAGNIFIERSCALE        4

#define MINIWIDTH           144 // SI50_MINIWIDTH_BIG. Must be a multiple of 4
#define MINIHEIGHT           18 // SI50_MINIHEIGHT, MIKIT_MINIHEIGHT
#define AXESWIDTH            96
#define AXESHEIGHT           96
#define INDUSTRIALWIDTH     192
#define INDUSTRIALHEIGHT    192
#define MAGNIFIERWIDTH      192
#define MAGNIFIERHEIGHT     192
#define PREVIEWWIDTH         24 // must be a multiple of 4
#define PREVIEWHEIGHT        40
#define MEMMAPWIDTH         128
#define MEMMAPHEIGHT        256
#define WAVEFORMWIDTH       320
#define WAVEFORMHEIGHT      128
#define BLOXWIDTH       (77 * 4) // enough for TWIN tracks
#define BLOXHEIGHT      (32 * 4) // enough for TWIN sectors
#define SPLITWIDTH           12
#define SPLITHEIGHT          18
#define CANVASWIDTH           PRINTERWIDTH_VIEW // width of widest canvas
#define CANVASHEIGHT          PRINTERHEIGHT_VIEW // height of tallest canvas

#define CANVAS_AXES         0
#define CANVAS_INDUSTRIAL   1
#define CANVAS_MAGNIFIER    2
#define CANVAS_MEMMAP       3
#define CANVAS_MINI         4
#define CANVAS_PREVIEW      5
#define CANVAS_PRINTER      6
#define CANVAS_ROLL1        7
#define CANVAS_ROLL2        8
#define CANVAS_WAVE         9
#ifdef WIN32
    #define CANVASES       10
#endif
#ifdef AMIGA
    #define CANVAS_BLOCKS1 10
    #define CANVAS_BLOCKS2 11
    #define CANVAS_SPLIT1  12
    #define CANVAS_SPLIT2  13
    #define CANVAS_SPLIT3  14
    #define CANVAS_SPLIT4  15
    #define CANVAS_SPLIT5  16
    #define CANVAS_SPLIT6  17
    #define CANVASES       18
#endif

#ifdef PAPERTAPESPROCKETS
    #define ROLLWIDTH       228
#else
    #define ROLLWIDTH       208
#endif
#define ROLLHEIGHT          180

#define PLANEPITCH_MAXDEG    20 // ie. range is -20..+20 degrees
#define PLANEPITCH_MAX       56 // each unit represents 0.35156 degrees
#define PLANEROLL_MAXDEG     44 // ie. range -44..+44 degrees
#define PLANEROLL_MAX       125 // each unit represents 0.35156 degrees

#define CONFIGMACHINE_AMIGA   0
#define CONFIGMACHINE_IBM     1

#define ISQWERTY \
(    machine == PIPBUG \
 ||  machine == BINBUG \
 ||  machine == TWIN \
 ||  machine == CD2650 \
 || (machine == PHUNSY && phunsy_biosver == PHUNSY_PHUNSY) \
 || (machine == SELBST && selbst_biosver == SELBST_BIOS20) \
 ||  machine == TYPERIGHT \
)

#define TITLEBARTEXTLENGTH  256

#define SPRITEMODE_INVISIBLE  0
#define SPRITEMODE_NUMBERED   1
#define SPRITEMODE_VISIBLE    2

#define AMBERSCALE            0
#define GREENSCALE            1
#define GREYSCALE             2
#define PURECOLOURS           3
#define PVICOLOURS            4
#define UVICOLOURS            5
#define COLOURSETS            6 // 0..5

#define VDU_ELEKTERMINAL      0 // PIPBUG
#define VDU_LCVDU_NARROW      1 // PIPBUG
#define VDU_LCVDU_WIDE        2 // PIPBUG
#define VDU_RADIOBULLETIN     3 // PIPBUG
#define VDU_SVT100            4 // PIPBUG
#define VDU_VT100             5 // PIPBUG
#define VDU_ANY               6 // PIPBUG

#define VDU_ASCII             0 // CD2650
#define VDU_CHESSMEN          1 // CD2650
#define VDU_LOWERCASE         2 // CD2650

#define ANGLES_2              0
#define ANGLES_4              1
#define ANGLES_RANDOM         2

#define PLAYERS_2             0
#define PLAYERS_3LT           1
#define PLAYERS_3RT           2
#define PLAYERS_4             3

#define PONGSPEED_SLOW        0
#define PONGSPEED_FAST        1
#define PONGSPEED_RANDOM      2

#define EA77CC4               0
#define ACOS                  1

#define SPEED_1QUARTER   0
#define SPEED_2QUARTERS  1
#define SPEED_3QUARTERS  2
#define SPEED_4QUARTERS  3
#define SPEED_5QUARTERS  4
#define SPEED_6QUARTERS  5
#define SPEED_8QUARTERS  6
#define SPEED_16QUARTERS 7
#define SPEED_32QUARTERS 8
#define SPEED_MIN        SPEED_1QUARTER
#define SPEED_MAX        SPEED_32QUARTERS

#define KIND_ILBM               0
#define KIND_ACBM               1
#define KIND_BMP                2
#define KIND_PCX                3
#define KIND_GIF                4
#define KIND_TIFF               5
#define KIND_PNG                6
// screenshot kinds must be first, savescreenshot() depends on it
#define KIND_BIN                7
#define KIND_COS                8
#define KIND_COR                9
#define KIND_IFFANIM           10
#define KIND_ZIP               11
#define KIND_AOF               12
#define KIND_EOF               13
#define KIND_HEX               14
#define KIND_WAV               15
#define KIND_PSG               16
#define KIND_YM                17
#define KIND_ASM               18
#define KIND_8SVX              19
#define KIND_AIFF              20
#define KIND_MNG               21
#define KIND_MIDI              22
#define KIND_SMUS              23
#define KIND_ASCII             24
#define KIND_TVC               25
#define KIND_MDCR              26
#define KIND_RAW               27
#define KIND_CMD               28
#define KIND_IMG               29
#define KIND_TWIN              30
#define KIND_MOD               31
#define KIND_SYM               32
#define KIND_IMAG              33
#define KIND_ICON              34
#define KIND_16SV              35
#define KIND_BPNF              36
#define KIND_SMS               37
#define KIND_PAP               38
#define KINDS                  39 // counting from 1
#define KIND_NIL               (KINDS + 1)

#define ELEKTOR_PHILIPS         0
#define ELEKTOR_HOBBYMODULE     1

#define TWIN_EXOS               0
#define TWIN_SDOS20             1
#define TWIN_SDOS40             2
#define TWIN_SDOS42             3
#define TWIN_TOS                4
#define TWIN_UDOS               5
#define TWIN_NODOS              6

#define PIPBUG_ARTEMIS          0
#define PIPBUG_HYBUG            1
#define PIPBUG_PIPBUG1BIOS      2
#define PIPBUG_PIPBUG2BIOS      3

#define CD2650_IPL              0
#define CD2650_POPMON           1
#define CD2650_SUPERVISOR       2

#define PHUNSY_MINIMONITOR      0
#define PHUNSY_PHUNSY           1

#define SELBST_BIOS00           0
#define SELBST_BIOS09           1
#define SELBST_BIOS10           2
#define SELBST_BIOS20           3

#define REGION_NTSC             0
#define REGION_PAL              1
#define REGION_ANY              2

#define KEYMAP_HOST             0
#define KEYMAP_EA               1
#define KEYMAP_TT               2

#define SERIALIZE_READ          0
#define SERIALIZE_WRITE         1

#define TIMEUNIT_CYCLES         0
#define TIMEUNIT_CLOCKS         1
#define TIMEUNIT_PIXELS         2

// coverage[] bitfield

#define COVERAGE_WRITE            0x1
#define COVERAGE_READ             0x2
#define COVERAGE_OPERAND          0x4
#define COVERAGE_OPCODE           0x8
#define COVERAGE_ADDRESS         0x10
// view_coverage() depends on coverage bits being bits 0..4 and in this order!
#define COVERAGE_BITS        (COVERAGE_ADDRESS | COVERAGE_OPERAND | COVERAGE_OPCODE | COVERAGE_READ | COVERAGE_WRITE)

#define COVERAGE_JUMPSINT       0x100
#define COVERAGE_CALLSINT       0x200
#define COVERAGE_JUMPS      0x1000000
#define COVERAGE_CALLS      0x2000000
#define ROUTINE_BITS       (COVERAGE_JUMPS     | COVERAGE_CALLS | COVERAGE_JUMPSINT | COVERAGE_CALLSINT)

#define COVERAGE_DEPTH     0x1C000000
#define COVERAGE_BITSHIFT          26 // %...###.......................... ->
                                      // %.............................###
#define COVERAGE_LOOPSTART 0x20000000
#define COVERAGE_LOOPEND   0x40000000
#define LOOP_BITS          (COVERAGE_LOOPSTART | COVERAGE_LOOPEND)

// memflags[] bitfield
typedef UWORD MEMFLAG;
#define ASIC                  0x1
#define NOREAD                0x2
#define NOWRITE               0x4
#define READONCE              0x8
#define WATCHPOINT           0x10 // must be <= 255
#define COMMENTED            0x20
#define BIOS                 0x40
#define BREAKPOINT           0x80
#define AUDIBLE             0x100
#define STEPPOINT           0x200
#define VISIBLE             0x400
#define TRAINABLE           0x800
#define BANKU              0x1000
#define BANKQ              0x2000
#define BIOSRAM            0x4000
#define BANKED             0x8000
#define SPECIALREAD       (WATCHPOINT | ASIC | BANKED | NOREAD  | READONCE)
#define SPECIALWRITE      (WATCHPOINT | ASIC | BANKED | NOWRITE | AUDIBLE)

#define NONPAGE            0x1FFF // mask page offset
#define PLEN               0x2000 // page length
#define PAGE               0x6000 // mask page
#define AMSK               0x7FFF // mask address range
#ifdef QUICK
    #define WRAPMEM(x)         (iar + x)
#else
    #define WRAPMEM(x)         ((iar & PAGE) + ((iar + x) & NONPAGE))
#endif
#define OPERAND            (memory[WRAPMEM(1)])
#define GET_RR             rr = (opcode & 3); if (rr && (psl & PSL_RS)) rr += 3;
#define BRANCHCODE         (opcode & 3)

#define TYPE_LOG                0
#define TYPE_BP                 1
#define TYPE_RUNTO              2

#define SUBWINDOW_CONTROLS      0
#define SUBWINDOW_DIPS          1
#define SUBWINDOW_GAMEINFO      2
#define SUBWINDOW_MEMORY        3
#define SUBWINDOW_MONITOR_CPU   4
#define SUBWINDOW_OPCODES       5
#define SUBWINDOW_PALETTE       6
#define SUBWINDOW_TAPEDECK      7
#define SUBWINDOW_SPRITES       8
#define SUBWINDOW_SPEED         9
#define SUBWINDOW_HOSTKYBD     10
#define SUBWINDOW_HOSTPADS     11
#define SUBWINDOW_MONITOR_XVI  12
#define SUBWINDOW_MONITOR_PSGS 13
#define SUBWINDOW_MUSIC        14
#define SUBWINDOW_OUTPUT       15
#define SUBWINDOW_PRINTER      16
#define SUBWINDOW_FLOPPYDRIVE  17
#define SUBWINDOW_PAPERTAPE    18
#define SUBWINDOW_INDUSTRIAL   19
#define SUBWINDOWS             20 // counting from 1

#define NET_OFF                 0
#define NET_CLIENT              1
#define NET_SERVER              2
#define NET_LISTENING           3

#define LOGFILE_APPEND          0
#define LOGFILE_IGNORE          1
#define LOGFILE_REPLACE         2

#define PIPBUG_BAUDRATE_110     0
#define PIPBUG_BAUDRATE_300     1
#define PIPBUG_BAUDRATE_4800    2
#define PIPBUG_BAUDRATE_ANY     3
#define BAUDRATE_DEFAULTPIPBUG1 PIPBUG_BAUDRATE_110 // what to use as default for PIPBUG 1 BIOS (PIPBUG_BAUDRATE_300 and PIPBUG_BAUDRATE_4800 are the other possibilities)
#define BAUDRATE_DEFAULTPIPBUG2 PIPBUG_BAUDRATE_300 // what to use as default for PIPBUG 2 BIOS (PIPBUG_BAUDRATE_110 is the other possibility)

#define BINBUG_BAUDRATE_150     0
#define BINBUG_BAUDRATE_300     1
#define BINBUG_BAUDRATE_1200    2
#define BINBUG_BAUDRATE_2400    3
#define BINBUG_BAUDRATE_ANY     4

/* #define TWIN_BAUDRATE_110    0
#define TWIN_BAUDRATE_150       1
#define TWIN_BAUDRATE_300       2
#define TWIN_BAUDRATE_600       3
#define TWIN_BAUDRATE_1200      4
#define TWIN_BAUDRATE_2400      5
#define TWIN_BAUDRATE_9600      6 */

#define HOSTVOLUME_MIN          0
#ifdef AMIGA
    #define HOSTVOLUME_DEFAULT  8
#endif
#ifdef WIN32
    #define HOSTVOLUME_DEFAULT  2
#endif
#define HOSTVOLUME_MAX          8

#define TAPEMODE_NONE           0 // must be 0!
#define TAPEMODE_STOP           1 // must be 1!
#define TAPEMODE_PLAY           2
#define TAPEMODE_RECORD         3

#define SENSITIVITY_VLOW        1
#define SENSITIVITY_LOW         2
#define SENSITIVITY_MEDIUM      3
#define SENSITIVITY_HIGH        4
#define SENSITIVITY_VHIGH       5
#define SENSITIVITY_MIN         1
#define SENSITIVITY_DEFAULT     3
#define SENSITIVITY_MAX         5

#define STYLE_SIGNETICS1        0
#define STYLE_SIGNETICS2        1
#define STYLE_OLDCALM           2
#define STYLE_NEWCALM           3
#define STYLE_IEEE              4
#define STYLES                  5

#define TOKEN_R0       65536
#define TOKEN_R1       65537
#define TOKEN_R2       65538
#define TOKEN_R3       65539
#define TOKEN_R4       65540
#define TOKEN_R5       65541
#define TOKEN_R6       65542
#define TOKEN_PSU      65543
#define TOKEN_PSL      65544
#define TOKEN_SP       65545
#define TOKEN_CC       65546
#define TOKEN_IAR      65547
#define TOKEN_RAS0     65548
#define TOKEN_RAS1     65549
#define TOKEN_RAS2     65550
#define TOKEN_RAS3     65551
#define TOKEN_RAS4     65552
#define TOKEN_RAS5     65553
#define TOKEN_RAS6     65554
#define TOKEN_RAS7     65555
#define FIRSTTOKEN     TOKEN_R0
#define LASTTOKEN      TOKEN_RAS7
#define ALLTOKENS      (LASTTOKEN + 1)
#define OUTOFRANGE     65556 // must be higher than any token

#define COMMASTRLEN    (13 + 1) // enough for "1,234,567,890" including EOS

#define PREVSPRITES       56 // for Interton/Elektor demultiplexing

#define EFLAG           TRUE // default flagline for most Elektor games

#define A_OFFSET_UPPERSCREEN 0x1800
#define A_OFFSET_LOWERSCREEN 0x1A00
#define A_OFFSET_SPRITES     0x1980
#define A_OFFSET_UDCS        0x19A0

// box widths must be multiples of 4!
#ifdef WIDEINSTRUCTOR
    #define INSTRUCTOR_BOXWIDTH 280
#else
    #define INSTRUCTOR_BOXWIDTH 184
#endif
#define COINOP_BOXWIDTH         240
#define SELBST_BOXWIDTH         512 // 64*8=512
#define MIKIT_BOXWIDTH          184
#ifdef BIGTYPERIGHT
    #define TYPERIGHT_BOXWIDTH  128
    #define TYPERIGHT_BOXHEIGHT  28
#else
    #define TYPERIGHT_BOXWIDTH   64
    #define TYPERIGHT_BOXHEIGHT  14
#endif
#define COINOP_BOXHEIGHT        240
#define EF9364_BOXHEIGHT        128 // purely for the VDU area
#define INSTRUCTOR_BOXHEIGHT     64
#define MIKIT_BOXHEIGHT          64
#define SELBST_BOXHEIGHT        194 // 16*8=128  +66=194

#define BOXWIDTH                904 // widest  box (CD2650 using margins)
#define BOXHEIGHT               356 // tallest box (Radio Bulletin VDU (PIPBUG) using margins & LEDs)
#define MAXBOXWIDTH            (BOXWIDTH    * 12)
#define MAXBOXHEIGHT           (BOXHEIGHT   *  6)
#define MAXBOX                 (MAXBOXWIDTH * MAXBOXHEIGHT)

#define MAX_VDUROWS              64 // for Elekterminal
#define MAX_VDUCOLUMNS           80 // for VT100

#define DG640_CHARWIDTH      9
#define DG640_CHARHEIGHT    16
#define SELBST_CHARWIDTH     8
#define SELBST_CHARHEIGHT    8
#define MIKIT_DIGITYLOC    171
#define SELBST_DIGITYLOC    57

#define PIPBUG_LEDHEIGHT    43 // 2 for divider +  6 for top margin + 29 for digits +  6 for bottom margin = 43
#define BINBUG_LEDHEIGHT    30 // 2 for divider + 11 for top margin +  6 for bulbs  + 11 for bottom margin = 30
#define PHUNSY_LEDHEIGHT    42 // 2 for divider +  7 for top margin + 26 for digits +  7 for bottom margin = 42
#define SELBST_LEDHEIGHT    55 // 2 for divider +  6 for top margin + 29 for digits +  6 for middle margin + 6 for bulbs + 6 for bottom margin = 55

#define UVI_HIDELEFT         6 // can be up to 14
#define PVI_HIDELEFT        11
#define PVI_RASTLINES      312
#define USG_XMARGIN         49
#define USG_YMARGIN         n1
#define HIDDEN_X            16 // for coin-ops
#define PVI_XSIZE          256 // 227 is unsuitable for Galaxia

// These are for Interton/Elektor, not Arcadia!
// Interton/Elektor colours are referred to in inverted RGB format!
#define BLACK              7 // 0,0,0 = 0
#define BLUE               6 // 0,0,1 = 1
#define GREEN              5 // 0,1,0 = 2 (different to Arcadia!)
#define CYAN               4 // 0,1,1 = 3 (different to Arcadia!)
#define RED                3 // 1,0,0 = 4 (different to Arcadia!)
#define PURPLE             2 // 1,0,1 = 5 (different to Arcadia!)
#define YELLOW             1 // 1,1,0 = 6
#define WHITE              0 // 1,1,1 = 7

#define GREY1              8
#define BROWN              9
#define DARKPURPLE        10
#define DARKRED           11
#define DARKCYAN          12
#define DARKGREEN         13
#define DARKBLUE          14
#define DKBLUE            DARKBLUE
#define DARKBLACK         15
#define GREY2             16
#define GREY3             17
#define GREY4             18
#define GREY5             19
#define GREY6             20
#define GREY7             21
#define ORANGE            22
#define PINK              23
#define DARKORANGE        DARKRED
#define DARKPINK          DARKRED
#define GUESTCOLOURS      24

#define EMUBRUSH_RED         0
#define EMUBRUSH_BLUE        1
#define EMUBRUSH_GREEN       2
#define EMUBRUSH_YELLOW      3
#define EMUBRUSH_ORANGE      4
#define EMUBRUSH_PURPLE      5
#define EMUBRUSH_CYAN        6
#define EMUBRUSH_GREY        7
#define EMUBRUSH_WHITE       8
#define EMUBRUSH_BLACK       9
#define EMUBRUSH_DARKRED    10
#define EMUBRUSH_DARKBLUE   11
#define EMUBRUSH_DARKGREEN  12
#define EMUBRUSH_DARKYELLOW 13
#define EMUBRUSH_DARKORANGE 14
#define EMUBRUSH_DARKPURPLE 15
#define EMUBRUSH_DARKCYAN   16
#define EMUBRUSH_PINK       17
#define EMUBRUSHES          18

// host joysticks
#define DAPTER_1    (1 <<  0)
#define DAPTER_2    (1 <<  1)
#define DAPTER_3    (1 <<  2)
#define DAPTER_4    (1 <<  3)
#define DAPTER_5    (1 <<  4)
#define DAPTER_6    (1 <<  5)
#define DAPTER_7    (1 <<  6)
#define DAPTER_8    (1 <<  7)
#define DAPTER_9    (1 <<  8)
#define DAPTER_C    (1 <<  9)
#define DAPTER_0    (1 << 10)
#define DAPTER_E    (1 << 11)
#define DAPTER_TOP    (0xFFF) // all 12 of the top buttons
#define JOYUP       (1 << 12)
#define JOYDOWN     (1 << 13)
#define JOYLEFT     (1 << 14)
#define JOYRIGHT    (1 << 15)
#define JOYFIRE1    (1 << 16)
#define JOYFIRE2    (1 << 17)
#define JOYFIRE3    (1 << 18)
#define JOYFIRE4    (1 << 19)
#define JOYSTART    (1 << 20)
#define JOYA        (1 << 21)
#define JOYB        (1 << 22)
#define JOYRESET    (1 << 23)
#define JOYPAUSE    (1 << 24)
#define JOYAUTOFIRE (1 << 25)

#define PSU_S            128 // %10000000 read-only
#define PSU_F             64 // %01000000
#define PSU_II            32 // %00100000
#define PSU_UF1           16 // %00010000 read-only on 2650A
#define PSU_UF2            8 // %00001000 read-only on 2650A
#define PSU_SP             7 // %00000111
#define PSU_WRITABLE_A  0x67 // %01100111
#define PSU_WRITABLE_B  0x7F // %01111111

#define PSL_CC           192 // %11000000
#define PSL_IDC           32
#define PSL_RS            16
#define PSL_WC             8
#define PSL_OVF            4
#define PSL_COM            2
#define PSL_C              1

#define A_SPRITE0Y       6384 // $18F0 R/W
#define A_SPRITE0X       6385 // $18F1 R/W
#define A_SPRITE1Y       6386 // $18F2 R/W
#define A_SPRITE1X       6387 // $18F3 R/W
#define A_SPRITE2Y       6388 // $18F4 R/W
#define A_SPRITE2X       6389 // $18F5 R/W
#define A_SPRITE3Y       6390 // $18F6 R/W
#define A_SPRITE3X       6391 // $18F7 R/W
#define A_VSCROLL        6396 // $18FC -/W?
#define A_PITCH          6397 // $18FD -/W?
#define A_VOLUME         6398 // $18FE -/W?
#define A_CHARLINE       6399 // $18FF (dma)
#define A_P1LEFTKEYS     6400 // $1900
#define A_P1MIDDLEKEYS   6401 // $1901
#define A_P1RIGHTKEYS    6402 // $1902
#define A_P1PALLADIUM    6403 // $1903
#define A_P2LEFTKEYS     6404 // $1904
#define A_P2MIDDLEKEYS   6405 // $1905
#define A_P2RIGHTKEYS    6406 // $1906
#define A_P2PALLADIUM    6407 // $1907
#define A_CONSOLE        6408 // $1908
#define A_GFXMODE        6648 // $19F8
#define A_BGCOLOUR       6649 // $19F9
#define A_SPRITES23CTRL  6650 // $19FA
#define A_SPRITES01CTRL  6651 // $19FB
#define A_BGCOLLIDE      6652 // $19FC
#define A_SPRITECOLLIDE  6653 // $19FD
#define A_P2PADDLE       6654 // $19FE
#define A_P1PADDLE       6655 // $19FF

#define IE_NOISE         0x1E80
#define IE_P1LEFTKEYS    0x1E88
#define IE_P1MIDDLEKEYS  0x1E89
#define IE_P1RIGHTKEYS   0x1E8A
#define IE_CONSOLE       0x1E8B
#define IE_P2LEFTKEYS    0x1E8C
#define IE_P2MIDDLEKEYS  0x1E8D
#define IE_P2RIGHTKEYS   0x1E8E

#define ASIC_UVI_SPRITE0Y      (1 <<  0) // $18F0 R/W
#define ASIC_UVI_SPRITE0X      (1 <<  1) // $18F1 R/W
#define ASIC_UVI_SPRITE1Y      (1 <<  2) // $18F2 R/W
#define ASIC_UVI_SPRITE1X      (1 <<  3) // $18F3 R/W
#define ASIC_UVI_SPRITE2Y      (1 <<  4) // $18F4 R/W
#define ASIC_UVI_SPRITE2X      (1 <<  5) // $18F5 R/W
#define ASIC_UVI_SPRITE3Y      (1 <<  6) // $18F6 R/W
#define ASIC_UVI_SPRITE3X      (1 <<  7) // $18F7 R/W
#define ASIC_UVI_VSCROLL       (1 <<  8) // $18FC -/W?
#define ASIC_UVI_PITCH         (1 <<  9) // $18FD -/W?
#define ASIC_UVI_VOLUME        (1 << 10) // $18FE -/W?
#define ASIC_UVI_CHARLINE      (1 << 11) // $18FF (dma)
#define ASIC_UVI_P1LEFTKEYS    (1 << 12) // $1900
#define ASIC_UVI_P1MIDDLEKEYS  (1 << 13) // $1901
#define ASIC_UVI_P1RIGHTKEYS   (1 << 14) // $1902
#define ASIC_UVI_P1PALLADIUM   (1 << 15) // $1903
#define ASIC_UVI_P2LEFTKEYS    (1 << 16) // $1904
#define ASIC_UVI_P2MIDDLEKEYS  (1 << 17) // $1905
#define ASIC_UVI_P2RIGHTKEYS   (1 << 18) // $1906
#define ASIC_UVI_P2PALLADIUM   (1 << 19) // $1907
#define ASIC_UVI_CONSOLE       (1 << 20) // $1908
#define ASIC_UVI_GFXMODE       (1 << 21) // $19F8
#define ASIC_UVI_BGCOLOUR      (1 << 22) // $19F9
#define ASIC_UVI_SPRITES23CTRL (1 << 23) // $19FA
#define ASIC_UVI_SPRITES01CTRL (1 << 24) // $19FB
#define ASIC_UVI_BGCOLLIDE     (1 << 25) // $19FC
#define ASIC_UVI_SPRITECOLLIDE (1 << 26) // $19FD
#define ASIC_UVI_P2PADDLE      (1 << 27) // $19FE
#define ASIC_UVI_P1PADDLE      (1 << 28) // $19FF

#define ASIC_PVI_SPRITE0AX     (1 <<  0)
#define ASIC_PVI_SPRITE0BX     (1 <<  1)
#define ASIC_PVI_SPRITE0AY     (1 <<  2)
#define ASIC_PVI_SPRITE0BY     (1 <<  3)
#define ASIC_PVI_SPRITE1AX     (1 <<  4)
#define ASIC_PVI_SPRITE1BX     (1 <<  5)
#define ASIC_PVI_SPRITE1AY     (1 <<  6)
#define ASIC_PVI_SPRITE1BY     (1 <<  7)
#define ASIC_PVI_SPRITE2AX     (1 <<  8)
#define ASIC_PVI_SPRITE2BX     (1 <<  9)
#define ASIC_PVI_SPRITE2AY     (1 << 10)
#define ASIC_PVI_SPRITE2BY     (1 << 11)
#define ASIC_PVI_SPRITE3AX     (1 << 12)
#define ASIC_PVI_SPRITE3BX     (1 << 13)
#define ASIC_PVI_SPRITE3AY     (1 << 14)
#define ASIC_PVI_SPRITE3BY     (1 << 15)
#define ASIC_PVI_SIZES         (1 << 16)
#define ASIC_PVI_SPR01COLOURS  (1 << 17)
#define ASIC_PVI_SPR23COLOURS  (1 << 18)
#define ASIC_PVI_SCORECTRL     (1 << 19)
#define ASIC_PVI_BGCOLOUR      (1 << 20)
#define ASIC_PVI_PITCH         (1 << 21)
#define ASIC_PVI_SCORELT       (1 << 22)
#define ASIC_PVI_SCORERT       (1 << 23)
#define ASIC_PVI_BGCOLLIDE     (1 << 24)
#define ASIC_PVI_SPRITECOLLIDE (1 << 25)
#define ASIC_PVI_P1PADDLE      (1 << 26)
#define ASIC_PVI_P2PADDLE      (1 << 27)
#define ASIC_IE_NOISE          (1 << 28)
#define ASIC_IE_P1KEYS         (1 << 29)
#define ASIC_IE_CONSOLE        (1 << 30)
#define ASIC_IE_P2KEYS         (1 << 31)

#define PVI_SPRITE0          0x00
#define PVI_SPRITE0AX        0x0A
#define PVI_SPRITE0BX        0x0B
#define PVI_SPRITE0AY        0x0C
#define PVI_SPRITE0BY        0x0D
#define PVI_SPRITE1          0x10
#define PVI_SPRITE1AX        0x1A
#define PVI_SPRITE1BX        0x1B
#define PVI_SPRITE1AY        0x1C
#define PVI_SPRITE1BY        0x1D
#define PVI_SPRITE2          0x20
#define PVI_SPRITE2AX        0x2A
#define PVI_SPRITE2BX        0x2B
#define PVI_SPRITE2AY        0x2C
#define PVI_SPRITE2BY        0x2D
#define PVI_SPRITE3          0x40
#define PVI_SPRITE3AX        0x4A
#define PVI_SPRITE3BX        0x4B
#define PVI_SPRITE3AY        0x4C
#define PVI_SPRITE3BY        0x4D
#define PVI_VERTGRID         0x80
#define PVI_HORIZ1           0xA8
#define PVI_HORIZ2           0xA9
#define PVI_HORIZ3           0xAA
#define PVI_HORIZ4           0xAB
#define PVI_HORIZ5           0xAC
#define PVI_SIZES            0xC0
#define PVI_SPR01COLOURS     0xC1
#define PVI_SPR23COLOURS     0xC2
#define PVI_SCORECTRL        0xC3
#define PVI_BGCOLOUR         0xC6
#define PVI_PITCH            0xC7
#define PVI_SCORELT          0xC8
#define PVI_SCORERT          0xC9
#define PVI_BGCOLLIDE        0xCA
#define PVI_SPRITECOLLIDE    0xCB
#define PVI_BGCOLLIDE_M2     0xDA
#define PVI_SPRITECOLLIDE_M2 0xDB
#define PVI_BGCOLLIDE_M3     0xEA
#define PVI_SPRITECOLLIDE_M3 0xEB
#define PVI_BGCOLLIDE_M4     0xFA
#define PVI_SPRITECOLLIDE_M4 0xFB
#define PVI_P1PADDLE         0xCC
#define PVI_P2PADDLE         0xCD

#define E_MONOB             0x800
#define E_MLINE             0x890
#define E_FSEQ              0x898
#define E_SILENC            0x898
#define E_ENTM              0x899
#define E_MFUNC             0x89A
#define E_MSCR              0x89B
#define E_RKBST             0x89D
#define E_LKBST             0x89E
#define E_MKBST             0x89F
#define E_FSCRM             0x8A0
#define E_SADR              0x8A2
#define E_BEGA              0x8A4
#define E_ENDA              0x8A8
#define E_BCC               0x8AA
#define E_MCNT              0x8AB
#define E_REGM              0x8AC
#define E_BK1               0x8B5
#define E_BK2               0x8B7
#define E_INTADR            0x8B9
#define E_ZPC               0x8BE

#define PSG_PITCHA1_L      0x1D00
#define PSG_PITCHA1_H      0x1D01
#define PSG_PITCHB1_L      0x1D02
#define PSG_PITCHB1_H      0x1D03
#define PSG_PITCHC1_L      0x1D04
#define PSG_PITCHC1_H      0x1D05
#define PSG_PITCHD1        0x1D06
#define PSG_MIXER1         0x1D07
#define PSG_AMPLITUDEA1    0x1D08
#define PSG_AMPLITUDEB1    0x1D09
#define PSG_AMPLITUDEC1    0x1D0A
#define PSG_PERIOD1_L      0x1D0B
#define PSG_PERIOD1_H      0x1D0C
#define PSG_SHAPE1         0x1D0D
#define PSG_PORTA1         0x1D0E
#define PSG_PORTB1         0x1D0F
#define PSG_PITCHA2_L      0x1D10
#define PSG_PITCHA2_H      0x1D11
#define PSG_PITCHB2_L      0x1D12
#define PSG_PITCHB2_H      0x1D13
#define PSG_PITCHC2_L      0x1D14
#define PSG_PITCHC2_H      0x1D15
#define PSG_PITCHD2        0x1D16
#define PSG_MIXER2         0x1D17
#define PSG_AMPLITUDEA2    0x1D18
#define PSG_AMPLITUDEB2    0x1D19
#define PSG_AMPLITUDEC2    0x1D1A
#define PSG_PERIOD2_L      0x1D1B
#define PSG_PERIOD2_H      0x1D1C
#define PSG_SHAPE2         0x1D1D
#define PSG_PORTA2         0x1D1E
#define PSG_PORTB2         0x1D1F
#define E_RANDOM2          0x1D20
#define E_LINEIN0          0x1D9C
#define E_LINEIN1          0x1D9D
#define E_LINEIN2          0x1D9E
#define E_LINEIN3          0x1D9F
#define E_LINEIN4          0x1DBC
#define E_LINEIN5          0x1DBD
#define E_LINEIN6          0x1DBE
#define E_LINEIN7          0x1DBF // aka CASIN
#define E_CASIN            0x1DBF // aka LINEIN7
#define E_LINEOUT0         0x1DDC
#define E_LINEOUT1         0x1DDD
#define E_LINEOUT2         0x1DDE
#define E_LINEOUT3         0x1DDF
#define E_LINEOUT4         0x1DFC
#define E_LINEOUT5         0x1DFD
#define E_LINEOUT6         0x1DFE
#define E_LINEOUT7         0x1DFF // aka CASOUT
#define E_CASOUT           0x1DFF // aka LINEOUT7
#define E_RANDOM1G         0x1FAF
#define E_RANDOM1E         0x1FFF

#define T_COM               0x400
#define T_XGOT              0x409
#define T_TEMP              0x40D
#define T_TEMQ              0x40F
#define T_TEMR              0x411
#define T_TEMS              0x412
#define T_BUFF              0x413
#define T_BPTR              0x427
#define T_MCNT              0x428
#define T_CNT               0x429
#define T_CODE              0x42A
#define T_OKGO              0x42B
#define T_BCC               0x42C
#define T_MARK              0x42D
#define T_HDAT              0x42F
#define T_LDAT              0x431
#define T_HADR              0x433
#define T_LADR              0x435

#define S_IOPORT            0xFFF
#define S_DIGITS           0x17D1 // $17D1..$17D8

// CD2650 RAM data
#define C_LENT             0x17EA
#define C_BKP1             0x17F4
#define C_BKP2             0x17F5
#define C_BPD1             0x17F6
#define C_BPD2             0x17F7
#define C_SUMC             0x17F9
#define C_ADD1             0x17FA
#define C_ADD2             0x17FB
#define C_CUR1             0x17FE
#define C_CUR2             0x17FF

// "Project" menu (mostly)
#define MENUITEM_RESET              0
#define MENUITEM_RESETTOMONITOR     1
#define MENUITEM_REINITIALIZE       2
#define MENUITEM_OPEN               3
#define MENUITEM_REVERT             4
#define MENUITEM_QUICKLOAD          5
#define MENUITEM_SAVESNP            6
#define MENUITEM_SAVEACBM           7
#define MENUITEM_SAVEILBM           8
#define MENUITEM_SAVEBMP            9
#define MENUITEM_SAVEGIF           10
#define MENUITEM_SAVEPCX           11
#define MENUITEM_SAVEPNG           12
#define MENUITEM_SAVETIFF          13
#define MENUITEM_SAVEASCII         14
#define MENUITEM_SAVEASM           15
#define MENUITEM_QUICKSAVE         16
#define MENUITEM_AUDIT             17
#define MENUITEM_REGISTER          18
#define MENUITEM_SERVER            19
#define MENUITEM_CLIENT            20
#define MENUITEM_RECENT1           21
#define MENUITEM_RECENT2           22
#define MENUITEM_RECENT3           23
#define MENUITEM_RECENT4           24
#define MENUITEM_RECENT5           25
#define MENUITEM_RECENT6           26
#define MENUITEM_RECENT7           27
#define MENUITEM_RECENT8           28
#define MENUITEM_ICONIFY           29
#define MENUITEM_UNICONIFY         30 // on tray menu
#define MENUITEM_QUIT              31
// "Edit" menu
#define MENUITEM_COPYSCRN          32
#define MENUITEM_COPYTEXT          33
#define MENUITEM_PASTETEXT         34
#define MENUITEM_EMPTYCLIPBOARD    35
// "View" menu
#define MENUITEM_VIEWHISCORES      36
#define MENUITEM_VIEWDEBUGGER      37
#define MENUITEM_MENUBAR           38
#define MENUITEM_POINTER           39
#define MENUITEM_SIDEBAR           40
#define MENUITEM_STATUSBAR         41
#define MENUITEM_TITLEBAR          42
#define MENUITEM_TOOLBAR           43
#define MENUITEM_BUILTIN           44
#define MENUFAKE_SORTBY            45
#define MENUFAKE_SPEEDINDICATOR    46
// "Macros" menu (mostly)
#define MENUITEM_STARTRECORDING    47
#define MENUITEM_RESTARTPLAYBACK   48
#define MENUITEM_STOP              49
#define MENUITEM_LOOP              50
#define MENUITEM_WARN              51
#define MENUITEM_IFFANIMS          52
#define MENUITEM_GIFANIMS          53
#define MENUITEM_MNG               54
#define MENUITEM_APNG              55
#define MENUITEM_GENERATE8SVX      56
#define MENUITEM_GENERATEAIFF      57
#define MENUITEM_GENERATESMUS      58
#define MENUITEM_GENERATEMIDI      59
#define MENUITEM_GENERATEPSG       60
#define MENUITEM_GENERATEWAV       61
#define MENUITEM_GENERATEYM        62
#define MENUITEM_RUNREXX           63
#define MENUITEM_REPEATREXX        64
// "Debug|General �" submenu
#define MENUITEM_QUOTES            65
#define MENUITEM_CLEARCOV          66
#define MENUITEM_CLS               67
#define MENUITEM_HELP              68
#define MENUITEM_SAY               69
#define MENUITEM_SYSTEM            70
// "Debug|File �" submenu
#define MENUITEM_ASTERISK          71
#define MENUITEM_ASM               72
#define MENUITEM_DELETE            73
#define MENUITEM_DIR               74
#define MENUITEM_DISGAME           75
#define MENUITEM_EDIT              76
#define MENUITEM_EXTRACT           77
#define MENUITEM_LOADBIN           78
#define MENUITEM_SAVEAOF           79
#define MENUITEM_SAVEBIN           80
#define MENUITEM_SAVEBPNF          81
#define MENUITEM_SAVECMD           82
#define MENUITEM_SAVEHEX           83
#define MENUITEM_SAVESMS           84
#define MENUITEM_SAVETVC           85
#define MENUFAKE_DRIVE             86
// "Debug|Edit �" submenu
#define MENUITEM_DOKE              87
#define MENUITEM_POKE              88
#define MENUITEM_FPOKE             89
#define MENUITEM_WRITEPORT         90
// "Debug|View �" submenu
#define MENUITEM_EQUALS            91
#define MENUITEM_COVER             92
#define MENUITEM_PEEK              93
#define MENUITEM_DIS               94
#define MENUITEM_ERROR             95
#define MENUITEM_FPEEK             96
#define MENUITEM_HISTORY           97
#define MENUITEM_IM                98
#define MENUITEM_READPORT          99
#define MENUITEM_VIEW_BASIC       100
#define MENUITEM_VIEW_BIOS        101
#define MENUITEM_VIEW_CPU         102
#define MENUITEM_VIEW_PSG         103
#define MENUITEM_VIEW_RAM         104
#define MENUITEM_VIEW_SCRN        105
#define MENUITEM_VIEW_UDG         106
#define MENUITEM_VIEW_XVI         107
// "Debug|Log �" submenu
#define MENUITEM_L_A              108
#define MENUITEM_L_B              109
#define MENUITEM_L_C              110
#define MENUITEM_L_I              111
#define MENUITEM_L_N              112
#define MENUITEM_L_S              113
#define MENUITEM_PL               114
#define MENUITEM_T                115
// "Debug|Run �" submenu
#define MENUITEM_G                116
#define MENUITEM_GI               117
#define MENUITEM_I                118
#define MENUITEM_JUMP             119
#define MENUITEM_O                120
#define MENUITEM_S                121
#define MENUITEM_R                122
#define MENUITEM_R_F              123
#define MENUITEM_R_I              124
#define MENUITEM_R_L              125
#define MENUITEM_R_R              126
#define MENUITEM_R_S              127
// "Debug|Breakpoints �" submenu
#define MENUITEM_BP               128
#define MENUITEM_BC               129
#define MENUITEM_BL               130
#define MENUITEM_FP               131
#define MENUITEM_FC               132
#define MENUITEM_FL               133
#define MENUITEM_IP               134
#define MENUITEM_IC               135
#define MENUITEM_IL               136
#define MENUITEM_WP               137
#define MENUITEM_WC               138
#define MENUITEM_WL               139
#define MENUITEM_PB               140
#define MENUITEM_WR               141
#define MENUFAKE_WW               142
// "Debug|Symbols �" submenu
#define MENUITEM_CLEARSYM         143
#define MENUITEM_DEFSYM           144
#define MENUITEM_LOADSYM          145
#define MENUITEM_SAVESYM          146
// "Debug|Tools �" submenu
#define MENUITEM_COMP             147
#define MENUITEM_COPY             148
#define MENUITEM_FILL             149
#define MENUITEM_FIND             150
#define MENUITEM_REL              151
#define MENUITEM_SWAP             152
#define MENUITEM_TRAIN            153
// "Debug|Options �" submenu
#define MENUFAKE_BASE             154
#define MENUFAKE_CPU              155
#define MENUFAKE_N                156
#define MENUFAKE_TU               157
#define MENUFAKE_VERBOSITY        158
#define MENUITEM_GR               159
// "Debug|Graphics �" submenu
#define MENUFAKE_DRAW             160
#define MENUFAKE_SPR              161
// "Debug" menu
#define MENUITEM_TOOLSDEBUGGER    162
// "Tools" menu
#define MENUITEM_EDITMEMORY       163
#define MENUITEM_MUSIC            164
#define MENUITEM_SCREENEDITOR     165
#define MENUITEM_SPRITEVIEWER     166
#define MENUITEM_VIEWMONITOR_CPU  167
#define MENUITEM_VIEWMONITOR_PSGS 168
#define MENUITEM_VIEWMONITOR_XVI  169
#define MENUITEM_CHEEVOS1         170
// "Peripherals" menu
#define MENUITEM_CONTROLS         171
#define MENUITEM_DIPSWITCHES      172
// "Peripherals|Left controller �" submenu
#define MENUFAKE_LEFT             173
#define MENUITEM_AUTOFIRE1        174
#define MENUITEM_REQUIREBUTTON1   175
#define MENUITEM_FORCEFEEDBACK1   176
// "Peripherals|Right controller �" submenu
#define MENUFAKE_RIGHT            177
#define MENUITEM_AUTOFIRE2        178
#define MENUITEM_REQUIREBUTTON2   179
#define MENUITEM_FORCEFEEDBACK2   180
// "Peripherals" menu
#define MENUITEM_TAPEDECK         181
#define MENUITEM_FLOPPYDRIVE      182
#define MENUITEM_PAPERTAPE        183
#define MENUITEM_PRINTER          184
// "Settings|BIOS �" submenu
#define MENUFAKE_ELEKTORBIOS      185
#define MENUFAKE_PIPBUGBIOS       186
#define MENUFAKE_BINBUGBIOS       187
#define MENUFAKE_CD2650BIOS       188
#define MENUFAKE_PHUNSYBIOS       189
#define MENUFAKE_SELBSTBIOS       190
// "Settings|Colours �" submenu
#define MENUITEM_PALETTE          191
#define MENUITEM_UNDITHER         192
#define MENUITEM_DARKENBG         193
#define MENUITEM_FLAGLINE         194
#define MENUITEM_INVERSE          195
#define MENUFAKE_COLOURSET        196
// "Settings|DOS �" submenu
#define MENUFAKE_BINBUGDOS        197
#define MENUFAKE_TWINDOS          198
#define MENUFAKE_CD2650DOS        199
// "Settings|Emulator �" submenu
#define MENUITEM_AUTOSAVE         200
#define MENUITEM_STARTUPUPDATES   201
#define MENUITEM_CONFIRM          202
#define MENUITEM_CREATEICONS      203
#define MENUITEM_CONSOLE          204
#define MENUITEM_EMUID            205
#define MENUITEM_POST             206
#define MENUITEM_RANDOMIZE        207
#define MENUITEM_SENSEGAME        208
#define MENUITEM_SHOWTOD          209
#define MENUITEM_USESTUBS         210
#define MENUFAKE_LOGTOFILE        211
#define MENUITEM_PATHS            212
// "Settings|Filters �" submenu
#define MENUFAKE_FILTER           213
#define MENUITEM_SCANLINES        214
#define MENUITEM_STRETCH43        215
#define MENUFAKE_STRETCHING       216
// "Settings|Graphics �" submenu
#define MENUFAKE_SIZE             217
#define MENUITEM_DEJITTER         218
#define MENUITEM_SKIES            219
#define MENUITEM_CORNERS          220
#define MENUITEM_FULLSCREEN       221
#define MENUITEM_NARROW           222
// spare                          223
#define MENUITEM_UNLIT            224
#define MENUITEM_SHOWLEDS         225
// "Settings|Input �" submenu
#define MENUITEM_ANALOG           226
#define MENUITEM_SHOWPOSITIONS    227
#define MENUITEM_SPRINGLOADED     228
#define MENUITEM_SWAPPED          229
#define MENUITEM_CALIBRATE        230
#define MENUITEM_REARRANGE        231
#define MENUITEM_REDEFINEKEYS     232
#define MENUITEM_SENSITIVITY      233
#define MENUITEM_LOWERCASEINPUT   234
#define MENUITEM_CONFINE          235
#define MENUITEM_AUTOCOIN         236
#define MENUITEM_GUESTRMB         237
#define MENUITEM_QUEUEKEYSTROKES  238
#define MENUFAKE_KEYMAP           239
// "Settings|Language �" submenu
#define MENUFAKE_LANGUAGE         240
// "Settings|Machine �" submenu
#define MENUFAKE_MACHINE          241
// "Settings|Sound �" submenu
#define MENUITEM_SOUND            242
#define MENUITEM_ADJUSTSOUND      243
#define MENUITEM_AMBIENT          244
#define MENUITEM_FILTERED         245
#define MENUITEM_RETUNE           246
#define MENUITEM_SOUNDOUTPUT      247
// "Settings|Speed �" submenu
#define MENUITEM_ADJUSTSPEED      248
#define MENUITEM_PAUSED           249
#define MENUITEM_AUTOPAUSE        250
#define MENUITEM_LIMITREFRESHES   251
#define MENUITEM_TURBO            252
#define MENUFAKE_REGION           253
#define MENUFAKE_PRIORITY         254
// "Settings|Sprites �" submenu
#define MENUITEM_COLLISIONS       255
#define MENUFAKE_DEMULTIPLEX      256
// "Settings|Trainers �" submenu
#define MENUITEM_CHEATLIVES       257
#define MENUITEM_CHEATTIME        258
#define MENUITEM_INVINCIBILITY    259
#define MENUITEM_LEVELSKIP        260
// "Settings|VDU �" submenu
#define MENUITEM_USEMARGINS       261
#define MENUITEM_BLINK            262
#define MENUITEM_COOMER           263
#define MENUITEM_ROTATE           264
#define MENUFAKE_PIPBUGVDU        265
// spare                          266
#define MENUFAKE_CD2650VDU        267
// "Help" menu
#define MENUITEM_GAMEINFO         268
#define MENUITEM_HOSTPADS         269
#define MENUITEM_HOSTKYBD         270
#define MENUITEM_OPCODES          271
#define MENUITEM_CODINGGUIDE      272
#define MENUITEM_GAMINGGUIDE      273
#define MENUITEM_MANUAL           274
#define MENUITEM_UPDATE           275
#define MENUITEM_REACTION         276
#define MENUITEM_ABOUT            277
// menus
#define MENUMENU_PROJECT          278
#define MENUMENU_EDIT             279
#define MENUMENU_VIEW             280
#define MENUMENU_MACRO            281
#define MENUMENU_DEBUG            282
#define MENUMENU_TOOLS            283
#define MENUMENU_PERIPHERALS      284
#define MENUMENU_SETTINGS         285
#define MENUMENU_HELP             286
// submenus
#define MENUMENU_DEBUG_GENERAL    287
#define MENUMENU_DEBUG_FILE       288
#define MENUMENU_DEBUG_EDIT       289
#define MENUMENU_DEBUG_VIEW       290
#define MENUMENU_DEBUG_LOG        291
#define MENUMENU_DEBUG_RUN        292
#define MENUMENU_DEBUG_BP         293
#define MENUMENU_DEBUG_SYMBOLS    294
#define MENUMENU_DEBUG_TOOLS      295
#define MENUMENU_DEBUG_OPTIONS    296
#define MENUMENU_DEBUG_GRAPHICS   297
#define MENUMENU_LEFT             298
#define MENUMENU_RIGHT            299
#define MENUMENU_BIOS             300
#define MENUMENU_DOS              301
#define MENUMENU_FILTERS          302
#define MENUMENU_MACHINE          303
#define MENUMENU_SPRITES          304
#define MENUMENU_TRAINERS         305
#define MENUMENU_VDU              306
// new ones
#define MENUFAKE_FRAMEBASED       307 // new for V34.7
#define MENUITEM_INJECT           308 // new for V34.7
#define MENUITEM_RENAME           309 // new for V34.7
#define MENUMENU_DEBUG_DISK       310 // new for V34.7
#define MENUITEM_EXACTSPEED       311 // new for V34.74
#define MENUITEM_GENERATE         312 // new for V34.91
#define MENUMENU_MACRO_ANIMS      313 // new for V34.91
#define MENUMENU_MACRO_SOUNDS     314 // new for V34.91
#define MENUITEM_BEZEL            315 // new for V35.1
#define MENUITEM_CHEEVOS2         316 // new for V35.21
#define MENUITEM_ERASEDEL         317 // new for V35.33
#define MENUITEM_INDUSTRIAL       318 // new for V35.4
#define MENUITEMS                 319 // counting from 1 (ie. last entry is [MENUITEMS-1])

// "View" menu
#define MENUOPT_SORTBYNAME          0
#define MENUOPT_SORTBYMACHINE       1
#define MENUOPT_INDICATOR_FPS       2
#define MENUOPT_INDICATOR_KHZ       3
#define MENUOPT_INDICATOR_PERCENT   4
// "Debug|File �" submenu
#define MENUOPT_DRIVE_0             5
#define MENUOPT_DRIVE_1             6
// "Debug|Breakpoints �" submenu
#define MENUOPT_WW_NONE             7
#define MENUOPT_WW_SOME             8
#define MENUOPT_WW_ALL              9
// "Debug|Options �" submenu
#define MENUOPT_BASE_BINARY        10
#define MENUOPT_BASE_OCTAL         11
#define MENUOPT_BASE_DECIMAL       12
#define MENUOPT_BASE_HEX           13
#define MENUOPT_CPU_0              14
#define MENUOPT_CPU_1              15
#define MENUOPT_N_0                16
#define MENUOPT_N_1                17
#define MENUOPT_N_2                18
#define MENUOPT_N_3                19
#define MENUOPT_TU_0               20
#define MENUOPT_TU_1               21
#define MENUOPT_TU_2               22
#define MENUOPT_VB_0               23
#define MENUOPT_VB_1               24
#define MENUOPT_VB_2               25
// "Debug|Graphics �" submenu
#define MENUOPT_DRAW_0             26
#define MENUOPT_DRAW_1             27
#define MENUOPT_DRAW_2             28
#define MENUOPT_DRAW_3             29
#define MENUOPT_DRAW_4             30
#define MENUOPT_SPR_0              31
#define MENUOPT_SPR_1              32
#define MENUOPT_SPR_2              33
// "Peripherals|Left controller �" submenu
#define MENUOPT_LEFT_TRACKBALL     34
#define MENUOPT_LEFT_NONE          35
#define MENUOPT_LEFT_1STDJOY       36
#define MENUOPT_LEFT_2NDDJOY       37
#define MENUOPT_LEFT_2NDAJOY       38
#define MENUOPT_LEFT_1STDPAD       39
#define MENUOPT_LEFT_2NDDPAD       40
#define MENUOPT_LEFT_1STAPAD       41
#define MENUOPT_LEFT_2NDAPAD       42
// "Peripherals|Right controller �" submenu
#define MENUOPT_RIGHT_TRACKBALL    43
#define MENUOPT_RIGHT_NONE         44
#define MENUOPT_RIGHT_1STDJOY      45
#define MENUOPT_RIGHT_2NDDJOY      46
#define MENUOPT_RIGHT_2NDAJOY      47
#define MENUOPT_RIGHT_1STDPAD      48
#define MENUOPT_RIGHT_2NDDPAD      49
#define MENUOPT_RIGHT_1STAPAD      50
#define MENUOPT_RIGHT_2NDAPAD      51
// "Settings|BIOS �" submenu
#define MENUOPT_PHILIPS            52
#define MENUOPT_HOBBYMODULE        53
#define MENUOPT_ARTEMIS            54
#define MENUOPT_HYBUG              55
#define MENUOPT_PIPBUG1BIOS        56
#define MENUOPT_PIPBUG2BIOS        57
#define MENUOPT_BINBUG35BIOS       58
#define MENUOPT_BINBUG36BIOS       59
#define MENUOPT_BINBUG61BIOS       60
#define MENUOPT_GBUG               61
#define MENUOPT_MIKEBUG            62
#define MENUOPT_MYBUG              63
#define MENUOPT_BIOS_IPL           64
#define MENUOPT_BIOS_POPMON        65
#define MENUOPT_BIOS_SUPERVISOR    66
#define MENUOPT_BIOS_MINIMONITOR   67
#define MENUOPT_BIOS_PHUNSY        68
#define MENUOPT_00BIOS             69
#define MENUOPT_09BIOS             70
#define MENUOPT_10BIOS             71
#define MENUOPT_20BIOS             72
// "Settings|Colours �" submenu
#define MENUOPT_AMBER              73
#define MENUOPT_GREEN              74
#define MENUOPT_GREYSCALE          75
#define MENUOPT_PURECOLOURS        76
#define MENUOPT_PVICOLOURS         77
#define MENUOPT_UVICOLOURS         78
// "Settings|DOS �" submenu
#define MENUOPT_MICRODOS           79
#define MENUOPT_VHSDOS             80
#define MENUOPT_NOBINBUGDOS        81
#define MENUOPT_DOS_EXOS           82
#define MENUOPT_DOS_SDOS20         83
#define MENUOPT_DOS_SDOS40         84
#define MENUOPT_DOS_SDOS42         85
#define MENUOPT_DOS_TOS            86
#define MENUOPT_DOS_UDOS           87
#define MENUOPT_DOS_NOTWINDOS      88
#define MENUOPT_CDDOS              89
#define MENUOPT_P1DOS              90
#define MENUOPT_NOCD2650DOS        91
// "Settings|Emulator �" submenu
#define MENUOPT_APPEND             92
#define MENUOPT_IGNORE             93
#define MENUOPT_REPLACE            94
// "Settings|Filter �" submenu
#define MENUOPT_FILTER_3D          95
#define MENUOPT_FILTER_HQX         96
#define MENUOPT_FILTER_SCALE2X     97
#define MENUOPT_FILTER_NONE        98
#define MENUOPT_UNSTRETCHED        99
#define MENUOPT_STRETCH43         100
#define MENUOPT_STRETCHTOFIT      101
// "Settings|Graphics �" submenu
#define MENUOPT_1XSIZE            102
#define MENUOPT_2XSIZE            103
#define MENUOPT_3XSIZE            104
#define MENUOPT_4XSIZE            105
#define MENUOPT_5XSIZE            106
#define MENUOPT_6XSIZE            107
// "Settings|Input �" submenu
#define MENUOPT_KEYMAP_HOST       108
#define MENUOPT_KEYMAP_EA         109
#define MENUOPT_KEYMAP_TT         110
// "Settings|Language �" submenu
#define MENUOPT_ENG               111
#define MENUOPT_HOL               112
#define MENUOPT_FRA               113
#define MENUOPT_GER               114
#define MENUOPT_GRE               115
#define MENUOPT_ITA               116
#define MENUOPT_POL               117
#define MENUOPT_RUS               118
#define MENUOPT_SPA               119
// "Settings|Machine �" submenu
#define MENUOPT_ARCADIA           120
#define MENUOPT_INTERTON          121
#define MENUOPT_ELEKTOR           122
#define MENUOPT_PIPBUG            123
#define MENUOPT_BINBUG            124
#define MENUOPT_INSTRUCTOR        125
#define MENUOPT_TWIN              126
#define MENUOPT_CD2650            127
#define MENUOPT_PHUNSY            128
#define MENUOPT_SELBST            129
#define MENUOPT_MIKIT             130
#define MENUOPT_ASTROWARS         131
#define MENUOPT_GALAXIA           132
#define MENUOPT_LASERBATTLE       133
#define MENUOPT_LAZARIAN          134
#define MENUOPT_MALZAK1           135
#define MENUOPT_MALZAK2           136
#define MENUOPT_8550              137
#define MENUOPT_8600              138
#define MENUOPT_TYPERIGHT         139
// "Settings|Sound �" submenu
#define MENUOPT_SOUND_AHI         140
#define MENUOPT_SOUND_PAULA       141
// "Settings|Speed �" submenu
#define MENUOPT_NTSC              142
#define MENUOPT_PAL               143
#define MENUOPT_HIGHPRI           144
#define MENUOPT_NORMALPRI         145
#define MENUOPT_LOWPRI            146
// "Settings|VDU �" submenu
#define MENUOPT_ELEKTERMINAL      147
#define MENUOPT_LCVDU_NARROW      148
#define MENUOPT_LCVDU_WIDE        149
#define MENUOPT_RADIOBULLETIN     150
#define MENUOPT_SVT100            151
#define MENUOPT_VT100             152
#define MENUOPT_ASCIIVDU          153
#define MENUOPT_CHESSMEN          154
#define MENUOPT_LOWERCASEVDU      155
// new ones
#define MENUOPT_DRIVE_2                 156
#define MENUOPT_DRIVE_3                 157
#define MENUOPT_DEMULTIPLEX_MULTIPLEX   158
#define MENUOPT_DEMULTIPLEX_TRANSPARENT 159
#define MENUOPT_DEMULTIPLEX_OPAQUE      160
#define MENUOPT_FRAMEBASED              161
#define MENUOPT_PIXELBASED              162
#define MENUOPT_N_4                     163
#define MENUOPTS                        164 // counting from 1 (ie. last entry is [MENUOPTS - 1])

// small gadget positions, left to right, on GID_SB2
// input
#define GADPOS_AUTOFIRE1       0
#define GADPOS_AUTOFIRE2       1
#define GADPOS_SWAPPED         2
// sound
#define GADPOS_SOUND           3
// speed
#define GADPOS_PAUSED          4
#define GADPOS_TURBO           5
// sprites
#define GADPOS_COLLISIONS      6
// macros
#define GADPOS_RECORD          7
#define GADPOS_PLAY            8
#define GADPOS_STOP            9
// console
#define GADPOS_START          10
#define GADPOS_A              11
#define GADPOS_B              12
#define GADPOS_RESET          13

#define RECMODE_NORMAL   1000000000
#define RECMODE_PLAY     1000000001
#define RECMODE_RECORD   1000000002

#define AUTOLINE_GAMENAME      0
#define AUTOLINE_CREDITS       1
#define AUTOLINE_REFERENCE     2
#define AUTOLINE_MACHINE       3
#define AUTOLINE_SIZE          4
#define AUTOLINE_CRC32         5
#define AUTOLINE_COMPLANG      6
#define AUTOLINE_STARTADDR     7
#define GAMEINFOLINES          8 // counting from 1 (last array index is GAMEINFOLINES-1)

#define NUMKEYS               28 // counting from 1 (last array index is NUMKEYS-1)
#define KEYS                  80 // counting from 1 (last array index is KEYS-1)

#define KIND_BP                0
#define KIND_WP                1
#define KIND_BC                2
#define KIND_WC                3

#define ARCADIA                0
#define INTERTON               1
#define ELEKTOR                2
#define PIPBUG                 3
#define BINBUG                 4
#define INSTRUCTOR             5
#define TWIN                   6
#define CD2650                 7
#define ZACCARIA               8
#define MALZAK                 9
#define PHUNSY                10
#define PONG                  11
#define SELBST                12
#define MIKIT                 13
#define TYPERIGHT             14
#define MACHINES              15

// these must start from 0 and be consecutive
#define MEMMAP_A               0 // Interton                    (2K ROM + 0K RAM)
#define MEMMAP_B               1 // Interton                    (4K ROM + 0K RAM)
#define MEMMAP_C               2 // Interton                    (4K ROM + 1K RAM)
#define MEMMAP_D               3 // Interton                    (6K ROM + 1K RAM)
#define MEMMAP_E               4 // Elektor                     (basic)
#define MEMMAP_F               5 // Elektor                     (expanded)
#define MEMMAP_G               6 // Arcadia    ( 512 bytes RAM) (Emerson)
#define MEMMAP_H               7 // Arcadia    (1024 bytes RAM) (Tele-Fever)
#define MEMMAP_I               8 // Arcadia    ( 512 bytes RAM) (Palladium)
#define MEMMAP_PIPBUG1         9 // PIPBUG     (       11K RAM) (11K EA 78up5 with EPROM)
#define MEMMAP_PIPBUG2        10 // PIPBUG     (modified ABC1500 with CP1002)
#define MEMMAP_BINBUG         11
#define MEMMAP_TWIN           12
#define MEMMAP_ARTEMIS        13 // PIPBUG
#define MEMMAP_N              14 // Instructor ( 640 bytes RAM) (basic)
#define MEMMAP_O              15 // Instructor (  28K+128b RAM) (expanded)
#define MEMMAP_CD2650         16 // aka memmap "U"
// 17..18 are spare
#define MEMMAP_PHUNSY         19 // PHUNSY
#define MEMMAP_SELBST         20
#define MEMMAP_MIKIT          21
#define MEMMAP_ASTROWARS      22 // aka memmap "X"
#define MEMMAP_GALAXIA        23 // aka memmap "W"
#define MEMMAP_LASERBATTLE    24
#define MEMMAP_LAZARIAN       25
#define MEMMAP_MALZAK1        26 // aka memmap "T"
#define MEMMAP_MALZAK2        27 // aka memmap "V"
#define MEMMAP_8550           28
#define MEMMAP_8600           29
#define MEMMAP_TYPERIGHT      30
#define MEMMAPS               31

#define MEMMAP_ARCADIA        MEMMAP_G // default Arcadia memory map you prefer
#ifdef BASICMODE
    #define MEMMAP_ELEKTOR    MEMMAP_E
#else
    #define MEMMAP_ELEKTOR    MEMMAP_F
#endif

#define FRAMESKIP_MIN          1
#define FRAMESKIP_MAX         10
#define FRAMESKIP_INFINITE    11

#define OVERLAYS            (278 + 1)
#define ARCADIA_OVERLAY        0
#define ELEKTOR_OVERLAY        1
#define SI50_OVERLAY           2
#define QWERTY_OVERLAY         3
#define PIPBUG_OVERLAY         QWERTY_OVERLAY
#define CD2650_OVERLAY         QWERTY_OVERLAY
#define INTERTON_OVERLAY      15
#define BLANK_OVERLAY        149
#define ASTROWARS_OVERLAY    160
#define MALZAK1_OVERLAY      161
#define MALZAK2_OVERLAY      173
#define PHUNSY_OVERLAY       183
#define AY8550_OVERLAY       195
#define LASERBATTLE_OVERLAY  197
#define BINBUG_OVERLAY       202
#define SELBST_OVERLAY       203
#define AY8600_OVERLAY       206
#define MIKIT_OVERLAY        209
#define GALAXIA_OVERLAY      231
#define TYPERIGHT_OVERLAY    236

#define KNOWNGAMES          (855 + 1)
#define LARGESTKNOWN       59796 // size of largest known game in bytes (AOF version of Pattern)

// Arcadia                           0..  29
// Interton/Elektor                 30..  37
#define FIRSTPHILIPSEQUIV           30
#define LASTPHILIPSEQUIV           324
#define FIRSTPSGEQUIV              273
#define  LASTPSGEQUIV              304
// Elektor                          38.. 324
#define FIRSTPVIEQUIV              325
#define FIRSTPIPBUG1EQUIV          424
#define  LASTPIPBUG1EQUIV          529
#define FIRSTSI50EQUIV             530
#define  LASTSI50EQUIV             782
#define FIRSTCD2650EQUIV           783
#define  LASTCD2650EQUIV           845
#define FIRSTPHUNSYEQUIV           846
#define  LASTPHUNSYEQUIV          1298
#define FIRSTBINBUGEQUIV          1299
#define  LASTBINBUGEQUIV          1405
#define FIRST09EQUIV              1406 // Selbst
#define FIRST10EQUIV              1406
#define  LAST10EQUIV              1445
#define  LAST09EQUIV              1564
#define FIRST20EQUIV              1565
#define  LAST20EQUIV              1706
#define FIRSTHOBBYMODULEEQUIV     1707
#define LASTHOBBYMODULEEQUIV      1716
#define FIRSTNONBINBUGEQUIV       1717
#define  LASTNONBINBUGEQUIV       1720
#define FIRSTGBUGEQUIV            1721
#define  LASTGBUGEQUIV            1835
#define FIRSTVHSDOSEQUIV          1836
#define  LASTVHSDOSEQUIV          2058
#define FIRSTMICRODOSEQUIV        2059
#define  LASTMICRODOSEQUIV        2300
#define FIRSTACOSEQUIV            2301
#define  LASTACOSEQUIV            2518
#define FIRSTBINBUG61EQUIV        2519
#define  LASTBINBUG61EQUIV        2629
#define FIRSTPIPBUG2EQUIV         2630
#define  LASTPIPBUG2EQUIV         2797
#define FIRSTPOPMONEQUIV          2798
#define  LASTPOPMONEQUIV          3016
#define FIRSTTWINEQUIV            3017
#define  LASTTWINEQUIV            3065
#define FIRSTSDOS20EQUIV          3066
#define  LASTSDOS20EQUIV          4155
#define FIRSTCPUEQUIV             4156
#define  LASTCPUEQUIV             4169
#define FIRSTP1DOSEQUIV           4170
#define  LASTP1DOSEQUIV           4224
#define FIRSTGAMEEQUIV            4225
#define EQUIVALENTS               (FIRSTGAMEEQUIV + 2554 + 1)

#define FIRSTHOBBYMODULECODECOMMENT  -1
#define  LASTHOBBYMODULECODECOMMENT  -1
#define FIRSTCODECOMMENT10           -1 // Selbst
#define  LASTCODECOMMENT10           -1
#define FIRSTPIPBUG1CODECOMMENT       0
#define  LASTPIPBUG1CODECOMMENT      66
#define FIRSTPHILIPSCODECOMMENT      67
#define LASTPHILIPSCODECOMMENT      527
// more here
#define FIRSTPHUNSYCODECOMMENT      921
#define  LASTNONMDCRCODECOMMENT    1068
#define FIRSTMDCRCODECOMMENT       1069
#define  LASTMDCRCODECOMMENT       1292
#define FIRSTBINBUG36CODECOMMENT   1293
#define  LASTBINBUG36CODECOMMENT   1450
#define FIRSTCODECOMMENT09         1451 // Selbst
#define  LASTCODECOMMENT09         1608
#define FIRSTCODECOMMENT20         1609
#define  LASTCODECOMMENT20         1712
// more here
#define FIRSTGBUGCODECOMMENT       3739
#define  LASTGBUGCODECOMMENT       3801
#define FIRSTVHSDOSCODECOMMENT     3802
#define  LASTVHSDOSCODECOMMENT     4136
#define FIRSTMICRODOSCODECOMMENT   4137
#define  LASTMICRODOSCODECOMMENT   4500
#define FIRSTACOSCODECOMMENT       4501
#define  LASTACOSCODECOMMENT       4891
#define FIRSTBINBUG61CODECOMMENT   4892
#define  LASTBINBUG61CODECOMMENT   5071
#define FIRSTPIPBUG2CODECOMMENT    5072
#define  LASTPIPBUG2CODECOMMENT    5205
// more here

#define FIRSTDATACOMMENT10           -1 // Selbst
#define  LASTDATACOMMENT10           -1
#define FIRSTPHILIPSDATACOMMENT       0
#define FIRSTHOBBYMODULEDATACOMMENT  74
#define  LASTHOBBYMODULEDATACOMMENT 120
#define  LASTPHILIPSDATACOMMENT     120
// more here
#define FIRSTPHUNSYDATACOMMENT      131
#define  LASTNONMDCRDATACOMMENT     223
#define  LASTMDCRDATACOMMENT        249
#define FIRSTPIPBUG1DATACOMMENT     250
#define  LASTPIPBUG1DATACOMMENT     250
#define FIRSTBINBUG36DATACOMMENT    251
#define  LASTBINBUG36DATACOMMENT    266
#define FIRSTDATACOMMENT09          267 // Selbst
#define  LASTDATACOMMENT09          283
#define FIRSTDATACOMMENT20          284
#define  LASTDATACOMMENT20          304
// SI50 and Arcadia hardware are here
#define FIRSTGBUGDATACOMMENT        383
#define  LASTGBUGDATACOMMENT        412
#define FIRSTVHSDOSDATACOMMENT      413
#define  LASTVHSDOSDATACOMMENT      475
#define FIRSTMICRODOSDATACOMMENT    476
#define  LASTMICRODOSDATACOMMENT    545
#define FIRSTACOSDATACOMMENT        546
#define  LASTACOSDATACOMMENT        592
#define FIRSTBINBUG61DATACOMMENT    593
#define  LASTBINBUG61DATACOMMENT    618
#define FIRSTPIPBUG2DATACOMMENT     619
#define  LASTPIPBUG2DATACOMMENT     631
#define FIRSTP1DOSDATACOMMENT       632
#define  LASTP1DOSDATACOMMENT       650
#define FIRSTGAMEDATACOMMENT        651

#define BUILTIN_MIN                 2
#define BUILTIN_MAX                10
#define FOUNDGAMES_MAX (KNOWNGAMES + BUILTIN_MAX) // this can be theoretically exceeded
/* +BUILTIN_MAX is because 1 for root
                           1 for parent
                           6 for coin-ops
                           2 for Pong */

#define SORTBYNAME      0
#define SORTBYMACHINE   1

#define DUMP_GOOD       0
#define DUMP_GOODOBS    DUMP_GOOD
#define DUMP_GOODENH    DUMP_GOOD
#define DUMP_OK         1
#define DUMP_OKOBS      DUMP_OK
#define DUMP_OKENH      DUMP_OK
#define DUMP_OKOBSENH   DUMP_OK
#define DUMP_SUSPECT    2
#define DUMP_SUSOBS     DUMP_SUSPECT
#define DUMP_BAD        3
#define DUMP_BADOBS     DUMP_BAD

#define CONTROLLER_NONE      0
#define CONTROLLER_TRACKBALL 1
#define CONTROLLER_1STDJOY   2
#define CONTROLLER_2NDDJOY   3
#define CONTROLLER_2NDAJOY   4
#define CONTROLLER_1STDPAD   5
#define CONTROLLER_2NDDPAD   6
#define CONTROLLER_1STAPAD   7
#define CONTROLLER_2NDAPAD   8

// Arcadia
#define A_GOLFPOS1              0
#define BASEBALLPOS             1
#define ESCAPEPOS               2
#define MACROSSPOS              3
#define PARASHOOTERPOS          4
#define ROBOTKILLERPOS          5
#define SPACESQUADRON1POS       6
#define SPACESQUADRON2POS       7
#define SPACEVULTURESPOS        8
#define TETRISPOS               9
#define _2DSOCCERPOS           11
#define _3DSOCCERAPOS          12
#define _3DSOCCERBPOS          13
#define ALIENINVPOS            14
#define ASTROINVPOS            15
#define AUTORACEPOS            16
#define A_BASKETBALLPOS        17
#define BATTLEPOS              18
#define A_BLACKJACKPOS         19
#define A_BOWLINGPOS           20
#define A_BOXINGPOS            21
#define BRAINQUIZPOS           22
#define BREAKAWAYPOS           23
#define A_CAPTUREPOS           24
#define CATTRAXPOS             25
#define A_CIRCUSPOS            26
#define A_COMBATPOS            27
#define CRAZYCLIMBERPOS        28
#define CRAZYGOBBLERPOS        29
#define DORAEMONPOS            30
#define DRSLUMPPOS             31
#define FUNKYFISHPOS           32
#define GRIDIRON1POS           33
#define HOBOPOS1               34
#define A_HORSERACINGPOS       35
#define JUMPBUG1POS            36
#define JUMPBUG2POS            37
#define JUNGLERPOS             38
#define MISSILEWARPOS          39
#define MONACOPOS              40
#define NIBBLEMENPOS           41
#define OCEANBATTLEPOS         42
#define PLEIADESPOS            43
#define R2DTANKPOS             44
#define REDCLASHPOS            45
#define ROUTE16POS             46
#define SPACEATTACKAPOS        47
#define SPACEATTACKBPOS        48
#define SPACEATTACKCPOS        49
#define SPACEMISSIONPOS        50
#define SPIDERSPOS             51
#define SPACERAIDERSPOS        52
#define STARCHESSPOS           53
#define SUPERGOBBLERPOS        54
#define TANKSALOTPOS           55
#define TENNISPOS              56
#define THEENDPOS1             57
#define TURTLESPOS             58
#define VIDLEXEGPOS            59
#define VIDLEXGEPOS            60
#define ALIENINV1POS           61
#define ALIENINV2POS           62
#define ALIENINV3POS           63
#define ALIENINV4POS           64
#define A_GOLFPOS2             65
#define STARCHESSENHPOS        66
#define MOTHERSHIPPOS          78
#define THEENDPOS2             90
#define HOBOPOS2               91
#define _3DSOCCERENHPOS        93
#define GRIDIRON2POS           94
#define SUPERBUG1POS           97
#define GUNDAMPOS              98
#define JTRON121POS            99
#define SUPERBUG2POS          102
#define FROGGER1POS           104
#define _3DATTACKPOS          105
#define IKANASHIPOS           111
#define ARCADIASTUBPOS        118
#define ASTROINVODPOS         119
#define AUTORACEODPOS         120
#define A_COMBATODPOS         121
#define A_GOLFODPOS           122
#define REDCLASHODPOS         123
#define _2DSOCCERODPOS        124
#define SPIDERSODPOS          125
#define JOURNEYPOS            126
#define SPACEBUSTERPOS        127
#define JTRON10POS            128
#define JTRON11POS            129
#define FROGGER2POS           830
#define FROGGER3POS           849
#define HOMERUNPOS            855

// Interton
#define SUPERINVPOS           131
#define INTERTONSTUBPOS       132
#define INTELLIGENCE1POS      133
#define SUPERMAZEPOS          134
#define MONSTERMANPOS         135
#define COMECOMEPOS           136
#define COMEFRUTASPOS1        137
#define COMEFRUTASPOS2        139
#define KABOOM1POS            141
#define KABOOM2POS            142
#define _4INAROWPOS           144
#define AIRSEABATTLEPOS       145
#define BACKGAMMONPOS         146
#define I_BLACKJACKPOS        147
#define I_BOWLINGPOS          148
#define I_BOXINGPOS           149
#define I_CAPTUREPOS          150
#define CASINOPOS             152
#define CHESS1POS             153
#define CHESS2POS             154
#define I_CIRCUSPOS           155
#define COCKPITPOS            156
#define MASTERMINDPOS         157
#define I_COMBATBPOS          159
#define DRAUGHTSPOS           160
#define I_GOLFPOS             161
#define CARRACESPOS           162
#define HEADONPOS             163
#define HIPPODROMEPOS         164
#define HUNTINGPOS            165
#define HYPERSPACEPOS         166
#define INVADERAPOS           167
#define INVADERBPOS           168
#define LASERATTACKPOS        169
#define I_MATH1POS            170
#define I_MATH2POS            171
#define METROPOLISPOS         172
#define MOTOCROSSPOS          173
#define MUSICALGAMESPOS       174
#define PADDLEGAMESPOS        175
#define PINBALLBPOS           177
#define SOCCERBPOS            181
#define SOLITAIREPOS          182
#define SPACEWARPOS           183
#define SUPERSPACEPOS         184
#define MEMORY1POS            185
#define WINTERSPORTSPOS       186
#define LEAPFROGPOS           188
#define SUPERKNOCKOUTPOS      193
#define AIRSEAATTACKPOS       194
#define COMEFRUTASPOS3        201
#define I_TETRISPOS1          206
#define I_TETRISPOS2          207
#define I_TETRISPOS3          208
// Elektor
#define MAZESPOS              236
#define CIRCLEDRIVEPOS1       237
#define COSMICADVENTUREPOS1   249
#define COSMICADVENTUREPOS2   253
#define ROCKETSHOOTINGPOS     263
#define SPACESHOOTOUTPOS      268
#define UFOSHOOTINGPOS        286
#define E_PINBALLPOS          290
#define HELICOPTERPOS         291
#define ATTACKFROMSPACEPOS    295
#define BURSTINGBALLOONSPOS   297
#define ASTEROIDSPOS          303
#define OMEGALANDINGPOS       304
#define BREAKOUTPOS           305
#define TINYTIMPOS            306
#define SUBMARINESRACINGPOS   311
#define CIRCLEDRIVEPOS2       312
#define QUEENPOS              354
#define SPACEBATTLEPOS        355
#define E_MATH1POS            356
#define PENALTYPOS            391
#define MOONLANDING1POS       392
#define MOONLANDING2POS       393
#define E_CARRACESPOS         397
#define ENTERPRISE1POS1       402
#define ENTERPRISE1POS2       403
#define HAMISH1POS1           404
#define LAUNCHINGPOS1         405
#define LAUNCHINGPOS2         409
#define HAMISH1POS2           411
#define HAMISH1POS3           415
#define ENTERPRISE3POS        423
#define CHICKEN1POS           424
#define CHICKEN2POS           425
#define CHICKEN3POS           438
// PIPBUG
#define PIPBUG_ASTROTREKPOS   453
#define PIPBUG_MORSEPOS       457
#define MINIDISASM1POS        460
#define MINIDISASM2POS        461
#define MAGNETOMETERPOS       462
#define MUSIC1POS             479
#define MUSIC2POS             488
#define PIPBUG_MASTERMIND1POS 501
#define LINEARISATIEPOS1      510
#define BIORHYTHM300POS       520
#define PIPBUG_MASTERMIND2POS 523
#define FURNACEPOS1           524
#define PIPBUG_LIFEMCPOS      525
#define RYTMONPOS             555
#define FURNACEPOS2           851
#define FURNACEPOS3           852
#define LINEARISATIEPOS2      853
// BINBUG
#define TARGETPOS             580
#define BINBUG_ASTROTREKPOS1  582
#define BINBUG_ASTROTREKPOS2  583
#define HEXKEYBOARDPOS        590
// SI50
#define SI50_THEMEPOS         616
// CD2650
#define CD2650_MORSEPOS       635
// PHUNSY
#define BELMACHPOS            652
#define BELMACH0POS           653
#define PHUNSY_THEMEPOS       654
#define PIANOPOS              655
// Selbst
#define OSCILLATORPOS         796
#define DICEPOS               802
#define DICEGAMEPOS           805
#define LOTTOPOS              808
#define SELBST_MORSEPOS       809
#define METRONOMPOS           812
// MIKIT
#define MIKITMUSICPOS         820

// ASCII values
#define NUL                 0 // same as EOS
#define STX                 2 // Ctrl+B
#define ETX                 3 // Ctrl+C
#define EOT                 4 // Ctrl+D
#define BEL                 7 // Ctrl+G
#define BS                  8 // Ctrl+H (left)
#define HT                  9 // Ctrl+I
#define LF                 10 // Ctrl+J (down)
#define VT                 11 // Ctrl+K (up)
#define FF                 12 // Ctrl+L (right)
#define CR                 13 // Ctrl+M
#define DC1                17 // Ctrl+Q
#define DC2                18 // Ctrl+R
#define DC3                19 // Ctrl+S
#define DC4                20 // Ctrl+T
#define NAK                21 // Ctrl+U
#define CAN                24 // Ctrl+X
#define EM                 25 // Ctrl+Y
#define SUB                26 // Ctrl+Z
#define ESC                27 // Ctrl+Shift+K
#define FS                 28 // Ctrl+Shift+L
#define GS                 29 // Ctrl+Shift+M
#define QUOTE            '\''
#define QUOTES           '\"'
#define DEL               127
#define NC                128 // No Code (a value that is not produced by any Ctrl-keys)
#define BRK               129 // BReaK
#define HI                130 // Here Is
#define DATE              131

#define UNLIT                DARKBLUE
// BLACK, DARKRED, DARKBLUE, DARKGREEN are supported
// (but DARKGREEN is too light)

#define PIPBUG_BLANK     0x80
#define TWIN_BLANK       0x20

#define PORTC             256
#define PORTD             257

// these must correspond with the ordering of the relevant IDC_#? gadgets and constants
#define INTSEL_ACLINE        0
#define INTSEL_KYBD          1
#define INTSEL_DEFAULT       INTSEL_KYBD
#define PARALLEL_MEMMAPPED   0
#define PARALLEL_EXTENDED    1
#define PARALLEL_NONEXTENDED 2
#define PARALLEL_DEFAULT     PARALLEL_EXTENDED
#define INTDIR_DIRECT        0
#define INTDIR_INDIRECT      1
#define INTDIR_DEFAULT       INTDIR_DIRECT

//  0..50: Arcadia
// 51..56: Coin-ops
#define HS_ASTROWARS_POS    51
#define HS_GALAXIA_POS      52
#define HS_LASERBATTLE_POS  53
#define HS_LAZARIAN_POS     54
#define HS_MALZAK1_POS      55
#define HS_MALZAK2_POS      56
// 57..62: Pong
#define HS_PRACTICE_POS     57
#define HS_RIFLE1_POS       58
#define HS_RIFLE2_POS       59
#define HS_PRACTICE2_POS    60
#define HS_BBPRACTICE_POS   61
#define HS_TARGET1_POS      62
// 63..85: Type-right
#define HS_CLASS1_POS       63
#define HS_CLASS2_POS       64
#define HS_CLASS3_POS       65
#define HS_CLASS4_POS       66
#define HS_CLASS5_POS       67
#define HS_CLASS6LEVEL1_POS 68
#define HS_CLASS6LEVEL2_POS 69
#define HS_CLASS6LEVEL3_POS 70
#define HS_CLASS6LEVEL4_POS 71
#define HS_CLASS7LEVEL1_POS 72
#define HS_CLASS7LEVEL2_POS 73
#define HS_CLASS7LEVEL3_POS 74
#define HS_CLASS7LEVEL4_POS 75
#define HS_CLASS8LEVEL1_POS 76
#define HS_CLASS8LEVEL2_POS 77
#define HS_CLASS8LEVEL3_POS 78
#define HS_CLASS8LEVEL4_POS 79
#define HS_CLASS9LEVEL1_POS 80
#define HS_CLASS9LEVEL2_POS 81
#define HS_CLASS9LEVEL3_POS 82
#define HS_CLASS9LEVEL4_POS 83
#define HS_GAME1_POS        84
#define HS_GAME2_POS        85
#define HISCORES            86 // ie. [0..85]

#define LLL(a, b) (STRPTR) GetCatalogStr(CatalogPtr, a, b)

#define BASE_BINARY  0
#define BASE_OCTAL   1
#define BASE_DECIMAL 2
#define BASE_HEX     3
// special ones...
#define BASE_CHAR    4
#define BASE_EBCDIC  5
#define BASE_LABEL   6

// computer languages
#define CL_MISC   0 // miscellaneous
#define CL_MC     1 // machine code
#define CL_TEXT   2 // text
#define CL_MWB    3 // MicroWorld BASIC (PHUNSY)
#define CL_LOTEL  4 // ?
#define CL_FORTH  5
#define CL_12KB   6 // 12K BASIC (CD2650)
#define CL_MB     7 // 2650 Micro BASIC (PIPBUG)
#define CL_C2001  8 // Compiler 2001 aka Lightning BASIC (Arcadia)
#define CL_8KB13  9 // 8K BASIC 1.3 (CD2650). Nothing known that uses 8K BASIC 1.0
#define CL_TCT   10 // TCT BASIC (PIPBUG)
#define CL_DATA  CL_MISC

#define UBANKSIZE   0x800
#define UBANK0     0x8000
#define UBANK1     0x8800
#define UBANK2     0x9000
#define UBANK3     0x9800
#define UBANK4     0xA000
#define UBANK5     0xA800
#define UBANK6     0xB000
#define UBANK7     0xB800
#define UBANK8     0xC000
#define UBANK9     0xC800
#define UBANKA     0xD000
#define UBANKB     0xD800
#define UBANKC     0xE000
#define UBANKD     0xE800
#define UBANKE     0xF000
#define UBANKF     0xF800

#define QBANKSIZE  0x4000
#define QBANK0    0x10000
#define QBANK1    0x14000
#define QBANK2    0x18000
#define QBANK3    0x1C000
#define QBANK4    0x20000
#define QBANK5    0x24000
#define QBANK6    0x28000
#define QBANK7    0x2C000
#define QBANK8    0x30000
#define QBANK9    0x34000
#define QBANKA    0x38000
#define QBANKB    0x3C000
#define QBANKC    0x40000
#define QBANKD    0x44000
#define QBANKE    0x48000
#define QBANKF    0x4C000

#define ARCADIA_FIRSTMONGAD     0
#define ARCADIA_LASTMONGAD    112
#define PVI1ST_FIRSTMONGAD    113
#define PVI1ST_LASTMONGAD     140
#define E_FIRSTMONGAD         141
#define I_FIRSTMONGAD         143
#define IE_LASTMONGAD         150
#define INTERTON_FIRSTMONGAD  151
#define INTERTON_LASTMONGAD   187
#define PVI2ND_FIRSTMONGAD    188
#define PVI2ND_LASTMONGAD     215
#define PVI3RD_FIRSTMONGAD    216
#define PVI3RD_LASTMONGAD     243
#define XVI_FIRSTMONGAD         0
#define XVI_LASTMONGAD        243
#define PSGS_FIRSTMONGAD      244
#define FIRSTDOUBLEBYTEPSG    260
#define MONGAD_PITCHA1        260
#define MONGAD_PITCHA2        261
#define MONGAD_PITCHB1        262
#define MONGAD_PITCHB2        263
#define MONGAD_PITCHC1        264
#define MONGAD_PITCHC2        265
#define MONGAD_PERIOD1        266
#define MONGAD_PERIOD2        267
#define LASTDOUBLEBYTEPSG     267
#define PSGS_LASTMONGAD       267
#define MONGAD_R0             268
#define CPU_FIRSTMONGAD       MONGAD_R0
#define MONGAD_R1             269
#define MONGAD_R2             270
#define MONGAD_R3             271
#define MONGAD_R4             272
#define MONGAD_R5             273
#define MONGAD_R6             274
#define MONGAD_RAS0           275
#define MONGAD_RAS1           276
#define MONGAD_RAS2           277
#define MONGAD_RAS3           278
#define MONGAD_RAS4           279
#define MONGAD_RAS5           280
#define MONGAD_RAS6           281
#define MONGAD_RAS7           282
#define MONGAD_S              283
#define MONGAD_F              284
#define MONGAD_II             285
#define MONGAD_UF1            286
#define MONGAD_UF2            287
#define MONGAD_SP             288
#define MONGAD_CC             289
#define MONGAD_IDC            290
#define MONGAD_RS             291
#define MONGAD_WC             292
#define MONGAD_OVF            293
#define MONGAD_COM            294
#define MONGAD_C              295
#define MONGAD_IAR            296
#define MONGAD_OPCODE         297
#define MONGAD_SLAVE_R0       298
#define MONGAD_SLAVE_R1       299
#define MONGAD_SLAVE_R2       300
#define MONGAD_SLAVE_R3       301
#define MONGAD_SLAVE_R4       302
#define MONGAD_SLAVE_R5       303
#define MONGAD_SLAVE_R6       304
#define MONGAD_SLAVE_RAS0     305
#define MONGAD_SLAVE_RAS1     306
#define MONGAD_SLAVE_RAS2     307
#define MONGAD_SLAVE_RAS3     308
#define MONGAD_SLAVE_RAS4     309
#define MONGAD_SLAVE_RAS5     310
#define MONGAD_SLAVE_RAS6     311
#define MONGAD_SLAVE_RAS7     312
#define MONGAD_SLAVE_S        313
#define MONGAD_SLAVE_F        314
#define MONGAD_SLAVE_II       315
#define MONGAD_SLAVE_UF1      316
#define MONGAD_SLAVE_UF2      317
#define MONGAD_SLAVE_SP       318
#define MONGAD_SLAVE_CC       319
#define MONGAD_SLAVE_IDC      320
#define MONGAD_SLAVE_RS       321
#define MONGAD_SLAVE_WC       322
#define MONGAD_SLAVE_OVF      323
#define MONGAD_SLAVE_COM      324
#define MONGAD_SLAVE_C        325
#define MONGAD_SLAVE_IAR      326
#define MONGAD_SLAVE_OPCODE   327
#define MONGAD_CLOCK          328
#define MONGAD_FRAME          329
#define MONGAD_TIME           330
#define MONGAD_X              331
#define MONGAD_Y              332
#define MONGAD_WPM_TR         333
#define MONGAD_ACCURACY       334
#define MONGAD_ERRORS         335
#define MONGAD_EXPECTING      336
#define CPU_LASTMONGAD        MONGAD_EXPECTING
#define MONITORGADS           338 // counting from 1 (last array index is MONITORGADS-1)

#define VIEWAS_FPS              0
#define VIEWAS_PERCENTS         1
#define VIEWAS_KHZ              2

// these are a maximum of 4 bits wide
#define COND_UN                 0
#define COND_LT                 1
#define COND_LE                 2
#define COND_EQ                 3
#define COND_NE                 4
#define COND_GE                 5
#define COND_GT                 6
#define COND_MASK               7

#define NAME_R0                 0
#define NAME_R1                 1
#define NAME_R3                 3
#define NAME_R4                 4
#define NAME_R6                 6
// 7..14 are NAME_RAS0..7
#define NAME_S                 15
#define NAME_F                 16
#define NAME_II                17
#define NAME_SP                20
// 21 is NAME_CC
#define NAME_IDC               22
#define NAME_RS                23
#define NAME_WC                24
#define NAME_OVF               25
#define NAME_COM               26
#define NAME_C                 27
// 28 is NAME_IAR
// 29 is NAME_OPCODE
#define CPUTIPS                30 // they go from 0..CPUTIPS-1

#define HASDISK                 machines[machine].drives
#define HASPAPERTAPE            (machine == PIPBUG || machine == BINBUG || machine == TWIN)
#define TAPABLE                 (machine == ELEKTOR || machine == INSTRUCTOR || machine == PIPBUG || machine == BINBUG || machine == CD2650 || machine == PHUNSY || machine == SELBST)
#define LABELLIMIT             40 // reassembling Central Data DOS requires this to be >= 33

#define BINBUG_35               0
#define BINBUG_36               1
#define BINBUG_61               2
#define BINBUG_GBUG             3
#define BINBUG_MIKEBUG          4
#define BINBUG_MYBUG            5

#define DOS_MICRODOS            0
#define DOS_VHSDOS              1
#define DOS_NOBINBUGDOS         2

#define DOS_CDDOS               0
#define DOS_P1DOS               1
#define DOS_NOCD2650DOS         2

#define FILTER_3D               0
#define FILTER_HQX              1
#define FILTER_SCALE2X          2
#define FILTER_NONE             3

#define WEST( x) (x >=                            1 ? x - 1 : x)
#define EAST( x) (x <= machines[machine].width  - 2 ? x + 1 : x)
#define NORTH(y) (y >=                            1 ? y - 1 : y)
#define SOUTH(y) (y <= machines[machine].height - 2 ? y + 1 : y)

#define OCTAVE     12
#define NOTE_A0     0
#define NOTE_AS0    1
#define NOTE_B0     2
#define NOTE_C1     3
#define NOTE_CS1    4
#define NOTE_D1     5
#define NOTE_DS1    6
#define NOTE_E1     7
#define NOTE_F1     8
#define NOTE_FS1    9
#define NOTE_G1    10
#define NOTE_GS1   11
#define NOTE_A1    12
#define NOTE_AS1   13
#define NOTE_B1    14
#define NOTE_C2  (NOTE_C1  +  OCTAVE     ) // 15
#define NOTE_CS2 (NOTE_CS1 +  OCTAVE     )
#define NOTE_D2  (NOTE_D1  +  OCTAVE     )
#define NOTE_DS2 (NOTE_DS1 +  OCTAVE     )
#define NOTE_E2  (NOTE_E1  +  OCTAVE     )
#define NOTE_F2  (NOTE_F1  +  OCTAVE     ) // 20
#define NOTE_FS2 (NOTE_FS1 +  OCTAVE     )
#define NOTE_G2  (NOTE_G1  +  OCTAVE     )
#define NOTE_GS2 (NOTE_GS1 +  OCTAVE     )
#define NOTE_A2  (NOTE_A1  +  OCTAVE     )
#define NOTE_AS2 (NOTE_AS1 +  OCTAVE     ) // 25
#define NOTE_B2  (NOTE_B1  +  OCTAVE     )
#define NOTE_C3  (NOTE_C1  + (OCTAVE * 2))
#define NOTE_CS3 (NOTE_CS1 + (OCTAVE * 2))
#define NOTE_D3  (NOTE_D1  + (OCTAVE * 2))
#define NOTE_DS3 (NOTE_DS1 + (OCTAVE * 2)) // 30
#define NOTE_E3  (NOTE_E1  + (OCTAVE * 2))
#define NOTE_F3  (NOTE_F1  + (OCTAVE * 2))
#define NOTE_FS3 (NOTE_FS1 + (OCTAVE * 2))
#define NOTE_G3  (NOTE_G1  + (OCTAVE * 2))
#define NOTE_GS3 (NOTE_GS1 + (OCTAVE * 2)) // 35
#define NOTE_A3  (NOTE_A1  + (OCTAVE * 2))
#define NOTE_AS3 (NOTE_AS1 + (OCTAVE * 2))
#define NOTE_B3  (NOTE_B1  + (OCTAVE * 2))
#define NOTE_C4  (NOTE_C1  + (OCTAVE * 3))
#define NOTE_CS4 (NOTE_CS1 + (OCTAVE * 3)) // 40
#define NOTE_D4  (NOTE_D1  + (OCTAVE * 3))
#define NOTE_DS4 (NOTE_DS1 + (OCTAVE * 3))
#define NOTE_E4  (NOTE_E1  + (OCTAVE * 3))
#define NOTE_F4  (NOTE_F1  + (OCTAVE * 3))
#define NOTE_FS4 (NOTE_FS1 + (OCTAVE * 3))
#define NOTE_G4  (NOTE_G1  + (OCTAVE * 3))
#define NOTE_GS4 (NOTE_GS1 + (OCTAVE * 3))
#define NOTE_A4  (NOTE_A1  + (OCTAVE * 3))
#define NOTE_AS4 (NOTE_AS1 + (OCTAVE * 3))
#define NOTE_B4  (NOTE_B1  + (OCTAVE * 3))
#define NOTE_C5  (NOTE_C1  + (OCTAVE * 4))
#define NOTE_CS5 (NOTE_CS1 + (OCTAVE * 4))
#define NOTE_D5  (NOTE_D1  + (OCTAVE * 4))
#define NOTE_DS5 (NOTE_DS1 + (OCTAVE * 4))
#define NOTE_E5  (NOTE_E1  + (OCTAVE * 4))
#define NOTE_F5  (NOTE_F1  + (OCTAVE * 4))
#define NOTE_FS5 (NOTE_FS1 + (OCTAVE * 4))
#define NOTE_G5  (NOTE_G1  + (OCTAVE * 4))
#define NOTE_GS5 (NOTE_GS1 + (OCTAVE * 4))
#define NOTE_A5  (NOTE_A1  + (OCTAVE * 4))
#define NOTE_AS5 (NOTE_AS1 + (OCTAVE * 4))
#define NOTE_B5  (NOTE_B1  + (OCTAVE * 4))
#define NOTE_C6  (NOTE_C1  + (OCTAVE * 5))
#define NOTE_CS6 (NOTE_CS1 + (OCTAVE * 5))
#define NOTE_D6  (NOTE_D1  + (OCTAVE * 5))
#define NOTE_DS6 (NOTE_DS1 + (OCTAVE * 5))
#define NOTE_E6  (NOTE_E1  + (OCTAVE * 5))
#define NOTE_F6  (NOTE_F1  + (OCTAVE * 5))
#define NOTE_FS6 (NOTE_FS1 + (OCTAVE * 5))
#define NOTE_G6  (NOTE_G1  + (OCTAVE * 5))
#define NOTE_GS6 (NOTE_GS1 + (OCTAVE * 5))
#define NOTE_A6  (NOTE_A1  + (OCTAVE * 5))
#define NOTE_AS6 (NOTE_AS1 + (OCTAVE * 5))
#define NOTE_B6  (NOTE_B1  + (OCTAVE * 5))
#define NOTE_C7  (NOTE_C1  + (OCTAVE * 6))
#define NOTE_CS7 (NOTE_CS1 + (OCTAVE * 6))
#define NOTE_D7  (NOTE_D1  + (OCTAVE * 6))
#define NOTE_DS7 (NOTE_DS1 + (OCTAVE * 6))
#define NOTE_E7  (NOTE_E1  + (OCTAVE * 6))
#define NOTE_F7  (NOTE_F1  + (OCTAVE * 6))
#define NOTE_FS7 (NOTE_FS1 + (OCTAVE * 6))
#define NOTE_G7  (NOTE_G1  + (OCTAVE * 6))
#define NOTE_GS7 (NOTE_GS1 + (OCTAVE * 6))
#define NOTE_A7  (NOTE_A1  + (OCTAVE * 6))
#define NOTE_AS7 (NOTE_AS1 + (OCTAVE * 6))
#define NOTE_B7  (NOTE_B1  + (OCTAVE * 6))
#define NOTE_C8  (NOTE_C1  + (OCTAVE * 7))
#define NOTE_CS8 (NOTE_CS1 + (OCTAVE * 7))
#define NOTE_D8  (NOTE_D1  + (OCTAVE * 7))
#define NOTE_DS8 (NOTE_DS1 + (OCTAVE * 7))
#define NOTE_E8  (NOTE_E1  + (OCTAVE * 7))
#define NOTE_F8  (NOTE_F1  + (OCTAVE * 7))
#define NOTE_FS8 (NOTE_FS1 + (OCTAVE * 7))
#define NOTE_G8  (NOTE_G1  + (OCTAVE * 7))
#define NOTE_GS8 (NOTE_GS1 + (OCTAVE * 7))
#define NOTE_A8  (NOTE_A1  + (OCTAVE * 7))
#define NOTE_AS8 (NOTE_AS1 + (OCTAVE * 7))
#define NOTE_B8  (NOTE_B1  + (OCTAVE * 7))
#define NOTE_C9  (NOTE_C1  + (OCTAVE * 8)) // 99
#define NOTES     100 // 0..99 are notes, 100 is rest
#define NOTE_REST NOTES

#define NOTES_FIRST_TOPY     13
#define NOTES_FIRST_BOTTOMY  21
#define NOTES_SECOND_TOPY    22
#define NOTES_SECOND_BOTTOMY 28

#define HZ_REST (float)   0.000
#define HZ_A0  (float)   27.500
#define HZ_AS0 (float)   29.135
#define HZ_B0  (float)   30.868
#define HZ_C1  (float)   32.703
#define HZ_CS1 (float)   34.648
#define HZ_D1  (float)   36.708
#define HZ_DS1 (float)   38.891
#define HZ_E1  (float)   41.203
#define HZ_F1  (float)   43.654
#define HZ_FS1 (float)   46.249
#define HZ_G1  (float)   48.999
#define HZ_GS1 (float)   51.913
#define HZ_A1  (float)   55.000
#define HZ_AS1 (float)   58.270
#define HZ_B1  (float)   61.735
#define HZ_C2  (float)   65.406
#define HZ_CS2 (float)   69.296
#define HZ_D2  (float)   73.416
#define HZ_DS2 (float)   77.782
#define HZ_E2  (float)   82.407
#define HZ_F2  (float)   87.307
#define HZ_FS2 (float)   92.499
#define HZ_G2  (float)   97.999
#define HZ_GS2 (float)  103.826
#define HZ_A2  (float)  110.000
#define HZ_AS2 (float)  116.541
#define HZ_B2  (float)  123.471
#define HZ_C3  (float)  130.813
#define HZ_CS3 (float)  138.591
#define HZ_D3  (float)  146.832
#define HZ_DS3 (float)  155.563
#define HZ_E3  (float)  164.814
#define HZ_F3  (float)  174.614
#define HZ_FS3 (float)  184.997
#define HZ_G3  (float)  195.998
#define HZ_GS3 (float)  207.652
#define HZ_A3  (float)  220.000
#define HZ_AS3 (float)  233.082
#define HZ_B3  (float)  246.942
#define HZ_C4  (float)  261.626
#define HZ_CS4 (float)  277.183
#define HZ_D4  (float)  293.665
#define HZ_DS4 (float)  311.127
#define HZ_E4  (float)  329.628
#define HZ_F4  (float)  349.238
#define HZ_FS4 (float)  369.994
#define HZ_G4  (float)  391.995
#define HZ_GS4 (float)  415.305
#define HZ_A4  (float)  440.000
#define HZ_AS4 (float)  466.164
#define HZ_B4  (float)  493.883
#define HZ_C5  (float)  523.251
#define HZ_CS5 (float)  554.365
#define HZ_D5  (float)  587.330
#define HZ_DS5 (float)  622.254
#define HZ_E5  (float)  659.255
#define HZ_F5  (float)  698.456
#define HZ_FS5 (float)  739.989
#define HZ_G5  (float)  783.991
#define HZ_GS5 (float)  830.609
#define HZ_A5  (float)  880.000
#define HZ_AS5 (float)  932.328
#define HZ_B5  (float)  987.767
#define HZ_C6  (float) 1046.502
#define HZ_CS6 (float) 1108.731
#define HZ_D6  (float) 1174.659
#define HZ_DS6 (float) 1244.508
#define HZ_E6  (float) 1318.510
#define HZ_F6  (float) 1396.913
#define HZ_FS6 (float) 1479.978
#define HZ_G6  (float) 1567.982
#define HZ_GS6 (float) 1661.219
#define HZ_A6  (float) 1760.000
#define HZ_AS6 (float) 1864.655
#define HZ_B6  (float) 1975.533
#define HZ_C7  (float) 2093.005
#define HZ_CS7 (float) 2217.461
#define HZ_D7  (float) 2349.318
#define HZ_DS7 (float) 2489.016
#define HZ_E7  (float) 2637.020
#define HZ_F7  (float) 2793.826
#define HZ_FS7 (float) 2959.955
#define HZ_G7  (float) 3135.963
#define HZ_GS7 (float) 3322.438
#define HZ_A7  (float) 3520.000
#define HZ_AS7 (float) 3729.310
#define HZ_B7  (float) 3951.066
#define HZ_C8  (float) 4186.009
#define HZ_CS8 (float) 4434.922
#define HZ_D8  (float) 4698.636
#define HZ_DS8 (float) 4978.032
#define HZ_E8  (float) 5274.041
#define HZ_F8  (float) 5587.652
#define HZ_FS8 (float) 5919.911
#define HZ_G8  (float) 6271.927
#define HZ_GS8 (float) 6644.875
#define HZ_A8  (float) 7040.000
#define HZ_AS8 (float) 7458.620
#define HZ_B8  (float) 7902.133
#define HZ_C9  (float) 8372.018

// bass clef
#define STAVE_G2   5
#define STAVE_A2  10
#define STAVE_B2  15
#define STAVE_C3  20 // low C
#define STAVE_D3  25
#define STAVE_E3  30
#define STAVE_F3  35
#define STAVE_G3  40
#define STAVE_A3  45
#define STAVE_B3  50

#define STAVE_C4  55 // middle C

// treble clef
#define STAVE_D4  60
#define STAVE_E4  65
#define STAVE_F4  70
#define STAVE_G4  75
#define STAVE_A4  80
#define STAVE_B4  85
#define STAVE_C5  90 // high C
#define STAVE_D5  95
#define STAVE_E5 100
#define STAVE_F5 105

#define MIDI_REST 0
#define MIDI_B0  23
#define MIDI_C1  24
#define MIDI_CS1 25
#define MIDI_D1  26
#define MIDI_DS1 27
#define MIDI_E1  28
#define MIDI_F1  29
#define MIDI_FS1 30
#define MIDI_G1  31
#define MIDI_GS1 32
#define MIDI_A1  33
#define MIDI_AS1 34
#define MIDI_B1  35
#define MIDI_C2  36
#define MIDI_CS2 37
#define MIDI_D2  38
#define MIDI_DS2 39
#define MIDI_E2  40
#define MIDI_F2  41
#define MIDI_FS2 42
#define MIDI_G2  43
#define MIDI_GS2 44
#define MIDI_A2  45
#define MIDI_AS2 46
#define MIDI_B2  47
#define MIDI_C3  48
#define MIDI_CS3 49
#define MIDI_D3  50
#define MIDI_DS3 51
#define MIDI_E3  52
#define MIDI_F3  53
#define MIDI_FS3 54
#define MIDI_G3  55
#define MIDI_GS3 56
#define MIDI_A3  57
#define MIDI_AS3 58
#define MIDI_B3  59
#define MIDI_C4  60
#define MIDI_CS4 61
#define MIDI_D4  62
#define MIDI_DS4 63
#define MIDI_E4  64
#define MIDI_F4  65
#define MIDI_FS4 66
#define MIDI_G4  67
#define MIDI_GS4 68
#define MIDI_A4  69
#define MIDI_AS4 70
#define MIDI_B4  71
#define MIDI_C5  72
#define MIDI_CS5 73
#define MIDI_D5  74
#define MIDI_DS5 75
#define MIDI_E5  76
#define MIDI_F5  77
#define MIDI_G5  79
#define MIDI_A5  81
#define MIDI_B5  83
#define MIDI_CS6 85
#define MIDI_E6  88
#define MIDI_G6  91
#define MIDI_B6  95
#define MIDI_E7  100
#define MIDI_B7  107

// Type-right stages
#define STAGE_BOOT                 0
#define STAGE_MENU                 1
#define STAGE_CLASS_SELECTLESSON   2
#define STAGE_CLASS_WAITLESSON     3
#define STAGE_CLASS1_PLAY          4
#define STAGE_GAME_SELECT          5
#define STAGE_GAME_WAIT            6
#define STAGE_GAME_PLAY            7
#define STAGE_CLASS6_SELECTLEVEL   8
#define STAGE_CLASS6_WAITLEVEL     9
#define STAGE_CLASS6_PLAY         10
#define STAGE_CLASS68_WAITRESULTS 11
#define STAGE_GAME_MUSIC          12
#define STAGE_GAME_START          13
#define STAGE_CLASS1_MUSIC        14
#define STAGE_CLASS8_PLAY         15

// PHUNSY MDCR states
#define MDCRSTATE_IDLE            0
#define MDCRSTATE_ATSTART         1
#define MDCRSTATE_ATEND           2
#define MDCRSTATE_FWD             3
#define MDCRSTATE_REV             4

#define MDCRFWDSTATE_IDLE         0
#define MDCRFWDSTATE_NUMREAD_A    1
#define MDCRFWDSTATE_NUMREAD_B    2
#define MDCRFWDSTATE_BLOCKREAD_A  3
#define MDCRFWDSTATE_BLOCKREAD_B  4
#define MDCRFWDSTATE_WRITE        5
#define MDCRFWDSTATE_WRITE_ACTIVE 6
#define MDCRFWDSTATE_INIT         7

// key rectangles
#define KEYRECT_FIRST_ARCADIA    0 // 0..2
#define KEYRECT_FIRST_8550       6 // 6..7
#define KEYRECT_FIRST_8600       8 // 8..9
#define KEYRECT_SI50_SML        10
#define KEYRECT_SI50_BIG        11
#define KEYRECT_ELEKTERMINAL    12
#define KEYRECT_TWIN            13
#define KEYRECT_MIKIT_SML       14
#define KEYRECT_MIKIT_BIG       15
#define KEYRECT_INTERTON_SML    16
#define KEYRECT_INTERTON_BIG    17
#define KEYRECT_ELEKTOR_SML     18
#define KEYRECT_BINBUG          19
#define KEYRECT_MALZAK          20
#define KEYRECT_PHUNSY          21
#define KEYRECT_SELBST          22
#define KEYRECT_TYPERIGHT       23
#define KEYRECT_ASTROWARS       24
#define KEYRECT_GALAXIA         25
#define KEYRECT_LASERBATTLE     26
#define KEYRECT_ELEKTOR_BIG     27
#define KEYRECT_VT100           28
#define KEYRECT_RADIOBULLETIN   29
#define KEYINFOS                30

// crtstatus
#define CRT_DA   0x01 // Data Available
#define CRT_TBMT 0x02 // Transmit Buffer Empty

// I/O port $E9 (aka crtcontrol)
#define CRT_TTY  0x04 // Enable TTY Interrupts

// I/O port $EA
#define PRT_INT  0x20 // enable printer interrupts
#define DISK_INT 0x10 // enable floppy interrupts

// diskcontrol
#define IND 0 // Send Data.    These are what you want the *drive* to do, not the CPU (ie. "send data to CPU").
#define PTS 2 // Printer
#define OTD 4 // Receive Data. These are what you want the *drive* to do, not the CPU (ie. "receive data from CPU").
#define SIO 6 // Start I/O.    Sent at the start of an I/O sequence.
#define RST 7 // Reset.        Sent at the end   of an I/O sequence.

#define TAPEIMAGE_EMPTY     0 //      0
#define TAPEIMAGE_PLAY      1 //  1.. 6
#define TAPEIMAGE_RECORD    7 //  7..12
#define TAPEIMAGE_STOP     13 // 13..18
#define TAPEIMAGE_PROTECT  19 // 19..24
#define SPOOLIMAGES        25 //  0..24
#define TAPEIMAGE_ANIMSTOP 25 // 25..27
#define TAPEIMAGE_ANIMPLAY 28 // 28..30
#define TAPEIMAGE_ANIMREC  31 // 31..33
#define ANIMIMAGES          9 // 25..33
#define TAPEIMAGES         (SPOOLIMAGES + ANIMIMAGES) // 34

// TYPEDEFS---------------------------------------------------------------

typedef signed char          FLAG;    /*  8-bit signed quantity (replaces BOOL) */
typedef signed char          SBYTE;   /*  8-bit signed quantity (replaces Amiga BYTE) */
typedef signed short         SWORD;   /* 16-bit signed quantity (replaces Amiga WORD) */
typedef signed long          SLONG;   /* 32-bit signed quantity (same as LONG) */
#ifdef WIN32
    typedef unsigned __int64 UHUGE;
    typedef signed __int64   SHUGE;
#endif
typedef unsigned int         UINT;

typedef unsigned char        ASCREEN; // we use whatever is fastest

typedef void* unzFile;
typedef struct tm_unz_s
{   UINT tm_sec;
    UINT tm_min;
    UINT tm_hour;
    UINT tm_mday;
    UINT tm_mon;
    UINT tm_year;
} tm_unz;
typedef struct unz_file_info_s
{   ULONG version;
    ULONG version_needed;
    ULONG flag;
    ULONG compression_method;
    ULONG dosDate;
    ULONG crc;
    ULONG compressed_size;
    ULONG uncompressed_size;
    ULONG size_filename;
    ULONG size_file_extra;
    ULONG size_file_comment;
    ULONG disk_num_start;
    ULONG internal_fa;
    ULONG external_fa;
    tm_unz tmu_date;
} unz_file_info;

// EXPORTED STRUCTURES----------------------------------------------------

#ifdef WIN32
struct AssociationStruct
{         int   flag;
    const int   gid;
    const UBYTE kind;
};
#endif

#define SYMLABELS_MAX 2048
EXPORT struct EquivalentStruct
{   UWORD address;
    TEXT  name[LABELLIMIT + 1],
          kind;
    FLAG  listed,
          used;
};

EXPORT struct CanvasStruct
{   const UWORD width,
                height;
    const int   subwindow;
};
EXPORT struct ConditionalStruct
{   ULONG the1st; // token (address or register or I/O port or floppy disk byte)
    UBYTE the2nd; // condition
    UWORD the3rd; // value
};
EXPORT struct DriveStruct
{   FLAG  inserted,
          spinning,
          headloaded;
    UBYTE track,
          sector;
    int   blockoffset,
          viewstart;
    ULONG writeprotect;
    TEXT  fn_disk[MAX_PATH + 1];
    UWORD firstcluster[78];
    int   filesize[78];
    TEXT  filename[100][10 + 1],
          fileext[100][4 + 1];
    UBYTE bam[CLUSTERS_MAX],
          flags[TWIN_DISKSIZE / 8],
          contents[TWIN_DISKSIZE];
};
EXPORT struct FlagNameStruct
{   STRPTR longer[STYLES],
           shorter[STYLES];
};
EXPORT struct IOPortStruct
{   UBYTE                    contents;
    FLAG                     ip;
    struct ConditionalStruct cond;
};
EXPORT struct MonitorStruct
{   ULONG          addr;
    FLAG           used;
    TEXT           string[4 + 1];
    UWORD          contents;
#ifdef WIN32
    HWND           gadget;
    RECT           rect;
#endif
#ifdef AMIGA
    struct Gadget* gadget;
#endif
};
EXPORT struct KeyTableStruct
{   const UWORD  a_address;
    const UBYTE  a_mask;
    const UWORD  ie_address;
    const UBYTE  ie_mask;
    const STRPTR ai_name;
    const STRPTR e_name;
    const UBYTE  p1overlay,
                 p2overlay;
    const ULONG  dapter;
};

EXPORT struct KeyNameStruct
{         STRPTR name;
    const UWORD  unshifted[3],
                 capslock[3],
                 shifted[3],
                 ctrl,
                 ctrlshift,
                 tr_unshifted,
                 tr_shifted;
    const FLAG   reserved;
    const int    gadget;
#ifdef WIN32
    const FLAG   desktop,
                 laptop;
#endif
#ifdef AMIGA
    const FLAG   a600,
                 a1000;
#endif
    const int    tr_index;
};

EXPORT struct KindStruct
{   const STRPTR extension,
                 extension2,
                 pattern;
    const FLAG   auditable;
#ifdef WIN32
    const STRPTR extname,
                 desc;
#endif
#ifdef AMIGA
          STRPTR icon;
#endif
          STRPTR hail;
};
EXPORT struct AuditStruct
{   TEXT  filename[MAX_PATH + 1],
          gamename[96 + 1];
    SWORD whichgame;
    int   glyph;
    SBYTE machine,
          memmap;
};
EXPORT struct CodeCommentStruct
{   UWORD  address;
    STRPTR text;
};
EXPORT struct DataCommentStruct
{   SWORD  first,
           last; // not UWORD!
    STRPTR text;
};
EXPORT struct KnownStruct
{   UBYTE  machine,
           memmap;
    ULONG  crc32,
           crc64_h,
           crc64_l;
    UWORD  size;
    SBYTE  key1,
           key2,
           key3,
           key4,
           up,
           down,
           left,
           right;
    FLAG   analog;
    UBYTE  sensitivity,
           demultiplex,
           spriteflips; // PIPBUG: pipbug_biosver, SI50: s_id
    SBYTE  udgflips,    // SI50: s_is
           firstrow,
           lastrow;
    FLAG   swapped;
    int    flagline;    // Arcadia: flagline, Interton/Elektor: darkenbg, PIPBUG: pipbug_baudrate, BINBUG: vdu, SI50: s_io, CD2650: vdu
    UBYTE  downframes,
           totalframes;
    UBYTE  dump;
    SBYTE  p1sprcol[6],
           p1bgcol[4],  // not UBYTE!
           p2sprcol[6],
           p2bgcol[4];  // not UBYTE!
    STRPTR ggnode,
           name,
           credits,
           reference;
    SWORD  whichoverlay;
    SBYTE  hiscore;     // not UBYTE!
    SWORD  firstequiv,
           lastequiv,
           firstcodecomment,
           lastcodecomment,
           firstdatacomment,
           lastdatacomment;
    UBYTE  complang;
    UWORD  startaddr;
    UBYTE  banked;
    ULONG  the1staddr;
    UWORD  the1stsize;
    ULONG  the2ndaddr;
    UWORD  the2ndsize;
    UBYTE  cpl,
           bios;
// We could conceivably want to also do turbo, etc. in this manner.
};
EXPORT struct MenuStruct
{   const STRPTR rexx;
    const ULONG  desc_id;
    const STRPTR desc_str;
};
EXPORT struct MachineStruct
{   STRPTR cli;
    int    width,
           height,
           memmap,
           region;
    double fps[2];
    int    wide;
    int    firstequiv,
           lastequiv,
           keys;
    UBYTE  readonce,
           cosversion;
    double cpf;
    int    pvis,
           ppc, // pixels per clock
           digits;
    UBYTE  digipos[3];
    int    pvibase;
    FLAG   coinop;
    int    firstcodecomment,
           lastcodecomment,
           firstdatacomment,
           lastdatacomment;
    STRPTR consolekeyname[4];
    int    consolekeyimage[4],
           firsthiscore,
           hiscores;
    FLAG   joystick,
           speech;
    int    disksize,
           drives;
    STRPTR keynames[2][NUMKEYS];
};
EXPORT struct HostMachineStruct
{   STRPTR titlebartext;
#ifdef WIN32
    int    monitor_cpu,
           monitor_psgs,
           monitor_xvi,
           controls;
    FLOAT  distance;
    int    textureheight;
#endif
#ifdef AMIGA
    int    controls;
#endif
};
#define MAX_USERNAMELEN 30
// UNLEN (defined as 256 in lmcons.h) is actual maximum possible username
// length, but we support less so that it fits in gadget
EXPORT struct HiScoreStruct
{   UWORD hipos[4];
    UBYTE length;
    SBYTE nextdigit; // must be signed!
    ULONG score;
    TEXT  thedatetime[14 + 1];
    ULONG played;
    ULONG playedmicro;
    TEXT  username[MAX_USERNAMELEN + 1];
};
EXPORT struct OpcodeStruct
{   STRPTR name,
           format,
           pseudocode,
           fullname,
           rflags,
           wflags,
           cc;
#ifdef WIN32
    RECT   rect;
#endif
};
EXPORT struct PaletteStruct
{   ULONG red,
          green,
          blue;
};
EXPORT struct PrinterStruct
{   int   condensed,
          page,
          pdu_x,
          pdu_y,
          queuepos,
          ribbon,
          sprocketrow,
          top,
          x, y;
    TEXT  pdu[PRINTERROWS][PRINTERCOLUMNS + 3];
    UBYTE queue[132 + 1],
          scrn[PRINTERHEIGHT_FULL][PRINTERWIDTH_FULL];
};
EXPORT struct RTCStruct
{   int wkday,
        day,
        month,
        year,
        hour,
        min,
        sec;
};
EXPORT struct KeyHelpStruct
{   const UBYTE player,
                pos,
                seq;
    const int   gid;
};
EXPORT struct GuestKeyStruct
{   const int   p1, p2;
};
EXPORT struct MemMapInfoStruct
{   const int    machine;
    const STRPTR name;
    const int    thesize,
                 overlay;
    const STRPTR credits;
    const UBYTE  gadenabled[ALLREDEFINEGADS];
    const FLAG   rotate;
    const int    menuopt;
    const UBYTE  channelused[TOTALCHANNELS];
          int    channels;
};
EXPORT struct SampleStruct
{   STRPTR filename;
    UBYTE* bodybase;
    ULONG  bodysize;
#ifdef WIN32
    UBYTE* filebase;
    ULONG  filesize;
#endif
#ifdef AMIGA
    ULONG  hertz,
           period,
           endframe;
#endif
#ifdef SCALEVOLUME
    UBYTE* scaledbase;
#endif
};

EXPORT struct Chunk
{   LONG          ckID;
    LONG          ckSize;
    LONG          ckType;
    void*         ckData;
    struct Chunk* ckNext;
};

EXPORT struct KeyInfoStruct
{   const SWORD left,
                top,
                width,
                height;
    const int   to_overlay,
                to_keypad,
                player;
};
EXPORT struct IdealStruct
{   float  hertz;
    STRPTR name;
    SBYTE  whichnote;
    UBYTE  midi;
};
EXPORT struct NoteStruct
{   const STRPTR name;
    const float  hertz;
    const UWORD  first_leftx,
                 first_rightx,
                 second_leftx,
                 second_rightx;
    const UBYTE  colour;
          UBYTE  ntscval,
                 palval,
                 stavepos;
    const FLAG   sharp;
// uninitialized
          FLAG   ghosted;
};
EXPORT struct LBNoteStruct
{   const int   low;
    const UBYTE lowmidi;
    const int   high;
    const UBYTE highmidi;
};
EXPORT struct VolumeStruct
{   const SWORD up16;
    const UBYTE up8,
                dn8;
};
EXPORT struct TODStruct
{   const int    cat;
    const STRPTR str;
};
EXPORT struct VariantStruct
{   const UBYTE bgc,
                fgc,
                rtc,
                ltc;
    const int   bats;
    struct
    {   int     x_76,
                xmin,
                x_77,
                xmax,
                player,
                facing,
                joycentre, joysensitivity;
    } batinfo[5];
};

// EXPORTED FUNCTIONS-----------------------------------------------------

// amiga|ibm.c (and others)
EXPORT void add_recent(void);
EXPORT void apply_hq2x(void);
EXPORT void apply_hq3x(void);
EXPORT void apply_hq4x(void);
EXPORT int asl(STRPTR hail, STRPTR pattern, FLAG save, STRPTR startdir, STRPTR startfile, STRPTR pathname);
EXPORT void audit(FLAG user);
EXPORT void beep(void);
EXPORT void blanker_off(void);
EXPORT void blanker_on(void);
EXPORT void break_pathname(STRPTR pathname, STRPTR path, STRPTR filename);
EXPORT void buildlistbrowser(void);
EXPORT void busypointer(void);
EXPORT void calc_size(void);
EXPORT void changecolours(void);
EXPORT void changefilepart(STRPTR pathname, STRPTR path, STRPTR filename, STRPTR newfilename);
EXPORT void checkmx(int which, int first, int last);
EXPORT void cleanexit(SBYTE rc);
EXPORT void clearkybd(void);
EXPORT void clearmsgs(void);
EXPORT void clip_close(void);
EXPORT void clip_write(STRPTR data, int localsize, unsigned int kind);
EXPORT void close_subwindow(int which);
EXPORT void cls(void);
EXPORT void closewindow(void);
EXPORT FLAG ctrl(void);
EXPORT void datestamp(int which);
EXPORT void datestamp2(void);
EXPORT FLAG debug_edit(STRPTR what, FLAG quiet, FLAG doke);
EXPORT void debugger_enter(void);
EXPORT void debugger_partial(STRPTR thecommand);
EXPORT void debugger_full(STRPTR thecommand);
EXPORT void do_eol(STRPTR thestring);
EXPORT void dogamename(void);
EXPORT void do_preview(FLAG force);
EXPORT void dospraddress(int y, STRPTR thetext);
EXPORT void edit_savetext(FLAG clip, FLAG wantasl, int whichprinter, FLAG partial);
EXPORT void edit_memory(void);
EXPORT void edit_palette(void);
EXPORT void edit_pastetext(void);
EXPORT void emptyclipboard(void);
EXPORT void enable2(int which, int enabled);
EXPORT void enablegad(int whichgad, int enabled);
EXPORT FLAG Exists(STRPTR name);
EXPORT void fix_paths(void);
EXPORT void free_display(void);
EXPORT ULONG getsize(const STRPTR filename);
EXPORT void ghost(int which, int ghosted);
EXPORT void ghost_menu(int which, UNUSED UINT outer, BOOL logic);
EXPORT void ghost_submenu(int which, UNUSED UINT outer, UNUSED UINT inner, BOOL logic);
EXPORT void help_gameinfo(void);
EXPORT void help_manual(int menunum);
EXPORT void help_opcodes(void);
EXPORT void help_hostkybd(void);
EXPORT void help_hostpads(void);
EXPORT void hexify(int y, UBYTE thebyte);
EXPORT void hostnamewindow(void);
EXPORT void invert(int localkey);
EXPORT void invert2(int left, int top, int right, int bottom);
EXPORT UBYTE KeyDown(UWORD scancode);
EXPORT void listen_close(void);
EXPORT void listen_loop(void);
EXPORT FLAG listen_open(void);
EXPORT void load_samples(void);
EXPORT FLAG load_papertape(FLAG wantasl, int whichunit);
EXPORT FLAG load_tape(FLAG wantasl);
EXPORT void lockgui(void);
EXPORT void macro_rexx(FLAG wantasl, FLAG startup);
EXPORT void make_display(void);
EXPORT void normalpointer(void);
EXPORT void openurl(STRPTR command);
EXPORT void openwindow(FLAG reopen);
EXPORT void open_output(FLAG clear);
EXPORT void open_speed(void);
EXPORT void open_spriteeditor(void);
EXPORT void play_sample(int sample);
EXPORT void process_code(void);
EXPORT void ReadJoystick(int joy);
EXPORT void read_rtc(void);
EXPORT void redrawscreen(void);
EXPORT void refreshkybd(void);
EXPORT void rq(STRPTR text);
EXPORT void say(STRPTR text);
EXPORT void saymessage(STRPTR text);
EXPORT void setbutton(int which, FLAG enabled, FLAG state);
EXPORT void setkybdtext(int scancode, HWND hwnd, ULONG gid, STRPTR contents);
EXPORT void setkybdtextandtips(int scancode, HWND hwnd, ULONG gid, STRPTR contents, STRPTR tipcontents);
EXPORT void setmemory(int whichgad, STRPTR contents);
EXPORT void setpadtext(ULONG gid, STRPTR contents);
EXPORT void setselection(void);
EXPORT void setsprview(int x, int y, int passedcolour);
EXPORT void setstatus(STRPTR thetext);
EXPORT void settitle(void);
EXPORT FLAG shift(void);
EXPORT FLAG lshift(void);
EXPORT FLAG rshift(void);
#if defined(__amigaos4__) || defined(__MORPHOS__)
    EXPORT FLAG speak(UNUSED STRPTR thespeech);
#else
    EXPORT FLAG speak(STRPTR thespeech);
#endif
EXPORT FLAG system_dopaddle(int source, int dest);
EXPORT void thesleep(int microseconds);
EXPORT ULONG thetime(void);
EXPORT void tick(int which, int ticked);
EXPORT void tools_music(void);
EXPORT void uncheckmx(int which);
EXPORT void unlockgui(void);
EXPORT void updatebiggads(void);
EXPORT void updatepadnames(void);
EXPORT void updatepointer(FLAG force, FLAG full);
EXPORT void updatescreen(void);
EXPORT void update_cpumonitortips(void);
EXPORT void update_memorytips(void);
EXPORT void update_notation(void);
EXPORT void update_psgmonitortips(void);
EXPORT void update_xvimonitortips(void);
EXPORT void update_opcodes(void);
EXPORT void update_overlaytips(void);
EXPORT void view_controls(void);
EXPORT void view_hiscores(void);
EXPORT void view_monitor(int kind);
EXPORT void wa_checkinput(void);
#ifdef AMIGA
    EXPORT void aa_checkinput(void);
    EXPORT void amiga_loadedconfig(void);
    EXPORT void amiga_setup(void);
    EXPORT void attachsmlgads(void);
    EXPORT void binbugroms(void);
    EXPORT void bringtofront(void);
    EXPORT void checkbreak(void);
    EXPORT void ch_clearlist(struct List* ListPtr);
    EXPORT void clip_open(UNUSED FLAG empty);
    EXPORT void closescreen(int whichscreen);
    EXPORT void detachsmlgads(void);
    EXPORT void do_autopause(void);
    EXPORT void do_recents(void);
    EXPORT LONG easyrequest(STRPTR title, STRPTR body, STRPTR buttons, ULONG image);
    EXPORT void edit_copytext_amiga(void);
    EXPORT void edit_instructor_dips(void);
    EXPORT void edit_malzak2_dips(void);
    EXPORT void edit_pong_dips(void);
    EXPORT void edit_zaccaria_dips(void);
    EXPORT void end_samples(void);
    EXPORT void fixdebuggercolour(void);
    EXPORT void flipbool(int* thevar, int command, FLAG doit);
    EXPORT void freepens(void);
    EXPORT void free_samples(void);
    EXPORT void getpens(void);
    EXPORT void ghostmonitorbutton(int kind, struct Gadget* which, int state);
    EXPORT void handle_keydown(UWORD code);
    EXPORT void handle_keyup(UWORD code);
    EXPORT void help_reaction(void);
    EXPORT void iconify(void);
    EXPORT void input_rearrange(void);
    EXPORT void input_sensitivity(void);
    EXPORT void InitHook(struct Hook* hook, ULONG (*func)(), void* data);
    EXPORT void load_images(int first, int last, FLAG splashing);
    EXPORT void load_splashy_images(int first1, int last1, int first2, int last2);
    EXPORT FLAG lockscreen(FLAG panic);
    EXPORT void lowerpri(void);
    EXPORT FLAG open_subwindow(int which);
    EXPORT FLAG mouseisover(int gid, SWORD mousex, SWORD mousey);
    EXPORT void raisepri(void);
    EXPORT void resize(int newsize, UNUSED FLAG force);
    EXPORT void rexxmsgs(void);
    EXPORT void setjoyports(void);
    EXPORT void setmonitorbutton(int kind, STRPTR contents, int whichmongad);
    EXPORT void setmonitorstring(int kind, struct Gadget* gadgetptr, STRPTR contents);
    EXPORT void uniconify(void);
    EXPORT void unlockscreen(void);
    EXPORT void updatebrightness(void);
    EXPORT void updatesidebartips(void);
    EXPORT void updatewheel(void);
    EXPORT void update_memory_system(void);
    EXPORT void upgradeconfigs(void);
    EXPORT void writeicon(int kind, STRPTR passedfn);
    #ifndef __amigaos4__
        EXPORT void updatefilter(void);
    #endif
    #ifdef __MORPHOS__
        #if __GNUC__ > 2
            EXPORT STRPTR strupr_mos(STRPTR s);
        #endif
    #endif
#endif
#ifdef __PPC__
    EXPORT void InitModules(void);
    EXPORT void CleanupModules(void);
    EXPORT void InitModulesPPC(void);
    EXPORT void CleanupModulesPPC(void);
#endif
#ifdef WIN32
    EXPORT FLAG associate(int kind);
    EXPORT void calibrate(void);
    EXPORT void capslock_off(void);
    EXPORT void changelanguage(void);
    EXPORT void clip_open(UNUSED FLAG empty);
    EXPORT void debugger_exit(void);
    EXPORT void doassociations(void);
    EXPORT void do_autopause(WPARAM wParam, LPARAM lParam);
    EXPORT void do_recents(HMENU themenu);
    EXPORT void drawitem(LPDRAWITEMSTRUCT lpDIS, FLAG pushed);
    EXPORT void flipbool(int* thevar, int command, FLAG doit);
    EXPORT void freelanguage(void);
    EXPORT STRPTR GetCatalogStr(int catalog, LONG stringNum, STRPTR defaultString);
    EXPORT void getpointerpos(int* x, int* y);
    EXPORT void ghostmonitorbutton(int kind, HWND which, int state);
    EXPORT void handle_keydown(UINT code);
    EXPORT void handle_keyup(UINT code);
    EXPORT void iconify(void);
    EXPORT void load_catalog(void);
    EXPORT void make_tips(int kind, int elements, int firstgad);
    EXPORT void make_statusbar(void);
    EXPORT void make_toolbar(void);
    EXPORT void move_subwindow(int which, HWND hwnd);
    EXPORT void parse_catalog(void);
    EXPORT void project_register(void);
    EXPORT void open_channels(int howmany);
    EXPORT void resize(int newsize, FLAG force);
    EXPORT void setcodepage(void);
    EXPORT void setdlgtext(HWND hwnd, int gid, int msg, STRPTR defaultmsg);
    EXPORT void setfont(HWND hwnd, int gid);
    EXPORT void setmonitorbutton(int kind, STRPTR contents, int whichmongad);
    EXPORT void setmonitorstring(int kind, HWND gadgetptr, STRPTR contents);
    EXPORT void setpri(void);
    EXPORT void translate(void);
    EXPORT void traymenu(void);
    EXPORT void uniconify(FLAG exiting);
    EXPORT void update_joynames(void);
    EXPORT double zatof(STRPTR inputstr);

    LRESULT CALLBACK TestWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    EXPORT void init_cheevos(void);
    EXPORT void RA_ProcessInputs(void);
    EXPORT void remove_cheevos(FLAG full);
    EXPORT UBYTE AByteReader( unsigned int nOffs                    );
    EXPORT void  AByteWriter( unsigned int nOffs, unsigned char nVal);
    EXPORT UBYTE EByteReader( unsigned int nOffs                    );
    EXPORT void  EByteWriter( unsigned int nOffs, unsigned char nVal);
    EXPORT UBYTE IByteReader1(unsigned int nOffs                    );
    EXPORT void  IByteWriter1(unsigned int nOffs, unsigned char nVal);
    EXPORT UBYTE IByteReader2(unsigned int nOffs                    );
    EXPORT void  IByteWriter2(unsigned int nOffs, unsigned char nVal);
#endif

#ifdef AMIGA
    // aa-subwin.c
    EXPORT int make_memorytips(void);
    EXPORT FLAG redefinekeys(FLAG preserve);
    EXPORT void updatekeycolours(struct Window* hwnd);
    EXPORT void updatepadcolours(struct Window* hwnd);
    EXPORT void settapegad(void);

    EXPORT void     controls_loop(void);
    EXPORT void         dips_loop(void);
    EXPORT void  floppydrive_loop(void);
    EXPORT void     gameinfo_loop(void);
    EXPORT void     hostkybd_loop(void);
    EXPORT void     hostpads_loop(void);
    EXPORT void   industrial_loop(void);
    EXPORT void       memory_loop(void);
    EXPORT void  monitor_cpu_loop(void);
    EXPORT void monitor_psgs_loop(void);
    EXPORT void  monitor_xvi_loop(void);
    EXPORT void        music_loop(void);
    EXPORT void      opcodes_loop(void);
    EXPORT void      palette_loop(void);
    EXPORT void    papertape_loop(void);
    EXPORT void      printer_loop(void);
    EXPORT void     tapedeck_loop(void);
    EXPORT void        speed_loop(void);
    EXPORT void spriteviewer_loop(void);

    // arexx.c
    EXPORT void init_arexx(void);
#endif

// aa/wa-modal.c
EXPORT FLAG confirm(void);
EXPORT void emu_paths(void);
EXPORT void help_about(void);
EXPORT void showthetod(void);
#ifdef WIN32
    EXPORT void rexx_input(void);
#endif
#ifdef AMIGA
    EXPORT void modal_on(void);
    EXPORT void modal_off(void);
    EXPORT void sayit(const char* text);
#endif

// aa/wa-subwin.c
EXPORT void redraw_axes(void);
EXPORT void setrasgads(int passedcpu);
#ifdef WIN32
    EXPORT void open_subwindow(int which, LPCTSTR dialog, DLGPROC function);
    EXPORT void update_palette(FLAG full);
#endif
EXPORT void drawupnote(int whichchan, int whichnote);
EXPORT void drawdownnote(int whichchan, int whichnote, int colour);
EXPORT void ghost_bar(int whichbar);
EXPORT void ghost_note(int whichnote);
EXPORT void bu_enable(int subwin, int gid, BOOL logic);
EXPORT void bu_set(int subwin, int gid);
EXPORT void bu_set2(int subwin, int gid, STRPTR thestring);
EXPORT void bu_select(int subwin, int gid, BOOL logic);
EXPORT void cb_set(int subwin, int gid, BOOL logic);
EXPORT void cb_enable(int subwin, int gid, BOOL logic);
EXPORT void ch_set(int subwin, int gid, ULONG value);
EXPORT void ch2_set(int subwin, int gid, ULONG value);
EXPORT void im_set(int whichsubwin, int gid, int whichimage);
EXPORT void ra_set(int subwin, int firstgid, int lastgid, int value);
EXPORT void ra_enable(int subwin, int gid, BOOL logic);
EXPORT void ra_enable2(int subwin, int gid, BOOL logic);
EXPORT void sl_set(int subwin, int gid, ULONG value);
EXPORT void sl_set2(int subwin, int gid, ULONG value1, ULONG value2);
EXPORT void sl_enable(int subwin, int gid, BOOL logic);
EXPORT void st_set(int subwin, int gid);
EXPORT void st_set2(int subwin, int gid, STRPTR thestring);
EXPORT void handle_escdown(int scancode, HWND subwin);
EXPORT void handle_escup(int scancode);
EXPORT void open_magnifier(void);
EXPORT void update_magnifier(void);
EXPORT void close_magnifier(void);

// aa/wa-periph.c
#ifdef AMIGA
    EXPORT void drawblox(int whichdrive, int x, int y, int whichpen);
    EXPORT int make_floppytips(void);
    EXPORT void redraw_blox(int whichunit);
#endif
EXPORT void open_floppydrive(FLAG needupdate);
EXPORT void open_industrial(void);
EXPORT void open_papertape(void);
EXPORT void open_tapedeck(void);
EXPORT void redraw_furnace(void);
EXPORT void redraw_roll(int whichunit);
EXPORT void redraw_waveform(void);
EXPORT void tools_printer(void);
EXPORT void update_floppytips(void);
EXPORT void update_printer(void);

// aa/wa-periph.c
EXPORT void edit_dips(void);

// 2650.c
EXPORT void checkinterrupt(void);
EXPORT void cpu_2650_tapable(void);
EXPORT void cpu_2650_untapable(void);
EXPORT void cpu_setup(void);
EXPORT void one_instruction(void);
EXPORT void pullras(void);
EXPORT void pushras(void);
EXPORT void set_pause(int thetype);
EXPORT void checkstep(void);
EXPORT void do_tape(void);

// disasm.c
EXPORT void disassemble_2650(int address1, int address2, FLAG quiet);
EXPORT void disgame(int address1, int address2, STRPTR filename);
EXPORT void doasmhdr(STRPTR equname);
EXPORT STRPTR labelstring2(TEXT whichkind);
EXPORT void saytrace(void);
EXPORT void show_data_comment(int whichaddr, int mode, STRPTR stringptr);
EXPORT void table_trace(void);
EXPORT void tracecpu_2650(FLAG full, FLAG quiet);
EXPORT void update_opcodes_engine(void);
EXPORT void view_next_2650(void);

// pvi.c
EXPORT int calcspritecolour(int whichsprite);
EXPORT void ie_inputmirrors(int address);
EXPORT void ie_stringchar(UBYTE data, int address);
EXPORT void ie_viewscreen(void);
EXPORT void newpvi(void);
EXPORT void newpvi_drawhelpgrid(void);
EXPORT void pviwrite(signed int address, UBYTE data, FLAG ispvi);
EXPORT void pvi_memmap(void);
EXPORT void pvi_vblank(void);
EXPORT void pvi_view_udgs(void);
EXPORT void view_ram_pvi(void);
EXPORT void changeabspixel(int x, int y, int colour);
EXPORT void changethisabspixel(int colour);
EXPORT void changethisbgpixel(int colour);
EXPORT void changerelpixel(int x, int y, int colour);
EXPORT void set_retuning(void);
EXPORT FLAG pvi_edit_screen(UWORD code);

// coin-ops.c
EXPORT void do_autocoin(void);
EXPORT void oldpvi(void);
EXPORT void serialize_coinops(void);
EXPORT void coinop_playerinput(int source, int dest);

// debugger.c
EXPORT void addhistory(void);
EXPORT void adduserlabel(STRPTR name, UWORD address, TEXT kind);
EXPORT FLAG allowable(FLAG loud);
EXPORT UBYTE aof_makebcc(int start, int end, UBYTE* BufferPtr);
EXPORT void breakrastline(void);
EXPORT void cd_configs(void);
EXPORT void cd_projects(void);
EXPORT void cd_progdir(void);
EXPORT void cd_screenshots(void);
EXPORT void changetimeunitnames(void);
EXPORT void check_labels(void);
EXPORT void clearcoverage(void);
EXPORT void clearruntos(void);
EXPORT FLAG conditional(struct ConditionalStruct* cond, UBYTE data, FLAG writing, int pointkind);
EXPORT FLAG debug_command(void);
EXPORT void dumprow(int i);
EXPORT int parse_expression(STRPTR thestring, int max, FLAG maskable);
EXPORT TEXT getfriendly(int number);
EXPORT TEXT guestchar(UBYTE input);
EXPORT void handletab(void);
EXPORT UBYTE iof_makebcc(int start, int end, UBYTE* BufferPtr);
EXPORT void loadsym(TEXT* bufptr);
EXPORT void logport(int port, UBYTE data, FLAG write);
EXPORT TEXT number_to_friendly(int number, STRPTR thestring, FLAG both, int multimode);
EXPORT int readbase(STRPTR input, FLAG maskable);
EXPORT UBYTE readnum_ebcdic(TEXT code);
EXPORT FLAG save_aof(STRPTR filename, int startaddr, int endaddr, int progstart, int preferredsize);
EXPORT ULONG scale_time(ULONG inputclocks);
EXPORT void view_bios_ram(void);
EXPORT void view_cpu_2650(FLAG linefeed);
EXPORT void view_ram(void);
EXPORT void view_udgs(void);
EXPORT void view_xvi(void);
EXPORT FLAG wp_add(int address, ULONG the1st, UBYTE the2nd, ULONG the3rd, UBYTE mask, FLAG update);
EXPORT FLAG wp_clear(int address);

// assembler.c
EXPORT void assemble(void);
EXPORT void asmerror(STRPTR errorstring);
EXPORT int parse_opcode(STRPTR passedstring, int length);
EXPORT int parsenumber(STRPTR stringptr);
EXPORT int parse_literal(STRPTR passedstring, int length);
EXPORT void savesym(FILE* symhandle);

// engine1.c
// misc
EXPORT void command_changemachine(int whichmachine, int whichmemmap);
EXPORT void fixextension(STRPTR extension, STRPTR passedfn, FLAG force, STRPTR suffix);
EXPORT void project_open(void);
EXPORT void settapespeed(void);
EXPORT void settitle_engine(void);
// config
EXPORT void loadconfig(void);
EXPORT void saveconfig(void);
// crcs
EXPORT ULONG crc32(UBYTE* address, int thesize);
EXPORT ULONG crc32_cont(UBYTE* address, int thesize, ULONG oldcrc);
EXPORT void crc64(UBYTE* address, int thesize);
EXPORT void generate_crc32table(void);
// emu control
EXPORT void cripple(void);
EXPORT void uncripple(void);
EXPORT void emu_pause(void); // unistd.h has a pause() function!
EXPORT void emu_unpause(void);
EXPORT void emulate(void);
EXPORT void engine_setup(void);
EXPORT void engine_quit(void);
// numbers
EXPORT void dec_to_bin(UBYTE number);
EXPORT void dec_to_hex(UBYTE number);
EXPORT void hex1(char* out, UBYTE value);
EXPORT void hex2(char* out, UWORD value);
// high scores
EXPORT void hiscore_register(int slot, int score);
EXPORT void loadhiscores(void);
EXPORT void savehiscores(void);
EXPORT void updatehiscores(int resolved);
// input
EXPORT void changeconsolekeynames(void);
EXPORT void engine_dopaddle(int source, int dest);
EXPORT int guest_to_host_joy(int guestjoy);
EXPORT FLAG keyexists(int thekey);
EXPORT int scan_to_scan(int code);
EXPORT UBYTE get_guest_key(void);
// monitor
EXPORT int getmonitoraddr(int whichgad);
EXPORT void ghostmonitor(void);
EXPORT void monchange(int which);
EXPORT void setmonused(void);
// mouse
EXPORT void controls_mousedown(int mousex, int mousey);
EXPORT void controls_mouseup(void);
EXPORT void domouse(void);
EXPORT void music_mousedown(int whichbutton, int mousex, int mousey);
EXPORT void music_mouseup(int whichbutton);
// network
EXPORT void ActAsServer_IPv6(unsigned short Port);
EXPORT void ConnectAsClient_IPv6(const char* Server, unsigned short Port);
EXPORT void NetPlay(int Switch);
EXPORT void NetClose(void);
EXPORT UBYTE NetReceiveByte(void);
EXPORT FLAG NetSendByte(UBYTE value);
EXPORT FLAG NetSend(const char* Out, int N);
EXPORT FLAG NetRecv(char* In, int N);
EXPORT void recv_usermsg(int msglen);
EXPORT void send_usermsg(int msglen);
// poke
EXPORT FLAG poke_start(STRPTR thestring, FLAG quiet, FLAG doke);
EXPORT void poke_end(FLAG quiet, FLAG doke);
// read/write
EXPORT UBYTE loadbyte(void);
EXPORT void savebyte(UBYTE value);
EXPORT UBYTE readport(int port);
EXPORT void writeport(int port, UBYTE data);
// sound
EXPORT void playsound(FLAG force);
// subwindows
EXPORT void close_subwindows(FLAG all);
EXPORT void reopen_subwindow(int which);
EXPORT void reopen_subwindows(void);
EXPORT void update_roll(int whichunit);
EXPORT void update_waveform(void);
// update
EXPORT void update_capslock(void);
EXPORT void update_controls(void);
EXPORT void update_controlstip(void);
EXPORT void updatekeynames(HWND hwnd);
EXPORT void update_speed(void);
// SAS/C emulation
#ifndef __SASC // eg. __VBCC__, __GNUC__, WIN32
    EXPORT int stcd_l(const char* in, SLONG* lvalue);
    EXPORT int stch_l(const char* in, SLONG* lvalue);
    EXPORT int stcl_d(char* out, long lvalue);
    EXPORT int stcl_h(char* out, unsigned long lvalue);
    EXPORT int stcul_d(char* out, unsigned long lvalue);
    EXPORT char* stptok(const char *s, char *tok, int toklen, char *brk);
#endif

// engine2.c
// misc
EXPORT void configure(void);
EXPORT void dosidebar(void);
EXPORT void enablegads(void);
EXPORT void generate_autotext(void);
EXPORT UBYTE getrandom(int modulo);
EXPORT void keynames_from_overlay(void);
EXPORT void patchrom(void);
EXPORT FLAG project_bpnf(void);
EXPORT void project_reset(FLAG keepalive);
// aofs
EXPORT int load_aof(int localsize, FLAG intel);
#ifdef CHECKBLOCKS
    EXPORT void checkblocks(void);
#endif
// audit
EXPORT void auditfile(STRPTR passedname, int passedsize, FLAG user);
EXPORT void audit_coinops(void);
EXPORT void makeaudittext(STRPTR outputstring);
EXPORT void sortaudit(void);
// console output
EXPORT void fliplog(int* thevar);
EXPORT void logfile_open(FLAG quiet);
EXPORT void logfile_close(void);
#ifdef WIN32
    EXPORT void zprintf(int whichcolour, const char* format, ...);
#endif
#ifdef AMIGA
    EXPORT void zprintf(UNUSED int whichcolour, const char* format, ...);
#endif
// conversion
EXPORT int bin2int(STRPTR thestring, int length);
EXPORT UBYTE getdigit(UBYTE data);
EXPORT int oct2int(STRPTR thestring, int length);
EXPORT int readnum_binary(STRPTR input, int length);
EXPORT int readnum_decimal(STRPTR input, int length);
EXPORT int readnum_hex(STRPTR input, int length);
EXPORT int readnum_octal(STRPTR input, int length);
EXPORT int readnum_char(TEXT input);
// emu control
EXPORT void endofframe(UNUSED int newsky);
// i/o buffer
EXPORT void alloc_iobuffer(ULONG thesize);
EXPORT void free_iobuffer(void);
// network
EXPORT void help_update(FLAG quiet);
// read/write
EXPORT UBYTE ReadByte(void);
EXPORT UBYTE ReadByteAt(int where);
EXPORT UWORD ReadWord(void);
EXPORT UWORD ReadWordAt(int where);
EXPORT ULONG ReadLong(void);
EXPORT void WriteByte(UBYTE what);
EXPORT void WriteWord(UWORD what);
EXPORT void WriteLong(ULONG what);
// serialization
EXPORT int parse_bytes(int mode);
EXPORT void serialize_cos(void);
EXPORT void serialize_byte(UBYTE* var);
EXPORT void serialize_byte_int(int* var);
EXPORT void serialize_byte_ulong(ULONG* var);
EXPORT void serialize_long(ULONG* var);
EXPORT void serialize_word(UWORD* var);
EXPORT void serialize_word_int(int* var);
// sound
EXPORT void sound_reset(void);
// timing
EXPORT void reset_fps(void);
EXPORT void update_fps(void);
EXPORT void wait_first(void);
EXPORT void wait_second(void);
// tips
EXPORT void make_memorytip(int address, STRPTR stringptr);
EXPORT void make_monitortip(int address, STRPTR stringptr);
EXPORT void make_opcodetip(int i, STRPTR stringptr);
EXPORT void make_overlaytip(int i, STRPTR stringptr);
EXPORT void make_overlayspecialtip(int left, int right, int id, STRPTR stringptr);
// zips
EXPORT FLAG IsZip(STRPTR filename);
EXPORT FLAG loadzip(STRPTR passedname, FLAG auditing, FLAG quiet);

// engine3.c
// misc
EXPORT void changemachine(int whichmachine, int whichmemmap, FLAG user, FLAG force, FLAG same);
EXPORT void fix_keyrects(void);
EXPORT int engine_load(FLAG silent);
EXPORT void engine_reset(void);
EXPORT int getsmallimage2(int thegame, int thememmap);
EXPORT void longcomma(ULONG value, STRPTR out);
EXPORT FLAG wantellipse(int localkey);
EXPORT void zstrncpy(char* to, const char* from, size_t n);
EXPORT void set_cpl(int newcpl);
// cpu monitor
EXPORT FLAG getmoncolour(int whichmongad, ULONG* whichemupen, int* whichemubrush);
EXPORT void update_monitor_cpu(FLAG force);
// floppy disk
EXPORT void dir_disk(FLAG quiet, int whichdrive);
EXPORT void get_disk_byte(int whichdrive, FLAG withoffset);
EXPORT int get_viewing_cluster(int whichdrive);
EXPORT FLAG load_disk(FLAG wantasl, int whichdrive, FLAG user);
EXPORT void set_drive_mode(int newdrivemode);
EXPORT void update_disk(int whichdrive);
// high scores
EXPORT void clearhs(void);
EXPORT FLAG enableclearhs(void);
// input
EXPORT void enqueue(UWORD hostcode, TEXT guestcode);
// industrial
EXPORT void update_industrial(FLAG force);
// memory
EXPORT void update_memory(FLAG force);
// palette
EXPORT void checkcolours(void);
EXPORT int parse_hexbyte(void);
EXPORT void update_sliders(void);
// sound
EXPORT FLAG nearby(int x1, int y1, int x2, int y2);
EXPORT void play_ambient_sample(int whichsound);
EXPORT void play_bell(int kind);
EXPORT void tools_music_engine(void);
EXPORT void update_music(void);
// sprite editor
EXPORT void dosprcolour(int newcolour);
EXPORT void makesprstring(int which);
EXPORT int spritebutton(int x, int y, int operation);
EXPORT void update_spriteeditor(FLAG force);
// cassette tape deck
EXPORT void gettapepos(void);
EXPORT void tapedeck_settitle(void);
EXPORT void verbosetape_load(void);
EXPORT void verbosetape_save(void);
// papertape unit
EXPORT void papertape_settitle(void);
// timing
EXPORT FLAG is_drawable(void);
// tips
EXPORT void generate_controlstip(int localkey);
// update
EXPORT void update_monitor(FLAG force);
EXPORT void update_monitor_psgs(FLAG force);
// xvi monitor
EXPORT void do_axes(void);
#ifdef WIN32
    EXPORT void drawaxesline(int x1, int y1, int x2, int y2, COLORREF colour);
#endif
#ifdef AMIGA
    EXPORT void drawaxesline(int x1, int y1, int x2, int y2, int colour);
#endif
EXPORT void update_monitor_xvi(FLAG force);

// formats.c
EXPORT void create_papertape(int whichunit);
EXPORT void create_tape(void);
EXPORT void engine_save(int kind, FLAG updatetitle);
EXPORT void finalize_anim(FILE* filehandle);
EXPORT void finalize_midi(void);
EXPORT void finalize_smus(void);
EXPORT void generate_tone(int guestchan, double hertz, int guestvolume);
EXPORT void generate_noise(int guestchan, double hertz, int guestvolume);
EXPORT void macro_restartplayback(void);
EXPORT void macro_start(void);
EXPORT void macro_stop(void);
EXPORT void midi_startrecording(void);
EXPORT void papertape_eject(int whichunit);
EXPORT void papertape_play(int whichunit);
EXPORT void papertape_record(int whichunit);
EXPORT void papertape_stop(int whichunit);
EXPORT void project_save(int kind);
EXPORT void saveasmscrnshot(void);
EXPORT void savescreenshot(int kind, FLAG wantasl, FLAG clip, FLAG giffing);
EXPORT void smus_startrecording(void);
EXPORT void sound_startrecording(void);
EXPORT void sound_stoprecording(void);
EXPORT void tape_eject(void);
EXPORT void tape_play(void);
EXPORT void tape_record(void);
EXPORT void tape_stop(void);
EXPORT UBYTE ticks2smus(void);
EXPORT void ulong_to_be(UBYTE* stringptr, ULONG value);
EXPORT void ulong_to_le(UBYTE* stringptr, ULONG value);
EXPORT void uword_to_le(UBYTE* stringptr, UWORD value);
EXPORT int variablelength(ULONG input, UBYTE* output);
EXPORT void writeanimframe(void);
EXPORT void writeapngframe(void);
EXPORT void writegifframe(void);
EXPORT void writemngframe(void);
EXPORT void writepsgfile(void);
EXPORT void writeymfile(void);
EXPORT void write_actl_start(FILE* filehandle);
EXPORT void write_actl_end(FILE* filehandle);
EXPORT void write_fctl(FILE* filehandle);
EXPORT void write_idat_or_fdat(FILE* filehandle, FLAG is_fdat, int kind, FLAG animating);
EXPORT void write_iend(FILE* filehandle);
EXPORT void write_mend(FILE* filehandle);
EXPORT void write_mhdr_end(FILE* filehandle);
EXPORT void write_wav(int guestchan, FLAG silencing);
EXPORT FLAG load_sms(int localsize);
EXPORT FLAG save_sms(STRPTR filename, int endaddr);
EXPORT FLAG load_bpnf(int localsize, int startaddr);
EXPORT FLAG save_bpnf(STRPTR filename, int startaddr, int endaddr);
#ifdef WIN32
    EXPORT int load_bmp(const char* thefilename);
#endif

// gfx.c
EXPORT void apply_scale2x(void);
EXPORT void apply_scale3x(void);
EXPORT void apply_scale4x(void);
EXPORT void calc_margins(void);
EXPORT void change_colour_names(void);
EXPORT void changepixel(int x, int y, int colour);
EXPORT void changebgpixel(int x, int y, int colour);
EXPORT void drawcontrolspixel(int x, int y, int colour);
EXPORT void drawctrlglow(int leftx, int lefty, FLAG lit);
EXPORT void drawdigit(int position, UBYTE value);
EXPORT void draw_guide_ray(FLAG erasing);
EXPORT void drawglow(int x, int y, int colour);
EXPORT void draw_margins(void);
EXPORT void drawsegment(int position, int segment, int colour);
EXPORT void drawsegments(int position);
EXPORT FLAG edit_screen(UWORD code);
EXPORT void edit_screen_sanitize(void);
EXPORT void fixupcolours(void);
EXPORT void gfx_setup(void);
EXPORT ULONG setkybdcolour(int gid);
EXPORT void show_position(int x, int y, FLAG setting);
EXPORT void thecout(UBYTE value);
EXPORT void view_controls_engine(void);

// gui.c
EXPORT void docommand(int which);
EXPORT void docommand2(int which);
EXPORT void handle_menu(int command);
EXPORT void handle_menu2(int command);
EXPORT void updatemenu(int which);
EXPORT void updatemenus(void);
EXPORT void updatesmlgad(int which, ULONG state, FLAG now);
EXPORT void updatesmlgads(void);

// periph.c
EXPORT void ghost_dips(BOOL state);
EXPORT void update_floppydrive(int level, int drive);
EXPORT void update_papertape(int whichunit, FLAG force);
EXPORT void update_tapedeck(FLAG force);

// arcadia.c
EXPORT void arcadia_drawhelpgrid(void);
EXPORT FLAG arcadia_edit_screen(UWORD code);
EXPORT void arcadia_setmemmap(void);
EXPORT void arcadia_serializerom(void);
EXPORT void arcadia_stringchar(UBYTE data, int address);
EXPORT void arcadia_update_miniglow(void);
EXPORT void arcadia_viewscreen(void);
EXPORT void arcadia_view_udgs(void);
EXPORT void uvi(void);
EXPORT void uviwrite(UWORD address, UBYTE data);
EXPORT void arcadia_reset(void);

// interton.c
EXPORT void interton_serializerom(void);
EXPORT void interton_setmemmap(void);
EXPORT void interton_reset(void);

// elektor.c
EXPORT void elektor_biosdetails(int ea);
EXPORT void elektor_setmemmap(void);
EXPORT void monob(int y);
EXPORT int load_eof(int localsize);
EXPORT FLAG save_eof(STRPTR filename, int startaddr, int endaddr, int progstart, int preferredsize);
EXPORT void elektor_reset(void);
EXPORT int hertz_to_note(float hertz);
EXPORT void view_psgs(void);

// pipbug.c
EXPORT void pipbug_emulate(void);
EXPORT float pipbug_getpitch(UWORD value);
EXPORT void pipbug_setmemmap(void);
EXPORT UBYTE pipbug_readport(int port);
EXPORT void pipbug_writeport(int port, UBYTE data);
EXPORT void pipbug_viewscreen(void);
EXPORT void pipbug_changebaud(void);
EXPORT void pipbug_changebios(FLAG user);
EXPORT void pipbug_serialize_cos(void);
EXPORT void pipbug_update_miniglows(void);
EXPORT void pipbug_drawhelpgrid(void);
EXPORT void pipbin_biosdetails(int ea);
EXPORT void pipbin_io(void);
EXPORT void pipbin_readtty(void);
EXPORT void pipbin_runcpu(void);
EXPORT void pipbin_prtdemo(void);
EXPORT void euy_margins(void);
EXPORT void euy_printchar(UBYTE thechar, FLAG eti);
EXPORT void printer_eject(int whichprinter);
EXPORT void printer_savepartial(int whichprinter);
EXPORT void printer_changepixel(int whichprinter, int x, int y, int colour);
EXPORT void printers_reset(void);
EXPORT void printer_scroll(void);
EXPORT void printer_copygfx(FLAG clip, int kind, FLAG wantasl, int whichprinter);
EXPORT void pipbug_view_basic(void);
EXPORT void elekterminal_scroll(int delta);
EXPORT void reset_vdu(FLAG full);
EXPORT void pipbug_reset(void);
EXPORT void pipbug_redrawleds(void);
EXPORT void industrial_reset(void);

// binbug.c
EXPORT void binbug_emulate(void);
EXPORT void binbug_setmemmap(void);
EXPORT void binbug_viewscreen(void);
EXPORT UBYTE binbug_readport(int port);
EXPORT void binbug_writeport(int port, UBYTE data);
EXPORT void binbug_view_udgs(void);
EXPORT void patch_acos(void);
EXPORT void do_postamble(void);
EXPORT void binbug_changebios(FLAG user);
EXPORT void binbug_dir_disk(FLAG quiet, int whichdrive);
EXPORT void binbug_create_disk(int whichdrive);
EXPORT FLAG binbug_extract_file(int whichfile);
EXPORT void binbug_drawhelpgrid(void);
EXPORT void extend_disk(int whichdrive);
EXPORT FLAG load_cmd(int localsize);
EXPORT FLAG save_cmd(STRPTR filename, int startaddr, int endaddr, int progstart);
EXPORT FLAG binbug_load_disk(FLAG wantasl, int whichdrive);
EXPORT FLAG binbug_edit_screen(UWORD code);
EXPORT void yesno(UBYTE data);
EXPORT void noyes(UBYTE data);
EXPORT void showdrivestatus(void);
EXPORT void binbug_delete_file(int whichfile, int whichdrive);
EXPORT void binbug_changebaud(void);
EXPORT void binbug_serialize_cos(void);
EXPORT FLAG binbug_rename_file(int whichfile, STRPTR newname);
EXPORT void binbug_reset(void);
EXPORT void binbug_inject_file(STRPTR thefilename);

// fd1771.c
EXPORT void fd1771_restore(UBYTE data);
EXPORT void fd1771_seek(UBYTE data);
EXPORT void fd1771_step(UBYTE data);
EXPORT void fd1771_force_interrupt(UBYTE data);
EXPORT void fd1771_read_sector(UBYTE data);
EXPORT void fd1771_write_sector(UBYTE data);
EXPORT void fd1771_step_in(UBYTE data);
EXPORT void fd1771_step_out(UBYTE data);
EXPORT void fd1771_read_address(UBYTE data);
EXPORT void fd1771_read_track(UBYTE data);
EXPORT void fd1771_write_track(UBYTE data);
EXPORT void fd1771_write_trackreg(UBYTE data);
EXPORT void fd1771_write_reqsector(UBYTE data);
EXPORT void fd1771_write_datareg(UBYTE data);
EXPORT UBYTE fd1771_read_datareg(void);
EXPORT UBYTE fd1771_read_status(void);
EXPORT UBYTE fd1771_read_controlreg(void);
EXPORT void reset_drives(void);
EXPORT void fd1771_spindown(void);

// instruct.c
EXPORT void instructor_biosdetails(int ea);
EXPORT void si50_emulate(void);
EXPORT void si50_setmemmap(void);
EXPORT void si50_viewscreen(void);
EXPORT UBYTE si50_readport(int port);
EXPORT void si50_update_miniglows(void);
EXPORT void si50_writeport(int port, UBYTE data);
EXPORT void si50_reset(void);
EXPORT void si50_updatedips(FLAG force);

// twin.c
EXPORT void twin_emulate(void);
EXPORT void twin_setmemmap(void);
EXPORT UBYTE twin_readport(int port);
EXPORT void twin_writeport(int port, UBYTE data);
EXPORT FLAG twin_load_disk(FLAG wantasl, int whichdrive);
EXPORT void twin_create_disk(int whichdrive);
EXPORT void twin_dir_disk(FLAG quiet, int whichdrive);
EXPORT FLAG twin_extract_file(int whichfile, int whichdrive);
EXPORT void twin_drawhelpgrid(void);
EXPORT void twin_viewscreen(void);
EXPORT void twin_serialize_cos(void);
EXPORT void master_to_slave(void);
EXPORT void slave_to_master(void);
EXPORT void view_svc(int sfc);
EXPORT void c306_margins(int whichprinter);
EXPORT void twin_prtdemo(void);
EXPORT void change_printer(void);
EXPORT void twin_swapdisks(void);
EXPORT FLAG load_mod(int localsize);
EXPORT FLAG save_mod(STRPTR filename, int startaddr, int endaddr, int progstart);
EXPORT void twin_logport(int port, UBYTE data, FLAG write);
EXPORT void twin_biosdetails(int ea);
EXPORT void twin_delete_file(int whichfile, int whichdrive);
EXPORT FLAG twin_rename_file(int whichfile, int whichdrive, STRPTR newname);
EXPORT void twin_reset(void);
EXPORT void twin_inject_file(STRPTR thefilename);
EXPORT void twin_view_dos(void);
EXPORT void twin_get_commands(int whichdrive, int whichfile, FLAG multiline);
// exos.c
EXPORT void exos_to_disk(int whichdrive);
EXPORT void exos_to_ram(void);
// sdos.c
EXPORT void sdos_to_disk(int whichdrive);
EXPORT void sdos_to_ram(void);
// tos.c
EXPORT void tos_to_disk(int whichdrive);
EXPORT void tos_to_ram(void);
// udos.c
EXPORT void udos_to_disk(int whichdrive);
EXPORT void udos_to_ram(void);

// cd2650.c
EXPORT void cd2650_emulate(void);
EXPORT void cd2650_setmemmap(void);
EXPORT void cd2650_viewscreen(void);
EXPORT UBYTE cd2650_readport(int port);
EXPORT void cd2650_writeport(int port, UBYTE data);
EXPORT void cd2650_biosdetails1(int ea);
EXPORT void cd2650_biosdetails2(int ea);
EXPORT void cd2650_drawhelpgrid(void);
EXPORT FLAG cd2650_edit_screen(UWORD code);
EXPORT void cd2650_create_disk(int whichdrive);
EXPORT void cd2650_dir_disk(FLAG quiet, int whichdrive);
EXPORT FLAG cd2650_load_disk(FLAG wantasl, int whichdrive);
EXPORT FLAG cd2650_extract_file(int whichfile);
EXPORT void cd2650_inject_file(STRPTR thefilename);
EXPORT void cd2650_updatecharset(void);
EXPORT void cd2650_changebios(void);
EXPORT FLAG load_imag(int localsize);
EXPORT FLAG save_imag(STRPTR filename, int startaddr, int endaddr, int progstart);
EXPORT void cd2650_delete_file(int whichfile, int whichdrive);
EXPORT FLAG cd2650_rename_file(int whichfile, STRPTR newname);
EXPORT void cd2650_view_basic(void);
EXPORT void cd2650_reset(void);
EXPORT void float_to_basic(float number, int start);
EXPORT float basic_to_float(int start);
EXPORT void print_float(int start);
EXPORT FLAG zisnormal(int start);
EXPORT void cd2650_serialize_cos(void);

// malzak.c
EXPORT void malzak_drawscreen(void);
EXPORT void switchbank(int whichbank);
EXPORT void malzak_setmemmap(void);
EXPORT void malzak_viewscreen(void);
EXPORT UBYTE malzak_readport(int port);
EXPORT void malzak_writeport(int port, UBYTE data);
EXPORT void malzak_emuinput(void);
EXPORT void malzak_savenvram(void);
EXPORT void malzak_drawhelpgrid(void);
EXPORT void malzak2_updatedips(void);

// zaccaria.c
EXPORT void astrowars_drawscreen(void);
EXPORT void astrowars_setmemmap(void);
EXPORT void coinop_record(void);
EXPORT void coinop_play(void);
EXPORT void galaxia_drawscreen(void);
EXPORT void galaxia_setmemmap(void);
EXPORT void lb_setmemmap(void);
EXPORT void lb_drawscreen(void);
EXPORT FLAG update_coinaval(int value);
EXPORT FLAG update_coinbval(int value);
EXPORT void update_collval(int value);
EXPORT void update_freezeval(int value);
EXPORT void update_gridval(int value);
EXPORT FLAG update_hiscoreval(int value);
EXPORT FLAG update_livesval(int value);
EXPORT void update_rapidfireval(int value);
EXPORT void view_tms(void);
EXPORT void zaccaria_drawhelpgrid(void);
EXPORT FLAG zaccaria_edit_screen(UWORD code);
EXPORT void zaccaria_emuinput(void);
EXPORT void zaccaria_ghostdips(void);
EXPORT UBYTE zaccaria_readport(int port);
EXPORT void zaccaria_updatedips(void);
EXPORT void zaccaria_viewscreen(void);
EXPORT void zaccaria_writeport(int port, UBYTE data);

// phunsy.c
EXPORT void phunsy_emulate(void);
EXPORT UBYTE phunsy_readport(int port);
EXPORT void phunsy_writeport(int port, UBYTE data);
EXPORT void phunsy_setmemmap(void);
EXPORT void ramtoubank(int bank);
EXPORT void ramtoqbank(int bank);
EXPORT void ubanktoram(int bank);
EXPORT void qbanktoram(int bank);
EXPORT void phunsy_viewscreen(void);
EXPORT float phunsy_getpianopitch(UWORD value);
EXPORT float phunsy_getbelmachpitch(UWORD value);
EXPORT void phunsy_biosdetails(int ea);
EXPORT void mdcr_exec(void);
EXPORT void create_mdcr(void);
EXPORT FLAG insert_mdcr(FLAG wantasl);
EXPORT FLAG update_mdcr(void);
EXPORT void mdcr_show(STRPTR thestring, int theval);
EXPORT void phunsy_drawhelpgrid(void);
EXPORT FLAG phunsy_edit_screen(UWORD code);
EXPORT void phunsy_reset(void);

// pong.c
EXPORT void pong_setmemmap(void);
EXPORT void pong_simulate(void);
EXPORT void pong_newgame(void);
EXPORT void pong_serialize_cos(void);
EXPORT void pong_updatedips(void);
EXPORT void updatebatsize(void);
EXPORT void fixx(void);
EXPORT void fixy(void);

// selbst.c
EXPORT void selbst_emulate(void);
EXPORT void selbst_setmemmap(void);
EXPORT UBYTE selbst_readport(int port);
EXPORT void selbst_writeport(int port, UBYTE data);
EXPORT void selbst_viewscreen(void);
EXPORT void selbst_biosdetails(int ea);
EXPORT void selbst_drawhelpgrid(void);
EXPORT void selbst_reset(void);

// mikit.c
EXPORT void mikit_emulate(void);
EXPORT void mikit_setmemmap(void);
EXPORT UBYTE mikit_readport(int port);
EXPORT float mikit_retune(UBYTE hertz);
EXPORT void mikit_viewscreen(void);
EXPORT void mikit_writeport(int port, UBYTE data);
EXPORT void mikit_reset(void);

// tr.c
EXPORT void tr_simulate(void);
EXPORT void tr_serialize_cos(void);
EXPORT void tr_setmemmap(void);
EXPORT void tr_reset(void);
EXPORT void tr_update_capslock(void);
EXPORT void tr_viewscreen(void);

// aa/wa-gfx.c
EXPORT void apply_scanlines(void);
EXPORT void drawpixelroutine(void);
EXPORT void drawpixel_null(int x, int y, int colour);
EXPORT UBYTE getred(int colourset, int whichpen);
EXPORT UBYTE getgreen(int colourset, int whichpen);
EXPORT UBYTE getblue(int colourset, int whichpen);
EXPORT int getsmallimage1(int thegame, int thememmap);
EXPORT void hosttoguestmouse(int* hostx, int* hosty, int* guestx, int* guesty, int* remx, int* remy);
EXPORT void make_stars(void);
EXPORT void measure_position(void);
EXPORT void resetcolour(int thecolourset, int whichcolour, FLAG update);
EXPORT void setred(int colourset, int whichpen, UBYTE value);
EXPORT void setgreen(int colourset, int whichpen, UBYTE value);
EXPORT void setblue(int colourset, int whichpen, UBYTE value);
#ifdef AMIGA
    EXPORT void make_viewaslist(void);
    EXPORT void setup_bm(int whichcanvas, UBYTE whichpen);
#endif

// aa/wa-help.c
EXPORT void drawctrlglow_system(FLAG lit);

// aa/wa-sound.c
EXPORT void play_any(int guestchan, float hertz, int volume);
EXPORT void sound_close(int guestchan);
EXPORT void sound_off(FLAG force);
EXPORT void sound_on(FLAG hadpaused);
EXPORT void sound_preinit(void);
EXPORT void sound_stop(int guestchan);
EXPORT FLAG open_narrator(void);
#ifdef WIN32
    EXPORT FLAG change_voice(WCHAR* desired);
    EXPORT int list_voices(HWND winptr);
#endif

#ifdef WIN32
// hqx.c
EXPORT void hqxInit(void);

// wa-gfx.c
EXPORT void cleanexit_3d(void);
EXPORT void clearscreen(void);
EXPORT void closewindow_3d(void);
EXPORT void make_display(void);
EXPORT void openwindow_3d(void);
EXPORT void update_menuheight(void);

// wa-modal.c
EXPORT BOOL CALLBACK   RearrangeDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
EXPORT BOOL CALLBACK    RedefineDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
EXPORT BOOL CALLBACK SensitivityDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

// wa-sound.c
EXPORT void sound_die(void);
#endif

// unzip.c
EXPORT unzFile unzOpen(const char* path);
EXPORT int unzGoToFirstFile(unzFile file);
EXPORT int unzGoToNextFile(unzFile file);
EXPORT int unzClose(unzFile file);
EXPORT int unzGetCurrentFileInfo(unzFile file, unz_file_info* pfile_info, char* szFileName, ULONG fileNameBufferSize, void* extraField, ULONG extraFieldBufferSize, char* szComment, ULONG commentBufferSize);
EXPORT int unzOpenCurrentFile(unzFile file);
EXPORT int unzCloseCurrentFile(unzFile file);
EXPORT int unzReadCurrentFile(unzFile file, void* buf, unsigned len);

// ff.c
EXPORT void ff_on(int joy, int amount);
EXPORT void ff_off(void);
EXPORT void ff_decay(int joy);

// png.c
EXPORT UBYTE* write_png(UBYTE* pImage, int w, int h);

// aa/wa-modal.c
EXPORT void adjust_sound(void);

#ifdef WIN32
// ff.c
    EXPORT void ff_init(void);
    EXPORT void ff_initjoys(void);
    EXPORT void ff_initjoy(int joy);
    EXPORT void ff_ReadJoystick(UWORD joynum);
    EXPORT void ff_uninitjoys(void);
    EXPORT void ff_die(void);
// speech
    EXPORT void speech_init(void);
    EXPORT void speech_setrate(int rate);
    EXPORT void speech_die(void);
#endif

#ifdef AMIGA
    // aa-sound.c
    EXPORT void changesoundoutput(void);
    EXPORT void close_narrator(void);
    EXPORT void free_sound(void);
    EXPORT void setup_sound(void);
    EXPORT void update_mouth(void);

    // menus.c
    EXPORT void TurnOffMenu1(int which);
    EXPORT void TurnOffMenu2(int which);
    EXPORT void TurnOnMenu1(int which);
    EXPORT void TurnOnMenu2(int which);
    EXPORT void createmenus(void);
    EXPORT void enable1(int which, int enabled);
    EXPORT FLAG ischecked_toggle(int command);
    EXPORT FLAG ischecked_mx(int command);
#endif
#ifdef __MORPHOS__
    #define strupr strupr_mos
#endif
#ifdef __VBCC__
    EXPORT int stricmp(const char* s1, const char* s2);
    EXPORT int strnicmp(const char *s1, const char *s2, size_t len);
    EXPORT char* strupr(char* s);
    EXPORT char* strlwr(char* s);
    EXPORT double strtod(const char* string, char** endPtr);
#endif
#ifdef AMIGA
    #ifdef __amigaos4__
        #include <proto/socket.h>
    #else
        #define SOCK_STREAM 1
        #define AF_INET     2

#ifndef __MORPHOS__
        struct in_addr
        {   ULONG          s_addr;
        };
#endif
        struct sockaddr
        {   UBYTE          sa_len;      // total length
            UBYTE          sa_family;   // address family
            char           sa_data[14]; // actually longer; address value
        };
#ifndef __MORPHOS__
        struct sockaddr_in
        {   UBYTE          sin_len;
            UBYTE          sin_family;
            USHORT         sin_port;
            struct in_addr sin_addr;
            char           sin_zero[8];
        };
#endif
        struct hostent
        {   char*          h_name;
            char**         h_aliases;
            int            h_addrtype;
            int            h_length;
            char**         h_addr_list;
        };

        struct hostent* gethostbyname(const UBYTE* name);
        LONG  recv(LONG s, UBYTE* buf, LONG len, LONG flags); /* V3 */
        LONG  CloseSocket(LONG d);
        LONG  connect(LONG s, const struct sockaddr* name, LONG namelen);
        LONG  send(LONG s, const UBYTE* msg, LONG len, LONG flags);
        LONG  socket(LONG domain, LONG type, LONG protocol);
        LONG  shutdown(LONG s, LONG how);
#ifndef __MORPHOS__
        char* Inet_NtoA(struct in_addr in);
        ULONG inet_addr(char* cp);
#endif
        LONG  bind(LONG s, const struct sockaddr* name, LONG namelen);
        LONG  listen(LONG s, LONG backlog);
        LONG  accept(LONG s, struct sockaddr* addr, LONG* addrlen);
        LONG  IoctlSocket(LONG d, ULONG request, char* argp);
        LONG  gethostname(STRPTR hostname, LONG size); /* V3 */
    #endif
    #if !defined(__amigaos4__) && !defined(__MORPHOS__)
        #pragma libcall SocketBase socket         1E 21003
        #pragma libcall SocketBase bind           24 18003
        #pragma libcall SocketBase listen         2A 1002
        #pragma libcall SocketBase accept         30 98003
        #pragma libcall SocketBase connect        36 18003
        #pragma libcall SocketBase send           42 218004
        #pragma libcall SocketBase recv           4E 218004
        #pragma libcall SocketBase shutdown       54 1002
        #pragma libcall SocketBase IoctlSocket    72 81003
        #pragma libcall SocketBase CloseSocket    78 001
        #pragma libcall SocketBase Inet_NtoA      AE 001
        #pragma libcall SocketBase inet_addr      B4 801
        #pragma libcall SocketBase gethostbyname  D2 801
        #pragma libcall SocketBase gethostname   11A 802
    #endif
#endif
