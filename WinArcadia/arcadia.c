// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
    #include <string.h>
    #include <stdlib.h>
    #include <proto/locale.h>      // GetCatalogStr()
#endif
#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
#endif

#include <stdio.h>

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"

#include "arcadia.h"

// DEFINES----------------------------------------------------------------

// #define LOGSPRITECOLLISIONS
// whether to print a message for inter-sprite collisions (not sprite-bg)

#define UDCFLIPS         7

#define UVIXTOSCREENX(x) (USG_XMARGIN + x - absxmin)
#define UVIYTOSCREENY(y) (USG_YMARGIN + y - absymin)

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       ULONG                 fractionator;
EXPORT       int                   frac[4];

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       ULONG                 arcadia_viewcontrolsas,
                                   arcadia_bigctrls,
                                   autofire[2],
                                   collisions,
                                   cycles_2650,
                                   demultiplex,
                                   downframes,
                                   frames,
                                   jff[2],
                                   oldcycles,
                                   region,
                                   swapped,
                                   totalframes;
IMPORT       UBYTE*                IOBuffer;
IMPORT       FLAG                  inframe,
                                   lmb, mmb, rmb;
IMPORT       FILE*                 MacroHandle;
IMPORT       TEXT                  file_game[MAX_PATH + 1],
                                   fn_game[MAX_PATH + 1],    // the entire pathname (path and file)
                                   fn_project[MAX_PATH + 1], // the entire pathname (path and file)
                                   friendly[FRIENDLYLENGTH + 1],
                                   netmsg_out[80 + 1],
                                   path_projects[MAX_PATH + 1],
                                   ProjectBuffer[PROJECTSIZE],
                                   stringchar[19 + 1];
IMPORT       int                   absxmin, absxmax,
                                   absymin, absymax,
                                   ax[2],
                                   ay[4],
                                   cpl,
                                   connected,
                                   console_start,
                                   console_a,
                                   console_b,
                                   console_reset,
                                   cpux,
                                   cpuy,
                                   dejitter,
                                   drawmode,
                                   editscreen,
                                   filesize,
                                   firstrow,
                                   framebased,
                                   game,
                                   hidetop,
                                   hostcontroller[2],
                                   key1,
                                   key2,
                                   key3,
                                   key4,
                                   lastrow,
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
                                   randomizememory,
                                   recmode,
                                   requirebutton[2],
                                   p1rumble,
                                   p2rumble,
                                   rumbling[2],
                                   scrnaddr,
                                   serializemode,
                                   spritemode,
                                   supercpu,
                                   trace,
                                   udcflips,
                                   undither,
                                   useff[2],
                                   usemargins,
                                   usestubs,
                                   watchwrites,
                                   whichgame;
IMPORT       STRPTR                colourname[8];
IMPORT       ASCREEN               screen[BOXWIDTH][BOXHEIGHT];
IMPORT       UBYTE                 memory[32768],
                                   opcode,
                                   OutputBuffer[18],
                                   psu,
                                   sx[2], sy[2];
IMPORT       UWORD                 console[4],
                                   iar,
                                   keypads[2][NUMKEYS],
                                   mirror_r[32768],
                                   mirror_w[32768],
                                   screen_iar[BOXWIDTH][BOXHEIGHT];
IMPORT       MEMFLAG               memflags[ALLTOKENS];
IMPORT const UBYTE                 table_opcolours_2650[2][256];
IMPORT const int                   guest_to_ansi_colour[8];
IMPORT       struct ConditionalStruct wp[ALLTOKENS];
IMPORT const struct KeyTableStruct keytable[16];
IMPORT const struct KindStruct     filekind[KINDS];
IMPORT const struct KnownStruct    known[KNOWNGAMES];
IMPORT       struct MachineStruct  machines[MACHINES];

#ifdef WIN32
    IMPORT       int               CatalogPtr;
    IMPORT       HWND              SubWindowPtr[SUBWINDOWS];
#endif
#ifdef AMIGA
    IMPORT       struct Catalog*   CatalogPtr;
    IMPORT       struct Window*    SubWindowPtr[SUBWINDOWS];
#endif
#ifdef BENCHMARK_GFX
    IMPORT       ULONG             cycles_2650;
#endif
#ifdef EMULATE_CPU
    IMPORT       int               slice_2650;
#endif

IMPORT void (* drawpixel  ) (int x, int y, int colour);
#ifdef WIN32
IMPORT void (* drawbgpixel) (int x, int y, int colour);
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
              dmascreen[312],
              fgc1, fgc2,         // only used for board mode. Arcadia format
              outerbgc, innerbgc, // Arcadia format
              hires,              // 0 for low resolution, 1 for high resolution
              rowbuf[UDCFLIPS + 1][26][16],
              scrnbgc,
              sprimagedata[4],
              udgimgbuf[UDCFLIPS + 1][8][8],
              t;
MODULE SLONG  blockmode,
              column,
              majorrow, // whenever minorrow gets to 8, minorrow is zeroed and majorrow is incremented
              minorrow, // goes up only 1 per 2 rasts in low-res mode
                        // 0..7: which y-pixel row in the current "band" (row of chars) we are doing
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
MODULE void arcadia_oneraster(void);
MODULE void arcadia_runcpu(void);
MODULE void run_cpu(int until);

// CODE-------------------------------------------------------------------

