/* pvi.c: Signetics 2636 PVI emulation module (host-independent)

INCLUDES--------------------------------------------------------------- */

#include "da.h"

#include <string.h> // for memset()

// DEFINES----------------------------------------------------------------

#define ACTIVEX 178 // (PVI_XSIZE - USG_XMARGIN)
#define ACTIVEY 269 // (PVI_YSIZE - USG_YMARGIN)

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       FLAG                 multisprite[4];
EXPORT       int                  darkenbg          =   TRUE,
                                  multiframe,
                                  prevsprnum[4];
EXPORT const UWORD                pvi_spritedata[4] = { PVI_SPRITE0,
                                                        PVI_SPRITE1,
                                                        PVI_SPRITE2,
                                                        PVI_SPRITE3 };

/* bits are:
  15..12: sprites #11..#8 respectively
  11.. 8: sprites  #7..#4 respectively
   7.. 4: sprites  #3..#0 respectively
       3: grid/playfield
   2.. 0: sprite foreground colour (wire OR'ed) */

EXPORT const int gridline[220] =                                                    // Interton/Elektor screen is vertically as follows (in PVI coords):
{   40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, //   0.. 19 top (visible) margin
     0,  0,                                                                         //  20.. 21 thin
             2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, //  22.. 39 thick
     4,  4,                                                                         //  40.. 41 thin
             6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6, //  42.. 59 thick
     8,  8,                                                                         //  60.. 61 thin
            10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, //  62.. 79 thick
    12, 12,                                                                         //  80.. 81 thin
            14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, //  82.. 99 thick
    16, 16,                                                                         // 100..101 thin
            18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, // 102..119 thick
    20, 20,                                                                         // 120..121 thin
            22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, // 122..139 thick
    24, 24,                                                                         // 140..141 thin
            26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, // 142..159 thick
    28, 28,                                                                         // 160..161 thin
            30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, // 162..179 thick
    32, 32,                                                                         // 180..181 thin
            34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, // 182..199 thick
    36, 36,                                                                         // 200..201 thin
            38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38  // 202..219 thick
};
MODULE const int gridline2[220] =                                                   // Interton/Elektor screen is vertically as follows (in PVI coords):
{    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //   0.. 19 top (visible) margin
     1,  1,                                                                         //  20.. 21 thin
             2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  4,  4,  4,  4,  4,  4, //  22.. 39 thick
     8,  8,                                                                         //  40.. 41 thin
            16, 16, 16, 16, 16, 16, 16, 16, 16, 32, 32, 32, 32, 32, 32, 32, 32, 32, //  42.. 59 thick
     1,  1,                                                                         //  60.. 61 thin
             2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  4,  4,  4,  4,  4,  4, //  62.. 79 thick
     8,  8,                                                                         //  80.. 81 thin
            16, 16, 16, 16, 16, 16, 16, 16, 16, 32, 32, 32, 32, 32, 32, 32, 32, 32, //  82.. 99 thick
     1,  1,                                                                         // 100..101 thin
             2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  4,  4,  4,  4,  4,  4, // 102..119 thick
     8,  8,                                                                         // 120..121 thin
            16, 16, 16, 16, 16, 16, 16, 16, 16, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 122..139 thick
     1,  1,                                                                         // 140..141 thin
             2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  4,  4,  4,  4,  4,  4, // 142..159 thick
     8,  8,                                                                         // 160..161 thin
            16, 16, 16, 16, 16, 16, 16, 16, 16, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 162..179 thick
     1,  1,                                                                         // 180..181 thin
             2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  4,  4,  4,  4,  4,  4, // 182..199 thick
     8,  8,                                                                         // 200..201 thin
            16, 16, 16, 16, 16, 16, 16, 16, 16, 32, 32, 32, 32, 32, 32, 32, 32, 32, // 202..219 thick
};

MODULE const UBYTE bitfor[160] =
{    0,   0,   0,   0,   0,   0,   0,   0, //   0..  7 left margin
     0,   0,   0,   0,   0,   0,   0,   0, //   8.. 15 left margin
     0,   0,   0,   0,   0,   0,   0,   0, //  16.. 23 left margin
     0,   0,   0,   0,   0,   0,   0,   0, //  24.. 31 left margin
   128, 128, 128, 128, 128, 128, 128, 128, //  32.. 39  1st column
    64,  64,  64,  64,  64,  64,  64,  64, //  40.. 47  2nd column
    32,  32,  32,  32,  32,  32,  32,  32, //  48.. 55  3rd column
    16,  16,  16,  16,  16,  16,  16,  16, //  56.. 63  4th column
     8,   8,   8,   8,   8,   8,   8,   8, //  64.. 71  5th column
     4,   4,   4,   4,   4,   4,   4,   4, //  72.. 79  6th column
     2,   2,   2,   2,   2,   2,   2,   2, //  80.. 87  7th column
     1,   1,   1,   1,   1,   1,   1,   1, //  88.. 95  8th column
   128, 128, 128, 128, 128, 128, 128, 128, //  96..103  9th column
    64,  64,  64,  64,  64,  64,  64,  64, // 104..111 10th column
    32,  32,  32,  32,  32,  32,  32,  32, // 112..119 11th column
    16,  16,  16,  16,  16,  16,  16,  16, // 120..127 12th column
     8,   8,   8,   8,   8,   8,   8,   8, // 128..135 13th column
     4,   4,   4,   4,   4,   4,   4,   4, // 136..143 14th column
     2,   2,   2,   2,   2,   2,   2,   2, // 144..151 15th column
     1,   1,   1,   1,   1,   1,   1,   1, // 152..159 16th column
}, multicolour_superspace[6][7] = {
{ 0    , 0     , 0      , PURPLE, 0      , 0     , 0    },
{ 0    , 0     , BLUE   , CYAN  , BLUE   , 0     , 0    },
{ 0    , ORANGE, GREY1  , YELLOW, GREY1  , PURPLE, 0    },
{ GREEN, GREEN , DARKRED, YELLOW, DARKRED, CYAN  , CYAN },
{ 0    , GREEN , ORANGE , GREY1 , ORANGE , YELLOW, 0    },
{ 0    , 0     , YELLOW , GREEN , GREEN  , 0     , 0    },
};

// IMPORTED VARIABLES-------------------------------------------------- */

