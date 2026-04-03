// INCLUDES---------------------------------------------------------------

#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
    #include "resource.h"
    #include <commctrl.h>
    typedef unsigned char bool;
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
    #include <conio.h>
#endif

#ifdef AMIGA
    #include <math.h>

    #include <intuition/intuition.h>
    #include <intuition/intuitionbase.h>
    #define ALL_REACTION_CLASSES
    #define ALL_REACTION_MACROS
    #include <reaction/reaction.h>
    #include <proto/graphics.h>
    #include <proto/intuition.h>
    #include <proto/locale.h>
    #ifndef __MORPHOS__
         #include <gadgets/clock.h>
    #endif
#endif

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"
#ifdef AMIGA
    #include "amiga.h"
#endif

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT     FLAG           oldspinning;
EXPORT     UBYTE          papertapebyte[2];
EXPORT     ULONG          papertapelength[2] = { 0, 0 },
                          papertapewhere[ 2] = { 0, 0 };
#ifdef WIN32
    EXPORT HICON          spinicon[2];
#endif

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT     TEXT           fn_tape[4][MAX_PATH + 1],
                          gtempstring[256 + 1],
                          papertapetitlestring[40 + MAX_PATH + 1],
                          tapeposstring[13 + 1],
                          tapetitlestring[40 + MAX_PATH + 1];
IMPORT     UBYTE          tapebyte,
                          tapeskewage;
IMPORT     ULONG          papertapeprotect[2],
                          samplewhere,
                          tapelength,
                          tape_hz,
                          tapewriteprotect,
                          viewdiskas;
IMPORT     int            diskbyte,
                          drive_mode,
                          machine,
                          mdcrblock,
                          mdcrblocks,
                          mdcrfwdstate,
                          mdcrrevstate,
                          mdcrstate,
                          mdcrlength,
                          mdcrpos,
                          memmap,
                          papertapemode[2],
                          showstatusbars[2],
                          tapeframe,
                          tapemode,
                          viewingdrive,
                          wsm;
IMPORT const UWORD        fileoffset[78];
IMPORT struct DriveStruct   drive[DRIVES_MAX];
IMPORT struct MachineStruct machines[MACHINES];
#ifdef WIN32
    IMPORT int             CatalogPtr; // APTR doesn't work
    IMPORT HICON           diskicon;
    IMPORT HWND            hStatusBar,
                           MainWindowPtr,
                           SubWindowPtr[SUBWINDOWS];
#endif
#ifdef AMIGA
    IMPORT        int      driveglyph_x, driveglyph_y,
                           bloxwidth,
                           bloxheight;
    IMPORT        LONG     emupens[EMUBRUSHES];
    IMPORT        ULONG    emulongpens[EMUBRUSHES],
                           tiptag1;
    IMPORT        ULONG*   bloxlongptr[2][BLOXHEIGHT];
    IMPORT struct Catalog* CatalogPtr;
    IMPORT struct Gadget*  gadgets[GIDS + 1];
    IMPORT struct Image*   images[IMAGES];
    IMPORT struct Window  *MainWindowPtr,
                          *SubWindowPtr[SUBWINDOWS];
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE     UBYTE           blockcontents[256];

MODULE const STRPTR mdcrstatestr[5] = {
"Idle",            // 0 MDCRSTATE_IDLE
"At start",        // 1 MDCRSTATE_ATSTART
"At end",          // 2 MDCRSTATE_ATEND
"Forward",         // 3 MDCRSTATE_FWD
"Reverse",         // 4 MDCRSTATE_REV
}, mdcrfwdstr[8] = {
": idle",          // 0 MDCRFWDSTATE_IDLE
": number read A", // 1 MDCRFWDSTATE_NUMREAD_A
": number read B", // 2 MDCRFWDSTATE_NUMREAD_B
": block read A",  // 3 MDCRFWDSTATE_BLOCKREAD_A
": block read B",  // 4 MDCRFWDSTATE_BLOCKREAD_B
": write",         // 5 MDCRFWDSTATE_WRITE
": write active",  // 6 MDCRFWDSTATE_WRITE_ACTIVE
": init",          // 7 MDCRFWDSTATE_INIT
}, mdcrrevstr[4] = {
": L gap",         // 0 MDCRREVSTATE_LGAP
": data",          // 1 MDCRREVSTATE_DATA
": S gap",         // 2 MDCRREVSTATE_SGAP
": B number",      // 3 MDCRREVSTATE_BNUM
};

#ifdef WIN32
MODULE const struct
{   const int xedge,     yedge,     //                  sector line (extended to edge of square)
              xarcstart, yarcstart, // just beyond      sector line (extended to edge of square)
              xarcend,   yarcend,   // just before next sector line (extended to edge of square)
              xstart,    ystart,    // sector line end
              xend,      yend;      // sector line start
} binbug_sectorpoint[BINBUG_SECTORS] = {
{ 107,   0, 106,   0,  25,   0, 107,   5, 107,  85 }, // sector  1
{  24,   0,  23,   0,   0,  70,  45,  27,  94,  90 }, // sector  2
{   0,  71,   0,  72,   0, 142,  11,  75,  86, 100 }, // sector  3
{   0, 143,   0, 144,  23, 214,  11, 139,  86, 114 }, // sector  4
{  24, 214,  25, 214, 106, 214,  45, 187,  94, 124 }, // sector  5
{ 107, 214, 108, 214, 189, 214, 107, 209, 107, 129 }, // sector  6
{ 190, 214, 191, 214, 214, 144, 169, 187, 120, 124 }, // sector  7
{ 214, 143, 214, 142, 214,  72, 203, 139, 128, 114 }, // sector  8
{ 214,  71, 214,  70, 191,   0, 203,  75, 128, 100 }, // sector  9
{ 190,   0, 189,   0, 108,   0, 169,  27, 120,  90 }, // sector 10
}, cd2650_sectorpoint[CD2650_SECTORS] = {
{ 107,   0, 106,   0,  22,   0, 107,   5, 107,  84 }, // sector 1
{  43-22,0,  20,   0,   0,  79,  43,  28,  92,  89 }, // sector 2
{   0,  80,   0,  81,   0, 163,   8,  82,  85, 101 }, // sector 3
{   0, 164,   0, 165,  59, 214,  18, 155,  87, 118 }, // sector 4
{  60, 214,  61, 214, 152, 214,  67, 201,  98, 128 }, // sector 5
{ 153, 214, 154, 214, 214, 165, 147, 201, 116, 128 }, // sector 6
{ 214, 164, 214, 163, 214,  81, 196, 155, 127, 118 }, // sector 7
{ 214,  80, 214,  79, 197,   0, 206,  82, 129, 101 }, // sector 8
{ 196,   0, 195,   0, 108,   0, 173,  28, 122,  89 }, // sector 9
}, twin_sectorpoint[TWIN_SECTORS] = {
{ 164,   0, 163,   0, 132,   0, 164,   5, 164, 120 }, //  0
{ 131,   0, 130,   0,  91,   0, 133,   8, 155, 121 },
{  90,   0,  89,   0,  50,   0,  99,  19, 146, 124 },
{  49,   0,  48,   0,   1,   0,  73,  34, 139, 128 },
{   0,   0,   0,   1,   0,  48,  52,  52, 133, 133 },
{   0,  49,   0,  50,   0,  89,  34,  73, 128, 139 },
{   0,  90,   0,  91,   0, 130,  19,  99, 124, 146 },
{   0, 131,   0, 132,   0, 163,   8, 133, 121, 155 },
{   0, 164,   0, 165,   0, 196,   5, 164, 120, 164 },
{   0, 197,   0, 198,   0, 237,   8, 195, 121, 173 },
{   0, 238,   0, 239,   0, 278,  19, 229, 124, 182 }, // 10
{   0, 279,   0, 280,   0, 327,  34, 255, 128, 189 },
{   0, 328,   1, 328,  48, 328,  52, 276, 133, 195 },
{  49, 328,  50, 328,  89, 328,  73, 294, 139, 200 },
{  90, 328,  91, 328, 130, 328,  99, 309, 146, 204 },
{ 131, 328, 132, 328, 163, 328, 133, 320, 155, 207 },
{ 164, 328, 165, 328, 196, 328, 164, 323, 164, 208 },
{ 197, 328, 198, 328, 237, 328, 195, 320, 173, 207 },
{ 238, 328, 239, 328, 278, 328, 229, 309, 182, 204 },
{ 279, 328, 280, 328, 327, 328, 255, 294, 189, 200 },
{ 328, 328, 328, 327, 328, 280, 276, 276, 195, 195 }, // 20
{ 328, 279, 328, 278, 328, 239, 294, 255, 200, 189 },
{ 328, 238, 328, 237, 328, 198, 309, 229, 204, 182 },
{ 328, 197, 328, 196, 328, 165, 320, 195, 207, 173 },
{ 328, 164, 328, 163, 328, 132, 323, 164, 208, 164 },
{ 328, 131, 328, 130, 328,  91, 320, 133, 207, 155 },
{ 328,  90, 328,  89, 328,  50, 309,  99, 204, 146 },
{ 328,  49, 328,  48, 328,   1, 294,  73, 200, 139 },
{ 328,   0, 327,   0, 280,   0, 276,  52, 195, 133 },
{ 279,   0, 278,   0, 239,   0, 255,  34, 189, 128 },
{ 238,   0, 237,   0, 198,   0, 229,  19, 182, 124 }, // 30
{ 197,   0, 196,   0, 165,   0, 195,   8, 173, 121 }, // 31
};
#endif

// CODE-------------------------------------------------------------------

EXPORT void update_tapedeck(FLAG force)
{   PERSIST UBYTE oldtapebyte;
    PERSIST ULONG oldsamplewhere,
                  oldtapelength;
    PERSIST int   oldmdcrpos,
                  oldmdcrblock,
                  oldmdcrblocks,
                  oldmdcrfwdstate,
                  oldmdcrrevstate,
                  oldmdcrlength,
                  oldmdcrstate,
                  oldtapeframe,
                  oldtapemode,
                  oldtapeanim;
    PERSIST TEXT  oldfn_tape;
    FAST    TEXT  mdcr_string[60 + 1];
    FAST    int   mins,
                  tapeanim,
                  totalsecs;
    FAST    float totalsecs_f,
                  rem;
#ifdef AMIGA
    FAST    ULONG divisions;
    FAST    int   whichemupen;
#endif

    if (!SubWindowPtr[SUBWINDOW_TAPEDECK])
    {   return;
    }

    if (samplewhere != oldsamplewhere || tapelength != oldtapelength || force)
    {
#ifdef AMIGA
        divisions = (tapelength / (2 * KILOBYTE)) + 1;
        /* "A SLIDER_MAX level beyond 65535 is not supported." - OS4.1 SDK.
            But the limit seems to be much less under OS3.9. 2048 is a safe value. */
        sl_set2(SUBWINDOW_TAPEDECK, IDC_POSITIONSLIDER, tapelength / divisions, samplewhere / divisions);
#endif
#ifdef WIN32
        // Windows doesn't move the slider if we specifiy a TBM_SETRANGEMAX of 0
        sl_set2(SUBWINDOW_TAPEDECK, IDC_POSITIONSLIDER, tapelength ? tapelength : 1, samplewhere);
#endif

        if (tapelength != oldtapelength || force)
        {   if (tapemode == TAPEMODE_NONE || tapelength == 0)
            {   strcpy((char*) gtempstring, "0:00.000000");
            } else
            {   // assert(tape_hz > 0.0);
                totalsecs_f = (float) tapelength / (float) tape_hz;
                totalsecs   = (int) totalsecs_f;
                mins        = totalsecs / 60;
                rem         = totalsecs_f - (float) (mins * 60.0);
                if (rem < 10.0)
                {   sprintf(gtempstring, "%d:0%6f", mins, rem);
                } else
                {   sprintf(gtempstring, "%d:%6f", mins, rem);
            }   }
            bu_set(SUBWINDOW_TAPEDECK, IDC_TAPEEND);
        }

        if (samplewhere != oldsamplewhere || force)
        {   if (tapemode == TAPEMODE_NONE)
            {   strcpy(tapeposstring, "0:00.000000");
            } else
            {   gettapepos();
            }
            bu_set2(SUBWINDOW_TAPEDECK, IDC_TAPEPOS, tapeposstring);
    }   }

    if (tapemode != oldtapemode || force)
    {   tapedeck_settitle();
#ifdef WIN32
        SetWindowText(SubWindowPtr[SUBWINDOW_TAPEDECK], tapetitlestring);

        DISCARD EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_POSITIONSLIDER), (tapemode == TAPEMODE_STOP) ? TRUE : FALSE);

        DISCARD EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_REWIND    ), (samplewhere > 0          && tapemode == TAPEMODE_STOP) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_STOPTAPE  ), (                            tapemode >  TAPEMODE_STOP) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_RECORD    ), (                            tapemode == TAPEMODE_STOP) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_PLAY      ), (samplewhere < tapelength && tapemode == TAPEMODE_STOP) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_FFWD      ), (samplewhere < tapelength && tapemode == TAPEMODE_STOP) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_EJECTTAPE ), (                            tapemode != TAPEMODE_NONE) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_CREATE8SVX), (                            tapemode == TAPEMODE_NONE) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_CREATEAIFF), (                            tapemode == TAPEMODE_NONE) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_CREATEWAV ), (                            tapemode == TAPEMODE_NONE) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_INSERTTAPE), (                            tapemode == TAPEMODE_NONE) ? TRUE : FALSE);

        DISCARD RedrawWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_REWIND    ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        DISCARD RedrawWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_STOPTAPE  ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        DISCARD RedrawWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_RECORD    ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        DISCARD RedrawWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_PLAY      ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        DISCARD RedrawWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_FFWD      ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        DISCARD RedrawWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_EJECTTAPE ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
