// INCLUDES---------------------------------------------------------------

#include "da.h"
#include "engine.h"
#include "games.h"
#include "overlays.h"

#include <stdlib.h> // for FILE*
#include <string.h> // for strcpy()
#include <errno.h>  // for errno

#define RC_CLIENT_SUPPORTS_HASH
#include "rc_client.h"
#include "rc_consoles.h"

// DEFINEDS---------------------------------------------------------------

// #define TESTINDICATORS
// whether to test the RetroAchievements indicators

typedef struct
{   rc_client_server_callback_t callback; // The callback function
    void* callback_data;                  // Additional data passed to the callback
} async_callback_data;

#define RA_RED    1
#define RA_BLUE   2
#define RA_YELLOW 3
#define RA_GREEN  4
#define RA_ORANGE 5

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT FLAG       bangercharging           = FALSE,
                  guestplaying[TOTALCHANNELS],
                  progressing              = FALSE,
                  tracking                 = FALSE;
EXPORT UBYTE      circle,
                  coinignore,
                  cosversion,
                  hx[2], hy[2],
                  ioport[258],
                  startaddr_h,
                  startaddr_l;
EXPORT UWORD      console[4],
                  guestpitch[GUESTCHANNELS];
EXPORT ULONG      analog                   = FALSE,
                  autofire                 = FALSE,
                  collisions               = TRUE,
                  demultiplex              = TRUE,
                  downframes               = 4,
                  frames,
                  hinput[2],
                  linebased                = FALSE,
                  oldcycles,
                  region                   = REGION_PAL,
                  sound                    = TRUE,
                  swapped                  = FALSE,
                  totalframes              = 16;
EXPORT int        absxmin, absxmax,
                  absymin, absymax,
                  coinop_1p,
                  coinop_2p,
                  coinop_coina,
                  coinop_coinb,
                  coinop_joy1left,
                  coinop_joy1right,
                  coinop_joy1up,
                  coinop_joy1down,
                  coinop_joy2left,
                  coinop_joy2right,
                  coinop_button1,
                  coinop_button2,
                  coinop_button3,
                  coinop_button4,
                  colourset                = 0,
                  console_start            = 0,
                  console_a                = 0,
                  console_b                = 0,
                  console_reset            = 0,
                  cpl,
                  cpux,
                  cpuy,
                  dejitter                 = FALSE,
                  drawcorners              = FALSE,
                  elektor_biosver          = ELEKTOR_PHILIPS,
                  firstrow                 = 26,
                  game                     = FALSE,
                  hidetop,
                  key1                     =  2,
                  key2                     =  1,
                  key3                     =  3,
                  key4                     =  0,
                  lastrow                  = -1,
                  layout                   = W8,
                  machine                  = ARCADIA,
                  memmap                   = MEMMAP_ARCADIA,
                  n1, n2, n3, n4,
                  nextinst,
                  offset,
                  p1rumble                 = 0,
                  p2rumble                 = 0,
                  p1bgcol[4],
                  p2bgcol[4],
                  p1sprcol[6],
                  p2sprcol[6],
                  paddleup                 = -1,
                  paddledown               = -1,
                  paddleleft               = -1,
                  paddleright              = -1,
                  ppc,
                  psgbase,
                  pvibase,
                  retune                   = FALSE,
                  romsize,
                  samplerate               = 11025,
                  serializemode,
                  SoundLength[TOTALCHANNELS], // in samples (ie. bytes for 8-bit sound or words for 16-bit sound)
                  throb,
                  trainer_lives            = FALSE,
                  trainer_time             = FALSE,
                  trainer_invincibility    = FALSE,
                  udcflips                 = 0,
                  undither                 = FALSE,
                  useff                    = TRUE,
                  whichgame                = -1,
                  whichoverlay;
EXPORT float      dividend;
EXPORT TEXT       fn_game[1024 + 1];
EXPORT ASCREEN    screen[MAXBOXWIDTH][MAXBOXHEIGHT];
EXPORT UBYTE*     IOBuffer;
EXPORT UWORD*     display;
EXPORT SWORD      SoundBuffer[TOTALCHANNELS][SOUNDLENGTH * 2];
EXPORT int        filesize;

#ifndef LIBRETRO
EXPORT JavaVM*    g_vm;
#endif

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT FLAG       multisprite[4],
                  protect[4];
IMPORT SBYTE      galaxia_scrolly;
IMPORT UBYTE      abeff1, abeff2,
                  awga_collide,
                  g_bank1[1024],
                  g_bank2[16],
                  lb_bank,
                  lb_spritecode,
                  lb_spritecolour,
                  lb_spriteenable,
                  lb_spritex,
                  lb_spritey,
                  lb_tone,
                  aw_dips1,
                  aw_dips2,
                  ga_dips,
                  lb_dips,
                  lz_dips2,
                  lz_dips3,
                  malzak_field[16][16],
                  memflags[32768],
                  memory[32768],
                  memory_effects[512],
                  mux,
                  neg1, neg2,
                  psu,
                  psl,
                  r[7],
                  squeal,
                  useshell;
IMPORT UWORD      iar,
                  ras[8];
IMPORT ULONG      cycles_2650,
                  fractionator,
                  lb_snd;
IMPORT int        darkenbg,
                  flagline,
                  frac[4],
                  interrupt_2650,
                  malzak_x,
                  malzak_y,
                  malzak_bank1, // text data bank
                  malzak_bank2, // terrain data bank
                  slice_2650,
                  spriteflip,
                  spriteflips,
                  udcflip;

IMPORT int        errno; // from errno.h

// MODULE VARIABLES-------------------------------------------------------

MODULE FLAG       badgemode    = FALSE,
                  cheevos      = FALSE, // must be FALSE!
                  hardcore     = TRUE,
                  spring       = TRUE;
MODULE UBYTE      chan_volume[GUESTCHANNELS],
                  cheevomem[12 * KILOBYTE],
                  fgtable[BOXHEIGHT][BOXWIDTH],
                  oldguestvolume[GUESTCHANNELS];
MODULE SWORD      downvol_16[GUESTCHANNELS],
                  upvol_16[GUESTCHANNELS];
MODULE UWORD      oldguestpitch[GUESTCHANNELS],
                  stars[MAXBOXHEIGHT][MAXBOXWIDTH];
MODULE ULONG      cheevos_buffer_size,
                  da_crc32_table[256],
                  bangfrom,
                  rc1,
                  rc2;
MODULE int        autocoin     = TRUE,
                  autofire2    = 0,
                  chan_status[TOTALCHANNELS],
                  cheevosize,
                  enhancestars = FALSE,
                  hostvol      = 8,
                  lock         = TRUE,
                  post         = FALSE,
                  sameplayer   = TRUE,
                  sensitivity  = SENSITIVITY_DEFAULT,
                  speed        = SPEED_4QUARTERS;
MODULE float      chan_hertz[GUESTCHANNELS];

// RetroAchievements
MODULE      TEXT               badgegfxurl[128    ] = "",
                               gamegfxurl[ 128    ] = "",
                               cmessage[   512 + 1] = "",
                               username[    80 + 1] = "",
                               password[    80 + 1] = "",
                               progressstr[ 24    ] = "",
                               progressurl[128    ] = "",
                               trackerstr[  24    ] = "";
MODULE       int               racolour,
                               ramachine;
MODULE       rc_client_t*      g_client = NULL;
MODULE const rc_client_game_t* ragame   = NULL;
    
// EXPORTED STRUCTURES----------------------------------------------------

EXPORT struct MachineStruct machines[MACHINES] =                  // keys      vers     cpf pvis digits                                                                     hiscores
{ {               0,                0, MEMMAP_ARCADIA,   REGION_NTSC, 48, 0xFF,  5,     0.0, 0,  0, { 1, 112, 254 },      0, FALSE, { "START" , "A"     , "B"     , "RESET"  }, 51 },
  {               0,                0, MEMMAP_D,         REGION_PAL , 40,    0,  5,     0.0, 1,  0, { 1, 112, 225 }, 0x1F00, FALSE, { "START" , "SELECT", "-"     , "RESET"  },  0 },
  {               0,                0, MEMMAP_F,         REGION_PAL , 36,    0,  5,     0.0, 1,  0, { 1, 111, 225 }, 0x1F00, FALSE, { "START" , "UC"    , "LC"    , "RESET"  },  0 },
  { COINOP_BOXWIDTH, COINOP_BOXHEIGHT, MEMMAP_ASTROWARS, REGION_PAL ,  0,    0, 34, 23608.0, 0,  0, { 0, 128, 255 }, 0x1500, TRUE , { "1UP"   , "2UP"   , "Coin A", "Coin B" },  6 },
  { COINOP_BOXWIDTH, COINOP_BOXHEIGHT, MEMMAP_MALZAK1,   REGION_PAL , 11,    0, 34,     0.0, 2,  0, { 0, 128, 255 }, 0x1400, TRUE , { "1UP"   , "2UP"   , "Coin"  , "Switch" },  6 },
};

EXPORT struct MemMapInfoStruct memmapinfo[MEMMAPS] = {
{ INTERTON  , "Interton VC 4000, type \"A\" (2K ROM + 0K RAM)",    INTERTON_OVERLAY, ""                      }, // MEMMAP_A
{ INTERTON  , "Interton VC 4000, type \"B\" (4K ROM + 0K RAM)",    INTERTON_OVERLAY, ""                      }, // MEMMAP_B
{ INTERTON  , "Interton VC 4000, type \"C\" (4K ROM + 1K RAM)",    INTERTON_OVERLAY, ""                      }, // MEMMAP_C
{ INTERTON  , "Interton VC 4000, type \"D\" (6K ROM + 1K RAM)",    INTERTON_OVERLAY, ""                      }, // MEMMAP_D
{ ELEKTOR   , "Elektor TV Games Computer (basic)"             ,     ELEKTOR_OVERLAY, ""                      }, // MEMMAP_E
{ ELEKTOR   , "Elektor TV Games Computer (expanded)"          ,     ELEKTOR_OVERLAY, ""                      }, // MEMMAP_F
{ ARCADIA   , "Emerson Arcadia 2001, type \"G\" (Emerson)"    ,     ARCADIA_OVERLAY, ""                      }, // MEMMAP_G
{ ARCADIA   , "Emerson Arcadia 2001, type \"H\" (Tele-Fever)" ,     ARCADIA_OVERLAY, ""                      }, // MEMMAP_H
{ ARCADIA   , "Emerson Arcadia 2001, type \"I\" (Palladium)"  ,     ARCADIA_OVERLAY, ""                      }, // MEMMAP_I
{ ZACCARIA  , "Coin-op (Astro Wars)"                          ,   ASTROWARS_OVERLAY, "Zaccaria, 1980"        }, // MEMMAP_ASTROWARS
{ ZACCARIA  , "Coin-op (Galaxia)"                             ,     GALAXIA_OVERLAY, "Zaccaria, 1979"        }, // MEMMAP_GALAXIA
{ ZACCARIA  , "Coin-op (Laser Battle)"                        , LASERBATTLE_OVERLAY, "Zaccaria, 1981"        }, // MEMMAP_LASERBATTLE
{ ZACCARIA  , "Coin-op (Lazarian)"                            , LASERBATTLE_OVERLAY, "Zaccaria/Midway, 1981" }, // MEMMAP_LAZARIAN
{ MALZAK    , "Coin-op (Malzak 1)"                            ,     MALZAK1_OVERLAY, "Kitronix, 1981?"       }, // MEMMAP_MALZAK1
{ MALZAK    , "Coin-op (Malzak 2)"                            ,     MALZAK2_OVERLAY, "Kitronix, 1981?"       }, // MEMMAP_MALZAK2
};

EXPORT const UWORD pencolours[4][GUESTCOLOURS] = {
//                                     RRRR,R GGG,GGG B,BBBB
{   0xFFFF, // white  (UVI #0, PVI #0) 1111,1 111,111 1,1111
    0xFFE0, // yellow (UVI #1, PVI #1) 1111,1 111,111 0,0000
    0xF81F, // purple (UVI #4, PVI #2) 1111,1 000,000 1,1111
    0xF800, // red    (UVI #5, PVI #3) 1111,1 000,000 0,0000
    0x07FF, // cyan   (UVI #2, PVI #4) 0000,0 111,111 1,1111
    0x06E0, // green  (UVI #3, PVI #5) 0000,0 110,111 0,0000
    0x423F, // blue   (UVI #6, PVI #6) 0100,0 010,001 1,1111
    0x0000, // black  (UVI #7, PVI #7) 0000,0 000,000 0,0000
    0xCE18, // grey #1                 1100,1 110,000 1,1000
    0xBDC0, // dark yellow             1011,1 101,110 0,0000
    0xB817, // dark purple             1011,1 000,000 1,0111
    0x9800, // dark red                1001,1 000,000 0,0000
    0x0555, // dark cyan               0000,0 101,010 1,0101
    0x0540, // dark green              0000,0 101,010 0,0000
    0x0015, // dark blue               0000,0 000,000 1,0101
    0x0000, // dark black              0000,0 000,000 0,0000
    0x2124, // grey #2                 0010,0 001,001 0,0100
    0x3186, // grey #3                 0011,0 001,100 0,0110
    0x6B6D, // grey #4                 0110,1 011,011 0,1101
    0x9494, // grey #5                 1001,0 100,100 1,0010
    0xB5B6, // grey #6                 1011,0 101,101 1,0110
    0xDEDB, // grey #7                 1101,1 110,110 1,1011
    0xFC00, // orange                  1111,1 100,000 0,0000
    0xFC10, // pink                    1111,1 100,000 1,0000
}, { // PVI colours                    RRRR,R    GGG,GGG   B,BBBB
    0xFFFF, // white  (UVI #0, PVI #0) 1111,1    111,111   1,1111
    0xB5A1, // yellow (UVI #1, PVI #1) 1011,0    101,101   0,0001
            // yellow                  1011 0100 101 10100 0 0001000 B4 B4 08
    0xB056, // purple (UVI #4, PVI #2) 1011,0    000,010   1,0110
    0xF0E0, // red    (UVI #5, PVI #3) 1111,0    000,111   0,0000
            // red                     1111 0001 000 11100 0 0000000 F1 1C 00
    0x0DD7, // cyan   (UVI #2, PVI #4) 0000,1    101,110   1,0111
    0x15A2, // green  (UVI #3, PVI #5) 0001,0    101,101   0,0010
    0x197F, // blue   (UVI #6, PVI #6) 0001,1    001,011   1,1111
            // blue                    0011 1111 001 01110 1 1111011 3F 4E FB
    0x0000, // black  (UVI #7, PVI #7) 0000,0    000,000   0,0000
    0xAB6A, // grey #1                 1010,1    101,011   0,1010
            // grey #1                 1010 1010 101 01101 0 1010010 AA AD 52
    0xA9D4, // dark yellow             1010,1    001,110   1,0100
            // dark yellow             1010 1000 001 11000 1 0100100 A8 38 A4
    0xA9D4, // dark purple             1010,1    001,110   1,0100
    0x8164, // dark red                1000,0    001,011   0,0100
    0x44D3, // dark cyan               0100,0    100,110   1,0011
    0x0480, // dark green              0000,0    100,100   0,0000
            // dark green              0000 0001 100 10000 0 0000000 01 90 00
    0x3171, // dark blue               0011,0    001,011   1,0001
    0x0000, // dark black              0000,0    000,000   0,0000
    0x2925, // grey #2                 0010,1    001,001   0,0101
    0x4A49, // grey #3                 0100,1    010,010   0,1001         
    0x766E, // grey #4                 0111,0    011,011   0,1110
    0x9491, // grey #5                 1001,0    100,100   1,0001
    0xBD77, // grey #6                 1011,1    101,011   1,0111
    0xDEFB, // grey #7                 1101,1    110,111   1,1011
    0xFC00, // orange                  1111,1    100,000   0,0000
    0xFC10, // pink                    1111,1    100,000   1,0000
}, { // UVI colours                    RRRR,R GGG,GGG B,BBBB
    0xFFFF, // white  (UVI #0, PVI #0) 1111,1 111,111 1,1111
    0xFFED, // yellow (UVI #1, PVI #1) 1111,1 111,111 0,1101
    0xD97B, // purple (UVI #4, PVI #2) 1101,1 001,011 1,1011
    0xF800, // red    (UVI #5, PVI #3) 1111,1 000,000 0,0000
    0x67FF, // cyan   (UVI #2, PVI #4) 0110,0 111,111 1,1111
    0x4FA8, // green  (UVI #3, PVI #5) 0100,1 111,101 0,1000
    0x0035, // blue   (UVI #6, PVI #6) 0000,0 000,001 1,0101
    0x0000, // black  (UVI #7, PVI #7) 0000,0 000,000 0,0000
    0xBDF7, // grey #1                 1011,1 101,111 1,0111
    0xBDE0, // dark yellow             1011,1 101,111 0,0000
    0xB817, // dark purple             1011,1 000,000 1,0111
    0x9800, // dark red                1001,1 000,000 0,0000
    0x0555, // dark cyan               0000,0 101,010 1,0101
    0x0540, // dark green              0000,0 101,010 0,0000
    0x000F, // dark blue               0000,0 000,000 0,1111
    0x0000, // dark black              0000,0 000,000 0,0000
    0x2925, // grey #2                 0010,1 001,001 0,0101
    0x4A49, // grey #3                 0100,1 010,010 0,1001         
    0x766E, // grey #4                 0111,0 011,011 0,1110
    0x9491, // grey #5                 1001,0 100,100 1,0001
    0xBD77, // grey #6                 1011,1 101,011 1,0111
    0xDEFB, // grey #7                 1101,1 110,111 1,1011
    0xFC00, // orange                  1111,1 100,000 0,0000
    0xFC10, // pink                    1111,1 100,000 1,0000
}, { // greyscale                      RRRR,R GGG,GGG B,BBBB
    0xFFFF, // white             32    1111,1 111,111 1,1111
    0xE71C, // yellow            28    1110,0 111,000 1,1100
    0x6B4D, // purple            13    0110,1 011,010 0,1101
    0x4A69, // red               9.5   0100,1 010,011 0,1001
    0xB596, // cyan              22    1011,0 101,100 1,0110
    0x8430, // green             16.5  1000,0 100,001 1,0000
    0x4A49, // blue              9     0100,1 010,010 0,1001
    0x0000, // black             0     0000,0 000,000 0,0000
    0xBDD7, // dark white        23    1011,1 101,110 1,0111
    0xAD55, // dark yellow       21    1010,1 101,010 1,0101
    0x4A69, // dark purple       9.5   0100,1 010,011 0,1001
    0x3186, // dark red          6     0011,0 001,100 0,0110
    0x7BCF, // dark cyan         15    0111,1 011,110 0,1111
    0x738E, // dark green        14    0111,0 011,100 0,1110
    0x2104, // dark blue         4     0010,0 001,000 0,0100
    0x0000, // dark black        0     0000,0 000,000 0,0000
    0x2945, // grey #2           5     0010,1 001,010 0,0101
    0x4A49, // grey #3           9     0100,1 010,010 0,1001
    0x738E, // grey #4           14    0111,0 011,100 0,1110
    0x9492, // grey #5           18    1001,0 100,100 1,0010
    0xBDD7, // grey #6           23    1011,1 101,110 1,0111
    0xDEDB, // grey #7           27    1101,1 110,110 1,1011
    0xA514, // orange            20    1010,0 101,000 1,0100
    0xA514, // pink              20    1010,0 101,000 1,0100
} };

EXPORT const SWORD volume_3to16[8] =
{      0,
     585, // 4095/7*1
    1170,
    1755,
    2340,
    2925,
    3510,
    4095
}, volume_4to16[16] =
{      0,
     273, // 4095/15*1
     546,
     819,
    1092,
    1365,
    1638,
    1911,
    2184,
    2457,
    2730,
    3003,
    3276,
    3549,
    3822,
    4095
};

// MODULE FUNCTIONS-------------------------------------------------------

MODULE FLAG autosense(int thesize, FLAG same);
MODULE void make_stars(void);
MODULE void updatehiscores(void);
MODULE void do_autocoin(void);
MODULE int resolvegame(void);

// RetroAchievements
MODULE uint32_t readmemory(uint32_t address, uint8_t* buffer, uint32_t num_bytes, rc_client_t* client);
MODULE void httpcallback(int status_code, const char* content, size_t content_size, void* userdata, const char* error_message);
MODULE void logincallback(int result, const char* error_message, rc_client_t* client, void* userdata);
MODULE void servercall
(   const rc_api_request_t*     request,
    rc_client_server_callback_t callback,
    void*                       callback_data,
    rc_client_t*                client
);
MODULE void loadgamecallback(int result, const char* error_message, rc_client_t* client, void* userdata);
MODULE void show_game_placard(void);
MODULE void achievement_triggered(const rc_client_achievement_t* achievement);
MODULE void event_handler(const rc_client_event_t* event, rc_client_t* client);
MODULE void rainit(void);
MODULE void cshutdownraclient(void);
#ifndef LIBRETRO
MODULE void csetravars(JNIEnv* env, jstring jusername, jstring jpassword, jboolean jcheevos, jboolean jhardcore);
#endif
MODULE void identifyragame(void);

// CODE-------------------------------------------------------------------

EXPORT void drawpixel(int x, int y, int colour)
{   *(display + (y * MAXBOXWIDTH) + x) = pencolours[colourset][colour];
}

EXPORT void drawbgpixel(int x, int y, int colour)
{   if (enhancestars)
    {   *(display + (y * MAXBOXWIDTH) + x) = stars[y][x];
    } else
    {   *(display + (y * MAXBOXWIDTH) + x) = pencolours[colourset][colour];
}   }

MODULE void updatehiscores(void)
{   int bestscore  = 0,
        i, j,
        multiplier = 0,  // initialized to avoid spurious SAS/C compiler warnings
        newscore,
        resolved   = -1; // initialized to avoid spurious SAS/C compiler warnings

    resolved = resolvegame();

    for (i = 0; i < 4; i++)
    {   if (hiscore[resolved].hipos[i] == 0)
        {   break;
        } // implied else

        // assert(whichgame != -1);
        if (resolved == known[BASEBALLPOS].hiscore)
        {   // assert(machine == ARCADIA);

            if
            (   (memory[hiscore[resolved].hipos[i] + 1] & 0x3F) >= 0x10 // '0'
             && (memory[hiscore[resolved].hipos[i] + 1] & 0x3F) <= 0x19 // '9'
            )
            {   newscore = (((memory[hiscore[resolved].hipos[i]    ] & 0x3F) - 0x10) * 10)
                         +  ((memory[hiscore[resolved].hipos[i] + 1] & 0x3F) - 0x10);
            } else
            {   newscore =   (memory[hiscore[resolved].hipos[i]    ] & 0x3F) - 0x10;
        }   }
        else
        {   newscore = 0;

            switch (hiscore[resolved].length)
            {
            case  1: multiplier =      1;
            acase 2: multiplier =     10;
            acase 3: multiplier =    100;
            acase 4: multiplier =   1000;
            acase 5: multiplier =  10000;
            acase 6: multiplier = 100000;
            }

            switch (machine)
            {
            case ARCADIA:
                for (j = 0; j < hiscore[resolved].length; j++)
                {   if
                    (   (memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)] & 0x3F) >= 0x10 // '0'
                     && (memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)] & 0x3F) <= 0x19 // '9'
                    )
                    {   newscore += ((memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)] & 0x3F) - 0x10) * multiplier;
                    }
                    multiplier /= 10;
                }
            acase MALZAK:
                for (j = 0; j < hiscore[resolved].length; j++)
                {   if
                    (   (memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)]) >= 0x30 // '0'
                     && (memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)]) <= 0x39 // '9'
                    )
                    {   newscore += ((memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)]) - 0x30) * multiplier;
                    }
                    multiplier /= 10;
                }
            acase ZACCARIA:
                if (memmap == MEMMAP_GALAXIA)
                {   for (j = 0; j < hiscore[resolved].length; j++)
                    {   if
                        (   (memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)] & 0x7F) >= 0x60 // '0'
                         && (memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)] & 0x7F) <= 0x69 // '9'
                        )
                        {   newscore += ((memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)] & 0x7F) - 0x60) * multiplier;
                        }
                        multiplier /= 10;
                }   }
                else
                {   // assert(memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN);
                    for (j = 0; j < hiscore[resolved].length; j++)
                    {   if
                        (   (memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)]) >= 0xB0 // '0'
                         && (memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)]) <= 0xB9 // '9'
                        )
                        {   newscore += ((memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)]) - 0xB0) * multiplier;
                        }
                        multiplier /= 10;
        }   }   }   }

        if (newscore > bestscore)
        {   bestscore = newscore;
    }   }

    if (bestscore > (int) hiscore[resolved].score)
    {   hiscore[resolved].score = (ULONG) bestscore;
        // datestamp(resolved);
}   }

MODULE int resolvegame(void)
{   switch (machine)
    {
    case ARCADIA:
        if (whichgame != -1)          return known[whichgame].hiscore;
    acase ZACCARIA:
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:        return HS_ASTROWARS_POS;
        case MEMMAP_GALAXIA:          return HS_GALAXIA_POS;
        case MEMMAP_LASERBATTLE:      return HS_LASERBATTLE_POS;
        case MEMMAP_LAZARIAN:         return HS_LAZARIAN_POS;
        }
    acase MALZAK:
        switch (memmap)
        {
        case MEMMAP_MALZAK1:          return HS_MALZAK1_POS;
        case MEMMAP_MALZAK2:          return HS_MALZAK2_POS;
    }   }

    return -1;
}

