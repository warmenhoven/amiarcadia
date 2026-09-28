// INCLUDES---------------------------------------------------------------

#include "ibm.h"

#include <commctrl.h>
#include <richedit.h>

#include "resource.h"

#ifndef PI
    #define PI 3.141592653589793
#endif

#include <stdio.h>
#include <io.h>       // for _open_osfhandle()
#include <fcntl.h>    // for _O_TEXT

#define EXEC_TYPES_H
#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       HICON                 diskicon = NULL;

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       FLAG                  allglyphs,
                                   oldspinning,
                                   opening,
                                   userrobot;
IMPORT       TEXT                  asciiname_short[259][3 + 1],
                                   fn_tape[4][MAX_PATH + 1],
                                   gtempstring[256 + 1];
IMPORT       UBYTE                 KeyMatrix[SCANCODES / 8],
                                   memory[32768],
                                   oldtapebyte,
                                   pitchangle,
                                   rotorspeed,
                                   windspeed,
                                   winddirection,
                                   yawsensordir,
                                   si50_toggles,
                                   tapeskewage;
IMPORT       UWORD                 linearx,
                                   lineary,
                                   linearu,
                                   linearv,
                                   magneticx,
                                   magneticy,
                                   magneticz,
                                   planepitch,
                                   planeroll;
IMPORT       ULONG                 binbug_interface,
                                   curdrive,
                                   declinate,
                                   eachpage,
                                   inverttape,
                                   narrowprinter,
                                   oldtapecycle,
                                   papertapelength[2],
                                   papertapeprotect[2],
                                   papertapewhere[2],
                                   pong_machine,
                                   samplewhere,
                                   tapelength,
                                   tapewriteprotect,
                                   turbodisk,
                                   verbosedisk,
                                   verbosetape,
                                   viewdiskas;
IMPORT       int                   angles,
                                   ax[2],
                                   ay[4],
                                   batvalue,
                                   binbug_biosver,
                                   binbug_userdrives,
                                   candy[CANDIES],
                                   CatalogPtr,
                                   cd2650_dosver,
                                   cd2650_userdrives,
                                   colourset,
                                   diskblocksize,
                                   drive_mode,
                                   headerlength,
                                   horn,
                                   hornpitch,
                                   lampson,
                                   lockhoriz,
                                   machine,
                                   mdcrblocks,
                                   mdcrfwdstate,
                                   mdcrstate,
                                   memmap,
                                   motion,
                                   otl,
                                   papertapemode[2],
                                   pendown,
                                   pipbug_biosver,
                                   pipbug_periph,
                                   playerid,
                                   players,
                                   pongspeed,
                                   pong_variant,
                                   prtscrolly,
                                   prtunit,
                                   recmode,
                                   robotspeed[2],
                                   robotwheel[2],
                                   serving,
                                   si50_id,
                                   si50_is,
                                   si50_io,
                                   size,
                                   tapeframe,
                                   tapekind,
                                   tapemode,
                                   twin_userdrives,
                                   watchreads,
                                   watchwrites,
                                   viewingdrive;
IMPORT       double                mechpower_kw,
                                   elecpower_kw,
                                   samplewhere_f;
IMPORT       UBYTE*                TapeCopy;
IMPORT       FILE*                 TapeHandle;
IMPORT const STRPTR                mdcrfwdstr[8],
                                   mdcrrevstr[4];
IMPORT       HBRUSH                hBrush[EMUBRUSHES];
IMPORT       HICON                 spinicon[2];
IMPORT       HINSTANCE             InstancePtr;
IMPORT       struct CanvasStruct    canvas[CANVASES];
IMPORT       struct DriveStruct     drive[DRIVES_MAX];
IMPORT       struct IOPortStruct    ioport[258];
IMPORT       struct MachineStruct   machines[MACHINES];
IMPORT const struct MemMapToStruct  memmap_to[MEMMAPS];
IMPORT       struct PrinterStruct   printer[2];
IMPORT       struct SubWindowStruct subwin[SUBWINDOWS];

// MODULE VARIABLES-------------------------------------------------------

MODULE       TEXT                      skewstring[4 + 1]; // enough for "-128"
MODULE       WNDPROC                   wpOldTapeEditProc; // Stores the old original window procedure for the edit control
MODULE       HICON                     tapeimage[TAPEIMAGES];

MODULE const int taperesource[TAPEIMAGES] =
{ IDB_TAPEEMPTY,      //  0
  IDB_TAPEPLAY1,      //  1
  IDB_TAPEPLAY2,      //  2
  IDB_TAPEPLAY3,      //  3
  IDB_TAPEPLAY4,      //  4
  IDB_TAPEPLAY5,      //  5
  IDB_TAPEPLAY6,      //  6
  IDB_TAPEREC1,       //  7
  IDB_TAPEREC2,       //  8
  IDB_TAPEREC3,       //  9
  IDB_TAPEREC4,       // 10
  IDB_TAPEREC5,       // 11
  IDB_TAPEREC6,       // 12
  IDB_TAPESTOP1,      // 13
  IDB_TAPESTOP2,      // 14
  IDB_TAPESTOP3,      // 15
  IDB_TAPESTOP4,      // 16
  IDB_TAPESTOP5,      // 17
  IDB_TAPESTOP6,      // 18
  IDB_TAPEPROTECT1,   // 19
  IDB_TAPEPROTECT2,   // 20
  IDB_TAPEPROTECT3,   // 21
  IDB_TAPEPROTECT4,   // 22
  IDB_TAPEPROTECT5,   // 23
  IDB_TAPEPROTECT6,   // 24
  IDB_ANIMSTOP1,      // 25
  IDB_ANIMSTOP2,      // 26
  IDB_ANIMSTOP3,      // 27
  IDB_ANIMPLAY1,      // 28
  IDB_ANIMPLAY2,      // 29
  IDB_ANIMPLAY3,      // 30
  IDB_ANIMREC1,       // 31
  IDB_ANIMREC2,       // 32
  IDB_ANIMREC3,       // 33
};

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

// MODULE FUNCTIONS-------------------------------------------------------

LRESULT CALLBACK TapeEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

MODULE BOOL CALLBACK  FloppyDriveDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK   IndustrialDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK    PapertapeDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK     PongDIPsDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK      PrinterDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK     SI50DIPsDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK     TapeDeckDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK   MalzakDIPsDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK ZaccariaDIPsDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

// CODE-------------------------------------------------------------------

EXPORT void open_floppydrive(FLAG needupdate)
{   if (!HASDISK)
    {   return; // important!
    } // implied else

    if (viewingdrive >= machines[machine].drives)
    {   viewingdrive = machines[machine].drives - 1;
    }

    if (needupdate)
    {   dir_disk(TRUE, viewingdrive);
    }
    oldspinning = FALSE; // needed even if it actually is spinning
    switch (machine)
    {
    case  BINBUG:
    case  CD2650:
        open_subwindow(SUBWINDOW_FLOPPYDRIVE, MAKEINTRESOURCE(IDD_FLOPPYDRIVE_BINBUG), FloppyDriveDlgProc);
        make_tips(SUBWINDOW_FLOPPYDRIVE, 256, IDC_DISK_0);
    acase TWIN:
        open_subwindow(SUBWINDOW_FLOPPYDRIVE, MAKEINTRESOURCE(IDD_FLOPPYDRIVES_TWIN ), FloppyDriveDlgProc);
        make_tips(SUBWINDOW_FLOPPYDRIVE, 128, IDC_DISK_0);
    }
    update_floppydrive(TRUE, viewingdrive);
}

EXPORT void open_industrial(void)
{   if (machine != PIPBUG)
    {   return;
    } // implied else

    switch (pipbug_periph)
    {
    case  PERIPH_FURNACE:      open_subwindow(SUBWINDOW_INDUSTRIAL, MAKEINTRESOURCE(IDD_FURNACE     ), IndustrialDlgProc);
    acase PERIPH_LINEARISATIE: open_subwindow(SUBWINDOW_INDUSTRIAL, MAKEINTRESOURCE(IDD_LINEARISATIE), IndustrialDlgProc);
    acase PERIPH_ROBOT:        open_subwindow(SUBWINDOW_INDUSTRIAL, MAKEINTRESOURCE(IDD_ROBOT       ), IndustrialDlgProc);
    acase PERIPH_MAGNETOMETER: open_subwindow(SUBWINDOW_INDUSTRIAL, MAKEINTRESOURCE(IDD_MAGNETOMETER), IndustrialDlgProc);
    acase PERIPH_PRINTER:      open_subwindow(SUBWINDOW_INDUSTRIAL, MAKEINTRESOURCE(IDD_EAPRINTER   ), IndustrialDlgProc);
    }

    update_industrial(TRUE);
}

EXPORT void open_papertape(void)
{   if (!HASPAPERTAPE)
    {   return; // important!
    } // implied else

    if (machine == TWIN)
    {   open_subwindow(SUBWINDOW_PAPERTAPE, MAKEINTRESOURCE(IDD_PAPERTAPE_TWIN), PapertapeDlgProc);
        update_papertape(0, TRUE, FALSE);
        update_papertape(1, TRUE, FALSE);
    } else
    {   open_subwindow(SUBWINDOW_PAPERTAPE, MAKEINTRESOURCE(IDD_PAPERTAPE     ), PapertapeDlgProc);
        update_papertape(0, TRUE, FALSE);
}   }

EXPORT void open_tapedeck(void)
{   if (!TAPABLE)
    {   return; // important!
    } // implied else

    if (machine == PHUNSY)
    {   open_subwindow(SUBWINDOW_TAPEDECK, MAKEINTRESOURCE(IDD_TAPEDECK_PHUNSY), TapeDeckDlgProc);
    } else
    {   open_subwindow(SUBWINDOW_TAPEDECK, MAKEINTRESOURCE(IDD_TAPEDECK       ), TapeDeckDlgProc);
    }
    update_tapedeck(TRUE);
}

EXPORT void tools_printer(void)
{   switch (machine)
    {
    case PIPBUG:
    case BINBUG:
    case CD2650:
        open_subwindow(SUBWINDOW_PRINTER, MAKEINTRESOURCE(IDD_PRINTER     ), PrinterDlgProc);
    acase TWIN:
        open_subwindow(SUBWINDOW_PRINTER, MAKEINTRESOURCE(IDD_PRINTER_TWIN), PrinterDlgProc);
    adefault:
        return;
    }

    change_printer();
    update_printer(); // can't do this during WM_INITDIALOG as the IDC_PRINTER gadget is not fully set up then
}

MODULE BOOL CALLBACK PrinterDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT int         mousex, mousey,
                          neweachpage;
    TRANSIENT signed int  movement;
    TRANSIENT LONG        gid;
    TRANSIENT ULONG       scancode;
    TRANSIENT PAINTSTRUCT localps;
    TRANSIENT RECT        localrect;
    TRANSIENT POINT       thepoint;
    PERSIST   FLAG        first = TRUE;
    PERSIST   RECT        candyrect;
    PERSIST   SCROLLINFO  TheScrollInfo =
{   sizeof(SCROLLINFO),
    SIF_PAGE | SIF_RANGE | SIF_POS,
    0,                   // minimum
    C306HEIGHT_FULL - 1, // maximum
    C306HEIGHT_VIEW,
    0,
    0
};

    switch (Message)
    {
    case WM_INITDIALOG:
        subwin[SUBWINDOW_PRINTER].hwnd = hwnd;

        if (machine == TWIN)
        {   SetWindowText(hwnd, LLL(              MSG_HAIL_C306    , "Centronics Model 306C Printer Output"));
            setdlgtext(   hwnd, IDL_PRINTERUNIT , MSG_UNIT         , "Unit"                                 );
            setdlgtext(   hwnd, IDL_DEFCPL      , MSG_DEFCPL       , "Default Characters per Line"          );
            setdlgtext(   hwnd, IDC_DEFCPL_80   , MSG_DEFCPL_80    , "80 (normal)"                          );
            setdlgtext(   hwnd, IDC_DEFCPL_132  , MSG_DEFCPL_132   , "132 (condensed)"                      );
            setdlgtext(   hwnd, IDL_RIBBON      , MSG_RIBBON       , "Ribbon"                               );
            setdlgtext(   hwnd, IDC_RIBBON_BLACK, MSG_COLOUR2_BLACK, "Black"                                );
            setdlgtext(   hwnd, IDC_RIBBON_RED  , MSG_LABEL_RED    , "Red"                                  );
            setdlgtext(   hwnd, IDC_RIBBON_GREEN, MSG_LABEL_GREEN  , "Green"                                );
            setdlgtext(   hwnd, IDC_RIBBON_BLUE , MSG_LABEL_BLUE   , "Blue"                                 );

            DISCARD CheckRadioButton
            (   subwin[SUBWINDOW_PRINTER].hwnd,
                IDC_PRINTERUNIT_1ST,
                IDC_PRINTERUNIT_2ND,
                IDC_PRINTERUNIT_1ST + prtunit // prtunit is 0..1
            );
            DISCARD CheckRadioButton
            (   subwin[SUBWINDOW_PRINTER].hwnd,
                IDC_DEFCPL_80,
                IDC_DEFCPL_132,
                IDC_DEFCPL_80 + printer[prtunit].condensed // condensed is 0..1
            );
            DISCARD CheckRadioButton
            (   subwin[SUBWINDOW_PRINTER].hwnd,
                IDC_RIBBON_BLACK,
                IDC_RIBBON_BLUE,
                IDC_RIBBON_BLACK + printer[prtunit].ribbon // ribbon is 0..3
            );

            TheScrollInfo.nPos = prtscrolly;
            SetScrollInfo(GetDlgItem(hwnd, IDC_PRINTERVSCROLL), SB_CTL, &TheScrollInfo, TRUE);
        } else
        {   SetWindowText(hwnd, LLL(                 MSG_HAIL_PRINTER, "EUY-10E023LE Printer Output"      ));
            setdlgtext(   hwnd, IDL_PRINTERCPL,      MSG_PRINTERCPL,   "Characters per Line"               );

            DISCARD CheckRadioButton
            (   subwin[SUBWINDOW_PRINTER].hwnd,
                IDC_PRINTERCPL_16,
                IDC_PRINTERCPL_32,
                IDC_PRINTERCPL_16 + narrowprinter // narrowprinter is 0..1
            );
        }
        setdlgtext(hwnd, IDC_PRTSAVETEXT, MSG_EDIT_SAVETEXT,  "Save text...");
        setdlgtext(hwnd, IDC_PRTCOPYTEXT, MSG_EDIT_COPYTEXT,  "Copy text"   );
        setdlgtext(hwnd, IDC_PRTEJECT,    MSG_KEY_EJECT_LONG, "Eject page"  );
        setdlgtext(hwnd, IDC_PRTDEMO,     MSG_DEMO,           "Demonstrate" );
        setdlgtext(hwnd, IDC_EACHPAGE,    MSG_EACHPAGE,       "Automatically save each page?");
        setdlgtext(hwnd, IDL_PAGE,        MSG_PAGE,           "Page:"       );
        // the others are owner-drawn buttons

        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_EACHPAGE),
            BM_SETCHECK,
            eachpage ? BST_CHECKED : BST_UNCHECKED,
            0
        );

        DISCARD GetWindowRect(GetDlgItem(hwnd, IDC_CANDY3), &localrect);
        thepoint.x        = localrect.left;
        thepoint.y        = localrect.top;
        DISCARD ScreenToClient(hwnd, &thepoint);
        candyrect.left    = thepoint.x;
        candyrect.top     = thepoint.y;
        thepoint.x        = localrect.right;
        thepoint.y        = localrect.bottom;
        DISCARD ScreenToClient(hwnd, &thepoint);
        candyrect.right   = thepoint.x;
        candyrect.bottom  = thepoint.y;
        if (candy[3 - 1])
        {   ShowWindow(GetDlgItem(hwnd, IDC_CANDY3), SW_SHOW);
        }

        move_subwindow(SUBWINDOW_PRINTER);

        change_printer();
        // can't do update_printer() in WM_INITDIALOG
    return FALSE;
    case WM_ACTIVATE:
        if (!opening)
        {   do_autopause(wParam, lParam);
        }
    acase WM_CLOSE:
        DestroyWindow(hwnd);
        subwin[SUBWINDOW_PRINTER].hwnd = NULL;
    acase WM_DRAWITEM:
        switch (wParam)
        {
        case  IDC_PRTSAVEBMP:  drawitem((struct tagDRAWITEMSTRUCT*) lParam, FALSE);
        acase IDC_PRTCOPYSCRN: drawitem((struct tagDRAWITEMSTRUCT*) lParam, FALSE);
        }
    return TRUE;
    case WM_COMMAND:
        gid = (int) LOWORD(wParam);
        switch (gid)
        {
        case IDC_PRTSAVEBMP:
            printer_copygfx(FALSE, KIND_BMP, TRUE, prtunit);
        acase IDC_PRTSAVETEXT:
            edit_savetext(FALSE, TRUE, prtunit, FALSE);
        acase IDC_PRTCOPYSCRN:
            printer_copygfx(TRUE, KIND_BMP, TRUE, prtunit);
        acase IDC_PRTCOPYTEXT:
            edit_savetext(TRUE, TRUE, prtunit, FALSE);
        acase IDC_PRTEJECT:
            printer_eject(prtunit);
            update_printer();
        acase IDC_PRTDEMO:
            if (machine == TWIN)
            {   twin_prtdemo();
            } else
            {   pipbin_prtdemo();
            }
        acase IDC_PRINTERCPL_16:
        case IDC_PRINTERCPL_32:
            narrowprinter = gid - IDC_PRINTERCPL_16;
            DISCARD CheckRadioButton
            (   hwnd,
                IDC_PRINTERCPL_16,
                IDC_PRINTERCPL_32,
                IDC_PRINTERCPL_16 + narrowprinter
            );
        acase IDC_PRINTERUNIT_1ST:
        case IDC_PRINTERUNIT_2ND:
            prtunit = gid - IDC_PRINTERUNIT_1ST;
            DISCARD CheckRadioButton
            (   hwnd,
                IDC_PRINTERUNIT_1ST,
                IDC_PRINTERUNIT_2ND,
                IDC_PRINTERUNIT_1ST + prtunit
            );
            DISCARD CheckRadioButton
            (   hwnd,
                IDC_DEFCPL_80,
                IDC_DEFCPL_132,
                IDC_DEFCPL_80 + printer[prtunit].condensed
            );
            DISCARD CheckRadioButton
            (   subwin[SUBWINDOW_PRINTER].hwnd,
                IDC_RIBBON_BLACK,
                IDC_RIBBON_BLUE,
                IDC_RIBBON_BLACK + printer[prtunit].ribbon // ribbon is 0..3
            );
            change_printer();
            update_printer();
        acase IDC_DEFCPL_80:
        case IDC_DEFCPL_132:
            printer[prtunit].condensed = gid - IDC_DEFCPL_80;
            DISCARD CheckRadioButton
            (   hwnd,
                IDC_DEFCPL_80,
                IDC_DEFCPL_132,
                IDC_DEFCPL_80 + printer[prtunit].condensed
            );
        acase IDC_RIBBON_BLACK:
        case IDC_RIBBON_RED:
        case IDC_RIBBON_GREEN:
        case IDC_RIBBON_BLUE:
            printer[prtunit].ribbon = gid - IDC_RIBBON_BLACK;
            DISCARD CheckRadioButton
            (   subwin[SUBWINDOW_PRINTER].hwnd,
                IDC_RIBBON_BLACK,
                IDC_RIBBON_BLUE,
                IDC_RIBBON_BLACK + printer[prtunit].ribbon // ribbon is 0..3
            );
        acase IDC_CANDY3:
            candy[3 - 1] = FALSE;
            ShowWindow(GetDlgItem(hwnd, IDC_CANDY3), SW_HIDE);
        acase IDC_EACHPAGE:
            neweachpage = (SendMessage(GetDlgItem(hwnd, IDC_EACHPAGE), BM_GETCHECK, 0, 0) == BST_CHECKED) ? TRUE : FALSE;
            if (eachpage && !neweachpage)
            {   printer_savepartial(0);
                printer_savepartial(1);
            }
            eachpage = neweachpage;
        }
    return TRUE;
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
        mousex = (int) LOWORD(lParam); // pixels -> dialog units
        mousey = (int) HIWORD(lParam); // pixels -> dialog units
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "X: %d, Y: %d.\n", mousex, mousey);
#endif
        if
        (   mousex >= candyrect.left
         && mousex <= candyrect.right
         && mousey >= candyrect.top
         && mousey <= candyrect.bottom
        )
        {   candy[3 - 1] = TRUE;
            ShowWindow(GetDlgItem(hwnd, IDC_CANDY3), SW_SHOW);
        }
    return FALSE;
    case WM_PAINT:
        BeginPaint(hwnd, &localps);
        update_printer();
        EndPaint(hwnd, &localps);
    return FALSE; // important!
    case WM_MOVE:
        reset_fps();
    return FALSE;
    case WM_KEYDOWN:
        scancode = (lParam & 33488896) >> 16;
        handle_keydown(scancode);
    return FALSE;
    case WM_KEYUP:
        scancode = (lParam & 33488896) >> 16;
        handle_keyup(scancode);
    return FALSE;
    case WM_VSCROLL:
        switch (LOWORD(wParam))
        {
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            prtscrolly = (int) HIWORD(wParam);
            SetScrollPos(GetDlgItem(hwnd, IDC_PRINTERVSCROLL), SB_CTL, prtscrolly, TRUE); // set new scroll position
            change_printer();
            update_printer();
        acase SB_LINEUP:
            if (prtscrolly < C306PRINTERY)
            {   prtscrolly =  0;
            } else
            {   prtscrolly -= C306PRINTERY;
            }
            SetScrollPos(GetDlgItem(hwnd, IDC_PRINTERVSCROLL), SB_CTL, prtscrolly, TRUE); // set new scroll position
            change_printer();
            update_printer();
        acase SB_LINEDOWN:
            if (prtscrolly + C306PRINTERY >= PRINTERHEIGHT_FULL - PRINTERHEIGHT_VIEW)
            {   prtscrolly =  PRINTERHEIGHT_FULL - PRINTERHEIGHT_VIEW;
            } else
            {   prtscrolly += C306PRINTERY;
            }
            SetScrollPos(GetDlgItem(hwnd, IDC_PRINTERVSCROLL), SB_CTL, prtscrolly, TRUE); // set new scroll position
            change_printer();
            update_printer();
        acase SB_PAGEUP:
            if (prtscrolly - PRINTERHEIGHT_VIEW - C306PRINTERY < 0)
            {   prtscrolly =  0;
            } else
            {   prtscrolly -= PRINTERHEIGHT_VIEW - C306PRINTERY;
            }
            SetScrollPos(GetDlgItem(hwnd, IDC_PRINTERVSCROLL), SB_CTL, prtscrolly, TRUE); // set new scroll position
            change_printer();
            update_printer();
        acase SB_PAGEDOWN:
            if (prtscrolly + PRINTERHEIGHT_VIEW - C306PRINTERY >= PRINTERHEIGHT_FULL - PRINTERHEIGHT_VIEW)
            {   prtscrolly =  PRINTERHEIGHT_FULL - PRINTERHEIGHT_VIEW;
            } else
            {   prtscrolly += PRINTERHEIGHT_VIEW - C306PRINTERY;
            }
            SetScrollPos(GetDlgItem(hwnd, IDC_PRINTERVSCROLL), SB_CTL, prtscrolly, TRUE); // set new scroll position
            change_printer();
            update_printer();
        }
    return TRUE;
    case WM_MOUSEWHEEL:
        if (machine == TWIN)
        {   movement = ((short) HIWORD(wParam)) / WHEELDELTA;
            if (movement >= 1) // forwards/up/away
            {   if (prtscrolly < C306PRINTERY)
                {   prtscrolly =  0;
                } else
                {   prtscrolly -= C306PRINTERY;
            }   }
            elif (movement <= -1) // back/down/towards
            {   if (prtscrolly + C306PRINTERY >= PRINTERHEIGHT_FULL - PRINTERHEIGHT_VIEW)
                {   prtscrolly =  PRINTERHEIGHT_FULL - PRINTERHEIGHT_VIEW;
                } else
                {   prtscrolly += C306PRINTERY;
            }   }
            DISCARD SetScrollPos(GetDlgItem(subwin[SUBWINDOW_PRINTER].hwnd, IDC_PRINTERVSCROLL), SB_CTL, prtscrolly, TRUE);
            change_printer();
            update_printer();
        }
    return TRUE;
    default:
    return FALSE;
    }
    return TRUE;
}

