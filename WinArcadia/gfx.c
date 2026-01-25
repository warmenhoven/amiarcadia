// INCLUDES---------------------------------------------------------------

#ifdef WIN32
    #include "ibm.h"
    typedef unsigned char bool;
    #include "RA_Interface.h"
#endif

#include <stdio.h>  // FILE*
#include <string.h> // strcpy()

#ifdef AMIGA
    #include <intuition/intuition.h> // struct Window*
    #include <proto/locale.h>        // GetCatalogStr()
#endif

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#ifdef WIN32
    #define EXEC_TYPES_H
#endif
#include "aa_strings.h"
#ifdef AMIGA
    #include "amiga.h"
#endif

// DEFINES----------------------------------------------------------------

// #define CHECKDRAWS
// whether changepixel() should bounds check the coordinates

// These are for ETI-640 (ie. BINBUG). See ETI Circuits #3, p. 32.
// Method A (ideal ETI-640)...
#define HPULSE_FIRST 642
#define HPULSE_LAST  701
// Method B (original unmodified ETI-640)...
// #define HPULSE_FIRST 672 // 642..671 is considered front porch (30 pixels)
// #define HPULSE_LAST  671 // 672..701 is considered back  porch (30 pixels)
// Method C (ETI-640 with improved sync modification)...
// #define HPULSE_FIRST 702
// #define HPULSE_LAST  761

// not sure what the correct values for these are
#define PONG_CBSTART_PAL  7
#define PONG_CBSTART_NTSC 7

#ifdef WIN32
#define SETPONGPIXEL(x, y, colour) screen[ (x) * 2     ][y] = \
                                   screen[((x) * 2) + 1][y] = colour; \
                                   fgtable[y][((x) * 2)    ] = \
                                   fgtable[y][((x) * 2) + 1] = 1
#define SETPIXEL(x, y, colour)     screen[x][y] = colour; \
                                   fgtable[y][x] = 1
#endif
#ifdef AMIGA
#define SETPONGPIXEL(x, y, colour) screen[ (x) * 2     ][y] = \
                                   screen[((x) * 2) + 1][y] = colour
#define SETPIXEL(x, y, colour)     screen[x][y] = colour
#endif

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT UBYTE                digitleds[DIGITLEDS],
                            sx[2], sy[2],
                            vdu_fgc, vdu_bgc,
                            vdu[MAX_VDUCOLUMNS][MAX_VDUROWS];
EXPORT TEXT                 coords[128 + 1];
EXPORT STRPTR               colourname[8],
                            colournames[GUESTCOLOURS + 1];
EXPORT int                  absxmin, absxmax,
                            absymin, absymax,
                            colourset = PURECOLOURS,
                            fastsize,
                            hidetop,
                            kx           = 0,
                            ky           = 0,
                            minidestx, minidesty,
                            miniwidth, miniheight,
                            rotating,
                            scrnaddr     = 0,
                            speakoffset,
                            widthheight;
EXPORT RECT                 therect;
#ifdef WIN32
    EXPORT UBYTE            sky          = (UBYTE) -1;
    EXPORT int              bezelwidth,
                            bezelheight;
#endif

EXPORT const ULONG defpencolours[COLOURSETS][GUESTCOLOURS] = {
{ // amber: red = ((red*2)+green)/3, green = ((red*2)+green)/6, blue = $00
    0x00FF8000, // white  (UVI #0, PVI #0) 3/3
    0x00E27100, // yellow (UVI #1, PVI #1) 3/3
    0x00683400, // purple (UVI #4, PVI #2) 2/3
    0x004C2600, // red    (UVI #5, PVI #3) 2/3
    0x00B25900, // cyan   (UVI #2, PVI #4) 1/3
    0x00844200, // green  (UVI #3, PVI #5) 1/3
    0x004A2500, // blue   (UVI #6, PVI #6) 0/3
    0x00000000, // black  (UVI #7, PVI #7) 0/3
    0x00BB5E00, // grey #1                 3/6
    0x00A65300, // dark yellow             3/6
    0x004C2600, // dark purple             2/6
    0x002D1700, // dark red                2/6
    0x00773C00, // dark cyan               1/6
    0x006E3800, // dark green              1/6
    0x001D0F00, // dark blue               0/6
    0x00000000, // dark black              0/6
    0x00251300, // grey #2 (grey)          1/7
    0x00492500, // grey #3 (grey)          2/7
    0x006D3700, // grey #4 (grey)          3/7
    0x00924900, // grey #5 (grey)          4/7
    0x00B65B00, // grey #6 (grey)          5/7
    0x00DB6E00, // grey #7 (grey)          6/7
    0x009D4F00, // orange                  5/6
    0x009D4F00, // pink                    5/6
},
{ // green: red = $00, green = green, blue = $00
    0x0000FF00, // white  (UVI #0, PVI #0)
    0x0000E200, // yellow (UVI #1, PVI #1)
    0x00006800, // purple (UVI #4, PVI #2)
    0x00004C00, // red    (UVI #5, PVI #3)
    0x0000B200, // cyan   (UVI #2, PVI #4)
    0x00008400, // green  (UVI #3, PVI #5)
    0x00004A00, // blue   (UVI #6, PVI #6)
    0x00000000, // black  (UVI #7, PVI #7)
    0x0000BB00, // grey #1
    0x0000A600, // dark yellow
    0x00004C00, // dark purple
    0x00002D00, // dark red
    0x00007700, // dark cyan
    0x00006E00, // dark green
    0x00001D00, // dark blue
    0x00000000, // dark black
    0x00002500, // grey #2 (grey)
    0x00004900, // grey #3 (grey)
    0x00006D00, // grey #4 (grey)
    0x00009200, // grey #5 (grey)
    0x0000B600, // grey #6 (grey)
    0x0000DB00, // grey #7 (grey)
    0x00009D00, // orange
    0x00009D00, // pink
},
{ // greyscale
    0x00FFFFFF, // white       $FF = 255
    0x00E2E2E2, // yellow      $E2 = 226
    0x00686868, // purple      $68 = 104
    0x004C4C4C, // red         $4C =  76 (same as dark purple!)
    0x00B2B2B2, // cyan        $B2 = 178
    0x00848484, // green       $84 = 132
    0x004A4A4A, // blue        $4A =  74
    0x00000000, // black       $00 =   0
    0x00BBBBBB, // dark white  $BB = 187 (grey #1)
    0x00A6A6A6, // dark yellow $A6 = 166 (brown)
    0x004C4C4C, // dark purple $4C =  76 (same as red!)
    0x002D2D2D, // dark red    $2D =  45
    0x00777777, // dark cyan   $77 = 119
    0x006E6E6E, // dark green  $6E = 110
    0x001D1D1D, // dark blue   $1A =  29
    0x00000000, // dark black  $00 =   0 (black)
    0x00252525, // grey #2
    0x00494949, // grey #3
    0x006D6D6D, // grey #4
    0x00929292, // grey #5
    0x00B6B6B6, // grey #6
    0x00DBDBDB, // grey #7
    0x009D9D9D, // orange      $9D = 157
    0x009D9D9D, // pink        $9D = 157
},
{ // pure colours
    0x00FFFFFF, // white  (UVI #0, PVI #0)
    0x00FFFF00, // yellow (UVI #1, PVI #1)
    0x00FF00FF, // purple (UVI #4, PVI #2)
    0x00FF0000, // red    (UVI #5, PVI #3)
    0x0000FFFF, // cyan   (UVI #2, PVI #4)
    0x0000DD00, // green  (UVI #3, PVI #5)
    0x004444FF, // blue   (UVI #6, PVI #6)
    0x00000000, // black  (UVI #7, PVI #7)
    0x00BBBBBB, // grey #1
    0x00BBBB00, // dark yellow
    0x00BB00BB, // dark purple
    0x00990000, // dark red
    0x0000AAAA, // dark cyan
    0x0000AA00, // dark green
    0x000000AA, // dark blue
    0x00000000, // dark black
    0x00252525, // grey #2 (grey)
    0x00494949, // grey #3 (grey)
    0x006D6D6D, // grey #4 (grey)
    0x00929292, // grey #5 (grey)
    0x00B6B6B6, // grey #6 (grey)
    0x00DBDBDB, // grey #7 (grey)
    0x00FF8000, // orange
    0x00FF8080, // pink
},
{ // PVI colours
    0x00FFFFFF, // white  (UVI #0, PVI #0) 255 255 255
    0x00B4B408, // yellow (UVI #1, PVI #1) 180 180   8
    0x00B408B4, // purple (UVI #4, PVI #2) 180   8 180
    0x00F11C00, // red    (UVI #5, PVI #3) 241  28   0
    0x0008BABA, // cyan   (UVI #2, PVI #4)   8 186 186
    0x0014B414, // green  (UVI #3, PVI #5)  20 180  20
    0x003F4EFB, // blue   (UVI #6, PVI #6)  63  78 251
    0x00000000, // black  (UVI #7, PVI #7)   0   0   0
    0x00B4B4AC, // grey #1                 180 180 172
    0x00AAAD52, // dark yellow             170 173  82
    0x00A838A4, // dark purple             168  56 164
    0x00822E24, // dark red                130  46  36
    0x00409898, // dark cyan                64 152 152
    0x00019000, // dark green                1 144   0
    0x002E2E8C, // dark blue                30  30 140
    0x00000000, // dark black                0   0   0
    0x00252525, // grey #2 (grey)
    0x00494949, // grey #3 (grey)
    0x006D6D6D, // grey #4 (grey)
    0x00929292, // grey #5 (grey)
    0x00B6B6B6, // grey #6 (grey)
    0x00DBDBDB, // grey #7 (grey)
    0x00FF8000, // orange
    0x00FF8080, // pink
},
{ // UVI colours
    0x00FFFCFF, // white  (UVI #0, PVI #0)
    0x00FFFE6C, // yellow (UVI #1, PVI #1)
    0x00D82CD8, // purple (UVI #4, PVI #2)
    0x00FF0000, // red    (UVI #5, PVI #3)
    0x005DFDFF, // cyan   (UVI #2, PVI #4)
    0x0047F43F, // green  (UVI #3, PVI #5)
    0x000303A9, // blue   (UVI #6, PVI #6)
    0x00000000, // black  (UVI #7, PVI #7)
    0x00BBBBBB, // grey #1
    0x00BBBB00, // dark yellow
    0x00BB00BB, // dark purple
    0x00990000, // dark red
    0x0000AAAA, // dark cyan
    0x0000AA00, // dark green
    0x00000077, // dark blue
    0x00000000, // dark black
    0x00252525, // grey #2 (grey)
    0x00494949, // grey #3 (grey)
    0x006D6D6D, // grey #4 (grey)
    0x00929292, // grey #5 (grey)
    0x00B6B6B6, // grey #6 (grey)
    0x00DBDBDB, // grey #7 (grey)
    0x00FF8000, // orange
    0x00FF8080, // pink
} };

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       int                  ambient,
                                  cpux, cpuy,
                                  drawcorners,
                                  drawunlit,
                                  echoconsole,
                                  editscreen,
                                  enhancestars,
                                  hoffsets[26],
                                  hostvolume,
                                  inverse,
                                  machine,
                                  memmap,
                                  n1, n2, n3, n4,
                                  papertapemode[2],
                                  papertapeprotect[2],
                                  phunsy_biosver,
                                  pipbug_biosver,
                                  pipbug_charwidth,
                                  pipbug_charheight,
                                  pipbug_charwidth2,
                                  pipbug_charheight2,
                                  pipbug_vdu,
                                  ponglt, pongrt,
                                  selbst_biosver,
                                  showdebugger[2],
                                  showleds,
                                  traceorstep,
                                  usemargins,
                                  usespeech,
                                  vdu_columns,
                                  vdu_rows_total,
                                  vdu_rows_visible,
                                  vdu_scrolly,
                                  vdu_x, vdu_y,
                                  wheremouse,
                                  whichgame;
IMPORT       FLAG                 ignore_cout,
                                  inframe,
                                  paperreaderenabled,
                                  paperpunchenabled,
                                  twin_escaped;
IMPORT       SBYTE                galaxia_scrolly;
IMPORT       UBYTE                blank,
                                  memory[32768],
                                  newkeys[KEYS],
                                  oldkeys[KEYS],
                                  papertapebyte[2],
                                  PapertapeBuffer[2][PAPERTAPEMAX],
                                  psu;
IMPORT       UWORD                console[4],
                                  keypads[2][NUMKEYS],
                                  ras[8];
IMPORT       ULONG                arcadia_bigctrls,
                                  arcadia_viewcontrolsas,
                                  papertapelength[2],
                                  papertapewhere[2],
                                  pong_machine,
                                  region,
                                  si50_bigctrls,
                                  sound,
                                  swapped;
IMPORT       ULONG*               display;
IMPORT       ASCREEN              screen[BOXWIDTH][BOXHEIGHT],
                                  miniscreen[MINIWIDTH][MINIHEIGHT];
IMPORT       struct KeyNameStruct keyname[SCANCODES];
IMPORT       struct MachineStruct machines[MACHINES];
IMPORT       struct MemMapInfoStruct memmapinfo[MEMMAPS];
IMPORT const int                  gridline[220];
#ifdef VERBOSE
    IMPORT   TEXT                 asciiname_long[259][9 + 1];
    IMPORT   ULONG                cycles_2650;
#endif
#ifdef AMIGA
    IMPORT   int                  scanlines;
    IMPORT   UBYTE                bytepens[GUESTCOLOURS];
    IMPORT   UWORD                wordpens[GUESTCOLOURS];
    IMPORT   UBYTE               *byteptr[MAXBOXHEIGHT],
                                 *canvasbyteptr[CANVASES][CANVASHEIGHT],
                                 *canvasdisplay[CANVASES];
    IMPORT   UWORD*               wordptr[MAXBOXHEIGHT];
    IMPORT   ULONG*               longptr[MAXBOXHEIGHT];
    IMPORT   struct Catalog*      CatalogPtr;
    IMPORT   struct Window*       SubWindowPtr[SUBWINDOWS];
#endif
#ifdef WIN32
    IMPORT   int                  CatalogPtr; // APTR doesn't work
    IMPORT   UBYTE                fgtable[BOXHEIGHT][BOXWIDTH];
    IMPORT   ULONG                pencolours[COLOURSETS][PENS];
    IMPORT   ULONG               *canvasdisplay[CANVASES],
                                 *pixelulong,
                                 *stars;
    IMPORT   HWND                 SubWindowPtr[SUBWINDOWS];
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE       UBYTE                blankscreen_2621[312][227],
                                  blankscreen_2622[262][227];

// MODULE ARRAYS----------------------------------------------------------

