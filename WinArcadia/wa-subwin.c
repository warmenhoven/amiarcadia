// INCLUDES---------------------------------------------------------------

#include "ibm.h"

#include <commctrl.h>
#include <richedit.h>

#include "resource.h"

typedef unsigned char bool;
#include "RA_Interface.h"

#include <stdio.h>
#include <io.h>       // for _open_osfhandle()
#include <fcntl.h>    // for _O_TEXT

#define EXEC_TYPES_H
#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"

// DEFINES----------------------------------------------------------------

#define CLEAROUTPUT
// whether you want the output window to be cleared when it is reopened

// #define OLDELEKTORLOGO
// whether to use the old (1970s) Elektor logo (ohm symbol) in most
// situations. (Elektor UVI monitor always uses the new logo.)

#define OUTPUTWIDTH    676
#define OUTPUTHEIGHT   409

typedef BOOL (WINAPI* AttachConsole_t)(DWORD dwProcessId);

#define DRAWPREVIEWPIXEL(x, y, z) canvasdisplay[CANVAS_PREVIEW][(  (y) * 2       * PREVIEWWIDTH) + ((x) * 2)    ] = \
                                  canvasdisplay[CANVAS_PREVIEW][(  (y) * 2       * PREVIEWWIDTH) + ((x) * 2) + 1] = \
                                  canvasdisplay[CANVAS_PREVIEW][((((y) * 2) + 1) * PREVIEWWIDTH) + ((x) * 2)    ] = \
                                  canvasdisplay[CANVAS_PREVIEW][((((y) * 2) + 1) * PREVIEWWIDTH) + ((x) * 2) + 1] = pencolours[colourset][(z)]

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       FLAG                  repaintmemmap;
EXPORT       int                   candy[CANDIES]       = { TRUE, TRUE, TRUE, TRUE },
                                   sprviewcolour,
                                   textcolour           = TEXTPEN_DEFAULT;
EXPORT       HBRUSH                hSpriteBrush[9];
EXPORT       HDC                   MusicRastPtr,
                                   StaveRastPtr;
EXPORT       HICON                 cpuicon[2];
EXPORT       HWND                  MagnifierWindowPtr   = NULL,
                                   memorygad[MEMGADGETS],
                                   RichTextGadget;

EXPORT const int gadgetimage[10][2] =
{ {  8, 18 }, // left  autofire
  {  9, 19 }, // right autofire
  { 10, 20 }, // swapped
  { 11, 21 }, // sound
  { 12, 22 }, // paused
  { 13, 23 }, // turbo
  { 14, 24 }, // collisions
  { 15, 15 }, // record
  { 16, 16 }, // play
  { 17, 17 }, // stop
};

EXPORT const struct MemMapToStruct memmap_to[MEMMAPS] = {
{MIB_INTERTON   , IDI_INTERTON   }, //  0 A
{MIB_INTERTON   , IDI_INTERTON   }, //  1 B
{MIB_INTERTON   , IDI_INTERTON   }, //  2 C
{MIB_INTERTON   , IDI_INTERTON   }, //  3 D
#ifdef OLDELEKTORLOGO
{MIB_ELEKTOR    , IDI_ELEKTOR2   }, //  4 E
{MIB_ELEKTOR    , IDI_ELEKTOR2   }, //  5 F
#else
{MIB_ELEKTOR    , IDI_ELEKTOR    }, //  4 E
{MIB_ELEKTOR    , IDI_ELEKTOR    }, //  5 F
#endif
{MIB_ARCADIA    , IDI_ARCADIA    }, //  6 G
{MIB_ARCADIA    , IDI_ARCADIA    }, //  7 H
{MIB_ARCADIA    , IDI_ARCADIA    }, //  8 I
{MIB_PIPBUG     , IDI_PIPBUG     }, //  9 PIPBUG1
{MIB_PIPBUG     , IDI_PIPBUG     }, // 10 PIPBUG2
{MIB_BINBUG     , IDI_BINBUG     }, // 11 BINBUG
{MIB_TWIN       , IDI_TWIN       }, // 12 TWIN
{MIB_ARCADIA    , IDI_ARCADIA    }, // 13 spare
{MIB_INSTRUCTOR , IDI_INSTRUCTOR }, // 14 N
{MIB_INSTRUCTOR , IDI_INSTRUCTOR }, // 15 O
{MIB_CD2650     , IDI_CD2650     }, // 16
{MIB_ARCADIA    , IDI_ARCADIA    }, // 17 spare
{MIB_ARCADIA    , IDI_ARCADIA    }, // 18 spare
// We have to use a valid icon for unused memory maps.
{MIB_PHUNSY     , IDI_PHUNSY     }, // 19
{MIB_SELBST     , IDI_SELBST     }, // 20
{MIB_MIKIT      , IDI_MIKIT      }, // 21
{MIB_ASTROWARS  , IDI_ASTROWARS  }, // 22
{MIB_GALAXIA    , IDI_GALAXIA    }, // 23
{MIB_LASERBATTLE, IDI_LASERBATTLE}, // 24
{MIB_LAZARIAN   , IDI_LASERBATTLE}, // 25
{MIB_M1         , IDI_MALZAK     }, // 26
{MIB_M2         , IDI_MALZAK     }, // 27
{MIB_8550       , IDI_PONG       }, // 28
{MIB_8600       , IDI_PONG       }, // 29
{MIB_TYPERIGHT  , IDI_TYPERIGHT  }, // 30
};

EXPORT CHARFORMAT2 fgformat =
{   sizeof(CHARFORMAT2),
    CFM_COLOR | CFM_BACKCOLOR,
    0, // dwEffects
    0, // yHeight
    0, // yOffset
    0, // crTextColor (fg colour when colours are turned off) (done later)
    0, // charset
    FIXED_PITCH | FF_MODERN,
    "Courier",
    0, 0, 0,
    0, // crBackColor (bg colour) (done later)
    0, 0, 0, 0, 0, 0
};

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       int                       autopause,
                                       binbug_baudrate,
                                       binbug_biosver,
                                       CatalogPtr,
                                       cheevos,
                                       climode,
                                       colourlog,
                                       colourset,
                                       confine,
                                       consolebg,
                                       cpl,
                                       DisplayWidth,
                                       DisplayHeight,
                                       fastbinbug,
                                       fastcd2650,
                                       fastpipbug,
                                       fastselbst,
                                       frameskip,
                                       fullscreen,
                                       game,
                                       language,
                                       leftwidth,
                                       logfile,
                                       machine,
                                       memmap,
                                       needaudit,
                                       outputpos,
                                       palettepen,
                                       pipbug_baudrate,
                                       pipbug_biosver,
                                       pvibase,
                                       regionstart,
                                       showlegend,
                                       showmenubars[2],
                                       showpointers[2],
                                       showsidebars[2],
                                       size,
                                       speedup,
                                       style,
                                       subwinx[SUBWINDOWS],
                                       subwiny[SUBWINDOWS],
                                       timeunit,
                                       titleheight,
                                    // twin_baudrate,
                                       wheremusicmouse[2],
                                       whichcpu,
                                       whichgame,
                                       wide,
                                       winheight,
                                       wsm,
                                       viewingsprite;
IMPORT const int                       num_to_num[NUMKEYS],
                                       speedupnum[SPEED_MAX - SPEED_MIN + 1],
                                       SubWindowMenuItem[SUBWINDOWS];
IMPORT       FLAG                      capslock,
                                       consoleopened,
                                       incli,
                                       opening,
                                       PSGSoundOpened[GUESTCHANNELS],
                                       reghost,
                                       resetdisabled,
                                       SampleOpened[SAMPLES];                                       
IMPORT       TEXT                      consolestring[OUTPUTLENGTH + 1],
                                       controltip[5][64 + 1],
                                       fn_game[MAX_PATH + 1], // the entire pathname (path and file)
                                       gtempstring[256 + 1],
                                       sprad[16][5 + 1],
                                       spritename[SPRITENAMELENGTH],
                                       titlebartext[MAX_PATH + TITLEBARTEXTLENGTH];
IMPORT       UBYTE                     cc,
                                       cd2650_chars_bmp[128][10],
                                       KeyMatrix[SCANCODES / 8],
                                       memory[32768],
                                       other_psu,
                                       psl,
                                       psu;
IMPORT const char                      g_szMagnifierClassName[];
IMPORT const UBYTE                     arcadia_pdg[2][64][8],
                                       dg640_chars[128][DG640_CHARHEIGHT],
                                       phunsy_chars[128][8],
                                       phunsy_gfx[16][8];
IMPORT       ULONG*                    canvasdisplay[CANVASES];
IMPORT       UWORD                     mirror_r[32768],
                                       mirror_w[32768];
IMPORT const UWORD                     pvi_spritedata[4];
IMPORT       ULONG                     analog,
                                       autoframeskip,
                                       collisions,
                                       coverage[32768],
                                       cycles_prev,
                                       demultiplex,
                                       paused,
                                       pencolours[COLOURSETS][PENS],
                                       region,
                                       regionlevel,
                                       showpalladiumkeys1,
                                       sound,
                                       turbo,
                                       viewkybdas2,
                                       viewmemas,
                                       viewmemas2,
                                       winwidth,
                                       winleftx,
                                       wintopy;
IMPORT const ULONG                     EmuBrush[EMUBRUSHES];
IMPORT       ULONG*                    canvasdisplay[CANVASES];
IMPORT       STRPTR                    colournames[GUESTCOLOURS + 1],
                                       timeunitstr1;
IMPORT       HWND                      hToolbar,
                                       MainWindowPtr,
                                       SubWindowPtr[SUBWINDOWS],
                                       TipsPtr[SUBWINDOWS];
IMPORT       HBRUSH                    hBrush[EMUBRUSHES];
IMPORT       HFONT                     hFont,
                                       hGreekFont,
                                       hMonospaceFont,
                                       hPolishFont,
                                       hRussianFont;
IMPORT       HICON                     cancelicon,
                                       okicon;
IMPORT       HINSTANCE                 InstancePtr;
IMPORT       RECT                      therect;
IMPORT       POINT                     ThePoint;
IMPORT       MEMFLAG                   memflags[ALLTOKENS];
IMPORT       struct CanvasStruct       canvas[CANVASES];
IMPORT       struct FlagNameStruct     flagname[CPUTIPS];
IMPORT       struct HostMachineStruct  hostmachines[MACHINES];
IMPORT       struct NoteStruct         notes[NOTES + 1];
IMPORT       struct KeyNameStruct      keyname[SCANCODES];
IMPORT       struct MachineStruct      machines[MACHINES];
IMPORT       struct MonitorStruct      monitor[MONITORGADS];
IMPORT const struct KnownStruct        known[KNOWNGAMES];
IMPORT const struct KeyHelpStruct      keyhelp[40];
IMPORT       ASCREEN                   screen[BOXWIDTH][BOXHEIGHT];
IMPORT struct
{   BITMAPINFOHEADER Header;
    DWORD            Colours[3];
} CanvasBitMapInfo[CANVASES];

// MODULE VARIABLES-------------------------------------------------------

MODULE       FLAG                      magnifying = FALSE;
MODULE       TEXT                      globalstring[32 + 1 + 1];
MODULE       int                       dlgtype,
                                       maxsprites;
MODULE       RECT                      axesrect;
MODULE       WNDPROC                   OldRichEditProc;

MODULE const UWORD ibmgad[MONITORGADS] = {
    ID_CONSOLE,        //   0 (ARCADIA_FIRSTMONGAD)
    ID_RESOLUTION,
    ID_BGCOLLIDE,
    ID_P1PADDLE,
    ID_P2PADDLE,
    ID_PITCH,
    ID_BGCOLOUR,
    ID_SPRITECOLLIDE,
    ID_P1LEFTKEYS,
    ID_P2LEFTKEYS,
    ID_VOLUME,
    ID_VSCROLL,
    ID_SPRITES01CTRL,
    ID_P1MIDDLEKEYS,
    ID_P2MIDDLEKEYS,
    ID_CHARLINE,
    ID_SPRITES23CTRL,
    ID_P1RIGHTKEYS,
    ID_P2RIGHTKEYS,
    ID_P1PALLADIUM,
    ID_P2PALLADIUM,
    ID_18D0,
    ID_18D1,
    ID_18D2,
    ID_18D3,
    ID_18D4,
    ID_18D5,
    ID_18D6,
    ID_18D7,
    ID_18D8,
    ID_18D9,
    ID_18DA,
    ID_18DB,
    ID_18DC,
    ID_18DD,
    ID_18DE,
    ID_18DF,
    ID_18E0,
    ID_18E1,
    ID_18E2,
    ID_18E3,
    ID_18E4,
    ID_18E5,
    ID_18E6,
    ID_18E7,
    ID_18E8,
    ID_18E9,
    ID_18EA,
    ID_18EB,
    ID_18EC,
    ID_18ED,
    ID_18EE,
    ID_18EF,
    ID_18F8,
    ID_18F9,
    ID_18FA,
    ID_18FB,
    ID_1AD0,
    ID_1AD1,
    ID_1AD2,
    ID_1AD3,
    ID_1AD4,
    ID_1AD5,
    ID_1AD6,
    ID_1AD7,
    ID_1AD8,
    ID_1AD9,
    ID_1ADA,
    ID_1ADB,
    ID_1ADC,
    ID_1ADD,
    ID_1ADE,
    ID_1ADF,
    ID_1AE0,
    ID_1AE1,
    ID_1AE2,
    ID_1AE3,
    ID_1AE4,
    ID_1AE5,
    ID_1AE6,
    ID_1AE7,
    ID_1AE8,
    ID_1AE9,
    ID_1AEA,
    ID_1AEB,
    ID_1AEC,
    ID_1AED,
    ID_1AEE,
    ID_1AEF,
    ID_1AF0,
    ID_1AF1,
    ID_1AF2,
    ID_1AF3,
    ID_1AF4,
    ID_1AF5,
    ID_1AF6,
    ID_1AF7,
    ID_1AF8,
    ID_1AF9,
    ID_1AFA,
    ID_1AFB,
    ID_1AFC,
    ID_1AFD,
    ID_1AFE,
    ID_1AFF,
    IDC_SPRITE0AX,
    IDC_SPRITE0AY,
    IDC_SPRITE1AX,
    IDC_SPRITE1AY,
    IDC_SPRITE2AX,
    IDC_SPRITE2AY,
    IDC_SPRITE3AX,
    IDC_SPRITE3AY,     // 112 (ARCADIA_LASTMONGAD)
    ID_P1PADDLE,       // 113 (PVI1ST_FIRSTMONGAD)
    ID_P2PADDLE,
    ID_PITCH,
    ID_SIZES,
    ID_SCORECTRL,
    ID_SCORELT,
    ID_SCORERT,
    ID_BGCOLOUR,
    ID_BGCOLLIDE,
    ID_SPRITECOLLIDE,
    ID_SPR01COLOURS,
    ID_SPR23COLOURS,
    IDC_SPRITE0AX,
    IDC_SPRITE0AY,
    IDC_SPRITE1AX,
    IDC_SPRITE1AY,
    IDC_SPRITE2AX,
    IDC_SPRITE2AY,
    IDC_SPRITE3AX,
    IDC_SPRITE3AY,
    IDC_SPRITE0BX,
    IDC_SPRITE0BY,
    IDC_SPRITE1BX,
    IDC_SPRITE1BY,
    IDC_SPRITE2BX,
    IDC_SPRITE2BY,
    IDC_SPRITE3BX,
    IDC_SPRITE3BY,     // 140 (PVI1ST_LASTMONGAD)
    IDC_CASIN,         // 141 (E_FIRSTMONGAD)
    IDC_CASOUT,        // 142
    ID_P1LEFTKEYS,     // 143 (I_FIRSTMONGAD)
    ID_P1MIDDLEKEYS,
    ID_P1RIGHTKEYS,
    ID_CONSOLE,
    ID_P2LEFTKEYS,
    ID_P2MIDDLEKEYS,
    ID_P2RIGHTKEYS,
    ID_NOISE,          // 150 (IE_LASTMONGAD)
    ID_1F0E,           // 151 (INTERTON_FIRSTMONGAD)
    ID_1F0F,
    ID_1F1E,
    ID_1F1F,
    ID_1F4E,
    ID_1F4F,
    ID_1F50,
    ID_1F51,
    ID_1F52,
    ID_1F53,           // 160
    ID_1F54,
    ID_1F55,
    ID_1F56,
    ID_1F57,
    ID_1F58,
    ID_1F59,
    ID_1F5A,
    ID_1F5B,
    ID_1F5C,
    ID_1F5D,           // 170
    ID_1F5E,
    ID_1F5F,
    ID_1F60,
    ID_1F61,
    ID_1F62,
    ID_1F63,
    ID_1F64,
    ID_1F65,
    ID_1F66,
    ID_1F67,           // 180
    ID_1F68,
    ID_1F69,
    ID_1F6A,
    ID_1F6B,
    ID_1F6C,
    ID_1F6D,
    ID_1FAD,           // 187 (INTERTON_LASTMONGAD)
    ID_P3PADDLE,       // 188 (PVI2ND_FIRSTMONGAD)
    ID_P4PADDLE,
    ID_PITCH2,
    ID_SIZES2,
    ID_SCORECTRL2,
    ID_SCORELT2,
    ID_SCORERT2,
    ID_BGCOLOUR2,
    ID_BGCOLLIDE2,
    ID_SPRITECOLLIDE2,
    ID_SPR45COLOURS,
    ID_SPR67COLOURS,
    IDC_SPRITE4AX,
    IDC_SPRITE4AY,
    IDC_SPRITE5AX,
    IDC_SPRITE5AY,
    IDC_SPRITE6AX,
    IDC_SPRITE6AY,
    IDC_SPRITE7AX,
    IDC_SPRITE7AY,
    IDC_SPRITE4BX,
    IDC_SPRITE4BY,
    IDC_SPRITE5BX,
    IDC_SPRITE5BY,
    IDC_SPRITE6BX,
    IDC_SPRITE6BY,
    IDC_SPRITE7BX,
    IDC_SPRITE7BY,     // 215 (PVI2ND_LASTMONGAD)
    ID_P5PADDLE,       // 216 (PVI3RD_FIRSTMONGAD)
    ID_P6PADDLE,
    ID_PITCH3,
    ID_SIZES3,
    ID_SCORECTRL3,
    ID_SCORELT3,
    ID_SCORERT3,
    ID_BGCOLOUR3,
    ID_BGCOLLIDE3,
    ID_SPRITECOLLIDE3,
    ID_SPR89COLOURS,
    ID_SPR1011COLOURS,
    IDC_SPRITE8AX,
    IDC_SPRITE8AY,
    IDC_SPRITE9AX,
    IDC_SPRITE9AY,
    IDC_SPRITE10AX,
    IDC_SPRITE10AY,
    IDC_SPRITE11AX,
    IDC_SPRITE11AY,
    IDC_SPRITE8BX,
    IDC_SPRITE8BY,
    IDC_SPRITE9BX,
    IDC_SPRITE9BY,
    IDC_SPRITE10BX,
    IDC_SPRITE10BY,
    IDC_SPRITE11BX,
    IDC_SPRITE11BY,    // 243 (PVI3RD_LASTMONGAD)
    ID_AMPLITUDEA1,    // 244 (PSGS_FIRSTMONGAD)
    ID_AMPLITUDEB1,
    ID_AMPLITUDEC1,
    ID_PITCHD1,
    ID_MIXER1,
    ID_SHAPE1,
    ID_PORTA1,         // 250
    ID_PORTB1,
    ID_AMPLITUDEA2,
    ID_AMPLITUDEB2,
    ID_AMPLITUDEC2,
    ID_PITCHD2,
    ID_MIXER2,
    ID_SHAPE2,
    ID_PORTA2,
    ID_PORTB2,
    IDC_PITCHA1,       // 260 (FIRSTDOUBLEBYTEPSG)
    IDC_PITCHA2,
    IDC_PITCHB1,
    IDC_PITCHB2,
    IDC_PITCHC1,
    IDC_PITCHC2,
    IDC_PERIOD1,
    IDC_PERIOD2,       // 267 (LASTDOUBLEBYTEPSG) (PSGS_LASTMONGAD)
    IDC_R0,            // 268 (CPU_FIRSTMONGAD)
    IDC_R1,
    IDC_R2,            // 270
    IDC_R3,
    IDC_R4,
    IDC_R5,
    IDC_R6,
    IDC_RAS0,
    IDC_RAS1,
    IDC_RAS2,
    IDC_RAS3,
    IDC_RAS4,
    IDC_RAS5,          // 280
    IDC_RAS6,
    IDC_RAS7,
    IDC_SENSE,
    IDC_FLAG,
    IDC_INTERRUPT,
    IDC_USERFLAG1,
    IDC_USERFLAG2,
    IDC_SP,
    IDC_CC,
    IDC_IDC,           // 290
    IDC_RS,
    IDC_WC,
    IDC_OVF,
    IDC_COM,
    IDC_C,
    IDC_IAR,
    IDC_OPCODE,
    IDC_SLAVE_R0,
    IDC_SLAVE_R1,
    IDC_SLAVE_R2,      // 300
    IDC_SLAVE_R3,
    IDC_SLAVE_R4,
    IDC_SLAVE_R5,
    IDC_SLAVE_R6,
    IDC_SLAVE_RAS0,
    IDC_SLAVE_RAS1,
    IDC_SLAVE_RAS2,
    IDC_SLAVE_RAS3,
    IDC_SLAVE_RAS4,
    IDC_SLAVE_RAS5,    // 310
    IDC_SLAVE_RAS6,
    IDC_SLAVE_RAS7,
    IDC_SLAVE_SENSE,
    IDC_SLAVE_FLAG,
    IDC_SLAVE_INTERRUPT,
    IDC_SLAVE_USERFLAG1,
    IDC_SLAVE_USERFLAG2,
    IDC_SLAVE_SP,
    IDC_SLAVE_CC,
    IDC_SLAVE_IDC,     // 320
    IDC_SLAVE_RS,
    IDC_SLAVE_WC,
    IDC_SLAVE_OVF,
    IDC_SLAVE_COM,
    IDC_SLAVE_C,
    IDC_SLAVE_IAR,
    IDC_SLAVE_OPCODE,
    IDC_CLOCK,
    IDC_FRAME,
    IDC_TIME,          // 330
    IDC_X,
    IDC_Y,
    IDC_WPM_TR,
    IDC_ACCURACY,
    IDC_ERRORS,
    IDC_EXPECTING,     // 336
};

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void clear_axes(HWND hwnd);
MODULE void clear_preview(HWND hwnd);
MODULE void draw_note(int whichnote, int colour);
MODULE void make_moretips(int kind, int elements, int firstgad, int howmany);

