// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
    #include <stdlib.h> // eg. for rand()
#endif
#ifdef WIN32
    #include "ibm.h"
    #include "resource.h"
#endif

#include <stdio.h> // for FILE*

#include "aa.h"
#include "zaccaria.h"

// DEFINES----------------------------------------------------------------

#define CVS_MAX_STARS 250
#define PLANETRADIUS   50

#define LETBITS654BE100(a) a &= 0x8F; a |= 0x40; // %x100xxxx
#define LETBITS654BE011(a) a &= 0x8F; a |= 0x30; // %x011xxxx
#define LETBITS654BE010(a) a &= 0x8F; a |= 0x20; // %x010xxxx
#define LETBITS654BE001(a) a &= 0x8F; a |= 0x10; // %x001xxxx
#define LETBITS654BE000(a) a &= 0x8F;            // %x000xxxx

#define LETBITS65BE11(a)              a |= 0x60; // %x11xxxxx
#define LETBITS65BE10(a)   a &= 0x9F; a |= 0x40; // %x10xxxxx
#define LETBITS65BE01(a)   a &= 0x9F; a |= 0x20; // %x01xxxxx
#define LETBITS65BE00(a)   a &= 0x9F;            // %x00xxxxx

#define LETBITS54BE11(a)              a |= 0x30; // %xx11xxxx
#define LETBITS54BE10(a)   a &= 0xCF; a |= 0x20; // %xx10xxxx
#define LETBITS54BE01(a)   a &= 0xCF; a |= 0x10; // %xx01xxxx
#define LETBITS54BE00(a)   a &= 0xCF;            // %xx00xxxx

#define LETBITS43BE11(a)              a |= 0x18; // %xxx11xxx
#define LETBITS43BE10(a)   a &= 0xE7; a |= 0x10; // %xxx10xxx
#define LETBITS43BE01(a)   a &= 0xE7; a |= 0x08; // %xxx01xxx
#define LETBITS43BE00(a)   a &= 0xE7;            // %xxx00xxx

#define LETBITS32BE11(a)              a |= 0x0C; // %xxxx11xx
#define LETBITS32BE10(a)   a &= 0xF3; a |= 0x08; // %xxxx10xx
#define LETBITS32BE01(a)   a &= 0xF3; a |= 0x04; // %xxxx01xx
#define LETBITS32BE00(a)   a &= 0xF3;            // %xxxx00xx

#define LETBITS10BE11(a)              a |= 0x03; // %xxxxxx11
#define LETBITS10BE10(a)   a &= 0xFC; a |= 0x02; // %xxxxxx10
#define LETBITS10BE01(a)   a &= 0xFC; a |= 0x01; // %xxxxxx01
#define LETBITS10BE00(a)   a &= 0xFC;            // %xxxxxx00

#define LETBIT7BE1(a)                 a |= 0x80; // %1xxxxxxx
#define LETBIT7BE0(a)      a &= 0x7F;            // %0xxxxxxx

#define LETBIT6BE1(a)                 a |= 0x40; // %x1xxxxxx
#define LETBIT6BE0(a)      a &= 0xBF;            // %x0xxxxxx

#define LETBIT3BE1(a)                 a |= 0x08; // %xxxx1xxx
#define LETBIT3BE0(a)      a &= 0xF7;            // %xxxx0xxx

#define LETBIT2BE1(a)                 a |= 0x04; // %xxxxx1xx
#define LETBIT2BE0(a)      a &= 0xFB;            // %xxxxx0xx

#define LETBIT1BE1(a)                 a |= 0x02; // %xxxxxx1x
#define LETBIT1BE0(a)      a &= 0xFD;            // %xxxxxx0x

#define LETBIT0BE1(a)                 a |= 0x01; // %xxxxxxx1
#define LETBIT0BE0(a)      a &= 0xFE;            // %xxxxxxx0

#define LETBITS210BE000(a) a &= 0xF8;            // %xxxxx000
#define LETBITS210BE001(a) a &= 0xF8; a |= 0x01; // %xxxxx001
#define LETBITS210BE011(a) a &= 0xF8; a |= 0x03; // %xxxxx011
#define LETBITS210BE101(a) a &= 0xF8; a |= 0x05; // %xxxxx101
#define LETBITS210BE111(a) a &= 0xF8; a |= 0x07; // %xxxxx111

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT SBYTE                galaxia_scrolly;
EXPORT UBYTE                abeff1, abeff2,
                            awga_collide,
                            g_bank1[1024],
                            g_bank2[16],
                            lb_bank,
                            lb_spritecode,
                            lb_spritecolour,
                            lb_spriteenable,
                            lb_spritex,
                            lb_spritey,
                            memory_effects[512],
                            mux,
                            neg1, neg2,
                            lb_tone,
                            aw_dips1    = 0x8F,
                            aw_dips2    =    0,
                            ga_dips     = 0x8F,
                            lb_dips     = 0xBF, // ie. %1,0,11,11,11 (real factory default is %1,0,01,00,00)
                            lz_dips2    = 0xBF,
                            lz_dips3    = 0xF2,
                            squeal,
                            useshell;
EXPORT ULONG                lb_snd;
EXPORT int                  starscroll;
#ifdef AMIGA
    EXPORT int              throb;
#endif

EXPORT const UBYTE galaxia_colours[4][4] = {
{ BLACK,
  BLUE,   // 3rd line of status area (exterior)
  PURPLE, // 3rd line of status area (interior)
  PURPLE, // 3rd line of status area (sides)
},
{ BLACK,
  CYAN,   // wings of 3rd enemy row
  BLUE,   // bodies of 3rd enemy row
  CYAN,   // 2nd line of status area, eyes of 3rd enemy row (black)
},
{ WHITE,  // unused?
  WHITE,  // unused?
  WHITE,  // unused?
  WHITE,  // unused?
},
{ BLACK,
  GREEN,  // wings of 4th..6th enemy rows, explosions
  RED,    // bodies of 4th..6th enemy rows, explosions
  YELLOW, // 1st line of status area (yellow), eyes of 4th..6th enemy rows (black), explosions
} }, galaxia_colours2[4][4] = {
{ BLACK,
  RED,    // heads of 1st enemy row, wings of 2nd enemy row
  BLUE,   // bodies of 1st..2nd enemy rows
  YELLOW, // wings of 1st enemy row, eyes of 2nd enemy row
},
{ WHITE,  // unused?
  WHITE,  // unused?
  WHITE,  // unused?
  WHITE,  // unused?
},
{ WHITE,  // unused?
  WHITE,  // unused?
  WHITE,  // unused?
  WHITE,  // unused?
},
{ BLACK,
  GREEN,  // wings of 4th..6th enemy rows, explosions
  RED,    // bodies of 4th..6th enemy rows, explosions
  YELLOW, // 1st line of status area (yellow), eyes of 4th..6th enemy rows (black), explosions
} },
astrowars_colours[8] =
{ WHITE,  // black  -> white
  YELLOW, // yellow -> yellow
  PURPLE, // purple -> purple
  RED,    // red    -> red
  CYAN,   // cyan   -> cyan
  GREEN,  // green  -> green
  BLUE,   // blue   -> blue
  BLACK   // white  -> black
};

/* For Galaxia:
 Explosions use the 14th..16th colours.
 So do the 4th..6th enemy rows.
 Explosion colours are probably wrong. */

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT UBYTE                cc,
                            circle,
                            coinops_colltable[3][PVI_RASTLINES][PVI_XSIZE],
                            memory[32768],
                            OutputBuffer[18],
                            sx[2],
                            sy[2];
IMPORT ASCREEN              screen[BOXWIDTH][BOXHEIGHT];
IMPORT UWORD                console[4],
                            pc,
                            mirror_r[32768],
                            mirror_w[32768],
                            sp;
IMPORT ULONG                analog,
                            collisions,
                            jff[2],
                            swapped;
IMPORT int                  ambient,
                            ax[2],
                            ay[4],
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
                            console_start,
                            console_a,
                            console_b,
                            console_reset,
                            cpl,
                            editscreen,
                            hostcontroller[2],
                            game,
                            key1,
                            key2,
                            key3,
                            key4,
                            machine,
                            memmap,
                            offset,
                            p1bgcol[4],
                            p2bgcol[4],
                            p1rumble,
                            p2rumble,
                            p1sprcol[6],
                            p2sprcol[6],
                            pvibase,
                            recmode,
                            region,
                            rotating,
                            scrnaddr,
                            useff[2],
                            whichkeyrect;
IMPORT UBYTE*               IOBuffer;
IMPORT FILE*                MacroHandle;
IMPORT MEMFLAG              memflags[ALLTOKENS];
IMPORT struct HostMachineStruct hostmachines[MACHINES];
IMPORT struct IOPortStruct  ioport[258];
IMPORT struct MachineStruct machines[MACHINES];
IMPORT struct NoteStruct    notes[NOTES + 1];
#ifdef AMIGA
    IMPORT struct Window*   SubWindowPtr[SUBWINDOWS];
#endif
#ifdef WIN32
    IMPORT HWND             SubWindowPtr[SUBWINDOWS];
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE int                  numstars,
                            starx[CVS_MAX_STARS],
                            stary[CVS_MAX_STARS];
MODULE const UBYTE*         tilesptr;

/* MODULE ARRAYS----------------------------------------------------------

(None)

MODULE FUNCTIONS------------------------------------------------------- */

MODULE void init_cvs_stars(void);
MODULE void do_cvs(void);

// CODE-------------------------------------------------------------------

EXPORT void astrowars_setmemmap(void)
{   int i, address, mirror;

    game = FALSE;

    for (i = 0x1400; i <= 0x7FFF; i++)
    {   memory[i] = 0; // important
    }
    for (i = 0; i <= 0x13FF; i++)
    {   memory[         i] = astrowars_game[         i]; // $0000..$13FF -> $0000..$13FF (  5K)
        memory[0x2000 + i] = astrowars_game[0x1400 + i]; // $1400..$27FF -> $2000..$33FF (  5K)
    }

    // This is necessary for the boss level to work properly (bad dump?).
    memory[0x2A00] = 0xA4; // $FB -> $A4
    memory[0x2A01] =    1; // $F8 -> $01

    for (address = 0; address < 32 * KILOBYTE; address++)
    {   memflags[address] = 0;
        mirror            = address;

        if
        (   (address >= 0x1600 && address <= 0x17FF)
         || (address >= 0x1D00 && address <= 0x1FFF)
        )
        {   memflags[address] |= NOREAD | NOWRITE;
        }

        if
        (   (address <= 0x13FF)                      // ROM
         || (address >= 0x2000 && address <= 0x33FF) // ROM
        )
        {   memflags[address] |= NOWRITE;
        }

        if  (address >= 0x1800 && address <= 0x1CFF) // RAM
        {   memflags[address] |= VISIBLE | BANKED;
        }

        if
        (   (address >= 0x3400    && address <= 0x3FFF    )
         || (address >= 0x5400    && address <= 0x5FFF    )
         || (address >= 0x7400 /* && address <= 0x7FFF */ )
        )
        {   mirror = address & 0x1FFF;
        }

        mirror_r[address] =
        mirror_w[address] = (UWORD) mirror;
    }

    patchrom();
    ioport[PORTD].contents = 0;

    machines[machine].pvis = 1;
    pvibase = 0x1500;
    pvi_memmap();

#ifdef WIN32
    hostmachines[machine].controls = IDD_CONTROLS_ASTROWARS;
    hostmachines[machine].monitor_xvi = IDD_MONITOR_1PVI;
#endif
#ifdef AMIGA
    hostmachines[machine].controls = IMAGE_CON_ASTROWARS;
#endif
    key1 =
    key2 =
    key3 =
    key4 = 2;
    machines[machine].keys = 6;
    whichkeyrect = KEYRECT_ASTROWARS;
    keynames_from_overlay();

    machines[machine].firstequiv =
    machines[machine].lastequiv  = -1;
    analog           =
    swapped          = FALSE;

    for (i = 0; i < 4; i++)
    {   p1bgcol[i] =
        p2bgcol[i] = 0;
    }
    for (i = 0; i < 6; i++)
    {   p1sprcol[i] =
        p2sprcol[i] = 0;
    }

    cpl = 227 / 3; // 75.6'
    machines[machine].cpf = (int) ((227.0 / 3.0) * 312.0);
    region = REGION_PAL;

    init_cvs_stars();
}