#ifndef LIBRETRO
JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_MainActivity_setframebuffer(JNIEnv* env, jobject this, jobject oBuf)
{   int i, x, y;

    display = (UWORD*) (*env)->GetDirectBufferAddress(env, oBuf);

    da_generate_crc32table();
    
    SoundLength[GUESTCHANNELS + SAMPLE_EXPLOSION] = LEN_EXPLOSION;
    SoundLength[GUESTCHANNELS + SAMPLE_SHOOT    ] = LEN_SHOOT;
    SoundLength[GUESTCHANNELS + SAMPLE_BANG     ] = LEN_BANG;
    SoundLength[GUESTCHANNELS + SAMPLE_COIN     ] = LEN_COIN;
    SoundLength[GUESTCHANNELS + SAMPLE_CLICK    ] = LEN_CLICK;
    SoundLength[GUESTCHANNELS + SAMPLE_CHEEVOS  ] = LEN_CHEEVOS;

    switch (machine)
    {
    case  ARCADIA:                arcadia_setmemmap();
    acase INTERTON:               interton_setmemmap();
    acase ELEKTOR:                elektor_setmemmap();
    acase ZACCARIA:
        switch (memmap)
        {
        case  MEMMAP_ASTROWARS:   astrowars_setmemmap();
        acase MEMMAP_GALAXIA:     galaxia_setmemmap();
        acase MEMMAP_LASERBATTLE:
        case  MEMMAP_LAZARIAN:    lb_setmemmap();
        }
    acase MALZAK:                 malzak_setmemmap();
    }
    
    update_margins();
    engine_reset();

    for (y = 0; y < MAXBOXHEIGHT; y++)
    {   for (x = 0; x < MAXBOXWIDTH; x++)
        {   screen[x][y] = GREY1;
            *(display + (y * MAXBOXWIDTH) + x) = pencolours[colourset][GREY1];
    }   }
    for (i = 0; i < TOTALCHANNELS; i++)
    {   chan_status[i]  = CHAN_OK;
        guestplaying[i] = FALSE;
}   }

JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_MainActivity_csoundoff(JNIEnv* env, jobject this)
{   int i;

    for (i = 0; i < TOTALCHANNELS; i++)
    {   chan_status[i]  = CHAN_OK;
        guestplaying[i] = FALSE;
}   }
JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_OptionsActivity_csoundoff(JNIEnv* env, jobject this)
{   int i;

    for (i = 0; i < TOTALCHANNELS; i++)
    {   chan_status[i]  = CHAN_OK;
        guestplaying[i] = FALSE;
}   }

JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_MainActivity_endofframe(JNIEnv* env, jobject this)
{   int i;

    frames++;

    if
    (   (machine == ARCADIA || machines[machine].coinop)
     && collisions
     && speed >= SPEED_4QUARTERS
  // && !paused
     && (machine != ARCADIA || (whichgame != -1 && known[whichgame].hiscore != -1))
     && !trainer_lives
     && !trainer_time
     && !trainer_invincibility
    )
    {   updatehiscores();
    }

    if (autocoin && machines[machine].coinop)
    {   do_autocoin();
    }

    rc1 = rc2 = 0;
    for (i = 0; i < TOTALCHANNELS; i++)
    {   if (sound)
        {   if (chan_status[i] == CHAN_PLAY)
            {   if (i >= GUESTCHANNELS)
                {   play_any(i, 0, 0);
                } else
                {   play_any(i, chan_hertz[i], chan_volume[i]);
                }
                rc1 |= (1 << i);
                chan_status[i] = CHAN_OK;
            } elif (chan_status[i] == CHAN_PLAYTHENSTOP)
            {   if (i >= GUESTCHANNELS)
                {   play_any(i, 0, 0);
                } else
                {   play_any(i, chan_hertz[i], chan_volume[i]);
                }
                rc1 |= (1 << i);
                chan_status[i] = CHAN_STOP;
            } elif (chan_status[i] == CHAN_STOP)
            {   guestplaying[i] = FALSE;
                rc2 |= (1 << i);
                chan_status[i] = CHAN_OK;
    }   }   }
    
    if (cheevos && g_client)
    {   rc_client_do_frame(g_client);
}   }

JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_getstarting(JNIEnv* env, jobject this)
{   return (jint) rc1;
}
JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_getstopping(JNIEnv* env, jobject this)
{   return (jint) rc2;
}
#endif /* !LIBRETRO */

EXPORT void changepixel(int x, int y, int colour)
{   if (screen[x][y] != (UBYTE) colour)
    {   screen[x][y] = (UBYTE) colour;
        fgtable[y][x] = 1;
        if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
        {   drawpixel(239 - y, x, colour);
        } else
        {   drawpixel(      x, y, colour);
}   }   }

EXPORT void changebgpixel(int x, int y, int colour)
{   if (screen[x][y] != (UBYTE) colour)
    {   screen[x][y] = (UBYTE) colour;
        fgtable[y][x] = 0;
        if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
        {   drawbgpixel(239 - y, x, colour);
        } else
        {   drawbgpixel(      x, y, colour);
}   }   }

EXPORT void update_margins(void)
{   if (region == REGION_PAL)
    {   n1 =  43; // Y-margin
        n2 = 177; // middle of screen
        n3 = 312; // entire raster
        n4 =   6;
        machines[machine].cpf = 17706.0; // (227 / 4) * 312 = 17,706   CPL. * 50.0804105952784 = 886,723.75 Hz
        dividend = 7812.5; // 64       / 500,000 = 7812.5
        hidetop = 0;
        machines[ARCADIA ].height =
        machines[INTERTON].height =
        machines[ELEKTOR ].height = 269; // ie. 312 - 43
    } else
    {   n1 =  20; // Y-margin
        n2 = 141; // middle of screen
        n3 = 262; // entire raster
        n4 =  10;
        machines[machine].cpf = 14868.5; // (227 / 4) * 262 = 14,868.5 CPL. * 60.1867202475031 = 894,886.25 Hz
        dividend = 7884.5; // 63.41588 / 500,000 = 7884.460485 (was 63.5 / 500,000 = 7874.0)
        hidetop = 8;
        machines[ARCADIA ].height =
        machines[INTERTON].height =
        machines[ELEKTOR ].height = 226; // ie. 262 - 36
    }

    // The XVI's origin (0,0) is at -XVI_HIDELEFT,0 on this screen
    // It omits the entire USG_XMARGIN and an additional HIDELEFT pixels from the left side, and some pixels from the right side.
    // It omits the entire USG_YMARGIN from the top side.

    machines[ARCADIA ].width  = 
    machines[INTERTON].width  =
    machines[ELEKTOR ].width  = 164;

    switch (machine)
    {
    case ARCADIA:
        absxmin = UVI_HIDELEFT + USG_XMARGIN;
        absxmax = UVI_HIDELEFT + USG_XMARGIN + machines[machine].width  - 1;
        absymin = hidetop      + USG_YMARGIN;
        absymax = hidetop      + USG_YMARGIN + machines[machine].height - 1;
    acase INTERTON:
    case ELEKTOR:
        absxmin = PVI_HIDELEFT + USG_XMARGIN;
        absxmax = PVI_HIDELEFT + USG_XMARGIN + machines[machine].width  - 1;
        absymin = hidetop      + USG_YMARGIN;
        absymax = hidetop      + USG_YMARGIN + machines[machine].height - 1;
    acase MALZAK:
    case ZACCARIA:
        absxmin = absymin =   0;
        absxmax = absymax = 255;
}   }

#ifndef LIBRETRO
JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_emulate(JNIEnv* env, jobject this)
{   switch (machine)
    {
    case  ARCADIA:                uvi();
    acase INTERTON: case ELEKTOR: newpvi();
    acase ZACCARIA: case MALZAK:  oldpvi();
    }
    
    return (jint) cycles_2650;
}

JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_getgame(JNIEnv* env, jobject this)
{   return (jint) whichgame;
}
JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_OptionsActivity_getgame(JNIEnv* env, jobject this)
{   return (jint) whichgame;
}
#endif /* !LIBRETRO */

EXPORT void engine_reset(void)
{   int i;

    bangercharging  = FALSE;
    interrupt_2650  = FALSE;
    slice_2650      = 0;
    frames          =
    cycles_2650     =
    oldcycles       =
    nextinst        =
    // no need for bangfrom = 0;
    awga_collide    =
    coinignore      = 0;
    galaxia_scrolly = 0;
    throb           = 0;
    squeal          = 0;

    // We should really clear all memory (important for netplay,
    // to keep states at both sides consistent).

    for (i = 0; i <= 6; i++)
    {   r[i]    = 0;
    }
    for (i = 0; i <= 7; i++)
    {   ras[i]  = 0;
    }
    psu         =
    psl         = 0;
    iar         = 0;

    switch (machine)
    {
    case ARCADIA:
        spriteflip =
        udcflip    = 0;
        /* This clearing helps avoid these problems:
           (a) Resetting from Dr. Slump game screen to title screen turns
               on constant rumbling (due to sprite imagery being retained
               in RAM).
           (b) For netplay, both machines should be in a consistent state. */
        fractionator = 0;
        for (i = 0x1800; i <= 0x1FFF; i++)
        {   memory[i] = 0;
        }
        for (i = 0x1909; i <= 0x190F; i++)
        {   memory[i] = 0xC0; // some of them (randomly) are $40 though
        }
        for (i = 0x19C0; i <= 0x19F7; i++)
        {   memory[i] = 0xFF;
        }
    acase INTERTON:
        for (i = 0x1800; i <= 0x1FFF; i++)
        {   memory[i] = 0;
        }
        memory[IE_NOISE] = 0x04; // Come Come never writes to NOISE, but expects tones to be audible anyway
    acase ELEKTOR:
        // $8A2/$8A3          = start address of game
        // $8B9/$8BA (INTADR) = address of game interrupt vector
        // $8BE/$8BF (PC)     = start address of game

        /* There are 3 feasible possibilities for starting the monitor:
        iar =     0; // gives black background with white "00 00" at top
        iar =  0x23; // gives blue  background with yellow "IIII" at bottom
        iar = 0x540; // gives blue  background
        */

        if (game)
        {   // make $8B9..$8BA = $0903 (important!)
            memory[0x8B9] = 0x09;
            memory[0x8BA] = 0x03;
            memory[0x8BD] = 0x1F; // BCTA,UN
            memory[0x8BE] = startaddr_h;
            memory[0x8BF] = startaddr_l;
            iar  = 0x8BD;

            /* The below lines can be used instead of the above lines, but it won't work with Jagen...
            memory[0x8A2] = startaddr_h; // SADR_H
            memory[0x8A3] = startaddr_l; // SADR_L
            iar = 0x540; */
        } else
        {   iar = 0x23; // gives blue  background with yellow "IIII" at bottom
        }

        for (i = 0x1D00; i <= 0x1D1F; i++)
        {   memory[i] = 0; // PSGs are cleared at reset
        }
        if (whichgame != -1 && known[whichgame].hiscore)
        {   psu |= PSU_II;
        }
        if (whichgame == OMEGALANDINGPOS)
        {   psl |= PSL_COM;
        }
        playsound(FALSE);
    }
    
    if (cheevos && g_client)
    {   rc_client_reset(g_client);
}   }

#ifndef LIBRETRO
JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_loadgame(JNIEnv* env, jobject this, jstring passedname, jint length, jbyteArray ptr)
{   int rc;
    const char* utfChars = (*env)->GetStringUTFChars(env, passedname, 0);
    strcpy(fn_game, utfChars);
    (*env)->ReleaseStringUTFChars(env, passedname, utfChars);
    // jsize length  = (*env)->GetArrayLength(env, ptr); // another way of getting the length
    filesize = (int) length;
    jbyte* IOBuffer2 = (*env)->GetByteArrayElements(env, ptr, 0);
    if ((IOBuffer = malloc(filesize)))
    {   memcpy(IOBuffer, IOBuffer2, filesize);
        (*env)->ReleaseByteArrayElements(env, ptr, IOBuffer2, JNI_ABORT);
        rc = parse_bytes(TRUE);
        free(IOBuffer);
        return (jint) rc;
    } else
    {   (*env)->ReleaseByteArrayElements(env, ptr, IOBuffer2, JNI_ABORT);
        return (jint) FALSE;
}   }
#endif /* !LIBRETRO */

EXPORT int parse_bytes(FLAG reconfigure)
{   TRANSIENT FLAG  same;
    TRANSIENT UBYTE kind;
    TRANSIENT UWORD loadaddress;
    TRANSIENT ULONG crc;
    TRANSIENT int   i,
                    length,
                    newmachine,
                    newmemmap,
                    tempsize;
    PERSIST   FLAG  found      = FALSE;
    
    serializemode = SERIALIZE_READ;

 /* TVC files are:
        0:  magic byte    (always $02)
     1..2:  load address  (where to load program to)
     3..4:  start address (where to point IAR to)
        5+: the program itself

    Magic bytes are:
    $02 = LODZ r2 = TVC
    $3A = BSTR,lt = AOF/HEX
    $4C = ANDA,r0 = EOF
    $BF = BSXA    = COS/COR
    all others    =       Arcadia/Interton/SI50/Dolphin/PHUNSY BIN

    kind can be any of KIND_AOF/BIN/COS/COR/EAP/EOF/HEX/TVC */

    same = FALSE;
    
    if (filesize == 0)
    {   return 1;
    }

    switch (IOBuffer[0])
    {
    case 2:
        kind = KIND_TVC;
        changemachine(ELEKTOR, MEMMAP_F, FALSE, FALSE, same);
        startaddr_h = ReadByteAt(3);
        startaddr_l = ReadByteAt(4);
        romsize = filesize - 5 + (startaddr_h * 256) + startaddr_l;
        offset = 0; // for autosense()
    acase 0xBF:
        if (IOBuffer[2] != 2) // 2 means DroidArcadia COS (0 means Ami/WinArcadia COS, 1 means COR)
        {   return 2;
        }
        newmemmap  = ReadByteAt(3);
        if (newmemmap >= MEMMAPS)
        {   return 3;
        }
        newmachine = memmapinfo[newmemmap].machine;
        cosversion = IOBuffer[1];
        if (cosversion != machines[newmachine].cosversion)
        {   return 4;
        }
        kind = KIND_COS;
        changemachine(newmachine, newmemmap, FALSE, FALSE, FALSE);
        romsize = 0;
        // startaddr_h|l aren't used for coin-ops
        // offset doesn't need to be set here
    adefault:
        if (filesize > 32 * KILOBYTE)
        {   return 5;
        }
        kind = KIND_BIN;
        romsize = filesize;
        startaddr_h = startaddr_l = 0;
        offset = 0; // for autosense()
    }

    unautosense();
    game = TRUE;
    switch (kind)
    {
    case KIND_BIN:
    case KIND_TVC:
        found = autosense(filesize, same);
    acase KIND_COS:
        whichgame = (int) ReadWordAt(4);
        if (whichgame >= 0 && whichgame < KNOWNGAMES)
        {   found = TRUE;
            configure(same, reconfigure);
            romsize = known[whichgame].size;
        } else
        {   found = FALSE;
            whichgame = -1;
    }   }

    if (!found && kind != KIND_COS && kind != KIND_COR)
    {   switch (machine)
        {
        case ARCADIA:
            if (romsize == 0 || romsize > 8192)
            {   changemachine(ARCADIA,    MEMMAP_I,       FALSE, FALSE, same);
            } else
            {   // assert(romsize <= 8192);
                changemachine(ARCADIA,    MEMMAP_ARCADIA, FALSE, FALSE, same);
            }
        acase INTERTON:
            if (romsize == 0 || romsize > 4096)
            {   changemachine(INTERTON,   MEMMAP_D,       FALSE, FALSE, same);
            } elif (romsize <= 2048)
            {   changemachine(INTERTON,   MEMMAP_A,       FALSE, FALSE, same);
            } else
            {   // assert(romsize <= 4096);
                changemachine(INTERTON,   MEMMAP_C,       FALSE, FALSE, same); // could be B or C, C is generally safer
            }
        acase ELEKTOR:
            if (kind == KIND_BIN)
            {   return 6;
            }
            changemachine(ELEKTOR,        MEMMAP_F,       FALSE, FALSE, same);
        acase ZACCARIA:
        case MALZAK:
            if (kind == KIND_BIN)
            {   return 6;
    }   }   }

    switch (kind)
    {
    case KIND_BIN:
        switch (machine)
        {
        case ARCADIA:
            arcadia_serializerom();
        acase INTERTON:
            interton_serializerom();
        }
        cheevosize = filesize;
    acase KIND_TVC:
        // assert(machine == ELEKTOR);
        loadaddress = ReadWordAt(1);
        offset = 5;
        for (i = 0; i < filesize - 5; i++)
        {   if (!(memflags[loadaddress + i] & NOWRITE))
            {   memory[loadaddress + i] = ReadByte();
            } else
            {   ReadByte();
        }   }
        cheevosize = filesize;
        if (cheevosize > 12 * KILOBYTE)
        {   cheevosize = 12 * KILOBYTE;
        }
        memcpy(cheevomem, IOBuffer, cheevosize);
    acase KIND_COS:
        serialize_cos();
    }

    game = TRUE; // must be done before engine_reset()!
    if (kind != KIND_COS)
    {   engine_reset();
    }
    patchrom();
    update_margins();

    if ((machine == ARCADIA || machine == INTERTON || memmap == MEMMAP_F) && (memory[IE_NOISE] & 0x10))
    {   bangercharging = TRUE;
        bangfrom       = cycles_2650; // we should really load and save these variables in COS and COR files
    } else
    {   bangercharging = FALSE;
    }

    if (machine == ELEKTOR && kind == KIND_TVC)
    {   // assert(machine == ELEKTOR);
        // memory[$1FC0..$1FC9] = memory[$177..$180]
        // ie.:
        //  SIZES        = $AA
        //  SPR01COLOURS = $09
        //  SPR23COLOURS = $09
        //  SCORECTRL    = $00
        //  $1FC4        = $00
        //  $1FC5        = $00
        //  BGCOLOUR     = $19
        //  PITCH        = $00
        //  SCORELT      = $AA
        //  SCORERT      = $AA
        if (elektor_biosver == ELEKTOR_HOBBYMODULE)
        {   for (i = 0; i < 10; i++)
            {   memory[0x1FC0 + i] = memory[0x57E + i];
        }   }
        else
        {   for (i = 0; i < 10; i++)
            {   memory[0x1FC0 + i] = memory[0x177 + i];
        }   }

        memory[0x1F00 + PVI_SPRITE0BX] =
        memory[0x1F00 + PVI_SPRITE0BY] =
        memory[0x1F00 + PVI_SPRITE1BX] =
        memory[0x1F00 + PVI_SPRITE1BY] =
        memory[0x1F00 + PVI_SPRITE2BX] =
        memory[0x1F00 + PVI_SPRITE2BY] =
        memory[0x1F00 + PVI_SPRITE3BX] =
        memory[0x1F00 + PVI_SPRITE3BY] = 0xFE; // helps with Figure27.tvc

        // make $8B9..$8BA = $0903 (important!)
        memory[0x8B9] = 0x09;
        memory[0x8BA] = 0x03;
        memory[0x8BD] = 0x1F; // BCTA,UN
        memory[0x8BE] = startaddr_h;
        memory[0x8BF] = startaddr_l;
        iar  = 0x8BD;
    }

    spriteflip     =
    udcflip        = 0;

    progressing    =
    tracking       = FALSE;
    progressurl[0] =
    trackerstr[ 0] = '-';
    progressurl[1] =
    trackerstr[ 1] = EOS;
    if (cheevos && g_client && !machines[machine].coinop)
    {   identifyragame();
    }

    return 0;
}

EXPORT UBYTE ReadByte(void)
{   UBYTE result;

    result = IOBuffer[offset++];
    return result;
}

EXPORT UBYTE ReadByteAt(int where)
{   UBYTE result;

    offset = where;
    result = IOBuffer[offset++];
    return result;
}

EXPORT UWORD ReadWordAt(int where)
{   UWORD result;

    offset = where;
    // big endian 16-bit word
    result = (UWORD) (256 * IOBuffer[offset])
                          + IOBuffer[offset + 1];
    offset += 2;
    return result;
}

EXPORT void WriteByte(UBYTE what)
{   IOBuffer[offset++] = what;
}

EXPORT void serialize_byte_int(int* var)
{   if (serializemode == SERIALIZE_WRITE)
    {   WriteByte((UBYTE) (*var));
    } else
    {   // assert(serializemode == SERIALIZE_READ);
        *var = (int) ReadByte();
}   }
EXPORT void serialize_word(UWORD* var)
{   if (serializemode == SERIALIZE_WRITE)
    {   WriteWord(*var);
    } else
    {   // assert(serializemode == SERIALIZE_READ);
        *var = ReadWord();
}   }
EXPORT void serialize_word_int(int* var)
{   if (serializemode == SERIALIZE_WRITE)
    {   WriteWord((UWORD) (*var));
    } else
    {   // assert(serializemode == SERIALIZE_READ);
        *var = (int) ReadWord();
        if (*var == 0xFFFF)
        {   *var = -1;
}   }   }
EXPORT void serialize_long(ULONG* var)
{   if (serializemode == SERIALIZE_WRITE)
    {   WriteLong(*var);
    } else
    {   // assert(serializemode == SERIALIZE_READ);
        *var = ReadLong();
}   }
EXPORT UWORD ReadWord(void)
{   UWORD result;

    // big endian 16-bit word
    result = (UWORD) (256 * IOBuffer[offset])
                          + IOBuffer[offset + 1];
    offset += 2;
    return result;
}
EXPORT ULONG ReadLong(void)
{   ULONG result;

    // big endian 32-bit longword
    result = (ULONG) (16777216 * IOBuffer[offset    ])
                   + (   65536 * IOBuffer[offset + 1])
                   + (     256 * IOBuffer[offset + 2])
                   +             IOBuffer[offset + 3];
    offset += 4;

    return result;
}
EXPORT void WriteWord(UWORD what)
{   // big endian 16-bit word
    IOBuffer[offset    ] = (UBYTE) (what / 256);
    IOBuffer[offset + 1] = (UBYTE) (what % 256);
    offset += 2;
}
EXPORT void WriteLong(ULONG what)
{   // big endian 32-bit word
    IOBuffer[offset    ] = (UBYTE)  (what / 16777216);
    IOBuffer[offset + 1] = (UBYTE) ((what % 16777217) / 65536);
    IOBuffer[offset + 2] = (UBYTE) ((what %    65536) /   256);
    IOBuffer[offset + 3] = (UBYTE)  (what %      256);
    offset += 4;
}

EXPORT void unautosense(void)
{   int i;

    downframes      = 4;
    totalframes     = 16;
    if (!machines[machine].coinop)
    {   if (machine == ARCADIA || machine == INTERTON)
        {   set_cpl(227);
        } elif (machine == ELEKTOR)
        {   set_cpl(226);
        }
        for (i = 0; i < 6; i++)
        {   p1sprcol[i] =
            p2sprcol[i] = 0;
        }
        for (i = 0; i < 4; i++)
        {   p1bgcol[i]  =
            p2bgcol[i]  = 0;
        }
        key1   = 2;
        key2   = 1;
        key3   = 3;
        key4   = 0;
    }
    sensitivity    = SENSITIVITY_DEFAULT;
    ppc            =  4;
    paddleup       =
    paddledown     =
    paddleleft     =
    paddleright    = -1;
    firstrow       = 26;
    lastrow        = -1;
    udcflips       =
    spriteflips    =  0;
    for (i = 0; i < 4; i++)
    {   protect[    i] =
        multisprite[i] = FALSE;
    }
    if (machine == INTERTON || machine == ELEKTOR)
    {   region = REGION_PAL; // because some games don't work in NTSC mode
    }
    // we deliberately leave flagline alone

    // Generally we default to the most powerful memory map of a machine.
    if (machine != MALZAK && machine != ZACCARIA)
    {   memmap = machines[machine].memmap;
    }

    whichoverlay = memmapinfo[memmap].overlay;
    generate_autotext();
}

EXPORT void serialize_byte(UBYTE* var)
{   if (serializemode == SERIALIZE_WRITE)
    {   WriteByte(*var);
    } else
    {   // assert(serializemode == SERIALIZE_READ);
        *var = ReadByte();
}   }

#ifndef LIBRETRO
JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_MainActivity_setinput(JNIEnv* env, jobject this, jint player, jint thekeypad, jint cx, jint cy)
{   int cplayer;

    cplayer         = (int) player;
    hinput[cplayer] = (ULONG) thekeypad;
    hx[cplayer]     = (UBYTE) ((jint) cx / 256);
    hy[cplayer]     = (UBYTE) ((jint) cy / 256);
}
#endif /* !LIBRETRO */