MODULE BOOL CALLBACK   CPUMonitorDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK    MagnifierDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK       MemoryDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK        MusicDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK       OutputDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK      PaletteDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK  PSGsMonitorDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK        SpeedDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK       SpriteDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK   XVIMonitorDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK RichEditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// CODE-------------------------------------------------------------------

EXPORT void view_monitor(int kind)
{   if (cheevos && RA_HardcoreModeIsActive())
    {   return;
    }

    switch (kind)
    {
    case SUBWINDOW_MONITOR_CPU:
        if (!hostmachines[machine].monitor_cpu)
        {   return;
        }
        open_subwindow(SUBWINDOW_MONITOR_CPU,  MAKEINTRESOURCE(hostmachines[machine].monitor_cpu ), CPUMonitorDlgProc);
        if (machine != TYPERIGHT)
        {   update_notation();
            make_tips(SUBWINDOW_MONITOR_CPU, (machine == TWIN) ? (CPUTIPS * 2) : CPUTIPS, 0);
            update_cpumonitortips();
        }
    acase SUBWINDOW_MONITOR_PSGS:
        if (!hostmachines[machine].monitor_psgs)
        {   return;
        }
        open_subwindow(SUBWINDOW_MONITOR_PSGS, MAKEINTRESOURCE(hostmachines[machine].monitor_psgs), PSGsMonitorDlgProc);
        if (memmap == MEMMAP_F)
        {   make_tips(SUBWINDOW_MONITOR_PSGS, PSGS_LASTMONGAD - PSGS_FIRSTMONGAD + 1, 0);
            update_psgmonitortips();
        }
    acase SUBWINDOW_MONITOR_XVI:
        if (!hostmachines[machine].monitor_xvi)
        {   return;
        }
        open_subwindow(SUBWINDOW_MONITOR_XVI,  MAKEINTRESOURCE(hostmachines[machine].monitor_xvi ), XVIMonitorDlgProc);
        switch (machine)
        {
        case ARCADIA:
            make_tips(    SUBWINDOW_MONITOR_XVI, ARCADIA_LASTMONGAD  - ARCADIA_FIRSTMONGAD + 1, ARCADIA_FIRSTMONGAD);
            update_xvimonitortips();
        acase INTERTON:
            make_tips(    SUBWINDOW_MONITOR_XVI, INTERTON_LASTMONGAD - I_FIRSTMONGAD       + 1, I_FIRSTMONGAD);
            make_moretips(SUBWINDOW_MONITOR_XVI, PVI1ST_LASTMONGAD   - PVI1ST_FIRSTMONGAD  + 1, PVI1ST_FIRSTMONGAD, INTERTON_LASTMONGAD - I_FIRSTMONGAD + 1);
            update_xvimonitortips();
        acase ELEKTOR:
            make_tips(    SUBWINDOW_MONITOR_XVI, IE_LASTMONGAD       - E_FIRSTMONGAD       + 1, E_FIRSTMONGAD);
            make_moretips(SUBWINDOW_MONITOR_XVI, PVI1ST_LASTMONGAD   - PVI1ST_FIRSTMONGAD  + 1, PVI1ST_FIRSTMONGAD, IE_LASTMONGAD       - E_FIRSTMONGAD + 1);
            update_xvimonitortips();
    }   }

    update_monitor(TRUE);
}

EXPORT void close_subwindow(int which)
{   RECT localrect;

    if (!SubWindowPtr[which])
    {   return;
    }

    GetWindowRect(SubWindowPtr[which], &localrect); // window relative to screen
    subwinx[which] = localrect.left;
    subwiny[which] = localrect.top;

    DestroyWindow(SubWindowPtr[which]);
    SubWindowPtr[which] = NULL;
    updatemenu(SubWindowMenuItem[which]);

    TipsPtr[which] = NULL;
}

EXPORT void setmonitorstring(int kind, HWND gadgetptr, STRPTR contents)
{   DISCARD SetWindowText(gadgetptr, contents);
}
EXPORT void setmonitorbutton(int kind, STRPTR contents, int whichmongad)
{   DISCARD SetWindowText(monitor[whichmongad].gadget, contents);
}
EXPORT void setmemory(int whichgad, STRPTR contents)
{   DISCARD SetWindowText(memorygad[whichgad], contents);
}
EXPORT void setsprview(int x, int y, int passedcolour)
{   SetDlgItemText(SubWindowPtr[SUBWINDOW_SPRITES], IDC_SPR_11 + (y * 8) + x, "");
}

EXPORT void make_tips(int kind, int elements, int firstgad)
{   TRANSIENT int      i;
    TRANSIENT TOOLINFO ti;
    FAST      RECT     therect1,
                       therect2;

    // assert(SubWindowPtr[kind]);

    if (!(TipsPtr[kind] = CreateWindowEx
    (   0,
        TOOLTIPS_CLASS,
        (LPSTR) NULL,
        TTS_ALWAYSTIP | TTS_NOPREFIX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        SubWindowPtr[kind],
        (HMENU) NULL,
        InstancePtr,
        NULL
    )))
    {   return;
    } // implied else

    // SetWindowPos(TipsPtr[kind], HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE); // apparently not needed

    SendMessage(TipsPtr[kind], TTM_SETMAXTIPWIDTH, 0,            32767); // to enable multi-line support
    SendMessage(TipsPtr[kind], TTM_SETDELAYTIME,   TTDT_AUTOPOP, 32767); // tooltip duration
    SendMessage(TipsPtr[kind], TTM_SETDELAYTIME,   TTDT_INITIAL,     0); // tooltip delay
    SendMessage(TipsPtr[kind], TTM_SETDELAYTIME,   TTDT_RESHOW,      0); // tooltip delay

    for (i = 0; i < elements; i++)
    {   if
        (   kind == SUBWINDOW_HOSTKYBD
         && (   keyname[i].gadget == -1
             || (viewkybdas2 == 0 && !keyname[i].desktop)
             || (viewkybdas2 == 1 && !keyname[i].laptop)
        )   )
        {   continue;
        } // implied else

        if
        (   kind == SUBWINDOW_GAMEINFO
         && !showpalladiumkeys1
         && ((i >= 2 && i <= 7) || i == 9 || i == 12)
        )
        {   continue;
        }

        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
        ti.hwnd     = SubWindowPtr[kind];
        ti.uId      = i;
        ti.hinst    = InstancePtr;
        ti.lpszText = ""; // this is done later

        // Get gadget coordinate relative to screen.
        // Now subtract the offset of the window corner relative to screen.
        // Now we have gadget coordinate relative to window corner
        // Now subtract the offset of the window client relative to window corner.
        switch (kind)
        {
        case SUBWINDOW_GAMEINFO:
            GetWindowRect(GetDlgItem(SubWindowPtr[kind], keyhelp[i].gid              ), &therect1); // gadgets relative to screen
        acase SUBWINDOW_HOSTKYBD:
            GetWindowRect(GetDlgItem(SubWindowPtr[kind], keyname[i].gadget           ), &therect1); // gadgets relative to screen
        acase SUBWINDOW_MONITOR_CPU:
            GetWindowRect(GetDlgItem(SubWindowPtr[kind], ibmgad[CPU_FIRSTMONGAD  + i]), &therect1); // gadgets relative to screen
        acase SUBWINDOW_MONITOR_XVI:
            GetWindowRect(GetDlgItem(SubWindowPtr[kind], ibmgad[firstgad + i]        ), &therect1); // gadgets relative to screen
        acase SUBWINDOW_MONITOR_PSGS:
            GetWindowRect(GetDlgItem(SubWindowPtr[kind], ibmgad[PSGS_FIRSTMONGAD + i]), &therect1); // gadgets relative to screen
        adefault: // eg. SUBWINDOW_HOSTPADS, SUBWINDOW_MEMORY, SUBWINDOW_OPCODES, SUBWINDOW_FLOPPYDRIVE
            GetWindowRect(GetDlgItem(SubWindowPtr[kind], firstgad + i                ), &therect1); // gadgets relative to screen
        }
        GetWindowRect(SubWindowPtr[kind], &therect2); // window relative to screen
        ti.rect.top    = therect1.top    - therect2.top  - titleheight - 2;
        ti.rect.bottom = therect1.bottom - therect2.top  - titleheight - 2;
        ti.rect.left   = therect1.left   - therect2.left - 3; // was -leftwidth rather than -3
        ti.rect.right  = therect1.right  - therect2.left - 3; // was -leftwidth rather than -3
        SendMessage(TipsPtr[kind], TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
    }

    // SendMessage(TipsPtr[kind], TTM_ACTIVATE, (WPARAM) TRUE, 0); // it's active anyway by default
}

MODULE void make_moretips(int kind, int elements, int firstgad, int howmany)
{   TRANSIENT int      i;
    TRANSIENT TOOLINFO ti;
    FAST      RECT     therect1,
                       therect2;

    // assert(SubWindowPtr[kind]);

    for (i = 0; i < elements; i++)
    {   ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
        ti.hwnd     = SubWindowPtr[kind];
        ti.uId      = i + howmany;
        ti.hinst    = InstancePtr;
        ti.lpszText = ""; // this is done later

        // Get gadget coordinate relative to screen.
        // Now subtract the offset of the window corner relative to screen.
        // Now we have gadget coordinate relative to window corner
        // Now subtract the offset of the window client relative to window corner.
        switch (kind)
        {
        case SUBWINDOW_MONITOR_XVI:
            GetWindowRect(GetDlgItem(SubWindowPtr[kind], ibmgad[firstgad + i]), &therect1); // gadgets relative to screen
        }
        GetWindowRect(SubWindowPtr[kind], &therect2); // window relative to screen
        ti.rect.top    = therect1.top    - therect2.top  - titleheight - 2;
        ti.rect.bottom = therect1.bottom - therect2.top  - titleheight - 2;
        ti.rect.left   = therect1.left   - therect2.left - leftwidth;
        ti.rect.right  = therect1.right  - therect2.left - leftwidth;

        SendMessage(TipsPtr[kind], TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
    }

    // SendMessage(TipsPtr[kind], TTM_ACTIVATE, (WPARAM) TRUE, 0); // it's active anyway by default
}

EXPORT void edit_palette(void)
{   open_subwindow(SUBWINDOW_PALETTE, MAKEINTRESOURCE(IDD_PALETTE), PaletteDlgProc);
}

EXPORT void tools_music(void)
{   if
    (   (   machine                != ARCADIA
         && machines[machine].pvis == 0
         && whichgame              != SI50_THEMEPOS
         && machine                != PONG
         && machine                != TYPERIGHT
        )
     || SubWindowPtr[SUBWINDOW_MUSIC]
    )
    {   return; // important!
    }

    open_subwindow(SUBWINDOW_MUSIC, MAKEINTRESOURCE(IDD_MUSIC_BIG), MusicDlgProc);
    tools_music_engine();
}

EXPORT void open_spriteeditor(void)
{   if
    (   SubWindowPtr[SUBWINDOW_SPRITES]
     || (machine != ARCADIA && machines[machine].pvis == 0 && machine != BINBUG && machine != CD2650 && machine != PHUNSY) // eg. PIPBUG, INSTRUCTOR
     || (cheevos && RA_HardcoreModeIsActive())
    )
    {   return;
    }

    switch (machine)
    {
    case ARCADIA:
        maxsprites = 128;
        if (viewingsprite == 0 || viewingsprite >= maxsprites)
        {   viewingsprite = 56; // 1st sprite
        }
        open_subwindow(SUBWINDOW_SPRITES, MAKEINTRESOURCE(IDD_SPRITEVIEWER8 ), SpriteDlgProc);
    acase INTERTON:
    case ELEKTOR:
    case ZACCARIA:
    case MALZAK:
        maxsprites = machines[machine].pvis * 4;
        if (viewingsprite >= maxsprites)
        {   viewingsprite = 0;
        }
        open_subwindow(SUBWINDOW_SPRITES, MAKEINTRESOURCE(IDD_SPRITEVIEWER10), SpriteDlgProc);
    acase BINBUG:
        maxsprites = 256;
        if (viewingsprite == 0 || viewingsprite >= maxsprites)
        {   viewingsprite = 128; // 1st UDG
        }
        open_subwindow(SUBWINDOW_SPRITES, MAKEINTRESOURCE(IDD_SPRITEVIEWER16), SpriteDlgProc);
    acase CD2650:
        maxsprites = 128;
        if (viewingsprite >= maxsprites)
        {   viewingsprite = 0;
        }
        open_subwindow(SUBWINDOW_SPRITES, MAKEINTRESOURCE(IDD_PDGVIEWER_CD2650), SpriteDlgProc);
    acase PHUNSY:
        maxsprites = 256;
        if (viewingsprite >= maxsprites)
        {   viewingsprite = 0;
        }
        open_subwindow(SUBWINDOW_SPRITES, MAKEINTRESOURCE(IDD_PDGVIEWER_PHUNSY), SpriteDlgProc);
    }

    // assert(SubWindowPtr[SUBWINDOW_SPRITES]);
    DISCARD SendMessage(GetDlgItem(SubWindowPtr[SUBWINDOW_SPRITES], IDC_WHICHSPRITE), WM_SETREDRAW,            FALSE,         0);
    DISCARD SendMessage(GetDlgItem(SubWindowPtr[SUBWINDOW_SPRITES], IDC_WHICHSPRITE), LB_SETTOPINDEX, (WPARAM) viewingsprite, 0);
    DISCARD SendMessage(GetDlgItem(SubWindowPtr[SUBWINDOW_SPRITES], IDC_WHICHSPRITE), LB_SETCURSEL,   (WPARAM) viewingsprite, 0);
    DISCARD SendMessage(GetDlgItem(SubWindowPtr[SUBWINDOW_SPRITES], IDC_WHICHSPRITE), WM_SETREDRAW,            TRUE,          0);

    updatemenu(MENUITEM_SPRITEVIEWER);
    update_spriteeditor(TRUE);
}

EXPORT void edit_memory(void)
{   if (machine == PONG || machine == TYPERIGHT || (cheevos && RA_HardcoreModeIsActive()))
    {   return; // important!
    }

    open_subwindow
    (   SUBWINDOW_MEMORY,
        MAKEINTRESOURCE(IDD_MEMORY_2650),
        MemoryDlgProc
    );

    make_tips(SUBWINDOW_MEMORY, MEMGADGETS, ID_MEM_0);

    repaintmemmap = FALSE;
    update_memory(TRUE);
}

EXPORT void open_speed(void)
{   switch (machine)
    {
    acase ARCADIA:
    case INTERTON:
    case ELEKTOR:
        open_subwindow(SUBWINDOW_SPEED, MAKEINTRESOURCE(IDD_SPEED_SOME  ), SpeedDlgProc);
    acase PIPBUG:
        open_subwindow(SUBWINDOW_SPEED, MAKEINTRESOURCE(IDD_SPEED_PIPBUG), SpeedDlgProc);
    acase BINBUG:
        open_subwindow(SUBWINDOW_SPEED, MAKEINTRESOURCE(IDD_SPEED_BINBUG), SpeedDlgProc);
    acase CD2650:
    case SELBST:
        open_subwindow(SUBWINDOW_SPEED, MAKEINTRESOURCE(IDD_SPEED_MOST  ), SpeedDlgProc);
    adefault: // INSTRUCTOR, TWIN, PHUNSY, MIKIT, PONG, TYPERIGHT, coin-ops
        open_subwindow(SUBWINDOW_SPEED, MAKEINTRESOURCE(IDD_SPEED_FEW   ), SpeedDlgProc);
}   }

EXPORT void do_preview(FLAG force)
{   FAST HDC   PreviewRastPtr;
    FAST UBYTE t;
    FAST int   x, y;

    if
    (   !SubWindowPtr[SUBWINDOW_SPRITES]
     || !GetDlgItem(SubWindowPtr[SUBWINDOW_SPRITES], IDC_PREVIEW)
    )
    {   return; // important!
    }

    switch (machine)
    {
    case ARCADIA:
        for (y = 0; y < 8; y++)
        {   for (x = 0; x < 8; x++)
            {   if (viewingsprite <= 55)
                {   t = (arcadia_pdg[0][viewingsprite     ][y]           & (128 >> x));
                } elif (viewingsprite >= 64)
                {   t = (arcadia_pdg[1][viewingsprite - 64][y]           & (128 >> x));
                } else
                {   // assert(viewingsprite >= 56 && viewingsprite <= 63);
                    t = (memory[0x1980 + ((viewingsprite - 56) * 8) + y] & (128 >> x));
                }
                DRAWPREVIEWPIXEL(x + 2, y + 6, t ? sprviewcolour : GREY1);
        }   }
    acase INTERTON:
    case ELEKTOR:
    case ZACCARIA:
    case MALZAK:
        for (y = 0; y < 10; y++)
        {   for (x = 0; x < 8; x++)
            {   t = (memory[pvibase + (0x100 * (viewingsprite / 4)) + pvi_spritedata[viewingsprite % 4] + y] & (128 >> x));
                DRAWPREVIEWPIXEL(x + 2, y + 5, t ? sprviewcolour : GREY1);
        }   }
    acase BINBUG:
        for (y = 0; y < 16; y++)
        {   for (x = 0; x < 8; x++)
            {   if (viewingsprite < 128)
                {   t = dg640_chars[      viewingsprite][              y] & (128 >> x);
                } else
                {   t = memory[0x7000 + ((viewingsprite - 128) * 16) + y] & (128 >> x);
                }
                DRAWPREVIEWPIXEL(x + 2, y + 2, t ? sprviewcolour : GREY1);
        }   }
    acase CD2650:
        for (y = 0; y < 8; y++)
        {   for (x = 0; x < 8; x++)
            {   t = cd2650_chars_bmp[viewingsprite][y] & (128 >> x);
                DRAWPREVIEWPIXEL(x + 2, y + 6, t ? sprviewcolour : GREY1);
        }   }
    acase PHUNSY:
        for (y = 0; y < 8; y++)
        {   for (x = 0; x < 6; x++)
            {   if (viewingsprite < 128)
                {   t = phunsy_chars[viewingsprite][            y] & (32 >> x);
                } else
                {   t = phunsy_gfx[( viewingsprite - 128) % 16][y] & (32 >> x);
                }
                DRAWPREVIEWPIXEL(x + 3, y + 6, t ? sprviewcolour : BLUE);
    }   }   }

    PreviewRastPtr = GetDC(GetDlgItem(SubWindowPtr[SUBWINDOW_SPRITES], IDC_PREVIEW));
    DISCARD StretchDIBits
    (   PreviewRastPtr,
          0,            // dest leftx
          0,            // dest topy
        PREVIEWWIDTH,   // dest width
        PREVIEWHEIGHT,  // dest height
          0,            // source leftx
          0,            // source topy
        PREVIEWWIDTH,   // source width
        PREVIEWHEIGHT,  // source height
        canvasdisplay[CANVAS_PREVIEW], // pointer to the bits
        (const struct tagBITMAPINFO*) &CanvasBitMapInfo[CANVAS_PREVIEW], // pointer to BITMAPINFO structure
        DIB_RGB_COLORS, // format of data
        SRCCOPY         // blit mode
    );
    ReleaseDC(GetDlgItem(SubWindowPtr[SUBWINDOW_SPRITES], IDC_PREVIEW), PreviewRastPtr);
}

EXPORT void ghostmonitorbutton(int kind, HWND which, int state)
{   switch (state)
    {
    case TRUE:
        EnableWindow(which, FALSE);
    acase FALSE:
        EnableWindow(which, TRUE);
}   }

EXPORT void open_subwindow(int which, LPCTSTR dialog, DLGPROC function)
{   if (SubWindowPtr[which])
    {   return; // important!
    }

    opening = TRUE;
    SubWindowPtr[which] = CreateDialog
    (   InstancePtr,
        dialog,
        MainWindowPtr,
        function
    );
    opening = FALSE;

    updatemenu(SubWindowMenuItem[which]);
    if (which != SUBWINDOW_MEMORY)
    {   DISCARD SetActiveWindow(MainWindowPtr);
    }

    if (fullscreen)
    {   showpointers[wsm] = TRUE;
        updatepointer(FALSE, TRUE);
        updatemenu(MENUITEM_POINTER);
}   }

EXPORT void update_memorytips(void)
{   int      i;
    TOOLINFO ti;
    TEXT     datatip[512 + 1];

    if (!SubWindowPtr[SUBWINDOW_MEMORY])
    {   return; // important!
    }

    for (i = 0; i < MEMGADGETS; i++)
    {   make_memorytip(regionstart + i, datatip);
        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
        ti.hwnd     = SubWindowPtr[SUBWINDOW_MEMORY];
        ti.uId      = i;
        ti.hinst    = InstancePtr;
        ti.lpszText = datatip; // this gets copied
        SendMessage(TipsPtr[SUBWINDOW_MEMORY], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
}   }

EXPORT void update_cpumonitortips(void)
{   int      i;
    TOOLINFO ti;
    TEXT     datatip[512 + 1];

    if (!SubWindowPtr[SUBWINDOW_MONITOR_CPU])
    {   return;
    }

    for (i = 0; i < CPUTIPS; i++)
    {   strcpy(datatip, flagname[i].longer[style]);
        // assert(datatip[0]);
        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP | TTF_IDISHWND;
        ti.uId      = i;
        ti.hwnd     = SubWindowPtr[SUBWINDOW_MONITOR_CPU];
        ti.hinst    = InstancePtr;
        ti.lpszText = datatip; // this gets copied
        SendMessage(TipsPtr[SUBWINDOW_MONITOR_CPU], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
    }

    if (machine == TWIN)
    {   for (i = 0; i < CPUTIPS; i++)
        {   strcpy(datatip, flagname[i].longer[style]);
            // assert(datatip[0]);
            ti.cbSize   = sizeof(TOOLINFO);
            ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP | TTF_IDISHWND;
            ti.uId      = CPUTIPS + i;
            ti.hwnd     = SubWindowPtr[SUBWINDOW_MONITOR_CPU];
            ti.hinst    = InstancePtr;
            ti.lpszText = datatip; // this gets copied
            SendMessage(TipsPtr[SUBWINDOW_MONITOR_CPU], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
}   }   }

EXPORT void update_psgmonitortips(void)
{   int      i;
    TOOLINFO ti;
    TEXT     datatip[512 + 1];

    if (!SubWindowPtr[SUBWINDOW_MONITOR_PSGS])
    {   return;
    }

    // assert(memmap == MEMMAP_F);

    for (i = PSGS_FIRSTMONGAD; i <= PSGS_LASTMONGAD; i++)
    {   make_monitortip(monitor[i].addr, datatip);
        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
        ti.hwnd     = SubWindowPtr[SUBWINDOW_MONITOR_PSGS];
        ti.uId      = i - PSGS_FIRSTMONGAD;
        ti.hinst    = InstancePtr;
        ti.lpszText = datatip; // this gets copied
        SendMessage(TipsPtr[SUBWINDOW_MONITOR_PSGS], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
}   }

EXPORT void update_xvimonitortips(void)
{   int      i;
    TOOLINFO ti;
    TEXT     datatip[512 + 1];

    if (!SubWindowPtr[SUBWINDOW_MONITOR_XVI])
    {   return;
    }

    switch (machine)
    {
    case ARCADIA:
        for (i = ARCADIA_FIRSTMONGAD; i <= ARCADIA_LASTMONGAD; i++)
        {   make_monitortip(monitor[i].addr, datatip);
            ti.cbSize   = sizeof(TOOLINFO);
            ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
            ti.hwnd     = SubWindowPtr[SUBWINDOW_MONITOR_XVI];
            ti.uId      = i - ARCADIA_FIRSTMONGAD;
            ti.hinst    = InstancePtr;
            ti.lpszText = datatip; // this gets copied
            SendMessage(TipsPtr[SUBWINDOW_MONITOR_XVI], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
        }
    acase INTERTON:
        for (i = I_FIRSTMONGAD; i <= INTERTON_LASTMONGAD; i++)
        {   make_monitortip(monitor[i].addr, datatip);
            ti.cbSize   = sizeof(TOOLINFO);
            ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
            ti.hwnd     = SubWindowPtr[SUBWINDOW_MONITOR_XVI];
            ti.uId      = i - I_FIRSTMONGAD;
            ti.hinst    = InstancePtr;
            ti.lpszText = datatip; // this gets copied
            SendMessage(TipsPtr[SUBWINDOW_MONITOR_XVI], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
        }
        for (i = PVI1ST_FIRSTMONGAD; i <= PVI1ST_LASTMONGAD; i++)
        {   make_monitortip(pvibase + monitor[i].addr, datatip);
            ti.cbSize   = sizeof(TOOLINFO);
            ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
            ti.hwnd     = SubWindowPtr[SUBWINDOW_MONITOR_XVI];
            ti.uId      = i - PVI1ST_FIRSTMONGAD + INTERTON_LASTMONGAD - I_FIRSTMONGAD + 1;
            ti.hinst    = InstancePtr;
            ti.lpszText = datatip; // this gets copied
            SendMessage(TipsPtr[SUBWINDOW_MONITOR_XVI], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
        }
    acase ELEKTOR:
        for (i = E_FIRSTMONGAD; i <= IE_LASTMONGAD; i++)
        {   make_monitortip(monitor[i].addr, datatip);
            ti.cbSize   = sizeof(TOOLINFO);
            ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
            ti.hwnd     = SubWindowPtr[SUBWINDOW_MONITOR_XVI];
            ti.uId      = i - E_FIRSTMONGAD;
            ti.hinst    = InstancePtr;
            ti.lpszText = datatip; // this gets copied
            SendMessage(TipsPtr[SUBWINDOW_MONITOR_XVI], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
        }
        for (i = PVI1ST_FIRSTMONGAD; i <= PVI1ST_LASTMONGAD; i++)
        {   make_monitortip(pvibase + monitor[i].addr, datatip);
            ti.cbSize   = sizeof(TOOLINFO);
            ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
            ti.hwnd     = SubWindowPtr[SUBWINDOW_MONITOR_XVI];
            ti.uId      = i - PVI1ST_FIRSTMONGAD + IE_LASTMONGAD - E_FIRSTMONGAD + 1;
            ti.hinst    = InstancePtr;
            ti.lpszText = datatip; // this gets copied
            SendMessage(TipsPtr[SUBWINDOW_MONITOR_XVI], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
}   }   }

EXPORT void setdlgtext(HWND hwnd, int gid, int msg, STRPTR defaultmsg)
{   setfont(hwnd, gid);
    SetDlgItemText(hwnd, gid, LLL(msg, defaultmsg));
}

EXPORT void move_subwindow(int which, HWND hwnd)
{   // SubWindowPtr[which] isn't set up yet, so we require
    // the window handle to be passed to us as an argument.

    // assert(hwnd);

    if (subwinx[which] != -1)
    {   // assert(subwiny[which] != -1);
        DISCARD SetWindowPos
        (   hwnd,
            HWND_TOP,
            subwinx[which],
            subwiny[which],
            0,
            0,
            SWP_NOCOPYBITS | SWP_NOSIZE
        );
}   }

EXPORT void update_notation(void)
{   update_opcodes();

    if (!SubWindowPtr[SUBWINDOW_MONITOR_CPU])
    {   return;
    }

    switch (style)
    {
    case STYLE_SIGNETICS1:
    case STYLE_SIGNETICS2:
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R0 , "R0:" );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R1 , "R1:" );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R2 , "R2:" );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R3 , "R3:" );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R4 , "R4:" );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R5 , "R5:" );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R6 , "R6:" );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_PSU, "PSU:");
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_PSL, "PSL:");
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_IAR, "IAR:");
    acase STYLE_CALM:
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R0 , "A:"  );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R1 , "B:"  );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R2 , "C:"  );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R3 , "D:"  );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R4 , "B':" );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R5 , "C':" );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R6 , "D':" );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_PSU, "U:"  );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_PSL, "L:"  );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_IAR, "PC:" );
    acase STYLE_IEEE:
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R0 , ".0:"  );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R1 , ".1:"  );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R2 , ".2:"  );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R3 , ".3:"  );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R4 , ".4':" );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R5 , ".5':" );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_R6 , ".6':" );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_PSU, ".U:"  );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_PSL, ".L:"  );
        SetDlgItemText(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDL_IAR, ".PC:" ); // not explicitly specified in IEEE-694 standard
    }

    update_cpumonitortips();
    update_monitor(TRUE);
}