EXPORT void update_printer(void)
{   if
    (   !subwin[SUBWINDOW_PRINTER].hwnd
     || !GetDlgItem(subwin[SUBWINDOW_PRINTER].hwnd, IDC_PRINTER)
    )
    {   return; // important!
    }

    wpa8(CANVAS_PRINTER, 0, 0);

    if (printer[prtunit].page > 999)
    {   strcpy(gtempstring, "###");
    } else
    {   sprintf(gtempstring, "%d", printer[prtunit].page);
    }
    SetDlgItemText(subwin[SUBWINDOW_PRINTER].hwnd, IDC_PAGE, gtempstring);
}

EXPORT void update_floppytips(void)
{   TRANSIENT int      i;
    TRANSIENT TOOLINFO ti;
    PERSIST   TEXT     clusterstr[3 + 1],
                       tempstring[80 + 1];

    for (i = 0; i < 256; i++)
    {   switch (machine)
        {
        case TWIN:
            if (drive[i / 128].viewstart < TWIN_TRACKSIZE)
            {   sprintf(clusterstr, "-");
            } else
            {   sprintf(clusterstr, "%d", (drive[i / 128].viewstart - TWIN_TRACKSIZE) / (TWIN_BLOCKSIZE * 8));
            }
            if (drive[i / 128].inserted)
            {   sprintf
                (   tempstring,
                    LLL
                    (   MSG_TCSBC,
                        "Track: %d\n" \
                        "Cluster: %s\n" \
                        "Sector: %d\n" \
                        "Byte: $%X\n" \
                        "Contents: $%02X (%s)"
                    ),
                    drive[i / 128].viewstart / TWIN_TRACKSIZE,
                    clusterstr,
                    (drive[i / 128].viewstart % TWIN_TRACKSIZE) / TWIN_BLOCKSIZE,
                    drive[i / 128].viewstart + (i % 128),
                    drive[i / 128].contents[drive[i / 128].viewstart + (i % 128)],
                    asciiname_short[drive[i / 128].contents[drive[i / 128].viewstart + (i % 128)]]
                );
            } else
            {   sprintf
                (   tempstring,
                    LLL
                    (   MSG_TCSB,
                        "Track: %d\n" \
                        "Cluster: %s\n" \
                        "Sector: %d\n" \
                        "Byte: $%X"
                    ),
                    drive[i / 128].viewstart / TWIN_TRACKSIZE,
                    clusterstr,
                    (drive[i / 128].viewstart % TWIN_TRACKSIZE) / TWIN_BLOCKSIZE,
                    drive[i / 128].viewstart + (i % 128)
                );
            }
        acase CD2650:
            if (drive[viewingdrive].inserted)
            {   sprintf
                (   tempstring,
                    LLL
                    (   MSG_TSBC,
                        "Track: %d\n" \
                        "Sector: %d\n" \
                        "Byte: $%X\n" \
                        "Contents: $%02X (%s)"
                    ),
                    drive[viewingdrive].viewstart / CD2650_TRACKSIZE,
                    ((drive[viewingdrive].viewstart % CD2650_TRACKSIZE) / CD2650_BLOCKSIZE) + 1,
                    drive[viewingdrive].viewstart + i,
                    drive[viewingdrive].contents[drive[viewingdrive].viewstart + i],
                    asciiname_short[drive[viewingdrive].contents[drive[viewingdrive].viewstart + i]]
                );
            } else
            {   sprintf
                (   tempstring,
                    LLL
                    (   MSG_TSB,
                        "Track: %d\n" \
                        "Sector: %d\n" \
                        "Byte: $%X"
                    ),
                    drive[viewingdrive].viewstart / CD2650_TRACKSIZE,
                    ((drive[viewingdrive].viewstart % CD2650_TRACKSIZE) / CD2650_BLOCKSIZE) + 1,
                    drive[viewingdrive].viewstart + i
                );
            }
        acase BINBUG:
            if (drive[viewingdrive].inserted)
            {   sprintf
                (   tempstring,
                    LLL
                    (   MSG_TSBC,
                        "Track: %d\n" \
                        "Sector: %d\n" \
                        "Byte: $%X\n" \
                        "Contents: $%02X (%s)"
                    ),
                    drive[viewingdrive].viewstart / BINBUG_TRACKSIZE,
                    ((drive[viewingdrive].viewstart % BINBUG_TRACKSIZE) / BINBUG_BLOCKSIZE) + 1,
                    drive[viewingdrive].viewstart + i,
                    drive[viewingdrive].contents[drive[viewingdrive].viewstart + i],
                    asciiname_short[drive[viewingdrive].contents[drive[viewingdrive].viewstart + i]]
                );
            } else
            {   sprintf
                (   tempstring,
                    LLL
                    (   MSG_TSB,
                        "Track: %d\n" \
                        "Sector: %d\n" \
                        "Byte: $%X"
                    ),
                    drive[viewingdrive].viewstart / BINBUG_TRACKSIZE,
                    ((drive[viewingdrive].viewstart % BINBUG_TRACKSIZE) / BINBUG_BLOCKSIZE) + 1,
                    drive[viewingdrive].viewstart + i
                );
        }   }

        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
        ti.hwnd     = subwin[SUBWINDOW_FLOPPYDRIVE].hwnd;
        ti.uId      = i;
        ti.hinst    = InstancePtr;
        ti.lpszText = tempstring; // this gets copied
        SendMessage(subwin[SUBWINDOW_FLOPPYDRIVE].tips, TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
}   }

EXPORT void edit_dips(void)
{   switch (machine)
    {
    case INSTRUCTOR:
        open_subwindow(    SUBWINDOW_DIPS, MAKEINTRESOURCE(IDD_DIPS_INSTRUCTOR), SI50DIPsDlgProc);
    acase ZACCARIA:
        open_subwindow(    SUBWINDOW_DIPS, MAKEINTRESOURCE(IDD_DIPS_ZACCARIA  ), ZaccariaDIPsDlgProc);
    acase MALZAK:
        if (memmap == MEMMAP_MALZAK2)
        {   open_subwindow(SUBWINDOW_DIPS, MAKEINTRESOURCE(IDD_DIPS_MALZAK2   ), MalzakDIPsDlgProc);
        }
    acase PONG:
        if (memmap == MEMMAP_8550)
        {   open_subwindow(SUBWINDOW_DIPS, MAKEINTRESOURCE(IDD_DIPS_8550      ), PongDIPsDlgProc);
        } else
        {   // assert(memmap == MEMMAP_8600);
            open_subwindow(SUBWINDOW_DIPS, MAKEINTRESOURCE(IDD_DIPS_8600      ), PongDIPsDlgProc);
    }   }

    if (recmode == RECMODE_PLAY)
    {   ghost_dips(FALSE);
}   }

MODULE BOOL CALLBACK TapeDeckDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT int         temp;
    TRANSIENT LONG        gid;
    TRANSIENT PAINTSTRUCT localps;
    FAST      int         i, j,
                          mousex, mousey;
    FAST      double      scaleby;
    FAST      POINT       thepoint;
    FAST      HICON       localhicon;
    FAST      RECT        localrect;
    PERSIST   FLAG        ignoreskew = FALSE;
    PERSIST   RECT        candyrect,
                          waveformrect;

    switch (Message)
    {
    case WM_INITDIALOG:
        subwin[SUBWINDOW_TAPEDECK].hwnd = hwnd;

        setdlgtext(hwnd, IDL_POSITION        , MSG_HAIL_POSITION, "Position"               );
        setdlgtext(hwnd, IDL_SKEWING         , MSG_HAIL_SKEWING , "Skewing"                );
        setdlgtext(hwnd, IDL_INTERFACE       , MSG_INTERFACE    , "Interface"              );
        setdlgtext(hwnd, IDC_CREATE8SVX      , MSG_CREATE8SVX   , "Create IFF 8SVX tape...");
        setdlgtext(hwnd, IDC_CREATEAIFF      , MSG_CREATEAIFF   , "Create IFF AIFF tape...");
        setdlgtext(hwnd, IDC_CREATEWAV       , MSG_CREATEWAV    , "Create WAV tape..."     );
        setdlgtext(hwnd, IDC_INSERTTAPE      , MSG_INSERTTAPE   , "Insert tape..."         );
        setdlgtext(hwnd, IDC_INVERTTAPE      , MSG_INVERTTAPE   , "Invert waveform?"       );
        setdlgtext(hwnd, IDC_VERBOSETAPE     , MSG_VERBOSETAPE  , "Verbose output?"        );
        setdlgtext(hwnd, IDC_TAPEWRITEPROTECT, MSG_WRITEPROTECT , "Write protect?"         );

        localhicon = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon), IMAGE_ICON, 32, 32, 0);
        DISCARD SendMessage(GetDlgItem(hwnd, IDL_GLYPH), STM_SETICON, (WPARAM) localhicon, (LPARAM) 0);

        if (machine != BINBUG)
        {   EnableWindow(GetDlgItem(hwnd, IDC_EA77CC4), FALSE);
            EnableWindow(GetDlgItem(hwnd, IDC_ACOS   ), FALSE);
        }
        DISCARD CheckRadioButton(hwnd, IDC_EA77CC4, IDC_ACOS, IDC_EA77CC4 + binbug_interface);
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_INVERTTAPE),
            BM_SETCHECK,
            inverttape ? BST_CHECKED : BST_UNCHECKED,
            0
        );

        if (machine == PHUNSY && fn_tape[1][0] == EOS)
        {   EnableWindow(GetDlgItem(hwnd, IDC_UPDATEMDCR), FALSE);
        }
        if
        (    machine != ELEKTOR
         && (machine != PIPBUG || pipbug_biosver != PIPBUG_PIPBUG1BIOS)
         && (machine != BINBUG || binbug_biosver != BINBUG_61)
         &&  machine != INSTRUCTOR
         &&  machine != CD2650
         &&  machine != PHUNSY
        )
        {   EnableWindow(GetDlgItem(hwnd, IDC_VERBOSETAPE), FALSE);
        }
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_VERBOSETAPE),
            BM_SETCHECK,
            verbosetape ? BST_CHECKED : BST_UNCHECKED,
            0
        );
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_TAPEWRITEPROTECT),
            BM_SETCHECK,
            tapewriteprotect ? BST_CHECKED : BST_UNCHECKED,
            0
        );
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_SKEW),
            TBM_SETRANGE,
            FALSE,
            MAKELONG(-96, 96)
        );
        temp = (int) (tapeskewage - 128);
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_SKEW),
            TBM_SETPOS,
            TRUE,
            temp
        );
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_SKEW),
            TBM_SETTICFREQ,
            16,
            0
        );
        stcl_d(skewstring, temp);
        SetDlgItemText(hwnd, IDC_SKEW2, skewstring);

        for (i = 0; i < TAPEIMAGES; i++)
        {   tapeimage[i] = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(taperesource[i]));
        }

        DISCARD GetWindowRect(GetDlgItem(hwnd, IDC_WAVEFORM), &localrect);
        thepoint.x           = localrect.left;
        thepoint.y           = localrect.top;
        DISCARD ScreenToClient(hwnd, &thepoint);
        waveformrect.left    = thepoint.x;
        waveformrect.top     = thepoint.y;

        if (machine == PHUNSY)
        {   DISCARD GetWindowRect(GetDlgItem(hwnd, IDC_CANDY1), &localrect);
            thepoint.x       = localrect.left;
            thepoint.y       = localrect.top;
            DISCARD ScreenToClient(hwnd, &thepoint);
            candyrect.left   = thepoint.x;
            candyrect.top    = thepoint.y;
            thepoint.x       = localrect.right;
            thepoint.y       = localrect.bottom;
            DISCARD ScreenToClient(hwnd, &thepoint);
            candyrect.right  = thepoint.x;
            candyrect.bottom = thepoint.y;
            if (candy[1 - 1])
            {   ShowWindow(GetDlgItem(hwnd, IDC_CANDY1), SW_SHOW);
        }   }

        wpOldTapeEditProc = (WNDPROC) SetWindowLong
        (   GetDlgItem(hwnd, IDC_SKEW2),
            GWL_WNDPROC,
            (LONG) TapeEditProc
        );

        move_subwindow(SUBWINDOW_TAPEDECK);
    return FALSE;
    case WM_ACTIVATE:
        do_autopause(wParam, lParam);
    acase WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
        mousex = (int) LOWORD(lParam);
        mousey = (int) HIWORD(lParam);
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "X: %d, Y: %d.\n", mousex, mousey);
#endif
        if
        (   machine == PHUNSY
         && mousex >= candyrect.left
         && mousex <= candyrect.right
         && mousey >= candyrect.top
         && mousey <= candyrect.bottom
        )
        {   candy[1 - 1] = TRUE;
            ShowWindow(GetDlgItem(hwnd, IDC_CANDY1), SW_SHOW);
        } elif
        (   tapemode == TAPEMODE_STOP
         && otl
         && mousex >= waveformrect.left
         && mousex <  waveformrect.left + WAVEFORMWIDTH
         && mousey >= waveformrect.top
         && mousey <  waveformrect.top  + WAVEFORMHEIGHT
        )
        {   // assert(TapeCopy);
            mousex        -= waveformrect.left;
            scaleby       = (double) otl / (double) WAVEFORMWIDTH;
            samplewhere_f = (double) mousex * scaleby;
            samplewhere   = (ULONG ) samplewhere_f;
            update_tapedeck(TRUE);
        }
    return FALSE;
    case WM_DRAWITEM:
        switch (wParam)
        {
        case  IDC_REWIND:    drawitem((struct tagDRAWITEMSTRUCT*) lParam,         FALSE                       );
        acase IDC_STOPTAPE:  drawitem((struct tagDRAWITEMSTRUCT*) lParam, (FLAG) (tapemode == TAPEMODE_STOP  ));
        acase IDC_PLAY:      drawitem((struct tagDRAWITEMSTRUCT*) lParam, (FLAG) (tapemode == TAPEMODE_PLAY  ));
        acase IDC_RECORD:    drawitem((struct tagDRAWITEMSTRUCT*) lParam, (FLAG) (tapemode == TAPEMODE_RECORD));
        acase IDC_FFWD:      drawitem((struct tagDRAWITEMSTRUCT*) lParam,         FALSE                       );
        acase IDC_EJECTTAPE: drawitem((struct tagDRAWITEMSTRUCT*) lParam, (FLAG) (tapemode == TAPEMODE_NONE  ));
        }
    return TRUE;
    case WM_HSCROLL:
        if (lParam == (long) GetDlgItem(hwnd, IDC_POSITIONSLIDER))
        {   if (tapemode == TAPEMODE_STOP)
            {   samplewhere = SendMessage
                (   GetDlgItem(hwnd, IDC_POSITIONSLIDER),
                    TBM_GETPOS,
                    0,
                    0
                ); // because LOWORD(lParam) is not always correct
                if (samplewhere > tapelength) // this can happen on Windows with an empty (zero-length) tape
                {   samplewhere = tapelength;
                }
                samplewhere_f = (double) samplewhere;
                DISCARD fseek(TapeHandle, headerlength + samplewhere, SEEK_SET);
                update_tapedeck(TRUE);
        }   }
    acase WM_VSCROLL:
        if (lParam == (long) GetDlgItem(hwnd, IDC_SKEW))
        {   temp = SendMessage
            (   GetDlgItem(hwnd, IDC_SKEW),
                TBM_GETPOS,
                0,
                0
            ); // because LOWORD(lParam) is not always correct
            // -96 (at top) to +96 (at bottom)

            tapeskewage = (UBYTE) (128 + temp);
            stcl_d(skewstring, temp);
            SetDlgItemText(hwnd, IDC_SKEW2, skewstring);
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "Tape skewage is $%02X.\n", tapeskewage);
#endif
            update_waveform();
        }
    acase WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_REWIND:
            // assert(tapemode == TAPEMODE_STOP);
            samplewhere_f = 0.0;
            samplewhere   = 0;
            DISCARD fseek(TapeHandle, headerlength + samplewhere, SEEK_SET);
            tapeframe = 0;
            update_tapedeck(TRUE);
        acase IDC_STOPTAPE:
            // assert(tapemode > TAPEMODE_STOP);
            tape_stop();
        acase IDC_RECORD:
            tape_record();
        acase IDC_PLAY:
            tape_play();
        acase IDC_FFWD:
            // assert(tapemode == TAPEMODE_STOP);
            samplewhere_f = tapelength;
            samplewhere   = tapelength;
            DISCARD fseek(TapeHandle, headerlength + samplewhere, SEEK_SET);
            tapeframe = 0; // not really correct
            update_tapedeck(TRUE);
        acase IDC_EJECTTAPE:
            // assert(tapemode != TAPEMODE_NONE);
            tape_eject();
        acase IDC_CREATE8SVX:
            // assert(tapemode == TAPEMODE_NONE);
            tapekind = KIND_8SVX;
            create_tape();
        acase IDC_CREATEAIFF:
            // assert(tapemode == TAPEMODE_NONE);
            tapekind = KIND_AIFF;
            create_tape();
        acase IDC_CREATEWAV:
            // assert(tapemode == TAPEMODE_NONE);
            tapekind = KIND_WAV;
            create_tape();
        acase IDC_INSERTTAPE:
            // assert(tapemode == TAPEMODE_NONE);
            load_tape(TRUE);
        acase IDC_EA77CC4:
            binbug_interface = EA77CC4;
        acase IDC_ACOS:
            binbug_interface = ACOS;
        acase IDC_INVERTTAPE:
            inverttape = (SendMessage(GetDlgItem(hwnd, IDC_INVERTTAPE), BM_GETCHECK, 0, 0) == BST_CHECKED) ? TRUE : FALSE;
            update_waveform();
        acase IDC_VERBOSETAPE:
            verbosetape = (SendMessage(GetDlgItem(hwnd, IDC_VERBOSETAPE), BM_GETCHECK, 0, 0) == BST_CHECKED) ? TRUE : FALSE;
        acase IDC_TAPEWRITEPROTECT:
            tapewriteprotect = (SendMessage(GetDlgItem(hwnd, IDC_TAPEWRITEPROTECT), BM_GETCHECK, 0, 0) == BST_CHECKED) ? TRUE : FALSE;
            update_waveform();
        acase IDC_CREATEMDCR:
            create_mdcr();
        acase IDC_INSERTMDCR:
            DISCARD insert_mdcr(TRUE);
        acase IDC_UPDATEMDCR:
            DISCARD update_mdcr();
        acase IDC_SPOOLER:
            if (tapemode == TAPEMODE_NONE)
            {   load_tape(TRUE);
            }
        acase IDC_CANDY1:
            candy[1 - 1] = FALSE;
            ShowWindow(GetDlgItem(hwnd, IDC_CANDY1), SW_HIDE);
        acase IDC_SKEW2:
            if (HIWORD(wParam) == EN_KILLFOCUS)
            {   if (ignoreskew)
                {   ignoreskew = FALSE;
                } else
                {   GetWindowText(GetDlgItem(hwnd, IDC_SKEW2), skewstring, 4 + 1);
                    temp = atoi(skewstring);
                    if (temp < -96)
                    {   temp = -96;
                    } elif (temp > 96)
                    {   temp = 96;
                    }
                    stcl_d(skewstring, temp);
                    SetDlgItemText(hwnd, IDC_SKEW2, skewstring);
                    tapeskewage = (UBYTE) (128 + temp);
                    SendMessage(GetDlgItem(hwnd, IDC_SKEW), TBM_SETPOS, TRUE, temp);
                    update_waveform();
        }   }   }
    acase WM_CLOSE:
        clearkybd();
        for (i = 0; i < TAPEIMAGES; i++)
        {   DeleteObject(tapeimage[i]);
        }
        DestroyWindow(hwnd);
        subwin[SUBWINDOW_TAPEDECK].hwnd = NULL;
        updatemenu(MENUITEM_TAPEDECK);
    return TRUE;
    case WM_CTLCOLORSTATIC:
        gid = GetWindowLong((HWND) lParam, GWL_ID);
        switch (gid)
        {
        case IDC_TAPEBYTE:
            switch (tapemode)
            {
            case TAPEMODE_PLAY:       SetBkColor((HDC) wParam, EMUPEN_CYAN  ); return (LRESULT) hBrush[EMUBRUSH_CYAN];
            case TAPEMODE_RECORD: if (tapewriteprotect)
                                  {   SetBkColor((HDC) wParam, EMUPEN_YELLOW); return (LRESULT) hBrush[EMUBRUSH_YELLOW];
                                  } else
                                  {   SetBkColor((HDC) wParam, EMUPEN_RED   ); return (LRESULT) hBrush[EMUBRUSH_RED];
                                  }
            default:                  SetBkColor((HDC) wParam, EMUPEN_GREEN ); return (LRESULT) hBrush[EMUBRUSH_GREEN];
            }
        case IDC_MDCRSTATUS: // no need for acase
            if (mdcrblocks == 0)
            {   SetBkColor((HDC) wParam, EMUPEN_WHITE);
                return (LRESULT) hBrush[EMUBRUSH_WHITE];
            }
            if (mdcrstate == MDCRSTATE_FWD)
            {   if (mdcrfwdstate >= MDCRFWDSTATE_NUMREAD_A && mdcrfwdstate <= MDCRFWDSTATE_BLOCKREAD_B) // 1..4
                {   SetBkColor((HDC) wParam, EMUPEN_CYAN);
                    return (LRESULT) hBrush[EMUBRUSH_CYAN];
                } elif (mdcrfwdstate >= MDCRFWDSTATE_WRITE) // 5..7
                {   SetBkColor((HDC) wParam, EMUPEN_RED);
                    return (LRESULT) hBrush[EMUBRUSH_RED];
            }   }
            SetBkColor((HDC) wParam, EMUPEN_GREEN);
        return (LRESULT) hBrush[EMUBRUSH_GREEN];
        default:
        return TRUE;
        }
    case WM_DESTROY: // no need for acase
        subwin[SUBWINDOW_TAPEDECK].hwnd = NULL;
        SetWindowLong(GetDlgItem(hwnd, IDC_SKEW2), GWL_WNDPROC, (LONG) wpOldTapeEditProc);
    return FALSE;
    case WM_MOVE:
        reset_fps();
    return FALSE;
    case WM_PAINT:
        DISCARD BeginPaint(hwnd, &localps);
        update_tapedeck(TRUE);
        DISCARD EndPaint(hwnd, &localps);
    return FALSE;
    default:
    return FALSE;
    }
    return TRUE;
}