#endif
#ifdef AMIGA
        SetWindowTitles(SubWindowPtr[SUBWINDOW_TAPEDECK], (const char*) tapetitlestring, (const char*) tapetitlestring);

        SetGadgetAttrs(gadgets[GID_TA_SL2 ], SubWindowPtr[SUBWINDOW_TAPEDECK], NULL, GA_Disabled, (tapemode == TAPEMODE_STOP) ? FALSE : TRUE, TAG_DONE); // this autorefreshes

        SetGadgetAttrs(gadgets[GID_TA_BU1 ], SubWindowPtr[SUBWINDOW_TAPEDECK], NULL, GA_Disabled, (                            tapemode == TAPEMODE_STOP) ? FALSE : TRUE, GA_Selected, (tapemode == TAPEMODE_RECORD) ? TRUE : FALSE, TAG_DONE); // record
        SetGadgetAttrs(gadgets[GID_TA_BU2 ], SubWindowPtr[SUBWINDOW_TAPEDECK], NULL, GA_Disabled, (samplewhere > 0          && tapemode == TAPEMODE_STOP) ? FALSE : TRUE,                                                            TAG_DONE); // rewind
        SetGadgetAttrs(gadgets[GID_TA_BU3 ], SubWindowPtr[SUBWINDOW_TAPEDECK], NULL, GA_Disabled, (samplewhere < tapelength && tapemode == TAPEMODE_STOP) ? FALSE : TRUE, GA_Selected, (tapemode == TAPEMODE_PLAY  ) ? TRUE : FALSE, TAG_DONE); // play
        SetGadgetAttrs(gadgets[GID_TA_BU4 ], SubWindowPtr[SUBWINDOW_TAPEDECK], NULL, GA_Disabled, (samplewhere < tapelength && tapemode == TAPEMODE_STOP) ? FALSE : TRUE,                                                            TAG_DONE); // ffwd
        SetGadgetAttrs(gadgets[GID_TA_BU5 ], SubWindowPtr[SUBWINDOW_TAPEDECK], NULL, GA_Disabled, (                            tapemode >  TAPEMODE_STOP) ? FALSE : TRUE, GA_Selected, (tapemode == TAPEMODE_STOP  ) ? TRUE : FALSE, TAG_DONE); // stop
        SetGadgetAttrs(gadgets[GID_TA_BU6 ], SubWindowPtr[SUBWINDOW_TAPEDECK], NULL, GA_Disabled, (                            tapemode != TAPEMODE_NONE) ? FALSE : TRUE, GA_Selected, (tapemode == TAPEMODE_NONE  ) ? TRUE : FALSE, TAG_DONE); // eject
        SetGadgetAttrs(gadgets[GID_TA_BU7 ], SubWindowPtr[SUBWINDOW_TAPEDECK], NULL, GA_Disabled, (                            tapemode == TAPEMODE_NONE) ? FALSE : TRUE,                                                            TAG_DONE);
        SetGadgetAttrs(gadgets[GID_TA_BU8 ], SubWindowPtr[SUBWINDOW_TAPEDECK], NULL, GA_Disabled, (                            tapemode == TAPEMODE_NONE) ? FALSE : TRUE,                                                            TAG_DONE);
        SetGadgetAttrs(gadgets[GID_TA_BU11], SubWindowPtr[SUBWINDOW_TAPEDECK], NULL, GA_Disabled, (                            tapemode == TAPEMODE_NONE) ? FALSE : TRUE,                                                            TAG_DONE);
        SetGadgetAttrs(gadgets[GID_TA_BU12], SubWindowPtr[SUBWINDOW_TAPEDECK], NULL, GA_Disabled, (                            tapemode == TAPEMODE_NONE) ? FALSE : TRUE,                                                            TAG_DONE);
#endif
    }

    if (tapeframe != oldtapeframe || tapemode != oldtapemode || force)
    {   switch (tapemode)
        {
        case  TAPEMODE_PLAY:
            im_set(    SUBWINDOW_TAPEDECK, IDC_SPOOLER, FIRSTSPOOLANIMIMAGE + TAPEIMAGE_PLAY    + tapeframe);
        acase TAPEMODE_RECORD:
            if (tapewriteprotect)
            {   im_set(SUBWINDOW_TAPEDECK, IDC_SPOOLER, FIRSTSPOOLANIMIMAGE + TAPEIMAGE_PROTECT + tapeframe);
            } else
            {   im_set(SUBWINDOW_TAPEDECK, IDC_SPOOLER, FIRSTSPOOLANIMIMAGE + TAPEIMAGE_RECORD  + tapeframe);
            }
        acase TAPEMODE_STOP:
            im_set(    SUBWINDOW_TAPEDECK, IDC_SPOOLER, FIRSTSPOOLANIMIMAGE + TAPEIMAGE_STOP    + tapeframe);
        acase TAPEMODE_NONE:
            im_set(    SUBWINDOW_TAPEDECK, IDC_SPOOLER, FIRSTSPOOLANIMIMAGE + TAPEIMAGE_EMPTY              );
    }   }

    switch (tapemode)
    {
    case  TAPEMODE_PLAY:   if (tapebyte > tapeskewage) tapeanim = 2; elif (tapebyte < tapeskewage) tapeanim = 0; else tapeanim = 1;
    acase TAPEMODE_RECORD: if (tapebyte > 0x80       ) tapeanim = 2; elif (tapebyte < 0x80       ) tapeanim = 0; else tapeanim = 1;
    acase TAPEMODE_STOP:
    case  TAPEMODE_NONE:   tapeanim = 1;
    }
    if (tapeanim != oldtapeanim || tapemode != oldtapemode || force)
    {   switch (tapemode)
        {
        case  TAPEMODE_PLAY:   im_set(SUBWINDOW_TAPEDECK, IDC_TAPEANIM, FIRSTSPOOLANIMIMAGE + TAPEIMAGE_ANIMPLAY + tapeanim);
        acase TAPEMODE_RECORD: im_set(SUBWINDOW_TAPEDECK, IDC_TAPEANIM, FIRSTSPOOLANIMIMAGE + TAPEIMAGE_ANIMREC  + tapeanim);
        adefault:              im_set(SUBWINDOW_TAPEDECK, IDC_TAPEANIM, FIRSTSPOOLANIMIMAGE + TAPEIMAGE_ANIMSTOP + tapeanim);
    }   }  
    if (tapebyte != oldtapebyte || tapemode != oldtapemode || force)
    {   sprintf(gtempstring, "%02X", tapebyte);
#ifdef WIN32
        SetDlgItemText(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_TAPEBYTE, gtempstring);
#endif
#ifdef AMIGA
        switch (tapemode)
        {
        case  TAPEMODE_PLAY:       SetGadgetAttrs(gadgets[GID_TA_BU17], SubWindowPtr[SUBWINDOW_TAPEDECK], NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[EMUBRUSH_CYAN  ], TAG_DONE);
        acase TAPEMODE_RECORD: if (tapewriteprotect)
                               {   SetGadgetAttrs(gadgets[GID_TA_BU17], SubWindowPtr[SUBWINDOW_TAPEDECK], NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[EMUBRUSH_YELLOW], TAG_DONE);
                               } else
                               {   SetGadgetAttrs(gadgets[GID_TA_BU17], SubWindowPtr[SUBWINDOW_TAPEDECK], NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[EMUBRUSH_RED   ], TAG_DONE);
                               }
        adefault:                  SetGadgetAttrs(gadgets[GID_TA_BU17], SubWindowPtr[SUBWINDOW_TAPEDECK], NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[EMUBRUSH_GREEN ], TAG_DONE);
        }
#endif
    }

    if
    (   samplewhere != oldsamplewhere
     || tapemode    != oldtapemode
     || tapebyte    != oldtapebyte
     || force
    )
    {   update_waveform();
    }

    if (machine == PHUNSY)
    {   if (mdcrpos != oldmdcrpos || force)
        {   mdcr_show(tapeposstring, mdcrpos);
            st_set2(SUBWINDOW_TAPEDECK, IDC_MDCRPOS, tapeposstring);
        }

        if (mdcrlength != oldmdcrlength || force)
        {   mdcr_show(tapeposstring, mdcrlength);
            st_set2(SUBWINDOW_TAPEDECK, IDC_MDCRLENGTH, tapeposstring);
        }

        if (mdcrblock != oldmdcrblock || force)
        {   sprintf(tapeposstring, "%d", mdcrblock);
            st_set2(SUBWINDOW_TAPEDECK, IDC_MDCRBLOCK, tapeposstring);
        }

        if (mdcrblocks != oldmdcrblocks || force)
        {   sprintf(tapeposstring, "%d", mdcrblocks);
            st_set2(SUBWINDOW_TAPEDECK, IDC_MDCRBLOCK, tapeposstring);
        }

        if
        (   force
         || mdcrstate    != oldmdcrstate
         || mdcrfwdstate != oldmdcrfwdstate
         || mdcrrevstate != oldmdcrrevstate
        )
        {   strcpy((char*) mdcr_string, mdcrstatestr[mdcrstate]);
            if (mdcrstate == MDCRSTATE_FWD)
            {   strcat((char*) mdcr_string, mdcrfwdstr[mdcrfwdstate]);
            } elif (mdcrstate == MDCRSTATE_REV)
            {   strcat((char*) mdcr_string, mdcrrevstr[mdcrrevstate]);
            }
#ifdef WIN32
            SetDlgItemText(SubWindowPtr[SUBWINDOW_TAPEDECK], IDC_MDCRSTATUS, mdcr_string);
#endif
#ifdef AMIGA
            if (mdcrblocks == 0)
            {   whichemupen = EMUBRUSH_WHITE;
            } else
            {   if (mdcrstate == MDCRSTATE_FWD)
                {   if (mdcrfwdstate >= MDCRFWDSTATE_NUMREAD_A && mdcrfwdstate <= MDCRFWDSTATE_BLOCKREAD_B) // 1..4
                    {   whichemupen = EMUBRUSH_CYAN;
                    } elif (mdcrfwdstate >= MDCRFWDSTATE_WRITE) // 5..7
                    {   whichemupen = EMUBRUSH_RED;
                    } else
                    {   whichemupen = EMUBRUSH_GREEN;
                }   }
                else
                {   whichemupen = EMUBRUSH_GREEN;
            }   }
            SetGadgetAttrs(gadgets[GID_TA_BU16], SubWindowPtr[SUBWINDOW_TAPEDECK], NULL, GA_Text, mdcr_string, BUTTON_BackgroundPen, emupens[whichemupen], TAG_DONE); // this refreshes automatically
#endif
        }

        if (force || fn_tape[1][0] != oldfn_tape)
        {   oldfn_tape = fn_tape[1][0];
            bu_enable(SUBWINDOW_TAPEDECK, IDC_UPDATEMDCR, (fn_tape[1][0] == EOS) ? FALSE : TRUE);
        }

        oldmdcrpos      = mdcrpos;
        oldmdcrlength   = mdcrlength;
        oldmdcrblock    = mdcrblock;
        oldmdcrblocks   = mdcrblocks;
        oldmdcrstate    = mdcrstate;
        oldmdcrfwdstate = mdcrfwdstate;
        oldmdcrrevstate = mdcrrevstate;
    }

    oldsamplewhere = samplewhere;
    oldtapelength  = tapelength;
    oldtapemode    = tapemode;
    oldtapeframe   = tapeframe;
    oldtapeanim    = tapeanim;
    oldtapebyte    = tapebyte;
}