IMPORT       FLAG                  protect[4];
IMPORT       ASCREEN               screen[BOXWIDTH][BOXHEIGHT];
IMPORT       int                   absxmin, absxmax,
                                   absymin, absymax,
                                   ax[2],
                                   ay[4],
                                   console_start,
                                   console_a,
                                   console_b,
                                   console_reset,
                                   cpl,
                                   cpux, cpuy,
                                   dejitter,
                                   drawcorners,
                                   elektor_biosver,
                                   interrupt_2650,
                                   key1,
                                   key2,
                                   key3,
                                   key4,
                                   log_interrupts,
                                   n1, n2, n3, n4,
                                   nextinst,
                                   p1bgcol[6],
                                   p2bgcol[6],
                                   p1sprcol[6],
                                   p2sprcol[6],
                                   paddleup,
                                   paddledown,
                                   paddleleft,
                                   paddleright,
                                   ppc,
                                   pvibase,
                                   machine,
                                   memmap,
                                   p1rumble,
                                   p2rumble,
                                   rumbling[2],
                                   spriteflip,
                                   spriteflips,
                                   supercpu,
                                   udcflips,
                                   undither,
                                   whichgame;
IMPORT       UBYTE                 aw_dips1,
                                   ga_dips,
                                   hx[2], hy[2],
                                   memflags[32768],
                                   memory[32768],
                                   opcode,
                                   psu;
IMPORT       UWORD                 console[4],
                                   iar,
                                   keypads[2][NUMKEYS],
                                   mirror_r[32768],
                                   mirror_w[32768];
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
IMPORT       float                 idealfreq_ntsc[256],
                                   idealfreq_pal[256];
IMPORT       struct MachineStruct  machines[MACHINES];
IMPORT const UBYTE                 table_opcolours_2650[2][256];
IMPORT const struct KnownStruct    known[KNOWNGAMES];
IMPORT       UBYTE                 KeyMatrix[16];

// MODULE VARIABLES-------------------------------------------------------

MODULE int    bgc,
              digity,
              pvix, pviy;
MODULE UBYTE  bgcread,
              colltable[ACTIVEY][ACTIVEX],
              digitcolour,
              pixelcolour,
              sizeshift0, sizeshift1, sizeshift2, sizeshift3,
              t;
MODULE FLAG   prevspritedone[4][PREVSPRITES];

MODULE struct
{   int   majory,
          minory,
          newstarty,
          startx,
          starty,
          endx;
    FLAG  active,
          dup,
          ending;
    UBYTE colour,
          imagery,
          size;
} newsprite[4];

MODULE struct
{   int   leftx,
          topy,
          size,
          colour;
    FLAG  inuse;
    UBYTE imagery[10];
} prevsprite[21 + 1][4][PREVSPRITES];

/*   -0-  -1-  -2-  -3-  -4-  -5-  -6-  -7-  -8-  -9-  -A-  -B-  -C-  -D-  -E-  -F-
abc  ###  ..#  ###  ###  #.#  ###  ###  ###  ###  ###  ###  #..  ###  ..#  ###  ###
l d  # #  . #  ..#  . #  # #  # .  # .  . #  # #  # #  # #  # .  # .  . #  # .  # .
kme  #.#  ..#  ###  ###  ###  ###  ###  ..#  ###  ###  ###  ###  #..  ###  ###  ###
j f  # #  . #  #..  . #  . #  . #  # #  . #  # #  . #  # #  # #  # .  # #  # .  # .
ihg  ###  ..#  ###  ###  ..#  ###  ###  ..#  ###  ###  #.#  ###  ###  ###  ###  #..

The Interton/Elektor handles $A..$F values by drawing nothing.
The capital $B looks the same as 8
and capital $D looks the same as 0
so we would use lowercase "b" and "d". */

PERSIST const UBYTE ie_digits[16][20][16] = {
{ // 0
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
}, { // 1
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
}, { // 2
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,0,0,0,0 },
{ 1,1,1,1,0,0,0,0,0,0,0,0 },
{ 1,1,1,1,0,0,0,0,0,0,0,0 },
{ 1,1,1,1,0,0,0,0,0,0,0,0 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
}, { // 3
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
}, { // 4
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
}, { // 5
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,0,0,0,0 },
{ 1,1,1,1,0,0,0,0,0,0,0,0 },
{ 1,1,1,1,0,0,0,0,0,0,0,0 },
{ 1,1,1,1,0,0,0,0,0,0,0,0 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
}, { // 6
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,0,0,0,0 },
{ 1,1,1,1,0,0,0,0,0,0,0,0 },
{ 1,1,1,1,0,0,0,0,0,0,0,0 },
{ 1,1,1,1,0,0,0,0,0,0,0,0 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
}, { // 7
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
}, { // 8
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
}, { // 9
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 0,0,0,0,0,0,0,0,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
{ 1,1,1,1,1,1,1,1,1,1,1,1 },
}, { // $A
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
}, { // $B
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
}, { // $C
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
}, { // $D
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
}, { // $E
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
}, { // $F
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0,0,0,0,0 },
} };

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void ie_emuinput(void);
MODULE void newspritedone(int whichsprite);
MODULE __inline void onepixel(void);
MODULE void unvblank(void);
MODULE void drawmultiplexedsprites(int whichframe);
MODULE FLAG overlaps(int leftx1, int leftx2, int topy1, int topy2, int xsize, int ysize);
MODULE void readsprites(void);
MODULE void newraster(void);
MODULE UBYTE pviread(int address);
MODULE __inline void do_cpu(void);
MODULE void setpaddle(int player, int which);
MODULE void run_cpu(int until);

// 8. CODE----------------------------------------------------------------

