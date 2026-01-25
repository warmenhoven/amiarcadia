// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
    #include <string.h>
    #include <stdlib.h>
    #include <proto/locale.h> // GetCatalogStr()
#endif
#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
#endif

#include "da.h"
#include "arcadia.h"

// DEFINES----------------------------------------------------------------

#define UDCFLIPS         7

#define UVIXTOSCREENX(x) (USG_XMARGIN + x - absxmin)
#define UVIYTOSCREENY(y) (USG_YMARGIN + y - absymin)

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       ULONG                 fractionator;
EXPORT       int                   frac[4];

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       ULONG                 autofire[2],
                                   collisions,
                                   cycles_2650,
                                   demultiplex,
                                   downframes,
                                   frames,
                                   hinput[2],
                                   oldcycles,
                                   region,
                                   swapped,
                                   totalframes;
IMPORT       UBYTE*                IOBuffer;
IMPORT       int                   absxmin, absxmax,
                                   absymin, absymax,
                                   cpl,
                                   console_start,
                                   console_a,
                                   console_b,
                                   console_reset,
                                   cpux,
                                   cpuy,
                                   dejitter,
                                   filesize,
                                   firstrow,
                                   game,
                                   hidetop,
                                   key1,
                                   key2,
                                   key3,
                                   key4,
                                   lastrow,
                                   linebased,
                                   machine,
                                   memmap,
                                   n1, n2, n3, n4,
                                   nextinst,
                                   offset,
                                   p1bgcol[4],
                                   p2bgcol[4],
                                   p1sprcol[6],
                                   p2sprcol[6],
                                   paddleup,
                                   paddledown,
                                   paddleleft,
                                   paddleright,
                                   p1rumble,
                                   p2rumble,
                                   romsize,
                                   rumbling[2],
                                   serializemode,
                                   slice_2650,
                                   supercpu,
                                   udcflips,
                                   undither,
                                   useff[2],
                                   whichgame;
IMPORT       ASCREEN               screen[BOXWIDTH][BOXHEIGHT];
IMPORT       UBYTE                 hx[2], hy[2],
                                   memory[32768],
                                   memflags[32768],
                                   opcode,
                                   psu;
IMPORT       UWORD                 iar,
                                   mirror_r[32768],
                                   mirror_w[32768],
                                   screen_iar[BOXWIDTH][BOXHEIGHT];
IMPORT const struct KnownStruct    known[KNOWNGAMES];
IMPORT       struct MachineStruct  machines[MACHINES];

#ifdef WIN32
    IMPORT       int               CatalogPtr;
#endif
#ifdef AMIGA
    IMPORT       struct Catalog*   CatalogPtr;
#endif
#ifdef BENCHMARK_GFX
    IMPORT       ULONG             cycles_2650;
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE int    endsprx[4],
              endspry[4],
              flag_cacheable,
              hiresnum,
              hoffset,
              hoffsetstart,
              hoffsetend,
              oldhoffset[UDCFLIPS + 1][26],
              uviy, uviy2,
              voffsetend;
MODULE FLAG   spritesdone;
MODULE UBYTE  bgc,
              bgc_cached,
              boardmode,
              colltable[312][227],
              fgc1, fgc2,         // only used for board mode. Arcadia format
              outerbgc, innerbgc, // Arcadia format
              hires,              // 0 for low resolution, 1 for high resolution
              rowbuf[UDCFLIPS + 1][26][16],
              scrnbgc,
              sprimagedata[4],
              udgimgbuf[UDCFLIPS + 1][8][8],
              t;
MODULE SLONG  minorrow, // goes up only 1 per 2 rasts in low-res mode
                        // 0..7: which y-pixel row in the current "band" (row of chars) we are doing
              majorrow, // whenever minorrow gets to 8, minorrow is zeroed and majorrow is incremented
              blockmode,
              column,
              rowlimit;
MODULE const UBYTE multicolour_robotkil[8][8] = {
{ YELLOW, BLACK , BLUE  , BLACK , YELLOW, BLACK , BLUE  , BLACK },
{ BLACK , GREEN , BLACK , RED   , BLACK , GREEN , BLACK , RED   },
{ BLUE  , BLACK , YELLOW, BLACK , BLUE  , BLACK , YELLOW, BLACK },
{ BLACK , RED   , BLACK , GREEN , BLACK , RED   , BLACK , GREEN },
{ YELLOW, BLACK , BLUE  , BLACK , YELLOW, BLACK , BLUE  , BLACK },
{ BLACK , GREEN , BLACK , RED   , BLACK , GREEN , BLACK , RED   },
{ BLUE  , BLACK , YELLOW, BLACK , BLUE  , BLACK , YELLOW, BLACK },
{ BLACK , RED   , BLACK , GREEN , BLACK , RED   , BLACK , GREEN }
}, multicolour_escape[8][8] = {
{ GREEN , BLACK , BLUE  , BLACK , GREEN , BLACK , BLUE  , BLACK },
{ BLACK , RED   , BLACK , PURPLE, BLACK , RED   , BLACK , PURPLE},
{ BLUE  , BLACK , GREEN , BLACK , BLUE  , BLACK , BLUE  , BLACK },
{ BLACK , PURPLE, YELLOW, RED   , YELLOW, PURPLE, YELLOW, BLACK },
{ GREEN , BLACK , BLUE  , BLACK , GREEN , BLACK , BLUE  , BLACK },
{ BLACK , RED   , BLACK , PURPLE, BLACK , RED   , BLACK , PURPLE},
{ BLUE  , BLACK , GREEN , BLACK , BLUE  , BLACK , GREEN , BLACK },
{ BLACK , PURPLE, BLACK , RED   , BLACK , PURPLE, BLACK , RED   }
};

MODULE struct
{   int   x,
          y;
    UBYTE imagery[8],
          colour;
    FLAG  tall;
} spr[FLIPS][4];

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       FLAG  protect[4];
EXPORT       UBYTE editcolour = 0x00;
EXPORT       int   flagline = TRUE,
                   hoffsets[26] = { 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
                                    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23 },
                   spriteflip,
                   spriteflips,
                   udcflip,
                   voffset;
EXPORT const UBYTE from_a[2][24] =
{ { 0, //  0 ->  0 white
    1, //  1 ->  1 yellow
    4, //  2 ->  4 cyan
    5, //  3 ->  5 green
    2, //  4 ->  2 purple
    3, //  5 ->  3 red
    6, //  6 ->  6 blue
    7, //  7 ->  7 black
    8,  9, 10, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23
  },
  { 7, //  0 ->  7 black
    6, //  1 ->  6 blue
    3, //  2 ->  3 red
    2, //  3 ->  2 purple
    5, //  4 ->  5 green
    4, //  5 ->  4 cyan
    1, //  6 ->  1 yellow
    0, //  7 ->  0 white
    8,  9, 10, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23
} };

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void a_playerinput(int source, int dest);
MODULE void vblank(void);
MODULE void unvblank(void);
MODULE void drawfakesprites(void);
MODULE void drawfakeudgs(void);
MODULE void newdma(void);
MODULE void newraster(void);
MODULE __inline void a_emuinput(void);
MODULE __inline void do_cpu(void);
MODULE __inline void onepixel(void);
MODULE __inline void do_sprites1(void);
MODULE __inline void do_sprites2(void);
MODULE void setpaddle(int player, int which);
MODULE void arcadia_oneraster(void);
MODULE void run_cpu(int until);
MODULE void arcadia_runcpu(void);

// CODE-------------------------------------------------------------------