EXPORT void playsound(FLAG force)
{   FAST          FLAG   firstchan;
    FAST          ULONG  temp1, temp2;
    FAST          UBYTE  guestvolume[GUESTCHANNELS];
    FAST          int    i,
                         length;
    FAST          float  hertz;
    PERSIST const float  lb_div[13 + 1] =
{   0.0, // $0000
  478.0, // $0001
  451.0, // $0002
  426.0, // $0004
  402.0, // $0008
  379.0, // $0010
  358.0, // $0020
  338.0, // $0040
  319.0, // $0080
  301.0, // $0100
  284.0, // $0200
  268.0, // $0400
  253.0, // $0800
  239.0  // $1000
};
    /* guestvolume[]: indexed by guest channel.
                      contents are stored in guest format.
       guestpitch[]:  indexed by guest channel.
                      contents are stored in guest format.

       Although Arcadia/PIPBUG/CD2650 and Interton/Elektor volumes do not match,
       this is unimportant as playsound(TRUE) is called when changing
       machines. Thus, Arcadia/PIPBUG/CD2650 volumes are never compared against
       Interton/Elektor volumes. */

    if (!sound)
    {   return;
    }

    for (i = 0; i < GUESTCHANNELS; i++)
    {   guestpitch[i]  =
        guestvolume[i] = 0;
    }

    switch (machine)
    {
    case ARCADIA:
        if ((memory[A_VOLUME]) & 0x08)
        {   guestpitch[TONE_1STXVI]   = memory[A_PITCH]  & 0x7F;
            guestvolume[TONE_1STXVI]  = memory[A_VOLUME] & 0x07; // 0..7
        }
        if ((memory[A_VOLUME]) & 0x10)
        {   guestpitch[NOISE_1STXVI]  = memory[A_PITCH]  & 0x7F;
            guestvolume[NOISE_1STXVI] = memory[A_VOLUME] & 0x07; // 0..7
        }
    acase INTERTON:
    case ELEKTOR:
        if ((whichgame == KABOOM1POS || whichgame == KABOOM2POS) && memory[0x1F00 + PVI_PITCH] == 0)
        {
#ifdef KABOOMCLICKS
            play_sample(SAMPLE_CLICK);
#else
            guestpitch[TONE_1STXVI] = 1;
#endif
        } else
        {   guestpitch[TONE_1STXVI] = (UWORD) memory[0x1F00 + PVI_PITCH];
        }
        if
        (   memmap == MEMMAP_F
         && (memory[0x1D00 + PSG_MIXER1] & 0x40)
        )
        {   guestvolume[TONE_1STXVI] = memory[0x1D00 + PSG_PORTA1] & 0xF;
        } elif (machine == INTERTON || memmap == MEMMAP_F)
        {   if     ((memory[IE_NOISE] & 0x04) == 0x00)
            {   guestvolume[TONE_1STXVI] =  0; // 0%
            } elif ((memory[IE_NOISE] & 0xC0) == 0xC0)
            {   guestvolume[TONE_1STXVI] =  4; // 25%
            } elif ((memory[IE_NOISE] & 0xC0) == 0x80)
            {   guestvolume[TONE_1STXVI] =  6; // 37.5% (not 50%! See Elektor mag, Jul 1982, p. 78.)
            } elif ((memory[IE_NOISE] & 0xC0) == 0x40)
            {   guestvolume[TONE_1STXVI] = 12; // 75%
            } else
            {   guestvolume[TONE_1STXVI] = 15; // 100%
        }   }
        else
        {   guestvolume[TONE_1STXVI] = 15; // 4/4
        }

        if (memmap == MEMMAP_E)
        {   guestpitch[NOISE_1STXVI]  =
            guestvolume[NOISE_1STXVI] = 0;
            break;
        }

        if (bangercharging)
        {   if (!(memory[IE_NOISE] & 0x10))
            {   bangercharging = FALSE;
                temp1 = cycles_2650 - bangfrom;
             /* Typical values are:
                          66         for Kaboom
                       7,623.. 7,638 for Noise Explosion
                About 16,818..16,992 for Hunting
                About 35,499..35,520 for Grand Prix */
                if (temp1 >= 7500)
                {   play_sample(SAMPLE_EXPLOSION);
                } else
                {   sound_stop(GUESTCHANNELS + SAMPLE_EXPLOSION);
                    play_sample(SAMPLE_CLICK);
        }   }   }
        else
        {   if (memory[IE_NOISE] & 0x10)
            {   bangercharging = TRUE;
                bangfrom       = cycles_2650;
        }   }

        if ((memory[IE_NOISE] & 0x8) && memory[0x1F00 + PVI_PITCH])
        {   guestpitch[NOISE_1STXVI]      = (UWORD) memory[0x1F00 + PVI_PITCH];
            if     ((memory[IE_NOISE] & 0xC0) == 0xC0)
            {   guestvolume[NOISE_1STXVI] =  4; // 1/4
            } elif ((memory[IE_NOISE] & 0xC0) == 0x80)
            {   guestvolume[NOISE_1STXVI] =  8; // 2/4
            } elif ((memory[IE_NOISE] & 0xC0) == 0x40)
            {   guestvolume[NOISE_1STXVI] = 12; // 3/4
            } else
            {   guestvolume[NOISE_1STXVI] = 15; // 4/4
        }   }
        else
        {   guestpitch[NOISE_1STXVI]  =
            guestvolume[NOISE_1STXVI] = 0;
        }

        if (memmap == MEMMAP_F)
        {   // 1st PSG, output channel A
            if (!(memory[psgbase + PSG_AMPLITUDEA1] & 0x10))
            {   guestvolume[TONE_A1]  =
                guestvolume[NOISE_A1] = memory[psgbase + PSG_AMPLITUDEA1] & 0x0F;
            } else
            {   guestvolume[TONE_A1]  =
                guestvolume[NOISE_A1] = 15;
            }
            if (!(memory[psgbase + PSG_MIXER1] &    1)) // 1st tone A enable
            {   guestpitch[TONE_A1] = ((memory[psgbase + PSG_PITCHA1_H] & 0x0F) * 256) + memory[psgbase + PSG_PITCHA1_L];
                if (guestpitch[TONE_A1] == 0)
                {   guestpitch[TONE_A1] = 1;
            }   }
            else
            {   guestpitch[TONE_A1] = 0;
            }
            if (!(memory[psgbase + PSG_MIXER1] &    8)) // 1st noise A enable
            {   guestpitch[NOISE_A1] = memory[psgbase + PSG_PITCHD1] & 0x1F;
            } else
            {   guestpitch[NOISE_A1] = 0;
            }

            // 1st PSG, output channel B
            if (!(memory[psgbase + PSG_AMPLITUDEB1] & 0x10))
            {   guestvolume[TONE_B1] =
                guestvolume[NOISE_B1] = memory[psgbase + PSG_AMPLITUDEB1] & 0x0F;
            } else
            {   guestvolume[TONE_B1] =
                guestvolume[NOISE_B1] = 15;
            }
            if (!(memory[psgbase + PSG_MIXER1] &    2)) // 1st tone B enable
            {   guestpitch[TONE_B1] = ((memory[psgbase + PSG_PITCHB1_H] & 0x0F) * 256) + memory[psgbase + PSG_PITCHB1_L];
            } else
            {   guestpitch[TONE_B1] = 0;
            }
            if (!(memory[psgbase + PSG_MIXER1] & 0x10)) // 1st noise B enable
            {   guestpitch[NOISE_B1] = memory[psgbase + PSG_PITCHD1] & 0x1F;
            } else
            {   guestpitch[NOISE_B1] = 0;
            }

            // 1st PSG, output channel C
            if (!(memory[psgbase + PSG_AMPLITUDEC1] & 0x10))
            {   guestvolume[TONE_C1]  =
                guestvolume[NOISE_C1] = memory[psgbase + PSG_AMPLITUDEC1] & 0x0F;
            } else
            {   guestvolume[TONE_C1]  =
                guestvolume[NOISE_C1] = 15;
            }
            if (!(memory[psgbase + PSG_MIXER1] &    4)) // 1st tone C enable
            {   guestpitch[TONE_C1] = ((memory[psgbase + PSG_PITCHC1_H] & 0x0F) * 256) + memory[psgbase + PSG_PITCHC1_L];
            } else
            {   guestpitch[TONE_C1] = 0;
            }
            if (!(memory[psgbase + PSG_MIXER1] & 0x20)) // 1st noise C enable
            {   guestpitch[NOISE_C1] = memory[psgbase + PSG_PITCHD1] & 0x1F;
            } else
            {   guestpitch[NOISE_C1] = 0;
            }

            // 2nd PSG, output channel A
            if (!(memory[psgbase + PSG_AMPLITUDEA2] & 0x10))
            {   guestvolume[TONE_A2]  =
                guestvolume[NOISE_A2] = memory[psgbase + PSG_AMPLITUDEA2] & 0x0F;
            } else
            {   guestvolume[TONE_A2]  =
                guestvolume[NOISE_A2] = 15;
            }
            if (!(memory[psgbase + PSG_MIXER2] &    1)) // 2nd tone A enable
            {   guestpitch[TONE_A2] = ((memory[psgbase + PSG_PITCHA2_H] & 0x0F) * 256) + memory[psgbase + PSG_PITCHA2_L];
            } else
            {   guestpitch[TONE_A2] = 0;
            }
            if (!(memory[psgbase + PSG_MIXER2] &    8)) // 2nd noise A enable
            {   guestpitch[NOISE_A2] = memory[psgbase + PSG_PITCHD2] & 0x1F;
            } else
            {   guestpitch[NOISE_A2] = 0;
            }

            // 2nd PSG, output channel B
            if (!(memory[psgbase + PSG_AMPLITUDEB2] & 0x10))
            {   guestvolume[TONE_B2]  =
                guestvolume[NOISE_B2] = memory[psgbase + PSG_AMPLITUDEB2] & 0x0F;
            } else
            {   guestvolume[TONE_B2]  =
                guestvolume[NOISE_B2] = 15;
            }
            if (!(memory[psgbase + PSG_MIXER2] &    2)) // 2nd tone B enable
            {   guestpitch[TONE_B2] = ((memory[psgbase + PSG_PITCHB2_H] & 0x0F) * 256) + memory[psgbase + PSG_PITCHB2_L];
            } else
            {   guestpitch[TONE_B2] = 0;
            }
            if (!(memory[psgbase + PSG_MIXER2] & 0x10)) // 2nd noise B enable
            {   guestpitch[NOISE_B2] = memory[psgbase + PSG_PITCHD2] & 0x1F;
            } else
            {   guestpitch[NOISE_B2] = 0;
            }

            // 2nd PSG, output channel C
            if (!(memory[psgbase + PSG_AMPLITUDEC2] & 0x10))
            {   guestvolume[TONE_C2]  =
                guestvolume[NOISE_C2] = memory[psgbase + PSG_AMPLITUDEC2] & 0x0F;
            } else
            {   guestvolume[TONE_C2]  =
                guestvolume[NOISE_C2] = 15;
            }
            if (!(memory[psgbase + PSG_MIXER2] &    4)) // 2nd tone C enable
            {   guestpitch[TONE_C2] = ((memory[psgbase + PSG_PITCHC2_H] & 0x0F) * 256) + memory[psgbase + PSG_PITCHC2_L];
            } else
            {   guestpitch[TONE_C2] = 0;
            }
            if (!(memory[psgbase + PSG_MIXER2] & 0x20)) // 2nd noise C enable
            {   guestpitch[NOISE_C2] = memory[psgbase + PSG_PITCHD2] & 0x1F;
            } else
            {   guestpitch[NOISE_C2] = 0;
        }   }
    acase ZACCARIA:
        if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
        {   temp1 =  lb_snd & 0x00001FFF;
            guestpitch[TONE_1STLB1] =
            guestpitch[TONE_1STLB2] = 0;
            firstchan = TRUE;
            for (i = 1; i <= 13; i++)
            {   if (temp1 & (1 << (i - 1)))
                {   if (firstchan)
                    {   guestpitch[TONE_1STLB1] = i;
                        firstchan = FALSE;
                    } else
                    {   guestpitch[TONE_1STLB2] = i;
                        break;
            }   }   }

            temp2 = (lb_snd & 0x03FFE000) >> 12;
            guestpitch[TONE_2NDLB1] =
            guestpitch[TONE_2NDLB2] = 0;
            firstchan = TRUE;
            for (i = 1; i <= 13; i++)
            {   if (temp2 & (1 << (i - 1)))
                {   if (firstchan)
                    {   guestpitch[TONE_2NDLB1] = i;
                        firstchan = FALSE;
                    } else
                    {   guestpitch[TONE_2NDLB2] = i;
                        break;
            }   }   }

            guestvolume[TONE_1STLB1] =
            guestvolume[TONE_1STLB2] =
            guestvolume[TONE_2NDLB1] =
            guestvolume[TONE_2NDLB2] = 15;
        } elif (memmap == MEMMAP_GALAXIA)
        {   // squealer
            guestpitch[TONE_A1] = squeal;
            if (squeal)
            {   guestvolume[TONE_A1] =  6;
            } else
            {   guestvolume[TONE_A1] =  0;
            }

            // throbber
            if (ioport[PORTD] & 8)
            {   guestpitch[ TONE_B1] = throb;
                guestvolume[TONE_B1] =  4;
            } else
            {   guestpitch[ TONE_B1] =  0;
                guestvolume[TONE_B1] =  0;
    }   }   }
    
    for (i = FIRSTGUESTTONE; i <= LASTGUESTTONE; i++)
    {   if (guestpitch[i] && guestvolume[i])
        {   if
            (   force
             || !guestplaying[i]
             || guestpitch[i]  != oldguestpitch[i]
             || guestvolume[i] != oldguestvolume[i]
            )
            {   switch (i)
                {
                case TONE_1STXVI: // 0
                case TONE_2NDXVI: // 1
                case TONE_3RDXVI: // 2
                    if (retune)
                    {   hertz = (region == REGION_NTSC) ? idealfreq_ntsc[guestpitch[i]] : idealfreq_pal[guestpitch[i]];
                    } else
                    {   hertz = (float) (dividend / (guestpitch[i] + 1)); // desired frequency of the waveform
                    }
                acase 3: // TONE_1STLB1 and TONE_A1
                    if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
                    {   hertz = (float) ( 62500.0 / lb_div[guestpitch[i]]);
                    } elif (memmap == MEMMAP_F)
                    {   hertz = (float) (110837.0 / guestpitch[i]); // desired frequency of the waveform
                    } elif (memmap == MEMMAP_GALAXIA) // squealer
                    {   hertz = (float) (    16.0 * guestpitch[i]); // 16..4080 Hz
                    }
                acase 4: // TONE_1STLB2 and TONE_B1
                    if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
                    {   hertz = (float) ( 62500.0 / lb_div[guestpitch[i]]);
                    } elif (memmap == MEMMAP_F)
                    {   hertz = (float) (110837.0 / guestpitch[i]); // desired frequency of the waveform
                    } elif (memmap == MEMMAP_GALAXIA) // throbber
                    {   if (throb <= 7) // 1.. 7 -> 220..244
                        {   hertz = 216.0 + (throb * 4.0);
                        } else          // 8..12 -> 240..224
                        {   hertz = 272.0 - (throb * 4.0);
                    }   }
                acase 5: // TONE_2NDLB1 and TONE_C1
                case  6: // TONE_2NDLB2 and TONE_A2
                    if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
                    {   hertz = (float) (125000.0 / lb_div[guestpitch[i]]);
                    } elif (memmap == MEMMAP_F)
                    {   hertz = (float) (110837.0 / guestpitch[i]); // desired frequency of the waveform
                    }
                acase TONE_B2: // 7
                case  TONE_C2: // 8
                    hertz = (float) (110837.0 / guestpitch[i]); // desired frequency of the waveform
                }

                chan_status[i] = CHAN_PLAY;
                chan_hertz[i]  = hertz;
                chan_volume[i] = guestvolume[i];
        }   }
        else
        {   if (chan_status[i] == CHAN_PLAY)
            {   chan_status[i] = CHAN_PLAYTHENSTOP;
                // don't touch chan_hertz[i] nor chan_volume[i]
            } else
            {   chan_status[i] = CHAN_STOP;
    }   }   }

    for (i = FIRSTGUESTNOISE; i <= LASTGUESTNOISE; i++)
    {   if (guestpitch[i] && guestvolume[i])
        {   if
            (   force
             || !guestplaying[i]
             || guestvolume[i] != oldguestvolume[i]
             || guestpitch[i] != oldguestpitch[i]
            )
            {   if (i == NOISE_1STXVI)
                {   hertz = (float) (dividend / (guestpitch[i] + 1)); // desired frequency of the waveform
                } else
                {   hertz = (float) (110837.0 / guestpitch[i]);
                }

#ifdef DEBUGSOUND
                zprintf(TEXTPEN_VERBOSE, "Channel %d is playing noise at pitch %f, vol %d.\n", i, hertz, guestvolume[i]);
#endif
                chan_status[i] = CHAN_PLAY;
                chan_hertz[i]  = hertz;
                chan_volume[i] = guestvolume[i];
        }   }
        else
        {   if (chan_status[i] == CHAN_PLAY)
            {   chan_status[i] = CHAN_PLAYTHENSTOP;
                // don't touch chan_hertz[i] nor chan_volume[i]
            } else
            {   chan_status[i] = CHAN_STOP;
    }   }   }

    for (i = 0; i < GUESTCHANNELS; i++)
    {   oldguestvolume[i] = guestvolume[i];
        oldguestpitch[i]  = guestpitch[i];
}   }

#define NUMLOOPS 32 // was 8
EXPORT void generate_tone(int guestchan, double hertz)
{   FAST double cyclefloat,
                stopfloat;
    FAST int    i, j, k,
                stopint;

    hertz      = hertz * TRANSPOSE;
    cyclefloat = samplerate / (hertz * 2.0);
    j          =
    k          = 0;
    stopfloat  = cyclefloat;
    stopint    = (int) stopfloat;

    // assert(bitrate == 16);
    for (i = 0; i < NUMLOOPS; i++)
    {   do
        {   SoundBuffer[guestchan][j++] = upvol_16[guestchan];
            k++;
        } while (k < stopint);
        stopfloat += cyclefloat;
        stopint = (int) stopfloat;
        do
        {   SoundBuffer[guestchan][j++] = downvol_16[guestchan];
            k++;
        } while (k < stopint);
        stopfloat += cyclefloat;
        stopint = (int) stopfloat;
        SoundLength[guestchan] = k; // in samples
}   }

EXPORT void generate_noise(int guestchan, double hertz)
{   FAST    double cyclefloat,
                   stopfloat;
    FAST    int    i, j,
                   stopint;
    FAST    UBYTE  newbit;
    FAST    SWORD  outvol_16;
    PERSIST UWORD  noise = 0xF0; // this should maybe get reinitialized when the machine is reset? and loaded/saved from cos/cor files?

    cyclefloat  = samplerate / (hertz * 2.0);
    i           =
    j           = 0;
    stopfloat   = cyclefloat;
    stopint     = (int) stopfloat;

    // assert(bitrate == 16);
    do
    {   newbit    = ((noise & 1) ^ ((noise & 0x10) ? 1 : 0));
        outvol_16 = (noise & 1) ? upvol_16[guestchan] : downvol_16[guestchan];
        noise >>= 1;
        if (newbit)
        {   noise |= 0x100;
        }
        do
        {   SoundBuffer[guestchan][i++] = outvol_16;
            j++;
        } while (j < stopint);
        stopfloat += cyclefloat;
        stopint   =  (int) stopfloat;
    } while (j < samplerate);
    SoundLength[guestchan] = j;
}

EXPORT void set_volumes(int guestchan, int volume)
{   if (machines[machine].pvis)
    {     upvol_16[guestchan] =   volume_4to16[volume] * hostvol / 8;
        downvol_16[guestchan] = -(volume_4to16[volume] * hostvol / 8);
    } else
    {     upvol_16[guestchan] =   volume_3to16[volume] * hostvol / 8;
        downvol_16[guestchan] = -(volume_3to16[volume] * hostvol / 8);
}   }

#ifndef LIBRETRO
JNIEXPORT jshortArray JNICALL Java_com_amigan_droidarcadia_MainActivity_getsoundbuffer(JNIEnv* env, jobject this, jint whichchan)
{   jintArray rc;
    int       cwhichchan;
    
    cwhichchan = (int) whichchan;

    rc = (*env)->NewShortArray(env, SoundLength[cwhichchan]);
    (*env)->SetShortArrayRegion(env, rc, 0, SoundLength[cwhichchan], SoundBuffer[cwhichchan]);

    return rc;
}
#endif /* !LIBRETRO */

EXPORT void play_any(int guestchan, float hertz, int volume)
{   FAST ULONG change; // same as a DWORD
    FAST int   i;

    if (!sound)
    {   return;
    }

    if (guestchan < GUESTCHANNELS)
    {   // assert(volume >= 1 && volume <= 15);
        set_volumes(guestchan, volume);
      
        if (guestchan >= FIRSTGUESTTONE && guestchan <= LASTGUESTTONE)
        {   generate_tone( guestchan, (double) hertz);
        } else
        {   // assert(guestchan >= FIRSTGUESTNOISE && guestchan <= LASTGUESTNOISE);
            generate_noise(guestchan, (double) hertz);
    }   }
    else
    {   /* * 257         converts from 8-bit to 16-bit sample
           * hostvol / 8 applies the host volume to it */
        switch (guestchan)
        {
        case  GUESTCHANNELS + SAMPLE_EXPLOSION: for (i = 0; i < LEN_EXPLOSION; i++) SoundBuffer[GUESTCHANNELS + SAMPLE_EXPLOSION][i] = (SWORD) (fx_explosion[i] * 257 * hostvol / 8);
        acase GUESTCHANNELS + SAMPLE_SHOOT    : for (i = 0; i < LEN_SHOOT    ; i++) SoundBuffer[GUESTCHANNELS + SAMPLE_SHOOT    ][i] = (SWORD) (fx_shoot[    i] * 257 * hostvol / 8);
        acase GUESTCHANNELS + SAMPLE_BANG     : for (i = 0; i < LEN_BANG     ; i++) SoundBuffer[GUESTCHANNELS + SAMPLE_BANG     ][i] = (SWORD) (fx_bang[     i] * 257 * hostvol / 8);
        acase GUESTCHANNELS + SAMPLE_COIN     : for (i = 0; i < LEN_COIN     ; i++) SoundBuffer[GUESTCHANNELS + SAMPLE_COIN     ][i] = (SWORD) (fx_coin[     i] * 257 * hostvol / 8);
        acase GUESTCHANNELS + SAMPLE_CLICK    : for (i = 0; i < LEN_CLICK    ; i++) SoundBuffer[GUESTCHANNELS + SAMPLE_CLICK    ][i] = (SWORD) (fx_click[    i] * 257 * hostvol / 8);
        acase GUESTCHANNELS + SAMPLE_CHEEVOS  : for (i = 0; i < LEN_CHEEVOS  ; i++) SoundBuffer[GUESTCHANNELS + SAMPLE_CHEEVOS  ][i] = (SWORD) (fx_cheevos[  i] * 257 * hostvol / 8);
    }   }
    
    guestplaying[guestchan] = TRUE;
}

#ifndef LIBRETRO
JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_OptionsActivity_csetoptions(JNIEnv* env, jobject this, jint options1, jint options2)
{   int coptions,
        newmachine,
        newmemmap;

    coptions              = (int) options1;
    linebased             = (coptions & ( 1 <<  0)) ? TRUE       : FALSE;
    undither              = (coptions & ( 1 <<  1)) ? TRUE       : FALSE;
    autofire2             = (coptions & ( 3 <<  2)) >>  2;                // bits  3.. 2 (2 bits)
    useff                 = (coptions & ( 1 <<  4)) ? TRUE       : FALSE;
    region                = (coptions & ( 1 <<  5)) ? REGION_PAL : REGION_NTSC;
    sound                 = (coptions & ( 1 <<  6)) ? TRUE       : FALSE;
    retune                = (coptions & ( 1 <<  7)) ? TRUE       : FALSE;
    collisions            = (coptions & ( 1 <<  8)) ? TRUE       : FALSE;
    demultiplex           = (coptions & ( 1 <<  9)) ? TRUE       : FALSE;
    trainer_lives         = (coptions & ( 1 << 10)) ? TRUE       : FALSE;
    trainer_time          = (coptions & ( 1 << 11)) ? TRUE       : FALSE;
    trainer_invincibility = (coptions & ( 1 << 12)) ? TRUE       : FALSE;
    layout                = (coptions & ( 7 << 13)) >> 13;                // bits 15..13 (3 bits)
    hostvol               = (coptions & (15 << 16)) >> 16;                // bits 19..16 (4 bits)
    speed                 = (coptions & (15 << 20)) >> 20;                // bits 23..20 (4 bits)
    enhancestars          = (coptions & ( 1 << 24)) ? TRUE       : FALSE; // bit      24 (1 bit )
    spring                = (coptions & ( 1 << 25)) ? TRUE       : FALSE; // bit      25 (1 bit )
    swapped               = (coptions & ( 1 << 26)) ? TRUE       : FALSE; // bit      26 (1 bit )
    drawcorners           = (coptions & ( 1 << 27)) ? TRUE       : FALSE; // bit      27 (1 bit )
    dejitter              = (coptions & ( 1 << 28)) ? TRUE       : FALSE; // bit      28 (1 bit )
    colourset             = (coptions & ( 3 << 29)) >> 29;                // bits 30..29 (2 bits)
    lock                  = (coptions & ( 1 << 31)) ? TRUE       : FALSE; // bit      31 (1 bit )

    coptions              = (int) options2;
    newmachine            = (coptions & ( 7 <<  0)) >>  0;                // bits  2.. 0 (3 bits)
    newmemmap             = (coptions & (15 <<  3)) >>  3;                // bits  6.. 3 (4 bits)
    autocoin              = (coptions & ( 1 <<  7)) >>  7;                // bit   7     (1 bit )
    darkenbg              = (coptions & ( 1 <<  8)) >>  8;                // bit   8     (1 bit )
    flagline              = (coptions & ( 1 <<  9)) >>  9;                // bit   9     (1 bit )
    sameplayer            = (coptions & ( 1 << 10)) >> 10;                // bit  10     (1 bit )
    sensitivity           = (coptions & ( 7 << 11)) >> 11;                // bits 13..11 (3 bits)
    if (newmachine != machine || newmemmap != memmap)
    {   switch (newmachine)
        {
        case  ARCADIA:  command_changemachine(ARCADIA,  MEMMAP_ARCADIA);
        acase INTERTON: command_changemachine(INTERTON, MEMMAP_D);
        acase ELEKTOR:  command_changemachine(ELEKTOR,  MEMMAP_F);
        acase ZACCARIA: command_changemachine(ZACCARIA, newmemmap);
        acase MALZAK:   command_changemachine(MALZAK,   newmemmap);
    }   }
    
    analog                = (layout == WAYS_ANALOG) ? TRUE       : FALSE;
    make_stars();
    patchrom();
    update_margins();
}

JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_MainActivity_csetoptions(JNIEnv* env, jobject this, jint options1, jint options2)
{   int coptions,
        newmachine,
        newmemmap;

    coptions              = (int) options1;
    linebased             = (coptions & ( 1 <<  0)) ? TRUE       : FALSE;
    undither              = (coptions & ( 1 <<  1)) ? TRUE       : FALSE;
    autofire2             = (coptions & ( 3 <<  2)) >>  2;                // bits  3.. 2 (2 bits)
    useff                 = (coptions & ( 1 <<  4)) ? TRUE       : FALSE;
    region                = (coptions & ( 1 <<  5)) ? REGION_PAL : REGION_NTSC;
    sound                 = (coptions & ( 1 <<  6)) ? TRUE       : FALSE;
    retune                = (coptions & ( 1 <<  7)) ? TRUE       : FALSE;
    collisions            = (coptions & ( 1 <<  8)) ? TRUE       : FALSE;
    demultiplex           = (coptions & ( 1 <<  9)) ? TRUE       : FALSE;
    trainer_lives         = (coptions & ( 1 << 10)) ? TRUE       : FALSE;
    trainer_time          = (coptions & ( 1 << 11)) ? TRUE       : FALSE;
    trainer_invincibility = (coptions & ( 1 << 12)) ? TRUE       : FALSE;
    layout                = (coptions & ( 7 << 13)) >> 13;                // bits 15..13 (3 bits)
    hostvol               = (coptions & (15 << 16)) >> 16;                // bits 19..16 (4 bits)
    speed                 = (coptions & (15 << 20)) >> 20;                // bits 23..20 (4 bits)
    enhancestars          = (coptions & ( 1 << 24)) ? TRUE       : FALSE; // bit      24 (1 bit )
    spring                = (coptions & ( 1 << 25)) ? TRUE       : FALSE; // bit      25 (1 bit )
    swapped               = (coptions & ( 1 << 26)) ? TRUE       : FALSE; // bit      26 (1 bit )
    drawcorners           = (coptions & ( 1 << 27)) ? TRUE       : FALSE; // bit      27 (1 bit )
    dejitter              = (coptions & ( 1 << 28)) ? TRUE       : FALSE; // bit      28 (1 bit )
    colourset             = (coptions & ( 3 << 29)) >> 29;                // bits 30..29 (2 bits)
    lock                  = (coptions & ( 1 << 31)) ? TRUE       : FALSE; // bit      31 (1 bit )
    
    coptions              = (int) options2;
    newmachine            = (coptions & ( 7 <<  0)) >>  0;                // bits  2.. 0 (3 bits)
    newmemmap             = (coptions & (15 <<  3)) >>  3;                // bits  6.. 3 (4 bits)
    autocoin              = (coptions & ( 1 <<  7)) >>  7;                // bit   7     (1 bit )
    darkenbg              = (coptions & ( 1 <<  8)) >>  8;                // bit   8     (1 bit )
    flagline              = (coptions & ( 1 <<  9)) >>  9;                // bit   9     (1 bit )
    sameplayer            = (coptions & ( 1 << 10)) >> 10;                // bit  10     (1 bit )
    sensitivity           = (coptions & ( 7 << 11)) >> 11;                // bits 13..11 (3 bits)
    if (newmachine != machine || newmemmap != memmap)
    {   switch (newmachine)
        {
        case  ARCADIA:  command_changemachine(ARCADIA,  MEMMAP_ARCADIA);
        acase INTERTON: command_changemachine(INTERTON, MEMMAP_D);
        acase ELEKTOR:  command_changemachine(ELEKTOR,  MEMMAP_F);
        acase ZACCARIA: command_changemachine(ZACCARIA, newmemmap);
        acase MALZAK:   command_changemachine(MALZAK,   newmemmap);
    }   }

    analog                = (layout == WAYS_ANALOG) ? TRUE       : FALSE;
    make_stars();
    patchrom();
    update_margins();
}

JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_cgetoptions1(JNIEnv* env, jobject this)
{   int coptions = 0;

    if (linebased            ) coptions |= (1 <<  0);
    if (undither             ) coptions |= (1 <<  1);
                               coptions |= (autofire2 <<  2); // bits  3.. 2 (2 bits)
    if (useff                ) coptions |= (1 <<  4);
    if (region               ) coptions |= (1 <<  5);
    if (sound                ) coptions |= (1 <<  6);
    if (retune               ) coptions |= (1 <<  7);
    if (collisions           ) coptions |= (1 <<  8);
    if (demultiplex          ) coptions |= (1 <<  9);
    if (trainer_lives        ) coptions |= (1 << 10);
    if (trainer_time         ) coptions |= (1 << 11);
    if (trainer_invincibility) coptions |= (1 << 12);
                               coptions |= (layout    << 13); // bits 15..13 (3 bits)
                               coptions |= (hostvol   << 16); // bits 19..16 (4 bits)
                               coptions |= (speed     << 20); // bits 23..20 (4 bits)
    if (enhancestars         ) coptions |= (1         << 24); // bit      24 (1 bit )
    if (spring               ) coptions |= (1         << 25); // bit      25 (1 bit )
    if (swapped              ) coptions |= (1         << 26); // bit      26 (1 bit )
    if (drawcorners          ) coptions |= (1         << 27); // bit      27 (1 bit )
    if (dejitter             ) coptions |= (1         << 28); // bit      28 (1 bit )
                               coptions |= (colourset << 29); // bits 30..29 (2 bits)
    if (lock                 ) coptions |= (1         << 31); // bit      31 (1 bit )

    return (jint) coptions;
}
JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_cgetoptions2(JNIEnv* env, jobject this)
{   int coptions = 0;

    coptions |= (machine     <<  0); // bits  2.. 0 (3 bits)
    coptions |= (memmap      <<  3); // bits  6.. 3 (4 bits)
    if (autocoin             ) coptions |= (1         <<  7); // bit       7 (1 bit )
    if (darkenbg             ) coptions |= (1         <<  8); // bit       8 (1 bit )
    if (flagline             ) coptions |= (1         <<  9); // bit       9 (1 bit )
    if (sameplayer           ) coptions |= (1         << 10); // bit      10 (1 bit )
    coptions |= (sensitivity << 11); // bits 13..11 (3 bits)
    
    return (jint) coptions;
}
JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_OptionsActivity_cgetoptions1(JNIEnv* env, jobject this)
{   int coptions = 0;

    if (linebased            ) coptions |= (1 <<  0);
    if (undither             ) coptions |= (1 <<  1);
                               coptions |= (autofire2 <<  2); // bits  3.. 2 (2 bits)
    if (useff                ) coptions |= (1 <<  4);
    if (region               ) coptions |= (1 <<  5);
    if (sound                ) coptions |= (1 <<  6);
    if (retune               ) coptions |= (1 <<  7);
    if (collisions           ) coptions |= (1 <<  8);
    if (demultiplex          ) coptions |= (1 <<  9);
    if (trainer_lives        ) coptions |= (1 << 10);
    if (trainer_time         ) coptions |= (1 << 11);
    if (trainer_invincibility) coptions |= (1 << 12);
                               coptions |= (layout    << 13); // bits 15..13 (3 bits)
                               coptions |= (hostvol   << 16); // bits 19..16 (4 bits)
                               coptions |= (speed     << 20); // bits 23..20 (4 bits)
    if (enhancestars         ) coptions |= (1         << 24); // bit      24 (1 bit )
    if (spring               ) coptions |= (1         << 25); // bit      25 (1 bit )
    if (swapped              ) coptions |= (1         << 26); // bit      26 (1 bit )
    if (drawcorners          ) coptions |= (1         << 27); // bit      27 (1 bit )
    if (dejitter             ) coptions |= (1         << 28); // bit      28 (1 bit )
                               coptions |= (colourset << 29); // bits 30..29 (2 bits)
    if (lock                 ) coptions |= (1         << 31); // bit      31 (1 bit )

    return (jint) coptions;
}
JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_OptionsActivity_cgetoptions2(JNIEnv* env, jobject this)
{   int coptions = 0;

    coptions |= (machine     <<  0); // bits  2.. 0 (3 bits)
    coptions |= (memmap      <<  3); // bits  6.. 3 (4 bits)
    if (autocoin             ) coptions |= (1         <<  7); // bit       7 (1 bit )
    if (darkenbg             ) coptions |= (1         <<  8); // bit       8 (1 bit )
    if (flagline             ) coptions |= (1         <<  9); // bit       9 (1 bit )
    if (sameplayer           ) coptions |= (1         << 10); // bit      10 (1 bit )
    coptions |= (sensitivity << 11); // bits 13..11 (3 bits)

    return (jint) coptions;
}
#endif /* !LIBRETRO */

EXPORT void changemachine(int whichmachine, int whichmemmap, FLAG user, FLAG force, FLAG same)
{   int x, y;

    if (whichgame == 65535)
    {   whichgame = -1; // important!
    }

    // malzak_savenvram();
    if (memmap != whichmemmap || force)
    {   machine = whichmachine;
        memmap  = whichmemmap;
        if (machine != ARCADIA)
        {   region = (ULONG) machines[machine].region;
        }

        update_margins(); // sets machines[machine].width and machines[machine].height
    }

    if (user || force)
    {   whichgame  =  -1;
        romsize    =   0;
        switch (machine)
        {
        case ARCADIA:
            whichgame  = ARCADIASTUBPOS;
            romsize    = known[whichgame].size;
        acase INTERTON:
            whichgame  = INTERTONSTUBPOS;
            romsize    = known[whichgame].size;
        }
        unautosense();
    }

    switch (machine)
    {
    case  ARCADIA:                arcadia_setmemmap();
    acase INTERTON:               interton_setmemmap();
    acase ELEKTOR:                elektor_setmemmap();
    acase ZACCARIA:
        switch (memmap)
        {
        case  MEMMAP_ASTROWARS:   astrowars_setmemmap();
        acase MEMMAP_GALAXIA:     galaxia_setmemmap();
        acase MEMMAP_LASERBATTLE:
        case  MEMMAP_LAZARIAN:    lb_setmemmap();
        }
    acase MALZAK:                 malzak_setmemmap();
    }

    if (user || force)
    {   engine_reset();
    }
    generate_autotext();

    for (y = 0; y < MAXBOXHEIGHT; y++)
    {   for (x = 0; x < MAXBOXWIDTH; x++)
        {   screen[x][y] = GUESTCOLOURS;
}   }   }

EXPORT void configure(FLAG same, FLAG all)
{   int   i;
 // ULONG oldanalog;

    // assert(whichgame != -1);

    switch (known[whichgame].machine)
    {
    case INTERTON:
        if
        (   whichgame == I_TETRISPOS1
         || whichgame == I_TETRISPOS2
         || whichgame == I_TETRISPOS3
        )
        {   machines[machine].digipos[0] = 0x0C;
            machines[machine].digipos[1] = 0x34;
            machines[machine].digipos[2] = 0x58;
        } else
        {   machines[machine].digipos[0] =    1;
            machines[machine].digipos[1] =  112;
            machines[machine].digipos[2] =  225;
        }
    acase ELEKTOR:
        if
        (   whichgame == ENTERPRISE1POS1
         || whichgame == ENTERPRISE1POS2
         || whichgame == HAMISH1POS1
         || whichgame == HAMISH1POS2
         || whichgame == HAMISH1POS3
         || whichgame == LAUNCHINGPOS1
         || whichgame == LAUNCHINGPOS2
        )
        {   machines[machine].digipos[0] =   0;
            machines[machine].digipos[1] =   1;
         // machines[machine].digipos[2] = 225;
        } else
        {   machines[machine].digipos[0] =   1;
            machines[machine].digipos[1] = 111;
         // machines[machine].digipos[2] = 225;
    }   }

    // assert(machine != MALZAK && machine != ZACCARIA);
    changemachine(known[whichgame].machine, known[whichgame].memmap, FALSE, FALSE, same);

    switch (machine)
    {
    case ARCADIA:
        if (known[whichgame].bios != REGION_ANY)
        {   region    = (ULONG) known[whichgame].bios;
        }
        if (all)
        {   flagline  =         known[whichgame].flagline;
        }
    acase INTERTON:
        if (all)
        {   darkenbg  =         known[whichgame].flagline;
        }
    acase ELEKTOR:
        if (all)
        {   darkenbg  =         known[whichgame].flagline;
        }
        elektor_biosver =       known[whichgame].bios;
    }
    // oldanalog      = analog;
    analog            = (ULONG) known[whichgame].analog;
    swapped           = (ULONG) known[whichgame].swapped;
    downframes        = (ULONG) known[whichgame].downframes;
    totalframes       = (ULONG) known[whichgame].totalframes;
    // upframes       = totalframes - downframes;
    key1              = (int)   known[whichgame].key1;
    key2              = (int)   known[whichgame].key2;
    key3              = (int)   known[whichgame].key3;
    key4              = (int)   known[whichgame].key4;
    sensitivity       = (int)   known[whichgame].sensitivity;
    for (i = 0; i < 4; i++)
    {   p1bgcol[i]    = (int)   known[whichgame].p1bgcol[i];
        p2bgcol[i]    = (int)   known[whichgame].p2bgcol[i];
    }
    for (i = 0; i < 6; i++)
    {   p1sprcol[i]   = (int)   known[whichgame].p1sprcol[i];
        p2sprcol[i]   = (int)   known[whichgame].p2sprcol[i];
    }
    paddleup          = (int)   known[whichgame].up;
    paddledown        = (int)   known[whichgame].down;
    paddleleft        = (int)   known[whichgame].left;
    paddleright       = (int)   known[whichgame].right;
    whichoverlay      = (int)   known[whichgame].whichoverlay;
    if (known[whichgame].cpl)
    {   set_cpl(        (int)   known[whichgame].cpl);
    } elif (machine == ELEKTOR)
    {   set_cpl(                227);
    }

    if (analog)
    {   layout = WAYS_ANALOG;
    } elif ((int) known[whichgame].firstequiv != -1)
    {   layout = (int) known[whichgame].firstequiv;
    } else // if (!analog && oldanalog)
    {   layout = W8;
    }
    
    if (whichgame == SUBMARINESRACINGPOS)
    {   ppc = 2;
    }

    if (known[whichgame].demultiplex || known[whichgame].spriteflips || known[whichgame].udcflips)
    {   firstrow    =  known[whichgame].firstrow;
        lastrow     =  known[whichgame].lastrow;
        udcflips    =  known[whichgame].udcflips;
        spriteflips =  known[whichgame].spriteflips;
        if (machine == INTERTON || machine == ELEKTOR)
        {   multisprite[0] = (known[whichgame].demultiplex & 0x80) ? TRUE : FALSE;
            multisprite[1] = (known[whichgame].demultiplex & 0x40) ? TRUE : FALSE;
            multisprite[2] = (known[whichgame].demultiplex & 0x20) ? TRUE : FALSE;
            multisprite[3] = (known[whichgame].demultiplex & 0x10) ? TRUE : FALSE;
            protect[    0] = (known[whichgame].demultiplex & 0x08) ? FALSE : TRUE;
            protect[    1] = (known[whichgame].demultiplex & 0x04) ? FALSE : TRUE;
            protect[    2] = (known[whichgame].demultiplex & 0x02) ? FALSE : TRUE;
            protect[    3] = (known[whichgame].demultiplex & 0x01) ? FALSE : TRUE;
        } else
        {   // assert(machine == ARCADIA);
            protect[0]     = (known[whichgame].demultiplex & 0x08) ? TRUE : FALSE;
            protect[1]     = (known[whichgame].demultiplex & 0x04) ? TRUE : FALSE;
            protect[2]     = (known[whichgame].demultiplex & 0x02) ? TRUE : FALSE;
            protect[3]     = (known[whichgame].demultiplex & 0x01) ? TRUE : FALSE;
    }   }

    make_stars();
    generate_autotext();
}

// this routine reads but doesn't write offset
MODULE FLAG autosense(int thesize, FLAG same)
{   int   i;
    ULONG thecrc32; // crc32() is a function name

    if (thesize < 1 || thesize > 32768)
    {   whichgame = -1;
        return FALSE;
    }

    thecrc32 = da_crc32(IOBuffer + offset, thesize);
    // crc64(IOBuffer + offset, thesize);

    for (i = 0; i < KNOWNGAMES; i++)
    {   if
        (   known[i].size    == thesize
         && known[i].crc32   == thecrc32
/*       && known[i].crc64_h == crc64_h
         && known[i].crc64_l == crc64_l
*/      )
        {   whichgame = i;
            configure(same, TRUE);
            return TRUE;
    }   }

    whichgame = -1;
    return FALSE;
}

EXPORT void da_generate_crc32table(void)
{   ULONG crc;
    int   i, j;

    for (i = 0; i < 256; i++)
    {   crc = (ULONG) i;
        for (j = 8; j > 0; j--)
        {   if (crc & 1)
            {   crc = (crc >> 1) ^ 0xEDB88320L;
            } else
            {   crc >>= 1;
        }   }
        da_crc32_table[i] = crc;
}   }

EXPORT ULONG da_crc32(const UBYTE* address, int thesize)
{   ULONG crc = 0xFFFFFFFF;
    int   i;

    for (i = 0; i < thesize; i++)
    {   crc = ((crc >> 8) & 0x00FFFFFF) ^ da_crc32_table[(         crc        ^ *(address + i)) & 0xFF];
    }
    return crc ^ 0xFFFFFFFF;
}

/* EXPORT void crc64(UBYTE* address, int thesize)
{   ULONG index;
    int   i;

    crc64_h = 0xFFFFFFFF;
    crc64_l = 0xFFFFFFFF;
    for (i = 0; i < thesize; i++)
    {   index   = ((crc64_h >> 24) ^ *(address + i)) & 0xFF;
        crc64_h = ((crc64_h << 8) | (crc64_l >> 24)) ^ crc64_table_h[index];
        crc64_l =  (crc64_l << 8)                    ^ crc64_table_l[index];
    }
    crc64_h ^= 0xFFFFFFFF;
    crc64_l ^= 0xFFFFFFFF;
} */

EXPORT void generate_autotext(void) { ; }