MODULE const int hbc_normal[2][49] = {
{ // NTSC
BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK, //  0..  9 active area (unused)
ORANGE  , ORANGE  , ORANGE  , ORANGE  , ORANGE  ,                                                // 10..14 front porch
CYAN    , CYAN    , CYAN    , CYAN    , CYAN    , CYAN    , CYAN    , CYAN    , CYAN    ,        // 15..31 horizontal retrace
CYAN    , CYAN    , CYAN    , CYAN    , CYAN    , CYAN    , CYAN    , CYAN    ,
ORANGE  , ORANGE  ,                                                                              // 32..33 front of back porch
WHITE   , WHITE   , WHITE   , WHITE   , WHITE   , WHITE   , WHITE   , WHITE   , WHITE   ,        // 34..42 colour burst (middle of back porch)
ORANGE  , ORANGE  , ORANGE  , ORANGE  , ORANGE  , ORANGE  ,                                      // 43..48 back of back porch
},
{ // PAL
BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   ,                                      //  0.. 5 active area (unused)
ORANGE  , ORANGE  , ORANGE  , ORANGE  , ORANGE  ,                                                //  6..10 front porch
CYAN    , CYAN    , CYAN    , CYAN    , CYAN    , CYAN    , CYAN    , CYAN    , CYAN    ,        // 11..27 horizontal retrace
CYAN    , CYAN    , CYAN    , CYAN    , CYAN    , CYAN    , CYAN    , CYAN    ,
ORANGE  , ORANGE  , ORANGE  ,                                                                    // 28..30 front of back porch
WHITE   , WHITE   , WHITE   , WHITE   , WHITE   , WHITE   , WHITE   , WHITE   , WHITE   ,        // 31..39 colour burst (middle of back porch)
ORANGE  , ORANGE  , ORANGE  , ORANGE  , ORANGE  , ORANGE  , ORANGE  , ORANGE  , ORANGE  ,        // 40..48 back of back porch
} }, hbc_vrst[2][49] = {
{ // NTSC
BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK, //  0.. 9 active area (unused)
RED     , RED     , RED     , RED     , RED     ,                                                // 10..14 front porch
BLUE    , BLUE    , BLUE    , BLUE    , BLUE    , BLUE    , BLUE    , BLUE    , BLUE    ,        // 15..31 horizontal retrace
BLUE    , BLUE    , BLUE    , BLUE    , BLUE    , BLUE    , BLUE    , BLUE    ,
RED     , RED     ,                                                                              // 32..33 front of back porch
GREY1   , GREY1   , GREY1   , GREY1   , GREY1   , GREY1   , GREY1   , GREY1   , GREY1   ,        // 34..42 colour burst (middle of back porch)
RED     , RED     , RED     , RED     , RED     , RED     ,                                      // 43..48 back of back porch
},
{ // PAL
BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   ,                                      //  0.. 5 active area (unused)
RED     , RED     , RED     , RED     , RED     ,                                                //  6..10 front porch
BLUE    , BLUE    , BLUE    , BLUE    , BLUE    , BLUE    , BLUE    , BLUE    , BLUE    ,        // 11..27 horizontal retrace
BLUE    , BLUE    , BLUE    , BLUE    , BLUE    , BLUE    , BLUE    , BLUE    ,
RED     , RED     , RED     ,                                                                    // 28..30 front of back porch
GREY1   , GREY1   , GREY1   , GREY1   , GREY1   , GREY1   , GREY1   , GREY1   , GREY1   ,        // 31..39 colour burst (middle of back porch)
RED     , RED     , RED     , RED     , RED     , RED     , RED     , RED     , RED     ,        // 40..48 back of back porch
} }, hbc_vs[2][49] = {
{ // NTSC
BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK, //  0.. 9 active area (unused)
DARKRED , DARKRED , DARKRED , DARKRED , DARKRED ,                                                // 10..14 horizontal front porch
DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE,        // 15..31 horizontal retrace
DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE,
DARKRED , DARKRED ,                                                                              // 32..33 front of back porch
BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   ,        // 34..42 colour burst (middle of back porch)
DARKRED , DARKRED , DARKRED , DARKRED , DARKRED , DARKRED ,                                      // 43..48 back of back porch
},
{ // PAL
BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   ,                                      //  0.. 5 active area (unused)
DARKRED , DARKRED , DARKRED , DARKRED , DARKRED ,                                                //  6..10 front porch
DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE,        // 11..27 horizontal retrace
DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE, DARKBLUE,
DARKRED , DARKRED , DARKRED ,                                                                    // 28..30 front of back porch
BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   , BLACK   ,        // 31..39 colour burst (middle of back porch)
DARKRED , DARKRED , DARKRED , DARKRED , DARKRED , DARKRED , DARKRED , DARKRED , DARKRED ,        // 40..48 back of back porch
} };

// FUNCTION POINTERS------------------------------------------------------

EXPORT void (* drawpixel  ) (int x, int y, int colour);
EXPORT void (* drawbgpixel) (int x, int y, int colour);

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void drawcontrolspixel2(int x, int y, int colour);
MODULE void draw_line(int x1, int y1, int x2, int y2, FLAG erasing);
MODULE void scroll_vdu(void);
MODULE __inline void segment_drawpixel(int x, int y, int colour);
MODULE STRPTR suffix(int number);

// CODE-------------------------------------------------------------------

EXPORT void draw_guide_ray(FLAG erasing)
{   PERSIST int guideray_x,
                guideray_y = -1;

    if (erasing && guideray_y == -1)
    {   return;
    }

    if (!erasing)
    {   switch (machine)
        {
        case ARCADIA:
        case INTERTON:
        case ELEKTOR:
            if (traceorstep || (inframe && (cpux % 227 || cpuy % n3 ))) // because we don't want to annoy the gamers with it
            {   guideray_x = cpux % 227; // this can happen, eg. R F command
                guideray_y = cpuy % n3;  // this can happen, eg. R F command
            } else
            {   return;
            }
        acase BINBUG:
            if (traceorstep || (inframe && (cpux % 384 || cpuy % 312))) // because we don't want to annoy the gamers with it. This is no mistake
            {   guideray_x = cpux % 768;
                guideray_y = cpuy % 313;
            } else
            {   return;
            }
        acase CD2650:
            if (traceorstep || (inframe && (cpux % 904 || cpuy % 264))) // because we don't want to annoy the gamers with it
            {   guideray_x = cpux % 904;
                guideray_y = cpuy % 264;
            } else
            {   return;
            }
        acase PHUNSY:
            if (traceorstep || (inframe && (cpux % 384 || cpuy % 313))) // because we don't want to annoy the gamers with it
            {   guideray_x = (cpux % 640) + 128;
                guideray_y =  cpuy % 313;
            } else
            {   return;
            }
        acase MALZAK:
        case ZACCARIA:
            if (traceorstep || (cpuy >= 1 && cpuy < COINOP_BOXHEIGHT)) // because we don't want to annoy the gamers with it
            {   guideray_y = cpuy;
            } else
            {   return;
    }   }   }

    switch (machine)
    {
    case ARCADIA:
    case INTERTON:
    case ELEKTOR:
        draw_line(USG_XMARGIN + 89, USG_YMARGIN + 134, guideray_x                 , guideray_y, erasing);
    acase BINBUG:
        draw_line(             288,               128, guideray_x                 , guideray_y, erasing);
    acase CD2650:
        draw_line(             320,                96, guideray_x                 , guideray_y, erasing);
    acase PHUNSY:
        draw_line(       128 + 192,               128, guideray_x                 , guideray_y, erasing);
    acase MALZAK:
    case ZACCARIA:
        draw_line(               0,        guideray_y, machines[machine].width - 1, guideray_y, erasing);
    }

    if (erasing)
    {   // guideray_x = -1;
        guideray_y = -1;
}   }

MODULE void draw_line(int x1, int y1, int x2, int y2, FLAG erasing)
{   FAST int dx,      // deltas
             dy,
             dx2,     // scaled deltas
             dy2,
             err,     // the error term
             i,       // looping variable
             ix,      // increase rate on the x axis
             iy,      // increase rate on the y axis
             nowx, nowy;

    // difference between starting and ending points
    dx = x2 - x1;
    dy = y2 - y1;

    // calculate direction of the vector and store in ix and iy
    if (dx >= 0)
    {   ix =  1;
    } elif (dx < 0)
    {   dx = -dx;
        ix = -1;
    } else
    {   ix = 0;
    }
    if (dy >= 0)
    {   iy =  1;
    } elif (dy < 0)
    {   dy = -dy;
        iy = -1;
    } else
    {   iy = 0;
    }

    // scale deltas and store in dx2 and dy2
    dx2 = dx * 2;
    dy2 = dy * 2;

    nowx = x1;
    nowy = y1;
    if (dx > dy) // dx is the major axis
    {   // initialize the error term
        err = dy2 - dx;
        for (i = 0; i <= dx; i++)
        {   if (!(nowx < absxmin || nowx > absxmax || nowy < absymin || nowy > absymax))
            {   if (erasing)
                {   changeabspixel(nowx, nowy, (screen[nowx - absxmin][nowy - absymin] ==  0) ? 23 : (screen[nowx - absxmin][nowy - absymin] - 1));
                } else
                {   changeabspixel(nowx, nowy, (screen[nowx - absxmin][nowy - absymin] == 23) ?  0 : (screen[nowx - absxmin][nowy - absymin] + 1));
            }   }
            if (err >= 0)
            {   err -= dx2;
                nowy += iy;
            }
            err += dy2;
            nowx += ix;
    }   }
    else // dy is the major axis
    {   // initialize the error term
        err = dx2 - dy;
        for (i = 0; i <= dy; i++)
        {   if (!(nowx < absxmin || nowx > absxmax || nowy < absymin || nowy > absymax))
            {   if (erasing)
                {   changeabspixel(nowx, nowy, (screen[nowx - absxmin][nowy - absymin] ==  0) ? 23 : (screen[nowx - absxmin][nowy - absymin] - 1));
                } else
                {   changeabspixel(nowx, nowy, (screen[nowx - absxmin][nowy - absymin] == 23) ?  0 : (screen[nowx - absxmin][nowy - absymin] + 1));
            }   }
            if (err >= 0)
            {   err -= dy2;
                nowx += ix;
            }
            err += dx2;
            nowy += iy;
}   }   }

EXPORT void apply_scale2x(void)
{   FAST    int   b, d, f, h,
                  e, d1, d2, d3, d4,
                  newx, newy,
                  x, y;
#ifdef WIN32
    FAST    int   pp;
#endif
#ifdef AMIGA
    FAST    int   px, py;
#endif

    for (x = 0; x < machines[machine].width; x++)
    {   for (y = 0; y < machines[machine].height; y++)
        {   if (rotating)
            {   newx = y;
                newy = widthheight - x;
                b = screen[WEST(newx)][      newy ];
                d = screen[     newx ][SOUTH(newy)];
                e = screen[     newx ][      newy ];
                f = screen[     newx ][NORTH(newy)];
                h = screen[EAST(newx)][      newy ];
            } else
            {   newx = x;
                newy = y;
                b = screen[        x ][NORTH(   y)];
                d = screen[WEST(   x)][         y ];
                e = screen[        x ][         y ];
                f = screen[EAST(   x)][         y ];
                h = screen[        x ][SOUTH(   y)];
            }

            if (b != h && d != f)
            {   d1 = d == b ? d : e;
                d2 = b == f ? f : e;
                d3 = d == h ? d : e;
                d4 = h == f ? f : e;
            } else
            {   d1 = d2 = d3 = d4 = e;
            }

            switch (fastsize)
            {
            case 2: // 2*2
#ifdef WIN32
                pp = (x * 2) + (y * 4 * machines[machine].width);
                if (sky == d1) { COPYSTAR(pp                                        ); }
                else           { COPYDISP(pp                                    , d1); }
                if (sky == d2) { COPYSTAR(pp + 1                                    ); }
                else           { COPYDISP(pp + 1                                , d2); }
                if (sky == d3) { COPYSTAR(pp +     (machines[machine].width * 2)    ); }
                else           { COPYDISP(pp +     (machines[machine].width * 2), d3); }
                if (sky == d4) { COPYSTAR(pp + 1 + (machines[machine].width * 2)    ); }
                else           { COPYDISP(pp + 1 + (machines[machine].width * 2), d4); }
#endif
#ifdef AMIGA
                px = x * 2;
                py = y * 2;
                *(byteptr[py] + px    ) = bytepens[d1];
                *(byteptr[py] + px + 1) = bytepens[d2];
                if (!scanlines)
                {   *(byteptr[++py] + px    ) = bytepens[d3];
                    *(byteptr[  py] + px + 1) = bytepens[d4];
                }
#endif
            acase 3: // 4*2
#ifdef WIN32
                pp = (x * 4) + (y * 8 * machines[machine].width);
                if (sky == d1) { COPYWIDESTAR(pp                                        ); }
                else           { COPYWIDEDISP(pp                                    , d1); }
                if (sky == d2) { COPYWIDESTAR(pp + 2                                    ); }
                else           { COPYWIDEDISP(pp + 2                                , d2); }
                if (sky == d3) { COPYWIDESTAR(pp +     (machines[machine].width * 4)    ); }
                else           { COPYWIDEDISP(pp +     (machines[machine].width * 4), d3); }
                if (sky == d4) { COPYWIDESTAR(pp + 2 + (machines[machine].width * 4)    ); }
                else           { COPYWIDEDISP(pp + 2 + (machines[machine].width * 4), d4); }
#endif
#ifdef AMIGA
                px = x * 2;
                py = y * 2;
                *(wordptr[py] + px    ) = wordpens[d1];
                *(wordptr[py] + px + 1) = wordpens[d2];
                if (!scanlines)
                {   *(wordptr[++py] + px    ) = wordpens[d3];
                    *(wordptr[  py] + px + 1) = wordpens[d4];
                }
#endif
}   }   }   }

EXPORT void apply_scale3x(void)
{   FAST int   a, b, c, d, f, g, h, i,
               d1, d2, d3, d4, d5, d6, d7, d8, d9,
               newx, newy,
               x, y;
#ifdef WIN32
    FAST int   pp;
#endif
#ifdef AMIGA
    FAST int   px, py;
#endif

    for (x = 0; x < machines[machine].width; x++)
    {   for (y = 0; y < machines[machine].height; y++)
        {   if (rotating)
            {   newx = y;
                newy = widthheight - x;
                b  = screen[WEST(newx)][      newy ];
                d  = screen[     newx ][SOUTH(newy)];
                d5 = screen[     newx ][      newy ];
                f  = screen[     newx ][NORTH(newy)];
                h  = screen[EAST(newx)][      newy ];
            } else
            {   newx = x;
                newy = y;
                b  = screen[        x ][NORTH(   y)];
                d  = screen[WEST(   x)][         y ];
                d5 = screen[        x ][         y ];
                f  = screen[EAST(   x)][         y ];
                h  = screen[        x ][SOUTH(   y)];
            }

            if (b != h && d != f)
            {   if (rotating)
                {   a  = screen[WEST(newx)][SOUTH(newy)];
                    c  = screen[WEST(newx)][NORTH(newy)];
                    g  = screen[EAST(newx)][SOUTH(newy)];
                    i  = screen[EAST(newx)][NORTH(newy)];
                } else
                {   a  = screen[WEST(   x)][NORTH(   y)];
                    c  = screen[EAST(   x)][NORTH(   y)];
                    g  = screen[WEST(   x)][SOUTH(   y)];
                    i  = screen[EAST(   x)][SOUTH(   y)];
                }
                d1 = d == b ? d : d5;
                d2 = (d == b && d5 != c) || (b == f && d5 != a) ? b : d5;
                d3 = b == f ? f : d5;
                d4 = (d == b && d5 != g) || (d == h && d5 != a) ? d : d5;
                d6 = (b == f && d5 != i) || (h == f && d5 != c) ? f : d5;
                d7 = d == h ? d : d5;
                d8 = (d == h && d5 != i) || (h == f && d5 != g) ? h : d5;
                d9 = h == f ? f : d5;
            } else
            {   d1 = d2 = d3 = d4 = d6 = d7 = d8 = d9 = d5;
            }

            switch (fastsize)
            {
            case 4: // 3*3
#ifdef WIN32
                pp = (x * 3) + (y * 9 * machines[machine].width);
                if (sky == d1) { COPYSTAR(pp                                        ); }
                else           { COPYDISP(pp                                    , d1); }
                if (sky == d2) { COPYSTAR(pp + 1                                    ); }
                else           { COPYDISP(pp + 1                                , d2); }
                if (sky == d3) { COPYSTAR(pp + 2                                    ); }
                else           { COPYDISP(pp + 2                                , d3); }
                if (sky == d4) { COPYSTAR(pp +     (machines[machine].width * 3),   ); }
                else           { COPYDISP(pp +     (machines[machine].width * 3), d4); }
                if (sky == d5) { COPYSTAR(pp + 1 + (machines[machine].width * 3),   ); }
                else           { COPYDISP(pp + 1 + (machines[machine].width * 3), d5); }
                if (sky == d6) { COPYSTAR(pp + 2 + (machines[machine].width * 3),   ); }
                else           { COPYDISP(pp + 2 + (machines[machine].width * 3), d6); }
                if (sky == d7) { COPYSTAR(pp +     (machines[machine].width * 6),   ); }
                else           { COPYDISP(pp +     (machines[machine].width * 6), d7); }
                if (sky == d8) { COPYSTAR(pp + 1 + (machines[machine].width * 6),   ); }
                else           { COPYDISP(pp + 1 + (machines[machine].width * 6), d8); }
                if (sky == d9) { COPYSTAR(pp + 2 + (machines[machine].width * 6),   ); }
                else           { COPYDISP(pp + 2 + (machines[machine].width * 6), d9); }
#endif
#ifdef AMIGA
                px = x * 3;
                py = y * 3;
                *(byteptr[  py] + px    ) = bytepens[d1];
                *(byteptr[  py] + px + 1) = bytepens[d2];
                *(byteptr[  py] + px + 2) = bytepens[d3];
                *(byteptr[++py] + px    ) = bytepens[d4];
                *(byteptr[  py] + px + 1) = bytepens[d5];
                *(byteptr[  py] + px + 2) = bytepens[d6];
                if (!scanlines)
                {   *(byteptr[++py] + px    ) = bytepens[d7];
                    *(byteptr[  py] + px + 1) = bytepens[d8];
                    *(byteptr[  py] + px + 2) = bytepens[d9];
                }
#endif
            acase 5: // 6*3
#ifdef WIN32
                pp = (x * 6) + (y * 18 * machines[machine].width);
                if (sky == d1) { COPYWIDESTAR(pp                                         ); }
                else           { COPYWIDEDISP(pp                                     , d1); }
                if (sky == d2) { COPYWIDESTAR(pp + 2                                     ); }
                else           { COPYWIDEDISP(pp + 2                                 , d2); }
                if (sky == d3) { COPYWIDESTAR(pp + 4                                     ); }
                else           { COPYWIDEDISP(pp + 4                                 , d3); }
                if (sky == d4) { COPYWIDESTAR(pp +     (machines[machine].width *  6)    ); }
                else           { COPYWIDEDISP(pp +     (machines[machine].width *  6), d4); }
                if (sky == d5) { COPYWIDESTAR(pp + 2 + (machines[machine].width *  6)    ); }
                else           { COPYWIDEDISP(pp + 2 + (machines[machine].width *  6), d5); }
                if (sky == d6) { COPYWIDESTAR(pp + 4 + (machines[machine].width *  6)    ); }
                else           { COPYWIDEDISP(pp + 4 + (machines[machine].width *  6), d6); }
                if (sky == d7) { COPYWIDESTAR(pp +     (machines[machine].width * 12)    ); }
                else           { COPYWIDEDISP(pp +     (machines[machine].width * 12), d7); }
                if (sky == d8) { COPYWIDESTAR(pp + 2 + (machines[machine].width * 12)    ); }
                else           { COPYWIDEDISP(pp + 2 + (machines[machine].width * 12), d8); }
                if (sky == d9) { COPYWIDESTAR(pp + 4 + (machines[machine].width * 12)    ); }
                else           { COPYWIDEDISP(pp + 4 + (machines[machine].width * 12), d9); }
#endif
#ifdef AMIGA
                px = x * 3;
                py = y * 3;
                *(wordptr[  py] + px    ) = wordpens[d1];
                *(wordptr[  py] + px + 1) = wordpens[d2];
                *(wordptr[  py] + px + 2) = wordpens[d3];
                *(wordptr[++py] + px    ) = wordpens[d4];
                *(wordptr[  py] + px + 1) = wordpens[d5];
                *(wordptr[  py] + px + 2) = wordpens[d6];
                if (!scanlines)
                {   *(wordptr[++py] + px    ) = wordpens[d7];
                    *(wordptr[  py] + px + 1) = wordpens[d8];
                    *(wordptr[  py] + px + 2) = wordpens[d9];
                }
#endif
}   }   }   }