EXPORT void update_floppydrive(int level, int whichdrive)
{   FAST    int      cluster,
                     howmany,
                     i, j, k,
                     ii,
                     length,
                     localsector,
                     localtrack,
                     startblock, endblock,
                     viewing,
                     where;
    FAST    UBYTE    t;
    PERSIST TEXT     subwintitle[256 + 1];
    PERSIST FLAG     outofrange;
    PERSIST UBYTE    oldtrack,
                     oldsector;
    PERSIST int      oldblockoffset,
                     oldcluster,
                     olddrivemode,
                     oldviewstart;
#ifdef WIN32
    FAST    double   angle,
                     byte_offset,
                     linelength;
    FAST    int      amount;
    FAST    COLORREF whichcolour;
    FAST    HDC      DiskRastPtr;
    FAST    HPEN     CyanPen, PinkPen, PurplePen, BlackPen, GreenPen, OrangePen, RedPen, WhitePen;
#endif
#ifdef AMIGA
    FAST    FLAG     redraw;
    FAST    int      dimicon,
                     glowicon,
                     viewbyte,
                     whichpen,
                     x, y;
#endif

    if (MainWindowPtr && showstatusbars[wsm] && (level >= 2 || drive_mode != olddrivemode || drive[whichdrive].track != oldtrack))
    {   if (whichdrive >= machines[machine].drives)
        {
#ifdef WIN32
            DISCARD SendMessage(hStatusBar, SB_SETTEXT, 4 + whichdrive, (LPARAM) "--");
#endif
#ifdef AMIGA
            SetGadgetAttrs(gadgets[GID_MA_BU3 + whichdrive], MainWindowPtr, NULL, GA_Text, "--", BUTTON_BackgroundPen, ~0, GA_Disabled, TRUE, TAG_END); // this refreshes automatically
#endif
        } else
        {   sprintf(gtempstring, "%02d", drive[whichdrive].track);
#ifdef AMIGA
            switch (drive_mode)
            {
            case  DRIVEMODE_IDLE:    whichpen = EMUBRUSH_BLUE;
            acase DRIVEMODE_READING: whichpen = EMUBRUSH_GREEN;
            acase DRIVEMODE_WRITING: whichpen = EMUBRUSH_YELLOW;
            adefault:                whichpen = EMUBRUSH_WHITE; // should never happen
            }
            SetGadgetAttrs(gadgets[GID_MA_BU3 + whichdrive], MainWindowPtr, NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[whichpen], GA_Disabled, FALSE, TAG_END); // this refreshes automatically
#endif
#ifdef WIN32
            DISCARD SendMessage(hStatusBar, SB_SETTEXT, 4 + whichdrive, (LPARAM) gtempstring);
#endif
        }
#if defined(WIN32) && defined(COLOURSTATUSBAR)
        if (!machines[machine].drives)
        {   DISCARD SendMessage(hStatusBar, SB_SETBKCOLOR, 0, (LPARAM) CLR_DEFAULT);
        } else
        {   switch (drive_mode)
            {
            case  DRIVEMODE_IDLE:    whichcolour = EMUPEN_BLUE;
            acase DRIVEMODE_READING: whichcolour = EMUPEN_GREEN;
            acase DRIVEMODE_WRITING: whichcolour = EMUPEN_YELLOW;
            adefault:                whichcolour = EMUPEN_WHITE; // should never happen
            }
            DISCARD SendMessage(hStatusBar, SB_SETBKCOLOR, 0, (LPARAM) whichcolour);
        }
#endif
    }

    if
    (   SubWindowPtr[SUBWINDOW_FLOPPYDRIVE]
     && whichdrive < machines[machine].drives
     && (machine == TWIN || whichdrive == viewingdrive)
    )
    {   if (machine != TWIN && oldspinning != drive[whichdrive].spinning)
        {
#ifdef WIN32
            SendMessage(GetDlgItem(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], IDC_DISKGLYPH), STM_SETIMAGE, IMAGE_ICON, (LPARAM) spinicon[drive[whichdrive].spinning ? 1 : 0]);
#endif
#ifdef AMIGA
            switch (machine)
            {
            case  BINBUG: dimicon = IMAGE_GLYPH_BINBUG_DIM; glowicon = IMAGE_GLYPH_BINBUG_GLOW;
            acase CD2650: dimicon = IMAGE_GLYPH_CD2650_DIM; glowicon = IMAGE_GLYPH_CD2650_GLOW;
            }
            if (drive[whichdrive].spinning)
            {   images[glowicon]->LeftEdge = driveglyph_x;
                images[glowicon]->TopEdge  = driveglyph_y;
                DrawImage(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE]->RPort, images[glowicon], 0, 0);
            } else
            {   images[dimicon ]->LeftEdge = driveglyph_x;
                images[dimicon ]->TopEdge  = driveglyph_y;
                DrawImage(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE]->RPort, images[dimicon ], 0, 0);
            }
#endif
            oldspinning = drive[whichdrive].spinning;
        }

        switch (machine)
        {
        case  BINBUG: howmany = BINBUG_BLOCKSIZE;
        acase CD2650: howmany = CD2650_BLOCKSIZE;
        acase TWIN:   howmany =   TWIN_BLOCKSIZE;
        }
        get_disk_byte(whichdrive, FALSE); // sets outofrange variable
        viewing = get_viewing_cluster(whichdrive);

#ifdef WIN32
        if (drive[whichdrive].inserted)
        {   DiskRastPtr = GetDC(GetDlgItem(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], (machine == TWIN && whichdrive == 1) ? IDC_PLATTER2 : IDC_PLATTER));
            RedPen    = CreatePen(PS_SOLID, 1, EMUPEN_RED);
            GreenPen  = CreatePen(PS_SOLID, 1, EMUPEN_DARKGREEN);
            OrangePen = CreatePen(PS_SOLID, 1, EMUPEN_DARKORANGE);
            CyanPen   = CreatePen(PS_SOLID, 1, EMUPEN_CYAN);
            PinkPen   = CreatePen(PS_SOLID, 1, EMUPEN_PINK);
            PurplePen = CreatePen(PS_SOLID, 1, EMUPEN_PURPLE);
            BlackPen  = CreatePen(PS_SOLID, 1, EMUPEN_BLACK);
            WhitePen  = CreatePen(PS_SOLID, 1, EMUPEN_WHITE);
        }
#endif

        if (level == 3)
        {   if (machines[machine].drives >= 2)
            {   strcpy(subwintitle, LLL(MSG_HAIL_FLOPPYDRIVES, "Floppy Disk Drives"));
            } else
            {   strcpy(subwintitle, LLL(MSG_HAIL_FLOPPYDRIVE,  "Floppy Disk Drive" ));
            }
            if (machine == TWIN)
            {   for (k = 0; k < 2; k++)
                {   if (drive[k].inserted && drive[k].fn_disk[0])
                    {   length = strlen(drive[k].fn_disk);
                        j = 0;
                        for (i = length - 1; i >= 0; i--)
                        {   if (drive[k].fn_disk[i] == ':' || drive[k].fn_disk[i] == CHAR_PARENT)
                            {   j = i + 1;
                                break;
                        }   }
                        if (k == 0 || !drive[0].inserted || drive[0].fn_disk[0] == EOS) // first
                        {   strcat(subwintitle, ": ");
                        } else
                        {   strcat(subwintitle, " & ");
                        }
                        strcat(subwintitle, &drive[k].fn_disk[j]);
            }   }   }
            else
            {   // assert(whichdrive == viewingdrive);
                ch2_set(SUBWINDOW_FLOPPYDRIVE, IDC_DRIVE, whichdrive); // ordinal number in list
                if (drive[whichdrive].inserted && drive[whichdrive].fn_disk[0])
                {   length = strlen(drive[whichdrive].fn_disk);
                    j = 0;
                    for (i = length - 1; i >= 0; i--)
                    {   if (drive[whichdrive].fn_disk[i] == ':' || drive[whichdrive].fn_disk[i] == CHAR_PARENT)
                        {   j = i + 1;
                            break;
                    }   }
                    strcat(subwintitle, ": ");
                    strcat(subwintitle, &drive[whichdrive].fn_disk[j]);
            }   }
#ifdef WIN32
            SetWindowText(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], subwintitle);
#endif
#ifdef AMIGA
            SetWindowTitles(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], (const char*) subwintitle, (const char*) subwintitle);
#endif

            switch (machine)
            {
            case TWIN:
                switch (whichdrive)
                {
                case 0:
                    bu_enable(SUBWINDOW_FLOPPYDRIVE, IDC_UPDATEDISK,  drive[0].inserted ? TRUE : FALSE);
                    bu_enable(SUBWINDOW_FLOPPYDRIVE, IDC_EJECTDISK,   drive[0].inserted ? TRUE : FALSE);
#ifdef WIN32
                    DISCARD SendMessage(GetDlgItem(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], IDC_PLATTER ), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) diskicon);
#endif
                acase 1:
                    bu_enable(SUBWINDOW_FLOPPYDRIVE, IDC_UPDATEDISK2, drive[1].inserted ? TRUE : FALSE);
                    bu_enable(SUBWINDOW_FLOPPYDRIVE, IDC_EJECTDISK2,  drive[1].inserted ? TRUE : FALSE);
#ifdef WIN32
                    DISCARD SendMessage(GetDlgItem(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], IDC_PLATTER2), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) diskicon);
#endif
                }