EXPORT void uvi(void)
{   FAST int   whichudg,
               kx, ky,
               x, x1, x2,
               y, y1, y2;
    FAST UBYTE ogc,
               tgc;
#ifdef BENCHMARK_GFX
    FAST int   cyclecolour = 0, x;
#endif

    // assert(machine == ARCADIA);

#ifndef EMULATE_XVI
    #ifdef EMULATE_CPU
        slice_2650 = 20000;
        cpu_2650_untapable();
    #endif

    endofframe(0);
    return;
#endif

#ifdef BENCHMARK_GFX
    cycles_2650 += 20000;
    for (x = 0; x < 227; x++)
    {   for (y = 0; y < n3; y++)
        {   changeabspixel(x, y, cyclecolour);
    }   }
    if (cyclecolour == 23)
    {   cyclecolour = 0;
    } else cyclecolour++;
#else
    inframe = TRUE;

    voffset        =
    voffsetend     = 999;
    flag_cacheable = (flagline && (psu & PSU_F)) ? 1 : 0;
    bgc_cached     = memory[A_BGCOLOUR];

    if (framebased)
    {   spritesdone = FALSE;
        cpuy = 0;
        vblank();

        for (cpux = 0; cpux < n4; cpux++)
        {   changethisbgpixel(bgc);
        }

        for (cpuy = 0; cpuy < n1; cpuy++)
        {   breakrastline();
            dmascreen[cpuy] = memory[A_CHARLINE] & 0x0F;
            arcadia_runcpu();
        }

        unvblank();

        for (cpuy = n1; cpuy < n2; cpuy++)
        {   arcadia_oneraster(); // this calls breakrastline()
            arcadia_runcpu();
        }

        wa_checkinput();
        a_emuinput();

        for (cpuy = n2; cpuy < n3; cpuy++)
        {   arcadia_oneraster(); // this calls breakrastline()
            arcadia_runcpu();
        }

        if (!spritesdone)
        {   do_sprites2();
    }   }
    else
    {   // scanline 0---------------------------------------------------------
        cpuy = 0;
        breakrastline();
        for (cpux = 0; cpux < n4; cpux++) // small amount of semi-normal drawing
        {   if (!dejitter)
            {   flag_cacheable = (flagline && (psu & PSU_F)) ? 1 : 0;
            }
            changethisbgpixel(from_a[flag_cacheable][bgc_cached & 0x07]);
            do_cpu();
        }
        vblank();
        dmascreen[cpuy] = memory[A_CHARLINE] & 0x0F;
        run_cpu(227); // the rest of raster 0
    
        // scanlines 1..42 (PAL) or 1..19 (NTSC)------------------------------
        for (cpuy = 1; cpuy < n1; cpuy++) // in vertical blank
        {   breakrastline();
            dmascreen[cpuy] = memory[A_CHARLINE] & 0x0F;
            run_cpu(227);
        }

        // scanline 43 (PAL) or 20 (NTSC)-------------------------------------
        cpuy = n1;
        breakrastline();
        run_cpu(n4); // small amount of vertical blanking
        unvblank();
        flag_cacheable = (flagline && (psu & PSU_F)) ? 1 : 0;
        bgc_cached = memory[A_BGCOLOUR];
        scrnbgc = from_a[flag_cacheable][bgc_cached & 0x07];
        newraster();
        run_cpu(49); // horizontal blanking
        for (cpux = 49; cpux < 227; cpux++) // normal drawing
        {   onepixel();
        }

        // scanlines 44..311 (PAL) or 21..261 (NTSC)--------------------------
        for (cpuy = n1 + 1; cpuy < n3; cpuy++)
        {   breakrastline();
            for (cpux = 0; cpux < n4; cpux++) // small amount of semi-normal drawing
            {   if (!dejitter)
                {   flag_cacheable = (flagline && (psu & PSU_F)) ? 1 : 0;
                }
                changethisbgpixel(from_a[flag_cacheable][bgc_cached & 0x07]);
                colltable[cpuy][cpux] = 0;
                do_cpu();
            }
            if (cpuy == n2) // ie. USG_YMARGIN + ((n3 - USGMARGIN) / 2). "Conversion takes place during the active scan".
            {   wa_checkinput();
                a_emuinput();
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
    } else udcflip++;

#ifdef SHOWRUMBLE
    if (rumbling[0])
    {   for (y = 0; y < machines[machine].height; y++)
        {   changepixel(                          0, y, 15 - screen[                          0][y]);
    }   }
    if (rumbling[1])
    {   for (y = 0; y < machines[machine].height; y++)
        {   changepixel(machines[machine].width - 1, y, 15 - screen[machines[machine].width - 1][y]);
    }   }
#endif // SHOWRUMBLE
#endif // BENCHMARK_GFX

    if (editscreen)
    {   kx = scrnaddr % 16;
        if (scrnaddr >= 0x1A00)
        {   ky = 13 + ((scrnaddr - 0x1A00) / 16);
        } else
        {   ky =      ((scrnaddr - 0x1800) / 16);
        }
        x1  =       (8 * kx)          +    hoffsets[ky]                               - 1;
        x2  =  x1 +  8                                                                + 1;
        y1  = 255 - memory[A_VSCROLL] + (((memory[A_BGCOLOUR] & 0x80) ? 8 : 16) * ky) - 1;
        y2  =  y1 +                      ((memory[A_BGCOLOUR] & 0x80) ? 8 : 16)       + 1;

        if (boardmode)
        {   if (editcolour & 0x40)
            {   ogc = from_a[flag_cacheable][fgc2];
            } else
            {   ogc = from_a[flag_cacheable][fgc1];
            }
            if (memory[scrnaddr] & 0x40)
            {   tgc = from_a[flag_cacheable][fgc2];
            } else
            {   tgc = from_a[flag_cacheable][fgc1];
            }
            if (memory[scrnaddr] & 0x80)
            {   bgc = from_a[flag_cacheable][innerbgc];
            } else
            {   bgc = from_a[flag_cacheable][outerbgc];
        }   }
        else
        {   ogc = from_a[flag_cacheable][               (editcolour >> 5) + ((memory[A_BGCOLOUR] & 0x08) >> 3)];
            tgc = from_a[flag_cacheable][((memory[scrnaddr] & 0xC0) >> 5) + ((memory[A_BGCOLOUR] & 0x08) >> 3)];
            bgc = from_a[flag_cacheable][memory[A_BGCOLOUR] & 0x07];
        }
        if (tgc == bgc || (!(memory[A_GFXMODE] & 0x40) && scrnaddr >= 0x1A00))
        {   tgc = GREY1;
        }
        if (ogc == bgc || (!(memory[A_GFXMODE] & 0x40) && scrnaddr >= 0x1A00))
        {   ogc = GREY1;
        }

        if (y1 < n3 - n1)
        {   for (x = x1; x <= x2; x++)
            {   changerelpixel(x, y1, ogc);
        }   }
        if (y2 < n3 - n1)
        {   for (x = x1; x <= x2; x++)
            {   changerelpixel(x, y2, ogc);
        }   }
        for (y = y1; y <= y2; y++)
        {   if (y >= n3 - n1)
            {   break;
            }
            changerelpixel(x1, y, ogc);
            changerelpixel(x2, y, ogc);
        }

        blockmode = (memory[A_GFXMODE] & 0x80) >> 7;
        if (kx)
        {   for (x = 0; x < kx; x++)
            {   if (memory[(scrnaddr & 0x1FF0) + x] == 0xC0)
                {   blockmode = 1;
                } elif (memory[(scrnaddr & 0x1FF0) + x] == 0x40)
                {   blockmode = 0;
        }   }   }

        minorrow = 0;
        for (y = y1 + 1; y <= y2 - 1; y++)
        {   if (y >= n3 - n1)
            {   break;
            }
            if (memory[scrnaddr] == 0xC0)
            {   t = arcadia_pdg[blockmode][64][minorrow];
            } elif (memory[scrnaddr] == 0x40)
            {   t = arcadia_pdg[blockmode][65][minorrow];
            } elif (!blockmode && (memory[scrnaddr] & 0x3F) >= 56) // this is a UDG
            {   t = memory[A_OFFSET_SPRITES + (8 * ((memory[scrnaddr] & 0x3F) - 56)) + minorrow];
            } else // this is a PDG
            {   t = arcadia_pdg[blockmode][memory[scrnaddr] & 0x3F][minorrow];
            }
            for (x = 0; x < 8; x++)
            {   if (t & (0x80 >> x))
                {   changerelpixel(x1 + 1 + x, y, tgc);
                } else
                {   changerelpixel(x1 + 1 + x, y, bgc);
            }   }

            if (!(memory[A_BGCOLOUR] & 0x80))
            {   y++;
                if (y < n3 - n1)
                {   for (x = 0; x < 8; x++)
                    {   if (t & (0x80 >> x))
                        {   changerelpixel(x1 + 1 + x, y, tgc);
                        } else
                        {   changerelpixel(x1 + 1 + x, y, bgc);
            }   }   }   }
            minorrow++;
    }   }

    if (drawmode)
    {   arcadia_drawhelpgrid();
    }

    endofframe(bgc);
}

MODULE void a_playerinput(int source, int dest)
{   FAST ULONG jg;

    // dest   is which guest side (0 or 1) you want to set the registers of.
    // source is which host  side (0 or 1) you want to use to do it.

    if (recmode == RECMODE_PLAY)
    {   t = IOBuffer[offset++];
        uviwrite((UWORD) (A_P1PADDLE     -  dest     ),           t              );
        if (bgc_cached & 0x40) // paddle interpolation bit
        {   ax[dest] = sx[dest] = t;
        } else
        {   ay[dest] = sy[dest] = t;
        }
        t = IOBuffer[offset++];
        uviwrite((UWORD) (A_P1LEFTKEYS   + (dest * 4)), (UBYTE) ((t & 0xF0) >> 4)); // bits 7..4 -> bits 3..0
        uviwrite((UWORD) (A_P1MIDDLEKEYS + (dest * 4)), (UBYTE)  (t & 0x0F)      ); // bits 3..0 -> bits 3..0 (masking is indeed necessary)
        t = IOBuffer[offset++];
        uviwrite((UWORD) (A_P1RIGHTKEYS  + (dest * 4)), (UBYTE) ((t & 0xF0) >> 4)); // bits 7..4 -> bits 3..0
        uviwrite((UWORD) (A_P1PALLADIUM  + (dest * 4)), (UBYTE)  (t & 0x0F)      ); // bits 3..0 -> bits 3..0 (masking is indeed necessary)
    } else
    {   // assert(recmode == RECMODE_NORMAL || recmode == RECMODE_RECORD);

        if
        (   (dest == 0 && connected == NET_CLIENT)
         || (dest == 1 && connected == NET_SERVER)
        )
        {   return;
        }

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

        memory[A_P1LEFTKEYS   + (dest * 4)] =
        memory[A_P1MIDDLEKEYS + (dest * 4)] =
        memory[A_P1RIGHTKEYS  + (dest * 4)] =
        memory[A_P1PALLADIUM  + (dest * 4)] = 0;
        if
        (   (jg & (JOYFIRE1 | keytable[key1].dapter))
         || KeyDown(keypads[source][0])
         || (   hostcontroller[source] == CONTROLLER_TRACKBALL
             && lmb
        )   )
        {   if (!autofire[source] || (frames % totalframes) < downframes)
            {   if (whichgame == _3DATTACKPOS && frames % 30 >= 15)
                {   memory[keytable[key2].a_address + (dest * 4)] |= keytable[key2].a_mask; // 2nd firebutton
                } else
                {   memory[keytable[key1].a_address + (dest * 4)] |= keytable[key1].a_mask; // 1st firebutton
        }   }   }
        elif (autofire[source] && !requirebutton[source] && (frames % totalframes) < downframes)
        {   if (whichgame == _3DATTACKPOS && frames % 30 >= 15)
            {   memory[keytable[key2].a_address + (dest * 4)] |= keytable[key2].a_mask; // 2nd firebutton
            } else
            {   memory[keytable[key1].a_address + (dest * 4)] |= keytable[key1].a_mask; // 1st firebutton
        }   }
        if
        (   (jg & JOYFIRE2)
         || (KeyDown(keypads[source][21]))
         || (hostcontroller[source] == CONTROLLER_TRACKBALL && mmb)
        )
        {   // 2nd firebutton
            memory[keytable[key2].a_address + (dest * 4)] |= keytable[key2].a_mask;
        }
        if
        (   (jg & JOYFIRE3)
         || (KeyDown(keypads[source][22]))
         || (hostcontroller[source] == CONTROLLER_TRACKBALL && rmb)
        )
        {   // 3rd firebutton
            memory[keytable[key3].a_address + (dest * 4)] |= keytable[key3].a_mask;
        }
        if
        (   (jg & JOYFIRE4)
         || (KeyDown(keypads[source][23]))
        )
        {   // 4th firebutton
            memory[keytable[key4].a_address + (dest * 4)] |= keytable[key4].a_mask;
        }

        // left column
        t =  memory[A_P1LEFTKEYS + (dest * 4)];
        t |= (UBYTE) (0x08 * KeyDown(keypads[source][ 1])); // "1"  key (Arcadia)
        t |= (UBYTE) (0x04 * KeyDown(keypads[source][ 4])); // "4"  key (Arcadia)
        t |= (UBYTE) (0x02 * KeyDown(keypads[source][ 7])); // "7"  key (Arcadia)
        t |= (UBYTE) (0x01 * KeyDown(keypads[source][10])); // "Cl" key (Arcadia)
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_1) && (jff[source] & DAPTER_1)) t |= 8;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_4) && (jff[source] & DAPTER_4)) t |= 4;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_7) && (jff[source] & DAPTER_7)) t |= 2;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_C) && (jff[source] & DAPTER_C)) t |= 1;

        if (ay[dest] <=  64)
        {   if   (paddleup    ==  1) t |= 8;
            elif (paddleup    ==  4) t |= 4;
            elif (paddleup    ==  7) t |= 2;
            elif (paddleup    == 10) t |= 1;
        } elif (ay[dest] >= 192)
        {   if   (paddledown  ==  1) t |= 8;
            elif (paddledown  ==  4) t |= 4;
            elif (paddledown  ==  7) t |= 2;
            elif (paddledown  == 10) t |= 1;
        }
        if (ax[dest] <=  64)
        {   if   (paddleleft  ==  1) t |= 8;
            elif (paddleleft  ==  4) t |= 4;
            elif (paddleleft  ==  7) t |= 2;
            elif (paddleleft  == 10) t |= 1;
        } elif (ax[dest] >= 192)
        {   if   (paddleright ==  1) t |= 8;
            elif (paddleright ==  4) t |= 4;
            elif (paddleright ==  7) t |= 2;
            elif (paddleright == 10) t |= 1;
        }
        uviwrite((UWORD) (A_P1LEFTKEYS   + (dest * 4)), t);

        // middle column
        t =  memory[A_P1MIDDLEKEYS + (dest * 4)];
        t |= (UBYTE) (0x08 * KeyDown(keypads[source][ 2])); // "2"  key (Arcadia)
        t |= (UBYTE) (0x04 * KeyDown(keypads[source][ 5])); // "5"  key (Arcadia)
        t |= (UBYTE) (0x02 * KeyDown(keypads[source][ 8])); // "8"  key (Arcadia)
        t |= (UBYTE) (0x01 * KeyDown(keypads[source][11])); // "0"  key (Arcadia)
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_2) && (jff[source] & DAPTER_2)) t |= 8;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_5) && (jff[source] & DAPTER_5)) t |= 4;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_8) && (jff[source] & DAPTER_8)) t |= 2;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_0) && (jff[source] & DAPTER_0)) t |= 1;

        if (ay[dest] <=  64)
        {   if   (paddleup    ==  2) t |= 8;
            elif (paddleup    ==  5) t |= 4;
            elif (paddleup    ==  8) t |= 2;
            elif (paddleup    ==  0) t |= 1;
        } elif (ay[dest] >= 192)
        {   if   (paddledown  ==  2) t |= 8;
            elif (paddledown  ==  5) t |= 4;
            elif (paddledown  ==  8) t |= 2;
            elif (paddledown  ==  0) t |= 1;
        }
        if (ax[dest] <=  64)
        {   if   (paddleleft  ==  2) t |= 8;
            elif (paddleleft  ==  5) t |= 4;
            elif (paddleleft  ==  8) t |= 2;
            elif (paddleleft  ==  0) t |= 1;
        } elif (ax[dest] >= 192)
        {   if   (paddleright ==  2) t |= 8;
            elif (paddleright ==  5) t |= 4;
            elif (paddleright ==  8) t |= 2;
            elif (paddleright ==  0) t |= 1;
        }
        uviwrite((UWORD) (A_P1MIDDLEKEYS + (dest * 4)), t);

        // right column
        t =  memory[A_P1RIGHTKEYS + (dest * 4)];
        t |= (UBYTE) (0x08 * KeyDown(keypads[source][ 3])); // "3"  key (Arcadia)
        t |= (UBYTE) (0x04 * KeyDown(keypads[source][ 6])); // "6"  key (Arcadia)
        t |= (UBYTE) (0x02 * KeyDown(keypads[source][ 9])); // "9"  key (Arcadia)
        t |= (UBYTE) (0x01 * KeyDown(keypads[source][12])); // "En" key (Arcadia)
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_3) && (jff[source] & DAPTER_3)) t |= 8;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_6) && (jff[source] & DAPTER_6)) t |= 4;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_9) && (jff[source] & DAPTER_9)) t |= 2;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_E) && (jff[source] & DAPTER_E)) t |= 1;

        if (ay[dest] <=  64)
        {   if   (paddleup    ==  3) t |= 8;
            elif (paddleup    ==  6) t |= 4;
            elif (paddleup    ==  9) t |= 2;
            elif (paddleup    == 11) t |= 1;
        } elif (ay[dest] >= 192)
        {   if   (paddledown  ==  3) t |= 8;
            elif (paddledown  ==  6) t |= 4;
            elif (paddledown  ==  9) t |= 2;
            elif (paddledown  == 11) t |= 1;
        }
        if (ax[dest] <=  64)
        {   if   (paddleleft  ==  3) t |= 8;
            elif (paddleleft  ==  6) t |= 4;
            elif (paddleleft  ==  9) t |= 2;
            elif (paddleleft  == 11) t |= 1;
        } elif (ax[dest] >= 192)
        {   if   (paddleright ==  3) t |= 8;
            elif (paddleright ==  6) t |= 4;
            elif (paddleright ==  9) t |= 2;
            elif (paddleright == 11) t |= 1;
        }
        uviwrite((UWORD) (A_P1RIGHTKEYS  + (dest * 4)), t);

        // Palladium "column"
        t =  memory[A_P1PALLADIUM + (dest * 4)];
        t |= (UBYTE) (0x08 * KeyDown(keypads[source][16])); // "x4" key (Arcadia)
        t |= (UBYTE) (0x04 * KeyDown(keypads[source][15])); // "x3" key (Arcadia)
        t |= (UBYTE) (0x02 * KeyDown(keypads[source][14])); // "x2" key (Arcadia)
        t |= (UBYTE) (0x01 * KeyDown(keypads[source][13])); // "x1" key (Arcadia)

        if (ay[dest] <=  64)
        {   if   (paddleup    == 15) t |= 8;
            elif (paddleup    == 14) t |= 4;
            elif (paddleup    == 13) t |= 2;
            elif (paddleup    == 12) t |= 1;
        } elif (ay[dest] >= 192)
        {   if   (paddledown  == 15) t |= 8;
            elif (paddledown  == 14) t |= 4;
            elif (paddledown  == 13) t |= 2;
            elif (paddledown  == 12) t |= 1;
        }
        if (ax[dest] <=  64)
        {   if   (paddleleft  == 15) t |= 8;
            elif (paddleleft  == 14) t |= 4;
            elif (paddleleft  == 13) t |= 2;
            elif (paddleleft  == 12) t |= 1;
        } elif (ax[dest] >= 192)
        {   if   (paddleright == 15) t |= 8;
            elif (paddleright == 14) t |= 4;
            elif (paddleright == 13) t |= 2;
            elif (paddleright == 12) t |= 1;
        }
        uviwrite((UWORD) (A_P1PALLADIUM + (dest * 4)), t);

        engine_dopaddle(source, dest);

        if (bgc_cached & 0x40) // paddle interpolation bit
        {   uviwrite((UWORD) (A_P1PADDLE - dest), (UBYTE) ax[dest]);
            sx[dest] = (UBYTE) ax[dest];
        } else
        {   uviwrite((UWORD) (A_P1PADDLE - dest), (UBYTE) ay[dest]);
            sy[dest] = (UBYTE) ay[dest];
}   }   }