LRESULT CALLBACK TapeEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{   int temp;

    switch (msg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_TAB:
        case VK_RETURN:
            GetWindowText(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_SKEW2), skewstring, 4 + 1);
            temp = atoi(skewstring);
            if (temp < -96)
            {   temp = -96;
            } elif (temp > 96)
            {   temp = 96;
            }
            stcl_d(skewstring, temp);
            SetDlgItemText(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_SKEW2, skewstring);
            tapeskewage = (UBYTE) (128 + temp);
            SendMessage(GetDlgItem(subwin[SUBWINDOW_TAPEDECK].hwnd, IDC_SKEW), TBM_SETPOS, TRUE, temp);
            update_waveform();
        return 0; // indicate that we processed the message
    }   }

    // Pass the messages we don't process here on to the
    // original window procedure for default handling.
    return CallWindowProc(wpOldTapeEditProc, hWnd, msg, wParam, lParam);
}

MODULE BOOL CALLBACK PapertapeDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT LONG        gid;
    TRANSIENT ULONG       scancode;
    TRANSIENT PAINTSTRUCT localps;
    FAST      int         i, j,
                          mousex, mousey;
    FAST      POINT       thepoint;
    FAST      HICON       localhicon;
    FAST      RECT        localrect;
    PERSIST   RECT        rollrect[2];

    switch (Message)
    {
    case WM_INITDIALOG:
        subwin[SUBWINDOW_PAPERTAPE].hwnd = hwnd;

        setdlgtext(hwnd, IDL_POSITION         , MSG_HAIL_POSITION  , "Position"           );
        setdlgtext(hwnd, IDC_CREATEPAPERTAPE  , MSG_CREATEPAPERTAPE, "Create papertape...");
        setdlgtext(hwnd, IDC_INSERTPAPERTAPE  , MSG_INSERTPAPERTAPE, "Insert papertape...");
        setdlgtext(hwnd, IDC_PAPERTAPEPROTECT , MSG_WRITEPROTECT   , "Write protect?"     );

        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_PAPERTAPEPROTECT),
            BM_SETCHECK,
            papertapeprotect[0] ? BST_CHECKED : BST_UNCHECKED,
            0
        );

        if (machine == TWIN)
        {   setdlgtext(hwnd, IDL_POSITION2       , MSG_HAIL_POSITION  , "Position"           );
            setdlgtext(hwnd, IDC_INSERTPAPERTAPE2, MSG_INSERTPAPERTAPE, "Insert papertape...");
        } else
        {   localhicon = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon), IMAGE_ICON, 32, 32, 0);
            DISCARD SendMessage(GetDlgItem(hwnd, IDL_GLYPH), STM_SETICON, (WPARAM) localhicon, (LPARAM) 0);
        }

        DISCARD GetWindowRect(GetDlgItem(hwnd, IDC_ROLL), &localrect);
        thepoint.x           = localrect.left;
        thepoint.y           = localrect.top;
        DISCARD ScreenToClient(hwnd, &thepoint);
        rollrect[0].left     = thepoint.x;
        rollrect[0].top      = thepoint.y;

        if (machine == TWIN)
        {   DISCARD GetWindowRect(GetDlgItem(hwnd, IDC_ROLL2), &localrect);
            thepoint.x           = localrect.left;
            thepoint.y           = localrect.top;
            DISCARD ScreenToClient(hwnd, &thepoint);
            rollrect[1].left     = thepoint.x;
            rollrect[1].top      = thepoint.y;
        }

        move_subwindow(SUBWINDOW_PAPERTAPE);
    return FALSE;
    case WM_ACTIVATE:
        do_autopause(wParam, lParam);
    acase WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
        mousex = (int) LOWORD(lParam);
        mousey = (int) HIWORD(lParam);
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "X: %d, Y: %d.\n", mousex, mousey);
#endif
        if
        (   papertapemode[0] <= TAPEMODE_STOP
         && mousex >= rollrect[0].left
         && mousex <  rollrect[0].left + ROLLWIDTH
         && mousey >= rollrect[0].top
         && mousey <  rollrect[0].top  + ROLLHEIGHT
        )
        {   load_papertape(TRUE, 0);
        } elif
        (   machine == TWIN
         && papertapemode[1] <= TAPEMODE_STOP
         && mousex >= rollrect[1].left
         && mousex <  rollrect[1].left + ROLLWIDTH
         && mousey >= rollrect[1].top
         && mousey <  rollrect[1].top  + ROLLHEIGHT
        )
        {   load_papertape(TRUE, 1);
        }
    return FALSE;
    case WM_DRAWITEM:
        switch (wParam)
        {
        case  IDC_PT_RECORD:     drawitem((struct tagDRAWITEMSTRUCT*) lParam, (FLAG) (papertapemode[0] == TAPEMODE_RECORD));
        acase IDC_PT_REWIND:     drawitem((struct tagDRAWITEMSTRUCT*) lParam,         FALSE                               );
        acase IDC_PT_PLAY:       drawitem((struct tagDRAWITEMSTRUCT*) lParam, (FLAG) (papertapemode[0] == TAPEMODE_PLAY  ));
        acase IDC_PT_FFWD:       drawitem((struct tagDRAWITEMSTRUCT*) lParam,         FALSE                               );
        acase IDC_PT_STOPTAPE:   drawitem((struct tagDRAWITEMSTRUCT*) lParam, (FLAG) (papertapemode[0] == TAPEMODE_STOP  ));
        acase IDC_PT_EJECTTAPE:  drawitem((struct tagDRAWITEMSTRUCT*) lParam, (FLAG) (papertapemode[0] == TAPEMODE_NONE  ));

        acase IDC_PT_REWIND2:    drawitem((struct tagDRAWITEMSTRUCT*) lParam,         FALSE                               );
        acase IDC_PT_PLAY2:      drawitem((struct tagDRAWITEMSTRUCT*) lParam, (FLAG) (papertapemode[1] == TAPEMODE_PLAY  ));
        acase IDC_PT_FFWD2:      drawitem((struct tagDRAWITEMSTRUCT*) lParam,         FALSE                               );
        acase IDC_PT_STOPTAPE2:  drawitem((struct tagDRAWITEMSTRUCT*) lParam, (FLAG) (papertapemode[1] == TAPEMODE_STOP  ));
        acase IDC_PT_EJECTTAPE2: drawitem((struct tagDRAWITEMSTRUCT*) lParam, (FLAG) (papertapemode[1] == TAPEMODE_NONE  ));
        }
    return TRUE;
    case WM_VSCROLL:
        if (lParam == (long) GetDlgItem(hwnd, IDC_PTPOSITIONSLIDER))
        {   if (papertapemode[0] == TAPEMODE_STOP)
            {   papertapewhere[0] = SendMessage
                (   GetDlgItem(hwnd, IDC_PTPOSITIONSLIDER),
                    TBM_GETPOS,
                    0,
                    0
                ); // because LOWORD(lParam) is not always correct
                if (papertapewhere[0] > papertapelength[0]) // this can happen on Windows with an empty (zero-length) papertape
                {   papertapewhere[0] = papertapelength[0];
                }
                update_papertape(0, TRUE, FALSE);
        }   }
        elif (lParam == (long) GetDlgItem(hwnd, IDC_PTPOSITIONSLIDER2))
        {   if (papertapemode[1] == TAPEMODE_STOP)
            {   papertapewhere[1] = SendMessage
                (   GetDlgItem(hwnd, IDC_PTPOSITIONSLIDER2),
                    TBM_GETPOS,
                    0,
                    0
                ); // because LOWORD(lParam) is not always correct
                if (papertapewhere[1] > papertapelength[1]) // this can happen on Windows with an empty (zero-length) papertape
                {   papertapewhere[1] = papertapelength[1];
                }
                update_papertape(1, TRUE, FALSE);
        }   }
    acase WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_PT_REWIND:
            // assert(papertapemode[0] == TAPEMODE_STOP);
            papertapewhere[0] = 0;
            update_papertape(0, TRUE, FALSE);
        acase IDC_PT_REWIND2:
            // assert(papertapemode[1] == TAPEMODE_STOP);
            papertapewhere[1] = 0;
            update_papertape(1, TRUE, FALSE);
        acase IDC_PT_STOPTAPE:
            // assert(papertapemode[0] > TAPEMODE_STOP);
            papertape_stop(0);
        acase IDC_PT_STOPTAPE2:
            // assert(papertapemode[1] > TAPEMODE_STOP);
            papertape_stop(1);
        acase IDC_PT_RECORD:
            papertape_record(0);
        acase IDC_PT_PLAY:
            papertape_play(0);
        acase IDC_PT_PLAY2:
            papertape_play(1);
        acase IDC_PT_FFWD:
            // assert(papertapemode[0] == TAPEMODE_STOP);
            papertapewhere[0] = papertapelength[0];
            update_papertape(0, TRUE, FALSE);
        acase IDC_PT_FFWD2:
            // assert(papertapemode[1] == TAPEMODE_STOP);
            papertapewhere[1] = papertapelength[1];
            update_papertape(1, TRUE, FALSE);
        acase IDC_PT_EJECTTAPE:
            // assert(papertapemode[0] != TAPEMODE_NONE);
            papertape_eject(0);
        acase IDC_PT_EJECTTAPE2:
            // assert(papertapemode[1] != TAPEMODE_NONE);
            papertape_eject(1);
        acase IDC_CREATEPAPERTAPE:
            // assert(papertapemode[0] == TAPEMODE_NONE);
            create_papertape(0);
        acase IDC_INSERTPAPERTAPE:
            // assert(papertapemode[0] == TAPEMODE_NONE);
            load_papertape(TRUE, 0);
        acase IDC_INSERTPAPERTAPE2:
            // assert(papertapemode[1] == TAPEMODE_NONE);
            load_papertape(TRUE, 1);
        acase IDC_PAPERTAPEPROTECT:
            papertapeprotect[0] = (SendMessage(GetDlgItem(hwnd, IDC_PAPERTAPEPROTECT), BM_GETCHECK, 0, 0) == BST_CHECKED) ? TRUE : FALSE;
            update_roll(0);
        }
    acase WM_CLOSE:
        clearkybd();
        DestroyWindow(hwnd);
        subwin[SUBWINDOW_PAPERTAPE].hwnd = NULL;
        updatemenu(MENUITEM_PAPERTAPE);
    return TRUE;
    case WM_CTLCOLORSTATIC:
        gid = GetWindowLong((HWND) lParam, GWL_ID);
        switch (gid)
        {
        case IDC_PAPERTAPEBYTE:
            switch (papertapemode[0])
            {
            case TAPEMODE_PLAY:       SetBkColor((HDC) wParam, EMUPEN_CYAN  ); return (LRESULT) hBrush[EMUBRUSH_CYAN];
            case TAPEMODE_RECORD: if (papertapeprotect[0])
                                  {   SetBkColor((HDC) wParam, EMUPEN_YELLOW); return (LRESULT) hBrush[EMUBRUSH_YELLOW];
                                  } else
                                  {   SetBkColor((HDC) wParam, EMUPEN_RED   ); return (LRESULT) hBrush[EMUBRUSH_RED];
                                  }
            default:                  SetBkColor((HDC) wParam, EMUPEN_GREEN ); return (LRESULT) hBrush[EMUBRUSH_GREEN];
            }
        case IDC_PAPERTAPEBYTE2: // no need for acase
            switch (papertapemode[1])
            {
            case TAPEMODE_PLAY:       SetBkColor((HDC) wParam, EMUPEN_CYAN  ); return (LRESULT) hBrush[EMUBRUSH_CYAN];
            case TAPEMODE_RECORD: if (papertapeprotect[1])
                                  {   SetBkColor((HDC) wParam, EMUPEN_YELLOW); return (LRESULT) hBrush[EMUBRUSH_YELLOW];
                                  } else
                                  {   SetBkColor((HDC) wParam, EMUPEN_RED   ); return (LRESULT) hBrush[EMUBRUSH_RED];
                                  }
            default:                  SetBkColor((HDC) wParam, EMUPEN_GREEN ); return (LRESULT) hBrush[EMUBRUSH_GREEN];
            }
        default:
        return TRUE;
        }
    case WM_DESTROY: // no need for acase
        subwin[SUBWINDOW_PAPERTAPE].hwnd = NULL;
    return FALSE;
    case WM_MOVE:
        reset_fps();
    return FALSE;
    case WM_PAINT:
        DISCARD BeginPaint(hwnd, &localps);
        update_papertape(0, TRUE, FALSE);
        if (machine == TWIN)
        {   update_papertape(1, TRUE, FALSE);
        }
        DISCARD EndPaint(hwnd, &localps);
    return FALSE;
    case WM_KEYDOWN:
        scancode = (lParam & 33488896) >> 16;
        handle_keydown(scancode);
    return FALSE;
    case WM_KEYUP:
        scancode = (lParam & 33488896) >> 16;
        handle_keyup(scancode);
    return FALSE;
    default:
    return FALSE;
    }
    return TRUE;
}