EXPORT void newpvi(void)
{   TRANSIENT int i, j,
                  whichsprite;

    // "The data is valid only at vertical reset." - 2636 PVI datasheet, p. 10.
    // But enabling these lines breaks Interton BOXING :-(
    // pviwrite(PVI_P1PADDLE, 0, TRUE);
    // pviwrite(PVI_P2PADDLE, 0, TRUE);

    multiframe = spriteflip % (spriteflips + 1);
    for (i = 0; i < 4; i++)
    {   prevsprnum[i] = 0;
        for (j = 0; j < PREVSPRITES; j++)
        {   prevsprite[multiframe][i][j].inuse = FALSE;
    }   }
    for (i = 0; i < ACTIVEX * ACTIVEY; i++)
    {   *(&colltable[0][0] + i) = 0;
    }

    cpuy = 0;
    for (cpux = 0; cpux < n4; cpux++)
    {   bgcread = memory[0x1F00 + PVI_BGCOLOUR];
        if (memmap != MEMMAP_E && (memory[IE_NOISE] & 0x20)) // invert
        {   if (bgcread & 8) // background/grid enable
            {   changethisbgpixel(bgcread & 7);
            } else
            {   changethisbgpixel(WHITE);
        }   }
        else
        {   if (bgcread & 8) // background/grid enable
            {   if (darkenbg)
                {   changethisbgpixel(15 - (bgcread & 7));
                } else
                {   changethisbgpixel( 7 - (bgcread & 7));
            }   }
            else
            {   changethisbgpixel(BLACK);
        }   }
        do_cpu();
    }
    pvi_vblank(); // vblank interrupts and sprite complete interrupts can't occur simultaneously, at least on the emulator
    run_cpu(227); // the rest of raster 0

    for (cpuy = 1; cpuy < n1; cpuy++)
    {   run_cpu(227);
    }

    for (cpuy = n1; cpuy < n3; cpuy++)
    {   if (cpuy == n1)
        {   run_cpu(n4);
            unvblank();
        } else
        {   for (cpux = 0; cpux < n4; cpux++)
            {   bgcread = memory[0x1F00 + PVI_BGCOLOUR];
                if (memmap != MEMMAP_E && (memory[IE_NOISE] & 0x20)) // invert
                {   if (bgcread & 8) // background/grid enable
                    {   changethisbgpixel(bgcread & 7);
                    } else
                    {   changethisbgpixel(WHITE);
                }   }
                else
                {   if (bgcread & 8) // background/grid enable
                    {   if (darkenbg)
                        {   changethisbgpixel(15 - (bgcread & 7));
                        } else
                        {   changethisbgpixel( 7 - (bgcread & 7));
                    }   }
                    else
                    {   changethisbgpixel(BLACK);
                }   }
                do_cpu();
            }
            if (cpuy == n2) // ie. USG_YMARGIN + (269 / 2). "Conversion takes place during the active scan".
            {   ie_emuinput();
        }   }
        newraster();
        run_cpu(12);
        readsprites();    // this timing might not
        checkinterrupt(); // be right for NTSC?
        run_cpu(49);
        for (cpux = 49; cpux < 227; cpux++)
        {   onepixel();
        }

        for (whichsprite = 0; whichsprite < 4; whichsprite++)
        {   if (newsprite[whichsprite].ending)
            {   newsprite[whichsprite].ending =
                newsprite[whichsprite].active = FALSE;
                newsprite[whichsprite].starty = newsprite[whichsprite].newstarty;
                newsprite[whichsprite].dup    = TRUE;
                if (prevsprite[multiframe][whichsprite][prevsprnum[whichsprite]].inuse)
                {   prevsprnum[whichsprite]++;
    }   }   }   }

    if (demultiplex && frames >= (ULONG) spriteflips)
    {   for (i = 0; i < 4; i++)
        {   for (j = 0; j < PREVSPRITES; j++)
            {   prevspritedone[i][j] = FALSE;
        }   }

        for (i = spriteflips; i >= 1; i--)
        {   drawmultiplexedsprites((spriteflip + i) % (spriteflips + 1));
    }   }
    if (spriteflip == spriteflips)
    {   spriteflip = 0;
    } else
    {   spriteflip++;
}   }

