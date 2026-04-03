// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
    #include <string.h>       // strlen()
    #include <proto/locale.h> // GetCatalogStr()
#endif
#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
#endif

#include <stdio.h>

#include "aa.h"

// DEFINES----------------------------------------------------------------

// #define WATCHPVIREADS
// whether reads by the PVI will trigger watchpoints
#ifndef WATCHPVIREADS
    #define pviread(x) memory[pvibase + x]
#endif

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       UBYTE                bgcoll,
                                  coinops_colltable[3][PVI_RASTLINES][PVI_XSIZE];
EXPORT       int                  collx, colly,
                                  fractional;

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       FLAG                 inframe,
                                  lmb, mmb, rmb,
                                  multisprite[4];
IMPORT       SBYTE                galaxia_scrolly;
IMPORT       UBYTE                awga_collide,
                                  coinignore,
                                  g_bank1[1024],
                                  memory[32768],
                                  psu,
                                  squeal,
                                  sx[2],
                                  sy[2];
IMPORT       UWORD                keypads[2][NUMKEYS];
IMPORT       ULONG                autofire[2],
                                  collisions,
                                  downframes,
                                  frames,
                                  jff[2],
                                  totalframes;
IMPORT       int                  ax[2],
                                  ay[4],
                                  coinop_1p,
                                  coinop_2p,
                                  coinop_button1,
                                  coinop_button2,
                                  coinop_button3,
                                  coinop_button4,
                                  coinop_coina,
                                  coinop_coinb,
                                  coinop_joy1left,
                                  coinop_joy1right,
                                  coinop_joy1up,
                                  coinop_joy1down,
                                  coinop_joy2left,
                                  coinop_joy2right,
                                  collx, colly,
                                  console_b,
                                  cpuy,
                                  drawmode,
                                  editscreen,
                                  hostcontroller[2],
                                  interrupt_2650,
                                  log_interrupts,
                                  machine,
                                  memmap,
                                  multiframe,
                                  offsetsprites,
                                  p1bgcol[6],
                                  p2bgcol[6],
                                  p1rumble,
                                  p2rumble,
                                  p1sprcol[6],
                                  p2sprcol[6],
                                  pvibase,
                                  requirebutton[2],
                                  scrnaddr,
                                  slice_2650,
                                  spritemode,
                                  trace,
                                  whichgame;
IMPORT const UBYTE                astrowars_colours[8],
                                  astrowars_tiles[2 * KILOBYTE],
                                  galaxia_colours[4][4],
                                  pvi_sprnumbers[2][4][10];
IMPORT const UWORD                pvi_spritedata[4];
IMPORT const int                  from_astrowars_spr[8],
                                  from_galaxia_spr[8],
                                  from_malzak_spr[8];
IMPORT       MEMFLAG              memflags[ALLTOKENS];
IMPORT       struct IOPortStruct  ioport[258];
IMPORT       struct MachineStruct machines[MACHINES];
IMPORT       ASCREEN              screen[BOXWIDTH][BOXHEIGHT];
#ifdef AMIGA
    IMPORT   int                  throb;
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE       int                  xx;

// MODULE STRUCTURES------------------------------------------------------

MODULE struct
{   int   x,
          majorrow,
          minorrow,
          size,
          starty;
    UBYTE colour,
          imagery;
} oldsprite[3][8];

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void drawminorrow(int whichpvi, int whichsprite);
MODULE __inline void drawsprites(int whichpvi);
MODULE void emulate_rast(void);
MODULE void initsprites(void);
MODULE void newmajorrow(int whichpvi, int whichsprite);
MODULE void newminorrow(int whichpvi, int whichsprite);
MODULE void oldspritedone(int whichpvi, int whichsprite);
MODULE void startsprite(int whichpvi, int whichsprite);
MODULE void oldpvi_collisions(void);
#ifdef WATCHPVIREADS
    MODULE UBYTE pviread(int address);
#endif

// CODE ------------------------------------------------------------------

MODULE void newminorrow(int whichpvi, int whichsprite)
{   oldsprite[whichpvi][whichsprite].minorrow++;
    if (oldsprite[whichpvi][whichsprite].minorrow == oldsprite[whichpvi][whichsprite].size)
    {   oldsprite[whichpvi][whichsprite].minorrow = 0;

        if (oldsprite[whichpvi][whichsprite].majorrow == 9)
        {   oldspritedone(whichpvi, whichsprite);
        } else
        {   oldsprite[whichpvi][whichsprite].majorrow++;
            newmajorrow(whichpvi, whichsprite);
}   }   }

