// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
    #include <dos/dostags.h>      // for SystemTags()
    #ifdef __amigaos4__
        #include <dos/obsolete.h> // for CurrentDir()
    #endif
    #include <proto/intuition.h>  // for WindowToFront()
    #include <proto/locale.h>     // for GetCatalogStr()
#endif
#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
    #include <conio.h>
#endif

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"

// DEFINES----------------------------------------------------------------

// #define CHECKLITERALS
// whether check_labels() should also check for any tokens/symbols/labels
// which could be interpreted as hex literals

#define ASKUSAGE (arg2[0] == '?' && arg2[1] == EOS && arg3[0] == EOS)

#define MINLENGTH       2

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       TEXT                     arg1[MAX_PATH + 80 + 1],
                                      arg2[MAX_PATH + 80 + 1],
                                      arg3[MAX_PATH + 80 + 1],
                                      arg4[MAX_PATH + 80 + 1],
                                      arg5[MAX_PATH + 80 + 1],
                                      arg6[MAX_PATH + 80 + 1],
                                      arg7[MAX_PATH + 80 + 1],
                                      fn_asm[MAX_PATH + 1],
                                      userinput[MAX_PATH + 80 + 1] = "";
EXPORT       int                      drawmode  = 0, // INTERTON/ELEKTOR/MALZAK: 0..3, ARCADIA/PIPBIN/CD2650/TWIN/PHUNSY/SELBST/ZACCARIA: 0..1
                                      foundequiv,
                                      poketoken;
EXPORT       struct ConditionalStruct bp[32768],
                                      wp[ALLTOKENS];
#ifdef WIN32
EXPORT       FLAG                     hurry     = FALSE;
#endif

EXPORT const TEXT                     led_chars[128 + 1] =
// 0               1               2               3
// 0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF
  " ·'·,·17_=····J·············u·J·'r·n···7········1······nLC···GU0" \
  "-=······=······3r··?n···c··2o·da·cuo··49·····5Y9·F·Ph·HAtE··66·8";

EXPORT const STRPTR ccstring[STYLES][4] =
{ { "eq",
    "gt",
    "lt",
    "un"
  },
  { "eq",
    "gt",
    "lt",
    "un"
  },
  { ",EQ",
    ",GT",
    ",LT",
    "   "
  },
  { "EQ",
    "GT",
    "LT",
    "  "
} };

EXPORT const TEXT pswbit[STYLES][9 + 1] =
{ "SFIDRWOMC", "SFIDRWOMC", "IOFHBWVLC", "SFIDRWOMC"
};

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       FLAG                     assembling,
                                      multimode;
IMPORT       UBYTE                    banked,
                                      cc,
                                      coverage_io[258],
                                      drivedata,
                                      memory[32768],
                                      psl,
                                      psu,
                                      q_bank[16][16384],
                                      r[7],
                                      startaddr_h,
                                      startaddr_l,
                                      reqsector,
                                      trackreg,
                                      u_bank[16][2048];
IMPORT       UBYTE*                   IOBuffer;
IMPORT       UWORD                    iar,
                                      mirror_r[32768],
                                      mirror_w[32768],
                                      pc,
                                      ras[8];
IMPORT       ULONG                    asicreads[32768],
                                      asicwrites[32768],
                                      coverage[32768],
                                      cycles_2650,
                                      paused,
                                      region,
                                      sound,
                                      usespeech,
                                      verbosedisk;
IMPORT       TEXT                     asciiname_short[259][3 + 1],
                                      diskext[100][3 + 1],
                                      diskname[100][10 + 1],
                                      file_game[MAX_PATH + 1],
                                      friendly[FRIENDLYLENGTH + 1],
                                      gtempstring[256 + 1],
                                      letters[8 + 1],
                                      netmsg_out[80 + 1],
                                      olduserinput[HISTORYLINES][MAX_PATH + 80 + 1],
                                      path_projects[MAX_PATH + 1],
                                      path_screenshots[MAX_PATH + 1],
                                      ProgDir[MAX_PATH + 1],
                                      v_bin[3 + 8 + 1],
                                      XStr[4 + 1], YStr[3 + 1];
IMPORT       int                      base,
                                      cd2650_biosver,
                                      cd2650_dosver,
                                      cd2650_vdu,
                                      connected,
                                      cpux, cpuy,
                                      crippled,
                                      debugdrive,
                                      disassembling,
                                      firstdosequiv, lastdosequiv,
                                      fromnum,
                                      interrupt_2650,
                                      lastparse,
                                      logbios,
                                      loginefficient,
                                      log_illegal,
                                      log_interrupts,
                                      logreadwrites,
                                      logsubroutines,
                                      machine,
                                      memmap,
                                      n1, n2, n3, n4,
                                      nextdis,
                                      number,
                                      pass,
                                      pausebreak,
                                      pauselog,
                                      phunsy_biosver,
                                      ppc,
                                      pvibase,
                                      rastn,
                                      runtointerrupt,
                                      runtoframe,
                                      runtoloopend,
                                      selbst_biosver,
                                      speakable,
                                      spritemode,
                                      step,
                                      stepdir,
                                      style,
                                      supercpu,
                                      table_size_2650[256],
                                      timeunit,
                                      tolimit,
                                      tonum,
                                      trace,
                                      traceorstep,
                                      twin_dosver,
                                      useguideray,
                                      userlabels,
                                      verbosity,
                                      watchreads,
                                      watchwrites,
                                      whichgame;
IMPORT       char                     pslbits[6 + 1],
                                      psubits[5 + 1];
IMPORT       float                    dividend;
IMPORT       STRPTR                   colourname[8],
                                      timeunitstr1,
                                      timeunitstr2;
IMPORT const UBYTE                    from_a[16];
IMPORT const STRPTR                   coveragename[32],
                                      tokenname[LASTTOKEN - FIRSTTOKEN + 1][STYLES];
IMPORT const TEXT                     arcadia_chars[64 + 1],
                                      astrowars_chars[256 + 1],
                                      cd2650_chars[128 + 1],
                                      elektor_chars1[256 + 1],
                                      galaxia_chars[128 + 1],
                                      instructor_chars1[256 + 1],
                                      lb_chars[256 + 1];
IMPORT       MEMFLAG                  memflags[ALLTOKENS];
IMPORT       struct DriveStruct       drive[DRIVES_MAX];
IMPORT       struct EquivalentStruct  equivalents[],
                                      symlabel[SYMLABELS_MAX + 1];
IMPORT       struct FlagNameStruct    flagname[CPUTIPS];
IMPORT       struct IdealStruct       idealfreq_ntsc[256],
                                      idealfreq_pal[256];
IMPORT       struct IOPortStruct      ioport[258];
IMPORT       struct MachineStruct     machines[MACHINES];
IMPORT       struct NoteStruct        notes[NOTES + 1];
IMPORT       struct OpcodeStruct      opcodes[3][256];
IMPORT const struct KindStruct        filekind[KINDS];
IMPORT const struct KnownStruct       known[KNOWNGAMES];
IMPORT const struct MenuStruct        menuinfo1[MENUITEMS],
                                      menuinfo2[MENUOPTS];
#ifdef AMIGA
    IMPORT   int                      morphos;
    IMPORT   UWORD                    wbver;
    IMPORT   BPTR                     ProgLock;
    IMPORT   struct Catalog*          CatalogPtr;
    IMPORT   struct Window*           SubWindowPtr[SUBWINDOWS];
#endif
#ifdef WIN32
    IMPORT   int                      CatalogPtr; // APTR doesn't work
    IMPORT   HWND                     MainWindowPtr,
                                      SubWindowPtr[SUBWINDOWS];
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE       UBYTE                    AOFBuffer[13 + (255 * 2) + 1],
                                      watchmask;
MODULE       TEXT                     firststring[FRIENDLYLENGTH + 1],
                                      secondstring[FRIENDLYLENGTH + 1];
MODULE       int                      friendlycolour,
                                      symoffset,
                                      symsize,
                                      tabpos;
MODULE       STRPTR                   condstring,
                                      SymBuffer;

/* MODULE STRUCTURES------------------------------------------------------

(None)

MODULE ARRAYS---------------------------------------------------------- */

MODULE const STRPTR p1dos_error[18 + 1] = {
"Unused"                                        , //  0
"errno_NoAccs: no access, wrong password"       , //  1
"errno_WrongCW: no access, wrong codeword"      , //  2
"errno_FileNF: file not found"                  , //  3
"errno_FilesOpen: too many files open"          , //  4
"errno_CRC: CRC error while reading disk"       , //  5
"errno_NotRdy: drive unit not ready"            , //  6
"errno_LostDATA: lost data reading/writing disk", //  7
"errno_RecordNF: sector not found"              , //  8
"errno_WriteP: disk is write protected"         , //  9
"errno_Seek: seek error"                        , // 10
"errno_TrkOVF: track or sector overflow"        , // 11
"errno_Exists: file already exists"             , // 12
"errno_BinFile: image file error"               , // 13
"errno_InvOp: invalid operation or command"     , // 14
"errno_NoSpace: no space left on disk"          , // 15
"errno_NoDirs: no directory space left (too many files)", // 16
"errno_TooLong: file too long"                  , // 17
"errno_TooSmall: file too small"                , // 18
}, twin_error[69 + 1] = {
"Unused"                             , //  0
"Directory read error"               , //  1
"Directory write error"              , //  2
"Command file not found"             , //  3
"Command file input error"           , //  4
"Command file busy"                  , //  5
"Device read error"                  , //  6
"Device write error or end-of-device", //  7
"Drive not specified when required"  , //  8
"Invalid drive number"               , //  9
"Overlay load failure"               , // 10
"Overlay area in use"                , // 11
"Invalid file name"                  , // 12
"Input file not found"               , // 13
"Invalid input device"               , // 14
"Invalid output device"              , // 15
"Input device assign failure"        , // 16
"Output device assign failure"       , // 17
"Device in use"                      , // 18
"Invalid channel number"             , // 19
"Channel in use"                     , // 20
"Channel assign failure"             , // 21
"Command line buffer overflow"       , // 22
"Invalid command"                    , // 23
"Job not active"                     , // 24
"Job not suspended"                  , // 25
"Job already suspended"              , // 26
"Job executing"                      , // 27
"Job under debug control"            , // 28
"PROM power failure (front panel)"   , // 29
"Invalid parameter"                  , // 30
"Parameter required"                 , // 31
"Too many parameters"                , // 32
"Bias parameter error"               , // 33
"Invalid address"                    , // 34
"Invalid start address"              , // 35
"Invalid end address"                , // 36
"Invalid go address"                 , // 37
"Unused"                             , // 38
"Invalid hex character"              , // 39
"Invalid RHEX input format"          , // 40
"Invalid breakpoint access mode"     , // 41
"Invalid register parameter"         , // 42
"Invalid data parameter"             , // 43
"Invalid trace mode parameter"       , // 44
"Invalid slave SRB address"          , // 45
"Slave halted"                       , // 46
"System area bad"                    , // 47
"Load file not found"                , // 48
"Load file assign failure"           , // 49
"File not a load module"             , // 50
"Invalid load request"               , // 51
"Invalid device"                     , // 52
"Unused"                             , // 53
"Invalid mode"                       , // 54
"Invalid memory"                     , // 55
"Invalid device address"             , // 56
"File name in use"                   , // 57
"Device assign failure"              , // 58
"Memory write error"                 , // 59
"End of media"                       , // 60
"File in use"                        , // 61
"Device not operational"             , // 62
"Directory full"                     , // 63
"Invalid diskette"                   , // 64
"Master memory parity error"         , // 65
"Slave memory parity error"          , // 66
"Unused"                             , // 67
"Debug utility routine conflict or memory wraparound", // 68
"Unused"                             , // 69
};

// MODULE FUNCTIONS-------------------------------------------------------

MODULE FLAG bp_add(int address, ULONG the1st, UBYTE the2nd, ULONG the3rd);
MODULE FLAG checkagainst(int number1, int number2, STRPTR thestring, FLAG both, int which);
MODULE FLAG checkagainst_sym(int number1, int number2, STRPTR thestring, FLAG both, int which);
MODULE FLAG check_range(int address1, int address2);
MODULE void help_commands(int which);
MODULE FLAG ip_add(int address, ULONG the1st, UBYTE the2nd, ULONG the3rd, UBYTE mask, FLAG update);
MODULE STRPTR labelstring1(TEXT whichkind);
MODULE void makestrings(struct ConditionalStruct* cond);
MODULE void multifunc(int number, TEXT labelkind, int multimode);
MODULE FLAG save_iof(STRPTR filename, int startaddr, int endaddr);
MODULE int showasic(int whichmem, ULONG whichflag);
MODULE void showfound(int startpoint, int endpoint);
MODULE void skipws(TEXT* bufptr);
MODULE void skipnonws(TEXT* bufptr);
MODULE void skipline(TEXT* bufptr);
MODULE int string_to_cond(STRPTR thearg);
MODULE void view_imagery(int address1, int address2, FILE* filehandle);
MODULE void view_coverage(int reporttype);
MODULE void write_aof_byte(UBYTE* output, UBYTE value);

// CODE-------------------------------------------------------------------