EXPORT void galaxia_setmemmap(void)
{   int i, address, mirror;

    game = FALSE;

    for (i = 0x1400; i <= 0x7FFF; i++)
    {   memory[i] = 0; // important
    }
    for (i = 0; i <= 0x13FF; i++)
    {   memory[         i] = galaxia_game[         i]; // $0000..$13FF -> $0000..$13FF (  5K)
        memory[0x2000 + i] = galaxia_game[0x1400 + i]; // $1400..$27FF -> $2000..$33FF (  5K)
    }

    for (address = 0; address < 32 * KILOBYTE; address++)
    {   memflags[address] = 0;
        mirror            = address;

        if
        (   (address <= 0x13FF)                      // ROM
         || (address >= 0x2000 && address <= 0x33FF) // ROM
        )
        {   memflags[address] |= NOWRITE;
        }

        if  (address >= 0x1400 && address <= 0x14FF) // RAM
        {   memflags[address] |= VISIBLE;
        }

        if  (address >= 0x1800 && address <= 0x1BFF) // RAM
        {   memflags[address] |= VISIBLE | BANKED;
        }

        if
        (   (address >= 0x3400    && address <= 0x3FFF    )
         || (address >= 0x5400    && address <= 0x5FFF    )
         || (address >= 0x7400 /* && address <= 0x7FFF */ )
        )
        {   mirror = address & 0x1FFF;
        }

        mirror_r[address] =
        mirror_w[address] = (UWORD) mirror;
    }

    patchrom();
    ioport[PORTD].contents = 0;

    machines[machine].pvis = 3;
    pvibase = 0x1500;
    pvi_memmap();
    pvibase = 0x1600;
    pvi_memmap();
    pvibase = 0x1700;
    pvi_memmap();
    pvibase = 0x1500;

#ifdef WIN32
    hostmachines[machine].controls = IDD_CONTROLS_GALAXIA;
    hostmachines[machine].monitor_xvi = IDD_MONITOR_3PVI;
#endif
#ifdef AMIGA
    hostmachines[machine].controls = IMAGE_CON_GALAXIA;
#endif
    key1 =
    key2 =
    key3 =
    key4 = 2;
    machines[machine].keys = 6;
    whichkeyrect = KEYRECT_GALAXIA;
    keynames_from_overlay();

    machines[machine].firstequiv =
    machines[machine].lastequiv  = -1;
    analog        =
    swapped       = FALSE;

    for (i = 0; i < 4; i++)
    {   p1bgcol[i] =
        p2bgcol[i] = 0;
    }
    for (i = 0; i < 6; i++)
    {   p1sprcol[i] =
        p2sprcol[i] = 0;
    }

    cpl = 227 / 3; // 75.6'
    machines[machine].cpf = (int) ((227.0 / 3.0) * 312.0);
    region = REGION_PAL;

    init_cvs_stars();

    squeal   = 0;
#ifdef AMIGA
    throb    = 0;
#endif
}

EXPORT void lb_setmemmap(void)
{   int address,
        i,
        mirror;

    game = FALSE;

    for (i = 0x1400; i <= 0x7FFF; i++)
    {   memory[i] = 0;
    }

    if (memmap == MEMMAP_LAZARIAN)
    {   /* Lazarian is:
        5K ($0000..$13FF -> $0000..$13FF)
        3K gap             ($1400..$1FFF)
        5K ($1400..$27FF -> $2000..$33FF)
        1K gap             ($3400..$37FF)
        1K ($2800..$2BFF -> $3800..$3BFF)
        1K gap             ($3C00..$3FFF)
        5K ($2C00..$3FFF -> $4000..$53FF)
        3K gap             ($5400..$5FFF)
        5K ($4000..$53FF -> $6000..$73FF)
        1K gap             ($7400..$77FF)
        1K ($5400..$57FF -> $7800..$7BFF)
        1K gap             ($7C00..$7FFF) */

        tilesptr = lz_tiles;
        for (i = 0; i <= 0x13FF; i++)
        {   memory[         i] = lz_game[i];           // $0000..$13FF -> $0000..$13FF (5K)
            memory[0x2000 + i] = lz_game[0x1400 + i];  // $1400..$27FF -> $2000..$33FF (5K)
            memory[0x4000 + i] = lz_game[0x2C00 + i];  // $2C00..$3FFF -> $4000..$53FF (5K)
            memory[0x6000 + i] = lz_game[0x4000 + i];  // $4000..$53FF -> $6000..$73FF (5K)
        }
        for (i = 0; i <=  0x3FF; i++)
        {   memory[0x3800 + i] = lz_game[0x2800 + i];  // $2800..$2BFF -> $3800..$3BFF (1K)
            memory[0x7800 + i] = lz_game[0x5400 + i];  // $5400..$57FF -> $7800..$7BFF (1K)
    }   }
    else
    {   // assert(memmap == MEMMAP_LASERBATTLE);
        tilesptr = lb_tiles;
        for (i = 0; i <= 0x13FF; i++)
        {   memory[         i] = lb_game[i];           // $0000..$13FF -> $0000..$13FF (5K)
            memory[0x2000 + i] = lb_game[0x1400 + i];  // $1400..$27FF -> $2000..$33FF (5K)
            memory[0x4000 + i] = lb_game[0x2800 + i];  // $2800..$3BFF -> $4000..$43FF (5K)
            memory[0x6000 + i] = lb_game[0x3C00 + i];  // $3C00..$4FFF -> $6000..$63FF (5K)
    }   }

#ifdef WIN32
    hostmachines[machine].controls = IDD_CONTROLS_LASERBATTLE;
    hostmachines[machine].monitor_xvi = IDD_MONITOR_3PVI;
#endif
#ifdef AMIGA
    hostmachines[machine].controls = IMAGE_CON_LASERBATTLE;
#endif
    key1 = 7;
    key2 = 8;
    key3 = 9;
    key4 = 5;
    machines[machine].keys = 11;
    whichkeyrect = KEYRECT_LASERBATTLE;
    keynames_from_overlay();

    for (address = 0; address < 32 * KILOBYTE; address++)
    {   memflags[address] = 0;
        mirror            = address;

        if
        (   (address <= 0x14FF                     ) // ROM 5K. If $14xx is mapped as RAM, Lazarian enemy bullet collisions don't work.
         || (address >= 0x2000 && address <= 0x33FF) // ROM 5K
         || (address >= 0x4000 && address <= 0x53FF) // ROM 5K
         || (address >= 0x6000 && address <= 0x73FF) // ROM 5K
         || (memmap == MEMMAP_LAZARIAN && address >= 0x3800 && address <= 0x3BFF) // ROM 1K
         || (memmap == MEMMAP_LAZARIAN && address >= 0x7800 && address <= 0x7BFF) // ROM 1K
        )
        {   memflags[address] |= NOWRITE;
        }

        if  (address >= 0x1800 && address <= 0x1BFF) // RAM
        {   memflags[address] |= VISIBLE | BANKED;
        }

        if
        (    (address & 0x1FFF) >= 0x1400
         && !(memmap == MEMMAP_LAZARIAN && address >= 0x3800 && address <= 0x3BFF)
         && !(memmap == MEMMAP_LAZARIAN && address >= 0x7800 && address <= 0x7BFF)
        )
        {   mirror = address & 0x1FFF;
        }

        mirror_r[address] =
        mirror_w[address] = (UWORD) mirror;
    }

    patchrom();

    lb_bank          =
    lb_tone          =
    lb_spritecode    =
    lb_spritecolour  =
    lb_spriteenable  =
    lb_spritex       =
    lb_spritey       =
    mux              = 0;
    lb_snd           = 0;

    abeff1           =
        abeff2           =
    useshell         =
    circle           =
        neg1             =
        neg2             = 0; // are these the correct initial values?

    machines[machine].pvis = 3;
    pvibase = 0x1500;
    pvi_memmap();
    pvibase = 0x1600;
    pvi_memmap();
    pvibase = 0x1700;
    pvi_memmap();
    pvibase = 0x1500;

    machines[machine].firstequiv =
    machines[machine].lastequiv  = -1;
    analog        =
    swapped       = FALSE;

    for (i = 0; i < 4; i++)
    {   p1bgcol[i] =
        p2bgcol[i] = 0;
    }
    for (i = 0; i < 6; i++)
    {   p1sprcol[i] =
        p2sprcol[i] = 0;
    }

    cpl = 227 / 3; // 75.6'
    machines[machine].cpf = (int) ((227.0 / 3.0) * 312.0);
    region = REGION_PAL;
}

EXPORT void astrowars_drawscreen(void)
{   PERSIST   int a, aa, // X-axis from guest's viewpoint, inverted Y-axis from user's viewpoint
                  b, bb, // Y-axis from guest's viewpoint,          X-axis from user's viewpoint
                  contents,
                  localoffset,
                  starta,
                  startb;
#ifdef SHOWCHARSET
    TRANSIENT int whichcontents = 0;
#endif

    // assert(memmap == MEMMAP_ASTROWARS);

    // Each character is 8*8 pixels (8 bytes).
    // Character PROM is 2K (with 256 characters, each of 8 bytes).

#ifdef SHOWCHARSET
    contents = 0;
    for (b = 2; b < 32; b++)
    {   for (a = 0; a < 30; a++)
        {   starta = a << 3;
            startb = (b - 2) << 3;

            if (a >= 8 && a <= 23 && b >= 8 && b <= 23)
            {   contents = whichcontents++;
            } else
            {   contents = 239;
            }
            localoffset = contents * 8;
            for (aa = 7; aa >= 0; aa--)
            {   for (bb = 0; bb < 8; bb++)
                {   if (astrowars_tiles[localoffset + bb] & (128 >> aa))
                    {   changepixel(starta + aa, startb + bb, WHITE);
                    } else
                    {   changepixel(starta + aa, startb + bb, BLACK);
    }   }   }   }   }
#else
    for (b = 2; b < 32; b++)        // Y-axis from guest's viewpoint,          X-axis from user's viewpoint
    {   for (a = 0; a < 30; a++)    // X-axis from guest's viewpoint, inverted Y-axis from user's viewpoint
        {   starta = a * 8;         // from guest's viewpoint, X-start of this character (ie. left edge)
            startb = (b - 2) * 8;   // from guest's viewpoint, Y-start of this character (ie. top  edge)
            contents = memory[0x1800 + (b * 32) + a];

            localoffset = contents * 8;
            for (aa = 7; aa >= 0; aa--)
            {   for (bb = 0; bb < 8; bb++)
                {   if (astrowars_tiles[localoffset + bb] & (128 >> aa))
                    {   changepixel(starta + aa, startb + bb, astrowars_colours[g_bank1[(b * 32) + a] & 7]);
                        coinops_colltable[0][startb + bb + 16][starta + aa + HIDDEN_X] |= 8; // tile pixel. [0][16..255][16..255]
                    } else
                    {   changebgpixel(starta + aa, startb + bb, BLACK);
    }   }   }   }   }

    do_cvs();
#endif
}