MODULE void drawminorrow(int whichpvi, int whichsprite)
{   FAST int   expand,
               i,
               rast2,
               x;
    FAST UBYTE c;

    switch (memmap)
    {
    case MEMMAP_ASTROWARS:
        xx = oldsprite[whichpvi][whichsprite].x * 4 / 3; // 1 PVI = 1.3' non-PVI
    adefault:
        xx = oldsprite[whichpvi][whichsprite].x;
    }

    for (i = 0; i < 8; i++) // the 8 columns of the sprite imagery
    {   if (oldsprite[whichpvi][whichsprite].imagery & (128 >> i))
        {   expand = TRUE;
            for (x = 0; x < oldsprite[whichpvi][whichsprite].size; x++, xx++)
            {   if (xx >= PVI_XSIZE)
                {   return;
                } // implied else

                coinops_colltable[whichpvi][cpuy][xx] |= (0x10 << whichsprite);

                switch (memmap)
                {
                case MEMMAP_ASTROWARS:
                    c = (UBYTE) from_astrowars_spr[oldsprite[whichpvi][whichsprite].colour];
                    rast2 = cpuy - 16;
                acase MEMMAP_GALAXIA:
                    c = (UBYTE) from_galaxia_spr[  oldsprite[whichpvi][whichsprite].colour];
                    rast2 = cpuy - 16;
                acase MEMMAP_LASERBATTLE:
                case MEMMAP_LAZARIAN:
                    c = (UBYTE) from_galaxia_spr[  oldsprite[whichpvi][whichsprite].colour];
                    rast2 = cpuy - 14;
                acase MEMMAP_MALZAK1:
                case MEMMAP_MALZAK2:
                    c = (UBYTE) from_malzak_spr[   oldsprite[whichpvi][whichsprite].colour];
                    rast2 = cpuy;
                }

                if
                (   rast2      >= 0
                 && rast2      <  machines[machine].height
                 && xx         >= HIDDEN_X
                 && xx         <  HIDDEN_X + machines[machine].width
                 && c          != screen[xx - HIDDEN_X][rast2]
                 && (machine != MALZAK || cpuy >= 11) // to avoid sprites in status area
                 && spritemode != SPRITEMODE_INVISIBLE
                )
                {   changepixel
                    (   xx - HIDDEN_X,
                        rast2,
                        c
                    );
        }   }   }
        else
        {   xx += oldsprite[whichpvi][whichsprite].size;
            expand = FALSE;
        }
        if (memmap == MEMMAP_ASTROWARS && (i == 2 || i == 5 || i == 8))
        {   if (expand)
            {   if
                (   rast2      >= 0
                 && rast2      <  machines[machine].height
                 && xx         >= HIDDEN_X
                 && xx         <  HIDDEN_X + machines[machine].width
                 && c          != screen[xx - HIDDEN_X][rast2]
                 && spritemode != SPRITEMODE_INVISIBLE
                )
                {   changepixel
                    (   xx - HIDDEN_X,
                        rast2,
                        c
                    );
            }   }
            xx++;
}   }   }

MODULE __inline void drawsprites(int whichpvi)
{   FAST int whichsprite;

    // assert(cpuy < 272);

    for (whichsprite = 0; whichsprite < 4; whichsprite++)
    {   if
        (   oldsprite[whichpvi][whichsprite].majorrow == -1
         && oldsprite[whichpvi][whichsprite].starty   == cpuy
        )
        {   startsprite(whichpvi, whichsprite);
        }
        if (oldsprite[whichpvi][whichsprite].majorrow != -1)
        {   drawminorrow(whichpvi, whichsprite);
            newminorrow(whichpvi, whichsprite);
}   }   }

