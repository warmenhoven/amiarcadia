// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
    #include <dos/dostags.h>      // for SystemTags()
    #ifdef __amigaos4__
        #include <dos/obsolete.h> // for CurrentDir()
    #endif
    #include <proto/locale.h>     // for GetCatalogStr()
#endif
#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
#endif

#include <ctype.h>                // for toupper()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"

// DEFINES----------------------------------------------------------------

#define ENDMARKER ";END INCLUDE "

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       FLAG                     assembling     = FALSE;
EXPORT       int                      errors,
                                      lastparse,
                                      pass,
                                      warnings;

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       UBYTE                    memory[32768],
                                      startaddr_h,
                                      startaddr_l;
IMPORT       TEXT                     asciiname_short[259][3 + 1],
                                      datetimestring[40 + 1],
                                      fn_asm[MAX_PATH + 1],
                                      gtempstring[256 + 1];
IMPORT       UWORD                    iar;
IMPORT       int                      base,
                                      firstdosequiv, lastdosequiv,
                                      game,
                                      machine,
                                      nextdis,
                                      phunsy_biosver,
                                      pvibase,
                                      selbst_biosver,
                                      style,
                                      supercpu,
                                      userlabels,
                                      verbosity,
                                      warn,
                                      whichgame;
IMPORT       MEMFLAG                  memflags[ALLTOKENS];
IMPORT const STRPTR                   hexchars[STYLES],
                                      decchars[STYLES],
                                      octchars[STYLES],
                                      binchars[STYLES];
IMPORT       struct EquivalentStruct  equivalents[],
                                      symlabel[SYMLABELS_MAX + 1];
IMPORT const struct KindStruct        filekind[KINDS];
IMPORT const struct KnownStruct       known[KNOWNGAMES];
IMPORT       struct MachineStruct     machines[MACHINES];
IMPORT       struct OpcodeStruct      opcodes[3][256];

#ifdef AMIGA
    IMPORT   struct Catalog*          CatalogPtr;
#endif
#ifdef WIN32
    IMPORT   FLAG                     hurry;
    IMPORT   int                      CatalogPtr; // APTR doesn't work
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE       FLAG                     commenting,
                                      ended,
                                      equmode        = FALSE,
                                      including      = FALSE,
                                      orgy,
                                      specified;
MODULE       UBYTE                    emitbuf[3],
                                      gopcode,
                                      gbyte1st,
                                      gbyte2nd;
MODULE       TEXT                     equstring[LABELLIMIT + 1],
                                      entirestring[256 + 1],
                                      fn_inc[MAX_PATH + 1],
                                      GraphBuffer[16 * KILOBYTE], // arbitrary size
                                      LineBuffer[256 + 1];
MODULE       STRPTR                   AsmBuffer      = NULL,
                                      evalstring;
MODULE       int                      asmsize2,
                                      asmstart,
                                      defasmbase,
                                      emitpos,
                                      firstaddr,
                                      graphnesting,
                                      icursor,
                                      lastaddr,
                                      line,
                                      linecursor,
                                      ocursor;
MODULE       FILE*                    ListFileHandle = NULL;

// MODULE ARRAYS/STRUCTURES-----------------------------------------------

MODULE const STRPTR                   apcchars[STYLES] = { "$", "$", "APC", "APC", "*" },
                                      indchars[STYLES] = { "*", "*", "@"  , "{"  , "@" };

// MODULE FUNCTIONS-------------------------------------------------------

MODULE FLAG asm_oldcalm(void);
MODULE FLAG asm_newcalm(void);
MODULE FLAG asm_calmdirective(void);
MODULE FLAG asm_ieee(void);
MODULE FLAG asmline(void);
MODULE FLAG asm_signetics(void);
MODULE void asmwarning(STRPTR errorstring);
MODULE FLAG compareit(STRPTR target, FLAG needspace);
MODULE UBYTE dorel(int wherefrom, FLAG indirect);
MODULE void emit(int data);
MODULE int findlabel(STRPTR passedstring, int length, FLAG allowall);
MODULE void getaddress(UBYTE offset);
MODULE UBYTE getcc_signetics(FLAG unallowed);
MODULE UBYTE getcc_calm(void);
MODULE void getdbx(void);
MODULE UBYTE getreg_signetics(int r0allowed, FLAG r1r2allowed);
MODULE UBYTE getreg_calm(int r0allowed, FLAG r1r2allowed);
MODULE UBYTE getreg_ieee(void);
MODULE void getimmediate(FLAG skipthem);
MODULE void getimmediatebytes(void);
MODULE void getimmediatewords(FLAG swap);
MODULE void getabsolute_nonbranch_signetics(UBYTE firstopcode, int reg);
MODULE void getabsolute_nonbranch_oldcalm(UBYTE firstopcode);
MODULE FLAG ganb_newcalm(FLAG allpages);
MODULE void getabsolute_nonbranch_ieee(UBYTE firstopcode, int thereg);
MODULE void getabsolute_branch(void);
MODULE void getabsolute_indexed_signetics(UBYTE firstopcode);
MODULE void getrelative(FLAG indirect);
MODULE void getzeropage(FLAG indirect);
MODULE int parseonenumber(STRPTR passedstring, int length);
MODULE UBYTE saveimmediate(FLAG skipthem);
MODULE FLAG skipblanks(FLAG mandatory);
MODULE FLAG includebin(void);
MODULE FLAG includefile(void);
MODULE FLAG ishexdigit(TEXT which);
MODULE int fiximmediate(int value);
MODULE int expr(void);
MODULE int factor(void);
MODULE FLAG isterminator(TEXT thechar);

// CODE-------------------------------------------------------------------

EXPORT void assemble(void)
{   TRANSIENT FLAG  done;
    TRANSIENT FILE *BinHandle      = NULL,
                   *TheLocalHandle = NULL;
    TRANSIENT int   asmsize1,
                    i,
                    length,
                    thekind;
    TRANSIENT UBYTE TVCBuffer[5];
    PERSIST   TEXT  fn_lstbin[MAX_PATH + 1]; // PERSISTent so as not to blow the stack

    // assert(!AsmBuffer);
    // assert(fn_asm[0]);
    // assert(!ListFileHandle);

    cd_projects();

    if (!Exists(fn_asm))
    {   fixextension(filekind[KIND_ASM].extension, (STRPTR) fn_asm, FALSE, "");
        if (!Exists(fn_asm))
        {   fixextension(".2650", (STRPTR) fn_asm, TRUE, "");
            if (!Exists(fn_asm))
            {   fixextension(filekind[KIND_ASM].extension, (STRPTR) fn_asm, TRUE, "");
    }   }   }

    asmsize1 = (int) getsize((STRPTR) fn_asm);
    if (!(AsmBuffer = malloc(asmsize1)))
    {   zprintf(TEXTPEN_CLIOUTPUT, "Out of memory!\n\n");
        goto ASMDONE;
    }
    if (!(TheLocalHandle = fopen((const char*) fn_asm, "rb")))
    {   zprintf(TEXTPEN_CLIOUTPUT, "Can't open %s for reading!\n\n", fn_asm);
        goto ASMDONE;
    }
    if (fread(AsmBuffer, (size_t) asmsize1, 1, TheLocalHandle) != 1)
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FAILED, "Failed!\n\n")); // can't read (eg. zero size)
        goto ASMDONE;
    }
    strcpy((char*) fn_lstbin, (const char*) fn_asm);
    fixextension(".lst", (STRPTR) fn_lstbin, TRUE, "");
    if (!(ListFileHandle = fopen((const char*) fn_lstbin, "wt")))
    {   zprintf(TEXTPEN_CLIOUTPUT, "Can't open %s for output!\n", fn_lstbin);
        goto ASMDONE;
    }

    switch (machine)
    {
    case  ELEKTOR: thekind = KIND_TVC;
    acase PIPBUG:
    case  BINBUG:
    case  TWIN:    // arguable for TWIN whether it should be BIN or AOF
    case  CD2650:
    case  SELBST:
    case  MIKIT:   thekind = KIND_AOF;
    adefault:      // eg. ARCADIA, INTERTON, INSTRUCTOR, PHUNSY, coin-ops
                   thekind = KIND_BIN;
    }
    fixextension(filekind[thekind].extension, (STRPTR) fn_lstbin, TRUE, "");
    if (thekind == KIND_BIN || thekind == KIND_TVC)
    {   if (!(BinHandle = fopen((const char*) fn_lstbin, "wb")))
        {   zprintf(TEXTPEN_CLIOUTPUT, "Can't open %s for output!\n", fn_lstbin);
            goto ASMDONE;
    }   }

#ifdef WIN32
    hurry = TRUE;
    cls();
#endif

    switch (style)
    {
    case  STYLE_SIGNETICS1: zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_ASSEMBLING, "Assembling %s as %s...\n"), fn_asm, "non-extended Signetics");
    acase STYLE_SIGNETICS2: zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_ASSEMBLING, "Assembling %s as %s...\n"), fn_asm, "extended Signetics");
    acase STYLE_OLDCALM:    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_ASSEMBLING, "Assembling %s as %s...\n"), fn_asm, "old CALM");
    acase STYLE_NEWCALM:    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_ASSEMBLING, "Assembling %s as %s...\n"), fn_asm, "new CALM");
    acase STYLE_IEEE:       zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_ASSEMBLING, "Assembling %s as %s...\n"), fn_asm, "IEEE-694");
    }

    firstaddr = 32767; // this is
    lastaddr  =     0; // no mistake

    userlabels = 0;
    defasmbase = BASE_DECIMAL; // decimal
    for (i = 0; i < SYMLABELS_MAX; i++)
    {   symlabel[i].used = FALSE;
    }

    line = 1;
    pass = 1;
    commenting =
    specified  = FALSE;
    assembling = TRUE;
    warnings = errors = 0;
    graphnesting = 0;
    GraphBuffer[0] = EOS;
#ifdef VERBOSE
    zprintf(TEXTPEN_CLIOUTPUT, "Pass 1...\n");
#endif

    switch (machine)
    {
    case  ELEKTOR: case SELBST: startaddr_h = 0x09; startaddr_l = 0x00; // $8C0 is also popular for Elektor
    acase PIPBUG:  case BINBUG: startaddr_h = 0x04; startaddr_l = 0x40; // $437 and $500 are also popular
    acase CD2650:               startaddr_h = 0x15; startaddr_l = 0x10; // various others are also popular
    acase PHUNSY:               startaddr_h = 0x08; startaddr_l = 0x00; // various others are also popular
    acase MIKIT:                startaddr_h = 0x05; startaddr_l = 0x00;
    adefault:                   startaddr_h =    0; startaddr_l =    0; // eg. ARCADIA, INTERTON, INSTRUCTOR, coin-ops
    }

    datestamp2();
    length = sprintf((char*) entirestring, "Assembled by %s %s at %s", EMUNAME, DECIMALVERSION, datetimestring);
    fwrite(entirestring, length, 1, ListFileHandle);
    length = 74 - length;
    if (length >= 1)
    {   for (i = 0; i < length; i++)
        {   entirestring[i] = '-';
        }
        entirestring[length] = EOS;
        fwrite(entirestring, length, 1, ListFileHandle);
    }
    fwrite("\n\n", 2, 1, ListFileHandle);

    if (AsmBuffer[0] == 239 && AsmBuffer[1] == 187 && AsmBuffer[2] == 191) // because sometimes it is prepended with "﻿"
    {   icursor = 3;
    } else
    {   icursor = 0;
    }
    asmsize2 = 0;
    while (icursor < asmsize1)
    {   if (AsmBuffer[icursor] == CR)
        {   if (icursor + 1 < asmsize1 && AsmBuffer[icursor + 1] == LF)
            {   icursor++;
            }
            AsmBuffer[asmsize2] = LF;
        } else   
        {   AsmBuffer[asmsize2] = AsmBuffer[icursor];
        }
        asmsize2++;
        icursor++;
    }
    icursor = ocursor = 0;

    while (icursor < asmsize2)
    {   linecursor = 0;

        // remove leading whitespace
        while (AsmBuffer[icursor] == ' ' || AsmBuffer[icursor] == HT)
        {   icursor++;
        }

        if (AsmBuffer[icursor] == '*')
        {   while
            (   icursor < asmsize2
             && AsmBuffer[icursor] != LF
            )
            {   icursor++;
            }
            icursor++;
            LineBuffer[0] = EOS;
        } else
        {   done = FALSE;
            do
            {   if (AsmBuffer[icursor] == ';')
                {   while (AsmBuffer[++icursor] != LF)
                    {   ;
                    }
                    LineBuffer[linecursor] = EOS;
                    if (!asmline()) goto DONE1;
                    done = TRUE;
                } elif (AsmBuffer[icursor] == LF)
                {   LineBuffer[linecursor] = EOS;
                    if (!asmline()) goto DONE1;
                    done = TRUE;
                } else
                {   LineBuffer[linecursor++] = AsmBuffer[icursor];
                }
                icursor++;
            } while (!done && icursor < asmsize2);
            if (!done && linecursor)
            {   LineBuffer[linecursor] = EOS;
                if (!asmline()) goto DONE1;
        }   }
        line++;
    }

DONE1:
    line = 1;
    icursor = ocursor = 0;
    orgy = ended = FALSE;
    pass = 2;
    commenting = FALSE;
#ifdef VERBOSE
    zprintf(TEXTPEN_CLIOUTPUT, "Pass 2...\n");
#endif

    while (icursor < asmsize2)
    {   linecursor = 0;
        asmstart   = icursor;

        // remove leading whitespace
        while (AsmBuffer[icursor] == ' ' || AsmBuffer[icursor] == HT)
        {   icursor++;
        }

        if (AsmBuffer[icursor] == '*')
        {   while
            (   icursor < asmsize2
             && AsmBuffer[icursor] != LF
            )
            {   icursor++;
            }
            fwrite("              :"   ,                 15, 1, ListFileHandle);
            fwrite(&AsmBuffer[asmstart], icursor - asmstart, 1, ListFileHandle);
            fwrite("\n"                ,                  1, 1, ListFileHandle);
            icursor++;
            LineBuffer[0] = EOS;
        } else
        {   done = FALSE;
            do
            {   if (AsmBuffer[icursor] == ';')
                {   if ((int) (icursor + strlen(ENDMARKER)) < asmsize2 && !strncmp(&AsmBuffer[icursor], ENDMARKER, strlen(ENDMARKER)))
                    {   if (graphnesting == 0)
                        {   asmerror("unexpected end of include");
                        } else
                        {   graphnesting--;
                    }   }
                    while (AsmBuffer[++icursor] != LF)
                    {   ;
                    }
                    LineBuffer[linecursor] = EOS;
                    if (!asmline()) goto DONE2;
                    done = TRUE;
                } elif (AsmBuffer[icursor] == LF)
                {   LineBuffer[linecursor] = EOS;
                    if (!asmline()) goto DONE2;
                    done = TRUE;
                } else
                {   LineBuffer[linecursor++] = AsmBuffer[icursor];
                }
                icursor++;
            } while (!done && icursor < asmsize2);
            if (!done && linecursor)
            {   LineBuffer[linecursor] = EOS;
                if (!asmline()) goto DONE2;
        }   }
        line++;
    }

    if (!ended)
    {   switch (style)
        {
        acase STYLE_OLDCALM:
        case  STYLE_NEWCALM: asmwarning("missing .END directive");
        acase STYLE_IEEE:    asmwarning("missing END directive");
        adefault:            asmwarning("missing END/.END directive");
    }   }

DONE2:
    fwrite("\nSymbol Table--------------------------------------------------------------\n\n", 77, 1, ListFileHandle);
    savesym(ListFileHandle);

    if (GraphBuffer[0] != EOS)
    {   fwrite("\nInclude Tree Graph--------------------------------------------------------\n\n", 77, 1, ListFileHandle);
        fwrite(GraphBuffer, strlen(GraphBuffer), 1, ListFileHandle);
        fwrite("\n", 1, 1, ListFileHandle);
    }

    length = sprintf((char*) entirestring, "Assembled %d line(s) (%d warning(s), %d error(s)).\n", line - 1, warnings, errors);
    zprintf(TEXTPEN_CLIOUTPUT, (const char*) entirestring);
    fwrite(entirestring, length, 1, ListFileHandle);

    iar = (startaddr_h * 256) + startaddr_l;
    nextdis = iar;

    if (!specified)
    {   zprintf
        (   TEXTPEN_CLIOUTPUT,
            LLL(MSG_ASSUMINGADDRESS, "Assuming start address of $%02X%02X.\n"),
            startaddr_h,
            startaddr_l
        );
    }

    // assert(firstaddr <= lastaddr);

    switch (thekind)
    {
    case KIND_TVC:
        // assert(machine == ELEKTOR);
        TVCBuffer[0] = 0x02;            // magic byte
        TVCBuffer[1] = firstaddr / 256; // load address
        TVCBuffer[2] = firstaddr % 256; // load address
        TVCBuffer[3] = startaddr_h;
        TVCBuffer[4] = startaddr_l;
        DISCARD fwrite( TVCBuffer     ,                           5, 1, BinHandle);
        DISCARD fwrite(&memory[firstaddr], lastaddr - firstaddr + 1, 1, BinHandle);
    acase KIND_AOF:
        save_aof(fn_lstbin, firstaddr, lastaddr, (startaddr_h * 256) + startaddr_l, 30);
    acase KIND_BIN:
        if (machine == ARCADIA && ocursor > 8192) // ie. >4K (as ocursor will be >= $2000)
        {   DISCARD fwrite( memory        ,           4096, 1, BinHandle); // $0000..$0FFF -> $0000..$0FFF
            DISCARD fwrite(&memory[0x2000], ocursor - 4096, 1, BinHandle); // $2000..$2FFF -> $1000..$1FFF
        } else // eg. INTERTON, INSTRUCTOR, PHUNSY
        {   DISCARD fwrite( memory        , ocursor       , 1, BinHandle);
    }   }

    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));

ASMDONE:
    assembling = FALSE;
#ifdef WIN32
    hurry = FALSE;
    zprintf(TEXTPEN_DEFAULT, "");
#endif

    if (BinHandle)
    {   fclose(BinHandle);
        // BinHandle = NULL;
    }
    if (ListFileHandle)
    {   fclose(ListFileHandle);
        ListFileHandle = NULL;
    }
    if (TheLocalHandle)
    {   fclose(TheLocalHandle);
        // TheLocalHandle = NULL;
    }
    if (AsmBuffer)
    {   free(AsmBuffer);
        AsmBuffer = NULL;
    }

    cd_progdir();

    game = TRUE; // so that "reset to game" works
}

MODULE FLAG asmline(void)
{   FAST int  i,
              oldocursor;
    FAST FLAG rc;
    FAST TEXT tempstring[LABELLIMIT + 1];

    emitpos    = 0;
    oldocursor = ocursor;

    if (linecursor >= 1)
    {   // remove trailing whitespace
        for (i = linecursor - 1; i >= 0; i--)
        {   if (LineBuffer[i] == ' ' || LineBuffer[i] == HT)
            {   LineBuffer[i] = EOS;
            } else
            {   break;
    }   }   }

    linecursor = 0;

    while
    (   LineBuffer[linecursor] == ' '
     || LineBuffer[linecursor] == HT
    )
    {   linecursor++;
    }

#ifdef VERBOSE
    zprintf(TEXTPEN_CLIOUTPUT, "Assembling line %d on pass %d: \"%s\"\n", line, pass, LineBuffer);
#endif

    switch (style)
    {
    case  STYLE_OLDCALM: rc = asm_oldcalm();
    acase STYLE_NEWCALM: rc = asm_newcalm();
    acase STYLE_IEEE:    rc = asm_ieee();
    adefault:            rc = asm_signetics();
    }

    if (pass == 2)
    {   if (ocursor > oldocursor)
        {   sprintf((char*) tempstring, "%04X  ", oldocursor);
            fwrite(tempstring, 6, 1, ListFileHandle);
            switch (emitpos)
            {
            case  0:  sprintf((char*) tempstring, "        :");
            acase 1:  sprintf((char*) tempstring, "%02X      :"    , emitbuf[0]);
            acase 2:  sprintf((char*) tempstring, "%02X%02X    :"  , emitbuf[0], emitbuf[1]);
            adefault: sprintf((char*) tempstring, "%02X%02X%02X  :", emitbuf[0], emitbuf[1], emitbuf[2]);
            }
            fwrite(tempstring, 9, 1, ListFileHandle);
            // assert(icursor - asmstart > 0);
            fwrite(&AsmBuffer[asmstart], icursor - asmstart, 1, ListFileHandle);
        } else
        {   if (icursor - asmstart > 0)
            {   fwrite("              :", 15, 1, ListFileHandle);
                fwrite(&AsmBuffer[asmstart], icursor - asmstart, 1, ListFileHandle);
        }   }
        fwrite("\n", 1, 1, ListFileHandle);
    }

    return rc;
}

