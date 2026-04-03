#define _WIN32_WINNT 0x0501
#ifndef ATTACH_PARENT_PROCESS
    #define ATTACH_PARENT_PROCESS ((DWORD) -1)
#endif

#include <windows.h>
#include <mmreg.h>

typedef unsigned char       UBYTE;
typedef unsigned char       TEXT;
typedef unsigned short      UWORD;
typedef unsigned char*      STRPTR;
typedef void*               APTR;
#define IMPORT              extern
#define UNUSED

#include <assert.h>

#define BLANKASCII          ' '
#define TITLEBARTEXT        "WinArcadia " DECIMALVERSION
#define LOGFILENAME         "WinArcadia.log"
#define OUTPUTLENGTH        (24 * KILOBYTE)

#define PATH_DISKS          "Disks"
#define PATH_GAMES          "Games"
#define PATH_PROJECTS       "Projects"
#define PATH_SCREENSHOTS    "Screenshots"
#define PATH_SCRIPTS        "Scripts"
#define PATH_SOUNDS         "Sounds"
#define PATH_TAPES          "Tapes"
#define PATH_BKGRNDS        "Backgrounds"

// #define LOGCHEEVOS
// whether to log cheevos calls (RA_#?()) that are not made every frame
// #define LOGALLCHEEVOS
// whether to log cheevos calls (RA_#?()) that are made every frame

// #define DISASSOCIATE
// whether you want to disassociate the filetypes that you don't want to associate

#define USEIPV6
// whether you want IPv6 (and IPv4) support or just IPv4 support

#define SCAN_ESCAPE       1
#define SCAN_A1           2
#define SCAN_A2           3
#define SCAN_A3           4
#define SCAN_A4           5
#define SCAN_A5           6
#define SCAN_A6           7
#define SCAN_A7           8
#define SCAN_A8           9
#define SCAN_A9          10
#define SCAN_A0          11
#define SCAN_AMINUS      12
#define SCAN_EQUALS      13
#define SCAN_BACKSPACE   14
#define SCAN_TAB         15
#define SCAN_Q           16
#define SCAN_W           17
#define SCAN_E           18
#define SCAN_R           19
#define SCAN_T           20
#define SCAN_Y           21
#define SCAN_U           22
#define SCAN_I           23
#define SCAN_O           24
#define SCAN_P           25
#define SCAN_OB          26
#define SCAN_CB          27
#define SCAN_RETURN      28
#define SCAN_AE          SCAN_RETURN
#define SCAN_LCTRL       29
#define SCAN_A           30
#define SCAN_S           31
#define SCAN_D           32
#define SCAN_F           33
#define SCAN_G           34
#define SCAN_H           35
#define SCAN_J           36
#define SCAN_K           37
#define SCAN_L           38
#define SCAN_SEMICOLON   39
#define SCAN_APOSTROPHE  40
#define SCAN_BACKTICK    41
#define SCAN_LSHIFT      42
#define SCAN_BACKSLASH   43
#define SCAN_Z           44
#define SCAN_X           45
#define SCAN_C           46
#define SCAN_V           47
#define SCAN_B           48
#define SCAN_N           49
#define SCAN_M           50
#define SCAN_COMMA       51
#define SCAN_FULLSTOP    52
#define SCAN_SLASH       53
#define SCAN_RSHIFT      54
#define SCAN_AS          55 // numeric *
#define SCAN_LALT        56
#define SCAN_SPACEBAR    57
#define SCAN_CAPSLOCK    58
#define SCAN_F1          59
#define SCAN_F2          60
#define SCAN_F3          61
#define SCAN_F4          62
#define SCAN_F5          63
#define SCAN_F6          64
#define SCAN_F7          65
#define SCAN_F8          66
#define SCAN_F9          67
#define SCAN_F10         68
#define SCAN_PAUSE       69
#define SCAN_SCRLK       70
#define SCAN_N7          71
#define SCAN_N8          72
#define SCAN_N9          73
#define SCAN_MI          74 // numeric -
#define SCAN_N4          75
#define SCAN_N5          76
#define SCAN_N6          77
#define SCAN_PL          78 // numeric +
#define SCAN_N1          79
#define SCAN_N2          80
#define SCAN_N3          81
#define SCAN_N0          82
#define SCAN_ND          83 // numeric .
#define SCAN_FN          85
#define SCAN_F11         87
#define SCAN_F12         88
#define SCAN_PREV      0x90
#define SCAN_NEXT      0x99
#define SCAN_MUTE      0xA0
#define SCAN_CALC      0xA1
#define SCAN_PLAY      0xA2
#define SCAN_STOP      0xA4
#define SCAN_VOLDOWN   0xAE
#define SCAN_VOLUP     0xB0
#define SCAN_WEB       0xB2
#define SCAN_FFWD      0xE9
#define SCAN_NE         284 // numeric ENTER
#define SCAN_RCTRL      285
#define SCAN_SL         309 // numeric /
#define SCAN_RALT       312
#define SCAN_NL         325 // NumLock
#define SCAN_HOME       327
#define SCAN_UP         328
#define SCAN_PGUP       329
#define SCAN_LEFT       331
#define SCAN_RIGHT      333
#define SCAN_END        335
#define SCAN_DOWN       336
#define SCAN_PGDN       337
#define SCAN_DEL        339
#define SCAN_LWIN       347
#define SCAN_RWIN       348
#define SCAN_MENU       349
#define UNASSIGNED      511 // unused scan code
#define SCANCODES       512

