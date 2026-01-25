// INCLUDES---------------------------------------------------------------

#ifdef WIN32
    #include "ibm.h"
    typedef unsigned char bool;
    #include "RA_Interface.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef AMIGA
    #include <intuition/intuition.h>
    #define ALL_REACTION_CLASSES
    #define ALL_REACTION_MACROS
    #include <reaction/reaction.h>
    #include <proto/gadtools.h>
    #include <proto/intuition.h>
    #include <proto/locale.h>        // GetCatalogStr()
#endif

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#ifdef WIN32
    #define EXEC_TYPES_H
    #include "aa_strings.h"
    #undef EXEC_TYPES_H
#endif
#ifdef AMIGA
    #include "aa_strings.h"
    #if defined(__SASC) || defined(__VBCC__)
        #define htonl(x)   (x)
        #define htons(x)   (x)
    #else
        #include <netinet/in.h>
        #ifndef __MORPHOS__
            #include <proto/bsdsocket.h>
        #else
            #define _SYS_SOCKET_H_
        #endif
    #endif
    #ifndef __amigaos4__
        #define h_addr     h_addr_list[0] /* address, for backward compatiblity */
    #endif
    #define _IOC(inout,group,num,len) (inout | ((len & IOCPARM_MASK) << 16) | ((group) << 8) | (num))
    #define _IOW(g,n,t)               _IOC(IOC_IN, (g), (n), sizeof(t))
    #define FIONBIO                   _IOW('f', 126, long) /* set/clear non-blocking i/o */
//  #define EAGAIN                    35        /* Resource temporarily unavailable */
    #define EWOULDBLOCK               EAGAIN        /* Operation would block */
    #define WSAEWOULDBLOCK            EWOULDBLOCK
    #if defined(__SASC) || defined(__VBCC__)
        typedef unsigned long         u_long;
    #endif
    typedef u_long                    n_long; /* long as received from the net */
    #ifndef INADDR_ANY
        #define INADDR_ANY            (u_long)0x00000000
    #endif
    #define IOCPARM_MASK              0x1fff /* parameter length, at most 13 bits */
    #define IOC_IN                    0x80000000 /* copy in parameters */

    #include "amiga.h"
#endif

#ifdef WIN32
    #include <commctrl.h>
    #include "resource.h"
#endif

#include "engine1.h"
#include "games.h"
#include "labels.h"
#include "codecomments.h"
#include "datacomments.h"

// DEFINES----------------------------------------------------------------

#define INTERLEAVED
// whether to write interleaved YM files (otherwise non-interleaved)

// #define SHOWTONES
// whether to show played tones in scientific pitch notation (requires sound to be enabled)

// #define DEBUGMIDI
// whether to log MIDI events to the output window

#define KABOOMCLICKS
// whether to play the click sample instead of a very short tone
// (sounds more authentic)

// #define TESTINVERSIONS
// whether to rapidly flash the keys as an aid to adjusting the key rectangles

#define HSENTRYSIZE      (24 + MAX_USERNAMELEN + 1)
#define HISCORESLENGTH   (1 + (HSENTRYSIZE * HISCORES))
#ifdef AMIGA
    #define FN_HISCORES "PROGDIR:Configs/AmiArcadia.hgh"
#endif
#ifdef WIN32
    #define FN_HISCORES "Configs\\WinArcadia.hgh"
#endif

// INI files
#define NEWER  0
#define OLDER  1
#define OLDEST 2

// MODULE VARIABLES-------------------------------------------------------

MODULE UBYTE  HiScoresBuffer[HISCORESLENGTH],
              oldguestvolume[GUESTCHANNELS];
MODULE UWORD  oldguestpitch[GUESTCHANNELS];
MODULE ULONG  crc32_table[256];

#ifdef WIN32
    MODULE TEXT asciivoice[80 + 1]     = "";
#endif

// EXPORTED VARIABLES-----------------------------------------------------

// variables which correspond to ReAction gadgets must be ULONGs
EXPORT ULONG  analog                   = FALSE,
              arcadia_bigctrls         = TRUE,
              autoframeskip            = FALSE,
              autofire[2]              = { FALSE, FALSE },
              binbug_interface         = EA77CC4,
              collisions               = TRUE,
#ifdef RELEASING
              confirmable              = TRUE,
#else
              confirmable              = FALSE,
#endif
              declinate                = TRUE,
              demultiplex              = TRUE,
              eachpage                 = FALSE,
              elektor_bigctrls         = TRUE,
              interton_bigctrls        = TRUE,
              inverttape               = FALSE,
              keymap                   = KEYMAP_HOST,
              mikit_bigctrls           = FALSE,
              narrowprinter            = 1,
              papertapeprotect[2]      = { FALSE, FALSE },
              paused                   = FALSE,
              pong_machine             = PONGMACHINE_8550,
              region                   = REGION_PAL,
              showpalladiumkeys1       = FALSE,
              showpalladiumkeys2       = FALSE,
              si50_bigctrls            = FALSE,
              sound                    = TRUE,
              swapped                  = FALSE,
              tape_hz,
              tapewriteprotect         = FALSE,
              turbo                    = FALSE,
              verbosedisk              = FALSE,
              verbosetape              = FALSE,
              arcadia_viewcontrolsas   = 1, // means MPT-03
              pong8550_viewcontrolsas  = 1, // means Sheen
              pong8600_viewcontrolsas  = 1, // means Telejogo
              viewdiskas               = 0, // means hex
              viewkybdas               = 2, // means overlays
#ifdef WIN32
              viewkybdas2              = 0, // means desktop
#endif
#ifdef AMIGA
              viewkybdas2              = A600, // fits better on low-res screens than A4000 does
              viewpadsas2              = 0, // means CD32
#endif
              viewmemas                = 0, // means hex
              viewmemas2               = 0, // means contents
              viewpadsas               = 2, // means overlays
// other ULONGs go here
              bangfrom,
              crc64_h,
              crc64_l,
              downframes               = 4,
              frames,
              totalframes              = 16;
EXPORT TEXT   asciiname_long[259][9 + 1],
              friendly[FRIENDLYLENGTH + 1],
           // "$1234 (DRAWBALLLOWERHALF)"
           //  0123456789012345678901234
              file_game[MAX_PATH + 1],
              fn_config[MAX_PATH + 1],
              fn_game[MAX_PATH + 1]    = "", // complete pathname of the game
              fn_save[MAX_PATH + 1],
              fn_script[MAX_PATH + 1]  = "",
              fromstr[5 + 1 + 1],
              MemoryStr[MEMGADGETS][2 + 1],
              netmsg_out[80 + 1]       = "",
              olduserinput[HISTORYLINES][MAX_PATH + 80 + 1],
              path_bkgrnd[MAX_PATH + 1],
              path_disks[MAX_PATH + 1],
              path_games[MAX_PATH + 1],
              path_projects[MAX_PATH + 1],
              path_screenshots[MAX_PATH + 1],
              path_scripts[MAX_PATH + 1],
              path_tapes[MAX_PATH + 1],
              recent[8][MAX_PATH + 1],
              StatusText[4][80 + 1],
              thequeue[QUEUESIZE],
              titlebartext[MAX_PATH + TITLEBARTEXTLENGTH],
              tostr[LABELLIMIT + 1 + 1],
              usermsg[80 + 1]          = "";
EXPORT int    aifffile                 = FALSE,
              ambient                  = TRUE,
              avianims                 = FALSE,
              apnganims                = FALSE,
              autocoin                 = TRUE,
              autopause                = FALSE,
              autosave                 = TRUE,
              ax[2],
              ay[4],
              bezel                    = FALSE,
              binbug_biosver           = BINBUG_61,
              binbug_dos               = DOS_VHSDOS,
              bitrate                  = 8,
              coinop_1p,
              coinop_2p,
              coinop_coina,
              coinop_coinb,
              coinop_joy1left,
              coinop_joy1right,
              coinop_joy1up,
              coinop_joy1down,
              coinop_joy2left,
              coinop_joy2right,
              coinop_button1,
              coinop_button2,
              coinop_button3,
              coinop_button4,
              configmachine,
              confine                  = FALSE,
              connected                = NET_OFF,
              console_start            = 0,
              console_a                = 0,
              console_b                = 0,
              console_reset            = 0,
              consolebg                = 1, // white
              cpux,
              cpuy,
              crippled                 = FALSE,
              debugdrive               = 0,
              dejitter                 = FALSE,
              drawcorners              = FALSE,
              drawunlit                = TRUE,
              echoconsole              = FALSE,
              editscreen               = FALSE,
              elektor_biosver          = ELEKTOR_PHILIPS,
              emuid                    = TRUE,
              esvxfile                 = FALSE,
              erasedel                 = TRUE,
              exactspeed               = FALSE,
              filter                   = FILTER_NONE,
#ifdef WIN32
              framebased               = FALSE,
#endif
#ifdef AMIGA
              framebased               = TRUE,
#endif
              frameskip                = FRAMESKIP_MIN,
              fromnum,
              foundgames               = 0,
              fullscreen               = FALSE,
              game                     = FALSE,
              generate                 = FALSE,
              gifanims                 = FALSE,
              guestrmb                 = FALSE,
              iffanims                 = FALSE,
              inverse                  = FALSE,
              limitrefreshes           = TRUE,
              logbios                  = FALSE,
              logfile                  = LOGFILE_IGNORE,
              loginefficient           = FALSE,
              log_illegal              = FALSE,
              log_interrupts           = FALSE,
              logreadwrites            = FALSE,
              logsubroutines           = FALSE,
              logwrites                = FALSE,
              loop                     = TRUE,
              lowercase                = FALSE,
              machine                  = ARCADIA,
              memmap                   = MEMMAP_ARCADIA,
              midifile                 = FALSE,
              mnganims                 = FALSE,
              nexttod                  = 0,
              offset,
              oldcontrolkey,
              p1rumble                 = 0,
              p2rumble                 = 0,
              papertapemode[2]         = { TAPEMODE_NONE, TAPEMODE_NONE },
              pausebreak               = TRUE,
              pauselog                 = FALSE,
              phunsy_biosver           = PHUNSY_PHUNSY,
              pipbug_baudrate          = PIPBUG_BAUDRATE_110,
              pipbug_biosver           = PIPBUG_PIPBUG1BIOS,
              post                     = TRUE,
              psgfile                  = FALSE,
              queuekeystrokes          = TRUE,
              randomizememory          = FALSE,
              recentgame[8],
              recentmemmap[8],
              recents                  = 0,
              recmode                  = RECMODE_NORMAL,
              recsize,
              regionstart              = 0x1800,
              requirebutton[2]         = { FALSE, FALSE },
              retune                   = FALSE,
              rotate                   = TRUE,
              samplerate               = 11025,
              scanlines                = FALSE,
              selbst_biosver           = SELBST_BIOS20,
              sensegame                = TRUE,
              sensitivity[2]           = { SENSITIVITY_DEFAULT, SENSITIVITY_DEFAULT },
              showbuiltin              = TRUE,
              showdebugger[2]          = { TRUE, FALSE },
              showleds                 = TRUE,
              showmenubars[2]          = { TRUE, FALSE },
              showpositions            = FALSE,
              showsidebars[2]          = { TRUE, FALSE },
              showstatusbars[2]        = { TRUE, FALSE },
              showtitlebars[2]         = { TRUE, FALSE },
              showtoolbars[2]          = { TRUE, FALSE },
              showpointers[2]          = { TRUE, FALSE },
              sidebar[FOUNDGAMES_MAX],
              size                     = 1,
              smusfile                 = FALSE,
              Socket                   = INVALID_SOCKET,
              sortby                   = SORTBYNAME,
              springloaded             = FALSE,
              spritemode               = SPRITEMODE_VISIBLE,
              startupupdates           = FALSE,
              step                     = FALSE,
              style                    = STYLE_SIGNETICS1,
              supercpu                 = 0,
              tapemode                 = TAPEMODE_NONE,
              timeunit                 = TIMEUNIT_CLOCKS,
              tolimit,
              tonum,
              trace                    = FALSE,
              trainer_lives            = FALSE,
              trainer_time             = FALSE,
              trainer_invincibility    = FALSE,
           // twin_baudrate            = TWIN_BAUDRATE_110,
              undither                 = FALSE,
              useguideray              = TRUE,
              usemargins               = FALSE,
              userlabels               = 0,
              usespeech                = FALSE,
              usestubs                 = TRUE,
              verbosity                = 2,
              viewingsprite            = 0,
              viewspeedas              = VIEWAS_PERCENTS,
              watchreads               = TRUE,
              watchwrites              = WATCH_ALL,
              wavfile                  = FALSE,
              wheremouse               = -1,
              wheremusicmouse[2]       = { -2, - 2 }, // -2 means button is not down, -1 means button is down but not on a valid note
              whichgame                = -1,
              wide                     = 2,
              wsm                      = 0, // windowed
              ymfile                   = FALSE;
EXPORT UBYTE  blank,
              button[2][8]             = { { 1,2,3,4,5,6,7,8 }, { 1,2,3,4,5,6,7,8 } },
              coverage_io[258],
              glow                     = 0,
              guestvolume[GUESTCHANNELS],
              oldkeys[KEYS],
              OutputBuffer[18],
              newkeys[KEYS],
              PapertapeBuffer[2][PAPERTAPEMAX],
              tapeskewage              = 0x80,
              tone                     = 0;
EXPORT UWORD  console[4],
              guestpitch[GUESTCHANNELS],
              joystate,
              keypads[2][NUMKEYS],
              netport                  = 6666;
EXPORT FLAG   aborting                 = FALSE,
              bangercharging           = FALSE,
              guestplaying[TOTALCHANNELS],
              ignore_cout,
              inframe                  = FALSE,
              loadedconfig             = FALSE,
              modal                    = FALSE,
              need[SUBWINDOWS],
              softctrl                 = FALSE,
              softlshift               = FALSE,
              softrept                 = FALSE,
              softrshift               = FALSE;
EXPORT signed int              dx[2], dy[2];
EXPORT float                   dividend;
EXPORT struct AuditStruct      auditlist[FOUNDGAMES_MAX];
EXPORT struct EquivalentStruct symlabel[SYMLABELS_MAX + 1];
EXPORT struct IOPortStruct     ioport[258];
EXPORT struct PrinterStruct    printer[2];
EXPORT UBYTE*                  IOBuffer     = NULL;
EXPORT FILE*                   MacroHandle  = NULL;
#ifdef AMIGA
    EXPORT int                 icons        = FALSE;
#endif
#ifdef WIN32
    EXPORT TEXT                fn_bkgrnd[MAX_PATH + 1] = "";
    EXPORT int                 colourlog    = TRUE,
                               enhancestars = FALSE,
                               stretching   = STRETCHING_TOFIT;
#endif
#ifdef RELEASING
    EXPORT int                 showtod      = TRUE;
#else
    EXPORT int                 showtod      = FALSE;
#endif