MODULE BOOL CALLBACK IndustrialDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT PAINTSTRUCT localps;
    PERSIST   HBRUSH      LowHornBrush,
                          HighHornBrush,
                          SilentBrush;
    FAST      int         gid,
                          i, j,
                          mousex, mousey,
                          temp;
    FAST      POINT       thepoint;
    FAST      RECT        therect;

    switch (Message)
    {
    case WM_INITDIALOG:
        subwin[SUBWINDOW_INDUSTRIAL].hwnd = hwnd;

        sprintf
        (   gtempstring,
            "%s: ",
            LLL(MSG_HAIL_INDUSTRIAL, "Industrial/Scientific Peripherals")
        );
        switch (pipbug_periph)
        {
        case  PERIPH_PRINTER:      strcat(gtempstring, LLL(MSG_EAPRINTER    , "EA printer"             ));
        acase PERIPH_LINEARISATIE: strcat(gtempstring, LLL(MSG_LINEARIZATION, "Linearization"          ));
        acase PERIPH_ROBOT:        strcat(gtempstring,                        "Tasman Turtle"           );
        acase PERIPH_FURNACE:      strcat(gtempstring, LLL(MSG_WFC          , "Wind Furnace Controller"));
        acase PERIPH_MAGNETOMETER: strcat(gtempstring, LLL(MSG_VM           , "Vector Magnetometer"    ));
        }
        SetWindowText(hwnd, gtempstring);

        setdlgtext(    hwnd, IDL_PERIPHERAL        , MSG_PERIPHERAL        , "Peripheral:");
        SendMessage(GetDlgItem(hwnd, IDC_PERIPHERAL), CB_ADDSTRING, (WPARAM) 0, (LPARAM) LLL(MSG_EAPRINTER    , "EA printer"             ));
        SendMessage(GetDlgItem(hwnd, IDC_PERIPHERAL), CB_ADDSTRING, (WPARAM) 0, (LPARAM) LLL(MSG_LINEARIZATION, "Linearization"          ));
        SendMessage(GetDlgItem(hwnd, IDC_PERIPHERAL), CB_ADDSTRING, (WPARAM) 0, (LPARAM)                        "Tasman Turtle"           );
        SendMessage(GetDlgItem(hwnd, IDC_PERIPHERAL), CB_ADDSTRING, (WPARAM) 0, (LPARAM) LLL(MSG_VM,            "Vector Magnetometer"    ));           
        SendMessage(GetDlgItem(hwnd, IDC_PERIPHERAL), CB_ADDSTRING, (WPARAM) 0, (LPARAM) LLL(MSG_WFC          , "Wind Furnace Controller"));

        switch (pipbug_periph)
        {
        case PERIPH_FURNACE:
            setdlgtext(hwnd, IDL_PITCHSPEED        , MSG_PITCHSPEED        , "Blade pitch motor speed:");
            setdlgtext(hwnd, IDL_YAWSPEED          , MSG_YAWSPEED          , "Hub yaw motor speed:");
            setdlgtext(hwnd, IDL_FIELDCURRENT      , MSG_FIELDCURRENT      , "Generator field current:");
            setdlgtext(hwnd, IDL_ROTORSPEED        , MSG_ROTORSPEED        , "Rotor speed:");
            setdlgtext(hwnd, IDL_PITCHANGLE        , MSG_PITCHANGLE        , "Blade pitch angle:");
            setdlgtext(hwnd, IDL_YAWSENSORDIRECTION, MSG_YAWSENSORDIRECTION, "Hub yaw direction:");
            setdlgtext(hwnd, IDL_WINDSPEED         , MSG_WINDSPEED         , "Wind speed:");
            setdlgtext(hwnd, IDL_WINDDIRECTION     , MSG_WINDDIRECTION     , "Wind direction:");
            setdlgtext(hwnd, IDL_MECHPOWER         , MSG_MECHPOWER         , "Mechanical power:");
            setdlgtext(hwnd, IDL_ELECPOWER         , MSG_ELECPOWER         , "Electrical power:");
            setdlgtext(hwnd, IDL_FIELDCURRENTUNITS , MSG_MV                , "mV");
            setdlgtext(hwnd, IDL_ROTORSPEEDUNITS   , MSG_RPM               , "rpm");
            setdlgtext(hwnd, IDL_PITCHANGLEUNITS   , MSG_DEGREES           , "degrees");
            setdlgtext(hwnd, IDL_YAWSENSORDIRUNITS , MSG_DEGREES           , "degrees");
            setdlgtext(hwnd, IDL_WINDSPEEDUNITS    , MSG_MPH               , "mph");
            setdlgtext(hwnd, IDL_WINDDIRECTIONUNITS, MSG_DEGREES           , "degrees");
            setdlgtext(hwnd, IDL_MECHPOWERUNITS    , MSG_KW                , "kW");
            setdlgtext(hwnd, IDL_ELECPOWERUNITS    , MSG_KW                , "kW");
            setdlgtext(hwnd, IDC_INDUSTRIAL_RESET  , MSG_RESETTODEFAULTS3  , "Reset to defaults");

            SendMessage(GetDlgItem(hwnd, IDC_PITCHSPEED       ), TBM_SETRANGE  , FALSE, MAKELONG(0, 127));
            SendMessage(GetDlgItem(hwnd, IDC_PITCHSPEED       ), TBM_SETTICFREQ,     8,                0);
            SendMessage(GetDlgItem(hwnd, IDC_YAWSPEED         ), TBM_SETRANGE  , FALSE, MAKELONG(0, 127));
            SendMessage(GetDlgItem(hwnd, IDC_YAWSPEED         ), TBM_SETTICFREQ,     8,                0);
            SendMessage(GetDlgItem(hwnd, IDC_FIELDCURRENT     ), TBM_SETRANGE  , FALSE, MAKELONG(0, 255));
            SendMessage(GetDlgItem(hwnd, IDC_FIELDCURRENT     ), TBM_SETTICFREQ,    16,                0);
            SendMessage(GetDlgItem(hwnd, IDC_PITCHANGLE       ), TBM_SETRANGE  , FALSE, MAKELONG(0,  90));
            SendMessage(GetDlgItem(hwnd, IDC_PITCHANGLE       ), TBM_SETTICFREQ,     6,                0);
            SendMessage(GetDlgItem(hwnd, IDC_YAWSENSORDIR     ), TBM_SETRANGE  , FALSE, MAKELONG(0, 255));
            SendMessage(GetDlgItem(hwnd, IDC_YAWSENSORDIR     ), TBM_SETTICFREQ,    16,                0);
            SendMessage(GetDlgItem(hwnd, IDC_WINDSPEED        ), TBM_SETRANGE  , FALSE, MAKELONG(0, 255));
            SendMessage(GetDlgItem(hwnd, IDC_WINDSPEED        ), TBM_SETTICFREQ,    16,                0);
            SendMessage(GetDlgItem(hwnd, IDC_WINDDIRECTION    ), TBM_SETRANGE  , FALSE, MAKELONG(0, 255));
            SendMessage(GetDlgItem(hwnd, IDC_WINDDIRECTION    ), TBM_SETTICFREQ,    16,                0);

            SendMessage(GetDlgItem(hwnd, IDC_PITCHDIRECTION   ), CB_ADDSTRING  , (WPARAM) 0, (LPARAM) LLL(MSG_UNFEATHERING   , "Unfeathering" ));
            SendMessage(GetDlgItem(hwnd, IDC_PITCHDIRECTION   ), CB_ADDSTRING  , (WPARAM) 0, (LPARAM) LLL(MSG_FEATHERING     , "Feathering"   ));
            SendMessage(GetDlgItem(hwnd, IDC_YAWMOTORDIR      ), CB_ADDSTRING  , (WPARAM) 0, (LPARAM) LLL(MSG_ANTICLOCKWISE  , "Anticlockwise"));
            SendMessage(GetDlgItem(hwnd, IDC_YAWMOTORDIR      ), CB_ADDSTRING  , (WPARAM) 0, (LPARAM) LLL(MSG_CLOCKWISE      , "Clockwise"    ));
            SendMessage(GetDlgItem(hwnd, IDC_YAWMOTORMODE     ), CB_ADDSTRING  , (WPARAM) 0, (LPARAM) LLL(MSG_MOTORMODE_DAMP , "Damp"         ));
            SendMessage(GetDlgItem(hwnd, IDC_YAWMOTORMODE     ), CB_ADDSTRING  , (WPARAM) 0, (LPARAM) LLL(MSG_MOTORMODE_DRIVE, "Drive"        ));
        acase PERIPH_LINEARISATIE:
            setdlgtext(hwnd, IDL_LINEARXUNITS, MSG_MV, "mV");
            setdlgtext(hwnd, IDL_LINEARYUNITS, MSG_MV, "mV");
            setdlgtext(hwnd, IDL_LINEARUUNITS, MSG_MV, "mV");
            setdlgtext(hwnd, IDL_LINEARVUNITS, MSG_MV, "mV");
            setdlgtext(hwnd, IDC_INDUSTRIAL_RESET, MSG_RESETTODEFAULTS3, "Reset to defaults");

            SendMessage(GetDlgItem(hwnd, IDC_LINEARX          ), TBM_SETRANGE  , FALSE, MAKELONG(0, 4095));
            SendMessage(GetDlgItem(hwnd, IDC_LINEARX          ), TBM_SETTICFREQ,   256,                 0);
            SendMessage(GetDlgItem(hwnd, IDC_LINEARY          ), TBM_SETRANGE  , FALSE, MAKELONG(0, 4095));
            SendMessage(GetDlgItem(hwnd, IDC_LINEARY          ), TBM_SETTICFREQ,   256,                 0);
            SendMessage(GetDlgItem(hwnd, IDC_LINEARU          ), TBM_SETRANGE  , FALSE, MAKELONG(0, 4095));
            SendMessage(GetDlgItem(hwnd, IDC_LINEARU          ), TBM_SETTICFREQ,   256,                 0);
            SendMessage(GetDlgItem(hwnd, IDC_LINEARV          ), TBM_SETRANGE  , FALSE, MAKELONG(0, 4095));
            SendMessage(GetDlgItem(hwnd, IDC_LINEARV          ), TBM_SETTICFREQ,   256,                 0);
        acase PERIPH_ROBOT:
            setdlgtext(hwnd, IDL_WHEELMOTORS     , MSG_WHEELMOTORS, "Wheel Motors");
            setdlgtext(hwnd, IDL_MOTION          , MSG_ROBOTMOTION, "Robot Motion");
            setdlgtext(hwnd, IDC_LAMPSON         , MSG_LAMPSLIT   , "Lamps lit?");
            setdlgtext(hwnd, IDC_PENDOWN         , MSG_PENDOWN    , "Pen down?");
            setdlgtext(hwnd, IDL_HORNPITCH       , MSG_HORNPITCH  , "Horn pitch:");
            setdlgtext(hwnd, IDC_HONK            , MSG_HONK       , "Honk");

            setdlgtext(hwnd, IDC_MOTION_FORWARDS , MSG_MOTION_0, "Forwards");
            setdlgtext(hwnd, IDC_MOTION_PIVOTLTLT, MSG_MOTION_1, "Pivot anticlockwise (left) about left wheel");
            setdlgtext(hwnd, IDC_MOTION_PIVOTRTRT, MSG_MOTION_2, "Pivot clockwise (right) about right wheel");
            setdlgtext(hwnd, IDC_MOTION_ROTATELT , MSG_MOTION_3, "Rotate anticlockwise (left) about centre");
            setdlgtext(hwnd, IDC_MOTION_STOPPED  , MSG_MOTION_4, "Stopped");
            setdlgtext(hwnd, IDC_MOTION_ROTATERT , MSG_MOTION_5, "Rotate clockwise (right) about centre");
            setdlgtext(hwnd, IDC_MOTION_PIVOTLTRT, MSG_MOTION_6, "Pivot anticlockwise (left) about right wheel");
            setdlgtext(hwnd, IDC_MOTION_PIVOTRTLT, MSG_MOTION_7, "Pivot clockwise (right) about left wheel");
            setdlgtext(hwnd, IDC_MOTION_BACKWARDS, MSG_MOTION_8, "Backwards");

            SendMessage(GetDlgItem(hwnd, IDC_ROBOT_LEFT       ), TBM_SETRANGE, FALSE, MAKELONG(0, 2));
            SendMessage(GetDlgItem(hwnd, IDC_ROBOT_LEFT       ), TBM_SETPOS,   TRUE,  robotwheel[0]);
            SendMessage(GetDlgItem(hwnd, IDC_ROBOT_RIGHT      ), TBM_SETRANGE, FALSE, MAKELONG(0, 2));
            SendMessage(GetDlgItem(hwnd, IDC_ROBOT_RIGHT      ), TBM_SETPOS,   TRUE,  robotwheel[1]);

            DISCARD CheckRadioButton
            (   subwin[SUBWINDOW_PRINTER].hwnd,
                IDC_MOTION_FORWARDS,
                IDC_MOTION_BACKWARDS,
                IDC_MOTION_FORWARDS + motion // motion is 0..8
            );

            SendMessage(GetDlgItem(hwnd, IDC_LAMPSON          ), BM_SETCHECK   , lampson   ? BST_CHECKED : BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwnd, IDC_PENDOWN          ), BM_SETCHECK   , pendown   ? BST_CHECKED : BST_UNCHECKED, 0);

            SendMessage(GetDlgItem(hwnd, IDC_HORNPITCH), CB_ADDSTRING, (WPARAM) 0, (LPARAM) LLL(MSG_LOW , "Low" ));
            SendMessage(GetDlgItem(hwnd, IDC_HORNPITCH), CB_ADDSTRING, (WPARAM) 0, (LPARAM) LLL(MSG_HIGH, "High"));
            SendMessage(GetDlgItem(hwnd, IDC_HORNPITCH), CB_SETCURSEL, (WPARAM) hornpitch, (LPARAM) 0);

            LowHornBrush  = CreateSolidBrush(EMUPEN_RED);
            HighHornBrush = CreateSolidBrush(EMUPEN_YELLOW);
            SilentBrush   = CreateSolidBrush(EMUPEN_GREEN);
        acase PERIPH_MAGNETOMETER:
            setdlgtext(            hwnd, IDL_MAGNETICX , MSG_MAGNETICX , "Plane Hx:");
            setdlgtext(            hwnd, IDL_MAGNETICY , MSG_MAGNETICY , "Plane Hy:");
            setdlgtext(            hwnd, IDL_MAGNETICZ , MSG_MAGNETICZ , "Plane Hz:");
            setdlgtext(            hwnd, IDL_PLANEPITCH, MSG_PLANEPITCH, "Plane pitch:");
            setdlgtext(            hwnd, IDL_PLANEROLL , MSG_PLANEROLL , "Plane roll:");
            setdlgtext(            hwnd, IDL_EARTHX    , MSG_EARTHX    , "Earth Hx:");
            setdlgtext(            hwnd, IDL_EARTHY    , MSG_EARTHY    , "Earth Hy:");
            setdlgtext(            hwnd, IDL_EARTHZ    , MSG_EARTHZ    , "Earth Hz:");
            setdlgtext(            hwnd, IDL_PLANEYAW  , MSG_PLANEYAW  , "Plane yaw:");
            setdlgtext(            hwnd, IDL_LATITUDE1 , MSG_LATITUDE1 , "Latitude (based on inclination):");
            setdlgtext(            hwnd, IDL_LATITUDE2 , MSG_LATITUDE2 , "Latitude (based on intensity):");
#ifdef USEMV
            // should also really have "0" instead of "-600" and "5000" instead of "+600"
            setdlgtext(hwnd, IDL_MAGNETICXUNITS    , MSG_MV     , "mV");
            setdlgtext(hwnd, IDL_MAGNETICYUNITS    , MSG_MV     , "mV");
            setdlgtext(hwnd, IDL_MAGNETICZUNITS    , MSG_MV     , "mV");
#else
            setdlgtext(hwnd, IDL_MAGNETICXUNITS    , MSG_MG     , "mG");
            setdlgtext(hwnd, IDL_MAGNETICYUNITS    , MSG_MG     , "mG");
            setdlgtext(hwnd, IDL_MAGNETICZUNITS    , MSG_MG     , "mG");
#endif
            setdlgtext(hwnd, IDL_PLANEPITCHUNITS   , MSG_DEGREES, "degrees");
            setdlgtext(hwnd, IDL_PLANEROLLUNITS    , MSG_DEGREES, "degrees");
            setdlgtext(hwnd, IDL_EARTHXUNITS       , MSG_MG     , "mG");
            setdlgtext(hwnd, IDL_EARTHYUNITS       , MSG_MG     , "mG");
            setdlgtext(hwnd, IDL_EARTHZUNITS       , MSG_MG     , "mG");
            setdlgtext(hwnd, IDL_PLANEYAWUNITS     , MSG_DEGREES, "degrees");
            setdlgtext(hwnd, IDL_LATITUDE1UNITS    , MSG_DEGREES, "degrees");
            setdlgtext(hwnd, IDL_LATITUDE1UNITS    , MSG_DEGREES, "degrees");
            setdlgtext(hwnd, IDC_DECLINATE         , MSG_DECLINATE, "Declinate?");
            setdlgtext(hwnd, IDC_INDUSTRIAL_RESET  , MSG_RESETTODEFAULTS3, "Reset to defaults");

            SendMessage(GetDlgItem(hwnd, IDC_MAGNETICX        ), TBM_SETRANGE  , FALSE, MAKELONG(0, 4095));
            SendMessage(GetDlgItem(hwnd, IDC_MAGNETICX        ), TBM_SETTICFREQ,   256,                 0);
            SendMessage(GetDlgItem(hwnd, IDC_MAGNETICY        ), TBM_SETRANGE  , FALSE, MAKELONG(0, 4095));
            SendMessage(GetDlgItem(hwnd, IDC_MAGNETICY        ), TBM_SETTICFREQ,   256,                 0);
            SendMessage(GetDlgItem(hwnd, IDC_MAGNETICZ        ), TBM_SETRANGE  , FALSE, MAKELONG(0, 4095));
            SendMessage(GetDlgItem(hwnd, IDC_MAGNETICZ        ), TBM_SETTICFREQ,   256,                 0);
            SendMessage(GetDlgItem(hwnd, IDC_PLANEPITCH       ), TBM_SETRANGE  , FALSE, MAKELONG(2048 - PLANEPITCH_MAX, 2048 + PLANEPITCH_MAX));
            SendMessage(GetDlgItem(hwnd, IDC_PLANEPITCH       ), TBM_SETTICFREQ,     6,                 0);
            SendMessage(GetDlgItem(hwnd, IDC_PLANEROLL        ), TBM_SETRANGE  , FALSE, MAKELONG(2048 - PLANEROLL_MAX , 2048 + PLANEROLL_MAX ));
            SendMessage(GetDlgItem(hwnd, IDC_PLANEROLL        ), TBM_SETTICFREQ,    12,                 0);
            SendMessage(GetDlgItem(hwnd, IDC_DECLINATE        ), BM_SETCHECK   , declinate ? BST_CHECKED : BST_UNCHECKED, 0);
        acase PERIPH_PRINTER:
            setdlgtext(            hwnd, IDC_TOGGLESUBWINDOW   , MSG_TOGGLESUBWINDOW   , "Open/close subwindow...");
        }

        SendMessage(    GetDlgItem(hwnd, IDC_PERIPHERAL       ), CB_SETCURSEL  , (WPARAM) pipbug_periph, (LPARAM) 0);

        move_subwindow(SUBWINDOW_INDUSTRIAL);
        SetFocus(hwnd);
    return FALSE;
    case WM_ACTIVATE:
        do_autopause(wParam, lParam);
    acase WM_HSCROLL:
        if (lParam == (long) GetDlgItem(hwnd, IDC_PITCHSPEED))
        {   ioport[0].contents &= 0x80;
            ioport[0].contents |= (UBYTE) SendMessage
            (   GetDlgItem(hwnd, IDC_PITCHSPEED),
                TBM_GETPOS,
                0,
                0
            ); // because LOWORD(lParam) is not always correct
            update_industrial(FALSE);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_YAWSPEED))
        {   ioport[1].contents &= 0x80;
            ioport[1].contents |= (UBYTE) SendMessage
            (   GetDlgItem(hwnd, IDC_YAWSPEED),
                TBM_GETPOS,
                0,
                0
            ); // because LOWORD(lParam) is not always correct
            update_industrial(FALSE);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_FIELDCURRENT))
        {   ioport[2].contents = (UBYTE) SendMessage
            (   GetDlgItem(hwnd, IDC_FIELDCURRENT),
                TBM_GETPOS,
                0,
                0
            ); // because LOWORD(lParam) is not always correct
            update_industrial(FALSE);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_PITCHANGLE))
        {   pitchangle = (UBYTE) SendMessage(GetDlgItem(hwnd, IDC_PITCHANGLE), TBM_GETPOS, 0, 0); // because LOWORD(lParam) is not always correct
            update_industrial(FALSE);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_YAWSENSORDIR))
        {   yawsensordir = (UBYTE) SendMessage(GetDlgItem(hwnd, IDC_YAWSENSORDIR), TBM_GETPOS, 0, 0); // because LOWORD(lParam) is not always correct
            update_industrial(FALSE);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_WINDSPEED))
        {   windspeed = (UBYTE) SendMessage(GetDlgItem(hwnd, IDC_WINDSPEED), TBM_GETPOS, 0, 0); // because LOWORD(lParam) is not always correct
            update_industrial(FALSE);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_WINDDIRECTION))
        {   winddirection = (UBYTE) SendMessage(GetDlgItem(hwnd, IDC_WINDDIRECTION), TBM_GETPOS, 0, 0); // because LOWORD(lParam) is not always correct
            update_industrial(FALSE);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_LINEARX))
        {   linearx = (UWORD) SendMessage(GetDlgItem(hwnd, IDC_LINEARX), TBM_GETPOS, 0, 0); // because LOWORD(lParam) is not always correct
            update_industrial(FALSE);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_LINEARY))
        {   lineary = (UWORD) SendMessage(GetDlgItem(hwnd, IDC_LINEARY), TBM_GETPOS, 0, 0); // because LOWORD(lParam) is not always correct
            update_industrial(FALSE);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_LINEARU))
        {   linearu = (UWORD) SendMessage(GetDlgItem(hwnd, IDC_LINEARU), TBM_GETPOS, 0, 0); // because LOWORD(lParam) is not always correct
            update_industrial(FALSE);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_LINEARV))
        {   linearv = (UWORD) SendMessage(GetDlgItem(hwnd, IDC_LINEARV), TBM_GETPOS, 0, 0); // because LOWORD(lParam) is not always correct
            update_industrial(FALSE);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_MAGNETICX))
        {   magneticx = (UWORD) SendMessage(GetDlgItem(hwnd, IDC_MAGNETICX), TBM_GETPOS, 0, 0); // because LOWORD(lParam) is not always correct
            update_industrial(FALSE);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_MAGNETICY))
        {   magneticy = (UWORD) SendMessage(GetDlgItem(hwnd, IDC_MAGNETICY), TBM_GETPOS, 0, 0); // because LOWORD(lParam) is not always correct
            update_industrial(FALSE);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_MAGNETICZ))
        {   magneticz = (UWORD) SendMessage(GetDlgItem(hwnd, IDC_MAGNETICZ), TBM_GETPOS, 0, 0); // because LOWORD(lParam) is not always correct
            update_industrial(FALSE);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_PLANEPITCH))
        {   planepitch = (UWORD) SendMessage(GetDlgItem(hwnd, IDC_PLANEPITCH), TBM_GETPOS, 0, 0); // because LOWORD(lParam) is not always correct
            update_industrial(FALSE);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_PLANEROLL))
        {   planeroll = (UWORD) SendMessage(GetDlgItem(hwnd, IDC_PLANEROLL), TBM_GETPOS, 0, 0); // because LOWORD(lParam) is not always correct
            update_industrial(FALSE);
        }
    return TRUE;
    case WM_VSCROLL:
        if (lParam == (long) GetDlgItem(hwnd, IDC_ROBOT_LEFT))
        {   robotwheel[0] = SendMessage(GetDlgItem(hwnd, IDC_ROBOT_LEFT), TBM_GETPOS, 0, 0);
            set_motion();
            ioport[0].contents &= ~2;
            if (robotwheel[0] == 0) // forward
            {   ioport[0].contents |= 2; // clockwise
            }
            userrobot = TRUE;
            update_industrial(FALSE);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_ROBOT_RIGHT))
        {   robotwheel[1] = SendMessage(GetDlgItem(hwnd, IDC_ROBOT_RIGHT), TBM_GETPOS, 0, 0);
            set_motion();
            ioport[0].contents &= ~8;
            if (robotwheel[1] == 2) // backward
            {   ioport[0].contents |= 2; // clockwise
            }
            userrobot = TRUE;
            update_industrial(FALSE);
        }
    return TRUE;
    case WM_COMMAND:
        gid = (int) LOWORD(wParam);
        switch (gid)
        {
        // Multiple-------------------------------------------------------

        case IDC_PERIPHERAL:
            temp = SendMessage(GetDlgItem(hwnd, IDC_PERIPHERAL), CB_GETCURSEL, 0, 0);
            if (temp != pipbug_periph)
            {   pipbug_periph = temp;
                close_subwindow(SUBWINDOW_INDUSTRIAL);
                open_industrial();
            }
        acase IDC_INDUSTRIAL_RESET:
            industrial_reset();

        // Printer--------------------------------------------------------

        acase IDC_TOGGLESUBWINDOW:
            if (subwin[SUBWINDOW_PRINTER].hwnd)
            {   close_subwindow(SUBWINDOW_PRINTER);
            } else
            {   tools_printer();
            }

        // Vector Magnetometer--------------------------------------------

        acase IDC_PITCHDIRECTION:
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {   if (SendMessage(GetDlgItem(hwnd, IDC_PITCHDIRECTION), CB_GETCURSEL, 0, 0) == 0)
                {   ioport[0].contents &= 0x7F; // anticlockwise
                } else
                {   ioport[0].contents |= 0x80; // clockwise
                }
                // update_industrial(FALSE); is not needed
            }
        acase IDC_YAWMOTORDIR:
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {   if (SendMessage(GetDlgItem(hwnd, IDC_YAWMOTORDIR), CB_GETCURSEL, 0, 0) == 0)
                {   ioport[1].contents &= 0x7F; // anticlockwise
                } else
                {   ioport[1].contents |= 0x80; // clockwise
                }
                // update_industrial(FALSE); is not needed
            }
        acase IDC_YAWMOTORMODE:
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {   if (SendMessage(GetDlgItem(hwnd, IDC_YAWMOTORMODE), CB_GETCURSEL, 0, 0) == 0)
                {   ioport[4].contents &= 0xFE; // damp
                } else
                {   ioport[4].contents |= 0x01; // drive
                }
                // update_industrial(FALSE); is not needed
            }
        acase IDC_DECLINATE:
            declinate = (SendMessage(GetDlgItem(hwnd, IDC_DECLINATE), BM_GETCHECK, 0, 0) == BST_CHECKED) ? TRUE : FALSE;
            update_industrial(TRUE);

        // Robot----------------------------------------------------------

        acase IDC_LAMPSON:
            lampson = (SendMessage(GetDlgItem(hwnd, IDC_LAMPSON), BM_GETCHECK, 0, 0) == BST_CHECKED) ? TRUE : FALSE;
            if (lampson) ioport[0].contents |= 0x10; else ioport[0].contents &= ~0x10;
            userrobot = TRUE;
            update_industrial(FALSE);
        acase IDC_PENDOWN:
            pendown = (SendMessage(GetDlgItem(hwnd, IDC_PENDOWN), BM_GETCHECK, 0, 0) == BST_CHECKED) ? TRUE : FALSE;
            if (pendown) ioport[0].contents |= 0x20; else ioport[0].contents &= ~0x20;
            userrobot = TRUE;
            // update_industrial(); is not needed
        acase IDC_HORNPITCH:
            hornpitch = SendMessage(GetDlgItem(hwnd, IDC_HORNPITCH), CB_GETCURSEL, 0, 0);
            if (hornpitch) ioport[0].contents |= 0x80; else ioport[0].contents &= ~0x80;
            userrobot = TRUE;
            playsound(FALSE);
        acase IDC_MOTION_FORWARDS:
            motion = 0;
            set_wheels();
        acase IDC_MOTION_PIVOTLTLT:
            motion = 1;
            set_wheels();
        acase IDC_MOTION_PIVOTRTRT:
            motion = 2;
            set_wheels();
        acase IDC_MOTION_ROTATELT:
            motion = 3;
            set_wheels();
        acase IDC_MOTION_STOPPED:
            motion = 4;
            set_wheels();
        acase IDC_MOTION_ROTATERT:
            motion = 5;
            set_wheels();
        acase IDC_MOTION_PIVOTLTRT:
            motion = 6;
            set_wheels();
        acase IDC_MOTION_PIVOTRTLT:
            motion = 7;
            set_wheels();
        acase IDC_MOTION_BACKWARDS:
            motion = 8;
            set_wheels();
        }
    acase WM_CLOSE:
        clearkybd();
        DestroyWindow(hwnd);
        subwin[SUBWINDOW_INDUSTRIAL].hwnd = NULL;
        updatemenu(MENUITEM_INDUSTRIAL);
    return TRUE;
    case WM_DESTROY:
        subwin[SUBWINDOW_INDUSTRIAL].hwnd = NULL;
        if (pipbug_periph == PERIPH_ROBOT)
        {   DeleteObject(LowHornBrush);
            DeleteObject(HighHornBrush);
            DeleteObject(SilentBrush);
        }
    return FALSE;
    case WM_MOVE:
        reset_fps();
    return FALSE;
    case WM_PAINT:
        BeginPaint(hwnd, &localps);
        update_industrial(TRUE);
        DISCARD EndPaint(hwnd, &localps);
    return FALSE; // important!
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        thepoint.x = LOWORD(lParam);
        thepoint.y = HIWORD(lParam);
        GetWindowRect(GetDlgItem(hwnd, IDC_HONK), &therect);
        ScreenToClient(hwnd, (POINT*) &therect.left);
        ScreenToClient(hwnd, (POINT*) &therect.right);
        if (PtInRect(&therect, thepoint))
        {   horn = TRUE;
            ioport[0].contents |= 0x40;
            userrobot = TRUE;
            SetCapture(hwnd);
            InvalidateRect(GetDlgItem(hwnd, IDC_HONK), NULL, TRUE);
            playsound(FALSE);
        }
    return TRUE;
    case WM_LBUTTONUP:
        if (horn)
        {   horn = FALSE;
            ioport[0].contents &= ~0x40;
            userrobot = TRUE;
            ReleaseCapture();
            InvalidateRect(GetDlgItem(hwnd, IDC_HONK), NULL, TRUE);
            playsound(FALSE);
        }
    return TRUE;
    case WM_CTLCOLORSTATIC:
        if ((HWND) lParam == GetDlgItem(hwnd, IDC_HONK))
        {   if (horn)
            {   if (hornpitch == 1)
                {   SetBkColor((HDC) wParam, EMUPEN_YELLOW);
                    return (INT_PTR) HighHornBrush;
                } else
                {   // assert(hornpitch == 0);
                    SetBkColor((HDC) wParam, EMUPEN_RED);
                    return (INT_PTR) LowHornBrush;
            }   }
            else
            {   SetBkColor((HDC) wParam, EMUPEN_GREEN);
                return (INT_PTR) SilentBrush;
        }   }
    adefault:
    return FALSE;
    }
    return TRUE;
}

