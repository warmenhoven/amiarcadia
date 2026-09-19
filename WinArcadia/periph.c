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

EXPORT     FLAG           disk_oor, // means out of range
                          oldspinning;
EXPORT     UBYTE          papertapebyte[2];
EXPORT     ULONG          papertapelength[2] = { 0, 0 },
                          papertapewhere[ 2] = { 0, 0 };
EXPORT     int            diskblocksize;
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
IMPORT     ULONG          curdrive,
                          papertapeprotect[2],
                          samplewhere,
                          tapelength,
                          tape_hz,
                          tapewriteprotect,
                          viewdiskas;
IMPORT     int            drive_mode,
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
IMPORT const UWORD            fileoffset[78];
IMPORT struct DriveStruct     drive[DRIVES_MAX];
IMPORT struct MachineStruct   machines[MACHINES];
IMPORT struct SubWindowStruct subwin[SUBWINDOWS];
#ifdef WIN32
    IMPORT int                CatalogPtr; // APTR doesn't work
    IMPORT HICON              diskicon;
    IMPORT HWND               hStatusBar,
                              MainWindowPtr;
#endif
#ifdef AMIGA
    IMPORT        int         driveglyph_x, driveglyph_y;
    IMPORT        LONG        emupens[EMUBRUSHES];
    IMPORT        ULONG       emulongpens[EMUBRUSHES],
                              tiptag1;
    IMPORT struct Catalog*    CatalogPtr;
    IMPORT struct Gadget*     gadgets[GIDS];
    IMPORT struct Image*      images[IMAGES];
    IMPORT struct Window*     MainWindowPtr;
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE     UBYTE              blockcontents[256];

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

