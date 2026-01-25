// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
    #include <stdlib.h> // eg. for rand()
#endif
#ifdef WIN32
    #include "ibm.h"
#endif

#include <stdio.h> // for FILE*

#include "da.h"
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
#define LETBIT7BE0(a)      a &= 0x7F;            // %1xxxxxxx

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

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT UBYTE                circle,
                            coinops_colltable[3][PVI_RASTLINES][PVI_XSIZE],
                            ioport[258],
                            memflags[32768],
                            memory[32768];
IMPORT ASCREEN              screen[BOXWIDTH][BOXHEIGHT];
IMPORT UWORD                pc,
                            mirror_r[32768],
                            mirror_w[32768];
IMPORT ULONG                analog,
                            collisions,
                            swapped;
IMPORT int                  coinop_1p,
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
                            hostcontroller[2],
                            game,
                            key1,
                            key2,
                            key3,
                            key4,
                            layout,
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
                            region,
                            teletype,
                            throb,
                            useff[2];
IMPORT UBYTE*               IOBuffer;
IMPORT struct MachineStruct machines[MACHINES];

// MODULE VARIABLES-------------------------------------------------------

MODULE int                  numstars,
                            starx[CVS_MAX_STARS],
                            stary[CVS_MAX_STARS];
MODULE const UBYTE*         tilesptr;

// MODULE ARRAYS----------------------------------------------------------