EXPORT void oldpvi(void)
{   FAST FLAG  sounding;
    FAST UBYTE ogc;
    FAST int   kx, x, x1, x2,
               ky, y, y1, y2;

    inframe = TRUE;

    // "The data is valid only at vertical reset." - 2636 PVI datasheet, p. 10.
    // But enabling these lines breaks Interton BOXING :-(
    // pviwrite(PVI_P1PADDLE, 0, TRUE);
    // pviwrite(PVI_P2PADDLE, 0, TRUE);

    memset(&coinops_colltable[0][0][0], 0, machines[machine].pvis * PVI_RASTLINES * PVI_XSIZE); // must precede foo_drawscreen()

    switch (machine)
    {
    case ZACCARIA:
        wa_checkinput();
        zaccaria_emuinput();
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:
            astrowars_drawscreen();
        acase MEMMAP_GALAXIA:
            galaxia_drawscreen();
        acase MEMMAP_LASERBATTLE:
        case MEMMAP_LAZARIAN:
            lb_drawscreen();
        }
    acase MALZAK:
        bgcoll = (memory[0x14CA] & 0xF0) | ((memory[0x15CA] & 0xF0) >> 4); // ie. PVI_BGCOLLIDE and PVI_BGCOLLIDE2
        if (memory[0x100A] == 0x06) // kludge
        {   offsetsprites = 68;
        } else
        {   offsetsprites =  0;
        }
        wa_checkinput();
        malzak_emuinput();
        malzak_drawscreen();
    }

    initsprites();

    pviwrite(PVI_SPRITECOLLIDE   , 0, TRUE); // clear VRST and sprite-sprite collision bits
    pviwrite(PVI_SPRITECOLLIDE_M2, 0, TRUE); // clear VRST and sprite-sprite collision bits
    pviwrite(PVI_SPRITECOLLIDE_M3, 0, TRUE); // clear VRST and sprite-sprite collision bits
    pviwrite(PVI_SPRITECOLLIDE_M4, 0, TRUE); // clear VRST and sprite-sprite collision bits
    pviwrite(PVI_BGCOLLIDE       , 0, TRUE); // clear sprite complete and sprite-bg collision bits
    pviwrite(PVI_BGCOLLIDE_M2    , 0, TRUE); // clear sprite complete and sprite-bg collision bits
    pviwrite(PVI_BGCOLLIDE_M3    , 0, TRUE); // clear sprite complete and sprite-bg collision bits
    pviwrite(PVI_BGCOLLIDE_M4    , 0, TRUE); // clear sprite complete and sprite-bg collision bits
    if (machines[machine].pvis >= 2)
    {   pvibase += 0x100; // switch to 2nd PVI
        pviwrite(PVI_SPRITECOLLIDE   , 0, TRUE); // clear VRST and sprite-sprite collision bits
        pviwrite(PVI_SPRITECOLLIDE_M2, 0, TRUE); // clear VRST and sprite-sprite collision bits
        pviwrite(PVI_SPRITECOLLIDE_M3, 0, TRUE); // clear VRST and sprite-sprite collision bits
        pviwrite(PVI_SPRITECOLLIDE_M4, 0, TRUE); // clear VRST and sprite-sprite collision bits
        pviwrite(PVI_BGCOLLIDE       , 0, TRUE); // clear sprite complete and sprite-bg collision bits
        pviwrite(PVI_BGCOLLIDE_M2    , 0, TRUE); // clear sprite complete and sprite-bg collision bits
        pviwrite(PVI_BGCOLLIDE_M3    , 0, TRUE); // clear sprite complete and sprite-bg collision bits
        pviwrite(PVI_BGCOLLIDE_M4    , 0, TRUE); // clear sprite complete and sprite-bg collision bits
        if (machines[machine].pvis >= 3)
        {   pvibase += 0x100; // switch to 3rd PVI
            pviwrite(PVI_SPRITECOLLIDE   , 0, TRUE); // clear VRST and sprite-sprite collision bits
            pviwrite(PVI_SPRITECOLLIDE_M2, 0, TRUE); // clear VRST and sprite-sprite collision bits
            pviwrite(PVI_SPRITECOLLIDE_M3, 0, TRUE); // clear VRST and sprite-sprite collision bits
            pviwrite(PVI_SPRITECOLLIDE_M4, 0, TRUE); // clear VRST and sprite-sprite collision bits
            pviwrite(PVI_BGCOLLIDE       , 0, TRUE); // clear sprite complete and sprite-bg collision bits
            pviwrite(PVI_BGCOLLIDE_M2    , 0, TRUE); // clear sprite complete and sprite-bg collision bits
            pviwrite(PVI_BGCOLLIDE_M3    , 0, TRUE); // clear sprite complete and sprite-bg collision bits
            pviwrite(PVI_BGCOLLIDE_M4    , 0, TRUE); // clear sprite complete and sprite-bg collision bits
        }
        pvibase = machines[machine].pvibase; // switch to 1st PVI
    }
    psu &= ~(PSU_S);
    interrupt_2650 = FALSE; // "Interrupts are reset on the trailing edge of the vertical reset signal." - 2636 PVI datasheet, p. 3.

    for (cpuy = 0; cpuy < 269; cpuy++) // 312 - 43
    {   /* For each rastline, we emulate the CPU first, and then the PVI.
        This is because on the real machine, the PVI would presumably generate the
        interrupt, write to the newsprite registers, etc. at the end of the rastline.
        So, by that time the CPU would have executed a rastline's worth of
        instructions. */
        emulate_rast();

        drawsprites(0);
        if (machines[machine].pvis >= 2)
        {   pvibase += 0x100; // switch to 2nd PVI
            drawsprites(1);
            if (machines[machine].pvis >= 3)
            {   pvibase += 0x100; // switch to 3rd PVI
                drawsprites(2);
            }
            pvibase = machines[machine].pvibase;
        }

        if (collisions)
        {   oldpvi_collisions();
    }   }

    cpuy = 269; // 312 - 43
    pvi_vblank();

    // We don't call oldspritedone() for truncated sprites (ie. ones that were partly-drawn when we hit raster 272).
    // This seems to be authentic, eg. see Interton PINBALLA.

    for (cpuy = 269; cpuy < 312; cpuy++) // 312 - 43
    {   emulate_rast();
    }

#ifdef SHOWRUMBLE
    if (rumbling[0])
    {   for (y = 0; y < machines[machine].height; y++)
        {   changepixel(             0, y, 15 - screen[             0][y]);
    }   }
    if (rumbling[1])
    {   for (y = 0; y < machines[machine].height; y++)
        {   changepixel(machines[machine].width - 1, y, 15 - screen[machines[machine].width - 1][y]);
    }   }