EXPORT void galaxia_drawscreen(void)
{   FAST int a, aa, // X-axis from guest's viewpoint, inverted Y-axis from user's viewpoint
             b, bb, // Y-axis from guest's viewpoint,          X-axis from user's viewpoint
             contents,
             starta,
             startb,
             ourstartb;

    // assert(memmap == MEMMAP_GALAXIA);

    if (galaxia_scrolly < 0)
    {   for (b = 240 + galaxia_scrolly; b < 240; b++)
        {   for (a = 4*8; a < 14*8; a++)
            {   changepixel(a, b, BLACK);
    }   }   }
    elif (galaxia_scrolly > 0)
    {   for (b = 0; b < galaxia_scrolly; b++)
        {   for (a = 4*8; a < 14*8; a++)
            {   changepixel(a, b, BLACK);
    }   }   }

    for (b = 2; b < 32; b++) // Y from machine's point of view
    {   for (a = 0; a < 30; a++) // X from machine's point of view
        {   starta = (int) ( a      * 8);
            startb = (int) ((b - 2) * 8);

#ifdef SHOWCHARSET
            if (a >= 8 && a < 24 && b >= 8 && b < 24) contents = ((((b - 8) * 16) + a - 8) & 0x7F) << 3; else
#endif
            contents = (memory[0x1800 + (b * 32) + a] & 0x7F) << 3;

            for (aa = 7; aa >= 0; aa--)
            {   ourstartb = startb;
                if (starta + aa >= 32 && starta + aa <= 111)
                {   ourstartb += galaxia_scrolly;
                }
                for (bb = 0; bb < 8; bb++)
                {   if
                    (   starta + aa <  0
                     || starta + aa >= COINOP_BOXWIDTH
                     || ourstartb + bb <  0
                     || ourstartb + bb >= COINOP_BOXHEIGHT
                    )
                    {   continue;
                    } // implied else
                    if ((memory[0x1800 + (b * 32) + a] & 0x7F) >= 0x10)
                    {   if (galaxia_tiles1[contents + bb] & (128 >> aa))
                        {   if (galaxia_tiles2[contents + bb] & (128 >> aa))
                            {   changepixel(starta + aa, ourstartb + bb, galaxia_colours[g_bank1[(b * 32) + a] & 3][3]);
                            } else
                            {   changepixel(starta + aa, ourstartb + bb, galaxia_colours[g_bank1[(b * 32) + a] & 3][2]);
                            }
                            coinops_colltable[0][ourstartb + bb + 16][starta + aa + HIDDEN_X] |= 8; // tile pixel. [0][16..255][16..255]
                        } else
                        {   if (galaxia_tiles2[contents + bb] & (128 >> aa))
                            {   changepixel(starta + aa, ourstartb + bb, galaxia_colours[g_bank1[(b * 32) + a] & 3][1]);
                                coinops_colltable[0][ourstartb + bb + 16][starta + aa + HIDDEN_X] |= 8; // tile pixel. [0][16..255][16..255]
                            } else
                            {   changebgpixel(starta + aa, ourstartb + bb, galaxia_colours[g_bank1[(b * 32) + a] & 3][0]); // or just BLACK
                    }   }   }
                    else
                    {   if (galaxia_tiles1[contents + bb] & (128 >> aa))
                        {   if (galaxia_tiles2[contents + bb] & (128 >> aa))
                            {   changepixel(starta + aa, ourstartb + bb, galaxia_colours2[g_bank1[(b * 32) + a] & 3][3]);
                            } else
                            {   changepixel(starta + aa, ourstartb + bb, galaxia_colours2[g_bank1[(b * 32) + a] & 3][2]);
                            }
                            coinops_colltable[0][ourstartb + bb + 16][starta + aa + HIDDEN_X] |= 8; // tile pixel. [0][16..255][16..255]
                        } else
                        {   if (galaxia_tiles2[contents + bb] & (128 >> aa))
                            {   changepixel(starta + aa, ourstartb + bb, galaxia_colours2[g_bank1[(b * 32) + a] & 3][1]);
                                coinops_colltable[0][ourstartb + bb + 16][starta + aa + HIDDEN_X] |= 8; // tile pixel. [0][16..255][16..255]
                            } else
                            {   changebgpixel(starta + aa, ourstartb + bb, galaxia_colours2[g_bank1[(b * 32) + a] & 3][0]); // or just BLACK
    }   }   }   }   }   }   }

    do_cvs();
}

EXPORT void lb_drawscreen(void)
{   PERSIST int   x, xx,
                  y, yy;
    PERSIST UBYTE t;
#ifndef SHOWCHARSET
    PERSIST int   colour;
#endif
    UBYTE eff1_val, eff2_val,
          eff1_cmp, eff2_cmp;
    FLAG  eff1, eff2,
          shell;

    // assert(memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN);

#ifdef SHOWCHARSET
    TRANSIENT int i = 0;

    for (y = 8; y < 24; y++)
    {   for (x = 8; x < 24; x++)
        {   memory[0x1800 + (y * 32) + x] = i++;
    }   }
#endif

    for (y = 0; y < COINOP_BOXHEIGHT; y++)
    {   eff1_val = memory_effects[((y + 16) & 0xff) | 0x100];
        eff2_val = memory_effects[((y + 16) & 0xff) | 0x000];

        for (x = 8; x < 128; x++)
        {   if (x > (~eff1_val & 0x7f))
            {   eff1_cmp = 1;
            } else
            {   eff1_cmp = 0;
            }
            if (x > (~eff2_val & 0x7f))
            {   eff2_cmp = 1;
            } else
            {   eff2_cmp = 0;
            }
            if (abeff1 && ((neg1 && !eff1_cmp) || (!neg1 && eff1_cmp)))
            {   eff1 = TRUE; // yes, going ahead with 1st area effect
            } else
            {   eff1 = FALSE;
            }
            eff2 = shell = FALSE;
            if (abeff2)
            {   if (!useshell)
                {   if ((neg2 && !eff2_cmp) || (!neg2 && eff2_cmp))
                    {   eff2 = TRUE; // yes, going ahead with 2nd area effect
                }   }
                else
                {   if (x == eff2_val)
                    {   shell = TRUE; // yes, going ahead with shell effect
            }   }   }

            if (shell)
            {   changepixel(x - 8, y, WHITE);
            } elif (eff1 || eff2)
            {   switch (circle)
                {
                case  0: changepixel(x - 8, y, BLACK);
                acase 1: changepixel(x - 8, y, GREEN); // unused?
                acase 2: changepixel(x - 8, y, abeff1 ? RED   : YELLOW);
                acase 3: changepixel(x - 8, y, BLUE);
        }   }   }

        for (x = 128; x < 248; x++)
        {   if ((x & 0x7F) < (eff1_val & 0x7F))
            {   eff1_cmp = 1;
            } else
            {   eff1_cmp = 0;
            }
            if ((x & 0x7F) < (eff2_val & 0x7F))
            {   eff2_cmp = 1;
            } else
            {   eff2_cmp = 0;
            }
            if (abeff1 && ((neg1 && !eff1_cmp) || (!neg1 && eff1_cmp)))
            {   eff1 = TRUE; // yes, going ahead with 1st area effect
            } else
            {   eff1 = FALSE;
            }
            eff2 = shell = FALSE;
            if (abeff2)
            {   if (!useshell)
                {   if ((neg2 && !eff2_cmp) || (!neg2 && eff2_cmp))
                    {   eff2 = TRUE; // yes, going ahead with 2nd area effect
                }   }
                else
                {   if (x == eff2_val)
                    {   shell = TRUE; // yes, going ahead with shell effect
            }   }   }

            if (shell)
            {   changepixel(x - 8, y, WHITE);
            } elif (eff1 || eff2)
            {   switch (circle)
                {
                case  0: changepixel(x - 8, y, BLACK);
                acase 1: changepixel(x - 8, y, GREEN); // unused?
                acase 2: changepixel(x - 8, y, abeff1 ? RED : YELLOW);
                acase 3: changepixel(x - 8, y, BLUE);
    }   }   }   }

    for (x = 0; x < 30; x++)
    {   for (y = 2; y < 32; y++)
        {   t = memory[0x1800 + (y * 32) + x];
            for (yy = 0; yy < 8; yy++)
            {   for (xx = 0; xx < 8; xx++)
                {   if (tilesptr[(t * 8) + yy] & (128 >> xx))
                    {   if (tilesptr[2048 + (t * 8) + yy] & (128 >> xx))
                        {   if (tilesptr[4096 + (t * 8) + yy] & (128 >> xx))
                            {   changepixel((x * 8) + xx, ((y - 2) * 8) + yy, WHITE);
                            } else
                            {   changepixel((x * 8) + xx, ((y - 2) * 8) + yy, YELLOW);
                        }   }
                        else
                        {   if (tilesptr[4096 + (t * 8) + yy] & (128 >> xx))
                            {   changepixel((x * 8) + xx, ((y - 2) * 8) + yy, PURPLE);
                            } else
                            {   changepixel((x * 8) + xx, ((y - 2) * 8) + yy, RED);
                    }   }   }
                    else
                    {   if (tilesptr[2048 + (t * 8) + yy] & (128 >> xx))
                        {   if (tilesptr[4096 + (t * 8) + yy] & (128 >> xx))
                            {   changepixel((x * 8) + xx, ((y - 2) * 8) + yy, CYAN);
                            } else
                            {   changepixel((x * 8) + xx, ((y - 2) * 8) + yy, GREEN);
                        }   }
                        else
                        {   if (tilesptr[4096 + (t * 8) + yy] & (128 >> xx))
                            {   changepixel((x * 8) + xx, ((y - 2) * 8) + yy, BLUE);
                            } else
                            {   changebgpixel((x * 8) + xx, ((y - 2) * 8) + yy, BLACK);
    }   }   }   }   }   }   }

#ifndef SHOWCHARSET
    if (lb_spriteenable)
    {   for (y = 0; y < 32; y++)
        {   for (x = 0; x < 8; x++)
            {   t = lb_sprites[(256 * lb_spritecode) + x + (y * 8)];
                xx = (250 - lb_spritex) + (x * 4);
                yy = (239 - lb_spritey) +  y     ;
                if (xx >= 0 && yy >= 0 && yy < COINOP_BOXHEIGHT)
                {   colour = (t & 0xC0) >> 6;
                    if (colour && xx     < COINOP_BOXWIDTH)
                    {   changepixel(xx    , yy, colour);
                    }
                    colour = (t & 0x30) >> 4;
                    if (colour && xx + 1 < COINOP_BOXWIDTH)
                    {   changepixel(xx + 1, yy, colour);
                    }
                    colour = (t & 0x0C) >> 2;
                    if (colour && xx + 2 < COINOP_BOXWIDTH)
                    {   changepixel(xx + 2, yy, colour);
                    }
                    colour = (t & 0x03);
                    if (colour && xx + 3 < COINOP_BOXWIDTH)
                    {   changepixel(xx + 3, yy, colour);
    }   }   }   }   }
#endif
}