MODULE BOOL CALLBACK SI50DIPsDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   int i;

    switch (Message)
    {
    case WM_ACTIVATE:
        do_autopause(wParam, lParam);
    acase WM_CLOSE:
        DestroyWindow(hwnd);
        subwin[SUBWINDOW_DIPS].hwnd = NULL;
        updatemenu(MENUITEM_DIPSWITCHES);
    acase WM_INITDIALOG:
        subwin[SUBWINDOW_DIPS].hwnd = hwnd;

        DISCARD SetWindowText(hwnd, LLL(MSG_HAIL_S_DIPS, "Signetics Instructor 50 DIP Switches"));
        setdlgtext(hwnd, IDL_PARALLELIO,           MSG_PARALLELIO,    "Parallel I/O"          );
        setdlgtext(hwnd, IDC_PARALLEL_MEMMAPPED,   MSG_MEMORYIO,      "Memory $0FFF"          );
        setdlgtext(hwnd, IDC_PARALLEL_EXTENDED,    MSG_EXTENDEDIO,    "Extended I/O Port $07" );
        setdlgtext(hwnd, IDC_PARALLEL_NONEXTENDED, MSG_NONEXTENDEDIO, "Non-Extended Data Port");
        setdlgtext(hwnd, IDL_INTERRUPTS,           MSG_INTERRUPTS,    "Interrupts"            );
        setdlgtext(hwnd, IDC_INTERRUPTS_DIRECT,    MSG_DIRECT,        "Direct"                );
        setdlgtext(hwnd, IDC_INTERRUPTS_INDIRECT,  MSG_INDIRECT,      "Indirect"              );
        setdlgtext(hwnd, IDL_INTSEL,               MSG_INTERRUPTSEL,  "Interrupt Selector"    );
        setdlgtext(hwnd, IDC_INTSELECTOR_ACLINE,   MSG_ACLINE,        "A.C. Line"             );
        setdlgtext(hwnd, IDC_INTSELECTOR_KYBD,     MSG_KEYBOARD,      "Keyboard"              );

        si50_updatedips(TRUE);

        move_subwindow(SUBWINDOW_DIPS);
    return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            DestroyWindow(hwnd);
            subwin[SUBWINDOW_DIPS].hwnd = NULL;
            updatemenu(MENUITEM_DIPSWITCHES);
        acase IDC_INTERRUPTS_DIRECT:    si50_id = INTDIR_DIRECT;
        acase IDC_INTERRUPTS_INDIRECT:  si50_id = INTDIR_INDIRECT;
        acase IDC_INTSELECTOR_ACLINE:   si50_is = INTSEL_ACLINE;
        acase IDC_INTSELECTOR_KYBD:     si50_is = INTSEL_KYBD;
        acase IDC_PARALLEL_MEMMAPPED:   si50_io = PARALLEL_MEMMAPPED;
        acase IDC_PARALLEL_EXTENDED:    si50_io = PARALLEL_EXTENDED;
        acase IDC_PARALLEL_NONEXTENDED: si50_io = PARALLEL_NONEXTENDED;
        adefault:
            if
            (   LOWORD(wParam) >= IDC_PARALLEL_BIT7
             && LOWORD(wParam) <= IDC_PARALLEL_BIT0
            )
            {   i = LOWORD(wParam) - IDC_PARALLEL_BIT7;

                if (SendMessage(GetDlgItem(hwnd, IDC_PARALLEL_BIT7 + i), BM_GETCHECK, 0, 0) == BST_CHECKED)
                {   si50_toggles |= 128 >> i;
                } else
                {   si50_toggles &= ~(128 >> i);
        }   }   }

        si50_updatedips(FALSE);
    return FALSE;
    case WM_MOVE:
        reset_fps();
    return FALSE;
    default:
    return FALSE;
    }

    return TRUE;
}