EXPORT FLAG debug_command(void)
{   TRANSIENT       int    address1, // not UWORD!
                           address2, // not UWORD!
                           address3, // not UWORD!
                           found,
                           length,
                           localsize,
                           i, j,
                           preferredsize,
                           result,
                           temp;
    TRANSIENT       FLAG   found2,
                           indirect,
                           ok,
                           started;
    TRANSIENT       TEXT   rowchars[16 + 1],
                           tempchar;
    TRANSIENT       UBYTE  t;
    TRANSIENT       FILE*  TheLocalHandle; // LocalHandle is a variable of winbase.h
    FAST            UBYTE  the2nd;
    FAST            ULONG  the1st;
    PERSIST         UBYTE  tempdata[5];
    PERSIST         TEXT   tempstring[MAX_PATH + 80 + 1],
                           thearg[80 + 1];
    PERSIST         int    trainval  = OUTOFRANGE;
    PERSIST   const STRPTR wwname[3] = { "NONE", "SOME", "ALL" };

    if (userinput[0] == '*')
    {
#ifdef AMIGA
        DISCARD SystemTags
        (   (const char*) &userinput[1],
            SYS_Asynch, TRUE,
            SYS_Input,  NULL,
            SYS_Output, NULL,
        TAG_DONE);
#endif
#ifdef WIN32
        DISCARD ShellExecute(MainWindowPtr, "open", &userinput[1], NULL, NULL, SW_SHOWNORMAL);
#endif
        return FALSE;
    }

    arg1[0] =
    arg2[0] =
    arg3[0] =
    arg4[0] =
    arg5[0] =
    arg6[0] =
    arg7[0] = EOS;
    DISCARD sscanf((const char*) userinput, "%s %s %s %s %s %s %s", arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    if (userinput[0] == QUOTES)
    {   if (connected == NET_SERVER || connected == NET_CLIENT)
        {   if (strlen((const char*) &userinput[1]) > 80)
            {   userinput[78] =
                userinput[79] =
                userinput[80] = '.';
                userinput[81] = EOS;
                // userinput[ 0    ] is  the quote
                // userinput[ 1..77] are the first 77 characters of the message
                // userinput[78..80] are an  ellipsis (...)
                // userinput[81    ] is  the EOS
            }
            strcpy((char*) netmsg_out, (const char*) &userinput[1]);
            zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
        } else
        {   zprintf
            (   TEXTPEN_CLIOUTPUT,
                LLL(
                    MSG_ENGINE_UNAVAILABLE,
                    "Command is unavailable.\n\n"
            )   );
    }   }
    elif (!stricmp((const char*) arg1, "SAY") || !stricmp((const char*) arg1, "SPEAK") || !stricmp((const char*) arg1, "SPK"))
    {   if (sound && speakable)
        {   // we don't require usespeech
            if (strlen((const char*) &userinput[1]) > 80)
            {   userinput[81] = EOS;
            }
            if (speak((STRPTR) &userinput[strlen((const char*) arg1) + 1]))
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
            } else
            {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
        }   }
        else
        {   zprintf
            (   TEXTPEN_CLIOUTPUT,
                LLL(
                    MSG_ENGINE_UNAVAILABLE,
                    "Command is unavailable.\n\n"
            )   );
            // maybe we should make it unavailable for non-speaking guests (eg. Arcadia)
    }   }
    elif (!stricmp((const char*) arg1, "S"))
    {   if (allowable(TRUE))
        {   step = TRUE;
            traceorstep = (trace || step) ? TRUE : FALSE;
            emu_unpause();
    }   }
    elif (!stricmp((const char*) arg1, "SPR"))
    {   if (ASKUSAGE || !arg2[0] || arg3[0])
        {   zprintf(TEXTPEN_CLIOUTPUT, "SPR 0: %s\n" \
                                       "SPR 1: %s\n" \
                                       "SPR 2: %s\n",
                                       LLL(menuinfo2[MENUOPT_SPR_0].desc_id, menuinfo2[MENUOPT_SPR_0].desc_str),
                                       LLL(menuinfo2[MENUOPT_SPR_1].desc_id, menuinfo2[MENUOPT_SPR_1].desc_str),
                                       LLL(menuinfo2[MENUOPT_SPR_2].desc_id, menuinfo2[MENUOPT_SPR_2].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, "%s: SPR 0|1|2\n\n", LLL(MSG_USAGE, "Usage"));
        } elif
        (   machine != ARCADIA
         && machines[machine].pvis == 0
         && machine != PONG
        )
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
        } else
        {   if   (!strcmp((const char*) arg2, "0")) { spritemode = SPRITEMODE_INVISIBLE; zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK")); }
            elif (!strcmp((const char*) arg2, "1")) { spritemode = SPRITEMODE_VISIBLE;   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK")); }
            elif (!strcmp((const char*) arg2, "2")) { spritemode = SPRITEMODE_NUMBERED;  zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK")); }
            else
            {   zprintf(TEXTPEN_CLIOUTPUT, "SPR 0: %s\n" \
                                           "SPR 1: %s\n" \
                                           "SPR 2: %s\n",
                                           LLL(menuinfo2[MENUOPT_SPR_0].desc_id, menuinfo2[MENUOPT_SPR_0].desc_str),
                                           LLL(menuinfo2[MENUOPT_SPR_1].desc_id, menuinfo2[MENUOPT_SPR_1].desc_str),
                                           LLL(menuinfo2[MENUOPT_SPR_2].desc_id, menuinfo2[MENUOPT_SPR_2].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, "%s: SPR 0|1|2\n\n", LLL(MSG_USAGE, "Usage"));
    }   }   }
    elif (!stricmp((const char*) arg1, "O"))
    {   if (allowable(TRUE))
        {   // shouldn't we resolve mirrors first?
            if ((memory[iar] & 0x78) == 0x38) // %x0111xxx
            {   DISCARD getfriendly((int) iar);
                zprintf
                (   TEXTPEN_TRACE,
                    LLL(
                        MSG_ENGINE_STEPPINGOVER,
                        "Stepping over subroutine call (%s) at %s.\n"
                    ),
                    opcodes[style][memory[iar]].name,
                    friendly
                );
                if (verbosity == 0) // ie. if we are not going to call view_next_2650()
                {   zprintf(TEXTPEN_TRACE, "\n");
                }
                memflags[(iar & PAGE) + ((iar + table_size_2650[memory[iar]]) & NONPAGE)] |= STEPPOINT;
                emu_unpause();
            } else
            {   step = TRUE;
                traceorstep = (trace || step) ? TRUE : FALSE;
                emu_unpause();
    }   }   }
    elif (!stricmp((const char*) arg1, "ASM"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_ASM].desc_id, menuinfo1[MENUITEM_ASM].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_ASM, "Usage: ASM <filename>\n\n"));
        } elif (allowable(TRUE))
        {   if (!arg2[0] || arg3[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_ASM].desc_id, menuinfo1[MENUITEM_ASM].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_ASM, "Usage: ASM <filename>\n\n"));
            } else
            {   strcpy((char*) fn_asm, (const char*) arg2);
                assemble();
    }   }   }
    elif (!stricmp((const char*) arg1, "BASE"))
    {   if (ASKUSAGE || arg3[0])
        {   zprintf(TEXTPEN_CLIOUTPUT, "BASE 2|%%: %s\n" \
                                       "BASE 8|@: %s\n" \
                                       "BASE 10|!: %s\n" \
                                       "BASE 16|$: %s\n",
                                       LLL(menuinfo2[MENUOPT_BASE_BINARY ].desc_id, menuinfo2[MENUOPT_BASE_BINARY ].desc_str),
                                       LLL(menuinfo2[MENUOPT_BASE_OCTAL  ].desc_id, menuinfo2[MENUOPT_BASE_OCTAL  ].desc_str),
                                       LLL(menuinfo2[MENUOPT_BASE_DECIMAL].desc_id, menuinfo2[MENUOPT_BASE_DECIMAL].desc_str),
                                       LLL(menuinfo2[MENUOPT_BASE_HEX    ].desc_id, menuinfo2[MENUOPT_BASE_HEX    ].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, "%s: BASE [2|8|10|16|%%|@|!|$]\n\n", LLL(MSG_USAGE, "Usage"));
        } elif (arg2[0])
        {   if     (!strcmp((const char*) arg2,  "2") || !strcmp((const char*) arg2, "%"))
            {   base = BASE_BINARY;
                zprintf
                (   TEXTPEN_CLIOUTPUT,
                    "%s %s.\n\n",
                    LLL(MSG_WRITEBASE,   "Set default input base to"),
                    LLL(MSG_BINARY,      "binary")
                );
            } elif (!strcmp((const char*) arg2,  "8") || !strcmp((const char*) arg2, "@"))
            {   base = BASE_OCTAL;
                zprintf
                (   TEXTPEN_CLIOUTPUT,
                    "%s %s.\n\n",
                    LLL(MSG_WRITEBASE,   "Set default input base to"),
                    LLL(MSG_OCTAL,       "octal")
                );
            } elif (!strcmp((const char*) arg2, "10") || !strcmp((const char*) arg2, "!"))
            {   base = BASE_DECIMAL;
                zprintf
                (   TEXTPEN_CLIOUTPUT,
                    "%s %s.\n\n",
                    LLL(MSG_WRITEBASE,   "Set default input base to"),
                    LLL(MSG_DECIMAL,     "decimal")
                );
            } elif (!strcmp((const char*) arg2, "16") || !strcmp((const char*) arg2, "$"))
            {   base = BASE_HEX;
                zprintf
                (   TEXTPEN_CLIOUTPUT,
                    "%s %s.\n\n",
                    LLL(MSG_WRITEBASE,   "Set default input base to"),
                    LLL(MSG_HEXADECIMAL, "hexadecimal")
                );
            } else
            {   zprintf(TEXTPEN_CLIOUTPUT, "BASE 2|%%: %s\n" \
                                           "BASE 8|@: %s\n" \
                                           "BASE 10|!: %s\n" \
                                           "BASE 16|$: %s\n",
                                           LLL(menuinfo2[MENUOPT_BASE_BINARY ].desc_id, menuinfo2[MENUOPT_BASE_BINARY ].desc_str),
                                           LLL(menuinfo2[MENUOPT_BASE_OCTAL  ].desc_id, menuinfo2[MENUOPT_BASE_OCTAL  ].desc_str),
                                           LLL(menuinfo2[MENUOPT_BASE_DECIMAL].desc_id, menuinfo2[MENUOPT_BASE_DECIMAL].desc_str),
                                           LLL(menuinfo2[MENUOPT_BASE_HEX    ].desc_id, menuinfo2[MENUOPT_BASE_HEX    ].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, "%s: BASE [2|8|10|16|%%|@|!|$]\n\n", LLL(MSG_USAGE, "Usage"));
        }   }
        else
        {   zprintf
            (   TEXTPEN_CLIOUTPUT,
                "%s ",
                LLL(MSG_READBASE, "Current default input base is")
            );
            switch (base)
            {
            case  BASE_DECIMAL: zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_DECIMAL,     "decimal"    ));
            acase BASE_HEX:     zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_HEXADECIMAL, "hexadecimal"));
            acase BASE_BINARY:  zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_BINARY,      "binary"     ));
            acase BASE_OCTAL:   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OCTAL,       "octal"      ));
    }   }   }
    elif (!stricmp((const char*) arg1, "BC") || !stricmp((const char*) arg1, "DB"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_BC].desc_id, menuinfo1[MENUITEM_BC].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_BC, "Usage: BC [<start-address> [<end-address>]]\n\n"));
        } elif (allowable(TRUE))
        {   if (arg4[0] || (arg2[0] == '?' && arg2[1] == EOS))
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_BC].desc_id, menuinfo1[MENUITEM_BC].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_BC, "Usage: BC [<start-address> [<end-address>]]\n\n"));
            } elif (arg3[0])
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                if (check_range(address1, address2))
                {   for (i = address1; i <= address2; i++)
                    {   memflags[i] &= ~(BREAKPOINT);
                    }
                    update_memory(TRUE);
                    update_monitor(TRUE);
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
            }   }
            elif (arg2[0])
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                if (check_range(address1, MAX_ADDR))
                {   ok = FALSE;
                    if (memflags[address1] & BREAKPOINT)
                    {   memflags[address1] &= ~(BREAKPOINT);
                        ok = TRUE;
                    }
                    DISCARD getfriendly(address1);
                    if (ok)
                    {   zprintf
                        (   TEXTPEN_CLIOUTPUT,
                            LLL
                            (   MSG_ENGINE_CLEAREDBP,
                                "Cleared code breakpoint at %s.\n\n"
                            ),
                            friendly
                        );
                        update_memory(TRUE);
                        update_monitor(TRUE);
                    } else
                    {   zprintf
                        (   TEXTPEN_CLIOUTPUT,
                            LLL
                            (   MSG_ENGINE_BPNOTFOUND,
                                "Breakpoint not found at %s!\n\n"
                            ),
                            friendly
                        );
            }   }   }
            else
            {   ok = FALSE;
                for (address1 = 0; address1 <= MAX_ADDR; address1++)
                {   if (memflags[address1] & BREAKPOINT)
                    {   memflags[address1] &= ~(BREAKPOINT);
                        ok = TRUE;
                }   }
                if (ok)
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL
                        (   MSG_ENGINE_CLEAREDBPS,
                            "Cleared all code breakpoints.\n\n"
                        )
                    );
                    update_memory(TRUE);
                    update_monitor(TRUE);
                } else
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL
                        (   MSG_NOBPS,
                            "No breakpoints to clear!\n\n"
                        )
                    );
    }   }   }   }
    elif (!stricmp((const char*) arg1, "BL") || !stricmp((const char*) arg1, "LB"))
    {   if (allowable(TRUE))
        {   ok = FALSE;
            for (i = 0; i <= MAX_ADDR; i++)
            {   if (memflags[i] & BREAKPOINT)
                {   DISCARD getfriendly(i);
                    zprintf(TEXTPEN_CLIOUTPUT, "%s", friendly);

                    if (bp[i].the2nd == COND_UN)
                    {   zprintf(TEXTPEN_CLIOUTPUT, "\n");
                    } else
                    {   makestrings(&bp[i]);
                        zprintf
                        (   TEXTPEN_CLIOUTPUT,
                            " %s %s %s %s\n",
                            LLL(MSG_WHEN, "when"),
                            firststring,
                            condstring,
                            secondstring
                        );
                    }
                    ok = TRUE;
            }   }
            if (ok)
            {   zprintf(TEXTPEN_CLIOUTPUT, "\n");
            } else
            {   zprintf
                (   TEXTPEN_CLIOUTPUT,
                    "%s.\n\n",
                    LLL(
                        MSG_NONE,
                        "None"
                )   );
    }   }   }
    elif (!stricmp((const char*) arg1, "BP") || !stricmp((const char*) arg1, "SB"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_BP].desc_id, menuinfo1[MENUITEM_BP].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_BP, "Usage: BP <start-addr> [<end-addr>] [<addr/reg> <condition> <value>]\n\n"));
        } elif (allowable(TRUE))
        {   if (arg7[0] || !arg2[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_BP].desc_id, menuinfo1[MENUITEM_BP].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_BP, "Usage: BP <start-addr> [<end-addr>] [<addr/reg> <condition> <value>]\n\n"));
            } elif (arg6[0])
            {   // BP <start-address> <end-address> <address/register> <condition> <value>
                address1 = parse_expression((STRPTR) arg2,  MAX_ADDR, FALSE);
                address2 = parse_expression((STRPTR) arg3,  MAX_ADDR, FALSE);
                the1st   = parse_expression((STRPTR) arg4, ALLTOKENS, FALSE);
                the2nd   = string_to_cond(  (STRPTR) arg5);
                address3 = parse_expression((STRPTR) arg6,       255, FALSE); // not really an address
                if
                (   address1 != OUTOFRANGE
                 && address2 != OUTOFRANGE
                 && address3 != OUTOFRANGE
                 && address1 <= address2
                )
                {   for (i = address1; i <= address2; i++)
                    {   memflags[i]  |= BREAKPOINT;
                        bp[i].the1st =  the1st;
                        bp[i].the2nd =  the2nd;
                        bp[i].the3rd =  (UWORD) address3;
                    }
                    update_memory(TRUE);
                    update_monitor(TRUE);
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_BP].desc_id, menuinfo1[MENUITEM_BP].desc_str));
                    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_BP, "Usage: BP <start-addr> [<end-addr>] [<addr/reg> <condition> <value>]\n\n"));
            }   }
            elif (arg5[0])
            {   // BP <start-address> <address/register> <condition> <value>
                address1 = parse_expression((STRPTR) arg2, MAX_ADDR,  FALSE);
                the1st   = parse_expression((STRPTR) arg3, ALLTOKENS, FALSE);
                the2nd   = string_to_cond(  (STRPTR) arg4);
                address3 = parse_expression((STRPTR) arg5, MAX_ADDR,  FALSE); // not really an address
                DISCARD bp_add(address1, the1st, the2nd, address3);
            } elif (arg4[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_BP].desc_id, menuinfo1[MENUITEM_BP].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_BP, "Usage: BP <start-addr> [<end-addr>] [<addr/reg> <condition> <value>]\n\n"));
            } elif (arg3[0])
            {   // BP <start-address> <end-address>
                address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                if
                (   address1 != OUTOFRANGE
                 && address2 != OUTOFRANGE
                 && address1 <= address2
                )
                {   for (i = address1; i <= address2; i++)
                    {   memflags[i]  |= BREAKPOINT;
                        bp[i].the2nd =  COND_UN; // no need to set bp[i].the1st nor bp[i].the3rd
                    }
                    update_memory(TRUE);
                    update_monitor(TRUE);
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_BP].desc_id, menuinfo1[MENUITEM_BP].desc_str));
                    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_BP, "Usage: BP <start-addr> [<end-addr>] [<addr/reg> <condition> <value>]\n\n"));
            }   }
            else
            {   // BP <start-address>
                address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                DISCARD bp_add(address1, 0, COND_UN, 0);
    }   }   }
    elif (!stricmp((const char*) arg1, "V") || !stricmp((const char*) arg1, "VIEW"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s: V|VIEW BASIC|BIOS|CPU|PSG|RAM|SCRN|UDG|XVI\n\n", LLL(MSG_USAGE, "Usage"));
        } else
        {   if (!stricmp((const char*) arg2, "BASIC"))
            {   switch (machine)
                {
                case  PIPBUG:
                    pipbug_view_basic();
                acase CD2650:
                    cd2650_view_basic();
                adefault:
                    zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
            }   }
            elif (!stricmp((const char*) arg2, "BIOS"))
            {   view_bios_ram();
            } elif (!stricmp((const char*) arg2, "CPU"))
            {   if (allowable(TRUE))
                {   view_cpu_2650(TRUE);
            }   }
            elif (!stricmp((const char*) arg2, "PSG"))
            {   if (memmap == MEMMAP_F)
                {   view_psgs();
                } elif (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
                {   view_tms();
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
            }   }
            elif (!stricmp((const char*) arg2, "RAM"))
            {   if (allowable(TRUE))
                {   view_ram();
            }   }
            elif (!stricmp((const char*) arg2, "SCRN"))
            {   switch (machine)
                {
                case  ARCADIA:                   arcadia_viewscreen();
                acase INTERTON:   case ELEKTOR:       ie_viewscreen();
                acase PIPBUG:                     pipbug_viewscreen();
                acase BINBUG:                     binbug_viewscreen();
                acase INSTRUCTOR:                   si50_viewscreen();
                acase TWIN:                         twin_viewscreen();
                acase CD2650:                     cd2650_viewscreen();
                acase ZACCARIA:                 zaccaria_viewscreen();
                acase MALZAK:                     malzak_viewscreen();
                acase PHUNSY:                     phunsy_viewscreen();
                acase SELBST:                     selbst_viewscreen();
                acase MIKIT:                       mikit_viewscreen();
                acase TYPERIGHT:                      tr_viewscreen();
                adefault: // eg. PONG
                    zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
            }   }
            elif (!stricmp((const char*) arg2, "UDG"))
            {   if (machine == ARCADIA || machines[machine].pvis || machine == BINBUG)
                {   view_udgs();
                } else // eg. PIPBUG, INSTRUCTOR, CD2650, PONG, TYPERIGHT
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
            }   }
            elif (!stricmp((const char*) arg2, "XVI"))
            {   if (machine == ARCADIA || machines[machine].pvis)
                {   view_xvi();
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
            }   }
            else
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s: V|VIEW BASIC|BIOS|CPU|PSG|RAM|SCRN|UDG|XVI\n\n", LLL(MSG_USAGE, "Usage"));
    }   }   }
    elif (!stricmp((const char*) arg1, "CLEARCOV"))
    {   if (allowable(TRUE))
        {   clearcoverage();
            zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
    }   }
    elif (!stricmp((const char*) arg1, "CLEARSYM"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_CLEARSYM].desc_id, menuinfo1[MENUITEM_CLEARSYM].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_CLEARSYM, "Usage: CLEARSYM [<label>]\n\n"));
        } elif (allowable(TRUE))
        {   if (arg3[0] || (arg2[0] == '?' && arg2[1] == EOS))
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_CLEARSYM].desc_id, menuinfo1[MENUITEM_CLEARSYM].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_CLEARSYM, "Usage: CLEARSYM [<label>]\n\n"));
            } elif (arg2[0])
            {   if (userlabels)
                {   found = -1;
                    for (i = 0; i < userlabels; i++)
                    {   if (!stricmp((const char*) arg2, (const char*) symlabel[i].name))
                        {   zprintf
                            (   TEXTPEN_CLIOUTPUT,
                                LLL(MSG_CLEARINGLABEL, "Clearing user-defined label %s (was $%X).\n"),
                                symlabel[i].name,
                                symlabel[i].address
                            );
                            found = i;
                            break;
                    }   }
                    if (found != -1)
                    {   if (found < userlabels - 1)
                        {   for (i = found + 1; i < userlabels; i++)
                            {   strcpy((char*) symlabel[i].name, (const char*) symlabel[i + 1].name);
                                symlabel[i].address = symlabel[i + 1].address;
                        }   }
                        userlabels--;
                }   }
                else
                {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_LABELNOTFOUND, "Label not found.\n\n"));
            }   }
            else
            {   if (userlabels == 0)
                {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NOLABELS, "No user-defined labels to clear!\n\n"));
                } else
                {   userlabels = 0;
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
    }   }   }   }
    elif (!stricmp((const char*) arg1, "CLS"))
    {   cls();
    } elif (!stricmp((const char*) arg1, "CO") || !stricmp((const char*) arg1, "COMP"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_COMP].desc_id, menuinfo1[MENUITEM_COMP].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_CO, "Usage: CO|COMP <1st-start> <1st-end> <2nd-start>\n\n"));
        } elif (allowable(TRUE))
        {   if (!arg2[0] || !arg3[0] || !arg4[0] || arg5[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_COMP].desc_id, menuinfo1[MENUITEM_COMP].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_CO, "Usage: CO|COMP <1st-start> <1st-end> <2nd-start>\n\n"));
            } else
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                address3 = parse_expression((STRPTR) arg4, MAX_ADDR, FALSE);
                if
                (   address1 >  address2
                 || address1 == OUTOFRANGE
                 || address2 == OUTOFRANGE
                 || address3 == OUTOFRANGE
                )
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL(
                            MSG_INVALIDRANGE,
                            "Valid range is $%X..$%X!\n\n"
                        ),
                        0,
                        MAX_ADDR
                    );
                } else
                {   j = 0;
                    for (i = 0; i <= address2 - address1; i++)
                    {   if (memory[address1 + i] != memory[address3 + i])
                        {   zprintf
                            (   TEXTPEN_CLIOUTPUT,
                                "$%04X/$%04X: $%02X vs. $%02X\n",
                                address1 + i,
                                address3 + i,
                                memory[address1 + i],
                                memory[address3 + i]
                            );
                            j++;
                    }   }
                    zprintf(TEXTPEN_CLIOUTPUT, "%d mismatch(es) found.\n\n", j);
    }   }   }   }
    elif (!stricmp((const char*) arg1, "COPY") || !stricmp((const char*) arg1, "MOVE"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_COPY].desc_id, menuinfo1[MENUITEM_COPY].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_COPY, "Usage: COPY|MOVE <source-start> <source-end> <target-start>\n\n"));
        } elif (allowable(TRUE))
        {   if (!arg2[0] || !arg3[0] || !arg4[0] || arg5[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_COPY].desc_id, menuinfo1[MENUITEM_COPY].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_COPY, "Usage: COPY|MOVE <source-start> <source-end> <target-start>\n\n"));
            } else
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                address3 = parse_expression((STRPTR) arg4, MAX_ADDR, FALSE);
                if
                (   address1 >  address2
                 || address1 == OUTOFRANGE
                 || address2 == OUTOFRANGE
                 || address3 == OUTOFRANGE
                )
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL(
                            MSG_INVALIDRANGE,
                            "Valid range is $%X..$%X!\n\n"
                        ),
                        0,
                        MAX_ADDR
                    );
                } else
                {   if (address1 < address3) // copying from lower to higher memory
                    {   for (i = address2 - address1; i >= 0; i--)
                        {   memory[address3 + i] = memory[address1 + i];
                    }   }
                    else // copying from higher to lower memory
                    {   for (i = 0; i <= address2 - address1; i++)
                        {   memory[address3 + i] = memory[address1 + i];
                    }   }
                    update_memory(FALSE);
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
    }   }   }   }
    elif (!stricmp((const char*) arg1, "COVER"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_COVER].desc_id, menuinfo1[MENUITEM_COVER].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_COVER, "Usage: COVER [<reporttype>]\n\n"));
        } elif (allowable(TRUE))
        {   if (arg3[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_COVER].desc_id, menuinfo1[MENUITEM_COVER].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_COVER, "Usage: COVER [<reporttype>]\n\n"));
            } elif (arg2[0])
            {   DISCARD stcd_l((const char*) arg2, (void*) &temp); // string -> decimal number
                if (temp >= 0 && temp <= 5)
                {   view_coverage(temp);
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_COVERERROR, "<reporttype> must be 0..5!\n"));
            }   }
            else
            {   view_coverage(0);
    }   }   }
    elif (!stricmp((const char*) arg1, "CPU"))
    {   if (ASKUSAGE || !arg2[0] || arg3[0])
        {   zprintf(TEXTPEN_CLIOUTPUT, "CPU 0: %s\n" \
                                       "CPU 1: %s\n",
                                       LLL(menuinfo2[MENUOPT_CPU_0].desc_id, menuinfo2[MENUOPT_CPU_0].desc_str),
                                       LLL(menuinfo2[MENUOPT_CPU_1].desc_id, menuinfo2[MENUOPT_CPU_1].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, "%s: CPU 0|1\n\n", LLL(MSG_USAGE, "Usage"));
        } else
        {   if (allowable(TRUE))
            {   if     (!strcmp((const char*) arg2, "0"))
                {   supercpu = 0;
                    updatemenu(MENUFAKE_CPU);
                    update_opcodes();
                    if (SubWindowPtr[SUBWINDOW_MONITOR_CPU])
                    {   close_subwindow(SUBWINDOW_MONITOR_CPU);
                        view_monitor(SUBWINDOW_MONITOR_CPU);
                    }
                    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_CPUISNOW, "CPU is now %s.\n\n"), "2650/2650A");
                } elif (!strcmp((const char*) arg2, "1"))
                {   supercpu = 1;
                    updatemenu(MENUFAKE_CPU);
                    update_opcodes();
                    if (SubWindowPtr[SUBWINDOW_MONITOR_CPU])
                    {   close_subwindow(SUBWINDOW_MONITOR_CPU);
                        view_monitor(SUBWINDOW_MONITOR_CPU);
                    }
                    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_CPUISNOW, "CPU is now %s.\n\n"), "2650B");
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, "CPU 0: %s\n" \
                                               "CPU 1: %s\n",
                                               LLL(menuinfo2[MENUOPT_CPU_0].desc_id, menuinfo2[MENUOPT_CPU_0].desc_str),
                                               LLL(menuinfo2[MENUOPT_CPU_1].desc_id, menuinfo2[MENUOPT_CPU_1].desc_str));
                    zprintf(TEXTPEN_CLIOUTPUT, "%s: CPU 0|1\n\n", LLL(MSG_USAGE, "Usage"));
    }   }   }   }
    elif (!stricmp((const char*) arg1, "D") || !stricmp((const char*) arg1, "PEEK") || !stricmp((const char*) arg1, "DM"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_PEEK].desc_id, menuinfo1[MENUITEM_PEEK].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_D, "Usage: D|PEEK <start-address> [<end-address>]\n\n"));
        } elif (allowable(TRUE))
        {   if (!arg2[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_PEEK].desc_id, menuinfo1[MENUITEM_PEEK].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_D, "Usage: D|PEEK <start-address> [<end-address>]\n\n"));
            } else
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                if (arg3[0])
                {   address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                } else
                {   address2 = address1 + 255;
                }
                if (address2 % 16 != 15) // round up
                {   address2 = ((address2 / 16) * 16) + 15;
                }
                if (check_range(address1, address2))
                {   for (i = address1; i <= address2; i += 16)
                    {   dumprow(i);
                    }
                    zprintf(TEXTPEN_CLIOUTPUT, "\n");
    }   }   }   }
    elif (!stricmp((const char*) arg1, "DEFSYM"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_DEFSYM].desc_id, menuinfo1[MENUITEM_DEFSYM].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_DEFSYM, "Usage: DEFSYM <label> <address> [C|D|P|U]\n\n"));
        } elif (allowable(TRUE))
        {   if (!arg2[0] || !arg3[0] || arg5[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_DEFSYM].desc_id, menuinfo1[MENUITEM_DEFSYM].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_DEFSYM, "Usage: DEFSYM <label> <address> [C|D|P|U]\n\n"));
            } else
            {   address1 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                if (arg4[0])
                {   tempchar = toupper(arg4[0]);
                    if (arg4[1] == EOS && (tempchar == 'C' || tempchar == 'D' || tempchar == 'P' || tempchar == 'U'))
                    {   adduserlabel((STRPTR) arg2, (UWORD) address1, tempchar);
                        zprintf(TEXTPEN_CLIOUTPUT, "\n");
                    } else
                    {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_DEFSYM].desc_id, menuinfo1[MENUITEM_DEFSYM].desc_str));
                        zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_DEFSYM, "Usage: DEFSYM <label> <address> [C|D|P|U]\n\n"));
                }   }
                else
                {   adduserlabel((STRPTR) arg2, (UWORD) address1, 'U');
                    zprintf(TEXTPEN_CLIOUTPUT, "\n");
    }   }   }   }
    elif (!stricmp((const char*) arg1, "DEL") || !stricmp((const char*) arg1, "DELETE"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_DELETE].desc_id, menuinfo1[MENUITEM_DELETE].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_DELETE, "Usage: DEL|DELETE <filename>\n\n"));
        } elif (!HASDISK)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
        } elif (!drive[debugdrive].inserted)
        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NODISK,       "No disk is inserted!\n\n"));
        } elif (!arg2[0] || arg3[0])
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_DELETE].desc_id, menuinfo1[MENUITEM_DELETE].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_DELETE, "Usage: DEL|DELETE <filename>\n\n"));
        } else
        {   dir_disk(TRUE, debugdrive);
            found2 = FALSE;
            switch (machine)
            {
            case BINBUG:
                for (i = 0; i < 100; i++)
                {   if (drive[0].filename[i][0] != EOS)
                    {   drive[0].filename[i][10] = EOS; // to avoid a spurious GCC warning
                        strcpy((char*) tempstring, (const char*) drive[0].filename[i]); // spurious GCC warning if we use sprintf() for this
                        strcat((char*) tempstring, ".");
                        strcat((char*) tempstring, (const char*) drive[0].fileext[i]);
                        if (!stricmp((const char*) tempstring, (const char*) arg2))
                        {   found2 = TRUE;
                            binbug_delete_file(i, debugdrive);
                            binbug_dir_disk(TRUE, debugdrive);
                            break; // for speed
                }   }   }
            acase CD2650:
                for (i = 0; i < 64; i++)
                {   if (drive[0].filename[i][0] != EOS)
                    {   drive[0].filename[i][8] = EOS; // to avoid a spurious GCC warning
                        strcpy((char*) tempstring, (const char*) drive[0].filename[i]); // spurious GCC warning if we use sprintf() for this
                        strcat((char*) tempstring, ".");
                        strcat((char*) tempstring, (const char*) drive[0].fileext[i]);
                        if (!stricmp((const char*) tempstring, (const char*) arg2))
                        {   found2 = TRUE;
                            cd2650_delete_file(i, debugdrive);
                            cd2650_dir_disk(TRUE, debugdrive);
                            break; // for speed
                }   }   }
            acase TWIN:
                for (i = 0; i < 78; i++)
                {   if (drive[debugdrive].filename[i][0] != EOS)
                    {   sprintf((char*) tempstring, "%s", drive[debugdrive].filename[i]);
                        if (!stricmp((const char*) tempstring, (const char*) arg2))
                        {   found2 = TRUE;
                            twin_delete_file(i, debugdrive);
                            twin_dir_disk(TRUE, debugdrive);
                            break; // for speed
            }   }   }   }
            if (found2)
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
            } else
            {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
    }   }   }
    elif (!stricmp((const char*) arg1, "DIR") || !stricmp((const char*) arg1, "LD") || !stricmp((const char*) arg1, "LDIR"))
    {   if (!HASDISK)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
        } elif (!drive[debugdrive].inserted)
        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NODISK, "No disk is inserted!\n\n"));
        } else
        {   dir_disk(FALSE, debugdrive);
    }   }
    elif (!stricmp((const char*) arg1, "DIS") || !stricmp((const char*) arg1, "DI"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_DIS].desc_id, menuinfo1[MENUITEM_DIS].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_DIS, "Usage: DIS [<start-address> [<end-address>]]\n\n"));
        } elif (allowable(TRUE))
        {   if (arg2[0])
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
            } else
            {   address1 = nextdis;
            }
            if (arg3[0])
            {   address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
            } else
            {   if (verbosity == 2)
                {   address2 = address1 + 11 - 1;
                } else
                {   address2 = address1 + 23 - 1;
// up to 23 lines of disassembly (to allow for blank line and prompt),
//  plus labels (up to 23 more lines)
// (IBM-PC console has 25 rows by 80 columns)
            }   }
            if (check_range(address1, address2))
            {   disassembling = 1; // means DIS command
                disassemble_2650(address1, address2, FALSE);
                disassembling = 0; // means tracing (or nothing)
                zprintf(TEXTPEN_CLIOUTPUT, "\n");
    }   }   }
    elif (!stricmp((const char*) arg1, "DISGAME"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_DISGAME].desc_id, menuinfo1[MENUITEM_DISGAME].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_DISGAME, "Usage: DISGAME [<start-address> <end-address>] [<filename>]\n\n"));
        } elif (allowable(TRUE))
        {   /* The legal possibilities are:
               DISGAME
               DISGAME <filename>
               DISGAME <start-address> <end-address>
               DISGAME <start-address> <end-address> <filename> */
            if (arg3[0])
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                if (check_range(address1, address2))
                {   if (!arg4[0])
                    {   strcpy((char*) arg4, (const char*) file_game);
                    }
                    disgame(address1, address2, (STRPTR) arg4);
            }   }
            else
            {   if (!arg2[0])
                {   strcpy((char*) arg2, (const char*) file_game);
                }
                disgame(0, 32767, (STRPTR) arg2);
    }   }   }
    elif (!stricmp((const char*) arg1, "DL"))
    {   if (allowable(TRUE) && !crippled)
        {   project_open();
    }   }
    elif (!stricmp((const char*) arg1, "DRAW")) // DM is already used (for Display Memory) so we can't use it for eg. "draw mode"
    {   if
        (   ASKUSAGE
         || (allowable(TRUE) && (arg2[0] < '0' || arg2[0] > '4' || arg2[1] || arg3[0]))
        )
        {   zprintf(TEXTPEN_CLIOUTPUT, "DRAW 0: %s\n" \
                                       "DRAW 1: %s\n" \
                                       "DRAW 2: %s\n" \
                                       "DRAW 3: %s\n" \
                                       "DRAW 4: %s\n",
                                       LLL(menuinfo2[MENUOPT_DRAW_0].desc_id, menuinfo2[MENUOPT_DRAW_0].desc_str),
                                       LLL(menuinfo2[MENUOPT_DRAW_1].desc_id, menuinfo2[MENUOPT_DRAW_1].desc_str),
                                       LLL(menuinfo2[MENUOPT_DRAW_2].desc_id, menuinfo2[MENUOPT_DRAW_2].desc_str),
                                       LLL(menuinfo2[MENUOPT_DRAW_3].desc_id, menuinfo2[MENUOPT_DRAW_3].desc_str),
                                       LLL(menuinfo2[MENUOPT_DRAW_4].desc_id, menuinfo2[MENUOPT_DRAW_4].desc_str)
                   );
            zprintf(TEXTPEN_CLIOUTPUT, "Usage: DRAW 0|1|2|3|4\n\n");
        } elif
        (   machine == INSTRUCTOR
         || machine == MIKIT
         || machine == PONG
         || machine == TYPERIGHT
        )
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
        } elif
        (   (   machine == PIPBUG
             || machine == BINBUG
             || machine == TWIN
             || machine == CD2650
             || machine == ZACCARIA
             || machine == PHUNSY
             || machine == SELBST
            )
         && arg2[0] > '1'
        )
        {   zprintf(TEXTPEN_CLIOUTPUT, "Allowable range is 0..1!\n\n");
        } elif
        (   (   machine == ARCADIA
             || machine == MALZAK
            )
         && arg2[0] > '3'
        )
        {   zprintf(TEXTPEN_CLIOUTPUT, "Allowable range is 0..3!\n\n");
        } else
        {   drawmode = arg2[0] - '0';
            draw_margins();
            redrawscreen();
            zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
    }   }
    elif (!stricmp((const char*) arg1, "DOKE"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_DOKE].desc_id, menuinfo1[MENUITEM_DOKE].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_DOKE, "Usage: DOKE <address> [<value>]\n\n"));
        } elif (allowable(TRUE))
        {   if (arg2[0] && arg3[0])
            {   if (poke_start((STRPTR) arg2, FALSE, TRUE))
                {   tonum = parse_expression((STRPTR) arg3, tolimit, FALSE);
                    poke_end(FALSE, TRUE);
            }   }
            elif (arg2[0] && (arg2[0] != '?' || arg2[1] != EOS))
            {   DISCARD debug_edit((STRPTR) arg2, FALSE, TRUE);
            } else
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_DOKE].desc_id, menuinfo1[MENUITEM_DOKE].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_DOKE, "Usage: DOKE <address> [<value>]\n\n"));
    }   }   }
    elif (!stricmp((const char*) arg1, "DR")) // Display Registers?
    {   if (allowable(TRUE))
        {   view_cpu_2650(TRUE);
    }   }
    elif (!stricmp((const char*) arg1, "DRIVE"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "DRIVE 0: %s\n" \
                                       "DRIVE 1: %s\n",
                                       "DRIVE 2: %s\n",
                                       "DRIVE 3: %s\n",
                                       LLL(menuinfo2[MENUOPT_DRIVE_0].desc_id, menuinfo2[MENUOPT_DRIVE_0].desc_str),
                                       LLL(menuinfo2[MENUOPT_DRIVE_1].desc_id, menuinfo2[MENUOPT_DRIVE_1].desc_str),
                                       LLL(menuinfo2[MENUOPT_DRIVE_2].desc_id, menuinfo2[MENUOPT_DRIVE_2].desc_str),
                                       LLL(menuinfo2[MENUOPT_DRIVE_3].desc_id, menuinfo2[MENUOPT_DRIVE_3].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_DRIVE, "Usage: DRIVE [<drive>]\n\n"));
        } elif (arg2[0])
        {   DISCARD stcd_l((const char*) arg2, (void*) &temp);
            if (temp < 0 || temp >= machines[machine].drives)
            {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_INVALIDDRIVENUMBER, "Invalid drive number!\n\n"));
            } else
            {   debugdrive = temp;
                zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
        }   }
        else
        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_CURRENTDRIVEIS, "Current drive is #%d.\n\n"), debugdrive);
            if (verbosedisk && (machine == BINBUG || machine == CD2650))
            {   zprintf
                (   TEXTPEN_VERBOSE,
                    "Track register: $%02X\n" \
                    "Desired sector: $%02X\n" \
                    "Data register:  $%02X\n" \
                    "Step direction: %d\n" \
                    "Multi sectors?  %s\n\n",
                    trackreg,
                    reqsector,
                    drivedata,
                    stepdir,
                    multimode ? "Yes" : "No"
                );
    }   }   }
    elif (!stricmp((const char*) arg1, "E") || !stricmp((const char*) arg1, "POKE") || !stricmp((const char*) arg1, "PM") || !stricmp((const char*) arg1, "PR"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_POKE].desc_id, menuinfo1[MENUITEM_POKE].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_E, "Usage: E|POKE [<address> [<value>]]\n\n"));
        } elif (allowable(TRUE))
        {   if (arg2[0] && arg3[0]) // E|POKE <address> <value>
            {   if (poke_start((STRPTR) arg2, FALSE, FALSE))
                {   tonum = parse_expression((STRPTR) arg3, tolimit, FALSE);
                    poke_end(FALSE, FALSE);
            }   }
            elif (arg2[0]) // E|POKE <address>
            {   DISCARD debug_edit((STRPTR) arg2, FALSE, FALSE);
            } else // E|POKE
            {   if (SubWindowPtr[SUBWINDOW_MEMORY])
                {
#ifdef WIN32
                    SetWindowPos(SubWindowPtr[SUBWINDOW_MEMORY], HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
#endif
#ifdef AMIGA
                    WindowToFront(SubWindowPtr[SUBWINDOW_MEMORY]);
#endif
                } else
                {   edit_memory();
                }
                zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
    }   }   }
    elif (!stricmp((const char*) arg1, "ED") || !stricmp((const char*) arg1, "EDIT"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_EDIT].desc_id, menuinfo1[MENUITEM_EDIT].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_ED, "Usage: ED|EDIT <filename>\n\n"));
        } elif (arg2[0])
        {   if (!Exists((STRPTR) arg2))
            {   fixextension(filekind[KIND_ASM].extension, (STRPTR) arg2, FALSE, "");
                // we should check whether it exists, and if it doesn't, we should tell the user so
            }
            if (strlen((const char*) arg2) > MAX_PATH)
            {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_PATHNAMEISTOOLONG, "Pathname is too long!\n\n"));
            } else
            {
                #ifdef AMIGA
                    #ifdef __MORPHOS__
                        arg2[MAX_PATH] = EOS; // to avoid a spurious GCC warning
                        strcpy((char*) tempstring, "MOSSYS:C/Ed "); // spurious GCC warning if we use sprintf() for this
                        strcat((char*) tempstring, (const char*) arg2);
                    #else
                        #ifdef __amigaos4__
                             sprintf(tempstring, "SYS:Utilities/MEmacs %s", arg2);
                        #else
                             if (wbver <= 45) sprintf(tempstring, "SYS:Tools/EditPad %s" , arg2);
                             if (wbver == 46) sprintf(tempstring, "SYS:C/Ed %s"          , arg2);
                             else             sprintf(tempstring, "SYS:Tools/TextEdit %s", arg2);
                        #endif
                    #endif
                    DISCARD SystemTags
                    (   (const char*) tempstring,
                        SYS_Asynch, TRUE,
                        SYS_Input,  NULL,
                        SYS_Output, NULL,
                    TAG_DONE);
                #endif
                #ifdef WIN32
                    DISCARD ShellExecute
                    (   NULL,
                        "open",
                        "NotePad.exe", // it's in the path, so no need to specify the directory
                        arg2,
                        NULL, // default dir (don't care)
                        SW_SHOWNORMAL
                    );
                #endif
        }   }
        else
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_EDIT].desc_id, menuinfo1[MENUITEM_EDIT].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_ED, "Usage: ED|EDIT <filename>\n\n"));
    }   }
    elif (!stricmp((const char*) arg1, "ERROR"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_ERROR].desc_id, menuinfo1[MENUITEM_ERROR].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_ERROR, "Usage: ERROR <number>\n\n"));
        } elif (!((machine == CD2650 && cd2650_biosver == CD2650_IPL && cd2650_dosver == DOS_P1DOS) || machine == TWIN))
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
        } elif (!arg2[0])
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_ERROR].desc_id, menuinfo1[MENUITEM_ERROR].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_ERROR, "Usage: ERROR <number>\n\n"));
        } else
        {   DISCARD stcd_l((const char*) arg2, (void*) &temp); // string -> decimal number
            if (temp < 1 || temp > 69)
            {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NUMBERMUSTBE, "<number> must be %d-%d!\n\n"), 1, 69);
            } else
            {   switch (machine)
                {
                case  CD2650: if (temp < 1 || temp > 18)
                              {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NUMBERMUSTBE, "<number> must be %d-%d!\n\n"), 1, 18);
                              } else
                              {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", p1dos_error[temp]);
                              }
                acase TWIN:   if (temp < 1 || temp > 69)
                              {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NUMBERMUSTBE, "<number> must be %d-%d!\n\n"), 1, 69);
                              } else
                              {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", twin_error[temp]);
    }   }   }   }             }
    elif (!stricmp((const char*) arg1, "EXTRACT"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_EXTRACT].desc_id, menuinfo1[MENUITEM_EXTRACT].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_EXTRACT, "Usage: EXTRACT [<filename>]\n\n"));
        } elif (!HASDISK)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
        } elif (!drive[debugdrive].inserted)
        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NODISK, "No disk is inserted!\n\n"));
        } else
        {   dir_disk(TRUE, debugdrive);
            if (arg2[0])
            {   found2 = FALSE;
                switch (machine)
                {
                case BINBUG:
                    for (i = 0; i < 100; i++)
                    {   if (drive[0].filename[i][0] != EOS)
                        {   drive[0].filename[i][10] = EOS; // to avoid a spurious GCC warning
                            strcpy((char*) tempstring, (const char*) drive[0].filename[i]); // spurious GCC warning if we use sprintf() for this
                            strcat((char*) tempstring, ".");
                            strcat((char*) tempstring, (const char*) drive[0].fileext[i]);
                            if (!stricmp((const char*) tempstring, (const char*) arg2))
                            {   cd_projects();
                                if (binbug_extract_file(i))
                                {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                                } else
                                {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                                }
                                cd_progdir();
                                found2 = TRUE;
                                break; // for speed
                    }   }   }
                acase CD2650:
                    for (i = 0; i < 64; i++)
                    {   if (drive[0].filename[i][0] != EOS)
                        {   drive[0].filename[i][8] = EOS; // to avoid a spurious GCC warning
                            strcpy((char*) tempstring, (const char*) drive[0].filename[i]); // spurious GCC warning if we use sprintf() for this
                            strcat((char*) tempstring, ".");
                            strcat((char*) tempstring, (const char*) drive[0].fileext[i]);
                            if (!stricmp((const char*) tempstring, (const char*) arg2))
                            {   cd_projects();
                                if (cd2650_extract_file(i))
                                {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                                } else
                                {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                                }
                                cd_progdir();
                                found2 = TRUE;
                                break; // for speed
                    }   }   }
                acase TWIN:
                    for (i = 0; i < 78; i++)
                    {   if (drive[debugdrive].filename[i][0] != EOS)
                        {   sprintf((char*) tempstring, "%s", drive[debugdrive].filename[i]);
                            if (!stricmp((const char*) tempstring, (const char*) arg2))
                            {   cd_projects();
                                if (twin_extract_file(i, debugdrive))
                                {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                                } else
                                {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                                }
                                cd_progdir();
                                found2 = TRUE;
                                break; // for speed
                }   }   }   }
                if (!found2)
                {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
            }   }
            else
            {   ok = TRUE;
                cd_projects();
                switch (machine)
                {
                case BINBUG:
                    for (i = 0; i < 100; i++)
                    {   if (drive[0].filename[i][0] != EOS && !binbug_extract_file(i))
                        {   ok = FALSE;
                    }   }
                acase CD2650:
                    for (i = 0; i < 64; i++)
                    {   if (drive[0].filename[i][0] != EOS && !cd2650_extract_file(i))
                        {   ok = FALSE;
                    }   }
                acase TWIN:
                    for (i = 0; i < 78; i++)
                    {   if (drive[debugdrive].filename[i][0] != EOS && !twin_extract_file(i, debugdrive))
                        {   ok = FALSE;
                }   }   }
                cd_progdir();
                if (ok)
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
    }   }   }   }
    elif (!stricmp((const char*) arg1, "FILL"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_FILL].desc_id, menuinfo1[MENUITEM_FILL].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_F, "Usage: FILL <start-address> <end-address> <value>\n\n"));
        } elif (allowable(TRUE))
        {   if (arg2[0])
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                if (check_range(address1, address2))
                {   tonum = parse_expression((STRPTR) arg4, 255, FALSE);
                    if (tonum >= -128 && tonum <= 255)
                    {   for (i = address1; i <= address2; i++)
                        {   memory[i] = (UBYTE) tonum;
                        }
                        update_memory(FALSE);
                        zprintf
                        (   TEXTPEN_CLIOUTPUT,
                            LLL(
                                MSG_FILLED,
                                "Filled $%X..$%X with $%X.\n\n"
                            ),
                            address1,
                            address2,
                            tonum
                        );
                    } else
                    {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_FILL].desc_id, menuinfo1[MENUITEM_FILL].desc_str));
                        zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_F, "Usage: FILL <start-address> <end-address> <value>\n\n"));
            }   }   }
            else
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_FILL].desc_id, menuinfo1[MENUITEM_FILL].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_F, "Usage: FILL <start-address> <end-address> <value>\n\n"));
    }   }   }
    elif (!stricmp((const char*) arg1, "FIND"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_FIND].desc_id, menuinfo1[MENUITEM_FIND].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_FIND, "Usage: FIND [[<start-address> <end-address>] <value>]\n\n"));
        } elif (allowable(TRUE))
        {   if ((arg3[0] && !arg4[0]) || arg5[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_FIND].desc_id, menuinfo1[MENUITEM_FIND].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_FIND, "Usage: FIND [[<start-address> <end-address>] <value>]\n\n"));
            } elif (arg2[0])
            {   if (arg4[0]) // FIND <start-address> <end-address> <value>
                {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                    address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                    if (check_range(address1, address2))
                    {   strcpy((char*) thearg, (const char*) arg4);
                    } else
                    {   thearg[0] = EOS;
                }   }
                else // FIND <value>
                {   address1 = 0;
                    address2 = MAX_ADDR;
                    strcpy((char*) thearg, (const char*) arg2);
                }

                if (thearg[0] == '"')
                {   if (machine == MIKIT)
                    {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
                    } else
                    {   length = strlen((const char*) thearg);
                        if (length >= 2 && thearg[length - 1] == '"')
                        {   thearg[--length] = EOS;
                        }
                        if (length >= 2)
                        {   for (i = 1; i < length; i++)
                            {   switch (machine)
                                {
                                case ARCADIA:
                                    for (j = 0; j < 64; j++)
                                    {   if (thearg[i] == arcadia_chars[j])
                                        {   tempstring[i - 1] = j;
                                            break; // for speed
                                    }   }
                                acase ELEKTOR:
                                    for (j = 0; j < 256; j++)
                                    {   if (thearg[i] == elektor_chars1[j])
                                        {   tempstring[i - 1] = j;
                                            break; // for speed
                                    }   }
                                acase PIPBUG:
                                case BINBUG:
                                case TWIN:
                                case SELBST:
                                case PHUNSY:
                                    tempstring[i - 1] = thearg[i];
                                acase CD2650:
                                    if (cd2650_vdu == VDU_ASCII || (whichgame != -1 && known[whichgame].complang == CL_8KB13))
                                    {   tempstring[i - 1] = EOS;
                                        for (j = 32; j < 128; j++)
                                        {   if (thearg[i] == cd2650_chars[j])
                                            {   tempstring[i - 1] = j;
                                                break; // for speed
                                        }   }
                                        // we do it this way so that uppercase characters are matched to their ASCII equivalents
                                        if (tempstring[i - 1] == EOS)
                                        {   for (j = 0; j < 32; j++)
                                            {   if (thearg[i] == cd2650_chars[j])
                                                {   tempstring[i - 1] = j;
                                                    break; // for speed
                                    }   }   }   }
                                    else
                                    {   for (j = 0; j < 128; j++)
                                        {   if (thearg[i] == cd2650_chars[j])
                                            {   tempstring[i - 1] = j;
                                                break; // for speed
                                    }   }   }
                                acase INSTRUCTOR:
                                    for (j = 0; j < 256; j++)
                                    {   if (thearg[i] == instructor_chars1[j])
                                        {   tempstring[i - 1] = j;
                                            break; // for speed
                                    }   }
                                acase MALZAK:
                                    tempstring[i - 1] = thearg[i] & 0x7F;
                                acase ZACCARIA:
                                    switch (memmap)
                                    {
                                    case MEMMAP_ASTROWARS:
                                        for (j = 0; j < 256; j++)
                                        {   if (thearg[i] == astrowars_chars[j])
                                            {   tempstring[i - 1] = j;
                                                break; // for speed
                                        }   }
                                    acase MEMMAP_GALAXIA:
                                        for (j = 0; j < 128; j++)
                                        {   if (thearg[i] == galaxia_chars[j])
                                            {   tempstring[i - 1] = j;
                                                break; // for speed
                                        }   }
                                    acase MEMMAP_LASERBATTLE:
                                    case MEMMAP_LAZARIAN:
                                        for (j = 0; j < 256; j++)
                                        {   if (thearg[i] == lb_chars[j])
                                            {   tempstring[i - 1] = j;
                                                break; // for speed
                            }   }   }   }   }
                            tempstring[i - 1] = EOS;
                            length--;
                            // assert(length >= 1);
                            for (i = address1; i <= address2 - length + 1; i++)
                            {   found2 = TRUE;
                                for (j = 0; j < length; j++)
                                {   t = memory[i + j];
                                    if   (machine == ARCADIA                                                 ) t &= 0x3F;
                                    elif (machine == CD2650  || machine == MALZAK || memmap == MEMMAP_GALAXIA) t &= 0x7F;
                                    if (t != tempstring[j])
                                    {   found2 = FALSE;
                                        break; // for speed
                                }   }
                                if (found2)
                                {   DISCARD getfriendly(i);
                                    zprintf(TEXTPEN_CLIOUTPUT, "%s\n", friendly);
                            }   }
                            zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                }   }   }
                elif (thearg[0] != EOS)
                {   tonum = parse_expression((STRPTR) thearg, 255, FALSE);
                    found2 = FALSE;
                    for (i = address1; i <= address2; i++)
                    {   if (memory[i] == tonum)
                        {   found2 = TRUE;
                            DISCARD getfriendly(i);
                            zprintf(TEXTPEN_CLIOUTPUT, "%s\n", friendly);
                    }   }
                    if (!found2)
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NOTHINGFOUND, "Nothing found.\n"));
                    }
                    zprintf(TEXTPEN_CLIOUTPUT, "\n");
            }   }
            else // FIND
            {   if (machine == MIKIT)
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
                } else
                {   FLAG on, oldon;
                    int  startpoint = 0; // initialized to avoid compiler warning
                    on = FALSE;
                    for (i = 0; i <= MAX_ADDR; i++)
                    {   oldon = on;
                        t = memory[i];
                        if   (machine == ARCADIA                                                 ) t &= 0x3F;
                        elif (machine == CD2650  || machine == MALZAK || memmap == MEMMAP_GALAXIA) t &= 0x7F;
                        if
                        (   ( machine == ARCADIA                                                                                      &&   t >= 0x1A && t <= 0x33                                   )
                         || ( machine == ELEKTOR                                                                                      &&   t >= 0x0A && t <= 0x15                                   )
                         || ( machine == INSTRUCTOR                                                                                   && ((t >= 0x0A && t <= 0x15) ||  t == 0x00                   ))
                         || ( machine == CD2650                                                                                       && ((t >= 1    && t <= 26  ) || (t >= 64 + 1 && t <= 64 + 26)))
                         || ((machine == PIPBUG || machine == BINBUG || machine == TWIN || machine == SELBST || machine == PHUNSY || machine == MALZAK) && ((t >= 'A'  && t <= 'Z' ) || (t >= 'a'    && t <= 'z'    )))
                         || ((memmap  == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN                                            ) &&   t >= 0xC0 && t <= 0xD9                                   )
                         || ( memmap  == MEMMAP_GALAXIA                                                                               &&   t >= 0x41 && t <= 0x59                                   )
                         || ( memmap  == MEMMAP_ASTROWARS                                                                             &&   t >= 0xC1 && t <= 0xDA                                   )
                        )
                        {   on = TRUE;
                            if (!oldon)
                            {   startpoint = i;
                        }   }
                        else
                        {   on = FALSE;
                            if (oldon && i - startpoint >= MINLENGTH) // we just finished a string
                            {   showfound(startpoint, i - 1);
                    }   }   }
                    if (on && 32768 - startpoint >= MINLENGTH)
                    {   showfound(startpoint, 32767);
                    }
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
    }   }   }   }
    elif (!stricmp((const char*) arg1, "FC"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_FC].desc_id, menuinfo1[MENUITEM_FC].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_FC, "Usage: FC [<start-address> [<end-address>]]\n\n"));
        } elif (HASDISK)
        {   if (arg3[0]) // FC <start-address> <end-address>
            {   address1 = readbase((STRPTR) arg2, FALSE);
                address2 = readbase((STRPTR) arg3, FALSE);
                if
                (   address1 >= 0
                 && address1 <  machines[machine].disksize
                 && address2 >= 0
                 && address2 <  machines[machine].disksize
                 && address1 <= address2
                )
                {   for (i = address1; i <= address2; i++)
                    {   drive[debugdrive].flags[i / 8] &= ~(1 << (i % 8));
                    }
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                    update_floppydrive(2, debugdrive);
                } else
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL
                        (   MSG_INVALIDRANGE,
                            "Valid range is $%X..$%X!\n\n"
                        ),
                        0,
                        machines[machine].disksize - 1
                    );
            }   }
            elif (arg2[0]) // FC <start-address>
            {   address1 = readbase((STRPTR) arg2, FALSE);
                if
                (   address1 >= 0
                 && address1 <  machines[machine].disksize
                )
                {   sprintf((char*) tempstring, "$%X", address1);
                    if (drive[debugdrive].flags[address1 / 8] & (1 << (address1 % 8)))
                    {   drive[debugdrive].flags[address1 / 8] &= ~(1 << (address1 % 8));
                        zprintf
                        (   TEXTPEN_CLIOUTPUT,
                            LLL
                            (   MSG_ENGINE_CLEAREDWP,
                                "Cleared data watchpoint at %s.\n\n"
                            ),
                            tempstring
                        );
                        update_floppydrive(2, debugdrive);
                    } else
                    {   zprintf
                        (   TEXTPEN_CLIOUTPUT,
                            LLL
                            (   MSG_ENGINE_WPNOTFOUND,
                                "Watchpoint not found at %s!\n\n"
                            ),
                            tempstring
                        );
                }   }
                else
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL
                        (   MSG_INVALIDRANGE,
                            "Valid range is $%X..$%X!\n\n"
                        ),
                        0,
                        machines[machine].disksize - 1
                    );
            }   }
            else // FC
            {   ok = FALSE;
                for (address1 = 0; address1 < machines[machine].disksize; address1++)
                {   if (drive[debugdrive].flags[address1 / 8] & (1 << (address1 % 8)))
                    {   drive[debugdrive].flags[address1 / 8] &= ~(1 << (address1 % 8));
                        ok = TRUE;
                }   }
                if (ok)
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL
                        (   MSG_ENGINE_CLEAREDWPS,
                            "Cleared all data watchpoints.\n\n"
                        )
                    );
                    update_floppydrive(2, debugdrive);
                } else
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL
                        (   MSG_NOWPS,
                            "No watchpoints to clear!\n\n"
                        )
                    );
        }   }   }
        else
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
    }   }
    elif (!stricmp((const char*) arg1, "FL"))
    {   if (HASDISK)
        {   ok = FALSE;
            for (address1 = 0; address1 < machines[machine].disksize; address1++)
            {   if (drive[debugdrive].flags[address1 / 8] & (1 << (address1 % 8)))
                {   zprintf(TEXTPEN_CLIOUTPUT, "$%X\n", address1);
                    ok = TRUE;
            }   }
            if (ok)
            {   zprintf(TEXTPEN_CLIOUTPUT, "\n");
            } else
            {   zprintf
                (   TEXTPEN_CLIOUTPUT,
                    "%s.\n\n",
                    LLL
                    (   MSG_NONE,
                        "None"
                )   );
        }   }
        else
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
    }   }
    elif (!stricmp((const char*) arg1, "FP"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_FP].desc_id, menuinfo1[MENUITEM_FP].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_FP, "Usage: FP <start-address> [<end-address>]\n\n"));
        } elif (HASDISK)
        {   if (arg3[0]) // FP <start-address> <end-address>
            {   address1 = readbase((STRPTR) arg2, FALSE);
                address2 = readbase((STRPTR) arg3, FALSE);
                if
                (   address1 >= 0
                 && address1 <  machines[machine].disksize
                 && address2 >= 0
                 && address2 <  machines[machine].disksize
                 && address1 <= address2
                )
                {   for (i = address1; i <= address2; i++)
                    {   drive[debugdrive].flags[i / 8] |= (1 << (i % 8));
                    }
                    if (!watchreads && !watchwrites)
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_WPWARNING, "Warning: watching of reads and writes is currently turned off! (WR and WW to enable.)\n"));
                    }
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                    update_floppydrive(2, debugdrive);
                } else
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL
                        (   MSG_INVALIDRANGE,
                            "Valid range is $%X..$%X!\n\n"
                        ),
                        0,
                        machines[machine].disksize - 1
                    );
            }   }
            elif (arg2[0]) // FP <addr>
            {   address1 = readbase((STRPTR) arg2, FALSE);
                if
                (   address1 >= 0
                 && address1 <  machines[machine].disksize
                )
                {   if (drive[debugdrive].flags[address1 / 8] & (1 << (address1 % 8)))
                    {   zprintf
                        (   TEXTPEN_CLIOUTPUT,
                            LLL(
                               MSG_WPALREADYSET,
                               "Watchpoint is already set!\n\n"
                        )   );
                    } else
                    {   drive[debugdrive].flags[address1 / 8] |= (1 << (address1 % 8));
                        update_floppydrive(2, debugdrive);
                        sprintf((char*) tempstring, "$%X", address1);
                        zprintf
                        (   TEXTPEN_CLIOUTPUT,
                            LLL
                            (   MSG_ADDWP,
                                "Added data watchpoint at %s.\n\n"
                            ),
                            tempstring
                        );
                        if (!watchreads && !watchwrites)
                        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_WPWARNING, "Warning: watching of reads and writes is currently turned off! (WR and WW to enable.)\n"));
                }   }   }
                else
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL
                        (   MSG_INVALIDRANGE,
                            "Valid range is $%X..$%X!\n\n"
                        ),
                        0,
                        machines[machine].disksize - 1
                    );
            }   }
            else
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_FP].desc_id, menuinfo1[MENUITEM_FP].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_FP, "Usage: FP <start-address> [<end-address>]\n\n"));
        }   }
        else
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
    }   }
    elif (!stricmp((const char*) arg1, "FPEEK"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_FPEEK].desc_id, menuinfo1[MENUITEM_FPEEK].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_FPEEK, "Usage: FPEEK <startaddr> [<endaddr>]\n\n"));
        } elif (!HASDISK)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
        } elif (!drive[debugdrive].inserted)
        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NODISK, "No disk is inserted!\n\n"));
        } elif (!arg2[0])
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_FPEEK].desc_id, menuinfo1[MENUITEM_FPEEK].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_FPEEK, "Usage: FPEEK <startaddr> [<endaddr>]\n\n"));
        } else
        {   address1 = readbase((STRPTR) arg2, FALSE);
            if (arg3[0])
            {   address2 = readbase((STRPTR) arg3, FALSE);
            } else
            {   address2 = address1 + 255;
            }
            if (address2 % 16 != 15) // round up
            {   address2 = ((address2 / 16) * 16) + 15;
            }
            if
            (   address1 >  address2
             || address1 >= machines[machine].disksize
             || address2 >= machines[machine].disksize
            )
            {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_INVALIDRANGE, "Valid range is $%X..$%X!\n\n"), 0, machines[machine].disksize - 1);
            } else
            {   for (i = address1; i <= address2; i += 16)
                {   for (j = 0; j < 16; j++)
                    {   rowchars[j] = guestchar(drive[debugdrive].contents[i + j]);
                    }
                    rowchars[16] = EOS;
                    zprintf
                    (   TEXTPEN_VIEW,
                        "$%05X: %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X %02X %02X %s\n",
                        i,
                        drive[debugdrive].contents[i     ],
                        drive[debugdrive].contents[i +  1],
                        drive[debugdrive].contents[i +  2],
                        drive[debugdrive].contents[i +  3],
                        drive[debugdrive].contents[i +  4],
                        drive[debugdrive].contents[i +  5],
                        drive[debugdrive].contents[i +  6],
                        drive[debugdrive].contents[i +  7],
                        drive[debugdrive].contents[i +  8],
                        drive[debugdrive].contents[i +  9],
                        drive[debugdrive].contents[i + 10],
                        drive[debugdrive].contents[i + 11],
                        drive[debugdrive].contents[i + 12],
                        drive[debugdrive].contents[i + 13],
                        drive[debugdrive].contents[i + 14],
                        drive[debugdrive].contents[i + 15],
                        rowchars
                    );
                }
                zprintf(TEXTPEN_CLIOUTPUT, "\n");
    }   }   }
    elif (!stricmp((const char*) arg1, "FPOKE"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_FPOKE].desc_id, menuinfo1[MENUITEM_FPOKE].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_FPOKE, "Usage: FPOKE <address> <value>\n\n"));
        } elif (!HASDISK)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
        } elif (!drive[debugdrive].inserted)
        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NODISK, "No disk is inserted!\n\n"));
        } elif (!arg3[0])
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_FPOKE].desc_id, menuinfo1[MENUITEM_FPOKE].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_FPOKE, "Usage: FPOKE <address> <value>\n\n"));
        } else
        {   address1 = readbase((STRPTR) arg2, FALSE);
            address2 = readbase((STRPTR) arg3, FALSE);
            if (address1 >= machines[machine].disksize)
            {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_INVALIDRANGE, "Valid range is $%X..$%X!\n\n"), 0, machines[machine].disksize - 1);
            } elif (address2 > 255)
            {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_ENGINE_OUTOFBOUNDS, "Value is out of bounds (limit is %d)!\n\n"), 255);
            } else
            {   drive[debugdrive].contents[address1] = address2;
                zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                update_floppydrive(2, debugdrive);
    }   }   }
    elif (!stricmp((const char*) arg1, "G") || !stricmp((const char*) arg1, "P") || !stricmp((const char*) arg1, "GO"))
    {   if (ASKUSAGE || arg3[0])
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_G].desc_id, menuinfo1[MENUITEM_G].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_G, "Usage: G|P [<address>]\n\n"));
        } elif (arg2[0])
        {   if (paused)
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                DISCARD bp_add(address1, 0, COND_UN, 0);
                emu_unpause();
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_ENGINE_GOING, "Unpausing...\n\n"));
            } // else print an error message?
        } else
        {   if (paused)
            {   emu_unpause();
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_ENGINE_GOING,   "Unpausing...\n\n"));
            } else
            {   emu_pause();
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_ENGINE_PAUSING, "Pausing...\n\n"));
    }   }   }
    elif (!stricmp((const char*) arg1, "GI"))
    {   if (machines[machine].pvis == 0 && machine != INSTRUCTOR && machine != PHUNSY)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
        } else
        {   interrupt_2650 = TRUE;
            if (psu & PSU_II)
            {   zprintf
                (   TEXTPEN_CLIOUTPUT,
                    LLL(
                        MSG_ENGINE_PENDED,
                        "Queued user-requested interrupt.\n\n"
                    )
                );
            } else
            {   zprintf
                (   TEXTPEN_CLIOUTPUT,
                    LLL
                    (   MSG_ENGINE_GENERATED,
                        "Generated user-requested interrupt.\n\n"
                    )
                );
            }
            checkinterrupt();
    }   }
    elif (!stricmp((const char*) arg1, "GR"))
    {   if
        (   machine == ARCADIA
         || machines[machine].pvis
         || machine == BINBUG
         || machine == CD2650
         || machine == PHUNSY
        )
        {   zprintf
            (   TEXTPEN_CLIOUTPUT,
                LLL(
                    MSG_DEBUG_USEGUIDEPIXEL,
                    "Use guide pixel"
            )   );
            fliplog(&useguideray);
            if (useguideray)
            {   if (paused)
                {   draw_guide_ray(FALSE);
            }   }
            else
            {   draw_guide_ray(TRUE);
            }
            updatescreen();
        } else // eg. PIPBUG, BINBUG, INSTRUCTOR, SELBST, MIKIT, PONG, TYPERIGHT
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
    }   }
    elif
    (   !stricmp((const char*) arg1, "H"   )
     || !stricmp((const char*) arg1, "HELP")
     || !stricmp((const char*) arg1, "?"   )
    )
    {   if   (!strcmp(arg2,  "1") || !stricmp(arg2, "F")) help_commands( 1);
        elif (!strcmp(arg2,  "2") || !stricmp(arg2, "D")) help_commands( 2);
        elif (!strcmp(arg2,  "3") || !stricmp(arg2, "E")) help_commands( 3);
        elif (!strcmp(arg2,  "4") || !stricmp(arg2, "V")) help_commands( 4);
        elif (!strcmp(arg2,  "5") || !stricmp(arg2, "L")) help_commands( 5);
        elif (!strcmp(arg2,  "6") || !stricmp(arg2, "R")) help_commands( 6);
        elif (!strcmp(arg2,  "7") || !stricmp(arg2, "B")) help_commands( 7);
        elif (!strcmp(arg2,  "8") || !stricmp(arg2, "S")) help_commands( 8);
        elif (!strcmp(arg2,  "9") || !stricmp(arg2, "T")) help_commands( 9);
        elif (!strcmp(arg2, "10") || !stricmp(arg2, "O")) help_commands(10);
        elif (!strcmp(arg2, "11") || !stricmp(arg2, "A")) help_commands(11);
        else                                              help_commands( 0);
    } elif (!stricmp((const char*) arg1, "I"))
    {   if (allowable(TRUE))
        {   zprintf
            (   TEXTPEN_CLIOUTPUT,
                LLL(
                    MSG_ENGINE_IGNORING,
                    "Ignoring %s instruction at $%X.\n"
                ),
                opcodes[style][memory[iar]].name,
                (int) iar
            );
            iar = (iar & PAGE) + ((iar + table_size_2650[memory[iar]]) & NONPAGE);
            if (verbosity >= 1)
            {   view_next_2650();
            } else
            {   zprintf(TEXTPEN_CLIOUTPUT, "\n");
    }   }   }
    elif (!stricmp((const char*) arg1, "IC"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_IC].desc_id, menuinfo1[MENUITEM_IC].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_IC, "Usage: IC [<start-port> [<end-port>]]\n\n"));
        } elif (allowable(TRUE))
        {   if (arg4[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_IC].desc_id, menuinfo1[MENUITEM_IC].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_IC, "Usage: IC [<start-port> [<end-port>]]\n\n"));
            } elif (arg3[0]) // IC <start-port> <end-port>
            {   address1 = readbase((STRPTR) arg2, FALSE);
                address2 = readbase((STRPTR) arg3, FALSE);
                if
                (   address1 >= 0
                 && address2 <= 257
                 && address1 <= address2
                )
                {   for (i = address1; i <= address2; i++)
                    {   ioport[i].ip = FALSE;
                    }
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                } else
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL(
                            MSG_INVALIDRANGE,
                            "Valid range is $%X..$%X!\n\n"
                        ),
                        0,
                        0x101
                    );
            }   }
            elif (arg2[0]) // IC <start-port>
            {   address1 = readbase((STRPTR) arg2, FALSE);
                if (address1 >= 0 && address1 <= 257)
                {   ok = FALSE;
                    if (ioport[address1].ip)
                    {   ioport[address1].ip = FALSE;
                        ok = TRUE;
                    }
                    if (ok)
                    {   zprintf(TEXTPEN_CLIOUTPUT, "Cleared I/O port watchpoint on ");
                    } else
                    {   zprintf(TEXTPEN_CLIOUTPUT, "I/O port watchpoint not found on ");
                    }
                    switch (address1)
                    {
                    case  PORTC: zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_IOCONTROLPORT,  "I/O control port"       )          );
                    acase PORTD: zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_IODATAPORT,     "I/O data port"          )          );
                    adefault:    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_IOEXTENDEDPORT, "I/O extended port $%02X"), address1);
                    }
                    zprintf(TEXTPEN_CLIOUTPUT, ".\n\n");
                } else
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL(
                            MSG_INVALIDRANGE,
                            "Valid range is $%X..$%X!\n\n"
                        ),
                        0,
                        0x101
                    );
            }   }
            else // IC
            {   ok = FALSE;
                for (address1 = 0; address1 <= 257; address1++)
                {   if (ioport[address1].ip)
                    {   ioport[address1].ip = FALSE;
                        ok = TRUE;
                }   }
                if (ok)
                {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_ENGINE_CLEAREDIPS, "Cleared all I/O port watchpoints.\n\n"));
                } else
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL
                        (   MSG_NOWPS,
                            "No watchpoints to clear!\n\n"
                        )
                    );
    }   }   }   }
    elif (!stricmp((const char*) arg1, "IL"))
    {   if (allowable(TRUE))
        {   ok = FALSE;
            for (i = 0; i <= 257; i++)
            {   if (ioport[i].ip)
                {   switch (i)
                    {
                    case  PORTC: zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_IOCONTROLPORT,  "I/O control port"       )   );
                    acase PORTD: zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_IODATAPORT,     "I/O data port"          )   );
                    adefault:    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_IOEXTENDEDPORT, "I/O extended port $%02X"), i);
                    }
                    switch (ioport[i].cond.the2nd)
                    {
                    case COND_UN:
                        zprintf(TEXTPEN_CLIOUTPUT, "\n");
                    acase COND_MASK:
                        dec_to_bin((UBYTE) ioport[i].cond.the3rd);
                        zprintf(TEXTPEN_CLIOUTPUT, " ");
                        zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_BITSONLY, "(bits %s only)\n"), v_bin);
                    adefault:
                        makestrings(&ioport[i].cond);
                        zprintf
                        (   TEXTPEN_CLIOUTPUT,
                            " %s %s %s %s\n",
                            LLL(MSG_WHEN, "when"),
                            firststring,
                            condstring,
                            secondstring
                        );
                    }
                    ok = TRUE;
            }   }
            if (ok)
            {   zprintf(TEXTPEN_CLIOUTPUT, "\n");
            } else
            {   zprintf
                (   TEXTPEN_CLIOUTPUT,
                    "%s.\n\n",
                    LLL(
                        MSG_NONE,
                        "None"
                )   );
    }   }   }
    elif (!stricmp((const char*) arg1, "HISTORY"))
    {   for (i = HISTORYLINES - 1; i >= 0; i--) // 0 will always be "HISTORY". i must be signed!
        {   if (olduserinput[i][0] != EOS)
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", olduserinput[i]);
        }   }
        zprintf(TEXTPEN_CLIOUTPUT, "\n");
    } elif (!stricmp((const char*) arg1, "IM"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_IM].desc_id, menuinfo1[MENUITEM_IM].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_IM, "Usage: IM [<start-address> <end-address>] [<filename>]\n\n"));
        } elif (allowable(TRUE))
        {   /* if (arg5[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_IM].desc_id, menuinfo1[MENUITEM_IM].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_IM, "Usage: IM [<start-address> <end-address>] [<filename>]\n\n"));
            } el */ if (arg4[0]) // <start-address> <end-address> <filename>
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                if (check_range(address1, address2))
                {   cd_projects();
                    fixextension(filekind[KIND_ASM].extension, (STRPTR) arg4, TRUE, "");
                    if ((TheLocalHandle = fopen((const char*) arg4, "wt")))
                    {
#ifdef WIN32
                        if (address2 - address1 > 256)
                        {   hurry = TRUE;
                            cls();
                        }
#endif
                        view_imagery(address1, address2, TheLocalHandle);
#ifdef WIN32
                        if (address2 - address1 > 256)
                        {   hurry = FALSE;
                            zprintf(TEXTPEN_DEFAULT, "");
                        }
#endif
                        DISCARD fclose(TheLocalHandle);
                        // TheLocalHandle = NULL;
                    } else
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                    }
                    cd_progdir();
            }   }
            elif (arg3[0]) // <start-address> <end-address>
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                if (check_range(address1, address2))
                {
#ifdef WIN32
                    if (address2 - address1 > 256)
                    {   hurry = TRUE;
                        cls();
                    }
#endif
                    view_imagery(address1, address2, NULL);
#ifdef WIN32
                    if (address2 - address1 > 256)
                    {   hurry = FALSE;
                        zprintf(TEXTPEN_DEFAULT, "");
                    }
#endif
            }   }
            elif (arg2[0]) // <filename>
            {   cd_projects();
                fixextension(filekind[KIND_ASM].extension, (STRPTR) arg4, TRUE, "");
                if ((TheLocalHandle = fopen((const char*) arg4, "wt")))
                {
#ifdef WIN32
                    hurry = TRUE;
                    cls();
#endif
                    view_imagery(0, 32767, TheLocalHandle);
#ifdef WIN32
                    hurry = FALSE;
                    zprintf(TEXTPEN_DEFAULT, "");
#endif
                    DISCARD fclose(TheLocalHandle);
                    // TheLocalHandle = NULL;
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                }
                cd_progdir();
            } else
            {
#ifdef WIN32
                hurry = TRUE;
                cls();
#endif
                view_imagery(0, 32767, NULL);
#ifdef WIN32
                hurry = FALSE;
                zprintf(TEXTPEN_DEFAULT, "");
#endif
    }   }   }
    elif (!stricmp((const char*) arg1, "INJECT"))
    {   if (ASKUSAGE || !arg2[0] || arg3[0])
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_INJECT].desc_id, menuinfo1[MENUITEM_INJECT].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_INJECT, "Usage: INJECT <filename>\n\n"));
        } elif (!HASDISK)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
        } elif (!drive[debugdrive].inserted)
        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NODISK, "No disk is inserted!\n\n"));
        } else
        {   dir_disk(TRUE, debugdrive);
            switch (machine)
            {
            case  BINBUG: binbug_inject_file(arg2);
            acase CD2650: cd2650_inject_file(arg2);
            acase TWIN:   twin_inject_file(arg2);
    }   }   }
    elif (!stricmp((const char*) arg1, "IP"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_IP].desc_id, menuinfo1[MENUITEM_IP].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_IP, "Usage: IP [<start-port> [<end-port>] [<addr/reg> <condition> <value>]]\n\n"));
        } elif (allowable(TRUE))
        {   if (arg7[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_IP].desc_id, menuinfo1[MENUITEM_IP].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_IP, "Usage: IP [<start-port> [<end-port>] [<addr/reg> <condition> <value>]]\n\n"));
            } elif (arg6[0])
            {   // IP <start-port> <end-port> <addr/reg> <condition> <value>
                address1 = readbase(        (STRPTR) arg2, FALSE);
                address2 = readbase(        (STRPTR) arg3, FALSE);
                the1st   = parse_expression((STRPTR) arg4, ALLTOKENS, FALSE);
                the2nd   = string_to_cond(  (STRPTR) arg5);
                address3 = parse_expression((STRPTR) arg6, MAX_ADDR,  FALSE); // not really an address
                if
                (   address1 >= 0
                 && address2 <= 257
                 && address3 != OUTOFRANGE
                 && address1 <= address2
                )
                {   for (i = address1; i <= address2; i++)
                    {   DISCARD ip_add(i, the1st, the2nd, address3, 0xFF, FALSE);
                    }
                    if (!watchreads && !watchwrites)
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_WPWARNING, "Warning: watching of reads and writes is currently turned off! (WR and WW to enable.)\n"));
                    }
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_IP].desc_id, menuinfo1[MENUITEM_IP].desc_str));
                    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_IP, "Usage: IP [<start-port> [<end-port>] [<addr/reg> <condition> <value>]]\n\n"));
            }   }
            elif (arg5[0])
            {   // IP <port> <addr/reg> <condition> <value>
                address1 = readbase(        (STRPTR) arg2, FALSE);
                the1st   = parse_expression((STRPTR) arg3, ALLTOKENS, FALSE);
                the2nd   = string_to_cond(  (STRPTR) arg4);
                address3 = parse_expression((STRPTR) arg5, MAX_ADDR,  FALSE); // not really an address
                DISCARD ip_add(address1, the1st, the2nd, address3, 0xFF, TRUE);
            } elif (arg4[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_IP].desc_id, menuinfo1[MENUITEM_IP].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_IP, "Usage: IP [<start-port> [<end-port>] [<addr/reg> <condition> <value>]]\n\n"));
            } elif (arg3[0])
            {   // IP <start-port> <end-port>
                address1 = readbase((STRPTR) arg2, FALSE);
                address2 = readbase((STRPTR) arg3, FALSE);
                if
                (   address1 >= 0
                 && address2 <= 257
                 && address1 <= address2
                )
                {   for (i = address1; i <= address2; i++)
                    {   DISCARD ip_add(i, 0, COND_UN, 0, 0xFF, FALSE);
                    }
                    update_memory(TRUE);
                    if (!watchreads && !watchwrites)
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_WPWARNING, "Warning: watching of reads and writes is currently turned off! (WR and WW to enable.)\n"));
                    }
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_IP].desc_id, menuinfo1[MENUITEM_IP].desc_str));
                    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_IP, "Usage: IP [<start-port> [<end-port>] [<addr/reg> <condition> <value>]]\n\n"));
            }   }
            elif (arg2[0])
            {   // IP <port>[:mask]
                address1 = readbase((STRPTR) arg2, TRUE);
                DISCARD ip_add(address1, 0, COND_UN, 0, watchmask, TRUE);
            } else
            {   // IP
                for (i = 0; i < 258; i++)
                {   DISCARD ip_add(i, 0, COND_UN, 0, 0xFF, FALSE);
                }
                zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
    }   }   }
    elif (!stricmp((const char*) arg1, "J") || !stricmp((const char*) arg1, "JUMP"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_JUMP].desc_id, menuinfo1[MENUITEM_JUMP].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_JUMP, "Usage: J|JUMP <address>\n\n"));
        } elif (allowable(TRUE))
        {   if (arg3[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_JUMP].desc_id, menuinfo1[MENUITEM_JUMP].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_JUMP, "Usage: J|JUMP <address>\n\n"));
            } elif (arg2[0])
            {   tolimit = MAX_ADDR;
                tonum = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                if (tonum == OUTOFRANGE)
                {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_INVALIDRANGE, "Valid range is $%X..$%X!\n\n"), 0, MAX_ADDR);
                } else
                {   fromnum = (int) iar;
                    poketoken = TOKEN_IAR;
                    tolimit = 32767;
                    poke_end(FALSE, FALSE);
            }   }
            else
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_JUMP].desc_id, menuinfo1[MENUITEM_JUMP].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_JUMP, "Usage: J|JUMP <address>\n\n"));
    }   }   }
    elif (!stricmp((const char*) arg1, "L"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s: L A|B|C|I|N|S\n\n", LLL(MSG_USAGE, "Usage"));
        } elif (allowable(TRUE))
        {   if (!arg2[0] || arg3[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s: L A|B|C|I|N|S\n\n", LLL(MSG_USAGE, "Usage"));
            } elif (!stricmp((const char*) arg2, "A"))
            {   zprintf
                (   TEXTPEN_CLIOUTPUT,
                    LLL(
                        MSG_ENGINE_LOGREADWRITES,
                        "Log illegal reads/writes"
                )   );
                fliplog(&logreadwrites);
            } elif (!stricmp((const char*) arg2, "B"))
            {   if
                (   machine == ELEKTOR
                 || machine == PIPBUG
                 || machine == BINBUG
                 || machine == INSTRUCTOR
                 || machine == TWIN
                 || machine == CD2650
                 || machine == PHUNSY
                 || machine == SELBST
                )
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL(
                            MSG_ENGINE_LOGMONITOR,
                            "Log BIOS calls"
                    )   );
                    fliplog(&logbios);
                } else // eg. ARCADIA, INTERTON, coin-ops, MIKIT, PONG, TYPERIGHT
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
            }   }
            elif (!stricmp((const char*) arg2, "C"))
            {   zprintf
                (   TEXTPEN_CLIOUTPUT,
                    LLL(
                        MSG_ENGINE_LOGINEFFICIENT,
                        "Log inefficient instructions"
                )   );
                fliplog(&loginefficient);
            } elif (!stricmp((const char*) arg2, "I"))
            {   zprintf
                (   TEXTPEN_CLIOUTPUT,
                    LLL(
                        MSG_ENGINE_LOGINSTRUCTIONS,
                        "Log illegal instructions"
                )   );
                fliplog(&log_illegal);
            } elif (!stricmp((const char*) arg2, "N"))
            {   zprintf
                (   TEXTPEN_CLIOUTPUT,
                    LLL(
                        MSG_ENGINE_LOGINTERRUPTS,
                        "Log interrupts"
                )   );
                fliplog(&log_interrupts);
            } elif (!stricmp((const char*) arg2, "S"))
            {   zprintf
                (   TEXTPEN_CLIOUTPUT,
                    LLL(
                        MSG_ENGINE_LOGSUBROUTINES,
                        "Log subroutine calls/returns"
                )   );
                fliplog(&logsubroutines);
            } else
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s: L A|B|C|I|N|S\n\n", LLL(MSG_USAGE, "Usage"));
    }   }   }
    elif (!stricmp((const char*) arg1, "LOADBIN") || !stricmp((const char*) arg1, "LOAD")) // LOAD synonym is deprecated and will be removed entirely soon
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_LOADBIN].desc_id, menuinfo1[MENUITEM_LOADBIN].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_LOADBIN, "Usage: LOADBIN <start-address> <filename>\n\n"));
        } elif (allowable(TRUE))
        {   if (!arg2[0] || !arg3[0] || arg4[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_LOADBIN].desc_id, menuinfo1[MENUITEM_LOADBIN].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_LOADBIN, "Usage: LOADBIN <start-address> <filename>\n\n"));
            } else
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                if (address1 == OUTOFRANGE)
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL(
                            MSG_INVALIDRANGE,
                            "Valid range is $%X..$%X!\n\n"
                        ),
                        0,
                        MAX_ADDR
                    );
                } else
                {   cd_projects();
                    if (getsize((STRPTR) arg3) == 0)
                    {   fixextension(filekind[KIND_BIN].extension, (STRPTR) arg3, FALSE, "");
                    }
                    localsize = (int) getsize((STRPTR) arg3);
                    if (localsize + address1 > MAX_ADDR + 1)
                    {   zprintf
                        (   TEXTPEN_CLIOUTPUT,
                            LLL(
                                MSG_ENGINE_TOOLARGE2,
                                "File is too large!\n\n"
                        )   );
                    } elif ((TheLocalHandle = fopen((const char*) arg3, "rb")))
                    {   if (fread(&memory[address1], (size_t) localsize, 1, TheLocalHandle) == 1)
                        {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                        } else
                        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                        }
                        DISCARD fclose(TheLocalHandle);
                        // TheLocalHandle = NULL;
                    } else
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                    }
                    cd_progdir();
    }   }   }   }
    elif (!stricmp((const char*) arg1, "LOADSYM"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_LOADSYM].desc_id, menuinfo1[MENUITEM_LOADSYM].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_LOADSYM, "Usage: LOADSYM [<filename>]\n\n"));
        } elif (allowable(TRUE))
        {   if (arg3[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_LOADSYM].desc_id, menuinfo1[MENUITEM_LOADSYM].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_LOADSYM, "Usage: LOADSYM [<filename>]\n\n"));
            } elif (!arg2[0])
            {   userlabels = 0;
                zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
            } else
            {   cd_projects();
                if (getsize((STRPTR) arg2) == 0)
                {   fixextension(filekind[KIND_SYM].extension, (STRPTR) arg2, FALSE, "");
                }
                symsize = (int) getsize((STRPTR) arg2);
                if ((TheLocalHandle = fopen((const char*) arg2, "rb")))
                {   SymBuffer = malloc(symsize);
                    if (fread(SymBuffer, (size_t) symsize, 1, TheLocalHandle) == 1)
                    {   loadsym((TEXT*) SymBuffer);
                        free(SymBuffer);
                        SymBuffer = NULL;
                        zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                    } else
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                    }
                    DISCARD fclose(TheLocalHandle);
                    // TheLocalHandle = NULL;
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                }
                cd_progdir();
    }   }   }
    elif (!stricmp((const char*) arg1, "N"))
    {   if (ASKUSAGE || !arg2[0] || arg3[0])
        {   zprintf(TEXTPEN_CLIOUTPUT, "N 0: %s\n" \
                                       "N 1: %s\n" \
                                       "N 2: %s\n" \
                                       "N 3: %s\n",
                                       LLL(menuinfo2[MENUOPT_N_0].desc_id, menuinfo2[MENUOPT_N_0].desc_str),
                                       LLL(menuinfo2[MENUOPT_N_1].desc_id, menuinfo2[MENUOPT_N_1].desc_str),
                                       LLL(menuinfo2[MENUOPT_N_2].desc_id, menuinfo2[MENUOPT_N_2].desc_str),
                                       LLL(menuinfo2[MENUOPT_N_3].desc_id, menuinfo2[MENUOPT_N_3].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, "%s: N 0|1|2|3\n\n", LLL(MSG_USAGE, "Usage"));
        } else
        {   if (allowable(TRUE))
            {   if     (!strcmp((const char*) arg2, "0"))
                {   style = STYLE_SIGNETICS1;
                    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NOTATION_SIG1, "Notation is now non-extended Signetics.\n\n"));
                    update_notation();
                } elif (!strcmp((const char*) arg2, "1"))
                {   style = STYLE_SIGNETICS2;
                    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NOTATION_SIG2, "Notation is now extended Signetics.\n\n"    ));
                    update_notation();
                } elif (!strcmp((const char*) arg2, "2"))
                {   style = STYLE_CALM;
                    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NOTATION_CALM, "Notation is now CALM.\n\n"                  ));
                    update_notation();
                } elif (!strcmp((const char*) arg2, "3"))
                {   style = STYLE_IEEE;
                    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NOTATION_IEEE, "Notation is now IEEE-694.\n\n"              ));
                    update_notation();
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, "N 0: %s\n" \
                                               "N 1: %s\n" \
                                               "N 2: %s\n" \
                                               "N 3: %s\n",
                                               LLL(menuinfo2[MENUOPT_N_0].desc_id, menuinfo2[MENUOPT_N_0].desc_str),
                                               LLL(menuinfo2[MENUOPT_N_1].desc_id, menuinfo2[MENUOPT_N_1].desc_str),
                                               LLL(menuinfo2[MENUOPT_N_2].desc_id, menuinfo2[MENUOPT_N_2].desc_str),
                                               LLL(menuinfo2[MENUOPT_N_3].desc_id, menuinfo2[MENUOPT_N_3].desc_str));
                    zprintf(TEXTPEN_CLIOUTPUT, "%s: N 0|1|2|3\n\n", LLL(MSG_USAGE, "Usage"));
    }   }   }   }
    elif (!stricmp((const char*) arg1, "PB"))
    {   if (allowable(TRUE))
        {   if (pausebreak)
            {   pausebreak = FALSE;
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_PB_OFF, "Pause after breakpoints/watchpoints is now off.\n\n"));
            } else
            {   pausebreak = TRUE;
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_PB_ON,  "Pause after breakpoints/watchpoints is now on.\n\n" ));
    }   }   }
    elif (!stricmp((const char*) arg1, "PL"))
    {   if (allowable(TRUE))
        {   if (pauselog)
            {   pauselog = FALSE;
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_PL_OFF, "Pause after logging is now off.\n\n"));
            } else
            {   pauselog = TRUE;
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_PL_ON,  "Pause after logging is now on.\n\n" ));
    }   }   }
    elif (!strcmp((const char*) arg1, "="))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_EQUALS].desc_id, menuinfo1[MENUITEM_EQUALS].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_EQUALS, "Usage: = [<address>]\n\n"));
        } elif (allowable(TRUE))
        {   if (arg3[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_EQUALS].desc_id, menuinfo1[MENUITEM_EQUALS].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_EQUALS, "Usage: = [<address>]\n\n"));
            } elif (arg2[0])
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                if (address1 == OUTOFRANGE)
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL(
                            MSG_INVALIDRANGE,
                            "Valid range is $%X..$%X!\n\n"
                        ),
                        0,
                        MAX_ADDR
                    );
                } else
                {   DISCARD number_to_friendly(address1, (STRPTR) friendly, FALSE, 1);

                    j = 0;
                    started = FALSE;
                    for (i = 0; i < 15; i++) // only expects up to 32K
                    {   if (address1 & (0x4000 >> i))
                        {   started = TRUE;
                            tempstring[j++] = '1';
                        } else
                        {   if (started)
                            {   tempstring[j++] = '0';
                    }   }   }
                    if (!started)
                    {   // assert(j == 0);
                        tempstring[j++] = '0';
                    }
                    tempstring[j] = EOS;
                    if (ISQWERTY && address1 <= 31)
                    {   zprintf(TEXTPEN_CLIOUTPUT, "%%%s, @%o, !%d, $%X, %s\n", tempstring, address1, address1, address1, asciiname_short[address1]);
                    } elif (ISQWERTY && address1 <= 127)
                    {   zprintf(TEXTPEN_CLIOUTPUT, "%%%s, @%o, !%d, $%X, '%c'\n", tempstring, address1, address1, address1, address1);
                    } elif (!ISQWERTY && machine != INTERTON && address1 <= 255)
                    {   zprintf(TEXTPEN_CLIOUTPUT, "%%%s, @%o, !%d, $%X, '%c'\n", tempstring, address1, address1, address1, guestchar((UBYTE) address1));
                    } else
                    {   zprintf(TEXTPEN_CLIOUTPUT, "%%%s, @%o, !%d, $%X\n", tempstring, address1, address1, address1);
                    }
                    show_data_comment(address1, 1, NULL);

                    if (mirror_r[address1] != address1 || mirror_w[address1] != address1)
                    {   DISCARD getfriendly(mirror_r[address1]);
                        zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_READMIRROR,  "Read mirror of %s." ), friendly);
                        zprintf(TEXTPEN_CLIOUTPUT, " ");
                        DISCARD getfriendly(mirror_w[address1]);
                        zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_WRITEMIRROR, "Write mirror of %s."), friendly);
                        zprintf(TEXTPEN_CLIOUTPUT, "\n");
                    }
                    /* This is redundant now as all data comments have it...
                    if (memflags[address1] & READONCE)
                    {   zprintf(TEXTPEN_CLIOUTPUT, "Read-once!\n");
                    } */
                    if (memflags[address1] & NOREAD)
                    {   if (memflags[address1] & NOWRITE)
                        {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n", LLL(MSG_LEGEND3, "Unmapped"));
                        } else
                        {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n", LLL(MSG_LEGEND4, "Write-only"));
                    }   }
                    else
                    {   if (memflags[address1] & NOWRITE)
                        {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n", LLL(MSG_LEGEND5, "Read-only"));
                        } else
                        {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n", LLL(MSG_LEGEND5, "Read/write"));
                    }   }
                    if (memflags[address1] & BREAKPOINT)
                    {   if (bp[address1].the2nd == COND_UN)
                        {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n", LLL(MSG_LEGEND1, "Breakpoint"));
                        } else
                        {   makestrings(&bp[address1]);
                            zprintf
                            (   TEXTPEN_CLIOUTPUT,
                                "%s %s %s %s %s.\n",
                                LLL(MSG_LEGEND1, "Breakpoint"),
                                LLL(MSG_WHEN, "when"),
                                firststring,
                                condstring,
                                secondstring
                            );
                    }   }
                    if (memflags[address1] & WATCHPOINT)
                    {   if (wp[address1].the2nd == COND_UN)
                        {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n", LLL(MSG_LEGEND7, "Watchpoint"));
                        } else
                        {   makestrings(&wp[address1]);
                            zprintf
                            (   TEXTPEN_CLIOUTPUT,
                                "%s %s %s %s %s.\n",
                                LLL(MSG_LEGEND7, "Watchpoint"),
                                LLL(MSG_WHEN, "when"),
                                firststring,
                                condstring,
                                secondstring
                            );
                    }   }
                    zprintf(TEXTPEN_CLIOUTPUT, "Coverage: %s\n", coveragename[coverage[address1] & COVERAGE_BITS]);
                    if (address1 <= 255)
                    {   zprintf(TEXTPEN_CLIOUTPUT, "%s %s\n", LLL(MSG_OPCODE, "Opcode:"), opcodes[style][address1].name);
                    }
                    zprintf(TEXTPEN_CLIOUTPUT, "\n");
            }   }
            else
            {   tabpos = 0;
                for (i = 0; i <= 0x7FFF; i++)
                {   DISCARD number_to_friendly(i, (STRPTR) friendly, FALSE, 2);
                }
                if (!tabpos)
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s.", LLL(MSG_NONE, "None"));
                }
                zprintf(TEXTPEN_CLIOUTPUT, "\n");
                check_labels();
                zprintf(TEXTPEN_CLIOUTPUT, "\n");
    }   }   }
    elif (!stricmp((const char*) arg1, "R"))
    {   if (ASKUSAGE || !arg2[0] || arg3[0])
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_R].desc_id, menuinfo1[MENUITEM_R].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_R, "Usage: R F|I|L|R|S|<number>\n\n"));
        } elif (!stricmp((const char*) arg2, "F"))
        {   clearruntos();
            runtoframe = TRUE;
            emu_unpause();
        } elif (!stricmp((const char*) arg2, "I"))
        {   if (machines[machine].pvis || machine == INSTRUCTOR || machine == TWIN || machine == PHUNSY || machine == BINBUG)
            {   clearruntos();
                runtointerrupt = TRUE;
                emu_unpause();
            } else // eg. ARCADIA, PIPBUG, CD2650, PONG, TYPERIGHT
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
        }   }
        elif (!stricmp((const char*) arg2, "L"))
        {   if (allowable(TRUE))
            {   clearruntos();
                runtoloopend = TRUE;
                emu_unpause();
        }   }
        elif (!stricmp((const char*) arg2, "R"))
        {   if
            (   machine == ARCADIA
             || machines[machine].pvis
             || machine == BINBUG
             || machine == CD2650
             || machine == PHUNSY
            )
            {   clearruntos();
                rastn = cpuy + 1;
                if (rastn >= PVI_RASTLINES)
                {   rastn = 0;
                }
                emu_unpause();
            } else // eg. PIPBUG, BINBUG, INSTRUCTOR, CD2650, PHUNSY, PONG, TYPERIGHT
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
        }   }
        elif (!stricmp((const char*) arg2, "S"))
        {   if (allowable(TRUE))
            {   if ((psu & PSU_SP) == 0)
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        (STRPTR) LLL(
                            MSG_RUNNING_TO,
                            "Running to $%X.\n\n"
                        ),
                        ras[7]
                    );
                    clearruntos();
                    memflags[ras[7]] |= BREAKPOINT;
                    emu_unpause();
                } else
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        (STRPTR) LLL(
                            MSG_RUNNING_TO,
                            "Running to $%X.\n\n"
                        ),
                        ras[(psu & PSU_SP) - 1]
                    );
                    clearruntos();
                    memflags[ras[(psu & PSU_SP) - 1]] |= BREAKPOINT;
                    emu_unpause();
        }   }   }
        else // R <number>
        {   if
            (   machine == ARCADIA
             || machines[machine].pvis
             || machine == BINBUG
             || machine == CD2650
             || machine == PHUNSY
            )
            {   clearruntos();
                DISCARD stcd_l((const char*) arg2, (void*) &rastn); // string -> decimal number
                /* if (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR)
                {   rastn += USG_YMARGIN;
                } */
                emu_unpause();
            } else // eg. PIPBUG, BINBUG, INSTRUCTOR, CD2650, PONG, TYPERIGHT
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
    }   }   }
    elif (!stricmp((const char*) arg1, "READPORT"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_READPORT].desc_id, menuinfo1[MENUITEM_READPORT].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_READPORT, "Usage: READPORT <port>|CTRL|DATA\n\n"));
        } elif (allowable(TRUE))
        {   if (!arg2[0] || arg3[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_READPORT].desc_id, menuinfo1[MENUITEM_READPORT].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_READPORT, "Usage: READPORT <port>|CTRL|DATA\n\n"));
            } else
            {   if (!stricmp(arg2, "CTRL"))
                {   zprintf(TEXTPEN_CLIOUTPUT, "$%02X\n\n", readport(PORTC));
                } elif (!stricmp(arg2, "DATA"))
                {   zprintf(TEXTPEN_CLIOUTPUT, "$%02X\n\n", readport(PORTD));
                } else
                {   address1 = readbase((STRPTR) arg2, FALSE);
                    if (address1 >= 0 && address1 <= 255)
                    {   zprintf(TEXTPEN_CLIOUTPUT, "$%02X\n\n", readport(address1));
                    } else
                    {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_READPORT].desc_id, menuinfo1[MENUITEM_READPORT].desc_str));
                        zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_READPORT, "Usage: READPORT <port>|CTRL|DATA\n\n"));
    }   }   }   }   }
    elif (!stricmp((const char*) arg1, "REL"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_REL].desc_id, menuinfo1[MENUITEM_REL].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_REL, "Usage: REL <source-addr> [*]<target-addr>\n\n"));
        } elif (arg2[0] && arg3[0])
        {   if (allowable(TRUE))
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                address1 = (address1 & PAGE) + ((address1 + 2) & NONPAGE);
                if (arg3[0] == '*')
                {   indirect = TRUE;
                    address2 = parse_expression((STRPTR) &arg3[1], MAX_ADDR, FALSE);
                } else
                {   indirect = FALSE;
                    address2 = parse_expression((STRPTR)  arg3   , MAX_ADDR, FALSE);
                }
                if (address1 == OUTOFRANGE || address2 == OUTOFRANGE)
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL(
                            MSG_INVALIDRANGE,
                            "Valid range is $%X..$%X!\n\n"
                        ),
                            0,
                        MAX_ADDR
                    );
                } elif ((address1 & PAGE) != (address2 & PAGE))
                {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_SAMEPAGE, "Addresses must be on the same page!\n\n"));
                } else
                {   address3 =  address1 & PAGE;
                    address1 &= NONPAGE;
                    address2 &= NONPAGE;
                    if (address2 < address1 - 64)
                    {   address2 += PLEN; // or could do address1 -= PLEN;
                    } elif (address2 > address1 + 63)
                    {   address1 += PLEN; // or could do address2 -= PLEN;
                    }
                    if (address2 < address1 - 64 || address2 > address1 + 63)
                    {   zprintf
                        (   TEXTPEN_CLIOUTPUT,
                            LLL(
                                MSG_INVALIDRANGE,
                                "Valid range is $%X..$%X!\n\n"
                            ),
                            address3 + ((address1 - 64) & NONPAGE),
                            address3 + ((address1 + 63) & NONPAGE)
                        );
                    } else
                    {   result = address2 - address1;
                        if (result < 0)
                        {   result = 0x80 + result; // -64..-1 -> $40..$7F
                        }
                        if (indirect)
                        {   result += 0x80;
                        }
                        zprintf(TEXTPEN_CLIOUTPUT, "$%02X\n\n", result);
        }   }   }   }
        else
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_REL].desc_id, menuinfo1[MENUITEM_REL].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_REL, "Usage: REL <source-addr> [*]<target-addr>\n\n"));
    }   }
    elif (!stricmp((const char*) arg1, "REN") || !stricmp((const char*) arg1, "RENAME"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_RENAME].desc_id, menuinfo1[MENUITEM_RENAME].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_RENAME, "Usage: REN|RENAME <oldname> <newname>\n\n"));
        } elif (!HASDISK)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
        } elif (!drive[debugdrive].inserted)
        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NODISK,       "No disk is inserted!\n\n"));
        } elif (!arg3[0] || arg4[0])
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_RENAME].desc_id, menuinfo1[MENUITEM_RENAME].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_RENAME, "Usage: REN|RENAME <oldname> <newname>\n\n"));
        } else
        {   dir_disk(TRUE, debugdrive);
            found2 = FALSE;
            switch (machine)
            {
            case BINBUG:
                for (i = 0; i < 100; i++)
                {   if (drive[0].filename[i][0] != EOS)
                    {   drive[0].filename[i][10] = EOS; // to avoid a spurious GCC warning
                        strcpy((char*) tempstring, (const char*) drive[0].filename[i]); // spurious GCC warning if we use sprintf() for this
                        strcat((char*) tempstring, ".");
                        strcat((char*) tempstring, (const char*) drive[0].fileext[i]);
                        if (!stricmp((const char*) tempstring, (const char*) arg2))
                        {   found2 = binbug_rename_file(i, arg3);
                            if (found2)
                            {   binbug_dir_disk(TRUE, debugdrive);
                            }
                            break; // for speed
                }   }   }
            acase CD2650:
                for (i = 0; i < 64; i++)
                {   if (drive[0].filename[i][0] != EOS)
                    {   drive[0].filename[i][8] = EOS; // to avoid a spurious GCC warning
                        strcpy((char*) tempstring, (const char*) drive[0].filename[i]); // spurious GCC warning if we use sprintf() for this
                        strcat((char*) tempstring, ".");
                        strcat((char*) tempstring, (const char*) drive[0].fileext[i]);
                        if (!stricmp((const char*) tempstring, (const char*) arg2))
                        {   found2 = cd2650_rename_file(i, arg3);
                            if (found2)
                            {   cd2650_dir_disk(TRUE, debugdrive);
                            }
                            break; // for speed
                }   }   }
            acase TWIN:
                for (i = 0; i < 78; i++)
                {   if (drive[debugdrive].filename[i][0] != EOS)
                    {   sprintf((char*) tempstring, "%s", drive[debugdrive].filename[i]);
                        if (!stricmp((const char*) tempstring, (const char*) arg2))
                        {   found2 = twin_rename_file(i, debugdrive, arg3);
                            if (found2)
                            {   twin_dir_disk(TRUE, debugdrive);
                            }
                            break; // for speed
            }   }   }   }
            if (found2)
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
            } else
            {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
    }   }   }
    elif (!stricmp((const char*) arg1, "SAVEBIN"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SAVEBIN].desc_id, menuinfo1[MENUITEM_SAVEBIN].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SAVEBIN, "Usage: SAVEBIN <start-address> <end-address> <filename>\n\n"));
        } elif (allowable(TRUE))
        {   if (arg2[0] && arg3[0] && arg4[0])
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                if (check_range(address1, address2))
                {   cd_projects();
                    fixextension(filekind[KIND_BIN].extension, (STRPTR) arg4, TRUE, "");
                    if ((TheLocalHandle = fopen((const char*) arg4, "wb")))
                    {   DISCARD fwrite(&memory[address1], (size_t) (address2 - address1 + 1), 1, TheLocalHandle);
                        DISCARD fclose(TheLocalHandle);
                        // TheLocalHandle = NULL;
                        zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                    } else
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                    }
#ifdef AMIGA
                    writeicon(KIND_BIN, (STRPTR) arg4);
#endif
                    cd_progdir();
            }   }
            else
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SAVEBIN].desc_id, menuinfo1[MENUITEM_SAVEBIN].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SAVEBIN, "Usage: SAVEBIN <start-address> <end-address> <filename>\n\n"));
    }   }   }
    elif (!stricmp((const char*) arg1, "SAVEAOF") || !stricmp((const char*) arg1, "SAVEEOF"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SAVEAOF].desc_id, menuinfo1[MENUITEM_SAVEAOF].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SAVEAOF, "Usage: SAVEAOF|SAVEEOF <start-address> <end-address> [<filename> [<game-start> [<block-size>]]]\n\n"));
        } elif (allowable(TRUE))
        {   if (!arg4[0])
            {   if (file_game[0])
                {   strcpy(arg4, file_game);
                } else
                {   strcpy(arg4, "Untitled");
            }   }
            if (arg2[0] && arg3[0] && arg4[0])
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                if (arg5[0])
                {   address3 = parse_expression((STRPTR) arg5, MAX_ADDR, FALSE);
                } else
                {   address3 = (startaddr_h * 256) + startaddr_l;
                }
                if
                (   address1 >  address2 // perhaps we might want a different error message for this case
                 || address1 == OUTOFRANGE
                 || address2 == OUTOFRANGE
                 || address3 == OUTOFRANGE
                )
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL(
                            MSG_INVALIDRANGE,
                            "Valid range is $%X..$%X!\n\n"
                        ),
                        0,
                        MAX_ADDR
                    );
                } else
                {   if (arg6[0])
                    {   preferredsize = readbase((STRPTR) arg6, FALSE);
                    } else
                    {   preferredsize = (machine == ELEKTOR) ? 16 : 30;
                    }
                    if (preferredsize < 1 || preferredsize > 255)
                    {   zprintf
                        (   TEXTPEN_CLIOUTPUT,
                            LLL
                            (   MSG_INVALIDRANGE,
                                "Valid range is $%X..$%X!\n\n"
                            ),
                            1,
                            0xFF
                        );
                    } else
                    {   cd_projects(); // or perhaps cd_games()?
                        if (machine == ELEKTOR)
                        {   fixextension(filekind[KIND_EOF].extension, (STRPTR) arg4, TRUE, "");
                            if (save_eof((STRPTR) arg4, address1, address2, address3, preferredsize))
                            {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                            } else
                            {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                        }   }
                        else
                        {   fixextension(filekind[KIND_AOF].extension, (STRPTR) arg4, TRUE, "");
                            if (save_aof((STRPTR) arg4, address1, address2, address3, preferredsize))
                            {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                            } else
                            {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                        }   }
                        cd_progdir();
            }   }   }
            else
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SAVEAOF].desc_id, menuinfo1[MENUITEM_SAVEAOF].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SAVEAOF, "Usage: SAVEAOF|SAVEEOF <start-address> <end-address> [<filename> [<game-start> [<block-size>]]]\n\n"));
    }   }   }
    elif (!stricmp((const char*) arg1, "SAVEBPNF") || !stricmp((const char*) arg1, "SAVEBNPF"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SAVEBPNF].desc_id, menuinfo1[MENUITEM_SAVEBPNF].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SAVEBPNF, "Usage: SAVEBPNF <start-address> <end-address> <filename>\n\n"));
        } elif (allowable(TRUE))
        {   if (arg2[0] && arg3[0] && arg4[0])
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                if (check_range(address1, address2))
                {   cd_projects();
                    fixextension(filekind[KIND_BPNF].extension, (STRPTR) arg4, TRUE, "");
                    if (save_bpnf((STRPTR) arg4, address1, address2))
                    {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                    } else
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                    }
                    cd_progdir();
            }   }
            else
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SAVEBPNF].desc_id, menuinfo1[MENUITEM_SAVEBPNF].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SAVEBPNF, "Usage: SAVEBPNF <start-address> <end-address> <filename>\n\n"));
    }   }   }
    elif (!stricmp((const char*) arg1, "SAVESMS"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SAVESMS].desc_id, menuinfo1[MENUITEM_SAVESMS].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SAVESMS, "Usage: SAVESMS <end-address> <filename>\n\n"));
        } elif (allowable(TRUE))
        {   if (arg2[0] && arg3[0])
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                if (check_range(0, address1))
                {   cd_projects();
                    fixextension(filekind[KIND_SMS].extension, (STRPTR) arg4, TRUE, "");
                    if (save_sms((STRPTR) arg3, address1))
                    {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                    } else
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                    }
                    cd_progdir();
            }   }
            else
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SAVESMS].desc_id, menuinfo1[MENUITEM_SAVESMS].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SAVESMS, "Usage: SAVESMS <end-address> <filename>\n\n"));
    }   }   }
    elif
    (   !stricmp((const char*) arg1, "SAVECMD" )
     || !stricmp((const char*) arg1, "SAVEIMAG")
     || !stricmp((const char*) arg1, "SAVEMOD" )
    )
    {   if (ASKUSAGE || !arg5[0] || arg6[0])
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SAVECMD].desc_id, menuinfo1[MENUITEM_SAVECMD].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SAVECMD, "Usage: SAVECMD|SAVEIMAG|SAVEMOD <start-address> <end-address> [<filename> [<game-start>]]\n\n"));
        } elif (!HASDISK)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
        } else
        {   if (!arg4[0])
            {   if (file_game[0])
                {   strcpy(arg4, file_game);
                } else
                {   strcpy(arg4, "Untitled");
            }   }
            address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
            address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
            if (arg5[0])
            {   address3 = parse_expression((STRPTR) arg5, MAX_ADDR, FALSE);
            } else
            {   address3 = (startaddr_h * 256) + startaddr_l;
            }
            if
            (   address1 >  address2 // perhaps we might want a different error message for this case
             || address1 == OUTOFRANGE
             || address2 == OUTOFRANGE
             || address3 == OUTOFRANGE
            )
            {   zprintf
                (   TEXTPEN_CLIOUTPUT,
                    LLL(
                        MSG_INVALIDRANGE,
                        "Valid range is $%X..$%X!\n\n"
                    ),
                    0,
                    MAX_ADDR
                );
            } else
            {   if     (machine == BINBUG && stricmp((const char*) arg1, "SAVECMD" ))
                {   zprintf(TEXTPEN_ERROR, "Assuming you meant SAVECMD.\n");
                } elif (machine == CD2650 && stricmp((const char*) arg1, "SAVEIMAG"))
                {   zprintf(TEXTPEN_ERROR, "Assuming you meant SAVEIMAG.\n");
                } elif (machine == TWIN   && stricmp((const char*) arg1, "SAVEMOD" ))
                {   zprintf(TEXTPEN_ERROR, "Assuming you meant SAVEMOD.\n");
                }
                cd_projects();
                switch (machine)
                {
                case BINBUG:
                    fixextension(filekind[KIND_CMD].extension, (STRPTR) arg4, TRUE, "");
                    ok = save_cmd((STRPTR) arg4, address1, address2, address3);
                acase TWIN:
                    fixextension(filekind[KIND_MOD].extension, (STRPTR) arg4, TRUE, "");
                    ok = save_mod((STRPTR) arg4, address1, address2, address3);
                acase CD2650:
                    fixextension(filekind[KIND_IMAG].extension, (STRPTR) arg4, TRUE, "");
                    ok = save_imag((STRPTR) arg4, address1, address2, address3);
                adefault:
                    ok = FALSE; // to avoid a spurious SAS/C warning
                }
                if (ok)
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                }
                cd_progdir();
    }   }   }
    elif (!stricmp((const char*) arg1, "SAVEHEX"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SAVEHEX].desc_id, menuinfo1[MENUITEM_SAVEHEX].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SAVEHEX, "Usage: SAVEHEX <start-address> <end-address> <filename>\n\n"));
        } elif (allowable(TRUE))
        {   if (arg2[0] && arg3[0] && arg4[0])
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                if (check_range(address1, address2))
                {   cd_projects();
                    fixextension(filekind[KIND_HEX].extension, (STRPTR) arg4, TRUE, "");
                    if (save_iof((STRPTR) arg4, address1, address2))
                    {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                    } else
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                    }
                    cd_progdir();
            }   }
            else
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SAVEHEX].desc_id, menuinfo1[MENUITEM_SAVEHEX].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SAVEHEX, "Usage: SAVEHEX <start-address> <end-address> <filename>\n\n"));
    }   }   }
    elif (!stricmp((const char*) arg1, "SAVESYM"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SAVESYM].desc_id, menuinfo1[MENUITEM_SAVESYM].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SAVESYM, "Usage: SAVESYM <filename>\n\n"));
        } elif (allowable(TRUE))
        {   if (!arg2[0] || arg3[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SAVESYM].desc_id, menuinfo1[MENUITEM_SAVESYM].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SAVESYM, "Usage: SAVESYM <filename>\n\n"));
            } else
            {   cd_projects();
                fixextension(filekind[KIND_SYM].extension, (STRPTR) arg2, TRUE, "");
                if ((TheLocalHandle = fopen((const char*) arg2, "wb")))
                {   savesym(TheLocalHandle);
                    DISCARD fclose(TheLocalHandle);
                    // TheLocalHandle = NULL;
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                }
                cd_progdir();
    }   }   }
    elif (!stricmp((const char*) arg1, "SAVETVC"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SAVETVC].desc_id, menuinfo1[MENUITEM_SAVETVC].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SAVETVC, "Usage: SAVETVC <start-address> <end-address> [<filename> [<game-start>]]\n\n"));
        } elif (machine != ELEKTOR)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
        } else
        {   if (!arg4[0])
            {   if (file_game[0])
                {   strcpy(arg4, file_game);
                } else
                {   strcpy(arg4, "Untitled");
            }   }
            if (arg2[0] && arg3[0] && arg4[0])
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                if (arg5[0])
                {   address3 = parse_expression((STRPTR) arg5, MAX_ADDR, FALSE);
                } else
                {   address3 = (startaddr_h * 256) + startaddr_l;
                }
                if
                (   address1 >  address2 // perhaps we might want a different error message for this case
                 || address1 == OUTOFRANGE
                 || address2 == OUTOFRANGE
                 || address3 == OUTOFRANGE
                )
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL(
                            MSG_INVALIDRANGE,
                            "Valid range is $%X..$%X!\n\n"
                        ),
                        0,
                        MAX_ADDR
                    );
                } else
                {   cd_projects(); // or perhaps cd_games()?
                    fixextension(filekind[KIND_TVC].extension, (STRPTR) arg4, TRUE, "");
                    if ((TheLocalHandle = fopen((const char*) arg4, "wb")))
                    {   tempdata[0] = 2;
                        tempdata[1] = address1 / 256;
                        tempdata[2] = address1 % 256;
                        tempdata[3] = address3 / 256;
                        tempdata[4] = address3 % 256;
                        DISCARD fwrite(tempdata, 5, 1, TheLocalHandle);
                        DISCARD fwrite(&memory[address1], address2 - address1 + 1, 1, TheLocalHandle);
                        DISCARD fclose(TheLocalHandle);
                        // TheLocalHandle = NULL;
                        zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                    } else
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n"));
                    }
                    cd_progdir();
            }   }
            else
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SAVETVC].desc_id, menuinfo1[MENUITEM_SAVETVC].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SAVETVC, "Usage: SAVETVC <start-address> <end-address> [<filename> [<game-start>]]\n\n"));
    }   }   }
    elif (!stricmp((const char*) arg1, "SWAP"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SWAP].desc_id, menuinfo1[MENUITEM_SWAP].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SWAP, "Usage: SWAP <1st-start> <1st-end> <2nd-start>\n\n"));
        } elif (allowable(TRUE))
        {   if (!arg2[0] || !arg3[0] || !arg4[0] || arg5[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_SWAP].desc_id, menuinfo1[MENUITEM_SWAP].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_SWAP, "Usage: SWAP <1st-start> <1st-end> <2nd-start>\n\n"));
            } else
            {   address1 = parse_expression((STRPTR) arg2, MAX_ADDR, FALSE);
                address2 = parse_expression((STRPTR) arg3, MAX_ADDR, FALSE);
                address3 = parse_expression((STRPTR) arg4, MAX_ADDR, FALSE);
                if
                (   address1 >  address2
                 || address1 == OUTOFRANGE
                 || address2 == OUTOFRANGE
                 || address3 == OUTOFRANGE
                )
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL(
                            MSG_INVALIDRANGE,
                            "Valid range is $%X..$%X!\n\n"
                        ),
                        0,
                        MAX_ADDR
                    );
                } else
                {   for (i = 0; i <= address2 - address1; i++)
                    {   t                    = memory[address1 + i];
                        memory[address1 + i] = memory[address3 + i];
                        memory[address3 + i] = t;
                    }
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
    }   }   }   }
    elif (!stricmp((const char*) arg1, "SYS") || !stricmp((const char*) arg1, "SYSTEM"))
    {   cleanexit(EXIT_SUCCESS);
    } elif (!stricmp((const char*) arg1, "T") || !stricmp((const char*) arg1, "SS"))
    {   if (allowable(TRUE))
        {   zprintf
            (   TEXTPEN_CLIOUTPUT,
                LLL(
                    MSG_ENGINE_TRACECPU,
                    "CPU tracing"
            )   );
            fliplog(&trace);
            traceorstep = (trace || step) ? TRUE : FALSE;
            if (trace && verbosity == 1)
            {   zprintf
                (   TEXTPEN_TRACE,
                    "Loca InOpAd Mnemoni XRU IAdd  IV EAdd    R0 R1 R2 R3 R4 R5 R6 PU PL IAR\n" \
                    "---- ------ ------- --- ----- -- ----    -- -- -- -- -- -- -- -- -- ----\n"
                );
            }
            if (!paused)
            {   crippled = TRUE;
                reset_fps();
                updatemenus();
    }   }   }
    elif (!stricmp((const char*) arg1, "TRAIN"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_TRAIN].desc_id, menuinfo1[MENUITEM_TRAIN].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, "Usage: TRAIN [<value>]\n\n");
        } elif (allowable(TRUE))
        {   if (arg3[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_TRAIN].desc_id, menuinfo1[MENUITEM_TRAIN].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, "Usage: TRAIN [<value>]\n\n");
            } elif (arg2[0])
            {   trainval = parse_expression((STRPTR) arg2, 255, FALSE);
                if (trainval == OUTOFRANGE)
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL
                        (   MSG_INVALIDRANGE,
                            "Valid range is $%X..$%X!\n\n"
                        ),
                        0,
                        255
                    );
                } else
                {   for (i = 0; i <= 0x7FFF; i++)
                    {   if
                        (   !(memflags[i] & (NOREAD | NOWRITE))
                         && memory[i] == (UBYTE) trainval
                        )
                        {   DISCARD getfriendly(i);
                            zprintf(TEXTPEN_CLIOUTPUT, "%s\n", friendly);
                            memflags[i] |= TRAINABLE;
                        } else
                        {   memflags[i] &= ~(TRAINABLE);
                    }   }
                    zprintf(TEXTPEN_CLIOUTPUT, "\n");
            }   }
            else
            {   found2 = FALSE;
                for (i = 0; i <= 0x7FFF; i++)
                {   if
                    (   (memflags[i] & TRAINABLE)
                     && memory[i] != (UBYTE) trainval
                    )
                    {   found2 = TRUE;
                        DISCARD getfriendly(i);
                        if (memory[i] > (UBYTE) trainval)
                        {   zprintf(TEXTPEN_CLIOUTPUT, "%s (+%3d) \n", friendly, memory[i] - trainval);
                        } else
                        {   zprintf(TEXTPEN_CLIOUTPUT, "%s (-%3d) \n", friendly, trainval - memory[i]);
                }   }   }
                if (!found2)
                {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NOTHINGFOUND, "Nothing found.\n"));
                }
                zprintf(TEXTPEN_CLIOUTPUT, "\n");
    }   }   }
    elif (!stricmp((const char*) arg1, "TU"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "TU 0: %s\n" \
                                       "TU 1: %s\n" \
                                       "TU 2: %s\n",
                                       LLL(menuinfo2[MENUOPT_TU_0].desc_id, menuinfo2[MENUOPT_TU_0].desc_str),
                                       LLL(menuinfo2[MENUOPT_TU_1].desc_id, menuinfo2[MENUOPT_TU_1].desc_str),
                                       LLL(menuinfo2[MENUOPT_TU_2].desc_id, menuinfo2[MENUOPT_TU_2].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, "Usage: TU 0|1|2\n\n");
        } elif (allowable(TRUE))
        {   if (arg2[0] < '0' || arg2[0] > '2' || arg2[1] || arg3[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "TU 0: %s\n" \
                                           "TU 1: %s\n" \
                                           "TU 2: %s\n",
                                           LLL(menuinfo2[MENUOPT_TU_0].desc_id, menuinfo2[MENUOPT_TU_0].desc_str),
                                           LLL(menuinfo2[MENUOPT_TU_1].desc_id, menuinfo2[MENUOPT_TU_1].desc_str),
                                           LLL(menuinfo2[MENUOPT_TU_2].desc_id, menuinfo2[MENUOPT_TU_2].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, "Usage: TU 0|1|2\n\n");
            } else
            {   timeunit = arg2[0] - '0';
                changetimeunitnames();
                update_opcodes();
                if (SubWindowPtr[SUBWINDOW_MONITOR_CPU])
                {   close_subwindow(SUBWINDOW_MONITOR_CPU);
                    view_monitor(SUBWINDOW_MONITOR_CPU);
                }
                zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
#ifdef AMIGA
                debugger_enter();
#endif
    }   }   }
    elif (!stricmp((const char*) arg1, "VB") || !stricmp((const char*) arg1, "VERBOSE"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "VB|VERBOSE 0: %s\n" \
                                       "VB|VERBOSE 1: %s\n" \
                                       "VB|VERBOSE 2: %s\n",
                                       LLL(menuinfo2[MENUOPT_VB_0].desc_id, menuinfo2[MENUOPT_VB_0].desc_str),
                                       LLL(menuinfo2[MENUOPT_VB_1].desc_id, menuinfo2[MENUOPT_VB_1].desc_str),
                                       LLL(menuinfo2[MENUOPT_VB_2].desc_id, menuinfo2[MENUOPT_VB_2].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, "Usage: VB 0|1|2\n\n");
        } elif (allowable(TRUE))
        {   if (arg2[0] < '0' || arg2[0] > '2' || arg2[1] || arg3[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "VB|VERBOSE 0: %s\n" \
                                       "VB|VERBOSE 1: %s\n" \
                                       "VB|VERBOSE 2: %s\n",
                                       LLL(menuinfo2[MENUOPT_VB_0].desc_id, menuinfo2[MENUOPT_VB_0].desc_str),
                                       LLL(menuinfo2[MENUOPT_VB_1].desc_id, menuinfo2[MENUOPT_VB_1].desc_str),
                                       LLL(menuinfo2[MENUOPT_VB_2].desc_id, menuinfo2[MENUOPT_VB_2].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, "Usage: VB 0|1|2\n\n");
            } else
            {   verbosity = arg2[0] - '0';
                switch (verbosity)
                {
                case  0: zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_VB_0, "Minimum verbosity.\n\n" ));
                acase 1: zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_VB_1, "Table tracing mode.\n\n"));
                acase 2: zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_VB_2, "Maximum verbosity.\n\n" ));
    }   }   }   }
    elif (!stricmp((const char*) arg1, "WC") || !stricmp((const char*) arg1, "DT"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_WC].desc_id, menuinfo1[MENUITEM_WC].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_WC, "Usage: WC [<start-address> [<end-address>]]\n\n"));
        } elif (allowable(TRUE))
        {   if (arg4[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_WC].desc_id, menuinfo1[MENUITEM_WC].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_WC, "Usage: WC [<start-address> [<end-address>]]\n\n"));
            } elif (arg3[0])
            {   address1 = parse_expression((STRPTR) arg2, LASTTOKEN, FALSE);
                address2 = parse_expression((STRPTR) arg3, LASTTOKEN, FALSE);
                if
                (   address1 != OUTOFRANGE
                 && address2 != OUTOFRANGE
                 && address1 <= address2
                )
                {   for (i = address1; i <= address2; i++)
                    {   memflags[i] &= ~(WATCHPOINT);
                    }
                    update_memory(TRUE);
                    update_monitor(TRUE);
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                } else
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL(
                            MSG_INVALIDRANGEWP,
                            "Valid range is $0000..$7FFF and %s..%s!\n\n"
                        ),
                        flagname[NAME_R0].shorter[style],
                        flagname[NAME_R6].shorter[style]
                    );
            }   }
            elif (arg2[0])
            {   address1 = parse_expression((STRPTR) arg2, LASTTOKEN, FALSE);
                if (address1 < 0 || address1 > LASTTOKEN)
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL(
                            MSG_INVALIDRANGEWP,
                            "Valid range is $0000..$7FFF and %s..%s!\n\n"
                        ),
                        flagname[NAME_R0].shorter[style],
                        flagname[NAME_R6].shorter[style]
                    );
                } else
                {   ok = FALSE;
                    if (memflags[address1] & WATCHPOINT)
                    {   memflags[address1] &= ~(WATCHPOINT);
                        ok = TRUE;
                    }
                    DISCARD getfriendly(address1);
                    if (ok)
                    {   zprintf
                        (   TEXTPEN_CLIOUTPUT,
                            LLL
                            (   MSG_ENGINE_CLEAREDWP,
                                "Cleared data watchpoint at %s.\n\n"
                            ),
                            friendly
                        );
                        update_memory(TRUE);
                        update_monitor(TRUE);
                    } else
                    {   zprintf
                        (   TEXTPEN_CLIOUTPUT,
                            LLL
                            (   MSG_ENGINE_WPNOTFOUND,
                                "Watchpoint not found at %s!\n\n"
                            ),
                            friendly
                        );
            }   }   }
            else
            {   ok = FALSE;
                for (address1 = 0; address1 <= LASTTOKEN; address1++)
                {   if (memflags[address1] & WATCHPOINT)
                    {   memflags[address1] &= ~(WATCHPOINT);
                        ok = TRUE;
                }   }
                if (ok)
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL
                        (   MSG_ENGINE_CLEAREDWPS,
                            "Cleared all data watchpoints.\n\n"
                        )
                    );
                    update_memory(TRUE);
                    update_monitor(TRUE);
                } else
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL
                        (   MSG_NOWPS,
                            "No watchpoints to clear!\n\n"
                        )
                    );
    }   }   }   }
    elif (!stricmp((const char*) arg1, "WL") || !stricmp((const char*) arg1, "LT"))
    {   if (allowable(TRUE))
        {   ok = FALSE;
            for (i = 0; i <= LASTTOKEN; i++)
            {   if (memflags[i] & WATCHPOINT)
                {   DISCARD getfriendly(i);
                    zprintf(TEXTPEN_CLIOUTPUT, "%s", friendly);

                    switch (wp[i].the2nd)
                    {
                    case COND_UN:
                        zprintf(TEXTPEN_CLIOUTPUT, "\n");
                    acase COND_MASK:
                        dec_to_bin((UBYTE) wp[i].the3rd);
                        zprintf(TEXTPEN_CLIOUTPUT, " ");
                        zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_BITSONLY, "(bits %s only)\n"), v_bin);
                    adefault:
                        makestrings(&wp[i]);
                        zprintf
                        (   TEXTPEN_CLIOUTPUT,
                            " %s %s %s %s\n",
                            LLL(MSG_WHEN, "when"),
                            firststring,
                            condstring,
                            secondstring
                        );
                    }
                    ok = TRUE;
            }   }
            if (ok)
            {   zprintf(TEXTPEN_CLIOUTPUT, "\n");
            } else
            {   zprintf
                (   TEXTPEN_CLIOUTPUT,
                    "%s.\n\n",
                    LLL(
                        MSG_NONE,
                        "None"
                )   );
    }   }   }
    elif (!stricmp((const char*) arg1, "WP") || !stricmp((const char*) arg1, "ST"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_WP].desc_id, menuinfo1[MENUITEM_WP].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_WP, "Usage: WP <start-addr> [<end-addr>] [<addr/reg> <condition> <value>]\n\n"));
        } elif (allowable(TRUE))
        {   if (arg7[0] || !arg2[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_WP].desc_id, menuinfo1[MENUITEM_WP].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_WP, "Usage: WP <start-addr> [<end-addr>] [<addr/reg> <condition> <value>]\n\n"));
            } elif (arg6[0])
            {   // WP <start-address> <end-address> <address/register> <condition> <value>
                address1 = parse_expression((STRPTR) arg2, LASTTOKEN, FALSE);
                address2 = parse_expression((STRPTR) arg3, LASTTOKEN, FALSE);
                the1st   = parse_expression((STRPTR) arg4, ALLTOKENS, FALSE);
                the2nd   = string_to_cond(  (STRPTR) arg5);
                address3 = parse_expression((STRPTR) arg6, MAX_ADDR,  FALSE); // not really an address
                if
                (   address1 != OUTOFRANGE
                 && address2 != OUTOFRANGE
                 && address3 != OUTOFRANGE
                 && address1 <= address2
                )
                {   for (i = address1; i <= address2; i++)
                    {   DISCARD wp_add(i, the1st, the2nd, address3, 0xFF, FALSE);
                    }
                    update_memory(TRUE);
                    if (!watchreads && !watchwrites)
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_WPWARNING, "Warning: watching of reads and writes is currently turned off! (WR and WW to enable.)\n"));
                    }
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_WP].desc_id, menuinfo1[MENUITEM_WP].desc_str));
                    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_WP, "Usage: WP <start-addr> [<end-addr>] [<addr/reg> <condition> <value>]\n\n"));
            }   }
            elif (arg5[0])
            {   // WP <start-address> <address/register> <condition> <value>
                address1 = parse_expression((STRPTR) arg2, LASTTOKEN, FALSE);
                the1st   = parse_expression((STRPTR) arg3, ALLTOKENS, FALSE);
                the2nd   = string_to_cond((STRPTR) arg4);
                address3 = parse_expression((STRPTR) arg5, MAX_ADDR, FALSE); // not really an address
                DISCARD wp_add(address1, the1st, the2nd, address3, 0xFF, TRUE);
            } elif (arg4[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_WP].desc_id, menuinfo1[MENUITEM_WP].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_WP, "Usage: WP <start-addr> [<end-addr>] [<addr/reg> <condition> <value>]\n\n"));
            } elif (arg3[0])
            {   // WP <start-address> <end-address>
                address1 = parse_expression((STRPTR) arg2, LASTTOKEN, FALSE);
                address2 = parse_expression((STRPTR) arg3, LASTTOKEN, FALSE);
                if
                (   address1 != OUTOFRANGE
                 && address2 != OUTOFRANGE
                 && address1 <= address2
                )
                {   for (i = address1; i <= address2; i++)
                    {   DISCARD wp_add(i, 0, COND_UN, 0, 0xFF, FALSE);
                    }
                    update_memory(TRUE);
                    if (!watchreads && !watchwrites)
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_WPWARNING, "Warning: watching of reads and writes is currently turned off! (WR and WW to enable.)\n"));
                    }
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_WP].desc_id, menuinfo1[MENUITEM_WP].desc_str));
                    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_WP, "Usage: WP <start-addr> [<end-addr>] [<addr/reg> <condition> <value>]\n\n"));
            }   }
            else
            {   // WP <start-address>[:mask]
                address1 = parse_expression((STRPTR) arg2, LASTTOKEN, TRUE);
                DISCARD wp_add(address1, 0, COND_UN, 0, watchmask, TRUE);
    }   }   }
    elif (!stricmp((const char*) arg1, "WR"))
    {   if (allowable(TRUE))
        {   if (watchreads)
            {   watchreads = FALSE;
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_WR_OFF, "Watch reads off.\n\n"));
            } else
            {   watchreads = TRUE;
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_WR_ON,  "Watch reads on.\n\n" ));
    }   }   }
    elif (!stricmp((const char*) arg1, "WW"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "WW NONE: %s\n" \
                                       "WW SOME: %s\n" \
                                       "WW ALL:  %s\n",
                                       LLL(menuinfo2[MENUOPT_WW_NONE].desc_id, menuinfo2[MENUOPT_WW_NONE].desc_str),
                                       LLL(menuinfo2[MENUOPT_WW_SOME].desc_id, menuinfo2[MENUOPT_WW_SOME].desc_str),
                                       LLL(menuinfo2[MENUOPT_WW_ALL ].desc_id, menuinfo2[MENUOPT_WW_ALL ].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, "%s: WW NONE|SOME|ALL\n", LLL(MSG_USAGE, "Usage"));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_CURRENTSETTING, "Current setting is %s.\n\n"), wwname[watchwrites]);
        } elif (allowable(TRUE))
        {   if (arg2[0])
            {   if   (!stricmp((const char*) arg2, "NONE") || !stricmp((const char*) arg2, "N") || !strcmp((const char*) arg2, "0") || !stricmp((const char*) arg2, "OFF")) { watchwrites = WATCH_NONE; zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK")); }
                elif (!stricmp((const char*) arg2, "SOME") || !stricmp((const char*) arg2, "S") || !strcmp((const char*) arg2, "1")                                 ) { watchwrites = WATCH_SOME; zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK")); }
                elif (!stricmp((const char*) arg2, "ALL" ) || !stricmp((const char*) arg2, "A") || !strcmp((const char*) arg2, "2")                                 ) { watchwrites = WATCH_ALL ; zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK")); }
                else
                {   zprintf(TEXTPEN_CLIOUTPUT, "WW NONE: %s\n" \
                                               "WW SOME: %s\n" \
                                               "WW ALL:  %s\n",
                                               LLL(menuinfo2[MENUOPT_WW_NONE].desc_id, menuinfo2[MENUOPT_WW_NONE].desc_str),
                                               LLL(menuinfo2[MENUOPT_WW_SOME].desc_id, menuinfo2[MENUOPT_WW_SOME].desc_str),
                                               LLL(menuinfo2[MENUOPT_WW_ALL ].desc_id, menuinfo2[MENUOPT_WW_ALL ].desc_str));
                    zprintf(TEXTPEN_CLIOUTPUT, "%s: WW NONE|SOME|ALL\n", LLL(MSG_USAGE, "Usage"));
                    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_CURRENTSETTING, "Current setting is %s.\n\n"), wwname[watchwrites]);
            }   }
            else
            {   zprintf(TEXTPEN_CLIOUTPUT, "WW NONE: %s\n" \
                                           "WW SOME: %s\n" \
                                           "WW ALL:  %s\n",
                                           LLL(menuinfo2[MENUOPT_WW_NONE].desc_id, menuinfo2[MENUOPT_WW_NONE].desc_str),
                                           LLL(menuinfo2[MENUOPT_WW_SOME].desc_id, menuinfo2[MENUOPT_WW_SOME].desc_str),
                                           LLL(menuinfo2[MENUOPT_WW_ALL ].desc_id, menuinfo2[MENUOPT_WW_ALL ].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, "%s: WW NONE|SOME|ALL\n", LLL(MSG_USAGE, "Usage"));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_CURRENTSETTING, "Current setting is %s.\n\n"), wwname[watchwrites]);
    }   }   }
    elif (!stricmp((const char*) arg1, "WRITEPORT"))
    {   if (ASKUSAGE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_WRITEPORT].desc_id, menuinfo1[MENUITEM_WRITEPORT].desc_str));
            zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_WRITEPORT, "Usage: WRITEPORT <port>|CTRL|DATA <value>\n\n"));
        } elif (allowable(TRUE))
        {   if (!arg3[0] || arg4[0])
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_WRITEPORT].desc_id, menuinfo1[MENUITEM_WRITEPORT].desc_str));
                zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_WRITEPORT, "Usage: WRITEPORT <port>|CTRL|DATA <value>\n\n"));
            } else
            {   address2 = readbase((STRPTR) arg3, FALSE);
                if (!stricmp(arg2, "CTRL"))
                {   writeport(PORTC, (UBYTE) address2);
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                } elif (!stricmp(arg2, "DATA"))
                {   writeport(PORTD, (UBYTE) address2);
                    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                } else
                {   address1 = readbase((STRPTR) arg2, FALSE);
                    if (address1 >= 0 && address1 <= 255)
                    {   writeport(address1, (UBYTE) address2);
                        zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
                    } else
                    {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n", LLL(menuinfo1[MENUITEM_WRITEPORT].desc_id, menuinfo1[MENUITEM_WRITEPORT].desc_str));
                        zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_USAGE_WRITEPORT, "Usage: WRITEPORT <port>|CTRL|DATA <value>\n\n"));
    }   }   }   }   }
    else
    {   zprintf
        (   TEXTPEN_CLIOUTPUT,
            LLL(
                MSG_ENGINE_UNKNOWNCOMMAND,
                "Unknown command \"%s\" (\"H\" for help).\n\n"
            ),
            strupr((char*) arg1)
        );
    }

    return FALSE;
}