#endif

    if (editscreen)
    {   // assert(machine == ZACCARIA);
        kx =  scrnaddr           % 32;
        ky = (scrnaddr - 0x1800) / 32;
        x1 = (kx * 8) - 1     ;
        x2 = (kx * 8) + 8     ;
        y1 = (ky * 8) - 1 - 16;
        y2 = (ky * 8) + 8 - 16;
        if (memmap == MEMMAP_GALAXIA && x1 >= 24 && x2 <= 111)
        {   y1 += galaxia_scrolly;
            y2 += galaxia_scrolly;
        }
        if (memmap == MEMMAP_ASTROWARS)
        {   if ((g_bank1[scrnaddr - 0x1800] & 7) == 7)
            {   ogc = GREY1;
            } else
            {   ogc = astrowars_colours[g_bank1[scrnaddr - 0x1800] & 7];
            }
            for (x = 7; x >= 0; x--)
            {   for (y = 0; y < 8; y++)
                {   if
                    (   (kx * 8) + x      >=  0
                     && (kx * 8) + x      < 240
                     && (ky * 8) + y - 16 >=  0
                     && (ky * 8) + y - 16 < 240
                    )
                    {   if (astrowars_tiles[(memory[scrnaddr] * 8) + y] & (128 >> x))
                        {   changepixel((kx * 8) + x, (ky * 8) + y - 16, ogc);
                        } else
                        {   changepixel((kx * 8) + x, (ky * 8) + y - 16, BLACK);
        }   }   }   }   }
        else
        {   ogc = RED;
        }

        for (x = x1; x <= x2; x++)
        {   for (y = y1; y <= y2; y++)
            {   if
                (   (x == x1 || x == x2 || y == y1 || y == y2)
                 &&  x >=  0
                 &&  x < 240
                 &&  y >=  0
                 &&  y < 240
                )
                {   changepixel(x, y, ogc);
    }   }   }   }

    if (machine == MALZAK)
    {   if (drawmode)
        {   malzak_drawhelpgrid();
    }   }
    else
    {   // assert(machine == ZACCARIA);
        if (drawmode)
        {   zaccaria_drawhelpgrid();
        }
        if (memmap == MEMMAP_GALAXIA)
        {   sounding = FALSE;
            if (squeal)
            {   squeal--;
                sounding = TRUE;
            }
#ifdef AMIGA
            if (throb)
            {   if (throb == 12) // throb goes 1..12
                {   throb = 1;
                } else
                {   throb++;
                }
                sounding = TRUE;
            }
#endif
            if (sounding)
            {   playsound(FALSE);
    }   }   }
    endofframe(BLACK);
}

MODULE void startsprite(int whichpvi, int whichsprite)
{   /* It is probable that mid-sprite changes to SIZES are ineffective
       (this behaviour helps HUNTING, SHOOTGAL). */

    oldsprite[whichpvi][whichsprite].majorrow =
    oldsprite[whichpvi][whichsprite].minorrow = 0;
    switch (whichsprite)
    {
    case  0: oldsprite[whichpvi][0].size = 1 << ( pviread(PVI_SIZES)       & 3);
    acase 1: oldsprite[whichpvi][1].size = 1 << ((pviread(PVI_SIZES) >> 2) & 3);
    acase 2: oldsprite[whichpvi][2].size = 1 << ((pviread(PVI_SIZES) >> 4) & 3);
    acase 3: oldsprite[whichpvi][3].size = 1 << ((pviread(PVI_SIZES) >> 6) & 3);
    }

    switch (memmap)
    {
    case MEMMAP_ASTROWARS:
        oldsprite[whichpvi][whichsprite].x = pviread(pvi_spritedata[whichsprite] + 10) +  1; // SPRITEnAX
    acase MEMMAP_GALAXIA:
        oldsprite[whichpvi][whichsprite].x = pviread(pvi_spritedata[whichsprite] + 10) - 11; // SPRITEnAX
        if (oldsprite[whichpvi][whichsprite].x == 255 - 11)
        {   oldsprite[whichpvi][whichsprite].x = 256;
        }
    acase MEMMAP_LASERBATTLE:
    case MEMMAP_LAZARIAN:
        oldsprite[whichpvi][whichsprite].x = pviread(pvi_spritedata[whichsprite] + 10) -  4; // SPRITEnAX
    acase MEMMAP_MALZAK1:
    case MEMMAP_MALZAK2:
        oldsprite[whichpvi][whichsprite].x = pviread(pvi_spritedata[whichsprite] + 10) -  3 + offsetsprites; // SPRITEnAX
    }

    newmajorrow(whichpvi, whichsprite);
}

MODULE void newmajorrow(int whichpvi, int whichsprite)
{   /* These are reread at the start of each major row:
       sprite colour?
       sprite imagery?
       sprite X-coordinate - actually every minor row too */

    // assert(minorrow == 0);

    switch (whichsprite)
    {
    case  0: oldsprite[whichpvi][0].colour = ((pviread(PVI_SPR01COLOURS) >> 3) & 7);
    acase 1: oldsprite[whichpvi][1].colour = ( pviread(PVI_SPR01COLOURS)       & 7);
    acase 2: oldsprite[whichpvi][2].colour = ((pviread(PVI_SPR23COLOURS) >> 3) & 7);
    acase 3: oldsprite[whichpvi][3].colour = ( pviread(PVI_SPR23COLOURS)       & 7);
    }

    switch (spritemode)
    {
    case SPRITEMODE_INVISIBLE:
        oldsprite[whichpvi][whichsprite].imagery = 0;
    acase SPRITEMODE_NUMBERED:
        oldsprite[whichpvi][whichsprite].imagery = pvi_sprnumbers[0][whichsprite][oldsprite[whichpvi][whichsprite].majorrow];
    acase SPRITEMODE_VISIBLE:
        oldsprite[whichpvi][whichsprite].imagery = pviread(pvi_spritedata[whichsprite]
                                                 + oldsprite[whichpvi][whichsprite].majorrow);
}   }

