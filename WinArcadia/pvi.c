/* pvi.c: Signetics 2636 PVI emulation module (host-independent)

INCLUDES--------------------------------------------------------------- */

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
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"

// DEFINES----------------------------------------------------------------

// #define NOGRID
// whether to turn off grid

// #define WATCHPVIREADS
// whether reads by the PVI will trigger watchpoints

#define WATCHPVIWRITES
// whether writes by the PVI will trigger watchpoints

// #define DEBUGSPRITES
// to show more info about sprites

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

IMPORT       FLAG                  inframe,
                                   lmb, mmb, rmb,
                                   protect[4];
IMPORT       ASCREEN               screen[BOXWIDTH][BOXHEIGHT];
IMPORT       UBYTE*                IOBuffer;
IMPORT       FILE*                 MacroHandle;
IMPORT       TEXT                  friendly[FRIENDLYLENGTH + 1],
                                   imgbits[8 + 1],
                                   netmsg_out[80 + 1],
                                   stringchar[19 + 1]; // enough for "%1,1,1,1,1,1,1,1"
IMPORT       int                   absxmin, absxmax,
                                   absymin, absymax,
                                   ax[2],
                                   ay[4],
                                   connected,
                                   console_start,
                                   console_a,
                                   console_b,
                                   console_reset,
                                   cpl,
                                   cpux,
                                   cpuy,
                                   dejitter,
                                   drawcorners,
                                   drawmode,
                                   editscreen,
                                   hostcontroller[2],
                                   interrupt_2650,
                                   key1,
                                   key2,
                                   key3,
                                   key4,
                                   kx, ky,
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
                                   offset,
                                   recmode,
                                   requirebutton[2],
                                   p1rumble,
                                   p2rumble,
                                   rumbling[2],
                                   scrnaddr,
                                   spriteflip,
                                   spriteflips,
                                   spritemode,
                                   supercpu,
                                   trace,
                                   undither,
                                   usemargins,
                                   watchreads,
                                   watchwrites,
                                   whichgame;
IMPORT       UBYTE                 cc,
                                   aw_dips1,
                                   ga_dips,
                                   KeyMatrix[SCANCODES / 8],
                                   opcode,
                                   psu,
                                   memory[32768],
                                   OutputBuffer[18],
                                   sx[2], sy[2];
IMPORT       UWORD                 console[4],
                                   iar,
                                   keypads[2][NUMKEYS],
                                   mirror_r[32768],
                                   mirror_w[32768],
                                   screen_iar[BOXWIDTH][BOXHEIGHT];
IMPORT       ULONG                 autofire[2],
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
IMPORT       STRPTR                colourname[8];
IMPORT       MEMFLAG               memflags[ALLTOKENS];
IMPORT       struct ConditionalStruct wp[ALLTOKENS];
IMPORT       struct IdealStruct    idealfreq_ntsc[256],
                                   idealfreq_pal[256];
IMPORT       struct MachineStruct  machines[MACHINES];
IMPORT       struct NoteStruct     notes[NOTES + 1];
IMPORT const UBYTE                 table_opcolours_2650[2][256];
IMPORT const int                   from_astrowars_spr[8],
                                   from_galaxia_spr[8],
                                   from_malzak_spr[8],
                                   guest_to_ansi_colour[8];
IMPORT const struct KeyTableStruct keytable[16];
IMPORT const struct KnownStruct    known[KNOWNGAMES];

#ifdef WIN32
    IMPORT       int               CatalogPtr;
#endif
#ifdef AMIGA
    IMPORT       int               size;
    IMPORT       ULONG             longpens[PENS];
    IMPORT       WORD              width;
    IMPORT       struct Catalog*   CatalogPtr;
#endif

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
MODULE UBYTE  dmascreen[312][227];

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

EXPORT const UBYTE pvi_sprnumbers[2][4][10] = { {
{ 0xFF, // ########
  0xFF, // ########
  0xE7, // ###..###
  0xDB, // ##.##.##
  0xDB, // ##.##.##
  0xDB, // ##.##.##
  0xDB, // ##.##.##
  0xE7, // ###..###
  0xFF, // ########
  0xFF  // ########
},
{ 0xFF, // ########
  0xFF, // ########
  0xF7, // ####.###
  0xE7, // ###..###
  0xF7, // ####.###
  0xF7, // ####.###
  0xF7, // ####.###
  0xE3, // ###...##
  0xFF, // ########
  0xFF  // ########
},
{ 0xFF, // ########
  0xFF, // ########
  0xE7, // ###..###
  0xDB, // ##.##.##
  0xFB, // #####.##
  0xE7, // ###..###
  0xDF, // ##.#####
  0xC3, // ##....##
  0xFF, // ########
  0xFF  // ########
},
{ 0xFF, // ########
  0xFF, // ########
  0xC7, // ##...###
  0xFB, // #####.##
  0xE7, // ###..###
  0xFB, // #####.##
  0xFB, // #####.##
  0xC7, // ##...###
  0xFF, // ########
  0xFF  // ########
} }, {
{ 0xFF, // ########
  0x81, // #......#
  0x99, // #..##..#
  0xA5, // #.#..#.#
  0xA5, // #.#..#.#
  0xA5, // #.#..#.#
  0xA5, // #.#..#.#
  0x99, // #..##..#
  0x81, // #......#
  0xFF  // ########
},
{ 0xFF, // ########
  0x81, // #......#
  0x89, // #...#..#
  0x99, // #..##..#
  0x89, // #...#..#
  0x89, // #...#..#
  0x89, // #...#..#
  0x9D, // #..###.#
  0x81, // #......#
  0xFF  // ########
},
{ 0xFF, // ########
  0x81, // #......#
  0x99, // #..##..#
  0xA5, // #.#..#.#
  0x85, // #....#.#
  0x99, // #..##..#
  0xA1, // #.#....#
  0xBD, // #.####.#
  0x81, // #......#
  0xFF  // ########
},
{ 0xFF, // ########
  0x81, // #......#
  0xB9, // #.###..#
  0x85, // #....#.#
  0x99, // #..##..#
  0x85, // #....#.#
  0x85, // #....#.#
  0xB9, // #.###..#
  0x81, // #......#
  0xFF  // ########
} } };

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void ie_playerinput(int source, int dest);
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
#ifdef DEBUGSPRITES
    MODULE void getimgbits(UBYTE data);
#endif
MODULE void run_cpu(int until);

// 8. CODE----------------------------------------------------------------