#define TIPSOFDAYS       25 // 20 shared + 5 WinArcadia-only
#define CODEPAGE_ENG    1252 // for Western European languages (English/Dutch/French/German/Italian/Spanish). 28605 is another possibility.
#define PRINTERDEVICE   "LPT1" // NB. no colon!
#define STRING_ROOT     "\\"
#define CHAR_ROOT       '\\'
#define STRING_PARENT   "\\"
#define CHAR_PARENT     '\\'
#define PATTERN_OPEN    "All Signetics files (2650,8SV,8SVX,AIF,AIFF,AOF,ASM,BIN,BMP,BNPF,BPNF,CMD,COR,COS,EOF,HEX,IMAG,IMG,MDCR,MOD,OBJ,PAP,RAW,SMS,SYM,TVC,TWIN,WAV,ZIP)\0" \
                        "*.2650;*.8SV;*.8SVX;*.AIF;*.AIFF;*.AOF;*.ASM;*.BIN;*.BMP;*.BNPF;*.BPNF;*.CMD;*.COR;*.COS;*.EOF;*.HEX;*.IMAG;*.IMG;*.MDCR;*.MOD;*.OBJ;*.PAP;*.RAW;*.SMS;*.SYM;*.TVC;*.TWIN;*.WAV;*.ZIP\0" \
                        "Programs (AOF,BIN,BPNF,CMD,EOF,IMAG,MOD,SMS,TVC,ZIP)\0*.AOF;*.BIN;*.BNPF;*.BPNF;*.CMD;*.EOF;*.IMAG;*.MOD;*.SMS;*.TVC;*.ZIP\0" \
                        "Recordings (COR,ZIP)\0*.COR;*.ZIP\0" \
                        "Saved States (COS,ZIP)\0*.COS;*.ZIP\0" \
                        "Absolute/Elektor Object Format (AOF,EOF,OBJ,ZIP)\0*.AOF;*.EOF;*.OBJ;*.ZIP\0" \
                        "Intel Hex Format (HEX,ZIP)\0*.HEX;*.ZIP\0" \
                        "BINBUG commands (CMD,ZIP)\0*.CMD;*.ZIP\0" \
                        "TWIN commands (MOD,ZIP)\0*.MOD;*.ZIP\0" \
                        "CD2650 commands (IMAG,ZIP)\0*.IMAG;*.ZIP\0" \
                        "Tapes (8SV,8SVX,AIF,AIFF,PAP,WAV,MDCR)\0*.8SV;*.8SVX;*.AIF;*.AIFF;*.PAP;*.WAV;*.MDCR\0" \
                        "Disks (IMG,RAW,TWIN)\0*.IMG;*.RAW;*.TWIN\0" \
                        "PROM burner files (BPNF,BNPF,SMS,ZIP)\0*.BPNF;*.BNPF;*.SMS;*.ZIP\0" \
                        "Assembler source code (ASM,2650)\0*.ASM;*.2650\0" \
                        "Symbols (SYM)\0*.SYM\0" \
                        "ZIP Archives (ZIP)\0*.ZIP\0" \
                        "Bitmaps (BMP)\0*.BMP\0" \
                        "All files (*.*)\0*.*\0"

#define TEXTPEN_RED          1
#define TEXTPEN_GREEN        2
#define TEXTPEN_YELLOW       3
#define TEXTPEN_BLUE         4
#define TEXTPEN_PURPLE       5
#define TEXTPEN_ORANGE       8

                          // 0    IBM: black (invisible)