EXPORT void apply_scale4x(void)
{   PERSIST UBYTE tempscreen[BOXWIDTH * 2][BOXHEIGHT * 2]; // PERSISTent so as not to blow the stack
    FAST    UBYTE w_up, w_lt, w_ce, w_rt, w_dn,
                  d1, d2, d3, d4;
    FAST    int   height_doubled,
                  newx, newy,
                  width_doubled,
                  x, y;
#ifdef WIN32
    FAST    int   pp;
#endif
#ifdef AMIGA
    FAST    int   px, py;
#endif

    for (y = 0; y < machines[machine].height; y++)
    {   for (x = 0; x < machines[machine].width; x++)
        {   if (rotating)
            {   newx = y;
                newy = widthheight - x;
                w_up = screen[WEST(newx)][newy       ];
                w_lt = screen[     newx ][SOUTH(newy)];
                w_ce = screen[     newx ][newy       ];
                w_rt = screen[     newx ][NORTH(newy)];
                w_dn = screen[EAST(newx)][newy       ];
            } else
            {   newx = x;
                newy = y;
                w_up = screen[        x ][NORTH(   y)];
                w_lt = screen[WEST(   x)][         y ];
                w_ce = screen[        x ][         y ];
                w_rt = screen[EAST(   x)][         y ];
                w_dn = screen[        x ][SOUTH(   y)];
            }

            if (w_up != w_dn && w_lt != w_rt)
            {   d1 = w_lt == w_up ? w_lt : w_ce;
                d2 = w_up == w_rt ? w_rt : w_ce;
                d3 = w_lt == w_dn ? w_lt : w_ce;
                d4 = w_dn == w_rt ? w_rt : w_ce;
            } else
            {   d1 = d2 = d3 = d4 = w_ce;
            }

            tempscreen[ x * 2     ][ y * 2     ] = d1;
            tempscreen[(x * 2) + 1][ y * 2     ] = d2;
            tempscreen[ x * 2     ][(y * 2) + 1] = d3;
            tempscreen[(x * 2) + 1][(y * 2) + 1] = d4;
    }   }

    height_doubled = (machines[machine].height * 2) - 1;
    width_doubled  = (machines[machine].width  * 2) - 1;
    for (y = 0; y <= height_doubled; y++)
    {   for (x = 0; x <= width_doubled; x++)
        {   w_up = tempscreen[x                            ][y >              0 ? y - 1 : y];
            w_lt = tempscreen[x >             0 ? x - 1 : x][y                             ];
            w_ce = tempscreen[x                            ][y                             ];
            w_rt = tempscreen[x < width_doubled ? x + 1 : x][y                             ];
            w_dn = tempscreen[x                            ][y < height_doubled ? y + 1 : y];

            if (w_up != w_dn && w_lt != w_rt)
            {   d1 = w_lt == w_up ? w_lt : w_ce;
                d2 = w_up == w_rt ? w_rt : w_ce;
                d3 = w_lt == w_dn ? w_lt : w_ce;
                d4 = w_dn == w_rt ? w_rt : w_ce;
            } else
            {   d1 = d2 = d3 = d4 = w_ce;
            }

            switch (fastsize)
            {
            case 6: // 4*4
#ifdef WIN32
                pp = (x * 2) + (y * 8 * machines[machine].width);
                if (sky == d1) { COPYSTAR(pp                                        ); }
                else           { COPYDISP(pp                                    , d1); }
                if (sky == d2) { COPYSTAR(pp + 1                                    ); }
                else           { COPYDISP(pp + 1                                , d2); }
                if (sky == d3) { COPYSTAR(pp +     (machines[machine].width * 4),   ); }
                else           { COPYDISP(pp +     (machines[machine].width * 4), d3); }
                if (sky == d4) { COPYSTAR(pp + 1 + (machines[machine].width * 4),   ); }
                else           { COPYDISP(pp + 1 + (machines[machine].width * 4), d4); }
#endif
#ifdef AMIGA
                px = x * 2;
                py = y * 2;
                *(byteptr[py] + px    ) = bytepens[d1];
                *(byteptr[py] + px + 1) = bytepens[d2];
                if (!scanlines || !(y & 1))
                {   *(byteptr[++py] + px    ) = bytepens[d3];
                    *(byteptr[  py] + px + 1) = bytepens[d4];
                }
#endif
            acase 7: // 8*4
#ifdef WIN32
                pp = (x * 4) + (y * 16 * machines[machine].width);
                if (sky == d1) { COPYWIDESTAR(pp                                        ); }
                else           { COPYWIDEDISP(pp                                    , d1); }
                if (sky == d2) { COPYWIDESTAR(pp + 2                                    ); }
                else           { COPYWIDEDISP(pp + 2                                , d2); }
                if (sky == d3) { COPYWIDESTAR(pp +     (machines[machine].width * 8),   ); }
                else           { COPYWIDEDISP(pp +     (machines[machine].width * 8), d3); }
                if (sky == d4) { COPYWIDESTAR(pp + 2 + (machines[machine].width * 8),   ); }
                else           { COPYWIDEDISP(pp + 2 + (machines[machine].width * 8), d4); }
#endif
#ifdef AMIGA
                px = x * 2;
                py = y * 2;
                *(wordptr[py] + px    ) = wordpens[d1];
                *(wordptr[py] + px + 1) = wordpens[d2];
                if (!scanlines || !(y & 1))
                {   *(wordptr[++py] + px    ) = wordpens[d3];
                    *(wordptr[  py] + px + 1) = wordpens[d4];
                }
#endif
}   }   }   }