MODULE BOOL CALLBACK MemoryDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT PAINTSTRUCT localps;
    PERSIST   LONG        gid;
    PERSIST   int         i,
                          memcolour,
                          mousex,
                          mousey;
    FAST      TEXT        tempstring[5 + 1];
    FAST      POINT       thepoint;
    FAST      RECT        localrect;
    PERSIST   RECT        memrect[MEMGADGETS],
                          memmaprect;

    switch (Message)
    {
    case WM_INITDIALOG:
        setdlgtext(hwnd, IDL_HOVER2,             MSG_HOVER,        "Hover over a button for more information.");
        SetWindowText(hwnd, LLL(                 MSG_HAIL_MEMORY,  "Memory Editor"));

        setdlgtext(hwnd, IDL_REGION,             MSG_REGION,       "Region: $");
        setdlgtext(hwnd, IDL_LEGEND,             MSG_LEGEND,       "Legend");
        setdlgtext(hwnd, IDL_LEGEND1,            MSG_LEGEND1,      "Breakpoint");
        setdlgtext(hwnd, IDL_LEGEND2,            MSG_LEGEND7,      "Watchpoint"); // this is no mistake
        setdlgtext(hwnd, IDL_LEGEND3,            MSG_LEGEND2,      "Mirror");
        setdlgtext(hwnd, IDL_LEGEND4,            MSG_LEGEND3,      "Unmapped");
        setdlgtext(hwnd, IDL_LEGEND5,            MSG_LEGEND4,      "Write-only");
        setdlgtext(hwnd, IDL_LEGEND6,            MSG_LEGEND5,      "Read-only");
        setdlgtext(hwnd, IDL_LEGEND7,            MSG_LEGEND6,      "Read/write");
        setdlgtext(hwnd, IDL_VIEWMEMAS1,         MSG_VIEWAS,       "View as");
        setdlgtext(hwnd, IDL_VIEWMEMAS2,         MSG_VIEWAS,       "View as");
        setdlgtext(hwnd, IDC_VIEWMEMAS_HEX,      MSG_HEXADECIMAL2, "Hexadecimal");
        setdlgtext(hwnd, IDC_VIEWMEMAS_CHARS,    MSG_CHARACTERS,   "Characters");
        setdlgtext(hwnd, IDC_VIEWMEMAS_CONTENTS, MSG_CONTENTS,     "Contents");
        setdlgtext(hwnd, IDC_VIEWMEMAS_MEMMAP,   MSG_MEMMAP,       "Memory map");
        setdlgtext(hwnd, IDC_VIEWMEMAS_COVERAGE, MSG_COVERAGE,     "Coverage report");
        if (regionstart >= 32 * KILOBYTE)
        {   regionstart = 0;
        }

        GetWindowRect(GetDlgItem(hwnd, IDC_MEMMAP), &localrect);
        thepoint.x        = localrect.left;
        thepoint.y        = localrect.top;
        DISCARD ScreenToClient(hwnd, &thepoint);
        memmaprect.left   = thepoint.x;
        memmaprect.top    = thepoint.y;
        thepoint.x        = localrect.right;
        thepoint.y        = localrect.bottom;
        DISCARD ScreenToClient(hwnd, &thepoint);
        memmaprect.right  = thepoint.x;
        memmaprect.bottom = thepoint.y;
        // If they move the subwindow later, it doesn't matter.

        for (i = 0; i < MEMGADGETS; i++)
        {   GetWindowRect(GetDlgItem(hwnd, ID_MEM_0 + i), &localrect);
            thepoint.x        = localrect.left;
            thepoint.y        = localrect.top;
            DISCARD ScreenToClient(hwnd, &thepoint);
            memrect[i].left   = thepoint.x;
            memrect[i].top    = thepoint.y;
            thepoint.x        = localrect.right;
            thepoint.y        = localrect.bottom;
            DISCARD ScreenToClient(hwnd, &thepoint);
            memrect[i].right  = thepoint.x;
            memrect[i].bottom = thepoint.y;
        }
        // If they move the subwindow later, it doesn't matter.

        DISCARD CheckRadioButton
        (   hwnd,
            IDC_VIEWMEMAS_HEX,
            IDC_VIEWMEMAS_CHARS,
            viewmemas ? IDC_VIEWMEMAS_CHARS : IDC_VIEWMEMAS_HEX
        );
        DISCARD CheckRadioButton
        (   hwnd,
            IDC_VIEWMEMAS_CONTENTS,
            IDC_VIEWMEMAS_COVERAGE,
            IDC_VIEWMEMAS_CONTENTS + viewmemas2
        );

        for (i = 0; i < MEMGADGETS; i++)
        {   memorygad[i] = GetDlgItem(hwnd, ID_MEM_0 + i);
        }

        SendMessage(GetDlgItem(hwnd, IDC_MEMREGION), TBM_SETRANGE   , FALSE, MAKELONG(0, (32768 / MEMGADGETS) - 1));
        SendMessage(GetDlgItem(hwnd, IDC_MEMREGION), TBM_SETPAGESIZE,     0,                                     1);
        SendMessage(GetDlgItem(hwnd, IDC_MEMREGION), TBM_SETPOS     ,  TRUE,                           regionlevel);
        DISCARD SetFocus(GetDlgItem(hwnd, IDC_MEMREGION));

        move_subwindow(SUBWINDOW_MEMORY, hwnd);

        return FALSE;
    case WM_PAINT: // no need for acase
        // note that SubWindowPtr[SUBWINDOW_MEMORY] is not necessarily yet set up correctly at this point
        BeginPaint(hwnd, &localps);
        DISCARD EndPaint(hwnd, &localps);
        repaintmemmap = TRUE;
        return FALSE;
    case WM_HSCROLL:
        gid = GetWindowLong((HWND) lParam, GWL_ID);
        if (gid == IDC_MEMREGION)
        {   update_memory(TRUE);
        }
        return FALSE;
    case WM_CTLCOLORSTATIC: // no need for acase
        gid = GetWindowLong((HWND) lParam, GWL_ID);
        switch (gid)
        {
        case IDL_MEM_1ST:
            SetBkColor((HDC) wParam, EMUPEN_CYAN);   // breakpoint
            return (LRESULT) hBrush[EMUBRUSH_CYAN];
        acase IDL_MEM_2ND:
            SetBkColor((HDC) wParam, EMUPEN_ORANGE); // watchpoint
            return (LRESULT) hBrush[EMUBRUSH_ORANGE];
        acase IDL_MEM_3RD:
            SetBkColor((HDC) wParam, EMUPEN_PURPLE); // mirror
            return (LRESULT) hBrush[EMUBRUSH_PURPLE];
        acase IDL_MEM_4TH:
            SetBkColor((HDC) wParam, EMUPEN_RED);    // unmapped
            return (LRESULT) hBrush[EMUBRUSH_RED];
        acase IDL_MEM_5TH:
            SetBkColor((HDC) wParam, EMUPEN_YELLOW); // write-only
            return (LRESULT) hBrush[EMUBRUSH_YELLOW];
        acase IDL_MEM_6TH:
            SetBkColor((HDC) wParam, EMUPEN_BLUE);   // read-only
            return (LRESULT) hBrush[EMUBRUSH_BLUE];
        acase IDL_MEM_7TH:
            SetBkColor((HDC) wParam, EMUPEN_GREEN);  // read/write
            return (LRESULT) hBrush[EMUBRUSH_GREEN];
        adefault:
            if (gid >= ID_MEM_0 && gid <= ID_MEM_255)
            {   if (memflags[gid - ID_MEM_0 + regionstart] & BREAKPOINT)
                {   memcolour = EMUBRUSH_CYAN;
                } elif (memflags[gid - ID_MEM_0 + regionstart] & WATCHPOINT)
                {   memcolour = EMUBRUSH_ORANGE;
                } elif
                (   mirror_r[gid - ID_MEM_0 + regionstart] != gid - ID_MEM_0 + regionstart
                 && mirror_w[gid - ID_MEM_0 + regionstart] != gid - ID_MEM_0 + regionstart
                )
                {   if (memory[mirror_r[gid - ID_MEM_0 + regionstart]])
                    {   memcolour = EMUBRUSH_PURPLE;
                    } else
                    {   memcolour = EMUBRUSH_DARKPURPLE;
                }   }
                elif (memflags[gid - ID_MEM_0 + regionstart] & NOWRITE)
                {   if (memflags[gid - ID_MEM_0 + regionstart] & NOREAD)
                    {   memcolour = EMUBRUSH_RED;
                    } elif (memory[gid - ID_MEM_0 + regionstart])
                    {   memcolour = EMUBRUSH_BLUE;
                    } else
                    {   memcolour = EMUBRUSH_DARKBLUE;
                }   }
                elif (memflags[gid - ID_MEM_0 + regionstart] & NOREAD)
                {   memcolour = EMUBRUSH_YELLOW;
                } elif (memory[gid - ID_MEM_0 + regionstart])
                {   memcolour = EMUBRUSH_GREEN;
                } else
                {   memcolour = EMUBRUSH_DARKGREEN;
                }
                SetBkColor((HDC) wParam, EmuBrush[memcolour]);
                return (LRESULT) hBrush[memcolour];
        }   }
    acase WM_ACTIVATE:
        do_autopause(wParam, lParam);
    acase WM_COMMAND:
        gid = (int) LOWORD(wParam);
        if (gid >= IDC_VIEWMEMAS_HEX && gid <= IDC_VIEWMEMAS_CHARS)
        {   viewmemas = gid - IDC_VIEWMEMAS_HEX;
            DISCARD CheckRadioButton
            (   hwnd,
                IDC_VIEWMEMAS_HEX,
                IDC_VIEWMEMAS_CHARS,
                viewmemas ? IDC_VIEWMEMAS_CHARS : IDC_VIEWMEMAS_HEX
            );
            update_memory(TRUE);
        } elif (gid >= IDC_VIEWMEMAS_CONTENTS && gid <= IDC_VIEWMEMAS_COVERAGE)
        {   viewmemas2 = gid - IDC_VIEWMEMAS_CONTENTS;
            DISCARD CheckRadioButton
            (   hwnd,
                IDC_VIEWMEMAS_CONTENTS,
                IDC_VIEWMEMAS_COVERAGE,
                IDC_VIEWMEMAS_CONTENTS + viewmemas2
            );
            update_memory(TRUE);
        }
    acase WM_CLOSE:
        DestroyWindow(hwnd);
        SubWindowPtr[SUBWINDOW_MEMORY] = NULL;
        updatemenu(MENUITEM_EDITMEMORY);
    acase WM_DESTROY:
        return FALSE;
    case WM_LBUTTONDOWN: // no need for acase
    // not WM_LBUTTONDBLCLK!
        mousex = (int) LOWORD(lParam);
        mousey = (int) HIWORD(lParam);

        if
        (   mousex >= memmaprect.left
         && mousex <= memmaprect.right
         && mousey >= memmaprect.top
         && mousey <= memmaprect.bottom
        )
        {   regionstart = (mousey - memmaprect.top) * MEMMAPWIDTH;
            regionlevel = regionstart / 256;
            DISCARD SendMessage
            (   GetDlgItem(hwnd, IDC_MEMREGION),
                TBM_SETPOS,
                TRUE,
                regionlevel
            );
            update_memory(TRUE);
        } else
        {   for (i = 0; i < MEMGADGETS; i++)
            {   if
                (   mousex >= memrect[i].left
                 && mousex <= memrect[i].right
                 && mousey >= memrect[i].top
                 && mousey <= memrect[i].bottom
                )
                {   sprintf(tempstring, "$%X", (int) (regionstart + i));
                    debug_edit(tempstring, TRUE, FALSE);
                    break; // for speed
        }   }   }

        return FALSE;
    case WM_MOVE: // no need for acase
        reset_fps();
        return FALSE;
    adefault: // no need for adefault
        return FALSE;
    }
    return TRUE;
}