#ifdef WIN32
                if (drive[whichdrive].inserted)
                {   for (i = 0; i < TWIN_DISKBLOCKS; i++) // 2464
                    {   localtrack  = i / TWIN_SECTORS;
                        localsector = i % TWIN_SECTORS;
                        if (localtrack == 0) // first track is reserved
                        {   SelectObject(DiskRastPtr, PurplePen);
                        } else
                        {   switch (drive[whichdrive].bam[(i - 32) / 8])
                            {
                            case  BAM_LOST: SelectObject(DiskRastPtr, BlackPen);
                            acase BAM_DIR:  SelectObject(DiskRastPtr, PurplePen);
                            acase BAM_FILE: SelectObject(DiskRastPtr, PinkPen);
                            acase BAM_FREE: SelectObject(DiskRastPtr, CyanPen);
                        }   }
#ifdef TESTSECTORLAYOUT
                        SetPixel(DiskRastPtr, twin_sectorpoint[localsector].xarcstart, twin_sectorpoint[localsector].yarcstart, 0x000000FF); // red  ($00BBGGRR format)
                        SetPixel(DiskRastPtr, twin_sectorpoint[localsector].xarcend,   twin_sectorpoint[localsector].yarcend,   0x00FF0000); // blue ($00BBGGRR format)
#endif
                        Arc
                        (   DiskRastPtr,
                              5 + (localtrack * 3 / 2),
                              5 + (localtrack * 3 / 2),
                            324 - (localtrack * 3 / 2),
                            324 - (localtrack * 3 / 2),
                            twin_sectorpoint[localsector].xarcstart,
                            twin_sectorpoint[localsector].yarcstart,
                            twin_sectorpoint[localsector].xarcend,
                            twin_sectorpoint[localsector].yarcend
                        );
                    }

#ifdef TESTSECTORLAYOUT
                    SelectObject(DiskRastPtr, OrangePen);
#else
                    SelectObject(DiskRastPtr, BlackPen);
#endif
                    for (i = 0; i < TWIN_SECTORS; i++)
                    {
#ifdef TESTSECTORLAYOUT
                        MoveToEx(DiskRastPtr, twin_sectorpoint[i].xedge,  twin_sectorpoint[i].yedge,  NULL);
#else
                        MoveToEx(DiskRastPtr, twin_sectorpoint[i].xstart, twin_sectorpoint[i].ystart, NULL);
#endif
                        LineTo(  DiskRastPtr, twin_sectorpoint[i].xend  , twin_sectorpoint[i].yend);
                }   }
#endif
            acase BINBUG:
                bu_enable(SUBWINDOW_FLOPPYDRIVE, IDC_UPDATEDISK,  drive[whichdrive].inserted ? TRUE : FALSE);
                bu_enable(SUBWINDOW_FLOPPYDRIVE, IDC_EJECTDISK,   drive[whichdrive].inserted ? TRUE : FALSE);
#ifdef WIN32
                DISCARD SendMessage(GetDlgItem(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], IDC_PLATTER), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) diskicon);
                if (drive[whichdrive].inserted)
                {   for (i = 0; i < BINBUG_DISKBLOCKS; i++)
                    {   localtrack  =  i / BINBUG_SECTORS;
                        localsector = (i % BINBUG_SECTORS) + 1;
                        switch (drive[whichdrive].bam[i])
                        {
                        case  BAM_LOST: SelectObject(DiskRastPtr, BlackPen);
                        acase BAM_DIR:  SelectObject(DiskRastPtr, PurplePen);
                        acase BAM_FILE: SelectObject(DiskRastPtr, PinkPen);
                        acase BAM_FREE: SelectObject(DiskRastPtr, CyanPen);
                        }
#ifdef TESTSECTORLAYOUT
                        SetPixel(DiskRastPtr, binbug_sectorpoint[localsector - 1].xarcstart, binbug_sectorpoint[localsector - 1].yarcstart, 0x000000FF); // red  ($00BBGGRR format)
                        SetPixel(DiskRastPtr, binbug_sectorpoint[localsector - 1].xarcend,   binbug_sectorpoint[localsector - 1].yarcend,   0x00FF0000); // blue ($00BBGGRR format)
#endif
                        Arc
                        (   DiskRastPtr,
                              5 + (localtrack * 2),
                              5 + (localtrack * 2),
                            210 - (localtrack * 2),
                            210 - (localtrack * 2),
                            binbug_sectorpoint[localsector - 1].xarcstart,
                            binbug_sectorpoint[localsector - 1].yarcstart,
                            binbug_sectorpoint[localsector - 1].xarcend,
                            binbug_sectorpoint[localsector - 1].yarcend
                        );
                    }
#ifdef TESTSECTORLAYOUT
                    SelectObject(DiskRastPtr, OrangePen);
#else
                    SelectObject(DiskRastPtr, BlackPen);
#endif
                    for (i = 0; i < BINBUG_SECTORS; i++)
                    {
#ifdef TESTSECTORLAYOUT
                        MoveToEx(DiskRastPtr, binbug_sectorpoint[i].xedge,  binbug_sectorpoint[i].yedge,  NULL);
#else
                        MoveToEx(DiskRastPtr, binbug_sectorpoint[i].xstart, binbug_sectorpoint[i].ystart, NULL);
#endif
                        LineTo(  DiskRastPtr, binbug_sectorpoint[i].xend,   binbug_sectorpoint[i].yend);
                }   }
#endif
            acase CD2650:
                bu_enable(SUBWINDOW_FLOPPYDRIVE, IDC_UPDATEDISK,  drive[whichdrive].inserted ? TRUE : FALSE);
                bu_enable(SUBWINDOW_FLOPPYDRIVE, IDC_EJECTDISK,   drive[whichdrive].inserted ? TRUE : FALSE);
#ifdef WIN32
                DISCARD SendMessage(GetDlgItem(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], IDC_PLATTER), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) diskicon);
                if (drive[whichdrive].inserted)
                {   for (i = 0; i < CD2650_DISKBLOCKS; i++)
                    {   localtrack  =  i / CD2650_SECTORS;
                        localsector = (i % CD2650_SECTORS) + 1;
                        switch (drive[whichdrive].bam[i])
                        {
                        case  BAM_DIR:  SelectObject(DiskRastPtr, PurplePen);
                        acase BAM_FILE: SelectObject(DiskRastPtr, PinkPen);
                        acase BAM_FREE: SelectObject(DiskRastPtr, CyanPen);
                        }
#ifdef TESTSECTORLAYOUT
                        SetPixel(DiskRastPtr, cd2650_sectorpoint[localsector - 1].xarcstart, cd2650_sectorpoint[localsector - 1].yarcstart, 0x000000FF); // red  ($00BBGGRR format)
                        SetPixel(DiskRastPtr, cd2650_sectorpoint[localsector - 1].xarcend,   cd2650_sectorpoint[localsector - 1].yarcend,   0x00FF0000); // blue ($00BBGGRR format)
#endif
                        amount = (localtrack * 2) + ((localtrack * 2) / 7);
                        Arc
                        (   DiskRastPtr,
                              5 + amount,
                              5 + amount,
                            210 - amount,
                            210 - amount,
                            cd2650_sectorpoint[localsector - 1].xarcstart,
                            cd2650_sectorpoint[localsector - 1].yarcstart,
                            cd2650_sectorpoint[localsector - 1].xarcend,
                            cd2650_sectorpoint[localsector - 1].yarcend
                        );
                    }
#ifdef TESTSECTORLAYOUT
                    SelectObject(DiskRastPtr, OrangePen);
#else
                    SelectObject(DiskRastPtr, BlackPen);
#endif
                    for (i = 0; i < CD2650_SECTORS; i++)
                    {
#ifdef TESTSECTORLAYOUT
                        MoveToEx(DiskRastPtr, cd2650_sectorpoint[i].xedge,  cd2650_sectorpoint[i].yedge,  NULL);
#else
                        MoveToEx(DiskRastPtr, cd2650_sectorpoint[i].xstart, cd2650_sectorpoint[i].ystart, NULL);
#endif
                        LineTo(  DiskRastPtr, cd2650_sectorpoint[i].xend,   cd2650_sectorpoint[i].yend);
                }   }