MODULE __inline void onepixel(void)
{   FAST       int   w,
                     whichset;
    FAST       UBYTE horizn;
    FAST const UBYTE horizn_table[4] = {1, 2, 1, 4};

    // Background-----------------------------------------------------

    if (!dejitter)
    {   bgcread = memory[0x1F00 + PVI_BGCOLOUR];
        if (memmap != MEMMAP_E && (memory[IE_NOISE] & 0x20)) // invert
        {   if (bgcread & 8) // background/grid enable
            {   bgc = bgcread & 7;
            } else
            {   bgc = 0; // white
        }   }
        else
        {   if (bgcread & 8) // background/grid enable
            {   if (darkenbg)
                {   bgc = 15 - (bgcread & 7);
                } else
                {   bgc =  7 - (bgcread & 7);
            }   }
            else
            {   bgc = 7; // black
    }   }   }
    pixelcolour = bgc;
    pvix = cpux - USG_XMARGIN;

    if (bgcread & 8)
    {   // Grid-------------------------------------------------------

        if (pviy >= 20 && pviy <= 219 && pvix >= 32 && pvix < 32 + 128)
        {   whichset = gridline[pviy];
            if (pviread(PVI_VERTGRID + whichset + ((pvix >= 96) ? 1 : 0)) & bitfor[pvix])
            {   horizn = pviread(PVI_HORIZ1 + (whichset >> 3)); // PVI_HORIZn
                if (horizn & gridline2[pviy]) w = 8; else w = horizn_table[(horizn & 0xC0) >> 6];
                if ((pvix & 7) < w)
                {   colltable[pviy][pvix] |= 8;
                    if (memmap != MEMMAP_E && (memory[IE_NOISE] & 0x20)) // invert
                    {   pixelcolour =      ((bgcread & 0x70) >> 4);
                    } elif (darkenbg)
                    {   pixelcolour = 15 - ((bgcread & 0x70) >> 4); // 8..15
                    } else
                    {   pixelcolour =  7 - ((bgcread & 0x70) >> 4);
                    }
                    goto SCORE; // for speed
            }   }

            if (drawcorners && !(whichset & 2) && !(pvix & 7))
            {   if (whichset > 0)
                {   horizn = pviread(PVI_HORIZ1 + ((whichset - 2) >> 3));
                    if (pviread(PVI_VERTGRID + whichset - ((pvix >= 96) ? 1 : 2)) & bitfor[pvix])
                    {   if (memmap != MEMMAP_E && (memory[IE_NOISE] & 0x20)) // invert
                        {   pixelcolour =      ((bgcread & 0x70) >> 4);
                        } elif (darkenbg)
                        {   pixelcolour = 15 - ((bgcread & 0x70) >> 4); // 8..15
                        } else
                        {   pixelcolour =  7 - ((bgcread & 0x70) >> 4);
                        }
                        goto SCORE; // for speed
                }   }
                if (whichset < 38)
                {   horizn = pviread(PVI_HORIZ1 + ((whichset + 2) >> 3));
                    if (pviread(PVI_VERTGRID + whichset + ((pvix >= 96) ? 3 : 2)) & bitfor[pvix])
                    {   if (memmap != MEMMAP_E && (memory[IE_NOISE] & 0x20)) // invert
                        {   pixelcolour =      ((bgcread & 0x70) >> 4);
                        } elif (darkenbg)
                        {   pixelcolour = 15 - ((bgcread & 0x70) >> 4); // 8..15
                        } else
                        {   pixelcolour =  7 - ((bgcread & 0x70) >> 4);
                        }
                        // goto SCORE; // for speed
    }   }   }   }   }

SCORE:
    // Score--------------------------------------------------

    if (pviy >= digity && pviy <= digity + 19)
    {   if           (pvix >=  60 && pvix <=  71) { if (ie_digits[pviread(PVI_SCORELT) >>   4][pviy - digity][pvix -  60]) pixelcolour = digitcolour; }
        elif         (pvix >=  76 && pvix <=  87) { if (ie_digits[pviread(PVI_SCORELT) &  0xf][pviy - digity][pvix -  76]) pixelcolour = digitcolour; }
        else
        {   if (pviread(PVI_SCORECTRL) & 2)
            {   if   (pvix >=  92 && pvix <= 103) { if (ie_digits[pviread(PVI_SCORERT) >>   4][pviy - digity][pvix -  92]) pixelcolour = digitcolour; }
                elif (pvix >= 108 && pvix <= 119) { if (ie_digits[pviread(PVI_SCORERT) &  0xf][pviy - digity][pvix - 108]) pixelcolour = digitcolour; }
            } else
            {   if   (pvix >= 108 && pvix <= 119) { if (ie_digits[pviread(PVI_SCORERT) >>   4][pviy - digity][pvix - 108]) pixelcolour = digitcolour; }
                elif (pvix >= 124 && pvix <= 135) { if (ie_digits[pviread(PVI_SCORERT) &  0xf][pviy - digity][pvix - 124]) pixelcolour = digitcolour; }
    }   }   }

    // Sprites--------------------------------------------------------

    if
    (   newsprite[0].active
     && pvix >= newsprite[0].startx
     && pvix <  newsprite[0].endx
     && (newsprite[0].imagery & (128 >> ((pvix - newsprite[0].startx) >> sizeshift0)))
    )
    {   colltable[pviy][pvix] |= 0x10 | newsprite[0].colour;
        pixelcolour = colltable[pviy][pvix] & 7;
    }
    if
    (   newsprite[1].active
     && pvix >= newsprite[1].startx
     && pvix <  newsprite[1].endx
     && (newsprite[1].imagery & (128 >> ((pvix - newsprite[1].startx) >> sizeshift1)))
    )
    {   colltable[pviy][pvix] |= 0x20 | newsprite[1].colour;
        pixelcolour = colltable[pviy][pvix] & 7;
    }
    if
    (   newsprite[2].active
     && pvix >= newsprite[2].startx
     && pvix <  newsprite[2].endx
     && (newsprite[2].imagery & (128 >> ((pvix - newsprite[2].startx) >> sizeshift2)))
    )
    {   colltable[pviy][pvix] |= 0x40 | newsprite[2].colour;
        pixelcolour = colltable[pviy][pvix] & 7;
    }
    if
    (   newsprite[3].active
     && pvix >= newsprite[3].startx
     && pvix <  newsprite[3].endx
     && (newsprite[3].imagery & (128 >> ((pvix - newsprite[3].startx) >> sizeshift3)))
    )
    {   colltable[pviy][pvix] |= 0x80 | newsprite[3].colour;
        if (whichgame == SUPERSPACEPOS && undither)
        {   pixelcolour = multicolour_superspace[pviy - newsprite[3].starty][pvix - newsprite[3].startx];
        } else
        {   pixelcolour = colltable[pviy][pvix] & 7;
    }   }

    // Collisions-------------------------------------------------

    if ((colltable[pviy][pvix] & 0xF0) && collisions)
    {   t = 0;
        // colltable[][] is 3210xxxx
        if ((colltable[pviy][pvix] & 0x30) == 0x30) { t |=  32; if (p1sprcol[0]) p1rumble = p1sprcol[0]; if (p2sprcol[0]) p2rumble = p2sprcol[0]; }
        if ((colltable[pviy][pvix] & 0x50) == 0x50) { t |=  16; if (p1sprcol[1]) p1rumble = p1sprcol[1]; if (p2sprcol[1]) p2rumble = p2sprcol[1]; }
        if ((colltable[pviy][pvix] & 0x90) == 0x90) { t |=   8; if (p1sprcol[2]) p1rumble = p1sprcol[2]; if (p2sprcol[2]) p2rumble = p2sprcol[2]; }
        if ((colltable[pviy][pvix] & 0x60) == 0x60) { t |=   4; if (p1sprcol[3]) p1rumble = p1sprcol[3]; if (p2sprcol[3]) p2rumble = p2sprcol[3]; }
        if ((colltable[pviy][pvix] & 0xA0) == 0xA0) { t |=   2; if (p1sprcol[4]) p1rumble = p1sprcol[4]; if (p2sprcol[4]) p2rumble = p2sprcol[4]; }
        if ((colltable[pviy][pvix] & 0xC0) == 0xC0) { t |=   1; if (p1sprcol[5]) p1rumble = p1sprcol[5]; if (p2sprcol[5]) p2rumble = p2sprcol[5]; }
        if (t)
        {   pviwrite(PVI_SPRITECOLLIDE   , (UBYTE) (pviread(PVI_SPRITECOLLIDE   ) | t), TRUE);
            pviwrite(PVI_SPRITECOLLIDE_M2, (UBYTE) (pviread(PVI_SPRITECOLLIDE_M2) | t), TRUE);
            pviwrite(PVI_SPRITECOLLIDE_M3, (UBYTE) (pviread(PVI_SPRITECOLLIDE_M3) | t), TRUE);
            pviwrite(PVI_SPRITECOLLIDE_M4, (UBYTE) (pviread(PVI_SPRITECOLLIDE_M4) | t), TRUE);
        }

        t = 0;
        if ((colltable[pviy][pvix] & 0x18) == 0x18) { t |= 128; if (p1bgcol[0] > 0) p1rumble = p1bgcol[ 0]; if (p2bgcol[0] > 0) p2rumble = p2bgcol[ 0]; }
        if ((colltable[pviy][pvix] & 0x28) == 0x28) { t |=  64; if (p1bgcol[1] > 0) p1rumble = p1bgcol[ 1]; if (p2bgcol[1] > 0) p2rumble = p2bgcol[ 1]; }
        if ((colltable[pviy][pvix] & 0x48) == 0x48) { t |=  32; if (p1bgcol[2] > 0) p1rumble = p1bgcol[ 2]; if (p2bgcol[2] > 0) p2rumble = p2bgcol[ 2]; }
        if ((colltable[pviy][pvix] & 0x88) == 0x88) { t |=  16; if (p1bgcol[3] > 0) p1rumble = p1bgcol[ 3]; if (p2bgcol[3] > 0) p2rumble = p2bgcol[ 3]; }
        if (t)
        {   pviwrite(PVI_BGCOLLIDE   ,     (UBYTE) (pviread(PVI_BGCOLLIDE       ) | t), TRUE);
            pviwrite(PVI_BGCOLLIDE_M2,     (UBYTE) (pviread(PVI_BGCOLLIDE_M2    ) | t), TRUE);
            pviwrite(PVI_BGCOLLIDE_M3,     (UBYTE) (pviread(PVI_BGCOLLIDE_M3    ) | t), TRUE);
            pviwrite(PVI_BGCOLLIDE_M4,     (UBYTE) (pviread(PVI_BGCOLLIDE_M4    ) | t), TRUE);
    }   }

    // Render-----------------------------------------------------

    if (pixelcolour == bgc)
    {   changethisbgpixel(pixelcolour);
    } else
    {   changethisabspixel(pixelcolour);
    }

    // CPU------------------------------------------------------------

    do_cpu();
}