MODULE FLAG asm_signetics(void)
{   FAST int    i,
                length,
                value1,
                value2;
    FAST UBYTE  t;
    FAST TEXT   delimiter[1 + 1];

    equstring[0] = EOS;

START:

    // Directives (except Equates)----------------------------------------

    if   (LineBuffer[linecursor] == EOS)
    {   ;
    }
    elif (compareit("CEJE"  , TRUE)) asmwarning("ignoring directive CEJE"  ); // Conditional EJEct (Signetics Relocatable Assembler)
    elif (compareit("CSECT" , TRUE)) asmwarning("ignoring directive CSECT" ); //                   (Signetics)
    elif (compareit("ENTRY" , TRUE)) asmwarning("ignoring directive ENTRY" ); //                   (Signetics)
    elif (compareit("EJE"   , TRUE)) asmwarning("ignoring directive EJE"   ); // EJEct             (Signetics)
    elif (compareit("LIBR"  , TRUE)) asmwarning("ignoring directive LIBR"  ); // LIBRary           (Signetics Prometheus assembler)
    elif (compareit("NAME"  , TRUE)) asmwarning("ignoring directive NAME"  ); // NAMEt             (Japanese homebrews)
    elif (compareit("PAG"   , TRUE)) asmwarning("ignoring directive PAG"   ); // PAGe              (MicroByte)
    elif (compareit("PAGE"  , TRUE)) asmwarning("ignoring directive PAGE"  ); // PAGE
    elif (compareit("PCH"   , TRUE)) asmwarning("ignoring directive PCH"   ); // PunCH control     (Signetics)
    elif (compareit("PLEN"  , TRUE)) asmwarning("ignoring directive PLEN"  ); // Page LENgth
    elif (compareit("PRT"   , TRUE)) asmwarning("ignoring directive PRT"   ); // PRinT             (Signetics)
    elif (compareit("REPRO" , TRUE)) asmwarning("ignoring directive REPRO" ); // REPROduce         (Signetics Relocatable Assembler)
    elif (compareit("SPA"   , TRUE)) asmwarning("ignoring directive SPA"   ); // SPAce             (MicroByte)
    elif (compareit("SPAC"  , TRUE)) asmwarning("ignoring directive SPAC"  ); // SPACe             (MicroByte)
    elif (compareit("SPC"   , TRUE)) asmwarning("ignoring directive SPC"   ); // SPaCe             (Signetics)
    elif (compareit("TITL"  , TRUE)) asmwarning("ignoring directive TITL"  ); // TITLe             (Signetics)
    elif (compareit(".LIST" , TRUE)) asmwarning("ignoring directive .LIST" ); //                   (X2650 cross-asm)
    elif (compareit(".PAGE" , TRUE)) asmwarning("ignoring directive .PAGE" ); //                   (X2650 cross-asm)
    elif (compareit(".TITLE", TRUE)) asmwarning("ignoring directive .TITLE"); //                   (X2650 cross-asm)
    elif (compareit("INCBIN", TRUE))
    {   if (pass == 2 && GraphBuffer[0] == EOS)
        {   sprintf(GraphBuffer, "$0000 %s\n", fn_asm);
        }
        if (!includebin())
        {   return FALSE;
        }
        if (pass == 2)
        {   length = strlen(GraphBuffer);
            graphnesting++;
            for (i = 0; i < graphnesting; i++)
            {   GraphBuffer[length + i] = ' ';
            }
            sprintf(&GraphBuffer[length + graphnesting], "$%04X %s\n", ocursor, fn_inc);
            graphnesting--;
    }   }
    elif (compareit("INCLUDE", TRUE)) // INCLUDE (Japanese homebrews, et al.)
    {   if (pass == 2 && GraphBuffer[0] == EOS)
        {   sprintf(GraphBuffer, "$0000 %s\n", fn_asm);
        }
        if (!includefile())
        {   return FALSE;
        }
        if (pass == 2)
        {   length = strlen(GraphBuffer);
            graphnesting++;
            for (i = 0; i < graphnesting; i++)
            {   GraphBuffer[length + i] = ' ';
            }
            sprintf(&GraphBuffer[length + graphnesting], "$%04X %s\n", ocursor, fn_inc);
    }   }
    elif
    (   compareit("DATA",  TRUE) // Signetics
     || compareit("DB",    TRUE)
     || compareit(".BYTE", TRUE) // X2650 cross-asm
    )
    {   getimmediatebytes();
    } elif (compareit("DBX", TRUE)) // VACS 1.24f
    {   getdbx();
    } elif
    (   compareit("ACON",   TRUE) // Signetics
     || compareit("DW",     TRUE)
     || compareit(".DBYTE", TRUE) // X2650 cross-asm
    )
    {   getimmediatewords(FALSE);
    } elif (compareit(".ADDR", TRUE)) // X2650 cross-asm
    {   getimmediatewords(TRUE);
        asmwarning("wrong endianness");
    } elif (compareit("DFLT", TRUE)) // MicroByte, asm2650
    {   skipblanks(TRUE);
        if   (compareit("0", TRUE) || compareit("10", TRUE)) defasmbase = BASE_DECIMAL;
        elif (compareit("1", TRUE) || compareit("16", TRUE)) defasmbase = BASE_HEX;
        else                                                 asmerror("value out of range");
    } elif (compareit(".BIN", TRUE)) // X2650 cross-asm
    {   defasmbase = BASE_BINARY;
    } elif (compareit(".DECM", TRUE)) // X2650 cross-asm
    {   defasmbase = BASE_DECIMAL;
    } elif
    (   compareit("END",  TRUE) // Signetics TWIN
     || compareit(".END", TRUE) // X2650 cross-asm
    )
    {   if (!skipblanks(FALSE))
        {   value1 = parsenumber(&LineBuffer[linecursor]);
            startaddr_h = value1 / 256;
            startaddr_l = value1 % 256;
            specified = TRUE;
        }
        ended = TRUE;
        return TRUE;
    } elif (compareit(".OCT", TRUE)) // X2650 cross-asm
    {   defasmbase = BASE_OCTAL;
    } elif (compareit(".HEX", TRUE)) // X2650 cross-asm
    {   defasmbase = BASE_HEX;
    } elif
    (   compareit("ORG",  TRUE) // Signetics PIPLA and TWIN
     || compareit(".ORG", TRUE) // X2650 cros-asm
    )
    {   skipblanks(TRUE);
        value1 = parsenumber(&LineBuffer[linecursor]);
        if (value1 < ocursor)
        {   asmwarning("setting origin backwards");
        }
        orgy = TRUE;
        ocursor = value1;
    } elif (compareit("ORGF", TRUE))
    {   skipblanks(TRUE);
        value1 = parsenumber(&LineBuffer[linecursor]);
        if (value1 < ocursor)
        {   asmerror("invalid address");
        }
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        }
        value2 = fiximmediate(parsenumber(&LineBuffer[linecursor]));
        if (value2 < 0 || value2 > 255)
        {   asmerror("immediate value out of range");
            value1 = 0;
        }
        if (value1 > ocursor)
        {   t = (UBYTE) value2;
            for (i = ocursor; i < value1; i++)
            {   memory[i] = t;
        }   }
        orgy = TRUE;
        ocursor = value1;
    } elif
    (   compareit("RES",  TRUE) // Signetics
     || compareit("DS",   TRUE)
    )
    {   skipblanks(TRUE);
        ocursor += parsenumber(&LineBuffer[linecursor]);
    } elif (compareit(".ZERO", TRUE)) // X2650 cross-asm
    {   skipblanks(TRUE);
        value1 = parsenumber(&LineBuffer[linecursor]);
        for (i = 0; i < value1; i++)
        {   emit(0x00);
    }   }
    elif (compareit("START", TRUE)) // this is unofficial (CALM .START is official, though)
    {   skipblanks(TRUE);
        value1 = parsenumber(&LineBuffer[linecursor]);
        startaddr_h = value1 / 256;
        startaddr_l = value1 % 256;
        specified = TRUE;
    } elif
    (   compareit("ASCI"  , TRUE) // PIPLA
     || compareit("STRING", TRUE)
    )
    {   skipblanks(TRUE);
        delimiter[0] = LineBuffer[linecursor];
        if (delimiter[0] == EOS)
        {   asmerror("string delimiter expected");
        } else
        {   delimiter[1] = EOS;
            linecursor++;
            while (!compareit((STRPTR) delimiter, FALSE))
            {   emit(LineBuffer[linecursor++]);
    }   }   }
    elif
    (   compareit("RES",  TRUE) // FOO RES n (from asm2650)
     || compareit("DS",   TRUE)
    )
    {   skipblanks(TRUE);
        ocursor += parsenumber(&LineBuffer[linecursor]);
    }

    // Instructions-------------------------------------------------------

    elif (compareit("LODZ,r0", TRUE ))
    {   emit(0x00);
    } elif (compareit("LODZ,r1", TRUE ) || compareit("LODZ,r4", TRUE))
    {   emit(0x01);
    } elif (compareit("LODZ,r2", TRUE ) || compareit("LODZ,r5", TRUE))
    {   emit(0x02);
    } elif (compareit("LODZ,r3", TRUE ) || compareit("LODZ,r6", TRUE))
    {   emit(0x03);
    } elif (compareit("LODZ",    TRUE )) // $00..$03
    {   skipblanks(TRUE);
        emit(getreg_signetics(1, TRUE));
    } elif (compareit("LODI,",   FALSE)) // $04..$07
    {   emit(0x04 + getreg_signetics(2, TRUE));
        getimmediate(TRUE);
    } elif (compareit("LODR,",   FALSE)) // $08..$0B
    {   emit(0x08 + getreg_signetics(2, TRUE));
        getrelative(FALSE);
    } elif (compareit("LODA,",   FALSE)) // $0C..$0F
    {   getabsolute_nonbranch_signetics(0x0C, -1);
    } elif (compareit("LODA",    TRUE)) // $0C
    {   getabsolute_nonbranch_signetics(0x0C, 0);
    } elif (compareit("LDPL",    TRUE ))  // $10
    {   emit(0x10);
        skipblanks(TRUE);
        getabsolute_branch(); // yes, branch!
        if (!supercpu)
        {   asmerror("2650B only");
    }   }
    elif   (compareit("STPL",    TRUE )) // $11
    {   emit(0x11);
        skipblanks(TRUE);
        getabsolute_branch(); // yes, branch!
        if (!supercpu)
        {   asmerror("2650B only");
    }   }
    elif   (compareit("SPSU",    TRUE ) || compareit("SPU",     TRUE )) // $12
    {   emit(0x12);
    } elif (compareit("SPSL",    TRUE ) || compareit("SPL",     TRUE )) // $13
    {   emit(0x13);
    } elif (compareit("RETC,",   FALSE) || compareit("RTC,",    FALSE)) // $14..$17
    {   emit(0x14 + getcc_signetics(TRUE));
    } elif (compareit("RETC",    TRUE ) || compareit("RTC",     TRUE )) // $17
    {   emit(0x17);
    } elif (compareit("BCTR,",   FALSE)) // $18..$1B
    {   emit(0x18 + getcc_signetics(TRUE));
        getrelative(FALSE);
    } elif (compareit("BCTR",    TRUE )) // $1B
    {   emit(0x1B);
        getrelative(FALSE);
    } elif (compareit("BCTA,",   FALSE)) // $1C..$1F
    {   emit(0x1C + getcc_signetics(TRUE));
        getabsolute_branch();
    } elif (compareit("BCTA",    TRUE )) // $1F
    {   emit(0x1F);
        getabsolute_branch();
    } elif (compareit("EORZ,r0", TRUE ) || compareit("XORZ,r0", TRUE ))
    {   emit(0x20);
    } elif (compareit("EORZ,r1", TRUE ) || compareit("EORZ,r4", TRUE ) || compareit("XORZ,r1", TRUE ) || compareit("XORZ,r4", TRUE ))
    {   emit(0x21);
    } elif (compareit("EORZ,r2", TRUE ) || compareit("EORZ,r5", TRUE ) || compareit("XORZ,r2", TRUE ) || compareit("XORZ,r5", TRUE ))
    {   emit(0x22);
    } elif (compareit("EORZ,r3", TRUE ) || compareit("EORZ,r6", TRUE ) || compareit("XORZ,r3", TRUE ) || compareit("XORZ,r6", TRUE ))
    {   emit(0x23);
    } elif (compareit("EORZ",    TRUE ) || compareit("XORZ",    TRUE )) // $20..$23
    {   skipblanks(TRUE);
        emit(0x20 + getreg_signetics(2, TRUE));
    } elif (compareit("EORI,",   FALSE) || compareit("XORI,",   FALSE)) // $24..$27
    {   emit(0x24 + getreg_signetics(2, TRUE));
        getimmediate(TRUE);
    } elif (compareit("EORR,",   FALSE) || compareit("XORR,",   FALSE)) // $28..$2B
    {   emit(0x28 + getreg_signetics(2, TRUE));
        getrelative(FALSE);
    } elif (compareit("EORA,",   FALSE) || compareit("XORA,",   FALSE)) // $2C..$2F
    {   getabsolute_nonbranch_signetics(0x2C, -1);
    } elif (compareit("EORA",    TRUE)) // $2C
    {   getabsolute_nonbranch_signetics(0x2C, 0);
    } elif (compareit("REDC,",   FALSE) || compareit("RDC,",    FALSE)) // $30..$33
    {   emit(0x30 + getreg_signetics(2, TRUE));
    } elif (compareit("RETE,",   FALSE) || compareit("RTE,",    FALSE)) // $34..$37
    {   emit(0x34 + getcc_signetics(TRUE));
    } elif (compareit("RETE",    TRUE ) || compareit("RTE",     TRUE )) // $37
    {   emit(0x37);
    } elif (compareit("BSTR,",   FALSE)) // $38..$3B
    {   emit(0x38 + getcc_signetics(TRUE));
        getrelative(FALSE);
    } elif (compareit("BSTR",    TRUE )) // $3B
    {   emit(0x3B);
        getrelative(FALSE);
    } elif (compareit("BSTA,",   FALSE)) // $3C..$3F
    {   emit(0x3C + getcc_signetics(TRUE));
        getabsolute_branch();
    } elif (compareit("BSTA",    TRUE )) // $3F
    {   emit(0x3F);
        getabsolute_branch();
    } elif (compareit("HALT",    TRUE ) || compareit("HLT",     TRUE ) /* || compareit("ANDZ,r0", TRUE) */ ) // $40
    {   emit(0x40);
    } elif (compareit("ANDZ,r1", TRUE ) || compareit("ANDZ,r4", TRUE ))
    {   emit(0x41);
    } elif (compareit("ANDZ,r2", TRUE ) || compareit("ANDZ,r5", TRUE ))
    {   emit(0x42);
    } elif (compareit("ANDZ,r3", TRUE ) || compareit("ANDZ,r6", TRUE ))
    {   emit(0x43);
    } elif (compareit("ANDZ",    TRUE )) // $41..$43
    {   skipblanks(TRUE);
        emit(0x40 + getreg_signetics(0, TRUE));
    } elif (compareit("ANDI,",   FALSE)) // $44..$47
    {   emit(0x44 + getreg_signetics(2, TRUE));
        getimmediate(TRUE);
    } elif (compareit("ANDR,",   FALSE)) // $48..$4B
    {   emit(0x48 + getreg_signetics(2, TRUE));
        getrelative(FALSE);
    } elif (compareit("ANDA,",   FALSE)) // $4C..$4F
    {   getabsolute_nonbranch_signetics(0x4C, -1);
    } elif (compareit("ANDA",    TRUE)) // $4C
    {   getabsolute_nonbranch_signetics(0x4C, 0);
    } elif (compareit("RRR,",    FALSE)) // $50..$53
    {   emit(0x50 + getreg_signetics(2, TRUE));
    } elif (compareit("REDE,",   FALSE) || compareit("RDE,",    FALSE)) // $54..$57
    {   emit(0x54 + getreg_signetics(2, TRUE));
        getimmediate(TRUE);
    } elif (compareit("BRNR,",   FALSE)) // $58..$5B
    {   emit(0x58 + getreg_signetics(2, TRUE));
        getrelative(FALSE);
    } elif (compareit("BRNA,",   FALSE)) // $5C..$5F
    {   emit(0x5C + getreg_signetics(2, TRUE));
        getabsolute_branch();
    } elif (compareit("IORZ,r0", TRUE ))
    {   emit(0x60);
    } elif (compareit("IORZ,r1", TRUE ) || compareit("IORZ,r4", TRUE ))
    {   emit(0x61);
    } elif (compareit("IORZ,r2", TRUE ) || compareit("IORZ,r5", TRUE ))
    {   emit(0x62);
    } elif (compareit("IORZ,r3", TRUE ) || compareit("IORZ,r6", TRUE ))
    {   emit(0x63);
    } elif (compareit("IORZ",    TRUE )) // $60..$63
    {   skipblanks(TRUE);
        emit(0x60 + getreg_signetics(2, TRUE));
    } elif (compareit("IORI,",   FALSE)) // $64..$67
    {   emit(0x64 + getreg_signetics(2, TRUE));
        getimmediate(TRUE);
    } elif (compareit("IORR,",   FALSE)) // $68..$6B
    {   emit(0x68 + getreg_signetics(2, TRUE));
        getrelative(FALSE);
    } elif (compareit("IORA,",   FALSE)) // $6C..$6F
    {   getabsolute_nonbranch_signetics(0x6C, -1);
    } elif (compareit("IORA",    TRUE)) // $6C
    {   getabsolute_nonbranch_signetics(0x6C, 0);
    } elif (compareit("REDD,",   FALSE) || compareit("RDD,",    FALSE)) // $70..$73
    {   emit(0x70 + getreg_signetics(2, TRUE));
    } elif (compareit("CPSU",    TRUE ) || compareit("CPU",     TRUE )) // $74
    {   emit(0x74);
        getimmediate(TRUE);
    } elif (compareit("CPSL",    TRUE ) || compareit("CPL",     TRUE )) // $75
    {   emit(0x75);
        getimmediate(TRUE);
    } elif (compareit("PPSU",    TRUE ) || compareit("PPU",     TRUE )) // $76
    {   emit(0x76);
        getimmediate(TRUE);
    } elif (compareit("PPSL",    TRUE ) || compareit("PPL",     TRUE )) // $77
    {   emit(0x77);
        getimmediate(TRUE);
    } elif (compareit("BSNR,",   FALSE)) // $78..$7B
    {   emit(0x78 + getreg_signetics(2, TRUE));
        getrelative(FALSE);
    } elif (compareit("BSNA,",   FALSE)) // $7C..$7F
    {   emit(0x7C + getreg_signetics(2, TRUE));
        getabsolute_branch();
    } elif (compareit("ADDZ,r0", TRUE ))
    {   emit(0x80);
    } elif (compareit("ADDZ,r1", TRUE ) || compareit("ADDZ,r4", TRUE ))
    {   emit(0x81);
    } elif (compareit("ADDZ,r2", TRUE ) || compareit("ADDZ,r5", TRUE ))
    {   emit(0x82);
    } elif (compareit("ADDZ,r3", TRUE ) || compareit("ADDZ,r6", TRUE ))
    {   emit(0x83);
    } elif (compareit("ADDZ",    TRUE )) // $80..$83
    {   skipblanks(TRUE);
        emit(0x80 + getreg_signetics(2, TRUE));
    } elif (compareit("ADDI,",   FALSE)) // $84..$87
    {   emit(0x84 + getreg_signetics(2, TRUE));
        getimmediate(TRUE);
    } elif (compareit("ADDR,",   FALSE)) // $88..$8B
    {   emit(0x88 + getreg_signetics(2, TRUE));
        getrelative(FALSE);
    } elif (compareit("ADDA,",   FALSE)) // $8C..$8F
    {   getabsolute_nonbranch_signetics(0x8C, -1);
    } elif (compareit("ANDA",    TRUE)) // $8C
    {   getabsolute_nonbranch_signetics(0x8C, 0);
    } elif (compareit("LPSU",    TRUE ) || compareit("LPU",     TRUE ))  // $92
    {   emit(0x92);
    } elif (compareit("LPSL",    TRUE ) || compareit("LPL",     TRUE ))  // $93
    {   emit(0x93);
    } elif (compareit("DAR,",    FALSE) || compareit("DA,",     FALSE)) // $94..$97
    {   emit(0x94 + getreg_signetics(2, TRUE));
    } elif (compareit("BCFR,",   FALSE)) // $98..$9A
    {   emit(0x98 + getcc_signetics(FALSE));
        getrelative(FALSE);
    } elif (compareit("ZBRR",    TRUE )) // $9B
    {   emit(0x9B);
        skipblanks(TRUE);
        getzeropage(FALSE);
    } elif (compareit("BCFA,",   FALSE)) // $9C..$9E
    {   emit(0x9C + getcc_signetics(FALSE));
        getabsolute_branch();
    } elif (compareit("BXA,",    FALSE)) // $9F BXA,r3 $1234
    {   emit(0x9C + getreg_signetics(0, FALSE));
        getabsolute_branch();
    } elif (compareit("BXA",     TRUE )) // $9F BXA    $1234,r3
    {   getabsolute_indexed_signetics(0x9F);
    } elif (compareit("SUBZ,r0", TRUE ))
    {   emit(0xA0);
    } elif (compareit("SUBZ,r1", TRUE ) || compareit("SUBZ,r4", TRUE ))
    {   emit(0xA1);
    } elif (compareit("SUBZ,r2", TRUE ) || compareit("SUBZ,r5", TRUE ))
    {   emit(0xA2);
    } elif (compareit("SUBZ,r3", TRUE ) || compareit("SUBZ,r6", TRUE ))
    {   emit(0xA3);
    } elif (compareit("SUBZ",    TRUE )) // $A0..$A3
    {   skipblanks(TRUE);
        emit(0xA0 + getreg_signetics(2, TRUE));
    } elif (compareit("SUBI,",   FALSE)) // $A4..$A7
    {   emit(0xA4 + getreg_signetics(2, TRUE));
        getimmediate(TRUE);
    } elif (compareit("SUBR,",   FALSE)) // $A8..$AB
    {   emit(0xA8 + getreg_signetics(2, TRUE));
        getrelative(FALSE);
    } elif (compareit("SUBA,",   FALSE)) // $AC..$AF
    {   getabsolute_nonbranch_signetics(0xAC, -1);
    } elif (compareit("SUBA",    TRUE)) // $AC
    {   getabsolute_nonbranch_signetics(0xAC, 0);
    } elif (compareit("WRTC,",   FALSE) || compareit("WRC,",    FALSE)) // $B0..$B3
    {   emit(0xB0 + getreg_signetics(2, TRUE));
    } elif (compareit("TPSU",    TRUE ) || compareit("TPU",     TRUE )) // $B4
    {   emit(0xB4);
        getimmediate(TRUE);
    } elif (compareit("TPSL",    TRUE ) || compareit("TPL",     TRUE )) // $B5
    {   emit(0xB5);
        getimmediate(TRUE);
    } elif (compareit("BSFR,",   FALSE)) // $B8..$BA
    {   emit(0xB8 + getcc_signetics(FALSE));
        getrelative(FALSE);
    } elif (compareit("ZBSR",    TRUE )) // $BB
    {   emit(0xBB);
        skipblanks(TRUE);
        getzeropage(FALSE);
    } elif (compareit("BSFA,",   FALSE)) // $BC..$BE
    {   emit(0xBC + getcc_signetics(FALSE));
        getabsolute_branch();
    } elif (compareit("BSXA,",   FALSE)) // $BF BSXA,r3 $1234
    {   emit(0xBC + getreg_signetics(0, FALSE));
        getabsolute_branch();
    } elif (compareit("BSXA",    TRUE )) // $BF BSXA    $1234,r3
    {   getabsolute_indexed_signetics(0xBF);
    } elif (compareit("NOP",     TRUE ) /* || compareit("STRZ,r0", TRUE) */ ) // $C0
    {   emit(0xC0);
    } elif (compareit("STRZ,r1", TRUE ) || compareit("STRZ,r4", TRUE ))
    {   emit(0xC1);
    } elif (compareit("STRZ,r2", TRUE ) || compareit("STRZ,r5", TRUE ))
    {   emit(0xC2);
    } elif (compareit("STRZ,r3", TRUE ) || compareit("STRZ,r6", TRUE ))
    {   emit(0xC3);
    } elif (compareit("STRZ",    TRUE )) // $C1..$C3
    {   skipblanks(TRUE);
        emit(0xC0 + getreg_signetics(0, TRUE));
    } elif (compareit("STRR,",   FALSE)) // $C8..$CB
    {   emit(0xC8 + getreg_signetics(2, TRUE));
        getrelative(FALSE);
    } elif (compareit("STRA,",   FALSE)) // $CC..$CF
    {   getabsolute_nonbranch_signetics(0xCC, -1);
    } elif (compareit("STRA",    TRUE)) // $CC
    {   getabsolute_nonbranch_signetics(0xCC, 0);
    } elif (compareit("RRL,",    FALSE)) // $D0..$D3
    {   emit(0xD0 + getreg_signetics(2, TRUE));
    } elif (compareit("WRTE,",   FALSE) || compareit("WRE,",    FALSE)) // $D4..$D7
    {   emit(0xD4 + getreg_signetics(2, TRUE));
        getimmediate(TRUE);
    } elif (compareit("BIRR,",   FALSE)) // $D8..$DB
    {   emit(0xD8 + getreg_signetics(2, TRUE));
        getrelative(FALSE);
    } elif (compareit("BIRA,",   FALSE)) // $DC..$DF
    {   emit(0xDC + getreg_signetics(2, TRUE));
        getabsolute_branch();
    } elif (compareit("COMZ,r0", TRUE ))
    {   emit(0xE0);
    } elif (compareit("COMZ,r1", TRUE ) || compareit("COMZ,r4", TRUE) )
    {   emit(0xE1);
    } elif (compareit("COMZ,r2", TRUE ) || compareit("COMZ,r5", TRUE) )
    {   emit(0xE2);
    } elif (compareit("COMZ,r3", TRUE ) || compareit("COMZ,r6", TRUE) )
    {   emit(0xE3);
    } elif (compareit("COMZ",    TRUE )) // $E0..$E3
    {   skipblanks(TRUE);
        emit(0xE0 + getreg_signetics(2, TRUE));
    } elif (compareit("COMI,",   FALSE)) // $E4..$E7
    {   emit(0xE4 + getreg_signetics(2, TRUE));
        getimmediate(TRUE);
    } elif (compareit("COMR,",   FALSE)) // $E8..$EB
    {   emit(0xE8 + getreg_signetics(2, TRUE));
        getrelative(FALSE);
    } elif (compareit("COMA,",   FALSE)) // $EC..$EF
    {   getabsolute_nonbranch_signetics(0xEC, -1);
    } elif (compareit("COMA",    TRUE)) // $EC
    {   getabsolute_nonbranch_signetics(0xEC, 0);
    } elif (compareit("WRTD,",   FALSE) || compareit("WRD,",    FALSE)) // $F0..$F3
    {   emit(0xF0 + getreg_signetics(2, TRUE));
    } elif (compareit("TMI,",    FALSE)) // $F4..$F7
    {   emit(0xF4 + getreg_signetics(2, TRUE));
        getimmediate(TRUE);
    } elif (compareit("BDRR,",   FALSE)) // $F8..$FB
    {   emit(0xF8 + getreg_signetics(2, TRUE));
        getrelative(FALSE);
    } elif (compareit("BDRA,",   FALSE)) // $FC..$FF
    {   emit(0xFC + getreg_signetics(2, TRUE));
        getabsolute_branch();
    }

    // Extended instruction set (supported by Signetics RASM assembler)---

    elif (style == STYLE_SIGNETICS2 && (compareit("RET" , TRUE)                                                     )) { emit(0x17);                       } // RETC,un
    elif (style == STYLE_SIGNETICS2 && (compareit("BER" , TRUE) || compareit("BZR" , TRUE) || compareit("BOR", TRUE))) { emit(0x18); getrelative(FALSE);        } // BCTR,eq
    elif (style == STYLE_SIGNETICS2 && (compareit("BHR" , TRUE) || compareit("BPR" , TRUE)                          )) { emit(0x19); getrelative(FALSE);        } // BCTR,gt
    elif (style == STYLE_SIGNETICS2 && (compareit("BLR" , TRUE) || compareit("BMR" , TRUE)                          )) { emit(0x1A); getrelative(FALSE);        } // BCTR,lt
    elif (style == STYLE_SIGNETICS2 && (compareit("BR"  , TRUE)                                                     )) { emit(0x1B); getrelative(FALSE);        } // BCTR,un
    elif (style == STYLE_SIGNETICS2 && (compareit("BEA" , TRUE) || compareit("BZA" , TRUE) || compareit("BOA", TRUE))) { emit(0x1C); getabsolute_branch(); } // BCTA,eq
    elif (style == STYLE_SIGNETICS2 && (compareit("BHA" , TRUE) || compareit("BPA" , TRUE)                          )) { emit(0x1D); getabsolute_branch(); } // BCTA,gt
    elif (style == STYLE_SIGNETICS2 && (compareit("BLA" , TRUE) || compareit("BMA" , TRUE)                          )) { emit(0x1E); getabsolute_branch(); } // BCTA,lt
    elif (style == STYLE_SIGNETICS2 && (compareit("BA"  , TRUE)                                                     )) { emit(0x1F); getabsolute_branch(); } // BCTA,un
    elif (style == STYLE_SIGNETICS2 && (compareit("BSR" , TRUE)                                                     )) { emit(0x3B); getrelative(FALSE);        } // BSTR,un
    elif (style == STYLE_SIGNETICS2 && (compareit("BSA" , TRUE)                                                     )) { emit(0x3F); getabsolute_branch(); } // BSTA,un
    elif (style == STYLE_SIGNETICS2 && (compareit("BNER", TRUE) || compareit("BNZR", TRUE)                          )) { emit(0x98); getrelative(FALSE);        } // BCFR,eq
    elif (style == STYLE_SIGNETICS2 && (compareit("BNHR", TRUE) || compareit("BNPR", TRUE)                          )) { emit(0x99); getrelative(FALSE);        } // BCFR,gt
    elif (style == STYLE_SIGNETICS2 && (compareit("BNLR", TRUE) || compareit("BNMR", TRUE)                          )) { emit(0x9A); getrelative(FALSE);        } // BCFR,lt
    elif (style == STYLE_SIGNETICS2 && (compareit("BNEA", TRUE) || compareit("BNZA", TRUE)                          )) { emit(0x9C); getabsolute_branch(); } // BCFA,eq
    elif (style == STYLE_SIGNETICS2 && (compareit("BNHA", TRUE) || compareit("BNPA", TRUE)                          )) { emit(0x9D); getabsolute_branch(); } // BCFA,gt
    elif (style == STYLE_SIGNETICS2 && (compareit("BNLA", TRUE) || compareit("BNMA", TRUE)                          )) { emit(0x9E); getabsolute_branch(); } // BCFA,lt

    // Labels and Equates-------------------------------------------------

    elif (equstring[0] || linecursor != 0)
    {   asmerror("syntax error");
        return FALSE;
    } else
    {   i = 0;
        while
        (   LineBuffer[linecursor] != ' '
         && LineBuffer[linecursor] != HT
         && LineBuffer[linecursor] != EOS
         && LineBuffer[linecursor] != ':'
         && LineBuffer[linecursor] != ','
         && LineBuffer[linecursor] != '>'
         && i <= LABELLIMIT
        )
        {   equstring[i++] = LineBuffer[linecursor++];
        }
        equstring[i] = EOS;
        if
        (   LineBuffer[linecursor] == ':' // label with colon
         || LineBuffer[linecursor] == ',' // label with comma (X2650 cross-asm)
         || LineBuffer[linecursor] == '>' // label with > (PIP4K source code)
        )
        {   linecursor++; // to get past it
        } elif (i > LABELLIMIT)
        {   asmerror("label too long");
            return FALSE;
        }

        skipblanks(FALSE);
        if
        (   compareit("EQU",  TRUE)
         || compareit("SET",  TRUE) // from PIPLA
         || compareit(".EQU", TRUE) // from X2650 cross-asm
         || compareit(".SET", TRUE) // from X2650 cross-asm
        )
        {   skipblanks(TRUE);
            equmode = TRUE;
            value1 = parsenumber(&LineBuffer[linecursor]);
            equmode = FALSE;
            if (pass == 1)
            {   adduserlabel((STRPTR) equstring, (UWORD) value1, UNKNOWN);
        }   }
        else
        {   adduserlabel((STRPTR) equstring, (UWORD) ocursor, UNKNOWN);
            goto START;
    }   }

    return TRUE;
}