#endif
        }   }

        if (level == 3 || drive[whichdrive].viewstart != oldviewstart)
        {   if (machine == TWIN)
            {   for (i = 0; i < howmany / 16; i++)
                {   if (outofrange)
                    {   strcpy((char*) gtempstring, "-:");
                    } else
                    {   sprintf((char*) gtempstring, "$%04Xx:", (drive[whichdrive].viewstart / 16) + i);
                    }
                    if (whichdrive == 0) st_set(SUBWINDOW_FLOPPYDRIVE, IDL_FREGION0 + i);
                    else                 st_set(SUBWINDOW_FLOPPYDRIVE, IDL_FREGION8 + i);
                }
                sl_set(SUBWINDOW_FLOPPYDRIVE, (whichdrive == 1) ? IDC_DISKREGION2 : IDC_DISKREGION1,                   drive[whichdrive].viewstart / howmany);
            } else
            {   for (i = 0; i < howmany / 16; i++)
                {   if (outofrange)
                    {   strcpy((char*) gtempstring, "-:");
                    } else
                    {   sprintf((char*) gtempstring, "$%04Xx:", (drive[whichdrive].viewstart / 16) + i);
                    }
                    st_set(SUBWINDOW_FLOPPYDRIVE, IDL_FREGION0 + i);
                }
                sl_set(SUBWINDOW_FLOPPYDRIVE,                                       IDC_DISKREGION1, outofrange ? 0 : (drive[whichdrive].viewstart / howmany));
        }   }

        if (drive[whichdrive].inserted)
        {
#ifdef WIN32
            for (i = 0; i < howmany; i++)
            {   if (!outofrange)
                {   t = drive[whichdrive].contents[drive[whichdrive].viewstart + i];
                }
                ii = i;
                if (machine == TWIN && whichdrive == 1)
                {   ii += 128;
                }
                if
                (   level                       >= 2
                 || t                           != blockcontents[ii]
                 || i                           == drive[whichdrive].blockoffset
                 || i                           == oldblockoffset
                 || drive[whichdrive].viewstart != oldviewstart
                )
                {   if (outofrange)
                    {   gtempstring[0] = '-';
                        gtempstring[1] = EOS;
                    } else
                    {   blockcontents[ii] = t;
                        if (viewdiskas == 0 || (machine == BINBUG && i <= 1))
                        {   hex1(gtempstring, t);
                            gtempstring[2] = EOS;
                        } else
                        {   gtempstring[0] = guestchar(t);
                            gtempstring[1] = EOS;
                    }   }
                    SetDlgItemText(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], ID_DISK_0 + ii, gtempstring);
            }   }

            switch (machine)
            {
            case BINBUG:
#ifdef DISKLINE
                byte_offset = ((drive[whichdrive].sector - 1) * 256) + drive[whichdrive].blockoffset;
                angle = (byte_offset / 2560.0) * 2.0 * PI;
                linelength = 10.0 + ((92.0 * (BINBUG_TRACKS - drive[whichdrive].track)) / BINBUG_TRACKS);
                SetROP2(DiskRastPtr, R2_XORPEN);
                SelectObject(DiskRastPtr, WhitePen);
                MoveToEx(DiskRastPtr, 107, 107, NULL);
                LineTo(DiskRastPtr, (int) (107 - (linelength * sin(angle))), (int) (107 - (linelength * cos(angle))));
                SetROP2(DiskRastPtr, R2_COPYPEN);
#endif

                if (level <= 2)
                {   switch (drive[whichdrive].bam[(oldtrack * BINBUG_SECTORS) + oldsector - 1])
                    {
                    case  BAM_LOST: SelectObject(DiskRastPtr, BlackPen);
                    acase BAM_DIR:  SelectObject(DiskRastPtr, PurplePen);
                    acase BAM_FILE: SelectObject(DiskRastPtr, PinkPen);
                    acase BAM_FREE: SelectObject(DiskRastPtr, CyanPen);
                    }
                    Arc
                    (   DiskRastPtr,
                          5 + (oldtrack * 2),
                          5 + (oldtrack * 2),
                        210 - (oldtrack * 2),
                        210 - (oldtrack * 2),
                        binbug_sectorpoint[oldsector - 1].xarcstart,
                        binbug_sectorpoint[oldsector - 1].yarcstart,
                        binbug_sectorpoint[oldsector - 1].xarcend,
                        binbug_sectorpoint[oldsector - 1].yarcend
                    );
                }

                switch (drive_mode)
                {
                case  DRIVEMODE_READING:
                    SelectObject(DiskRastPtr, GreenPen);
                acase DRIVEMODE_WRITING:
                    SelectObject(DiskRastPtr, RedPen);
                acase DRIVEMODE_IDLE:
                    switch (drive[whichdrive].bam[(drive[whichdrive].track * BINBUG_SECTORS) + drive[whichdrive].sector - 1])
                    {
                    case  BAM_LOST: SelectObject(DiskRastPtr, BlackPen);
                    acase BAM_DIR:  SelectObject(DiskRastPtr, PurplePen);
                    acase BAM_FILE: SelectObject(DiskRastPtr, PinkPen);
                    acase BAM_FREE: SelectObject(DiskRastPtr, CyanPen);
                }   }
                Arc
                (   DiskRastPtr,
                      5 + (drive[whichdrive].track * 2),
                      5 + (drive[whichdrive].track * 2),
                    210 - (drive[whichdrive].track * 2),
                    210 - (drive[whichdrive].track * 2),
                    binbug_sectorpoint[drive[whichdrive].sector - 1].xarcstart,
                    binbug_sectorpoint[drive[whichdrive].sector - 1].yarcstart,
                    binbug_sectorpoint[drive[whichdrive].sector - 1].xarcend,
                    binbug_sectorpoint[drive[whichdrive].sector - 1].yarcend
                );
            acase CD2650:
#ifdef DISKLINE
                byte_offset = ((drive[whichdrive].sector - 1) * 256) + drive[whichdrive].blockoffset;
                angle = (byte_offset / 2304.0) * 2 * PI;
                linelength = 10.0 + ((92.0 * (CD2650_TRACKS - drive[whichdrive].track)) / CD2650_TRACKS);
                SetROP2(DiskRastPtr, R2_XORPEN);
                SelectObject(DiskRastPtr, WhitePen);
                MoveToEx(DiskRastPtr, 107, 107, NULL);
                LineTo(DiskRastPtr, (int) (107 - (linelength * sin(angle))), (int) (107 - (linelength * cos(angle))));
                SetROP2(DiskRastPtr, R2_COPYPEN);
#endif

                if (level <= 2)
                {   switch (drive[whichdrive].bam[(oldtrack * CD2650_SECTORS) + oldsector - 1])
                    {
                    case  BAM_DIR:  SelectObject(DiskRastPtr, PurplePen);
                    acase BAM_FILE: SelectObject(DiskRastPtr, PinkPen);
                    acase BAM_FREE: SelectObject(DiskRastPtr, CyanPen);
                    }
                    amount = (oldtrack * 2) + ((oldtrack * 2) / 7);
                    Arc
                    (   DiskRastPtr,
                          5 + amount,
                          5 + amount,
                        210 - amount,
                        210 - amount,
                        cd2650_sectorpoint[oldsector - 1].xarcstart,
                        cd2650_sectorpoint[oldsector - 1].yarcstart,
                        cd2650_sectorpoint[oldsector - 1].xarcend,
                        cd2650_sectorpoint[oldsector - 1].yarcend
                    );
                }

                switch (drive_mode)
                {
                case  DRIVEMODE_READING:
                    SelectObject(DiskRastPtr, GreenPen);
                acase DRIVEMODE_WRITING:
                    SelectObject(DiskRastPtr, RedPen);
                acase DRIVEMODE_IDLE:
                    switch (drive[whichdrive].bam[(drive[whichdrive].track * CD2650_SECTORS) + drive[whichdrive].sector - 1])
                    {
                    case  BAM_DIR:  SelectObject(DiskRastPtr, PurplePen);
                    acase BAM_FILE: SelectObject(DiskRastPtr, PinkPen);
                    acase BAM_FREE: SelectObject(DiskRastPtr, CyanPen);
                }   }
                amount = (drive[whichdrive].track * 2) + ((drive[whichdrive].track * 2) / 7);
                Arc
                (   DiskRastPtr,
                      5 + amount,
                      5 + amount,
                    210 - amount,
                    210 - amount,
                    cd2650_sectorpoint[drive[whichdrive].sector - 1].xarcstart,
                    cd2650_sectorpoint[drive[whichdrive].sector - 1].yarcstart,
                    cd2650_sectorpoint[drive[whichdrive].sector - 1].xarcend,
                    cd2650_sectorpoint[drive[whichdrive].sector - 1].yarcend
                );
            acase TWIN:
#ifdef DISKLINE
                byte_offset = (drive[whichdrive].sector * 128) + drive[whichdrive].blockoffset;
                angle = (byte_offset / 4096.0) * 2 * PI;
                linelength = 10.0 + ((92.0 * (TWIN_TRACKS - drive[whichdrive].track)) / TWIN_TRACKS);
                SetROP2(DiskRastPtr, R2_XORPEN);
                SelectObject(DiskRastPtr, WhitePen);
                MoveToEx(DiskRastPtr, 107, 107, NULL);
                LineTo(DiskRastPtr, (int) (107 - (linelength * sin(angle))), (int) (107 - (linelength * cos(angle))));
                SetROP2(DiskRastPtr, R2_COPYPEN);
#endif

                if (level <= 2)
                {   if (oldtrack == 0) // first track is reserved
                    {   SelectObject(DiskRastPtr, PurplePen);
                    } else
                    {   switch (drive[whichdrive].bam[(((oldtrack * TWIN_SECTORS) + oldsector) - 32) / 8])
                        {
                        case  BAM_LOST: SelectObject(DiskRastPtr, BlackPen);
                        acase BAM_DIR:  SelectObject(DiskRastPtr, PurplePen);
                        acase BAM_FILE: SelectObject(DiskRastPtr, PinkPen);
                        acase BAM_FREE: SelectObject(DiskRastPtr, CyanPen);
                    }   }
                    Arc
                    (   DiskRastPtr,
                          5 + (oldtrack * 3 / 2),
                          5 + (oldtrack * 3 / 2),
                        324 - (oldtrack * 3 / 2),
                        324 - (oldtrack * 3 / 2),
                        twin_sectorpoint[oldsector].xarcstart,
                        twin_sectorpoint[oldsector].yarcstart,
                        twin_sectorpoint[oldsector].xarcend,
                        twin_sectorpoint[oldsector].yarcend
                    );
                }

                switch (drive_mode)
                {
                case  DRIVEMODE_READING:
                    SelectObject(DiskRastPtr, GreenPen);
                acase DRIVEMODE_WRITING:
                    SelectObject(DiskRastPtr, RedPen);
                acase DRIVEMODE_IDLE:
                    switch (drive[whichdrive].bam[(((drive[whichdrive].track * TWIN_SECTORS) + drive[whichdrive].sector) - 32) / 8])
                    {
                    case  BAM_LOST: SelectObject(DiskRastPtr, BlackPen);
                    acase BAM_DIR:  SelectObject(DiskRastPtr, PurplePen);
                    acase BAM_FILE: SelectObject(DiskRastPtr, PinkPen);
                    acase BAM_FREE: SelectObject(DiskRastPtr, CyanPen);
                }   }
                Arc
                (   DiskRastPtr,
                      5 + (drive[whichdrive].track * 3 / 2),
                      5 + (drive[whichdrive].track * 3 / 2),
                    324 - (drive[whichdrive].track * 3 / 2),
                    324 - (drive[whichdrive].track * 3 / 2),
                    twin_sectorpoint[drive[whichdrive].sector].xarcstart,
                    twin_sectorpoint[drive[whichdrive].sector].yarcstart,
                    twin_sectorpoint[drive[whichdrive].sector].xarcend,
                    twin_sectorpoint[drive[whichdrive].sector].yarcend
                );
            }
#endif

#ifdef AMIGA
            for (i = 0; i < howmany; i++)
            {   t = drive[whichdrive].contents[drive[whichdrive].viewstart + i];
                ii = i;
                if (machine == TWIN && whichdrive == 1)
                {   ii += 128;
                }
                if
                (   level                       >= 2
                 || t                           != blockcontents[ii]
                 || i                           == drive[whichdrive].blockoffset
                 || i                           == oldblockoffset
                 || drive[whichdrive].viewstart != oldviewstart
                )
                {   if (outofrange)
                    {   gtempstring[0] = '-';
                        gtempstring[1] = EOS;
                    } else
                    {   blockcontents[ii] = t;
                        if (viewdiskas == 0 || (machine == BINBUG && i < 2))
                        {   hex1((char*) gtempstring, t);
                            gtempstring[2] = EOS;
                        } else
                        {   gtempstring[0] = guestchar(t);
                            gtempstring[1] = EOS;
                    }   }

                    viewbyte = drive[whichdrive].viewstart + i;

                    if (outofrange)              whichpen = EMUPEN_GREY;
                    elif (viewbyte == diskbyte)
                    {   switch (drive_mode)
                        {
                        case  DRIVEMODE_IDLE:    whichpen = EMUPEN_BLUE;
                        acase DRIVEMODE_READING: whichpen = EMUPEN_GREEN;
                        acase DRIVEMODE_WRITING: whichpen = EMUPEN_YELLOW;
                    }   }
                    elif (drive[whichdrive].flags[viewbyte / 8] & (1 << (viewbyte % 8)))
                    {                            whichpen = EMUPEN_ORANGE;
                    } elif (machine == BINBUG && i <= 1)
                    {                            whichpen = EMUPEN_RED;
                    } elif (machine == TWIN && viewing == -1)
                    {                            whichpen = EMUPEN_PURPLE;
                    } else
                    {   switch (drive[whichdrive].bam[viewing])
                        {
                        case  BAM_LOST:          whichpen = EMUPEN_GREY;
                        acase BAM_DIR:           whichpen = EMUPEN_PURPLE;
                        acase BAM_FILE:          whichpen = EMUPEN_PINK;
                        acase BAM_FREE:          whichpen = EMUPEN_CYAN;
                    }   }

                    SetGadgetAttrs(gadgets[GID_FIRSTDISKGAD + ii], SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], NULL, GA_Disabled, FALSE, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[whichpen], TAG_DONE); // this autorefreshes
            }   }

            redraw = FALSE;
            switch (machine)
            {
            case BINBUG:
                for (i = 0; i < BINBUG_DISKBLOCKS; i++)
                {   if
                    (   level <= 2
                     && i != (               oldtrack * BINBUG_SECTORS) +                oldsector - 1
                     && i != (drive[whichdrive].track * BINBUG_SECTORS) + drive[whichdrive].sector - 1
                    )
                    {   continue;
                    }

                    redraw = TRUE;
                    localtrack  =  i / BINBUG_SECTORS;
                    localsector = (i % BINBUG_SECTORS) + 1;

                    if (drive_mode != DRIVEMODE_IDLE && localtrack == drive[whichdrive].track && localsector == drive[whichdrive].sector)
                    {   switch (drive_mode)
                        {
                        case  DRIVEMODE_READING: drawblox(whichdrive, localtrack, localsector - 1, EMUPEN_GREEN);
                        acase DRIVEMODE_WRITING: drawblox(whichdrive, localtrack, localsector - 1, EMUPEN_RED);
                    }   }
                    else
                    {   switch (drive[whichdrive].bam[i])
                        {
                        case  BAM_LOST: drawblox(whichdrive, localtrack, localsector - 1, EMUPEN_BLACK);
                        acase BAM_DIR:  drawblox(whichdrive, localtrack, localsector - 1, EMUPEN_PURPLE);
                        acase BAM_FILE: drawblox(whichdrive, localtrack, localsector - 1, EMUPEN_PINK);
                        acase BAM_FREE: drawblox(whichdrive, localtrack, localsector - 1, EMUPEN_CYAN);
                }   }   }
            acase CD2650:
                for (i = 0; i < CD2650_DISKBLOCKS; i++)
                {   if
                    (   level <= 2
                     && i != (               oldtrack * CD2650_SECTORS) +                oldsector - 1
                     && i != (drive[whichdrive].track * CD2650_SECTORS) + drive[whichdrive].sector - 1
                    )
                    {   continue;
                    }

                    redraw = TRUE;
                    localtrack  =  i / CD2650_SECTORS;
                    localsector = (i % CD2650_SECTORS) + 1;

                    if (drive_mode != DRIVEMODE_IDLE && localtrack == drive[whichdrive].track && localsector == drive[whichdrive].sector)
                    {   switch (drive_mode)
                        {
                        case  DRIVEMODE_READING: drawblox(whichdrive, localtrack, localsector - 1, EMUPEN_GREEN);
                        acase DRIVEMODE_WRITING: drawblox(whichdrive, localtrack, localsector - 1, EMUPEN_RED);
                    }   }
                    else
                    {   switch (drive[whichdrive].bam[i])
                        {
                        case  BAM_DIR:  drawblox(whichdrive, localtrack, localsector - 1, EMUPEN_PURPLE);
                        acase BAM_FILE: drawblox(whichdrive, localtrack, localsector - 1, EMUPEN_PINK);
                        acase BAM_FREE: drawblox(whichdrive, localtrack, localsector - 1, EMUPEN_CYAN);
                }   }   }
            acase TWIN:
                for (i = 0; i < TWIN_DISKBLOCKS; i++) // 2464
                {   if
                    (   level <= 2
                     && i != (               oldtrack * TWIN_SECTORS) +                oldsector
                     && i != (drive[whichdrive].track * TWIN_SECTORS) + drive[whichdrive].sector
                    )
                    {   continue;
                    }

                    redraw = TRUE;
                    localtrack  = i / TWIN_SECTORS;
                    localsector = i % TWIN_SECTORS;

                    if (drive_mode != DRIVEMODE_IDLE && localtrack == drive[whichdrive].track && localsector == drive[whichdrive].sector)
                    {   switch (drive_mode)
                        {
                        case  DRIVEMODE_READING: drawblox(whichdrive, localtrack, localsector, EMUPEN_GREEN);
                        acase DRIVEMODE_WRITING: drawblox(whichdrive, localtrack, localsector, EMUPEN_RED);
                    }   }
                    else
                    {   if (localtrack == 0) // first track is reserved
                        {   drawblox(whichdrive, localtrack, localsector, EMUPEN_PURPLE);
                        } else
                        {   switch (drive[whichdrive].bam[(i - 32) / 8])
                            {
                            case  BAM_LOST: drawblox(whichdrive, localtrack, localsector, EMUPEN_BLACK);
                            acase BAM_DIR:  drawblox(whichdrive, localtrack, localsector, EMUPEN_PURPLE);
                            acase BAM_FILE: drawblox(whichdrive, localtrack, localsector, EMUPEN_PINK);
                            acase BAM_FREE: drawblox(whichdrive, localtrack, localsector, EMUPEN_CYAN);
            }   }   }   }   }

            if (redraw)
            {   redraw_blox(whichdrive);
            }