MODULE __inline void a_emuinput(void)
{   UBYTE therecv[5],
          thesend[5];

    ReadJoystick(0);
    ReadJoystick(1);
    // must always do a_playerinput(foo, 0) then a_playerinput(foo, 1).
    if (swapped)
    {   a_playerinput(1, 0);
        a_playerinput(0, 1);
    } else
    {   a_playerinput(0, 0);
        a_playerinput(1, 1);
    }
    p1rumble =
    p2rumble = 0;

    if (recmode != RECMODE_PLAY)
    {   // assert(recmode == RECMODE_NORMAL || recmode == RECMODE_RECORD);

        if (KeyDown(console[0]) || (jff[0] & JOYSTART) || (jff[1] & JOYSTART) || console_start) { t =  1; } else { t = 0; }
        if (KeyDown(console[1]) || (jff[0] & JOYA    ) || (jff[1] & JOYA    ) || console_a    ) { t |= 2; }
        if (KeyDown(console[2]) || (jff[0] & JOYB    ) || (jff[1] & JOYB    ) || console_b    ) { t |= 4; }

        uviwrite(A_CONSOLE, t);

        domouse();
    }

    /* 1st argument is pointer to receive buffer.
       2nd argument is pointer to send buffer.
       3rd argument is size in bytes of each buffer.

    Server is left player, client is right player.
    4 bytes per frame (27 significant bits) are exchanged for Arcadia.
    3 bytes per frame (24 significant bits) are exchanged for Interton/Elektor. */

    if (connected == NET_SERVER)
    {   // we are left player

        thesend[0] = ((memory[A_P1LEFTKEYS  ] & 0x0F) <<  4) // bits 3..0 -> bits  7..4 (masking not really needed)
                   |  (memory[A_P1MIDDLEKEYS] & 0x0F);       // bits 3..0 -> bits  3..0 (masking not really needed)
        thesend[1] = ((memory[A_P1RIGHTKEYS ] & 0x0F) <<  4) // bits 3..0 -> bits  7..4 (masking not really needed)
                   |  (memory[A_P1PALLADIUM ] & 0x0F);       // bits 3..0 -> bits  3..0 (masking not really needed)
        thesend[2] =   memory[A_P1PADDLE];                   // bits 7..0 -> bits  7..0
        thesend[3] =   memory[A_CONSOLE];                    // bits 7..0 -> bits  7..0
        thesend[4] = strlen((const char*) netmsg_out);
#ifdef LOGNETWORK
        zprintf(TEXTPEN_VERBOSE, "Server: Sending  $%02X, $%02X, $%02X, $%02X, %d.\n", thesend[0], thesend[1], thesend[2], thesend[3], thesend[4]);
#endif
        DISCARD NetSend((const char*) thesend, 5);
        send_usermsg((int) thesend[4]);

        if (NetRecv((char*) therecv, 5))
        {
#ifdef LOGNETWORK
            zprintf(TEXTPEN_VERBOSE, "Server: Received $%02X, $%02X, $%02X, $%02X, %d.\n", therecv[0], therecv[1], therecv[2], therecv[3], therecv[4]);
#endif
            uviwrite(A_P2LEFTKEYS  , (UBYTE) ((therecv[0] & 0xF0) >> 4)); // bits 7..4 -> bits 3..0
            uviwrite(A_P2MIDDLEKEYS, (UBYTE)  (therecv[0] & 0x0F)      ); // bits 3..0 -> bits 3..0
            uviwrite(A_P2RIGHTKEYS , (UBYTE) ((therecv[1] & 0xF0) >> 4)); // bits 7..4 -> bits 3..0
            uviwrite(A_P2PALLADIUM , (UBYTE)  (therecv[1] & 0x0F)      ); // bits 3..0 -> bits 3..0
            uviwrite(A_P2PADDLE    ,           therecv[2]              ); // bits 7..0 -> bits 7..0
            recv_usermsg((int) therecv[4]);
    }   }
    elif (connected == NET_CLIENT)
    {   // we are right player

        thesend[0] = ((memory[A_P2LEFTKEYS  ] & 0x0F) <<  4) // bits 3..0 -> bits  7..4 (masking not really needed)
                   |  (memory[A_P2MIDDLEKEYS] & 0x0F);       // bits 3..0 -> bits  3..0 (masking not really needed)
        thesend[1] = ((memory[A_P2RIGHTKEYS ] & 0x0F) <<  4) // bits 3..0 -> bits  7..4 (masking not really needed)
                   |  (memory[A_P2PALLADIUM ] & 0x0F);       // bits 3..0 -> bits  3..0 (masking not really needed)
        thesend[2] =   memory[A_P2PADDLE];                   // bits 7..0 -> bits  7..0
        thesend[3] = 0; // not used
        thesend[4] = strlen((const char*) netmsg_out);
#ifdef LOGNETWORK
        zprintf(TEXTPEN_VERBOSE, "Client: Sending  $%02X, $%02X, $%02X, $%02X, %d.\n", thesend[0], thesend[1], thesend[2], thesend[3], thesend[4]);
#endif
        DISCARD NetSend((const char*) thesend, 5);
        send_usermsg((int) thesend[4]);

        if (NetRecv((char*) therecv, 5))
        {
#ifdef LOGNETWORK
            zprintf(TEXTPEN_VERBOSE, "Client: Received $%02X, $%02X, $%02X, $%02X, %d.\n", therecv[0], therecv[1], therecv[2], therecv[3], therecv[4]);
#endif
            uviwrite(A_P1LEFTKEYS  , (UBYTE) ((therecv[0] & 0xF0) >> 4)); // bits 7..4 -> bits 3..0
            uviwrite(A_P1MIDDLEKEYS, (UBYTE)  (therecv[0] & 0x0F)      ); // bits 3..0 -> bits 3..0
            uviwrite(A_P1RIGHTKEYS , (UBYTE) ((therecv[1] & 0xF0) >> 4)); // bits 7..4 -> bits 3..0
            uviwrite(A_P1PALLADIUM , (UBYTE)  (therecv[1] & 0x0F)      ); // bits 3..0 -> bits 3..0
            uviwrite(A_P1PADDLE    ,           therecv[2]              ); // bits 7..0 -> bits 7..0
            uviwrite(A_CONSOLE     ,           therecv[3]              ); // bits 7..0 -> bits 7..0
            recv_usermsg((int) therecv[4]);
    }   }

    if (recmode == RECMODE_PLAY)
    {   uviwrite(A_CONSOLE, IOBuffer[offset++]);
    } elif (recmode == RECMODE_RECORD)
    {   // assert(MacroHandle);

        OutputBuffer[0] =   memory[A_P1PADDLE];                   // bits 7..0 -> bits  7..0
        OutputBuffer[1] = ((memory[A_P1LEFTKEYS  ] & 0x0F) <<  4) // bits 3..0 -> bits  7..4 (masking not really needed)
                        |  (memory[A_P1MIDDLEKEYS] & 0x0F);       // bits 3..0 -> bits  3..0 (masking not really needed)
        OutputBuffer[2] = ((memory[A_P1RIGHTKEYS ] & 0x0F) <<  4) // bits 3..0 -> bits  7..4 (masking not really needed)
                        |  (memory[A_P1PALLADIUM ] & 0x0F);       // bits 3..0 -> bits  3..0 (masking not really needed)
        OutputBuffer[3] =   memory[A_P2PADDLE];                   // bits 7..0 -> bits  7..0
        OutputBuffer[4] = ((memory[A_P2LEFTKEYS  ] & 0x0F) <<  4) // bits 3..0 -> bits  7..4 (masking not really needed)
                        |  (memory[A_P2MIDDLEKEYS] & 0x0F);       // bits 3..0 -> bits  3..0 (masking not really needed)
        OutputBuffer[5] = ((memory[A_P2RIGHTKEYS ] & 0x0F) <<  4) // bits 3..0 -> bits  7..4 (masking not really needed)
                        |  (memory[A_P2PALLADIUM ] & 0x0F);       // bits 3..0 -> bits  3..0 (masking not really needed)
        OutputBuffer[6] =   memory[A_CONSOLE];                    // bits 7..0 -> bits  7..0 (5 bits are wasted)

        DISCARD fwrite(OutputBuffer, 7, 1, MacroHandle);
        // should really check return code of fwrite()
        // 7 bytes per frame * 60 frames per second = 420 bytes/sec.
    }

    if (console_start) console_start--;
    if (console_a    ) console_a--;
    if (console_b    ) console_b--;
    if (console_reset) console_reset--;
}