EXPORT void patchrom(void)
{   int i;

    switch (memmap)
    {
    case MEMMAP_ASTROWARS:
        // ADDI,r0 $01 (patient) vs. LODI,r0 $55 (impatient)
        memory[ 0x198] = post                  ? 0x84 : 0x04;
        memory[ 0x199] = post                  ? 0x01 : 0x55;

        memory[ 0xEB2] = trainer_lives         ? 0x00 : 0x01;     // SUBI,r0 1             -> SUBI,r0 0

        memory[0x25FF] = trainer_time          ? 0x00 : 0x02; // fuel

        memory[ 0x93B] = trainer_invincibility ? 0x00 : 0x22; // ships
        memory[0x11A4] = trainer_invincibility ? 0x00 : 0x22; // bombs
        memory[0x2548] = trainer_invincibility ? 0x00 : 0x22; // bullets
    acase MEMMAP_GALAXIA:
        // ADDI,r0 $01 (patient) vs. LODI,r0 $55 (impatient)
        memory[ 0x179] = post                  ? 0x84 : 0x04;
        memory[ 0x17A] = post                  ? 0x01 : 0x55;

        memory[ 0x6F1] = trainer_lives         ? 0x00 : 0x01;     // SUBI,r0 1             -> SUBI,r0 0

        memory[0x284F] = trainer_invincibility ? 0x00 : 0x26; // bullets
        memory[0x2934] = trainer_invincibility ? 0x1B : 0x98; // ships

        memory[0x27BB] = trainer_time          ? 0xC0 : 0x16;     // RETC,lt               -> NOP       (energy)
    acase MEMMAP_LASERBATTLE:
        // ADDI,r0 $01 (patient) vs. LODI,r0 $55 (impatient)
        memory[ 0x269] = post                  ? 0x84 : 0x04;
        memory[ 0x26A] = post                  ? 0x01 : 0x55;

        memory[0x2715] = trainer_time          ? 0x00 : 0x02;     // SUBI,r0 2             -> SUBI,r0 0 (fuel)

        memory[0x520D] = trainer_lives         ? 0x00 : 0x01;     // SUBI,r0 1             -> SUBI,r0 0

        memory[0x50DC] = trainer_invincibility ? 0x20 : 0x0C;     // LODA,r0 $5C4B         -> EORZ    r0
        memory[0x50DD] = trainer_invincibility ? 0x17 : 0x1C;     //                          RETC,un
    acase MEMMAP_LAZARIAN:
        // ADDI,r0 $01 (patient) vs. LODI,r0 $55 (impatient)
        memory[ 0x281] = post                  ? 0x84 : 0x04;
        memory[ 0x282] = post                  ? 0x01 : 0x55;

        memory[0x30A2] = trainer_invincibility ? 0x20 : 0x0D;     // LODA,r1 $3C01         -> EORZ    r0
        memory[0x30A3] = trainer_invincibility ? 0x17 : 0x1C;     //                          RETC,un

        memory[0x31A8] = trainer_lives         ? 0x00 : 0x01;     // SUBI,r0 1             -> SUBI,r0 0

        memory[0x46E2] = trainer_time          ? 0x00 : 0x02;     // SUBI,r0 2             -> SUBI,r0 0 (fuel)
    acase MEMMAP_MALZAK1:
        // STRA,r0 $1800 (patient) vs. BCTA,un $00A0 (impatient)
        memory[  0x52] = post                  ? 0xCC : 0x1F;
        memory[  0x53] = post                  ? 0x18 : 0x00;
        memory[  0x54] = post                  ? 0x00 : 0xA0;

        memory[0x24AD] = trainer_lives         ? 0x00 : 0x01;     // SUBI,r0 1             -> SUBI,r0 0

        memory[0x27A5] = trainer_time          ? 0x00 : 0x02;     // SUBI,r0 2             -> SUBI,r0 0 (fuel)

        memory[0x41FD] = trainer_invincibility ? 0x42 : 0x24;
        memory[0x41FE] = trainer_invincibility ? 0x09 : 0x00;
    acase MEMMAP_MALZAK2:
        // LODI,r0 $00 (patient) vs. LODI,r0 $01 (impatient)
        memory[  0x84] = post                  ? 0x00 : 0x01;

        memory[0x24AD] = trainer_lives         ? 0x00 : 0x01;     // SUBI,r0 1             -> SUBI,r0 0

        memory[0x279E] = trainer_time          ? 0xA4 : 0xAC;     // SUBA,r0 $30CF         -> SUBI,r0 0 (fuel)
        memory[0x279F] = trainer_time          ? 0x00 : 0x10;
        memory[0x27A0] = trainer_time          ? 0xA4 : 0xCF;     //                          SUBI,r0 0
        memory[0x27A1] = trainer_time          ? 0x00 : 0xAC;     // SUBA,r0 $30CF
        memory[0x27A2] = trainer_time          ? 0xA4 : 0x10;     //                          SUBI,r0 0
        memory[0x27A3] = trainer_time          ? 0x00 : 0xCF;

        memory[0x41FF] = trainer_invincibility ? 0x42 : 0x24;
        memory[0x4200] = trainer_invincibility ? 0x09 : 0x00;
    adefault:
        switch (whichgame)
        {
        case _2DSOCCERPOS: // time
        case _2DSOCCERODPOS:
            memory[ 0x62D] = trainer_time          ? 0xC0 : 0x84; // ADDI,r0 $FF           -> NOP
            memory[ 0x62E] = trainer_time          ? 0xC0 : 0xFF; //                          NOP
            memory[ 0x62F] = trainer_time          ? 0xC0 : 0x94; // DAR,r0                -> NOP
        acase _3DATTACKPOS:
            memory[ 0x97C] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $9AE          -> NOP
            memory[ 0x97D] = trainer_invincibility ? 0xC0 : 0x30; //                          NOP
            memory[ 0x985] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $9AE          -> NOP
            memory[ 0x986] = trainer_invincibility ? 0xC0 : 0x27; //                          NOP
            memory[ 0x98C] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $9AE          -> NOP
            memory[ 0x98D] = trainer_invincibility ? 0xC0 : 0x20; //                          NOP
            memory[ 0x995] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $9AE          -> NOP
            memory[ 0x996] = trainer_invincibility ? 0xC0 : 0x17; //                          NOP
            memory[ 0x99C] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $9AE          -> NOP
            memory[ 0x99D] = trainer_invincibility ? 0xC0 : 0x10; //                          NOP
            memory[ 0x9AA] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $9AE          -> NOP
            memory[ 0x9AB] = trainer_invincibility ? 0xC0 : 0x02; //                          NOP
            memory[ 0x9C0] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $9AE          -> NOP
            memory[ 0x9C1] = trainer_invincibility ? 0xC0 : 0x6C; //                          NOP
         // memory[ 0xA1F] = trainer_invincibility ? 0xC0 : 0x1D; // BCTA,gt $9AE          -> NOP
         // memory[ 0xA20] = trainer_invincibility ? 0xC0 : 0x09; //                          NOP
         // memory[ 0xA21] = trainer_invincibility ? 0xC0 : 0xAE; //                          NOP

            memory[ 0x15D] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0

            memory[ 0xCC2] = trainer_time          ? 0x00 : 0xFF; // ADDI,r0 $FF           -> ADDI,r0 0
        acase _3DSOCCERAPOS: // time
        case _3DSOCCERBPOS:
        case _3DSOCCERENHPOS:
            memory[ 0x181] = trainer_time          ? 0x00 : 0xFF; // ADDI,r0 $FF           -> ADDI,r0 0
        acase AIRSEAATTACKPOS:
            memory[ 0x60D] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0 (player 1)
            memory[ 0x65A] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0 (player 2)
        acase AIRSEABATTLEPOS:
            memory[ 0x60F] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0 (player 1)
            memory[ 0x65C] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0 (player 2)
        acase ALIENINVPOS:
        case ALIENINV1POS:
        case ALIENINV2POS:
        case ALIENINV3POS:
        case ALIENINV4POS:
            memory[ 0x82A] = trainer_lives         ? 0x00 : 0xFF; // ADDI,r1 $FF           -> ADDI,r1 0

            memory[ 0x7DC] = trainer_invincibility ? 0xC0 : 0x98; // BCFR,eq $7E8          -> NOP
            memory[ 0x7DD] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x7E5] = trainer_invincibility ? 0x1F : 0x1C; // BCTA,eq $1D6          -> BCTA,un $1D6

            memory[  0xE3] = trainer_time          ? 0xC0 : 0xCC; // STRA,r0 PITCH         -> NOP
            memory[  0xE4] = trainer_time          ? 0xC0 : 0x18; //                          NOP
            memory[  0xE5] = trainer_time          ? 0xC0 : 0xFD; //                          NOP
            memory[ 0xBFB] = trainer_time          ? 0x17 : 0x0C; // LODA,r0 $18F8         -> RETC,un
        acase ASTEROIDSPOS:
            memory[ 0x976] = trainer_invincibility ? 0x3F : 0x0F; // LODA,r3 SPRITECOLLIDE -> BSTA,un $1446
            memory[ 0x977] = trainer_invincibility ? 0x14 : 0x1F;
            memory[ 0x978] = trainer_invincibility ? 0x46 : 0xCB;
            memory[0x1446] = trainer_invincibility ? 0x0F : 0x00; //                          LODA,r3 SPRITECOLLIDE
            memory[0x1447] = trainer_invincibility ? 0x1F : 0x00;
            memory[0x1448] = trainer_invincibility ? 0xCB : 0x00;
            memory[0x1449] = trainer_invincibility ? 0x47 : 0x00; //                          ANDI,r3 $07
            memory[0x144A] = trainer_invincibility ? 0x07 : 0x00;
            memory[0x144B] = trainer_invincibility ? 0x17 : 0x00; //                          RETC,un
        acase ASTROINVPOS:
        case ASTROINVODPOS:
            memory[ 0x174] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
        acase ATTACKFROMSPACEPOS:
            memory[ 0xBB2] = trainer_invincibility ? 0x1B : 0x98; // BCFR,eq $BA9          -> BCTR,un $BA9
        acase A_BOXINGPOS:
            memory[ 0x6B7] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase A_GOLFPOS1:
        case A_GOLFPOS2:
        case A_GOLFODPOS:
            memory[ 0xDC1] = trainer_time          ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
        acase A_BASKETBALLPOS:
            memory[ 0x112] = trainer_time          ? 0x00 : 0x01; // SUBI,r1 1             -> SUBI,r1 0
        acase BREAKAWAYPOS:
            memory[ 0x356] = trainer_lives         ? 0x00 : 0xFF; // ADDI,r0 $FF           -> ADDI,r0 0
        acase BREAKOUTPOS:
            memory[ 0xF2C] = trainer_lives         ? 0xC0 : 0xF9; // BDRR,r1 $F2E          -> NOP
            memory[ 0xF2D] = trainer_lives         ? 0xC0 : 0x00; //                          NOP
        acase BURSTINGBALLOONSPOS:
            memory[ 0xC2D] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase E_CARRACESPOS:
            memory[ 0x9BC] = trainer_time          ? 0x66 : 0x67; // ADDI,r2 $67           -> ADDI,r2 $66

            memory[ 0x947] = trainer_invincibility ? 0x00 : 0x15; // BCTR,lt $95D          -> BCTR,lt $948
            memory[ 0x94F] = trainer_invincibility ? 0x00 : 0x0D; // BCTR,gt $95D          -> BCTR,gt $950
            memory[ 0xCD4] = trainer_invincibility ? 0x1F : 0x9C; // BCFA,eq $91B          -> BCTA,un $91B
        acase CATTRAXPOS:
            memory[ 0x21C] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
        acase CHICKEN1POS:
        case  CHICKEN2POS:
        case  CHICKEN3POS:
            memory[ 0x90A] = trainer_invincibility ? 0x00 : 0x15; // ANDI,r3 $15           -> ANDI,r3 0
        acase A_CIRCUSPOS:
            memory[ 0xDEA] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase COSMICADVENTUREPOS1:
        case COSMICADVENTUREPOS2:
            memory[ 0xDC1] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0 (for 1st level)
            memory[0x1218] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0 (for 2nd level)

            memory[0x12AC] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0 (for 2nd level)
            memory[0x19D8] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0 (for 3rd level)
        acase CIRCLEDRIVEPOS1:
        case CIRCLEDRIVEPOS2:
            memory[ 0x960] = trainer_invincibility ? 0x46 : 0x6A; // IORR,r2 *$963         -> ANDI,r2 $F8
            memory[ 0x961] = trainer_invincibility ? 0xF8 : 0x81;
        acase COMECOMEPOS:
            memory[ 0xB62] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase COMEFRUTASPOS1:
        case COMEFRUTASPOS2:
        case COMEFRUTASPOS3:
            memory[ 0xB6E] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase CRAZYGOBBLERPOS:
            memory[ 0x2DD] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase CRAZYCLIMBERPOS:
            memory[ 0xDA3] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase DORAEMONPOS:
            memory[ 0x210] = trainer_lives         ? 0xC0 : 0x20; // EORZ    r0            -> NOP
        acase DRSLUMPPOS:
            memory[0x2D97] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0

            memory[ 0x703] = trainer_time          ? 0x00 : 0x01; // LODI,r0 1             -> LODI,r0 0
        acase ENTERPRISE3POS:
            memory[0x114C] = trainer_time          ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
        acase ESCAPEPOS:
            memory[ 0x1FA] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0

            memory[ 0x25D] = trainer_invincibility ? 0x64 : 0x44; // ANDI,r0 2             -> IORI,r0 2
            memory[ 0x277] = trainer_invincibility ? 0x64 : 0x44; // ANDI,r0 4             -> IORI,r0 4
            memory[ 0x27D] = trainer_invincibility ? 0x64 : 0x44; // ANDI,r0 $20           -> IORI,r0 $20

            memory[ 0x733] = trainer_time          ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
        acase FROGGER1POS:
            memory[ 0x662] = trainer_invincibility ? 0xC0 : 0x3F; // BSTA,un $A7A          -> NOP snake
            memory[ 0x663] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x664] = trainer_invincibility ? 0xC0 : 0x7A; //                          NOP
            memory[ 0x67C] = trainer_invincibility ? 0xC0 : 0x3C; // BSTA,eq $A7A          -> NOP north
            memory[ 0x67D] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x67E] = trainer_invincibility ? 0xC0 : 0x7A; //                          NOP
            memory[ 0x687] = trainer_invincibility ? 0xC0 : 0xBC; // BSFA,eq $A7A          -> NOP south
            memory[ 0x688] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x689] = trainer_invincibility ? 0xC0 : 0x7A; //                          NOP
            memory[ 0x8FF] = trainer_invincibility ? 0xC0 : 0x3F; // BSTA,un $A7A          -> NOP not-home
            memory[ 0x900] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x901] = trainer_invincibility ? 0xC0 : 0x7A; //                          NOP

            memory[ 0x96A] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0xA91] = trainer_lives         ? 0x1F : 0x0C; // LODA,r0 $18C2         -> BCTA,un $AB9
            memory[ 0xA92] = trainer_lives         ? 0x0A : 0x18;
            memory[ 0xA93] = trainer_lives         ? 0xB9 : 0xC2;
        acase FROGGER2POS:
            memory[ 0x663] = trainer_invincibility ? 0xC0 : 0x3F; // BSTA,un $ADD          -> NOP snake
            memory[ 0x664] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x665] = trainer_invincibility ? 0xC0 : 0xDD; //                          NOP
            memory[ 0x67D] = trainer_invincibility ? 0xC0 : 0x3C; // BSTA,eq $ADD          -> NOP north
            memory[ 0x67E] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x67F] = trainer_invincibility ? 0xC0 : 0xDD; //                          NOP
            memory[ 0x688] = trainer_invincibility ? 0xC0 : 0xBC; // BSFA,eq $ADD          -> NOP south
            memory[ 0x689] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x68A] = trainer_invincibility ? 0xC0 : 0xDD; //                          NOP
            memory[ 0x962] = trainer_invincibility ? 0xC0 : 0x3F; // BSTA,un $ADD          -> NOP not-home
            memory[ 0x963] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x964] = trainer_invincibility ? 0xC0 : 0xDD; //                          NOP

            memory[ 0x9CD] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0xAFC] = trainer_lives         ? 0x1F : 0x0C; // LODA,r0 $18C2         -> BCTA,un DIEROUTINE_END ($B24)
            memory[ 0xAFD] = trainer_lives         ? 0x0B : 0x18;
            memory[ 0xAFE] = trainer_lives         ? 0x24 : 0xC2;
        acase FROGGER3POS:
            memory[ 0x663] = trainer_invincibility ? 0xC0 : 0x3F; // BSTA,un $ADD          -> NOP snake
            memory[ 0x664] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x665] = trainer_invincibility ? 0xC0 : 0xDD; //                          NOP
            memory[ 0x67D] = trainer_invincibility ? 0xC0 : 0x3C; // BSTA,eq $ADD          -> NOP north
            memory[ 0x67E] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x67F] = trainer_invincibility ? 0xC0 : 0xDD; //                          NOP
            memory[ 0x688] = trainer_invincibility ? 0xC0 : 0xBC; // BSFA,eq $ADD          -> NOP south
            memory[ 0x689] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x68A] = trainer_invincibility ? 0xC0 : 0xDD; //                          NOP
            memory[ 0x962] = trainer_invincibility ? 0xC0 : 0x3F; // BSTA,un $ADD          -> NOP not-home
            memory[ 0x963] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x964] = trainer_invincibility ? 0xC0 : 0xDD; //                          NOP

            memory[ 0x9CD] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0xAF5] = trainer_lives         ? 0x1F : 0x0C; // LODA,r0 $18C2         -> BCTA,un DIEROUTINE_END ($B1D)
            memory[ 0xAF6] = trainer_lives         ? 0x0B : 0x18;
            memory[ 0xAF7] = trainer_lives         ? 0x1D : 0xC2;
        acase FUNKYFISHPOS:
            memory[  0xDD] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0x7AD] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0xBC3] = trainer_invincibility ? 0x1B : 0x9A; // BCFR,lt $BCA          -> BCTR,un $BCA
            memory[ 0xCF7] = trainer_invincibility ? 0x00 : 0x12; // BCTR,un $D0A          -> BCTR,un $CF8
        acase GUNDAMPOS:
            memory[ 0x5C2] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
            memory[ 0xC00] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase HEADONPOS:
            memory[  0xDE] = trainer_invincibility ? 0x00 : 0x38; // ANDI,r3 $38           -> ANDI,r3 0
        acase HELICOPTERPOS:
            memory[ 0xD80] = trainer_time          ? 0x02 : 0x82; // ADDZ    r2            -> LODZ    r2
        acase HOBOPOS1:
        case  HOBOPOS2:
            memory[ 0x1B4] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
            memory[ 0x1D8] = trainer_lives         ? 0x00 : 0x01; // ADDI,r2 1             -> ADDI,r2 0

            memory[ 0x65C] = trainer_invincibility ? 0x00 : 0x3C; // LODI,r0 $3C           -> LODI,r0 $00  (level 3)
            memory[ 0x830] = trainer_invincibility ? 0x17 : 0x0C; // LODA,r0 SPRITE0Y      -> RETC,un      (level 1)
            memory[ 0x9A0] = trainer_invincibility ? 0x17 : 0x20; // EORZ    r0            -> RETC,un      (level 2)          
        acase HOMERUNPOS:
            memory[ 0x233] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase I_BOXINGPOS:
            memory[ 0x100] = trainer_time          ? 0x00 : 0x01; // SUBI,r2 1             -> SUBI,r2 0
        acase I_CIRCUSPOS:
            memory[ 0x3DF] = trainer_lives         ? 0x00 : 0x01; // SUBI,r1 1             -> SUBI,r1 0
        acase I_GOLFPOS:
            memory[ 0x787] = trainer_time          ? 0x66 : 0x67; // LODI,r0 $67           -> LODI,r0 $66
        acase I_MATH1POS:
            memory[ 0x306] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase I_MATH2POS:
            memory[ 0x2DC] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase E_MATH1POS:
            memory[ 0xD06] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase IKANASHIPOS:
            memory[ 0x275] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase INVADERAPOS:
            memory[ 0x377] = trainer_lives         ? 0x00 : 0x01; // SUBI,r2 1             -> SUBI,r2 0
        acase INVADERBPOS:
            memory[ 0x36C] = trainer_lives         ? 0x00 : 0x01; // SUBI,r2 1             -> SUBI,r2 0
        acase JOURNEYPOS:
            memory[0x2408] = trainer_lives         ? 0x00 : 0x01; // SUBI,r1 1             -> SUBI,r1 0

            memory[ 0x196] = trainer_invincibility ? 0x00 : 0x01; // BCTR,eq $198          -> BCTR,eq $197
        acase JUMPBUG1POS:
            memory[ 0x16B] = trainer_lives         ? 0x00 : 0x01;
            memory[ 0x190] = trainer_lives         ? 0x00 : 0x01;

            memory[ 0x354] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $35C          -> NOP
            memory[ 0x355] = trainer_invincibility ? 0xC0 : 0x06; //                          NOP
            memory[ 0x358] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $35C          -> NOP
            memory[ 0x359] = trainer_invincibility ? 0xC0 : 0x02; //                          NOP
            memory[ 0x4CD] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $4DB          -> NOP
            memory[ 0x4CE] = trainer_invincibility ? 0xC0 : 0x0C; //                          NOP
            memory[ 0x4D1] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $4DB          -> NOP
            memory[ 0x4D2] = trainer_invincibility ? 0xC0 : 0x08; //                          NOP

            memory[ 0x394] = trainer_time          ? 0x00 : 0xFF;
        acase JUMPBUG2POS:
            memory[ 0x149] = trainer_lives         ? 0x00 : 0x01;

            memory[ 0x305] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $30D          -> NOP
            memory[ 0x306] = trainer_invincibility ? 0xC0 : 0x06; //                          NOP
            memory[ 0x309] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $30D          -> NOP
            memory[ 0x30A] = trainer_invincibility ? 0xC0 : 0x02; //                          NOP
            memory[ 0x31C] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $324          -> NOP
            memory[ 0x31D] = trainer_invincibility ? 0xC0 : 0x06; //                          NOP
            memory[ 0x320] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $324          -> NOP
            memory[ 0x321] = trainer_invincibility ? 0xC0 : 0x02; //                          NOP
            memory[ 0x47E] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $48C          -> NOP
            memory[ 0x47F] = trainer_invincibility ? 0xC0 : 0x0C; //                          NOP
            memory[ 0x482] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $48C          -> NOP
            memory[ 0x483] = trainer_invincibility ? 0xC0 : 0x08; //                          NOP

            memory[ 0x345] = trainer_time          ? 0x00 : 0xFF;
        acase JUNGLERPOS:
            memory[ 0x149] = trainer_lives         ? 0x00 : 0x01;
        acase LASERATTACKPOS:
            memory[ 0x2E8] = trainer_lives         ? 0xC0 : 0xC8; // STRR,r0 *$2D9         -> NOP
            memory[ 0x2E9] = trainer_lives         ? 0xC0 : 0xEF; //                          NOP

            memory[ 0x4AD] = trainer_invincibility ? 0x00 : 0x01; // LODI,r0 1             -> LODI,r0 0
        acase LEAPFROGPOS:
            memory[ 0x4A0] = trainer_lives         ? 0x00 : 0x24; // IORI,r2 $24           -> IORI,r2 0

            memory[ 0x9CF] = trainer_time          ? 0xC0 : 0xD1; // RRL     r1            -> NOP
        acase MACROSSPOS:
            memory[ 0x21D] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
            memory[ 0x5E2] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[0x2946] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase MISSILEWARPOS:
            memory[ 0xE8B] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
            memory[ 0xE8F] = trainer_time          ? 0x17 : 0x04; // LODI,r0 $50           -> RETC,un
        acase MONACOPOS:
            memory[ 0x72F] = trainer_time          ? 0x00 : 0xFF; // ADDI,r0 $FF           -> ADDI,r0 0
            memory[ 0x730] = trainer_time          ? 0xC0 : 0x94; // DAR,r0                -> NOP
            memory[ 0x740] = trainer_time          ? 0x00 : 0xFF; // ADDI,r1 $FF           -> ADDI,r1 0
        acase MOONLANDING1POS:
        case MOONLANDING2POS:
            memory[ 0xA89] = trainer_time          ? 0x04 : 0x03; // LODZ    r3            -> LODI,r0 $F0
            memory[ 0xA8A] = trainer_time          ? 0xF0 : 0x24; // EORI,r0 $FF
            memory[ 0xA8B] = trainer_time          ? 0xCC : 0xFF; //                       -> STRA,r0 $1F52
            memory[ 0xA8C] = trainer_time          ? 0x1F : 0xC8; // STRR,r0 *$A71
            memory[ 0xA8D] = trainer_time          ? 0x52 : 0xE3;
            memory[ 0xA8E] = trainer_time          ? 0xCC : 0x5B; // BRNR,r3 $A89          -> STRA,r0 $1F52
            memory[ 0xA8F] = trainer_time          ? 0x1F : 0x79;
            memory[ 0xA90] = trainer_time          ? 0xC8 : 0xCB; // STRR,r3 *$A71
            memory[ 0xA91] = trainer_time          ? 0x1F : 0xDF; //                       -> BCTA,un $A66
            memory[ 0xA92] = trainer_time          ? 0x0A : 0x1F; // BCTA,un $A00
            memory[ 0xA93] = trainer_time          ? 0x66 : 0x0A;

            memory[ 0xC31] = trainer_time          ? 0xC0 : 0xD8; // BIRR,r0 $C33          -> NOP
            memory[ 0xC32] = trainer_time          ? 0xC0 : 0x00; //                          NOP

            memory[ 0xC56] = trainer_invincibility ? 0x00 : 0x0A; // BCTR,eq $C61          -> BCTR,eq $C57
        acase OMEGALANDINGPOS:
            // player-enemy collisions
            memory[ 0xF00] = trainer_invincibility ? 0x1B : 0x18; // BCTR,eq $F13          -> BCTR,un $F13

            if (memmap == MEMMAP_F) // because our patch is in expanded memory
            {   // player-building collisions
                memory[ 0xF19] = trainer_invincibility ? 0x1F : 0xCF; // STRA,r3 $1F5F         -> BCTA,un $1000
                memory[ 0xF1A] = trainer_invincibility ? 0x10 : 0x1F;
                memory[ 0xF1B] = trainer_invincibility ? 0x00 : 0x5F;
                memory[0x1000] = trainer_invincibility ? 0x47 : 0x00; //                          ANDI,r3 $7F
                memory[0x1001] = trainer_invincibility ? 0x7F : 0x00;
                memory[0x1002] = trainer_invincibility ? 0xCF : 0x00; //                          STRA,r3 $1F5F
                memory[0x1003] = trainer_invincibility ? 0x1F : 0x00;
                memory[0x1004] = trainer_invincibility ? 0x5F : 0x00;
                memory[0x1005] = trainer_invincibility ? 0x1F : 0x00; //                          BCTA,un $1FC
                memory[0x1006] = trainer_invincibility ? 0x0F : 0x00;
                memory[0x1007] = trainer_invincibility ? 0x1C : 0x00;
            }
        acase PARASHOOTERPOS:
            memory[ 0x269] = trainer_lives         ? 0x00 : 0x01; // SUBI,r1 1             -> SUBI,r1 0
        acase E_PINBALLPOS:
            memory[0x1359] = trainer_lives         ? 0x00 : 0x10; // SUBI,r1 $10           -> SUBI,r1 $00
        acase PLEIADESPOS:
            memory[ 0x6A8] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0x683] = memory[0xCE3] = trainer_invincibility ? 0x04 : 0x0C; // LODA,r0 $18D5 -> LODI,r0 1
            memory[ 0x684] = memory[0xCE4] = trainer_invincibility ? 0x01 : 0x18;
            memory[ 0x685] = memory[0xCE5] = trainer_invincibility ? 0xC0 : 0xD5; //                  NOP
        acase R2DTANKPOS:
            memory[ 0x4D4] = trainer_lives         ? 0x00 : 0x01; // ADDI,r1 1             -> ADDI,r1 0 (player 1)
            memory[ 0x4F0] = trainer_lives         ? 0x00 : 0x01; // ADDI,r1 1             -> ADDI,r1 0 (player 2)
        acase REDCLASHPOS:
        case REDCLASHODPOS:
            memory[ 0x4C0] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0x500] = trainer_invincibility ? 0x00 : 0x70; // LODI,r0 $70           -> LODI,r0 0
        acase ROBOTKILLERPOS:
            memory[ 0x24B] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0

            memory[ 0x2AE] = trainer_invincibility ? 0x64 : 0x44; // ANDI,r0 2             -> IORI,r0 2
            memory[ 0x2C8] = trainer_invincibility ? 0x64 : 0x44; // ANDI,r0 4             -> IORI,r0 4
            memory[ 0x2CE] = trainer_invincibility ? 0x64 : 0x44; // ANDI,r0 $20           -> IORI,r0 $20

            memory[ 0x73D] = trainer_time          ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
        acase ROCKETSHOOTINGPOS:
            memory[ 0xBED] = trainer_time          ? 0x00 : 0x01; // SUBI,r6 1             -> SUBI,r6 0
        acase ROUTE16POS:
            memory[ 0xA7A] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase SPACEATTACKAPOS:
            memory[ 0x18B] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0x2EF] = trainer_invincibility ? 0x00 : 0x04; // TMI,r2  $04           -> TMI,r2  $00
            memory[ 0x2F5] = trainer_invincibility ? 0x00 : 0x10; // TMI,r2  $10           -> TMI,r2  $00

            memory[ 0x310] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase SPACEATTACKBPOS:
            memory[ 0x1C5] =                         0x00       ; // BCTR,eq $1EC          -> BCTR,eq $1C6 (compatibility patch)

            memory[ 0x1F9] = trainer_invincibility ? 0x00 : 0x04; // TMI,r2  $04           -> TMI,r2  $00
            memory[ 0x1FF] = trainer_invincibility ? 0x00 : 0x10; // TMI,r2  $10           -> TMI,r2  $00

            memory[ 0x779] = trainer_time          ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0

            memory[ 0x7CB] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase SPACEATTACKCPOS:
            memory[ 0x1C5] =                         0x00       ; // BCTR,eq $1EC          -> BCTR,eq $1C6 (compatibility patch)

            memory[ 0x1F9] = trainer_invincibility ? 0x00 : 0x04; // TMI,r2  $04           -> TMI,r2  $00
            memory[ 0x1FF] = trainer_invincibility ? 0x00 : 0x10; // TMI,r2  $10           -> TMI,r2  $00

            memory[ 0x21A] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase SPACEBATTLEPOS:
            memory[ 0x923] = trainer_time          ? 0x1F : 0xFC; // BDRA,r0 $8D0          -> BCTA,un $8D0
            memory[ 0x936] = trainer_time          ? 0x1F : 0xFC; // BDRA,r0 $8D8          -> BCTA,un $8D8
        acase SPACEBUSTERPOS:
            memory[ 0x2D2] = trainer_invincibility ? 0x1B : 0x98; // BCFR,eq $2D7          -> BCTR,un $2D7

            memory[ 0x473] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase SPACEMISSIONPOS:
            memory[ 0x692] = trainer_time          ? 0x00 : 0xFF; // ADDI,r0 255           -> ADDI,r0 0
            memory[ 0x693] = trainer_time          ? 0xC0 : 0x94; // DAR                   -> NOP
        acase SPACESHOOTOUTPOS:
            memory[ 0xC06] = trainer_time          ? 0x00 : 0x01; // SUBI,r6 1             -> SUBI,r6 0
        acase SPACEVULTURESPOS:
            memory[ 0x6FE] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase SPACEWARPOS: // Interton
            memory[ 0x15C] = trainer_time          ? 0x00 : 0xFF; // ADDI,r0 255           -> ADDI,r0 0
            memory[ 0x15D] = trainer_time          ? 0xC0 : 0x94; // DAR                   -> NOP
        acase NIBBLEMENPOS:
        case SUPERGOBBLERPOS:
            memory[ 0x281] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
        acase SPACERAIDERSPOS:
            memory[ 0x10C] = trainer_time          ? 0x00 : 0xFF; // ADDI,r1 $FF           -> ADDI,r0 0

            memory[ 0x1A5] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        // execution flows through here
        case SPACESQUADRON1POS:
        case SPACESQUADRON2POS:
            if (whichgame != SPACERAIDERSPOS)
            {   memory[ 0x1A8] = trainer_lives     ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
            }

            if (trainer_invincibility)
            {   if (whichgame == SPACERAIDERSPOS)
                {   memory[ 0x752] = 0x1F; // BCTA,un $2040
                    memory[ 0x753] = 0x20;
                    memory[ 0x754] = 0x40;
                    memory[0x2054] = 0x07; // BCTA,un $755
                    memory[0x2055] = 0x55;
                } else
                {   memory[ 0x869] = 0x1F; // BCTA,un $2040
                    memory[ 0x86A] = 0x20;
                    memory[ 0x86B] = 0x40;
                    memory[0x2054] = 0x08; // BCTA,un $86C
                    memory[0x2055] = 0x6C;

                }
                memory[0x2040] = 0x0E; // LODA,r2 $1AB4
                memory[0x2041] = 0x1A;
                memory[0x2042] = 0xB4;
                memory[0x2043] = 0x66; // IORI,r2 $03
                memory[0x2044] = 0x03;
                memory[0x2045] = 0xCE; // STRA,r2 $1AB4
                memory[0x2046] = 0x1A;
                memory[0x2047] = 0xB4;
                memory[0x2048] = 0x0E; // LODA,r2 $1AB5
                memory[0x2049] = 0x1A;
                memory[0x204A] = 0xB5;
                memory[0x204B] = 0x66; // IORI,r2 $1F
                memory[0x204C] = 0x1F;
                memory[0x204D] = 0xCE; // STRA,r2 $1AB5
                memory[0x204E] = 0x1A;
                memory[0x204F] = 0xB5;
                memory[0x2050] = 0x0E; // LODA,r2 P1PADDLE
                memory[0x2051] = 0x19;
                memory[0x2052] = 0xFF;
                memory[0x2053] = 0x1F; // BCTA,un
            } else
            {   if (whichgame == SPACERAIDERSPOS)
                {   memory[ 0x752] = 0x0E; // LODA,r2 P1PADDLE
                    memory[ 0x753] = 0x19;
                    memory[ 0x754] = 0xFF;
                } else
                {   memory[ 0x869] = 0x0E; // LODA,r2 P1PADDLE
                    memory[ 0x86A] = 0x19;
                    memory[ 0x86B] = 0xFF;
                }
                for (i = 0x2040; i <= 0x2055; i++)
                {   memory[i] = 0;
            }   }
        acase SPIDERSPOS:
        case SPIDERSODPOS:
            memory[ 0x22A] = trainer_lives         ? 0x00 : 0x01; // ADDI,r2 1             -> ADDI,r2 0
        acase SUBMARINESRACINGPOS:
            memory[ 0x9AF] = trainer_lives         ? 0x66 : 0x67; // ADDI,r3 $67           -> ADDI,r3 $66 (for Racing)

            memory[ 0xDA2] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0   (for Submarines)
        acase SUPERBUG1POS:
        case SUPERBUG2POS:
            memory[ 0xAC3] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0x3A1] = trainer_invincibility ? 0xC0 : 0x9C;
            memory[ 0x3A2] = trainer_invincibility ? 0xC0 : 0x0A;
            memory[ 0x3A3] = trainer_invincibility ? 0xC0 : 0x91;
            memory[ 0x47E] = trainer_invincibility ? 0xC0 : 0x1C;
            memory[ 0x47F] = trainer_invincibility ? 0xC0 : 0x0A;
            memory[ 0x480] = trainer_invincibility ? 0xC0 : 0x91;
            memory[ 0x483] = trainer_invincibility ? 0xC0 : 0x1C;
            memory[ 0x484] = trainer_invincibility ? 0xC0 : 0x0A;
            memory[ 0x485] = trainer_invincibility ? 0xC0 : 0x91;

            memory[ 0x321] = trainer_time          ? 0x00 : 0x01; // SUBI,r2 1             -> SUBI,r2 0
        acase SUPERINVPOS:
            memory[  0x80] = trainer_time          ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
        acase INTELLIGENCE1POS:
            memory[ 0x1EA] = trainer_time          ? 0x00 : 0x01;
        acase SUPERMAZEPOS:
            memory[ 0x1EE] = trainer_time          ? 0x00 : 0x01;
        acase MAZESPOS:
            memory[ 0xAEA] = trainer_time          ? 0x00 : 0x01;
        acase SUPERKNOCKOUTPOS:
            memory[ 0x545] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase TANKSALOTPOS:
            memory[ 0x247] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0

            memory[ 0x2CE] = trainer_invincibility ? 0x11 : 0x05;
            memory[ 0x2CF] = trainer_invincibility ? 0x1B : 0xFF;
            memory[ 0x5D8] = trainer_invincibility ? 0x19 : 0x00;
        acase THEENDPOS1:
        case THEENDPOS2:
            memory[ 0x173] = trainer_lives         ? 0x00 : 0x01; // ADDI,r2 1             -> ADDI,r2 0
        acase TINYTIMPOS:
            memory[ 0xD46] = trainer_time          ? 0x00 : 0x01;
        acase TURTLESPOS:
            memory[ 0x153] = trainer_lives         ? 0x00 : 0x01; // ADDI,r2 1             -> ADDI,r2 0

            memory[ 0x386] = trainer_invincibility ? 0x00 : 0x05;

            memory[ 0x64A] = trainer_time          ? 0x00 : 0xFF; // ADDI,r0 $FF           -> ADDI,r0 0 // energy
        acase UFOSHOOTINGPOS:
            memory[ 0x9E6] = trainer_time          ? 0x00 : 0x01; // LODI,r1 1             -> LODI,r1 0 // ammo
}   }   }