EXPORT void uvi(void)
{   FAST int   whichudg,
               kx, ky,
               x, x1, x2,
               y, y1, y2;
    FAST UBYTE ogc,
               tgc;

    // assert(machine == ARCADIA);

    voffset = voffsetend = 999;
    flag_cacheable = (flagline && (psu & PSU_F)) ? 1 : 0;
    bgc_cached     = memory[A_BGCOLOUR];

    if (linebased)
    {   spritesdone = FALSE;
        cpuy = 0;
        vblank();

        for (cpux = 0; cpux < n4; cpux++)
        {   changethisbgpixel(bgc);
        }

        for (cpuy = 0; cpuy < n1; cpuy++)
        {   arcadia_runcpu();
        }

        unvblank();

        for (cpuy = n1; cpuy < n2; cpuy++)
        {   arcadia_oneraster();
            arcadia_runcpu();
        }

        a_emuinput();

        for (cpuy = n2; cpuy < n3; cpuy++)
        {   arcadia_oneraster();
            arcadia_runcpu();
        }

        if (!spritesdone)
        {   do_sprites2();
    }   }
    else
    {   // scanline 0---------------------------------------------------------
        cpuy = 0;
        for (cpux = 0; cpux < n4; cpux++) // small amount of semi-normal drawing
        {   if (!dejitter)
            {   flag_cacheable = (flagline && (psu & PSU_F)) ? 1 : 0;
            }
            changethisbgpixel(from_a[flag_cacheable][bgc_cached & 0x07]);
            do_cpu();
        }
        vblank();
        run_cpu(227);
    
        // scanlines 1..42 (PAL) or 1..19 (NTSC)------------------------------
        for (cpuy = 1; cpuy < n1; cpuy++) // in vertical blank
        {   run_cpu(227);
        }

        // scanline 43 (PAL) or 20 (NTSC)-------------------------------------
        cpuy = n1;
        run_cpu(n4); // small amount of vertical blanking
        unvblank();
        flag_cacheable = (flagline && (psu & PSU_F)) ? 1 : 0;
        bgc_cached = memory[A_BGCOLOUR];
        scrnbgc = from_a[flag_cacheable][bgc_cached & 0x07];
        newraster();
        run_cpu(49);
        for (cpux = 49; cpux < 227; cpux++) // normal drawing
        {   onepixel();
        }

        // scanlines 44..311 (PAL) or 21..261 (NTSC)--------------------------
        for (cpuy = n1 + 1; cpuy < n3; cpuy++)
        {   for (cpux = 0; cpux < n4; cpux++) // small amount of semi-normal drawing
            {   if (!dejitter)
                {   flag_cacheable = (flagline && (psu & PSU_F)) ? 1 : 0;
                }
                changethisbgpixel(from_a[flag_cacheable][bgc_cached & 0x07]);
                do_cpu();
            }
            if (cpuy == n2) // ie. USG_YMARGIN + ((n3 - USGMARGIN) / 2). "Conversion takes place during the active scan".
            {   a_emuinput();
            }
            newraster();
            run_cpu(49);
            for (cpux = 49; cpux < 227; cpux++)
            {   onepixel();
    }   }   }

    for (whichudg = 0; whichudg < 8; whichudg++)
    {   for (y = 0; y < 8; y++)
        {   udgimgbuf[udcflip][whichudg][y] = memory[A_OFFSET_SPRITES + (whichudg * 8) + y];
    }   }

    drawfakesprites();
    if (spriteflip >= spriteflips)
    {   spriteflip = 0;
    } else spriteflip++;

    drawfakeudgs();
    if (udcflip >= udcflips)
    {   udcflip = 0;
    } else
    {   udcflip++;
}   }

MODULE __inline void a_emuinput(void)
{   FAST int i,
             player;

    for (i = 0x1900; i <= 0x1908; i++)
    {   memory[i] = 0;
    }
    for (player = 0; player < 2; player++)
    {   if (hinput[player] & (1 <<  0)) memory[A_P1LEFTKEYS   + (player * 4)] |= 0x08; // "1"
        if (hinput[player] & (1 <<  3)) memory[A_P1LEFTKEYS   + (player * 4)] |= 0x04; // "4"
        if (hinput[player] & (1 <<  6)) memory[A_P1LEFTKEYS   + (player * 4)] |= 0x02; // "7"
        if (hinput[player] & (1 <<  9)) memory[A_P1LEFTKEYS   + (player * 4)] |= 0x01; // "Cl"
        if (hinput[player] & (1 <<  1)) memory[A_P1MIDDLEKEYS + (player * 4)] |= 0x08; // "2"
        if (hinput[player] & (1 <<  4)) memory[A_P1MIDDLEKEYS + (player * 4)] |= 0x04; // "5"
        if (hinput[player] & (1 <<  7)) memory[A_P1MIDDLEKEYS + (player * 4)] |= 0x02; // "8"
        if (hinput[player] & (1 << 10)) memory[A_P1MIDDLEKEYS + (player * 4)] |= 0x01; // "0"
        if (hinput[player] & (1 <<  2)) memory[A_P1RIGHTKEYS  + (player * 4)] |= 0x08; // "3"
        if (hinput[player] & (1 <<  5)) memory[A_P1RIGHTKEYS  + (player * 4)] |= 0x04; // "6"
        if (hinput[player] & (1 <<  8)) memory[A_P1RIGHTKEYS  + (player * 4)] |= 0x02; // "9"
        if (hinput[player] & (1 << 11)) memory[A_P1RIGHTKEYS  + (player * 4)] |= 0x01; // "En"
        if (memory[A_BGCOLOUR] & 0x40) // paddle interpolation bit
        {   memory[A_P1PADDLE - player] = hx[player];
        } else
        {   memory[A_P1PADDLE - player] = hy[player];
        }
        if   (hy[player] <=  64) setpaddle(player, paddleup);
        elif (hy[player] >= 192) setpaddle(player, paddledown);
        if   (hx[player] <=  64) setpaddle(player, paddleleft);
        elif (hx[player] >= 192) setpaddle(player, paddleright);
    }
        
    if (console_start) { memory[A_CONSOLE] |= 0x01; console_start--; } // START
    if (console_a    ) { memory[A_CONSOLE] |= 0x02; console_a--;     } // A
    if (console_b    ) { memory[A_CONSOLE] |= 0x04; console_b--;     } // B
    if (console_reset) { iar = 0;                   console_reset--; } // RESET

    p1rumble = p2rumble = 0;
}

EXPORT void uviwrite(UWORD address, UBYTE data)
{   // assert(address < 32768);

    memory[address] = data;
}