/* MODULE FUNCTIONS-------------------------------------------------------

(None)

CODE------------------------------------------------------------------- */

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

    if (!subwin[SUBWINDOW_TAPEDECK].hwnd)
    {   return;
    }

    if (samplewhere != oldsamplewhere || tapelength != oldtapelength || force)
    {
#ifdef AMIGA
        divisions = (tapelength / (2 * KILOBYTE)) + 1;
        // "A SLIDER_MAX level beyond 65535 is not supported." - OS4.1 SDK.
        //  But the limit seems to be much less under OS3.9. 2048 is a safe value.
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
        SetWindowText(subwin[SUBWINDOW_TAPEDECK].hwnd, tapetitlestring);

        DISCARD EnableWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_POSITIONSLIDER), (tapemode == TAPEMODE_STOP) ? TRUE : FALSE);

        DISCARD EnableWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_REWIND    ), (samplewhere > 0          && tapemode == TAPEMODE_STOP) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_STOPTAPE  ), (                            tapemode >  TAPEMODE_STOP) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_RECORD    ), (                            tapemode == TAPEMODE_STOP) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_PLAY      ), (samplewhere < tapelength && tapemode == TAPEMODE_STOP) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_FFWD      ), (samplewhere < tapelength && tapemode == TAPEMODE_STOP) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_EJECTTAPE ), (                            tapemode != TAPEMODE_NONE) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_CREATE8SVX), (                            tapemode == TAPEMODE_NONE) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_CREATEAIFF), (                            tapemode == TAPEMODE_NONE) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_CREATEWAV ), (                            tapemode == TAPEMODE_NONE) ? TRUE : FALSE);
        DISCARD EnableWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_INSERTTAPE), (                            tapemode == TAPEMODE_NONE) ? TRUE : FALSE);

        DISCARD RedrawWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_REWIND    ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        DISCARD RedrawWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_STOPTAPE  ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        DISCARD RedrawWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_RECORD    ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        DISCARD RedrawWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_PLAY      ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        DISCARD RedrawWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_FFWD      ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        DISCARD RedrawWindow(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_EJECTTAPE ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
#endif
#ifdef AMIGA
        SetWindowTitles(subwin[SUBWINDOW_TAPEDECK].hwnd, (const char*) tapetitlestring, (const char*) tapetitlestring);

        SetGadgetAttrs(gadgets[GID_TA_SL2 ], subwin[SUBWINDOW_TAPEDECK].hwnd, NULL, GA_Disabled, (                            tapemode == TAPEMODE_STOP) ? FALSE : TRUE,                                                            TAG_DONE); // this autorefreshes

        SetGadgetAttrs(gadgets[GID_TA_BU1 ], subwin[SUBWINDOW_TAPEDECK].hwnd, NULL, GA_Disabled, (                            tapemode == TAPEMODE_STOP) ? FALSE : TRUE, GA_Selected, (tapemode == TAPEMODE_RECORD) ? TRUE : FALSE, TAG_DONE); // record
        SetGadgetAttrs(gadgets[GID_TA_BU2 ], subwin[SUBWINDOW_TAPEDECK].hwnd, NULL, GA_Disabled, (samplewhere > 0          && tapemode == TAPEMODE_STOP) ? FALSE : TRUE,                                                            TAG_DONE); // rewind
        SetGadgetAttrs(gadgets[GID_TA_BU3 ], subwin[SUBWINDOW_TAPEDECK].hwnd, NULL, GA_Disabled, (samplewhere < tapelength && tapemode == TAPEMODE_STOP) ? FALSE : TRUE, GA_Selected, (tapemode == TAPEMODE_PLAY  ) ? TRUE : FALSE, TAG_DONE); // play
        SetGadgetAttrs(gadgets[GID_TA_BU4 ], subwin[SUBWINDOW_TAPEDECK].hwnd, NULL, GA_Disabled, (samplewhere < tapelength && tapemode == TAPEMODE_STOP) ? FALSE : TRUE,                                                            TAG_DONE); // ffwd
        SetGadgetAttrs(gadgets[GID_TA_BU5 ], subwin[SUBWINDOW_TAPEDECK].hwnd, NULL, GA_Disabled, (                            tapemode >  TAPEMODE_STOP) ? FALSE : TRUE, GA_Selected, (tapemode == TAPEMODE_STOP  ) ? TRUE : FALSE, TAG_DONE); // stop
        SetGadgetAttrs(gadgets[GID_TA_BU6 ], subwin[SUBWINDOW_TAPEDECK].hwnd, NULL, GA_Disabled, (                            tapemode != TAPEMODE_NONE) ? FALSE : TRUE, GA_Selected, (tapemode == TAPEMODE_NONE  ) ? TRUE : FALSE, TAG_DONE); // eject
        SetGadgetAttrs(gadgets[GID_TA_BU7 ], subwin[SUBWINDOW_TAPEDECK].hwnd, NULL, GA_Disabled, (                            tapemode == TAPEMODE_NONE) ? FALSE : TRUE,                                                            TAG_DONE);
        SetGadgetAttrs(gadgets[GID_TA_BU8 ], subwin[SUBWINDOW_TAPEDECK].hwnd, NULL, GA_Disabled, (                            tapemode == TAPEMODE_NONE) ? FALSE : TRUE,                                                            TAG_DONE);
        SetGadgetAttrs(gadgets[GID_TA_BU11], subwin[SUBWINDOW_TAPEDECK].hwnd, NULL, GA_Disabled, (                            tapemode == TAPEMODE_NONE) ? FALSE : TRUE,                                                            TAG_DONE);
        SetGadgetAttrs(gadgets[GID_TA_BU12], subwin[SUBWINDOW_TAPEDECK].hwnd, NULL, GA_Disabled, (                            tapemode == TAPEMODE_NONE) ? FALSE : TRUE,                                                            TAG_DONE);
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
        SetDlgItemText(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_TAPEBYTE, gtempstring);
#endif
#ifdef AMIGA
        switch (tapemode)
        {
        case  TAPEMODE_PLAY:       SetGadgetAttrs(gadgets[GID_TA_BU17], subwin[SUBWINDOW_TAPEDECK].hwnd, NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[EMUBRUSH_CYAN  ], TAG_DONE);
        acase TAPEMODE_RECORD: if (tapewriteprotect)
                               {   SetGadgetAttrs(gadgets[GID_TA_BU17], subwin[SUBWINDOW_TAPEDECK].hwnd, NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[EMUBRUSH_YELLOW], TAG_DONE);
                               } else
                               {   SetGadgetAttrs(gadgets[GID_TA_BU17], subwin[SUBWINDOW_TAPEDECK].hwnd, NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[EMUBRUSH_RED   ], TAG_DONE);
                               }
        adefault:                  SetGadgetAttrs(gadgets[GID_TA_BU17], subwin[SUBWINDOW_TAPEDECK].hwnd, NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[EMUBRUSH_GREEN ], TAG_DONE);
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
            SetDlgItemText(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_MDCRSTATUS, mdcr_string);
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
            SetGadgetAttrs(gadgets[GID_TA_BU16], subwin[SUBWINDOW_TAPEDECK].hwnd, NULL, GA_Text, mdcr_string, BUTTON_BackgroundPen, emupens[whichemupen], TAG_DONE); // this refreshes automatically
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

EXPORT void update_floppydrive(FLAG force, int whichdrive)
{   TRANSIENT FLAG  refreshtips = FALSE;
    FAST      int   cluster,
                    i, j,
                    length,
                    startblock, endblock,
                    viewingbyte,
                    viewingcluster,
                    where,
                    whichpen;
    FAST      UBYTE t,
                    viewingtrack,
                    viewingsector;
    PERSIST   TEXT  subwintitle[256 + 1];
    PERSIST   UBYTE oldtrack[DRIVES_MAX],
                    oldsector;
    PERSIST   int   oldblockoffset,
                    oldcluster,
                    olddrivemode,
                    oldviewingdrive,
                    oldviewstart;
#ifdef AMIGA
    FAST      int   dimicon,
                    glowicon;
#endif

    if (viewingdrive != oldviewingdrive)
    {   force = TRUE;
    }

    // Status bar---------------------------------------------------------

    if (MainWindowPtr && showstatusbars[wsm] && (force || drive_mode != olddrivemode || drive[whichdrive].track != oldtrack[whichdrive]))
    {   if (whichdrive >= machines[machine].drives)
        {
#ifdef AMIGA
            SetGadgetAttrs(gadgets[GID_MA_BU3 + whichdrive], MainWindowPtr, NULL, GA_Text, "--", BUTTON_BackgroundPen, ~0, GA_Disabled, TRUE, TAG_END); // this refreshes automatically
#endif
#ifdef WIN32
            SendMessage(hStatusBar, SB_SETTEXT, 4 + whichdrive, (LPARAM) "--");
#ifdef COLOURSTATUSBAR
            SendMessage(hStatusBar, SB_SETBKCOLOR, 0, (LPARAM) CLR_DEFAULT);
#endif
#endif
        } else
        {   sprintf(gtempstring, "%02d", drive[whichdrive].track);
            whichpen = getdiskmodecolour();
#ifdef AMIGA
            SetGadgetAttrs(gadgets[GID_MA_BU3 + whichdrive], MainWindowPtr, NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[whichpen], GA_Disabled, FALSE, TAG_END); // this refreshes automatically
#endif
#ifdef WIN32
            DISCARD SendMessage(hStatusBar, SB_SETTEXT, 4 + whichdrive, (LPARAM) gtempstring);
#ifdef COLOURSTATUSBAR
            DISCARD SendMessage(hStatusBar, SB_SETBKCOLOR, 0, (LPARAM) whichpen);
#endif
#endif
    }   }

    if
    (   !subwin[SUBWINDOW_FLOPPYDRIVE].hwnd
     || whichdrive >= machines[machine].drives
     || whichdrive != viewingdrive
    )
    {   oldtrack[whichdrive] = drive[whichdrive].track;
        olddrivemode         = drive_mode;
        return;
    }

    if (force)
    {   // Subwindow title------------------------------------------------
        if (machines[machine].drives >= 2)
        {   strcpy(subwintitle, LLL(MSG_HAIL_FLOPPYDRIVES, "Floppy Disk Drives"));
        } else
        {   strcpy(subwintitle, LLL(MSG_HAIL_FLOPPYDRIVE,  "Floppy Disk Drive" ));
        }
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
        }
#ifdef WIN32
        SetWindowText(subwin[SUBWINDOW_FLOPPYDRIVE].hwnd, subwintitle);
#endif
#ifdef AMIGA
        SetWindowTitles(subwin[SUBWINDOW_FLOPPYDRIVE].hwnd, (const char*) subwintitle, (const char*) subwintitle);
#endif

        // Buttons--------------------------------------------------------
        bu_enable(SUBWINDOW_FLOPPYDRIVE, IDC_UPDATEDISK, drive[whichdrive].inserted ? TRUE : FALSE);
        bu_enable(SUBWINDOW_FLOPPYDRIVE, IDC_EJECTDISK,  drive[whichdrive].inserted ? TRUE : FALSE);
    }

    // Glyph--------------------------------------------------------------

    if (machine != TWIN && oldspinning != drive[whichdrive].spinning)
    {
#ifdef WIN32
        SendMessage(GetDlgItem(subwin[SUBWINDOW_FLOPPYDRIVE].hwnd, IDC_DISKGLYPH), STM_SETIMAGE, IMAGE_ICON, (LPARAM) spinicon[drive[whichdrive].spinning ? 1 : 0]);
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
            DrawImage(subwin[SUBWINDOW_FLOPPYDRIVE].hwnd->RPort, images[glowicon], 0, 0);
        } else
        {   images[dimicon ]->LeftEdge = driveglyph_x;
            images[dimicon ]->TopEdge  = driveglyph_y;
            DrawImage(subwin[SUBWINDOW_FLOPPYDRIVE].hwnd->RPort, images[dimicon ], 0, 0);
        }
#endif
        oldspinning = drive[whichdrive].spinning;
    }

    // Platter------------------------------------------------------------

    if (force)
    {   draw_platter(TRUE, oldtrack[viewingdrive], oldsector);
    } elif (drive[viewingdrive].track != oldtrack[viewingdrive] || drive[viewingdrive].sector != oldsector)
    {   draw_platter(FALSE, oldtrack[viewingdrive], oldsector);
    }
    // Grid labels & slider-----------------------------------------------

    switch (machine)
    {
    case  TWIN:
        viewingtrack  =   drive[viewingdrive].viewstart /   TWIN_TRACKSIZE;
        viewingsector =  (drive[viewingdrive].viewstart %   TWIN_TRACKSIZE) /   TWIN_BLOCKSIZE    ;
    acase BINBUG:
        viewingtrack  =   drive[viewingdrive].viewstart / BINBUG_TRACKSIZE;
        viewingsector = ((drive[viewingdrive].viewstart % BINBUG_TRACKSIZE) / BINBUG_BLOCKSIZE) + 1;
    acase CD2650:
        viewingtrack  =   drive[viewingdrive].viewstart / CD2650_TRACKSIZE;
        viewingsector = ((drive[viewingdrive].viewstart % CD2650_TRACKSIZE) / CD2650_BLOCKSIZE) + 1;
    }
    get_disk_byte(viewingdrive, viewingtrack, viewingsector, 0, &viewingbyte, &viewingcluster);

    if (force || drive[whichdrive].viewstart != oldviewstart)
    {   for (i = 0; i < diskblocksize / 16; i++)
        {   if (viewingbyte == -1)
            {   strcpy((char*) gtempstring, "-:");
            } else
            {   sprintf((char*) gtempstring, "$%04Xx:", (drive[whichdrive].viewstart / 16) + i);
            }
            st_set(SUBWINDOW_FLOPPYDRIVE, IDL_FREGION0 + i);
        }
        sl_set(SUBWINDOW_FLOPPYDRIVE, IDC_DISKREGION1, (viewingbyte == -1) ? 0 : (drive[whichdrive].viewstart / diskblocksize));
    }

    // Grid buttons-------------------------------------------------------

    for (i = 0; i < diskblocksize; i++)
    {   t = drive[whichdrive].contents[drive[whichdrive].viewstart + i];
        if
        (   force
         || drive[whichdrive].viewstart != oldviewstart
         || t                           != blockcontents[i]
         || i                           == drive[whichdrive].blockoffset
         || i                           == oldblockoffset
        )
        {   if (!drive[viewingdrive].inserted || viewingbyte == -1)
            {   gtempstring[0] = '-';
                gtempstring[1] = EOS;
                setdrivegad(IDC_DISK_0 + i, EMUPEN_GREY);
            } else
            {   blockcontents[i] = t;
                if (viewdiskas == 0 || (machine == BINBUG && i < 2))
                {   hex1((char*) gtempstring, t);
                    gtempstring[2] = EOS;
                } else
                {   gtempstring[0] = guestchar(t);
                    gtempstring[1] = EOS;
                }
                whichpen = getdiskbytecolour(drive[viewingdrive].viewstart + i);
                setdrivegad(IDC_DISK_0 + i, whichpen);
            }
            refreshtips = TRUE;
    }   }

    // Head Position------------------------------------------------------

    whichpen = getdiskmodecolour();
    if (force || drive_mode != olddrivemode || drive[viewingdrive].track  != oldtrack[viewingdrive])
    {   sprintf((char*) gtempstring, "%d", (int) drive[viewingdrive].track);
        setdrivegad(IDC_TRACK, whichpen);
    }
    if (force || drive_mode != olddrivemode || drive[viewingdrive].sector != oldsector)
    {   sprintf((char*) gtempstring, "%d", (int) drive[viewingdrive].sector);
        setdrivegad(IDC_SECTOR, whichpen);
    }
    if (force || drive_mode != olddrivemode || drive[viewingdrive].track  != oldtrack[viewingdrive] || drive[viewingdrive].sector != oldsector || drive[viewingdrive].blockoffset != oldblockoffset)
    {   if (viewingbyte == -1)
        {   gtempstring[0] = '-';
            gtempstring[1] = EOS;
        } else
        {   switch (machine)
            {
            case  BINBUG: sprintf((char*) gtempstring, "%X", (int) ((drive[viewingdrive].track * BINBUG_TRACKSIZE) + ((drive[viewingdrive].sector - 1) * BINBUG_BLOCKSIZE) + drive[viewingdrive].blockoffset));
            acase TWIN:   sprintf((char*) gtempstring, "%X", (int) ((drive[viewingdrive].track *   TWIN_TRACKSIZE) + ( drive[viewingdrive].sector      *   TWIN_BLOCKSIZE) + drive[viewingdrive].blockoffset));
            acase CD2650: sprintf((char*) gtempstring, "%X", (int) ((drive[viewingdrive].track * CD2650_TRACKSIZE) + ((drive[viewingdrive].sector - 1) * CD2650_BLOCKSIZE) + drive[viewingdrive].blockoffset));
        }   }
        setdrivegad(IDC_BYTE, whichpen);
    }
    if (machine == TWIN)
    {   cluster = (drive[viewingdrive].track == 0) ? (-1) : (((drive[viewingdrive].track - 1) * 4) + (drive[viewingdrive].sector / 8));
        if (force || drive_mode != olddrivemode || cluster != oldcluster)
        {   if (drive[viewingdrive].track == 0)
            {   sprintf(gtempstring, "-");
            } else
            {   sprintf(gtempstring, "%d", cluster);
            }
            setdrivegad(IDC_CLUSTER, whichpen);
    }   }

    // Filename-----------------------------------------------------------

    if (force || drive[viewingdrive].viewstart != oldviewstart)
    {   if (!drive[viewingdrive].inserted || viewingcluster == -1)
        {   strcpy(gtempstring, "-");
            whichpen = EMUPEN_GREY;
        } else
        {   switch (machine)
            {
            case BINBUG:
                switch (drive[viewingdrive].bam[viewingcluster])
                {
                case  BAM_LOST: strcpy(gtempstring, "(Lost)");
                acase BAM_DIR:  strcpy(gtempstring, "(Directory)");
                acase BAM_FILE:
                    strcpy(            gtempstring, "(File)");
                    where = 0;
                    for (j = 0; j < 10; j++)
                    {   where += 16; // skip identity section
                        for (i = 0; i < 10; i++)
                        {   if (drive[viewingdrive].filename[(j * 10) + i][0] != EOS)
                            {   startblock = (drive[viewingdrive].contents[where + 13] * 10) + drive[viewingdrive].contents[where + 14] - 1;
                                endblock   = (drive[viewingdrive].contents[where + 15] * 10) + drive[viewingdrive].contents[where + 16] - 1;
                                if (viewingcluster >= startblock && viewingcluster <= endblock)
                                {   sprintf(gtempstring, "%s.%s", drive[viewingdrive].filename[(j * 10) + i], drive[viewingdrive].fileext[(j * 10) + i]);
                                    break;
                            }   }
                            where += 24;
                    }   }
                acase BAM_FREE: strcpy(gtempstring, "(Free)");
                adefault:       strcpy(gtempstring, "(Unknown)"); // should never happen
                }
            acase TWIN:
                if (viewingcluster == -2)
                {   sprintf(           gtempstring, "(Track zero)");
                } else
                {   switch (drive[viewingdrive].bam[viewingcluster])
                    {
                    case  BAM_LOST: strcpy(gtempstring, "(Lost)");
                    acase BAM_DIR:  strcpy(gtempstring, "(Directory)");
                    acase BAM_FILE:
                        strcpy(            gtempstring, "(File)");
                        for (i = 0; i < 78; i++)
                        {   if
                            (   drive[viewingdrive].filename[i][0] != EOS
                             && drive[viewingdrive].contents[fileoffset[i] + (viewingcluster / 8)] & (0x80 >> (viewingcluster % 8))
                            )
                            {   strcpy(gtempstring, drive[viewingdrive].filename[i]);
                                twin_get_commands(viewingdrive, i, FALSE); // extends gtempstring
                                break;
                        }   }
                    acase BAM_FREE: strcpy(gtempstring, "(Free)");
                    adefault:       strcpy(gtempstring, "(Unknown)"); // should never happen
                }   }
            acase CD2650:
                switch (drive[viewingdrive].bam[viewingcluster])
                {
                // there is no BAM_LOST currently implemented for CD2650
                case  BAM_DIR:  strcpy(gtempstring, "(Directory)");
                acase BAM_FILE: strcpy(gtempstring, "(File)");
                    for (i = 0; i < 64; i++)
                    {   if (drive[viewingdrive].filename[i][0] != EOS)
                        {   startblock = (drive[viewingdrive].contents[(i * 64) + 28] * CD2650_SECTORS) // track
                                       +  drive[viewingdrive].contents[(i * 64) + 29] - 1;              // sector
                            endblock   = (drive[viewingdrive].contents[(i * 64) + 30] * CD2650_SECTORS) // track
                                       +  drive[viewingdrive].contents[(i * 64) + 31] - 1;              // sector
                            if (viewingcluster >= startblock && viewingcluster <= endblock)
                            {   sprintf(gtempstring, "%s.%s", drive[viewingdrive].filename[i], drive[viewingdrive].fileext[i]);
                                break;
                    }   }   }
                acase BAM_FREE: strcpy(gtempstring, "(Free)");
                adefault:       strcpy(gtempstring, "(Unknown)"); // should never happen
            }   }
            whichpen = getdiskclustercolour(viewingbyte);
        }
        setdrivegad(IDC_FILENAME, whichpen);
     // zprintf(TEXTPEN_VERBOSE, "%s\n", gtempstring);
    }

    if (refreshtips)
    {
#ifdef WIN32
        update_floppytips();
#endif
#ifdef AMIGA
        if (tiptag1 != TAG_IGNORE)
        {   make_floppytips();
        }
#endif
    }

    oldcluster           = cluster;
    oldtrack[whichdrive] = drive[whichdrive].track;
    oldsector            = drive[whichdrive].sector;
    oldblockoffset       = drive[whichdrive].blockoffset;
    oldviewstart         = drive[whichdrive].viewstart;
    olddrivemode         = drive_mode;
    oldviewingdrive      = viewingdrive;
}
/* The black radial lines (sector dividers) go from x/ystart to x/yend.

You give it a rectangle; the arc is part of an ellipse filling this rectangle.
The startx/y and endx/y points get imaginary lines drawn from them to the centre of the ellipse.
The part of the ellipse between the imaginary lines is the arc.

The rectangle (ellipse) we calculate algorithmically.
x1/y1 is just the point on the "wall" of the disk's "box" where the sector line intersects.
x2/y2 is just the x1/y1 of the next sector. */