EXPORT void newpvi(void)
{   FAST int i, j,
             kheight,
             whichsprite,
             x, xx, y, yy;
PERSIST const UBYTE mininumber[5][5] = {
{ 6, // ##.
  2, // .#.
  2, // .#.
  2, // .#.
  7, // ###
},
{ 7, // ###
  1, // ..#
  7, // ###
  4, // #..
  7, // ###
},
{ 6, // ##.
  2, // .#.
  2, // .#.
  2, // .#.
  7, // ###
},
{ 5, // #.#
  5, // #.#
  7, // ###
  1, // ..#
  1, // ..#
},
{ 7, // ###
  5, // #.#
  7, // ###
  5, // #.#
  7, // ###
} };

    inframe = TRUE;

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
    memset(&colltable[0][0],     0, sizeof(colltable));
    memset(&dmascreen[0][0], BLACK, sizeof(dmascreen));

    cpuy = 0;
    breakrastline();
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
    pvi_vblank();
    run_cpu(227); // the rest of raster 0

    for (cpuy = 1; cpuy < n1; cpuy++) // typically 43
    {   breakrastline();
        run_cpu(227);
    }

    for (cpuy = n1; cpuy < n3; cpuy++) // typically n1 is 43 and and n3 is 312
    {   breakrastline();
        if (cpuy == n1)
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
            if (cpuy == n2) // ie. USG_YMARGIN + (269 / 2). "Conversion takes place during the active scan". Typically 177
            {   wa_checkinput();
                ie_emuinput();
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

    if (demultiplex != 0 && frames >= (ULONG) spriteflips && spritemode != SPRITEMODE_INVISIBLE)
    {   for (i = 0; i < 4; i++)
        {   for (j = 0; j < PREVSPRITES; j++)
            {   prevspritedone[i][j] = FALSE;
            }
         /* if (prevsprite[multiframe][i][j].inuse)
            {    zprintf
                 (   "#%d: X=%d, Y=%d, colour=%d\n",
                     i,
                     prevsprite[multiframe][i][j].leftx,
                     prevsprite[multiframe][i][j].topy,
                     prevsprite[multiframe][i][j].colour
                 );
            } */
        }

        for (i = spriteflips; i >= 1; i--)
        {   drawmultiplexedsprites((spriteflip + i) % (spriteflips + 1));
    }   }
    if (spriteflip == spriteflips)
    {   spriteflip = 0;
    } else
    {   spriteflip++;
    }

#ifdef SHOWRUMBLE
    if (rumbling[0])
    {   for (y = 0; y < machines[machine].height; y++)
        {   changepixel(                          0, y, 15 - screen[                          0][y]);
    }   }
    if (rumbling[1])
    {   for (y = 0; y < machines[machine].height; y++)
        {   changepixel(machines[machine].width - 1, y, 15 - screen[machines[machine].width - 1][y]);
    }   }
#endif

    if (editscreen)
    {   xx      = 31;
        yy      = 19 + ((ky / 6) * 40);
        kheight = 42;
        for (y = 0; y < kheight; y++)
        {   for (x = 0; x < 130; x++)
            {   if (y == 0 || y == kheight - 1 || x == 0 || x == 129)
                {   changerelpixel
                    (   xx + x,
                        yy + y,
                        PINK
                    );
        }   }   }
        for (y = 0; y < 5; y++)
        {   for (x = 0; x < 3; x++)
            {   if (mininumber[(memory[0x1FA8 + (ky / 6)] & 0xC0) >> 6][y] & (4 >> x))
                {   changerelpixel(xx + 131 + x, yy + 18 + y, PINK);
        }   }   }

        yy = 19 + ((ky / 3) * 20);
        if (ky % 3 == 0)
        {   kheight = 4;
        } else
        {   kheight = 11;
            if (ky % 3 == 1)
            {   yy +=  2;
            } else
            {   yy += 11;
        }   }
        for (y = 0; y < kheight; y++)
        {   for (x = 0; x < 130; x++)
            {   if (y == 0 || y == kheight - 1 || x == 0 || x == 129)
                {   changerelpixel
                    (   xx + x,
                        yy + y,
                        ORANGE
                    );
        }   }   }
        if (memory[0x1FA8 + (ky / 6)] & (1 << (ky % 6)))
        {   for (y = 0; y < 5; y++)
            {   for (x = 0; x < 3; x++)
                {   if (mininumber[4][y] & (4 >> x))
                    {   changerelpixel(27 + x, yy + (kheight / 2) - 2 + y, ORANGE);
        }   }   }   }
        else
        {   for (y = 0; y < 5; y++)
            {   for (x = 0; x < 3; x++)
                {   if (mininumber[(memory[0x1FA8 + (ky / 6)] & 0xC0) >> 6][y] & (4 >> x))
                    {   changerelpixel(27 + x, yy + (kheight / 2) - 2 + y, ORANGE);
        }   }   }   }

        xx = 31 + ( kx      *  8);
        yy = 19 + ((ky / 3) * 20);
        if (ky % 3 == 0)
        {   kheight = 4;
        } else
        {   kheight = 20;
            yy +=  2;
        }
        for (y = 0; y < kheight; y++)
        {   for (x = 0; x < 10; x++)
            {   if (y == 0 || y == kheight - 1 || x == 0 || x == 9)
                {   changerelpixel
                    (   xx + x,
                        yy + y,
                        GREY1
                    );
    }   }   }   }

    if (drawmode)
    {   newpvi_drawhelpgrid();
    }
    endofframe(bgc);
}

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
    {   // Grid-----------------------------------------------------------

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
        if ((colltable[pviy][pvix] & 0x30) == 0x30) { t |=  32; if (p1sprcol[0]   ) p1rumble = p1sprcol[0]; if (p2sprcol[0]   ) p2rumble = p2sprcol[0]; /* zprintf(TEXTPEN_VERBOSE, "#0 & #1!\n"); */ }
        if ((colltable[pviy][pvix] & 0x50) == 0x50) { t |=  16; if (p1sprcol[1]   ) p1rumble = p1sprcol[1]; if (p2sprcol[1]   ) p2rumble = p2sprcol[1]; /* zprintf(TEXTPEN_VERBOSE, "#0 & #2!\n"); */ }
        if ((colltable[pviy][pvix] & 0x90) == 0x90) { t |=   8; if (p1sprcol[2]   ) p1rumble = p1sprcol[2]; if (p2sprcol[2]   ) p2rumble = p2sprcol[2]; /* zprintf(TEXTPEN_VERBOSE, "#0 & #3!\n"); */ }
        if ((colltable[pviy][pvix] & 0x60) == 0x60) { t |=   4; if (p1sprcol[3]   ) p1rumble = p1sprcol[3]; if (p2sprcol[3]   ) p2rumble = p2sprcol[3]; /* zprintf(TEXTPEN_VERBOSE, "#1 & #2!\n"); */ }
        if ((colltable[pviy][pvix] & 0xA0) == 0xA0) { t |=   2; if (p1sprcol[4]   ) p1rumble = p1sprcol[4]; if (p2sprcol[4]   ) p2rumble = p2sprcol[4]; /* zprintf(TEXTPEN_VERBOSE, "#1 & #3!\n"); */ }
        if ((colltable[pviy][pvix] & 0xC0) == 0xC0) { t |=   1; if (p1sprcol[5]   ) p1rumble = p1sprcol[5]; if (p2sprcol[5]   ) p2rumble = p2sprcol[5]; /* zprintf(TEXTPEN_VERBOSE, "#2 & #3!\n"); */ }
        if (t)
        {   pviwrite(PVI_SPRITECOLLIDE   , (UBYTE) (pviread(PVI_SPRITECOLLIDE   ) | t), TRUE);
            pviwrite(PVI_SPRITECOLLIDE_M2, (UBYTE) (pviread(PVI_SPRITECOLLIDE_M2) | t), TRUE);
            pviwrite(PVI_SPRITECOLLIDE_M3, (UBYTE) (pviread(PVI_SPRITECOLLIDE_M3) | t), TRUE);
            pviwrite(PVI_SPRITECOLLIDE_M4, (UBYTE) (pviread(PVI_SPRITECOLLIDE_M4) | t), TRUE);
        }

        t = 0;
        if ((colltable[pviy][pvix] & 0x18) == 0x18) { t |= 128; if (p1bgcol[0] > 0) p1rumble = p1bgcol[ 0]; if (p2bgcol[0] > 0) p2rumble = p2bgcol[ 0]; /* zprintf(TEXTPEN_VERBOSE, "#0 & BG!\n"); */ }
        if ((colltable[pviy][pvix] & 0x28) == 0x28) { t |=  64; if (p1bgcol[1] > 0) p1rumble = p1bgcol[ 1]; if (p2bgcol[1] > 0) p2rumble = p2bgcol[ 1]; /* zprintf(TEXTPEN_VERBOSE, "#1 & BG!\n"); */ }
        if ((colltable[pviy][pvix] & 0x48) == 0x48) { t |=  32; if (p1bgcol[2] > 0) p1rumble = p1bgcol[ 2]; if (p2bgcol[2] > 0) p2rumble = p2bgcol[ 2]; /* zprintf(TEXTPEN_VERBOSE, "#2 & BG!\n"); */ }
        if ((colltable[pviy][pvix] & 0x88) == 0x88) { t |=  16; if (p1bgcol[3] > 0) p1rumble = p1bgcol[ 3]; if (p2bgcol[3] > 0) p2rumble = p2bgcol[ 3]; /* zprintf(TEXTPEN_VERBOSE, "#3 & BG!\n"); */ }
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

    if (drawmode == 4)
    {   if (dmascreen[cpuy][cpux] != BLACK)
        {   dmascreen[cpuy][cpux] = WHITE;
        } elif
        (   (address >= 0x1F00 && address <= 0x1F0D) // sprite #0
         || (address >= 0x1F10 && address <= 0x1F1D) // sprite #1
         || (address >= 0x1F20 && address <= 0x1F2D) // sprite #2
         || (address >= 0x1F40 && address <= 0x1F4D) // sprite #3
         || (address >= 0x1FC0 && address <= 0x1FC2) // SIZES/SPRnnCOLOURS
         || (address == 0x1FCA && address <= 0x1FCB) // BGCOLLIDE/SPRITECOLLIDE
        )
        {   dmascreen[cpuy][cpux] = PURPLE;
        } elif (address >= 0x1F80 && address <= 0x1FAC) // grid
        {   dmascreen[cpuy][cpux] = PINK;
        } elif (address == 0x1FC3 || address == 0x1FC8 || address == 0x1FC9) // SCORECTRL/SCORELT/SCORERT
        {   dmascreen[cpuy][cpux] = GREEN;
        } elif (address == 0x1FC7 || address == 0x1E80) // PITCH/NOISE
        {   dmascreen[cpuy][cpux] = YELLOW;
        } elif ((address >= 0x1E88 && address <= 0x1E8E) || address >= 0x1FCC || address == 0x1FCD) // PnFOOKEYS/CONSOLE/PnPADDLE
        {   dmascreen[cpuy][cpux] = ORANGE;
        } else
        {   dmascreen[cpuy][cpux] = GREY1;
    }   }

#ifdef WATCHPVIWRITES
    if
    (   (memflags[address] & WATCHPOINT)
     &&  watchwrites != WATCH_NONE
     && (watchwrites == WATCH_ALL || data != memory[address])
     && conditional(&wp[address], data, TRUE, 0)
    )
    {   if (machine == INTERTON || machine == ELEKTOR)
        {   ie_stringchar(data, address);
        } else
        {   stringchar[0] = EOS;
        }
        DISCARD getfriendly(address);
        zprintf
        (   TEXTPEN_DEBUG,
            LLL(MSG_PVI_HITWP, "PVI is writing $%X%s to %s at raster %ld!\n\n"),
                       data,
                       stringchar,
            (char*)    friendly,
            (long int) cpuy
        );
        set_pause(TYPE_BP);
    }
#endif

    memory[address] = data;
}

MODULE UBYTE pviread(int address)
{   address += pvibase;

    if (drawmode == 3)
    {   if (dmascreen[cpuy][cpux] != BLACK)
        {   dmascreen[cpuy][cpux] = WHITE;
        } elif
        (   (address >= 0x1F00 && address <= 0x1F0D) // sprite #0
         || (address >= 0x1F10 && address <= 0x1F1D) // sprite #1
         || (address >= 0x1F20 && address <= 0x1F2D) // sprite #2
         || (address >= 0x1F40 && address <= 0x1F4D) // sprite #3
         || (address >= 0x1FC0 && address <= 0x1FC2) // SIZES/SPRnnCOLOURS
         || (address == 0x1FCA && address <= 0x1FCB) // BGCOLLIDE/SPRITECOLLIDE
        )
        {   dmascreen[cpuy][cpux] = PURPLE;
        } elif (address >= 0x1F80 && address <= 0x1FAC) // grid
        {   dmascreen[cpuy][cpux] = PINK;
        } elif (address == 0x1FC3 || address == 0x1FC8 || address == 0x1FC9) // SCORECTRL/SCORELT/SCORERT
        {   dmascreen[cpuy][cpux] = GREEN;
        } elif (address == 0x1FC7 || address == 0x1E80) // PITCH/NOISE
        {   dmascreen[cpuy][cpux] = YELLOW;
        } elif ((address >= 0x1E88 && address <= 0x1E8E) || address >= 0x1FCC || address == 0x1FCD) // PnFOOKEYS/CONSOLE/PnPADDLE
        {   dmascreen[cpuy][cpux] = ORANGE;
        } else
        {   dmascreen[cpuy][cpux] = GREY1;
    }   }

#ifdef WATCHPVIREADS
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
#endif

    return memory[address];
}

MODULE void ie_playerinput(int source, int dest)
{   FAST ULONG jg;

    // dest is which side   (0 or 1) you want to set the registers of.
    // source is which side (0 or 1) you want to use to do it.

    if (recmode == RECMODE_PLAY)
    {   // IE_P1PADDLE is $1FCC, IE_P2PADDLE is $1FCD
        t = IOBuffer[offset++];
        pviwrite(PVI_P1PADDLE + dest, t, TRUE);
        if (psu & PSU_F) // paddle interpolation bit
        {   ay[dest] = sy[dest] = t;
        } else
        {   ax[dest] = sx[dest] = t;
        }
        t = IOBuffer[offset++];
        pviwrite(IE_P1LEFTKEYS   + (dest * 4), (UBYTE) ( (t & 0xF0)       | 0x0F), FALSE); // bits 7..4 -> bits  7..4 (masking is indeed necessary)
        pviwrite(IE_P1MIDDLEKEYS + (dest * 4), (UBYTE) (((t & 0x0F) << 4) | 0x0F), FALSE); // bits 3..0 -> bits  7..4
        t = IOBuffer[offset++];
        pviwrite(IE_P1RIGHTKEYS  + (dest * 4),            t                      , FALSE);
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

        memory[IE_P1LEFTKEYS   + (dest * 4)] =
        memory[IE_P1MIDDLEKEYS + (dest * 4)] =
        memory[IE_P1RIGHTKEYS  + (dest * 4)] = 0x0F;
        if (autofire[source] && (whichgame != ROCKETSHOOTINGPOS || dest == 0))
        {   if
            (   (   !requirebutton[source]
                 || (jg & (JOYFIRE1 | keytable[key1].dapter))
                 || KeyDown(keypads[source][0])
                 || (   hostcontroller[source] == CONTROLLER_TRACKBALL
                     && lmb
                )   )
             && ((frames % totalframes) < downframes)
            )
            {   // 1st firebutton
                memory[keytable[key1].ie_address + (dest * 4)] |= keytable[key1].ie_mask;
        }   }
        else
        {   if
            (   (jg & (JOYFIRE1 | keytable[key1].dapter))
             || KeyDown(keypads[source][0])
             || (hostcontroller[source] == CONTROLLER_TRACKBALL && lmb)
            )
            {   // 1st firebutton
                memory[keytable[key1].ie_address + (dest * 4)] |= keytable[key1].ie_mask;
        }   }
        if
        (   (jg & JOYFIRE2)
         || (KeyDown(keypads[source][21]))
         || (hostcontroller[source] == CONTROLLER_TRACKBALL && mmb)
        )
        {   // 2nd firebutton
            memory[keytable[key2].ie_address + (dest * 4)] |= keytable[key2].ie_mask;
        }
        if
        (   (jg & JOYFIRE3)
         || (KeyDown(keypads[source][22]))
         || (hostcontroller[source] == CONTROLLER_TRACKBALL && rmb)
        )
        {   // 3rd firebutton
            memory[keytable[key3].ie_address + (dest * 4)] |= keytable[key3].ie_mask;
        }
        if
        (   (jg & JOYFIRE4)
         || (KeyDown(keypads[source][23]))
        )
        {   // 4th firebutton
            memory[keytable[key4].ie_address + (dest * 4)] |= keytable[key4].ie_mask;
        }

        // left column
        t =  memory[IE_P1LEFTKEYS + (dest * 4)];
        t |= (UBYTE) (0x80 * KeyDown(keypads[source][ 1])); // "1"  key (Interton) or "RCAS"  key (Elektor)
        t |= (UBYTE) (0x40 * KeyDown(keypads[source][ 4])); // "4"  key (Interton) or "BP1/2" key (Elektor)
        t |= (UBYTE) (0x20 * KeyDown(keypads[source][ 7])); // "7"  key (Interton) or "PC"    key (Elektor)
        t |= (UBYTE) (0x10 * KeyDown(keypads[source][10])); // "Cl" key (Interton) or "-"     key (Elektor)
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_1) && (jff[source] & DAPTER_1)) t |= 8;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_4) && (jff[source] & DAPTER_4)) t |= 4;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_7) && (jff[source] & DAPTER_7)) t |= 2;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_C) && (jff[source] & DAPTER_C)) t |= 1;

        if (ay[dest] <=  64)
        {   if   (paddleup    ==  1) t |= 0x80;
            elif (paddleup    ==  4) t |= 0x40;
            elif (paddleup    ==  7) t |= 0x20;
            elif (paddleup    == 10) t |= 0x10;
        } elif (ay[dest] >= 192)
        {   if   (paddledown  ==  1) t |= 0x80;
            elif (paddledown  ==  4) t |= 0x40;
            elif (paddledown  ==  7) t |= 0x20;
            elif (paddledown  == 10) t |= 0x10;
        }
        if (ax[dest] <=  64)
        {   if   (paddleleft  ==  1) t |= 0x80;
            elif (paddleleft  ==  4) t |= 0x40;
            elif (paddleleft  ==  7) t |= 0x20;
            elif (paddleleft  == 10) t |= 0x10;
        } elif (ax[dest] >= 192)
        {   if   (paddleright ==  1) t |= 0x80;
            elif (paddleright ==  4) t |= 0x40;
            elif (paddleright ==  7) t |= 0x20;
            elif (paddleright == 10) t |= 0x10;
        }
        pviwrite(IE_P1LEFTKEYS    + (dest * 4), t, FALSE);

        // middle column
        t =  memory[IE_P1MIDDLEKEYS + (dest * 4)];
        t |= (UBYTE) (0x80 * KeyDown(keypads[source][ 2])); // "2"  key (Interton) or "WCAS" key (Elektor)
        t |= (UBYTE) (0x40 * KeyDown(keypads[source][ 5])); // "5"  key (Interton) or "REG"  key (Elektor)
        t |= (UBYTE) (0x20 * KeyDown(keypads[source][ 8])); // "8"  key (Interton) or "MEM"  key (Elektor)
        t |= (UBYTE) (0x10 * KeyDown(keypads[source][11])); // "0"  key (Interton) or "+"    key (Elektor)
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_2) && (jff[source] & DAPTER_2)) t |= 8;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_5) && (jff[source] & DAPTER_5)) t |= 4;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_8) && (jff[source] & DAPTER_8)) t |= 2;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_0) && (jff[source] & DAPTER_0)) t |= 1;

        if (ay[dest] <=  64)
        {   if   (paddleup    ==  2) t |= 0x80;
            elif (paddleup    ==  5) t |= 0x40;
            elif (paddleup    ==  8) t |= 0x20;
            elif (paddleup    ==  0) t |= 0x10;
        } elif (ay[dest] >= 192)
        {   if   (paddledown  ==  2) t |= 0x80;
            elif (paddledown  ==  5) t |= 0x40;
            elif (paddledown  ==  8) t |= 0x20;
            elif (paddledown  ==  0) t |= 0x10;
        }
        if (ax[dest] <=  64)
        {   if   (paddleleft  ==  2) t |= 0x80;
            elif (paddleleft  ==  5) t |= 0x40;
            elif (paddleleft  ==  8) t |= 0x20;
            elif (paddleleft  ==  0) t |= 0x10;
        } elif (ax[dest] >= 192)
        {   if   (paddleright ==  2) t |= 0x80;
            elif (paddleright ==  5) t |= 0x40;
            elif (paddleright ==  8) t |= 0x20;
            elif (paddleright ==  0) t |= 0x10;
        }
        pviwrite(IE_P1MIDDLEKEYS  + (dest * 4), t, FALSE);

        // right column
        t =  memory[IE_P1RIGHTKEYS + (dest * 4)];
        t |= (UBYTE) (0x80 * KeyDown(keypads[source][ 3])); // "3"  key (Interton) or "C" key (Elektor)
        t |= (UBYTE) (0x40 * KeyDown(keypads[source][ 6])); // "6"  key (Interton) or "8" key (Elektor)
        t |= (UBYTE) (0x20 * KeyDown(keypads[source][ 9])); // "9"  key (Interton) or "4" key (Elektor)
        t |= (UBYTE) (0x10 * KeyDown(keypads[source][12])); // "En" key (Interton) or "0" key (Elektor)
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_3) && (jff[source] & DAPTER_3)) t |= 8;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_6) && (jff[source] & DAPTER_6)) t |= 4;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_9) && (jff[source] & DAPTER_9)) t |= 2;
        if ((!autofire[source] || keytable[key1].dapter != DAPTER_E) && (jff[source] & DAPTER_E)) t |= 1;

        if (ay[dest] <=  64)
        {   if   (paddleup    ==  3) t |= 0x80;
            elif (paddleup    ==  6) t |= 0x40;
            elif (paddleup    ==  9) t |= 0x20;
            elif (paddleup    == 11) t |= 0x10;
        } elif (ay[dest] >= 192)
        {   if   (paddledown  ==  3) t |= 0x80;
            elif (paddledown  ==  6) t |= 0x40;
            elif (paddledown  ==  9) t |= 0x20;
            elif (paddledown  == 11) t |= 0x10;
        }
        if (ax[dest] <=  64)
        {   if   (paddleleft  ==  3) t |= 0x80;
            elif (paddleleft  ==  6) t |= 0x40;
            elif (paddleleft  ==  9) t |= 0x20;
            elif (paddleleft  == 11) t |= 0x10;
        } elif (ax[dest] >= 192)
        {   if   (paddleright ==  3) t |= 0x80;
            elif (paddleright ==  6) t |= 0x40;
            elif (paddleright ==  9) t |= 0x20;
            elif (paddleright == 11) t |= 0x10;
        }
        pviwrite(IE_P1RIGHTKEYS   + (dest * 4), t, FALSE);

        engine_dopaddle(source, dest);

        if (psu & PSU_F) // paddle interpolation bit
        {   pviwrite(PVI_P1PADDLE + dest, (UBYTE) ay[dest], TRUE);
            sy[dest] = (UBYTE) ay[dest];
        } else
        {   pviwrite(PVI_P1PADDLE + dest, (UBYTE) ax[dest], TRUE);
            sx[dest] = (UBYTE) ax[dest];
}   }   }