MODULE BOOL CALLBACK CPUMonitorDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT PAINTSTRUCT localps;
    PERSIST   LONG        gid;
    PERSIST   int         i,
                          mousex,
                          mousey,
                          whichaddr;
    PERSIST   HICON       localhicon;
    PERSIST   POINT       thepoint;
    PERSIST   RECT        localrect;
    FAST      ULONG       whichemupen;
    FAST      int         whichemubrush;

    switch (Message)
    {
    case WM_INITDIALOG:
        // note that SubWindowPtr[SUBWINDOW_MONITOR_CPU] is not yet set up correctly at this point

        if (machine == TYPERIGHT)
        {   SetWindowText(    hwnd, LLL(                 MSG_HAIL_TR_MONITOR  , "Type-right Monitor"));
            setdlgtext(       hwnd, IDL_SPEED_TR       , MSG_SPD_GAD          , "Speed:");
            setdlgtext(       hwnd, IDL_WPM            , MSG_WORDSPERMINUTE   , "wpm");
            setdlgtext(       hwnd, IDL_ACCURACY       , MSG_ACCURACY         , "Accuracy:");
            setdlgtext(       hwnd, IDL_ERRORS         , MSG_ERRORS           , "Errors:");
            setdlgtext(       hwnd, IDL_EXPECTING      , MSG_EXPECTING        , "Expecting:");
        } else
        {   if (machine == TWIN)
            {   SetWindowText(hwnd, LLL(                 MSG_HAIL_MONITOR_CPUS, "Real-Time CPUs Monitor"));
                setdlgtext(   hwnd, IDL_CPU_MASTERGROUP, MSG_MASTERCPU_GAD    , "Master CPU Status");
                setdlgtext(   hwnd, IDL_CPU_SLAVEGROUP , MSG_SLAVECPU_GAD     , "Slave CPU Status");
            } else
            {   SetWindowText(hwnd, LLL(                 MSG_HAIL_MONITOR     , "Real-Time CPU Monitor"));
                setdlgtext(   hwnd, IDL_CPU_MASTERGROUP, MSG_CPU_GAD          , "2650 CPU Status");
            }
            setdlgtext(       hwnd, IDL_HOVER6         , MSG_HOVER            , "Hover over a button for more information.");
            setfont(          hwnd, IDL_CLOCK2650);
            SetDlgItemText(   hwnd, IDL_CLOCK2650                             , timeunitstr1);
        }
        setdlgtext(           hwnd, IDL_FRAME          , MSG_FRAME_LONG       , "Frame:");
        setdlgtext(           hwnd, IDL_TIME           , MSG_TIME_LONG        , "Time:");

        // we can't call update_notation() here, so that is done later

        DISCARD RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        localhicon = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon), IMAGE_ICON, 32, 32, 0);
        ghostmonitor();

        for (i = CPU_FIRSTMONGAD; i <= CPU_LASTMONGAD; i++)
        {   monitor[i].gadget = GetDlgItem(hwnd, ibmgad[i]);
            GetWindowRect(monitor[i].gadget, &localrect);
            thepoint.x        = localrect.left;
            thepoint.y        = localrect.top;
            DISCARD ScreenToClient(hwnd, &thepoint);
            monitor[i].rect.left   = thepoint.x;
            monitor[i].rect.top    = thepoint.y;
            thepoint.x        = localrect.right;
            thepoint.y        = localrect.bottom;
            DISCARD ScreenToClient(hwnd, &thepoint);
            monitor[i].rect.right  = thepoint.x;
            monitor[i].rect.bottom = thepoint.y;
        }
        // If they move the subwindow later, it doesn't matter.

        if (machine == TWIN)
        {   cpuicon[0] = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_GLYPH_TWIN_DIM ), IMAGE_ICON, 48, 48, 0);
            cpuicon[1] = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_GLYPH_TWIN_GLOW), IMAGE_ICON, 48, 48, 0);
        }

        move_subwindow(SUBWINDOW_MONITOR_CPU, hwnd);

        return FALSE;
    acase WM_NOTIFY:
        switch (((LPNMHDR) lParam)->code)
        {
        case TTN_SHOW:
            return FALSE;
        default: // no need for acase
            return FALSE;
        }
    acase WM_ACTIVATE:
        do_autopause(wParam, lParam);
    acase WM_CLOSE:
        DestroyWindow(hwnd);
        SubWindowPtr[SUBWINDOW_MONITOR_CPU] = NULL;
        updatemenu(MENUITEM_VIEWMONITOR_CPU);
        if (machine == TWIN)
        {   DeleteObject(cpuicon[0]);
            DeleteObject(cpuicon[1]);
        }
    acase WM_DESTROY:
        return FALSE;
    case WM_PAINT: // no need for acase
        // note that SubWindowPtr[SUBWINDOW_MONITOR_CPU] is not necessarily yet set up correctly at this point
        BeginPaint(hwnd, &localps);
        DISCARD EndPaint(hwnd, &localps);
        return FALSE; // important!
    acase WM_CTLCOLORSTATIC:
        gid = GetWindowLong((HWND) lParam, GWL_ID);
        for (i = CPU_FIRSTMONGAD; i <= CPU_LASTMONGAD; i++)
        {   if (ibmgad[i] == gid)
            {   if (getmoncolour(i, &whichemupen, &whichemubrush))
                {   SetBkColor((HDC) wParam, whichemupen);
                    return (LRESULT) hBrush[whichemubrush];
                } // implied else
                return TRUE;
        }   }
        return TRUE;
    acase WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
        mousex = (int) LOWORD(lParam);
        mousey = (int) HIWORD(lParam);

        for (i = CPU_FIRSTMONGAD; i <= CPU_LASTMONGAD; i++)
        {   if
            (   mousex >= monitor[i].rect.left
             && mousex <= monitor[i].rect.right
             && mousey >= monitor[i].rect.top
             && mousey <= monitor[i].rect.bottom
            )
            {   if (whichcpu == 0) // master
                {   switch (i)
                    {
                    case  MONGAD_R0:         debug_edit("R0"  , TRUE, FALSE);
                    acase MONGAD_R1:         debug_edit("R1"  , TRUE, FALSE);
                    acase MONGAD_R2:         debug_edit("R2"  , TRUE, FALSE);
                    acase MONGAD_R3:         debug_edit("R3"  , TRUE, FALSE);
                    acase MONGAD_R4:         debug_edit("R4"  , TRUE, FALSE);
                    acase MONGAD_R5:         debug_edit("R5"  , TRUE, FALSE);
                    acase MONGAD_R6:         debug_edit("R6"  , TRUE, FALSE);
                    acase MONGAD_RAS0:       debug_edit("RAS0", TRUE, FALSE);
                    acase MONGAD_RAS1:       debug_edit("RAS1", TRUE, FALSE);
                    acase MONGAD_RAS2:       debug_edit("RAS2", TRUE, FALSE);
                    acase MONGAD_RAS3:       debug_edit("RAS3", TRUE, FALSE);
                    acase MONGAD_RAS4:       debug_edit("RAS4", TRUE, FALSE);
                    acase MONGAD_RAS5:       debug_edit("RAS5", TRUE, FALSE);
                    acase MONGAD_RAS6:       debug_edit("RAS6", TRUE, FALSE);
                    acase MONGAD_RAS7:       debug_edit("RAS7", TRUE, FALSE);
                    acase MONGAD_S:          psu ^= 0x80; update_monitor(FALSE);
                    acase MONGAD_F:          psu ^= 0x40; update_monitor(FALSE);
                    acase MONGAD_II:         psu ^= 0x20; update_monitor(FALSE);
                    acase MONGAD_UF1:        psu ^= 0x10; update_monitor(FALSE);
                    acase MONGAD_UF2:        psu ^= 0x08; update_monitor(FALSE);
                    acase MONGAD_SP:         debug_edit("SP"  , TRUE, FALSE);
                    acase MONGAD_CC:         debug_edit("CC"  , TRUE, FALSE);
                    acase MONGAD_IDC:        psl ^= 0x20; update_monitor(FALSE);
                    acase MONGAD_RS:         psl ^= 0x10; update_monitor(FALSE);
                    acase MONGAD_WC:         psl ^= 0x08; update_monitor(FALSE);
                    acase MONGAD_OVF:        psl ^= 0x04; update_monitor(FALSE);
                    acase MONGAD_COM:        psl ^= 0x02; update_monitor(FALSE);
                    acase MONGAD_C:          psl ^= 0x01; update_monitor(FALSE);
                    acase MONGAD_IAR:        debug_edit("IAR" , TRUE, FALSE);
                }   }
                else // slave
                {   switch (i)
                    {
                    case  MONGAD_SLAVE_R0:   debug_edit("R0"  , TRUE, FALSE);
                    acase MONGAD_SLAVE_R1:   debug_edit("R1"  , TRUE, FALSE);
                    acase MONGAD_SLAVE_R2:   debug_edit("R2"  , TRUE, FALSE);
                    acase MONGAD_SLAVE_R3:   debug_edit("R3"  , TRUE, FALSE);
                    acase MONGAD_SLAVE_R4:   debug_edit("R4"  , TRUE, FALSE);
                    acase MONGAD_SLAVE_R5:   debug_edit("R5"  , TRUE, FALSE);
                    acase MONGAD_SLAVE_R6:   debug_edit("R6"  , TRUE, FALSE);
                    acase MONGAD_SLAVE_RAS0: debug_edit("RAS0", TRUE, FALSE);
                    acase MONGAD_SLAVE_RAS1: debug_edit("RAS1", TRUE, FALSE);
                    acase MONGAD_SLAVE_RAS2: debug_edit("RAS2", TRUE, FALSE);
                    acase MONGAD_SLAVE_RAS3: debug_edit("RAS3", TRUE, FALSE);
                    acase MONGAD_SLAVE_RAS4: debug_edit("RAS4", TRUE, FALSE);
                    acase MONGAD_SLAVE_RAS5: debug_edit("RAS5", TRUE, FALSE);
                    acase MONGAD_SLAVE_RAS6: debug_edit("RAS6", TRUE, FALSE);
                    acase MONGAD_SLAVE_RAS7: debug_edit("RAS7", TRUE, FALSE);
                    acase MONGAD_SLAVE_S:    psu ^= 0x80; update_monitor(FALSE);
                    acase MONGAD_SLAVE_F:    psu ^= 0x40; update_monitor(FALSE);
                    acase MONGAD_SLAVE_II:   psu ^= 0x20; update_monitor(FALSE);
                    acase MONGAD_SLAVE_UF1:  psu ^= 0x10; update_monitor(FALSE);
                    acase MONGAD_SLAVE_UF2:  psu ^= 0x08; update_monitor(FALSE);
                    acase MONGAD_SLAVE_SP:   debug_edit("SP"  , TRUE, FALSE);
                    acase MONGAD_SLAVE_CC:   debug_edit("CC"  , TRUE, FALSE);
                    acase MONGAD_SLAVE_IDC:  psl ^= 0x20; update_monitor(FALSE);
                    acase MONGAD_SLAVE_RS:   psl ^= 0x10; update_monitor(FALSE);
                    acase MONGAD_SLAVE_WC:   psl ^= 0x08; update_monitor(FALSE);
                    acase MONGAD_SLAVE_OVF:  psl ^= 0x04; update_monitor(FALSE);
                    acase MONGAD_SLAVE_COM:  psl ^= 0x02; update_monitor(FALSE);
                    acase MONGAD_SLAVE_C:    psl ^= 0x01; update_monitor(FALSE);
                    acase MONGAD_SLAVE_IAR:  debug_edit("IAR" , TRUE, FALSE);
                }   }
                return FALSE; // for speed
        }   }
        return FALSE;
    case WM_MOVE: // no need for acase
        reset_fps();
        return FALSE;
    adefault: // no need for adefault
        return FALSE;
    }

    return TRUE;
}

MODULE BOOL CALLBACK PSGsMonitorDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT PAINTSTRUCT localps;
    PERSIST   LONG        gid;
    PERSIST   int         address,
                          i,
                          mousex,
                          mousey,
                          whichaddr;
    PERSIST   POINT       thepoint;
    PERSIST   RECT        localrect;

    switch (Message)
    {
    case WM_INITDIALOG:
        // note that SubWindowPtr[SUBWINDOW_MONITOR_PSGS] is not yet set up correctly at this point

        DISCARD SetWindowText(hwnd, LLL( MSG_HAIL_MONITOR_PSGS, "Real-Time PSGs Monitor"));
        if (memmap == MEMMAP_F)
        {   setdlgtext(hwnd, IDL_HOVER5, MSG_HOVER,             "Hover over a button for more information.");
        } else
        {   SetDlgItemText(hwnd, IDL_HOVER5, "");
        }

        DISCARD RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

        for (i = PSGS_FIRSTMONGAD; i <= PSGS_LASTMONGAD; i++)
        {   monitor[i].gadget = GetDlgItem(hwnd, ibmgad[i]);
            GetWindowRect(monitor[i].gadget, &localrect);
            thepoint.x        = localrect.left;
            thepoint.y        = localrect.top;
            DISCARD ScreenToClient(hwnd, &thepoint);
            monitor[i].rect.left   = thepoint.x;
            monitor[i].rect.top    = thepoint.y;
            thepoint.x        = localrect.right;
            thepoint.y        = localrect.bottom;
            DISCARD ScreenToClient(hwnd, &thepoint);
            monitor[i].rect.right  = thepoint.x;
            monitor[i].rect.bottom = thepoint.y;
        }
        // If they move the subwindow later, it doesn't matter.

        move_subwindow(SUBWINDOW_MONITOR_PSGS, hwnd);

        return FALSE;
    acase WM_ACTIVATE:
        do_autopause(wParam, lParam);
    acase WM_CLOSE:
        for (i = PSGS_FIRSTMONGAD; i <= PSGS_LASTMONGAD; i++)
        {   monitor[i].gadget      = NULL;
            monitor[i].rect.left   =
            monitor[i].rect.right  =
            monitor[i].rect.top    =
            monitor[i].rect.bottom = 0;
        }
        DestroyWindow(hwnd);
        SubWindowPtr[SUBWINDOW_MONITOR_PSGS] = NULL;
        updatemenu(MENUITEM_VIEWMONITOR_PSGS);
    acase WM_DESTROY:
        return FALSE;
    case WM_PAINT: // no need for acase
        // note that SubWindowPtr[SUBWINDOW_MONITOR_PSGS] is not necessarily yet set up correctly at this point
        BeginPaint(hwnd, &localps);
        DISCARD EndPaint(hwnd, &localps);
        return FALSE; // important!
    case WM_MBUTTONDBLCLK: // no need for acase
    case WM_MBUTTONDOWN:
        mousex = (int) LOWORD(lParam);
        mousey = (int) HIWORD(lParam);

        for (i = PSGS_FIRSTMONGAD; i <= PSGS_LASTMONGAD; i++)
        {   if
            (   mousex >= monitor[i].rect.left
             && mousex <= monitor[i].rect.right
             && mousey >= monitor[i].rect.top
             && mousey <= monitor[i].rect.bottom
            )
            {   whichaddr = getmonitoraddr(i);
                if (memflags[whichaddr] & WATCHPOINT)
                {   wp_clear(whichaddr);
                } else
                {   wp_add(whichaddr, 0, COND_UN, 0, 0xFF, TRUE);
                }
                break; // for speed
        }   }
    acase WM_CTLCOLORSTATIC:
        gid = GetWindowLong((HWND) lParam, GWL_ID);

        whichaddr = OUTOFRANGE;
        for (i = PSGS_FIRSTMONGAD; i <= PSGS_LASTMONGAD; i++)
        {   if (gid == ibmgad[i])
            {   whichaddr = getmonitoraddr(i);
                break; // for speed
        }   }

        if (whichaddr != OUTOFRANGE)
        {   if (memflags[whichaddr] & BREAKPOINT)
            {   SetBkColor((HDC) wParam, EMUPEN_CYAN);     // breakpoint
                return (LRESULT) hBrush[EMUBRUSH_CYAN];
            } elif (memflags[whichaddr] & WATCHPOINT)
            {   SetBkColor((HDC) wParam, EMUPEN_ORANGE);   // watchpoint
                return (LRESULT) hBrush[EMUBRUSH_ORANGE];
            } elif
            (   mirror_r[whichaddr] != whichaddr
             && mirror_w[whichaddr] != whichaddr
            )
            {   SetBkColor((HDC) wParam, EMUPEN_PURPLE);   // full mirror
                return (LRESULT) hBrush[EMUBRUSH_PURPLE];
            } elif (memflags[whichaddr] & NOWRITE)
            {   if (memflags[whichaddr] & NOREAD)
                {   SetBkColor((HDC) wParam, EMUPEN_RED);  // unmapped
                    return (LRESULT) hBrush[EMUBRUSH_RED];
                } else
                {   SetBkColor((HDC) wParam, EMUPEN_BLUE); // read-only
                    return (LRESULT) hBrush[EMUBRUSH_BLUE];
            }   }
            elif (memflags[whichaddr] & NOREAD)
            {   SetBkColor((HDC) wParam, EMUPEN_YELLOW);   // write-only
                return (LRESULT) hBrush[EMUBRUSH_YELLOW];
            } else
            {   SetBkColor((HDC) wParam, EMUPEN_GREEN);    // read/write
                return (LRESULT) hBrush[EMUBRUSH_GREEN];
        }   }
    acase WM_LBUTTONDOWN:
    // not WM_LBUTTONDBLCLK!
        mousex = (int) LOWORD(lParam);
        mousey = (int) HIWORD(lParam);

        for (i = PSGS_FIRSTMONGAD; i <= PSGS_LASTMONGAD; i++)
        {   if
            (   mousex >= monitor[i].rect.left
             && mousex <= monitor[i].rect.right
             && mousey >= monitor[i].rect.top
             && mousey <= monitor[i].rect.bottom
            )
            {   if (i >= FIRSTDOUBLEBYTEPSG && i <= LASTDOUBLEBYTEPSG)
                {   address = getmonitoraddr(i);
                    switch (address)
                    {
                    case  PSG_PITCHA1_L: debug_edit("PITCHA1_L", TRUE, TRUE);
                    acase PSG_PITCHB1_L: debug_edit("PITCHB1_L", TRUE, TRUE);
                    acase PSG_PITCHC1_L: debug_edit("PITCHC1_L", TRUE, TRUE);
                    acase PSG_PITCHA2_L: debug_edit("PITCHA2_L", TRUE, TRUE);
                    acase PSG_PITCHB2_L: debug_edit("PITCHB2_L", TRUE, TRUE);
                    acase PSG_PITCHC2_L: debug_edit("PITCHC2_L", TRUE, TRUE);
                    acase PSG_PERIOD1_L: debug_edit("PERIOD1_L", TRUE, TRUE);
                    acase PSG_PERIOD2_L: debug_edit("PERIOD2_L", TRUE, TRUE);
                }   }
                else
                {   monchange(i);
                }
                break; // for speed
        }   }

        return FALSE;
    case WM_MOVE: // no need for acase
        reset_fps();
        return FALSE;
    adefault: // no need for adefault
        return FALSE;
    }

    return TRUE;
}