MODULE void oldspritedone(int whichpvi, int whichsprite)
{   // pvibase is expected to be set according to the PVI you are using

    oldsprite[whichpvi][whichsprite].majorrow = -1;

    pviwrite(PVI_BGCOLLIDE   , (UBYTE) (pviread(PVI_BGCOLLIDE   ) | (8 >> whichsprite)), TRUE);
    pviwrite(PVI_BGCOLLIDE_M2, (UBYTE) (pviread(PVI_BGCOLLIDE_M2) | (8 >> whichsprite)), TRUE);
    pviwrite(PVI_BGCOLLIDE_M3, (UBYTE) (pviread(PVI_BGCOLLIDE_M3) | (8 >> whichsprite)), TRUE);
    pviwrite(PVI_BGCOLLIDE_M4, (UBYTE) (pviread(PVI_BGCOLLIDE_M4) | (8 >> whichsprite)), TRUE);

    interrupt_2650 = TRUE;
    if (log_interrupts)
    {   if (psu & PSU_II)
        {   zprintf
            (   TEXTPEN_LOG,
                "Pended sprite #%d display complete interrupt at rastline %d.\n",
                (whichpvi * 4) + whichsprite,
                cpuy
            );
        } else
        {   zprintf
            (   TEXTPEN_LOG,
                "Generated sprite #%d display complete interrupt at rastline %d.\n",
                (whichpvi * 4) + whichsprite,
                cpuy
            );
    }   }
    checkinterrupt();
}

MODULE void initsprites(void)
{   int i,
        whichsprite;

    for (whichsprite = 0; whichsprite < 4; whichsprite++)
    {   oldsprite[0][whichsprite].starty           = ((UBYTE) (pviread(pvi_spritedata[whichsprite] + 12) + 1)); // SPRITEnAY (the +1 is necessary, eg. for Interton MATHEMA2)
        oldsprite[0][whichsprite].majorrow         = -1;
        if (machines[machine].pvis >= 2)
        {   pvibase += 0x100; // switch to 2nd PVI
            oldsprite[1][whichsprite].starty       = ((UBYTE) (pviread(pvi_spritedata[whichsprite] + 12) + 1)); // SPRITEnAY
            oldsprite[1][whichsprite].majorrow     = -1;
            if (machines[machine].pvis >= 3)
            {   pvibase += 0x100; // switch to 3rd PVI
                oldsprite[2][whichsprite].starty   = ((UBYTE) (pviread(pvi_spritedata[whichsprite] + 12) + 1)); // SPRITEnAY
                oldsprite[2][whichsprite].majorrow = -1;
            }
            pvibase = machines[machine].pvibase; // switch to 1st PVI
    }   }

    if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
    {   for (i = 0; i <= 2; i++)
        {   for (whichsprite = 0; whichsprite < 4; whichsprite++)
            {    oldsprite[i][whichsprite].starty += 2;
}   }   }   }

MODULE void emulate_rast(void)
{   breakrastline();

#ifdef MALZAK_885KHZ
    if (machine == MALZAK) // 227 / 4 = 56.75
    {   // This relies on the fact that 312 % 4 == 0
        if (cpuy % 4 == 0)
        {   slice_2650 += 56;
        } else
        {   slice_2650 += 57;
    }   }
    else // 227 / 3 = 75.6'
#endif
    {   // This relies on the fact that 312 % 3 == 0
        if (cpuy % 3 == 0)
        {   slice_2650 += 75;
        } else
        {   slice_2650 += 76;
    }   }

    cpu_2650_untapable();
}