EXPORT void pviwrite(signed int address, UBYTE data, FLAG ispvi)
{   /* Writes to memory from the PVI always go through this
       routine (so that watchpoints work properly). */

    if (ispvi)
    {   address += pvibase;
    }
    memory[address] = data;
}

MODULE UBYTE pviread(int address)
{   return memory[pvibase + address];
}

EXPORT void ie_inputmirrors(int address)
{   // write mirrors
    if (    mirror_w[address] >= 0x1E81 && mirror_w[address] <= 0x1EFF)
    {   mirror_w[address] = 0x1E80;
    }
    if (             address  == 0x1E80) // only applies to real $1E80, not mirrors thereof!
    {   memflags[address] |= NOREAD;
    }

    // read mirrors
    if (    mirror_r[address] >= 0x1E81 && mirror_r[address] <= 0x1E97)
    {   memflags[address] |= NOWRITE;
    } elif (mirror_r[address] >= 0x1E98 && mirror_r[address] <= 0x1E9B)
    {   mirror_r[address] -= 0x10;
    } elif (mirror_r[address] >= 0x1E9C && mirror_r[address] <= 0x1EA7)
    {   memflags[address] |= NOWRITE;
    } elif (mirror_r[address] >= 0x1EA8 && mirror_r[address] <= 0x1EAE)
    {   mirror_r[address] -= 0x20;
    } elif (mirror_r[address] >= 0x1EAF && mirror_r[address] <= 0x1EB7)
    {   memflags[address] |= NOWRITE;
    } elif (mirror_r[address] >= 0x1EB8 && mirror_r[address] <= 0x1EBB)
    {   mirror_r[address] -= 0x30;
    } elif (mirror_r[address] >= 0x1EBC && mirror_r[address] <= 0x1EC7)
    {   memflags[address] |= NOWRITE;
    } elif (mirror_r[address] >= 0x1EC8 && mirror_r[address] <= 0x1ECE)
    {   mirror_r[address] -= 0x40;
    } elif (mirror_r[address] >= 0x1ECF && mirror_r[address] <= 0x1ED7)
    {   memflags[address] |= NOWRITE;
    } elif (mirror_r[address] >= 0x1ED8 && mirror_r[address] <= 0x1EDB)
    {   mirror_r[address] -= 0x50;
    } elif (mirror_r[address] >= 0x1EDC && mirror_r[address] <= 0x1EE7)
    {   memflags[address] |= NOWRITE;
    } elif (mirror_r[address] >= 0x1EE8 && mirror_r[address] <= 0x1EEE)
    {   mirror_r[address] -= 0x60;
    } elif (mirror_r[address] >= 0x1EEF && mirror_r[address] <= 0x1EF7)
    {   memflags[address] |= NOWRITE;
    } elif (mirror_r[address] >= 0x1EF8 && mirror_r[address] <= 0x1EFB)
    {   mirror_r[address] -= 0x70;
    } elif (mirror_r[address] >= 0x1EFC && mirror_r[address] <= 0x1EFF)
    {   memflags[address] |= NOWRITE;
}   }

EXPORT void pvi_memmap(void)
{   int i;

    for (i = 0; i <= 255; i++)
    {   memflags[pvibase + i] = 0;
        mirror_r[pvibase + i] =
        mirror_w[pvibase + i] = (UWORD) (pvibase + i);

        if
        (
         // legal: $00..        $2D     hardware registers, RAM, h/w regs, RAM, h/w regs
            (i >= 0x2E && i <= 0x3F) // unmapped
         // legal: $40..        $6D     hardware registers, RAM
         || (i >= 0x6E && i <= 0x7F) // unmapped
         // legal: $80..        $AD     grid, RAM
         || (i >= 0xAE && i <= 0xBF) // unmapped
         // legal: $C0..        $C3     hardware registers
         || (i >= 0xC4 && i <= 0xC5) // unused
         // legal: $C6..        $C9     hardware registers
         ||  i >= 0xCA               // R/- hardware registers, unused, mirrors (not occurring), semi-mirrors
        )
        {   memflags[pvibase + i] |= NOWRITE;
        }

        if
        (
         // legal: $00..        $2D     hardware registers, RAM, h/w regs, RAM, h/w regs
            (i >= 0x2E && i <= 0x3F) // unmapped
         // legal: $40..        $6D     hardware registers, RAM
         || (i >= 0x6E && i <= 0x7F) // unmapped
         // legal: $80..        $AD     grid, RAM
         || (i >= 0xAE && i <= 0xC9) // unmapped, -/W (write-only) and -/- (unusable) hardware registers
         // legal: $CA..        $CD     R/- (read-only) hardware registers
         || (i >= 0xCE && i <= 0xD9) // unused, mirrors (not occurring)
         // legal: $DA..        $DB     semi-mirrors
         || (i >= 0xDC && i <= 0xE9) // mirrors (not occurring)
         // legal: $EA..        $EB     semi-mirrors
         || (i >= 0xEC && i <= 0xF9) // mirrors (not occurring)
         // legal: $FA..        $FB     semi-mirrors
         ||  i >= 0xFC               // mirrors (not occurring)
        )
        {   memflags[pvibase + i] |= NOREAD;
        }

        if
        (   (i >= 0xCA && i <= 0xCB)
         || (i >= 0xDA && i <= 0xDB)
         || (i >= 0xEA && i <= 0xEB)
         || (i >= 0xFA && i <= 0xFB)
        )
        {   memflags[pvibase + i] |= READONCE; // semi-mirrors
        }

        if (i == PVI_PITCH)
        {   memflags[pvibase + i] |= AUDIBLE;
}   }   }

MODULE void newspritedone(int whichsprite)
{   // pvibase is expected to be set according to the PVI you are using

    pviwrite(PVI_BGCOLLIDE   , (UBYTE) (pviread(PVI_BGCOLLIDE   ) | (8 >> whichsprite)), TRUE);
    pviwrite(PVI_BGCOLLIDE_M2, (UBYTE) (pviread(PVI_BGCOLLIDE_M2) | (8 >> whichsprite)), TRUE);
    pviwrite(PVI_BGCOLLIDE_M3, (UBYTE) (pviread(PVI_BGCOLLIDE_M3) | (8 >> whichsprite)), TRUE);
    pviwrite(PVI_BGCOLLIDE_M4, (UBYTE) (pviread(PVI_BGCOLLIDE_M4) | (8 >> whichsprite)), TRUE);

    if (!interrupt_2650)
    {   interrupt_2650 = TRUE;
        /* Don't call checkinterrupt() because only one interrupt
        should be generated even if multiple sprites finish
        simultaneously (important for Interton Capture). */
    }

    t = pviread(pvi_spritedata[whichsprite] + 13); // SPRITEnBY
    if (t == 255)
    {   newsprite[whichsprite].newstarty = pviy + 1;
    } else
    {   newsprite[whichsprite].newstarty = pviy + 2 + t;
    }

    newsprite[whichsprite].ending = TRUE;
}