MODULE void help_commands(int which)
{   switch (which)
    {
    case 0:
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            GetCatalogStr(CatalogPtr, MSG_ENGINE_DEBUGHELP,
"General:\n"\
"\"<message>                      send message to other player\n"\
"CLEARCOV                        clear coverage report\n"\
"CLS                             clear screen (of console window)\n"\
"H|HELP|? [0|G]                  view general help\n"\
"H|HELP|? 1|F                    view file help\n"\
"H|HELP|? 2|D                    view disk help\n"\
"H|HELP|? 3|E                    view edit help\n"\
"H|HELP|? 4|V                    view view help\n"\
"H|HELP|? 5|L                    view log help\n"\
"H|HELP|? 6|R                    view run help\n"\
"H|HELP|? 7|B                    view breakpoints help\n"\
"H|HELP|? 8|S                    view symbols help\n"\
"H|HELP|? 9|T                    view tools help\n"\
"H|HELP|? 10|O                   view options help\n"\
"H|HELP|? 11|A                   view graphics help\n"\
"SAY|SPEAK|SPK <message>         speak message\n"\
"SYS|SYSTEM                      quit emulator\n\n"
        ));
    acase 1:
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            GetCatalogStr(CatalogPtr, MSG_DEBUGHELP1,
"File:\n"\
"*<command>                      execute host OS command\n"\
"ASM <filename>                  assemble source code\n"\
"DISGAME [<start-addr> <end-addr>] [<filename>]\n"\
"                                disassemble and show address range\n"\
"ED|EDIT <filename>              edit source code\n"\
"LOADBIN <start-addr> <filename> load binary to start-addr from filename\n"\
"SAVEAOF|SAVEEOF\n"\
" <start-addr> <end-addr> [<filename> [<game-start> [<block-size>]]]\n"\
"SAVECMD|SAVEIMAG|SAVEMOD|SAVETVC\n"\
" <start-addr> <end-addr> [<filename> [<game-start>]]\n"\
"SAVEBIN|SAVEBPNF|SAVEHEX <start-addr> <end-addr> <filename>\n"\
"                                save address range as filename\n"\
"SAVESMS <end-addr> <filename>   save address $0 to end-addr as filename\n\n"
        ));
    acase 2:
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            GetCatalogStr(CatalogPtr, MSG_DEBUGHELP2,
"Disk:\n"\
"DEL|DELETE <filename>           delete file from floppy disk\n"\
"DIR|LD|LDIR                     list files on floppy disk\n"\
"DRIVE [<drive>]                 change debugger drive\n"\
"EXTRACT [<filename>]            extract files from floppy disk\n"\
"INJECT <filename>               inject files onto floppy disk\n"\
"REN|RENAME <filename>           rename file on floppy disk\n\n"
        ));
    acase 3:
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            GetCatalogStr(CatalogPtr, MSG_DEBUGHELP3,
"Edit:\n"\
"DOKE <address> [<value>]        change word at address to value\n"\
"E|POKE [<address> [<value>]]    change byte at address to value\n"\
"FPOKE <address> <value>         change byte on disk to value\n"\
"WRITEPORT <port>|CTRL|DATA <value>\n"\
"                                write to I/O port\n\n"
        ));
    acase 4:
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            GetCatalogStr(CatalogPtr, MSG_DEBUGHELP4,
"View:\n"\
"=                               list all known labels\n"\
"= <addr>                        view addr as label and hexadecimal\n"\
"COVER [<reporttype>]            view coverage report\n"\
"D|PEEK <start-addr> [<end-addr>]\n"\
"                                display address range as data\n"\
"DIS [<start-addr> [<end-addr>]] disassemble address range as code\n"\
"ERROR <number>                  show error message\n"\
"FPEEK <start-addr> [<end-addr>] display address range from disk\n"\
"HISTORY                         view command history\n"\
"IM [<start-addr> <end-addr>] [<filename>]\n"\
"                                view address range as imagery\n"\
"READPORT <port>|CTRL|DATA       read from I/O port\n"\
"V|VIEW BASIC                    view BASIC variables\n"\
"V|VIEW BIOS                     view BIOS RAM\n"\
"V|VIEW CPU                      view CPU status\n"\
"V|VIEW PSG                      view PSGs/TMSes status\n"\
"V|VIEW RAM                      view RAM\n"\
"V|VIEW SCRN                     view screen contents\n"\
"V|VIEW UDG                      view user-defined graphics\n"\
"V|VIEW XVI                      view UVI/PVI status\n\n"
        ));
    acase 5:
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            GetCatalogStr(CatalogPtr, MSG_DEBUGHELP5,
"Log:\n"\
"L A                             log illegal memory accesses on/off\n"\
"L B                             log BIOS calls on/off\n"\
"L C                             log inefficient code on/off\n"\
"L I                             log illegal instructions on/off\n"\
"L N                             log interrupts on/off\n"\
"L S                             log subroutine calls on/off\n"\
"PL                              pause after logging on/off\n"\
"T                               trace CPU on/off\n\n"
        ));
    acase 6:
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            GetCatalogStr(CatalogPtr, MSG_DEBUGHELP6,
"Run:\n"\
"G|P [<address>]                 pause/unpause\n" \
"GI                              generate an interrupt\n"\
"I                               ignore next instruction\n"\
"J|JUMP <address>                jump to address\n"\
"O                               step over subroutine\n"\
"R <number>                      run to rasterline number\n"\
"R F                             run to next frame\n"\
"R I                             run to next interrupt\n"\
"R L                             run to end of loop\n"\
"R R                             run to next rasterline\n"\
"R S                             run to end of subroutine\n"\
"S                               step next instruction\n\n"
        ));
    acase 7:
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            GetCatalogStr(CatalogPtr, MSG_DEBUGHELP7,
"Breakpoints:\n"\
"BC|FC|IC|WC [<start-addr> [<end-addr>]]\n"\
"                                clear bp/fp/ip/wp from address/port range\n"\
"BL|FL|IL|WL                     list all breakpoints/watchpoints\n"\
"BP|WP <start-addr> [<end-addr>] [<addr/reg> <condition> <value>]\n"\
"                                add bp/wp to address range\n"\
"FP <start-addr> [<end-addr>]    add disk watchpoint to address range\n"\
"IP [<start-port> [<end-port>] [<addr/reg> <condition> <value>]]\n"\
"                                add I/O port wp to port range\n"\
"PB                              pause after breakpoints/watchpoints on/off\n"\
"WR                              watch reads on/off\n"\
"WW NONE|SOME|ALL                watch none/some/all writes\n\n"
        ));
    acase 8:
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            GetCatalogStr(CatalogPtr, MSG_DEBUGHELP8,
"Symbols:\n"\
"CLEARSYM [<label>]              clear symbol(s)\n"\
"DEFSYM <label> <address> [C|D|P|U]\n"\
"                                define symbol\n"\
"LOADSYM [<filename>]            load symbol table from filename\n"\
"SAVESYM <filename>              save symbol table as filename\n\n"
        ));
    acase 9:
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            GetCatalogStr(CatalogPtr, MSG_DEBUGHELP9,
"Tools:\n"\
"CO|COMP <1st-start> <1st-end> <2nd-start>\n"\
"                                compare memory blocks\n"\
"COPY|MOVE <source-start> <source-end> <target-start>\n"\
"                                copy memory from source to target\n"\
"FILL <start-addr> <end-addr> <value>\n"\
"                                fill address range with value\n"\
"FIND [[<start-addr> <end-addr>] <value>]\n"\
"                                find value in address range\n"\
"REL <source-addr> [*]<target-addr>\n"\
"                                calculate relative offset of target-addr\n"\
"SWAP <1st-start> <1st-end> <2nd-start>\n"\
"                                swap memory blocks\n"\
"TRAIN [<value>]                 make trainer\n\n"
        ));
    acase 10:
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            GetCatalogStr(CatalogPtr, MSG_DEBUGHELP10,
"Options:\n"\
"BASE [2|8|10|16|%%|@|!|$]        view/set default input base\n"\
"CPU 0|1                         change CPU (2650/2650A vs. 2650B)\n"\
"GR                              use guide ray on/off\n" \
"N 0|1|2                         change notation\n"\
"SPR 0|1|2                       change sprite display mode\n"\
"TU 0|1|2                        change time unit\n"\
"VB|VERBOSE 0|1|2                change verbosity\n\n"
        ));
    acase 11:
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            GetCatalogStr(CatalogPtr, MSG_DEBUGHELP11,
"Graphics:\n"\
"DRAW 0|1|2|3                    change drawing mode\n"\
"SPR 0|1|2                       change sprite display mode\n\n"
        ));
}   }