EXPORT void arcadia_setmemmap(void)
{   int i, address, mirror;

    game = FALSE;

    for (i = 0; i <= 0x7FFF; i++)
    {   memory[i] = 0;
    }
    for (i = 0x1909; i <= 0x190F; i++)
    {   memory[i] = 0x40;
    }

    for (i = 0; i < 348; i++)
    {   memory[i] = a_bios[i];
    }
    for (i = 0x19C0; i <= 0x19F7; i++)
    {   memory[i] = 0xFF; // unmapped
    }

    for (address = 0; address < 32 * KILOBYTE; address++)
    {   memflags[address] = 0;
        mirror            = address;

        if (memmap == MEMMAP_I)
        {   if (address < 0x1800 || address > 0x1BFF)
            {   memflags[address] |= NOWRITE;
        }   }
        else
        {   // assert(memmap == MEMMAP_G || memmap == MEMMAP_H);

            if
            (    address <= 0x0FFF                       // ROM
     // legal/not occurring: $1000..$17FF                   extra RAM and/or mirrored
        // legal:            $1800..$18CF                   upper screen
        // legal:            $18D0..$18EF                   user RAM
        // legal:            $18F0..$18F7                   R/W hardware registers
        // legal:            $18F8..$18FB                   user RAM
        // legal:            $18FC                          R/W hardware register
        // legal:            $18FD                          -/W hardware register
        // legal:            $18FE                          R/W hardware register
             || (address >= 0x18FF && address <= 0x197F) // CHARLINE, PnXKEYS/PALLADIUM, CONSOLE, unmapped
        // legal:            $1980..$19BF                   R/W hardware registers
             || (address >= 0x19C0 && address <= 0x19F7) // unmapped
        // legal:            $19F8..$19FB                   -/W hardware registers
             || (address >= 0x19FC && address <= 0x19FF) // BGCOLLIDE, SPRITECOLLIDE, PnPADDLE
        // legal:            $1A00..$1ACF                   lower screen
        // legal:            $1AD0..$1AFF                   user RAM
             ||  address >= 0x1B00                       // not occurring, ROM
            )
            {   memflags[address] |= NOWRITE;
        }   }

        if
        (
        // legal:                $0000              ..$0FFF     ROM
        // legal/not occurring:  $1000              ..$17FF     extra RAM and/or mirrored
        // legal:                $1800              ..$18CF     upper screen
        // legal:                $18D0              ..$18EF     user RAM
        // legal:                $18F0              ..$18F7     R/W hardware regs
        // legal:                $18F8              ..$18FB     user RAM
        // legal:                $18FC              ..$18FE     R/W hardware regs
        // legal:                $18FF              ..$1908     R/- hardware regs
                    (address >= 0x1909 && address <= 0x190F) // unmapped (apparently reads as $40 or $C0)
        // not occurring:        $1910              ..$197F     mirrored
        // legal:                $1980              ..$19BF     R/W hardware regs
                 || (address >= 0x19C0 && address <= 0x19FB) // unmapped (apparently reads as $FF), -/W hardware regs
        // legal:                $19FC              ..$19FF     read-once hardware regs
        // legal:                $1A00              ..$1ACF     lower screen
        // legal:                $1AD0              ..$1AFF     user RAM
        // not occurring:        $1B00              ..$1FFF     mirrored
        // legal:                $2000              ..$2FFF     ROM
        // not occurring:        $3000              ..$3FFF     mirrored
        // not occurring:        $4000              ..$4FFF     mirrored?
        // not occurring:        $5000              ..$5FFF     mirrored
        // not occurring:        $6000              ..$6FFF     mirrored?
        // not occurring:        $7000              ..$7FFF     mirrored
        )
        {   memflags[address] |= NOREAD;
        }

        if (address >= 0x19FC && address <= 0x19FF) // read-once: $19FC..$19FF: PnPADDLE, SPRITECOLLIDE, BGCOLLIDE
        {   memflags[address] |= READONCE;
        }

        if (address == A_PITCH || address == A_VOLUME)
        {   memflags[address] |= AUDIBLE;
        }

        // G,H: The only accesses that are NOT mirrored are those to:
        // $0000..$10FF (with memory map "H")
        // $1200..$12FF (with memory map "H")
        // $1800..$19FF
        // $1A00..$1AFF (with memory maps "G" and "H")
        // $2000..$2FFF
        // I:   All is ROM except $1800..$1BFF.

        if (memmap == MEMMAP_I)
        {   if (mirror >= 0x1B00 && mirror <= 0x1BFF)
            {   mirror -= 0x200; // $1Bxx -> $19xx
        }   }
        else
        {   // assert(memmap == MEMMAP_G || memmap == MEMMAP_H);

            if (mirror & 0x1000) // $1xxx, $3xxx, $5xxx, $7xxx
            {   mirror &= 0x1FFF; // -> $1xxx (mask of %0001 1111 1111 1111)

                if (mirror & 0x100) // $x1xx, $x3xx, $x5xx, $x7xx, $x9xx, $xBxx, $xDxx, $xFxx
                {   mirror &= 0x11FF; // -> $11xx
                    mirror += 0x800;  // -> $19xx
                }

                if (mirror >= 0x1400 && mirror <= 0x17FF)
                {   mirror -= 0x400; // -> $1000..$13FF
                } elif (mirror >= 0x1C00 && mirror <= 0x1FFF)
                {   mirror -= 0x400; // -> $1800..$1AFF
                }

                if (memmap == MEMMAP_G)
                {   if (mirror >= 0x1000 && mirror <= 0x13FF)
                    {   mirror += 0x800; // -> $1800..$1BFF
                }   }

                if (mirror >= 0x1F10 && mirror <= 0x1F7F)
                {   mirror &= 0x1F0F; // $1F10..$1F7F -> $1F00..$1F0F
            }   }
            else // else is just for speed
            {   if ((mirror & 0x5000) == 0x4000) // $4xxx, $6xxx
                {   mirror &= 0xFFF; // -> $0xxx
        }   }   }

        mirror_r[address] =
        mirror_w[address] = (UWORD) mirror;
    }

    nextinst = 0;
    set_cpl(227);
}

EXPORT void arcadia_serializerom(void)
{   int i;

    /* This is for Palladium (ie. memmap "I"). Emerson ("G") and Tele-
       Fever ("H") are different (see EA2001 Coding Guide). */

    // assert(machine == ARCADIA);
    // assert(romsize <= 31 * KILOBYTE);
    // assert(serializemode == SERIALIZE_READ);

    if (romsize <= 4 * KILOBYTE)
    {   for (i = 0; i < romsize; i++)
        {   serialize_byte(&memory[i]);                // file $0000..$0FFF -> memory $0000..$0FFF
        }
        goto DONE;
    }
    for (i = 0; i < 4 * KILOBYTE; i++)
    {   serialize_byte(&memory[i]);                    // file $0000..$0FFF -> memory $0000..$0FFF
    }

    if (romsize <= 8 * KILOBYTE)
    {   for (i = 0; i < romsize - (4 * KILOBYTE); i++)
        {   serialize_byte(&memory[0x2000 + i]);       // file $1000..$1FFF -> memory $2000..$2FFF
        }
        goto DONE;
    }
    for (i = 0; i < 4 * KILOBYTE; i++)
    {   serialize_byte(&memory[0x2000 + i]);           // file $1000..$1FFF -> memory $2000..$2FFF
    }

    if (romsize <= 12 * KILOBYTE)
    {   for (i = 0; i < romsize - (8 * KILOBYTE); i++)
        {   serialize_byte(&memory[0x4000 + i]);       // file $2000..$2FFF -> memory $4000..$4FFF
        }
        goto DONE;
    }
    for (i = 0; i < 4 * KILOBYTE; i++)
    {   serialize_byte(&memory[0x4000 + i]);           // file $2000..$2FFF -> memory $4000..$4FFF
    }

    // arbitrary...

    if (romsize <= 16 * KILOBYTE)
    {   for (i = 0; i < romsize - (12 * KILOBYTE); i++)
        {   serialize_byte(&memory[0x6000 + i]);       // file $3000..$3FFF -> memory $6000..$6FFF
        }
        goto DONE;
    }
    for (i = 0; i < 4 * KILOBYTE; i++)
    {   serialize_byte(&memory[0x6000 + i]);           // file $3000..$3FFF -> memory $6000..$6FFF
    }

    if (romsize <= 20 * KILOBYTE)
    {   for (i = 0; i < romsize - (16 * KILOBYTE); i++)
        {   serialize_byte(&memory[0x3000 + i]);       // file $4000..$4FFF -> memory $3000..$3FFF
        }
        goto DONE;
    }
    for (i = 0; i < 4 * KILOBYTE; i++)
    {   serialize_byte(&memory[0x3000 + i]);           // file $4000..$4FFF -> memory $3000..$3FFF
    }

    if (romsize <= 24 * KILOBYTE)
    {   for (i = 0; i < romsize - (20 * KILOBYTE); i++)
        {   serialize_byte(&memory[0x5000 + i]);       // file $5000..$5FFF -> memory $5000..$5FFF
        }
        goto DONE;
    }
    for (i = 0; i < 4 * KILOBYTE; i++)
    {   serialize_byte(&memory[0x5000 + i]);           // file $5000..$5FFF -> memory $5000..$5FFF
    }

    if (romsize <= 28 * KILOBYTE)
    {   for (i = 0; i < romsize - (24 * KILOBYTE); i++)
        {   serialize_byte(&memory[0x7000 + i]);       // file $6000..$6FFF -> memory $7000..$7FFF
        }
        goto DONE;
    }
    for (i = 0; i < 4 * KILOBYTE; i++)
    {   serialize_byte(&memory[0x7000 + i]);           // file $6000..$6FFF -> memory $7000..$7FFF
    }

    if (romsize <= 30 * KILOBYTE)
    {   for (i = 0; i < romsize - (28 * KILOBYTE); i++)
        {   serialize_byte(&memory[0x1000 + i]);       // file $7000..$77FF -> memory $1000..$17FF
        }
        goto DONE;
    }
    for (i = 0; i < 2 * KILOBYTE; i++)
    {   serialize_byte(&memory[0x1000 + i]);           // file $7000..$77FF -> memory $1000..$17FF
    }

    for (i = 0; i < romsize - (30 * KILOBYTE); i++)
    {   serialize_byte(&memory[0x1C00 + i]);           // file $7800..$7BFF -> memory $1C00..$1FFF
    }

DONE:
    if (whichgame == A_GOLFPOS1 || whichgame == A_GOLFODPOS)
    {   // This is for unpatched Arcadia Golf
        // It mirrors $2000..$27FF at $4000..$47FF
        for (i = 0; i < 2 * KILOBYTE; i++)
        {   memory[0x4000 + i] = memory[0x2000 + i];
}   }   }