EXPORT void pvi_vblank(void)
{   pviwrite(PVI_SPRITECOLLIDE   , (UBYTE) (memory[pvibase + PVI_SPRITECOLLIDE   ] | 0x40), TRUE); // set VRST bit
    pviwrite(PVI_SPRITECOLLIDE_M2, (UBYTE) (memory[pvibase + PVI_SPRITECOLLIDE_M2] | 0x40), TRUE); // set VRST bit
    pviwrite(PVI_SPRITECOLLIDE_M3, (UBYTE) (memory[pvibase + PVI_SPRITECOLLIDE_M3] | 0x40), TRUE); // set VRST bit
    pviwrite(PVI_SPRITECOLLIDE_M4, (UBYTE) (memory[pvibase + PVI_SPRITECOLLIDE_M4] | 0x40), TRUE); // set VRST bit

    if (memmap == MEMMAP_ASTROWARS)
    {   psu |= PSU_S & aw_dips1;
    } elif (memmap == MEMMAP_GALAXIA)
    {   psu |= PSU_S & ga_dips;
    } else
    {   psu |= PSU_S;
        if (whichgame == QUEENPOS)
        {   // assert(machine == ELEKTOR);
            memory[0x8CA] = 0; // compatibility fix
    }   }

    interrupt_2650 = TRUE;
    checkinterrupt();
}

EXPORT int calcspritecolour(int whichsprite)
{   int localcolour = 0; // initialized to prevent a spurious SAS/C compiler warning

    // whichsprite is 0..11.
    // pvibase must be its original value (ie. must point to the 1st PVI)!
    // We deliberately don't use pviread()

    // assert(machines[machine].pvis);
    switch (whichsprite % 4)
    {
    case  0: localcolour = (memory[pvibase + (0x100 * (whichsprite / 4)) + PVI_SPR01COLOURS] & 0x38) >> 3;
    acase 1: localcolour =  memory[pvibase + (0x100 * (whichsprite / 4)) + PVI_SPR01COLOURS] & 7;
    acase 2: localcolour = (memory[pvibase + (0x100 * (whichsprite / 4)) + PVI_SPR23COLOURS] & 0x38) >> 3;
    acase 3: localcolour =  memory[pvibase + (0x100 * (whichsprite / 4)) + PVI_SPR23COLOURS] & 7;
    }

    return localcolour;
}

MODULE void unvblank(void)
{   pviwrite(PVI_SPRITECOLLIDE   , 0, TRUE); // clear VRST and sprite-sprite collision bits
    pviwrite(PVI_SPRITECOLLIDE_M2, 0, TRUE); // clear VRST and sprite-sprite collision bits
    pviwrite(PVI_SPRITECOLLIDE_M3, 0, TRUE); // clear VRST and sprite-sprite collision bits
    pviwrite(PVI_SPRITECOLLIDE_M4, 0, TRUE); // clear VRST and sprite-sprite collision bits
    pviwrite(PVI_BGCOLLIDE       , 0, TRUE); // clear sprite complete and sprite-bg collision bits
    pviwrite(PVI_BGCOLLIDE_M2    , 0, TRUE); // clear sprite complete and sprite-bg collision bits
    pviwrite(PVI_BGCOLLIDE_M3    , 0, TRUE); // clear sprite complete and sprite-bg collision bits
    pviwrite(PVI_BGCOLLIDE_M4    , 0, TRUE); // clear sprite complete and sprite-bg collision bits

    psu &= ~(PSU_S);
    interrupt_2650 = FALSE; // "Interrupts are reset on the trailing edge of the vertical reset signal." - 2636 PVI datasheet, p. 3.

    t = pviread(PVI_SPRITE0AY) + 1; /* overflow is OK */ newsprite[0].starty = t;
    t = pviread(PVI_SPRITE1AY) + 1; /* overflow is OK */ newsprite[1].starty = t;
    t = pviread(PVI_SPRITE2AY) + 1; /* overflow is OK */ newsprite[2].starty = t;
    t = pviread(PVI_SPRITE3AY) + 1; /* overflow is OK */ newsprite[3].starty = t;
    newsprite[0].dup    =
    newsprite[1].dup    =
    newsprite[2].dup    =
    newsprite[3].dup    =
    newsprite[0].active =
    newsprite[1].active =
    newsprite[2].active =
    newsprite[3].active = FALSE;
    newsprite[0].ending =
    newsprite[1].ending =
    newsprite[2].ending =
    newsprite[3].ending = FALSE;
}

MODULE void drawmultiplexedsprites(int whichframe)
{   FAST int i,
             whichsprite,
             x, xx, xxx, y, yy, yyy;

    // assert(machine == INTERTON || machine == ELEKTOR);
    // assert(whichpvi == 0);

    for (whichsprite = 0; whichsprite < 4; whichsprite++)
    {   for (i = 0; i < PREVSPRITES; i++)
        {   if
            (   (!prevspritedone[whichsprite][i] || multisprite[whichsprite])
             &&  prevsprite[whichframe][whichsprite][i].inuse
             && !protect[whichsprite]
             && (   whichgame == ASTEROIDSPOS || !overlaps
                    (        prevsprite[whichframe][whichsprite][i].leftx,
                             prevsprite[multiframe][whichsprite][i].leftx,
                             prevsprite[whichframe][whichsprite][i].topy,
                             prevsprite[multiframe][whichsprite][i].topy,
                         8 * prevsprite[whichframe][whichsprite][i].size,
                        10 * prevsprite[whichframe][whichsprite][i].size
            )   )   )
            {   prevspritedone[whichsprite][i] = TRUE;

                for (y = 0; y < 10; y++)
                {   for (yy = 0; yy < prevsprite[whichframe][whichsprite][i].size; yy++)
                    {   for (x = 0; x < 8; x++)
                        {   for (xx = 0; xx < prevsprite[whichframe][whichsprite][i].size; xx++)
                            {   if (prevsprite[whichframe][whichsprite][i].imagery[y] & (128 >> x))
                                {   xxx = prevsprite[whichframe][whichsprite][i].leftx + (x * prevsprite[whichframe][whichsprite][i].size) + xx;
                                    yyy = prevsprite[whichframe][whichsprite][i].topy  + (y * prevsprite[whichframe][whichsprite][i].size) + yy;

                                    changerelpixel
                                    (   xxx,
                                        yyy,
                                        prevsprite[whichframe][whichsprite][i].colour
                                    );
}   }   }   }   }   }   }   }   }