EXPORT void loadsym(TEXT* bufptr)
{   int   address   = 0,
          i;
    UBYTE labelkind = UNKNOWN;
    TEXT  tempstring[LABELLIMIT + 1];

    // assert(bufptr);

    symoffset = 0;
    while (symoffset < symsize)
    {   skipws(bufptr);
        if (bufptr[symoffset] == ';')
        {   skipline(bufptr);
        } elif
        (   !strnicmp(&bufptr[symoffset], "addrtab"  , 7)
         || !strnicmp(&bufptr[symoffset], "byte"     , 4)
         || !strnicmp(&bufptr[symoffset], "code"     , 4)
         || !strnicmp(&bufptr[symoffset], "string"   , 6)
         || !strnicmp(&bufptr[symoffset], "symbol"   , 6)
         || !strnicmp(&bufptr[symoffset], "vectab"   , 6)
         || !strnicmp(&bufptr[symoffset], "vector"   , 6)
         || !strnicmp(&bufptr[symoffset], "word"     , 4)
        )
        {   skipnonws(bufptr);
            skipws(bufptr);
            if (bufptr[symoffset] == '0')
            {   if (bufptr[symoffset + 1] == 'x')
                {   symoffset += 2;
                    address = readnum_hex(&bufptr[symoffset], 4);
                } else
                {   symoffset++;
                    address = readnum_octal(&bufptr[symoffset], 5);
            }   }
            elif (bufptr[symoffset] >= '1' && bufptr[symoffset] <= '9')
            {   address = readnum_decimal(&bufptr[symoffset], 5);
            }

            if
            (   !strnicmp(&bufptr[symoffset], "code"     , 4) // code
            )
            {   labelkind = CODE;
            } elif
            (   !strnicmp(&bufptr[symoffset], "vector"   , 6) // single pointer to code
             || !strnicmp(&bufptr[symoffset], "vectab"   , 6) // multiple pointers to code
            )
            {   labelkind = POINTER;
            } elif
            (   !strnicmp(&bufptr[symoffset], "addrtab"  , 7) // multiple pointers to data
             || !strnicmp(&bufptr[symoffset], "byte"     , 4) // one or more data bytes
             || !strnicmp(&bufptr[symoffset], "string"   , 6) // one or more data bytes
             || !strnicmp(&bufptr[symoffset], "word"     , 4) // two or more data bytes
            )
            {   labelkind = DATA;
            } elif
            (   !strnicmp(&bufptr[symoffset], "symbol"   , 6) // anything
            )
            {   labelkind = UNKNOWN;
            }

            skipnonws(bufptr);
            skipws(bufptr);
            // parse label
            i = 0;
            while
            (   bufptr[symoffset] != CR
             && bufptr[symoffset] != LF
             && bufptr[symoffset] != ' '
             && bufptr[symoffset] != HT
             && i < LABELLIMIT
            )
            {   tempstring[i] = bufptr[symoffset];
                symoffset++;
                i++;
            }
            tempstring[i] = EOS;
            skipline(bufptr);

            if (address == OUTOFRANGE)
            {   zprintf
                (   TEXTPEN_ERROR,
                    LLL
                    (   MSG_OUTOFRANGE,
                        "Label %s is out of range!\n"
                    ),
                    tempstring
                );
                zprintf
                (   TEXTPEN_CLIOUTPUT,
                    LLL
                    (   MSG_INVALIDRANGE,
                        "Valid range is $%X..$%X!\n\n"
                    ),
                    0,
                    0x7FFF
                );
            } else
            {   adduserlabel((STRPTR) tempstring, (UWORD) address, labelkind);
        }   }
        else /* if
        (   !strnicmp(&bufptr[symoffset], "cpu"      , 3)
         || !strnicmp(&bufptr[symoffset], "include"  , 7)
         || !strnicmp(&bufptr[symoffset], "message"  , 7)
         || !strnicmp(&bufptr[symoffset], "numformat", 9)
         || !strnicmp(&bufptr[symoffset], "org"      , 3)
         || !strnicmp(&bufptr[symoffset], "skip"     , 4)
        ) */
        {   skipline(bufptr);
}   }   }

