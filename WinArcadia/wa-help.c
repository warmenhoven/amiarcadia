// INCLUDES---------------------------------------------------------------

#include "ibm.h"

#include <commctrl.h>
#include <richedit.h>

#include "resource.h"

#include <stdio.h>
#include <io.h>       // for _open_osfhandle()
#include <fcntl.h>    // for _O_TEXT

#define EXEC_TYPES_H
#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"

// DEFINES----------------------------------------------------------------

#define MPT03ONEMERSON
// whether to show MPT-03 overlays in Arcadia game info subwin whenever
// Palladium keys are not being shown (on) vs. only when MPT-03 controls
// and boxes are in use (off)

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       HDC                   ControlsRastPtr1,
                                   ControlsRastPtr2;
EXPORT const struct KeyTableStruct keytable[16];

EXPORT const struct KeyHelpStruct keyhelp[40] = {
{  0,  2,  2, IDC_LT_2         }, //  0
{  1,  2,  2, IDC_RT_2         },
{  0, 13, 12, IDC_LT_X1        },
{  0, 14, 13, IDC_LT_X2        },
{  0, 15, 14, IDC_LT_X3        },
{  1, 13, 12, IDC_RT_X1        },
{  1, 14, 13, IDC_RT_X2        },
{  1, 15, 14, IDC_RT_X3        },
{  0,  1,  1, IDC_LT_1         },
{  0, 16, 15, IDC_LT_X4        },
{  0,  3,  3, IDC_LT_3         }, // 10
{  1,  1,  1, IDC_RT_1         },
{  1, 16, 15, IDC_RT_X4        },
{  1,  3,  3, IDC_RT_3         },
{  0,  4,  4, IDC_LT_4         },
{  0,  5,  5, IDC_LT_5         },
{  0,  6,  6, IDC_LT_6         },
{  1,  4,  4, IDC_RT_4         },
{  1,  5,  5, IDC_RT_5         },
{  1,  6,  6, IDC_RT_6         },
{  0,  7,  7, IDC_LT_7         }, // 20
{  0,  8,  8, IDC_LT_8         },
{  0,  9,  9, IDC_LT_9         },
{  1,  7,  7, IDC_RT_7         },
{  1,  8,  8, IDC_RT_8         },
{  1,  9,  9, IDC_RT_9         },
{  0, 10, 10, IDC_LT_CL        },
{  0, 11,  0, IDC_LT_0         },
{  0, 12, 11, IDC_LT_EN        },
{  1, 10, 10, IDC_RT_CL        },
{  1, 11,  0, IDC_RT_0         }, // 30
{  1, 12, 11, IDC_RT_EN        },
{  0,  0,  0, IDL_GAMEINFO_1ST },
{  0,  0,  0, IDL_GAMEINFO_2ND },
{  0,  0,  0, IDL_GAMEINFO_3RD },
{  0,  0,  0, IDL_GAMEINFO_4TH }, // 35
{  0,  2,  2, IDC_LT_EXTRAFIRE1}, // 36
{  0,  2,  2, IDC_LT_EXTRAFIRE2},
{  1,  2,  2, IDC_RT_EXTRAFIRE1},
{  1,  2,  2, IDC_RT_EXTRAFIRE2}, // 39
};

/* Keys we don't hear: both Windows keys, both Alt keys, F10.
Also Power, Sleep, Wake, Fn, PrtSc.
Picture Writer (part of DG640 driver) relies on shifted numbers to be in ASCII order (ie. works better with guest keymap)
 (ie. Shift+0..Shift+7 must produce ASCII codes $20..$27). */