MODULE __inline void onepixel(void)
{   FAST    int   x;
    PERSIST UBYTE imagedata,
                  thechar;

    if (!dejitter)
    {   flag_cacheable = (flagline && (psu & PSU_F)) ? 1 : 0;
    }
    bgc = from_a[flag_cacheable][bgc_cached & 0x07];

    if (uviy < voffset || uviy >= voffsetend || cpux < hoffsetstart || cpux >= hoffsetend)
    {   changethisbgpixel(bgc);
    } else
    {   x = (cpux - hoffsetstart) & 7; // or % 8
        if (x == 0)
        {   column = (cpux - hoffsetstart) >> 3; // or / 8
            if (column == 0)
            {   blockmode = (memory[A_GFXMODE] & 0x80) >> 7;
                if (hires)
                {   minorrow =  uviy2 &   7      ; // or  uviy2 %  8
                } else
                {   minorrow = (uviy2 & 0xF) >> 1; // or (uviy2 % 16) / 2
            }   }
            thechar = rowbuf[udcflip][majorrow][column];
            if (thechar == 0xC0)
            {   blockmode = 1;
            } elif (thechar == 0x40)
            {   blockmode = 0;
            }
            if (!blockmode && (thechar & 0x3F) >= 56) // this is a UDG
            {   imagedata = memory[A_OFFSET_SPRITES + (8 * ((thechar & 0x3F) - 56)) + minorrow];
            } else // this is a PDG
            {   imagedata = arcadia_pdg[blockmode][thechar & 0x3F][minorrow];
        }   }

        if (boardmode)
        {   if (imagedata & (0x80 >> x))
            {   t = from_a[flag_cacheable][(thechar & 0x40) ? fgc2 : fgc1];
                changethisabspixel(t);
            } elif (thechar & 0x80)
            {   t = from_a[flag_cacheable][innerbgc];
                changethisabspixel(t);
            } else
            {   t = from_a[flag_cacheable][outerbgc];
                changethisbgpixel(t);
        }   }
        else
        {   if (imagedata & (0x80 >> x))
            {   t = from_a[flag_cacheable][((thechar &  0xC0) >> 5) + ((bgc_cached >> 3) & 1)];
                if
                (   undither
                 && (whichgame == ESCAPEPOS || whichgame == ROBOTKILLERPOS)
                 && !flag_cacheable
                 && thechar == 0x3F
                )
                {   switch (whichgame)
                    {
                    case  ESCAPEPOS:      changethisabspixel(multicolour_escape[  minorrow][x]);
                    acase ROBOTKILLERPOS: changethisabspixel(multicolour_robotkil[minorrow][x]);
                }   }
                else
                {   changethisabspixel(t);
            }   }
            else
            {   changethisbgpixel(bgc);
    }   }   }

    do_sprites1();
    do_cpu();
}

MODULE __inline void do_sprites1(void)
{   FAST    UBYTE sprcollisions,
                  sprcolours,
                  sprpixel;
    FAST    int   whichsprite;

    // Sprites----------------------------------------------------

    sprpixel = sprcolours = 0;

    if
    (   cpux >= spr[spriteflip][0].x
     && cpux <= endsprx[0]
     && cpuy >= spr[spriteflip][0].y
     && cpuy <= endspry[0]
     && (sprimagedata[0] & (0x80 >> (cpux - spr[spriteflip][0].x)))
    )
    {   sprpixel   |= 1;
        sprcolours |= spr[spriteflip][0].colour;
    }
    if
    (   cpux >= spr[spriteflip][1].x
     && cpux <= endsprx[1]
     && cpuy >= spr[spriteflip][1].y
     && cpuy <= endspry[1]
     && (sprimagedata[1] & (0x80 >> (cpux - spr[spriteflip][1].x)))
    )
    {   sprpixel   |= 2;
        sprcolours |= spr[spriteflip][1].colour;
    }
    if
    (   cpux >= spr[spriteflip][2].x
     && cpux <= endsprx[2]
     && cpuy >= spr[spriteflip][2].y
     && cpuy <= endspry[2]
     && (sprimagedata[2] & (0x80 >> (cpux - spr[spriteflip][2].x)))
    )
    {   sprpixel   |= 4;
        sprcolours |= spr[spriteflip][2].colour;
    }
    if
    (   cpux >= spr[spriteflip][3].x
     && cpux <= endsprx[3]
     && cpuy >= spr[spriteflip][3].y
     && cpuy <= endspry[3]
     && (sprimagedata[3] & (0x80 >> (cpux - spr[spriteflip][3].x)))
    )
    {   sprpixel   |= 8;
        sprcolours |= spr[spriteflip][3].colour;
    }

    if (sprpixel)
    {   if (collisions)
        {   sprcollisions = 0;
            if ((sprpixel & 3) == 3) // %0011 #0 & #1
            {   sprcollisions |= 1;
                if (p1sprcol[0]) p1rumble = p1sprcol[0];
                if (p2sprcol[0]) p2rumble = p2sprcol[0];
                // zprintf(TEXTPEN_VERBOSE, "Sprites #0 & #1 have collided!\n");
            }
            if ((sprpixel & 5) == 5) // %0101 #0 & #2
            {   sprcollisions |= 2;
                if (p1sprcol[1]) p1rumble = p1sprcol[1];
                if (p2sprcol[1]) p2rumble = p2sprcol[1];
                // zprintf(TEXTPEN_VERBOSE, "Sprites #0 & #2 have collided!\n");
            }
            if ((sprpixel & 9) == 9) // %1001 #0 & #3
            {   sprcollisions |= 4;
                if (p1sprcol[2]) p1rumble = p1sprcol[2];
                if (p2sprcol[2]) p2rumble = p2sprcol[2];
                // zprintf(TEXTPEN_VERBOSE, "Sprites #0 & #3 have collided!\n");
            }
            if ((sprpixel & 6) == 6) // %0110 #1 & #2
            {   sprcollisions |= 8;
                if (p1sprcol[3]) p1rumble = p1sprcol[3];
                if (p2sprcol[3]) p2rumble = p2sprcol[3];
                // zprintf(TEXTPEN_VERBOSE, "Sprites #1 & #2 have collided!\n");
            }
            if ((sprpixel & 10) == 10) // %1010 #1 & #3
            {   sprcollisions |= 16;
                if (p1sprcol[4]) p1rumble = p1sprcol[4];
                if (p2sprcol[4]) p2rumble = p2sprcol[4];
                // zprintf(TEXTPEN_VERBOSE, "Sprites #1 & #3 have collided!\n");
            }
            if ((sprpixel & 12) == 12) // %1100 #2 & #3
            {   sprcollisions |= 32;
                if (p1sprcol[5]) p1rumble = p1sprcol[5];
                if (p2sprcol[5]) p2rumble = p2sprcol[5];
                // zprintf(TEXTPEN_VERBOSE, "Sprites #2 & #3 have collided!\n");
            }

            // Are these written (a) now, or (b) at the end of the line, or (c) at the end of the frame?
            if (sprcollisions)
            {   uviwrite((UWORD) A_SPRITECOLLIDE, (UBYTE) (memory[A_SPRITECOLLIDE] & (~sprcollisions)));
            }
            if (screen[cpux - absxmin][cpuy - absymin] != bgc) // we assume it works like this even in board mode
            {   uviwrite((UWORD) A_BGCOLLIDE,     (UBYTE) (memory[A_BGCOLLIDE    ] & (~sprpixel)));
                for (whichsprite = 0; whichsprite < 4; whichsprite++)
                {   if (sprpixel & (1 << whichsprite))
                    {   if (useff[swapped ? 1 : 0])
                        {   if (p1bgcol[whichsprite] > 0)
                            {   p1rumble = p1bgcol[whichsprite];
                                // zprintf(TEXTPEN_VERBOSE, "p1 background rumble!\n");
                            } elif (p1bgcol[whichsprite] < 0)
                            {   p1rumble = -p1bgcol[whichsprite];
                        }   }
                        if (useff[swapped ? 0 : 1])
                        {   if (p2bgcol[whichsprite] > 0)
                            {   p2rumble = p2bgcol[whichsprite];
                                // zprintf(TEXTPEN_VERBOSE, "p2 background rumble!\n");
                            } elif (p2bgcol[whichsprite] < 0)
                            {   p2rumble = -p2bgcol[whichsprite];
        }   }   }   }   }   }

        changethisabspixel(from_a[flag_cacheable][sprcolours]);
}   }