EXPORT void adduserlabel(STRPTR name, UWORD address, TEXT kind)
{   int  i,
         length;
    TEXT errorstring[80 + 1];

    length = strlen(name);
    if (length == 0)
    {   strcpy((char*) errorstring, "symbol has zero-length name");
        goto PRINTERROR;
    }
    if (length > LABELLIMIT)
    {   strcpy((char*) errorstring, "symbol name is too long");
        goto PRINTERROR;
    }

    strcpy((char*) symlabel[userlabels].name, name);
    strupr((char*) symlabel[userlabels].name);
    symlabel[userlabels].address = address;
    symlabel[userlabels].kind    = kind;

    for (i = 0; i < length; i++)
    {   if
        (   (symlabel[userlabels].name[i] >= 'A' && symlabel[userlabels].name[i] <= 'Z'          )
         || (symlabel[userlabels].name[i] >= '0' && symlabel[userlabels].name[i] <= '9' && i >= 1)
         ||  symlabel[userlabels].name[i] == '_'
        )
        {   ;
        } else
        {   sprintf((char*) errorstring, "illegal symbol name \"%s\"", symlabel[userlabels].name);
            goto PRINTERROR;
    }   }

    if (userlabels)
    {   for (i = 0; i < userlabels; i++)
        {   if (!stricmp((const char*) symlabel[userlabels].name, (const char*) symlabel[i].name))
            {   if (symlabel[userlabels].address != symlabel[i].address || symlabel[userlabels].kind != symlabel[i].kind)
                {   zprintf
                    (   TEXTPEN_CLIOUTPUT,
                        LLL(MSG_REDEFININGLABEL, "Redefining user-defined label %s (was $%X (%c), now $%X (%c)).\n"),
                        symlabel[userlabels].name,
                        symlabel[i].address,
                        symlabel[i].kind,
                        symlabel[userlabels].address,
                        symlabel[userlabels].kind
                    );
                    symlabel[i].address = symlabel[userlabels].address;
                    symlabel[i].kind    = symlabel[userlabels].kind;
                    // name doesn't need copying as both are the same
                }
                return;
    }   }   }
    // If we they redefine it to be the same as its built-in value, we should ideally remove the entry altogether

    if (userlabels >= SYMLABELS_MAX)
    {   strcpy((char*) errorstring, "too many user-defined symbols");
        goto PRINTERROR;
    }

    for (i = FIRSTCPUEQUIV; i <= LASTCPUEQUIV; i++)
    {   if (!stricmp((const char*) symlabel[userlabels].name, (const char*) equivalents[i].name))
        {   if (symlabel[userlabels].address == equivalents[i].address)
            {   goto ZIGNORE;
            } else
            {   goto OVERRIDE;
    }   }   }
    if (machines[machine].firstequiv != -1)
    {   for (i = machines[machine].firstequiv; i <= machines[machine].lastequiv; i++)
        {   if (!stricmp((const char*) symlabel[userlabels].name, (const char*) equivalents[i].name))
            {   if (symlabel[userlabels].address == equivalents[i].address)
                {   goto ZIGNORE;
                } else
                {   goto OVERRIDE;
    }   }   }   }
    if (machine == BINBUG)
    {   if (firstdosequiv != -1)
        {   for (i = firstdosequiv; i <= lastdosequiv; i++)
            {   if (!stricmp((const char*) symlabel[userlabels].name, (const char*) equivalents[i].name))
                {   if (symlabel[userlabels].address == equivalents[i].address)
                    {   goto ZIGNORE;
                    } else
                    {   goto OVERRIDE;
        }   }   }   }
        for (i = FIRSTACOSEQUIV; i <= LASTACOSEQUIV; i++)
        {   if (!stricmp((const char*) symlabel[userlabels].name, (const char*) equivalents[i].name))
            {   if (symlabel[userlabels].address == equivalents[i].address)
                {   goto ZIGNORE;
                } else
                {   goto OVERRIDE;
    }   }   }   }
    if (whichgame != -1 && known[whichgame].firstequiv != -1)
    {   for (i = FIRSTGAMEEQUIV + known[whichgame].firstequiv; i <= FIRSTGAMEEQUIV + known[whichgame].lastequiv; i++)
        {   if (!stricmp((const char*) symlabel[userlabels].name, (const char*) equivalents[i].name))
            {   if (symlabel[userlabels].address == equivalents[i].address)
                {   goto ZIGNORE;
                } else
                {   goto OVERRIDE;
    }   }   }   }
    if (machines[machine].pvis)
    {   for (i = FIRSTPVIEQUIV; i < FIRSTPVIEQUIV + (33 * machines[machine].pvis); i++)
        {   if (!stricmp((const char*) symlabel[userlabels].name, (const char*) equivalents[i].name))
            {   if (symlabel[userlabels].address == pvibase + equivalents[i].address)
                {   goto ZIGNORE;
                } else
                {   goto OVERRIDE;
    }   }   }   }

    zprintf
    (   TEXTPEN_CLIOUTPUT,
        LLL(MSG_ADDINGLABEL, "Adding user-defined %s label %s as $%X.\n"),
        labelstring2(symlabel[userlabels].kind),
        symlabel[userlabels].name,
        symlabel[userlabels].address
    );
    userlabels++;
    return;

ZIGNORE:
    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_IGNORINGLABEL  , "Ignoring redundant definition of %s as $%X.\n"), symlabel[userlabels].name, symlabel[userlabels].address);
    return;

OVERRIDE:
    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_OVERRIDINGLABEL, "Overriding built-in label %s as $%X.\n"       ), symlabel[userlabels].name, symlabel[userlabels].address);
    userlabels++;
    return;

PRINTERROR:
    if (assembling)
    {   // assert(pass == 1);
        pass = 2;
        asmerror((STRPTR) errorstring);
        pass = 1;
    } else
    {   zprintf(TEXTPEN_CLIOUTPUT, "Error: %s\n", errorstring);
}   }

MODULE STRPTR labelstring1(TEXT whichkind)
{   // Strings for assemblers
    switch (whichkind)
    {
    case CODE:    return "code  ";
    case DATA:    return "data  ";
    case POINTER: return "vector";
    default:      return "symbol";
}   }

EXPORT void savesym(FILE* symhandle)
{   int i;

    fprintf(symhandle, ";CPU flags:\n");
    for (i = FIRSTCPUEQUIV; i <= LASTCPUEQUIV; i++)
    {   fprintf(        symhandle, "%s          0x%04X  %s\n", labelstring1(equivalents[i].kind),                  equivalents[i].address , equivalents[i].name);
    }

    if (machines[machine].firstequiv != -1 || machines[machine].pvis)
    {   fprintf(symhandle, ";Guest-specific:\n");
        if (machines[machine].firstequiv != -1)
        {   for (i = machines[machine].firstequiv; i <= machines[machine].lastequiv; i++)
            {   fprintf(symhandle, "%s          0x%04X  %s\n", labelstring1(equivalents[i].kind),                  equivalents[i].address , equivalents[i].name);
        }   }

        if (machine == BINBUG && firstdosequiv != -1)
        {   for (i = firstdosequiv; i <= lastdosequiv; i++)
            {   fprintf(symhandle, "%s          0x%04X  %s\n", labelstring1(equivalents[i].kind),                  equivalents[i].address , equivalents[i].name);
        }   }

        if (machines[machine].pvis)
        {   for (i = FIRSTPVIEQUIV; i < FIRSTPVIEQUIV + (33 * machines[machine].pvis); i++)
            {   fprintf(symhandle, "%s          0x%04X  %s\n", labelstring1(equivalents[i].kind), (int) (pvibase + equivalents[i].address), equivalents[i].name);
    }   }   }

    if (whichgame != -1 && known[whichgame].firstequiv != -1)
    {   fprintf(symhandle, ";Game-specific:\n");
        for (i = FIRSTGAMEEQUIV + known[whichgame].firstequiv; i <= FIRSTGAMEEQUIV + known[whichgame].lastequiv; i++)
        {   fprintf(    symhandle, "%s          0x%04X  %s\n", labelstring1(equivalents[i].kind),                  equivalents[i].address , equivalents[i].name);
    }   }

    if (userlabels)
    {   fprintf(symhandle, ";User-defined:\n");
        for (i = 0; i < userlabels; i++)
        {   if (!assembling || symlabel[i].used)
            {   fprintf(        symhandle, "%s          0x%04X  %s\n",            labelstring1(symlabel[   i].kind),                  symlabel[   i].address , symlabel[   i].name);
            } else
            {   fprintf(        symhandle, "%s          0x%04X  %-17s ;Unused\n", labelstring1(symlabel[   i].kind),                  symlabel[   i].address , symlabel[   i].name);
}   }   }   }

MODULE void skipws(TEXT* bufptr)
{   while (bufptr[symoffset] == ' ' || bufptr[symoffset] == HT)
    {   symoffset++;
}   }
MODULE void skipnonws(TEXT* bufptr)
{   while (bufptr[symoffset] != ' ' && bufptr[symoffset] != HT)
    {   symoffset++;
}   }
MODULE void skipline(TEXT* bufptr)
{   while (bufptr[symoffset] != LF)
    {   symoffset++;
    }
    symoffset++; // get past the LF
}

MODULE FLAG bp_add(int address, ULONG the1st, UBYTE the2nd, ULONG the3rd)
{   if (address < 0 || address > MAX_ADDR)
    {   zprintf
        (   TEXTPEN_DEBUG,
            LLL
            (   MSG_INVALIDRANGE,
                "Valid range is $%X..$%X!\n\n"
            ),
            0,
            MAX_ADDR
        );
        return FALSE;
    }

    if
    (   (memflags[address] & BREAKPOINT)
     && bp[address].the1st == the1st
     && bp[address].the2nd == the2nd
     && bp[address].the3rd == the3rd
    )
    {   zprintf
        (   TEXTPEN_DEBUG,
            LLL(
                MSG_BPALREADYSET,
                "Breakpoint is already set!\n\n"
        )   );
        return FALSE;
    }

    if (the1st == OUTOFRANGE)
    {   zprintf(TEXTPEN_DEBUG, "Conditional token is out of range!\n\n");
        return FALSE;
    }
    if (the3rd == OUTOFRANGE)
    {   zprintf(TEXTPEN_DEBUG, "Conditional value is out of range!\n\n");
        return FALSE;
    }

    memflags[address]  |= BREAKPOINT;
    bp[address].the1st =  the1st;
    bp[address].the2nd =  the2nd;
    bp[address].the3rd =  (UWORD) the3rd;

    DISCARD getfriendly(address); // makestrings() relies on this!

    if (the2nd == COND_UN)
    {   zprintf
        (   TEXTPEN_DEBUG,
            LLL
            (   MSG_ADDBP,
                "Added code breakpoint at %s.\n\n"
            ),
            friendly
        );
    } else
    {   makestrings(&bp[address]);
        DISCARD getfriendly(address); // must be done *after* bp_makestrings()!
        zprintf
        (   TEXTPEN_DEBUG,
            LLL
            (   MSG_ADDBPCOND,
                "Added code breakpoint at %s when %s %s %s.\n\n"
            ),
            friendly,
            firststring,
            condstring,
            secondstring
        );
    }

    update_memory(TRUE);
    update_monitor(TRUE); // to update button colours

    return TRUE;
}
EXPORT FLAG wp_add(int address, ULONG the1st, UBYTE the2nd, ULONG the3rd, UBYTE mask, FLAG update)
{   if
    (    address >= 0
     && (address <= MAX_ADDR || (address >= TOKEN_R0 && address <= TOKEN_R6))
    )
    {   ;
    } else
    {   if (update)
        {   zprintf
            (   TEXTPEN_CLIOUTPUT,
                LLL(
                    MSG_INVALIDRANGEWP,
                    "Valid range is $0000..$7FFF and %s..%s!\n\n"
                ),
                flagname[NAME_R0].shorter[style],
                flagname[NAME_R6].shorter[style]
            );
        }
        return FALSE;
    }

    if (mask != 0xFF)
    {   the1st = address;
        the2nd = COND_MASK;
        the3rd = mask;
    }

    if
    (   (memflags[address] & WATCHPOINT)
     && wp[address].the1st == the1st
     && wp[address].the2nd == the2nd
     && wp[address].the3rd == the3rd
    )
    {   if (update)
        {   zprintf
            (   TEXTPEN_DEBUG,
                LLL(
                    MSG_WPALREADYSET,
                    "Watchpoint is already set!\n\n"
            )   );
        }
        return FALSE;
    }

    if (the1st == OUTOFRANGE)
    {   zprintf(TEXTPEN_DEBUG, "Conditional token is out of range!\n\n");
        return FALSE;
    }
    if (the3rd == OUTOFRANGE)
    {   zprintf(TEXTPEN_DEBUG, "Conditional value is out of range!\n\n");
        return FALSE;
    }

    memflags[address]  |= WATCHPOINT;
    wp[address].the1st =  the1st;
    wp[address].the2nd =  the2nd;
    wp[address].the3rd =  (UWORD) the3rd;

    if (!update)
    {   return TRUE;
    }

    update_memory(TRUE);
    update_monitor(TRUE); // to update button colours

    if (!watchreads && !watchwrites)
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_WPWARNING, "Warning: watching of reads and writes is currently turned off! (WR and WW to enable.)\n"));
    }

    switch (the2nd)
    {
    case COND_UN:
        DISCARD getfriendly(address);
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            LLL
            (   MSG_ADDWP,
                "Added data watchpoint at %s.\n\n"
            ),
            friendly
        );
    acase COND_MASK:
        DISCARD getfriendly(address);
        dec_to_bin((UBYTE) the3rd);
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            LLL
            (   MSG_ADDWPBITS,
                "Added data watchpoint at %s (bits %s only).\n\n"
            ),
            friendly,
            v_bin
        );
    adefault:
        makestrings(&wp[address]);
        DISCARD getfriendly(address); // must be done *after* makestrings()!
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            LLL
            (   MSG_ADDWPCOND,
                "Added data watchpoint at %s when %s %s %s.\n\n"
            ),
            friendly,
            firststring,
            condstring,
            secondstring
        );
    }

    return TRUE;
}
MODULE FLAG ip_add(int address, ULONG the1st, UBYTE the2nd, ULONG the3rd, UBYTE mask, FLAG update)
{   FAST TEXT ipstring[40 + 1];

    if
    (   address <   0
     || address > 257
    )
    {   if (update)
        {   zprintf
            (   TEXTPEN_CLIOUTPUT,
                LLL(
                    MSG_INVALIDRANGE,
                    "Valid range is $%X..$%X!\n\n"
                ),
                0,
                0x101
            );
        }
        return FALSE;
    }

    if (mask != 0xFF)
    {   the1st = address;
        the2nd = COND_MASK;
        the3rd = mask;
    }

    if
    (   ioport[address].ip
     && ioport[address].cond.the1st == the1st
     && ioport[address].cond.the2nd == the2nd
     && ioport[address].cond.the3rd == the3rd
    )
    {   if (update)
        {   zprintf
            (   TEXTPEN_DEBUG,
                LLL(
                    MSG_WPALREADYSET,
                    "Watchpoint is already set!\n\n"
            )   );
        }
        return FALSE;
    }

    if (the1st == OUTOFRANGE)
    {   zprintf(TEXTPEN_DEBUG, "Conditional token is out of range!\n\n");
        return FALSE;
    }
    if (the3rd == OUTOFRANGE)
    {   zprintf(TEXTPEN_DEBUG, "Conditional value is out of range!\n\n");
        return FALSE;
    }

    ioport[address].ip          = TRUE;
    ioport[address].cond.the1st = the1st;
    ioport[address].cond.the2nd = the2nd;
    ioport[address].cond.the3rd = (UWORD) the3rd;

    if (!update)
    {   return TRUE;
    }

    if (!watchreads && !watchwrites)
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_WPWARNING, "Warning: watching of reads and writes is currently turned off! (WR and WW to enable.)\n"));
    }

    switch (address)
    {
    case  PORTC: strcpy( ipstring, LLL(MSG_IOCONTROLPORT,  "I/O control port"       )         );
    acase PORTD: strcpy( ipstring, LLL(MSG_IODATAPORT,     "I/O data port"          )         );
    adefault:    sprintf(ipstring, LLL(MSG_IOEXTENDEDPORT, "I/O extended port $%02X"), address);
    }

    switch (the2nd)
    {
    case COND_UN:
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            LLL(
                MSG_ADDIP,
                "Added I/O port watchpoint on %s.\n\n"
            ),
            ipstring
        );
    acase COND_MASK:
        dec_to_bin((UBYTE) the3rd);
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            LLL(
                MSG_ADDIPBITS,
                "Added I/O port watchpoint on %s (bits %s only).\n\n"
            ),
            ipstring,
            v_bin
        );
    adefault:
        makestrings(&ioport[address].cond);
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            LLL(
                MSG_ADDIPCOND,
                "Added I/O port watchpoint on %s when %s %s %s.\n\n"
            ),
            ipstring,
            firststring,
            condstring,
            secondstring
        );
    }

    return TRUE;
}

MODULE void makestrings(struct ConditionalStruct* cond)
{   if (cond->the1st >= FIRSTTOKEN)
    {   strcpy(friendly, tokenname[cond->the1st - FIRSTTOKEN][style]);
    } else
    {   getfriendly(cond->the1st);
    }
    strcpy((char*) firststring, (const char*) friendly);

    switch (cond->the2nd)
    {
    case  COND_LT: condstring = "<";
    acase COND_LE: condstring = "<=";
    acase COND_EQ: condstring = "==";
    acase COND_NE: condstring = "!=";
    acase COND_GE: condstring = ">=";
    acase COND_GT: condstring = ">";
    }

    DISCARD getfriendly(cond->the3rd);
    strcpy((char*) secondstring, (const char*) friendly);
}

MODULE int string_to_cond(STRPTR thearg)
{   if   (!strcmp(thearg, "<" ) || !stricmp(thearg, "LT"  )                                                  ) return COND_LT;
    elif (!strcmp(thearg, "<=") || !stricmp(thearg, "LE"  ) || !strcmp( thearg, "=<")                        ) return COND_LE;
    elif (!strcmp(thearg, "==") || !stricmp(thearg, "EQ"  ) || !stricmp(thearg, "IS") || !strcmp(thearg, "=")) return COND_EQ;
    elif (!strcmp(thearg, "!=") || !stricmp(thearg, "NE"  ) || !strcmp( thearg, "<>")                        ) return COND_NE;
    elif (!strcmp(thearg, ">" ) || !stricmp(thearg, "GT"  )                                                  ) return COND_GT;
    elif (!strcmp(thearg, ">=") || !stricmp(thearg, "GE"  ) || !strcmp( thearg, "=>")                        ) return COND_GE;
    else                                                                                      /* assert(0); */ return COND_UN;
}

EXPORT FLAG save_aof(STRPTR filename, int startaddr, int endaddr, int progstart, int preferredsize)
{   FILE* AOFHandle;
    int   blocksize,
          cursor,
          i,
          tempstart,
          tempend;

    // assert(startaddr <= endaddr);

#ifdef VERBOSE
    zprintf(TEXTPEN_CLIOUTPUT, "Saving %s from $%X..$%X as AOF...\n", filename, startaddr, endaddr);
#endif

    if (!(AOFHandle = fopen(filename, "wb")))
    {   zprintf(TEXTPEN_CLIOUTPUT, "Can't open \"%s\" for writing!\n", filename);
        return FALSE;
    } // implied else

    tempstart = startaddr;
    do
    {   blocksize = endaddr - tempstart + 1;
        if (blocksize > preferredsize)
        {   blocksize = preferredsize;
        }
        tempend = tempstart + blocksize - 1;
        if ((tempstart & 0x6000) != (tempend & 0x6000)) // to avoid crossing an 8K page boundary
        {   tempend = (tempstart & 0x6000) | 0x1FFF;
            blocksize = tempend - tempstart + 1;
        }

        AOFBuffer[0] = ':';                                             //  0     colon
        write_aof_byte(&AOFBuffer[1], (UBYTE) (tempstart / 256));       //  1.. 2 start address high byte
        write_aof_byte(&AOFBuffer[3], (UBYTE) (tempstart % 256));       //  3.. 4 start address low  byte
        write_aof_byte(&AOFBuffer[5], (UBYTE)  blocksize);              //  5.. 6 length
        write_aof_byte(&AOFBuffer[7], aof_makebcc(1, 6, AOFBuffer));    //  7.. 8 BCC
        cursor = 9;
        for (i = tempstart; i <= tempend; i++)
        {   write_aof_byte(&AOFBuffer[cursor], memory[i]);              //  9..68
            cursor += 2;
        }
        write_aof_byte(&AOFBuffer[cursor], aof_makebcc(9, cursor - 1, AOFBuffer)); // 69..70
        cursor += 2;
        AOFBuffer[cursor++] = CR;                                       // 71
        AOFBuffer[cursor++] = LF;                                       // 72
        DISCARD fwrite(AOFBuffer, (size_t) cursor, 1, AOFHandle);
#ifdef VERBOSE
        AOFBuffer[cursor] = EOS;                                        // 73
        zprintf(TEXTPEN_CLIOUTPUT, "%s", AOFBuffer);
#endif

        tempstart += blocksize;
    } while (tempstart <= endaddr);

    // final block
    AOFBuffer[0] = ':';                                       // 0
    write_aof_byte(&AOFBuffer[1], (UBYTE) (progstart / 256)); // 1..2
    write_aof_byte(&AOFBuffer[3], (UBYTE) (progstart % 256)); // 3..4
    write_aof_byte(&AOFBuffer[5],          0               ); // 5..6
    AOFBuffer[7] = CR;                                        // 7
    AOFBuffer[8] = LF;                                        // 8
    DISCARD fwrite( AOFBuffer, 9, 1, AOFHandle);              // 0..8
#ifdef VERBOSE
    AOFBuffer[9] = EOS;
    zprintf(TEXTPEN_CLIOUTPUT, "%s\n", AOFBuffer);
#endif

    DISCARD fclose(AOFHandle);
    // AOFHandle = NULL;

#ifdef AMIGA
    writeicon(KIND_AOF, filename);
#endif

    return TRUE;
}

MODULE FLAG save_iof(STRPTR filename, int startaddr, int endaddr)
{   FILE* AOFHandle;
    int   blocksize,
          cursor,
          i,
          tempstart,
          tempend;

    // assert(startaddr <= endaddr);

#ifdef VERBOSE
    zprintf(TEXTPEN_CLIOUTPUT, "Saving %s from $%X..$%X as Intel HEX...\n", filename, startaddr, endaddr);
#endif

    if (!(AOFHandle = fopen(filename, "wb")))
    {   zprintf(TEXTPEN_CLIOUTPUT, "Can't open \"%s\" for writing!\n", filename);
        return FALSE;
    } // implied else

    tempstart = startaddr;
    do
    {   blocksize = endaddr - tempstart + 1;
        if (blocksize > 16) // 255 is the maximum permitted by the standard
        {   blocksize = 16;
        }
        tempend = tempstart + blocksize - 1;

        AOFBuffer[0] = ':';                                             //  0     colon
        write_aof_byte(&AOFBuffer[1], (UBYTE)  blocksize);              //  1.. 2 length
        write_aof_byte(&AOFBuffer[3], (UBYTE) (tempstart / 256));       //  3.. 4 start address high byte
        write_aof_byte(&AOFBuffer[5], (UBYTE) (tempstart % 256));       //  5.. 6 start address low  byte
        write_aof_byte(&AOFBuffer[7],          0);                      //  7.. 8 record type
        cursor = 9;
        for (i = tempstart; i <= tempend; i++)
        {   write_aof_byte(&AOFBuffer[cursor], memory[i]);              //  9..68 payload
            cursor += 2;
        }
        write_aof_byte(&AOFBuffer[cursor], iof_makebcc(1, cursor - 1, AOFBuffer)); // 69..70
        cursor += 2;
        AOFBuffer[cursor++] = CR;                                       // 71
        AOFBuffer[cursor++] = LF;                                       // 72
        DISCARD fwrite(AOFBuffer, (size_t) cursor, 1, AOFHandle);
#ifdef VERBOSE
        AOFBuffer[cursor] = EOS;                                        // 73
        zprintf(TEXTPEN_CLIOUTPUT, "%s", AOFBuffer);
#endif

        tempstart += blocksize;
    } while (tempstart <= endaddr);

    // final block (":00000001FF")
    AOFBuffer[0]                                                                                            = ':'; //  0     colon
    AOFBuffer[1] = AOFBuffer[ 2] = AOFBuffer[3] = AOFBuffer[4] = AOFBuffer[5] = AOFBuffer[6] = AOFBuffer[7] = '0'; //  1.. 7 length, start address, record type
    AOFBuffer[8]                                                                                            = '1'; //  8     record type
    AOFBuffer[9] = AOFBuffer[10]                                                                            = 'F'; //  9..10 checksum
    AOFBuffer[11]                                                                                           = CR ; // 11     CR
    AOFBuffer[12]                                                                                           = LF ; // 12     LF
    DISCARD fwrite(AOFBuffer, 13, 1, AOFHandle);
#ifdef VERBOSE
    AOFBuffer[13] = EOS;                                                                                           // 13
    zprintf(TEXTPEN_CLIOUTPUT, "%s\n", AOFBuffer);
#endif

    DISCARD fclose(AOFHandle);
    // AOFHandle = NULL;

    return TRUE;
}

MODULE void write_aof_byte(UBYTE* output, UBYTE value)
{   if (value / 16 >= 10)
    {   *(output    ) = (UBYTE) ('A' + (value / 16) - 10);
    } else
    {   *(output    ) = (UBYTE) ('0' + (value / 16)     );
    }
    if (value % 16 >= 10)
    {   *(output + 1) = (UBYTE) ('A' + (value % 16) - 10);
    } else
    {   *(output + 1) = (UBYTE) ('0' + (value % 16)     );
}   }