EXPORT void show_position(int x, int y, FLAG setting)
{   int addr,
        bit,
        hoffset,
        lines,
        row,
        rows,
        px, py,
        rx, ry,
        ux, uy,
        temp,
        whichset,
        voffset;

    if
    (   x >= 0
     && x <  machines[machine].width
     && y >= 0
     && y <  machines[machine].height
    )
    {   if (rotating)
        {   temp = y;
            y = machines[machine].height - 1 - x;
            x = temp;
        }

        #ifdef OPCOLOURS
            sprintf(coords, "$%X: %s", screen_iar[x][y], opcodes_2650[style][memory[screen_iar[x][y]]].name);
            return;
        #endif

        // ux,uy are the coordinates we tell the user; they are currently in USG coords.
        // px,py are the coordinates in PVI coords (only used for Arcadia/Interton/Elektor).
        // rx,ry are the coordinates in screen[][] coords.

        rx = px = ux = x;
        ry = py = uy = y;
        switch (machine)
        {
        case ARCADIA:
        case INTERTON:
        case ELEKTOR:
            if (usemargins)
            {   px -= USG_XMARGIN;
                py -= USG_YMARGIN;
            } else
            {   if (machine == ARCADIA)
                {   ux += USG_XMARGIN + UVI_HIDELEFT;
                    px +=               UVI_HIDELEFT;
                } else // INTERTON, ELEKTOR
                {   ux += USG_XMARGIN + PVI_HIDELEFT;
                    px +=               PVI_HIDELEFT;
                }
                uy += USG_YMARGIN + hidetop;
                py +=               hidetop;
            }
            if (px == 178)
            {   strcpy((char*) coords, "-,-");
                return;
            } // implied else
#ifdef VERBOSE
            sprintf
            (   (char*) coords,
                "USG X=%d,Y=%d. %cVI X=%d,Y=%d (pen %d (%s), colour $%06X)",
                ux,
                uy,
                (machine == ARCADIA) ? 'U' : 'P',
                ux - USG_XMARGIN,
                uy - USG_YMARGIN,
                screen[rx][ry],
                colournames[screen[rx][ry]],
                defpencolours[colourset][screen[rx][ry]]
            );
#else
            sprintf
            (   (char*) coords,
                "USG X=%d,Y=%d. %cVI X=%d,Y=%d (%s)",
                ux,
                uy,
                (machine == ARCADIA) ? 'U' : 'P',
                ux - USG_XMARGIN,
                uy - USG_YMARGIN,
                colournames[screen[rx][ry]]
            );
#endif
        acase BINBUG:
            if ((py == 312 && px >= 384) || py >= 313)
            {   strcpy((char*) coords, "-,-");
                return;
            }
        acase PHUNSY:
            if (py >= 313)
            {   strcpy((char*) coords, "-,-");
                return;
            }
        acase ZACCARIA:
        case MALZAK:
            ux += HIDDEN_X;
        }

        if (machine != ARCADIA && machine != INTERTON && machine != ELEKTOR)
        {
#ifdef VERBOSE
            sprintf
            (   (char*) coords,
                "X=%d,Y=%d (pen %d (%s), colour $%06X)",
                ux,
                uy,
                screen[rx][ry],
                colournames[screen[rx][ry]],
                defpencolours[colourset][screen[rx][ry]]
            );
#else
            sprintf
            (   (char*) coords,
                "X=%d,Y=%d (%s)",
                ux,
                uy,
                colournames[screen[rx][ry]]
            );
#endif
        }

        switch (machine)
        {
        case ARCADIA:
            voffset = 255 - memory[A_VSCROLL];
            lines   = (memory[A_BGCOLOUR] & 0x80) ?  8 : 16;
            rows    = (memory[A_GFXMODE ] & 0x40) ? 26 : 13;
            row     = (py - voffset) / lines;
            hoffset = hoffsets[row];
            addr    = ((row <= 12) ? (0x1800 + (row * 16)) : (0x1A00 + ((row - 13) * 16))) + ((px - hoffset) / 8);
            if (py >= voffset && row < rows && px >= hoffset && px < hoffset + 128)
            {   sprintf
                (   (char*) ENDOF(coords),
                    ". $%X: $%02X ('%c')",
                    addr,
                    memory[addr],
                    guestchar(memory[addr])
                );
                if (setting)
                {   scrnaddr = addr;
            }   }
        acase INTERTON:
        case ELEKTOR:
            if (px >= 32 && px <= 159 && py >= 20 && py <= 219)
            {   whichset = gridline[py];
                switch (whichset & 3)
                {
                case 0:
                    bit = 0;
                acase 1:
                    bit = (((py - 20) % 40) <= 10) ? 1 : 2;
                acase 2:
                    bit = 3;
                adefault: // ie. 3
                    bit = (((py - 20) % 40) <= 30) ? 4 : 5;
                }
                sprintf
                (   (char*) ENDOF(coords),
                    ". H: $%X:7..6,%d. V: $%X:%d",
                    0x1F00 + PVI_VERTGRID + (whichset * 2) + ((px >= 96) ? 1 : 0),
                    (px >= 96) ? (7 - ((px - 96) / 8)) : (7 - ((px - 32) / 8)),
                    0x1FA8 + (whichset / 4),
                    bit
                );
                if (setting)
                {   kx =  (px - 32) /  8;
                    ky = ((py - 20) / 20) * 3;
                    if (py % 20 >= 11)
                    {   ky += 2;
                    } elif (py % 20 >= 2)
                    {   ky++;
            }   }   }
        acase PIPBUG:
            x = ((x - absxmin) / pipbug_charwidth ) + 1;
            y = ((y - absymin) / pipbug_charheight) + 1;
            if (x >= 1 && x <= vdu_columns && y >= 1 && y <= vdu_rows_visible)
            {   if (pipbug_vdu == VDU_ELEKTERMINAL)
                {   sprintf
                    (   (char*) ENDOF(coords),
                        LLL(MSG_ROWCOLUMN2, ". Row=%d%s (%d%s), column=%d%s"),
                        y,
                        suffix(y),
                        (y + vdu_scrolly > 64) ? (y + vdu_scrolly - 64) : (y + vdu_scrolly),
                        suffix((y + vdu_scrolly > 64) ? (y + vdu_scrolly - 64) : (y + vdu_scrolly)),
                        x,
                        suffix(x)
                    );
                } else
                {   sprintf
                    (   (char*) ENDOF(coords),
                        LLL(MSG_ROWCOLUMN, ". Row=%d%s, column=%d%s"),
                        y,
                        suffix(y),
                        x,
                        suffix(x)
                    );
            }   }
        acase BINBUG:
            if (x < 576 && y < 256)
            {   x /=  9;
                y /= 16;
                addr = 0x7800 + (y * 64) + x;
                sprintf
                (   (char*) ENDOF(coords),
                    ". $%X: $%02X ('%c')",
                    addr,
                    memory[addr],
                    guestchar(memory[addr])
                );
                if (setting)
                {   scrnaddr = addr;
            }   }
        acase TWIN:
            if (x < 560 && y < 250)
            {   x = (x /  7) + 1;
                y = (y / 10) + 1;
                sprintf
                (   (char*) ENDOF(coords),
                    LLL(MSG_ROWCOLUMN, ". Row=%d%s, column=%d%s"),
                    y,
                    suffix(y),
                    x,
                    suffix(x)
                );
            }
        acase CD2650:
            if (x < 640 && y < 192)
            {   x /= 8;
                y /= 12;
                addr = 0x1000 + (x * 16) + y;
                sprintf
                (   (char*) ENDOF(coords),
                    ". $%X: $%02X ('%c')",
                    addr,
                    memory[addr],
                    guestchar(memory[addr])
                );
                if (setting)
                {   scrnaddr = addr;
            }   }
        acase PHUNSY:
            if (x >= 128 && y < 256)
            {   x = (x - 128) / 6;
                y /= 8;
                addr = 0x1000 + (y * 64) + x;
                sprintf
                (   (char*) ENDOF(coords),
                    ". $%X: $%02X ('%c')",
                    addr,
                    memory[addr],
                    guestchar(memory[addr])
                );
                if (setting)
                {   scrnaddr = addr;
            }   }
       acase ZACCARIA:
            if (memmap == MEMMAP_GALAXIA && x >= 32 && x <= 111)
            {   y -= galaxia_scrolly;
            }
            x /= 8;
            y /= 8;
            if (x >= 0 && x < 30 && y >= 0 && y < 30)
            {   addr = 0x1840 + (y * 32) + x;
                sprintf
                (   (char*) ENDOF(coords),
                    ". $%X: $%02X ('%c')",
                    addr,
                    memory[addr],
                    guestchar(memory[addr])
                );
                if (setting)
                {   scrnaddr = addr;
        }   }   }

        if (usemargins)
        {   switch (machine)
            {
            case ARCADIA:
            case INTERTON:
            case ELEKTOR:
                if (region == REGION_PAL)
                {   if (rx >= 6 && rx <= 48)
                    {   if   (rx <= 10) strcat((char*) coords, ". Horizontal front porch");
                        elif (rx <= 27) strcat((char*) coords, ". Horizontal retrace"); // aka horizontal sync pulse
                        elif (rx <= 30) strcat((char*) coords, ". Horizontal back porch");
                        elif (rx <= 39) strcat((char*) coords, ". Horizontal back porch (colour burst)");
                        else            strcat((char*) coords, ". Horizontal back porch");
                    }
                    if   ((ry ==  0 && rx >=   6) || (ry >=  1 && ry <=  11) || (ry == 12 && rx <= 112)) strcat((char*) coords, ". Vertical front porch");
                    elif ((ry == 12 && rx >= 113) || (ry >= 13 && ry <=  14) || (ry == 15 && rx <=  10)) strcat((char*) coords, ". Vertical retrace");
                    elif ((ry == 15 && rx >=  11) || (ry >= 16 && ry <=  42) || (ry == 43 && rx <=   5)) strcat((char*) coords, ". Vertical back porch");

                    if (rx >= 11 && rx <= 27)
                    {   strcat((char*) coords, ", CBLNK, HRST, HS");
                        if ((ry == 12 && rx >= 113) || (ry >= 13 && ry <=  14) || (ry == 15 && rx <=  10))
                        {   strcat((char*) coords, ", CSYNC");
                    }   }
                    else
                    {   if ((ry == 12 && rx >= 113) || (ry >= 13 && ry <=  14) || (ry == 15 && rx <=  10))
                        {   ;
                        } else
                        {   strcat((char*) coords, ", CSYNC");
                        }
                        if (rx >= 6 && rx <= 48)
                        {   // assert(rx <= 10 || rx >= 28);
                            strcat((char*) coords, ", CBLNK, HRST");
                            if (rx >= 31 && rx <= 39)
                            {   strcat((char*) coords, ", CBF");
                    }   }   }

                    if ((ry == 12 && rx >= 113) || (ry >= 13 && ry <=  14) || (ry == 15 && rx <=  10))
                    {   strcat((char*) coords, ", VRST, VS"); // vertical retrace
                    } elif
                    (   (ry ==  0 && rx >=   6)
                     || (ry >=  1 && ry <=  42)
                     || (ry == 43 && rx <=   5)
                    )
                    {   strcat((char*) coords, ", VRST");
                    }

                    if
                    (   (rx >= 19 && (ry & 1) == 0)
                     || (rx <= 18 && (ry & 1) == 1)
                    )
                    strcat((char*) coords, ", OE");
                } else
                {   if (rx >= 10 && rx <= 48)
                    {   if   (rx <= 14) strcat((char*) coords, ". Horizontal front porch");
                        elif (rx <= 31) strcat((char*) coords, ". Horizontal retrace"); // aka horizontal sync pulse
                        elif (rx <= 33) strcat((char*) coords, ". Horizontal back porch");
                        elif (rx <= 42) strcat((char*) coords, ". Horizontal back porch (colour burst)");
                        else            strcat((char*) coords, ". Horizontal back porch");
                    }
                    if   ((ry ==  0 && rx >=  10) || (ry >=  1 && ry <=   2) || (ry ==  3 && rx <=   9)) strcat((char*) coords, ". Vertical front porch"); // vertical front porch is 10,0..31, 3
                    elif ((ry ==  3 && rx >=  32) || (ry >=  4 && ry <=   5) || (ry ==  6 && rx <=  31)) strcat((char*) coords, ". Vertical retrace");     // vertical retrace     is 32,3..31, 6
                    elif ((ry ==  6 && rx >=  32) || (ry >=  7 && ry <=  19) || (ry == 20 && rx <=   9)) strcat((char*) coords, ". Vertical back porch");  // vertical back  porch is 32,6.. 9,20

                    if (rx >= 15 && rx <= 31)
                    {   strcat((char*) coords, ", CBLNK, HRST, HS");
                        if ((ry ==  3 && rx >=  32) || (ry >=  4 && ry <=   5) || (ry ==  6 && rx <=  31))
                        {   strcat((char*) coords, ", CSYNC");
                    }   }
                    else
                    {   if ((ry ==  3 && rx >=  32) || (ry >=  4 && ry <=   5) || (ry ==  6 && rx <=  31))
                        {   ;
                        } else
                        {   strcat((char*) coords, ", CSYNC");
                        }
                        if (rx >= 10 && rx <= 48)
                        {   // assert(rx <= 14 || rx >= 32);
                            strcat((char*) coords, ", CBLNK, HRST");
                            if (rx >= 34 && rx <= 42)
                            {   strcat((char*) coords, ", CBF");
                    }   }   }

                    if ((ry ==  3 && rx >=  32) || (ry >=  4 && ry <=   5) || (ry ==  6 && rx <=  31))
                    {   strcat((char*) coords, ", VRST, VS"); // vertical retrace     is 32,3..31, 6
                    } elif
                    (   (ry ==  0 && rx >=  10)
                     || (ry >=  1 && ry <=  19)
                     || (ry == 20 && rx <=   9)
                    )
                    {   strcat((char*) coords, ", VRST");
                    }

                    if
                    (   (rx >= 23 && (ry & 1) == 0)
                     || (rx <= 22 && (ry & 1) == 1)
                    )
                    strcat((char*) coords, ", OE");
                }
            acase PIPBUG:
                switch (screen[rx][ry])
                {
                case  CYAN:                      strcat((char*) coords, ". Horizontal retrace");
                acase DARKBLUE:                  strcat((char*) coords, ". Horizontal retrace. Vertical retrace");
                acase PURPLE:                    strcat((char*) coords, ". Vertical retrace");
                acase DARKRED: if (rx <= 83)     strcat((char*) coords, ". Horizontal back porch. Vertical retrace");
                               else              strcat((char*) coords, ". Horizontal front porch. Vertical retrace");
                acase PINK:    if (ry <= 42)     strcat((char*) coords, ". Vertical back porch");
                               else              strcat((char*) coords, ". Vertical front porch");
                acase ORANGE:  if (rx <= 83)     strcat((char*) coords, ". Horizontal back porch");
                               else              strcat((char*) coords, ". Horizontal front porch");
                acase BLUE:    if (ry <= 42)     strcat((char*) coords, ". Horizontal retrace. Vertical back porch");
                               else              strcat((char*) coords, ". Horizontal retrace. Vertical front porch");
                acase RED:     if (ry <= 42)
                                   if (rx <= 83) strcat((char*) coords, ". Horizontal back porch. Vertical back porch");
                                   else          strcat((char*) coords, ". Horizontal front porch. Vertical back porch");
                               else
                                   if (rx <= 83) strcat((char*) coords, ". Horizontal back porch. Vertical front porch");
                                   else          strcat((char*) coords, ". Horizontal front porch. Vertical front porch");
                }
            acase BINBUG:
                if   (ry <= 311)
                {   if (rx >= 576 && rx <= 767)
                    {   if (ry < 256)
                        {   if   (rx <          642) strcat((char*) coords, ". Horizontal front porch");
                            elif (rx >          701) strcat((char*) coords, ". Horizontal back porch");
                            else                     strcat((char*) coords, ". Horizontal retrace");
                        } else
                        {   if   (rx < HPULSE_FIRST) strcat((char*) coords, ". Horizontal front porch");
                            elif (rx > HPULSE_LAST ) strcat((char*) coords, ". Horizontal back porch");
                            else                     strcat((char*) coords, ". Horizontal retrace");
                    }   }

                    if   ((ry >= 256 && ry <= 280) || (ry == 281 && rx <= 695)                            ) strcat((char*) coords, ". Vertical front porch");
                    elif ((ry == 281 && rx >= 696) || (ry >= 282 && ry <= 285) || (ry == 286 && rx <= 455)) strcat((char*) coords, ". Vertical retrace");
                    elif ((ry == 286 && rx >= 456) || (ry >= 287 && ry <= 313)                            ) strcat((char*) coords, ". Vertical back porch");
                }
            acase TWIN:
                if   (rx >= 560 && rx <= 699) strcat((char*) coords, ". Horizontal porch/retrace");

                if   (ry >= 250 && ry <= 269) strcat((char*) coords, ". Vertical porch/retrace");
            acase CD2650:
                if   (rx >= 640 && rx <= 711) strcat((char*) coords, ". Horizontal front porch");
                elif (rx >= 712 && rx <= 775) strcat((char*) coords, ". Horizontal retrace");
                elif (rx >= 776 && rx <= 903) strcat((char*) coords, ". Horizontal back porch");

                if   (ry >= 192 && ry <= 215) strcat((char*) coords, ". Vertical front porch");
                elif (ry >= 216 && ry <= 227) strcat((char*) coords, ". Vertical retrace");
                elif (ry >= 228 && ry <= 263) strcat((char*) coords, ". Vertical back porch");
            acase PHUNSY:
                if   (ry <= 312)
                {   if       (rx <=  25) strcat((char*) coords, ". Horizontal front porch");
                    elif     (rx <=  57) strcat((char*) coords, ". Horizontal retrace");
                    elif     (rx <=  65) strcat((char*) coords, ". Horizontal back porch");
                    elif     (rx <=  81) strcat((char*) coords, ". Horizontal back porch (colour burst)");
                    elif     (rx <= 127) strcat((char*) coords, ". Horizontal back porch");

                    if       (ry >= 256)
                    {   if   (ry <= 268) strcat((char*) coords, ". Vertical front porch");
                        elif (ry <= 272) strcat((char*) coords, ". Vertical retrace");
                        else             strcat((char*) coords, ". Vertical back porch");
                }   }
            acase PONG:
                rx /= 2;

                if   (                rx <  ponglt)
                {                                   strcat((char*) coords, ". Horizontal back porch");
                    if (colourset != GREYSCALE)
                    {   if (region == REGION_PAL)
                        {   if (rx >= PONG_CBSTART_PAL  && rx <= PONG_CBSTART_PAL  + 9) strcat((char*) coords, " (colour burst)");
                        } else
                        {   if (rx >= PONG_CBSTART_NTSC && rx <= PONG_CBSTART_NTSC + 9) strcat((char*) coords, " (colour burst)");
                }   }   }
                elif (rx >  pongrt && rx <=    118) strcat((char*) coords, ". Horizontal front porch");
                elif (rx >=    119                ) strcat((char*) coords, ". Horizontal retrace");

                if (region == REGION_PAL)
                {   if   (             ry <=  43) strcat((char*) coords, ". Vertical back porch");
                    elif (ry >= 276 && ry <= 305) strcat((char*) coords, ". Vertical front porch");
                    elif (ry >= 306             ) strcat((char*) coords, ". Vertical retrace");
                } else
                {   if   (             ry <=  41) strcat((char*) coords, ". Vertical back porch");
                    elif (ry >= 234 && ry <= 257) strcat((char*) coords, ". Vertical front porch");
                    elif (ry >= 258             ) strcat((char*) coords, ". Vertical retrace");
    }   }   }   }
    else
    {   strcpy((char*) coords, "-,-");
}   }

EXPORT void calc_margins(void)
{
#ifdef WIN32
   int temp;
#endif

    switch (machine)
    {
    case PIPBUG:
        switch (pipbug_vdu)
        {
        case VDU_ELEKTERMINAL:
            vdu_columns        = 64;
            vdu_rows_visible   = 16;
            vdu_rows_total     = 64;
            pipbug_charwidth   =  8;
            pipbug_charwidth2  =  5;
            pipbug_charheight  = 12;
            pipbug_charheight2 =  8; // top row is always blank
        acase VDU_LCVDU_NARROW:
            vdu_columns        = 32;
            vdu_rows_visible   =
            vdu_rows_total     = 16;
            pipbug_charwidth   =  6;
            pipbug_charwidth2  =  5;
            pipbug_charheight  = 12;
            pipbug_charheight2 =  8;
        acase VDU_LCVDU_WIDE:
            vdu_columns        = 64;
            vdu_rows_visible   =
            vdu_rows_total     = 16;
            pipbug_charwidth   =  6;
            pipbug_charwidth2  =  5;
            pipbug_charheight  = 12;
            pipbug_charheight2 =  8;
        acase VDU_RADIOBULLETIN:
            vdu_columns        = 50;
            vdu_rows_visible   =
            vdu_rows_total     = 16;
            pipbug_charwidth   =  7;
            pipbug_charwidth2  =  5;
            pipbug_charheight  = 16;
            pipbug_charheight2 =  9;
        acase VDU_SVT100:
            vdu_columns        = 64;
            vdu_rows_visible   =
            vdu_rows_total     = 16;
            pipbug_charwidth   =  6;
            pipbug_charwidth2  =  5;
            pipbug_charheight  =  8;
            pipbug_charheight2 =  8; // top row is always blank
        acase VDU_VT100:
            vdu_columns        = 80;
            vdu_rows_visible   =
            vdu_rows_total     = 24;
            pipbug_charwidth   =  8;
            pipbug_charwidth2  =  8;
            pipbug_charheight  = 12;
            pipbug_charheight2 = 10;
        }
    acase TWIN:
        vdu_columns            = 80;
        vdu_rows_visible       =
        vdu_rows_total         = 25;
    acase SELBST:
        vdu_columns            = 64;
        vdu_rows_visible       =
        vdu_rows_total         = 16;
    }
    machines[PIPBUG].width  = vdu_columns      * pipbug_charwidth;
    machines[PIPBUG].height = vdu_rows_visible * pipbug_charheight; // 128 or 192 or 288
    if (machine == PIPBUG && usemargins && pipbug_vdu == VDU_RADIOBULLETIN)
    {   machines[PIPBUG].width  += 168; // 350 -> 518
        machines[PIPBUG].height +=  57; // 256 -> 313
    }
    if (showleds)
    {   machines[PIPBUG].height += PIPBUG_LEDHEIGHT;
    }

    if (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR)
    {   if (region == REGION_PAL)
        {   n1 =  43; // Y-margin
            n2 = 177; // middle of screen
            n3 = 312; // entire raster
            n4 =   6;
            machines[machine].cpf = 17706.0; // (227 / 4) * 312 = 17,706   CPL. * 50.0804105952784 = 886,723.75 Hz
        } else
        {   n1 =  20; // Y-margin
            n2 = 141; // middle of screen
            n3 = 262; // entire raster
            n4 =  10;
            machines[machine].cpf = 14868.5; // (227 / 4) * 262 = 14,868.5 CPL. * 60.1867202475031 = 894,886.25 Hz
    }   }

    if (memmap == MEMMAP_8550 && pong_machine == PONGMACHINE_1976)
    {   ponglt =  23-2; // it isn't centred,
        pongrt = 101-2; // this is authentic
    } else
    {   ponglt =  27-2;
        pongrt =  99-2;
    }

#ifdef WIN32
    switch (machine)
    {
    case  PIPBUG:   bezelwidth =    VT100_BEZELWIDTH; bezelheight =    VT100_BEZELHEIGHT;
    acase TWIN:     bezelwidth =     TWIN_BEZELWIDTH; bezelheight =     TWIN_BEZELHEIGHT;
    acase ZACCARIA:
        switch (memmap)
        {
        case  MEMMAP_ASTROWARS:   bezelwidth = ASTROWARS_BEZELWIDTH; bezelheight = ASTROWARS_BEZELHEIGHT;
        acase MEMMAP_GALAXIA:     bezelwidth =   GALAXIA_BEZELWIDTH; bezelheight =   GALAXIA_BEZELHEIGHT;
        acase MEMMAP_LASERBATTLE:
        case  MEMMAP_LAZARIAN:    bezelwidth =  LAZARIAN_BEZELWIDTH; bezelheight =  LAZARIAN_BEZELHEIGHT;
        }
        if (memmapinfo[memmap].rotate && !rotating)
        {   temp        = bezelwidth;
            bezelwidth  = bezelheight;
            bezelheight = temp;
    }   }
#endif

    if (usemargins)
    {   // The XVI's origin (0,0) is at USG_XMARGIN,USG_YMARGIN on this screen

        machines[ARCADIA ].width  =
        machines[INTERTON].width  =
        machines[ELEKTOR ].width  = 228;
        machines[BINBUG  ].width  = 768;
        machines[TWIN    ].width  = 700;
        machines[CD2650  ].width  = 904; // 113* 8=904
        machines[PHUNSY  ].width  = 512;
        machines[PONG    ].width  = 256; // 128* 2=256

        machines[BINBUG  ].height = 313;
        machines[TWIN    ].height = 270;
        machines[CD2650  ].height = 264; //  22*12=264
        machines[PHUNSY  ].height = 313;
        machines[SELBST  ].height = 128; //  16* 8=128
        if (region == REGION_PAL)
        {   machines[ARCADIA ].height =
            machines[INTERTON].height =
            machines[ELEKTOR ].height =
            machines[PONG    ].height = 312;
        } else
        {   machines[ARCADIA ].height =
            machines[INTERTON].height =
            machines[ELEKTOR ].height =
            machines[PONG    ].height = 262;
        }
        if (showleds)
        {   machines[BINBUG  ].height += BINBUG_LEDHEIGHT;
            machines[PHUNSY  ].height += PHUNSY_LEDHEIGHT;
            machines[SELBST  ].height += SELBST_LEDHEIGHT;
        }

        if (machine == PIPBUG && pipbug_vdu == VDU_RADIOBULLETIN)
        {   absxmin = 84;
            absymin = 43;
            absxmax = 84 + machines[machine].width  - 1;
            absymax = 43 + machines[machine].height - 1;
        } else
        {   absxmin =
            absymin = 0;
            absxmax = machines[machine].width  - 1;
            absymax = machines[machine].height - 1;
    }   }
    else
    {   // The XVI's origin (0,0) is at -XVI_HIDELEFT,0 on this screen
        // It omits the entire USG_XMARGIN and an additional HIDELEFT pixels from the left side, and some pixels from the right side.
        // It omits the entire USG_YMARGIN from the top side.

        machines[ARCADIA ].width  =
        machines[INTERTON].width  =
        machines[ELEKTOR ].width  = 164;
        machines[BINBUG  ].width  = 576; //  64* 9=576
        machines[TWIN    ].width  = 560; //  80* 7=560
        machines[CD2650  ].width  = 640; //  80* 8=640
        machines[PHUNSY  ].width  = 384; //  64* 6=384
        if (memmap == MEMMAP_8550 && pong_machine == PONGMACHINE_1976)
        {   machines[PONG].width  = ROUNDTO4((79 * 2) + (PONGXMARGIN * 4));
        } else
        {   machines[PONG].width  = ROUNDTO4((73 * 2) + (PONGXMARGIN * 4));
        }
        if (region == REGION_PAL)
        {   hidetop = 0;
            machines[ARCADIA ].height =
            machines[INTERTON].height =
            machines[ELEKTOR ].height = 269; // ie. 312 - 43
            machines[PONG    ].height = 232 + (PONGYMARGIN * 2);
        } else
        {   hidetop = 8;
            machines[ARCADIA ].height =
            machines[INTERTON].height =
            machines[ELEKTOR ].height = 226; // ie. 262 - 36
            machines[PONG    ].height = 192 + (PONGYMARGIN * 2);
        }
        machines[BINBUG      ].height = 256;
        machines[TWIN        ].height = 250; // 25*10=250
        machines[CD2650      ].height = 192; // 16*12=192
        machines[PHUNSY      ].height = 256;
        machines[SELBST      ].height = 128; // 16* 8=128
        if (showleds)
        {   machines[BINBUG  ].height += BINBUG_LEDHEIGHT;
            machines[PHUNSY  ].height += PHUNSY_LEDHEIGHT;
            machines[SELBST  ].height += SELBST_LEDHEIGHT;
        }

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
        acase PIPBUG:
        case BINBUG:
        case TWIN:
        case CD2650:
            absxmin =
            absymin = 0;
            absxmax = machines[machine].width  - 1;
            absymax = machines[machine].height - 1;
        acase PHUNSY:
            absxmin = 128;
            absymin = 0;
            absxmax = machines[machine].width  + 128 - 1;
            absymax = machines[machine].height       - 1;
        acase PONG:
            absxmin = 27;
            if (region == REGION_PAL)
            {   absymin = 44;
            } else
            {   absymin = 42;
            }
            absxmax = machines[machine].width  - 1;
            absymax = machines[machine].height - 1;
        acase MALZAK:
        case ZACCARIA:
            absxmin = absymin =   0;
            absxmax = absymax = 255;
}   }   }