MODULE BOOL CALLBACK XVIMonitorDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT PAINTSTRUCT localps;
    PERSIST   LONG        gid;
    PERSIST   int         i,
                          mousex,
                          mousey,
                          whichaddr;
    PERSIST   HICON       localhicon;
    PERSIST   POINT       thepoint;
    PERSIST   RECT        localrect;

    switch (Message)
    {
    case WM_INITDIALOG:
        // note that SubWindowPtr[SUBWINDOW_MONITOR_XVI] is not yet set up correctly at this point

        DISCARD SetWindowText(hwnd, LLL( MSG_HAIL_MONITOR_XVI, "Real-Time UVI/PVI(s) Monitor"));
        switch (machine)
        {
        case ARCADIA:
            setdlgtext(hwnd, IDC_RAM_GROUP,  MSG_UVIRAM_GAD,  "UVI RAM Contents");
            setdlgtext(hwnd, IDC_UVI_GROUP,  MSG_UVI_GAD,     "2637 UVI Status");
            setdlgtext(hwnd, IDL_PADDLES,    MSG_PADDLES,     "Paddles");
            setdlgtext(hwnd, IDL_HOVER4,     MSG_HOVER,       "Hover over a button for more information.");
        acase INTERTON:
            setdlgtext(hwnd, IDC_RAM_GROUP,  MSG_PVIRAM_GAD,  "PVI RAM Contents");
            setdlgtext(hwnd, IDC_PVI1_GROUP, MSG_PVI_GAD,     "2636 PVI Status");
            setdlgtext(hwnd, IDL_OTHER_GROUP,MSG_OTHERS,      "Other Registers");
            setdlgtext(hwnd, IDL_PADDLES,    MSG_PADDLES,     "Paddles");
            setdlgtext(hwnd, IDL_HOVER4,     MSG_HOVER,       "Hover over a button for more information.");
        acase ELEKTOR:
            setdlgtext(hwnd, IDC_PSG1_GROUP, MSG_PSG1_GAD,    "AY-3-8910 PSG #1 Status");
            setdlgtext(hwnd, IDC_PSG2_GROUP, MSG_PSG2_GAD,    "AY-3-8910 PSG #2 Status");
            setdlgtext(hwnd, IDC_PVI1_GROUP, MSG_PVI_GAD,     "2636 PVI Status");
            setdlgtext(hwnd, IDL_OTHER_GROUP,MSG_OTHERS,      "Other Registers");
            setdlgtext(hwnd, IDL_PADDLES,    MSG_PADDLES,     "Paddles");
            setdlgtext(hwnd, IDL_HOVER4,     MSG_HOVER,       "Hover over a button for more information.");
        acase ZACCARIA:
            if (machines[machine].pvis == 1)
            {   setdlgtext(hwnd, IDC_PVI1_GROUP, MSG_PVI_GAD,     "2636 PVI Status");
            } elif (machines[machine].pvis == 3)
            {   setdlgtext(hwnd, IDC_PVI1_GROUP, MSG_PVI1_GAD,    "2636 PVI #1 Status");
                setdlgtext(hwnd, IDC_PVI2_GROUP, MSG_PVI2_GAD,    "2636 PVI #2 Status");
                setdlgtext(hwnd, IDC_PVI3_GROUP, MSG_PVI3_GAD,    "2636 PVI #3 Status");
            }
        acase MALZAK:
            setdlgtext(hwnd, IDC_PVI1_GROUP, MSG_PVI1_GAD,    "2636 PVI #1 Status");
            setdlgtext(hwnd, IDC_PVI2_GROUP, MSG_PVI2_GAD,    "2636 PVI #2 Status");
        acase PONG:
        case BINBUG:
            setdlgtext(hwnd, IDL_PADDLES,    MSG_PADDLES,     "Paddles");
        }
        localhicon = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon), IMAGE_ICON, 32, 32, 0);
        DISCARD SendMessage(GetDlgItem(hwnd, IDL_XVIMONITORGLYPH), STM_SETICON, (WPARAM) localhicon, (LPARAM) 0);

        if (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR || machine == PONG || machine == BINBUG)
        {   axesrect.right  =
            axesrect.bottom = 96;
            axesrect.left   =
            axesrect.top    = 0;
        }

        setmonused();
        if (machine != PONG && machine != BINBUG)
        {   DISCARD RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        }

        for (i = XVI_FIRSTMONGAD; i <= XVI_LASTMONGAD; i++)
        {   if (monitor[i].used)
            {   monitor[i].gadget = GetDlgItem(hwnd, ibmgad[i]);
                GetWindowRect(monitor[i].gadget, &localrect);
                thepoint.x        = localrect.left;
                thepoint.y        = localrect.top;
                DISCARD ScreenToClient(hwnd, &thepoint);
                monitor[i].rect.left   = thepoint.x;
                monitor[i].rect.top    = thepoint.y;
                thepoint.x        = localrect.right;
                thepoint.y        = localrect.bottom;
                DISCARD ScreenToClient(hwnd, &thepoint);
                monitor[i].rect.right  = thepoint.x;
                monitor[i].rect.bottom = thepoint.y;
            } else
            {   monitor[i].gadget      = NULL;
                monitor[i].rect.left   =
                monitor[i].rect.right  =
                monitor[i].rect.top    =
                monitor[i].rect.bottom = 0;
        }   }
        // If they move the subwindow later, it doesn't matter.

        move_subwindow(SUBWINDOW_MONITOR_XVI, hwnd);

        return FALSE;
    acase WM_ACTIVATE:
        do_autopause(wParam, lParam);
    acase WM_CLOSE:
        DestroyWindow(hwnd);
        SubWindowPtr[SUBWINDOW_MONITOR_XVI] = NULL;
        for (i = XVI_FIRSTMONGAD; i <= XVI_LASTMONGAD; i++)
        {   monitor[i].used        = FALSE;
            monitor[i].gadget      = NULL;
            monitor[i].rect.left   =
            monitor[i].rect.right  =
            monitor[i].rect.top    =
            monitor[i].rect.bottom = 0;
        }
        updatemenu(MENUITEM_VIEWMONITOR_XVI);
    acase WM_DESTROY:
        return FALSE;
    case WM_PAINT: // no need for acase
        BeginPaint(hwnd, &localps);
        DISCARD EndPaint(hwnd, &localps);
        if (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR || machine == PONG || machine == BINBUG)
        {   clear_axes(hwnd);
            SubWindowPtr[SUBWINDOW_MONITOR_XVI] = hwnd; // important!
            do_axes();
            // UpdateWindow(GetDlgItem(hwnd, IDC_AXES));
        }
        return FALSE; // important!
    case WM_MBUTTONDBLCLK: // no need for acase
    case WM_MBUTTONDOWN:
        mousex = (int) LOWORD(lParam);
        mousey = (int) HIWORD(lParam);

        for (i = XVI_FIRSTMONGAD; i <= XVI_LASTMONGAD; i++)
        {   if
            (   monitor[i].used
             && mousex >= monitor[i].rect.left
             && mousex <= monitor[i].rect.right
             && mousey >= monitor[i].rect.top
             && mousey <= monitor[i].rect.bottom
            )
            {   whichaddr = getmonitoraddr(i);
                if (memflags[whichaddr] & WATCHPOINT)
                {   wp_clear(whichaddr);
                } else
                {   wp_add(whichaddr, 0, COND_UN, 0, 0xFF, TRUE);
                }
                break; // for speed
        }   }
    acase WM_CTLCOLORSTATIC:
        gid = GetWindowLong((HWND) lParam, GWL_ID);

        whichaddr = OUTOFRANGE;
        for (i = XVI_FIRSTMONGAD; i <= XVI_LASTMONGAD; i++)
        {   if (monitor[i].used && gid == ibmgad[i])
            {   whichaddr = getmonitoraddr(i);
                break; // for speed
        }   }

        if (whichaddr != OUTOFRANGE)
        {   if (memflags[whichaddr] & BREAKPOINT)
            {   SetBkColor((HDC) wParam, EMUPEN_CYAN);     // breakpoint
                return (LRESULT) hBrush[EMUBRUSH_CYAN];
            } elif (memflags[whichaddr] & WATCHPOINT)
            {   SetBkColor((HDC) wParam, EMUPEN_ORANGE);   // watchpoint
                return (LRESULT) hBrush[EMUBRUSH_ORANGE];
            } elif
            (   mirror_r[whichaddr] != whichaddr
             || mirror_w[whichaddr] != whichaddr
            )
            {   SetBkColor((HDC) wParam, EMUPEN_PURPLE);   // mirror
                return (LRESULT) hBrush[EMUBRUSH_PURPLE];
            } elif (memflags[whichaddr] & NOWRITE)
            {   if (memflags[whichaddr] & NOREAD)
                {   SetBkColor((HDC) wParam, EMUPEN_RED);  // unmapped
                    return (LRESULT) hBrush[EMUBRUSH_RED];
                } else
                {   SetBkColor((HDC) wParam, EMUPEN_BLUE); // read-only
                    return (LRESULT) hBrush[EMUBRUSH_BLUE];
            }   }
            elif (memflags[whichaddr] & NOREAD)
            {   SetBkColor((HDC) wParam, EMUPEN_YELLOW);   // write-only
                return (LRESULT) hBrush[EMUBRUSH_YELLOW];
            } else
            {   SetBkColor((HDC) wParam, EMUPEN_GREEN);    // read/write
                return (LRESULT) hBrush[EMUBRUSH_GREEN];
        }   }
    acase WM_LBUTTONDOWN:
    // not WM_LBUTTONDBLCLK!
        mousex = (int) LOWORD(lParam);
        mousey = (int) HIWORD(lParam);

        for (i = XVI_FIRSTMONGAD; i <= XVI_LASTMONGAD; i++)
        {   if
            (   monitor[i].used
             && mousex >= monitor[i].rect.left
             && mousex <= monitor[i].rect.right
             && mousey >= monitor[i].rect.top
             && mousey <= monitor[i].rect.bottom
            )
            {   monchange(i);
                break; // for speed
        }   }

        return FALSE;
    case WM_MOVE: // no need for acase
        reset_fps();
        return FALSE;
    adefault:
        return FALSE;
    }

    return TRUE;
}

MODULE BOOL CALLBACK PaletteDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT int    i,
                     number;
    TRANSIENT LONG   gid;
    PERSIST   ULONG  oldpencolours[GUESTCOLOURS],
                     wincolour;
    PERSIST   HBRUSH hLocalBrush;

    switch (Message)
    {
    case WM_INITDIALOG:
        SubWindowPtr[SUBWINDOW_PALETTE] = hwnd;

        DISCARD SetWindowText(hwnd, LLL(   MSG_HAIL_PALETTE    , "Palette Editor"   ));

        setdlgtext(hwnd, IDL_R,            MSG_LABEL_RED       , "Red"              );
        setdlgtext(hwnd, IDL_G,            MSG_LABEL_GREEN     , "Green"            );
        setdlgtext(hwnd, IDL_B,            MSG_LABEL_BLUE      , "Blue"             );
        for (i = 0; i < GUESTCOLOURS; i++)
        {   setfont(hwnd, IDC_PEN0 + i);
            SetDlgItemText(hwnd, IDC_PEN0 + i, colournames[i]);
        }
        setdlgtext(hwnd, IDC_RESETPALETTE, MSG_RESETTODEFAULTS2, "&Reset to defaults");
        setdlgtext(hwnd, IDOK,             MSG_OK2             , "&OK"               );
        setdlgtext(hwnd, IDCANCEL,         MSG_CANCEL2         , "&Cancel"           );

        DISCARD CheckRadioButton
        (   hwnd,
            IDC_PEN0,
            IDC_PEN23,
            IDC_PEN0 + palettepen
        );
        checkcolours();
        if (resetdisabled) // all are defaults
        {   EnableWindow(GetDlgItem(hwnd, IDC_RESETPALETTE), FALSE);
        } else // not all are defaults
        {   EnableWindow(GetDlgItem(hwnd, IDC_RESETPALETTE), TRUE );
        }
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_RED),
            TBM_SETRANGE,
            FALSE,
            MAKELONG(0, 255)
        );
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_GREEN),
            TBM_SETRANGE,
            FALSE,
            MAKELONG(0, 255)
        );
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_BLUE),
            TBM_SETRANGE,
            FALSE,
            MAKELONG(0, 255)
        );
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_RED),
            TBM_SETTICFREQ,
            64,
            0
        );
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_GREEN),
            TBM_SETTICFREQ,
            64,
            0
        );
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_BLUE),
            TBM_SETTICFREQ,
            64,
            0
        );

        SendMessage(GetDlgItem(hwnd, IDC_REDDEC  ), EM_LIMITTEXT, 3, 0);
        SendMessage(GetDlgItem(hwnd, IDC_GREENDEC), EM_LIMITTEXT, 3, 0);
        SendMessage(GetDlgItem(hwnd, IDC_BLUEDEC ), EM_LIMITTEXT, 3, 0);
        SendMessage(GetDlgItem(hwnd, IDC_REDHEX  ), EM_LIMITTEXT, 2, 0);
        SendMessage(GetDlgItem(hwnd, IDC_GREENHEX), EM_LIMITTEXT, 2, 0);
        SendMessage(GetDlgItem(hwnd, IDC_BLUEHEX ), EM_LIMITTEXT, 2, 0);

        update_palette(TRUE);
        redrawscreen();

        for (i = 0; i < GUESTCOLOURS; i++)
        {   oldpencolours[i] = pencolours[colourset][i];
        }

        move_subwindow(SUBWINDOW_PALETTE, hwnd);

        return TRUE;
    case WM_DRAWITEM: // no need for acase
        drawitem((struct tagDRAWITEMSTRUCT*) lParam, FALSE);
        return TRUE;
    case WM_CLOSE: // no need for acase
        DestroyWindow(hwnd);
        SubWindowPtr[SUBWINDOW_PALETTE] = NULL;
        updatemenu(MENUITEM_PALETTE);
        return TRUE;
    case WM_COMMAND: // no need for acase
        if (LOWORD(wParam) >= IDC_PEN0 && LOWORD(wParam) <= IDC_PEN23)
        {   palettepen = LOWORD(wParam) - IDC_PEN0;
            update_palette(FALSE);
            redrawscreen();
        } else
        {   switch (LOWORD(wParam))
            {
            case IDC_RESETPALETTE:
                for (i = 0; i < GUESTCOLOURS; i++)
                {   resetcolour(colourset, i, FALSE);
                }
                update_palette(TRUE);
                redrawscreen();
            acase IDCANCEL:
                for (i = 0; i < GUESTCOLOURS; i++)
                {   pencolours[colourset][i] = oldpencolours[i];
                }
                redrawscreen();
            //lint -fallthrough
            case IDOK:
                DestroyWindow(hwnd);
                SubWindowPtr[SUBWINDOW_PALETTE] = NULL;
                updatemenu(MENUITEM_PALETTE);
                update_spriteeditor(TRUE);
                return TRUE;
            acase IDC_REDDEC:
                if (HIWORD(wParam) == EN_KILLFOCUS)
                {   GetWindowText(GetDlgItem(hwnd, IDC_REDDEC), gtempstring, 3 + 1);
                    number = atoi(gtempstring);
                    if (number < 0) number = 0; elif (number > 255) number = 255;
                    pencolours[colourset][palettepen] &= 0x0000FFFF;
                    pencolours[colourset][palettepen] |= (number << 16);
                    update_palette(FALSE);
                    redrawscreen();
                }
            acase IDC_GREENDEC:
                if (HIWORD(wParam) == EN_KILLFOCUS)
                {   GetWindowText(GetDlgItem(hwnd, IDC_GREENDEC), gtempstring, 3 + 1);
                    number = atoi(gtempstring);
                    if (number < 0) number = 0; elif (number > 255) number = 255;
                    pencolours[colourset][palettepen] &= 0x00FF00FF;
                    pencolours[colourset][palettepen] |= (number <<  8);
                    update_palette(FALSE);
                    redrawscreen();
                }
            acase IDC_BLUEDEC:
                if (HIWORD(wParam) == EN_KILLFOCUS)
                {   GetWindowText(GetDlgItem(hwnd, IDC_BLUEDEC), gtempstring, 3 + 1);
                    number = atoi(gtempstring);
                    if (number < 0) number = 0; elif (number > 255) number = 255;
                    pencolours[colourset][palettepen] &= 0x00FFFF00;
                    pencolours[colourset][palettepen] |= number;
                    update_palette(FALSE);
                    redrawscreen();
                }
            acase IDC_REDHEX:
                if (HIWORD(wParam) == EN_KILLFOCUS)
                {   GetWindowText(GetDlgItem(hwnd, IDC_REDHEX), gtempstring, 2 + 1);
                    number = parse_hexbyte();
                    if (number != -1)
                    {   pencolours[colourset][palettepen] &= 0x0000FFFF;
                        pencolours[colourset][palettepen] |= (number << 16);
                        update_palette(FALSE);
                        redrawscreen();
                }   }
            acase IDC_GREENHEX:
                if (HIWORD(wParam) == EN_KILLFOCUS)
                {   GetWindowText(GetDlgItem(hwnd, IDC_GREENHEX), gtempstring, 2 + 1);
                    number = parse_hexbyte();
                    if (number != -1)
                    {   pencolours[colourset][palettepen] &= 0x00FF00FF;
                        pencolours[colourset][palettepen] |= (number << 8);
                        update_palette(FALSE);
                        redrawscreen();
                }   }
            acase IDC_BLUEHEX:
                if (HIWORD(wParam) == EN_KILLFOCUS)
                {   GetWindowText(GetDlgItem(hwnd, IDC_BLUEHEX), gtempstring, 2 + 1);
                    number = parse_hexbyte();
                    if (number != -1)
                    {   pencolours[colourset][palettepen] &= 0x00FFFF00;
                        pencolours[colourset][palettepen] |= number;
                        update_palette(FALSE);
                        redrawscreen();
        }   }   }   }
        return FALSE;
    case WM_VSCROLL: // no need for acase
        if (lParam == (long) GetDlgItem(hwnd, IDC_RED))
        {   pencolours[colourset][palettepen] &= 0x0000FFFF;
            pencolours[colourset][palettepen] |= ((ULONG) (255 - SendMessage
            (   GetDlgItem(hwnd, IDC_RED),
                TBM_GETPOS,
                0,
                0
            )) << 16);
            // because LOWORD(lParam) is not always correct

            if (colourset == GREYSCALE)
            {   pencolours[colourset][palettepen] &= 0x00FF0000;
                pencolours[colourset][palettepen] |= (pencolours[colourset][palettepen] >>  8)
                                                |  (pencolours[colourset][palettepen] >> 16);
        }   }
        elif (lParam == (long) GetDlgItem(hwnd, IDC_GREEN))
        {   pencolours[colourset][palettepen] &= 0x00FF00FF;
            pencolours[colourset][palettepen] |= ((ULONG) (255 - SendMessage
            (   GetDlgItem(hwnd, IDC_GREEN),
                TBM_GETPOS,
                0,
                0
            )) << 8);
            // because LOWORD(lParam) is not always correct

            if (colourset == GREYSCALE)
            {   pencolours[colourset][palettepen] &= 0x0000FF00;
                pencolours[colourset][palettepen] |= (pencolours[colourset][palettepen] <<  8)
                                                |  (pencolours[colourset][palettepen] >>  8);
        }   }
        elif (lParam == (long) GetDlgItem(hwnd, IDC_BLUE))
        {   pencolours[colourset][palettepen] &= 0x00FFFF00;
            pencolours[colourset][palettepen] |= (ULONG) (255 - SendMessage
            (   GetDlgItem(hwnd, IDC_BLUE),
                TBM_GETPOS,
                0,
                0
            )); // because LOWORD(lParam) is not always correct

            if (colourset == GREYSCALE)
            {   pencolours[colourset][palettepen] &= 0x000000FF;
                pencolours[colourset][palettepen] |= (pencolours[colourset][palettepen] << 16)
                                                |  (pencolours[colourset][palettepen] <<  8);
        }   }

        update_palette(FALSE);
        redrawscreen();
        return FALSE;
    case WM_ACTIVATE: // no need for acase
        do_autopause(wParam, lParam);
        return TRUE;
    case WM_CTLCOLORSTATIC: // no need for acase
        gid = GetWindowLong((HWND) lParam, GWL_ID);

        if (gid == IDC_PALETTEPREVIEW)
        {   wincolour = ((pencolours[colourset][palettepen] & 0x00FF0000) >> 16)
                      | ((pencolours[colourset][palettepen] & 0x0000FF00)      )
                      | ((pencolours[colourset][palettepen] & 0x000000FF) << 16);
            hLocalBrush = CreateSolidBrush(wincolour);
            return (LRESULT) hLocalBrush;
        } elif (gid >= IDC_PALETTE0 && gid <= IDC_PALETTE23)
        {   wincolour = ((pencolours[colourset][gid - IDC_PALETTE0] & 0x00FF0000) >> 16)
                      | ((pencolours[colourset][gid - IDC_PALETTE0] & 0x0000FF00)      )
                      | ((pencolours[colourset][gid - IDC_PALETTE0] & 0x000000FF) << 16);
            hLocalBrush = CreateSolidBrush(wincolour);
            return (LRESULT) hLocalBrush;
        } else
        {   return FALSE;
        }
    case WM_DESTROY: // no need for acase
        DeleteObject(hLocalBrush);
        return TRUE;
    case WM_MOVE: // no need for acase
        reset_fps();
        return FALSE;
    default: // no need for adefault
        return FALSE;
}   }