MODULE void oldpvi_collisions(void)
{   FAST UBYTE t1[3], t2[3],
               tt1, tt2;
    FAST int   whichpvi,
               x;

    for (whichpvi = 0; whichpvi < machines[machine].pvis; whichpvi++)
    {   t1[whichpvi] =
        t2[whichpvi] = 0;
    }

    for (x = 9; x < PVI_XSIZE; x++)
    {   if (memmap == MEMMAP_GALAXIA)
        {   // inter-PVI collisions
            tt1 = coinops_colltable[0][cpuy][x] & 0xF0;
            tt2 = coinops_colltable[1][cpuy][x] & 0xF0;
            if (tt1 && tt2) // logical not bitwise! #0 & #2
            {   awga_collide |= 1;
            }
            tt1 = coinops_colltable[2][cpuy][x] & 0xF0;
            if (tt1 && tt2) // logical not bitwise! #2 & #1
            {   awga_collide |= 2;
            }
            tt2 = coinops_colltable[0][cpuy][x] & 0xF0;
            if (tt1 && tt2) // logical not bitwise! #2 & #0
            {   awga_collide |= 4;
        }   }

        for (whichpvi = 0; whichpvi < machines[machine].pvis; whichpvi++)
        {   if (!coinops_colltable[whichpvi][cpuy][x]) // just a speed optimization
            {   continue;
            }

            // CVS-PVI collisions (not used?)
            if
            (   (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
             && (coinops_colltable[       0][cpuy][x] &    4)
             && (coinops_colltable[whichpvi][cpuy][x] & 0xF0)
            )
            {   awga_collide |= 8;
            }

            // intra-PVI collisions
            if ((coinops_colltable[whichpvi][cpuy][x] & 0x30) == 0x30) { t1[whichpvi] |=  32; /* zprintf(TEXTPEN_VERBOSE, "#0 & #1!\n"); */ if (p1sprcol[0]) p1rumble = p1sprcol[0]; if (p2sprcol[0]) p2rumble = p2sprcol[0]; }
            if ((coinops_colltable[whichpvi][cpuy][x] & 0x50) == 0x50) { t1[whichpvi] |=  16; /* zprintf(TEXTPEN_VERBOSE, "#0 & #2!\n"); */ if (p1sprcol[1]) p1rumble = p1sprcol[1]; if (p2sprcol[1]) p2rumble = p2sprcol[1]; }
            if ((coinops_colltable[whichpvi][cpuy][x] & 0x90) == 0x90) { t1[whichpvi] |=   8; /* zprintf(TEXTPEN_VERBOSE, "#0 & #3!\n"); */ if (p1sprcol[2]) p1rumble = p1sprcol[2]; if (p2sprcol[2]) p2rumble = p2sprcol[2]; }
            if ((coinops_colltable[whichpvi][cpuy][x] & 0x60) == 0x60) { t1[whichpvi] |=   4; /* zprintf(TEXTPEN_VERBOSE, "#1 & #2!\n"); */ if (p1sprcol[3]) p1rumble = p1sprcol[3]; if (p2sprcol[3]) p2rumble = p2sprcol[3]; }
            if ((coinops_colltable[whichpvi][cpuy][x] & 0xA0) == 0xA0) { t1[whichpvi] |=   2; /* zprintf(TEXTPEN_VERBOSE, "#1 & #3!\n"); */ if (p1sprcol[4]) p1rumble = p1sprcol[4]; if (p2sprcol[4]) p2rumble = p2sprcol[4]; }
            if ((coinops_colltable[whichpvi][cpuy][x] & 0xC0) == 0xC0) { t1[whichpvi] |=   1; /* zprintf(TEXTPEN_VERBOSE, "#2 & #3!\n"); */ if (p1sprcol[5]) p1rumble = p1sprcol[5]; if (p2sprcol[5]) p2rumble = p2sprcol[5]; }

            if (coinops_colltable[0][cpuy][x] & 8) // background pixel
            {   if (coinops_colltable[whichpvi][cpuy][x] & 0xF0) // PVI sprite pixel
                {   // PVI vs. background collisions
                    if (memmap == MEMMAP_ASTROWARS)
                    {   awga_collide |= 1;
                    } elif (memmap == MEMMAP_GALAXIA && whichpvi != 0)
                    {   awga_collide |= (0x10 << whichpvi);
                }   }

                if (coinops_colltable[whichpvi][cpuy][x] & 0x10) { t2[whichpvi] |= 128; /* zprintf(TEXTPEN_VERBOSE, "#0 & BG!\n"); */ if (p1bgcol[0] > 0) p1rumble = p1bgcol[ 0]; if (p2bgcol[0] > 0) p2rumble = p2bgcol[ 0]; collx = x; colly = cpuy; }
                if (coinops_colltable[whichpvi][cpuy][x] & 0x20) { t2[whichpvi] |=  64; /* zprintf(TEXTPEN_VERBOSE, "#1 & BG!\n"); */ if (p1bgcol[1] > 0) p1rumble = p1bgcol[ 1]; if (p2bgcol[1] > 0) p2rumble = p2bgcol[ 1]; collx = x; colly = cpuy; }
                if (coinops_colltable[whichpvi][cpuy][x] & 0x40) { t2[whichpvi] |=  32; /* zprintf(TEXTPEN_VERBOSE, "#2 & BG!\n"); */ if (p1bgcol[2] > 0) p1rumble = p1bgcol[ 2]; if (p2bgcol[2] > 0) p2rumble = p2bgcol[ 2]; collx = x; colly = cpuy; }
                if (coinops_colltable[whichpvi][cpuy][x] & 0x80) { t2[whichpvi] |=  16; /* zprintf(TEXTPEN_VERBOSE, "#3 & BG!\n"); */ if (p1bgcol[3] > 0) p1rumble = p1bgcol[ 3]; if (p2bgcol[3] > 0) p2rumble = p2bgcol[ 3]; collx = x; colly = cpuy; }
    }   }   }

    for (whichpvi = 0; whichpvi < machines[machine].pvis; whichpvi++)
    {   if (t1[whichpvi]) // just a speed optimization
        {   pviwrite(PVI_SPRITECOLLIDE   , (UBYTE) (pviread(PVI_SPRITECOLLIDE   ) | t1[whichpvi]), TRUE);
            pviwrite(PVI_SPRITECOLLIDE_M2, (UBYTE) (pviread(PVI_SPRITECOLLIDE_M2) | t1[whichpvi]), TRUE);
            pviwrite(PVI_SPRITECOLLIDE_M3, (UBYTE) (pviread(PVI_SPRITECOLLIDE_M3) | t1[whichpvi]), TRUE);
            pviwrite(PVI_SPRITECOLLIDE_M4, (UBYTE) (pviread(PVI_SPRITECOLLIDE_M4) | t1[whichpvi]), TRUE);
        }
        if (t2[whichpvi]) // just a speed optimization
        {   pviwrite(PVI_BGCOLLIDE   ,     (UBYTE) (pviread(PVI_BGCOLLIDE       ) | t2[whichpvi]), TRUE);
            pviwrite(PVI_BGCOLLIDE_M2,     (UBYTE) (pviread(PVI_BGCOLLIDE_M2    ) | t2[whichpvi]), TRUE);
            pviwrite(PVI_BGCOLLIDE_M3,     (UBYTE) (pviread(PVI_BGCOLLIDE_M3    ) | t2[whichpvi]), TRUE);
            pviwrite(PVI_BGCOLLIDE_M4,     (UBYTE) (pviread(PVI_BGCOLLIDE_M4    ) | t2[whichpvi]), TRUE);
        }
        pvibase += 0x100;
    }
    pvibase = machines[machine].pvibase; // switch to 1st PVI
}

#ifdef WATCHPVIREADS
MODULE UBYTE pviread(int address)
{   address += pvibase;

    if
    (   watchreads
     && (memflags[address] & WATCHPOINT)
     && conditional(&wp[address], machines[machine].readonce, FALSE)
    )
    {   DISCARD getfriendly(address);
        zprintf
        (   TEXTPEN_DEBUG,
            LLL(MSG_PVIREADING, "PVI is reading $%02X from address %s at raster %ld!\n"),
            memory[address],
            (char*) friendly,
            (long int) cpuy
        );
        set_pause(TYPE_BP);
    }

    return memory[address];
}
#endif

EXPORT void coinop_playerinput(int source, int dest)
{   FAST ULONG jg;

    if
    (   hostcontroller[source] == CONTROLLER_1STDJOY
     || hostcontroller[source] == CONTROLLER_1STDPAD
  // || hostcontroller[source] == CONTROLLER_1STAJOY
     || hostcontroller[source] == CONTROLLER_1STAPAD
    )
    {   jg = jff[0];
    } elif
    (   hostcontroller[source] == CONTROLLER_2NDDJOY
     || hostcontroller[source] == CONTROLLER_2NDDPAD
     || hostcontroller[source] == CONTROLLER_2NDAJOY
     || hostcontroller[source] == CONTROLLER_2NDAPAD
    )
    {   jg = jff[1];
    } else
    {   jg = 0;
    }

    if (autofire[source])
    {   if
        (   (   !requirebutton[source]
             || (jg & JOYFIRE1)
             || ((memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN) && (jg & DAPTER_5  ))
             || ( memmap != MEMMAP_LASERBATTLE && memmap != MEMMAP_LAZARIAN  && (jg & DAPTER_TOP))
             || KeyDown(keypads[source][0])
             || (   hostcontroller[source] == CONTROLLER_TRACKBALL
                 && lmb
            )   )
         && (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN || ((frames % totalframes) < downframes))
        )
        {   // 1st firebutton
            switch (memmap)
            {
            case MEMMAP_ASTROWARS:
            case MEMMAP_GALAXIA:
                if (dest == 0)
                {   coinop_button1 |= 1;
                } else
                {   // assert(dest == 1);
                    coinop_button2 |= 1;
                }
            acase MEMMAP_MALZAK1:
            case MEMMAP_MALZAK2:
                coinop_button1 |= 1;
            acase MEMMAP_LASERBATTLE:
            case MEMMAP_LAZARIAN:
                switch (frames % 2)
                {
                case 0:
                     coinop_button3 |= 1;
                acase 1:
                     coinop_button3 |= 0;
    }   }   }   }
    else
    {   if
        (   (jg & JOYFIRE1)
         || KeyDown(keypads[source][0])
         || KeyDown(keypads[source][2])
         || ((memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN) && (KeyDown(keypads[source][7]) || (jg & DAPTER_7))) // guest "7" key (host "A")
         || (hostcontroller[source] == CONTROLLER_TRACKBALL && lmb)
        )
        {   // 1st firebutton ("left")
            switch (machine)
            {
            case ZACCARIA:
                if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
                {   if (dest == 0)
                    {   coinop_button1 |= 1;
                    } else
                    {   // assert(dest == 1);
                        coinop_button2 |= 1;
                }   }
                else
                {   // assert(memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN);
                    coinop_button1 |= 1;
                }
            acase MALZAK:
                coinop_button1 |= 1;
    }   }   }

    if
    (   (jg & JOYFIRE2)
     || KeyDown(keypads[source][21])
     || ((memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN) && (KeyDown(keypads[source][8]) || (jg & DAPTER_8))) // guest "8" key (host "S")
     || (hostcontroller[source] == CONTROLLER_TRACKBALL && mmb)
    )
    {   // 2nd firebutton ("down")
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:
        case MEMMAP_GALAXIA:
            if (dest == 0)
            {   coinop_button1 |= 1;
            } else
            {   // assert(dest == 1);
                coinop_button2 |= 1;
            }
        acase MEMMAP_LASERBATTLE:
        case MEMMAP_LAZARIAN:
            coinop_button4 |= 1;
        acase MEMMAP_MALZAK1:
        case MEMMAP_MALZAK2:
            coinop_button1 |= 1;
    }   }

    if
    (   (jg & JOYFIRE3)
     || KeyDown(keypads[source][22])
     || ((memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN) && (KeyDown(keypads[source][9]) || (jg & DAPTER_9))) // guest "9" key (host "D")
     || (hostcontroller[source] == CONTROLLER_TRACKBALL && rmb)
    )
    {   // 3rd firebutton ("right")
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:
        case MEMMAP_GALAXIA:
            if (dest == 0)
            {   coinop_button1 |= 1;
            } else
            {   // assert(dest == 1);
                coinop_button2 |= 1;
            }
        acase MEMMAP_LASERBATTLE:
        case MEMMAP_LAZARIAN:
            coinop_button2 |= 1;
        acase MEMMAP_MALZAK1:
        case MEMMAP_MALZAK2:
            coinop_button1 |= 1;
    }   }

    if
    (   (jg & JOYFIRE4)
     || (KeyDown(keypads[source][23]))
     || ((memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN) && (KeyDown(keypads[source][5]) || (!autofire[source] && (jg & DAPTER_5)))) // guest "5" key (host "W")
    )
    {   // 4th firebutton ("up")
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:
        case MEMMAP_GALAXIA:
            if (dest == 0)
            {   coinop_button1 |= 1;
            } else
            {   // assert(dest == 1);
                coinop_button2 |= 1;
            }
        acase MEMMAP_LASERBATTLE:
        case MEMMAP_LAZARIAN:
            coinop_button3 |= 1;
        acase MEMMAP_MALZAK1:
        case MEMMAP_MALZAK2:
            coinop_button1 |= 1;
    }   }

    engine_dopaddle(source, 0);

    if (ax[0] < 64)
    {   switch (memmap)
        {
        case MEMMAP_ASTROWARS:
        case MEMMAP_GALAXIA:
            if (dest == 0)
            {   coinop_joy1left |= 1;
            } else
            {   // assert(dest == 1);
                coinop_joy2left |= 1;
            }
        acase MEMMAP_LASERBATTLE:
        case MEMMAP_LAZARIAN:
        case MEMMAP_MALZAK1:
        case MEMMAP_MALZAK2:
            coinop_joy1left |= 1;
    }   }
    elif (ax[0] > 192)
    {   switch (machine)
        {
        case ZACCARIA:
            if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
            {   if (dest == 0)
                {   coinop_joy1right |= 1;
                } else
                {   // assert(dest == 1);
                    coinop_joy2right |= 1;
            }   }
            else
            {   // assert(memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN);
                coinop_joy1right |= 1;
            }
        acase MALZAK:
            coinop_joy1right |= 1;
    }   }

    if (ay[0] < 64)
    {   coinop_joy1up |= 1;
    } elif (ay[0] > 192)
    {   coinop_joy1down |= 1;
    }

    sx[0] = (UBYTE) ax[0];
    sy[0] = (UBYTE) ay[0];
}