EXPORT UBYTE zaccaria_readport(int port)
{   UBYTE t;

    t = 0;
    switch (port)
    {
    case 0:
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:
        case MEMMAP_GALAXIA:
            if (coinop_button1) t |= 0x20; // set bit 5 (%00100000)
            if (coinop_button2) t |= 0x10; // set bit 4 (%00010000)
            if (coinop_coina  ) t |= 0x08; // set bit 3 (%00001000)
            if (coinop_coinb  ) t |= 0x04; // set bit 2 (%00000100)
            if (coinop_2p     ) t |= 0x02; // set bit 1 (%00000010)
            if (coinop_1p     ) t |= 0x01; // set bit 0 (%00000001)
        }
    acase 1:
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:
        case MEMMAP_GALAXIA:
            t = 0xFF;
        }
    acase 2: // all read this port
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:
        case MEMMAP_GALAXIA:
            // bit  7    is freeze (active low)
            // bit  6    is unused?
            // bit  5    is p1 right
            // bit  4    is p1 left
            // bit  3    is p2 right
            // bit  2    is p2 left
            // bit  1    is unused?
            // bit  0    0=multi-firebutton cabinet, 1=single-firebutton cabinet
            //  (currently we only emulate the multi-firebutton cabinet)
            if (memmap == MEMMAP_ASTROWARS)
            {   t = (UBYTE) (aw_dips1 & 0x80);
            } else
            {   // assert(memmap == MEMMAP_GALAXIA);
                t = (UBYTE) (ga_dips  & 0x80);
            }
            if (coinop_joy1right) t |= 0x20; // set bit 5 (%00100000)
            if (coinop_joy1left ) t |= 0x10; // set bit 4 (%00010000)
            if (coinop_joy2right) t |= 0x08; // set bit 3 (%00001000)
            if (coinop_joy2left ) t |= 0x04; // set bit 2 (%00000100)
        acase MEMMAP_LAZARIAN:
            switch (mux)
            {
            case 0:
                t = 0xFF;
                if (coinop_button4  ) t &= 0x7F; // clear bit 7 (%01111111)
                if (coinop_button3  ) t &= 0xBF; // clear bit 6 (%10111111)
                if (coinop_button2  ) t &= 0xDF; // clear bit 5 (%11011111)
                if (coinop_button1  ) t &= 0xEF; // clear bit 4 (%11101111)
                // bit 3 is service button?
                if (coinop_coinb    ) t &= 0xFB; // clear bit 2 (%11111011)
                if (coinop_2p       ) t &= 0xFD; // clear bit 1 (%11111101)
                if (coinop_1p       ) t &= 0xFE; // clear bit 0 (%11111110)
            acase 1:
             /* bit  7    is unused?
                bit  6    is coin A
                bits 5..0 are unused?
                The "unused?" bits should be set high to avoid problems with player 2's controls. */
                t = coinop_coina ? 0xBF : 0xFF;
            acase 2:
             /* bit  7   : collision detection on/off (%0=trainer, %1=normal)
                bit  6   : calibration grid    on/off (%0=normal , %1=grid  )
                bits 5..4: number of ships:
                 %00=2
                 %01=3
                 %10=4
                 %11=5
                bits 3..2: coin slot B:
                 %11=7 credits
                 %10=5 credits
                 %01=3 credits
                 %00=2  credits
                bits 1..0: coin slot A:
                 %00=½ credit
                 %01=1 credit
                 %10=2 credits
                 %11=3 credits */
                t = lz_dips2;
            acase 3:
             /* bits 7..4 are joystick directions
                bit  3    is  unused?
                bit  2    is  freeze       (%1=frozen)
                bit  1    is  rapid firing (%1=rapid)
                bit  0    is  unused? */
                t = lz_dips3 | 0xF0;
                if (coinop_joy1down ) t &= 0x7F; // clear bit 7 (%01111111)
                if (coinop_joy1up   ) t &= 0xBF; // clear bit 6 (%10111111)
                if (coinop_joy1right) t &= 0xDF; // clear bit 5 (%11011111)
                if (coinop_joy1left ) t &= 0xEF; // clear bit 4 (%11101111)
            }
        acase MEMMAP_LASERBATTLE:
            switch (mux)
            {
            case 0:
                t = 0xFF;
                if (coinop_button4  ) t &= 0x7F; // clear bit 7 (%01111111)
                if (coinop_button3  ) t &= 0xBF; // clear bit 6 (%10111111)
                if (coinop_button2  ) t &= 0xDF; // clear bit 5 (%11011111)
                if (coinop_button1  ) t &= 0xEF; // clear bit 4 (%11101111)
                // bit 3 is service button?
                if (coinop_coinb    ) t &= 0xFB; // clear bit 2 (%11111011)
                if (coinop_2p       ) t &= 0xFD; // clear bit 1 (%11111101)
                if (coinop_1p       ) t &= 0xFE; // clear bit 0 (%11111110)
            acase 1:
                t = 0xFF;
                if (coinop_coina)
                {   t &= 0xBF; // clear bit 6 (%10111111)
                }
            acase 2:
                t = lb_dips;
            acase 3:
                // bit 0 controls random high scores (%1=random)
                t = 0xF0;
                if (coinop_joy1down ) t &= 0x7F; // clear bit 7 (%01111111)
                if (coinop_joy1up   ) t &= 0xBF; // clear bit 6 (%10111111)
                if (coinop_joy1right) t &= 0xDF; // clear bit 5 (%11011111)
                if (coinop_joy1left ) t &= 0xEF; // clear bit 4 (%11101111)
        }   }
    acase 5:
        if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
        {   // bits 7..1 are unused
            // bit  0    enables collisions
            t = 0xFF;
        }
    acase 6:
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:
            // bits 7..6 are unknown
            // bits 5..4 are used but unknown (difficulty?)
            // bit  3    is  no. of lives (clear=3, set=5)
            // bit  2    is  B coinage ( %1 is best)
            // bits 1..0 are A coinage (%11 is best)
            t = (UBYTE) (aw_dips1 & 0x7F);
        acase MEMMAP_GALAXIA:
            // bits 7..6 are unknown
            // bits 5..4 are used but unknown (difficulty?)
            // bit  3    is  no. of lives (clear=3, set=5)
            // bit  2    is  B coinage ( %1 is best)
            // bits 1..0 are A coinage (%11 is best)
            t = (UBYTE) (ga_dips & 0x7F);
        }
    acase 7:
        if (memmap == MEMMAP_ASTROWARS)
        {   //  bits 7..5 are unknown
            //  bits 4..3 are used but unknown
            //  bits 2..1: starting hiscore (Astro Wars only?)
            //   %00 =  5000
            //   %01 =  7500
            //   %10 = 10000
            //   %11 = 12500
            //  bit  0    is whether to have a starting hiscore (otherwise 0) (Astro Wars only?)
            t = (UBYTE) aw_dips2;
        }
    acase 0xAC:
        if (memmap == MEMMAP_GALAXIA)
        {   if (coinop_button1) t |= 0x20; // set bit 5 (%00100000)
            if (coinop_button2) t |= 0x10; // set bit 4 (%00010000)
            if (coinop_coina  ) t |= 0x08; // set bit 3 (%00001000)
            if (coinop_coinb  ) t |= 0x04; // set bit 2 (%00000100)
            if (coinop_2p     ) t |= 0x02; // set bit 1 (%00000010)
            if (coinop_1p     ) t |= 0x01; // set bit 0 (%00000001)
        }
    acase PORTC: // used for collision detection
        if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
        {   t = awga_collide;
        }
    acase PORTD:
        if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
        {   t = 0xFF;
            awga_collide = 0;
    }   }

    logport(port, t, FALSE);
    return t;
}

EXPORT void zaccaria_writeport(int port, UBYTE data)
{   FAST FLAG sounding;

    sounding = FALSE;

    switch (port)
    {
    case 0:
        switch (memmap)
        {
        case MEMMAP_GALAXIA:
        case MEMMAP_ASTROWARS:
            galaxia_scrolly = 255 - data;
        acase MEMMAP_LASERBATTLE:
        case MEMMAP_LAZARIAN:
            abeff1   = (data & 0x01) ? 0 : 1;
                abeff2   = (data & 0x02) ? 0 : 1;
            useshell = (data & 0x04) ? 0 : 1;
            circle   = (data & 0x18) >> 3;
                neg1     = (data & 0x20) ? 0 : 1;
                neg2     = (data & 0x40) ? 0 : 1;
        }
    acase 1:
        if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
        {   lb_spritecode   = (data & 0xE0) >> 5;
            lb_bank         = (data & 0x10) >> 4;
            lb_spritecolour = (data & 0x06) >> 1;
            lb_spriteenable = (data & 0x01) ^  1;
        }
    acase 2:
        if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
        {   lb_tone = data;
        }
    acase 4:
        if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
        {   lb_spritex = data;
        }
    acase 5:
        if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
        {   lb_spritey = data;
        }
    acase 6:
        if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
        {   mux = (data & 0x30) >> 4;
            // For Lazarian, at least:
            // bit 3 is set during p2's turn    (and cleared otherwise)
            // bit 1 is set during title screen (and cleared otherwise)
        }
    acase 7:
        if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
        {   if (data & 2)
            {   lb_snd |= 0x1000000;
            } else
            {   lb_snd &= 0x0FFFFFF;
            }
            switch (data & 0x1C)
            {
            case  0x14: // %...101..
                lb_snd &= 0x1FFFF00;
                lb_snd |= lb_tone;
                playsound(FALSE);
            acase 0x18: // %...110..
                lb_snd &= 0x1FF00FF;
                lb_snd |= (lb_tone <<  8);
                playsound(FALSE);
            acase 0x1C: // %...111..
                lb_snd &= 0x100FFFF;
                lb_snd |= (lb_tone << 16);
                playsound(FALSE);
        }   }
 /* acase PORTC:
        // Astro Wars writes $20 here
        // Galaxia also writes here
        ; */
    acase PORTD:
        if (memmap == MEMMAP_ASTROWARS)
        {   /* Bits 7..5: unused?
               Bit  4:    unknown
               Bit  3:    unused?
               Bit  2:    fire (player)
               Bit  1:    die (player or enemy)
               Bit  0:    unused? */
            if ((data & 4) && !(ioport[PORTD].contents & 4))
            {   play_sample(SAMPLE_SHOOT);
            }
            if ((data & 2) && !(ioport[PORTD].contents & 2))
            {   play_sample(SAMPLE_BANG);
        }   }
        elif (memmap == MEMMAP_GALAXIA)
        {   /* Bits 7..6: unused?
               Bits 5..3: background throbbing?
               Bit  2:    fire (player)
               Bit  1:    die (player)
               Bit  0:    dive (enemy)? */
            if ((data & 8) != (ioport[PORTD].contents & 8))
            {
#ifdef AMIGA
                throb = (data & 8) ? 1 : 0;
#endif
                sounding = TRUE;
            }
            if ((data & 4) && !(ioport[PORTD].contents & 4))
            {   play_sample(SAMPLE_SHOOT);
            }
            if ((data & 2) && !(ioport[PORTD].contents & 2))
            {   play_sample(SAMPLE_BANG);
            }
            if (data & 1)
            {   if (!(ioport[PORTD].contents & 1))
                {   squeal = 0xFF;
                    sounding = TRUE;
            }   }
            else
            {   if (  ioport[PORTD].contents & 1 )
                {   squeal = 0;
                    sounding = TRUE;
    }   }   }   }

    logport(port, data, TRUE);
    if (sounding)
    {   playsound(FALSE); // we wait until after ioport[].contents has been changed
}   }