#define TEXTPEN_TAPE         TEXTPEN_RED
#define TEXTPEN_DISK         TEXTPEN_RED
#define TEXTPEN_LABEL        TEXTPEN_RED
#define TEXTPEN_ERROR        TEXTPEN_RED
#define TEXTPEN_NET          TEXTPEN_GREEN
#define TEXTPEN_VERBOSE      TEXTPEN_GREEN
#define TEXTPEN_DEBUG        TEXTPEN_GREEN
#define TEXTPEN_VIEW         TEXTPEN_GREEN
#define TEXTPEN_TRACE        TEXTPEN_YELLOW
#define TEXTPEN_PRINTER      TEXTPEN_BLUE
#define TEXTPEN_RTTY         TEXTPEN_BLUE
#define TEXTPEN_PSEUDOCODE   TEXTPEN_BLUE
#define TEXTPEN_PIPBUG       TEXTPEN_PURPLE
#define TEXTPEN_COMMENT      TEXTPEN_PURPLE
#define TEXTPEN_CLIOUTPUT    6 // IBM: cyan
#define TEXTPEN_TRACE2       6 // IBM: cyan
#define TEXTPEN_CLIINPUT     7 // IBM: white
#define TEXTPEN_REXX         8 // IBM: orange
#define TEXTPEN_LOG          8 // IBM: orange
#define TEXTPEN_DEFAULT      9 // IBM: grey
#define TEXTPENS            10

                              // BBGGRR
#define EMUPEN_RED           0x008888FF
#define EMUPEN_BLUE          0x00FFAAAA
#define EMUPEN_GREEN         0x0088FF88
#define EMUPEN_YELLOW        0x0088FFFF
#define EMUPEN_ORANGE        0x0088CCFF
#define EMUPEN_PURPLE        0x00FF88FF
#define EMUPEN_CYAN          0x00FFFF88
#define EMUPEN_GREY          0x00888888
#define EMUPEN_WHITE         0x00FFFFFF
#define EMUPEN_BLACK         0x00000000
#define EMUPEN_DARKRED       0x000000CC
#define EMUPEN_DARKBLUE      0x00FF6666
#define EMUPEN_DARKGREEN     0x0000CC00
#define EMUPEN_DARKYELLOW    0x0066CCCC
#define EMUPEN_DARKORANGE    0x000066CC
#define EMUPEN_DARKPURPLE    0x00CC00CC
#define EMUPEN_DARKCYAN      0x00CCCC00
#define EMUPEN_PINK          0x00CCCCFF

                              // RRGGBB
#define EMURGBPEN_RED        0x00FF8888
#define EMURGBPEN_BLUE       0x00AAAAFF
#define EMURGBPEN_GREEN      0x0088FF88
#define EMURGBPEN_YELLOW     0x00FFFF88
#define EMURGBPEN_ORANGE     0x00FFCC88
#define EMURGBPEN_PURPLE     0x00FF88FF
#define EMURGBPEN_CYAN       0x0088FFFF
#define EMURGBPEN_GREY       0x00888888
#define EMURGBPEN_WHITE      0x00FFFFFF
#define EMURGBPEN_BLACK      0x00000000
#define EMURGBPEN_DARKRED    0x00CC0000
#define EMURGBPEN_DARKBLUE   0x006666FF
#define EMURGBPEN_DARKGREEN  0x0000CC00
#define EMURGBPEN_DARKORANGE 0x00CC6600
#define EMURGBPEN_DARKPURPLE 0x00CC00CC
#define EMURGBPEN_DARKCYAN   0x0000CCCC
#define EMURGBPEN_PINK       0x00FFCCCC

typedef struct bmiData
{
  BITMAPINFOHEADER	bmiHeader;
  RGBQUAD			bmiColors[256];
} bmiData;

struct LangStruct
{
// initialized constants
    const int    mainmenu,
                 popupmenu,
                 traymenu;
    const STRPTR filename;
    const TEXT   groupsep,
                 datesep,
                 timesep,
                 decpoint;
    const USHORT lang, sublang;
    const int    codepage;
    const STRPTR tooltip[18];
// uninitialized (non-constants)
          UBYTE* winbuffer;
          ULONG* offset;
          int    size;
};

#define USR_NOTIFYICON (WM_USER + 1)
#define WHEELDELTA     120

// always define one (only!) of these (normally GUI)
// #define CLI
#define GUI

#define PENS               64