#endif
        } else
        {   if (level == 3)
            {   for (i = 0; i < howmany; i++)
                {   if (machine == TWIN && whichdrive == 1)
#ifdef WIN32
                        SetDlgItemText(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], ID_DISK_128 + i, "-");
#endif
#ifdef AMIGA
                        SetGadgetAttrs(gadgets[GID_FIRSTDISKGAD + 128 + i], SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], NULL, GA_Disabled, TRUE, GA_Text, "-", BUTTON_BackgroundPen, ~0, TAG_DONE); // this autorefreshes
#endif
                    else
#ifdef WIN32
                        SetDlgItemText(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], ID_DISK_0   + i, "-");
#endif
#ifdef AMIGA
                        SetGadgetAttrs(gadgets[GID_FIRSTDISKGAD       + i], SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], NULL, GA_Disabled, TRUE, GA_Text, "-", BUTTON_BackgroundPen, ~0, TAG_DONE); // this autorefreshes
#endif
                }
#ifdef AMIGA
                for (y = 0; y < bloxheight; y++)
                {   for (x = 0; x < bloxwidth / 4; x++)
                    {   *(bloxlongptr[(machine == TWIN) ? whichdrive : 0][y] + x) = emulongpens[EMUBRUSH_WHITE];
                }   }
                redraw_blox(whichdrive);
#endif
        }   }

#ifdef AMIGA
        switch (drive_mode)
        {
        case  DRIVEMODE_IDLE:    whichpen = EMUBRUSH_BLUE;
        acase DRIVEMODE_READING: whichpen = EMUBRUSH_GREEN;
        acase DRIVEMODE_WRITING: whichpen = EMUBRUSH_RED;
        adefault:                whichpen = EMUBRUSH_WHITE; // should never happen
        }
#endif

        if (level == 3 || drive_mode != olddrivemode || drive[whichdrive].track != oldtrack)
        {   sprintf((char*) gtempstring, "%d", (int) drive[whichdrive].track);
#ifdef WIN32
            SetDlgItemText(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], (machine != TWIN || whichdrive == 0) ? IDC_TRACK : IDC_TRACK2, gtempstring);
#endif
#ifdef AMIGA
            SetGadgetAttrs(gadgets[(machine != TWIN || whichdrive == 0) ? GID_FL_BU8 : GID_FL_BU16], SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], NULL, BUTTON_BackgroundPen, emupens[whichpen], GA_Text, gtempstring, TAG_DONE); // this autorefreshes
#endif
        }

        if (level == 3 || drive_mode != olddrivemode || drive[whichdrive].sector != oldsector)
        {   sprintf((char*) gtempstring, "%d", (int) drive[whichdrive].sector);
#ifdef WIN32
            SetDlgItemText(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], (machine != TWIN || whichdrive == 0) ? IDC_SECTOR : IDC_SECTOR2, gtempstring);
#endif
#ifdef AMIGA
            SetGadgetAttrs(gadgets[(machine != TWIN || whichdrive == 0) ? GID_FL_BU9 : GID_FL_BU17], SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], NULL, BUTTON_BackgroundPen, emupens[whichpen], GA_Text, gtempstring, TAG_DONE); // this autorefreshes
#endif
        }

        if (level == 3 || drive_mode != olddrivemode || drive[whichdrive].track != oldtrack || drive[whichdrive].sector != oldsector || drive[whichdrive].blockoffset != oldblockoffset)
        {   if (outofrange)
            {   gtempstring[0] = '-';
                gtempstring[1] = EOS;
            } else
            {   switch (machine)
                {
                case  BINBUG: sprintf((char*) gtempstring, "%X", (int) ((drive[whichdrive].track * BINBUG_TRACKSIZE) + ((drive[whichdrive].sector - 1) * BINBUG_BLOCKSIZE) + drive[whichdrive].blockoffset));
                acase TWIN:   sprintf((char*) gtempstring, "%X", (int) ((drive[whichdrive].track *   TWIN_TRACKSIZE) + ( drive[whichdrive].sector      *   TWIN_BLOCKSIZE) + drive[whichdrive].blockoffset));
                acase CD2650: sprintf((char*) gtempstring, "%X", (int) ((drive[whichdrive].track * CD2650_TRACKSIZE) + ((drive[whichdrive].sector - 1) * CD2650_BLOCKSIZE) + drive[whichdrive].blockoffset));
            }   }
#ifdef WIN32
            SetDlgItemText(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], (machine != TWIN || whichdrive == 0) ? IDC_BYTE : IDC_BYTE2, gtempstring);
#endif
#ifdef AMIGA
            SetGadgetAttrs(gadgets[(machine != TWIN || whichdrive == 0) ? GID_FL_BU10 : GID_FL_BU18], SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], NULL, BUTTON_BackgroundPen, emupens[whichpen], GA_Text, gtempstring, TAG_DONE); // this autorefreshes
#endif
        }

        if (machine == TWIN)
        {   cluster = (drive[whichdrive].track == 0) ? (-1) : (((drive[whichdrive].track - 1) * 4) + (drive[whichdrive].sector / 8));
            if (level == 3 || drive_mode != olddrivemode || cluster != oldcluster)
            {   if (drive[whichdrive].track == 0)
                {   sprintf(gtempstring, "-");
                } else
                {   sprintf(gtempstring, "%d", cluster);
                }
#ifdef WIN32
                SetDlgItemText(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], (whichdrive == 0) ? IDC_CLUSTER : IDC_CLUSTER2, gtempstring);
#endif
#ifdef AMIGA
                SetGadgetAttrs(gadgets[(whichdrive == 1) ? GID_FL_BU19 : GID_FL_BU11], SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], NULL, BUTTON_BackgroundPen, emupens[whichpen], GA_Text, gtempstring, TAG_DONE); // this autorefreshes
#endif
            }
            oldcluster = cluster;
        }

        if (level >= 2 || drive[whichdrive].viewstart != oldviewstart)
        {   if (!drive[whichdrive].inserted || outofrange)
            {   strcpy(gtempstring, "-");
#ifdef AMIGA
                whichpen = ~0;
#endif
            } else
            {
#ifdef WIN32
                viewing = get_viewing_cluster(whichdrive); // already done earlier on AMIGA
#endif
                switch (machine)
                {
                case BINBUG:
                    switch (drive[whichdrive].bam[viewing])
                    {
                    case  BAM_LOST: strcpy(gtempstring, "(Lost)");
                    acase BAM_DIR:  strcpy(gtempstring, "(Directory)");
                    acase BAM_FILE:
                        strcpy(            gtempstring, "(File)");
                        where = 0;
                        for (j = 0; j < 10; j++)
                        {   where += 16; // skip identity section
                            for (i = 0; i < 10; i++)
                            {   if (drive[whichdrive].filename[(j * 10) + i][0] != EOS)
                                {   startblock = (drive[whichdrive].contents[where + 13] * 10) + drive[whichdrive].contents[where + 14] - 1;
                                    endblock   = (drive[whichdrive].contents[where + 15] * 10) + drive[whichdrive].contents[where + 16] - 1;
                                    if (viewing >= startblock && viewing <= endblock)
                                    {   sprintf(gtempstring, "%s.%s", drive[whichdrive].filename[(j * 10) + i], drive[whichdrive].fileext[(j * 10) + i]);
                                        break;
                                }   }
                                where += 24;
                        }   }
                    acase BAM_FREE: strcpy(gtempstring, "(Free)");
                    adefault:       strcpy(gtempstring, "(Unknown)"); // should never happen
                    }
                acase TWIN:
                    if (viewing == -1)
                    {   sprintf(           gtempstring, "(Track zero)");
                    } else switch (drive[whichdrive].bam[viewing])
                    {
                    case  BAM_LOST: strcpy(gtempstring, "(Lost)");
                    acase BAM_DIR:  strcpy(gtempstring, "(Directory)");
                    acase BAM_FILE:
                        strcpy(            gtempstring, "(File)");
                        for (i = 0; i < 78; i++)
                        {   if
                            (   drive[whichdrive].filename[i][0] != EOS
                             && drive[whichdrive].contents[fileoffset[i] + (viewing / 8)] & (0x80 >> (viewing % 8))
                            )
                            {   strcpy(gtempstring, drive[whichdrive].filename[i]);
                                twin_get_commands(whichdrive, i, FALSE); // extends gtempstring
                                break;
                        }   }
                    acase BAM_FREE: strcpy(gtempstring, "(Free)");
                    adefault:       strcpy(gtempstring, "(Unknown)"); // should never happen
                    }
                acase CD2650:
                    switch (drive[whichdrive].bam[viewing])
                    {
                    // there is no BAM_LOST currently implemented for CD2650
                    case  BAM_DIR:  strcpy(gtempstring, "(Directory)");
                    acase BAM_FILE: strcpy(gtempstring, "(File)");
                        for (i = 0; i < 64; i++)
                        {   if (drive[whichdrive].filename[i][0] != EOS)
                            {   startblock = (drive[whichdrive].contents[(i * 64) + 28] * CD2650_SECTORS) // track
                                           +  drive[whichdrive].contents[(i * 64) + 29] - 1;              // sector
                                endblock   = (drive[whichdrive].contents[(i * 64) + 30] * CD2650_SECTORS) // track
                                           +  drive[whichdrive].contents[(i * 64) + 31] - 1;              // sector
                                if (viewing >= startblock && viewing <= endblock)
                                {   sprintf(gtempstring, "%s.%s", drive[whichdrive].filename[i], drive[whichdrive].fileext[i]);
                                    break;
                        }   }   }
                    acase BAM_FREE: strcpy(gtempstring, "(Free)");
                    adefault:       strcpy(gtempstring, "(Unknown)"); // should never happen
                }   }

#ifdef AMIGA
                if (viewing == -1)
                {   // assert(machine == TWIN);
                    whichpen = emupens[EMUPEN_PURPLE];
                } else switch (drive[whichdrive].bam[viewing])
                {
                case  BAM_LOST: whichpen = emupens[EMUPEN_GREY  ]; // ideally EMUPEN_BLACK but text would be illegible
                acase BAM_DIR:  whichpen = emupens[EMUPEN_PURPLE];
                acase BAM_FILE: whichpen = emupens[EMUPEN_PINK  ];
                acase BAM_FREE: whichpen = emupens[EMUPEN_CYAN  ];
                }
#endif
            }
#ifdef WIN32
            SetDlgItemText(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], (machine != TWIN || whichdrive == 0) ? IDC_FILENAME1 : IDC_FILENAME2, gtempstring);