MODULE void newdma(void)
{   FAST int x;

    majorrow = uviy2 / hiresnum;
    if (majorrow != rowlimit)
    {   boardmode = memory[A_PITCH] & 0x80; // Bowling relies on boardmode being refreshed each DMA
        if (boardmode)
        {   fgc1     = (memory[A_GFXMODE ] & 0x38) >> 3; // %00111000 -> %00000111
            fgc2     = (bgc_cached         & 0x38) >> 3; // %00111000 -> %00000111 colours of tile set 0
            outerbgc =  memory[A_GFXMODE ] & 0x07      ; // %00000111
            innerbgc =  bgc_cached         & 0x07      ; // %00000111              background colour
        }

        // Red Clash relies on the row of characters being cached
        if (majorrow <= 12)
        {   for (x = 0; x < 16; x++)
            {   rowbuf[udcflip][majorrow][x] = memory[0x1800 + (majorrow << 4) + x];
        }   }
        elif (majorrow <= 25)
        {   for (x = 0; x < 16; x++)
            {   rowbuf[udcflip][majorrow][x] = memory[0x1930 + (majorrow << 4) + x];
        }   }

        uviwrite(A_CHARLINE, (UBYTE) (0xF0 | (majorrow % 13)));
        oldhoffset[udcflip][majorrow] = hoffset = hoffsets[majorrow] = 23 + ((memory[A_VOLUME] & 0xE0) >> 5); // 3 bits wide
        hoffsetstart = 49 + hoffset;
        hoffsetend   = hoffsetstart + 128;
    } else
    {   if (linebased)
        {   do_sprites2(); // because CIRCUS checks for collisions as soon as CHARLINE becomes $FD
            spritesdone = TRUE;
        }
        uviwrite(A_CHARLINE, 0xFD);
}   }

MODULE void vblank(void)
{   psu |= PSU_S;

    uviwrite((UWORD) A_CHARLINE, 0xFD);
}

MODULE void unvblank(void)
{   FAST int whichsprite,
             y;

    psu &= ~(PSU_S);

    uviwrite(A_CHARLINE, 0xFF);
    uviwrite(A_P1PADDLE, 0xFF); // "The potentiometer is set to $FF
    uviwrite(A_P2PADDLE, 0xFF); // on the trailing edge of VRST."

    // "Each [bit of BGCOLLIDE and SPRITECOLLIDE] is reset [to 1]...at the end of the vertical blanking period."
    if (whichgame != SPACEATTACKBPOS && whichgame != SPACEATTACKCPOS) // these games read them on rasters 113..115
    {   // this is needed for eg. Super Bug
        uviwrite(A_BGCOLLIDE,     0xFF);
        uviwrite(A_SPRITECOLLIDE, 0xFF);
    }

    voffset      = (SWORD) 255 - memory[A_VSCROLL];
    if (memory[A_GFXMODE] & 0x40) // lower screen on/off flag
    {   rowlimit = 26;
    } else
    {   rowlimit = 13;
    }
    bgc_cached   =  memory[A_BGCOLOUR  ];
    hires        = (memory[A_BGCOLOUR  ] & 0x80) >> 7; // %10000000 -> %00000001 (doublescan flag)
    hiresnum     = (hires ? 8 : 16);
    voffsetend   = voffset + (rowlimit * hiresnum);
    hoffset      = 23      + ((memory[A_VOLUME] & 0xE0) >> 5); // 3 bits wide
    hoffsetstart = 49 + hoffset;
    hoffsetend   = hoffsetstart + 128;

    // "The vertical and horizontal coordinates of the four
    // objects and vertical offset will be accessed at DMA 15."
    spr[spriteflip][0].x      =           memory[A_SPRITE0X] - 20 + 49;
    spr[spriteflip][0].y      =     256 - memory[A_SPRITE0Y]      + n1;
    spr[spriteflip][1].x      =           memory[A_SPRITE1X] - 20 + 49;
    spr[spriteflip][1].y      =     256 - memory[A_SPRITE1Y]      + n1;
    spr[spriteflip][2].x      =           memory[A_SPRITE2X] - 20 + 49;
    spr[spriteflip][2].y      =     256 - memory[A_SPRITE2Y]      + n1;
    spr[spriteflip][3].x      =           memory[A_SPRITE3X] - 20 + 49;
    spr[spriteflip][3].y      =     256 - memory[A_SPRITE3Y]      + n1;

    spr[spriteflip][0].colour = (UBYTE) ((memory[A_SPRITES01CTRL] & 0x38) >> 3); // %00111000 -> %00000111
    spr[spriteflip][1].colour = (UBYTE) ( memory[A_SPRITES01CTRL] & 0x07      ); // %00000111 -> %00000111
    spr[spriteflip][2].colour = (UBYTE) ((memory[A_SPRITES23CTRL] & 0x38) >> 3); // %00111000 -> %00000111
    spr[spriteflip][3].colour = (UBYTE) ( memory[A_SPRITES23CTRL] & 0x07      ); // %00000111 -> %00000111
    if (whichgame == DRSLUMPPOS)
    {   if (memory[A_SPRITE2Y] >= 0xC0 && spr[spriteflip][2].colour == 6)
        {   spr[spriteflip][2].colour = 1; // blue -> yellow
        }
        if (memory[A_SPRITE3Y] >= 0xC0 && spr[spriteflip][3].colour == 6)
        {   spr[spriteflip][3].colour = 1; // blue -> yellow
    }   }

    spr[spriteflip][0].tall   =     1 - ((memory[A_SPRITES01CTRL] & 0x80) >> 7); // %10000000 -> %00000001
    spr[spriteflip][1].tall   =     1 - ((memory[A_SPRITES01CTRL] & 0x40) >> 6); // %01000000 -> %00000001
    spr[spriteflip][2].tall   =     1 - ((memory[A_SPRITES23CTRL] & 0x80) >> 7); // %10000000 -> %00000001
    spr[spriteflip][3].tall   =     1 - ((memory[A_SPRITES23CTRL] & 0x40) >> 6); // %01000000 -> %00000001

    endsprx[0] = spr[spriteflip][0].x +                                 7 ;
    endsprx[1] = spr[spriteflip][1].x +                                 7 ;
    endsprx[2] = spr[spriteflip][2].x +                                 7 ;
    endsprx[3] = spr[spriteflip][3].x +                                 7 ;
    endspry[0] = spr[spriteflip][0].y + (spr[spriteflip][0].tall ? 15 : 7);
    endspry[1] = spr[spriteflip][1].y + (spr[spriteflip][1].tall ? 15 : 7);
    endspry[2] = spr[spriteflip][2].y + (spr[spriteflip][2].tall ? 15 : 7);
    endspry[3] = spr[spriteflip][3].y + (spr[spriteflip][3].tall ? 15 : 7);

    for (whichsprite = 0; whichsprite < 4; whichsprite++)
    {   for (y = 0; y < 8; y++)
        {   spr[spriteflip][whichsprite].imagery[y] = memory[A_OFFSET_SPRITES + (whichsprite * 8) + y];
}   }   }