EXPORT void uviwrite(UWORD address, UBYTE data)
{   // assert(address < 32768);

    /* Writes to memory from the UVI always go through this
    routine (so that watchpoints work properly).
      No mirroring is done here; the emulator is expected to use the
    resolved address.
      If you have a watchpoint on a mirrored address, eg. $3000,
    then ideally an access of $1000 by the UVI should trigger the.
    watchpoint. The easiest way to deal with this would be to resolve
    the address early (during the "set watchpoint" requester). The same
    applies, of course, for the PVI. */

    if
    (   (memflags[address] & WATCHPOINT)
     &&  watchwrites != WATCH_NONE
     && (watchwrites == WATCH_ALL || data != memory[address])
     && conditional(&wp[address], data, TRUE, 0)
    )
    {   arcadia_stringchar(data, address);
        DISCARD getfriendly((int) address);
        zprintf
        (   TEXTPEN_DEBUG,
            LLL(MSG_UVI_HITWP, "UVI is writing $%X%s to %s at raster %ld!\n\n"),
                       data,
                       stringchar,
            (char*)    friendly,
            (long int) cpuy
        );
        set_pause(TYPE_BP);
    }

    memory[address] = data;
}

EXPORT void arcadia_setmemmap(void)
{   int i, address, mirror;

    game = FALSE;

    if (randomizememory)
    {   for (i = 0; i <= 0x7FFF; i++)
        {   memory[i] = rand() & 0xFF; // ie. rand() % 256
        }
        for (i = 0x1909; i <= 0x190F; i++)
        {   memory[i] = (rand() & 1) ? 0xC0 : 0x40;
    }   }
    else
    {   for (i = 0; i <= 0x7FFF; i++)
        {   memory[i] = 0;
        }
        for (i = 0x1909; i <= 0x190F; i++)
        {   memory[i] = 0x40;
    }   }

    if (usestubs)
    {   for (i = 0; i < 348; i++)
        {   memory[i] = a_bios[i];
    }   }
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

        if
        (   (address >= 0x18F0 && address <= 0x18F7)
         || (address >= 0x18FC && address <= 0x1908)
         || (address >= 0x19F8 && address <= 0x19FF)
        )
        {   memflags[address] |= ASIC;
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

EXPORT void arcadia_viewscreen(void)
{   int y;

    // Ideally, we would show the text in the correct colours

    for (y = 0; y <= 12; y++)
    {   dumprow(A_OFFSET_UPPERSCREEN + (y * 16));
    }
    for (y = 0; y <= 12; y++)
    {   dumprow(A_OFFSET_LOWERSCREEN + (y * 16));
    }
    zprintf(TEXTPEN_VIEW, "\n");
}

EXPORT void arcadia_serializerom(void)
{   int i;

    /* This is for Palladium (ie. memmap "I"). Emerson ("G") and Tele-
       Fever ("H") are different (see EA2001 Coding Guide). */

    // assert(machine == ARCADIA);
    // assert(filesize <= 31 * KILOBYTE);
    // assert(serializemode == SERIALIZE_READ);

    if (filesize <= 4 * KILOBYTE)
    {   for (i = 0; i < filesize; i++)
        {   serialize_byte(&memory[i]);                // file $0000..$0FFF -> memory $0000..$0FFF
        }
        goto DONE;
    }
    for (i = 0; i < 4 * KILOBYTE; i++)
    {   serialize_byte(&memory[i]);                    // file $0000..$0FFF -> memory $0000..$0FFF
    }

    if (filesize <= 8 * KILOBYTE)
    {   for (i = 0; i < filesize - (4 * KILOBYTE); i++)
        {   serialize_byte(&memory[0x2000 + i]);       // file $1000..$1FFF -> memory $2000..$2FFF
        }
        goto DONE;
    }
    for (i = 0; i < 4 * KILOBYTE; i++)
    {   serialize_byte(&memory[0x2000 + i]);           // file $1000..$1FFF -> memory $2000..$2FFF
    }

    if (filesize <= 12 * KILOBYTE)
    {   for (i = 0; i < filesize - (8 * KILOBYTE); i++)
        {   serialize_byte(&memory[0x4000 + i]);       // file $2000..$2FFF -> memory $4000..$4FFF
        }
        goto DONE;
    }
    for (i = 0; i < 4 * KILOBYTE; i++)
    {   serialize_byte(&memory[0x4000 + i]);           // file $2000..$2FFF -> memory $4000..$4FFF
    }

    // arbitrary...

    if (filesize <= 16 * KILOBYTE)
    {   for (i = 0; i < filesize - (12 * KILOBYTE); i++)
        {   serialize_byte(&memory[0x6000 + i]);       // file $3000..$3FFF -> memory $6000..$6FFF
        }
        goto DONE;
    }
    for (i = 0; i < 4 * KILOBYTE; i++)
    {   serialize_byte(&memory[0x6000 + i]);           // file $3000..$3FFF -> memory $6000..$6FFF
    }

    if (filesize <= 20 * KILOBYTE)
    {   for (i = 0; i < filesize - (16 * KILOBYTE); i++)
        {   serialize_byte(&memory[0x3000 + i]);       // file $4000..$4FFF -> memory $3000..$3FFF
        }
        goto DONE;
    }
    for (i = 0; i < 4 * KILOBYTE; i++)
    {   serialize_byte(&memory[0x3000 + i]);           // file $4000..$4FFF -> memory $3000..$3FFF
    }

    if (filesize <= 24 * KILOBYTE)
    {   for (i = 0; i < filesize - (20 * KILOBYTE); i++)
        {   serialize_byte(&memory[0x5000 + i]);       // file $5000..$5FFF -> memory $5000..$5FFF
        }
        goto DONE;
    }
    for (i = 0; i < 4 * KILOBYTE; i++)
    {   serialize_byte(&memory[0x5000 + i]);           // file $5000..$5FFF -> memory $5000..$5FFF
    }

    if (filesize <= 28 * KILOBYTE)
    {   for (i = 0; i < filesize - (24 * KILOBYTE); i++)
        {   serialize_byte(&memory[0x7000 + i]);       // file $6000..$6FFF -> memory $7000..$7FFF
        }
        goto DONE;
    }
    for (i = 0; i < 4 * KILOBYTE; i++)
    {   serialize_byte(&memory[0x7000 + i]);           // file $6000..$6FFF -> memory $7000..$7FFF
    }

    if (filesize <= 30 * KILOBYTE)
    {   for (i = 0; i < filesize - (28 * KILOBYTE); i++)
        {   serialize_byte(&memory[0x1000 + i]);       // file $7000..$77FF -> memory $1000..$17FF
        }
        goto DONE;
    }
    for (i = 0; i < 2 * KILOBYTE; i++)
    {   serialize_byte(&memory[0x1000 + i]);           // file $7000..$77FF -> memory $1000..$17FF
    }

    for (i = 0; i < filesize - (30 * KILOBYTE); i++)
    {   serialize_byte(&memory[0x1C00 + i]);           // file $7800..$7BFF -> memory $1C00..$1FFF
    }

DONE:
    if (whichgame == A_GOLFPOS1 || whichgame == A_GOLFODPOS)
    {   // This is for unpatched Arcadia Golf
        // It mirrors $2000..$27FF at $4000..$47FF
        for (i = 0; i < 2 * KILOBYTE; i++)
        {   memory[0x4000 + i] = memory[0x2000 + i];
}   }   }

EXPORT void arcadia_view_udgs(void)
{   int   localflagging,
          row,
          whichsprite;
    TEXT  bits[8 + 1];
    UBYTE thecolour = 0,
          thesize   = 0; // both initialized to prevent spurious SAS/C warnings

    // assert(machine == ARCADIA);

    bits[8] = EOS;
    localflagging = (flagline && (psu & PSU_F)) ? 1 : 0;

    for (row = 0; row < 8; row++)
    {   for (whichsprite = 0; whichsprite < 4; whichsprite++)
        {   for (column = 0; column < 8; column++)
            {   bits[column] = ((memory[A_OFFSET_SPRITES + (whichsprite * 8) + row] & (0x80 >> column)) ? '#' : '.');
            }

            switch (whichsprite)
            {
            case  0:
                thecolour = (memory[A_SPRITES01CTRL] & 0x38) >> 3;
                thesize   = (memory[A_SPRITES01CTRL] & 0x80) >> 7;
            acase 1:
                thecolour =  memory[A_SPRITES01CTRL] & 0x07      ;
                thesize   = (memory[A_SPRITES01CTRL] & 0x40) >> 6;
            acase 2:
                thecolour = (memory[A_SPRITES23CTRL] & 0x38) >> 3;
                thesize   = (memory[A_SPRITES23CTRL] & 0x80) >> 7;
            acase 3:
                thecolour =  memory[A_SPRITES23CTRL] & 0x07      ;
                thesize   = (memory[A_SPRITES23CTRL] & 0x40) >> 6;
            }
            thecolour = from_a[localflagging][thecolour];

            switch (row)
            {
            case 0:
                zprintf
                (   guest_to_ansi_colour[thecolour],
                    " SPRITE%d: %s ",
                    whichsprite,
                    bits
                );
            acase 1:
                switch (thesize)
                {
                case 0:
                    zprintf
                    (   guest_to_ansi_colour[thecolour],
                        "    (8*8) %s ",
                        bits
                    );
                acase 1:
                    zprintf
                    (   guest_to_ansi_colour[thecolour],
                        "   (8*16) %s ",
                        bits
                    );
                }
            acase 2:
                zprintf
                (   guest_to_ansi_colour[thecolour],
                    "%9s %s ",
                    colourname[thecolour],
                    bits
                );
            adefault:
                zprintf
                (   guest_to_ansi_colour[thecolour],
                    "          %s ",
                    bits
                );
        }   }
        zprintf(guest_to_ansi_colour[thecolour], "\n");
    }

    zprintf(guest_to_ansi_colour[thecolour], "\n");

    for (row = 0; row < 8; row++)
    {   for (whichsprite = 0; whichsprite < 4; whichsprite++)
        {   for (column = 0; column < 8; column++)
            {   if (memory[A_OFFSET_SPRITES + ((whichsprite + 4) * 8) + row] & (0x80 >> column))
                {   bits[column] = '#';
                } else
                {   bits[column] = '.';
            }   }

            if (memory[A_PITCH] & 0x80) // board mode
            {   thecolour = (memory[A_GFXMODE ] &  0x38) >> 3;
            } else
            {   thecolour = (memory[A_BGCOLOUR] >>    3) &  1; // 0 or 1
            }
            if (flagline && (psu & PSU_F))
            {   thecolour = 7 - thecolour;
            }

            switch (row)
            {
            case 0:
                zprintf
                (   guest_to_ansi_colour[thecolour],
                    "    UDG%d: %s ",
                    whichsprite,
                    bits
                );
            acase 1:
                if (memory[A_BGCOLOUR] & 0x80)
                {   zprintf
                    (   guest_to_ansi_colour[thecolour],
                        "    (8*8) %s ",
                        bits
                    );
                } else
                {   zprintf
                    (   guest_to_ansi_colour[thecolour],
                        "   (8*16) %s ",
                        bits
                    );
                }
            acase 2:
            case 3:
            case 4:
            case 5:
                if (memory[A_PITCH] & 0x80) // board mode
                {   if (row >= 4)
                    {   zprintf
                        (   guest_to_ansi_colour[thecolour],
                            "          %s ",
                            bits
                        );
                        break;
                    }
                    if (row == 2)
                    {   thecolour = (memory[A_GFXMODE ] & 0x38) >> 3;
                    } else
                    {   // assert(row == 3);
                        thecolour = (memory[A_BGCOLOUR] & 0x38) >> 3;
                }   }
                else
                {   thecolour = ((row - 2) * 2)                  // 2,3,4,5 -> 0,2,4,6
                              + ((memory[A_BGCOLOUR] >> 3) & 1); // 0 or 1
                }
                if (localflagging)
                {   thecolour = 7 - thecolour;
                }
                zprintf
                (   guest_to_ansi_colour[from_a[0][thecolour]],
                    "%9s %s ",
                    colourname[from_a[0][thecolour]],
                    bits
                );
            adefault:
                zprintf
                (   guest_to_ansi_colour[thecolour],
                    "          %s ",
                    bits
                );
        }   }
        zprintf(guest_to_ansi_colour[thecolour], "\n");
    }

    zprintf(guest_to_ansi_colour[thecolour], "\n");
}

EXPORT void arcadia_drawhelpgrid(void)
{   int i,
        lines,
        rows,
        x, y, yy;

    // assert(machine == ARCADIA);

    switch (drawmode)
    {
    case 1:
        // we really should use the values from last frame rather than the current values
        y       = 255 - memory[A_VSCROLL];
        lines   = (memory[A_BGCOLOUR] & 0x80) ?  8 : 16;
        rows    = (memory[A_GFXMODE ] & 0x40) ? 26 : 13;

        for (i = 0; i < rows; i++)
        {   if (y < n3 - n1)
            {   for (        x = hoffsets[i]; x <= hoffsets[i] + 128; x++   ) // horizontal lines
                {   changerelpixel(x, y, GREY1);
                }
                for (yy = 1; yy < lines; yy++)
                {   if (y + yy < n3 - n1)
                    {   for (x = hoffsets[i]; x <= hoffsets[i] + 128; x += 8) // vertical   lines
                        {   changerelpixel(x, y + yy, GREY1);
                }   }   }
                y += lines;
                if (y < n3 - n1)
                {   for (    x = hoffsets[i]; x <= hoffsets[i] + 128; x++   ) // horizontal lines
                    {   changerelpixel(x, y, GREY1);
            }   }   }
            else
            {   break;
        }   }

        if (!usemargins)
        {   return;
        }

        if (region == REGION_NTSC)
        {   for (x = -49; x <= -40; x++)
            {   changerelpixel(x, -20, GREY4);
                for (y = 1; y <= 241; y++)
                {   changerelpixel(x, y, GREY4);
            }   }
            for (x =   0; x <=   5; x++)
            {   for (y = 0; y <= 241; y++)
                {   changerelpixel(x, y, GREY4);
            }   }
            for (x = 170; x <= 177; x++)
            {   for (y = 0; y <= 241; y++)
                {   changerelpixel(x, y, GREY4);
        }   }   }
        else
        {   // assert(region == REGION_PAL);
            for (x = -49; x <= -44; x++)
            {   changerelpixel(x, -43, GREY4);
                for (y = 1; y <= 268; y++)
                {   changerelpixel(x, y, GREY4);
            }   }
            for (x =   0; x <=   5; x++)
            {   for (y = 0; y <= 268; y++)
                {   changerelpixel(x, y, GREY4);
            }   }
            for (x = 170; x <= 177; x++)
            {   for (y = 0; y <= 268; y++)
                {   changerelpixel(x, y, GREY4);
        }   }   }
    acase 2:
        for (x = 0; x < n4; x++)
        {   changeabspixel(    x, 0 , colltable[0 ][x] % GUESTCOLOURS);
        }
        for (x = 49; x < 227; x++)
        {   changeabspixel(    x, n1, colltable[n1][x] % GUESTCOLOURS);
        }
        for (y = n1 + 1; y < n3; y++)
        {   for (x = 0; x < n4; x++)
            {   changeabspixel(x, y , colltable[y ][x] % GUESTCOLOURS);
            }
            for (x = 49; x < 227; x++)
            {   changeabspixel(x, y , colltable[y ][x] % GUESTCOLOURS);
        }   }
    acase 3:
        for (x = 0; x < n4; x++)
        {   if (!colltable[0][x])
            {   changeabspixel(x, 0, dmascreen[0]);
        }   }
        for (x = 49; x < 227; x++)
        {   if (!colltable[n1][x])
            {   changeabspixel(x, n1, dmascreen[n1]);
        }   }
        for (y = n1 + 1; y < n3; y++)
        {   for (x = 0; x < n4; x++)
            {   if (!colltable[y][x])
                {   changeabspixel(x, y, dmascreen[y]);
            }   }
            for (x = 49; x < 227; x++)
            {   if (!colltable[y][x])
                {   changeabspixel(x, y, dmascreen[y]);
}   }   }   }   }

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
        colltable[cpuy][cpux] = 0;
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
                colltable[cpuy][cpux] = 0x10;
                changethisabspixel(t);
            } elif (thechar & 0x80)
            {   t = from_a[flag_cacheable][innerbgc];
                colltable[cpuy][cpux] = 0x10;
                changethisabspixel(t);
            } else
            {   t = from_a[flag_cacheable][outerbgc];
                colltable[cpuy][cpux] = 0;
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
                }
                colltable[cpuy][cpux] = 0x10;
            } else
            {   changethisbgpixel(bgc);
                colltable[cpuy][cpux] = 0;
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
        colltable[cpuy][cpux] |= 1;
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
        colltable[cpuy][cpux] |= 2;
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
        colltable[cpuy][cpux] |= 4;
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
        colltable[cpuy][cpux] |= 8;
    }

    if (sprpixel)
    {   if (collisions)
        {   sprcollisions = 0;
            if ((sprpixel & 3) == 3) // %0011 #0 & #1
            {   sprcollisions |= 1;
                if (p1sprcol[0]) p1rumble = p1sprcol[0];
                if (p2sprcol[0]) p2rumble = p2sprcol[0];
#ifdef LOGSPRITECOLLISIONS
                zprintf(TEXTPEN_VERBOSE, "Sprites #0 & #1 have collided!\n");
#endif
            }
            if ((sprpixel & 5) == 5) // %0101 #0 & #2
            {   sprcollisions |= 2;
                if (p1sprcol[1]) p1rumble = p1sprcol[1];
                if (p2sprcol[1]) p2rumble = p2sprcol[1];
#ifdef LOGSPRITECOLLISIONS
                zprintf(TEXTPEN_VERBOSE, "Sprites #0 & #2 have collided!\n");
#endif
            }
            if ((sprpixel & 9) == 9) // %1001 #0 & #3
            {   sprcollisions |= 4;
                if (p1sprcol[2]) p1rumble = p1sprcol[2];
                if (p2sprcol[2]) p2rumble = p2sprcol[2];
#ifdef LOGSPRITECOLLISIONS
                zprintf(TEXTPEN_VERBOSE, "Sprites #0 & #3 have collided!\n");
#endif
            }
            if ((sprpixel & 6) == 6) // %0110 #1 & #2
            {   sprcollisions |= 8;
                if (p1sprcol[3]) p1rumble = p1sprcol[3];
                if (p2sprcol[3]) p2rumble = p2sprcol[3];
#ifdef LOGSPRITECOLLISIONS
                zprintf(TEXTPEN_VERBOSE, "Sprites #1 & #2 have collided!\n");
#endif
            }
            if ((sprpixel & 10) == 10) // %1010 #1 & #3
            {   sprcollisions |= 16;
                if (p1sprcol[4]) p1rumble = p1sprcol[4];
                if (p2sprcol[4]) p2rumble = p2sprcol[4];
#ifdef LOGSPRITECOLLISIONS
                zprintf(TEXTPEN_VERBOSE, "Sprites #1 & #3 have collided!\n");
#endif
            }
            if ((sprpixel & 12) == 12) // %1100 #2 & #3
            {   sprcollisions |= 32;
                if (p1sprcol[5]) p1rumble = p1sprcol[5];
                if (p2sprcol[5]) p2rumble = p2sprcol[5];
#ifdef LOGSPRITECOLLISIONS
                zprintf(TEXTPEN_VERBOSE, "Sprites #2 & #3 have collided!\n");
#endif
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
    {   if (framebased)
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

    if (!spriteflips || !demultiplex || frames < (ULONG) spriteflips || spritemode == SPRITEMODE_INVISIBLE)
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
    dmascreen[cpuy] = memory[A_CHARLINE] & 0x0F;
    if (memory[A_CHARLINE] == 0xFD || memory[A_CHARLINE] == 0xFF)
    {   bgc_cached = memory[A_BGCOLOUR];
    }

    if (!framebased)
    {   switch (spritemode)
        {
        case  SPRITEMODE_INVISIBLE:
            sprimagedata[0] = sprimagedata[1] = sprimagedata[2] = sprimagedata[3] = 0;
        acase SPRITEMODE_NUMBERED:
            sprimagedata[0] = sprnumbers[     0][        (cpuy - spr[spriteflip][0].y) >> (spr[spriteflip][0].tall ? 1 : 0)];
            sprimagedata[1] = sprnumbers[     1][        (cpuy - spr[spriteflip][1].y) >> (spr[spriteflip][1].tall ? 1 : 0)];
            sprimagedata[2] = sprnumbers[     2][        (cpuy - spr[spriteflip][2].y) >> (spr[spriteflip][2].tall ? 1 : 0)];
            sprimagedata[3] = sprnumbers[     3][        (cpuy - spr[spriteflip][3].y) >> (spr[spriteflip][3].tall ? 1 : 0)];
        acase SPRITEMODE_VISIBLE:
            sprimagedata[0] = spr[spriteflip][0].imagery[(cpuy - spr[spriteflip][0].y) >> (spr[spriteflip][0].tall ? 1 : 0)];
            sprimagedata[1] = spr[spriteflip][1].imagery[(cpuy - spr[spriteflip][1].y) >> (spr[spriteflip][1].tall ? 1 : 0)];
            sprimagedata[2] = spr[spriteflip][2].imagery[(cpuy - spr[spriteflip][2].y) >> (spr[spriteflip][2].tall ? 1 : 0)];
            sprimagedata[3] = spr[spriteflip][3].imagery[(cpuy - spr[spriteflip][3].y) >> (spr[spriteflip][3].tall ? 1 : 0)];
    }   }

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

#ifdef EMULATE_CPU
    if (cpux == nextinst)
    {   oldcycles = cycles_2650;
        checkstep();
        one_instruction();
        nextinst += (cycles_2650 - oldcycles) * 4; // in pixels
        if (nextinst >= 227)
        {   nextinst -= cpl;
    }   }
#endif
}

EXPORT FLAG arcadia_edit_screen(UWORD code)
{   int i;

    switch (code)
    {
    case  SCAN_UP:               if (shift())
                                 {   scrnaddr = 0x1800 + (scrnaddr % 16);
                                 } else
                                 {   if     (scrnaddr >= 0x1800 && scrnaddr <= 0x180F)
                                     {   scrnaddr += 0x2C0; // -> $1ACx
                                     } elif (scrnaddr >= 0x1A00 && scrnaddr <= 0x1A0F)
                                     {   scrnaddr -= 0x140; // -> $18Cx
                                     } else
                                     {   scrnaddr -= 16;
                                 }   }
    acase SCAN_DOWN:             if (shift())
                                 {   scrnaddr = 0x1AC0 + (scrnaddr % 16);
                                 } else
                                 {   if     (scrnaddr >= 0x18C0 && scrnaddr <= 0x18CF)
                                     {   scrnaddr += 0x140; // -> $1A0x
                                     } elif (scrnaddr >= 0x1AC0 && scrnaddr <= 0x1ACF)
                                     {   scrnaddr -= 0x2C0; // -> $180x
                                     } else
                                     {   scrnaddr += 16;
                                 }   }
    acase SCAN_LEFT:             if (shift())
                                 {   scrnaddr &= 0x7FF0;
                                 } elif ((scrnaddr & 0x000F) ==  0)
                                 {   scrnaddr += 15;
                                 } else
                                 {   scrnaddr--;
                                 }
    acase SCAN_RIGHT:            if (shift())
                                 {   scrnaddr |= 0x000F;
                                 } elif ((scrnaddr & 0x000F) == 15)
                                 {   scrnaddr -= 15;
                                 } else
                                 {   scrnaddr++;
                                 }
    acase SCAN_OB:               if (shift())
                                 {   if (memory[A_GFXMODE] & 0x40)
                                     {   memory[A_GFXMODE] &= 0xBF;
                                     } else
                                     {   memory[A_GFXMODE] |= 0x40;
                                     }
                                     setstatus("Toggle 13/26 character rows");
                                 } else
                                 {   editcolour -= 0x40;
                                     setstatus("Change (2nd) foreground colour backwards");
                                 }
    acase SCAN_CB:               if (shift())
                                 {   if (memory[A_BGCOLOUR] & 0x80)
                                     {   memory[A_BGCOLOUR] &= 0x7F;
                                     } else
                                     {   memory[A_BGCOLOUR] |= 0x80;
                                     }
                                     setstatus("Toggle low/high resolution");
                                 } else
                                 {   editcolour += 0x40;
                                     setstatus("Change (2nd) foreground colour forwards");
                                 }
    acase SCAN_BACKSPACE:        if   (scrnaddr == 0x1800) scrnaddr = 0x1ACF;
                                 elif (scrnaddr == 0x1A00) scrnaddr = 0x18CF;
                                 else  scrnaddr--;
                                 memory[scrnaddr] = 0x00;
    acase SCAN_DEL:              if (shift())
                                 {   for (i = 0x1800; i <= 0x18CF; i++)
                                     {   memory[i] = memory[i + 0x200] = 0;
                                     }
                                     setstatus("Clear screen");
                                 } else
                                 {   memory[scrnaddr] = 0x00;
                                 }
    acase SCAN_AE: case SCAN_NE: scrnaddr += 16 - (scrnaddr % 16);
                                 if   (scrnaddr == 0x18D0) scrnaddr = 0x1A00;
                                 elif (scrnaddr == 0x1AD0) scrnaddr = 0x1800;
#ifdef WIN32
    acase SCAN_HOME:             if (shift()) scrnaddr = 0x1800; else scrnaddr &= 0x7FF0;
    acase SCAN_END:              if (shift()) scrnaddr = 0x1ACF; else scrnaddr |= 0x000F;
    acase SCAN_PGUP:             memory[scrnaddr]++; setstatus("Increment screen memory"); // overflow  is OK
    acase SCAN_PGDN:             memory[scrnaddr]--; setstatus("Decrement screen memory"); // underflow is OK
#endif
#ifdef AMIGA
    acase SCAN_HELP:             if (shift())
                                 {   memory[scrnaddr]--; // underflow is OK
                                 } else
                                 {   memory[scrnaddr]++; // overflow  is OK
                                 }
#endif
    acase SCAN_ESCAPE:           editscreen = FALSE;
                                 updatemenus();
                                 settitle();
                                 dogamename();
    adefault:
        switch (code)
        {
        case  SCAN_SPACEBAR:         memory[scrnaddr] = shift() ? 0x03 : 0x00;
        acase SCAN_SLASH:            if (shift()) return FALSE; else memory[scrnaddr] = 0x01;
        acase SCAN_SL:               memory[scrnaddr] = 0x01;
        acase SCAN_BACKSLASH:        memory[scrnaddr] = 0x02;
        acase SCAN_AMINUS:           if (shift()) memory[scrnaddr] = 0x06; else return FALSE;
        acase SCAN_A1:               if (shift()) { memory[scrnaddr] = 0xC0; setstatus("Turn on block mode locally ($C0)" ); }
                                     else           memory[scrnaddr] = 0x11;
        acase SCAN_N1:                              memory[scrnaddr] = 0x11;
        acase SCAN_A2:               if (shift()) { memory[scrnaddr] = 0x40; setstatus("Turn off block mode locally ($40)"); }
                                     else           memory[scrnaddr] = 0x12;
        acase SCAN_N2:               memory[scrnaddr] =                  0x12;
        acase SCAN_A3:               memory[scrnaddr] = shift() ? 0x03 : 0x13;
        acase SCAN_N3:               memory[scrnaddr] =                  0x13;
        acase SCAN_A4:               memory[scrnaddr] = shift() ? 0x37 : 0x14;
        acase SCAN_N4:               memory[scrnaddr] =                  0x14;
        acase SCAN_A5:               if (shift()) { memory[scrnaddr] = 0x80; setstatus("Useful in board mode ($80)"       ); }
                                     else           memory[scrnaddr] = 0x15;
        acase SCAN_N5:                              memory[scrnaddr] = 0x15;
        acase SCAN_A6:               if (shift())
                                     {   t = (memory[A_BGCOLOUR] & 7) + 1;
                                         memory[A_BGCOLOUR] &= 0xF8;
                                         memory[A_BGCOLOUR] |= (t & 7);
                                         setstatus("Change (outer) background colour");
                                         return TRUE;
                                     } else
                                     {   memory[scrnaddr] = 0x16;
                                     }
        acase SCAN_N6:               memory[scrnaddr] = 0x16;
        acase SCAN_A7:               if (shift())
                                     {   if (memory[A_GFXMODE] & 0x80)
                                         {   memory[A_GFXMODE] &= 0x7F;
                                         } else
                                         {   memory[A_GFXMODE] |= 0x80;
                                         }
                                         setstatus("Toggle block mode globally");
                                         return TRUE;
                                     } else
                                     {   memory[scrnaddr] = 0x17;
                                     }
        acase SCAN_N7:               memory[scrnaddr] = 0x17;
        acase SCAN_A8:               if (shift())
                                     {   if (memory[A_PITCH] & 0x80)
                                         {   memory[A_PITCH] &= 0x7F;
                                         } else
                                         {   memory[A_PITCH] |= 0x80;
                                         }
                                         setstatus("Toggle board mode globally");
                                         return TRUE;
                                     } else
                                     {   memory[scrnaddr] = 0x18;
                                     }
        acase SCAN_N8:               memory[scrnaddr] = 0x18;
        acase SCAN_A9:               if (shift())
                                     {   t = ((memory[A_GFXMODE] & 0x38) >> 3) + 1;
                                         t = ((t & 7) << 3);
                                         memory[A_GFXMODE] &= 0xC7;
                                         memory[A_GFXMODE] |= t;
                                         setstatus("Change 1st foreground colour (board mode)");
                                         return TRUE;
                                     } else
                                     {   memory[scrnaddr] = 0x19;
                                     }
        acase SCAN_N9:               memory[scrnaddr] = 0x19;
        acase SCAN_A0:               if (shift())
                                     {   t =  (memory[A_GFXMODE] & 0x07)       + 1;
                                         t = (t & 7);
                                         memory[A_GFXMODE] &= 0xF8;
                                         memory[A_GFXMODE] |= t;
                                         setstatus("Change inner background colour (board mode)");
                                         return TRUE;
                                     } else
                                     {   memory[scrnaddr] = 0x10;
                                     }
        acase SCAN_N0:               memory[scrnaddr] = 0x10;
        acase SCAN_A:                memory[scrnaddr] = shift() ? 0x0A : 0x1A; // bottom left  corner
        acase SCAN_B:                memory[scrnaddr] = shift() ? 0x3C : 0x1B; // UDG    #0
        acase SCAN_C:                memory[scrnaddr] = shift() ? 0x3A : 0x1C; // sprite #2
        acase SCAN_D:                memory[scrnaddr] = 0x1D;
        acase SCAN_E:                memory[scrnaddr] = 0x1E;
        acase SCAN_F:                memory[scrnaddr] = shift() ? 0x0F : 0x1F; // bottom left  diamond
        acase SCAN_G:                memory[scrnaddr] = shift() ? 0x0E : 0x20; // bottom right diamond
        acase SCAN_H:                memory[scrnaddr] = shift() ? 0x3F : 0x21; // UDG    #3
        acase SCAN_I:                memory[scrnaddr] = shift() ? 0x04 : 0x22; // top    line (overscore)
        acase SCAN_J:                memory[scrnaddr] = shift() ? 0x07 : 0x23; // left   line
        acase SCAN_K:                memory[scrnaddr] = shift() ? 0x06 : 0x24; // bottom line (underscore)
        acase SCAN_L:                memory[scrnaddr] = shift() ? 0x05 : 0x25; // right  line
        acase SCAN_M:                memory[scrnaddr] = shift() ? 0x3E : 0x26; // UDG    #2
        acase SCAN_N:                memory[scrnaddr] = shift() ? 0x3D : 0x27; // UDG    #1
        acase SCAN_O:                memory[scrnaddr] = 0x28;
        acase SCAN_P:                memory[scrnaddr] = 0x29;
        acase SCAN_Q:                memory[scrnaddr] = shift() ? 0x09 : 0x2A; // top    left  corner
        acase SCAN_R:                memory[scrnaddr] = shift() ? 0x0C : 0x2B; // top    left  diamond
        acase SCAN_S:                memory[scrnaddr] = shift() ? 0x0B : 0x2C; // bottom right corner
        acase SCAN_T:                memory[scrnaddr] = shift() ? 0x0D : 0x2D; // top    right diamond
        acase SCAN_U:                memory[scrnaddr] = 0x2E;
        acase SCAN_V:                memory[scrnaddr] = shift() ? 0x3B : 0x2F; // sprite #3
        acase SCAN_W:                memory[scrnaddr] = shift() ? 0x08 : 0x30; // top    right corner
        acase SCAN_X:                memory[scrnaddr] = shift() ? 0x39 : 0x31; // sprite #1
        acase SCAN_Y:                memory[scrnaddr] = 0x32;
        acase SCAN_Z:                memory[scrnaddr] = shift() ? 0x38 : 0x33; // sprite #0
        acase SCAN_FULLSTOP:         if (shift()) return FALSE; else memory[scrnaddr] = 0x34;
        acase SCAN_ND:               memory[scrnaddr] = 0x34;
        acase SCAN_COMMA:            memory[scrnaddr] = shift() ? 0x3F : 0x35; // UDG    #3
        acase SCAN_EQUALS:           if (shift()) memory[scrnaddr] = 0x36; else return FALSE;
        acase SCAN_PL:               memory[scrnaddr] = 0x36;
        adefault:                    return FALSE;
        }
        if (memory[scrnaddr] >= 0x01 && memory[scrnaddr] <= 0x3F)
        {   memory[scrnaddr] += editcolour;
        }
        if (scrnaddr == 0x18CF) scrnaddr = 0x1A00; elif (scrnaddr == 0x1ACF) scrnaddr = 0x1800; else scrnaddr++;
    }

    return TRUE; // return code is whether we ate the keystroke
}

EXPORT void arcadia_stringchar(UBYTE data, int address)
{   FAST TEXT digit[8];

    // assert(machine == ARCADIA);

    digit[0] = '0' + ((data & 0x80) >> 7); // not memory[address], it may not have changed yet!
    digit[1] = '0' + ((data & 0x40) >> 6);
    digit[2] = '0' + ((data & 0x20) >> 5);
    digit[3] = '0' + ((data & 0x10) >> 4);
    digit[4] = '0' + ((data & 0x08) >> 3);
    digit[5] = '0' + ((data & 0x04) >> 2);
    digit[6] = '0' + ((data & 0x02) >> 1);
    digit[7] = '0' +  (data & 0x01)      ;

    switch (address)
    {
    case  A_BGCOLLIDE:
    case  A_P1LEFTKEYS:
    case  A_P1MIDDLEKEYS:
    case  A_P1RIGHTKEYS:
    case  A_P1PALLADIUM:
    case  A_P2LEFTKEYS:
    case  A_P2MIDDLEKEYS:
    case  A_P2RIGHTKEYS:
    case  A_P2PALLADIUM:   sprintf(stringchar, " [%%----,%c,%c,%c,%c]"    ,                                         digit[4], digit[5], digit[6], digit[7]);
    acase A_BGCOLOUR:
    case  A_GFXMODE:
    case  A_SPRITES01CTRL:
    case  A_SPRITES23CTRL: sprintf(stringchar, " [%%%c,%c,%c%c%c,%c%c%c]" , digit[0], digit[1], digit[2], digit[3], digit[4], digit[5], digit[6], digit[7]);
    acase A_CHARLINE:      sprintf(stringchar, " [%%----,%c%c%c%c]"       ,                                         digit[4], digit[5], digit[6], digit[7]);
    acase A_CONSOLE:       sprintf(stringchar, " [%%-----,%c,%c,%c]"      ,                                                   digit[5], digit[6], digit[7]);
    acase A_PITCH:         sprintf(stringchar, " [%%%c,%c%c%c%c%c%c%c]"   , digit[0], digit[1], digit[2], digit[3], digit[4], digit[5], digit[6], digit[7]);
    acase A_SPRITECOLLIDE: sprintf(stringchar, " [%%--,%c,%c,%c,%c,%c,%c]",                     digit[2], digit[3], digit[4], digit[5], digit[6], digit[7]);
    acase A_VOLUME:        sprintf(stringchar, " [%%%c%c%c,%c,%c,%c%c%c]" , digit[0], digit[1], digit[2], digit[3], digit[4], digit[5], digit[6], digit[7]);
    acase A_P1PADDLE:
    acase A_P2PADDLE:
    acase A_SPRITE0X:
    acase A_SPRITE0Y:
    acase A_SPRITE1X:
    acase A_SPRITE1Y:
    acase A_SPRITE2X:
    acase A_SPRITE2Y:
    acase A_SPRITE3X:
    acase A_SPRITE3Y:
    acase A_VSCROLL:       sprintf(stringchar, " [%d]"                    , data);
    adefault:              sprintf(stringchar, " ['%c']"                  , guestchar(data));
}   }

EXPORT void arcadia_reset(void)
{   int i;

    spriteflip   =
    udcflip      = 0;
    fractionator = 0;
    /* This clearing helps avoid these problems:
       (a) Resetting from Dr. Slump game screen to title screen turns
           on constant rumbling (due to sprite imagery being retained
           in RAM).
       (b) For netplay, both machines should be in a consistent state. */
    for (i = 0x1800; i <= 0x1FFF; i++)
    {   memory[i] = 0;
    }
    for (i = 0x1909; i <= 0x190F; i++)
    {   memory[i] = 0xC0; // some of them (randomly) are $40 though
    }
    for (i = 0x19C0; i <= 0x19F7; i++)
    {   memory[i] = 0xFF;
}   }

MODULE void arcadia_oneraster(void)
{   FAST UBYTE imagedata,
               thechar;
    FAST int   bgc2,
               x;

    breakrastline();
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
            checkstep();
            one_instruction();
            slice_2650 -= (cycles_2650 - oldcycles);
    }   }
    else
    {   // cycle counter will not overflow, so we can use a faster method
        oldcycles = cycles_2650;
        while (cycles_2650 < endcycle)
        {   checkstep();
            one_instruction();
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
        checkstep();
        one_instruction();
        nextinst += (cycles_2650 - oldcycles) * 4; // in pixels
        cpux = nextinst;
    }
    if (nextinst >= 227)
    {   nextinst -= cpl;
}   }

EXPORT void arcadia_update_miniglow(void)
{   if (machine != ARCADIA || !SubWindowPtr[SUBWINDOW_CONTROLS])
    {   return;
    }

    if (arcadia_viewcontrolsas == 0) // Emerson
    {   if (arcadia_bigctrls)
        {   drawctrlglow(430, 156, (FLAG) TRUE);
        } else
        {   drawctrlglow(217,  79, (FLAG) TRUE);
    }   }
    elif (arcadia_viewcontrolsas == 1) // MPT-03
    {   if (arcadia_bigctrls)
        {   drawctrlglow(271, 100, (FLAG) TRUE);
        } else
        {   drawctrlglow(135,  50, (FLAG) TRUE);
}   }   }