MODULE const UBYTE galaxia_colours[4][4] = {
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

// MODULE FUNCTIONS-------------------------------------------------------

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
        {   memflags[address] |= BANKED;
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
    ioport[PORTD] = 0;

    machines[ZACCARIA].pvis = 1;
    pvibase = 0x1500;
    pvi_memmap();

    key1             =
    key2             =
    key3             =
    key4             = 2;
    analog           =
    swapped          = FALSE;
    layout           = W2H;
   
    for (i = 0; i < 4; i++)
    {   p1bgcol[i] =
        p2bgcol[i] = 0;
    }
    for (i = 0; i < 6; i++)
    {   p1sprcol[i] =
        p2sprcol[i] = 0;
    }

    set_cpl(227 / 3); // 75.6'
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

        if  (address >= 0x1800 && address <= 0x1BFF) // RAM
        {   memflags[address] |= BANKED;
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
    ioport[PORTD] = 0;

    machines[ZACCARIA].pvis = 3;
    pvibase = 0x1500;
    pvi_memmap();
    pvibase = 0x1600;
    pvi_memmap();
    pvibase = 0x1700;
    pvi_memmap();
    pvibase = 0x1500;

    key1          =
    key2          =
    key3          =
    key4          = 2;
    analog        =
    swapped       = FALSE;
    layout        = W2H;

    for (i = 0; i < 4; i++)
    {   p1bgcol[i] =
        p2bgcol[i] = 0;
    }
    for (i = 0; i < 6; i++)
    {   p1sprcol[i] =
        p2sprcol[i] = 0;
    }

    set_cpl(227 / 3); // 75.6'
    machines[machine].cpf = (int) ((227.0 / 3.0) * 312.0);
    region = REGION_PAL;

    init_cvs_stars();

    squeal   = 0;
    throb    = 0;
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
        {   memflags[address] |= BANKED;
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

    machines[ZACCARIA].pvis = 3;
    pvibase = 0x1500;
    pvi_memmap();
    pvibase = 0x1600;
    pvi_memmap();
    pvibase = 0x1700;
    pvi_memmap();
    pvibase = 0x1500;

    analog  =
    swapped = FALSE;
    key1 = 7;
    key2 = 8;
    key3 = 9;
    key4 = 5;
    layout = W8;

    for (i = 0; i < 4; i++)
    {   p1bgcol[i] =
        p2bgcol[i] = 0;
    }
    for (i = 0; i < 6; i++)
    {   p1sprcol[i] =
        p2sprcol[i] = 0;
    }

    set_cpl(227 / 3); // 75.6'
    machines[machine].cpf = (int) ((227.0 / 3.0) * 312.0);
    region = REGION_PAL;
}

EXPORT void astrowars_drawscreen(void)
{   PERSIST int a, aa, // X-axis from guest's viewpoint, inverted Y-axis from user's viewpoint
                b, bb, // Y-axis from guest's viewpoint,          X-axis from user's viewpoint
                contents,
                localoffset,
                starta,
                startb;

    // assert(memmap == MEMMAP_ASTROWARS);

    // Each character is 8*8 pixels (8 bytes).
    // Character PROM is 2K (with 256 characters, each of 8 bytes).

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
            contents = (memory[0x1800 + (b * 32) + a] & 0x7F) << 3;
            for (aa = 7; aa >= 0; aa--)
            {   ourstartb = startb;
                if (starta + aa >= 32 && starta + aa <= 111)
                {   ourstartb += galaxia_scrolly;
                }
                for (bb = 0; bb < 8; bb++)
                {   if
                    (   starta    + aa <  0
                     || starta    + aa >= COINOP_BOXWIDTH
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
{   PERSIST int   colour,
                  x, xx,
                  y, yy;
    PERSIST UBYTE t;

    // assert(memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN);

    for (x = 0; x < COINOP_BOXWIDTH; x++)
    {   for (y = 0; y < COINOP_BOXHEIGHT; y++)
        {   changepixel(x, y, BLACK);
    }   }

    for (y = 0; y < COINOP_BOXHEIGHT; y++)
    {   UBYTE eff1_val = memory_effects[((y + 16) & 0xff) | 0x100];
	    UBYTE eff2_val = memory_effects[((y + 16) & 0xff) | 0x000];
	    UBYTE eff1_cmp, eff2_cmp;
        FLAG  eff1;
        FLAG  eff2;
        FLAG  shell;

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
                case  3: changepixel(x - 8, y, BLUE);
                acase 2: changepixel(x - 8, y, abeff1 ? RED   : YELLOW);
                acase 1: changepixel(x - 8, y, GREEN); // unused?
                acase 0: changepixel(x - 8, y, BLACK);
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
                case  3: changepixel(x - 8, y, BLUE);
                acase 2: changepixel(x - 8, y, abeff1 ? RED : YELLOW);
                acase 1: changepixel(x - 8, y, GREEN); // unused?
                acase 0: changepixel(x - 8, y, BLACK);
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
}   }   }   }   }   }

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

    return t;
}

EXPORT void zaccaria_writeport(int port, UBYTE data)
{   FAST FLAG sounding;

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
            if ((data & 4) && !(ioport[PORTD] & 4))
            {   play_sample(SAMPLE_SHOOT);
            }
            if ((data & 2) && !(ioport[PORTD] & 2))
            {   play_sample(SAMPLE_BANG);
            }
            ioport[PORTD] = data;
        } elif (memmap == MEMMAP_GALAXIA)
        {   /* Bits 7..6: unused?
               Bits 5..3: background throbbing?
               Bit  2:    fire (player)
               Bit  1:    die (player)
               Bit  0:    dive (enemy)? */
            sounding = FALSE;
            if ((data & 8) != (ioport[PORTD] & 8))
            {   throb = (data & 8) ? 1 : 0;
                sounding = TRUE;
            }
            if ((data & 4) && !(ioport[PORTD] & 4))
            {   play_sample(SAMPLE_SHOOT);
            }
            if ((data & 2) && !(ioport[PORTD] & 2))
            {   play_sample(SAMPLE_BANG);
            }
            if (data & 1)
            {   if (!(ioport[PORTD] & 1))
                {   squeal = 0xFF;
                    sounding = TRUE;
            }   }
            else
            {   if (  ioport[PORTD] & 1 )
                {   squeal = 0;
                    sounding = TRUE;
            }   }
            ioport[PORTD] = data;
            if (sounding)
            {   playsound(FALSE);
}   }   }   }

EXPORT void zaccaria_emuinput(void)
{   TRANSIENT UBYTE t;
    TRANSIENT int   old_coina, old_coinb;
    PERSIST   int   stale[4] = { 0, 0, 0, 0 };

        old_coina        = coinop_coina;
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

        coinop_emuinput();
        p1rumble =
        p2rumble = 0;

        if (console_start)
        {   if (!stale[0])
            {   coinop_1p |= 1; // 1UP
            }
            stale[0] = 1;
        } else
        {   stale[0] = 0;
        }

        if (console_a)
        {   if (!stale[1])
            {   coinop_2p |= 1; // 2UP
            }
            stale[1] = 1;
        } else
        {   stale[1] = 0;
        }

        if (console_b)
        {   if (!stale[2])
            {   coinop_coina |= 1; // generous coin slot A (1C, 2C, 3C or 5C)
                if (!(old_coina & 1))
                {   play_sample(SAMPLE_COIN);
            }   }
            stale[2] = 1;
        } else
        {   stale[2] = 0;
        }

        if (console_reset)
        {   if (!stale[3])
            {   coinop_coinb |= 1; // miserly coin slot B (½C or 1C)
                if (!(old_coinb & 1))
                {   play_sample(SAMPLE_COIN);
            }   }
            stale[3] = 1;
        } else
        {   stale[3] = 0;
        }

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