MODULE void ie_emuinput(void)
{   UBYTE therecv[4],
          thesend[4];

    // assert(machine == INTERTON || machine == ELEKTOR);

#ifdef DEBUGPSU
    if (psu & PSU_F) // paddle interpolation bit
    {   zprintf(TEXTPEN_VERBOSE, "Reading vertical   axis at frame %d, raster %d!\n", frames, cpuy);
    } else
    {   zprintf(TEXTPEN_VERBOSE, "Reading horizontal axis at frame %d, raster %d!\n", frames, cpuy);
    }
#endif

    ReadJoystick(0);
    ReadJoystick(1);
    // must always do ie_playerinput(foo, 0) then ie_playerinput(foo, 1).
    if (swapped)
    {   ie_playerinput(1, 0);
        ie_playerinput(0, 1);
    } else
    {   ie_playerinput(0, 0);
        ie_playerinput(1, 1);
    }
    p1rumble =
    p2rumble = 0;

    if (recmode != RECMODE_PLAY)
    {   if (KeyDown(console[0]) || (jff[0] & JOYSTART) || (jff[1] & JOYSTART) || console_start) // START /START
        {   t = 0x4F;
        } else
        {   t = 0x0F;
        }
        if (KeyDown(console[1]) || (jff[0] & JOYA    ) || (jff[1] & JOYA    ) || console_a    ) // SELECT/UC
        {   t |= 0x80;
        }
        if (machine == ELEKTOR)
        {   if (KeyDown(console[2]) || (jff[0] & JOYB) || (jff[1] & JOYB    ) || console_b    ) // -/LC
            {   t |= 0x20;
            }
            if (KeyDown(console[3]) || console_reset) // -/RESET (soft)
            {   t |= 0x10;
        }   }

        pviwrite(IE_CONSOLE, t, FALSE);

        domouse();
    }

    if (connected == NET_SERVER)
    {   // we are left player

        thesend[0] = ((memory[IE_P1LEFTKEYS  ] & 0x0F) <<  4) // bits 3..0 -> bits  7..4
                   |  (memory[IE_P1MIDDLEKEYS] & 0x0F);       // bits 3..0 -> bits  3..0
        thesend[1] =  (memory[IE_P1RIGHTKEYS ] & 0x0F)        // bits 3..0 -> bits  3..0
                   |  (memory[IE_CONSOLE     ] & 0xF0);       // bits 7..4 -> bits  7..4
        thesend[2] =   pviread(PVI_P1PADDLE);                 // bits 7..0 -> bits  7..0
        thesend[3] = strlen((const char*) netmsg_out);
#ifdef LOGNETWORK
        zprintf(TEXTPEN_VERBOSE, "Server: Sending  $%02X, $%02X, $%02X, %d.\n", thesend[0], thesend[1], thesend[2], thesend[3]);
#endif
        DISCARD NetSend((const char*) thesend, 4);
        send_usermsg((int) thesend[3]);

        if (NetRecv((char*) therecv, 4))
        {
#ifdef LOGNETWORK
            zprintf(TEXTPEN_VERBOSE, "Server: Received $%02X, $%02X, $%02X, %d.\n", therecv[0], therecv[1], therecv[2], therecv[3]);
#endif
            pviwrite(IE_P2LEFTKEYS  , (UBYTE) (((therecv[0] & 0xF0) >> 4) | 0x0F              ), FALSE); // bits 7..4 -> bits 3..0
            pviwrite(IE_P2MIDDLEKEYS, (UBYTE) (( therecv[0] & 0x0F      ) | 0x0F              ), FALSE); // bits 3..0 -> bits 3..0
            pviwrite(IE_P2RIGHTKEYS , (UBYTE) (( therecv[1] & 0x0F      ) | 0x0F              ), FALSE); // bits 3..0 -> bits 3..0 (masking not really needed)
            pviwrite(PVI_P2PADDLE   ,            therecv[2]                                    , TRUE );
            recv_usermsg((int) therecv[3]);
    }   }
    elif (connected == NET_CLIENT)
    {   // we are right player

        thesend[0] = ((memory[IE_P2LEFTKEYS  ] & 0x0F) <<  4) // bits 3..0 -> bits  7..4
                   |  (memory[IE_P2MIDDLEKEYS] & 0x0F);       // bits 3..0 -> bits  3..0
        thesend[1] =  (memory[IE_P2RIGHTKEYS ] & 0x0F);       // bits 3..0 -> bits  3..0
        thesend[2] =   pviread(PVI_P2PADDLE);                 // bits 7..0 -> bits  7..0
        thesend[3] = strlen((const char*) netmsg_out);
#ifdef LOGNETWORK
        zprintf(TEXTPEN_VERBOSE, "Client: Sending  $%02X, $%02X, $%02X, %d.\n", thesend[0], thesend[1], thesend[2], thesend[3]);
#endif
        DISCARD NetSend((const char*) thesend, 4);
        send_usermsg((int) thesend[3]);

        if (NetRecv((char*) therecv, 4))
        {
#ifdef LOGNETWORK
            zprintf(TEXTPEN_VERBOSE, "Client: Received $%02X, $%02X, $%02X, %d.\n", therecv[0], therecv[1], therecv[2], therecv[3]);
#endif
            pviwrite(IE_P1LEFTKEYS  , (UBYTE) (((therecv[0] & 0xF0) >> 4) | 0x0F              ), FALSE); // bits 7..4 -> bits 3..0
            pviwrite(IE_P1MIDDLEKEYS, (UBYTE) (( therecv[0] & 0x0F      ) | 0x0F              ), FALSE); // bits 3..0 -> bits 3..0
            pviwrite(IE_P1RIGHTKEYS , (UBYTE) (( therecv[1] & 0x0F      ) | 0x0F              ), FALSE); // bits 3..0 -> bits 3..0 (masking not really needed)
            pviwrite(PVI_P1PADDLE   ,            therecv[2]                                    , TRUE ); // bits 7..0 -> bits 7..0
            pviwrite(IE_CONSOLE     , (UBYTE) (( therecv[1] & 0xF0      ) | memory[IE_CONSOLE]), FALSE);
            recv_usermsg((int) therecv[3]);
    }   }

    if (recmode == RECMODE_PLAY)
    {   pviwrite(IE_CONSOLE, IOBuffer[offset++], FALSE);
    } elif (recmode == RECMODE_RECORD)
    {   // assert(MacroHandle);

        OutputBuffer[0] =   pviread(PVI_P1PADDLE);
        OutputBuffer[1] =  (memory[IE_P1LEFTKEYS  ] & 0xF0)         // bits 7..4 -> bits  7..4 (masking not really needed)
                        | ((memory[IE_P1MIDDLEKEYS] & 0xF0) >>  4); // bits 7..4 -> bits  3..0 (masking not really needed)
        OutputBuffer[2] =   memory[IE_P1RIGHTKEYS ];                // 4 bits are wasted
        OutputBuffer[3] =   pviread(PVI_P2PADDLE);
        OutputBuffer[4] =  (memory[IE_P2LEFTKEYS  ] & 0xF0)         // bits 7..4 -> bits  7..4 (masking not really needed)
                        | ((memory[IE_P2MIDDLEKEYS] & 0xF0) >>  4); // bits 7..4 -> bits  3..0 (masking not really needed)
        OutputBuffer[5] =   memory[IE_P2RIGHTKEYS ];                // 4 bits are wasted
        OutputBuffer[6] =   memory[IE_CONSOLE     ];                // 6 (Interton) or 4 (Elektor) bits are wasted

        DISCARD fwrite(OutputBuffer, 7, 1, MacroHandle);
        /* fwrite() should really check return code
        7 bytes per frame * 50 frames per second = 350 bytes/sec.
        Plus, for Elektor, 1 byte every time the E_RANDOMn hardware
        registers are read. */
    }

    if (console_start) console_start--;
    if (console_a    ) console_a--;
    if (console_b    ) console_b--;
    if (console_reset) console_reset--;
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

EXPORT void ie_viewscreen(void)
{   // assert(machine == INTERTON || machine == ELEKTOR);

    UWORD where = 0x1F80;
    UBYTE data;
    int   i, j;

    for (j = 0; j < 20; j++)
    {   zprintf
        (   TEXTPEN_VIEW,
            "$%04X: %02X %02X ",
            where,
            memory[where],
            memory[where + 1]
        );

        data = memory[where++];
        for (i = 0; i < 8; i++)
        {   if (data & (128 >> i))
            {   zprintf(TEXTPEN_VIEW, "#");
            } else
            {   zprintf(TEXTPEN_VIEW, ".");
        }   }
        zprintf(TEXTPEN_VIEW, " ");
        data = memory[where++];
        for (i = 0; i < 8; i++)
        {   if (data & (128 >> i))
            {   zprintf(TEXTPEN_VIEW, "#");
            } else
            {   zprintf(TEXTPEN_VIEW, ".");
        }   }
        zprintf(TEXTPEN_VIEW, "\n");
    }

    zprintf
    (   TEXTPEN_VIEW,
        "$1FA8: %02X %02X %02X %02X %02X\n\n",
        memory[0x1FA8],
        memory[0x1FA9],
        memory[0x1FAA],
        memory[0x1FAB],
        memory[0x1FAC]
    );
}

EXPORT void pvi_memmap(void)
{   int i;

    for (i = 0; i <= 255; i++)
    {   memflags[pvibase + i] = 0;
        mirror_r[pvibase + i] =
        mirror_w[pvibase + i] = (UWORD) (pvibase + i);

        if
        (   (i >= 0x0A && i <= 0x0D) // SPRITE0AX..SPRITE0BY
         || (i >= 0x1A && i <= 0x1D) // SPRITE1AX..SPRITE1BY
         || (i >= 0x2A && i <= 0x2D) // SPRITE2AX..SPRITE2BY
         || (i >= 0x4A && i <= 0x4D) // SPRITE3AX..SPRITE3BY
         || (i >= 0xC0 && i <= 0xCB) // SIZES..SPRITECOLLIDE
         || (i >= 0xCC && i <= 0xCD) // P1PADDLE..P2PADDLE
        )
        {   memflags[pvibase + i] |= ASIC;
        }

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

EXPORT void pvi_view_udgs(void)
{   int    column,
           row,
           whichpvi,
           whichsprite;
    UBYTE  thecolour = 0,
           thesize   = 0; // both initialized to avoid spurious SAS/C warnings
    TEXT   bits[8 + 1];

    // assert(machines[machine].pvis);

    bits[8] = EOS;

    // We deliberately don't use pviread()

    for (whichpvi = 0; whichpvi < machines[machine].pvis; whichpvi++)
    {   for (row = 0; row < 10; row++)
        {   for (whichsprite = 0; whichsprite < 4; whichsprite++)
            {   for (column = 0; column < 8; column++)
                {   if (memory[pvibase + pvi_spritedata[whichsprite] + row] & (0x80 >> column))
                    {   bits[column] = '#';
                    } else
                    {   bits[column] = '.';
                }   }

                switch (whichsprite)
                {
                case 0:
                    thecolour = (memory[pvibase + PVI_SPR01COLOURS] >> 3) & 7;
                    thesize   =  memory[pvibase + PVI_SIZES       ]       & 3;
                acase 1:
                    thecolour =  memory[pvibase + PVI_SPR01COLOURS]       & 7;
                    thesize   = (memory[pvibase + PVI_SIZES       ] >> 2) & 3;
                acase 2:
                    thecolour = (memory[pvibase + PVI_SPR23COLOURS] >> 3) & 7;
                    thesize   = (memory[pvibase + PVI_SIZES       ] >> 4) & 3;
                acase 3:
                    thecolour =  memory[pvibase + PVI_SPR23COLOURS]       & 7;
                    thesize   = (memory[pvibase + PVI_SIZES       ] >> 6) & 3;
                }
                switch (memmap)
                {
                case MEMMAP_ASTROWARS:
                    thecolour = from_astrowars_spr[thecolour];
                acase MEMMAP_GALAXIA:
                case MEMMAP_LASERBATTLE:
                case MEMMAP_LAZARIAN:
                    thecolour = from_galaxia_spr[  thecolour];
                acase MEMMAP_MALZAK1:
                case MEMMAP_MALZAK2:
                    thecolour = from_malzak_spr[   thecolour];
                }

                switch (row)
                {
                case 0:
                    zprintf
                    (   guest_to_ansi_colour[thecolour],
                        "SPRITE%02d: %s ",
                        (whichpvi * 4) + whichsprite,
                        bits
                    );
                acase 1:
                    switch (thesize)
                    {
                    case 0:
                        zprintf
                        (   guest_to_ansi_colour[thecolour],
                            "   (8*10) %s ",
                            bits
                        );
                    acase 1:
                        zprintf
                        (   guest_to_ansi_colour[thecolour],
                            "  (16*20) %s ",
                            bits
                        );
                    acase 2:
                        zprintf
                        (   guest_to_ansi_colour[thecolour],
                            "  (32*40) %s ",
                            bits
                        );
                    acase 3:
                        zprintf
                        (   guest_to_ansi_colour[thecolour],
                            "  (64*80) %s ",
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
        pvibase += 0x100;
    }

    pvibase = machines[machine].pvibase;
}

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
        if (log_interrupts)
        {   if (psu & PSU_II)
            {   zprintf
                (   TEXTPEN_LOG,
                    "Pended %s sprite #%d display complete interrupt at rastline %d.\n",
                    newsprite[whichsprite].dup ? "duplicate" : "original",
                    whichsprite,
                    cpuy
                );
            } else
            {   zprintf
                (   TEXTPEN_LOG,
                    "Generated %s sprite #%d display complete interrupt at rastline %d.\n",
                    newsprite[whichsprite].dup ? "duplicate" : "original",
                    whichsprite,
                    cpuy
                );
    }   }   }
    elif (log_interrupts)
    {   zprintf
        (   TEXTPEN_LOG,
            "%s sprite #%d display is complete at rastline %d. An interrupt is already pended.\n",
            newsprite[whichsprite].dup ? "Duplicate" : "Original",
            whichsprite,
            cpuy
        );
    }

    t = pviread(pvi_spritedata[whichsprite] + 13); // SPRITEnBY
    if (t == 255)
    {   newsprite[whichsprite].newstarty = pviy + 1;
    } else
    {   newsprite[whichsprite].newstarty = pviy + 2 + t;
    }
    newsprite[whichsprite].ending = TRUE;
}

EXPORT void pvi_vblank(void) // used by pvi.c and coinops.c
{   pviwrite(PVI_SPRITECOLLIDE   , (UBYTE) (memory[pvibase + PVI_SPRITECOLLIDE   ] | 0x40), TRUE); // set VRST bit
    pviwrite(PVI_SPRITECOLLIDE_M2, (UBYTE) (memory[pvibase + PVI_SPRITECOLLIDE_M2] | 0x40), TRUE); // set VRST bit
    pviwrite(PVI_SPRITECOLLIDE_M3, (UBYTE) (memory[pvibase + PVI_SPRITECOLLIDE_M3] | 0x40), TRUE); // set VRST bit
    pviwrite(PVI_SPRITECOLLIDE_M4, (UBYTE) (memory[pvibase + PVI_SPRITECOLLIDE_M4] | 0x40), TRUE); // set VRST bit
    if (machines[machine].pvis >= 2)
    {   pvibase += 0x100; // 2nd PVI
        pviwrite(PVI_SPRITECOLLIDE   , (UBYTE) (memory[pvibase + PVI_SPRITECOLLIDE   ] | 0x40), TRUE); // set VRST bit
        pviwrite(PVI_SPRITECOLLIDE_M2, (UBYTE) (memory[pvibase + PVI_SPRITECOLLIDE_M2] | 0x40), TRUE); // set VRST bit
        pviwrite(PVI_SPRITECOLLIDE_M3, (UBYTE) (memory[pvibase + PVI_SPRITECOLLIDE_M3] | 0x40), TRUE); // set VRST bit
        pviwrite(PVI_SPRITECOLLIDE_M4, (UBYTE) (memory[pvibase + PVI_SPRITECOLLIDE_M4] | 0x40), TRUE); // set VRST bit
        if (machines[machine].pvis >= 3)
        {   pvibase += 0x100; // 3rd PVI
            pviwrite(PVI_SPRITECOLLIDE   , (UBYTE) (memory[pvibase + PVI_SPRITECOLLIDE   ] | 0x40), TRUE); // set VRST bit
            pviwrite(PVI_SPRITECOLLIDE_M2, (UBYTE) (memory[pvibase + PVI_SPRITECOLLIDE_M2] | 0x40), TRUE); // set VRST bit
            pviwrite(PVI_SPRITECOLLIDE_M3, (UBYTE) (memory[pvibase + PVI_SPRITECOLLIDE_M3] | 0x40), TRUE); // set VRST bit
            pviwrite(PVI_SPRITECOLLIDE_M4, (UBYTE) (memory[pvibase + PVI_SPRITECOLLIDE_M4] | 0x40), TRUE); // set VRST bit
        }
        pvibase = machines[machine].pvibase; // 1st PVI
    }

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
    if (log_interrupts)
    {   if (psu & PSU_II)
        {   zprintf(TEXTPEN_VERBOSE, "Pended vblank interrupt at rastline %d.\n", cpuy);
        } else
        {   zprintf(TEXTPEN_VERBOSE, "Generated vblank interrupt at rastline %d.\n", cpuy);
    }   }
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
    switch (machine)
    {
    case ZACCARIA:
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:
            localcolour = from_astrowars_spr[localcolour];
        acase MEMMAP_GALAXIA:
        case MEMMAP_LASERBATTLE:
        case MEMMAP_LAZARIAN:
            localcolour = from_galaxia_spr[localcolour];
        }
    acase MALZAK:
        localcolour = from_malzak_spr[localcolour];
    }

    return localcolour;
}

EXPORT void view_ram_pvi(void)
{   // We deliberately don't use pviread()

    zprintf(TEXTPEN_VIEW, LLL(MSG_PVIAT, "PVI at $%04X:\n"), pvibase);
    zprintf
    (   TEXTPEN_VIEW,
        "$%04X:                                            %02X %02X\n",
        pvibase + 0x0E,
        memory[pvibase + 0x0E],
        memory[pvibase + 0x0F]
    );
    zprintf
    (   TEXTPEN_VIEW,
        "$%04X:                                            %02X %02X\n",
        pvibase + 0x1E,
        memory[pvibase + 0x1E],
        memory[pvibase + 0x1F]
    );
    zprintf
    (   TEXTPEN_VIEW,
        "$%04X:                                            %02X %02X\n",
        pvibase + 0x4E,
        memory[pvibase + 0x4E],
        memory[pvibase + 0x4F]
    );
    dumprow(pvibase + 0x50);
    dumprow(pvibase + 0x60);
    zprintf
    (   TEXTPEN_VIEW,
        "$%04X:                                            %02X\n",
        pvibase + 0xAD,
        memory[pvibase + 0xAD]
    );
}

EXPORT void newpvi_drawhelpgrid(void)
{   int x, y;

    // assert(machine == INTERTON || machine == ELEKTOR);

    switch (drawmode)
    {
    case 1:
        for (y = 20; y <= 220; y += 20)
        {   for (x = 32; x <= 160; x++)
            {   changerelpixel(x, y     , GREY1);
                if (y == 220)
                {   continue;
                }
                changerelpixel(x, y +  2, GREY1);
                changerelpixel(x, y + 11, GREY1);
        }   }
        for (x = 32; x <= 160; x += 8)
        {   for (y = 20; y <= 219; y++)
            {   changerelpixel(x, y     , GREY1);
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
            for (x =   0; x <=  10; x++)
            {   for (y = 0; y <= 241; y++)
                {   changerelpixel(x, y, GREY4);
            }   }
            for (x = 175; x <= 177; x++)
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
            for (x =   0; x <=  10; x++)
            {   for (y = 0; y <= 268; y++)
                {   changerelpixel(x, y, GREY4);
            }   }
            for (x = 175; x <= 177; x++)
            {   for (y = 0; y <= 268; y++)
                {   changerelpixel(x, y, GREY4);
        }   }   }
    acase 2:
        for (y = 0; y < n3 - n1; y++)
        {   for (x = 0; x < ACTIVEX; x++)
            {   if
                (   (colltable[y][x] & 0x30) == 0x30
                 || (colltable[y][x] & 0x50) == 0x50
                 || (colltable[y][x] & 0x90) == 0x90
                 || (colltable[y][x] & 0x60) == 0x60
                 || (colltable[y][x] & 0xA0) == 0xA0
                 || (colltable[y][x] & 0xC0) == 0xC0
                ) // two or more sprites
                {   if (colltable[y][x] & 8) // and grid
                    {   changerelpixel(x, y, WHITE);
                    } else                   // but not grid
                    {   changerelpixel(x, y, BLUE);
                }   }
                elif
                (   (colltable[y][x] & 0xF0) == 0x10
                 || (colltable[y][x] & 0xF0) == 0x20
                 || (colltable[y][x] & 0xF0) == 0x40
                 || (colltable[y][x] & 0xF0) == 0x80
                ) // one sprite
                {   if (colltable[y][x] & 8) // and grid
                    {   changerelpixel(x, y, CYAN);
                    } else                   // but not grid
                    {   changerelpixel(x, y, DARKBLUE);
                }   }
                else // no sprites
                {   if (colltable[y][x] & 8) // grid
                    {   changerelpixel(x, y, DARKGREEN);
                    } else                   // not grid
                    {   changerelpixel(x, y, BLACK);
        }   }   }   }
    acase 3:
    case 4:
        if (usemargins)
        {   for (y = 0; y < n3; y++)
            {   for (x = 0; x < 227; x++)
                {   changeabspixel(x, y, dmascreen[y][x]);
        }   }   }
        else
        {   for (y = absymin; y <= absymax; y++)
            {   for (x = 60; x <= 223; x++)
                {   changeabspixel(x, y, dmascreen[y][x]);
}   }   }   }   }

MODULE void unvblank(void)
{   pviwrite(PVI_SPRITECOLLIDE   , 0, TRUE); // clear VRST and sprite-sprite collision bits
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

    if (machines[machine].coinop)
    {   return;
    }

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
        newsprite[whichsprite].startx = t + 1;
        newsprite[whichsprite].endx   = newsprite[whichsprite].startx + (8 * newsprite[whichsprite].size);

        switch (spritemode)
        {
        case SPRITEMODE_INVISIBLE:
            newsprite[whichsprite].imagery = 0;
        acase SPRITEMODE_NUMBERED:
            if (newsprite[whichsprite].dup)
            {   newsprite[whichsprite].imagery = pvi_sprnumbers[1][whichsprite][newsprite[whichsprite].majory];
            } else
            {   newsprite[whichsprite].imagery = pvi_sprnumbers[0][whichsprite][newsprite[whichsprite].majory];
            }
        acase SPRITEMODE_VISIBLE:
            newsprite[whichsprite].imagery = pviread(pvi_spritedata[whichsprite] + newsprite[whichsprite].majory);
            #ifdef DEBUGSPRITES
                getimgbits(newsprite[whichsprite].imagery);
                zprintf
                (   consolecolours[newsprite[whichsprite].colour],
                    "Sprite #%d. Raster %3d: #%s\n",
                    whichsprite,
                    cpuy,
                    imgbits
                );
            #endif
        }

        if (demultiplex != 0 && prevsprnum[whichsprite] < PREVSPRITES)
        {   if (pviy == newsprite[whichsprite].starty)
            {   prevsprite[multiframe][whichsprite][prevsprnum[whichsprite]].inuse  = TRUE;
                prevsprite[multiframe][whichsprite][prevsprnum[whichsprite]].leftx  = newsprite[whichsprite].startx; // for prevsprites, we don't support changing these every line
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
{   /* For usemargins, the entire routine could be:
    changepixel(x, y, colour); */

    if (x >= absxmin && x <= absxmax && y >= absymin && y <= absymax)
    {   changepixel(x - absxmin, y - absymin, colour);
}   }
EXPORT void changethisabspixel(int colour)
{   /* For usemargins, the entire routine could be:
    changepixel(cpux, cpuy, colour); */

    if (cpux >= absxmin && cpux <= absxmax && cpuy >= absymin && cpuy <= absymax)
    {   changepixel(cpux - absxmin, cpuy - absymin, colour);
}   }
EXPORT void changethisbgpixel(int colour)
{   /* For usemargins, the entire routine could be:
    changepixel(cpux, cpuy, colour); */

    if (cpux >= absxmin && cpux <= absxmax && cpuy >= absymin && cpuy <= absymax)
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
        checkstep();
        do_tape(); // for Elektor
        one_instruction();
        nextinst += (cycles_2650 - oldcycles) * ppc; // in pixels
        if (nextinst >= 227)
        {   nextinst -= cpl;
}   }   }

EXPORT void set_retuning(void)
{   // assert(machine == INTERTON || machine == ELEKTOR);

    // for Musical Games
    idealfreq_ntsc[0x18].hertz     = idealfreq_pal[0x18].hertz     = HZ_E4;
    idealfreq_ntsc[0x18].name      = idealfreq_pal[0x18].name      = "e4";
    idealfreq_ntsc[0x18].whichnote = idealfreq_pal[0x18].whichnote = NOTE_E4;
    idealfreq_ntsc[0x18].midi      = idealfreq_pal[0x18].midi      = MIDI_E4;
    notes[NOTE_DS4].ntscval = notes[NOTE_DS4].palval = 0x19;

    // for Musical Games
    idealfreq_ntsc[0x19].hertz     = idealfreq_pal[0x19].hertz     = HZ_DS4;
    idealfreq_ntsc[0x19].name      = idealfreq_pal[0x19].name      = "d#4";
    idealfreq_ntsc[0x19].whichnote = idealfreq_pal[0x19].whichnote = NOTE_DS4;
    idealfreq_ntsc[0x19].midi      = idealfreq_pal[0x19].midi      = MIDI_DS4;

    // for Musical Games
    idealfreq_ntsc[0x1B].hertz     = idealfreq_pal[0x1B].hertz     = HZ_D4;
    idealfreq_ntsc[0x1B].name      = idealfreq_pal[0x1B].name      = "d4";
    idealfreq_ntsc[0x1B].whichnote = idealfreq_pal[0x1B].whichnote = NOTE_D4;
    idealfreq_ntsc[0x1B].midi      = idealfreq_pal[0x1B].midi      = MIDI_D4;
    notes[NOTE_CS4].ntscval = notes[NOTE_CS4].palval = 0x1C;

    // for Surround
    idealfreq_ntsc[0x28].hertz     = idealfreq_pal[0x28].hertz     = HZ_FS3;
    idealfreq_ntsc[0x28].name      = idealfreq_pal[0x28].name      = "f#3";
    idealfreq_ntsc[0x28].whichnote = idealfreq_pal[0x28].whichnote = NOTE_FS3;
    idealfreq_ntsc[0x28].midi      = idealfreq_pal[0x28].midi      = MIDI_FS3;
}

#define KXLIMIT 15
#define KYLIMIT 29
EXPORT FLAG pvi_edit_screen(UWORD code)
{   int horizaddr,
        i;

    scrnaddr = 0x1F80 + (kx / 8);
    switch (ky % 3)
    {
    case 0:
        scrnaddr +=     ((ky / 3) * 4);
    acase 1:
    case  2:
        scrnaddr += 2 + ((ky / 3) * 4);
    }
    horizaddr = 0x1FA8 + (ky / 6);

    switch (code)
    {
    case  SCAN_UP:               if (shift()) ky =       0; elif (ky ==       0) ky = KYLIMIT; else ky--;
    acase SCAN_DOWN:             if (shift()) ky = KYLIMIT; elif (ky == KYLIMIT) ky =       0; else ky++;
    acase SCAN_LEFT:             if (shift()) kx =       0; elif (kx ==       0) kx = KXLIMIT; else kx--;
    acase SCAN_RIGHT:            if (shift()) kx = KXLIMIT; elif (kx == KXLIMIT) kx =       0; else kx++;
#ifdef WIN32
    acase SCAN_HOME:             kx =       0; if (shift()) ky =       0;
    acase SCAN_END:              kx = KXLIMIT; if (shift()) ky = KYLIMIT;
    acase SCAN_PGUP:             memory[scrnaddr]++; setstatus("Increment screen memory"); // overflow  is OK
    acase SCAN_PGDN:             memory[scrnaddr]--; setstatus("Decrement screen memory"); // underflow is OK
#endif
    acase SCAN_ESCAPE:           editscreen = FALSE;
                                 updatemenus();
                                 settitle();
                              // dogamename();
    acase SCAN_Q:
        memory[scrnaddr] &= ~(128 >> (kx % 8));
        setstatus("Turn off grey cell");
    acase SCAN_W:
        memory[scrnaddr] |=  (128 >> (kx % 8));
        setstatus("Turn on grey cell");
    acase SCAN_A:
        memory[horizaddr] &= 0x3F;
        setstatus("Set width of all bars in pink area to 1");
    acase SCAN_S:
        memory[horizaddr] &= 0x3F;
        memory[horizaddr] |= 0x40;
        setstatus("Set width of all bars in pink area to 2");
    acase SCAN_D:
        memory[horizaddr] |= 0xC0;
        setstatus("Set width of all bars in pink area to 4");
    acase SCAN_Z:
        memory[horizaddr] &= ~(1 << (ky % 6));
        setstatus("Set width of all bars in orange area to 1/2/4");
    acase SCAN_X:
        memory[horizaddr] |=  (1 << (ky % 6));
        setstatus("Set width of all bars in orange area to 8");
    acase SCAN_SPACEBAR:
        if (shift())
        {   for (i = 0x1F80; i <= 0x1FAC; i++)
            {   memory[i] = 0xFF;
            }
            setstatus("Fill screen");
        } else
        {   memory[scrnaddr ] |=  (128 >> (kx % 8));
            memory[horizaddr] |=  (  1 << (ky % 6));
            if (ky % 3 == 1)
            {   memory[horizaddr] |=  (  1 << ((ky + 1) % 6));
            } elif (ky % 3 == 2)
            {   memory[horizaddr] |=  (  1 << ((ky - 1) % 6));
            }
            setstatus("Turn on grey cell and set its width to 8");
        }
    acase SCAN_BACKSPACE:
    case SCAN_DEL:
        if (shift())
        {   for (i = 0x1F80; i <= 0x1FAC; i++)
            {   memory[i] = 0;
            }
            setstatus("Clear screen");
        } else
        {   memory[scrnaddr ] &= ~(128 >> (kx % 8));
            setstatus("Clear grey cell");
        }
     // memory[horizaddr] &= ~(  1 << (ky % 6));
     // memory[horizaddr] &= 0x3F;
    acase SCAN_A0:
    case SCAN_N0:
        if (shift())
        {   for (i = 0; i < 40; i++)
            {   memory[0x1F80 + i] = 0;
            }
            for (i = 0; i < 5; i++)
            {   memory[0x1FA8 + i] = 0;
            }
            setstatus("Turn off all bars (& set their width to 1)");
        } else
        {   for (i = 0; i < 8; i++)
            {   memory[0x1F80 + ((ky / 6) * 8) + i] = 0;
            }
            memory[horizaddr] = 0;
            setstatus("Turn off all bars in pink area (& set their width to 1)");
        }
    acase SCAN_A1:
    case SCAN_N1:
        if (shift())
        {   for (i = 0; i < 40; i++)
            {   memory[0x1F80 + i] = 0xFF;
            }
            for (i = 0; i < 5; i++)
            {   memory[0x1FA8 + i] = 0;
            }
            setstatus("Turn on all bars & set their width to 1");
        } else
        {   for (i = 0; i < 8; i++)
            {   memory[0x1F80 + ((ky / 6) * 8) + i] = 0xFF;
            }
            memory[horizaddr] = 0;
            setstatus("Turn on all bars in pink area & set their width to 1");
        }
    acase SCAN_A2:
    case SCAN_N2:
        if (shift())
        {   for (i = 0; i < 40; i++)
            {   memory[0x1F80 + i] = 0xFF;
            }
            for (i = 0; i < 5; i++)
            {   memory[0x1FA8 + i] = 0x40;
            }
            setstatus("Turn on all bars & set their width to 2");
        } else
        {   for (i = 0; i < 8; i++)
            {   memory[0x1F80 + ((ky / 6) * 8) + i] = 0xFF;
            }
            memory[horizaddr] = 0x40;
            setstatus("Turn on all bars in pink area & set their width to 2");
        }
    acase SCAN_A4:
    case SCAN_N4:
        if (shift())
        {   for (i = 0; i < 40; i++)
            {   memory[0x1F80 + i] = 0xFF;
            }
            for (i = 0; i < 5; i++)
            {   memory[0x1FA8 + i] = 0xC0;
            }
            setstatus("Turn on all bars & set their width to 4");
        } else
        {   for (i = 0; i < 8; i++)
            {   memory[0x1F80 + ((ky / 6) * 8) + i] = 0xFF;
            }
            memory[horizaddr] = 0xC0;
            setstatus("Turn on all bars in pink area & set their width to 4");
        }
    acase SCAN_A8:
    case SCAN_N8:
        if (shift())
        {   for (i = 0; i < 40; i++)
            {   memory[0x1F80 + i] = 0xFF;
            }
            for (i = 0; i < 5; i++)
            {   memory[0x1FA8 + i] = 0xFF;
            }
            setstatus("Turn on all bars & set their width to 8");
        } else
        {   for (i = 0; i < 8; i++)
            {   memory[0x1F80 + ((ky / 6) * 8) + i] = 0xFF;
            }
            memory[horizaddr] = 0xFF;
            setstatus("Turn on all bars in pink area & set their width to 8 (fill screen)");
        }
    adefault:
        return FALSE;
    }

    return TRUE; // return code is whether we ate the keystroke
}

EXPORT void ie_stringchar(UBYTE data, int address)
{   FAST TEXT digit[8];

    // assert(machine == INTERTON || machine == ELEKTOR);

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
    case  IE_CONSOLE:                 sprintf(stringchar, " [%%%c,%c,------]"           , digit[0], digit[1]);
    acase IE_NOISE:                   sprintf(stringchar, " [%%%c%c,%c,%c,%c,%c,--]"    , digit[0], digit[1], digit[2], digit[3], digit[4], digit[5]);
    acase IE_P1LEFTKEYS:
    case  IE_P1MIDDLEKEYS:
    case  IE_P1RIGHTKEYS:
    case  IE_P2LEFTKEYS:
    case  IE_P2MIDDLEKEYS:
    case  IE_P2RIGHTKEYS:             sprintf(stringchar, " [%%%c,%c,%c,%c,----]"       , digit[0], digit[1], digit[2], digit[3]);
    acase 0x1F00 + PVI_BGCOLLIDE:     sprintf(stringchar, " [%%%c,%c,%c,%c,%c,%c,%c,%c]", digit[0], digit[1], digit[2], digit[3], digit[4], digit[5], digit[6], digit[7]);
    acase 0x1F00 + PVI_SPRITECOLLIDE: sprintf(stringchar, " [%%-,%c,%c,%c,%c,%c,%c,%c]" ,           digit[1], digit[2], digit[3], digit[4], digit[5], digit[6], digit[7]);
    acase 0x1F00 + PVI_BGCOLOUR:      sprintf(stringchar, " [%%-,%c%c%c,%c,%c%c%c]"     ,           digit[1], digit[2], digit[3], digit[4], digit[5], digit[6], digit[7]);
    acase 0x1F00 + PVI_SCORECTRL:     sprintf(stringchar, " [%%------,%c,%c]"           ,                                                             digit[6], digit[7]);
    acase 0x1F00 + PVI_SCORELT:
    case  0x1F00 + PVI_SCORERT:       sprintf(stringchar, " [%%%c%c%c%c,%c%c%c%c]"      , digit[0], digit[1], digit[2], digit[3], digit[4], digit[5], digit[6], digit[7]);
    acase 0x1F00 + PVI_SPR01COLOURS:
    case  0x1F00 + PVI_SPR23COLOURS:  sprintf(stringchar, " [%%--,%c%c%c,%c%c%c]"       ,                     digit[2], digit[3], digit[4], digit[5], digit[6], digit[7]);
    acase 0x1F00 + PVI_SIZES:         sprintf(stringchar, " [%%%c%c,%c%c,%c%c,%c%c]"    , digit[0], digit[1], digit[2], digit[3], digit[4], digit[5], digit[6], digit[7]);
    acase 0x1F00 + PVI_PITCH:
    case  0x1F00 + PVI_P1PADDLE:
    case  0x1F00 + PVI_P2PADDLE:
    case  0x1F00 + PVI_SPRITE0AX:
    case  0x1F00 + PVI_SPRITE0AY:
    case  0x1F00 + PVI_SPRITE0BX:
    case  0x1F00 + PVI_SPRITE0BY:
    case  0x1F00 + PVI_SPRITE1AX:
    case  0x1F00 + PVI_SPRITE1AY:
    case  0x1F00 + PVI_SPRITE1BX:
    case  0x1F00 + PVI_SPRITE1BY:
    case  0x1F00 + PVI_SPRITE2AX:
    case  0x1F00 + PVI_SPRITE2AY:
    case  0x1F00 + PVI_SPRITE2BX:
    case  0x1F00 + PVI_SPRITE2BY:
    case  0x1F00 + PVI_SPRITE3AX:
    case  0x1F00 + PVI_SPRITE3AY:
    case  0x1F00 + PVI_SPRITE3BX:
    case  0x1F00 + PVI_SPRITE3BY:     sprintf(stringchar, " [%d]"                       , data);
    adefault:
        if (machine == INTERTON)
        {                                     stringchar[0] = EOS;
        } else
        {   // assert(machine == ELEKTOR);
                                      sprintf(stringchar, " ['%c']"                     , guestchar(data));
}   }   }

#ifdef DEBUGSPRITES
MODULE void getimgbits(UBYTE data)
{   int i;

    for (i = 0; i < 8; i++)
    {   if (data & (0x80 >> i))
        {   imgbits[i] = '#';
        } else
        {   imgbits[i] = '.';
    }   }
    imgbits[8] = EOS;
}
#endif

MODULE void run_cpu(int until)
{   // This is a quicker equivalent to repeatedly incrementing cpux and calling do_cpu().

    cpux = nextinst;
    while (cpux < until)
    {   oldcycles = cycles_2650;
        checkstep();
        do_tape(); // for Elektor
        one_instruction();
        nextinst += (cycles_2650 - oldcycles) * ppc; // in pixels
        cpux = nextinst;
    }
    if (nextinst >= 227)
    {   nextinst -= cpl;
}   }