EXPORT struct MachineStruct machines[MACHINES] =                                                                                      // --labels-- keys     vers   cpf pvis ppc digits                             [--code--] [--data--]                                                                                                       hiscores                                     [---------------------------------------------------------------------------------------------------player 1 keys-------------------------------------------------------------------------------------------------------------------------------------------]  [---------------------------------------------------------------------------------------------------player 2 keys--------------------------------------------------------------------------------------------------------------------------------------------]
{ { "ARCADIA"   ,                   0,                    0, MEMMAP_ARCADIA,     REGION_NTSC, { 60.1867202475031, 50.0804105952784 }, 2,   0,   29, 50, 0xFF, 38,     0.0, 0,  4, 0, { 1, 112, 254 },      0, FALSE,  -1,   -1, 355, 382, { "START" , "A"     , "B"     , "-"        }, { IMAGE_START, IMAGE_A   , IMAGE_B    , IMAGE_RESET   },  0, 51, TRUE , FALSE, 0,               0, { { "1st"  , "1"    , "2"    , "3"    , "4"    , "5"    , "6"    , "7"    , "8"    , "9"    , "Cl"   , "0"    , "En"   , "x1"   , "x2"   , "x3"   , "x4"   , "Up"   , "Dn"   , "Lt"   , "Rt"   , "2nd"  , "3rd"  , "4th"  , "UpLt" , "UpRt" , "DnLt" , "DnRt" }, { "1st"  , "1"    , "2"    , "3"    , "4"    , "5"    , "6"    , "7"    , "8"    , "9"    , "Cl"   , "0"    , "En"   , "x1"   , "x2"   , "x3"   , "x4"   , "Up"   , "Dn"   , "Lt"   , "Rt"   , "2nd"  , "3rd"  , "4th"  , "UpLt" , "UpRt" , "DnLt" , "DnRt"  } } }, // ARCADIA
  { "INTERTON"  ,                   0,                    0, MEMMAP_D,           REGION_PAL , {  0.0            , 50.0804105952784 }, 2,  30,   37, 42,    0, 38,     0.0, 1,  4, 0, { 1, 112, 225 }, 0x1F00, FALSE,  -1,   -1,  98, 120, { "START" , "SELECT", "-"     , "-"        }, { IMAGE_START, IMAGE_A   , IMAGE_B    , IMAGE_RESET   }, -1,  0, TRUE , FALSE, 0,               0, { { "1st"  , "1"    , "2"    , "3"    , "4"    , "5"    , "6"    , "7"    , "8"    , "9"    , "Cl"   , "0"    , "En"   , "x1"   , "x2"   , "x3"   , "x4"   , "Up"   , "Dn"   , "Lt"   , "Rt"   , "2nd"  , "3rd"  , "4th"  , "UpLt" , "UpRt" , "DnLt" , "DnRt" }, { "1st"  , "1"    , "2"    , "3"    , "4"    , "5"    , "6"    , "7"    , "8"    , "9"    , "Cl"   , "0"    , "En"   , "x1"   , "x2"   , "x3"   , "x4"   , "Up"   , "Dn"   , "Lt"   , "Rt"   , "2nd"  , "3rd"  , "4th"  , "UpLt" , "UpRt" , "DnLt" , "DnRt"  } } }, // INTERTON
  { "ELEKTOR"   ,                   0,                    0, MEMMAP_F,           REGION_PAL , {  0.0            , 50.0804105952784 }, 2,  -1,   -1, 36,    0, 38,     0.0, 1,  4, 0, { 1, 111, 225 }, 0x1F00, FALSE,  -1,   -1,  -1,  -1, { "START" , "UC"    , "LC"    , "RESET"    }, { IMAGE_START, IMAGE_UC  , IMAGE_LC   , IMAGE_RESET   }, -1,  0, TRUE , FALSE, 0,               0, { { "1st"  , "RCAS" , "WCAS" , "C"    , "BP"   , "REG"  , "8"    , "PC"   , "MEM"  , "4"    , "-"    , "+"    , "0"    , "-----", "-----", "-----", "-----", "Up"   , "Dn"   , "Lt"   , "Rt"   , "2nd"  , "3rd"  , "4th"  , "UpLt" , "UpRt" , "DnLt" , "DnRt" }, { "1st"  , "D"    , "E"    , "F"    , "9"    , "A"    , "B"    , "5"    , "6"    , "7"    , "1"    , "2"    , "3"    , "-----", "-----", "-----", "-----", "Up"   , "Dn"   , "Lt"   , "Rt"   , "2nd"  , "3rd"  , "4th"  , "UpLt" , "UpRt" , "DnLt" , "DnRt"  } } }, // ELEKTOR
  { "PIPBUG"    ,                   0,                    0, MEMMAP_PIPBUG1,     REGION_PAL , {  0.0            , 50.0             }, 1,  -1,   -1,  0,    0, 42, 20000.0, 0,  0, 4, { 0, 128, 255 },      0, FALSE,  -1,   -1,  -1,  -1, { "-"     , "-"     , "-"     , "-"        }, { IMAGE_START, IMAGE_A   , IMAGE_B    , IMAGE_RESET   }, -1,  0, FALSE, TRUE , 0,               0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   }, // PIPBUG
  { "BINBUG"    ,                   0,                    0, MEMMAP_BINBUG,      REGION_PAL , {  0.0            , 50.0             }, 1,  -1,   -1, 16,    0, 43, 20000.0, 0,  0, 0, { 0, 128, 255 },      0, FALSE,  -1,   -1,  -1,  -1, { "-"     , "-"     , "-"     , "-"        }, { IMAGE_START, IMAGE_A   , IMAGE_B    , IMAGE_RESET   }, -1,  0, TRUE , FALSE, BINBUG_DISKSIZE, 2, { { "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----"}, { "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----" } } }, // BINBUG
  { "INSTRUCTOR", INSTRUCTOR_BOXWIDTH, INSTRUCTOR_BOXHEIGHT, MEMMAP_O,           REGION_PAL , {  0.0            , 50.0             }, 1,FIRSTSI50EQUIV,LASTSI50EQUIV,32,0,36,17900.0,0,0,8,{0,0, 0 },      0, FALSE,1922, 2692, 305, 354, { "SENS"  , "INT"   , "MON"   , "RST"      }, { IMAGE_START, IMAGE_INT , IMAGE_MON  , IMAGE_RESET   }, -1,  0, FALSE, FALSE, 0,               0, { { "-----", "WCAS" , "BKPT" , "C"    , "RCAS" , "REG"  , "8"    , "STEP" , "MEM"  , "4"    , "RUN"  , "NXT"  , "0"    , "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----"}, { "-----", "D"    , "E"    , "F"    , "9"    , "A"    , "B"    , "5"    , "6"    , "7"    , "1"    , "2"    , "3"    , "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----" } } }, // INSTRUCTOR
  { "TWIN"      ,                   0,                    0, MEMMAP_TWIN,        REGION_NTSC, { 60.0            ,  0.0             }, 1,  -1,   -1, 55,    0, 40, 20800.0, 0,  0, 0, { 0,   0,   0 },      0, FALSE,  -1,   -1,  -1,  -1, { "-"     , "-"     , "-"     , "-"        }, { IMAGE_START, IMAGE_A   , IMAGE_B    , IMAGE_RESET   }, -1,  0, FALSE, FALSE, TWIN_DISKSIZE,   2,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   }, // TWIN
  { "CD2650"    ,                   0,                    0, MEMMAP_CD2650,      REGION_NTSC, { 59.4690265486726,  0.0             }, 1,  -1,   -1,  0,    0, 43, 19888.0, 0, 12, 0, { 0,   0,   0 },      0, FALSE,  -1,   -1,  -1,  -1, { "-"     , "-"     , "-"     , "-"        }, { IMAGE_START, IMAGE_A   , IMAGE_B    , IMAGE_RESET   }, -1,  0, FALSE, FALSE, CD2650_DISKSIZE, 4,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   }, // CD2650
  { "ZACCARIA"  ,     COINOP_BOXWIDTH,     COINOP_BOXHEIGHT, MEMMAP_ASTROWARS,   REGION_PAL , { 60.0            , 50.0             }, 1,  -1,   -1,  0,    0, 34, 23608.0, 0,  0, 0, { 0, 128, 255 }, 0x1500, TRUE ,  -1,   -1,  -1,  -1, { "1UP"   , "2UP"   , "Coin A", "Coin B"   }, { IMAGE_1UP  , IMAGE_2UP , IMAGE_COINA, IMAGE_COINB   }, 51,  6, TRUE , FALSE, 0,               0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   }, // ZACCARIA
  { "MALZAK"    ,     COINOP_BOXWIDTH,     COINOP_BOXHEIGHT, MEMMAP_MALZAK1,     REGION_PAL , { 60.0            , 50.0             }, 1,  -1,   -1, 11,    0, 34,     0.0, 2,  0, 0, { 0, 128, 255 }, 0x1400, TRUE ,  -1,   -1,  -1,  -1, { "1UP"   , "2UP"   , "Coin"  , "Switch"   }, { IMAGE_1UP  , IMAGE_2UP , IMAGE_COINA, IMAGE_DIPS    }, 51,  6, TRUE , FALSE, 0,               0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   }, // MALZAK
  { "PHUNSY"    ,                   0,                    0, MEMMAP_PHUNSY,      REGION_PAL,  {  0.0            , 50.0             }, 1,FIRSTPHUNSYEQUIV,LASTPHUNSYEQUIV,24,0,36,20032.0,0,0,0,{0,0,0},    0, FALSE,  -1,   -1,  -1,  -1, { "-"     , "-"     , "-"     , "-"        }, { IMAGE_START, IMAGE_A   , IMAGE_B    , IMAGE_RESET   }, -1,  0, FALSE, FALSE, 0,               0, { { "-----", "0"    , "4"    , "8"    , "1"    , "5"    , "9"    , "2"   , "6"     , "A"    , "3"    , "7"    , "B"    , "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----"}, { "-----", "C"    , "D->M" , "RESET", "D"    , "M->D" , "HALT" , "E"    , "GO TO", "DUMPC", "F"    , "CLEAR", "LOADC", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----" } } }, // PHUNSY
  { "PONG"      ,                   0,                    0, MEMMAP_8550,        REGION_PAL,  { 60.0            , 50.0             }, 2,  -1,   -1, 15,    0, 33,     1.0, 0,  0, 0, { 0, 128, 255 },      0, FALSE,  -1,   -1,  -1,  -1, { "Start" , "Game"  , "Serve" , "Bat size" }, { IMAGE_START2,IMAGE_GAME, IMAGE_SERVE, IMAGE_BATSIZE }, 57,  6, TRUE , FALSE, 0,               0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   }, // PONG
  { "SELBST"    ,     SELBST_BOXWIDTH,     SELBST_BOXHEIGHT, MEMMAP_SELBST,      REGION_PAL,  { 60.0            , 50.0             }, 1,  -1,   -1, 24,    0, 39, 20000.0, 0,  0, 6, { 0,   0,   0 },      0, FALSE,  -1,   -1,  -1,  -1, { "-"     , "-"     , "-"     , "-"        }, { IMAGE_START, IMAGE_A   , IMAGE_B    , IMAGE_RESET   }, -1,  0, FALSE, TRUE , 0,               0, { { "-----", "C"    , "D"    , "E"    , "8"    , "9"    , "A"    , "4"   , "5"     , "6"    , "0"    , "1"    , "2"    , "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----"}, { "-----", "F"    , "CMD"  , "FLAG" , "B"    , "RUN"  , "MON"  , "7"    , "GOTO" , "PC"   , "3"    , "RST"  , "NXT"  , "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----" } } }, // SELBST
  { "MIKIT"     ,      MIKIT_BOXWIDTH,      MIKIT_BOXHEIGHT, MEMMAP_MIKIT,       REGION_PAL,  { 60.0            , 50.0             }, 1,  -1,   -1, 24,    0, 39, 20000.0, 0,  0, 6, { 0,   0,   0 },      0, FALSE,  -1,   -1,  -1,  -1, { "-"     , "-"     , "-"     , "-"        }, { IMAGE_START, IMAGE_A   , IMAGE_B    , IMAGE_RESET   }, -1,  0, FALSE, FALSE, 0,               0, { { "-----", "BLANK", "R"    , "C"    , "+"    , "G"    , "8"    , "H"   , "P"     , "4"    , "L"    , "S"    , "0"    , "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----"}, { "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----", "-----" } } }, // MIKIT
  { "TYPERIGHT" ,  TYPERIGHT_BOXWIDTH,  TYPERIGHT_BOXHEIGHT, MEMMAP_TYPERIGHT,   REGION_PAL,  {  0.0            , 50.0             }, 1,  -1,   -1, 52,    0, 33,     1.0, 0,  0, 0, { 0,   0,   0 },      0, FALSE,  -1,   -1,  -1,  -1, { "Class" , "Game"  , "Speed" , "Accuracy" }, { IMAGE_CLASS, IMAGE_GAME, IMAGE_START, IMAGE_A       }, 63, 23, FALSE, FALSE, 0,               0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   }, // TYPERIGHT
};

                                                                                                                       // - - - - L L L L R R R R L L L L R R R R L L L L L L L L R R R R R R R R L L L L L L L L L L L L R R R R R R R R R R R R
                                                                                                                       // S A B Rx1x2x3x4x1x2x3x4#1#2#3#4#1#2#3#4LtRtUpDnULURDLDRLtRtUpDnULURDLDR 1 2 3 4 5 6 7 8 9 C 0 E 1 2 3 4 5 6 7 8 9 C 0 E                                 ------tones------  ----noises--- -----samples----
EXPORT struct MemMapInfoStruct memmapinfo[MEMMAPS] = {                                                                 // 0 1 2 3 4 5 6 7 8 91011121314151617181920212223242526272829303132333435363738394041424344454647484950515253545556575859   rotate                        0 1 2 3 4 5 6 7 8  9101112131415 16171819202122232425
{ INTERTON  , "Interton VC 4000, type \"A\" (2K ROM + 0K RAM)",     0,    INTERTON_OVERLAY, ""                         , {1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, FALSE, MENUOPT_INTERTON    , {1,0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0, 1,0,0,0,0,1,0,0,0,0} }, //  0 MEMMAP_A
{ INTERTON  , "Interton VC 4000, type \"B\" (4K ROM + 0K RAM)",     0,    INTERTON_OVERLAY, ""                         , {1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, FALSE, MENUOPT_INTERTON    , {1,0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0, 1,0,0,0,0,1,0,0,0,0} }, //  1 MEMMAP_B
{ INTERTON  , "Interton VC 4000, type \"C\" (4K ROM + 1K RAM)",     0,    INTERTON_OVERLAY, ""                         , {1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, FALSE, MENUOPT_INTERTON    , {1,0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0, 1,0,0,0,0,1,0,0,0,0} }, //  2 MEMMAP_C
{ INTERTON  , "Interton VC 4000, type \"D\" (6K ROM + 1K RAM)",     0,    INTERTON_OVERLAY, ""                         , {1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, FALSE, MENUOPT_INTERTON    , {1,0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0, 1,0,0,0,0,1,0,0,0,0} }, //  3 MEMMAP_D
{ ELEKTOR   , "Elektor TV Games Computer (basic)"             ,     0,     ELEKTOR_OVERLAY, ""                         , {1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, FALSE, MENUOPT_ELEKTOR     , {1,0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0} }, //  4 MEMMAP_E
{ ELEKTOR   , "Elektor TV Games Computer (expanded)"          ,     0,     ELEKTOR_OVERLAY, ""                         , {1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, FALSE, MENUOPT_ELEKTOR     , {1,0,0,1,1,1,1,1,1, 1,1,1,1,1,1,1, 1,0,0,0,0,1,0,0,0,0} }, //  5 MEMMAP_F
{ ARCADIA   , "Emerson Arcadia 2001, type \"G\" (Emerson)"    ,     0,     ARCADIA_OVERLAY, ""                         , {1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, FALSE, MENUOPT_ARCADIA     , {1,0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0} }, //  6 MEMMAP_G
{ ARCADIA   , "Emerson Arcadia 2001, type \"H\" (Tele-Fever)" ,     0,     ARCADIA_OVERLAY, ""                         , {1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, FALSE, MENUOPT_ARCADIA     , {1,0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0} }, //  7 MEMMAP_H
{ ARCADIA   , "Emerson Arcadia 2001, type \"I\" (Palladium)"  ,     0,     ARCADIA_OVERLAY, ""                         , {1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, FALSE, MENUOPT_ARCADIA     , {1,0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0} }, //  8 MEMMAP_I
#ifdef KEYCLICKS
{ PIPBUG    , "PIPBUG 1-based machine"                        ,     0,      PIPBUG_OVERLAY, ""                         , {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, FALSE, MENUOPT_PIPBUG      , {1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,1,1,0,0,0,1} }, //  9 MEMMAP_PIPBUG1
{ PIPBUG    , "PIPBUG 2-based machine (modified ABC1500)"     ,     0,      PIPBUG_OVERLAY, ""                         , {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, FALSE, MENUOPT_PIPBUG      , {0,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,1,1,0,0,0,1} }, // 10 MEMMAP_PIPBUG2
{ BINBUG    , "BINBUG-based machine"                          ,     0,      BINBUG_OVERLAY, ""                         , {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, FALSE, MENUOPT_BINBUG      , {0,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,1,1,0,1,1,1} }, // 11 MEMMAP_BINBUG
#else
{ PIPBUG    , "PIPBUG 1-based machine"                        ,     0,      PIPBUG_OVERLAY, ""                         , {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, FALSE, MENUOPT_PIPBUG      , {1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,1,0,0,0,0,1} }, //  9 MEMMAP_PIPBUG1
{ PIPBUG    , "PIPBUG 2-based machine (modified ABC1500)"     ,     0,      PIPBUG_OVERLAY, ""                         , {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, FALSE, MENUOPT_PIPBUG      , {0,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,1,0,0,0,0,1} }, // 10 MEMMAP_PIPBUG2
{ BINBUG    , "BINBUG-based machine"                          ,     0,      BINBUG_OVERLAY, ""                         , {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, FALSE, MENUOPT_BINBUG      , {0,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,1,0,0,1,1,1} }, // 11 MEMMAP_BINBUG
#endif
{ TWIN      , "Signetics TWIN"                                ,     0,      QWERTY_OVERLAY, ""                         , {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, FALSE, MENUOPT_TWIN        , {0,1,1,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,1,0,1,1,1,1} }, // 12 MEMMAP_TWIN
{ PIPBUG    , "Artemis"                                       ,     0,      QWERTY_OVERLAY, ""                         , {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, FALSE, MENUOPT_PIPBUG      , {0,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,1,0,0,0,0,1} }, // 13 MEMMAP_ARTEMIS
{ INSTRUCTOR, "Signetics Instructor 50 (basic)"               ,     0,        SI50_OVERLAY, ""                         , {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, FALSE, MENUOPT_INSTRUCTOR  , {1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0} }, // 14 MEMMAP_N
{ INSTRUCTOR, "Signetics Instructor 50 (expanded)"            ,     0,        SI50_OVERLAY, ""                         , {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, FALSE, MENUOPT_INSTRUCTOR  , {1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0} }, // 15 MEMMAP_O
{ CD2650    , "Central Data 2650"                             ,     0,      CD2650_OVERLAY, ""                         , {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, FALSE, MENUOPT_CD2650      , {1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,1,0,0,1,1,0} }, // 16 MEMMAP_CD2650
{ 0         , ""                                              ,     0,                   0, ""                         , {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, FALSE, 0                   , {0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0} }, // 17 spare
{ 0         , ""                                              ,     0,                   0, ""                         , {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, FALSE, 0                   , {0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0} }, // 18 spare
{ PHUNSY    , "PHUNSY"                                        ,     0,      PHUNSY_OVERLAY, ""                         , {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, FALSE, MENUOPT_PHUNSY      , {1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0} }, // 19 MEMMAP_PHUNSY
{ SELBST    , "Selbstbaucomputer"                             ,     0,      SELBST_OVERLAY, ""                         , {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, FALSE, MENUOPT_SELBST      , {0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0} }, // 20 MEMMAP_SELBST
{ MIKIT     , "MIKIT 2650"                                    ,     0,       MIKIT_OVERLAY, ""                         , {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, FALSE, MENUOPT_MIKIT       , {0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0} }, // 21 MEMMAP_MIKIT
{ ZACCARIA  , "Coin-op (Astro Wars)"                          , 12800,   ASTROWARS_OVERLAY, "Zaccaria, 1980"           , {1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0}, TRUE , MENUOPT_ASTROWARS   , {1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,1,1,1,0,0,0,0,0,0} }, // 22 MEMMAP_ASTROWARS
{ ZACCARIA  , "Coin-op (Galaxia)"                             , 12800,     GALAXIA_OVERLAY, "Zaccaria, 1979"           , {1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0}, TRUE , MENUOPT_GALAXIA     , {1,1,1,1,1,0,0,0,0, 0,0,0,0,0,0,0, 0,1,1,1,0,0,0,0,0,0} }, // 23 MEMMAP_GALAXIA
{ ZACCARIA  , "Coin-op (Laser Battle)"                        , 20480, LASERBATTLE_OVERLAY, "Zaccaria, 1981"           , {1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, FALSE, MENUOPT_LASERBATTLE , {1,1,1,1,1,1,1,0,0, 0,0,0,0,0,0,0, 0,0,0,1,0,0,0,0,0,0} }, // 24 MEMMAP_LASERBATTLE
{ ZACCARIA  , "Coin-op (Lazarian)"                            , 22528, LASERBATTLE_OVERLAY, "Zaccaria/Midway, 1981"    , {1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, FALSE, MENUOPT_LAZARIAN    , {1,1,1,1,1,1,1,0,0, 0,0,0,0,0,0,0, 0,0,0,1,0,0,0,0,0,0} }, // 25 MEMMAP_LAZARIAN
{ MALZAK    , "Coin-op (Malzak 1)"                            ,  8704,     MALZAK1_OVERLAY, "Kitronix, 1981?"          , {1,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0}, FALSE, MENUOPT_MALZAK1     , {1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,1,0,0,0,0,0,0} }, // 26 MEMMAP_MALZAK1
{ MALZAK    , "Coin-op (Malzak 2)"                            ,  8192,     MALZAK2_OVERLAY, "Kitronix, 1981?"          , {1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0}, FALSE, MENUOPT_MALZAK2     , {1,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,1,0,0,0,0,0,0} }, // 27 MEMMAP_MALZAK2
{ PONG      , "AY-3-8500/8550/8550-1 + AY-3-8515"             ,     0,      AY8550_OVERLAY, "General Instrument, 1975?", {1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0}, FALSE, MENUOPT_8550        , {1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0} }, // 28 MEMMAP_8550 (Tennis/Soccer/Squash/Practice/Handicap/Rifle.)
{ PONG      , "AY-3-8600/AY-3-8600-1 + AY-3-8615"             ,     0,      AY8600_OVERLAY, "General Instrument, 1976?", {1,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0}, FALSE, MENUOPT_8600        , {1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0} }, // 29 MEMMAP_8600 (Tns/Hcky/Scr/Sqsh/Prctce/Grdbl/Bsktbl/BsktblPrctce/Trgt.)
{ TYPERIGHT , "VTech Type-right"                              ,     0,   TYPERIGHT_OVERLAY, "VTech, 1985"              , {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, FALSE, MENUOPT_TYPERIGHT   , {1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0} }, // 30 MEMMAP_TYPERIGHT
}; // There is no "s" at the end of "General Instrument"

EXPORT const struct CanvasStruct canvas[CANVASES] = {
{       AXESWIDTH,       AXESHEIGHT, SUBWINDOW_MONITOR_XVI },
{ INDUSTRIALWIDTH, INDUSTRIALHEIGHT, SUBWINDOW_INDUSTRIAL  },
{  MAGNIFIERWIDTH,  MAGNIFIERHEIGHT, 0                     },
{     MEMMAPWIDTH,     MEMMAPHEIGHT, SUBWINDOW_MEMORY      },
{       MINIWIDTH,       MINIHEIGHT, SUBWINDOW_CONTROLS    },
{    PREVIEWWIDTH,    PREVIEWHEIGHT, SUBWINDOW_SPRITES     },
{ PRINTERWIDTH_VIEW, PRINTERHEIGHT_VIEW, SUBWINDOW_PRINTER },
{       ROLLWIDTH,       ROLLHEIGHT, SUBWINDOW_PAPERTAPE   },
{       ROLLWIDTH,       ROLLHEIGHT, SUBWINDOW_PAPERTAPE   },
{   WAVEFORMWIDTH,   WAVEFORMHEIGHT, SUBWINDOW_TAPEDECK    },
#ifdef AMIGA
{       BLOXWIDTH,       BLOXHEIGHT, SUBWINDOW_FLOPPYDRIVE },
{       BLOXWIDTH,       BLOXHEIGHT, SUBWINDOW_FLOPPYDRIVE },
{      SPLITWIDTH,      SPLITHEIGHT, SUBWINDOW_CONTROLS    },
{      SPLITWIDTH,      SPLITHEIGHT, SUBWINDOW_CONTROLS    },
{      SPLITWIDTH,      SPLITHEIGHT, SUBWINDOW_CONTROLS    },
{      SPLITWIDTH,      SPLITHEIGHT, SUBWINDOW_CONTROLS    },
{      SPLITWIDTH,      SPLITHEIGHT, SUBWINDOW_CONTROLS    },
{      SPLITWIDTH,      SPLITHEIGHT, SUBWINDOW_CONTROLS    },
#endif
};

EXPORT const FLAG loopable[TOTALCHANNELS] =
{ TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, // synthetic sounds
  FALSE, // explode
  FALSE, // shoot
  FALSE, // bang
  FALSE, // coin
  FALSE, // printer
  FALSE, // click
  TRUE,  // spin
  FALSE, // step
  TRUE,  // grind
  TRUE,  // punch
};

EXPORT const int speedupnum[SPEED_MAX - SPEED_MIN + 1] =
{    25,
     50,
     75,
    100,
    125,
    150,
    200,
    400,
    800
}, SubWindowMenuItem[SUBWINDOWS] =
{   MENUITEM_CONTROLS,         //  0
    MENUITEM_DIPSWITCHES,      //  1
    MENUITEM_GAMEINFO,         //  2
    MENUITEM_EDITMEMORY,       //  3
    MENUITEM_VIEWMONITOR_CPU,  //  4
    MENUITEM_OPCODES,          //  5
    MENUITEM_PALETTE,          //  6
    MENUITEM_TAPEDECK,         //  7
    MENUITEM_SPRITEVIEWER,     //  8
    MENUITEM_ADJUSTSPEED,      //  9
    MENUITEM_HOSTKYBD,         // 10
    MENUITEM_HOSTPADS,         // 11
    MENUITEM_VIEWMONITOR_XVI,  // 12
    MENUITEM_VIEWMONITOR_PSGS, // 13
    MENUITEM_MUSIC,            // 14
    -1,                        // 15 output subwindow
    MENUITEM_PRINTER,          // 16
    MENUITEM_FLOPPYDRIVE,      // 17
    MENUITEM_PAPERTAPE,        // 18
    MENUITEM_INDUSTRIAL,       // 19
}, num_to_num[NUMKEYS] =
{ 11, // "0"/"+"
   1, // "1"
   2, // "2"
   3, // "3"
   4, // "4"
   5, // "5"
   6, // "6"
   7, // "7"
   8, // "8"
   9, // "9"
  10, // "Cl",
  12, // "En"
  13, // "x1"
  14, // "x2"
  15, // "x3"
  16, // "x4"
  17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27
};

EXPORT const struct GuestKeyStruct guestkeys[NUMKEYS] =
{   { -1, -1 }, //  0
    {  8, 11 },
    {  0,  1 },
    { 10, 13 },
    { 14, 17 },
    { 15, 18 }, //  5
    { 16, 19 },
    { 20, 23 },
    { 21, 24 },
    { 22, 25 },
    { 26, 29 }, // 10
    { 27, 30 },
    { 28, 31 },
    {  2,  5 },
    {  3,  6 },
    {  4,  7 }, // 15
    {  9, 12 },
    { -1, -1 }, // 17
    { -1, -1 },
    { -1, -1 },
    { -1, -1 }, // 20
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { -1, -1 }, // 25
    { -1, -1 },
    { -1, -1 }  // 27
};

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       FLAG                     capslock,
                                      d3d,
                                      midiplaying[GUESTCHANNELS],
                                      paperreaderenabled,
                                      twin_escaped,
                                      upgrade;
IMPORT       UBYTE                    acca,
                                      accb,
                                      cc,
                                      chan_volume[GUESTCHANNELS],
                                      dg640_chars[128][DG640_CHARHEIGHT],
                                      guestkey,
                                      keys_column[7],
                                      memory[32768],
                                      papertapebyte[2],
                                      psu,
                                      psl,
                                      r[7],
                                      s_toggles,
                                      aw_dips1, aw_dips2,
                                      ga_dips,
                                      lb_dips,
                                      lz_dips2, lz_dips3,
                                      squeal,
                                      smusnote[GUESTCHANNELS],
                                      smusplaying[GUESTCHANNELS],
                                      smusvolume[GUESTCHANNELS],
                                      vdu[MAX_VDUCOLUMNS][MAX_VDUROWS];
IMPORT       ASCREEN                  screen[BOXWIDTH][BOXHEIGHT];
IMPORT       TEXT                     autotext[GAMEINFOLINES][80 + 1],
                                      Clock2650Str[COMMASTRLEN],
                                      controltip[5][64 + 1],
                                      fn_screenshot[MAX_PATH + 1],      // complete pathname of the screenshot
                                      gtempstring[256 + 1],
                                      hostname[256 + 1],
                                      pgmtext[6 * KILOBYTE],
                                      FrameStr[COMMASTRLEN],
                                      TimeStr[11 + 1], // "hh:mm:ss.uu"
                                      temppath_games[MAX_PATH + 1],
                                      userinput[MAX_PATH + 80 + 1],
                                      username[MAX_USERNAMELEN + 1];
IMPORT       char                     mn[256 + 1];
IMPORT       UWORD                    iar,
                                      ix,
                                      mirror_w[32768],
                                      pc,
                                      ras[8],
                                      sp;
IMPORT       ULONG                    arcadia_bigctrls,
                                      asicreads[32768],
                                      asicwrites[32768],
                                      cd2650_downtill,
                                      cycles_2650,
                                      keyframes[SCANCODES],
                                      lb_snd,
                                      mtrk[GUESTCHANNELS],
                                      miditime[GUESTCHANNELS],
                                      papertapelength[2],
                                      papertapewhere[2],
                                      smusduration,
                                      smustime[GUESTCHANNELS],
                                      strk[GUESTCHANNELS];
IMPORT       int                      angles,
                                      base,
                                      batvalue,
                                      belling[2],
                                      binbug_baudrate,
                                      binbug_dosver,
                                      binbug_userdrives,
                                      blink,
                                      cd2650_biosver,
                                      cd2650_dosver,
                                      cd2650_userdrives,
                                      cd2650_vdu,
                                      chan_status[GUESTCHANNELS],
                                      colourset,
                                      coomer,
                                      darkenbg,
                                      fastbinbug,
                                      fastcd2650,
                                      fastpipbug,
                                      fastselbst,
                                      flagline,
                                      hostcontroller[2],
                                      hostvolume,
                                      key1,
                                      key2,
                                      key3,
                                      key4,
                                      lockhoriz,
                                      n1, n2, n3, n4,
                                      paddleup,
                                      paddledown,
                                      paddleleft,
                                      paddleright,
                                      pipbug_periph,
                                      pipbug_vdu,
                                      playerid,
                                      players,
                                      poketoken,
                                      pong_start,
                                      pong_a,
                                      pong_b,
                                      pong_reset,
                                      ponghertz,
                                      pongspeed,
                                      pong_variant,
                                      prtunit,
                                      pvibase,
                                      queuepos,
                                      rastn,
                                      reassociate,
                                      robotspeed[2],
                                      score[2],
                                      serializemode,
                                      serving,
                                      sidebarwidth,
                                      s_io,
                                      s_id,
                                      s_is,
                                      slice_2650,
                                      speedup,
                                      step,
                                      stretch43,
                                      twin_dosver,
                                      useff[2],
                                      vdu_rows_total,
                                      vdu_x, vdu_y,
                                      whichkeyrect,
                                      whichoverlay,
                                      winleftx,
                                      wintopy;
IMPORT       float                    chan_hertz[GUESTCHANNELS];
IMPORT       double                   tapespeed;
IMPORT       FILE                    *MIDIHandle,
                                     *MIDITrackHandle[GUESTCHANNELS],
                                     *SMUSHandle,
                                     *SMUSTrackHandle[GUESTCHANNELS];
IMPORT       MEMFLAG                  memflags[ALLTOKENS];
IMPORT       struct DriveStruct       drive[DRIVES_MAX];
IMPORT       struct KeyNameStruct     keyname[SCANCODES];
IMPORT       struct KindStruct        filekind[KINDS];
IMPORT const struct KeyTableStruct    keytable[16];
IMPORT const struct LBNoteStruct      lb_note[13 + 1];
IMPORT       struct MonitorStruct     monitor[MONITORGADS];
IMPORT const struct VolumeStruct      volume_3to16[8],
                                      volume_4to16[16];
IMPORT       struct NoteStruct        notes[NOTES + 1];
IMPORT       struct OpcodeStruct      opcodes[3][256];
IMPORT const UWORD                    pvi_spritedata[4];
IMPORT const STRPTR                   overlays[OVERLAYS][33],
                                      tokenname[LASTTOKEN - FIRSTTOKEN + 1][STYLES];
IMPORT       struct HostMachineStruct hostmachines[MACHINES];

#ifdef AMIGA
    IMPORT       int                  currentgame,
                                      led,
                                      morphos,
                                      soundoutput,
                                      throb,
                                      whichimagery,
                                      windowed_size,
                                      windowed_wide;
    IMPORT       TEXT                 speech_race,
                                      speech_sex;
    IMPORT       UWORD                InvisiblePointerData[6];
    IMPORT       ULONG                clipunit,
                                      pending,
                                      speech_articulation,
                                      speech_centralizenum,
                                      speech_centralizeto,
                                      speech_enthusiasm,
                                      speech_frication,
                                      speech_perturbation,
                                      speech_pitch,
                                      speech_voicing,
                                      speech_wpm;
    IMPORT       struct Gadget*       gadgets[GIDS + 1];
    IMPORT       struct MenuItem*     ItemPtr;
    IMPORT       struct Screen*       ScreenPtr;
    IMPORT       struct Catalog*      CatalogPtr;
    IMPORT       struct Image*        images[IMAGES];
    IMPORT       struct Library*      SocketBase;
    IMPORT       struct Window       *MainWindowPtr,
                                     *SubWindowPtr[SUBWINDOWS];
    IMPORT const int                  memmap_to_smlimage[MEMMAPS];
#endif
#ifdef WIN32
    IMPORT FLAG           incli,
                          opening,
                          unit[2];
    IMPORT WCHAR          voicename[80 + 1];
    IMPORT HWND           hStatusBar,
                          hToolbar,
                          MainWindowPtr,
                          SubWindowPtr[SUBWINDOWS];
    IMPORT HDC            ControlsRastPtr1,
                          ControlsRastPtr2;
    IMPORT HINSTANCE      InstancePtr;
    IMPORT int            candy[CANDIES],
                          cheevos,
                          language,
                          memmap_to_smlimage[MEMMAPS],
                          priority,
                          realwide,
                          statusbarheight,
                          CatalogPtr; // APTR doesn't work
    IMPORT ULONG          speech_rate;
    IMPORT struct AssociationStruct assoc[ASSOCIATIONS];
#endif

/* MODULE STRUCTURES------------------------------------------------------

(none)

MODULE ARRAYS-------------------------------------------------------------

(see engine1.h)

MODULE FUNCTIONS------------------------------------------------------- */

MODULE void ConnectAsClient_IPv4(const char* Server, unsigned short Port);
MODULE void ActAsServer_IPv4(unsigned short Port);
MODULE void readhiscore(int src, int dest);
MODULE void aserialize_bool(STRPTR label, int* value);
MODULE void aserialize_int(STRPTR label, int* value, int lowest, int highest);
MODULE void aserialize_uword(STRPTR label, UWORD* value, int lowest, int highest);
MODULE void aserialize_ubyte(STRPTR label, UBYTE* value);
MODULE void aserialize_colours(STRPTR label, UBYTE* red, UBYTE* green, UBYTE* blue);
MODULE void aserialize_str(STRPTR label, STRPTR contents);
MODULE void serializeconfig(void);

// CODE-------------------------------------------------------------------

EXPORT FLAG poke_start(STRPTR thestring, FLAG quiet, FLAG doke)
{   int i;

    DISCARD strupr(thestring);

    poketoken = 0; // ie. memory
    for (i = 0; i < LASTTOKEN - FIRSTTOKEN + 1; i++)
    {   if (!strcmp(thestring, (const char*) tokenname[i][style]))
        {   poketoken = i + FIRSTTOKEN;
            switch (poketoken)
            {
            case  TOKEN_PSU: fromnum = (int)  psu             ; tolimit =   255;
            acase TOKEN_PSL: fromnum = (int)  psl             ; tolimit =   255;
            acase TOKEN_SP:  fromnum = (int) (psu &    7)     ; tolimit =     7;
            acase TOKEN_CC:  fromnum = (int) (psl & 0xC0) >> 6; tolimit =     3;
            acase TOKEN_IAR: fromnum = (int)  iar             ; tolimit = 32767;
            adefault:
                if     (poketoken >= TOKEN_R0   && poketoken <= TOKEN_R6  )
                {            fromnum = (int)  r[   poketoken -  TOKEN_R0  ]; tolimit =   255;
                } elif (poketoken >= TOKEN_RAS0 && poketoken <= TOKEN_RAS7)
                {            fromnum = (int)  ras[ poketoken -  TOKEN_RAS0]; tolimit = 32767;
            }   }
            break;
    }   }

    if (doke && poketoken != 0)
    {   if (!quiet)
        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_DOKEMEMORY, "You can only DOKE memory!\n\n"));
        }
        return FALSE;
    }

    if (poketoken == 0)
    {   if (doke)
        {   tolimit = 65535;
            poketoken = parse_expression(thestring, MAX_ADDR - 1, FALSE);
        } else
        {   tolimit = 255;
            poketoken = parse_expression(thestring, MAX_ADDR    , FALSE);
#ifdef ADVENTURETRAINER
            if (machine == BINBUG && whichgame == -1 && poketoken == 0x1E4F)
            {   tolimit = 0x63;
            } else
#endif
            tolimit = 255;
    }   }
    if (poketoken == OUTOFRANGE)
    {   if (!quiet)
        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_INVALIDRANGE, "Valid range is $%X..$%X!\n\n"), 0, doke ? (MAX_ADDR - 1) : MAX_ADDR);
        }
        return FALSE;
    }

    if (doke)
    {   if
        (   machine   == ELEKTOR
         && poketoken >= 0x1D00
         && poketoken <= 0x1D1E
        )
        {   fromnum = (int) ((memory[poketoken + 1] * 256) + memory[poketoken    ]); // little-endian
        } else
        {   fromnum = (int) ((memory[poketoken    ] * 256) + memory[poketoken + 1]);
    }   }
    elif (poketoken <= 32767) // memory
    {   fromnum = (int) memory[poketoken];
    }

    sprintf(fromstr, "$%X", (unsigned int) fromnum);
    tonum = fromnum;
    strcpy((char*) tostr, (const char*) fromstr);

    return TRUE;
}

EXPORT void poke_end(FLAG quiet, FLAG doke)
{   if (tonum >= -256 && tonum < 0)
    {   tonum += 256;
    }

    if (poketoken == OUTOFRANGE || tonum < 0 || tonum > tolimit)
    {   if
        (   !quiet
#ifdef ADVENTURETRAINER
         || (machine == BINBUG && whichgame == -1 && poketoken == 0x1E4F)
#endif
        )
        {   zprintf
            (   TEXTPEN_DEBUG,
                LLL(
                    MSG_ENGINE_OUTOFBOUNDS,
                    "Value is out of bounds (limit is %d)!\n\n"
                ),
                tolimit
            );
        }
        return;
    } // implied else

    switch (poketoken)
    {
    case TOKEN_PSU:
        psu = (UBYTE) tonum;
    acase TOKEN_PSL:
        psl = (UBYTE) tonum;
    acase TOKEN_SP:
        psu &= 0xF8;
        psu |= (UBYTE) tonum;
    acase TOKEN_CC:
        psl &= 0x3F;
        psl |= (UBYTE) (tonum << 6);
    acase TOKEN_IAR:
        iar = (UWORD) tonum;
    acase TOKEN_RAS0:
    case TOKEN_RAS1:
    case TOKEN_RAS2:
    case TOKEN_RAS3:
    case TOKEN_RAS4:
    case TOKEN_RAS5:
    case TOKEN_RAS6:
    case TOKEN_RAS7:
        ras[poketoken - TOKEN_RAS0] = (UWORD) tonum;
    acase TOKEN_R0:
    case TOKEN_R1:
    case TOKEN_R2:
    case TOKEN_R3:
    case TOKEN_R4:
    case TOKEN_R5:
    case TOKEN_R6:
        r[poketoken - TOKEN_R0] = (UBYTE) tonum;
    adefault:
        if (tonum <= 65535)
        {   poketoken = mirror_w[poketoken];
            if (doke)
            {   if
                (   machine == ELEKTOR
                 && (poketoken >= 0x1D00 && poketoken <= 0x1D1E) // little-endian
                )
                {   memory[poketoken + 1] = (UBYTE) (tonum / 256);
                    memory[poketoken    ] = (UBYTE) (tonum % 256);
                } else
                {   memory[poketoken    ] = (UBYTE) (tonum / 256);
                    memory[poketoken + 1] = (UBYTE) (tonum % 256);
                }
                if
                (   (memflags[poketoken    ] & AUDIBLE)
                 || (memflags[poketoken + 1] & AUDIBLE)
                )
                {   playsound(TRUE);
            }   }
            else
            {   memory[poketoken] = (UBYTE) tonum;
                if (memflags[poketoken] & AUDIBLE)
                {   playsound(TRUE);
            }   }
            DISCARD getfriendly(poketoken);
    }   }

    if (poketoken >= FIRSTTOKEN)
    {   strcpy(friendly, tokenname[poketoken - FIRSTTOKEN][style]);
    }

    if (fromnum != tonum)
    {   if (!quiet)
        {   if (doke)
            {   zprintf
                (   TEXTPEN_DEBUG,
                    LLL(
                        MSG_ENGINE_DOKECHANGED,
                        "Changed contents of %s..$%04X from $%04X to $%04X.\n\n"
                    ),
                    friendly,
                    (ULONG) (poketoken + 1),
                    (ULONG) fromnum,
                    (ULONG) tonum
                );
            } else
            {   zprintf
                (   TEXTPEN_DEBUG,
                    LLL(
                        MSG_ENGINE_CHANGEDCONTENTS,
                        "Changed contents of %s from $%X to $%X.\n\n" // can be more than 2 hex digits because we support eg. E IAR 1234
                    ),
                    friendly,
                    (ULONG) fromnum,
                    (ULONG) tonum
                );
        }   }

        update_monitor(FALSE);
    } else
    {   if (!quiet)
        {   zprintf
            (   TEXTPEN_DEBUG,
                LLL(
                    MSG_ENGINE_NOCHANGE,
                    "No effect (no change to contents of address).\n\n"
            )   );
}   }   }

EXPORT void emu_pause(void)
{   if (!paused)
    {   paused = TRUE;
        if (inframe && !crippled)
        {   cripple();
        }
        clearruntos();
        sound_off(FALSE);
        updatesmlgad(GADPOS_PAUSED, paused, TRUE);
        updatemenu(MENUITEM_PAUSED);
        settitle();
        blanker_on();
        updatepointer(FALSE, TRUE);
        if (useguideray)
        {   draw_guide_ray(FALSE);
        }
        redrawscreen(); // helpful in frame skipping mode (ensures most
        // recent executed frame is shown, not a frame from a while ago).
        // And also needed for guide ray.
#ifdef WIN32
        if (cheevos)
        {
#ifdef LOGCHEEVOS
           zprintf(TEXTPEN_VERBOSE, "RA_SetPaused(TRUE)\n");
#endif
           RA_SetPaused(TRUE);
        }
#endif
}   }

EXPORT void emu_unpause(void)
{   if (paused) // important!
    {   paused = FALSE;
        sound_on(TRUE);
        updatesmlgad(GADPOS_PAUSED, paused, TRUE);
        updatemenu(MENUITEM_PAUSED);
        settitle();
        blanker_off();
        updatepointer(FALSE, TRUE);
        if (useguideray)
        {   draw_guide_ray(TRUE);
            redrawscreen();
        }
#ifdef WIN32
        if (cheevos)
        {
#ifdef LOGCHEEVOS
           zprintf(TEXTPEN_VERBOSE, "RA_SetPaused(FALSE)\n");
#endif
           RA_SetPaused(FALSE);
        }
#endif
}   }

EXPORT void cripple(void)
{   crippled = TRUE;
    updatemenus();
    updatebiggads();
    updatesmlgads();
}
EXPORT void uncripple(void)
{   crippled = FALSE;
    updatemenus();
    updatebiggads();
    updatesmlgads();
}

EXPORT void fixextension(STRPTR extension, STRPTR passedfn, FLAG force, STRPTR suffix)
{   FLAG   found = FALSE;
    int    i;
    size_t length;

    // This routine should really only be used on the file component of the pathname.

    // force means we will replace an existing extension
    //  (typically used before showing an ASL save requester)
    // otherwise, we just add an extension if there is no recognised extension
    //  (typically used after showing an ASL load requester)

    length = strlen(passedfn);
    if (force) // saving
    {   if (length >= 1)
        {   // find extension
            for (i = (int) (length - 1); i >= 0; i--)
            {   /* if (passedfn[i] == '/' || passedfn[i] == '\\' || passedfn[i] == ':')
                {   break;
                } else */ if (passedfn[i] == '.')
                {   found = TRUE;
                    break;
            }   }
            if (found)
            {   passedfn[i] = EOS;
        }   }
        strcat(passedfn, suffix);
        strcat(passedfn, extension);
    } else // loading
    {   for (i = 0; i < KINDS; i++) // this is why filekind[].extension needs to be fully populated
        {   if
            (   length > strlen(filekind[i].extension)
             && !stricmp(&passedfn[length - strlen(filekind[i].extension)], filekind[i].extension)
            )
            {   found = TRUE;
                break; // for speed
        }   }
        if (!found)
        {   strcat(passedfn, extension); // add extension
}   }   }

EXPORT ULONG crc32(UBYTE* address, int thesize)
{   ULONG crc = 0xFFFFFFFF;
    int   i;

    for (i = 0; i < thesize; i++)
    {   crc = ((crc >> 8) & 0x00FFFFFF) ^ crc32_table[(         crc        ^ *(address + i)) & 0xFF];
    }
    return crc ^ 0xFFFFFFFF;
}

EXPORT ULONG crc32_cont(UBYTE* address, int thesize, ULONG oldcrc)
{   ULONG crc;
    int   i;

    crc = oldcrc ^ 0xFFFFFFFF;
    for (i = 0; i < thesize; i++)
    {   crc = ((crc >> 8) & 0x00FFFFFF) ^ crc32_table[(         crc        ^ *(address + i)) & 0xFF];
    }
    return crc ^ 0xFFFFFFFF;
}

/* EXPORT UHUGE crc64(UBYTE* address, int thesize)
{   UHUGE crc = 0xFFFFFFFFFFFFFFFF;
    int   i;

    for (i = 0; i < thesize; i++)
    {   crc =  (crc << 8)               ^ crc64_table[((ULONG) (crc >> 56) ^ *(address + i)) & 0xFF];
    }
    return crc ^ 0xFFFFFFFFFFFFFFFF;
} */
EXPORT void crc64(UBYTE* address, int thesize)
{   ULONG index;
    int   i;

    crc64_h = 0xFFFFFFFF;
    crc64_l = 0xFFFFFFFF;
    for (i = 0; i < thesize; i++)
    {   index   = ((crc64_h >> 24) ^ *(address + i)) & 0xFF;
        crc64_h = ((crc64_h << 8) | (crc64_l >> 24)) ^ crc64_table_h[index];
        crc64_l =  (crc64_l << 8)                    ^ crc64_table_l[index];
    }
    crc64_h ^= 0xFFFFFFFF;
    crc64_l ^= 0xFFFFFFFF;
}

EXPORT void generate_crc32table(void)
{   ULONG crc;
    int   i, j;

    for (i = 0; i < 256; i++)
    {   crc = (ULONG) i;
        for (j = 8; j > 0; j--)
        {   if (crc & 1)
            {   crc = (crc >> 1) ^ 0xEDB88320L;
            } else
            {   crc >>= 1;
        }   }
        crc32_table[i] = crc;
}   }

EXPORT void hex1(char* out, UBYTE value)
{   ULONG digit;

    digit = value / 16;
    if (digit <= 9)
    {   *(out) = (char) ('0' + digit);
    } else
    {   *(out) = (char) ('A' + digit - 10);
    }

    digit = value % 16;
    if (digit <= 9)
    {   *(out + 1) = (char) ('0' + digit);
    } else
    {   *(out + 1) = (char) ('A' + digit - 10);
    }

    *(out + 2) = EOS;
}

EXPORT void hex2(char* out, UWORD value)
{   ULONG digit;

    // This routine deliberately doesn't append an EOS, for speed.

    digit = value / 4096;
    if (digit <= 9)
    {   *(out) = (char) ('0' + digit);
    } else
    {   *(out) = (char) ('A' + digit - 10);
    }

    digit = (value % 4096) / 256;
    if (digit <= 9)
    {   *(out + 1) = (char) ('0' + digit);
    } else
    {   *(out + 1) = (char) ('A' + digit - 10);
    }

    digit = (value % 256) / 16;
    if (digit <= 9)
    {   *(out + 2) = (char) ('0' + digit);
    } else
    {   *(out + 2) = (char) ('A' + digit - 10);
    }

    digit = value % 16;
    if (digit <= 9)
    {   *(out + 3) = (char) ('0' + digit);
    } else
    {   *(out + 3) = (char) ('A' + digit - 10);
}   }

EXPORT void engine_setup(void)
{   int i, j;

    srand((unsigned int) time(NULL));

    generate_crc32table();

    Clock2650Str[COMMASTRLEN - 1] =
    FrameStr[    COMMASTRLEN - 1] = EOS;
    for (i = 0; i < MONITORGADS; i++)
    {   monitor[i].string[2] =
        monitor[i].string[4] = EOS; // both are needed
    }
    for (i = 0; i < MEMGADGETS; i++)
    {   MemoryStr[i][2] = EOS;
    }
    for (i = 0; i < SUBWINDOWS; i++)
    {   SubWindowPtr[i] = NULL;
        need[i]         = FALSE;
    }
    StatusText[0][0] =
    StatusText[1][0] =
    StatusText[2][0] =
    StatusText[3][0] = EOS;

    for (i = 0; i < 2; i++)
    {   printer[i].condensed = TRUE;
        printer[i].ribbon = 0; // means black
    }

    for (i = 0; i < 2; i++)
    {   for (j = 0; j < NUMKEYS; j++)
        {   keypads[i][j] = default_keypads[i][j];
    }   }
    for (i = 0; i < 4; i++)
    {   console[i] = default_console[i];
    }

    pgmtext[0] = EOS;
    for (i = 0; i < GAMEINFOLINES; i++)
    {   strcpy((char*) autotext[i], "-");
    }

    for (i = 0; i < HISTORYLINES; i++)
    {   olduserinput[i][0] = EOS;
    }

    for (i = 0; i < HISCORES; i++)
    {   hiscore[i].score          =
        hiscore[i].played         =
        hiscore[i].playedmicro    = 0;
        hiscore[i].thedatetime[0] =
        hiscore[i].username[0]    = '-';
        hiscore[i].thedatetime[1] =
        hiscore[i].username[1]    = EOS;
    }

    for (i = 0; i < SCANCODES; i++)
    {   keyframes[i] = 0;
    }

    for (i = 0; i < TOTALCHANNELS; i++)
    {   guestplaying[i]   = FALSE;
    }

    for (i = 0; i < GUESTCHANNELS; i++)
    {   oldguestpitch[i]   =
        oldguestvolume[i]  = 0;
        MIDITrackHandle[i] =
        SMUSTrackHandle[i] = NULL;
    }

    strcpy((char*) path_disks      , PATH_DISKS);
    strcpy((char*) path_games      , PATH_GAMES);
    strcpy((char*) path_projects   , PATH_PROJECTS);
    strcpy((char*) path_screenshots, PATH_SCREENSHOTS);
    strcpy((char*) path_scripts    , PATH_SCRIPTS);
    strcpy((char*) path_tapes      , PATH_TAPES);
#ifdef WIN32
    strcpy((char*) path_bkgrnd     , PATH_BKGRNDS);

    strcpy(recent[0], "&1 ");
    strcpy(recent[1], "&2 ");
    strcpy(recent[2], "&3 ");
    strcpy(recent[3], "&4 ");
    strcpy(recent[4], "&5 ");
    strcpy(recent[5], "&6 ");
    strcpy(recent[6], "&7 ");
    strcpy(recent[7], "&8 ");
#endif
#ifdef AMIGA
    strcpy((char*) recent[0], "1: ");
    strcpy((char*) recent[1], "2: ");
    strcpy((char*) recent[2], "3: ");
    strcpy((char*) recent[3], "4: ");
    strcpy((char*) recent[4], "5: ");
    strcpy((char*) recent[5], "6: ");
    strcpy((char*) recent[6], "7: ");
    strcpy((char*) recent[7], "8: ");
#endif
    recentmemmap[0] =
    recentmemmap[1] =
    recentmemmap[2] =
    recentmemmap[3] =
    recentmemmap[4] =
    recentmemmap[5] =
    recentmemmap[6] =
    recentmemmap[7] = -1;

    for (i = 32; i <= 255; i++)
    {   asciiname_short[i][0] =
        asciiname_short[i][2] = QUOTE;
#ifdef WIN32
        asciiname_short[i][1] = (i <= 127) ? i : '.';
#endif
#ifdef AMIGA
        asciiname_short[i][1] = i;
#endif
        asciiname_short[i][3] = EOS;
    }
    strcpy((char*) asciiname_short[127], "DEL");
    strcpy((char*) asciiname_short[149], "MW" ); // because 'ò' emits a beep, under Windows 7 at least
    strcpy((char*) asciiname_short[164], "CQ" ); // curved quote. otherwise a linefeed, under Windows 8.1 at least
    strcpy((char*) asciiname_short[165], "ELL"); // ellipsis. otherwise a linefeed, under Windows 8.1 at least
    strcpy((char*) asciiname_short[NC ], "NC" ); // user should never see this
    strcpy((char*) asciiname_short[BRK], "Brk");
    strcpy((char*) asciiname_short[HI ], "HI" );
    for (i = 0; i < 259; i++)
    {   strcpy((char*) asciiname_long[i], (const char*) asciiname_short[i]);
        if (i < 256)
        {   sprintf((char*) ENDOF(asciiname_long[i]), " ($%02X)", i);
        } else
        {   strcat((char*) asciiname_long[i], " (-)");
    }   }

    for (i = 0; i < MEMMAPS; i++)
    {   memmapinfo[i].channels = 0;
        for (j = 0; j < TOTALCHANNELS; j++)
        {   if (memmapinfo[i].channelused[j] == 1)
            {   memmapinfo[i].channels++;
    }   }   }

    cpu_setup();
    gfx_setup();
}

EXPORT void project_open(void)
{   FLAG found;
    int  i,
         length,
         rc;

    if (crippled)
    {   return;
    }

    macro_stop();
    // asl() turns sound off and on for us
    strcpy((char*) temppath_games, (const char*) path_games);
    rc = asl
    (   (STRPTR) LLL(
            MSG_ENGINE_OPEN,
            "Open Game/State/Recording/Tape/Disk" // perhaps append "/Bitmap" as well on WinArcadia
        ),
        PATTERN_OPEN,
        FALSE,
        (STRPTR) path_games,
        (STRPTR) file_game,
        (STRPTR) fn_game
    );
    if (rc == 5)
    {   audit(FALSE);
    }
    if (rc != EXIT_SUCCESS)
    {   return;
    }

    length = (int) strlen((const char*) fn_game);
    found = FALSE;
    if (length)
    {   for (i = 0; i < length; i++)
        {   if (fn_game[i] == '.')
            {   found = TRUE;
                break;
    }   }   }

    if (found)
    {   if (!((rc = engine_load(FALSE))))
        {   beep();
            return;
    }   }
    else
    {   switch (machine)
        {
        case ELEKTOR:
            fixextension(filekind[KIND_TVC].extension, (STRPTR) fn_game, FALSE, "");
            if (!((rc = engine_load(FALSE))))
            {   fixextension(filekind[KIND_ZIP].extension, (STRPTR) fn_game, TRUE, "");
                if (!engine_load(FALSE))
                {   beep();
                    return;
            }   }
        acase PIPBUG:
        case BINBUG:
        case CD2650:
        case MIKIT:
        case SELBST:
            fixextension(filekind[KIND_AOF].extension, (STRPTR) fn_game, FALSE, "");
            if (!((rc = engine_load(FALSE))))
            {   fixextension(filekind[KIND_ZIP].extension, (STRPTR) fn_game, TRUE, "");
                if (!engine_load(FALSE))
                {   beep();
                    return;
            }   }
        adefault: // eg. ARCADIA, INTERTON, INSTRUCTOR, coin-ops, PHUNSY
            fixextension(filekind[KIND_BIN].extension, (STRPTR) fn_game, FALSE, "");
            if (!((rc = engine_load(FALSE))))
            {   fixextension(filekind[KIND_ZIP].extension, (STRPTR) fn_game, TRUE, "");
                if (!engine_load(FALSE))
                {   beep();
                    return;
    }   }   }   }

    if (rc == 1 || rc == 3)
    {   break_pathname((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game);
        add_recent();
    }
    if (stricmp((const char*) temppath_games, (const char*) path_games)) // if directory path has changed
    {   audit(FALSE);
    }
#ifdef AMIGA
    else
    {   if (showsidebars[wsm])
        {   DISCARD SetGadgetAttrs
            (   gadgets[GID_MA_LB1],
                MainWindowPtr,
                NULL,
                LISTBROWSER_Selected, (ULONG) ~0,
            TAG_END); // this autorefreshes
        }

        currentgame = -1;
    }
#endif
// how to do this for IBM-PC?

    setselection();

    sound_off(FALSE);
    sound_on(TRUE);
}

/* This routine returns its results in ax[dest] and ay[dest]. */
EXPORT void engine_dopaddle(int source, int dest)
{   FLAG kybdonly;

    // centred value of >= $70 is needed for Interton SUPERINV
    // centred value of <= $6F is needed for Elektor JOYSTICK
    // very low left X-value is needed for Interton INVADERS
    // 2636 PVI datasheet recommends 20 and 225 as min and max values

    dx[dest] = dy[dest] = 0;
    kybdonly = system_dopaddle(source, dest);
    if
    (    KeyDown(keypads[source][24])
     && !KeyDown(keypads[source][25])
     && !KeyDown(keypads[source][26])
     && !KeyDown(keypads[source][27])
    )
    {   dx[dest] =
        dy[dest] = -1;
    } elif
    (   !KeyDown(keypads[source][24])
     &&  KeyDown(keypads[source][25])
     && !KeyDown(keypads[source][26])
     && !KeyDown(keypads[source][27])
    )
    {   dx[dest] =  1;
        dy[dest] = -1;
    } elif
    (   !KeyDown(keypads[source][24])
     && !KeyDown(keypads[source][25])
     &&  KeyDown(keypads[source][26])
     && !KeyDown(keypads[source][27])
    )
    {   dx[dest] = -1;
        dy[dest] =  1;
    } elif
    (   !KeyDown(keypads[source][24])
     && !KeyDown(keypads[source][25])
     && !KeyDown(keypads[source][26])
     &&  KeyDown(keypads[source][27])
    )
    {   dx[dest] =
        dy[dest] =  1;
    } else
    {   if   ( KeyDown(keypads[source][17]) && !KeyDown(keypads[source][18])) dy[dest] = -1;
        elif (!KeyDown(keypads[source][17]) &&  KeyDown(keypads[source][18])) dy[dest] =  1;
        if   ( KeyDown(keypads[source][19]) && !KeyDown(keypads[source][20])) dx[dest] = -1;
        elif (!KeyDown(keypads[source][19]) &&  KeyDown(keypads[source][20])) dx[dest] =  1;
    }

    if   (wheremouse == 28 + (dest * 4)) dy[dest] = -1;
    elif (wheremouse == 29 + (dest * 4)) dx[dest] = -1;
    elif (wheremouse == 30 + (dest * 4)) dy[dest] =  1;
    elif (wheremouse == 31 + (dest * 4)) dx[dest] =  1;

    if
    (   dest == 0
     && p1rumble
     && useff[source]
#ifdef WIN32
     && (hostcontroller[source] == CONTROLLER_1STDJOY || hostcontroller[source] == CONTROLLER_2NDDJOY)
#endif
#ifdef AMIGA
     && (hostcontroller[source] == CONTROLLER_1STAPAD || hostcontroller[source] == CONTROLLER_2NDAPAD)
#endif
    )
    {   ff_on(guest_to_host_joy(source), p1rumble);
    } elif
    (   dest == 1
     && p2rumble
     && useff[source]
#ifdef WIN32
     && (hostcontroller[source] == CONTROLLER_1STDJOY || hostcontroller[source] == CONTROLLER_2NDDJOY)
#endif
#ifdef AMIGA
     && (hostcontroller[source] == CONTROLLER_1STAPAD || hostcontroller[source] == CONTROLLER_2NDAPAD)
#endif
    )
    {   ff_on(guest_to_host_joy(source), p2rumble);
    } else
    {   ff_decay(guest_to_host_joy(source));
    }

    if (dx[dest] == 0 && dy[dest] == 0 && !kybdonly)
    {   return;
    }

    if (analog)
    {   // host is digital or keyboard, guest is analog
        if   (dx[dest] == -1) { ax[dest] -= sensitivity[source]; if (ax[dest] <   0) ax[dest] =   0; }
        elif (dx[dest] ==  1) { ax[dest] += sensitivity[source]; if (ax[dest] > 255) ax[dest] = 255; }
        elif (springloaded  )   ax[dest] =  machines[machine].digipos[1];

        if   (dy[dest] == -1) { ay[dest] -= sensitivity[source]; if (ay[dest] <   0) ay[dest] =   0; }
        elif (dy[dest] ==  1) { ay[dest] += sensitivity[source]; if (ay[dest] > 255) ay[dest] = 255; }
        elif (springloaded  )   ay[dest] =  machines[machine].digipos[1];
    } else
    {   // host is digital or keyboard, guest is digital
        ax[dest] = machines[machine].digipos[dx[dest] + 1];
        ay[dest] = machines[machine].digipos[dy[dest] + 1];
}   }

EXPORT void update_controls(void)
{   FAST int i;

    if (!SubWindowPtr[SUBWINDOW_CONTROLS])
    {   return;
    }

    switch (machine)
    {
    case ARCADIA:
        arcadia_update_miniglow();
        newkeys[0]  = memory[A_CONSOLE]      & 0x01;
        newkeys[1]  = memory[A_CONSOLE]      & 0x02;
        newkeys[2]  = memory[A_CONSOLE]      & 0x04;
        newkeys[3]  = 0;
        newkeys[4]  = memory[A_P1LEFTKEYS]   & 0x08;
        newkeys[5]  = memory[A_P1LEFTKEYS]   & 0x04;
        newkeys[6]  = memory[A_P1LEFTKEYS]   & 0x02;
        newkeys[7]  = memory[A_P1LEFTKEYS]   & 0x01;
        newkeys[8]  =
        newkeys[36] =
        newkeys[37] = memory[A_P1MIDDLEKEYS] & 0x08;
        newkeys[9]  = memory[A_P1MIDDLEKEYS] & 0x04;
        newkeys[10] = memory[A_P1MIDDLEKEYS] & 0x02;
        newkeys[11] = memory[A_P1MIDDLEKEYS] & 0x01;
        newkeys[12] = memory[A_P1RIGHTKEYS]  & 0x08;
        newkeys[13] = memory[A_P1RIGHTKEYS]  & 0x04;
        newkeys[14] = memory[A_P1RIGHTKEYS]  & 0x02;
        newkeys[15] = memory[A_P1RIGHTKEYS]  & 0x01;
        newkeys[16] = memory[A_P2LEFTKEYS]   & 0x08;
        newkeys[17] = memory[A_P2LEFTKEYS]   & 0x04;
        newkeys[18] = memory[A_P2LEFTKEYS]   & 0x02;
        newkeys[19] = memory[A_P2LEFTKEYS]   & 0x01;
        newkeys[20] =
        newkeys[38] =
        newkeys[39] = memory[A_P2MIDDLEKEYS] & 0x08;
        newkeys[21] = memory[A_P2MIDDLEKEYS] & 0x04;
        newkeys[22] = memory[A_P2MIDDLEKEYS] & 0x02;
        newkeys[23] = memory[A_P2MIDDLEKEYS] & 0x01;
        newkeys[24] = memory[A_P2RIGHTKEYS]  & 0x08;
        newkeys[25] = memory[A_P2RIGHTKEYS]  & 0x04;
        newkeys[26] = memory[A_P2RIGHTKEYS]  & 0x02;
        newkeys[27] = memory[A_P2RIGHTKEYS]  & 0x01;
        newkeys[40] = memory[A_P1PALLADIUM]  & 0x01;
        newkeys[41] = memory[A_P1PALLADIUM]  & 0x02;
        newkeys[42] = memory[A_P1PALLADIUM]  & 0x04;
        newkeys[43] = memory[A_P1PALLADIUM]  & 0x08;
        newkeys[44] = memory[A_P2PALLADIUM]  & 0x01;
        newkeys[45] = memory[A_P2PALLADIUM]  & 0x02;
        newkeys[46] = memory[A_P2PALLADIUM]  & 0x04;
        newkeys[47] = memory[A_P2PALLADIUM]  & 0x08;
    acase INTERTON:
    case ELEKTOR:
        newkeys[0]  = memory[IE_CONSOLE]      & 0x40;
        newkeys[1]  = memory[IE_CONSOLE]      & 0x80;
        newkeys[2]  = memory[IE_CONSOLE]      & 0x20;
        newkeys[3]  = memory[IE_CONSOLE]      & 0x10;
        newkeys[4]  = memory[IE_P1LEFTKEYS]   & 0x80;
        newkeys[5]  = memory[IE_P1LEFTKEYS]   & 0x40;
        newkeys[6]  = memory[IE_P1LEFTKEYS]   & 0x20;
        newkeys[7]  = memory[IE_P1LEFTKEYS]   & 0x10;
        newkeys[8]  =
        newkeys[36] =
        newkeys[37] = memory[IE_P1MIDDLEKEYS] & 0x80;
        newkeys[9]  = memory[IE_P1MIDDLEKEYS] & 0x40;
        newkeys[10] = memory[IE_P1MIDDLEKEYS] & 0x20;
        newkeys[11] = memory[IE_P1MIDDLEKEYS] & 0x10;
        newkeys[12] = memory[IE_P1RIGHTKEYS]  & 0x80;
        newkeys[13] = memory[IE_P1RIGHTKEYS]  & 0x40;
        newkeys[14] = memory[IE_P1RIGHTKEYS]  & 0x20;
        newkeys[15] = memory[IE_P1RIGHTKEYS]  & 0x10;
        newkeys[16] = memory[IE_P2LEFTKEYS]   & 0x80;
        newkeys[17] = memory[IE_P2LEFTKEYS]   & 0x40;
        newkeys[18] = memory[IE_P2LEFTKEYS]   & 0x20;
        newkeys[19] = memory[IE_P2LEFTKEYS]   & 0x10;
        newkeys[20] =
        newkeys[38] =
        newkeys[39] = memory[IE_P2MIDDLEKEYS] & 0x80;
        newkeys[21] = memory[IE_P2MIDDLEKEYS] & 0x40;
        newkeys[22] = memory[IE_P2MIDDLEKEYS] & 0x20;
        newkeys[23] = memory[IE_P2MIDDLEKEYS] & 0x10;
        newkeys[24] = memory[IE_P2RIGHTKEYS]  & 0x80;
        newkeys[25] = memory[IE_P2RIGHTKEYS]  & 0x40;
        newkeys[26] = memory[IE_P2RIGHTKEYS]  & 0x20;
        newkeys[27] = memory[IE_P2RIGHTKEYS]  & 0x10;
    acase PIPBUG:
        pipbug_update_miniglows();
        for (i = 0; i < KEYS; i++)
        {   newkeys[i] = FALSE;
        }
        if (wheremouse != -1)
        {   newkeys[wheremouse] = TRUE;
        }
        switch (pipbug_vdu)
        {
        case VDU_RADIOBULLETIN:
            // 1st row
            newkeys[ 0] |=  KeyDown(SCAN_SPACEBAR  );
            newkeys[ 1] |=  KeyDown(SCAN_A1        ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // !
            newkeys[ 2] |=  KeyDown(SCAN_APOSTROPHE) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // "
            newkeys[ 3] |=  KeyDown(SCAN_A3        ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // #
            newkeys[ 4] |=  KeyDown(SCAN_A4        ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // $
            newkeys[ 5] |=  KeyDown(SCAN_A5        ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // %
            newkeys[ 6] |=  KeyDown(SCAN_A7        ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // &
            newkeys[ 7] |=  KeyDown(SCAN_APOSTROPHE) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // '
#ifdef WIN32
            newkeys[ 8] |=  KeyDown(SCAN_A9        ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT));                     // (
            newkeys[ 9] |=  KeyDown(SCAN_A0        ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT));                     // )
#endif
#ifdef AMIGA
            newkeys[ 8] |= (KeyDown(SCAN_A9        ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_OP); // (
            newkeys[ 9] |= (KeyDown(SCAN_A0        ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_CP); // )
#endif
            newkeys[10] |= (KeyDown(SCAN_A8        ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_AS); // *
            newkeys[11] |= (KeyDown(SCAN_EQUALS    ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_PL); // +
            newkeys[12] |=  KeyDown(SCAN_COMMA     ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))                    ; // ,
            newkeys[13] |= (KeyDown(SCAN_AMINUS    ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_MI); // -
            newkeys[14] |= (KeyDown(SCAN_FULLSTOP  ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_ND); // .
            newkeys[15] |= (KeyDown(SCAN_SLASH     ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_SL); // /
            newkeys[16] |= (KeyDown(SCAN_J         ) &  (KeyDown(SCAN_LCTRL ) | KeyDown(SCAN_RCTRL ))) | (KeyDown(SCAN_AE)
                                                                                                       |  KeyDown(SCAN_NE)); // $0A (Ctrl-J) (LF)
            newkeys[17] |=  KeyDown(SCAN_K         ) &  (KeyDown(SCAN_LCTRL ) | KeyDown(SCAN_RCTRL ))                    ; // $0B (Ctrl-K) (VT)
            newkeys[18] |=  KeyDown(SCAN_M         ) &  (KeyDown(SCAN_LCTRL ) | KeyDown(SCAN_RCTRL ))                    ; // $0D (Ctrl-M) (CR)
            newkeys[19] |=  KeyDown(SCAN_G         ) &  (KeyDown(SCAN_LCTRL ) | KeyDown(SCAN_RCTRL ))                    ; // $07 (Ctrl-G) (BEL)
            // 2nd row
            newkeys[20] |= (KeyDown(SCAN_A0        ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_N0);
            newkeys[21] |= (KeyDown(SCAN_A1        ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_N1);
            newkeys[22] |= (KeyDown(SCAN_A2        ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_N2);
            newkeys[23] |= (KeyDown(SCAN_A3        ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_N3);
            newkeys[24] |= (KeyDown(SCAN_A4        ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_N4);
            newkeys[25] |= (KeyDown(SCAN_A5        ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_N5);
            newkeys[26] |= (KeyDown(SCAN_A6        ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_N6);
            newkeys[27] |= (KeyDown(SCAN_A7        ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_N7);
            newkeys[28] |= (KeyDown(SCAN_A8        ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_N8);
            newkeys[29] |= (KeyDown(SCAN_A9        ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))) | KeyDown(SCAN_N9);
            newkeys[30] |=  KeyDown(SCAN_SEMICOLON ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // :
            newkeys[31] |=  KeyDown(SCAN_SEMICOLON ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // ;
            newkeys[32] |=  KeyDown(SCAN_COMMA     ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // <
            newkeys[33] |=  KeyDown(SCAN_EQUALS    ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // =
            newkeys[34] |=  KeyDown(SCAN_FULLSTOP  ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // >
            newkeys[35] |=  KeyDown(SCAN_SLASH     ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // ?
            newkeys[36] |=  KeyDown(SCAN_Z         ) &  (KeyDown(SCAN_LCTRL ) | KeyDown(SCAN_RCTRL )); // $1A (Ctrl-Z) (SUB)
            newkeys[37] |=  KeyDown(SCAN_OB        ) &  (KeyDown(SCAN_LCTRL ) | KeyDown(SCAN_RCTRL )); // $1B (Ctrl-[) (ESC)
            newkeys[38] |=  KeyDown(SCAN_CB        ) &  (KeyDown(SCAN_LCTRL ) | KeyDown(SCAN_RCTRL )); // $1D (Ctrl-]) (GS)
            newkeys[39] |=  KeyDown(SCAN_AMINUS    ) & ((KeyDown(SCAN_LCTRL ) | KeyDown(SCAN_RCTRL ))
                                                     &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT))); // $1F (Ctrl-_) (US)
            // 3rd row
            newkeys[40] |=  KeyDown(SCAN_A2        ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // @
            newkeys[41] |=  KeyDown(SCAN_A);
            newkeys[42] |=  KeyDown(SCAN_B);
            newkeys[43] |=  KeyDown(SCAN_C);
            newkeys[44] |=  KeyDown(SCAN_D);
            newkeys[45] |=  KeyDown(SCAN_E);
            newkeys[46] |=  KeyDown(SCAN_F);
            newkeys[47] |=  KeyDown(SCAN_G);
            newkeys[48] |=  KeyDown(SCAN_H);
            newkeys[49] |=  KeyDown(SCAN_I);
            if (lowercase && !capslock)
            {   newkeys[50] |=  KeyDown(SCAN_J) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // 'J'
                newkeys[51] |=  KeyDown(SCAN_K) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // 'K'
                newkeys[53] |=  KeyDown(SCAN_M) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // 'M'
                newkeys[55] |=  KeyDown(SCAN_O) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // 'O'
                newkeys[56] |=  KeyDown(SCAN_J) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // $6A ('j')
                newkeys[57] |=  KeyDown(SCAN_K) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // $6B ('k')
                newkeys[58] |=  KeyDown(SCAN_M) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // $6D ('m')
                newkeys[59] |=  KeyDown(SCAN_O) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // $6F ('o')
            } else
            {   // clicking on the key with mouse will still generate the character but won't invert it visually
                newkeys[50] |=  KeyDown(SCAN_J);
                newkeys[51] |=  KeyDown(SCAN_K);
                newkeys[53] |=  KeyDown(SCAN_M);
                newkeys[55] |=  KeyDown(SCAN_O);
                newkeys[56] = newkeys[57] = newkeys[58] = newkeys[59] = FALSE;
            }
            newkeys[52] |=  KeyDown(SCAN_L);
            newkeys[54] |=  KeyDown(SCAN_N);
            // 4th row
            newkeys[60] |=  KeyDown(SCAN_P);
            newkeys[61] |=  KeyDown(SCAN_Q);
            newkeys[62] |=  KeyDown(SCAN_R);
            newkeys[63] |=  KeyDown(SCAN_S);
            newkeys[64] |=  KeyDown(SCAN_T);
            newkeys[65] |=  KeyDown(SCAN_U);
            newkeys[66] |=  KeyDown(SCAN_V);
            newkeys[67] |=  KeyDown(SCAN_W);
            newkeys[68] |=  KeyDown(SCAN_X);
            newkeys[69] |=  KeyDown(SCAN_Y);
            if (lowercase && !capslock)
            {   newkeys[70] |=  KeyDown(SCAN_Z     ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // 'Z'
                newkeys[76] |=  KeyDown(SCAN_Z     ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // $7A ('z')
            } else
            {   newkeys[70] |=  KeyDown(SCAN_Z);
                newkeys[76] =   FALSE;
            }
            newkeys[71] |=  KeyDown(SCAN_OB        ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // [
            newkeys[72] |=  KeyDown(SCAN_BACKSLASH ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // backslash
            newkeys[73] |=  KeyDown(SCAN_CB        ) & !(KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // ]
            newkeys[74] |=  KeyDown(SCAN_A6        ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // ^
            newkeys[75] |=  KeyDown(SCAN_AMINUS    ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // _
            newkeys[77] |=  KeyDown(SCAN_OB        ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // $7B ('{')
            newkeys[78] |=  KeyDown(SCAN_CB        ) &  (KeyDown(SCAN_LSHIFT) | KeyDown(SCAN_RSHIFT)); // $7D ('}')
            newkeys[79] |=  KeyDown(SCAN_DEL       );
        acase VDU_VT100:
            // 1st row
            newkeys[ 0] |= KeyDown(SCAN_UP);
            newkeys[ 1] |= KeyDown(SCAN_DOWN);
            newkeys[ 2] |= KeyDown(SCAN_LEFT);
            newkeys[ 3] |= KeyDown(SCAN_RIGHT);
            // 2nd row
            newkeys[ 4] |= KeyDown(SCAN_ESCAPE);
            newkeys[ 5] |= KeyDown(SCAN_A1);
            newkeys[ 6] |= KeyDown(SCAN_A2);
            newkeys[ 7] |= KeyDown(SCAN_A3);
            newkeys[ 8] |= KeyDown(SCAN_A4);
            newkeys[ 9] |= KeyDown(SCAN_A5);
            newkeys[10] |= KeyDown(SCAN_A6);
            newkeys[11] |= KeyDown(SCAN_A7);
            newkeys[12] |= KeyDown(SCAN_A8) | KeyDown(SCAN_AS);
            newkeys[13] |= KeyDown(SCAN_A9);
            newkeys[14] |= KeyDown(SCAN_A0);
            newkeys[15] |= KeyDown(SCAN_AMINUS);
            newkeys[16] |= KeyDown(SCAN_EQUALS) | KeyDown(SCAN_SL);
            newkeys[17] |= KeyDown(SCAN_BACKTICK);
            newkeys[18] |= KeyDown(SCAN_BACKSPACE);
            //      19 is Break
            newkeys[20] |= KeyDown(SCAN_N7);
            newkeys[21] |= KeyDown(SCAN_N8);
            newkeys[22] |= KeyDown(SCAN_N9);
            newkeys[23] |= KeyDown(SCAN_MI);
            // 3rd row
            newkeys[24] |= KeyDown(SCAN_TAB);
            newkeys[25] |= KeyDown(SCAN_Q);
            newkeys[26] |= KeyDown(SCAN_W);
            newkeys[27] |= KeyDown(SCAN_E);
            newkeys[28] |= KeyDown(SCAN_R);
            newkeys[29] |= KeyDown(SCAN_T);
            newkeys[30] |= KeyDown(SCAN_Y);
            newkeys[31] |= KeyDown(SCAN_U);
            newkeys[32] |= KeyDown(SCAN_I);
            newkeys[33] |= KeyDown(SCAN_O);
            newkeys[34] |= KeyDown(SCAN_P);
            newkeys[35] |= KeyDown(SCAN_OB);
            newkeys[36] |= KeyDown(SCAN_CB);
            newkeys[37] |= KeyDown(SCAN_AE);
            newkeys[38] |= KeyDown(SCAN_DEL);
            newkeys[39] |= KeyDown(SCAN_N4);
            newkeys[40] |= KeyDown(SCAN_N5);
            newkeys[41] |= KeyDown(SCAN_N6);
            newkeys[42] |= KeyDown(SCAN_COMMA);
            // 4th row
#ifdef WIN32
            newkeys[43] |= KeyDown(SCAN_LCTRL) | KeyDown(SCAN_RCTRL);
#endif
#ifdef AMIGA
            newkeys[43] |= KeyDown(SCAN_CTRL);
#endif
            //      44 is Caps Lock
            newkeys[45] |= KeyDown(SCAN_A);
            newkeys[46] |= KeyDown(SCAN_S);
            newkeys[47] |= KeyDown(SCAN_D);
            newkeys[48] |= KeyDown(SCAN_F);
            newkeys[49] |= KeyDown(SCAN_G);
            newkeys[50] |= KeyDown(SCAN_H);
            newkeys[51] |= KeyDown(SCAN_J);
            newkeys[52] |= KeyDown(SCAN_K);
            newkeys[53] |= KeyDown(SCAN_L);
            newkeys[54] |= KeyDown(SCAN_SEMICOLON);
            newkeys[55] |= KeyDown(SCAN_APOSTROPHE);
            newkeys[56] |= KeyDown(SCAN_BACKSLASH);
            newkeys[57] |= KeyDown(SCAN_N1);
            newkeys[58] |= KeyDown(SCAN_N2);
            newkeys[59] |= KeyDown(SCAN_N3);
            newkeys[60] |= KeyDown(SCAN_NE);
            // 5th row
#if defined(WIN32) || defined(__MORPHOS__)
            newkeys[61] |= KeyDown(SCAN_SCRLK);
#endif
            newkeys[62] |= KeyDown(SCAN_LSHIFT);
            newkeys[63] |= KeyDown(SCAN_Z);
            newkeys[64] |= KeyDown(SCAN_X);
            newkeys[65] |= KeyDown(SCAN_C);
            newkeys[66] |= KeyDown(SCAN_V);
            newkeys[67] |= KeyDown(SCAN_B);
            newkeys[68] |= KeyDown(SCAN_N);
            newkeys[69] |= KeyDown(SCAN_M);
            newkeys[70] |= KeyDown(SCAN_COMMA);
            newkeys[71] |= KeyDown(SCAN_FULLSTOP);
            newkeys[72] |= KeyDown(SCAN_SLASH) | KeyDown(SCAN_SL);
            newkeys[73] |= KeyDown(SCAN_RSHIFT);
            //      74 is LF
            newkeys[75] |= KeyDown(SCAN_N0);
            newkeys[76] |= KeyDown(SCAN_ND);
            // 6th row
            newkeys[77] |= KeyDown(SCAN_SPACEBAR);
        adefault:
            // 1st row
            newkeys[ 0] |= KeyDown(SCAN_A1) | KeyDown(SCAN_N1);
            newkeys[ 1] |= KeyDown(SCAN_A2) | KeyDown(SCAN_N2);
            newkeys[ 2] |= KeyDown(SCAN_A3) | KeyDown(SCAN_N3);
            newkeys[ 3] |= KeyDown(SCAN_A4) | KeyDown(SCAN_N4);
            newkeys[ 4] |= KeyDown(SCAN_A5) | KeyDown(SCAN_N5);
            newkeys[ 5] |= KeyDown(SCAN_A6) | KeyDown(SCAN_N6);
            newkeys[ 6] |= KeyDown(SCAN_A7) | KeyDown(SCAN_N7);
            newkeys[ 7] |= KeyDown(SCAN_A8) | KeyDown(SCAN_N8);
            newkeys[ 8] |= KeyDown(SCAN_A9) | KeyDown(SCAN_N9);
            newkeys[ 9] |= KeyDown(SCAN_A0) | KeyDown(SCAN_N0);
            newkeys[10] |= KeyDown(SCAN_AS);
            newkeys[11] |= KeyDown(SCAN_AMINUS) | KeyDown(SCAN_MI) | KeyDown(SCAN_EQUALS);
            newkeys[12] |= KeyDown(SCAN_BACKTICK);
            newkeys[14] |= KeyDown(SCAN_PGUP);
            // 2nd row
            newkeys[15] |= KeyDown(SCAN_ESCAPE);
            newkeys[16] |= KeyDown(SCAN_Q);
            newkeys[17] |= KeyDown(SCAN_W);
            newkeys[18] |= KeyDown(SCAN_E);
            newkeys[19] |= KeyDown(SCAN_R);
            newkeys[20] |= KeyDown(SCAN_T);
            newkeys[21] |= KeyDown(SCAN_Y);
            newkeys[22] |= KeyDown(SCAN_U);
            newkeys[23] |= KeyDown(SCAN_I);
            newkeys[24] |= KeyDown(SCAN_O);
            newkeys[25] |= KeyDown(SCAN_P);
            newkeys[26] |= KeyDown(SCAN_OB);
            newkeys[27] |= KeyDown(SCAN_CB);
            newkeys[28] |= KeyDown(SCAN_DOWN);
            newkeys[29] |= KeyDown(SCAN_AE) | KeyDown(SCAN_NE);
            // 3rd row
#ifdef WIN32
            newkeys[31] |= KeyDown(SCAN_LCTRL) | KeyDown(SCAN_RCTRL);
#endif
#ifdef AMIGA
            newkeys[31] |= KeyDown(SCAN_CTRL);
#endif
            newkeys[32] |= KeyDown(SCAN_A);
            newkeys[33] |= KeyDown(SCAN_S);
            newkeys[34] |= KeyDown(SCAN_D);
            newkeys[35] |= KeyDown(SCAN_F);
            newkeys[36] |= KeyDown(SCAN_G);
            newkeys[37] |= KeyDown(SCAN_H);
            newkeys[38] |= KeyDown(SCAN_J);
            newkeys[39] |= KeyDown(SCAN_K);
            newkeys[40] |= KeyDown(SCAN_L);
            newkeys[41] |= KeyDown(SCAN_SEMICOLON) | KeyDown(SCAN_PL);
            newkeys[43] |= KeyDown(SCAN_BACKSPACE) | KeyDown(SCAN_DEL);
            newkeys[44] |= KeyDown(SCAN_TAB);
            newkeys[45] |= KeyDown(SCAN_PGDN);
            // 4th row
            newkeys[46] |= KeyDown(SCAN_LSHIFT);
            newkeys[47] |= KeyDown(SCAN_Z);
            newkeys[48] |= KeyDown(SCAN_X);
            newkeys[49] |= KeyDown(SCAN_C);
            newkeys[50] |= KeyDown(SCAN_V);
            newkeys[51] |= KeyDown(SCAN_B);
            newkeys[52] |= KeyDown(SCAN_N);
            newkeys[53] |= KeyDown(SCAN_M);
            newkeys[54] |= KeyDown(SCAN_COMMA);
            newkeys[55] |= KeyDown(SCAN_FULLSTOP) | KeyDown(SCAN_ND);
            newkeys[56] |= KeyDown(SCAN_SLASH) | KeyDown(SCAN_SL);
            newkeys[57] |= KeyDown(SCAN_RSHIFT);
            newkeys[58] |= KeyDown(SCAN_UP);
            // 5th row
            newkeys[60] |= KeyDown(SCAN_SPACEBAR);
        }
    acase BINBUG:
    case SELBST:
    case MIKIT:
    case PHUNSY:
        newkeys[0]  = keys_column[0] & 1; // C/BLANK
        newkeys[1]  = keys_column[0] & 2; // 8/+
        newkeys[2]  = keys_column[0] & 4; // 4/H
        newkeys[3]  = keys_column[0] & 8; // 0/L
        newkeys[4]  = keys_column[1] & 1; // D/R
        newkeys[5]  = keys_column[1] & 2; // 9/G
        newkeys[6]  = keys_column[1] & 4; // 5/P
        newkeys[7]  = keys_column[1] & 8; // 1/S
        newkeys[8]  = keys_column[2] & 1; // E/C
        newkeys[9]  = keys_column[2] & 2; // A/8
        newkeys[10] = keys_column[2] & 4; // 6/4
        newkeys[11] = keys_column[2] & 8; // 2/0
        newkeys[12] = keys_column[3] & 1; // F/D
        newkeys[13] = keys_column[3] & 2; // B/9
        newkeys[14] = keys_column[3] & 4; // 7/5
        newkeys[15] = keys_column[3] & 8; // 3/1
        if (machine != BINBUG)
        {   newkeys[16] = keys_column[4] & 1; // CMD/E
            newkeys[17] = keys_column[4] & 2; // RUN/A
            newkeys[18] = keys_column[4] & 4; // GOTO/6
            newkeys[19] = keys_column[4] & 8; // RST/2
            newkeys[20] = keys_column[5] & 1; // FLAG/F
            newkeys[21] = keys_column[5] & 2; // MON/B
            newkeys[22] = keys_column[5] & 4; // PC/7
            newkeys[23] = keys_column[5] & 8; // NXT/3
        }
    acase INSTRUCTOR:
        si50_update_miniglows();
        newkeys[0]  = keys_column[0] & 0x1;
        newkeys[1]  = keys_column[0] & 0x2;
        newkeys[2]  = keys_column[0] & 0x4;
        newkeys[3]  = keys_column[0] & 0x8;
        newkeys[4]  = keys_column[1] & 0x1;
        newkeys[5]  = keys_column[1] & 0x2;
        newkeys[6]  = keys_column[1] & 0x4;
        newkeys[7]  = keys_column[1] & 0x8;
        newkeys[8]  =
        newkeys[36] =
        newkeys[37] = keys_column[2] & 0x1;
        newkeys[9]  = keys_column[2] & 0x2;
        newkeys[10] = keys_column[2] & 0x4;
        newkeys[11] = keys_column[2] & 0x8;
        newkeys[12] = keys_column[3] & 0x8;
        newkeys[13] = keys_column[3] & 0x4;
        newkeys[14] = keys_column[3] & 0x2;
        newkeys[15] = keys_column[3] & 0x1;
        newkeys[16] = keys_column[4] & 0x8;
        newkeys[17] = keys_column[4] & 0x4;
        newkeys[18] = keys_column[4] & 0x2;
        newkeys[19] = keys_column[4] & 0x1;
        newkeys[20] =
        newkeys[38] =
        newkeys[39] = keys_column[5] & 0x8;
        newkeys[21] = keys_column[5] & 0x4;
        newkeys[22] = keys_column[5] & 0x2;
        newkeys[23] = keys_column[5] & 0x1;
        newkeys[24] = keys_column[6] & 0x8;
        newkeys[25] = keys_column[6] & 0x4;
        newkeys[26] = keys_column[6] & 0x2;
        newkeys[27] = keys_column[6] & 0x1;
    acase MALZAK:
        newkeys[0]  = coinop_button1 ? 1 : 0;
        newkeys[1]  = coinop_1p      ? 1 : 0;
        newkeys[2]  = coinop_2p      ? 1 : 0;
        newkeys[3]  = // 10 03 04
        newkeys[4]  = // 09    05
        newkeys[5]  = // 08 07 06
        newkeys[6]  =
        newkeys[7]  =
        newkeys[8]  =
        newkeys[9]  =
        newkeys[10] = 0;
        if ( coinop_joy1left  &&  coinop_joy1up   && !coinop_joy1right) newkeys[10] = 1; // up-left
        if (!coinop_joy1left  &&  coinop_joy1up   &&  coinop_joy1right) newkeys[ 4] = 1; // up-right
        if (!coinop_joy1left  &&  coinop_joy1up   && !coinop_joy1right) newkeys[ 3] = 1; // up
        if ( coinop_joy1left  &&  coinop_joy1down && !coinop_joy1right) newkeys[ 8] = 1; // down-left
        if (!coinop_joy1left  &&  coinop_joy1down &&  coinop_joy1right) newkeys[ 6] = 1; // down-right
        if (!coinop_joy1left  &&  coinop_joy1down && !coinop_joy1right) newkeys[ 7] = 1; // down
        if ( coinop_joy1left  && !coinop_joy1up   && !coinop_joy1down ) newkeys[ 9] = 1; // left
        if ( coinop_joy1right && !coinop_joy1up   && !coinop_joy1down ) newkeys[ 5] = 1; // right
    acase ZACCARIA:
        if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
        {   newkeys[0]  = coinop_button1                       ? 1 : 0;
            newkeys[1]  = coinop_1p                            ? 1 : 0;
            newkeys[2]  = coinop_2p                            ? 1 : 0;
            newkeys[3]  = coinop_joy1left                      ? 1 : 0;
            newkeys[4]  = coinop_joy1right                     ? 1 : 0;
            newkeys[5]  = coinop_joy1up                        ? 1 : 0;
            newkeys[6]  = coinop_joy1down                      ? 1 : 0;
            newkeys[7]  = coinop_button4                       ? 1 : 0;
            newkeys[8]  = coinop_button2                       ? 1 : 0;
            newkeys[9]  = coinop_button3                       ? 1 : 0;
        } else
        {   // assert(memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA);
            newkeys[0]  = coinop_button1   || coinop_button2   ? 1 : 0;
            newkeys[1]  = coinop_1p                            ? 1 : 0;
            newkeys[2]  = coinop_2p                            ? 1 : 0;
            newkeys[3]  = coinop_joy1left  || coinop_joy2left  ? 1 : 0;
            newkeys[4]  = coinop_joy1right || coinop_joy2right ? 1 : 0;
        }
    acase PONG:
        newkeys[0]  = pong_start    & 1;
        newkeys[1]  = pong_a        & 1;
        newkeys[2]  = pong_b        & 1;
        newkeys[3]  = pong_reset    & 1;
    acase TYPERIGHT:
        newkeys[0]  = console_start;
        newkeys[1]  = console_a;
        newkeys[2]  = console_b;
        newkeys[3]  = console_reset;
        for (i = 4; i <= 51; i++)
        {   newkeys[i] = FALSE;
        }
        if (wheremouse != -1)
        {   newkeys[wheremouse] = TRUE;
        }
        for (i = 0; i < SCANCODES; i++)
        {   if (keyname[i].tr_index != -1 && KeyDown((UWORD) i))
            {   newkeys[keyname[i].tr_index] = TRUE;
        }   }
    acase TWIN:
        for (i = 0; i < 53; i++)
        {   newkeys[i] = FALSE;
        }
        if (wheremouse != -1)
        {   newkeys[wheremouse] = TRUE;
        }
        newkeys[ 0] |= KeyDown(SCAN_A1) | KeyDown(SCAN_N1);
        newkeys[ 1] |= KeyDown(SCAN_A2) | KeyDown(SCAN_N2);
        newkeys[ 2] |= KeyDown(SCAN_A3) | KeyDown(SCAN_N3);
        newkeys[ 3] |= KeyDown(SCAN_A4) | KeyDown(SCAN_N4);
        newkeys[ 4] |= KeyDown(SCAN_A5) | KeyDown(SCAN_N5);
        newkeys[ 5] |= KeyDown(SCAN_A6) | KeyDown(SCAN_N6);
        newkeys[ 6] |= KeyDown(SCAN_A7) | KeyDown(SCAN_N7);
        newkeys[ 7] |= KeyDown(SCAN_A8) | KeyDown(SCAN_N8);
        newkeys[ 8] |= KeyDown(SCAN_A9) | KeyDown(SCAN_N9);
        newkeys[ 9] |= KeyDown(SCAN_A0) | KeyDown(SCAN_N0);
        newkeys[10] |= KeyDown(SCAN_SEMICOLON) | KeyDown(SCAN_AS);
        newkeys[11] |= KeyDown(SCAN_AMINUS) | KeyDown(SCAN_MI) | KeyDown(SCAN_EQUALS);
        newkeys[13] |= KeyDown(SCAN_ESCAPE);
        newkeys[14] |= KeyDown(SCAN_Q);
        newkeys[15] |= KeyDown(SCAN_W);
        newkeys[16] |= KeyDown(SCAN_E);
        newkeys[17] |= KeyDown(SCAN_R);
        newkeys[18] |= KeyDown(SCAN_T);
        newkeys[19] |= KeyDown(SCAN_Y);
        newkeys[20] |= KeyDown(SCAN_U);
        newkeys[21] |= KeyDown(SCAN_I);
        newkeys[22] |= KeyDown(SCAN_O);
        newkeys[23] |= KeyDown(SCAN_P);
        newkeys[24] |= KeyDown(SCAN_AE) | KeyDown(SCAN_NE);
        newkeys[25] |= KeyDown(SCAN_AE) | KeyDown(SCAN_NE);
#ifdef WIN32
        newkeys[26] |= KeyDown(SCAN_LCTRL) | KeyDown(SCAN_RCTRL);
#endif
#ifdef AMIGA
        newkeys[26] |= KeyDown(SCAN_CTRL);
#endif
        newkeys[27] |= KeyDown(SCAN_A);
        newkeys[28] |= KeyDown(SCAN_S);
        newkeys[29] |= KeyDown(SCAN_D);
        newkeys[30] |= KeyDown(SCAN_F);
        newkeys[31] |= KeyDown(SCAN_G);
        newkeys[32] |= KeyDown(SCAN_H);
        newkeys[33] |= KeyDown(SCAN_J);
        newkeys[34] |= KeyDown(SCAN_K);
        newkeys[35] |= KeyDown(SCAN_L);
        newkeys[36] |= KeyDown(SCAN_SEMICOLON) | KeyDown(SCAN_PL);
        newkeys[37] |= KeyDown(SCAN_BACKSPACE) | KeyDown(SCAN_DEL);
        newkeys[40] |= KeyDown(SCAN_LSHIFT);
        newkeys[41] |= KeyDown(SCAN_Z);
        newkeys[42] |= KeyDown(SCAN_X);
        newkeys[43] |= KeyDown(SCAN_C);
        newkeys[44] |= KeyDown(SCAN_V);
        newkeys[45] |= KeyDown(SCAN_B);
        newkeys[46] |= KeyDown(SCAN_N);
        newkeys[47] |= KeyDown(SCAN_M);
        newkeys[48] |= KeyDown(SCAN_COMMA);
        newkeys[49] |= KeyDown(SCAN_FULLSTOP) | KeyDown(SCAN_ND);
        newkeys[50] |= KeyDown(SCAN_SLASH) | KeyDown(SCAN_SL);
        newkeys[51] |= KeyDown(SCAN_RSHIFT);
        newkeys[52] |= KeyDown(SCAN_SPACEBAR);
    }

    if (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR)
    {   if (dx[0] == -1) { newkeys[29] = 1; newkeys[31] = 0; } elif (dx[0] == 1) { newkeys[29] = 0; newkeys[31] = 1; } else { newkeys[29] = newkeys[31] = 0; }
        if (dx[1] == -1) { newkeys[33] = 1; newkeys[35] = 0; } elif (dx[1] == 1) { newkeys[33] = 0; newkeys[35] = 1; } else { newkeys[33] = newkeys[35] = 0; }
        if (dy[0] == -1) { newkeys[28] = 1; newkeys[30] = 0; } elif (dy[0] == 1) { newkeys[28] = 0; newkeys[30] = 1; } else { newkeys[28] = newkeys[30] = 0; }
        if (dy[1] == -1) { newkeys[32] = 1; newkeys[34] = 0; } elif (dy[1] == 1) { newkeys[32] = 0; newkeys[34] = 1; } else { newkeys[32] = newkeys[34] = 0; }
    } elif (machine == PONG)
    {   if (dy[0] == -1) { newkeys[11] = 1; newkeys[12] = 0; } elif (dy[0] == 1) { newkeys[11] = 0; newkeys[12] = 1; } else { newkeys[11] = newkeys[12] = 0; }
        if (dy[1] == -1) { newkeys[13] = 1; newkeys[14] = 0; } elif (dy[1] == 1) { newkeys[13] = 0; newkeys[14] = 1; } else { newkeys[13] = newkeys[14] = 0; }
    }

#ifdef WIN32
    ControlsRastPtr1 = GetDC(GetDlgItem(SubWindowPtr[SUBWINDOW_CONTROLS], IDC_CONTROLS));
    if (machine == PIPBUG)
    {   ControlsRastPtr2 = GetDC(GetDlgItem(SubWindowPtr[SUBWINDOW_CONTROLS], IDC_CONTROLS2));
    }
#endif

#ifdef TESTINVERSIONS
    if (machine == PIPBUG)
    {   if (pipbug_vdu == VDU_VT100)
        {   newkeys[78] = oldkeys[78] ? FALSE : TRUE;
        } elif (pipbug_vdu != VDU_RADIOBULLETIN)
        {   newkeys[61] = oldkeys[61] ? FALSE : TRUE;
    }   }
    for (i = 0; i < machines[machine].keys; i++)
    {   invert(i);
        oldkeys[i] = newkeys[i];
    }
#else
    for (i = 0; i < machines[machine].keys; i++)
    {   if (oldkeys[i] != newkeys[i])
        {   invert(i);
            oldkeys[i] = newkeys[i];
    }   }
#endif

#ifdef WIN32
    ReleaseDC(GetDlgItem(SubWindowPtr[SUBWINDOW_CONTROLS], IDC_CONTROLS), ControlsRastPtr1);
    if (machine == PIPBUG)
    {   ReleaseDC(GetDlgItem(SubWindowPtr[SUBWINDOW_CONTROLS], IDC_CONTROLS2), ControlsRastPtr2);
    }
#endif

    update_controlstip();
}

EXPORT void update_controlstip(void)
{   FAST int   i,
               mousex, mousey;
#ifdef WIN32
    FAST POINT thepoint;
    FAST RECT  therect;
#endif

    if
    (   !SubWindowPtr[SUBWINDOW_CONTROLS]
     || (    machine != ARCADIA
         && !machines[machine].pvis
         &&  machine != BINBUG
         &&  machine != INSTRUCTOR
         &&  machine != PHUNSY
         &&  machine != SELBST
         &&  machine != MIKIT
         &&  machine != PONG
    )   )
    {   return;
    }

#ifdef WIN32
    DISCARD GetCursorPos(&thepoint);
    DISCARD GetWindowRect(GetDlgItem(SubWindowPtr[SUBWINDOW_CONTROLS], IDC_CONTROLS), &therect);
    mousex = thepoint.x - therect.left;
    mousey = thepoint.y - therect.top;
#endif
#ifdef AMIGA
    mousex = (int) SubWindowPtr[SUBWINDOW_CONTROLS]->MouseX - images[whichimagery]->LeftEdge,
    mousey = (int) SubWindowPtr[SUBWINDOW_CONTROLS]->MouseY - images[whichimagery]->TopEdge;
#endif

    for (i = 0; i < machines[machine].keys; i++)
    {   if
        (   mousex >= keyinfo[whichkeyrect][i].left
         && mousex <  keyinfo[whichkeyrect][i].left + keyinfo[whichkeyrect][i].width
         && mousey >= keyinfo[whichkeyrect][i].top
         && mousey <  keyinfo[whichkeyrect][i].top  + keyinfo[whichkeyrect][i].height
        )
        {   if (i == oldcontrolkey)
            {   return;
            }
            generate_controlstip(i);
            oldcontrolkey = i;
            goto DONE;
    }   }
    if (oldcontrolkey != -1)
    {   strcpy((char*) controltip[0], "-");
        strcpy((char*) controltip[1], "-");
        strcpy((char*) controltip[2], "-");
        strcpy((char*) controltip[3], "-");
        strcpy((char*) controltip[4], "-");
        oldcontrolkey = -1;
    }

DONE:
    if (!machines[machine].coinop && machine != BINBUG && machine != PONG)
    {   st_set2(SUBWINDOW_CONTROLS, IDC_CONTROLS_OVERLAY  , controltip[0]);
    }
    st_set2(    SUBWINDOW_CONTROLS, IDC_CONTROLS_GUESTKEY , controltip[1]);
    st_set2(    SUBWINDOW_CONTROLS, IDC_CONTROLS_HOSTKEY  , controltip[2]);
    if (machine == ARCADIA || machines[machine].pvis || machine == PONG)
    {   st_set2(SUBWINDOW_CONTROLS, IDC_CONTROLS_HOSTPAD  , controltip[3]);
        st_set2(SUBWINDOW_CONTROLS, IDC_CONTROLS_HOSTMOUSE, controltip[4]);
}   }

EXPORT void controls_mousedown(int mousex, int mousey)
{   FAST int   i, j;
#ifdef WIN32
    FAST POINT thepoint;
    FAST RECT  therect;
#endif

    if (recmode == RECMODE_PLAY)
    {   return;
    }

#ifdef WIN32
    thepoint.x = mousex;
    thepoint.y = mousey;
    ClientToScreen(SubWindowPtr[SUBWINDOW_CONTROLS], &thepoint);
    if (machine == PIPBUG)
    {   DISCARD GetWindowRect(GetDlgItem(SubWindowPtr[SUBWINDOW_CONTROLS], IDC_CONTROLS2), &therect);
    } else
    {   DISCARD GetWindowRect(GetDlgItem(SubWindowPtr[SUBWINDOW_CONTROLS], IDC_CONTROLS ), &therect);
    }
    mousex = thepoint.x - therect.left;
    mousey = thepoint.y - therect.top;
#endif

    // assert(SubWindowPtr[SUBWINDOW_CONTROLS]);

    wheremouse = -1;
    for (i = 0; i < machines[machine].keys; i++)
    {   if
        (   mousex >= keyinfo[whichkeyrect][i].left
         && mousex <  keyinfo[whichkeyrect][i].left + keyinfo[whichkeyrect][i].width
         && mousey >= keyinfo[whichkeyrect][i].top
         && mousey <  keyinfo[whichkeyrect][i].top  + keyinfo[whichkeyrect][i].height
        )
        {   switch (machine)
            {
            case ARCADIA:
                if (i == 3 || i == 49) // RESET key or power switch
                {   project_reset(TRUE);
                } elif (i == 48) // cartridge slot
                {   project_open();
                } elif (a_keyemul[i].address != 0)
                {   wheremouse = i;
                    newkeys[i] |= memory[a_keyemul[i].mask];
                } elif (i >= 28 && i <= 35) // directions
                {   wheremouse = i;
                }
            acase INTERTON:
                if (i == 3 || i == 41) // RESET key or power switch
                {   project_reset(TRUE);
                } elif (i == 40) // cartridge slot
                {   project_open();
                } elif (i_keyemul[i].address != 0)
                {   wheremouse = i;
                    newkeys[i] |= memory[i_keyemul[i].mask];
                } elif (i >= 28 && i <= 35) // directions
                {   wheremouse = i;
                }
            acase ELEKTOR:
                if (e_keyemul[i].address != 0)
                {   wheremouse = i;
                    newkeys[i] |= memory[e_keyemul[i].mask];
                } elif (i >= 28 && i <= 35) // directions
                {   wheremouse = i;
                }
            acase INSTRUCTOR:
                if (i == 28 || i == 29)
                {   if (SubWindowPtr[SUBWINDOW_DIPS])
                    {   close_subwindow(SUBWINDOW_DIPS);
                    } else
                    {   edit_dips();
                }   }
                elif (i == 30)
                {   if (SubWindowPtr[SUBWINDOW_TAPEDECK])
                    {   close_subwindow(SUBWINDOW_TAPEDECK);
                    } else
                    {   open_tapedeck();
                }   }
                elif (i == 31)
                {   if (SubWindowPtr[SUBWINDOW_OPCODES])
                    {   close_subwindow(SUBWINDOW_OPCODES);
                    } else
                    {   help_opcodes();
                }   }
                else
                {   wheremouse = i;
                    newkeys[i] |= keys_column[s_keyemul[i].mask];
                }
            acase TWIN:
                wheremouse = i;
                switch (i)
                {
                case   0: thequeue[queuepos++] = (shift() ? '!'  : '1');
                acase  1: thequeue[queuepos++] = (shift() ? QUOTES : '2');
                acase  2: thequeue[queuepos++] = (shift() ? '#'  : '3');
                acase  3: thequeue[queuepos++] = (shift() ? '$'  : '4');
                acase  4: thequeue[queuepos++] = (shift() ? '%'  : '5');
                acase  5: thequeue[queuepos++] = (shift() ? '&'  : '6');
                acase  6: thequeue[queuepos++] = (shift() ? QUOTE  : '7');
                acase  7: thequeue[queuepos++] = (shift() ? '('  : '8');
                acase  8: thequeue[queuepos++] = (shift() ? ')'  : '9');
                acase  9: thequeue[queuepos++] = (shift() ? ' '  : '0');
                acase 10: thequeue[queuepos++] = (shift() ? '*'  : ':');
                acase 11: thequeue[queuepos++] = (shift() ? '='  : '-');
                acase 13: thequeue[queuepos++] = ESC;
                acase 14: thequeue[queuepos++] = (ctrl()  ?   17 : 'Q');
                acase 15: thequeue[queuepos++] = (ctrl()  ?   23 : 'W');
                acase 16: thequeue[queuepos++] = (ctrl()  ?    5 : 'E');
                acase 17: thequeue[queuepos++] = (ctrl()  ?   18 : 'R');
                acase 18: thequeue[queuepos++] = (ctrl()  ?   20 : 'T');
                acase 19: thequeue[queuepos++] = (ctrl()  ?   25 : 'Y');
                acase 20: thequeue[queuepos++] = (ctrl()  ?   21 : 'U');
                acase 21: thequeue[queuepos++] = (ctrl()  ?    9 : 'I');
                acase 22: if (ctrl()) thequeue[queuepos++] = 15; else thequeue[queuepos++] = (shift() ? '_'  : 'O');
                acase 23: if (ctrl()) thequeue[queuepos++] = 16; else thequeue[queuepos++] = (shift() ? '@'  : 'P');
                acase 24: thequeue[queuepos++] = LF;
                acase 25: thequeue[queuepos++] = CR;
                acase 26: softctrl = TRUE;
                acase 27: thequeue[queuepos++] = (ctrl()  ?    1 : 'A');
                acase 28: thequeue[queuepos++] = (ctrl()  ?   19 : 'S');
                acase 29: thequeue[queuepos++] = (ctrl()  ?    4 : 'D');
                acase 30: thequeue[queuepos++] = (ctrl()  ?    6 : 'F');
                acase 31: thequeue[queuepos++] = (ctrl()  ? BEL  : 'G');
                acase 32: thequeue[queuepos++] = (ctrl()  ?    8 : 'H');
                acase 33: thequeue[queuepos++] = (ctrl()  ?   10 : 'J');
                acase 34: if (ctrl()) thequeue[queuepos++] = 11; else thequeue[queuepos++] = (shift() ? '['  : 'K');
                acase 35: if (ctrl()) thequeue[queuepos++] = 12; else thequeue[queuepos++] = (shift() ? '\\' : 'L');
                acase 36: thequeue[queuepos++] = (shift() ? '+'  : ';');
                acase 37: thequeue[queuepos++] = BS;
                acase 38: softrept = TRUE;
                acase 39: thequeue[queuepos++] = NUL;
                acase 40: softlshift = TRUE;
                acase 41: thequeue[queuepos++] = (ctrl()  ?   26 : 'Z');
                acase 42: thequeue[queuepos++] = (ctrl()  ?   24 : 'X');
                acase 43: thequeue[queuepos++] = (ctrl()  ?    3 : 'C');
                acase 44: thequeue[queuepos++] = (ctrl()  ?   22 : 'V');
                acase 45: thequeue[queuepos++] = (ctrl()  ?    2 : 'B');
                acase 46: if (ctrl()) thequeue[queuepos++] = 14; else thequeue[queuepos++] = (shift() ? '^'  : 'N');
                acase 47: if (ctrl()) thequeue[queuepos++] = 13; else thequeue[queuepos++] = (shift() ? ']'  : 'M');
                acase 48: thequeue[queuepos++] = (shift() ? '<'  : ',');
                acase 49: thequeue[queuepos++] = (shift() ? '>'  : '.');
                acase 50: thequeue[queuepos++] = (shift() ? '?'  : '/');
                acase 51: softrshift = TRUE;
                acase 52: thequeue[queuepos++] = ' ';
                acase 53: wheremouse = -1; // so it doesn't invert
                          if (!SubWindowPtr[SUBWINDOW_PALETTE])
                          {   edit_palette();
                          }
                acase 54: wheremouse = -1; // so it doesn't invert
                          project_reset(FALSE);
                }
            acase PIPBUG:
                wheremouse = i;
                switch (pipbug_vdu)
                {
                case VDU_RADIOBULLETIN:
                    switch (i)
                    {
                    // 1st row
                    case   0: thequeue[queuepos++] = ' ';
                    acase  1: thequeue[queuepos++] = '!';
                    acase  2: thequeue[queuepos++] = QUOTES;
                    acase  3: thequeue[queuepos++] = '#';
                    acase  4: thequeue[queuepos++] = '$';
                    acase  5: thequeue[queuepos++] = '%';
                    acase  6: thequeue[queuepos++] = '&';
                    acase  7: thequeue[queuepos++] = QUOTE;
                    acase  8: thequeue[queuepos++] = '(';
                    acase  9: thequeue[queuepos++] = ')';
                    acase 10: thequeue[queuepos++] = '*';
                    acase 11: thequeue[queuepos++] = '+';
                    acase 12: thequeue[queuepos++] = ',';
                    acase 13: thequeue[queuepos++] = '-';
                    acase 14: thequeue[queuepos++] = '.';
                    acase 15: thequeue[queuepos++] = '/';
                    acase 16: thequeue[queuepos++] = LF;
                    acase 17: thequeue[queuepos++] = 0x0B;
                    acase 18: thequeue[queuepos++] = CR;
                    acase 19: thequeue[queuepos++] = BEL;
                    // 2nd row
                    acase 20: thequeue[queuepos++] = '0';
                    acase 21: thequeue[queuepos++] = '1';
                    acase 22: thequeue[queuepos++] = '2';
                    acase 23: thequeue[queuepos++] = '3';
                    acase 24: thequeue[queuepos++] = '4';
                    acase 25: thequeue[queuepos++] = '5';
                    acase 26: thequeue[queuepos++] = '6';
                    acase 27: thequeue[queuepos++] = '7';
                    acase 28: thequeue[queuepos++] = '8';
                    acase 29: thequeue[queuepos++] = '9';
                    acase 30: thequeue[queuepos++] = ':';
                    acase 31: thequeue[queuepos++] = ';';
                    acase 32: thequeue[queuepos++] = '<';
                    acase 33: thequeue[queuepos++] = '=';
                    acase 34: thequeue[queuepos++] = '>';
                    acase 35: thequeue[queuepos++] = '?';
                    acase 36: thequeue[queuepos++] = 0x1A;
                    acase 37: thequeue[queuepos++] = 0x1B;
                    acase 38: thequeue[queuepos++] = 0x1D;
                    acase 39: thequeue[queuepos++] = 0x1F;
                    // 3rd row
                    acase 40: thequeue[queuepos++] = '@';
                    acase 41: thequeue[queuepos++] = 'A';
                    acase 42: thequeue[queuepos++] = 'B';
                    acase 43: thequeue[queuepos++] = 'C';
                    acase 44: thequeue[queuepos++] = 'D';
                    acase 45: thequeue[queuepos++] = 'E';
                    acase 46: thequeue[queuepos++] = 'F';
                    acase 47: thequeue[queuepos++] = 'G';
                    acase 48: thequeue[queuepos++] = 'H';
                    acase 49: thequeue[queuepos++] = 'I';
                    acase 50: thequeue[queuepos++] = 'J';
                    acase 51: thequeue[queuepos++] = 'K';
                    acase 52: thequeue[queuepos++] = 'L';
                    acase 53: thequeue[queuepos++] = 'M';
                    acase 54: thequeue[queuepos++] = 'N';
                    acase 55: thequeue[queuepos++] = 'O';
                    acase 56: thequeue[queuepos++] = 0x6A;
                    acase 57: thequeue[queuepos++] = 0x6B;
                    acase 58: thequeue[queuepos++] = 0x6D;
                    acase 59: thequeue[queuepos++] = 0x6F;
                    // 4th row
                    acase 60: thequeue[queuepos++] = 'P';
                    acase 61: thequeue[queuepos++] = 'Q';
                    acase 62: thequeue[queuepos++] = 'R';
                    acase 63: thequeue[queuepos++] = 'S';
                    acase 64: thequeue[queuepos++] = 'T';
                    acase 65: thequeue[queuepos++] = 'U';
                    acase 66: thequeue[queuepos++] = 'V';
                    acase 67: thequeue[queuepos++] = 'W';
                    acase 68: thequeue[queuepos++] = 'X';
                    acase 69: thequeue[queuepos++] = 'Y';
                    acase 70: thequeue[queuepos++] = 'Z';
                    acase 71: thequeue[queuepos++] = '[';
                    acase 72: thequeue[queuepos++] = '\\';
                    acase 73: thequeue[queuepos++] = ']';
                    acase 74: thequeue[queuepos++] = '^';
                    acase 75: thequeue[queuepos++] = '_';
                    acase 76: thequeue[queuepos++] = 0x7A;
                    acase 77: thequeue[queuepos++] = 0x7B;
                    acase 78: thequeue[queuepos++] = 0x7D;
                    acase 79: thequeue[queuepos++] = DEL;
                    }
                acase VDU_VT100:
                    switch (i)
                    {
                    // 1st row
                    case   0: thequeue[queuepos++] = VT; // up
                    acase  1: thequeue[queuepos++] = LF; // down
                    acase  2: thequeue[queuepos++] = BS; // left
                    acase  3: thequeue[queuepos++] = FF; // right
                    // 2nd row
                    acase  4: thequeue[queuepos++] = ESC;
                    acase  5: thequeue[queuepos++] = (shift() ? '!'  : '1');
                    acase  6: thequeue[queuepos++] = (shift() ? '@'  : '2');
                    acase  7: thequeue[queuepos++] = (shift() ? '#'  : '3'); // or pound sterling symbol
                    acase  8: thequeue[queuepos++] = (shift() ? '$'  : '4');
                    acase  9: thequeue[queuepos++] = (shift() ? '%'  : '5');
                    acase 10: thequeue[queuepos++] = (shift() ? '^'  : '6');
                    acase 11: thequeue[queuepos++] = (shift() ? '&'  : '7');
                    acase 12: thequeue[queuepos++] = (shift() ? '*'  : '8');
                    acase 13: thequeue[queuepos++] = (shift() ? '('  : '9');
                    acase 14: thequeue[queuepos++] = (shift() ? ')'  : '0');
                    acase 15: thequeue[queuepos++] = (shift() ? '_'  : '-');
                    acase 16: thequeue[queuepos++] = (shift() ? '+'  : '=');
                    acase 17: thequeue[queuepos++] = (shift() ? '`'  : '~');
                    acase 18: thequeue[queuepos++] = BS;
                    acase 19: thequeue[queuepos++] = NUL;
                    acase 20: thequeue[queuepos++] = '7';
                    acase 21: thequeue[queuepos++] = '8';
                    acase 22: thequeue[queuepos++] = '9';
                    acase 23: thequeue[queuepos++] = '-';
                    // 3rd row
                    acase 24: thequeue[queuepos++] = ESC;
                    acase 25: thequeue[queuepos++] = (ctrl()  ?   17 : 'Q');
                    acase 26: thequeue[queuepos++] = (ctrl()  ?   23 : 'W');
                    acase 27: thequeue[queuepos++] = (ctrl()  ?    5 : 'E');
                    acase 28: thequeue[queuepos++] = (ctrl()  ?   18 : 'R');
                    acase 29: thequeue[queuepos++] = (ctrl()  ?   20 : 'T');
                    acase 30: thequeue[queuepos++] = (ctrl()  ?   25 : 'Y');
                    acase 31: thequeue[queuepos++] = (ctrl()  ?   21 : 'U');
                    acase 32: thequeue[queuepos++] = (ctrl()  ?    9 : 'I');
                    acase 33: thequeue[queuepos++] = (ctrl()  ?   15 : 'O');
                    acase 34: thequeue[queuepos++] = (ctrl()  ?   16 : 'P');
                    acase 35: if (ctrl()) thequeue[queuepos++] = ESC;  else thequeue[queuepos++] = (shift() ? '{' : '[');
                    acase 36: if (ctrl()) thequeue[queuepos++] = 0x1D; else thequeue[queuepos++] = (shift() ? '}' : ']'); // GS
                    acase 37: thequeue[queuepos++] = CR;
                    acase 38: thequeue[queuepos++] = DEL;
                    acase 39: thequeue[queuepos++] = '4';
                    acase 40: thequeue[queuepos++] = '5';
                    acase 41: thequeue[queuepos++] = '6';
                    acase 42: thequeue[queuepos++] = ',';
                    // 4th row
                    acase 43: softctrl = TRUE;
                    acase 44: capslock = capslock ? FALSE : TRUE; update_capslock();
                    acase 45: thequeue[queuepos++] = (ctrl()  ?    1 : 'A');
                    acase 46: thequeue[queuepos++] = (ctrl()  ?   19 : 'S');
                    acase 47: thequeue[queuepos++] = (ctrl()  ?    4 : 'D');
                    acase 48: thequeue[queuepos++] = (ctrl()  ?    6 : 'F');
                    acase 49: thequeue[queuepos++] = (ctrl()  ? BEL  : 'G');
                    acase 50: thequeue[queuepos++] = (ctrl()  ?    8 : 'H');
                    acase 51: thequeue[queuepos++] = (ctrl()  ?   10 : 'J');
                    acase 52: thequeue[queuepos++] = (ctrl()  ?   11 : 'K');
                    acase 53: thequeue[queuepos++] = (ctrl()  ?   12 : 'L');
                    acase 54: thequeue[queuepos++] = (shift() ? ':'  : ';');
                    acase 55: thequeue[queuepos++] = (shift() ? QUOTES : QUOTE);
                    acase 56: thequeue[queuepos++] = (shift() ? '|'  : '\\');
                    acase 57: thequeue[queuepos++] = '4';
                    acase 58: thequeue[queuepos++] = '5';
                    acase 59: thequeue[queuepos++] = '6';
                    acase 60: thequeue[queuepos++] = CR;
                    // 5th row
                    //    61 is No Scroll
                    acase 62: softlshift = TRUE;
                    acase 63: thequeue[queuepos++] = (ctrl()  ?   26 : 'Z');
                    acase 64: thequeue[queuepos++] = (ctrl()  ?   24 : 'X');
                    acase 65: thequeue[queuepos++] = (ctrl()  ?    3 : 'C');
                    acase 66: thequeue[queuepos++] = (ctrl()  ?   22 : 'V');
                    acase 67: thequeue[queuepos++] = (ctrl()  ?    2 : 'B');
                    acase 68: thequeue[queuepos++] = (ctrl()  ?   14 : 'N');
                    acase 69: thequeue[queuepos++] = (ctrl()  ?   13 : 'M');
                    acase 70: thequeue[queuepos++] = (shift() ? '<'  : ',');
                    acase 71: thequeue[queuepos++] = (shift() ? '>'  : '.');
                    acase 72: thequeue[queuepos++] = (shift() ? '?'  : '/');
                    acase 73: softrshift = TRUE;
                    acase 74: thequeue[queuepos++] = LF;
                    acase 75: thequeue[queuepos++] = '0';
                    acase 76: thequeue[queuepos++] = '.';
                    // 6th row
                    acase 77: thequeue[queuepos++] = ' ';
                    // reset button
                    acase 78: wheremouse = -1; // so it doesn't invert
                              project_reset(FALSE);
                    }
                adefault:
                    switch (i)
                    {
                    // 1st row
                    case   0: thequeue[queuepos++] = (shift() ? '!'  : '1');
                    acase  1: thequeue[queuepos++] = (shift() ? QUOTES : '2');
                    acase  2: thequeue[queuepos++] = (shift() ? '#'  : '3');
                    acase  3: thequeue[queuepos++] = (shift() ? '$'  : '4');
                    acase  4: thequeue[queuepos++] = (shift() ? '%'  : '5');
                    acase  5: thequeue[queuepos++] = (shift() ? '&'  : '6');
                    acase  6: thequeue[queuepos++] = (shift() ? QUOTE  : '7');
                    acase  7: thequeue[queuepos++] = (shift() ? '('  : '8');
                    acase  8: thequeue[queuepos++] = (shift() ? ')'  : '9');
                    acase  9: thequeue[queuepos++] = (shift() ? ' '  : '0'); // or NUL?
                    acase 10: thequeue[queuepos++] = (shift() ? '*'  : ':');
                    acase 11: thequeue[queuepos++] = (shift() ? '='  : '-');
                    acase 12: thequeue[queuepos++] = (shift() ? '`'  : '@');
                    acase 13: thequeue[queuepos++] = 0x0C; // FF
                    acase 14: thequeue[queuepos++] = EM; // PgUp
                    // 2nd row
                    acase 15: thequeue[queuepos++] = ESC;
                    acase 16: thequeue[queuepos++] = (ctrl()  ?   17 : 'Q');
                    acase 17: thequeue[queuepos++] = (ctrl()  ?   23 : 'W');
                    acase 18: thequeue[queuepos++] = (ctrl()  ?    5 : 'E');
                    acase 19: thequeue[queuepos++] = (ctrl()  ?   18 : 'R');
                    acase 20: thequeue[queuepos++] = (ctrl()  ?   20 : 'T');
                    acase 21: thequeue[queuepos++] = (ctrl()  ?   25 : 'Y');
                    acase 22: thequeue[queuepos++] = (ctrl()  ?   21 : 'U');
                    acase 23: thequeue[queuepos++] = (ctrl()  ?    9 : 'I');
                    acase 24: thequeue[queuepos++] = (ctrl()  ?   15 : 'O');
                    acase 25: thequeue[queuepos++] = (ctrl()  ?   16 : 'P');
                    acase 26: if (ctrl()) thequeue[queuepos++] = ESC;  else thequeue[queuepos++] = (shift() ? '{' : '[');
                    acase 27: if (ctrl()) thequeue[queuepos++] = 0x1D; else thequeue[queuepos++] = (shift() ? '}' : ']'); // GS
                    acase 28: thequeue[queuepos++] = (pipbug_vdu == VDU_ELEKTERMINAL ? EOT : LF);
                    acase 29: thequeue[queuepos++] = CR;
                    acase 30: thequeue[queuepos++] = SUB;
                    // 3rd row
                    acase 31: softctrl = TRUE;
                    acase 32: thequeue[queuepos++] = (ctrl()  ?    1 : 'A');
                    acase 33: thequeue[queuepos++] = (ctrl()  ?   19 : 'S');
                    acase 34: thequeue[queuepos++] = (ctrl()  ?    4 : 'D');
                    acase 35: thequeue[queuepos++] = (ctrl()  ?    6 : 'F');
                    acase 36: thequeue[queuepos++] = (ctrl()  ? BEL  : 'G');
                    acase 37: thequeue[queuepos++] = (ctrl()  ?    8 : 'H');
                    acase 38: thequeue[queuepos++] = (ctrl()  ?   10 : 'J');
                    acase 39: thequeue[queuepos++] = (ctrl()  ?   11 : 'K');
                    acase 40: thequeue[queuepos++] = (ctrl()  ?   12 : 'L');
                    acase 41: thequeue[queuepos++] = (shift() ? '+'  : ';');
                    acase 42: thequeue[queuepos++] = 0x1C; // FS
                    acase 43: if (ctrl()) thequeue[queuepos++] = 0x1F; else thequeue[queuepos++] = BS; // US
                    acase 44: thequeue[queuepos++] = 0x09; // HT
                    acase 45: thequeue[queuepos++] = CAN;
                    // 4th row
                    acase 46: softlshift = TRUE;
                    acase 47: thequeue[queuepos++] = (ctrl()  ?   26 : 'Z');
                    acase 48: thequeue[queuepos++] = (ctrl()  ?   24 : 'X');
                    acase 49: thequeue[queuepos++] = (ctrl()  ?    3 : 'C');
                    acase 50: thequeue[queuepos++] = (ctrl()  ?   22 : 'V');
                    acase 51: thequeue[queuepos++] = (ctrl()  ?    2 : 'B');
                    acase 52: thequeue[queuepos++] = (ctrl()  ?   14 : 'N');
                    acase 53: thequeue[queuepos++] = (ctrl()  ?   13 : 'M');
                    acase 54: thequeue[queuepos++] = (shift() ? '<'  : ',');
                    acase 55: thequeue[queuepos++] = (shift() ? '>'  : '.');
                    acase 56: thequeue[queuepos++] = (shift() ? '?'  : '/');
                    acase 57: softrshift = TRUE;
                    acase 58: thequeue[queuepos++] = (pipbug_vdu == VDU_ELEKTERMINAL ? NAK : VT);
                    acase 59: thequeue[queuepos++] = NUL;
                    // 5th row
                    acase 60: thequeue[queuepos++] = ' ';
                    // reset button
                    acase 61: wheremouse = -1; // so it doesn't invert
                              project_reset(FALSE);
                }   }
            acase ZACCARIA:
                wheremouse = i;
                switch (i)
                {
                case   0: newkeys[ 0] |=  coinop_button1;
                acase  1: newkeys[ 1] |=  coinop_1p;
                acase  2: newkeys[ 2] |=  coinop_2p;
                acase  3: newkeys[ 3] |=  coinop_joy1left;
                acase  4: newkeys[ 4] |=  coinop_joy1right;
                acase  5:
                    if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
                    {   if (SubWindowPtr[SUBWINDOW_GAMEINFO])
                        {   close_subwindow(SUBWINDOW_GAMEINFO);
                        } else
                        {   help_gameinfo();
                    }   }
                    else
                    {   newkeys[ 5] |=  coinop_joy1up;
                    }
                acase  6: newkeys[ 6] |=  coinop_joy1down;
                acase  7: newkeys[ 7] |=  coinop_button2;
                acase  8: newkeys[ 8] |=  coinop_button3;
                acase  9: newkeys[ 9] |=  coinop_button4;
                acase 10:
                    if (SubWindowPtr[SUBWINDOW_GAMEINFO])
                    {   close_subwindow(SUBWINDOW_GAMEINFO);
                    } else
                    {   help_gameinfo();
                }   }
            acase MALZAK:
                wheremouse = i;
                switch (i)
                {
                case   0: newkeys[ 0] |=  coinop_button1;
                acase  1: newkeys[ 1] |=  coinop_1p;
                acase  2: newkeys[ 2] |=  coinop_2p;
                acase  3: newkeys[ 3] |=  coinop_joy1up;
                acase  4: newkeys[ 4] |= (coinop_joy1up & coinop_joy1right);
                acase  5: newkeys[ 5] |=  coinop_joy1right;
                acase  6: newkeys[ 6] |= (coinop_joy1right & coinop_joy1down);
                acase  7: newkeys[ 7] |=  coinop_joy1down;
                acase  8: newkeys[ 8] |= (coinop_joy1down & coinop_joy1left);
                acase  9: newkeys[ 9] |=  coinop_joy1left;
                acase 10: newkeys[10] |= (coinop_joy1left & coinop_joy1up);
                }
            acase PONG:
                switch (i)
                {
                case 4: // angles
                    if (angles == 2) angles = 0; else angles++;
                    pong_updatedips();
                acase 5: // speed
                    if (memmap == MEMMAP_8550)
                    {  if (pongspeed == 2) pongspeed = 0; else pongspeed++; // 0..2
                    } else
                    {  if (pongspeed == 1) pongspeed = 0; else pongspeed++; // 0..1
                    }
                    pong_updatedips();
                acase 6: // auto/manual serving
                    serving = (serving ? 0 : 1);
                    pong_updatedips();
                acase 7: // reset
                    project_reset(TRUE);
                acase 8: // lock horizontal axis
                    if (lockhoriz) lockhoriz = FALSE; else lockhoriz = TRUE;
                    pong_updatedips();
                acase 9: // sound
                    sound = sound ? FALSE : TRUE;
                    docommand(MENUITEM_SOUND);
                acase 10: // swap controllers
                    swapped = swapped ? FALSE : TRUE;
                    docommand(MENUITEM_SWAPPED);
                adefault:
                    wheremouse = i;
                    newkeys[i] |= 1;
                }
            acase BINBUG:
                switch (i)
                {
                case   0: enqueue(SCAN_C , 'C');
                acase  1: enqueue(SCAN_A8, '8');
                acase  2: enqueue(SCAN_A4, '4');
                acase  3: enqueue(SCAN_A0, '0');
                acase  4: enqueue(SCAN_D , 'D');
                acase  5: enqueue(SCAN_A9, '9');
                acase  6: enqueue(SCAN_A5, '5');
                acase  7: enqueue(SCAN_A1, '1');
                acase  8: enqueue(SCAN_E , 'E');
                acase  9: enqueue(SCAN_A , 'A');
                acase 10: enqueue(SCAN_A6, '6');
                acase 11: enqueue(SCAN_A2, '2');
                acase 12: enqueue(SCAN_F , 'F');
                acase 13: enqueue(SCAN_B , 'B');
                acase 14: enqueue(SCAN_A7, '7');
                acase 15: enqueue(SCAN_A3, '3');
                }
            //lint -fallthrough
            case SELBST:
            case MIKIT:
            case PHUNSY:
                wheremouse = i;
                newkeys[i] |= keys_column[selbst_keyemul[i].mask];
            acase TYPERIGHT:
                wheremouse = i;
                switch (i)
                {
                case  0: console_start = 2;
                acase 1: console_a     = 2;
                acase 2: console_b     = 2;
                acase 3: console_reset = 2;
                adefault:
                    wheremouse = i;
                    for (j = 0; j < SCANCODES; j++)
                    {   if (keyname[j].tr_index == i)
                        {   newkeys[keyname[j].tr_index] = TRUE;
                            if (capslock || shift())
                            {   enqueue((UWORD) j, (TEXT) keyname[j].tr_shifted);
                            } else
                            {   enqueue((UWORD) j, (TEXT) keyname[j].tr_unshifted);
                            }
                            break;
            }   }   }   }

            return; // for speed
}   }   }

EXPORT void controls_mouseup(void)
{   wheremouse = -1;
}

EXPORT void music_mousedown(int whichbutton, int mousex, int mousey)
{   FAST UBYTE t;
    FAST int   i;
#ifdef WIN32
    FAST POINT thepoint;
    FAST RECT  therect;
#endif

    if (recmode == RECMODE_PLAY)
    {   return;
    }

    // assert(SubWindowPtr[SUBWINDOW_MUSIC]);

#ifdef WIN32
    thepoint.x = mousex;
    thepoint.y = mousey;
    ClientToScreen(SubWindowPtr[SUBWINDOW_MUSIC], &thepoint);
    DISCARD GetWindowRect(GetDlgItem(SubWindowPtr[SUBWINDOW_MUSIC], IDC_MUSIC), &therect);
    mousex = thepoint.x - therect.left;
    mousey = thepoint.y - therect.top;
#endif
    mousex /= 2;
    mousey /= 2;

    for (i = 0; i < NOTES; i++)
    {   if
        (   (   mousex >= notes[i].first_leftx   -  6
             && mousex <= notes[i].first_rightx  -  6
             && mousey >= NOTES_FIRST_TOPY       - 11
             && mousey <= NOTES_FIRST_BOTTOMY    - 11
            )
         || (   notes[i].colour == EMUBRUSH_WHITE
             && mousex >= notes[i].second_leftx  -  6
             && mousex <= notes[i].second_rightx -  6
             && mousey >= NOTES_SECOND_TOPY      - 11
             && mousey <= NOTES_SECOND_BOTTOMY   - 11
        )   )
        {   t = (region == REGION_NTSC) ? (notes[i].ntscval) : (notes[i].palval);
            if (t == 0 || (machine == ARCADIA && t >= 0x80))
            {   return; // ghosted note
            }

            wheremusicmouse[whichbutton] = i;

            if (whichbutton == 0) // LMB
            {   if (machine == ARCADIA)
                {   memory[A_PITCH ] &= 0x80;
                    memory[A_PITCH ] |= t;
                    memory[A_VOLUME] |= 8; // turn tone on
                    if (!(memory[A_VOLUME] & 0x07)) // if guest volume is silent
                    {   memory[A_VOLUME] |= 7; // set guest volume to maximum
                }   }
                elif (machines[machine].pvis)
                {   memory[pvibase + PVI_PITCH] = t;
                    if (machine == INTERTON || memmap == MEMMAP_F)
                    {   memory[IE_NOISE] |= 4; // turn tone on
                        if (memmap == MEMMAP_F)
                        {   memory[0x1D0E] |= 0xF; // set guest tone volume to maximum
            }   }   }   }
            else // RMB
            {   if (machine == ARCADIA)
                {   memory[A_PITCH ] &= 0x80;
                    memory[A_PITCH ] |= t;
                    memory[A_VOLUME] |= 0x10; // turn noise on
                    if (!(memory[A_VOLUME] & 0x07)) // if guest volume is silent
                    {   memory[A_VOLUME] |= 7; // set guest volume to maximum
                }   }
                elif (machine == INTERTON || memmap == MEMMAP_F)
                {   memory[pvibase + PVI_PITCH] = t;
                    memory[IE_NOISE] |= 8; // turn noise on
            }   }

            playsound(FALSE);
            return;
    }   }

    wheremusicmouse[whichbutton] = -1;
}

EXPORT void music_mouseup(int whichbutton)
{   if (wheremusicmouse[whichbutton] != -2)
    {   if (whichbutton == 0) // LMB
        {   if (machine == ARCADIA)
            {   memory[A_VOLUME] &= 0xF7; // turn tone off
            } elif (machines[machine].pvis)
            {   memory[pvibase + PVI_PITCH] = 0; // set tone to rest
                if (machine == INTERTON || memmap == MEMMAP_F)
                {   memory[IE_NOISE] &= 0xFB; // turn tone off
                    if (memmap == MEMMAP_F)
                    {   memory[0x1D0E] &= 0xF0; // set guest tone volume to silent
        }   }   }   }
        else // RMB
        {   if (machine == ARCADIA)
            {   memory[A_VOLUME] &= 0xEF; // turn noise off
            } elif (machine == INTERTON || memmap == MEMMAP_F)
            {   memory[IE_NOISE] &= 0xF7; // turn noise off
        }   }

        playsound(FALSE);
        wheremusicmouse[whichbutton] = -2;
}   }

EXPORT void domouse(void)
{   FAST UBYTE t;

    if (wheremouse != -1)
    {   // assert(SubWindowPtr[SUBWINDOW_CONTROLS]);

        switch (machine)
        {
        case ARCADIA:
            t = memory[a_keyemul[wheremouse].address] | a_keyemul[wheremouse].mask;
            uviwrite(a_keyemul[wheremouse].address, t);
        acase INTERTON:
            t = memory[i_keyemul[wheremouse].address] | i_keyemul[wheremouse].mask;
            pviwrite((int) i_keyemul[wheremouse].address, t, FALSE);
        acase ELEKTOR:
            t = memory[e_keyemul[wheremouse].address] | e_keyemul[wheremouse].mask;
            pviwrite((int) e_keyemul[wheremouse].address, t, FALSE);
        acase INSTRUCTOR:
            keys_column[s_keyemul[wheremouse].address] |= s_keyemul[wheremouse].mask;
        acase ZACCARIA:
            switch (wheremouse)
            {
            case  0: coinop_button1   |= 1;
            acase 1: coinop_1p        |= 1;
            acase 2: coinop_2p        |= 1;
            acase 3: coinop_joy1left  |= 1;
            acase 4: coinop_joy1right |= 1;
            acase 5: coinop_joy1up    |= 1;
            acase 6: coinop_joy1down  |= 1;
            acase 7: coinop_button4   |= 1;
            acase 8: coinop_button2   |= 1;
            acase 9: coinop_button3   |= 1;
            }
        acase MALZAK:
            switch (wheremouse)
            {
            case   0: coinop_button1   |= 1;
            acase  1: coinop_1p        |= 1;
            acase  2: coinop_2p        |= 1;
            acase  3: coinop_joy1up    |= 1;
            acase  4: coinop_joy1up    |= 1;
                      coinop_joy1right |= 1;
            acase  5: coinop_joy1right |= 1;
            acase  6: coinop_joy1down  |= 1;
                      coinop_joy1right |= 1;
            acase  7: coinop_joy1down  |= 1;
            acase  8: coinop_joy1down  |= 1;
                      coinop_joy1left  |= 1;
            acase  9: coinop_joy1left  |= 1;
            acase 10: coinop_joy1left  |= 1;
                      coinop_joy1up    |= 1;
            }
        acase TYPERIGHT:
            switch (wheremouse)
            {
            case  0: console_start = 2;
            acase 1: console_a     = 2;
            acase 2: console_b     = 2;
            acase 3: console_reset = 2;
            }
        acase BINBUG:
        case SELBST:
        case MIKIT:
        case PHUNSY:
            keys_column[selbst_keyemul[wheremouse].address] |= selbst_keyemul[wheremouse].mask;
}   }   }

/* Connect/disconnect netplay interface. */
EXPORT void NetPlay(int Switch)
{   int i;

#ifdef AMIGA
    if (!SocketBase)
    {   return;
    }
#endif

    switch (Switch)
    {
    case NET_OFF:
        // Disconnect netplay
        NetClose();
    acase NET_SERVER:
        // If already connected, drop out
        if (connected != NET_OFF)
        {   break;
        }

#ifdef USEIPV6
        ActAsServer_IPv6(netport);
#else
        ActAsServer_IPv4(netport);
#endif
    acase NET_CLIENT:
        // If already connected, drop out
        if (connected != NET_OFF)
        {   break;
        }

        aborting = FALSE;
        sound_off(FALSE);
        hostnamewindow();
        sound_on(TRUE);
        if (!hostname[0] || aborting)
        {   aborting = FALSE;

            zprintf
            (   TEXTPEN_NET,
                LLL(
                    MSG_CANCELLED,
                    "Cancelled.\n\n"
            )   );
            return;
        }

        // convert to lower case
        for (i = 0; i < (int) strlen((const char*) hostname); i++)
        {   if (hostname[i] >= 'A' && hostname[i] <= 'Z')
            {   hostname[i] += 32;
        }   }

        zprintf
        (   TEXTPEN_NET,
            LLL(
                MSG_NET_CONNECTINGASCLIENT,
                "Connecting as client to server at %s on port %d...please wait...\n"
            ),
            hostname,
            (int) netport
        );
        lockgui();
#ifdef USEIPV6
        ConnectAsClient_IPv6(              hostname, netport);
#else
        ConnectAsClient_IPv4((const char*) hostname, netport);
#endif
        unlockgui();
    }

    settitle();
}

EXPORT void NetClose(void)
{
#ifdef AMIGA
    if (!SocketBase)
    {   return;
    }
#endif

    if (Socket >= 0)
    {   // assert(connected != NET_OFF);

        DISCARD CloseSocket((unsigned int) Socket);
        Socket = INVALID_SOCKET;
        connected = NET_OFF;
        zprintf
        (   TEXTPEN_NET,
            LLL(
                MSG_NET_DISCONNECTED,
                "Disconnected.\n"
        )   );
        updatemenus();
}   }

EXPORT FLAG NetSend(const char* Out, int N)
{   int I, J;

    if
    (   Socket < 0
#ifdef AMIGA
     || !SocketBase
#endif
    )
    {   return FALSE;
    }

    // Send data
    for (I = J = N; (J >= 0) && I;)
    {   J = (int) send((LONG) Socket, (APTR) Out, (LONG) I, 0); // OS4 needs the APTR cast!
        if (J > 0)
        {   Out += J;
            I   -= J;
    }   }
    if (I)
    {   return FALSE;
    }

    return TRUE;
}
EXPORT FLAG NetRecv(char* In, int N)
{   int I, J;

    if
    (   Socket < 0
#ifdef AMIGA
     || !SocketBase
#endif
    )
    {   return FALSE;
    }

    // Receive data
    for (I = J = N; (J >= 0) && I;)
    {   J = (int) recv((unsigned int) Socket, (UBYTE*) In, I, 0);
        if (J > 0)
        {   In += J;
            I  -= J;
    }   }
    if (I)
    {   return FALSE;
    }

    return TRUE;
}

EXPORT FLAG NetSendByte(UBYTE value)
{
#ifdef AMIGA
    if (!SocketBase)
    {   return FALSE;
    }
#endif

    /* Have to have a socket */
    if (Socket < 0)
    {   return FALSE;
    }

    /* Send data */
    if (send((unsigned int) Socket, &value, 1, 0) > 0)
    {   return TRUE;
    } // implied else
    return FALSE;
}

EXPORT UBYTE NetReceiveByte(void)
{   FAST UBYTE value;

#ifdef AMIGA
    if (!SocketBase)
    {   return 0;
    }
#endif

    /* Have to have a socket */
    if (Socket < 0)
    {   return 0;
    }

    /* Send data */
    if (recv((unsigned int) Socket, &value, 1, 0) > 0)
    {   return value;
    } // implied else
    return 0;

    /* When successful, returns 0..255.
       When failed, returns 0.
       Therefore, 0 can mean either success or failure :-( . */
}

EXPORT void playsound(FLAG force)
{   FAST          FLAG   firstchan;
    FAST          ULONG  temp1, temp2;
    FAST          UBYTE  MIDIBuffer[4],
                         midivolume,
                         oldsmusvolume,
                         SMUSBuffer[2];
    FAST          int    i,
                         length;
    FAST          float  hertz;
#ifdef DEBUGMIDI
    FAST          int    j;
#endif
    PERSIST const float  lb_div[13 + 1] =
{   0.0, // $0000
  478.0, // $0001
  451.0, // $0002
  426.0, // $0004
  402.0, // $0008
  379.0, // $0010
  358.0, // $0020
  338.0, // $0040
  319.0, // $0080
  301.0, // $0100
  284.0, // $0200
  268.0, // $0400
  253.0, // $0800
  239.0  // $1000
};
    /* guestvolume[]: indexed by guest channel.
                      contents are stored in guest format.
       guestpitch[]:  indexed by guest channel.
                      contents are stored in guest format.

       Although Arcadia/PIPBUG/CD2650 and Interton/Elektor volumes do not match,
       this is unimportant as playsound(TRUE) is called when changing
       machines. Thus, Arcadia/PIPBUG/CD2650 volumes are never compared against
       Interton/Elektor volumes. */

    if (!sound)
    {   return;
    }

    for (i = 0; i < GUESTCHANNELS; i++)
    {   guestpitch[i]  =
        guestvolume[i] = 0;
    }

    if (machines[machine].pvis >= 1)
    {   if ((whichgame == KABOOM1POS || whichgame == KABOOM2POS) && memory[pvibase + PVI_PITCH] == 0)
        {
#ifdef KABOOMCLICKS
            play_sample(SAMPLE_CLICK);
#else
            guestpitch[TONE_1STXVI] = 1;
#endif
        } else
        {   guestpitch[TONE_1STXVI] = (UWORD) memory[pvibase + PVI_PITCH];
        }
        if
        (   memmap == MEMMAP_F
         && (memory[PSG_MIXER1] & 0x40)
        )
        {   guestvolume[TONE_1STXVI] = memory[PSG_PORTA1] & 0xF;
        } elif (machine == INTERTON || memmap == MEMMAP_F)
        {   if     ((memory[IE_NOISE] & 0x04) == 0x00)
            {   guestvolume[TONE_1STXVI] =  0; // 0%
            } elif ((memory[IE_NOISE] & 0xC0) == 0xC0)
            {   guestvolume[TONE_1STXVI] =  4; // 25%
            } elif ((memory[IE_NOISE] & 0xC0) == 0x80)
            {   guestvolume[TONE_1STXVI] =  6; // 37.5% (not 50%! See Elektor mag, Jul 1982, p. 78.)
            } elif ((memory[IE_NOISE] & 0xC0) == 0x40)
            {   guestvolume[TONE_1STXVI] = 12; // 75%
            } else
            {   guestvolume[TONE_1STXVI] = 15; // 100%
        }   }
        else
        {   guestvolume[TONE_1STXVI] = 15; // 4/4
        }
        if (machines[machine].pvis >= 2)
        {   guestpitch[TONE_2NDXVI]      = memory[pvibase + 0x100 + PVI_PITCH];
            guestvolume[TONE_2NDXVI]     = 15;
            if (machines[machine].pvis >= 3)
            {   guestpitch[TONE_3RDXVI]  = memory[pvibase + 0x200 + PVI_PITCH];
                guestvolume[TONE_3RDXVI] = 15;
    }   }   }

    switch (machine)
    {
    case ARCADIA:
        if ((memory[A_VOLUME]) & 0x08)
        {   guestpitch[TONE_1STXVI]   = memory[A_PITCH]  & 0x7F;
            guestvolume[TONE_1STXVI]  = memory[A_VOLUME] & 0x07; // 0..7
        }
        if ((memory[A_VOLUME]) & 0x10)
        {   guestpitch[NOISE_1STXVI]  = memory[A_PITCH]  & 0x7F;
            guestvolume[NOISE_1STXVI] = memory[A_VOLUME] & 0x07; // 0..7
        }
    acase INTERTON:
    case ELEKTOR:
        if (memmap == MEMMAP_E)
        {   guestpitch[NOISE_1STXVI]  =
            guestvolume[NOISE_1STXVI] = 0;
            break;
        }

        if (bangercharging)
        {   if (!(memory[IE_NOISE] & 0x10))
            {   bangercharging = FALSE;
                temp1 = cycles_2650 - bangfrom;
             /* Typical values are:
                          66         for Kaboom
                       7,623.. 7,638 for Noise Explosion
                About 16,818..16,992 for Hunting
                About 35,499..35,520 for Grand Prix */
                if (temp1 >= 7500)
                {   play_sample(SAMPLE_EXPLOSION);
                } else
                {   sound_stop(GUESTCHANNELS + SAMPLE_EXPLOSION);
                    play_sample(SAMPLE_CLICK);
        }   }   }
        else
        {   if (memory[IE_NOISE] & 0x10)
            {   bangercharging = TRUE;
                bangfrom       = cycles_2650;
        }   }

        if ((memory[IE_NOISE] & 0x8) && memory[pvibase + PVI_PITCH])
        {   guestpitch[NOISE_1STXVI]      = (UWORD) memory[pvibase + PVI_PITCH];
            if     ((memory[IE_NOISE] & 0xC0) == 0xC0)
            {   guestvolume[NOISE_1STXVI] =  4; // 1/4
            } elif ((memory[IE_NOISE] & 0xC0) == 0x80)
            {   guestvolume[NOISE_1STXVI] =  8; // 2/4
            } elif ((memory[IE_NOISE] & 0xC0) == 0x40)
            {   guestvolume[NOISE_1STXVI] = 12; // 3/4
            } else
            {   guestvolume[NOISE_1STXVI] = 15; // 4/4
        }   }
        else
        {   guestpitch[NOISE_1STXVI]  =
            guestvolume[NOISE_1STXVI] = 0;
        }
    acase ZACCARIA:
        if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
        {   temp1 =  lb_snd & 0x00001FFF;
            guestpitch[TONE_1STLB1] =
            guestpitch[TONE_1STLB2] = 0;
            firstchan = TRUE;
            for (i = 1; i <= 13; i++)
            {   if (temp1 & (1 << (i - 1)))
                {   if (firstchan)
                    {   guestpitch[TONE_1STLB1] = i;
                        firstchan = FALSE;
                    } else
                    {   guestpitch[TONE_1STLB2] = i;
                        break;
            }   }   }

            temp2 = (lb_snd & 0x03FFE000) >> 12;
            guestpitch[TONE_2NDLB1] =
            guestpitch[TONE_2NDLB2] = 0;
            firstchan = TRUE;
            for (i = 1; i <= 13; i++)
            {   if (temp2 & (1 << (i - 1)))
                {   if (firstchan)
                    {   guestpitch[TONE_2NDLB1] = i;
                        firstchan = FALSE;
                    } else
                    {   guestpitch[TONE_2NDLB2] = i;
                        break;
            }   }   }

            guestvolume[TONE_1STLB1] =
            guestvolume[TONE_1STLB2] =
            guestvolume[TONE_2NDLB1] =
            guestvolume[TONE_2NDLB2] = 15;

#ifdef DEBUGSOUND
            view_tms();
#endif
        } elif (memmap == MEMMAP_GALAXIA)
        {   // squealer
            guestpitch[TONE_A1] = squeal;
            if (squeal)
            {   guestvolume[TONE_A1] =  6;
            } else
            {   guestvolume[TONE_A1] =  0;
            }

            // throbber
            if (ioport[PORTD].contents & 8)
            {
#ifdef WIN32
                guestpitch[ TONE_B1] = 0xFF; // arbitrary meaningless value
#endif
#ifdef AMIGA
                guestpitch[ TONE_B1] = throb;
#endif
                guestvolume[TONE_B1] =  4;
            } else
            {   guestpitch[ TONE_B1] =  0;
                guestvolume[TONE_B1] =  0;
        }   }
    acase PIPBUG:
        if (whichgame == MUSIC1POS || whichgame == MUSIC2POS)
        {   if (iar == 0x4A5)
            {   guestpitch[TONE_1STXVI] = r[0];
                if (pipbug_getpitch(guestpitch[TONE_1STXVI]) != 0.0)
                {   guestvolume[TONE_1STXVI] = 7;
                } else
                {   guestvolume[TONE_1STXVI] = 0;
            }   }
            else
            {   guestpitch[TONE_1STXVI]  = oldguestpitch[TONE_1STXVI];
                guestvolume[TONE_1STXVI] = oldguestvolume[TONE_1STXVI];
        }   }
        elif (whichgame == PIPBUG_MORSEPOS && iar == 0x49B)
        {   guestpitch[TONE_1STXVI]  = 0x55; // middle A (440 Hz)
            guestvolume[TONE_1STXVI] = 7;
        }
    acase INSTRUCTOR:
        if (whichgame == SI50_THEMEPOS)
        {   if (iar == 0x1F)
            {   guestpitch[TONE_1STXVI]  = r[3] + 1;
                guestvolume[TONE_1STXVI] = 7;
            } else
            {   guestpitch[TONE_1STXVI]  = oldguestpitch[TONE_1STXVI];
                guestvolume[TONE_1STXVI] = oldguestvolume[TONE_1STXVI];
        }   }
    acase CD2650:
    case SELBST:
        if (tone)
        {   guestpitch[TONE_1STXVI]  = 0x55; // middle A (440 Hz)
            guestvolume[TONE_1STXVI] = 7;
        } else
        {   guestpitch[TONE_1STXVI]  =
            guestvolume[TONE_1STXVI] = 0;
        }
    acase PHUNSY:
        if (whichgame == PIANOPOS)
        {   if (iar == 0x1816)
            {   guestpitch[TONE_1STXVI] = r[3] + 1;
                if (phunsy_getpianopitch(guestpitch[TONE_1STXVI]) != 0.0)
                {   guestvolume[TONE_1STXVI] = 7;
                } else
                {   guestvolume[TONE_1STXVI] = 0;
            }   }
            elif (iar == 0x1827)
            {   guestpitch[TONE_1STXVI]  =
                guestvolume[TONE_1STXVI] = 0;
            } // else don't change from previous value
        } elif (whichgame == PHUNSY_THEMEPOS)
        {   // assert(iar == 0x1827);
            guestpitch[TONE_1STXVI]  = r[3] + 1;
            guestvolume[TONE_1STXVI] = 7;
        } elif (whichgame == BELMACHPOS || whichgame == BELMACH0POS)
        {   if ((r[0] & 0x3E) == 0x3E) // rest
            {   guestpitch[TONE_1STXVI]  =
                guestvolume[TONE_1STXVI] = 0;
            } else
            {   guestpitch[TONE_1STXVI]  = r[0] & 0x3F;
                guestvolume[TONE_1STXVI] = 7;
        }   }
    acase PONG:
    case TYPERIGHT:
        if (ponghertz)
        {   guestpitch[TONE_1STXVI]  = ponghertz;
            guestvolume[TONE_1STXVI] = 7;
        }
    acase MIKIT:
        if (whichgame == MIKITMUSICPOS)
        {   if (iar >= 0x528 && iar <= 0x560)
            {   guestpitch[TONE_1STXVI]  = r[3];
                guestvolume[TONE_1STXVI] = 7;
            } else
            {   guestpitch[TONE_1STXVI]  = oldguestpitch[TONE_1STXVI];
                guestvolume[TONE_1STXVI] = oldguestvolume[TONE_1STXVI];
    }   }   }

    if (belling[0]) // machine bell
    {   guestpitch[TONE_2NDXVI] = 1;
        guestvolume[TONE_2NDXVI] = 7; // full
    }
    if (belling[1]) // printer bell
    {   guestpitch[TONE_3RDXVI] = 1;
        guestvolume[TONE_3RDXVI] = 7; // full
    }

    if (memmap == MEMMAP_F)
    {   // 1st PSG, output channel A
        if (!(memory[PSG_AMPLITUDEA1] & 0x10))
        {   guestvolume[TONE_A1]  =
            guestvolume[NOISE_A1] = memory[PSG_AMPLITUDEA1] & 0x0F;
        } else
        {   guestvolume[TONE_A1]  =
            guestvolume[NOISE_A1] = 15;
        }
        if (!(memory[PSG_MIXER1] &    1)) // 1st tone A enable
        {   guestpitch[TONE_A1] = ((memory[PSG_PITCHA1_H] & 0x0F) * 256) + memory[PSG_PITCHA1_L];
            if (guestpitch[TONE_A1] == 0)
            {   guestpitch[TONE_A1] = 1;
        }   }
        else
        {   guestpitch[TONE_A1] = 0;
        }
        if (!(memory[PSG_MIXER1] &    8)) // 1st noise A enable
        {   guestpitch[NOISE_A1] = memory[PSG_PITCHD1] & 0x1F;
        } else
        {   guestpitch[NOISE_A1] = 0;
        }

        // 1st PSG, output channel B
        if (!(memory[PSG_AMPLITUDEB1] & 0x10))
        {   guestvolume[TONE_B1] =
            guestvolume[NOISE_B1] = memory[PSG_AMPLITUDEB1] & 0x0F;
        } else
        {   guestvolume[TONE_B1] =
            guestvolume[NOISE_B1] = 15;
        }
        if (!(memory[PSG_MIXER1] &    2)) // 1st tone B enable
        {   guestpitch[TONE_B1] = ((memory[PSG_PITCHB1_H] & 0x0F) * 256) + memory[PSG_PITCHB1_L];
        } else
        {   guestpitch[TONE_B1] = 0;
        }
        if (!(memory[PSG_MIXER1] & 0x10)) // 1st noise B enable
        {   guestpitch[NOISE_B1] = memory[PSG_PITCHD1] & 0x1F;
        } else
        {   guestpitch[NOISE_B1] = 0;
        }

        // 1st PSG, output channel C
        if (!(memory[PSG_AMPLITUDEC1] & 0x10))
        {   guestvolume[TONE_C1]  =
            guestvolume[NOISE_C1] = memory[PSG_AMPLITUDEC1] & 0x0F;
        } else
        {   guestvolume[TONE_C1]  =
            guestvolume[NOISE_C1] = 15;
        }
        if (!(memory[PSG_MIXER1] &    4)) // 1st tone C enable
        {   guestpitch[TONE_C1] = ((memory[PSG_PITCHC1_H] & 0x0F) * 256) + memory[PSG_PITCHC1_L];
        } else
        {   guestpitch[TONE_C1] = 0;
        }
        if (!(memory[PSG_MIXER1] & 0x20)) // 1st noise C enable
        {   guestpitch[NOISE_C1] = memory[PSG_PITCHD1] & 0x1F;
        } else
        {   guestpitch[NOISE_C1] = 0;
        }

        // 2nd PSG, output channel A
        if (!(memory[PSG_AMPLITUDEA2] & 0x10))
        {   guestvolume[TONE_A2]  =
            guestvolume[NOISE_A2] = memory[PSG_AMPLITUDEA2] & 0x0F;
        } else
        {   guestvolume[TONE_A2]  =
            guestvolume[NOISE_A2] = 15;
        }
        if (!(memory[PSG_MIXER2] &    1)) // 2nd tone A enable
        {   guestpitch[TONE_A2] = ((memory[PSG_PITCHA2_H] & 0x0F) * 256) + memory[PSG_PITCHA2_L];
        } else
        {   guestpitch[TONE_A2] = 0;
        }
        if (!(memory[PSG_MIXER2] &    8)) // 2nd noise A enable
        {   guestpitch[NOISE_A2] = memory[PSG_PITCHD2] & 0x1F;
        } else
        {   guestpitch[NOISE_A2] = 0;
        }

        // 2nd PSG, output channel B
        if (!(memory[PSG_AMPLITUDEB2] & 0x10))
        {   guestvolume[TONE_B2]  =
            guestvolume[NOISE_B2] = memory[PSG_AMPLITUDEB2] & 0x0F;
        } else
        {   guestvolume[TONE_B2]  =
            guestvolume[NOISE_B2] = 15;
        }
        if (!(memory[PSG_MIXER2] &    2)) // 2nd tone B enable
        {   guestpitch[TONE_B2] = ((memory[PSG_PITCHB2_H] & 0x0F) * 256) + memory[PSG_PITCHB2_L];
        } else
        {   guestpitch[TONE_B2] = 0;
        }
        if (!(memory[PSG_MIXER2] & 0x10)) // 2nd noise B enable
        {   guestpitch[NOISE_B2] = memory[PSG_PITCHD2] & 0x1F;
        } else
        {   guestpitch[NOISE_B2] = 0;
        }

        // 2nd PSG, output channel C
        if (!(memory[PSG_AMPLITUDEC2] & 0x10))
        {   guestvolume[TONE_C2]  =
            guestvolume[NOISE_C2] = memory[PSG_AMPLITUDEC2] & 0x0F;
        } else
        {   guestvolume[TONE_C2]  =
            guestvolume[NOISE_C2] = 15;
        }
        if (!(memory[PSG_MIXER2] &    4)) // 2nd tone C enable
        {   guestpitch[TONE_C2] = ((memory[PSG_PITCHC2_H] & 0x0F) * 256) + memory[PSG_PITCHC2_L];
        } else
        {   guestpitch[TONE_C2] = 0;
        }
        if (!(memory[PSG_MIXER2] & 0x20)) // 2nd noise C enable
        {   guestpitch[NOISE_C2] = memory[PSG_PITCHD2] & 0x1F;
        } else
        {   guestpitch[NOISE_C2] = 0;
    }   }

    for (i = FIRSTGUESTTONE; i <= LASTGUESTTONE; i++)
    {   if (guestpitch[i] && guestvolume[i])
        {   if
            (   force
             || !guestplaying[i]
             || guestpitch[i]  != oldguestpitch[i]
             || guestvolume[i] != oldguestvolume[i]
            )
            {   switch (i)
                {
                case TONE_1STXVI: // 0
                    switch (machine)
                    {
                    case PIPBUG:
                    case CD2650:
                    case SELBST:
                        hertz = pipbug_getpitch(guestpitch[i]);
                    acase INSTRUCTOR:
                        if (whichgame == SI50_THEMEPOS)
                        {   hertz = (float) ((dividend * 1.5) / guestpitch[i]);
                        }
                    acase PHUNSY:
                        if (whichgame == PIANOPOS)
                        {   hertz = phunsy_getpianopitch(guestpitch[i]);
                        } elif (whichgame == PHUNSY_THEMEPOS)
                        {   hertz = (float) (dividend / guestpitch[i]);
                        } else
                        {   // assert(whichgame == BELMACHPOS || whichgame == BELMACH0POS);
                            hertz = phunsy_getbelmachpitch(guestpitch[i]);
                        }
                    acase MIKIT:
                        if (retune)
                        {   hertz = mikit_retune((UBYTE) guestpitch[i]);
                        } else
                        {   hertz = (float) guestpitch[i];
                        }
                    acase PONG:
                    case TYPERIGHT:
                        hertz = (float) ponghertz;
                    adefault: // ARCADIA, INTERTON, ELEKTOR, coin-ops
#ifdef SHOWTONES
                        if (region == REGION_NTSC)
                        {   zprintf(TEXTPEN_VERBOSE, "Playing note %s.\n", idealfreq_ntsc[guestpitch[i]].name);
                        } else
                        {   zprintf(TEXTPEN_VERBOSE, "Playing note %s.\n", idealfreq_pal[guestpitch[i]].name);
                        }
#endif
                        if (retune)
                        {   hertz = (region == REGION_NTSC) ? (idealfreq_ntsc[guestpitch[i]].hertz) : (idealfreq_pal[guestpitch[i]].hertz);
                        } else
                        {   hertz = (float) (dividend / (guestpitch[i] + 1)); // desired frequency of the waveform
                    }   }
                acase TONE_2NDXVI: // 1
                case  TONE_3RDXVI: // 2
                    if (machines[machine].pvis)
                    {   if (retune)
                        {   hertz = (region == REGION_NTSC) ? (idealfreq_ntsc[guestpitch[i]].hertz) : (idealfreq_pal[guestpitch[i]].hertz);
                        } else
                        {   hertz = (float) (dividend / (guestpitch[i] + 1)); // desired frequency of the waveform
                    }   }
                    else // bells
                    {   if (i == TONE_2NDXVI) // machine bell
                        {   hertz = 800.0;
                        } else
                        {   // assert(i == TONE_3RDXVI); // printer bell
                            hertz = 2000.0; // guess
                    }   }
                acase 3: // TONE_1STLB1 and TONE_A1
                    if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
                    {   hertz = (float) ( 62500.0 / lb_div[guestpitch[i]]);
                    } elif (memmap == MEMMAP_F)
                    {   hertz = (float) (110837.0 / guestpitch[i]); // desired frequency of the waveform
                    } elif (memmap == MEMMAP_GALAXIA) // squealer
                    {   hertz = (float) (    16.0 * guestpitch[i]); // 16..4080 Hz
                    }
                acase 4: // TONE_1STLB2 and TONE_B1
                    if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
                    {   hertz = (float) ( 62500.0 / lb_div[guestpitch[i]]);
                    } elif (memmap == MEMMAP_F)
                    {   hertz = (float) (110837.0 / guestpitch[i]); // desired frequency of the waveform
                    } elif (memmap == MEMMAP_GALAXIA) // throbber
                    {
#ifdef WIN32
                        hertz = (float) 220.0; // arbitrary meaningless value
#endif
#ifdef AMIGA
                        if (throb <= 7) // 1.. 7 -> 220..244
                        {   hertz = 216.0 + (throb * 4.0);
                        } else      // 8..12 -> 240..224
                        {   hertz = 272.0 - (throb * 4.0);
                        }
#endif
                    }
                acase 5: // TONE_2NDLB1 and TONE_C1
                case  6: // TONE_2NDLB2 and TONE_A2
                    if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
                    {   hertz = (float) (125000.0 / lb_div[guestpitch[i]]);
                    } elif (memmap == MEMMAP_F)
                    {   hertz = (float) (110837.0 / guestpitch[i]); // desired frequency of the waveform
                    }
                acase TONE_B2: // 7
                case  TONE_C2: // 8
                    hertz = (float) (110837.0 / guestpitch[i]); // desired frequency of the waveform
                }

#ifdef DEBUGSOUND
                zprintf(TEXTPEN_VERBOSE, "Channel %d is playing tone at pitch %f, vol %d on cycle %u.\n", i, hertz, guestvolume[i], cycles_2650);
#endif
                chan_status[i] = CHAN_PLAY;
                chan_hertz[i]  = hertz;
                chan_volume[i] = guestvolume[i];
        }   }
        else
        {   if (chan_status[i] == CHAN_PLAY)
            {   chan_status[i] = CHAN_PLAYTHENSTOP;
                // don't touch chan_hertz[i] nor chan_volume[i]
            } else
            {   chan_status[i] = CHAN_STOP;
    }   }   }

    for (i = FIRSTGUESTNOISE; i <= LASTGUESTNOISE; i++)
    {   if (guestpitch[i] && guestvolume[i])
        {   if
            (   force
             || !guestplaying[i]
             || guestvolume[i] != oldguestvolume[i]
             || guestpitch[i] != oldguestpitch[i]
            )
            {   if (i == NOISE_1STXVI)
                {   hertz = (float) (dividend / (guestpitch[i] + 1)); // desired frequency of the waveform
                } else
                {   hertz = (float) (110837.0 / guestpitch[i]);
                }

#ifdef DEBUGSOUND
                zprintf(TEXTPEN_VERBOSE, "Channel %d is playing noise at pitch %f, vol %d on cycle %u.\n", i, hertz, guestvolume[i], cycles_2650);
#endif
                chan_status[i] = CHAN_PLAY;
                chan_hertz[i]  = hertz;
                chan_volume[i] = guestvolume[i];
        }   }
        else
        {   if (chan_status[i] == CHAN_PLAY)
            {   chan_status[i] = CHAN_PLAYTHENSTOP;
                // don't touch chan_hertz[i] nor chan_volume[i]
            } else
            {   chan_status[i] = CHAN_STOP;
    }   }   }

    if (MIDIHandle)
    {   for (i = 0; i < GUESTCHANNELS; i++)
        {   if
            (   memmapinfo[memmap].channelused[i]
             && (   oldguestpitch[i]  != guestpitch[i]
                 || oldguestvolume[i] != guestvolume[i]
            )   )
            {   if (guestpitch[i])
                {   length = variablelength(frames - miditime[i], MIDIBuffer);
                    DISCARD fwrite(MIDIBuffer, length, 1, MIDITrackHandle[i]);
                    mtrk[i] += length;
#ifdef DEBUGMIDI
                    zprintf(TEXTPEN_VERBOSE, "Frame %d, channel #%2d, delay:", frames, i);
                    for (j = 0; j < 4; j++)
                    {   if (j < length)
                        {   zprintf(TEXTPEN_VERBOSE, " $%02X", MIDIBuffer[j]);
                        } else
                        {   zprintf(TEXTPEN_VERBOSE, "    ");
                    }   }
#endif

                    if (oldguestpitch[i] && midiplaying[i]) // we were already playing a note, so we need to insert a rest (and a zero delay) first
                    {   MIDIBuffer[0] = 0x80 + i; // note off
                        if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
                        {   if (i == TONE_1STLB1 || i == TONE_1STLB2)
                            {   MIDIBuffer[1] = lb_note[oldguestpitch[i]].lowmidi;
                            } else
                            {   // assert(i == TONE_2NDLB1 || i == TONE_2NDLB2);
                                MIDIBuffer[1] = lb_note[oldguestpitch[i]].highmidi;
                        }   }
                        else
                        {   MIDIBuffer[1] = (region == REGION_NTSC) ? (idealfreq_ntsc[oldguestpitch[i]].midi) : (idealfreq_pal[oldguestpitch[i]].midi);
                        }
                        MIDIBuffer[2] = 0x40; // volume
                        MIDIBuffer[3] = 0x00; // delay
                        DISCARD fwrite(MIDIBuffer, 4, 1, MIDITrackHandle[i]);
                        mtrk[i] += 4;
#ifdef DEBUGMIDI
                        zprintf
                        (   TEXTPEN_VERBOSE,
                            ", rest: $8%X $%02X $40\n" \
                            "Frame %d, channel #%2d, delay: $00            ",
                            i,
                            (region == REGION_NTSC) ? (idealfreq_ntsc[oldguestpitch[i]].midi) : (idealfreq_pal[oldguestpitch[i]].midi),
                            frames,
                            i
                        );
#endif
                    }

                    if (machines[machine].pvis)
                    {   midivolume = volume_4to16[guestvolume[i]].up8 - 128; // volume is 0..127
                    } else
                    {   switch (machine)
                        {
                        case ARCADIA:
                        case PIPBUG:
                        case INSTRUCTOR:
                        case CD2650:
                        case PHUNSY:
                        case SELBST:
                        case MIKIT:
                        case PONG:
                        case TYPERIGHT:
                            midivolume = volume_3to16[guestvolume[i]].up8 - 128; // volume is 0..127
                        adefault: // eg. BINBUG, TWIN
                            midivolume = 0;
                    }   }
                    MIDIBuffer[0] = 0x90 + i; // note on
                    if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
                    {   if (i == TONE_1STLB1 || i == TONE_1STLB2)
                        {   MIDIBuffer[1] = lb_note[guestpitch[i]].lowmidi;
                        } else
                        {   // assert(i == TONE_2NDLB1 || i == TONE_2NDLB2);
                            MIDIBuffer[1] = lb_note[guestpitch[i]].highmidi;
                    }   }
                    else
                    {   MIDIBuffer[1] = (region == REGION_NTSC) ? (idealfreq_ntsc[guestpitch[i]].midi) : (idealfreq_pal[guestpitch[i]].midi);
                    }
                    MIDIBuffer[2] = midivolume;
                    DISCARD fwrite(MIDIBuffer, 3, 1, MIDITrackHandle[i]);
                    mtrk[i] += 3;
#ifdef DEBUGMIDI
                    zprintf
                    (   TEXTPEN_VERBOSE,
                        ", note: $9%X $%02X $%02X\n",
                        i,
                        (region == REGION_NTSC) ? (idealfreq_ntsc[guestpitch[i]].midi) : (idealfreq_pal[guestpitch[i]].midi),
                        midivolume
                    );
#endif

                    miditime[i] = frames;
                    midiplaying[i] = TRUE;
                } else // a rest
                {   if (midiplaying[i])
                    {   length = variablelength(frames - miditime[i], MIDIBuffer);
                        DISCARD fwrite(MIDIBuffer, length, 1, MIDITrackHandle[i]);
                        mtrk[i] += length;
#ifdef DEBUGMIDI
                        zprintf(TEXTPEN_VERBOSE, "Frame %d, channel #%2d, delay:", frames, i);
                        for (j = 0; j < 4; j++)
                        {   if (j < length)
                            {   zprintf(TEXTPEN_VERBOSE, " $%02X", MIDIBuffer[j]);
                            } else
                            {   zprintf(TEXTPEN_VERBOSE, "    ");
                        }   }
#endif
                        MIDIBuffer[0] = 0x80 + i; // note off
                        if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
                        {   if (i == TONE_1STLB1 || i == TONE_1STLB2)
                            {   MIDIBuffer[1] = lb_note[oldguestpitch[i]].lowmidi;
                            } else
                            {   // assert(i == TONE_2NDLB1 || i == TONE_2NDLB2);
                                MIDIBuffer[1] = lb_note[oldguestpitch[i]].highmidi;
                        }   }
                        else
                        {   MIDIBuffer[1] = (region == REGION_NTSC) ? (idealfreq_ntsc[oldguestpitch[i]].midi) : (idealfreq_pal[oldguestpitch[i]].midi);
                        }
                        MIDIBuffer[2] = 0x40;
                        DISCARD fwrite(MIDIBuffer, 3, 1, MIDITrackHandle[i]);
                        mtrk[i] += 3;
#ifdef DEBUGMIDI
                        zprintf
                        (   TEXTPEN_VERBOSE,
                            ", rest: $8%X $%02X $40\n",
                            i,
                            (region == REGION_NTSC) ? (idealfreq_ntsc[oldguestpitch[i]].midi) : (idealfreq_pal[oldguestpitch[i]].midi)
                        );
#endif

                        miditime[i] = frames;
                        midiplaying[i] = FALSE;
    }   }   }   }   }

    if (SMUSHandle)
    {   for (i = 0; i < GUESTCHANNELS; i++)
        {   if
            (   memmapinfo[memmap].channelused[i]
             && (   smusplaying[i]    == 0
                 || oldguestpitch[i]  != guestpitch[i]
                 || oldguestvolume[i] != guestvolume[i]
            )   )
            {   if (smusplaying[i] != 0)
                {   // write duration of old note/rest
                    smusduration = (frames - smustime[i]) * 4;
                    SMUSBuffer[0] = ticks2smus();
                    if (smusduration && smusnote[i] != 0x80)
                    {   SMUSBuffer[0] |= 0x40; // tied note
                    }
                    DISCARD fwrite(SMUSBuffer, 1, 1, SMUSTrackHandle[i]);
                    strk[i]++;
#ifdef DEBUGSMUS
                    zprintf(TEXTPEN_VERBOSE, "Frame %d, channel #%2d: duration       $%02X\n", frames, i, SMUSBuffer[0]);
#endif
                    while (smusduration)
                    {   SMUSBuffer[0] = smusnote[i];
                        SMUSBuffer[1] = ticks2smus();
                        if (smusduration && smusnote[i] != 0x80)
                        {   SMUSBuffer[1] |= 0x40; // tied note
                        }
                        DISCARD fwrite(SMUSBuffer, 2, 1, SMUSTrackHandle[i]);
                        strk[i] += 2;
#ifdef DEBUGSMUS
                        zprintf(TEXTPEN_VERBOSE, "Frame %d, channel #%2d: note pitch $%02X\n", frames, i, SMUSBuffer[0]);
                        zprintf(TEXTPEN_VERBOSE, "Frame %d, channel #%2d: duration       $%02X\n", frames, i, SMUSBuffer[1]);
#endif
                }   }

                if (guestpitch[i] && guestvolume[i])
                {   oldsmusvolume = smusvolume[i];
                    if (machines[machine].pvis)
                    {   smusvolume[i] = volume_4to16[guestvolume[i]].up8 - 128; // volume is 0..127
                    } else
                    {   switch (machine)
                        {
                        case ARCADIA:
                        case INSTRUCTOR:
                        case PIPBUG:
                        case CD2650:
                        case PHUNSY:
                        case SELBST:
                        case MIKIT:
                        case PONG:
                        case TYPERIGHT:
                            smusvolume[i] = volume_3to16[guestvolume[i]].up8 - 128; // volume is 0..127
                        adefault: // eg. BINBUG, TWIN
                            smusvolume[i] = 0;
                    }   }
                    if (smusvolume[i] != oldsmusvolume)
                    {   SMUSBuffer[0] = 0x84; // volume change event
                        SMUSBuffer[1] = smusvolume[i];
#ifdef DEBUGSMUS
                        zprintf(TEXTPEN_VERBOSE, "Frame %d, channel #%2d: volume     $%02X\n", frames, i, SMUSBuffer[1]);
#endif
                        DISCARD fwrite(SMUSBuffer, 2, 1, SMUSTrackHandle[i]);
                        strk[i] += 2;
                    }
                    // write pitch of new note
                    if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
                    {   if (i == TONE_1STLB1 || i == TONE_1STLB2)
                        {   SMUSBuffer[0] = lb_note[guestpitch[i]].lowmidi;
                        } else
                        {   // assert(i == TONE_2NDLB1 || i == TONE_2NDLB2);
                            SMUSBuffer[0] = lb_note[guestpitch[i]].highmidi;
                    }   }
                    else
                    {   SMUSBuffer[0] = (region == REGION_NTSC) ? (idealfreq_ntsc[guestpitch[i]].midi) : (idealfreq_pal[guestpitch[i]].midi);
                    }
#ifdef DEBUGSMUS
                    zprintf(TEXTPEN_VERBOSE, "Frame %d, channel #%2d: note pitch $%02X\n", frames, i, SMUSBuffer[0]);
#endif
                    smusplaying[i] = 2;
                } else
                {   // write "pitch" of new rest
                    SMUSBuffer[0] = 0x80;
#ifdef DEBUGSMUS
                    zprintf(TEXTPEN_VERBOSE, "Frame %d, channel #%2d: rest       $80\n", frames, i);
#endif
                    if (smusplaying[i] == 0)
                    {   smusplaying[i] = 1;
                }   }
                DISCARD fwrite(SMUSBuffer, 1, 1, SMUSTrackHandle[i]);
                strk[i]++;

                smusnote[i] = SMUSBuffer[0];
                smustime[i] = frames;
    }   }   }

    for (i = 0; i < GUESTCHANNELS; i++)
    {   oldguestvolume[i] = guestvolume[i];
        oldguestpitch[i]  = guestpitch[i];
}   }

EXPORT UBYTE get_guest_key(void)
{   int   i;
    FLAG  papertapeenabled;
    UBYTE inchar;

    /* assert
    (   machine == PIPBUG
     || machine == BINBUG
     || machine == TWIN
     || machine == CD2650
     || machine == PHUNSY
     || machine == SELBST
    ); */

    if (machine == CD2650 && queuekeystrokes && cycles_2650 < cd2650_downtill)
    {   return NC;
    } 

    switch (machine)
    {
    case PIPBUG:
    case BINBUG:
        papertapeenabled = ((ioport[PORTC].contents & 0x80) && paperreaderenabled) ? TRUE : FALSE;
    acase TWIN:
        papertapeenabled = ((ioport[ 0xE9].contents & 0x01) && paperreaderenabled) ? TRUE : FALSE;
    adefault: // eg. CD2650, PHUNSY, SELBST
        papertapeenabled = FALSE;
    }

    if (recmode == RECMODE_PLAY)
    {   inchar = (TEXT) IOBuffer[offset++];
    } elif (papertapeenabled && papertapemode[0] == TAPEMODE_PLAY)
    {   papertapebyte[0] = inchar = PapertapeBuffer[0][papertapewhere[0]];
        papertapewhere[0]++;
        if (papertapewhere[0] == papertapelength[0])
        {   papertape_stop(0); // this calls update_papertape()
        } else
        {   update_papertape(0, FALSE);
        }
        if (machine == TWIN)
        {   ioport[0xE9].contents &= 0xFE;
    }   }
    elif (queuekeystrokes)
    {   if (queuepos)
        {   inchar = (UBYTE) thequeue[0];
            // if ((whichgame != CHESS3POS1 && whichgame != CHESS3POS2) || iar != 0x27BD)
            {   queuepos--;
                if (queuepos)
                {   for (i = 0; i < queuepos; i++)
                    {   thequeue[i] = thequeue[i + 1];
        }   }   }   }
        else
        {   inchar = NC;
    }   }
    else
    {   inchar = guestkey;
    }

    if
    (   (    machine == TWIN
         || (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
    )   )
    {   if (inchar == BS)
        {   inchar = DEL;
        }
        if
        (   inchar == DEL
         && erasedel
         && (   (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS          )
             || (machine == TWIN   && memory[0x3FFE] == '1' && memory[0x3FFF] == '6') // UDOS 1.6
             || (machine == TWIN   && memory[0x3FFE] == '2' && memory[0x3FFF] == '0') // SDOS 2.0
             || (machine == TWIN   && memory[0x3FFE] == '4' && memory[0x3FFF] == '0') // SDOS 4.0
        )   )
        {   thecout(DEL);
            ignore_cout = TRUE;
    }   }

    if ((whichgame == PIPBUG_MASTERMIND1POS || whichgame == PIPBUG_MASTERMIND2POS) && inchar == CR)
    {   inchar = 0x0C; // Form Feed
    } elif (machine == TWIN && inchar == ESC)
    {   inchar = NC;
        twin_escaped = TRUE;
    }
        
    if (inchar != NC && (whichgame == MINIDISASM1POS || whichgame == MINIDISASM2POS))
    {   // assert(machine == PIPBUG);
        thecout(inchar);
    }

    if ((trace || logbios) && (machine != CD2650 || inchar != NC)) // CD2650 gets trapped in endless loop without even changing IAR
    {   zprintf
        (   TEXTPEN_TRACE,
            LLL(
                MSG_ENGINE_CHIN,
                "Returning from %s with input of %s.\n\n"
            ),
            (machine == CD2650) ? "KBIN" : "CHIN", // CD2650 note: they may not actually be using KBIN.
            asciiname_long[inchar]
        );
    }
    if (recmode == RECMODE_RECORD)
    {   OutputBuffer[0] = 1;
        OutputBuffer[1] = inchar;
        DISCARD fwrite(OutputBuffer, 2, 1, MacroHandle);
    }

    return inchar;
}

EXPORT void updatehiscores(int resolved)
{   int bestscore  = 0,
        i, j,
        multiplier = 0,  // initialized to avoid spurious SAS/C compiler warnings
        newscore;

    // assert(resolved != -1);

    if (machine == PONG)
    {   switch (pong_variant)
        {
        case  VARIANT_8550_PRACTICE:   bestscore = score[1];
        acase VARIANT_8550_RIFLE1:     bestscore = score[0];
        acase VARIANT_8550_RIFLE2:     bestscore = score[0];
        acase VARIANT_8600_PRACTICE:   bestscore = score[1];
        acase VARIANT_8600_BBPRACTICE: bestscore = score[0];
        acase VARIANT_8600_TARGET1:    bestscore = score[1];
    }   }
    else
    {   for (i = 0; i < 4; i++)
        {   if (hiscore[resolved].hipos[i] == 0)
            {   break;
            } // implied else

            // assert(whichgame != -1);
            if (resolved == known[BASEBALLPOS].hiscore)
            {   // assert(machine == ARCADIA);

                if
                (   (memory[hiscore[resolved].hipos[i] + 1] & 0x3F) >= 0x10 // '0'
                 && (memory[hiscore[resolved].hipos[i] + 1] & 0x3F) <= 0x19 // '9'
                )
                {   newscore = (((memory[hiscore[resolved].hipos[i]    ] & 0x3F) - 0x10) * 10)
                             +  ((memory[hiscore[resolved].hipos[i] + 1] & 0x3F) - 0x10);
                } else
                {   newscore =   (memory[hiscore[resolved].hipos[i]    ] & 0x3F) - 0x10;
            }   }
            else
            {   newscore = 0;

                switch (hiscore[resolved].length)
                {
                case  1: multiplier =      1;
                acase 2: multiplier =     10;
                acase 3: multiplier =    100;
                acase 4: multiplier =   1000;
                acase 5: multiplier =  10000;
                acase 6: multiplier = 100000;
                }

                switch (machine)
                {
                case ARCADIA:
                    for (j = 0; j < hiscore[resolved].length; j++)
                    {   if
                        (   (memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)] & 0x3F) >= 0x10 // '0'
                         && (memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)] & 0x3F) <= 0x19 // '9'
                        )
                        {   newscore += ((memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)] & 0x3F) - 0x10) * multiplier;
                        }
                        multiplier /= 10;
                    }
                acase MALZAK:
                    for (j = 0; j < hiscore[resolved].length; j++)
                    {   if
                        (   (memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)]) >= 0x30 // '0'
                         && (memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)]) <= 0x39 // '9'
                        )
                        {   newscore += ((memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)]) - 0x30) * multiplier;
                        }
                        multiplier /= 10;
                    }
                acase ZACCARIA:
                    if (memmap == MEMMAP_GALAXIA)
                    {   for (j = 0; j < hiscore[resolved].length; j++)
                        {   if
                            (   (memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)] & 0x7F) >= 0x60 // '0'
                             && (memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)] & 0x7F) <= 0x69 // '9'
                            )
                            {   newscore += ((memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)] & 0x7F) - 0x60) * multiplier;
                            }
                            multiplier /= 10;
                    }   }
                    else
                    {   // assert(memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN);
                        for (j = 0; j < hiscore[resolved].length; j++)
                        {   if
                            (   (memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)]) >= 0xB0 // '0'
                             && (memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)]) <= 0xB9 // '9'
                            )
                            {   newscore += ((memory[hiscore[resolved].hipos[i] + (j * hiscore[resolved].nextdigit)]) - 0xB0) * multiplier;
                            }
                            multiplier /= 10;
            }   }   }   }

            if (newscore > bestscore)
            {   bestscore = newscore;
    }   }   }

    if (bestscore > (int) hiscore[resolved].score)
    {   hiscore[resolved].score = (ULONG) bestscore;
        datestamp(resolved);
        strcpy(hiscore[resolved].username, username);
}   }

EXPORT void hiscore_register(int slot, int score)
{   if
    (   speedup >= SPEED_4QUARTERS
     && recmode != RECMODE_PLAY
     && score > (int) hiscore[slot].score
    )
    {   hiscore[slot].score = (ULONG) score;
        datestamp(slot);
}   }

EXPORT void loadhiscores(void)
{   TRANSIENT ULONG thesize;
    TRANSIENT FILE* TheLocalHandle /* = NULL */ ; // LocalHandle is a variable of winbase.h
    TRANSIENT int   i;
    PERSIST   TEXT  tempstring[80 + 1];

    thesize = getsize(FN_HISCORES);
    if (thesize == (ULONG) HISCORESLENGTH)
    {   if ((TheLocalHandle = fopen(FN_HISCORES, "rb")))
        {   if (fread(HiScoresBuffer, (size_t) thesize, 1, TheLocalHandle) == 1)
            {   if   (HiScoresBuffer[0] == 15) // 15 means V35.5+
                {   for (i = 0; i <= 85; i++)
                    {   readhiscore(i, i);
                }   }
                elif (HiScoresBuffer[0] == 14) // 14 means V35.22-35.5 beta
                {   for (i = 0; i <= 1; i++)
                    {   readhiscore(i, i);
                    }
                    for (i = 2; i <= 84; i++)
                    {   readhiscore(i, i + 1);
                }   }
                else
                {   sprintf((char*) tempstring, LLL(MSG_UNSUPPORTEDVER2, "Unsupported %s version (obsolete/future version)!"), "HGH");
                    say((STRPTR) tempstring);
            }   }

            DISCARD fclose(TheLocalHandle);
            // TheLocalHandle = NULL;
}   }   }

MODULE void readhiscore(int src, int dest)
{   int i;

    hiscore[dest].score          = (ULONG)
                                 ( (   65536 * HiScoresBuffer[1 + (src * HSENTRYSIZE) +  0])
                                 + (     256 * HiScoresBuffer[1 + (src * HSENTRYSIZE) +  1])
                                 +             HiScoresBuffer[1 + (src * HSENTRYSIZE) +  2]);
    for (i = 0; i < 15; i++)
    {   hiscore[dest].thedatetime[i] = (TEXT)  HiScoresBuffer[1 + (src * HSENTRYSIZE) +  3 + i] ; // 3..17
    }
    hiscore[dest].played         = (ULONG)
                                 ( (   65536 * HiScoresBuffer[1 + (src * HSENTRYSIZE) + 18])
                                 + (     256 * HiScoresBuffer[1 + (src * HSENTRYSIZE) + 19])
                                 +             HiScoresBuffer[1 + (src * HSENTRYSIZE) + 20]);
    hiscore[dest].playedmicro    = (ULONG)
                                 ( (   65536 * HiScoresBuffer[1 + (src * HSENTRYSIZE) + 21])
                                 + (     256 * HiScoresBuffer[1 + (src * HSENTRYSIZE) + 22])
                                 +             HiScoresBuffer[1 + (src * HSENTRYSIZE) + 23]);

    for (i = 0; i < MAX_USERNAMELEN; i++)
    {   hiscore[dest].username[i]    = (TEXT)  HiScoresBuffer[1 + (src * HSENTRYSIZE) + 24 + i] ; // 24..53
    }
    hiscore[dest].username[MAX_USERNAMELEN] = EOS;
}

EXPORT void savehiscores(void)
{   FILE* TheLocalHandle /* = NULL */ ; // LocalHandle is a variable of winbase.h
    int   i, j;

    if ((TheLocalHandle = fopen(FN_HISCORES, "wb")))
    {   HiScoresBuffer[0] = 15; // 15 means V35.5+
        for (i = 0; i < HISCORES; i++)
        {   HiScoresBuffer[     0 +     (i * HSENTRYSIZE) + 1] = (UBYTE)  (hiscore[i].score / 65536);
            HiScoresBuffer[     1 +     (i * HSENTRYSIZE) + 1] = (UBYTE) ((hiscore[i].score % 65536) / 256);
            HiScoresBuffer[     2 +     (i * HSENTRYSIZE) + 1] = (UBYTE)  (hiscore[i].score %   256);
            for (j = 0; j < 15; j++)
            {   HiScoresBuffer[ 3 + j + (i * HSENTRYSIZE) + 1] = (UBYTE)   hiscore[i].thedatetime[j]; // 3..17
            }
            HiScoresBuffer[    18 +     (i * HSENTRYSIZE) + 1] = (UBYTE)  (hiscore[i].played / 65536);
            HiScoresBuffer[    19 +     (i * HSENTRYSIZE) + 1] = (UBYTE) ((hiscore[i].played % 65536) / 256);
            HiScoresBuffer[    20 +     (i * HSENTRYSIZE) + 1] = (UBYTE)  (hiscore[i].played %   256);
            HiScoresBuffer[    21 +     (i * HSENTRYSIZE) + 1] = (UBYTE)  (hiscore[i].playedmicro / 65536);
            HiScoresBuffer[    22 +     (i * HSENTRYSIZE) + 1] = (UBYTE) ((hiscore[i].playedmicro % 65536) / 256);
            HiScoresBuffer[    23 +     (i * HSENTRYSIZE) + 1] = (UBYTE)  (hiscore[i].playedmicro %   256);
            for (j = 0; j < MAX_USERNAMELEN; j++)
            {   HiScoresBuffer[24 +     (i * HSENTRYSIZE) + 1 + j] = (UBYTE) hiscore[i].username[j];
        }   }

        DISCARD fwrite(HiScoresBuffer, (size_t) HISCORESLENGTH, 1, TheLocalHandle);
        DISCARD fclose(TheLocalHandle);
        // TheLocalHandle = NULL;
}   }

EXPORT void engine_quit(void)
{   // engine_save() requires the window to be open (on Amiga at least)

    if
    (   !upgrade
     && autosave
     && !crippled
    )
    {   changefilepart((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game, "AUTOSAVE.COS");
        engine_save(KIND_COS, FALSE);
    }
    if (!upgrade)
    {   malzak_savenvram();
    }

    logfile_close();
}

EXPORT void emulate(void)
{
#ifdef BENCHMARK_CPU
    slice_2650 += (int) machines[machine].cpf;
    cpu_2650_tapable();
    frames++;
#else
    switch (machine)
    {
    case  ARCADIA:                 uvi();
    acase INTERTON: case ELEKTOR:  newpvi();
    acase MALZAK:   case ZACCARIA: oldpvi();
    acase PIPBUG:                  pipbug_emulate();
    acase BINBUG:                  binbug_emulate();
    acase INSTRUCTOR:              si50_emulate();
    acase TWIN:                    twin_emulate();
    acase CD2650:                  cd2650_emulate();
    acase PHUNSY:                  phunsy_emulate();
    acase PONG:                    pong_simulate();
    acase SELBST:                  selbst_emulate();
    acase MIKIT:                   mikit_emulate();
    acase TYPERIGHT:               tr_simulate();
    }
#endif
}

EXPORT UBYTE readport(int port)
{   FLAG  handled;
    UBYTE t = 0; // initialized to avoid spurious SAS/C optimizer warnings

    if (emuid)
    {   handled = TRUE;
        switch (port)
        {
        case  111: // $6F
            t = 0x55;
        acase 112: // $70
#ifdef WIN32
            t = 1;
#endif
#ifdef AMIGA
    #ifdef __amigaos4__
            t = 3; // OS4.x
    #else
        #ifdef __morphos__
            t = 4; // MOS
        #else
            t = 2; // OS3.x
        #endif
    #endif
#endif
        acase 113: // $71
            t = MAJORVERSION;
        acase 114: // $72
            t = MINORVERSION;
        acase 115: // $73
            t = (UBYTE) ((cycles_2650 / 3) % 256);
        adefault:
            handled = FALSE;
        }
        if (handled)
        {   logport(port, t, FALSE);
            return t;
    }   }

    switch (machine)
    {
    case  PIPBUG:     return   pipbug_readport(port);
    acase BINBUG:     return   binbug_readport(port);
    acase INSTRUCTOR: return     si50_readport(port);
    acase TWIN:       return     twin_readport(port);
    acase CD2650:     return   cd2650_readport(port);
    acase ZACCARIA:   return zaccaria_readport(port);
    acase MALZAK:     return   malzak_readport(port);
    acase PHUNSY:     return   phunsy_readport(port);
    acase SELBST:     return   selbst_readport(port);
    acase MIKIT:      return    mikit_readport(port);
    adefault: // eg. ARCADIA, INTERTON, ELEKTOR
        logport(port, 0, FALSE);
        return 0; // arbitrary
}   }

EXPORT void writeport(int port, UBYTE data)
{   switch (machine)
    {
    case  PIPBUG:       pipbug_writeport(port, data);
    acase BINBUG:       binbug_writeport(port, data);
    acase INSTRUCTOR:     si50_writeport(port, data);
    acase TWIN:           twin_writeport(port, data);
    acase CD2650:       cd2650_writeport(port, data);
    acase MALZAK:       malzak_writeport(port, data);
    acase ZACCARIA:   zaccaria_writeport(port, data);
    acase PHUNSY:       phunsy_writeport(port, data);
    acase SELBST:       selbst_writeport(port, data);
    acase MIKIT:         mikit_writeport(port, data);
    adefault: // eg. ARCADIA, INTERTON, ELEKTOR
        logport(port, data, TRUE);
}   }

MODULE void aserialize_str(STRPTR label, STRPTR contents)
{   int i,
        length;

    length = strlen(label);

    if (serializemode == SERIALIZE_READ)
    {   if
        (   !strnicmp((char*) &IOBuffer[offset], label, length)
         && IOBuffer[offset + length] == '='
        )
        {   offset += length + 1;
            i = 0;
            while (IOBuffer[offset] != CR && IOBuffer[offset] != LF)
            {   contents[i++] = IOBuffer[offset++];
            }
            contents[i] = EOS;
            while (IOBuffer[offset] == CR || IOBuffer[offset] == LF)
            {   offset++;
    }   }   }
    else
    {   // assert(serializemode == SERIALIZE_WRITE);
        for (i = 0; i < length; i++)
        {   IOBuffer[offset++] = label[i];
        }
        IOBuffer[offset++] = '=';
        length = strlen(contents);
        for (i = 0; i < length; i++)
        {   IOBuffer[offset++] = contents[i];
        }
#ifdef WIN32
        IOBuffer[offset++] = CR;
#endif
        IOBuffer[offset++] = LF;
}   }

MODULE void aserialize_colours(STRPTR label, UBYTE* red, UBYTE* green, UBYTE* blue)
{   TEXT formatstring[80 + 1];
    int  length,
         tempred,
         tempgreen,
         tempblue;

    length = strlen(label);
    strcpy((char*) formatstring, label);
    strcat((char*) formatstring, "=$%02X%02X%02X");
    if (serializemode == SERIALIZE_READ)
    {   sscanf((char*) &IOBuffer[offset], (const char*) formatstring, &tempred, &tempgreen, &tempblue);
        *red   = (UBYTE) tempred;
        *green = (UBYTE) tempgreen;
        *blue  = (UBYTE) tempblue;
        offset += length + 1;
        while (IOBuffer[offset] != CR && IOBuffer[offset] != LF)
        {   offset++;
        }
        while (IOBuffer[offset] == CR || IOBuffer[offset] == LF)
        {   offset++;
    }   }
    else
    {   // assert(serializemode == SERIALIZE_WRITE);
        offset += sprintf((char*) &IOBuffer[offset], (const char*) formatstring, *red, *green, *blue);
#ifdef WIN32
        IOBuffer[offset++] = CR;
#endif
        IOBuffer[offset++] = LF;
}   }

MODULE void aserialize_int(STRPTR label, int* value, int lowest, int highest)
{   TEXT formatstring[80 + 1];
    int  temp; // must be signed!

    strcpy((char*) formatstring, label);
    strcat((char*) formatstring, "=%d");
    if (serializemode == SERIALIZE_READ)
    {   sscanf((char*) &IOBuffer[offset], (const char*) formatstring, &temp);
        if (temp >= lowest && temp <= highest)
        {   *value = temp;
        } else
        {   zprintf(TEXTPEN_ERROR, "%s: integer value of %d is out of range! (Range is %d..%d.)\n", label, temp, lowest, highest);
        }
        while (IOBuffer[offset] != CR && IOBuffer[offset] != LF)
        {   offset++;
        }
        while (IOBuffer[offset] == CR || IOBuffer[offset] == LF)
        {   offset++;
    }   }
    else
    {   // assert(serializemode == SERIALIZE_WRITE);
        offset += sprintf((char*) &IOBuffer[offset], (const char*) formatstring, *value);
#ifdef WIN32
        IOBuffer[offset++] = CR;
#endif
        IOBuffer[offset++] = LF;
}   }

MODULE void aserialize_uword(STRPTR label, UWORD* value, int lowest, int highest)
{   TEXT formatstring[80 + 1];
    int  temp; // must be signed!

    strcpy((char*) formatstring, label);
    strcat((char*) formatstring, "=%d");
    if (serializemode == SERIALIZE_READ)
    {   sscanf((char*) &IOBuffer[offset], (const char*) formatstring, &temp);
        if (temp >= lowest && temp <= highest)
        {   *value = (UWORD) temp;
        } else
        {   zprintf(TEXTPEN_ERROR, "%s: unsigned word value of %d is out of range! (Range is %d..%d.)\n", label, temp, lowest, highest);
        }
        while (IOBuffer[offset] != CR && IOBuffer[offset] != LF)
        {   offset++;
        }
        while (IOBuffer[offset] == CR || IOBuffer[offset] == LF)
        {   offset++;
    }   }
    else
    {   // assert(serializemode == SERIALIZE_WRITE);
        temp = (int) *value;
        offset += sprintf((char*) &IOBuffer[offset], (const char*) formatstring, temp);
#ifdef WIN32
        IOBuffer[offset++] = CR;
#endif
        IOBuffer[offset++] = LF;
}   }

MODULE void aserialize_ubyte(STRPTR label, UBYTE* value)
{   TEXT formatstring[80 + 1];
    int  temp; // must be signed!

    strcpy((char*) formatstring, label);
    strcat((char*) formatstring, "=%d");
    if (serializemode == SERIALIZE_READ)
    {   sscanf((char*) &IOBuffer[offset], (const char*) formatstring, &temp);
        if (temp >= 0 && temp <= 255)
        {   *value = (UBYTE) temp;
        } else
        {   zprintf(TEXTPEN_ERROR, "%s: unsigned byte value of %d is out of range!\n", label, temp);
        }
        while (IOBuffer[offset] != CR && IOBuffer[offset] != LF)
        {   offset++;
        }
        while (IOBuffer[offset] == CR || IOBuffer[offset] == LF)
        {   offset++;
    }   }
    else
    {   // assert(serializemode == SERIALIZE_WRITE);
        temp = (int) *value;
        offset += sprintf((char*) &IOBuffer[offset], (const char*) formatstring, temp);
#ifdef WIN32
        IOBuffer[offset++] = CR;
#endif
        IOBuffer[offset++] = LF;
}   }

MODULE void aserialize_bool(STRPTR label, int* value)
{   int i,
        length;

    length = strlen(label);

    if (serializemode == SERIALIZE_READ)
    {   if
        (   !strnicmp((char*) &IOBuffer[offset], label, length)
         && IOBuffer[offset + length] == '='
        )
        {   offset += length + 1;
            if     (!strnicmp((char*) &IOBuffer[offset], "true" , 4))
            {   *value = TRUE;
                offset += 4;
            } elif (!strnicmp((char*) &IOBuffer[offset], "false", 5))
            {   *value = FALSE;
                offset += 5;
            } else
            {   zprintf(TEXTPEN_ERROR, "%s: invalid boolean!\n", label);
            }
            while (IOBuffer[offset] != CR && IOBuffer[offset] != LF)
            {   offset++;
            }
            while (IOBuffer[offset] == CR || IOBuffer[offset] == LF)
            {   offset++;
    }   }   }
    else
    {   // assert(serializemode == SERIALIZE_WRITE);
        for (i = 0; i < length; i++)
        {   IOBuffer[offset++] = label[i];
        }
        IOBuffer[offset++] = '=';
        if (*value)
        {   strcpy((char*) &IOBuffer[offset], "true" );
            offset += 4;
        } else
        {   strcpy((char*) &IOBuffer[offset], "false");
            offset += 5;
        }
#ifdef WIN32
        IOBuffer[offset++] = CR;
#endif
        IOBuffer[offset++] = LF;
}   }

EXPORT void loadconfig(void)
{   ULONG thesize;
    FILE* TheLocalHandle /* = NULL */ ; // LocalHandle is a variable of winbase.h

    thesize = getsize((STRPTR) fn_config);
    if (thesize)
    {
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Attempting to read config file \"%s\" of size %d...\n", fn_config, thesize);
#endif
        alloc_iobuffer(thesize);
        if ((TheLocalHandle = fopen((const char*) fn_config, "rb")))
        {   if (fread(IOBuffer, thesize, 1, TheLocalHandle) == 1)
            {   serializemode = SERIALIZE_READ;
                if (IOBuffer[0] == 0xC6 && IOBuffer[1] == 0x02)
                {   ; // say("CFG files are unsupported (obsolete)!");
                } else
                {   serializeconfig();
                }
#ifdef AMIGA
                amiga_loadedconfig();
#endif
                loadedconfig = TRUE;
                // zprintf(TEXTPEN_VERBOSE, "Read config!\n");
        }   }
        DISCARD fclose(TheLocalHandle);
        // TheLocalHandle = NULL;
        free_iobuffer();
    }
#ifdef VERBOSE
    else
    {   zprintf(TEXTPEN_VERBOSE, "Config file \"%s\" doesn't exist or is zero-sized.\n", fn_config);
    }
#endif
}

EXPORT void saveconfig(void)
{   FILE* TheLocalHandle /* = NULL */ ;

#ifdef AMIGA
    configmachine = CONFIGMACHINE_AMIGA;
#endif
#ifdef WIN32
    configmachine = CONFIGMACHINE_IBM;
#endif
    serializemode = SERIALIZE_WRITE;
    alloc_iobuffer(16 * KILOBYTE);
    serializeconfig();
    if ((TheLocalHandle = fopen((const char*) fn_config, "wb")))
    {   DISCARD fwrite(IOBuffer, offset, 1, TheLocalHandle);
        DISCARD fclose(TheLocalHandle);
        // TheLocalHandle = NULL;
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Wrote config file \"%s\".\n", fn_config);
#endif
    }

    free_iobuffer();
}

MODULE void serializeconfig(void)
{   TEXT  tempstring[80 + 1];
    UBYTE red,
          green,
          blue;
    int   configversion = NEWER,
          i, j,
          temp;

    offset = 0;

    if (serializemode == SERIALIZE_WRITE)
    {   strcpy((char*) tempstring, NEWCONFIGVERSION);
    }
    aserialize_str( "version"             , (STRPTR) tempstring);
    if (serializemode == SERIALIZE_READ)
    {   if     (!strcmp((char*) tempstring, NEWCONFIGVERSION))
        {   configversion = NEWER;
        } elif (!strcmp((char*) tempstring, OLDCONFIGVERSION))
        {   configversion = OLDER;
        } elif (!strcmp((char*) tempstring, VERYOLDCONFIGVERSION))
        {   configversion = OLDEST;
        } else
        {   zprintf(TEXTPEN_ERROR, "Wrong INI file version (obsolete/future)!\n");
            return;
    }   }
    aserialize_int( "host"                , &configmachine, 0, 1); // amiga, ibm
    if (serializemode == SERIALIZE_READ &&
#ifdef WIN32
    configmachine != CONFIGMACHINE_IBM
#endif
#ifdef AMIGA
    configmachine != CONFIGMACHINE_AMIGA
#endif
    )
    {   zprintf(TEXTPEN_ERROR, "Foreign host INI file format!\n");
        return;
    }

    aserialize_bool("2650b"               , &supercpu          );
    aserialize_bool("analog"              , (int*) &analog     );
#ifdef WIN32
    for (i = 0; i <= 23; i++) // 0..23
    {   sprintf(tempstring, "associate_%d", i);
        aserialize_bool(tempstring        , &assoc[i].flag     );
    }
#endif
    aserialize_bool("artefacts"           , &undither          );
    aserialize_bool("autocoin"            , &autocoin          );
    aserialize_bool("autofire_left"       , (int*) &autofire[0]);
    aserialize_bool("autofire_right"      , (int*) &autofire[1]);
    aserialize_bool("autoframeskip"       , (int*) &autoframeskip);
    aserialize_bool("autopause"           , &autopause         );
    aserialize_bool("autosave"            , &autosave          );
#ifdef WIN32
    aserialize_bool("bezel"               , (int*) &bezel      );
#endif
    aserialize_bool("bigcontrols_arcadia" , (int*) &arcadia_bigctrls);
    aserialize_bool("bigcontrols_interton", (int*) &interton_bigctrls);
    aserialize_bool("bigcontrols_elektor" , (int*) &elektor_bigctrls);
    aserialize_bool("bigcontrols_mikit"   , (int*) &mikit_bigctrls);
    aserialize_bool("bigcontrols_si50"    , (int*) &si50_bigctrls);
    if (configversion == OLDEST)
    {   aserialize_bool("bigmusic"        , (int*) &temp       );
    }
    aserialize_bool("blankingareas"       , &usemargins        );
    aserialize_bool("blink"               , &blink             );
#ifdef WIN32
    aserialize_bool("candy_tapedeck_phunsy",&candy[1 - 1]      );
    aserialize_bool("candy_hostkybd"      , &candy[2 - 1]      );
    aserialize_bool("candy_printer"       , &candy[3 - 1]      );
    aserialize_bool("candy_controls"      , &candy[4 - 1]      );
#endif
    aserialize_bool("collisions"          , (int*) &collisions );
#ifdef WIN32
    aserialize_bool("colourlog"           , &colourlog         );
#endif
    aserialize_bool("confinepointer"      , &confine           );
    aserialize_bool("confirmquit"         , (int*) &confirmable);
#ifdef AMIGA
    aserialize_bool("createicons"         , &icons             );
#endif
    aserialize_bool("darkenbg"            , &darkenbg          );
    aserialize_bool("declinate"           , (int*) &declinate  );
    aserialize_bool("dejitter"            , &dejitter          );
    aserialize_bool("demultiplex"         , (int*) &demultiplex);
    aserialize_bool("drawcorners"         , &drawcorners       );
    aserialize_bool("drawunlit"           , &drawunlit         );
    aserialize_bool("eachpage"            , (int*) &eachpage   );
    aserialize_bool("echoconsole"         , &echoconsole       );
#ifdef WIN32
    aserialize_bool("enhanceskies"        , &enhancestars      );
#endif
    aserialize_bool("erasedeleted"        , &erasedel          );
    aserialize_bool("exactspeed"          , &exactspeed        );
    aserialize_bool("framebased"          , &framebased        );
    aserialize_bool("identifyemulator"    , &emuid             );
    aserialize_bool("fastbinbug"          , &fastbinbug        );
    aserialize_bool("fastcd2650"          , &fastcd2650        );
    aserialize_bool("fastpipbug"          , &fastpipbug        );
    aserialize_bool("fastselbstbaucomputer",&fastselbst        );
    aserialize_bool("flagline"            , (int*) &flagline   );
    aserialize_bool("invertflashed"       , &coomer            );
    aserialize_bool("forcefeedback_left"  , &useff[0]          );
    aserialize_bool("forcefeedback_right" , &useff[1]          );
    aserialize_bool("fullscreen"          , &fullscreen       );
#ifdef AMIGA
    if (configversion != NEWER)
#endif
    {   aserialize_bool("fullscreen_debugger" , &showdebugger[  1]);
        aserialize_bool("fullscreen_menubar"  , &showmenubars[  1]);
        aserialize_bool("fullscreen_pointer"  , &showpointers[  1]);
        aserialize_bool("fullscreen_sidebar"  , &showsidebars[  1]);
        aserialize_bool("fullscreen_statusbar", &showstatusbars[1]);
        aserialize_bool("fullscreen_titlebar" , &showtitlebars[ 1]);
        aserialize_bool("fullscreen_toolbar"  , &showtoolbars[  1]);
    }
    aserialize_bool("generate"            , &generate          );
    aserialize_bool("generateanim"        , &iffanims          );
    aserialize_bool("generateavi"         , &avianims          );
    aserialize_bool("generategif"         , &gifanims          );
    aserialize_bool("generatemng"         , &mnganims          );
    aserialize_bool("generatepng"         , &apnganims         );
    aserialize_bool("generatepsg"         , &psgfile           );
    aserialize_bool("generate8svx"        , &esvxfile          );
    aserialize_bool("generateaiff"        , &aifffile          );
    aserialize_bool("generatemidi"        , &midifile          );
    aserialize_bool("generatesmus"        , &smusfile          );
    aserialize_bool("generatewav"         , &wavfile           );
    aserialize_bool("generateym"          , &ymfile            );
    aserialize_bool("guestrmb"            , &guestrmb          );
    aserialize_bool("guidepixel"          , &useguideray       );
    aserialize_bool("inversevideo"        , &inverse           );
    aserialize_bool("inverttape"          , (int*) &inverttape );
#ifdef AMIGA
    aserialize_bool("ledfilter"           , &led               );
#endif
    aserialize_bool("limitrefreshes"      , &limitrefreshes    );
    aserialize_bool("logbios"             , &logbios           );
    aserialize_bool("loginefficient"      , &loginefficient    );
    aserialize_bool("logillegal"          , &log_illegal       );
    aserialize_bool("loginterrupts"       , &log_interrupts    );
    aserialize_bool("logreadwrites"       , &logreadwrites     );
    aserialize_bool("logsubroutines"      , &logsubroutines    );
    aserialize_bool("loop"                , &loop              );
    aserialize_bool("lowercase"           , &lowercase         );
    aserialize_bool("paused"              , (int*) &paused     );
    aserialize_bool("pausebreak"          , &pausebreak        );
    aserialize_bool("pauselog"            , &pauselog          );
    aserialize_bool("palladium_gameinfo"  , (int*) &showpalladiumkeys1);
    aserialize_bool("palladium_redefinekeys", (int*) &showpalladiumkeys2);
    aserialize_bool("pipbug_blink"        , &blink             );
    aserialize_bool("pong_lockhorizontal" , &lockhoriz         );
    aserialize_bool("pong_playerid"       , &playerid          );
    aserialize_bool("post"                , &post              );
    aserialize_bool("queuekeystrokes"     , &queuekeystrokes   );
    aserialize_bool("randomizememory"     , &randomizememory   );
#ifdef WIN32
    aserialize_bool("reassociate"         , &reassociate       );
#endif
    aserialize_bool("requirebutton_left"  , &requirebutton[0]  );
    aserialize_bool("requirebutton_right" , &requirebutton[1]  );
#ifdef WIN32
    aserialize_bool("retroachievements"   , &cheevos           );
#endif
    aserialize_bool("retune"              , &retune            );
    aserialize_bool("rotatescreen"        , &rotate            );
    aserialize_bool("scanlines"           , &scanlines         );
    aserialize_bool("showpositions"       , &showpositions     );
    aserialize_bool("showtips"            , &showtod           );
    aserialize_bool("sensegame"           , &sensegame         );
    aserialize_bool("showleds"            , &showleds          );
    aserialize_bool("sound"               , (int*) &sound      );
    aserialize_bool("speech"              , &usespeech         );
    aserialize_bool("springloaded"        , &springloaded      );
    aserialize_bool("startupupdates"      , &startupupdates    );
#ifdef WIN32
    aserialize_bool("stretchntsc"         , &stretch43         );
#endif
    aserialize_bool("stubprograms"        , &usestubs          );
    aserialize_bool("swapped"             , (int*) &swapped    );
    aserialize_bool("teletypesounds"      , &ambient           );
    aserialize_bool("trainer_lives"       , &trainer_lives     );
    aserialize_bool("trainer_time"        , &trainer_time      );
    aserialize_bool("trainer_invincibility",&trainer_invincibility);
    aserialize_bool("turbo"               , (int*) &turbo      );
    aserialize_bool("verbosedisk"         , (int*) &verbosedisk);
    aserialize_bool("verbosetape"         , (int*) &verbosetape);
    aserialize_bool("viewbuiltin"         , &showbuiltin       );
    aserialize_bool("watchreads"          , &watchreads        );
    aserialize_int( "watchwrites"         , &watchwrites, 0, 2 );
    aserialize_bool("windowed_debugger"   , &showdebugger[  0] );
#ifdef WIN32
    aserialize_bool("windowed_menubar"    , &showmenubars[  0] );
#endif
    aserialize_bool("windowed_pointer"    , &showpointers[  0] );
    aserialize_bool("windowed_titlebar"   , &showtitlebars[ 0] );
    aserialize_bool("windowed_toolbar"    , &showtoolbars[  0] );
    aserialize_bool("windowed_sidebar"    , &showsidebars[  0] );
    aserialize_bool("windowed_statusbar"  , &showstatusbars[0] );
    aserialize_bool("writeprotect"        , (int*) &drive[0].writeprotect);
    aserialize_bool("writeprotect2"       , (int*) &drive[1].writeprotect);
    aserialize_bool("writeprotect3"       , (int*) &tapewriteprotect);
    aserialize_bool("writeprotect4"       , (int*) &papertapeprotect[0]);

    for (i = 0; i < 24; i++)
    {   for (j = 0; j < COLOURSETS; j++)
        {   sprintf((char*) tempstring, "colour_%02d_%d", i, j);
            if (serializemode == SERIALIZE_WRITE)
            {   red   = getred(  j, i);
                green = getgreen(j, i);
                blue  = getblue( j, i);
            }
#ifdef __GNUC__
            else
            {   red = green = blue = 0; // otherwise GCC emits a spurious warning about these being uninitialized
            }
#endif
            aserialize_colours((STRPTR) tempstring, &red, &green, &blue);
            if (serializemode == SERIALIZE_READ)
            {   setred(  j, i, red);
                setgreen(j, i, green);
                setblue( j, i, blue);
    }   }   }

    aserialize_str("path_disks"      , (STRPTR) path_disks);
    aserialize_str("path_games"      , (STRPTR) path_games);
    aserialize_str("path_projects"   , (STRPTR) path_projects);
    aserialize_str("path_screenshots", (STRPTR) path_screenshots);
    aserialize_str("path_scripts"    , (STRPTR) path_scripts);
    aserialize_str("path_tapes"      , (STRPTR) path_tapes);
#ifdef WIN32
    aserialize_str("file_bkgrnd"     , (STRPTR) fn_bkgrnd);
#endif

    for (i = 0; i < 8; i++)
    {   sprintf((char*) tempstring, "recent_%d", i);
        aserialize_str((STRPTR) tempstring, (STRPTR) &recent[i][3]);
    }
    if (serializemode == SERIALIZE_READ)
    {   if   (recent[7][3]) recents = 8;
        elif (recent[6][3]) recents = 7;
        elif (recent[5][3]) recents = 6;
        elif (recent[4][3]) recents = 5;
        elif (recent[3][3]) recents = 4;
        elif (recent[2][3]) recents = 3;
        elif (recent[1][3]) recents = 2;
        elif (recent[0][3]) recents = 1;
        else                recents = 0;
    }

    aserialize_int(  "base"               , &base                   ,             0, 3); // binary, octal, decimal, hex
    aserialize_int(  "binbug_baudrate"    , &binbug_baudrate        ,             0, 3); // 150, 300, 1200, 2400
    aserialize_int(  "binbug_biosversion" , &binbug_biosver         ,             0, 5); // binbug 3.5, binbug 3.6, binbug 6.1, gbug, mikebug, mybug
    aserialize_int(  "binbug_dosversion"  , &binbug_dosver          ,             0, 2); // microdos, vhs dos, none
    aserialize_int(  "binbug_interface"   , (int*) &binbug_interface,             0, 1); // EA77CC4, ACOS
    aserialize_int(  "binbug_userdrives"  , (int*) &binbug_userdrives,            1, 2);
#ifdef WIN32
    aserialize_int(  "bitrate"            , &bitrate                ,             8, 16); // 8, 16
#endif
    aserialize_int(  "cd2650_biosversion" , &cd2650_biosver         ,             0, 2); // IPL, POPMON, Supervisor
    aserialize_int(  "cd2650_dosversion"  , &cd2650_dosver          ,             0, 2); // Central Data DOS, P1 DOS, None
    aserialize_int(  "cd2650_userdrives"  , (int*) &cd2650_userdrives,            1, 4);
    aserialize_int(  "cd2650_vdu"         , &cd2650_vdu             ,             0, 2); // ASCII, chessmen, lowercase
#ifdef AMIGA
    aserialize_int(  "clipunit"           , (int*) &clipunit        ,             0, 255);
#endif
    aserialize_int(  "colourset"          , &colourset              ,             0, 5); // amber, green, greyscale, pure colours, PVI colours, UVI colours
#ifdef WIN32
    aserialize_int(  "consolebg"          , &consolebg              ,             0, 1); // black, white
#endif
    aserialize_int(  "controller_left"    , &hostcontroller[0]      ,             0, 8); // host-specific
    aserialize_int(  "controller_right"   , &hostcontroller[1]      ,             0, 8); // host-specific
    aserialize_int(  "debugger_drive"     , &debugdrive             ,             0, 1);
    aserialize_ubyte("dips_astrowars"     , &aw_dips1);
    aserialize_ubyte("dips_astrowars_2"   , &aw_dips2);
    aserialize_ubyte("dips_galaxia"       , &ga_dips);
    aserialize_ubyte("dips_laserbattle"   , &lb_dips);
    aserialize_ubyte("dips_lazarian_2"    , &lz_dips2);
    aserialize_ubyte("dips_lazarian_3"    , &lz_dips3);
    aserialize_int(  "elektor_biosversion", &elektor_biosver        ,             0, 1); // Philips, Hobby Module
    aserialize_int(  "filter"             , &filter                 ,             0, 3); // 3D, HQx, Scale2x, none
#ifdef AMIGA
    if (filter == FILTER_3D || filter == FILTER_HQX)
    {   filter = FILTER_NONE;
    }
#endif
    aserialize_int(  "frameskipping"      , &frameskip              , FRAMESKIP_MIN, FRAMESKIP_INFINITE);
#ifdef WIN32
    aserialize_int("language"             , &language               ,             0, LANGUAGES - 1);
#endif
    aserialize_int("logtofile"            , &logfile                ,             0, 2); // append, ignore, replace
#ifdef WIN32
    aserialize_int("keyboardtype"         , (int*) &viewkybdas2     ,             0, 1); // desktop, netbook. Host-specific
#endif
#ifdef AMIGA
    aserialize_int("keyboardtype"         , (int*) &viewkybdas2     ,             0, 2); // a600, a1000, other amiga. Host-specific
#endif
    aserialize_int("keymap"               , (int*) &keymap          ,             0, 2); // host, ea, tt
    aserialize_int("memorymap"            , &memmap                 ,             0, MEMMAPS - 1);
    if (serializemode == SERIALIZE_READ)
    {   machine = memmapinfo[memmap].machine;
    }
    aserialize_uword("netport"            , &netport                ,             0, 65535);
    aserialize_int("nexttip"              , &nexttod                ,             0, TIPSOFDAYS - 1);
    aserialize_int("notation"             , &style                  ,             0, 3); // non-extended, extended, calm, ieee
    aserialize_int("phunsy_biosversion"   , &phunsy_biosver         ,             0, 1); // mini-monitor, phunsy
    aserialize_int("pipbug_baudrate"      , &pipbug_baudrate        ,             0, 2); // 110, 300, 4800
    aserialize_int("pipbug_biosversion"   , &pipbug_biosver         ,             0, 3); // Artemis, HYBUG, PIPBUG 1, PIPBUG 2
    aserialize_int("pipbug_peripheral"    , &pipbug_periph          ,             0, 3); // EA printer, Linearization, VM, WFC
    if (configversion == NEWER)
    {   aserialize_int("pipbug_vdu"       , &pipbug_vdu             ,             0, 5); // elekterminal, narrow lcvdu, wide lcvdu, rb, svt-100, vt100
    } else
    {   aserialize_int("pipbug_vdu"       , &pipbug_vdu             ,             0, 4); // elekterminal, narrow lcvdu, wide lcvdu, svt-100, vt100
        if (pipbug_vdu >= 3)
        {   pipbug_vdu++; // 3..4 -> 4..5
    }   }
    aserialize_int("pong_angles"          , &angles                 ,             0, 2); // 2, 4, random
    aserialize_int("pong_batsizes"        , &batvalue               ,             0, 3); // short+short, tall+tall, short+tall, tall+short
    aserialize_int("pong_machine"         , (int*) &pong_machine    ,             0, 2); // AY-3-8500 (1976), AY-3-8500 (1977), AY-3-8550
    aserialize_int("pong_players"         , &players                ,             0, 3); // 2, 3 (left team), 3 (right team), 4
    aserialize_int("pong_serving"         , &serving                ,             0, 1); // auto, manual
    aserialize_int("pong_speed"           , &pongspeed              ,             0, 2); // slow, fast, random
    aserialize_int("pong_variant"         , &pong_variant           ,             0, VARIANTS - 1);
#ifdef WIN32
    aserialize_int("priority"             , &priority               ,             0, 2); // high, normal, low
#endif
    for (i = 0; i < 8; i++)
    {   sprintf((char*) tempstring, "recentmemmap_%d", i);
        aserialize_int((char*) tempstring , &recentmemmap[i]        ,            -1, MEMMAPS    - 1); // -1..30
        sprintf((char*) tempstring, "recentgame_%d", i);
        aserialize_int((char*) tempstring , &recentgame[i]          ,            -1, KNOWNGAMES - 1); // -1..KNOWNGAMES-1
    }
    aserialize_int("region"               , (int*) &region          ,             0, 1); // ntsc, pal
    aserialize_int("regionstart"          , &regionstart            ,             0, 32767 - 256);
    aserialize_int("robot_left"           , &robotspeed[0]          ,             0, ROBOTSPEED_INFINITE);
    aserialize_int("robot_right"          , &robotspeed[1]          ,             0, ROBOTSPEED_INFINITE);
#ifdef WIN32
    aserialize_int("samplerate"           , &samplerate             ,         11025, 44100); // 11025, 22050, 44100
#endif
#ifdef AMIGA
    aserialize_int("samplerate"           , &samplerate             ,         11025, 22050); // 11025, 22050
#endif
    aserialize_int("selbst_biosversion"   , &selbst_biosver         ,             0, 3); // none, 0.9, 1.0, 2.0
    aserialize_int("sensitivity_left"     , &sensitivity[0]         ,             1, 5); // very low, low, medium, high, very high
    aserialize_int("sensitivity_right"    , &sensitivity[1]         ,             1, 5); // very low, low, medium, high, very high
    aserialize_int("sidebarwidth"         , &sidebarwidth           ,             0, 65535);
    aserialize_int("si50_intselector"     , &s_is                   ,             0, 1); // AC line, keyboard
    aserialize_int("si50_interrupts"      , &s_id                   ,             0, 1); // direct, indirect
    aserialize_int("si50_parallelio"      , &s_io                   ,             0, 2); // memory mapped, extended, non-extended
    aserialize_ubyte("si50_toggles"       , &s_toggles);
#ifdef AMIGA
    if (serializemode == SERIALIZE_WRITE && wsm == 1)
    {   aserialize_int("size"             , &windowed_size          ,             1, 6); // 1..6
    } else
#endif
        aserialize_int("size"             , &size                   ,             1, 6); // 1..6
    aserialize_int("sortby"               , &sortby                 ,             0, 1); // game name, machine
#ifdef AMIGA
    aserialize_int("soundoutput"          , &soundoutput            ,             0, 1); // AHI V4+, audio.device
#endif
#ifdef WIN32
    aserialize_int("speech_rate"          , &speech_rate            ,             0, 20); // 0..20 (ie. -10..+10)
    if (serializemode == SERIALIZE_WRITE)
    {   WideCharToMultiByte // fag to normal
        (   CP_ACP,
            0,
            voicename,
            -1,
            asciivoice,
            80, // not including terminator
            NULL,
            NULL
        );
    }
    aserialize_str("speech_voice"     ,  asciivoice);
    if (serializemode == SERIALIZE_READ)
    {   MultiByteToWideChar // normal to fag
        (   CP_ACP,
            0,
            asciivoice,
            -1,
            voicename,
            80 // not including terminator
        );
    }
#endif
#ifdef AMIGA
    aserialize_int("speech_pitch"         , (int*) &speech_pitch    ,            65, 320);
    aserialize_ubyte("speech_race"        , (UBYTE*) &speech_race);
    aserialize_ubyte("speech_sex"         , (UBYTE*) &speech_sex);
    aserialize_int("speech_wpm"           , (int*) &speech_wpm      ,            40, 400);
    aserialize_int("speech_articulation"  , (int*) &speech_articulation,          0, 255);
    aserialize_int("speech_centralization", (int*) &speech_centralizenum,         0, 100);
    aserialize_int("speech_centralizeto"  , (int*) &speech_centralizeto,          0,  10);
    aserialize_int("speech_enthusiasm"    , (int*) &speech_enthusiasm,            0, 255);
    aserialize_int("speech_frication"     , (int*) &speech_frication,           -32,  31);
    aserialize_int("speech_perturbation"  , (int*) &speech_perturbation,          0, 255);
    aserialize_int("speech_voicing"       , (int*) &speech_voicing,             -32,  31);
#endif
    aserialize_int("speed"                , &speedup                ,     SPEED_MIN, SPEED_MAX);
#ifdef WIN32
    aserialize_int("stretch"              , &stretching             ,             0, 2); // unstretched, stretch 4:3, stretch to fit
#endif
    aserialize_ubyte("tapeskewage"        , &tapeskewage);
    aserialize_int("timeunit"             , &timeunit               ,             0, 2); // cycles, clocks, pixels
    aserialize_int("twin_dosversion"      , &twin_dosver            ,             0, 6); // EXOS, SDOS 2.0, SDOS 4.0, SDOS 4.2, TOS, UDOS, None
    aserialize_int("twin_printerunit"     , &prtunit                ,             0, 1); // 1st, 2nd
    for (i = 0; i < 2; i++)
    {   sprintf((char*) tempstring, "twin_ribbon_%d", i);
        aserialize_int((STRPTR) tempstring, &(printer[i].ribbon)    ,             0, 3); // black, red, green, blue

        sprintf((char*) tempstring, "twin_condensed_%d", i);
        aserialize_int((STRPTR) tempstring, &(printer[i].condensed) ,             0, 1); // normal, condensed
    }
    aserialize_int("verbosity"            , &verbosity              ,             0, 2); // minimum, table, maximum
    aserialize_int("viewctrlsas_arcadia"  , (int*) &arcadia_viewcontrolsas ,      0, 2); // Emerson, MPT-03, Palladium
    aserialize_int("viewctrlsas_8550"     , (int*) &pong8550_viewcontrolsas,      0, 1); // Interton 3000, Sheen
    aserialize_int("viewctrlsas_8600"     , (int*) &pong8600_viewcontrolsas,      0, 1); // Telstar, Telejogo
    aserialize_int("viewdiskas"           , (int*) &viewdiskas      ,             0, 1); // hex, characters
    aserialize_int("viewkybdas"           , (int*) &viewkybdas      ,             0, 2); // guest, host, overlays
    aserialize_int("viewmemoryas"         , (int*) &viewmemas       ,             0, 1); // hex, characters
    aserialize_int("viewmemorymapas"      , (int*) &viewmemas2             ,      0, 2); // contents, memory map, coverage report
    aserialize_int("viewpadsas"           , (int*) &viewpadsas      ,             0, 2); // guest, host, overlays
#ifdef AMIGA
    aserialize_int("viewpadsas2"          , (int*) &viewpadsas2     ,             0, 1); // CD32, Megadrive
#endif
    aserialize_int("viewspeedas"          , &viewspeedas            ,             0, 2); // fps, khz, percents
    aserialize_int("volume"               , &hostvolume             ,HOSTVOLUME_MIN, HOSTVOLUME_MAX);
    aserialize_int("narrowprinter"        , (int*) &narrowprinter   ,             0, 1); // 0, 1
#ifdef AMIGA
    if (serializemode == SERIALIZE_WRITE && wsm == 1)
    {   aserialize_int("width"            , &windowed_wide          ,             1, 2); // 1..2
    } else
#endif
        aserialize_int("width"            , &wide                   ,             1, 2); // 1, 2
    aserialize_int("winleftx"             , &winleftx               ,        -32768, 32767);
    aserialize_int("wintopy"              , &wintopy                ,        -32768, 32767);
    aserialize_uword("console_start"      , &console[0]             ,             0, SCANCODES - 1); // host-specific
    aserialize_uword("console_a"          , &console[1]             ,             0, SCANCODES - 1); // host-specific
    aserialize_uword("console_b"          , &console[2]             ,             0, SCANCODES - 1); // host-specific
    aserialize_uword("console_reset"      , &console[3]             ,             0, SCANCODES - 1); // host-specific
    for (i = 0; i < 28; i++)
    {   sprintf((char*) tempstring, "left_%02d", i);
        aserialize_uword((STRPTR) tempstring, &keypads[0][i]        ,             0, SCANCODES - 1); // host-specific
    }
    for (i = 0; i < 28; i++)
    {   sprintf((char*) tempstring, "right_%02d", i);
        aserialize_uword((STRPTR) tempstring, &keypads[1][i]        ,             0, SCANCODES - 1); // host-specific
    }
#ifdef WIN32
    for (i = 0; i < 8; i++)
    {   sprintf(tempstring, "leftbutton_%d", i);
        aserialize_ubyte(         tempstring, &button[0][i]);
    }
    for (i = 0; i < 8; i++)
    {   sprintf(tempstring, "rightbutton_%d", i);
        aserialize_ubyte(         tempstring, &button[1][i]);
    }
#endif
#ifdef AMIGA
    for (i = 0; i < 4; i++)
    {   sprintf((char*) tempstring, "leftbutton_%d", i);
        aserialize_ubyte((STRPTR) tempstring, &button[0][i]);
    }
    for (i = 0; i < 4; i++)
    {   sprintf((char*) tempstring, "rightbutton_%d", i);
        aserialize_ubyte((STRPTR) tempstring, &button[1][i]);
    }
#endif
}

EXPORT int guest_to_host_joy(int guestjoy)
{   // Ascertains "which host joystick is assigned to this guest joystick?"

    if     (hostcontroller[guestjoy] == CONTROLLER_1STDJOY || hostcontroller[guestjoy] == CONTROLLER_1STAPAD)
    {   return 0;
    } elif (hostcontroller[guestjoy] == CONTROLLER_2NDDJOY || hostcontroller[guestjoy] == CONTROLLER_2NDAPAD)
    {   return 1;
    } else
    {   return 2; // error!
}   }

EXPORT UBYTE loadbyte(void)
{   // assert(recmode == RECMODE_PLAY);
    return IOBuffer[offset++];
}
EXPORT void savebyte(UBYTE value)
{   // assert(recmode == RECMODE_RECORD);

    OutputBuffer[0] = value;
    DISCARD fwrite(OutputBuffer, 1, 1, MacroHandle); // should really check return code
}

EXPORT void update_speed(void)
{   // 1 / frequency = 2 * (PITCH+1) * horizontal line period
    // or, frequency = (2 * horizontal line period / 1,000,000) / (PITCH+1)
    // or, frequency = (horizontal line period / 500,000) / (PITCH+1)
    if (region == REGION_NTSC)
    {   if (exactspeed)
        {   dividend = 7874.0; // 63.5     / 500,000 = 7874.0
        } else
        {   dividend = 7884.5; // 63.41588 / 500,000 = 7884.460485
    }   }
    else
    {   dividend = 7812.5; // 64       / 500,000 = 7812.5
    }

    if (!MainWindowPtr)
    {   return;
    }

    if (SubWindowPtr[SUBWINDOW_SPEED])
    {   sl_set(SUBWINDOW_SPEED, IDC_ADJUSTSPEED, speedup);
#ifdef WIN32
        sprintf(gtempstring, "%d", speedupnum[speedup]);
        st_set(SUBWINDOW_SPEED, IDC_EMUSPEED_NUM);
#endif
#ifdef AMIGA
        SetGadgetAttrs(gadgets[GID_SD_IN1], SubWindowPtr[SUBWINDOW_SPEED], NULL, INTEGER_Number, (ULONG) speedupnum[speedup], TAG_DONE); // this autorefreshes
#endif
    }

    if (showstatusbars[wsm])
    {   switch (viewspeedas)
        {
        case  VIEWAS_FPS:      strcpy((char*) StatusText[1], LLL(MSG_FPS_GAD, "FPS:"  ));
        acase VIEWAS_KHZ:      strcpy((char*) StatusText[1], LLL(MSG_KHZ_GAD, "kHz:"  ));
        acase VIEWAS_PERCENTS: strcpy((char*) StatusText[1], LLL(MSG_SPD_GAD, "Speed:"));
        }

#ifdef WIN32
        // assert(hStatusBar);
        SendMessage(hStatusBar, SB_SETTEXT, 2 | SBT_NOBORDERS, (LPARAM) StatusText[1]);
#endif
#ifdef AMIGA
        SetGadgetAttrs(gadgets[GID_MA_BU2], MainWindowPtr, NULL, GA_Text, StatusText[1], TAG_DONE); // this autorefreshes
#endif

        update_fps();
    }

    reset_fps();
}

EXPORT int getmonitoraddr(int whichgad)
{   int whichaddr;

    whichaddr = monitor[whichgad].addr;
    if
    (   (whichgad >= PVI1ST_FIRSTMONGAD && whichgad <= PVI1ST_LASTMONGAD)
     || (whichgad >= PVI2ND_FIRSTMONGAD && whichgad <= PVI2ND_LASTMONGAD)
     || (whichgad >= PVI3RD_FIRSTMONGAD && whichgad <= PVI3RD_LASTMONGAD)
    )
    {   whichaddr += pvibase;
    }

    return whichaddr;
}

EXPORT void update_capslock(void)
{   if (machine == TYPERIGHT)
    {   tr_update_capslock();
    }

    if (!MainWindowPtr || !showstatusbars[wsm])
    {   return;
    }

    if (capslock)
    {   strcpy((char*) StatusText[3], (STRPTR) LLL(MSG_CAPSLOCK, "CAPS"));
    } else
    {
#ifdef LOWERCAPS
        strcpy((char*) StatusText[3], (STRPTR) LLL(MSG_CAPSLOCK, "CAPS"));
        DISCARD strlwr((char*) StatusText[3]);
#else
        StatusText[3][0] = EOS;
#endif
    }

#ifdef AMIGA
    SetGadgetAttrs(gadgets[GID_MA_ST3], MainWindowPtr, NULL, STRINGA_TextVal, StatusText[3], TAG_DONE); // this autorefreshes
#endif
#ifdef WIN32
    SendMessage(hStatusBar, SB_SETTEXT, 8, (LPARAM) StatusText[3]);
#endif
}

EXPORT void setmonused(void)
{   int i;

    // Only the XVI monitor uses this.

    for (i = XVI_FIRSTMONGAD; i < XVI_LASTMONGAD; i++)
    {   monitor[i].used = FALSE;
    }

    if (machines[machine].pvis >= 1)
    {   for (i = PVI1ST_FIRSTMONGAD; i <= PVI1ST_LASTMONGAD; i++)
        {   monitor[i].used = TRUE;
        }
        if (machines[machine].pvis >= 2)
        {   for (i = PVI2ND_FIRSTMONGAD; i <= PVI2ND_LASTMONGAD; i++)
            {   monitor[i].used = TRUE;
            }
            if (machines[machine].pvis >= 3)
            {   for (i = PVI3RD_FIRSTMONGAD; i <= PVI3RD_LASTMONGAD; i++)
                {   monitor[i].used = TRUE;
    }   }   }   }

    switch (machine)
    {
    case ARCADIA:
        for (i = ARCADIA_FIRSTMONGAD; i <= ARCADIA_LASTMONGAD; i++)
        {   monitor[i].used = TRUE;
        }
    acase INTERTON:
        for (i = I_FIRSTMONGAD; i <= IE_LASTMONGAD; i++)
        {   monitor[i].used = TRUE;
        }
        for (i = INTERTON_FIRSTMONGAD; i <= INTERTON_LASTMONGAD; i++)
        {   monitor[i].used = TRUE;
        }
    acase ELEKTOR:
        for (i = E_FIRSTMONGAD; i <= IE_LASTMONGAD; i++)
        {   monitor[i].used = TRUE;
}   }   }

EXPORT void monchange(int which)
{   int  address;
    TEXT tempstring[40 + 1]; // shorten

    address = getmonitoraddr(which);
    sprintf((char*) tempstring, "$%X", address);
    debug_edit((STRPTR) tempstring, TRUE, FALSE);
}

EXPORT void ghostmonitor(void)
{   if (machine != TYPERIGHT)
    {   ghostmonitorbutton(SUBWINDOW_MONITOR_CPU, monitor[MONGAD_UF1].gadget, supercpu ? FALSE : TRUE);
        ghostmonitorbutton(SUBWINDOW_MONITOR_CPU, monitor[MONGAD_UF2].gadget, supercpu ? FALSE : TRUE);
}   }

EXPORT void close_subwindows(FLAG all)
{   int i;

    for (i = 0; i < SUBWINDOWS; i++)
    {   if (SubWindowPtr[i] && (all || i != SUBWINDOW_OUTPUT))
        {   close_subwindow(i);
            need[i] = TRUE;
}   }   }

EXPORT void reopen_subwindows(void)
{   int i;

    if (!MainWindowPtr)
    {   return;
    }

#ifdef WIN32
    if (need[SUBWINDOW_OUTPUT      ]) open_output(FALSE);
#endif

    if (need[SUBWINDOW_CONTROLS    ]) view_controls();
    if (need[SUBWINDOW_DIPS        ]) edit_dips();
    if (need[SUBWINDOW_FLOPPYDRIVE ]) open_floppydrive(FALSE); // or TRUE if there were substantial differences between DOSes
    if (need[SUBWINDOW_GAMEINFO    ]) help_gameinfo();
    if (need[SUBWINDOW_MEMORY      ]) edit_memory();
    if (need[SUBWINDOW_MONITOR_CPU ]) view_monitor(SUBWINDOW_MONITOR_CPU);
    if (need[SUBWINDOW_OPCODES     ]) help_opcodes();
    if (need[SUBWINDOW_PALETTE     ]) edit_palette();
    if (need[SUBWINDOW_PRINTER     ]) tools_printer();
    if (need[SUBWINDOW_SPRITES     ]) open_spriteeditor();
    if (need[SUBWINDOW_SPEED       ]) open_speed();
    if (need[SUBWINDOW_TAPEDECK    ]) open_tapedeck();
    if (need[SUBWINDOW_HOSTKYBD    ]) help_hostkybd();
    if (need[SUBWINDOW_HOSTPADS    ]) help_hostpads();
    if (need[SUBWINDOW_MONITOR_PSGS]) view_monitor(SUBWINDOW_MONITOR_PSGS);
    if (need[SUBWINDOW_MONITOR_XVI ]) view_monitor(SUBWINDOW_MONITOR_XVI);
    if (need[SUBWINDOW_MUSIC       ]) tools_music();
    if (need[SUBWINDOW_PAPERTAPE   ]) open_papertape();
    if (need[SUBWINDOW_INDUSTRIAL  ]) open_industrial();

    for (i = 0; i < SUBWINDOWS; i++)
    {   need[i] = FALSE;
    }

#ifdef AMIGA
    ActivateWindow(MainWindowPtr);
#endif
#ifdef WIN32
    SetActiveWindow(MainWindowPtr);
#endif
}

EXPORT void reopen_subwindow(int which)
{   if (SubWindowPtr[which])
    {   close_subwindow(which);
        need[which] = TRUE;
        reopen_subwindows();
}   }

EXPORT void command_changemachine(int whichmachine, int whichmemmap)
{   if (crippled)
    {   return;
    }

    if (memmap != whichmemmap)
    {   changemachine(whichmachine, whichmemmap, TRUE, FALSE, FALSE);
        update_opcodes(); // because our INTERTON emulation has a different length for the $10 opcode
        updatemenus();
        updatebiggads();
        updatesmlgads();
        settitle();
#ifdef WIN32
        free_display();
        make_display();
#endif
        redrawscreen();
        reopen_subwindows();
}   }

EXPORT void updatekeynames(HWND hwnd)
{   int   i, j, k,
          matches,
          val;
    ULONG maybeswapped;
    TEXT  tempstring[128 + 5 + 1];

    if (machines[machine].joystick)
    {   maybeswapped = swapped;
    } else
    {   maybeswapped = FALSE;
    }

    switch (viewkybdas)
    {
    case 0: // guest
    case 2: // overlays
        if (ISQWERTY)
        {   for (i = 0; i < SCANCODES; i++)
            {   if (keyname[i].gadget != -1)
                {   matches = 0;
                    if (i != SCAN_P && i != SCAN_TAB && (keyname[i].reserved || i == SCAN_LSHIFT || i == SCAN_RSHIFT))
                    {   setkybdtext(i, hwnd, keyname[i].gadget, keyname[i].name);
                        matches = 1;
                    } elif (keyname[i].unshifted[keymap] == NC)
                    {   setkybdtext(i, hwnd, keyname[i].gadget, "");
                        // matches = 0;
                    } else
                    {   if (machine == TYPERIGHT)
                        {   if (keyname[i].tr_unshifted == NC)
                            {   setkybdtextandtips(i, hwnd, keyname[i].gadget, "", "");
                                matches = 0;
                            } else
                            {   sprintf
                                (   (char*) tempstring,
                                    LLL(
                                        MSG_QWERTYTOOLTIP,
                                        "Unshifted: %s\n" \
                                        "Shifted/Caps Lock: %s"
                                    ),
                                    asciiname_long[(keymap == KEYMAP_HOST) ?   keyname[i].unshifted[KEYMAP_HOST]
                                                                           :   keyname[i].tr_unshifted],
                                    asciiname_long[(keymap == KEYMAP_HOST) ? ((keyname[i].shifted[KEYMAP_HOST] == '^') ? '-' : keyname[i].shifted[KEYMAP_HOST])
                                                                           :   keyname[i].tr_shifted]
                                );
                                setkybdtextandtips(i, hwnd, keyname[i].gadget, (STRPTR) asciiname_short[(keymap == KEYMAP_HOST) ? keyname[i].unshifted[KEYMAP_HOST] : keyname[i].tr_unshifted], (STRPTR) tempstring);
                                matches = 1;
                        }   }
                        else
                        {   j = scan_to_scan(i);
                            sprintf
                            (   (char*) tempstring,
                                LLL(
                                    MSG_QWERTYTOOLTIP,
                                    "Unshifted: %s\n" \
                                    "Caps Lock: %s\n" \
                                    "Shifted: %s\n" \
                                    "Ctrl: %s\n" \
                                    "Ctrl+Shift: %s"
                                ),
                                asciiname_long[keyname[i].unshifted[keymap]],
                                asciiname_long[keyname[i].capslock[keymap]],
                                asciiname_long[keyname[i].shifted[keymap]],
                                asciiname_long[keyname[i].ctrl],
                                asciiname_long[keyname[i].ctrlshift]
                            );
                            setkybdtextandtips(j, hwnd, keyname[i].gadget, (STRPTR) asciiname_short[keyname[j].unshifted[keymap]], (STRPTR) tempstring);
                            // matches = 1;
                    }   }
                    if (machine == TYPERIGHT)
                    {   k = -1;
                        for (j = 0; j < 4; j++)
                        {   if (console[j] == i)
                            {   k = j;
                                matches++;
                        }   }
                        if (k != -1)
                        {   if (matches >= 2)
                            {   setkybdtext(i, hwnd, keyname[i].gadget, "#");
                            } else
                            {   // assert(matches == 1);
                                setkybdtext(i, hwnd, keyname[i].gadget, machines[machine].consolekeyname[k]);
        }   }   }   }   }   }
        else
        {   for (i = 0; i < SCANCODES; i++)
            {   if (keyname[i].gadget != -1)
                {   matches = 0;
                    for (j = 0; j <= 1; j++)
                    {   for (k = 0; k < NUMKEYS; k++)
                        {   if (keypads[j][k] == i && keyexists(k))
                            {   matches++;
                    }   }   }
                    for (j = 0; j < 4; j++)
                    {   if (console[j] == i && machine != SELBST)
                        {   matches++;
                    }   }

                    if (matches == 0)
                    {   setkybdtext(i, hwnd, keyname[i].gadget, keyname[i].reserved ? keyname[i].name : (STRPTR) "");
                    } elif (matches == 1)
                    {   for (j = 0; j < NUMKEYS; j++)
                        {   if ((keypads[0][j] == i || keypads[1][j] == i) && keyexists(j))
                            {   switch (j)
                                {
                                case 0:
                                case 21:
                                case 22:
                                case 23:
                                    val = ((j == 0) ? key1 : ((j == 21) ? key2 : ((j == 22) ? key3 : key4)));
                                    if (viewkybdas == 0)
                                    {   if (keypads[0][j] == i)
                                        {   setkybdtext(i, hwnd, keyname[i].gadget, machines[machine].keynames[maybeswapped ? 1 : 0][num_to_num[val]]);
                                        } else
                                        {   setkybdtext(i, hwnd, keyname[i].gadget, machines[machine].keynames[maybeswapped ? 0 : 1][num_to_num[val]]);
                                    }   }
                                    else
                                    {   // assert(viewkybdas == 2);
                                        if (keypads[0][j] == i)
                                        {   setkybdtext(i, hwnd, keyname[i].gadget, overlays[whichoverlay][maybeswapped ? keytable[val].p2overlay : keytable[val].p1overlay]);
                                        } else
                                        {   setkybdtext(i, hwnd, keyname[i].gadget, overlays[whichoverlay][maybeswapped ? keytable[val].p1overlay : keytable[val].p2overlay]);
                                    }   }
                                adefault:
                                    if     (j == 17 && paddleup    != -1 && viewkybdas == 0)
                                    {   setkybdtext(i, hwnd, keyname[i].gadget, machines[machine].keynames[maybeswapped ? 1 : 0][num_to_num[paddleup   ]]);
                                    } elif (j == 18 && paddledown  != -1 && viewkybdas == 0)
                                    {   setkybdtext(i, hwnd, keyname[i].gadget, machines[machine].keynames[maybeswapped ? 1 : 0][num_to_num[paddledown ]]);
                                    } elif (j == 19 && paddleleft  != -1 && viewkybdas == 0)
                                    {   setkybdtext(i, hwnd, keyname[i].gadget, machines[machine].keynames[maybeswapped ? 1 : 0][num_to_num[paddleleft ]]);
                                    } elif (j == 20 && paddleright != -1 && viewkybdas == 0)
                                    {   setkybdtext(i, hwnd, keyname[i].gadget, machines[machine].keynames[maybeswapped ? 1 : 0][num_to_num[paddleright]]);
                                    }
                                    /* elif (j == 24 && paddleup != -1 && paddleleft != -1)
                                    {   setkybdtext(i, hwnd, keyname[i].gadget, "#");
                                    } elif (j == 25 && paddleup != -1 && paddleright != -1)
                                    {   setkybdtext(i, hwnd, keyname[i].gadget, "#");
                                    } elif (j == 26 && paddledown != -1 && paddleleft != -1)
                                    {   setkybdtext(i, hwnd, keyname[i].gadget, "#");
                                    } elif (j == 27 && paddledown != -1 && paddleright != -1)
                                    {   setkybdtext(i, hwnd, keyname[i].gadget, "#");
                                    } */ else
                                    {   if (viewkybdas == 0 || guestkeys[j].p1 == -1)
                                        {   if (keypads[0][j] == i)
                                            {   setkybdtext(i, hwnd, keyname[i].gadget, machines[machine].keynames[maybeswapped ? 1 : 0][j]);
                                            } else
                                            {   setkybdtext(i, hwnd, keyname[i].gadget, machines[machine].keynames[maybeswapped ? 0 : 1][j]);
                                        }   }
                                        else
                                        {   if (keypads[0][j] == i)
                                            {   setkybdtext(i, hwnd, keyname[i].gadget, overlays[whichoverlay][maybeswapped ? guestkeys[j].p2 : guestkeys[j].p1]);
                                            } else
                                            {   // assert(keypads[1][j] == i);
                                                setkybdtext(i, hwnd, keyname[i].gadget, overlays[whichoverlay][maybeswapped ? guestkeys[j].p1 : guestkeys[j].p2]);
                                    }   }   }
                                    break; // for speed
                        }   }   }

                        for (j = 0; j < 4; j++)
                        {   if (console[j] == i)
                            {   setkybdtext(i, hwnd, keyname[i].gadget, machines[machine].consolekeyname[j]);
                                break; // for speed
                    }   }   }
                    elif (matches >= 2)
                    {   setkybdtext(i, hwnd, keyname[i].gadget, "#");
        }   }   }   }
    acase 1: // host
        for (i = 0; i < SCANCODES; i++)
        {   j = scan_to_scan(i);
            if (keyname[i].gadget != -1)
            {   setkybdtext(i, hwnd, keyname[i].gadget, keyname[j].name);
    }   }   }

#ifdef AMIGA
    updatekeycolours(hwnd);
#endif
}

EXPORT void changeconsolekeynames(void)
{   machines[ARCADIA   ].consolekeyname[0] = LLL(MSG_START,    "START");
    machines[INTERTON  ].consolekeyname[0] = LLL(MSG_START,    "START");
    machines[ELEKTOR   ].consolekeyname[0] = LLL(MSG_START,    "START");
    machines[INSTRUCTOR].consolekeyname[0] = LLL(MSG_SENS,     "SENS");
    machines[ZACCARIA  ].consolekeyname[0] = LLL(MSG_P1,       "1UP");
    machines[MALZAK    ].consolekeyname[0] = LLL(MSG_P1,       "1UP");
    machines[PONG      ].consolekeyname[0] = LLL(MSG_START,    "START");

 // machines[ARCADIA   ].consolekeyname[1] = "A";
    machines[INTERTON  ].consolekeyname[1] = LLL(MSG_SELECT,   "SELECT");
 // machines[ELEKTOR   ].consolekeyname[1] = "UC";
 // machines[INSTRUCTOR].consolekeyname[1] = "INT";
    machines[ZACCARIA  ].consolekeyname[1] = LLL(MSG_P2,       "2UP");
    machines[MALZAK    ].consolekeyname[1] = LLL(MSG_P2,       "2UP");
    machines[PONG      ].consolekeyname[1] = LLL(MSG_GAME2,    "Game");

 // machines[ARCADIA   ].consolekeyname[2] = "B";
 // machines[INTERTON  ].consolekeyname[2] = "-";
 // machines[ELEKTOR   ].consolekeyname[2] = "LC";
 // machines[INSTRUCTOR].consolekeyname[2] = "MON";
    machines[ZACCARIA  ].consolekeyname[2] = LLL(MSG_COIN_A,   "Coin A");
    machines[MALZAK    ].consolekeyname[2] = LLL(MSG_COIN_A,   "Coin A");
    machines[PONG      ].consolekeyname[2] = LLL(MSG_SERVE2,   "Serve");

 // machines[ARCADIA   ].consolekeyname[3] = "-";
 // machines[INTERTON  ].consolekeyname[3] = "-";
    machines[ELEKTOR   ].consolekeyname[3] = LLL(MSG_RESET,    "RESET");
    machines[INSTRUCTOR].consolekeyname[3] = LLL(MSG_RST,      "RST");
    machines[ZACCARIA  ].consolekeyname[3] = LLL(MSG_COIN_B,   "Coin B");
    machines[MALZAK    ].consolekeyname[3] = LLL(MSG_SWITCH2,  "Test switch");
    machines[PONG      ].consolekeyname[3] = LLL(MSG_BATSIZE2, "Bat size");
}

EXPORT FLAG keyexists(int thekey)
{   if
    (   (thekey >= 1 && thekey <= 12) // the basic 12 keys
     || machine == ARCADIA // x1..x4, all directions, 1st..4th firebuttons
     || (   (machine == INTERTON || machine == ELEKTOR || machine == PONG || machine == MALZAK || memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
         && (thekey == 0 || thekey >= 17) // all directions, 1st..4th firebuttons
        )
     || (machine == ZACCARIA && (thekey == 0 || (thekey >= 19 && thekey <= 23))) // left, right, 1st..4th firebuttons
    )
    {   return TRUE;
    } // implied else
    return FALSE;
}

EXPORT void settitle_engine(void)
{   if (connected == NET_SERVER)
    {   strcat((char*) titlebartext, " (");
        strcat((char*) titlebartext, LLL(MSG_SERVER,    "server"));
        strcat((char*) titlebartext, ")");
    } elif (connected == NET_CLIENT)
    {   strcat((char*) titlebartext, " (");
        strcat((char*) titlebartext, LLL(MSG_CLIENT,    "client"));
        strcat((char*) titlebartext, ")");
    } elif (connected == NET_LISTENING)
    {   strcat((char*) titlebartext, " (");
        strcat((char*) titlebartext, LLL(MSG_LISTENING, "listening"));
        strcat((char*) titlebartext, ")");
    }

    if (recmode == RECMODE_RECORD)
    {   strcat((char*) titlebartext, " (");
        strcat((char*) titlebartext, LLL(MSG_RECORDING, "recording"));
        strcat((char*) titlebartext, ")");
    } elif (recmode == RECMODE_PLAY)
    {   strcat((char*) titlebartext, " (");
        strcat((char*) titlebartext, LLL(MSG_PLAYBACK , "playback"));
        strcat((char*) titlebartext, ")");
    }

    if (editscreen)
    {   strcat((char*) titlebartext, " (");
        strcat((char*) titlebartext, LLL(MSG_EDITINGSCREEN, "editing screen"));
        strcat((char*) titlebartext, ")");
    }

    if (paused)
    {   strcat((char*) titlebartext, " (");
        strcat((char*) titlebartext, LLL(MSG_PAUSED   , "paused"  ));
        strcat((char*) titlebartext, ")");
    } elif (turbo)
    {   strcat((char*) titlebartext, " (");
        strcat((char*) titlebartext, LLL(MSG_TURBO    , "turbo"   ));
        strcat((char*) titlebartext, ")");
    }

    if (modal)
    {   strcat((char*) titlebartext, " ");
        strcat((char*) titlebartext, LLL(MSG_CLOSE_SUBWINDOW, "(close subwindow)"));
}   }

MODULE void ConnectAsClient_IPv4(const char* Server, unsigned short Port)
{   struct hostent*    Host;
    struct sockaddr_in Addr;
#ifdef WIN32
    unsigned long      J;
#endif
#ifdef AMIGA
    char               J;

    if (!SocketBase)
    {   return;
    }
#endif

    NetClose();

    // Look up server address
#ifdef __MORPHOS__
    if (!(Host = gethostbyname((const UBYTE*) Server)))
#else
    if (!(Host = gethostbyname((STRPTR) Server)))
#endif
    {   zprintf
        (   TEXTPEN_NET,
            (STRPTR)LLL(
                MSG_NET_NOGETHOSTNAME,
                "Can't get hostname!\n"
        )   );
        return;
    } // implied else

    // Set fields of the address structure
    memset(&Addr, 0, sizeof(Addr));
    memcpy(&Addr.sin_addr, Host->h_addr, (size_t) (Host->h_length));
    Addr.sin_family = AF_INET;
    Addr.sin_port   = htons(Port);

    // Open a socket
    if ((Socket = (int) socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {   zprintf
        (   TEXTPEN_NET,
            LLL(
                MSG_NET_NOCREATECLIENT,
                "Can't create client socket!\n"
        )   );
        return;
    } // implied else

    // Connecting...

#ifdef __amigaos4__
    if (connect(               Socket, (      struct sockaddr*) &Addr,             sizeof(Addr)) < 0)
#else
    if (connect((unsigned int) Socket, (const struct sockaddr*) &Addr, (LONG     ) sizeof(Addr)) < 0)
#endif
    {   DISCARD CloseSocket((unsigned int) Socket);
        Socket = INVALID_SOCKET;
        zprintf
        (   TEXTPEN_NET,
            LLL(
                MSG_NET_NOCONNECTCLIENT,
                "Can't connect as client!\n"
        )   );
        return;
    } // implied else

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

MODULE void ActAsServer_IPv4(unsigned short Port)
{   struct hostent*     Host;
    struct in_addr      addr;
    int                 i,
                        LSocket;
    struct sockaddr_in  From;
#ifdef WIN32
    unsigned long       J;
    int                 FromLength;
#endif
#ifdef AMIGA
    char                J;
    long                FromLength;

    if (!SocketBase)
    {   return;
    }
#endif

#ifdef WIN32
    FromLength = sizeof(From);
#endif
#ifdef AMIGA
    FromLength = (long) sizeof(From);
#endif

    NetClose();

    // Get local hostname
    DISCARD gethostname((STRPTR) hostname, (int) sizeof(hostname));
#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Hostname is %s.\n", hostname);
#endif
    // Look up address
    if
    (   !(Host = (struct hostent*) gethostbyname(hostname)) // maybe use getifaddrs(hostname) instead?
     || !(Host->h_addr_list)
     || !(Host->h_addr_list[0])
    )
    {   zprintf
        (   TEXTPEN_NET,
            LLL(
                MSG_NET_NORESOLVEHOSTNAME,
                "Can't resolve hostname!"
        )   );
        return;
    }

    for (i = 0; Host->h_addr_list[i] != 0; i++)
    {   memcpy(&addr, Host->h_addr_list[i], sizeof(struct in_addr));

#if defined(__MORPHOS__)
        sprintf
        (   (char*) hostname, "%d.%d.%d.%d",
            (int)  (addr.s_addr / 16777216),
            (int) ((addr.s_addr % 16777216) / 65536),
            (int) ((addr.s_addr %    65536) /   256),
            (int)  (addr.s_addr %      256)
        );
#else
    #ifdef __amigaos4__
        sprintf(hostname, "%s", (char*) Inet_NtoA(addr.s_addr));
    #else
        sprintf(hostname, "%s", (char*) Inet_NtoA(addr));
    #endif
#endif
        zprintf
        (   TEXTPEN_NET,
            LLL(
                MSG_NET_ACTINGASSERVER,
                "Acting as server at %s on port %d...\n"
            ),
            hostname,
            (int) netport
        );
    }

    // Set fields of the address structure
    memset(&From, 0, sizeof(From));
    From.sin_addr.s_addr = htonl(INADDR_ANY);
    From.sin_family      = AF_INET;
    From.sin_port        = htons(Port);

    // Open a listening socket
    if ((LSocket = (int) socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {   zprintf
        (   TEXTPEN_NET,
            LLL(
                MSG_NET_NOCREATESERVER,
                "Can't create server socket!\n"
        )   );
        return;
    } // implied else

    // Bind listening socket
    if (bind((unsigned int) LSocket, (struct sockaddr*) &From, FromLength) < 0)
    {   DISCARD CloseSocket((unsigned int) LSocket);
        // LSocket = INVALID_SOCKET;
        zprintf
        (   TEXTPEN_NET,
            LLL(
                MSG_NET_NOBINDSOCKET,
                "Can't bind server socket!\n"
        )   );
        return;
    } // implied else

    // Make listening socket non-blocking
    J = 1;
    if (IoctlSocket((unsigned int) LSocket, (long) FIONBIO, &J) < 0)
    {   DISCARD CloseSocket((unsigned int) LSocket);
        // LSocket = INVALID_SOCKET;
        zprintf
        (   TEXTPEN_NET,
            LLL(
                MSG_NET_NOUNBLOCKSOCKET,
                "Can't unblock server socket!\n"
        )   );
        return;
    }

    // Listen for one client
    if (listen((unsigned int) LSocket, 1) < 0)
    {   DISCARD CloseSocket((unsigned int) LSocket);
        // LSocket = INVALID_SOCKET;
        zprintf
        (   TEXTPEN_NET,
            LLL(
                MSG_NET_NOLISTEN,
                "Can't listen!\n"
        )   );
        return;
    }

    connected = NET_LISTENING;
    settitle();
    zprintf
    (   TEXTPEN_NET,
        LLL(
            MSG_NET_LISTENING,
            "Listening...\n"
    )   );

#ifdef AMIGA
    lowerpri();
#endif

    lockgui();
    aborting = FALSE;
    if (listen_open())
    {   do
        {   // Process all messages
#ifdef WIN32
            clearmsgs();
            Socket = (int) accept((unsigned int) LSocket, (struct sockaddr*) &From, &FromLength);
#endif
#ifdef AMIGA
            listen_loop();
    #ifdef __amigaos4__
            Socket = (int) accept((unsigned int) LSocket, (struct sockaddr*) &From, (socklen_t*) &FromLength);
    #else
            Socket = (int) accept((unsigned int) LSocket, (struct sockaddr*) &From, (long*     ) &FromLength);
    #endif
#endif
        } while (Socket < 0 && !aborting);
    }

#ifdef AMIGA
    raisepri();
#endif

    listen_close();
    connected = NET_OFF;

    DISCARD CloseSocket((unsigned int) LSocket);
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

#if defined(__MORPHOS__)
    sprintf
    (   (char*) hostname, "%d.%d.%d.%d",
        (int)  (From.sin_addr.s_addr / 16777216),
        (int) ((From.sin_addr.s_addr % 16777216) / 65536),
        (int) ((From.sin_addr.s_addr %    65536) /   256),
        (int)  (From.sin_addr.s_addr %      256)
    );
#else
    #ifdef __amigaos4__
        sprintf(hostname, "%s", (char*) Inet_NtoA(From.sin_addr.s_addr));
    #else
        sprintf(hostname, "%s", (char*) Inet_NtoA(From.sin_addr));
    #endif
#endif
    zprintf
    (   TEXTPEN_NET,
        LLL
        (   MSG_NET_SERVERCONNECTED,
            "The client at %s has successfully connected to your server!\n"
        ),
        hostname
    );
}

EXPORT void send_usermsg(int msglen)
{   if (!msglen)
    {   return;
    }

#ifdef LOGNETWORK
    zprintf(TEXTPEN_VERBOSE, "%s: Sending  \"%s\".\n", (connected == NET_SERVER) ? "Server" : "Client", netmsg_out);
#endif
    DISCARD NetSend((const char*) netmsg_out, msglen);

    netmsg_out[0] = EOS;
}
EXPORT void recv_usermsg(int msglen)
{   if (!msglen)
    {   return;
    }

    if (NetRecv((char*) usermsg, msglen))
    {   usermsg[msglen] = EOS;
#ifdef LOGNETWORK
        zprintf(TEXTPEN_VERBOSE, "%s: Received \"%s\".\n", (connected == NET_SERVER) ? "Server" : "Client", usermsg);
#endif

        dogamename();
}   }

EXPORT void settapespeed(void)
{   switch (machine)
    {
    case  ELEKTOR:    tapespeed =                          870350  / (double) tape_hz;
    acase PIPBUG:     tapespeed = (fastpipbug ? 2000000 : 1000000) / (double) tape_hz;
    acase BINBUG:     tapespeed = (fastbinbug ? 2000000 : 1000000) / (double) tape_hz;
    acase INSTRUCTOR: tapespeed =                          895000  / (double) tape_hz;
    acase CD2650:     tapespeed = (fastcd2650 ? 1774080 : 1182720) / (double) tape_hz;
    acase SELBST:     tapespeed =                         1000000  / (double) tape_hz; // this assumes fastselbst
    acase PHUNSY:     tapespeed =                         1000000  / (double) tape_hz;
}   }

EXPORT int scan_to_scan(int code)
{
#ifdef WIN32
    if (ISQWERTY)
    {   switch (language)
        {
        case  LANGUAGE_GER: // QWERTZ
            if   (code == SCAN_Z) code = SCAN_Y;
            elif (code == SCAN_Y) code = SCAN_Z;
        acase LANGUAGE_FRA: // AZERTY
            if   (code == SCAN_Q) code = SCAN_A;
            elif (code == SCAN_W) code = SCAN_Z;
            elif (code == SCAN_A) code = SCAN_Q;
            elif (code == SCAN_Z) code = SCAN_W;
     /* acase LANGUAGE_ITA: // QZERTY
            if   (code == SCAN_Z) code = SCAN_W;
            elif (code == SCAN_W) code = SCAN_Z; */
    }   }
#endif

    return code;
}

#ifndef __SASC
    EXPORT int stcd_l(const char* in, SLONG* lvalue)
    {   return sscanf(in, "%ld", lvalue);
    }
    EXPORT int stch_l(const char* in, SLONG* lvalue)
    {   char *endptr;

        *lvalue = strtoul(in, &endptr, 16);
        return endptr - in;
    }
    EXPORT int stcl_d(char* out, long lvalue)
    {   return sprintf(out, "%ld", lvalue);
    }
    EXPORT int stcl_h(char* out, unsigned long lvalue)
    {   return sprintf(out, "%lX", lvalue);
    }
    EXPORT int stcul_d(char* out, unsigned long val)
    {   // Convert (32-bit) unsigned integer to ASCII decimal string.
        return sprintf(out, "%lu", val);
    }
    EXPORT char* stptok(const char* s, char* tok, int toklen, char* brk)
    {   char *lim,
             *b;

        if (!*s)
        {   return NULL;
        }

        lim = tok + toklen - 1;
        while (*s && tok < lim)
        {   for (b = brk; *b; b++)
            {   if (*s == *b)
                {   *tok = 0;
                    return (char*) s;
            }   }
            *tok++ = *s++;
        }
        *tok = 0;
        return (char*) s;
    }
#endif