EXPORT void serialize_coinops(void)
{   serialize_byte_int(&coinop_1p);
    serialize_byte_int(&coinop_2p);
    serialize_byte_int(&coinop_coina);
    serialize_byte_int(&coinop_coinb);
    serialize_byte_int(&coinop_button1);
    serialize_byte_int(&coinop_button2);
    serialize_byte_int(&coinop_button3);
    serialize_byte_int(&coinop_button4);
    serialize_byte_int(&coinop_joy1left);
    serialize_byte_int(&coinop_joy1right);
    serialize_byte_int(&coinop_joy1up);
    serialize_byte_int(&coinop_joy1down);
    serialize_byte_int(&coinop_joy2left);
    serialize_byte_int(&coinop_joy2right);
    serialize_byte(    &coinignore);
}

EXPORT void do_autocoin(void)
{   FAST UBYTE tens, ones;
    FAST int   credits = 0; // initialized to avoid spurious SAS/C optimizer warnings

    // assert(machines[machine].coinop);

    if
    (   memmap == MEMMAP_MALZAK2
     && memory[0x1400 + PVI_P1PADDLE] != 0xF0 // state "1" (normal play)
     && memory[0x1400 + PVI_P1PADDLE] != 0x90 // state "2"
    )
    {   return;
    }

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