MODULE BOOL CALLBACK ZaccariaDIPsDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT int   gid;
    FAST      HICON localhicon;

    switch (Message)
    {
    case WM_ACTIVATE:
        do_autopause(wParam, lParam);
    acase WM_INITDIALOG:
        subwin[SUBWINDOW_DIPS].hwnd = hwnd;

        DISCARD SetWindowText(hwnd, LLL(MSG_HAIL_G_DIPS, "Zaccaria DIP Switches"));

        setdlgtext(hwnd, IDL_LIVES,      MSG_LIVES,      "Lives");
        setdlgtext(hwnd, IDL_COINA,      MSG_COIN_A,     "Coin A");
        setdlgtext(hwnd, IDL_COINB,      MSG_COIN_B,     "Coin B");
        setdlgtext(hwnd, IDL_HISCORE,    MSG_HISCORE,    "High score");

        setdlgtext(hwnd, IDC_FREEZE,     MSG_FREEZE2,    "Freeze?");
        setdlgtext(hwnd, IDC_COLLISIONS, MSG_DETECTCOLL, "Detect collisions?");
        setdlgtext(hwnd, IDC_RAPIDFIRE,  MSG_RAPIDFIRE,  "Rapid fire?");
        setdlgtext(hwnd, IDC_CALIBGRID,  MSG_CALIBGRID,  "Calibration grid?");

        setfont(   hwnd, IDC_COINA_HALFC);
        setfont(   hwnd, IDC_COINA_1C);
        setfont(   hwnd, IDC_COINA_2C);
        setfont(   hwnd, IDC_COINA_3C);
        setfont(   hwnd, IDC_COINA_5C);
        setfont(   hwnd, IDC_COINB_HALFC);
        setfont(   hwnd, IDC_COINB_1C);
        setfont(   hwnd, IDC_COINB_2C);
        setfont(   hwnd, IDC_COINB_3C);
        setfont(   hwnd, IDC_COINB_5C);
        setfont(   hwnd, IDC_COINB_7C);
        setfont(   hwnd, IDC_LIVES_INFINITE);

        sprintf(gtempstring, "%s %s", allglyphs ? "½" : "1/2", LLL(MSG_CREDIT, "credit"));
        SetDlgItemText(hwnd, IDC_COINA_HALFC, gtempstring);
        SetDlgItemText(hwnd, IDC_COINB_HALFC, gtempstring);
        sprintf(gtempstring, "1 %s", LLL(MSG_CREDIT,  "credit"));
        SetDlgItemText(hwnd, IDC_COINA_1C, gtempstring);
        SetDlgItemText(hwnd, IDC_COINB_1C, gtempstring);
        sprintf(gtempstring, "2 %s", LLL(MSG_CREDITS, "credits"));
        SetDlgItemText(hwnd, IDC_COINA_2C, gtempstring);
        SetDlgItemText(hwnd, IDC_COINB_2C, gtempstring);
        sprintf(gtempstring, "3 %s", LLL(MSG_CREDITS, "credits"));
        SetDlgItemText(hwnd, IDC_COINA_3C, gtempstring);
        SetDlgItemText(hwnd, IDC_COINB_3C, gtempstring);
        sprintf(gtempstring, "5 %s", LLL(MSG_CREDITS, "credits"));
        SetDlgItemText(hwnd, IDC_COINA_5C, gtempstring);
        SetDlgItemText(hwnd, IDC_COINB_5C, gtempstring);
        sprintf(gtempstring, "7 %s", LLL(MSG_CREDITS, "credits"));
        strcat(gtempstring, LLL(MSG_CREDITS, "credits"));
        SetDlgItemText(hwnd, IDC_COINB_7C, gtempstring);

        SetDlgItemText(hwnd, IDC_LIVES_INFINITE,      LLL( MSG_INFINITE,            "Infinite"      ));

        localhicon = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon), IMAGE_ICON, 32, 32, 0);
        DISCARD SendMessage(GetDlgItem(hwnd, IDL_DIPSGLYPH), STM_SETICON, (WPARAM) localhicon, (LPARAM) 0);

        zaccaria_ghostdips();
        zaccaria_updatedips();

        move_subwindow(SUBWINDOW_DIPS);
    return TRUE;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        subwin[SUBWINDOW_DIPS].hwnd = NULL;
        updatemenu(MENUITEM_DIPSWITCHES);
    acase WM_COMMAND:
        gid = (int) LOWORD(wParam);
        if   (gid >= IDC_LIVES_2     && gid <= IDC_LIVES_INFINITE) DISCARD update_livesval(  gid - IDC_LIVES_2    );
        elif (gid >= IDC_COINA_HALFC && gid <= IDC_COINA_5C      ) DISCARD update_coinaval(  gid - IDC_COINA_HALFC);
        elif (gid >= IDC_COINB_HALFC && gid <= IDC_COINB_7C      ) DISCARD update_coinbval(  gid - IDC_COINB_HALFC);
        elif (gid >= IDC_SCORE_0     && gid <= IDC_SCORE_12500   ) DISCARD update_hiscoreval(gid - IDC_SCORE_0    );
        else
        {   switch (gid)
            {
            case IDOK:
                DestroyWindow(hwnd);
                subwin[SUBWINDOW_DIPS].hwnd = NULL;
                updatemenu(MENUITEM_DIPSWITCHES);
            acase IDC_COLLISIONS:
                // assert(memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN);
                if (SendMessage(GetDlgItem(hwnd, IDC_COLLISIONS), BM_GETCHECK, 0, 0) == BST_CHECKED)
                {   update_collval(1);
                } else
                {   update_collval(0);
                }
            acase IDC_FREEZE:
                // assert(memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA || memmap == LAZARIAN);
                if (SendMessage(GetDlgItem(hwnd, IDC_FREEZE    ), BM_GETCHECK, 0, 0) == BST_CHECKED)
                {   update_freezeval(1);
                } else
                {   update_freezeval(0);
                }
            acase IDC_RAPIDFIRE:
                // assert(memmap == MEMMAP_LAZARIAN);
                if (SendMessage(GetDlgItem(hwnd, IDC_RAPIDFIRE ), BM_GETCHECK, 0, 0) == BST_CHECKED)
                {   update_rapidfireval(1);
                } else
                {   update_rapidfireval(0);
                }
            acase IDC_CALIBGRID:
                // assert(memmap == MEMMAP_LAZARIAN);
                if (SendMessage(GetDlgItem(hwnd, IDC_CALIBGRID ), BM_GETCHECK, 0, 0) == BST_CHECKED)
                {   update_gridval(1);
                } else
                {   update_gridval(0);
        }   }   }
    return FALSE;
    case WM_MOVE:
        reset_fps();
    return FALSE;
    default:
    return FALSE;
    }

    return TRUE;
}

MODULE BOOL CALLBACK PongDIPsDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   FAST HICON localhicon;

    switch (Message)
    {
    case WM_ACTIVATE:
        do_autopause(wParam, lParam);
    acase WM_INITDIALOG:
        subwin[SUBWINDOW_DIPS].hwnd = hwnd;

        setdlgtext(hwnd, IDL_BATS,           MSG_BATSIZE         , "Bat size (F4)"         );
        setdlgtext(hwnd, IDC_BATS_SHORT,     MSG_BATSIZE_SHORT   , "Short"                 );
        setfont(hwnd, IDC_BATS_SHORTTALL);
        setfont(hwnd, IDC_BATS_TALLSHORT);
        sprintf
        (   gtempstring,
            "%s, %s",
            (LLL(MSG_BATSIZE_SHORT, "Short")),
            (LLL(MSG_BATSIZE_TALL,  "Tall"))
        );
        SetDlgItemText(hwnd, IDC_BATS_SHORTTALL, gtempstring);
        sprintf
        (   gtempstring,
            "%s, %s",
            (LLL(MSG_BATSIZE_TALL,  "Tall")),
            (LLL(MSG_BATSIZE_SHORT, "Short"))
        );
        SetDlgItemText(hwnd, IDC_BATS_TALLSHORT, gtempstring);
        setdlgtext(hwnd, IDC_BATS_TALL,      MSG_BATSIZE_TALL    , "Tall"                  );
        setdlgtext(hwnd, IDL_SPEED,          MSG_SPEED_MENU      , "Speed"                 );
        setdlgtext(hwnd, IDC_SPEED_SLOW,     MSG_PONGSPEED_SLOW  , "Slow"                  );
        setdlgtext(hwnd, IDC_SPEED_FAST,     MSG_PONGSPEED_FAST  , "Fast"                  );
        setdlgtext(hwnd, IDC_SPEED_RANDOM,   MSG_RANDOM          , "Random"                );
        setdlgtext(hwnd, IDL_GAME,           MSG_GAME            , "Game (F2)"             );
        setdlgtext(hwnd, IDC_LOCKHORIZ,      MSG_LOCKHORIZAXIS   , "Lock horizontal axis?" );
        setdlgtext(hwnd, IDL_ROBOTSPEED,     MSG_ROBOTSPEED      , "Robot speed"           );
        setdlgtext(hwnd, IDL_ROBOTLEFT,      MSG_LEFT2           , "Left:"                 );
        setdlgtext(hwnd, IDL_ROBOTRIGHT,     MSG_RIGHT2          , "Right:"                );

        if (memmap == MEMMAP_8550)
        {   DISCARD SetWindowText(hwnd, LLL(      MSG_HAIL_PONG_DIPS    , "AY-3-8500/8550 DIP Switches"));

            setdlgtext(hwnd, IDL_PONGMACHINE,     MSG_MACHINE_MENU      , "Machine"               );

            setdlgtext(hwnd, IDC_PLAYERID,        MSG_PLAYERID          , "Player identification?");

            setdlgtext(hwnd, IDL_ANGLES,          MSG_ANGLES            , "Angles"                );
            setdlgtext(hwnd, IDC_ANGLES_RANDOM,   MSG_RANDOM            , "Random"                );

            setdlgtext(hwnd, IDL_SERVING,         MSG_SERVING           , "Serving"               );
            setdlgtext(hwnd, IDC_SERVING_AUTO,    MSG_SERVING_AUTO      , "Automatic"             );
            setdlgtext(hwnd, IDC_SERVING_MANUAL,  MSG_SERVING_MANUAL    , "Manual"                );

            setdlgtext(hwnd, IDC_8550_TENNIS,     MSG_VARIANT_TENNIS    , "Tennis"                );
            setdlgtext(hwnd, IDC_8550_SOCCER,     MSG_VARIANT_SOCCER    , "Soccer/Hockey"         );
            setdlgtext(hwnd, IDC_8550_HANDICAP,   MSG_VARIANT_HANDICAP  , "Handicap"              );
            setdlgtext(hwnd, IDC_8550_SQUASH,     MSG_VARIANT_SQUASH    , "Squash/Handball"       );
            setdlgtext(hwnd, IDC_8550_PRACTICE,   MSG_VARIANT_PRACTICE  , "Practice"              );
            setdlgtext(hwnd, IDC_8550_RIFLE1,     MSG_VARIANT_RIFLE1    , "Rifle #1"              );
            setdlgtext(hwnd, IDC_8550_RIFLE2,     MSG_VARIANT_RIFLE2    , "Rifle #2"              );

            setdlgtext(hwnd, IDL_PLAYERS,         MSG_PLAYERS           , "Players"               );
            setdlgtext(hwnd, IDC_PLAYERS_3LT,     MSG_PLAYERS_3LT       , "3 (left team)"         );
            setdlgtext(hwnd, IDC_PLAYERS_3RT,     MSG_PLAYERS_3RT       , "3 (right team)"        );

            localhicon = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon), IMAGE_ICON, 32, 32, 0);
            DISCARD SendMessage(GetDlgItem(hwnd, IDL_DIPSGLYPH), STM_SETICON, (WPARAM) localhicon, (LPARAM) 0);
        } else
        {   // assert(memmap == MEMMAP_8600);

            DISCARD SetWindowText(hwnd, LLL(      MSG_HAIL_8600_DIPS    , "AY-3-8600 DIP Switches"));

            setdlgtext(hwnd, IDC_8600_TENNIS,     MSG_VARIANT_TENNIS    , "Tennis"                );
            setdlgtext(hwnd, IDC_8600_HOCKEY,     MSG_VARIANT_HOCKEY    , "Hockey"                );
            setdlgtext(hwnd, IDC_8600_SOCCER,     MSG_VARIANT_SOCCER2   , "Soccer"                );
            setdlgtext(hwnd, IDC_8600_SQUASH,     MSG_VARIANT_SQUASH2   , "Squash"                );
            setdlgtext(hwnd, IDC_8600_PRACTICE,   MSG_VARIANT_PRACTICE  , "Practice"              );
            setdlgtext(hwnd, IDC_8600_GRIDBALL,   MSG_VARIANT_GRIDBALL  , "Gridball"              );
            setdlgtext(hwnd, IDC_8600_BASKETBALL, MSG_VARIANT_BASKETBALL, "Basketball"            );
            setdlgtext(hwnd, IDC_8600_BBPRACTICE, MSG_VARIANT_BBPRACTICE, "Basketball Practice"   );
            setdlgtext(hwnd, IDC_8600_TARGET1,    MSG_VARIANT_TARGET1   , "1-Player Target"       );
            setdlgtext(hwnd, IDC_8600_TARGET2,    MSG_VARIANT_TARGET2   , "2-Player Target"       );
        }

        SendMessage(GetDlgItem(hwnd, IDC_ROBOTLEFT ), TBM_SETRANGE, FALSE, MAKELONG(0, ROBOTSPEED_INFINITE));
        SendMessage(GetDlgItem(hwnd, IDC_ROBOTLEFT ), TBM_SETPOS,   TRUE,  robotspeed[0]);
        SendMessage(GetDlgItem(hwnd, IDC_ROBOTRIGHT), TBM_SETRANGE, FALSE, MAKELONG(0, ROBOTSPEED_INFINITE));
        SendMessage(GetDlgItem(hwnd, IDC_ROBOTRIGHT), TBM_SETPOS,   TRUE,  robotspeed[1]);

        pong_updatedips();

        move_subwindow(SUBWINDOW_DIPS);
    return TRUE;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        subwin[SUBWINDOW_DIPS].hwnd = NULL;
        updatemenu(MENUITEM_DIPSWITCHES);
    acase WM_HSCROLL:
        if (lParam == (long) GetDlgItem(hwnd, IDC_ROBOTLEFT))
        {   robotspeed[0] = SendMessage
            (   GetDlgItem(hwnd, IDC_ROBOTLEFT),
                TBM_GETPOS,
                0,
                0
            ); // because LOWORD(lParam) is not always correct
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_ROBOTRIGHT))
        {   robotspeed[1] = SendMessage
            (   GetDlgItem(hwnd, IDC_ROBOTRIGHT),
                TBM_GETPOS,
                0,
                0
            ); // because LOWORD(lParam) is not always correct
        }
    acase WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            DestroyWindow(hwnd);
            subwin[SUBWINDOW_DIPS].hwnd = NULL;
            updatemenu(MENUITEM_DIPSWITCHES);
        acase IDC_PONGMACHINE_1976: pong_machine = PONGMACHINE_1976; calc_margins(); draw_margins(); resize(size, TRUE); redrawscreen();
        acase IDC_PONGMACHINE_1977: pong_machine = PONGMACHINE_1977; calc_margins(); draw_margins(); resize(size, TRUE); redrawscreen();
        acase IDC_PONGMACHINE_8550: pong_machine = PONGMACHINE_8550; calc_margins(); draw_margins(); resize(size, TRUE); redrawscreen();
        acase IDC_BATS_SHORT:       batvalue     = 0;                updatebatsize();
        acase IDC_BATS_TALL:        batvalue     = 1;                updatebatsize();
        acase IDC_BATS_SHORTTALL:   batvalue     = 2;                updatebatsize();
        acase IDC_BATS_TALLSHORT:   batvalue     = 3;                updatebatsize();
        acase IDC_SPEED_SLOW:       pongspeed    = PONGSPEED_SLOW;
        acase IDC_SPEED_FAST:       pongspeed    = PONGSPEED_FAST;
        acase IDC_SPEED_RANDOM:     pongspeed    = PONGSPEED_RANDOM;
        acase IDC_ANGLES_2:         angles       = ANGLES_2;
        acase IDC_ANGLES_4:         angles       = ANGLES_4;
        acase IDC_ANGLES_RANDOM:    angles       = ANGLES_RANDOM;
        acase IDC_PLAYERS_2:        players      = PLAYERS_2;
        acase IDC_PLAYERS_3LT:      players      = PLAYERS_3LT;
        acase IDC_PLAYERS_3RT:      players      = PLAYERS_3RT;
        acase IDC_PLAYERS_4:        players      = PLAYERS_4;
        acase IDC_SERVING_AUTO:     serving      = SERVING_AUTO;
        acase IDC_SERVING_MANUAL:   serving      = SERVING_MANUAL;
        acase IDC_8550_TENNIS:      pong_variant = VARIANT_8550_TENNIS;     dogamename();
        acase IDC_8550_SOCCER:      pong_variant = VARIANT_8550_SOCCER;     dogamename();
        acase IDC_8550_HANDICAP:    pong_variant = VARIANT_8550_HANDICAP;   dogamename();
        acase IDC_8550_SQUASH:      pong_variant = VARIANT_8550_SQUASH;     dogamename();
        acase IDC_8550_PRACTICE:    pong_variant = VARIANT_8550_PRACTICE;   dogamename();
        acase IDC_8550_RIFLE1:      pong_variant = VARIANT_8550_RIFLE1;     dogamename();
        acase IDC_8550_RIFLE2:      pong_variant = VARIANT_8550_RIFLE2;     dogamename();
        acase IDC_8600_TENNIS:      pong_variant = VARIANT_8600_TENNIS;     dogamename();
        acase IDC_8600_HOCKEY:      pong_variant = VARIANT_8600_HOCKEY;     dogamename();
        acase IDC_8600_SOCCER:      pong_variant = VARIANT_8600_SOCCER;     dogamename();
        acase IDC_8600_SQUASH:      pong_variant = VARIANT_8600_SQUASH;     dogamename();
        acase IDC_8600_PRACTICE:    pong_variant = VARIANT_8600_PRACTICE;   dogamename();
        acase IDC_8600_GRIDBALL:    pong_variant = VARIANT_8600_GRIDBALL;   dogamename();
        acase IDC_8600_BASKETBALL:  pong_variant = VARIANT_8600_BASKETBALL; dogamename();
        acase IDC_8600_BBPRACTICE:  pong_variant = VARIANT_8600_BBPRACTICE; dogamename();
        acase IDC_8600_TARGET1:     pong_variant = VARIANT_8600_TARGET1;    dogamename();
        acase IDC_8600_TARGET2:     pong_variant = VARIANT_8600_TARGET2;    dogamename();
        acase IDC_LOCKHORIZ:
            if (SendMessage(GetDlgItem(hwnd, IDC_LOCKHORIZ), BM_GETCHECK, 0, 0) == BST_CHECKED)
            {   lockhoriz = TRUE;
            } else
            {   lockhoriz = FALSE;
            }
        acase IDC_PLAYERID:
            if (SendMessage(GetDlgItem(hwnd, IDC_PLAYERID ), BM_GETCHECK, 0, 0) == BST_CHECKED)
            {   playerid = TRUE;
            } else
            {   playerid = FALSE;
        }   }

        pong_updatedips();
    return FALSE;
    case WM_MOVE:
        reset_fps();
    return FALSE;
    default:
    return FALSE;
    }

    return TRUE;
}