MODULE BOOL CALLBACK SpeedDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   FAST    HICON localhicon;
    PERSIST int   oldspeed, newspeed;

    switch (Message)
    {
    case WM_INITDIALOG:
        DISCARD SetWindowText(hwnd,                LLL(MSG_HAIL_ADJUSTSPEED, "Adjust Speed")      );
        setdlgtext(           hwnd, IDL_EMUSPEED,      MSG_HAIL_EMUSPEED,    "Emulation Speed"    );
        setdlgtext(           hwnd, IDL_FRAMESKIP,     MSG_HAIL_FRAMESKIP,   "Frame Skipping"     );
        setdlgtext(           hwnd, IDC_AUTOFRAMESKIP, MSG_AUTOFRAMESKIP,    "Increase frame skipping automatically as needed?");
        if (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR)
        {   setdlgtext(       hwnd, IDL_CPL,           MSG_HAIL_CPL,         "Pixels per Scanline");
            sprintf(gtempstring, "%d", cpl);
            SetDlgItemText(hwnd, IDC_CPL_NUM, gtempstring);

            DISCARD SendMessage
            (   GetDlgItem(hwnd, IDC_CPL),
                TBM_SETRANGE,
                FALSE,
                MAKELONG(CPL_MIN, 227)
            );
            DISCARD SendMessage
            (   GetDlgItem(hwnd, IDC_CPL),
                TBM_SETPOS,
                TRUE,
                cpl
            );
        }
        if (machine == PIPBUG || machine == BINBUG || machine == CD2650 || machine == SELBST)
        {   setdlgtext(       hwnd, IDL_CLOCKSPEED,    MSG_HAIL_CLOCKSPEED,  "Clock Speed"        );
            setfont(hwnd, IDC_CLOCKSPEED_1ST);
            setfont(hwnd, IDC_CLOCKSPEED_2ND);
        }
        if (machine == PIPBUG || machine == BINBUG || machine == TWIN)
        {   setdlgtext(       hwnd, IDL_BAUDRATE,      MSG_HAIL_BAUDRATE,    "Baud Rate"          );
        }

        sprintf(gtempstring, "%d", speedupnum[speedup]);
        SetDlgItemText(hwnd, IDC_EMUSPEED_NUM, gtempstring);
        SendMessage(GetDlgItem(hwnd, IDC_ADJUSTSPEED), TBM_SETRANGE, FALSE, MAKELONG(SPEED_MIN, SPEED_MAX));
        SendMessage(GetDlgItem(hwnd, IDC_ADJUSTSPEED), TBM_SETPOS,   TRUE,  speedup);

        if (frameskip == FRAMESKIP_INFINITE)
        {   SetDlgItemText(hwnd, IDC_FRAMESKIP_NUM, "Inf."); // localize
        } else
        {   sprintf(gtempstring, "%d", frameskip);
            SetDlgItemText(hwnd, IDC_FRAMESKIP_NUM, gtempstring);
        }
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_FRAMESKIP),
            TBM_SETRANGE,
            FALSE,
            MAKELONG(FRAMESKIP_MIN, FRAMESKIP_INFINITE)
        );
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_FRAMESKIP),
            TBM_SETPOS,
            TRUE,
            frameskip
        );
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_AUTOFRAMESKIP),
            BM_SETCHECK,
            autoframeskip ? BST_CHECKED : BST_UNCHECKED,
            0
        );

        localhicon = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon), IMAGE_ICON, 32, 32, 0);
        DISCARD SendMessage(GetDlgItem(hwnd, IDL_CLOCKSPEEDGLYPH), STM_SETICON, (WPARAM) localhicon, (LPARAM) 0);

        switch (machine)
        {
        case PIPBUG:
        case BINBUG:
            sprintf(gtempstring, "1 %s", LLL(MSG_MHZ, "MHz"));
            SetDlgItemText(hwnd, IDC_CLOCKSPEED_1ST, gtempstring);
            sprintf(gtempstring, "2 %s", LLL(MSG_MHZ, "MHz"));
            SetDlgItemText(hwnd, IDC_CLOCKSPEED_2ND, gtempstring);
        acase CD2650:
            sprintf(gtempstring, "1.183 %s", LLL(MSG_MHZ, "MHz"));
            SetDlgItemText(hwnd, IDC_CLOCKSPEED_1ST, gtempstring);
            sprintf(gtempstring, "1.774 %s", LLL(MSG_MHZ, "MHz"));
            SetDlgItemText(hwnd, IDC_CLOCKSPEED_2ND, gtempstring);
        acase SELBST:
            sprintf(gtempstring, "1 %s", LLL(MSG_HERTZ, "Hz"));
            SetDlgItemText(hwnd, IDC_CLOCKSPEED_1ST, gtempstring);
            sprintf(gtempstring, "1 %s", LLL(MSG_MHZ, "MHz"));
            SetDlgItemText(hwnd, IDC_CLOCKSPEED_2ND, gtempstring);
        }

        if
        (   (machine == PIPBUG && fastpipbug)
         || (machine == BINBUG && fastbinbug)
         || (machine == CD2650 && fastcd2650)
         || (machine == SELBST && fastselbst)
        )
        {   DISCARD CheckRadioButton(hwnd, IDC_CLOCKSPEED_1ST, IDC_CLOCKSPEED_2ND, IDC_CLOCKSPEED_2ND);
            oldspeed = 1;
        } elif (machine == PIPBUG || machine == BINBUG || machine == CD2650 || machine == SELBST)
        {   DISCARD CheckRadioButton(hwnd, IDC_CLOCKSPEED_1ST, IDC_CLOCKSPEED_2ND, IDC_CLOCKSPEED_1ST);
            oldspeed = 0;
        }
        newspeed = oldspeed;

        switch (machine)
        {
        case PIPBUG:
            DISCARD CheckRadioButton(hwnd, IDC_PIPBUG_110BAUD, IDC_PIPBUG_4800BAUD, IDC_PIPBUG_110BAUD + pipbug_baudrate);
            if (pipbug_biosver != PIPBUG_PIPBUG1BIOS && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
            {   EnableWindow(GetDlgItem(hwnd, IDC_PIPBUG_110BAUD), FALSE);
            }
            if (pipbug_biosver != PIPBUG_HYBUG && pipbug_biosver != PIPBUG_PIPBUG1BIOS && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
            {   EnableWindow(GetDlgItem(hwnd, IDC_PIPBUG_300BAUD), FALSE);
            }
            if (pipbug_biosver != PIPBUG_ARTEMIS && PIPBUG_PIPBUG1BIOS)
            {   EnableWindow(GetDlgItem(hwnd, IDC_PIPBUG_4800BAUD), FALSE);
            }
        acase BINBUG:
            DISCARD CheckRadioButton(hwnd, IDC_BINBUG_150BAUD, IDC_BINBUG_2400BAUD, IDC_BINBUG_150BAUD + binbug_baudrate);
            if (binbug_biosver != BINBUG_61)
            {   EnableWindow(GetDlgItem(hwnd, IDC_BINBUG_150BAUD ), FALSE);
                EnableWindow(GetDlgItem(hwnd, IDC_BINBUG_1200BAUD), FALSE);
                EnableWindow(GetDlgItem(hwnd, IDC_BINBUG_2400BAUD), FALSE);
            }
     /* acase TWIN:
            DISCARD CheckRadioButton(hwnd, IDC_TWIN_110BAUD, IDC_TWIN_9600BAUD, IDC_TWIN_110BAUD + twin_baudrate); */
        }

        move_subwindow(SUBWINDOW_SPEED, hwnd);

        return FALSE;
    case WM_ACTIVATE: // no need for acase
        do_autopause(wParam, lParam);
    acase WM_HSCROLL:
        if (lParam == (long) GetDlgItem(hwnd, IDC_ADJUSTSPEED))
        {   speedup = SendMessage
            (   GetDlgItem(hwnd, IDC_ADJUSTSPEED),
                TBM_GETPOS,
                0,
                0
            ); // because LOWORD(lParam) is not always correct
            if (cheevos && RA_HardcoreModeIsActive() && speedup < SPEED_4QUARTERS)
            {   speedup = SPEED_4QUARTERS;
                DISCARD SendMessage
                (   GetDlgItem(hwnd, IDC_ADJUSTSPEED),
                    TBM_SETPOS,
                    TRUE,
                    speedup
                );
            }
            sprintf(gtempstring, "%d", speedupnum[speedup]);
            SetDlgItemText(hwnd, IDC_EMUSPEED_NUM, gtempstring);

            settitle();
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_FRAMESKIP))
        {   frameskip = SendMessage
            (   GetDlgItem(hwnd, IDC_FRAMESKIP),
                TBM_GETPOS,
                0,
                0
            ); // because LOWORD(lParam) is not always correct
            if (frameskip == FRAMESKIP_INFINITE)
            {   SetDlgItemText(hwnd, IDC_FRAMESKIP_NUM, "Inf."); // localize
            } else
            {   sprintf(gtempstring, "%d", frameskip);
                SetDlgItemText(hwnd, IDC_FRAMESKIP_NUM, gtempstring);
        }   }
        elif (lParam == (long) GetDlgItem(hwnd, IDC_CPL))
        {   set_cpl(SendMessage
            (   GetDlgItem(hwnd, IDC_CPL),
                TBM_GETPOS,
                0,
                0
            )); // because LOWORD(lParam) is not always correct
            sprintf(gtempstring, "%d", cpl);
            SetDlgItemText(hwnd, IDC_CPL_NUM, gtempstring);
        }
    acase WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case  IDC_CLOCKSPEED_1ST:  newspeed = 0;
        acase IDC_CLOCKSPEED_2ND:  newspeed = 1;
        acase IDC_PIPBUG_110BAUD:  pipbug_baudrate = PIPBUG_BAUDRATE_110;  pipbug_changebaud();
        acase IDC_PIPBUG_300BAUD:  pipbug_baudrate = PIPBUG_BAUDRATE_300;  pipbug_changebaud();
        acase IDC_PIPBUG_4800BAUD: pipbug_baudrate = PIPBUG_BAUDRATE_4800; pipbug_changebaud();
        acase IDC_BINBUG_150BAUD:  binbug_baudrate = BINBUG_BAUDRATE_150;  binbug_changebaud();
        acase IDC_BINBUG_300BAUD:  binbug_baudrate = BINBUG_BAUDRATE_300;  binbug_changebaud();
        acase IDC_BINBUG_1200BAUD: binbug_baudrate = BINBUG_BAUDRATE_1200; binbug_changebaud();
        acase IDC_BINBUG_2400BAUD: binbug_baudrate = BINBUG_BAUDRATE_2400; binbug_changebaud();
     /* acase IDC_TWIN_110BAUD:      twin_baudrate =   TWIN_BAUDRATE_110;    twin_changebaud();
        acase IDC_TWIN_150BAUD:      twin_baudrate =   TWIN_BAUDRATE_150;    twin_changebaud();
        acase IDC_TWIN_300BAUD:      twin_baudrate =   TWIN_BAUDRATE_300;    twin_changebaud();
        acase IDC_TWIN_600BAUD:      twin_baudrate =   TWIN_BAUDRATE_600;    twin_changebaud();
        acase IDC_TWIN_1200BAUD:     twin_baudrate =   TWIN_BAUDRATE_1200;   twin_changebaud();
        acase IDC_TWIN_2400BAUD:     twin_baudrate =   TWIN_BAUDRATE_2400;   twin_changebaud();
        acase IDC_TWIN_9600BAUD:     twin_baudrate =   TWIN_BAUDRATE_9600;   twin_changebaud(); */
        acase IDC_AUTOFRAMESKIP:
            if (SendMessage(GetDlgItem(hwnd, IDC_AUTOFRAMESKIP), BM_GETCHECK, 0, 0) == BST_CHECKED)
            {   autoframeskip = TRUE;
            } else
            {   autoframeskip = FALSE;
        }   }

        if (newspeed != oldspeed)
        {   switch (machine)
            {
            case  PIPBUG: fastpipbug = (newspeed == 1) ? TRUE : FALSE;
                          machines[PIPBUG].cpf = fastpipbug ? 40000.0 : 20000.0;
            acase BINBUG: fastbinbug = (newspeed == 1) ? TRUE : FALSE;
                          machines[BINBUG].cpf = fastbinbug ? 40000.0 : 20000.0;
                          patch_acos();
            acase CD2650: fastcd2650 = (newspeed == 1) ? TRUE : FALSE;
                          machines[CD2650].cpf = fastcd2650 ? 29832.0 : 19888.0;
            acase SELBST: fastselbst = (newspeed == 1) ? TRUE : FALSE;
                          machines[SELBST].cpf = fastselbst ? 20000.0 :     0.02;
            }
            update_speed();
            oldspeed = newspeed;
        }
    acase WM_CLOSE:
        clearkybd();
        DestroyWindow(hwnd);
        SubWindowPtr[SUBWINDOW_SPEED] = NULL;
        updatemenu(MENUITEM_ADJUSTSPEED);
        return TRUE;
    case WM_DESTROY: // no need for acase
        SubWindowPtr[SUBWINDOW_SPEED] = NULL;
        return FALSE;
    case WM_MOVE: // no need for acase
        reset_fps();
        return FALSE;
    default: // no need for adefault
        return FALSE;
    }
    return TRUE;
}