EXPORT struct KeyNameStruct keyname[SCANCODES] = {
//         -----NORMAL---------  ---CAPS LOCK-------  -----SHIFT--------  ---HOST--- -Type-right-
//  name     host  ea     tt       host  ea    tt       host ea    tt     ctrl   c+s normal shift rsvrd  gadget              laptop desktop tr_index
{ "(0)",   { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $00
{ "Esc",   { ESC , ESC ,  NC  }, { ESC , ESC , ESC }, { NC , NC  , NC  },   NC,   NC, ESC , NC  , TRUE , IDC_KYBD_ESC      , TRUE , TRUE , -1 }, // $01
{ "a1",    { '1' , '1' ,  '1' }, { '1' , '1' , '1' }, { '!', '!' , '!' },   NC,   NC, '1' , '!' , FALSE, IDC_KYBD_A1       , TRUE , TRUE ,  4 }, // $02
{ "a2",    { '2' , '2' ,  '2' }, { '2' , '2' , '2' }, { '@', '"' , '"' },   NC, 0x00, '2' , '@' , FALSE, IDC_KYBD_A2       , TRUE , TRUE ,  5 }, // $03 ctrl-shift code assumes host layout
{ "a3",    { '3' , '3' ,  '3' }, { '3' , '3' , '3' }, { '#', '#' , '#' },   NC,   NC, '3' , '#' , FALSE, IDC_KYBD_A3       , TRUE , TRUE ,  6 }, // $04
{ "a4",    { '4' , '4' ,  '4' }, { '4' , '4' , '4' }, { '$', '$' , '$' },   NC,   NC, '4' , '$' , FALSE, IDC_KYBD_A4       , TRUE , TRUE ,  7 }, // $05
{ "a5",    { '5' , '5' ,  '5' }, { '5' , '5' , '5' }, { '%', '%' , '%' },   NC,   NC, '5' , '%' , FALSE, IDC_KYBD_A5       , TRUE , TRUE ,  8 }, // $06
{ "a6",    { '6' , '6' ,  '6' }, { '6' , '6' , '6' }, { '^', '&' , '&' },   NC, 0x1E, '6' , '-' , FALSE, IDC_KYBD_A6       , TRUE , TRUE ,  9 }, // $07 ctrl-shift code assumes host layout
{ "a7",    { '7' , '7' ,  '7' }, { '7' , '7' , '7' }, { '&',QUOTE,QUOTE},   NC,   NC, '7' , '&' , FALSE, IDC_KYBD_A7       , TRUE , TRUE , 10 }, // $08
{ "a8",    { '8' , '8' ,  '8' }, { '8' , '8' , '8' }, { '*', '(' , '(' },   NC,   NC, '8' , '*' , FALSE, IDC_KYBD_A8       , TRUE , TRUE , 11 }, // $09
{ "a9",    { '9' , '9' ,  '9' }, { '9' , '9' , '9' }, { '(', ')' , ')' },   NC,   NC, '9' , '(' , FALSE, IDC_KYBD_A9       , TRUE , TRUE , 12 }, // $0A
{ "a0",    { '0' , '0' ,  '0' }, { '0' , '0' , '0' }, { ')', ' ' , ' ' },   NC,   NC, '0' , ')' , FALSE, IDC_KYBD_A0       , TRUE , TRUE , 13 }, // $0B
{ "a-",    { '-' , '-' ,  '-' }, { '-' , '-' , ':' }, { '_', '=' , '*' },   NC, 0x1F, ':' , '-' , FALSE, IDC_KYBD_AMINUS   , TRUE , TRUE ,  9 }, // $0C ctrl-shift code assumes host layout
{ "=",     { '=' , '^' ,  '=' }, { '=' , '^' , '-' }, { '+', '~' , '=' },   NC,   NC, '-' , '+' , FALSE, IDC_KYBD_EQUALS   , TRUE , TRUE , 14 }, // $0D
{ "BkSp",  { BS  , BS  ,  BS  }, { BS  , BS  , BS  }, { BS , BS  , BS  },   NC,   NC, BS  , BS  , FALSE, IDC_KYBD_BS       , TRUE , TRUE , 15 }, // $0E
{ "Tab",   { HT  , HT  ,  HT  }, { HT  , HT  , HT  }, { HT , HT  , HT  },   NC,   NC, HT  , HT  , TRUE , IDC_KYBD_TAB      , TRUE , TRUE , -1 }, // $0F
{ "Q",     { 'q' , 'q' ,  'q' }, { 'Q' , 'Q' , 'Q' }, { 'Q', 'Q' , 'Q' }, 0x11, 0x11, 'q' , 'Q' , FALSE, IDC_KYBD_Q        , TRUE , TRUE , 16 }, // $10
{ "W",     { 'w' , 'w' ,  'w' }, { 'W' , 'W' , 'W' }, { 'W', 'W' , 'W' }, 0x17, 0x17, 'w' , 'W' , FALSE, IDC_KYBD_W        , TRUE , TRUE , 17 }, // $11
{ "E",     { 'e' , 'e' ,  'e' }, { 'E' , 'E' , 'E' }, { 'E', 'E' , 'E' }, 0x05, 0x05, 'e' , 'E' , FALSE, IDC_KYBD_E        , TRUE , TRUE , 18 }, // $12
{ "R",     { 'r' , 'r' ,  'r' }, { 'R' , 'R' , 'R' }, { 'R', 'R' , 'R' }, 0x12, 0x12, 'r' , 'R' , FALSE, IDC_KYBD_R        , TRUE , TRUE , 19 }, // $13
{ "T",     { 't' , 't' ,  't' }, { 'T' , 'T' , 'T' }, { 'T', 'T' , 'T' }, 0x14, 0x14, 't' , 'T' , FALSE, IDC_KYBD_T        , TRUE , TRUE , 20 }, // $14
{ "Y",     { 'y' , 'y' ,  'y' }, { 'Y' , 'Y' , 'Y' }, { 'Y', 'Y' , 'Y' }, 0x19, 0x19, 'y' , 'Y' , FALSE, IDC_KYBD_Y        , TRUE , TRUE , 21 }, // $15
{ "U",     { 'u' , 'u' ,  'u' }, { 'U' , 'U' , 'U' }, { 'U', 'U' , 'U' }, 0x15, 0x15, 'u' , 'U' , FALSE, IDC_KYBD_U        , TRUE , TRUE , 22 }, // $16
{ "I",     { 'i' , 'i' ,  'i' }, { 'I' , 'I' , 'I' }, { 'I', 'I' , 'I' }, 0x09, 0x09, 'i' , 'I' , FALSE, IDC_KYBD_I        , TRUE , TRUE , 23 }, // $17
{ "O",     { 'o' , 'o' ,  'o' }, { 'O' , 'O' , 'O' }, { 'O', 'O' , 'O' }, 0x0F, 0x0F, 'o' , 'O' , FALSE, IDC_KYBD_O        , TRUE , TRUE , 24 }, // $18
{ "P",     { 'p' , 'p' ,  'p' }, { 'P' , 'P' , 'P' }, { 'P', 'P' , 'P' }, 0x10, 0x10, 'p' , 'P' , FALSE, IDC_KYBD_P        , TRUE , TRUE , 25 }, // $19
{ "[",     { '[' , '@' ,  '=' }, { '[' , '@' , LF  }, { '{', '`' , LF  }, 0x1B,   NC, LF  , '+' , FALSE, IDC_KYBD_OB       , TRUE , TRUE , 14 }, // $1A ctrl-unshift code assumes host layout
{ "]",     { ']' , '[' ,  NC  }, { ']' , '[' , CR  }, { '}', '{' , CR  }, 0x1D,   NC, CR  , NC  , FALSE, IDC_KYBD_CB       , TRUE , TRUE , -1 }, // $1B ctrl-unshift code assumes host layout
{ "aE",    { CR  , LF  ,  LF  }, { CR  , LF  , CR  }, { CR , LF  , CR  },   NC,   NC, CR  , LF  , FALSE, IDC_KYBD_AE       , TRUE , TRUE , 26 }, // $1C
{ "LCtrl", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , IDC_KYBD_LCTRL    , TRUE , TRUE , -1 }, // $1D
{ "A",     { 'a' , 'a' ,  'a' }, { 'A' , 'A' , 'A' }, { 'A', 'A' , 'A' }, 0x01, 0x01, 'a' , 'A' , FALSE, IDC_KYBD_A        , TRUE , TRUE , 27 }, // $1E
{ "S",     { 's' , 's' ,  's' }, { 'S' , 'S' , 'S' }, { 'S', 'S' , 'S' }, 0x13, 0x13, 's' , 'S' , FALSE, IDC_KYBD_S        , TRUE , TRUE , 28 }, // $1F
{ "D",     { 'd' , 'd' ,  'd' }, { 'D' , 'D' , 'D' }, { 'D', 'D' , 'D' }, 0x04, 0x04, 'd' , 'D' , FALSE, IDC_KYBD_D        , TRUE , TRUE , 29 }, // $20
{ "F",     { 'f' , 'f' ,  'f' }, { 'F' , 'F' , 'F' }, { 'F', 'F' , 'F' }, 0x06, 0x06, 'f' , 'F' , FALSE, IDC_KYBD_F        , TRUE , TRUE , 30 }, // $21
{ "G",     { 'g' , 'g' ,  'g' }, { 'G' , 'G' , 'G' }, { 'G', 'G' , 'G' }, 0x07, 0x07, 'g' , 'G' , FALSE, IDC_KYBD_G        , TRUE , TRUE , 31 }, // $22
{ "H",     { 'h' , 'h' ,  'h' }, { 'H' , 'H' , 'H' }, { 'H', 'H' , 'H' }, 0x08, 0x08, 'h' , 'H' , FALSE, IDC_KYBD_H        , TRUE , TRUE , 32 }, // $23
{ "J",     { 'j' , 'j' ,  'j' }, { 'J' , 'J' , 'J' }, { 'J', 'J' , 'J' }, 0x0A, 0x0A, 'j' , 'J' , FALSE, IDC_KYBD_J        , TRUE , TRUE , 33 }, // $24
{ "K",     { 'k' , 'k' ,  'k' }, { 'K' , 'K' , 'K' }, { 'K', 'K' , 'K' }, 0x0B, 0x0B, 'k' , 'K' , FALSE, IDC_KYBD_K        , TRUE , TRUE , 34 }, // $25
{ "L",     { 'l' , 'l' ,  'l' }, { 'L' , 'L' , 'L' }, { 'L', 'L' , 'L' }, 0x0C, 0x0C, 'l' , 'L' , FALSE, IDC_KYBD_L        , TRUE , TRUE , 35 }, // $26
{ ";",     { ';' , ';' ,  ';' }, { ';' , ';' , ';' }, { ':', '+' , '+' },   NC,   NC, ';' , ':' , FALSE, IDC_KYBD_SEMICOLON, TRUE , TRUE , 36 }, // $27
{ "'",     {QUOTE, ':' , QUOTE}, {QUOTE, ':' , BS  }, { '"', '*' , BS  },   NC,   NC, BS  , '"' , FALSE, IDC_KYBD_QUOTE    , TRUE , TRUE , 37 }, // $28
{ "`",     { '`' , DEL ,  NC  }, { '`' , DEL , '`' }, { '~', DEL , '~' },   NC, 0x1A, '`' , NC  , FALSE, IDC_KYBD_BACKTICK , TRUE , TRUE , -1 }, // $29 ctrl-shift code assumes guest layout
{ "LShift",{ NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, IDC_KYBD_LSHIFT   , TRUE , TRUE , 49 }, // $2A
{ "R\\",   { '\\', ']' ,  NC  }, { '\\', ']' , '\\'}, { '|', '}' , '|' }, 0x1C,   NC, '\\', NC  , FALSE, IDC_KYBD_BACKSLASH, TRUE , TRUE , -1 }, // $2B right (normal) backslash
{ "Z",     { 'z' , 'z' ,  'z' }, { 'Z' , 'Z' , 'Z' }, { 'Z', 'Z' , 'Z' }, 0x1A, 0x1A, 'z' , 'Z' , FALSE, IDC_KYBD_Z        , TRUE , TRUE , 38 }, // $2C
{ "X",     { 'x' , 'x' ,  'x' }, { 'X' , 'X' , 'X' }, { 'X', 'X' , 'X' }, 0x18, 0x18, 'x' , 'X' , FALSE, IDC_KYBD_X        , TRUE , TRUE , 39 }, // $2D
{ "C",     { 'c' , 'c' ,  'c' }, { 'C' , 'C' , 'C' }, { 'C', 'C' , 'C' }, 0x03, 0x03, 'c' , 'C' , FALSE, IDC_KYBD_C        , TRUE , TRUE , 40 }, // $2E
{ "V",     { 'v' , 'v' ,  'v' }, { 'V' , 'V' , 'V' }, { 'V', 'V' , 'V' }, 0x16, 0x16, 'v' , 'V' , FALSE, IDC_KYBD_V        , TRUE , TRUE , 41 }, // $2F
{ "B",     { 'b' , 'b' ,  'b' }, { 'B' , 'B' , 'B' }, { 'B', 'B' , 'B' }, 0x02, 0x02, 'b' , 'B' , FALSE, IDC_KYBD_B        , TRUE , TRUE , 42 }, // $30
{ "N",     { 'n' , 'n' ,  'n' }, { 'N' , 'N' , 'N' }, { 'N', 'N' , 'N' }, 0x0E, 0x0E, 'n' , 'N' , FALSE, IDC_KYBD_N        , TRUE , TRUE , 43 }, // $31
{ "M",     { 'm' , 'm' ,  'm' }, { 'M' , 'M' , 'M' }, { 'M', 'M' , 'M' }, 0x0D, 0x0D, 'm' , 'M' , FALSE, IDC_KYBD_M        , TRUE , TRUE , 44 }, // $32
{ ",",     { ',' , ',' ,  ',' }, { ',' , ',' , ',' }, { '<', '<' , '<' },   NC,   NC, ',' , ',' , FALSE, IDC_KYBD_COMMA    , TRUE , TRUE , 45 }, // $33
{ "a.",    { '.' , '.' ,  '.' }, { '.' , '.' , '.' }, { '>', '>' , '>' },   NC,   NC, '.' , '.' , FALSE, IDC_KYBD_AFULLSTOP, TRUE , TRUE , 46 }, // $34
{ "a/",    { '/' , '/' ,  '/' }, { '/' , '/' , '/' }, { '?', '?' , '?' },   NC, 0xFF, '/' , '?' , FALSE, IDC_KYBD_ASLASH   , TRUE , TRUE , 47 }, // $35
{ "RShift",{ NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, IDC_KYBD_RSHIFT   , TRUE , TRUE , 50 }, // $36
{ "n*",    { '*' , '*' ,  '*' }, { '*' , '*' , '*' }, { '*', '*' , '*' },   NC,   NC, '*' , '*' , FALSE, IDC_KYBD_ASTERISK , TRUE , FALSE, 11 }, // $37
{ "LAlt",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , IDC_KYBD_LALT     , TRUE , TRUE , -1 }, // $38
{ "Space", { ' ' , ' ' ,  ' ' }, { ' ' , ' ' , ' ' }, { ' ', ' ' , ' ' },   NC, 0x00, ' ' , ' ' , FALSE, IDC_KYBD_SPACEBAR , TRUE , TRUE , 48 }, // $39
{ "CL",    { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , IDC_KYBD_CAPS     , TRUE , TRUE , 51 }, // $3A
{ "F1",    { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, IDC_KYBD_F1       , TRUE , TRUE , -1 }, // $3B
{ "F2",    { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, IDC_KYBD_F2       , TRUE , TRUE , -1 }, // $3C
{ "F3",    { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, IDC_KYBD_F3       , TRUE , TRUE , -1 }, // $3D
{ "F4",    { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, IDC_KYBD_F4       , TRUE , TRUE , -1 }, // $3E
{ "F5",    { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , IDC_KYBD_F5       , TRUE , TRUE , -1 }, // $3F
{ "F6",    { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, IDC_KYBD_F6       , TRUE , TRUE , -1 }, // $40
{ "F7",    { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, IDC_KYBD_F7       , TRUE , TRUE , -1 }, // $41
{ "F8",    { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, IDC_KYBD_F8       , TRUE , TRUE , -1 }, // $42
{ "F9",    { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, IDC_KYBD_F9       , TRUE , TRUE , -1 }, // $43
{ "F10",   { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , IDC_KYBD_F10      , TRUE , TRUE , -1 }, // $44
{ "Pause", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , IDC_KYBD_PAUSE    , TRUE , TRUE , -1 }, // $45
{ "SL",    { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, IDC_KYBD_SL       , TRUE , FALSE, -1 }, // $46
{ "n7",    { '7' , '7' ,  '7' }, { '7' , '7' , '7' }, { '7', '7' , '7' },   NC,   NC, '7' , '7' , FALSE, IDC_KYBD_N7       , TRUE , FALSE, 10 }, // $47
{ "n8",    { '8' , '8' ,  '8' }, { '8' , '8' , '8' }, { '8', '8' , '8' },   NC,   NC, '8' , '8' , FALSE, IDC_KYBD_N8       , TRUE , FALSE, 11 }, // $48
{ "n9",    { '9' , '9' ,  '9' }, { '9' , '9' , '9' }, { '9', '9' , '9' },   NC,   NC, '9' , '9' , FALSE, IDC_KYBD_N9       , TRUE , FALSE, 12 }, // $49
{ "n-",    { '-' , '-' ,  '-' }, { '-' , '-' , '-' }, { '-', '-' , '-' },   NC,   NC, '-' , '-' , FALSE, IDC_KYBD_NMINUS   , TRUE , FALSE,  9 }, // $4A
{ "n4",    { '4' , '4' ,  '4' }, { '4' , '4' , '4' }, { '4', '4' , '4' },   NC,   NC, '4' , '4' , FALSE, IDC_KYBD_N4       , TRUE , FALSE,  7 }, // $4B
{ "n5",    { '5' , '5' ,  '5' }, { '5' , '5' , '5' }, { '5', '5' , '5' },   NC,   NC, '5' , '5' , FALSE, IDC_KYBD_N5       , TRUE , FALSE,  8 }, // $4C
{ "n6",    { '6' , '6' ,  '6' }, { '6' , '6' , '6' }, { '6', '6' , '6' },   NC,   NC, '6' , '6' , FALSE, IDC_KYBD_N6       , TRUE , FALSE,  9 }, // $4D
{ "n+",    { '+' , '+' ,  '+' }, { '+' , '+' , '+' }, { '+', '+' , '+' },   NC,   NC, '+' , '+' , FALSE, IDC_KYBD_PLUS     , TRUE , FALSE, 14 }, // $4E
{ "n1",    { '1' , '1' ,  '1' }, { '1' , '1' , '1' }, { '1', '1' , '1' },   NC,   NC, '1' , '1' , FALSE, IDC_KYBD_N1       , TRUE , FALSE,  4 }, // $4F
{ "n2",    { '2' , '2' ,  '2' }, { '2' , '2' , '2' }, { '2', '2' , '2' },   NC,   NC, '2' , '2' , FALSE, IDC_KYBD_N2       , TRUE , FALSE,  5 }, // $50
{ "n3",    { '3' , '3' ,  '3' }, { '3' , '3' , '3' }, { '3', '3' , '3' },   NC,   NC, '3' , '3' , FALSE, IDC_KYBD_N3       , TRUE , FALSE,  6 }, // $51
{ "n0",    { '0' , '0' ,  '0' }, { '0' , '0' , '0' }, { '0', '0' , '0' },   NC,   NC, '0' , '0' , FALSE, IDC_KYBD_N0       , TRUE , FALSE, 13 }, // $52
{ "n.",    { '.' , '.' ,  '.' }, { '.' , '.' , '.' }, { '.', '.' , '.' },   NC,   NC, '.' , '.' , FALSE, IDC_KYBD_NFULLSTOP, TRUE , FALSE, 46 }, // $53
{ "(84)",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $54
{ "Fn",    { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , IDC_KYBD_FN       , FALSE, TRUE , -1 }, // $55 (on IBM ThinkPad. Doesn't map to any scancode on eg. Acer AspireOne)
{ "L\\",   { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  }, 0x1C,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $56 left backslash
{ "F11",   { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, IDC_KYBD_F11      , TRUE , TRUE , -1 }, // $57
{ "F12",   { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, IDC_KYBD_F12      , TRUE , TRUE , -1 }, // $58
{ "(89)",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $59
{ "(90)",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $5A
{ "(91)",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $5B
{ "(92)",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $5C
{ "(93)",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $5D
{ "(94)",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $5E
{ "(95)",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $5F
{ "(96)",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $60
{ "(97)",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $61
{ "(98)",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $62
{ "(99)",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $63
{ "F13"  , { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $64
{ "F14"  , { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $65
{ "F15"  , { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $66
{ "F16"  , { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $67
{ "F17"  , { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $68
{ "F18"  , { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $69
{ "F19"  , { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $6A
{ "F20"  , { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $6B
{ "F21"  , { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $6C
{ "F22"  , { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $6D
{ "F23"  , { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $6E ("AI" key emits Ctrl+Shift+F23 apparently)
{ "F24"  , { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $6F
{ "(112)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $70
{ "(113)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $71
{ "(114)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $72
{ "(115)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $73
{ "(116)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $74
{ "(117)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $75
{ "(118)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $76
{ "(119)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $77
{ "(120)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $78
{ "(121)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $79
{ "(122)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $7A
{ "(123)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $7B
{ "(124)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $7C
{ "(125)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $7D
{ "(126)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $7E
{ "(127)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $7F
{ "(128)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $80
{ "(129)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $81
{ "(130)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $82
{ "(131)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $83
{ "(132)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $84
{ "(133)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $85
{ "(134)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $86
{ "(135)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $87
{ "(136)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $88
{ "(137)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $89
{ "(138)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $8A
{ "(139)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $8B
{ "(140)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $8C
{ "(141)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $8D
{ "(142)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $8E
{ "(143)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $8F
{ "PrvTrk",{ NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , -1                , FALSE, FALSE, -1 }, // $90 Prev Track
{ "(145)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $91
{ "(146)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $92
{ "(147)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $93
{ "(148)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $94
{ "(149)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $95
{ "(150)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $96
{ "(151)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $97
{ "(152)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $98
{ "NxtTrk",{ NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , -1                , FALSE, FALSE, -1 }, // $99 Next Track
{ "(154)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $9A
{ "(155)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $9B
{ "(156)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $9C Numeric ENTER
{ "(157)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $9D Right Ctrl
{ "(158)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $9E
{ "(159)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $9F
{ "Mute",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , -1                , FALSE, FALSE, -1 }, // $A0
{ "Calc",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , -1                , FALSE, FALSE, -1 }, // $A1 Calculator
{ "Play",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , -1                , FALSE, FALSE, -1 }, // $A2 Play/Pause
{ "(163)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $A3
{ "MedStp",{ NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , -1                , FALSE, FALSE, -1 }, // $A4 Media Stop
{ "(165)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $A5
{ "(166)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $A6
{ "(167)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $A7
{ "(168)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $A8
{ "(169)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $A9
{ "(170)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $AA
{ "(171)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $AB
{ "(172)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $AC
{ "(173)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $AD
{ "Vol-",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , -1                , FALSE, FALSE, -1 }, // $AE Volume -
{ "(175)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $AF
{ "Vol+",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , -1                , FALSE, FALSE, -1 }, // $B0 Volume +
{ "(177)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $B1
{ "Web",   { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , -1                , FALSE, FALSE, -1 }, // $B2 Web/Home
{ "(179)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $B3
{ "(180)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $B4
{ "(181)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $B5 Num /
{ "(182)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $B6
{ "(183)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $B7 SysRq
{ "(184)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $B8 Right Alt
{ "(185)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $B9
{ "(186)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $BA
{ "(187)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $BB
{ "(188)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $BC
{ "(189)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $BD
{ "(190)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $BE
{ "(191)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $BF
{ "(192)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $C0
{ "(193)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $C1
{ "(194)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $C2
{ "(195)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $C3
{ "(196)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $C4
{ "(197)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $C5 Pause
{ "(198)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $C6
{ "(199)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $C7 Home
{ "(200)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $C8 Up
{ "(201)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $C9 Page Up
{ "(202)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $CA
{ "(203)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $CB Left
{ "(204)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $CC
{ "(205)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $CD Right
{ "(206)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $CE
{ "(207)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $CF End
{ "(208)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $D0 Down
{ "(209)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $D1 Page Down
{ "(210)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $D2 Insert
{ "(211)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $D3 Delete
{ "(212)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $D4
{ "(213)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $D5
{ "(214)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $D6
{ "(215)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $D7
{ "(216)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $D8
{ "(217)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $D9
{ "(218)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $DA
{ "(219)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $DB Left Windows
{ "(220)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $DC Right Windows
{ "App",   { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $DD Application
{ "(222)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $DE Power
{ "(223)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $DF Sleep
{ "(224)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $E0
{ "(225)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $E1
{ "(226)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $E2
{ "(227)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $E3 Wake
{ "(228)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $E4
{ "Search",{ NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $E5
{ "Faves", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $E6 Favourites
{ "Rfrsh", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $E7 Refresh
{ "WebStp",{ NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $E8 Web Stop
{ "Frwds", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , -1                , FALSE, FALSE, -1 }, // $E9 Forward
{ "Back",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $EA
{ "Comp",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $EB My Computer
{ "Mail",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $EC
{ "Media", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $ED
{ "(238)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $EE
{ "(239)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // $EF
{ "(240)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(241)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(242)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(243)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(244)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(245)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(246)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(247)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(248)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(249)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(250)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(251)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(252)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(253)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(254)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(255)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(256)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }, // all Fn-keys on Anko Wired Gaming Keyboard return this code
{ "(257)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(258)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(259)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(260)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(261)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(262)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(263)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(264)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(265)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(266)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(267)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(268)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(269)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(270)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(271)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(272)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(273)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(274)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(275)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(276)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(277)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(278)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(279)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(280)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(281)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(282)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(283)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "nE",    { CR  , CR  ,  LF  }, { CR  , CR  , CR  }, { CR , CR  , CR  },   NC,   NC, CR  , LF  , FALSE, IDC_KYBD_NE       , TRUE , FALSE, 26 },
{ "RCtrl", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , IDC_KYBD_RCTRL    , TRUE , TRUE , -1 },
{ "(286)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(287)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(288)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(289)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(290)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(291)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(292)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(293)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(294)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(295)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(296)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(297)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(298)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(299)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(300)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(301)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(302)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(303)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(304)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(305)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(306)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(307)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(308)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "n/",    { '/' , '/' ,  '/' }, { '/' , '/' , '/' }, { '/', '/' , '/' },   NC,   NC, '/' , '/' , FALSE, IDC_KYBD_NSLASH   , TRUE , FALSE, 47 },
{ "(310)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "PrtSc", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , IDC_KYBD_PRTSC    , TRUE , TRUE , -1 }, // we hear this in "redefine keys" subwindow but not handlekybd()!?
{ "RAlt",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , IDC_KYBD_RALT     , TRUE , TRUE , -1 },
{ "(313)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(314)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(315)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(316)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(317)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(318)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(319)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(320)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(321)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(322)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(323)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(324)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "NL",    { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, IDC_KYBD_NL       , TRUE , FALSE, -1 },
{ "(326)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "Home",  { FS  , FS  ,  NC  }, { FS  , FS  , FS  }, { FS , FS  , FS  },   NC,   NC, FS  , NC  , FALSE, IDC_KYBD_HOME     , TRUE , FALSE, -1 },
{ "Up",    { NAK , NAK ,  NC  }, { NAK , NAK , NAK }, { NAK, NAK , NAK },   NC,   NC, NAK , NC  , FALSE, IDC_KYBD_UP       , TRUE , TRUE , -1 }, // NAK = Ctrl-U
{ "PgUp",  { EM  , EM  ,  NC  }, { EM  , EM  , EM  }, { EM , EM  , EM  },   NC,   NC, EM  , NC  , FALSE, IDC_KYBD_PGUP     , TRUE , TRUE , -1 },
{ "(330)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "Lt",    { BS  , BS  ,  BS  }, { BS  , BS  , BS  }, { BS , BS  , BS  },   NC,   NC, BS  , BS  , FALSE, IDC_KYBD_LT       , TRUE , TRUE , 15 },
{ "(332)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "Rt",    { ' ' , ' ' ,  ' ' }, { ' ' , ' ' , ' ' }, { ' ', ' ' , ' ' },   NC,   NC, ' ' , ' ' , FALSE, IDC_KYBD_RT       , TRUE , TRUE , 48 },
{ "(334)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "End",   { BRK , BRK ,  NC  }, { BRK , BRK , BRK }, { NC , NC  , NC  },   NC,   NC, BRK , NC  , FALSE, IDC_KYBD_END      , TRUE , FALSE, -1 },
{ "Dn",    { EOT , EOT ,  NC  }, { EOT , EOT , EOT }, { EOT, EOT , EOT },   NC,   NC, EOT , NC  , FALSE, IDC_KYBD_DN       , TRUE , TRUE , -1 }, // EOT = Ctrl-D
{ "PgDn",  { CAN , CAN ,  NC  }, { CAN , CAN , CAN }, { CAN, CAN , CAN },   NC,   NC, CAN , NC  , FALSE, IDC_KYBD_PGDN     , TRUE , TRUE , -1 },
{ "Ins",   { HI  , BRK ,  NC  }, { HI  , BRK , HI  }, { NC , NC  , NC  },   NC,   NC, HI  , NC  , FALSE, IDC_KYBD_INS      , TRUE , TRUE , -1 },
{ "Del",   { DEL , DEL ,  BS  }, { DEL , DEL , DEL }, { DATE,DATE, DATE},   NC,   NC, DEL , BS  , FALSE, IDC_KYBD_DEL      , TRUE , TRUE , 15 },
{ "(340)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(341)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(342)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(343)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(344)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(345)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(346)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "LWin",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , IDC_KYBD_LWIN     , TRUE , TRUE , -1 },
{ "RWin",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , TRUE , IDC_KYBD_RWIN     , TRUE , FALSE, -1 },
{ "Menu",  { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, IDC_KYBD_MENU     , TRUE , TRUE , -1 },
{ "(350)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(351)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(352)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(353)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(354)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(355)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(356)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(357)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(358)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(359)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(360)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(361)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(362)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(363)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(364)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(365)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(366)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(367)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(368)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(369)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(370)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(371)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(372)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(373)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(374)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(375)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(376)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(377)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(378)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(379)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(380)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(381)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(382)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(383)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(384)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(385)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(386)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(387)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(388)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(389)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(390)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(391)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(392)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(393)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(394)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(395)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(396)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(397)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(398)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(399)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(400)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(401)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(402)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(403)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(404)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(405)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(406)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(407)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(408)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(409)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(410)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(411)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(412)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(413)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(414)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(415)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(416)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(417)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(418)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(419)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(420)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(421)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(422)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(423)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(424)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(425)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(426)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(427)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(428)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(429)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(430)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(431)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(432)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(433)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(434)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(435)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(436)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(437)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(438)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(439)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(440)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(441)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(442)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(443)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(444)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(445)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(446)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(447)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(448)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(449)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(450)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(451)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(452)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(453)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(454)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(455)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(456)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(457)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(458)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(459)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(460)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(461)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(462)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(463)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(464)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(465)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(466)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(467)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(468)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(469)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(470)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(471)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(472)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(473)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(474)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(475)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(476)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(477)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(478)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(479)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(480)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(481)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(482)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(483)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(484)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(485)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(486)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(487)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(488)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(489)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(490)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(491)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(492)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(493)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(494)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(495)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(496)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(497)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(498)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(499)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(500)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(501)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(502)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(503)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(504)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(505)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(506)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(507)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(508)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(509)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(510)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 },
{ "(511)", { NC  , NC  ,  NC  }, { NC  , NC  , NC  }, { NC , NC  , NC  },   NC,   NC, NC  , NC  , FALSE, -1                , FALSE, FALSE, -1 }
};

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       FLAG                      lmb, mmb, rmb,
                                       softctrl, softlshift, softrshift;
IMPORT       UBYTE                     button[2][8],
                                       jx[2], jy[2],
                                       KeyMatrix[SCANCODES / 8],
                                       newkeys[KEYS],
                                       sx[2], sy[2];
IMPORT const UBYTE                     table_opcolours_2650[2][256];
IMPORT       TEXT                      autotext[GAMEINFOLINES][80 + 1],
                                       pgmtext[6 * KILOBYTE],
                                       joyname[2][MAX_PATH];
IMPORT       ULONG                     arcadia_bigctrls,
                                       arcadia_viewcontrolsas,
                                       elektor_bigctrls,
                                       interton_bigctrls,
                                       pong8550_viewcontrolsas,
                                       pong8600_viewcontrolsas,
                                       jff[2],
                                       keyframes[SCANCODES],
                                       mikit_bigctrls,
                                       showpalladiumkeys1,
                                       si50_bigctrls,
                                       swapped,
                                       viewkybdas,
                                       viewkybdas2,
                                       viewpadsas;
IMPORT const DWORD                     joyfires[8];
IMPORT const STRPTR                    overlays[OVERLAYS][33];
IMPORT       HBRUSH                    hBrush[EMUBRUSHES];
IMPORT       HFONT                     hSmallFont;
IMPORT       HINSTANCE                 InstancePtr;
IMPORT       HWND                      SubWindowPtr[SUBWINDOWS],
                                       TipsPtr[SUBWINDOWS];
IMPORT       RECT                      therect;
IMPORT       int                       candy[CANDIES],
                                       CatalogPtr,
                                       clicked,
                                       guestrmb,
                                       hostcontroller[2],
                                       key1,
                                       key2,
                                       key3,
                                       key4,
                                       language,
                                       machine,
                                       memmap,
                                       oldcontrolkey,
                                       paddleup,
                                       paddledown,
                                       paddleleft,
                                       paddleright,
                                       style,
                                       supercpu,
                                       whichgame,
                                       whichkeyrect,
                                       whichoverlay;
IMPORT const int                       num_to_num[NUMKEYS];
IMPORT const struct GuestKeyStruct     guestkeys[NUMKEYS];
IMPORT       struct HostMachineStruct  hostmachines[MACHINES];
IMPORT const struct KeyInfoStruct      keyinfo[KEYINFOS][KEYS];
IMPORT       struct MachineStruct      machines[MACHINES];
IMPORT const struct MemMapToStruct     memmap_to[MEMMAPS];
IMPORT       struct OpcodeStruct       opcodes[3][256];

// MODULE VARIABLES-------------------------------------------------------

MODULE       HICON                     controlsicon = NULL;

// MODULE FUNCTIONS-------------------------------------------------------

MODULE int buttontranslate(int player, int which);

MODULE BOOL CALLBACK ControlsDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK GameInfoDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK     KybdDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK  OpcodesDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK     PadsDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

// CODE-------------------------------------------------------------------

EXPORT void help_gameinfo(void)
{   if (showpalladiumkeys1)
    {   open_subwindow(SUBWINDOW_GAMEINFO, MAKEINTRESOURCE(IDD_GAMEINFO_LARGE   ), GameInfoDlgProc);
    } elif (machine == ARCADIA
#ifndef MPT03ONEMERSON
         && arcadia_viewcontrolsas == 1
#endif
    )
    {   open_subwindow(SUBWINDOW_GAMEINFO, MAKEINTRESOURCE(IDD_GAMEINFO_OVERLAYS), GameInfoDlgProc);
        make_tips(SUBWINDOW_GAMEINFO, 40, 0);
        update_overlaytips();
        return;
    } else
    {   open_subwindow(SUBWINDOW_GAMEINFO, MAKEINTRESOURCE(IDD_GAMEINFO_SMALL   ), GameInfoDlgProc);
    }

    if
    (   machine == ARCADIA
     || machine == INSTRUCTOR
     || machine == PONG
     || machine == SELBST
     || machine == MIKIT
     || machines[machine].pvis
    )
    {   make_tips(SUBWINDOW_GAMEINFO, 36, 0);
        update_overlaytips();
}   }

EXPORT void view_controls(void)
{   if (!hostmachines[machine].controls) // eg. CD2650
    {   return; // important!
    }

    view_controls_engine();
    open_subwindow(SUBWINDOW_CONTROLS, MAKEINTRESOURCE(hostmachines[machine].controls), ControlsDlgProc);
    oldcontrolkey = -2;

    switch (machine)
    {
    case  ARCADIA:
        arcadia_update_miniglow();
    acase PIPBUG:
        pipbug_update_miniglows();
    acase INSTRUCTOR:
        redrawscreen(); // to redraw mini-display in controls subwindow
        si50_update_miniglows();
    acase PHUNSY:
    case  SELBST:
    case  MIKIT:
    case  TYPERIGHT:
        redrawscreen(); // to redraw mini-display in controls subwindow
}   }

EXPORT void help_opcodes(void)
{   if (machine == PONG || machine == TYPERIGHT)
    {   return;
    }

#ifdef THOLIN
    open_subwindow(SUBWINDOW_OPCODES, MAKEINTRESOURCE(IDD_OPCODES_THOLIN), OpcodesDlgProc);
#else
    open_subwindow(SUBWINDOW_OPCODES, MAKEINTRESOURCE(IDD_OPCODES       ), OpcodesDlgProc);
#endif
    make_tips(SUBWINDOW_OPCODES, 256, IDC_OPCODE0);
    update_opcodes();
}

EXPORT void help_hostpads(void)
{   if (machine != ARCADIA && machine != PONG && !machines[machine].pvis)
    {   return;
    }

    open_subwindow(SUBWINDOW_HOSTPADS, MAKEINTRESOURCE(IDD_HOSTPADS), PadsDlgProc);
    make_tips(SUBWINDOW_HOSTPADS, 51, IDC_PADS_LT_1);
    updatepadnames();
}

EXPORT void help_hostkybd(void)
{   open_subwindow(SUBWINDOW_HOSTKYBD, MAKEINTRESOURCE(viewkybdas2 ? IDD_LAPTOP : IDD_DESKTOP), KybdDlgProc);
    make_tips(SUBWINDOW_HOSTKYBD, SCANCODES, 0);
    updatekeynames(SubWindowPtr[SUBWINDOW_HOSTKYBD]);
}

EXPORT void update_opcodes(void)
{   int      i;
    TOOLINFO ti;
    TEXT     datatip[512 + 1];

    update_opcodes_engine();

    if (!SubWindowPtr[SUBWINDOW_OPCODES])
    {   return; // important!
    }

    DISCARD SetWindowText(SubWindowPtr[SUBWINDOW_OPCODES], LLL(MSG_HAIL_OPCODES, "Opcodes"));

    for (i = 0; i < 256; i++)
    {   DISCARD SetDlgItemText(SubWindowPtr[SUBWINDOW_OPCODES], IDC_OPCODE0 + i, opcodes[style][i].name);

        make_opcodetip(i, datatip);
        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
        ti.hwnd     = SubWindowPtr[SUBWINDOW_OPCODES];
        ti.uId      = i;
        ti.hinst    = InstancePtr;
        ti.lpszText = datatip; // this gets copied
        SendMessage(TipsPtr[SUBWINDOW_OPCODES], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
}   }

EXPORT void update_overlaytips(void)
{   int      i;
    TOOLINFO ti;
    TEXT     datatip[256 + 1];

    if (!SubWindowPtr[SUBWINDOW_GAMEINFO])
    {   return; // important!
    }

    for (i = 0; i < 32; i++)
    {   make_overlaytip(i, datatip);
        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
        ti.hwnd     = SubWindowPtr[SUBWINDOW_GAMEINFO];
        ti.uId      = i;
        ti.hinst    = InstancePtr;
        ti.lpszText = datatip; // this gets copied
        SendMessage(TipsPtr[SUBWINDOW_GAMEINFO], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
    }

    if
    (   machine == ARCADIA
     || machine == PONG
     || machines[machine].pvis
    )
    {   make_overlayspecialtip(keytable[key1].p1overlay, keytable[key1].p2overlay, 32, datatip);
        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
        ti.hwnd     = SubWindowPtr[SUBWINDOW_GAMEINFO];
        ti.uId      = 32;
        ti.hinst    = InstancePtr;
        ti.lpszText = datatip; // this gets copied
        SendMessage(TipsPtr[SUBWINDOW_GAMEINFO], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);

        make_overlayspecialtip(keytable[key2].p1overlay, keytable[key2].p2overlay, 33, datatip);
        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
        ti.hwnd     = SubWindowPtr[SUBWINDOW_GAMEINFO];
        ti.uId      = 33;
        ti.hinst    = InstancePtr;
        ti.lpszText = datatip; // this gets copied
        SendMessage(TipsPtr[SUBWINDOW_GAMEINFO], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);

        make_overlayspecialtip(keytable[key3].p1overlay, keytable[key3].p2overlay, 34, datatip);
        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
        ti.hwnd     = SubWindowPtr[SUBWINDOW_GAMEINFO];
        ti.uId      = 34;
        ti.hinst    = InstancePtr;
        ti.lpszText = datatip; // this gets copied
        SendMessage(TipsPtr[SUBWINDOW_GAMEINFO], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);

        make_overlayspecialtip(keytable[key4].p1overlay, keytable[key4].p2overlay, 35, datatip);
        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
        ti.hwnd     = SubWindowPtr[SUBWINDOW_GAMEINFO];
        ti.uId      = 35;
        ti.hinst    = InstancePtr;
        ti.lpszText = datatip; // this gets copied
        SendMessage(TipsPtr[SUBWINDOW_GAMEINFO], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
    }

    if (!showpalladiumkeys1 && machine == ARCADIA
#ifndef MPT03ONEMERSON
         && arcadia_viewcontrolsas == 1
#endif
    )
    {   for (i = 36; i < 40; i++)
        {   make_overlaytip(i, datatip);
            ti.cbSize   = sizeof(TOOLINFO);
            ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
            ti.hwnd     = SubWindowPtr[SUBWINDOW_GAMEINFO];
            ti.uId      = i;
            ti.hinst    = InstancePtr;
            ti.lpszText = datatip; // this gets copied
            SendMessage(TipsPtr[SUBWINDOW_GAMEINFO], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
}   }   }

MODULE BOOL CALLBACK KybdDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT LONG   gid;
    TRANSIENT ULONG  colour;
    TRANSIENT POINT  thepoint;
    TRANSIENT RECT   localrect;
    TRANSIENT int    i,
                     mousex, mousey;
    PERSIST   RECT   candyrect,
                     hostkybdrect[SCANCODES];

    switch (Message)
    {
    case WM_INITDIALOG:
        DISCARD SetWindowText(hwnd, LLL(          MSG_HAIL_HOSTKYBD, "Host Keyboard"));
        setdlgtext(hwnd, IDL_VIEWKYBDAS,          MSG_VIEWAS,        "View as");
        setdlgtext(hwnd, IDC_VIEWKYBDAS_GUEST,    MSG_GUESTKEYS,     "Guest keys");
        setdlgtext(hwnd, IDC_VIEWKYBDAS_HOST,     MSG_HOSTKEYS,      "Host keys");
        setdlgtext(hwnd, IDC_VIEWKYBDAS_OVERLAYS, MSG_OVERLAYS,      "Overlays");
        setdlgtext(hwnd, IDL_VIEWKYBDAS2,         MSG_VIEWAS,        "View as");
        setdlgtext(hwnd, IDC_VIEWKYBDAS_DESKTOP,  MSG_DESKTOP,       "Desktop");
        setdlgtext(hwnd, IDC_VIEWKYBDAS_LAPTOP,   MSG_LAPTOP,        "Netbook");
        setdlgtext(hwnd, IDL_KYBDLEGEND,          MSG_LEGEND,        "Legend");
        setdlgtext(hwnd, IDL_KYBDLEGEND1,         MSG_KYBDLEGEND_1,  "Left player");
        setdlgtext(hwnd, IDL_KYBDLEGEND2,         MSG_KYBDLEGEND_2,  "Right player");
        setdlgtext(hwnd, IDL_KYBDLEGEND3,         MSG_KYBDLEGEND_3,  "Console");
        setdlgtext(hwnd, IDL_KYBDLEGEND4,         MSG_KYBDLEGEND_4,  "Reserved");
        setdlgtext(hwnd, IDL_KYBDLEGEND5,         MSG_KYBDLEGEND_5,  "Unused");
        setdlgtext(hwnd, IDL_KYBDLEGEND6,         MSG_KYBDLEGEND_6,  "Multiple");
        setdlgtext(hwnd, IDL_KYBDLEGEND7,         MSG_KYBDLEGEND_7,  "Pressed");
        setdlgtext(hwnd, IDL_HOVER3,              MSG_HOVER,         "Hover over a button for more information.");

        if (viewkybdas2 == 1 && language != LANGUAGE_GRE && language != LANGUAGE_POL && language != LANGUAGE_RUS)
        {   DISCARD SendMessage(GetDlgItem(hwnd, IDC_KYBD_PGUP), WM_SETFONT, (WPARAM) hSmallFont, MAKELPARAM(TRUE, 0));
            DISCARD SendMessage(GetDlgItem(hwnd, IDC_KYBD_PGDN), WM_SETFONT, (WPARAM) hSmallFont, MAKELPARAM(TRUE, 0));
            DISCARD SendMessage(GetDlgItem(hwnd, IDC_KYBD_UP  ), WM_SETFONT, (WPARAM) hSmallFont, MAKELPARAM(TRUE, 0));
            DISCARD SendMessage(GetDlgItem(hwnd, IDC_KYBD_DN  ), WM_SETFONT, (WPARAM) hSmallFont, MAKELPARAM(TRUE, 0));
            DISCARD SendMessage(GetDlgItem(hwnd, IDC_KYBD_LT  ), WM_SETFONT, (WPARAM) hSmallFont, MAKELPARAM(TRUE, 0));
            DISCARD SendMessage(GetDlgItem(hwnd, IDC_KYBD_RT  ), WM_SETFONT, (WPARAM) hSmallFont, MAKELPARAM(TRUE, 0));
        }

        DISCARD CheckRadioButton
        (   hwnd,
            IDC_VIEWKYBDAS_GUEST,
            IDC_VIEWKYBDAS_OVERLAYS,
            IDC_VIEWKYBDAS_GUEST + viewkybdas
        );
        DISCARD CheckRadioButton
        (   hwnd,
            IDC_VIEWKYBDAS_DESKTOP,
            IDC_VIEWKYBDAS_LAPTOP,
            IDC_VIEWKYBDAS_DESKTOP + viewkybdas2
        );

        DISCARD GetWindowRect(GetDlgItem(hwnd, IDC_CANDY2), &localrect);
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
        if (candy[2 - 1])
        {   ShowWindow(GetDlgItem(hwnd, IDC_CANDY2), SW_SHOW);
        }

        for (i = 0; i < SCANCODES; i++)
        {   if (keyname[i].gadget != -1)
            {   GetWindowRect(GetDlgItem(hwnd, keyname[i].gadget), &localrect);
                thepoint.x             = localrect.left;
                thepoint.y             = localrect.top;
                DISCARD ScreenToClient(hwnd, &thepoint);
                hostkybdrect[i].left   = thepoint.x;
                hostkybdrect[i].top    = thepoint.y;
                thepoint.x             = localrect.right;
                thepoint.y             = localrect.bottom;
                DISCARD ScreenToClient(hwnd, &thepoint);
                hostkybdrect[i].right  = thepoint.x;
                hostkybdrect[i].bottom = thepoint.y;
        }   }
        // If they move the subwindow later, it doesn't matter.

        move_subwindow(SUBWINDOW_HOSTKYBD, hwnd);

        return TRUE;
    case WM_LBUTTONDBLCLK: // no need for acase
    case WM_LBUTTONDOWN:
        SetCapture(hwnd);
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
        {   candy[2 - 1] = TRUE;
            ShowWindow(GetDlgItem(hwnd, IDC_CANDY2), SW_SHOW);
        } else
        {   for (i = 0; i < SCANCODES; i++)
            {   if
                (   keyname[i].gadget != -1
                 && mousex >= hostkybdrect[i].left
                 && mousex <= hostkybdrect[i].right
                 && mousey >= hostkybdrect[i].top
                 && mousey <= hostkybdrect[i].bottom
                )
                {   clicked = i;
                    handle_keydown(i);
                    break;
        }   }   }

        return FALSE;
    case WM_LBUTTONUP:
        ReleaseCapture();
        mousex = (int) LOWORD(lParam); // pixels -> dialog units
        mousey = (int) HIWORD(lParam); // pixels -> dialog units
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "X: %d, Y: %d.\n", mousex, mousey);
#endif
        if (clicked != -1)
        {   KeyMatrix[clicked / 8] &= ~(1 << (clicked % 8));
            keyframes[scan_to_scan(clicked)] = 0;
            clicked = -1;
            DISCARD RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        }

        return FALSE;
    case WM_CTLCOLORSTATIC: // no need for acase
        gid = GetWindowLong((HWND) lParam, GWL_ID);
        switch (gid)
        {
        case IDL_KYBD_1ST:
            SetBkColor((HDC) wParam, EMUPEN_RED);
            return (LRESULT) hBrush[EMUBRUSH_RED];
        acase IDL_KYBD_2ND:
            SetBkColor((HDC) wParam, EMUPEN_BLUE);
            return (LRESULT) hBrush[EMUBRUSH_BLUE];
        acase IDL_KYBD_3RD:
            SetBkColor((HDC) wParam, EMUPEN_GREEN);
            return (LRESULT) hBrush[EMUBRUSH_GREEN];
        acase IDL_KYBD_4TH:
            SetBkColor((HDC) wParam, EMUPEN_WHITE);
            return (LRESULT) hBrush[EMUBRUSH_WHITE];
        acase IDL_KYBD_5TH:
            SetBkColor((HDC) wParam, EMUPEN_GREY);
            return (LRESULT) hBrush[EMUBRUSH_GREY];
        acase IDL_KYBD_6TH:
            SetBkColor((HDC) wParam, EMUPEN_PURPLE);
            return (LRESULT) hBrush[EMUBRUSH_PURPLE];
        acase IDL_KYBD_7TH:
            SetBkColor((HDC) wParam, EMUPEN_ORANGE);
            return (LRESULT) hBrush[EMUBRUSH_ORANGE];
        adefault:
            colour = setkybdcolour(gid);
            for (i = 0; i < SCANCODES; i++)
            {   if (keyname[i].gadget == gid && KeyDown((UWORD) (scan_to_scan(i))))
                {   colour = EMUPEN_ORANGE;
                    break; // for speed
            }   }
            SetBkColor((HDC) wParam, colour);
            switch (colour)
            {
            case  EMUPEN_RED:    return (LRESULT) hBrush[EMUBRUSH_RED];
            acase EMUPEN_BLUE:   return (LRESULT) hBrush[EMUBRUSH_BLUE];
            acase EMUPEN_PURPLE: return (LRESULT) hBrush[EMUBRUSH_PURPLE];
            acase EMUPEN_WHITE:  return (LRESULT) hBrush[EMUBRUSH_WHITE];
            acase EMUPEN_GREEN:  return (LRESULT) hBrush[EMUBRUSH_GREEN];
            acase EMUPEN_GREY:   return (LRESULT) hBrush[EMUBRUSH_GREY];
            acase EMUPEN_ORANGE: return (LRESULT) hBrush[EMUBRUSH_ORANGE];
            adefault:            return TRUE;
        }   }
        return TRUE;
    case WM_CLOSE: // no need for acase
        clearkybd();
        DestroyWindow(SubWindowPtr[SUBWINDOW_HOSTKYBD]);
        SubWindowPtr[SUBWINDOW_HOSTKYBD] = NULL;
        updatemenu(MENUITEM_HOSTKYBD);

        return TRUE;
    case WM_DESTROY: // no need for acase
        SubWindowPtr[SUBWINDOW_HOSTKYBD] = NULL;
        return FALSE;
    case WM_COMMAND: // no need for acase
        gid = (int) LOWORD(wParam);
        if (gid >= IDC_VIEWKYBDAS_GUEST && gid <= IDC_VIEWKYBDAS_OVERLAYS)
        {   viewkybdas = gid - IDC_VIEWKYBDAS_GUEST;
            DISCARD CheckRadioButton
            (   hwnd,
                IDC_VIEWKYBDAS_GUEST,
                IDC_VIEWKYBDAS_OVERLAYS,
                IDC_VIEWKYBDAS_GUEST + viewkybdas
            );
            updatekeynames(hwnd);
        } elif (gid >= IDC_VIEWKYBDAS_DESKTOP && gid <= IDC_VIEWKYBDAS_LAPTOP)
        {   viewkybdas2 = gid - IDC_VIEWKYBDAS_DESKTOP;
            DISCARD CheckRadioButton
            (   hwnd,
                IDC_VIEWKYBDAS_DESKTOP,
                IDC_VIEWKYBDAS_LAPTOP,
                IDC_VIEWKYBDAS_DESKTOP + viewkybdas2
            );
            close_subwindow(SUBWINDOW_HOSTKYBD);
            help_hostkybd();
        } elif (gid == IDC_CANDY2)
        {   candy[2 - 1] = FALSE;
            ShowWindow(GetDlgItem(hwnd, IDC_CANDY2), SW_HIDE);
        }
        return TRUE;
    case WM_MOVE: // no need for acase
        reset_fps();
        return FALSE;
    default: // no need for adefault
        return FALSE;
}   }

MODULE BOOL CALLBACK PadsDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT LONG   gid;
    TRANSIENT ULONG  colour;
    PERSIST   HICON  localhicon;
    TRANSIENT TEXT   tempstring[256 + 1];

    switch (Message)
    {
    case WM_INITDIALOG:
        DISCARD SetWindowText( hwnd,                  LLL(MSG_HAIL_HOSTPADS  , "Host Gamepads/Mouse"));

        setfont(hwnd, IDL_LEFTPLAYER);
        strcpy(tempstring, LLL(MSG_LEFTCONTROLLER, "Left Controller"));
        if (hostcontroller[0] == CONTROLLER_1STDJOY)
        {   sprintf(&tempstring[strlen(tempstring)], " (%s)", joyname[0]);
        }
        if (hostcontroller[0] == CONTROLLER_2NDDJOY)
        {   sprintf(&tempstring[strlen(tempstring)], " (%s)", joyname[1]);
        }
        DISCARD SetDlgItemText(hwnd, IDL_LEFTPLAYER, tempstring);

        setfont(hwnd, IDL_RIGHTPLAYER);
        strcpy(tempstring, LLL(MSG_RIGHTCONTROLLER, "Right Controller"));
        if (hostcontroller[1] == CONTROLLER_1STDJOY)
        {   sprintf(&tempstring[strlen(tempstring)], " (%s)", joyname[0]);
        }
        if (hostcontroller[1] == CONTROLLER_2NDDJOY)
        {   sprintf(&tempstring[strlen(tempstring)], " (%s)", joyname[1]);
        }
        DISCARD SetDlgItemText(hwnd, IDL_RIGHTPLAYER, tempstring);

        setdlgtext(hwnd, IDL_MOUSE,               MSG_MOUSE,         "Mouse");
        setdlgtext(hwnd, IDL_VIEWPADSAS,          MSG_VIEWAS,        "View as");
        setdlgtext(hwnd, IDC_VIEWPADSAS_GUEST,    MSG_GUESTKEYS,     "Guest keys");
        setdlgtext(hwnd, IDC_VIEWPADSAS_HOST,     MSG_HOSTKEYS,      "Host keys");
        setdlgtext(hwnd, IDC_VIEWPADSAS_OVERLAYS, MSG_OVERLAYS,      "Overlays");
        setdlgtext(hwnd, IDL_PADSLEGEND,          MSG_LEGEND,        "Legend");
        setdlgtext(hwnd, IDL_PADSLEGEND1,         MSG_KYBDLEGEND_1,  "Left player");
        setdlgtext(hwnd, IDL_PADSLEGEND2,         MSG_KYBDLEGEND_2,  "Right player");
        setdlgtext(hwnd, IDL_PADSLEGEND3,         MSG_KYBDLEGEND_3,  "Console");
        setdlgtext(hwnd, IDL_PADSLEGEND4,         MSG_KYBDLEGEND_4,  "Reserved");
        setdlgtext(hwnd, IDL_PADSLEGEND5,         MSG_KYBDLEGEND_5,  "Unused");
        setdlgtext(hwnd, IDL_PADSLEGEND6,         MSG_KYBDLEGEND_7,  "Pressed");

        DISCARD CheckRadioButton
        (   hwnd,
            IDC_VIEWPADSAS_GUEST,
            IDC_VIEWPADSAS_OVERLAYS,
            IDC_VIEWPADSAS_GUEST + viewpadsas
        );

        localhicon = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon), IMAGE_ICON, 32, 32, 0);
        DISCARD SendMessage(GetDlgItem(hwnd, IDL_PADSGLYPH), STM_SETICON, (WPARAM) localhicon, (LPARAM) 0);

        move_subwindow(SUBWINDOW_HOSTPADS, hwnd);

        return TRUE;
    case WM_CTLCOLORSTATIC: // no need for acase
        gid = GetWindowLong((HWND) lParam, GWL_ID);
        switch (gid)
        {
        case IDL_PADS_1ST:
            SetBkColor((HDC) wParam, EMUPEN_RED);
            return (LRESULT) hBrush[EMUBRUSH_RED];
        acase IDL_PADS_2ND:
            SetBkColor((HDC) wParam, EMUPEN_BLUE);
            return (LRESULT) hBrush[EMUBRUSH_BLUE];
        acase IDL_PADS_3RD:
            SetBkColor((HDC) wParam, EMUPEN_GREEN);
            return (LRESULT) hBrush[EMUBRUSH_GREEN];
        acase IDL_PADS_4TH:
            SetBkColor((HDC) wParam, EMUPEN_WHITE);
            return (LRESULT) hBrush[EMUBRUSH_WHITE];
        acase IDL_PADS_5TH:
            SetBkColor((HDC) wParam, EMUPEN_GREY);
            return (LRESULT) hBrush[EMUBRUSH_GREY];
        acase IDL_PADS_6TH:
            SetBkColor((HDC) wParam, EMUPEN_ORANGE);
            return (LRESULT) hBrush[EMUBRUSH_ORANGE];
        adefault:
            if
            (   gid == IDC_PADS_LT_5
             || gid == IDC_PADS_LT_6
             || gid == IDC_PADS_LT_A
             || gid == IDC_PADS_RT_5
             || gid == IDC_PADS_RT_6
             || gid == IDC_PADS_RT_A
            )
            {   if
                (   (gid == IDC_PADS_LT_5 && (jff[0] & JOYA))
                 || (gid == IDC_PADS_LT_6 && (jff[0] & JOYB))
                 || (gid == IDC_PADS_LT_A && (jff[0] & JOYSTART))
                 || (gid == IDC_PADS_RT_5 && (jff[1] & JOYA))
                 || (gid == IDC_PADS_RT_6 && (jff[1] & JOYB))
                 || (gid == IDC_PADS_RT_A && (jff[1] & JOYSTART))
                )
                {   colour = EMUPEN_ORANGE;
                } else
                {   colour = EMUPEN_GREEN;
            }   }
            elif
            (   gid == IDC_PADS_LT_B
             || gid == IDC_PADS_LT_7
             || gid == IDC_PADS_LT_8
             || gid == IDC_PADS_RT_B
             || gid == IDC_PADS_RT_7
             || gid == IDC_PADS_RT_8
             || (!guestrmb && gid == IDC_PADS_RTMOUSE)
            )
            {   if
                (   (gid == IDC_PADS_LT_B && (jff[0] & JOYRESET   ))
                 || (gid == IDC_PADS_LT_7 && (jff[0] & JOYAUTOFIRE))
                 || (gid == IDC_PADS_LT_8 && (jff[0] & JOYPAUSE   ))
                 || (gid == IDC_PADS_RT_B && (jff[1] & JOYRESET   ))
                 || (gid == IDC_PADS_RT_7 && (jff[1] & JOYAUTOFIRE))
                 || (gid == IDC_PADS_RT_8 && (jff[1] & JOYPAUSE   ))
                )
                {   colour = EMUPEN_ORANGE;
                } else
                {   colour = EMUPEN_WHITE;
            }   }
            elif
            (   gid == IDC_PADS_LT_AUP2
             || gid == IDC_PADS_LT_ADN2
             || gid == IDC_PADS_LT_ALT2
             || gid == IDC_PADS_LT_ART2
             || gid == IDC_PADS_RT_AUP2
             || gid == IDC_PADS_RT_ADN2
             || gid == IDC_PADS_RT_ALT2
             || gid == IDC_PADS_RT_ART2
            )
            {   colour = EMUPEN_GREY;
            } elif
            (   gid == IDC_PADS_LT_DUP
             || gid == IDC_PADS_LT_DDN
             || gid == IDC_PADS_LT_DLT
             || gid == IDC_PADS_LT_DRT
             || gid == IDC_PADS_LT_AUP
             || gid == IDC_PADS_LT_ADN
             || gid == IDC_PADS_LT_ALT
             || gid == IDC_PADS_LT_ART
             || gid == IDC_PADS_LT_1
             || gid == IDC_PADS_LT_2
             || gid == IDC_PADS_LT_3
             || gid == IDC_PADS_LT_4
             || gid == IDC_PADS_LT_11
             || gid == IDC_PADS_LT_12
             || gid == IDC_PADS_LTMOUSE
             || gid == IDC_PADS_MDMOUSE
             || (guestrmb && gid == IDC_PADS_RTMOUSE)
            )
            {   if
                (   ( gid == IDC_PADS_LT_1                              && (jff[0] & joyfires[button[0][0] - 1]))
                 || ((gid == IDC_PADS_LT_11  || gid == IDC_PADS_LT_12 ) && (jff[0] & JOYFIRE1                  ))
                 || ( gid == IDC_PADS_LT_2                              && (jff[0] & joyfires[button[0][1] - 1]))
                 || ( gid == IDC_PADS_LT_3                              && (jff[0] & joyfires[button[0][2] - 1]))
                 || ( gid == IDC_PADS_LT_4                              && (jff[0] & joyfires[button[0][3] - 1]))
                 || ((gid == IDC_PADS_LT_DUP || gid == IDC_PADS_LT_AUP) &&  jy[0] <=  64     )
                 || ((gid == IDC_PADS_LT_DDN || gid == IDC_PADS_LT_ADN) &&  jy[0] >= 192     )
                 || ((gid == IDC_PADS_LT_DLT || gid == IDC_PADS_LT_ALT) &&  jx[0] <=  64     )
                 || ((gid == IDC_PADS_LT_DRT || gid == IDC_PADS_LT_ART) &&  jx[0] >= 192     )
                 || ( gid == IDC_PADS_LTMOUSE                           &&  lmb              )
                 || ( gid == IDC_PADS_MDMOUSE                           &&  mmb              )
                 || ( gid == IDC_PADS_RTMOUSE                           &&  rmb              )
                )
                {   colour = EMUPEN_ORANGE;
                } else
                {   colour = (swapped ? EMUPEN_BLUE : EMUPEN_RED);
            }   }
            elif
            (   gid == IDC_PADS_RT_DUP
             || gid == IDC_PADS_RT_DDN
             || gid == IDC_PADS_RT_DLT
             || gid == IDC_PADS_RT_DRT
             || gid == IDC_PADS_RT_AUP
             || gid == IDC_PADS_RT_ADN
             || gid == IDC_PADS_RT_ALT
             || gid == IDC_PADS_RT_ART
             || gid == IDC_PADS_RT_1
             || gid == IDC_PADS_RT_2
             || gid == IDC_PADS_RT_3
             || gid == IDC_PADS_RT_4
             || gid == IDC_PADS_RT_11
             || gid == IDC_PADS_RT_12
            )
            {   if
                (   ( gid == IDC_PADS_RT_1                              && (jff[1] & joyfires[button[1][0] - 1]))
                 || ((gid == IDC_PADS_RT_11  || gid == IDC_PADS_RT_12 ) && (jff[1] & JOYFIRE1                  ))
                 || ( gid == IDC_PADS_RT_2                              && (jff[1] & joyfires[button[1][1] - 1]))
                 || ( gid == IDC_PADS_RT_3                              && (jff[1] & joyfires[button[1][2] - 1]))
                 || ( gid == IDC_PADS_RT_4                              && (jff[1] & joyfires[button[1][3] - 1]))
                 || ((gid == IDC_PADS_RT_DUP || gid == IDC_PADS_RT_AUP) &&  jy[1] <=  64     )
                 || ((gid == IDC_PADS_RT_DDN || gid == IDC_PADS_RT_ADN) &&  jy[1] >= 192     )
                 || ((gid == IDC_PADS_RT_DLT || gid == IDC_PADS_RT_ALT) &&  jx[1] <=  64     )
                 || ((gid == IDC_PADS_RT_DRT || gid == IDC_PADS_RT_ART) &&  jx[1] >= 192     )
                )
                {   colour = EMUPEN_ORANGE;
                } else
                {   colour = (swapped ? EMUPEN_RED : EMUPEN_BLUE);
            }   }
            SetBkColor((HDC) wParam, colour);
            switch (colour)
            {
            case  EMUPEN_RED:    return (LRESULT) hBrush[EMUBRUSH_RED];
            acase EMUPEN_BLUE:   return (LRESULT) hBrush[EMUBRUSH_BLUE];
            acase EMUPEN_PURPLE: return (LRESULT) hBrush[EMUBRUSH_PURPLE];
            acase EMUPEN_WHITE:  return (LRESULT) hBrush[EMUBRUSH_WHITE];
            acase EMUPEN_GREEN:  return (LRESULT) hBrush[EMUBRUSH_GREEN];
            acase EMUPEN_GREY:   return (LRESULT) hBrush[EMUBRUSH_GREY];
            acase EMUPEN_ORANGE: return (LRESULT) hBrush[EMUBRUSH_ORANGE];
            adefault:            return TRUE;
        }   }
    case WM_CLOSE: // no need for acase
        clearkybd();
        DestroyWindow(SubWindowPtr[SUBWINDOW_HOSTPADS]);
        SubWindowPtr[SUBWINDOW_HOSTPADS] = NULL;
        updatemenu(MENUITEM_HOSTPADS);

        return TRUE;
    case WM_DESTROY: // no need for acase
        DeleteObject(localhicon);
        SubWindowPtr[SUBWINDOW_HOSTPADS] = NULL;
        return FALSE;
    case WM_COMMAND: // no need for acase
        gid = (int) LOWORD(wParam);
        if (gid >= IDC_VIEWPADSAS_GUEST && gid <= IDC_VIEWPADSAS_OVERLAYS)
        {   viewpadsas = gid - IDC_VIEWPADSAS_GUEST;
            DISCARD CheckRadioButton
            (   hwnd,
                IDC_VIEWPADSAS_GUEST,
                IDC_VIEWPADSAS_OVERLAYS,
                IDC_VIEWPADSAS_GUEST + viewpadsas
            );
            updatepadnames();
        }
        return TRUE;
    case WM_MOVE: // no need for acase
        reset_fps();
        return FALSE;
    default: // no need for adefault
        return FALSE;
}   }

MODULE BOOL CALLBACK ControlsDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT PAINTSTRUCT localps;
    TRANSIENT RECT        localrect;
    TRANSIENT POINT       thepoint;
    TRANSIENT LONG        gid;
    TRANSIENT ULONG       scancode;
    TRANSIENT int         mousex, mousey,
                          oldviewcontrolsas;
    PERSIST   RECT        candyrect;

    switch (Message)
    {
    case WM_INITDIALOG:
        DISCARD SetWindowText(hwnd, LLL(MSG_HAIL_CONTROLS, "Guest Controls"));
        switch (machine)
        {
        case ARCADIA:
            setdlgtext(hwnd, IDL_VIEWCONTROLSAS, MSG_VIEWAS, "View as");
            DISCARD CheckRadioButton
            (   hwnd,
                IDC_VIEWCONTROLSAS_EMERSON,
                IDC_VIEWCONTROLSAS_PALLADIUM,
                IDC_VIEWCONTROLSAS_EMERSON + arcadia_viewcontrolsas
            );
            setdlgtext(hwnd, IDC_LARGEIMAGE,     MSG_LARGEIMAGE, "Large image?");
            DISCARD SendMessage
            (   GetDlgItem(hwnd, IDC_LARGEIMAGE),
                BM_SETCHECK,
                arcadia_bigctrls ? BST_CHECKED : BST_UNCHECKED,
                0
            );
            switch (arcadia_viewcontrolsas)
            {
            case  0: controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(arcadia_bigctrls ?   IDB_CONTROLS_EMERSON_BIG :   IDB_CONTROLS_EMERSON_SML));
            acase 1: controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(arcadia_bigctrls ?     IDB_CONTROLS_MPT03_BIG :     IDB_CONTROLS_MPT03_SML));
            acase 2: controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(arcadia_bigctrls ? IDB_CONTROLS_PALLADIUM_BIG : IDB_CONTROLS_PALLADIUM_SML));
            }
            DISCARD SendMessage(GetDlgItem(hwnd, IDC_CONTROLS), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) controlsicon);

            if (arcadia_bigctrls)
            {   DISCARD GetWindowRect(GetDlgItem(hwnd, IDC_CANDY4), &localrect);
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
                if (candy[4 - 1])
                {   ShowWindow(GetDlgItem(hwnd, IDC_CANDY4), SW_SHOW);
            }   }
        acase INTERTON:
            setdlgtext(hwnd, IDC_LARGEIMAGE,     MSG_LARGEIMAGE, "Large image?");
            DISCARD SendMessage
            (   GetDlgItem(hwnd, IDC_LARGEIMAGE),
                BM_SETCHECK,
                interton_bigctrls ? BST_CHECKED : BST_UNCHECKED,
                0
            );
            controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(interton_bigctrls ? IDB_CONTROLS_INTERTON_BIG : IDB_CONTROLS_INTERTON_SML));
        acase ELEKTOR:
            setdlgtext(hwnd, IDC_LARGEIMAGE,     MSG_LARGEIMAGE, "Large image?");
            DISCARD SendMessage
            (   GetDlgItem(hwnd, IDC_LARGEIMAGE),
                BM_SETCHECK,
                elektor_bigctrls ? BST_CHECKED : BST_UNCHECKED,
                0
            );
            controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(elektor_bigctrls ? IDB_CONTROLS_ELEKTOR_BIG : IDB_CONTROLS_ELEKTOR_SML));
        acase PIPBUG:
            controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_PIPBUG));
        acase BINBUG:
            controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_BINBUG));
        acase INSTRUCTOR:
            setdlgtext(hwnd, IDC_LARGEIMAGE,     MSG_LARGEIMAGE, "Large image?");
            DISCARD SendMessage
            (   GetDlgItem(hwnd, IDC_LARGEIMAGE),
                BM_SETCHECK,
                si50_bigctrls ? BST_CHECKED : BST_UNCHECKED,
                0
            );
         /* if (si50_bigctrls)
            {   controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_SI50_BIG));
            } else
            {   controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_SI50_SML));
            } */
        acase PONG:
            if (memmap == MEMMAP_8550)
            {   setdlgtext(hwnd, IDL_VIEWCONTROLSAS, MSG_VIEWAS,     "View as");
                DISCARD CheckRadioButton
                (   hwnd,
                    IDC_VIEWCONTROLSAS_INTERTON3000,
                    IDC_VIEWCONTROLSAS_SHEEN,
                    pong8550_viewcontrolsas ? IDC_VIEWCONTROLSAS_SHEEN : IDC_VIEWCONTROLSAS_INTERTON3000
                );
                if (pong8550_viewcontrolsas == 0)
                {   controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_INTERTON3000));
                } else
                {   controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_SHEEN       ));
                }
                DISCARD SendMessage(GetDlgItem(hwnd, IDC_CONTROLS), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) controlsicon);
            } elif (memmap == MEMMAP_8600)
            {   setdlgtext(hwnd, IDL_VIEWCONTROLSAS, MSG_VIEWAS, "View as");
                DISCARD CheckRadioButton
                (   hwnd,
                    IDC_VIEWCONTROLSAS_TELSTAR,
                    IDC_VIEWCONTROLSAS_TELEJOGO,
                    pong8600_viewcontrolsas ? IDC_VIEWCONTROLSAS_TELEJOGO : IDC_VIEWCONTROLSAS_TELSTAR
                );
                if (pong8600_viewcontrolsas == 0)
                {   controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_TELSTAR));
                } else
                {   controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_TELEJOGO));
                }
                DISCARD SendMessage(GetDlgItem(hwnd, IDC_CONTROLS), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) controlsicon);
            }
        acase MALZAK:
            controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_MALZAK));
        acase ZACCARIA:
            if (memmap == MEMMAP_ASTROWARS)
            {   controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_ASTROWARS));
            } elif (memmap == MEMMAP_GALAXIA)
            {   controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_GALAXIA));
            } elif (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
            {   controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_LASERBATTLE));
            }
        acase MIKIT:
            setdlgtext(hwnd, IDC_LARGEIMAGE, MSG_LARGEIMAGE, "Large image?");
            DISCARD SendMessage
            (   GetDlgItem(hwnd, IDC_LARGEIMAGE),
                BM_SETCHECK,
                mikit_bigctrls ? BST_CHECKED : BST_UNCHECKED,
                0
            );
            controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_MIKIT_SML));
     /* acase TWIN:
            controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_TWIN));
        acase PHUNSY:
            controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_PHUNSY));
        acase SELBST:
            controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_SELBST));
        acase TYPERIGHT:
            controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_TYPERIGHT)); */
        }

        if
        (   machine == ARCADIA
         || machines[machine].pvis
         || machine == BINBUG
         || machine == INSTRUCTOR
         || machine == PHUNSY
         || machine == SELBST
         || machine == MIKIT
         || machine == PONG
        )
        {   setdlgtext(hwnd, IDL_HOVER7            , MSG_HOVER             , "Hover over a button for more information.");
            if (machines[machine].coinop || machine == PONG)
            {   setdlgtext(hwnd, IDL_CONTROLS_OVERLAY  , MSG_CONTROLS_OVERLAY2, "Overlay/guest key:");
            } else
            {   setdlgtext(hwnd, IDL_CONTROLS_OVERLAY  , MSG_CONTROLS_OVERLAY , "Overlay:");
                setdlgtext(hwnd, IDL_CONTROLS_GUESTKEY , MSG_CONTROLS_GUESTKEY, "Guest key:");
            }
            setdlgtext(hwnd, IDL_CONTROLS_HOSTKEY  , MSG_CONTROLS_HOSTKEY  , "Host key(s):");
            setdlgtext(hwnd, IDL_CONTROLS_HOSTPAD  , MSG_CONTROLS_HOSTPAD  , "Host gamepad button(s):");
            setdlgtext(hwnd, IDL_CONTROLS_HOSTMOUSE, MSG_CONTROLS_HOSTMOUSE, "Host mouse button(s):");
        }

        // this is so that paddle axes which are never read by the
        // game are reported to be centred.
        sx[0] =
        sy[0] =
        sx[1] =
        sy[1] = 128;

        move_subwindow(SUBWINDOW_CONTROLS, hwnd);

    return FALSE;
    acase WM_ACTIVATE:
        do_autopause(wParam, lParam);
    acase WM_COMMAND:
        gid = (int) LOWORD(wParam);
        if (gid >= IDC_VIEWCONTROLSAS_EMERSON && gid <= IDC_VIEWCONTROLSAS_PALLADIUM)
        {   // assert(machine == ARCADIA);
            oldviewcontrolsas = arcadia_viewcontrolsas;
            arcadia_viewcontrolsas = gid - IDC_VIEWCONTROLSAS_EMERSON;
            DISCARD CheckRadioButton
            (   hwnd,
                IDC_VIEWCONTROLSAS_EMERSON,
                IDC_VIEWCONTROLSAS_PALLADIUM,
                IDC_VIEWCONTROLSAS_EMERSON + arcadia_viewcontrolsas
            );
            switch (arcadia_viewcontrolsas)
            {
            case  0: controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(arcadia_bigctrls ?   IDB_CONTROLS_EMERSON_BIG :   IDB_CONTROLS_EMERSON_SML));
            acase 1: controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(arcadia_bigctrls ?     IDB_CONTROLS_MPT03_BIG :     IDB_CONTROLS_MPT03_SML));
            acase 2: controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(arcadia_bigctrls ? IDB_CONTROLS_PALLADIUM_BIG : IDB_CONTROLS_PALLADIUM_SML));
            }
            if
            (   (arcadia_viewcontrolsas == 1 && oldviewcontrolsas != 1)
             || (arcadia_viewcontrolsas != 1 && oldviewcontrolsas == 1)
            )
            {   close_subwindow(SUBWINDOW_CONTROLS);
                fix_keyrects();
                view_controls();
                if (SubWindowPtr[SUBWINDOW_GAMEINFO])
                {   close_subwindow(SUBWINDOW_GAMEINFO);
                    help_gameinfo();
                }
                return TRUE;
            } else
            {   DISCARD SendMessage(GetDlgItem(hwnd, IDC_CONTROLS), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) controlsicon);
                fix_keyrects();
                return FALSE;
        }   }
        elif (gid >= IDC_VIEWCONTROLSAS_INTERTON3000 && gid <= IDC_VIEWCONTROLSAS_SHEEN)
        {   pong8550_viewcontrolsas = gid - IDC_VIEWCONTROLSAS_INTERTON3000;
            DISCARD CheckRadioButton
            (   hwnd,
                IDC_VIEWCONTROLSAS_INTERTON3000,
                IDC_VIEWCONTROLSAS_SHEEN,
                pong8550_viewcontrolsas ? IDC_VIEWCONTROLSAS_SHEEN : IDC_VIEWCONTROLSAS_INTERTON3000
            );
            if (pong8550_viewcontrolsas == 0)
            {   controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_INTERTON3000));
            } else
            {   controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_SHEEN       ));
            }
            DISCARD SendMessage(GetDlgItem(hwnd, IDC_CONTROLS), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) controlsicon);
            // assert(machine == PONG);
            fix_keyrects();
            return FALSE; // important!
        } elif (gid >= IDC_VIEWCONTROLSAS_TELSTAR && gid <= IDC_VIEWCONTROLSAS_TELEJOGO)
        {   pong8600_viewcontrolsas = gid - IDC_VIEWCONTROLSAS_TELSTAR;
            DISCARD CheckRadioButton
            (   hwnd,
                IDC_VIEWCONTROLSAS_TELSTAR,
                IDC_VIEWCONTROLSAS_TELEJOGO,
                pong8600_viewcontrolsas ? IDC_VIEWCONTROLSAS_TELEJOGO : IDC_VIEWCONTROLSAS_TELSTAR
            );
            if (pong8600_viewcontrolsas == 0)
            {   controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_TELSTAR));
            } else
            {   controlsicon = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_CONTROLS_TELEJOGO));
            }
            DISCARD SendMessage(GetDlgItem(hwnd, IDC_CONTROLS), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) controlsicon);
            // assert(machine == PONG);
            fix_keyrects();
            return FALSE; // important!
        } elif (gid == IDC_LARGEIMAGE)
        {   switch (machine)
            {
            case  ARCADIA:     arcadia_bigctrls = (SendMessage(GetDlgItem(hwnd, IDC_LARGEIMAGE), BM_GETCHECK, 0, 0) == BST_CHECKED) ? TRUE : FALSE;
            acase INTERTON:   interton_bigctrls = (SendMessage(GetDlgItem(hwnd, IDC_LARGEIMAGE), BM_GETCHECK, 0, 0) == BST_CHECKED) ? TRUE : FALSE;
            acase ELEKTOR:     elektor_bigctrls = (SendMessage(GetDlgItem(hwnd, IDC_LARGEIMAGE), BM_GETCHECK, 0, 0) == BST_CHECKED) ? TRUE : FALSE;
            acase MIKIT:         mikit_bigctrls = (SendMessage(GetDlgItem(hwnd, IDC_LARGEIMAGE), BM_GETCHECK, 0, 0) == BST_CHECKED) ? TRUE : FALSE;
            acase INSTRUCTOR:     si50_bigctrls = (SendMessage(GetDlgItem(hwnd, IDC_LARGEIMAGE), BM_GETCHECK, 0, 0) == BST_CHECKED) ? TRUE : FALSE;
            }
            close_subwindow(SUBWINDOW_CONTROLS);
            fix_keyrects();
            view_controls();
        } elif (gid == IDC_CANDY4)
        {   candy[4 - 1] = FALSE;
            ShowWindow(GetDlgItem(hwnd, IDC_CANDY4), SW_HIDE);
        }
    return TRUE;
    case WM_LBUTTONDBLCLK: // no need for acase
    case WM_LBUTTONDOWN:
        mousex = (int) LOWORD(lParam); // pixels -> dialog units
        mousey = (int) HIWORD(lParam); // pixels -> dialog units
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "X: %d, Y: %d.\n", mousex, mousey);
#endif
        if
        (   machine == ARCADIA
         && arcadia_bigctrls
         && mousex >= candyrect.left
         && mousex <= candyrect.right
         && mousey >= candyrect.top
         && mousey <= candyrect.bottom
        )
        {   candy[4 - 1] = TRUE;
            ShowWindow(GetDlgItem(hwnd, IDC_CANDY4), SW_SHOW);
        } else
        {   SetCapture(hwnd);
            controls_mousedown(mousex, mousey);
        }
    acase WM_LBUTTONUP:
        ReleaseCapture();
        controls_mouseup();
    acase WM_CLOSE:
        softctrl = softlshift = softrshift = FALSE;
        if (controlsicon)
        {   DeleteObject(controlsicon);
            controlsicon = NULL;
        }
        DestroyWindow(hwnd);
        SubWindowPtr[SUBWINDOW_CONTROLS] = NULL;
        updatemenu(MENUITEM_CONTROLS);
    acase WM_PAINT:
        BeginPaint(hwnd, &localps);
        switch (machine)
        {
        case  ARCADIA:
            arcadia_update_miniglow();
        acase PIPBUG:
            pipbug_update_miniglows();
        acase INSTRUCTOR:
            updatescreen(); // to update mini-display in controls subwindow
            si50_update_miniglows();
        acase PHUNSY:
        case  SELBST:
        case  MIKIT:
        case  TYPERIGHT:
            updatescreen(); // to update mini-display in controls subwindow
        }
        DISCARD EndPaint(hwnd, &localps);
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

MODULE BOOL CALLBACK GameInfoDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT PAINTSTRUCT localps;
    TRANSIENT LONG        gid;
    TRANSIENT ULONG       scancode;
    TRANSIENT int         i;
    PERSIST   HICON       localhicon1,
                          localhicon2 = NULL,
                          localhicon3 = NULL,
                          localhicon4 = NULL;

PERSIST const int key_to_gid[2][16] = {
{ IDC_LT_0,  //  0
  IDC_LT_1,  //  1
  IDC_LT_2,  //  2
  IDC_LT_3,  //  3
  IDC_LT_4,  //  4
  IDC_LT_5,  //  5
  IDC_LT_6,  //  6
  IDC_LT_7,  //  7
  IDC_LT_8,  //  8
  IDC_LT_9,  //  9
  IDC_LT_CL, // 10
  IDC_LT_EN, // 11
  IDC_LT_X1, // 12
  IDC_LT_X2, // 13
  IDC_LT_X3, // 14
  IDC_LT_X4  // 15
},
{ IDC_RT_0,  //  0
  IDC_RT_1,  //  1
  IDC_RT_2,  //  2
  IDC_RT_3,  //  3
  IDC_RT_4,  //  4
  IDC_RT_5,  //  5
  IDC_RT_6,  //  6
  IDC_RT_7,  //  7
  IDC_RT_8,  //  8
  IDC_RT_9,  //  9
  IDC_RT_CL, // 10
  IDC_RT_EN, // 11
  IDC_RT_X1, // 12
  IDC_RT_X2, // 13
  IDC_RT_X3, // 14
  IDC_RT_X4  // 15
} };

    switch (Message)
    {
    case WM_INITDIALOG:
        SubWindowPtr[SUBWINDOW_GAMEINFO] = hwnd;

        DISCARD SetWindowText(hwnd, LLL(        MSG_HAIL_INFO,         "Game Information"));
        setdlgtext(hwnd, IDOK,                  MSG_OK,                "OK");
        setdlgtext(hwnd, IDC_AUTOSENSE_LEFT,    MSG_LEFT,              "Left");
        setdlgtext(hwnd, IDC_AUTOSENSE_RIGHT,   MSG_RIGHT,             "Right");
        setdlgtext(hwnd, IDL_GAMEINFOLEGEND,    MSG_LEGEND,            "Legend");
        setdlgtext(hwnd, IDL_HOVER,             MSG_HOVER,             "Hover over a button for more information.");
        setdlgtext(hwnd, IDL_AUTOLABEL1,        MSG_INFO_NAME,         "Game name:");
        setdlgtext(hwnd, IDL_AUTOLABEL2,        MSG_INFO_CREDITS,      "Credits, Year:");
        setdlgtext(hwnd, IDL_AUTOLABEL3,        MSG_INFO_REFERENCE,    "Reference:");
        setdlgtext(hwnd, IDL_AUTOLABEL4,        MSG_INFO_MACHINE,      "Machine:");
        setdlgtext(hwnd, IDL_AUTOLABEL5,        MSG_INFO_SIZE,         "Size:");
        setfont(   hwnd, IDL_AUTOLABEL6);
        DISCARD SetDlgItemText(hwnd, IDL_AUTOLABEL6, "CRC32/64:");
        setdlgtext(hwnd, IDL_AUTOLABEL7,        MSG_INFO_COMPLANG,     "Language:");
        setdlgtext(hwnd, IDL_AUTOLABEL8,        MSG_INFO_ADDRESS,      "Start address:");
        setdlgtext(hwnd, IDL_1STBUTTON,         MSG_1STBUTTON,         "1st button");
        setdlgtext(hwnd, IDL_2NDBUTTON,         MSG_2NDBUTTON,         "2nd button");
        setdlgtext(hwnd, IDL_3RDBUTTON,         MSG_3RDBUTTON,         "3rd button");
        setdlgtext(hwnd, IDL_4THBUTTON,         MSG_4THBUTTON,         "4th button");
        setdlgtext(hwnd, IDC_SHOWPALLADIUMKEYS, MSG_SHOWPALLADIUMKEYS, "Show Palladium keys?");
        for (i = 0; i < GAMEINFOLINES; i++)
        {   setfont(hwnd, IDC_AUTOSENSE1 + i);
            DISCARD SetDlgItemText(hwnd, IDC_AUTOSENSE1 + i, autotext[i]);
        }
        SetDlgItemText(hwnd, IDC_PGMINFO, pgmtext);

        if (!showpalladiumkeys1 && machine == ARCADIA
#ifndef MPT03ONEMERSON
         && arcadia_viewcontrolsas == 1
#endif
        )
        {   switch (whichgame)
            {
            case ALIENINVPOS:
            case ALIENINV1POS:
            case ALIENINV2POS:
            case ALIENINV3POS:
            case ALIENINV4POS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_ALIENINVADER    )); // this is actually the Alien Armada overlay
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_DEFAULT         ));
            acase AUTORACEPOS:
            case AUTORACEODPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_AUTORACE        ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_AUTORACE        ));
            acase A_BOWLINGPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_3DBOWLING       ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_3DBOWLING       ));
            acase A_BOXINGPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_BOXING          ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_BOXING          ));
            acase GRIDIRON1POS:
            case GRIDIRON2POS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_AMERICANFOOTBALL));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_AMERICANFOOTBALL));
            acase BASEBALLPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_BASEBALL        ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_BASEBALL        ));
            acase A_BASKETBALLPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_BASKETBALL      ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_BASKETBALL      ));
            acase A_BLACKJACKPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_BLACKJACKPOKER  ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_BLACKJACKPOKER  ));
            acase BREAKAWAYPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_BREAKAWAY       ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_DEFAULT         ));
            acase A_CAPTUREPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_CAPTURE         ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_CAPTURE         ));
            acase A_CIRCUSPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_CIRCUS          ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_CIRCUS          ));
            acase A_COMBATPOS:
            case A_COMBATODPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_COMBAT          ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_COMBAT          ));
            acase CRAZYCLIMBERPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_DEFAULT         )); // no buttons on either keypad are used...
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_CRAZYCLIMBER    )); // ...but game is played with right paddle
            acase A_GOLFPOS1:
            case A_GOLFPOS2:
            case A_GOLFODPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_GOLF            ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_GOLF            ));
            acase A_HORSERACINGPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_HORSERACING     ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_HORSERACING     ));
            acase BRAINQUIZPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_MATHLOGIC       )); // this overlay is only for...
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_MATHLOGIC       )); // ..."Mind Breaker" subgame
            acase MISSILEWARPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_MISSILEWAR      ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_DEFAULT         ));
            acase NIBBLEMENPOS:
            case SUPERGOBBLERPOS: // arguable
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_NIBBLEMEN       )); // we also have an overlay for Devilman that we could use
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_DEFAULT         )); // right keypad pauses/unpauses but there is no overlay saying that
            acase OCEANBATTLEPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_SEABATTLE       )); // this is actually the Naval Encounter overlay
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_SEABATTLE       ));
            acase ROBOTKILLERPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_ROBOTKILLER     ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_DEFAULT         )); // but apparently two identical overlays are included!?
            acase _2DSOCCERPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_SOCCER          ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_SOCCER          ));
            acase SPACEMISSIONPOS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_SPACEMISSION    ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_DEFAULT         )); // but apparently two identical overlays are included!?
            acase SPACESQUADRON1POS:
            case SPACESQUADRON2POS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_SPACESQUADRON   ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_DEFAULT         ));
            acase SUPERBUG1POS:
            case SUPERBUG2POS:
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_SUPERBUG        ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_DEFAULT         ));
            adefault:
                /* We are missing MPT-03 overlays for these 4 games:
                   Cat Track, Escape, Space Raider, Video Chess
                   and probably for other subgame(s) of Math/Logic */
                localhicon3 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_DEFAULT         ));
                localhicon4 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_OVERLAY_DEFAULT         ));
            }
            DISCARD SendMessage(GetDlgItem(hwnd, IDL_LEFTOVERLAY ), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) localhicon3); // needed even if it is NULL (so it is blank)
            DISCARD SendMessage(GetDlgItem(hwnd, IDL_RIGHTOVERLAY), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) localhicon4); // needed even if it is NULL (so it is blank)

            SetDlgItemText(hwnd, IDC_LT_EXTRAFIRE1, overlays[whichoverlay][0]);
            SetDlgItemText(hwnd, IDC_LT_EXTRAFIRE2, overlays[whichoverlay][0]);
            SetDlgItemText(hwnd, IDC_RT_EXTRAFIRE1, overlays[whichoverlay][1]);
            SetDlgItemText(hwnd, IDC_RT_EXTRAFIRE2, overlays[whichoverlay][1]);
        }

        SetDlgItemText(hwnd, IDC_LT_2,    overlays[whichoverlay][ 0]);
        SetDlgItemText(hwnd, IDC_LT_X1,   overlays[whichoverlay][ 2]);
        SetDlgItemText(hwnd, IDC_LT_X2,   overlays[whichoverlay][ 3]);
        SetDlgItemText(hwnd, IDC_LT_X3,   overlays[whichoverlay][ 4]);
        SetDlgItemText(hwnd, IDC_LT_1,    overlays[whichoverlay][ 8]);
        SetDlgItemText(hwnd, IDC_LT_X4,   overlays[whichoverlay][ 9]);
        SetDlgItemText(hwnd, IDC_LT_3,    overlays[whichoverlay][10]);
        SetDlgItemText(hwnd, IDC_LT_4,    overlays[whichoverlay][14]);
        SetDlgItemText(hwnd, IDC_LT_5,    overlays[whichoverlay][15]);
        SetDlgItemText(hwnd, IDC_LT_6,    overlays[whichoverlay][16]);
        SetDlgItemText(hwnd, IDC_LT_7,    overlays[whichoverlay][20]);
        SetDlgItemText(hwnd, IDC_LT_8,    overlays[whichoverlay][21]);
        SetDlgItemText(hwnd, IDC_LT_9,    overlays[whichoverlay][22]);
        SetDlgItemText(hwnd, IDC_LT_CL,   overlays[whichoverlay][26]);
        SetDlgItemText(hwnd, IDC_LT_0,    overlays[whichoverlay][27]);
        SetDlgItemText(hwnd, IDC_LT_EN,   overlays[whichoverlay][28]);
        SetDlgItemText(hwnd, IDC_RT_2,    overlays[whichoverlay][ 1]);
        SetDlgItemText(hwnd, IDC_RT_X1,   overlays[whichoverlay][ 5]);
        SetDlgItemText(hwnd, IDC_RT_X2,   overlays[whichoverlay][ 6]);
        SetDlgItemText(hwnd, IDC_RT_X3,   overlays[whichoverlay][ 7]);
        SetDlgItemText(hwnd, IDC_RT_1,    overlays[whichoverlay][11]);
        SetDlgItemText(hwnd, IDC_RT_X4,   overlays[whichoverlay][12]);
        SetDlgItemText(hwnd, IDC_RT_3,    overlays[whichoverlay][13]);
        SetDlgItemText(hwnd, IDC_RT_4,    overlays[whichoverlay][17]);
        SetDlgItemText(hwnd, IDC_RT_5,    overlays[whichoverlay][18]);
        SetDlgItemText(hwnd, IDC_RT_6,    overlays[whichoverlay][19]);
        SetDlgItemText(hwnd, IDC_RT_7,    overlays[whichoverlay][23]);
        SetDlgItemText(hwnd, IDC_RT_8,    overlays[whichoverlay][24]);
        SetDlgItemText(hwnd, IDC_RT_9,    overlays[whichoverlay][25]);
        SetDlgItemText(hwnd, IDC_RT_CL,   overlays[whichoverlay][29]);
        SetDlgItemText(hwnd, IDC_RT_0,    overlays[whichoverlay][30]);
        SetDlgItemText(hwnd, IDC_RT_EN,   overlays[whichoverlay][31]);

        localhicon1 = LoadImage( InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon), IMAGE_ICON, 32, 32, 0);
        DISCARD SendMessage(GetDlgItem(hwnd, IDL_GLYPH), STM_SETICON, (WPARAM) localhicon1, (LPARAM) 0);

        switch (whichgame)
        {
        // Arcadia
        case  _3DATTACKPOS:      localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_3DATTACK ));
        acase _3DSOCCERAPOS:
        case  _3DSOCCERBPOS:
        case  _3DSOCCERENHPOS:   localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_3DSOCCER ));
        acase ALIENINVPOS:
        case  ALIENINV1POS:
        case  ALIENINV2POS:
        case  ALIENINV3POS:
        case  ALIENINV4POS:      localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_ALIENINVADERS    : IDB_BOX_ALIENINVADERS));
        acase GRIDIRON1POS:
        case  GRIDIRON2POS:      localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_AMERICANFOOTBALL : IDB_BOX_AMERICANFOOTBALL));
        acase ASTROINVPOS:
        case  ASTROINVODPOS:     localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_ASTROINVADER));
        acase AUTORACEPOS:
        case  AUTORACEODPOS:     localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_AUTORACE         : IDB_BOX_AUTORACE));
        acase BASEBALLPOS:       localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_BASEBALL         : IDB_BOX_BASEBALL));
        acase A_BASKETBALLPOS:   localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_BASKETBALL       : IDB_BOX_BASKETBALL));
        acase A_BLACKJACKPOS:    localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_BLACKJACKPOKER   : IDB_BOX_BLACKJACKPOKER));
        acase A_BOWLINGPOS:      localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_3DBOWLING        : IDB_BOX_3DBOWLING));
        acase A_BOXINGPOS:       localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_BOXING           : IDB_BOX_BOXING));
        acase BRAINQUIZPOS:      localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_BRAINQUIZ        : IDB_BOX_BRAINQUIZ));
        acase BREAKAWAYPOS:      localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_BREAKAWAY        : IDB_BOX_BREAKAWAY));
        acase A_CAPTUREPOS:      localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_CAPTURE          : IDB_BOX_CAPTURE));
        acase CATTRAXPOS:        localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_CATTRAX          : IDB_BOX_CATTRAX));
        acase A_CIRCUSPOS:       localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_CIRCUS           : IDB_BOX_CIRCUS));
        acase A_COMBATPOS:
        case  A_COMBATODPOS:     localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_COMBAT           : IDB_BOX_COMBAT));
        acase CRAZYCLIMBERPOS:   localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_CRAZYCLIMBER     : IDB_BOX_CRAZYCLIMBER));
        acase CRAZYGOBBLERPOS:   localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_CRAZYGOBBLER));
        acase DORAEMONPOS:       localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_DORAEMON ));
        acase DRSLUMPPOS:        localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_DRSLUMP  ));
        acase ESCAPEPOS:         localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_ESCAPE           : IDB_BOX_ESCAPE));
        acase FROGGER1POS:
        case  FROGGER2POS:
        case  FROGGER3POS:       localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_FROGGER  ));
        acase FUNKYFISHPOS:      localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_FUNKYFISH));
        acase A_GOLFPOS1:
        case  A_GOLFPOS2:
        case  A_GOLFODPOS:       localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_GOLF             : IDB_BOX_GOLF));
        acase TENNISPOS:         localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_GRANDSLAMTENNIS));
        acase GUNDAMPOS:         localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_GUNDAM   ));
        acase HOBOPOS1:
        case  HOBOPOS2:          localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_HOBO     ));
        acase A_HORSERACINGPOS:  localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_HORSERACING      : IDB_BOX_HORSERACING));
        acase JTRON121POS:
        case  JTRON10POS:
        case  JTRON11POS:        localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_JTRON    ));
        acase JUMPBUG1POS:
        case  JUMPBUG2POS:       localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_JUMPBUG  ));
        acase JUNGLERPOS:        localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_JUNGLER  ));
        acase MACROSSPOS:        localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_MACROSS  ));
        acase MISSILEWARPOS:     localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_MISSILEWAR       : IDB_BOX_MISSILEWAR));
        acase MONACOPOS:         localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_MONACOGRANDPRIX));
        acase NIBBLEMENPOS:      localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_NIBBLEMEN        : IDB_BOX_NIBBLEMEN));
        acase OCEANBATTLEPOS:    localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_OCEANBATTLE      : IDB_BOX_OCEANBATTLE));
        acase PARASHOOTERPOS:    localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_PARASHOOTER));
        acase PLEIADESPOS:       localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_PLEIADES ));
        acase R2DTANKPOS:        localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_R2DTANK  ));
        acase REDCLASHPOS:
        case  REDCLASHODPOS:     localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_REDCLASH ));
        acase ROBOTKILLERPOS:    localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_ROBOTKILLER      : IDB_BOX_ROBOTKILLER));
        acase ROUTE16POS:        localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_ROUTE16  ));
        acase _2DSOCCERPOS:
        case  _2DSOCCERODPOS:    localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_SOCCER           : IDB_BOX_SOCCER));
        acase SPACEATTACKAPOS:
        case  SPACEATTACKBPOS:
        case  SPACEATTACKCPOS:   localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_SPACEATTACK));
        acase SPACEMISSIONPOS:   localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_SPACEMISSION     : IDB_BOX_SPACEMISSION));
        acase SPACERAIDERSPOS:   localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_SPACERAIDERS     : IDB_BOX_SPACERAIDERS));
        acase SPACESQUADRON1POS:
        case  SPACESQUADRON2POS: localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_SPACESQUADRON    : IDB_BOX_SPACESQUADRON));
        acase SPACEVULTURESPOS:  localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_SPACEVULTURES));
        acase SPIDERSPOS:
        case  SPIDERSODPOS:      localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_SPIDERS  ));
        acase STARCHESSPOS:
        case  STARCHESSENHPOS:   localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_STARCHESS        : IDB_BOX_STARCHESS));
        acase SUPERBUG1POS:      localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE((arcadia_viewcontrolsas == 1) ? IDB_BOX_MPT_SUPERBUG         : IDB_BOX_SUPERBUG));
        acase SUPERGOBBLERPOS:   localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_SUPERGOBBLER));
        acase TANKSALOTPOS:      localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_TANKSALOT));
        acase TETRISPOS:         localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_TETRIS   ));
        acase THEENDPOS1:
        case  THEENDPOS2:        localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_THEEND   ));
        acase TURTLESPOS:        localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_TURTLESTURPIN));
        // Interton
        acase CARRACESPOS:       localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_1 ));
        acase I_BLACKJACKPOS:    localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_2 ));
        acase PADDLEGAMESPOS:    localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_3 ));
        acase I_COMBATBPOS:      localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_4 ));
        acase I_MATH1POS:
        case  231:               localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_5 ));
        acase I_MATH2POS:        localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_6 ));
        acase AIRSEABATTLEPOS:
        case  225:               localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_7 ));
        acase MEMORY1POS:        localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_8 ));
        acase INTELLIGENCE1POS:
        case  230:               localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_9 ));
        acase WINTERSPORTSPOS:   localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_10));
        acase HIPPODROMEPOS:
        case  229:               localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_11));
        acase HUNTINGPOS:        localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_12));
        acase CHESS1POS:         localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_13));
        acase MOTOCROSSPOS:      localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_14));
        acase _4INAROWPOS:
        case  227:               localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_15));
        acase MASTERMINDPOS:     localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_16));
        acase I_CIRCUSPOS:       localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_17));
        acase I_BOXINGPOS:
        case  226:               localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_18));
        acase SPACEWARPOS:       localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_19));
        acase MUSICALGAMESPOS:   localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_20));
        acase I_CAPTUREPOS:
        case  233:               localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_21));
        acase CHESS2POS:         localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_22));
        acase PINBALLBPOS:       localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_23));
        acase SOCCERBPOS:        localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_24));
        acase I_BOWLINGPOS:      localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_25));
        acase DRAUGHTSPOS:       localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_26));
        acase I_GOLFPOS:
        case  228:               localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_27));
        acase COCKPITPOS:        localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_28));
        acase METROPOLISPOS:     localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_29));
        acase SOLITAIREPOS:      localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_30));
        acase CASINOPOS:         localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_31));
        acase INVADERBPOS:       localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_32));
        acase SUPERINVPOS:       localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_33));
        acase BACKGAMMONPOS:     localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_36));
        acase MONSTERMANPOS:
        case  232:               localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_37));
        acase HYPERSPACEPOS:     localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_38));
        acase SUPERSPACEPOS:     localhicon2 = (HICON) LoadBitmap(InstancePtr, MAKEINTRESOURCE(IDB_BOX_40));
        // all
        adefault:                localhicon2 = NULL;
        } // Mothership and Super Bug 2 could arguably use the box of their related game
        DISCARD SendMessage(GetDlgItem(hwnd, IDL_BOX), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) localhicon2); // needed even if it is NULL (so it is blank instead of JTron box)

        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_SHOWPALLADIUMKEYS),
            BM_SETCHECK,
            showpalladiumkeys1 ? BST_CHECKED : BST_UNCHECKED,
            0
        );

        enablegads();

        move_subwindow(SUBWINDOW_GAMEINFO, hwnd);

        // DISCARD SetFocus(GetDlgItem(hwnd, IDOK));
    return FALSE; // must be FALSE so that our SetFocus()sing is respected
    case WM_CTLCOLORSTATIC: // no need for acase
        if
        (   machine == ARCADIA
         || machines[machine].pvis
         || machine == PONG
        )
        {   gid = GetWindowLong((HWND) lParam, GWL_ID);
            if (gid == IDC_LT_EXTRAFIRE1 || gid == IDC_LT_EXTRAFIRE2)
            {   gid = IDC_LT_2;
            } elif (gid == IDC_RT_EXTRAFIRE1 || gid == IDC_RT_EXTRAFIRE2)
            {   gid = IDC_RT_2;
            }

            if
            (   gid == IDL_GAMEINFO_1ST
             || (   key1 != -1
                 && (gid == key_to_gid[0][key1] || (memmap != MEMMAP_LASERBATTLE && memmap != MEMMAP_LAZARIAN && machine != MALZAK && gid == key_to_gid[1][key1]))
            )   )
            {   SetBkColor((HDC) wParam, EMUPEN_RED);
                return (LRESULT) hBrush[EMUBRUSH_RED];
            } elif
            (   gid == IDL_GAMEINFO_2ND
             || (   key2 != -1
                 && (gid == key_to_gid[0][key2] || (memmap != MEMMAP_LASERBATTLE && memmap != MEMMAP_LAZARIAN && machine != MALZAK && gid == key_to_gid[1][key2]))
            )   )
            {   SetBkColor((HDC) wParam, EMUPEN_BLUE);
                return (LRESULT) hBrush[EMUBRUSH_BLUE];
            } elif
            (   gid == IDL_GAMEINFO_3RD
             || (   key3 != -1
                 && (gid == key_to_gid[0][key3] || (memmap != MEMMAP_LASERBATTLE && memmap != MEMMAP_LAZARIAN && machine != MALZAK && gid == key_to_gid[1][key3]))
            )   )
            {   SetBkColor((HDC) wParam, EMUPEN_GREEN);
                return (LRESULT) hBrush[EMUBRUSH_GREEN];
            } elif
            (   gid == IDL_GAMEINFO_4TH
             || (   key4 != -1
                 && (gid == key_to_gid[0][key4] || (memmap != MEMMAP_LASERBATTLE && memmap != MEMMAP_LAZARIAN && machine != MALZAK && gid == key_to_gid[1][key4]))
            )   )
            {   SetBkColor((HDC) wParam, EMUPEN_YELLOW);
                return (LRESULT) hBrush[EMUBRUSH_YELLOW];
        }   }
    return FALSE;
    case WM_CLOSE: // no need for acase
        clearkybd();
        DestroyWindow(SubWindowPtr[SUBWINDOW_GAMEINFO]);
        SubWindowPtr[SUBWINDOW_GAMEINFO] = NULL;
        updatemenu(MENUITEM_GAMEINFO);
    return TRUE;
    case WM_COMMAND: // no need for acase
        switch (LOWORD(wParam))
        {
        case IDOK:
        case IDCANCEL:
            clearkybd();
            DestroyWindow(SubWindowPtr[SUBWINDOW_GAMEINFO]);
            SubWindowPtr[SUBWINDOW_GAMEINFO] = NULL;
            updatemenu(MENUITEM_GAMEINFO);
        acase IDC_SHOWPALLADIUMKEYS:
            if (SendMessage(GetDlgItem(hwnd, IDC_SHOWPALLADIUMKEYS), BM_GETCHECK, 0, 0) == BST_CHECKED)
            {   showpalladiumkeys1 = TRUE;
            } else
            {   showpalladiumkeys1 = FALSE;
            }
            close_subwindow(SUBWINDOW_GAMEINFO);
            help_gameinfo();
        acase IDC_PGMINFO:
            SendMessage(GetDlgItem(hwnd, IDC_PGMINFO), EM_SETSEL , -1, -1); // this prevents it from selecting all the text when clicking on the rest of the window
        }
    return TRUE;
    case WM_DESTROY: // no need for acase
        DeleteObject(localhicon1);
        if (localhicon2)
        {   DeleteObject(localhicon2);
            localhicon2 = NULL;
        }
        if (localhicon3)
        {   DeleteObject(localhicon3);
            localhicon3 = NULL;
        }
        if (localhicon4)
        {   DeleteObject(localhicon4);
            localhicon4 = NULL;
        }
        SubWindowPtr[SUBWINDOW_GAMEINFO] = NULL;
    return FALSE;
    case WM_PAINT: // no need for acase
        DISCARD BeginPaint(hwnd, &localps);
        DISCARD EndPaint(hwnd, &localps);
    return FALSE;
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
    // using return DefWindowProc(hwnd, Message, wParam, lParam); means that we would
    // have to manually reactivate the main window after closing this requester.
    }

    return FALSE;
}