EXPORT void change_colour_names(void)
{   colourname[  0] = (STRPTR) LLL(MSG_COLOUR_WHITE   , "white"        );
    colourname[  1] = (STRPTR) LLL(MSG_COLOUR_YELLOW  , "yellow"       );
    colourname[  2] = (STRPTR) LLL(MSG_COLOUR_PURPLE  , "purple"       );
    colourname[  3] = (STRPTR) LLL(MSG_COLOUR_RED     , "red"          );
    colourname[  4] = (STRPTR) LLL(MSG_COLOUR_CYAN    , "cyan"         );
    colourname[  5] = (STRPTR) LLL(MSG_COLOUR_GREEN   , "green"        );
    colourname[  6] = (STRPTR) LLL(MSG_COLOUR_BLUE    , "blue"         );
    colourname[  7] = (STRPTR) LLL(MSG_COLOUR_BLACK   , "black"        );

    colournames[ 0] = (STRPTR) LLL(MSG_COLOUR2_WHITE  , "White"        );
    colournames[ 1] = (STRPTR) LLL(MSG_COLOUR2_YELLOW , "Yellow"       );
    colournames[ 2] = (STRPTR) LLL(MSG_COLOUR2_LPURPLE, "Bright Purple");
    colournames[ 3] = (STRPTR) LLL(MSG_COLOUR2_LRED   , "Bright Red"   );
    colournames[ 4] = (STRPTR) LLL(MSG_COLOUR2_LCYAN  , "Bright Cyan"  );
    colournames[ 5] = (STRPTR) LLL(MSG_COLOUR2_LGREEN , "Bright Green" );
    colournames[ 6] = (STRPTR) LLL(MSG_COLOUR2_LBLUE  , "Bright Blue"  );
    colournames[ 7] = (STRPTR) LLL(MSG_COLOUR2_BLACK1 , "Black #1"     );
    colournames[ 8] = (STRPTR) LLL(MSG_COLOUR2_GREY   , "Grey #1"      );
    colournames[ 9] = (STRPTR) LLL(MSG_COLOUR2_BROWN  , "Brown"        );
    colournames[10] = (STRPTR) LLL(MSG_COLOUR2_DPURPLE, "Dark Purple"  );
    colournames[11] = (STRPTR) LLL(MSG_COLOUR2_DRED   , "Dark Red"     );
    colournames[12] = (STRPTR) LLL(MSG_COLOUR2_DCYAN  , "Dark Cyan"    );
    colournames[13] = (STRPTR) LLL(MSG_COLOUR2_DGREEN , "Dark Green"   );
    colournames[14] = (STRPTR) LLL(MSG_COLOUR2_DBLUE  , "Dark Blue"    );
    colournames[15] = (STRPTR) LLL(MSG_COLOUR2_BLACK2 , "Black #2"     );
    colournames[16] = (STRPTR) LLL(MSG_COLOUR2_GREY2  , "Grey #2"      );
    colournames[17] = (STRPTR) LLL(MSG_COLOUR2_GREY3  , "Grey #3"      );
    colournames[18] = (STRPTR) LLL(MSG_COLOUR2_GREY4  , "Grey #4"      );
    colournames[19] = (STRPTR) LLL(MSG_COLOUR2_GREY5  , "Grey #5"      );
    colournames[20] = (STRPTR) LLL(MSG_COLOUR2_GREY6  , "Grey #6"      );
    colournames[21] = (STRPTR) LLL(MSG_COLOUR2_GREY7  , "Grey #7"      );
    colournames[22] = (STRPTR) LLL(MSG_COLOUR2_ORANGE , "Orange"       );
    colournames[23] = (STRPTR) LLL(MSG_COLOUR2_PINK   , "Pink"         );
    colournames[24] = NULL;
}

EXPORT FLAG edit_screen(UWORD code)
{   // assert(editscreen);

    // KeyMatrix[code / 8] |= (1 << (code % 8));
    if (ctrl())
    {   return FALSE;
    }

    switch (machine)
    {
    case  ARCADIA:  return  arcadia_edit_screen(code);
    acase INTERTON:
    case  ELEKTOR:  return      pvi_edit_screen(code);
    acase BINBUG:   return   binbug_edit_screen(code);
    acase CD2650:   return   cd2650_edit_screen(code);
    acase PHUNSY:   return   phunsy_edit_screen(code);
    acase ZACCARIA: return zaccaria_edit_screen(code);
    adefault:       return FALSE;
}   }

EXPORT void edit_screen_sanitize(void)
{   if (editscreen)
    {   switch (machine)
        {
        case ARCADIA:
            if
            (   (scrnaddr >= 0x1800 && scrnaddr <= 0x18CF)
             || (scrnaddr >= 0x1A00 && scrnaddr <= 0x1ACF)
            )
            {   ;
            } else
            {   scrnaddr = 0x1800;
            }
        acase INTERTON:
        case ELEKTOR:
            if (kx < 0 || kx > 15)
            {   kx = 0;
            }
            if (ky < 0 || ky > 29)
            {   ky = 0;
            }
        acase BINBUG:
            if  (scrnaddr >= 0x7800 && scrnaddr <= 0x7BFF)
            {   ;
            } else
            {   scrnaddr = 0x7800;
            }
        acase CD2650:
            if  (scrnaddr >= 0x1000 && scrnaddr <= 0x14FF)
            {   ;
            } else
            {   scrnaddr = 0x1000;
            }
        acase PHUNSY:
            if  (scrnaddr >= 0x1000 && scrnaddr <= 0x17FF)
            {   ;
            } else
            {   scrnaddr = 0x1000;
            }
        acase ZACCARIA:
            if  (scrnaddr >= 0x1800 && scrnaddr <= 0x1BFF)
            {   ;
            } else
            {   scrnaddr = 0x1840; // $1800 is outside visible area
            }
        adefault:
            editscreen = FALSE;
}   }   }

EXPORT void thecout(UBYTE value)
{   TRANSIENT int   x, y;
    PERSIST   UBYTE SpeakBuffer[80 + 1]; // PERSISTent so as not to blow the stack

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "COUT: %s at cycle %d.\n", asciiname_long[value], cycles_2650);
#endif

    if (ignore_cout)
    {   ignore_cout = FALSE;
        return;
    }

    if (HASPAPERTAPE && papertapemode[0] == TAPEMODE_RECORD && paperpunchenabled)
    {   // assert(papertapewhere[0] < PAPERTAPEMAX);
        if (papertapeprotect[0])
        {   papertapebyte[0] = 0;
            if (papertapewhere[0] == papertapelength[0])
            {   PapertapeBuffer[0][papertapewhere[0]] = papertapebyte[0]; // extend papertape with blanks
        }   }
        else
        {   PapertapeBuffer[0][papertapewhere[0]] = papertapebyte[0] = value;
            if (ambient)
            {   play_sample(SAMPLE_PUNCH);
        }   }
        if (papertapewhere[0] == papertapelength[0])
        {   papertapelength[0]++;
        }
        papertapewhere[0]++;
        if (papertapewhere[0] == PAPERTAPEMAX)
        {   papertape_stop(0); // this calls update_papertape()
        } else
        {   update_papertape(0, FALSE);
        }
        // we assume we should still echo it to the terminal as usual
    }

    // emit value as an ASCII character
    if (value != NUL)
    {   /* if (ambient)
        {   if (value > ' ')
            {   play_sample(SAMPLE_TELETYPE);
            } elif (value == ' ' || value == HT || value == VT || value == CR || value == BS || value == DEL)
            {   play_sample(SAMPLE_SPACE);
        }   } */

        if (whichgame == PIPBUG_LIFEMCPOS)
        {   value &= 0x7F;
        }

        if (echoconsole)
        {   zprintf(TEXTPEN_PIPBUG, "%c", value);
            if (value == 8) // backspace
            {   zprintf(TEXTPEN_PIPBUG, " \b");
            }
#ifdef AMIGA
            DISCARD fflush(NULL); // it would be better to only flush stdout?
#endif
        }

        if (machine == TWIN)
        {   if (twin_escaped)
            {   switch (value)
                {
                case 'A': // up
                    if (vdu_y == 0)
                    {   vdu_y = 24;
                    } else
                    {   vdu_y--;
                    }
                acase 'B': // down
                    if (vdu_y == 24)
                    {   vdu_y = 0;
                    } else
                    {   vdu_y++;
                    }
                acase 'C': // right
                    if (vdu_x == 79)
                    {   vdu_x = 0;
                        if (vdu_y == 24)
                        {   vdu_y = 0;
                        } else
                        {   vdu_y++;
                    }   }
                    else
                    {   vdu_x++;
                    }
                acase 'D': // left
                    if (vdu_x == 0)
                    {   vdu_x = 79;
                        if (vdu_y == 0)
                        {   vdu_y = 24;
                        } else
                        {   vdu_y--;
                    }   }
                    else
                    {   vdu_x--;
                    }
                acase 'E': // clear screen
                    for (y = 0; y < 25; y++)
                    {   for (x = 0; x < 80; x++)
                        {   vdu[x][y] = blank;
                    }   }
                //lint -fallthrough
                case 'H': // home
                    vdu_x = vdu_y = 0;
                acase 'J': // erase to end of screen
                    // We assume this also affects the byte at the cursor position.
                    for (x = vdu_x; x < 80; x++)
                    {   vdu[x][vdu_y] = blank;
                    }
                acase 'K': // erase line
                    for (x = 0; x < 80; x++)
                    {   vdu[x][vdu_y] = blank;
                }   }
                twin_escaped = FALSE;
                return;
            } elif (value == ESC)
            {   twin_escaped = TRUE;
                return;
        }   }

        switch (value)
        {
        case EOT: // 4 (Ctrl+D) Down
            if (machine == PIPBUG && pipbug_vdu == VDU_ELEKTERMINAL)
            {   elekterminal_scroll(1);
            }
        acase BEL: // Ctrl+G, ie. bell (\a)
            play_bell(0);
        acase BS: //  8 (BackSpace) (Ctrl+H or Left or Backspace)
            if (vdu_x)
            {   vdu_x--;
                vdu[vdu_x][vdu_y] = blank;
            }
        acase DEL:
            if
            (   vdu_x
             && (   (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
                 ||  machine == TWIN
            )   )
            {   vdu_x--;
                vdu[vdu_x][vdu_y] = blank;
            }
        acase HT: //  9 (Horizontal Tab) (Ctrl+I or Right)
            if (vdu_x < vdu_columns - 1)
            {   vdu_x++;
            }
        acase LF: // 10 (Line Feed) (Ctrl+J or Down)
            vdu_x = 0;
            if (vdu_y == vdu_rows_total - 1)
            {   scroll_vdu();
            } else
            {   vdu_y++;
                for (x = 0; x < vdu_columns; x++)
                {   vdu[x][vdu_y] = blank;
            }   }
        acase VT: // 11 (Vertical Tab) (Ctrl+K or Up)
            if (vdu_y)
            {   vdu_y--;
            }
        acase FF: // 12 (Form Feed) (Ctrl+L)
            if (machine == PIPBUG && pipbug_vdu == VDU_ELEKTERMINAL)
            {   for (x = 0; x < vdu_columns; x++)
                {   for (y = 48; y < 64; y++)
                    {   vdu[x][y] = blank;
                }   }
                vdu_y = 48;
            } else
            {   for (x = 0; x < vdu_columns; x++)
                {   for (y = 0; y < vdu_rows_visible; y++)
                    {   vdu[x][y] = blank;
                }   }
                vdu_y = 0;
            }
            vdu_x = 0;
        acase CR: // 13 (Carriage Return) (Ctrl+M or ENTER)
            if (vdu_x < vdu_columns - 1)
            {   for (x = vdu_x; x < vdu_columns; x++)
                {   vdu[x][vdu_y] = blank;
            }   }
            vdu_x = 0;
        acase DC1: // 17 (Ctrl+Q)
            paperreaderenabled = TRUE;
        acase DC2: // 18 (Ctrl+R)
            paperpunchenabled = TRUE;
        acase DC3: // 19 (Ctrl+S)
            paperreaderenabled = FALSE;
        acase DC4: // 20 (Ctrl+T)
            paperpunchenabled = FALSE;
        acase NAK: // 21 (Ctrl+U) Up
            if (machine == PIPBUG && pipbug_vdu == VDU_ELEKTERMINAL)
            {   elekterminal_scroll(-1);
            }
        acase SUB: // 26 (SUBstitute) (Ctrl+Z)
            for (x = 0; x < vdu_columns; x++)
            {   vdu[x][vdu_y] = blank;
            }
        acase CAN: // 24 (Ctrl+X)
            if (machine == PIPBUG && pipbug_vdu == VDU_ELEKTERMINAL)
            {   elekterminal_scroll(16);
            }
        acase EM: // 25 (Ctrl+Y)
            if (machine == PIPBUG && pipbug_vdu == VDU_ELEKTERMINAL)
            {   elekterminal_scroll(-16);
            }
        acase FS: // 28 (File Separator) (Ctrl+Shift+L or Home)
            vdu_x =
            vdu_y = 0;
        acase GS: // 29 (Group Separator) (Ctrl+Shift+M)
            vdu_x = 0;
        adefault:
            if (value < 32)
            {   return;
            } else
            {   vdu[vdu_x++][vdu_y] = value;
                if (vdu_x == vdu_columns)
                {   vdu_x = 0;
                    if (vdu_y == vdu_rows_total - 1)
                    {   scroll_vdu();
                    } else
                    {   vdu_y++;
        }   }   }   }

        if (usespeech)
        {   if (value == LF)
            {   SpeakBuffer[speakoffset] = EOS;
                // assert(speakoffset == strlen(SpeakBuffer));
                if (sound && hostvolume && speakoffset)
                {   speak((STRPTR) SpeakBuffer);
                }
                speakoffset = 0;
            } elif
            (   value != 13
             && speakoffset < 80
             && (ras[(psu & PSU_SP) - 1] != 0x002C) // better than (value != '*')
            )
            {   if
                (   value == '='
                 && !strncmp((const char*) SpeakBuffer, "NO LEFT", 7) // special-case code for Nim.pgm
                )
                {   strcpy((char*) SpeakBuffer, "NUMBER LEFT="); // NO. instead of NUMBER doesn't work because of end-of-sentence (.) detection
                    speakoffset = 12;
                } elif
                (   value == '-'
                 && !strncmp((const char*) SpeakBuffer, "MOVE-",   5) // special-case code for Nim.pgm
                )
                {   strcpy((char*) SpeakBuffer, "MOVE");
                    speakoffset = 4;
                } else
                {   SpeakBuffer[speakoffset++] = value;

                    if
                    (   value == '!'
                     || value == '?'
                     || value == '.'
                    )
                    {   SpeakBuffer[speakoffset] = EOS;
                        // assert(speakoffset == strlen(SpeakBuffer));
                        if (sound && hostvolume & strcmp((char*) SpeakBuffer, "?")) // so it doesn't try to speak the "?" error message, which would abort speaking the user's input
                        {   // assert(speakoffset);
                            speak((STRPTR) SpeakBuffer);
                        }
                        speakoffset = 0;
}   }   }   }   }   }

