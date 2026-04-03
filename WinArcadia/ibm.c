// INCLUDES---------------------------------------------------------------

#include "ibm.h"

#include "ws2tcpip.h"
#include <commctrl.h>
#include <richedit.h>
#include <vfw.h>       // for eg. PAVIFILE
#include <windowsx.h>  // for eg. SelectBitmap()
#include <Shlobj.h>    // for eg. SHAddToRecentDocs()
#include <Shlwapi.h>   // for eg. PathRemoveFileSpec()
// #include <signal.h> // for eg. signal()
#include <sys/stat.h>  // for eg. _S_IREAD
#include "dinput.h"
#include <d3dx9.h>
#include "resource.h"

#define INCL_RXFUNC
#define INCL_RXSUBCOM
#define INCL_RXSYSEXIT
#include "rexxsaa.h"

typedef unsigned char bool;
#include "RA_Interface.h"

#include <stdio.h>

#define EXEC_TYPES_H
#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"

// DEFINES----------------------------------------------------------------

// #define TRACKEXIT
#ifdef TRACKEXIT
    #define EXITING(a)  zprintf(TEXTPEN_VERBOSE, "Exit action %d...\n", a);
#else
    #define EXITING(a)  ;
#endif

// #define USEGRADIENT
// enable this if you want gradient fills

// #define AUTOPADS
// enable this you want to automagically configure detected gamepads

// #define COUNTBITSEQUENCES
// enable this to count and show the number of bit sequences that are
//  valid instructions
// Note that it doesn't bother to preserve and restore the proper value of
//  supercpu in this mode.
// The 2650A has 4,222,536 valid bit sequences.
// The 2650B has 4,353,608 valid bit sequences.
// The number of valid byte sequences is equal, in these
//  cases, to the number of valid bit sequences.
// In some cases a particular bit might be ignored;
//  these are still valid bit sequences though.
// LODZ,r0 is considered legal in this context,
//  but other illegal instructions are not.

#define UISF_HIDEACCEL           2
#define UIS_CLEAR                2
#define WM_CHANGEUISTATE     0x127
#define SPI_GETKEYBOARDCUES 0x100A
#define SPI_SETKEYBOARDCUES 0x100B
#define LVS_EX_LABELTIP     0x4000
#define SIDEBARWIDTH           118
#define CAPSLOCKWIDTH           57 // this value is the same for Win9x as for WinXP

#define BELOW_NORMAL_PRIORITY_CLASS 0x00004000
#define ABOVE_NORMAL_PRIORITY_CLASS 0x00008000

#ifdef VERBOSE
    #define IPPROTO_IPV4    4
    #define IPPROTO_IPV6   41
#endif

#define IDC_TOOLBAR       400
#define IDC_FPS           401 // actual FPS value
#define IDL_FPS           402 // "FPS:" label
#define IDC_STATUSBAR     403

// this #ifndef is for Visual Studio 9 compatibility
#ifndef _WINUSER_
    typedef struct
    {   DWORD  vkCode;
        DWORD  scanCode;
        DWORD  flags;
        DWORD  time;
        ULONG* dwExtraInfo;
    } KBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT;
#endif

// #define MEASURECAPSLOCK
// define this if you want to measure the width of the Caps Lock indicator.
// You give the desired gadget width as a CLI argument.

#if defined(_WIN64)
    typedef unsigned __int64 ULONG_PTR;
#else
    typedef unsigned long    ULONG_PTR;
#endif

#define _SS_MAXSIZE   128
#define _SS_ALIGNSIZE (sizeof(__int64))
#define _SS_PAD1SIZE  (_SS_ALIGNSIZE - sizeof (short))
#define _SS_PAD2SIZE  (_SS_MAXSIZE - (sizeof (short) + _SS_PAD1SIZE \
                                                     + _SS_ALIGNSIZE))
typedef struct sockaddr_storage
{   short   ss_family;
    char    __ss_pad1[_SS_PAD1SIZE];
    __int64 __ss_align;
    char    __ss_pad2[_SS_PAD2SIZE];
} SOCKADDR_STORAGE, *PSOCKADDR_STORAGE;

// EXPORTED VARIABLES ----------------------------------------------------

EXPORT const char               g_szClassName[]          = "WinArcadia",
                                g_szMagnifierClassName[] = "WinArcadia magnifier",
                                g_szTestClassName[]      = "WinArcadia test";
EXPORT       UWORD              temp_console[4],
                                temp_keypads[2][NUMKEYS];
EXPORT       POINT              ThePoint;
EXPORT       FLAG               capslock             = FALSE,
                                d3d                  = FALSE,
                                incli                = FALSE,
                                allglyphs            = FALSE,
                                lmb                  = FALSE,
                                mmb                  = FALSE,
                                opening              = FALSE,
                                quitting             = FALSE,
                                ready                = FALSE,
                                rmb                  = FALSE,
                                sizing               = FALSE,
                                skiphostname         = FALSE,
                                unit[2]              = { FALSE, FALSE },
                                upgrade              = FALSE,
                                win7                 = FALSE,
                                yank                 = FALSE;
EXPORT       int                bottomheight,
                                CatalogPtr, // never used
                                clientheight,
                                clientwidth,
                                climode              = 0,
                                cheevos              = FALSE,
                                currentgame          = -1,
                                debuggerheight,
                                debuggertop,
                                debuggerwidth,
                                destwidth,
                                destheight,
                                DisplayWidth,
                                DisplayHeight,
                                fastwide,
                                hostcontroller[2]    = { CONTROLLER_NONE, CONTROLLER_NONE },
                                hostheight,
                                hostwidth,
                                hostvolume           = HOSTVOLUME_DEFAULT,
                                joys                 = 0,
                                language             = LANGUAGE_ENG,
                                leftwidth,
                                memmap_to_smlimage[MEMMAPS],
                                menuheight,
                                needaudit            = FALSE,
                                nextrow,
                                nextrow2,
                                outputpos            = 0,
                                priority             = PRI_NORMAL,
                                promptwidth,
                                quiet                = FALSE,
                                toolbarheight,
                                rastwidth,
                                rastheight,
                                realsize,
                                realwide,
                                reassociate          = FALSE,
                                sidebarheight,
                                sidebartopy,
                                sidebarwidth         = SIDEBARWIDTH,
                                sourcewidth,
                                sourceheight,
                                statusbarheight,
                                storedmenu1          = -1,
                                storedmenu2          = -1;
                                stretch43            = FALSE,
                                subwinx[SUBWINDOWS],
                                subwiny[SUBWINDOWS],
                                titleheight,
                                useff[2]             = { TRUE, TRUE },
                                winheight,
                                winwidth,
                                winleftx             = -1,
                                wintopy              = -1,
                                xoffset,
                                yoffset;
EXPORT       ULONG              jff[2],
                                keyframes[SCANCODES],
                                ptrinc,
                                speech_rate          = 10;
EXPORT       ULONG             *canvasdisplay[CANVASES],
                               *stars                = NULL;
EXPORT       UBYTE              jx[2], jy[2],
                                KeyMatrix[SCANCODES / 8]; // to allow keycodes up to 511
EXPORT       STRPTR             rexxwhere;
EXPORT       ASCREEN            screen[BOXWIDTH][BOXHEIGHT];
EXPORT       UINT               storedcode           = 0;
EXPORT       DWORD              winstyle;
EXPORT       HWND               hDebugger            = NULL,
                                hSideBar             = NULL,
                                hStatusBar           = NULL,
                                hToolbar             = NULL,
                                ListeningWindowPtr   = NULL,
                                MainWindowPtr        = NULL,
                                SubWindowPtr[SUBWINDOWS],
                                TipsPtr[SUBWINDOWS];
EXPORT       HBRUSH             hBrush[EMUBRUSHES];
EXPORT       HCURSOR            hArrow,
                                hBusy,
                                hCross[3];
EXPORT       HFONT              hFont                = NULL,
                                hBoldFont            = NULL,
                                hGreekFont           = NULL,
                                hMonospaceFont       = NULL,
                                hPolishFont          = NULL,
                                hRussianFont         = NULL,
                                hSmallFont           = NULL;
EXPORT       HICON              bigicon,
                                icons[15],
                                smlicon;
EXPORT       HMENU              MenuPtr              = NULL;
EXPORT       HBITMAP            BezelBitMap[BEZELS],
                                OurhBitMap           = NULL;
EXPORT       TEXT               consolestring[OUTPUTLENGTH + 1],
                                hostname[256 + 1],
                                ProgDir[MAX_PATH + 1],
                                ProgName[MAX_PATH + 1],
                                username[MAX_USERNAMELEN + 1];
EXPORT       HDC                OurhDC               = NULL;
EXPORT       HINSTANCE          InstancePtr          = NULL;
EXPORT       struct RTCStruct   rtc;
EXPORT const DWORD              joyfires[8]          = { JOYFIRE1, JOYFIRE2, JOYFIRE3, JOYFIRE4, JOYA, JOYB, JOYAUTOFIRE, JOYPAUSE };

EXPORT struct
{   BITMAPINFOHEADER Header;
    DWORD            Colours[3];
} CanvasBitMapInfo[CANVASES];

EXPORT       struct HostMachineStruct hostmachines[MACHINES] =
{ {          "WinArcadia " DECIMALVERSION, IDD_MONITOR_CPU_2650 , 0               , IDD_MONITOR_XVI_ARCADIA , 0                      ,  4.0f, 512 }, // height of 269 or 312 (assuming PAL)
  {         "WinInterton " DECIMALVERSION, IDD_MONITOR_CPU_2650 , 0               , IDD_MONITOR_XVI_INTERTON, 0                      ,  4.0f, 512 }, // height of 269 or 312 (assuming PAL)
  {          "WinElektor " DECIMALVERSION, IDD_MONITOR_CPU_2650 , IDD_MONITOR_PSGS, IDD_MONITOR_XVI_ELEKTOR , IDD_CONTROLS_ELEKTOR   ,  4.0f, 512 }, // height of 269 or 312 (assuming PAL)
  {           "WinPIPBUG " DECIMALVERSION, IDD_MONITOR_CPU_2650 , 0               , 0                       , 0                      ,  5.0f, 256 }, // height of 234
  {           "WinBINBUG " DECIMALVERSION, IDD_MONITOR_CPU_2650 , 0               , IDD_MONITOR_XVI_PONG    , IDD_CONTROLS_BINBUG    ,  5.0f, 512 }, // height of 256 or 313. ID_MONITOR_XVI_PONG is no mistake
  {       "WinInstructor " DECIMALVERSION, IDD_MONITOR_CPU_2650 , 0               , 0                       , 0                      ,  2.5f,  64 }, // height of  64
  {             "WinTWIN " DECIMALVERSION, IDD_MONITOR_CPUS_TWIN, 0               , 0                       , IDD_CONTROLS_TWIN      ,  5.0f, 512 }, // height of 225
  {           "WinCD2650 " DECIMALVERSION, IDD_MONITOR_CPU_2650 , 0               , 0                       , 0                      ,  5.0f, 512 }, // height of 192 or 264
  {         "WinZaccaria " DECIMALVERSION, IDD_MONITOR_CPU_2650 , 0               , 0                       , 0                      ,  5.0f, 256 }, // height of 240
  {           "WinMalzak " DECIMALVERSION, IDD_MONITOR_CPU_2650 , 0               , IDD_MONITOR_2PVI        , IDD_CONTROLS_MALZAK    ,  5.0f, 256 }, // height of 240
  {           "WinPHUNSY " DECIMALVERSION, IDD_MONITOR_CPU_2650 , 0               , 0                       , IDD_CONTROLS_PHUNSY    ,  5.0f, 512 }, // height of 298 or 355
  {             "WinPong " DECIMALVERSION, 0                    , 0               , IDD_MONITOR_XVI_PONG    , 0                      ,  5.0f, 512 }, // height of 242 or 312 (assuming PAL)
  {"WinSelbstbaucomputer " DECIMALVERSION, IDD_MONITOR_CPU_2650 , 0               , 0                       , IDD_CONTROLS_SELBST    ,  5.0f, 256 }, // height of 194 or 204
  {            "WinMIKIT " DECIMALVERSION, IDD_MONITOR_CPU_2650 , 0               , 0                       , 0                      ,  5.0f,  64 }, // height of  64
#ifdef BIGTYPERIGHT
  {       "WinType-right " DECIMALVERSION, IDD_MONITOR_TR       , 0               , 0                       , IDD_CONTROLS_TYPERIGHT ,  2.5f, 128 }, // height of 128
#else
  {       "WinType-right " DECIMALVERSION, IDD_MONITOR_TR       , 0               , 0                       , IDD_CONTROLS_TYPERIGHT ,  2.5f,  64 }, // height of  64
#endif
};

EXPORT const ULONG EmuBrush[EMUBRUSHES] =
{   EMUPEN_RED,
    EMUPEN_BLUE,
    EMUPEN_GREEN,
    EMUPEN_YELLOW,
    EMUPEN_ORANGE,
    EMUPEN_PURPLE,
    EMUPEN_CYAN,
    EMUPEN_GREY,
    EMUPEN_WHITE,
    EMUPEN_BLACK,
    EMUPEN_DARKRED,
    EMUPEN_DARKBLUE,
    EMUPEN_DARKGREEN,
    EMUPEN_DARKYELLOW,
    EMUPEN_DARKORANGE,
    EMUPEN_DARKPURPLE,
    EMUPEN_DARKCYAN,
    EMUPEN_PINK,
};

EXPORT const int menucode[MENUITEMS] = {
// "Project" menu (mostly)
    ID_FILE_RESET,
    ID_FILE_RESETTOMONITOR,
    ID_FILE_REINITIALIZE,
    ID_FILE_OPEN,
    ID_FILE_REVERT,
    ID_FILE_QUICKLOAD,
    ID_FILE_SAVESNP,
    ID_FILE_SAVEACBM,
    ID_FILE_SAVEILBM,
    ID_FILE_SAVEBMP,
    ID_FILE_SAVEGIF,             //  10
    ID_FILE_SAVEPCX,
    ID_FILE_SAVEPNG,
    ID_FILE_SAVETIFF,
    ID_FILE_SAVEASCII,
    ID_FILE_SAVEASM,
    ID_FILE_QUICKSAVE,
    ID_FILE_AUDIT,
    ID_FILE_REGISTER,
    ID_FILE_SERVER,
    ID_FILE_CLIENT,              //  20
    ID_FILE_RECENT1,
    ID_FILE_RECENT2,
    ID_FILE_RECENT3,
    ID_FILE_RECENT4,
    ID_FILE_RECENT5,
    ID_FILE_RECENT6,
    ID_FILE_RECENT7,
    ID_FILE_RECENT8,
    ID_FILE_ICONIFY,
    ID_FILE_UNICONIFY,           //  30
    ID_FILE_EXIT,
// "Edit" menu
    ID_EDIT_COPY,
    ID_EDIT_COPYTEXT,
    ID_EDIT_PASTETEXT,
    ID_EDIT_EMPTYCLIPBOARD,
// "View" menu
    ID_VIEW_HISCORES,
    ID_VIEW_DEBUGGER,
    ID_VIEW_MENU,
    ID_VIEW_POINTER,
    ID_VIEW_SIDEBAR,             //  40
    ID_VIEW_STATUS,
    ID_VIEW_TITLE,
    ID_VIEW_TOOL,
    ID_VIEW_BUILTIN,
    -1, // MENUFAKE_SORTBY
    -1, // MENUFAKE_SPEEDINDICATOR
// "Macross" ;-) menu
    ID_MACRO_STARTRECORDING,
    ID_MACRO_RESTARTPLAYBACK,
    ID_MACRO_STOP,
    ID_MACRO_LOOP,               //  50
    ID_DEBUG_WARN,
    ID_MACRO_IFFANIMS,
    ID_MACRO_GIF,
    ID_MACRO_MNG,
    ID_MACRO_APNG,
    ID_MACRO_8SVX,
    ID_MACRO_AIFF,
    ID_MACRO_SMUS,
    ID_MACRO_MIDI,
    ID_MACRO_PSGS,               //  60
    ID_MACRO_WAV,
    ID_MACRO_YM,
    ID_MACRO_RUNREXX,
    ID_MACRO_REPEATREXX,
// "Debug|General" submenu
    ID_DEBUG_QUOTES,
    ID_DEBUG_CLEARCOV,
    ID_DEBUG_CLS,
    ID_DEBUG_HELP,
    ID_DEBUG_SAY,
    ID_DEBUG_SYSTEM,             //  70
// "Debug|File »" submenu
    ID_DEBUG_ASTERISK,
    ID_DEBUG_ASM,
    ID_DEBUG_DEL,
    ID_DEBUG_DIR,
    ID_DEBUG_DISGAME,
    ID_DEBUG_EDIT,
    ID_DEBUG_EXTRACT,
    ID_DEBUG_LOAD,
    ID_DEBUG_SAVEAOF,
    ID_DEBUG_SAVE,               //  80
    ID_DEBUG_SAVEBPNF,
    ID_DEBUG_SAVECMD,
    ID_DEBUG_SAVEHEX,
    ID_DEBUG_SAVESMS,
    ID_DEBUG_SAVETVC,
    -1, // MENUFAKE_DRIVE
// "Debug|Edit »" submenu
    ID_DEBUG_DOKE,
    ID_DEBUG_POKE,
    ID_DEBUG_FPOKE,
    ID_DEBUG_WRITEPORT,          //  90
// "Debug|View »" submenu
    ID_DEBUG_EQUALS,
    ID_DEBUG_COVER,
    ID_DEBUG_PEEK,
    ID_DEBUG_DIS,
    ID_DEBUG_ERROR,
    ID_DEBUG_FPEEK,
    ID_DEBUG_HISTORY,
    ID_DEBUG_IM,
    ID_DEBUG_READPORT,
    ID_DEBUG_VIEW_BASIC,         // 100
    ID_DEBUG_VIEW_BIOS,
    ID_DEBUG_VIEW_CPU,
    ID_DEBUG_VIEW_PSG,
    ID_DEBUG_VIEW_RAM,
    ID_DEBUG_VIEW_SCRN,
    ID_DEBUG_VIEW_UDG,
    ID_DEBUG_VIEW_XVI,
// "Debug|Log »" submenu
    ID_DEBUG_L_A,
    ID_DEBUG_L_B,
    ID_DEBUG_L_C,                // 110
    ID_DEBUG_L_I,
    ID_DEBUG_L_N,
    ID_DEBUG_L_S,
    ID_DEBUG_PL,
    ID_DEBUG_T,
// "Debug|Run »" submenu
    ID_DEBUG_G,
    ID_DEBUG_GI,
    ID_DEBUG_I,
    ID_DEBUG_JUMP,
    ID_DEBUG_O,                  // 120
    ID_DEBUG_S,
    ID_DEBUG_R,
    ID_DEBUG_R_F,
    ID_DEBUG_R_I,
    ID_DEBUG_R_L,
    ID_DEBUG_R_R,
    ID_DEBUG_R_S,
// "Debug|Breakpoints »" submenu
    ID_DEBUG_BP,
    ID_DEBUG_BC,
    ID_DEBUG_BL,                 // 130
    ID_DEBUG_FP,
    ID_DEBUG_FC,
    ID_DEBUG_FL,
    ID_DEBUG_IP,
    ID_DEBUG_IC,
    ID_DEBUG_IL,
    ID_DEBUG_WP,
    ID_DEBUG_WC,
    ID_DEBUG_WL,
    ID_DEBUG_PB,                 // 140
    ID_DEBUG_WR,
    -1, // MENUFAKE_WW
// "Debug|Symbols »" submenu
    ID_DEBUG_CLEARSYM,
    ID_DEBUG_DEFSYM,
    ID_DEBUG_LOADSYM,
    ID_DEBUG_SAVESYM,
// "Debug|Tools »" submenu
    ID_DEBUG_COMP,
    ID_DEBUG_COPY,
    ID_DEBUG_FILL,
    ID_DEBUG_FIND,               // 150
    ID_DEBUG_REL,
    ID_DEBUG_SWAP,
    ID_DEBUG_TRAIN,
// "Debug|Options »" submenu
    -1, // MENUFAKE_BASE
    -1, // MENUFAKE_CPU
    -1, // MENUFAKE_N
    -1, // MENUFAKE_TU
    -1, // MENUFAKE_VERBOSITY
    ID_DEBUG_GR,
// "Debug|Graphics »" submenu
    -1, // MENUFAKE_DRAW,        // 160
    -1, // MENUFAKE_SPR
// "Debug" menu
    ID_TOOLS_DEBUGGER,
// "Tools" menu
    ID_TOOLS_MEMORY,
    ID_TOOLS_MUSIC,
    ID_TOOLS_SCREENEDITOR,
    ID_TOOLS_SPRITES,
    ID_TOOLS_MONITOR_CPUS,
    ID_TOOLS_MONITOR_PSGS,
    ID_TOOLS_MONITOR_XVI,
    ID_TOOLS_CHEEVOS,            // 170
// "Peripherals" menu
    ID_TOOLS_CONTROLS,
    ID_TOOLS_DIPS,
// "Peripherals|Left controller »" submenu
    -1, // MENUFAKE_LEFT
    ID_LEFT_AUTOFIRE,
    ID_LEFT_REQUIREBUTTON,
    ID_LEFT_FORCEFEEDBACK,
// "Peripherals|Right controller »" submenu
    -1, // MENUFAKE_RIGHT
    ID_RIGHT_AUTOFIRE,
    ID_RIGHT_REQUIREBUTTON,
    ID_RIGHT_FORCEFEEDBACK,      // 180
// "Peripherals" menu
    ID_TOOLS_TAPEDECK,
    ID_TOOLS_FLOPPYDRIVE,
    ID_TOOLS_PAPERTAPE,
    ID_TOOLS_PRINTER,
// "Settings|BIOS »" submenu
    -1, // MENUFAKE_ELEKTORBIOS
    -1, // MENUFAKE_PIPBUGBIOS
    -1, // MENUFAKE_BINBUGBIOS
    -1, // MENUFAKE_CD2650BIOS
    -1, // MENUFAKE_PHUNSYBIOS
    -1, // MENUFAKE_SELBSTBIOS      190
// "Settings|Colours »" submenu
    ID_TOOLS_PALETTE,
    ID_COLOURS_ARTEFACTS,
    ID_COLOURS_DARKENBG,
    ID_GRAPHICS_FLAGLINE,
    ID_COLOURS_INVERSE,
    -1, // MENUFAKE_COLOURSET
// "Settings|DOS »" submenu
    -1, // MENUFAKE_BINBUGDOS
    -1, // MENUFAKE_TWINDOS
    -1, // MENUFAKE_CD2650DOS
// "Settings|Emulator »" submenu
    ID_EMULATOR_AUTOSAVE,        // 200
    ID_EMULATOR_STARTUPUPDATES,
    ID_EMULATOR_CONFIRM,
    -1, // MENUITEM_CREATEICONS
    ID_LOG_CONSOLE,
    ID_EMULATOR_EMUID,
    ID_EMULATOR_POST,
    ID_EMULATOR_RANDOMIZE,
    ID_EMULATOR_SENSEGAME,
    ID_EMULATOR_SHOWTOD,
    ID_EMULATOR_USESTUBS,        // 210
    -1, // MENUFAKE_LOGTOFILE
    ID_EMULATOR_PATHS,
// "Settings|Filters »" submenu
    -1, // MENUFAKE_FILTER
    ID_GRAPHICS_SCANLINES,
    ID_FILTERS_STRETCHWINDOWED,
    -1, // MENUFAKE_STRETCHING,
// "Settings|Graphics »" submenu
    -1, // MENUFAKE_SIZE
    ID_GRAPHICS_DEJITTER,
    ID_GRAPHICS_SKIES,
    ID_GRAPHICS_CORNERS,         // 220
    ID_GRAPHICS_FULLSCREEN,
    ID_GRAPHICS_NARROW,
    -1, // spare
    ID_GRAPHICS_UNLIT,
    ID_GRAPHICS_SHOWLEDS,
// "Settings|Input »" submenu
    ID_INPUT_ANALOG,
    ID_INPUT_SHOWPOSITIONS,
    ID_INPUT_SPRINGLOADED,
    ID_INPUT_SWAPPED,
    ID_INPUT_CALIBRATE,          // 230
    ID_INPUT_REARRANGE,
    ID_INPUT_REDEFINE,
    ID_INPUT_SENSITIVITY,
    ID_INPUT_LOWERCASE,
    ID_INPUT_CONFINE,
    ID_INPUT_AUTOCOIN,
    ID_INPUT_GUESTRMB,
    ID_INPUT_QUEUEKEYSTROKES,
    -1, // MENUFAKE_KEYMAP
// "Settings|Language »" submenu
    -1, // MENUFAKE_LANGUAGE     // 240
// "Settings|Machine »" submenu
    -1, // MENUFAKE_MACHINE
// "Settings|Sound »" submenu
    ID_SOUND_ENABLED,
    ID_SOUND_ADJUST,
    ID_SOUND_AMBIENT,
    -1, // MENUITEM_FILTERED
    ID_SOUND_RETUNE,
    -1, // MENUFAKE_SOUNDOUTPUT
// "Settings|Speed »" submenu
    ID_SPEED_ADJUST,
    ID_SPEED_PAUSED,
    ID_SPEED_AUTOPAUSE,          // 250
    ID_SPEED_LIMIT,
    ID_SPEED_TURBO,
    -1, // MENUFAKE_REGION
    -1, // MENUFAKE_PRIORITY
// "Settings|Sprites »" submenu
    ID_SPRITES_COLLISIONS,
    ID_SPRITES_DEMULTIPLEX,
// "Settings|Trainers »" submenu
    ID_CHEATS_LIVES,
    ID_CHEATS_TIME,
    ID_CHEATS_INVINCIBILITY,
    ID_CHEATS_LEVELSKIP,         // 260
// "Settings|VDU »" submenu
    ID_GRAPHICS_USEMARGINS,
    ID_VDU_BLINK,
    ID_VDU_COOMER,
    ID_GRAPHICS_ROTATE,
    -1, // MENUFAKE_PIPBUGVDU
    -1, // MENUFAKE_BINBUGVDU
    -1, // MENUFAKE_CD2650VDU
// "Help" menu
    ID_HELP_GAMEINFO,
    ID_HELP_PADS,
    ID_HELP_KYBD,                // 270
    ID_HELP_OPCODES,
    ID_HELP_CODINGGUIDE,
    ID_HELP_GAMINGGUIDE,
    ID_HELP_MANUAL,
    ID_HELP_UPDATE,
    -1, // MENUITEM_REACTION
    ID_HELP_ABOUT,
// menus
    -1, // MENUMENU_PROJECT
    -1, // MENUMENU_EDIT
    -1, // MENUMENU_VIEW            280
    -1, // MENUMENU_MACRO
    -1, // MENUMENU_DEBUG
    -1, // MENUMENU_TOOLS
    -1, // MENUMENU_PERIPHERALS,
    -1, // MENUMENU_SETTINGS
    -1, // MENUMENU_HELP
// submenus
    -1, // MENUMENU_DEBUG_GENERAL
    -1, // MENUMENU_DEBUG_FILE
    -1, // MENUMENU_DEBUG_EDIT
    -1, // MENUMENU_DEBUG_VIEW      290
    -1, // MENUMENU_DEBUG_LOG
    -1, // MENUMENU_DEBUG_RUN
    -1, // MENUMENU_DEBUG_BP
    -1, // MENUMENU_DEBUG_SYMBOLS
    -1, // MENUMENU_DEBUG_TOOLS
    -1, // MENUMENU_DEBUG_OPTIONS
    -1, // MENUMENU_DEBUG_GRAPHICS
    -1, // MENUMENU_LEFT
    -1, // MENUMENU_RIGHT
    -1, // MENUMENU_BIOS            300
    -1, // MENUMENU_DOS
    -1, // MENUMENU_FILTERS
    -1, // MENUMENU_MACHINE
    -1, // MENUMENU_SPRITES
    -1, // MENUMENU_TRAINERS
    -1, // MENUMENU_VDU             306
// new ones
    -1, // MENUFAKE_FRAMEBASED      307
    ID_DEBUG_INJECT,             // 308
    ID_DEBUG_REN,                // 309
    -1, // MENUMENU_DEBUG_DISK      310
    ID_SPEED_EXACT,              // 311
    ID_MACRO_GENERATE,           // 312
    -1, // MENUMENU_MACRO_ANIMS     313
    -1, // MENUMENU_MACRO_SOUNDS    314
    ID_VDU_BEZEL,                // 315
    ID_EMULATOR_CHEEVOS,         // 316
    ID_INPUT_ERASEDEL,           // 317
    ID_PERIPHERALS_INDUSTRIAL,   // 318
}, menuopt[MENUOPTS] = {
    ID_SORTBY_NAME,              //   0 "View" menu
    ID_SORTBY_MACHINE,
    ID_INDICATOR_FPS,
    ID_INDICATOR_KHZ,
    ID_INDICATOR_PERCENT,
    ID_DEBUG_DRIVE_0,            //   5 "Debug|File »" submenu
    ID_DEBUG_DRIVE_1,
    ID_DEBUG_WW_NONE,            //   7 "Debug|Breakpoints »" submenu
    ID_DEBUG_WW_SOME,
    ID_DEBUG_WW_ALL,
    ID_DEBUG_BASE_BINARY,        //  10 "Debug|Options »" submenu
    ID_DEBUG_BASE_OCTAL,
    ID_DEBUG_BASE_DECIMAL,
    ID_DEBUG_BASE_HEX,
    ID_DEBUG_CPU_0,
    ID_DEBUG_CPU_1,
    ID_DEBUG_N_0,
    ID_DEBUG_N_1,
    ID_DEBUG_N_2,
    ID_DEBUG_N_3,
    ID_DEBUG_TU_0,
    ID_DEBUG_TU_1,
    ID_DEBUG_TU_2,
    ID_DEBUG_VB_0,
    ID_DEBUG_VB_1,
    ID_DEBUG_VB_2,
    ID_DEBUG_DRAW_0,             //  26 "Debug|Graphics »" submenu
    ID_DEBUG_DRAW_1,
    ID_DEBUG_DRAW_2,
    ID_DEBUG_DRAW_3,
    ID_DEBUG_DRAW_4,
    ID_DEBUG_SPR_0,
    ID_DEBUG_SPR_1,
    ID_DEBUG_SPR_2,
    ID_LEFT_TRACKBALL,           //  34 "Peripherals|Left controller »" submenu
    ID_LEFT_NONE,
    ID_LEFT_1STDJOY,
    ID_LEFT_2NDDJOY,
    -1, // MENUOPT_LEFT_2NDAJOY
    -1, // MENUOPT_LEFT_1STDPAD
    -1, // MENUOPT_LEFT_2NDDPAD
    -1, // MENUOPT_LEFT_1STAPAD
    -1, // MENUOPT_LEFT_2NDAPAD
    ID_RIGHT_TRACKBALL,          //  43 "Peripherals|Right controller »" submenu
    ID_RIGHT_NONE,
    ID_RIGHT_1STDJOY,
    ID_RIGHT_2NDDJOY,
    -1, // MENUOPT_RIGHT_2NDAJOY
    -1, // MENUOPT_RIGHT_1STDPAD
    -1, // MENUOPT_RIGHT_2NDDPAD
    -1, // MENUOPT_RIGHT_1STAPAD
    -1, // MENUOPT_RIGHT_2NDAPAD
    ID_BIOS_PHILIPS,             //  52 "Settings|BIOS »" submenu
    ID_BIOS_HOBBYMODULE,
    ID_BIOS_ARTEMIS,
    ID_BIOS_HYBUG,
    ID_BIOS_PIPBUG,
    ID_BIOS_PIPBUG2,
    ID_BIOS_BINBUG35,
    ID_BIOS_BINBUG36,
    ID_BIOS_BINBUG61,
    ID_BIOS_GBUG,
    ID_BIOS_MIKEBUG,
    ID_BIOS_MYBUG,
    ID_BIOS_IPL,
    ID_BIOS_POPMON,
    ID_BIOS_SUPERVISOR,
    ID_BIOS_MINIMONITOR,
    ID_BIOS_PHUNSY,
    ID_BIOS_SELBST00,
    ID_BIOS_SELBST09,
    ID_BIOS_SELBST10,
    ID_BIOS_SELBST20,
    ID_COLOURS_AMBER,            //  73 "Settings|Colours »" submenu
    ID_COLOURS_GREEN,
    ID_COLOURS_GREYSCALE,
    ID_COLOURS_PURE,
    ID_COLOURS_PVI,
    ID_COLOURS_UVI,
    ID_DOS_MICRODOS,             //  79 "Settings|DOS »" submenu
    ID_DOS_VHSDOS,
    ID_DOS_NOBINBUGDOS,
    ID_DOS_EXOS,
    ID_DOS_SDOS20,
    ID_DOS_SDOS40,
    ID_DOS_SDOS42,
    ID_DOS_TOS,
    ID_DOS_UDOS,
    ID_DOS_NOTWINDOS,
    ID_DOS_CDDOS,
    ID_DOS_P1DOS,
    ID_DOS_NOCD2650DOS,
    ID_LOG_APPEND,               //  92 "Settings|Emulator »" submenu
    ID_LOG_IGNORE,
    ID_LOG_REPLACE,
    ID_FILTERS_3D,               //  95 "Settings|Filters »" submenu
    ID_FILTERS_HQX,
    ID_FILTERS_SCALE2X,
    ID_FILTERS_NONE,
    ID_FILTERS_UNSTRETCHED,
    ID_FILTERS_STRETCH43,
    ID_FILTERS_STRETCHTOFIT,
    ID_GRAPHICS_1XSIZE,          // 102 "Settings|Graphics »" submenu
    ID_GRAPHICS_2XSIZE,
    ID_GRAPHICS_3XSIZE,
    ID_GRAPHICS_4XSIZE,
    ID_GRAPHICS_5XSIZE,
    ID_GRAPHICS_6XSIZE,
    ID_INPUT_HOSTLAYOUT,         // 108 "Settings|Input »" submenu
    ID_INPUT_EALAYOUT,
    ID_INPUT_TTLAYOUT,
    ID_LANGUAGE_ENG,             // 111 "Settings|Language »" submenu
    ID_LANGUAGE_HOL,
    ID_LANGUAGE_FRA,
    ID_LANGUAGE_GER,
    ID_LANGUAGE_GRE,
    ID_LANGUAGE_ITA,
    ID_LANGUAGE_POL,
    ID_LANGUAGE_RUS,
    ID_LANGUAGE_SPA,
    ID_MACHINE_ARCADIA,          // 120 "Settings|Machine »" submenu
    ID_MACHINE_INTERTON,
    ID_MACHINE_ELEKTOR,
    ID_MACHINE_PIPBUG,
    ID_MACHINE_BINBUG,
    ID_MACHINE_INSTRUCTOR,
    ID_MACHINE_TWIN,
    ID_MACHINE_CD2650,
    ID_MACHINE_PHUNSY,
    ID_MACHINE_SELBST,
    ID_MACHINE_MIKIT,
    ID_MACHINE_ASTROWARS,
    ID_MACHINE_GALAXIA,
    ID_MACHINE_LASERBATTLE,
    ID_MACHINE_LAZARIAN,
    ID_MACHINE_M1,
    ID_MACHINE_M2,
    ID_MACHINE_8550,
    ID_MACHINE_8600,
    ID_MACHINE_TYPERIGHT,
    -1, // MENUOPT_SOUND_AHI     // 140 "Settings|Sound »" submenu
    -1, // MENUOPT_SOUND_PAULA
    ID_SPEED_NTSC,               // 142 "Settings|Speed »" submenu
    ID_SPEED_PAL,
    ID_SPEED_HIGHPRI,
    ID_SPEED_NORMALPRI,
    ID_SPEED_LOWPRI,
    ID_VDU_ELEKTERMINAL,         // 147 "Settings|VDU »" submenu
    ID_VDU_LCVDU_NARROW,
    ID_VDU_LCVDU_WIDE,
    ID_VDU_RADIOBULLETIN,        // 150
    ID_VDU_SVT100,               // 151
    ID_VDU_VT100,
    ID_VDU_ASCII,
    ID_VDU_CHESSMEN,
    ID_VDU_LOWERCASE,            // 155
// new ones
    ID_DEBUG_DRIVE_2,            // 156
    ID_DEBUG_DRIVE_3,            // 157
    ID_DEMULTIPLEX_MULTIPLEX,    // 158
    ID_DEMULTIPLEX_TRANSPARENT,  // 159
    ID_DEMULTIPLEX_OPAQUE,       // 160
    ID_EMULATOR_FRAMEBASED,      // 161
    ID_EMULATOR_PIXELBASED,      // 162
    ID_DEBUG_N_4,                // 163
};

// MODULE VARIABLES ------------------------------------------------------

MODULE FLAG                    cheevos_reopen      = FALSE,
                               emulating           = FALSE,
                               iconified           = FALSE,
                               ignorelmb           = FALSE,
                               lockconfig          = FALSE,
                               menuhelp            = FALSE,
                               override_fullscreen = FALSE,
                               showingpointer      = TRUE;
// MODULE FLAG                 win8                = FALSE;
MODULE TEXT                    coinstring[2][40 + 1];
MODULE UINT                    storedaltcode       = 0;
MODULE int                     highestid           = 0,
                               historyline         = -1,
                               minwinwidth,
#ifdef MEASURECAPSLOCK
                               measurement,
#endif
                               newgame             = -1,
                            // promptheight,
                               setting_fullscreen;
MODULE STRPTR                  error;
MODULE HANDLE                  ProcessPtr          = NULL,
                               MainThreadPtr       = NULL,
                               SubThreadPtr        = NULL;
MODULE HGLOBAL                 hEditDS             = NULL;
MODULE HHOOK                   g_hKeyboardHook     = NULL;
MODULE HBITMAP                 smlbitmap[2][8];
MODULE HICON                   hicon               = NULL;
MODULE HIMAGELIST              himl;
MODULE HMENU                   SystemMenuPtr       = NULL,
                               PopUpMenuPtr        = NULL,
                               TrayMenuPtr         = NULL;
MODULE WNDPROC                 wpOldEditProc; // Stores the old original window procedure for the edit control

#define MIBS 106 // counting from 1 (ie. items go from 0..MIBS-1)
MODULE struct
{   const DWORD   menuitem,
                  resource;
    const FLAG    mx;
    const SBYTE   column;
// uninitialized
          HBITMAP bitmap;
} mib[MIBS] =
{ { ID_FILE_RESET           , IDB_M_RESET         , FALSE, -1      }, //  0
  { ID_FILE_OPEN            , IDB_M_OPEN          , FALSE, -1      },
  { ID_FILE_QUICKLOAD       , IDB_M_QUICKLOAD     , FALSE, -1      },
  { ID_FILE_SAVESNP         , IDB_M_SAVESNP       , FALSE, -1      },
  { ID_FILE_SAVEBMP         , IDB_M_SAVEBMP       , FALSE, -1      },
  { ID_FILE_QUICKSAVE       , IDB_M_QUICKSAVE     , FALSE, -1      },
  { ID_FILE_SERVER          , IDB_M_SERVER        , FALSE, -1      },
  { ID_FILE_CLIENT          , IDB_M_CLIENT        , FALSE, -1      },
  { ID_FILE_ICONIFY         , IDB_M_ICONIFY       , FALSE, -1      }, //  8 MIB_ICONIFY
  { ID_FILE_EXIT            , IDB_M_EXIT          , FALSE, -1      }, //  9 MIB_EXIT
  { ID_EDIT_COPY            , IDB_M_COPY          , FALSE, -1      }, // 10
  { ID_EDIT_PASTETEXT       , IDB_M_PASTE         , FALSE, -1      },
  { ID_EDIT_EMPTYCLIPBOARD  , IDB_M_EMPTYCLIPBOARD, FALSE, -1      },
  { ID_VIEW_HISCORES        , IDB_M_HISCORES      , FALSE, -1      },
  { ID_VIEW_DEBUGGER        , IDB_M_DEBUGGER      , FALSE, -1      },
  { IN_SORTBY               , IDB_M_SORTBY        , FALSE, MN_VIEW },
  { ID_MACRO_STARTRECORDING , IDB_M_RECORD        , FALSE, -1      },
  { ID_MACRO_RESTARTPLAYBACK, IDB_M_PLAY          , FALSE, -1      },
  { ID_MACRO_STOP           , IDB_M_STOP          , FALSE, -1      },
  { ID_MACRO_LOOP           , IDB_M_LOOP          , FALSE, -1      },
  { ID_MACRO_RUNREXX        , IDB_M_REXX          , FALSE, -1      }, // 20
  { ID_TOOLS_PRINTER        , IDB_M_PRINTER       , FALSE, -1      },
  { ID_TOOLS_DEBUGGER       , IDB_M_DEBUGGER      , FALSE, -1      },
  { ID_LEFT_AUTOFIRE        , IDB_M_AUTOFIRE1     , TRUE , -1      },
  { ID_RIGHT_AUTOFIRE       , IDB_M_AUTOFIRE2     , TRUE , -1      },
  { ID_BIOS_PHILIPS         , IDB_M_ELEKTOR       , TRUE , -1      },
  { ID_BIOS_HOBBYMODULE     , IDB_M_ELEKTOR       , TRUE , -1      },
  { ID_BIOS_PIPBUG          , IDB_M_PIPBUG        , TRUE , -1      },
  { ID_BIOS_PIPBUG2         , IDB_M_PIPBUG        , TRUE , -1      },
  { ID_BIOS_HYBUG           , IDB_M_PIPBUG        , TRUE , -1      },
  { ID_DOS_EXOS             , IDB_M_TWIN          , TRUE , -1      }, // 30
  { ID_DOS_SDOS20           , IDB_M_TWIN          , TRUE , -1      },
  { ID_DOS_SDOS40           , IDB_M_TWIN          , TRUE , -1      },
  { ID_DOS_SDOS42           , IDB_M_TWIN          , TRUE , -1      },
  { ID_DOS_TOS              , IDB_M_TWIN          , TRUE , -1      },
  { ID_DOS_UDOS             , IDB_M_TWIN          , TRUE , -1      },
  { ID_BIOS_IPL             , IDB_M_CD2650        , TRUE , -1      },
  { ID_BIOS_POPMON          , IDB_M_CD2650        , TRUE , -1      },
  { ID_BIOS_SUPERVISOR      , IDB_M_CD2650        , TRUE , -1      },
  { ID_BIOS_MINIMONITOR     , IDB_M_PHUNSY        , TRUE , -1      },
  { ID_BIOS_PHUNSY          , IDB_M_PHUNSY        , TRUE , -1      }, // 40
  { ID_BIOS_SELBST00        , IDB_M_SELBST        , TRUE , -1      },
  { ID_BIOS_SELBST09        , IDB_M_SELBST        , TRUE , -1      },
  { ID_BIOS_SELBST10        , IDB_M_SELBST        , TRUE , -1      },
  { ID_BIOS_SELBST20        , IDB_M_SELBST        , TRUE , -1      },
  { ID_TOOLS_PALETTE        , IDB_M_PALETTE       , FALSE, -1      },
  { ID_GRAPHICS_FULLSCREEN  , IDB_M_FULLSCREEN    , FALSE, -1      },
  { ID_INPUT_SWAPPED        , IDB_M_SWAPPED       , TRUE , -1      },
  { ID_LANGUAGE_ENG         , IDB_M_ENG           , TRUE , -1      },
  { ID_LANGUAGE_HOL         , IDB_M_HOL           , TRUE , -1      },
  { ID_LANGUAGE_FRA         , IDB_M_FRA           , TRUE , -1      }, // 50
  { ID_LANGUAGE_GER         , IDB_M_GER           , TRUE , -1      },
  { ID_LANGUAGE_GRE         , IDB_M_GRE           , TRUE , -1      },
  { ID_LANGUAGE_ITA         , IDB_M_ITA           , TRUE , -1      },
  { ID_LANGUAGE_POL         , IDB_M_POL           , TRUE , -1      },
  { ID_LANGUAGE_RUS         , IDB_M_RUS           , TRUE , -1      },
  { ID_LANGUAGE_SPA         , IDB_M_SPA           , TRUE , -1      },
  { ID_MACHINE_ARCADIA      , IDB_M_ARCADIA       , TRUE , -1      }, // 57 MIB_ARCADIA
  { ID_MACHINE_INTERTON     , IDB_M_INTERTON      , TRUE , -1      }, // 58 MIB_INTERTON
  { ID_MACHINE_ELEKTOR      , IDB_M_ELEKTOR       , TRUE , -1      }, // 59 MIB_ELEKTOR
  { ID_MACHINE_PIPBUG       , IDB_M_PIPBUG        , TRUE , -1      }, // 60 MIB_PIPBUG
  { ID_MACHINE_BINBUG       , IDB_M_BINBUG        , TRUE , -1      }, // 61 MIB_BINBUG
  { ID_MACHINE_INSTRUCTOR   , IDB_M_SI50          , TRUE , -1      }, // 62 MIB_INSTRUCTOR
  { ID_MACHINE_TWIN         , IDB_M_TWIN          , TRUE , -1      }, // 63 MIB_TWIN
  { ID_MACHINE_CD2650       , IDB_M_CD2650        , TRUE , -1      }, // 64 MIB_CD2650
  { ID_MACHINE_PHUNSY       , IDB_M_PHUNSY        , TRUE , -1      }, // 65 MIB_PHUNSY
  { ID_MACHINE_SELBST       , IDB_M_SELBST        , TRUE , -1      }, // 66 MIB_SELBST
  { ID_MACHINE_MIKIT        , IDB_M_MIKIT         , TRUE , -1      }, // 67 MIB_MIKIT
  { ID_MACHINE_ASTROWARS    , IDB_M_ASTROWARS     , TRUE , -1      }, // 68 MIB_ASTROWARS
  { ID_MACHINE_GALAXIA      , IDB_M_GALAXIA       , TRUE , -1      }, // 69 MIB_GALAXIA
  { ID_MACHINE_LASERBATTLE  , IDB_M_LASERBATTLE   , TRUE , -1      }, // 70 MIB_LASERBATTLE
  { ID_MACHINE_LAZARIAN     , IDB_M_LASERBATTLE   , TRUE , -1      }, // 71 MIB_LAZARIAN
  { ID_MACHINE_M1           , IDB_M_MALZAK        , TRUE , -1      }, // 72 MIB_M1
  { ID_MACHINE_M2           , IDB_M_MALZAK        , TRUE , -1      }, // 73 MIB_M2
  { ID_MACHINE_8550         , IDB_M_PONG          , TRUE , -1      }, // 74 MIB_8550
  { ID_MACHINE_8600         , IDB_M_PONG          , TRUE , -1      }, // 75 MIB_8600
  { ID_MACHINE_TYPERIGHT    , IDB_M_TYPERIGHT     , TRUE , -1      }, // 76 MIB_TYPERIGHT
  { ID_SOUND_ENABLED        , IDB_M_SOUND         , TRUE , -1      }, // 77
  { ID_SPEED_PAUSED         , IDB_M_PAUSED        , TRUE , -1      }, // 78
  { ID_SPEED_TURBO          , IDB_M_TURBO         , TRUE , -1      }, // 79
  { ID_SPRITES_COLLISIONS   , IDB_M_COLLISIONS    , TRUE , -1      }, // 80
  { ID_HELP_OPCODES         , IDB_M_TABLE         , FALSE, -1      }, // 81
  { ID_HELP_MANUAL          , IDB_M_MANUAL        , FALSE, -1      }, // 82
  { ID_HELP_UPDATE          , IDB_M_UPDATE        , FALSE, -1      }, // 83
  { ID_HELP_ABOUT           , IDB_M_ABOUT         , FALSE, -1      }, // 84
  { ID_FILE_UNICONIFY       , IDB_M_UNICONIFY     , FALSE, -1      }, // 85
// new ones
  { ID_BIOS_BINBUG35        , IDB_M_BINBUG        , TRUE , -1      }, // 86
  { ID_BIOS_BINBUG36        , IDB_M_BINBUG        , TRUE , -1      }, // 87
  { ID_BIOS_BINBUG61        , IDB_M_BINBUG        , TRUE , -1      }, // 88
  { ID_BIOS_GBUG            , IDB_M_BINBUG        , TRUE , -1      }, // 89
  { ID_BIOS_MIKEBUG         , IDB_M_BINBUG        , TRUE , -1      }, // 90
  { ID_BIOS_MYBUG           , IDB_M_BINBUG        , TRUE , -1      }, // 91
  { ID_DOS_MICRODOS         , IDB_M_BINBUG        , TRUE , -1      }, // 92
  { ID_DOS_VHSDOS           , IDB_M_BINBUG        , TRUE , -1      }, // 93
  { ID_DOS_NOBINBUGDOS      , IDB_M_BINBUG        , TRUE , -1      }, // 94
  { ID_DOS_NOTWINDOS        , IDB_M_TWIN          , TRUE , -1      }, // 95
  { ID_DOS_CDDOS            , IDB_M_CD2650        , TRUE , -1      }, // 96
  { ID_DOS_P1DOS            , IDB_M_CD2650        , TRUE , -1      }, // 97
  { ID_DOS_NOCD2650DOS      , IDB_M_CD2650        , TRUE , -1      }, // 98
  { ID_VDU_ELEKTERMINAL     , IDB_M_PIPBUG        , TRUE , -1      }, // 99
  { ID_VDU_ASCII            , IDB_M_CD2650        , TRUE , -1      }, // 100
  { ID_VDU_LCVDU_NARROW     , IDB_M_PIPBUG        , TRUE , -1      }, // 101
  { ID_VDU_LCVDU_WIDE       , IDB_M_PIPBUG        , TRUE , -1      }, // 102
  { ID_VDU_VT100            , IDB_M_PIPBUG        , TRUE , -1      }, // 103
  { ID_BIOS_ARTEMIS         , IDB_M_PIPBUG        , TRUE , -1      }, // 104
  { ID_TOOLS_MUSIC          , IDB_M_MUSIC         , FALSE, -1      }, // 105
};

MODULE const int gadgetcode[] =
{   ID_LEFT_AUTOFIRE,         // 0
    ID_RIGHT_AUTOFIRE,        // 1
    ID_INPUT_SWAPPED,         // 2
    ID_SOUND_ENABLED,         // 3
    ID_SPEED_PAUSED,          // 4
    ID_SPEED_TURBO,           // 5
    ID_SPRITES_COLLISIONS,    // 6
    ID_MACRO_STARTRECORDING,  // 7
    ID_MACRO_RESTARTPLAYBACK, // 8
    ID_MACRO_STOP,            // 9
};

// IMPORTED VARIABLES ----------------------------------------------------

IMPORT       FLAG                        aborting,
                                         consoleopened,
                                         loadedconfig,
                                         modal,
                                         repaintmemmap,
                                         softctrl,
                                         softlshift,
                                         softrshift;
IMPORT       UBYTE                       cheevomem[12 * KILOBYTE],
                                         guestkey,
                                         editcolour,
                                         memory[32768],
                                         oldkeys[KEYS],
                                         psu,
                                         startaddr_h,
                                         startaddr_l;
IMPORT       TEXT                        arg1[MAX_PATH + 80 + 1],
                                         arg2[MAX_PATH + 80 + 1],
                                         arg3[MAX_PATH + 80 + 1],
                                         arg4[MAX_PATH + 80 + 1],
                                         arg5[MAX_PATH + 80 + 1],
                                         arg6[MAX_PATH + 80 + 1],
                                         arg7[MAX_PATH + 80 + 1],
                                         autotext[GAMEINFOLINES][80 + 1],
                                         datetimestring[40 + 1],
                                         decpoint,
                                         file_game[MAX_PATH + 1],
                                         fn_asm[MAX_PATH + 1],
                                         fn_bkgrnd[MAX_PATH + 1],
                                         fn_config[MAX_PATH + 1],
                                         fn_game[MAX_PATH + 1], // the entire pathname (path and file)
                                         fn_screenshot[MAX_PATH + 1],
                                         fn_script[MAX_PATH + 1],
                                         groupsep,
                                         joyname[2][MAX_PATH],
                                         olduserinput[HISTORYLINES][MAX_PATH + 80 + 1],
                                         path_disks[MAX_PATH + 1],
                                         path_games[MAX_PATH + 1],
                                         path_projects[MAX_PATH + 1],
                                         path_screenshots[MAX_PATH + 1],
                                         path_scripts[MAX_PATH + 1],
                                         path_tapes[MAX_PATH + 1],
                                         recent[8][MAX_PATH + 1],
                                         sprad[16][5 + 1],
                                         StatusText[4][80 + 1],
                                         thequeue[QUEUESIZE],
                                         titlebartext[MAX_PATH + TITLEBARTEXTLENGTH],
                                         userinput[MAX_PATH + 80 + 1],
                                         usermsg[80 + 1];
IMPORT       UWORD                       hostkey,
                                         console[4],
                                         keypads[2][NUMKEYS],
                                         loadaddress,
                                         mirror_r[32768],
                                         mirror_w[32768],
                                         netport;
IMPORT       ULONG                       analog,
                                         autofire[2],
                                         collisions,
                                         frames,
                                         paused,
                                         region,
                                         sound,
                                         swapped,
                                         turbo,
                                         viewkybdas2;
IMPORT       int                         ambient,
                                         apnganims,
                                         ax[2],
                                         ay[4],
                                         autopause,
                                         autosave,
                                         base,
                                         bezel,
                                         cd2650_biosver,
                                         cheevosize,
                                         colourset,
                                         confine,
                                         connected,
                                         console_start,
                                         console_a,
                                         console_b,
                                         console_reset,
                                         crippled,
                                         editscreen,
                                         errors,
                                         fastsize,
                                         filesize,
                                         filter,
                                         found_dirs,
                                         foundgames,
                                         frameskip,
                                         fullscreen,
                                         game,
                                         generate,
                                         gifanims,
                                         guestrmb,
                                         gx[2], gy[2],
                                         iffanims,
                                         keymap,
                                         lowercase,
                                         machine,
                                         memmap,
                                         mnganims,
                                         offset,
                                         pipbug_biosver,
                                         phunsy_biosver,
                                         pipbug_vdu,
                                         pong_variant,
                                         queuekeystrokes,
                                         queuepos,
                                         randomizememory,
                                         recentgame[8],
                                         recents,
                                         recentmemmap[8],
                                         recmode,
                                         retune,
                                         rotating,
                                         s_io,
                                         s_id,
                                         s_intdir,
                                         scale2x,
                                         scrnaddr,
                                         selbst_biosver,
                                         sensegame,
                                         showdebugger[2],
                                         showleds,
                                         showmenubars[2],
                                         showpointers[2],
                                         showsidebars[2],
                                         showstatusbars[2],
                                         showtitlebars[2],
                                         showtod,
                                         showtoolbars[2],
                                         sidebar[FOUNDGAMES_MAX],
                                         size,
                                         Socket,
                                         sortby,
                                         speedup,
                                         spritesvisible,
                                         startupupdates,
                                         step,
                                         stretching,
                                         timeunit,
                                         trace,
                                         trainer_invincibility,
                                         trainer_lives,
                                         trainer_time,
                                         usemargins,
                                         usespeech,
                                         warnings,
                                         wsm,
                                         whichgame,
                                         wide;
IMPORT       STRPTR                      timeunitstr1,
                                         timeunitstr2;
IMPORT       UBYTE                      *IOBuffer,
                                        *pixelubyte;
IMPORT       ULONG                      *pixelulong,
                                        *RGBtoYUV;
IMPORT       RECT                        therect;
IMPORT       FILE*                       DisHandle;
IMPORT       struct AuditStruct          auditlist[FOUNDGAMES_MAX];
IMPORT       struct KeyNameStruct        keyname[SCANCODES];
IMPORT       struct KindStruct           filekind[KINDS];
IMPORT       struct MachineStruct        machines[MACHINES];
IMPORT       struct HiScoreStruct        hiscore[HISCORES];
IMPORT       struct IDirectInput*        lpdi;
IMPORT       struct IDirectInputDevice2* ffjoy2[2];
IMPORT       struct MemMapInfoStruct     memmapinfo[MEMMAPS];
IMPORT const struct CanvasStruct         canvas[CANVASES];
IMPORT const struct KnownStruct          known[KNOWNGAMES];
IMPORT const struct MenuStruct           menuinfo1[MENUITEMS],
                                         menuinfo2[MENUOPTS];
IMPORT const struct MemMapToStruct       memmap_to[MEMMAPS];
IMPORT const int                         gadgetimage[10][2];
IMPORT const STRPTR                      pongvarname[VARIANTS];
IMPORT IDirect3D9*                       Direct3DInterface;
IMPORT       HWND                        RichTextGadget;
#ifdef WIN32
    IMPORT   TEXT                        file_bkgrnd[MAX_PATH + 1],
                                         path_bkgrnd[MAX_PATH + 1];
#endif
#ifdef COUNTBITSEQUENCES
    IMPORT   int                         supercpu,
                                         table_size_2650[256];
#endif

// function pointers
IMPORT void (* drawpixel) (int x, int y, int colour);

// MODULE ARRAYS----------------------------------------------------------

MODULE       int                         StatusBarArray[9] = { 0, 0, 0, 0, 0, 0, 0, 0, -1 };

// EXPORTED STRUCTURES----------------------------------------------------

EXPORT struct LangStruct langs[LANGUAGES] = {
{ IDR_MAINMENU_ENG,
  IDR_POPUPMENU_ENG,
  IDR_TRAYMENU_ENG,
  /* these are answers to the question "what is the narrowest window that
     will fit all menu items into one/two/three rows (respectively)?"
     Windows XP without RA first, then Windows 7 without RA, then Windows
     7 with RA. */
  NULL,
  ',', // group separator
  '/', // date separator
  ':', // time separator
  '.', // decimal point
  LANG_ENGLISH,
  SUBLANG_ENGLISH_UK,
  CODEPAGE_ENG,
  { "Reset to game (F5, Alt+R)",
    "Open (Ctrl+O)",
    "Save state (Ctrl+S)",
    "Save screenshot as BMP (Ctrl+Y)",
    "Copy screenshot (Ctrl+C)",
    "Run REXX script (Ctrl+.)",
    "Quickload state (Ctrl+Shift+9, Alt+L)",
    "Quicksave state (Ctrl+Shift+0, Alt+S)",
    "Left autofire? (Ctrl+B)",
    "Right autofire? (Ctrl+Z)",
    "Controllers swapped? (Ctrl+J, Alt+J)",
    "Paused? (Pause, Play/Pause)",
    "Turbo speed? (Ctrl+T, Alt+W, Forwards)",
    "Collisions? (Ctrl+L)",
    "Sound? (Ctrl+U, Mute)",
    "Start recording (Ctrl+{)",
    "Restart playback",
    "Stop recording/playback (Ctrl+})"
} },
{ IDR_MAINMENU_HOL,
  IDR_POPUPMENU_HOL,
  IDR_TRAYMENU_HOL,
  ".\\Catalogs\\dutch.iff",
  '.', // group separator
  '-', // date separator
  ':', // time separator
  ',', // decimal point
  LANG_DUTCH,
  SUBLANG_DUTCH,
  CODEPAGE_ENG,
  { "Reset om spel (F5, Alt+R)",
    "Open (Ctrl+O)",
    "Opslaan momentopname (Ctrl+S)",
    "Opslaan screenshot als BMP (Ctrl+Y)",
    "Copy screenshot (Ctrl+C)",
    "ARexx-script uitvoeren (Ctrl+.)",
    "Snel laden momentopname (Ctrl+Shift+9, Alt+L)",
    "Snel opslaan momentopname (Ctrl+Shift+0, Alt+S)",
    "Left autofire? (Ctrl+B)",
    "Right autofire? (Ctrl+Z)",
    "Controllers verwisseld? (Ctrl+J, Alt+J)",
    "Onderbroken? (Pause, Play/Pause)",
    "Turbo snelheid? (Ctrl+T, Alt+W, Forwards)",
    "Aanvaringen? (Ctrl+L)",
    "Geluid? (Ctrl+U, Mute)",
    "Start opname (Ctrl+{)",
    "Restart afspelen",
    "Stop opnemen/afspelen (Ctrl+})"
} },
{ IDR_MAINMENU_FRA,
  IDR_POPUPMENU_FRA,
  IDR_TRAYMENU_FRA,
  ".\\Catalogs\\french.iff",
  ' ', // group separator
  '.', // date separator
  ':', // time separator
  ',', // decimal point
  LANG_FRENCH,
  SUBLANG_FRENCH,
  CODEPAGE_ENG,
  { "Réinitialiser le Jeu (F5, Alt+R)",
    "Ouvrir (Ctrl+O)",
    "Sauvegarder un Etat (Ctrl+S)",
    "Sauvegarder une Capture d'Ecran en BMP (Ctrl+Y)",
    "Copier une Capture d'Ecran (Ctrl+C)",
    "Lancer un script REXX (Ctrl+.)",
    "Chargement d'Etat Rapide (Ctrl+Shift+9, Alt+L)",
    "Enregistrement d'Etat Rapide (Ctrl+Shift+0, Alt+S)",
    "Left Tir Auto? (Ctrl+B)",
    "Right Tir Auto? (Ctrl+Z)",
    "Echanger les Contrôleurs? (Ctrl+J, Alt+J)",
    "Pause? (Pause, Play/Pause)",
    "Vitesse Turbo? (Ctrl+T, Alt+W, Forwards)",
    "Collisions? (Ctrl+L)",
    "Audio? (Ctrl+U, Mute)",
    "Démarrer l'Enregistrement (Ctrl+{)",
    "Redémarrer le Playback",
    "Arrêter l'enregistrement/exécution (Ctrl+})"
} },
{ IDR_MAINMENU_GER,
  IDR_POPUPMENU_GER,
  IDR_TRAYMENU_GER,
  ".\\Catalogs\\german.iff",
  '.', // group separator
  '.', // date separator
  ':', // time separator
  ',', // decimal point
  LANG_GERMAN,
  SUBLANG_GERMAN,
  CODEPAGE_ENG,
  { "Rücksetzen auf Spiel (F5, Alt+R)",
    "Öffnen (Ctrl+O)",
    "Speichern Momentaufnahme (Ctrl+S)",
    "Speichern als BMP-Bildschirmfoto (Ctrl+Y)",
    "Kopieren Bildschirmfoto (Ctrl+C)",
    "Ausführen REXX-Skript (Ctrl+.)",
    "Schnellladen Momentaufnahme (Ctrl+Shift+9, Alt+L)",
    "Schnellspeichern Momentaufnahme (Ctrl+Shift+0, Alt+S",
    "Links selbstschuss? (Ctrl+B)",
    "Rechts selbstschuss? (Ctrl+Y)",
    "Steuerung vertauschen? (Ctrl+J, Alt+J)",
    "Pausiert? (Pause, Play/Pause)",
    "Höchstgeschwindigkeit? (Ctrl+T, Alt+W, Forwards)",
    "Kollisionen? (Ctrl+L)",
    "Ton? (Ctrl+U, Mute)",
    "Aufnahme starten (Ctrl+{)",
    "Wiedergabe neustarten",
    "Anhalten der Aufnahme/Wiedergabe (Ctrl+})"
} },
{ IDR_MAINMENU_GRE,
  IDR_POPUPMENU_GRE,
  IDR_TRAYMENU_GRE,
  ".\\Catalogs\\greek.iff",
  '.', // group separator
  '/', // date separator
  ':', // time separator
  ',', // decimal point
  LANG_GREEK,
  SUBLANG_DEFAULT,
  1253,
  { "ÅðáíáöïñÜ óôï ðáé÷íßäé (F5, Alt+R)",
    "¶íïéãìá (Ctrl+O)",
    "ÁðïèÞêåõóç êáôÜóôáóçò (Ctrl+S)",
    "ÁðïèÞêåõóç åéêüíáò ùò BMP (Ctrl+Y)",
    "ÁíôéãñáöÞ åéêüíáò (Ctrl+C)",
    "ÅêôÝëåóç äÝóìçò åíôïëþí REXX (Ctrl+.)",
    "ÊáôÜóôáóç ãñÞãïñçò öüñôùóçò (Ctrl+Shift+9, Alt+L)",
    "ÊáôÜóôáóç ãñÞãïñçò áðïèÞêåõóçò (Ctrl+Shift+0, Alt+S)",
    "Left autofire; (Ctrl+B)",
    "Right autofire; (Ctrl+Z)",
    "ÅíáëëáãìÝíá ÷åéñéóôÞñéá; (Ctrl+J, Alt+J)",
    "Ðáýóç; (Pause, Play/Pause)",
    "Ôá÷ýôçôá turbo; (Ctrl+T, Alt+W, Forwards)",
    "Áíß÷íåõóç óõãêñïýóåùí; (Ctrl+L)",
    "¹÷ïò; (Ctrl+U, Mute)",
    "¸íáñîç êáôáãñáöÞò (Ctrl+{)",
    "Åðáíåêêßíçóç áíáðáñáãùãÞò",
    "ÄéáêïðÞ åããñáöÞò/áíáðáñáãùãÞò (Ctrl+})"
} },
{ IDR_MAINMENU_ITA,
  IDR_POPUPMENU_ITA,
  IDR_TRAYMENU_ITA,
  ".\\Catalogs\\italian.iff",
  '.', // group separator
  '-', // date separator
  ':', // time separator
  ',', // decimal point
  LANG_ITALIAN,
  SUBLANG_ITALIAN,
  CODEPAGE_ENG,
  { "Ritorna al gioco (F5, Alt+R)",
    "Apri (Ctrl+O)",
    "Salva snapshot (Ctrl+S)",
    "Salva screenshot come BMP (Ctrl+Y)",
    "Copia screenshot (Ctrl+C)",
    "Avvia script REXX (Ctrl+.)",
    "Caricamento rapido snapshot (Ctrl+Shift+9, Alt+L)",
    "Salvataggio rapido snapshot (Ctrl+Shift+0, Alt+S)",
    "Sinistra autofire? (Ctrl+B)",
    "Destra autofire? (Ctrl+Z)",
    "Scambia controlli? (Ctrl+J, Alt+J)",
    "Pausa? (Pausa, Play/Pausa)",
    "Velocità Turbo? (Ctrl+T, Alt+W, Avanzamento)",
    "Collisioni? (Ctrl+L)",
    "Audio? (Ctrl+U, Mute)",
    "Inizia la registrazione (Ctrl+{)",
    "Riavvia playback",
    "Stoppa registrazione/playback (Ctrl+})"
} },
{ IDR_MAINMENU_POL,
  IDR_POPUPMENU_POL,
  IDR_TRAYMENU_POL,
  ".\\Catalogs\\polish.iff",
  ' ', // group separator
  '.', // date separator
  ':', // time separator
  ',', // decimal point
  LANG_POLISH,
  SUBLANG_DEFAULT,
  1250,
  { "Zresetuj do gry (F5, Alt+R)",
    "Otwórz (Ctrl+O)",
    "Zapisz stan (Ctrl+S)",
    "Zapisz zrzut ekranu jako BMP (Ctrl+Y)",
    "Skopiuj zrzut ekranu (Ctrl+C)",
    "Uruchom skrypt REXX (Ctrl+.)",
    "Stan szybkiego ladowania (Ctrl+Shift+9, Alt+L)",
    "Stan Quicksave (Ctrl+Shift+0, Alt+S)",
    "Left automatyczne strzelanie? (Ctrl+B)",
    "Right automatyczne strzelanie? (Ctrl+Z)",
    "Kontrolery zamienione? (Ctrl+J, Alt+J)",
    "Paused? (Pauza, Play/Pause)",
    "Szybkosc turbo? (Ctrl+T, Alt+W, Do przodu)",
    "Zderzenia? (Ctrl+L)",
    "Dzwiek? (Ctrl+U, Mute)",
    "Rozpocznij nagrywanie (Ctrl+{)",
    "Ponownie uruchom odtwarzanie",
    "Zatrzymaj nagrywanie/odtwarzanie (Ctrl+})"
} },
{ IDR_MAINMENU_RUS,
  IDR_POPUPMENU_RUS,
  IDR_TRAYMENU_RUS,
  ".\\Catalogs\\russian.iff",
  ' ', // group separator
  '.', // date separator
  ':', // time separator
  ',', // decimal point
  LANG_RUSSIAN,
  SUBLANG_DEFAULT,
  1251,
  { "Ñáðîñèòü èãðó (F5, Alt+R)",
    "Îòêðûòü (Ctrl+O)",
    "Ñîõðàíèòü èãðó (Ctrl+S)",
    "Ñîõðàíèòü ñêðèíøîò â ôîðìàòå BMP (Ctrl+Y)",
    "Êîïèðîâàòü ñêðèíøîò (Ctrl+C)",
    "Çàïóñòèòü ARexx ñêðèïò (Ctrl+.)",
    "Áûñòðàÿ çàãðóçêà (Ctrl+Shift+9, Alt+L)",
    "Áûñòðîå ñîõðàíåíèå (Ctrl+Shift+0, Alt+S)",
    "Left Àâòîìàòè÷åñêàÿ ñòðåëüáà? (Ctrl+B)",
    "Right Àâòîìàòè÷åñêàÿ ñòðåëüáà? (Ctrl+Z)",
    "Ïîìåíÿòü ìåñòàìè êîíòðîëëåðû? (Ctrl+J, Alt+J)",
    "Ïàóçà? (Pause, Play/Pause)",
    "Ðåæèì Òóðáî? (Ctrl+T, Alt+W, Forwards)",
    "Îïðåäåëåíèå ñòîëêíîâåíèÿ ñïðàéòîâ? (Ctrl+L)",
    "Çâóê? (Ctrl+U, Mute)",
    "Íà÷àòü çàïèñü (Ctrl+{)",
    "Âîçîáíîâèòü âîñïðîèçâåäåíèå",
    "Çàêîí÷èòü çàïèñü/âîñïðîèçâåäåíèå (Ctrl+})"
} },
{ IDR_MAINMENU_SPA,
  IDR_POPUPMENU_SPA,
  IDR_TRAYMENU_SPA,
  ".\\Catalogs\\spanish.iff",
  '.', // group separator
  '/', // date separator
  ':', // time separator
  ',', // decimal point
  LANG_SPANISH,
  SUBLANG_SPANISH,
  CODEPAGE_ENG,
  { "Reiniciar el juego (F5, Alt+R)",
    "Abrir (Ctrl+O)",
    "Guardar el estado actual (Ctrl+S)",
    "Guardar la imagen de la pantalla en formato BMP (Ctrl+Y)",
    "Copiar la imagen de la pantalla (Ctrl+C)",
    "Ejecutar un guión REXX (Ctrl+.)",
    "Carga rápida del estado (Ctrl+Shift+9, Alt+L)",
    "Grabación rápida de estado actual (Ctrl+Shift+0, Alt+S)",
    "¿Left activar el disparo automático? (Ctrl+B)",
    "¿Right activar el disparo automático? (Ctrl+Z)",
    "¿Intercambiar los controladores? (Ctrl+J, Alt+J)",
    "¿En modo pausa? (Pausa, Reproducir/pausa)",
    "¿Velocidad turbo? (Ctrl+T, Alt+W, Forwards)",
    "¿Activar las colisiones? (Ctrl+L)",
    "¿Activar el sonido? (Ctrl+U, Mute)",
    "Comenzar la grabación (Ctrl+{)",
    "Reiniciar la reproducción",
    "Detener la grabación/reproducción (Ctrl+})"
} } };

// MODULE FUNCTIONS-------------------------------------------------------

// general functions
MODULE void printusage(STRPTR progname);
MODULE void sidebar_down(void);
MODULE void sidebar_up(void);
MODULE void subreq(void);
MODULE void disassociate(int kind);
MODULE void upgradeconfigs(void);
MODULE HBITMAP getmib(void);
MODULE void dragndrop(HDROP wParam);
MODULE void refresh_quals(void);

// RetroAchievements functions
MODULE void CausePause(void);
MODULE void CauseUnpause(void);
MODULE void GetEstimatedGameTitle(char* sNameOut);
MODULE void RebuildMenu(void);
MODULE void ResetEmulator(void);
MODULE void LoadROM(const char* sFullPath);
MODULE void go_hardcore(void);
MODULE void go_softcore(void);

LRESULT CALLBACK CustomEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MagnifierWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

APIRET APIENTRY rexx_activate(        CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_activatewindow(  CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_clear(           CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_client(          CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_closecontrols(   CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_closedips(       CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_closediskdrive(  CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_closegameinfo(   CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_closegamepads(   CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_closekeyboard(   CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_closememory(     CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_closemonitor(    CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_closemusic(      CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_closeopcodes(    CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_closepalette(    CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_closesprites(    CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_closetapedeck(   CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_cmdshell(        CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_copy(            CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_copytext(        CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_deactivate(      CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_debugger(        CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_getattr(         CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_getcredits(      CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_gethostname(     CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_getmachine(      CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_getmemmap(       CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_getname(         CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_getport(         CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_getreference(    CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_getversion(      CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_help(            CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_jumptomonitor(   CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_menu(            CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_movewindow(      CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_new(             CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_open(            CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_opencontrols(    CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_opendips(        CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_opendiskdrive(   CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_opengamepads(    CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_opengameinfo(    CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_openkeyboard(    CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_openmemory(      CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_openmonitor(     CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_openmusic(       CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_openopcodes(     CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_openpalette(     CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_opensprites(     CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_opentapedeck(    CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_paste(           CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_pause(           CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_peek(            CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_quit(            CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_revert(          CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_saveacbm(        CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_saveas(          CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_savebmp(         CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_savegif(         CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_saveilbm(        CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_savepcx(         CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_savepng(         CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_savetiff(        CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_server(          CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_setmachine(      CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_setport(         CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_setspeed(        CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_turbo(           CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_unpause(         CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_viewhighscores(  CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_windowtoback(    CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
APIRET APIENTRY rexx_windowtofront(   CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr);
LONG APIENTRY rexx_rxsio(LONG ExitNumber, LONG Subfunction, PEXIT ParmBlock);

// CODE-------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{   TRANSIENT PAINTSTRUCT   localps;
    TRANSIENT LPTOOLTIPTEXT lpttt;
    TRANSIENT int           i,
                            mousex, mousey;
    TRANSIENT signed int    oldcurrentgame,
                            movement;
    TRANSIENT LPNMLISTVIEW  lpnmlv;
    TRANSIENT RECT          localrect;
    TRANSIENT LONG          gid;
    TRANSIENT LPWINDOWPOS   posptr;
    PERSIST   TEXT          constring[4][40 + 1]; // these must remain valid!

    /* "When a window procedure processes a message, it should return 0 from the window
       procedure. All messages that a window procedure chooses not to process must be
       passed to a Windows function named DefWindowProc(). The value returned from
       DefWindowProc() must be returned from the window procedure. */

    switch (msg)
    {
    case WM_CREATE:
        if (GetKeyState(VK_CAPITAL)) capslock = TRUE; else capslock = FALSE;
    acase WM_SYSCOMMAND:
        if (wParam == SC_MAXIMIZE)
        {   if (fullscreen) fullscreen = FALSE; else fullscreen = TRUE;
            docommand(MENUITEM_FULLSCREEN);
        } elif (LOWORD(wParam) == ID_FILE_ICONIFY)
        {   iconify();
        }
    acase WM_NCLBUTTONDBLCLK:
        if (fullscreen) fullscreen = FALSE; else fullscreen = TRUE;
        docommand(MENUITEM_FULLSCREEN);
    acase WM_DROPFILES: // this is sent when a file is dropped onto any of our subwindows too, not just the main window
        dragndrop((HDROP) wParam);
    acase WM_PAINT:
        BeginPaint(hwnd, &localps);
        if (fullscreen)
        {   clearscreen(); // so that the grey border is repainted
        }
        redrawscreen();
        DISCARD EndPaint(hwnd, &localps);
    acase USR_NOTIFYICON:
        // assert(iconified);
        switch (lParam)
        {
        case WM_LBUTTONDBLCLK:
            uniconify(FALSE);
        acase WM_RBUTTONUP:
            traymenu();
        }
    acase WM_CLOSE:
        if (confirm())
        {   cleanexit(EXIT_SUCCESS);
        } // implied else
        return 0; // important!
    acase WM_DESTROY:
        if (hDebugger)
        {   SetWindowLong(hDebugger, GWL_WNDPROC, (LONG) wpOldEditProc);
        }
        PostQuitMessage(0);
    acase WM_MOUSEMOVE:
        updatepointer(FALSE, FALSE);
    acase WM_MOVE:
        if (fullscreen)
        {   DISCARD SetWindowPos
            (   MainWindowPtr,
                HWND_TOP,
                0, 0,
                winwidth,
                winheight,
                SWP_NOSIZE | SWP_NOCOPYBITS
            ); // move it back!
        } elif (ready)
        {   winleftx =  (int) LOWORD(lParam);
            if (winleftx >= 32768)
            {   winleftx -= 65536;
            }
            wintopy  =  (int) HIWORD(lParam);
            if (wintopy >= 32768)
            {   wintopy -= 65536;
            }
            winleftx -= leftwidth;

            // don't subtract titlebar, only menubar and toolbar!
            if (showmenubars[wsm])
            {   wintopy -= menuheight;
            }
            if (showtoolbars[wsm])
            {   wintopy -= toolbarheight;
        }   }
        reset_fps(); // because Windows doesn't run a program while its window or subwindow is being moved by the user
    acase WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
        if (menuhelp)
        {   menuhelp = FALSE;
            dogamename();
        }
        SetCapture(hwnd);
        mousex = (int) (LOWORD(lParam));
        mousey = (int) (HIWORD(lParam));
        if (incli)
        {   debugger_exit();
        }
        if (editscreen)
        {   if (filter != FILTER_3D)
            {   hosttoguestmouse(NULL, NULL, &mousex, &mousey, NULL, NULL);
                show_position(mousex, mousey, TRUE);
        }   }
        elif (hostcontroller[0] == CONTROLLER_TRACKBALL || hostcontroller[1] == CONTROLLER_TRACKBALL)
        {   // assuming it is within in guest screen boundaries
            if (!lmb && !ignorelmb)
            {   if (SubWindowPtr[SUBWINDOW_HOSTPADS])
                {   lmb = TRUE;
                    DISCARD RedrawWindow(SubWindowPtr[SUBWINDOW_HOSTPADS], NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
                } else
                {   lmb = TRUE;
        }   }   }
        ignorelmb = FALSE;
    acase WM_LBUTTONUP:
        ReleaseCapture();
        if (lmb)
        {   if (SubWindowPtr[SUBWINDOW_HOSTPADS])
            {   lmb = FALSE;
                DISCARD RedrawWindow(SubWindowPtr[SUBWINDOW_HOSTPADS], NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
            } else
            {   lmb = FALSE;
        }   }
    acase WM_MBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
        SetCapture(hwnd);
        if (!mmb)
        {   if (SubWindowPtr[SUBWINDOW_HOSTPADS])
            {   mmb = TRUE;
                DISCARD RedrawWindow(SubWindowPtr[SUBWINDOW_HOSTPADS], NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
            } else
            {   mmb = TRUE;
        }   }
    acase WM_MBUTTONUP:
        ReleaseCapture();
        if (mmb)
        {   if (SubWindowPtr[SUBWINDOW_HOSTPADS])
            {   mmb = FALSE;
                DISCARD RedrawWindow(SubWindowPtr[SUBWINDOW_HOSTPADS], NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
            } else
            {   mmb = FALSE;
        }   }
    acase WM_RBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
        if
        (   guestrmb
         && !paused
         && (   hostcontroller[0] == CONTROLLER_TRACKBALL
             || hostcontroller[1] == CONTROLLER_TRACKBALL
            )
         && (machine == BINBUG || !ISQWERTY)
        )
        {   SetCapture(hwnd);
            if (!rmb)
            {   if (SubWindowPtr[SUBWINDOW_HOSTPADS])
                {   rmb = TRUE;
                    DISCARD RedrawWindow(SubWindowPtr[SUBWINDOW_HOSTPADS], NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
                } else
                {   rmb = TRUE;
        }   }   }
    acase WM_RBUTTONUP:
        if
        (   guestrmb
         && !paused
         && (   hostcontroller[0] == CONTROLLER_TRACKBALL
             || hostcontroller[1] == CONTROLLER_TRACKBALL
            )
         && (machine == BINBUG || !ISQWERTY)
        )
        {   ReleaseCapture();
            if (rmb)
            {   if (SubWindowPtr[SUBWINDOW_HOSTPADS])
                {   rmb = FALSE;
                    DISCARD RedrawWindow(SubWindowPtr[SUBWINDOW_HOSTPADS], NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
                } else
                {   rmb = FALSE;
        }   }   }
    acase WM_MOUSEWHEEL:
        if
        (    showsidebars[wsm]
          && foundgames
          && !crippled
          && ctrl()
        )
        {   movement = ((short) HIWORD(wParam)) / WHEELDELTA;
            oldcurrentgame = currentgame;
            if (movement >= 1) // forwards/up/away
            {   if (currentgame == -1)
                {   currentgame = foundgames; // move to bottom
                } else
                {   for (i = 0; i < movement; i++)
                    {   if (currentgame > 0)
                        {   currentgame--; // move up by 1
            }   }   }   }
            elif (movement <= -1) // back/down/towards
            {   if (currentgame == -1)
                {   currentgame = 0; // move to top
                } else
                {   for (i = 0; i < abs(movement); i++)
                    {   if (currentgame < foundgames - 1)
                        {   currentgame++; // move down by 1
            }   }   }   }
            if (currentgame != oldcurrentgame)
            {   ListView_SetItemState(hSideBar, oldcurrentgame, 0, 0x000F);
                ListView_SetItemState(hSideBar, currentgame, (LVIS_FOCUSED | LVIS_SELECTED), 0x000F);
                if (currentgame != foundgames - 1)
                {   DISCARD ListView_EnsureVisible(hSideBar, currentgame, TRUE); // doesn't seem to work properly for items in the rightmost column
                }
                macro_stop();
                dosidebar(); // but window might be about to be resized!
        }   }
    acase WM_ENTERMENULOOP:
        if (!showingpointer)
        {   ShowCursor(TRUE);
            showingpointer = TRUE;
        }
        sound_off(FALSE);
        ff_off();
        ff_uninitjoys();
        ff_initjoys();
        update_joynames();
        reset_fps();
    acase WM_EXITMENULOOP:
        if (emulating)
        {   sound_on(FALSE);
        }
        updatepointer(FALSE, FALSE);
    acase WM_ACTIVATE:
        if (wParam == WA_CLICKACTIVE || wParam == WA_ACTIVE)
        {   if (incli)
            {   debugger_enter();
            } else
            {   ignorelmb = TRUE;
            }
            refreshkybd();
            do_autopause(wParam, lParam);
        }
    acase WM_SETFOCUS: // needed for Alt-Tabbing to work properly
        if (incli)
        {   debugger_enter();
        }
    acase WM_EXITSIZEMOVE:
        GetWindowRect(MainWindowPtr, &localrect);
        if
        (   !fullscreen
         && ready
         && (   localrect.right  - localrect.left != winwidth
             || localrect.bottom - localrect.top  != winheight
        )   )
        {   if (showsidebars[wsm])
            {   if (localrect.right - localrect.left > minwinwidth)
                {   winwidth = localrect.right - localrect.left;
                } else
                {   winwidth = minwinwidth;
                }
                sidebarwidth = winwidth - rastwidth;
            }
            i = localrect.bottom - localrect.top + 1 - winheight;
            if (i >= machines[machine].height)
            {   while (i >= machines[machine].height && size < 6)
                {   i -= machines[machine].height;
                    size++;
            }   }
            elif (i <= -machines[machine].height)
            {   while (i <= -machines[machine].height && size > 1)
                {   i += machines[machine].height;
                    size--;
            }   }
            resize(size, TRUE);
        }
    acase WM_COMMAND:
        if (menuhelp)
        {   menuhelp = FALSE;
            dogamename();
        }
        if (!ListeningWindowPtr)
        {   if (LOWORD(wParam) == menucode[MENUITEM_CHEEVOS2] && !cheevos)
            {   flipbool(&cheevos, MENUITEM_CHEEVOS1, FALSE);
                init_cheevos();
            } // we should probably pend this
            for (i = 0; i < MENUITEMS; i++)
            {   if (menucode[i] == LOWORD(wParam))
                {   if (i != MENUITEM_CHEEVOS2)
                    {   storedmenu1 = i;
                        return 0; // for speed
            }   }   }
            for (i = 0; i < MENUOPTS; i++)
            {   if (menuopt[i] == LOWORD(wParam))
                {   storedmenu2 = i;
                    return 0; // for speed
            }   }
            switch (LOWORD(wParam))
            {
            case ID_CONSOLE_START:
                if (machine == ELEKTOR)
                {   if (whichgame == SUBMARINESRACINGPOS)
                    {   console_start = 30;
                    } else
                    {   console_start = 24; // needed for eg. Penalty
                }   }
                elif (machine == TYPERIGHT)
                {   console_start =  2;
                } else
                {   console_start = 16; // >= 16 is needed for eg. Arcadia Circus, >= 5 is needed for eg. Super Bug
                }
            acase ID_CONSOLE_A:
                if (machine == INTERTON)
                {   console_a     = 16; // must be exactly 16 for Hunting. And 5 is not enough for Crazy Crab.
                } elif (machine == ELEKTOR)
                {   switch (whichgame)
                    {
                    case  PENALTYPOS:          console_a = 24;
                    acase SUBMARINESRACINGPOS: console_a = 30;
                    adefault:                  console_a = 16; // must be exactly 16 for Hunting
                }   }
                elif (machine == TYPERIGHT)
                {   console_a     =  2;
                } else
                {   console_a     =  5;
                }
            acase ID_CONSOLE_B:
                if (machine == TYPERIGHT)
                {   console_b     =  2;
                } else
                {   console_b     =  5;
                }
            acase ID_CONSOLE_RESET:
                if (machine == TYPERIGHT)
                {   console_reset =  2;
                } else
                {   console_reset =  5;
                }
            adefault:
                if (LOWORD(wParam) >= IDM_RA_MENUSTART && LOWORD(wParam) < IDM_RA_MENUEND)
                {   // assert(cheevos);
#ifdef LOGCHEEVOS
                    zprintf(TEXTPEN_VERBOSE, "RA_InvokeDialog()\n");
#endif
                    RA_InvokeDialog(LOWORD(wParam));
                    if (!RA_HardcoreModeIsActive())
                    {   go_softcore();
        }   }   }   }
    acase WM_NOTIFY:
        switch (((LPNMHDR) lParam)->code)
        {
        case TTN_NEEDTEXT:
            lpttt = (LPTOOLTIPTEXT) lParam; // button info
            lpttt->hinst = InstancePtr;

            switch (lpttt->hdr.idFrom) // Check which button we hover on
            {
            case  ID_FILE_RESET:            lpttt->lpszText = langs[language].tooltip[ 0];
            acase ID_FILE_OPEN:             lpttt->lpszText = langs[language].tooltip[ 1];
            acase ID_FILE_SAVESNP:          lpttt->lpszText = langs[language].tooltip[ 2];
            acase ID_FILE_SAVEBMP:          lpttt->lpszText = langs[language].tooltip[ 3];
            acase ID_EDIT_COPY:             lpttt->lpszText = langs[language].tooltip[ 4];
            acase ID_MACRO_RUNREXX:         lpttt->lpszText = langs[language].tooltip[ 5];
            acase ID_FILE_QUICKLOAD:        lpttt->lpszText = langs[language].tooltip[ 6];
            acase ID_FILE_QUICKSAVE:        lpttt->lpszText = langs[language].tooltip[ 7];
            acase ID_LEFT_AUTOFIRE:         lpttt->lpszText = langs[language].tooltip[ 8];
            acase ID_RIGHT_AUTOFIRE:        lpttt->lpszText = langs[language].tooltip[ 9];
            acase ID_INPUT_SWAPPED:         lpttt->lpszText = langs[language].tooltip[10];
            acase ID_SPEED_PAUSED:          lpttt->lpszText = langs[language].tooltip[11];
            acase ID_SPEED_TURBO:           lpttt->lpszText = langs[language].tooltip[12];
            acase ID_SPRITES_COLLISIONS:    lpttt->lpszText = langs[language].tooltip[13];
            acase ID_SOUND_ENABLED:         lpttt->lpszText = langs[language].tooltip[14];
            acase ID_MACRO_STARTRECORDING:  lpttt->lpszText = langs[language].tooltip[15];
            acase ID_MACRO_RESTARTPLAYBACK: lpttt->lpszText = langs[language].tooltip[16];
            acase ID_MACRO_STOP:            lpttt->lpszText = langs[language].tooltip[17];
            acase ID_CONSOLE_START:
                strcpy(constring[0], machines[machine].consolekeyname[0]);
                strcat(constring[0], " (F1)");
                lpttt->lpszText = constring[0];
            acase ID_CONSOLE_A:
                strcpy(constring[1], machines[machine].consolekeyname[1]);
                strcat(constring[1], " (F2)");
                lpttt->lpszText = constring[1];
            acase ID_CONSOLE_B:
                strcpy(constring[2], machines[machine].consolekeyname[2]);
                strcat(constring[2], " (F3)");
                lpttt->lpszText = constring[2];
            acase ID_CONSOLE_RESET:
                strcpy(constring[3], machines[machine].consolekeyname[3]);
                strcat(constring[3], " (F4)");
                lpttt->lpszText = constring[3];
            }
        acase LVN_BEGINLABELEDIT:
            return 1; // this disallows label editing
        acase LVN_KEYDOWN:
            return 1; // this disallows keyboard input
        acase NM_CLICK:
        case NM_DBLCLK:
        case NM_RCLICK:
            if (incli)
            {   debugger_exit();
            }
            lpnmlv = (LPNMLISTVIEW) lParam;
            if (lpnmlv->hdr.hwndFrom == hSideBar)
            {   if (lpnmlv->iItem == -1)
                {   if (currentgame != -1)
                    {   ListView_SetItemState(hSideBar, currentgame, (LVIS_FOCUSED | LVIS_SELECTED), 0x000F);
                }   }
                else
                {   newgame = lpnmlv->iItem;
        }   }   }
    acase WM_CTLCOLOREDIT:
        gid = GetWindowLong((HWND) lParam, GWL_ID);
        switch (gid)
        {
        case IDC_DEBUGGER:
            if (incli)
            {   SetBkColor((HDC) wParam, EMUPEN_DARKORANGE);
                return (LRESULT) hBrush[EMUBRUSH_DARKORANGE];
            } else
            {   SetBkColor((HDC) wParam, EMUPEN_WHITE);
                return (LRESULT) hBrush[EMUBRUSH_WHITE];
        }   }
    acase WM_WINDOWPOSCHANGING:
        // This is for blocking the window snapshotting (eg. Windows+Lt/Rt) "feature" of later Windows versions
        if (sizing)
        {   sizing = FALSE;
        } else
        {   posptr = (LPWINDOWPOS) lParam;
            posptr->flags |= SWP_NOSIZE;
            return 0;
        }
    acase WM_SIZING:
        sizing = TRUE;
    acase WM_MENUSELECT:
        if (showstatusbars[wsm])
        {   UINT uItem;

            uItem = (UINT) LOWORD(wParam);
            for (i = 0; i < MENUITEMS; i++)
            {   if (uItem == (UINT) menucode[i])
                {   if (menuinfo1[i].desc_str[0] != EOS)
                    {   SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM) LLL(menuinfo1[i].desc_id, menuinfo1[i].desc_str));
                        menuhelp = TRUE;
                    } else
                    {   if (menuhelp)
                        {   menuhelp = FALSE;
                            dogamename();
                    }   }
                    return FALSE;
            }   }
            for (i = 0; i < MENUOPTS; i++)
            {   if (uItem == (UINT) menuopt[i])
                {   if (menuinfo2[i].desc_str[0] != EOS)
                    {   SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM) LLL(menuinfo2[i].desc_id, menuinfo2[i].desc_str));
                        menuhelp = TRUE;
                    } else
                    {   if (menuhelp)
                        {   menuhelp = FALSE;
                            dogamename();
                    }   }
                    return FALSE;
    }   }   }   }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

#ifdef GUI
    int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
#ifdef CLI
    int main(void)
#endif
{   FLAG          settings      = FALSE;
    BOOL          b;
    DWORD         usernamemax   = MAX_USERNAMELEN;
    int           argc,
                  gotfilename   = FALSE,
                  i, j,
                  oldlanguage,
                  rc,
                  temp_menubar  = 2,
                  temp_titlebar = 2,
                  temp_toolbar  = 2,
                  x, y;
    char**        argv;
    WSADATA       WSAInfo;
    WNDCLASSEX    wc;
    OSVERSIONINFO Vers;
    LANGID        langid;
 // DEVMODE       TheDevMode;
#ifndef MEASURECAPSLOCK
    SLONG         number;
#endif

    // Start of program.

    argc = __argc;
    argv = __argv;

    for (x = 0; x < BOXWIDTH; x++)
    {   for (y = 0; y < BOXHEIGHT; y++)
        {   screen[x][y]   = 8;
    }   }
    for (i = 0; i < MIBS; i++)
    {   mib[i].bitmap      = NULL;
    }
    for (i = 0; i < BEZELS; i++)
    {   BezelBitMap[i]     = NULL;
    }
    for (i = 0; i < LANGUAGES; i++)
    {   langs[i].winbuffer = NULL;
        langs[i].offset    = NULL;
    }
    for (i = 0; i < SUBWINDOWS; i++)
    {   subwinx[i]         =
        subwiny[i]         = -1;
        TipsPtr[i]         = NULL;
    }
    for (i = 0; i < MEMMAPS; i++)
    {   memmap_to_smlimage[i] = i;
    }
    for (i = 0; i < CANVASES; i++)
    {   canvasdisplay[i]   = NULL;
    }

    joyname[0][0] =
    joyname[1][0] = EOS;
    for (i = 0; i < EMUBRUSHES; i++)
    {   hBrush[i] = NULL;
    }

    for (i = 0; i < COLOURSETS; i++)
    {   for (j = 0; j < GUESTCOLOURS; j++)
        {   resetcolour(i, j, FALSE);
    }   }

    sound_preinit();
    drawpixel = drawpixel_null;
    engine_setup();

    InitCommonControls();
    LoadLibrary("RICHED20.DLL");
    InstancePtr = GetModuleHandle(NULL);

    // This is done early so we can open a console early if needed
    hMonospaceFont = CreateFont
    (   8,                      // int nHeight
        0,                      // int nWidth
        0,                      // int nEscapement
        0,                      // int nOrientation
        FW_MEDIUM,              // int nWeight
        FALSE,                  // BYTE bItalic
        FALSE,                  // BYTE bUnderline
        0,                      // BYTE cStrikeOut
        ANSI_CHARSET,           // BYTE nCharSet
        OUT_DEFAULT_PRECIS,     // BYTE nOutPrecision
        CLIP_CHARACTER_PRECIS,  // BYTE nClipPrecision
        DEFAULT_QUALITY,        // BYTE nQuality
        FF_SWISS,               // BYTE nPitchAndFamily
        "Courier"               // LPCTSTR lpszFacename (doesn't work with "MS Sans Serif")
    );

    DisplayWidth  = GetSystemMetrics(SM_CXSCREEN); // these 2 lines must be done before loadconfig()
    DisplayHeight = GetSystemMetrics(SM_CYSCREEN);

    stars = malloc(MAXBOX * sizeof(ULONG));

    /* major    minor
         3        51     Windows NT version 3.51
         4         0     Windows NT version 4.0, Windows 95
         4        10     Windows 98SE
         5         0     Windows 2000
         5         1     Windows XP (all SP versions)
         6         0     Windows Vista
         6         1     Windows 7
         6         2     Windows 8?
         6         3     Windows 8.1 (requires manifest?)?
         7         3     Windows XP running AppVerifier (HighVersionLie)

    Under Win8.1, when running from VC6+SP6, it tells us V5.1.
    Under Win8.1, when running from desktop, it tells us V6.2.

    Windows 95 fails if dwOSVersionInfoSize == sizeof(OSVERSIONINFOEX).
    You have to retry with dwOSVersionInfoSize == sizeof(OSVERSIONINFO).

    By default, AppVerifier adds 2 to the major and minor version numbers
    (eg. V5.1 becomes V7.3). */

    Vers.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    DISCARD GetVersionEx((LPOSVERSIONINFO) &Vers);
    if
    (   Vers.dwPlatformId   == VER_PLATFORM_WIN32_NT
     && Vers.dwMajorVersion >= 6
    )
    {   // zprintf(TEXTPEN_VERBOSE, "Windows Vista/7/8 detected!\n");
        win7 = TRUE;
        /* if (Vers.dwMinorVersion >= 1 || Vers.dwMajorVersion >= 7)
        {   win8 = TRUE;
        } */
    }
    /* if
    (   Vers.dwPlatformId   == VER_PLATFORM_WIN32_NT
     && (    Vers.dwMajorVersion >  6
         || (Vers.dwMajorVersion == 6 && Vers.dwMinorVersion >  2)
         || (Vers.dwMajorVersion == 6 && Vers.dwMinorVersion == 2 && Vers.dwBuildNumber >= 9200)
    )   )
    {   // zprintf(TEXTPEN_VERBOSE, "Windows 11 detected!\n");
        win11 = TRUE;
    } */

#ifdef VERBOSE
    zprintf
    (   TEXTPEN_VERBOSE,
        "Version: \"%s\" (%d.%d)\n"\
        "Build:   %d\n",
        Vers.szCSDVersion,
        (int) Vers.dwMajorVersion,
        (int) Vers.dwMinorVersion,
        (int) Vers.dwBuildNumber
    );
#endif

 // EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &TheDevMode);
 // hostrefresh = TheDevMode.dmDisplayFrequency; // host screen refresh rate (60 Hz on the development system)

    speech_init();

    hArrow    = LoadCursor(NULL, IDC_ARROW);
    hBusy     = LoadCursor(NULL, IDC_WAIT);
    hCross[0] = LoadImage(InstancePtr, MAKEINTRESOURCE(IDU_REDCROSS   ), IMAGE_CURSOR, 32, 32, 0);
    hCross[1] = LoadImage(InstancePtr, MAKEINTRESOURCE(IDU_BLUECROSS  ), IMAGE_CURSOR, 32, 32, 0);
    hCross[2] = LoadImage(InstancePtr, MAKEINTRESOURCE(IDU_PURPLECROSS), IMAGE_CURSOR, 32, 32, 0);
    // "The system automatically deletes these resources when the process that created them terminates."

    file_game[0] =
    hostname[ 0] = EOS;
    // no need to zero fn_screenshot[0]

    strcpy(fn_config, "Configs\\WinArcadia.ini");

    DISCARD WSAStartup(MAKEWORD(2, 2), &WSAInfo); // initialize networking

    langid = GetUserDefaultLangID();

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "User language ID is $%2X.\n", langid);
#endif

    switch (langid & 0xFF)
    {
    case  0x07: language = LANGUAGE_GER;
    acase 0x08: language = LANGUAGE_GRE;
    acase 0x0A: language = LANGUAGE_SPA;
    acase 0x0C: language = LANGUAGE_FRA;
    acase 0x10: language = LANGUAGE_ITA;
    acase 0x13: language = LANGUAGE_HOL;
    acase 0x15: language = LANGUAGE_POL;
    acase 0x19: language = LANGUAGE_RUS;
    adefault:   language = LANGUAGE_ENG;
                if (langid == 0x0109) yank = TRUE;
    }

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Emulator language is %d.\n", language);
#endif

    if (DisplayWidth >= 1280 && DisplayHeight >= 1024) // 1280*1024
    {   realsize = 3;
    } elif (DisplayWidth >= 1024 && DisplayHeight >= 768) // 1024*768
    {   realsize = 2;
    } else // 800*600, 640*480
    {   realsize = 1;
    }
    size = realsize;

    wsm = fullscreen ? 1 : 0;
    if (temp_menubar  != 2)
    {   showmenubars[ wsm] = temp_menubar;
    }
    if (temp_titlebar != 2)
    {   showtitlebars[wsm] = temp_titlebar;
    }
    if (temp_toolbar  != 2)
    {   showtoolbars[ wsm] = temp_toolbar;
    }

    DISCARD GetModuleFileName(NULL, ProgDir, MAX_PATH);
    DISCARD PathRemoveFileSpec(ProgDir);
    DISCARD SetCurrentDirectory(ProgDir);
    strcpy(ProgName, argv[0]);

    loadconfig(); // load default configuration file
    wsm = fullscreen ? 1 : 0;

    if (fn_bkgrnd[0])
    {   break_pathname(fn_bkgrnd, path_bkgrnd, file_bkgrnd);
        if (file_bkgrnd[0])
        {   DISCARD load_bmp(fn_bkgrnd);
    }   }

    Direct3DInterface = Direct3DCreate9(D3D_SDK_VERSION);
    if (Direct3DInterface)
    {   d3d = TRUE;
    } /* else
    {   zprintf(TEXTPEN_ERROR, "Direct3DCreate9() failed!\n");
    } */

#ifdef MEASURECAPSLOCK
    if (argc == 2)
    {   DISCARD stcd_l(argv[1], &measurement);
    }
#else
    if (argc >= 2)
    {   for (i = 1; i < argc; i++)
        {   if (!strcmp(argv[i], "?"))
            {   printusage(argv[0]);
            } elif (!stricmp(argv[i], "LOCKCONFIG"))
            {   lockconfig = TRUE;
            } elif (!strnicmp(argv[i], "PORT=", 5))
            {   DISCARD stcd_l(&argv[i][5], &number); // string -> decimal number
                if (number < 0 || number > 65535)
                {   printusage(argv[0]);
                } else
                {   netport = (UWORD) number;
            }   }
            elif (!strnicmp(argv[i], "SETTINGS=", 9))
            {   strcpy(fn_config, ".\\Configs\\");
                strcat(fn_config, (STRPTR) &argv[i][9]);
                settings = TRUE;
                // we don't load it yet
            } elif (!stricmp(argv[i], "UPGRADE"))
            {   upgrade = TRUE;
            } elif (!strnicmp(argv[i], "FULLSCREEN=", 11))
            {   if (!stricmp(&argv[i][11], "ON"))
                {   override_fullscreen = TRUE;
                    setting_fullscreen  = TRUE;
                } elif (!stricmp(&argv[i][11], "OFF"))
                {   override_fullscreen = TRUE;
                    setting_fullscreen  = FALSE;
                } else
                {   printusage(argv[0]);
            }   }
            elif (!strnicmp(argv[i], "STARTUP=", 8))
            {   strcpy(fn_script, (STRPTR) &argv[i][8]);
                // we don't load it yet
            } elif (!strnicmp(argv[i], "FILE=", 5))
            {   strcpy(fn_game, (STRPTR) &argv[i][5]);
                gotfilename = TRUE; // we don't load it yet
            } elif (!strnicmp(argv[i], "ASM=", 4))
            {   strcpy(fn_asm, (STRPTR) &argv[i][4]);
                climode = 1;
            } elif (!strnicmp(argv[i], "DISGAME=", 8))
            {   strcpy(fn_game, (STRPTR) &argv[i][8]);
                climode = 2;
            } else
            {   strcpy(fn_game, argv[i]);
                gotfilename = TRUE; // we don't load it yet
    }   }   }
#endif

    if (upgrade && lockconfig)
    {   upgrade = FALSE;
        printusage(argv[0]);
    }

    if (upgrade)
    {   upgradeconfigs();
    } else
    {   if (settings)
        {   oldlanguage = language;
            loadconfig();
            if (language != oldlanguage)
            {   changelanguage();
        }   }
        if (override_fullscreen)
        {   fullscreen = setting_fullscreen;
            wsm = fullscreen ? 1 : 0;
        }
        loadhiscores();
    }

    if (!d3d && filter == FILTER_3D)
    {   filter = FILTER_NONE;
    }

    if (reassociate)
    {   doassociations();
    }
    leftwidth       = GetSystemMetrics(SM_CXSIZEFRAME); // normally 4  (Win9x) or  4 (WinXP) or  8/ 9 (Win7) or 4 (Win8.1)
    bottomheight    = GetSystemMetrics(SM_CYSIZEFRAME); // normally 4? (Win9x) or  4 (WinXP) or  8/ 9 (Win7) or 4 (Win8.1)
    menuheight      = GetSystemMetrics(SM_CYMENU);      // normally 19 (Win9x) or 20 (WinXP) or 20/25 (Win7)
    titleheight     = GetSystemMetrics(SM_CYCAPTION);   // normally 19 (Win9x) or 26 (WinXP) or 22/27 (Win7)
 // scrollerheight  = GetSystemMetrics(SM_CYHSCROLL);   // normally  ? (Win9x) or 17 (WinXP) or  ?/21 (Win7)
    toolbarheight   = 30; // 12 + bitmap height. We get an exact value later
    debuggerheight  = 17;
    statusbarheight = 23; // 21 on WinXP?
    /* GetSystemMetrics() doesn't seem to support those.

       SM_CXSIZEFRAME, SM_CYSIZEFRAME are normally 4.
       SM_CXBORDER,    SM_CXBORDER    are normally 1. */

    hFont = CreateFont
    (   14,                     // int nHeight
        0,                      // int nWidth
        0,                      // int nEscapement
        0,                      // int nOrientation
        FW_MEDIUM,              // int nWeight
        FALSE,                  // BYTE bItalic
        FALSE,                  // BYTE bUnderline
        0,                      // BYTE cStrikeOut
        ANSI_CHARSET,           // BYTE nCharSet
        OUT_DEFAULT_PRECIS,     // BYTE nOutPrecision
        CLIP_CHARACTER_PRECIS,  // BYTE nClipPrecision
        DEFAULT_QUALITY,        // BYTE nQuality
        FF_SWISS,               // BYTE nPitchAndFamily
        "MS Sans Serif"         // LPCTSTR lpszFacename
    );
    hSmallFont = CreateFont
    (   8,                      // int nHeight
        0,                      // int nWidth
        0,                      // int nEscapement
        0,                      // int nOrientation
        FW_MEDIUM,              // int nWeight
        FALSE,                  // BYTE bItalic
        FALSE,                  // BYTE bUnderline
        0,                      // BYTE cStrikeOut
        ANSI_CHARSET,           // BYTE nCharSet
        OUT_DEFAULT_PRECIS,     // BYTE nOutPrecision
        CLIP_CHARACTER_PRECIS,  // BYTE nClipPrecision
        DEFAULT_QUALITY,        // BYTE nQuality
        FF_SWISS,               // BYTE nPitchAndFamily
        "Arial"                 // LPCTSTR lpszFacename (doesn't work with "MS Sans Serif")
    );
    hBoldFont = CreateFont
    (   14,                     // int nHeight
        0,                      // int nWidth
        0,                      // int nEscapement
        0,                      // int nOrientation
        FW_EXTRABOLD,           // int nWeight
        FALSE,                  // BYTE bItalic
        FALSE,                  // BYTE bUnderline
        0,                      // BYTE cStrikeOut
        ANSI_CHARSET,           // BYTE nCharSet
        OUT_DEFAULT_PRECIS,     // BYTE nOutPrecision
        CLIP_CHARACTER_PRECIS,  // BYTE nClipPrecision
        DEFAULT_QUALITY,        // BYTE nQuality
        FF_SWISS,               // BYTE nPitchAndFamily
        "MS Sans Serif"         // LPCTSTR lpszFacename
    );
    hGreekFont = CreateFont
    (   14,                     // int nHeight
        0,                      // int nWidth
        0,                      // int nEscapement
        0,                      // int nOrientation
        FW_MEDIUM,              // int nWeight
        FALSE,                  // BYTE bItalic
        FALSE,                  // BYTE bUnderline
        0,                      // BYTE cStrikeOut
        GREEK_CHARSET,          // BYTE nCharSet
        OUT_DEFAULT_PRECIS,     // BYTE nOutPrecision
        CLIP_CHARACTER_PRECIS,  // BYTE nClipPrecision
        DEFAULT_QUALITY,        // BYTE nQuality
        FF_SWISS,               // BYTE nPitchAndFamily
        "MS Sans Serif"         // LPCTSTR lpszFacename
    );
    hPolishFont = CreateFont
    (   14,                     // int nHeight
        0,                      // int nWidth
        0,                      // int nEscapement
        0,                      // int nOrientation
        FW_MEDIUM,              // int nWeight
        FALSE,                  // BYTE bItalic
        FALSE,                  // BYTE bUnderline
        0,                      // BYTE cStrikeOut
        EASTEUROPE_CHARSET,     // BYTE nCharSet
        OUT_DEFAULT_PRECIS,     // BYTE nOutPrecision
        CLIP_CHARACTER_PRECIS,  // BYTE nClipPrecision
        DEFAULT_QUALITY,        // BYTE nQuality
        FF_SWISS,               // BYTE nPitchAndFamily
        "MS Sans Serif"         // LPCTSTR lpszFacename
    );
    hRussianFont = CreateFont
    (   14,                     // int nHeight
        0,                      // int nWidth
        0,                      // int nEscapement
        0,                      // int nOrientation
        FW_MEDIUM,              // int nWeight
        FALSE,                  // BYTE bItalic
        FALSE,                  // BYTE bUnderline
        0,                      // BYTE cStrikeOut
        RUSSIAN_CHARSET,        // BYTE nCharSet
        OUT_DEFAULT_PRECIS,     // BYTE nOutPrecision
        CLIP_CHARACTER_PRECIS,  // BYTE nClipPrecision
        DEFAULT_QUALITY,        // BYTE nQuality
        FF_SWISS,               // BYTE nPitchAndFamily
        "MS Sans Serif"         // LPCTSTR lpszFacename
    );

    // we could DestroyIcon() these at exit but Windows does that automatically for us anyway
    bigicon    = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon), IMAGE_ICON, 32, 32, 0);
    smlicon    = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon), IMAGE_ICON, 16, 16, 0);
    icons[ 0]  = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_OK                ), IMAGE_ICON, 16, 16, 0);
    icons[ 1]  = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_CANCEL            ), IMAGE_ICON, 16, 16, 0);
    icons[ 2]  = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_CLEARHISCORES     ), IMAGE_ICON, 16, 16, 0);
    icons[ 3]  = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_REWIND            ), IMAGE_ICON, 16, 16, 0);
    icons[ 4]  = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_STOPTAPE          ), IMAGE_ICON, 16, 16, 0);
    icons[ 5]  = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_RECORD            ), IMAGE_ICON, 16, 16, 0);
    icons[ 6]  = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_PLAY              ), IMAGE_ICON, 16, 16, 0);
    icons[ 7]  = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_FFWD              ), IMAGE_ICON, 16, 16, 0);
    icons[ 8]  = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_EJECT             ), IMAGE_ICON, 16, 16, 0);
    icons[ 9]  = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_COPY              ), IMAGE_ICON, 16, 16, 0);
    icons[10]  = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_SAVESCRN          ), IMAGE_ICON, 16, 16, 0);
    icons[11]  = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_NEW               ), IMAGE_ICON, 16, 16, 0);
    icons[12]  = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_OPEN              ), IMAGE_ICON, 16, 16, 0);
    icons[13]  = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_SAVE              ), IMAGE_ICON, 16, 16, 0);
    icons[14]  = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_COPY              ), IMAGE_ICON, 16, 16, 0);

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = InstancePtr;
    wc.hIcon         = bigicon;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH) (COLOR_GRAYTEXT + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = smlicon;
    if (!RegisterClassEx(&wc))
    {   rq("Window registration failed!");
    }

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = TestWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = InstancePtr;
    wc.hIcon         = bigicon;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH) (COLOR_GRAYTEXT + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szTestClassName;
    wc.hIconSm       = smlicon;
    if (!RegisterClassEx(&wc))
    {   rq("Window registration failed!");
    }

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = MagnifierWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = InstancePtr;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH) (COLOR_GRAYTEXT + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szMagnifierClassName;
    wc.hIconSm       = NULL;
    if (!RegisterClassEx(&wc))
    {   rq("Window registration failed!");
    }

    for (i = 0; i < CANVASES; i++)
    {   memset(&CanvasBitMapInfo[i], 0, sizeof(CanvasBitMapInfo[i]));
        CanvasBitMapInfo[i].Header.biSize        = sizeof(BITMAPINFOHEADER);
        CanvasBitMapInfo[i].Header.biWidth       = canvas[i].width;
        CanvasBitMapInfo[i].Header.biHeight      = -canvas[i].height;
        CanvasBitMapInfo[i].Header.biBitCount    = 32;
        CanvasBitMapInfo[i].Header.biPlanes      = 1;
        CanvasBitMapInfo[i].Header.biCompression = BI_RGB;
        CanvasBitMapInfo[i].Colours[0]           = 0x00FF0000;
        CanvasBitMapInfo[i].Colours[1]           = 0x0000FF00;
        CanvasBitMapInfo[i].Colours[2]           = 0x000000FF;

        canvasdisplay[i] = malloc(canvas[i].width * canvas[i].height * sizeof(ULONG));
    }

    for (i = 0; i < MIBS; i++)
    {   mib[i].bitmap = LoadBitmap(InstancePtr, MAKEINTRESOURCE(mib[i].resource));
    }

    changemachine(machine, memmap, FALSE, TRUE, FALSE);
    allglyphs = (langs[language].codepage == CODEPAGE_ENG) ? TRUE : FALSE;
    load_catalog();
    translate();
    // Unfortunately we can't do load_catalog() earlier because it
    // (via generate_autotext()) relies on having coherence between the
    // machine and memmap variables, and changemachine() makes them
    // coherent.
    needaudit = TRUE;

    if (climode == 1)
    {   assemble();
        if (errors)
        {   cleanexit(EXIT_ERROR);
        } elif (warnings)
        {   cleanexit(EXIT_WARNING);
        } else
        {   cleanexit(EXIT_SUCCESS);
    }   }
    elif (climode == 2 || gotfilename)
    {   break_pathname(fn_game, path_games, file_game);
        rc = engine_load(FALSE);
        /* if (rc == 2)
        {   cleanexit(EXIT_SUCCESS);
        } else */ if (rc == 0)
        {   // don't beep, the user may have just wanted to set the directory.
            project_reset(FALSE);
        }
        if (climode == 2)
        {   disgame(0, 32767, (STRPTR) fn_game);
            cleanexit(EXIT_SUCCESS);
    }   }
    else
    {   DISCARD strcpy(fn_game, path_games);
        if (autosave)
        {   changefilepart(fn_game, path_games, file_game, "AUTOSAVE.COS");
            if (!engine_load(TRUE))
            {   // don't beep
                project_reset(FALSE);
        }   }
        else
        {   project_reset(FALSE);
    }   }

    update_opcodes();
    sound_reset();
    speech_setrate((int) (speech_rate - 10));
    ff_init();
    ff_initjoys();
    openwindow(TRUE);
    if (gotfilename)
    {   add_recent(); // must not be done until after opening the window
    }
    load_samples(); // must not be done until after opening the window
    // load_samples() must precede open_channels()
    open_channels(TOTALCHANNELS); // we keep the channels open permanently

    if (!paused) // must not be done until after loading the game of course
    {   sound_on(TRUE);
    }

#if AUTOPADS
    if (!loadedconfig)
    {   if (ffjoy2[0] && hostcontroller[0] == CONTROLLER_NONE)
        {   hostcontroller[0] = CONTROLLER_1STDJOY;
            updatemenu(MENUFAKE_LEFT);
        }
        if (ffjoy2[1] && hostcontroller[1] == CONTROLLER_NONE)
        {   hostcontroller[1] = CONTROLLER_2NDDJOY;
            updatemenu(MENUFAKE_RIGHT);
    }   }
#endif

    ProcessPtr    = GetCurrentProcess();
    MainThreadPtr = GetCurrentThread();
    setpri();

    for (i = 0; i < EMUBRUSHES; i++)
    {   hBrush[i] = CreateSolidBrush(EmuBrush[i]);
    }

    SystemParametersInfo(SPI_GETKEYBOARDCUES, 0, &b, 0);
    if (!b)
    {   b = TRUE;
        SystemParametersInfo(SPI_SETKEYBOARDCUES, 0, &b, 0);
    }

    if (startupupdates)
    {   help_update(TRUE);
    }

#ifdef COUNTBITSEQUENCES
    supercpu = 0;
    update_opcodes();
    j = 0;
    for (i = 0; i < 256; i++)
    {   if (i == 0x10 || i == 0x11 || i == 0x90 || i == 0x91 || i == 0xB6 || i == 0xB7 || i == 0xC4 || i == 0xC5 || i == 0xC6 || i == 0xC7)
        {   ;
        } elif (table_size_2650[i] == 1)
        {   j++;
        } elif (table_size_2650[i] == 2)
        {   j += 256;
        } elif (table_size_2650[i] == 3)
        {   j += 65536;
    }   }
    zprintf(TEXTPEN_VERBOSE, "2650A has %d valid bit masks.\n", j);
    supercpu = 1;
    update_opcodes();
    j = 0;
    for (i = 0; i < 256; i++)
    {   if (i == 0x90 || i == 0x91 || i == 0xB6 || i == 0xB7 || i == 0xC4 || i == 0xC5 || i == 0xC6 || i == 0xC7)
        {   ;
        } elif (table_size_2650[i] == 1)
        {   j++;
        } elif (table_size_2650[i] == 2)
        {   j += 256;
        } elif (table_size_2650[i] == 3)
        {   j += 65536;
    }   }
    zprintf(TEXTPEN_VERBOSE, "2650B has %d valid bit masks.\n", j);
#endif

    GetUserName(username, &usernamemax);

    if
    (   RexxRegisterFunctionExe("ACTIVATE"        , (RexxFunctionHandler*) rexx_activate        ) != RXFUNC_OK
     || RexxRegisterFunctionExe("ACTIVATEWINDOW"  , (RexxFunctionHandler*) rexx_activatewindow  ) != RXFUNC_OK
     || RexxRegisterFunctionExe("CLEAR"           , (RexxFunctionHandler*) rexx_clear           ) != RXFUNC_OK
     || RexxRegisterFunctionExe("CLIENT"          , (RexxFunctionHandler*) rexx_client          ) != RXFUNC_OK
     || RexxRegisterFunctionExe("CLOSECONTROLS"   , (RexxFunctionHandler*) rexx_closecontrols   ) != RXFUNC_OK
     || RexxRegisterFunctionExe("CLOSEDIPS"       , (RexxFunctionHandler*) rexx_closedips       ) != RXFUNC_OK
     || RexxRegisterFunctionExe("CLOSEDISKDRIVE"  , (RexxFunctionHandler*) rexx_closediskdrive  ) != RXFUNC_OK
     || RexxRegisterFunctionExe("CLOSEGAMEINFO"   , (RexxFunctionHandler*) rexx_closegameinfo   ) != RXFUNC_OK
     || RexxRegisterFunctionExe("CLOSEGAMEPADS"   , (RexxFunctionHandler*) rexx_closegamepads   ) != RXFUNC_OK
     || RexxRegisterFunctionExe("CLOSEKEYBOARD"   , (RexxFunctionHandler*) rexx_closekeyboard   ) != RXFUNC_OK
     || RexxRegisterFunctionExe("CLOSEMEMORY"     , (RexxFunctionHandler*) rexx_closememory     ) != RXFUNC_OK
     || RexxRegisterFunctionExe("CLOSEMONITOR"    , (RexxFunctionHandler*) rexx_closemonitor    ) != RXFUNC_OK
     || RexxRegisterFunctionExe("CLOSEMUSIC"      , (RexxFunctionHandler*) rexx_closemusic      ) != RXFUNC_OK
     || RexxRegisterFunctionExe("CLOSEOPCODES"    , (RexxFunctionHandler*) rexx_closeopcodes    ) != RXFUNC_OK
     || RexxRegisterFunctionExe("CLOSEPALETTE"    , (RexxFunctionHandler*) rexx_closepalette    ) != RXFUNC_OK
     || RexxRegisterFunctionExe("CLOSESPRITES"    , (RexxFunctionHandler*) rexx_closesprites    ) != RXFUNC_OK
     || RexxRegisterFunctionExe("CLOSETAPEDECK"   , (RexxFunctionHandler*) rexx_closetapedeck   ) != RXFUNC_OK
     || RexxRegisterFunctionExe("CMDSHELL"        , (RexxFunctionHandler*) rexx_cmdshell        ) != RXFUNC_OK
     || RexxRegisterFunctionExe("COPY"            , (RexxFunctionHandler*) rexx_copy            ) != RXFUNC_OK
     || RexxRegisterFunctionExe("COPYTEXT"        , (RexxFunctionHandler*) rexx_copytext        ) != RXFUNC_OK
     || RexxRegisterFunctionExe("DEACTIVATE"      , (RexxFunctionHandler*) rexx_deactivate      ) != RXFUNC_OK
     || RexxRegisterFunctionExe("DEBUGGER"        , (RexxFunctionHandler*) rexx_debugger        ) != RXFUNC_OK
     || RexxRegisterFunctionExe("GETATTR"         , (RexxFunctionHandler*) rexx_getattr         ) != RXFUNC_OK
     || RexxRegisterFunctionExe("GETCREDITS"      , (RexxFunctionHandler*) rexx_getcredits      ) != RXFUNC_OK
     || RexxRegisterFunctionExe("GETHOSTNAME"     , (RexxFunctionHandler*) rexx_gethostname     ) != RXFUNC_OK
     || RexxRegisterFunctionExe("GETMACHINE"      , (RexxFunctionHandler*) rexx_getmachine      ) != RXFUNC_OK
     || RexxRegisterFunctionExe("GETMEMMAP"       , (RexxFunctionHandler*) rexx_getmemmap       ) != RXFUNC_OK
     || RexxRegisterFunctionExe("GETNAME"         , (RexxFunctionHandler*) rexx_getname         ) != RXFUNC_OK
     || RexxRegisterFunctionExe("GETPORT"         , (RexxFunctionHandler*) rexx_getport         ) != RXFUNC_OK
     || RexxRegisterFunctionExe("GETREFERENCE"    , (RexxFunctionHandler*) rexx_getreference    ) != RXFUNC_OK
     || RexxRegisterFunctionExe("GETVERSION"      , (RexxFunctionHandler*) rexx_getversion      ) != RXFUNC_OK
     || RexxRegisterFunctionExe("HELP"            , (RexxFunctionHandler*) rexx_help            ) != RXFUNC_OK
     || RexxRegisterFunctionExe("HIDE"            , (RexxFunctionHandler*) rexx_deactivate      ) != RXFUNC_OK
     || RexxRegisterFunctionExe("JUMPTOMONITOR"   , (RexxFunctionHandler*) rexx_jumptomonitor   ) != RXFUNC_OK
     || RexxRegisterFunctionExe("NEW"             , (RexxFunctionHandler*) rexx_new             ) != RXFUNC_OK
     || RexxRegisterFunctionExe("MENU"            , (RexxFunctionHandler*) rexx_menu            ) != RXFUNC_OK
     || RexxRegisterFunctionExe("MOVEWINDOW"      , (RexxFunctionHandler*) rexx_movewindow      ) != RXFUNC_OK
     || RexxRegisterFunctionExe("OPEN"            , (RexxFunctionHandler*) rexx_open            ) != RXFUNC_OK
     || RexxRegisterFunctionExe("OPENCONTROLS"    , (RexxFunctionHandler*) rexx_opencontrols    ) != RXFUNC_OK
     || RexxRegisterFunctionExe("OPENDIPS"        , (RexxFunctionHandler*) rexx_opendips        ) != RXFUNC_OK
     || RexxRegisterFunctionExe("OPENDISKDRIVE"   , (RexxFunctionHandler*) rexx_opendiskdrive   ) != RXFUNC_OK
     || RexxRegisterFunctionExe("OPENGAMEPADS"    , (RexxFunctionHandler*) rexx_opengamepads    ) != RXFUNC_OK
     || RexxRegisterFunctionExe("OPENGAMEINFO"    , (RexxFunctionHandler*) rexx_opengameinfo    ) != RXFUNC_OK
     || RexxRegisterFunctionExe("OPENKEYBOARD"    , (RexxFunctionHandler*) rexx_openkeyboard    ) != RXFUNC_OK
     || RexxRegisterFunctionExe("OPENMEMORY"      , (RexxFunctionHandler*) rexx_openmemory      ) != RXFUNC_OK
     || RexxRegisterFunctionExe("OPENMONITOR"     , (RexxFunctionHandler*) rexx_openmonitor     ) != RXFUNC_OK
     || RexxRegisterFunctionExe("OPENMUSIC"       , (RexxFunctionHandler*) rexx_openmusic       ) != RXFUNC_OK
     || RexxRegisterFunctionExe("OPENOPCODES"     , (RexxFunctionHandler*) rexx_openopcodes     ) != RXFUNC_OK
     || RexxRegisterFunctionExe("OPENPALETTE"     , (RexxFunctionHandler*) rexx_openpalette     ) != RXFUNC_OK
     || RexxRegisterFunctionExe("OPENSPRITES"     , (RexxFunctionHandler*) rexx_opensprites     ) != RXFUNC_OK
     || RexxRegisterFunctionExe("OPENTAPEDECK"    , (RexxFunctionHandler*) rexx_opentapedeck    ) != RXFUNC_OK
     || RexxRegisterFunctionExe("PASTE"           , (RexxFunctionHandler*) rexx_paste           ) != RXFUNC_OK
     || RexxRegisterFunctionExe("PAUSE"           , (RexxFunctionHandler*) rexx_pause           ) != RXFUNC_OK
     || RexxRegisterFunctionExe("PEEK"            , (RexxFunctionHandler*) rexx_peek            ) != RXFUNC_OK
     || RexxRegisterFunctionExe("QUIT"            , (RexxFunctionHandler*) rexx_quit            ) != RXFUNC_OK
     || RexxRegisterFunctionExe("RELOAD"          , (RexxFunctionHandler*) rexx_revert          ) != RXFUNC_OK
     || RexxRegisterFunctionExe("REVERT"          , (RexxFunctionHandler*) rexx_revert          ) != RXFUNC_OK
     || RexxRegisterFunctionExe("SAVEACBM"        , (RexxFunctionHandler*) rexx_saveacbm        ) != RXFUNC_OK
     || RexxRegisterFunctionExe("SAVEAS"          , (RexxFunctionHandler*) rexx_saveas          ) != RXFUNC_OK
     || RexxRegisterFunctionExe("SAVEBMP"         , (RexxFunctionHandler*) rexx_savebmp         ) != RXFUNC_OK
     || RexxRegisterFunctionExe("SAVEGIF"         , (RexxFunctionHandler*) rexx_savegif         ) != RXFUNC_OK
     || RexxRegisterFunctionExe("SAVEILBM"        , (RexxFunctionHandler*) rexx_saveilbm        ) != RXFUNC_OK
     || RexxRegisterFunctionExe("SAVEPCX"         , (RexxFunctionHandler*) rexx_savepcx         ) != RXFUNC_OK
     || RexxRegisterFunctionExe("SAVEPNG"         , (RexxFunctionHandler*) rexx_savepng         ) != RXFUNC_OK
     || RexxRegisterFunctionExe("SAVETIFF"        , (RexxFunctionHandler*) rexx_savetiff        ) != RXFUNC_OK
     || RexxRegisterFunctionExe("SERVER"          , (RexxFunctionHandler*) rexx_server          ) != RXFUNC_OK
     || RexxRegisterFunctionExe("SETMACHINE"      , (RexxFunctionHandler*) rexx_setmachine      ) != RXFUNC_OK
     || RexxRegisterFunctionExe("SETPORT"         , (RexxFunctionHandler*) rexx_setport         ) != RXFUNC_OK
     || RexxRegisterFunctionExe("SETSPEED"        , (RexxFunctionHandler*) rexx_setspeed        ) != RXFUNC_OK
     || RexxRegisterFunctionExe("SHOW"            , (RexxFunctionHandler*) rexx_activate        ) != RXFUNC_OK
     || RexxRegisterFunctionExe("TURBO"           , (RexxFunctionHandler*) rexx_turbo           ) != RXFUNC_OK
     || RexxRegisterFunctionExe("UNPAUSE"         , (RexxFunctionHandler*) rexx_unpause         ) != RXFUNC_OK
     || RexxRegisterFunctionExe("VIEWHIGHSCORES"  , (RexxFunctionHandler*) rexx_viewhighscores  ) != RXFUNC_OK
     || RexxRegisterFunctionExe("WINDOWTOBACK"    , (RexxFunctionHandler*) rexx_windowtoback    ) != RXFUNC_OK
     || RexxRegisterFunctionExe("WINDOWTOFRONT"   , (RexxFunctionHandler*) rexx_windowtofront   ) != RXFUNC_OK
    )
    {   zprintf(TEXTPEN_ERROR, LLL(MSG_REXXERROR1, "Can't register REXX function(s)!\n\n"));
    }
    if (RexxRegisterExitExe("RXSIO", rexx_rxsio, NULL) != RXEXIT_OK)
    {   zprintf(TEXTPEN_ERROR, LLL(MSG_REXXERROR2, "Can't register REXX I/O handler!\n\n"));
    }
    if (fn_script[0])
    {   macro_rexx(FALSE, TRUE);
    } elif (showtod)
    {   showthetod();
    }

    reset_fps();
    emulating = TRUE;
    for (;;)
    {   if (paused)
        {   wa_checkinput();
            process_code();
            update_controlstip();
        } else
        {   wait_first();
            emulate();
            wait_second();
    }   }

    // control can never reach here

#ifndef _lint
    return 0; // Visual C gets upset if this return is missing, Lint gets upset if it is present :-(
#endif
}

EXPORT ULONG getsize(const STRPTR filename)
{   HANDLE hFile /* = NULL */ ;
    ULONG  localsize;

    hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {   return 0;
    }
    localsize = GetFileSize(hFile, NULL);
    DISCARD CloseHandle(hFile);
    // hFile = NULL;

    if (localsize == (ULONG) -1)
    {   return 0;
    }

    return localsize;
}

EXPORT void wa_checkinput(void)
{   TRANSIENT int   dest, i;
    TRANSIENT MSG   Msg;
    TRANSIENT ULONG scancode;
    PERSIST   FLAG  fresh[2][2] = { { TRUE, TRUE },
                                    { TRUE, TRUE } };

    Sleep(0);
    /* We need to call this regularly even while paused, to check whether the
       user is trying to eg. unpause via host gamepad.
    if (paused)
    {   DISCARD WaitMessage(); // to give better multitasking
    } */
    refresh_quals();

    while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
    {   switch (Msg.message)
        {
        case WM_KEYDOWN:
            scancode = (Msg.lParam & 33488896) >> 16;
            if
            (   (SubWindowPtr[SUBWINDOW_TAPEDECK  ] && GetActiveWindow() == SubWindowPtr[SUBWINDOW_TAPEDECK  ])
             || (SubWindowPtr[SUBWINDOW_INDUSTRIAL] && GetActiveWindow() == SubWindowPtr[SUBWINDOW_INDUSTRIAL])
            )
            {   handle_escdown(scancode, GetActiveWindow());
            } else
            {   handle_keydown(scancode);
            }
        acase WM_SYSKEYDOWN:
            scancode = (Msg.lParam & 33488896) >> 16;
            if (Msg.lParam & (1 << 29))
            {   storedaltcode = scancode;
            } elif
            (   (SubWindowPtr[SUBWINDOW_TAPEDECK  ] && GetActiveWindow() == SubWindowPtr[SUBWINDOW_TAPEDECK])
             || (SubWindowPtr[SUBWINDOW_INDUSTRIAL] && GetActiveWindow() == SubWindowPtr[SUBWINDOW_INDUSTRIAL])
            )
            {   ;
            } else
            {   handle_keydown(scancode);
            }
        acase WM_KEYUP:
            scancode = (Msg.lParam & 33488896) >> 16;
            if
            (   (SubWindowPtr[SUBWINDOW_TAPEDECK  ] && GetActiveWindow() == SubWindowPtr[SUBWINDOW_TAPEDECK  ])
             || (SubWindowPtr[SUBWINDOW_INDUSTRIAL] && GetActiveWindow() == SubWindowPtr[SUBWINDOW_INDUSTRIAL])
            )
            {   handle_escup(scancode);
            } else
            {   handle_keyup(scancode);
            }
        acase WM_RBUTTONUP:
            if (!guestrmb && GetActiveWindow() != SubWindowPtr[SUBWINDOW_MUSIC])
            {   SetCursor(hArrow);
                if (!showpointers[wsm])
                {   showpointers[wsm] = TRUE;
                    docommand(MENUITEM_POINTER);
                }
                DISCARD ClipCursor(NULL);

                ThePoint.x = LOWORD(Msg.lParam);
                ThePoint.y = HIWORD(Msg.lParam);
                DISCARD ClientToScreen(GetActiveWindow(), &ThePoint);
                TrackPopupMenu
                (   PopUpMenuPtr,
                    TPM_RIGHTBUTTON,
                    ThePoint.x,
                    ThePoint.y,
                    0,
                    MainWindowPtr,
                    NULL
                );

                updatepointer(FALSE, TRUE);
        }   }

        TranslateMessage(&Msg);
        DISCARD DispatchMessage(&Msg);
    }

    ReadJoystick(0);
    ReadJoystick(1);

    if
    (   !crippled
     && (   (jff[0] & JOYRESET)
         || (jff[1] & JOYRESET)
    )   )
    {   storedcode = SCAN_F5; // ie. pending |= PENDING_RESET;
    }

    for (i = 0; i < 2; i++)
    {   // find out which player this is
        if
        (   (i == 0 && hostcontroller[0] == CONTROLLER_1STDJOY) // if this is 1st host USB and "Left controller" is "1st host USB"
         || (i == 1 && hostcontroller[0] == CONTROLLER_2NDDJOY) // if this is 2nd host USB and "Left controller" is "2nd host USB"
        )
        {   dest = 0; // left player (at least)
            if
            (   (i == 0 && hostcontroller[1] == CONTROLLER_1STDJOY) // if this is 1st host USB and "Right controller" is "1st host USB"
             || (i == 1 && hostcontroller[1] == CONTROLLER_2NDDJOY) // if this is 2nd host USB and "Right controller" is "2nd host USB"
            )
            {   dest = 3; // both players
        }   }
        elif
        (   (i == 0 && hostcontroller[1] == CONTROLLER_1STDJOY) // if this is 1st host USB and "Right controller" is "1st host USB"
         || (i == 1 && hostcontroller[1] == CONTROLLER_2NDDJOY) // if this is 2nd host USB and "Right controller" is "2nd host USB"
        )
        {   dest = 1; // right player only
        } else
        {   dest = 2; // ignore
        }
        if (swapped && dest <= 1)
        {   dest = 1 - dest; // 0 -> 1 or 1 -> 0
        }

        if (dest != 2 && (jff[i] & JOYPAUSE))
        {   if (fresh[i][0])
            {   fresh[i][0] = FALSE;
                if (paused)
                {   emu_unpause();
                } else
                {   emu_pause();
        }   }   }
        else
        {   fresh[i][0] = TRUE;
        }

        if (dest != 2 && (jff[i] & JOYAUTOFIRE))
        {   if (fresh[i][1])
            {   fresh[i][1] = FALSE;
                if (dest == 0 || dest == 3)
                {   autofire[0] = autofire[0] ? FALSE : TRUE;
                    docommand(MENUITEM_AUTOFIRE1);
                }
                if (dest == 1 || dest == 3)
                {   autofire[1] = autofire[1] ? FALSE : TRUE;
                    docommand(MENUITEM_AUTOFIRE2);
        }   }   }
        else
        {   fresh[i][1] = TRUE;
    }   }

    measure_position();

    if (paused && repaintmemmap)
    {   update_memory(FALSE);
    }

    if (cheevos)
    {
#ifdef LOGALLCHEEVOS
        zprintf(TEXTPEN_VERBOSE, "RA_ProcessInputs()\n");
#endif
        RA_ProcessInputs();
    }

    return; // return Msg.wParam;
}

EXPORT UBYTE KeyDown(UWORD scancode) // scancode must be at least 16 bits wide!
{   if (KeyMatrix[scancode / 8] & (1 << (scancode % 8)))
    {   return 1;
    }
    return 0;
}

EXPORT void freeall(void)
{   int i;

    opening = TRUE;
    EXITING(1);
    sound_off(FALSE);
    close_subwindows(TRUE);
    close_magnifier();
    EXITING(2);
    macro_stop(); // must be done *after* sound_off()
    EXITING(3);
    tape_eject();
    papertape_eject(0);
    papertape_eject(1);
    EXITING(4);
    ff_off();
    ff_uninitjoys();
    ff_die();
    EXITING(5);
    printer_savepartial(0);
    printer_savepartial(1);
    EXITING(6);
    for (i = 0; i < GUESTCHANNELS; i++)
    {   sound_close(i);
    }
    EXITING(7);
    NetPlay(NET_OFF);
    if (cheevos)
    {   remove_cheevos(FALSE);
        cheevos = TRUE; // we turn it back on so that it is saved correctly in .ini file
    }
    EXITING(8);
    DISCARD ClipCursor(NULL);
    EXITING(9);
    if (DisHandle)
    {   DISCARD fclose(DisHandle);
        DisHandle = NULL;
    }
    EXITING(10);
    engine_quit();
    EXITING(11);
    if (RGBtoYUV)
    {   free(RGBtoYUV);
        RGBtoYUV = NULL;
    }
    EXITING(12);
    sound_die();
    EXITING(13);
    for (i = 0; i < MIBS; i++)
    {   if (mib[i].bitmap)
        {   DeleteObject(mib[i].bitmap);
            mib[i].bitmap = NULL;
    }   }
    EXITING(14);
    for (i = 0; i < EMUBRUSHES; i++)
    {   if (hBrush[i])
        {   DeleteObject(hBrush[i]);
            hBrush[i] = NULL;
    }   }
    EXITING(15);
    if (!upgrade)
    {   if (!lockconfig)
        {   saveconfig();
        }
        EXITING(16);
        savehiscores();
    }
    EXITING(17);
    if (stars)
    {   free(stars);
        EXITING(18);
        stars = NULL;
    }
    EXITING(19);
    if (pixelubyte)
    {   free(pixelubyte);
        pixelubyte = NULL;
        pixelulong = NULL;
    }
    for (i = 0; i < CANVASES; i++)
    {   if (canvasdisplay[i])
        {   free(canvasdisplay[i]);
            canvasdisplay[i] = NULL;
    }   }
    EXITING(20);
    freelanguage();
    EXITING(21);
    speech_die();
    EXITING(22);
    cleanexit_3d();
    EXITING(23);
    // Do we need to free listview nodes? It seems not.
    UnhookWindowsHookEx(g_hKeyboardHook);
    EXITING(24);
    if (iconified)
    {   uniconify(TRUE);
    }
    EXITING(25);
    DISCARD WSACleanup();
    EXITING(26);
    RexxDeregisterFunction("ACTIVATE");
    RexxDeregisterFunction("ACTIVATEWINDOW");
    RexxDeregisterFunction("CLEAR");
    RexxDeregisterFunction("CLIENT");
    RexxDeregisterFunction("CLOSECONTROLS");
    RexxDeregisterFunction("CLOSEDIPS");
    RexxDeregisterFunction("CLOSEDISKDRIVE");
    RexxDeregisterFunction("CLOSEGAMEINFO");
    RexxDeregisterFunction("CLOSEGAMEPADS");
    RexxDeregisterFunction("CLOSEKEYBOARD");
    RexxDeregisterFunction("CLOSEMEMORY");
    RexxDeregisterFunction("CLOSEMONITOR");
    RexxDeregisterFunction("CLOSEMUSIC");
    RexxDeregisterFunction("CLOSEOPCODES");
    RexxDeregisterFunction("CLOSEPALETTE");
    RexxDeregisterFunction("CLOSESPRITES");
    RexxDeregisterFunction("CLOSETAPEDECK");
    RexxDeregisterFunction("CMDSHELL");
    RexxDeregisterFunction("COPY");
    RexxDeregisterFunction("COPYTEXT");
    RexxDeregisterFunction("DEACTIVATE");
    RexxDeregisterFunction("DEBUGGER");
    RexxDeregisterFunction("GETATTR");
    RexxDeregisterFunction("GETCREDITS");
    RexxDeregisterFunction("GETHOSTNAME");
    RexxDeregisterFunction("GETMACHINE");
    RexxDeregisterFunction("GETMEMMAP");
    RexxDeregisterFunction("GETNAME");
    RexxDeregisterFunction("GETPORT");
    RexxDeregisterFunction("GETREFERENCE");
    RexxDeregisterFunction("GETVERSION");
    RexxDeregisterFunction("HELP");
    RexxDeregisterFunction("HIDE");
    RexxDeregisterFunction("JUMPTOMONITOR");
    RexxDeregisterFunction("MENU");
    RexxDeregisterFunction("MOVEWINDOW");
    RexxDeregisterFunction("NEW");
    RexxDeregisterFunction("OPEN");
    RexxDeregisterFunction("OPENCONTROLS");
    RexxDeregisterFunction("OPENDIPS");
    RexxDeregisterFunction("OPENDISKDRIVE");
    RexxDeregisterFunction("OPENGAMEPADS");
    RexxDeregisterFunction("OPENGAMEINFO");
    RexxDeregisterFunction("OPENKEYBOARD");
    RexxDeregisterFunction("OPENMEMORY");
    RexxDeregisterFunction("OPENMONITOR");
    RexxDeregisterFunction("OPENMUSIC");
    RexxDeregisterFunction("OPENOPCODES");
    RexxDeregisterFunction("OPENPALETTE");
    RexxDeregisterFunction("OPENSPRITES");
    RexxDeregisterFunction("OPENTAPEDECK");
    RexxDeregisterFunction("PASTE");
    RexxDeregisterFunction("PAUSE");
    RexxDeregisterFunction("PEEK");
    RexxDeregisterFunction("QUIT");
    RexxDeregisterFunction("RELOAD");
    RexxDeregisterFunction("REVERT");
    RexxDeregisterFunction("SAVEACBM");
    RexxDeregisterFunction("SAVEAS");
    RexxDeregisterFunction("SAVEBMP");
    RexxDeregisterFunction("SAVEGIF");
    RexxDeregisterFunction("SAVEILBM");
    RexxDeregisterFunction("SAVEPCX");
    RexxDeregisterFunction("SAVEPNG");
    RexxDeregisterFunction("SAVETIFF");
    RexxDeregisterFunction("SERVER");
    RexxDeregisterFunction("SETMACHINE");
    RexxDeregisterFunction("SETPORT");
    RexxDeregisterFunction("SETSPEED");
    RexxDeregisterFunction("SHOW");
    RexxDeregisterFunction("TURBO");
    RexxDeregisterFunction("UNPAUSE");
    RexxDeregisterFunction("VIEWHIGHSCORES");
    RexxDeregisterFunction("WINDOWTOBACK");
    RexxDeregisterFunction("WINDOWTOFRONT");
    RexxDeregisterExit("WINARCADIA", "RXSIO");
    EXITING(28);
    blanker_on(); // do this *after* uniconify()
    EXITING(29);
    for (i = 0; i < BEZELS; i++)
    {   DeleteObject(BezelBitMap[i]);
    }
    DeleteObject(hFont);
    DeleteObject(hSmallFont);
    DeleteObject(hBoldFont);
    DeleteObject(hGreekFont);
    DeleteObject(hPolishFont);
    DeleteObject(hRussianFont);
    EXITING(30);
    DeleteObject(hMonospaceFont);
}

MODULE void upgradeconfigs(void)
{   WIN32_FIND_DATA FindData;
    HANDLE          FindHandle;
    int             thelength;

    DISCARD SetCurrentDirectory("Configs");

    FindHandle = FindFirstFile("*", &FindData);
    if (FindHandle == INVALID_HANDLE_VALUE)
    {   DISCARD SetCurrentDirectory(ProgDir);
        // printf("No files found!\n"); // not zprintf()!
        cleanexit(EXIT_FAILURE);
    }

    do
    {   if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        {   thelength = (int) strlen(FindData.cFileName);
            if
            (   thelength > 4
             && !stricmp(&FindData.cFileName[thelength - 4], ".ini")
            )
            {   strcpy(fn_config, FindData.cFileName);
                // printf("Upgrading %s...", FindData.cFileName); // not zprintf()!
                loadconfig();
                saveconfig();
                // printf("done.\n"); // not zprintf()!
            } else
            {   ; // printf("Ignoring %s...", FindData.cFileName); // not zprintf()!
    }   }   }
    while (FindNextFile(FindHandle, &FindData));
    DISCARD FindClose(FindHandle);

    // zprintf(TEXTPEN_VERBOSE, "All done.\n");
    DISCARD SetCurrentDirectory(ProgDir);
    cleanexit(EXIT_SUCCESS);
}

EXPORT void cleanexit(SBYTE rc)
{   TEXT buffer[MAX_PATH];

    if (rc == EXIT_SUCCESS && cheevos)
    {
#ifdef LOGCHEEVOS
        zprintf(TEXTPEN_VERBOSE, "RA_ConfirmLoadNewRom(TRUE)\n");
#endif
        if (!RA_ConfirmLoadNewRom(TRUE))
        {   return;
    }   }

    if (SubWindowPtr[SUBWINDOW_OUTPUT])
    {   quitting = TRUE;
        zprintf(TEXTPEN_DEFAULT, "%s.\n", LLL(MSG_QUITTING, "Quitting"));
    }

    freeall();
    // PostMessage(MainWindowPtr, WM_CLOSE, 0, 0);
    DestroyWindow(MainWindowPtr); // this will also destroy all child windows
    // MainWindowPtr = NULL;

    if (consoleopened && GetCurrentDirectory(MAX_PATH, buffer))
    {   printf("%s>", buffer); // because Windows isn't showing the command prompt for some reason
    }

    exit(rc);
}

EXPORT void calc_size(void)
{   int excess,
        ratiox,    ratioy,
        realsizex, realsizey,
        tempwidth, tempheight;

/* x/yoffset           = distance from left/top edge of window to left/top edge of guest screen
   DisplayWidth/Height = actual size of user's host screen (desktop)
   winwidth/height     = overall window dimensions
   clientwidth/height  = dimensions of window excluding eg. gadgets (only used in full-screen mode)
   rastwidth/height    = dimensions of guest screen
   winleftx/topy       = screen coordinate of left/top edge of window
   sourcewidth/height  = machine dimensions multiplied by integer scalings

   All quantities are in host pixels. */

    calc_margins();

    if (fullscreen)
    {   winwidth  = clientwidth  = rastwidth  = DisplayWidth;
        winheight = clientheight = rastheight = DisplayHeight;

        if (showtitlebars[wsm])
        {   clientheight -= (titleheight + 6);
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "Titlebar is %d pixels tall.\n", titleheight);
#endif
        }
        if (showmenubars[wsm])
        {   clientheight -= menuheight;
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "Menu bar is %d pixels tall.\n", menuheight);
#endif
        }
        if (showtoolbars[wsm])
        {   clientheight -= toolbarheight;
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "Toolbar is %d pixels tall.\n", toolbarheight);
#endif
        }
        if (showdebugger[wsm])
        {   clientheight -= debuggerheight;
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "Debugger is %d pixels tall.\n", debuggerheight);
#endif
        }
        if (showstatusbars[wsm])
        {   clientheight -= statusbarheight;
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "Status bar is %d pixels tall.\n", statusbarheight);
#endif
        }
        if (showsidebars[wsm])
        {   sidebarwidth = SIDEBARWIDTH; // in case the user has made the sidebar very wide
            // Ideally, we would remember the old value, and restore it when going back to windowed mode
            clientwidth  -= sidebarwidth + leftwidth;
            if (!showtitlebars[wsm])
            {   sidebarwidth += leftwidth;
            }
            clientwidth -= 2; // so that sidebar divider pane is visible
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "Sidebar is %d pixels wide.\n", sidebarwidth);
#endif
        }

#ifdef VERBOSE
        zprintf
        (   TEXTPEN_VERBOSE,
            "Screen is %d*%d pixels. Client area of window is %d*%d.\n",
            DisplayWidth,
            DisplayHeight,
            clientwidth,
            clientheight
        );
#endif

        realwide  = machines[machine].wide;
        realsizex = clientwidth  / (machines[machine].width * realwide);
        if (realsizex > 4)
        {   realsizex = 4; // because fullscreen unstretched si50 could be 5x or more graphics scaling!
        }
        realsizey = clientheight /  machines[machine].height;
        if (realsizey > 4)
        {   realsizey = 4;
        }
        if (realsizey < realsizex)
        {   realsize = realsizey;
        } else
        {   realsize = realsizex;
        }
        fastwide     =          realsize                 * realwide;
        rastwidth    = (SWORD) (machines[machine].width  * fastwide);
        rastheight   = (SWORD) (machines[machine].height * realsize);

        if (stretching != STRETCHING_NONE && filter != FILTER_3D)
        {   if (stretching == STRETCHING_43)
            {   if (machines[machine].coinop)
                {   if (!memmapinfo[memmap].rotate || rotating) // portrait orientation
                    {   xoffset = (clientwidth - (clientheight * 4 / 5)) / 2; // 4:5
                    } else // landscape orientation
                    {   xoffset = (clientwidth - (clientheight * 5 / 4)) / 2; // 5:4
                }   }
                else
                {   xoffset     = (clientwidth - (clientheight * 4 / 3)) / 2; // 4:3
            }   }
            else
            {   xoffset = 0;
        }   }
        else
        {   xoffset = (clientwidth - rastwidth ) / 2;
        }
        if (xoffset < 0) // eg. very narrow (<4:3) host screen trying to show 4:3 screen
        {   xoffset = 0;
        }
        sourcewidth  = destwidth  = rastwidth;
        sourceheight = destheight = rastheight;
    } else
    {   winwidth  = sourcewidth  = rastwidth  = machines[machine].width  * size * wide;
        winheight = sourceheight = rastheight = machines[machine].height * size;

        if (BEZELABLE && bezel)
        {   switch (machine)
            {
            case PIPBUG:
                winwidth          += (    VT100_BEZELWIDTH  -              640) * size * wide;
                winheight         += (    VT100_BEZELHEIGHT -              288) * size;
                if (showleds)
                {   winheight     -= PIPBUG_LEDHEIGHT;
                }
            acase TWIN:
                winwidth          += (     TWIN_BEZELWIDTH  -              560) * size * wide;
                winheight         += (     TWIN_BEZELHEIGHT -              250) * size;
            acase ZACCARIA:
                switch (memmap)
                {
                case  MEMMAP_ASTROWARS:
                    if (rotating)
                    {   winwidth  += (ASTROWARS_BEZELWIDTH  - COINOP_BOXWIDTH ) * size * wide;
                        winheight += (ASTROWARS_BEZELHEIGHT - COINOP_BOXHEIGHT) * size;
                    } else
                    {   winwidth  += (ASTROWARS_BEZELHEIGHT - COINOP_BOXHEIGHT) * size * wide;
                        winheight += (ASTROWARS_BEZELWIDTH  - COINOP_BOXWIDTH ) * size;
                    }
                acase MEMMAP_GALAXIA:
                    if (rotating)
                    {   winwidth  += (  GALAXIA_BEZELWIDTH  - COINOP_BOXWIDTH ) * size * wide;
                        winheight += (  GALAXIA_BEZELHEIGHT - COINOP_BOXHEIGHT) * size;
                    } else
                    {   winwidth  += (  GALAXIA_BEZELHEIGHT - COINOP_BOXHEIGHT) * size * wide;
                        winheight += (  GALAXIA_BEZELWIDTH  - COINOP_BOXWIDTH ) * size;
                    }
                acase MEMMAP_LASERBATTLE:
                case  MEMMAP_LAZARIAN:
                    winwidth  += ( LAZARIAN_BEZELWIDTH  - COINOP_BOXWIDTH ) * size * wide;
                    winheight += ( LAZARIAN_BEZELHEIGHT - COINOP_BOXHEIGHT) * size;
        }   }   }

        if
        (   stretch43
         && (   machine == ARCADIA
             || machines[machine].pvis
             || machine == PIPBUG
             || machine == BINBUG
             || machine == TWIN
             || machine == CD2650
             || machine == PHUNSY
             || machine == SELBST
             || machine == PONG
        )   ) // ie. only machines with a 4:3 TV-style display (not INSTRUCTOR, MIKIT, TYPERIGHT)
        {   tempwidth  = machines[machine].width * wide;
            tempheight = machines[machine].height;
            if (showleds)
            {   switch (machine)
                {
                case  PIPBUG: tempheight -= PIPBUG_LEDHEIGHT;
                acase BINBUG: tempheight -= BINBUG_LEDHEIGHT;
                acase PHUNSY: tempheight -= PHUNSY_LEDHEIGHT;
                acase SELBST: tempheight -= SELBST_LEDHEIGHT;
            }   }
            if
            (   machine == ARCADIA
             || machine == INTERTON
             || machine == ELEKTOR
             || machine == PONG
            )
            {   ratiox     = (wide == 2) ?  4 : 2; // 2:3 (narrow) or  4:3 (wide)
                ratioy     = 3;
            } elif (machines[machine].coinop)
            {   if (!memmapinfo[memmap].rotate || rotating) // portrait orientation
                {   ratiox = (wide == 2) ?  8 : 4; // 4:5 (narrow) or  8:5 (wide)
                    ratioy = 5;
                } else // landscape orientation
                {   ratiox = (wide == 2) ? 10 : 5; // 5:4 (narrow) or 10:4 (wide)
                    ratioy = 4;
            }   }
            else
            {   ratiox     = (wide == 2) ?  8 : 4; // 4:3 (narrow) or  8:3 (wide)
                ratioy     = 3;
            }
            if (tempwidth > tempheight * ratiox / ratioy)
            {   winheight = winwidth;
                if (showleds)
                {   switch (machine)
                    {
                    case  PIPBUG: winheight += PIPBUG_LEDHEIGHT;
                    acase BINBUG: winheight += BINBUG_LEDHEIGHT;
                    acase PHUNSY: winheight += PHUNSY_LEDHEIGHT;
                    acase SELBST: winheight += SELBST_LEDHEIGHT;
                }   }
                winheight *= ratioy;
                winheight /= ratiox;
            } elif (tempheight > tempwidth * ratioy / ratiox)
            {   winwidth  = winheight * ratiox / ratioy;
        }   }
        clientwidth  = destwidth  = winwidth;
        clientheight = destheight = winheight;

        xoffset   =  0;

        winwidth  += (leftwidth    * 2);
        winheight += (bottomheight * 2);
        if (showtitlebars[wsm])
        {   winheight += titleheight;
        }
        if (showsidebars[wsm])
        {   winwidth += sidebarwidth;
        }
        if (showmenubars[wsm])
        {   winheight += menuheight;
        }
        if (showtoolbars[wsm])
        {   winheight += toolbarheight;
        }
        if (showdebugger[wsm])
        {   winheight += debuggerheight;
        }
        if (showstatusbars[wsm])
        {   winheight += statusbarheight;
        }

        realwide = wide;
        realsize = size;
    }

    debuggerwidth = clientwidth - 4;
    if (showtoolbars[wsm])
    {   yoffset = toolbarheight;
    } else
    {   yoffset = 0;
    }
    sidebartopy = yoffset;
    debuggertop = yoffset + clientheight;
    if (fullscreen && (stretching == STRETCHING_NONE || filter == FILTER_3D))
    {   xoffset += leftwidth;
        yoffset += (clientheight - rastheight) / 2;
    }

    if (winleftx == -1)
    {   winleftx = (DisplayWidth  / 2) - (winwidth  / 2);
    }
    if (wintopy == -1)
    {   wintopy  = (DisplayHeight / 2) - (winheight / 2);
    }

    if (winleftx < 0 || winleftx >= DisplayWidth)
    {   winleftx = 0;
    }
    if (wintopy < 0 || wintopy >= DisplayHeight)
    {   wintopy = 0;
    }

    if (winleftx + winwidth >= DisplayWidth)
    {   excess   =  winleftx + winwidth - DisplayWidth;
        winleftx -= excess;
        if (winleftx < 0)
        {   winleftx = 0;
    }   }
    if (wintopy + winheight >= DisplayHeight)
    {   excess   =  wintopy + winheight - DisplayHeight;
        wintopy  -= excess;
        if (wintopy < 0)
        {   wintopy = 0;
    }   }

    minwinwidth = winwidth - sidebarwidth + SIDEBARWIDTH;

    if   (realsize == 1 && realwide == 1) { fastsize =  0;                                         nextrow2 = machines[machine].width     ; ptrinc =  machines[machine].width           ; } //  1*1
    elif (realsize == 1 && realwide == 2) { fastsize =  1;                                         nextrow2 = machines[machine].width *  2; ptrinc = (machines[machine].width *  2) -  1; } //  2*1
    elif (realsize == 2 && realwide == 1) { fastsize =  2; nextrow = machines[machine].width *  2; nextrow2 = machines[machine].width *  4; ptrinc = (machines[machine].width *  2) -  1; } //  2*2
    elif (realsize == 2 && realwide == 2) { fastsize =  3; nextrow = machines[machine].width *  4; nextrow2 = machines[machine].width *  8; ptrinc = (machines[machine].width *  4) -  3; } //  4*2
    elif (realsize == 3 && realwide == 1) { fastsize =  4; nextrow = machines[machine].width *  3; nextrow2 = machines[machine].width *  9; ptrinc = (machines[machine].width *  3) -  2; } //  3*3
    elif (realsize == 3 && realwide == 2) { fastsize =  5; nextrow = machines[machine].width *  6; nextrow2 = machines[machine].width * 18; ptrinc = (machines[machine].width *  6) -  5; } //  6*3
    elif (realsize == 4 && realwide == 1) { fastsize =  6; nextrow = machines[machine].width *  4; nextrow2 = machines[machine].width * 16; ptrinc = (machines[machine].width *  4) -  3; } //  4*4
    elif (realsize == 4 && realwide == 2) { fastsize =  7; nextrow = machines[machine].width *  8; nextrow2 = machines[machine].width * 32; ptrinc = (machines[machine].width *  8) -  7; } //  8*4
    elif (realsize == 5 && realwide == 1) { fastsize =  8; nextrow = machines[machine].width *  5; nextrow2 = machines[machine].width * 25; ptrinc = (machines[machine].width *  5) -  4; } //  5*5
    elif (realsize == 5 && realwide == 2) { fastsize =  9; nextrow = machines[machine].width * 10; nextrow2 = machines[machine].width * 50; ptrinc = (machines[machine].width * 10) -  9; } // 10*5
    elif (realsize == 6 && realwide == 1) { fastsize = 10; nextrow = machines[machine].width *  6; nextrow2 = machines[machine].width * 36; ptrinc = (machines[machine].width *  6) -  5; } //  6*6
    elif (realsize == 6 && realwide == 2) { fastsize = 11; nextrow = machines[machine].width * 12; nextrow2 = machines[machine].width * 72; ptrinc = (machines[machine].width * 12) - 11; } // 12*6
    // else assert(0);
    fastwide = realsize * realwide;

    sidebarheight = clientheight;
    if (showdebugger[wsm])
    {   sidebarheight += debuggerheight;
    }
    if (fullscreen)
    {   if (!showtitlebars[wsm])
        {   clientwidth += 2;
    }   }

    if (fullscreen && stretching != STRETCHING_NONE && filter != FILTER_3D)
    {   if (stretching == STRETCHING_43)
        {   if (machines[machine].coinop && !memmapinfo[memmap].rotate || rotating)
            {   destwidth = clientheight * 3 / 4; // 3:4
            } else
            {   destwidth = clientheight * 4 / 3; // 4:3
        }   }
        else
        {   // assert(stretching == STRETCHING_TOFIT);
            destwidth = clientwidth;
        }
        destheight = clientheight;
    }

    hostwidth  = destwidth;
    hostheight = destheight;

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Window position is %d,%d and its size is %d,%d.\n", winleftx, wintopy, winwidth, winheight);
#endif
}

EXPORT void make_toolbar(void)
{   TBBUTTON tbb[BIGGADGETS + 11 + 7];
    FLAG     widegadgets;
    int      next,
             sparewidth;

    // assert(MainWindowPtr);

    if (hToolbar)
    {   DestroyWindow(hToolbar);
        hToolbar = NULL;
    }

    if (!showtoolbars[wsm])
    {   return;
    }

    ZeroMemory(tbb, sizeof(tbb));

#define BUTTONWIDTH    25
#define SEPARATORWIDTH  8

/* Left window border and margin is 2.
   Each bland button is 25x24.
   Gap (separator) is 8.
   Each colourful button is 25x24.
   Any spare room is here.
   Right window border and margin is 10. */

    sparewidth = winwidth - 20; // 20 is for margins/gap

    if (sparewidth >= (10 * BUTTONWIDTH) + SEPARATORWIDTH)
    {   sparewidth  -= (10 * BUTTONWIDTH) + SEPARATORWIDTH;
        widegadgets = TRUE;
    } else
    {   widegadgets = FALSE;
    }

    if (sparewidth >= (8 * BUTTONWIDTH) + SEPARATORWIDTH)
    {   tbb[0].iBitmap   = 0;
        tbb[0].fsState   = 0;
        tbb[0].fsStyle   = TBSTYLE_BUTTON;
        tbb[0].idCommand = ID_FILE_RESET;
        tbb[1].iBitmap   = 1;
        tbb[1].fsState   = TBSTATE_ENABLED;
        tbb[1].fsStyle   = TBSTYLE_BUTTON;
        tbb[1].idCommand = ID_FILE_OPEN;
        tbb[2].iBitmap   = 2;
        tbb[2].fsState   = 0;
        tbb[2].fsStyle   = TBSTYLE_BUTTON;
        tbb[2].idCommand = ID_FILE_SAVESNP;
        tbb[3].iBitmap   = 3;
        tbb[3].fsState   = TBSTATE_ENABLED;
        tbb[3].fsStyle   = TBSTYLE_BUTTON;
        tbb[3].idCommand = ID_FILE_SAVEBMP;
        tbb[4].iBitmap   = 4;
        tbb[4].fsState   = TBSTATE_ENABLED;
        tbb[4].fsStyle   = TBSTYLE_BUTTON;
        tbb[4].idCommand = ID_EDIT_COPY;
        tbb[5].iBitmap   = 5;
        tbb[5].fsState   = TBSTATE_ENABLED;
        tbb[5].fsStyle   = TBSTYLE_BUTTON;
        tbb[5].idCommand = ID_MACRO_RUNREXX;
        tbb[6].iBitmap   = 6;
        tbb[6].fsState   = TBSTATE_ENABLED;
        tbb[6].fsStyle   = TBSTYLE_BUTTON;
        tbb[6].idCommand = ID_FILE_QUICKLOAD;
        tbb[7].iBitmap   = 7;
        tbb[7].fsState   = TBSTATE_ENABLED;
        tbb[7].fsStyle   = TBSTYLE_BUTTON;
        tbb[7].idCommand = ID_FILE_QUICKSAVE;

        tbb[8].iBitmap   = 0;
        tbb[8].fsState   = 0;
        tbb[8].fsStyle   = TBSTYLE_SEP;
        tbb[8].idCommand = 0;

        next             = 9;
        sparewidth       -= (8 * BUTTONWIDTH) + SEPARATORWIDTH;
    } elif (sparewidth >= (3 * BUTTONWIDTH) + SEPARATORWIDTH)
    {   tbb[0].iBitmap   = 0;
        tbb[0].fsState   = 0;
        tbb[0].fsStyle   = TBSTYLE_BUTTON;
        tbb[0].idCommand = ID_FILE_RESET;
        tbb[1].iBitmap   = 1;
        tbb[1].fsState   = TBSTATE_ENABLED;
        tbb[1].fsStyle   = TBSTYLE_BUTTON;
        tbb[1].idCommand = ID_FILE_OPEN;
        tbb[2].iBitmap   = 2;
        tbb[2].fsState   = 0;
        tbb[2].fsStyle   = TBSTYLE_BUTTON;
        tbb[2].idCommand = ID_FILE_SAVESNP;

        tbb[3].iBitmap   = 0;
        tbb[3].fsState   = 0;
        tbb[3].fsStyle   = TBSTYLE_SEP;
        tbb[3].idCommand = 0;

        next             = 4;
        sparewidth       -= (3 * BUTTONWIDTH) + SEPARATORWIDTH;
    } else
    {   next             = 0;
    }

    if (widegadgets)
    {   tbb[next     ].iBitmap   = gadgetimage[GADPOS_AUTOFIRE1][0];
        tbb[next     ].fsState   = TBSTATE_ENABLED;
        tbb[next     ].fsStyle   = TBSTYLE_CHECK;
        tbb[next     ].idCommand = ID_LEFT_AUTOFIRE;
        tbb[next +  1].iBitmap   = gadgetimage[GADPOS_AUTOFIRE2][0];
        tbb[next +  1].fsState   = TBSTATE_ENABLED;
        tbb[next +  1].fsStyle   = TBSTYLE_CHECK;
        tbb[next +  1].idCommand = ID_RIGHT_AUTOFIRE;
        tbb[next +  2].iBitmap   = gadgetimage[GADPOS_SWAPPED][0];
        tbb[next +  2].fsState   = TBSTATE_ENABLED;
        tbb[next +  2].fsStyle   = TBSTYLE_CHECK;
        tbb[next +  2].idCommand = ID_INPUT_SWAPPED;

        tbb[next +  3].iBitmap   = gadgetimage[GADPOS_SOUND][0];
        tbb[next +  3].fsState   = TBSTATE_ENABLED;
        tbb[next +  3].fsStyle   = TBSTYLE_CHECK;
        tbb[next +  3].idCommand = ID_SOUND_ENABLED;
        tbb[next +  4].iBitmap   = gadgetimage[GADPOS_PAUSED][0];
        tbb[next +  4].fsState   = TBSTATE_ENABLED;
        tbb[next +  4].fsStyle   = TBSTYLE_CHECK;
        tbb[next +  4].idCommand = ID_SPEED_PAUSED;
        tbb[next +  5].iBitmap   = gadgetimage[GADPOS_TURBO][0];
        tbb[next +  5].fsState   = TBSTATE_ENABLED;
        tbb[next +  5].fsStyle   = TBSTYLE_CHECK;
        tbb[next +  5].idCommand = ID_SPEED_TURBO;
        tbb[next +  6].iBitmap   = gadgetimage[GADPOS_COLLISIONS][0];
        tbb[next +  6].fsState   = TBSTATE_ENABLED;
        tbb[next +  6].fsStyle   = TBSTYLE_CHECK;
        tbb[next +  6].idCommand = ID_SPRITES_COLLISIONS;

        tbb[next +  7].iBitmap   = 0;
        tbb[next +  7].fsState   = 0;
        tbb[next +  7].fsStyle   = TBSTYLE_SEP;
        tbb[next +  7].idCommand = 0;

        tbb[next +  8].iBitmap   = gadgetimage[GADPOS_RECORD][0];
        tbb[next +  8].fsState   = TBSTATE_ENABLED;
        tbb[next +  8].fsStyle   = TBSTYLE_CHECK;
        tbb[next +  8].idCommand = ID_MACRO_STARTRECORDING;
        tbb[next +  9].iBitmap   = gadgetimage[GADPOS_PLAY][0];
        tbb[next +  9].fsState   = TBSTATE_ENABLED;
        tbb[next +  9].fsStyle   = TBSTYLE_CHECK;
        tbb[next +  9].idCommand = ID_MACRO_RESTARTPLAYBACK;
        tbb[next + 10].iBitmap   = gadgetimage[GADPOS_STOP][0];
        tbb[next + 10].fsState   = TBSTATE_ENABLED;
        tbb[next + 10].fsStyle   = TBSTYLE_CHECK;
        tbb[next + 10].idCommand = ID_MACRO_STOP;

        next += 11;
    }

    if (sparewidth >= (4 * BUTTONWIDTH) + SEPARATORWIDTH)
    {   sparewidth -= (4 * BUTTONWIDTH) + SEPARATORWIDTH;

        tbb[next    ].iBitmap   = 0;
        tbb[next    ].fsState   = 0;
        tbb[next    ].fsStyle   = TBSTYLE_SEP;
        tbb[next    ].idCommand = 0;

        tbb[next + 1].iBitmap   = machines[machine].consolekeyimage[0];
        tbb[next + 1].fsState   = TBSTATE_ENABLED;
        tbb[next + 1].fsStyle   = TBSTYLE_BUTTON;
        tbb[next + 1].idCommand = ID_CONSOLE_START;

        tbb[next + 2].iBitmap   = machines[machine].consolekeyimage[1];
        tbb[next + 2].fsState   = TBSTATE_ENABLED;
        tbb[next + 2].fsStyle   = TBSTYLE_BUTTON;
        tbb[next + 2].idCommand = ID_CONSOLE_A;

        tbb[next + 3].iBitmap   = machines[machine].consolekeyimage[2];
        tbb[next + 3].fsState   = TBSTATE_ENABLED;
        tbb[next + 3].fsStyle   = TBSTYLE_BUTTON;
        tbb[next + 3].idCommand = ID_CONSOLE_B;

        tbb[next + 4].iBitmap   = machines[machine].consolekeyimage[3];
        tbb[next + 4].fsState   = TBSTATE_ENABLED;
        tbb[next + 4].fsStyle   = TBSTYLE_BUTTON;
        tbb[next + 4].idCommand = ID_CONSOLE_RESET;

        next += 5;
    }

    hToolbar = CreateToolbarEx
    (   MainWindowPtr,
        WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS,
        IDC_TOOLBAR,
        43, // number of images in IDB_TOOLBAR
        InstancePtr,
        (UINT) MAKEINTRESOURCE(IDB_TOOLBAR),
        tbb,
        (int) next,
        18,
        18,
        18,
        18,
        sizeof(TBBUTTON)
    );
    if (hToolbar == NULL)
    {   rq("Can't create toolbar!");
    }

    // Send the TB_BUTTONSTRUCTSIZE message, which is required
    DISCARD SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);
    DISCARD SendMessage(hToolbar, TB_AUTOSIZE, 0, 0);
    updatebiggads();
    updatesmlgads();

    DISCARD GetWindowRect(hToolbar, &therect);
    toolbarheight = therect.bottom - therect.top + 1;
}

EXPORT void settitle(void)
{   NOTIFYICONDATA tnid;

    if (cheevos)
    {   if (file_game != NULL)
        {
#ifdef LOGCHEEVOS
            zprintf(TEXTPEN_VERBOSE, "RA_UpdateAppTitle(\"%s\")\n", file_game);
#endif
            RA_UpdateAppTitle(file_game);
        } else
        {
#ifdef LOGCHEEVOS
            zprintf(TEXTPEN_VERBOSE, "RA_UpdateAppTitle(\"\")\n");
#endif
            RA_UpdateAppTitle("");
    }   }
    else
    {   strcpy(titlebartext, hostmachines[machine].titlebartext);
        if (game && file_game[0])
        {   strcat(titlebartext, ": ");
            strcat(titlebartext, file_game);
        }
        settitle_engine();

#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Setting title to %s.\n", titlebartext);
#endif

        if (iconified)
        {   tnid.cbSize           = sizeof(NOTIFYICONDATA);
            tnid.hWnd             = MainWindowPtr;
            tnid.uID              = 1;
            tnid.uFlags           = NIF_MESSAGE | NIF_ICON | NIF_TIP;
            tnid.uCallbackMessage = USR_NOTIFYICON;
            tnid.hIcon            = smlicon;
            strcpy(tnid.szTip, titlebartext);
            DISCARD Shell_NotifyIcon(NIM_MODIFY, &tnid);
        }

        if (MainWindowPtr)
        {   DISCARD SetWindowText(MainWindowPtr, titlebartext);
    }   }

    updatepointer(FALSE, TRUE); // for pointer confinement/deconfinement
}

EXPORT FLAG ctrl(void)
{   if
    (   (KeyMatrix[SCAN_LCTRL  / 8] & (1 << (SCAN_LCTRL  % 8)))
     || (KeyMatrix[SCAN_RCTRL  / 8] & (1 << (SCAN_RCTRL  % 8)))
     || softctrl
    )
    {   return TRUE;
    } else
    {   return FALSE;
}   }
EXPORT FLAG shift(void)
{   if
    (   (KeyMatrix[SCAN_LSHIFT / 8] & (1 << (SCAN_LSHIFT % 8)))
     || (KeyMatrix[SCAN_RSHIFT / 8] & (1 << (SCAN_RSHIFT % 8)))
     || (machine == TYPERIGHT && (oldkeys[49] || oldkeys[50]))
     || softlshift
     || softrshift
    )
    {   return TRUE;
    } else
    {   return FALSE;
}   }
EXPORT FLAG lshift(void)
{   if
    (   (KeyMatrix[SCAN_LSHIFT / 8] & (1 << (SCAN_LSHIFT % 8)))
     || (machine == TYPERIGHT && oldkeys[49])
     || softlshift
    )
    {   return TRUE;
    } else
    {   return FALSE;
}   }
EXPORT FLAG rshift(void)
{   if
    (   (KeyMatrix[SCAN_RSHIFT / 8] & (1 << (SCAN_RSHIFT % 8)))
     || (machine == TYPERIGHT && oldkeys[50])
     || softrshift
    )
    {   return TRUE;
    } else
    {   return FALSE;
}   }

EXPORT void openwindow(FLAG reopen)
{   int        i;
    HDC        OnScreenRastPort;
    HFONT      OldFontPtr;
    HMENU      MenuColumnPtr;
    TEXTMETRIC tm;
PERSIST const int glyph_to_idi[ARCADIAGLYPHS] = {
IDI_3DATTACK,
IDI_3DSOCCER,
IDI_HOMERUN,
IDI_ALIENINVADERS,
IDI_ASTROINVADER,
IDI_AUTORACE,
IDI_BASEBALL,      //  5
IDI_BASKETBALL,
IDI_BATTLE,
IDI_BLACKJACK,
IDI_BOWLING,
IDI_BOXING,
IDI_BRAINQUIZ,
IDI_BREAKAWAY,
IDI_CAPTURE,
IDI_CATTRAX,
IDI_CIRCUS,
IDI_COMBAT,
IDI_CRAZYCLIMBER,
IDI_CRAZYGOBBLER,
IDI_DICTIONARY,
IDI_DORAEMON,
IDI_DRSLUMP,
IDI_ESCAPE,
IDI_GRIDIRON,
IDI_FROGGER,
IDI_FUNKYFISH,
IDI_GOLF,
IDI_GUNDAM,
IDI_HOBO,
IDI_HORSERACING,
IDI_JOURNEY,
IDI_JUMPBUG,
IDI_JUNGLER,
IDI_MACROSS,
IDI_MISSILEWAR,
IDI_MONACO,
IDI_NIBBLEMEN,
IDI_OCEANBATTLE,
IDI_PARASHOOTER,
IDI_PLEIADES,
IDI_R2DTANK,
IDI_REDCLASH,
IDI_ROBOTKILLER,
IDI_ROUTE16,
IDI_2DSOCCER,
IDI_SPACEATTACK,
IDI_SPACEBUSTER,
IDI_SPACEMISSION,
IDI_SPACERAIDERS,
IDI_SPACESQUADRON,
IDI_SPACEVULTURES,
IDI_SPIDERS,
IDI_STARCHESS,
IDI_SUPERBUG,
IDI_TANKSALOT,
IDI_TENNIS,
IDI_THEEND,
IDI_TURTLES,
};

    ready = FALSE;

    calc_size();

    winstyle = WS_POPUP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    if (!fullscreen)
    {   winstyle |= WS_BORDER | WS_THICKFRAME; // it still seems to have a border even in full-screen mode for some reason :-(
        // Ideally, we would lock the vertical axis of the window.
        // However, Windows seems too crap to allow this; you can only have all borders draggable or none.
    }
    if (showtitlebars[wsm])
    {   winstyle |= WS_CAPTION | WS_SYSMENU;
    }

    update_menuheight();

    if (showmenubars[wsm])
    {   if (!(MenuPtr = LoadMenu(InstancePtr, MAKEINTRESOURCE(langs[language].mainmenu))))
        {   rq("LoadMenu() failed for main menu!");
    }   }
    else
    {   MenuPtr = NULL;
    }
    MainWindowPtr = CreateWindowEx
    (   0,
        g_szClassName,
        showtitlebars[wsm] ? hostmachines[machine].titlebartext : NULL,
        winstyle,
        winleftx,
        wintopy,
        winwidth,
        winheight,
        NULL,
        MenuPtr,
        InstancePtr,
        NULL
    );
    if (!MainWindowPtr)
    {   rq("Main window creation failed!");
    }

    SendMessage(MainWindowPtr, WM_SETICON, ICON_BIG,   (LPARAM) bigicon);
    SendMessage(MainWindowPtr, WM_SETICON, ICON_SMALL, (LPARAM) smlicon);

    make_toolbar();

    if (showdebugger[wsm])
    {   hEditDS = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE, MAX_PATH + 80 + 1);
        if (hEditDS == NULL)
        {   hEditDS = InstancePtr;
        }

        OnScreenRastPort = GetDC(MainWindowPtr);
        OldFontPtr = SelectObject(OnScreenRastPort, hFont);
        DISCARD GetTextMetrics(OnScreenRastPort, &tm);
        promptwidth  = (int) tm.tmAveCharWidth;
     // promptheight = (int) tm.tmAscent;
        SelectObject(OnScreenRastPort, OldFontPtr);
        DISCARD ReleaseDC(MainWindowPtr, OnScreenRastPort);

        hDebugger = CreateWindow
        (   "edit",
            "",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            promptwidth,
            debuggertop,
            debuggerwidth,
            debuggerheight,
            MainWindowPtr,
            (HMENU) IDC_DEBUGGER,
            hEditDS,
            0
        );

        wpOldEditProc = (WNDPROC) SetWindowLong
        (   hDebugger,
            GWL_WNDPROC,
            (LONG) CustomEditProc
        );

        switch (language)
        {
        case  LANGUAGE_GRE: DISCARD SendMessage(hDebugger, WM_SETFONT, (WPARAM) hGreekFont  , MAKELPARAM(TRUE, 0));
        acase LANGUAGE_POL: DISCARD SendMessage(hDebugger, WM_SETFONT, (WPARAM) hPolishFont , MAKELPARAM(TRUE, 0));
        acase LANGUAGE_RUS: DISCARD SendMessage(hDebugger, WM_SETFONT, (WPARAM) hRussianFont, MAKELPARAM(TRUE, 0));
        adefault:           DISCARD SendMessage(hDebugger, WM_SETFONT, (WPARAM) hFont,        MAKELPARAM(TRUE, 0));
        }
        DISCARD SendMessage(hDebugger, EM_LIMITTEXT,   MAX_PATH + 80, 0);
        DISCARD SendMessage(hDebugger, EM_SETREADONLY, FALSE        , 0);
        DISCARD SendMessage(hDebugger, WM_SETTEXT,     0            , (LPARAM) "");

        if (incli)
        {   debugger_enter();
    }   }
    else
    {   hDebugger = NULL;
    }

    if (showsidebars[wsm])
    {   if (needaudit)
        {   audit(FALSE);
        }

        himl = ImageList_Create(16, 16, ILC_COLOR8, ICONS, 0);
        // sidebar listview control automatically destroys this image list for us when it is itself destroyed

        // This has to be done in machine order.
        for (i = 0; i < MEMMAPS; i++)
        {   hicon = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[i].icon), IMAGE_ICON, 16, 16, 0);
            DISCARD ImageList_AddIcon(himl, hicon);
            DeleteObject(hicon);
        }
        hicon = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_FOLDER), IMAGE_ICON, 16, 16, 0);
        DISCARD ImageList_AddIcon(himl, hicon);
        DeleteObject(hicon);
        hicon = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_ROOT  ), IMAGE_ICON, 16, 16, 0);
        DISCARD ImageList_AddIcon(himl, hicon);
        DeleteObject(hicon);
        hicon = LoadImage(InstancePtr, MAKEINTRESOURCE(IDI_PARENT), IMAGE_ICON, 16, 16, 0);
        DISCARD ImageList_AddIcon(himl, hicon);
        DeleteObject(hicon);

        for (i = 0; i < ARCADIAGLYPHS; i++)
        {   hicon = LoadImage(InstancePtr, MAKEINTRESOURCE(glyph_to_idi[i]), IMAGE_ICON, 16, 16, 0);
            DISCARD ImageList_AddIcon(himl, hicon);
            DeleteObject(hicon);
        }

        if (fullscreen)
        {   hSideBar = CreateWindowEx
            (   0,
                WC_LISTVIEW,
                "",
                WS_CHILD
              | WS_BORDER
              | WS_HSCROLL
              | WS_TABSTOP
              | WS_VSCROLL
              | WS_VISIBLE
              | LVS_LIST
              | LVS_SINGLESEL
              | LVS_SHOWSELALWAYS
              | WS_EX_CLIENTEDGE,
                showtitlebars[wsm] ? (winwidth - sidebarwidth - leftwidth) : (winwidth - sidebarwidth + 2),
                sidebartopy,
                sidebarwidth,
                sidebarheight,
                MainWindowPtr,
                (HMENU) IDC_SIDEBAR,
                InstancePtr,
                NULL
            );
        } else
        {   hSideBar = CreateWindowEx
            (   0,
                WC_LISTVIEW,
                "",
                WS_CHILD
              | WS_BORDER
              | WS_HSCROLL
              | WS_TABSTOP
              | WS_VSCROLL
              | WS_VISIBLE
              | LVS_LIST
              | LVS_SINGLESEL
              | LVS_SHOWSELALWAYS
              | WS_EX_CLIENTEDGE,
                winwidth - sidebarwidth - (leftwidth * 2) + 2,
                sidebartopy,
                sidebarwidth,
                sidebarheight,
                MainWindowPtr,
                (HMENU) IDC_SIDEBAR,
                InstancePtr,
                NULL
            );
        }

        if (hSideBar == NULL)
        {   rq("Can't create sidebar!");
        }

        ListView_SetBkColor(hSideBar, EMURGBPEN_WHITE);
        ListView_SetTextColor(hSideBar, EMURGBPEN_BLACK);
        ListView_SetTextBkColor(hSideBar, EMURGBPEN_WHITE);

        if (language == LANGUAGE_GRE)
        {   DISCARD SendMessage(hSideBar, WM_SETFONT, (WPARAM) hGreekFont,   MAKELPARAM(TRUE, 0));
        } elif (language == LANGUAGE_RUS)
        {   DISCARD SendMessage(hSideBar, WM_SETFONT, (WPARAM) hRussianFont, MAKELPARAM(TRUE, 0));
        } else
        {   DISCARD SendMessage(hSideBar, WM_SETFONT, (WPARAM) hFont,        MAKELPARAM(TRUE, 0));
        }
        ListView_SetImageList(hSideBar, himl, LVSIL_SMALL);
    }

    if (!(PopUpMenuPtr = LoadMenu(InstancePtr, MAKEINTRESOURCE(langs[language].popupmenu))))
    {   rq("LoadMenu() failed for pop-up menu!");
    }
    PopUpMenuPtr = GetSubMenu(PopUpMenuPtr, 0);

    update_joynames();
    make_statusbar();

    if (MenuPtr)
    {   // assert(showmenubars[wsm]);
        for (i = 0; i < MIBS; i++)
        {   if (mib[i].column == -1)
            {   SetMenuItemBitmaps
                (   MenuPtr,                          // handle of menu
                    mib[i].menuitem,                  // menu item to receive new bitmaps
                    MF_BYCOMMAND,                     // menu item flags
                    mib[i].mx ? NULL : mib[i].bitmap, // handle of unchecked bitmap
                    mib[i].mx ? mib[i].bitmap : NULL  // handle of checked bitmap
                );
            } else
            {   MenuColumnPtr = GetSubMenu(MenuPtr, mib[i].column);
                SetMenuItemBitmaps
                (   MenuColumnPtr,                    // handle of menu
                    mib[i].menuitem,                  // menu item to receive new bitmaps
                    MF_BYPOSITION,                    // menu item flags
                    mib[i].bitmap,                    // handle of unchecked bitmap
                    NULL                              // handle of checked bitmap
                );
        }   }
        do_recents(MenuPtr);
    }
    if (PopUpMenuPtr)
    {   for (i = 0; i < MIBS; i++)
        {   DISCARD SetMenuItemBitmaps
            (   PopUpMenuPtr,     // handle of menu
                mib[i].menuitem,  // menu item to receive new bitmaps
                MF_BYCOMMAND,     // menu item flags
                mib[i].mx ? NULL : mib[i].bitmap, // handle of unchecked bitmap
                mib[i].mx ? mib[i].bitmap : NULL  // handle of checked bitmap
            );
        }
        do_recents(PopUpMenuPtr);
    }

    SystemMenuPtr = GetSystemMenu(MainWindowPtr, FALSE);
    if (SystemMenuPtr)
    {   DISCARD InsertMenu(SystemMenuPtr, 6, MF_BYPOSITION | MF_STRING, ID_FILE_ICONIFY, "Iconify\tCtrl+F12");
        DISCARD SetMenuItemBitmaps
        (   SystemMenuPtr,             // handle of menu
            mib[MIB_ICONIFY].menuitem, // menu item to receive new bitmaps
            MF_BYCOMMAND,              // menu item flags
            mib[MIB_ICONIFY].bitmap,   // handle of unchecked bitmap
            NULL                       // handle of checked bitmap
        );
        /* DISCARD SetMenuItemBitmaps
        (   SystemMenuPtr,             // handle of menu
            mib[MIB_EXIT].menuitem,    // menu item to receive new bitmaps
            MF_BYCOMMAND,              // menu item flags
            mib[MIB_EXIT].bitmap,      // handle of unchecked bitmap
            NULL                       // handle of checked bitmap
        ); */
    }

    openwindow_3d();
    if (reopen)
    {   reopen_subwindows();
    }
    make_display();

    updatebiggads();
    updatesmlgads();
    updatemenus();
    redrawscreen();
    settitle();
    update_speed();
    buildlistbrowser();

    DragAcceptFiles(MainWindowPtr, TRUE);
    ShowWindow(MainWindowPtr, SW_SHOW);
    DISCARD UpdateWindow(MainWindowPtr);

    SendMessage(MainWindowPtr, WM_CHANGEUISTATE, UISF_HIDEACCEL << 16 | UIS_CLEAR, 0); // this shows the keyboard shortcuts for the menu

    if (cheevos)
    {   if (cheevos_reopen)
        {   cheevos_reopen = FALSE;
#ifdef LOGCHEEVOS
            zprintf(TEXTPEN_VERBOSE, "RA_UpdateHWnd()\n");
#endif
            RA_UpdateHWnd(MainWindowPtr);
            RebuildMenu();
        } else
        {   init_cheevos();
    }   }

    blanker_off();
    clearkybd();
    ready = TRUE;

/*  GetWindowRect(MainWindowPtr, &therect);
    zprintf
    (   TEXTPEN_VERBOSE,
        "Frame:  %d,%d-%d,%d! (%d*%d).\n",
        therect.left,
        therect.top,
        therect.right,
        therect.bottom,
        therect.right - therect.left + 1,
        therect.bottom - therect.top + 1
    );

    GetClientRect(MainWindowPtr, &therect);
    zprintf
    (   TEXTPEN_VERBOSE,
        "Client: %d,%d-%d,%d! (%d*%d).\n",
        therect.left,
        therect.top,
        therect.right,
        therect.bottom,
        therect.right - therect.left + 1,
        therect.bottom - therect.top + 1
    );

    Frame is  8 pixels wider than client.
    Frame is 46 pixels wider than client.
    The toolbar IS part of the client area, it is normally 30 pixels tall. */
}

EXPORT void closewindow(void)
{   close_subwindows(TRUE);
    if (MenuPtr)
    {   SetMenu(MainWindowPtr, MenuPtr); // so it is (supposedly) automatically freed for us
    }
    if (PopUpMenuPtr)
    {   DISCARD DestroyMenu(PopUpMenuPtr);
        PopUpMenuPtr = NULL;
    }
    if (hSideBar)
    {   ListView_DeleteAllItems(hSideBar);
        hSideBar = NULL;
    }
    if (cheevos)
    {   cheevos_reopen = TRUE;
    }
    // PostMessage(MainWindowPtr, WM_CLOSE, 0, 0);
    DestroyWindow(MainWindowPtr);
    MainWindowPtr = NULL;
    if (hEditDS != NULL && hEditDS != InstancePtr)
    {   GlobalFree(hEditDS);
        hEditDS = NULL;
    }
    closewindow_3d();
    free_display();
}

MODULE void printusage(STRPTR progname)
{   TEXT tempstring[256 + 1];

    sprintf
    (   tempstring,
        "%s: %s [[FILE]=<file>|ASM=<file>|DISGAME=<file>] [FULLSCREEN=ON|OFF] [LOCKCONFIG|UPGRADE] [PORT=<port>] [SETTINGS=<file>] [STARTUP=<file>]\n",
        LLL(MSG_USAGE, "Usage"),
        progname
    );
    saymessage(tempstring);
    cleanexit(EXIT_FAILURE);
}

EXPORT void setcodepage(void)
{   UINT cp;
    TEXT tempstring[80 + 1];

    cp = GetACP();
    if (cp != (UINT) langs[language].codepage)
    {   /* None of these work here (on Win8.1) :-( but they do work when typed at the command line.
        WinExec("%systemroot%\\system32\\control.exe /name Microsoft.RegionalAndLanguageOptions /page /p:\"administrative\"", SW_NORMAL);
        WinExec("%systemroot%\\system32\\control.exe international", SW_NORMAL);
        openurl("control.exe international");

        Better to leave this string in English because if the code page is wrong
        the user will be unable to read text in their preferred language. */
        sprintf(tempstring, "Incorrect code page (%d expected, %d found)!\n", langs[language].codepage, cp);
        say(tempstring);
    }

    allglyphs = (langs[language].codepage == CODEPAGE_ENG) ? TRUE : FALSE;
}

EXPORT void break_pathname(STRPTR pathname, STRPTR path, STRPTR filename)
{   int i,
        length;

    // Splits a complete pathname into path and filename components.

    length = strlen(pathname);
    if (length >= 1)
    {   for (i = length - 1; i >= 0; i--)
        {   if (pathname[i] == '\\')
            {   if (filename)
                {   strcpy(filename, &pathname[i + 1]);
                }
                zstrncpy(path, pathname, i + 1);
                fix_paths();
                return;
    }   }   }

    if (filename)
    {   filename[0] = EOS;
    }
    strcpy(path, pathname);
    fix_paths();
}

EXPORT void do_autopause(WPARAM wParam, LPARAM lParam)
{   // For WA_INACTIVE: "(HWND) lParam is the handle of the window being activated."

    int found,
        i;

    found = FALSE;
    for (i = 0; i < SUBWINDOWS; i++)
    {   if ((HWND) lParam == SubWindowPtr[i])
        {   found = TRUE;
            break; // for speed
    }   }

    if
    (   LOWORD(wParam) == WA_INACTIVE
     && autopause
     && !crippled
     && !paused
     && !opening
     && (       (HWND) lParam == NULL
         || (   (HWND) lParam !=      MainWindowPtr
             && (HWND) lParam != ListeningWindowPtr
             && !found
    )   )   )
    {   emu_pause();
}   }

EXPORT void datestamp2(void)
{   SYSTEMTIME TheTime;

    GetLocalTime(&TheTime);
    DISCARD sprintf
    (   datetimestring,
        "%d%c%02d%c%02d %s %d%c%d%c%d",
        (int) TheTime.wHour,      //   2 chars
        langs[language].timesep,  //   1
        (int) TheTime.wMinute,    //   2
        langs[language].timesep,  //   1
        (int) TheTime.wSecond,    //   2
        LLL(MSG_ENGINE_ON, "on"), // 1+2+1+
        (int) (yank ? TheTime.wMonth : TheTime.wDay  ), // 2
        langs[language].datesep,  //   1
        (int) (yank ? TheTime.wDay   : TheTime.wMonth), // 2
        langs[language].datesep,  //   1
        (int) TheTime.wYear       //   4
    );                            // = 22+
}

/* Some relevant locale IDs are:
   $4 07 = German
   $4 09 = American
   $4 0C = French
   $4 10 = Italian
   $8 09 = English
   $C 09 = Australian
   $? 13 = Dutch
   $? 19 = Russian
   $? 08 = Greek
   $? 15 = Polish */

EXPORT void freelanguage(void)
{   if (langs[language].winbuffer)
    {   free(langs[language].winbuffer);
        langs[language].winbuffer = NULL;
    }
    if (langs[language].offset)
    {   free(langs[language].offset);
        langs[language].offset = NULL;
}   }

EXPORT void changelanguage(void)
{   setcodepage();
    closewindow();
    load_catalog();
    translate();
    needaudit = TRUE;
    openwindow(TRUE);
}

EXPORT void load_catalog(void)
{   UBYTE        *p8data,
                 *fbase;
    UBYTE         LocalIOBuffer[8];
    UBYTE*        dataptr = NULL;
    int           code;
    struct Chunk* p8Chunk;
    ULONG         fsize,
                  thesize;
    FILE*         TheLocalHandle; // LocalHandle is a variable of winbase.h

/* Open the file.
   Read the first 8 bytes.
   Assure it is a FORM.
   Allocate enough memory to read the rest of the FORM chunk.
   Read the rest of the FORM chunk.
   Close the file. */

    groupsep = langs[language].groupsep;
    decpoint = langs[language].decpoint;

    if (language == LANGUAGE_ENG)
    {   return;
    }

    if (!(TheLocalHandle = fopen(langs[language].filename, "rb")))
    {   code = 1;                               /* can't open file */
        goto DONE;
    }
    if (fread(LocalIOBuffer, 8, 1, TheLocalHandle) != 1)
    {   code = 2;                               /* can't read file (2), or too short (3) */
        DISCARD fclose(TheLocalHandle);
        // TheLocalHandle = NULL;
        goto DONE;
    }

    p8Chunk = (struct Chunk*) &LocalIOBuffer[0];
    if (p8Chunk->ckID != (ULONG) ID_FORM)
    {   code = 3;
        DISCARD fclose(TheLocalHandle);
        // TheLocalHandle = NULL;
        goto DONE;
    }
    fsize = (LocalIOBuffer[4] * 16777216)
          + (LocalIOBuffer[5] *    65536)
          + (LocalIOBuffer[6] *      256)
          +  LocalIOBuffer[7];
    if (!(fbase = (UBYTE*) malloc(fsize)))
    {   code = 4;
        DISCARD fclose(TheLocalHandle);
        // TheLocalHandle = NULL;
        goto DONE;
    }
    p8data = fbase;
    if (fread(p8data, fsize, 1, TheLocalHandle) != 1)
    {   code = 5;
        DISCARD fclose(TheLocalHandle);
        // TheLocalHandle = NULL;
        goto DONE;
    }
    DISCARD fclose(TheLocalHandle);
    // TheLocalHandle = NULL;

/* Assure it is a CTLG.
   Look at each chunk: if it is a STRS, remember offset and length.
   Allocate enough memory to store a copy of the STRS chunk.
   Copy the STRS chunk. */

    if (MAKE_ID(*p8data, *(p8data + 1), *(p8data + 2), *(p8data + 3)) != ID_CTLG)
    {   code = 6;
        goto DONE;
    }
    p8data += 4;

    while (p8data < fbase + fsize)
    {   thesize = (p8data[4] * 16777216)
                + (p8data[5] *    65536)
                + (p8data[6] *      256)
                +  p8data[7];
        p8Chunk = (struct Chunk*) p8data;
        switch (p8Chunk->ckID)
        {
        case ID_STRS:
            langs[language].size = (int) thesize;
            dataptr = (UBYTE*) (p8data + 8);
        }
        p8data += 8 + thesize;
        if ((thesize & 1) == 1)
        {   p8data++;
    }   }

    if (dataptr == NULL) // no STRS chunk found
    {   code = 7;
        goto DONE;
    }

    langs[language].winbuffer = (UBYTE*) malloc((ULONG) langs[language].size);
    if (langs[language].winbuffer == NULL)
    {   code = 8;
        goto DONE;
    }

    code = 0; // success
    memcpy(langs[language].winbuffer, dataptr, (unsigned int) langs[language].size);
    free(fbase);
    // fbase = NULL;

DONE:
    if (code)
    {   zprintf(TEXTPEN_ERROR, "Can't load \"%s\" (error %d)!\n\n", langs[language].filename, code);
    } else
    {   // zprintf(TEXTPEN_VERBOSE, "Loaded \"%s\" (%d bytes at $%X).\n\n", langs[language].filename, langs[language].size, dataptr);
        parse_catalog();
}   }

EXPORT STRPTR GetCatalogStr(int catalog, LONG stringNum, STRPTR defaultString)
{   if
    (   language                          == LANGUAGE_ENG
     || langs[language].winbuffer         == NULL
     || langs[language].offset            == NULL
     || stringNum                         >  highestid
     || langs[language].offset[stringNum] == 0
    )
    {   return defaultString;
    } // implied else
    return &langs[language].winbuffer[langs[language].offset[stringNum]];
}

EXPORT void parse_catalog(void)
{   int i, j,
        id,
        length;

    // assert(langs[language].winbuffer);

    // find the highest ID
    i = 0;
    highestid = 0;
    while (i < langs[language].size)
    {   id     = (langs[language].winbuffer[i    ] * 16777216)
               + (langs[language].winbuffer[i + 1] *    65536)
               + (langs[language].winbuffer[i + 2] *      256)
               + (langs[language].winbuffer[i + 3]);
        if (id > highestid)
        {   highestid = id;
        }
        length = (langs[language].winbuffer[i + 4] * 16777216)
               + (langs[language].winbuffer[i + 5] *    65536)
               + (langs[language].winbuffer[i + 6] *      256)
               + (langs[language].winbuffer[i + 7]);

        i += 8 + length + 1;
        if   ((i % 4) == 1) i += 3;
        elif ((i % 4) == 2) i += 2;
        elif ((i % 4) == 3) i++;
    }

    // fill langs[].offset[] array
    langs[language].offset = (ULONG*) calloc((size_t) (4 * (highestid + 1)), 1);
    i = 0;
    while (i < langs[language].size)
    {   id     = (langs[language].winbuffer[i    ] * 16777216)
               + (langs[language].winbuffer[i + 1] *    65536)
               + (langs[language].winbuffer[i + 2] *      256)
               + (langs[language].winbuffer[i + 3]);
        length = (langs[language].winbuffer[i + 4] * 16777216)
               + (langs[language].winbuffer[i + 5] *    65536)
               + (langs[language].winbuffer[i + 6] *      256)
               + (langs[language].winbuffer[i + 7]);
        langs[language].offset[id] = (ULONG) (i + 8);

        // zprintf(TEXTPEN_VERBOSE, "%d,%d,%d:%s\n", i, id, length, &langs[language].winbuffer[langs[language].offset[id]]);

        if (language == LANGUAGE_GRE)
        {   // convert from ELOT 928 to Windows-1253
            for (j = i + 8; j < i + 8 + length; j++)
            {   if (langs[language].winbuffer[j] == 181) // ./.
                {   langs[language].winbuffer[j] = 161;
                } elif (langs[language].winbuffer[j] == 182) // 'A
                {   langs[language].winbuffer[j] = 162;
        }   }   }

        i += 8 + length + 1;
        if   ((i % 4) == 1) i += 3;
        elif ((i % 4) == 2) i += 2;
        elif ((i % 4) == 3) i++;
}   }

MODULE void translate(void)
{   keyname[SCAN_SPACEBAR].name = LLL(MSG_KEY_SPACE           , "Space"                         );
//  keyname[SCAN_BKSP    ].name = LLL(MSG_KEY_BKSP            , "BkSp"                          );
    keyname[SCAN_TAB     ].name = LLL(MSG_KEY_TAB             , "Tab"                           );
//  keyname[SCAN_ESC     ].name = LLL(MSG_KEY_ESC             , "Esc"                           );
    keyname[SCAN_DEL     ].name = LLL(MSG_KEY_DEL             , "Del"                           );
    keyname[SCAN_UP      ].name = LLL(MSG_KEY_UP              , "Up"                            );
    keyname[SCAN_DOWN    ].name = LLL(MSG_KEY_DN              , "Dn"                            );
    keyname[SCAN_RIGHT   ].name = LLL(MSG_KEY_RT              , "Rt"                            );
    keyname[SCAN_LEFT    ].name = LLL(MSG_KEY_LT              , "Lt"                            );
//  keyname[SCAN_HELP    ].name = LLL(MSG_KEY_HELP            , "Help"                          );
    keyname[SCAN_LSHIFT  ].name = LLL(MSG_KEY_LSHFT           , "LShft"                         );
    keyname[SCAN_RSHIFT  ].name = LLL(MSG_KEY_RSHFT           , "RShft"                         );
//  keyname[SCAN_CL      ].name = LLL(MSG_KEY_CL              , "CL"                            );
//  keyname[SCAN_LCTRL   ].name = LLL(MSG_KEY_LCTRL           , "LCtrl"                         );
//  keyname[SCAN_RCTRL   ].name = LLL(MSG_KEY_RCTRL           , "RCtrl"                         );
    keyname[SCAN_LALT    ].name = LLL(MSG_KEY_LALT            , "LAlt"                          );
    keyname[SCAN_RALT    ].name = LLL(MSG_KEY_RALT            , "RAlt"                          );
//  keyname[SCAN_LAMIGA  ].name = LLL(MSG_KEY_LAMI            , "LAmi"                          );
//  keyname[SCAN_RAMIGA  ].name = LLL(MSG_KEY_RAMI            , "RAmi"                          );
//  keyname[SCAN_STOP    ].name = LLL(MSG_KEY_STOP            , "Stop"                          );
//  keyname[SCAN_PLAY    ].name = LLL(MSG_KEY_PLAY            , "Play"                          );
//  keyname[SCAN_PREV    ].name = LLL(MSG_KEY_PREV            , "Prev"                          );
//  keyname[SCAN_NEXT    ].name = LLL(MSG_KEY_NEXT            , "Next"                          );
//  keyname[SCAN_RWND    ].name = LLL(MSG_KEY_RWND            , "Rwnd"                          );
//  keyname[SCAN_FFWD    ].name = LLL(MSG_KEY_FFWD            , "FFwd"                          );
//  keyname[SCAN_RESET   ].name = LLL(MSG_KEY_RESET           , "Reset"                         );
//  keyname[SCAN_WHLDN   ].name = LLL(MSG_KEY_WHLDN           , "WhlDn"                         );
//  keyname[SCAN_WHLUP   ].name = LLL(MSG_KEY_WHLUP           , "WhlUp"                         );

    filekind[KIND_ILBM ].hail   = LLL(MSG_HAIL_SAVEILBM       , "Save Screenshot as IFF ILBM"   );
    filekind[KIND_ACBM ].hail   = LLL(MSG_HAIL_SAVEACBM       , "Save Screenshot as IFF ACBM"   );
    filekind[KIND_BMP  ].hail   = LLL(MSG_HAIL_SAVEBMP        , "Save Screenshot as BMP"        );
    filekind[KIND_GIF  ].hail   = LLL(MSG_HAIL_SAVEGIF        , "Save Screenshot as GIF"        );
    filekind[KIND_PCX  ].hail   = LLL(MSG_HAIL_SAVEPCX        , "Save Screenshot as PCX"        );
    filekind[KIND_TIFF ].hail   = LLL(MSG_HAIL_SAVETIFF       , "Save Screenshot as TIFF"       );
    filekind[KIND_PNG  ].hail   = LLL(MSG_HAIL_SAVEPNG        , "Save Screenshot as PNG"        );
    filekind[KIND_BIN  ].hail   = LLL(MSG_HAIL_SAVEBIN        , "Save Game ROM (BIN)"           );
    filekind[KIND_COS  ].hail   = LLL(MSG_HAIL_SAVECOS        , "Save State"                    );
    filekind[KIND_COR  ].hail   = LLL(MSG_HAIL_SAVECOR        , "Start Recording"               );
    filekind[KIND_WAV  ].hail   = LLL(MSG_HAIL_CREATEWAV      , "Create WAV Tape"               );
    filekind[KIND_8SVX ].hail   = LLL(MSG_HAIL_CREATE8SVX     , "Create IFF 8SVX Tape"          );
    filekind[KIND_AIFF ].hail   = LLL(MSG_HAIL_CREATEAIFF     , "Create IFF AIFF Tape"          );
    filekind[KIND_ASCII].hail   = LLL(MSG_HAIL_SAVEASCII      , "Save as ASCII Text"            );
    filekind[KIND_TVC  ].hail   = LLL(MSG_HAIL_SAVETVC        , "Save Program (TVC)"            );
    filekind[KIND_MDCR ].hail   = LLL(MSG_HAIL_CREATEMDCR     , "Create MDCR Tape"              );
    filekind[KIND_RAW  ].hail   = LLL(MSG_HAIL_SAVERAW        , "Save BINGUG Floppy Disk"       );
    filekind[KIND_ASM  ].hail   = LLL(MSG_HAIL_SAVEASM        , "Save Assembler Source Code"    );
    filekind[KIND_PAP  ].hail   = LLL(MSG_HAIL_CREATEPAPERTAPE, "Create Papertape"              );
 // hails of ANIM/AOF/CMD/EOF/IFF/MOD/MNG/PSG/SMUS/YM/ZIP aren't used

    changetimeunitnames();
    keynames_from_overlay();
    change_colour_names();
    changeconsolekeynames();
    generate_autotext();
}

EXPORT void capslock_off(void)
{   UBYTE keys[256];

    GetKeyboardState(keys);
    if (keys[VK_CAPITAL])
    {   // simulate keypress
        keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
        // simulate key release
        keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}   }

EXPORT void ReadJoystick(int joy)
{   ff_ReadJoystick(0);
    ff_ReadJoystick(1);
}

EXPORT void setpri(void)
{   // assert(ProcessPtr);
    // assert(MainThreadPtr);

    switch (priority)
    {
	case PRI_HIGH:
        SetPriorityClass(ProcessPtr, HIGH_PRIORITY_CLASS);
        SetThreadPriority(MainThreadPtr, THREAD_PRIORITY_NORMAL      ); //  0
     // SetThreadPriority(MainThreadPtr, THREAD_PRIORITY_ABOVE_NORMAL); // +1
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "High priority.\n");
#endif
    acase PRI_NORMAL:
        SetPriorityClass(ProcessPtr, NORMAL_PRIORITY_CLASS);
        SetThreadPriority(MainThreadPtr, THREAD_PRIORITY_NORMAL      ); //  0
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Normal priority.\n");
#endif
    acase PRI_LOW:
        SetPriorityClass(ProcessPtr, BELOW_NORMAL_PRIORITY_CLASS);
        SetThreadPriority(MainThreadPtr, THREAD_PRIORITY_BELOW_NORMAL); // -1
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Low priority.\n");
#endif
}   }

EXPORT void read_rtc(void)
{   FAST SYSTEMTIME TheTime;

    GetLocalTime(&TheTime);
    rtc.wkday = (int) TheTime.wDayOfWeek; // 3 bits
    rtc.year  = (int) TheTime.wYear;
    rtc.month = (int) TheTime.wMonth;     // 4 bits
    rtc.day   = (int) TheTime.wDay;       // 5 bits
    rtc.hour  = (int) TheTime.wHour;      // 5 bits
    rtc.min   = (int) TheTime.wMinute;    // 6 bits
    rtc.sec   = (int) TheTime.wSecond;    // 6 bits
}

EXPORT void flipbool(int* thevar, int command, FLAG doit)
{   if (*thevar) *thevar = FALSE; else *thevar = TRUE;
    updatemenu(command);
    if (doit)
    {   docommand(command);
}   }

EXPORT void help_manual(int menunum)
{   openurl("WinArcadia.htm");
}

EXPORT void calibrate(void)
{   // assert(lpdi);
    IDirectInput_RunControlPanel(lpdi, MainWindowPtr, 0);
}

EXPORT void openurl(STRPTR command)
{   DISCARD ShellExecute(0, NULL, command, NULL, NULL, SW_SHOW);
}

EXPORT FLAG Exists(STRPTR name)
{   if (GetFileAttributes(name) != 0xFFFFFFFF)
    {   return TRUE;
    } else
    {   return FALSE;
}   }

EXPORT void make_statusbar(void)
{   if (showstatusbars[wsm])
    {   hStatusBar = CreateStatusWindow
        (   WS_CHILD | WS_VISIBLE,
            "",
            MainWindowPtr,
            IDC_STATUSBAR
        );
        if (language == LANGUAGE_GRE)
        {   DISCARD SendMessage(hStatusBar, WM_SETFONT, (WPARAM) hGreekFont,   MAKELPARAM(FALSE, 0));
        } elif (language == LANGUAGE_RUS)
        {   DISCARD SendMessage(hStatusBar, WM_SETFONT, (WPARAM) hRussianFont, MAKELPARAM(FALSE, 0));
        }
//   0       1       2            3         4    5    6    7     8
// Glyph | Title | Speed: | ###% of ###% | 76 | 76 | 76 | 76 | CAPS

        StatusBarArray[7] = winwidth - (win7 ? 68 : 57);
        StatusBarArray[6] = StatusBarArray[7] - (( 2 * 7) + 4);
        StatusBarArray[5] = StatusBarArray[6] - (( 2 * 7) + 4);
        StatusBarArray[4] = StatusBarArray[5] - (( 2 * 7) + 4);
        StatusBarArray[3] = StatusBarArray[4] - (( 2 * 7) + 4);
        StatusBarArray[2] = StatusBarArray[3] -  (21 * 7);
        StatusBarArray[1] = StatusBarArray[2] -  ( 7 * 7); // enough for "Geschw.:"
        StatusBarArray[0] = 16 + 4; // enough for glyph
        DISCARD SendMessage(hStatusBar, SB_SETPARTS, 9, (LPARAM) StatusBarArray);
        update_capslock();
        update_speed();
    }

    generate_autotext();      // for panes 0 and 1
    update_floppydrive(2, 0); // for pane  4
    update_floppydrive(2, 1); // for pane  5
    update_floppydrive(2, 2); // for pane  6
    update_floppydrive(2, 3); // for pane  7
}

EXPORT void process_code(void)
{   switch (storedcode)
    {
    case SCAN_ESCAPE:
        if (!ctrl() && confirm())
        {   cleanexit(EXIT_SUCCESS);
        }
    acase SCAN_FULLSTOP:
        if (ctrl())
        {   if (shift())
            {   if (fn_script[0] != EOS && (!cheevos || !RA_HardcoreModeIsActive()))
                {   macro_rexx(FALSE, FALSE);
            }   }
            else
            {   if (!cheevos || !RA_HardcoreModeIsActive())
                {   macro_rexx(TRUE, FALSE);
        }   }   }
    acase SCAN_VOLDOWN:
        if (hostvolume > 0)
        {   sound_off(TRUE); // we want to force a refresh of the sound
            hostvolume--;
            sound_on(FALSE);
        }
    acase SCAN_VOLUP:
        if (hostvolume < HOSTVOLUME_MAX)
        {   sound_off(TRUE); // we want to force a refresh of the sound
            hostvolume++;
            sound_on(FALSE);
        }
    acase SCAN_WEB:
        openurl("http://amigan.yatho.com/");
    acase SCAN_BACKTICK:
        if (ctrl())
        {   if (shift())
            {   docommand(MENUITEM_LEVELSKIP);
            } else
            {   if (SubWindowPtr[SUBWINDOW_MONITOR_XVI])
                {   close_subwindow(SUBWINDOW_MONITOR_XVI);
                } else
                {   view_monitor(SUBWINDOW_MONITOR_XVI);
        }   }   }
    acase SCAN_B:
        if (ctrl() && !shift())
        {   if
            (   machine == ARCADIA
          // || machine == BINBUG
             || machines[machine].pvis
             || memmap == MEMMAP_8600
            )
            {   autofire[0] = autofire[0] ? FALSE : TRUE;
                docommand(MENUITEM_AUTOFIRE1);
        }   }
    acase SCAN_C:
        if (ctrl() && !shift())
        {   savescreenshot(KIND_BMP, FALSE, TRUE, FALSE);
        }
    acase SCAN_D:
        if (ctrl())
        {   if (!shift())
            {   showstatusbars[wsm] = showstatusbars[wsm] ? FALSE : TRUE;
                docommand(MENUITEM_STATUSBAR);
            } else
            {   ;
#ifdef CHECKBLOCKS
                checkblocks();
#endif
        }   }
    acase SCAN_E:
        if (ctrl() && !shift())
        {   showtitlebars[wsm] = showtitlebars[wsm] ? FALSE : TRUE;
            docommand(MENUITEM_TITLEBAR);
        }
    acase SCAN_F:
        if (ctrl() && !shift())
        {   fullscreen = fullscreen ? FALSE : TRUE;
            docommand(MENUITEM_FULLSCREEN);
        }
    acase SCAN_MI:
        if
        (   ctrl()
         && !shift()
         && speedup > SPEED_MIN
         && (!cheevos || !RA_HardcoreModeIsActive() || speedup >= SPEED_5QUARTERS)
        )
        {   speedup--;
            update_speed();
        }
    acase SCAN_PL:
        if (ctrl() && !shift() && speedup < SPEED_MAX)
        {   speedup++;
            update_speed();
        }
    acase SCAN_G:
        if (ctrl() && !shift())
        {   if (SubWindowPtr[SUBWINDOW_HOSTPADS])
            {   close_subwindow(SUBWINDOW_HOSTPADS);
            } else
            {   help_hostpads();
        }   }
    acase SCAN_H:
        if (ctrl() && !shift())
        {   showtoolbars[wsm] = showtoolbars[wsm] ? FALSE : TRUE;
            docommand(MENUITEM_TOOLBAR);
        }
    acase SCAN_I:
        if (ctrl() && !shift())
        {   docommand(MENUITEM_SIDEBAR);
        }
    acase SCAN_J:
        if (ctrl() && !shift())
        {   if (machines[machine].joystick && machine != BINBUG)
            {   swapped = swapped ? FALSE : TRUE;
                docommand(MENUITEM_SWAPPED);
        }   }
    acase SCAN_K:
        if (ctrl() && !shift())
        {   if (SubWindowPtr[SUBWINDOW_HOSTKYBD])
            {   close_subwindow(SUBWINDOW_HOSTKYBD);
            } else
            {   help_hostkybd();
        }   }
    acase SCAN_L:
        if (ctrl() && !shift())
        {   if
            (   (machine == ARCADIA || machine == PONG || machines[machine].pvis)
             && (!cheevos || !RA_HardcoreModeIsActive())
            )
            {   collisions = collisions ? FALSE : TRUE;
                docommand(MENUITEM_COLLISIONS);
        }   }
    acase SCAN_M:
        if (ctrl() && !shift())
        {   showmenubars[wsm] = showmenubars[wsm] ? FALSE : TRUE;
            docommand(MENUITEM_MENUBAR);
        }
    acase SCAN_N:
        if
        (   ctrl()
         && !shift()
         && !fullscreen
         && (   (recmode == RECMODE_NORMAL || (recmode == RECMODE_PLAY && !generate))
             || (!apnganims && !iffanims && !gifanims && !mnganims)
        )   )
        {   if (wide == 1)
            {   wide = realwide = 2;
            } else
            {   // assert(wide == 2);
                wide = realwide = 1;
            }
            updatemenu(MENUITEM_NARROW);
            resize(size, FALSE);
        }
    acase SCAN_O:
        if (!crippled && ctrl() && !shift())
        {   project_open();
        }
    acase SCAN_P:
        if (!ctrl() && !shift())
        {   if (paused)
            {   emu_unpause();
            } elif (!ISQWERTY)
            {   FLAG found = FALSE;
                int  j, k;

                for (j = 0; j < 4; j++)
                {   if (console[j] == SCAN_P && machine != SELBST)
                    {   found = TRUE;
                        break; // for speed
                }   }
                if (!found)
                {   for (j = 0; j < 2; j++)
                    {   for (k = 0; k < NUMKEYS; k++)
                        {   if (keypads[j][k] == SCAN_P && keyexists(k))
                            {   found = TRUE;
                                break; // for speed
                    }   }   }
                    if (!found)
                    {   emu_pause();
        }   }   }   }
    acase SCAN_R:
        if (ctrl() && !shift())
        {   if (!crippled && game)
            {   docommand(MENUITEM_REVERT);
        }   }
    acase SCAN_S:
        if (ctrl() && !shift())
        {   if (!crippled && machine != PONG)
            {   project_save(KIND_COS);
        }   }
    acase SCAN_T:
        if (ctrl() && !shift())
        {   turbo = turbo ? FALSE : TRUE;
            docommand(MENUITEM_TURBO);
        }
    acase SCAN_STOP:
        if (ctrl() && !shift())
        {   macro_stop();
        }
    acase SCAN_U:
        if (ctrl() && !shift())
        {   sound = sound ? FALSE : TRUE;
            docommand(MENUITEM_SOUND);
        }
    acase SCAN_MUTE:
        if (!ctrl() && !shift())
        {   sound = sound ? FALSE : TRUE;
            docommand(MENUITEM_SOUND);
        }
    acase SCAN_V:
        if (ctrl() && !shift())
        {   edit_pastetext();
        }
    acase SCAN_W:
        if (ctrl() && !shift())
        {   if (SubWindowPtr[SUBWINDOW_CONTROLS])
            {   close_subwindow(SUBWINDOW_CONTROLS);
            } else
            {   view_controls();
        }   }
    acase SCAN_X:
        if (ctrl() && !shift())
        {   if (showpointers[wsm]) showpointers[wsm] = FALSE; else showpointers[wsm] = TRUE;
            docommand(MENUITEM_POINTER);
        }
    acase SCAN_Y:
        if (ctrl() && !shift())
        {   savescreenshot(KIND_BMP, TRUE, FALSE, FALSE);
        }
    acase SCAN_Z:
        if (ctrl() && !shift())
        {   if
            (   machine == ARCADIA
          // || machine == BINBUG
             || machines[machine].pvis
             || memmap == MEMMAP_8600
            )
            {   autofire[1] = autofire[1] ? FALSE : TRUE;
                docommand(MENUITEM_AUTOFIRE2);
        }   }
    acase SCAN_A1:
    case SCAN_N1:
        if (ctrl())
        {   if (shift())
            {   view_hiscores();
            } else
            {   command_changemachine(ARCADIA, MEMMAP_ARCADIA);
        }   }
    acase SCAN_A2:
    case SCAN_N2:
        if (ctrl())
        {   if (shift())
            {   if (SubWindowPtr[SUBWINDOW_DIPS])
                {   close_subwindow(SUBWINDOW_DIPS);
                } else
                {   edit_dips();
            }   }
            else
            {   command_changemachine(INTERTON, MEMMAP_D);
        }   }
    acase SCAN_A3:
    case SCAN_N3:
        if (ctrl())
        {   if (shift())
            {   if (SubWindowPtr[SUBWINDOW_MEMORY])
                {   close_subwindow(SUBWINDOW_MEMORY);
                } else
                {   edit_memory();
            }   }
            else
            {   command_changemachine(ELEKTOR, MEMMAP_F);
        }   }
    acase SCAN_A4:
    case SCAN_N4:
        if (ctrl())
        {   if (shift())
            {   if (SubWindowPtr[SUBWINDOW_PALETTE])
                {   close_subwindow(SUBWINDOW_PALETTE);
                } else
                {   edit_palette();
            }   }
            else
            {   command_changemachine(PIPBUG, MEMMAP_PIPBUG1);
        }   }
    acase SCAN_A5:
    case SCAN_N5:
        if (ctrl())
        {   if (shift())
            {   if (SubWindowPtr[SUBWINDOW_SPRITES])
                {   close_subwindow(SUBWINDOW_SPRITES);
                } else
                {   open_spriteeditor();
            }   }
            else
            {   if (!crippled)
                {   command_changemachine(BINBUG, MEMMAP_BINBUG);
        }   }   }
    acase SCAN_A6:
    case SCAN_N6:
        if (ctrl())
        {   if (shift())
            {   if (SubWindowPtr[SUBWINDOW_MONITOR_PSGS])
                {   close_subwindow(SUBWINDOW_MONITOR_PSGS);
                } else
                {   view_monitor(SUBWINDOW_MONITOR_PSGS);
            }   }
            else
            {   command_changemachine(INSTRUCTOR, MEMMAP_O);
        }   }
    acase SCAN_A7:
    case SCAN_N7:
        if (ctrl())
        {   if (shift())
            {   help_gameinfo();
            } else
            {   command_changemachine(    TWIN,       MEMMAP_TWIN);
        }   }
    acase SCAN_A8:
    case SCAN_N8:
        if (ctrl())
        {   if (shift())
            {   if
                (   machine == ARCADIA
                 || machines[machine].pvis
                 || machine == INSTRUCTOR
                 || machine == PONG
                 || machine == SELBST
                 || machine == MIKIT
                 || machine == TYPERIGHT
                ) // eg. PIPBUG, BINBUG, CD2650
                {   docommand(MENUITEM_REDEFINEKEYS);
            }   }
            else
            {   command_changemachine(CD2650, MEMMAP_CD2650);
        }   }
    acase SCAN_A9:
    case SCAN_N9:
        if (ctrl())
        {   if (shift())
            {   if (!crippled)
                {   docommand(MENUITEM_QUICKLOAD);
            }   }
            else
            {   command_changemachine(PHUNSY, MEMMAP_PHUNSY);
        }   }
    acase SCAN_A0:
    case SCAN_N0:
        if (ctrl())
        {   if (shift())
            {   if (!crippled)
                {   docommand(MENUITEM_QUICKSAVE);
            }   }
            else
            {   command_changemachine(SELBST, MEMMAP_SELBST);
        }   }
    acase SCAN_F1:
        if (ctrl())
        {   if (shift())
            {   command_changemachine(MIKIT, MEMMAP_MIKIT);
            } else
            {   if (!fullscreen && size != 1)
                {   resize(1, FALSE);
        }   }   }
    acase SCAN_F2:
        if (ctrl())
        {   if (shift())
            {   command_changemachine(ZACCARIA,   MEMMAP_ASTROWARS);
            } else
            {   if (!fullscreen && size != 2)
                {   resize(2, FALSE);
        }   }   }
    acase SCAN_F3:
        if (ctrl())
        {   if (shift())
            {   command_changemachine(ZACCARIA,   MEMMAP_GALAXIA);
            } else
            {   if (!fullscreen && size != 3)
                {   resize(3, FALSE);
        }   }   }
    acase SCAN_F4:
        if (ctrl())
        {   if (shift())
            {   command_changemachine(ZACCARIA,   MEMMAP_LASERBATTLE);
            } else
            {   if (!fullscreen && size != 4)
                {   resize(4, FALSE);
        }   }   }
    acase SCAN_F5:
        if (ctrl())
        {   if (shift())
            {   command_changemachine(ZACCARIA,   MEMMAP_LAZARIAN);
            } else
            {   if (!fullscreen && size != 5)
                {   resize(5, FALSE);
        }   }   }
        else
        {   if (shift())
            {   if
                (   machine == ELEKTOR
                 || machine == PIPBUG
                 || machine == BINBUG
                 || machine == INSTRUCTOR
                 || machine == CD2650
                 || machine == PHUNSY
                 || machine == SELBST
                 || machine == MIKIT
                )
                {   docommand(MENUITEM_RESETTOMONITOR); // Shift+F5
            }   }
            else
            {   if (!crippled)
                {   project_reset(FALSE); // F5
        }   }   }
    acase SCAN_F6:
        if (ctrl())
        {   if (shift())
            {   command_changemachine(MALZAK,     MEMMAP_MALZAK1);
            } else
            {   if (!fullscreen && size != 6)
                {   resize(6, FALSE);
        }   }   }
    acase SCAN_F7:
        if (ctrl() && shift())
        {   command_changemachine(    MALZAK,     MEMMAP_MALZAK2);
        }
    acase SCAN_F8:
        if (ctrl() && shift())
        {   command_changemachine(    PONG,       MEMMAP_8550);
        }
    acase SCAN_F9:
        if (ctrl() && shift())
        {   command_changemachine(    PONG,       MEMMAP_8600);
        }
    acase SCAN_F11:
        if (ctrl() && shift())
        {   command_changemachine(    TYPERIGHT,  MEMMAP_TYPERIGHT);
        } elif (!ctrl() && !shift())
        {   fullscreen = fullscreen ? FALSE : TRUE;
            docommand(MENUITEM_FULLSCREEN);
        }
    acase SCAN_FFWD:
        if (!ctrl() && !shift())
        {   turbo = turbo ? FALSE : TRUE;
            docommand(MENUITEM_TURBO);
        }
    acase SCAN_F12:
        if (shift())
        {   if (!ctrl())
            {   if (SubWindowPtr[SUBWINDOW_MONITOR_CPU])
                {   close_subwindow(SUBWINDOW_MONITOR_CPU);
                } else
                {   view_monitor(SUBWINDOW_MONITOR_CPU);
        }   }   }
        else
        {   if (ctrl())
            {   // assert(!iconified);
                clearkybd();
                iconify();
        }   }
    acase SCAN_SEMICOLON:
        if (ctrl() && shift())
        {   adjust_sound();
        }
    acase SCAN_AMINUS:
        if (ctrl())
        {   if (shift())
            {   savescreenshot(KIND_ILBM, TRUE, FALSE, FALSE);
        }   }
    acase SCAN_OB:
        if (ctrl())
        {   if (shift())
            {   if (!crippled)
                {   project_save(KIND_COR);
            }   }
            else
            {   if (recmode == RECMODE_NORMAL && (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR || machine == PONG))
                {   region = REGION_NTSC;
                    docommand(MENUFAKE_REGION);
        }   }   }
    acase SCAN_CB:
        if (ctrl())
        {   if (shift())
            {   macro_stop();
            } else
            {   if (recmode == RECMODE_NORMAL && (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR || machine == PONG))
                {   region = REGION_PAL;
                    docommand(MENUFAKE_REGION);
        }   }   }
    acase SCAN_COMMA:
        if (ctrl() && !shift())
        {   if
            (   machine == ELEKTOR
             || machine == PIPBUG
             || machine == BINBUG
             || machine == INSTRUCTOR
             || machine == CD2650
             || machine == PHUNSY
             || machine == SELBST
             || machine == MIKIT
            )
            {   docommand(MENUITEM_RESETTOMONITOR);
        }   }
    acase SCAN_SLASH:
        if (ctrl())
        {   if (shift())
            {   help_about();
            } else
            {   if (!cheevos || !RA_HardcoreModeIsActive())
                {   showdebugger[wsm] = showdebugger[wsm] ? FALSE : TRUE;
                    docommand(MENUITEM_VIEWDEBUGGER);
        }   }   }
    acase SCAN_AE:
    case SCAN_NE:
        if (!ctrl() && !shift())
        {   if (GetActiveWindow() == SubWindowPtr[SUBWINDOW_PALETTE])
            {   SetFocus
                (   GetNextDlgTabItem
                    (   SubWindowPtr[SUBWINDOW_PALETTE],
                        GetFocus(),
                        FALSE
                )   );
        }   }
    acase SCAN_TAB:
        if (!ctrl())
        {   if (GetActiveWindow() == SubWindowPtr[SUBWINDOW_PALETTE])
            {   SetFocus
                (   GetNextDlgTabItem
                    (   SubWindowPtr[SUBWINDOW_PALETTE],
                        GetFocus(),
                        shift() ? TRUE : FALSE
                )   );
            } elif (showdebugger[wsm])
            {   debugger_enter();
        }   }
    acase SCAN_PAUSE:
    case SCAN_PLAY:
        if (!ctrl() && !shift())
        {   if (paused)
            {   emu_unpause();
            } else
            {   emu_pause();
        }   }
    acase SCAN_CALC:
        if (!shift() && !ctrl())
        {   if (SubWindowPtr[SUBWINDOW_MONITOR_CPU])
            {   close_subwindow(SUBWINDOW_MONITOR_CPU);
            } else
            {   view_monitor(SUBWINDOW_MONITOR_CPU);
        }   }
 /* acase SCAN_SCRLK:
        if (!ctrl() && !shift())
        {   if (frameskip == FRAMESKIP_INFINITE)
            {   frameskip = 1;
            } else
            {   frameskip = FRAMESKIP_INFINITE;
        }   } */
    acase SCAN_LCTRL:
    case SCAN_RCTRL:
    case SCAN_LSHIFT:
    case SCAN_RSHIFT:
        KeyMatrix[storedcode / 8] |= (1 << (storedcode % 8));
    acase SCAN_UP:
    case SCAN_LEFT:
        if
        (   ctrl()
         && showsidebars[wsm]
         && foundgames
         && !crippled
        )
        {   sidebar_up();
        }
    acase SCAN_PREV:
        if
        (   showsidebars[wsm]
         && foundgames
         && !crippled
        )
        {   sidebar_up();
        }
    acase SCAN_DOWN:
    case SCAN_RIGHT:
        if
        (   ctrl()
         && showsidebars[wsm]
         && foundgames
         && !crippled
        )
        {   sidebar_down();
        }
    acase SCAN_NEXT:
        if
        (   showsidebars[wsm]
         && foundgames
         && !crippled
        )
        {   sidebar_down();
    }   }
    storedcode = 0;

    switch (storedaltcode)
    {
    case SCAN_C:
        ax[0] = ax[1] = ay[0] = ay[1] = ay[2] = ay[3] = machines[machine].digipos[1];
        do_axes();
    acase SCAN_J:
        if (machines[machine].joystick)
        {   swapped = swapped ? FALSE : TRUE;
            docommand(MENUITEM_SWAPPED);
        }
    acase SCAN_L:
        if (!crippled)
        {   storedaltcode = 0; // important!
            docommand(MENUITEM_QUICKLOAD);
        }
    acase SCAN_R:
        if (!crippled)
        {   project_reset(FALSE);
        }
    acase SCAN_S:
        if (!crippled)
        {   docommand(MENUITEM_QUICKSAVE);
        }
    acase SCAN_W:
        turbo = turbo ? FALSE : TRUE;
        docommand(MENUITEM_TURBO);
    acase SCAN_AE:
    case SCAN_NE:
        if (!ctrl() && !shift())
        {   fullscreen = fullscreen ? FALSE : TRUE;
            docommand(MENUITEM_FULLSCREEN);
    }   }
    storedaltcode = 0;

    if (storedmenu1 != -1)
    {   handle_menu(storedmenu1);
        storedmenu1 = -1;
    }
    if (storedmenu2 != -1)
    {   handle_menu2(storedmenu2);
        storedmenu2 = -1;
    }

    if (newgame != -1)
    {   macro_stop();
        if (hSideBar && currentgame != -1 && newgame != currentgame) // to handle the user holding down Ctrl and clicking
        {   ListView_SetItemState(hSideBar, currentgame, 0, 0x000F);
        }
        currentgame = newgame;
        newgame = -1;
        dosidebar();
}   }

MODULE void sidebar_up(void)
{   int oldcurrentgame;

    oldcurrentgame = currentgame;
    if (currentgame == -1 || shift())
    {   if (foundgames > 2)
        {   currentgame = 2; // move to "top"
    }   }
    elif (currentgame > 0)
    {   currentgame--; // move up by 1
    }
    if (currentgame != oldcurrentgame)
    {   ListView_SetItemState(hSideBar, oldcurrentgame, 0, 0x000F);
        ListView_SetItemState(hSideBar, currentgame, (LVIS_FOCUSED | LVIS_SELECTED), 0x000F);
        if (currentgame != foundgames - 1)
        {   DISCARD ListView_EnsureVisible(hSideBar, currentgame, TRUE); // doesn't seem to work properly for items in the rightmost column
        }
        macro_stop();
        dosidebar(); // but window might be about to be resized!
}   }

MODULE void sidebar_down(void)
{   int oldcurrentgame;

    oldcurrentgame = currentgame;
    if (currentgame == -1 || shift())
    {   if (foundgames > 2)
        {   currentgame = foundgames - 1; // move to bottom
    }   }
    elif (currentgame < foundgames - 1)
    {   currentgame++; // move down by 1
    }
    if (currentgame != oldcurrentgame)
    {   ListView_SetItemState(hSideBar, oldcurrentgame, 0, 0x000F);
        ListView_SetItemState(hSideBar, currentgame, (LVIS_FOCUSED | LVIS_SELECTED), 0x000F);
        if (currentgame != foundgames - 1)
        {   DISCARD ListView_EnsureVisible(hSideBar, currentgame, TRUE); // doesn't seem to work properly for items in the rightmost column
        }
        macro_stop();
        dosidebar(); // but window might be about to be resized!
}   }

EXPORT void clearkybd(void)
{   refreshkybd();
}

EXPORT void refreshkybd(void)
{   int i;

    for (i = 0; i < 64; i++)
    {   KeyMatrix[i] = 0;
    }

    if (GetKeyState(VK_LCONTROL) & 0x8000) KeyMatrix[SCAN_LCTRL  / 8] |= 1 << (SCAN_LCTRL  % 8); else KeyMatrix[SCAN_LCTRL  / 8] &= ~(1 << (SCAN_LCTRL  % 8));
    if (GetKeyState(VK_RCONTROL) & 0x8000) KeyMatrix[SCAN_RCTRL  / 8] |= 1 << (SCAN_RCTRL  % 8); else KeyMatrix[SCAN_RCTRL  / 8] &= ~(1 << (SCAN_RCTRL  % 8));
    if (GetKeyState(VK_LSHIFT  ) & 0x8000) KeyMatrix[SCAN_LSHIFT / 8] |= 1 << (SCAN_LSHIFT % 8); else KeyMatrix[SCAN_LSHIFT / 8] &= ~(1 << (SCAN_LSHIFT % 8));
    if (GetKeyState(VK_RSHIFT  ) & 0x8000) KeyMatrix[SCAN_RSHIFT / 8] |= 1 << (SCAN_RSHIFT % 8); else KeyMatrix[SCAN_RSHIFT / 8] &= ~(1 << (SCAN_RSHIFT % 8));
}

EXPORT void updatebiggads(void)
{   if (!MainWindowPtr)
    {   return; // important!
    }

    if (crippled)
    {   if (hToolbar)
        {   DISCARD SendMessage(hToolbar, TB_SETSTATE, ID_FILE_RESET,                      0);
            DISCARD SendMessage(hToolbar, TB_SETSTATE, ID_FILE_OPEN,                       0);
            DISCARD SendMessage(hToolbar, TB_SETSTATE, ID_FILE_SAVESNP,                    0);
            DISCARD SendMessage(hToolbar, TB_SETSTATE, ID_FILE_QUICKLOAD,                  0);
            DISCARD SendMessage(hToolbar, TB_SETSTATE, ID_FILE_QUICKSAVE,                  0);
        }
        if (hSideBar)
        {   EnableWindow(hSideBar, FALSE);
    }   }
    else
    {   if (hToolbar)
        {   DISCARD SendMessage(hToolbar, TB_SETSTATE, ID_FILE_RESET,        TBSTATE_ENABLED);
            DISCARD SendMessage(hToolbar, TB_SETSTATE, ID_FILE_OPEN,         TBSTATE_ENABLED);
            DISCARD SendMessage(hToolbar, TB_SETSTATE, ID_FILE_SAVESNP,      TBSTATE_ENABLED);
            DISCARD SendMessage(hToolbar, TB_SETSTATE, ID_FILE_QUICKLOAD,    (cheevos && RA_HardcoreModeIsActive()) ? 0 : TBSTATE_ENABLED);
            DISCARD SendMessage(hToolbar, TB_SETSTATE, ID_FILE_QUICKSAVE,    TBSTATE_ENABLED);
        }
        if (hSideBar && foundgames)
        {   EnableWindow(hSideBar, TRUE);
    }   }

    if (hToolbar)
    {   DISCARD SendMessage(hToolbar, TB_SETSTATE, ID_MACRO_RUNREXX,         (cheevos && RA_HardcoreModeIsActive()) ? 0 : TBSTATE_ENABLED);
}   }

EXPORT void ghost(int which, int ghosted)
{   if (menucode[which] != -1)
    {   if (ghosted)
        {   if (MenuPtr)
            {   DISCARD EnableMenuItem(MenuPtr,  (UINT) menucode[which], MF_BYCOMMAND | MF_GRAYED);
            }
            DISCARD EnableMenuItem(PopUpMenuPtr, (UINT) menucode[which], MF_BYCOMMAND | MF_GRAYED);
        } else
        {   if (MenuPtr)
            {   DISCARD EnableMenuItem(MenuPtr,  (UINT) menucode[which], MF_BYCOMMAND | MF_ENABLED);
            }
            DISCARD EnableMenuItem(PopUpMenuPtr, (UINT) menucode[which], MF_BYCOMMAND | MF_ENABLED);
}   }   }
EXPORT void enable2(int which, int enabled)
{   if (menuopt[which] != -1)
    {   if (enabled)
        {   if (MenuPtr)
            {   DISCARD EnableMenuItem(MenuPtr,  (UINT) menuopt[ which], MF_BYCOMMAND | MF_ENABLED);
            }
            DISCARD EnableMenuItem(PopUpMenuPtr, (UINT) menuopt[ which], MF_BYCOMMAND | MF_ENABLED);
        } else
        {   if (MenuPtr)
            {   DISCARD EnableMenuItem(MenuPtr,  (UINT) menuopt[ which], MF_BYCOMMAND | MF_GRAYED);
            }
            DISCARD EnableMenuItem(PopUpMenuPtr, (UINT) menuopt[ which], MF_BYCOMMAND | MF_GRAYED);
}   }   }
EXPORT void checkmx(int which, int first, int last)
{   if (menuopt[which] != -1)
    {   // assert(menuopt[first] != -1);
        // assert(menuopt[last ] != -1);
        if (MenuPtr)
        {   DISCARD CheckMenuRadioItem(MenuPtr,  (UINT) menuopt[first], (UINT) menuopt[last], (UINT) menuopt[which], MF_BYCOMMAND);
        }
        DISCARD CheckMenuRadioItem(PopUpMenuPtr, (UINT) menuopt[first], (UINT) menuopt[last], (UINT) menuopt[which], MF_BYCOMMAND);
}   }
EXPORT void tick(int which, int ticked)
{   if (menucode[which] != -1)
    {   if (ticked)
        {   if (MenuPtr)
            {   DISCARD CheckMenuItem( MenuPtr,  (UINT) menucode[which], MF_BYCOMMAND | MF_CHECKED);
            }
            DISCARD CheckMenuItem( PopUpMenuPtr, (UINT) menucode[which], MF_BYCOMMAND | MF_CHECKED);
        } else
        {   if (MenuPtr)
            {   DISCARD CheckMenuItem( MenuPtr,  (UINT) menucode[which], MF_BYCOMMAND | MF_UNCHECKED);
            }
            DISCARD CheckMenuItem( PopUpMenuPtr, (UINT) menucode[which], MF_BYCOMMAND | MF_UNCHECKED);
}   }   }
EXPORT void setbutton(int which, FLAG enabled, FLAG state)
{   if (state)
    {   DISCARD SendMessage(hToolbar, TB_CHANGEBITMAP, (UINT) gadgetcode[which], gadgetimage[which][0]);
    } else
    {   DISCARD SendMessage(hToolbar, TB_CHANGEBITMAP, (UINT) gadgetcode[which], gadgetimage[which][1]);
    }
    if (!enabled)
    {   if (state)
        {   DISCARD SendMessage(hToolbar, TB_SETSTATE, (UINT) gadgetcode[which], TBSTATE_CHECKED);
        } else
        {   DISCARD SendMessage(hToolbar, TB_SETSTATE, (UINT) gadgetcode[which], 0);
    }   }
    else
    {   if (state)
        {   DISCARD SendMessage(hToolbar, TB_SETSTATE, (UINT) gadgetcode[which], TBSTATE_ENABLED | TBSTATE_CHECKED);
        } else
        {   DISCARD SendMessage(hToolbar, TB_SETSTATE, (UINT) gadgetcode[which], TBSTATE_ENABLED);
}   }   }

EXPORT void changefilepart(STRPTR pathname, STRPTR path, STRPTR filename, STRPTR newfilename)
{   int length;

    // sets pathname and filename according to path and newfilename

    strcpy(filename, newfilename);
    strcpy(pathname, path);
    length = strlen(path);
    if (length >= 1 && path[length - 1] == '\\')
    {   ;
    } else
    {   strcat(pathname, "\\");
    }
    strcat(pathname, filename);
}

EXPORT void uncheckmx(int which) { ; }
EXPORT void beep(void)           { ; }
EXPORT void lockgui(void)        { ; }
EXPORT void unlockgui(void)      { ; }

EXPORT void clearmsgs(void)
{   MSG Msg;

    if (!MainWindowPtr)
    {   return;
    }

    while (PeekMessage(&Msg, MainWindowPtr, 0, 0, PM_REMOVE))
    {   TranslateMessage(&Msg);
        DISCARD DispatchMessage(&Msg);
    }

    if (!ListeningWindowPtr)
    {   return;
    }

    while (PeekMessage(&Msg, ListeningWindowPtr, 0, 0, PM_REMOVE))
    {   TranslateMessage(&Msg);
        DISCARD DispatchMessage(&Msg);
}   }

EXPORT void audit(FLAG user)
{   TRANSIENT WIN32_FIND_DATA FindData;
    TRANSIENT HANDLE          FindHandle;
    TRANSIENT int             thelength,
                              thesize;
    PERSIST   TEXT            activefile[MAX_PATH + 1],
                              resultstring[768 + MAX_PATH + 1];

    if (showsidebars[wsm])
    {   busypointer();
        DISCARD ListView_DeleteAllItems(hSideBar);
    } else
    {   if (!user)
        {   needaudit = TRUE;
            return;
        }
        busypointer();
    }
    needaudit = FALSE;

    audit_coinops();

    DISCARD SetCurrentDirectory(path_games);

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Sidebar directory is %s!\n", path_games);
#endif

    FindHandle = FindFirstFile("*", &FindData);
    if (FindHandle == INVALID_HANDLE_VALUE)
    {   DISCARD SetCurrentDirectory(ProgDir);
        normalpointer();
        return;
    }

    do
    {   if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {   if
            (   strcmp(FindData.cFileName, "." )
             && strcmp(FindData.cFileName, "..")
            )
            {
#ifdef VERBOSE
                zprintf(TEXTPEN_VERBOSE, "Found directory \"%s\".\n", FindData.cFileName);
#endif
                strcpy(auditlist[foundgames].filename, FindData.cFileName);
                strcpy(auditlist[foundgames].gamename, FindData.cFileName);
                auditlist[foundgames].whichgame  = -1;
                auditlist[foundgames].machine   = -1;
                auditlist[foundgames].glyph     = IMAGE_FOLDER;
                auditlist[foundgames].memmap    = -1;
                sidebar[foundgames]             = foundgames;
                foundgames++;
                found_dirs++;
        }   }
        else
        {   thelength = (int) strlen(FindData.cFileName);
            strcpy(activefile, path_games);
            thelength = (int) strlen(activefile);
            if
            (   thelength >= 1
             && activefile[thelength - 1] != '\\'
             && activefile[thelength - 1] != ':'
            )
            {   strcat(activefile, "\\");
            }
            strcat(activefile, FindData.cFileName);
            DISCARD SetCurrentDirectory(ProgDir);
            thesize = (int) getsize(activefile);
            auditfile(activefile, thesize, user);
            DISCARD SetCurrentDirectory(path_games);
    }   }
    while (FindNextFile(FindHandle, &FindData));

    DISCARD FindClose(FindHandle);
    DISCARD SetCurrentDirectory(ProgDir);

    buildlistbrowser();

    if (user)
    {   DISCARD ShellExecute(MainWindowPtr, "explore", path_games, NULL, NULL, SW_SHOWNORMAL);
        makeaudittext(resultstring);
        sound_off(FALSE);
        normalpointer();

        MessageBoxEx
        (   MainWindowPtr,
            resultstring,
            LLL(MSG_AUDITRESULTS, "Audit Results"),
            MB_OK | MB_ICONINFORMATION | MB_APPLMODAL,
            (WORD) MAKELANGID(langs[language].lang, langs[language].sublang) // button is still always in English for some reason
        );

        clearkybd();
        sound_on(TRUE);
    } else
    {   normalpointer();
}   }

EXPORT void buildlistbrowser(void)
{   int    i,
           items = 0;
    LVITEM lvi;

    // This gets called twice at startup, unfortunately
    // (once by audit() and once by openwindow()).

    if (!showsidebars[wsm])
    {   return;
    }

    DISCARD ListView_DeleteAllItems(hSideBar);

    sortaudit();
    lvi.iSubItem    = 0;
    lvi.state       = 0;
    lvi.stateMask   = 0;
    lvi.mask        = LVIF_TEXT | LVIF_IMAGE;
    for (i = 0; i < foundgames; i++)
    {   lvi.iItem   = i;
        lvi.iImage  = auditlist[sidebar[i]].glyph;
        lvi.pszText = auditlist[sidebar[i]].gamename;
        DISCARD ListView_InsertItem(hSideBar, &lvi);
    }

    EnableWindow(hSideBar, TRUE);
    currentgame = -1;

    setselection();
}

/* The return value is whether to process keyboard directional input */
EXPORT FLAG system_dopaddle(int source, int dest)
{   FAST int x, y;

    if (hostcontroller[source] == CONTROLLER_TRACKBALL)
    {   getpointerpos(&x, &y);
        x /= 256;              // scale from 0..65535 down to 0..255
        y /= 256;              // scale from 0..65535 down to 0..255
        ax[dest] = x;
        ay[dest] = y;

        hosttoguestmouse(NULL, NULL, &gx[dest], &gy[dest], NULL, NULL);

        return FALSE;
    } // implied else

    if (hostcontroller[source] == CONTROLLER_1STDJOY)
    {   ax[dest] = jx[0];
        ay[dest] = jy[0];

        if (whichgame == SUPERINVPOS)
        {   if (ax[dest] == 0x80) ax[dest] = 0x7F;
            if (ay[dest] == 0x80) ay[dest] = 0x7F;
        }

        gx[dest] = (ax[dest] * machines[machine].width ) / 256;
        gy[dest] = (ay[dest] * machines[machine].height) / 256;

        return FALSE;
    } elif (hostcontroller[source] == CONTROLLER_2NDDJOY)
    {   ax[dest] = jx[1];
        ay[dest] = jy[1];

        if (whichgame == SUPERINVPOS)
        {   if (ax[dest] == 0x80) ax[dest] = 0x7F;
            if (ay[dest] == 0x80) ay[dest] = 0x7F;
        }

        gx[dest] = (ax[dest] * machines[machine].width ) / 256;
        gy[dest] = (ay[dest] * machines[machine].height) / 256;

        return FALSE;
    }

    // assert(hostcontroller[source] == CONTROLLER_NONE);
    return TRUE;
}

EXPORT void updatepointer(FLAG force, FLAG full)
{   int x, y;

    if (!MainWindowPtr)
    {   return;
    }

    ThePoint.x = 0;
    ThePoint.y = 0;
    DISCARD ClientToScreen(MainWindowPtr, &ThePoint);
    therect.left   = ThePoint.x + xoffset;
    therect.top    = ThePoint.y + yoffset;
    if (fullscreen && stretching != STRETCHING_NONE)
    {   therect.right  = ThePoint.x + xoffset + hostwidth  - 1;
        therect.bottom = ThePoint.y + yoffset + hostheight - 6;
    } else
    {   therect.right  = ThePoint.x + xoffset + hostwidth     ;
        therect.bottom = ThePoint.y + yoffset + hostheight    ;
        if (!fullscreen)
        {   therect.bottom--;
    }   }

    DISCARD GetCursorPos(&ThePoint);
    DISCARD ScreenToClient(MainWindowPtr, &ThePoint);
    x = ThePoint.x;
    y = ThePoint.y;
    if
    (   x >= xoffset
     && y >= yoffset
     && x <  xoffset + hostwidth
     && y <  yoffset + hostheight
    )
    {   if (!showpointers[wsm] && !paused)
        {   if (showingpointer)
            {   ShowCursor(FALSE);
                showingpointer = FALSE;
        }   }
        else
        {   if
            (   machines[machine].joystick
             && hostcontroller[0] == CONTROLLER_TRACKBALL
             && hostcontroller[1] == CONTROLLER_TRACKBALL
            )
            {   SetCursor(hCross[2]); // purple
            } elif
            (   machines[machine].joystick
             && hostcontroller[swapped ? 1 : 0] == CONTROLLER_TRACKBALL
            )
            {   SetCursor(hCross[0]); // red
            } elif
            (   machines[machine].joystick
             && hostcontroller[swapped ? 0 : 1] == CONTROLLER_TRACKBALL
            )
            {   SetCursor(hCross[1]); // blue
            } else
            {   SetCursor(hArrow);
            }
            if (!showingpointer)
            {   ShowCursor(TRUE);
                showingpointer = TRUE;
    }   }   }
    else
    {   SetCursor(hArrow);
        if (!showingpointer)
        {   ShowCursor(TRUE);
            showingpointer = TRUE;
    }   }

    measure_position();

    if (!full)
    {   return;
    }

    if
    (   machines[machine].joystick
     && confine
     && !paused
     && !modal
     && (   hostcontroller[0] == CONTROLLER_TRACKBALL
         || hostcontroller[1] == CONTROLLER_TRACKBALL
    )   )
    {   DISCARD ClipCursor(&therect);
    } else
    {   DISCARD ClipCursor(NULL);
}   }

EXPORT void changecolours(void)
{   make_stars();
    redrawscreen();
}

EXPORT void traymenu(void)
{   int          i;
    HMENU        TraySubMenuPtr;
    MENUITEMINFO mii =
    {   44,
        MIIM_STATE,
        0,
        MFS_DEFAULT, // to embolden it
        ID_FILE_ICONIFY,
        NULL,
        NULL,
        NULL,
        ID_FILE_ICONIFY,
        0,
        0
    };

    // assert(iconified);

    DISCARD SetForegroundWindow(MainWindowPtr);
    DISCARD GetCursorPos(&ThePoint);

    TrayMenuPtr = LoadMenu(InstancePtr, MAKEINTRESOURCE(langs[language].traymenu));
    for (i = 0; i < MIBS; i++)
    {   DISCARD SetMenuItemBitmaps
        (   TrayMenuPtr,                      // handle of menu
            mib[i].menuitem,                  // menu item to receive new bitmaps
            MF_BYCOMMAND,                     // menu item flags
            mib[i].mx ? NULL : mib[i].bitmap, // handle of unchecked bitmap
            mib[i].mx ? mib[i].bitmap : NULL  // handle of checked bitmap
        );
    }
    TraySubMenuPtr = GetSubMenu(TrayMenuPtr, 0);
    DISCARD SetMenuItemInfo
    (   TraySubMenuPtr,
        ID_FILE_UNICONIFY,
        FALSE,
        &mii
    );
    if (paused)
    {   DISCARD CheckMenuItem(TrayMenuPtr, (UINT) ID_SPEED_PAUSED, MF_BYCOMMAND | MF_CHECKED);
    } else
    {   DISCARD CheckMenuItem(TrayMenuPtr, (UINT) ID_SPEED_PAUSED, MF_BYCOMMAND | MF_UNCHECKED);
    }
    TrackPopupMenu
    (   TraySubMenuPtr,
        TPM_RIGHTALIGN | TPM_RIGHTBUTTON,
        ThePoint.x,
        ThePoint.y,
        0,
        MainWindowPtr,
        NULL
    );
    PostMessage(MainWindowPtr, WM_NULL, 0, 0);
}

EXPORT void setselection(void)
{   int i;

    if (!hSideBar)
    {   return;
    }

    SetFocus(hSideBar);

    if (currentgame != -1)
    {   ListView_SetItemState(hSideBar, currentgame, 0, 0x000F);
        currentgame = -1;
    }

    for (i = 0; i < foundgames; i++)
    {   if (auditlist[sidebar[i]].filename[0] != EOS && !stricmp(auditlist[sidebar[i]].filename, fn_game)) // entry has a filename and it matches ours
        {   currentgame = i;
            goto DONE;
    }   }
    // no match yet...
    for (i = 0; i < foundgames; i++)
    {   if (auditlist[sidebar[i]].filename[0] == EOS && memmap == auditlist[sidebar[i]].memmap) // entry lacks a filename and its memory map matches ours
        {   currentgame = i;
            break;
    }   }

DONE:
    ListView_SetItemState(hSideBar, currentgame, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
    DISCARD ListView_EnsureVisible(hSideBar, currentgame, TRUE);
}

EXPORT void clip_open(FLAG empty)
{   DISCARD OpenClipboard(NULL);
    if (empty)
    {   DISCARD EmptyClipboard();
}   }

EXPORT void clip_write(STRPTR data, int localsize, UINT kind)
{   HGLOBAL hGlobal;
    STRPTR  pGlobal;
    int     i;

    hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (DWORD) localsize + 1);
    pGlobal = GlobalLock(hGlobal);
    for (i = 0; i < localsize; i++)
    {   pGlobal[i] = data[i]; // perhaps use memcpy() instead?
    }
    pGlobal[localsize] = EOS; // because it must be NUL-terminated
    GlobalUnlock(hGlobal);

    DISCARD SetClipboardData(kind, hGlobal);
}

EXPORT void clip_read(void)
{   TRANSIENT HGLOBAL hGlobal;
    TRANSIENT STRPTR  pGlobal;
    TRANSIENT int     i, j,
                      thesize;
    PERSIST   TEXT    PasteBuffer[QUEUESIZE + 1]; // PERSISTent so as not to blow the stack

 /* if (!(IsClipboardFormatAvailable(CF_TEXT)))
    {   return;
    } */
    if (!(hGlobal = GetClipboardData(CF_TEXT)))
    {   return;
    }
    thesize = (int) GlobalSize(hGlobal);
    if (machine == CD2650)
    {   if (thesize > QUEUESIZE / 2)
        {   thesize = QUEUESIZE / 2;
    }   }
    else
    {   if (thesize > QUEUESIZE)
        {   thesize = QUEUESIZE;
    }   }
    pGlobal = GlobalLock(hGlobal);
    for (i = 0; i < thesize; i++)
    {   PasteBuffer[i] = pGlobal[i];
    }
    GlobalUnlock(hGlobal);
    PasteBuffer[thesize] = EOS;

    if (thesize <= 1) // because it's NULL-terminated
    {   return;
    }

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Pasting %s (%d characters).\n", PasteBuffer, thesize);
#endif

    for (i = 0; i < thesize - 1; i++)
    {   if
        (   (machine == CD2650 && queuepos >= QUEUESIZE - 1)
         || queuepos >= QUEUESIZE
         || PasteBuffer[i] == EOS
        )
        {   break;
        } // implied else

        switch (machine)
        {
        case PIPBUG:
        case BINBUG:
            if (PasteBuffer[i] == LF)
            {   continue;
            }
        acase CD2650:
            if (PasteBuffer[i] >= 'a' && PasteBuffer[i] <= 'z')
            {   PasteBuffer[i] -= 96;
        }   }

        // not enqueue() for these!
        thequeue[queuepos++] = PasteBuffer[i];
        if (machine == CD2650)
        {   thequeue[queuepos++] = 0xFF;
        } elif (machine == PIPBUG && PasteBuffer[i] == CR)
        {   for (j = 0; j < 13; j++)
            {   thequeue[queuepos++] = NC;
}   }   }   }

EXPORT void clip_close(void)
{   CloseClipboard();
}

EXPORT void edit_pastetext(void)
{   if (!ISQWERTY || recmode == RECMODE_PLAY)
    {   return; // important!
    }

    if (machine != TYPERIGHT && !queuekeystrokes)
    {   queuekeystrokes = TRUE;
        updatemenu(MENUITEM_QUEUEKEYSTROKES);
    }

    clip_open(FALSE);
    clip_read();
    clip_close(); // it would be better to close the clipboard earlier
}

EXPORT void iconify(void)
{   NOTIFYICONDATA tnid;
    int            i;

    if (iconified)
    {   return;
    }

    if (autopause)
    {   emu_pause();
    }

    tnid.cbSize           = sizeof(NOTIFYICONDATA);
    tnid.hWnd             = MainWindowPtr;
    tnid.uID              = 1;
    tnid.uFlags           = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    tnid.uCallbackMessage = USR_NOTIFYICON;
    tnid.hIcon            = smlicon;
    strcpy(tnid.szTip, titlebartext);
    DISCARD Shell_NotifyIcon(NIM_ADD, &tnid);

    ShowWindow(MainWindowPtr, SW_HIDE);
    for (i = 0; i < SUBWINDOWS; i++)
    {   if (SubWindowPtr[i])
        {   ShowWindow(SubWindowPtr[i], SW_HIDE);
    }   }

    blanker_on();

    iconified = TRUE;
}

EXPORT void uniconify(FLAG exiting)
{   NOTIFYICONDATA tnid;
    int            i;

    if (!exiting)
    {   ShowWindow(MainWindowPtr, SW_SHOW);
        for (i = 0; i < SUBWINDOWS; i++)
        {   if (SubWindowPtr[i])
            {   ShowWindow(SubWindowPtr[i], SW_SHOW);
        }   }
        DISCARD SetActiveWindow(MainWindowPtr);
    }

    tnid.cbSize = sizeof(NOTIFYICONDATA);
    tnid.hWnd   = MainWindowPtr;
    tnid.uID    = 1;
    DISCARD Shell_NotifyIcon(NIM_DELETE, &tnid);
    DestroyMenu(TrayMenuPtr);
    TrayMenuPtr = NULL;

    if (!paused)
    {   blanker_off();
    }

    iconified = FALSE;
}

EXPORT void getpointerpos(int* x, int* y)
{   DISCARD GetCursorPos(&ThePoint);
    DISCARD ScreenToClient(MainWindowPtr, &ThePoint);
    *x = ThePoint.x;
    *y = ThePoint.y;
    if (*x < xoffset) *x = xoffset;
    if (*y < yoffset) *y = yoffset;
    if (*x > xoffset + hostwidth ) *x = xoffset + hostwidth;
    if (*y > yoffset + hostheight) *y = yoffset + hostheight;
    *x -= xoffset;
    *y -= yoffset;
    // assert(*x >= 0     );
    // assert(*y >= 0     );
    // assert(*x <  hostwidth );
    // assert(*y <  hostheight);
    *x *= (65535 / hostwidth ); // scale from 0..width  up to 0..65535
    *y *= (65535 / hostheight); // scale from 0..height up to 0..65535
}

EXPORT void update_joynames(void)
{   if (!unit[0] && hostcontroller[0] == CONTROLLER_1STDJOY)
    {   hostcontroller[0] = CONTROLLER_NONE;
    }
    if (!unit[0] && hostcontroller[1] == CONTROLLER_1STDJOY)
    {   hostcontroller[1] = CONTROLLER_NONE;
    }
    if (!unit[1] && hostcontroller[0] == CONTROLLER_2NDDJOY)
    {   hostcontroller[0] = CONTROLLER_NONE;
    }
    if (!unit[1] && hostcontroller[1] == CONTROLLER_2NDDJOY)
    {   hostcontroller[1] = CONTROLLER_NONE;
    }

    if (showmenubars[wsm])
    {   if (joyname[0][0])
        {   ModifyMenu( MenuPtr, ID_LEFT_1STDJOY,  MF_BYCOMMAND | MF_STRING, ID_LEFT_1STDJOY,  joyname[0]);
            ModifyMenu( MenuPtr, ID_RIGHT_1STDJOY, MF_BYCOMMAND | MF_STRING, ID_RIGHT_1STDJOY, joyname[0]);
        } else
        {   ModifyMenu( MenuPtr, ID_LEFT_1STDJOY,  MF_BYCOMMAND | MF_STRING, ID_LEFT_1STDJOY,  LLL(MSG_1STPAD, "&1st gamepad"));
            ModifyMenu( MenuPtr, ID_RIGHT_1STDJOY, MF_BYCOMMAND | MF_STRING, ID_RIGHT_1STDJOY, LLL(MSG_1STPAD, "&1st gamepad"));
        }
        if (joyname[1][0])
        {   ModifyMenu( MenuPtr, ID_LEFT_2NDDJOY,  MF_BYCOMMAND | MF_STRING, ID_LEFT_2NDDJOY,  joyname[1]);
            ModifyMenu( MenuPtr, ID_RIGHT_2NDDJOY, MF_BYCOMMAND | MF_STRING, ID_RIGHT_2NDDJOY, joyname[1]);
        } else
        {   ModifyMenu( MenuPtr, ID_LEFT_2NDDJOY,  MF_BYCOMMAND | MF_STRING, ID_LEFT_2NDDJOY,  LLL(MSG_2NDPAD, "&2nd gamepad"));
            ModifyMenu( MenuPtr, ID_RIGHT_2NDDJOY, MF_BYCOMMAND | MF_STRING, ID_RIGHT_2NDDJOY, LLL(MSG_2NDPAD, "&2nd gamepad"));
    }   }

    if (joyname[0][0])
    {   ModifyMenu(PopUpMenuPtr, ID_LEFT_1STDJOY,  MF_BYCOMMAND | MF_STRING, ID_LEFT_1STDJOY,  joyname[0]);
        ModifyMenu(PopUpMenuPtr, ID_RIGHT_1STDJOY, MF_BYCOMMAND | MF_STRING, ID_RIGHT_1STDJOY, joyname[0]);
    } else
    {   ModifyMenu(PopUpMenuPtr, ID_LEFT_1STDJOY,  MF_BYCOMMAND | MF_STRING, ID_LEFT_1STDJOY,  LLL(MSG_1STPAD, "&1st gamepad"));
        ModifyMenu(PopUpMenuPtr, ID_RIGHT_1STDJOY, MF_BYCOMMAND | MF_STRING, ID_RIGHT_1STDJOY, LLL(MSG_1STPAD, "&1st gamepad"));
    }
    if (joyname[1][0])
    {   ModifyMenu(PopUpMenuPtr, ID_LEFT_2NDDJOY,  MF_BYCOMMAND | MF_STRING, ID_LEFT_2NDDJOY,  joyname[1]);
        ModifyMenu(PopUpMenuPtr, ID_RIGHT_2NDDJOY, MF_BYCOMMAND | MF_STRING, ID_RIGHT_2NDDJOY, joyname[1]);
    } else
    {   ModifyMenu(PopUpMenuPtr, ID_LEFT_2NDDJOY,  MF_BYCOMMAND | MF_STRING, ID_LEFT_2NDDJOY,  LLL(MSG_2NDPAD, "&2nd gamepad"));
        ModifyMenu(PopUpMenuPtr, ID_RIGHT_2NDDJOY, MF_BYCOMMAND | MF_STRING, ID_RIGHT_2NDDJOY, LLL(MSG_2NDPAD, "&2nd gamepad"));
    }

    updatemenus(); // eg. for correct ghosting
}

EXPORT void add_recent(void)
{   TRANSIENT int          found,
                           i;
    PERSIST   MENUITEMINFO nmi;

    SHAddToRecentDocs(SHARD_PATH, fn_game);

    found = -1;
    if (recents)
    {   for (i = 0; i < recents; i++)
        {   if (!stricmp(fn_game, &recent[i][3]))
            {   found = i;
                break;
    }   }   }

    if (found == 0) // game is already at top of list
    {   return;
    } // implied else

    if (recents >= 1)
    {   if (MenuPtr)
        {   RemoveMenu(         MenuPtr, ID_FILE_RECENTSEPARATOR, MF_BYCOMMAND);
        }
        if (PopUpMenuPtr)
        {   RemoveMenu(    PopUpMenuPtr, ID_FILE_RECENTSEPARATOR, MF_BYCOMMAND);
        }
        for (i = 1; i <= recents; i++)
        {   if (MenuPtr)
            {   RemoveMenu(     MenuPtr, ID_FILE_RECENT1 - 1 + i, MF_BYCOMMAND);
                DeleteObject(smlbitmap[0][i - 1]);
                // smlbitmap[0][i - 1] = NULL;
            }
            if (PopUpMenuPtr)
            {   RemoveMenu(PopUpMenuPtr, ID_FILE_RECENT1 - 1 + i, MF_BYCOMMAND);
                DeleteObject(smlbitmap[1][i - 1]);
                // smlbitmap[0][i - 1] = NULL;
    }   }   }

    if (found >= 1 && found <= 6) // item is further down the list, remove it
    {   for (i = found; i <= 6; i++)
        {   strcpy(&recent[i][3], &recent[i + 1][3]);
            recentmemmap[i] = recentmemmap[i + 1];
            recentgame[  i] = recentgame[  i + 1];
    }   }
    elif (found == -1 && recents < 8)
    {   recents++;
    }

    // add item to top of list
    strcpy(&recent[7][3], &recent[6][3]);
    strcpy(&recent[6][3], &recent[5][3]);
    strcpy(&recent[5][3], &recent[4][3]);
    strcpy(&recent[4][3], &recent[3][3]);
    strcpy(&recent[3][3], &recent[2][3]);
    strcpy(&recent[2][3], &recent[1][3]);
    strcpy(&recent[1][3], &recent[0][3]);
    strcpy(&recent[0][3], fn_game);
    recentmemmap[7]      = recentmemmap[6];
    recentmemmap[6]      = recentmemmap[5];
    recentmemmap[5]      = recentmemmap[4];
    recentmemmap[4]      = recentmemmap[3];
    recentmemmap[3]      = recentmemmap[2];
    recentmemmap[2]      = recentmemmap[1];
    recentmemmap[1]      = recentmemmap[0];
    recentmemmap[0]      = memmap;
    recentgame[7]        = recentgame[6];
    recentgame[6]        = recentgame[5];
    recentgame[5]        = recentgame[4];
    recentgame[4]        = recentgame[3];
    recentgame[3]        = recentgame[2];
    recentgame[2]        = recentgame[1];
    recentgame[1]        = recentgame[0];
    recentgame[0]        = whichgame;

    if (     MenuPtr) do_recents(     MenuPtr);
    if (PopUpMenuPtr) do_recents(PopUpMenuPtr);
}

EXPORT void do_recents(HMENU themenu)
{   TRANSIENT int          i;
    TRANSIENT ICONINFO     iconInfo;
    TRANSIENT HICON        localicon;
    PERSIST   MENUITEMINFO nmi;

    if (recents < 1)
    {   return;
    }

    nmi.cbSize        = sizeof(MENUITEMINFO);
    nmi.fMask         = MIIM_CHECKMARKS | MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_SUBMENU | MIIM_TYPE;
    nmi.fType         = MFT_SEPARATOR;
    nmi.fState        = MFS_ENABLED;
    nmi.wID           = ID_FILE_RECENTSEPARATOR;
    nmi.hSubMenu      = NULL;
    nmi.hbmpChecked   = NULL;
    nmi.hbmpUnchecked = NULL;
    nmi.dwItemData    = 0;
    InsertMenuItem(themenu, ID_FILE_ICONIFY, FALSE, &nmi);

    for (i = 1; i <= recents; i++)
    {   localicon = LoadImage(InstancePtr, MAKEINTRESOURCE(getsmallimage1(recentgame[i - 1], recentmemmap[i - 1])), IMAGE_ICON, 16, 16, 0);
        if (GetIconInfo(localicon, &iconInfo))
        {   smlbitmap[(themenu == MenuPtr) ? 0 : 1][i - 1] = iconInfo.hbmColor;
            DeleteObject(iconInfo.hbmMask);
        }
        DestroyIcon(localicon);
        // localicon = NULL;
        nmi.cbSize        = sizeof(MENUITEMINFO);
        nmi.fMask         = MIIM_CHECKMARKS | MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_SUBMENU | MIIM_TYPE;
        nmi.fType         = MFT_STRING;
        nmi.fState        = MFS_ENABLED;
        nmi.wID           = ID_FILE_RECENT1 + i - 1;
        nmi.hSubMenu      = NULL;
        nmi.hbmpChecked   = NULL;
        nmi.hbmpUnchecked = smlbitmap[(themenu == MenuPtr) ? 0 : 1][i - 1];
        nmi.dwItemData    = 0;
        nmi.dwTypeData    = recent[i - 1];
        InsertMenuItem(themenu, ID_FILE_RECENTSEPARATOR, FALSE, &nmi);
}   }

EXPORT void cls(void)
{   consolestring[0] = EOS;
    outputpos = 0;
    zprintf(TEXTPEN_CLIOUTPUT, "");
}

EXPORT void hexify(int y, UBYTE thebyte)
{   TEXT tempstring[2 + 1];

    hex1(tempstring, thebyte);
    DISCARD SetDlgItemText(SubWindowPtr[SUBWINDOW_SPRITES], IDC_SPRITEHEX_0 + y, tempstring);
    if (sprad[y][0] == '-' && sprad[y][1] == EOS)
    {   EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_SPRITES], IDC_SPRITEHEX_0 + y), FALSE);
    } else
    {   EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_SPRITES], IDC_SPRITEHEX_0 + y), TRUE);
}   }

EXPORT void dospraddress(int y, STRPTR thetext)
{   DISCARD SetDlgItemText(SubWindowPtr[SUBWINDOW_SPRITES], IDC_SPRITEADDR_1 + y, thetext);
    strcpy(sprad[y], thetext);
}

EXPORT void dogamename(void)
{   if (!showstatusbars[wsm] || !MainWindowPtr)
    {   return;
    }

    if (usermsg[0])
    {   SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM) usermsg);
    } elif (machine == PONG)
    {   SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM) pongvarname[pong_variant]);
    } else
    {   SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM) autotext[AUTOLINE_GAMENAME]);
}   }

EXPORT void blanker_off(void)
{   BOOL pvParam;

    SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &pvParam, 0);
    if (pvParam == TRUE)
    {   SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, 0, SPIF_SENDWININICHANGE);
}   }

EXPORT void blanker_on(void)
{   BOOL pvParam;

    SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &pvParam, 0);
    if (pvParam == FALSE)
    {   SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, 0, SPIF_SENDWININICHANGE);
}   }

EXPORT void ActAsServer_IPv6(unsigned short Port)
{   ADDRINFO            Hints;
    ADDRINFO           *AddrInfo,
                       *AI;
    TEXT                netportstr[5 + 1];
    int                 FromLength,
                        i,
                        NumSocks;
    SOCKET              LSocket[FD_SETSIZE];
    struct sockaddr_storage From;
    struct sockaddr_in* sockaddr_ipv4;
    LPSOCKADDR          sockaddr_ip;
    char                ipstringbuffer[46];
    DWORD               ipbufferlength = 46;
    unsigned long       J;

    FromLength = sizeof(From);

    NetClose();

    // Look up server address
    memset(&Hints, 0, sizeof(Hints));
    Hints.ai_family   = PF_UNSPEC;
    Hints.ai_socktype = SOCK_STREAM;
    Hints.ai_flags    = AI_NUMERICHOST;
    sprintf(netportstr, "%d", (int) Port);
    if ((getaddrinfo(NULL, netportstr, &Hints, &AddrInfo)))
    {   zprintf
        (   TEXTPEN_NET,
            (STRPTR)LLL(
                MSG_NET_NOGETHOSTNAME,
                "Can't get hostname!\n"
        )   );
        return;
    }

    // For each address getaddrinfo returned, we create a new socket,
    // bind that address to it, and create a queue to listen on.
    for (i = 0, AI = AddrInfo; AI != NULL; AI = AI->ai_next)
    {
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Socket type:    ");
        if (AI->ai_socktype == SOCK_STREAM)
        {   zprintf(TEXTPEN_VERBOSE, "Stream\n");
        } else
        {   zprintf(TEXTPEN_VERBOSE, "%d\n", AI->ai_socktype);
        }
        zprintf(TEXTPEN_VERBOSE, "Protocol:       ");
        switch (AI->ai_protocol)
		{
        case IPPROTO_IP:
            zprintf(TEXTPEN_VERBOSE, "IP\n");
        acase IPPROTO_IPV4:
            zprintf(TEXTPEN_VERBOSE, "IPv4\n");
        acase IPPROTO_IPV6:
            zprintf(TEXTPEN_VERBOSE, "IPv6\n");
        adefault:
            zprintf(TEXTPEN_VERBOSE, "%d\n", AI->ai_protocol);
        }
        zprintf(TEXTPEN_VERBOSE, "Flags:          %d\n", AI->ai_flags);     //  4 = AI_NUMERICHOST
        zprintf(TEXTPEN_VERBOSE, "Address length: %d\n", AI->ai_addrlen);   // 16
#endif

        if (i == FD_SETSIZE)
        {
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "Too many addresses!\n");
#endif
            break;
        }

        switch (AI->ai_family)
        {
        case PF_INET:
            sockaddr_ipv4 = (struct sockaddr_in*) AI->ai_addr;
            strcpy(hostname, Inet_NtoA(sockaddr_ipv4->sin_addr));
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "IPv4 address:   %s\n", hostname);
#endif
        acase PF_INET6:
            // We use WSAAddressToString since it is supported on Windows XP and later
            sockaddr_ip = (LPSOCKADDR) AI->ai_addr;
            // The buffer length is changed by each call to WSAAddresstoString
            // So we need to set it for each iteration through the loop for safety
            ipbufferlength = 46;
            if (WSAAddressToString
            (   sockaddr_ip,
                (DWORD) AI->ai_addrlen,
                NULL,
                ipstringbuffer,
                &ipbufferlength
            ))
            {   ;
#ifdef VERBOSE
                zprintf(TEXTPEN_VERBOSE, "Can't get IPv6 address (error code %d)!\n", WSAGetLastError());
#endif
            } else
            {   strcpy(hostname, ipstringbuffer);
#ifdef VERBOSE
                zprintf(TEXTPEN_VERBOSE, "IPv6 address:   %s\n", hostname);
#endif
            }
        adefault:
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "Unsupported family (must be IPv4 or IPv6)!\n");
#endif
            continue;
        }

        zprintf
        (   TEXTPEN_NET,
            LLL(
                MSG_NET_ACTINGASSERVER,
                "Acting as server at %s on port %d...\n"
            ),
            hostname,
            (int) netport
        );
        // Open a socket with the correct address family for this address
        LSocket[i] = socket(AI->ai_family, AI->ai_socktype, AI->ai_protocol);
        if (LSocket[i] == INVALID_SOCKET)
        {   zprintf
            (   TEXTPEN_NET,
                LLL(
                    MSG_NET_NOCREATESERVER,
                    "Can't create server socket!\n"
            )   );
            continue;
        }
        // Bind listening socket
        if (bind(LSocket[i], AI->ai_addr, AI->ai_addrlen) == SOCKET_ERROR)
        {   CloseSocket(LSocket[i]);
            LSocket[i] = INVALID_SOCKET;
            zprintf
            (   TEXTPEN_NET,
                LLL(
                    MSG_NET_NOBINDSOCKET,
                    "Can't bind server socket!\n"
            )   );
            continue;
        }
        // Make listening socket non-blocking
        J = 1;
        if (IoctlSocket((unsigned int) LSocket[i], (long) FIONBIO, &J) < 0)
        {   DISCARD CloseSocket((unsigned int) LSocket);
            LSocket[i] = INVALID_SOCKET;
            zprintf
            (   TEXTPEN_NET,
                LLL(
                    MSG_NET_NOUNBLOCKSOCKET,
                    "Can't unblock server socket!\n"
            )   );
            return;
        }
        if (listen(LSocket[i], 1) == SOCKET_ERROR)
        {   CloseSocket(LSocket[i]);
            LSocket[i] = INVALID_SOCKET;
            zprintf
            (   TEXTPEN_NET,
                LLL(
                    MSG_NET_NOLISTEN,
                    "Can't listen!\n"
            )   );
            continue;
        }

        i++;
    }

    freeaddrinfo(AddrInfo);

    if (i == 0)
    {   zprintf(TEXTPEN_NET, "Can't serve on any address!\n");
        return;
    }
    NumSocks = i;

    connected = NET_LISTENING;
    settitle();
    zprintf
    (   TEXTPEN_NET,
        LLL(
            MSG_NET_LISTENING,
            "Listening...\n"
    )   );

    lockgui();
    aborting = FALSE;
    if (listen_open()) // always returns TRUE on IBM anyway
    {   do
        {   // Process all messages
            clearmsgs();
            for (i = 0; i < NumSocks; i++)
            {   if (LSocket[i] != INVALID_SOCKET)
                {   Socket = accept(LSocket[i], (LPSOCKADDR) &From, &FromLength);
                    if (Socket >= 0)
                    {   break; // important!
        }   }   }   }
        while (Socket < 0 && !aborting);
    }

    listen_close();
    connected = NET_OFF;

    for (i = 0; i < NumSocks; i++)
    {   if (LSocket[i] != INVALID_SOCKET)
        {   DISCARD CloseSocket((unsigned int) LSocket[i]);
            // LSocket[i] = INVALID_SOCKET;
    }   }

    clearkybd();
    unlockgui();

    // Client failed to connect
    if (Socket < 0)
    {   zprintf
        (   TEXTPEN_NET,
            LLL(
                MSG_NET_ABORTED,
                "Aborted by user!\n"
        )   );
        return;
    }

    connected = NET_SERVER;

    if (getnameinfo((LPSOCKADDR) &From, FromLength, hostname, sizeof(hostname), NULL, 0, NI_NUMERICHOST) != 0)
    {   strcpy(hostname, "<unknown>");
    }
    zprintf
    (   TEXTPEN_NET,
        LLL
        (   MSG_NET_SERVERCONNECTED,
            "The client at %s has successfully connected to your server!\n",
        ),
        hostname
    );
}

EXPORT void ConnectAsClient_IPv6(const char* Server, unsigned short Port)
{   ADDRINFO           Hints;
    ADDRINFO          *AddrInfo,
                      *AI;
    TEXT               netportstr[5 + 1];
    unsigned long      J;

    NetClose();

    // Look up server address
    memset(&Hints, 0, sizeof(Hints));
    Hints.ai_family   = PF_UNSPEC;
    Hints.ai_socktype = SOCK_STREAM;
    sprintf(netportstr, "%d", (int) Port);
    if ((getaddrinfo(Server, netportstr, &Hints, &AddrInfo)))
    {   zprintf
        (   TEXTPEN_NET,
            (STRPTR) LLL(
                MSG_NET_NOGETHOSTNAME,
                "Can't get hostname!\n"
        )   );
        return;
    } // implied else

    // Try each address getaddrinfo returned, until we find one to which
    // we can successfully connect.
    for (AI = AddrInfo; AI != NULL; AI = AI->ai_next)
    {   // Open a socket with the correct address family for this address

        Socket = socket(AI->ai_family, SOCK_STREAM, AI->ai_protocol);
        if (Socket == INVALID_SOCKET)
        {   continue;
        } // implied else

        if (connect(Socket, AI->ai_addr, (int) AI->ai_addrlen) != SOCKET_ERROR)
        {   break; // success
        } // implied else

        DISCARD CloseSocket(Socket);
    }

    if (AI == NULL)
    {   zprintf
        (   TEXTPEN_NET,
            LLL(
                MSG_NET_NOCONNECTCLIENT,
                "Can't connect as client!\n"
        )   );
        return;
    } // implied else

    // We are done with the address info chain, so we can free it
    freeaddrinfo(AddrInfo);

    J = 0;
    if (IoctlSocket((unsigned int) Socket, (long) FIONBIO, &J) < 0)
    {   DISCARD CloseSocket((unsigned int) Socket);
        Socket = INVALID_SOCKET;
        zprintf
        (   TEXTPEN_NET,
            LLL(
                MSG_NET_NOBLOCKCLIENT,
                "Can't block client socket!\n"
        )   );
        return;
    }

    connected = NET_CLIENT;
    zprintf
    (   TEXTPEN_NET,
        LLL(
            MSG_NET_CLIENTCONNECTED,
            "Successfully connected as client!\n"
    )   );
}

EXPORT void emptyclipboard(void)
{   clip_open(TRUE);
    clip_close();
}

LRESULT CALLBACK CustomEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{   PERSIST FLAG debugctrl = FALSE;

    switch (msg)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_TAB:
            DISCARD GetWindowText(hDebugger, userinput, sizeof(userinput));
            handletab();
            DISCARD SendMessage(hDebugger, WM_SETTEXT,  0, (LPARAM) userinput);
            DISCARD SendMessage(hDebugger, EM_SETSEL ,  0, -1); // these move the cursor
            DISCARD SendMessage(hDebugger, EM_SETSEL , -1, -1); // to the end of the line
        acase VK_ESCAPE:
            userinput[0] = EOS;
            DISCARD SendMessage(hDebugger, WM_SETTEXT, 0, (LPARAM) userinput);
            debugger_exit();
        acase VK_RETURN:
			if (!GetWindowText(hDebugger, userinput, MAX_PATH + 80 + 1)) // doesn't work when started from inside Visual C 6.0SP6 (empty string always)
            {   TEXT  tempstring[80 + 1];
                DWORD rc;

                if ((rc = GetLastError()))
                {   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, rc, 0, tempstring, 80, NULL);
                    zprintf(TEXTPEN_CLIINPUT, "%s\n", tempstring);
            }   }
            if (userinput[0])
            {   addhistory();
                zprintf(TEXTPEN_CLIINPUT, ">%s\n", userinput);
                debug_command();
                userinput[0] = EOS;
                DISCARD SendMessage(hDebugger, WM_SETTEXT, 0, (LPARAM) userinput);
            } else
            {   debugger_exit();
            }
            historyline = -1;
        return 0; // indicate that we processed the message
        case VK_UP:
            if
            (   historyline < HISTORYLINES - 1
             && olduserinput[historyline + 1][0] != EOS
            )
            {   historyline++;
                strcpy(userinput, olduserinput[historyline]);
                DISCARD SendMessage(hDebugger, WM_SETTEXT,  0, (LPARAM) userinput);
                DISCARD SendMessage(hDebugger, EM_SETSEL ,  0, -1); // these move the cursor
                DISCARD SendMessage(hDebugger, EM_SETSEL , -1, -1); // to the end of the line
            }
        return 0; // indicate that we processed the message
        case VK_DOWN:
            if (historyline >= 0)
            {   historyline--;
                if (historyline >= 0)
                {   strcpy(userinput, olduserinput[historyline]);
                } else
                {   userinput[0] = EOS;
                }
                DISCARD SendMessage(hDebugger, WM_SETTEXT,  0, (LPARAM) userinput);
                DISCARD SendMessage(hDebugger, EM_SETSEL ,  0, -1); // these move the cursor
                DISCARD SendMessage(hDebugger, EM_SETSEL , -1, -1); // to the end of the line
            }
        return 0; // indicate that we processed the message
        }
        if (!incli)
        {   return 0;
        }
	acase WM_CHAR:
		if (wParam == VK_RETURN || wParam == VK_TAB)
        {   return 0; // needed to avoid beeping
		}
    acase WM_LBUTTONDOWN:
        ignorelmb = FALSE;
        debugger_enter();
	acase WM_KEYUP:
        if (!incli)
        {   return 0;
        }
        if (wParam == VK_CONTROL)
        {   debugctrl = FALSE;
    }   }

    // Pass the messages we don't process here on to the
    // original window procedure for default handling.
    return CallWindowProc(wpOldEditProc, hWnd, msg, wParam, lParam);
}

EXPORT void ghost_menu(int which, UINT outer, BOOL logic)
{   if (logic)
    {   EnableMenuItem(     MenuPtr, outer, MF_BYPOSITION | MF_ENABLED);
        EnableMenuItem(PopUpMenuPtr, outer, MF_BYPOSITION | MF_ENABLED);
    } else
    {   EnableMenuItem(     MenuPtr, outer, MF_BYPOSITION | MF_GRAYED);
        EnableMenuItem(PopUpMenuPtr, outer, MF_BYPOSITION | MF_GRAYED);
}   }

EXPORT void ghost_submenu(int which, UINT outer, UINT inner, BOOL logic)
{   HMENU MenuColumnPtr;

    MenuColumnPtr = GetSubMenu(MenuPtr, outer);
    if (logic)
    {   EnableMenuItem(MenuColumnPtr, inner, MF_BYPOSITION | MF_ENABLED);
        MenuColumnPtr = GetSubMenu(PopUpMenuPtr, outer);
        EnableMenuItem(MenuColumnPtr, inner, MF_BYPOSITION | MF_ENABLED);
    } else
    {   EnableMenuItem(MenuColumnPtr, inner, MF_BYPOSITION | MF_GRAYED);
        MenuColumnPtr = GetSubMenu(PopUpMenuPtr, outer);
        EnableMenuItem(MenuColumnPtr, inner, MF_BYPOSITION | MF_GRAYED);
}   }

// For some fucked up reason, sscanf() with %f and atof() crash on
// Windows 7 and 8.1 (at least).
EXPORT double zatof(STRPTR inputstr)
{   int    decpoint = 0,
           i, j,
           length,
           start,
           temp;
    double result   = 0.0;

    if (inputstr[0] == '-')
    {   start = 1;
    } else
    {   start = 0;
    }

    length = strlen(inputstr);
    for (i = start; i < length; i++)
    {   if (inputstr[i] == '.')
        {   decpoint = i;
            break; // for speed
    }   }

    j = 1;
    for (i = decpoint - 1; i >= start; i--)
    {   temp = inputstr[i] - '0';
        result += (double) (temp * j);
        j *= 10;
    }

    j = 10;
    for (i = decpoint + 1; i < length; i++)
    {   if (inputstr[i] < '0' || inputstr[i] > '9') // eg. CR
        {   break;
        }
        temp = inputstr[i] - '0';
        result += (double) (((double) temp) / j);
        j *= 10;
    }

    if (start == 1)
    {   result = -result;
    }

    return result;
}

EXPORT void busypointer(void)
{   SetCursor(hBusy);
}
EXPORT void normalpointer(void)
{   updatepointer(FALSE, TRUE);
}
EXPORT void do_eol(STRPTR thestring)
{   thestring[0] = CR;
    thestring[1] = LF;
    thestring[2] = EOS;
}

MODULE void dragndrop(HDROP wParam)
{   TRANSIENT WIN32_FIND_DATA FindData;
    TRANSIENT HANDLE          FindHandle;
    TRANSIENT int             length,
                              rc;
    PERSIST   TEXT            tempstring[MAX_PATH + 1];

    DISCARD DragQueryFile((HDROP) wParam, 0, tempstring, MAX_PATH);
    DragFinish((HDROP) wParam);
    length = strlen(tempstring);
    // assert(length <= MAX_PATH);
    FindHandle = FindFirstFile(tempstring, &FindData);
    if (FindHandle == INVALID_HANDLE_VALUE)
    {   return;
    }
    if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {   DISCARD FindClose(FindHandle);
        strcpy(path_games, tempstring);
        audit(FALSE);
        setselection();
        return;
    }
    DISCARD FindClose(FindHandle);

    if (crippled)
    {   say("You can only do this between frames!");
        return;
    }

    strcpy(fn_game, tempstring);
    rc = engine_load(FALSE);
    if (rc == 1 || rc == 3)
    {   break_pathname(fn_game, path_games, file_game);
        add_recent();
    }
    audit(FALSE);
    if (incli)
    {   debugger_enter();
}   }

EXPORT ULONG thetime(void)
{   FAST LARGE_INTEGER perffreq,
                       perftime;

    QueryPerformanceFrequency(&perffreq);
    QueryPerformanceCounter(&perftime);
    perftime.QuadPart *= ONE_MILLION;
    perftime.QuadPart /= perffreq.QuadPart;

    return (ULONG) (perftime.LowPart);
}

EXPORT void drawitem(LPDRAWITEMSTRUCT lpDIS, FLAG pushed)
{   int    centrex, centrey,
           totalwidth, totalheight;
    FLAG   multiline = FALSE;
    ULONG  stateflag;
    RECT   localrect = lpDIS->rcItem;
    SIZE   localsize;
    HICON  localicon;
    HDC    hdc       = lpDIS->hDC;
    STRPTR stringptr1,
           stringptr2;

    switch (lpDIS->CtlID)
    {
    case IDOK:
        stringptr1 = LLL(MSG_OK             , "OK"     ); // no ampersand! otherwise text size is calculated wrongly
        stringptr2 = LLL(MSG_OK2            , "&OK"    );
        localicon  = icons[0];
    acase IDC_CONFIRM_YES:
        stringptr1 = LLL(MSG_ENGINE_YES     , "Yes"    ); // no ampersand! otherwise text size is calculated wrongly
        stringptr2 = LLL(MSG_ENGINE_YES3    , "&Yes"   );
        localicon  = icons[0];
    acase IDCANCEL:
        stringptr1 = LLL(MSG_CANCEL         , "Cancel" ); // no ampersand! otherwise text size is calculated wrongly
        stringptr2 = LLL(MSG_CANCEL2        , "&Cancel");
        localicon  = icons[1];
    acase IDC_CONFIRM_NO:
        stringptr1 = LLL(MSG_ENGINE_NO      , "No"     ); // no ampersand! otherwise text size is calculated wrongly
        stringptr2 = LLL(MSG_ENGINE_NO3     , "&No"    );
        localicon  = icons[1];
    acase IDC_CLEARHS:
        stringptr1 =
        stringptr2 = LLL(MSG_CLEARDATA      , "Clear data");
        localicon  = icons[2];
    acase IDC_REWIND:
    case IDC_PT_REWIND:
    case IDC_PT_REWIND2:
        stringptr1 =
        stringptr2 = LLL(MSG_KEY_RWND_SHORT , "Rew."   );
        localicon  = icons[3];
        multiline  = TRUE;
    acase IDC_STOPTAPE:
    case IDC_PT_STOPTAPE:
    case IDC_PT_STOPTAPE2:
        stringptr1 =
        stringptr2 = LLL(MSG_KEY_STOP_SHORT , "Stop"   );
        localicon  = icons[4];
        multiline  = TRUE;
    acase IDC_RECORD:
    case IDC_PT_RECORD:
        stringptr1 =
        stringptr2 = LLL(MSG_KEY_REC_SHORT  , "Rec."   );
        localicon  = icons[5];
        multiline  = TRUE;
    acase IDC_PLAY:
    case IDC_PT_PLAY:
    case IDC_PT_PLAY2:
        stringptr1 =
        stringptr2 = LLL(MSG_KEY_PLAY_SHORT , "Play"   );
        localicon  = icons[6];
        multiline  = TRUE;
    acase IDC_FFWD:
    case IDC_PT_FFWD:
    case IDC_PT_FFWD2:
        stringptr1 =
        stringptr2 = LLL(MSG_KEY_FFWD_SHORT , "FFwd"   );
        localicon  = icons[7];
        multiline  = TRUE;
    acase IDC_EJECTTAPE:
    case IDC_PT_EJECTTAPE:
    case IDC_PT_EJECTTAPE2:
        stringptr1 =
        stringptr2 = LLL(MSG_KEY_EJECT_SHORT, "Eject"  );
        localicon  = icons[8];
        multiline  = TRUE;
    acase IDC_PRTSAVEBMP:
        stringptr1 =
        stringptr2 = LLL(MSG_HAIL_SAVEBMP   , "Save graphics as BMP...");
        localicon  = icons[9];
        multiline  = FALSE;
    acase IDC_PRTCOPYSCRN:
        stringptr1 =
        stringptr2 = LLL(MSG_EDIT_COPY      , "Copy graphics");
        localicon  = icons[10];
        multiline  = FALSE;
    acase IDC_CREATEDISK:
    case IDC_CREATEDISK2:
        stringptr1 =
        stringptr2 = LLL(MSG_CREATEDISK     , "Create disk");
        localicon  = icons[11];
        multiline  = FALSE;
    acase IDC_INSERTDISK:
    case IDC_INSERTDISK2:
        stringptr1 =
        stringptr2 = LLL(MSG_INSERTDISK     , "Insert disk");
        localicon  = icons[12];
        multiline  = FALSE;
    acase IDC_UPDATEDISK:
    case IDC_UPDATEDISK2:
        stringptr1 =
        stringptr2 = LLL(MSG_UPDATEDISK     , "Update disk");
        localicon  = icons[13];
        multiline  = FALSE;
    acase IDC_COPYTEXT:
        stringptr1 =
        stringptr2 = LLL(MSG_EDIT_COPYTEXT  , "Copy all text");
        localicon  = icons[14];
        multiline  = FALSE;
    }

    centrex = (localrect.right  - localrect.left + 1) / 2;
    centrey = (localrect.bottom - localrect.top  + 1) / 2;
    if ((lpDIS->itemState & ODS_SELECTED) || pushed)
	{   DrawFrameControl(hdc, &localrect, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_PUSHED);
        centrex++;
        centrey++;
    } else
    {   DrawFrameControl(hdc, &localrect, DFC_BUTTON, DFCS_BUTTONPUSH);
    }
    if (lpDIS->itemState & ODS_DISABLED)
    {   stateflag = DSS_DISABLED;
    } else
    {   stateflag = DSS_NORMAL;
	}

    DISCARD GetTextExtentPoint32
    (   hdc,                // handle to device context
        stringptr1,         // pointer to text string
        strlen(stringptr1), // number of characters in string
        &localsize          // pointer to structure for string size
    );

    SetBkColor(hdc, GetSysColor(COLOR_3DFACE)); // this helps for
    // the text background but the image background is still white

    if (multiline)
    {   totalwidth  =  localsize.cx;
        totalheight =  localsize.cy + 4 + 16;

        DrawState
        (   hdc,
            NULL,
            NULL,
            (LPARAM) localicon,
            (WPARAM) NULL,
            centrex - (         16 / 2),
            centrey - (totalheight / 2),
            16, 16,
            DST_ICON | stateflag
        );
        DrawState
        (   hdc,
            NULL,
            NULL,
            (LPARAM) stringptr2,
            (WPARAM) NULL,
            centrex - (totalwidth  / 2),
            centrey - (totalheight / 2) + localsize.cy + 4,
            localsize.cx,
            localsize.cy,
            DST_PREFIXTEXT | stateflag
        );
    } else
    {   totalwidth  =  localsize.cx + 4 + 16;
        totalheight = (localsize.cy > 16) ? localsize.cy : 16;

        DrawState
        (   hdc,
            NULL,
            NULL,
            (LPARAM) localicon,
            (WPARAM) NULL,
            centrex - (totalwidth  / 2),
            centrey - (totalheight / 2) + ((totalheight - 16) / 2),
            16, 16,
            DST_ICON | stateflag
        );
        DrawState
        (   hdc,
            NULL,
            NULL,
            (LPARAM) stringptr2,
            (WPARAM) NULL,
            centrex - (totalwidth  / 2) + 16 + 4,
            centrey - (totalheight / 2) + ((totalheight - localsize.cy) / 2),
            localsize.cx,
            localsize.cy,
            DST_PREFIXTEXT | stateflag
        );
    }

    if (!(lpDIS->itemState & ODS_DISABLED) && (lpDIS->itemState & ODS_FOCUS))
	{   localrect.left   += 3;
        localrect.right  -= 3;
        localrect.top    += 3;
        localrect.bottom -= 3;
        DrawFocusRect(hdc, &localrect);
}   }

EXPORT void thesleep(int microseconds)
{   Sleep(microseconds / 1000);
}

EXPORT void debugger_enter(void)
{   // assert(hDebugger);

    incli = TRUE;
    DISCARD SetFocus(hDebugger);
}
EXPORT void debugger_exit(void)
{   // assert(hDebugger);

    incli = FALSE;
    SetFocus(MainWindowPtr); // important!
    DISCARD RedrawWindow(hDebugger, NULL, NULL, RDW_INVALIDATE);
    setselection();
}

EXPORT void fix_paths(void)
{   int length;

    // Removes trailing slashes from paths (except roots, eg. C:\ ).

    if (path_disks[1] == ':' && path_disks[2] == '\\' && path_disks[3] == EOS)
    {   ; // C:\ or similar, which is OK
    } else
    {   length = strlen(path_disks);
        if (length >= 1 && path_disks[length - 1] == '\\')
        {   path_disks[length - 1] = EOS; // remove trailing backslash
    }   }

    if (path_games[1] == ':' && path_games[2] == '\\' && path_games[3] == EOS)
    {   ; // C:\ or similar, which is OK
    } else
    {   length = strlen(path_games);
        if (length >= 1 && path_games[length - 1] == '\\')
        {   path_games[length - 1] = EOS; // remove trailing backslash
    }   }

    if (path_projects[1] == ':' && path_projects[2] == '\\' && path_projects[3] == EOS)
    {   ; // C:\ or similar, which is OK
    } else
    {   length = strlen(path_projects);
        if (length >= 1 && path_projects[length - 1] == '\\')
        {   path_projects[length - 1] = EOS; // remove trailing backslash
    }   }

    if (path_screenshots[1] == ':' && path_screenshots[2] == '\\' && path_screenshots[3] == EOS)
    {   ; // C:\ or similar, which is OK
    } else
    {   length = strlen(path_screenshots);
        if (length >= 1 && path_screenshots[length - 1] == '\\')
        {   path_screenshots[length - 1] = EOS; // remove trailing backslash
    }   }

    if (path_scripts[1] == ':' && path_scripts[2] == '\\' && path_scripts[3] == EOS)
    {   ; // C:\ or similar, which is OK
    } else
    {   length = strlen(path_scripts);
        if (length >= 1 && path_scripts[length - 1] == '\\')
        {   path_scripts[length - 1] = EOS; // remove trailing backslash
    }   }

    if (path_tapes[1] == ':' && path_tapes[2] == '\\' && path_tapes[3] == EOS)
    {   ; // C:\ or similar, which is OK
    } else
    {   length = strlen(path_tapes);
        if (length >= 1 && path_tapes[length - 1] == '\\')
        {   path_tapes[length - 1] = EOS; // remove trailing backslash
}   }   }

EXPORT void macro_rexx(FLAG wantasl, FLAG startup)
{   TRANSIENT APIRET    rc1;
    TRANSIENT SWORD     rc2;
    TRANSIENT RXSTRING  rc3;
    TRANSIENT RXSYSEXIT rxexits[2];
    PERSIST   TEXT      rc3buffer[256 + 1];

    if (wantasl && asl
    (   LLL(MSG_HAIL_AREXX, "Run REXX Script"),
        "*.RX|*.REX|*.REXX",
        FALSE,
        path_scripts,
        fn_script, // this fortunately doesn't contain the path right now
        fn_script
    ) != EXIT_SUCCESS)
    {   return;
    }
#ifdef SETPATHS
    if (!startup)
    {   break_pathname(fn_script, path_scripts, NULL);
    }
#endif

    zprintf(TEXTPEN_REXX, "Running REXX script \"%s\"...\n", fn_script);

    rc3.strptr              = rc3buffer;
    rc3.strlength           = sizeof(rc3buffer);
    rxexits[0].sysexit_name = "RXSIO";
    rxexits[0].sysexit_code = RXSIO;
    rxexits[1].sysexit_name = NULL;
    rxexits[1].sysexit_code = RXENDLST;

    rc1 = RexxStart
    (   0,            // LONG       ArgCount
        NULL,         // PRXSTRING  ArgList
        fn_script,    // PCSZ       ProgramName
        NULL,         // PRXSTRING  Instore
        "WINARCADIA", // PCSZ       EnvName
        RXSUBROUTINE, // LONG       CallType
        rxexits,      // PRXSYSEXIT Exits
        &rc2,         // PSHORT     ReturnCode
        &rc3          // PRXSTRING  Result
    );
    if (rc1 == RX_START_OK)
    {   zprintf(TEXTPEN_REXX, "REXX script returned %d", (int) rc2);
        if (rc3.strptr) // because it can be NULL
        {   zprintf(TEXTPEN_REXX, " and %s", rc3.strptr);
        }
        zprintf(TEXTPEN_REXX, ".\n\n");
    } else
    {   zprintf(TEXTPEN_REXX, "REXX script failed (error %d)!\n\n", rc1);
    }

    updatemenu(MENUITEM_REPEATREXX);
}

// REXX FUNCTIONS---------------------------------------------------------

APIRET APIENTRY rexx_clear(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (crippled)
    {   errno = 10;
        error = "CLEAR is unavailable during the frame\n";
    } else
    {   project_reset(FALSE);
    }

    // CLEAR doesn't return anything (unlike NEW)

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_client(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if
    (   !crippled
     && (   machine == ARCADIA
         || machine != INTERTON
         || machine != ELEKTOR
    )   )
    {   if (numargs >= 1)
        {   DISCARD strcpy(hostname, args[0].strptr);
            skiphostname = TRUE;
            docommand(MENUITEM_CLIENT);
            skiphostname = FALSE;
        } else
        {   docommand(MENUITEM_CLIENT);
    }   }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_closemonitor(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   close_subwindow(SUBWINDOW_MONITOR_CPU);
    close_subwindow(SUBWINDOW_MONITOR_XVI);
    close_subwindow(SUBWINDOW_MONITOR_PSGS);

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_cmdshell(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (numargs >= 1)
    {   if (!stricmp(args[0].strptr, "OPEN"))
        {   showdebugger[wsm] = TRUE;
            docommand(MENUITEM_VIEWDEBUGGER);
        } elif (!stricmp(args[0].strptr, "CLOSE"))
        {   showdebugger[wsm] = FALSE;
            docommand(MENUITEM_VIEWDEBUGGER);
    }   }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_debugger(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (numargs >= 1 && !stricmp(args[0].strptr, "QUIET"))
    {   quiet = TRUE;
        if (numargs >= 8) strcpy(arg7, args[7].strptr);
        if (numargs >= 7) strcpy(arg6, args[6].strptr);
        if (numargs >= 6) strcpy(arg5, args[5].strptr);
        if (numargs >= 5) strcpy(arg4, args[4].strptr);
        if (numargs >= 4) strcpy(arg3, args[3].strptr);
        if (numargs >= 3) strcpy(arg2, args[2].strptr);
        if (numargs >= 2) strcpy(arg1, args[1].strptr); else arg1[0] = EOS;
    } else
    {   if (numargs >= 7) strcpy(arg7, args[6].strptr);
        if (numargs >= 6) strcpy(arg6, args[5].strptr);
        if (numargs >= 5) strcpy(arg5, args[4].strptr);
        if (numargs >= 4) strcpy(arg4, args[3].strptr);
        if (numargs >= 3) strcpy(arg3, args[2].strptr);
        if (numargs >= 2) strcpy(arg2, args[1].strptr);
        if (numargs >= 1) strcpy(arg1, args[0].strptr); else arg1[0] = EOS;
    }

    if (arg1[0] != EOS)
    {   debug_command();
    }
    quiet = FALSE;

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_getattr(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (numargs >= 1)
    {   if   (!stricmp(args[0].strptr, "APPLICATION.PORT"   )) sprintf(retstr->strptr, "%d", (int) netport);
        elif (!stricmp(args[0].strptr, "APPLICATION.VERSION")) strcpy( retstr->strptr, DECIMALVERSION);
    }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_getcredits(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (whichgame >= 0 && whichgame < KNOWNGAMES)
    {   strcpy(retstr->strptr, known[whichgame].credits);
    } else
    {   strcpy(retstr->strptr, "?");
    }
    retstr->strlength = strlen(retstr->strptr);

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_gethostname(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   strcpy(retstr->strptr, hostname);
    retstr->strlength = strlen(retstr->strptr);

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_getmachine(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   strcpy(retstr->strptr, machines[machine].cli);
    retstr->strlength = strlen(retstr->strptr);

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_getmemmap(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   retstr->strptr[0] = 'A' + memmap;
    retstr->strptr[1] = EOS;
    retstr->strlength = 1;

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_getname(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (whichgame >= 0 && whichgame < KNOWNGAMES)
    {   strcpy(retstr->strptr, known[whichgame].name);
    } else
    {   strcpy(retstr->strptr, "?");
    }
    retstr->strlength = strlen(retstr->strptr);

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_getport(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   sprintf(retstr->strptr, "%d", (int) netport);
    retstr->strlength = strlen(retstr->strptr);

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_getreference(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (whichgame >= 0 && whichgame < KNOWNGAMES)
    {   strcpy(retstr->strptr, known[whichgame].reference);
    } else
    {   strcpy(retstr->strptr, "?");
    }
    retstr->strlength = strlen(retstr->strptr);

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_getversion(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (numargs >= 1)
    {   if (!stricmp(args[0].strptr, "DECIMAL"))
        {   strcpy(retstr->strptr, DECIMALVERSION);
            retstr->strlength = strlen(retstr->strptr);
        } elif (!stricmp(args[0].strptr, "INTEGER"))
        {   strcpy(retstr->strptr, INTEGERVERSION);
            retstr->strlength = strlen(retstr->strptr);
    }   }
    else
    {   strcpy(retstr->strptr, DECIMALVERSION);
        retstr->strlength = strlen(retstr->strptr);
    }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_help(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   // The maximum length of retstr->strptr is 255 characters (+ NUL terminator).
    // Therefore we zprintf() this text instead of returning it as the result string.

    zprintf(TEXTPEN_REXX,
"ACTIVATE|SHOW\n"    \
"ACTIVATEWINDOW\n"   \
"CLEAR\n"            \
"CLIENT\n"           \
"CLOSECONTROLS\n"    \
"CLOSEDIPS\n"        \
"CLOSEDISKDRIVE\n"   \
"CLOSEGAMEINFO\n"    \
"CLOSEGAMEPADS\n"    \
"CLOSEKEYBOARD\n"    \
"CLOSEMEMORY\n"      \
"CLOSEMUSIC\n"       \
"CLOSEMONITOR\n"     \
"CLOSEOPCODES\n"     \
"CLOSEPALETTE\n"     \
"CLOSESPRITES\n"     \
"CLOSETAPEDECK\n"    \
"CMDSHELL\n"         \
"COPY\n"             \
"COPYTEXT\n"         \
"DEACTIVATE|HIDE\n"  \
"DEBUGGER\n"         \
"GETATTR\n"          \
"GETCREDITS\n"       \
"GETHOSTNAME\n"      \
"GETMACHINE\n"       \
"GETMEMMAP\n"        \
"GETNAME\n"          \
"GETPORT\n"          \
"GETREFERENCE\n"     \
"GETVERSION\n"       \
"HELP\n"             \
"JUMPTOMONITOR\n"    \
"MENU\n"             \
"MOVEWINDOW\n"       \
"NEW\n"              \
"OPEN\n"             \
"OPENCONTROLS\n"     \
"OPENDIPS\n"         \
"OPENDISKDRIVE\n"    \
"OPENGAMEINFO\n"     \
"OPENGAMEPADS\n"     \
"OPENKEYBOARD\n"     \
"OPENMEMORY\n"       \
"OPENMONITOR\n"      \
"OPENMUSIC\n"        \
"OPENOPCODES\n"      \
"OPENPALETTE\n"      \
"OPENSPRITES\n"      \
"OPENTAPEDECK\n"     \
"PASTE\n"            \
"PAUSE\n"            \
"PEEK\n"             \
"QUIT\n"             \
"RELOAD|REVERT\n"    \
"SAVEACBM\n"         \
"SAVEAS\n"           \
"SAVEBMP\n"          \
"SAVEGIF\n"          \
"SAVEILBM\n"         \
"SAVEPCX\n"          \
"SAVEPNG\n"          \
"SAVETIFF\n"         \
"SERVER\n"           \
"SETMACHINE\n"       \
"SETPORT\n"          \
"SETSPEED\n"         \
"TURBO\n"            \
"UNPAUSE\n"          \
"VIEWHIGHSCORES\n"   \
"WINDOWTOBACK\n"     \
"WINDOWTOFRONT\n"    );

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_jumptomonitor(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if
    (   !crippled
     && (   machine == ELEKTOR
         || machine == PIPBUG
         || machine == BINBUG
         || machine == INSTRUCTOR
         || machine == CD2650
         || machine == PHUNSY
         || machine == SELBST
         || machine == MIKIT
    )   )
    {   docommand(MENUITEM_RESETTOMONITOR);
    }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_menu(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   int i;

    if (numargs >= 1)
    {   if (iconified)
        {   if (!stricmp(args[0].strptr, menuinfo1[MENUITEM_UNICONIFY].rexx))
            {   handle_menu(MENUITEM_UNICONIFY);
            }
            return RXFUNC_OK;
        }
        for (i = 0; i < MENUITEMS; i++)
        {   if (menuinfo1[i].rexx[0] != EOS && !stricmp(args[0].strptr, menuinfo1[i].rexx))
            {   handle_menu(i);
                return RXFUNC_OK;
        }   }
        for (i = 0; i < MENUOPTS; i++)
        {   if (menuinfo2[i].rexx[0] != EOS && !stricmp(args[0].strptr, menuinfo2[i].rexx))
            {   handle_menu2(i);
                return RXFUNC_OK;
    }   }   }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_movewindow(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   int number1, number2;

    if (!iconified && numargs >= 2)
    {   // We could do bounds checking on these input values
        // and return an error if value(s) is/are inappropriate
        if (numargs >= 4)
        {   number1 = (SLONG) atoi(args[1].strptr);
            number2 = (SLONG) atoi(args[3].strptr);
        } else
        {   number1 = (SLONG) atoi(args[0].strptr);
            number2 = (SLONG) atoi(args[1].strptr);
        }
        if (number1 == -1) number1 = winleftx;
        if (number2 == -1) number2 = wintopy;
        SetWindowPos(MainWindowPtr, 0, number1, number2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_new(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   // Our implementation of NEW doesn't support changing the portname
    if (!crippled)
    {   project_reset(FALSE);
    }

    strcpy(retstr->strptr, "WINARCADIA");
    retstr->strlength = strlen(retstr->strptr);

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_open(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (!crippled)
    {   if (numargs >= 1)
        {   macro_stop();
            strcpy(fn_game, args[0].strptr);
            DISCARD engine_load(FALSE);
        } else
        {   project_open();
    }   }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_opengameinfo(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (game && autotext[AUTOLINE_GAMENAME][0] != EOS)
    {   help_gameinfo();
    }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_openmonitor(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   view_monitor(SUBWINDOW_MONITOR_CPU);
    view_monitor(SUBWINDOW_MONITOR_XVI);
    view_monitor(SUBWINDOW_MONITOR_PSGS);

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_peek(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   int address;

    if (numargs >= 1)
    {   address = atoi(args[0].strptr);
        if (address >= 0 && address <= 32767)
        {   sprintf(args[0].strptr, "%d", memory[address]);
            retstr->strlength = strlen(retstr->strptr);
    }   }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_revert(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (!crippled)
    {   macro_stop();
        DISCARD engine_load(FALSE);
    }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_saveacbm(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (numargs >= 1)
    {   DISCARD strcpy(fn_screenshot, args[0].strptr);
        savescreenshot(KIND_ACBM, FALSE, FALSE, FALSE);
    } else
    {   savescreenshot(KIND_ACBM, TRUE, FALSE, FALSE);
    }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_saveas(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (!crippled)
    {   if (numargs >= 1)
        {   macro_stop();
            strcpy(fn_game, args[0].strptr);
            engine_save(KIND_COS, TRUE);
        } else
        {   project_save(KIND_COS);
    }   }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_savebmp(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (numargs >= 1)
    {   DISCARD strcpy(fn_screenshot, args[0].strptr);
        savescreenshot(KIND_BMP, FALSE, FALSE, FALSE);
    } else
    {   savescreenshot(KIND_BMP, TRUE, FALSE, FALSE);
    }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_savegif(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (numargs >= 1)
    {   DISCARD strcpy(fn_screenshot, args[0].strptr);
        savescreenshot(KIND_GIF, FALSE, FALSE, FALSE);
    } else
    {   savescreenshot(KIND_GIF, TRUE, FALSE, FALSE);
    }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_saveilbm(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (numargs >= 1)
    {   DISCARD strcpy(fn_screenshot, args[0].strptr);
        savescreenshot(KIND_ILBM, FALSE, FALSE, FALSE);
    } else
    {   savescreenshot(KIND_ILBM, TRUE, FALSE, FALSE);
    }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_savepcx(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (numargs >= 1)
    {   DISCARD strcpy(fn_screenshot, args[0].strptr);
        savescreenshot(KIND_PCX, FALSE, FALSE, FALSE);
    } else
    {   savescreenshot(KIND_PCX, TRUE, FALSE, FALSE);
    }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_savepng(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (numargs >= 1)
    {   DISCARD strcpy(fn_screenshot, args[0].strptr);
        savescreenshot(KIND_PNG, FALSE, FALSE, FALSE);
    } else
    {   savescreenshot(KIND_PNG, TRUE, FALSE, FALSE);
    }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_savetiff(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (numargs >= 1)
    {   DISCARD strcpy(fn_screenshot, args[0].strptr);
        savescreenshot(KIND_TIFF, FALSE, FALSE, FALSE);
    } else
    {   savescreenshot(KIND_TIFF, TRUE, FALSE, FALSE);
    }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_server(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if
    (   !crippled
     && (   machine == ARCADIA
         || machine != INTERTON
         || machine != ELEKTOR
    )   )
    {   docommand(MENUITEM_SERVER);
    }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_setmachine(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   int i;

    if (numargs >= 1)
    {   for (i = 0; i < MACHINES; i++)
        {   if (!stricmp(args[0].strptr, machines[i].cli))
            {   changemachine(i, TRUE, FALSE, TRUE, FALSE);
                break;
    }   }   }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_setport(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   int number1;

    if (numargs >= 1)
    {   number1 = atoi(args[0].strptr);
        if (number1 >= 0 && number1 <= 65535)
        {   netport = (UWORD) number1;
    }   }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_setspeed(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (numargs >= 1)
    {   if   (!strcmp(args[0].strptr,  "25")) speedup = 0;
        elif (!strcmp(args[0].strptr,  "50")) speedup = 1;
        elif (!strcmp(args[0].strptr,  "75")) speedup = 2;
        elif (!strcmp(args[0].strptr, "100")) speedup = 3;
        elif (!strcmp(args[0].strptr, "125")) speedup = 4;
        elif (!strcmp(args[0].strptr, "150")) speedup = 5;
        elif (!strcmp(args[0].strptr, "200")) speedup = 6;
        elif (!strcmp(args[0].strptr, "400")) speedup = 7;
        elif (!strcmp(args[0].strptr, "800")) speedup = 8;
        update_speed();
    }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_turbo(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr)
{   if (numargs >= 1)
    {   if (!stricmp(args[0].strptr, "ON"))
        {   turbo = TRUE;
            updatemenu(MENUITEM_TURBO);
            updatesmlgad(GADPOS_TURBO, turbo, TRUE);
        } elif (!stricmp(args[0].strptr, "OFF"))
        {   turbo = FALSE;
            updatemenu(MENUITEM_TURBO);
            updatesmlgad(GADPOS_TURBO, turbo, TRUE);
    }   }

    return RXFUNC_OK;
}

APIRET APIENTRY rexx_activate(      CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { if (iconified) uniconify(FALSE);              return RXFUNC_OK; }
APIRET APIENTRY rexx_activatewindow(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { SetActiveWindow(MainWindowPtr);               return RXFUNC_OK; }
APIRET APIENTRY rexx_closecontrols( CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { close_subwindow(SUBWINDOW_CONTROLS);          return RXFUNC_OK; }
APIRET APIENTRY rexx_closedips(     CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { close_subwindow(SUBWINDOW_DIPS);              return RXFUNC_OK; }
APIRET APIENTRY rexx_closediskdrive(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { close_subwindow(SUBWINDOW_FLOPPYDRIVE);       return RXFUNC_OK; }
APIRET APIENTRY rexx_closegameinfo( CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { close_subwindow(SUBWINDOW_GAMEINFO);          return RXFUNC_OK; }
APIRET APIENTRY rexx_closegamepads( CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { close_subwindow(SUBWINDOW_HOSTPADS);          return RXFUNC_OK; }
APIRET APIENTRY rexx_closekeyboard( CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { close_subwindow(SUBWINDOW_HOSTKYBD);          return RXFUNC_OK; }
APIRET APIENTRY rexx_closememory(   CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { close_subwindow(SUBWINDOW_MEMORY);            return RXFUNC_OK; }
APIRET APIENTRY rexx_closemusic(    CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { close_subwindow(SUBWINDOW_MUSIC);             return RXFUNC_OK; }
APIRET APIENTRY rexx_closeopcodes(  CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { close_subwindow(SUBWINDOW_OPCODES);           return RXFUNC_OK; }
APIRET APIENTRY rexx_closepalette(  CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { close_subwindow(SUBWINDOW_PALETTE);           return RXFUNC_OK; }
APIRET APIENTRY rexx_closesprites(  CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { close_subwindow(SUBWINDOW_SPRITES);           return RXFUNC_OK; }
APIRET APIENTRY rexx_closetapedeck( CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { close_subwindow(SUBWINDOW_TAPEDECK);          return RXFUNC_OK; }
APIRET APIENTRY rexx_copy(          CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { savescreenshot(KIND_BMP, FALSE, TRUE, FALSE); return RXFUNC_OK; }
APIRET APIENTRY rexx_copytext(      CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { edit_savetext(TRUE, FALSE, -1, FALSE);        return RXFUNC_OK; }
APIRET APIENTRY rexx_deactivate(    CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { if (!iconified) iconify();                    return RXFUNC_OK; }
APIRET APIENTRY rexx_opencontrols(  CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { view_controls();                              return RXFUNC_OK; }
APIRET APIENTRY rexx_opendips(      CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { edit_dips();                                  return RXFUNC_OK; }
APIRET APIENTRY rexx_opendiskdrive( CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { open_floppydrive(TRUE);                       return RXFUNC_OK; }
APIRET APIENTRY rexx_opengamepads(  CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { help_hostpads();                              return RXFUNC_OK; }
APIRET APIENTRY rexx_openkeyboard(  CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { help_hostkybd();                              return RXFUNC_OK; }
APIRET APIENTRY rexx_openmemory(    CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { edit_memory();                                return RXFUNC_OK; }
APIRET APIENTRY rexx_openmusic(     CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { tools_music();                                return RXFUNC_OK; }
APIRET APIENTRY rexx_openopcodes(   CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { help_opcodes();                               return RXFUNC_OK; }
APIRET APIENTRY rexx_openpalette(   CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { edit_palette();                               return RXFUNC_OK; }
APIRET APIENTRY rexx_opensprites(   CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { open_spriteeditor();                          return RXFUNC_OK; }
APIRET APIENTRY rexx_opentapedeck(  CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { open_tapedeck();                              return RXFUNC_OK; }
APIRET APIENTRY rexx_paste(         CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { edit_pastetext();                             return RXFUNC_OK; }
APIRET APIENTRY rexx_pause(         CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { emu_pause();                                  return RXFUNC_OK; }
APIRET APIENTRY rexx_quit(          CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { cleanexit(EXIT_SUCCESS);                      return RXFUNC_OK; }
APIRET APIENTRY rexx_unpause(       CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { emu_unpause();                                return RXFUNC_OK; }
APIRET APIENTRY rexx_viewhighscores(CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { view_hiscores();                              return RXFUNC_OK; }
APIRET APIENTRY rexx_windowtoback(  CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { SetWindowPos(MainWindowPtr, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); return RXFUNC_OK; }
APIRET APIENTRY rexx_windowtofront( CONST CHAR* name, ULONG numargs, RXSTRING args[], CONST UCHAR* queuename, RXSTRING* retstr) { SetWindowPos(MainWindowPtr, HWND_TOP,    0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); return RXFUNC_OK; }

LONG APIENTRY rexx_rxsio(LONG ExitNumber, LONG Subfunction, PEXIT ParmBlock)
{   RXSTRING* rxstring;

    if (ExitNumber != RXSIO)
    {   return RXEXIT_NOT_HANDLED;
    }

    rxstring = (RXSTRING*) ParmBlock;

    switch (Subfunction)
    {
    case RXSIOSAY:
        zprintf(TEXTPEN_REXX, "%s\n", rxstring->strptr);
    acase RXSIOTRD:
        rexxwhere = rxstring->strptr;
        rexx_input();
        rxstring->strlength = strlen(rxstring->strptr);
    adefault:
        return RXEXIT_NOT_HANDLED;
    }

    return RXEXIT_HANDLED;
}

MODULE void RebuildMenu(void) 
{   // assert(cheevos);

    if (MenuPtr)
    {   DeleteMenu(MenuPtr, MN_CHEEVOS, MF_BYPOSITION);
#ifdef LOGCHEEVOS
        zprintf(TEXTPEN_VERBOSE, "RA_CreatePopupMenu() for main menu\n");
#endif
        AppendMenu(MenuPtr, MF_POPUP | MF_STRING, (UINT_PTR) RA_CreatePopupMenu(), TEXT("&RetroAchievements"));
        DrawMenuBar(MainWindowPtr);
    }
    if (PopUpMenuPtr)
    {   DeleteMenu(PopUpMenuPtr, MN_CHEEVOS, MF_BYPOSITION);
#ifdef LOGCHEEVOS
        zprintf(TEXTPEN_VERBOSE, "RA_CreatePopupMenu() for popup menu\n");
#endif
        AppendMenu(PopUpMenuPtr, MF_POPUP | MF_STRING, (UINT_PTR) RA_CreatePopupMenu(), TEXT("&RetroAchievements"));
}   }

EXPORT void init_cheevos(void)
{   // assert(MainWindowPtr);
    // assert(cheevos);

    if (machine != ARCADIA && machine != INTERTON && machine != ELEKTOR)
    {   cheevos = FALSE;
        updatemenu(MENUITEM_CHEEVOS1);
        return;
    }

    busypointer();

#ifdef LOGCHEEVOS
    zprintf
    (   TEXTPEN_VERBOSE,
        "RA_Init()\n" \
        "RA_SetConsoleID() for machine %d\n" \
        "RA_InstallSharedFunctions()\n",
        machine
    );
#endif
    RA_Init(MainWindowPtr, 12, INTEGERVERSION); // initialize the DLL
    // We have to assume RA_Init() was successful as it has no return code
    // but it can indeed fail. :-( The user has to turn "Tools|
    // RetroAchievements?" off himself in this case.

    switch (machine)
    {
    case  ARCADIA:                 RA_SetConsoleID(73);
    acase INTERTON:                RA_SetConsoleID(74);
    acase ELEKTOR:                 RA_SetConsoleID(75);
 // acase MALZAK:   case ZACCARIA: RA_SetConsoleID(27);
    }

    // provide callbacks to the DLL
    RA_InstallSharedFunctions(NULL, CauseUnpause, CausePause, RebuildMenu, GetEstimatedGameTitle, ResetEmulator, LoadROM);

#ifdef LOGCHEEVOS
    zprintf
    (   TEXTPEN_VERBOSE,
        "RA_ClearMemoryBanks()\n" \
        "RA_InstallMemoryBank()\n"
    );
#endif
    // register the system memory
    RA_ClearMemoryBanks();
    switch (machine)
    {
    case  ARCADIA:  RA_InstallMemoryBank(0, AByteReader , AByteWriter ,  0x300); // $1800..$1AFF ( 768 bytes)
    acase INTERTON: RA_InstallMemoryBank(0, IByteReader1, IByteWriter1,  0x400); // $1800..$1BFF (1024 bytes)
                    RA_InstallMemoryBank(1, IByteReader2, IByteWriter2,  0x200); // $1E00..$1FFF ( 512 bytes)
    acase ELEKTOR:  RA_InstallMemoryBank(0, EByteReader , EByteWriter , 0x1800); // $0800..$1FFF (6144 bytes)
    }

    RebuildMenu(); // add a placeholder menu item and...
#ifdef LOGCHEEVOS
    zprintf(TEXTPEN_VERBOSE, "RA_AttemptLogin(TRUE)\n");
#endif
    RA_AttemptLogin(TRUE); // V35.8 hung once when we cancelled this (although it usually works)
    settitle();
    randomizememory = FALSE;  
    if (RA_HardcoreModeIsActive())
    {   go_hardcore();
    } /* else
    {   go_softcore();
    } */

    #ifdef LOGCHEEVOS
        zprintf(TEXTPEN_VERBOSE, "1: RA_OnLoadNewRom(memory, %d)\n", cheevosize);
    #endif
    if (machine == ELEKTOR)
    {   RA_OnLoadNewRom( cheevomem      , cheevosize);
    } elif (machine == INTERTON || cheevosize <= 4096)
    {   RA_OnLoadNewRom( memory         , cheevosize);
    } else
    {   // assert(machine == ARCADIA);
        memcpy(         &cheevomem[     0], &memory[        0],               4096); // $0000..$0FFF -> $0000..$0FFF
        if (cheevosize <= 8192)
        {   memcpy(     &cheevomem[0x1000], &memory[   0x2000],  cheevosize - 4096); // $2000..$2FFF -> $1000..$1FFF
        } else
        {   memcpy(     &cheevomem[4096], &memory[     0x2000],               4096);
            memcpy(     &cheevomem[8192], &memory[     0x4000], (cheevosize - 8192) <= 4096 ? (cheevosize - 8192) : 4096);
        }
        RA_OnLoadNewRom( cheevomem      , cheevosize);
    }

    normalpointer();
    SetActiveWindow(MainWindowPtr);
}

MODULE void GetEstimatedGameTitle(char* sNameOut)
{   const STRPTR ptr = file_game;

    if (ptr)
    {   zstrncpy(sNameOut, ptr, 64 - 1);
    } else
    {   strcpy(sNameOut, "?");
}   }

EXPORT void RA_ProcessInputs(void)
{   struct ControllerInput input;

#ifdef LOGALLCHEEVOS
    zprintf(TEXTPEN_VERBOSE, "RA_IsOverlayFullyVisible()\n");
#endif
    if (RA_IsOverlayFullyVisible())
    {   ff_ReadJoystick(0);

        input.m_bUpPressed      = (jff[0] & JOYUP   ) ? TRUE : FALSE;
        input.m_bDownPressed    = (jff[0] & JOYDOWN ) ? TRUE : FALSE;
        input.m_bLeftPressed    = (jff[0] & JOYLEFT ) ? TRUE : FALSE;
        input.m_bRightPressed   = (jff[0] & JOYRIGHT) ? TRUE : FALSE;
        input.m_bConfirmPressed = (jff[0] & JOYA    ) ? TRUE : FALSE;
        input.m_bCancelPressed  = (jff[0] & JOYB    ) ? TRUE : FALSE;
        input.m_bQuitPressed    = (jff[0] & JOYSTART) ? TRUE : FALSE;

#ifdef LOGCHEEVOS
        zprintf(TEXTPEN_VERBOSE, "RA_NavigateOverlay()\n");
#endif
        RA_NavigateOverlay(&input);
}   }

MODULE void CauseUnpause(void)
{   emu_unpause();
}
MODULE void CausePause(void)
{   emu_pause();
}

MODULE void LoadROM(const char* sFullPath) { ; }

MODULE void ResetEmulator(void)
{   project_reset(FALSE);

    // we may have just gone into hardcore mode
    if (cheevos)
    {   if (RA_HardcoreModeIsActive())
        {   go_hardcore();
        } /* else
        {   go_softcore();
        } */
}   }

EXPORT void remove_cheevos(FLAG full)
{
#ifdef LOGCHEEVOS
    zprintf(TEXTPEN_VERBOSE, "RA_Shutdown()\n");
#endif
    RA_Shutdown();

    if (MenuPtr)
    {   DeleteMenu(MenuPtr, MN_CHEEVOS, MF_BYPOSITION);
        AppendMenu(MenuPtr, MF_STRING, ID_EMULATOR_CHEEVOS, "&RetroAchievements");
        DrawMenuBar(MainWindowPtr);
    }
    if (PopUpMenuPtr)
    {   DeleteMenu(PopUpMenuPtr, MN_CHEEVOS, MF_BYPOSITION);
        AppendMenu(PopUpMenuPtr, MF_STRING, ID_EMULATOR_CHEEVOS, "&RetroAchievements");
    }

    cheevos = FALSE;

    if (!full)
    {   return;
    }

    settitle();
    updatemenus();
    updatebiggads();
    updatesmlgads();
}

MODULE void go_hardcore(void)
{   collisions            = TRUE;
    trainer_lives         =
    trainer_time          =
    trainer_invincibility = FALSE;

    if (speedup < SPEED_4QUARTERS)
    {   speedup = SPEED_4QUARTERS;
        update_speed();
    }

    macro_stop();
    close_subwindow(SUBWINDOW_MEMORY);
    close_subwindow(SUBWINDOW_SPRITES);
    close_subwindow(SUBWINDOW_TAPEDECK);
    close_subwindow(SUBWINDOW_MONITOR_CPU);
    close_subwindow(SUBWINDOW_MONITOR_PSGS);
    close_subwindow(SUBWINDOW_MONITOR_XVI);
    updatemenus();
    updatebiggads(); // unnecessary?
    updatesmlgads();

    if (showdebugger[wsm])
    {   showdebugger[wsm] = FALSE;
        docommand(MENUITEM_VIEWDEBUGGER);
    }
    showdebugger[wsm ? 0 : 1] = FALSE; // the other one
}

MODULE void go_softcore(void)
{   macro_stop(); // unnecessary?
    updatemenus(); // unnecessary?
    updatebiggads(); // unnecessary?
    updatesmlgads(); // important
}

EXPORT UBYTE AByteReader( unsigned int nOffs                    ) { return memory[         0x1800 + nOffs ]       ; }
EXPORT void  AByteWriter( unsigned int nOffs, unsigned char nVal) {        memory[         0x1800 + nOffs ] = nVal; }
EXPORT UBYTE EByteReader( unsigned int nOffs                    ) { return memory[          0x800 + nOffs ]       ; }
EXPORT void  EByteWriter( unsigned int nOffs, unsigned char nVal) {        memory[          0x800 + nOffs ] = nVal; }
EXPORT UBYTE IByteReader1(unsigned int nOffs                    ) { return memory[mirror_r[0x1800 + nOffs]]       ; }
EXPORT void  IByteWriter1(unsigned int nOffs, unsigned char nVal) {        memory[mirror_w[0x1800 + nOffs]] = nVal; }
EXPORT UBYTE IByteReader2(unsigned int nOffs                    ) { return memory[         0x1E00 + nOffs ]       ; }
EXPORT void  IByteWriter2(unsigned int nOffs, unsigned char nVal) {        memory[         0x1E00 + nOffs ] = nVal; }

MODULE void refresh_quals(void)
{   FLAG ok = FALSE;
    int  i;
    HWND activewin;

    activewin = GetForegroundWindow();
    if (activewin != MainWindowPtr)
    {   for (i = 0; i < SUBWINDOWS; i++)
        {   if (activewin == SubWindowPtr[i])
            {   ok = TRUE;
                break;
        }   }
        if (!ok)
        {   return;
    }   }

    if (GetAsyncKeyState(VK_LSHIFT  ) & 0x8000) KeyMatrix[SCAN_LSHIFT / 8] |=  (1 << (SCAN_LSHIFT % 8));
    else                                        KeyMatrix[SCAN_LSHIFT / 8] &= ~(1 << (SCAN_LSHIFT % 8));
    if (GetAsyncKeyState(VK_RSHIFT  ) & 0x8000) KeyMatrix[SCAN_RSHIFT / 8] |=  (1 << (SCAN_RSHIFT % 8));
    else                                        KeyMatrix[SCAN_RSHIFT / 8] &= ~(1 << (SCAN_RSHIFT % 8));
    if (GetAsyncKeyState(VK_LCONTROL) & 0x8000) KeyMatrix[SCAN_LCTRL  / 8] |=  (1 << (SCAN_LCTRL  % 8));
    else                                        KeyMatrix[SCAN_LCTRL  / 8] &= ~(1 << (SCAN_LCTRL  % 8));
    if (GetAsyncKeyState(VK_RCONTROL) & 0x8000) KeyMatrix[SCAN_RCTRL  / 8] |=  (1 << (SCAN_RCTRL  % 8));
    else                                        KeyMatrix[SCAN_RCTRL  / 8] &= ~(1 << (SCAN_RCTRL  % 8));
    if (GetAsyncKeyState(VK_LMENU   ) & 0x8000) KeyMatrix[SCAN_LALT   / 8] |=  (1 << (SCAN_LALT   % 8));
    else                                        KeyMatrix[SCAN_LALT   / 8] &= ~(1 << (SCAN_LALT   % 8));
    if (GetAsyncKeyState(VK_RMENU   ) & 0x8000) KeyMatrix[SCAN_RALT   / 8] |=  (1 << (SCAN_RALT   % 8));
    else                                        KeyMatrix[SCAN_RALT   / 8] &= ~(1 << (SCAN_RALT   % 8));
}

EXPORT void debugger_partial(STRPTR thecommand)
{   if (!showdebugger[wsm])
    {   showdebugger[wsm] = TRUE;
        closewindow();
        openwindow(TRUE);
    }
    strcpy(userinput, thecommand);
    debugger_enter();
    DISCARD SendMessage(hDebugger, WM_SETTEXT,  0, (LPARAM) userinput);
    DISCARD SendMessage(hDebugger, EM_SETSEL ,  0, -1); // these move the cursor
    DISCARD SendMessage(hDebugger, EM_SETSEL , -1, -1); // to the end of the line
    dogamename();
}
EXPORT void debugger_full(STRPTR thecommand)
{   strcpy(userinput, thecommand);
    addhistory();
    zprintf(TEXTPEN_CLIINPUT, ">%s\n", userinput);
    debug_command();
    dogamename();
}

EXPORT void setstatus(STRPTR thetext)
{   if (hStatusBar)
    {   SendMessage(hStatusBar, SB_SETTEXT, 1, (LPARAM) thetext);
}   }

EXPORT void resize(int newsize, FLAG force)
{   int oldwidth  = winwidth,
        oldheight = winheight;

    size = realsize = newsize;
    updatemenu(MENUFAKE_SIZE);
    calc_size();

    if (!MainWindowPtr)
    {   return;
    }

    if (force || winwidth != oldwidth || winheight != oldheight)
    {   ready = FALSE;
        sizing = TRUE;

        closewindow_3d();

        update_menuheight();
        DISCARD SetWindowPos
        (   MainWindowPtr,
            HWND_TOP,
            winleftx,
            wintopy,
            winwidth,
            winheight,
            SWP_NOCOPYBITS
        );
        make_toolbar();
        if (showsidebars[wsm])
        {   if (fullscreen)
            {   DISCARD SetWindowPos
                (   hSideBar,
                    HWND_TOP,
                    showtitlebars[wsm] ? (winwidth - sidebarwidth - leftwidth) : (winwidth - sidebarwidth + 2),
                    sidebartopy,
                    sidebarwidth,
                    sidebarheight,
                    SWP_NOCOPYBITS
                );
            } else
            {   DISCARD SetWindowPos
                (   hSideBar,
                    HWND_TOP,
                    winwidth - sidebarwidth - (leftwidth * 2) + 2,
                    sidebartopy,
                    sidebarwidth,
                    sidebarheight,
                    SWP_NOCOPYBITS
                );
            }
            DISCARD ListView_EnsureVisible(hSideBar, currentgame, TRUE); // doesn't seem to work properly for items in the rightmost column
        }
        if (showdebugger[wsm])
        {   DISCARD SetWindowPos
            (   hDebugger,
                HWND_TOP,
                promptwidth,
                debuggertop,
                debuggerwidth,
                debuggerheight,
                SWP_NOCOPYBITS
            );
        }
        if (showstatusbars[wsm])
        {   DestroyWindow(hStatusBar);
            // hStatusBar = NULL;
            make_statusbar();
        }

        openwindow_3d();
        free_display();
        make_display();
        redrawscreen();
        updatepointer(FALSE, TRUE);
        update_fps();
        clearkybd();

        ready = TRUE;
}   }

LRESULT CALLBACK MagnifierWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{   PAINTSTRUCT localps;

    switch (msg)
    {
    case WM_CREATE:
    return 0;
    case WM_DESTROY:
    return 0;
    case WM_PAINT:
        BeginPaint(hwnd, &localps);
        update_magnifier();
        EndPaint(hwnd, &localps);
    return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