MODULE BOOL CALLBACK MalzakDIPsDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   switch (Message)
    {
    case WM_ACTIVATE:
        do_autopause(wParam, lParam);
    acase WM_CLOSE:
        DestroyWindow(hwnd);
        subwin[SUBWINDOW_DIPS].hwnd = NULL;
        updatemenu(MENUITEM_DIPSWITCHES);
    acase WM_INITDIALOG:
        subwin[SUBWINDOW_DIPS].hwnd = hwnd;

        DISCARD SetWindowText(hwnd, LLL(MSG_HAIL_M_DIPS, "Malzak 2 DIP Switches"));
        setdlgtext(hwnd, IDL_TESTSWITCH, MSG_SWITCH,  "Test Switch (F4)");
        setdlgtext(hwnd, IDC_SWITCH_1,   MSG_SWITCH1, "1 (normal play)");
        setdlgtext(hwnd, IDC_SWITCH_4,   MSG_SWITCH4, "4 (change settings)");

        malzak2_updatedips();

        move_subwindow(SUBWINDOW_DIPS);
    return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            DestroyWindow(hwnd);
            subwin[SUBWINDOW_DIPS].hwnd = NULL;
            updatemenu(MENUITEM_DIPSWITCHES);
        acase IDC_SWITCH_1:
            memory[0x1400 + PVI_P1PADDLE] = 0xF0;
        acase IDC_SWITCH_2:
            memory[0x1400 + PVI_P1PADDLE] = 0x90;
        acase IDC_SWITCH_3:
            memory[0x1400 + PVI_P1PADDLE] = 0x70;
        acase IDC_SWITCH_4:
            memory[0x1400 + PVI_P1PADDLE] = 0x00;
        }
    return TRUE;
    case WM_MOVE:
        reset_fps();
    return FALSE;
    default:
    return FALSE;
    }

    return TRUE;
}

MODULE BOOL CALLBACK FloppyDriveDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   FAST      UBYTE       bam;
    FAST      int         i,
                          mousex,
                          mousey,
                          temp,
                          viewbyte,
                          viewing,
                          whichdrive,
                          whichpen;
    FAST      LONG        gid;
    FAST      HICON       localhicon;
    FAST      PAINTSTRUCT localps;
    FAST      POINT       thepoint;
    FAST      RECT        localrect;
    PERSIST   RECT        diskrect[256];

    switch (Message)
    {
    case WM_INITDIALOG:
        subwin[SUBWINDOW_FLOPPYDRIVE].hwnd = hwnd;

        setdlgtext(hwnd, IDC_CREATEDISK      , MSG_CREATEDISK   , "Create disk"    );
        setdlgtext(hwnd, IDC_VERBOSEDISK     , MSG_VERBOSETAPE  , "Verbose output?");
        setdlgtext(hwnd, IDC_INSERTDISK      , MSG_INSERTDISK   , "Insert disk..." );
        setdlgtext(hwnd, IDC_UPDATEDISK      , MSG_UPDATEDISK   , "Update disk..." );
        setdlgtext(hwnd, IDC_EJECTDISK       , MSG_EJECTDISK    , "Eject disk"     );
        setdlgtext(hwnd, IDC_WRITEPROTECT    , MSG_WRITEPROTECT , "Write protect?" );
        setdlgtext(hwnd, IDL_HEADPOSITION    , MSG_HEADPOSITION , "Head Position"  );
        setdlgtext(hwnd, IDL_TRACK           , MSG_TRACK        , "Track:"         );
        setdlgtext(hwnd, IDL_SECTOR          , MSG_SECTOR       , "Sector:"        );
        setdlgtext(hwnd, IDL_BYTE            , MSG_BYTE         , "Byte: $"        );
        setdlgtext(hwnd, IDL_BLOCKCONTENTS   , MSG_BLOCKCONTENTS, "Sector Contents");
        setdlgtext(hwnd, IDL_VIEWDISKAS      , MSG_VIEWAS2      , "View as:"       );
        setdlgtext(hwnd, IDC_VIEWDISKAS_HEX  , MSG_HEXADECIMAL2 , "Hexadecimal"    );
        setdlgtext(hwnd, IDC_VIEWDISKAS_CHARS, MSG_CHARACTERS   , "Characters"     );
        setdlgtext(hwnd, IDL_FILENAME        , MSG_FILENAME     , "Filename:"      );

        SendMessage(GetDlgItem(hwnd, IDC_VERBOSEDISK  ), BM_SETCHECK, verbosedisk ? BST_CHECKED : BST_UNCHECKED, 0);

        switch (machine)
        {
        case BINBUG:
            setdlgtext(hwnd, IDL_DRIVE        , MSG_DRIVE         , "Drive:"         );
            SendMessage(GetDlgItem(hwnd, IDC_WRITEPROTECT ), BM_SETCHECK, drive[viewingdrive].writeprotect ? BST_CHECKED : BST_UNCHECKED, 0);
            diskicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_DISKEMPTY_525));
            localhicon = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon), IMAGE_ICON, 32, 32, 0);
            SendMessage(GetDlgItem(hwnd, IDL_GLYPH      ), STM_SETICON    , (WPARAM) localhicon, (LPARAM) 0);
            SendMessage(GetDlgItem(hwnd, IDC_DISKREGION1), TBM_SETRANGE   ,          FALSE     , MAKELONG(0, BINBUG_DISKBLOCKS - 1));
            spinicon[0] = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_GLYPH_BINBUG_DIM ), IMAGE_ICON, 48, 48, 0);
            spinicon[1] = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_GLYPH_BINBUG_GLOW), IMAGE_ICON, 48, 48, 0);
        acase CD2650:
            setdlgtext(hwnd, IDL_DRIVE         , MSG_DRIVE        , "Drive:"         );
            SendMessage(GetDlgItem(hwnd, IDC_WRITEPROTECT ), BM_SETCHECK, drive[viewingdrive].writeprotect ? BST_CHECKED : BST_UNCHECKED, 0);
            diskicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_DISKEMPTY_525));
            localhicon = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon), IMAGE_ICON, 32, 32, 0);
            SendMessage(GetDlgItem(hwnd, IDL_GLYPH      ), STM_SETICON    , (WPARAM) localhicon, (LPARAM) 0);
            SendMessage(GetDlgItem(hwnd, IDC_DISKREGION1), TBM_SETRANGE   ,          FALSE     , MAKELONG(0, CD2650_DISKBLOCKS - 1));
            spinicon[0] = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_GLYPH_CD2650_DIM ), IMAGE_ICON, 48, 48, 0);
            spinicon[1] = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_GLYPH_CD2650_GLOW), IMAGE_ICON, 48, 48, 0);
        acase TWIN:
            setdlgtext(hwnd, IDL_CLUSTER       , MSG_CLUSTER      , "Cluster:"       );
            SendMessage(GetDlgItem(hwnd, IDC_WRITEPROTECT ), BM_SETCHECK, drive[viewingdrive].writeprotect ? BST_CHECKED : BST_UNCHECKED, 0);
            diskicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_DISKEMPTY_8  ));
            SendMessage(GetDlgItem(hwnd, IDC_DISKREGION1), TBM_SETRANGE   , FALSE, MAKELONG(0, TWIN_DISKBLOCKS - 1));
            SendMessage(GetDlgItem(hwnd, IDC_DISKREGION1), TBM_SETPAGESIZE,     0,                                1);
            spinicon[0] = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_GLYPH_TWIN_DIM ), IMAGE_ICON, 48, 48, 0);
            spinicon[1] = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_GLYPH_TWIN_GLOW), IMAGE_ICON, 48, 48, 0);
        }
        SendMessage(    GetDlgItem(hwnd, IDC_DISKREGION1), TBM_SETPAGESIZE,     0,                                1);

        for (i = 0; i < diskblocksize; i++)
        {   GetWindowRect(GetDlgItem(hwnd, IDC_DISK_0 + i), &localrect);
            thepoint.x        = localrect.left;
            thepoint.y        = localrect.top;
            DISCARD ScreenToClient(hwnd, &thepoint);
            diskrect[i].left  = thepoint.x;
            diskrect[i].top   = thepoint.y;
            thepoint.x        = localrect.right;
            thepoint.y        = localrect.bottom;
            DISCARD ScreenToClient(hwnd, &thepoint);
            diskrect[i].right  = thepoint.x;
            diskrect[i].bottom = thepoint.y;
        }
        // If they move the subwindow later, it doesn't matter.

        DISCARD CheckRadioButton
        (   hwnd,
            IDC_VIEWDISKAS_HEX,
            IDC_VIEWDISKAS_CHARS,
            viewdiskas ? IDC_VIEWDISKAS_CHARS : IDC_VIEWDISKAS_HEX
        );

        for (whichdrive = 0; whichdrive < machines[machine].drives; whichdrive++)
        {   sprintf(gtempstring, "%d", whichdrive);
            SendMessage(GetDlgItem(hwnd, IDC_DRIVE ), LB_ADDSTRING, 0, (LPARAM) gtempstring);
            sprintf(gtempstring, "%d", whichdrive + 1);
            SendMessage(GetDlgItem(hwnd, IDC_DRIVES), LB_ADDSTRING, 0, (LPARAM) gtempstring);
        }
        SendMessage(    GetDlgItem(hwnd, IDC_DRIVE ), LB_SETCURSEL, (WPARAM) viewingdrive, 0); // ordinal number in list
        switch (machine)
        {
        case  BINBUG: SendMessage(GetDlgItem(hwnd, IDC_DRIVES), LB_SETCURSEL, (WPARAM) (binbug_userdrives - 1), 0); // ordinal number in list
        acase TWIN:   SendMessage(GetDlgItem(hwnd, IDC_DRIVES), LB_SETCURSEL, (WPARAM) (  twin_userdrives - 1), 0); // ordinal number in list
        acase CD2650: SendMessage(GetDlgItem(hwnd, IDC_DRIVES), LB_SETCURSEL, (WPARAM) (cd2650_userdrives - 1), 0); // ordinal number in list
        }

        move_subwindow(SUBWINDOW_FLOPPYDRIVE);
    return FALSE;
    case WM_ACTIVATE:
        do_autopause(wParam, lParam);
    acase WM_COMMAND:
        gid = (LONG) LOWORD(wParam);
        switch (gid)
        {
        case IDC_CREATEDISK:
            switch (machine)
            {
            case  BINBUG: binbug_create_disk(viewingdrive);
            acase CD2650: cd2650_create_disk(viewingdrive);
            acase TWIN:   twin_create_disk(  viewingdrive);
            }
        acase IDC_INSERTDISK:
            load_disk(TRUE, viewingdrive, TRUE);
        acase IDC_UPDATEDISK:
            update_disk(viewingdrive);
        acase IDC_EJECTDISK:
            drive[viewingdrive].inserted = FALSE;
            // play_ambient_sample(SAMPLE_EJECTDISK);
            update_floppydrive(TRUE, viewingdrive);
        acase IDC_VERBOSEDISK:
            if (SendMessage(GetDlgItem(hwnd, IDC_VERBOSEDISK), BM_GETCHECK, 0, 0) == BST_CHECKED)
            {   verbosedisk = TRUE;
            } else
            {   verbosedisk = FALSE;
            }
        acase IDC_WRITEPROTECT:
            if (SendMessage(GetDlgItem(hwnd, IDC_WRITEPROTECT), BM_GETCHECK, 0, 0) == BST_CHECKED)
            {   drive[viewingdrive].writeprotect = TRUE;
            } else
            {   drive[viewingdrive].writeprotect = FALSE;
            }
        acase IDC_PLATTER:
            if (!drive[viewingdrive].inserted)
            {   load_disk(TRUE, viewingdrive, TRUE);
            }
        acase IDC_VIEWDISKAS_HEX:
        case IDC_VIEWDISKAS_CHARS:
            viewdiskas = gid - IDC_VIEWDISKAS_HEX;
            DISCARD CheckRadioButton
            (   hwnd,
                IDC_VIEWDISKAS_HEX,
                IDC_VIEWDISKAS_CHARS,
                viewdiskas ? IDC_VIEWDISKAS_CHARS : IDC_VIEWDISKAS_HEX
            );
            update_floppydrive(TRUE, viewingdrive);
        acase IDC_DRIVE:
            if (HIWORD(wParam) == LBN_SELCHANGE)
            {   temp = SendMessage(GetDlgItem(hwnd, IDC_DRIVE), LB_GETCURSEL, 0, 0);
                if
                (   machine == BINBUG && temp >= binbug_userdrives
                 || machine == CD2650 && temp >= cd2650_userdrives
                 || machine == TWIN   && temp >=   twin_userdrives
                )
                {   SendMessage(   GetDlgItem(hwnd, IDC_DRIVE), LB_SETCURSEL, (WPARAM) viewingdrive, 0); // ordinal number in list
                } elif (viewingdrive != temp)
                {   viewingdrive = temp;
                    update_floppydrive(TRUE, viewingdrive);
            }   }
        acase IDC_DRIVES:
            if (HIWORD(wParam) == LBN_SELCHANGE)
            {   switch (machine)
                {
                case BINBUG:
                    binbug_userdrives = SendMessage(GetDlgItem(hwnd, IDC_DRIVES), LB_GETCURSEL, 0, 0) + 1;
                    if ((int) curdrive >= binbug_userdrives)
                    {   curdrive = binbug_userdrives - 1;
                    }
                    if (viewingdrive >= binbug_userdrives)
                    {   viewingdrive = binbug_userdrives - 1;
                        update_floppydrive(TRUE, viewingdrive);
                    }
                acase TWIN:
                    twin_userdrives = SendMessage(GetDlgItem(hwnd, IDC_DRIVES), LB_GETCURSEL, 0, 0) + 1;
                    if ((int) curdrive >= twin_userdrives)
                    {   curdrive = twin_userdrives - 1;
                    }
                    if (viewingdrive >= twin_userdrives)
                    {   viewingdrive = twin_userdrives - 1;
                        update_floppydrive(TRUE, viewingdrive);
                    }
                acase CD2650:
                    cd2650_userdrives = SendMessage(GetDlgItem(hwnd, IDC_DRIVES), LB_GETCURSEL, 0, 0) + 1;
                    if (cd2650_dosver == DOS_P1DOS)
                    {   memory[0x60BE] = cd2650_userdrives - 1;
                        update_memory(FALSE); // in case they are looking at that region in memory editor
                    }
                    if ((int) curdrive >= cd2650_userdrives)
                    {   curdrive = cd2650_userdrives - 1;
                    }
                    if (viewingdrive >= cd2650_userdrives)
                    {   viewingdrive = cd2650_userdrives - 1;
                        update_floppydrive(TRUE, viewingdrive);
        }   }   }   }
    acase WM_DRAWITEM:
        switch (wParam)
        {
        case IDC_CREATEDISK:
        case IDC_INSERTDISK:
        case IDC_UPDATEDISK:
            drawitem((struct tagDRAWITEMSTRUCT*) lParam, FALSE);
        }
    return TRUE;
    case WM_CLOSE:
        clearkybd();
        if (diskicon)
        {   DeleteObject(diskicon);
            diskicon = NULL;
        }
        DestroyWindow(hwnd);
        subwin[SUBWINDOW_FLOPPYDRIVE].hwnd = NULL;
        updatemenu(MENUITEM_FLOPPYDRIVE);
        DeleteObject(spinicon[0]);
        DeleteObject(spinicon[1]);
    return TRUE;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
        mousex = (int) LOWORD(lParam);
        mousey = (int) HIWORD(lParam);
        for (i = 0; i < diskblocksize; i++)
        {   if
            (   mousex >= diskrect[i].left
             && mousex <= diskrect[i].right
             && mousey >= diskrect[i].top
             && mousey <= diskrect[i].bottom
            )
            {   viewbyte = drive[viewingdrive].viewstart + i;
                if (drive[viewingdrive].flags[viewbyte / 8] &   (1 << (viewbyte % 8)))
                {   drive[viewingdrive].flags[viewbyte / 8] &= ~(1 << (viewbyte % 8));
                } else
                {   drive[viewingdrive].flags[viewbyte / 8] |=  (1 << (viewbyte % 8));
                    if (!watchreads && !watchwrites)
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_WPWARNING, "Warning: watching of reads and writes is currently turned off! (WR and WW to enable.)\n"));
                }   }
                update_floppydrive(TRUE, viewingdrive);
                break; // for speed
        }   }
    return FALSE;
    case WM_CTLCOLORSTATIC:
        gid = GetWindowLong((HWND) lParam, GWL_ID);
        switch (gid)
        {
        case IDC_TRACK:
        case IDC_CLUSTER:
        case IDC_SECTOR:
        case IDC_BYTE:
            whichpen = getdiskmodecolour();
            SetBkColor((HDC) wParam, whichpen);
            switch (whichpen)
            {
            case EMUPEN_BLUE:       return (LRESULT) hBrush[EMUBRUSH_BLUE];
            case EMUPEN_GREEN:      return (LRESULT) hBrush[EMUBRUSH_GREEN];
            case EMUPEN_RED:        return (LRESULT) hBrush[EMUBRUSH_RED];
            }
        acase IDC_FILENAME:
            whichpen = getdiskclustercolour(drive[viewingdrive].viewstart);
            SetBkColor((HDC) wParam, whichpen);
            switch (whichpen)
            {
            case EMUPEN_GREY:       return (LRESULT) hBrush[EMUBRUSH_GREY];
            case EMUPEN_PURPLE:     return (LRESULT) hBrush[EMUBRUSH_PURPLE];
            case EMUPEN_PINK:       return (LRESULT) hBrush[EMUBRUSH_PINK];
            case EMUPEN_CYAN:       return (LRESULT) hBrush[EMUBRUSH_CYAN];
            }
        adefault:
            if (gid >= IDC_DISK_0 && gid <= IDC_DISK_255)
            {   whichpen = getdiskbytecolour(drive[viewingdrive].viewstart + gid - IDC_DISK_0);
                SetBkColor((HDC) wParam, whichpen); 
                switch (whichpen)
                {
                case EMUPEN_GREY:   return (LRESULT) hBrush[EMUBRUSH_GREY];
                case EMUPEN_BLUE:   return (LRESULT) hBrush[EMUBRUSH_BLUE];
                case EMUPEN_GREEN:  return (LRESULT) hBrush[EMUBRUSH_GREEN];
                case EMUPEN_YELLOW: return (LRESULT) hBrush[EMUBRUSH_YELLOW];
                case EMUPEN_ORANGE: return (LRESULT) hBrush[EMUBRUSH_ORANGE];
                case EMUPEN_RED:    return (LRESULT) hBrush[EMUBRUSH_RED];
                case EMUPEN_PURPLE: return (LRESULT) hBrush[EMUBRUSH_PURPLE];
                case EMUPEN_PINK:   return (LRESULT) hBrush[EMUBRUSH_PINK];
                case EMUPEN_CYAN:   return (LRESULT) hBrush[EMUBRUSH_CYAN];
        }   }   }
    acase WM_DESTROY:
        subwin[SUBWINDOW_FLOPPYDRIVE].hwnd = NULL;
    return FALSE;
    case WM_MOVE:
        reset_fps();
    return FALSE;
    case WM_PAINT:
        DISCARD BeginPaint(hwnd, &localps);
        update_floppydrive(TRUE, viewingdrive);
        DISCARD EndPaint(hwnd, &localps);
    return FALSE; // important!
    case WM_HSCROLL:
        gid = GetWindowLong((HWND) lParam, GWL_ID);
        if (gid == IDC_DISKREGION1)
        {   drive[viewingdrive].viewstart = SendMessage
            (   GetDlgItem(hwnd, IDC_DISKREGION1),
                TBM_GETPOS,
                0,
                0
            ) * diskblocksize; // because LOWORD(lParam) is not always correct
            update_floppydrive(FALSE, viewingdrive);
        }
    return FALSE;
    default:
    return FALSE;
    }
    return TRUE;
}