MODULE FLAG overlaps(int leftx1, int leftx2, int topy1, int topy2, int xsize, int ysize)
{   if
    (   leftx1 + xsize - 1 >= leftx2             // if  the rightmost  part of the 1st is right of the leftmost  part of the 2nd
     && leftx1             <= leftx2 + xsize - 1 // and the leftmost   part of the 1st is left  of the rightmost part of the 2nd
     &&  topy1 + ysize - 1 >=  topy2             // if  the bottommost part of the 1st is below    the top            of the 2nd
     &&  topy1             <=  topy2 + ysize - 1 // and the topmost    part of the 1st is above    the bottom         of the 2nd
    )
    {   return TRUE;
    } else
    {   return FALSE;
}   }

MODULE void readsprites(void)
{   FAST int whichsprite;
#ifdef DEBUGSPRITES
PERSIST const int consolecolours[8] =
{ 7, // 0 -> 7 white
  3, // 1 -> 3 yellow
  5, // 2 -> 5 purple
  1, // 3 -> 1 red
  6, // 4 -> 6 cyan
  2, // 5 -> 2 green
  4, // 6 -> 4 blue
  8, // 7 -> 8 black (shown as grey)
};
#endif

    for (whichsprite = 0; whichsprite < 4; whichsprite++)
    {   if (pviy == newsprite[whichsprite].starty && (pviy < 253 || whichgame == MOONLANDING1POS || whichgame == MOONLANDING2POS))
        {   newsprite[whichsprite].majory =  0;
            newsprite[whichsprite].minory = -1;
            newsprite[whichsprite].active = TRUE;
        }

        if (!newsprite[whichsprite].active)
        {   continue;
        }

        // These need to be refreshed every line for Leapfrog
        switch (whichsprite)
        {
        case  0: sizeshift0          =       pviread(PVI_SIZES       )       & 3;
                 newsprite[0].size   = 1 << sizeshift0;
                 newsprite[0].colour =      (pviread(PVI_SPR01COLOURS) >> 3) & 7;
        acase 1: sizeshift1          =      (pviread(PVI_SIZES       ) >> 2) & 3;
                 newsprite[1].size   = 1 << sizeshift1;
                 newsprite[1].colour =       pviread(PVI_SPR01COLOURS)       & 7;
        acase 2: sizeshift2          =      (pviread(PVI_SIZES       ) >> 4) & 3;
                 newsprite[2].size   = 1 << sizeshift2;
                 newsprite[2].colour =      (pviread(PVI_SPR23COLOURS) >> 3) & 7;
        acase 3: sizeshift3          =      (pviread(PVI_SIZES       ) >> 6) & 3;
                 newsprite[3].size   = 1 << sizeshift3;
                 newsprite[3].colour =       pviread(PVI_SPR23COLOURS)       & 7;
        }

        newsprite[whichsprite].minory++;
        if (newsprite[whichsprite].minory >= newsprite[whichsprite].size)
        {   newsprite[whichsprite].minory = 0;
            newsprite[whichsprite].majory++;
        }

        if (newsprite[whichsprite].dup)
        {   t = pviread(pvi_spritedata[whichsprite] + 11); // SPRITEnBX
        } else
        {   t = pviread(pvi_spritedata[whichsprite] + 10); // SPRITEnAX
        }
        newsprite[whichsprite].startx  = t + 1;
        newsprite[whichsprite].endx    = newsprite[whichsprite].startx + (8 * newsprite[whichsprite].size);
        newsprite[whichsprite].imagery = pviread(pvi_spritedata[whichsprite] + newsprite[whichsprite].majory);

        if (demultiplex && prevsprnum[whichsprite] < PREVSPRITES)
        {   if (pviy == newsprite[whichsprite].starty)
            {   prevsprite[multiframe][whichsprite][prevsprnum[whichsprite]].inuse  = TRUE;
                prevsprite[multiframe][whichsprite][prevsprnum[whichsprite]].leftx  = newsprite[whichsprite].startx; // for prevsprites, we don't support having changing these every line
                prevsprite[multiframe][whichsprite][prevsprnum[whichsprite]].topy   = pviy;
                prevsprite[multiframe][whichsprite][prevsprnum[whichsprite]].size   = newsprite[whichsprite].size;
                prevsprite[multiframe][whichsprite][prevsprnum[whichsprite]].colour = newsprite[whichsprite].colour;
            }

            if (prevsprite[multiframe][whichsprite][prevsprnum[whichsprite]].inuse)
            {   prevsprite[multiframe][whichsprite][prevsprnum[whichsprite]].imagery[(newsprite[whichsprite].majory * newsprite[whichsprite].size) + newsprite[whichsprite].minory]
                                                                                    = newsprite[whichsprite].imagery;
    }   }   }

    for (whichsprite = 0; whichsprite < 4; whichsprite++) // don't combine these loops!
    {   if (newsprite[whichsprite].active && newsprite[whichsprite].majory == 9 && newsprite[whichsprite].minory == newsprite[whichsprite].size - 1)
        {   newspritedone(whichsprite);
}   }   }

EXPORT void changeabspixel(int x, int y, int colour)
{   if (x >= absxmin && x <= absxmax && y >= absymin && y <= absymax)
    {   changepixel(x - absxmin, y - absymin, colour);
}   }
EXPORT void changethisabspixel(int colour)
{   if (cpux >= absxmin && cpux <= absxmax && cpuy >= absymin && cpuy <= absymax)
    {   changepixel(cpux - absxmin, cpuy - absymin, colour);
}   }
EXPORT void changethisbgpixel(int colour)
{   if (cpux >= absxmin && cpux <= absxmax && cpuy >= absymin && cpuy <= absymax)
    {   changebgpixel(cpux - absxmin, cpuy - absymin, colour);
}   }

EXPORT void changerelpixel(int x, int y, int colour)
{   x += USG_XMARGIN;
    y += USG_YMARGIN;

    changeabspixel(x, y, colour);
}