EXPORT void zaccaria_emuinput(void)
{   TRANSIENT UBYTE t;
    TRANSIENT int   old_coina, old_coinb;
    PERSIST   int   stale[4] = { 0, 0, 0, 0 };

    ReadJoystick(0);
    ReadJoystick(1);

    if (recmode == RECMODE_PLAY)
    {   coinop_play();
        t = IOBuffer[offset++];
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:
            if (t != aw_dips1)
            {   aw_dips1 = t;
                zaccaria_updatedips();
            }
            if (t != aw_dips2)
            {   aw_dips2 = t;
                zaccaria_updatedips();
            }
        acase MEMMAP_GALAXIA:
            if (t != ga_dips)
            {   ga_dips = t;
                zaccaria_updatedips();
            }
        acase MEMMAP_LAZARIAN:
            if (t != lz_dips2)
            {   lz_dips2 = t;
                zaccaria_updatedips();
            }
            t = IOBuffer[offset++];
            if (t != lz_dips3)
            {   lz_dips3 = t;
                zaccaria_updatedips();
            }
        acase MEMMAP_LASERBATTLE:
            if (t != lb_dips)
            {   lb_dips = t;
                zaccaria_updatedips();
        }   }

        if (coinop_joy1left == coinop_joy1right)
        {   ax[0] = sx[0] = 128;
        } elif (coinop_joy1right)
        {   ax[0] = sx[0] = 255;
        } elif (coinop_joy1left)
        {   ax[0] = sx[0] =   0;
        }
        if (coinop_joy1up == coinop_joy1down)
        {   ay[0] = sy[0] = 128;
        } elif (coinop_joy1down)
        {   ay[0] = sy[0] = 255;
        } elif (coinop_joy1up)
        {   ay[0] = sy[0] =   0;
        }
        if (coinop_joy2left == coinop_joy2right)
        {   ax[1] = sx[1] = 128;
        } elif (coinop_joy2right)
        {   ax[1] = sx[1] = 255;
        } elif (coinop_joy2left)
        {   ax[1] = sx[1] =   0;
        }
        ay[1] = sy[1] = 128;
    } else
    {   old_coina        = coinop_coina;
        old_coinb        = coinop_coinb;

        coinop_1p        =
        coinop_2p        =
        coinop_coina     =
        coinop_coinb     =
        coinop_joy1left  =
        coinop_joy1right =
        coinop_joy1up    =
        coinop_joy1down  =
        coinop_joy2left  =
        coinop_joy2right =
        coinop_button1   =
        coinop_button2   =
        coinop_button3   =
        coinop_button4   = 0;

        if (swapped)
        {   coinop_playerinput(1, 0);
            coinop_playerinput(0, 1);
        } else
        {   coinop_playerinput(0, 0);
            coinop_playerinput(1, 1);
        }

        if (KeyDown(console[0]) || (jff[0] & JOYSTART) || (jff[1] & JOYSTART) || console_start)
        {   if (!stale[0])
            {   coinop_1p |= 1; // 1UP
            }
            stale[0] = 1;
        } else
        {   stale[0] = 0;
        }

        if (KeyDown(console[1]) || (jff[0] & JOYA    ) || (jff[1] & JOYA    ) || console_a    )
        {   if (!stale[1])
            {   coinop_2p |= 1; // 2UP
            }
            stale[1] = 1;
        } else
        {   stale[1] = 0;
        }

        if (KeyDown(console[2]) || (jff[0] & JOYB    ) || (jff[1] & JOYB    ) || console_b    )
        {   if (!stale[2])
            {   coinop_coina |= 1; // generous coin slot A (1C, 2C, 3C or 5C)
                if (ambient && !(old_coina & 1))
                {   play_sample(SAMPLE_COIN);
            }   }
            stale[2] = 1;
        } else
        {   stale[2] = 0;
        }

        if (KeyDown(console[3]) || console_reset)
        {   if (!stale[3])
            {   coinop_coinb |= 1; // miserly coin slot B (½C or 1C)
                if (ambient && !(old_coinb & 1))
                {   play_sample(SAMPLE_COIN);
            }   }
            stale[3] = 1;
        } else
        {   stale[3] = 0;
        }

        domouse();
        // order-dependent re. the pviwrite(0x7214) below!

        if
        (   p1rumble
         && useff[swapped ? 1 : 0]
#ifdef WIN32
         && (hostcontroller[swapped ? 1 : 0] == CONTROLLER_1STDJOY || hostcontroller[swapped ? 1 : 0] == CONTROLLER_2NDDJOY)
#endif
#ifdef AMIGA
         && (hostcontroller[swapped ? 1 : 0] == CONTROLLER_1STAPAD || hostcontroller[swapped ? 1 : 0] == CONTROLLER_2NDAPAD)
#endif
        )
        {   ff_on(guest_to_host_joy(swapped ? 1 : 0), p1rumble);
            p1rumble = 0;
        } else
        {   ff_decay(guest_to_host_joy(swapped ? 1 : 0));
        }
        p2rumble = 0;

        if (recmode == RECMODE_RECORD)
        {   coinop_record();
            switch (memmap)
            {
            case  MEMMAP_ASTROWARS:   OutputBuffer[2] = aw_dips1; OutputBuffer[3] = aw_dips2; DISCARD fwrite(OutputBuffer, 4, 1, MacroHandle);
            acase MEMMAP_GALAXIA:     OutputBuffer[2] = ga_dips ;                             DISCARD fwrite(OutputBuffer, 3, 1, MacroHandle);
            acase MEMMAP_LAZARIAN:    OutputBuffer[2] = lz_dips2; OutputBuffer[3] = lz_dips3; DISCARD fwrite(OutputBuffer, 4, 1, MacroHandle);
            acase MEMMAP_LASERBATTLE: OutputBuffer[2] = lb_dips ;                             DISCARD fwrite(OutputBuffer, 3, 1, MacroHandle);
            }
            // should really check return code of fwrite()
    }   }

    if (memmap == MEMMAP_GALAXIA)
    {   t = 0;
        if (coinop_button1) t |= 0x20; // set bit 5 (%00100000)
        if (coinop_button2) t |= 0x10; // set bit 4 (%00010000)
        if (coinop_coina  ) t |= 0x08; // set bit 3 (%00001000)
        if (coinop_coinb  ) t |= 0x04; // set bit 2 (%00000100)
        if (coinop_2p     ) t |= 0x02; // set bit 1 (%00000010)
        if (coinop_1p     ) t |= 0x01; // set bit 0 (%00000001)
        pviwrite(0x7214, t, FALSE);
    }

    if (console_start) console_start--;
    if (console_a    ) console_a--;
    if (console_b    ) console_b--;
    if (console_reset) console_reset--;
}

EXPORT void zaccaria_viewscreen(void)
{   TEXT rowchars[16 + 1];
    int  where,
         x, y;

    if (machine != ZACCARIA)
    {   return;
    }

    rowchars[16] = EOS;

    if (rotating)
    {   zprintf(TEXTPEN_VIEW, "---Left Half---\n");
        for (y = 0; y < 32; y++)
        {   where = 0x1BE0 + y;
            for (x = 0; x < 16; x++)
            {   rowchars[x] = guestchar(memory[where - (x * 0x20)]);
            }
            zprintf
            (   TEXTPEN_VIEW, // ideally, we would show the text in the correct colours
                "$%04X: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %s\n",
                where,
                memory[where        ],
                memory[where -  0x20],
                memory[where -  0x40],
                memory[where -  0x60],
                memory[where -  0x80],
                memory[where -  0xA0],
                memory[where -  0xC0],
                memory[where -  0xE0],
                memory[where - 0x100],
                memory[where - 0x120],
                memory[where - 0x140],
                memory[where - 0x160],
                memory[where - 0x180],
                memory[where - 0x1A0],
                memory[where - 0x1C0],
                memory[where - 0x1E0],
                rowchars
            );
        }
        zprintf(TEXTPEN_VIEW, "---Right Half---\n");
        for (y = 0; y < 32; y++)
        {   where = 0x19E0 + y;
            for (x = 0; x < 16; x++)
            {   rowchars[x] = guestchar(memory[where - (x * 0x20)]);
            }
            zprintf
            (   TEXTPEN_VIEW, // ideally, we would show the text in the correct colours
                "$%04X: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %s\n",
                where,
                memory[where        ],
                memory[where -  0x20],
                memory[where -  0x40],
                memory[where -  0x60],
                memory[where -  0x80],
                memory[where -  0xA0],
                memory[where -  0xC0],
                memory[where -  0xE0],
                memory[where - 0x100],
                memory[where - 0x120],
                memory[where - 0x140],
                memory[where - 0x160],
                memory[where - 0x180],
                memory[where - 0x1A0],
                memory[where - 0x1C0],
                memory[where - 0x1E0],
                rowchars
            );
    }   }
    else
    {   for (where = 0x1800; where < 0x1C00; where += 16)
        {   dumprow(where);
            if (where % 32)
            {   zprintf(TEXTPEN_VIEW, "\n");
    }   }   }
    zprintf(TEXTPEN_VIEW, "\n");

    if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
    {   zprintf(TEXTPEN_VIEW, "Colour attribute RAM:\n");
        if (rotating)
        {   zprintf(TEXTPEN_VIEW, "---Left Half---\n");
            for (y = 0; y < 32; y++)
            {   where = 0x3E0 + y;
                zprintf
                (   TEXTPEN_VIEW, // ideally, we would show the text in the correct colours
                    "$%04X: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
                    where + 0x1800,
                    g_bank1[where        ],
                    g_bank1[where -  0x20],
                    g_bank1[where -  0x40],
                    g_bank1[where -  0x60],
                    g_bank1[where -  0x80],
                    g_bank1[where -  0xA0],
                    g_bank1[where -  0xC0],
                    g_bank1[where -  0xE0],
                    g_bank1[where - 0x100],
                    g_bank1[where - 0x120],
                    g_bank1[where - 0x140],
                    g_bank1[where - 0x160],
                    g_bank1[where - 0x180],
                    g_bank1[where - 0x1A0],
                    g_bank1[where - 0x1C0],
                    g_bank1[where - 0x1E0]
                );
            }
            zprintf(TEXTPEN_VIEW, "---Right Half---\n");
            for (y = 0; y < 32; y++)
            {   where = 0x1E0 + y;
                zprintf
                (   TEXTPEN_VIEW, // ideally, we would show the text in the correct colours
                    "$%04X: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
                    where + 0x1800,
                    g_bank1[where        ],
                    g_bank1[where -  0x20],
                    g_bank1[where -  0x40],
                    g_bank1[where -  0x60],
                    g_bank1[where -  0x80],
                    g_bank1[where -  0xA0],
                    g_bank1[where -  0xC0],
                    g_bank1[where -  0xE0],
                    g_bank1[where - 0x100],
                    g_bank1[where - 0x120],
                    g_bank1[where - 0x140],
                    g_bank1[where - 0x160],
                    g_bank1[where - 0x180],
                    g_bank1[where - 0x1A0],
                    g_bank1[where - 0x1C0],
                    g_bank1[where - 0x1E0]
                );
        }   }
        else
        {   for (y = 0; y < 64; y++)
            {   where = y * 16;
                zprintf
                (   TEXTPEN_VIEW, // ideally, we would show the text in the correct colours
                    "$%04X: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
                    where + 0x1800,
                    g_bank1[where     ],
                    g_bank1[where +  1],
                    g_bank1[where +  2],
                    g_bank1[where +  3],
                    g_bank1[where +  4],
                    g_bank1[where +  5],
                    g_bank1[where +  6],
                    g_bank1[where +  7],
                    g_bank1[where +  8],
                    g_bank1[where +  9],
                    g_bank1[where + 10],
                    g_bank1[where + 11],
                    g_bank1[where + 12],
                    g_bank1[where + 13],
                    g_bank1[where + 14],
                    g_bank1[where + 15]
                );
                if (y % 2)
                {   zprintf(TEXTPEN_VIEW, "\n");
        }   }   }

        // Should we show palette RAM differently according to the rotation flag?
        zprintf
        (   TEXTPEN_VIEW, // ideally, we would show the text in the correct colours
            "Palette RAM:\n" \
            "$%04X: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n\n",
            (memmap == MEMMAP_ASTROWARS) ? 0x1C00 : 0x1400,
            g_bank2[ 0],
            g_bank2[ 1],
            g_bank2[ 2],
            g_bank2[ 3],
            g_bank2[ 4],
            g_bank2[ 5],
            g_bank2[ 6],
            g_bank2[ 7],
            g_bank2[ 8],
            g_bank2[ 9],
            g_bank2[10],
            g_bank2[11],
            g_bank2[12],
            g_bank2[13],
            g_bank2[14],
            g_bank2[15]
        );
    } else
    {   // assert(memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN);
        zprintf(TEXTPEN_VIEW, "Effect RAM:\n");
        for (where = 0; where < 512; where += 16)
        {   zprintf
            (   TEXTPEN_VIEW,
                "$%04X: %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X %02X %02X\n",
                where + 0x1800,
                memory_effects[where     ],
                memory_effects[where +  1],
                memory_effects[where +  2],
                memory_effects[where +  3],
                memory_effects[where +  4],
                memory_effects[where +  5],
                memory_effects[where +  6],
                memory_effects[where +  7],
                memory_effects[where +  8],
                memory_effects[where +  9],
                memory_effects[where + 10],
                memory_effects[where + 11],
                memory_effects[where + 12],
                memory_effects[where + 13],
                memory_effects[where + 14],
                memory_effects[where + 15]
            );
        }
        zprintf(TEXTPEN_VIEW, "\n");
}   }