MODULE void scroll_vdu(void)
{   int x, y;

    for (y = 0; y < vdu_rows_total - 1; y++)
    {   for (x = 0; x < vdu_columns; x++)
        {   vdu[x][y] = vdu[x][y + 1];
    }   }
    for (x = 0; x < vdu_columns; x++)
    {   vdu[x][vdu_rows_total - 1] = blank;
}   }

MODULE STRPTR suffix(int number)
{   number %= 100;

    if (number >= 4 && number <= 13)
    {   return "th";
    } else
    {   switch (number % 10)
        {
        case  1:  return "st";
        acase 2:  return "nd";
        acase 3:  return "rd";
        adefault: return "th";
}   }   }

EXPORT void draw_margins(void)
{   FAST int x, y,
             y1, y2, y3, y4;

    switch (machine)
    {
    case ARCADIA:
    case INTERTON:
    case ELEKTOR:
        for (y = 0; y < machines[machine].height; y++)
        {   for (x = 0; x < machines[machine].width; x++)
            {   screen[x][y] = GREY1;
        }   }
        if (!usemargins)
        {   return;
        }
        switch (region)
        {
        case REGION_PAL:
            for (x = 6; x < 227; x++)
            {   SETPIXEL(    x,  0, blankscreen_2621[ 0][x]);
            }
            for (y = 1; y <= 42; y++)
            {   for (x = 0; x < 227; x++)
                {   SETPIXEL(x,  y, blankscreen_2621[ y][x]);
            }   }
            for (x = 0; x <= 48; x++)
            {   SETPIXEL(    x, 43, blankscreen_2621[43][x]);
            }
            for (y = 44; y < 312; y++)
            {   for (x = 6; x <= 48; x++)
                {   SETPIXEL(x,  y, blankscreen_2621[ y][x]);
            }   }
        acase REGION_NTSC:
            for (x = 6; x < 227; x++)
            {   SETPIXEL(    x,  0, blankscreen_2622[ 0][x]);
            }
            for (y = 1; y <= 19; y++)
            {   for (x = 0; x < 227; x++)
                {   SETPIXEL(x,  y, blankscreen_2622[ y][x]);
            }   }
            for (x = 0; x <= 48; x++)
            {   SETPIXEL(    x, 20, blankscreen_2622[20][x]);
            }
            for (y = 21; y < 262; y++)
            {   for (x = 6; x <= 48; x++)
                {   SETPIXEL(x,  y, blankscreen_2622[ y][x]);
        }   }   }
    acase PIPBUG:
        if (showleds)
        {   for (x = 0; x < machines[machine].width; x++)
            {   SETPIXEL(x, machines[machine].height - PIPBUG_LEDHEIGHT    , GREY1);
                SETPIXEL(x, machines[machine].height - PIPBUG_LEDHEIGHT + 1, GREY1);
                for (y = machines[machine].height - PIPBUG_LEDHEIGHT + 2; y < machines[machine].height; y++)
                {   screen[x][y] = BLACK; // not cy_bgc!
        }   }   }
        if (!usemargins || pipbug_vdu != VDU_RADIOBULLETIN)
        {   return;
        }
        for (y = 0; y <= 42; y++)
        {   for (x = 0; x <= 83; x++)
            {   SETPIXEL(x, y, RED);             // horizontal back porch  + vertical back porch
            }
            for (x = 84; x <= 433; x++)
            {   SETPIXEL(x, y, PINK);            //                          vertical back porch (43 lines)
            }
            for (x = 434; x <= 482; x++)
            {   SETPIXEL(x, y, RED);             // horizontal front porch + vertical back porch
            }
            for (x = 483; x <= 517; x++)
            {   SETPIXEL(x, y, BLUE);            // horizontal retrace     + vertical back porch
        }   }
        for (y = 43; y <= 298; y++)
        {   for (x = 0; x <= 83; x++)
            {   SETPIXEL(x, y, ORANGE);          // horizontal back  porch (12*7=84 pixels)
            }
            for (x = 434; x <= 482; x++)
            {   SETPIXEL(x, y, ORANGE);          // horizontal front porch ( 7*7=49 pixels)
            }
            for (x = 483; x <= 517; x++)
            {   SETPIXEL(x, y, CYAN);            // horizontal retrace     ( 5*7=35 pixels)
        }   }
        for (y = 299; y <= 309; y++)
        {   for (x = 0; x <= 83; x++)
            {   SETPIXEL(x, y, RED);             // horizontal back porch  + vertical front porch
            }
            for (x = 84; x <= 433; x++)
            {   SETPIXEL(x, y, PINK);            //                          vertical front porch (11 lines)
            }
            for (x = 434; x <= 482; x++)
            {   SETPIXEL(x, y, RED);             // horizontal front porch + vertical front porch
            }
            for (x = 483; x <= 517; x++)
            {   SETPIXEL(x, y, BLUE);            // horizontal retrace     + vertical front porch
        }   }
        for (y = 310; y <= 312; y++)
        {   for (x = 0; x <= 83; x++)
            {   SETPIXEL(x, y, DARKRED);         // horizontal back porch  + vertical retrace
            }
            for (x = 84; x <= 433; x++)
            {   SETPIXEL(x, y, PURPLE);          //                          vertical retrace (3 lines)
            }
            for (x = 434; x <= 482; x++)
            {   SETPIXEL(x, y, DARKRED);         // horizontal front porch + vertical retrace
            }
            for (x = 483; x <= 517; x++)
            {   SETPIXEL(x, y, DARKBLUE);        // horizontal retrace     + vertical retrace
        }   }
    acase BINBUG:
        if (showleds)
        {   for (y = 0; y < machines[machine].height - BINBUG_LEDHEIGHT; y++)
            {   for (x = 0; x < machines[machine].width; x++)
                {   screen[x][y] = inverse ? WHITE : BLACK;
            }   }
            for (x = 0; x < machines[machine].width; x++)
            {   SETPIXEL(x, machines[machine].height - BINBUG_LEDHEIGHT    , GREY1);
                SETPIXEL(x, machines[machine].height - BINBUG_LEDHEIGHT + 1, GREY1);
                for (y = machines[machine].height - BINBUG_LEDHEIGHT + 2; y < machines[machine].height; y++)
                {   screen[x][y] = BLACK;
        }   }   }
        else
        {   for (y = 0; y < machines[machine].height; y++)
            {   for (x = 0; x < machines[machine].width; x++)
                {   screen[x][y] = inverse ? WHITE : BLACK;
        }   }   }

        if (!usemargins)
        {   return;
        }

        for (y = 0; y <= 255; y++)
        {   for (x = 576; x <= 767; x++)
            {   if (x >= 642 && x <= 701)
                {   SETPIXEL(x, y, CYAN);        // horizontal retrace
                } else
                {   SETPIXEL(x, y, ORANGE);      // horizontal porch
        }   }   }

        for (y = 256; y <= 312; y++)
        {   for (x = 0; x <= 767; x++)
            {   if
                (   (y == 281 && x >= 696) //   72 pixels
                 || (y >= 282 && y <= 285) // 3072 pixels
                 || (y == 286 && x <= 455) //  456 pixels
                )
                {   if (x >= 576)
                    {   if (x >= HPULSE_FIRST && x <= HPULSE_LAST)
                        {   SETPIXEL(x, y, DARKBLUE); // horizontal retrace + vertical retrace
                        } else
                        {   SETPIXEL(x, y, DARKRED);  // horizontal porch   + vertical retrace
                    }   }
                    else
                    {   SETPIXEL(    x, y, PURPLE);   //                      vertical retrace
                }   }
                elif (y == 312 && x >= 384)
                {   SETPIXEL(        x, y, GREY1);    // nonexistent half-scanline
                } else
                {   if (x >= 576)
                    {   if (x >= HPULSE_FIRST && x <= HPULSE_LAST)
                        {   SETPIXEL(x, y, BLUE);     // horizontal retrace + vertical porch
                        } else
                        {   SETPIXEL(x, y, RED);      // horizontal porch   + vertical porch
                    }   }
                    else
                    {   SETPIXEL(    x, y, PINK);     //                      vertical porch
        }   }   }   }
    acase TWIN:
        for (y = 0; y < machines[machine].height; y++)
        {   for (x = 0; x < machines[machine].width; x++)
            {   screen[x][y] = inverse ? WHITE : BLACK;
        }   }
        if (!usemargins)
        {   return;
        }

        for (y = 0; y <= 249; y++)
        {   for (x = 560; x <= 699; x++)
            {   SETPIXEL(x, y, ORANGE);          // horizontal porch
        }   }
        for (y = 250; y <= 269; y++)
        {   for (x = 0; x <= 559; x++)
            {   SETPIXEL(x, y, PINK);            //                      vertical porch
            }
            for (x = 560; x <= 699; x++)
            {   SETPIXEL(x, y, RED);             // horizontal porch   + vertical porch
        }   }
    acase CD2650:
        for (y = 0; y < machines[machine].height; y++)
        {   for (x = 0; x < machines[machine].width; x++)
            {   screen[x][y] = inverse ? WHITE : BLACK;
        }   }
        if (!usemargins)
        {   return;
        }

        for (y = 0; y <= 191; y++)
        {   for (x = 640; x <= 903; x++)
            {   if (x >= 712 && x <= 775)
                {   SETPIXEL(x, y, CYAN);        // horizontal retrace
                } else
                {   SETPIXEL(x, y, ORANGE);      // horizontal porch
        }   }   }

        for (y = 192; y <= 263; y++)
        {   for (x = 0; x <= 639; x++)
            {   if (y >= 216 && y <= 227)
                {   SETPIXEL(x, y, PURPLE);      //                      vertical retrace
                } else
                {   SETPIXEL(x, y, PINK);        //                      vertical porch
            }   }

            for (x = 640; x <= 903; x++)
            {   if (y >= 216 && y <= 227)
                {   if (x >= 712 && x <= 775)
                    {   SETPIXEL(x, y, DARKBLUE); // horizontal retrace + vertical retrace
                    } else
                    {   SETPIXEL(x, y, DARKRED);  // horizontal porch   + vertical retrace
                }   }
                else
                {   if (x >= 712 && x <= 775)
                    {   SETPIXEL(x, y, BLUE);     // horizontal retrace + vertical porch
                    } else
                    {   SETPIXEL(x, y, RED);      // horizontal porch   + vertical porch
        }   }   }   }
    acase PHUNSY:
        if (showleds)
        {   for (y = 0; y < machines[machine].height - PHUNSY_LEDHEIGHT; y++)
            {   for (x = 0; x < machines[machine].width; x++)
                {   screen[x][y] = inverse ? WHITE : BLACK;
            }   }
            for (x = 0; x < machines[machine].width; x++)
            {   SETPIXEL(x, machines[machine].height - PHUNSY_LEDHEIGHT    , GREY1);
                SETPIXEL(x, machines[machine].height - PHUNSY_LEDHEIGHT + 1, GREY1);
                for (y = machines[machine].height - PHUNSY_LEDHEIGHT + 2; y < machines[machine].height; y++)
                {   screen[x][y] = BLACK;
        }   }   }
        else
        {   for (y = 0; y < machines[machine].height; y++)
            {   for (x = 0; x < machines[machine].width; x++)
                {   screen[x][y] = inverse ? WHITE : BLACK;
        }   }   }

        if (!usemargins)
        {   return;
        }

        for (y = 0; y <= 255; y++)
        {   for (x = 0; x <= 127; x++)
            {   if (x >= 26 && x <= 57)
                {   SETPIXEL(x, y, CYAN);         // horizontal retrace
                } elif (x >= 66 && x <= 81)
                {   SETPIXEL(x, y, WHITE);        // colourburst
                } else
                {   SETPIXEL(x, y, ORANGE);       // horizontal porch
        }   }   }

        for (y = 256; y <= 312; y++)
        {   for (x = 128; x <= 511; x++)
            {   if (y >= 269 && y <= 272)
                {   SETPIXEL(x, y, PURPLE);       //                      vertical retrace
                } else
                {   SETPIXEL(x, y, PINK);         //                      vertical porch
            }   }

            for (x = 0; x <= 127; x++)
            {   if (y >= 269 && y <= 272)
                {   if (x >= 26 && x <= 57)
                    {   SETPIXEL(x, y, DARKBLUE); // horizontal retrace + vertical retrace
                    } elif (x >= 66 && x <= 81)
                    {   SETPIXEL(x, y, BLACK);    // colourburst        + vertical retrace
                    } else
                    {   SETPIXEL(x, y, DARKRED);  // horizontal porch   + vertical retrace
                }   }
                else
                {   if (x >= 26 && x <= 57)
                    {   SETPIXEL(x, y, BLUE);     // horizontal retrace + vertical porch
                    } elif (x >= 66 && x <= 81)
                    {   SETPIXEL(x, y, GREY1);    // colourburst        + vertical porch
                    } else
                    {   SETPIXEL(x, y, RED);      // horizontal porch   + vertical porch
        }   }   }   }
    acase SELBST:
        if (showleds)
        {   for (x = 0; x < machines[machine].width; x++)
            {   SETPIXEL(x, machines[machine].height - SELBST_LEDHEIGHT    , GREY1);
                SETPIXEL(x, machines[machine].height - SELBST_LEDHEIGHT + 1, GREY1);
                for (y = machines[machine].height - SELBST_LEDHEIGHT + 2; y < machines[machine].height; y++)
                {   screen[x][y] = BLACK; // not cy_bgc!
        }   }   }
    acase PONG:
        for (y = 0; y < machines[machine].height; y++)
        {   for (x = 0; x < machines[machine].width; x++)
            {   screen[x][y] = GREY1;
        }   }
        if (!usemargins)
        {   return;
        }
        if (region == REGION_PAL)
        {   y1 =  43; // end of vertical back  porch
            y2 = 275; // end of main drawing area
            y3 = 305; // end of vertical front porch
            y4 = 311; // end of vertical retrace
        } else
        {   y1 =  41; // end of vertical back  porch
            y2 = 233; // end of main drawing area
            y3 = 257; // end of vertical front porch
            y4 = 261; // end of vertical retrace
        }
        for (y =      0; y <= y3; y++)
        {   if (y <= y1 || y > y2)     // vertical porches
            {   for (x =      0    ; x <  ponglt; x++) { SETPONGPIXEL(x, y, RED);      } // horizontal back  porch   + vertical porch. The braces are needed!
                if (colourset != GREYSCALE)
                {   if (region == REGION_PAL)
                    {   for (x = PONG_CBSTART_PAL ; x <= PONG_CBSTART_PAL  + 9; x++) { SETPONGPIXEL(x, y, GREY1); } // colourburst
                    } else
                    {   for (x = PONG_CBSTART_NTSC; x <= PONG_CBSTART_NTSC + 9; x++) { SETPONGPIXEL(x, y, GREY1); } // colourburst
                }   }
                for (x = ponglt    ; x <= pongrt; x++) { SETPONGPIXEL(x, y, PINK);     } //                            vertical porch
                for (x = pongrt + 1; x <=    118; x++) { SETPONGPIXEL(x, y, RED);      } // horizontal front porch   + vertical porch
                for (x =    119    ; x <=    127; x++) { SETPONGPIXEL(x, y, BLUE);     } // horizontal retrace       + vertical porch
        }   }
        for (y = y1 + 1; y <= y2; y++) // main drawing area
        {   for     (x =      0    ; x <  ponglt; x++) { SETPONGPIXEL(x, y, ORANGE);   } // horizontal back  porch
            if (colourset != GREYSCALE)
            {   if (region == REGION_PAL)
                {   for (x = PONG_CBSTART_PAL ; x <= PONG_CBSTART_PAL  + 9; x++) { SETPONGPIXEL(x, y, WHITE);    } // colourburst
                } else
                {   for (x = PONG_CBSTART_NTSC; x <= PONG_CBSTART_NTSC + 9; x++) { SETPONGPIXEL(x, y, WHITE);    } // colourburst
            }   }
            for     (x = pongrt + 1; x <=    118; x++) { SETPONGPIXEL(x, y, ORANGE);   } // horizontal front porch
            for     (x =    119    ; x <=    127; x++) { SETPONGPIXEL(x, y, CYAN);     } // horizontal retrace
        }
        for (y = y3 + 1; y <= y4; y++) // vertical retrace
        {   for     (x =      0    ; x <  ponglt; x++) { SETPONGPIXEL(x, y, DARKRED);  } // horizontal porch   + vertical retrace
            if (colourset != GREYSCALE)
            {   if (region == REGION_PAL)
                {   for (x = PONG_CBSTART_PAL ; x <= PONG_CBSTART_PAL  + 9; x++) { SETPONGPIXEL(x, y, BLACK); } // colourburst
                } else
                {   for (x = PONG_CBSTART_NTSC; x <= PONG_CBSTART_NTSC + 9; x++) { SETPONGPIXEL(x, y, BLACK); } // colourburst
            }   }
            for     (x = ponglt    ; x <= pongrt; x++) { SETPONGPIXEL(x, y, PURPLE);   } //                      vertical retrace
            for     (x = pongrt + 1; x <=    118; x++) { SETPONGPIXEL(x, y, DARKRED);  } // horizontal porch   + vertical retrace
            for     (x =    119    ; x <=    127; x++) { SETPONGPIXEL(x, y, DARKBLUE); } // horizontal retrace + vertical retrace
}   }   }