#define TB_START   22
#define TB_A       23
#define TB_B       24
#define TB_RESET   25
#define TB_1P      26
#define TB_2P      27
#define TB_COINA   28
#define TB_COINB   29
#define TB_DIPS    30
#define TB_UC      31
#define TB_LC      32
#define TB_INT     33
#define TB_MON     34
#define TB_BATSIZE 35
#define TB_CLASS   36
#define TB_GAME    37

#define LANGUAGE_ENG        0
#define LANGUAGE_HOL        1
#define LANGUAGE_FRA        2
#define LANGUAGE_GER        3
#define LANGUAGE_GRE        4
#define LANGUAGE_ITA        5
#define LANGUAGE_POL        6
#define LANGUAGE_RUS        7
#define LANGUAGE_SPA        8
#define LANGUAGES           9 // counting from 1

#define MAKE_ID(a,b,c,d)	\
	((ULONG) (d)<<24 | (ULONG) (c)<<16 | (ULONG) (b)<<8 | (ULONG) (a))
// note that MAKE_ID() is defined differently for IBM-PC because of endianness issues!

#define ID_FORM             MAKE_ID('F', 'O', 'R', 'M')
#define ID_CTLG             MAKE_ID('C', 'T', 'L', 'G')
#define ID_STRS             MAKE_ID('S', 'T', 'R', 'S')

#define BIGGADGETS          8
#define MEMGADGETS        256

// these are used on both Amiga and IBM-PC but have different values on each
#define IMAGE_FOLDER      MEMMAPS
#define IMAGE_ROOT       (MEMMAPS + 1)
#define IMAGE_PARENT     (MEMMAPS + 2)
#define IMAGE_FIRSTGAME  (MEMMAPS + 3)
#define ARCADIAGLYPHS    59
#define IMAGE_LASTGAME   (IMAGE_FIRSTGAME + ARCADIAGLYPHS - 1)
#define ICONS            (IMAGE_LASTGAME  + 1) // sidebar icons

#define PRI_HIGH            0
#define PRI_NORMAL          1
#define PRI_LOW             2

// Menus
#define MN_FILE        0
//      MN_EDIT        1
#define MN_VIEW        2
//      MN_MACROS      3
#define MN_DEBUG       4
//      MN_TOOLS       5
#define MN_PERIPHERALS 6
#define MN_SETTINGS    7
//      MN_HELP        8
#define MN_CHEEVOS     9

// View menu
#define IN_SORTBY     11

// Debug menu
// IN_DEBUG_GENERAL        0
// IN_DEBUG_FILE           1
#define IN_DEBUG_DISK      2
#define IN_DEBUG_EDIT      3
// IN_DEBUG_VIEW           4
#define IN_DEBUG_LOG       5
// IN_DEBUG_RUN            6
#define IN_DEBUG_BP        7
#define IN_DEBUG_SYMBOLS   8
#define IN_DEBUG_TOOLS     9
#define IN_DEBUG_OPTIONS  10
#define IN_DEBUG_GRAPHICS 11

// Peripherals menu
#define IN_LEFT        0
#define IN_RIGHT       1

// Options menu
#define IN_BIOS        0
#define IN_COLOURS     1
#define IN_DOS         2
#define IN_EMULATOR    3
#define IN_FILTERS     4
#define IN_GRAPHICS    5
#define IN_INPUT       6
#define IN_LANGUAGE    7
#define IN_MACHINE     8
#define IN_SOUND       9
#define IN_SPEED      10
#define IN_SPRITES    11
#define IN_TRAINERS   12
#define IN_VDU        13

/* EXPORT */ struct MemMapToStruct
{   const int mib;
    const int icon;
};

// console key images on toolbar
#define IMAGE_START   25
#define IMAGE_A       26
#define IMAGE_B       27
#define IMAGE_RESET   28
#define IMAGE_1UP     29
#define IMAGE_2UP     30
#define IMAGE_COINA   31
#define IMAGE_COINB   32
#define IMAGE_DIPS    33
#define IMAGE_UC      34
#define IMAGE_LC      35
#define IMAGE_INT     36
#define IMAGE_MON     37
#define IMAGE_BATSIZE 38
#define IMAGE_CLASS   39
#define IMAGE_GAME    40
#define IMAGE_START2  41
#define IMAGE_SERVE   42

#define CANDIES        4

#define COPYSTAR(     a  ) display[a] = stars[a]
#define COPYWIDESTAR( a  ) display[a] = stars[a]; display[(a) + 1] = stars[(a) + 1]
#define COPYDISP(     a,b) display[a] = pencolours[colourset][b]
#define COPYWIDEDISP( a,b) display[a] = display[(a) + 1] = pencolours[colourset][b]
#define COPYDISP2(    a,b) display[a] = b
#define COPYWIDEDISP2(a,b) display[a] = display[(a) + 1] = b