EXPORT void coinop_record(void)
{   // assert(machines[machine].coinop);

    OutputBuffer[0] = (UBYTE) (
                      ((coinop_1p        & 1) << 7)
                    | ((coinop_2p        & 1) << 6)
                    | ((coinop_coina     & 1) << 5)
                    | ((coinop_coinb     & 1) << 4)
                    | ((coinop_joy1left  & 1) << 3)
                    | ((coinop_joy1right & 1) << 2)
                    | ((coinop_joy1up    & 1) << 1)
                    |  (coinop_joy1down  & 1)
                      );
    OutputBuffer[1] = (UBYTE) (
                      ((coinop_joy2left  & 1) << 7)
                    | ((coinop_joy2right & 1) << 6)
                    | ((coinop_button1   & 1) << 5)
                    | ((coinop_button2   & 1) << 4)
                    | ((coinop_button3   & 1) << 3)
                    | ((coinop_button4   & 1) << 2)
                      );
}
EXPORT void coinop_play(void)
{   FAST UBYTE t;

    // assert(machines[machine].coinop);

    t = IOBuffer[offset++];
    coinop_1p        = (int) ((t & 0x80) >> 7);
    coinop_2p        = (int) ((t & 0x40) >> 6);
    coinop_coina     = (int) ((t & 0x20) >> 5);
    coinop_coinb     = (int) ((t & 0x10) >> 4);
    coinop_joy1left  = (int) ((t & 0x08) >> 3);
    coinop_joy1right = (int) ((t & 0x04) >> 2);
    coinop_joy1up    = (int) ((t & 0x02) >> 1);
    coinop_joy1down  = (int) ( t & 0x01      );
    t = IOBuffer[offset++];
    coinop_joy2left  = (int) ((t & 0x80) >> 7);
    coinop_joy2right = (int) ((t & 0x40) >> 6);
    coinop_button1   = (int) ((t & 0x20) >> 5);
    coinop_button2   = (int) ((t & 0x10) >> 4);
    coinop_button3   = (int) ((t & 0x08) >> 3);
    coinop_button4   = (int) ((t & 0x04) >> 2);
}

MODULE void do_cvs(void)
{   FAST int   bx, ct, i, y;
    FAST UBYTE t,
               xx, yy; // must be UBYTEs!

    /* Each byte of the bullet RAM represents one rastline.
       The value in that byte represents how far the bullet is from the right-hand side of the screen.
       Note that this explanation assumes "Settings|Graphics|Rotate screen?" is off
       (ie. it is from the computer's point of view, not the player's). */

    // do bullets
    for (y = 8; y < 248; y++)
    {   if (memmap == MEMMAP_ASTROWARS)
        {   t = memory[0x1C00 + y];
        } else
        {   // assert(memmap == MEMMAP_GALAXIA);
            t = memory[0x1400 + y];
        }
        if (t)
        {   for (ct = 0; ct < 4; ct++)
            {   bx = 255 - 7 - t - ct;
                if
                (   bx >= 0
                 && bx <  machines[machine].width // machines[machine].width is always 240 for these guests
                )
                {   if (collisions)
                    {   // enemy bullet vs. background
                        if (memmap == MEMMAP_ASTROWARS && screen[bx][y - 8] != 0)
                        {   awga_collide |=    2;
                        } elif (memmap == MEMMAP_GALAXIA && screen[bx][y - 8] != 7 && bx >= 211 && bx <= 223)
                        {   awga_collide |= 0x80;
                            p1rumble = 25;
                    }   }
                    changepixel(bx, y - 8, YELLOW);
                    coinops_colltable[0][y + 8][bx + HIDDEN_X] |= 4; // CVS pixel. [0][16..255][16..255]
    }   }   }   }

    // do stars
    for (i = 0; i < numstars; i++)
    {   xx = (starx[i] + starscroll) >> 1; // overflow is OK
        yy = stary[i] + ((starscroll + starx[i]) >> 9); // overflow is OK

        if ((yy & 1) ^ ((xx >> 4) & 1))
        {   if
            (   xx < machines[machine].width
             && yy < machines[machine].height
             && screen[xx][yy] == BLACK
            )
            {   changepixel(xx, yy, (yy % 6) + 1);
    }   }   }
    starscroll++;
}

MODULE void init_cvs_stars(void)
{   int bit1, bit2,
        generator = 0,
        x, y;

    numstars = 0;
    for (y = 255; y >= 0; y--)
    {   for (x = 511; x >= 0; x--)
        {   generator <<= 1;
            bit1 = (~generator >> 17) & 1;
            bit2 = ( generator >>  5) & 1;
            if (bit1 ^ bit2)
            {   generator |= 1;
            }
            if (((~generator >> 16) & 1) && (generator & 0xfe) == 0xfe)
            {   if (((~(generator >> 12)) & 0x01) && ((~(generator >> 13)) & 0x01))
                {   if (numstars < CVS_MAX_STARS)
                    {   starx[numstars] = x;
                        stary[numstars] = y;
                        numstars++;
}   }   }   }   }   }

EXPORT void zaccaria_ghostdips(void)
{   if
    (   machine != ZACCARIA
     || !SubWindowPtr[SUBWINDOW_DIPS]
    )
    {   return;
    }

    switch (memmap)
    {
    case MEMMAP_LAZARIAN:
        ra_enable( SUBWINDOW_DIPS, IDC_LIVES_6       , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_LIVES_INFINITE, FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_COINA_5C      , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_COINB_HALFC   , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_COINB_1C      , FALSE);
    //lint -fallthrough
    case MEMMAP_GALAXIA:
        ra_enable( SUBWINDOW_DIPS, IDC_SCORE_0       , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_SCORE_5000    , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_SCORE_7500    , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_SCORE_10000   , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_SCORE_12500   , FALSE);
        ra_enable2(SUBWINDOW_DIPS, IDC_SCORE         , FALSE);
    //lint -fallthrough
    case MEMMAP_ASTROWARS:
        ra_enable( SUBWINDOW_DIPS, IDC_LIVES_2       , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_LIVES_4       , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_LIVES_6       , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_LIVES_INFINITE, FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_COINA_HALFC   , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_COINB_2C      , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_COINB_3C      , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_COINB_5C      , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_COINB_7C      , FALSE);

        cb_enable( SUBWINDOW_DIPS, IDC_COLLISIONS    , FALSE);
        cb_enable( SUBWINDOW_DIPS, IDC_RAPIDFIRE     , FALSE);
        cb_enable( SUBWINDOW_DIPS, IDC_CALIBGRID     , FALSE);
    acase MEMMAP_LASERBATTLE:
        ra_enable(SUBWINDOW_DIPS, IDC_LIVES_4       , FALSE);
        ra_enable(SUBWINDOW_DIPS, IDC_COINA_HALFC   , FALSE);
        ra_enable(SUBWINDOW_DIPS, IDC_COINB_HALFC   , FALSE);
        ra_enable(SUBWINDOW_DIPS, IDC_COINB_1C      , FALSE);

        cb_enable(SUBWINDOW_DIPS, IDC_FREEZE        , FALSE);
        cb_enable(SUBWINDOW_DIPS, IDC_RAPIDFIRE     , FALSE);
        cb_enable(SUBWINDOW_DIPS, IDC_CALIBGRID     , FALSE);

        ra_enable( SUBWINDOW_DIPS, IDC_SCORE_0       , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_SCORE_5000    , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_SCORE_7500    , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_SCORE_10000   , FALSE);
        ra_enable( SUBWINDOW_DIPS, IDC_SCORE_12500   , FALSE);
        ra_enable2(SUBWINDOW_DIPS, IDC_SCORE         , FALSE);
}   }

EXPORT void zaccaria_updatedips(void)
{   FAST int coinagad,
             coinbgad,
             collgad,
             freezegad,
             gridgad,
             hiscoregad,
             livesgad,
             rapidfiregad;

    if
    (   machine != ZACCARIA
     || !SubWindowPtr[SUBWINDOW_DIPS]
    )
    {   return;
    }

    switch (memmap)
    {
    case MEMMAP_ASTROWARS:
        if (aw_dips1 & 8)
        {   livesgad = 3; // 5 lives
        } else
        {   livesgad = 1; // 3 lives
        }
        coinagad      = 1 +  (aw_dips1 & 0x03)               ; // 1..4 (ie. 1, 2, 3 or 5 lives)
        coinbgad      =      (aw_dips1 & 0x04) >> 2          ; // 0..1 (ie. half or 1 credit)
        if (aw_dips2 & 1)
        {   hiscoregad = 1 + ((aw_dips2 & 6) >> 1);
        } else
        {   hiscoregad = 0;
        }
        freezegad     =      (aw_dips1 & 0x80) ? FALSE : TRUE;
        gridgad       =
        rapidfiregad  =
        collgad       = 0;
    acase MEMMAP_GALAXIA:
        if (ga_dips & 8)
        {   livesgad = 3; // 5 lives
        } else
        {   livesgad = 1; // 3 lives
        }
        hiscoregad    = 0;
        coinagad      = 1 +  (ga_dips  & 0x03)               ; // 1..4 (ie. 1, 2, 3 or 5 lives)
        coinbgad      =      (ga_dips  & 0x04) >> 2          ; // 0..1 (ie. half or 1 credit)
        freezegad     =      (ga_dips  & 0x80) ? FALSE : TRUE;
        gridgad       =
        rapidfiregad  =
        collgad       = 0;
    acase MEMMAP_LAZARIAN:
        livesgad      =      (lz_dips2 & 0x30) >> 4          ; // 0..3 (ie. 2..5 lives)
        coinagad      =      (lz_dips2 & 0x03)               ; // 0..3
        coinbgad      = 2 + ((lz_dips2 & 0x0C) >> 2);        ; // 2..5
        freezegad     =      (lz_dips3 & 0x04) ? TRUE : FALSE;
        collgad       =      (lz_dips2 & 0x80) >> 7          ;
        rapidfiregad  =      (lz_dips3 & 0x02) ? TRUE : FALSE;
        gridgad       =      (lz_dips2 & 0x40) ? TRUE : FALSE;
        hiscoregad    = 0;
    acase MEMMAP_LASERBATTLE:
        switch (lb_dips & 0x70)
        {
        case  0x00: livesgad = 0; // 2 lives
        acase 0x10: livesgad = 1; // 3 lives
        acase 0x20: livesgad = 3; // 5 lives
        acase 0x30: livesgad = 4; // 6 lives
        adefault:   livesgad = 5; // infinite lives
        }
        coinagad      = 1 +  (lb_dips & 0x03);
        coinbgad      = 2 + ((lb_dips & 0x0C) >> 2);
        collgad       = (lb_dips & 0x80) >> 7;
        freezegad     =
        gridgad       =
        rapidfiregad  =
        hiscoregad    = 0;
    }

    ra_set(SUBWINDOW_DIPS, IDC_LIVES_2    , IDC_LIVES_INFINITE, livesgad);
    ra_set(SUBWINDOW_DIPS, IDC_COINA_HALFC, IDC_COINA_5C      , coinagad);
    ra_set(SUBWINDOW_DIPS, IDC_COINB_HALFC, IDC_COINB_7C      , coinbgad);
    ra_set(SUBWINDOW_DIPS, IDC_SCORE_0    , IDC_SCORE_12500   , hiscoregad);
    cb_set(SUBWINDOW_DIPS, IDC_COLLISIONS , collgad      ? TRUE : FALSE);
    cb_set(SUBWINDOW_DIPS, IDC_FREEZE     , freezegad    ? TRUE : FALSE);
    cb_set(SUBWINDOW_DIPS, IDC_RAPIDFIRE  , rapidfiregad ? TRUE : FALSE);
    cb_set(SUBWINDOW_DIPS, IDC_CALIBGRID  , gridgad      ? TRUE : FALSE);
}