MODULE void drawfakesprites(void)
{   FAST FLAG  localflagging;
    FAST UBYTE imagedata;
    FAST int   flipper,
               i,
               whichsprite,
               x, xx, y, yy;

    if (!spriteflips || !demultiplex || frames < (ULONG) spriteflips)
    {   return;
    }

    localflagging = (flagline && (psu & PSU_F));

#define LEFTMOST  USG_XMARGIN
#define RIGHTMOST 226

    for (i = 1; i <= spriteflips; i++)
    {   flipper = (spriteflip + i) % (spriteflips + 1);
        if (flipper == spriteflip)
        {   continue;
        } // implied else

        for (whichsprite = 0; whichsprite < 4; whichsprite++)
        {   if (!protect[whichsprite])
            {   x = spr[flipper][whichsprite].x;
                y = spr[flipper][whichsprite].y;

                // This anti-smearing code is good for Baseball with lots of runners on the bases, and for Macross title screen,
                // but bad for (at least) the Red Clash title screen.
                if
                (   (whichgame != BASEBALLPOS && whichgame != MACROSSPOS)
                 || spr[spriteflip][whichsprite].x <= x -  8
                 || spr[spriteflip][whichsprite].x >= x +  8
                 || spr[spriteflip][whichsprite].y <= y - 16
                 || spr[spriteflip][whichsprite].y >= y + 16
                )
                {   if (!spr[flipper][whichsprite].tall)
                    {   for (yy = 0; yy < 8; yy++)
                        {   if (y + yy >= USG_YMARGIN && y + yy < n3)
                            {   imagedata = spr[flipper][whichsprite].imagery[yy];
                                for (xx = 0; xx < 8; xx++)
                                {   if (x + xx >= LEFTMOST && x + xx <= RIGHTMOST)
                                    {   if (imagedata & (0x80 >> xx))
                                        {   changeabspixel(x + xx, y + yy, from_a[localflagging][spr[flipper][whichsprite].colour]);
                    }   }   }   }   }   }
                    else
                    {   for (yy = 0; yy < 8; yy++)
                        {   if (y + (yy * 2) >= USG_YMARGIN && y + (yy * 2) < n3)
                            {   imagedata = spr[flipper][whichsprite].imagery[yy];
                                for (xx = 0; xx < 8; xx++)
                                {   if (x + xx >= LEFTMOST && x + xx <= RIGHTMOST)
                                    {   if (imagedata & (0x80 >> xx))
                                        {   changeabspixel(x + xx, y + (yy * 2), from_a[localflagging][spr[flipper][whichsprite].colour]);
                                }   }   }
                                if (y + (yy * 2) + 1 >= USG_YMARGIN && y + (yy * 2) + 1 < n3)
                                {   for (xx = 0; xx < 8; xx++)
                                    {   if (x + xx >= LEFTMOST && x + xx <= RIGHTMOST)
                                        {   if (imagedata & (0x80 >> xx))
                                            {   changeabspixel(x + xx, y + (yy * 2) + 1, from_a[localflagging][spr[flipper][whichsprite].colour]);
}   }   }   }   }   }   }   }   }   }   }   }

MODULE void drawfakeudgs(void)
{   FAST FLAG  ok;
    FAST int   column,
               localflagging,
               flipper,
               i,
               row,
               x, y,
               xx, yy;
    FAST UBYTE fgc,
               imagedata,
               thechar;
    // These have similar names and purposes to some MODULE variables,
    // but we must use separate variables so that the values of such
    // MODULE variables are preserved.

    if (!udcflips || !demultiplex || frames < (ULONG) udcflips)
    {   return;
    }

    localflagging = (flagline && (psu & PSU_F)) ? 1 : 0;

    if (whichgame == SPACESQUADRON1POS || whichgame == SPACESQUADRON2POS) // demultiplex radar display
    {   for (y = 0; y < 8; y++)
        {   for (x = 0; x < 40; x++)
            {   imagedata = memory[0x1A30 + ((x / 8) * 8) + y];
                if (imagedata & (0x80 >> (x % 8)))
                {   xx = oldhoffset[udcflip][0] + 40 + x;
                    yy = voffset + (y * 2);

                    changerelpixel(    xx    , yy    , BLACK);
                    changerelpixel(    xx    , yy + 1, BLACK);
    }   }   }   }

    /* We draw the new frame first (during the normal emulation),
       then we draw all the other frames (ie. the old ones) on top now.
       So we only draw where it is black, to avoid overwriting new
       graphics with old graphics (eg. the bomb on level 3 of Macross). */

    for (i = 1; i <= udcflips; i++)
    {   flipper = (udcflip + i) % (udcflips + 1);
        if (flipper == udcflip)
        {   continue;
        } // implied else

        for (row = firstrow; row <= lastrow; row++)
        {   for (column = 0; column < 16; column++)
            {   thechar =  rowbuf[flipper][row][column];
                fgc     =  from_a[localflagging][  ((thechar            &  0xC0) >> 5)   // %11000000 -> %00000110 (0, 2, 4 or 6)
                                                 + ((memory[A_BGCOLOUR] >>    3) &  1)]; // 0 or 1
                thechar &= 0x3F;

                // anti-smearing check (helps Space Squadron)
                ok = TRUE;
                for (y = 0; y < 8; y++)
                {   if (hires) yy = voffset + (row *  8) +  y;
                    else       yy = voffset + (row * 16) + (y * 2);
                    if (yy < n3 - n1)
                    {   if (thechar <= 55)
                        {   imagedata = arcadia_pdg[0][thechar][y];
                        } else
                        {   imagedata = udgimgbuf[flipper][thechar - 56][y];
                        }
                        for (x = 0; x < 8; x++)
                        {   if (imagedata & (0x80 >> x))
                            {   xx = oldhoffset[flipper][row] + (column * 8) + x;
                                if (screen[UVIXTOSCREENX(xx)][UVIYTOSCREENY(yy)] == fgc)
                                {   ok = FALSE;
                                    break; // for speed
                }   }   }   }   }
                if (!ok)
                {   continue;
                }

                for (y = 0; y < 8; y++)
                {   if (hires) yy = voffset + (row *  8) +  y;
                    else       yy = voffset + (row * 16) + (y * 2);
                    if (yy < n3 - n1)
                    {   if (thechar <= 55)
                        {   imagedata = arcadia_pdg[0][thechar][y];
                        } else
                        {   imagedata = udgimgbuf[flipper][thechar - 56][y];
                        }

                        for (x = 0; x < 8; x++)
                        {   if (imagedata & (0x80 >> x))
                            {   xx = oldhoffset[flipper][row] + (column * 8) + x;
                                changerelpixel(        xx    , yy    ,     fgc);
                                if (!hires && yy < n3 - n1 - 1)
                                {   changerelpixel(    xx    , yy + 1,     fgc);
}   }   }   }   }   }   }   }   }