MODULE BOOL CALLBACK SpriteDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT LONG        gid;
    TRANSIENT FLAG        ok           = FALSE;
    TRANSIENT PAINTSTRUCT localps;
    FAST      RECT        localrect;
    FAST      POINT       thepoint;
    FAST      int         i, j,
                          mousex, mousey,
                          x, y,
                          t;
    FAST      ULONG       reversed;
    PERSIST   RECT        colourrect[8],
                          spritehexrect[16],
                          spriterect[8][16];
    PERSIST   int         operation,
                          spr_x, spr_y,
                          sprmousedown = FALSE;

    switch (Message)
    {
    case WM_INITDIALOG:
        // remember SubWindowPtr[SUBWINDOW_SPRITES] isn't set up yet!

        DISCARD SetWindowText(hwnd, LLL(MSG_HAIL_SPRITEVIEWER, "Sprite/Character Editor/Viewer"));
     // setdlgtext(hwnd, IDL_WHICHSPRITE,   MSG_SPRITE,        "Sprite:");
        setdlgtext(hwnd, IDL_SPRITEIMAGERY, MSG_SPRITEIMAGERY, "Sprite Imagery");
        setdlgtext(hwnd, IDL_SPRCOLOUR,     MSG_COLOUR,        "Colour:");

        for (i = 0; i < 9; i++)
        {   // 0RGB -> 0BGR
            if (machine == PHUNSY)
            {   switch (i)
                {
                case  0: j =  7; // black
                acase 1: j = 16; // grey #2
                acase 2: j = 17; // grey #3
                acase 3: j = 18; // grey #4
                acase 4: j = 19; // grey #5
                acase 5: j = 20; // grey #6
                acase 6: j = 21; // grey #7
                acase 7: j =  0; // white
                acase 8: j =  6; // blue
            }   }
            else
            {   j = i;
            }
            reversed = ((pencolours[colourset][j] & 0x00FF0000) >> 16)  // red
                     |  (pencolours[colourset][j] & 0x0000FF00)         // green
                     | ((pencolours[colourset][j] & 0x000000FF) << 16); // blue
            hSpriteBrush[i] = CreateSolidBrush(reversed);
        }

        if (machine != BINBUG)
        {   for (i = 0; i < 8; i++)
            {   GetWindowRect(GetDlgItem(hwnd, IDC_COLOUR0 + i), &localrect);
                thepoint.x              = localrect.left;
                thepoint.y              = localrect.top;
                DISCARD ScreenToClient(hwnd, &thepoint);
                colourrect[i].left      = thepoint.x;
                colourrect[i].top       = thepoint.y;
                thepoint.x              = localrect.right;
                thepoint.y              = localrect.bottom;
                DISCARD ScreenToClient(hwnd, &thepoint);
                colourrect[i].right     = thepoint.x;
                colourrect[i].bottom    = thepoint.y;
        }   }

        switch (machine)
        {
        case ARCADIA:
        case CD2650:
            spr_x =
            spr_y =  8;
        acase INTERTON:
        case ELEKTOR:
        case ZACCARIA:
        case MALZAK:
            spr_x =  8;
            spr_y = 10;
        acase BINBUG:
            spr_x =  8;
            spr_y = 16;
        acase PHUNSY:
            spr_x =  6;
            spr_y =  8;
        }
        for (y = 0; y < spr_y; y++)
        {   for (x = 0; x < spr_x; x++)
            {   GetWindowRect(GetDlgItem(hwnd, IDC_SPR_11 + (y * 8) + x), &localrect);
                thepoint.x              = localrect.left;
                thepoint.y              = localrect.top;
                DISCARD ScreenToClient(hwnd, &thepoint);
                spriterect[x][y].left   = thepoint.x;
                spriterect[x][y].top    = thepoint.y;
                thepoint.x              = localrect.right;
                thepoint.y              = localrect.bottom;
                DISCARD ScreenToClient(hwnd, &thepoint);
                spriterect[x][y].right  = thepoint.x;
                spriterect[x][y].bottom = thepoint.y;
            }
            GetWindowRect(GetDlgItem(hwnd, IDC_SPRITEHEX_0 + y), &localrect);
            thepoint.x              = localrect.left;
            thepoint.y              = localrect.top;
            DISCARD ScreenToClient(hwnd, &thepoint);
            spritehexrect[y].left   = thepoint.x;
            spritehexrect[y].top    = thepoint.y;
            thepoint.x              = localrect.right;
            thepoint.y              = localrect.bottom;
            DISCARD ScreenToClient(hwnd, &thepoint);
            spritehexrect[y].right  = thepoint.x;
            spritehexrect[y].bottom = thepoint.y;
        }
        // If they move the subwindow later, it doesn't matter.

        SendMessage(GetDlgItem(hwnd, IDC_WHICHSPRITE), TBM_SETRANGE   , FALSE, MAKELONG(0, maxsprites - 1));
        SendMessage(GetDlgItem(hwnd, IDC_WHICHSPRITE), TBM_SETPAGESIZE,     0,                          1 );
        SendMessage(GetDlgItem(hwnd, IDC_WHICHSPRITE), TBM_SETPOS     ,  TRUE,              viewingsprite );
        DISCARD SetFocus(GetDlgItem(hwnd, IDC_MEMREGION));

        move_subwindow(SUBWINDOW_SPRITES, hwnd);

    return FALSE;
    case WM_CTLCOLORSTATIC: // no need for acase
        gid = GetWindowLong((HWND) lParam, GWL_ID);
        x = (gid - IDC_SPR_11) % 8;
        y = (gid - IDC_SPR_11) / 8;

        if (gid >= IDC_COLOUR0 && gid <= IDC_COLOUR7)
        {   // assert(machine != BINBUG);
            SetBkColor((HDC) wParam, pencolours[colourset][gid - IDC_COLOUR0]);
            return (LRESULT) hSpriteBrush[gid - IDC_COLOUR0];
        } else switch (machine)
        {
        case ARCADIA:
            if (gid >= IDC_SPR_11 && gid <= IDC_SPR_88)
            {   t = 0;
                if (viewingsprite <= 55)
                {   t = (arcadia_pdg[0][viewingsprite     ][y]           & (128 >> x));
                } elif (viewingsprite >= 64)
                {   t = (arcadia_pdg[1][viewingsprite - 64][y]           & (128 >> x));
                } else
                {   // assert(viewingsprite >= 56 && viewingsprite <= 63);
                    t = (memory[0x1980 + ((viewingsprite - 56) * 8) + y] & (128 >> x));
                }
                if (t)
                {   reversed = ((pencolours[colourset][sprviewcolour] & 0x00FF0000) >> 16)  // red
                             |  (pencolours[colourset][sprviewcolour] & 0x0000FF00)         // green
                             | ((pencolours[colourset][sprviewcolour] & 0x000000FF) << 16); // blue
                    SetBkColor((HDC) wParam, reversed);
                    return (LRESULT) hSpriteBrush[sprviewcolour];
                } else
                {   SetBkColor((HDC) wParam, EMUPEN_GREY); // grey
                    return (LRESULT) hSpriteBrush[GREY1];
            }   }
        acase INTERTON:
        case ELEKTOR:
        case ZACCARIA:
        case MALZAK:
            if (gid >= IDC_SPR_11 && gid <= IDC_SPR_108)
            {   if
                (   memory[pvibase + (0x100 * (viewingsprite / 4)) + pvi_spritedata[viewingsprite % 4] + y]
                 &  (128 >> x)
                )
                {   reversed = ((pencolours[colourset][sprviewcolour] & 0x00FF0000) >> 16)  // red
                             |  (pencolours[colourset][sprviewcolour] & 0x0000FF00)         // green
                             | ((pencolours[colourset][sprviewcolour] & 0x000000FF) << 16); // blue
                    SetBkColor((HDC) wParam, reversed);
                    return (LRESULT) hSpriteBrush[sprviewcolour];
                } else
                {   SetBkColor((HDC) wParam, EMUPEN_GREY); // grey
                    return (LRESULT) hSpriteBrush[GREY1];
            }   }
        acase BINBUG:
            if (gid >= IDC_SPR_11 && gid <= IDC_SPR_168)
            {   if (viewingsprite < 128)
                {   t = dg640_chars[      viewingsprite][              y] & (128 >> x);
                } else
                {   t = memory[0x7000 + ((viewingsprite - 128) * 16) + y] & (128 >> x);
                }
                if (t)
                {   reversed = ((pencolours[colourset][sprviewcolour] & 0x00FF0000) >> 16)  // red
                             |  (pencolours[colourset][sprviewcolour] & 0x0000FF00)         // green
                             | ((pencolours[colourset][sprviewcolour] & 0x000000FF) << 16); // blue
                    SetBkColor((HDC) wParam, reversed);
                    return (LRESULT) hSpriteBrush[sprviewcolour];
                } else
                {   SetBkColor((HDC) wParam, EMUPEN_GREY); // grey
                    return (LRESULT) hSpriteBrush[GREY1];
            }   }
        acase CD2650:
            if (gid >= IDC_SPR_11 && gid <= IDC_SPR_88)
            {   t = cd2650_chars_bmp[viewingsprite][y] & (128 >> x);
                if (t)
                {   reversed = ((pencolours[colourset][sprviewcolour] & 0x00FF0000) >> 16)  // red
                             |  (pencolours[colourset][sprviewcolour] & 0x0000FF00)         // green
                             | ((pencolours[colourset][sprviewcolour] & 0x000000FF) << 16); // blue
                    SetBkColor((HDC) wParam, reversed);
                    return (LRESULT) hSpriteBrush[sprviewcolour];
                } else
                {   SetBkColor((HDC) wParam, EMUPEN_GREY); // grey
                    return (LRESULT) hSpriteBrush[GREY1];
            }   }
        acase PHUNSY:
            if (gid >= IDC_SPR_11 && gid <= IDC_SPR_88)
            {   if (viewingsprite < 128)
                {   t = phunsy_chars[viewingsprite][            y] & (0x20 >> x);
                } else
                {   t = phunsy_gfx[( viewingsprite - 128) % 16][y] & (0x20 >> x);
                }
                if (t)
                {   reversed = ((pencolours[colourset][sprviewcolour] & 0x00FF0000) >> 16)  // red
                             |  (pencolours[colourset][sprviewcolour] & 0x0000FF00)         // green
                             | ((pencolours[colourset][sprviewcolour] & 0x000000FF) << 16); // blue
                    SetBkColor((HDC) wParam, reversed);
                    switch (sprviewcolour)
                    {
                    case BLACK: return (LRESULT) hSpriteBrush[0];
                    case GREY2: return (LRESULT) hSpriteBrush[1];
                    case GREY3: return (LRESULT) hSpriteBrush[2];
                    case GREY4: return (LRESULT) hSpriteBrush[3];
                    case GREY5: return (LRESULT) hSpriteBrush[4];
                    case GREY6: return (LRESULT) hSpriteBrush[5];
                    case GREY7: return (LRESULT) hSpriteBrush[6];
                    case WHITE: return (LRESULT) hSpriteBrush[7];
                }   }
                else
                {   SetBkColor((HDC) wParam, EMUPEN_BLUE);
                    return (LRESULT) hSpriteBrush[8];
        }   }   }
    return FALSE;
    case WM_ACTIVATE: // no need for acase
        do_autopause(wParam, lParam);
    acase WM_LBUTTONDBLCLK: // no need for acase
        palettepen = sprviewcolour;
        edit_palette();
    acase WM_HSCROLL:
        gid = GetWindowLong((HWND) lParam, GWL_ID);
        if (gid == IDC_WHICHSPRITE)
        {   viewingsprite = SendMessage
            (   GetDlgItem(SubWindowPtr[SUBWINDOW_SPRITES], IDC_WHICHSPRITE),
                TBM_GETPOS,
                0,
                0
            );
            update_spriteeditor(TRUE);
        }
    return FALSE;
    acase WM_LBUTTONDOWN:
        SetCapture(hwnd);
        mousex = (int) (LOWORD(lParam));
        mousey = (int) (HIWORD(lParam));

        for (i = 0; i < 8; i++)
        {   if
            (   mousex >= colourrect[i].left
             && mousex <= colourrect[i].right
             && mousey >= colourrect[i].top
             && mousey <= colourrect[i].bottom
            )
            {   dosprcolour(i);
                return TRUE;
        }   }

        for (y = 0; y < spr_y; y++)
        {   for (x = 0; x < spr_x; x++)
            {   if
                (   mousex >= spriterect[x][y].left
                 && mousex <= spriterect[x][y].right
                 && mousey >= spriterect[x][y].top
                 && mousey <= spriterect[x][y].bottom
                )
                {   operation = spritebutton(x, y, 2);
                    if (operation != 2)
                    {   sprmousedown = TRUE;
                    }
                    ok = TRUE;
                    break; // for speed
        }   }   }

        if (!ok)
        {   for (y = 0; y < spr_y; y++)
            {   if
                (   (sprad[y][0] != '-' || sprad[y][1] != EOS)
                 && mousex >= spritehexrect[y].left
                 && mousex <= spritehexrect[y].right
                 && mousey >= spritehexrect[y].top
                 && mousey <= spritehexrect[y].bottom
                )
                {   debug_edit(sprad[y], TRUE, FALSE);
                    break; // for speed
        }   }   }

    return TRUE;
    case WM_MOUSEMOVE: // no need for acase
        if (sprmousedown)
        {   mousex = (int) (LOWORD(lParam));
            mousey = (int) (HIWORD(lParam));

            for (y = 0; y < spr_y; y++)
            {   for (x = 0; x < spr_x; x++)
                {   if
                    (   mousex >= spriterect[x][y].left
                     && mousex <= spriterect[x][y].right
                     && mousey >= spriterect[x][y].top
                     && mousey <= spriterect[x][y].bottom
                    )
                    {   DISCARD spritebutton(x, y, operation);
                        break; // for speed
        }   }   }   }
    return TRUE;
    case WM_LBUTTONUP: // no need for acase
        ReleaseCapture();
        sprmousedown = FALSE;
    return TRUE;
    case WM_CLOSE: // no need for acase
        DestroyWindow(hwnd);
        SubWindowPtr[SUBWINDOW_SPRITES] = NULL;
        updatemenu(MENUITEM_SPRITEVIEWER);
    acase WM_DESTROY:
        for (i = 0; i < 9; i++)
        {   DeleteObject(hSpriteBrush[i]);
        }
    acase WM_PAINT:
        DISCARD BeginPaint(hwnd, &localps);
        DISCARD EndPaint(hwnd, &localps);
        clear_preview(hwnd);
        do_preview(TRUE);
        // UpdateWindow(GetDlgItem(hwnd, IDC_PREVIEW));
    return FALSE;
    case WM_MOVE: // no need for acase
        reset_fps();
    return FALSE;
    default: // no need for adefault
    return FALSE;
    }
    return TRUE;
}

MODULE void clear_axes(HWND hwnd)
{   int x, y;
    HDC AxesRastPtr;

    for (y = 0; y < AXESHEIGHT; y++)
    {   for (x = 0; x < AXESWIDTH; x++)
        {   canvasdisplay[CANVAS_AXES][(y * AXESWIDTH) + x] = EMURGBPEN_WHITE;
    }   }

    AxesRastPtr = GetDC(GetDlgItem(hwnd, IDC_AXES));
    DISCARD StretchDIBits
    (   AxesRastPtr,
          0,            // dest leftx
          0,            // dest topy
        AXESWIDTH,      // dest width
        AXESHEIGHT,     // dest height
          0,            // source leftx
          0,            // source topy
        AXESWIDTH,      // source width
        AXESHEIGHT,     // source height
        canvasdisplay[CANVAS_AXES], // pointer to the bits
        (const struct tagBITMAPINFO*) &CanvasBitMapInfo[CANVAS_AXES], // pointer to BITMAPINFO structure
        DIB_RGB_COLORS, // format of data
        SRCCOPY         // blit mode
    );
    ReleaseDC(GetDlgItem(hwnd, IDC_AXES), AxesRastPtr);
}

MODULE void clear_preview(HWND hwnd)
{   int x, y;
    HDC PreviewRastPtr;

    for (y = 0; y < PREVIEWHEIGHT; y++)
    {   for (x = 0; x < PREVIEWWIDTH; x++)
        {   canvasdisplay[CANVAS_PREVIEW][(y * PREVIEWWIDTH) + x] = pencolours[colourset][machine == PHUNSY ? BLUE : GREY1];
    }   }

    PreviewRastPtr = GetDC(GetDlgItem(hwnd, IDC_PREVIEW));
    DISCARD StretchDIBits
    (   PreviewRastPtr,
          0,            // dest leftx
          0,            // dest topy
        PREVIEWWIDTH,   // dest width
        PREVIEWHEIGHT,  // dest height
          0,            // source leftx
          0,            // source topy
        PREVIEWWIDTH,   // source width
        PREVIEWHEIGHT,  // source height
        canvasdisplay[CANVAS_PREVIEW], // pointer to the bits
        (const struct tagBITMAPINFO*) &CanvasBitMapInfo[CANVAS_PREVIEW], // pointer to BITMAPINFO structure
        DIB_RGB_COLORS, // format of data
        SRCCOPY         // blit mode
    );
    ReleaseDC(GetDlgItem(hwnd, IDC_PREVIEW), PreviewRastPtr);
}

EXPORT void setfont(HWND hwnd, int gid)
{   switch (language)
    {
    case  LANGUAGE_GRE: DISCARD SendMessage(GetDlgItem(hwnd, gid), WM_SETFONT, (WPARAM) hGreekFont  , MAKELPARAM(FALSE, 0));
    acase LANGUAGE_POL: DISCARD SendMessage(GetDlgItem(hwnd, gid), WM_SETFONT, (WPARAM) hPolishFont , MAKELPARAM(FALSE, 0));
    acase LANGUAGE_RUS: DISCARD SendMessage(GetDlgItem(hwnd, gid), WM_SETFONT, (WPARAM) hRussianFont, MAKELPARAM(FALSE, 0));
    adefault:           DISCARD SendMessage(GetDlgItem(hwnd, gid), WM_SETFONT, (WPARAM) hFont,        MAKELPARAM(FALSE, 0));
}   }

MODULE BOOL CALLBACK MusicDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   PAINTSTRUCT localps;
    ULONG       scancode;

    switch (Message)
    {
    case WM_INITDIALOG:
        SubWindowPtr[SUBWINDOW_MUSIC] = hwnd;

        DISCARD SetWindowText(hwnd, LLL(MSG_HAIL_MUSIC, "Musical Keyboard"));

        move_subwindow(SUBWINDOW_MUSIC, hwnd);

        return FALSE;
    acase WM_ACTIVATE:
        do_autopause(wParam, lParam);
    acase WM_CLOSE:
        wheremusicmouse[0] =
        wheremusicmouse[1] = -2;
        DestroyWindow(hwnd);
        SubWindowPtr[SUBWINDOW_MUSIC] = NULL;
        updatemenu(MENUITEM_MUSIC);
    acase WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
        SetCapture(hwnd);
        music_mousedown(0, (int) (LOWORD(lParam)), (int) (HIWORD(lParam)));
    acase WM_RBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
        SetCapture(hwnd);
        music_mousedown(1, (int) (LOWORD(lParam)), (int) (HIWORD(lParam)));
    acase WM_MOUSEMOVE:
        if (wheremusicmouse[0] != -2)
        {   music_mousedown(0, (int) (LOWORD(lParam)), (int) (HIWORD(lParam)));
        }
        if (wheremusicmouse[1] != -2)
        {   music_mousedown(1, (int) (LOWORD(lParam)), (int) (HIWORD(lParam)));
        }
    acase WM_LBUTTONUP:
        ReleaseCapture();
        music_mouseup(0);
    acase WM_RBUTTONUP:
        ReleaseCapture();
        music_mouseup(1);
    acase WM_PAINT:
        BeginPaint(hwnd, &localps);
        EndPaint(hwnd, &localps);
        reghost = TRUE;
    return FALSE; // important!
    case WM_KEYDOWN: // no need for acase
        scancode = (lParam & 33488896) >> 16;
        handle_keydown(scancode);
    return FALSE;
    case WM_KEYUP: // no need for acase
        scancode = (lParam & 33488896) >> 16;
        handle_keyup(scancode);
    return FALSE;
    case WM_MOVE: // no need for acase
        reset_fps();
    return FALSE;
    default: // no need for adefault
    return FALSE;
    }
    return TRUE;
}

EXPORT void drawupnote(int whichchan, int whichnote)
{   FAST int y;

    if (whichnote == NOTE_REST)
    {   return;
    }

    draw_note(whichnote, notes[whichnote].colour);
    if (notes[whichnote].ghosted)
    {   ghost_note(whichnote);
    }

    therect.left   =  82 + (40 * whichchan);
    therect.top    =  20;
    therect.right  = 118 + (40 * whichchan);
    therect.bottom = 131;
    DISCARD FillRect(StaveRastPtr, &therect, hBrush[EMUBRUSH_WHITE]);

    for (y = 0; y < 11; y++)
    {   if (y != 5)
        {   therect.left   =  82 + (40 * whichchan);
            therect.top    =  24 + (10 * y);
            therect.right  = 118 + (40 * whichchan);
            therect.bottom =  26 + (10 * y);
            DISCARD FillRect(StaveRastPtr, &therect, hBrush[EMUBRUSH_BLACK]);
}   }   }

EXPORT void drawdownnote(int whichchan, int whichnote, int colour)
{   if (whichnote == NOTE_REST)
    {   return;
    }

    draw_note(whichnote, colour);

    if (notes[whichnote].stavepos == STAVE_C4)
    {   therect.left   = 205 - 113 + (40 * whichchan);
        therect.top    = 217 -  88 - notes[whichnote].stavepos;
        therect.right  = 221 - 113 + (40 * whichchan);
        therect.bottom = 219 -  88 - notes[whichnote].stavepos;
        DISCARD FillRect(StaveRastPtr, &therect, hBrush[EMUBRUSH_BLACK]);
    }

    therect.left   =  96 + (40 * whichchan);
    therect.top    = 126 - notes[whichnote].stavepos;
    therect.right  = 105 + (40 * whichchan);
    therect.bottom = 135 - notes[whichnote].stavepos;
    SelectObject(StaveRastPtr, hBrush[colour]);
    Ellipse
    (   StaveRastPtr,
        therect.left,
        therect.top,
        therect.right,
        therect.bottom
    );

    if (notes[whichnote].sharp)
    {   // vertical lines
        MoveToEx(StaveRastPtr, 203 - 113 + (40 * whichchan), 214 - 88 - notes[whichnote].stavepos, NULL);
        LineTo(  StaveRastPtr, 203 - 113 + (40 * whichchan), 222 - 88 - notes[whichnote].stavepos      );
        MoveToEx(StaveRastPtr, 206 - 113 + (40 * whichchan), 214 - 88 - notes[whichnote].stavepos, NULL);
        LineTo(  StaveRastPtr, 206 - 113 + (40 * whichchan), 222 - 88 - notes[whichnote].stavepos      );

        // horizontalish lines
        MoveToEx(StaveRastPtr, 201 - 113 + (40 * whichchan), 217 - 88 - notes[whichnote].stavepos, NULL);
        LineTo(  StaveRastPtr, 209 - 113 + (40 * whichchan), 215 - 88 - notes[whichnote].stavepos      );
        MoveToEx(StaveRastPtr, 201 - 113 + (40 * whichchan), 220 - 88 - notes[whichnote].stavepos, NULL);
        LineTo(  StaveRastPtr, 209 - 113 + (40 * whichchan), 218 - 88 - notes[whichnote].stavepos      );
    }

    if (notes[whichnote].stavepos > STAVE_C4) // 'p'-stem
    {   MoveToEx(StaveRastPtr,  95 + (40 * whichchan), 129 - notes[whichnote].stavepos, NULL);
        LineTo(  StaveRastPtr,  95 + (40 * whichchan), 164 - notes[whichnote].stavepos      );
        MoveToEx(StaveRastPtr,  96 + (40 * whichchan), 129 - notes[whichnote].stavepos, NULL);
        LineTo(  StaveRastPtr,  96 + (40 * whichchan), 164 - notes[whichnote].stavepos      );
    } else // "d"-stem
    {   MoveToEx(StaveRastPtr, 104 + (40 * whichchan),  97 - notes[whichnote].stavepos, NULL);
        LineTo(  StaveRastPtr, 104 + (40 * whichchan), 132 - notes[whichnote].stavepos      );
        MoveToEx(StaveRastPtr, 105 + (40 * whichchan),  97 - notes[whichnote].stavepos, NULL);
        LineTo(  StaveRastPtr, 105 + (40 * whichchan), 132 - notes[whichnote].stavepos      );
}   }

MODULE void draw_note(int whichnote, int colour)
{   FAST int i;

    // assert(whichnote != NOTE_REST);

    therect.left   = (notes[whichnote].first_leftx       -  6    ) * 2;
    therect.top    = (NOTES_FIRST_TOPY                   - 11    ) * 2;
    therect.right  = (notes[whichnote].first_rightx      -  6 + 1) * 2;
    therect.bottom = (NOTES_FIRST_BOTTOMY                - 11 + 1) * 2;
    DISCARD FillRect(MusicRastPtr, &therect, hBrush[colour]);

    if (notes[whichnote].second_leftx)
    {   therect.left   = (notes[whichnote].second_leftx  -  6    ) * 2;
        therect.top    = (NOTES_SECOND_TOPY              - 11    ) * 2;
        therect.right  = (notes[whichnote].second_rightx -  6 + 1) * 2;
        therect.bottom = (NOTES_SECOND_BOTTOMY           - 11 + 1) * 2;
        DISCARD FillRect(MusicRastPtr, &therect, hBrush[colour]);
}   }

EXPORT void ghost_note(int whichnote)
{   // assert(whichnote != NOTE_REST);
    // assert(MusicRastPtr);

    if (notes[whichnote].colour == EMUBRUSH_BLACK)
    {   therect.left   = (notes[whichnote].first_leftx   -  6 + 1) * 2;
        therect.top    = (NOTES_FIRST_TOPY               - 11    ) * 2;
        therect.right  = (notes[whichnote].first_rightx  -  6    ) * 2;
        therect.bottom = (NOTES_FIRST_BOTTOMY            - 11    ) * 2;
    } else
    {   // assert(notes[whichnote].colour == EMUBRUSH_WHITE);
        // assert(notes[whichnote].second_leftx);
        therect.left   = (notes[whichnote].first_leftx   -  6    ) * 2;
        therect.top    = (NOTES_FIRST_TOPY               - 11    ) * 2;
        therect.right  = (notes[whichnote].first_rightx  -  6 + 1) * 2;
        therect.bottom = (NOTES_FIRST_BOTTOMY            - 11 + 1) * 2;
        DISCARD FillRect(MusicRastPtr, &therect, hBrush[EMUBRUSH_GREY]);

        therect.left   = (notes[whichnote].second_leftx  -  6    ) * 2;
        therect.top    = (NOTES_SECOND_TOPY              - 11    ) * 2;
        therect.right  = (notes[whichnote].second_rightx -  6 + 1) * 2;
        therect.bottom = (NOTES_SECOND_BOTTOMY           - 11 + 1) * 2;
    }
    DISCARD FillRect(MusicRastPtr, &therect, hBrush[EMUBRUSH_GREY]);

    notes[whichnote].ghosted = TRUE;
}