EXPORT void update_papertape(int whichunit, FLAG force, FLAG interim)
{   PERSIST UBYTE oldpapertapebyte[  2];
    PERSIST ULONG oldpapertapewhere[ 2],
                  oldpapertapelength[2];
    PERSIST int   oldpapertapemode[  2];
#ifdef AMIGA
    FAST    ULONG divisions;
#endif

    if (!subwin[SUBWINDOW_PAPERTAPE].hwnd)
    {   return;
    }

#ifdef AMIGA
    if (!interim && (papertapewhere[whichunit] != oldpapertapewhere[whichunit] || papertapelength[whichunit] != oldpapertapelength[whichunit] || force))
    {   divisions = (papertapelength[whichunit] / (2 * KILOBYTE)) + 1;
        /* "A SLIDER_MAX level beyond 65535 is not supported." - OS4.1 SDK.
            But limit seems to be much less under OS3.9. 2048 is a safe value. */

        DISCARD SetGadgetAttrs
        (   gadgets[GID_PT_SL1 + whichunit],
            subwin[SUBWINDOW_PAPERTAPE].hwnd, NULL,
            SLIDER_Level,                     papertapewhere[whichunit]  / divisions,
            SLIDER_Max,                       papertapelength[whichunit] / divisions,
        TAG_DONE); // this autorefreshes
    }
#endif

    if (papertapelength[whichunit] != oldpapertapelength[whichunit] || force)
    {
#ifdef WIN32
        DISCARD SendMessage
        (   GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd, whichunit ? IDC_PTPOSITIONSLIDER2 : IDC_PTPOSITIONSLIDER),
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
        (   GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd, whichunit ? IDC_PTPOSITIONSLIDER2 : IDC_PTPOSITIONSLIDER),
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
        SetWindowText(subwin[SUBWINDOW_PAPERTAPE].hwnd, papertapetitlestring);
        EnableWindow(GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd, whichunit ? IDC_PTPOSITIONSLIDER2 : IDC_PTPOSITIONSLIDER), (papertapemode[whichunit] == TAPEMODE_STOP) ? TRUE : FALSE);
        if (whichunit == 0)
        {   EnableWindow(GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd,                                 IDC_PT_RECORD       ), (papertapewhere[whichunit] < PAPERTAPEMAX               && papertapemode[whichunit] == TAPEMODE_STOP) ? TRUE : FALSE);

            RedrawWindow(GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd,                                 IDC_PT_RECORD       ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

            EnableWindow(GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd,                                 IDC_CREATEPAPERTAPE ), (                                                          papertapemode[whichunit] == TAPEMODE_NONE) ? TRUE : FALSE);
        }

        EnableWindow(GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd, whichunit ? IDC_PT_REWIND2        : IDC_PT_REWIND       ), (papertapewhere[whichunit] > 0                          && papertapemode[whichunit] == TAPEMODE_STOP) ? TRUE : FALSE);
        EnableWindow(GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd, whichunit ? IDC_PT_PLAY2          : IDC_PT_PLAY         ), (papertapewhere[whichunit] < papertapelength[whichunit] && papertapemode[whichunit] == TAPEMODE_STOP) ? TRUE : FALSE);
        EnableWindow(GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd, whichunit ? IDC_PT_FFWD2          : IDC_PT_FFWD         ), (papertapewhere[whichunit] < papertapelength[whichunit] && papertapemode[whichunit] == TAPEMODE_STOP) ? TRUE : FALSE);
        EnableWindow(GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd, whichunit ? IDC_PT_STOPTAPE2      : IDC_PT_STOPTAPE     ), (                                                          papertapemode[whichunit] >  TAPEMODE_STOP) ? TRUE : FALSE);
        EnableWindow(GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd, whichunit ? IDC_PT_EJECTTAPE2     : IDC_PT_EJECTTAPE    ), (                                                          papertapemode[whichunit] != TAPEMODE_NONE) ? TRUE : FALSE);

        RedrawWindow(GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd, whichunit ? IDC_PT_REWIND2        : IDC_PT_REWIND       ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        RedrawWindow(GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd, whichunit ? IDC_PT_STOPTAPE2      : IDC_PT_STOPTAPE     ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        RedrawWindow(GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd, whichunit ? IDC_PT_PLAY2          : IDC_PT_PLAY         ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        RedrawWindow(GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd, whichunit ? IDC_PT_FFWD2          : IDC_PT_FFWD         ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        RedrawWindow(GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd, whichunit ? IDC_PT_EJECTTAPE2     : IDC_PT_EJECTTAPE    ), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

        EnableWindow(GetDlgItem(subwin[SUBWINDOW_PAPERTAPE].hwnd, whichunit ? IDC_INSERTPAPERTAPE2  : IDC_INSERTPAPERTAPE ), (                                                          papertapemode[whichunit] == TAPEMODE_NONE) ? TRUE : FALSE);
#endif
#ifdef AMIGA
        SetWindowTitles(subwin[SUBWINDOW_PAPERTAPE].hwnd, (const char*) papertapetitlestring, (const char*) papertapetitlestring);
        SetGadgetAttrs(gadgets[GID_PT_SL1 + whichunit], subwin[SUBWINDOW_PAPERTAPE].hwnd, NULL, GA_Disabled, (papertapemode[whichunit] == TAPEMODE_STOP) ? FALSE : TRUE, TAG_DONE);
        RefreshGList((struct Gadget*) gadgets[GID_PT_SL1 + whichunit], subwin[SUBWINDOW_PAPERTAPE].hwnd, NULL, 1);
        if (whichunit == 0)
        {   SetGadgetAttrs(gadgets[                   GID_PT_BU4 ], subwin[SUBWINDOW_PAPERTAPE].hwnd, NULL, GA_Disabled, (papertapewhere[whichunit] < PAPERTAPEMAX               && papertapemode[whichunit] == TAPEMODE_STOP) ? FALSE : TRUE, GA_Selected, (papertapemode[whichunit] == TAPEMODE_RECORD) ? TRUE : FALSE, TAG_DONE); // record

            SetGadgetAttrs(gadgets[                   GID_PT_BU10], subwin[SUBWINDOW_PAPERTAPE].hwnd, NULL, GA_Disabled, (                                                          papertapemode[whichunit] == TAPEMODE_NONE) ? FALSE : TRUE,                                                                            TAG_DONE); // create
        }

        SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU12 : GID_PT_BU5 ], subwin[SUBWINDOW_PAPERTAPE].hwnd, NULL, GA_Disabled, (papertapewhere[whichunit] > 0                          && papertapemode[whichunit] == TAPEMODE_STOP) ? FALSE : TRUE,                                                                            TAG_DONE); // rewind
        SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU13 : GID_PT_BU6 ], subwin[SUBWINDOW_PAPERTAPE].hwnd, NULL, GA_Disabled, (papertapewhere[whichunit] < papertapelength[whichunit] && papertapemode[whichunit] == TAPEMODE_STOP) ? FALSE : TRUE, GA_Selected, (papertapemode[whichunit] == TAPEMODE_PLAY  ) ? TRUE : FALSE, TAG_DONE); // play
        SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU14 : GID_PT_BU7 ], subwin[SUBWINDOW_PAPERTAPE].hwnd, NULL, GA_Disabled, (papertapewhere[whichunit] < papertapelength[whichunit] && papertapemode[whichunit] == TAPEMODE_STOP) ? FALSE : TRUE,                                                                            TAG_DONE); // ffwd
        SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU15 : GID_PT_BU8 ], subwin[SUBWINDOW_PAPERTAPE].hwnd, NULL, GA_Disabled, (                                                          papertapemode[whichunit] >  TAPEMODE_STOP) ? FALSE : TRUE, GA_Selected, (papertapemode[whichunit] == TAPEMODE_STOP  ) ? TRUE : FALSE, TAG_DONE); // stop
        SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU16 : GID_PT_BU9 ], subwin[SUBWINDOW_PAPERTAPE].hwnd, NULL, GA_Disabled, (                                                          papertapemode[whichunit] != TAPEMODE_NONE) ? FALSE : TRUE, GA_Selected, (papertapemode[whichunit] == TAPEMODE_NONE  ) ? TRUE : FALSE, TAG_DONE); // eject

        SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU17 : GID_PT_BU11], subwin[SUBWINDOW_PAPERTAPE].hwnd, NULL, GA_Disabled, (                                                          papertapemode[whichunit] == TAPEMODE_NONE) ? FALSE : TRUE,                                                                            TAG_DONE); // insert