EXPORT void gfx_setup(void)
{   int x, y;

    // left column
    for (x = 0; x <= 5; x++)
    {   for (y = 1; y <= 43; y++)
        {   blankscreen_2621[y][x] = (y >= 13 && y <= 15) ? PURPLE : PINK;
    }   }
    for (x = 0; x <= 9; x++)
    {   for (y = 1; y <= 20; y++)
        {   blankscreen_2622[y][x] = (y >=  4 && y <=  6) ? PURPLE : PINK;
    }   }

    // hblank column
    for (x = 6; x <= 48; x++)
    {   for (y = 0; y < 312; y++)
        {   if
            (   (y >= 13 && y <=  14)
             || (y == 15 && x <=  10)
            )
            {   blankscreen_2621[y][x] = hbc_vs[REGION_PAL][x];
            } elif (y <= 42)
            {   blankscreen_2621[y][x] = hbc_vrst[REGION_PAL][x];
            } else
            {   blankscreen_2621[y][x] = hbc_normal[REGION_PAL][x];
    }   }   }
    for (x = 10; x <= 48; x++)
    {   for (y = 0; y < 262; y++)
        {   if
            (   (y == 3 && x >= 32)
             ||  y == 4
             ||  y == 5
             || (y == 6 && x <= 31)
            )
            {   blankscreen_2622[y][x] = hbc_vs[REGION_NTSC][x];
            } elif
            (    y <= 19
             || (y == 20 && x <= 9)
            )
            {   blankscreen_2622[y][x] = hbc_vrst[REGION_NTSC][x];
            } else
            {   blankscreen_2622[y][x] = hbc_normal[REGION_NTSC][x];
    }   }   }

    // main column
    for (x = 49; x < 227; x++)
    {   for (y = 0; y <= 42; y++)
        {   if
            (   (y == 12 && x >= 113)
             ||  y == 13
             ||  y == 14
            )
            {   blankscreen_2621[y][x] = PURPLE;
            } else
            {   blankscreen_2621[y][x] = PINK;
        }   }
        for (y = 0; y <= 19; y++)
        {   if (y >= 3 && y <= 5)
            {   blankscreen_2622[y][x] = PURPLE;
            } else
            {   blankscreen_2622[y][x] = PINK;
}   }   }   }

EXPORT void fixupcolours(void)
{   FAST int x, y;

    switch (machine)
    {
    case INSTRUCTOR:
    case MIKIT:
        vdu_bgc = BLACK;
    acase PIPBUG:
    case BINBUG:
    case CD2650:
    case PHUNSY:
    case SELBST:
        if (inverse)
        {   vdu_fgc = BLACK;
            vdu_bgc = WHITE;
        } else
        {   vdu_fgc = WHITE;
            vdu_bgc = BLACK;
        }
        if (machine == PIPBUG && pipbug_vdu == VDU_ELEKTERMINAL)
        {   vdu_bgc = GREY4; // closest available grey to $888888
        }
    acase TYPERIGHT:
        if
        (   inverse
#ifdef WIN32
         || (enhancestars && !pixelulong)
#endif
        )
        {   vdu_fgc = WHITE;
        } else
        {   vdu_fgc = BLACK;
        }
        vdu_bgc = GREY5;
    adefault: // ARCADIA, INTERTON, ELEKTOR, coin-ops, PONG
        vdu_bgc = GREY1;
    }

    if
    (   machine == INSTRUCTOR
     || machine == CD2650
     || machine == MIKIT
    )
    {   for (x = 0; x < machines[machine].width; x++)
        {   for (y = 0; y < machines[machine].height; y++)
            {   screen[x][y] = (UBYTE) vdu_bgc; // NOT changepixel()!
    }   }   }

    draw_margins();

#ifdef AMIGA
    changecolours();
    if (SubWindowPtr[SUBWINDOW_PALETTE])
    {   update_sliders();
        updatewheel();
        updatebrightness();
    }
    fixdebuggercolour();
#endif
#ifdef WIN32
    if (SubWindowPtr[SUBWINDOW_PALETTE])
    {   update_palette(TRUE);
    }
#endif
}

EXPORT ULONG setkybdcolour(int gid)
{   int   found = 0,
          i, j, k,
          matches;
    ULONG emupen,
          maybeswapped;

    if (machines[machine].joystick)
    {   maybeswapped = swapped;
    } else
    {   maybeswapped = FALSE;
    }

    for (i = 0; i < SCANCODES; i++)
    {   if (gid == keyname[i].gadget)
        {   if
            (   ISQWERTY
             && (   keyname[i].unshifted[KEYMAP_HOST] != NC
                 || i == SCAN_TAB
                 || i == SCAN_CAPSLOCK
                 || i == SCAN_LSHIFT
                 || i == SCAN_RSHIFT
#ifdef WIN32
                 || (machine != TYPERIGHT && (i == SCAN_LCTRL || i == SCAN_RCTRL))
#endif
#ifdef AMIGA
                 || (machine != TYPERIGHT && (i == SCAN_CTRL                    ))
#endif
                )
             && (machine != TYPERIGHT || i != SCAN_ESCAPE)
            )
            {   emupen = EMUPEN_GREEN;
                matches = 1;
            } elif (keyname[i].reserved)
            {   emupen = EMUPEN_WHITE;
                // matches = 1; (dead assignment)
                return emupen;
            } else
            {   emupen = EMUPEN_GREY;
                matches = 0;
            }
            if (ISQWERTY && machine != TYPERIGHT)
            {   return emupen;
            }

            if (machine == TYPERIGHT)
            {   k = -1;
                for (j = 0; j < 4; j++)
                {   if (console[j] == i)
                    {   k = j;
                        matches++;
                }   }
                if (k != -1)
                {   if (matches >= 2)
                    {   return EMUPEN_PURPLE;
                    } else
                    {   // assert(matches == 1);
                        return EMUPEN_GREEN;
                }   }
                return emupen;
            }

            for (j = 0; j < 4; j++)
            {   if (console[j] == i && machine != SELBST)
                {   found++;
            }   }
            for (j = 0; j < 2; j++)
            {   for (k = 0; k < NUMKEYS; k++)
                {   if (keypads[j][k] == i && keyexists(k))
                    {   found++;
            }   }   }
            if (found == 0)
            {   return EMUPEN_GREY;
            } // implied else
            if (found >= 2)
            {   return EMUPEN_PURPLE;
            }

            for (j = 0; j < 4; j++)
            {   if (console[j] == i)
                {   return EMUPEN_GREEN;
            }   }
            for (j = 0; j < NUMKEYS; j++)
            {   if (keypads[0][j] == i && keyexists(j))
                {   return (ULONG) (maybeswapped ? EMUPEN_BLUE : EMUPEN_RED);
                }
                if (keypads[1][j] == i && keyexists(j))
                {   return (ULONG) (maybeswapped ? EMUPEN_RED : EMUPEN_BLUE);
    }   }   }   }

    return 0; // key not found in diagram
}

EXPORT void changepixel(int x, int y, int colour)
{
#ifdef CHECKDRAWS
    if (x < 0 || y < 0 || x >= machines[machine].width || y >= machines[machine].height)
    {   zprintf
        (   TEXTPEN_ERROR,
            "changepixel(): illegal coords: %d,%d! Valid coords are 0,0..%d,%d.\n",
            x,
            y,
            machines[machine].width  - 1,
            machines[machine].height - 1
        );
        return;
    }
#endif

    if (screen[x][y] != (UBYTE) colour)
    {   screen[x][y] = (UBYTE) colour;
        drawpixel(x, y, colour);
#ifdef WIN32
        fgtable[y][x] = 1;
#endif
}   }

EXPORT void changebgpixel(int x, int y, int colour)
{
#ifdef CHECKDRAWS
    if (x < 0 || y < 0 || x >= machines[machine].width || y >= machines[machine].height)
    {   zprintf
        (   TEXTPEN_ERROR,
            "changepixel(): illegal coords: %d,%d! Valid coords are 0,0..%d,%d.\n",
            x,
            y,
            machines[machine].width  - 1,
            machines[machine].height - 1
        );
        return;
    }
#endif

    if (screen[x][y] != (UBYTE) colour)
    {   screen[x][y] = (UBYTE) colour;
#ifdef AMIGA
        drawpixel(x, y, colour);
#endif
#ifdef WIN32
        drawbgpixel(x, y, colour);
        fgtable[y][x] = 0;
#endif
}   }

EXPORT void drawglow(int x, int y, int colour)
{   if
    (   !showleds
     && (   machine == PIPBUG
         || machine == BINBUG
         || machine == PHUNSY
         || machine == SELBST
    )   )
    {   return;
    }

    changepixel(x + 1, y    , colour);
    changepixel(x + 2, y    , colour);
    changepixel(x + 3, y    , colour);
    changepixel(x + 4, y    , colour);
    changepixel(x    , y + 1, colour);
    changepixel(x + 1, y + 1, colour);
    changepixel(x + 2, y + 1, colour);
    changepixel(x + 3, y + 1, colour);
    changepixel(x + 4, y + 1, colour);
    changepixel(x + 5, y + 1, colour);
    changepixel(x    , y + 2, colour);
    changepixel(x + 1, y + 2, colour);
    changepixel(x + 2, y + 2, colour);
    changepixel(x + 3, y + 2, colour);
    changepixel(x + 4, y + 2, colour);
    changepixel(x + 5, y + 2, colour);
    changepixel(x    , y + 3, colour);
    changepixel(x + 1, y + 3, colour);
    changepixel(x + 2, y + 3, colour);
    changepixel(x + 3, y + 3, colour);
    changepixel(x + 4, y + 3, colour);
    changepixel(x + 5, y + 3, colour);
    changepixel(x    , y + 4, colour);
    changepixel(x + 1, y + 4, colour);
    changepixel(x + 2, y + 4, colour);
    changepixel(x + 3, y + 4, colour);
    changepixel(x + 4, y + 4, colour);
    changepixel(x + 5, y + 4, colour);
    changepixel(x + 1, y + 5, colour);
    changepixel(x + 2, y + 5, colour);
    changepixel(x + 3, y + 5, colour);
    changepixel(x + 4, y + 5, colour);
}

EXPORT void drawdigit(int position, UBYTE value)
{   PERSIST const UBYTE pipbug_digitglyphs[16] = // we have made them the same as for Instructor
    {   0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }, //  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
    mikit_digitglyphs[128] =
    {   0x5F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x3D, 0x76, 0x06, 0x1E, 0x76, 0x38, 0x37, 0x37, 0x3F, // $0x @ABCDEFGHIJKLMNO
        0x73, 0x67, 0x77, 0x6D, 0x31, 0x3E, 0x3E, 0x3E, 0x76, 0x6E, 0x5B, 0x39, 0x64, 0x0F, 0x23, 0x08, // $1x PQRSTUVWXYZ[\]^_
        0x00, 0x06, 0x22, 0x3F, 0x6D, 0x52, 0x23, 0x02, 0x39, 0x0F, 0x76, 0x46, 0x0C, 0x40, 0x08, 0x52, // $2x  !"#$%^'()*+,-./
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67, 0x41, 0x45, 0x39, 0x09, 0x0F, 0x53, // $3x 0123456789:;<=>?
        0x5F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x3D, 0x76, 0x06, 0x1E, 0x76, 0x38, 0x37, 0x37, 0x3F, // $4x @ABCDEFGHIJKLMNO
        0x73, 0x67, 0x77, 0x6D, 0x31, 0x3E, 0x3E, 0x3E, 0x76, 0x6E, 0x5B, 0x39, 0x64, 0x0F, 0x23, 0x08, // $5x PQRSTUVWXYZ[\]^_
        0x20, 0x5F, 0x7C, 0x58, 0x5E, 0x7B, 0x71, 0x6F, 0x74, 0x04, 0x0E, 0x76, 0x06, 0x54, 0x54, 0x5C, // $6x `abcdefghijklmno
        0x73, 0x67, 0x50, 0x6D, 0x78, 0x1C, 0x1C, 0x1C, 0x76, 0x6E, 0x5B, 0x39, 0x30, 0x0F, 0x62, 0x7F, // $7x pqrstuvwxyz{|}~#
    }; //  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F

    // assert(machine == PIPBUG || machine == MIKIT);

    // value is the number to draw ($0..$F)
    // now we convert this into the raw segments

    switch (machine)
    {
    case  PIPBUG: digitleds[position] = pipbug_digitglyphs[value];
    acase MIKIT:  digitleds[position] = mikit_digitglyphs[value];
    }
    drawsegments(position);
}

EXPORT void drawsegments(int position)
{   FAST int i;

    // assert(machine == PIPBUG || machine == INSTRUCTOR || machine == MIKIT || machine == SELBST);

    for (i = 0; i <= 13; i++)
    {   drawsegment(position, i, BLACK);
    }

    if (drawcorners)
    {   if (digitleds[position] & 0x21) // top left
        {   drawsegment(position, 8, RED);
        } elif (drawunlit)
        {   drawsegment(position, 8, UNLIT);
        }
        if (digitleds[position] & 0x03) // top right
        {   drawsegment(position, 9, RED);
        } elif (drawunlit)
        {   drawsegment(position, 9, UNLIT);
        }
        if (digitleds[position] & 0x70) // middle left
        {   drawsegment(position, 10, RED);
        } elif (drawunlit)
        {   drawsegment(position, 10, UNLIT);
        }
        if (digitleds[position] & 0x46) // middle right
        {   drawsegment(position, 11, RED);
        } elif (drawunlit)
        {   drawsegment(position, 11, UNLIT);
        }
        if (digitleds[position] & 0x18) // bottom left
        {   drawsegment(position, 12, RED);
        } elif (drawunlit)
        {   drawsegment(position, 12, UNLIT);
        }
        if (digitleds[position] & 0x0C) // bottom right
        {   drawsegment(position, 13, RED);
        } elif (drawunlit)
        {   drawsegment(position, 13, UNLIT);
    }   }

    for (i = 0; i <= 7; i++)
    {   if
        (   i == 7 // decimal point
         && (   (machine == PIPBUG && position >= 4)
             ||  machine == SELBST
             ||  machine == MIKIT
        )   )
        {   break;
        }

        if (digitleds[position] & (1 << i))
        {   drawsegment(position, i, RED);
        } elif (drawunlit)
        {   drawsegment(position, i, UNLIT);
}   }   }