EXPORT void ghost_bar(int whichbar)
{   HDC  StaveRastPtr;
    RECT localrect;
    int  i;

    StaveRastPtr = GetDC(GetDlgItem(SubWindowPtr[SUBWINDOW_MUSIC], IDC_STAVE));

    for (i = 0; i < 4; i++)
    {   localrect.left   =  81 + (40 * whichbar);
        localrect.top    =  26 + ( i * 10);
        localrect.right  = 119 + (40 * whichbar);
        localrect.bottom =  34 + ( i * 10);
        FillRect
        (   StaveRastPtr,
            &localrect,
            hBrush[EMUBRUSH_GREY]
        );
        localrect.top    =  86 + ( i * 10);
        localrect.bottom =  94 + ( i * 10);
        FillRect
        (   StaveRastPtr,
            &localrect,
            hBrush[EMUBRUSH_GREY]
        );
    }
    localrect.top        =  66;
    localrect.bottom     =  84;
    FillRect
    (   StaveRastPtr,
        &localrect,
        hBrush[EMUBRUSH_GREY]
    );

    ReleaseDC(GetDlgItem(SubWindowPtr[SUBWINDOW_MUSIC], IDC_STAVE), StaveRastPtr);
}

MODULE BOOL CALLBACK OutputDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT int         gid;
    TRANSIENT ULONG       scancode;
    TRANSIENT PAINTSTRUCT localps;
    FAST      TEXT        tempstring[80 + 1];
    FAST      DWORD       wparam, lparam;

    switch (Message)
    {
    case WM_INITDIALOG:
        SubWindowPtr[SUBWINDOW_OUTPUT] = hwnd;

        DISCARD SetWindowText(hwnd, LLL(      MSG_WA_CONSOLE   , "WinArcadia Output"));
        setdlgtext(hwnd, IDC_COPYSELECTION  , MSG_COPYSELECTEDTEXT, "Copy selected text");
        setdlgtext(hwnd, IDC_COLOURFUL      , MSG_COLOURFUL    , "Colourful?"       );

        setdlgtext(hwnd, IDL_CONSOLEBG      , MSG_BGCOLOUR     , "Background:"      );
        SendMessage(GetDlgItem(hwnd, IDC_CONSOLEBG), CB_ADDSTRING, (WPARAM) 0, (LPARAM) LLL(MSG_COLOUR2_BLACK, "Black"));
        SendMessage(GetDlgItem(hwnd, IDC_CONSOLEBG), CB_ADDSTRING, (WPARAM) 0, (LPARAM) LLL(MSG_COLOUR2_WHITE, "White"));

        setdlgtext(hwnd, IDC_CLS            , MSG_CLEARWINDOW  , "Clear window"     );

        SendMessage(GetDlgItem(hwnd, IDC_COLOURFUL), BM_SETCHECK , colourlog ? BST_CHECKED : BST_UNCHECKED, 0);

        SendMessage(GetDlgItem(hwnd, IDC_CONSOLEBG), CB_SETCURSEL, (WPARAM) consolebg, (LPARAM) 0);

        /* "In Microsoft Visual Studio 2005 and later, it is possible to
           add a rich edit control into a dialog template by dragging the
           control from the toolbox." Hence we must do it programmatically
           instead for VC6. */
        RichTextGadget = CreateWindowEx
        (   0,
            "RichEdit20A",
            "",
            WS_CHILD
          | WS_BORDER
          | WS_VSCROLL
          | WS_VISIBLE
          | WS_EX_LEFT
          | ES_MULTILINE
          | ES_READONLY
          | WS_EX_CLIENTEDGE,
            10,
            10,
            650,
            332,
            hwnd,
            (HMENU) IDC_OUTPUT,
            InstancePtr,
            NULL
        );
        DISCARD SendMessage(RichTextGadget, WM_SETFONT      , (WPARAM) hMonospaceFont, MAKELPARAM(FALSE, 0));
        if (consolebg == 0) // black
        {   DISCARD SendMessage(RichTextGadget, EM_SETBKGNDCOLOR, 0, RGB(  0,   0,   0));
            fgformat.crTextColor = RGB(255, 255, 255);
            fgformat.crBackColor = RGB(  0,   0,   0);
        } else
        {   // assert(consolebg == 1); // white
            DISCARD SendMessage(RichTextGadget, EM_SETBKGNDCOLOR, 0, RGB(255, 255, 255));
            fgformat.crTextColor = RGB(  0,   0,   0);
            fgformat.crBackColor = RGB(255, 255, 255);
        }
        DISCARD SendMessage(RichTextGadget, EM_SETCHARFORMAT, SCF_ALL, (LPARAM) &fgformat);

        if (subwinx[SUBWINDOW_OUTPUT] == -1 && !fullscreen)
        {   int rightroom,
                bottomroom;

            rightroom  = DisplayWidth  - (winleftx + winwidth);
            bottomroom = DisplayHeight - (wintopy  + winheight);

            if   (rightroom  >= OUTPUTWIDTH ) SetWindowPos(hwnd, NULL, winleftx + winwidth    + 5, wintopy                + 5, 0, 0, SWP_NOSIZE | SWP_NOZORDER); // put at right
            elif (bottomroom >= OUTPUTHEIGHT) SetWindowPos(hwnd, NULL, winleftx               + 5, wintopy + winheight    + 5, 0, 0, SWP_NOSIZE | SWP_NOZORDER); // put at bottom
            elif (winleftx   >= OUTPUTWIDTH ) SetWindowPos(hwnd, NULL, winleftx - OUTPUTWIDTH + 5, wintopy                + 5, 0, 0, SWP_NOSIZE | SWP_NOZORDER); // put at left
            elif (wintopy    >= OUTPUTHEIGHT) SetWindowPos(hwnd, NULL, winleftx               + 5, wintopy - OUTPUTHEIGHT + 5, 0, 0, SWP_NOSIZE | SWP_NOZORDER); // put at bottom
            // else don't move it
        } else
        {   move_subwindow(SUBWINDOW_OUTPUT, hwnd);
        }
    return FALSE;
    case WM_ACTIVATE:
        if (!opening)
        {   do_autopause(wParam, lParam);
            if (LOWORD(wParam) == WA_INACTIVE)
            {   DISCARD SetWindowText(hwnd, LLL(MSG_WA_CONSOLE, "WinArcadia Output"));
            } else // eg, WA_ACTIVE, WA_CLICKACTIVE
            {   sprintf
                (   tempstring,
                    "%s (%s)",
                    LLL(MSG_WA_CONSOLE,    "WinArcadia Output"),
                    LLL(MSG_TABTOACTIVATE, "Tab to activate main window")
                );
                DISCARD SetWindowText(hwnd, tempstring);
        }   }
    acase WM_SETFOCUS:
        SendMessage(RichTextGadget, EM_SETSEL, (WPARAM) -1, (LPARAM) -1);
    acase WM_CLOSE:
        logfile_close();
        DestroyWindow(hwnd);
        SubWindowPtr[SUBWINDOW_OUTPUT] = NULL;
    acase WM_COMMAND:
        gid = (int) LOWORD(wParam);
        switch (gid)
        {
        case IDC_COLOURFUL:
            colourlog = (SendMessage(GetDlgItem(hwnd, IDC_COLOURFUL), BM_GETCHECK, 0, 0) == BST_CHECKED) ? TRUE : FALSE;
            zprintf(TEXTPEN_DEFAULT, "");
        acase IDC_COPYTEXT:
            clip_open(TRUE);
            clip_write(consolestring, strlen(consolestring), CF_TEXT);
            clip_close();
        acase IDC_COPYSELECTION:
            SendMessage(RichTextGadget, EM_GETSEL, (WPARAM) &wparam, (LPARAM) &lparam);
            if (wParam != -1 && lParam != -1)
            {   clip_open(TRUE);
                clip_write(&consolestring[wparam], lparam - wparam, CF_TEXT);
                clip_close();
            }
        acase IDC_CONSOLEBG:
            consolebg = SendMessage(GetDlgItem(hwnd, IDC_CONSOLEBG), CB_GETCURSEL, 0, 0);
            switch (consolebg)
            {
            case 0: // black
                DISCARD SendMessage(RichTextGadget, EM_SETBKGNDCOLOR, 0, RGB(  0,   0,   0));
                fgformat.crTextColor = RGB(255, 255, 255);
                fgformat.crBackColor = RGB(  0,   0,   0);
            acase 1: // white
                DISCARD SendMessage(RichTextGadget, EM_SETBKGNDCOLOR, 0, RGB(255, 255, 255));
                fgformat.crTextColor = RGB(  0,   0,   0);
                fgformat.crBackColor = RGB(255, 255, 255);
            }
            DISCARD SendMessage(RichTextGadget, EM_SETCHARFORMAT, SCF_ALL, (LPARAM) &fgformat);
            zprintf(TEXTPEN_DEFAULT, ""); // to refresh text colours
        acase IDC_CLS:
            cls();
        }
    acase WM_PAINT:
        BeginPaint(hwnd, &localps);
        DISCARD EndPaint(hwnd, &localps);
    return FALSE; // important!
    case WM_MOVE: // no need for acase
        reset_fps();
    return FALSE;
    case WM_KEYDOWN: // no need for acase
        scancode = (lParam & 33488896) >> 16;
        if (scancode == SCAN_TAB)
        {   SetActiveWindow(MainWindowPtr);
        } else
        {   handle_keydown(scancode);
        }
    return FALSE;
    case WM_KEYUP: // no need for acase
        scancode = (lParam & 33488896) >> 16;
        handle_keyup(scancode);
    return FALSE;
    case WM_DRAWITEM: // no need for acase
        drawitem((struct tagDRAWITEMSTRUCT*) lParam, FALSE);
    return TRUE;
    default: // no need for adefault
    return FALSE;
    }
    return TRUE;
}

EXPORT void open_output(FLAG clear)
{   HMODULE hKernel32;

    if (SubWindowPtr[SUBWINDOW_OUTPUT])
    {   return;
    }

    if (climode)
    {   hKernel32 = LoadLibrary("kernel32.dll");
        if (hKernel32)
        {   // Get the address of the AttachConsole function
            AttachConsole_t AttachConsole = (AttachConsole_t)GetProcAddress(hKernel32, "AttachConsole");
            if (AttachConsole)
            {   // Try attaching to the parent process's console
                if (AttachConsole(ATTACH_PARENT_PROCESS))
                {   freopen("CONOUT$", "w", stdout);
                    freopen("CONOUT$", "w", stderr);
                    consoleopened = TRUE;
    }   }   }   }
    else
    {   open_subwindow(SUBWINDOW_OUTPUT, MAKEINTRESOURCE(IDD_OUTPUT), OutputDlgProc);
        OldRichEditProc = (WNDPROC) GetWindowLong(RichTextGadget, GWL_WNDPROC);
        SetWindowLong(RichTextGadget, GWL_WNDPROC, (LONG) RichEditProc);
        if (clear)
        {   outputpos = 0;
        } elif (outputpos)
        {   zprintf(TEXTPEN_DEFAULT, ""); // for old contents
        }
        SetActiveWindow(MainWindowPtr);
        refreshkybd();
}   }
   
EXPORT void redraw_axes(void)
{   FAST HDC AxesRastPtr;

    AxesRastPtr = GetDC(GetDlgItem(SubWindowPtr[SUBWINDOW_MONITOR_XVI], IDC_AXES));
    DISCARD StretchDIBits
    (   AxesRastPtr,
        0,              // dest leftx
        0,              // dest topy
        AXESWIDTH,      // dest width
        AXESHEIGHT,     // dest height
        0,              // source leftx
        0,              // source topy
        AXESWIDTH,      // source width
        AXESHEIGHT,     // source height
        canvasdisplay[CANVAS_AXES], // pointer to the bits
        (const struct tagBITMAPINFO*) &CanvasBitMapInfo[CANVAS_AXES], // pointer to BITMAPINFO structure
        DIB_RGB_COLORS, // format of data
        SRCCOPY         // blit mode
    );
    ReleaseDC(GetDlgItem(SubWindowPtr[SUBWINDOW_MONITOR_XVI], IDC_AXES), AxesRastPtr);
}

EXPORT void setrasgads(int passedcpu)
{   UBYTE t;
    int   i;

    // assert(SUBWINDOW_MONITOR_CPU);

    t = ((whichcpu == passedcpu) ? psu : other_psu) & PSU_SP;

    for (i = 0; i <= 7; i++)
    {   RedrawWindow
        (   GetDlgItem
            (   SubWindowPtr[SUBWINDOW_MONITOR_CPU],
                ibmgad[((passedcpu == 0) ? (MONGAD_RAS0) : (MONGAD_SLAVE_RAS0)) + i]
            ),
            NULL,
            NULL,
            RDW_INVALIDATE | RDW_UPDATENOW
        );
}   }

LRESULT CALLBACK RichEditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{   if ((msg == WM_KEYDOWN || msg == WM_KEYUP) && ((lParam & 33488896) >> 16) == SCAN_TAB)
    {   // Forward the keystroke to the parent window (or to another window)
        HWND hwndParent = GetParent(hwnd);
        PostMessage(hwndParent, msg, wParam, lParam);
        return 0; // Message is handled here
    }

    // Call the original window procedure if necessary
    return CallWindowProc(OldRichEditProc, hwnd, msg, wParam, lParam);
}

EXPORT void bu_enable(int subwin, int gid, BOOL logic)
{   EnableWindow(GetDlgItem(SubWindowPtr[subwin], gid), logic);
}
EXPORT void bu_set(int subwin, int gid)
{   SetDlgItemText(SubWindowPtr[subwin], gid, gtempstring);
}
EXPORT void bu_set2(int subwin, int gid, STRPTR thestring)
{   SetDlgItemText(SubWindowPtr[subwin], gid, thestring);
}
EXPORT void bu_select(int subwin, int gid, BOOL logic)
{   SendMessage(GetDlgItem(SubWindowPtr[subwin], gid), BM_SETCHECK, logic ? BST_CHECKED : BST_UNCHECKED, 0);
}
EXPORT void cb_set(int subwin, int gid, BOOL logic)
{   SendMessage(GetDlgItem(SubWindowPtr[subwin], gid), BM_SETCHECK, logic ? BST_CHECKED : BST_UNCHECKED, 0);
}
EXPORT void cb_enable(int subwin, int gid, BOOL logic)
{   EnableWindow(GetDlgItem(SubWindowPtr[subwin], gid), logic);
}
EXPORT void ch_set(int subwin, int gid, ULONG value)
{   SendMessage(GetDlgItem(SubWindowPtr[subwin], gid), CB_SETCURSEL, (WPARAM) value, (LPARAM)     0);
}
EXPORT void ch2_set(int subwin, int gid, ULONG value)
{   SendMessage(GetDlgItem(SubWindowPtr[subwin], gid), LB_SETCURSEL, (WPARAM) value, (LPARAM)     0);
}
EXPORT void ra_set(int subwin, int firstgid, int lastgid, int value)
{   CheckRadioButton(SubWindowPtr[subwin], firstgid, lastgid, firstgid + value);
}
EXPORT void ra_enable(int subwin, int gid, BOOL logic)
{   EnableWindow(GetDlgItem(SubWindowPtr[subwin], gid), logic ? TRUE : FALSE);
}
EXPORT void ra_enable2(int subwin, int gid, BOOL logic) { ; }
EXPORT void sl_set(int subwin, int gid, ULONG value)
{   SendMessage(GetDlgItem(SubWindowPtr[subwin], gid), TBM_SETPOS  ,          TRUE , (LPARAM) value);
}
EXPORT void sl_set2(int subwin, int gid, ULONG value1, ULONG value2)
{   SendMessage(GetDlgItem(SubWindowPtr[subwin], gid), TBM_SETRANGEMAX, FALSE, (LPARAM) value1); // don't use TBM_SETRANGE, because it only supports 16-bit arguments!
    SendMessage(GetDlgItem(SubWindowPtr[subwin], gid), TBM_SETPOS     , TRUE , (LPARAM) value2);
}
EXPORT void sl_enable(int subwin, int gid, BOOL logic)
{   EnableWindow(GetDlgItem(SubWindowPtr[subwin], gid), logic ? TRUE : FALSE);
}
EXPORT void st_set(int subwin, int gid)
{   SetDlgItemText(SubWindowPtr[subwin], gid, gtempstring);
}
EXPORT void st_set2(int subwin, int gid, STRPTR thestring)
{   SetDlgItemText(SubWindowPtr[subwin], gid, thestring);
}

EXPORT void handle_escdown(int scancode, HWND subwin)
{   if (scancode == SCAN_LSHIFT)
    {   KeyMatrix[SCAN_LSHIFT / 8] |= (1 << (SCAN_LSHIFT % 8));
    } elif (scancode == SCAN_RSHIFT)
    {   KeyMatrix[SCAN_RSHIFT / 8] |= (1 << (SCAN_RSHIFT % 8));
    } elif (scancode == SCAN_ESCAPE)
    {   if (shift())
        {   if (confirm())
            {   cleanexit(EXIT_SUCCESS);
            } else
            {   KeyMatrix[SCAN_LSHIFT / 8] &= ~(1 << (SCAN_LSHIFT % 8));
                KeyMatrix[SCAN_RSHIFT / 8] &= ~(1 << (SCAN_RSHIFT % 8));
        }   }
        else
        {   if   (subwin == SubWindowPtr[SUBWINDOW_TAPEDECK  ]) close_subwindow(SUBWINDOW_TAPEDECK  );
            elif (subwin == SubWindowPtr[SUBWINDOW_INDUSTRIAL]) close_subwindow(SUBWINDOW_INDUSTRIAL);
}   }   }
EXPORT void handle_escup(int scancode)
{   if (scancode == SCAN_LSHIFT)
    {   KeyMatrix[SCAN_LSHIFT / 8] &= ~(1 << (SCAN_LSHIFT % 8));
    } elif (scancode == SCAN_RSHIFT)
    {   KeyMatrix[SCAN_RSHIFT / 8] &= ~(1 << (SCAN_RSHIFT % 8));
}   }

EXPORT void open_magnifier(void)
{   int hostx,  hosty,
        guestx, guesty;

    if (magnifying) // as MagnifierWindowPtr takes some time to open
    {   return;
    }
    magnifying = TRUE;

    hosttoguestmouse(&hostx, &hosty, &guestx, &guesty, NULL, NULL);

    MagnifierWindowPtr = CreateWindowEx
    (   0,
        g_szMagnifierClassName,
        "",
        WS_BORDER | WS_POPUP | WS_VISIBLE,
        hostx - (MAGNIFIERWIDTH  / 2),
        hosty - (MAGNIFIERHEIGHT / 2),
        MAGNIFIERWIDTH,
        MAGNIFIERHEIGHT,
        MainWindowPtr,
        NULL,
        InstancePtr,
        NULL
    );
    update_magnifier();
}

EXPORT void close_magnifier(void)
{   if (MagnifierWindowPtr)
    {   DestroyWindow(MagnifierWindowPtr);
        MagnifierWindowPtr = NULL;
        magnifying = FALSE;
}   }

EXPORT void update_palette(FLAG full)
{   TRANSIENT int i;
    PERSIST   FLAG already = FALSE;

    if (already)
    {   return;
    }
    already = TRUE;

    update_sliders();

    // filled colours-----------------------------------------------------

    if (full)
    {   for (i = 0; i < GUESTCOLOURS; i++)
        {   DISCARD RedrawWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PALETTE], IDC_PALETTE0 + i), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }   }
    else
    {   DISCARD RedrawWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PALETTE], IDC_PALETTE0 + palettepen), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
    DISCARD RedrawWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PALETTE], IDC_PALETTEPREVIEW), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

    checkcolours();
    EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PALETTE], IDC_RESETPALETTE), resetdisabled ? FALSE : TRUE);

    update_spriteeditor(TRUE);

    already = FALSE;
}