MODULE void newraster(void)
{   uviy  = cpuy - n1;
    uviy2 = uviy - voffset;
    if (uviy >= voffset && uviy <= voffsetend && uviy2 % hiresnum == 0)
    {   newdma();
    }
    if (memory[A_CHARLINE] == 0xFD || memory[A_CHARLINE] == 0xFF)
    {   bgc_cached = memory[A_BGCOLOUR];
    }

    if (!linebased)
    {   sprimagedata[0] = spr[spriteflip][0].imagery[(cpuy - spr[spriteflip][0].y) >> (spr[spriteflip][0].tall ? 1 : 0)];
        sprimagedata[1] = spr[spriteflip][1].imagery[(cpuy - spr[spriteflip][1].y) >> (spr[spriteflip][1].tall ? 1 : 0)];
        sprimagedata[2] = spr[spriteflip][2].imagery[(cpuy - spr[spriteflip][2].y) >> (spr[spriteflip][2].tall ? 1 : 0)];
        sprimagedata[3] = spr[spriteflip][3].imagery[(cpuy - spr[spriteflip][3].y) >> (spr[spriteflip][3].tall ? 1 : 0)];
    }
        
    if (dejitter)
    {   if (whichgame != -1)
        {   if (region == REGION_NTSC)
            {   if (cpuy == (int) known[whichgame].the1staddr || cpuy == (int) known[whichgame].the2ndaddr)
                {   flag_cacheable = (flagline && (psu & PSU_F)) ? 1 : 0;
            }   }
            else
            {   // assert(region == REGION_PAL);
                if (cpuy == (int) known[whichgame].the1stsize || cpuy == (int) known[whichgame].the2ndsize)
                {   flag_cacheable = (flagline && (psu & PSU_F)) ? 1 : 0;
}   }   }   }   }

MODULE __inline void do_cpu(void)
{
#ifdef OPCOLOURS
    if (cpuy >= absymin && cpuy <= absymax && cpux >= absxmin && cpux <= absxmax)
    {   changepixel(cpux - absxmin, cpuy - absymin, table_opcolours_2650[supercpu][opcode]);
    }
    screen_iar[cpux][cpuy] = iar;
#endif

    if (cpux == nextinst)
    {   oldcycles = cycles_2650;
        one_instruction();
        nextinst += (cycles_2650 - oldcycles) * 4; // in pixels
        if (nextinst >= 227)
        {   nextinst -= cpl;
}   }   }

MODULE void setpaddle(int player, int which)
{   switch (which)
    {
    case   0: memory[A_P1MIDDLEKEYS + (player * 4)] |= 0x01; // "0"
    acase  1: memory[A_P1LEFTKEYS   + (player * 4)] |= 0x08; // "1"
    acase  2: memory[A_P1MIDDLEKEYS + (player * 4)] |= 0x08; // "2"
    acase  3: memory[A_P1RIGHTKEYS  + (player * 4)] |= 0x08; // "3"
    acase  4: memory[A_P1LEFTKEYS   + (player * 4)] |= 0x04; // "4"
    acase  5: memory[A_P1MIDDLEKEYS + (player * 4)] |= 0x04; // "5"
    acase  6: memory[A_P1RIGHTKEYS  + (player * 4)] |= 0x04; // "6"
    acase  7: memory[A_P1LEFTKEYS   + (player * 4)] |= 0x02; // "7"
    acase  8: memory[A_P1MIDDLEKEYS + (player * 4)] |= 0x02; // "8"
    acase  9: memory[A_P1RIGHTKEYS  + (player * 4)] |= 0x02; // "9"
    acase 10: memory[A_P1LEFTKEYS   + (player * 4)] |= 0x01; // "Cl"
    acase 11: memory[A_P1RIGHTKEYS  + (player * 4)] |= 0x01; // "En"
}   }

MODULE void arcadia_oneraster(void)
{   FAST UBYTE imagedata,
               thechar;
    FAST int   bgc2,
               x;

    newraster();

    if (!dejitter)
    {   flag_cacheable = (flagline && (psu & PSU_F)) ? 1 : 0;
    }
    bgc = from_a[flag_cacheable][bgc_cached & 0x07];

    if (cpuy != n1)
    {   for (cpux = 0; cpux < n4; cpux++)
        {   changethisbgpixel(bgc);
            colltable[cpuy][cpux] = 0;
    }   }

    for (cpux = 49; cpux < hoffsetstart; cpux++)
    {   changethisbgpixel(bgc);
        colltable[cpuy][cpux] = 0;
    }

    if (uviy < voffset || uviy >= voffsetend)
    {   for (cpux = hoffsetstart; cpux < 227; cpux++)
        {   changethisbgpixel(bgc);
            colltable[cpuy][cpux] = 0;
        }
        return;
    }

    cpux = hoffsetstart;
    blockmode = (memory[A_GFXMODE] & 0x80) >> 7;
    if (hires)
    {   minorrow =  uviy2 &   7      ; // or  uviy2 %  8
    } else
    {   minorrow = (uviy2 & 0xF) >> 1; // or (uviy2 % 16) / 2
    }
    
    for (column = 0; column < 16; column++)
    {   thechar = rowbuf[udcflip][majorrow][column];
        if (thechar == 0xC0)
        {   blockmode = 1;
        } elif (thechar == 0x40)
        {   blockmode = 0;
        }
        if (!blockmode && (thechar & 0x3F) >= 56) // this is a UDG
        {   imagedata = memory[A_OFFSET_SPRITES + (8 * ((thechar & 0x3F) - 56)) + minorrow];
        } else // this is a PDG
        {   imagedata = arcadia_pdg[blockmode][thechar & 0x3F][minorrow];
        }

        if (boardmode)
        {   t    = from_a[flag_cacheable][ (thechar & 0x40) ? fgc2     : fgc1    ];
            bgc2 = from_a[flag_cacheable][ (thechar & 0x80) ? innerbgc : outerbgc];
        } else
        {   t    = from_a[flag_cacheable][((thechar & 0xC0) >> 5) + ((bgc_cached >> 3) & 1)];
            bgc2 = bgc;
        }

        if
        (   undither
         && (whichgame == ESCAPEPOS || whichgame == ROBOTKILLERPOS)
         && !flag_cacheable
         && thechar == 0x3F
         && !boardmode
        )
        {   for (x = 0; x < 8; x++, cpux++)
            {   if (imagedata & (0x80 >> x))
                {   switch (whichgame)
                    {
                    case  ESCAPEPOS:      changethisabspixel(multicolour_escape[  minorrow][x]);
                    acase ROBOTKILLERPOS: changethisabspixel(multicolour_robotkil[minorrow][x]);
                    }
                    colltable[cpuy][cpux] = 0x10;
                } else
                {   changethisbgpixel(bgc);
                    colltable[cpuy][cpux] = 0;
        }   }   }
        else
        {   // unrolled for speed
            if (imagedata & 0x80) { colltable[cpuy][cpux] = 0x10; changethisabspixel(t); } else { colltable[cpuy][cpux] = 0; changethisbgpixel(bgc2); } cpux++;
            if (imagedata & 0x40) { colltable[cpuy][cpux] = 0x10; changethisabspixel(t); } else { colltable[cpuy][cpux] = 0; changethisbgpixel(bgc2); } cpux++;
            if (imagedata & 0x20) { colltable[cpuy][cpux] = 0x10; changethisabspixel(t); } else { colltable[cpuy][cpux] = 0; changethisbgpixel(bgc2); } cpux++;
            if (imagedata & 0x10) { colltable[cpuy][cpux] = 0x10; changethisabspixel(t); } else { colltable[cpuy][cpux] = 0; changethisbgpixel(bgc2); } cpux++;
            if (imagedata & 0x08) { colltable[cpuy][cpux] = 0x10; changethisabspixel(t); } else { colltable[cpuy][cpux] = 0; changethisbgpixel(bgc2); } cpux++;
            if (imagedata & 0x04) { colltable[cpuy][cpux] = 0x10; changethisabspixel(t); } else { colltable[cpuy][cpux] = 0; changethisbgpixel(bgc2); } cpux++;
            if (imagedata & 0x02) { colltable[cpuy][cpux] = 0x10; changethisabspixel(t); } else { colltable[cpuy][cpux] = 0; changethisbgpixel(bgc2); } cpux++;
            if (imagedata & 0x01) { colltable[cpuy][cpux] = 0x10; changethisabspixel(t); } else { colltable[cpuy][cpux] = 0; changethisbgpixel(bgc2); } cpux++;
    }   }

    for (cpux = hoffsetend; cpux < 227; cpux++)
    {   changethisbgpixel(bgc);
        colltable[cpuy][cpux] = 0;
}   }