#define EMUNAME     "WinArcadia"

#define STRETCHING_NONE  0
#define STRETCHING_43    1
#define STRETCHING_TOFIT 2

#define MIB_ICONIFY      8
#define MIB_EXIT         9
#define MIB_ARCADIA     57
#define MIB_INTERTON    58
#define MIB_ELEKTOR     59
#define MIB_PIPBUG      60
#define MIB_BINBUG      61
#define MIB_INSTRUCTOR  62
#define MIB_TWIN        63
#define MIB_CD2650      64
#define MIB_PHUNSY      65
#define MIB_SELBST      66
#define MIB_MIKIT       67
#define MIB_ASTROWARS   68
#define MIB_GALAXIA     69
#define MIB_LASERBATTLE 70
#define MIB_LAZARIAN    71
#define MIB_M1          72
#define MIB_M2          73
#define MIB_8550        74
#define MIB_8600        75
#define MIB_TYPERIGHT   76

#define CloseSocket closesocket
#define Inet_NtoA   inet_ntoa
#define IoctlSocket ioctlsocket

#define DRAWMAGNIFIERPIXEL(x, y, z)  canvasdisplay[    CANVAS_MAGNIFIER           ][((                      y ) *  MAGNIFIERWIDTH) + (x)] = pencolours[colourset][z]
#define DRAWAXESPIXEL(x, y, z)       canvasdisplay[    CANVAS_AXES                ][((                      y ) *       AXESWIDTH) + (x)] = z
#define DRAWMEMMAPPIXEL(x, y, z)     canvasdisplay[    CANVAS_MEMMAP              ][((                      y ) *     MEMMAPWIDTH) + (x)] = z
#define DRAWINDUSTRIALPIXEL(x, y, z) if (x >= 0 && x < INDUSTRIALWIDTH && y >= 0 && y < INDUSTRIALHEIGHT) \
                                         canvasdisplay[CANVAS_INDUSTRIAL          ][((                      y ) * INDUSTRIALWIDTH) + (x)] = z
// needed because Linearisatie dots and Vector Magnetometer Earth subdiagram can protrude over edges
#define DRAWROLLPIXEL(a, x, y, z)    canvasdisplay[a ? CANVAS_ROLL2 : CANVAS_ROLL1][((                      y ) *       ROLLWIDTH) + (x)] = z
#define DRAWWAVEFORMPIXEL(x, y, z)   canvasdisplay[    CANVAS_WAVE                ][((WAVEFORMHEIGHT - 1 - (y)) *   WAVEFORMWIDTH) + (x)] = z

#define ASSOCIATIONS    24 // 0..23

#define BEZELABLE             ((machine == TWIN || (machine == PIPBUG && pipbug_vdu == VDU_VT100) || machine == ZACCARIA) && !fullscreen)
#define BEZEL_VT100             0
#define BEZEL_TWIN              1
#define BEZEL_ASTROWARS         2
#define BEZEL_ASTROWARS_ROTATED 3
#define BEZEL_GALAXIA           4
#define BEZEL_GALAXIA_ROTATED   5
#define BEZEL_LAZARIAN          6
#define BEZELS                  7

#define VT100_BEZELWIDTH      925
#define VT100_BEZELHEIGHT     580
#define TWIN_BEZELWIDTH       670
#define TWIN_BEZELHEIGHT      450
#define ASTROWARS_BEZELWIDTH  382
#define ASTROWARS_BEZELHEIGHT 325
#define GALAXIA_BEZELWIDTH    360
#define GALAXIA_BEZELHEIGHT   316
#define LAZARIAN_BEZELWIDTH   440
#define LAZARIAN_BEZELHEIGHT  385

// radio buttons for ra_enable2()
#define IDC_INTERRUPTS       0
#define IDC_INTSELECTOR      0
#define IDC_PARALLEL         0
#define IDC_LIVES            0
#define IDC_COINA            0
#define IDC_COINB            0
#define IDC_SWITCH           0
#define IDC_BATS             0
#define IDC_SPEED            0
#define IDC_PONGMACHINE      0
#define IDC_ANGLES           0
#define IDC_PLAYERS          0
#define IDC_SERVING          0
#define IDC_8550_PONGVARIANT 0
#define IDC_8600_PONGVARIANT 0
#define IDC_SCORE            0

#define FIRSTSPOOLANIMIMAGE  0