MODULE BOOL CALLBACK OpcodesDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   LONG  gid;
    UBYTE category;

    switch (Message)
    {
    case WM_INITDIALOG:
        DISCARD SetWindowText(hwnd, LLL(    MSG_HAIL_OPCODES, "Opcodes"));

        setdlgtext(hwnd, IDL_OPCODESLEGEND, MSG_LEGEND,       "Legend");
#ifdef THOLIN
     // SetDlgItemText(hwnd, IDL_LEGEND8,                     "Unused");
        setdlgtext(hwnd, IDL_LEGEND9,       MSG_LEGEND10,     "Branch");
     // SetDlgItemText(hwnd, IDL_LEGEND10,                    "Arithmetic");
     // SetDlgItemText(hwnd, IDL_LEGEND11,                    "Subroutine");
     // SetDlgItemText(hwnd, IDL_LEGEND12,                    "Logical");
     // SetDlgItemText(hwnd, IDL_LEGEND13,                    "Load/Store");
     // SetDlgItemText(hwnd, IDL_LEGEND14,                    "Status Reg.");
     // SetDlgItemText(hwnd, IDL_LEGEND15,                    "I/O");
     // SetDlgItemText(hwnd, IDL_LEGEND16,                    "Misc.");
     // SetDlgItemText(hwnd, IDL_LEGEND17,                    "Compare");
#else
        setdlgtext(hwnd, IDL_LEGEND8,       MSG_LEGEND8,      "Arithmetic, transfer");
        setdlgtext(hwnd, IDL_LEGEND9,       MSG_LEGEND9,      "I/O, PSW, mixed");
        setdlgtext(hwnd, IDL_LEGEND10,      MSG_LEGEND10,     "Branch");
        setdlgtext(hwnd, IDL_LEGEND11,      MSG_LEGEND11,     "Special");
        setdlgtext(hwnd, IDL_LEGEND12,      MSG_LEGEND12,     "Illegal");
#endif

        move_subwindow(SUBWINDOW_OPCODES, hwnd);

        return FALSE;
    case WM_ACTIVATE: // no need for acase
        do_autopause(wParam, lParam);
    acase WM_CTLCOLORSTATIC:
        gid = GetWindowLong((HWND) lParam, GWL_ID);
        if (gid >= IDC_OPCODE0 && gid <= IDC_OPCODE255)
        {   category = table_opcolours_2650[supercpu][gid - IDC_OPCODE0];
            switch (category)
            {
            case RED:    SetBkColor((HDC) wParam, EMUPEN_RED);      return (LRESULT) hBrush[EMUBRUSH_RED];
            case BLUE:   SetBkColor((HDC) wParam, EMUPEN_BLUE);     return (LRESULT) hBrush[EMUBRUSH_BLUE];
            case GREEN:  SetBkColor((HDC) wParam, EMUPEN_GREEN);    return (LRESULT) hBrush[EMUBRUSH_GREEN];
            case YELLOW: SetBkColor((HDC) wParam, EMUPEN_YELLOW);   return (LRESULT) hBrush[EMUBRUSH_YELLOW];
            case PURPLE: SetBkColor((HDC) wParam, EMUPEN_PURPLE);   return (LRESULT) hBrush[EMUBRUSH_PURPLE];
            case ORANGE: SetBkColor((HDC) wParam, EMUPEN_ORANGE);   return (LRESULT) hBrush[EMUBRUSH_ORANGE];
            case GREY1:  SetBkColor((HDC) wParam, EMUPEN_GREY);     return (LRESULT) hBrush[EMUBRUSH_GREY];
            case CYAN:   SetBkColor((HDC) wParam, EMUPEN_CYAN);     return (LRESULT) hBrush[EMUBRUSH_CYAN];
            case WHITE:  SetBkColor((HDC) wParam, EMUPEN_WHITE);    return (LRESULT) hBrush[EMUBRUSH_WHITE];
            case DKBLUE: SetBkColor((HDC) wParam, EMUPEN_DARKBLUE); return (LRESULT) hBrush[EMUBRUSH_DARKBLUE];
        }   }
        else
        {   switch (gid)
            {
            case IDL_OP_1ST:  SetBkColor((HDC) wParam, EMUPEN_RED     ); return (LRESULT) hBrush[EMUBRUSH_RED];
            case IDL_OP_2ND:  SetBkColor((HDC) wParam, EMUPEN_BLUE    ); return (LRESULT) hBrush[EMUBRUSH_BLUE];
            case IDL_OP_3RD:  SetBkColor((HDC) wParam, EMUPEN_GREEN   ); return (LRESULT) hBrush[EMUBRUSH_GREEN];
            case IDL_OP_4TH:  SetBkColor((HDC) wParam, EMUPEN_YELLOW  ); return (LRESULT) hBrush[EMUBRUSH_YELLOW];
            case IDL_OP_5TH:  SetBkColor((HDC) wParam, EMUPEN_PURPLE  ); return (LRESULT) hBrush[EMUBRUSH_PURPLE];
            case IDL_OP_6TH:  SetBkColor((HDC) wParam, EMUPEN_ORANGE  ); return (LRESULT) hBrush[EMUBRUSH_ORANGE];
            case IDL_OP_7TH:  SetBkColor((HDC) wParam, EMUPEN_GREY    ); return (LRESULT) hBrush[EMUBRUSH_GREY];
            case IDL_OP_8TH:  SetBkColor((HDC) wParam, EMUPEN_CYAN    ); return (LRESULT) hBrush[EMUBRUSH_CYAN];
            case IDL_OP_9TH:  SetBkColor((HDC) wParam, EMUPEN_WHITE   ); return (LRESULT) hBrush[EMUBRUSH_WHITE];
            case IDL_OP_10TH: SetBkColor((HDC) wParam, EMUPEN_DARKBLUE); return (LRESULT) hBrush[EMUBRUSH_DARKBLUE];
        }   }
        return FALSE;
    case WM_CLOSE: // no need for acase
        clearkybd();
        DestroyWindow(hwnd);
        SubWindowPtr[SUBWINDOW_OPCODES] = NULL;
        updatemenu(MENUITEM_OPCODES);

        return TRUE;
    case WM_DESTROY: // no need for acase
        SubWindowPtr[SUBWINDOW_OPCODES] = NULL;
        return FALSE;
    case WM_MOVE: // no need for acase
        reset_fps();
        return FALSE;
    default: // no need for adefault
        return FALSE;
    }
    return TRUE;
}