EXPORT FLAG update_livesval(int value)
{   switch (value)
    {
    case 0: // 2 lives
        switch (memmap)
        {
        case  MEMMAP_LAZARIAN:    LETBITS54BE00(  lz_dips2); return TRUE;
        acase MEMMAP_LASERBATTLE: LETBITS654BE000(lb_dips ); return TRUE;
        }
    acase 1: // 3 lives
        switch (memmap)
        {
        case  MEMMAP_LAZARIAN:    LETBITS54BE01(  lz_dips2); return TRUE;
        acase MEMMAP_ASTROWARS:   LETBIT3BE0(     aw_dips1); return TRUE;
        acase MEMMAP_GALAXIA:     LETBIT3BE0(     ga_dips ); return TRUE;
        acase MEMMAP_LASERBATTLE: LETBITS654BE001(lb_dips ); return TRUE;
        }
    acase 2: // 4 lives
        switch (memmap)
        {
        case  MEMMAP_LAZARIAN:    LETBITS54BE10(  lz_dips2); return TRUE;
        }
    acase 3: // 5 lives
        switch (memmap)
        {
        case  MEMMAP_LAZARIAN:    LETBITS54BE11(  lz_dips2); return TRUE;
        acase MEMMAP_ASTROWARS:   LETBIT3BE1(     aw_dips1); return TRUE;
        acase MEMMAP_GALAXIA:     LETBIT3BE1(     ga_dips ); return TRUE;
        acase MEMMAP_LASERBATTLE: LETBITS654BE010(lb_dips ); return TRUE;
        }
    acase 4: // 6 lives
        switch (memmap)
        {
        case  MEMMAP_LASERBATTLE: LETBITS654BE011(lb_dips ); return TRUE;
        }
    acase 5: // infinite lives
        switch (memmap)
        {
        case  MEMMAP_LASERBATTLE: LETBITS654BE100(lb_dips ); return TRUE;
    }   }

    return FALSE;
}

EXPORT FLAG update_coinaval(int value)
{   switch (value)
    {
    case 0: // ½ credit
        switch (memmap)
        {
        case  MEMMAP_LAZARIAN:    LETBITS10BE00(  lz_dips2); return TRUE;
        }
    acase 1: // 1 credit
        switch (memmap)
        {
        case  MEMMAP_LAZARIAN:    LETBITS10BE01(  lz_dips2); return TRUE;
        acase MEMMAP_LASERBATTLE: LETBITS10BE00(  lb_dips ); return TRUE;
        acase MEMMAP_ASTROWARS:   LETBITS10BE00(  aw_dips1); return TRUE;
        acase MEMMAP_GALAXIA:     LETBITS10BE00(  ga_dips ); return TRUE;
        }
    acase 2: // 2 credits
        switch (memmap)
        {
        case  MEMMAP_LAZARIAN:    LETBITS10BE10(  lz_dips2); return TRUE;
        acase MEMMAP_LASERBATTLE: LETBITS10BE01(  lb_dips ); return TRUE;
        acase MEMMAP_ASTROWARS:   LETBITS10BE01(  aw_dips1); return TRUE;
        acase MEMMAP_GALAXIA:     LETBITS10BE01(  ga_dips ); return TRUE;
        }
    acase 3: // 3 credits
        switch (memmap)
        {
        case  MEMMAP_LAZARIAN:    LETBITS10BE11(  lz_dips2); return TRUE;
        acase MEMMAP_LASERBATTLE: LETBITS10BE10(  lb_dips ); return TRUE;
        acase MEMMAP_ASTROWARS:   LETBITS10BE10(  aw_dips1); return TRUE;
        acase MEMMAP_GALAXIA:     LETBITS10BE10(  ga_dips ); return TRUE;
        }
    acase 4: // 5 credits
        switch (memmap)
        {
        case  MEMMAP_LASERBATTLE: LETBITS10BE11(  lb_dips ); return TRUE;
        acase MEMMAP_ASTROWARS:   LETBITS10BE11(  aw_dips1); return TRUE;
        acase MEMMAP_GALAXIA:     LETBITS10BE11(  ga_dips ); return TRUE;
    }   }

    return FALSE;
}

EXPORT FLAG update_coinbval(int value)
{   switch (value)
    {
    case 0: // ½ credit
        switch (memmap)
        {
        case  MEMMAP_ASTROWARS:   LETBIT2BE0(     aw_dips1); return TRUE;
        acase MEMMAP_GALAXIA:     LETBIT2BE0(     ga_dips ); return TRUE;
        }
    acase 1: // 1 credit
        switch (memmap)
        {
        case  MEMMAP_ASTROWARS:   LETBIT2BE1(     aw_dips1); return TRUE;
        acase MEMMAP_GALAXIA:     LETBIT2BE1(     ga_dips ); return TRUE;
        }
    acase 2: // 2 credits
        switch (memmap)
        {
        case  MEMMAP_LAZARIAN:    LETBITS32BE00(  lz_dips2); return TRUE;
        acase MEMMAP_LASERBATTLE: LETBITS32BE00(  lb_dips ); return TRUE;
        }
    acase 3: // 3 credits
        switch (memmap)
        {
        case  MEMMAP_LAZARIAN:    LETBITS32BE01(  lz_dips2); return TRUE;
        acase MEMMAP_LASERBATTLE: LETBITS32BE01(  lb_dips ); return TRUE;
        }
    acase 4: // 5 credits
        switch (memmap)
        {
        case  MEMMAP_LAZARIAN:    LETBITS32BE10(  lz_dips2); return TRUE;
        acase MEMMAP_LASERBATTLE: LETBITS32BE10(  lb_dips ); return TRUE;
        }
    acase 5: // 7 credits
        switch (memmap)
        {
        case  MEMMAP_LAZARIAN:    LETBITS32BE11(  lz_dips2); return TRUE;
        acase MEMMAP_LASERBATTLE: LETBITS32BE11(  lb_dips ); return TRUE;
    }   }

    return FALSE;
}

EXPORT void update_collval(int value)
{   switch (value)
    {
    case 0:
        switch (memmap)
        {
        case  MEMMAP_LAZARIAN:    LETBIT7BE0(     lz_dips2);
        acase MEMMAP_LASERBATTLE: LETBIT7BE0(     lb_dips );
        }
    acase 1:
        switch (memmap)
        {
        case  MEMMAP_LAZARIAN:    LETBIT7BE1(     lz_dips2);
        acase MEMMAP_LASERBATTLE: LETBIT7BE1(     lb_dips );
}   }   }

EXPORT void update_freezeval(int value)
{   switch (value)
    {
    case 0:
        switch (memmap)
        {
        case  MEMMAP_LAZARIAN:    LETBIT2BE0(     lz_dips3);
        acase MEMMAP_ASTROWARS:   LETBIT7BE1(     aw_dips1);
        acase MEMMAP_GALAXIA:     LETBIT7BE1(     ga_dips );
        }
    acase 1:
        switch (memmap)
        {
        case  MEMMAP_LAZARIAN:    LETBIT2BE1(     lz_dips3);
        acase MEMMAP_ASTROWARS:   LETBIT7BE0(     aw_dips1);
        acase MEMMAP_GALAXIA:     LETBIT7BE0(     ga_dips );
}   }   }

EXPORT void update_rapidfireval(int value)
{   if (memmap == MEMMAP_LAZARIAN)
    {   if (value)
        {   LETBIT1BE1(lz_dips3);
        } else
        {   LETBIT1BE0(lz_dips3);
}   }   }
EXPORT void update_gridval(int value)
{   if (memmap == MEMMAP_LAZARIAN)
    {   if (value)
        {   LETBIT6BE1(lz_dips2);
        } else
        {   LETBIT6BE0(lz_dips2);
}   }   }

EXPORT FLAG update_hiscoreval(int value)
{   if (memmap == MEMMAP_ASTROWARS)
    {   switch (value)
        {
        case  0: LETBITS210BE000(aw_dips2);
        acase 1: LETBITS210BE001(aw_dips2);
        acase 2: LETBITS210BE011(aw_dips2);
        acase 3: LETBITS210BE101(aw_dips2);
        acase 4: LETBITS210BE111(aw_dips2);
        }

        return TRUE;
    }

    return FALSE;
}

/* DIP switch bit allocations are:

                    LB               LZ                 AW                GA
Lives        654 of lb_dips    54 of lz_dips2      3 of aw_dips1     3 of ga_dips
Coin A        10 of lb_dips    10 of lz_dips2     10 of aw_dips1    10 of ga_dips
Coin B        32 of lb_dips    32 of lz_dips2      2 of aw_dips1     2 of ga_dips
Diff.        --------------    ??????????????    ---------------    --------------
Ext. Play    --------------    ??????????????    ---------------    --------------
Coll.          7 of lb_dips     7 of lz_dips2    ---------------    --------------
Freeze       --------------     2 of lz_dips3      7 of aw_dips1     7 of ga_dips
Rapid        --------------     1 of lz_dips3    ---------------    --------------
Grid         --------------     6 of lz_dips2    ---------------    --------------
Hiscores       0 of mux 3      --------------    210 of aw_dips2    210 of port 7? */

EXPORT void view_tms(void)
{   FLAG  firstchan;
    ULONG temp;
    int   i,
          tmsnote[4] = { NOTE_REST, NOTE_REST, NOTE_REST, NOTE_REST };

    // assert(memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN);

    temp =  lb_snd & 0x00001FFF;
    firstchan = TRUE;
    for (i = 1; i <= 13; i++)
    {   if (temp & (1 << (i - 1)))
        {   if (firstchan)
            {   tmsnote[0] = lb_note[i].low;
                firstchan = FALSE;
            } else
            {   tmsnote[1] = lb_note[i].low;
                break;
    }   }   }

    temp = (lb_snd & 0x03FFE000) >> 12;
    firstchan = TRUE;
    for (i = 1; i <= 13; i++)
    {   if (temp & (1 << (i - 1)))
        {   if (firstchan)
            {   tmsnote[2] = lb_note[i].high;
                firstchan = FALSE;
            } else
            {   tmsnote[3] = lb_note[i].high;
                break;
    }   }   }

    zprintf
    (   TEXTPEN_VIEW,
        "1st tone unit (bass):   %s, %s\n2nd tone unit (treble): %s, %s\n\n",
        notes[tmsnote[0]].name,
        notes[tmsnote[1]].name,
        notes[tmsnote[2]].name,
        notes[tmsnote[3]].name
    );
}