MODULE void arcadia_runcpu(void)
{   FAST ULONG endcycle;

    slice_2650 += frac[fractionator & 3];
    fractionator++;

    // assert(slice_2650 >= 1);

    endcycle = cycles_2650 + slice_2650;
    if (endcycle < cycles_2650)
    {   // cycle counter will overflow, so we need to use the slow method
        while (slice_2650 >= 1)
        {   oldcycles = cycles_2650;
            one_instruction();
            slice_2650 -= (cycles_2650 - oldcycles);
    }   }
    else
    {   // cycle counter will not overflow, so we can use a faster method
        oldcycles = cycles_2650;
        while (cycles_2650 < endcycle)
        {   one_instruction();
        }
        slice_2650 -= (cycles_2650 - oldcycles);
}   }

MODULE __inline void do_sprites2(void)
{   FAST UBYTE bgcollisions,
               sprcollisions,
               sprcolours;
    FAST int   whichsprite,
               x, y;

    // Sprites----------------------------------------------------

    bgcollisions  =
    sprcollisions = 0;

    for (whichsprite = 0; whichsprite < 4; whichsprite++)
    {   for (y = spr[spriteflip][whichsprite].y; y <= endspry[whichsprite]; y++)
        {   t = spr[spriteflip][whichsprite].imagery[(y - spr[spriteflip][whichsprite].y) >> (spr[spriteflip][whichsprite].tall ? 1 : 0)];
            for (x = spr[spriteflip][whichsprite].x; x <= endsprx[whichsprite]; x++)
            {   if (t & (0x80 >> (x - spr[spriteflip][whichsprite].x)))
                {   colltable[y][x] |= (1 << whichsprite);

                    if (collisions && y >= n1 && x >= USG_XMARGIN)
                    {   if (colltable[y][x] & 0x10)
                        {   bgcollisions |= (1 << whichsprite);
                            if (useff[swapped ? 1 : 0])
                            {   if (p1bgcol[whichsprite] > 0)
                                {   p1rumble = p1bgcol[whichsprite];
                                    // zprintf(TEXTPEN_VERBOSE, "p1 background rumble!\n");
                                } elif (p1bgcol[whichsprite] < 0)
                                {   p1rumble = -p1bgcol[whichsprite];
                            }   }
                            if (useff[swapped ? 0 : 1])
                            {   if (p2bgcol[whichsprite] > 0)
                                {   p2rumble = p2bgcol[whichsprite];
                                    // zprintf(TEXTPEN_VERBOSE, "p2 background rumble!\n");
                                } elif (p2bgcol[whichsprite] < 0)
                                {   p2rumble = -p2bgcol[whichsprite];
                        }   }   }

                        if ((colltable[y][x] & 3) == 3) // %0011 #0 & #1
                        {   sprcollisions |= 1;
                            if (p1sprcol[0]) p1rumble = p1sprcol[0];
                            if (p2sprcol[0]) p2rumble = p2sprcol[0];
#ifdef LOGSPRITECOLLISIONS
                            zprintf(TEXTPEN_VERBOSE, "Sprites #0 & #1 have collided!\n");
#endif
                        }
                        if ((colltable[y][x] & 5) == 5) // %0101 #0 & #2
                        {   sprcollisions |= 2;
                            if (p1sprcol[1]) p1rumble = p1sprcol[1];
                            if (p2sprcol[1]) p2rumble = p2sprcol[1];
#ifdef LOGSPRITECOLLISIONS
                            zprintf(TEXTPEN_VERBOSE, "Sprites #0 & #2 have collided!\n");
#endif
                        }
                        if ((colltable[y][x] & 9) == 9) // %1001 #0 & #3
                        {   sprcollisions |= 4;
                            if (p1sprcol[2]) p1rumble = p1sprcol[2];
                            if (p2sprcol[2]) p2rumble = p2sprcol[2];
#ifdef LOGSPRITECOLLISIONS
                            zprintf(TEXTPEN_VERBOSE, "Sprites #0 & #3 have collided!\n");
#endif
                        }
                        if ((colltable[y][x] & 6) == 6) // %0110 #1 & #2
                        {   sprcollisions |= 8;
                            if (p1sprcol[3]) p1rumble = p1sprcol[3];
                            if (p2sprcol[3]) p2rumble = p2sprcol[3];
#ifdef LOGSPRITECOLLISIONS
                            zprintf(TEXTPEN_VERBOSE, "Sprites #1 & #2 have collided!\n");
#endif
                        }
                        if ((colltable[y][x] & 10) == 10) // %1010 #1 & #3
                        {   sprcollisions |= 16;
                            if (p1sprcol[4]) p1rumble = p1sprcol[4];
                            if (p2sprcol[4]) p2rumble = p2sprcol[4];
#ifdef LOGSPRITECOLLISIONS
                            zprintf(TEXTPEN_VERBOSE, "Sprites #1 & #3 have collided!\n");
#endif
                        }
                        if ((colltable[y][x] & 12) == 12) // %1100 #2 & #3
                        {   sprcollisions |= 32;
                            if (p1sprcol[5]) p1rumble = p1sprcol[5];
                            if (p2sprcol[5]) p2rumble = p2sprcol[5];
#ifdef LOGSPRITECOLLISIONS
                            zprintf(TEXTPEN_VERBOSE, "Sprites #2 & #3 have collided!\n");
#endif
                    }   }

                    if (x >= USG_XMARGIN + UVI_HIDELEFT)
                    {   sprcolours = 0;
                        if (colltable[y][x] & 1) sprcolours |= spr[spriteflip][0].colour;
                        if (colltable[y][x] & 2) sprcolours |= spr[spriteflip][1].colour;
                        if (colltable[y][x] & 4) sprcolours |= spr[spriteflip][2].colour;
                        if (colltable[y][x] & 8) sprcolours |= spr[spriteflip][3].colour;
                        changeabspixel(x, y, from_a[flag_cacheable][sprcolours]);
    }   }   }   }   }

    uviwrite((UWORD) A_SPRITECOLLIDE, (UBYTE) (~sprcollisions));
    uviwrite((UWORD) A_BGCOLLIDE    , (UBYTE) (~bgcollisions ));
}

MODULE void run_cpu(int until)
{   // This is a quicker equivalent to repeatedly incrementing cpux and calling do_cpu().

    cpux = nextinst;
    while (cpux < until)
    {   oldcycles = cycles_2650;
        one_instruction();
        nextinst += (cycles_2650 - oldcycles) * 4; // in pixels
        cpux = nextinst;
    }
    if (nextinst >= 227)
    {   nextinst -= cpl;
}   }