#endif
#ifdef AMIGA
            SetGadgetAttrs(gadgets[(machine != TWIN || whichdrive == 0) ? GID_FL_BU2 : GID_FL_BU3], SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, whichpen, TAG_DONE); // this refreshes automatically
#endif
         // zprintf(TEXTPEN_VERBOSE, "%s\n", gtempstring);

#ifdef WIN32
            update_floppytips();
#endif
#ifdef AMIGA
            if (tiptag1 != TAG_IGNORE)
            {   make_floppytips();
            }
#endif
        }

#ifdef WIN32
        if (drive[whichdrive].inserted)
        {   ReleaseDC(GetDlgItem(SubWindowPtr[SUBWINDOW_FLOPPYDRIVE], (machine == TWIN && whichdrive == 1) ? IDC_PLATTER2 : IDC_PLATTER), DiskRastPtr);
            DeleteObject(CyanPen);
            DeleteObject(PinkPen);
            DeleteObject(PurplePen);
            DeleteObject(BlackPen);
            DeleteObject(GreenPen);
            DeleteObject(RedPen);
            DeleteObject(WhitePen);
            DeleteObject(OrangePen);
        }
#endif

        oldtrack       = drive[whichdrive].track;
        oldsector      = drive[whichdrive].sector;
        oldblockoffset = drive[whichdrive].blockoffset;
        oldviewstart   = drive[whichdrive].viewstart;
        olddrivemode   = drive_mode;
}   }
/* The black radial lines (sector dividers) go from x/ystart to x/yend.

You give it a rectangle; the arc is part of an ellipse filling this rectangle.
The startx/y and endx/y points get imaginary lines drawn from them to the centre of the ellipse.
The part of the ellipse between the imaginary lines is the arc.

The rectangle (ellipse) we calculate algorithmically.
x1/y1 is just the point on the "wall" of the disk's "box" where the sector line intersects.
x2/y2 is just the x1/y1 of the next sector. */

EXPORT void update_papertape(int whichunit, FLAG force)
{   PERSIST UBYTE oldpapertapebyte[  2];
    PERSIST ULONG oldpapertapewhere[ 2],
                  oldpapertapelength[2];
    PERSIST int   oldpapertapemode[  2];
#ifdef AMIGA
    FAST    ULONG divisions;
#endif

    if (!SubWindowPtr[SUBWINDOW_PAPERTAPE])
    {   return;
    }

#ifdef AMIGA
    if (papertapewhere[whichunit] != oldpapertapewhere[whichunit] || papertapelength[whichunit] != oldpapertapelength[whichunit] || force)
    {   divisions = (papertapelength[whichunit] / (2 * KILOBYTE)) + 1;
        /* "A SLIDER_MAX level beyond 65535 is not supported." - OS4.1 SDK.
            But limit seems to be much less under OS3.9. 2048 is a safe value. */

        DISCARD SetGadgetAttrs
        (   gadgets[GID_PT_SL1 + whichunit],
            SubWindowPtr[SUBWINDOW_PAPERTAPE], NULL,
            SLIDER_Level,                     papertapewhere[whichunit]  / divisions,
            SLIDER_Max,                       papertapelength[whichunit] / divisions,
        TAG_DONE); // this autorefreshes
    }
#endif

    if (papertapelength[whichunit] != oldpapertapelength[whichunit] || force)
    {
#ifdef WIN32
        DISCARD SendMessage
        (   GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE], whichunit ? IDC_PTPOSITIONSLIDER2 : IDC_PTPOSITIONSLIDER),
            TBM_SETRANGEMAX, // don't use TBM_SETRANGE, because it only supports 16-bit arguments!
            FALSE,
            papertapelength[whichunit] ? papertapelength[whichunit] : 1 // Windows doesn't move the slider if we specifiy a TBM_SETRANGEMAX of 0
        );
#endif
        if (papertapemode[whichunit] == TAPEMODE_NONE || papertapelength[whichunit] == 0)
        {   strcpy(gtempstring, "0:00.0");
        } else
        {   sprintf((char*) gtempstring, "%d:%02d:%d", (int) (papertapelength[whichunit] / 600), (int) ((papertapelength[whichunit] % 600) / 10), (int) (papertapelength[whichunit] % 10));
        }
        bu_set(SUBWINDOW_PAPERTAPE, whichunit ? IDC_PAPERTAPEEND2 : IDC_PAPERTAPEEND);
    }

    if (papertapewhere[whichunit] != oldpapertapewhere[whichunit] || force)
    {
#ifdef WIN32
        DISCARD SendMessage
        (   GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE], whichunit ? IDC_PTPOSITIONSLIDER2 : IDC_PTPOSITIONSLIDER),
            TBM_SETPOS,
            TRUE,
            papertapewhere[whichunit]
        );
#endif
        if (papertapemode[whichunit] == TAPEMODE_NONE)
        {   strcpy(gtempstring, "0:00.0");
        } else
        {   sprintf((char*) gtempstring, "%d:%02d:%d", (int) (papertapewhere[whichunit] / 600), (int) ((papertapewhere[whichunit] % 600) / 10), (int) (papertapewhere[whichunit] % 10));
        }
        bu_set(SUBWINDOW_PAPERTAPE, whichunit ? IDC_PAPERTAPEPOS2 : IDC_PAPERTAPEPOS);
    }

    if (papertapemode[whichunit] != oldpapertapemode[whichunit] || force)
    {   papertape_settitle();
#ifdef WIN32
        SetWindowText(SubWindowPtr[SUBWINDOW_PAPERTAPE], papertapetitlestring);
        EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE], whichunit ? IDC_PTPOSITIONSLIDER2 : IDC_PTPOSITIONSLIDER), (papertapemode[whichunit] == TAPEMODE_STOP) ? TRUE : FALSE);
        if (whichunit == 0)
        {   EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE],                                 IDC_PT_RECORD       ), (papertapewhere[whichunit] < PAPERTAPEMAX               && papertapemode[whichunit] == TAPEMODE_STOP) ? TRUE : FALSE);

            RedrawWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE],                                 IDC_PT_RECORD       ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

            EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE],                                 IDC_CREATEPAPERTAPE ), (                                                          papertapemode[whichunit] == TAPEMODE_NONE) ? TRUE : FALSE);
        }

        EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE], whichunit ? IDC_PT_REWIND2        : IDC_PT_REWIND       ), (papertapewhere[whichunit] > 0                          && papertapemode[whichunit] == TAPEMODE_STOP) ? TRUE : FALSE);
        EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE], whichunit ? IDC_PT_PLAY2          : IDC_PT_PLAY         ), (papertapewhere[whichunit] < papertapelength[whichunit] && papertapemode[whichunit] == TAPEMODE_STOP) ? TRUE : FALSE);
        EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE], whichunit ? IDC_PT_FFWD2          : IDC_PT_FFWD         ), (papertapewhere[whichunit] < papertapelength[whichunit] && papertapemode[whichunit] == TAPEMODE_STOP) ? TRUE : FALSE);
        EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE], whichunit ? IDC_PT_STOPTAPE2      : IDC_PT_STOPTAPE     ), (                                                          papertapemode[whichunit] >  TAPEMODE_STOP) ? TRUE : FALSE);
        EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE], whichunit ? IDC_PT_EJECTTAPE2     : IDC_PT_EJECTTAPE    ), (                                                          papertapemode[whichunit] != TAPEMODE_NONE) ? TRUE : FALSE);

        RedrawWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE], whichunit ? IDC_PT_REWIND2        : IDC_PT_REWIND       ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        RedrawWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE], whichunit ? IDC_PT_STOPTAPE2      : IDC_PT_STOPTAPE     ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        RedrawWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE], whichunit ? IDC_PT_PLAY2          : IDC_PT_PLAY         ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        RedrawWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE], whichunit ? IDC_PT_FFWD2          : IDC_PT_FFWD         ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        RedrawWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE], whichunit ? IDC_PT_EJECTTAPE2     : IDC_PT_EJECTTAPE    ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

        EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_PAPERTAPE], whichunit ? IDC_INSERTPAPERTAPE2  : IDC_INSERTPAPERTAPE ), (                                                          papertapemode[whichunit] == TAPEMODE_NONE) ? TRUE : FALSE);
#endif
#ifdef AMIGA
        SetWindowTitles(SubWindowPtr[SUBWINDOW_PAPERTAPE], (const char*) papertapetitlestring, (const char*) papertapetitlestring);
        SetGadgetAttrs(gadgets[GID_PT_SL1 + whichunit], SubWindowPtr[SUBWINDOW_PAPERTAPE], NULL, GA_Disabled, (papertapemode[whichunit] == TAPEMODE_STOP) ? FALSE : TRUE, TAG_DONE); // this autorefreshes
        if (whichunit == 0)
        {   SetGadgetAttrs(gadgets[                   GID_PT_BU4 ], SubWindowPtr[SUBWINDOW_PAPERTAPE], NULL, GA_Disabled, (papertapewhere[whichunit] < PAPERTAPEMAX               && papertapemode[whichunit] == TAPEMODE_STOP) ? FALSE : TRUE, GA_Selected, (papertapemode[whichunit] == TAPEMODE_RECORD) ? TRUE : FALSE, TAG_DONE); // record

            SetGadgetAttrs(gadgets[                   GID_PT_BU10], SubWindowPtr[SUBWINDOW_PAPERTAPE], NULL, GA_Disabled, (                                                          papertapemode[whichunit] == TAPEMODE_NONE) ? FALSE : TRUE,                                                                            TAG_DONE); // create
        }

        SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU12 : GID_PT_BU5 ], SubWindowPtr[SUBWINDOW_PAPERTAPE], NULL, GA_Disabled, (papertapewhere[whichunit] > 0                          && papertapemode[whichunit] == TAPEMODE_STOP) ? FALSE : TRUE,                                                                            TAG_DONE); // rewind
        SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU13 : GID_PT_BU6 ], SubWindowPtr[SUBWINDOW_PAPERTAPE], NULL, GA_Disabled, (papertapewhere[whichunit] < papertapelength[whichunit] && papertapemode[whichunit] == TAPEMODE_STOP) ? FALSE : TRUE, GA_Selected, (papertapemode[whichunit] == TAPEMODE_PLAY  ) ? TRUE : FALSE, TAG_DONE); // play
        SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU14 : GID_PT_BU7 ], SubWindowPtr[SUBWINDOW_PAPERTAPE], NULL, GA_Disabled, (papertapewhere[whichunit] < papertapelength[whichunit] && papertapemode[whichunit] == TAPEMODE_STOP) ? FALSE : TRUE,                                                                            TAG_DONE); // ffwd
        SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU15 : GID_PT_BU8 ], SubWindowPtr[SUBWINDOW_PAPERTAPE], NULL, GA_Disabled, (                                                          papertapemode[whichunit] >  TAPEMODE_STOP) ? FALSE : TRUE, GA_Selected, (papertapemode[whichunit] == TAPEMODE_STOP  ) ? TRUE : FALSE, TAG_DONE); // stop
        SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU16 : GID_PT_BU9 ], SubWindowPtr[SUBWINDOW_PAPERTAPE], NULL, GA_Disabled, (                                                          papertapemode[whichunit] != TAPEMODE_NONE) ? FALSE : TRUE, GA_Selected, (papertapemode[whichunit] == TAPEMODE_NONE  ) ? TRUE : FALSE, TAG_DONE); // eject

        SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU17 : GID_PT_BU11], SubWindowPtr[SUBWINDOW_PAPERTAPE], NULL, GA_Disabled, (                                                          papertapemode[whichunit] == TAPEMODE_NONE) ? FALSE : TRUE,                                                                            TAG_DONE); // insert