MODULE void make_stars(void)
{   UWORD colour;
    int   destx, desty,
          inc,
          i, j,
          x, y;

    if (!enhancestars)
    {   return;
    } // implied else
        
    colour = pencolours[colourset][BLACK];
        
    // these 6 parts (sky, stars, nebula, sun, comet, moon) are order-dependent (farthest to nearest)!

    for (y = 0; y < MAXBOXHEIGHT; y++)
    {   for (x = 0; x < MAXBOXWIDTH; x++)
        {   stars[y][x] = colour;
    }   }

    j = (MAXBOXWIDTH * MAXBOXHEIGHT) / STARDENSITY;
    for (i = 0; i < j; i++)
    {   x = rand() % MAXBOXWIDTH;
        y = rand() % MAXBOXHEIGHT;
        stars[y][x] = (( 8 + (rand() % 24)) << 11)  // red   (5 bits)
                    + ((16 + (rand() % 48)) <<  5)  // green (6 bits)
                    + (( 8 + (rand() % 24))      ); // blue  (5 bits)
    }

    if (machines[machine].coinop) // unstretched
    {   if (rand() % NEBULAFREQ == 0)
        {   destx = rand() % (MAXBOXWIDTH - SMLNEBULAWIDTH);
            desty = rand() % (MAXBOXHEIGHT / 2); // top 1/2 of the screen
            for (x = 0; x < SMLNEBULAWIDTH; x++)
            {   for (y = 0; y < SMLNEBULAHEIGHT; y++)
                {   stars[desty + y][destx + x] = ((smlnebulaimagery[(y * SMLNEBULAWIDTH) + x] & 0x00F80000) >> 8)  // red   RRRRRrrr,gggggggg,bbbbbbbb -> RRRRRggg,gggbbbbb
                                                + ((smlnebulaimagery[(y * SMLNEBULAWIDTH) + x] & 0x0000FC00) >> 5)  // green rrrrrrrr,GGGGGGgg,bbbbbbbb -> rrrrrGGG,GGGbbbbb
                                                + ((smlnebulaimagery[(y * SMLNEBULAWIDTH) + x] & 0x000000F8) >> 3); // blue  rrrrrrrr,gggggggg,BBBBBbbb -> rrrrrggg,gggBBBBB
        }   }   }

        if (rand() % SUNFREQ == 0)
        {   destx = rand() % (MAXBOXWIDTH - SMLSUNWIDTH);
            desty = rand() % (MAXBOXHEIGHT - SMLSUNHEIGHT);
            for (x = 0; x < SMLSUNWIDTH; x++)
            {   for (y = 0; y < SMLSUNHEIGHT; y++)
                {   if (smlsunimagery[(y * SMLSUNWIDTH) + x] != 0x000000)
                    {   stars[desty + y][destx + x] = ((smlsunimagery[(y * SMLSUNWIDTH) + x] & 0x00F80000) >> 8)  // red   RRRRRrrr,gggggggg,bbbbbbbb -> RRRRRggg,gggbbbbb
                                                    + ((smlsunimagery[(y * SMLSUNWIDTH) + x] & 0x0000FC00) >> 5)  // green rrrrrrrr,GGGGGGgg,bbbbbbbb -> rrrrrGGG,GGGbbbbb
                                                    + ((smlsunimagery[(y * SMLSUNWIDTH) + x] & 0x000000F8) >> 3); // blue  rrrrrrrr,gggggggg,BBBBBbbb -> rrrrrggg,gggBBBBB
        }   }   }   }
    
        if (rand() % COMETFREQ == 0)
        {   destx = rand() % (MAXBOXWIDTH - SMLCOMETWIDTH);
            desty = rand() % ((MAXBOXHEIGHT / 8) * 3); // top 3/8ths of the screen
            for (x = 0; x < SMLCOMETWIDTH; x++)
            {   for (y = 0; y < SMLCOMETHEIGHT; y++)
                {   if (smlcometimagery[(y * SMLCOMETWIDTH) + x] != 0x000000)
                    {   stars[desty + y][destx + x] = ((smlcometimagery[(y * SMLCOMETWIDTH) + x] & 0x00F80000) >> 8)  // red   RRRRRrrr,gggggggg,bbbbbbbb -> RRRRRggg,gggbbbbb
                                                    + ((smlcometimagery[(y * SMLCOMETWIDTH) + x] & 0x0000FC00) >> 5)  // green rrrrrrrr,GGGGGGgg,bbbbbbbb -> rrrrrGGG,GGGbbbbb
                                                    + ((smlcometimagery[(y * SMLCOMETWIDTH) + x] & 0x000000F8) >> 3); // blue  rrrrrrrr,gggggggg,BBBBBbbb -> rrrrrggg,gggBBBBB
        }   }   }   }

        if (rand() % MOONFREQ == 0)
        {   destx = rand() % (MAXBOXWIDTH - SMLMOONWIDTH);
            desty = rand() % ((MAXBOXHEIGHT / 8) * 3); // top 3/8ths of the screen
            for (x = 0; x < SMLMOONWIDTH; x++)
            {   for (y = 0; y < SMLMOONHEIGHT; y++)
                {   if (smlmoonimagery[(y * SMLMOONWIDTH) + x] != 0x000000)
                    {   stars[desty + y][destx + x] = ((smlmoonimagery[(y * SMLMOONWIDTH) + x] & 0x00F80000) >> 8)  // red   RRRRRrrr,gggggggg,bbbbbbbb -> RRRRRggg,gggbbbbb
                                                    + ((smlmoonimagery[(y * SMLMOONWIDTH) + x] & 0x0000FC00) >> 5)  // green rrrrrrrr,GGGGGGgg,bbbbbbbb -> rrrrrGGG,GGGbbbbb
                                                    + ((smlmoonimagery[(y * SMLMOONWIDTH) + x] & 0x000000F8) >> 3); // blue  rrrrrrrr,gggggggg,BBBBBbbb -> rrrrrggg,gggBBBBB
    }   }   }   }   }
    else // stretched
    {   if (rand() % NEBULAFREQ == 0)
        {   destx = rand() % (MAXBOXWIDTH - SMLNEBULAWIDTH);
            desty = rand() % (MAXBOXHEIGHT / 2); // top 1/2 of the screen
            for (x = 0; x < SMLNEBULAWIDTH; x++)
            {   for (y = 0; y < SMLNEBULAHEIGHT; y++)
                {   stars[desty + (y * 2)    ][destx + x] =
                    stars[desty + (y * 2) + 1][destx + x] = ((smlnebulaimagery[(y * SMLNEBULAWIDTH) + x] & 0x00F80000) >> 8)  // red   RRRRRrrr,gggggggg,bbbbbbbb -> RRRRRggg,gggbbbbb
                                                          + ((smlnebulaimagery[(y * SMLNEBULAWIDTH) + x] & 0x0000FC00) >> 5)  // green rrrrrrrr,GGGGGGgg,bbbbbbbb -> rrrrrGGG,GGGbbbbb
                                                          + ((smlnebulaimagery[(y * SMLNEBULAWIDTH) + x] & 0x000000F8) >> 3); // blue  rrrrrrrr,gggggggg,BBBBBbbb -> rrrrrggg,gggBBBBB
        }   }   }

        if (rand() % SUNFREQ == 0)
        {   destx = rand() % (MAXBOXWIDTH - SMLSUNWIDTH);
            desty = rand() % (MAXBOXHEIGHT - (SMLSUNHEIGHT * 2));
            for (x = 0; x < SMLSUNWIDTH; x++)
            {   for (y = 0; y < SMLSUNHEIGHT; y++)
                {   if (smlsunimagery[(y * SMLSUNWIDTH) + x] != 0x000000)
                    {   stars[desty + (y * 2)    ][destx + x] =
                        stars[desty + (y * 2) + 1][destx + x] = ((smlsunimagery[(y * SMLSUNWIDTH) + x] & 0x00F80000) >> 8)  // red   RRRRRrrr,gggggggg,bbbbbbbb -> RRRRRggg,gggbbbbb
                                                              + ((smlsunimagery[(y * SMLSUNWIDTH) + x] & 0x0000FC00) >> 5)  // green rrrrrrrr,GGGGGGgg,bbbbbbbb -> rrrrrGGG,GGGbbbbb
                                                              + ((smlsunimagery[(y * SMLSUNWIDTH) + x] & 0x000000F8) >> 3); // blue  rrrrrrrr,gggggggg,BBBBBbbb -> rrrrrggg,gggBBBBB
        }   }   }   }
    
        if (rand() % COMETFREQ == 0)
        {   destx = rand() % (MAXBOXWIDTH - SMLCOMETWIDTH);
            desty = rand() % ((MAXBOXHEIGHT / 8) * 3); // top 3/8ths of the screen
            for (x = 0; x < SMLCOMETWIDTH; x++)
            {   for (y = 0; y < SMLCOMETHEIGHT; y++)
                {   if (smlcometimagery[(y * SMLCOMETWIDTH) + x] != 0x000000)
                    {   stars[desty + (y * 2)    ][destx + x] =
                        stars[desty + (y * 2) + 1][destx + x] = ((smlcometimagery[(y * SMLCOMETWIDTH) + x] & 0x00F80000) >> 8)  // red   RRRRRrrr,gggggggg,bbbbbbbb -> RRRRRggg,gggbbbbb
                                                              + ((smlcometimagery[(y * SMLCOMETWIDTH) + x] & 0x0000FC00) >> 5)  // green rrrrrrrr,GGGGGGgg,bbbbbbbb -> rrrrrGGG,GGGbbbbb
                                                              + ((smlcometimagery[(y * SMLCOMETWIDTH) + x] & 0x000000F8) >> 3); // blue  rrrrrrrr,gggggggg,BBBBBbbb -> rrrrrggg,gggBBBBB
        }   }   }   }

        if (rand() % MOONFREQ == 0)
        {   destx = rand() % (MAXBOXWIDTH - SMLMOONWIDTH);
            desty = rand() % ((MAXBOXHEIGHT / 8) * 3); // top 3/8ths of the screen
            for (x = 0; x < SMLMOONWIDTH; x++)
            {   for (y = 0; y < SMLMOONHEIGHT; y++)
                {   if (smlmoonimagery[(y * SMLMOONWIDTH) + x] != 0x000000)
                    {   stars[desty + (y * 2)    ][destx + x] =
                        stars[desty + (y * 2) + 1][destx + x] = ((smlmoonimagery[(y * SMLMOONWIDTH) + x] & 0x00F80000) >> 8)  // red   RRRRRrrr,gggggggg,bbbbbbbb -> RRRRRggg,gggbbbbb
                                                              + ((smlmoonimagery[(y * SMLMOONWIDTH) + x] & 0x0000FC00) >> 5)  // green rrrrrrrr,GGGGGGgg,bbbbbbbb -> rrrrrGGG,GGGbbbbb
                                                              + ((smlmoonimagery[(y * SMLMOONWIDTH) + x] & 0x000000F8) >> 3); // blue  rrrrrrrr,gggggggg,BBBBBbbb -> rrrrrggg,gggBBBBB
}   }   }   }   }   }

#ifndef LIBRETRO
JNIEXPORT jboolean JNICALL Java_com_amigan_droidarcadia_OptionsActivity_skiplevel(JNIEnv* env, jobject this)
{   int i;

    // these happen to all be Arcadia games at the moment but that is just coincidental
    switch (whichgame)
    {
    case  _3DATTACKPOS:                                                  memory[0x18DA] =   0x64;
    acase BREAKAWAYPOS:                                                  memory[0x18D5] =   0x01;
    acase CATTRAXPOS:                                                    memory[0x18E5] =   0x95;
    acase CRAZYGOBBLERPOS:                                               iar            =  0x227;
    acase ESCAPEPOS:                                                     iar            =  0x1DF;
    acase FROGGER1POS:                                                   memory[0x1802] = memory[0x1805] = memory[0x1808] = memory[0x180B] = memory[0x180E] = 0x78;
                                                                         pushras(); iar =  0xAFF;
    acase FROGGER2POS:                                                   memory[0x1802] = memory[0x1805] = memory[0x1808] = memory[0x180B] = memory[0x180E] = 0x78;
                                                                         pushras(); iar =  0xB4E; // LIVEROUTINE
    acase FROGGER3POS:                                                   memory[0x1802] = memory[0x1805] = memory[0x1808] = memory[0x180B] = memory[0x180E] = 0x78;
                                                                         pushras(); iar =  0xB47; // LIVEROUTINE
    acase FUNKYFISHPOS:                                                  memory[0x1AD0] = memory[0x1AD1] = memory[0x1AD2] = memory[0x1AD3] = memory[0x1AD4] = 0;
    acase HOBOPOS1:         case HOBOPOS2:                               memory[A_SPRITE1X] = 0x9B;
                                                                         memory[A_SPRITE1Y] = 0xCE;
    acase JUMPBUG1POS:                                                   iar            =  0x495;
    acase JUMPBUG2POS:                                                   iar            =  0x446;
    acase MONACOPOS:
        if (memory[0x1AF0] >= 2) // ie. if not already on level 2
        {   memory[0x1AF0] = 2; // minutes
            memory[0x1AEF] = 0; // seconds (in packed BCD format)
        } // Note that this requires unlimited time trainer to be turned off, momentarily at least
    acase NIBBLEMENPOS:     case SUPERGOBBLERPOS:                        memory[0x18E5] =   0x9A;
    acase REDCLASHPOS:      case REDCLASHODPOS:                          iar            =  0x3F4;
    acase ROBOTKILLERPOS:                                                iar            =  0x230;
    acase SPACEATTACKAPOS:  case SPACEATTACKBPOS:  case SPACEATTACKCPOS:
        for (i = 0x1820; i <= 0x187F; i++)
        {   memory[i] = 0;
        }
        memory[0x18D4] = memory[0x18D5] = 0xE4;
        memory[0x18E3] = memory[0x18E4] = 0x32;
        memory[A_SPRITE3X] = 0;
    acase SPIDERSPOS:       case SPIDERSODPOS:                           iar            =  0x796;
    acase SUPERBUG1POS:     case SUPERBUG2POS:                           iar            =  0xA6A; // or iar = 0xA53, if you want to play end-of-level song first
    acase TURTLESPOS:                                                    iar            =  0x273;
    adefault:
        return (jboolean) FALSE;
    }
    
    return (jboolean) TRUE;
}

JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_doautofire(JNIEnv* env, jobject this)
{   if (whichgame == _3DATTACKPOS)
    {   if ((frames % totalframes) < downframes)
        {   if (frames % 30 >= 15)
            {   return (jint) 4; // 2nd firebutton
            } else
            {   return (jint) 3; // 1st firebutton
        }   }
        else
        {   return (jint) 2;
    }   }
    else
    {   if ((frames % totalframes) < downframes)
        {   return (jint) 1; // 1st firebutton
        } else
        {   return (jint) 0;
}   }   }

JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_getfirebuttons(JNIEnv* env, jobject this)
{   int rc =  key1
           | (key2 <<  4)
           | (key3 <<  8)
           | (key4 << 12);

    return (jint) rc;
}
JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_OptionsActivity_getfirebuttons(JNIEnv* env, jobject this)
{   int rc =  key1
           | (key2 <<  4)
           | (key3 <<  8)
           | (key4 << 12);

    return (jint) rc;
}

JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_getdigipos(JNIEnv* env, jobject this, jint whichpos)
{   return (jint) machines[machine].digipos[(int) whichpos];
}
JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_OptionsActivity_getdigipos(JNIEnv* env, jobject this, jint whichpos)
{   return (jint) machines[machine].digipos[(int) whichpos];
}

JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_getsensitivity(JNIEnv* env, jobject this)
{   return (jint) sensitivity;
}

JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_cloadcos(JNIEnv* env, jobject this, jint whichtype, jint length, jbyteArray ptr, jboolean reconfigure)
{   int rc;

    if (whichtype == 0) strcpy(fn_game, "Autosave.cos"); else strcpy(fn_game, "Quicksave.cos");
    // jsize length  = (*env)->GetArrayLength(env, ptr); // another way of getting the length
    filesize = (int) length;
    jbyte* IOBuffer2 = (*env)->GetByteArrayElements(env, ptr, 0);
    if ((IOBuffer = malloc(filesize)))
    {   memcpy(IOBuffer, IOBuffer2, filesize);
        (*env)->ReleaseByteArrayElements(env, ptr, IOBuffer2, JNI_ABORT);
        rc = parse_bytes(reconfigure ? TRUE : FALSE);
        free(IOBuffer);
        return (jint) rc;
    } else
    {   (*env)->ReleaseByteArrayElements(env, ptr, IOBuffer2, JNI_ABORT);
        return (jint) FALSE;
}   }
JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_OptionsActivity_cloadcos(JNIEnv* env, jobject this, jint whichtype, jint length, jbyteArray ptr, jboolean reconfigure)
{   int rc;

    if (whichtype == 0) strcpy(fn_game, "Autosave.cos"); else strcpy(fn_game, "Quicksave.cos");
    // jsize length  = (*env)->GetArrayLength(env, ptr); // another way of getting the length
    filesize = (int) length;
    jbyte* IOBuffer2 = (*env)->GetByteArrayElements(env, ptr, 0);
    if ((IOBuffer = malloc(filesize)))
    {   memcpy(IOBuffer, IOBuffer2, filesize);
        (*env)->ReleaseByteArrayElements(env, ptr, IOBuffer2, JNI_ABORT);
        rc = parse_bytes(reconfigure ? TRUE : FALSE);
        free(IOBuffer);
        return (jint) rc;
    } else
    {   (*env)->ReleaseByteArrayElements(env, ptr, IOBuffer2, JNI_ABORT);
        return (jint) FALSE;
}   }

JNIEXPORT jbyteArray JNICALL Java_com_amigan_droidarcadia_MainActivity_csavecos(JNIEnv* env, jobject this)
{   serializemode = SERIALIZE_WRITE;

    if (!machines[machine].coinop && cheevos)
    {   cheevos_buffer_size = rc_client_progress_size(g_client);
    } else
    {   cheevos_buffer_size = 0;
    }        
    IOBuffer = malloc((36 * KILOBYTE) + cheevos_buffer_size); // shouild check return code
    serialize_cos();
    jintArray rc = (*env)->NewByteArray(env, offset);
    (*env)->SetByteArrayRegion(env, rc, 0, offset, (const jbyte*) IOBuffer);
    free(IOBuffer);

    return rc;
}
JNIEXPORT jbyteArray JNICALL Java_com_amigan_droidarcadia_OptionsActivity_csavecos(JNIEnv* env, jobject this)
{   serializemode = SERIALIZE_WRITE;

    if (!machines[machine].coinop && cheevos)
    {   cheevos_buffer_size = rc_client_progress_size(g_client);
    } else
    {   cheevos_buffer_size = 0;
    }        
    IOBuffer = malloc((36 * KILOBYTE) + cheevos_buffer_size); // shouild check return code
    serialize_cos();
    jintArray rc = (*env)->NewByteArray(env, offset);
    (*env)->SetByteArrayRegion(env, rc, 0, offset, (const jbyte*) IOBuffer);
    free(IOBuffer);

    return rc;
}
#endif /* !LIBRETRO */

EXPORT void serialize_cos(void)
{   int i,
        x, y;

    offset = 0;
    if (serializemode == SERIALIZE_WRITE)
    {   IOBuffer[0] = 0xBF;
        IOBuffer[1] = machines[machine].cosversion;
        IOBuffer[2] = 2; // means DroidArcadia COS (0 means Ami/WinArcadia COS, 1 means COR)
        IOBuffer[3] = memmap;
    }
    offset = 4;

    serialize_word_int(&whichgame); // must be here!
    serialize_word(&iar);
    serialize_byte(&psu);
    serialize_byte(&psl);
    for (i = 0; i < 8; i++)
    {   serialize_word(&ras[i]);
    }
    for (i = 0; i < 7; i++)
    {   serialize_byte(&r[i]);
    }
    serialize_byte_int(&interrupt_2650);
    serialize_word_int(&romsize);
    serialize_byte(&startaddr_h);
    serialize_byte(&startaddr_l);
    serialize_long(&frames);
    serialize_long(&cycles_2650);
    serialize_long(&oldcycles);
    serialize_word_int(&nextinst);
    serialize_long((ULONG*) &cpl); // this one is unnecessary on DroidArcadia as cpl isn't user-adjustable
    if (serializemode == SERIALIZE_READ)
    {   set_cpl(cpl);
    }

    switch (machine)
    {
    case ARCADIA:
        serialize_long((ULONG*) &fractionator);
        serialize_byte_int((int*) &region);
    //lint -fallthrough
    case INTERTON:
        serialize_long((ULONG*) &cheevosize);
        for (i =      0; i <=      0x7FFF; i++) /*  32K   */ serialize_byte(&memory[i]);
    acase ELEKTOR:
        serialize_byte_int(&elektor_biosver);
        serialize_long((ULONG*) &cheevosize);
        for (i =  0x800; i <=      0x1FFF; i++) /*   6K   */ serialize_byte(&memory[i]);
        for (i =      0; i <   cheevosize; i++) /*   8K   */ serialize_byte(&cheevomem[i]);
    acase MALZAK:
        serialize_coinops();
        for (i = 0x1000; i <= 0x1FFF; i++) /*  4K    */ serialize_byte(&memory[i]);
        /* DIPs are at memory[$14CC] and thus get done above.
           updatedips() gets done for us by engine_load() */
        for (x = 0; x < 16; x++)
        {   for (y = 0; y < 16; y++)
            {   serialize_byte(&malzak_field[x][y]); // 16*16=256
        }   }
        serialize_byte_int(&malzak_bank1);
        serialize_byte_int(&malzak_bank2);
        serialize_byte_int(&malzak_x);
        serialize_byte_int(&malzak_y);
    acase ZACCARIA:
        serialize_coinops();
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:
        case MEMMAP_GALAXIA:
            for (i = 0x1400; i <= 0x1FFF; i++) /* 3K  */ serialize_byte(&memory[i]);
            for (i =      0; i <=  0x3FF; i++) /* 1K  */ serialize_byte(&g_bank1[i]);
            for (i =      0; i <=     15; i++) /* 16b */ serialize_byte(&g_bank2[i]);
            serialize_byte(         &ioport[PORTD]);
            serialize_byte(         &awga_collide);
            serialize_byte((UBYTE*) &galaxia_scrolly);
            if (memmap == MEMMAP_ASTROWARS)
            {   serialize_byte(&aw_dips1);
                serialize_byte(&aw_dips2);
                // updatedips() gets done for us by engine_load()
            } else
            {   // assert(memmap == MEMMAP_GALAXIA);
                serialize_byte(&ga_dips); // updatedips() gets done for us by engine_load()
            }
        acase MEMMAP_LASERBATTLE:
        case MEMMAP_LAZARIAN:
            if (memmap == MEMMAP_LAZARIAN)
            {   serialize_byte(&lz_dips2);
                serialize_byte(&lz_dips3); // updatedips() gets done for us by engine_load()
            } else
            {   // assert(memmap == MEMMAP_LASERBATTLE);
                serialize_byte(&lb_dips ); // updatedips() gets done for us by engine_load()
                offset++;
            }
            serialize_byte(&lb_bank);
            serialize_byte(&mux);
            serialize_byte(&lb_spritecode);
            serialize_byte(&lb_spritecolour);
            serialize_byte(&lb_spriteenable);
            serialize_byte(&lb_spritex);
            serialize_byte(&lb_spritey);
            serialize_byte(&lb_tone);
            serialize_long(&lb_snd);
            for (i = 0x1500; i <= 0x1FFF; i++) /* 3K */ serialize_byte(&memory[i]);
            serialize_byte(&circle);
            for (i = 0; i < 512; i++)
            {   serialize_byte(&memory_effects[i]);
            }
            serialize_byte(&abeff1);
            serialize_byte(&abeff2);
            serialize_byte(&useshell);
            serialize_byte(&neg1);
            serialize_byte(&neg2);
    }   }
    
    if (!machines[machine].coinop && cheevos && g_client && offset < filesize)
    {   if (serializemode == SERIALIZE_READ)
        {   if
            (   IOBuffer[offset    ] == 'R'
             && IOBuffer[offset + 1] == 'C'
             && IOBuffer[offset + 2] == 'H'
             && IOBuffer[offset + 3] == 'V'
            )
            {   // found achievement data marker
                offset += 4;
                memcpy(&cheevos_buffer_size, &IOBuffer[offset], 4); // assumes endianness?
                offset += 4;
                DISCARD rc_client_deserialize_progress(g_client, &IOBuffer[offset]);
                // offset += cheevos_buffer_size;
        }   }
        else
        {   // assert(serializemode == SERIALIZE_WRITE);
            if (rc_client_serialize_progress(g_client, &IOBuffer[offset + 8]) == RC_OK)
            {   // write a marker, the buffer size, then the buffer contents
                WriteByte('R');
                WriteByte('C');
                WriteByte('H');
                WriteByte('V');
                memcpy(&IOBuffer[offset], &cheevos_buffer_size, 4); // assumes endianness?
                // offset += 4 + cheevos_buffer_size;
}   }   }   }