MODULE void newraster(void)
{   pviy = cpuy - USG_YMARGIN;
    if (pviread(PVI_SCORECTRL) & 1)
    {   digity = 200;
    } else
    {   digity =  20;
    }
    if (pviy == digity) // this is cached for eg. LASERATT game over screen
    {   if (memory[0x1F00 + PVI_BGCOLOUR] & 8)
        {   digitcolour = (memory[0x1F00 + PVI_BGCOLOUR] >> 4) & 7;
        } else
        {   digitcolour = WHITE;
    }   }

    if (dejitter)
    {   bgcread = memory[0x1F00 + PVI_BGCOLOUR];
        if (pviy != 0 && whichgame != -1 && pviy == (int) known[whichgame].banked)
        {   if     ((bgcread & 7) == (UBYTE) (7 - known[whichgame].the1staddr))
            {   bgcread = bgcread & 0xF8;
                bgcread |= 7 - known[whichgame].the1stsize;
            } elif ((bgcread & 7) == (UBYTE) (7 - known[whichgame].the2ndaddr))
            {   bgcread = bgcread & 0xF8;
                bgcread |= 7 - known[whichgame].the2ndsize;
        }   }
        if (memmap != MEMMAP_E && (memory[IE_NOISE] & 0x20)) // invert
        {   if (bgcread & 8) // background/grid enable
            {   bgc = bgcread & 7;
            } else
            {   bgc = 0; // white
        }   }
        else
        {   if (bgcread & 8) // background/grid enable
            {   if (darkenbg)
                {   bgc = 15 - (bgcread & 7);
                } else
                {   bgc =  7 - (bgcread & 7);
            }   }
            else
            {   bgc = 7; // black
}   }   }   }

MODULE __inline void do_cpu(void)
{
#ifdef OPCOLOURS
    if (cpuy >= absymin && cpuy <= absymax)
    {   changethisabspixel(table_opcolours_2650[supercpu][opcode]);
    }
    screen_iar[cpux][cpuy] = iar;
#endif

    if (cpux == nextinst)
    {   oldcycles = cycles_2650;
        one_instruction();
        nextinst += (cycles_2650 - oldcycles) * ppc; // in pixels
        if (nextinst >= 227)
        {   nextinst -= cpl;
}   }   }

MODULE void ie_emuinput(void)
{   FAST int i,
             player;

    for (i = 0x1E88; i <= 0x1E8E; i++)
    {   memory[i] = 0x0F;
    }
    for (player = 0; player < 2; player++)
    {   if (hinput[player] & (1 <<  0)) memory[IE_P1LEFTKEYS   + (player * 4)] |= 0x80; // "1"
        if (hinput[player] & (1 <<  3)) memory[IE_P1LEFTKEYS   + (player * 4)] |= 0x40; // "4"
        if (hinput[player] & (1 <<  6)) memory[IE_P1LEFTKEYS   + (player * 4)] |= 0x20; // "7"
        if (hinput[player] & (1 <<  9)) memory[IE_P1LEFTKEYS   + (player * 4)] |= 0x10; // "Cl"
        if (hinput[player] & (1 <<  1)) memory[IE_P1MIDDLEKEYS + (player * 4)] |= 0x80; // "2"
        if (hinput[player] & (1 <<  4)) memory[IE_P1MIDDLEKEYS + (player * 4)] |= 0x40; // "5"
        if (hinput[player] & (1 <<  7)) memory[IE_P1MIDDLEKEYS + (player * 4)] |= 0x20; // "8"
        if (hinput[player] & (1 << 10)) memory[IE_P1MIDDLEKEYS + (player * 4)] |= 0x10; // "0"
        if (hinput[player] & (1 <<  2)) memory[IE_P1RIGHTKEYS  + (player * 4)] |= 0x80; // "3"
        if (hinput[player] & (1 <<  5)) memory[IE_P1RIGHTKEYS  + (player * 4)] |= 0x40; // "6"
        if (hinput[player] & (1 <<  8)) memory[IE_P1RIGHTKEYS  + (player * 4)] |= 0x20; // "9"
        if (hinput[player] & (1 << 11)) memory[IE_P1RIGHTKEYS  + (player * 4)] |= 0x10; // "En"
        if (psu & PSU_F) // paddle interpolation bit
        {   memory[0x1F00 + PVI_P1PADDLE + player] = hy[player];
        } else
        {   memory[0x1F00 + PVI_P1PADDLE + player] = hx[player];
        }
        if   (hy[player] <=  64) setpaddle(player, paddleup);
        elif (hy[player] >= 192) setpaddle(player, paddledown);
        if   (hx[player] <=  64) setpaddle(player, paddleleft);
        elif (hx[player] >= 192) setpaddle(player, paddleright);
    }

    if (console_start) { memory[IE_CONSOLE] |= 0x40;                                       console_start--; } // START
    if (console_a    ) { memory[IE_CONSOLE] |= 0x80;                                       console_a--;     } // A
    if (console_b    ) { memory[IE_CONSOLE] |= 0x20;                                       console_b--;     } // B
    if (console_reset) { if (machine == ELEKTOR) memory[IE_CONSOLE] |= 0x10; else iar = 0; console_reset--; } // RESET

    p1rumble = p2rumble = 0;
}

MODULE void setpaddle(int player, int which)
{   switch (which)
    {
    case   0: memory[IE_P1MIDDLEKEYS + (player * 4)] |= 0x10; // "0"
    acase  1: memory[IE_P1LEFTKEYS   + (player * 4)] |= 0x80; // "1"
    acase  2: memory[IE_P1MIDDLEKEYS + (player * 4)] |= 0x80; // "2"
    acase  3: memory[IE_P1RIGHTKEYS  + (player * 4)] |= 0x80; // "3"
    acase  4: memory[IE_P1LEFTKEYS   + (player * 4)] |= 0x40; // "4"
    acase  5: memory[IE_P1MIDDLEKEYS + (player * 4)] |= 0x40; // "5"
    acase  6: memory[IE_P1RIGHTKEYS  + (player * 4)] |= 0x40; // "6"
    acase  7: memory[IE_P1LEFTKEYS   + (player * 4)] |= 0x20; // "7"
    acase  8: memory[IE_P1MIDDLEKEYS + (player * 4)] |= 0x20; // "8"
    acase  9: memory[IE_P1RIGHTKEYS  + (player * 4)] |= 0x20; // "9"
    acase 10: memory[IE_P1LEFTKEYS   + (player * 4)] |= 0x10; // "Cl"
    acase 11: memory[IE_P1RIGHTKEYS  + (player * 4)] |= 0x10; // "En"
}   }

EXPORT void set_retuning(void)
{   // Musical Games expects these
    idealfreq_ntsc[0x18] = idealfreq_pal[0x18] = HZ_E4;
    idealfreq_ntsc[0x19] = idealfreq_pal[0x19] = HZ_DS4;
    idealfreq_ntsc[0x1B] = idealfreq_pal[0x1B] = HZ_D4;

    // Surround expects this
    idealfreq_ntsc[0x28] = idealfreq_pal[0x28] = HZ_FS3;
}

MODULE void run_cpu(int until)
{   // This is a quicker equivalent to repeatedly incrementing cpux and calling do_cpu().

    cpux = nextinst;
    while (cpux < until)
    {   oldcycles = cycles_2650;
        one_instruction();
        nextinst += (cycles_2650 - oldcycles) * ppc; // in pixels
        cpux = nextinst;
    }
    if (nextinst >= 227)
    {   nextinst -= cpl;
}   }