#endif
    }

    if (papertapebyte[whichunit] != oldpapertapebyte[whichunit] || papertapemode[whichunit] != oldpapertapemode[whichunit] || force)
    {   sprintf((char*) gtempstring, "%02X", papertapebyte[whichunit]);
#ifdef WIN32
        SetDlgItemText(SubWindowPtr[SUBWINDOW_PAPERTAPE], whichunit ? IDC_PAPERTAPEBYTE2 : IDC_PAPERTAPEBYTE, gtempstring);
#endif
#ifdef AMIGA
        switch (papertapemode[whichunit])
        {
        case  TAPEMODE_PLAY:       SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU20 : GID_PT_BU3], SubWindowPtr[SUBWINDOW_PAPERTAPE], NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[EMUBRUSH_CYAN  ], TAG_DONE);
        acase TAPEMODE_RECORD: if (papertapeprotect[whichunit])
                               {   SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU20 : GID_PT_BU3], SubWindowPtr[SUBWINDOW_PAPERTAPE], NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[EMUBRUSH_YELLOW], TAG_DONE);
                               } else
                               {   SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU20 : GID_PT_BU3], SubWindowPtr[SUBWINDOW_PAPERTAPE], NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[EMUBRUSH_RED   ], TAG_DONE);
                               }
        adefault:                  SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU20 : GID_PT_BU3], SubWindowPtr[SUBWINDOW_PAPERTAPE], NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[EMUBRUSH_GREEN ], TAG_DONE);
        }
#endif
    }

    if
    (   papertapewhere[whichunit] != oldpapertapewhere[whichunit]
     || papertapemode[ whichunit] != oldpapertapemode[ whichunit]
     || papertapebyte[ whichunit] != oldpapertapebyte[ whichunit]
     || force
    )
    {   update_roll(whichunit);
    }

    oldpapertapewhere[ whichunit] = papertapewhere[ whichunit];
    oldpapertapelength[whichunit] = papertapelength[whichunit];
    oldpapertapemode[  whichunit] = papertapemode[  whichunit];
    oldpapertapebyte[  whichunit] = papertapebyte[  whichunit];
}

EXPORT void ghost_dips(BOOL state)
{   if (!SubWindowPtr[SUBWINDOW_DIPS])
    {   return; // important!
    }

    switch (machine)
    {
    case INSTRUCTOR:
        ra_enable( SUBWINDOW_DIPS, IDC_INTERRUPTS_DIRECT   , state);
        ra_enable( SUBWINDOW_DIPS, IDC_INTERRUPTS_INDIRECT , state);
        ra_enable2(SUBWINDOW_DIPS, IDC_INTERRUPTS          , state);

        ra_enable( SUBWINDOW_DIPS, IDC_INTSELECTOR_ACLINE  , state);
        ra_enable( SUBWINDOW_DIPS, IDC_INTSELECTOR_KYBD    , state);
        ra_enable2(SUBWINDOW_DIPS, IDC_INTSELECTOR         , state);

        ra_enable( SUBWINDOW_DIPS, IDC_PARALLEL_MEMMAPPED  , state);
        ra_enable( SUBWINDOW_DIPS, IDC_PARALLEL_EXTENDED   , state);
        ra_enable( SUBWINDOW_DIPS, IDC_PARALLEL_NONEXTENDED, state);
        ra_enable2(SUBWINDOW_DIPS, IDC_PARALLEL            , state);

        bu_enable( SUBWINDOW_DIPS, IDC_PARALLEL_BIT7       , state);
        bu_enable( SUBWINDOW_DIPS, IDC_PARALLEL_BIT6       , state);
        bu_enable( SUBWINDOW_DIPS, IDC_PARALLEL_BIT5       , state);
        bu_enable( SUBWINDOW_DIPS, IDC_PARALLEL_BIT4       , state);
        bu_enable( SUBWINDOW_DIPS, IDC_PARALLEL_BIT3       , state);
        bu_enable( SUBWINDOW_DIPS, IDC_PARALLEL_BIT2       , state);
        bu_enable( SUBWINDOW_DIPS, IDC_PARALLEL_BIT1       , state);
        bu_enable( SUBWINDOW_DIPS, IDC_PARALLEL_BIT0       , state);
    acase ZACCARIA:
        ra_enable( SUBWINDOW_DIPS, IDC_LIVES_2             , state);
        ra_enable( SUBWINDOW_DIPS, IDC_LIVES_3             , state);
        ra_enable( SUBWINDOW_DIPS, IDC_LIVES_4             , state);
        ra_enable( SUBWINDOW_DIPS, IDC_LIVES_5             , state);
        ra_enable( SUBWINDOW_DIPS, IDC_LIVES_6             , state);
        ra_enable( SUBWINDOW_DIPS, IDC_LIVES_INFINITE      , state);
        ra_enable2(SUBWINDOW_DIPS, IDC_LIVES               , state);

        ra_enable( SUBWINDOW_DIPS, IDC_COINA_HALFC         , state);
        ra_enable( SUBWINDOW_DIPS, IDC_COINA_1C            , state);
        ra_enable( SUBWINDOW_DIPS, IDC_COINA_2C            , state);
        ra_enable( SUBWINDOW_DIPS, IDC_COINA_3C            , state);
        ra_enable( SUBWINDOW_DIPS, IDC_COINA_5C            , state);
        ra_enable2(SUBWINDOW_DIPS, IDC_COINA               , state);

        ra_enable( SUBWINDOW_DIPS, IDC_COINB_HALFC         , state);
        ra_enable( SUBWINDOW_DIPS, IDC_COINB_1C            , state);
        ra_enable( SUBWINDOW_DIPS, IDC_COINB_2C            , state);
        ra_enable( SUBWINDOW_DIPS, IDC_COINB_3C            , state);
        ra_enable( SUBWINDOW_DIPS, IDC_COINB_5C            , state);
        ra_enable( SUBWINDOW_DIPS, IDC_COINB_7C            , state);
        ra_enable2(SUBWINDOW_DIPS, IDC_COINB               , state);

        cb_enable( SUBWINDOW_DIPS, IDC_FREEZE              , state);

        cb_enable( SUBWINDOW_DIPS, IDC_COLLISIONS          , state);

        if (state)
        {   zaccaria_ghostdips(); // re-ghosts as appropriate
        }
    acase MALZAK:
        ra_enable( SUBWINDOW_DIPS, IDC_SWITCH_1            , state);
        ra_enable( SUBWINDOW_DIPS, IDC_SWITCH_2            , state);
        ra_enable( SUBWINDOW_DIPS, IDC_SWITCH_3            , state);
        ra_enable( SUBWINDOW_DIPS, IDC_SWITCH_4            , state);
        ra_enable2(SUBWINDOW_DIPS, IDC_SWITCH              , state);
    acase PONG:
        cb_enable( SUBWINDOW_DIPS, IDC_LOCKHORIZ           , state);

        ra_enable( SUBWINDOW_DIPS, IDC_BATS_SHORT          , state);
        ra_enable( SUBWINDOW_DIPS, IDC_BATS_TALL           , state);
        ra_enable2(SUBWINDOW_DIPS, IDC_BATS                , state);

        ra_enable( SUBWINDOW_DIPS, IDC_SPEED_SLOW          , state);
        ra_enable( SUBWINDOW_DIPS, IDC_SPEED_FAST          , state);
        ra_enable2(SUBWINDOW_DIPS, IDC_SPEED               , state);

        sl_enable( SUBWINDOW_DIPS, IDC_ROBOTLEFT           , state);

        sl_enable( SUBWINDOW_DIPS, IDC_ROBOTRIGHT          , state);

        if (memmap == MEMMAP_8550)
        {   ra_enable( SUBWINDOW_DIPS, IDC_PONGMACHINE_1976, state);
            ra_enable( SUBWINDOW_DIPS, IDC_PONGMACHINE_1977, state);
            ra_enable( SUBWINDOW_DIPS, IDC_PONGMACHINE_8550, state);
            ra_enable2(SUBWINDOW_DIPS, IDC_PONGMACHINE     , state);

            ra_enable( SUBWINDOW_DIPS, IDC_ANGLES_2        , state);
            ra_enable( SUBWINDOW_DIPS, IDC_ANGLES_4        , state);
            ra_enable( SUBWINDOW_DIPS, IDC_ANGLES_RANDOM   , state);
            ra_enable2(SUBWINDOW_DIPS, IDC_ANGLES          , state);

            ra_enable( SUBWINDOW_DIPS, IDC_PLAYERS_2       , state);
            ra_enable( SUBWINDOW_DIPS, IDC_PLAYERS_3LT     , state);
            ra_enable( SUBWINDOW_DIPS, IDC_PLAYERS_3RT     , state);
            ra_enable( SUBWINDOW_DIPS, IDC_PLAYERS_4       , state);
            ra_enable2(SUBWINDOW_DIPS, IDC_PLAYERS         , state);

            ra_enable( SUBWINDOW_DIPS, IDC_SERVING_AUTO    , state);
            ra_enable( SUBWINDOW_DIPS, IDC_SERVING_MANUAL  , state);
            ra_enable2(SUBWINDOW_DIPS, IDC_SERVING         , state);

            cb_enable( SUBWINDOW_DIPS, IDC_PLAYERID        , state);

            ra_enable( SUBWINDOW_DIPS, IDC_8550_TENNIS     , state);
            ra_enable( SUBWINDOW_DIPS, IDC_8550_SOCCER     , state);
            ra_enable( SUBWINDOW_DIPS, IDC_8550_HANDICAP   , state);
            ra_enable( SUBWINDOW_DIPS, IDC_8550_SQUASH     , state);
            ra_enable( SUBWINDOW_DIPS, IDC_8550_PRACTICE   , state);
            ra_enable( SUBWINDOW_DIPS, IDC_8550_RIFLE1     , state);
            ra_enable( SUBWINDOW_DIPS, IDC_8550_RIFLE2     , state);
            ra_enable2(SUBWINDOW_DIPS, IDC_8550_PONGVARIANT, state);
        } else
        {   // assert(memmap == MEMMAP_8600);
            ra_enable( SUBWINDOW_DIPS, IDC_8600_TENNIS     , state);
            ra_enable( SUBWINDOW_DIPS, IDC_8600_HOCKEY     , state);
            ra_enable( SUBWINDOW_DIPS, IDC_8600_SOCCER     , state);
            ra_enable( SUBWINDOW_DIPS, IDC_8600_SQUASH     , state);
            ra_enable( SUBWINDOW_DIPS, IDC_8600_PRACTICE   , state);
            ra_enable( SUBWINDOW_DIPS, IDC_8600_GRIDBALL   , state);
            ra_enable( SUBWINDOW_DIPS, IDC_8600_BASKETBALL , state);
            ra_enable( SUBWINDOW_DIPS, IDC_8600_BBPRACTICE , state);
            ra_enable( SUBWINDOW_DIPS, IDC_8600_TARGET1    , state);
            ra_enable( SUBWINDOW_DIPS, IDC_8600_TARGET2    , state);
            ra_enable2(SUBWINDOW_DIPS, IDC_8600_PONGVARIANT, state);
}   }   }