EXPORT void zaccaria_drawhelpgrid(void)
{   int a,  b,
        aa, bb,
        starta,
        startb;

    for (b = 2; b < 32; b++)        // Y-axis from guest's viewpoint,          X-axis from user's viewpoint
    {   for (a = 0; a < 30; a++)    // X-axis from guest's viewpoint, inverted Y-axis from user's viewpoint
        {   starta = a * 8;         // from guest's viewpoint, X-start of this character (ie. left edge)
            startb = (b - 2) * 8;   // from guest's viewpoint, Y-start of this character (ie. top  edge)
            if (memmap == MEMMAP_GALAXIA && starta >= 32 && starta <= 111)
            {   startb += galaxia_scrolly;
            }
            for (aa = 7; aa >= 0; aa--)
            {   for (bb = 0; bb < 8; bb++)
                {   if (aa == 7 || aa == 0 || bb == 0 || bb == 7)
                    {   changepixel(starta + aa, startb + bb, GREY1);
}   }   }   }   }   }

#define KXLIMIT 31
#define KYLIMIT 31
EXPORT FLAG zaccaria_edit_screen(UWORD code)
{   int   i,
          kx, ky;
    UBYTE blank;

    kx =  scrnaddr           % 32;
    ky = (scrnaddr - 0x1800) / 32;
    if   (memmap == MEMMAP_ASTROWARS) blank = 0x20;
    elif (memmap == MEMMAP_GALAXIA  ) blank = 0x3F;
    else                              blank = 0xFF;

    switch (code)
    {
    case  SCAN_UP:
        if (rotating)            if (shift()) kx =       0; elif (kx ==  0) kx = KXLIMIT; else kx--;
        else                     if (shift()) ky =       0; elif (ky ==  0) ky = KYLIMIT; else ky--;
    acase SCAN_DOWN:
        if (rotating)            if (shift()) kx = KXLIMIT; elif (kx == KXLIMIT) kx =  0; else kx++;
        else                     if (shift()) ky = KYLIMIT; elif (ky == KYLIMIT) ky =  0; else ky++;
    acase SCAN_LEFT:
        if (rotating)            if (shift()) ky = KYLIMIT; elif (ky == KYLIMIT) ky =  0; else ky++;
        else                     if (shift()) kx =       0; elif (kx ==  0) kx = KXLIMIT; else kx--;
    acase SCAN_RIGHT:
        if (rotating)            if (shift()) ky =       0; elif (ky ==  0) ky = KYLIMIT; else ky--;
        else                     if (shift()) kx = KXLIMIT; elif (kx == KXLIMIT) kx =  0; else kx++;
    acase SCAN_BACKSPACE:        if (kx == 0 && ky == 0)
                                 {   kx = KXLIMIT;
                                     ky = KYLIMIT;
                                 } elif (rotating)
                                 {   if (ky == KYLIMIT)
                                     {   ky = 0;
                                         kx--;
                                     } else
                                     {   ky++;
                                 }   }
                                 else
                                 {   if (kx == 0)
                                     {   kx = KXLIMIT;
                                         ky--;
                                     } else
                                     {   kx--;
                                 }   }
                                 scrnaddr = 0x1800 + kx + (ky * 32);
                                 memory[scrnaddr] = blank;
    acase SCAN_DEL:              if (shift())
                                 {   for (i = 0x1800; i <= 0x1BFF; i++)
                                     {   memory[i] = blank;
                                 }   }
                                 else
                                 {   memory[scrnaddr] = blank;
                                 }
    acase SCAN_AE: case SCAN_NE: kx = 0;
                                 if (ky == KYLIMIT)
                                 {   ky = 0;
                                 } else
                                 {   ky++;
                                 }
    acase SCAN_OB:               if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
                                 {   g_bank1[scrnaddr - 0x1800]--; setstatus("Decrement foreground colour"); // underflow is OK
                                 }
    acase SCAN_CB:               if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
                                 {   g_bank1[scrnaddr - 0x1800]++; setstatus("Increment foreground colour"); // overflow is OK
                                 }
#ifdef WIN32
    acase SCAN_HOME:             kx =  0; if (shift()) ky =  0;
    acase SCAN_END:              kx = KXLIMIT; if (shift()) ky = KYLIMIT;
    acase SCAN_PGUP:             memory[scrnaddr]++; setstatus("Increment screen memory"); // overflow  is OK
    acase SCAN_PGDN:             memory[scrnaddr]--; setstatus("Decrement screen memory"); // underflow is OK
#endif
#ifdef AMIGA
    acase SCAN_HELP:             if (shift())
                                 {   memory[scrnaddr]--; // underflow is OK
                                     setstatus("Decrement screen memory");
                                 } else
                                 {   memory[scrnaddr]++; // overflow  is OK
                                     setstatus("Increment screen memory");
                                 }
#endif
    acase SCAN_ESCAPE:           editscreen = FALSE;
                                 updatemenus();
                                 settitle();
                              // dogamename();
    adefault:
        switch (code)
        {
        case  SCAN_LSHIFT:
        case  SCAN_RSHIFT:
        case  SCAN_CAPSLOCK:
            return FALSE;
        case  SCAN_A0:
            if (memmap == MEMMAP_ASTROWARS && shift())
            {   memory[scrnaddr] = 0x73;
            } elif (memmap == MEMMAP_GALAXIA)
            {   memory[scrnaddr] = 0xE0;
            } else
            {   memory[scrnaddr] = 0xB0;
            }
        acase SCAN_A1:
            if (memmap == MEMMAP_ASTROWARS && shift())
            {   memory[scrnaddr] = 0x74;
            } elif (memmap == MEMMAP_GALAXIA)
            {   memory[scrnaddr] = shift() ? 0x97 : 0xE1;
            } else
            {   memory[scrnaddr] = 0xB1;
            }
        acase SCAN_A2:
            if (memmap == MEMMAP_ASTROWARS && shift())
            {   memory[scrnaddr] = 0x75;
            } elif (memmap == MEMMAP_GALAXIA)
            {   memory[scrnaddr] = shift() ? 0x98 : 0xE2;
            } else
            {   memory[scrnaddr] = 0xB2;
            }
        acase SCAN_A3:
            if (memmap == MEMMAP_ASTROWARS && shift())
            {   memory[scrnaddr] = 0x76;
            } elif (memmap == MEMMAP_GALAXIA)
            {   memory[scrnaddr] = shift() ? 0x99 : 0xE3;
            } else
            {   memory[scrnaddr] = 0xB3;
            }
        acase SCAN_A4:
            if (memmap == MEMMAP_ASTROWARS && shift())
            {   memory[scrnaddr] = 0x77;
            } elif (memmap == MEMMAP_GALAXIA)
            {   memory[scrnaddr] = shift() ? 0x9A : 0xE4;
            } else
            {   memory[scrnaddr] = 0xB4;
            }
        acase SCAN_A5:
            if (memmap == MEMMAP_ASTROWARS && shift())
            {   memory[scrnaddr] = 0x78;
            } elif (memmap == MEMMAP_GALAXIA)
            {   memory[scrnaddr] = shift() ? 0x9B : 0xE5;
            } else
            {   memory[scrnaddr] = 0xB5;
            }
        acase SCAN_A6:
            if (memmap == MEMMAP_GALAXIA)
            {   memory[scrnaddr] = shift() ? 0x9C : 0xE6;
            } else
            {   memory[scrnaddr] = 0xB6;
            }
        acase SCAN_A7: case SCAN_N7: memory[scrnaddr] = (memmap == MEMMAP_GALAXIA) ? 0xE7 : 0xB7;
        acase SCAN_A8: case SCAN_N8: memory[scrnaddr] = (memmap == MEMMAP_GALAXIA) ? 0xE8 : 0xB8;
        acase SCAN_A9: case SCAN_N9: memory[scrnaddr] = (memmap == MEMMAP_GALAXIA) ? 0xE9 : 0xB9;
        acase SCAN_N0:               memory[scrnaddr] = (memmap == MEMMAP_GALAXIA) ? 0xE0 : 0xB0;
        acase SCAN_N1:               memory[scrnaddr] = (memmap == MEMMAP_GALAXIA) ? 0xE1 : 0xB1;
        acase SCAN_N2:               memory[scrnaddr] = (memmap == MEMMAP_GALAXIA) ? 0xE2 : 0xB2;
        acase SCAN_N3:               memory[scrnaddr] = (memmap == MEMMAP_GALAXIA) ? 0xE3 : 0xB3;
        acase SCAN_N4:               memory[scrnaddr] = (memmap == MEMMAP_GALAXIA) ? 0xE4 : 0xB4;
        acase SCAN_N5:               memory[scrnaddr] = (memmap == MEMMAP_GALAXIA) ? 0xE5 : 0xB5;
        acase SCAN_N6:               memory[scrnaddr] = (memmap == MEMMAP_GALAXIA) ? 0xE6 : 0xB6;
        acase SCAN_A:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xC1 : 0xC0;
        acase SCAN_B:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xC2 : 0xC1;
        acase SCAN_C:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xC3 : 0xC2;
        acase SCAN_D:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xC4 : 0xC3;
        acase SCAN_E:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xC5 : 0xC4;
        acase SCAN_F:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xC6 : 0xC5;
        acase SCAN_G:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xC7 : 0xC6;
        acase SCAN_H:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xC8 : 0xC7;
        acase SCAN_I:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xC9 : 0xC8;
        acase SCAN_J:                if (memmap == MEMMAP_GALAXIA) return FALSE; else memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS) ? 0xCA : 0xC9;
        acase SCAN_K:                if (memmap == MEMMAP_GALAXIA) return FALSE; else memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS) ? 0xCB : 0xCA;
        acase SCAN_L:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xCC : 0xCB;
        acase SCAN_M:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xCD : 0xCC;
        acase SCAN_N:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xCE : 0xCD;
        acase SCAN_O:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xCF : 0xCE;
        acase SCAN_P:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xD0 : 0xCF;
        acase SCAN_Q:                if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) return FALSE; else memory[scrnaddr] = 0xD0;
        acase SCAN_R:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xD2 : 0xD1;
        acase SCAN_S:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xD3 : 0xD2;
        acase SCAN_T:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xD4 : 0xD3;
        acase SCAN_U:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xD5 : 0xD4;
        acase SCAN_V:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xD6 : 0xD5;
        acase SCAN_W:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xD7 : 0xD6;
        acase SCAN_X:                if (memmap == MEMMAP_GALAXIA) return FALSE; else memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS) ? 0xD8 : 0xD7;
        acase SCAN_Y:                memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) ? 0xD9 : 0xD8;
        acase SCAN_Z:                if (memmap == MEMMAP_GALAXIA) return FALSE; else memory[scrnaddr] = (memmap == MEMMAP_ASTROWARS) ? 0xDA : 0xD9;
        acase SCAN_SPACEBAR:
            if   (memmap == MEMMAP_ASTROWARS) memory[scrnaddr] = shift() ? 0xEB : 0x20;
            elif (memmap == MEMMAP_GALAXIA  ) memory[scrnaddr] =                  0x3F;
            else                              memory[scrnaddr] = shift() ? 0x14 : 0xFF;
        adefault:                    return FALSE;
        }
        if (kx == KXLIMIT && ky == KYLIMIT)
        {   kx = ky = 0;
        } elif (rotating)
        {   if (ky == 0)
            {   ky = KYLIMIT;
                kx++;
            } else
            {   ky--;
        }   }
        else
        {   if (kx == KXLIMIT)
            {   kx = 0;
                ky++;
            } else
            {   kx++;
    }   }   }
    scrnaddr = 0x1800 + kx + (ky * 32);

    return TRUE; // return code is whether we ate the keystroke
}