EXPORT void im_set(int whichsubwin, int gid, int whichimage)
{   SendMessage(GetDlgItem(subwin[whichsubwin].hwnd, gid), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) tapeimage[whichimage]);
}

EXPORT void draw_platter(FLAG all, UBYTE oldtrack, UBYTE oldsector)
{   FAST int      i,
                  localsector,
                  localtrack;
    FAST HDC      DiskRastPtr;
    FAST HPEN     CyanPen, PinkPen, PurplePen, BlackPen, GreenPen, OrangePen, RedPen, WhitePen;
    FAST double   angle,
                  byte_offset,
                  linelength;
    FAST int      amount;
    FAST COLORREF whichcolour;

    if (all)
    {   SendMessage(GetDlgItem(subwin[SUBWINDOW_FLOPPYDRIVE].hwnd, IDC_PLATTER ), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) diskicon);
    }

    if (!drive[viewingdrive].inserted)
    {   return;
    }

    DiskRastPtr = GetDC(GetDlgItem(subwin[SUBWINDOW_FLOPPYDRIVE].hwnd, IDC_PLATTER));
    RedPen    = CreatePen(PS_SOLID, 1, EMUPEN_RED);
    GreenPen  = CreatePen(PS_SOLID, 1, EMUPEN_DARKGREEN);
    OrangePen = CreatePen(PS_SOLID, 1, EMUPEN_DARKORANGE);
    CyanPen   = CreatePen(PS_SOLID, 1, EMUPEN_CYAN);
    PinkPen   = CreatePen(PS_SOLID, 1, EMUPEN_PINK);
    PurplePen = CreatePen(PS_SOLID, 1, EMUPEN_PURPLE);
    BlackPen  = CreatePen(PS_SOLID, 1, EMUPEN_BLACK);
    WhitePen  = CreatePen(PS_SOLID, 1, EMUPEN_WHITE);

    switch (machine)
    {
    case TWIN:
        if (all)
        {   // Tracks-----------------------------------------------------
            for (i = 0; i < TWIN_DISKBLOCKS; i++) // 2464
            {   localtrack  = i / TWIN_SECTORS;
                localsector = i % TWIN_SECTORS;
                if (localtrack == 0) // first track is reserved
                {   SelectObject(DiskRastPtr, PurplePen);
                } else
                {   switch (drive[viewingdrive].bam[(i - 32) / 8])
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

            // Sectors----------------------------------------------------
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
        else
        {
#ifdef DISKLINE
            byte_offset = (drive[viewingdrive].sector * 128) + drive[viewingdrive].blockoffset;
            angle = (byte_offset / 4096.0) * 2 * PI;
            linelength = 10.0 + ((92.0 * (TWIN_TRACKS - drive[viewingdrive].track)) / TWIN_TRACKS);
            SetROP2(DiskRastPtr, R2_XORPEN);
            SelectObject(DiskRastPtr, WhitePen);
            MoveToEx(DiskRastPtr, 107, 107, NULL);
            LineTo(DiskRastPtr, (int) (107 - (linelength * sin(angle))), (int) (107 - (linelength * cos(angle))));
            SetROP2(DiskRastPtr, R2_COPYPEN);
#endif

            if (oldtrack == 0) // first track is reserved
            {   SelectObject(DiskRastPtr, PurplePen);
            } else
            {   switch (drive[viewingdrive].bam[(((oldtrack * TWIN_SECTORS) + oldsector) - 32) / 8])
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

            switch (drive_mode)
            {
            case  DRIVEMODE_READING: SelectObject(DiskRastPtr, GreenPen);
            acase DRIVEMODE_WRITING: SelectObject(DiskRastPtr, RedPen);
            acase DRIVEMODE_IDLE:
                switch (drive[viewingdrive].bam[(((drive[viewingdrive].track * TWIN_SECTORS) + drive[viewingdrive].sector) - 32) / 8])
                {
                case  BAM_LOST: SelectObject(DiskRastPtr, BlackPen);
                acase BAM_DIR:  SelectObject(DiskRastPtr, PurplePen);
                acase BAM_FILE: SelectObject(DiskRastPtr, PinkPen);
                acase BAM_FREE: SelectObject(DiskRastPtr, CyanPen);
            }   }
            Arc
            (   DiskRastPtr,
                  5 + (drive[viewingdrive].track * 3 / 2),
                  5 + (drive[viewingdrive].track * 3 / 2),
                324 - (drive[viewingdrive].track * 3 / 2),
                324 - (drive[viewingdrive].track * 3 / 2),
                twin_sectorpoint[drive[viewingdrive].sector].xarcstart,
                twin_sectorpoint[drive[viewingdrive].sector].yarcstart,
                twin_sectorpoint[drive[viewingdrive].sector].xarcend,
                twin_sectorpoint[drive[viewingdrive].sector].yarcend
            );
        }
    acase BINBUG:
        if (all)
        {   // Tracks-----------------------------------------------------
            for (i = 0; i < BINBUG_DISKBLOCKS; i++)
            {   localtrack  =  i / BINBUG_SECTORS;
                localsector = (i % BINBUG_SECTORS) + 1;
                switch (drive[viewingdrive].bam[i])
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

            // Sectors----------------------------------------------------
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
        else
        {
#ifdef DISKLINE
            byte_offset = ((drive[viewingdrive].sector - 1) * 256) + drive[viewingdrive].blockoffset;
            angle = (byte_offset / 2560.0) * 2.0 * PI;
            linelength = 10.0 + ((92.0 * (BINBUG_TRACKS - drive[viewingdrive].track)) / BINBUG_TRACKS);
            SetROP2(DiskRastPtr, R2_XORPEN);
            SelectObject(DiskRastPtr, WhitePen);
            MoveToEx(DiskRastPtr, 107, 107, NULL);
            LineTo(DiskRastPtr, (int) (107 - (linelength * sin(angle))), (int) (107 - (linelength * cos(angle))));
            SetROP2(DiskRastPtr, R2_COPYPEN);
#endif

            switch (drive[viewingdrive].bam[(oldtrack * BINBUG_SECTORS) + oldsector - 1])
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

            switch (drive_mode)
            {
            case  DRIVEMODE_READING: SelectObject(DiskRastPtr, GreenPen);
            acase DRIVEMODE_WRITING: SelectObject(DiskRastPtr, RedPen);
            acase DRIVEMODE_IDLE:
                switch (drive[viewingdrive].bam[(drive[viewingdrive].track * BINBUG_SECTORS) + drive[viewingdrive].sector - 1])
                {
                case  BAM_LOST: SelectObject(DiskRastPtr, BlackPen);
                acase BAM_DIR:  SelectObject(DiskRastPtr, PurplePen);
                acase BAM_FILE: SelectObject(DiskRastPtr, PinkPen);
                acase BAM_FREE: SelectObject(DiskRastPtr, CyanPen);
            }   }
            Arc
            (   DiskRastPtr,
                  5 + (drive[viewingdrive].track * 2),
                  5 + (drive[viewingdrive].track * 2),
                210 - (drive[viewingdrive].track * 2),
                210 - (drive[viewingdrive].track * 2),
                binbug_sectorpoint[drive[viewingdrive].sector - 1].xarcstart,
                binbug_sectorpoint[drive[viewingdrive].sector - 1].yarcstart,
                binbug_sectorpoint[drive[viewingdrive].sector - 1].xarcend,
                binbug_sectorpoint[drive[viewingdrive].sector - 1].yarcend
            );
        }
    acase CD2650:
        if (all)
        {   // Tracks-----------------------------------------------------
            for (i = 0; i < CD2650_DISKBLOCKS; i++)
            {   localtrack  =  i / CD2650_SECTORS;
                localsector = (i % CD2650_SECTORS) + 1;
                switch (drive[viewingdrive].bam[i])
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

            // Sectors----------------------------------------------------
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
        else
        {
#ifdef DISKLINE
            byte_offset = ((drive[viewingdrive].sector - 1) * 256) + drive[viewingdrive].blockoffset;
            angle = (byte_offset / 2304.0) * 2 * PI;
            linelength = 10.0 + ((92.0 * (CD2650_TRACKS - drive[viewingdrive].track)) / CD2650_TRACKS);
            SetROP2(DiskRastPtr, R2_XORPEN);
            SelectObject(DiskRastPtr, WhitePen);
            MoveToEx(DiskRastPtr, 107, 107, NULL);
            LineTo(DiskRastPtr, (int) (107 - (linelength * sin(angle))), (int) (107 - (linelength * cos(angle))));
            SetROP2(DiskRastPtr, R2_COPYPEN);
#endif

            switch (drive[viewingdrive].bam[(oldtrack * CD2650_SECTORS) + oldsector - 1])
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

            switch (drive_mode)
            {
            case  DRIVEMODE_READING: SelectObject(DiskRastPtr, GreenPen);
            acase DRIVEMODE_WRITING: SelectObject(DiskRastPtr, RedPen);
            acase DRIVEMODE_IDLE:
                switch (drive[viewingdrive].bam[(drive[viewingdrive].track * CD2650_SECTORS) + drive[viewingdrive].sector - 1])
                {
                case  BAM_DIR:  SelectObject(DiskRastPtr, PurplePen);
                acase BAM_FILE: SelectObject(DiskRastPtr, PinkPen);
                acase BAM_FREE: SelectObject(DiskRastPtr, CyanPen);
            }   }
            amount = (drive[viewingdrive].track * 2) + ((drive[viewingdrive].track * 2) / 7);
            Arc
            (   DiskRastPtr,
                  5 + amount,
                  5 + amount,
                210 - amount,
                210 - amount,
                cd2650_sectorpoint[drive[viewingdrive].sector - 1].xarcstart,
                cd2650_sectorpoint[drive[viewingdrive].sector - 1].yarcstart,
                cd2650_sectorpoint[drive[viewingdrive].sector - 1].xarcend,
                cd2650_sectorpoint[drive[viewingdrive].sector - 1].yarcend
            );
    }   }

    ReleaseDC(GetDlgItem(subwin[SUBWINDOW_FLOPPYDRIVE].hwnd, IDC_PLATTER), DiskRastPtr);
    DeleteObject(CyanPen);
    DeleteObject(PinkPen);
    DeleteObject(PurplePen);
    DeleteObject(BlackPen);
    DeleteObject(GreenPen);
    DeleteObject(RedPen);
    DeleteObject(WhitePen);
    DeleteObject(OrangePen);
}

EXPORT void setdrivegad(int gid, int whichpen)
{   // assert(subwin[SUBWINDOW_FLOPPYDRIVE].hwnd);

    SetDlgItemText(subwin[SUBWINDOW_FLOPPYDRIVE].hwnd, gid, gtempstring);
}