EXPORT void drawsegment(int position, int segment, int colour)
{   FAST int controlsx,
             x, xx, xxx,
             y, yy;

    // assert(machine == PIPBUG || machine == INSTRUCTOR || machine == MIKIT || machine == SELBST);

    controlsx = 9 * position;
#ifdef WIDEINSTRUCTOR
    if (machine == INSTRUCTOR)
    {   x   = 33 * position;
        xxx = 6;
    } else
#endif
    {   x   = 21 * position;
        xxx = 0;
    }
    switch (machine)
    {
    case PIPBUG:
        if (pipbug_vdu == VDU_LCVDU_NARROW)
        {   x += (machines[PIPBUG].width  - 162) / 2;
        } else
        {   x += (machines[PIPBUG].width  - 330) / 2;
        }
        if (position >= 4)
        {   x += 21;
            if (position >= 7)
            {   x += 21;
                if (position >= 10)
                {   x += 21;
        }   }   }
        y =   machines[PIPBUG].height -  35;
    acase INSTRUCTOR:
        x +=  10;
        y =   10;
    acase SELBST:
        x += 195;
        y =  136;
    acase MIKIT:
        if (position >= 4)
        {   x += 42;
        } else
        {   x += 21;
        }
        y =   10;
    }

    switch (segment)
    {
    case 0: // top side
        if (!drawcorners)
        {   segment_drawpixel(x +  2      ,  y +  1, colour);
            segment_drawpixel(x + 11 + xxx,  y +  1, colour);
        }
        for (xx =  3; xx <= 10 + xxx; xx++)
        {   segment_drawpixel(x + xx,  y     , colour);
            segment_drawpixel(x + xx,  y +  1, colour);
            segment_drawpixel(x + xx,  y +  2, colour);
        }

        drawcontrolspixel(controlsx + 1, 0, colour);
        drawcontrolspixel(controlsx + 2, 0, colour);
        drawcontrolspixel(controlsx + 3, 0, colour);
        drawcontrolspixel(controlsx + 4, 0, colour);
    acase 1: // top right side
        if (!drawcorners)
        {   segment_drawpixel(x + 12 + xxx,  y +  2, colour);
            segment_drawpixel(x + 12 + xxx,  y + 13, colour);
        }
        for (yy =  3; yy <= 12; yy++)
        {   segment_drawpixel(x + 11 + xxx,  y + yy, colour);
            segment_drawpixel(x + 12 + xxx,  y + yy, colour);
            segment_drawpixel(x + 13 + xxx,  y + yy, colour);
        }

        drawcontrolspixel(controlsx + 5, 1, colour);
        drawcontrolspixel(controlsx + 5, 2, colour);
        drawcontrolspixel(controlsx + 5, 3, colour);
    acase 2: // bottom right side
        if (!drawcorners)
        {   segment_drawpixel(x + 12 + xxx, y + 15, colour);
            segment_drawpixel(x + 12 + xxx, y + 26, colour);
        }
        for (yy = 16; yy <= 25; yy++)
        {   segment_drawpixel(x + 11 + xxx, y + yy, colour);
            segment_drawpixel(x + 12 + xxx, y + yy, colour);
            segment_drawpixel(x + 13 + xxx, y + yy, colour);
        }
        drawcontrolspixel(controlsx + 5, 5, colour);
        drawcontrolspixel(controlsx + 5, 6, colour);
        drawcontrolspixel(controlsx + 5, 7, colour);
    acase 3: // bottom side
        if (!drawcorners)
        {   segment_drawpixel(x +  2      , y + 27, colour);
            segment_drawpixel(x + 11 + xxx, y + 27, colour);
        }
        for (xx =  3; xx <= 10 + xxx; xx++)
        {   segment_drawpixel(x + xx, y + 26, colour);
            segment_drawpixel(x + xx, y + 27, colour);
            segment_drawpixel(x + xx, y + 28, colour);
        }
        drawcontrolspixel(controlsx + 1, 8, colour);
        drawcontrolspixel(controlsx + 2, 8, colour);
        drawcontrolspixel(controlsx + 3, 8, colour);
        drawcontrolspixel(controlsx + 4, 8, colour);
    acase 4: // bottom left side
        if (!drawcorners)
        {   segment_drawpixel(x + 1, y + 15, colour);
            segment_drawpixel(x + 1, y + 26, colour);
        }
        for (yy = 16; yy <= 25; yy++)
        {   segment_drawpixel(x    , y + yy, colour);
            segment_drawpixel(x + 1, y + yy, colour);
            segment_drawpixel(x + 2, y + yy, colour);
        }
        drawcontrolspixel(controlsx    , 5, colour);
        drawcontrolspixel(controlsx    , 6, colour);
        drawcontrolspixel(controlsx    , 7, colour);
    acase 5: // top left side
        if (!drawcorners)
        {   segment_drawpixel(x + 1,  y +  2, colour);
            segment_drawpixel(x + 1,  y + 13, colour);
        }
        for (yy =  3; yy <= 12; yy++)
        {   segment_drawpixel(x    ,  y + yy, colour);
            segment_drawpixel(x + 1,  y + yy, colour);
            segment_drawpixel(x + 2,  y + yy, colour);
        }
        drawcontrolspixel(controlsx    , 1, colour);
        drawcontrolspixel(controlsx    , 2, colour);
        drawcontrolspixel(controlsx    , 3, colour);
    acase 6: // middle "side"
        if (!drawcorners)
        {   segment_drawpixel(x +  2      ,  y + 14, colour);
            segment_drawpixel(x + 11 + xxx,  y + 14, colour);
        }
        for (xx =  3; xx <= 10 + xxx; xx++)
        {   segment_drawpixel(x + xx,  y + 13, colour);
            segment_drawpixel(x + xx,  y + 14, colour);
            segment_drawpixel(x + xx,  y + 15, colour);
        }
        drawcontrolspixel(controlsx + 1, 4, colour);
        drawcontrolspixel(controlsx + 2, 4, colour);
        drawcontrolspixel(controlsx + 3, 4, colour);
        drawcontrolspixel(controlsx + 4, 4, colour);
    acase 7: // decimal point (INSTRUCTOR, PIPBUG left group) (not PIPBUG right group, SELBST, MIKIT)
        segment_drawpixel(x + 16 + ((xxx * 3) / 2), y + 26, colour);
        segment_drawpixel(x + 17 + ((xxx * 3) / 2), y + 26, colour);
        segment_drawpixel(x + 18 + ((xxx * 3) / 2), y + 26, colour);
        segment_drawpixel(x + 16 + ((xxx * 3) / 2), y + 27, colour);
        segment_drawpixel(x + 17 + ((xxx * 3) / 2), y + 27, colour);
        segment_drawpixel(x + 18 + ((xxx * 3) / 2), y + 27, colour);
        segment_drawpixel(x + 16 + ((xxx * 3) / 2), y + 28, colour);
        segment_drawpixel(x + 17 + ((xxx * 3) / 2), y + 28, colour);
        segment_drawpixel(x + 18 + ((xxx * 3) / 2), y + 28, colour);
        drawcontrolspixel(controlsx + 7, 8, colour);
    acase 8: // top left corner
        segment_drawpixel(x     , y     , colour);
        segment_drawpixel(x +  1, y     , colour);
        segment_drawpixel(x +  2, y     , colour);
        segment_drawpixel(x     , y +  1, colour);
        segment_drawpixel(x +  1, y +  1, colour);
        segment_drawpixel(x +  2, y +  1, colour);
        segment_drawpixel(x     , y +  2, colour);
        segment_drawpixel(x +  1, y +  2, colour);
        segment_drawpixel(x +  2, y +  2, colour);
        drawcontrolspixel(controlsx,   0, colour);
    acase 9: // top right corner
        segment_drawpixel(x + 11 + xxx, y     , colour);
        segment_drawpixel(x + 12 + xxx, y     , colour);
        segment_drawpixel(x + 13 + xxx, y     , colour);
        segment_drawpixel(x + 11 + xxx, y +  1, colour);
        segment_drawpixel(x + 12 + xxx, y +  1, colour);
        segment_drawpixel(x + 13 + xxx, y +  1, colour);
        segment_drawpixel(x + 11 + xxx, y +  2, colour);
        segment_drawpixel(x + 12 + xxx, y +  2, colour);
        segment_drawpixel(x + 13 + xxx, y +  2, colour);
        drawcontrolspixel(controlsx + 5, 0, colour);
    acase 10: // middle left "corner"
        segment_drawpixel(x     , y + 13, colour);
        segment_drawpixel(x +  1, y + 13, colour);
        segment_drawpixel(x +  2, y + 13, colour);
        segment_drawpixel(x     , y + 14, colour);
        segment_drawpixel(x +  1, y + 14, colour);
        segment_drawpixel(x +  2, y + 14, colour);
        segment_drawpixel(x     , y + 15, colour);
        segment_drawpixel(x +  1, y + 15, colour);
        segment_drawpixel(x +  2, y + 15, colour);
        drawcontrolspixel(controlsx    , 4, colour);
    acase 11: // middle right "corner"
        segment_drawpixel(x + 11 + xxx, y + 13, colour);
        segment_drawpixel(x + 12 + xxx, y + 13, colour);
        segment_drawpixel(x + 13 + xxx, y + 13, colour);
        segment_drawpixel(x + 11 + xxx, y + 14, colour);
        segment_drawpixel(x + 12 + xxx, y + 14, colour);
        segment_drawpixel(x + 13 + xxx, y + 14, colour);
        segment_drawpixel(x + 11 + xxx, y + 15, colour);
        segment_drawpixel(x + 12 + xxx, y + 15, colour);
        segment_drawpixel(x + 13 + xxx, y + 15, colour);
        drawcontrolspixel(controlsx + 5, 4, colour);
    acase 12: // bottom left corner
        segment_drawpixel(x     , y + 26, colour);
        segment_drawpixel(x +  1, y + 26, colour);
        segment_drawpixel(x +  2, y + 26, colour);
        segment_drawpixel(x     , y + 27, colour);
        segment_drawpixel(x +  1, y + 27, colour);
        segment_drawpixel(x +  2, y + 27, colour);
        segment_drawpixel(x     , y + 28, colour);
        segment_drawpixel(x +  1, y + 28, colour);
        segment_drawpixel(x +  2, y + 28, colour);
        drawcontrolspixel(controlsx    , 8, colour);
    acase 13: // bottom right corner
        segment_drawpixel(x + 11 + xxx, y + 26, colour);
        segment_drawpixel(x + 12 + xxx, y + 26, colour);
        segment_drawpixel(x + 13 + xxx, y + 26, colour);
        segment_drawpixel(x + 11 + xxx, y + 27, colour);
        segment_drawpixel(x + 12 + xxx, y + 27, colour);
        segment_drawpixel(x + 13 + xxx, y + 27, colour);
        segment_drawpixel(x + 11 + xxx, y + 28, colour);
        segment_drawpixel(x + 12 + xxx, y + 28, colour);
        segment_drawpixel(x + 13 + xxx, y + 28, colour);
        drawcontrolspixel(controlsx + 5, 8, colour);
}   }

MODULE __inline void segment_drawpixel(int x, int y, int colour)
{   // assert(machine != PHUNSY);

    if
    (   !showleds
     && (   machine == PIPBUG
         || machine == BINBUG
         || machine == SELBST
    )   )
    {   return;
    }

#ifdef WIDEINSTRUCTOR
    if (machine == INSTRUCTOR)
    {   x += (28 + 1 - (y - 10)) / 4; // 0..28 -> 0..7
    }
#endif

    if (screen[x][y] != colour)
    {   screen[x][y] = (UBYTE) colour;
        drawpixel(x, y, colour);
}   }

EXPORT void drawcontrolspixel(int x, int y, int colour)
{   FAST int x1, x2;

    switch (machine)
    {
    case INSTRUCTOR:
        if (si50_bigctrls)
        {
#ifdef WIDEINSTRUCTOR
            x1 = (17 - (y * 2)) / 3;
            x2 = (16 - (y * 2)) / 3;
#else
            x1 = x2 = 0;
#endif
            drawcontrolspixel2(x1 + (x * 2)    ,  y * 2     , colour);
            drawcontrolspixel2(x1 + (x * 2) + 1,  y * 2     , colour);
            drawcontrolspixel2(x2 + (x * 2)    , (y * 2) + 1, colour);
            drawcontrolspixel2(x2 + (x * 2) + 1, (y * 2) + 1, colour);
        } else
        {
#ifdef WIDEINSTRUCTOR
            x1 = ((8 - y) / 3);
#else
            x1 = 0;
#endif
            drawcontrolspixel2(x1 + x          ,  y         , colour);
        }
    acase SELBST:
    case MIKIT:
        drawcontrolspixel2(         x * 2     ,  y * 2     , colour);
        drawcontrolspixel2(        (x * 2) + 1,  y * 2     , colour);
        drawcontrolspixel2(         x * 2     , (y * 2) + 1, colour);
        drawcontrolspixel2(        (x * 2) + 1, (y * 2) + 1, colour);
    acase PHUNSY:
        drawcontrolspixel2(        (x * 2) + 5,  y * 2     , colour);
        drawcontrolspixel2(        (x * 2) + 6,  y * 2     , colour);
        drawcontrolspixel2(        (x * 2) + 5, (y * 2) + 1, colour);
        drawcontrolspixel2(        (x * 2) + 6, (y * 2) + 1, colour);
    acase TYPERIGHT:
        drawcontrolspixel2(         x         ,  y         , colour);
}   }

MODULE void drawcontrolspixel2(int x, int y, int colour)
{   miniscreen[x][y] = (ASCREEN) colour;

#ifdef WIN32
    canvasdisplay[CANVAS_MINI][x + (y * MINIWIDTH)] = pencolours[PURECOLOURS][colour];
#endif
#ifdef AMIGA
    if (SubWindowPtr[SUBWINDOW_CONTROLS])
    {   switch (machine)
        {
        case INSTRUCTOR:
        case PHUNSY:
        case TYPERIGHT:
            *(canvasbyteptr[CANVAS_MINI][y] + x) = bytepens[colour];
        acase MIKIT:
        case SELBST:
            if (x % SPLITDISTANCE < SPLITWIDTH)
            {   *(canvasbyteptr[CANVAS_SPLIT1 + (x / SPLITDISTANCE)][y] + (x % SPLITDISTANCE)) = bytepens[colour];
    }   }   }
#endif
}

EXPORT void drawctrlglow(int leftx, int topy, FLAG lit) // Note that there are also global variables with the same names as these parameters.
{   // assert(SubWindowPtr[SUBWINDOW_CONTROLS]);
    // assert(machine == PIPBUG || machine == BINBUG || machine == INSTRUCTOR);

    if (machine == PIPBUG || machine == BINBUG)
    {   leftx += 153;
        topy  -= 396;
    }
    therect.left = leftx;
    therect.top  = topy;
    switch (machine)
    {
    case ARCADIA:
        if (arcadia_bigctrls)
        {   if (arcadia_viewcontrolsas == 0) // Emerson
            {   therect.right  = leftx + 5;
                therect.bottom = topy  + 5;
            } else
            {   // assert(arcadia_viewcontrolsas == 1); // MPT-03
                therect.right  = leftx + 2;
                therect.bottom = topy  + 2;
        }   }
        else
        {   therect.right  = leftx + 1;
            therect.bottom = topy  + 1;
        }
    acase PIPBUG:
    case BINBUG:
        therect.right      = leftx + 7;
        therect.bottom     = topy  + 7;
    acase INSTRUCTOR:
        if (si50_bigctrls)
        {   therect.right  = leftx + 6;
            therect.bottom = topy  + 6;
        } else
        {   therect.right  = leftx + 4;
            therect.bottom = topy  + 4;
    }   }

    drawctrlglow_system(lit);
}

EXPORT void view_controls_engine(void)
{   int i;
#ifdef WIN32
    int fillcolour,
        x, y;
#endif

    for (i = 0; i < KEYS; i++)
    {   oldkeys[i] =
        newkeys[i] = 0;
    }

    sx[0] =
    sx[1] =
    sy[0] =
    sy[1] = 128; // so we don't have any permanently lit paddles

    switch (machine)
    {
    case PIPBUG:
        switch (pipbug_vdu)
        {
        case VDU_RADIOBULLETIN:
            machines[machine].keys = 80;
        acase VDU_VT100:
            machines[machine].keys = 79;
        adefault:
            machines[machine].keys = 62;
        }
    acase INSTRUCTOR:
        if (si50_bigctrls)
        {   miniwidth  = SI50_MINIWIDTH_BIG;
            miniheight = SI50_MINIHEIGHT_BIG;
            minidestx  = 524;
            minidesty  =  69;
        } else
        {   miniwidth  = SI50_MINIWIDTH_SML;
            miniheight = SI50_MINIHEIGHT_SML;
            minidestx  = 301;
            minidesty  = 111;
        }
    acase PHUNSY:
        miniwidth  = PHUNSY_MINIWIDTH;
        miniheight = PHUNSY_MINIHEIGHT;
        minidestx  =  81;
        minidesty  =  17;
    acase SELBST:
        miniwidth  = SELBST_MINIWIDTH;
        miniheight = SELBST_MINIHEIGHT;
        // minidestx and minidesty will change for every digit
    acase MIKIT:
        miniwidth  = MIKIT_MINIWIDTH;
        miniheight = MIKIT_MINIHEIGHT;
        // minidestx and minidesty will change for every digit
    acase TYPERIGHT:
        miniwidth  = TYPERIGHT_MINIWIDTH;
        miniheight = TYPERIGHT_MINIHEIGHT;
        minidestx  = 230;
        minidesty  =  41;
    adefault:
        miniwidth  =
        miniheight =
        minidestx  =
        minidesty  = 0;
    }

#ifdef WIN32 // AmiArcadia does this in view_controls()
    fillcolour = (machine == TYPERIGHT) ? GREY1 : BLACK;
    for (y = 0; y < MINIHEIGHT; y++)
    {   for (x = 0; x < MINIWIDTH; x++)
        {   miniscreen[x][y] = fillcolour;
            canvasdisplay[CANVAS_MINI][x + (y * MINIWIDTH)] = pencolours[PURECOLOURS][fillcolour];
    }   }
#endif

    wheremouse = -1;
    fix_keyrects(); // probably not needed anymore
}