#ifndef LIBRETRO
JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_OptionsActivity_creset(JNIEnv* env, jobject this)
{   engine_reset();
}

JNIEXPORT jbyteArray JNICALL Java_com_amigan_droidarcadia_GameInfoActivity_getautotext(JNIEnv* env, jobject this, jint whichfield)
{   STRPTR     strptr;
    int        cwhichfield,
               length;
    jbyte*     pNativeMessage;
    jbyteArray rc;
    
    cwhichfield = (int) whichfield;

    if (whichgame == -1)
    {   if (machines[machine].coinop)
        {   switch (cwhichfield)
            {
            case  0: strptr = (STRPTR) memmapinfo[memmap].name;
            acase 1: strptr = (STRPTR) memmapinfo[memmap].credits;
            acase 2: strptr = "-";
            acase 3: strptr = overlays[whichoverlay][32];
        }   }
        else
        {   strptr = "?";
    }   }
    else
    {   switch (cwhichfield)
        {
        case  0: strptr = known[whichgame].name;
        acase 1: strptr = known[whichgame].credits;
        acase 2: strptr = known[whichgame].reference;
        acase 3: strptr = overlays[whichoverlay][32];
    }   }

    length = strlen(strptr);
    rc = (*env)->NewByteArray(env, length);
    pNativeMessage = (jbyte*) strptr;
    (*env)->SetByteArrayRegion(env, rc, 0, length, pNativeMessage);

    return rc;
}

JNIEXPORT jbyteArray JNICALL Java_com_amigan_droidarcadia_MainActivity_getkeyname(JNIEnv* env, jobject this, jint whichkey)
{   jbyte*     pNativeMessage;
    jbyteArray rc;
    int        cwhichkey,
               length;
    STRPTR     strptr;

    cwhichkey = (int) whichkey;

    // assert(whichoverlay >= 0 && whichoverlay < KNOWNGAMES);
    switch (cwhichkey)
    {
    case   0: strptr = overlays[whichoverlay][ 8 + (swapped ? 3 : 0)]; // "1"
    acase  1: strptr = overlays[whichoverlay][ 0 + (swapped ? 1 : 0)]; // "2/Fi"
    acase  2: strptr = overlays[whichoverlay][10 + (swapped ? 3 : 0)]; // "3"
    acase  3: strptr = overlays[whichoverlay][14 + (swapped ? 3 : 0)]; // "4"
    acase  4: strptr = overlays[whichoverlay][15 + (swapped ? 3 : 0)]; // "5"
    acase  5: strptr = overlays[whichoverlay][16 + (swapped ? 3 : 0)]; // "6"
    acase  6: strptr = overlays[whichoverlay][20 + (swapped ? 3 : 0)]; // "7"
    acase  7: strptr = overlays[whichoverlay][21 + (swapped ? 3 : 0)]; // "8"
    acase  8: strptr = overlays[whichoverlay][22 + (swapped ? 3 : 0)]; // "9"
    acase  9: strptr = overlays[whichoverlay][26 + (swapped ? 3 : 0)]; // "Cl"
    acase 10: strptr = overlays[whichoverlay][27 + (swapped ? 3 : 0)]; // "0"
    acase 11: strptr = overlays[whichoverlay][28 + (swapped ? 3 : 0)]; // "En"
    acase 12: strptr = machines[machine].consolekeyname[0];
    acase 13: strptr = machines[machine].consolekeyname[1];
    acase 14: strptr = machines[machine].consolekeyname[2];
    acase 15: strptr = machines[machine].consolekeyname[3];
    }
    length = strlen(strptr);
    rc = (*env)->NewByteArray(env, length);
    pNativeMessage = (jbyte*) strptr;
    (*env)->SetByteArrayRegion(env, rc, 0, length, pNativeMessage);

    return rc;
}

JNIEXPORT jbyteArray JNICALL Java_com_amigan_droidarcadia_GameInfoActivity_getgglink(JNIEnv* env, jobject this)
{   jbyte*     pNativeMessage;
    jbyteArray rc;
    int        length;
    TEXT       ggstring[60 + 1];

    switch (machine)
    {
    case ARCADIA:
    case INTERTON:
    case ELEKTOR:
        if (machine == ARCADIA)
        {   strcpy(ggstring, "http://amigan.yatho.com/agg/");
        } elif (machine == INTERTON)
        {   strcpy(ggstring, "http://amigan.yatho.com/igg/");
        } elif (machine == ELEKTOR)
        {   strcpy(ggstring, "http://amigan.yatho.com/egg/");
        }
        if (whichgame != -1 && known[whichgame].ggnode[0])
        {   strcat(ggstring, "#");
            strcat(ggstring, known[whichgame].ggnode);
        }
    adefault: // eg. coin-ops
        strcpy(ggstring, "http://amigan.yatho.com/s-coding.html#coin-opd");
    }

    length = strlen((const char*) ggstring);
    rc = (*env)->NewByteArray(env, length);
    pNativeMessage = (jbyte*) ggstring;
    (*env)->SetByteArrayRegion(env, rc, 0, length, pNativeMessage);

    return rc;
}

JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_setconsolekey(JNIEnv* env, jobject this, jint whichkey)
{   int cwhichkey,
        rc;

    cwhichkey = (int) whichkey;

    switch (cwhichkey)
    {
    case  0:
        if (machine == ELEKTOR)
        {   console_start = (whichgame == SUBMARINESRACINGPOS) ? 30 : 24;
        } else
        {   console_start = 16;
        }
        rc = console_start;
    acase 1:
        switch (machine)
        {
        case INTERTON:
            console_a = 16; // must be exactly 16 for Hunting
        acase ELEKTOR:
            switch (whichgame)
            {
            case  PENALTYPOS:          console_a = 24;
            acase SUBMARINESRACINGPOS: console_a = 30;
            adefault:                  console_a = 16; // must be exactly 16 for Hunting
            }
        adefault:
            console_a = 5;
        }
        rc = console_a;
    acase 2:
        console_b = 5;
        rc = console_b;
    acase 3:
        console_reset = 5;
        rc = console_reset;
    }

    return (jint) rc;
}

JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_getff(JNIEnv* env, jobject this)
{   int rc;

    if (useff)
    {   if (swapped)
        {   if (p2rumble < 0) rc = -p2rumble + 1; elif (p2rumble > 0) rc = p2rumble + 1; else rc = 0;
        } else
        {   if (p1rumble < 0) rc = -p1rumble + 1; elif (p1rumble > 0) rc = p1rumble + 1; else rc = 0;
    }   }
    else
    {   rc = 0;
    }

    return (jint) rc;
}

/* JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_getcycles(JNIEnv* env, jobject this)
{   return (jint) cycles_2650;
} */

JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_ArcadiaHiScoresActivity_gethiscore(JNIEnv* env, jobject this, jint whichscore)
{   return (jint) hiscore[(int) whichscore].score;
}
JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_CoinOpHiScoresActivity_gethiscore(JNIEnv* env, jobject this, jint whichscore)
{   return (jint) hiscore[(int) whichscore].score;
}
JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_gethiscore(JNIEnv* env, jobject this, jint whichscore)
{   return (jint) hiscore[(int) whichscore].score;
}
JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_ArcadiaHiScoresActivity_sethiscore(JNIEnv* env, jobject this, jint whichscore, jint thescore)
{   hiscore[(int) whichscore].score = (int) thescore;
}
JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_CoinOpHiScoresActivity_sethiscore(JNIEnv* env, jobject this, jint whichscore, jint thescore)
{   hiscore[(int) whichscore].score = (int) thescore;
}
JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_MainActivity_sethiscore(JNIEnv* env, jobject this, jint whichscore, jint thescore)
{   hiscore[(int) whichscore].score = (int) thescore;
}

JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_MainActivity_redrawscreen(JNIEnv* env, jobject this)
{   int boxwidth,
        boxheight,
        x, y;

    switch (machine)
    {
    case ARCADIA:
    case INTERTON:
    case ELEKTOR:
        boxwidth  = 164;
        boxheight = (region == REGION_NTSC) ? 226 : 269;
    acase ZACCARIA:
    case MALZAK:
        boxwidth  =
        boxheight = 240;
    }

    if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
    {   for (y = 0; y < boxheight; y++)
        {   for (x = 0; x < boxwidth; x++)
            {   if (!enhancestars || fgtable[y][x])
                {   *(display + (x * MAXBOXWIDTH) + 239 - y) = pencolours[colourset][screen[x][y]];
                } else
                {   *(display + (x * MAXBOXWIDTH) + 239 - y) = stars[y][x];
    }   }   }   }
    else
    {   for (y = 0; y < boxheight; y++)
        {   for (x = 0; x < boxwidth; x++)
            {   if (!enhancestars || fgtable[y][x])
                {   *(display + (y * MAXBOXWIDTH) + x) = pencolours[colourset][screen[x][y]];
                } else
                {   *(display + (y * MAXBOXWIDTH) + x) = stars[y][x];
}   }   }   }   }
    
JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_OptionsActivity_redrawscreen(JNIEnv* env, jobject this)
{   int boxwidth,
        boxheight,
        x, y;

    switch (machine)
    {
    case ARCADIA:
    case INTERTON:
    case ELEKTOR:
        boxwidth  = 164;
        boxheight = (region == REGION_NTSC) ? 226 : 269;
    acase ZACCARIA:
    case MALZAK:
        boxwidth  =
        boxheight = 240;
    }

    if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
    {   for (y = 0; y < boxheight; y++)
        {   for (x = 0; x < boxwidth; x++)
            {   if (!enhancestars || fgtable[y][x])
                {   *(display + (x * MAXBOXWIDTH) + 239 - y) = pencolours[colourset][screen[x][y]];
                } else
                {   *(display + (x * MAXBOXWIDTH) + 239 - y) = stars[y][x];
    }   }   }   }
    else
    {   for (y = 0; y < boxheight; y++)
        {   for (x = 0; x < boxwidth; x++)
            {   if (!enhancestars || fgtable[y][x])
                {   *(display + (y * MAXBOXWIDTH) + x) = pencolours[colourset][screen[x][y]];
                } else
                {   *(display + (y * MAXBOXWIDTH) + x) = stars[y][x];
}   }   }   }   }
#endif /* !LIBRETRO */

EXPORT UBYTE readport(int port)
{   UBYTE t;

    switch (port)
    {
    case  111: // $6F
        t = 0x55;
    acase 112: // $70
        t = 5; // means DroidArcadia
    acase 113: // $71
        t = MAJORVERSION;
    acase 114: // $72
        t = MINORVERSION;
    acase 115: // $73
        t = (UBYTE) ((cycles_2650 / 3) % 256);
    adefault:
        switch (machine)
        {
        case  ZACCARIA: t = zaccaria_readport(port);
        acase MALZAK:   t = malzak_readport(port);
        adefault:       t = 0; // arbitrary
    }   }

    return t;
}

EXPORT void writeport(int port, UBYTE data)
{   switch (machine)
    {
    acase MALZAK:     malzak_writeport(port, data);
    case  ZACCARIA: zaccaria_writeport(port, data);
}   }

#ifndef LIBRETRO
JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_GameInfoActivity_getglyph(JNIEnv* env, jobject this)
{   switch (whichgame)
    {
    case _3DATTACKPOS:                              return  0;
    case _3DSOCCERAPOS:     case _3DSOCCERBPOS:     case _3DSOCCERENHPOS:
                                                    return  1;
    case HOMERUNPOS:                                return  2;
    case ALIENINVPOS:       case ALIENINV1POS:      case ALIENINV2POS: case ALIENINV3POS: case ALIENINV4POS:
                                                    return  3;
    case ASTROINVPOS:       case ASTROINVODPOS:     return  4;
    case AUTORACEPOS:       case AUTORACEODPOS:     return  5;
    case BASEBALLPOS:                               return  6;
    case A_BASKETBALLPOS:                           return  7;
    case BATTLEPOS:                                 return  8;
    case A_BLACKJACKPOS:                            return  9;
    case A_BOWLINGPOS:                              return 10;
    case A_CAPTUREPOS:                              return 11;
    case CATTRAXPOS:                                return 12;
    case A_CIRCUSPOS:                               return 13;
    case A_COMBATPOS:       case A_COMBATODPOS:     return 14;
    case CRAZYCLIMBERPOS:                           return 15;
    case CRAZYGOBBLERPOS:                           return 16;
    case VIDLEXEGPOS:       case VIDLEXGEPOS:       return 17;
    case DORAEMONPOS:                               return 18;
    case DRSLUMPPOS:                                return 19;
    case ESCAPEPOS:                                 return 20;
    case GRIDIRON1POS:      case GRIDIRON2POS:      return 21;
    case FROGGER1POS:       case FROGGER2POS:       case FROGGER3POS:
                                                    return 22;
    case FUNKYFISHPOS:                              return 23;
    case A_GOLFPOS1:        case A_GOLFPOS2:        case A_GOLFODPOS:
                                                    return 24;
    case GUNDAMPOS:                                 return 25;
    case HOBOPOS1:          case HOBOPOS2:          return 26;
    case A_HORSERACINGPOS:                          return 27;
    case JOURNEYPOS:                                return 28;
    case JUMPBUG1POS:       case JUMPBUG2POS:       return 29;
    case JUNGLERPOS:                                return 30;
    case MACROSSPOS:                                return 31;
    case MISSILEWARPOS:                             return 32;
    case MONACOPOS:                                 return 33;
    case NIBBLEMENPOS:      case SUPERGOBBLERPOS:   return 34;
    case OCEANBATTLEPOS:                            return 35;
    case PARASHOOTERPOS:                            return 36;
    case PLEIADESPOS:                               return 37;
    case R2DTANKPOS:                                return 38;
    case REDCLASHPOS:       case REDCLASHODPOS:     return 39;
    case ROBOTKILLERPOS:                            return 40;
    case ROUTE16POS:                                return 41;
    case _2DSOCCERPOS:      case _2DSOCCERODPOS:    return 42;
    case SPACEATTACKAPOS:   case SPACEATTACKBPOS:   case SPACEATTACKCPOS:
                                                    return 43;
    case SPACEBUSTERPOS:                            return 44;
    case SPACEMISSIONPOS:                           return 45;
    case SPACERAIDERSPOS:                           return 46;
    case SPACESQUADRON1POS: case SPACESQUADRON2POS: return 47;
    case SPACEVULTURESPOS:  case MOTHERSHIPPOS:     return 48;
    case SPIDERSPOS:        case SPIDERSODPOS:      return 49;
    case SUPERBUG1POS:      case SUPERBUG2POS:      return 50;
    case TANKSALOTPOS:                              return 51;
    case TENNISPOS:                                 return 52;
    case THEENDPOS1:        case THEENDPOS2:        return 53;
    case TURTLESPOS:                                return 54;
    // new ones
    case A_BOXINGPOS:                               return 55;
    case BRAINQUIZPOS:                              return 56;
    case BREAKAWAYPOS:                              return 57;
    case STARCHESSPOS:      case STARCHESSENHPOS:   return 58;
    adefault:
        if      (machine == ARCADIA)                return 59;
        else if (machine == INTERTON)               return 60;
        else if (machine == ELEKTOR)                return 61;
        else if (memmap  == MEMMAP_ASTROWARS)       return 62;
        else if (memmap  == MEMMAP_GALAXIA)         return 63;
        else if (memmap  == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN) return 64;
        else if (machine == MALZAK)                 return 65;
        return 0; // should never happen
}   }

JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_GameInfoActivity_getbox(JNIEnv* env, jobject this)
{   int rc;

    switch (whichgame)
    {
    // Arcadia
    case  _3DATTACKPOS:      rc =  0;
    acase _3DSOCCERAPOS:
    case  _3DSOCCERBPOS:
    case  _3DSOCCERENHPOS:   rc =  1;
    acase ALIENINVPOS:
    case  ALIENINV1POS:
    case  ALIENINV2POS:
    case  ALIENINV3POS:
    case  ALIENINV4POS:      rc =  2;
    acase GRIDIRON1POS:
    case  GRIDIRON2POS:      rc =  3;
    acase ASTROINVPOS:
    case  ASTROINVODPOS:     rc =  4;
    acase AUTORACEPOS:
    case  AUTORACEODPOS:     rc =  5;
    acase BASEBALLPOS:       rc =  6;
    acase A_BASKETBALLPOS:   rc =  7;
    acase A_BLACKJACKPOS:    rc =  8;
    acase A_BOWLINGPOS:      rc =  9;
    acase A_BOXINGPOS:       rc = 10;
    acase BRAINQUIZPOS:      rc = 11;
    acase BREAKAWAYPOS:      rc = 12;
    acase A_CAPTUREPOS:      rc = 13;
    acase CATTRAXPOS:        rc = 14;
    acase A_CIRCUSPOS:       rc = 15;
    acase A_COMBATPOS:
    case  A_COMBATODPOS:     rc = 16;
    acase CRAZYCLIMBERPOS:   rc = 17;
    acase CRAZYGOBBLERPOS:   rc = 18;
    acase DORAEMONPOS:       rc = 19;
    acase DRSLUMPPOS:        rc = 20;
    acase ESCAPEPOS:         rc = 21;
    acase FROGGER1POS:
    case  FROGGER2POS:
    case  FROGGER3POS:       rc = 22;
    acase FUNKYFISHPOS:      rc = 23;
    acase A_GOLFPOS1:
    case  A_GOLFPOS2:
    case  A_GOLFODPOS:       rc = 24;
    acase TENNISPOS:         rc = 25;
    acase GUNDAMPOS:         rc = 26;
    acase HOBOPOS1:
    case  HOBOPOS2:          rc = 27;
    acase A_HORSERACINGPOS:  rc = 28;
    acase JTRON121POS:
    case  JTRON10POS:
    case  JTRON11POS:        rc = 29;
    acase JUMPBUG1POS:
    case  JUMPBUG2POS:       rc = 30;
    acase JUNGLERPOS:        rc = 31;
    acase MACROSSPOS:        rc = 32;
    acase MISSILEWARPOS:     rc = 33;
    acase MONACOPOS:         rc = 34;
    acase NIBBLEMENPOS:      rc = 35;
    acase OCEANBATTLEPOS:    rc = 36;
    acase PARASHOOTERPOS:    rc = 37;
    acase PLEIADESPOS:       rc = 38;
    acase R2DTANKPOS:        rc = 39;
    acase REDCLASHPOS:
    case  REDCLASHODPOS:     rc = 40;
    acase ROBOTKILLERPOS:    rc = 41;
    acase ROUTE16POS:        rc = 42;
    acase _2DSOCCERPOS:
    case  _2DSOCCERODPOS:    rc = 43;
    acase SPACEATTACKAPOS:
    case  SPACEATTACKBPOS:
    case  SPACEATTACKCPOS:   rc = 44;
    acase SPACEMISSIONPOS:   rc = 45;
    acase SPACERAIDERSPOS:   rc = 46;
    acase SPACESQUADRON1POS:
    case  SPACESQUADRON2POS: rc = 47;
    acase SPACEVULTURESPOS:  rc = 48;
    acase SPIDERSPOS:
    case  SPIDERSODPOS:      rc = 49;
    acase STARCHESSPOS:
    case  STARCHESSENHPOS:   rc = 50;
    acase SUPERBUG1POS:      rc = 51;
    acase SUPERGOBBLERPOS:   rc = 52;
    acase TANKSALOTPOS:      rc = 53;
    acase TETRISPOS:         rc = 54;
    acase THEENDPOS1:
    case  THEENDPOS2:        rc = 55;
    acase TURTLESPOS:        rc = 56;
    // Interton
    acase CARRACESPOS:       rc = 56 +  1;
    acase I_BLACKJACKPOS:    rc = 56 +  2;
    acase PADDLEGAMESPOS:    rc = 56 +  3;
    acase I_COMBATBPOS:      rc = 56 +  4;
    acase I_MATH1POS:
    case  231:               rc = 56 +  5;
    acase I_MATH2POS:        rc = 56 +  6;
    acase AIRSEABATTLEPOS:
    case  225:               rc = 56 +  7;
    acase MEMORY1POS:        rc = 56 +  8;
    acase INTELLIGENCE1POS:
    case  230:               rc = 56 +  9;
    acase WINTERSPORTSPOS:   rc = 56 + 10;
    acase HIPPODROMEPOS:
    case  229:               rc = 56 + 11;
    acase HUNTINGPOS:        rc = 56 + 12;
    acase CHESS1POS:         rc = 56 + 13;
    acase MOTOCROSSPOS:      rc = 56 + 14;
    acase _4INAROWPOS:
    case  227:               rc = 56 + 15;
    acase MASTERMINDPOS:     rc = 56 + 16;
    acase I_CIRCUSPOS:       rc = 56 + 17;
    acase I_BOXINGPOS:
    case  226:               rc = 56 + 18;
    acase SPACEWARPOS:       rc = 56 + 19;
    acase MUSICALGAMESPOS:   rc = 56 + 20;
    acase I_CAPTUREPOS:
    case  233:               rc = 56 + 21;
    acase CHESS2POS:         rc = 56 + 22;
    acase PINBALLBPOS:       rc = 56 + 23;
    acase SOCCERBPOS:        rc = 56 + 24;
    acase I_BOWLINGPOS:      rc = 56 + 25;
    acase DRAUGHTSPOS:       rc = 56 + 26;
    acase I_GOLFPOS:
    case  228:               rc = 56 + 27;
    acase COCKPITPOS:        rc = 56 + 28;
    acase METROPOLISPOS:     rc = 56 + 29;
    acase SOLITAIREPOS:      rc = 56 + 30;
    acase CASINOPOS:         rc = 56 + 31;
    acase INVADERBPOS:       rc = 56 + 32;
    acase SUPERINVPOS:       rc = 56 + 33;
    acase BACKGAMMONPOS:     rc = 56 + 34;
    acase MONSTERMANPOS:
    case  232:               rc = 56 + 35;
    acase HYPERSPACEPOS:     rc = 56 + 36;
    acase SUPERSPACEPOS:     rc = 56 + 37;
    // all
    adefault:                rc = -1;
    } // Mothership and Super Bug 2 could arguably use the box of their related game

    return (jint) rc;
}
#endif /* !LIBRETRO */

EXPORT void command_changemachine(int whichmachine, int whichmemmap)
{   if (memmap != whichmemmap)
    {   changemachine(whichmachine, whichmemmap, TRUE, FALSE, FALSE);
}   }

MODULE void do_autocoin(void)
{   FAST UBYTE tens, ones;
    FAST int   credits = 0; // initialized to avoid spurious SAS/C optimizer warnings

    // assert(machines[machine].coinop);

    if (coinignore)
    {   coinignore--;
    } elif (console_b == 0)
    {   switch (memmap)
        {
        case MEMMAP_ASTROWARS:
            credits = memory[0x150E];
        acase MEMMAP_GALAXIA:
            credits = memory[0x1D22];
        acase MEMMAP_LASERBATTLE:
            credits = memory[0x1C7D];
        acase MEMMAP_LAZARIAN:
            credits = memory[0x1F00];
        acase MEMMAP_MALZAK1:
        case MEMMAP_MALZAK2:
            tens    = memory[0x101C] ? (memory[0x101C] - 0x30) : 0;
            ones    = memory[0x101D] ? (memory[0x101D] - 0x30) : 0;
            credits = (tens * 10) + ones;
        }

        if (credits >= 0 && credits < 2)
        {   console_b = 5;
            if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
            {   coinignore = 100;
            } else // MALZAK, LASERBATTLE, LAZARIAN
            {   coinignore =  10;
}   }   }   }

EXPORT void play_sample(int sample)
{   chan_status[GUESTCHANNELS + sample] = CHAN_PLAY;
}
EXPORT void sound_stop(int sample)
{   if (sample >= GUESTCHANNELS)
    {   chan_status[sample] = CHAN_STOP;
}   }

/* JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_MainActivity_cloadnvram(JNIEnv* env, jobject this, jbyteArray ptr)
{   jbyte* IOBuffer2 = (*env)->GetByteArrayElements(env, ptr, 0);
    memcpy(IOBuffer, IOBuffer2, 257);
    (*env)->ReleaseByteArrayElements(env, ptr, IOBuffer2, JNI_ABORT);
    
    memcpy(&memory[0x1700], IOBuffer, 256);
    memory[0x14CC] = IOBuffer[256];
}
JNIEXPORT jbyteArray JNICALL Java_com_amigan_droidarcadia_MainActivity_csavenvram(JNIEnv* env, jobject this)
{   memcpy(IOBuffer, &memory[0x1700], 256);
    IOBuffer[256] = memory[0x14CC];

    jintArray rc = (*env)->NewByteArray(env, 257);
    (*env)->SetByteArrayRegion(env, rc, 0, 257, (const jbyte*) IOBuffer);

    return rc;
} */

#ifndef LIBRETRO
JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_DIPsActivity_cgetdips(JNIEnv* env, jobject this)
{   int rc;

    switch (memmap)
    {
    case  MEMMAP_ASTROWARS:   rc = ((aw_dips1 << 8) | aw_dips2);
    acase MEMMAP_GALAXIA:     rc =                    ga_dips  ;
    acase MEMMAP_LASERBATTLE: rc =                    lb_dips  ;
    acase MEMMAP_LAZARIAN:    rc = ((lz_dips2 << 8) | lz_dips3);
    adefault:                 rc = 0; // eg. MEMMAP_MALZAK1, MEMMAP_MALZAK2
    }
    if (autocoin) rc |= 0x10000;

    return (jint) rc;
}
    
JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_DIPsActivity_csetdips(JNIEnv* env, jobject this, jint thedips)
{   int cthedips = (int) thedips;

    switch (memmap)
    {
    case  MEMMAP_ASTROWARS:   aw_dips1 = (cthedips & 0x0FF00) >> 8;
                              aw_dips2 =  cthedips & 0x000FF      ;
    acase MEMMAP_GALAXIA:     ga_dips  =  cthedips & 0x000FF      ;
    acase MEMMAP_LASERBATTLE: lb_dips  =  cthedips & 0x000FF      ;
    acase MEMMAP_LAZARIAN:    lz_dips2 = (cthedips & 0x0FF00) >> 8;
                              lz_dips3 =  cthedips & 0x000FF      ;
    }
    
    autocoin = (cthedips & 0x10000) ? TRUE : FALSE;
}

// RETROACHIEVEMENTS SUPPORT----------------------------------------------

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{   g_vm = vm; // Store the JavaVM pointer for later use

    return JNI_VERSION_1_6; // Return JNI version
}

MODULE uint32_t readmemory(uint32_t address, uint8_t* buffer, uint32_t num_bytes, rc_client_t* client)
{   // RetroAchievements addresses start at $00000000, which normally represents the first physical byte
    // of memory for the system. Normally, an emulator stores its RAM in a singular contiguous buffer,
    // which makes reading from a RetroAchievements address simple:

    // if (address + num_bytes >= RAM_size)
    //     return 0; 
    // memcpy(buffer, &RAM[address], num_bytes);
    // return num_bytes;

    // Sometimes an emulator only exposes a virtual BUS. In that case, it may be necessary to translate
    // the RetroAchievements address to a real address.

    if (address + num_bytes >= 0x8000)
    {   return 0;
    }
    
    switch (machine)
    {
    case ARCADIA:
        memcpy(buffer, &memory[0x1800 + address], num_bytes); // $1800..$1AFF ( 768 bytes)
    acase INTERTON:
        if (address < 1024)
        {   memcpy(buffer, &memory[0x1800 + address       ], num_bytes); // $1800..$1BFF (1024 bytes)
        } else
        {   memcpy(buffer, &memory[0x1E00 + address - 1024], num_bytes); // $1E00..$1FFF ( 512 bytes)
        }            
    acase ELEKTOR:
        memcpy(buffer, &memory[ 0x800 + address], num_bytes); // $0800..$1FFF (6144 bytes)
    }
    
    return num_bytes;
}

MODULE void logincallback(int result, const char* error_message, rc_client_t* client, void* userdata)
{   // If not successful, just report the error and bail.
    if (result != RC_OK)
    {   sprintf(cmessage, "Login failed: %s!", error_message); // eg. invalid JSON
        racolour = RA_RED;
        return;
    }

    // Login was successful. Capture the token for future logins so we don't have to store the password anywhere.
    const rc_client_user_t* user = rc_client_get_user_info(client);
    // store_retroachievements_credentials(user->username, user->token);

    if (user->score == 1)
    {   sprintf(cmessage, "Logged in as %s (1 point).", user->display_name);
    } else
    {   sprintf(cmessage, "Logged in as %s (%u points).", user->display_name, user->score);
    }
    racolour = RA_BLUE;
}

MODULE void loadgamecallback(int result, const char* error_message, rc_client_t* client, void* userdata)
{   if (result != RC_OK)
    {   sprintf(cmessage, "Game load failed: %s!", error_message);
        racolour = RA_RED;
        return;
    }

    show_game_placard();
}

MODULE void show_game_placard(void)
{   char                          message[128];
    rc_client_user_game_summary_t summary;

    ragame = rc_client_get_game_info(g_client);
    rc_client_get_user_game_summary(g_client, &summary);

    // Construct a message indicating the number of achievements unlocked by the user.
    if (summary.num_core_achievements == 0)
    {   sprintf(cmessage, "%s has no achievements.", ragame->title);
        racolour = RA_YELLOW;
    } elif (summary.num_unsupported_achievements)
    {   sprintf(cmessage, "%s: %u of %u achievements unlocked (%d unsupported).",
            ragame->title,
            summary.num_unlocked_achievements,
            summary.num_core_achievements,
            summary.num_unsupported_achievements);
        racolour = RA_YELLOW;
    } else
    {   sprintf(cmessage, "%s: %u of %u achievements unlocked.",
            ragame->title,
            summary.num_unlocked_achievements,
            summary.num_core_achievements);
        racolour = RA_BLUE;
    }
    
    // The emulator is responsible for managing images. This uses rc_client to build
    // the URL where the image should be downloaded from.
    rc_client_game_get_image_url(ragame, gamegfxurl, sizeof(gamegfxurl));

#ifdef TESTINDICATORS
    strcpy(progressurl, gamegfxurl);
    strcpy(progressstr, "Progress");
    strcpy(trackerstr, "Tracker");
    progressing = tracking = TRUE;
#endif
}

MODULE void servercall
(   const rc_api_request_t*     request,
    rc_client_server_callback_t callback,
    void*                       callback_data,
    rc_client_t*                client
)
{   JNIEnv*   env;
    jclass    clazz;
    jmethodID methodId;
    int       res;

    async_callback_data* async_data = malloc(sizeof(async_callback_data));
    async_data->callback            = callback;
    async_data->callback_data       = callback_data;

    res = (*g_vm)->AttachCurrentThread(g_vm, &env, NULL);
    if (res != JNI_OK)
    {   return;
    }
    clazz = (*env)->FindClass(env, "com/amigan/droidarcadia/NetworkUtil");
    if (clazz == NULL)
    {   return;
    }
    methodId = (*env)->GetStaticMethodID(env, clazz, "asynchttppost", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;J)V");
    if (methodId == NULL)
    {   return;
    }

    jstring jurl       = (*env)->NewStringUTF(env, request->url);
    jstring jpostdata  = (*env)->NewStringUTF(env, request->post_data);
    jstring juseragent = (*env)->NewStringUTF(env, "DroidArcadia/" INTEGERVERSION);

    (*env)->CallStaticVoidMethod(env, clazz, methodId, jurl, jpostdata, juseragent, (jlong) async_data); // main code of this function will complete, but not the asynchronous task, before continuing

    (*env)->DeleteLocalRef(env, jurl);
    (*env)->DeleteLocalRef(env, jpostdata);
    (*env)->DeleteLocalRef(env, juseragent);
}

MODULE void rainit(void)
{   // assert(!machines[machine].coinop);
        
    sprintf(cmessage, "Logging in as %s...", username);
    racolour = RA_ORANGE;
    g_client = rc_client_create(readmemory, servercall);
    rc_client_set_event_handler(g_client, event_handler);
    rc_client_set_hardcore_enabled(g_client, hardcore ? 1 : 0);
    DISCARD rc_client_begin_login_with_password(g_client, username, password, logincallback, NULL);
    switch (machine)
    {
    case  ARCADIA:  ramachine = RC_CONSOLE_ARCADIA_2001;              // 73
    acase INTERTON: ramachine = RC_CONSOLE_INTERTON_VC_4000;          // 74
    acase ELEKTOR:  ramachine = RC_CONSOLE_ELEKTOR_TV_GAMES_COMPUTER; // 75
    }
    identifyragame();
    hardcore = rc_client_get_hardcore_enabled(g_client); // in case eg. RetroAchievements has put us into softcore mode
}

JNIEXPORT jstring JNICALL Java_com_amigan_droidarcadia_MainActivity_getmessage(JNIEnv *env, jobject this)
{   jstring rc;

    rc = (*env)->NewStringUTF(env, cmessage);
    cmessage[0] = EOS;
    return rc;
}

JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_MainActivity_getracolour(JNIEnv *env, jobject this)
{   return (jint) racolour;
}

JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_NetworkUtil_chttpcallback(JNIEnv* env, jclass this, jint statuscode, jstring content, jlong userdata, jstring errormessage)
{   char *ccontent      = NULL,
         *cerrormessage = NULL;
    const char* ccontentData      = (*env)->GetStringUTFChars(env, content     , 0);
    const char* cerrormessageData = (*env)->GetStringUTFChars(env, errormessage, 0);

    if (ccontentData != NULL)
    {   ccontent = (char*) malloc(strlen(ccontentData) + 1); // +1 for null-terminator
        strcpy(ccontent, ccontentData);
        (*env)->ReleaseStringUTFChars(env, content, ccontentData);
    }
    if (cerrormessageData != NULL)
    {   cerrormessage = (char*) malloc(strlen(cerrormessageData) + 1); // +1 for null-terminator
        strcpy(cerrormessage, cerrormessageData);
        (*env)->ReleaseStringUTFChars(env, errormessage, cerrormessageData);
    }

    httpcallback(statuscode, ccontent, ccontent ? strlen(ccontent) : 0, (void*) userdata, cerrormessage);

    free(ccontent);
    free(cerrormessage);
}

// This is the callback function for the asynchronous HTTP call
MODULE void httpcallback(int status_code, const char* content, size_t content_size, void* userdata, const char* error_message)
{ // Prepare a data object to pass the HTTP response to the callback
  rc_api_server_response_t server_response;
  memset(&server_response, 0, sizeof(server_response));
  server_response.body = content;
  server_response.body_length = content_size;
  server_response.http_status_code = status_code;

  // handle non-http errors (socket timeout, no internet available, etc)
  if (status_code == 0 && error_message && error_message[0]) {
      // assume no server content and pass the error through instead
      server_response.body = error_message;
      server_response.body_length = strlen(error_message);
      // Let rc_client know the error was not catastrophic and could be retried. It may decide to retry or just
      // immediately pass the error to the callback. To prevent possible retries, use RC_API_SERVER_RESPONSE_CLIENT_ERROR.
      server_response.http_status_code = RC_API_SERVER_RESPONSE_RETRYABLE_CLIENT_ERROR;
  }

  // Get the rc_client callback and call it
  async_callback_data* async_data = (async_callback_data*)userdata;
  async_data->callback(&server_response, async_data->callback_data);

  // Release the captured rc_client callback data
  free(async_data);
}

JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_MainActivity_jshutdownraclient(JNIEnv* env, jobject this)
{   cshutdownraclient();
}

MODULE void cshutdownraclient(void)
{   if (g_client)
    {   // Release resources associated to the client instance
        rc_client_destroy(g_client);
        g_client = NULL;
}   }

MODULE void achievement_triggered(const rc_client_achievement_t* achievement)
{   // The runtime already took care of dispatching the server request to notify the
    // server; we just have to tell the player.

    if (rc_client_achievement_get_image_url(achievement, RC_CLIENT_ACHIEVEMENT_STATE_UNLOCKED, badgegfxurl, sizeof(badgegfxurl)) == RC_OK)
    {   sprintf(badgegfxurl, "ach_%s.png", achievement->badge_name);
        badgemode = TRUE;
    }

    if (achievement->category == RC_CLIENT_ACHIEVEMENT_CATEGORY_UNOFFICIAL)
    {   sprintf(cmessage, "Unofficial achievement unlocked: %s!", achievement->title);
        racolour = RA_GREEN;
        play_sample(SAMPLE_CHEEVOS);
    } elif (!strcmp(achievement->title, "Warning: Unknown Emulator"))
    {   sprintf(cmessage, "%s!", achievement->title);
        racolour = RA_RED;
    } else
    {   sprintf(cmessage, "Achievement unlocked: %s!", achievement->title);
        racolour = RA_GREEN;
        play_sample(SAMPLE_CHEEVOS);
}   }

MODULE void event_handler(const rc_client_event_t* event, rc_client_t* client)
{   switch (event->type)
    {
    case RC_CLIENT_EVENT_ACHIEVEMENT_TRIGGERED: // 1
        achievement_triggered(event->achievement);
    acase RC_CLIENT_EVENT_LEADERBOARD_STARTED: // 2
        sprintf(cmessage, "Leaderboard attempt started: %s - %s", event->leaderboard->title, event->leaderboard->description);
        racolour = RA_ORANGE;
    acase RC_CLIENT_EVENT_LEADERBOARD_FAILED: // 3
        sprintf(cmessage, "Leaderboard attempt failed: %s", event->leaderboard->title);
        racolour = RA_RED;
    acase RC_CLIENT_EVENT_LEADERBOARD_SUBMITTED: // 4
        sprintf(cmessage, "Submitted %s for %s!", event->leaderboard->tracker_value, event->leaderboard->title);
        racolour = RA_GREEN;
        // play_sample(SAMPLE_CHEEVOS); needed or not?
    acase RC_CLIENT_EVENT_ACHIEVEMENT_CHALLENGE_INDICATOR_SHOW: // 5
        ;
    acase RC_CLIENT_EVENT_ACHIEVEMENT_CHALLENGE_INDICATOR_HIDE: // 6
        ;
    acase RC_CLIENT_EVENT_ACHIEVEMENT_PROGRESS_INDICATOR_SHOW: // 7
        progressing = TRUE;
        strcpy(progressstr, event->achievement->measured_progress);
    acase RC_CLIENT_EVENT_ACHIEVEMENT_PROGRESS_INDICATOR_HIDE: // 8
        progressing = FALSE;
    acase RC_CLIENT_EVENT_ACHIEVEMENT_PROGRESS_INDICATOR_UPDATE: // 9
        strcpy(progressstr, event->achievement->measured_progress);
        rc_client_achievement_get_image_url(event->achievement, RC_CLIENT_ACHIEVEMENT_STATE_ACTIVE, progressurl, sizeof(progressurl));
    acase RC_CLIENT_EVENT_LEADERBOARD_TRACKER_SHOW: // 10
        tracking = TRUE;
        strcpy(trackerstr, event->leaderboard_tracker->display);
    acase RC_CLIENT_EVENT_LEADERBOARD_TRACKER_HIDE: // 11
        tracking = FALSE;
    acase RC_CLIENT_EVENT_LEADERBOARD_TRACKER_UPDATE: // 12
        strcpy(trackerstr, event->leaderboard_tracker->display);
    acase RC_CLIENT_EVENT_LEADERBOARD_SCOREBOARD: // 13
        strcpy(cmessage, "Leaderboard entry submitted!");
        racolour = RA_GREEN;
        play_sample(SAMPLE_CHEEVOS);
    acase RC_CLIENT_EVENT_RESET: // 14
        engine_reset();
    acase RC_CLIENT_EVENT_GAME_COMPLETED: // 15
        sprintf(cmessage, "%s %s!", hardcore ? "Mastered" : "Completed", ragame->title);
        racolour = RA_GREEN;
        // play_sample(SAMPLE_CHEEVOS); is probably not needed (earning final achievement will play this sample anyway)
    acase RC_CLIENT_EVENT_SERVER_ERROR: // 16
        sprintf(cmessage, "%s: %s!", event->server_error->api, event->server_error->error_message);
        racolour = RA_RED;
    acase RC_CLIENT_EVENT_DISCONNECTED: // 17
        strcpy(cmessage, "Disconnected!");
        racolour = RA_RED;
    acase RC_CLIENT_EVENT_RECONNECTED: // 18
        strcpy(cmessage, "Reconnected.");
        racolour = RA_BLUE;
    adefault: // should never happen
        sprintf(cmessage, "Unhandled event %d!", event->type);
        racolour = RA_RED;
}   }

JNIEXPORT jboolean JNICALL Java_com_amigan_droidarcadia_MainActivity_jsetravars(JNIEnv* env, jobject this, jstring jusername, jstring jpassword, jboolean jcheevos, jboolean jhardcore)
{   csetravars(env, jusername, jpassword, jcheevos, jhardcore);
    if (hardcore)
    {   return (jboolean) JNI_TRUE;
    } else
    {   return (jboolean) JNI_FALSE;
}   }
JNIEXPORT jboolean JNICALL Java_com_amigan_droidarcadia_OptionsActivity_jsetravars(JNIEnv* env, jobject this, jstring jusername, jstring jpassword, jboolean jcheevos, jboolean jhardcore)
{   csetravars(env, jusername, jpassword, jcheevos, jhardcore);
    if (hardcore)
    {   return (jboolean) JNI_TRUE;
    } else
    {   return (jboolean) JNI_FALSE;
}   }
JNIEXPORT jboolean JNICALL Java_com_amigan_droidarcadia_RAActivity_jsetravars(JNIEnv* env, jobject this, jstring jusername, jstring jpassword, jboolean jcheevos, jboolean jhardcore)
{   csetravars(env, jusername, jpassword, jcheevos, jhardcore);
    if (hardcore)
    {   return (jboolean) JNI_TRUE;
    } else
    {   return (jboolean) JNI_FALSE;
}   }

MODULE void csetravars(JNIEnv* env, jstring jusername, jstring jpassword, jboolean jcheevos, jboolean jhardcore)
{   FLAG        oldcheevos,
                oldhardcore;
    const char* utfChars;

    utfChars = (*env)->GetStringUTFChars(env, jusername, 0);
    if (utfChars)
    {   strncpy(username, utfChars, sizeof(username) - 1);
        username[sizeof(username) - 1] = EOS;
    } else
    {   username[0] = EOS;
    }
    (*env)->ReleaseStringUTFChars(env, jusername, utfChars);

    utfChars = (*env)->GetStringUTFChars(env, jpassword, 0);
    if (utfChars)
    {   strncpy(password, utfChars, sizeof(password) - 1);
        username[sizeof(password) - 1] = EOS;
    } else
    {   password[0] = EOS;
    }
    (*env)->ReleaseStringUTFChars(env, jpassword, utfChars);

    oldcheevos = cheevos;
    cheevos = (jcheevos == JNI_TRUE) ? 1 : 0;
    oldhardcore = hardcore;
    hardcore = (jhardcore == JNI_TRUE) ? 1 : 0;
    if (cheevos && !oldcheevos)
    {   if (!machines[machine].coinop)
        {   rainit();
    }   }
    elif (!cheevos && oldcheevos)
    {   cshutdownraclient();
    } elif (hardcore != oldhardcore)
    {   rc_client_set_hardcore_enabled(g_client, hardcore ? 1 : 0);
        hardcore = rc_client_get_hardcore_enabled(g_client); // in case eg. RetroAchievements has put us into softcore mode
}   }

JNIEXPORT jstring JNICALL Java_com_amigan_droidarcadia_MainActivity_getbadgegfxurl(JNIEnv *env, jobject this)
{   jstring rc;

    rc = (*env)->NewStringUTF(env, badgegfxurl);
    return rc;
}

JNIEXPORT jstring JNICALL Java_com_amigan_droidarcadia_MainActivity_getprogressurl(JNIEnv *env, jobject this)
{   jstring rc;

    rc = (*env)->NewStringUTF(env, progressurl);
    progressurl[0] = EOS;
    return rc;
}

JNIEXPORT jstring JNICALL Java_com_amigan_droidarcadia_MainActivity_getgamegfxurl(JNIEnv *env, jobject this)
{   jstring rc;

    rc = (*env)->NewStringUTF(env, gamegfxurl);
    return rc;
}
JNIEXPORT jstring JNICALL Java_com_amigan_droidarcadia_RAActivity_getgamegfxurl(JNIEnv *env, jobject this)
{   jstring rc;

    rc = (*env)->NewStringUTF(env, gamegfxurl);
    return rc;
}

JNIEXPORT jint JNICALL Java_com_amigan_droidarcadia_RAActivity_getgameid(JNIEnv *env, jobject this)
{   if (ragame)
    {   return ragame->id;
    } else
    {   return 0;
}   }

JNIEXPORT jstring JNICALL Java_com_amigan_droidarcadia_MainActivity_getprogressstr(JNIEnv *env, jobject this)
{   jstring rc;

    rc = (*env)->NewStringUTF(env, progressstr);
    return rc;
}

JNIEXPORT jstring JNICALL Java_com_amigan_droidarcadia_MainActivity_gettrackerstr(JNIEnv *env, jobject this)
{   jstring rc;

    rc = (*env)->NewStringUTF(env, trackerstr);
    return rc;
}

JNIEXPORT jboolean JNICALL Java_com_amigan_droidarcadia_MainActivity_isprogressing(JNIEnv *env, jobject this)
{   return (jboolean) progressing;
}
JNIEXPORT jboolean JNICALL Java_com_amigan_droidarcadia_MainActivity_istracking(JNIEnv *env, jobject this)
{   return (jboolean) tracking;
}

JNIEXPORT jboolean JNICALL Java_com_amigan_droidarcadia_MainActivity_canpause(JNIEnv* env, jobject this)
{   int rc = (!cheevos || !hardcore || rc_client_can_pause(g_client, NULL)) ? JNI_TRUE : JNI_FALSE;

    return (jboolean) rc;
}

JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_MainActivity_clientidle(JNIEnv* env, jobject this)
{   // assert(cheevos);

    if (g_client)
    {   rc_client_idle(g_client);
}   }

JNIEXPORT void JNICALL Java_com_amigan_droidarcadia_RAActivity_showachievements(JNIEnv* env, jobject this)
{         char  cheevourl[128];
    const char* progress;

    jclass clazz = (*env)->FindClass(env, "com/amigan/droidarcadia/RAActivity");
    if (clazz == NULL)
    {   return;
    }
    jmethodID methodId = (*env)->GetStaticMethodID(env, clazz, "addcheevo", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    if (methodId == NULL)
    {   return;
    }

    // This will return a list of lists. Each top-level item is an achievement category
    // (Active Challenge, Unlocked, etc). Empty categories are not returned, so we can
    // just display everything that is returned.
    rc_client_achievement_list_t* list = rc_client_create_achievement_list
    (   g_client,
        RC_CLIENT_ACHIEVEMENT_CATEGORY_CORE_AND_UNOFFICIAL,
        RC_CLIENT_ACHIEVEMENT_LIST_GROUPING_PROGRESS
    );

    for (int i = 0; i < list->num_buckets; i++)
    {   // Create a header item for the achievement category
        jstring jurl   = (*env)->NewStringUTF(env, "");
        jstring jtitle = (*env)->NewStringUTF(env, list->buckets[i].label);
        jstring jdesc  = (*env)->NewStringUTF(env, "");
        jstring jprog  = (*env)->NewStringUTF(env, "");
        (*env)->CallStaticVoidMethod(env, clazz, methodId, jurl, jtitle, jdesc, jprog);
        (*env)->DeleteLocalRef(env, jurl);
        (*env)->DeleteLocalRef(env, jtitle);
        (*env)->DeleteLocalRef(env, jdesc);
        (*env)->DeleteLocalRef(env, jprog);

        for (int j = 0; j < list->buckets[i].num_achievements; j++)
        {   const rc_client_achievement_t* achievement = list->buckets[i].achievements[j];

            rc_client_achievement_get_image_url(achievement, achievement->state, cheevourl, sizeof(cheevourl));

            // Determine the "progress" of the achievement. This can also be used to show
            // locked/unlocked icons and progress bars.
            if (list->buckets[i].subset_id == RC_CLIENT_ACHIEVEMENT_BUCKET_UNSUPPORTED)
                progress = "Unsupported";
            else if (achievement->unlocked)
                progress = "Unlocked";
            else if (achievement->measured_percent)
                progress = achievement->measured_progress;
            else
                progress = "Locked";

            jstring jurl   = (*env)->NewStringUTF(env, cheevourl);
            jstring jtitle = (*env)->NewStringUTF(env, achievement->title);
            jstring jdesc  = (*env)->NewStringUTF(env, achievement->description);
            jstring jprog  = (*env)->NewStringUTF(env, progress);
            (*env)->CallStaticVoidMethod(env, clazz, methodId, jurl, jtitle, jdesc, jprog);
            (*env)->DeleteLocalRef(env, jurl);
            (*env)->DeleteLocalRef(env, jtitle);
            (*env)->DeleteLocalRef(env, jdesc);
            (*env)->DeleteLocalRef(env, jprog);
    }   }

    rc_client_destroy_achievement_list(list);
}

JNIEXPORT jboolean JNICALL Java_com_amigan_droidarcadia_MainActivity_isbadgegfx(JNIEnv* env, jobject this)
{   if (badgemode)
    {   badgemode = FALSE;
        return (jboolean) TRUE;
    } // implied else
    return (jboolean) FALSE;
}
#endif /* !LIBRETRO */

EXPORT void set_cpl(int newcpl)
{   // assert(newcpl <= 227);

    cpl = newcpl;

    switch (cpl % 4)
    {
    case  0: frac[0] = frac[1] = frac[2] = frac[3] =  cpl / 4;
    acase 1: frac[0] = frac[1]           = frac[3] =  cpl / 4;
                                 frac[2]           = (cpl / 4) + 1;
    acase 2: frac[0]           = frac[2]           =  cpl / 4;
                       frac[1]           = frac[3] = (cpl / 4) + 1;
    acase 3: frac[0]                               =  cpl / 4;
                       frac[1] = frac[2] = frac[3] = (cpl / 4) + 1;
}   }

MODULE void identifyragame(void)
{   if (machine == ELEKTOR)
    {   rc_client_begin_identify_and_load_game
        (   g_client,
            ramachine,
            NULL,
            cheevomem,
            cheevosize,
            loadgamecallback,
            NULL
        );
    } elif (machine == INTERTON || cheevosize <= 4096)
    {   rc_client_begin_identify_and_load_game
        (   g_client,
            ramachine,
            NULL,
            memory,
            cheevosize,
            loadgamecallback,
            NULL
        );
    } else
    {   // assert(machine == ARCADIA);
        memcpy(         &cheevomem[   0], &memory[          0],               4096);
        if (cheevosize <= 8192)
        {   memcpy(     &cheevomem[4096], &memory[     0x2000],  cheevosize - 4096);
        } else
        {   memcpy(     &cheevomem[4096], &memory[     0x2000],               4096);
            memcpy(     &cheevomem[8192], &memory[     0x4000], (cheevosize - 8192) <= 4096 ? (cheevosize - 8192) : 4096);
        }
        rc_client_begin_identify_and_load_game
        (   g_client,
            ramachine,
            NULL,
            cheevomem,
            cheevosize,
            loadgamecallback,
            NULL
        );
}   }