EXPORT void handletab(void)
{   int    i,
           length,
           matches   = 0,
           wordstart = 0;
    STRPTR matchptr  = NULL; // initialized to avoid a spurious SAS/C warning

    length = strlen((const char*) userinput);
    for (i = length - 1; i >= 0; i--)
    {   if (userinput[i] == ' ')
        {   wordstart = i + 1;
    }   }

    if (userlabels)
    {   for (i = 0; i < userlabels; i++)
        {   if (!strnicmp((const char*) &userinput[wordstart], (const char*) symlabel[i].name, length - wordstart))
            {   matches++;
                matchptr = (STRPTR) &symlabel[i].name[length - wordstart];
    }   }   }
    for (i = FIRSTCPUEQUIV; i <= LASTCPUEQUIV; i++)
    {   if (!strnicmp((const char*) &userinput[wordstart], (const char*) equivalents[i].name, length - wordstart))
        {   matches++;
            matchptr = (STRPTR) &equivalents[i].name[length - wordstart];
    }   }
    if (machines[machine].pvis)
    {   for (i = FIRSTPVIEQUIV; i < FIRSTPVIEQUIV + (33 * machines[machine].pvis); i++)
        {   if (!strnicmp((const char*) &userinput[wordstart], (const char*) equivalents[i].name, length - wordstart))
            {   matches++;
                matchptr = (STRPTR) &equivalents[i].name[length - wordstart];
    }   }   }
    if (machines[machine].firstequiv != -1)
    {   for (i = machines[machine].firstequiv; i <= machines[machine].lastequiv; i++)
        {   if (!strnicmp((const char*) &userinput[wordstart], (const char*) equivalents[i].name, length - wordstart))
            {   matches++;
                matchptr = (STRPTR) &equivalents[i].name[length - wordstart];
    }   }   }
    if (machine == BINBUG)
    {   if (firstdosequiv != -1)
        {   for (i = firstdosequiv; i <= lastdosequiv; i++)
            {   if (!strnicmp((const char*) &userinput[wordstart], (const char*) equivalents[i].name, length - wordstart))
                {   matches++;
                    matchptr = (STRPTR) &equivalents[i].name[length - wordstart];
        }   }   }
        for (i = FIRSTACOSEQUIV; i <= LASTACOSEQUIV; i++)
        {   if (!strnicmp((const char*) &userinput[wordstart], (const char*) equivalents[i].name, length - wordstart))
            {   matches++;
                matchptr = (STRPTR) &equivalents[i].name[length - wordstart];
    }   }   }
    if (whichgame != -1 && known[whichgame].firstequiv != -1)
    {   for (i = FIRSTGAMEEQUIV + known[whichgame].firstequiv; i <= FIRSTGAMEEQUIV + known[whichgame].lastequiv; i++)
        {   if (!strnicmp((const char*) &userinput[wordstart], (const char*) equivalents[i].name, length - wordstart))
            {   matches++;
                matchptr = (STRPTR) &equivalents[i].name[length - wordstart];
    }   }   }

    if (matches == 1)
    {   strcat((char*) userinput, matchptr);
}   }

EXPORT void clearruntos(void)
{   runtoframe     =
    runtointerrupt =
    runtoloopend   = FALSE;
    rastn          = INVALIDRASTER;
}

MODULE void showfound(int startpoint, int endpoint)
{   int  i;
    TEXT t = EOS; // initialized to avoid compiler warning

    DISCARD getfriendly(startpoint);
    zprintf(TEXTPEN_CLIOUTPUT, "%s: \"", friendly);
    for (i = startpoint; i <= endpoint; i++)
    {   switch (machine)
        {
        case ARCADIA:
            t = arcadia_chars[memory[i] & 0x3F];
        acase ELEKTOR:
            t = elektor_chars1[memory[i]];
        acase PIPBUG:
        case BINBUG:
        case TWIN:
        case SELBST:
        case PHUNSY:
            t = memory[i];
        acase MALZAK:
            t = memory[i] & 0x7F;
        acase CD2650:
            t = cd2650_chars[memory[i] & 0x7F];
        acase INSTRUCTOR:
            t = instructor_chars1[memory[i]];
        acase ZACCARIA:
            switch (memmap)
            {
            case MEMMAP_ASTROWARS:
                t = astrowars_chars[memory[i]];
            acase MEMMAP_GALAXIA:
                t = galaxia_chars[memory[i] & 0x7F];
            acase MEMMAP_LASERBATTLE:
            case MEMMAP_LAZARIAN:
                t = lb_chars[memory[i]];
        }   }
        // assert(t != EOS);
        zprintf(TEXTPEN_CLIOUTPUT, "%c", t);
    }
    zprintf(TEXTPEN_CLIOUTPUT, "\"\n");
}

MODULE void view_imagery(int address1, int address2, FILE* filehandle)
{   FAST int  i, j;
    FAST TEXT bits[8 + 1],
              outputstring[37 + 1];

    // assert(address1 <= address2);

    bits[8] = EOS;

    for (i = address1; i <= address2; i++)
    {   for (j = 0; j < 8; j++)
        {   if (memory[i] & (0x80 >> j))
            {   bits[j] = '#';
            } else
            {   bits[j] = '.';
        }   }

        sprintf
        (   (char*) outputstring,
            "        db      $%02X ;%s $%04X\n",
          // 01234567890123456  789015901234 5
            memory[i],           // 26
            bits,                // 37
            i                    // 48
        );
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            outputstring
        );
        if (filehandle)
        {   DISCARD fwrite(outputstring, 36, 1, filehandle);
    }   }

    zprintf(TEXTPEN_CLIOUTPUT, "\n");
    // We could add an END directive to the end of the file.
}

MODULE void view_coverage(int reporttype)
{   int   i, j, k,
          localregionstart;
    FLAG  ok;
    ULONG flags,
          stackdepth;

    /* Possible enhancements:
    keep a count of how many times they were each accessed.
    does coverage work properly with mirroring, especially for the "A"
     flag?
    read/writes by the UVI/PVI don't set any coverage bits; perhaps they
     should?
    make some distinction between relative/absolute/zero page accesses
     (rather pointless). */

    if (machine == PONG || machine == TYPERIGHT)
    {   return;
    }

/* reporttypes are:
   0 = all
   1 = accessed bytes
   2 = I/O port accesses
   3 = branches and subroutine calls
   4 = loops
   5 = UVI/PVI accesses */

#ifdef WIN32
    hurry = TRUE;
    cls();
#endif

    if (reporttype <= 1)
    {   zprintf(TEXTPEN_CLIOUTPUT, "Accessed bytes:\n");
        i = 0;
        do
        {   localregionstart = i;
            flags = (coverage[i] & COVERAGE_BITS);
            do
            {   i++;
            } while
            (   (coverage[i] & COVERAGE_BITS) == flags
             && i < 32 * KILOBYTE
            );
            if (flags != 0) // to show all areas (including uncovered), change this line to "if (1)"
            {   if (localregionstart != i - 1)
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s: $%04X..$%04X\n", coveragename[flags], (unsigned int) localregionstart, (unsigned int) (i - 1));
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s: $%04X\n"       , coveragename[flags], (unsigned int) localregionstart);
        }   }   }
        while (i < 32 * KILOBYTE);
        zprintf(TEXTPEN_CLIOUTPUT, "\n");
    }

    if (reporttype == 0 || reporttype == 2)
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_IOPORTACCESSES, "I/O port accesses:\n"));
        ok = FALSE;

        flags = (coverage_io[PORTC] & COVERAGE_BITS);
        if (flags != 0) // to show all areas (including uncovered), change this line to "if (1)"
        {   ok = TRUE;
            zprintf(TEXTPEN_CLIOUTPUT,         "%s: Control\n"       , coveragename[flags]);
        }

        flags = (coverage_io[PORTD] & COVERAGE_BITS);
        if (flags != 0) // to show all areas (including uncovered), change this line to "if (1)"
        {   ok = TRUE;
            zprintf(TEXTPEN_CLIOUTPUT,         "%s: Data\n"          , coveragename[flags]);
        }

        i = 0;
        do
        {   localregionstart = i;
            flags = (coverage_io[i] & COVERAGE_BITS);
            do
            {   i++;
            } while
            (   ((coverage_io[i] & COVERAGE_BITS) == (UBYTE) flags)
             && i < 256
            );
            if (flags != 0) // to show all areas (including uncovered), change this line to "if (1)"
            {   ok = TRUE;
                if (localregionstart != i - 1)
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s:  $%03X.. $%03X\n", coveragename[flags], (unsigned int) localregionstart, (unsigned int) (i - 1));
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, "%s:  $%03X\n"        , coveragename[flags], (unsigned int) localregionstart);
        }   }   }
        while (i < 256);

        if (!ok)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n", LLL(MSG_NONE, "None"));
        }
        zprintf(TEXTPEN_CLIOUTPUT, "\n");
    }

    if (reporttype == 0 || reporttype == 3)
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_BRANCHESANDSUBS, "Branches and subroutine calls:\n"));
        j = FALSE;
        for (i = 0; i <= 0x7FFF; i++)
        {   if (coverage[i] & ROUTINE_BITS)
            {   j = TRUE;
                stackdepth = (coverage[i] & COVERAGE_DEPTH) >> COVERAGE_BITSHIFT;
                zprintf
                (   TEXTPEN_CLIOUTPUT,
                    "%c%c%c%c: ",
                    (coverage[i] & COVERAGE_JUMPS   ) ? 'C' : '-', // mainline code section
                    (coverage[i] & COVERAGE_CALLS   ) ? 'S' : '-', // mainline subroutine
                    (coverage[i] & COVERAGE_JUMPSINT) ? 'c' : '-', // interrupt code section
                    (coverage[i] & COVERAGE_CALLSINT) ? 's' : '-'  // interrupt subroutine
                );
                if (stackdepth)
                {   for (k = 0; k < (int) stackdepth; k++)
                    {   zprintf(TEXTPEN_CLIOUTPUT, " ");
                }   }
                DISCARD getfriendly(i);
                zprintf(TEXTPEN_CLIOUTPUT, "%s\n", friendly);
        }   }
        if (!j)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n", LLL(MSG_NONE, "None"));
        }
        zprintf(TEXTPEN_CLIOUTPUT, "\n");
    }

    if (reporttype == 0 || reporttype == 4)
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_LOOPS, "Loops:\n"));
        j = FALSE;
        for (i = 0; i <= 0x7FFF; i++)
        {   if (coverage[i] & LOOP_BITS)
            {   j = TRUE;
                zprintf
                (   TEXTPEN_CLIOUTPUT,
                    "%c%c: ",
                    (coverage[i] & COVERAGE_LOOPSTART) ? 'S' : '-',
                    (coverage[i] & COVERAGE_LOOPEND  ) ? 'E' : '-'
                );
                DISCARD getfriendly(i);
                zprintf(TEXTPEN_CLIOUTPUT, "%s\n", friendly);
        }   }
        if (!j)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s.\n", LLL(MSG_NONE, "None"));
        }
        zprintf(TEXTPEN_CLIOUTPUT, "\n");
    }

    if (reporttype == 0 || reporttype == 5)
    {   if (machine == ARCADIA)
        {   zprintf(TEXTPEN_CLIOUTPUT, "UVI accesses:\n");
            j = 0;
            j += showasic(A_SPRITE0Y       , ASIC_UVI_SPRITE0Y     );
            j += showasic(A_SPRITE0X       , ASIC_UVI_SPRITE0X     );
            j += showasic(A_SPRITE1Y       , ASIC_UVI_SPRITE1Y     );
            j += showasic(A_SPRITE1X       , ASIC_UVI_SPRITE1X     );
            j += showasic(A_SPRITE2Y       , ASIC_UVI_SPRITE2Y     );
            j += showasic(A_SPRITE2X       , ASIC_UVI_SPRITE2X     );
            j += showasic(A_SPRITE3Y       , ASIC_UVI_SPRITE3Y     );
            j += showasic(A_SPRITE3X       , ASIC_UVI_SPRITE3X     );
            j += showasic(A_VSCROLL        , ASIC_UVI_VSCROLL      );
            j += showasic(A_PITCH          , ASIC_UVI_PITCH        );
            j += showasic(A_VOLUME         , ASIC_UVI_VOLUME       );
            j += showasic(A_CHARLINE       , ASIC_UVI_CHARLINE     );
            j += showasic(A_P1LEFTKEYS     , ASIC_UVI_P1LEFTKEYS   );
            j += showasic(A_P1MIDDLEKEYS   , ASIC_UVI_P1MIDDLEKEYS );
            j += showasic(A_P1RIGHTKEYS    , ASIC_UVI_P1RIGHTKEYS  );
            j += showasic(A_P1PALLADIUM    , ASIC_UVI_P1PALLADIUM  );
            j += showasic(A_P2LEFTKEYS     , ASIC_UVI_P2LEFTKEYS   );
            j += showasic(A_P2MIDDLEKEYS   , ASIC_UVI_P2MIDDLEKEYS );
            j += showasic(A_P2RIGHTKEYS    , ASIC_UVI_P2RIGHTKEYS  );
            j += showasic(A_P2PALLADIUM    , ASIC_UVI_P2PALLADIUM  );
            j += showasic(A_CONSOLE        , ASIC_UVI_CONSOLE      );
            j += showasic(A_GFXMODE        , ASIC_UVI_GFXMODE      );
            j += showasic(A_BGCOLOUR       , ASIC_UVI_BGCOLOUR     );
            j += showasic(A_SPRITES23CTRL  , ASIC_UVI_SPRITES23CTRL);
            j += showasic(A_SPRITES01CTRL  , ASIC_UVI_SPRITES01CTRL);
            j += showasic(A_BGCOLLIDE      , ASIC_UVI_BGCOLLIDE    );
            j += showasic(A_SPRITECOLLIDE  , ASIC_UVI_SPRITECOLLIDE);
            j += showasic(A_P2PADDLE       , ASIC_UVI_P2PADDLE     );
            j += showasic(A_P1PADDLE       , ASIC_UVI_P1PADDLE     );
            if (!j)
            {   zprintf(TEXTPEN_CLIOUTPUT, "None\n");
            }
            zprintf(TEXTPEN_CLIOUTPUT, "\n");
        } elif (machines[machine].pvis)
        {   zprintf(TEXTPEN_CLIOUTPUT, "PVI accesses:\n");
            j = 0;
            if (machine == INTERTON || machine == ELEKTOR)
            {   j += showasic(      IE_NOISE         , ASIC_IE_NOISE         );
                j += showasic(      IE_P1MIDDLEKEYS  , ASIC_IE_P1KEYS        );
                j += showasic(      IE_CONSOLE       , ASIC_IE_CONSOLE       );
                j += showasic(      IE_P2MIDDLEKEYS  , ASIC_IE_P2KEYS        );
            }
            j += showasic(pvibase + PVI_SPRITE0AX    , ASIC_PVI_SPRITE0AX    );
            j += showasic(pvibase + PVI_SPRITE0BX    , ASIC_PVI_SPRITE0BX    );
            j += showasic(pvibase + PVI_SPRITE0AY    , ASIC_PVI_SPRITE0AY    );
            j += showasic(pvibase + PVI_SPRITE0BY    , ASIC_PVI_SPRITE0BY    );
            j += showasic(pvibase + PVI_SPRITE1AX    , ASIC_PVI_SPRITE1AX    );
            j += showasic(pvibase + PVI_SPRITE1BX    , ASIC_PVI_SPRITE1BX    );
            j += showasic(pvibase + PVI_SPRITE1AY    , ASIC_PVI_SPRITE1AY    );
            j += showasic(pvibase + PVI_SPRITE1BY    , ASIC_PVI_SPRITE1BY    );
            j += showasic(pvibase + PVI_SPRITE2AX    , ASIC_PVI_SPRITE2AX    );
            j += showasic(pvibase + PVI_SPRITE2BX    , ASIC_PVI_SPRITE2BX    );
            j += showasic(pvibase + PVI_SPRITE2AY    , ASIC_PVI_SPRITE2AY    );
            j += showasic(pvibase + PVI_SPRITE2BY    , ASIC_PVI_SPRITE2BY    );
            j += showasic(pvibase + PVI_SPRITE3AX    , ASIC_PVI_SPRITE3AX    );
            j += showasic(pvibase + PVI_SPRITE3BX    , ASIC_PVI_SPRITE3BX    );
            j += showasic(pvibase + PVI_SPRITE3AY    , ASIC_PVI_SPRITE3AY    );
            j += showasic(pvibase + PVI_SPRITE3BY    , ASIC_PVI_SPRITE3BY    );
            j += showasic(pvibase + PVI_SIZES        , ASIC_PVI_SIZES        );
            j += showasic(pvibase + PVI_SPR01COLOURS , ASIC_PVI_SPR01COLOURS );
            j += showasic(pvibase + PVI_SPR23COLOURS , ASIC_PVI_SPR23COLOURS );
            j += showasic(pvibase + PVI_SCORECTRL    , ASIC_PVI_SCORECTRL    );
            j += showasic(pvibase + PVI_BGCOLOUR     , ASIC_PVI_BGCOLOUR     );
            j += showasic(pvibase + PVI_PITCH        , ASIC_PVI_PITCH        );
            j += showasic(pvibase + PVI_SCORELT      , ASIC_PVI_SCORELT      );
            j += showasic(pvibase + PVI_SCORERT      , ASIC_PVI_SCORERT      );
            j += showasic(pvibase + PVI_BGCOLLIDE    , ASIC_PVI_BGCOLLIDE    );
            j += showasic(pvibase + PVI_SPRITECOLLIDE, ASIC_PVI_SPRITECOLLIDE);
            j += showasic(pvibase + PVI_P1PADDLE     , ASIC_PVI_P1PADDLE     );
            j += showasic(pvibase + PVI_P2PADDLE     , ASIC_PVI_P2PADDLE     );
            if (!j)
            {   zprintf(TEXTPEN_CLIOUTPUT, "None\n");
            }
            zprintf(TEXTPEN_CLIOUTPUT, "\n");
        } elif (reporttype == 5)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
    }   }

#ifdef WIN32
    hurry = FALSE;
    zprintf(TEXTPEN_DEFAULT, "");
#endif
}

MODULE int showasic(int whichmem, ULONG whichflag)
{   FLAG ok = FALSE;
    int  i,
         reads, writes;

    for (i = 0; i < 32768; i++)
    {   if ((asicreads[i] & whichflag) || (asicwrites[i] & whichflag))
        {   ok = TRUE;
            break; // for speed
    }   }
    if (!ok)
    {   return 0;
    }

    getfriendly(whichmem);
    zprintf(TEXTPEN_CLIOUTPUT, "%21s: ", friendly);

    reads = 0;
    for (i = 0; i < 32768; i++)
    {   if (asicreads[i] & whichflag)
        {   if (reads == 0)
            {   zprintf(TEXTPEN_CLIOUTPUT, "Read    by ");
            } else
            {   zprintf(TEXTPEN_CLIOUTPUT, ", ");
            }
            DISCARD getfriendly(i);
            zprintf(TEXTPEN_CLIOUTPUT, "%s", friendly);
            reads++;
    }   }
    if (reads)
    {   if (memflags[whichmem] & NOREAD)
        {   zprintf(TEXTPEN_CLIOUTPUT, "!\n");
        } else
        {   zprintf(TEXTPEN_CLIOUTPUT, ".\n");
    }   }

    writes = 0;
    for (i = 0; i < 32768; i++)
    {   if (asicwrites[i] & whichflag)
        {   if (writes == 0)
            {   if (reads)
                {   zprintf(TEXTPEN_CLIOUTPUT, "                       ");
                }
                zprintf(TEXTPEN_CLIOUTPUT, "Written by ");
            } else
            {   zprintf(TEXTPEN_CLIOUTPUT, ", ");
            }
            DISCARD getfriendly(i);
            zprintf(TEXTPEN_CLIOUTPUT, "%s", friendly);
            writes++;
    }   }
    if (writes)
    {   if (memflags[whichmem] & NOWRITE)
        {   zprintf(TEXTPEN_CLIOUTPUT, "!\n");
        } else
        {   zprintf(TEXTPEN_CLIOUTPUT, ".\n");
    }   }

    return 1;
}

EXPORT FLAG wp_clear(int address)
{   if (address < 0 || address > MAX_ADDR)
    {   zprintf
        (   TEXTPEN_DEBUG,
            LLL(
                MSG_INVALIDRANGE,
                "Valid range is $%X..$%X!\n\n"
            ),
            0,
            MAX_ADDR
        );
        return FALSE;
    }

    DISCARD getfriendly(address);
    if (memflags[address] & WATCHPOINT)
    {   memflags[address] &= ~(WATCHPOINT);
        zprintf
        (   TEXTPEN_DEBUG,
            LLL
            (   MSG_ENGINE_CLEAREDWP,
                "Cleared data watchpoint at %s.\n\n"
            ),
            friendly
        );
        update_memory(TRUE);
        update_monitor(TRUE); // to update button colours
        return TRUE;
    } // implied else

    zprintf
    (   TEXTPEN_CLIOUTPUT,
        LLL
        (   MSG_ENGINE_WPNOTFOUND,
            "Watchpoint not found at %s!\n\n"
        ),
        friendly
    );
    return FALSE;
}

EXPORT UBYTE aof_makebcc(int start, int end, UBYTE* BufferPtr)
{   UBYTE ourbcc = 0;
    int   i;

    for (i = start; i <= end; i += 2)
    {   ourbcc ^= ( (getdigit(BufferPtr[i    ]) * 16)
                  +  getdigit(BufferPtr[i + 1])
                  );
        if (ourbcc >= 128)
        {   ourbcc = ((ourbcc & 127) * 2) + 1;
        } else ourbcc <<= 1;
    }
    return ourbcc;
}

EXPORT UBYTE iof_makebcc(int start, int end, UBYTE* BufferPtr)
{   UBYTE ourbcc = 0;
    int   i;

    for (i = start; i <= end; i += 2)
    {   ourbcc += ( (getdigit(BufferPtr[i    ]) * 16)
                  +  getdigit(BufferPtr[i + 1])
                  ); // overflow is OK
    }
    ourbcc ^= 0xFF;
    ourbcc++;
    return ourbcc;
}

EXPORT void cd_configs(void)
{
#ifdef AMIGA
    BPTR LocalLock;

    LocalLock = Lock((const char*) "PROGDIR:Configs", ACCESS_READ);
    DISCARD CurrentDir(LocalLock);
#endif
#ifdef WIN32
    DISCARD SetCurrentDirectory("Configs");
#endif
}

EXPORT void cd_projects(void)
{
#ifdef AMIGA
    BPTR ProjectsLock;

    ProjectsLock = Lock((const char*) path_projects, ACCESS_READ);
    DISCARD CurrentDir(ProjectsLock);
#endif
#ifdef WIN32
    DISCARD SetCurrentDirectory(path_projects);
#endif
}

EXPORT void cd_screenshots(void)
{
#ifdef AMIGA
    BPTR LocalLock;

    LocalLock = Lock((const char*) path_screenshots, ACCESS_READ);
    DISCARD CurrentDir(LocalLock);
#endif
#ifdef WIN32
    DISCARD SetCurrentDirectory(path_screenshots);
#endif
}

EXPORT void cd_progdir(void)
{
#ifdef WIN32
    DISCARD SetCurrentDirectory(ProgDir);
#endif
#ifdef AMIGA
    DISCARD CurrentDir(ProgLock);
#endif
}

MODULE FLAG check_range(int address1, int address2)
{   if
    (   address1 != OUTOFRANGE
     && address2 != OUTOFRANGE
     && address1 <= address2 // perhaps we might want a different error message for this case
    )
    {   return TRUE;
    } // implied else

    zprintf
    (   TEXTPEN_CLIOUTPUT,
        LLL(
            MSG_INVALIDRANGE,
            "Valid range is $%X..$%X!\n\n"
           ),
        0,
        MAX_ADDR
    );
    return FALSE;
}

EXPORT ULONG scale_time(ULONG inputclocks)
{   switch (timeunit)
    {
    case  TIMEUNIT_CYCLES: return inputclocks / 3;
    acase TIMEUNIT_CLOCKS: return inputclocks;
    acase TIMEUNIT_PIXELS: return inputclocks * ppc;
    adefault:              return 0; // should never happen
}   }

EXPORT void changetimeunitnames(void)
{   switch (timeunit)
    {
    case  TIMEUNIT_CYCLES: timeunitstr1 = LLL(MSG_CPUCYCLE,   "CPU Cycle:"); timeunitstr2 = LLL(MSG_CPUCYCLES,    "CPU cycles"   );
    acase TIMEUNIT_CLOCKS: timeunitstr1 = LLL(MSG_CLOCK_LONG, "Clock:"    ); timeunitstr2 = LLL(MSG_CLOCKPERIODS, "clock periods");
    acase TIMEUNIT_PIXELS: timeunitstr1 = LLL(MSG_PIXEL,      "Pixel:"    ); timeunitstr2 = LLL(MSG_PIXELS,       "pixels"       );
}   }

EXPORT UBYTE readnum_ebcdic(TEXT code)
{   lastparse = BASE_EBCDIC;

    switch (code)
    {
    case ' ': return 0x40;
    case '.': return 0x4B;
    case '<': return 0x4C;
    case '(': return 0x4D;
    case '+': return 0x4E;
    case '|': return 0x4F;
    case '&': return 0x50;
    case '!': return 0x5A;
    case '$': return 0x5B;
    case '*': return 0x5C;
    case ')': return 0x5D;
    case ';': return 0x5E;
    case '~': return 0x5F;
    case '-': return 0x60;
    case '/': return 0x61;
    case ',': return 0x6B;
    case '%': return 0x6C;
    case '_': return 0x6D;
    case '>': return 0x6E;
    case '?': return 0x6F;
    case ':': return 0x7A;
    case '#': return 0x7B;
    case '@': return 0x7C;
    case QUOTE: return 0x7D;
    case '=': return 0x7E;
    case '"': return 0x7F;
    default:
        if (code >= '0' && code <= '9')
        {   return (UBYTE) (code - '0' + 0xF0); // $F0..$F9
        }
        code = toupper(code);
        if (code >= 'A' && code <= 'I')
        {   return (UBYTE) (code - 'A' + 0xC1); // $C1..$C9
        }
        if (code >= 'J' && code <= 'R')
        {   return (UBYTE) (code - 'J' + 0xD1); // $D1..$D9
        }
        if (code >= 'S' && code <= 'Z')
        {   return (UBYTE) (code - 'S' + 0xE2); // $E2..$E9
        }
        return 0; // unsupported character
}   }

EXPORT FLAG conditional(struct ConditionalStruct* cond, UBYTE data, FLAG writing, int pointkind)
{   UWORD value;

    if (pointkind == 0) // BP/WP
    {   switch (cond->the1st)
        {
        case  TOKEN_PSU:  value = psu;
        acase TOKEN_PSL:  value = psl;
        acase TOKEN_SP:   value = (psu & 7);
        acase TOKEN_CC:   value = (psl & 0xC0) >> 6;
        acase TOKEN_IAR:  value = iar;
        acase TOKEN_RAS0: value = ras[0];
        acase TOKEN_RAS1: value = ras[1];
        acase TOKEN_RAS2: value = ras[2];
        acase TOKEN_RAS3: value = ras[3];
        acase TOKEN_RAS4: value = ras[4];
        acase TOKEN_RAS5: value = ras[5];
        acase TOKEN_RAS6: value = ras[6];
        acase TOKEN_RAS7: value = ras[7];
        acase TOKEN_R0:   value = r[0];
        acase TOKEN_R1:   value = r[1];
        acase TOKEN_R2:   value = r[2];
        acase TOKEN_R3:   value = r[3];
        acase TOKEN_R4:   value = r[4];
        acase TOKEN_R5:   value = r[5];
        acase TOKEN_R6:   value = r[6];
        adefault:         value = memory[cond->the1st];
    }   }
    else
    {   // assert(pointkind == 1); // IP
        value = ioport[cond->the1st].contents;
    }

    switch (cond->the2nd)
    {
    case COND_UN: return TRUE;
    case COND_LT: return (FLAG) (value <  cond->the3rd);
    case COND_LE: return (FLAG) (value <= cond->the3rd);
    case COND_EQ: return (FLAG) (value == cond->the3rd);
    case COND_NE: return (FLAG) (value != cond->the3rd);
    case COND_GE: return (FLAG) (value >= cond->the3rd);
    case COND_GT: return (FLAG) (value >  cond->the3rd);
    case COND_MASK:
        // assert(kind == WATCHPOINT);
        if (writing)
        {   if ((value & cond->the3rd) != (data & cond->the3rd))
            {   return TRUE;
            } else
            {   return FALSE;
        }   }
        else
        {   return TRUE;
        }
    default:
        // assert(0);
        return FALSE;
}   }

EXPORT int readbase(STRPTR input, FLAG maskable)
{   TRANSIENT SLONG number;
    TRANSIENT int   i,
                    length;
    FAST      TEXT  thenewstring[80 + 1];

    length = strlen(input);
    if (length == 0) return OUTOFRANGE;

    strcpy((char*) thenewstring, input);
    strupr((char*) thenewstring);

    if (maskable)
    {   watchmask = 0xFF;
    }
    for (i = 0; i < length; i++)
    {   if (thenewstring[i] == ':')
        {   thenewstring[i] = EOS;
            if (i + 1 < length)
            {   stch_l((const char*) &thenewstring[i + 1], (SLONG*) &number);
                if (number < 0 || number > 255)
                {   zprintf(TEXTPEN_CLIOUTPUT, "Mask is out of range!\n");
                } elif (!maskable)
                {   zprintf(TEXTPEN_CLIOUTPUT, "Ignoring :%02X mask specifier.\n", number);
                } else
                {   watchmask = (UBYTE) number;
                }
            }
            length = i;
            break;
    }   }

    i = parse_opcode(thenewstring, length);
    if (i != OUTOFRANGE)
    {   return i;
    }
    i = parse_literal(thenewstring, length);
    if (i < 0)
    {   i += 256;
    }
    return i;
}

EXPORT int parse_expression(STRPTR thestring, int max, FLAG maskable)
{   int  i,
         length;
    LONG number;

    if (machine == PONG || machine == TYPERIGHT)
    {   thestring[0] = EOS;
        return OUTOFRANGE;
    }
    length = strlen(thestring);
    if (length < 1 || length > 80)
    {   thestring[0] = EOS;
        return OUTOFRANGE;
    }

    if (maskable)
    {   watchmask = 0xFF;
    }
    for (i = 0; i < length; i++)
    {   if (thestring[i] == ':')
        {   thestring[i] = EOS;
            if (i + 1 < length)
            {   stch_l((const char*) &thestring[i + 1], (SLONG*) &number);
                if (number < 0 || number > 255)
                {   zprintf(TEXTPEN_CLIOUTPUT, "Mask is out of range!\n");
                } elif (!maskable)
                {   zprintf(TEXTPEN_CLIOUTPUT, "Ignoring :%02X mask specifier.\n", number);
                } else
                {   watchmask = (UBYTE) number;
            }   }
            // length = i;
            break;
    }   }

    // registers, etc.
    for (i = FIRSTTOKEN; i <= LASTTOKEN; i++)
    {   if (!stricmp(tokenname[i - FIRSTTOKEN][style], (const char*) thestring))
        {   if (i > max)
            {   return OUTOFRANGE;
            } else\
            {   return i;
    }   }   }

    i = parsenumber(thestring);
    return ((i > max) ? OUTOFRANGE: i);
}
    
EXPORT TEXT getfriendly(int number)
{   return number_to_friendly(number, (STRPTR) friendly, TRUE, 0);
}

MODULE void multifunc(int number, TEXT labelkind, int multimode)
{   if (multimode == 1)
    {   switch (labelkind)
        {
        case  'C': zprintf(friendlycolour, "%s (code)"    , friendly);
        acase 'D': zprintf(friendlycolour, "%s (data)"    , friendly);
        acase 'P': zprintf(friendlycolour, "%s (pointer)" , friendly);
        acase 'U': zprintf(friendlycolour, "%s (unknown)" , friendly);
        acase 'R': zprintf(friendlycolour, "%s (register)", friendly);
        }
        zprintf(friendlycolour, "\n");
    } else
    {   // assert(multimode == 2);
        if (strlen(friendly) > 30)
        {   if (tabpos % 2 == 1)
            {   zprintf(friendlycolour, "\n");
                tabpos++;
            }
            zprintf(friendlycolour, "%69s %c $%04X\n", friendly, labelkind, number);
            tabpos += 2;
        } else
        {   if (tabpos % 2 == 1)
            {   zprintf(friendlycolour, " %30s %c $%04X\n", friendly, labelkind, number);
            } else
            {   zprintf(friendlycolour, "%30s %c $%04X", friendly, labelkind, number);
            }
            tabpos++;
}   }   }

EXPORT TEXT number_to_friendly(int number, STRPTR thestring, FLAG both, int multimode)
{   int i;

/*  number:    input number
    thestring: pointer to output string buffer
    both:      do we want friendly and hex names (TRUE) (eg. "BGCOLOUR
               ($19FD)") or just friendly names (eg. "BGCOLOUR")? */

    if (!allowable(FALSE))
    {   return EOS;
    }

    if (number >= FIRSTTOKEN && number <= LASTTOKEN)
    {   strcpy(thestring, tokenname[number - FIRSTTOKEN][style]);
        foundequiv = OUTOFRANGE;
        friendlycolour = TEXTPEN_YELLOW; // means register
        if (multimode)
        {   multifunc(number, 'R', multimode);
        } else
        {   return 'R';
    }   }

    if (userlabels)
    {   for (i = 0; i < userlabels; i++)
        {   if (checkagainst_sym(number, (int) symlabel[i].address, thestring, both, i))
            {   foundequiv = ONE_MILLION + i;
                friendlycolour = TEXTPEN_RED; // means user label
                if (multimode)
                {   multifunc(number, symlabel[i].kind, multimode);
                } else
                {   return symlabel[i].kind;
    }   }   }   }
    for (i = FIRSTCPUEQUIV; i <= LASTCPUEQUIV; i++)
    {   if (checkagainst(number, (int) equivalents[i].address, thestring, both, i))
        {   foundequiv = i;
            friendlycolour = TEXTPEN_PURPLE; // means CPU
            if (multimode)
            {   multifunc(number, equivalents[i].kind, multimode);
            } else
            {   return equivalents[i].kind;
    }   }   }
    if (machines[machine].pvis)
    {   for (i = FIRSTPVIEQUIV; i < FIRSTPVIEQUIV + (33 * machines[machine].pvis); i++)
        {   if (checkagainst(number, (int) (pvibase + equivalents[i].address), thestring, both, i))
            {   foundequiv = i;
                friendlycolour = TEXTPEN_BLUE; // means machine
                if (multimode)
                {   multifunc(number, equivalents[i].kind, multimode);
                } else
                {   return equivalents[i].kind;
    }   }   }   }
    if (machines[machine].firstequiv != -1)
    {   for (i = machines[machine].firstequiv; i <= machines[machine].lastequiv; i++)
        {   if (checkagainst(number, (int) equivalents[i].address, thestring, both, i))
            {   foundequiv = i;
                friendlycolour = TEXTPEN_BLUE; // means machine
                if (multimode)
                {   multifunc(number, equivalents[i].kind, multimode);
                } else
                {   return equivalents[i].kind;
    }   }   }   }
    if (machine == BINBUG)
    {   if (firstdosequiv != -1)
        {   for (i = firstdosequiv; i <= lastdosequiv; i++)
            {   if (checkagainst(number, (int) equivalents[i].address, thestring, both, i))
                {   foundequiv = i;
                    friendlycolour = TEXTPEN_BLUE; // means machine
                    if (multimode)
                    {   multifunc(number, equivalents[i].kind, multimode);
                    } else
                    {   return equivalents[i].kind;
        }   }   }   }
        for (i = FIRSTACOSEQUIV; i <= LASTACOSEQUIV; i++)
        {   if (checkagainst(number, (int) equivalents[i].address, thestring, both, i))
            {   foundequiv = i;
                friendlycolour = TEXTPEN_BLUE; // means machine
                if (multimode)
                {   multifunc(number, equivalents[i].kind, multimode);
                } else
                {   return equivalents[i].kind;
    }   }   }   }
    if (whichgame != -1 && known[whichgame].firstequiv != -1)
    {   for (i = FIRSTGAMEEQUIV + known[whichgame].firstequiv; i <= FIRSTGAMEEQUIV + known[whichgame].lastequiv; i++)
        {   if (checkagainst(number, (int) equivalents[i].address, thestring, both, i))
            {   foundequiv = i;
                friendlycolour = TEXTPEN_GREEN; // means game
                if (multimode)
                {   multifunc(number, equivalents[i].kind, multimode);
                } else
                {   return equivalents[i].kind;
    }   }   }   }

    if (ISQWERTY && number <= 31)
    {   strcpy(friendly, asciiname_short[number]);
        foundequiv = -1;
        friendlycolour = TEXTPEN_ORANGE; // means guestchar
        if (multimode)
        {   multifunc(number, 'D', multimode);
        } else
        {   return 'D';
    }   }

    sprintf(thestring, "$%X", (unsigned int) number);
    foundequiv = -1;
    return EOS;
}

MODULE FLAG checkagainst(int number1, int number2, STRPTR thestring, FLAG both, int which)
{   if (number1 == number2)
    {   strcpy(thestring, (const char*) equivalents[which].name);
        if (both)
        {   sprintf(ENDOF(thestring), " ($%X)", (unsigned int) number1);
        }
        return TRUE;
    } // implied else
    return FALSE;
}

MODULE FLAG checkagainst_sym(int number1, int number2, STRPTR thestring, FLAG both, int which)
{   if (number1 == number2)
    {   strcpy(thestring, (const char*) symlabel[which].name);
        if (both)
        {   sprintf(ENDOF(thestring), " ($%X)", (unsigned int) number1);
        }
        return TRUE;
    } // implied else
    return FALSE;
}