EXPORT void setkybdtext(int scancode, HWND hwnd, ULONG gid, STRPTR contents)
{   TOOLINFO ti;

    SetDlgItemText(hwnd, gid, contents);

    ti.cbSize   = sizeof(TOOLINFO);
    ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
    ti.hwnd     = hwnd;
    ti.uId      = scancode;
    ti.hinst    = InstancePtr;
    ti.lpszText = contents; // this gets copied
    SendMessage(TipsPtr[SUBWINDOW_HOSTKYBD], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
}
EXPORT void setkybdtextandtips(int scancode, HWND hwnd, ULONG gid, STRPTR contents, STRPTR tipcontents)
{   TOOLINFO ti;

    SetDlgItemText(hwnd, gid, contents);

    ti.cbSize   = sizeof(TOOLINFO);
    ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
    ti.hwnd     = hwnd;
    ti.uId      = scancode;
    ti.hinst    = InstancePtr;
    ti.lpszText = tipcontents; // this gets copied
    SendMessage(TipsPtr[SUBWINDOW_HOSTKYBD], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
}
EXPORT void setpadtext(ULONG gid, STRPTR contents)
{   TOOLINFO ti;

    SetDlgItemText(SubWindowPtr[SUBWINDOW_HOSTPADS], gid, contents);

    ti.cbSize   = sizeof(TOOLINFO);
    ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
    ti.hwnd     = SubWindowPtr[SUBWINDOW_HOSTPADS];
    ti.uId      = gid - IDC_PADS_LT_1;
    ti.hinst    = InstancePtr;
    ti.lpszText = contents; // this gets copied
    SendMessage(TipsPtr[SUBWINDOW_HOSTPADS], TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
}

EXPORT void updatepadnames(void)
{   switch (viewpadsas)
    {
    case 0: // guest
    case 2: // overlays
        switch (button[swapped ? 1 : 0][4])
        {
        case  5: setpadtext(IDC_PADS_LT_5, machines[machine].consolekeyname[1]); // joy A
        acase 6: setpadtext(IDC_PADS_LT_5, machines[machine].consolekeyname[2]); // joy B
        acase 7: setpadtext(IDC_PADS_LT_5, LLL(MSG_AUTOFIRE, "Autofire"));
        acase 8: setpadtext(IDC_PADS_LT_5, LLL(MSG_PAUSE   , "Pause"));
        }
        switch (button[swapped ? 1 : 0][5])
        {
        case  5: setpadtext(IDC_PADS_LT_6, machines[machine].consolekeyname[1]); // joy A
        acase 6: setpadtext(IDC_PADS_LT_6, machines[machine].consolekeyname[2]); // joy B
        acase 7: setpadtext(IDC_PADS_LT_6, LLL(MSG_AUTOFIRE, "Autofire"));
        acase 8: setpadtext(IDC_PADS_LT_6, LLL(MSG_PAUSE   , "Pause"   ));
        }
        switch (button[swapped ? 1 : 0][6])
        {
        case  5: setpadtext(IDC_PADS_LT_7, machines[machine].consolekeyname[1]); // joy A
        acase 6: setpadtext(IDC_PADS_LT_7, machines[machine].consolekeyname[2]); // joy B
        acase 7: setpadtext(IDC_PADS_LT_7, LLL(MSG_AUTOFIRE, "Autofire"));
        acase 8: setpadtext(IDC_PADS_LT_7, LLL(MSG_PAUSE   , "Pause"   ));
        }
        switch (button[swapped ? 1 : 0][7])
        {
        case  5: setpadtext(IDC_PADS_LT_8, machines[machine].consolekeyname[1]); // joy A
        acase 6: setpadtext(IDC_PADS_LT_8, machines[machine].consolekeyname[2]); // joy B
        acase 7: setpadtext(IDC_PADS_LT_8, LLL(MSG_AUTOFIRE, "Autofire"));
        acase 8: setpadtext(IDC_PADS_LT_8, LLL(MSG_PAUSE   , "Pause"   ));
        }

        setpadtext(IDC_PADS_LT_A,    machines[machine].consolekeyname[0]);
        setpadtext(IDC_PADS_LT_B,    "Reset");
        setpadtext(IDC_PADS_LT_AUP2, "");
        setpadtext(IDC_PADS_LT_ADN2, "");
        setpadtext(IDC_PADS_LT_ALT2, "");
        setpadtext(IDC_PADS_LT_ART2, "");

        switch (button[swapped ? 0 : 1][4])
        {
        case  5: setpadtext(IDC_PADS_RT_5, machines[machine].consolekeyname[1]); // joy A
        acase 6: setpadtext(IDC_PADS_RT_5, machines[machine].consolekeyname[2]); // joy B
        acase 7: setpadtext(IDC_PADS_RT_5, LLL(MSG_AUTOFIRE, "Autofire"));
        acase 8: setpadtext(IDC_PADS_RT_5, LLL(MSG_PAUSE   , "Pause"   ));
        }
        switch (button[swapped ? 0 : 1][5])
        {
        case  5: setpadtext(IDC_PADS_RT_6, machines[machine].consolekeyname[1]); // joy A
        acase 6: setpadtext(IDC_PADS_RT_6, machines[machine].consolekeyname[2]); // joy B
        acase 7: setpadtext(IDC_PADS_RT_6, LLL(MSG_AUTOFIRE, "Autofire"));
        acase 8: setpadtext(IDC_PADS_RT_6, LLL(MSG_PAUSE   , "Pause"));
        }
        switch (button[swapped ? 0 : 1][6])
        {
        case  5: setpadtext(IDC_PADS_RT_7, machines[machine].consolekeyname[1]); // joy A
        acase 6: setpadtext(IDC_PADS_RT_7, machines[machine].consolekeyname[2]); // joy B
        acase 7: setpadtext(IDC_PADS_RT_7, LLL(MSG_AUTOFIRE, "Autofire"));
        acase 8: setpadtext(IDC_PADS_RT_7, LLL(MSG_PAUSE   , "Pause"   ));
        }
        switch (button[swapped ? 0 : 1][7])
        {
        case  5: setpadtext(IDC_PADS_RT_8, machines[machine].consolekeyname[1]); // joy A
        acase 6: setpadtext(IDC_PADS_RT_8, machines[machine].consolekeyname[2]); // joy B
        acase 7: setpadtext(IDC_PADS_RT_8, LLL(MSG_AUTOFIRE, "Autofire"));
        acase 8: setpadtext(IDC_PADS_RT_8, LLL(MSG_PAUSE   , "Pause"   ));
        }

        setpadtext(IDC_PADS_RT_A,    machines[machine].consolekeyname[0]);
        setpadtext(IDC_PADS_RT_B,    "Reset");
        setpadtext(IDC_PADS_RT_AUP2, "");
        setpadtext(IDC_PADS_RT_ADN2, "");
        setpadtext(IDC_PADS_RT_ALT2, "");
        setpadtext(IDC_PADS_RT_ART2, "");

        if (paddleup == -1)
        {   setpadtext(IDC_PADS_LT_DUP, LLL(MSG_KEY_UP, "Up"));
            setpadtext(IDC_PADS_LT_AUP, LLL(MSG_KEY_UP, "Up"));
            setpadtext(IDC_PADS_RT_DUP, LLL(MSG_KEY_UP, "Up"));
            setpadtext(IDC_PADS_RT_AUP, LLL(MSG_KEY_UP, "Up"));
        } elif (viewpadsas == 0) // guest
        {   setpadtext(IDC_PADS_LT_DUP, machines[machine].keynames[swapped ? 1 : 0][num_to_num[paddleup]]);
            setpadtext(IDC_PADS_LT_AUP, machines[machine].keynames[swapped ? 1 : 0][num_to_num[paddleup]]);
            setpadtext(IDC_PADS_RT_DUP, machines[machine].keynames[swapped ? 0 : 1][num_to_num[paddleup]]);
            setpadtext(IDC_PADS_RT_AUP, machines[machine].keynames[swapped ? 0 : 1][num_to_num[paddleup]]);
        } else
        {   // assert(viewpadsas == 2);
            setpadtext(IDC_PADS_LT_DUP, overlays[whichoverlay][swapped ? guestkeys[num_to_num[paddleup]].p2 : guestkeys[num_to_num[paddleup]].p1]);
            setpadtext(IDC_PADS_LT_AUP, overlays[whichoverlay][swapped ? guestkeys[num_to_num[paddleup]].p2 : guestkeys[num_to_num[paddleup]].p1]);
            setpadtext(IDC_PADS_RT_DUP, overlays[whichoverlay][swapped ? guestkeys[num_to_num[paddleup]].p1 : guestkeys[num_to_num[paddleup]].p2]);
            setpadtext(IDC_PADS_RT_AUP, overlays[whichoverlay][swapped ? guestkeys[num_to_num[paddleup]].p1 : guestkeys[num_to_num[paddleup]].p2]);
        }
        if (paddledown == -1)
        {   setpadtext(IDC_PADS_LT_DDN, LLL(MSG_KEY_DN, "Dn"));
            setpadtext(IDC_PADS_LT_ADN, LLL(MSG_KEY_DN, "Dn"));
            setpadtext(IDC_PADS_RT_DDN, LLL(MSG_KEY_DN, "Dn"));
            setpadtext(IDC_PADS_RT_ADN, LLL(MSG_KEY_DN, "Dn"));
        } elif (viewpadsas == 0) // guest
        {   setpadtext(IDC_PADS_LT_DDN, machines[machine].keynames[swapped ? 1 : 0][num_to_num[paddledown]]);
            setpadtext(IDC_PADS_LT_ADN, machines[machine].keynames[swapped ? 1 : 0][num_to_num[paddledown]]);
            setpadtext(IDC_PADS_RT_DDN, machines[machine].keynames[swapped ? 0 : 1][num_to_num[paddledown]]);
            setpadtext(IDC_PADS_RT_ADN, machines[machine].keynames[swapped ? 0 : 1][num_to_num[paddledown]]);
        } else
        {   // assert(viewpadsas == 2);
            setpadtext(IDC_PADS_LT_DDN, overlays[whichoverlay][swapped ? guestkeys[num_to_num[paddledown]].p2 : guestkeys[num_to_num[paddledown]].p1]);
            setpadtext(IDC_PADS_LT_ADN, overlays[whichoverlay][swapped ? guestkeys[num_to_num[paddledown]].p2 : guestkeys[num_to_num[paddledown]].p1]);
            setpadtext(IDC_PADS_RT_DDN, overlays[whichoverlay][swapped ? guestkeys[num_to_num[paddledown]].p1 : guestkeys[num_to_num[paddledown]].p2]);
            setpadtext(IDC_PADS_RT_ADN, overlays[whichoverlay][swapped ? guestkeys[num_to_num[paddledown]].p1 : guestkeys[num_to_num[paddledown]].p2]);
        }
        if (paddleleft == -1)
        {   setpadtext(IDC_PADS_LT_DLT, LLL(MSG_KEY_LT, "Lt"));
            setpadtext(IDC_PADS_LT_ALT, LLL(MSG_KEY_LT, "Lt"));
            setpadtext(IDC_PADS_RT_DLT, LLL(MSG_KEY_LT, "Lt"));
            setpadtext(IDC_PADS_RT_ALT, LLL(MSG_KEY_LT, "Lt"));
        } elif (viewpadsas == 0) // guest
        {   setpadtext(IDC_PADS_LT_DLT, machines[machine].keynames[swapped ? 1 : 0][num_to_num[paddleleft]]);
            setpadtext(IDC_PADS_LT_ALT, machines[machine].keynames[swapped ? 1 : 0][num_to_num[paddleleft]]);
            setpadtext(IDC_PADS_RT_DLT, machines[machine].keynames[swapped ? 0 : 1][num_to_num[paddleleft]]);
            setpadtext(IDC_PADS_RT_ALT, machines[machine].keynames[swapped ? 0 : 1][num_to_num[paddleleft]]);
        } else
        {   // assert(viewpadsas == 2);
            setpadtext(IDC_PADS_LT_DLT, overlays[whichoverlay][swapped ? guestkeys[num_to_num[paddleleft]].p2 : guestkeys[num_to_num[paddleleft]].p1]);
            setpadtext(IDC_PADS_LT_ALT, overlays[whichoverlay][swapped ? guestkeys[num_to_num[paddleleft]].p2 : guestkeys[num_to_num[paddleleft]].p1]);
            setpadtext(IDC_PADS_RT_DLT, overlays[whichoverlay][swapped ? guestkeys[num_to_num[paddleleft]].p1 : guestkeys[num_to_num[paddleleft]].p2]);
            setpadtext(IDC_PADS_RT_ALT, overlays[whichoverlay][swapped ? guestkeys[num_to_num[paddleleft]].p1 : guestkeys[num_to_num[paddleleft]].p2]);
        }
        if (paddleright == -1)
        {   setpadtext(IDC_PADS_LT_DRT, LLL(MSG_KEY_RT, "Rt"));
            setpadtext(IDC_PADS_LT_ART, LLL(MSG_KEY_RT, "Rt"));
            setpadtext(IDC_PADS_RT_DRT, LLL(MSG_KEY_RT, "Rt"));
            setpadtext(IDC_PADS_RT_ART, LLL(MSG_KEY_RT, "Rt"));
        } elif (viewpadsas == 0) // guest
        {   setpadtext(IDC_PADS_LT_DRT, machines[machine].keynames[swapped ? 1 : 0][num_to_num[paddleright]]);
            setpadtext(IDC_PADS_LT_ART, machines[machine].keynames[swapped ? 1 : 0][num_to_num[paddleright]]);
            setpadtext(IDC_PADS_RT_DRT, machines[machine].keynames[swapped ? 0 : 1][num_to_num[paddleright]]);
            setpadtext(IDC_PADS_RT_ART, machines[machine].keynames[swapped ? 0 : 1][num_to_num[paddleright]]);
        } else
        {   // assert(viewpadsas == 2);
            setpadtext(IDC_PADS_LT_DRT, overlays[whichoverlay][swapped ? guestkeys[num_to_num[paddleright]].p2 : guestkeys[num_to_num[paddleright]].p1]);
            setpadtext(IDC_PADS_LT_ART, overlays[whichoverlay][swapped ? guestkeys[num_to_num[paddleright]].p2 : guestkeys[num_to_num[paddleright]].p1]);
            setpadtext(IDC_PADS_RT_DRT, overlays[whichoverlay][swapped ? guestkeys[num_to_num[paddleright]].p1 : guestkeys[num_to_num[paddleright]].p2]);
            setpadtext(IDC_PADS_RT_ART, overlays[whichoverlay][swapped ? guestkeys[num_to_num[paddleright]].p1 : guestkeys[num_to_num[paddleright]].p2]);
        }

        if (viewpadsas == 0)
        {   setpadtext(IDC_PADS_LT_1,    machines[machine].keynames[swapped ? 1 : 0][num_to_num[buttontranslate(0, 0)]]);
            setpadtext(IDC_PADS_LT_11,   machines[machine].keynames[swapped ? 1 : 0][num_to_num[key1]]);
            setpadtext(IDC_PADS_LT_12,   machines[machine].keynames[swapped ? 1 : 0][num_to_num[key1]]);
            setpadtext(IDC_PADS_LT_2,    machines[machine].keynames[swapped ? 1 : 0][num_to_num[buttontranslate(0, 1)]]);
            setpadtext(IDC_PADS_LT_3,    machines[machine].keynames[swapped ? 1 : 0][num_to_num[buttontranslate(0, 2)]]);
            setpadtext(IDC_PADS_LT_4,    machines[machine].keynames[swapped ? 1 : 0][num_to_num[buttontranslate(0, 3)]]);
            setpadtext(IDC_PADS_LTMOUSE, machines[machine].keynames[swapped ? 1 : 0][num_to_num[key1]]);
            setpadtext(IDC_PADS_MDMOUSE, machines[machine].keynames[swapped ? 1 : 0][num_to_num[key2]]);
            if (guestrmb)
            {   setpadtext(IDC_PADS_RTMOUSE, machines[machine].keynames[swapped ? 1 : 0][num_to_num[key3]]);
            } else
            {   setpadtext(IDC_PADS_RTMOUSE, LLL(MSG_RIGHT , "Right" ));
            }

            setpadtext(IDC_PADS_RT_1,    machines[machine].keynames[swapped ? 0 : 1][num_to_num[buttontranslate(1, 0)]]);
            setpadtext(IDC_PADS_RT_11,   machines[machine].keynames[swapped ? 0 : 1][num_to_num[key1]]);
            setpadtext(IDC_PADS_RT_12,   machines[machine].keynames[swapped ? 0 : 1][num_to_num[key1]]);
            setpadtext(IDC_PADS_RT_2,    machines[machine].keynames[swapped ? 0 : 1][num_to_num[buttontranslate(1, 1)]]);
            setpadtext(IDC_PADS_RT_3,    machines[machine].keynames[swapped ? 0 : 1][num_to_num[buttontranslate(1, 2)]]);
            setpadtext(IDC_PADS_RT_4,    machines[machine].keynames[swapped ? 0 : 1][num_to_num[buttontranslate(1, 3)]]);
        } else
        {   setpadtext(IDC_PADS_LT_1,    overlays[whichoverlay][swapped ? guestkeys[num_to_num[buttontranslate(0, 0)]].p2 : guestkeys[num_to_num[buttontranslate(0, 0)]].p1]);
            setpadtext(IDC_PADS_LT_11,   overlays[whichoverlay][swapped ? guestkeys[num_to_num[key1                 ]].p2 : guestkeys[num_to_num[key1                 ]].p1]);
            setpadtext(IDC_PADS_LT_12,   overlays[whichoverlay][swapped ? guestkeys[num_to_num[key1                 ]].p2 : guestkeys[num_to_num[key1                 ]].p1]);
            setpadtext(IDC_PADS_LT_2,    overlays[whichoverlay][swapped ? guestkeys[num_to_num[buttontranslate(0, 1)]].p2 : guestkeys[num_to_num[buttontranslate(0, 1)]].p1]);
            setpadtext(IDC_PADS_LT_3,    overlays[whichoverlay][swapped ? guestkeys[num_to_num[buttontranslate(0, 2)]].p2 : guestkeys[num_to_num[buttontranslate(0, 2)]].p1]);
            setpadtext(IDC_PADS_LT_4,    overlays[whichoverlay][swapped ? guestkeys[num_to_num[buttontranslate(0, 3)]].p2 : guestkeys[num_to_num[buttontranslate(0, 3)]].p1]);
            setpadtext(IDC_PADS_LTMOUSE, overlays[whichoverlay][swapped ? guestkeys[num_to_num[key1                 ]].p2 : guestkeys[num_to_num[key1                 ]].p1]);
            setpadtext(IDC_PADS_MDMOUSE, overlays[whichoverlay][swapped ? guestkeys[num_to_num[key2                 ]].p2 : guestkeys[num_to_num[key2                 ]].p1]);
            if (guestrmb)
            {   setpadtext(IDC_PADS_RTMOUSE, overlays[whichoverlay][swapped ? guestkeys[num_to_num[key3             ]].p2 : guestkeys[num_to_num[key3                 ]].p1]);
            } else
            {   setpadtext(IDC_PADS_RTMOUSE, LLL(MSG_RIGHT , "Right" ));
            }

            setpadtext(IDC_PADS_RT_1,    overlays[whichoverlay][swapped ? guestkeys[num_to_num[buttontranslate(1, 0)]].p1 : guestkeys[num_to_num[buttontranslate(1, 0)]].p2]);
            setpadtext(IDC_PADS_RT_11,   overlays[whichoverlay][swapped ? guestkeys[num_to_num[key1                 ]].p1 : guestkeys[num_to_num[key1                 ]].p2]);
            setpadtext(IDC_PADS_RT_12,   overlays[whichoverlay][swapped ? guestkeys[num_to_num[key1                 ]].p1 : guestkeys[num_to_num[key1                 ]].p2]);
            setpadtext(IDC_PADS_RT_2,    overlays[whichoverlay][swapped ? guestkeys[num_to_num[buttontranslate(1, 1)]].p1 : guestkeys[num_to_num[buttontranslate(1, 1)]].p2]);
            setpadtext(IDC_PADS_RT_3,    overlays[whichoverlay][swapped ? guestkeys[num_to_num[buttontranslate(1, 2)]].p1 : guestkeys[num_to_num[buttontranslate(1, 2)]].p2]);
            setpadtext(IDC_PADS_RT_4,    overlays[whichoverlay][swapped ? guestkeys[num_to_num[buttontranslate(1, 3)]].p1 : guestkeys[num_to_num[buttontranslate(1, 3)]].p2]);
        }
    acase 1: // host
        setpadtext(IDC_PADS_LT_A,    LLL(MSG_START2, "Start" ));
        setpadtext(IDC_PADS_LT_B,    LLL(MSG_SELECT, "SELECT")); // would be better in mixed case in this instance
        setpadtext(IDC_PADS_LT_1,    "1");
        setpadtext(IDC_PADS_LT_2,    "2");
        setpadtext(IDC_PADS_LT_3,    "3");
        setpadtext(IDC_PADS_LT_4,    "4");
        setpadtext(IDC_PADS_LT_5,    "5");
        setpadtext(IDC_PADS_LT_6,    "6");
        setpadtext(IDC_PADS_LT_7,    "7");
        setpadtext(IDC_PADS_LT_8,    "8");
        setpadtext(IDC_PADS_LT_11,   "11");
        setpadtext(IDC_PADS_LT_12,   "12");
        setpadtext(IDC_PADS_LT_DUP,  LLL(MSG_KEY_UP, "Up"    ));
        setpadtext(IDC_PADS_LT_DDN,  LLL(MSG_KEY_DN, "Dn"    ));
        setpadtext(IDC_PADS_LT_DLT,  LLL(MSG_KEY_LT, "Lt"    ));
        setpadtext(IDC_PADS_LT_DRT,  LLL(MSG_KEY_RT, "Rt"    ));
        setpadtext(IDC_PADS_LT_AUP,  LLL(MSG_KEY_UP, "Up"    ));
        setpadtext(IDC_PADS_LT_ADN,  LLL(MSG_KEY_DN, "Dn"    ));
        setpadtext(IDC_PADS_LT_ALT,  LLL(MSG_KEY_LT, "Lt"    ));
        setpadtext(IDC_PADS_LT_ART,  LLL(MSG_KEY_RT, "Rt"    ));
        setpadtext(IDC_PADS_LT_AUP2, LLL(MSG_KEY_UP, "Up"    ));
        setpadtext(IDC_PADS_LT_ADN2, LLL(MSG_KEY_DN, "Dn"    ));
        setpadtext(IDC_PADS_LT_ALT2, LLL(MSG_KEY_LT, "Lt"    ));
        setpadtext(IDC_PADS_LT_ART2, LLL(MSG_KEY_RT, "Rt"    ));

        setpadtext(IDC_PADS_RT_A,    LLL(MSG_START2, "Start" ));
        setpadtext(IDC_PADS_RT_B,    LLL(MSG_SELECT, "SELECT")); // would be better in mixed case in this instance
        setpadtext(IDC_PADS_RT_1,    "1");
        setpadtext(IDC_PADS_RT_2,    "2");
        setpadtext(IDC_PADS_RT_3,    "3");
        setpadtext(IDC_PADS_RT_4,    "4");
        setpadtext(IDC_PADS_RT_5,    "5");
        setpadtext(IDC_PADS_RT_6,    "6");
        setpadtext(IDC_PADS_RT_7,    "7");
        setpadtext(IDC_PADS_RT_8,    "8");
        setpadtext(IDC_PADS_RT_11,   "11");
        setpadtext(IDC_PADS_RT_12,   "12");
        setpadtext(IDC_PADS_RT_DUP,  LLL(MSG_KEY_UP, "Up"    ));
        setpadtext(IDC_PADS_RT_DDN,  LLL(MSG_KEY_DN, "Dn"    ));
        setpadtext(IDC_PADS_RT_DLT,  LLL(MSG_KEY_LT, "Lt"    ));
        setpadtext(IDC_PADS_RT_DRT,  LLL(MSG_KEY_RT, "Rt"    ));
        setpadtext(IDC_PADS_RT_AUP,  LLL(MSG_KEY_UP, "Up"    ));
        setpadtext(IDC_PADS_RT_ADN,  LLL(MSG_KEY_DN, "Dn"    ));
        setpadtext(IDC_PADS_RT_ALT,  LLL(MSG_KEY_LT, "Lt"    ));
        setpadtext(IDC_PADS_RT_ART,  LLL(MSG_KEY_RT, "Rt"    ));
        setpadtext(IDC_PADS_RT_AUP2, LLL(MSG_KEY_UP, "Up"    ));
        setpadtext(IDC_PADS_RT_ADN2, LLL(MSG_KEY_DN, "Dn"    ));
        setpadtext(IDC_PADS_RT_ALT2, LLL(MSG_KEY_LT, "Lt"    ));
        setpadtext(IDC_PADS_RT_ART2, LLL(MSG_KEY_RT, "Rt"    ));

        setpadtext(IDC_PADS_LTMOUSE, LLL(MSG_LEFT  , "Left"  ));
        setpadtext(IDC_PADS_MDMOUSE, LLL(MSG_MIDDLE, "Middle"));
        setpadtext(IDC_PADS_RTMOUSE, LLL(MSG_RIGHT , "Right" ));
}   }

EXPORT void invert(int localkey)
{   HBITMAP OldBitmapPtr;
    HDC     CompatibleRastPort;

    if (keyinfo[whichkeyrect][localkey].left == -1)
    {   return;
    }

    therect.left   = keyinfo[whichkeyrect][localkey].left;
    therect.top    = keyinfo[whichkeyrect][localkey].top;
    therect.right  = keyinfo[whichkeyrect][localkey].left + keyinfo[whichkeyrect][localkey].width;
    therect.bottom = keyinfo[whichkeyrect][localkey].top  + keyinfo[whichkeyrect][localkey].height;
    if (wantellipse(localkey))
    {   if (newkeys[localkey])
        {   SelectObject(ControlsRastPtr1, hBrush[EMUBRUSH_BLACK]);
            Ellipse
            (   ControlsRastPtr1,
                therect.left   - (machine == BINBUG ? -2 : 2),
                therect.top    - (machine == BINBUG ? -2 : 2),
                therect.right  + (machine == BINBUG ? -2 : 2),
                therect.bottom + (machine == BINBUG ? -2 : 2)
            );
        } else
        {   CompatibleRastPort = CreateCompatibleDC(ControlsRastPtr1);
            OldBitmapPtr = (HBITMAP) SelectObject(CompatibleRastPort, controlsicon);
            BitBlt
            (   ControlsRastPtr1,
                therect.left   - 2,
                therect.top    - 2,
                therect.right  + 2 - (therect.left - 2) + 1,
                therect.bottom + 2 - (therect.top  - 2) + 1,
                CompatibleRastPort,
                therect.left   - 2,
                therect.top    - 2,
                SRCCOPY
            );
            SelectObject(CompatibleRastPort, OldBitmapPtr);
            DeleteDC(CompatibleRastPort);
    }   }
    else
    {   InvertRect((machine == PIPBUG) ? ControlsRastPtr2 : ControlsRastPtr1, &therect);
}   }

EXPORT void invert2(int left, int top, int right, int bottom)
{   RECT localrect;

    localrect.left   = left;
    localrect.top    = top;
    localrect.right  = right;
    localrect.bottom = bottom;

    if (machine == PIPBUG)
    {   ControlsRastPtr2 = GetDC(GetDlgItem(SubWindowPtr[SUBWINDOW_CONTROLS], IDC_CONTROLS2));
        InvertRect(ControlsRastPtr2, &localrect);
        ReleaseDC(GetDlgItem(SubWindowPtr[SUBWINDOW_CONTROLS], IDC_CONTROLS2), ControlsRastPtr2);
    } else
    {   ControlsRastPtr1 = GetDC(GetDlgItem(SubWindowPtr[SUBWINDOW_CONTROLS], IDC_CONTROLS));
        InvertRect(ControlsRastPtr1, &localrect);
        ReleaseDC(GetDlgItem(SubWindowPtr[SUBWINDOW_CONTROLS], IDC_CONTROLS), ControlsRastPtr1);
}   }

EXPORT void drawctrlglow_system(FLAG lit)
{   ControlsRastPtr1 = GetDC(GetDlgItem(SubWindowPtr[SUBWINDOW_CONTROLS], IDC_CONTROLS));
    if (machine == INSTRUCTOR)
    {   SelectObject(ControlsRastPtr1, hBrush[lit ? EMUBRUSH_RED     : EMUBRUSH_DARKBLUE]); // almost pink
    } else
    {   // assert(machine == ARCADIA || machine == PIPBUG);
        SelectObject(ControlsRastPtr1, hBrush[lit ? EMUBRUSH_DARKRED : EMUBRUSH_DARKBLUE]);
    }
    Ellipse
    (   ControlsRastPtr1,
        therect.left   - 2,
        therect.top    - 2,
        therect.right  + 2,
        therect.bottom + 2
    );
    ReleaseDC(GetDlgItem(SubWindowPtr[SUBWINDOW_CONTROLS], IDC_CONTROLS), ControlsRastPtr1);
}

MODULE int buttontranslate(int player, int which)
{   switch (button[swapped ? (1 - player) : player][which])
    {
    acase 2: return key2;
    acase 3: return key3;
    acase 4: return key4;
    adefault: // eg. 1
        return key1;
}   }