MODULE FLAG asm_oldcalm(void)
{   FAST UBYTE byte1st,
               byte2nd,
               reg1, reg2;
    FAST int   i,
               value1;

    equstring[0] = EOS;

START:
    if (asm_calmdirective())
    {   return TRUE;
    }

    // Opcodes------------------------------------------------------------

    if (compareit("NOT", TRUE))                        // NOT reg -> XOR reg,$FF (EORI,reg $FF)
    {   skipblanks(TRUE);
        emit(0x24 + getreg_calm(2, TRUE));
        emit(0xFF);
    } elif (compareit("LOAD", TRUE))
    {   skipblanks(TRUE);
        if     (compareit("U,A", TRUE))                // LOAD U,A (LPSU)
        {   emit(0x92);
        } elif (compareit("L,A", TRUE))                // LOAD L,A (LPSL)
        {   emit(0x93);
        } elif (compareit("A,U", TRUE))                // LOAD A,U (SPSU)
        {   emit(0x12);
        } elif (compareit("A,L", TRUE))                // LOAD A,L (SPSL)
        {   emit(0x13);
        } elif (compareit("L,", FALSE))                // LOAD L,abs (LDPL abs)
        {   emit(0x10);
            getabsolute_branch(); // yes, branch!
            if (!supercpu)
            {   asmerror("2650B only");
        }   }
        elif   (compareit("$CTRL,", FALSE))            // LOAD $CTRL,reg (WRTC reg)
        {   emit(0xB0 + getreg_calm(2, TRUE));
        } elif (compareit("$DATA,", FALSE))            // LOAD $DATA,reg (WRTD reg)
        {   emit(0xF0 + getreg_calm(2, TRUE));
        } elif (compareit("$", FALSE))                 // LOAD $port,reg (WRTE,reg)
        {   value1 = fiximmediate(parsenumber(&LineBuffer[linecursor]));
            if (value1 < 0 || value1 > 255)
            {   asmerror("immediate value out of range");
                value1 = 0;
            }
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg1 = getreg_calm(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            emit(0xD4 + reg1);
            emit((UBYTE) value1);
        } elif (compareit("@.+", FALSE))               // LOAD @.+rel,reg (STRR,reg *rel)
        {   byte1st = dorel(2, TRUE);
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            emit(0xC8 + getreg_calm(2, TRUE));
            emit(byte1st);
        } elif (compareit(".+", FALSE))                // LOAD .+rel,reg (STRR,reg rel)
        {   byte1st = dorel(2, FALSE);
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            emit(0xC8 + getreg_calm(2, TRUE));
            emit(byte1st);
        } else
        {   reg1 = getreg_calm(2, TRUE);
            if (reg1 <= 3)
            {   if (!compareit(",", FALSE))
                {   asmerror("comma expected");
                }
                if (compareit("#", FALSE))                 // LOAD reg,#imm
                {   emit(0x04 + reg1);
                    getimmediate(FALSE);
                } elif (compareit("@.+", FALSE))           // LOAD reg,@.+rel (LODR)
                {   emit(0x08 + reg1);
                    getrelative(TRUE);
                } elif (compareit(".+", FALSE))            // LOAD reg,.+rel (LODR)
                {   emit(0x08 + reg1);
                    getrelative(FALSE);
                } elif (compareit("$CTRL", TRUE))          // LOAD reg,$CTRL (REDC)
                {   emit(0x30 + reg1);
                } elif (compareit("$DATA", TRUE))          // LOAD reg,$DATA (REDD)
                {   emit(0x70 + reg1);
                } elif (compareit("$", FALSE))             // LOAD reg,$port (REDE)
                {   emit(0x54 + reg1);
                    getimmediate(FALSE);
                } else
                {   reg2 = getreg_calm(2, TRUE);
                    if (reg2 <= 3)
                    {   if (reg1 == 0 && reg2 == 0)        // LOAD A,A (LODZ/STRZ)
                        {   emit(0x00);
                            asmwarning("indeterminate opcode");
                        } elif (reg1 == 0)
                        {   emit(0x00 + reg2);             // LOAD A,reg (LODZ)
                        } elif (reg2 == 0)
                        {   emit(0xC0 + reg1);             // LOAD reg,A (STRZ)
                        } else
                        {   asmerror("one register must be A");
                    }   }
                    else
                    {   getabsolute_nonbranch_oldcalm((UBYTE) (0x0C + reg1)); // LOAD reg,abs (LODA)
            }   }   }
            else
            {   // This resolves it as a non-branch absolute address (assumes STRA),
                // but STPL is actually meant to be resolved as a branch absolute address.
                if     (compareit("(-", FALSE))
                {   byte1st = 0x40;
                } elif (compareit("(+", FALSE))
                {   byte1st = 0x20;
                } elif (compareit("(",  FALSE))
                {   byte1st = 0x60;
                } else
                {   byte1st = 0;
                }
                reg1 = 4;
                if (byte1st)
                {   reg1 = getreg_calm(2, TRUE);
                    if (!compareit(")+", FALSE))
                    {   asmerror(")+ expected");
                }   }
                if (compareit("@", FALSE))
                {   byte1st |= 0x80;
                }
                value1  =  parsenumber(&LineBuffer[linecursor]);
                byte1st |= (value1 / 256);
                byte2nd =  (value1 % 256);
                if (!compareit(",", FALSE))
                {   asmerror("comma expected");
                }
                if (compareit("L", TRUE))         // LOAD abs,L (STPL)
                {   emit(0x12);
                    emit(byte1st);
                    emit(byte2nd);
                    if (!supercpu)
                    {   asmerror("2650B only");
                }   }
                else
                {   reg2 = getreg_calm(2, TRUE);   // LOAD abs,reg (STRA)
                    if (reg1 <= 3)
                    {   emit(0xCC + reg1);
                        if (reg2 != 0)
                        {   asmerror("bad operand");
                    }   }
                    elif (reg2 <= 3)
                    {   emit(0xCC + reg2);
                    } else
                    {   emit(0xCC);
                        asmerror("register expected");
                    }
                    emit(byte1st);
                    emit(byte2nd);
    }   }   }   }
    elif (compareit("RET", TRUE))
    {   emit(0x17);
    } elif (compareit("RET,", FALSE))
    {   emit(0x14 + getcc_calm());
    } elif (compareit("JUMP,", FALSE))
    {   if     (compareit("ANE", TRUE))
        {   getaddress(0x58);
        } elif (compareit("BNE", TRUE) || compareit("B'NE", TRUE))
        {   getaddress(0x59);
        } elif (compareit("CNE", TRUE) || compareit("C'NE", TRUE))
        {   getaddress(0x5A);
        } elif (compareit("DNE", TRUE) || compareit("D'NE", TRUE))
        {   getaddress(0x5B);
        } elif (compareit("EQ", TRUE))
        {   getaddress(0x18);
        } elif (compareit("GT", TRUE))
        {   getaddress(0x19);
        } elif (compareit("LT", TRUE))
        {   getaddress(0x1A);
        } elif (compareit("NE", TRUE))
        {   getaddress(0x98);
        } elif (compareit("LE", TRUE))
        {   getaddress(0x99);
        } elif (compareit("GE", TRUE))
        {   getaddress(0x9A);
    }   }
    elif (compareit("JUMP", TRUE))
    {   skipblanks(TRUE);
        if (compareit("(D)+", FALSE))
        {   emit(0x9F);
            getabsolute_branch();
        } elif (compareit("@0+", FALSE))
        {   emit(0x9B);
            getzeropage(TRUE);
        } elif (compareit("0+", FALSE))
        {   emit(0x9B);
            getzeropage(FALSE);
        } elif (compareit("@.+", FALSE))
        {   emit(0x1B);
            getrelative(TRUE);
        } elif (compareit(".+", FALSE))
        {   emit(0x1B);
            getrelative(FALSE);
        } else
        {   emit(0x1F);
            getabsolute_branch();
    }   }
    elif (compareit("CLR", TRUE))
    {   skipblanks(TRUE);
        if   (compareit("A"        , TRUE))   emit(0x20);               // CLR A is same as XOR A,A
        elif (compareit("STACK"    , TRUE)) { emit(0x74); emit(0x07); }
        elif (compareit("IOF"      , TRUE)) { emit(0x74); emit(0x20); } // CLR IOF is same as ION
        elif (compareit("OUTPUT"   , TRUE)) { emit(0x74); emit(0x40); }
        elif (compareit("INPUT"    , TRUE)) { emit(0x74); emit(0x80); }
        elif (compareit("CARRY"    , TRUE)) { emit(0x75); emit(0x01); } // CLR CARRY is same as CLRC
        elif (compareit("LOGICOMP" , TRUE)) { emit(0x75); emit(0x02); }
        elif (compareit("OVERFLOW" , TRUE)) { emit(0x75); emit(0x04); } // CLR OVERFLOW is same as CLRV
        elif (compareit("WITHCARRY", TRUE)) { emit(0x75); emit(0x08); }
        elif (compareit("BANK", TRUE) || compareit("BANK1", TRUE))
                                            { emit(0x75); emit(0x10); }
        elif (compareit("HALFCARRY", TRUE)) { emit(0x75); emit(0x20); }
        else                                { emit(0x75); emit(0x00);
                                              asmerror("bad operand"); }
    } elif (compareit("CLRC", TRUE)) // CLRC is same as CLR CARRY
    {   emit(0x75);
        emit(0x01);
    } elif (compareit("CLRV", TRUE)) // CLRV is same as CLR OVERFLOW
    {   emit(0x75);
        emit(0x04);
    } elif (compareit("ION", TRUE)) // ION is same as CLR IOF
    {   emit(0x74);
        emit(0x20);
    } elif (compareit("SET", TRUE))
    {   skipblanks(TRUE);
        if   (compareit("IOF"      , TRUE)) { emit(0x76); emit(0x20); } // SET IOF is same as IOF
        elif (compareit("OUTPUT"   , TRUE)) { emit(0x76); emit(0x40); }
        elif (compareit("INPUT"    , TRUE)) { emit(0x76); emit(0x80); }
        elif (compareit("CARRY"    , TRUE)) { emit(0x77); emit(0x01); } // SET CARRY is same as SETC
        elif (compareit("LOGICOMP" , TRUE)) { emit(0x77); emit(0x02); }
        elif (compareit("OVERFLOW" , TRUE)) { emit(0x77); emit(0x04); } // SET OVERFLOW is same as SETV
        elif (compareit("WITHCARRY", TRUE)) { emit(0x77); emit(0x08); }
        elif (compareit("BANK"     , TRUE) || compareit("BANK1", TRUE))
                                            { emit(0x77); emit(0x10); }
        elif (compareit("HALFCARRY", TRUE)) { emit(0x77); emit(0x20); }
        else                                { emit(0x76); emit(0x00);
                                              asmerror("bad operand"); }
    } elif (compareit("SETC", TRUE)) // SETC is same as SET CARRY
    {   emit(0x77);
        emit(0x01);
    } elif (compareit("SETV", TRUE)) // SETV is same as SET OVERFLOW
    {   emit(0x77);
        emit(0x04);
    } elif (compareit("IOF", TRUE)) // IOF is same as SET IOF
    {   emit(0x76);
        emit(0x20);
    } elif (compareit("XOR", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_calm(2, TRUE);
        if (reg1 == 4)
        {   asmerror("register expected");
            reg1 = 0;
        }
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        }
        if (compareit("#", FALSE))
        {   emit(0x24 + reg1);
            getimmediate(FALSE);
        } elif (compareit("@.+", FALSE))
        {   emit(0x28 + reg1);
            getrelative(TRUE);
        } elif (compareit(".+", FALSE))
        {   emit(0x28 + reg1);
            getrelative(FALSE);
        } else
        {   reg2 = 4;
            if (reg1 == 0)
            {   reg2 = getreg_calm(2, TRUE);
                if (reg2 <= 3)
                {   emit(0x20 + reg2);
            }   }
            if (reg2 == 4)
            {   getabsolute_nonbranch_oldcalm((UBYTE) (0x2C + reg1));
    }   }   }
    elif (compareit("RETION", TRUE))
    {   emit(0x37);
    } elif (compareit("RETION,", FALSE))
    {   emit(0x34 + getcc_calm());
    } elif (compareit("CALL,", FALSE))
    {   if     (compareit("ANE", TRUE))
        {   getaddress(0x78);
        } elif (compareit("BNE", TRUE) || compareit("B'NE", TRUE))
        {   getaddress(0x79);
        } elif (compareit("CNE", TRUE) || compareit("C'NE", TRUE))
        {   getaddress(0x7A);
        } elif (compareit("DNE", TRUE) || compareit("D'NE", TRUE))
        {   getaddress(0x7B);
        } elif (compareit("EQ", TRUE))
        {   getaddress(0x38);
        } elif (compareit("GT", TRUE))
        {   getaddress(0x39);
        } elif (compareit("LT", TRUE))
        {   getaddress(0x3A);
        } elif (compareit("NE", TRUE))
        {   getaddress(0xB8);
        } elif (compareit("LE", TRUE))
        {   getaddress(0xB9);
        } elif (compareit("GE", TRUE))
        {   getaddress(0xBA);
    }   }
    elif (compareit("CALL", TRUE))
    {   skipblanks(TRUE);
        if (compareit("(D)+", FALSE))
        {   emit(0xBF);
            getabsolute_branch();
        } elif (compareit("@0+", FALSE))
        {   emit(0xBB);
            getzeropage(TRUE);
        } elif (compareit("0+", FALSE))
        {   emit(0xBB);
            getzeropage(FALSE);
        } elif (compareit("@.+", FALSE))
        {   emit(0x3B);
            getrelative(TRUE);
        } elif (compareit(".+", FALSE))
        {   emit(0x3B);
            getrelative(FALSE);
        } else
        {   emit(0x3F);
            getabsolute_branch();
    }   }
    elif (compareit("WAIT", TRUE))
    {   emit(0x40);
    } elif (compareit("AND", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_calm(2, TRUE);
        if (reg1 == 4)
        {   asmerror("register expected");
            reg1 = 0;
        }
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        }
        if (compareit("#", FALSE))
        {   emit(0x44 + reg1);
            getimmediate(FALSE);
        } elif (compareit("@.+", FALSE))
        {   emit(0x48 + reg1);
            getrelative(TRUE);
        } elif (compareit(".+", FALSE))
        {   emit(0x48 + reg1);
            getrelative(FALSE);
        } else
        {   reg2 = 4;
            if (reg1 == 0)
            {   reg2 = getreg_calm(0, TRUE);
                if (reg2 <= 3)
                {   emit(0x40 + reg2);
            }   }
            if (reg2 == 4)
            {   getabsolute_nonbranch_oldcalm((UBYTE) (0x4C + reg1));
    }   }   }
    elif (compareit("RR", TRUE) || compareit("RRC", TRUE) || compareit("SR", TRUE))
    {   skipblanks(TRUE);
        emit(0x50 + getreg_calm(2, TRUE));
    } elif (compareit("OR", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_calm(2, TRUE);
        if (reg1 == 4)
        {   skipblanks(TRUE);
            if (compareit("U,#", FALSE))
            {   emit(0x76);
                getimmediate(FALSE);
            } elif (compareit("L,#", FALSE))
            {   emit(0x77);
                getimmediate(FALSE);
            } else
            {   emit(0x76);
                asmerror("bad operand");
        }   }
        else
        {   if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            if (compareit("#", FALSE))
            {   emit(0x64 + reg1);
                getimmediate(FALSE);
            } elif (compareit("@.+", FALSE))
            {   emit(0x68 + reg1);
                getrelative(TRUE);
            } elif (compareit(".+", FALSE))
            {   emit(0x68 + reg1);
                getrelative(FALSE);
            } else
            {   reg2 = 4;
                if (reg1 == 0)
                {   reg2 = getreg_calm(2, TRUE);
                    if (reg2 <= 3)
                    {   emit(0xA0 + reg2);
                }   }
                if (reg2 == 4)
                {   getabsolute_nonbranch_oldcalm((UBYTE) (0x6C + reg1));
    }   }   }   }
    elif (compareit("BIC", TRUE))
    {   skipblanks(TRUE);
        if (compareit("U,#", FALSE))
        {   emit(0x74);
            getimmediate(FALSE);
        } elif (compareit("L,#", FALSE))
        {   emit(0x75);
            getimmediate(FALSE);
    }   }
    elif (compareit("ADD", TRUE) || compareit("ADDC", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_calm(2, TRUE);
        if (reg1 == 4)
        {   reg1 = 0;
            asmerror("register expected");
        }
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        }
        if (compareit("#", FALSE))
        {   emit(0x84 + reg1);
            getimmediate(FALSE);
        } elif (compareit("@.+", FALSE))
        {   emit(0x88 + reg1);
            getrelative(TRUE);
        } elif (compareit(".+", FALSE))
        {   emit(0x88 + reg1);
            getrelative(FALSE);
        } else
        {   reg2 = 4;
            if (reg1 == 0)
            {   reg2 = getreg_calm(2, TRUE);
                if (reg2 <= 3)
                {   emit(0x80 + reg2);
            }   }
            if (reg2 == 4)
            {   getabsolute_nonbranch_oldcalm((UBYTE) (0x8C + reg1));
    }   }   }
    elif (compareit("DA", TRUE))
    {   skipblanks(TRUE);
        emit(0x94 + getreg_calm(2, TRUE));
    } elif (compareit("SUB", TRUE) || compareit("SUBB", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_calm(2, TRUE);
        if (reg1 == 4)
        {   asmerror("register expected");
            reg1 = 0;
        }
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        }
        if (compareit("#", FALSE))
        {   emit(0xA4 + reg1);
            getimmediate(FALSE);
        } elif (compareit("@.+", FALSE))
        {   emit(0xA8 + reg1);
            getrelative(TRUE);
        } elif (compareit(".+", FALSE))
        {   emit(0xA8 + reg1);
            getrelative(FALSE);
        } else
        {   reg2 = 4;
            if (reg1 == 0)
            {   reg2 = getreg_calm(2, TRUE);
                if (reg2 <= 3)
                {   emit(0xA0 + reg2);
            }   }
            if (reg2 == 4)
            {   getabsolute_nonbranch_oldcalm((UBYTE) (0xAC + reg1));
    }   }   }
    elif (compareit("TEST", TRUE))
    {   skipblanks(TRUE);
        if   (compareit("IOF"      , TRUE)) { emit(0xB4); emit(0x20); }
        elif (compareit("OUTPUT"   , TRUE)) { emit(0xB4); emit(0x40); }
        elif (compareit("INPUT"    , TRUE)) { emit(0xB4); emit(0x80); }
        elif (compareit("CARRY"    , TRUE)) { emit(0xB5); emit(0x01); }
        elif (compareit("LOGICOMP" , TRUE)) { emit(0xB5); emit(0x02); }
        elif (compareit("OVERFLOW" , TRUE)) { emit(0xB5); emit(0x04); }
        elif (compareit("WITHCARRY", TRUE)) { emit(0xB5); emit(0x08); }
        elif (compareit("BANK", TRUE) || compareit("BANK1", TRUE))
                                            { emit(0xB5); emit(0x10); }
        elif (compareit("HALFCARRY", TRUE)) { emit(0xB5); emit(0x20); }
        elif (compareit("U,#", FALSE))
        {   emit(0xB4);
            getimmediate(FALSE);
        } elif (compareit("L,#", FALSE))
        {   emit(0xB5);
            getimmediate(FALSE);
        } else
        {   emit(0xF4 + getreg_calm(2, TRUE)); // $F4..F7
            if (!compareit(",#", FALSE))
            {   asmerror(",# expected");
            }
            getimmediate(FALSE);
    }   }
    elif (compareit("NOP", TRUE))
    {   emit(0xC0);
    } elif (compareit("RL", TRUE) || compareit("RLC", TRUE) || compareit("SL", TRUE) || compareit("ASL", TRUE))
    {   skipblanks(TRUE);
        emit(0xD0 + getreg_calm(2, TRUE));
    } elif (compareit("INCJ,NE", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_calm(2, TRUE);
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        }
        if (compareit("@.+", FALSE))
        {   emit(0xD8 + reg1);
            getrelative(TRUE);
        } elif (compareit(".+", FALSE))
        {   emit(0xD8 + reg1);
            getrelative(FALSE);
        } else
        {   emit(0xDC + reg1);
            getabsolute_branch();
    }   }
    elif (compareit("INC", TRUE))
    {   skipblanks(TRUE);
        emit(0xD8 + getreg_calm(2, TRUE));
        emit(0x00);
    } elif (compareit("COMP", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_calm(2, TRUE);
        if (reg1 == 4)
        {   asmerror("register expected");
            reg1 = 0;
        }
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        }
        if (compareit("#", FALSE))
        {   emit(0xE4 + reg1);
            getimmediate(FALSE);
        } elif (compareit("@.+", FALSE))
        {   emit(0xE8 + reg1);
            getrelative(TRUE);
        } elif (compareit(".+", FALSE))
        {   emit(0xE8 + reg1);
            getrelative(FALSE);
        } else
        {   reg2 = 4;
            if (reg1 == 0)
            {   reg2 = getreg_calm(2, TRUE);
                if (reg2 <= 3)
                {   emit(0xE0 + reg2);
            }   }
            if (reg2 == 4)
            {   getabsolute_nonbranch_oldcalm((UBYTE) (0xEC + reg1));
    }   }   }
    elif (compareit("DECJ,NE", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_calm(2, TRUE);
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        }
        if (compareit("@.+", FALSE))
        {   emit(0xF8 + reg1);
            getrelative(TRUE);
        } elif (compareit(".+", FALSE))
        {   emit(0xF8 + reg1);
            getrelative(FALSE);
        } else
        {   emit(0xFC + reg1);
            getabsolute_branch();
    }   }
    elif (compareit("DEC", TRUE))
    {   skipblanks(TRUE);
        emit(0xF8 + getreg_calm(2, TRUE));
        emit(0x00);
    }

    // Labels and Equates-------------------------------------------------

    elif (equstring[0] || linecursor != 0)
    {   asmerror("syntax error");
        return FALSE;
    } else
    {   i = 0;
        while
        (   LineBuffer[linecursor] != ' '
         && LineBuffer[linecursor] != HT
         && LineBuffer[linecursor] != EOS
         && LineBuffer[linecursor] != ':'
      // && LineBuffer[linecursor] != ',' (we don't allow this for CALM)
      // && LineBuffer[linecursor] != '>' (we don't allow this for CALM)
         && i <= LABELLIMIT
        )
        {   equstring[i++] = LineBuffer[linecursor++];
        }
        equstring[i] = EOS;
        if
        (   LineBuffer[linecursor] == ':' // label with colon
      // || LineBuffer[linecursor] == ',' // label with comma (X2650 cross-asm) (we don't allow this for CALM)
      // || LineBuffer[linecursor] == '>' // label with > (PIP4K source code) (we don't allow this for CALM)
        )
        {   linecursor++; // to get past it
        } elif (i > LABELLIMIT)
        {   asmerror("label too long");
            return FALSE;
        }

        skipblanks(FALSE);
        if (compareit(".EQU", TRUE)) // we don't allow EQU/SET/.SET for CALM
        {   skipblanks(TRUE);
            equmode = TRUE;
            value1 = parsenumber(&LineBuffer[linecursor]);
            equmode = FALSE;
            if (pass == 1)
            {   adduserlabel((STRPTR) equstring, (UWORD) value1, UNKNOWN);
        }   }
        else
        {   adduserlabel((STRPTR) equstring, (UWORD) ocursor, UNKNOWN);
            goto START;
    }   }

    return TRUE;
}

MODULE FLAG asm_calmdirective(void)
{   FAST UBYTE byte1st,
               byte2nd,
               byte3rd,
               byte4th,
               t;
    FAST int   i,
               length,
               value1,
               value2;

    // Directives (except Equates)----------------------------------------

    if (compareit(".ENDTEXT",    TRUE))
    {   commenting = FALSE;
    } elif (commenting || LineBuffer[0] == EOS)
    {   ;
    } elif
    (   compareit(".BASE",       TRUE)
     || compareit(".CHAP",       TRUE)
     || compareit(".ELSE",       TRUE)
     || compareit(".ENDIF",      TRUE)
     || compareit(".ENDLIST",    TRUE)
     || compareit(".ENDMACRO",   TRUE)
     || compareit(".ERROR",      TRUE)
     || compareit(".IF",         TRUE)
     || compareit(".IMPORT",     TRUE)
     || compareit(".INS",        TRUE)
     || compareit(".EXPORT",     TRUE)
     || compareit(".LAYOUT",     TRUE)
     || compareit(".LIST",       TRUE)
     || compareit(".LISTIF",     TRUE)
     || compareit(".LOCALMACRO", TRUE)
     || compareit(".MACRO",      TRUE)
     || compareit(".MESSAGE",    TRUE)
     || compareit(".PAGE",       TRUE)
     || compareit(".PROCSET",    TRUE)
     || compareit(".PROCVAL",    TRUE)
     || compareit(".RANGE",      TRUE)
     || compareit(".REF",        TRUE)
     || compareit(".SYSCALL.8",  TRUE)
     || compareit(".SYSCALL.16", TRUE)
     || compareit(".SYSCALL.32", TRUE)
     || compareit(".TITLE",      TRUE)
    )
    {   asmwarning("ignoring directive");
    } elif (compareit(".ALIGN", TRUE))
    {   skipblanks(TRUE);
        value1 = parsenumber(&LineBuffer[linecursor]);
        i = ocursor % value1;
        if (i)
        {   ocursor -= i;
            ocursor += value1;
    }   }
    elif (compareit(".ASCII", TRUE))
    {   skipblanks(TRUE);
        if (compareit("\"", FALSE))
        {   while (!compareit("\"", FALSE))
            {   emit(LineBuffer[linecursor++]);
        }   }
        else
        {   asmerror("\" expected");
    }   }
    elif (compareit(".ASCIZ", TRUE))
    {   skipblanks(TRUE);
        if (compareit("\"", FALSE))
        {   while (!compareit("\"", FALSE))
            {   emit(LineBuffer[linecursor++]);
            }
            emit(EOS);
        } else
        {   asmerror("\" expected");
    }   }
    elif (compareit(".ASCIZE", TRUE))
    {   skipblanks(TRUE);
        if (compareit("\"", FALSE))
        {   while (!compareit("\"", FALSE))
            {   emit(LineBuffer[linecursor++]);
            }
            emit(EOS);
            if (ocursor % 2 == 1)
            {   ocursor++;
        }   }
        else
        {   asmerror("\" expected");
    }   }
    elif (compareit(".BLK.8", TRUE))
    {   skipblanks(TRUE);
        ocursor += parsenumber(&LineBuffer[linecursor]);
    } elif (compareit(".BLK.16", TRUE))
    {   skipblanks(TRUE);
        ocursor += parsenumber(&LineBuffer[linecursor]) * 2;
    } elif (compareit(".BLK.32", TRUE))
    {   skipblanks(TRUE);
        ocursor += parsenumber(&LineBuffer[linecursor]) * 4;
    } elif
    (   compareit(".DATA", TRUE)
     || compareit(".DATA.8", TRUE)
     || compareit(".8", TRUE)
    )
    {   getimmediatebytes();
    } elif
    (   compareit(".DATA.16", TRUE)
     || compareit(".16", TRUE)
    )
    {   getimmediatewords(FALSE);
    } elif (compareit(".END", TRUE)) // .END <address> is unofficial for CALM
    {   if (!skipblanks(FALSE))
        {   value1 = parsenumber(&LineBuffer[linecursor]);
            startaddr_h = value1 / 256;
            startaddr_l = value1 % 256;
            specified = TRUE;
        }
        ended = TRUE;
        return TRUE;
    } elif (compareit(".EVEN", TRUE))
    {   if (ocursor % 2 == 1)
        {   ocursor++;
    }   }
    elif (compareit(".FILL.8", TRUE))
    {   skipblanks(TRUE);
        value1 = parsenumber(&LineBuffer[linecursor]);
        skipblanks(TRUE);
        i = fiximmediate(parsenumber(&LineBuffer[linecursor]));
        if (i < 0 || i > 255)
        {   asmerror("immediate value out of range");
        } else
        {   byte1st = i;
            for (i = 0; i < value1; i++)
            {   emit(byte1st);
    }   }   }
    elif (compareit(".FILL.16", TRUE))
    {   skipblanks(TRUE);
        value1 = parsenumber(&LineBuffer[linecursor]);
        skipblanks(TRUE);
        i = parsenumber(&LineBuffer[linecursor]);
        if (i >= -32768 && i <= -1)
        {   i += 65536; // -32768..-1 -> 32768..65535
        }
        if (i < 0 || i > 65535)
        {   asmerror("immediate value out of range");
        } else
        {   byte1st = i / 256;
            byte2nd = i % 256;
            for (i = 0; i < value1; i += 2)
            {   emit(byte1st);
                emit(byte2nd);
    }   }   }
    elif (compareit(".FILL.32", TRUE))
    {   skipblanks(TRUE);
        value1 = parsenumber(&LineBuffer[linecursor]);
        skipblanks(TRUE);
        i = parsenumber(&LineBuffer[linecursor]);
        byte1st =  i / 16777216;
        byte2nd = (i % 16777216) / 65536;
        byte3rd = (i %    65535) /   256;
        byte4th =  i %      256;
        for (i = 0; i < value1; i += 4)
        {   emit(byte1st);
            emit(byte2nd);
            emit(byte3rd);
            emit(byte4th);
    }   }
    elif (compareit(".INCLUDE", TRUE))
    {   if (pass == 2 && GraphBuffer[0] == EOS)
        {   sprintf(GraphBuffer, "$0000 %s\n", fn_asm);
        }
        if (!includefile())
        {   return FALSE;
        }
        if (pass == 2)
        {   length = strlen(GraphBuffer);
            graphnesting++;
            for (i = 0; i < graphnesting; i++)
            {   GraphBuffer[length + i] = ' ';
            }
            sprintf(&GraphBuffer[length + graphnesting], "$%04X %s\n", ocursor, fn_inc);
    }   }
    elif (compareit(".LOC", TRUE))
    {   skipblanks(TRUE);
        value1 = parsenumber(&LineBuffer[linecursor]);
        if (value1 < ocursor)
        {   asmwarning("setting origin backwards");
        }
        orgy = TRUE;
        ocursor = value1;
    } elif (compareit(".LOCF", TRUE))
    {   skipblanks(TRUE);
        value1 = parsenumber(&LineBuffer[linecursor]);
        if (value1 < ocursor)
        {   asmerror("invalid address");
        }
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        }
        value2 = fiximmediate(parsenumber(&LineBuffer[linecursor]));
        if (value2 < 0 || value2 > 255)
        {   asmerror("immediate value out of range");
            value1 = 0;
        }
        if (value1 > ocursor)
        {   t = (UBYTE) value2;
            for (i = ocursor; i < value1; i++)
            {   memory[i] = t;
        }   }
        orgy = TRUE;
        ocursor = value1;
    } elif (compareit(".ODD", TRUE))
    {   if (ocursor % 2 == 0)
        {   ocursor++;
    }   }
    elif (compareit(".PROC", TRUE))
    {   skipblanks(TRUE);
        if (!compareit("S2650", TRUE) || !compareit("S2650A", TRUE))
        {   if (supercpu)
            {   supercpu = FALSE;
                asmwarning("switching to 2650/2650A CPU");
                updatemenu(MENUFAKE_CPU);
        }   }
        elif (!compareit("S2650B", TRUE))
        {   if (!supercpu)
            {   supercpu = TRUE;
                asmwarning("switching to 2650B CPU");
                updatemenu(MENUFAKE_CPU);
        }   }
        else
        {   asmerror("unsupported CPU");
    }   }
    elif (compareit(".START", TRUE))
    {   skipblanks(TRUE);
        value1 = parsenumber(&LineBuffer[linecursor]);
        startaddr_h = value1 / 256;
        startaddr_l = value1 % 256;
        specified = TRUE;
    } elif (compareit(".STRING", TRUE))
    {   skipblanks(TRUE);
        if (compareit("\"", FALSE))
        {   value1 = 0;
            i = linecursor;
            while (!compareit("\"", FALSE))
            {   value1++;
            }
            linecursor = i;
            if (value1 >= 256)
            {   asmerror("string too long");
            } else
            {   emit((UBYTE) value1);
                while (!compareit("\"", FALSE))
                {   emit(LineBuffer[linecursor++]);
        }   }   }
        else
        {   asmerror("opening \" expected");
    }   }
    elif (compareit(".TEXT",    TRUE))
    {   commenting = TRUE;
    } else
    {   return FALSE;
    }

    return TRUE;
}

MODULE FLAG asm_newcalm(void)
{   FAST UBYTE byte1st,
               reg1, reg2,
               t;
    FAST int   i,
               value1;
 
    equstring[0] = EOS;

START:
    if (asm_calmdirective())
    {   return TRUE;
    }

    // Opcodes------------------------------------------------------------

    if (compareit("NOT", TRUE))                        // NOT reg -> XOR reg,$FF (EORI,reg $FF)
    {   skipblanks(TRUE);
        reg1 = getreg_signetics(2, TRUE);
        if (reg1 == 4)
        {   asmerror("register expected");
            reg1 = 0;
        }
        emit(0x24 + reg1);
        emit(0xFF);
    } elif (compareit("MOVE", TRUE))                   // LPSx/SPSx/REDx/WRTx/LDPL/STPL/LODx/STRx
    {   skipblanks(TRUE);
        if     (compareit("r0,PSU", TRUE))             // MOVE r0,PSU (LPSU)
        {   emit(0x92);
        } elif (compareit("r0,PSL", TRUE))             // MOVE r0,PSL (LPSL)
        {   emit(0x93);
        } elif (compareit("PSU,r0", TRUE))             // MOVE PSU,r0 (SPSU)
        {   emit(0x12);
        } elif (compareit("PSL,r0", TRUE))             // MOVE PSL,r0 (SPSL)
        {   emit(0x13);
        } elif (compareit("$CTRL,", FALSE))            // MOVE $CTRL,reg (REDC reg)
        {   reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            emit(0x30 + reg1);
        } elif (compareit("$DATA,", FALSE))            // MOVE $DATA,reg (REDD reg)
        {   reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            emit(0x70 + reg1);
        } elif (compareit("$", FALSE))                 // MOVE $port,reg (REDE,reg port)
        {   value1 = fiximmediate(parsenumber(&LineBuffer[linecursor]));
            if (value1 < 0 || value1 > 255)
            {   asmerror("immediate value out of range");
                value1 = 0;
            }
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            emit(0x54 + reg1);
            emit((UBYTE) value1);
        } elif
        (   compareit("PSL,U^", FALSE)
         || compareit("PSL,"  , FALSE)                 // MOVE PSL,U^abs (STPL abs)
        )
        {   emit(0x11);
            getabsolute_branch(); // yes, branch!
            if (!supercpu)
            {   asmerror("2650B only");
        }   }
        elif (compareit("R^", FALSE))                  // MOVE R^rel,reg (LODR,reg rel)
        {   byte1st = dorel(2, FALSE);
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            emit(0x08 + reg1);
            emit(byte1st);
        } elif (compareit("U^", FALSE))
        {   gopcode = 0x0C;
            if (ganb_newcalm(TRUE))
            {   if (!compareit(",r0", FALSE))
                {   asmerror(",r0 expected");
                }
                emit(gopcode);
                gbyte1st &= 0x1F;
            } else
            {   if (!compareit(",", FALSE))
                {   asmerror("comma expected");
                }
                if (!compareit("PSL", TRUE))           // MOVE U^abs,PSL (LDPL abs)
                {   emit(0x10);
                    if (!supercpu)
                    {   asmerror("2650B only");
                }   }                                  // MOVE U^abs,reg (LODA,reg abs)
                else
                {   reg1 = getreg_signetics(2, TRUE);
                    if (reg1 == 4)
                    {   if (supercpu)
                        {   asmerror("PSL or register expected");
                        } else
                        {   asmerror("register expected");
                        }
                        reg1 = 0;
                    }
                    emit(0x0C + reg1);
                    gbyte1st &= 0x1F;
                }
                emit(gbyte1st);
                emit(gbyte2nd);
        }   }
        elif (compareit("#", FALSE))                   // MOVE #imm,reg (LODI,reg imm)
        {   value1 = parsenumber(&LineBuffer[linecursor]);
            value1 = fiximmediate(value1);
            if (value1 < 0 || value1 > 255)
            {   asmerror("immediate value out of range");
                value1 = 0;
            }
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            emit(0x04 + reg1);
            emit(value1);
        } else
        {   reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("unexpected input");
            } else
            {   if (!compareit(",", FALSE))
                {   asmerror("comma expected");
                }
                if (compareit("R^", FALSE))                // MOVE reg,R^rel (STRR,reg rel)
                {   emit(0xC8 + reg1);
                    emit(dorel(1, FALSE));
                } elif (compareit("U^{", FALSE))           // MOVE reg,U^abs (STRA,reg abs)
                {   gopcode = 0xCC;
                    if (ganb_newcalm(FALSE))
                    {   if (!compareit(",r0", FALSE))
                        {   asmerror(",r0 expected");
                        }
                        emit(gopcode);
                    } else
                    {   if (!compareit(",", FALSE))
                        {   asmerror("comma expected");
                        }
                        reg1 = getreg_signetics(2, TRUE);
                        if (reg1 == 4)
                        {   asmerror("register expected");
                            reg1 = 0;
                        }
                        emit(0xCC + reg1);
                    }
                    emit(gbyte1st);
                    emit(gbyte2nd);
                } elif (compareit("$CTRL", TRUE))          // MOVE reg,$CTRL (WRTC,reg)
                {   emit(0xB0 + reg1);
                } elif (compareit("$DATA", TRUE))          // MOVE reg,$DATA (WRTD,reg)
                {   emit(0xF0 + reg1);
                } elif (compareit("$", FALSE))             // MOVE reg,$port (WRTE,reg port)
                {   emit(0xD4 + reg1);
                    getimmediate(FALSE);
                } else
                {   reg2 = getreg_calm(2, TRUE);
                    if (reg2 == 4)
                    {   asmerror("unexpected input");
                    } else
                    {   if (reg1 == 0 && reg2 == 0)        // MOVE r0,r0 (LODZ/STRZ)
                        {   emit(0x00);
                            asmwarning("indeterminate opcode");
                        } elif (reg1 == 0)
                        {   emit(0xC0 + reg2);             // MOVE r0,reg (STRZ reg)
                        } elif (reg2 == 0)
                        {   emit(0x00 + reg1);             // MOVE reg,r0 (LODZ reg)
                        } else
                        {   asmerror("one register must be r0");
    }   }   }   }   }   }
    elif (compareit("RET", TRUE))
    {   emit(0x17);
    } elif (compareit("RET,", FALSE))
    {   emit(0x14 + getcc_calm());
    } elif (compareit("JUMP,", FALSE))
    {   if (compareit("NE", TRUE)) // JUMP,NE
        {   if (compareit("R^", FALSE)) // JUMP,NE R^rel (BCFR,eq)
            {   emit(0x98);
                emit(dorel(1, FALSE));
            } elif (compareit("U^", FALSE)) // JUMP,NE U^rel (BCFA,eq)
            {   emit(0x9C);
                getabsolute_branch();
            } else
            {   reg1 = getreg_signetics(2, TRUE);
                if (reg1 == 4)
                {   asmerror("R^ or U^ or register expected");
                } else
                {   if (!compareit(",", FALSE))
                    {   asmerror("comma expected");
                    }
                    if (compareit("R^", FALSE)) // JUMP,NE reg,R^rel (BRNR,reg rel)
                    {   emit(0x58 + reg1);
                        emit(dorel(1, FALSE));
                    } elif (compareit("U^", FALSE)) // JUMP,NE reg,U^abs (BRNA,reg abs)
                    {   emit(0x5C + reg1);
                        getabsolute_branch();
                    } else
                    {   asmerror("R^ or U^ expected");
        }   }   }   }
        elif (compareit("EQ", TRUE)) // JUMP,EQ
        {   if (compareit("R^", FALSE)) // JUMP,EQ R^rel (BCTR,eq rel)
            {   emit(0x18);
                emit(dorel(1, FALSE));
            } elif (compareit("U^", FALSE)) // JUMP,EQ U^abs (BCTA,eq abs)
            {   emit(0x1C);
                getabsolute_branch();
            } else
            {   asmerror("R^ or U^ expected");
        }   }
        elif (compareit("GT", TRUE)) // JUMP,GT
        {   if (compareit("R^", FALSE)) // JUMP,GT R^rel (BCTR,gt rel)
            {   emit(0x19);
                emit(dorel(1, FALSE));
            } elif (compareit("U^", FALSE)) // JUMP,GT U^abs (BCTA,gt abs)
            {   emit(0x1D);
                getabsolute_branch();
            } else
            {   asmerror("R^ or U^ expected");
        }   }
        elif (compareit("LT", TRUE)) // JUMP,LT
        {   if (compareit("R^", FALSE)) // JUMP,LT R^rel (BCTR,eq rel)
            {   emit(0x1A);
                emit(dorel(1, FALSE));
            } elif (compareit("U^", FALSE)) // JUMP,LT U^abs (BCTA,lt abs)
            {   emit(0x1E);
                getabsolute_branch();
            } else
            {   asmerror("R^ or U^ expected");
        }   }
        elif (compareit("LE", TRUE))
        {   if (compareit("R^", FALSE)) // JUMP,LE R^rel (BCFR,gt rel)
            {   emit(0x99);
                emit(dorel(1, FALSE));
            } elif (compareit("U^", FALSE)) // JUMP,LE U^abs (BCFA,gt abs)
            {   emit(0x9D);
                getabsolute_branch();
            } else
            {   asmerror("R^ or U^ expected");
        }   }
        elif (compareit("GE", TRUE))
        {   if (compareit("R^", FALSE)) // JUMP,GE R^rel (BCFR,lt rel)
            {   emit(0x9A);
                emit(dorel(1, FALSE));
            } elif (compareit("U^", FALSE)) // JUMP,GE U^abs (BCFA,lt abs)
            {   emit(0x9E);
                getabsolute_branch();
            } else
            {   asmerror("R^ or U^ expected");
    }   }   }
    elif (compareit("JUMP", TRUE))
    {   skipblanks(TRUE);
        if (compareit("U^{r3}+", FALSE) || compareit("U^{r6}+", FALSE)) // JUMP U^{r3}+abs (BXA,r3 abs)
        {   DISCARD compareit("U^", FALSE); // optional
            emit(0x9F);
            getabsolute_branch();
        } elif (compareit("0+", FALSE)) // JUMP 0+zero (ZBRR zero)
        {   emit(0x9B);
            getzeropage(FALSE);
        } elif (compareit("R^", FALSE)) // JUMP R^rel (BCTR,un rel)
        {   emit(0x1B);
            getrelative(FALSE);
        } elif (compareit("U^", FALSE)) // JUMP U^abs (BCTA,un abs)
        {   emit(0x1F);
            getabsolute_branch();
        } else
        {   asmerror("0+ or R^ or U^ expected");
    }   }
    elif (compareit("CLR", TRUE))
    {   skipblanks(TRUE);
        if   (compareit("r0"       , TRUE))   emit(0x20);               // CLR r0 is same as XOR r0,r0
        elif (compareit("SP"       , TRUE)) { emit(0x74); emit(0x07); }
        elif (compareit("IOF"      , TRUE)) { emit(0x74); emit(0x20); } // CLR IOF is same as ION
        elif (compareit("OUTPUT"   , TRUE)) { emit(0x74); emit(0x40); }
        elif (compareit("INPUT"    , TRUE)) { emit(0x74); emit(0x80); }
        elif (compareit("CARRY"    , TRUE)) { emit(0x75); emit(0x01); } // CLR CARRY is same as CLRC
        elif (compareit("LOGICOMP" , TRUE)) { emit(0x75); emit(0x02); }
        elif (compareit("OVERFLOW" , TRUE)) { emit(0x75); emit(0x04); } // CLR OVERFLOW is same as CLRV
        elif (compareit("WITHCARRY", TRUE)) { emit(0x75); emit(0x08); }
        elif (compareit("BANK", TRUE) || compareit("BANK1", TRUE))
                                            { emit(0x75); emit(0x10); }
        elif (compareit("HALFCARRY", TRUE)) { emit(0x75); emit(0x20); }
        else                                { emit(0x75); emit(0x00);
                                              asmerror("bad operand"); }
    } elif (compareit("CLRC", TRUE)) // CLRC is same as CLR CARRY
    {   emit(0x75);
        emit(0x01);
    } elif (compareit("CLRV", TRUE)) // CLRV is same as CLR OVERFLOW
    {   emit(0x75);
        emit(0x04);
    } elif (compareit("ION", TRUE)) // ION is same as CLR IOF
    {   emit(0x74);
        emit(0x20);
    } elif (compareit("SET", TRUE))
    {   skipblanks(TRUE);
        if   (compareit("IOF"      , TRUE)) { emit(0x76); emit(0x20); } // SET IOF is same as IOF
        elif (compareit("OUTPUT"   , TRUE)) { emit(0x76); emit(0x40); }
        elif (compareit("INPUT"    , TRUE)) { emit(0x76); emit(0x80); }
        elif (compareit("CARRY"    , TRUE)) { emit(0x77); emit(0x01); } // SET CARRY is same as SETC
        elif (compareit("LOGICOMP" , TRUE)) { emit(0x77); emit(0x02); }
        elif (compareit("OVERFLOW" , TRUE)) { emit(0x77); emit(0x04); } // SET OVERFLOW is same as SETV
        elif (compareit("WITHCARRY", TRUE)) { emit(0x77); emit(0x08); }
        elif (compareit("BANK"     , TRUE) || compareit("BANK1", TRUE))
                                            { emit(0x77); emit(0x10); }
        elif (compareit("HALFCARRY", TRUE)) { emit(0x77); emit(0x20); }
        else                                { emit(0x76); emit(0x00);
                                              asmerror("bad operand"); }
    } elif (compareit("SETC", TRUE)) // SETC is same as SET CARRY
    {   emit(0x77);
        emit(0x01);
    } elif (compareit("SETV", TRUE)) // SETV is same as SET OVERFLOW
    {   emit(0x77);
        emit(0x04);
    } elif (compareit("IOF", TRUE)) // IOF is same as SET IOF
    {   emit(0x76);
        emit(0x20);
    } elif (compareit("XOR", TRUE))
    {   skipblanks(TRUE);
        if (compareit("#", FALSE)) // XOR #imm,reg (EORI,reg imm)
        {   byte1st = saveimmediate(FALSE);
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            emit(0x24 + reg1);
            emit(byte1st);
        } elif (compareit("R^", FALSE)) // XOR R^rel,reg (EORR,reg rel)
        {   byte1st = dorel(2, FALSE);
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            emit(0x28 + reg1);
            emit(byte1st);
        } elif (compareit("U^", FALSE)) // XOR U^abs,reg (EORA,reg abs)
        {   gopcode = 0x2C;
            if (ganb_newcalm(FALSE))
            {   if (!compareit(",r0", FALSE))
                {   asmerror(",r0 expected");
                }
                emit(gopcode);
            } else
            {   if (!compareit(",", FALSE))
                {   asmerror("comma expected");
                }
                reg1 = getreg_signetics(2, TRUE);
                if (reg1 == 4)
                {   asmerror("register expected");
                    reg1 = 0;
                }
                emit(0x2C + reg1);
            }
            emit(gbyte1st);
            emit(gbyte2nd);
        } else // XOR reg,r0 (EORZ reg)
        {   reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg2 = getreg_signetics(2, TRUE);
            if (reg2 != 0)
            {   asmerror("r0 expected");
            }
            emit(0x20 + reg1);
    }   }
    elif (compareit("RETI", TRUE))
    {   emit(0x37);
    } elif (compareit("RETI,", FALSE))
    {   emit(0x34 + getcc_calm());
    } elif (compareit("CALL,", FALSE))
    {   if (compareit("NE", TRUE)) // CALL,NE
        {   if (compareit("R^", FALSE)) // CALL,NE R^rel (BSFR,eq)
            {   emit(0xB8);
                emit(dorel(1, FALSE));
            } elif (compareit("U^", FALSE)) // CALL,NE U^rel (BSFA,eq)
            {   emit(0xBC);
                getabsolute_branch();
            } else
            {   reg1 = getreg_signetics(2, TRUE);
                if (reg1 == 4)
                {   asmerror("R^ or U^ or register expected");
                } else
                {   if (!compareit(",", FALSE))
                    {   asmerror("comma expected");
                    }
                    if (compareit("R^", FALSE)) // CALL,NE reg,R^rel (BSNR,reg rel)
                    {   emit(0x78 + reg1);
                        emit(dorel(1, FALSE));
                    } elif (compareit("U^", FALSE)) // CALL,NE reg,U^abs (BSNA,reg abs)
                    {   emit(0x7C + reg1);
                        getabsolute_branch();
                    } else
                    {   asmerror("R^ or U^ expected");
        }   }   }   }
        elif (compareit("EQ", TRUE)) // CALL,EQ
        {   if (compareit("R^", FALSE)) // CALL,EQ R^rel (BSTR,eq rel)
            {   emit(0x38);
                emit(dorel(1, FALSE));
            } elif (compareit("U^", FALSE)) // CALL,EQ U^abs (BSTA,eq abs)
            {   emit(0x3C);
                getabsolute_branch();
            } else
            {   asmerror("R^ or U^ expected");
        }   }
        elif (compareit("GT", TRUE)) // CALL,GT
        {   if (compareit("R^", FALSE)) // CALL,GT R^rel (BSTR,gt rel)
            {   emit(0x39);
                emit(dorel(1, FALSE));
            } elif (compareit("U^", FALSE)) // CALL,GT U^abs (BSTA,gt abs)
            {   emit(0x3D);
                getabsolute_branch();
            } else
            {   asmerror("R^ or U^ expected");
        }   }
        elif (compareit("LT", TRUE)) // CALL,LT
        {   if (compareit("R^", FALSE)) // CALL,LT R^rel (BSTR,eq rel)
            {   emit(0x3A);
                emit(dorel(1, FALSE));
            } elif (compareit("U^", FALSE)) // CALL,LT U^abs (BSTA,lt abs)
            {   emit(0x3E);
                getabsolute_branch();
            } else
            {   asmerror("R^ or U^ expected");
        }   }
        elif (compareit("LE", TRUE))
        {   if (compareit("R^", FALSE)) // CALL,LE R^rel (BSFR,gt rel)
            {   emit(0xB9);
                emit(dorel(1, FALSE));
            } elif (compareit("U^", FALSE)) // CALL,LE U^abs (BSFA,gt abs)
            {   emit(0xBD);
                getabsolute_branch();
            } else
            {   asmerror("R^ or U^ expected");
        }   }
        elif (compareit("GE", TRUE))
        {   if (compareit("R^", FALSE)) // CALL,GE R^rel (BSFR,lt rel)
            {   emit(0xBA);
                emit(dorel(1, FALSE));
            } elif (compareit("U^", FALSE)) // CALL,GE U^abs (BSFA,lt abs)
            {   emit(0xBE);
                getabsolute_branch();
            } else
            {   asmerror("R^ or U^ expected");
    }   }   }
    elif (compareit("CALL", TRUE))
    {   skipblanks(TRUE);
        if (compareit("U^{r3}+", FALSE) || compareit("U^{r6}+", FALSE)) // CALL U^{r3}+abs (BSXA,r3 abs)
        {   DISCARD compareit("U^", FALSE); // optional
            emit(0xBF);
            getabsolute_branch();
        } elif (compareit("0+", FALSE)) // CALL 0+zero (ZBSR zero)
        {   emit(0xBB);
            getzeropage(FALSE);
        } elif (compareit("R^", FALSE)) // CALL R^rel (BSTR,un rel)
        {   emit(0x3B);
            getrelative(FALSE);
        } elif (compareit("U^", FALSE)) // CALL U^abs (BSTA,un abs)
        {   emit(0x3F);
            getabsolute_branch();
        } else
        {   asmerror("0+ or R^ or U^ expected");
    }   }
    elif (compareit("WAIT", TRUE) || compareit("HALT", TRUE))
    {   emit(0x40);
    } elif (compareit("AND", TRUE))
    {   skipblanks(TRUE);
        if (compareit("#", FALSE)) // AND #imm,reg
        {   byte1st = saveimmediate(FALSE);
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            emit(0x44 + reg1);
            emit(byte1st);
        } elif (compareit("R^", FALSE)) // AND R^rel,reg
        {   byte1st = dorel(2, FALSE);
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            emit(0x48 + reg1);
            emit(byte1st);
        } elif (compareit("U^", FALSE)) // AND U^abs,reg (ANDA)
        {   gopcode = 0x4C;
            if (ganb_newcalm(FALSE))
            {   if (!compareit(",r0", FALSE))
                {   asmerror(",r0 expected");
                }
                emit(gopcode);
            } else
            {   if (!compareit(",", FALSE))
                {   asmerror("comma expected");
                }
                reg1 = getreg_signetics(2, TRUE);
                if (reg1 == 4)
                {   asmerror("register expected");
                    reg1 = 0;
                }
                emit(0x4C + reg1);
            }
            emit(gbyte1st);
            emit(gbyte2nd);
        } else // AND reg,r0
        {   reg1 = getreg_signetics(2, TRUE);
            if (reg1 < 1 || reg1 > 3)
            {   asmerror("r1 or r2 or r3 expected");
                reg1 = 1;
            }
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg2 = getreg_signetics(2, TRUE);
            if (reg2 != 0)
            {   asmerror("r0 expected");
                reg1 = 1;
            }
            emit(0x40 + reg1);
    }   }
    elif (compareit("RR", TRUE) || compareit("RRC", TRUE) || compareit("SR", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_signetics(2, TRUE);
        if (reg1 == 4)
        {   asmerror("register expected");
            reg1 = 0;
        }
        emit(0x50 + reg1);
    } elif (compareit("OR", TRUE))
    {   skipblanks(TRUE);
        if (compareit("#", FALSE)) // OR #imm,reg (IORI,reg imm)
        {   byte1st = saveimmediate(FALSE);
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            emit(0x64 + reg1);
            emit(byte1st);
        } elif (compareit("R^", FALSE)) // OR R^rel,reg (IORR,reg rel)
        {   byte1st = dorel(2, FALSE);
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            emit(0x68 + reg1);
            emit(byte1st);
        } elif (compareit("U^", FALSE)) // OR U^abs,reg (IORA,reg abs)
        {   gopcode = 0x6C;
            if (ganb_newcalm(FALSE))
            {   if (!compareit(",r0", FALSE))
                {   asmerror(",r0 expected");
                }
                emit(gopcode);
            } else
            {   if (!compareit(",", FALSE))
                {   asmerror("comma expected");
                }
                if (reg1 == 4)
                {   asmerror("register expected");
                    reg1 = 0;
                }
                emit(0x6C + reg1);
            }
            emit(gbyte1st);
            emit(gbyte2nd);
        } else // OR reg,r0 (IORZ reg)
        {   reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg2 = getreg_signetics(2, TRUE);
            if (reg2 != 0)
            {   asmerror("r0 expected");
            }
            emit(0x60 + reg1);
    }   }
    elif (compareit("BIC", TRUE))
    {   skipblanks(TRUE);
        if (compareit("#", FALSE))
        {   asmerror("# expected");
        }
        byte1st = saveimmediate(FALSE);
        if (compareit("comma", FALSE))
        {   asmerror("comma expected");
        }
        if (compareit("PSU", TRUE))
        {   emit(0x74);
            emit(byte1st);
        } elif (compareit("PSL", TRUE))
        {   emit(0x75);
            emit(byte1st);
        } else
        {   asmerror("PSU or PSL expected");
            emit(0x00);
            emit(byte1st);
    }   }
    elif (compareit("ADD", TRUE) || compareit("ADDC", TRUE))
    {   skipblanks(TRUE);
        if (compareit("#", FALSE)) // ADD #imm,reg (ADDI)
        {   byte1st = saveimmediate(FALSE);
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            emit(0x84 + reg1);
            emit(byte1st);
        } elif (compareit("R^", FALSE)) // ADD R^rel,reg (ADDR)
        {   byte1st = dorel(2, FALSE);
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            emit(0x88 + reg1);
            emit(byte1st);
        } elif (compareit("U^", FALSE)) // ADD U^abs,reg (ADDA)
        {   gopcode = 0x8C;
            if (ganb_newcalm(FALSE))
            {   if (!compareit(",r0", FALSE))
                {   asmerror(",r0 expected");
                }
                emit(gopcode);
            } else
            {   if (!compareit(",", FALSE))
                {   asmerror("comma expected");
                }
                reg1 = getreg_signetics(2, TRUE);
                if (reg1 == 4)
                {   asmerror("register expected");
                    reg1 = 0;
                }
                emit(0x8C + reg1);
            }
            emit(gbyte1st);
            emit(gbyte2nd);
        } else // ADD reg,r0 (ADDZ)
        {   reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg2 = getreg_signetics(2, TRUE);
            if (reg2 != 0)
            {   asmerror("r0 expected");
            }
            emit(0x80 + reg1);
    }   }
    elif (compareit("DAA", TRUE) || compareit("DAR", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_signetics(2, TRUE);
        if (reg1 == 4)
        {   asmerror("register expected");
            reg1 = 0;
        }
        emit(0x94 + reg1);
    } elif (compareit("SUB", TRUE) || compareit("SUBC", TRUE))
    {   skipblanks(TRUE);
        if (compareit("#", FALSE)) // SUB #imm,reg (SUBI)
        {   t = saveimmediate(FALSE);
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            emit(0xA4 + reg1);
            emit(t);
        } elif (compareit("R^", FALSE)) // SUB R^rel,reg (SUBR)
        {   byte1st = dorel(2, FALSE);
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            emit(0xA8 + reg1);
            emit(byte1st);
        } elif (compareit("U^", FALSE)) // SUB U^abs,reg (SUBA)
        {   gopcode = 0xAC;
            if (ganb_newcalm(FALSE))
            {   if (!compareit(",r0", FALSE))
                {   asmerror(",r0 expected");
                }
                emit(gopcode);
            } else
            {   if (!compareit(",", FALSE))
                {   asmerror("comma expected");
                }
                reg1 = getreg_signetics(2, TRUE);
                if (reg1 == 4)
                {   asmerror("register expected");
                    reg1 = 0;
                }
                emit(0xAC + reg1);
            }
            emit(gbyte1st);
            emit(gbyte2nd);
        } else // SUB reg,r0 (SUBZ)
        {   reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("register expected");
                reg1 = 0;
            }
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg2 = getreg_signetics(2, TRUE);
            if (reg2 != 0)
            {   asmerror("r0 expected");
            }
            emit(0xA0 + reg1);
    }   }
    elif (compareit("TEST", TRUE))
    {   skipblanks(TRUE);
        if   (compareit("IOF"      , TRUE)) { emit(0xB4); emit(0x20); }
        elif (compareit("OUTPUT"   , TRUE)) { emit(0xB4); emit(0x40); }
        elif (compareit("INPUT"    , TRUE)) { emit(0xB4); emit(0x80); }
        elif (compareit("CARRY"    , TRUE)) { emit(0xB5); emit(0x01); }
        elif (compareit("LOGICOMP" , TRUE)) { emit(0xB5); emit(0x02); }
        elif (compareit("OVERFLOW" , TRUE)) { emit(0xB5); emit(0x04); }
        elif (compareit("WITHCARRY", TRUE)) { emit(0xB5); emit(0x08); }
        elif (compareit("BANK", TRUE) || compareit("BANK1", TRUE))
                                            { emit(0xB5); emit(0x10); }
        elif (compareit("HALFCARRY", TRUE)) { emit(0xB5); emit(0x20); }
        elif (compareit("PSU:#", FALSE) || compareit("PSU,#", FALSE))
        {   emit(0xB4);
            getimmediate(FALSE);
        } elif (compareit("PSL:#", FALSE) || compareit("PSL,#", FALSE))
        {   emit(0xB5);
            getimmediate(FALSE);
        } else
        {   reg1 = getreg_signetics(2, TRUE);
            if (reg1 == 4)
            {   asmerror("PSU:# or PSL:# or register expected");
                reg1 = 0;
            }
            emit(0xF4 + reg1);
            if (!compareit(":#", FALSE) && !compareit(",#", FALSE))
            {   asmerror(":# expected");
            }
            getimmediate(FALSE);
    }   }
    elif (compareit("NOP", TRUE))
    {   emit(0xC0);
    } elif (compareit("RL", TRUE) || compareit("RLC", TRUE) || compareit("SL", TRUE) || compareit("ASL", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_signetics(2, TRUE);
        if (reg1 == 4)
        {   asmerror("register expected");
            reg1 = 0;
        }
        emit(0xD0 + reg1);
    } elif (compareit("IJ,NE", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_signetics(2, TRUE);
        if (reg1 == 4)
        {   asmerror("register expected");
            reg1 = 0;
        }
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        }
        if (compareit("R^", FALSE)) // IJ,NE reg,R^rel (BIRR,reg rel)
        {   emit(0xD8 + reg1);
            emit(dorel(1, FALSE));
        } elif (compareit("U^", FALSE))
        {   emit(0xDC + reg1);
            getabsolute_branch();
        } else
        {   asmerror("R^ or U^ expected");
    }   }
    elif (compareit("INC", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_signetics(2, TRUE);
        if (reg1 == 4)
        {   asmerror("register expected");
            reg1 = 0;
        }
        emit(0xD8 + reg1);
        emit(0x00);
    } elif (compareit("COMP", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_calm(2, TRUE);
        if (reg1 == 4)
        {   asmerror("register expected");
            reg1 = 0;
        }
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        }
        if (compareit("#", FALSE))
        {   emit(0xE4 + reg1);
            getimmediate(FALSE);
        } elif (compareit("@.+", FALSE))
        {   emit(0xE8 + reg1);
            getrelative(TRUE);
        } elif (compareit(".+", FALSE))
        {   emit(0xE8 + reg1);
            getrelative(FALSE);
        } else
        {   reg2 = 4;
            if (reg1 == 0)
            {   reg2 = getreg_calm(2, TRUE);
                if (reg2 <= 3)
                {   emit(0xE0 + reg2);
            }   }
            if (reg2 == 4)
            {   getabsolute_nonbranch_oldcalm((UBYTE) (0xEC + reg1));
    }   }   }
    elif (compareit("DJ,NE", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_signetics(2, TRUE);
        if (reg1 == 4)
        {   asmerror("register expected");
            reg1 = 0;
        }
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        }
        if (compareit("R^", FALSE)) // IJ,NE reg,R^rel (BIRR,reg rel)
        {   emit(0xF8 + reg1);
            emit(dorel(1, FALSE));
        } elif (compareit("U^", FALSE))
        {   emit(0xFC + reg1);
            getabsolute_branch();
        } else
        {   asmerror("R^ or U^ expected");
    }   }
    elif (compareit("DEC", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_signetics(2, TRUE);
        if (reg1 == 4)
        {   asmerror("register expected");
            reg1 = 0;
        }
        emit(0xF8 + reg1);
        emit(0x00);
    }

    // Labels and Equates-------------------------------------------------

    elif (equstring[0] || linecursor != 0)
    {   asmerror("syntax error");
        return FALSE;
    } else
    {   i = 0;
        while
        (   LineBuffer[linecursor] != ' '
         && LineBuffer[linecursor] != HT
         && LineBuffer[linecursor] != EOS
         && LineBuffer[linecursor] != ':'
      // && LineBuffer[linecursor] != ',' (we don't allow this for CALM)
      // && LineBuffer[linecursor] != '>' (we don't allow this for CALM)
         && i <= LABELLIMIT
        )
        {   equstring[i++] = LineBuffer[linecursor++];
        }
        equstring[i] = EOS;
        if
        (   LineBuffer[linecursor] == ':' // label with colon
      // || LineBuffer[linecursor] == ',' // label with comma (X2650 cross-asm) (we don't allow this for CALM)
      // || LineBuffer[linecursor] == '>' // label with > (PIP4K source code) (we don't allow this for CALM)
        )
        {   linecursor++; // to get past it
        } elif (i > LABELLIMIT)
        {   asmerror("label too long");
            return FALSE;
        }

        skipblanks(FALSE);
        if (compareit(".EQU", TRUE)) // we don't allow EQU/SET/.SET for CALM
        {   skipblanks(TRUE);
            equmode = TRUE;
            value1 = parsenumber(&LineBuffer[linecursor]);
            equmode = FALSE;
            if (pass == 1)
            {   adduserlabel((STRPTR) equstring, (UWORD) value1, UNKNOWN);
        }   }
        else
        {   adduserlabel((STRPTR) equstring, (UWORD) ocursor, UNKNOWN);
            goto START;
    }   }

    return TRUE;
}

MODULE FLAG skipblanks(FLAG mandatory)
{   FLAG found = FALSE; // whether non-ws was found immediately

    if (LineBuffer[linecursor] != ' ' && LineBuffer[linecursor] != HT)
    {   if (mandatory)
        {   asmerror("whitespace expected");
        } else
        {   found = TRUE;
    }   }

    while
    (   LineBuffer[linecursor] == ' '
     || LineBuffer[linecursor] == HT
    )
    {   linecursor++;
    }

    return found;
}

MODULE UBYTE getcc_signetics(FLAG unallowed)
{   skipblanks(FALSE); // so eg. "RETC, UN" works
    if     (compareit("eq", TRUE) || compareit("0", TRUE) || compareit("z", TRUE) || compareit("a1", TRUE)) // a1 = "all 1s"  (eg. TPSU then BCTR,a1) (used by Derek Andrews assembler)
    {   return 0;                                                                                           // "BCFR,0" etc. are used in uP-Programmierfibel book
    } elif (compareit("gt", TRUE) || compareit("1", TRUE) || compareit("p", TRUE)                         )
    {   return 1;
    } elif (compareit("lt", TRUE) || compareit("2", TRUE) || compareit("n", TRUE) || compareit("s0", TRUE)) // s0 = "some 0s" (eg. TPSU then BCTR,s0) (used by Derek Andrews assembler)
    {   return 2;
    } elif (compareit("un", TRUE) || compareit("3", TRUE)                                                 )
    {   if (!unallowed)
        {   asmerror("un is not allowed for this opcode");
            return 4;
        }
        return 3;
    } else
    {   asmerror("condition code expected");
        return 4;
}   }
MODULE UBYTE getcc_calm(void)
{   skipblanks(TRUE);
    if (compareit("EQ", TRUE) || compareit("AO", TRUE))
    {   return 0;
    } elif (compareit("GT", TRUE))
    {   return 1;
    } elif (compareit("LT", TRUE) || compareit("NO", TRUE))
    {   return 2;
    } else
    {   asmerror("condition code expected");
        return 4;
}   }

MODULE UBYTE getreg_signetics(int r0allowed, FLAG r1r2allowed)
{   /* For r0allowed:
    0 means not allowed at all
    1 means to emit a warning but allow it (used for LODZ r0)
    2 means fully allowed */

    skipblanks(FALSE); // so eg. "LODI, r0" works

    if (compareit("r0", TRUE) || compareit("0", TRUE)) // "BDRR,0" etc. are used in uP-Programmierfibel book
    {   if (r0allowed == 0)
        {   asmerror("r0 is not allowed for this opcode");
            return 4;
        } elif (r0allowed == 1)
        {   asmwarning("r0 is not allowed for this opcode");
        }
        return 0;
    } elif (compareit("r1", TRUE) || compareit("r4", TRUE) || compareit("1", TRUE) || compareit("4", TRUE))
    {   if (!r1r2allowed)
        {   asmerror("r1 is not allowed for this opcode");
            return 4;
        }
        return 1;
    } elif (compareit("r2", TRUE) || compareit("r5", TRUE) || compareit("2", TRUE) || compareit("5", TRUE))
    {   if (!r1r2allowed)
        {   asmerror("r2 is not allowed for this opcode");
            return 4;
        }
        return 2;
    } elif (compareit("r3", TRUE) || compareit("r6", TRUE) || compareit("3", TRUE) || compareit("6", TRUE))
    {   return 3;
    } elif (style == STYLE_NEWCALM)
    {   return 4;
    } else
    {   asmerror("register expected");
        return 0;
}   }
MODULE UBYTE getreg_calm(int r0allowed, FLAG r1r2allowed)
{   /* For r0allowed:
    0 means not allowed at all
    1 means to emit a warning but allow it (used for LODZ r0)
    2 means fully allowed */

    if (compareit("A",  TRUE))
    {   if (r0allowed == 0)
        {   asmerror("A is not allowed for this opcode");
            return 4;
        } elif (r0allowed == 1)
        {   asmwarning("A is not allowed for this opcode");
        }
        return 0;
    } elif (compareit("B", TRUE) || compareit("B'", TRUE))
    {   if (!r1r2allowed)
        {   asmerror("B is not allowed for this opcode");
            return 4;
        }
        return 1;
    } elif (compareit("C", TRUE) || compareit("C'", TRUE))
    {   if (!r1r2allowed)
        {   asmerror("C is not allowed for this opcode");
            return 4;
        }
        return 2;
    } elif (compareit("D", TRUE) || compareit("D'", TRUE))
    {   return 3;
    } else
    {   // asmerror("register expected");
        return 4;
}   }
MODULE UBYTE getreg_ieee(void)
{   if (compareit(".0", TRUE)                         ) return 0;
    if (compareit(".1", TRUE) || compareit(".4", TRUE)) return 1;
    if (compareit(".2", TRUE) || compareit(".5", TRUE)) return 2;
    if (compareit(".3", TRUE) || compareit(".6", TRUE)) return 3;

    asmerror("register expected");
    return 4;
}

MODULE void getabsolute_nonbranch_signetics(UBYTE firstopcode, int reg)
{   int   value;
    UBYTE byte1st,
          byte2nd,
          oldreg;

    if (reg == -1)
    {   reg = getreg_signetics(2, TRUE);
    }
    emit(firstopcode + reg); // must do this so that ocursor is correct for parsenumber(&LineBuffer[linecursor])
    skipblanks(TRUE);

    if (compareit("*", FALSE))
    {   byte1st = 0x80;
    } else
    {   byte1st = 0;
    }
    value = parsenumber(&LineBuffer[linecursor]) & 0x1FFF;

    if (compareit(",", FALSE))
    {   if     (compareit("+", TRUE ) || compareit(",+", TRUE )) // so eg. LODA,r1 $1234,+ works (means LODA,r0 $1234,r1+)
        {   byte1st |= 0x20;
        } elif (compareit("-", TRUE ) || compareit(",-", TRUE ))
        {   byte1st |= 0x40;
        } else
        {   oldreg = reg;
            reg = getreg_signetics(2, TRUE);
            if (oldreg != 0)
            {   asmwarning("r0 is required as first register");
            }
            if     (compareit("+", FALSE) || compareit(",+", FALSE)) // eg. LODA,r0 $1234,r1+
            {   byte1st |= 0x20;
            } elif (compareit("-", FALSE) || compareit(",-", FALSE))
            {   byte1st |= 0x40;
            } else
            {   byte1st |= 0x60;
    }   }   }

    byte1st |= (value / 256);
    byte2nd =  (value % 256);
    ocursor--;
    emitpos--;
    emit(firstopcode + reg);
    emit(byte1st);
    emit(byte2nd);
}

MODULE void getabsolute_branch(void)
{   int   value;
    UBYTE byte1st,
          byte2nd;

    if (style != STYLE_OLDCALM && style != STYLE_NEWCALM && style != STYLE_IEEE)
    {   skipblanks(TRUE);
    }

    if (compareit(indchars[style], FALSE))
    {   byte1st = 0x80;
        value = parsenumber(&LineBuffer[linecursor]);
        if (style == STYLE_NEWCALM && !compareit("}", FALSE))
        {   asmerror("} expected");
    }   }
    else
    {   byte1st = 0;
        value = parsenumber(&LineBuffer[linecursor]);
    }

    if (compareit(",", FALSE))
    {   asmerror("unexpected comma");
    }

    byte1st |= (value / 256);
    byte2nd =  (value % 256);
    emit(byte1st);
    emit(byte2nd);
}

MODULE void getabsolute_indexed_signetics(UBYTE firstopcode) // "BXA $1234,r3"
{   int   value;
    UBYTE byte1st,
          byte2nd;

    // assert(style != STYLE_CALM);

    emit(firstopcode); // must do this so that ocursor is correct for parsenumber(&LineBuffer[linecursor])
    skipblanks(TRUE);

    if (compareit("*", FALSE))
    {   byte1st = 0x80;
    } else
    {   byte1st = 0;
    }
    value = parsenumber(&LineBuffer[linecursor]) & 0x7FFF;

    if (compareit(",", FALSE))
    {   DISCARD getreg_signetics(0, FALSE);
        if     (compareit("+", FALSE) || compareit(",+", FALSE))
        {   asmerror("auto-increment not allowed");
        } elif (compareit("-", FALSE) || compareit(",-", FALSE))
        {   asmerror("auto-increment not allowed");
    }   }

    byte1st |= (value / 256);
    byte2nd =  (value % 256);
    emit(byte1st);
    emit(byte2nd);
}

MODULE void getabsolute_nonbranch_oldcalm(UBYTE firstopcode)
{   int   value;
    UBYTE byte1st,
          byte2nd,
          reg1;

    if     (compareit("(-", FALSE)) // eg. LOAD A,(-reg)+abs
    {   byte1st = 0x40;
    } elif (compareit("(+", FALSE)) // eg. LOAD A,(+reg)+abs
    {   byte1st = 0x20;
    } elif (compareit("(",  FALSE)) // eg. LOAD A,(reg)+abs
    {   byte1st = 0x60;
    } else
    {   byte1st = 0;
    }
    if (byte1st)
    {   reg1 = getreg_calm(2, TRUE);
        if (reg1 == 4)
        {   asmerror("register expected");
            reg1 = 0;
        }
        firstopcode &= 0xFC;
        firstopcode |= reg1;
        if (!compareit(")+", FALSE))
        {   asmerror(")+ expected");
    }   }
    if (compareit("@", FALSE))
    {   byte1st |= 0x80;
    }

    value   =  parsenumber(&LineBuffer[linecursor]) & 0x1FFF;
    byte1st |= value         /    256;
    byte2nd =  value         %    256;
    emit(firstopcode);
    emit(byte1st);
    emit(byte2nd);
}

MODULE FLAG ganb_newcalm(FLAG allpages) // GANB means get absolute non-branch
{   FLAG  rc;
    UBYTE reg1;
    int   maskval,
          value;

    // allpages is whether to mask with $7FFF instead (in case this turns out to be a MOVE U^abs,PSL instruction).
    // Return code is whether indexing was used.

    maskval = allpages ? 0x7FFF : 0x1FFF;

    if   (compareit("{-", FALSE)) // eg. MOVE U^{-reg}+abs,r0 (LODA,r0 abs,reg)
    {   gbyte1st = 0x40;
        reg1 = getreg_signetics(2, TRUE);
        if (reg1 == 4)
        {   asmerror("register expected");
            reg1 = 0;
        }
        gopcode &= 0xFC;
        gopcode |= reg1;
        if (!compareit("}+", FALSE))
        {   asmerror("}+ expected");
        }
        rc = TRUE;
    } elif (compareit("{+", FALSE)) // eg. MOVE U^{+reg}+abs,r0
    {   gbyte1st = 0x20;
        reg1 = getreg_signetics(2, TRUE);
        if (reg1 == 4)
        {   asmerror("register expected");
            reg1 = 0;
        }
        gopcode &= 0xFC;
        gopcode |= reg1;
        if (!compareit("}+", FALSE))
        {   asmerror("}+ expected");
        }
        rc = TRUE;
    } elif (compareit("{",  FALSE)) // eg. MOVE U^{r0}+abs,reg or MOVE U^{abs},reg
    {   reg1 = getreg_signetics(2, TRUE);
        if (reg1 == 4) // eg. MOVE U^{abs}
        {   gbyte1st = 0x80;
            value   =  parsenumber(&LineBuffer[linecursor]) & maskval;
            if (!compareit("}", FALSE))
            {   asmerror("} expected");
            }
            gbyte1st |= value / 256;
            gbyte2nd =  value % 256;
            return FALSE;
        } else // eg. MOVE U^{r0}+abs,reg
        {   gbyte1st = 0x60;
            gopcode &= 0xFC;
            gopcode |= reg1;
            if (!compareit("}+", FALSE))
            {   asmerror("}+ expected");
            }
            rc = TRUE;
    }   }
    else
    {   gbyte1st = 0;             // eg. MOVE U^abs
        rc = FALSE;
    }

    if (compareit("{", FALSE))
    {   gbyte1st |= 0x80;
        value = parsenumber(&LineBuffer[linecursor]) & maskval;
        if (!compareit("}", FALSE))
        {   asmerror("} expected");
    }   }
    else
    {   value = parsenumber(&LineBuffer[linecursor]) & maskval;
    }
    gbyte1st |= value / 256;
    gbyte2nd =  value % 256;

    return rc;
}

MODULE void getabsolute_nonbranch_ieee(UBYTE firstopcode, int thereg)
{   int   value;
    UBYTE byte1st,
          byte2nd;

    if (compareit("@", FALSE))
    {   byte1st = 0x80;
    } else
    {   byte1st = 0;
    }
    value = parsenumber(&LineBuffer[linecursor]) & 0x1FFF;

    if (compareit("(", FALSE))
    {   if     (compareit("+", FALSE))
        {   byte1st |= 0x20;
        } elif (compareit("-", FALSE))
        {   byte1st |= 0x40;
        } else
        {   byte1st |= 0x60;
        }
        if (thereg != 0)
        {   asmwarning(".0 is required as first register");
        }
        thereg = getreg_ieee();
        if (!compareit(")", FALSE))
        {   asmerror(") expected");
    }   }

    byte1st |= (value / 256);
    byte2nd =  (value % 256);
    emit(firstopcode + thereg);
    emit(byte1st);
    emit(byte2nd);
}

MODULE int fiximmediate(int value)
{   if (value >= 0xFF80)
    {   value -= 65536;
    }
    if (value >= -128 && value <= -1)
    {   value += 256; // -128..-1 -> 128..255
    }
    return value;
}

MODULE void getrelative(FLAG indirect)
{   // Used when the opcode has already been emitted.

    if (style != STYLE_OLDCALM && style != STYLE_NEWCALM && style != STYLE_IEEE)
    {   skipblanks(TRUE);
    }

    emit(dorel(1, indirect));
}

MODULE UBYTE dorel(int wherefrom, FLAG indirect)
{   // Returns the operand byte.

    UBYTE rc;
    int   backwards,
          forwards,
          middle,
          value;

    if (indirect || compareit(indchars[style], FALSE))
    {   rc = 0x80;
        value  = parsenumber(&LineBuffer[linecursor]);
        if (style == STYLE_NEWCALM && !compareit("}", FALSE))
        {   asmerror("} expected");
    }   }
    else
    {   rc = 0;
        value  = parsenumber(&LineBuffer[linecursor]);
    }
    middle = ocursor + wherefrom; // should really wrap this

    if (style != STYLE_NEWCALM && compareit(",", FALSE))
    {   asmwarning("ignoring indexing on relative address");
    }

    if ((middle & PAGE) != (value & PAGE))
    {   asmerror("relative address out of range (wrong page)");
        rc = 0;
    } else
    {   forwards = (value - middle) & 0x1FFF;
        if (forwards >= 0 && forwards <= 63)
        {   rc |= forwards;
        } else
        {   backwards = (middle - value) & 0x1FFF;
            if (backwards >= 1 && backwards <= 64)
            {   rc |= (0x80 - backwards);
            } else
            {   asmerror("relative address out of range (too far)");
                rc = 0;
    }   }   }

    return rc;
}

MODULE void getzeropage(FLAG indirect)
{   int   value;
    UBYTE byte1st;

    if (indirect || compareit(indchars[style], FALSE))
    {   byte1st = 0x80;
        value = parsenumber(&LineBuffer[linecursor]);
        if (style == STYLE_NEWCALM && !compareit("}", FALSE))
        {   asmerror("} expected");
    }   }
    else
    {   byte1st = 0;
        value = parsenumber(&LineBuffer[linecursor]);
    }
    if (value >= 8192 - 64 && value <= 8192 - 1)
    {   value -= 8192; // -64..  -1
        value +=  128; // +64..+127
        byte1st |= (UBYTE) value; // $40..$7F
    } elif (value >= 0 && value <= 63)
    {   byte1st |= (UBYTE) value; // $00..$3F
    } else
    {   asmerror("zero page address out of range");
        byte1st = 0;
    }
    emit(byte1st);
}

MODULE void emit(int data)
{
#ifdef VERBOSE
    zprintf(TEXTPEN_CLIOUTPUT, "$%04X: $%02X\n", ocursor, data);
#endif

    if (ocursor >= 32768)
    {   asmerror("attempted to emit code/data beyond 32K");
        return;
    }

    if
    (   (   machine == ARCADIA
         || machine == INTERTON
        )
     && (   !(memflags[ocursor] & NOWRITE) // if writable   (ie. RAM)
         ||  (memflags[ocursor] & NOREAD ) // or unreadable (ie. write-only or unmapped)
    )   )
    {   asmwarning("code or data in non-ROM area");
    }

    if (ocursor < firstaddr) firstaddr = ocursor;
    if (ocursor > lastaddr ) lastaddr  = ocursor;

    if (pass == 2 && !orgy)
    {   sprintf(gtempstring, "origin not set (assuming $%X)", ocursor);
        asmwarning(gtempstring);
        orgy = TRUE;
    }

    memory[ocursor++] = (UBYTE) data;

    if (emitpos < 3)
    {   emitbuf[emitpos++] = data;
}   }

EXPORT int parsenumber(STRPTR stringptr)
{   int value;

    if (stringptr == NULL || *stringptr == EOS)
    {   asmerror("empty expression");
        return 0;
    }
    evalstring = stringptr;
    value = expr();
    if (assembling)
    {   linecursor += evalstring - stringptr;
    }

    if
    (   *evalstring != EOS
     && (   !assembling
         || (   *evalstring != ','
             && *evalstring != ' '
             && *evalstring != HT
             && *evalstring != ';'
             && *evalstring != '+'
             && *evalstring != '-'
             && *evalstring != '.'
             && *evalstring != '}'
    )   )   )
    {   sprintf(gtempstring, "invalid character '%c'", *evalstring);
        asmerror(gtempstring);
        return 0;
    }

    return value;
}

MODULE int expr(void) // this can be called recursively!
{   int sign,
        val;

    switch (*evalstring)
    {
    case  '+': sign =  1; evalstring++;
    acase '-': sign = -1; evalstring++;
    adefault:  sign =  1;
    }
    val = sign * factor();
    for (;;)
    {   switch (*evalstring)
        {
        case  '+':            evalstring++;    val += factor();
        acase '-':            evalstring++;    val -= factor();
        acase '*': case 0xD7: evalstring++;    val *= factor();
        acase '/': case 0xF7: evalstring++;    val /= factor();
        acase '^':            evalstring++;    val ^= factor();
        acase '|':            evalstring++;    val |= factor();
        acase '&':            evalstring++;    val &= factor();
        acase '%':            if (val) { evalstring++; val %= factor(); } else { asmerror("divide by zero"); val = 0; }
        acase '<':            if ((*evalstring + 1) == '<') { evalstring += 2; val <<= factor(); } else return val;
        acase '>':            if ((*evalstring + 1) == '>') { evalstring += 2; val >>= factor(); } else return val;
        acase '~':            evalstring++;    val = factor() ^ 0xFF;
        adefault:             return val;
}   }   }

MODULE int factor(void) // this can be called recursively!
{   int    length,
           val;
    STRPTR evalend;

    if (*evalstring == '(')
    {   evalstring++;
        val = expr();
        if (*evalstring == ')')
            evalstring++;
        else
        {   asmerror(") expected");
            return 0;
    }   }
    elif (!strnicmp(evalstring, "HI(", 3))
    {   evalstring += 3;
        val = expr() / 256;
        if (*evalstring == ')')
            evalstring++;
        else
        {   asmerror(") expected");
            return 0;
    }   }
    elif (!strnicmp(evalstring, "LO(", 3))
    {   evalstring += 3;
        val = expr() % 256;
        if (*evalstring == ')')
            evalstring++;
        else
        {   asmerror(") expected");
            return 0;
    }   }
    elif (*evalstring == '<' && *(evalstring + 1) != '<') // MSB
    {   evalstring++;
        val = expr() / 256;
    } elif (*evalstring == '>' && *(evalstring + 1) != '>') // LSB
    {   evalstring++;
        val = expr() % 256;
    } else
    {   evalend = evalstring;
        length = 0;
        while
        (   !isterminator(*evalend)
         && *evalend != '+'
         && *evalend != '-'
         && *evalend != '*' && *evalend != 0xD7
         && *evalend != '/' && *evalend != 0xF7
         && *evalend != '^'
         && *evalend != '|'
         && *evalend != '&'
         && *evalend != '%'
         && *evalend != '<'
         && *evalend != '>'
         && *evalend != '~'
         && *evalend != '}'
        )
        {   if (length == 0) // so that eg. 16'1234 works
            {   if (*evalend == QUOTE)
                {   length++;
                    evalend++;
                    while (*evalend != QUOTE && *evalend != EOS && *evalend != CR && *evalend != LF)
                    {   length++;
                        evalend++;
                }   }
                if (*evalend == QUOTES)
                {   length++;
                    evalend++;
                    while (*evalend != QUOTES && *evalend != EOS && *evalend != CR && *evalend != LF)
                    {   length++;
                        evalend++;
            }   }   }
            length++;
            evalend++;
        }
        val = parseonenumber(evalstring, length);
        evalstring += length;
    }
    return val;
}

MODULE FLAG compareit(STRPTR target, FLAG needspace)
{   int length;

    length = strlen(target);

    if
    (   !strnicmp((const char*) &LineBuffer[linecursor], target, length)
     && (   !needspace
         || LineBuffer[linecursor + length] == EOS
         || LineBuffer[linecursor + length] == ' '
         || LineBuffer[linecursor + length] == HT
         || LineBuffer[linecursor + length] == ','
         || LineBuffer[linecursor + length] == '+'
         || LineBuffer[linecursor + length] == '-'
         || LineBuffer[linecursor + length] == ')'
         || LineBuffer[linecursor + length] == '}'
    )   )
    {   linecursor += length;
        return TRUE;
    } else
    {   return FALSE;
}   }

MODULE void getimmediate(FLAG skipthem)
{   int value;

    if (skipthem)
    {   skipblanks(TRUE);
    }

    value = parsenumber(&LineBuffer[linecursor]);
    value = fiximmediate(value);
    if (value < 0 || value > 255)
    {   asmerror("immediate value out of range");
        emit(0);
    } else
    {   emit((UBYTE) value);
}   }

MODULE UBYTE saveimmediate(FLAG skipthem)
{   int value;

    if (skipthem)
    {   skipblanks(TRUE);
    }

    value = parsenumber(&LineBuffer[linecursor]);
    value = fiximmediate(value);
    if (value < 0 || value > 255)
    {   asmerror("immediate value out of range");
        return 0;
    } else
    {   return (UBYTE) value;
}   }

MODULE void getdbx(void)
{   UBYTE t;
    int   i;

    skipblanks(TRUE);

    t = 0;
    if (compareit("\"", FALSE))
    {   for (i = 0; i < 8; i++)
        {   if (LineBuffer[linecursor] == EOS)
            {   asmerror("unexpected end of line");
                emit(0);
                return;
            }
            if (LineBuffer[linecursor] == '"')
            {   asmerror("string too short");
                emit(0);
                return;
            }
            if (LineBuffer[linecursor] != '.' && LineBuffer[linecursor] != ' ')
            {   t |= (0x80 >> i);
            }
            linecursor++;
        }
        if (!compareit("\"", FALSE))
        {   asmwarning("\" expected");
        }
        emit(t);
    } else
    {   asmerror("\" expected");
        emit(0);
}   }

MODULE void getimmediatebytes(void)
{   UBYTE t;
    TEXT  nextchar;
    int   value;

    skipblanks(TRUE);

    for (;;)
    {   if
        (   compareit("'" ,  FALSE)
         || compareit("N'",  FALSE)
        )
        {   while (!compareit("'", FALSE))
            {   if
                (              LineBuffer[linecursor    ] == '\\'
                 &&            LineBuffer[linecursor + 1] == 'x'
                 && ishexdigit(LineBuffer[linecursor + 2])
                 && ishexdigit(LineBuffer[linecursor + 3])
                ) // \x12 format (asm2650)
                {   if       (LineBuffer[linecursor + 2] >= 'A' && LineBuffer[linecursor + 2] <= 'F')
                    {   t =  (LineBuffer[linecursor + 2] -  'A' + 10) * 16;
                    } elif   (LineBuffer[linecursor + 2] >= 'a' && LineBuffer[linecursor + 2] <= 'f')
                    {   t =  (LineBuffer[linecursor + 2] -  'a' + 10) * 16;
                    } else
                    {   t =  (LineBuffer[linecursor + 2] -  '0'     ) * 16;
                    }
                    if       (LineBuffer[linecursor + 3] >= 'A' && LineBuffer[linecursor + 3] <= 'F')
                    {   t += (LineBuffer[linecursor + 3] -  'A' + 10);
                    } elif   (LineBuffer[linecursor + 3] >= 'a' && LineBuffer[linecursor + 3] <= 'f')
                    {   t += (LineBuffer[linecursor + 3] -  'a' + 10);
                    } else
                    {   t += (LineBuffer[linecursor + 3] -  '0'     );
                    }
                    emit(t);
                    linecursor += 4;
                } else
                {   if (machine == INTERTON)
                    {   emit(LineBuffer[linecursor++]);
                    } else
                    {   emit(guestchar(LineBuffer[linecursor++]));
        }   }   }   }
        elif
        (   compareit("\"" , FALSE)
         || compareit("N\"", FALSE)
        )
        {   while (!compareit("\"", FALSE))
            {   if
                (              LineBuffer[linecursor    ] == '\\'
                 &&            LineBuffer[linecursor + 1] == 'x'
                 && ishexdigit(LineBuffer[linecursor + 2])
                 && ishexdigit(LineBuffer[linecursor + 3])
                ) // \x12 format (asm2650)
                {   if       (LineBuffer[linecursor + 2] >= 'A' && LineBuffer[linecursor + 2] <= 'F')
                    {   t =  (LineBuffer[linecursor + 2] -  'A' + 10) * 16;
                    } elif   (LineBuffer[linecursor + 2] >= 'a' && LineBuffer[linecursor + 2] <= 'f')
                    {   t =  (LineBuffer[linecursor + 2] -  'a' + 10) * 16;
                    } else
                    {   t =  (LineBuffer[linecursor + 2] -  '0'     ) * 16;
                    }
                    if       (LineBuffer[linecursor + 3] >= 'A' && LineBuffer[linecursor + 3] <= 'F')
                    {   t += (LineBuffer[linecursor + 3] -  'A' + 10);
                    } elif   (LineBuffer[linecursor + 3] >= 'a' && LineBuffer[linecursor + 3] <= 'f')
                    {   t += (LineBuffer[linecursor + 3] -  'a' + 10);
                    } else
                    {   t += (LineBuffer[linecursor + 3] -  '0'     );
                    }
                    emit(t);
                    linecursor += 4;
                } else
                {   if (machine == INTERTON)
                    {   emit(LineBuffer[linecursor++]);
                    } else
                    {   emit(guestchar(LineBuffer[linecursor++]));
        }   }   }   }
        elif (compareit("A'", FALSE))
        {   while (!compareit("'", FALSE))
            {   emit(LineBuffer[linecursor++]);
        }   }
        elif (compareit("A\"", FALSE))
        {   while (!compareit("\"", FALSE))
            {   emit(LineBuffer[linecursor++]);
        }   }
        elif (compareit("E'", FALSE))
        {   while (!compareit("'", FALSE))
            {   emit(readnum_ebcdic(LineBuffer[linecursor++]));
        }   }
        elif (compareit("E\"", FALSE))
        {   while (!compareit("\"", FALSE))
            {   emit(readnum_ebcdic(LineBuffer[linecursor++]));
        }   }
        elif (compareit("H'", FALSE)) // eg. DATA H'43,0,D,A,A,42,41,4E' COMMENT
        {   for (;;)
            {
NEXT:
                nextchar = LineBuffer[linecursor];
                if (nextchar >= '0' && nextchar <= '9')
                {   t = nextchar - '0';
                } elif (nextchar >= 'A' && nextchar <= 'F')
                {   t = nextchar - 'A' + 10;
                } elif (nextchar >= 'a' && nextchar <= 'f')
                {   t = nextchar - 'a' + 10;
                } else
                {   asmerror("unexpected character");
                    emit(0);
                    return;
                }

                linecursor++;
                nextchar = LineBuffer[linecursor];
                if (nextchar >= '0' && nextchar <= '9')
                {   t <<= 4;
                    t |= nextchar - '0';
                } elif (nextchar >= 'A' && nextchar <= 'F')
                {   t <<= 4;
                    t |= nextchar - 'A' + 10;
                } elif (nextchar >= 'a' && nextchar <= 'f')
                {   t <<= 4;
                    t |= nextchar - 'a' + 10;
                } elif (nextchar == QUOTE)
                {   linecursor++;
                    emit(t);
                    goto DONE;
                } elif (nextchar == ',')
                {   linecursor++;
                    emit(t);
                    goto NEXT;
                } else
                {   asmerror("unexpected character");
                    emit(0);
                    return;
                }

                linecursor++;
                nextchar = LineBuffer[linecursor];
                if (nextchar == QUOTE)
                {   linecursor++;
                    emit(t);
                    goto DONE;
                } elif (nextchar == ',')
                {   linecursor++;
                    emit(t);
                    goto NEXT;
                } else
                {   asmerror("unexpected character");
                    emit(0);
                    return;
        }   }   }
        else
        {   value = fiximmediate(parsenumber(&LineBuffer[linecursor]));
            if (value < 0 || value > 255)
            {   asmerror("immediate value out of range");
                emit(0);
            } else
            {   emit((UBYTE) value);
        }   }

DONE:
        if (LineBuffer[linecursor] != ',')
        {   return;
        }
        linecursor++;
        skipblanks(FALSE);
}   }
MODULE void getimmediatewords(FLAG swap)
{   int value;

    skipblanks(TRUE);

    for (;;)
    {   value = parsenumber(&LineBuffer[linecursor]);
        if (value >= -32768 && value <= -1)
        {   value += 65536; // -32768..-1 -> 32768..65535
        }
        if (value < 0 || value > 65535)
        {   asmerror("immediate value out of range");
            emit(0);
            emit(0);
        } elif (swap) // little-endian
        {   emit((UBYTE) (value % 256));
            emit((UBYTE) (value / 256));
        } else
        {   emit((UBYTE) (value / 256));
            emit((UBYTE) (value % 256));
        }

        skipblanks(FALSE);
        if (LineBuffer[linecursor] != ',') // eg. endline comment without preceding ;
        {   return;
        }
        linecursor++;
        skipblanks(FALSE);
}   }

EXPORT void asmerror(STRPTR errorstring)
{   int i,
        thelength;

    if (!assembling)
    {   zprintf(TEXTPEN_ERROR, "Error: %s!\n\n", errorstring);
        return;
    }

    if (pass == 2 || including)
    {   // assert(ListFileHandle);
        fwrite("*** ", 4, 1, ListFileHandle);
        sprintf((char*) entirestring, "Error at line %d: %s!\n", line, errorstring);
        zprintf(TEXTPEN_CLIOUTPUT, (const char*) entirestring);
        fwrite(entirestring, strlen((const char*) entirestring), 1, ListFileHandle);
        zprintf(TEXTPEN_CLIOUTPUT, "$%04X: ", ocursor);
        thelength = strlen(LineBuffer);
        if (thelength)
        {   for (i = 0; i < thelength; i++)
            {   if (LineBuffer[i] == HT)
                {   zprintf(TEXTPEN_CLIOUTPUT, " "); // so that the ^ is aligned properly
                } else
                {   zprintf(TEXTPEN_CLIOUTPUT, "%c", LineBuffer[i]);
        }   }   }
        zprintf(TEXTPEN_CLIOUTPUT, "\n       ");
        if (linecursor)
        {   for (i = 0; i < linecursor; i++)
            {   zprintf(TEXTPEN_CLIOUTPUT, " ");
        }   }
        zprintf(TEXTPEN_CLIOUTPUT, "^\n");
        errors++;
}   }
MODULE void asmwarning(STRPTR errorstring)
{   int i;

    /* if (!assembling)
    {   zprintf(TEXTPEN_ERROR, "Warning: %s!\n\n", errorstring);
        return;
    } */

    if (pass == 2 || including)
    {   if (warn)
        {   // assert(ListFileHandle);
            fwrite("*** ", 4, 1, ListFileHandle);
            sprintf((char*) entirestring, "Warning at line %d: %s!\n", line, errorstring);
            zprintf(TEXTPEN_CLIOUTPUT, (const char*) entirestring);
            fwrite(entirestring, strlen((const char*) entirestring), 1, ListFileHandle);
            zprintf(TEXTPEN_CLIOUTPUT, "$%04X: %s\n       ", ocursor, LineBuffer);
            if (linecursor)
            {   for (i = 0; i < linecursor; i++)
                {   zprintf(TEXTPEN_CLIOUTPUT, " ");
            }   }
            zprintf(TEXTPEN_CLIOUTPUT, "^\n");
        }
        warnings++;
}   }

MODULE void getaddress(UBYTE offset)
{   // assert(style == STYLE_OLDCALM);

    skipblanks(TRUE);

    if (compareit("@.+", FALSE))
    {   emit(offset);
        getrelative(TRUE);
    } elif (compareit(".+", FALSE))
    {   emit(offset);
        getrelative(FALSE);
    } else
    {   emit(offset + 4);
        getabsolute_branch();
}   }

MODULE int findlabel(STRPTR passedstring, int length, FLAG allowall)
{   int i;

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Matching \"%s\" of length %d...\n", passedstring, length);
#endif

    if (userlabels)
    {   for (i = 0; i < userlabels; i++)
        {   if
            (   length == (int) strlen((const char*) symlabel[i].name)
             && !strnicmp((const char*) symlabel[i].name, passedstring, length)
            )
            {
#ifdef VERBOSE
                zprintf(TEXTPEN_CLIOUTPUT, "Found user-defined label %s ($%X).\n", passedstring, symlabel[i].address);
#endif
                if (assembling)
                {   symlabel[i].used = TRUE;
                }
                return symlabel[i].address;
    }   }   }

    for (i = FIRSTCPUEQUIV; i <= LASTCPUEQUIV; i++)
    {   if (length == (int) strlen((const char*) equivalents[i].name) && !strnicmp((const char*) equivalents[i].name, passedstring, length))
        {
#ifdef VERBOSE
            zprintf(TEXTPEN_CLIOUTPUT, "Found CPU equate %s ($%X).\n", passedstring, (int) equivalents[i].address);
#endif
            return (int) equivalents[i].address;
    }   }

    if (machines[machine].pvis)
    {   for (i = FIRSTPVIEQUIV; i < FIRSTPVIEQUIV + (33 * machines[machine].pvis); i++)
        {   if (length == (int) strlen((const char*) equivalents[i].name) && !strnicmp((const char*) equivalents[i].name, passedstring, length))
            {
#ifdef VERBOSE
                zprintf(TEXTPEN_CLIOUTPUT, "Found PVI label %s ($%X).\n", passedstring, (int) (pvibase + equivalents[i].address));
#endif
                return (int) (pvibase + equivalents[i].address);
    }   }   }

    if (machines[machine].firstequiv != -1)
    {   for (i = machines[machine].firstequiv; i <= machines[machine].lastequiv; i++)
        {   if (length == (int) strlen((const char*) equivalents[i].name) && !strnicmp((const char*) equivalents[i].name, passedstring, length))
            {   
#ifdef VERBOSE
                zprintf(TEXTPEN_CLIOUTPUT, "Found built-in guest label %s ($%X).\n", passedstring, (int) equivalents[i].address);
#endif
                return equivalents[i].address;
    }   }   }

    if (machine == BINBUG && firstdosequiv != -1)
    {   for (i = firstdosequiv; i <= lastdosequiv; i++)
        {   if (length == (int) strlen((const char*) equivalents[i].name) && !strnicmp((const char*) equivalents[i].name, passedstring, length))
            {
#ifdef VERBOSE
                zprintf(TEXTPEN_CLIOUTPUT, "Found built-in DOS label %s ($%X).\n", passedstring, (int) equivalents[i].address);
#endif
                return equivalents[i].address;
    }   }   }

    if (machine == BINBUG)
    {   for (i = FIRSTACOSEQUIV; i <= LASTACOSEQUIV; i++)
        {   if (length == (int) strlen((const char*) equivalents[i].name) && !strnicmp((const char*) equivalents[i].name, passedstring, length))
            {
#ifdef VERBOSE
                zprintf(TEXTPEN_CLIOUTPUT, "Found built-in ACOS label %s ($%X).\n", passedstring, (int) equivalents[i].address);
#endif
                return equivalents[i].address;
    }   }   }

    if (whichgame != -1 && known[whichgame].firstequiv != -1)
    {   for (i = FIRSTGAMEEQUIV + known[whichgame].firstequiv; i <= FIRSTGAMEEQUIV + known[whichgame].lastequiv; i++)
        {   if (length == (int) strlen((const char*) equivalents[i].name) && !strnicmp((const char*) equivalents[i].name, passedstring, length))
            {
#ifdef VERBOSE
                zprintf(TEXTPEN_CLIOUTPUT, "Found game-specific label %s ($%X).\n", passedstring, (int) equivalents[i].address);
#endif
                return equivalents[i].address;
    }   }   }

    if (!allowall)
    {   lastparse = BASE_LABEL;
        return OUTOFRANGE;
    }

    i = parse_opcode(passedstring, length);
    if (i != OUTOFRANGE)
    {   return i;
    }
    i = parse_literal(passedstring, length);
    return i;
}

MODULE FLAG includebin(void)
{   FLAG   done;
    TEXT   quotechar[1 + 1];
    int    i = 0,
           incsize;
    UBYTE* IncBuffer;
    FILE*  IncHandle;

    skipblanks(TRUE);
    if (compareit("\"", FALSE))
    {   quotechar[0] = QUOTES;
    } elif (compareit("'", FALSE))
    {   quotechar[0] = QUOTE;
    } else
    {   while (LineBuffer[linecursor] != EOS)
        {   fn_inc[i++] = LineBuffer[linecursor++];
        }
        fn_inc[i] = EOS;
        goto DONE;
    }    
    quotechar[1] = EOS;
    done = FALSE;
    do
    {   if (LineBuffer[linecursor] == EOS)
        {   asmerror("unexpected end of line");
            return FALSE;
        } // implied else
        if (compareit(quotechar, FALSE))
        {   done = TRUE;
        } else
        {   fn_inc[i++] = LineBuffer[linecursor++];
    }   }
    while (!done);
    fn_inc[i] = EOS;
    skipblanks(FALSE);
    if (LineBuffer[linecursor] != EOS)
    {   asmerror("EOL expected");
        return FALSE;
    } // implied else

DONE:
    if (pass == 2)
    {   return TRUE;
    }

    incsize = (int) getsize((STRPTR) fn_inc);
    if (!(IncBuffer = malloc(incsize)))
    {   asmerror("out of memory");
        return FALSE;
    }
    if (!(IncHandle = fopen((const char*) fn_inc, "rb")))
    {   asmerror("can't open include file for reading");
        free(IncBuffer);
        return FALSE;
    }
    if (fread(IncBuffer, (size_t) incsize, 1, IncHandle) != 1)
    {   asmerror("can't read from include file");
        fclose(IncHandle);
        free(IncBuffer);
        return FALSE;
    }
    fclose(IncHandle);

    for (i = 0; i < incsize; i++)
    {   emit(IncBuffer[i]);
    }

    free(IncBuffer);

    return TRUE;
}

MODULE FLAG includefile(void)
{   FLAG   done;
    TEXT   quotechar[1 + 1];
    int    i = 0,
           incsize1, incsize2;
    UBYTE *IncBuffer,
          *TotalAsmBuffer;
    FILE*  IncHandle;

    including = TRUE;
    skipblanks(TRUE);
    if (compareit("\"", FALSE))
    {   quotechar[0] = QUOTES;
    } elif (compareit("'", FALSE))
    {   quotechar[0] = QUOTE;
    } else
    {   while (LineBuffer[linecursor] != EOS)
        {   fn_inc[i++] = LineBuffer[linecursor++];
        }
        fn_inc[i] = EOS;
        goto DONE;
    }    
    quotechar[1] = EOS;
    done = FALSE;
    do
    {   if (LineBuffer[linecursor] == EOS)
        {   asmerror("unexpected end of line");
            including = FALSE;
            return FALSE;
        } // implied else
        if (compareit(quotechar, FALSE))
        {   done = TRUE;
        } else
        {   fn_inc[i++] = LineBuffer[linecursor++];
    }   }
    while (!done);
    fn_inc[i] = EOS;
    skipblanks(FALSE);
    if (LineBuffer[linecursor] != EOS)
    {   asmerror("EOL expected");
        including = FALSE;
        return FALSE;
    } // implied else

DONE:
    if (pass == 2)
    {   including = FALSE;
        return TRUE;
    }

    incsize1 = (int) getsize((STRPTR) fn_inc);
    if (!(IncBuffer = malloc(incsize1 + 2 + strlen(ENDMARKER) + strlen(fn_inc) + 1)))
    {   asmerror("out of memory");
        including = FALSE;
        return FALSE;
    }
    IncBuffer[0] = LF;
    if (!(IncHandle = fopen((const char*) fn_inc, "rb")))
    {   asmerror("can't open include file for reading");
        free(IncBuffer);
        including = FALSE;
        return FALSE;
    }
    if (fread(&IncBuffer[1], (size_t) incsize1, 1, IncHandle) != 1)
    {   asmerror("can't read from include file");
        fclose(IncHandle);
        free(IncBuffer);
        including = FALSE;
        return FALSE;
    }
    fclose(IncHandle);
    IncBuffer[incsize1 + 1] = LF;

    i = incsize2 = 0;
    while (i < incsize1 + 2)
    {   if (IncBuffer[i] != CR)
        {   IncBuffer[incsize2] = IncBuffer[i];
            incsize2++;
        }
        i++;
    }
    strcpy((char*) &IncBuffer[incsize2], ENDMARKER);
    incsize2 += strlen(ENDMARKER);
    strcpy((char*) &IncBuffer[incsize2], fn_inc);
    incsize2 += strlen(fn_inc);
    IncBuffer[incsize2++] = LF;
    // don't add a null terminator!

    if (!(TotalAsmBuffer = malloc(asmsize2 + incsize2)))
    {   asmerror("out of memory");
        free(IncBuffer);
        including = FALSE;
        return FALSE;
    }
    memcpy( TotalAsmBuffer                    ,  AsmBuffer         , icursor);
    memcpy(&TotalAsmBuffer[icursor           ],  IncBuffer         , incsize2);
    memcpy(&TotalAsmBuffer[icursor + incsize2], &AsmBuffer[icursor], asmsize2 - icursor);
    free(AsmBuffer);
    free(IncBuffer);
    AsmBuffer = (STRPTR) TotalAsmBuffer;
    asmsize2 += incsize2;

#ifdef VERBOSE
    if (!(IncHandle = fopen("Combined.asm", "wt")))
    {   asmerror("can't open output file for writing");
        including = FALSE;
        return FALSE;
    }
    fwrite(AsmBuffer, asmsize2, 1, IncHandle);
    fclose(IncHandle);
    // zprintf(TEXTPEN_VERBOSE, "%s!\n", AsmBuffer);
#endif

    including = FALSE;
    return TRUE;
}

MODULE FLAG ishexdigit(TEXT which)
{   if 
    (   (which >= '0' && which <= '9')
     || (which >= 'A' && which <= 'F')
     || (which >= 'a' && which <= 'f')
    )
    {   return TRUE;
    } // implied else
    return FALSE;
}

#define OPCODESYNONYMS_NONEXTENDED 34
#define OPCODESYNONYMS_ALL         65
EXPORT int parse_opcode(STRPTR passedstring, int length)
{   int i;
PERSIST const struct
{   const STRPTR name;
    const UBYTE  opcode;
} opcodesynonym[OPCODESYNONYMS_ALL] = {
{ "LODZ,r0", 0x00 }, //  0
{ "LODZ,r1", 0x01 },
{ "LODZ,r2", 0x02 },
{ "LODZ,r3", 0x03 },
{ "EORZ,r0", 0x20 },
{ "EORZ,r1", 0x21 },
{ "EORZ,r2", 0x22 },
{ "EORZ,r3", 0x23 },
{ "ANDZ,r1", 0x41 },
{ "ANDZ,r2", 0x42 },
{ "ANDZ,r3", 0x43 }, // 10
{ "IORZ,r0", 0x60 },
{ "IORZ,r1", 0x61 },
{ "IORZ,r2", 0x62 },
{ "IORZ,r3", 0x63 },
{ "ADDZ,r0", 0x80 },
{ "ADDZ,r1", 0x81 },
{ "ADDZ,r2", 0x82 },
{ "ADDZ,r3", 0x83 },
{ "SUBZ,r0", 0xA0 },
{ "SUBZ,r1", 0xA1 }, // 20
{ "SUBZ,r2", 0xA2 },
{ "SUBZ,r3", 0xA3 },
{ "STRZ,r1", 0xC1 },
{ "STRZ,r2", 0xC2 },
{ "STRZ,r3", 0xC3 },
{ "COMZ,r0", 0xE0 },
{ "COMZ,r1", 0xE1 },
{ "COMZ,r2", 0xE2 },
{ "COMZ,r3", 0xE3 },
{ "EQ"     ,    0 }, // 30
{ "GT"     ,    1 },
{ "LT"     ,    2 },
{ "UN"     ,    3 }, // 33
// Extended opcodes start here...
{ "RET"    , 0x17 }, // 34
{ "BER"    , 0x18 },
{ "BZR"    , 0x18 },
{ "BOR"    , 0x18 },
{ "BHR"    , 0x19 },
{ "BPR"    , 0x19 },
{ "BLR"    , 0x1A }, // 40
{ "BMR"    , 0x1A },
{ "BR"     , 0x1B },
{ "BEA"    , 0x1C },
{ "BZA"    , 0x1C },
{ "BOA"    , 0x1C },
{ "BHA"    , 0x1D },
{ "BPA"    , 0x1D },
{ "BLA"    , 0x1E },
{ "BMA"    , 0x1E },
{ "BA"     , 0x1B }, // 50
{ "BSR"    , 0x3B },
{ "BSA"    , 0x3F },
{ "BNER"   , 0x98 },
{ "BNZR"   , 0x98 },
{ "BNHR"   , 0x99 },
{ "BNPR"   , 0x99 },
{ "BNLR"   , 0x9A },
{ "BNMR"   , 0x9A },
{ "BNEA"   , 0x9C },
{ "BNZA"   , 0x9C }, // 60
{ "BNHA"   , 0x9D },
{ "BNPA"   , 0x9D },
{ "BNLA"   , 0x9E },
{ "BNMA"   , 0x9E }, // 64
};

    for (i = 0; i < 256; i++)
    {   if (length == (int) strlen(opcodes[style][i].name) && !strnicmp(passedstring, opcodes[style][i].name, length))
        {   return i;
    }   }
    if (style == STYLE_SIGNETICS1)
    {   for (i = 0; i < OPCODESYNONYMS_NONEXTENDED; i++)
        {   if (length == (int) strlen(opcodesynonym[i].name) && !strnicmp(passedstring, opcodesynonym[i].name, length))
            {   return (int) opcodesynonym[i].opcode;
    }   }   }
    elif (style == STYLE_SIGNETICS2)
    {   for (i = 0; i < OPCODESYNONYMS_ALL; i++)
        {   if (length == (int) strlen(opcodesynonym[i].name) && !strnicmp(passedstring, opcodesynonym[i].name, length))
            {   return (int) opcodesynonym[i].opcode;
    }   }   }

    return OUTOFRANGE;
}

EXPORT int parse_literal(STRPTR passedstring, int length)
{   int  i,
         localbase,
         value = 0; // initialized to avoid a spurious SAS/C warning
    TEXT firstletter;

    firstletter = toupper(passedstring[0]);

    zstrncpy(gtempstring, passedstring, length);
    for (i = 0; i < length; i++)
    {   if
        (   gtempstring[i] == EOS || gtempstring[i] == ' ' || gtempstring[i] == HT || gtempstring[i] == ';'
         || (assembling && style == STYLE_IEEE && (gtempstring[i] == '(' || gtempstring[i] == ')'))
        )
        {   gtempstring[i] = EOS; // but we currently don't use gtempstring, only passedstring
            linecursor -= (length - i);
            length = i;
            break;
    }   }

    if     (firstletter == 'H' && passedstring[1] == QUOTE && passedstring[length - 1] == QUOTE)                   // H'1234' format (Signetics)
    {   return readnum_hex(    (STRPTR) &passedstring[2], length - 3);
    } elif (firstletter == 'D' && passedstring[1] == QUOTE && passedstring[length - 1] == QUOTE)                   // D'1234' format (Signetics, MicroByte)
    {   return readnum_decimal((STRPTR) &passedstring[2], length - 3);
    } elif (firstletter == 'O' && passedstring[1] == QUOTE && passedstring[length - 1] == QUOTE)                   // O'1234' format (Signetics)
    {   return readnum_octal(  (STRPTR) &passedstring[2], length - 3);
    } elif (firstletter == 'B' && passedstring[1] == QUOTE && passedstring[length - 1] == QUOTE)                   // B'0101' format (Signetics)
    {   return readnum_binary( (STRPTR) &passedstring[2], length - 3);
    } elif (firstletter == 'A' && passedstring[1] == QUOTE && passedstring[3] == QUOTE && length == 4)             // A'Z' format (MicroByte)
    {   return passedstring[2];
    } elif (firstletter == 'E' && passedstring[1] == QUOTE && passedstring[length - 1] == QUOTE)                   // E'XYZ' format (Signetics)
    {   return readnum_ebcdic(passedstring[2]);
    } elif (firstletter == 'N' && passedstring[1] == QUOTE && passedstring[3] == QUOTE && length == 4)             // N'Z' format
    {   return readnum_char(passedstring[2]);
    } elif (passedstring[0] == '0' && toupper(passedstring[1]) >= 'A' && toupper(passedstring[1]) <= 'F')          // 0FFFF format (MicroByte)
    {   return readnum_hex(    (STRPTR) &passedstring[1], length - 1);
    } elif (!strnicmp(passedstring, hexchars[style],  strlen(hexchars[style])))                                    // 'H'/16'/H' prefix
    {   return readnum_hex(    (STRPTR) &passedstring[strlen(hexchars[style])], length - strlen(hexchars[style]));
    } elif (!strnicmp(passedstring, decchars[style],  strlen(decchars[style])))                                    // 'D'/10'/D' prefix
    {   return readnum_decimal((STRPTR) &passedstring[strlen(decchars[style])], length - strlen(decchars[style]));
    } elif (!strnicmp(passedstring, octchars[style],  strlen(octchars[style])))                                    // 'O'/8'/Q' prefix
    {   return readnum_octal(  (STRPTR) &passedstring[strlen(octchars[style])], length - strlen(octchars[style]));
    } elif (!strnicmp(passedstring, binchars[style],  strlen(binchars[style])))                                    // 'B'/2'/B' prefix
    {   return readnum_binary( (STRPTR) &passedstring[strlen(binchars[style])], length - strlen(binchars[style]));
    } elif (passedstring[0] == '$' && length >= 2)                                                                 // $ prefix
    {   return readnum_hex(    (STRPTR) &passedstring[                      1], length -                       1);
    } elif (passedstring[0] == '!')                                                                                // ! prefix
    {   return readnum_decimal((STRPTR) &passedstring[                      1], length -                       1);
    } elif (passedstring[0] == '@')                                                                                // @ prefix
    {   return readnum_decimal((STRPTR) &passedstring[                      1], length -                       1);
    } elif (passedstring[0] == '%')                                                                                // % prefix
    {   return readnum_decimal((STRPTR) &passedstring[                      1], length -                       1);
    } elif (passedstring[0] == QUOTE                                                                               // 'Z' format
         || passedstring[0] == QUOTES)                                                                             // "Z" format
    {   return readnum_char(passedstring[1]);
    } else // 1234 format
    {   localbase = assembling ? defasmbase : base;
        if (toupper(passedstring[length - 1]) == 'H')                                                              // 1234H format
        {   return readnum_hex(    (STRPTR)  passedstring   , length - 1);
        } elif
        (   toupper(passedstring[length - 1]) == 'O'                                                               // 1234O format
         || toupper(passedstring[length - 1]) == 'Q'                                                               // 1234Q format
        )
        {   return readnum_octal(  (STRPTR)  passedstring   , length - 1);
        } elif (localbase != BASE_HEX && toupper(passedstring[length - 1]) == 'D')                                 // 1234D format
        {   return readnum_decimal((STRPTR)  passedstring   , length - 1);
        } elif (localbase != BASE_HEX && toupper(passedstring[length - 1]) == 'B')                                 // 1234B format
        {   return readnum_binary( (STRPTR)  passedstring   , length - 1);
        } else
        {   switch (localbase)
            {
            case  BASE_DECIMAL:
                value = readnum_decimal(passedstring, length);
                if (value == OUTOFRANGE)
                {   if (assembling && pass != 1)
                    {   asmerror("invalid decimal number");
                    }
                    if (assembling)
                    {   value = 0;
                }   }
            acase BASE_HEX:
                value = readnum_hex(    passedstring, length);
                if (value == OUTOFRANGE)
                {   if (assembling && pass != 1)
                    {   asmerror("invalid hex number");
                    }
                    if (assembling)
                    {   value = 0;
                }   }
            acase BASE_BINARY:
                value = readnum_binary( passedstring, length);
                if (value == OUTOFRANGE)
                {   if (assembling && pass != 1)
                    {   asmerror("invalid binary number");
                    }
                    if (assembling)
                    {   value = 0;
                }   }
            acase BASE_OCTAL:
                value = readnum_octal(  passedstring, length);
                if (value == OUTOFRANGE)
                {   if (assembling && pass != 1)
                    {   asmerror("invalid octal number");
                    }
                    if (assembling)
                    {   value = 0;
    }   }   }   }   }

    return value;
}

MODULE int parseonenumber(STRPTR passedstring, int length)
{   int i,
        value;

    if (length == 0)
    {   return 0; // important!
    }

    if
    (   (length == (int) strlen(apcchars[style]) && !strnicmp(passedstring, apcchars[style], length)                                                   )
     || (length ==                             1 && passedstring[0] == '.') // used by X2650 cross-asm
    )
    {   if (equmode)
        {   value = ocursor;
        } else
        {   value = ocursor - 1;
    }   }
    elif (length > (int) strlen(hexchars[style]) && !strnicmp(passedstring, hexchars[style], strlen(hexchars[style])))
    {   value = readnum_hex(    &passedstring[strlen(hexchars[style])], length - strlen(hexchars[style]));
        if (value == OUTOFRANGE)
        {   asmerror("invalid hex number");
            value = 0;
    }   }
    elif (length > (int) strlen(decchars[style]) && !strnicmp(passedstring, decchars[style], strlen(decchars[style])))
    {   value = readnum_decimal(&passedstring[strlen(decchars[style])], length - strlen(decchars[style]));
        if (value == OUTOFRANGE)
        {   asmerror("invalid decimal number");
            value = 0;
    }   }
    elif (length > (int) strlen(octchars[style]) && !strnicmp(passedstring, octchars[style], strlen(octchars[style])))
    {   value = readnum_octal(  &passedstring[strlen(octchars[style])], length - strlen(octchars[style]));
        if (value == OUTOFRANGE)
        {   asmerror("invalid octal number");
            value = 0;
    }   }
    elif (length > (int) strlen(binchars[style]) && !strnicmp(passedstring, binchars[style], strlen(binchars[style])))
    {   value = readnum_binary( &passedstring[strlen(binchars[style])], length - strlen(binchars[style]));
        if (value == OUTOFRANGE)
        {   asmerror("invalid binary number");
            value = 0;
    }   }
    else switch (passedstring[0])
    {
    case '^': // X2650 cross-asm
        if (style != STYLE_OLDCALM && style != STYLE_NEWCALM)
        {   switch (toupper(passedstring[1]))
            {
            case 'B':
                value = readnum_binary(&passedstring[2], length - 2);
                if (value == OUTOFRANGE)
                {   asmerror("invalid binary number");
                    value = 0;
                }
            acase 'C':
                value = findlabel(&passedstring[2], length - 2, TRUE);
                if (value == OUTOFRANGE)
                {   asmerror("invalid label or number for ^C");
                    value = 0;
                } else
                {   value = 65535 - value;
                }
            acase 'D':
                value = readnum_decimal(&passedstring[2], length - 2);
                if (value == OUTOFRANGE)
                {   asmerror("invalid decimal number");
                    value = 0;
                }
            acase 'H':
                value = readnum_hex(&passedstring[2], length - 2);
                if (value == OUTOFRANGE)
                {   asmerror("invalid hex number");
                    value = 0;
                }
            acase 'L':
                value = findlabel(&passedstring[2], length - 2, TRUE);
                if (value == OUTOFRANGE)
                {   asmerror("invalid label or number for ^L");
                    value = 0;
                } else
                {   value %= 256;
                }
            acase 'M':
                value = findlabel(&passedstring[2], length - 2, TRUE);
                if (value == OUTOFRANGE)
                {   asmerror("invalid label or number for ^M");
                    value = 0;
                } else
                {   value /= 256;
                }
            acase 'O':
                value = readnum_octal(&passedstring[2], length - 2);
                if (value == OUTOFRANGE)
                {   asmerror("invalid octal number");
                    value = 0;
                }
            adefault:
                asmerror("B/C/D/H/L/M/O expected");
                value = 0;
        }   }
        else
        {   value = findlabel(passedstring, length, TRUE);
            if (value == OUTOFRANGE && assembling)
            {   asmerror("invalid label or number");
                value = 0;
        }   }
    acase QUOTE:
    case QUOTES:
        value = readnum_char(passedstring[1]);
    acase '<': // high byte
        value = findlabel(&passedstring[1], length - 1, FALSE);
        if (value == OUTOFRANGE)
        {   asmerror("invalid label for <");
            return 0;
        }
        return value / 256;
    acase '>': // low byte
        value = findlabel(&passedstring[1], length - 1, FALSE);
        if (value == OUTOFRANGE)
        {   asmerror("invalid label for >");
            return 0;
        }
        return value % 256;
    acase '(': // (FOO)H or (FOO)L
        if (passedstring[length - 2] != ')')
        {   asmerror(") expected");
            return 0;
        }
        if (passedstring[length - 1] == 'H' || passedstring[length - 1] == 'h')
        {   value = findlabel(&passedstring[1], length - 3, FALSE);
            if (value == OUTOFRANGE)
            {   asmerror("invalid label for ()H");
                return 0;
            }
            return value / 256;
        }
        if (passedstring[length - 1] == 'L' || passedstring[length - 1] == 'l')
        {   value = findlabel(&passedstring[1], length - 3, FALSE);
            if (value == OUTOFRANGE)
            {   asmerror("invalid label for ()L");
                return 0;
            }
            return value % 256;
        }
        asmerror("H or L expected");
        return 0;
    acase '$':
        if (style == STYLE_SIGNETICS1 || style == STYLE_SIGNETICS2)
        {   value = readnum_hex(&passedstring[1], length - 1);
            if (value == OUTOFRANGE)
            {   asmerror("invalid hex number");
                value = 0;
        }   }
        else // CALM I/O port, IEEE-694 relative address
        {   asmerror("unexpected $");
            value = 0;
        }
    acase '!':
        if (style == STYLE_SIGNETICS1 || style == STYLE_SIGNETICS2 || style == STYLE_OLDCALM || style == STYLE_NEWCALM)
        {   value = readnum_decimal(&passedstring[1], length - 1);
            if (value == OUTOFRANGE)
            {   asmerror("invalid decimal number");
                value = 0;
        }   }
        else // zero page address
        {   asmerror("unexpected !");
            value = 0;
        }
    acase '@':
        if (style == STYLE_SIGNETICS1 || style == STYLE_SIGNETICS2)
        {   value = readnum_octal(&passedstring[1], length - 1);
            if (value == OUTOFRANGE)
            {   asmerror("invalid octal number");
                value = 0;
        }   }
        else // indirection
        {   asmerror("unexpected @");
            value = 0;
        }
    acase '%':
        value = readnum_binary(&passedstring[1], length - 1);
        if (value == OUTOFRANGE)
        {   asmerror("invalid binary number");
            value = 0;
        }
    adefault:
        value = findlabel(passedstring, length, TRUE);
        if (value == OUTOFRANGE)
        {   if (ISQWERTY) // ISASCII is what we really mean in this case
            {   // we do this after readbase() so that FF with hex default base is interpreted as $FF, not as Form Feed ($0C)
                for (i = 0; i < 32; i++)
                {   if (!strnicmp(passedstring, asciiname_short[i], length))
                    {   return i;
            }   }   }
            if (assembling)
            {   switch (lastparse)
                {
                case  BASE_BINARY:  asmerror("invalid label or number (parsed as binary)");
                acase BASE_OCTAL:   asmerror("invalid label or number (parsed as octal)");
                acase BASE_DECIMAL: asmerror("invalid label or number (parsed as decimal)");
                acase BASE_HEX:     asmerror("invalid label or number (parsed as hexadecimal)");
                acase BASE_CHAR:    asmerror("invalid label or number (parsed as character)");
                acase BASE_EBCDIC:  asmerror("invalid label or number (parsed as EBCDIC)");
                acase BASE_LABEL:   asmerror("invalid label or number (parsed as label)");
                }
                value = 0;
    }   }   }

    return value;
}

MODULE FLAG asm_ieee(void)
{   FAST UBYTE byte1st,
               byte2nd,
               reg1, reg2;
    FAST int   i,
               value1;

    equstring[0] = EOS;

START:

    // Directives (except Equates)----------------------------------------

    if (LineBuffer[0] == EOS)
    {   ;
    } elif
    (   compareit("PAGE",  TRUE)
     || compareit("TITLE", TRUE)
    )
    {   asmwarning("ignoring directive");
    } elif (compareit("RES", TRUE))
    {   skipblanks(TRUE);
        ocursor += parsenumber(&LineBuffer[linecursor]);
    } elif
    (   compareit("DATA" , TRUE)
     || compareit("DATAB", TRUE)
    )
    {   getimmediatebytes();
    } elif
    (   compareit("DATAL", TRUE)
    )
    {   getimmediatewords(FALSE);
    } elif (compareit("END", TRUE)) // END <address> is unofficial for IEEE-694
    {   if (!skipblanks(FALSE))
        {   value1 = parsenumber(&LineBuffer[linecursor]);
            startaddr_h = value1 / 256;
            startaddr_l = value1 % 256;
            specified = TRUE;
        }
        ended = TRUE;
        return TRUE;
    } elif (compareit("ORG", TRUE))
    {   skipblanks(TRUE);
        value1 = parsenumber(&LineBuffer[linecursor]);
        if (value1 < ocursor)
        {   asmwarning("setting origin backwards");
        }
        orgy = TRUE;
        ocursor = value1;
    } elif (compareit("BASE", TRUE))
    {   skipblanks(TRUE);
        if   (compareit("B", TRUE)) defasmbase = BASE_BINARY;
        elif (compareit("Q", TRUE)) defasmbase = BASE_OCTAL;
        elif (compareit("D", TRUE)) defasmbase = BASE_DECIMAL;
        elif (compareit("H", TRUE)) defasmbase = BASE_HEX;
        else asmerror("value out of range");
    }

    // Opcodes------------------------------------------------------------

    elif   (compareit("CLR", TRUE)) // CLR .0  -> XOR .0,.r0   (EORZ r0)
    {   skipblanks(TRUE);
        if (!compareit(".0" , TRUE))
        {   asmerror(".0 expected");
        }
        emit(0x20);
    } elif (compareit("CLRC",TRUE)) // CLRC    -> AND  .L,~$01 (CPSL     $01)
    {   emit(0x75);
        emit(0x01);
    } elif (compareit("CLRV",TRUE)) // CLRV    -> AND  .L,~$04 (CPSL     $04)
    {   emit(0x75);
        emit(0x04);
    } elif (compareit("DEC", TRUE)) // DEC reg -> DBNZ reg,'H$ (BDRR,reg $)
    {   skipblanks(TRUE);
        emit(0xF8 + getreg_ieee()); // could also (less efficiently) be $FC + getreg_ieee() and then two zero bytes, but we have no way to distinguish them
        emit(0x00);
    } elif (compareit("DI" , TRUE)) // DI      -> OR   .U,$20  (PPSU     $20)
    {   emit(0x76);
        emit(0x20);
    } elif (compareit("EI" , TRUE)) // EI      -> AND  .U,~$20 (CPSU     $20)
    {   emit(0x74);
        emit(0x20);
    } elif (compareit("INC", TRUE)) // INC reg -> IBNZ reg,'H$ (BIRR,reg $)
    {   skipblanks(TRUE);
        emit(0xD8 + getreg_ieee()); // could also (less efficiently) be $DC + getreg_ieee() and then two zero bytes, but we have no way to distinguish them
        emit(0x00);
    } elif (compareit("NOT", TRUE)) // NOT reg -> XOR  reg,$FF (EORI,reg $FF)
    {   skipblanks(TRUE);
        emit(0x24 + getreg_ieee());
        emit(0xFF);
    } elif (compareit("SETC",TRUE)) // SETC    -> OR   .L,$01  (PPSL     $01)
    {   emit(0x77);
        emit(0x01);
    } elif (compareit("SETV",TRUE)) // SETV    -> OR   .L,$04  (PPSL     $04)
    {   emit(0x77);
        emit(0x04);
    } elif (compareit("MOV", TRUE))
    {   skipblanks(TRUE);
        if   (compareit(".0,.U", TRUE)) emit(0x12);
        elif (compareit(".0,.L", TRUE)) emit(0x13);
        elif (compareit(".U,.0", TRUE)) emit(0x92);
        elif (compareit(".L,.0", TRUE)) emit(0x93);
        else
        {   reg1 = getreg_ieee();
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            }
            reg2 = getreg_ieee();
            if   (reg1 == 0 && reg2 == 0) { emit(0x00); asmwarning(".0 for both opcodes is not allowed"); }
            elif (reg1 == 0 && reg2 == 1)   emit(0x01);
            elif (reg1 == 0 && reg2 == 2)   emit(0x02);
            elif (reg1 == 0 && reg2 == 3)   emit(0x03);
            elif (reg1 == 1 && reg2 == 0)   emit(0xC1);
            elif (reg1 == 2 && reg2 == 0)   emit(0xC2);
            elif (reg1 == 3 && reg2 == 0)   emit(0xC3);
            else asmerror("unsupported operation");
    }   }
    elif (compareit("LD", TRUE))
    {   skipblanks(TRUE);
        if (compareit(".L,/", FALSE))
        {   emit(0x10);
            getabsolute_branch(); // yes, branch!
            if (!supercpu)
            {   asmerror("2650B only");
        }   }
        else
        {   reg1 = getreg_ieee();
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            } elif (compareit("#", FALSE))
            {   value1 = fiximmediate(parsenumber(&LineBuffer[linecursor]));
                if (value1 < 0 || value1 > 255)
                {   asmerror("immediate value out of range");
                    value1 = 0;
                }
                emit(0x04 + reg1);
                emit((UBYTE) value1);
            } elif (compareit("$", FALSE))
            {   emit(0x08 + reg1);
                getrelative(FALSE);
            } elif (compareit("/", FALSE))
            {   getabsolute_nonbranch_ieee(0x0C, reg1);
            } else
            {   asmerror("#, $ or / expected");
    }   }   }
    elif (compareit("ST", TRUE))
    {   skipblanks(TRUE);
        if (compareit(".L,/", FALSE))
        {   emit(0x11);
            getabsolute_branch(); // yes, branch!
            if (!supercpu)
            {   asmerror("2650B only");
        }   }
        else
        {   reg1 = getreg_ieee();
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            } elif (compareit("$", FALSE))
            {   emit(0xC8 + reg1);
                getrelative(FALSE);
            } elif (compareit("/", FALSE))
            {   getabsolute_nonbranch_ieee(0xCC, reg1);
            } else
            {  asmerror("$ or / expected");
    }   }   }
    elif     (compareit("RETEQ" , TRUE ))   emit(0x14);
    elif     (compareit("RETGT" , TRUE ))   emit(0x15);
    elif     (compareit("RETLT" , TRUE ))   emit(0x16);
    elif     (compareit("RET"   , TRUE ))   emit(0x17);
    elif     (compareit("BEQ"   , TRUE ))
    {   skipblanks(TRUE);
        if   (compareit("$"     , FALSE)) { emit(0x1B); getrelative(FALSE);   }
        elif (compareit("/"     , FALSE)) { emit(0x1F); getabsolute_branch(); }
        else asmerror("$ or / expected");
    } elif   (compareit("BGT"   , TRUE ))
    {   skipblanks(TRUE);
        if   (compareit("$"     , FALSE)) { emit(0x1B); getrelative(FALSE);   }
        elif (compareit("/"     , FALSE)) { emit(0x1F); getabsolute_branch(); }
        else asmerror("$ or / expected");
    } elif   (compareit("BLT"   , TRUE ))
    {   skipblanks(TRUE);
        if   (compareit("$"     , FALSE)) { emit(0x1B); getrelative(FALSE);   }
        elif (compareit("/"     , FALSE)) { emit(0x1F); getabsolute_branch(); }
        else asmerror("$ or / expected");
    } elif   (compareit("BR"    , TRUE ))
    {   skipblanks(TRUE);
        if   (compareit("$"     , FALSE)) { emit(0x1B); getrelative(FALSE);   }
        elif (compareit("!"     , FALSE)) { emit(0x9B); getzeropage(FALSE);   }
        elif (compareit("/"     , FALSE))
        {   if (compareit("@", FALSE))
            {   byte1st = 0x80;
            } else
            {   byte1st = 0;
            }
            value1 = parsenumber(&LineBuffer[linecursor]);
            byte1st |= (value1 / 256);
            byte2nd =  (value1 % 256);
            if (compareit(",", FALSE))
            {   asmerror("unexpected comma");
            }
            if (compareit("(.3)", TRUE) || compareit("(.6)", TRUE))
            {   emit(0x9F);
            } else
            {   emit(0x1F);
            }
            emit(byte1st);
            emit(byte2nd);
        } else asmerror("$, ! or / expected");
    }
    elif     (compareit("XOR"   , TRUE ))
    {   skipblanks(TRUE);
        reg1 = getreg_ieee();
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        } elif (compareit("#", FALSE))
        {   emit(0x24 + reg1);
            getimmediate(FALSE);
        } elif (compareit("$", FALSE))
        {   emit(0x28 + reg1);
            getrelative(FALSE);
        } elif (compareit("/", FALSE))
        {   getabsolute_nonbranch_ieee(0x2C, reg1);
        } elif (reg1 == 0)
        {   reg2 = getreg_ieee();
            emit(0x20 + reg2);
        } else
        {   asmerror("unsupported operation");
    }   }
    elif     (compareit("IN"   , TRUE ))
    {   skipblanks(TRUE);
        reg1 = getreg_ieee();
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        } elif (compareit("CTRL", TRUE))
        {   emit(0x30 + reg1);
        } elif (compareit("DATA", TRUE))
        {   emit(0x70 + reg1);
        } else
        {   emit(0x54 + reg1);
            getimmediate(FALSE);
    }   }
    elif     (compareit("RETIEQ", TRUE ))   emit(0x34);
    elif     (compareit("RETIGT", TRUE ))   emit(0x35);
    elif     (compareit("RETILT", TRUE ))   emit(0x36);
    elif     (compareit("RETI"  , TRUE ))   emit(0x37);
    elif     (compareit("CALLEQ", TRUE ))
    {   skipblanks(TRUE);
        if   (compareit("$"     , FALSE)) { emit(0x38); getrelative(FALSE);   }
        elif (compareit("/"     , FALSE)) { emit(0x3C); getabsolute_branch(); }
        else asmerror("$ or / expected");
    } elif   (compareit("CALLGT", TRUE ))
    {   skipblanks(TRUE);
        if   (compareit("$"     , FALSE)) { emit(0x39); getrelative(FALSE);   }
        elif (compareit("/"     , FALSE)) { emit(0x3D); getabsolute_branch(); }
        else asmerror("$ or / expected");
    } elif   (compareit("CALLLT", TRUE ))
    {   skipblanks(TRUE);
        if   (compareit("$"     , FALSE)) { emit(0x3A); getrelative(FALSE);   }
        elif (compareit("/"     , FALSE)) { emit(0x3E); getabsolute_branch(); }
        else asmerror("$ or / expected");
    } elif   (compareit("CALL"  , TRUE ))
    {   skipblanks(TRUE);
        if   (compareit("$"     , FALSE)) { emit(0x3B); getrelative(FALSE);   }
        elif (compareit("!"     , FALSE)) { emit(0xBB); getzeropage(FALSE);   }
        elif (compareit("/"     , FALSE))
        {   if (compareit("@", FALSE))
            {   byte1st = 0x80;
            } else
            {   byte1st = 0;
            }
            value1 = parsenumber(&LineBuffer[linecursor]);
            byte1st |= (value1 / 256);
            byte2nd =  (value1 % 256);
            if (compareit(",", FALSE))
            {   asmerror("unexpected comma");
            }
            if (compareit("(.3)", TRUE) || compareit("(.6)", TRUE))
            {   emit(0xBF);
            } else
            {   emit(0x3F);
            }
            emit(byte1st);
            emit(byte2nd);
        } else asmerror("$, ! or / expected");
    }
    elif     (compareit("HALT"  , TRUE )  || compareit("WAIT"  , TRUE))
    {   emit(0x40);
    } elif   (compareit("AND"   , TRUE ))
    {   skipblanks(TRUE);
        if (compareit(".U,#", FALSE))
        {   emit(0x74);
            value1 = fiximmediate(parsenumber(&LineBuffer[linecursor]));
            if (value1 < 0 || value1 > 255)
            {   asmerror("immediate value out of range");
                emit(0);
            } else
            {   emit((UBYTE) ~value1);
        }   }
        elif (compareit(".L,#", FALSE))
        {   emit(0x75);
            value1 = fiximmediate(parsenumber(&LineBuffer[linecursor]));
            if (value1 < 0 || value1 > 255)
            {   asmerror("immediate value out of range");
                emit(0);
            } else
            {   emit((UBYTE) ~value1);
        }   }
        else
        {   reg1 = getreg_ieee();
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            } elif (compareit("#", FALSE))
            {   emit(0x44 + reg1);
                getimmediate(FALSE);
            } elif (compareit("$", FALSE))
            {   emit(0x48 + reg1);
                getrelative(FALSE);
            } elif (compareit("/", FALSE))
            {   getabsolute_nonbranch_ieee(0x4C, reg1);
            } elif (reg1 == 0)
            {   reg2 = getreg_ieee();
                if (reg2 == 0)
                {   asmerror("unsupported operation");
                } else
                {   emit(0x40 + reg2); // $41..$43
            }   }
            else
            {   asmerror("unsupported operation");
    }   }   }
    elif     (compareit("ROR"   , TRUE ) || compareit("RORC", TRUE) || compareit("SHR", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_ieee();
        emit(0x50 + reg1);
    } elif   (compareit("BNZ"   , TRUE ))
    {   skipblanks(TRUE);
        reg1 = getreg_ieee();
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        } elif (compareit("$", FALSE))
        {   emit(0x58 + reg1);
            getrelative(FALSE);
        } elif (compareit("/", FALSE))
        {   emit(0x5C + reg1);
            getabsolute_branch();
        } else
        {   asmerror("$ or / expected");
    }   }
    elif   (compareit("OR"    , TRUE ))
    {   skipblanks(TRUE);
        if     (compareit(".U,#", FALSE))
        {   emit(0x76);
            getimmediate(FALSE);
        } elif (compareit(".L,#", FALSE))
        {   emit(0x77);
            getimmediate(FALSE);
        } else
        {   reg1 = getreg_ieee();
            if (!compareit(",", FALSE))
            {   asmerror("comma expected");
            } elif (compareit("#", FALSE))
            {   emit(0x64 + reg1);
                getimmediate(FALSE);
            } elif (compareit("$", FALSE))
            {   emit(0x68 + reg1);
                getrelative(FALSE);
            } elif (compareit("/", FALSE))
            {   getabsolute_nonbranch_ieee(0x6C, reg1);
            } elif (reg1 == 0)
            {   reg2 = getreg_ieee();
                emit(0x60 + reg2); // $60..$63
            } else
            {   asmerror("unsupported operation");
    }   }   }
    elif     (compareit("CALLNZ", TRUE ))
    {   skipblanks(TRUE);
        reg1 = getreg_ieee();
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        } elif (compareit("$", FALSE))
        {   emit(0x78 + reg1);
            getrelative(FALSE);
        } elif (compareit("/", FALSE))
        {   emit(0x7C + reg1);
            getabsolute_branch();
        } else
        {   asmerror("$ or / expected");
    }   }
    elif   (compareit("ADD"   , TRUE ) || compareit("ADDC", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_ieee();
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        } elif (compareit("#", FALSE))
        {   emit(0x84 + reg1);
            getimmediate(FALSE);
        } elif (compareit("$", FALSE))
        {   emit(0x88 + reg1);
            getrelative(FALSE);
        } elif (compareit("/", FALSE))
        {   getabsolute_nonbranch_ieee(0x8C, reg1);
        } elif (reg1 == 0)
        {   reg2 = getreg_ieee();
            emit(0x80 + reg2); // $80..$83
        } else
        {   asmerror("unsupported operation");
    }   }
    elif     (compareit("ADJ"   , TRUE ))
    {   skipblanks(TRUE);
        reg1 = getreg_ieee();
        emit(0x94 + reg1);
    } elif   (compareit("BNE"   , TRUE ))
    {   skipblanks(TRUE);
        if   (compareit("$"     , FALSE)) { emit(0x98); getrelative(FALSE);   }
        elif (compareit("/"     , FALSE)) { emit(0x9C); getabsolute_branch(); }
        else asmerror("$ or / expected");
    } elif   (compareit("BLE"   , TRUE ))
    {   skipblanks(TRUE);
        if   (compareit("$"     , FALSE)) { emit(0x99); getrelative(FALSE);   }
        elif (compareit("/"     , FALSE)) { emit(0x9D); getabsolute_branch(); }
        else asmerror("$ or / expected");
    } elif   (compareit("BGE"   , TRUE ))
    {   skipblanks(TRUE);
        if   (compareit("$"     , FALSE)) { emit(0x9A); getrelative(FALSE);   }
        elif (compareit("/"     , FALSE)) { emit(0x9E); getabsolute_branch(); }
        else asmerror("$ or / expected");
    } elif   (compareit("SUB"   , TRUE ) || compareit("SUBC", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_ieee();
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        } elif (compareit("#", FALSE))
        {   emit(0xA4 + reg1);
            getimmediate(FALSE);
        } elif (compareit("$", FALSE))
        {   emit(0xA8 + reg1);
            getrelative(FALSE);
        } elif (compareit("/", FALSE))
        {   getabsolute_nonbranch_ieee(0xAC, reg1);
        } elif (reg1 == 0)
        {   reg2 = getreg_ieee();
            emit(0xA0 + reg2); // $A0..$A3
        } else
        {   asmerror("unsupported operation");
    }   }
    elif     (compareit("OUT"   , TRUE ))
    {   skipblanks(TRUE);
        reg1 = getreg_ieee();
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        } elif (compareit("CTRL", TRUE ))
        {   emit(0xB0 + reg1);
        } elif (compareit("DATA", TRUE ))
        {   emit(0xF0 + reg1);
        } else
        {   emit(0xD4 + reg1);
            getimmediate(FALSE);
    }   }
    elif     (compareit("TEST"  , TRUE ))
    {   skipblanks(TRUE);
        if (compareit(".U,#",     FALSE))
        {   emit(0xB4);
            getimmediate(FALSE);
        } elif (compareit(".L,#", FALSE))
        {   emit(0xB5);
            getimmediate(FALSE);
        } else
        {   reg1 = getreg_ieee();
            if (!compareit(",#",  FALSE))
            {   asmerror(",# expected");
            }
            emit(0xF4 + reg1);
            getimmediate(FALSE);
    }   }
    elif     (compareit("CALLNE", TRUE ))
    {   skipblanks(TRUE);
        if   (compareit("$"     , FALSE)) { emit(0xB8); getrelative(FALSE);   }
        elif (compareit("/"     , FALSE)) { emit(0xBC); getabsolute_branch(); }
        else asmerror("$ or / expected");
    } elif   (compareit("CALLLE", TRUE ))
    {   skipblanks(TRUE);
        if   (compareit("$"     , FALSE)) { emit(0xB9); getrelative(FALSE);   }
        elif (compareit("/"     , FALSE)) { emit(0xBD); getabsolute_branch(); }
        else asmerror("$ or / expected");
    } elif   (compareit("CALLGE", TRUE ))
    {   skipblanks(TRUE);
        if   (compareit("$"     , FALSE)) { emit(0xBA); getrelative(FALSE);   }
        elif (compareit("/"     , FALSE)) { emit(0xBE); getabsolute_branch(); }
        else asmerror("$ or / expected");
    } elif   (compareit("NOP"   , FALSE))   emit(0xC0);
    elif     (compareit("ROL"   , TRUE )  || compareit("ROLC", TRUE) || compareit("SHL", TRUE) || compareit("SHLA", TRUE))
    {   skipblanks(TRUE);
        reg1 = getreg_ieee();
        emit(0xD0 + reg1);
    } elif   (compareit("IBNZ"  , TRUE ))
    {   skipblanks(TRUE);
        reg1 = getreg_ieee();
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        } elif (compareit("$"   , FALSE)) { emit(0xD8 + reg1); getrelative(FALSE);   }
        elif (compareit("/"     , FALSE)) { emit(0xDC + reg1); getabsolute_branch(); }
        else asmerror("$ or / expected");
    } elif   (compareit("CMP"   , TRUE ))
    {   skipblanks(TRUE);
        reg1 = getreg_ieee();
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        } elif (compareit("#", FALSE))
        {   emit(0xE4 + reg1);
            getimmediate(FALSE);
        } elif (compareit("$", FALSE))
        {   emit(0xE8 + reg1);
            getrelative(FALSE);
        } elif (compareit("/", FALSE))
        {   getabsolute_nonbranch_ieee(0xEC, reg1);
        } elif (reg1 == 0)
        {   reg2 = getreg_ieee();
            emit(0xE0 + reg2); // $A0..$A3
        } else
        {   asmerror("unsupported operation");
    }   }
    elif     (compareit("DBNZ"  , TRUE ))
    {   skipblanks(TRUE);
        reg1 = getreg_ieee();
        if (!compareit(",", FALSE))
        {   asmerror("comma expected");
        } elif (compareit("$"   , FALSE)) { emit(0xF8 + reg1); getrelative(FALSE);   }
        elif (compareit("/"     , FALSE)) { emit(0xFC + reg1); getabsolute_branch(); }
        else asmerror("$ or / expected");
    }

    // Labels and Equates-------------------------------------------------

    elif (equstring[0] || linecursor != 0)
    {   asmerror("syntax error");
        return FALSE;
    } else
    {   i = 0;
        while
        (   LineBuffer[linecursor] != ' '
         && LineBuffer[linecursor] != HT
         && LineBuffer[linecursor] != EOS
         && LineBuffer[linecursor] != ':'
      // && LineBuffer[linecursor] != ',' (we don't allow this for CALM)
      // && LineBuffer[linecursor] != '>' (we don't allow this for CALM)
         && i <= LABELLIMIT
        )
        {   equstring[i++] = LineBuffer[linecursor++];
        }
        equstring[i] = EOS;
        if
        (   LineBuffer[linecursor] == ':' // label with colon
      // || LineBuffer[linecursor] == ',' // label with comma (X2650 cross-asm) (we don't allow this for CALM)
      // || LineBuffer[linecursor] == '>' // label with > (PIP4K source code) (we don't allow this for CALM)
        )
        {   linecursor++; // to get past it
        } elif (i > LABELLIMIT)
        {   asmerror("label too long");
            return FALSE;
        }

        skipblanks(FALSE);
        if (compareit("EQU", TRUE))
        {   skipblanks(TRUE);
            equmode = TRUE;
            value1 = parsenumber(&LineBuffer[linecursor]);
            equmode = FALSE;
            if (pass == 1)
            {   adduserlabel((STRPTR) equstring, (UWORD) value1, UNKNOWN);
        }   }
        else
        {   adduserlabel((STRPTR) equstring, (UWORD) ocursor, UNKNOWN);
            goto START;
    }   }

    return TRUE;
}

MODULE FLAG isterminator(TEXT thechar)
{   if
    (   thechar == EOS
     || thechar == CR
     || thechar == LF
     || thechar == ' '
     || thechar == HT
     || thechar == ';'
     || thechar == ':'
     || thechar == '.'
     || thechar == '}'
     || (assembling && thechar == ',') // so that eg. "E 1234 BCTA,UN" works
    )
    {   return TRUE;
    } // implied else
    return FALSE;
}