EXPORT void check_labels(void)
{   int    i, j,
           thevalue;
    STRPTR thestring;
#ifdef CHECKLITERALS
    int    thelength;
    FLAG   ok;
#endif

    if
    (   machine == PONG
     || machine == TYPERIGHT
    )
    {   return;
    }

    // ASCII names
    if (ISQWERTY) // ISASCII is what we really mean in this case
    {   for (i = 0; i <= 258; i++)
        {   thestring = asciiname_short[i];
            thevalue = i;
            for (j = FIRSTTOKEN; j <= LASTTOKEN; j++)
            {   if (!stricmp(thestring, tokenname[j - FIRSTTOKEN][style]))
                {   zprintf(TEXTPEN_ERROR, "ASCII name %s ($%X) matches built-in token %s!\n", thestring, thevalue, thestring);
            }   }
            if (userlabels)
            {   for (j = 0; j < userlabels; j++)
                {   if (!stricmp(thestring, symlabel[j].name))
                    {   zprintf(TEXTPEN_ERROR, "ASCII name %s ($%X) matches user-defined label %s ($%X)!\n", thestring, thevalue, thestring, symlabel[j].address);
            }   }   }
            if (machines[machine].pvis)
            {   for (j = FIRSTPVIEQUIV; j < FIRSTPVIEQUIV + (33 * machines[machine].pvis); j++)
                {   if (!stricmp(thestring, equivalents[j].name))
                    {   zprintf(TEXTPEN_ERROR, "ASCII name %s ($%X) matches PVI label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address + pvibase);
            }   }   }
            if (machines[machine].firstequiv != -1)
            {   for (j = machines[machine].firstequiv; j <= machines[machine].lastequiv; j++)
                {   if (!stricmp(thestring, equivalents[j].name))
                    {   zprintf(TEXTPEN_ERROR, "ASCII name %s ($%X) matches machine label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
            }   }   }
            if (machine == BINBUG)
            {   if (firstdosequiv != -1)
                {   for (j = firstdosequiv; j <= lastdosequiv; j++)
                    {   if (!stricmp(thestring, equivalents[j].name))
                        {   zprintf(TEXTPEN_ERROR, "ASCII name %s ($%X) matches DOS label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
                }   }   }
                for (j = FIRSTACOSEQUIV; j <= LASTACOSEQUIV; j++)
                {   if (!stricmp(thestring, equivalents[j].name))
                    {   zprintf(TEXTPEN_ERROR, "ASCII name %s ($%X) matches ACOS label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
            }   }   }
            if (whichgame != -1 && known[whichgame].firstequiv != -1)
            {   for (j = FIRSTGAMEEQUIV + known[whichgame].firstequiv; j <= FIRSTGAMEEQUIV + known[whichgame].lastequiv; j++)
                {   if (!stricmp(thestring, equivalents[j].name))
                    {   zprintf(TEXTPEN_ERROR, "ASCII name %s ($%X) matches game label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
            }   }   }
#ifdef CHECKLITERALS
            thelength = strlen(thestring);
            if (thelength >= 1 && thelength <= 4)
            {   ok = FALSE;
                for (j = 0; j < thelength; j++)
                {   if
                    (   (thestring[j] >= 'A' && thestring[j] <= 'F') // all labels are uppercase
                     || (thestring[j] >= '0' && thestring[j] <= '9')
                    )
                    {   ;
                    } else
                    {   ok = TRUE;
                        break;
                }   }
                if (!ok)
                {   zprintf(TEXTPEN_ERROR, "ASCII name %s ($%X) is a hex literal!\n", thestring);
            }   }
#endif
    }   }

    // built-in tokens (registers, etc.)
    for (i = FIRSTTOKEN; i <= LASTTOKEN; i++)
    {   thestring = tokenname[i - FIRSTTOKEN][style];
        if (userlabels)
        {   for (j = 0; j < userlabels; j++)
            {   if (!stricmp(thestring, symlabel[i].name))
                {   zprintf(TEXTPEN_ERROR, "Built-in token %s matches user-defined label!\n", thestring);
        }   }   }
        if (machines[machine].pvis)
        {   for (j = FIRSTPVIEQUIV; j < FIRSTPVIEQUIV + (33 * machines[machine].pvis); j++)
            {   if (!stricmp(thestring, equivalents[j].name))
                {   zprintf(TEXTPEN_ERROR, "Built-in token %s matches PVI label!\n", thestring);
        }   }   }
        if (machines[machine].firstequiv != -1)
        {   for (j = machines[machine].firstequiv; j <= machines[machine].lastequiv; j++)
            {   if (!stricmp(thestring, equivalents[j].name))
                {   zprintf(TEXTPEN_ERROR, "Built-in token %s matches machine label!\n", thestring);
        }   }   }
        if (machine == BINBUG)
        {   if (firstdosequiv != -1)
            {   for (j = firstdosequiv; j <= lastdosequiv; j++)
                {   if (!stricmp(thestring, equivalents[j].name))
                    {   zprintf(TEXTPEN_ERROR, "Built-in token %s matches DOS label!\n", thestring);
            }   }   }
            for (j = FIRSTACOSEQUIV; j <= LASTACOSEQUIV; j++)
            {   if (!stricmp(thestring, equivalents[j].name))
                {   zprintf(TEXTPEN_ERROR, "Built-in token %s matches ACOS label!\n", thestring);
        }   }   }
        if (whichgame != -1 && known[whichgame].firstequiv != -1)
        {   for (j = FIRSTGAMEEQUIV + known[whichgame].firstequiv; j <= FIRSTGAMEEQUIV + known[whichgame].lastequiv; j++)
            {   if (!stricmp(thestring, equivalents[j].name))
                {   zprintf(TEXTPEN_ERROR, "Built-in token %s matches game label!\n", thestring);
        }   }   }
#ifdef CHECKLITERALS
        thelength = strlen(thestring);
        if (thelength >= 1 && thelength <= 4)
        {   ok = FALSE;
            for (j = 0; j < thelength; j++)
            {   if
                (   (thestring[j] >= 'A' && thestring[j] <= 'F') // all labels are uppercase
                 || (thestring[j] >= '0' && thestring[j] <= '9')
                )
                {   ;
                } else
                {   ok = TRUE;
                    break;
            }   }
            if (!ok)
            {   zprintf(TEXTPEN_ERROR, "Built-in token %s is a hex literal!\n", thestring);
        }   }
#endif
    }

    if (userlabels)
    {   for (i = 0; i < userlabels; i++)
        {   thestring = symlabel[i].name;
            thevalue = symlabel[i].address;
            if (machines[machine].pvis)
            {   for (j = FIRSTPVIEQUIV; j < FIRSTPVIEQUIV + (33 * machines[machine].pvis); j++)
                {   if (!stricmp(thestring, equivalents[j].name))
                    {   zprintf(TEXTPEN_ERROR, "User-defined label %s ($%X) matches PVI label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address + pvibase);
                    }   }   }
            if (machines[machine].firstequiv != -1)
            {   for (j = machines[machine].firstequiv; j <= machines[machine].lastequiv; j++)
                {   if (!stricmp(thestring, equivalents[j].name))
                    {   zprintf(TEXTPEN_ERROR, "User-defined label %s ($%X) matches machine label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
            }   }   }
            if (machine == BINBUG)
            {   if (firstdosequiv != -1)
                {   for (j = firstdosequiv; j <= lastdosequiv; j++)
                    {   if (!stricmp(thestring, equivalents[j].name))
                        {   zprintf(TEXTPEN_ERROR, "User-defined label %s ($%X) matches DOS label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
                }   }   }
                for (j = FIRSTACOSEQUIV; j <= LASTACOSEQUIV; j++)
                {   if (!stricmp(thestring, equivalents[j].name))
                    {   zprintf(TEXTPEN_ERROR, "User-defined label %s ($%X) matches ACOS label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
            }   }   }
            if (whichgame != -1 && known[whichgame].firstequiv != -1)
            {   for (j = FIRSTGAMEEQUIV + known[whichgame].firstequiv; j <= FIRSTGAMEEQUIV + known[whichgame].lastequiv; j++)
                {   if (!stricmp(thestring, equivalents[j].name))
                    {   zprintf(TEXTPEN_ERROR, "User-defined label %s ($%X) matches game label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
            }   }   }
#ifdef CHECKLITERALS
            thelength = strlen(thestring);
            if (thelength >= 1 && thelength <= 4)
            {   ok = FALSE;
                for (j = 0; j < thelength; j++)
                {   if
                    (   (thestring[j] >= 'A' && thestring[j] <= 'F') // all labels are uppercase
                     || (thestring[j] >= '0' && thestring[j] <= '9')
                    )
                    {   ;
                    } else
                    {   ok = TRUE;
                        break;
                }   }
                if (!ok)
                {   zprintf(TEXTPEN_ERROR, "User-defined label %s is a hex literal!\n", thestring);
            }   }
#endif
    }   }

    if (machines[machine].pvis)
    {   for (i = FIRSTPVIEQUIV; i < FIRSTPVIEQUIV + (33 * machines[machine].pvis); i++)
        {   thestring = equivalents[i].name;
            thevalue = equivalents[i].address + pvibase;
            if (machines[machine].firstequiv != -1)
            {   for (j = machines[machine].firstequiv; j <= machines[machine].lastequiv; j++)
                {   if (!stricmp(thestring, equivalents[j].name))
                    {   zprintf(TEXTPEN_ERROR, "PVI label %s ($%X) matches machine label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
            }   }   }
            if (machine == BINBUG)
            {   if (firstdosequiv != -1)
                {   for (j = firstdosequiv; j <= lastdosequiv; j++)
                    {   if (!stricmp(thestring, equivalents[j].name))
                        {   zprintf(TEXTPEN_ERROR, "PVI label %s ($%X) matches DOS label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
                }   }   }
                for (j = FIRSTACOSEQUIV; j <= LASTACOSEQUIV; j++)
                {   if (!stricmp(thestring, equivalents[j].name))
                    {   zprintf(TEXTPEN_ERROR, "PVI label %s ($%X) matches ACOS label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
            }   }   }
            if (whichgame != -1 && known[whichgame].firstequiv != -1)
            {   for (j = FIRSTGAMEEQUIV + known[whichgame].firstequiv; j <= FIRSTGAMEEQUIV + known[whichgame].lastequiv; j++)
                {   if (!stricmp(thestring, equivalents[j].name))
                    {   zprintf(TEXTPEN_ERROR, "PVI label %s ($%X) matches game label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
            }   }   }
#ifdef CHECKLITERALS
            thelength = strlen(thestring);
            if (thelength >= 1 && thelength <= 4)
            {   ok = FALSE;
                for (j = 0; j < thelength; j++)
                {   if
                    (   (thestring[j] >= 'A' && thestring[j] <= 'F') // all labels are uppercase
                     || (thestring[j] >= '0' && thestring[j] <= '9')
                    )
                    {   ;
                    } else
                    {   ok = TRUE;
                        break;
                }   }
                if (!ok)
                {   zprintf(TEXTPEN_ERROR, "PVI label %s is a hex literal!\n", thestring);
            }   }
#endif
    }   }

    if (machines[machine].firstequiv != -1)
    {   for (i = machines[machine].firstequiv; i <= machines[machine].lastequiv; i++)
        {   thestring = equivalents[i].name;
            thevalue = equivalents[i].address;
            if (machine == BINBUG)
            {   if (firstdosequiv != -1)
                {   for (j = firstdosequiv; j <= lastdosequiv; j++)
                    {   if (!stricmp(thestring, equivalents[j].name))
                        {   zprintf(TEXTPEN_ERROR, "Machine label %s ($%X) matches DOS label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
                }   }   }
                for (j = FIRSTACOSEQUIV; j <= LASTACOSEQUIV; j++)
                {   if (!stricmp(thestring, equivalents[j].name))
                    {   zprintf(TEXTPEN_ERROR, "Machine label %s ($%X) matches ACOS label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
            }   }   }
            if (whichgame != -1 && known[whichgame].firstequiv != -1)
            {   for (j = FIRSTGAMEEQUIV + known[whichgame].firstequiv; j <= FIRSTGAMEEQUIV + known[whichgame].lastequiv; j++)
                {   if (!stricmp(thestring, equivalents[j].name))
                    {   zprintf(TEXTPEN_ERROR, "Machine label %s ($%X) matches game label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
            }   }   }
#ifdef CHECKLITERALS
            thelength = strlen(thestring);
            if (thelength >= 1 && thelength <= 4)
            {   ok = FALSE;
                for (j = 0; j < thelength; j++)
                {   if
                    (   (thestring[j] >= 'A' && thestring[j] <= 'F') // all labels are uppercase
                     || (thestring[j] >= '0' && thestring[j] <= '9')
                    )
                    {   ;
                    } else
                    {   ok = TRUE;
                        break;
                }   }
                if (!ok)
                {   zprintf(TEXTPEN_ERROR, "Machine label %s is a hex literal!\n", thestring);
            }   }
#endif
    }   }

    if (machine == BINBUG)
    {   if (firstdosequiv != -1)
        {   for (i = firstdosequiv; i <= lastdosequiv; i++)
            {   thestring = equivalents[i].name;
                thevalue = equivalents[i].address;
                for (j = FIRSTACOSEQUIV; j <= LASTACOSEQUIV; j++)
                {   if (!stricmp(thestring, equivalents[j].name))
                    {   zprintf(TEXTPEN_ERROR, "DOS label %s ($%X) matches ACOS label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
                }   }
                if (whichgame != -1 && known[whichgame].firstequiv != -1)
                {   for (j = FIRSTGAMEEQUIV + known[whichgame].firstequiv; j <= FIRSTGAMEEQUIV + known[whichgame].lastequiv; j++)
                    {   if (!stricmp(thestring, equivalents[j].name))
                        {   zprintf(TEXTPEN_ERROR, "DOS label %s ($%X) matches game label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
                }   }   }
#ifdef CHECKLITERALS
                thelength = strlen(thestring);
                if (thelength >= 1 && thelength <= 4)
                {   ok = FALSE;
                    for (j = 0; j < thelength; j++)
                    {   if
                        (   (thestring[j] >= 'A' && thestring[j] <= 'F') // all labels are uppercase
                         || (thestring[j] >= '0' && thestring[j] <= '9')
                        )
                        {   ;
                        } else
                        {   ok = TRUE;
                            break;
                    }   }
                    if (!ok)
                    {   zprintf(TEXTPEN_ERROR, "DOS label %s is a hex literal!\n", thestring);
                }   }
#endif
        }   }

        for (i = FIRSTACOSEQUIV; i <= LASTACOSEQUIV; i++)
        {   thestring = equivalents[i].name;
            thevalue = equivalents[i].address;
            if (whichgame != -1 && known[whichgame].firstequiv != -1)
            {   for (j = FIRSTGAMEEQUIV + known[whichgame].firstequiv; j <= FIRSTGAMEEQUIV + known[whichgame].lastequiv; j++)
                {   if (!stricmp(thestring, equivalents[j].name))
                    {   zprintf(TEXTPEN_ERROR, "ACOS label %s ($%X) matches game label %s ($%X)!\n", thestring, thevalue, thestring, equivalents[j].address);
            }   }   }
#ifdef CHECKLITERALS
            thelength = strlen(thestring);
            if (thelength >= 1 && thelength <= 4)
            {   ok = FALSE;
                for (j = 0; j < thelength; j++)
                {   if
                    (   (thestring[j] >= 'A' && thestring[j] <= 'F') // all labels are uppercase
                     || (thestring[j] >= '0' && thestring[j] <= '9')
                    )
                    {   ;
                    } else
                    {   ok = TRUE;
                        break;
                }   }
                if (!ok)
                {   zprintf(TEXTPEN_ERROR, "ACOS label %s is a hex literal!\n", thestring);
            }   }
#endif
    }   }

#ifdef CHECKLITERALS
    if (whichgame != -1 && known[whichgame].firstequiv != -1)
    {   for (i = FIRSTGAMEEQUIV + known[whichgame].firstequiv; i <= FIRSTGAMEEQUIV + known[whichgame].lastequiv; i++)
        {   thestring = equivalents[i].name;
            thelength = strlen(thestring);
            if (thelength >= 1 && thelength <= 4)
            {   ok = FALSE;
                for (j = 0; j < thelength; j++)
                {   if
                    (   (thestring[j] >= 'A' && thestring[j] <= 'F') // all labels are uppercase
                     || (thestring[j] >= '0' && thestring[j] <= '9')
                    )
                    {   ;
                    } else
                    {   ok = TRUE;
                        break;
                }   }
                if (!ok)
                {   zprintf(TEXTPEN_ERROR, "Game label %s is a hex literal!\n", thestring);
    }   }   }   }
#endif
}

EXPORT void logport(int port, UBYTE data, FLAG write)
{   PERSIST TEXT  asciival[3 + 1],
                  portstr[80 + 1];
    FAST    UBYTE oldval;

    if (write)
    {   coverage_io[port] |= COVERAGE_WRITE;
        if
        (   ioport[port].ip == FALSE
         || watchwrites == WATCH_NONE
         || (watchwrites == WATCH_SOME && ioport[port].contents == data)
         || !conditional(&ioport[port].cond, data, write, 1)
        )
        {   ioport[port].contents = data;
            return;
        } else
        {   oldval = ioport[port].contents;
            ioport[port].contents = data;
    }   }
    else
    {   coverage_io[port] |= COVERAGE_READ;
        if
        (   ioport[port].ip == FALSE
         || !watchreads
         || !conditional(&ioport[port].cond, data, write, 1)
        )
        {   return;
    }   }

    switch (port)
    {
    case PORTC:
        strcpy
        (   (char*) portstr,
            LLL(
                MSG_IOCONTROLPORT,
                "I/O control port"
        )   );
    acase PORTD:
        strcpy
        (   (char*) portstr,
            LLL(
                MSG_IODATAPORT,
                "I/O data port"
        )   );
    adefault:
        sprintf
        (   (char*) portstr,
            LLL(
                MSG_IOEXTENDEDPORT,
                "extended I/O port $%02X"
            ),
            (unsigned int) port
        );
    }
    DISCARD getfriendly((int) iar);

    if (machine == INSTRUCTOR)
    {   asciival[0] =
        asciival[2] = QUOTE;
        asciival[1] = led_chars[data & 0x7F];
        asciival[3] = EOS;
    } else
    {   strcpy((char*) asciival, (const char*) asciiname_short[data]);
    }

    if (write)
    {   zprintf
        (   TEXTPEN_LOG,
            LLL
            (   MSG_HANDLEDWRITE,
                "Handled write of $%02X (%s) to %s at address %s. Previous value was $%02X.\n"
            ),
            data,
            asciival,
            portstr,
            friendly,
            oldval
        );
        set_pause(TYPE_BP);
    } else
    {   zprintf
        (   TEXTPEN_LOG,
            LLL
            (   MSG_HANDLEDREAD,
                "Handled read of $%02X (%s) from %s at address %s.\n"
            ),
            data,
            asciival,
            portstr,
            friendly
        );
        set_pause(TYPE_BP);
    }

    if (verbosity && machine == TWIN)
    {   twin_logport(port, data, write);
    }

    zprintf(TEXTPEN_LOG, "\n");
}

EXPORT void breakrastline(void)
{   if (cpuy == rastn)
    {   zprintf
        (   TEXTPEN_DEBUG,
            LLL(MSG_XVI_REACHEDRASTLINEN, "Reached raster line %ld.\n\n"),
            (long int) cpuy
        );
        set_pause(TYPE_RUNTO);
}   }

EXPORT TEXT guestchar(UBYTE input)
{   FAST TEXT thechar;

    switch (machine)
    {
    case ARCADIA:
        thechar = arcadia_chars[input & 0x3F];
    acase ELEKTOR:
        thechar = elektor_chars1[input];
    acase PIPBUG:
    case BINBUG:
    case PHUNSY:
    case SELBST:
    case TWIN:
        if
        (    input <=  31
         || (input >= 0x80 && input <= 0x9F)
#ifdef WIN32
         ||  input >= 128
#endif
        )
        {   thechar = (TEXT) '·';
        } else
        {   thechar = input;
        }
    acase INSTRUCTOR:
        thechar = instructor_chars1[input];
    acase CD2650:
        thechar = cd2650_chars[input & 0x7F];
    acase ZACCARIA:
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:
            thechar = astrowars_chars[input];
        acase MEMMAP_GALAXIA:
            thechar = galaxia_chars[input & 0x7F];
        acase MEMMAP_LASERBATTLE:
        case MEMMAP_LAZARIAN:
            thechar = lb_chars[input];
        }
    acase MALZAK:
        if ((input & 0x7F) <= 31)
        {   thechar = (TEXT) ' ';
        } else
        {   thechar = (TEXT) (input & 0x7F);
        }
    adefault: // eg. INTERTON, PONG, MIKIT
        thechar = (TEXT) '·';
    }

#ifdef WIN32
    if (thechar == (TEXT) '·')
    {   thechar = '.';
    }
#endif

    return thechar;
}

EXPORT FLAG allowable(FLAG loud)
{   if (machine != PONG && machine != TYPERIGHT)
    {   return TRUE;
    } // implied else

    if (loud)
    {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
    }
    return FALSE;
}

EXPORT void dumprow(int i)
{   PERSIST int  x;
    PERSIST TEXT rowchars[16 + 1];

    if (machine == INTERTON)
    {   rowchars[0] = EOS;
    } else
    {   for (x = 0; x < 16; x++)
        {   rowchars[x] = guestchar(memory[mirror_r[i + x]]);
        }
        rowchars[16] = EOS;
    }
    zprintf
    (   TEXTPEN_VIEW,
        "$%04X: %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X %02X %02X %s\n",
        i,
        memory[mirror_r[i     ]],
        memory[mirror_r[i +  1]],
        memory[mirror_r[i +  2]],
        memory[mirror_r[i +  3]],
        memory[mirror_r[i +  4]],
        memory[mirror_r[i +  5]],
        memory[mirror_r[i +  6]],
        memory[mirror_r[i +  7]],
        memory[mirror_r[i +  8]],
        memory[mirror_r[i +  9]],
        memory[mirror_r[i + 10]],
        memory[mirror_r[i + 11]],
        memory[mirror_r[i + 12]],
        memory[mirror_r[i + 13]],
        memory[mirror_r[i + 14]],
        memory[mirror_r[i + 15]],
        rowchars
    );
}

EXPORT void addhistory(void)
{   int i, j;

    if (userinput[0] != EOS)
    {   for (i = HISTORYLINES - 1; i >= 1; i--)
        {   strcpy((char*) olduserinput[i], (const char*) olduserinput[i - 1]);
        }
        strcpy((char*) olduserinput[0], (const char*) userinput);

        // now remove all duplicates
        for (i = 1; i < HISTORYLINES; i++)
        {   if (olduserinput[i][0] == EOS)
            {   break; // done
            }
            while (!strcmp((const char*) olduserinput[0], (const char*) olduserinput[i]))
            {   if (i < HISTORYLINES - 1)
                {   for (j = i; j < HISTORYLINES - 1; j++)
                    {   strcpy((char*) olduserinput[j], (const char*) olduserinput[j + 1]);
                }   }
                olduserinput[HISTORYLINES - 1][0] = EOS;
}   }   }   }

EXPORT void view_ram(void)
{   int  i, j, k,
         limit;
    TEXT asciiversion[16 + 1];

    if (machine == PONG || machine == TYPERIGHT)
    {   return;
    }

    zprintf(TEXTPEN_VIEW, LLL(MSG_MOTHERBOARDRAM, "Motherboard RAM:\n"));

    switch (machine)
    {
    case ARCADIA:
        // Only non-display RAM is shown by this.

        if (memmap == MEMMAP_H)
        {   for (i = 0x1000; i <= 0x10FF; i += 16) dumprow(i);
            for (i = 0x1200; i <= 0x12FF; i += 16) dumprow(i);
            zprintf(TEXTPEN_VIEW, "\n");
        }

        dumprow(0x18D0);
        dumprow(0x18E0);
        zprintf
        (   TEXTPEN_VIEW,
            "$18F8:                          %02X %02X %02X %02X\n",
            memory[0x18F8],
            memory[0x18F9],
            memory[0x18FA],
            memory[0x18FB]
        );

        dumprow(0x1AD0);
        dumprow(0x1AE0);
        dumprow(0x1AF0);
    acase INTERTON:
    case ELEKTOR:
        if (machine == ELEKTOR)
        {   // monitor RAM
            for (i = 0x800; i <= 0x8BF; i += 16)
            {   dumprow(i);
            }
            zprintf(TEXTPEN_VIEW, "\n");

            if (memmap == MEMMAP_E)
            {   limit = 0x0FFF;
            } else
            {   // assert(memmap == MEMMAP_F);
                limit = 0x15FF;
            }

            // user RAM
            for (i = 0x8C0; i <= limit; i += 16)
            {   dumprow(i);
            }
            zprintf(TEXTPEN_VIEW, "\n");

            if (memmap == MEMMAP_F)
            {   // user RAM
                // 8 bytes in this range can be actually obscured RAM, so
                // shouldn't really be shown (if old monitor ROM is in
                // use).
                for (i = 0x1800; i <= 0x1BFF; i += 16)
                {   dumprow(i);
                }
                zprintf(TEXTPEN_VIEW, "\n");
        }   }
        view_ram_pvi();

        if (memmap == MEMMAP_C || memmap == MEMMAP_D)
        {   zprintf(TEXTPEN_VIEW, LLL(MSG_CARTRIDGERAM, "Cartridge RAM:\n"));
            if (memmap == MEMMAP_C)
            {   limit = 0x1000; // 1K ($1000..$13FF)
            } else
            {   // assert(memmap == MEMMAP_D);
                limit = 0x1800; // 1K ($1800..$1BFF)
            }
            for (i = limit; i <= limit + 1023; i += 16)
            {   dumprow(i);
        }   }
    acase PIPBUG:
        switch (memmap)
        {
        case  MEMMAP_PIPBUG1: for (i = 0x400; i <= 0x7FFF; i += 16) dumprow(i); // 31K
        acase MEMMAP_PIPBUG2: for (i = 0x800; i <=  0x87F; i += 16) dumprow(i); // 128 bytes
                              for (i = 0xC00; i <=  0xFFF; i += 16) dumprow(i); // 1K
        }
    acase BINBUG:
        for (i =  0x400; i <= 0x1FFF; i += 16) dumprow(i);
    acase INSTRUCTOR:
        for (i =    0x0; i <=  0x1FF; i += 16) dumprow(i);
        for (i = 0x1780; i <= 0x17FF; i += 16) dumprow(i);
    acase CD2650:
        for (i =  0x400; i <= 0x17FF; i += 16) dumprow(i);
        for (i = 0x2000; i <= 0x7FFF; i += 16) dumprow(i);
    acase MALZAK:
        for (i = 0x1000; i <= 0x13FF; i += 16) dumprow(i);
        view_ram_pvi();
        pvibase = 0x1500;
        view_ram_pvi();
        pvibase = 0x1400;
        for (i = 0x1600; i <= 0x1FFF; i += 16) dumprow(i);
        if (memmap == MEMMAP_MALZAK2)
        {   zprintf(TEXTPEN_VIEW, "Potentiometer ($14CC): $%02X\n\n", memory[0x14CC]);
        }
    acase ZACCARIA:
        if (memmap == MEMMAP_ASTROWARS)
        {   for (i = 0x1400; i <= 0x14FF; i += 16) dumprow(i);
            view_ram_pvi();
        } else
        {   // assert(memmap == MEMMAP_GALAXIA || memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN);
            for (i = 0x1400; i <= 0x14FF; i += 16) dumprow(i);
            view_ram_pvi();
            pvibase = 0x1600;
            view_ram_pvi();
            pvibase = 0x1700;
            view_ram_pvi();
            pvibase = 0x1500;
            if (memmap == MEMMAP_GALAXIA)
            {   for (i = 0x1C00; i <= 0x1FFF; i += 16) dumprow(i);
            } else
            {   // assert(memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN);
                for (i = 0x1C00; i <= 0x73FF; i += 16) dumprow(i);
        }   }
    acase PHUNSY:
        // Only non-display RAM is shown by this.

        for (i =  0x800; i <=  0xFFF; i += 16) dumprow(i);

        asciiversion[16] = EOS;
        for (i = 0; i <= 15; i++)
        {   zprintf(TEXTPEN_VIEW, "\nBank U%X ($1800..$1FFF) (currently using U%X):\n", i, (banked & 0xF0) >> 4);
            for (j = 0; j < 0x800; j += 16)
            {   for (k = 0; k < 16; k++)
                {   if (u_bank[i][j + k] <= 31) asciiversion[k] = '.'; else asciiversion[k] = u_bank[i][j + k];
                }
                zprintf
                (   TEXTPEN_VIEW,
                    "$%04X: %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X %02X %02X %s\n",
                    0x1800 + j,
                    u_bank[i][j     ],
                    u_bank[i][j +  1],
                    u_bank[i][j +  2],
                    u_bank[i][j +  3],
                    u_bank[i][j +  4],
                    u_bank[i][j +  5],
                    u_bank[i][j +  6],
                    u_bank[i][j +  7],
                    u_bank[i][j +  8],
                    u_bank[i][j +  9],
                    u_bank[i][j + 10],
                    u_bank[i][j + 11],
                    u_bank[i][j + 12],
                    u_bank[i][j + 13],
                    u_bank[i][j + 14],
                    u_bank[i][j + 15],
                    asciiversion
                );
        }   }

        for (i = 0x2000; i <= 0x3FFF; i += 16) dumprow(i);

        for (i = 0; i <= 15; i++)
        {   zprintf(TEXTPEN_VIEW, "\nBank Q%X ($4000..$7FFF) (currently using Q%X):\n", i, banked & 0x0F);
            for (j = 0; j < 0x4000; j += 16)
            {   for (k = 0; k < 16; k++)
                {   if (q_bank[i][j + k] <= 31) asciiversion[k] = '.'; else asciiversion[k] = q_bank[i][j + k];
                }
                zprintf
                (   TEXTPEN_VIEW,
                    "$%04X: %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X %02X %02X %s\n",
                    0x4000 + j,
                    q_bank[i][j     ],
                    q_bank[i][j +  1],
                    q_bank[i][j +  2],
                    q_bank[i][j +  3],
                    q_bank[i][j +  4],
                    q_bank[i][j +  5],
                    q_bank[i][j +  6],
                    q_bank[i][j +  7],
                    q_bank[i][j +  8],
                    q_bank[i][j +  9],
                    q_bank[i][j + 10],
                    q_bank[i][j + 11],
                    q_bank[i][j + 12],
                    q_bank[i][j + 13],
                    q_bank[i][j + 14],
                    q_bank[i][j + 15],
                    asciiversion
                );
        }   }
    acase SELBST:
        for (i = 0x800; i <= 0x1FFF; i += 16) dumprow(i);
    }

    zprintf(TEXTPEN_VIEW, "\n");
}

EXPORT void view_xvi(void)
{   TRANSIENT       int    whichpvi;
    PERSIST   const STRPTR spritesizes[4] = { "single", "double", "quadruple", "octuple" };

    if (machines[machine].pvis == 0 && machine != ARCADIA) // eg. PIPBUG, BINBUG, INSTRUCTOR, CD2650, PHUNSY
    {   return;
    }

    switch (machine)
    {
    case ARCADIA:
        zprintf
        (   TEXTPEN_VIEW,
            "CONSOLE:        %02X PITCH:          %02X VOLUME:         %02X\n",
            memory[A_CONSOLE],
            memory[A_PITCH],
            memory[A_VOLUME]
        );
        zprintf
        (   TEXTPEN_VIEW,
            "GFXMODE:        %02X BGCOLOUR:       %02X VSCROLL:        %02X CHARLINE:       %02X\n",
            memory[A_GFXMODE],
            memory[A_BGCOLOUR],
            memory[A_VSCROLL],
            memory[A_CHARLINE]
        );
        zprintf
        (   TEXTPEN_VIEW,
            "BGCOLLIDE:      %02X SPRITECOLLIDE:  %02X SPRITES01CTRL:  %02X SPRITES23CTRL:  %02X\n",
            memory[A_BGCOLLIDE    ],
            memory[A_SPRITECOLLIDE],
            memory[A_SPRITES01CTRL],
            memory[A_SPRITES23CTRL]
        );
        zprintf
        (   TEXTPEN_VIEW,
            "SPRITE0X,Y:  %02X,%02X SPRITE1X,Y:  %02X,%02X SPRITE2X,Y:  %02X,%02X SPRITE3X,Y:  %02X,%02X\n",
            memory[A_SPRITE0X],
            memory[A_SPRITE0Y],
            memory[A_SPRITE1X],
            memory[A_SPRITE1Y],
            memory[A_SPRITE2X],
            memory[A_SPRITE2Y],
            memory[A_SPRITE3X],
            memory[A_SPRITE3Y]
        );
        zprintf
        (   TEXTPEN_VIEW,
            "P1PADDLE:       %02X P1LEFTKEYS:     %02X P1MIDDLEKEYS:   %02X P1RIGHTKEYS:    %02X\n",
            memory[A_P1PADDLE],
            memory[A_P1LEFTKEYS],
            memory[A_P1MIDDLEKEYS],
            memory[A_P1RIGHTKEYS]
        );
        zprintf
        (   TEXTPEN_VIEW,
            "P2PADDLE:       %02X P2LEFTKEYS:     %02X P2MIDDLEKEYS:   %02X P2RIGHTKEYS:    %02X\n",
            memory[A_P2PADDLE],
            memory[A_P2LEFTKEYS],
            memory[A_P2MIDDLEKEYS],
            memory[A_P2RIGHTKEYS]
        );
        zprintf
        (   TEXTPEN_VIEW,
            "P1PALLADIUM:    %02X P2PALLADIUM:    %02X\n",
            memory[A_P1PALLADIUM],
            memory[A_P2PALLADIUM]
        );

        if (verbosity)
        {   zprintf
            (   TEXTPEN_VIEW,
                "%-27s %s\n",
                LLL(MSG_BOARDMODE, "Board mode:"),
                (memory[A_PITCH     ] & 0x80) ? LLL(MSG_ENGINE_YES2, "yes")
                                              : LLL(MSG_ENGINE_NO2,  "no")
            );
            zprintf
            (   TEXTPEN_VIEW,
                "%-27s %s\n",
                LLL(MSG_BLOCKMODE, "Block mode:"),
                (memory[A_GFXMODE   ] & 0x80) ? LLL(MSG_ENGINE_YES2, "yes")
                                              : LLL(MSG_ENGINE_NO2,  "no")
            );

            zprintf
            (   TEXTPEN_VIEW,
                "%-27s %4.3f %s (note %s)\n",
                LLL(MSG_PITCH, "Pitch:"),
                (float) (dividend / ((memory[A_PITCH] & 0x7F) + 1)),
                LLL(MSG_HERTZ, "Hz"),
                (region == REGION_NTSC) ? (notes[idealfreq_ntsc[memory[A_PITCH] & 0x7F].whichnote].name) : (notes[idealfreq_pal[memory[A_PITCH] & 0x7F].whichnote].name)
            );
            zprintf
            (   TEXTPEN_VIEW,
                "%-27s %d %s\n",
                LLL(MSG_HORIZSCROLLING, "Horizontal scrolling:"),
                (memory[A_VOLUME] & 0xE0) >> 5,
                LLL(MSG_PIXELS, "pixels")
            );
            zprintf
            (   TEXTPEN_VIEW,
                "%-27s %s\n",
                LLL(MSG_TONE, "Tone:"),
                (memory[A_VOLUME] & 0x08) ? LLL(MSG_ENGINE_ON, "on")
                                          : LLL(MSG_ENGINE_OFF, "off")
            );
            zprintf
            (   TEXTPEN_VIEW,
                "%-27s %s\n",
                LLL(MSG_NOISE, "Noise:"),
                (memory[A_VOLUME] & 0x10) ? LLL(MSG_ENGINE_ON, "on")
                                          : LLL(MSG_ENGINE_OFF, "off")
            );
            zprintf
            (   TEXTPEN_VIEW,
                "%-27s %d %s 7\n",
                LLL(MSG_VOLUME, "Volume:"),
                memory[A_VOLUME] & 0x07,
                LLL(MSG_OF, "of")
            );
            zprintf
            (   TEXTPEN_VIEW,
                "%-27s %d\n",
                LLL(MSG_CHARROWS, "Character rows:"),
                (memory[A_GFXMODE] & 0x40) ? 26 : 13
            );
            zprintf
            (   TEXTPEN_VIEW,
                "%-27s %s\n",
                LLL(MSG_VERTRES, "Vertical resolution:"),
                (memory[A_BGCOLOUR] & 0x80) ? LLL(MSG_ENGINE_HIGH, "high")
                                            : LLL(MSG_ENGINE_LOW,  "low")
            );
            zprintf
            (   TEXTPEN_VIEW,
                "%-27s %s\n",
                LLL(MSG_PADDLEINT, "Paddle interpolation:"),
                (memory[A_BGCOLOUR] & 0x40) ? LLL(MSG_XAXIS, "X-axis")
                                            : LLL(MSG_YAXIS, "Y-axis")
            );
            zprintf
            (   TEXTPEN_VIEW,
                "%s\n %-26s %s, %s, %s, %s\n",
                LLL(MSG_COLOURS, "Colours:"),
                LLL(MSG_FOREGROUND, "Foreground:"),
                colourname[from_a[  (memory[A_BGCOLOUR] & 0x38) >> 3          ]],
                colourname[from_a[(((memory[A_BGCOLOUR] & 0x38) >> 3) + 2) % 8]],
                colourname[from_a[(((memory[A_BGCOLOUR] & 0x38) >> 3) + 4) % 8]],
                colourname[from_a[(((memory[A_BGCOLOUR] & 0x38) >> 3) + 6) % 8]]
            );
            zprintf
            (   TEXTPEN_VIEW,
                " %-26s %s\n",
                LLL(MSG_BOARDMODE, "Board mode:"),
                colourname[from_a[(memory[A_GFXMODE] & 0x38) >> 3]]
            );
            zprintf
            (   TEXTPEN_VIEW,
                " %-26s %s\n",
                LLL(MSG_INNERBG, "Inner background:"),
                colourname[from_a[memory[A_BGCOLOUR  ] & 0x07]]
            );
            zprintf
            (   TEXTPEN_VIEW,
                " %-26s %s\n",
                LLL(MSG_OUTERBG, "Outer background:"),
                colourname[from_a[memory[A_GFXMODE] & 0x07]]
            );
            zprintf
            (   TEXTPEN_VIEW,
                " %-25s: %s, %s, %s, %s\n",
                LLL(MSG_SPRITES_MENU, "Sprites"),
                colourname[from_a[(memory[A_SPRITES01CTRL] & 0x38) >> 3]],
                colourname[from_a[(memory[A_SPRITES01CTRL] & 0x07)     ]],
                colourname[from_a[(memory[A_SPRITES23CTRL] & 0x38) >> 3]],
                colourname[from_a[(memory[A_SPRITES23CTRL] & 0x07)     ]]
            );

            zprintf
            (   TEXTPEN_VIEW,
                "%-27s %s, %s, %s, %s\n",
                "Sprite heights:",
                (memory[A_SPRITES01CTRL] & 0x80) ? "double" : "normal",
                (memory[A_SPRITES01CTRL] & 0x40) ? "double" : "normal",
                (memory[A_SPRITES23CTRL] & 0x80) ? "double" : "normal",
                (memory[A_SPRITES23CTRL] & 0x40) ? "double" : "normal"
            );

            zprintf
            (   TEXTPEN_VIEW,
                "%s\n %-26s %s, %s, %s, %s\n",
                LLL(MSG_COLLISIONS, "Collisions:"),
                LLL(MSG_SPRVSBG, "Sprites vs. background:"),
                (memory[A_BGCOLLIDE] & 0x01) ? LLL(MSG_ENGINE_NO2,  "no")
                                             : LLL(MSG_ENGINE_YES2, "yes"),
                (memory[A_BGCOLLIDE] & 0x02) ? LLL(MSG_ENGINE_NO2,  "no")
                                             : LLL(MSG_ENGINE_YES2, "yes"),
                (memory[A_BGCOLLIDE] & 0x04) ? LLL(MSG_ENGINE_NO2,  "no")
                                             : LLL(MSG_ENGINE_YES2, "yes"),
                (memory[A_BGCOLLIDE] & 0x08) ? LLL(MSG_ENGINE_NO2,  "no")
                                             : LLL(MSG_ENGINE_YES2, "yes")
            );
            sprintf
            (   (char*) gtempstring,
                "%s #0 %s #1/#2/#3:",
                LLL(MSG_SPRITE2, "Sprite"),
                LLL(MSG_VERSUS, "vs.")
            );
            zprintf
            (   TEXTPEN_VIEW,
                " %-26s %s, %s, %s\n",
                gtempstring,
                (memory[A_SPRITECOLLIDE] & 0x01) ? LLL(MSG_ENGINE_NO2,  "no")
                                                 : LLL(MSG_ENGINE_YES2, "yes"),
                (memory[A_SPRITECOLLIDE] & 0x02) ? LLL(MSG_ENGINE_NO2,  "no")
                                                 : LLL(MSG_ENGINE_YES2, "yes"),
                (memory[A_SPRITECOLLIDE] & 0x04) ? LLL(MSG_ENGINE_NO2,  "no")
                                                 : LLL(MSG_ENGINE_YES2, "yes")
            );
            sprintf
            (   (char*) gtempstring,
                "%s #1 %s #0/#2/#3:",
                LLL(MSG_SPRITE2, "Sprite"),
                LLL(MSG_VERSUS, "vs.")
            );
            zprintf
            (   TEXTPEN_VIEW,
                " %-26s %s, %s, %s\n",
                gtempstring,
                (memory[A_SPRITECOLLIDE] & 0x01) ? LLL(MSG_ENGINE_NO2,  "no")
                                                 : LLL(MSG_ENGINE_YES2, "yes"),
                (memory[A_SPRITECOLLIDE] & 0x08) ? LLL(MSG_ENGINE_NO2,  "no")
                                                 : LLL(MSG_ENGINE_YES2, "yes"),
                (memory[A_SPRITECOLLIDE] & 0x10) ? LLL(MSG_ENGINE_NO2,  "no")
                                                 : LLL(MSG_ENGINE_YES2, "yes")
            );
            sprintf
            (   (char*) gtempstring,
                "%s #2 %s #0/#1/#3:",
                LLL(MSG_SPRITE2, "Sprite"),
                LLL(MSG_VERSUS, "vs.")
            );
            zprintf
            (   TEXTPEN_VIEW,
                " %-26s %s, %s, %s\n",
                gtempstring,
                (memory[A_SPRITECOLLIDE] & 0x02) ? LLL(MSG_ENGINE_NO2,  "no")
                                                 : LLL(MSG_ENGINE_YES2, "yes"),
                (memory[A_SPRITECOLLIDE] & 0x08) ? LLL(MSG_ENGINE_NO2,  "no")
                                                 : LLL(MSG_ENGINE_YES2, "yes"),
                (memory[A_SPRITECOLLIDE] & 0x20) ? LLL(MSG_ENGINE_NO2,  "no")
                                                 : LLL(MSG_ENGINE_YES2, "yes")
            );
            sprintf
            (   (char*) gtempstring,
                "%s #3 %s #0/#1/#2:",
                LLL(MSG_SPRITE2, "Sprite"),
                LLL(MSG_VERSUS, "vs.")
            );
            zprintf
            (   TEXTPEN_VIEW,
                " %-26s %s, %s, %s\n",
                gtempstring,
                (memory[A_SPRITECOLLIDE] & 0x04) ? LLL(MSG_ENGINE_NO2,  "no")
                                                 : LLL(MSG_ENGINE_YES2, "yes"),
                (memory[A_SPRITECOLLIDE] & 0x10) ? LLL(MSG_ENGINE_NO2,  "no")
                                                 : LLL(MSG_ENGINE_YES2, "yes"),
                (memory[A_SPRITECOLLIDE] & 0x20) ? LLL(MSG_ENGINE_NO2,  "no")
                                                 : LLL(MSG_ENGINE_YES2, "yes")
            );
        }
    adefault:
        if (machine == INTERTON || machine == ELEKTOR)
        {   zprintf
            (   TEXTPEN_VIEW,
                "CONSOLE:        %02X NOISE:          %02X\n",
                memory[IE_CONSOLE],
                memory[IE_NOISE]
            );
        }

        if (machines[machine].pvis >= 2)
        {   zprintf(TEXTPEN_VIEW, "---1st PVI---\n");
        } elif (machine == INTERTON || machine == ELEKTOR)
        {   zprintf
            (   TEXTPEN_VIEW,
                "P1LEFTKEYS:     %02X P1MIDDLEKEYS:   %02X P1RIGHTKEYS:    %02X\n",
                memory[IE_P1LEFTKEYS],
                memory[IE_P1MIDDLEKEYS],
                memory[IE_P1RIGHTKEYS]
            );
            zprintf
            (   TEXTPEN_VIEW,
                "P2LEFTKEYS:     %02X P2MIDDLEKEYS:   %02X P2RIGHTKEYS:    %02X\n",
                memory[IE_P2LEFTKEYS],
                memory[IE_P2MIDDLEKEYS],
                memory[IE_P2RIGHTKEYS]
            );
        }

        for (whichpvi = 0; whichpvi <= 2; whichpvi++)
        {   // don't adjust pvibase!
            if (whichpvi >= machines[machine].pvis)
            {   break;
            }
            if (whichpvi == 1)
            {   zprintf(TEXTPEN_VIEW, "---2nd PVI---\n");
            } elif (whichpvi == 2)
            {   zprintf(TEXTPEN_VIEW, "---3rd PVI---\n");
            }

            zprintf
            (   TEXTPEN_VIEW,
                "PITCH:          %02X SIZES:          %02X P1PADDLE:       %02X P2PADDLE:       %02X\n",
                memory[pvibase + (0x100 * whichpvi) + PVI_PITCH],
                memory[pvibase + (0x100 * whichpvi) + PVI_SIZES],
                memory[pvibase + (0x100 * whichpvi) + PVI_P1PADDLE],
                memory[pvibase + (0x100 * whichpvi) + PVI_P2PADDLE]
            );
            zprintf
            (   TEXTPEN_VIEW,
                "SCORECTRL:      %02X SCORELT:        %02X SCORERT:        %02X BGCOLOUR:       %02X\n",
                memory[pvibase + (0x100 * whichpvi) + PVI_SCORECTRL],
                memory[pvibase + (0x100 * whichpvi) + PVI_SCORELT],
                memory[pvibase + (0x100 * whichpvi) + PVI_SCORERT],
                memory[pvibase + (0x100 * whichpvi) + PVI_BGCOLOUR]
            );
            zprintf
            (   TEXTPEN_VIEW,
                "BGCOLLIDE:      %02X SPRITECOLLIDE:  %02X SPR01COLOURS:   %02X SPR23COLOURS:   %02X\n",
                memory[pvibase + (0x100 * whichpvi) + PVI_BGCOLLIDE],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITECOLLIDE],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPR01COLOURS],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPR23COLOURS]
            );
            zprintf
            (   TEXTPEN_VIEW,
                "SPRITE0AX,Y: %02X,%02X SPRITE1AX,Y: %02X,%02X SPRITE2AX,Y: %02X,%02X SPRITE3AX,Y: %02X,%02X\n",
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITE0AX],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITE0AY],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITE1AX],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITE1AY],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITE2AX],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITE2AY],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITE3AX],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITE3AY]
            );
            zprintf
            (   TEXTPEN_VIEW,
                "SPRITE0BX,Y: %02X,%02X SPRITE1BX,Y: %02X,%02X SPRITE2BX,Y: %02X,%02X SPRITE3BX,Y: %02X,%02X\n",
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITE0BX],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITE0BY],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITE1BX],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITE1BY],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITE2BX],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITE2BY],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITE3BX],
                memory[pvibase + (0x100 * whichpvi) + PVI_SPRITE3BY]
            );

            if (verbosity)
            {   zprintf
                (   TEXTPEN_VIEW,
                    "%-27s %4.3f %s (note %s)\n",
                    LLL(MSG_PITCH, "Pitch:"),
                    (float) (dividend / (memory[pvibase + (0x100 * whichpvi) + PVI_PITCH] + 1)),
                    LLL(MSG_HERTZ, "Hz"),
                    (region == REGION_NTSC) ? (notes[idealfreq_ntsc[memory[pvibase + (0x100 * whichpvi) + PVI_PITCH]].whichnote].name) : (notes[idealfreq_pal[memory[pvibase + (0x100 * whichpvi) + PVI_PITCH]].whichnote].name)
                );

                if (memory[pvibase + (0x100 * whichpvi) + PVI_SCORECTRL] & 0x02)
                {   zprintf
                    (   TEXTPEN_VIEW,
                        "%-27s 1 %s 4 %s\n",
                        LLL(MSG_SCOREFORMAT, "Score format:"),
                        LLL(MSG_GROUPSOF, "group(s) of"),
                        LLL(MSG_DIGITS, "digits")
                    );
                } else
                {   zprintf
                    (   TEXTPEN_VIEW,
                        "%-27s 2 %s 2 %s\n",
                        LLL(MSG_SCOREFORMAT, "Score format:"),
                        LLL(MSG_GROUPSOF, "group(s) of"),
                        LLL(MSG_DIGITS, "digits")
                    );
                }

                if (memory[pvibase + (0x100 * whichpvi) + PVI_SCORECTRL] & 0x01)
                {   zprintf
                    (   TEXTPEN_VIEW,
                        "%-27s %s\n",
                        LLL(MSG_SCOREPOS, "Score position:"),
                        LLL(MSG_BOTTOMPOS, "bottom of screen")
                    );
                } else
                {   zprintf
                    (   TEXTPEN_VIEW,
                        "%-27s %s\n",
                        LLL(MSG_SCOREPOS, "Score position:"),
                        LLL(MSG_TOPPOS, "top of screen")
                    );
                }

                zprintf
                (   TEXTPEN_VIEW,
                    "%-27s %s\n",
                    LLL(MSG_GRIDBG, "Grid/background:"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_BGCOLOUR] & 0x08) ? LLL(MSG_ENABLED, "enabled") : LLL(MSG_DISABLED, "disabled")
                );
                // grid and background colours are probably wrong for coin-ops
                // (unimportant as they don't make use of this anyway)
                zprintf
                (   TEXTPEN_VIEW,
                    "%s\n %-26s %s\n",
                    LLL(MSG_COLOURS, "Colours:"),
                    LLL(MSG_BGCOLOUR, "Background:"),
                    colourname[memory[pvibase + (0x100 * whichpvi) + PVI_BGCOLOUR] & 0x07]
                );
                zprintf
                (   TEXTPEN_VIEW,
                    " %-26s %s\n",
                    LLL(MSG_GRIDCOLOUR, "Grid:"),
                    colourname[(memory[pvibase + (0x100 * whichpvi) + PVI_BGCOLOUR] & 0x70) >> 4]
                );
                zprintf
                (   TEXTPEN_VIEW,
                    " %-25s: %s, %s, %s, %s\n",
                    LLL(MSG_SPRITES_MENU, "Sprites"),
                    colourname[calcspritecolour((whichpvi * 4)    )],
                    colourname[calcspritecolour((whichpvi * 4) + 1)],
                    colourname[calcspritecolour((whichpvi * 4) + 2)],
                    colourname[calcspritecolour((whichpvi * 4) + 3)]
                );

                zprintf
                (   TEXTPEN_VIEW,
                    "%-27s %s, %s, %s, %s\n",
                    LLL(MSG_SPRITESIZES, "Sprite sizes:"),
                    spritesizes[ memory[pvibase + (0x100 * whichpvi) + PVI_SIZES] & 0x03      ],
                    spritesizes[(memory[pvibase + (0x100 * whichpvi) + PVI_SIZES] & 0x0C) >> 2],
                    spritesizes[(memory[pvibase + (0x100 * whichpvi) + PVI_SIZES] & 0x30) >> 4],
                    spritesizes[(memory[pvibase + (0x100 * whichpvi) + PVI_SIZES] & 0xC0) >> 6]
                );

                zprintf
                (   TEXTPEN_VIEW,
                    "%-27s %s, %s, %s, %s\n",
                    LLL(MSG_SPRITESDRAWN, "Sprites drawn?"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_BGCOLLIDE] & 0x08) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_BGCOLLIDE] & 0x04) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_BGCOLLIDE] & 0x02) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_BGCOLLIDE] & 0x01) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no")
                );

                zprintf
                (   TEXTPEN_VIEW,
                    "%s\n %-26s %s, %s, %s, %s\n",
                    LLL(MSG_COLLISIONS, "Collisions:"),
                    LLL(MSG_SPRVSBG, "Sprites vs. background:"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_BGCOLLIDE] & 0x80) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_BGCOLLIDE] & 0x40) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_BGCOLLIDE] & 0x20) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_BGCOLLIDE] & 0x10) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no")
                );
                sprintf
                (   (char*) gtempstring,
                    "%s #0 %s #1/#2/#3:",
                    LLL(MSG_SPRITE2, "Sprite"),
                    LLL(MSG_VERSUS, "vs.")
                );
                zprintf
                (   TEXTPEN_VIEW,
                    " %-26s %s, %s, %s\n",
                    gtempstring,
                    (memory[pvibase + (0x100 * whichpvi) + PVI_SPRITECOLLIDE] & 0x20) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_SPRITECOLLIDE] & 0x10) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_SPRITECOLLIDE] & 0x08) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no")
                );
                sprintf
                (   (char*) gtempstring,
                    "%s #1 %s #0/#2/#3:",
                    LLL(MSG_SPRITE2, "Sprite"),
                    LLL(MSG_VERSUS, "vs.")
                );
                zprintf
                (   TEXTPEN_VIEW,
                    " %-26s %s, %s, %s\n",
                    gtempstring,
                    (memory[pvibase + (0x100 * whichpvi) + PVI_SPRITECOLLIDE] & 0x20) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_SPRITECOLLIDE] & 0x04) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_SPRITECOLLIDE] & 0x02) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no")
                );
                sprintf
                (   (char*) gtempstring,
                    "%s #2 %s #0/#1/#3:",
                    LLL(MSG_SPRITE2, "Sprite"),
                    LLL(MSG_VERSUS, "vs.")
                );
                zprintf
                (   TEXTPEN_VIEW,
                    " %-26s %s, %s, %s\n",
                    gtempstring,
                    (memory[pvibase + (0x100 * whichpvi) + PVI_SPRITECOLLIDE] & 0x10) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_SPRITECOLLIDE] & 0x04) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_SPRITECOLLIDE] & 0x01) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no")
                );
                sprintf
                (   (char*) gtempstring,
                    "%s #3 %s #0/#1/#2:",
                    LLL(MSG_SPRITE2, "Sprite"),
                    LLL(MSG_VERSUS, "vs.")
                );
                zprintf
                (   TEXTPEN_VIEW,
                    " %-26s %s, %s, %s\n",
                    gtempstring,
                    (memory[pvibase + (0x100 * whichpvi) + PVI_SPRITECOLLIDE] & 0x08) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_SPRITECOLLIDE] & 0x02) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no"),
                    (memory[pvibase + (0x100 * whichpvi) + PVI_SPRITECOLLIDE] & 0x01) ? LLL(MSG_ENGINE_YES2, "yes") : LLL(MSG_ENGINE_NO2, "no")
                );
    }   }   }

    zprintf(TEXTPEN_VIEW, "\n");
}