#endif
    }

    if (papertapebyte[whichunit] != oldpapertapebyte[whichunit] || papertapemode[whichunit] != oldpapertapemode[whichunit] || force)
    {   sprintf((char*) gtempstring, "%02X", papertapebyte[whichunit]);
#ifdef WIN32
        SetDlgItemText(subwin[SUBWINDOW_PAPERTAPE].hwnd, whichunit ? IDC_PAPERTAPEBYTE2 : IDC_PAPERTAPEBYTE, gtempstring);
#endif
#ifdef AMIGA
        switch (papertapemode[whichunit])
        {
        case  TAPEMODE_PLAY:       SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU20 : GID_PT_BU3], subwin[SUBWINDOW_PAPERTAPE].hwnd, NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[EMUBRUSH_CYAN  ], TAG_DONE);
        acase TAPEMODE_RECORD: if (papertapeprotect[whichunit])
                               {   SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU20 : GID_PT_BU3], subwin[SUBWINDOW_PAPERTAPE].hwnd, NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[EMUBRUSH_YELLOW], TAG_DONE);
                               } else
                               {   SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU20 : GID_PT_BU3], subwin[SUBWINDOW_PAPERTAPE].hwnd, NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[EMUBRUSH_RED   ], TAG_DONE);
                               }
        adefault:                  SetGadgetAttrs(gadgets[whichunit ? GID_PT_BU20 : GID_PT_BU3], subwin[SUBWINDOW_PAPERTAPE].hwnd, NULL, GA_Text, gtempstring, BUTTON_BackgroundPen, emupens[EMUBRUSH_GREEN ], TAG_DONE);
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
{   if (!subwin[SUBWINDOW_DIPS].hwnd)
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

EXPORT int getdiskmodecolour(void)
{   int whichpen;

    switch (drive_mode)
    {
    case  DRIVEMODE_IDLE:    whichpen = EMUPEN_BLUE;
    acase DRIVEMODE_READING: whichpen = EMUPEN_GREEN;
    acase DRIVEMODE_WRITING: whichpen = EMUPEN_RED;
    adefault:                whichpen = EMUPEN_WHITE; // should never happen
    }

    return whichpen;
}

EXPORT int getdiskclustercolour(int viewingbyte)
{   UBYTE viewingtrack,
          viewingsector;
    int   viewingcluster,
          whichpen;

    switch (machine)
    {
    case  TWIN:
        viewingtrack  =   viewingbyte /   TWIN_TRACKSIZE;
        viewingsector =  (viewingbyte %   TWIN_TRACKSIZE) /   TWIN_BLOCKSIZE    ;
    acase BINBUG:
        viewingtrack  =   viewingbyte / BINBUG_TRACKSIZE;
        viewingsector = ((viewingbyte % BINBUG_TRACKSIZE) / BINBUG_BLOCKSIZE) + 1;
    acase CD2650:
        viewingtrack  =   viewingbyte / CD2650_TRACKSIZE;
        viewingsector = ((viewingbyte % CD2650_TRACKSIZE) / CD2650_BLOCKSIZE) + 1;
    adefault:
        return EMUPEN_WHITE; // should never happen
    }
    get_disk_byte(viewingdrive, viewingtrack, viewingsector, 0, &viewingbyte, &viewingcluster);

    if (!drive[viewingdrive].inserted || viewingcluster == -1)
                                 whichpen = EMUPEN_GREY;
    elif (viewingcluster == -2)  whichpen = EMUPEN_PURPLE;
    else
    {   switch (drive[viewingdrive].bam[viewingcluster])
        {
        case  BAM_LOST:          whichpen = EMUPEN_GREY; // ideally EMU#?_BLACK but text would be illegible
        acase BAM_DIR:           whichpen = EMUPEN_PURPLE;
        acase BAM_FILE:          whichpen = EMUPEN_PINK;
        acase BAM_FREE:          whichpen = EMUPEN_CYAN;
        adefault:                whichpen = EMUPEN_WHITE; // should never happen
    }   }

    return whichpen;
}

EXPORT int getdiskbytecolour(int viewingbyte)
{   FAST UBYTE viewingtrack,
               viewingsector;
    FAST int   diskbyte,
               viewingcluster,
               viewingoffset,
               whichpen;

    switch (machine)
    {
    case  TWIN:
        viewingtrack  =   viewingbyte /   TWIN_TRACKSIZE;
        viewingsector =  (viewingbyte %   TWIN_TRACKSIZE) /   TWIN_BLOCKSIZE    ;
        viewingoffset =  (viewingbyte                     %   TWIN_BLOCKSIZE)   ;
    acase BINBUG:
        viewingtrack  =   viewingbyte / BINBUG_TRACKSIZE;
        viewingsector = ((viewingbyte % BINBUG_TRACKSIZE) / BINBUG_BLOCKSIZE) + 1;
        viewingoffset =  (viewingbyte                     % BINBUG_BLOCKSIZE)   ;
    acase CD2650:
        viewingtrack  =   viewingbyte / CD2650_TRACKSIZE;
        viewingsector = ((viewingbyte % CD2650_TRACKSIZE) / CD2650_BLOCKSIZE) + 1;
        viewingoffset =  (viewingbyte                     % CD2650_BLOCKSIZE)   ;
    }
    get_disk_byte(viewingdrive, viewingtrack, viewingsector, viewingoffset, NULL, &viewingcluster);
    get_disk_byte(curdrive, drive[curdrive].track, drive[curdrive].sector, drive[curdrive].blockoffset, &diskbyte, NULL);

    if (!drive[viewingdrive].inserted || viewingcluster == -1)
                                 whichpen = EMUPEN_GREY;
    elif (viewingbyte == diskbyte)
    {   switch (drive_mode)
        {
        case  DRIVEMODE_IDLE:    whichpen = EMUPEN_BLUE;
        acase DRIVEMODE_READING: whichpen = EMUPEN_GREEN;
        acase DRIVEMODE_WRITING: whichpen = EMUPEN_RED;
    }   }
    elif (drive[viewingdrive].flags[viewingbyte / 8] & (1 << (viewingbyte % 8)))
    {                            whichpen = EMUPEN_ORANGE;
    } elif (machine == BINBUG && viewingoffset <= 1)
    {                            whichpen = EMUPEN_YELLOW;
    } elif (viewingcluster == -2)
    {                            whichpen = EMUPEN_PURPLE;
    } else
    {   switch (drive[viewingdrive].bam[viewingcluster])
        {
        case  BAM_LOST:          whichpen = EMUPEN_GREY;
        acase BAM_DIR:           whichpen = EMUPEN_PURPLE;
        acase BAM_FILE:          whichpen = EMUPEN_PINK;
        acase BAM_FREE:          whichpen = EMUPEN_CYAN;
    }   }

    return whichpen;
}

EXPORT void get_disk_byte(int whichdrive, UBYTE whichtrack, UBYTE whichsector, int whichoffset, int* rc_diskbyte, int* rc_diskcluster)
{   int tempdiskbyte,
        tempcluster;

    if (!drive[whichdrive].inserted)
    {   tempdiskbyte = -1;
        tempcluster = -1;
        goto DONE;
    }

    switch (machine)
    {
    case BINBUG:
        if
        (   whichsector == 0
         || whichsector >  BINBUG_SECTORS
         || whichtrack  >= BINBUG_TRACKS
        )
        {   tempdiskbyte = -1;
            tempcluster  = -1;
        } else
        {   tempdiskbyte = (whichtrack * BINBUG_TRACKSIZE) + ((whichsector - 1) * BINBUG_BLOCKSIZE) + whichoffset;
            tempcluster  = tempdiskbyte / BINBUG_BLOCKSIZE; // zero-based
        }
    acase TWIN:
        if
        (   whichsector >= TWIN_SECTORS
         || whichtrack  >= TWIN_TRACKS
        )
        {   tempdiskbyte = -1;
            tempcluster  = -1;
        } else
        {   tempdiskbyte = (whichtrack *   TWIN_TRACKSIZE) + ( whichsector      *   TWIN_BLOCKSIZE) + whichoffset;
            tempcluster  = (whichtrack == 0) ? (-2) : (((whichtrack - 1) * 4) + (whichsector / 8));
        }
    acase CD2650:
        if
        (   whichsector == 0
         || whichsector >  CD2650_SECTORS
         || whichtrack  >= CD2650_TRACKS
        )
        {   tempdiskbyte = -1;
            tempcluster  = -1;
        } else
        {   tempdiskbyte = (whichtrack * CD2650_TRACKSIZE) + ((whichsector - 1) * CD2650_BLOCKSIZE) + whichoffset;
            tempcluster  = tempdiskbyte / CD2650_BLOCKSIZE; // zero-based
        }
    adefault: // should never happen
        tempdiskbyte = -1;
        tempcluster  = -1;
    }

DONE:
    if (rc_diskbyte)
    {   *rc_diskbyte = tempdiskbyte;
    }
    if (rc_diskcluster)
    {   *rc_diskcluster = tempcluster;
}   }

EXPORT void set_drive_mode(int newdrivemode)
{   drive_mode = newdrivemode;

    update_floppydrive(FALSE, 0);
    update_floppydrive(FALSE, 1);
    update_floppydrive(FALSE, 2);
    update_floppydrive(FALSE, 3);
}

EXPORT void redraw_roll(int whichunit)
{   if   (whichunit == 0) wpa8(CANVAS_ROLL1, 0, 0);
    elif (whichunit == 1) wpa8(CANVAS_ROLL2, 0, 0);
    // else panic
}