EXPORT void view_udgs(void)
{   if (machine == ARCADIA)
    {   arcadia_view_udgs();
    } elif (machines[machine].pvis)
    {   pvi_view_udgs();
    } elif (machine == BINBUG)
    {   binbug_view_udgs();
}   }

EXPORT void view_bios_ram(void)
{   int  i, y;

    switch (machine)
    {
    case ELEKTOR:
        if (memory[0x8B4] & PSU_S  ) psubits[0] = pswbit[style][0]; else psubits[0] = pswbit[style][0] + 32;
        if (memory[0x8B4] & PSU_F  ) psubits[1] = pswbit[style][1]; else psubits[1] = pswbit[style][1] + 32;
        if (memory[0x8B4] & PSU_II ) psubits[2] = pswbit[style][2]; else psubits[2] = pswbit[style][2] + 32;
        // no need to bother about UF1 & UF2 bits
        psubits[3] = EOS;
        if (memory[0x8B3] & PSL_IDC) pslbits[0] = pswbit[style][3]; else pslbits[0] = pswbit[style][3] + 32;
        if (memory[0x8B3] & PSL_RS ) pslbits[1] = pswbit[style][4]; else pslbits[1] = pswbit[style][4] + 32;
        if (memory[0x8B3] & PSL_WC ) pslbits[2] = pswbit[style][5]; else pslbits[2] = pswbit[style][5] + 32;
        if (memory[0x8B3] & PSL_OVF) pslbits[3] = pswbit[style][6]; else pslbits[3] = pswbit[style][6] + 32;
        if (memory[0x8B3] & PSL_COM) pslbits[4] = pswbit[style][7]; else pslbits[4] = pswbit[style][7] + 32;
        if (memory[0x8B3] & PSL_C  ) pslbits[5] = pswbit[style][8]; else pslbits[5] = pswbit[style][8] + 32;
        pslbits[6] = EOS;

        for (y = 0; y < 6; y++) // 6 character rows
        {   if (y == 0)
            {   zprintf(TEXTPEN_VIEW, "MONOB:  \"");
            } else
            {   zprintf(TEXTPEN_VIEW, "        \"");
            }

            monob(y);
            zprintf(TEXTPEN_VIEW, "%s", letters);

            if (y == 0)
            {   zprintf
                (   TEXTPEN_VIEW,
                    "\" $8A0..$8AB: $%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
                    memory[0x8A0],
                    memory[0x8A1],
                    memory[0x8A2],
                    memory[0x8A3],
                    memory[0x8A4],
                    memory[0x8A5],
                    memory[0x8A6],
                    memory[0x8A7],
                    memory[0x8A8],
                    memory[0x8A9],
                    memory[0x8AA],
                    memory[0x8AB]
                );
            } elif (y == 1)
            {   zprintf
                (   TEXTPEN_VIEW,
                    "\" $8B9..$8BD:                            $%02X %02X %02X %02X %02X\n",
                    memory[0x8B9],
                    memory[0x8BA],
                    memory[0x8BB],
                    memory[0x8BC],
                    memory[0x8BD]
                ); // this area would be better shown as code rather than data
            } elif (y == 3)
            {   zprintf
                (   TEXTPEN_VIEW,
                    "\" LKBST:         $%02X RKBST:         $%02X MKBST:         $%02X\n",
                    memory[E_LKBST],
                    memory[E_RKBST],
                    memory[E_MKBST]
                );
            } elif (y == 4)
            {   zprintf
                (   TEXTPEN_VIEW,
                    "\" BK1:         $%04X BK2:         $%04X PC:          $%04X\n",
                    (256 * memory[E_BK1]) + memory[E_BK1 + 1],
                    (256 * memory[E_BK2]) + memory[E_BK2 + 1],
                    (256 * memory[E_ZPC]) + memory[E_ZPC + 1]
                );
            } elif (y == 5)
            {   zprintf
                (   TEXTPEN_VIEW,
                    "\" R0:            $%02X PSU:           %s PSL:        %s\n",
                    memory[0x8AC],
                    psubits,
                    pslbits
                );
            } else
            {   zprintf(TEXTPEN_VIEW, "\"\n");
        }   }

        zprintf(TEXTPEN_VIEW, "MLINE:  \"");
        for (i = 0; i < 8; i++)
        {   zprintf(TEXTPEN_VIEW, "%c", elektor_chars1[memory[0x890 + i]]);
        }
        zprintf
        (   TEXTPEN_VIEW,
            "\" R1:            $%02X R2:            $%02X R3:            $%02X\n",
            memory[0x8AD],
            memory[0x8AE],
            memory[0x8AF]
        );
        zprintf
        (   TEXTPEN_VIEW,
            "                   R4:            $%02X R5:            $%02X R6:            $%02X\n",
            memory[0x8B0],
            memory[0x8B1],
            memory[0x8B2]
        );
        zprintf
        (   TEXTPEN_VIEW,
            "FSEQ:          $%02X ENTM:          $%02X MFUNC:         $%02X MSCR:        $%04X\n\n",
            memory[E_FSEQ], // FSEQ is aka SILENCE
            memory[E_ENTM],
            memory[E_MFUNC],
            (256 * memory[E_MSCR]) + memory[E_MSCR + 1]
        );
    acase PIPBUG:
        zprintf
        (   TEXTPEN_VIEW,
            "COM  ($400..$408):  $%02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
            memory[0x400],
            memory[0x401],
            memory[0x402],
            memory[0x403],
            memory[0x404],
            memory[0x405],
            memory[0x406],
            memory[0x407],
            memory[0x408]
        );
        zprintf
        (   TEXTPEN_VIEW,
            "XGOT ($409..$40C):  $%02X %02X %02X %02X\n", // better to show this as code
            memory[0x409],
            memory[0x40A],
            memory[0x40B],
            memory[0x40C]
        );
        zprintf
        (   TEXTPEN_VIEW,
            "BUFF ($413..$426):  $%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
            memory[0x413],
            memory[0x414],
            memory[0x415],
            memory[0x416],
            memory[0x417],
            memory[0x418],
            memory[0x419],
            memory[0x41A],
            memory[0x41B],
            memory[0x41C]
        );
        zprintf
        (   TEXTPEN_VIEW,
            "                    $%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
            memory[0x41D],
            memory[0x41E],
            memory[0x41F],
            memory[0x420],
            memory[0x421],
            memory[0x422],
            memory[0x423],
            memory[0x424],
            memory[0x425],
            memory[0x426]
        );
        zprintf
        (   TEXTPEN_VIEW,
            "TEMP: $%04X TEMQ: $%04X TEMR:   $%02X TEMS:   $%02X\n",
            (memory[0x40D] * 256) + memory[0x40E],
            (memory[0x40F] * 256) + memory[0x410],
            memory[T_TEMR],
            memory[T_TEMS]
        );
        zprintf
        (   TEXTPEN_VIEW,
            "MARK: $%04X HDAT: $%04X LDAT: $%04X HADR: $%04X LADR: $%04X\n",
            (memory[T_MARK] * 256) + memory[T_MARK + 1],
            (memory[T_HDAT] * 256) + memory[T_HDAT + 1],
            (memory[T_LDAT] * 256) + memory[T_LDAT + 1],
            (memory[T_HADR] * 256) + memory[T_HADR + 1],
            (memory[T_LADR] * 256) + memory[T_LADR + 1]
        );
        zprintf
        (   TEXTPEN_VIEW,
            "BPTR:   $%02X MCNT:   $%02X CNT:    $%02X CODE:   $%02X OKGO:   $%02X BCC:   $%02X\n\n",
            memory[T_BPTR],
            memory[T_MCNT],
            memory[T_CNT ],
            memory[T_CODE],
            memory[T_OKGO],
            memory[T_BCC ]
        );
    acase INSTRUCTOR:
        for (i = 0x17C0; i < 0x1800; i += 16)
        {   dumprow(i);
        }
        zprintf(TEXTPEN_VIEW, "\n");
    acase TWIN:
        if (twin_dosver != TWIN_NODOS)
        {   twin_view_dos();
        } else
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!")); // wrong DOS really
        }
    acase CD2650:
        zprintf
        (   TEXTPEN_VIEW,
            "ADD:  $%04X BKP:  $%04X BPD:  $%04X CUR:  $%04X\n",
            (256 * memory[C_ADD1]) + memory[C_ADD2],
            (256 * memory[C_BKP1]) + memory[C_BKP2],
            (256 * memory[C_BPD1]) + memory[C_BPD2],
            (256 * memory[C_CUR1]) + memory[C_CUR2]
        );
        zprintf
        (   TEXTPEN_VIEW,
            "LENT:   $%02X SUMC:   $%02X\n\n",
            memory[C_LENT],
            memory[C_SUMC]
        );
    acase PHUNSY:
        zprintf(TEXTPEN_VIEW, "MDCR RAM ($C00..$E40): \n");
        for (i = 0xC00; i <= 0xE40; i += 16) // it would be better to omit $E41..$E4F but dumprow() doesn't allow this
        {   dumprow(i);
        }
        zprintf
        (   TEXTPEN_VIEW,
            "\n%s (%s) ($EE0..$F00):\n",
            LLL(MSG_VIEW_BRAM,  "BIOS RAM"  ),
            LLL(MSG_SCRATCHPAD, "scratchpad")
        );
        zprintf
        (   TEXTPEN_VIEW,
            "ZB1A:   $%04X ZB2A:   $%04X CMAD:   $%04X ERET:   $%04X\n",
            (256 * memory[0xEE0]) + memory[0xEE1], // ZB1A
            (256 * memory[0xEE2]) + memory[0xEE3], // ZB2A
            (256 * memory[0xEE4]) + memory[0xEE5], // CMAD
            (256 * memory[0xEE6]) + memory[0xEE7]  // ERET
        );
        zprintf
        (   TEXTPEN_VIEW,
            "IOAD:   $%04X IIAD:   $%04X ICAD:   $%04X\n",
            (256 * memory[0xEE8]) + memory[0xEE9], // IOAD
            (256 * memory[0xEEA]) + memory[0xEEB], // IIAD
            (256 * memory[0xEEC]) + memory[0xEED]  // ICAD
        );
        zprintf
        (   TEXTPEN_VIEW,
            "CURS:   $%04X TBSR:   $%04X\n",
            (256 * memory[0xEF4]) + memory[0xEF5], // CURS (cursor position)
            (256 * memory[0xEFE]) + memory[0xEFF]  // TBSR (two bytes register)
        );
        zprintf
        (   TEXTPEN_VIEW,
            "STAD:   $%04X ENAD:   $%04X TOAD:   $%04X\n",
            (256 * memory[0xEF8]) + memory[0xEF9], // STAD (start address)
            (256 * memory[0xEFA]) + memory[0xEFB], // ENAD (end address)
            (256 * memory[0xEFC]) + memory[0xEFD]  // TOAD (target address)
        );
        zprintf
        (   TEXTPEN_VIEW,
            "BRTRES:   $%02X CURRES:   $%02X COMRES:   $%02X CPORES:   $%02X\n",
            memory[0xEEE], // BRTRES (baud rate)
            memory[0xEEF], // CURRES (cursor position)
            memory[0xEF6], // COMRES (command compare)
            memory[0xEF7]  // CPORES (control port out storage)
        );
        zprintf
        (   TEXTPEN_VIEW,
            "LESTRE:   $%02X RIENRE:   $%02X UPSTRE:   $%02X LOENRE:   $%02X\n",
            memory[0xEF1], // LESTRE (screen border left)
            memory[0xEF3], // RIENRE (screen border right)
            memory[0xEF0], // UPSTRE (screen border upper)
            memory[0xEF2]  // LESTRE (screen border lower)
        );
        zprintf
        (   TEXTPEN_VIEW,
            "INPRAM:   $%02X\n",
            memory[0xF00]  // INPRAM (index)
        );
        zprintf
        (   TEXTPEN_VIEW,
            "\n%s (%s) ($F01..$FFF):\n",
            LLL(MSG_VIEW_BRAM, "BIOS RAM"            ),
            LLL(MSG_BUFFER,    "command input buffer")
        );
        for (i = 0xF01; i < 0x1000; i += 16) // $F01..$1000 (it would be better to omit $1000 but dumprow() doesn't allow this
        {   dumprow(i);
        }
        zprintf(TEXTPEN_VIEW, "\n");
    acase SELBST:
        for (i = 0x800; i <= 0x8FF; i += 16)
        {   dumprow(i);
        }

        if (selbst_biosver == SELBST_BIOS09)
        {   if (memory[0x80C] & PSU_S  ) psubits[0] = pswbit[style][0]; else psubits[0] = pswbit[style][0] + 32;
            if (memory[0x80C] & PSU_F  ) psubits[1] = pswbit[style][1]; else psubits[1] = pswbit[style][1] + 32;
            if (memory[0x80C] & PSU_II ) psubits[2] = pswbit[style][2]; else psubits[2] = pswbit[style][2] + 32;
            psubits[3] = EOS;
            if (memory[0x80B] & PSL_IDC) pslbits[0] = pswbit[style][3]; else pslbits[0] = pswbit[style][3] + 32;
            if (memory[0x80B] & PSL_RS ) pslbits[1] = pswbit[style][4]; else pslbits[1] = pswbit[style][4] + 32;
            if (memory[0x80B] & PSL_WC ) pslbits[2] = pswbit[style][5]; else pslbits[2] = pswbit[style][5] + 32;
            if (memory[0x80B] & PSL_OVF) pslbits[3] = pswbit[style][6]; else pslbits[3] = pswbit[style][6] + 32;
            if (memory[0x80B] & PSL_COM) pslbits[4] = pswbit[style][7]; else pslbits[4] = pswbit[style][7] + 32;
            if (memory[0x80B] & PSL_C  ) pslbits[5] = pswbit[style][8]; else pslbits[5] = pswbit[style][8] + 32;
            pslbits[6] = EOS;

            zprintf
            (   TEXTPEN_VIEW,
                "\nSVR0:          $%02X SVR1:          $%02X PSUS:          %s PSLS:       %s\n",
                memory[0x804],
                memory[0x801],
                psubits,
                pslbits
            );
            zprintf
            (   TEXTPEN_VIEW,
                "                   SR10:          $%02X SR20:          $%02X SR30:          $%02X\n",
                memory[0x805],
                memory[0x806],
                memory[0x807]
            );
            zprintf
            (   TEXTPEN_VIEW,
                "                   SR11:          $%02X SR21:          $%02X SR31:          $%02X\n",
                memory[0x808],
                memory[0x809],
                memory[0x80A]
            );
            zprintf
            (   TEXTPEN_VIEW,
                "SR0:           $%02X SR1:           $%02X SR2:           $%02X SR3:           $%02X\n",
                memory[0x80D],
                memory[0x80E],
                memory[0x80F],
                memory[0x810]
            );
            zprintf
            (   TEXTPEN_VIEW,
                "TC:          $%04X TR:          $%04X BKP:         $%04X BPD:         $%04X\n",
                (256 * memory[0x815]) + memory[0x816],
                (256 * memory[0x817]) + memory[0x818],
                (256 * memory[0x823]) + memory[0x824],
                (256 * memory[0x825]) + memory[0x826]
            );
            zprintf
            (   TEXTPEN_VIEW,
                "SVDT:          $%02X TDAS:          $%02X HPOS:          $%02X\n",
                memory[0x802],
                memory[0x803],
                memory[0x814]
            );
            zprintf
            (   TEXTPEN_VIEW,
                "ADD1:          $%02X ADD2:          $%02X LENT:          $%02X SUMC:          $%02X\n",
                memory[0x81F],
                memory[0x820],
                memory[0x821],
                memory[0x822]
            );
        }
    adefault: // eg. ARCADIA, INTERTON, BINBUG, coin-ops, PONG, MIKIT, TYPERIGHT
        zprintf(TEXTPEN_CLIOUTPUT, "%s\n\n", LLL(MSG_WRONGGUEST, "Wrong guest machine!"));
}   }   

EXPORT void clearcoverage(void)
{   int i;

    for (i = 0; i < 258; i++)
    {   coverage_io[i] = 0;
    }
    for (i = 0; i < 32 * KILOBYTE; i++)
    {   coverage[i]   = 0;
        asicreads[i]  =
        asicwrites[i] = 0;
}   }

EXPORT void view_cpu_2650(FLAG newline)
{   FAST TEXT ClockStr[13 + 1];

    if (machine == PONG || machine == TYPERIGHT)
    {   return;
    }

    if (psu & PSU_S  ) psubits[0] = pswbit[style][0]; else psubits[0] = pswbit[style][0] + 32;
    if (psu & PSU_F  ) psubits[1] = pswbit[style][1]; else psubits[1] = pswbit[style][1] + 32;
    if (psu & PSU_II ) psubits[2] = pswbit[style][2]; else psubits[2] = pswbit[style][2] + 32;
    if (psu & PSU_UF1) psubits[3] = '1';              else psubits[3] = '.';
    if (psu & PSU_UF2) psubits[4] = '2';              else psubits[4] = '.';
    psubits[5] = EOS;
    if (psl & PSL_IDC) pslbits[0] = pswbit[style][3]; else pslbits[0] = pswbit[style][3] + 32;
    if (psl & PSL_RS ) pslbits[1] = pswbit[style][4]; else pslbits[1] = pswbit[style][4] + 32;
    if (psl & PSL_WC ) pslbits[2] = pswbit[style][5]; else pslbits[2] = pswbit[style][5] + 32;
    if (psl & PSL_OVF) pslbits[3] = pswbit[style][6]; else pslbits[3] = pswbit[style][6] + 32;
    if (psl & PSL_COM) pslbits[4] = pswbit[style][7]; else pslbits[4] = pswbit[style][7] + 32;
    if (psl & PSL_C  ) pslbits[5] = pswbit[style][8]; else pslbits[5] = pswbit[style][8] + 32;
    pslbits[6] = EOS;

    ClockStr[13] = EOS;
    longcomma(scale_time(cycles_2650), (STRPTR) ClockStr);
    DISCARD stcl_d((char*) XStr, (long) cpux);
    DISCARD stcl_d((char*) YStr, (long) cpuy);

    switch (style)
    {
    case STYLE_CALM:
        zprintf
        (   TEXTPEN_VIEW,
            "%-11s %s  A: $%02X  B: $%02X  C: $%02X  D: $%02X   U: %s  SP: %d\n",
            timeunitstr1,
            ClockStr,
            r[0],
            r[1],
            r[2],
            r[3],
            psubits,
            (int) (psu & PSU_SP)
        );
        zprintf
        (   TEXTPEN_VIEW,
            "    X: %3s         Y: %3s         B': $%02X C': $%02X D': $%02X   L: %s CC: %s\n",
            (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR || machine == BINBUG || machine == CD2650 || machine == PHUNSY) ? ((char*) XStr) : "-",
            (machine == ARCADIA || machines[machine].pvis                    || machine == BINBUG || machine == CD2650 || machine == PHUNSY) ? ((char*) YStr) : "-",
            r[4],
            r[5],
            r[6],
            pslbits,
            ccstring[style][(psl & 0xC0) >> 6] // %11000000 -> %00000011
        );
        zprintf
        (   TEXTPEN_VIEW,
            "    RAS: $%04X $%04X $%04X $%04X $%04X $%04X $%04X $%04X   PC: $%04X\n",
            ras[0],
            ras[1],
            ras[2],
            ras[3],
            ras[4],
            ras[5],
            ras[6],
            ras[7],
            iar
        );
    acase STYLE_IEEE:
        zprintf
        (   TEXTPEN_VIEW,
            "%-11s %s .0: $%02X .1: $%02X .2: $%02X .3: $%02X  .U: %s  .SP: %d\n",
            timeunitstr1,
            ClockStr,
            r[0],
            r[1],
            r[2],
            r[3],
            psubits,
            (int) (psu & PSU_SP)
        );
        zprintf
        (   TEXTPEN_VIEW,
            "    X: %3s         Y: %3s         .4: $%02X .5: $%02X .6: $%02X  .L: %s .CC: %s\n",
            (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR || machine == BINBUG || machine == CD2650 || machine == PHUNSY) ? ((char*) XStr) : "-",
            (machine == ARCADIA || machines[machine].pvis                    || machine == BINBUG || machine == CD2650 || machine == PHUNSY) ? ((char*) YStr) : "-",
            r[4],
            r[5],
            r[6],
            pslbits,
            ccstring[style][(psl & 0xC0) >> 6] // %11000000 -> %00000011
        );
        zprintf
        (   TEXTPEN_VIEW,
            "   .RAS: $%04X $%04X $%04X $%04X $%04X $%04X $%04X $%04X  .PC: $%04X\n",
            ras[0],
            ras[1],
            ras[2],
            ras[3],
            ras[4],
            ras[5],
            ras[6],
            ras[7],
            iar
        );
    adefault:
        zprintf
        (   TEXTPEN_VIEW,
            "%-11s %s R0: $%02X R1: $%02X R2: $%02X R3: $%02X PSU: %s  SP: %d\n",
            timeunitstr1,
            ClockStr,
            r[0],
            r[1],
            r[2],
            r[3],
            psubits,
            (int) (psu & PSU_SP)
        );
        zprintf
        (   TEXTPEN_VIEW,
            "    X: %3s         Y: %3s         R4: $%02X R5: $%02X R6: $%02X PSL: %s CC: %s\n",
            (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR || machine == BINBUG || machine == CD2650 || machine == PHUNSY) ? ((char*) XStr) : "-",
            (machine == ARCADIA || machines[machine].pvis                    || machine == BINBUG || machine == CD2650 || machine == PHUNSY) ? ((char*) YStr) : "-",
            r[4],
            r[5],
            r[6],
            pslbits,
            ccstring[style][(psl & 0xC0) >> 6] // %11000000 -> %00000011
        );
        zprintf
        (   TEXTPEN_VIEW,
            "    RAS: $%04X $%04X $%04X $%04X $%04X $%04X $%04X $%04X  IAR: $%04X\n",
            ras[0],
            ras[1],
            ras[2],
            ras[3],
            ras[4],
            ras[5],
            ras[6],
            ras[7],
            iar
        );
    }

    if (newline)
    {   zprintf(TEXTPEN_VIEW, "\n");
}   }
