// 1. INCLUDES------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
    #include <proto/locale.h>  // GetCatalogStr()
    #include <proto/dos.h>     // Write()
#endif
#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
    #include "resource.h"
#endif

#include <stdio.h>             // zprintf()
#include <stdlib.h>            // for malloc(), calloc(), free(), rand()
#include <string.h>            // strcpy()

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"

// 2. DEFINES-------------------------------------------------------------

#define CCMODE_UNDEFINED  0
#define CCMODE_COMPARE    1
#define CCMODE_ARITHMETIC 2
#define CCMODE_TEST       3

// 3. EXPORTED VARIABLES--------------------------------------------------

EXPORT char  mn[256 + 1];
EXPORT int   disassembling = 0,
             nextdis       = 0;
EXPORT FILE* DisHandle     = NULL;

EXPORT int table_duration_2650[256] =
{   6, 6, 6, 6, 6, 6, 6, 6, 9, 9, 9, 9, 12, 12, 12, 12, //   0..15  (LOD)
   12,12, 6, 6, 9, 9, 9, 9, 9, 9, 9, 9,  9,  9,  9,  9, //  16..31
    6, 6, 6, 6, 6, 6, 6, 6, 9, 9, 9, 9, 12, 12, 12, 12, //  32..47  (EOR)
    6, 6, 6, 6, 9, 9, 9, 9, 9, 9, 9, 9,  9,  9,  9,  9, //  48..63
    3, 6, 6, 6, 6, 6, 6, 6, 9, 9, 9, 9, 12, 12, 12, 12, //  64..79  (HALT/AND)
    6, 6, 6, 6, 9, 9, 9, 9, 9, 9, 9, 9,  9,  9,  9,  9, //  80..95  (RRR/REDE/BRNR/BRNA)
    6, 6, 6, 6, 6, 6, 6, 6, 9, 9, 9, 9, 12, 12, 12, 12, //  96..111 (IOR)
    6, 6, 6, 6, 9, 9, 9, 9, 9, 9, 9, 9,  9,  9,  9,  9, // 112..127
    6, 6, 6, 6, 6, 6, 6, 6, 9, 9, 9, 9, 12, 12, 12, 12, // 128..143 (ADD)
    6, 6, 6, 6, 9, 9, 9, 9, 9, 9, 9, 9,  9,  9,  9,  9, // 144..159
    6, 6, 6, 6, 6, 6, 6, 6, 9, 9, 9, 9, 12, 12, 12, 12, // 160..175 (SUB)
    6, 6, 6, 6, 9, 9, 6, 6, 9, 9, 9, 9,  9,  9,  9,  9, // 176..191
    6, 6, 6, 6, 6, 6, 6, 6, 9, 9, 9, 9, 12, 12, 12, 12, // 192..207 (NOP/STR)
    6, 6, 6, 6, 9, 9, 9, 9, 9, 9, 9, 9,  9,  9,  9,  9, // 208..223 (RRL/WRTE/BIRR/BIRA)
    6, 6, 6, 6, 6, 6, 6, 6, 9, 9, 9, 9, 12, 12, 12, 12, // 224..239 (COM)
    6, 6, 6, 6, 9, 9, 9, 9, 9, 9, 9, 9,  9,  9,  9,  9  // 240..255 (WRTD/TMI/BDRR/BDRA)
};

EXPORT struct FlagNameStruct flagname[CPUTIPS] = {
{ { "r0"    , "r0"    , "A"                      , "r0"  , ".0"     }, { "Register 0"                  , "Register 0"                  , "Register A"             , "Register 0"                  , "Register 0"              } }, //  0 NAME_R0
{ { "r1"    , "r1"    , "B"                      , "r1"  , ".1"     }, { "Register 1"                  , "Register 1"                  , "Register B"             , "Register 1"                  , "Register 1"              } }, //  1 NAME_R1
{ { "r2"    , "r2"    , "C"                      , "r2"  , ".2"     }, { "Register 2"                  , "Register 2"                  , "Register C"             , "Register 2"                  , "Register 2"              } }, //  2 NAME_R2
{ { "r3"    , "r3"    , "D"                      , "r3"  , ".3"     }, { "Register 3"                  , "Register 3"                  , "Register D"             , "Register 3"                  , "Register 3"              } }, //  3 NAME_R3
{ { "r4"    , "r4"    , "B'"                     , "r4"  , ".4"     }, { "Register 4"                  , "Register 4"                  , "Register B'"            , "Register 4"                  , "Register 4"              } }, //  4 NAME_R4
{ { "r5"    , "r5"    , "C'"                     , "r5"  , ".5"     }, { "Register 5"                  , "Register 5"                  , "Register C'"            , "Register 5"                  , "Register 5"              } }, //  5 NAME_R5
{ { "r6"    , "r6"    , "D'"                     , "r6"  , ".6"     }, { "Register 6"                  , "Register 6"                  , "Register D'"            , "Register 6"                  , "Register 6"              } }, //  6 NAME_R6
{ { "RAS0"  , "RAS0"  , "RAS0"                   , "RAS0", ".RAS0"  }, { "Return Address Stack #0"     , "Return Address Stack #0"     , "Return Address Stack #0", "Return Address Stack #0"     , "Return Address Stack #0" } }, //  7 NAME_RAS0
{ { "RAS1"  , "RAS1"  , "RAS1"                   , "RAS1", ".RAS1"  }, { "Return Address Stack #1"     , "Return Address Stack #1"     , "Return Address Stack #1", "Return Address Stack #1"     , "Return Address Stack #1" } }, //  8 NAME_RAS1
{ { "RAS2"  , "RAS2"  , "RAS2"                   , "RAS2", ".RAS2"  }, { "Return Address Stack #2"     , "Return Address Stack #2"     , "Return Address Stack #2", "Return Address Stack #2"     , "Return Address Stack #2" } }, //  9 NAME_RAS2
{ { "RAS3"  , "RAS3"  , "RAS3"                   , "RAS3", ".RAS3"  }, { "Return Address Stack #3"     , "Return Address Stack #3"     , "Return Address Stack #3", "Return Address Stack #3"     , "Return Address Stack #3" } }, // 10 NAME_RAS3
{ { "RAS4"  , "RAS4"  , "RAS4"                   , "RAS4", ".RAS4"  }, { "Return Address Stack #4"     , "Return Address Stack #4"     , "Return Address Stack #4", "Return Address Stack #4"     , "Return Address Stack #4" } }, // 11 NAME_RAS4
{ { "RAS5"  , "RAS5"  , "RAS5"                   , "RAS5", ".RAS5"  }, { "Return Address Stack #5"     , "Return Address Stack #5"     , "Return Address Stack #5", "Return Address Stack #5"     , "Return Address Stack #5" } }, // 12 NAME_RAS5
{ { "RAS6"  , "RAS6"  , "RAS6"                   , "RAS6", ".RAS6"  }, { "Return Address Stack #6"     , "Return Address Stack #6"     , "Return Address Stack #6", "Return Address Stack #6"     , "Return Address Stack #6" } }, // 13 NAME_RAS6
{ { "RAS7"  , "RAS7"  , "RAS7"                   , "RAS7", ".RAS7"  }, { "Return Address Stack #7"     , "Return Address Stack #7"     , "Return Address Stack #7", "Return Address Stack #7"     , "Return Address Stack #7" } }, // 14 NAME_RAS7
{ { "S"     , "S"     , "INPUT"                  , "INPUT",".S"     }, { "Sense"                       , "Sense"                       , "Input"                  , "Input"                       , "Sense"                   } }, // 15 NAME_S
{ { "F"     , "F"     , "OUTPUT"                 , "OUTPUT",".F"    }, { "Flag"                        , "Flag"                        , "Output"                 , "Output"                      , "Flag"                    } }, // 16 NAME_F
{ { "II"    , "II"    , "IOF"                    , "IOF" , ".I"     }, { "Interrupt Inhibit"           , "Interrupt Inhibit"           , "Interrupt Off"          , "Interrupt Off"               , "Interrupt Inhibit"       } }, // 17 NAME_II
{ { "UF1"   , "UF1"   , "UF1"                    , "UF1" , ".UF1"   }, { "User Flag #1"                , "User Flag #1"                , "User Flag #1"           , "User Flag #1"                , "User Flag #1"            } }, // 18 NAME_USERFLAG1
{ { "UF2"   , "UF2"   , "UF2"                    , "UF2" , ".UF2"   }, { "User Flag #2"                , "User Flag #2"                , "User Flag #2"           , "User Flag #2"                , "User Flag #2"            } }, // 19 NAME_USERFLAG2
{ { "SP"    , "SP"    , "STACK"                  , "SP"  , ".SP"    }, { "Stack Pointer"               , "Stack Pointer"               , "Stack Pointer"          , "Stack Pointer"               , "Stack Pointer"           } }, // 20 NAME_SP
{ { "CC"    , "CC"    , "CC"                     , "CC"  , ".CC"    }, { "Condition Code"              , "Condition Code"              , "Condition Code"         , "Condition Code"              , "Condition Code"          } }, // 21 NAME_CC
{ { "IDC"   , "IDC"   , "HALFCARRY" /* or H/X */ , "HALFCARRY",".IDC"},{ "Inter-Digit Carry"           , "Inter-Digit Carry"           , "Half Carry"             , "Half Carry"                  , "Inter-Digit Carry"       } }, // 22 NAME_IDC
{ { "RS"    , "RS"    , "BANK1"     /* or B   */ , "BANK1",".RS"    }, { "Register Select"             , "Register Select"             , "Register Bank"          , "Register Bank"               , "Register Select"         } }, // 23 NAME_RS
{ { "WC"    , "WC"    , "WITHCARRY" /* or W   */ , "WITHCARRY",".WC"}, { "With Carry"                  , "With Carry"                  , "With Carry"             , "With Carry"                  , "With Carry"              } }, // 24 NAME_WC
{ { "OVF"   , "OVF"   , "OVERFLOW"  /* or V   */ , "OVERFLOW",".V"  }, { "Overflow"                    , "Overflow"                    , "Overflow"               , "Overflow"                    , "Overflow"                } }, // 25 NAME_OVF
{ { "COM"   , "COM"   , "LOGICOMP"  /* or L   */ , "LOGICOMP",".COM"}, { "Compare"                     , "Compare"                     , "Logical Compare"        , "Logical Compare"             , "Compare"                 } }, // 26 NAME_COM
{ { "C"     , "C"     , "CARRY"     /* or C   */ , "CARRY",".C"     }, { "Carry"                       , "Carry"                       , "Carry"                  , "Carry"                       , "Carry"                   } }, // 27 NAME_C
{ { "IAR"   , "IAR"   , "PC"                     , "PC"  , ".PC"    }, { "Instruction Address Register", "Instruction Address Register", "Program Counter"        , "Program Counter"             , "Program Counter"         } }, // 28 NAME_IAR
{ { "Opcode", "Opcode", "Opcode"                 , "Opcode","Opcode"}, { "Operation Code"              , "Operation Code"              , "Operation Code"         , "Operation Code"              , "Operation Code"          } }, // 29 NAME_OPCODE
};

EXPORT const STRPTR hexchars[STYLES] = { "'H'", "'H'", "16'", "16'", "H'" }, // EMU USES $
                    decchars[STYLES] = { "'D'", "'D'", "10'", "10'", "D'" }, // EMU USES !
                    octchars[STYLES] = { "'O'", "'O'", "8'" , "8'" , "Q'" }, // EMU USES @
                    binchars[STYLES] = { "'B'", "'B'", "2'" , "2'" , "B'" }; // EMU USES %

EXPORT struct OpcodeStruct opcodes[STYLES][256] = { {
// Signetics non-extended format
{ "LODZ r0", "",              ";"                                    , "Load from Register Zero"                                 , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $00 indeterminate! or, "r0 = r0;"
{ "LODZ r1", "",              "r0 = r1;"                             , "Load from Register Zero"                                 , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $01
{ "LODZ r2", "",              "r0 = r2;"                             , "Load from Register Zero"                                 , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $02
{ "LODZ r3", "",              "r0 = r3;"                             , "Load from Register Zero"                                 , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $03
{ "LODI,r0", "$vv",           "r0 = $vv;"                            , "Load Immediate"                                          , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $04
{ "LODI,r1", "$vv",           "r1 = $vv;"                            , "Load Immediate"                                          , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $05
{ "LODI,r2", "$vv",           "r2 = $vv;"                            , "Load Immediate"                                          , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $06
{ "LODI,r3", "$vv",           "r3 = $vv;"                            , "Load Immediate"                                          , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $07
{ "LODR,r0", "$aaaa",         "r0 = *($aaaa);"                       , "Load Relative"                                           , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $08
{ "LODR,r1", "$aaaa",         "r1 = *($aaaa);"                       , "Load Relative"                                           , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $09
{ "LODR,r2", "$aaaa",         "r2 = *($aaaa);"                       , "Load Relative"                                           , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $0A
{ "LODR,r3", "$aaaa",         "r3 = *($aaaa);"                       , "Load Relative"                                           , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $0B
{ "LODA,r0", "$aaaa",         "r0 = *($aaaa);"                       , "Load Absolute"                                           , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $0C
{ "LODA,r1", "$aaaa",         "r1 = *($aaaa);"                       , "Load Absolute"                                           , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $0D
{ "LODA,r2", "$aaaa",         "r2 = *($aaaa);"                       , "Load Absolute"                                           , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $0E
{ "LODA,r3", "$aaaa",         "r3 = *($aaaa);"                       , "Load Absolute"                                           , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $0F
{ "LDPL",    "$aaaa",         "PSL = *($aaaa);"                      , "Load Program Status, Lower, Absolute"                    , "None"                        , "CC, IDC, RS, WC, OVF, COM, C", "CC = (*($aaaa) & $C0) >> 6;"                                        }, // $10 2650B only
{ "STPL",    "$aaaa",         "*($aaaa) = PSL;"                      , "Store Program Status, Lower, Absolute"                   , "CC, IDC, RS, WC, OVF, COM, C", "None"                , "Undefined"                                                                  }, // $11 2650B only
{ "SPSU",    "",              "r0 = PSU;"                            , "Store Program Status, Upper"                             , "S, F, II, UF1, UF2, SP"      , "CC"                  , "if (S) CC = LT; elif (PSU == 0) CC = EQ; else CC = GT;"                     }, // $12
{ "SPSL",    "",              "r0 = PSL;"                            , "Store Program Status, Lower to Register Zero"            , "CC, IDC, RS, WC, OVF, COM, C", "CC"                  , "if (CC == LT || CC == UN) CC = LT; elif (PSL == 0) CC = EQ; else CC = GT;"  }, // $13
{ "RETC,eq", "",              "if CC == EQ return;"                  , "Return from Subroutine, Conditional"                     , "SP, CC"                      , "SP"                  , "Unchanged"                                                                  }, // $14 or "if (PSL & $C0) == $00 return;", or "if == return;"
{ "RETC,gt", "",              "if CC == GT return;"                  , "Return from Subroutine, Conditional"                     , "SP, CC"                      , "SP"                  , "Unchanged"                                                                  }, // $15 or "if (PSL & $C0) == $40 return;", or "if > return;"
{ "RETC,lt", "",              "if CC == LT return;"                  , "Return from Subroutine, Conditional"                     , "SP, CC"                      , "SP"                  , "Unchanged"                                                                  }, // $16 or "if (PSL & $C0) == $80 return;", or "if < return;"
{ "RETC,un", "",              "return;"                              , "Return from Subroutine"                                  , "SP"                          , "SP"                  , "Unchanged"                                                                  }, // $17
{ "BCTR,eq", "$aaaa",         "if CC == EQ goto $aaaa;"              , "Branch on Condition True, Relative"                      , "CC"                          , "None"                , "Unchanged"                                                                  }, // $18 or "if (PSL & $C0) == $00 goto $aaaa;", or "if == goto $aaaa;"
{ "BCTR,gt", "$aaaa",         "if CC == GT goto $aaaa;"              , "Branch on Condition True, Relative"                      , "CC"                          , "None"                , "Unchanged"                                                                  }, // $19 or "if (PSL & $C0) == $40 goto $aaaa;", or "if > goto $aaaa;"
{ "BCTR,lt", "$aaaa",         "if CC == LT goto $aaaa;"              , "Branch on Condition True, Relative"                      , "CC"                          , "None"                , "Unchanged"                                                                  }, // $1A or "if (PSL & $C0) == $80 goto $aaaa;", or "if < goto $aaaa;"
{ "BCTR,un", "$aaaa",         "goto $aaaa;"                          , "Branch Relative"                                         , "None"                        , "None"                , "Unchanged"                                                                  }, // $1B
{ "BCTA,eq", "$aaaa",         "if CC == EQ goto $aaaa;"              , "Branch on Condition True, Absolute"                      , "CC"                          , "None"                , "Unchanged"                                                                  }, // $1C or "if (PSL & $C0) == $00 goto $aaaa;", or "if == goto $aaaa;"
{ "BCTA,gt", "$aaaa",         "if CC == GT goto $aaaa;"              , "Branch on Condition True, Absolute"                      , "CC"                          , "None"                , "Unchanged"                                                                  }, // $1D or "if (PSL & $C0) == $40 goto $aaaa;", or "if > goto $aaaa;"
{ "BCTA,lt", "$aaaa",         "if CC == LT goto $aaaa;"              , "Branch on Condition True, Absolute"                      , "CC"                          , "None"                , "Unchanged"                                                                  }, // $1E or "if (PSL & $C0) == $80 goto $aaaa;", or "if < goto $aaaa;"
{ "BCTA,un", "$aaaa",         "goto $aaaa;"                          , "Branch Absolute"                                         , "None"                        , "None"                , "Unchanged"                                                                  }, // $1F
{ "EORZ r0", "",              "r0 = 0;"                              , "Exclusive OR to Register Zero"                           , "None"                        , "CC"                  , "CC = EQ;"                                                                   }, // $20 or "r0 ^= r0;"
{ "EORZ r1", "",              "r0 ^= r1;"                            , "Exclusive OR to Register Zero"                           , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $21
{ "EORZ r2", "",              "r0 ^= r2;"                            , "Exclusive OR to Register Zero"                           , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $22
{ "EORZ r3", "",              "r0 ^= r3;"                            , "Exclusive OR to Register Zero"                           , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $23
{ "EORI,r0", "$vv",           "r0 ^= $vv;"                           , "Exclusive OR Immediate"                                  , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $24
{ "EORI,r1", "$vv",           "r1 ^= $vv;"                           , "Exclusive OR Immediate"                                  , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $25
{ "EORI,r2", "$vv",           "r2 ^= $vv;"                           , "Exclusive OR Immediate"                                  , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $26
{ "EORI,r3", "$vv",           "r3 ^= $vv;"                           , "Exclusive OR Immediate"                                  , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $27
{ "EORR,r0", "$aaaa",         "r0 ^= *($aaaa);"                      , "Exclusive OR Relative"                                   , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $28
{ "EORR,r1", "$aaaa",         "r1 ^= *($aaaa);"                      , "Exclusive OR Relative"                                   , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $29
{ "EORR,r2", "$aaaa",         "r2 ^= *($aaaa);"                      , "Exclusive OR Relative"                                   , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $2A
{ "EORR,r3", "$aaaa",         "r3 ^= *($aaaa);"                      , "Exclusive OR Relative"                                   , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $2B
{ "EORA,r0", "$aaaa",         "r0 ^= *($aaaa);"                      , "Exclusive OR Absolute"                                   , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $2C
{ "EORA,r1", "$aaaa",         "r1 ^= *($aaaa);"                      , "Exclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $2D
{ "EORA,r2", "$aaaa",         "r2 ^= *($aaaa);"                      , "Exclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $2E
{ "EORA,r3", "$aaaa",         "r3 ^= *($aaaa);"                      , "Exclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $2F
{ "REDC,r0", "",              "r0 = IOPORT(CTRL);"                   , "Read Control"                                            , "None"                        , "CC"                  , "if (IOPORT(CTRL) & $80) CC = LT; elif (IOPORT(CTRL) == 0) CC = EQ; else CC = GT;" }, // $30
{ "REDC,r1", "",              "r1 = IOPORT(CTRL);"                   , "Read Control"                                            , "RS"                          , "CC"                  , "if (IOPORT(CTRL) & $80) CC = LT; elif (IOPORT(CTRL) == 0) CC = EQ; else CC = GT;" }, // $31
{ "REDC,r2", "",              "r2 = IOPORT(CTRL);"                   , "Read Control"                                            , "RS"                          , "CC"                  , "if (IOPORT(CTRL) & $80) CC = LT; elif (IOPORT(CTRL) == 0) CC = EQ; else CC = GT;" }, // $32
{ "REDC,r3", "",              "r3 = IOPORT(CTRL);"                   , "Read Control"                                            , "RS"                          , "CC"                  , "if (IOPORT(CTRL) & $80) CC = LT; elif (IOPORT(CTRL) == 0) CC = EQ; else CC = GT;" }, // $33
{ "RETE,eq", "",              "if CC == EQ then { PSU &= ~PSU_II; return; }","Return from Subroutine and Enable Interrupt, Conditional","SP, CC"                 , "II, SP"              , "Unchanged"                                                                  }, // $34 or "if (PSL & $C0) == $00 { PSU &= $DF; return; }", or "if == then { PSU &= ~PSU_II; return; }"
{ "RETE,gt", "",              "if CC == GT then { PSU &= ~PSU_II; return; }","Return from Subroutine and Enable Interrupt, Conditional","SP, CC"                 , "II, SP"              , "Unchanged"                                                                  }, // $35 or "if (PSL & $C0) == $40 { PSU &= $DF; return; }", or "if > then { PSU &= ~PSU_II; return; }"
{ "RETE,lt", "",              "if CC == LT then { PSU &= ~PSU_II; return; }","Return from Subroutine and Enable Interrupt, Conditional","SP, CC"                 , "II, SP"              , "Unchanged"                                                                  }, // $36 or "if (PSL & $C0) == $80 { PSU &= $DF; return; }", or "if < then { PSU &= ~PSU_II; return; }"
{ "RETE,un", "",              "PSU &= ~PSU_II; return;"              , "Return from Subroutine and Enable Interrupt"             , "SP"                          , "II, SP"              , "Unchanged"                                                                  }, // $37 or "PSU &= $DF; return;"
{ "BSTR,eq", "$aaaa",         "if CC == EQ gosub $aaaa;"             , "Branch to Subroutine on Condition True, Relative"        , "SP, CC"                      , "SP"                  , "Unchanged"                                                                  }, // $38 or "if (PSL & $C0) == $00 gosub $aaaa;", or "if == gosub $aaaa;"
{ "BSTR,gt", "$aaaa",         "if CC == GT gosub $aaaa;"             , "Branch to Subroutine on Condition True, Relative"        , "SP, CC"                      , "SP"                  , "Unchanged"                                                                  }, // $39 or "if (PSL & $C0) == $40 gosub $aaaa;", or "if > gosub $aaaa;"
{ "BSTR,lt", "$aaaa",         "if CC == LT gosub $aaaa;"             , "Branch to Subroutine on Condition True, Relative"        , "SP, CC"                      , "SP"                  , "Unchanged"                                                                  }, // $3A or "if (PSL & $C0) == $80 gosub $aaaa;", or "if < gosub $aaaa;"
{ "BSTR,un", "$aaaa",         "gosub $aaaa;"                         , "Branch to Subroutine, Relative"                          , "SP"                          , "SP"                  , "Unchanged"                                                                  }, // $3B
{ "BSTA,eq", "$aaaa",         "if CC == EQ gosub $aaaa;"             , "Branch to Subroutine on Condition True, Absolute"        , "SP, CC"                      , "SP"                  , "Unchanged"                                                                  }, // $3C or "if (PSL & $C0) == $00 gosub $aaaa;", or "if == gosub $aaaa;"
{ "BSTA,gt", "$aaaa",         "if CC == GT gosub $aaaa;"             , "Branch to Subroutine on Condition True, Absolute"        , "SP, CC"                      , "SP"                  , "Unchanged"                                                                  }, // $3D or "if (PSL & $C0) == $40 gosub $aaaa;", or "if > gosub $aaaa;"
{ "BSTA,lt", "$aaaa",         "if CC == LT gosub $aaaa;"             , "Branch to Subroutine on Condition True, Absolute"        , "SP, CC"                      , "SP"                  , "Unchanged"                                                                  }, // $3E or "if (PSL & $C0) == $80 gosub $aaaa;", or "if < gosub $aaaa;"
{ "BSTA,un", "$aaaa",         "gosub $aaaa;"                         , "Branch to Subroutine, Absolute"                          , "SP"                          , "SP"                  , "Unchanged"                                                                  }, // $3F
{ "HALT",    "",              "for (;;);"                            , "Halt, Enter Wait State"                                  , "None"                        , "None"                , "Unchanged"                                                                  }, // $40
{ "ANDZ r1", "",              "r0 &= r1;"                            , "AND to Register Zero"                                    , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $41
{ "ANDZ r2", "",              "r0 &= r2;"                            , "AND to Register Zero"                                    , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $42
{ "ANDZ r3", "",              "r0 &= r3;"                            , "AND to Register Zero"                                    , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $43
{ "ANDI,r0", "$vv",           "r0 &= $vv;"                           , "AND Immediate"                                           , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $44
{ "ANDI,r1", "$vv",           "r1 &= $vv;"                           , "AND Immediate"                                           , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $45
{ "ANDI,r2", "$vv",           "r2 &= $vv;"                           , "AND Immediate"                                           , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $46
{ "ANDI,r3", "$vv",           "r3 &= $vv;"                           , "AND Immediate"                                           , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $47
{ "ANDR,r0", "$aaaa",         "r0 &= *($aaaa);"                      , "AND Relative"                                            , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $48
{ "ANDR,r1", "$aaaa",         "r1 &= *($aaaa);"                      , "AND Relative"                                            , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $49
{ "ANDR,r2", "$aaaa",         "r2 &= *($aaaa);"                      , "AND Relative"                                            , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $4A
{ "ANDR,r3", "$aaaa",         "r3 &= *($aaaa);"                      , "AND Relative"                                            , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $4B
{ "ANDA,r0", "$aaaa",         "r0 &= *($aaaa);"                      , "AND Absolute"                                            , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $4C
{ "ANDA,r1", "$aaaa",         "r1 &= *($aaaa);"                      , "AND Absolute"                                            , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $4D
{ "ANDA,r2", "$aaaa",         "r2 &= *($aaaa);"                      , "AND Absolute"                                            , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $4E
{ "ANDA,r3", "$aaaa",         "r3 &= *($aaaa);"                      , "AND Absolute"                                            , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $4F
{ "RRR,r0",  "",              "r0 >>= 1;"                            , "Rotate Register Right"                                   , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $50
{ "RRR,r1",  "",              "r1 >>= 1;"                            , "Rotate Register Right"                                   , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $51
{ "RRR,r2",  "",              "r2 >>= 1;"                            , "Rotate Register Right"                                   , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $52
{ "RRR,r3",  "",              "r3 >>= 1;"                            , "Rotate Register Right"                                   , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $53
{ "REDE,r0", "$vv",           "r0 = IOPORT($vv);"                    , "Read Extended"                                           , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $54
{ "REDE,r1", "$vv",           "r1 = IOPORT($vv);"                    , "Read Extended"                                           , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $55
{ "REDE,r2", "$vv",           "r2 = IOPORT($vv);"                    , "Read Extended"                                           , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $56
{ "REDE,r3", "$vv",           "r3 = IOPORT($vv);"                    , "Read Extended"                                           , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $57
{ "BRNR,r0", "$aaaa",         "if (r0 != 0) goto $aaaa;"             , "Branch on Non-Zero Register, Relative"                   , "None"                        , "None"                , "Unchanged"                                                                  }, // $58
{ "BRNR,r1", "$aaaa",         "if (r1 != 0) goto $aaaa;"             , "Branch on Non-Zero Register, Relative"                   , "RS"                          , "None"                , "Unchanged"                                                                  }, // $59
{ "BRNR,r2", "$aaaa",         "if (r2 != 0) goto $aaaa;"             , "Branch on Non-Zero Register, Relative"                   , "RS"                          , "None"                , "Unchanged"                                                                  }, // $5A
{ "BRNR,r3", "$aaaa",         "if (r3 != 0) goto $aaaa;"             , "Branch on Non-Zero Register, Relative"                   , "RS"                          , "None"                , "Unchanged"                                                                  }, // $5B
{ "BRNA,r0", "$aaaa",         "if (r0 != 0) goto $aaaa;"             , "Branch on Non-Zero Register, Absolute"                   , "None"                        , "None"                , "Unchanged"                                                                  }, // $5C
{ "BRNA,r1", "$aaaa",         "if (r1 != 0) goto $aaaa;"             , "Branch on Non-Zero Register, Absolute"                   , "RS"                          , "None"                , "Unchanged"                                                                  }, // $5D
{ "BRNA,r2", "$aaaa",         "if (r2 != 0) goto $aaaa;"             , "Branch on Non-Zero Register, Absolute"                   , "RS"                          , "None"                , "Unchanged"                                                                  }, // $5E
{ "BRNA,r3", "$aaaa",         "if (r3 != 0) goto $aaaa;"             , "Branch on Non-Zero Register, Absolute"                   , "RS"                          , "None"                , "Unchanged"                                                                  }, // $5F
{ "IORZ r0", "",              ";"                                    , "Inclusive OR to Register Zero"                           , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $60 or "r0 |= r0;"
{ "IORZ r1", "",              "r0 |= r1;"                            , "Inclusive OR to Register Zero"                           , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $61
{ "IORZ r2", "",              "r0 |= r2;"                            , "Inclusive OR to Register Zero"                           , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $62
{ "IORZ r3", "",              "r0 |= r3;"                            , "Inclusive OR to Register Zero"                           , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $63
{ "IORI,r0", "$vv",           "r0 |= $vv;"                           , "Inclusive OR Absolute"                                   , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $64
{ "IORI,r1", "$vv",           "r1 |= $vv;"                           , "Inclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $65
{ "IORI,r2", "$vv",           "r2 |= $vv;"                           , "Inclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $66
{ "IORI,r3", "$vv",           "r3 |= $vv;"                           , "Inclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $67
{ "IORR,r0", "$aaaa",         "r0 |= *($aaaa);"                      , "Inclusive OR Relative"                                   , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $68
{ "IORR,r1", "$aaaa",         "r1 |= *($aaaa);"                      , "Inclusive OR Relative"                                   , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $69
{ "IORR,r2", "$aaaa",         "r2 |= *($aaaa);"                      , "Inclusive OR Relative"                                   , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $6A
{ "IORR,r3", "$aaaa",         "r3 |= *($aaaa);"                      , "Inclusive OR Relative"                                   , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $6B
{ "IORA,r0", "$aaaa",         "r0 |= *($aaaa);"                      , "Inclusive OR Absolute"                                   , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $6C
{ "IORA,r1", "$aaaa",         "r1 |= *($aaaa);"                      , "Inclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $6D
{ "IORA,r2", "$aaaa",         "r2 |= *($aaaa);"                      , "Inclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $6E
{ "IORA,r3", "$aaaa",         "r3 |= *($aaaa);"                      , "Inclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $6F
{ "REDD,r0", "",              "r0 = IOPORT(DATA);"                   , "Read Data"                                               , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $70
{ "REDD,r1", "",              "r1 = IOPORT(DATA);"                   , "Read Data"                                               , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $71
{ "REDD,r2", "",              "r2 = IOPORT(DATA);"                   , "Read Data"                                               , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $72
{ "REDD,r3", "",              "r3 = IOPORT(DATA);"                   , "Read Data"                                               , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $73
{ "CPSU",    "$vv",           "PSU &= ~($vv & %01100111);"           , "Clear Program Status, Upper"                             , "None"                        , "F, II, UF1, UF2, SP" , "Unchanged"                                                                  }, // $74
{ "CPSL",    "$vv",           "PSL &= ~($vv);"                       , "Clear Program Status, Lower"                             , "None"                        , "CC, IDC, RS, WC, OVF, COM, C", "CC &= ~(($vv & $C0) >> 6);"                                         }, // $75
{ "PPSU",    "$vv",           "PSU |= ($vv & %01100111);"            , "Preset Program Status, Upper"                            , "None"                        , "F, II, UF1, UF2, SP" , "Unchanged"                                                                  }, // $76
{ "PPSL",    "$vv",           "PSL |= $vv;"                          , "Preset Program Status, Lower"                            , "None"                        , "CC, IDC, RS, WC, OVF, COM, C", "CC |= ($vv & $C0) >> 6;"                                            }, // $77
{ "BSNR,r0", "$aaaa",         "if (r0 != 0) gosub $aaaa;"            , "Branch to Subroutine on Non-Zero Register, Relative"     , "SP"                          , "SP"                  , "Unchanged"                                                                  }, // $78
{ "BSNR,r1", "$aaaa",         "if (r1 != 0) gosub $aaaa;"            , "Branch to Subroutine on Non-Zero Register, Relative"     , "SP, RS"                      , "SP"                  , "Unchanged"                                                                  }, // $79
{ "BSNR,r2", "$aaaa",         "if (r2 != 0) gosub $aaaa;"            , "Branch to Subroutine on Non-Zero Register, Relative"     , "SP, RS"                      , "SP"                  , "Unchanged"                                                                  }, // $7A
{ "BSNR,r3", "$aaaa",         "if (r3 != 0) gosub $aaaa;"            , "Branch to Subroutine on Non-Zero Register, Relative"     , "SP, RS"                      , "SP"                  , "Unchanged"                                                                  }, // $7B
{ "BSNA,r0", "$aaaa",         "if (r0 != 0) gosub $aaaa;"            , "Branch to Subroutine on Non-Zero Register, Absolute"     , "SP"                          , "SP"                  , "Unchanged"                                                                  }, // $7C
{ "BSNA,r1", "$aaaa",         "if (r1 != 0) gosub $aaaa;"            , "Branch to Subroutine on Non-Zero Register, Absolute"     , "SP, RS"                      , "SP"                  , "Unchanged"                                                                  }, // $7D
{ "BSNA,r2", "$aaaa",         "if (r2 != 0) gosub $aaaa;"            , "Branch to Subroutine on Non-Zero Register, Absolute"     , "SP, RS"                      , "SP"                  , "Unchanged"                                                                  }, // $7E
{ "BSNA,r3", "$aaaa",         "if (r3 != 0) gosub $aaaa;"            , "Branch to Subroutine on Non-Zero Register, Absolute"     , "SP, RS"                      , "SP"                  , "Unchanged"                                                                  }, // $7F
{ "ADDZ r0", "",              "r0 *= 2;"                             , "Add to Register Zero"                                    , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $80 or "r0 += r0;"
{ "ADDZ r1", "",              "r0 += r1;"                            , "Add to Register Zero"                                    , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $81
{ "ADDZ r2", "",              "r0 += r2;"                            , "Add to Register Zero"                                    , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $82
{ "ADDZ r3", "",              "r0 += r3;"                            , "Add to Register Zero"                                    , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $83
{ "ADDI,r0", "$vv",           "r0 += $vv;"                           , "Add Immediate"                                           , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $84
{ "ADDI,r1", "$vv",           "r1 += $vv;"                           , "Add Immediate"                                           , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $85
{ "ADDI,r2", "$vv",           "r2 += $vv;"                           , "Add Immediate"                                           , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $86
{ "ADDI,r3", "$vv",           "r3 += $vv;"                           , "Add Immediate"                                           , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $87
{ "ADDR,r0", "$aaaa",         "r0 += *($aaaa);"                      , "Add Relative"                                            , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $88
{ "ADDR,r1", "$aaaa",         "r1 += *($aaaa);"                      , "Add Relative"                                            , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $89
{ "ADDR,r2", "$aaaa",         "r2 += *($aaaa);"                      , "Add Relative"                                            , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $8A
{ "ADDR,r3", "$aaaa",         "r3 += *($aaaa);"                      , "Add Relative"                                            , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $8B
{ "ADDA,r0", "$aaaa",         "r0 += *($aaaa);"                      , "Add Absolute"                                            , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $8C
{ "ADDA,r1", "$aaaa",         "r1 += *($aaaa);"                      , "Add Absolute"                                            , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $8D
{ "ADDA,r2", "$aaaa",         "r2 += *($aaaa);"                      , "Add Absolute"                                            , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $8E
{ "ADDA,r3", "$aaaa",         "r3 += *($aaaa);"                      , "Add Absolute"                                            , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $8F
{ "-------", "",              "?"                                    , "-"                                                       , "None"                        , "None"                , "Undefined"                                                                  }, // $90
{ "-------", "",              "?"                                    , "-"                                                       , "None"                        , "None"                , "Undefined"                                                                  }, // $91
{ "LPSU",    "",              "PSU &= %10000000; PSU |= (r0 & %01100111);", "Load Program Status, Upper"                         , "None"                        , "F, II, UF1, UF2, SP" , "Unchanged"                                                                  }, // $92
{ "LPSL",    "",              "PSL = r0;"                            , "Load Program Status, Lower from Register Zero"           , "None"                        , "CC, IDC, RS, WC, OVF, COM, C", "CC = ($vv & $C0) >> 6;"                                             }, // $93
{ "DAR,r0",  "",              "r0 = BCD(r0);"                        , "Decimal Adjust Register"                                 , "IDC, C"                      , "CC"                  , "Meaningless"                                                                }, // $94
{ "DAR,r1",  "",              "r1 = BCD(r1);"                        , "Decimal Adjust Register"                                 , "IDC, RS, C"                  , "CC"                  , "Meaningless"                                                                }, // $95
{ "DAR,r2",  "",              "r2 = BCD(r2);"                        , "Decimal Adjust Register"                                 , "IDC, RS, C"                  , "CC"                  , "Meaningless"                                                                }, // $96
{ "DAR,r3",  "",              "r3 = BCD(r3);"                        , "Decimal Adjust Register"                                 , "IDC, RS, C"                  , "CC"                  , "Meaningless"                                                                }, // $97
{ "BCFR,eq", "$aaaa",         "if CC != EQ goto $aaaa;"              , "Branch on Condition False, Relative"                     , "CC"                          , "None"                , "Unchanged"                                                                  }, // $98 or "if (PSL & $C0) != $00 goto $aaaa;", or "if != goto $aaaa;"
{ "BCFR,gt", "$aaaa",         "if CC != GT goto $aaaa;"              , "Branch on Condition False, Relative"                     , "CC"                          , "None"                , "Unchanged"                                                                  }, // $99 or "if (PSL & $C0) != $40 goto $aaaa;", or "if <= goto $aaaa;"
{ "BCFR,lt", "$aaaa",         "if CC != LT goto $aaaa;"              , "Branch on Condition False, Relative"                     , "CC"                          , "None"                , "Unchanged"                                                                  }, // $9A or "if (PSL & $C0) != $80 goto $aaaa;", or "if >= goto $aaaa;"
{ "ZBRR",    "$aaaa",         "goto $aaaa;"                          , "Zero Branch, Relative"                                   , "None"                        , "None"                , "Unchanged"                                                                  }, // $9B
{ "BCFA,eq", "$aaaa",         "if CC != EQ goto $aaaa;"              , "Branch on Condition False, Absolute"                     , "CC"                          , "None"                , "Unchanged"                                                                  }, // $9C or "if (PSL & $C0) != $00 goto $aaaa;", or "if != goto $aaaa;"
{ "BCFA,gt", "$aaaa",         "if CC != GT goto $aaaa;"              , "Branch on Condition False, Absolute"                     , "CC"                          , "None"                , "Unchanged"                                                                  }, // $9D or "if (PSL & $C0) != $40 goto $aaaa;", or "if <= goto $aaaa;"
{ "BCFA,lt", "$aaaa",         "if CC != LT goto $aaaa;"              , "Branch on Condition False, Absolute"                     , "CC"                          , "None"                , "Unchanged"                                                                  }, // $9E or "if (PSL & $C0) != $80 goto $aaaa;", or "if >= goto $aaaa;"
{ "BXA,r3",  "BXA $aaaa,r3",  "goto $aaaa + r3;"                     , "Branch Indexed, Absolute"                                , "RS"                          , "None"                , "Unchanged"                                                                  }, // $9F
{ "SUBZ r0", "",              "r0 = 0;"                              , "Subtract from Register Zero"                             , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $A0 or "r0 -= r0;"
{ "SUBZ r1", "",              "r0 -= r1;"                            , "Subtract from Register Zero"                             , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $A1
{ "SUBZ r2", "",              "r0 -= r2;"                            , "Subtract from Register Zero"                             , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $A2
{ "SUBZ r3", "",              "r0 -= r3;"                            , "Subtract from Register Zero"                             , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $A3
{ "SUBI,r0", "$vv",           "r0 -= $vv;"                           , "Subtract Immediate"                                      , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $A4
{ "SUBI,r1", "$vv",           "r1 -= $vv;"                           , "Subtract Immediate"                                      , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $A5
{ "SUBI,r2", "$vv",           "r2 -= $vv;"                           , "Subtract Immediate"                                      , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $A6
{ "SUBI,r3", "$vv",           "r3 -= $vv;"                           , "Subtract Immediate"                                      , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $A7
{ "SUBR,r0", "$aaaa",         "r0 -= *($aaaa);"                      , "Subtract Relative"                                       , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $A8
{ "SUBR,r1", "$aaaa",         "r1 -= *($aaaa);"                      , "Subtract Relative"                                       , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $A9
{ "SUBR,r2", "$aaaa",         "r2 -= *($aaaa);"                      , "Subtract Relative"                                       , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $AA
{ "SUBR,r3", "$aaaa",         "r3 -= *($aaaa);"                      , "Subtract Relative"                                       , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $AB
{ "SUBA,r0", "$aaaa",         "r0 -= *($aaaa);"                      , "Subtract Absolute"                                       , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $AC
{ "SUBA,r1", "$aaaa",         "r1 -= *($aaaa);"                      , "Subtract Absolute"                                       , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $AD
{ "SUBA,r2", "$aaaa",         "r2 -= *($aaaa);"                      , "Subtract Absolute"                                       , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $AE
{ "SUBA,r3", "$aaaa",         "r3 -= *($aaaa);"                      , "Subtract Absolute"                                       , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $AF
{ "WRTC,r0", "",              "IOPORT(CTRL) = r0;"                   , "Write Control"                                           , "None"                        , "None"                , "Unchanged"                                                                  }, // $B0
{ "WRTC,r1", "",              "IOPORT(CTRL) = r1;"                   , "Write Control"                                           , "RS"                          , "None"                , "Unchanged"                                                                  }, // $B1
{ "WRTC,r2", "",              "IOPORT(CTRL) = r2;"                   , "Write Control"                                           , "RS"                          , "None"                , "Unchanged"                                                                  }, // $B2
{ "WRTC,r3", "",              "IOPORT(CTRL) = r3;"                   , "Write Control"                                           , "RS"                          , "None"                , "Unchanged"                                                                  }, // $B3
{ "TPSU",    "$vv",           "CC = (PSU & $vv == $vv) ? EQ : LT;"   , "Test Program Status, Upper"                              , "S, F, II, UF1, UF2, SP"      , "CC"                  , "CC = (PSU & $vv == $vv) ? EQ : LT;"                                         }, // $B4
{ "TPSL",    "$vv",           "CC = (PSL & $vv == $vv) ? EQ : LT;"   , "Test Program Status, Lower"                              , "CC, IDC, RS, WC, OVF, COM, C", "CC"                  , "CC = (PSL & $vv == $vv) ? EQ : LT;"                                         }, // $B5
{ "-------", "",              "?"                                    , "-"                                                       , "None"                        , "None"                , "Undefined"                                                                  }, // $B6
{ "-------", "",              "?"                                    , "-"                                                       , "None"                        , "None"                , "Undefined"                                                                  }, // $B7
{ "BSFR,eq", "$aaaa",         "if CC != EQ gosub $aaaa;"             , "Branch to Subroutine on Condition False, Relative"       , "SP, CC"                      , "SP"                  , "Unchanged"                                                                  }, // $B8 or "if (PSL & $C0) != $00 gosub $aaaa;", or "if != gosub $aaaa;"
{ "BSFR,gt", "$aaaa",         "if CC != GT gosub $aaaa;"             , "Branch to Subroutine on Condition False, Relative"       , "SP, CC"                      , "SP"                  , "Unchanged"                                                                  }, // $B9 or "if (PSL & $C0) != $40 gosub $aaaa;", or "if <= gosub $aaaa;"
{ "BSFR,lt", "$aaaa",         "if CC != LT gosub $aaaa;"             , "Branch to Subroutine on Condition False, Relative"       , "SP, CC"                      , "SP"                  , "Unchanged"                                                                  }, // $BA or "if (PSL & $C0) != $80 gosub $aaaa;", or "if >= gosub $aaaa;"
{ "ZBSR",    "$aaaa",         "gosub $aaaa;"                         , "Zero Branch to Subroutine, Relative"                     , "SP"                          , "SP"                  , "Unchanged"                                                                  }, // $BB
{ "BSFA,eq", "$aaaa",         "if CC != EQ gosub $aaaa;"             , "Branch to Subroutine on Condition False, Absolute"       , "SP, CC"                      , "SP"                  , "Unchanged"                                                                  }, // $BC or "if (PSL & $C0) != $00 gosub $aaaa;", or "if != gosub $aaaa;"
{ "BSFA,gt", "$aaaa",         "if CC <= GT gosub $aaaa;"             , "Branch to Subroutine on Condition False, Absolute"       , "SP, CC"                      , "SP"                  , "Unchanged"                                                                  }, // $BD or "if (PSL & $C0) != $40 gosub $aaaa;", or "if <= gosub $aaaa;"
{ "BSFA,lt", "$aaaa",         "if CC >= LT gosub $aaaa;"             , "Branch to Subroutine on Condition False, Absolute"       , "SP, CC"                      , "SP"                  , "Unchanged"                                                                  }, // $BE or "if (PSL & $C0) != $80 gosub $aaaa;", or "if >= gosub $aaaa;"
{ "BSXA,r3", "BSXA $aaaa,r3", "gosub $aaaa + r3;"                    , "Branch to Subroutine Indexed, Absolute"                  , "SP, RS"                      , "SP"                  , "Unchanged"                                                                  }, // $BF
{ "NOP",     "",              ";"                                    , "No Operation"                                            , "None"                        , "None"                , "Unchanged"                                                                  }, // $C0
{ "STRZ r1", "",              "r1 = r0;"                             , "Store to Register Zero"                                  , "RS"                          , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $C1
{ "STRZ r2", "",              "r2 = r0;"                             , "Store to Register Zero"                                  , "RS"                          , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $C2
{ "STRZ r3", "",              "r3 = r0;"                             , "Store to Register Zero"                                  , "RS"                          , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $C3
{ "-------", "",              "?"                                    , "-"                                                       , "None"                        , "None"                , "Undefined"                                                                  }, // $C4
{ "-------", "",              "?"                                    , "-"                                                       , "None"                        , "None"                , "Undefined"                                                                  }, // $C5
{ "-------", "",              "?"                                    , "-"                                                       , "None"                        , "None"                , "Undefined"                                                                  }, // $C6
{ "-------", "",              "?"                                    , "-"                                                       , "None"                        , "None"                , "Undefined"                                                                  }, // $C7
{ "STRR,r0", "$aaaa",         "*($aaaa) = r0;"                       , "Store Relative"                                          , "None"                        , "None"                , "Unchanged"                                                                  }, // $C8
{ "STRR,r1", "$aaaa",         "*($aaaa) = r1;"                       , "Store Relative"                                          , "RS"                          , "None"                , "Unchanged"                                                                  }, // $C9
{ "STRR,r2", "$aaaa",         "*($aaaa) = r2;"                       , "Store Relative"                                          , "RS"                          , "None"                , "Unchanged"                                                                  }, // $CA
{ "STRR,r3", "$aaaa",         "*($aaaa) = r3;"                       , "Store Relative"                                          , "RS"                          , "None"                , "Unchanged"                                                                  }, // $CB
{ "STRA,r0", "$aaaa",         "*($aaaa) = r0;"                       , "Store Absolute"                                          , "None"                        , "None"                , "Unchanged"                                                                  }, // $CC
{ "STRA,r1", "$aaaa",         "*($aaaa) = r1;"                       , "Store Absolute"                                          , "RS"                          , "None"                , "Unchanged"                                                                  }, // $CD
{ "STRA,r2", "$aaaa",         "*($aaaa) = r2;"                       , "Store Absolute"                                          , "RS"                          , "None"                , "Unchanged"                                                                  }, // $CE
{ "STRA,r3", "$aaaa",         "*($aaaa) = r3;"                       , "Store Absolute"                                          , "RS"                          , "None"                , "Unchanged"                                                                  }, // $CF
{ "RRL,r0",  "",              "r0 <<= 1;"                            , "Rotate Register Left"                                    , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"               }, // $D0
{ "RRL,r1",  "",              "r1 <<= 1;"                            , "Rotate Register Left"                                    , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"               }, // $D1
{ "RRL,r2",  "",              "r2 <<= 1;"                            , "Rotate Register Left"                                    , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"               }, // $D2
{ "RRL,r3",  "",              "r3 <<= 1;"                            , "Rotate Register Left"                                    , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"               }, // $D3
{ "WRTE,r0", "$vv",           "IOPORT($vv) = r0;"                    , "Write Extended"                                          , "None"                        , "None"                , "Unchanged"                                                                  }, // $D4
{ "WRTE,r1", "$vv",           "IOPORT($vv) = r1;"                    , "Write Extended"                                          , "RS"                          , "None"                , "Unchanged"                                                                  }, // $D5
{ "WRTE,r2", "$vv",           "IOPORT($vv) = r2;"                    , "Write Extended"                                          , "RS"                          , "None"                , "Unchanged"                                                                  }, // $D6
{ "WRTE,r3", "$vv",           "IOPORT($vv) = r3;"                    , "Write Extended"                                          , "RS"                          , "None"                , "Unchanged"                                                                  }, // $D7
{ "BIRR,r0", "$aaaa",         "if (++r0 != 0) goto $aaaa;"           , "Branch on Incrementing Register, Relative"               , "None"                        , "None"                , "Unchanged"                                                                  }, // $D8
{ "BIRR,r1", "$aaaa",         "if (++r1 != 0) goto $aaaa;"           , "Branch on Incrementing Register, Relative"               , "RS"                          , "None"                , "Unchanged"                                                                  }, // $D9
{ "BIRR,r2", "$aaaa",         "if (++r2 != 0) goto $aaaa;"           , "Branch on Incrementing Register, Relative"               , "RS"                          , "None"                , "Unchanged"                                                                  }, // $DA
{ "BIRR,r3", "$aaaa",         "if (++r3 != 0) goto $aaaa;"           , "Branch on Incrementing Register, Relative"               , "RS"                          , "None"                , "Unchanged"                                                                  }, // $DB
{ "BIRA,r0", "$aaaa",         "if (++r0 != 0) goto $aaaa;"           , "Branch on Incrementing Register, Absolute"               , "None"                        , "None"                , "Unchanged"                                                                  }, // $DC
{ "BIRA,r1", "$aaaa",         "if (++r1 != 0) goto $aaaa;"           , "Branch on Incrementing Register, Absolute"               , "RS"                          , "None"                , "Unchanged"                                                                  }, // $DD
{ "BIRA,r2", "$aaaa",         "if (++r2 != 0) goto $aaaa;"           , "Branch on Incrementing Register, Absolute"               , "RS"                          , "None"                , "Unchanged"                                                                  }, // $DE
{ "BIRA,r3", "$aaaa",         "if (++r3 != 0) goto $aaaa;"           , "Branch on Incrementing Register, Absolute"               , "RS"                          , "None"                , "Unchanged"                                                                  }, // $DF
{ "COMZ r0", "",              "CC = EQ;"                             , "Compare to Register Zero"                                , "COM"                         , "CC"                  , "CC = EQ;"                                                                   }, // $E0
{ "COMZ r1", "",              "compare r0 against r1;"               , "Compare to Register Zero"                                , "RS, COM"                     , "CC"                  , "if (r0 > r1) CC = GT; elif (r0 < r1) CC = LT; else CC = EQ;"                }, // $E1
{ "COMZ r2", "",              "compare r0 against r2;"               , "Compare to Register Zero"                                , "RS, COM"                     , "CC"                  , "if (r0 > r2) CC = GT; elif (r0 < r2) CC = LT; else CC = EQ;"                }, // $E2
{ "COMZ r3", "",              "compare r0 against r3;"               , "Compare to Register Zero"                                , "RS, COM"                     , "CC"                  , "if (r0 > r3) CC = GT; elif (r0 < r3) CC = LT; else CC = EQ;"                }, // $E3
{ "COMI,r0", "$vv",           "compare r0 against $vv;"              , "Compare Immediate"                                       , "COM"                         , "CC"                  , "if (r0 > $vv) CC = GT; elif (r0 < $vv) CC = LT; else CC = EQ;"              }, // $E4
{ "COMI,r1", "$vv",           "compare r1 against $vv;"              , "Compare Immediate"                                       , "RS, COM"                     , "CC"                  , "if (r1 > $vv) CC = GT; elif (r1 < $vv) CC = LT; else CC = EQ;"              }, // $E5
{ "COMI,r2", "$vv",           "compare r2 against $vv;"              , "Compare Immediate"                                       , "RS, COM"                     , "CC"                  , "if (r2 > $vv) CC = GT; elif (r2 < $vv) CC = LT; else CC = EQ;"              }, // $E6
{ "COMI,r3", "$vv",           "compare r3 against $vv;"              , "Compare Immediate"                                       , "RS, COM"                     , "CC"                  , "if (r3 > $vv) CC = GT; elif (r3 < $vv) CC = LT; else CC = EQ;"              }, // $E7
{ "COMR,r0", "$aaaa",         "compare r0 against *($aaaa);"         , "Compare Relative"                                        , "COM"                         , "CC"                  , "if (r0 > *($aaaa)) CC = GT; elif (r0 < *($aaaa)) CC = LT; else CC = EQ;"    }, // $E8
{ "COMR,r1", "$aaaa",         "compare r1 against *($aaaa);"         , "Compare Relative"                                        , "RS, COM"                     , "CC"                  , "if (r1 > *($aaaa)) CC = GT; elif (r1 < *($aaaa)) CC = LT; else CC = EQ;"    }, // $E9
{ "COMR,r2", "$aaaa",         "compare r2 against *($aaaa);"         , "Compare Relative"                                        , "RS, COM"                     , "CC"                  , "if (r2 > *($aaaa)) CC = GT; elif (r2 < *($aaaa)) CC = LT; else CC = EQ;"    }, // $EA
{ "COMR,r3", "$aaaa",         "compare r3 against *($aaaa);"         , "Compare Relative"                                        , "RS, COM"                     , "CC"                  , "if (r3 > *($aaaa)) CC = GT; elif (r3 < *($aaaa)) CC = LT; else CC = EQ;"    }, // $EB
{ "COMA,r0", "$aaaa",         "compare r0 against *($aaaa);"         , "Compare Absolute"                                        , "COM"                         , "CC"                  , "if (r0 > *($aaaa)) CC = GT; elif (r0 < *($aaaa)) CC = LT; else CC = EQ;"    }, // $EC
{ "COMA,r1", "$aaaa",         "compare r1 against *($aaaa);"         , "Compare Absolute"                                        , "RS, COM"                     , "CC"                  , "if (r1 > *($aaaa)) CC = GT; elif (r1 < *($aaaa)) CC = LT; else CC = EQ;"    }, // $ED
{ "COMA,r2", "$aaaa",         "compare r2 against *($aaaa);"         , "Compare Absolute"                                        , "RS, COM"                     , "CC"                  , "if (r2 > *($aaaa)) CC = GT; elif (r2 < *($aaaa)) CC = LT; else CC = EQ;"    }, // $EE
{ "COMA,r3", "$aaaa",         "compare r3 against *($aaaa);"         , "Compare Absolute"                                        , "RS, COM"                     , "CC"                  , "if (r3 > *($aaaa)) CC = GT; elif (r3 < *($aaaa)) CC = LT; else CC = EQ;"    }, // $EF
{ "WRTD,r0", "",              "IOPORT(DATA) = r0;"                   , "Write Data"                                              , "None"                        , "None"                , "Unchanged"                                                                  }, // $F0
{ "WRTD,r1", "",              "IOPORT(DATA) = r1;"                   , "Write Data"                                              , "RS"                          , "None"                , "Unchanged"                                                                  }, // $F1
{ "WRTD,r2", "",              "IOPORT(DATA) = r2;"                   , "Write Data"                                              , "RS"                          , "None"                , "Unchanged"                                                                  }, // $F2
{ "WRTD,r3", "",              "IOPORT(DATA) = r3;"                   , "Write Data"                                              , "RS"                          , "None"                , "Unchanged"                                                                  }, // $F3
{ "TMI,r0",  "$vv",           "CC = (r0 & $vv == $vv) ? EQ : LT;"    , "Test under Mask Immediate"                               , "None"                        , "CC"                  , "CC = (r0 & $vv == $vv) ? EQ : LT;"                                          }, // $F4
{ "TMI,r1",  "$vv",           "CC = (r1 & $vv == $vv) ? EQ : LT;"    , "Test under Mask Immediate"                               , "RS"                          , "CC"                  , "CC = (r1 & $vv == $vv) ? EQ : LT;"                                          }, // $F5
{ "TMI,r2",  "$vv",           "CC = (r2 & $vv == $vv) ? EQ : LT;"    , "Test under Mask Immediate"                               , "RS"                          , "CC"                  , "CC = (r2 & $vv == $vv) ? EQ : LT;"                                          }, // $F6
{ "TMI,r3",  "$vv",           "CC = (r3 & $vv == $vv) ? EQ : LT;"    , "Test under Mask Immediate"                               , "RS"                          , "CC"                  , "CC = (r3 & $vv == $vv) ? EQ : LT;"                                          }, // $F7
{ "BDRR,r0", "$aaaa",         "if (--r0 != 0) goto $aaaa;"           , "Branch on Decrementing Register, Relative"               , "None"                        , "None"                , "Unchanged"                                                                  }, // $F8
{ "BDRR,r1", "$aaaa",         "if (--r1 != 0) goto $aaaa;"           , "Branch on Decrementing Register, Relative"               , "RS"                          , "None"                , "Unchanged"                                                                  }, // $F9
{ "BDRR,r2", "$aaaa",         "if (--r2 != 0) goto $aaaa;"           , "Branch on Decrementing Register, Relative"               , "RS"                          , "None"                , "Unchanged"                                                                  }, // $FA
{ "BDRR,r3", "$aaaa",         "if (--r3 != 0) goto $aaaa;"           , "Branch on Decrementing Register, Relative"               , "RS"                          , "None"                , "Unchanged"                                                                  }, // $FB
{ "BDRA,r0", "$aaaa",         "if (--r0 != 0) goto $aaaa;"           , "Branch on Decrementing Register, Absolute"               , "None"                        , "None"                , "Unchanged"                                                                  }, // $FC
{ "BDRA,r1", "$aaaa",         "if (--r1 != 0) goto $aaaa;"           , "Branch on Decrementing Register, Absolute"               , "RS"                          , "None"                , "Unchanged"                                                                  }, // $FD
{ "BDRA,r2", "$aaaa",         "if (--r2 != 0) goto $aaaa;"           , "Branch on Decrementing Register, Absolute"               , "RS"                          , "None"                , "Unchanged"                                                                  }, // $FE
{ "BDRA,r3", "$aaaa",         "if (--r3 != 0) goto $aaaa;"           , "Branch on Decrementing Register, Absolute"               , "RS"                          , "None"                , "Unchanged"                                                                  }, // $FF
}, {
// Signetics extended format
{ "LODZ r0", "",              ";"                                    , "Load from Register Zero"                                 , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $00 LOD indeterminate! or, "r0 = r0;"
{ "LODZ r1", "",              "r0 = r1;"                             , "Load from Register Zero"                                 , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $01 LOD
{ "LODZ r2", "",              "r0 = r2;"                             , "Load from Register Zero"                                 , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $02 LOD
{ "LODZ r3", "",              "r0 = r3;"                             , "Load from Register Zero"                                 , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $03 LOD
{ "LODI,r0", "$vv",           "r0 = $vv;"                            , "Load Immediate"                                          , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $04 LOD
{ "LODI,r1", "$vv",           "r1 = $vv;"                            , "Load Immediate"                                          , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $05 LOD
{ "LODI,r2", "$vv",           "r2 = $vv;"                            , "Load Immediate"                                          , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $06 LOD
{ "LODI,r3", "$vv",           "r3 = $vv;"                            , "Load Immediate"                                          , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $07 LOD
{ "LODR,r0", "$aaaa",         "r0 = *($aaaa);"                       , "Load Relative"                                           , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $08 LOD
{ "LODR,r1", "$aaaa",         "r1 = *($aaaa);"                       , "Load Relative"                                           , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $09 LOD
{ "LODR,r2", "$aaaa",         "r2 = *($aaaa);"                       , "Load Relative"                                           , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $0A LOD
{ "LODR,r3", "$aaaa",         "r3 = *($aaaa);"                       , "Load Relative"                                           , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $0B LOD
{ "LODA,r0", "$aaaa",         "r0 = *($aaaa);"                       , "Load Absolute"                                           , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $0C LOD
{ "LODA,r1", "$aaaa",         "r1 = *($aaaa);"                       , "Load Absolute"                                           , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $0D LOD
{ "LODA,r2", "$aaaa",         "r2 = *($aaaa);"                       , "Load Absolute"                                           , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $0E LOD
{ "LODA,r3", "$aaaa",         "r3 = *($aaaa);"                       , "Load Absolute"                                           , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $0F LOD
{ "LDPL",    "$aaaa",         "PSL = *($aaaa);"                      , "Load Program Status, Lower, Absolute"                    , "None"                        , "CC, IDC, RS, WC, OVF, COM, C", "CC = (*($aaaa) & $C0) >> 6;"                                       }, // $10 2650B only
{ "STPL",    "$aaaa",         "*($aaaa) = PSL;"                      , "Store Program Status, Lower, Absolute"                   , "CC, IDC, RS, WC, OVF, COM, C", "None"                , "Undefined"                                                                 }, // $11 2650B only
{ "SPSU",    "",              "r0 = PSU;"                            , "Store Program Status, Upper"                             , "S, F, II, UF1, UF2, SP"      , "CC"                  , "if (S) CC = LT; elif (PSU == 0) CC = EQ; else CC = GT;"                    }, // $12
{ "SPSL",    "",              "r0 = PSL;"                            , "Store Program Status, Lower to Register Zero"            , "CC, IDC, RS, WC, OVF, COM, C", "CC"                  , "if (CC == LT || CC == UN) CC = LT; elif (PSL == 0) CC = EQ; else CC = GT;" }, // $13
{ "RETC,eq", "",              "if == return;"                        , "Return from Subroutine, Conditional"                     , "SP, CC"                      , "SP"                  , "Unchanged"                                                                 }, // $14 RETC or, "if (PSL & $C0) == $00 return;"
{ "RETC,gt", "",              "if > return;"                         , "Return from Subroutine, Conditional"                     , "SP, CC"                      , "SP"                  , "Unchanged"                                                                 }, // $15 RETC or, "if (PSL & $C0) == $40 return;"
{ "RETC,lt", "",              "if < return;"                         , "Return from Subroutine, Conditional"                     , "SP, CC"                      , "SP"                  , "Unchanged"                                                                 }, // $16 RETC or, "if (PSL & $C0) == $80 return;"
{ "RET",     "",              "return;"                              , "Return from Subroutine"                                  , "SP"                          , "SP"                  , "Unchanged"                                                                 }, // $17 RETC
{ "BER/BZR/BOR", "$aaaa",     "if == goto $aaaa;"                    , "Branch if Equal, Relative\n" \
                                                                       "or Branch if Zero, Relative\n" \
                                                                       "or Branch if Ones, Relative"                             , "CC"                          , "None"                , "Unchanged"                                                                 }, // $18 BCTR or, "if (PSL & $C0) == $00 goto $aaaa;"
{ "BHR/BPR", "$aaaa",         "if > goto $aaaa;"                     , "Branch if Higher, Relative\n" \
                                                                       "or Branch if Positive, Relative"                         , "CC"                          , "None"                , "Unchanged"                                                                 }, // $19 BCTR or, "if (PSL & $C0) == $40 goto $aaaa;"
{ "BLR/BMR", "$aaaa",         "if < goto $aaaa;"                     , "Branch if Lower, Relative\n" \
                                                                       "or Branch if Minus/Mixed, Relative"                      , "CC"                          , "None"                , "Unchanged"                                                                 }, // $1A BCTR or, "if (PSL & $C0) == $80 goto $aaaa;"
{ "BR",      "$aaaa",         "goto $aaaa;"                          , "Branch Relative"                                         , "None"                        , "None"                , "Unchanged"                                                                 }, // $1B BCTR
{ "BEA/BZA/BOA", "$aaaa",     "if == goto $aaaa;"                    , "Branch if Equal, Absolute\n" \
                                                                       "or Branch if Zero, Absolute\n" \
                                                                       "or Branch if Ones, Absolute"                             , "CC"                          , "None"                , "Unchanged"                                                                 }, // $1C BCTA or, "if (PSL & $C0) == $00 goto $aaaa;"
{ "BHA/BPA", "$aaaa",         "if > goto $aaaa;"                     , "Branch if Higher, Absolute\n" \
                                                                       "or Branch if Positive, Absolute"                         , "CC"                          , "None"                , "Unchanged"                                                                 }, // $1D BCTA or, "if (PSL & $C0) == $40 goto $aaaa;"
{ "BLA/BMA", "$aaaa",         "if < goto $aaaa;"                     , "Branch if Lower, Absolute\n" \
                                                                       "or Branch if Minus/Mixed, Absolute"                      , "CC"                          , "None"                , "Unchanged"                                                                 }, // $1E BCTA or, "if (PSL & $C0) == $80 goto $aaaa;"
{ "BA",      "$aaaa",         "goto $aaaa;"                          , "Branch Absolute"                                         , "None"                        , "None"                , "Unchanged"                                                                 }, // $1F BCTA
{ "EORZ r0", "",              "r0 = 0;"                              , "Exclusive OR to Register Zero"                           , "None"                        , "CC"                  , "CC = EQ;"                                                                  }, // $20 EOR  or, "r0 ^= r0;"
{ "EORZ r1", "",              "r0 ^= r1;"                            , "Exclusive OR to Register Zero"                           , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $21 EOR
{ "EORZ r2", "",              "r0 ^= r2;"                            , "Exclusive OR to Register Zero"                           , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $22 EOR
{ "EORZ r3", "",              "r0 ^= r3;"                            , "Exclusive OR to Register Zero"                           , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $23 EOR
{ "EORI,r0", "$vv",           "r0 ^= $vv;"                           , "Exclusive OR Immediate"                                  , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $24 EOR
{ "EORI,r1", "$vv",           "r1 ^= $vv;"                           , "Exclusive OR Immediate"                                  , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $25 EOR
{ "EORI,r2", "$vv",           "r2 ^= $vv;"                           , "Exclusive OR Immediate"                                  , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $26 EOR
{ "EORI,r3", "$vv",           "r3 ^= $vv;"                           , "Exclusive OR Immediate"                                  , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $27 EOR
{ "EORR,r0", "$aaaa",         "r0 ^= *($aaaa);"                      , "Exclusive OR Relative"                                   , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $28 EOR
{ "EORR,r1", "$aaaa",         "r1 ^= *($aaaa);"                      , "Exclusive OR Relative"                                   , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $29 EOR
{ "EORR,r2", "$aaaa",         "r2 ^= *($aaaa);"                      , "Exclusive OR Relative"                                   , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $2A EOR
{ "EORR,r3", "$aaaa",         "r3 ^= *($aaaa);"                      , "Exclusive OR Relative"                                   , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $2B EOR
{ "EORA,r0", "$aaaa",         "r0 ^= *($aaaa);"                      , "Exclusive OR Absolute"                                   , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $2C EOR
{ "EORA,r1", "$aaaa",         "r1 ^= *($aaaa);"                      , "Exclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $2D EOR
{ "EORA,r2", "$aaaa",         "r2 ^= *($aaaa);"                      , "Exclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $2E EOR
{ "EORA,r3", "$aaaa",         "r3 ^= *($aaaa);"                      , "Exclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $2F EOR
{ "REDC,r0", "",              "r0 = IOPORT(CTRL);"                   , "Read Control"                                            , "None"                        , "CC"                  , "if (IOPORT(CTRL) & $80) CC = LT; elif (IOPORT(CTRL) == 0) CC = EQ; else CC = GT;"        }, // $30 REDC
{ "REDC,r1", "",              "r1 = IOPORT(CTRL);"                   , "Read Control"                                            , "RS"                          , "CC"                  , "if (IOPORT(CTRL) & $80) CC = LT; elif (IOPORT(CTRL) == 0) CC = EQ; else CC = GT;"        }, // $31 REDC
{ "REDC,r2", "",              "r2 = IOPORT(CTRL);"                   , "Read Control"                                            , "RS"                          , "CC"                  , "if (IOPORT(CTRL) & $80) CC = LT; elif (IOPORT(CTRL) == 0) CC = EQ; else CC = GT;"        }, // $32 REDC
{ "REDC,r3", "",              "r3 = IOPORT(CTRL);"                   , "Read Control"                                            , "RS"                          , "CC"                  , "if (IOPORT(CTRL) & $80) CC = LT; elif (IOPORT(CTRL) == 0) CC = EQ; else CC = GT;"        }, // $33 REDC
{ "RETE,eq", "",              "if == then { PSU &= ~PSU_II; return; }","Return from Subroutine and Enable Interrupt, Conditional", "SP, CC"                      , "II, SP"              , "Unchanged"                                                                 }, // $34 RETE or, "if (PSL & $C0) == $00 { PSU &= $DF; return; }"
{ "RETE,gt", "",              "if > then { PSU &= ~PSU_II; return; }", "Return from Subroutine and Enable Interrupt, Conditional", "SP, CC"                      , "II, SP"              , "Unchanged"                                                                 }, // $35 RETE or, "if (PSL & $C0) == $40 { PSU &= $DF; return; }"
{ "RETE,lt", "",              "if < then { PSU &= ~PSU_II; return; }", "Return from Subroutine and Enable Interrupt, Conditional", "SP, CC"                      , "II, SP"              , "Unchanged"                                                                 }, // $36 RETE or, "if (PSL & $C0) == $80 { PSU &= $DF; return; }"
{ "RETE,un", "",              "PSU &= ~PSU_II; return;"              , "Return from Subroutine and Enable Interrupt"             , "SP"                          , "II, SP"              , "Unchanged"                                                                 }, // $37 RETE or, "PSU &= $DF; return;"
{ "BSTR,eq", "$aaaa",         "if == gosub $aaaa;"                   , "Branch to Subroutine on Condition True, Relative"        , "SP, CC"                      , "SP"                  , "Unchanged"                                                                 }, // $38 BSTR or, "if (PSL & $C0) == $00 gosub $aaaa;"
{ "BSTR,gt", "$aaaa",         "if > gosub $aaaa;"                    , "Branch to Subroutine on Condition True, Relative"        , "SP, CC"                      , "SP"                  , "Unchanged"                                                                 }, // $39 BSTR or, "if (PSL & $C0) == $40 gosub $aaaa;"
{ "BSTR,lt", "$aaaa",         "if < gosub $aaaa;"                    , "Branch to Subroutine on Condition True, Relative"        , "SP, CC"                      , "SP"                  , "Unchanged"                                                                 }, // $3A BSTR or, "if (PSL & $C0) == $80 gosub $aaaa;"
{ "BSR",     "$aaaa",         "gosub $aaaa;"                         , "Branch to Subroutine, Relative"                          , "SP"                          , "SP"                  , "Unchanged"                                                                 }, // $3B BSTR
{ "BSTA,eq", "$aaaa",         "if == gosub $aaaa;"                   , "Branch to Subroutine on Condition True, Absolute"        , "SP, CC"                      , "SP"                  , "Unchanged"                                                                 }, // $3C BSTA or, "if (PSL & $C0) == $00 gosub $aaaa;"
{ "BSTA,gt", "$aaaa",         "if > gosub $aaaa;"                    , "Branch to Subroutine on Condition True, Absolute"        , "SP, CC"                      , "SP"                  , "Unchanged"                                                                 }, // $3D BSTA or, "if (PSL & $C0) == $40 gosub $aaaa;"
{ "BSTA,lt", "$aaaa",         "if < gosub $aaaa;"                    , "Branch to Subroutine on Condition True, Absolute"        , "SP, CC"                      , "SP"                  , "Unchanged"                                                                 }, // $3E BSTA or, "if (PSL & $C0) == $80 gosub $aaaa;"
{ "BSA",     "$aaaa",         "gosub $aaaa;"                         , "Branch to Subroutine, Absolute"                          , "SP"                          , "SP"                  , "Unchanged"                                                                 }, // $3F BSTA
{ "HALT",    "",              "for (;;);"                            , "Halt, Enter Wait State"                                  , "None"                        , "None"                , "Unchanged"                                                                 }, // $40 HALT
{ "ANDZ r1", "",              "r0 &= r1;"                            , "AND to Register Zero"                                    , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $41 AND
{ "ANDZ r2", "",              "r0 &= r2;"                            , "AND to Register Zero"                                    , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $42 AND
{ "ANDZ r3", "",              "r0 &= r3;"                            , "AND to Register Zero"                                    , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $43 AND
{ "ANDI,r0", "$vv",           "r0 &= $vv;"                           , "AND Immediate"                                           , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $44 AND
{ "ANDI,r1", "$vv",           "r1 &= $vv;"                           , "AND Immediate"                                           , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $45 AND
{ "ANDI,r2", "$vv",           "r2 &= $vv;"                           , "AND Immediate"                                           , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $46 AND
{ "ANDI,r3", "$vv",           "r3 &= $vv;"                           , "AND Immediate"                                           , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $47 AND
{ "ANDR,r0", "$aaaa",         "r0 &= *($aaaa);"                      , "AND Relative"                                            , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $48 AND
{ "ANDR,r1", "$aaaa",         "r1 &= *($aaaa);"                      , "AND Relative"                                            , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $49 AND
{ "ANDR,r2", "$aaaa",         "r2 &= *($aaaa);"                      , "AND Relative"                                            , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $4A AND
{ "ANDR,r3", "$aaaa",         "r3 &= *($aaaa);"                      , "AND Relative"                                            , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $4B AND
{ "ANDA,r0", "$aaaa",         "r0 &= *($aaaa);"                      , "AND Absolute"                                            , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $4C AND
{ "ANDA,r1", "$aaaa",         "r1 &= *($aaaa);"                      , "AND Absolute"                                            , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $4D AND
{ "ANDA,r2", "$aaaa",         "r2 &= *($aaaa);"                      , "AND Absolute"                                            , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $4E AND
{ "ANDA,r3", "$aaaa",         "r3 &= *($aaaa);"                      , "AND Absolute"                                            , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $4F AND
{ "RRR,r0",  "",              "r0 >>= 1;"                            , "Rotate Register Right"                                   , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $50 RRR
{ "RRR,r1",  "",              "r1 >>= 1;"                            , "Rotate Register Right"                                   , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $51 RRR
{ "RRR,r2",  "",              "r2 >>= 1;"                            , "Rotate Register Right"                                   , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $52 RRR
{ "RRR,r3",  "",              "r3 >>= 1;"                            , "Rotate Register Right"                                   , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $53 RRR
{ "REDE,r0", "$vv",           "r0 = IOPORT($vv);"                    , "Read Extended"                                           , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $54 REDE
{ "REDE,r1", "$vv",           "r1 = IOPORT($vv);"                    , "Read Extended"                                           , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $55 REDE
{ "REDE,r2", "$vv",           "r2 = IOPORT($vv);"                    , "Read Extended"                                           , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $56 REDE
{ "REDE,r3", "$vv",           "r3 = IOPORT($vv);"                    , "Read Extended"                                           , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $57 REDE
{ "BRNR,r0", "$aaaa",         "if (r0 != 0) goto $aaaa;"             , "Branch on Non-Zero Register, Relative"                   , "None"                        , "None"                , "Unchanged"                                                                 }, // $58 BRN
{ "BRNR,r1", "$aaaa",         "if (r1 != 0) goto $aaaa;"             , "Branch on Non-Zero Register, Relative"                   , "RS"                          , "None"                , "Unchanged"                                                                 }, // $59 BRN
{ "BRNR,r2", "$aaaa",         "if (r2 != 0) goto $aaaa;"             , "Branch on Non-Zero Register, Relative"                   , "RS"                          , "None"                , "Unchanged"                                                                 }, // $5A BRN
{ "BRNR,r3", "$aaaa",         "if (r3 != 0) goto $aaaa;"             , "Branch on Non-Zero Register, Relative"                   , "RS"                          , "None"                , "Unchanged"                                                                 }, // $5B BRN
{ "BRNA,r0", "$aaaa",         "if (r0 != 0) goto $aaaa;"             , "Branch on Non-Zero Register, Absolute"                   , "None"                        , "None"                , "Unchanged"                                                                 }, // $5C BRN
{ "BRNA,r1", "$aaaa",         "if (r1 != 0) goto $aaaa;"             , "Branch on Non-Zero Register, Absolute"                   , "RS"                          , "None"                , "Unchanged"                                                                 }, // $5D BRN
{ "BRNA,r2", "$aaaa",         "if (r2 != 0) goto $aaaa;"             , "Branch on Non-Zero Register, Absolute"                   , "RS"                          , "None"                , "Unchanged"                                                                 }, // $5E BRN
{ "BRNA,r3", "$aaaa",         "if (r3 != 0) goto $aaaa;"             , "Branch on Non-Zero Register, Absolute"                   , "RS"                          , "None"                , "Unchanged"                                                                 }, // $5F BRN
{ "IORZ r0", "",              ";"                                    , "Inclusive OR to Register Zero"                           , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $60 IOR  or, "r0 |= r0;"
{ "IORZ r1", "",              "r0 |= r1;"                            , "Inclusive OR to Register Zero"                           , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $61 IOR
{ "IORZ r2", "",              "r0 |= r2;"                            , "Inclusive OR to Register Zero"                           , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $62 IOR
{ "IORZ r3", "",              "r0 |= r3;"                            , "Inclusive OR to Register Zero"                           , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $63 IOR
{ "IORI,r0", "$vv",           "r0 |= $vv;"                           , "Inclusive OR Absolute"                                   , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $64 IOR
{ "IORI,r1", "$vv",           "r1 |= $vv;"                           , "Inclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $65 IOR
{ "IORI,r2", "$vv",           "r2 |= $vv;"                           , "Inclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $66 IOR
{ "IORI,r3", "$vv",           "r3 |= $vv;"                           , "Inclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $67 IOR
{ "IORR,r0", "$aaaa",         "r0 |= *($aaaa);"                      , "Inclusive OR Relative"                                   , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $68 IOR
{ "IORR,r1", "$aaaa",         "r1 |= *($aaaa);"                      , "Inclusive OR Relative"                                   , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $69 IOR
{ "IORR,r2", "$aaaa",         "r2 |= *($aaaa);"                      , "Inclusive OR Relative"                                   , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $6A IOR
{ "IORR,r3", "$aaaa",         "r3 |= *($aaaa);"                      , "Inclusive OR Relative"                                   , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $6B IOR
{ "IORA,r0", "$aaaa",         "r0 |= *($aaaa);"                      , "Inclusive OR Absolute"                                   , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $6C IOR
{ "IORA,r1", "$aaaa",         "r1 |= *($aaaa);"                      , "Inclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $6D IOR
{ "IORA,r2", "$aaaa",         "r2 |= *($aaaa);"                      , "Inclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $6E IOR
{ "IORA,r3", "$aaaa",         "r3 |= *($aaaa);"                      , "Inclusive OR Absolute"                                   , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $6F IOR
{ "REDD,r0", "",              "r0 = IOPORT(DATA);"                   , "Read Data"                                               , "None"                        , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $70 REDD
{ "REDD,r1", "",              "r1 = IOPORT(DATA);"                   , "Read Data"                                               , "RS"                          , "CC"                  , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $71 REDD
{ "REDD,r2", "",              "r2 = IOPORT(DATA);"                   , "Read Data"                                               , "RS"                          , "CC"                  , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $72 REDD
{ "REDD,r3", "",              "r3 = IOPORT(DATA);"                   , "Read Data"                                               , "RS"                          , "CC"                  , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $73 REDD
{ "CPSU",    "$vv",           "PSU &= ~($vv & %01100111);"           , "Clear Program Status, Upper"                             , "None"                        , "F, II, UF1, UF2, SP" , "Unchanged"                                                                 }, // $74 CPSU
{ "CPSL",    "$vv",           "PSL &= ~($vv);"                       , "Clear Program Status, Lower"                             , "None"                        , "CC, IDC, RS, WC, OVF, COM, C", "CC &= ~(($vv & $C0) >> 6);"                                        }, // $75 CPSL
{ "PPSU",    "$vv",           "PSU |= ($vv & %01100111);"            , "Preset Program Status, Upper"                            , "None"                        , "F, II, UF1, UF2, SP" , "Unchanged"                                                                 }, // $76 PPSU
{ "PPSL",    "$vv",           "PSL |= $vv;"                          , "Preset Program Status, Lower"                            , "None"                        , "CC, IDC, RS, WC, OVF, COM, C", "CC |= ($vv & $C0) >> 6;"                                           }, // $77 PPSL
{ "BSNR,r0", "$aaaa",         "if (r0 != 0) gosub $aaaa;"            , "Branch to Subroutine on Non-Zero Register, Relative"     , "SP"                          , "SP"                  , "Unchanged"                                                                 }, // $78 BSNR
{ "BSNR,r1", "$aaaa",         "if (r1 != 0) gosub $aaaa;"            , "Branch to Subroutine on Non-Zero Register, Relative"     , "SP, RS"                      , "SP"                  , "Unchanged"                                                                 }, // $79 BSNR
{ "BSNR,r2", "$aaaa",         "if (r2 != 0) gosub $aaaa;"            , "Branch to Subroutine on Non-Zero Register, Relative"     , "SP, RS"                      , "SP"                  , "Unchanged"                                                                 }, // $7A BSNR
{ "BSNR,r3", "$aaaa",         "if (r3 != 0) gosub $aaaa;"            , "Branch to Subroutine on Non-Zero Register, Relative"     , "SP, RS"                      , "SP"                  , "Unchanged"                                                                 }, // $7B BSNR
{ "BSNA,r0", "$aaaa",         "if (r0 != 0) gosub $aaaa;"            , "Branch to Subroutine on Non-Zero Register, Absolute"     , "SP"                          , "SP"                  , "Unchanged"                                                                 }, // $7C BSNA
{ "BSNA,r1", "$aaaa",         "if (r1 != 0) gosub $aaaa;"            , "Branch to Subroutine on Non-Zero Register, Absolute"     , "SP, RS"                      , "SP"                  , "Unchanged"                                                                 }, // $7D BSNA
{ "BSNA,r2", "$aaaa",         "if (r2 != 0) gosub $aaaa;"            , "Branch to Subroutine on Non-Zero Register, Absolute"     , "SP, RS"                      , "SP"                  , "Unchanged"                                                                 }, // $7E BSNA
{ "BSNA,r3", "$aaaa",         "if (r3 != 0) gosub $aaaa;"            , "Branch to Subroutine on Non-Zero Register, Absolute"     , "SP, RS"                      , "SP"                  , "Unchanged"                                                                 }, // $7F BSNA
{ "ADDZ r0", "",              "r0 *= 2;"                             , "Add to Register Zero"                                    , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $80 ADDZ or, "r0 += r0;"
{ "ADDZ r1", "",              "r0 += r1;"                            , "Add to Register Zero"                                    , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $81 ADDZ
{ "ADDZ r2", "",              "r0 += r2;"                            , "Add to Register Zero"                                    , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $82 ADDZ
{ "ADDZ r3", "",              "r0 += r3;"                            , "Add to Register Zero"                                    , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $83 ADDZ
{ "ADDI,r0", "$vv",           "r0 += $vv;"                           , "Add Immediate"                                           , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $84 ADDI
{ "ADDI,r1", "$vv",           "r1 += $vv;"                           , "Add Immediate"                                           , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $85 ADDI
{ "ADDI,r2", "$vv",           "r2 += $vv;"                           , "Add Immediate"                                           , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $86 ADDI
{ "ADDI,r3", "$vv",           "r3 += $vv;"                           , "Add Immediate"                                           , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $87 ADDI
{ "ADDR,r0", "$aaaa",         "r0 += *($aaaa);"                      , "Add Relative"                                            , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $88 ADDR
{ "ADDR,r1", "$aaaa",         "r1 += *($aaaa);"                      , "Add Relative"                                            , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $89 ADDR
{ "ADDR,r2", "$aaaa",         "r2 += *($aaaa);"                      , "Add Relative"                                            , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $8A ADDR
{ "ADDR,r3", "$aaaa",         "r3 += *($aaaa);"                      , "Add Relative"                                            , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $8B ADDR
{ "ADDA,r0", "$aaaa",         "r0 += *($aaaa);"                      , "Add Absolute"                                            , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $8C ADDA
{ "ADDA,r1", "$aaaa",         "r1 += *($aaaa);"                      , "Add Absolute"                                            , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $8D ADDA
{ "ADDA,r2", "$aaaa",         "r2 += *($aaaa);"                      , "Add Absolute"                                            , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $8E ADDA
{ "ADDA,r3", "$aaaa",         "r3 += *($aaaa);"                      , "Add Absolute"                                            , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $8F ADDA
{ "-------", "",              "?"                                    , "-"                                                       , "None"                        , "None"                , "Undefined"                                                                 }, // $90 ---
{ "-------", "",              "?"                                    , "-"                                                       , "None"                        , "None"                , "Undefined"                                                                 }, // $91 ---
{ "LPSU",    "",              "PSU &= %10000000; PSU |= (r0 & %01100111);", "Load Program Status, Upper"                         , "None"                        , "F, II, UF1, UF2, SP" , "Unchanged"                                                                 }, // $92 LPSU
{ "LPSL",    "",              "PSL = r0;"                            , "Load Program Status, Lower from Register Zero"           , "None"                        , "CC, IDC, RS, WC, OVF, COM, C", "CC = ($vv & $C0) >> 6;"                                            }, // $93 LPSL
{ "DAR,r0",  "",              "r0 = BCD(r0);"                        , "Decimal Adjust Register"                                 , "IDC, C"                      , "CC"                  , "Meaningless"                                                               }, // $94 DAR
{ "DAR,r1",  "",              "r1 = BCD(r1);"                        , "Decimal Adjust Register"                                 , "IDC, RS, C"                  , "CC"                  , "Meaningless"                                                               }, // $95 DAR
{ "DAR,r2",  "",              "r2 = BCD(r2);"                        , "Decimal Adjust Register"                                 , "IDC, RS, C"                  , "CC"                  , "Meaningless"                                                               }, // $96 DAR
{ "DAR,r3",  "",              "r3 = BCD(r3);"                        , "Decimal Adjust Register"                                 , "IDC, RS, C"                  , "CC"                  , "Meaningless"                                                               }, // $97 DAR
{ "BNER/BNZR", "$aaaa",       "if != goto $aaaa;"                    , "Branch if Not Equal, Relative\n" \
                                                                       "or Branch if Not Zero, Relative"                         , "CC"                          , "None"                , "Unchanged"                                                                 }, // $98 BCFR,eq or, "if (PSL & $C0) != $00 goto $aaaa;"
{ "BNHR/BNPR", "$aaaa",       "if <= goto $aaaa;"                    , "Branch if Not Higher, Relative\n" \
                                                                       "or Branch if Not Positive, Relative"                     , "CC"                          , "None"                , "Unchanged"                                                                 }, // $99 BCFR,gt or, "if (PSL & $C0) != $40 goto $aaaa;"
{ "BNLR/BNMR", "$aaaa",       "if >= goto $aaaa;"                    , "Branch if Not Lower, Relative\n" \
                                                                       "or Branch if Not Minus, Relative"                        , "CC"                          , "None"                , "Unchanged"                                                                 }, // $9A BCFR,lt or, "if (PSL & $C0) != $80 goto $aaaa;"
{ "ZBRR",    "$aaaa",         "goto $aaaa;"                          , "Zero Branch, Relative"                                   , "None"                        , "None"                , "Unchanged"                                                                 }, // $9B ZBRR
{ "BNEA/BNZA", "$aaaa",       "if != goto $aaaa;"                    , "Branch if Not Equal, Absolute\n" \
                                                                       "or Branch if Not Zero, Absolute"                         , "CC"                          , "None"                , "Unchanged"                                                                 }, // $9C BCFA,eq or, "if (PSL & $C0) != $00 goto $aaaa;"
{ "BNHA/BNPA", "$aaaa",       "if <= goto $aaaa;"                    , "Branch if Not Higher, Absolute\n" \
                                                                       "or Branch if Not Positive, Absolute"                     , "CC"                          , "None"                , "Unchanged"                                                                 }, // $9D BCFA,gt or, "if (PSL & $C0) != $40 goto $aaaa;"
{ "BNLA/BNMA", "$aaaa",       "if >= goto $aaaa;"                    , "Branch if Not Lower, Absolute\n" \
                                                                       "or Branch if Not Minus, Absolute"                        , "CC"                          , "None"                , "Unchanged"                                                                 }, // $9E BCFA,lt or, "if (PSL & $C0) != $80 goto $aaaa;"
{ "BXA,r3",  "BXA $aaaa,r3",  "goto $aaaa + r3;"                     , "Branch Indexed, Absolute"                                , "RS"                          , "None"                , "Unchanged"                                                                 }, // $9F BXA
{ "SUBZ r0", "",              "r0 -= r0;"                            , "Subtract from Register Zero"                             , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $A0 SUBZ    or, "r0 -= r0;". CC is not simply "CC = EQ;" because the result might not be zero (due to borrow).
{ "SUBZ r1", "",              "r0 -= r1;"                            , "Subtract from Register Zero"                             , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $A1 SUBZ
{ "SUBZ r2", "",              "r0 -= r2;"                            , "Subtract from Register Zero"                             , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $A2 SUBZ
{ "SUBZ r3", "",              "r0 -= r3;"                            , "Subtract from Register Zero"                             , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $A3 SUBZ
{ "SUBI,r0", "$vv",           "r0 -= $vv;"                           , "Subtract Immediate"                                      , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $A4 SUBI
{ "SUBI,r1", "$vv",           "r1 -= $vv;"                           , "Subtract Immediate"                                      , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $A5 SUBI
{ "SUBI,r2", "$vv",           "r2 -= $vv;"                           , "Subtract Immediate"                                      , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $A6 SUBI
{ "SUBI,r3", "$vv",           "r3 -= $vv;"                           , "Subtract Immediate"                                      , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $A7 SUBI
{ "SUBR,r0", "$aaaa",         "r0 -= *($aaaa);"                      , "Subtract Relative"                                       , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $A8 SUBR
{ "SUBR,r1", "$aaaa",         "r1 -= *($aaaa);"                      , "Subtract Relative"                                       , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $A9 SUBR
{ "SUBR,r2", "$aaaa",         "r2 -= *($aaaa);"                      , "Subtract Relative"                                       , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $AA SUBR
{ "SUBR,r3", "$aaaa",         "r3 -= *($aaaa);"                      , "Subtract Relative"                                       , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $AB SUBR
{ "SUBA,r0", "$aaaa",         "r0 -= *($aaaa);"                      , "Subtract Absolute"                                       , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $AC SUBA
{ "SUBA,r1", "$aaaa",         "r1 -= *($aaaa);"                      , "Subtract Absolute"                                       , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $AD SUBA
{ "SUBA,r2", "$aaaa",         "r2 -= *($aaaa);"                      , "Subtract Absolute"                                       , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $AE SUBA
{ "SUBA,r3", "$aaaa",         "r3 -= *($aaaa);"                      , "Subtract Absolute"                                       , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $AF SUBA
{ "WRTC,r0", "",              "IOPORT(CTRL) = r0;"                   , "Write Control"                                           , "None"                        , "None"                , "Unchanged"                                                                 }, // $B0 WRTC
{ "WRTC,r1", "",              "IOPORT(CTRL) = r1;"                   , "Write Control"                                           , "RS"                          , "None"                , "Unchanged"                                                                 }, // $B1 WRTC
{ "WRTC,r2", "",              "IOPORT(CTRL) = r2;"                   , "Write Control"                                           , "RS"                          , "None"                , "Unchanged"                                                                 }, // $B2 WRTC
{ "WRTC,r3", "",              "IOPORT(CTRL) = r3;"                   , "Write Control"                                           , "RS"                          , "None"                , "Unchanged"                                                                 }, // $B3 WRTC
{ "TPSU",    "$vv",           "CC = (PSU & $vv == $vv) ? EQ : LT;"   , "Test Program Status, Upper"                              , "S, F, II, UF1, UF2, SP"      , "CC"                  , "CC = (PSU & $vv == $vv) ? EQ : LT;"                                        }, // $B4 TPSU
{ "TPSL",    "$vv",           "CC = (PSL & $vv == $vv) ? EQ : LT;"   , "Test Program Status, Lower"                              , "CC, IDC, RS, WC, OVF, COM, C", "CC"                  , "CC = (PSL & $vv == $vv) ? EQ : LT;"                                        }, // $B5 TPSL
{ "-------", "",              "?"                                    , "-"                                                       , "None"                        , "None"                , "Undefined"                                                                 }, // $B6 ---
{ "-------", "",              "?"                                    , "-"                                                       , "None"                        , "None"                , "Undefined"                                                                 }, // $B7 ---
{ "BSFR,eq", "$aaaa",         "if != gosub $aaaa;"                   , "Branch to Subroutine on Condition False, Relative"       , "SP, CC"                      , "SP"                  , "Unchanged"                                                                 }, // $B8 BSFR,eq or, "if (PSL & $C0) != $00 gosub $aaaa;"
{ "BSFR,gt", "$aaaa",         "if <= gosub $aaaa;"                   , "Branch to Subroutine on Condition False, Relative"       , "SP, CC"                      , "SP"                  , "Unchanged"                                                                 }, // $B9 BSFR,gt or, "if (PSL & $C0) != $40 gosub $aaaa;"
{ "BSFR,lt", "$aaaa",         "if >= gosub $aaaa;"                   , "Branch to Subroutine on Condition False, Relative"       , "SP, CC"                      , "SP"                  , "Unchanged"                                                                 }, // $BA BSFR,lt or, "if (PSL & $C0) != $80 gosub $aaaa;"
{ "ZBSR",    "$aaaa",         "gosub $aaaa;"                         , "Zero Branch to Subroutine, Relative"                     , "SP"                          , "SP"                  , "Unchanged"                                                                 }, // $BB ZBSR
{ "BSFA,eq", "$aaaa",         "if != gosub $aaaa;"                   , "Branch to Subroutine on Condition False, Absolute"       , "SP, CC"                      , "SP"                  , "Unchanged"                                                                 }, // $BC BSFA    or, "if (PSL & $C0) != $00 gosub $aaaa;"
{ "BSFA,gt", "$aaaa",         "if <= gosub $aaaa;"                   , "Branch to Subroutine on Condition False, Absolute"       , "SP, CC"                      , "SP"                  , "Unchanged"                                                                 }, // $BD BSFA    or, "if (PSL & $C0) != $40 gosub $aaaa;"
{ "BSFA,lt", "$aaaa",         "if >= gosub $aaaa;"                   , "Branch to Subroutine on Condition False, Absolute"       , "SP, CC"                      , "SP"                  , "Unchanged"                                                                 }, // $BE BSFA    or, "if (PSL & $C0) != $80 gosub $aaaa;"
{ "BSXA,r3", "BSXA $aaaa,r3", "gosub $aaaa + r3;"                    , "Branch to Subroutine Indexed, Absolute"                  , "SP, RS"                      , "SP"                  , "Unchanged"                                                                 }, // $BF BSXA
{ "NOP",     "",              ";"                                    , "No Operation"                                            , "None"                        , "None"                , "Unchanged"                                                                 }, // $C0 NOP
{ "STRZ r1", "",              "r1 = r0;"                             , "Store to Register Zero"                                  , "RS"                          , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $C1 STRZ
{ "STRZ r2", "",              "r2 = r0;"                             , "Store to Register Zero"                                  , "RS"                          , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $C2 STRZ
{ "STRZ r3", "",              "r3 = r0;"                             , "Store to Register Zero"                                  , "RS"                          , "CC"                  , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $C3 STRZ
{ "-------", "",              "?"                                    , "-"                                                       , "None"                        , "None"                , "Undefined"                                                                 }, // $C4 ---
{ "-------", "",              "?"                                    , "-"                                                       , "None"                        , "None"                , "Undefined"                                                                 }, // $C5 ---
{ "-------", "",              "?"                                    , "-"                                                       , "None"                        , "None"                , "Undefined"                                                                 }, // $C6 ---
{ "-------", "",              "?"                                    , "-"                                                       , "None"                        , "None"                , "Undefined"                                                                 }, // $C7 ---
{ "STRR,r0", "$aaaa",         "*($aaaa) = r0;"                       , "Store Relative"                                          , "None"                        , "None"                , "Unchanged"                                                                 }, // $C8 STRR
{ "STRR,r1", "$aaaa",         "*($aaaa) = r1;"                       , "Store Relative"                                          , "RS"                          , "None"                , "Unchanged"                                                                 }, // $C9 STRR
{ "STRR,r2", "$aaaa",         "*($aaaa) = r2;"                       , "Store Relative"                                          , "RS"                          , "None"                , "Unchanged"                                                                 }, // $CA STRR
{ "STRR,r3", "$aaaa",         "*($aaaa) = r3;"                       , "Store Relative"                                          , "RS"                          , "None"                , "Unchanged"                                                                 }, // $CB STRR
{ "STRA,r0", "$aaaa",         "*($aaaa) = r0;"                       , "Store Absolute"                                          , "None"                        , "None"                , "Unchanged"                                                                 }, // $CC STRA
{ "STRA,r1", "$aaaa",         "*($aaaa) = r1;"                       , "Store Absolute"                                          , "RS"                          , "None"                , "Unchanged"                                                                 }, // $CD STRA
{ "STRA,r2", "$aaaa",         "*($aaaa) = r2;"                       , "Store Absolute"                                          , "RS"                          , "None"                , "Unchanged"                                                                 }, // $CE STRA
{ "STRA,r3", "$aaaa",         "*($aaaa) = r3;"                       , "Store Absolute"                                          , "RS"                          , "None"                , "Unchanged"                                                                 }, // $CF STRA
{ "RRL,r0",  "",              "r0 <<= 1;"                            , "Rotate Register Left"                                    , "WC, C"                       , "CC, IDC, OVF, C"     , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"              }, // $D0 RRL
{ "RRL,r1",  "",              "r1 <<= 1;"                            , "Rotate Register Left"                                    , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"              }, // $D1 RRL
{ "RRL,r2",  "",              "r2 <<= 1;"                            , "Rotate Register Left"                                    , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"              }, // $D2 RRL
{ "RRL,r3",  "",              "r3 <<= 1;"                            , "Rotate Register Left"                                    , "RS, WC, C"                   , "CC, IDC, OVF, C"     , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"              }, // $D3 RRL
{ "WRTE,r0", "$vv",           "IOPORT($vv) = r0;"                    , "Write Extended"                                          , "None"                        , "None"                , "Unchanged"                                                                 }, // $D4 WRTE
{ "WRTE,r1", "$vv",           "IOPORT($vv) = r1;"                    , "Write Extended"                                          , "RS"                          , "None"                , "Unchanged"                                                                 }, // $D5 WRTE
{ "WRTE,r2", "$vv",           "IOPORT($vv) = r2;"                    , "Write Extended"                                          , "RS"                          , "None"                , "Unchanged"                                                                 }, // $D6 WRTE
{ "WRTE,r3", "$vv",           "IOPORT($vv) = r3;"                    , "Write Extended"                                          , "RS"                          , "None"                , "Unchanged"                                                                 }, // $D7 WRTE
{ "BIRR,r0", "$aaaa",         "if (++r0 != 0) goto $aaaa;"           , "Branch on Incrementing Register, Relative"               , "None"                        , "None"                , "Unchanged"                                                                 }, // $D8 BIRR
{ "BIRR,r1", "$aaaa",         "if (++r1 != 0) goto $aaaa;"           , "Branch on Incrementing Register, Relative"               , "RS"                          , "None"                , "Unchanged"                                                                 }, // $D9 BIRR
{ "BIRR,r2", "$aaaa",         "if (++r2 != 0) goto $aaaa;"           , "Branch on Incrementing Register, Relative"               , "RS"                          , "None"                , "Unchanged"                                                                 }, // $DA BIRR
{ "BIRR,r3", "$aaaa",         "if (++r3 != 0) goto $aaaa;"           , "Branch on Incrementing Register, Relative"               , "RS"                          , "None"                , "Unchanged"                                                                 }, // $DB BIRR
{ "BIRA,r0", "$aaaa",         "if (++r0 != 0) goto $aaaa;"           , "Branch on Incrementing Register, Absolute"               , "None"                        , "None"                , "Unchanged"                                                                 }, // $DC BIRA
{ "BIRA,r1", "$aaaa",         "if (++r1 != 0) goto $aaaa;"           , "Branch on Incrementing Register, Absolute"               , "RS"                          , "None"                , "Unchanged"                                                                 }, // $DD BIRA
{ "BIRA,r2", "$aaaa",         "if (++r2 != 0) goto $aaaa;"           , "Branch on Incrementing Register, Absolute"               , "RS"                          , "None"                , "Unchanged"                                                                 }, // $DE BIRA
{ "BIRA,r3", "$aaaa",         "if (++r3 != 0) goto $aaaa;"           , "Branch on Incrementing Register, Absolute"               , "RS"                          , "None"                , "Unchanged"                                                                 }, // $DF BIRA
{ "COMZ r0", "",              "CC = EQ;"                             , "Compare to Register Zero"                                , "COM"                         , "CC"                  , "CC = EQ;"                                                                  }, // $E0 COMZ
{ "COMZ r1", "",              "compare r0 against r1;"               , "Compare to Register Zero"                                , "RS, COM"                     , "CC"                  , "if (r0 > r1) CC = GT; elif (r0 < r1) CC = LT; else CC = EQ;"               }, // $E1 COMZ
{ "COMZ r2", "",              "compare r0 against r2;"               , "Compare to Register Zero"                                , "RS, COM"                     , "CC"                  , "if (r0 > r2) CC = GT; elif (r0 < r2) CC = LT; else CC = EQ;"               }, // $E2 COMZ
{ "COMZ r3", "",              "compare r0 against r3;"               , "Compare to Register Zero"                                , "RS, COM"                     , "CC"                  , "if (r0 > r3) CC = GT; elif (r0 < r3) CC = LT; else CC = EQ;"               }, // $E3 COMZ
{ "COMI,r0", "$vv",           "compare r0 against $vv;"              , "Compare Immediate"                                       , "COM"                         , "CC"                  , "if (r0 > $vv) CC = GT; elif (r0 < $vv) CC = LT; else CC = EQ;"             }, // $E4 COMI
{ "COMI,r1", "$vv",           "compare r1 against $vv;"              , "Compare Immediate"                                       , "RS, COM"                     , "CC"                  , "if (r1 > $vv) CC = GT; elif (r1 < $vv) CC = LT; else CC = EQ;"             }, // $E5 COMI
{ "COMI,r2", "$vv",           "compare r2 against $vv;"              , "Compare Immediate"                                       , "RS, COM"                     , "CC"                  , "if (r2 > $vv) CC = GT; elif (r2 < $vv) CC = LT; else CC = EQ;"             }, // $E6 COMI
{ "COMI,r3", "$vv",           "compare r3 against $vv;"              , "Compare Immediate"                                       , "RS, COM"                     , "CC"                  , "if (r3 > $vv) CC = GT; elif (r3 < $vv) CC = LT; else CC = EQ;"             }, // $E7 COMI
{ "COMR,r0", "$aaaa",         "compare r0 against *($aaaa);"         , "Compare Relative"                                        , "COM"                         , "CC"                  , "if (r0 > *($aaaa)) CC = GT; elif (r0 < *($aaaa)) CC = LT; else CC = EQ;"   }, // $E8 COMR
{ "COMR,r1", "$aaaa",         "compare r1 against *($aaaa);"         , "Compare Relative"                                        , "RS, COM"                     , "CC"                  , "if (r1 > *($aaaa)) CC = GT; elif (r1 < *($aaaa)) CC = LT; else CC = EQ;"   }, // $E9 COMR
{ "COMR,r2", "$aaaa",         "compare r2 against *($aaaa);"         , "Compare Relative"                                        , "RS, COM"                     , "CC"                  , "if (r2 > *($aaaa)) CC = GT; elif (r2 < *($aaaa)) CC = LT; else CC = EQ;"   }, // $EA COMR
{ "COMR,r3", "$aaaa",         "compare r3 against *($aaaa);"         , "Compare Relative"                                        , "RS, COM"                     , "CC"                  , "if (r3 > *($aaaa)) CC = GT; elif (r3 < *($aaaa)) CC = LT; else CC = EQ;"   }, // $EB COMR
{ "COMA,r0", "$aaaa",         "compare r0 against *($aaaa);"         , "Compare Absolute"                                        , "COM"                         , "CC"                  , "if (r0 > *($aaaa)) CC = GT; elif (r0 < *($aaaa)) CC = LT; else CC = EQ;"   }, // $EC COMA
{ "COMA,r1", "$aaaa",         "compare r1 against *($aaaa);"         , "Compare Absolute"                                        , "RS, COM"                     , "CC"                  , "if (r1 > *($aaaa)) CC = GT; elif (r1 < *($aaaa)) CC = LT; else CC = EQ;"   }, // $ED COMA
{ "COMA,r2", "$aaaa",         "compare r2 against *($aaaa);"         , "Compare Absolute"                                        , "RS, COM"                     , "CC"                  , "if (r2 > *($aaaa)) CC = GT; elif (r2 < *($aaaa)) CC = LT; else CC = EQ;"   }, // $EE COMA
{ "COMA,r3", "$aaaa",         "compare r3 against *($aaaa);"         , "Compare Absolute"                                        , "RS, COM"                     , "CC"                  , "if (r3 > *($aaaa)) CC = GT; elif (r3 < *($aaaa)) CC = LT; else CC = EQ;"   }, // $EF COMA
{ "WRTD,r0", "",              "IOPORT(DATA) = r0;"                   , "Write Data"                                              , "None"                        , "None"                , "Unchanged"                                                                 }, // $F0 WRTD
{ "WRTD,r1", "",              "IOPORT(DATA) = r1;"                   , "Write Data"                                              , "RS"                          , "None"                , "Unchanged"                                                                 }, // $F1 WRTD
{ "WRTD,r2", "",              "IOPORT(DATA) = r2;"                   , "Write Data"                                              , "RS"                          , "None"                , "Unchanged"                                                                 }, // $F2 WRTD
{ "WRTD,r3", "",              "IOPORT(DATA) = r3;"                   , "Write Data"                                              , "RS"                          , "None"                , "Unchanged"                                                                 }, // $F3 WRTD
{ "TMI,r0",  "$vv",           "CC = (r0 & $vv == $vv) ? EQ : LT;"    , "Test under Mask Immediate"                               , "None"                        , "CC"                  , "CC = (r0 & $vv == $vv) ? EQ : LT;"                                         }, // $F4 TMI
{ "TMI,r1",  "$vv",           "CC = (r1 & $vv == $vv) ? EQ : LT;"    , "Test under Mask Immediate"                               , "RS"                          , "CC"                  , "CC = (r1 & $vv == $vv) ? EQ : LT;"                                         }, // $F5 TMI
{ "TMI,r2",  "$vv",           "CC = (r2 & $vv == $vv) ? EQ : LT;"    , "Test under Mask Immediate"                               , "RS"                          , "CC"                  , "CC = (r2 & $vv == $vv) ? EQ : LT;"                                         }, // $F6 TMI
{ "TMI,r3",  "$vv",           "CC = (r3 & $vv == $vv) ? EQ : LT;"    , "Test under Mask Immediate"                               , "RS"                          , "CC"                  , "CC = (r3 & $vv == $vv) ? EQ : LT;"                                         }, // $F7 TMI
{ "BDRR,r0", "$aaaa",         "if (--r0 != 0) goto $aaaa;"           , "Branch on Decrementing Register, Relative"               , "None"                        , "None"                , "Unchanged"                                                                 }, // $F8 BDR
{ "BDRR,r1", "$aaaa",         "if (--r1 != 0) goto $aaaa;"           , "Branch on Decrementing Register, Relative"               , "RS"                          , "None"                , "Unchanged"                                                                 }, // $F9 BDR
{ "BDRR,r2", "$aaaa",         "if (--r2 != 0) goto $aaaa;"           , "Branch on Decrementing Register, Relative"               , "RS"                          , "None"                , "Unchanged"                                                                 }, // $FA BDR
{ "BDRR,r3", "$aaaa",         "if (--r3 != 0) goto $aaaa;"           , "Branch on Decrementing Register, Relative"               , "RS"                          , "None"                , "Unchanged"                                                                 }, // $FB BDR
{ "BDRA,r0", "$aaaa",         "if (--r0 != 0) goto $aaaa;"           , "Branch on Decrementing Register, Absolute"               , "None"                        , "None"                , "Unchanged"                                                                 }, // $FC BDR
{ "BDRA,r1", "$aaaa",         "if (--r1 != 0) goto $aaaa;"           , "Branch on Decrementing Register, Absolute"               , "RS"                          , "None"                , "Unchanged"                                                                 }, // $FD BDR
{ "BDRA,r2", "$aaaa",         "if (--r2 != 0) goto $aaaa;"           , "Branch on Decrementing Register, Absolute"               , "RS"                          , "None"                , "Unchanged"                                                                 }, // $FE BDR
{ "BDRA,r3", "$aaaa",         "if (--r3 != 0) goto $aaaa;"           , "Branch on Decrementing Register, Absolute"               , "RS"                          , "None"                , "Unchanged"                                                                 }, // $FF BDR
}, {
// Old CALM format. We use LOGICOMP instead of L because L is also used by CALM to mean the PSL.
{ "LOAD A,A",    ""            , ";"                                    , "Load A with A"                                    , "None"                               , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $00
{ "LOAD A,B",    ""            , "A = B;"                               , "Load A with B"                                    , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $01
{ "LOAD A,C",    ""            , "A = C;"                               , "Load A with C"                                    , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $02
{ "LOAD A,D",    ""            , "A = D;"                               , "Load A with D"                                    , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $03
{ "LOAD A",      ",#imm"       , "A = imm;"                             , "Load A with imm"                                  , "None"                               , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $04 LOAD A,#<value>
{ "LOAD B",      ",#imm"       , "B = imm;"                             , "Load B with imm"                                  , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $05 LOAD B,#<value>
{ "LOAD C",      ",#imm"       , "C = imm;"                             , "Load C with imm"                                  , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $06 LOAD C,#<value>
{ "LOAD D",      ",#imm"       , "D = imm;"                             , "Load D with imm"                                  , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $07 LOAD D,#<value>
{ "LOAD A",      ",.+rel"      , "A = *(rel);"                          , "Load A with *(rel)"                               , "None"                               , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $08 LOAD A,<relative>
{ "LOAD B",      ",.+rel"      , "B = *(rel);"                          , "Load B with *(rel)"                               , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $09 LOAD B,<relative>
{ "LOAD C",      ",.+rel"      , "C = *(rel);"                          , "Load C with *(rel)"                               , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $0A LOAD C,<relative>
{ "LOAD D",      ",.+rel"      , "D = *(rel);"                          , "Load D with *(rel)"                               , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $0B LOAD D,<relative>
{ "LOAD A",      ",abs"        , "A = *(abs);"                          , "Load A with *(abs)"                               , "None"                               , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $0C LOAD A,<absolute>
{ "LOAD B",      ",abs"        , "B = *(abs);"                          , "Load B with *(abs)"                               , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $0D LOAD B,<absolute>
{ "LOAD C",      ",abs"        , "C = *(abs);"                          , "Load C with *(abs)"                               , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $0E LOAD C,<absolute>
{ "LOAD D",      ",abs"        , "D = *(abs);"                          , "Load D with *(abs)"                               , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $0F LOAD D,<absolute>
{ "LOAD L",      ",abs"        , "L = *(abs);"                          , "Load L with *(abs)"                               , "None"                               , "CC, H, B, W, V, LOGICOMP, C", "CC = (*(abs) & $C0) >> 6;"                                          }, // $10 LOAD L,<absolute>. 2560-B only
{ "LOAD L",      "LOAD abs,L"  , "*(abs) = L;"                          , "Load *(abs) with L"                               , "CC, H, B, W, V, LOGICOMP, C"        , "None"                       , "Undefined"                                                          }, // $11 LOAD <absolute>,L. 2560-B only
{ "LOAD A,U",    ""            , "A = U;"                               , "Load A with U"                                    , "INPUT, OUTPUT, IOF, UF1, UF2, STACK", "CC"                         , "if (INPUT) CC = LT; elif (PSU == 0) CC = EQ; else CC = GT;"         }, // $12
{ "LOAD A,L",    ""            , "A = L;"                               , "Load A with L"                                    , "CC, H, B, W, V, LOGICOMP, C"        , "CC"                         , "if (CC == LT || CC == UN) CC = LT; elif (PSL == 0) CC = EQ; else CC = GT;" }, // $13
{ "RET,EQ",      ""            , "if == return;"                        , "Return if EQ true"                                , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $14
{ "RET,GT",      ""            , "if > return;"                         , "Return if GT true"                                , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $15
{ "RET,LT",      ""            , "if < return;"                         , "Return if LT true"                                , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $16
{ "RET",         ""            , "return;"                              , "Return"                                           , "STACK"                              , "STACK"                      , "Unchanged"                                                          }, // $17
{ "JUMP,EQ",     " .+rel"      , "if == goto rel;"                      , "Jump if EQ true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $18 JUMP EQ <relative>
{ "JUMP,GT",     " .+rel"      , "if > goto rel;"                       , "Jump if GT true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $19 JUMP GT <relative>
{ "JUMP,LT",     " .+rel"      , "if < goto rel;"                       , "Jump if LT true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $1A JUMP LT <relative>
{ "JUMP",        " .+rel"      , "goto rel;"                            , "Jump"                                             , "None"                               , "None"                       , "Unchanged"                                                          }, // $1B JUMP <relative>
{ "JUMP,EQ",     " abs"        , "if == goto abs;"                      , "Jump if EQ true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $1C JUMP EQ <absolute>
{ "JUMP,GT",     " abs"        , "if > goto abs;"                       , "Jump if GT true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $1D JUMP GT <absolute>
{ "JUMP,LT",     " abs"        , "if < goto abs;"                       , "Jump if LT true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $1E JUMP LT <absolute>
{ "JUMP",        " abs"        , "goto abs;"                            , "Jump"                                             , "None"                               , "None"                       , "Unchanged"                                                          }, // $1F JUMP <absolute>
{ "CLR A",       ""            , "A = 0;"                               , "Clear A"                                          , "None"                               , "CC"                         , "CC = EQ;"                                                           }, // $20
{ "XOR A,B",     ""            , "A ^= B;"                              , "Exclusive OR A with B"                            , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $21
{ "XOR A,C",     ""            , "A ^= C;"                              , "Exclusive OR A with C"                            , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $22
{ "XOR A,D",     ""            , "A ^= D;"                              , "Exclusive OR A with D"                            , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $23
{ "XOR A",       ",#imm"       , "A ^= imm;"                            , "Exclusive OR A with imm"                          , "None"                               , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $24 XOR A,#<value>
{ "XOR B",       ",#imm"       , "B ^= imm;"                            , "Exclusive OR B with imm"                          , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $25 XOR B,#<value>
{ "XOR C",       ",#imm"       , "C ^= imm;"                            , "Exclusive OR C with imm"                          , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $26 XOR C,#<value>
{ "XOR D",       ",#imm"       , "D ^= imm;"                            , "Exclusive OR D with imm"                          , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $27 XOR D,#<value>
{ "XOR A",       ",.+rel"      , "A ^= *(rel);"                         , "Exclusive OR A with *(rel)"                       , "None"                               , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $28 XOR A,<relative>
{ "XOR B",       ",.+rel"      , "B ^= *(rel);"                         , "Exclusive OR B with *(rel)"                       , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $29 XOR B,<relative>
{ "XOR C",       ",.+rel"      , "C ^= *(rel);"                         , "Exclusive OR C with *(rel)"                       , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $2A XOR C,<relative>
{ "XOR D",       ",.+rel"      , "D ^= *(rel);"                         , "Exclusive OR D with *(rel)"                       , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $2B XOR D,<relative>
{ "XOR A",       ",abs"        , "A ^= *(abs);"                         , "Exclusive OR A with *(abs)"                       , "None"                               , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $2C XOR A,<absolute>
{ "XOR B",       ",abs"        , "B ^= *(abs);"                         , "Exclusive OR B with *(abs)"                       , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $2D XOR B,<absolute>
{ "XOR C",       ",abs"        , "C ^= *(abs);"                         , "Exclusive OR C with *(abs)"                       , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $2E XOR C,<absolute>
{ "XOR D",       ",abs"        , "D ^= *(abs);"                         , "Exclusive OR D with *(abs)"                       , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $2F XOR D,<absolute>
{ "LOAD A,$CTRL",""            , "A = IOPORT(CTRL);"                    , "Read specially decoded peripheral"                , "None"                               , "CC"                         , "if (IOPORT(CTRL) & $80) CC = LT; elif (IOPORT(CTRL) == 0) CC = EQ; else CC = GT;" }, // $30
{ "LOAD B,$CTRL",""            , "B = IOPORT(CTRL);"                    , "Read specially decoded peripheral"                , "B"                                  , "CC"                         , "if (IOPORT(CTRL) & $80) CC = LT; elif (IOPORT(CTRL) == 0) CC = EQ; else CC = GT;" }, // $31
{ "LOAD C,$CTRL",""            , "C = IOPORT(CTRL);"                    , "Read specially decoded peripheral"                , "B"                                  , "CC"                         , "if (IOPORT(CTRL) & $80) CC = LT; elif (IOPORT(CTRL) == 0) CC = EQ; else CC = GT;" }, // $32
{ "LOAD D,$CTRL",""            , "D = IOPORT(CTRL);"                    , "Read specially decoded peripheral"                , "B"                                  , "CC"                         , "if (IOPORT(CTRL) & $80) CC = LT; elif (IOPORT(CTRL) == 0) CC = EQ; else CC = GT;" }, // $33
{ "RETION,EQ",   ""            , "if == { U &= ~PSU_IOF; return; }"     , "Return and clear interrupt mask if EQ true"       , "STACK, CC"                          , "IOF, STACK"                 , "Unchanged"                                                          }, // $34
{ "RETION,GT",   ""            , "if > { U &= ~PSU_IOF; return; }"      , "Return and clear interrupt mask if GT true"       , "STACK, CC"                          , "IOF, STACK"                 , "Unchanged"                                                          }, // $35
{ "RETION,LT",   ""            , "if < { U &= ~PSU_IOF; return; }"      , "Return and clear interrupt mask if LT true"       , "STACK, CC"                          , "IOF, STACK"                 , "Unchanged"                                                          }, // $36
{ "RETION",      ""            , "U &= ~PSU_IOF; return;"               , "Return and clear interrupt mask"                  , "STACK"                              , "IOF, STACK"                 , "Unchanged"                                                          }, // $37
{ "CALL,EQ",     " .+rel"      , "if == gosub rel;"                     , "Call if EQ true"                                  , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $38 CALL EQ <relative>
{ "CALL,GT",     " .+rel"      , "if > gosub rel;"                      , "Call if GT true"                                  , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $39 CALL GT <relative>
{ "CALL,LT",     " .+rel"      , "if < gosub rel;"                      , "Call if LT true"                                  , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $3A CALL LT <relative>
{ "CALL",        " .+rel"      , "gosub rel;"                           , "Call"                                             , "STACK"                              , "STACK"                      , "Unchanged"                                                          }, // $3B CALL <relative>
{ "CALL,EQ",     " abs"        , "if == gosub abs;"                     , "Call if EQ true"                                  , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $3C CALL EQ <absolute>
{ "CALL,GT",     " abs"        , "if > gosub abs;"                      , "Call if GT true"                                  , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $3D CALL GT <absolute>
{ "CALL,LT",     " abs"        , "if < gosub abs;"                      , "Call if LT true"                                  , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $3E CALL LT <absolute>
{ "CALL",        " abs"        , "gosub abs;"                           , "Call"                                             , "STACK"                              , "STACK"                      , "Unchanged"                                                          }, // $3F CALL <absolute>
{ "WAIT",        ""            , "for (;;);"                            , "Wait for interrupt"                               , "None"                               , "None"                       , "Unchanged"                                                          }, // $40
{ "AND A,B",     ""            , "A &= B;"                              , "Logical AND A with B"                             , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $41
{ "AND A,C",     ""            , "A &= C;"                              , "Logical AND A with C"                             , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $42
{ "AND A,D",     ""            , "A &= D;"                              , "Logical AND A with D"                             , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $43
{ "AND A",       ",#imm"       , "A &= imm;"                            , "Logical AND A with imm"                           , "None"                               , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $44 AND A,#<value>
{ "AND B",       ",#imm"       , "B &= imm;"                            , "Logical AND B with imm"                           , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $45 AND B,#<value>
{ "AND C",       ",#imm"       , "C &= imm;"                            , "Logical AND C with imm"                           , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $46 AND C,#<value>
{ "AND D",       ",#imm"       , "D &= imm;"                            , "Logical AND D with imm"                           , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $47 AND D,#<value>
{ "AND A",       ",.+rel"      , "A &= *(rel);"                         , "Logical AND A with *(rel)"                        , "None"                               , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $48 AND A,<relative>
{ "AND B",       ",.+rel"      , "B &= *(rel);"                         , "Logical AND B with *(rel)"                        , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $49 AND B,<relative>
{ "AND C",       ",.+rel"      , "C &= *(rel);"                         , "Logical AND C with *(rel)"                        , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $4A AND C,<relative>
{ "AND D",       ",.+rel"      , "D &= *(rel);"                         , "Logical AND D with *(rel)"                        , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $4B AND D,<relative>
{ "AND A",       ",abs"        , "A &= *(abs);"                         , "Logical AND A with *(abs)"                        , "None"                               , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $4C AND A,<absolute>
{ "AND B",       ",abs"        , "B &= *(abs);"                         , "Logical AND B with *(abs)"                        , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $4D AND B,<absolute>
{ "AND C",       ",abs"        , "C &= *(abs);"                         , "Logical AND C with *(abs)"                        , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $4E AND C,<absolute>
{ "AND D",       ",abs"        , "D &= *(abs);"                         , "Logical AND D with *(abs)"                        , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $4F AND D,<absolute>
{ "RR A",        ""            , "A >>= 1;"                             , "Rotate right"                                     , "W, C"                               , "CC, H, V, C"                , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $50 or RRC
{ "RR B",        ""            , "B >>= 1;"                             , "Rotate right"                                     , "B, W, C"                            , "CC, H, V, C"                , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $51 or RRC
{ "RR C",        ""            , "C >>= 1;"                             , "Rotate right"                                     , "B, W, C"                            , "CC, H, V, C"                , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $52 or RRC
{ "RR D",        ""            , "D >>= 1;"                             , "Rotate right"                                     , "B, W, C"                            , "CC, H, V, C"                , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $53 or RRC
{ "LOAD A",      ",$port"      , "A = IOPORT(port);"                    , "Read peripheral port data into A"                 , "None"                               , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $54
{ "LOAD B",      ",$port"      , "B = IOPORT(port);"                    , "Read peripheral port data into B"                 , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $55
{ "LOAD C",      ",$port"      , "C = IOPORT(port);"                    , "Read peripheral port data into C"                 , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $56
{ "LOAD D",      ",$port"      , "D = IOPORT(port);"                    , "Read peripheral port data into D"                 , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $57
{ "JUMP,ANE",    " .+rel"      , "if (A != 0) goto rel;"                , "Jump if register A not equal to zero"             , "None"                               , "None"                       , "Unchanged"                                                          }, // $58 (relative)
{ "JUMP,BNE",    " .+rel"      , "if (B != 0) goto rel;"                , "Jump if register B not equal to zero"             , "B"                                  , "None"                       , "Unchanged"                                                          }, // $59 (relative)
{ "JUMP,CNE",    " .+rel"      , "if (C != 0) goto rel;"                , "Jump if register C not equal to zero"             , "B"                                  , "None"                       , "Unchanged"                                                          }, // $5A (relative)
{ "JUMP,DNE",    " .+rel"      , "if (D != 0) goto rel;"                , "Jump if register D not equal to zero"             , "B"                                  , "None"                       , "Unchanged"                                                          }, // $5B (relative)
{ "JUMP,ANE",    " abs"        , "if (A != 0) goto abs;"                , "Jump if register A not equal to zero"             , "None"                               , "None"                       , "Unchanged"                                                          }, // $5C (absolute)
{ "JUMP,BNE",    " abs"        , "if (B != 0) goto abs;"                , "Jump if register B not equal to zero"             , "B"                                  , "None"                       , "Unchanged"                                                          }, // $5D (absolute)
{ "JUMP,CNE",    " abs"        , "if (C != 0) goto abs;"                , "Jump if register C not equal to zero"             , "B"                                  , "None"                       , "Unchanged"                                                          }, // $5E (absolute)
{ "JUMP,DNE",    " abs"        , "if (D != 0) goto abs;"                , "Jump if register D not equal to zero"             , "B"                                  , "None"                       , "Unchanged"                                                          }, // $5F (absolute)
{ "OR A,A",      ""            , "A |= A;"                              , "Logical OR A with A"                              , "None"                               , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $60
{ "OR A,B",      ""            , "A |= B;"                              , "Logical OR A with B"                              , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $61
{ "OR A,C",      ""            , "A |= C;"                              , "Logical OR A with C"                              , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $62
{ "OR A,D",      ""            , "A |= D;"                              , "Logical OR A with D"                              , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $63
{ "OR A",        ",#imm"       , "A |= imm;"                            , "Logical OR A with imm"                            , "None"                               , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $64 OR A,#<value>
{ "OR B",        ",#imm"       , "B |= imm;"                            , "Logical OR B with imm"                            , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $65 OR B,#<value>
{ "OR C",        ",#imm"       , "C |= imm;"                            , "Logical OR C with imm"                            , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $66 OR C,#<value>
{ "OR D",        ",#imm"       , "D |= imm;"                            , "Logical OR D with imm"                            , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $67 OR D,#<value>
{ "OR A",        ",.+rel"      , "A |= *(rel);"                         , "Logical OR A with *(rel)"                         , "None"                               , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $68 OR A,<relative>
{ "OR B",        ", +rel"      , "B |= *(rel);"                         , "Logical OR B with *(rel)"                         , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $69 OR B,<relative>
{ "OR C",        ",.+rel"      , "C |= *(rel);"                         , "Logical OR C with *(rel)"                         , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $6A OR C,<relative>
{ "OR D",        ",.+rel"      , "D |= *(rel);"                         , "Logical OR D with *(rel)"                         , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $6B OR D,<relative>
{ "OR A",        ",abs"        , "A |= *(abs);"                         , "Logical OR A with *(abs)"                         , "None"                               , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $6C OR A,<absolute>
{ "OR B",        ",abs"        , "B |= *(abs);"                         , "Logical OR B with *(abs)"                         , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $6D OR B,<absolute>
{ "OR C",        ",abs"        , "C |= *(abs);"                         , "Logical OR C with *(abs)"                         , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $6E OR C,<absolute>
{ "OR D",        ",abs"        , "D |= *(abs);"                         , "Logical OR D with *(abs)"                         , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $6F OR D,<absolute>
{ "LOAD A,$DATA",""            , "A = IOPORT(DATA);"                    , "Read specially decoded peripheral"                , "None"                               , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $70
{ "LOAD B,$DATA",""            , "B = IOPORT(DATA);"                    , "Read specially decoded peripheral"                , "B"                                  , "CC"                         , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $71
{ "LOAD C,$DATA",""            , "C = IOPORT(DATA);"                    , "Read specially decoded peripheral"                , "B"                                  , "CC"                         , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $72
{ "LOAD D,$DATA",""            , "D = IOPORT(DATA);"                    , "Read specially decoded peripheral"                , "B"                                  , "CC"                         , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $73
{ "BIC U",       ",#imm"       , "U &= ~(imm & %01100111);"             , "Bit clear"                                        , "None"                               , "OUTPUT, IOF, UF1, UF2, STACK","Unchanged"                                                          }, // $74 BIC U,#<value> (or CLR IOF, etc.)
{ "BIC L",       ",#imm"       , "L &= ~(imm);"                         , "Bit clear"                                        , "None"                               , "CC, H, B, W, V, LOGICOMP, C", "CC &= ~((imm & $C0) >> 6);"                                         }, // $75 BIC L,#<value> (or CLR CARRY, etc.)
{ "OR U",        ",#imm"       , "U |= imm & %01100111;"                , "Bit set"                                          , "None"                               , "OUTPUT, IOF, UF1, UF2, STACK","Unchanged"                                                          }, // $76 OR  U,#<value> (or SET IOF, etc.)
{ "OR L",        ",#imm"       , "L |= imm;"                            , "Bit set"                                          , "None"                               , "CC, H, B, W, V, LOGICOMP, C", "CC |= (imm & $C0) >> 6;"                                            }, // $77 OR  L,#<value> (or SET CARRY, etc.)
{ "CALL,ANE",    " .+rel"      , "if (A != 0) gosub rel;"               , "Call if register A not equal to zero"             , "STACK"                              , "STACK"                      , "Unchanged"                                                          }, // $78 (relative)
{ "CALL,BNE",    " .+rel"      , "if (B != 0) gosub rel;"               , "Call if register B not equal to zero"             , "STACK, B"                           , "STACK"                      , "Unchanged"                                                          }, // $79 (relative)
{ "CALL,CNE",    " .+rel"      , "if (C != 0) gosub rel;"               , "Call if register C not equal to zero"             , "STACK, B"                           , "STACK"                      , "Unchanged"                                                          }, // $7A (relative)
{ "CALL,DNE",    " .+rel"      , "if (D != 0) gosub rel;"               , "Call if register D not equal to zero"             , "STACK, B"                           , "STACK"                      , "Unchanged"                                                          }, // $7B (relative)
{ "CALL,ANE",    " abs"        , "if (A != 0) gosub abs;"               , "Call if register A not equal to zero"             , "STACK"                              , "STACK"                      , "Unchanged"                                                          }, // $7C (absolute)
{ "CALL,BNE",    " abs"        , "if (B != 0) gosub abs;"               , "Call if register B not equal to zero"             , "STACK, B"                           , "STACK"                      , "Unchanged"                                                          }, // $7D (absolute)
{ "CALL,CNE",    " abs"        , "if (C != 0) gosub abs;"               , "Call if register C not equal to zero"             , "STACK, B"                           , "STACK"                      , "Unchanged"                                                          }, // $7E (absolute)
{ "CALL,DNE",    " abs"        , "if (D != 0) gosub abs;"               , "Call if register D not equal to zero"             , "STACK, B"                           , "STACK"                      , "Unchanged"                                                          }, // $7F (absolute)
{ "ADD A,A",     ""            , "A *= 2;"                              , "Add A and A, result in A"                         , "W, C"                               , "CC, H, V, C"                , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $80 (all ADD could also be ADDC). Or "A += A;"
{ "ADD A,B",     ""            , "A += B;"                              , "Add A and B, result in A"                         , "B, W, C"                            , "CC, H, V, C"                , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $81
{ "ADD A,C",     ""            , "A += C;"                              , "Add A and C, result in A"                         , "B, W, C"                            , "CC, H, V, C"                , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $82
{ "ADD A,D",     ""            , "A += D;"                              , "Add A and D, result in A"                         , "B, W, C"                            , "CC, H, V, C"                , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $83
{ "ADD A",       ",#imm"       , "A += imm;"                            , "Add A and imm, result in A"                       , "W, C"                               , "CC, H, V, C"                , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $84 ADD A,#<value>
{ "ADD B",       ",#imm"       , "B += imm;"                            , "Add B and imm, result in B"                       , "B, W, C"                            , "CC, H, V, C"                , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $85 ADD B,#<value>
{ "ADD C",       ",#imm"       , "C += imm;"                            , "Add C and imm, result in C"                       , "B, W, C"                            , "CC, H, V, C"                , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $86 ADD C,#<value>
{ "ADD D",       ",#imm"       , "D += imm;"                            , "Add D and imm, result in D"                       , "B, W, C"                            , "CC, H, V, C"                , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $87 ADD D,#<value>
{ "ADD A",       ",.+rel"      , "A += *(rel);"                         , "Add A and *(rel), result in A"                    , "W, C"                               , "CC, H, V, C"                , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $88 ADD A,<relative>
{ "ADD B",       ",.+rel"      , "B += *(rel);"                         , "Add B and *(rel), result in B"                    , "B, W, C"                            , "CC, H, V, C"                , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $89 ADD B,<relative>
{ "ADD C",       ",.+rel"      , "C += *(rel);"                         , "Add C and *(rel), result in C"                    , "B, W, C"                            , "CC, H, V, C"                , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $8A ADD C,<relative>
{ "ADD D",       ",.+rel"      , "D += *(rel);"                         , "Add D and *(rel), result in D"                    , "B, W, C"                            , "CC, H, V, C"                , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $8B ADD D,<relative>
{ "ADD A",       ",abs"        , "A += *(abs);"                         , "Add A and *(abs), result in A"                    , "W, C"                               , "CC, H, V, C"                , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $8C ADD A,<absolute>
{ "ADD B",       ",abs"        , "B += *(abs);"                         , "Add B and *(abs), result in B"                    , "B, W, C"                            , "CC, H, V, C"                , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $8D ADD B,<absolute>
{ "ADD C",       ",abs"        , "C += *(abs);"                         , "Add C and *(abs), result in C"                    , "B, W, C"                            , "CC, H, V, C"                , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $8E ADD C,<absolute>
{ "ADD D",       ",abs"        , "D += *(abs);"                         , "Add D and *(abs), result in D"                    , "B, W, C"                            , "CC, H, V, C"                , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $8F ADD D,<absolute>
{ "--------",    ""            , "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $90
{ "--------",    ""            , "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $91
{ "LOAD U,A",    ""            , "U &= %10000000; U |= (A & %01100111);", "Load U with A"                                    , "None"                               , "OUTPUT, IOF, UF1, UF2, STACK","Unchanged"                                                          }, // $92
{ "LOAD L,A",    ""            , "L = A;"                               , "Load L with A"                                    , "None"                               , "CC, H, B, W, V, LOGICOMP, C", "CC = (imm & $C0) >> 6;"                                             }, // $93
{ "DA A",        ""            , "A = BCD(A);"                          , "Decimal adjust"                                   , "H, C"                               , "CC"                         , "Meaningless"                                                        }, // $94
{ "DA B",        ""            , "B = BCD(B);"                          , "Decimal adjust"                                   , "H, B, C"                            , "CC"                         , "Meaningless"                                                        }, // $95
{ "DA C",        ""            , "C = BCD(C);"                          , "Decimal adjust"                                   , "H, B, C"                            , "CC"                         , "Meaningless"                                                        }, // $96
{ "DA D",        ""            , "D = BCD(D);"                          , "Decimal adjust"                                   , "H, B, C"                            , "CC"                         , "Meaningless"                                                        }, // $97
{ "JUMP,NE",     " .+rel"      , "if != goto rel;"                      , "Jump if not equal"                                , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $98 (relative)
{ "JUMP,LE",     " .+rel"      , "if <= goto rel;"                      , "Jump if less than or equal"                       , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $99 (relative)
{ "JUMP,GE",     " .+rel"      , "if >= goto rel;"                      , "Jump if greater than or equal"                    , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $9A (relative)
{ "JUMP",        " 0+zero"     , "goto zero;"                           , "Jump always"                                      , "None"                               , "None"                       , "Unchanged"                                                          }, // $9B (zero-page)
{ "JUMP,NE",     " abs"        , "if != goto abs;"                      , "Jump if not equal"                                , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $9C (absolute)
{ "JUMP,LE",     " abs"        , "if <= goto abs;"                      , "Jump if less than or equal"                       , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $9D (absolute)
{ "JUMP,GE",     " abs"        , "if >= goto abs;"                      , "Jump if greater than or equal"                    , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $9E (absolute)
{ "JUMP (D)+",   "abs"         , "goto abs + D;"                        , "Jump always"                                      , "B"                                  , "None"                       , "Unchanged"                                                          }, // $9F (indexed)
{ "SUB A,A",     ""            , "A -= A;"                              , "Subtract A from A, result in A"                   , "W, C"                               , "CC, H, V, C"                , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $A0 (all SUB could also be SUBB)
{ "SUB A,B",     ""            , "A -= B;"                              , "Subtract B from A, result in A"                   , "B, W, C"                            , "CC, H, V, C"                , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $A1
{ "SUB A,C",     ""            , "A -= C;"                              , "Subtract C from A, result in A"                   , "B, W, C"                            , "CC, H, V, C"                , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $A2
{ "SUB A,D",     ""            , "A -= D;"                              , "Subtract D from A, result in A"                   , "B, W, C"                            , "CC, H, V, C"                , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $A3
{ "SUB A",       ",#imm"       , "A -= imm;"                            , "Subtract imm from A, result in A"                 , "W, C"                               , "CC, H, V, C"                , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $A4 SUB A,#<value>
{ "SUB B",       ",#imm"       , "B -= imm;"                            , "Subtract imm from B, result in B"                 , "B, W, C"                            , "CC, H, V, C"                , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $A5 SUB B,#<value>
{ "SUB C",       ",#imm"       , "C -= imm;"                            , "Subtract imm from C, result in C"                 , "B, W, C"                            , "CC, H, V, C"                , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $A6 SUB C,#<value>
{ "SUB D",       ",#imm"       , "D -= imm;"                            , "Subtract imm from D, result in D"                 , "B, W, C"                            , "CC, H, V, C"                , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $A7 SUB D,#<value>
{ "SUB A",       ",.+rel"      , "A -= *(rel);"                         , "Subtract *(rel) from A, result in A"              , "W, C"                               , "CC, H, V, C"                , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $A8 SUB A,<relative>
{ "SUB B",       ",.+rel"      , "B -= *(rel);"                         , "Subtract *(rel) from B, result in B"              , "B, W, C"                            , "CC, H, V, C"                , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $A9 SUB B,<relative>
{ "SUB C",       ",.+rel"      , "C -= *(rel);"                         , "Subtract *(rel) from C, result in C"              , "B, W, C"                            , "CC, H, V, C"                , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $AA SUB C,<relative>
{ "SUB D",       ",.+rel"      , "D -= *(rel);"                         , "Subtract *(rel) from D, result in D"              , "B, W, C"                            , "CC, H, V, C"                , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $AB SUB D,<relative>
{ "SUB A",       ",abs"        , "A -= *(abs);"                         , "Subtract *(abs) from A, result in A"              , "W, C"                               , "CC, H, V, C"                , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $AC SUB A,<absolute>
{ "SUB B",       ",abs"        , "B -= *(abs);"                         , "Subtract *(abs) from B, result in B"              , "B, W, C"                            , "CC, H, V, C"                , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $AD SUB B,<absolute>
{ "SUB C",       ",abs"        , "C -= *(abs);"                         , "Subtract *(abs) from C, result in C"              , "B, W, C"                            , "CC, H, V, C"                , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $AE SUB C,<absolute>
{ "SUB D",       ",abs"        , "D -= *(abs);"                         , "Subtract *(abs) from D, result in D"              , "B, W, C"                            , "CC, H, V, C"                , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $AF SUB D,<absolute>
{ "LOAD $CTRL,A",""            , "IOPORT(CTRL) = A;"                    , "Write to specially decoded peripheral"            , "None"                               , "None"                       , "Unchanged"                                                          }, // $B0
{ "LOAD $CTRL,B",""            , "IOPORT(CTRL) = B;"                    , "Write to specially decoded peripheral"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $B1
{ "LOAD $CTRL,C",""            , "IOPORT(CTRL) = C;"                    , "Write to specially decoded peripheral"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $B2
{ "LOAD $CTRL,D",""            , "IOPORT(CTRL) = D;"                    , "Write to specially decoded peripheral"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $B3
{ "TEST U",      ",#imm"       , "compare (U & imm) against imm;"       , "Test bits"                                        , "INPUT, OUTPUT, IOF, UF1, UF2, STACK", "CC"                         , "CC = (PSU & imm == imm) ? EQ : LT;"                                 }, // $B4 TEST U,#<value> (or TEST IOF, etc.)
{ "TEST L",      ",#imm"       , "compare (L & imm) against imm;"       , "Test bits"                                        , "CC, H, B, W, V, LOGICOMP, C"        , "CC"                         , "CC = (PSL & imm == imm) ? EQ : LT;"                                 }, // $B5 TEST L,#<value> (or TEST CARRY, etc.)
{ "--------",    ""            , "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $B6
{ "--------",    ""            , "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $B7
{ "CALL,NE",     " .+rel"      , "if != gosub rel;"                     , "Call if not equal"                                , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $B8 (relative)
{ "CALL,LE",     " .+rel"      , "if <= gosub rel;"                     , "Call if less than or equal"                       , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $B9 (relative)
{ "CALL,GE",     " .+rel"      , "if >= gosub rel;"                     , "Call if greater than or equal"                    , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $BA (relative)
{ "CALL",        " 0+zero"     , "gosub zero;"                          , "Call always"                                      , "STACK"                              , "STACK"                      , "Unchanged"                                                          }, // $BB (zero-page)
{ "CALL,NE",     " abs"        , "if != gosub abs;"                     , "Call if not equal"                                , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $BC (absolute)
{ "CALL,LE",     " abs"        , "if <= gosub abs;"                     , "Call if less than or equal"                       , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $BD (absolute)
{ "CALL,GE",     " abs"        , "if >= gosub abs;"                     , "Call if greater than or equal"                    , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $BE (absolute)
{ "CALL (D)+",   "abs"         , "gosub abs + D;"                       , "Call always"                                      , "STACK, B"                           , "STACK"                      , "Unchanged"                                                          }, // $BF (indexed)
{ "NOP",         ""            , ";"                                    , "No operation"                                     , "None"                               , "None"                       , "Unchanged"                                                          }, // $C0
{ "LOAD B,A",    ""            , "B = A;"                               , "Load B with A"                                    , "B"                                  , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $C1
{ "LOAD C,A",    ""            , "C = A;"                               , "Load C with A"                                    , "B"                                  , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $C2
{ "LOAD D,A",    ""            , "D = A;"                               , "Load D with A"                                    , "B"                                  , "CC"                         , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $C3
{ "--------",    ""            , "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $C4
{ "--------",    ""            , "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $C5
{ "--------",    ""            , "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $C6
{ "--------",    ""            , "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $C7
{ "LOAD A",      "LOAD .+rel,A", "*(rel) = A;"                          , "Load *(rel) with A"                               , "None"                               , "None"                       , "Unchanged"                                                          }, // $C8 LOAD <relative>,A
{ "LOAD B",      "LOAD .+rel,B", "*(rel) = B;"                          , "Load *(rel) with B"                               , "B"                                  , "None"                       , "Unchanged"                                                          }, // $C9 LOAD <relative>,B
{ "LOAD C",      "LOAD .+rel,C", "*(rel) = C;"                          , "Load *(rel) with C"                               , "B"                                  , "None"                       , "Unchanged"                                                          }, // $CA LOAD <relative>,C
{ "LOAD D",      "LOAD .+rel,D", "*(rel) = D;"                          , "Load *(rel) with D"                               , "B"                                  , "None"                       , "Unchanged"                                                          }, // $CB LOAD <relative>,D
{ "LOAD A",      "LOAD abs,A"  , "*(abs) = A;"                          , "Load *(abs) with A"                               , "None"                               , "None"                       , "Unchanged"                                                          }, // $CC LOAD <absolute>,A
{ "LOAD B",      "LOAD abs,B"  , "*(abs) = B;"                          , "Load *(abs) with B"                               , "B"                                  , "None"                       , "Unchanged"                                                          }, // $CD LOAD <absolute>,B
{ "LOAD C",      "LOAD abs,C"  , "*(abs) = C;"                          , "Load *(abs) with C"                               , "B"                                  , "None"                       , "Unchanged"                                                          }, // $CE LOAD <absolute>,C
{ "LOAD D",      "LOAD abs,D"  , "*(abs) = D;"                          , "Load *(abs) with D"                               , "B"                                  , "None"                       , "Unchanged"                                                          }, // $CF LOAD <absolute>,D
{ "RL A",        ""            , "A <<= 1;"                             , "Rotate left"                                      , "W, C"                               , "H, V, C"                    , "if (A & $80) CC = LT; elif (A == 0) CC = EQ; else CC = GT;"         }, // $D0 or RLC
{ "RL B",        ""            , "B <<= 1;"                             , "Rotate left"                                      , "B, W, C"                            , "H, V, C"                    , "if (B & $80) CC = LT; elif (B == 0) CC = EQ; else CC = GT;"         }, // $D1 or RLC
{ "RL C",        ""            , "C <<= 1;"                             , "Rotate left"                                      , "B, W, C"                            , "H, V, C"                    , "if (C & $80) CC = LT; elif (C == 0) CC = EQ; else CC = GT;"         }, // $D2 or RLC
{ "RL D",        ""            , "D <<= 1;"                             , "Rotate left"                                      , "B, W, C"                            , "H, V, C"                    , "if (D & $80) CC = LT; elif (D == 0) CC = EQ; else CC = GT;"         }, // $D3 or RLC
{ "LOAD A",      "LOAD $port,A", "IOPORT(port) = A;"                    , "Write peripheral port data from A"                , "None"                               , "None"                       , "Unchanged"                                                          }, // $D4 LOAD $<port>,A
{ "LOAD B",      "LOAD $port,B", "IOPORT(port) = B;"                    , "Write peripheral port data from B"                , "B"                                  , "None"                       , "Unchanged"                                                          }, // $D5 LOAD $<port>,B
{ "LOAD C",      "LOAD $port,C", "IOPORT(port) = C;"                    , "Write peripheral port data from C"                , "B"                                  , "None"                       , "Unchanged"                                                          }, // $D6 LOAD $<port>,C
{ "LOAD D",      "LOAD $port,D", "IOPORT(port) = D;"                    , "Write peripheral port data from D"                , "B"                                  , "None"                       , "Unchanged"                                                          }, // $D7 LOAD $<port>,D
{ "INCJ,NE A",   ",.+rel"      , "if (++A != 0) goto rel;"              , "Increment A and jump if not equal to zero"        , "None"                               , "None"                       , "Unchanged"                                                          }, // $D8 (relative) (or INC A)
{ "INCJ,NE B",   ",.+rel"      , "if (++B != 0) goto rel;"              , "Increment B and jump if not equal to zero"        , "B"                                  , "None"                       , "Unchanged"                                                          }, // $D9 (relative) (or INC B)
{ "INCJ,NE C",   ",.+rel"      , "if (++C != 0) goto rel;"              , "Increment C and jump if not equal to zero"        , "B"                                  , "None"                       , "Unchanged"                                                          }, // $DA (relative) (or INC C)
{ "INCJ,NE D",   ",.+rel"      , "if (++D != 0) goto rel;"              , "Increment D and jump if not equal to zero"        , "B"                                  , "None"                       , "Unchanged"                                                          }, // $DB (relative) (or INC D)
{ "INCJ,NE A",   ",abs"        , "if (++A != 0) goto abs;"              , "Increment A and jump if not equal to zero"        , "None"                               , "None"                       , "Unchanged"                                                          }, // $DC (absolute) (or INC A)
{ "INCJ,NE B",   ",abs"        , "if (++B != 0) goto abs;"              , "Increment B and jump if not equal to zero"        , "B"                                  , "None"                       , "Unchanged"                                                          }, // $DD (absolute) (or INC B)
{ "INCJ,NE C",   ",abs"        , "if (++C != 0) goto abs;"              , "Increment C and jump if not equal to zero"        , "B"                                  , "None"                       , "Unchanged"                                                          }, // $DE (absolute) (or INC C)
{ "INCJ,NE D",   ",abs"        , "if (++D != 0) goto abs;"              , "Increment D and jump if not equal to zero"        , "B"                                  , "None"                       , "Unchanged"                                                          }, // $DF (absolute) (or INC D)
{ "COMP A,A",    ""            , "CC = EQ;"                             , "Compare A against A"                              , "LOGICOMP"                           , "CC"                         , "CC = EQ;"                                                           }, // $E0
{ "COMP A,B",    ""            , "compare A against B;"                 , "Compare A against B"                              , "B, LOGICOMP"                        , "CC"                         , "if (A > B) CC = GT; elif (A < B) CC = LT; else CC = EQ;"            }, // $E1
{ "COMP A,C",    ""            , "compare A against C;"                 , "Compare A against C"                              , "B, LOGICOMP"                        , "CC"                         , "if (A > C) CC = GT; elif (A < C) CC = LT; else CC = EQ;"            }, // $E2
{ "COMP A,D",    ""            , "compare A against D;"                 , "Compare A against D"                              , "B, LOGICOMP"                        , "CC"                         , "if (A > D) CC = GT; elif (A < D) CC = LT; else CC = EQ;"            }, // $E3
{ "COMP A",      ",#imm"       , "compare A against imm;"               , "Compare A against imm"                            , "LOGICOMP"                           , "CC"                         , "if (A > imm) CC = GT; elif (A < imm) CC = LT; else CC = EQ;"        }, // $E4 COMP A,#<value>
{ "COMP B",      ",#imm"       , "compare B against imm;"               , "Compare B against imm"                            , "B, LOGICOMP"                        , "CC"                         , "if (B > imm) CC = GT; elif (B < imm) CC = LT; else CC = EQ;"        }, // $E5 COMP B,#<value>
{ "COMP C",      ",#imm"       , "compare C against imm;"               , "Compare C against imm"                            , "B, LOGICOMP"                        , "CC"                         , "if (C > imm) CC = GT; elif (C < imm) CC = LT; else CC = EQ;"        }, // $E6 COMP C,#<value>
{ "COMP D",      ",#imm"       , "compare D against imm;"               , "Compare D against imm"                            , "B, LOGICOMP"                        , "CC"                         , "if (D > imm) CC = GT; elif (D < imm) CC = LT; else CC = EQ;"        }, // $E7 COMP D,#<value>
{ "COMP A",      ",.+rel"      , "compare A against *(rel);"            , "Compare A against *(rel)"                         , "LOGICOMP"                           , "CC"                         , "if (A > *(rel)) CC = GT; elif (A < *(rel)) CC = LT; else CC = EQ;"  }, // $E8 COMP A,<relative>
{ "COMP B",      ",.+rel"      , "compare B against *(rel);"            , "Compare B against *(rel)"                         , "B, LOGICOMP"                        , "CC"                         , "if (B > *(rel)) CC = GT; elif (B < *(rel)) CC = LT; else CC = EQ;"  }, // $E9 COMP B,<relative>
{ "COMP C",      ",.+rel"      , "compare C against *(rel);"            , "Compare C against *(rel)"                         , "B, LOGICOMP"                        , "CC"                         , "if (C > *(rel)) CC = GT; elif (C < *(rel)) CC = LT; else CC = EQ;"  }, // $EA COMP C,<relative>
{ "COMP D",      ",.+rel"      , "compare D against *(rel);"            , "Compare D against *(rel)"                         , "B, LOGICOMP"                        , "CC"                         , "if (D > *(rel)) CC = GT; elif (D < *(rel)) CC = LT; else CC = EQ;"  }, // $EB COMP D,<relative>
{ "COMP A",      ",abs"        , "compare A against *(abs);"            , "Compare A against *(abs)"                         , "LOGICOMP"                           , "CC"                         , "if (A > *(abs)) CC = GT; elif (A < *(abs)) CC = LT; else CC = EQ;"  }, // $EC COMP A,<absolute>
{ "COMP B",      ",abs"        , "compare B against *(abs);"            , "Compare B against *(abs)"                         , "B, LOGICOMP"                        , "CC"                         , "if (B > *(abs)) CC = GT; elif (B < *(abs)) CC = LT; else CC = EQ;"  }, // $ED COMP B,<absolute>
{ "COMP C",      ",abs"        , "compare C against *(abs);"            , "Compare C against *(abs)"                         , "B, LOGICOMP"                        , "CC"                         , "if (C > *(abs)) CC = GT; elif (C < *(abs)) CC = LT; else CC = EQ;"  }, // $EE COMP C,<absolute>
{ "COMP D",      ",abs"        , "compare D against *(abs);"            , "Compare D against *(abs)"                         , "B, LOGICOMP"                        , "CC"                         , "if (D > *(abs)) CC = GT; elif (D < *(abs)) CC = LT; else CC = EQ;"  }, // $EF COMP D,<absolute>
{ "LOAD $DATA,A",""            , "IOPORT(DATA) = A;"                    , "Write to specially decoded peripheral"            , "None"                               , "None"                       , "Unchanged"                                                          }, // $F0
{ "LOAD $DATA,B",""            , "IOPORT(DATA) = B;"                    , "Write to specially decoded peripheral"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $F1
{ "LOAD $DATA,C",""            , "IOPORT(DATA) = C;"                    , "Write to specially decoded peripheral"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $F2
{ "LOAD $DATA,D",""            , "IOPORT(DATA) = D;"                    , "Write to specially decoded peripheral"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $F3
{ "TEST A",      ",#imm"       , "CC = (A & imm == imm) ? EQ : LT;"     , "Test bits imm of A"                               , "None"                               , "CC"                         , "CC = (A & imm == imm) ? EQ : LT;"                                   }, // $F4 TEST A,#<value>
{ "TEST B",      ",#imm"       , "CC = (B & imm == imm) ? EQ : LT;"     , "Test bits imm of B"                               , "B"                                  , "CC"                         , "CC = (B & imm == imm) ? EQ : LT;"                                   }, // $F5 TEST B,#<value>
{ "TEST C",      ",#imm"       , "CC = (C & imm == imm) ? EQ : LT;"     , "Test bits imm of C"                               , "B"                                  , "CC"                         , "CC = (C & imm == imm) ? EQ : LT;"                                   }, // $F6 TEST C,#<value>
{ "TEST D",      ",#imm"       , "CC = (D & imm == imm) ? EQ : LT;"     , "Test bits imm of D"                               , "B"                                  , "CC"                         , "CC = (D & imm == imm) ? EQ : LT;"                                   }, // $F7 TEST D,#<value>
{ "DECJ,NE A",   ",.+rel"      , "if (--A != 0) goto rel;"              , "Decrement A and jump if not equal to zero"        , "None"                               , "None"                       , "Unchanged"                                                          }, // $F8 (relative) (or DEC A)
{ "DECJ,NE B",   ",.+rel"      , "if (--B != 0) goto rel;"              , "Decrement B and jump if not equal to zero"        , "B"                                  , "None"                       , "Unchanged"                                                          }, // $F9 (relative) (or DEC B)
{ "DECJ,NE C",   ",.+rel"      , "if (--C != 0) goto rel;"              , "Decrement C and jump if not equal to zero"        , "B"                                  , "None"                       , "Unchanged"                                                          }, // $FA (relative) (or DEC C)
{ "DECJ,NE D",   ",.+rel"      , "if (--D != 0) goto rel;"              , "Decrement D and jump if not equal to zero"        , "B"                                  , "None"                       , "Unchanged"                                                          }, // $FB (relative) (or DEC D)
{ "DECJ,NE A",   ",abs"        , "if (--A != 0) goto abs;"              , "Decrement A and jump if not equal to zero"        , "None"                               , "None"                       , "Unchanged"                                                          }, // $FC (absolute) (or DEC A)
{ "DECJ,NE B",   ",abs"        , "if (--B != 0) goto abs;"              , "Decrement B and jump if not equal to zero"        , "B"                                  , "None"                       , "Unchanged"                                                          }, // $FD (absolute) (or DEC B)
{ "DECJ,NE C",   ",abs"        , "if (--C != 0) goto abs;"              , "Decrement C and jump if not equal to zero"        , "B"                                  , "None"                       , "Unchanged"                                                          }, // $FE (absolute) (or DEC C)
{ "DECJ,NE D",   ",abs"        , "if (--D != 0) goto abs;"              , "Decrement D and jump if not equal to zero"        , "B"                                  , "None"                       , "Unchanged"                                                          }, // $FF (absolute) (or DEC D)
}, {
// New CALM format
{ "MOVE r0,r0"     , "", ";"                                    , "Move R0 into R0"                                  , "None"                               , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $00
{ "MOVE r1,r0"     , "", "r0 = r1;"                             , "Move R1 into R0"                                  , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $01
{ "MOVE r2,r0"     , "", "r0 = r2;"                             , "Move R2 into R0"                                  , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $02
{ "MOVE r3,r0"     , "", "r0 = r3;"                             , "Move R3 into R0"                                  , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $03
{ "MOVE #imm,r0"   , "", "r0 = imm;"                            , "Move imm into R0"                                 , "None"                               , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $04
{ "MOVE #imm,r1"   , "", "r1 = imm;"                            , "Move imm into R1"                                 , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $05
{ "MOVE #imm,r2"   , "", "r2 = imm;"                            , "Move imm into R2"                                 , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $06
{ "MOVE #imm,r3"   , "", "r3 = imm;"                            , "Move imm into R3"                                 , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $07
{ "MOVE R^rel,r0"  , "", "r0 = *(rel);"                         , "Move *rel into R0"                                , "None"                               , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $08
{ "MOVE R^rel,r1"  , "", "r1 = *(rel);"                         , "Move *rel into R1"                                , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $09
{ "MOVE R^rel,r2"  , "", "r2 = *(rel);"                         , "Move *rel into R2"                                , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $0A
{ "MOVE R^rel,r3"  , "", "r3 = *(rel);"                         , "Move *rel into R3"                                , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $0B
{ "MOVE U^abs,r0"  , "", "r0 = *(abs);"                         , "Move *abs into R0"                                , "None"                               , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $0C
{ "MOVE U^abs,r1"  , "", "r1 = *(abs);"                         , "Move *abs into R1"                                , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $0D
{ "MOVE U^abs,r2"  , "", "r2 = *(abs);"                         , "Move *abs into R2"                                , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $0E
{ "MOVE U^abs,r3"  , "", "r3 = *(abs);"                         , "Move *abs into R3"                                , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $0F
{ "MOVE U^abs,PSL" , "", "PSL = *(abs);"                        , "Move *abs into PSL"                               , "None"                               , "CC, H, B, W, V, LOGICOMP, C", "CC = (*(abs) & $C0) >> 6;"                                          }, // $10 2560-B only
{ "MOVE PSL,U^abs" , "", "*(abs) = PSL;"                        , "Move PSL into *abs"                               , "CC, H, B, W, V, LOGICOMP, C"        , "None"                       , "Undefined"                                                          }, // $11 2560-B only
{ "MOVE PSU,r0"    , "", "r0 = PSU;"                            , "Move PSU into R0"                                 , "INPUT, OUTPUT, IOF, UF1, UF2, SP"   , "CC"                         , "if (INPUT) CC = LT; elif (PSU == 0) CC = EQ; else CC = GT;"         }, // $12
{ "MOVE PSL,r0"    , "", "r0 = PSL;"                            , "Move PSL into R0"                                 , "CC, H, B, W, V, LOGICOMP, C"        , "CC"                         , "if (CC == LT || CC == UN) CC = LT; elif (PSL == 0) CC = EQ; else CC = GT;" }, // $13
{ "RET,EQ"         , "", "if == return;"                        , "Return if EQ true"                                , "SP, CC"                             , "SP"                         , "Unchanged"                                                          }, // $14
{ "RET,GT"         , "", "if > return;"                         , "Return if GT true"                                , "SP, CC"                             , "SP"                         , "Unchanged"                                                          }, // $15
{ "RET,LT"         , "", "if < return;"                         , "Return if LT true"                                , "SP, CC"                             , "SP"                         , "Unchanged"                                                          }, // $16
{ "RET"            , "", "return;"                              , "Return"                                           , "SP"                                 , "SP"                         , "Unchanged"                                                          }, // $17
{ "JUMP,EQ R^rel"  , "", "if == goto rel;"                      , "Jump if EQ true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $18
{ "JUMP,GT R^rel"  , "", "if > goto rel;"                       , "Jump if GT true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $19
{ "JUMP,LT R^rel"  , "", "if < goto rel;"                       , "Jump if LT true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $1A
{ "JUMP R^rel"     , "", "goto rel;"                            , "Jump"                                             , "None"                               , "None"                       , "Unchanged"                                                          }, // $1B
{ "JUMP,EQ U^abs"  , "", "if == goto abs;"                      , "Jump if EQ true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $1C
{ "JUMP,GT U^abs"  , "", "if > goto abs;"                       , "Jump if GT true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $1D
{ "JUMP,LT U^abs"  , "", "if < goto abs;"                       , "Jump if LT true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $1E
{ "JUMP U^abs"     , "", "goto abs;"                            , "Jump"                                             , "None"                               , "None"                       , "Unchanged"                                                          }, // $1F
{ "CLR r0"         , "", "r0 = 0;"                              , "Clear R0"                                         , "None"                               , "CC"                         , "CC = EQ;"                                                           }, // $20
{ "XOR r1,r0"      , "", "r0 ^= r1;"                            , "Exclusive OR R1 with R0"                          , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $21
{ "XOR r2,r0"      , "", "r0 ^= r2;"                            , "Exclusive OR R2 with R0"                          , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $22
{ "XOR r3,r0"      , "", "r0 ^= r3;"                            , "Exclusive OR R3 with R0"                          , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $23
{ "XOR #imm,r0"    , "", "r0 ^= imm;"                           , "Exclusive OR imm with R0"                         , "None"                               , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $24
{ "XOR #imm,r1"    , "", "r1 ^= imm;"                           , "Exclusive OR imm with R0"                         , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $25
{ "XOR #imm,r2"    , "", "r2 ^= imm;"                           , "Exclusive OR imm with R0"                         , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $26
{ "XOR #imm,r3"    , "", "r3 ^= imm;"                           , "Exclusive OR imm with R0"                         , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $27
{ "XOR R^rel,r0"   , "", "r0 ^= *(rel);"                        , "Exclusive OR r0 with *rel"                        , "None"                               , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $28
{ "XOR R^rel,r1"   , "", "r1 ^= *(rel);"                        , "Exclusive OR r1 with *rel"                        , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $29
{ "XOR R^rel,r2"   , "", "r2 ^= *(rel);"                        , "Exclusive OR r2 with *rel"                        , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $2A
{ "XOR R^rel,r3"   , "", "r3 ^= *(rel);"                        , "Exclusive OR r3 with *rel"                        , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $2B
{ "XOR r0^abs,r0"  , "", "r0 ^= *(abs);"                        , "Exclusive OR r0 with *abs"                        , "None"                               , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $2C
{ "XOR r1^abs,r1"  , "", "r1 ^= *(abs);"                        , "Exclusive OR r1 with *abs"                        , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $2D
{ "XOR r2^abs,r2"  , "", "r2 ^= *(abs);"                        , "Exclusive OR r2 with *abs"                        , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $2E
{ "XOR r3^abs,r3"  , "", "r3 ^= *(abs);"                        , "Exclusive OR r3 with *abs"                        , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $2F
{ "MOVE $CTRL,r0"  , "", "r0 = IOPORT(CTRL);"                   , "Move from control port into R0"                   , "None"                               , "CC"                         , "if (IOPORT(CTRL) & $80) CC = LT; elif (IOPORT(CTRL) == 0) CC = EQ; else CC = GT;" }, // $30
{ "MOVE $CTRL,r1"  , "", "r1 = IOPORT(CTRL);"                   , "Move from control port into R1"                   , "B"                                  , "CC"                         , "if (IOPORT(CTRL) & $80) CC = LT; elif (IOPORT(CTRL) == 0) CC = EQ; else CC = GT;" }, // $31
{ "MOVE $CTRL,r2"  , "", "r2 = IOPORT(CTRL);"                   , "Move from control port into R2"                   , "B"                                  , "CC"                         , "if (IOPORT(CTRL) & $80) CC = LT; elif (IOPORT(CTRL) == 0) CC = EQ; else CC = GT;" }, // $32
{ "MOVE $CTRL,r3"  , "", "r3 = IOPORT(CTRL);"                   , "Move from control port into R3"                   , "B"                                  , "CC"                         , "if (IOPORT(CTRL) & $80) CC = LT; elif (IOPORT(CTRL) == 0) CC = EQ; else CC = GT;" }, // $33
{ "RETI,EQ"        , "", "if == { PSU &= ~PSU_IOF; return; }"   , "Return and clear interrupt mask if EQ true"       , "SP, CC"                             , "IOF, SP"                    , "Unchanged"                                                          }, // $34
{ "RETI,GT"        , "", "if > { PSU &= ~PSU_IOF; return; }"    , "Return and clear interrupt mask if GT true"       , "SP, CC"                             , "IOF, SP"                    , "Unchanged"                                                          }, // $35
{ "RETI,LT"        , "", "if < { PSU &= ~PSU_IOF; return; }"    , "Return and clear interrupt mask if LT true"       , "SP, CC"                             , "IOF, SP"                    , "Unchanged"                                                          }, // $36
{ "RETI"           , "", "PSU &= ~PSU_IOF; return;"             , "Return and clear interrupt mask"                  , "SP"                                 , "IOF, SP"                    , "Unchanged"                                                          }, // $37
{ "CALL,EQ R^rel"  , "", "if == gosub rel;"                     , "Call if EQ true"                                  , "SP, CC"                             , "SP"                         , "Unchanged"                                                          }, // $38
{ "CALL,GT R^rel"  , "", "if > gosub rel;"                      , "Call if GT true"                                  , "SP, CC"                             , "SP"                         , "Unchanged"                                                          }, // $39
{ "CALL,LT R^rel"  , "", "if < gosub rel;"                      , "Call if LT true"                                  , "SP, CC"                             , "SP"                         , "Unchanged"                                                          }, // $3A
{ "CALL R^rel"     , "", "gosub rel;"                           , "Call"                                             , "SP"                                 , "SP"                         , "Unchanged"                                                          }, // $3B
{ "CALL,EQ U^abs"  , "", "if == gosub abs;"                     , "Call if EQ true"                                  , "SP, CC"                             , "SP"                         , "Unchanged"                                                          }, // $3C
{ "CALL,GT U^abs"  , "", "if > gosub abs;"                      , "Call if GT true"                                  , "SP, CC"                             , "SP"                         , "Unchanged"                                                          }, // $3D
{ "CALL,LT U^abs"  , "", "if < gosub abs;"                      , "Call if LT true"                                  , "SP, CC"                             , "SP"                         , "Unchanged"                                                          }, // $3E
{ "CALL U^abs"     , "", "gosub abs;"                           , "Call"                                             , "SP"                                 , "SP"                         , "Unchanged"                                                          }, // $3F
{ "HALT"           , "", "for (;;);"                            , "Halt"                                             , "None"                               , "None"                       , "Unchanged"                                                          }, // $40 (could also be WAIT)
{ "AND r1,r0"      , "", "r0 &= r1;"                            , "Logical AND r1 with r0"                           , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $41
{ "AND r2,r0"      , "", "r0 &= r2;"                            , "Logical AND r2 with r0"                           , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $42
{ "AND r3,r0"      , "", "r0 &= r3;"                            , "Logical AND r3 with r0"                           , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $43
{ "AND #imm,r0"    , "", "r0 &= imm;"                           , "Logical AND imm with r0"                          , "None"                               , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $44
{ "AND #imm,r1"    , "", "r1 &= imm;"                           , "Logical AND imm with r1"                          , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $45
{ "AND #imm,r2"    , "", "r2 &= imm;"                           , "Logical AND imm with r2"                          , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $46
{ "AND #imm,r3"    , "", "r3 &= imm;"                           , "Logical AND imm with r3"                          , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $47
{ "AND R^rel,r0"   , "", "r0 &= *(rel);"                        , "Logical AND *rel with r0"                         , "None"                               , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $48
{ "AND R^rel,r1"   , "", "r1 &= *(rel);"                        , "Logical AND *rel with r1"                         , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $49
{ "AND R^rel,r2"   , "", "r2 &= *(rel);"                        , "Logical AND *rel with r2"                         , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $4A
{ "AND R^rel,r3"   , "", "r3 &= *(rel);"                        , "Logical AND *rel with r3"                         , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $4B
{ "AND U^abs,r0"   , "", "r0 &= *(abs);"                        , "Logical AND *abs with r0"                         , "None"                               , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $4C
{ "AND U^abs,r1"   , "", "r1 &= *(abs);"                        , "Logical AND *abs with r1"                         , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $4D
{ "AND U^abs,r2"   , "", "r2 &= *(abs);"                        , "Logical AND *abs with r2"                         , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $4E
{ "AND U^abs,r3"   , "", "r3 &= *(abs);"                        , "Logical AND *abs with r3"                         , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $4F
{ "RR r0"          , "", "r0 >>= 1;"                            , "Rotate right"                                     , "W, C"                               , "CC, H, V, C"                , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $50 or RRC
{ "RR r1"          , "", "r1 >>= 1;"                            , "Rotate right"                                     , "B, W, C"                            , "CC, H, V, C"                , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $51 or RRC
{ "RR r2"          , "", "r2 >>= 1;"                            , "Rotate right"                                     , "B, W, C"                            , "CC, H, V, C"                , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $52 or RRC
{ "RR r3"          , "", "r3 >>= 1;"                            , "Rotate right"                                     , "B, W, C"                            , "CC, H, V, C"                , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $53 or RRC
{ "MOVE $port,r0"  , "", "r0 = IOPORT(port);"                   , "Move from extended I/O port into r0"              , "None"                               , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $54
{ "MOVE $port,r1"  , "", "r1 = IOPORT(port);"                   , "Move from extended I/O port into r1"              , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $55
{ "MOVE $port,r2"  , "", "r2 = IOPORT(port);"                   , "Move from extended I/O port into r2"              , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $56
{ "MOVE $port,r3"  , "", "r3 = IOPORT(port);"                   , "Move from extended I/O port into r3"              , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $57
{ "JUMP,NE r0,R^rel","", "if (r0 != 0) goto rel;"               , "Jump if register r0 not equal to zero"            , "None"                               , "None"                       , "Unchanged"                                                          }, // $58
{ "JUMP,NE r0,R^rel","", "if (r1 != 0) goto rel;"               , "Jump if register r1 not equal to zero"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $59
{ "JUMP,NE r0,R^rel","", "if (r2 != 0) goto rel;"               , "Jump if register r2 not equal to zero"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $5A
{ "JUMP,NE r0,R^rel","", "if (r3 != 0) goto rel;"               , "Jump if register r3 not equal to zero"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $5B
{ "JUMP,NE r0,U^abs","", "if (r0 != 0) goto abs;"               , "Jump if register r0 not equal to zero"            , "None"                               , "None"                       , "Unchanged"                                                          }, // $5C
{ "JUMP,NE r0,U^abs","", "if (r1 != 0) goto abs;"               , "Jump if register r1 not equal to zero"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $5D
{ "JUMP,NE r0,U^abs","", "if (r2 != 0) goto abs;"               , "Jump if register r2 not equal to zero"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $5E
{ "JUMP,NE r0,U^abs","", "if (r3 != 0) goto abs;"               , "Jump if register r3 not equal to zero"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $5F
{ "OR r0,r0"       , "", "r0 |= r0;"                            , "Logical OR r0 with r0"                            , "None"                               , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $60
{ "OR r1,r0"       , "", "r0 |= r1;"                            , "Logical OR r1 with r0"                            , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $61
{ "OR r2,r0"       , "", "r0 |= r2;"                            , "Logical OR r2 with r0"                            , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $62
{ "OR r3,r0"       , "", "r0 |= r3;"                            , "Logical OR r3 with r0"                            , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $63
{ "OR #imm,r0"     , "", "r0 |= imm;"                           , "Logical OR imm with r0"                           , "None"                               , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $64
{ "OR #imm,r1"     , "", "r1 |= imm;"                           , "Logical OR imm with r1"                           , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $65
{ "OR #imm,r2"     , "", "r2 |= imm;"                           , "Logical OR imm with r2"                           , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $66
{ "OR #imm,r3"     , "", "r3 |= imm;"                           , "Logical OR imm with r3"                           , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $67
{ "OR R^rel,r0"    , "", "r0 |= *(rel);"                        , "Logical OR *rel with r0"                          , "None"                               , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $68
{ "OR R^rel,r1"    , "", "r1 |= *(rel);"                        , "Logical OR *rel with r1"                          , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $69
{ "OR R^rel,r2"    , "", "r2 |= *(rel);"                        , "Logical OR *rel with r2"                          , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $6A
{ "OR R^rel,r3"    , "", "r3 |= *(rel);"                        , "Logical OR *rel with r3"                          , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $6B
{ "OR U^abs,r0"    , "", "r0 |= *(abs);"                        , "Logical OR *abs with r0"                          , "None"                               , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $6C
{ "OR U^abs,r1"    , "", "r1 |= *(abs);"                        , "Logical OR *abs with r1"                          , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $6D
{ "OR U^abs,r2"    , "", "r2 |= *(abs);"                        , "Logical OR *abs with r2"                          , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $6E
{ "OR U^abs,r3"    , "", "r3 |= *(abs);"                        , "Logical OR *abs with r3"                          , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $6F
{ "MOVE $DATA,r0"  , "", "r0 = IOPORT(DATA);"                   , "Move from data port into r0"                      , "None"                               , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $70
{ "MOVE $DATA,r1"  , "", "r1 = IOPORT(DATA);"                   , "Move from data port into r1"                      , "B"                                  , "CC"                         , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $71
{ "MOVE $DATA,r2"  , "", "r2 = IOPORT(DATA);"                   , "Move from data port into r2"                      , "B"                                  , "CC"                         , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $72
{ "MOVE $DATA,r3"  , "", "r3 = IOPORT(DATA);"                   , "Move from data port into r3"                      , "B"                                  , "CC"                         , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $73
{ "BIC #imm,PSU"   , "", "PSU &= ~(imm & %01100111);"           , "Bit clear"                                        , "None"                               , "OUTPUT, IOF, UF1, UF2, SP"  , "Unchanged"                                                          }, // $74 or CLR IOF, etc.
{ "BIC #imm,PSL"   , "", "PSL &= ~(imm);"                       , "Bit clear"                                        , "None"                               , "CC, H, B, W, V, LOGICOMP, C", "CC &= ~((imm & $C0) >> 6);"                                         }, // $75 or CLR CARRY, etc.
{ "OR #imm,PSU"    , "", "PSU |= imm & %01100111;"              , "Bit set"                                          , "None"                               , "OUTPUT, IOF, UF1, UF2, SP"  , "Unchanged"                                                          }, // $76 or SET IOF, etc.
{ "OR #imm,PSL"    , "", "PSL |= imm;"                          , "Bit set"                                          , "None"                               , "CC, H, B, W, V, LOGICOMP, C", "CC |= (imm & $C0) >> 6;"                                            }, // $77 or SET CARRY, etc.
{ "CALL,NE r0,R^rel","", "if (r0 != 0) gosub rel;"              , "Call if register r0 not equal to zero"            , "SP"                                 , "SP"                         , "Unchanged"                                                          }, // $78
{ "CALL,NE r1,R^rel","", "if (r1 != 0) gosub rel;"              , "Call if register r1 not equal to zero"            , "SP, B"                              , "SP"                         , "Unchanged"                                                          }, // $79
{ "CALL,NE r2,R^rel","", "if (r2 != 0) gosub rel;"              , "Call if register r2 not equal to zero"            , "SP, B"                              , "SP"                         , "Unchanged"                                                          }, // $7A
{ "CALL,NE r3,R^rel","", "if (r3 != 0) gosub rel;"              , "Call if register r3 not equal to zero"            , "SP, B"                              , "SP"                         , "Unchanged"                                                          }, // $7B
{ "CALL,NE r0,U^abs","", "if (r0 != 0) gosub abs;"              , "Call if register r0 not equal to zero"            , "SP"                                 , "SP"                         , "Unchanged"                                                          }, // $7C
{ "CALL,NE r1,U^abs","", "if (r1 != 0) gosub abs;"              , "Call if register r1 not equal to zero"            , "SP, B"                              , "SP"                         , "Unchanged"                                                          }, // $7D
{ "CALL,NE r2,U^abs","", "if (r2 != 0) gosub abs;"              , "Call if register r2 not equal to zero"            , "SP, B"                              , "SP"                         , "Unchanged"                                                          }, // $7E
{ "CALL,NE r3,U^abs","", "if (r3 != 0) gosub abs;"              , "Call if register r3 not equal to zero"            , "SP, B"                              , "SP"                         , "Unchanged"                                                          }, // $7F
{ "ADD r0,r0"      , "", "r0 *= 2;"                             , "Add r0 to r0"                                     , "W, C"                               , "CC, H, V, C"                , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $80 (all ADD could also be ADDC). Or "r0 += r0;"
{ "ADD r1,r0"      , "", "r0 += r1;"                            , "Add r1 to r0"                                     , "B, W, C2"                           , "CC, H, V, C"                , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $81
{ "ADD r2,r0"      , "", "r0 += r2;"                            , "Add r2 to r0"                                     , "B, W, C2"                           , "CC, H, V, C"                , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $82
{ "ADD r3,r0"      , "", "r0 += r3;"                            , "Add r3 to r0"                                     , "B, W, C2"                           , "CC, H, V, C"                , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $83
{ "ADD #imm,r0"    , "", "r0 += imm;"                           , "Add imm to r0"                                    , "W, C"                               , "CC, H, V, C"                , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $84
{ "ADD #imm,r1"    , "", "r1 += imm;"                           , "Add imm to r1"                                    , "B, W, C2"                           , "CC, H, V, C"                , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $85
{ "ADD #imm,r2"    , "", "r2 += imm;"                           , "Add imm to r2"                                    , "B, W, C2"                           , "CC, H, V, C"                , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $86
{ "ADD #imm,r3"    , "", "r3 += imm;"                           , "Add imm to r3"                                    , "B, W, C2"                           , "CC, H, V, C"                , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $87
{ "ADD R^rel,r0"   , "", "r0 += *(rel);"                        , "Add *rel to r0"                                   , "W, C"                               , "CC, H, V, C"                , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $88
{ "ADD R^rel,r1"   , "", "r1 += *(rel);"                        , "Add *rel to r1"                                   , "B, W, C2"                           , "CC, H, V, C"                , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $89
{ "ADD R^rel,r2"   , "", "r2 += *(rel);"                        , "Add *rel to r2"                                   , "B, W, C2"                           , "CC, H, V, C"                , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $8A
{ "ADD R^rel,r3"   , "", "r3 += *(rel);"                        , "Add *rel to r3"                                   , "B, W, C2"                           , "CC, H, V, C"                , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $8B
{ "ADD U^abs,r0"   , "", "r0 += *(abs);"                        , "Add *abs to r0"                                   , "W, C"                               , "CC, H, V, C"                , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $8C
{ "ADD U^abs,r1"   , "", "r1 += *(abs);"                        , "Add *abs to r1"                                   , "B, W, C2"                           , "CC, H, V, C"                , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $8D
{ "ADD U^abs,r2"   , "", "r2 += *(abs);"                        , "Add *abs to r2"                                   , "B, W, C2"                           , "CC, H, V, C"                , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $8E
{ "ADD U^abs,r3"   , "", "r3 += *(abs);"                        , "Add *abs to r3"                                   , "B, W, C2"                           , "CC, H, V, C"                , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $8F
{ "--------"       , "", "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $90
{ "--------"       , "", "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $91
{ "MOVE r0,PSU"    , "", "PSU &= %10000000; PSU |= (r0 & %01100111);","Move r0 into PSU"                             , "None"                               , "OUTPUT, IOF, UF1, UF2, SP"  , "Unchanged"                                                          }, // $92
{ "MOVE r0,PSL"    , "", "PSL = r0;"                            , "Move r0 into PSL"                                 , "None"                               , "CC, H, B, W, V, LOGICOMP, C", "CC = (imm & $C0) >> 6;"                                             }, // $93
{ "DAA r0"         , "", "r0 = BCD(r0);"                        , "Decimal adjust"                                   , "H, C"                               , "CC"                         , "Meaningless"                                                        }, // $94
{ "DAA r1"         , "", "r1 = BCD(r1);"                        , "Decimal adjust"                                   , "H, B, C"                            , "CC"                         , "Meaningless"                                                        }, // $95
{ "DAA r2"         , "", "r2 = BCD(r2);"                        , "Decimal adjust"                                   , "H, B, C"                            , "CC"                         , "Meaningless"                                                        }, // $96
{ "DAA r3"         , "", "r3 = BCD(r3);"                        , "Decimal adjust"                                   , "H, B, C"                            , "CC"                         , "Meaningless"                                                        }, // $97
{ "JUMP,NE R^rel"  , "", "if != goto rel;"                      , "Jump if not equal"                                , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $98
{ "JUMP,LE R^rel"  , "", "if <= goto rel;"                      , "Jump if less than or equal"                       , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $99
{ "JUMP,GE R^rel"  , "", "if >= goto rel;"                      , "Jump if greater than or equal"                    , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $9A
{ "JUMP 0+zero"    , "", "goto zero;"                           , "Jump always"                                      , "None"                               , "None"                       , "Unchanged"                                                          }, // $9B
{ "JUMP,NE U^abs"  , "", "if != goto abs;"                      , "Jump if not equal"                                , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $9C
{ "JUMP,LE U^abs"  , "", "if <= goto abs;"                      , "Jump if less than or equal"                       , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $9D
{ "JUMP,GE U^abs"  , "", "if >= goto abs;"                      , "Jump if greater than or equal"                    , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $9E
{ "JUMP U^{r3}+abs", "", "goto abs + r3;"                       , "Jump always"                                      , "B"                                  , "None"                       , "Unchanged"                                                          }, // $9F
{ "SUB r0,r0"      , "", "r0 -= r0;"                            , "Subtract r0 from r0"                              , "W, C"                               , "CC, H, V, C"                , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $A0 (all SUB could also be SUBC)
{ "SUB r1,r0"      , "", "r0 -= r1;"                            , "Subtract r1 from r0"                              , "B, W, C"                            , "CC, H, V, C"                , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $A1
{ "SUB r2,r0"      , "", "r0 -= r2;"                            , "Subtract r2 from r0"                              , "B, W, C"                            , "CC, H, V, C"                , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $A2
{ "SUB r3,r0"      , "", "r0 -= r3;"                            , "Subtract r3 from r0"                              , "B, W, C"                            , "CC, H, V, C"                , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $A3
{ "SUB #imm,r0"    , "", "r0 -= imm;"                           , "Subtract imm from r0"                             , "W, C"                               , "CC, H, V, C"                , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $A4
{ "SUB #imm,r1"    , "", "r1 -= imm;"                           , "Subtract imm from r1"                             , "B, W, C"                            , "CC, H, V, C"                , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $A5
{ "SUB #imm,r2"    , "", "r2 -= imm;"                           , "Subtract imm from r2"                             , "B, W, C"                            , "CC, H, V, C"                , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $A6
{ "SUB #imm,r3"    , "", "r3 -= imm;"                           , "Subtract imm from r3"                             , "B, W, C"                            , "CC, H, V, C"                , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $A7
{ "SUB R^rel,r0"   , "", "r0 -= *(rel);"                        , "Subtract *rel from r0"                            , "W, C"                               , "CC, H, V, C"                , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $A8
{ "SUB R^rel,r1"   , "", "r1 -= *(rel);"                        , "Subtract *rel from r1"                            , "B, W, C"                            , "CC, H, V, C"                , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $A9
{ "SUB R^rel,r2"   , "", "r2 -= *(rel);"                        , "Subtract *rel from r2"                            , "B, W, C"                            , "CC, H, V, C"                , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $AA
{ "SUB R^rel,r3"   , "", "r3 -= *(rel);"                        , "Subtract *rel from r3"                            , "B, W, C"                            , "CC, H, V, C"                , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $AB
{ "SUB U^abs,r0"   , "", "r0 -= *(abs);"                        , "Subtract *abs from r0"                            , "W, C"                               , "CC, H, V, C"                , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $AC
{ "SUB U^abs,r1"   , "", "r1 -= *(abs);"                        , "Subtract *abs from r1"                            , "B, W, C"                            , "CC, H, V, C"                , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $AD
{ "SUB U^abs,r2"   , "", "r2 -= *(abs);"                        , "Subtract *abs from r2"                            , "B, W, C"                            , "CC, H, V, C"                , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $AE
{ "SUB U^abs,r3"   , "", "r3 -= *(abs);"                        , "Subtract *abs from r3"                            , "B, W, C"                            , "CC, H, V, C"                , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $AF
{ "MOVE r0,$CTRL"  , "", "IOPORT(CTRL) = r0;"                   , "Move from control port to r0"                     , "None"                               , "None"                       , "Unchanged"                                                          }, // $B0
{ "MOVE r1,$CTRL"  , "", "IOPORT(CTRL) = r1;"                   , "Move from control port to r1"                     , "B"                                  , "None"                       , "Unchanged"                                                          }, // $B1
{ "MOVE r2,$CTRL"  , "", "IOPORT(CTRL) = r2;"                   , "Move from control port to r2"                     , "B"                                  , "None"                       , "Unchanged"                                                          }, // $B2
{ "MOVE r3,$CTRL"  , "", "IOPORT(CTRL) = r3;"                   , "Move from control port to r3"                     , "B"                                  , "None"                       , "Unchanged"                                                          }, // $B3
{ "TEST PSU:#imm"  , "", "compare (PSU & imm) against imm;"     , "Test PSU's imm bits"                              , "INPUT, OUTPUT, IOF, UF1, UF2, SP"   , "CC"                         , "CC = (PSU & imm == imm) ? EQ : LT;"                                 }, // $B4
{ "TEST PSL:#imm"  , "", "compare (PSL & imm) against imm;"     , "Test PSL's imm bits"                              , "CC, H, B, W, V, LOGICOMP, C"        , "CC"                         , "CC = (PSL & imm == imm) ? EQ : LT;"                                 }, // $B5
{ "--------"       , "", "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $B6
{ "--------"       , "", "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $B7
{ "CALL,NE R^rel"  , "", "if != gosub rel;"                     , "Call if not equal"                                , "SP, CC"                             , "SP"                         , "Unchanged"                                                          }, // $B8
{ "CALL,LE R^rel"  , "", "if <= gosub rel;"                     , "Call if less than or equal"                       , "SP, CC"                             , "SP"                         , "Unchanged"                                                          }, // $B9
{ "CALL,GE R^rel"  , "", "if >= gosub rel;"                     , "Call if greater than or equal"                    , "SP, CC"                             , "SP"                         , "Unchanged"                                                          }, // $BA
{ "CALL 0+zero"    , "", "gosub zero;"                          , "Call always"                                      , "SP"                                 , "SP"                         , "Unchanged"                                                          }, // $BB
{ "CALL,NE U^abs"  , "", "if != gosub abs;"                     , "Call if not equal"                                , "SP, CC"                             , "SP"                         , "Unchanged"                                                          }, // $BC
{ "CALL,LE U^abs"  , "", "if <= gosub abs;"                     , "Call if less than or equal"                       , "SP, CC"                             , "SP"                         , "Unchanged"                                                          }, // $BD
{ "CALL,GE U^abs"  , "", "if >= gosub abs;"                     , "Call if greater than or equal"                    , "SP, CC"                             , "SP"                         , "Unchanged"                                                          }, // $BE
{ "CALL U^{r3}+abs", "", "gosub abs + r3;"                      , "Call always"                                      , "SP, B"                              , "SP"                         , "Unchanged"                                                          }, // $BF
{ "NOP"            , "", ";"                                    , "No operation"                                     , "None"                               , "None"                       , "Unchanged"                                                          }, // $C0
{ "MOVE r0,r1"     , "", "r0 = r1;"                             , "Move r1 into r0"                                  , "B"                                  , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $C1
{ "MOVE r0,r2"     , "", "r0 = r2;"                             , "Move r2 into r0"                                  , "B"                                  , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $C2
{ "MOVE r0,r3"     , "", "r0 = r3;"                             , "Move r3 into r0"                                  , "B"                                  , "CC"                         , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $C3
{ "--------"       , "", "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $C4
{ "--------"       , "", "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $C5
{ "--------"       , "", "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $C6
{ "--------"       , "", "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $C7
{ "MOVE r0,R^rel"  , "", "*(rel) = r0;"                         , "Move r0 into *rel"                                , "None"                               , "None"                       , "Unchanged"                                                          }, // $C8
{ "MOVE r1,R^rel"  , "", "*(rel) = r1;"                         , "Move r1 into *rel"                                , "B"                                  , "None"                       , "Unchanged"                                                          }, // $C9
{ "MOVE r2,R^rel"  , "", "*(rel) = r2;"                         , "Move r2 into *rel"                                , "B"                                  , "None"                       , "Unchanged"                                                          }, // $CA
{ "MOVE r3,R^rel"  , "", "*(rel) = r3;"                         , "Move r3 into *rel"                                , "B"                                  , "None"                       , "Unchanged"                                                          }, // $CB
{ "MOVE r0,U^abs"  , "", "*(abs) = r0;"                         , "Move r0 into *abs"                                , "None"                               , "None"                       , "Unchanged"                                                          }, // $CC
{ "MOVE r1,U^abs"  , "", "*(abs) = r1;"                         , "Move r1 into *abs"                                , "B"                                  , "None"                       , "Unchanged"                                                          }, // $CD
{ "MOVE r2,U^abs"  , "", "*(abs) = r2;"                         , "Move r2 into *abs"                                , "B"                                  , "None"                       , "Unchanged"                                                          }, // $CE
{ "MOVE r3,U^abs"  , "", "*(abs) = r3;"                         , "Move r3 into *abs"                                , "B"                                  , "None"                       , "Unchanged"                                                          }, // $CF
{ "RL r0"          , "", "r0 <<= 1;"                            , "Rotate left"                                      , "W, C"                               , "H, V, C"                    , "if (r0 & $80) CC = LT; elif (r0 == 0) CC = EQ; else CC = GT;"       }, // $D0 or RLC
{ "RL r1"          , "", "r1 <<= 1;"                            , "Rotate left"                                      , "B, W, C"                            , "H, V, C"                    , "if (r1 & $80) CC = LT; elif (r1 == 0) CC = EQ; else CC = GT;"       }, // $D1 or RLC
{ "RL r2"          , "", "r2 <<= 1;"                            , "Rotate left"                                      , "B, W, C"                            , "H, V, C"                    , "if (r2 & $80) CC = LT; elif (r2 == 0) CC = EQ; else CC = GT;"       }, // $D2 or RLC
{ "RL r3"          , "", "r3 <<= 1;"                            , "Rotate left"                                      , "B, W, C"                            , "H, V, C"                    , "if (r3 & $80) CC = LT; elif (r3 == 0) CC = EQ; else CC = GT;"       }, // $D3 or RLC
{ "MOVE r0,$port"  , "", "IOPORT(port) = r0;"                   , "Move from r0 into extended I/O port"              , "None"                               , "None"                       , "Unchanged"                                                          }, // $D4
{ "MOVE r1,$port"  , "", "IOPORT(port) = r1;"                   , "Move from r1 into extended I/O port"              , "B"                                  , "None"                       , "Unchanged"                                                          }, // $D5
{ "MOVE r2,$port"  , "", "IOPORT(port) = r2;"                   , "Move from r2 into extended I/O port"              , "B"                                  , "None"                       , "Unchanged"                                                          }, // $D6
{ "MOVE r3,$port"  , "", "IOPORT(port) = r3;"                   , "Move from r3 into extended I/O port"              , "B"                                  , "None"                       , "Unchanged"                                                          }, // $D7
{ "IJ,NE r0,R^rel" , "", "if (++r0 != 0) goto rel;"             , "Increment r0 and jump if not equal to zero"       , "None"                               , "None"                       , "Unchanged"                                                          }, // $D8 or INC r0
{ "IJ,NE r1,R^rel" , "", "if (++r1 != 0) goto rel;"             , "Increment r1 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $D9 or INC r1
{ "IJ,NE r2,R^rel" , "", "if (++r2 != 0) goto rel;"             , "Increment r2 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $DA or INC r2
{ "IJ,NE r3,R^rel" , "", "if (++r3 != 0) goto rel;"             , "Increment r3 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $DB or INC r3
{ "IJ,NE r0,U^abs" , "", "if (++r0 != 0) goto abs;"             , "Increment r0 and jump if not equal to zero"       , "None"                               , "None"                       , "Unchanged"                                                          }, // $DC or INC r0
{ "IJ,NE r1,U^abs" , "", "if (++r1 != 0) goto abs;"             , "Increment r1 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $DD or INC r1
{ "IJ,NE r2,U^abs" , "", "if (++r2 != 0) goto abs;"             , "Increment r2 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $DE or INC r2
{ "IJ,NE r3,U^abs" , "", "if (++r3 != 0) goto abs;"             , "Increment r3 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $DF or INC r3
{ "COMP r0,r0"     , "", "CC = EQ;"                             , "Compare r0 against r0"                            , "LOGICOMP"                           , "CC"                         , "CC = EQ;"                                                           }, // $E0
{ "COMP r1,r0"     , "", "compare r0 against r1;"               , "Compare r1 against r0"                            , "B, LOGICOMP"                        , "CC"                         , "if (r0 > r1) CC = GT; elif (r0 < r1) CC = LT; else CC = EQ;"        }, // $E1
{ "COMP r2,r0"     , "", "compare r1 against r2;"               , "Compare r2 against r0"                            , "B, LOGICOMP"                        , "CC"                         , "if (r0 > r2) CC = GT; elif (r0 < r2) CC = LT; else CC = EQ;"        }, // $E2
{ "COMP r3,r0"     , "", "compare r2 against r3;"               , "Compare r3 against r0"                            , "B, LOGICOMP"                        , "CC"                         , "if (r0 > r3) CC = GT; elif (r0 < r3) CC = LT; else CC = EQ;"        }, // $E3
{ "COMP #imm,r0"   , "", "compare imm against r0;"              , "Compare r0 against imm"                           , "LOGICOMP"                           , "CC"                         , "if (r0 > imm) CC = GT; elif (r0 < imm) CC = LT; else CC = EQ;"      }, // $E4
{ "COMP #imm,r1"   , "", "compare imm against r1;"              , "Compare r1 against imm"                           , "B, LOGICOMP"                        , "CC"                         , "if (r1 > imm) CC = GT; elif (r1 < imm) CC = LT; else CC = EQ;"      }, // $E5
{ "COMP #imm,r2"   , "", "compare imm against r2;"              , "Compare r2 against imm"                           , "B, LOGICOMP"                        , "CC"                         , "if (r2 > imm) CC = GT; elif (r2 < imm) CC = LT; else CC = EQ;"      }, // $E6
{ "COMP #imm,r3"   , "", "compare imm against r3;"              , "Compare r3 against imm"                           , "B, LOGICOMP"                        , "CC"                         , "if (r3 > imm) CC = GT; elif (r3 < imm) CC = LT; else CC = EQ;"      }, // $E7
{ "COMP R^rel,r0"  , "", "compare *rel against r0;"             , "Compare r0 against *(rel)"                        , "LOGICOMP"                           , "CC"                         , "if (r0 > *(rel)) CC = GT; elif (r0 < *(rel)) CC = LT; else CC = EQ;"}, // $E8
{ "COMP R^rel,r1"  , "", "compare *rel against r1;"             , "Compare r1 against *(rel)"                        , "B, LOGICOMP"                        , "CC"                         , "if (r1 > *(rel)) CC = GT; elif (r1 < *(rel)) CC = LT; else CC = EQ;"}, // $E9
{ "COMP R^rel,r2"  , "", "compare *rel against r2;"             , "Compare r2 against *(rel)"                        , "B, LOGICOMP"                        , "CC"                         , "if (r2 > *(rel)) CC = GT; elif (r2 < *(rel)) CC = LT; else CC = EQ;"}, // $EA
{ "COMP R^rel,r3"  , "", "compare *rel against r3;"             , "Compare r3 against *(rel)"                        , "B, LOGICOMP"                        , "CC"                         , "if (r3 > *(rel)) CC = GT; elif (r3 < *(rel)) CC = LT; else CC = EQ;"}, // $EB
{ "COMP U^abs,r0"  , "", "compare *abs against r0;"             , "Compare r0 against *(abs)"                        , "LOGICOMP"                           , "CC"                         , "if (r0 > *(abs)) CC = GT; elif (r0 < *(abs)) CC = LT; else CC = EQ;"}, // $EC
{ "COMP U^abs,r1"  , "", "compare *abs against r1;"             , "Compare r1 against *(abs)"                        , "B, LOGICOMP"                        , "CC"                         , "if (r1 > *(abs)) CC = GT; elif (r1 < *(abs)) CC = LT; else CC = EQ;"}, // $ED
{ "COMP U^abs,r2"  , "", "compare *abs against r2;"             , "Compare r2 against *(abs)"                        , "B, LOGICOMP"                        , "CC"                         , "if (r2 > *(abs)) CC = GT; elif (r2 < *(abs)) CC = LT; else CC = EQ;"}, // $EE
{ "COMP U^abs,r3"  , "", "compare *abs against r3;"             , "Compare r3 against *(abs)"                        , "B, LOGICOMP"                        , "CC"                         , "if (r3 > *(abs)) CC = GT; elif (r3 < *(abs)) CC = LT; else CC = EQ;"}, // $EF
{ "MOVE r0,$DATA"  , "", "IOPORT(DATA) = r0;"                   , "Move from r0 into data port"                      , "None"                               , "None"                       , "Unchanged"                                                          }, // $F0
{ "MOVE r1,$DATA"  , "", "IOPORT(DATA) = r1;"                   , "Move from r1 into data port"                      , "B"                                  , "None"                       , "Unchanged"                                                          }, // $F1
{ "MOVE r2,$DATA"  , "", "IOPORT(DATA) = r2;"                   , "Move from r2 into data port"                      , "B"                                  , "None"                       , "Unchanged"                                                          }, // $F2
{ "MOVE r3,$DATA"  , "", "IOPORT(DATA) = r3;"                   , "Move from r3 into data port"                      , "B"                                  , "None"                       , "Unchanged"                                                          }, // $F3
{ "TEST r0:#imm"   , "", "CC = (r0 & imm == imm) ? EQ : LT;"    , "Test r0's imm bits"                               , "None"                               , "CC"                         , "CC = (r0 & imm == imm) ? EQ : LT;"                                  }, // $F4
{ "TEST r1:#imm"   , "", "CC = (r1 & imm == imm) ? EQ : LT;"    , "Test r1's imm bits"                               , "B"                                  , "CC"                         , "CC = (r1 & imm == imm) ? EQ : LT;"                                  }, // $F5
{ "TEST r2:#imm"   , "", "CC = (r2 & imm == imm) ? EQ : LT;"    , "Test r2's imm bits"                               , "B"                                  , "CC"                         , "CC = (r2 & imm == imm) ? EQ : LT;"                                  }, // $F6
{ "TEST r3:#imm"   , "", "CC = (r3 & imm == imm) ? EQ : LT;"    , "Test r3's imm bits"                               , "B"                                  , "CC"                         , "CC = (r3 & imm == imm) ? EQ : LT;"                                  }, // $F7
{ "DJ,NE r0,R^rel" , "", "if (--r0 != 0) goto rel;"             , "Decrement r0 and jump if not equal to zero"       , "None"                               , "None"                       , "Unchanged"                                                          }, // $F8 or DEC r0
{ "DJ,NE r1,R^rel" , "", "if (--r1 != 0) goto rel;"             , "Decrement r1 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $F9 or DEC r1
{ "DJ,NE r2,R^rel" , "", "if (--r2 != 0) goto rel;"             , "Decrement r2 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $FA or DEC r2
{ "DJ,NE r3,R^rel" , "", "if (--r3 != 0) goto rel;"             , "Decrement r3 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $FB or DEC r3
{ "DJ,NE r0,U^abs" , "", "if (--r0 != 0) goto abs;"             , "Decrement r0 and jump if not equal to zero"       , "None"                               , "None"                       , "Unchanged"                                                          }, // $FC or DEC r0
{ "DJ,NE r1,U^abs" , "", "if (--r1 != 0) goto abs;"             , "Decrement r1 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $FD or DEC r1
{ "DJ,NE r2,U^abs" , "", "if (--r2 != 0) goto abs;"             , "Decrement r2 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $FE or DEC r2
{ "DJ,NE r3,U^abs" , "", "if (--r3 != 0) goto abs;"             , "Decrement r3 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $FF or DEC r3
}, {
// IEEE-694 format
{ "MOV .0,.0",   "",             ";"                                    , "Move .0 into .0"                                  , "None"                               , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $00
{ "MOV .0,.1",   "",             ".0 = .1;"                             , "Move .1 into .0"                                  , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $01
{ "MOV .0,.2",   "",             ".0 = .2;"                             , "Move .2 into .0"                                  , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $02
{ "MOV .0,.3",   "",             ".0 = .3;"                             , "Move .3 into .0"                                  , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $03
{ "LD .0",       ",#imm",        ".0 = imm;"                            , "Load .0 with imm"                                 , "None"                               , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $04
{ "LD .1",       ",#imm",        ".1 = imm;"                            , "Load .1 with imm"                                 , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $05
{ "LD .2",       ",#imm",        ".2 = imm;"                            , "Load .2 with imm"                                 , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $06
{ "LD .3",       ",#imm",        ".3 = imm;"                            , "Load .3 with imm"                                 , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $07
{ "LD .0",       ",$rel",        ".0 = *(rel);"                         , "Load .0 with *(rel)"                              , "None"                               , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $08
{ "LD .1",       ",$rel",        ".1 = *(rel);"                         , "Load .1 with *(rel)"                              , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $09
{ "LD .2",       ",$rel",        ".2 = *(rel);"                         , "Load .2 with *(rel)"                              , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $0A
{ "LD .3",       ",$rel",        ".3 = *(rel);"                         , "Load .3 with *(rel)"                              , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $0B
{ "LD .0",       ",/abs",        ".0 = *(abs);"                         , "Load .0 with *(abs)"                              , "None"                               , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $0C
{ "LD .1",       ",/abs",        ".1 = *(abs);"                         , "Load .1 with *(abs)"                              , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $0D
{ "LD .2",       ",/abs",        ".2 = *(abs);"                         , "Load .2 with *(abs)"                              , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $0E
{ "LD .3",       ",/abs",        ".3 = *(abs);"                         , "Load .3 with *(abs)"                              , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $0F
{ "LD .L",       ",/abs",        ".L = *(abs);"                         , "Load .L with *(abs)"                              , "None"                               , "CC, H, B, W, V, LOGICOMP, C", ".CC = (*(abs) & $C0) >> 6;"                                         }, // $10 LOAD L,<absolute>. 2560-B only
{ "ST .L",       ",/abs",        "*(abs) = .L;"                         , "Store .L in *(abs)"                               , "CC, H, B, W, V, LOGICOMP, C"        , "None"                       , "Undefined"                                                          }, // $11 LOAD <absolute>,L. 2560-B only
{ "MOV .0,.U",   "",             ".0 = .U;"                             , "Move .U into .0"                                  , "INPUT, OUTPUT, IOF, UF1, UF2, STACK", "CC"                         , "if (.S) .CC = LT; elif (.U == 0) .CC = EQ; else .CC = GT;"          }, // $12
{ "MOV .0,.L",   "",             ".0 = .L;"                             , "Move .U into .0"                                  , "CC, H, B, W, V, LOGICOMP, C"        , "CC"                         , "if (.CC == LT || .CC == UN) .CC = LT; elif (.L == 0) .CC = EQ; else .CC = GT;" }, // $13
{ "RETEQ",       "",             "if == return;"                        , "Return if EQ true"                                , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $14
{ "RETGT",       "",             "if > return;"                         , "Return if GT true"                                , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $15
{ "RETLT",       "",             "if < return;"                         , "Return if LT true"                                , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $16
{ "RET",         "",             "return;"                              , "Return"                                           , "STACK"                              , "STACK"                      , "Unchanged"                                                          }, // $17
{ "BEQ",         " $rel",        "if == goto rel;"                      , "Jump if EQ true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $18
{ "BGT",         " $rel",        "if > goto rel;"                       , "Jump if GT true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $19
{ "BLT",         " $rel",        "if < goto rel;"                       , "Jump if LT true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $1A
{ "BR",          " $rel",        "goto rel;"                            , "Jump"                                             , "None"                               , "None"                       , "Unchanged"                                                          }, // $1B
{ "BEQ",         " /abs",        "if == goto abs;"                      , "Jump if EQ true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $1C
{ "BGT",         " /abs",        "if > goto abs;"                       , "Jump if GT true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $1D
{ "BLT",         " /abs",        "if < goto abs;"                       , "Jump if LT true"                                  , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $1E
{ "BR",          " /abs",        "goto abs;"                            , "Jump"                                             , "None"                               , "None"                       , "Unchanged"                                                          }, // $1F
{ "CLR .0",      "",             ".0 = 0;"                              , "Clear .0"                                         , "None"                               , "CC"                         , ".CC = EQ;"                                                          }, // $20
{ "XOR .0,.1",   "",             ".0 ^= .1;"                            , "Exclusive OR .0 with .1"                          , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $21
{ "XOR .0,.2",   "",             ".0 ^= .2;"                            , "Exclusive OR .0 with .2"                          , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $22
{ "XOR .0,.3",   "",             ".0 ^= .3;"                            , "Exclusive OR .0 with .3"                          , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $23
{ "XOR .0",      ",#imm",        ".0 ^= imm;"                           , "Exclusive OR .0 with imm"                         , "None"                               , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $24
{ "XOR .1",      ",#imm",        ".1 ^= imm;"                           , "Exclusive OR .1 with imm"                         , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $25
{ "XOR .2",      ",#imm",        ".2 ^= imm;"                           , "Exclusive OR .2 with imm"                         , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $26
{ "XOR .3",      ",#imm",        ".3 ^= imm;"                           , "Exclusive OR .3 with imm"                         , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $27
{ "XOR .0",      ",$rel",        ".0 ^= *(rel);"                        , "Exclusive OR .0 with *(rel)"                      , "None"                               , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $28
{ "XOR .1",      ",$rel",        ".1 ^= *(rel);"                        , "Exclusive OR .1 with *(rel)"                      , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $29
{ "XOR .2",      ",$rel",        ".2 ^= *(rel);"                        , "Exclusive OR .2 with *(rel)"                      , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $2A
{ "XOR .3",      ",$rel",        ".3 ^= *(rel);"                        , "Exclusive OR .3 with *(rel)"                      , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $2B
{ "XOR .0",      ",/abs",        ".0 ^= *(abs);"                        , "Exclusive OR .0 with *(abs)"                      , "None"                               , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $2C
{ "XOR .1",      ",/abs",        ".1 ^= *(abs);"                        , "Exclusive OR .1 with *(abs)"                      , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $2D
{ "XOR .2",      ",/abs",        ".2 ^= *(abs);"                        , "Exclusive OR .2 with *(abs)"                      , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $2E
{ "XOR .3",      ",/abs",        ".3 ^= *(abs);"                        , "Exclusive OR .3 with *(abs)"                      , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $2F
{ "IN .0,CTRL",  "",             ".0 = IOPORT(CTRL);"                   , "Read specially decoded peripheral into .0"        , "None"                               , "CC"                         , "if (IOPORT(CTRL) & $80) .CC = LT; elif (IOPORT(CTRL) == 0) .CC = EQ; else .CC = GT;" }, // $30
{ "IN .1,CTRL",  "",             ".1 = IOPORT(CTRL);"                   , "Read specially decoded peripheral into .1"        , "B"                                  , "CC"                         , "if (IOPORT(CTRL) & $80) .CC = LT; elif (IOPORT(CTRL) == 0) .CC = EQ; else .CC = GT;" }, // $31
{ "IN .2,CTRL",  "",             ".2 = IOPORT(CTRL);"                   , "Read specially decoded peripheral into .2"        , "B"                                  , "CC"                         , "if (IOPORT(CTRL) & $80) .CC = LT; elif (IOPORT(CTRL) == 0) .CC = EQ; else .CC = GT;" }, // $32
{ "IN .3,CTRL",  "",             ".3 = IOPORT(CTRL);"                   , "Read specially decoded peripheral into .3"        , "B"                                  , "CC"                         , "if (IOPORT(CTRL) & $80) .CC = LT; elif (IOPORT(CTRL) == 0) .CC = EQ; else .CC = GT;" }, // $33
{ "RETIEQ",      "",             "if == { .U &= ~..U_II; return; }"     , "Return and clear interrupt mask if EQ true"       , "STACK, CC"                          , "IOF, STACK"                 , "Unchanged"                                                          }, // $34
{ "RETIGT",      "",             "if > { .U &= ~.U_II; return; }"       , "Return and clear interrupt mask if GT true"       , "STACK, CC"                          , "IOF, STACK"                 , "Unchanged"                                                          }, // $35
{ "RETILT",      "",             "if < { .U &= ~.U_II; return; }"       , "Return and clear interrupt mask if LT true"       , "STACK, CC"                          , "IOF, STACK"                 , "Unchanged"                                                          }, // $36
{ "RETI",        "",             ".U &= ~.U_II; return;"                , "Return and clear interrupt mask"                  , "STACK"                              , "IOF, STACK"                 , "Unchanged"                                                          }, // $37
{ "CALLEQ",      " $rel",        "if == gosub rel;"                     , "Call if EQ true"                                  , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $38 CALL EQ <relative>
{ "CALLGT",      " $rel",        "if > gosub rel;"                      , "Call if GT true"                                  , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $39 CALL GT <relative>
{ "CALLLT",      " $rel",        "if < gosub rel;"                      , "Call if LT true"                                  , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $3A CALL LT <relative>
{ "CALL",        " $rel",        "gosub rel;"                           , "Call"                                             , "STACK"                              , "STACK"                      , "Unchanged"                                                          }, // $3B CALL <relative>
{ "CALLEQ",      " /abs",        "if == gosub abs;"                     , "Call if EQ true"                                  , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $3C CALL EQ <absolute>
{ "CALLGT",      " /abs",        "if > gosub abs;"                      , "Call if GT true"                                  , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $3D CALL GT <absolute>
{ "CALLLT",      " /abs",        "if < gosub abs;"                      , "Call if LT true"                                  , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $3E CALL LT <absolute>
{ "CALL",        " /abs",        "gosub abs;"                           , "Call"                                             , "STACK"                              , "STACK"                      , "Unchanged"                                                          }, // $3F CALL <absolute>
{ "HALT",        "",             "for (;;);"                            , "Wait for interrupt"                               , "None"                               , "None"                       , "Unchanged"                                                          }, // $40
{ "AND .0,.1",   "",             ".0 &= .1;"                            , "Logical AND .0 with .1"                           , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $41
{ "AND .0,.2",   "",             ".0 &= .2;"                            , "Logical AND .0 with .2"                           , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $42
{ "AND .0,.3",   "",             ".0 &= .3;"                            , "Logical AND .0 with .3"                           , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $43
{ "AND .0",      ",#imm",        ".0 &= imm;"                           , "Logical AND .0 with imm"                          , "None"                               , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $44
{ "AND .1",      ",#imm",        ".1 &= imm;"                           , "Logical AND .1 with imm"                          , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $45
{ "AND .2",      ",#imm",        ".2 &= imm;"                           , "Logical AND .2 with imm"                          , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $46
{ "AND .3",      ",#imm",        ".3 &= imm;"                           , "Logical AND .3 with imm"                          , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $47
{ "AND .0",      ",$rel",        ".0 &= *(rel);"                        , "Logical AND .0 with *(rel)"                       , "None"                               , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $48
{ "AND .1",      ",$rel",        ".1 &= *(rel);"                        , "Logical AND .1 with *(rel)"                       , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $49
{ "AND .2",      ",$rel",        ".2 &= *(rel);"                        , "Logical AND .2 with *(rel)"                       , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $4A
{ "AND .3",      ",$rel",        ".3 &= *(rel);"                        , "Logical AND .3 with *(rel)"                       , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $4B
{ "AND .0",      ",/abs",        ".0 &= *(abs);"                        , "Logical AND .0 with *(abs)"                       , "None"                               , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $4C
{ "AND .1",      ",/abs",        ".1 &= *(abs);"                        , "Logical AND .1 with *(abs)"                       , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $4D
{ "AND .2",      ",/abs",        ".2 &= *(abs);"                        , "Logical AND .2 with *(abs)"                       , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $4E
{ "AND .3",      ",/abs",        ".3 &= *(abs);"                        , "Logical AND .3 with *(abs)"                       , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $4F
{ "ROR .0",      "",             ".0 >>= 1;"                            , "Rotate .0 right"                                  , "W, C"                               , "CC, H, V, C"                , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $50
{ "ROR .1",      "",             ".1 >>= 1;"                            , "Rotate .1 right"                                  , "B, W, C"                            , "CC, H, V, C"                , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $51
{ "ROR .2",      "",             ".2 >>= 1;"                            , "Rotate .2 right"                                  , "B, W, C"                            , "CC, H, V, C"                , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $52
{ "ROR .3",      "",             ".3 >>= 1;"                            , "Rotate .3 right"                                  , "B, W, C"                            , "CC, H, V, C"                , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $53
{ "LD .0",       ",port",        ".0 = IOPORT(port);"                   , "Read peripheral port data into .0"                , "None"                               , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $54
{ "LD .1",       ",port",        ".1 = IOPORT(port);"                   , "Read peripheral port data into .1"                , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $55
{ "LD .2",       ",port",        ".2 = IOPORT(port);"                   , "Read peripheral port data into .2"                , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $56
{ "LD .3",       ",port",        ".3 = IOPORT(port);"                   , "Read peripheral port data into .3"                , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $57
{ "BNZ",         ".0,$rel",      "if (.0 != 0) goto rel;"               , "Jump if register .0 not equal to zero"            , "None"                               , "None"                       , "Unchanged"                                                          }, // $58 (relative)
{ "BNZ",         ".1,$rel",      "if (.1 != 0) goto rel;"               , "Jump if register .1 not equal to zero"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $59 (relative)
{ "BNZ",         ".2,$rel",      "if (.2 != 0) goto rel;"               , "Jump if register .2 not equal to zero"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $5A (relative)
{ "BNZ",         ".3,$rel",      "if (.3 != 0) goto rel;"               , "Jump if register .3 not equal to zero"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $5B (relative)
{ "BNZ",         ".0,/abs",      "if (.0 != 0) goto abs;"               , "Jump if register .0 not equal to zero"            , "None"                               , "None"                       , "Unchanged"                                                          }, // $5C (absolute)
{ "BNZ",         ".1,/abs",      "if (.1 != 0) goto abs;"               , "Jump if register .1 not equal to zero"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $5D (absolute)
{ "BNZ",         ".2,/abs",      "if (.2 != 0) goto abs;"               , "Jump if register .2 not equal to zero"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $5E (absolute)
{ "BNZ",         ".3,/abs",      "if (.3 != 0) goto abs;"               , "Jump if register .3 not equal to zero"            , "B"                                  , "None"                       , "Unchanged"                                                          }, // $5F (absolute)
{ "OR .0,.0",    "",             ".0 |= .0;"                            , "Logical OR .0 with .0"                            , "None"                               , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $60
{ "OR .0,.1",    "",             ".0 |= .1;"                            , "Logical OR .0 with .1"                            , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $61
{ "OR .0,.2",    "",             ".0 |= .2;"                            , "Logical OR .0 with .2"                            , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $62
{ "OR .0,.3",    "",             ".0 |= .3;"                            , "Logical OR .0 with .3"                            , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $63
{ "OR .0",       ",#imm",        ".0 |= imm;"                           , "Logical OR .0 with imm"                           , "None"                               , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $64 OR .0,#<value>
{ "OR .1",       ",#imm",        ".1 |= imm;"                           , "Logical OR .1 with imm"                           , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $65 OR .1,#<value>
{ "OR .2",       ",#imm",        ".2 |= imm;"                           , "Logical OR .2 with imm"                           , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $66 OR C,#<value>
{ "OR .3",       ",#imm",        ".3 |= imm;"                           , "Logical OR .3 with imm"                           , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $67 OR D,#<value>
{ "OR .0",       ",$rel",        ".0 |= *(rel);"                        , "Logical OR .0 with *(rel)"                        , "None"                               , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $68 OR .0,<relative>
{ "OR .1",       ",$rel",        ".1 |= *(rel);"                        , "Logical OR .1 with *(rel)"                        , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $69 OR B,<relative>
{ "OR .2",       ",$rel",        ".2 |= *(rel);"                        , "Logical OR .2 with *(rel)"                        , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $6A OR C,<relative>
{ "OR .3",       ",$rel",        ".3 |= *(rel);"                        , "Logical OR .3 with *(rel)"                        , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $6B OR D,<relative>
{ "OR .0",       ",/abs",        ".0 |= *(abs);"                        , "Logical OR .0 with *(abs)"                        , "None"                               , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $6C OR .0,<absolute>
{ "OR .1",       ",/abs",        ".1 |= *(abs);"                        , "Logical OR .1 with *(abs)"                        , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $6D OR B,<absolute>
{ "OR .2",       ",/abs",        ".2 |= *(abs);"                        , "Logical OR .2 with *(abs)"                        , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $6E OR C,<absolute>
{ "OR .3",       ",/abs",        ".3 |= *(abs);"                        , "Logical OR .3 with *(abs)"                        , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $6F OR D,<absolute>
{ "IN .0,DATA",  "",             ".0 = IOPORT(DATA);"                   , "Read specially decoded peripheral into .0"        , "None"                               , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $70
{ "IN .1,DATA",  "",             ".1 = IOPORT(DATA);"                   , "Read specially decoded peripheral into .1"        , "B"                                  , "CC"                         , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $71
{ "IN .2,DATA",  "",             ".2 = IOPORT(DATA);"                   , "Read specially decoded peripheral into .2"        , "B"                                  , "CC"                         , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $72
{ "IN .3,DATA",  "",             ".3 = IOPORT(DATA);"                   , "Read specially decoded peripheral into .3"        , "B"                                  , "CC"                         , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $73
{ "AND .U",      ",#~imm",       ".U &= ~(imm & %01100111);"            , "Bit clear"                                        , "None"                               , "OUTPUT, IOF, UF1, UF2, STACK","Unchanged"                                                          }, // $74
{ "AND .L",      ",#~imm",       ".L &= ~(imm);"                        , "Bit clear"                                        , "None"                               , "CC, H, B, W, V, LOGICOMP, C", ".CC &= ~((imm & $C0) >> 6);"                                        }, // $75
{ "OR .U",       ",#imm",        ".U |= imm & %01100111;"               , "Bit set"                                          , "None"                               , "OUTPUT, IOF, UF1, UF2, STACK","Unchanged"                                                          }, // $76
{ "OR .L",       ",#imm",        ".L |= imm;"                           , "Bit set"                                          , "None"                               , "CC, H, B, W, V, LOGICOMP, C", ".CC |= (imm & $C0) >> 6;"                                           }, // $77
{ "CALLNZ .0",   ",$rel",        "if (.0 != 0) gosub rel;"              , "Call if register .0 not equal to zero"            , "STACK"                              , "STACK"                      , "Unchanged"                                                          }, // $78
{ "CALLNZ .1",   ",$rel",        "if (.1 != 0) gosub rel;"              , "Call if register .1 not equal to zero"            , "STACK, B"                           , "STACK"                      , "Unchanged"                                                          }, // $79
{ "CALLNZ .2",   ",$rel",        "if (.2 != 0) gosub rel;"              , "Call if register .2 not equal to zero"            , "STACK, B"                           , "STACK"                      , "Unchanged"                                                          }, // $7A
{ "CALLNZ .3",   ",$rel",        "if (.3 != 0) gosub rel;"              , "Call if register .3 not equal to zero"            , "STACK, B"                           , "STACK"                      , "Unchanged"                                                          }, // $7B
{ "CALLNZ .0",   ",/abs",        "if (.0 != 0) gosub abs;"              , "Call if register .0 not equal to zero"            , "STACK"                              , "STACK"                      , "Unchanged"                                                          }, // $7C
{ "CALLNZ .1",   ",/abs",        "if (.1 != 0) gosub abs;"              , "Call if register .1 not equal to zero"            , "STACK, B"                           , "STACK"                      , "Unchanged"                                                          }, // $7D
{ "CALLNZ .2",   ",/abs",        "if (.2 != 0) gosub abs;"              , "Call if register .2 not equal to zero"            , "STACK, B"                           , "STACK"                      , "Unchanged"                                                          }, // $7E
{ "CALLNZ .3",   ",/abs",        "if (.3 != 0) gosub abs;"              , "Call if register .3 not equal to zero"            , "STACK, B"                           , "STACK"                      , "Unchanged"                                                          }, // $7F
{ "ADD .0,.0",   "",             ".0 *= 2;"                             , "Add .0 and .0, result in .0"                      , "W, C"                               , "CC, H, V, C"                , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $80 (all ADD could also be ADDC). Or ".0 += .0;"
{ "ADD .0,.1",   "",             ".0 += .1;"                            , "Add .0 and .1, result in .0"                      , "B, W, C"                            , "CC, H, V, C"                , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $81
{ "ADD .0,.2",   "",             ".0 += .2;"                            , "Add .0 and .2, result in .0"                      , "B, W, C"                            , "CC, H, V, C"                , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $82
{ "ADD .0,.3",   "",             ".0 += .3;"                            , "Add .0 and .3, result in .0"                      , "B, W, C"                            , "CC, H, V, C"                , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $83
{ "ADD .0",      ",#imm",        ".0 += imm;"                           , "Add .0 and imm, result in .0"                     , "W, C"                               , "CC, H, V, C"                , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $84
{ "ADD .1",      ",#imm",        ".1 += imm;"                           , "Add .1 and imm, result in .1"                     , "B, W, C"                            , "CC, H, V, C"                , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $85
{ "ADD .2",      ",#imm",        ".2 += imm;"                           , "Add .2 and imm, result in .2"                     , "B, W, C"                            , "CC, H, V, C"                , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $86
{ "ADD .3",      ",#imm",        ".3 += imm;"                           , "Add .3 and imm, result in .3"                     , "B, W, C"                            , "CC, H, V, C"                , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $87
{ "ADD .0",      ",$rel",        ".0 += *(rel);"                        , "Add .0 and *(rel), result in .0"                  , "W, C"                               , "CC, H, V, C"                , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $88
{ "ADD .1",      ",$rel",        ".1 += *(rel);"                        , "Add .1 and *(rel), result in .1"                  , "B, W, C"                            , "CC, H, V, C"                , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $89
{ "ADD .2",      ",$rel",        ".2 += *(rel);"                        , "Add .2 and *(rel), result in .2"                  , "B, W, C"                            , "CC, H, V, C"                , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $8A
{ "ADD .3",      ",$rel",        ".3 += *(rel);"                        , "Add .3 and *(rel), result in .3"                  , "B, W, C"                            , "CC, H, V, C"                , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $8B
{ "ADD .0",      ",/abs",        ".0 += *(abs);"                        , "Add .0 and *(abs), result in .0"                  , "W, C"                               , "CC, H, V, C"                , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $8C
{ "ADD .1",      ",/abs",        ".1 += *(abs);"                        , "Add .1 and *(abs), result in .1"                  , "B, W, C"                            , "CC, H, V, C"                , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $8D
{ "ADD .2",      ",/abs",        ".2 += *(abs);"                        , "Add .2 and *(abs), result in .2"                  , "B, W, C"                            , "CC, H, V, C"                , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $8E
{ "ADD .3",      ",/abs",        ".3 += *(abs);"                        , "Add .3 and *(abs), result in .3"                  , "B, W, C"                            , "CC, H, V, C"                , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $8F
{ "--------",    "",             "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $90
{ "--------",    "",             "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $91
{ "MOV .U,.0",   "",             ".U &= %10000000; .U |= (.0 & %01100111);","Load .U with .0"                                , "None"                               , "OUTPUT, IOF, UF1, UF2, STACK","Unchanged"                                                          }, // $92
{ "MOV .L,.0",   "",             ".L = .0;"                             , "Load .L with .0"                                  , "None"                               , "CC, H, B, W, V, LOGICOMP, C", ".CC = (imm & $C0) >> 6;"                                         }, // $93
{ "ADJ .0",      "",             ".0 = BCD(.0);"                        , "Decimal adjust .0"                                , "H, C"                               , "CC"                         , "Meaningless"                                                        }, // $94
{ "ADJ .1",      "",             ".1 = BCD(.1);"                        , "Decimal adjust .1"                                , "H, B, C"                            , "CC"                         , "Meaningless"                                                        }, // $95
{ "ADJ .2",      "",             ".2 = BCD(.2);"                        , "Decimal adjust .2"                                , "H, B, C"                            , "CC"                         , "Meaningless"                                                        }, // $96
{ "ADJ .3",      "",             ".3 = BCD(.3);"                        , "Decimal adjust .3"                                , "H, B, C"                            , "CC"                         , "Meaningless"                                                        }, // $97
{ "BNE",         " $rel",        "if != goto rel;"                      , "Jump if not equal"                                , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $98 (relative)
{ "BLE",         " $rel",        "if <= goto rel;"                      , "Jump if less than or equal"                       , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $99 (relative)
{ "BGE",         " $rel",        "if >= goto rel;"                      , "Jump if greater than or equal"                    , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $9A (relative)
{ "BR",          " !zero",       "goto zero;"                           , "Jump always"                                      , "None"                               , "None"                       , "Unchanged"                                                          }, // $9B (zero-page)
{ "BNE",         " /abs",        "if != goto abs;"                      , "Jump if not equal"                                , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $9C (absolute)
{ "BLE",         " /abs",        "if <= goto abs;"                      , "Jump if less than or equal"                       , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $9D (absolute)
{ "BGE",         " /abs",        "if >= goto abs;"                      , "Jump if greater than or equal"                    , "CC"                                 , "None"                       , "Unchanged"                                                          }, // $9E (absolute)
{ "BR",          " /abs(.3)",    "goto abs + .3;"                       , "Jump always"                                      , "B"                                  , "None"                       , "Unchanged"                                                          }, // $9F (indexed)
{ "SUB .0,.0",   "",             ".0 -= .0;"                            , "Subtract .0 from .0, result in .0"                , "W, C"                               , "CC, H, V, C"                , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $A0 (all SUB could also be SUBB)
{ "SUB .0,.1",   "",             ".0 -= .1;"                            , "Subtract .1 from .0, result in .0"                , "B, W, C"                            , "CC, H, V, C"                , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $A1
{ "SUB .0,.2",   "",             ".0 -= .2;"                            , "Subtract .2 from .0, result in .0"                , "B, W, C"                            , "CC, H, V, C"                , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $A2
{ "SUB .0,.3",   "",             ".0 -= .3;"                            , "Subtract .3 from .0, result in .0"                , "B, W, C"                            , "CC, H, V, C"                , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $A3
{ "SUB .0",      ",#imm",        ".0 -= imm;"                           , "Subtract imm from .0, result in .0"               , "W, C"                               , "CC, H, V, C"                , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $A4
{ "SUB .1",      ",#imm",        ".1 -= imm;"                           , "Subtract imm from .1, result in .1"               , "B, W, C"                            , "CC, H, V, C"                , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $A5
{ "SUB .2",      ",#imm",        ".2 -= imm;"                           , "Subtract imm from .2, result in .2"               , "B, W, C"                            , "CC, H, V, C"                , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $A6
{ "SUB .3",      ",#imm",        ".3 -= imm;"                           , "Subtract imm from .3, result in .3"               , "B, W, C"                            , "CC, H, V, C"                , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $A7
{ "SUB .0",      ",$rel",        ".0 -= *(rel);"                        , "Subtract *(rel) from .0, result in .0"            , "W, C"                               , "CC, H, V, C"                , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $A8
{ "SUB .1",      ",$rel",        ".1 -= *(rel);"                        , "Subtract *(rel) from .1, result in..1"            , "B, W, C"                            , "CC, H, V, C"                , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $A9
{ "SUB .2",      ",$rel",        ".2 -= *(rel);"                        , "Subtract *(rel) from .2, result in .2"            , "B, W, C"                            , "CC, H, V, C"                , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $AA
{ "SUB .3",      ",$rel",        ".3 -= *(rel);"                        , "Subtract *(rel) from .3, result in .3"            , "B, W, C"                            , "CC, H, V, C"                , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $AB
{ "SUB .0",      ",/abs",        ".0 -= *(abs);"                        , "Subtract *(abs) from .0, result in .0"            , "W, C"                               , "CC, H, V, C"                , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $AC
{ "SUB .1",      ",/abs",        ".1 -= *(abs);"                        , "Subtract *(abs) from .1, result in .1"            , "B, W, C"                            , "CC, H, V, C"                , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $AD
{ "SUB .2",      ",/abs",        ".2 -= *(abs);"                        , "Subtract *(abs) from .2, result in .2"            , "B, W, C"                            , "CC, H, V, C"                , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $AE
{ "SUB .3",      ",/abs",        ".3 -= *(abs);"                        , "Subtract *(abs) from .3, result in .3"            , "B, W, C"                            , "CC, H, V, C"                , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $AF
{ "OUT .0,CTRL", "",             "IOPORT(CTRL) = .0;"                   , "Write .0 to specially decoded peripheral"         , "None"                               , "None"                       , "Unchanged"                                                          }, // $B0
{ "OUT .1,CTRL", "",             "IOPORT(CTRL) = .1;"                   , "Write .1 to specially decoded peripheral"         , "B"                                  , "None"                       , "Unchanged"                                                          }, // $B1
{ "OUT .2,CTRL", "",             "IOPORT(CTRL) = .2;"                   , "Write .2 to specially decoded peripheral"         , "B"                                  , "None"                       , "Unchanged"                                                          }, // $B2
{ "OUT .3,CTRL", "",             "IOPORT(CTRL) = .3;"                   , "Write .3 to specially decoded peripheral"         , "B"                                  , "None"                       , "Unchanged"                                                          }, // $B3
{ "TEST .U",     ",#imm",        "compare (.U & imm) against imm;"      , "Test bits of .U"                                  , "INPUT, OUTPUT, IOF, UF1, UF2, STACK", "CC"                         , ".CC = (.U & imm == imm) ? EQ : LT;"                                 }, // $B4
{ "TEST .L",     ",#imm",        "compare (.L & imm) against imm;"      , "Test bits of .L"                                  , "CC, H, B, W, V, LOGICOMP, C"        , "CC"                         , ".CC = (.L & imm == imm) ? EQ : LT;"                                 }, // $B5
{ "--------",    "",             "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $B6
{ "--------",    "",             "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $B7
{ "CALLNE",      " $rel",        "if != gosub rel;"                     , "Call if not equal"                                , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $B8 (relative)
{ "CALLLE",      " $rel",        "if <= gosub rel;"                     , "Call if less than or equal"                       , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $B9 (relative)
{ "CALLGE",      " $rel",        "if >= gosub rel;"                     , "Call if greater than or equal"                    , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $BA (relative)
{ "CALL",        " !zero",       "gosub zero;"                          , "Call always"                                      , "STACK"                              , "STACK"                      , "Unchanged"                                                          }, // $BB (zero-page)
{ "CALLNE",      " /abs",        "if != gosub abs;"                     , "Call if not equal"                                , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $BC (absolute)
{ "CALLLE",      " /abs",        "if <= gosub abs;"                     , "Call if less than or equal"                       , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $BD (absolute)
{ "CALLGE",      " /abs",        "if >= gosub abs;"                     , "Call if greater than or equal"                    , "STACK, CC"                          , "STACK"                      , "Unchanged"                                                          }, // $BE (absolute)
{ "CALL"         " /abs(.3)",    "gosub abs + .3;"                      , "Call always"                                      , "STACK, B"                           , "STACK"                      , "Unchanged"                                                          }, // $BF (indexed)
{ "NOP",         "",             ";"                                    , "No operation"                                     , "None"                               , "None"                       , "Unchanged"                                                          }, // $C0
{ "MOV .1,.0",   "",             ".1 = .0;"                             , "Move .0 into .1"                                  , "B"                                  , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $C1
{ "MOV .2,.0",   "",             ".2 = .0;"                             , "Move .0 into .2"                                  , "B"                                  , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $C2
{ "MOV .3,.0",   "",             ".3 = .0;"                             , "Move .0 into .3"                                  , "B"                                  , "CC"                         , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $C3
{ "--------",    "",             "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $C4
{ "--------",    "",             "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $C5
{ "--------",    "",             "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $C6
{ "--------",    "",             "?"                                    , "-"                                                , "None"                               , "None"                       , "Undefined"                                                          }, // $C7
{ "ST .0",       ",$rel",        "*(rel) = .0;"                         , "Store .0 in *(rel)"                               , "None"                               , "None"                       , "Unchanged"                                                          }, // $C8
{ "ST .1",       ",$rel",        "*(rel) = .1;"                         , "Store .1 in *(rel)"                               , "B"                                  , "None"                       , "Unchanged"                                                          }, // $C9
{ "ST .2",       ",$rel",        "*(rel) = .2;"                         , "Store .2 in *(rel)"                               , "B"                                  , "None"                       , "Unchanged"                                                          }, // $CA
{ "ST .3",       ",$rel",        "*(rel) = .3;"                         , "Store .3 in *(rel)"                               , "B"                                  , "None"                       , "Unchanged"                                                          }, // $CB
{ "ST .0",       ",/abs",        "*(abs) = .0;"                         , "Store .0 in *(abs)"                               , "None"                               , "None"                       , "Unchanged"                                                          }, // $CC
{ "ST .1",       ",/abs",        "*(abs) = .1;"                         , "Store .1 in *(abs)"                               , "B"                                  , "None"                       , "Unchanged"                                                          }, // $CD
{ "ST .2",       ",/abs",        "*(abs) = .2;"                         , "Store .2 in *(abs)"                               , "B"                                  , "None"                       , "Unchanged"                                                          }, // $CE
{ "ST .3",       ",/abs",        "*(abs) = .3;"                         , "Store .3 in *(abs)"                               , "B"                                  , "None"                       , "Unchanged"                                                          }, // $CF
{ "ROL .0",      "",             ".0 <<= 1;"                            , "Rotate .0 left"                                   , "W, C"                               , "H, V, C"                    , "if (.0 & $80) .CC = LT; elif (.0 == 0) .CC = EQ; else .CC = GT;"    }, // $D0 or RLC
{ "ROL .1",      "",             ".1 <<= 1;"                            , "Rotate .1 left"                                   , "B, W, C"                            , "H, V, C"                    , "if (.1 & $80) .CC = LT; elif (.1 == 0) .CC = EQ; else .CC = GT;"    }, // $D1 or RLC
{ "ROL .2",      "",             ".2 <<= 1;"                            , "Rotate .2 left"                                   , "B, W, C"                            , "H, V, C"                    , "if (.2 & $80) .CC = LT; elif (.2 == 0) .CC = EQ; else .CC = GT;"    }, // $D2 or RLC
{ "ROL .3",      "",             ".3 <<= 1;"                            , "Rotate .3 left"                                   , "B, W, C"                            , "H, V, C"                    , "if (.3 & $80) .CC = LT; elif (.3 == 0) .CC = EQ; else .CC = GT;"    }, // $D3 or RLC
{ "OUT .0",      ",port",        "IOPORT(port) = .0;"                   , "Write peripheral port data from .0"               , "None"                               , "None"                       , "Unchanged"                                                          }, // $D4
{ "OUT .1",      ",port",        "IOPORT(port) = .1;"                   , "Write peripheral port data from .1"               , "B"                                  , "None"                       , "Unchanged"                                                          }, // $D5
{ "OUT .2",      ",port",        "IOPORT(port) = .2;"                   , "Write peripheral port data from .2"               , "B"                                  , "None"                       , "Unchanged"                                                          }, // $D6
{ "OUT .3",      ",port",        "IOPORT(port) = .3;"                   , "Write peripheral port data from .3"               , "B"                                  , "None"                       , "Unchanged"                                                          }, // $D7
{ "IBNZ .0",     ",$rel",        "if (++.0 != 0) goto rel;"             , "Increment .0 and jump if not equal to zero"       , "None"                               , "None"                       , "Unchanged"                                                          }, // $D8 or INC .0)
{ "IBNZ .1",     ",$rel",        "if (++.1 != 0) goto rel;"             , "Increment .1 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $D9 or INC .1)
{ "IBNZ .2",     ",$rel",        "if (++.2 != 0) goto rel;"             , "Increment .2 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $DA or INC .2)
{ "IBNZ .3",     ",$rel",        "if (++.3 != 0) goto rel;"             , "Increment .3 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $DB or INC .3)
{ "IBNZ .0",     ",/abs",        "if (++.0 != 0) goto abs;"             , "Increment .0 and jump if not equal to zero"       , "None"                               , "None"                       , "Unchanged"                                                          }, // $DC or INC .0)
{ "IBNZ .1",     ",/abs",        "if (++.1 != 0) goto abs;"             , "Increment .1 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $DD or INC .1)
{ "IBNZ .2",     ",/abs",        "if (++.2 != 0) goto abs;"             , "Increment .2 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $DE or INC .2)
{ "IBNZ .3",     ",/abs",        "if (++.3 != 0) goto abs;"             , "Increment .3 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $DF or INC .3)
{ "COMP .0,.0",  "",             ".CC = EQ;"                            , "Compare .0 against .0"                            , "LOGICOMP"                           , "CC"                         , ".CC = EQ;"                                                          }, // $E0
{ "COMP .0,.1",  "",             "compare .0 against .1;"               , "Compare .0 against .1"                            , "B, LOGICOMP"                        , "CC"                         , "if (.0 > .1) .CC = GT; elif (.0 < .1) .CC = LT; else .CC = EQ;"     }, // $E1
{ "COMP .0,.2",  "",             "compare .0 against .2;"               , "Compare .0 against .2"                            , "B, LOGICOMP"                        , "CC"                         , "if (.0 > .2) .CC = GT; elif (.0 < .2) .CC = LT; else .CC = EQ;"     }, // $E2
{ "COMP .0,.3",  "",             "compare .0 against .3;"               , "Compare .0 against .3"                            , "B, LOGICOMP"                        , "CC"                         , "if (.0 > .3) .CC = GT; elif (.0 < .3) .CC = LT; else .CC = EQ;"     }, // $E3
{ "COMP .0",     ",#imm",        "compare .0 against imm;"              , "Compare .0 against imm"                           , "LOGICOMP"                           , "CC"                         , "if (.0 > imm) .CC = GT; elif (.0 < imm) .CC = LT; else .CC = EQ;"   }, // $E4
{ "COMP .1",     ",#imm",        "compare .1 against imm;"              , "Compare .1 against imm"                           , "B, LOGICOMP"                        , "CC"                         , "if (.1 > imm) .CC = GT; elif (.1 < imm) .CC = LT; else .CC = EQ;"   }, // $E5
{ "COMP .2",     ",#imm",        "compare .2 against imm;"              , "Compare .2 against imm"                           , "B, LOGICOMP"                        , "CC"                         , "if (.2 > imm) .CC = GT; elif (.2 < imm) .CC = LT; else .CC = EQ;"   }, // $E6
{ "COMP .3",     ",#imm",        "compare .3 against imm;"              , "Compare .3 against imm"                           , "B, LOGICOMP"                        , "CC"                         , "if (.3 > imm) .CC = GT; elif (.3 < imm) .CC = LT; else .CC = EQ;"   }, // $E7
{ "COMP .0",     ",$rel",        "compare .0 against *(rel);"           , "Compare .0 against *(rel)"                        , "LOGICOMP"                           , "CC"                         , "if (.0 > *(rel)) .CC = GT; elif (.0 < *(rel)) .CC = LT; else .CC = EQ;" }, // $E8
{ "COMP .1",     ",$rel",        "compare .1 against *(rel);"           , "Compare .1 against *(rel)"                        , "B, LOGICOMP"                        , "CC"                         , "if (.1 > *(rel)) .CC = GT; elif (.1 < *(rel)) .CC = LT; else .CC = EQ;" }, // $E9
{ "COMP .2",     ",$rel",        "compare .2 against *(rel);"           , "Compare .2 against *(rel)"                        , "B, LOGICOMP"                        , "CC"                         , "if (.2 > *(rel)) .CC = GT; elif (.2 < *(rel)) .CC = LT; else .CC = EQ;" }, // $EA
{ "COMP .3",     ",$rel",        "compare .3 against *(rel);"           , "Compare .3 against *(rel)"                        , "B, LOGICOMP"                        , "CC"                         , "if (.3 > *(rel)) .CC = GT; elif (.3 < *(rel)) .CC = LT; else .CC = EQ;" }, // $EB
{ "COMP .0",     ",/abs",        "compare .0 against *(abs);"           , "Compare .0 against *(abs)"                        , "LOGICOMP"                           , "CC"                         , "if (.0 > *(abs)) .CC = GT; elif (.0 < *(abs)) .CC = LT; else .CC = EQ;" }, // $EC
{ "COMP .1",     ",/abs",        "compare .1 against *(abs);"           , "Compare .1 against *(abs)"                        , "B, LOGICOMP"                        , "CC"                         , "if (.1 > *(abs)) .CC = GT; elif (.1 < *(abs)) .CC = LT; else .CC = EQ;" }, // $ED
{ "COMP .2",     ",/abs",        "compare .2 against *(abs);"           , "Compare .2 against *(abs)"                        , "B, LOGICOMP"                        , "CC"                         , "if (.2 > *(abs)) .CC = GT; elif (.2 < *(abs)) .CC = LT; else .CC = EQ;" }, // $EE
{ "COMP .3",     ",/abs",        "compare .3 against *(abs);"           , "Compare .3 against *(abs)"                        , "B, LOGICOMP"                        , "CC"                         , "if (.3 > *(abs)) .CC = GT; elif (.3 < *(abs)) .CC = LT; else .CC = EQ;" }, // $EF
{ "OUT .0,DATA", "",             "IOPORT(DATA) = .0;"                   , "Write .0 to specially decoded peripheral"         , "None"                               , "None"                       , "Unchanged"                                                          }, // $F0
{ "OUT .1,DATA", "",             "IOPORT(DATA) = .1;"                   , "Write .1 to specially decoded peripheral"         , "B"                                  , "None"                       , "Unchanged"                                                          }, // $F1
{ "OUT .2,DATA", "",             "IOPORT(DATA) = .2;"                   , "Write .2 to specially decoded peripheral"         , "B"                                  , "None"                       , "Unchanged"                                                          }, // $F2
{ "OUT .3,DATA", "",             "IOPORT(DATA) = .3;"                   , "Write .3 to specially decoded peripheral"         , "B"                                  , "None"                       , "Unchanged"                                                          }, // $F3
{ "TEST .0",     ",#imm",        ".CC = (.0 & imm == imm) ? EQ : LT;"   , "Test bits imm of .0"                              , "None"                               , "CC"                         , "CC = (.0 & imm == imm) ? EQ : LT;"                                  }, // $F4
{ "TEST .1",     ",#imm",        ".CC = (.1 & imm == imm) ? EQ : LT;"   , "Test bits imm of .1"                              , "B"                                  , "CC"                         , "CC = (.1 & imm == imm) ? EQ : LT;"                                  }, // $F5
{ "TEST .2",     ",#imm",        ".CC = (.2 & imm == imm) ? EQ : LT;"   , "Test bits imm of .2"                              , "B"                                  , "CC"                         , "CC = (.2 & imm == imm) ? EQ : LT;"                                  }, // $F6
{ "TEST .3",     ",#imm",        ".CC = (.3 & imm == imm) ? EQ : LT;"   , "Test bits imm of .3"                              , "B"                                  , "CC"                         , "CC = (.3 & imm == imm) ? EQ : LT;"                                  }, // $F7
{ "DBNZ .0",     ",$rel",        "if (--.0 != 0) goto rel;"             , "Decrement .0 and jump if not equal to zero"       , "None"                               , "None"                       , "Unchanged"                                                          }, // $F8 or DEC .0
{ "DBNZ .1",     ",$rel",        "if (--.1 != 0) goto rel;"             , "Decrement .1 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $F9 or DEC .1
{ "DBNZ .2",     ",$rel",        "if (--.2 != 0) goto rel;"             , "Decrement .2 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $FA or DEC .2
{ "DBNZ .3",     ",$rel",        "if (--.3 != 0) goto rel;"             , "Decrement .3 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $FB or DEC .3
{ "DBNZ .0",     ",/abs",        "if (--.0 != 0) goto abs;"             , "Decrement .0 and jump if not equal to zero"       , "None"                               , "None"                       , "Unchanged"                                                          }, // $FC or DEC .0
{ "DBNZ .1",     ",/abs",        "if (--.1 != 0) goto abs;"             , "Decrement .1 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $FD or DEC .1
{ "DBNZ .2",     ",/abs",        "if (--.2 != 0) goto abs;"             , "Decrement .2 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $FE or DEC .2
{ "DBNZ .3",     ",/abs",        "if (--.3 != 0) goto abs;"             , "Decrement .3 and jump if not equal to zero"       , "B"                                  , "None"                       , "Unchanged"                                                          }, // $FF or DEC .3
} };

EXPORT const STRPTR opcodelink[256] = {
"lodz",
"lodz",
"lodz",
"lodz",
"lodi",
"lodi",
"lodi",
"lodi",
"lodr",
"lodr",
"lodr",
"lodr",
"loda",
"loda",
"loda",
"loda",
"ldpl",
"stpl",
"spsu",
"spsl",
"retc",
"retc",
"retc",
"retc",
"bctr",
"bctr",
"bctr",
"bctr",
"bcta",
"bcta",
"bcta",
"bcta",
"eorz",
"eorz",
"eorz",
"eorz",
"eori",
"eori",
"eori",
"eori",
"eorr",
"eorr",
"eorr",
"eorr",
"eora",
"eora",
"eora",
"eora",
"redc",
"redc",
"redc",
"redc",
"rete",
"rete",
"rete",
"rete",
"bstr",
"bstr",
"bstr",
"bstr",
"bsta",
"bsta",
"bsta",
"bsta",
"halt",
"andz",
"andz",
"andz",
"andi",
"andi",
"andi",
"andi",
"andr",
"andr",
"andr",
"andr",
"anda",
"anda",
"anda",
"anda",
"rrr",
"rrr",
"rrr",
"rrr",
"rede",
"rede",
"rede",
"rede",
"brnr",
"brnr",
"brnr",
"brnr",
"brna",
"brna",
"brna",
"brna",
"iorz",
"iorz",
"iorz",
"iorz",
"iori",
"iori",
"iori",
"iori",
"iorr",
"iorr",
"iorr",
"iorr",
"iora",
"iora",
"iora",
"iora",
"redd",
"redd",
"redd",
"redd",
"cpsu",
"cpsl",
"ppsu",
"ppsl",
"bsnr",
"bsnr",
"bsnr",
"bsnr",
"bsna",
"bsna",
"bsna",
"bsna",
"addz",
"addz",
"addz",
"addz",
"addi",
"addi",
"addi",
"addi",
"addr",
"addr",
"addr",
"addr",
"adda",
"adda",
"adda",
"adda",
"",
"",
"lpsu",
"lpsl",
"dar",
"dar",
"dar",
"dar",
"bcfr",
"bcfr",
"bcfr",
"zbrr",
"bcfa",
"bcfa",
"bcfa",
"bxa",
"subz",
"subz",
"subz",
"subz",
"subi",
"subi",
"subi",
"subi",
"subr",
"subr",
"subr",
"subr",
"suba",
"suba",
"suba",
"suba",
"wrtc",
"wrtc",
"wrtc",
"wrtc",
"tpsu",
"tpsl",
"",
"",
"bsfr",
"bsfr",
"bsfr",
"zbsr",
"bsfa",
"bsfa",
"bsfa",
"bsxa",
"nop",
"strz",
"strz",
"strz",
"",
"",
"",
"",
"strr",
"strr",
"strr",
"strr",
"stra",
"stra",
"stra",
"stra",
"rrl",
"rrl",
"rrl",
"rrl",
"wrte",
"wrte",
"wrte",
"wrte",
"birr",
"birr",
"birr",
"birr",
"bira",
"bira",
"bira",
"bira",
"comz",
"comz",
"comz",
"comz",
"comi",
"comi",
"comi",
"comi",
"comr",
"comr",
"comr",
"comr",
"coma",
"coma",
"coma",
"coma",
"wrtd",
"wrtd",
"wrtd",
"wrtd",
"tmi",
"tmi",
"tmi",
"tmi",
"bdrr",
"bdrr",
"bdrr",
"bdrr",
"bdra",
"bdra",
"bdra",
"bdra",
};

// 4. IMPORTED VARIABLES--------------------------------------------------

IMPORT       FLAG  forcedollar,
                   hurry;
IMPORT       TEXT  asciiname_short[259][3 + 1],
                   datetimestring[40 + 1],
                   friendly[FRIENDLYLENGTH + 1],
                   v_bin[3 + 8 + 1];
IMPORT       UBYTE memory[32768],
                   opcode,
                   psl,
                   psu,
                   startaddr_h, startaddr_l,
                   rr, // register (0..6)
                   r[7]; // register is a reserved word
IMPORT       UWORD iar,
                   mirror_r[32768],
                   mirror_w[32768];
IMPORT       ULONG cycles_2650;
IMPORT       char  v[5 + 1]; // enough for "16'AB"
IMPORT       int   addr,
                   firstdoscodecomment, lastdoscodecomment,
                   firstdosdatacomment, lastdosdatacomment,
                   firstdosequiv,       lastdosequiv,
                   foundequiv,
                   machine,
                   memmap,
                   phunsy_biosver,
                   ppc,
                   pvibase,
                   selbst_biosver,
                   style,
                   supercpu,
                   table_size_2650[256],
                   timeunit,
                   userlabels,
                   verbosity,
                   whichgame;
IMPORT       MEMFLAG                  memflags[ALLTOKENS];
IMPORT       struct EquivalentStruct  equivalents[],
                                      symlabel[SYMLABELS_MAX + 1];
IMPORT       struct HostMachineStruct hostmachines[MACHINES];
IMPORT       struct KindStruct        filekind[KINDS];
IMPORT       struct MachineStruct     machines[MACHINES];
IMPORT       struct MemMapInfoStruct  memmapinfo[MEMMAPS];
IMPORT const int                      relative_2650[0x100];
IMPORT const STRPTR                   ccstring[STYLES][4];
IMPORT const struct CodeCommentStruct codecomment[];
IMPORT const struct DataCommentStruct datacomment[];
IMPORT const struct KnownStruct       known[KNOWNGAMES];

#ifdef AMIGA
    IMPORT   struct Catalog*          CatalogPtr;
#endif
#ifdef WIN32
    IMPORT   int                      CatalogPtr;
#endif

// 5. MODULE VARIABLES----------------------------------------------------

MODULE UWORD disflag[32768],    // seems to cause a crash on WinArcadia...
             threadlist[32768]; // ...if these are PERSISTent to disassemble_2650()
MODULE ULONG tea         = OUTOFRANGE; // trace effective address
MODULE char  addressstring[80 + 1], // "*SPRITECOLLIDE,r3+ [SPRITECOLLIDE]" is no longer the largest possible
             an[128 + 1], // [80 + 1] is probably still enough
             byte1[2 + 1],
             byte2[2 + 1],
             byte3[2 + 1],
             en[512 + 1],
             mn2[256 + 1],
             pseudostring[80 + 1],
             reg[2 + 1],
             v_pse[3 + 1], // enough for "$12"
             tn[80 + 1];
MODULE int   ccmode      = CCMODE_UNDEFINED,
             duration;

MODULE const STRPTR cctrue[STYLES][4] = {
{ "if CC == EQ then ", // was "if == then "
  "if CC == GT then ", // was "if > then "
  "if CC == LT then ", // was "if < then "
  ""
},
{ "if CC == EQ then ", // was "if == then "
  "if CC == GT then ", // was "if > then "
  "if CC == LT then ", // was "if < then "
  ""
},
{ "if CC == EQ then ", // was "if == then "
  "if CC == GT then ", // was "if > then "
  "if CC == LT then ", // was "if < then "
  ""
},
{ "if CC == EQ then ",
  "if CC == GT then ",
  "if CC == LT then ",
  ""
},
{ "if .CC == EQ then ", // was "if == then "
  "if .CC == GT then ", // was "if > then "
  "if .CC == LT then ", // was "if < then "
  ""
} }, ccfalse[STYLES][4] = {
{ "if CC != EQ then ", // was "if != then "
  "if CC != GT then ", // was "if <= then "
  "if CC != LT then ", // was "if >= then "
  "if 0 then "
},
{ "if CC != EQ then ", // was "if != then "
  "if CC != GT then ", // was "if <= then "
  "if CC != LT then ", // was "if >= then "
  "if 0 then "
},
{ "if CC != EQ then ", // was "if != then "
  "if CC != GT then ", // was "if <= then "
  "if CC != LT then ", // was "if >= then "
  "if 0 then "
},
{ "if CC != EQ then ",
  "if CC != GT then ",
  "if CC != LT then ",
  "if 0 then "
},
{ "if .CC != EQ then ", // was "if != then "
  "if .CC != GT then ", // was "if <= then "
  "if .CC != LT then ", // was "if >= then "
  "if 0 then "
} };

/* 6. STRUCTURES----------------------------------------------------------

(None)

7. MODULE FUNCTIONS---------------------------------------------------- */

MODULE FLAG do_data_comment(int whichcomment, int whichaddr, int mode, FLAG first, STRPTR stringptr);
MODULE void extracomment(int whichcomment, int address, STRPTR stringptr);
MODULE void show_code_comment(int whichaddr);
MODULE void show_comments(int whichaddr);
MODULE void ABS_EA_TRACE(void);
MODULE void REL_EA_TRACE(void);
MODULE void BRA_EA_TRACE(FLAG full);
MODULE void ZERO_EA_TRACE(FLAG full);

// 8. CODE----------------------------------------------------------------

EXPORT void tracecpu_2650(FLAG full, FLAG quiet)
{   FAST FLAG maybe,
              started;
    FAST int  i,
              length;

    OPERAND = memory[WRAPMEM(1)];

    if (full)
    {   if (verbosity && !quiet)
        {   forcedollar = TRUE;
            if (number_to_friendly(iar, (STRPTR) friendly, FALSE, 0) != EOS)
            {   zprintf(TEXTPEN_LABEL, "%s: ;$%X\n", friendly, iar);
            }
            forcedollar = FALSE;
    }   }
    else
    {   opcode = memory[iar]; // needed!
    }

    GET_RR;
    switch (style)
    {
    case STYLE_OLDCALM:
        if (rr >= 4)
        {   reg[0] = 'B' + rr - 4;
            reg[1] = QUOTE;
            reg[2] = EOS;
        } else
        {   reg[0] = 'A' + rr;
            reg[1] = EOS;
        }
    acase STYLE_IEEE:
        reg[0] = '.';
        reg[1] = '0' + rr;
        reg[2] = EOS;
    adefault:
        reg[0] = 'r';
        reg[1] = '0' + rr;
        reg[2] = EOS;
    }

    if (verbosity == 1)
    {   sprintf(tn, "%04X %02X", iar, opcode);

        if (table_size_2650[opcode] == 3)
        {   sprintf(ENDOF(tn), "%02X%02X ", OPERAND, memory[WRAPMEM(2)]);
        } elif (table_size_2650[opcode] == 2)
        {   sprintf(ENDOF(tn), "%02X   ", OPERAND); // or "%02X.. "
        } else
        {   // assert(table_size_2650[opcode] == 1);
            strcat(tn, "     "); // or ".... "
        }

        if ((opcode & 0x1F) >= 0x0C && (opcode & 0x1F) <= 0x0F && (OPERAND & 0x60)) // indexed absolute non-branch
        {   if   (opcode <= 0x0F) strcat(tn, "LOD");
            elif (opcode <= 0x2F) strcat(tn, "EOR");
            elif (opcode <= 0x4F) strcat(tn, "AND");
            elif (opcode <= 0x6F) strcat(tn, "IOR");
            elif (opcode <= 0x8F) strcat(tn, "ADD");
            elif (opcode <= 0xAF) strcat(tn, "SUB");
            elif (opcode <= 0xCF) strcat(tn, "STR");
            else                  strcat(tn, "COM"); // $EC..$EF
            sprintf(ENDOF(tn), "A,r0 r%d", opcode & 3);
            switch (OPERAND & 0x60)
            {
            case  0x20: strcat(tn, "+");
            acase 0x40: strcat(tn, "-");
            adefault:   strcat(tn, ".");
            }
        } else
        {   sprintf(ENDOF(tn), "%-7s    ", opcodes[style][opcode].name); // or "%-7s ..."
        }

        if     ((opcode & 0x1F) >= 0x1C) // absolute branch
        {   BRA_EA_TRACE(TRUE);
        } elif ((opcode & 0x0F) >= 0x0C) // absolute non-branch
        {   ABS_EA_TRACE();
        } elif ((opcode & 0x0F) >= 0x08) // relative
        {   REL_EA_TRACE();
        }

        if ((opcode & 0x0F) >= 8 && (OPERAND & 0x80)) // indirect
        {   sprintf(ENDOF(tn), " *%04X", addr);
        } elif ((opcode & 0x1F) >= 0x1C) // absolute branch
        {   sprintf(ENDOF(tn), "  %04X", (unsigned int) tea);
        } elif ((opcode & 0x0F) >= 0x0C) // absolute non-branch
        {   sprintf(ENDOF(tn), "  %04X", addr);
        } elif ((opcode & 0x0F) >= 0x08) // relative
        {   sprintf(ENDOF(tn), "  %04X", (unsigned int) tea);
        } else
        {   strcat(tn, "      "); // or " ....."
        }

        if ((opcode & 0x1F) >= 0x0C && (opcode & 0x1F) <= 0x0F && (OPERAND & 0x60)) // indexed absolute (non-branch)
        {   sprintf(ENDOF(tn), "+%02X", r[rr]);
        } else
        {   strcat(tn, "   "); // or " .."
        }

        if ((opcode & 0x0F) >= 0x08)
        {   sprintf(ENDOF(tn), "=%04X", (unsigned int) tea);
        } else
        {   strcat(tn, "     "); // or " ...."
    }   }
    else
    {   en[0] = EOS;
        mn[8] = EOS;
    }

    switch (opcode)
    {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
        switch (style)
        {
        case  STYLE_OLDCALM:  sprintf(ENDOF(mn), "LOAD     A,%s"  , reg   );               sprintf(an, "A = %s;" , reg);
        acase STYLE_NEWCALM:  sprintf(ENDOF(mn), "MOVE     %s,r0" , reg   );               sprintf(an, "r0 = %s;", reg);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "MOV    .0,%s"   , reg   );               sprintf(an, ".0 = %s;", reg);
        adefault:             sprintf(ENDOF(mn), "LODZ    %s"     , reg   );               sprintf(an, "r0 = %s;", reg);
        }
        ccmode = CCMODE_ARITHMETIC;
    acase 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
        dec_to_hex(OPERAND);
        switch (style)
        {
        case  STYLE_OLDCALM:  sprintf(ENDOF(mn), "LOAD     %s,#%s", reg, v);
        acase STYLE_NEWCALM:  sprintf(ENDOF(mn), "MOVE     #%s,%s", v, reg);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "LD     %s,#%s"  , reg, v);
        adefault:             sprintf(ENDOF(mn), "LODI,%s %s"     , reg, v);
        }
                                                                                           sprintf(an, "%s = %s", reg, v_pse);
                                                                                           if (ISQWERTY)
                                                                                           {   sprintf(ENDOF(an), " [%s];", asciiname_short[OPERAND]);
                                                                                           } elif (machine == INTERTON)
                                                                                           {   strcat(an, ";");
                                                                                           } else
                                                                                           {   sprintf(ENDOF(an), " ['%c'];", guestchar(OPERAND));
                                                                                           }
        ccmode = CCMODE_ARITHMETIC;
    acase 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM:  sprintf(ENDOF(mn), "LOAD     %s,%s", reg, addressstring);
        acase STYLE_NEWCALM:  sprintf(ENDOF(mn), "MOVE     %s,%s", addressstring, reg);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "LD     %s,%s"  , reg, addressstring);
        adefault:             sprintf(ENDOF(mn), "LODR,%s %s"    , reg, addressstring);
        }
                                                                                           sprintf(an, "%s = *(%s);", reg, pseudostring);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x0C:
    case 0x0D:
    case 0x0E:
    case 0x0F:
        ABS_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM:  sprintf(ENDOF(mn), "LOAD     %s,%s", reg, addressstring);
        acase STYLE_NEWCALM:  sprintf(ENDOF(mn), "MOVE     %s,%s", addressstring, reg);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "LD     %s,%s"  , reg, addressstring);
        adefault:             sprintf(ENDOF(mn), "LODA,%s %s"    , reg, addressstring);
        }
                                                                                           sprintf(an, "%s = *(%s);", reg, pseudostring);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x10:
        if (supercpu)
        {   BRA_EA_TRACE(TRUE);
            switch (style)
            {
            case  STYLE_OLDCALM: sprintf(ENDOF(mn), "LOAD     L,%s", addressstring);       sprintf(an, "L = *(%s);"  , pseudostring);
            acase STYLE_NEWCALM: sprintf(ENDOF(mn), "MOVE     %s,PSL", addressstring);     sprintf(an, "PSL = *(%s);", pseudostring);
            acase STYLE_IEEE:    sprintf(ENDOF(mn), "LD     .L,%s" , addressstring);       sprintf(an, ".L = *(%s);" , pseudostring);
            adefault:            sprintf(ENDOF(mn), "LDPL    %s"   , addressstring);       sprintf(an, "PSL = *(%s);", pseudostring);
            }

            if (!disassembling)
            {   switch (memory[tea] & 0xC0)
                {
                case  0xC0: strcat(en, ";CC (Condition Code) is now UN (%11)!\n");
                acase 0x80: strcat(en, ";CC (Condition Code) is now LT (%10)\n");
                acase 0x40: strcat(en, ";CC (Condition Code) is now GT (%01)\n");
                acase 0x00: strcat(en, ";CC (Condition Code) is now EQ (%00)\n");
                }
                sprintf(ENDOF(en), ";%s %s (%s) bit\n",                   (memory[tea] & 0x20) ? "Set" : "Clear", flagname[NAME_IDC].shorter[style], flagname[NAME_IDC].longer[style]);
                sprintf(ENDOF(en), ";%s %s (%s) bit (now %s..%s)",        (memory[tea] & 0x10) ? "Set" : "Clear", flagname[NAME_RS ].shorter[style], flagname[NAME_RS ].longer[style],
                                                                          (memory[tea] & 0x10) ?                  flagname[NAME_R4 ].shorter[style] : flagname[NAME_R1].shorter[style],
                                                                          (memory[tea] & 0x10) ?                  flagname[NAME_R6 ].shorter[style] : flagname[NAME_R3].shorter[style]);
                sprintf(ENDOF(en), ";%s %s (%s) bit\n",                   (memory[tea] &  0x8) ? "Set" : "Clear", flagname[NAME_WC ].shorter[style], flagname[NAME_WC ].longer[style]);
                sprintf(ENDOF(en), ";%s %s (%s) bit\n",                   (memory[tea] &  0x4) ? "Set" : "Clear", flagname[NAME_OVF].shorter[style], flagname[NAME_OVF].longer[style]);
                sprintf(ENDOF(en), ";%s %s (%s) bit (now %ssigned)\n",    (memory[tea] &  0x2) ? "Set" : "Clear", flagname[NAME_COM].shorter[style], flagname[NAME_COM].longer[style],
                                                                          (memory[tea] &  0x2) ? "un"  : "");
                sprintf(ENDOF(en), ";%s %s (%s) bit\n",                   (memory[tea] &  0x1) ? "Set" : "Clear", flagname[NAME_C  ].shorter[style], flagname[NAME_C  ].longer[style]);
        }   }
        else
        {   switch (style)
            {
            case  STYLE_OLDCALM:
            case  STYLE_NEWCALM: sprintf(ENDOF(mn), "DB       16'%02X", opcode);
            acase STYLE_IEEE:    sprintf(ENDOF(mn), "DATA   $%02X"    , opcode);
            adefault:            sprintf(ENDOF(mn), "DB      $%02X"   , opcode);
            }                                                                              strcpy( an, "?");
        }
        ccmode = CCMODE_UNDEFINED;
    acase 0x11:
        if (supercpu)
        {   BRA_EA_TRACE(TRUE);
            switch (style)
            {
            case  STYLE_OLDCALM: sprintf(ENDOF(mn), "LOAD     %s,L", addressstring);       sprintf(an, "*(%s) = L;"  , pseudostring);
            acase STYLE_NEWCALM: sprintf(ENDOF(mn), "MOVE     PSL,%s", addressstring);     sprintf(an, "*(%s) = PSL;", pseudostring);
            acase STYLE_IEEE:    sprintf(ENDOF(mn), "ST     .L,%s" , addressstring);       sprintf(an, "*(%s) = .L;" , pseudostring);
            adefault:            sprintf(ENDOF(mn), "STPL    %s"   , addressstring);       sprintf(an, "*(%s) = PSL;", pseudostring);
            }

            if (!disassembling)
            {   switch (psl & 0xC0)
                {
                case  0xC0: strcat(en, ";CC (Condition Code) is UN (%11)!\n");
                acase 0x80: strcat(en, ";CC (Condition Code) is LT\n");
                acase 0x40: strcat(en, ";CC (Condition Code) is GT\n");
                acase 0x00: strcat(en, ";CC (Condition Code) is EQ\n");
                }
                sprintf(    ENDOF(en), ";%s (%s) bit is %s\n",          flagname[NAME_IDC].shorter[style], flagname[NAME_IDC].longer[style], (psl & 0x20) ? "set"            : "clear"         );
                sprintf(    ENDOF(en), ";%s (%s) bit is %s (%s..%s)\n", flagname[NAME_RS ].shorter[style], flagname[NAME_RS ].longer[style], (psl & 0x10) ? "set"            : "clear"         ,
                                                                        (psl & 0x10) ? flagname[NAME_R4].shorter[style] : flagname[NAME_R1].shorter[style],
                                                                        (psl & 0x10) ? flagname[NAME_R6].shorter[style] : flagname[NAME_R3].shorter[style]);
                sprintf(    ENDOF(en), ";%s (%s) bit is %s\n",          flagname[NAME_WC ].shorter[style], flagname[NAME_WC ].longer[style], (psl &  0x8) ? "set"            : "clear"         );
                sprintf(    ENDOF(en), ";%s (%s) bit is %s\n",          flagname[NAME_OVF].shorter[style], flagname[NAME_OVF].longer[style], (psl &  0x4) ? "set"            : "clear"         );
                sprintf(    ENDOF(en), ";%s (%s) bit is %s\n",          flagname[NAME_COM].shorter[style], flagname[NAME_COM].longer[style], (psl &  0x2) ? "set (unsigned)" : "clear (signed)");
                sprintf(    ENDOF(en), ";%s (%s) bit is %s\n",          flagname[NAME_C  ].shorter[style], flagname[NAME_C  ].longer[style], (psl &  0x1) ? "set"            : "clear"         );
        }   }
        else
        {   switch (style)
            {
            case  STYLE_OLDCALM:
            case  STYLE_NEWCALM: sprintf(ENDOF(mn), "DB       16'%02X", opcode);
            acase STYLE_IEEE:    sprintf(ENDOF(mn), "DATA   $%02X"    , opcode);
            adefault:            sprintf(ENDOF(mn), "DB      $%02X"   , opcode);
            }                                                                              strcpy( an, "?");
        }
        ccmode = CCMODE_UNDEFINED;
    acase 0x12:
        switch (style)
        {
        case  STYLE_OLDCALM: strcat(mn, "LOAD     A,U");                                   strcpy(an, "A = U;");
        acase STYLE_NEWCALM: strcat(mn, "MOVE     PSU,r0");                                strcpy(an, "r0 = PSU;");
        acase STYLE_IEEE:    strcat(mn, "MOV    .0,.U");                                   strcpy(an, ".0 = .U;");
        adefault:            strcat(mn, "SPSU"); /* or "SPSU    r0" */                     strcpy(an, "r0 = PSU;");
        }

        if (!disassembling)
        {   sprintf(    ENDOF(en), ";%s (%s) bit is %s\n", flagname[NAME_S ].shorter[style], flagname[NAME_S ].longer[style], (psu & 0x80) ? "set"             : "clear"              );
            sprintf(    ENDOF(en), ";%s (%s) bit is %s\n", flagname[NAME_F ].shorter[style], flagname[NAME_F ].longer[style], (psu & 0x40) ? "set"             : "clear"              );
            sprintf(    ENDOF(en), ";%s (%s) bit is %s\n", flagname[NAME_II].shorter[style], flagname[NAME_II].longer[style], (psu & 0x20) ? "set (inhibited)" : "clear (uninhibited)");
            if (supercpu)
            {   sprintf(ENDOF(en), ";User Flag #1 is %s\n", (psu & 0x10) ? "set" : "clear");
                sprintf(ENDOF(en), ";User Flag #2 is %s\n", (psu &  0x8) ? "set" : "clear");
            }
            sprintf(    ENDOF(en), ";%s (%s) is %d\n",     flagname[NAME_SP].shorter[style], flagname[NAME_SP].longer[style], (int) (psu & 0x7));
        }
        ccmode = CCMODE_ARITHMETIC; // arguably
    acase 0x13:
        switch (style)
        {
        case  STYLE_OLDCALM: strcat(mn, "LOAD     A,L");                                   strcpy(an, "A = L;");
        acase STYLE_NEWCALM: strcat(mn, "MOVE     PSL,r0");                                strcpy(an, "r0 = PSL;");
        acase STYLE_IEEE:    strcat(mn, "MOV    .0,.L");                                   strcpy(an, ".0 = .L;");
        adefault:            strcat(mn, "SPSL"); /* or "SPSL    r0" */                     strcpy(an, "r0 = PSL;");
        }

        if (!disassembling)
        {   switch (psl & 0xC0)
            {
            case  0xC0: strcat(en, ";CC (Condition Code) is UN (%11)!\n");
            acase 0x80: strcat(en, ";CC (Condition Code) is LT\n");
            acase 0x40: strcat(en, ";CC (Condition Code) is GT\n");
            acase 0x00: strcat(en, ";CC (Condition Code) is EQ\n");
            }
            sprintf(ENDOF(en), ";%s (%s) bit is %s\n",          flagname[NAME_IDC].shorter[style], flagname[NAME_IDC].longer[style], (psl & 0x20) ? "set"             : "clear"         );
            sprintf(ENDOF(en), ";%s (%s) bit is %s (%s..%s)\n", flagname[NAME_RS ].shorter[style], flagname[NAME_RS ].longer[style], (psl & 0x10) ? "set"             : "clear"         ,
                                                                (psl & 0x10) ? flagname[NAME_R4].shorter[style] : flagname[NAME_R1].shorter[style],
                                                                (psl & 0x10) ? flagname[NAME_R6].shorter[style] : flagname[NAME_R3].shorter[style]);
            sprintf(ENDOF(en), ";%s (%s) bit is %s\n",          flagname[NAME_WC ].shorter[style], flagname[NAME_WC ].longer[style], (psl &  0x8) ? "set"             : "clear"         );
            sprintf(ENDOF(en), ";%s (%s) bit is %s\n",          flagname[NAME_OVF].shorter[style], flagname[NAME_OVF].longer[style], (psl &  0x4) ? "set"             : "clear"         );
            sprintf(ENDOF(en), ";%s (%s) bit is %s\n",          flagname[NAME_COM].shorter[style], flagname[NAME_COM].longer[style], (psl &  0x2) ? "set (unsigned)"  : "clear (signed)");
            sprintf(ENDOF(en), ";%s (%s) bit is %s\n",          flagname[NAME_C  ].shorter[style], flagname[NAME_C  ].longer[style], (psl &  0x1) ? "set"             : "clear"         );
        }
        ccmode = CCMODE_ARITHMETIC; // arguably
    acase 0x14:
        switch (style)
        {
        case STYLE_OLDCALM:
            if (ccmode == CCMODE_TEST)
            {   strcat(mn, "RET,AO");
            } else
            {   sprintf(ENDOF(mn), "RET%s", ccstring[style][BRANCHCODE]);
            }
        acase STYLE_NEWCALM:
        case STYLE_IEEE:
            sprintf(ENDOF(mn), "RET%s"  , ccstring[style][BRANCHCODE]);
        adefault:
            sprintf(ENDOF(mn), "RETC,%s", ccstring[style][BRANCHCODE]);
        }
                                                                                       sprintf(an, "%sreturn;", cctrue[style][BRANCHCODE]);
    acase 0x15:
        switch (style)
        {
        case STYLE_OLDCALM:
        case STYLE_NEWCALM:
        case STYLE_IEEE:
            sprintf(ENDOF(mn), "RET%s"  , ccstring[style][BRANCHCODE]);
        adefault:
            sprintf(ENDOF(mn), "RETC,%s", ccstring[style][BRANCHCODE]);
        }
                                                                                       sprintf(an, "%sreturn;", cctrue[style][BRANCHCODE]);
    acase 0x16:
        switch (style)
        {
        case STYLE_OLDCALM:
            if (ccmode == CCMODE_TEST)
            {   strcat(mn, "RET,NO");
            } else
            {   sprintf(ENDOF(mn), "RET%s"  , ccstring[style][BRANCHCODE]);
            }
        acase STYLE_NEWCALM:
        case STYLE_IEEE:
            sprintf(ENDOF(mn), "RET%s"  , ccstring[style][BRANCHCODE]);
        adefault:
            sprintf(ENDOF(mn), "RETC,%s", ccstring[style][BRANCHCODE]);
        }
                                                                                       sprintf(an, "%sreturn;", cctrue[style][BRANCHCODE]);
    acase 0x17:
        if (style == STYLE_SIGNETICS1)
        {   strcat(mn, "RETC,un");
        } else
        {   strcat(mn, "RET");
        }
                                                                                       strcpy(an, "return;");
    acase 0x18:
        REL_EA_TRACE();
        switch (style)
        {
        acase STYLE_SIGNETICS1:
            strcat(mn,                          "BCTR,eq ");
        acase STYLE_SIGNETICS2:
            switch (ccmode)
            {
            case  CCMODE_UNDEFINED:  strcat(mn, "BCTR,eq ");
            acase CCMODE_COMPARE:    strcat(mn, "BER     "); // Branch if Equal, Relative
            acase CCMODE_ARITHMETIC: strcat(mn, "BZR     "); // Branch if Zero, Relative
            acase CCMODE_TEST:       strcat(mn, "BOR     "); // Branch if Ones, Relative
            }
        acase STYLE_OLDCALM:
            if (ccmode == CCMODE_TEST)
            {   strcat(mn,                      "JUMP,AO  ");
            } else
            {   strcat(mn,                      "JUMP,EQ  ");
            }
        acase STYLE_NEWCALM:
            strcat(mn,                          "JUMP,EQ  ");
        acase STYLE_IEEE:
            strcat(mn,                          "BEQ    ");
        }
        strcat(mn, addressstring);
                                                                                       sprintf(an, "%sgoto %s;", cctrue[style][BRANCHCODE], pseudostring);
    acase 0x19:
        REL_EA_TRACE();
        switch (style)
        {
        acase STYLE_SIGNETICS1:
            strcat(mn,                          "BCTR,gt ");
        acase STYLE_SIGNETICS2:
            switch (ccmode)
            {
            case  CCMODE_UNDEFINED:  strcat(mn, "BCTR,gt ");
            acase CCMODE_COMPARE:    strcat(mn, "BHR     "); // Branch if Higher, Relative
            acase CCMODE_ARITHMETIC: strcat(mn, "BPR     "); // Branch if Positive, Relative
            acase CCMODE_TEST:       strcat(mn, "BCTR,gt "); // we should warn the user about this!
            }
        acase STYLE_OLDCALM:
        case STYLE_NEWCALM:
            strcat(mn,                          "JUMP,GT  ");
        acase STYLE_IEEE:
            strcat(mn,                          "BGT    ");
        }
        strcat(mn, addressstring);
                                                                                       sprintf(an, "%sgoto %s;", cctrue[style][BRANCHCODE], pseudostring);
    acase 0x1A:
        REL_EA_TRACE();
        switch (style)
        {
        acase STYLE_SIGNETICS1:
            strcat(mn,                          "BCTR,lt ");
        acase STYLE_SIGNETICS2:
            switch (ccmode)
            {
            case  CCMODE_UNDEFINED:  strcat(mn, "BCTR,lt ");
            acase CCMODE_COMPARE:    strcat(mn, "BLR     "); // Branch if Lower, Relative
            acase CCMODE_ARITHMETIC: strcat(mn, "BMR     "); // Branch if Minus, Relative
            acase CCMODE_TEST:       strcat(mn, "BMR     "); // Branch if Mixed, Relative
            }
        acase STYLE_OLDCALM:
            if (ccmode == CCMODE_TEST)
            {   strcat(mn,                      "JUMP,NO  ");
            } else
            {   strcat(mn,                      "JUMP,LT  ");
            }
        acase STYLE_NEWCALM:
            {   strcat(mn,                      "JUMP,LT  ");
            }
        acase STYLE_IEEE:
            strcat(mn,                          "BLT    ");
        }
        strcat(mn, addressstring);
                                                                                       sprintf(an, "%sgoto %s;", cctrue[style][BRANCHCODE], pseudostring);
    acase 0x1B:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_SIGNETICS1:      strcat(mn, "BCTR,un ");
        acase STYLE_SIGNETICS2:      strcat(mn, "BR      ");
        acase STYLE_OLDCALM:
        case  STYLE_NEWCALM:         strcat(mn, "JUMP     ");
        acase STYLE_IEEE:            strcat(mn, "BR     ");
        }
        strcat(mn, addressstring);
                                                                                       sprintf(an, "goto %s;", pseudostring);
    acase 0x1C:
        BRA_EA_TRACE(TRUE);
        switch (style)
        {
        case STYLE_SIGNETICS1:
            strcat(mn,                          "BCTA,eq ");
        acase STYLE_SIGNETICS2:
            switch (ccmode)
            {
            case  CCMODE_UNDEFINED:  strcat(mn, "BCTA,eq ");
            acase CCMODE_COMPARE:    strcat(mn, "BEA     "); // Branch if Equal, Absolute
            acase CCMODE_ARITHMETIC: strcat(mn, "BZA     "); // Branch if Zero, Absolute
            acase CCMODE_TEST:       strcat(mn, "BOA     "); // Branch if Ones, Absolute
            }
        acase STYLE_OLDCALM:
            if (ccmode == CCMODE_TEST)
            {   strcat(mn,                      "JUMP,AO  ");
            } else
            {   strcat(mn,                      "JUMP,EQ  ");
            }
        acase STYLE_NEWCALM:
            strcat(mn,                          "JUMP,EQ  ");
        acase STYLE_IEEE:
            strcat(mn,                          "BEQ    ");
        }
        strcat(mn, addressstring);
                                                                                       sprintf(an, "%sgoto %s;", cctrue[style][BRANCHCODE], pseudostring);
    acase 0x1D:
        BRA_EA_TRACE(TRUE);
        switch (style)
        {
        case STYLE_SIGNETICS1:
            strcat(mn,                          "BCTA,gt ");
        acase STYLE_SIGNETICS2:
            switch (ccmode)
            {
            case  CCMODE_UNDEFINED:  strcat(mn, "BCTA,gt ");
            acase CCMODE_COMPARE:    strcat(mn, "BHA     "); // Branch if Higher, Absolute
            acase CCMODE_ARITHMETIC: strcat(mn, "BPA     "); // Branch if Positive, Absolute
            acase CCMODE_TEST:       strcat(mn, "BCTA,gt "); // we should warn the user about this!
            }
        acase STYLE_OLDCALM:
        case  STYLE_NEWCALM:
            strcat(mn,                          "JUMP,GT  ");
        acase STYLE_IEEE:
            strcat(mn,                          "BGT    ");
        }
        strcat(mn, addressstring);
                                                                                       sprintf(an, "%sgoto %s;", cctrue[style][BRANCHCODE], pseudostring);
    acase 0x1E:
        BRA_EA_TRACE(TRUE);
        switch (style)
        {
        case STYLE_SIGNETICS1:
            strcat(mn,                          "BCTA,lt ");
        acase STYLE_SIGNETICS2:
            switch (ccmode)
            {
            case  CCMODE_UNDEFINED:  strcat(mn, "BCTA,lt ");
            acase CCMODE_COMPARE:    strcat(mn, "BLA     "); // Branch if Lower, Absolute
            acase CCMODE_ARITHMETIC: strcat(mn, "BMA     "); // Branch if Minus, Absolute
            acase CCMODE_TEST:       strcat(mn, "BMA     "); // Branch if Mixed, Absolute
            }
        acase STYLE_OLDCALM:
            if (ccmode == CCMODE_TEST)
            {   strcat(mn,                      "JUMP,NO  ");
            } else
            {   strcat(mn,                      "JUMP,LT  ");
            }
        acase STYLE_NEWCALM:
            strcat(mn,                          "JUMP,LT  ");
        acase STYLE_IEEE:
            strcat(mn,                          "BLT    ");
        }
        strcat(mn, addressstring);
                                                                                       sprintf(an, "%sgoto %s;", cctrue[style][BRANCHCODE], pseudostring);
    acase 0x1F:
        BRA_EA_TRACE(TRUE);
        switch (style)
        {
        case  STYLE_SIGNETICS1:      strcat(mn, "BCTA,un ");
        acase STYLE_SIGNETICS2:      strcat(mn, "BA      ");
        acase STYLE_OLDCALM:
        case  STYLE_NEWCALM:         strcat(mn, "JUMP     ");
        acase STYLE_IEEE:            strcat(mn, "BR     ");
        }
        strcat(mn, addressstring);
                                                                                       sprintf(an, "goto %s;", pseudostring);
    acase 0x20:
        switch (style)
        {
        case  STYLE_OLDCALM: strcat(       mn , "CLR      A"          );               strcpy( an, "A = 0;" );
        acase STYLE_NEWCALM: strcat(       mn , "CLR      r0"         );               strcpy( an, "r0 = 0;");
        acase STYLE_IEEE:    strcat(       mn , "CLR    .0"           );               strcpy( an, ".0 = 0;");
        adefault:            strcat(       mn , "EORZ    r0"          );               strcpy( an, "r0 = 0;");
        }
        ccmode = CCMODE_ARITHMETIC;
    acase 0x21:
    case 0x22:
    case 0x23:
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "XOR      A,%s"  , reg);               sprintf(an, "A ^= %s;" , reg);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "XOR      %s,r0" , reg);               sprintf(an, "r0 ^= %s;", reg);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "XOR    .0,%s"   , reg);               sprintf(an, ".0 ^= %s;", reg);
        adefault:            sprintf(ENDOF(mn), "EORZ    %s"     , reg);               sprintf(an, "r0 ^= %s;", reg);
        }
        ccmode = CCMODE_ARITHMETIC;
    acase 0x24:
    case 0x25:
    case 0x26:
    case 0x27:
        dec_to_hex(OPERAND);
        switch (style)
        {
        case  STYLE_OLDCALM:
                      if (OPERAND == 0xFF)
                          sprintf(ENDOF(mn), "NOT      %s"    , reg);
                      else
                          sprintf(ENDOF(mn), "XOR      %s,#%s", reg, v);
        acase STYLE_NEWCALM:
                      if (OPERAND == 0xFF)
                          sprintf(ENDOF(mn), "NOT      %s"    , reg);
                      else
                          sprintf(ENDOF(mn), "XOR      #%s,%s", v, reg);
        acase STYLE_IEEE:
                      if (OPERAND == 0xFF)
                          sprintf(ENDOF(mn), "NOT    %s"      , reg);
                      else
                          sprintf(ENDOF(mn), "XOR    %s,#%s"  , reg, v);
        adefault:         sprintf(ENDOF(mn), "EORI,%s %s"     , reg, v);
        }                                                                              sprintf(an, "%s ^= %s;"   , reg, v_pse);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x28:
    case 0x29:
    case 0x2A:
    case 0x2B:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "XOR      %s,%s", reg, addressstring);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "XOR      %s,%s", addressstring, reg);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "XOR    %s,#%s" , reg, v);
        adefault:            sprintf(ENDOF(mn), "EORR,%s %s"    , reg, addressstring);
        }                                                                              sprintf(an, "%s ^= *(%s);", reg, pseudostring);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x2C:
    case 0x2D:
    case 0x2E:
    case 0x2F:
        ABS_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "XOR      %s,%s", reg, addressstring);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "XOR      %s,%s", addressstring, reg);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "XOR    %s,%s"  , reg, addressstring);
        adefault:            sprintf(ENDOF(mn), "EORA,%s %s"    , reg, addressstring);
        }                                                                              sprintf(an, "%s ^= *(%s);", reg, pseudostring);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "LOAD     %s,$CTRL", reg);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "MOVE     $CTRL,%s", reg);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "IN     %s,CTRL"   , reg);
        adefault:            sprintf(ENDOF(mn), "REDC,%s"          , reg);
        }
                                                                                       sprintf(an, "%s = IOPORT(CTRL);" , reg);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x34:
        switch (style)
        {
        case  STYLE_OLDCALM: if (ccmode == CCMODE_TEST)
                             {   strcat(mn, "RETION,AO");
                             } else
                             {   sprintf(ENDOF(mn), "RETION%s", ccstring[style][BRANCHCODE]);
                             }                                                            sprintf(an, "%s{ U &= ~U_IOF; return; }"    , cctrue[style][BRANCHCODE]);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "RETI%s"  , ccstring[style][BRANCHCODE]); sprintf(an, "%s{ PSU &= ~PSU_IOF; return; }", cctrue[style][BRANCHCODE]);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "RETI%s"  , ccstring[style][BRANCHCODE]); sprintf(an, "%s{ .U &= ~.U_II; return; }"   , cctrue[style][BRANCHCODE]);
        adefault:            sprintf(ENDOF(mn), "RETE,%s" , ccstring[style][BRANCHCODE]); sprintf(an, "%s{ PSU &= ~PSU_II; return; }" , cctrue[style][BRANCHCODE]);
        }
    acase 0x35:
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "RETION%s", ccstring[style][BRANCHCODE]); sprintf(an, "%s{ U &= ~U_IOF; return; }"   ,  cctrue[style][BRANCHCODE]);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "RETI%s"  , ccstring[style][BRANCHCODE]); sprintf(an, "%s{ PSU &= ~PSU_IOF; return; }", cctrue[style][BRANCHCODE]);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "RETI%s"  , ccstring[style][BRANCHCODE]); sprintf(an, "%s{ .U &= ~.U_II; return; }"  ,  cctrue[style][BRANCHCODE]);
        adefault:            sprintf(ENDOF(mn), "RETE,%s" , ccstring[style][BRANCHCODE]); sprintf(an, "%s{ PSU &= ~PSU_II; return; }",  cctrue[style][BRANCHCODE]);
        }
    acase 0x36:
        switch (style)
        {
        case  STYLE_OLDCALM: if (ccmode == CCMODE_TEST)
                             {   strcat(mn, "RETION,NO");
                             } else
                             {   sprintf(ENDOF(mn), "RETION%s", ccstring[style][BRANCHCODE]);
                             }                                                            sprintf(an, "%s{ U &= ~U_IOF; return; }"    , cctrue[style][BRANCHCODE]);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "RETI%s"  , ccstring[style][BRANCHCODE]); sprintf(an, "%s{ PSU &= ~PSU_IOF; return; }", cctrue[style][BRANCHCODE]);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "RETI%s"  , ccstring[style][BRANCHCODE]); sprintf(an, "%s{ .U &= ~.U_II; return; }"   , cctrue[style][BRANCHCODE]);
        adefault:            sprintf(ENDOF(mn), "RETE,%s" , ccstring[style][BRANCHCODE]); sprintf(an, "%s{ PSU &= ~PSU_II; return; }" , cctrue[style][BRANCHCODE]);
        }
    acase 0x37:
        switch (style)
        {
        case  STYLE_OLDCALM: strcat(       mn , "RETION"                 );               strcpy( an, "U &= ~U_IOF; return;"    );
        acase STYLE_NEWCALM: strcat(       mn , "RETI"                   );               strcpy( an, "PSU &= ~PSU_IOF; return;");
        acase STYLE_IEEE:    strcat(       mn , "RETI"                   );               strcpy( an, ".U &= ~.U_II; return;"   );
        adefault:            strcat(       mn , "RETE,un"                );               strcpy( an, "PSU &= ~PSU_II; return;" );
        }
    acase 0x38:
    case 0x39:
    case 0x3A:
        REL_EA_TRACE();
        switch (style)
        {
        case STYLE_OLDCALM:
        case STYLE_NEWCALM:
        case STYLE_IEEE:  sprintf(ENDOF(mn), "CALL%s %s" , ccstring[style][BRANCHCODE], addressstring);
        adefault:         sprintf(ENDOF(mn), "BSTR,%s %s", ccstring[style][BRANCHCODE], addressstring);
        }
                                                                                       sprintf(an, "%sgosub %s;"    , cctrue[style][BRANCHCODE], pseudostring);
    acase 0x3B:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_SIGNETICS1: strcat( mn , "BSTR,un ");
        acase STYLE_SIGNETICS2: strcat( mn , "BSR     ");
        acase STYLE_OLDCALM:
        case  STYLE_NEWCALM:    strcat( mn , "CALL     ");
        acase STYLE_IEEE:       strcat( mn , "CALL   ");
        }
        strcat(mn, addressstring);
                                                                                       sprintf(an, "gosub %s;"      , pseudostring);
    acase 0x3C:
    case 0x3D:
    case 0x3E:
        BRA_EA_TRACE(TRUE);
        switch (style)
        {
        case STYLE_OLDCALM:
        case STYLE_NEWCALM:
        case STYLE_IEEE:  sprintf(ENDOF(mn), "CALL%s %s" , ccstring[style][BRANCHCODE], addressstring);
        adefault:         sprintf(ENDOF(mn), "BSTA,%s %s", ccstring[style][BRANCHCODE], addressstring);
        }
                                                                                       sprintf(an, "%sgosub %s;"    , cctrue[style][BRANCHCODE], pseudostring);
    acase 0x3F:
        BRA_EA_TRACE(TRUE);
        switch (style)
        {
        case  STYLE_SIGNETICS1:  strcat(mn , "BSTA,un ");
        acase STYLE_SIGNETICS2:  strcat(mn , "BSA     ");
        acase STYLE_OLDCALM:
        case  STYLE_NEWCALM:     strcat(mn , "CALL     ");
        acase STYLE_IEEE:        strcat(mn , "CALL   ");
        }
        strcat(mn, addressstring);
                                                                                       sprintf(an, "gosub %s;"      , pseudostring);
    acase 0x40:
        if (style == STYLE_OLDCALM || style == STYLE_NEWCALM)
                                 strcat(mn , "WAIT");
        else                     strcat(mn , "HALT");
                                                                                       strcpy( an, "for (;;);");
    acase 0x41:
    case 0x42:
    case 0x43:
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "AND      A,%s", reg);                    sprintf(an, "A &= %s;"       , reg);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "AND      %s,r0", reg);                   sprintf(an, "r0 &= %s;"      , reg);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "AND    .0,%s" , reg);                    sprintf(an, ".0 &= %s;"      , reg);
        adefault:            sprintf(ENDOF(mn), "ANDZ    %s"   , reg);                    sprintf(an, "r0 &= %s;"      , reg);
        }
        ccmode = CCMODE_ARITHMETIC;
    acase 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
        dec_to_hex(OPERAND);
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "AND      %s,#%s", reg, v);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "AND      #%s,%s", v, reg);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "AND    %s,#%s"  , reg, v);
        adefault:            sprintf(ENDOF(mn), "ANDI,%s %s"     , reg, v);
        }
                                                                                       sprintf(an, "%s &= %s;"      , reg, v_pse);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x48:
    case 0x49:
    case 0x4A:
    case 0x4B:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "AND      %s,%s" , reg, addressstring);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "AND      %s,%s" , addressstring, reg);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "AND    %s,%s"   , reg, addressstring);
        adefault:            sprintf(ENDOF(mn), "ANDR,%s %s"     , reg, addressstring);
        }
                                                                                       sprintf(an, "%s &= *(%s);"   , reg, pseudostring);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x4C:
    case 0x4D:
    case 0x4E:
    case 0x4F:
        ABS_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "AND      %s,%s" , reg, addressstring);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "AND      %s,%s" , addressstring, reg);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "AND    %s,%s"   , reg, addressstring);
        adefault:            sprintf(ENDOF(mn), "ANDA,%s %s"     , reg, addressstring);
        }
                                                                                       sprintf(an, "%s &= *(%s);"   , reg, pseudostring);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x50:
    case 0x51:
    case 0x52:
    case 0x53:
        switch (style)
        {
        case STYLE_OLDCALM:
        case STYLE_NEWCALM:
            if (psl & PSL_WC)
            {   if (psl & PSL_C)
                {         sprintf(ENDOF(mn), "RRC      %s", reg);
                } else
                {         sprintf(ENDOF(mn), "SR       %s", reg);
            }   }
            else
            {             sprintf(ENDOF(mn), "RR       %s", reg);
            }
        acase STYLE_IEEE:
            if (psl & PSL_WC)
            {   if (psl & PSL_C)
                {         sprintf(ENDOF(mn), "RORC   %s"  , reg);
                } else
                {         sprintf(ENDOF(mn), "SHR    %s"  , reg);
            }   }
            else
            {             sprintf(ENDOF(mn), "ROR    %s"  , reg);
            }
        adefault:
                          sprintf(ENDOF(mn), "RRR,%s"     , reg);
        }
                                                                                       sprintf(an, "%s >>= 1;"      , reg);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x54:
    case 0x55:
    case 0x56:
    case 0x57:
        dec_to_hex(OPERAND);
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "LOAD     %s,$%s", reg, v);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "MOVE     $%s,%s", v, reg);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "IN     %s,%s"   , reg, v);
        adefault:            sprintf(ENDOF(mn), "REDE,%s %s"     , reg, v);
        }
                                                                                       sprintf(an, "%s = IOPORT(%s);", reg, v_pse);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x58:
    case 0x59:
    case 0x5A:
    case 0x5B:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "JUMP,%sNE %s"  , reg, addressstring);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "JUMP,NE  %s,%s", reg, addressstring);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "BNZ    %s,%s"  , reg, addressstring);
        adefault:            sprintf(ENDOF(mn), "BRNR,%s %s"    , reg, addressstring);
        }
                                                                                       sprintf(an, "if (%s != 0) goto %s;", reg, pseudostring);
    acase 0x5C:
    case 0x5D:
    case 0x5E:
    case 0x5F:
        BRA_EA_TRACE(TRUE);
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "JUMP,%sNE %s"  , reg, addressstring);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "JUMP,NE  %s,%s", reg, addressstring);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "BNZ    %s,%s"  , reg, addressstring);
        adefault:            sprintf(ENDOF(mn), "BRNA,%s %s"    , reg, addressstring);
        }
                                                                                       sprintf(an, "if (%s != 0) goto %s;", reg, pseudostring);
    acase 0x60:
    case 0x61:
    case 0x62:
    case 0x63:
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "OR       A,%s" , reg);                sprintf(an, "A |= %s;"       , reg);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "OR       %s,r0", reg);                sprintf(an, "r0 |= %s;"      , reg);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "OR     .0,%s"  , reg);                sprintf(an, ".0 |= %s;"      , reg);
        adefault:            sprintf(ENDOF(mn), "IORZ    %s"    , reg);                sprintf(an, "r0 |= %s;"      , reg);
        }
        ccmode = CCMODE_ARITHMETIC;
    acase 0x64:
    case 0x65:
    case 0x66:
    case 0x67:
        dec_to_hex(OPERAND);
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "OR       %s,#%s", reg, v);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "OR       #%s,%s", v, reg);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "OR     %s,#%s"  , reg, v);
        adefault:            sprintf(ENDOF(mn), "IORI,%s %s"     , reg, v);
        }                                                                              sprintf(an, "%s |= %s;"      , reg, v_pse);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x68:
    case 0x69:
    case 0x6A:
    case 0x6B:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "OR       %s,%s" , reg, addressstring);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "OR       %s,%s" , addressstring, reg);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "OR     %s,%s"   , reg, addressstring);
        adefault:            sprintf(ENDOF(mn), "IORR,%s %s"     , reg, addressstring);
        }                                                                              sprintf(an, "%s |= *(%s);"   , reg, pseudostring);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x6C:
    case 0x6D:
    case 0x6E:
    case 0x6F:
        ABS_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "OR       %s,%s" , reg, addressstring);
        case  STYLE_NEWCALM: sprintf(ENDOF(mn), "OR       %s,%s" , addressstring, reg);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "OR     %s,%s"   , reg, addressstring);
        adefault:            sprintf(ENDOF(mn), "IORA,%s %s"     , reg, addressstring);
        }                                                                              sprintf(an, "%s |= *(%s);", reg, pseudostring);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "LOAD     %s,$DATA", reg);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "MOVE     $DATA,%s", reg);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "IN     %s,DATA"   , reg);
        adefault:            sprintf(ENDOF(mn), "REDD,%s"          , reg);
        }                                                                              sprintf(an, "%s = IOPORT(DATA);", reg);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x74:
        dec_to_hex(OPERAND);
        switch (style)
        {
        case   STYLE_OLDCALM:
        case   STYLE_NEWCALM:
            switch (OPERAND)
            {
            case  0x07:  strcat(       mn , "CLR      STACK");
            acase 0x20:  strcat(       mn , "CLR      IOF"); // or just ION
            acase 0x40:  strcat(       mn , "CLR      OUTPUT");
            acase 0x80:  strcat(       mn , "CLR      INPUT");
            adefault:    if (style == STYLE_OLDCALM)
                         {   sprintf(ENDOF(mn), "BIC      U,#%s", v);
                         } else
                         {   sprintf(ENDOF(mn), "BIC      #%s,PSU", v);
            }            }
            if (style == STYLE_OLDCALM)
            {   if (supercpu)                                                          sprintf(an, "U &= ~(%s & %%01111111);", v_pse);
                else                                                                   sprintf(an, "U &= ~(%s & %%01100111);", v_pse);
            } else
            {   if (supercpu)                                                          sprintf(an, "PSU &= ~(%s & %%01111111);", v_pse);
                else                                                                   sprintf(an, "PSU &= ~(%s & %%01100111);", v_pse);
            }
        acase STYLE_IEEE:
            switch (OPERAND)
            {
            case 0x20:   strcat(       mn , "EI");
            adefault:    sprintf(ENDOF(mn), "AND    .U,#$FF-%s", v);
            }
            if (supercpu)                                                              sprintf(an, ".U &= ~(%s & %%01111111);", v_pse);
            else                                                                       sprintf(an, ".U &= ~(%s & %%01100111);", v_pse);
        adefault:
                         strcat(       mn , "CPSU    ");
            if ((OPERAND & 0x1F) == 0x00)
            {   started = FALSE;
                if ((OPERAND & 0x80) == 0x80) {                               strcat(mn, "S"  ); started = TRUE; }
                if ((OPERAND & 0x40) == 0x40) { if (started) strcat(mn, "+"); strcat(mn, "F"  ); started = TRUE; }
                if ((OPERAND & 0x20) == 0x20) { if (started) strcat(mn, "+"); strcat(mn, "II" );                 }
            } else
            {   strcat(mn, v);
            }
            if (supercpu)                                                              sprintf(an, "PSU &= ~(%s & %%01111111);", v);
            else                                                                       sprintf(an, "PSU &= ~(%s & %%01100111);", v);
        }

        if (OPERAND & 0x40) sprintf(ENDOF(en), ";Clear %s (%s) bit\n",                    flagname[NAME_F ].shorter[style], flagname[NAME_F ].longer[style]);
        if (OPERAND & 0x20) sprintf(ENDOF(en), ";Clear %s (%s) bit (now disinhibited)\n", flagname[NAME_II].shorter[style], flagname[NAME_II].longer[style]);
        if (supercpu)
        {   if (OPERAND & 0x10) strcat(en, ";Clear User Flag #1\n");
            if (OPERAND &  0x8) strcat(en, ";Clear User Flag #2\n");
        }
        if (OPERAND &  0x7)
        {   if ((OPERAND & 0x7) == 0x7)
            {   sprintf(    ENDOF(en), ";Clear %s (%s) to 0!\n",          flagname[NAME_SP].shorter[style], flagname[NAME_SP].longer[style]);
            } else
            {   if ((OPERAND & 0x4) == 0x4)
                {   sprintf(ENDOF(en), ";Clear high bit of %s (%s)!\n",   flagname[NAME_SP].shorter[style], flagname[NAME_SP].longer[style]);
                }
                if ((OPERAND & 0x2) == 0x2)
                {   sprintf(ENDOF(en), ";Clear middle bit of %s (%s)!\n", flagname[NAME_SP].shorter[style], flagname[NAME_SP].longer[style]);
                }
                if ((OPERAND & 0x1) == 0x1)
                {   sprintf(ENDOF(en), ";Clear low bit of %s (%s)!\n",    flagname[NAME_SP].shorter[style], flagname[NAME_SP].longer[style]);
        }   }   }
    acase 0x75:
        dec_to_hex(OPERAND);
        switch (style)
        {
        case STYLE_OLDCALM:
        case STYLE_NEWCALM:
            switch (OPERAND)
            {
            case  0x01: strcat(       mn , "CLR      CARRY"); // or just CLRC
            acase 0x02: strcat(       mn , "CLR      LOGICOMP");
            acase 0x04: strcat(       mn , "CLR      OVERFLOW"); // or just CLRV
            acase 0x08: strcat(       mn , "CLR      WITHCARRY");
            acase 0x10: strcat(       mn , "CLR      BANK"); // BANK1 is acceptable as a synonym of BANK
            acase 0x20: strcat(       mn , "CLR      HALFCARRY");
            adefault:
                if (style == STYLE_OLDCALM)
                {       sprintf(ENDOF(mn), "BIC      L,#%s", v);
                } else
                {       sprintf(ENDOF(mn), "BIC      #%s,PSL", v);
            }   }
            if (style == STYLE_OLDCALM)
            {   if (OPERAND == 0xFF)                                                   strcpy( an, "L = 0;");
                else                                                                   sprintf(an, "L &= ~(%s);", v_pse);
            } else
            {   if (OPERAND == 0xFF)                                                   strcpy( an, "PSL = 0;");
                else                                                                   sprintf(an, "PSL &= ~(%s);", v_pse);
            }
        acase STYLE_IEEE:
            switch (OPERAND)
            {
            case  0x01: strcat(       mn , "CLRC");
            acase 0x04: strcat(       mn , "CLRV");
            adefault:   sprintf(ENDOF(mn), "AND    .L,#$FF-%s", v);
            }
            if (OPERAND == 0xFF)                                                       strcpy( an, ".L = 0;");
            else                                                                       sprintf(an, ".L &= ~(%s);", v);
        adefault:
            strcat(mn, "CPSL    ");
            if ((OPERAND & 0xC0) == 0x00)
            {   started = FALSE;
                if ((OPERAND & 0x20) == 0x20) {                               strcat(mn, "IDC"); started = TRUE; }
                if ((OPERAND & 0x10) == 0x10) { if (started) strcat(mn, "+"); strcat(mn, "RS" ); started = TRUE; }
                if ((OPERAND & 0x08) == 0x08) { if (started) strcat(mn, "+"); strcat(mn, "WC" ); started = TRUE; }
                if ((OPERAND & 0x04) == 0x04) { if (started) strcat(mn, "+"); strcat(mn, "OVF"); started = TRUE; }
                if ((OPERAND & 0x02) == 0x02) { if (started) strcat(mn, "+"); strcat(mn, "COM"); started = TRUE; }
                if ((OPERAND & 0x01) == 0x01) { if (started) strcat(mn, "+"); strcat(mn, "C"  );                 }
            } else
            {   strcat(mn, v);
            }
            if (OPERAND == 0xFF)                                                       strcpy( an, "PSL = 0;");
            else                                                                       sprintf(an, "PSL &= ~(%s);", v);
        }

        if (OPERAND & 0xC0)
        {   switch (OPERAND & 0xC0)
            {
            case 0xC0:
                strcat(en, ";Set CC (Condition Code) to EQ\n");
            acase 0x80:
                strcat(en, ";Clear high bit of CC (Condition Code)\n");
            acase 0x40:
                strcat(en, ";Clear low bit of CC (Condition Code)\n");
        }   }
        if (OPERAND & 0x20) sprintf(ENDOF(en), ";Clear %s (%s) bit\n",              flagname[NAME_IDC].shorter[style], flagname[NAME_IDC].longer[style]);
        if (OPERAND & 0x10) sprintf(ENDOF(en), ";Clear %s (%s) bit (now %s..%s)\n", flagname[NAME_RS ].shorter[style], flagname[NAME_RS ].longer[style],
                                                                                    flagname[NAME_R1 ].shorter[style], flagname[NAME_R3].shorter[style]);
        if (OPERAND &  0x8) sprintf(ENDOF(en), ";Clear %s (%s) bit\n",              flagname[NAME_WC ].shorter[style], flagname[NAME_WC ].longer[style]);
        if (OPERAND &  0x4) sprintf(ENDOF(en), ";Clear %s (%s) bit\n",              flagname[NAME_OVF].shorter[style], flagname[NAME_OVF].longer[style]);
        if (OPERAND &  0x2) sprintf(ENDOF(en), ";Clear %s (%s) bit (now signed/arithmetic)\n", flagname[NAME_COM].shorter[style], flagname[NAME_COM].longer[style]);
        if (OPERAND &  0x1) sprintf(ENDOF(en), ";Clear %s (%s) bit\n",              flagname[NAME_C  ].shorter[style], flagname[NAME_C  ].longer[style]);

        ccmode = CCMODE_UNDEFINED;
    acase 0x76:
        dec_to_hex(OPERAND);
        switch (style)
        {
        case STYLE_OLDCALM:
        case STYLE_NEWCALM:
            switch (OPERAND)
            {
            case  0x20:   strcat(       mn , "SET      IOF"); // or just IOF
            acase 0x40:   strcat(       mn , "SET      OUTPUT");
            acase 0x80:   strcat(       mn , "SET      INPUT");
            adefault:     if (style == STYLE_OLDCALM)
                          {    sprintf(ENDOF(mn), "OR       U,#%s", v);
                          } else
                          {    sprintf(ENDOF(mn), "OR       #%s,PSU", v);
            }             }
            if (style == STYLE_OLDCALM)
            {   if (supercpu)                                                          sprintf(an, "U |= %s & %%01111111;", v_pse);
                else                                                                   sprintf(an, "U |= %s & %%01100111;", v_pse);
            } else
            {   if (supercpu)                                                          sprintf(an, "PSU |= %s & %%01111111;", v_pse);
                else                                                                   sprintf(an, "PSU |= %s & %%01100111;", v_pse);
            }
        acase STYLE_IEEE:
            switch (OPERAND)
            {
            case 0x20:    strcat(       mn , "DI");
            adefault:     sprintf(ENDOF(mn), "OR     .U,#%s", v);
            }
            if (supercpu)                                                              sprintf(an, "PSU |= %s & %%01111111;", v);
            else                                                                       sprintf(an, "PSU |= %s & %%01100111;", v);
        adefault:
                          strcat(       mn, "PPSU    ");
            if ((OPERAND & 0x1F) == 0x00)
            {   started = FALSE;
                if ((OPERAND & 0x80) == 0x80) {                               strcat(mn, "S"  ); started = TRUE; }
                if ((OPERAND & 0x40) == 0x40) { if (started) strcat(mn, "+"); strcat(mn, "F"  ); started = TRUE; }
                if ((OPERAND & 0x20) == 0x20) { if (started) strcat(mn, "+"); strcat(mn, "II" );                 }
            } else
            {   strcat(mn, v);
            }
            if (supercpu)                                                              sprintf(an, "PSU |= %s & %%01111111;", v);
            else                                                                       sprintf(an, "PSU |= %s & %%01100111;", v);
        }

        if (OPERAND & 0x40) sprintf(ENDOF(en), ";Set %s (%s) bit\n",                 flagname[NAME_F ].shorter[style], flagname[NAME_F ].longer[style]);
        if (OPERAND & 0x20) sprintf(ENDOF(en), ";Set %s (%s) bit (now inhibited)\n", flagname[NAME_II].shorter[style], flagname[NAME_II].longer[style]);
        if (supercpu)
        {   if (OPERAND & 0x10) strcat(en, ";Set User Flag #1\n");
            if (OPERAND &  0x8) strcat(en, ";Set User Flag #2\n");
        }
        if (OPERAND &  0x7)
        {   if ((OPERAND & 0x7) == 0x7)
            {   sprintf(ENDOF(en), ";Set %s (%s) to 7!\n",              flagname[NAME_SP].shorter[style], flagname[NAME_SP].longer[style]); // SP
            } else
            {   if ((OPERAND & 0x4) == 0x4)
                {   sprintf(ENDOF(en), ";Set high bit of %s (%s)!\n",   flagname[NAME_SP].shorter[style], flagname[NAME_SP].longer[style]); // SP
                }
                if ((OPERAND & 0x2) == 0x2)
                {   sprintf(ENDOF(en), ";Set middle bit of %s (%s)!\n", flagname[NAME_SP].shorter[style], flagname[NAME_SP].longer[style]); // SP
                }
                if ((OPERAND & 0x1) == 0x1)
                {   sprintf(ENDOF(en), ";Set low bit of %s (%s)!\n",    flagname[NAME_SP].shorter[style], flagname[NAME_SP].longer[style]); // SP
        }   }   }
    acase 0x77:
        dec_to_hex(OPERAND);
        switch (style)
        {
        case  STYLE_OLDCALM:
            switch (OPERAND)
            {
            case  0x01:   strcat(       mn , "SET      CARRY"); // or just SETC
            acase 0x02:   strcat(       mn , "SET      LOGICOMP");
            acase 0x04:   strcat(       mn , "SET      OVERFLOW"); // or just SETV
            acase 0x08:   strcat(       mn , "SET      WITHCARRY");
            acase 0x10:   strcat(       mn , "SET      BANK"); // BANK1 is acceptable as a synonym of BANK
            acase 0x20:   strcat(       mn , "SET      HALFCARRY");
            adefault:     sprintf(ENDOF(mn), "OR       L,#%s", v);
            }
            if (OPERAND == 0xFF)                                          strcpy( an, "L = $FF;");
            else                                                          sprintf(an, "L |= %s;" , v_pse);
        acase STYLE_NEWCALM:
            switch (OPERAND)
            {
            case  0x01:   strcat(       mn , "SET      CARRY"); // or just SETC
            acase 0x02:   strcat(       mn , "SET      LOGICOMP");
            acase 0x04:   strcat(       mn , "SET      OVERFLOW"); // or just SETV
            acase 0x08:   strcat(       mn , "SET      WITHCARRY");
            acase 0x10:   strcat(       mn , "SET      BANK"); // BANK1 is acceptable as a synonym of BANK
            acase 0x20:   strcat(       mn , "SET      HALFCARRY");
            adefault:     sprintf(ENDOF(mn), "OR       #%s,PSL", v);
            }
            if (OPERAND == 0xFF)                                          strcpy( an, "PSL = $FF;");
            else                                                          sprintf(an, "PSL |= %s;" , v_pse);
        acase STYLE_IEEE:
            switch (OPERAND)
            {
            case  0x01:   strcat(       mn , "SETC");
            acase 0x04:   strcat(       mn , "SETV");
            adefault:     sprintf(ENDOF(mn), "OR     .L,#%s", v);
            }
            if (OPERAND == 0xFF)                                          strcpy( an, ".L = $FF;");
            else                                                          sprintf(an, ".L |= %s;" , v);
        adefault:
                          strcat(       mn , "PPSL    ");
            if ((OPERAND & 0xC0) == 0x00)
            {   started = FALSE;
                if ((OPERAND & 0x20) == 0x20) {                               strcat(mn, "IDC"); started = TRUE; }
                if ((OPERAND & 0x10) == 0x10) { if (started) strcat(mn, "+"); strcat(mn, "RS" ); started = TRUE; }
                if ((OPERAND & 0x08) == 0x08) { if (started) strcat(mn, "+"); strcat(mn, "WC" ); started = TRUE; }
                if ((OPERAND & 0x04) == 0x04) { if (started) strcat(mn, "+"); strcat(mn, "OVF"); started = TRUE; }
                if ((OPERAND & 0x02) == 0x02) { if (started) strcat(mn, "+"); strcat(mn, "COM"); started = TRUE; }
                if ((OPERAND & 0x01) == 0x01) { if (started) strcat(mn, "+"); strcat(mn, "C"  );                 }
            } else
            {   strcat(mn, v);
            }
            if (OPERAND == 0xFF)                                          strcpy( an, "PSL = $FF;");
            else                                                          sprintf(an, "PSL |= %s;" , v);
        }

        if (OPERAND & 0xC0)
        {   switch (OPERAND & 0xC0)
            {
            case 0xC0:
                strcat(en, ";Set CC (Condition Code) to UN (%11)!\n");
            acase 0x80:
                strcat(en, ";Set high bit of CC (Condition Code)\n");
            acase 0x40:
                strcat(en, ";Set low bit of CC (Condition Code)\n");
        }   }
        if (OPERAND & 0x20) sprintf(ENDOF(en), ";Set %s (%s) bit\n",                flagname[NAME_IDC].shorter[style], flagname[NAME_IDC].longer[style]);
        if (OPERAND & 0x10) sprintf(ENDOF(en), ";Set %s (%s) bit (now %s..%s)\n",   flagname[NAME_RS ].shorter[style], flagname[NAME_RS ].longer[style],
                                                                                    flagname[NAME_R4 ].shorter[style], flagname[NAME_R6].shorter[style]);
        if (OPERAND &  0x8) sprintf(ENDOF(en), ";Set %s (%s) bit\n",                flagname[NAME_WC ].shorter[style], flagname[NAME_WC ].longer[style]);
        if (OPERAND &  0x4) sprintf(ENDOF(en), ";Set %s (%s) bit\n",                flagname[NAME_OVF].shorter[style], flagname[NAME_OVF].longer[style]);
        if (OPERAND &  0x2) sprintf(ENDOF(en), ";Set %s (%s) bit (now unsigned/logical)\n", flagname[NAME_COM].shorter[style], flagname[NAME_COM].longer[style]);
        if (OPERAND &  0x1) sprintf(ENDOF(en), ";Set %s (%s) bit\n",                flagname[NAME_C  ].shorter[style], flagname[NAME_C  ].longer[style]);

        ccmode = CCMODE_UNDEFINED;
    acase 0x78:
    case 0x79:
    case 0x7A:
    case 0x7B:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "CALL,%sNE %s"  , reg, addressstring);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "CALL,NE  %s,%s", reg, addressstring);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "CALLNZ %s,%s"  , reg, addressstring);
        adefault:            sprintf(ENDOF(mn), "BSNR,%s %s"    , reg, addressstring);
        }
                                                                                       sprintf(an, "if (%s != 0) gosub %s;", reg, pseudostring);
    acase 0x7C:
    case 0x7D:
    case 0x7E:
    case 0x7F:
        BRA_EA_TRACE(TRUE);
        switch (style)
        {
        case  STYLE_OLDCALM: sprintf(ENDOF(mn), "CALL,%sNE %s"  , reg, addressstring);
        acase STYLE_NEWCALM: sprintf(ENDOF(mn), "CALL,NE  %s,%s", reg, addressstring);
        acase STYLE_IEEE:    sprintf(ENDOF(mn), "CALLNZ %s,%s"  , reg, addressstring);
        adefault:            sprintf(ENDOF(mn), "BSNA,%s %s"    , reg, addressstring);
        }
                                                                                       sprintf(an, "if (%s != 0) gosub %s;", reg, pseudostring);
    acase 0x80:
        switch (style)
        {
        case  STYLE_OLDCALM:
            if (psl & PSL_WC) { strcat( mn , "ADDC     A,A");                          strcpy( an, "A += A;");   }
            else              { strcat( mn , "ADD      A,A");                          strcpy( an, "A *= 2;");   }
        acase STYLE_NEWCALM:
            if (psl & PSL_WC) { strcat( mn , "ADDC     r0,r0");                        strcpy( an, "r0 += r0;"); }
            else              { strcat( mn , "ADD      r0,r0");                        strcpy( an, "r0 *= 2;");  }
        acase STYLE_IEEE:
            if (psl & PSL_WC) { strcat( mn , "ADC    .0,.0");                          strcpy( an, ".0 += .0;"); }
            else              { strcat( mn , "ADD    .0,.0");                          strcpy( an, ".0 *= 2;");  }
        adefault:
                                strcat( mn , "ADDZ    r0"  );
            if (psl & PSL_WC)                                                          strcpy( an, "r0 += r0;");
            else                                                                       strcpy( an, "r0 *= 2;");
        }
        ccmode = CCMODE_ARITHMETIC;
    acase 0x81:
    case 0x82:
    case 0x83:
        switch (style)
        {
        case  STYLE_OLDCALM:
            if (psl & PSL_WC) sprintf(ENDOF(mn), "ADDC     A,%s", reg);
            else              sprintf(ENDOF(mn), "ADD      A,%s", reg);
                                                                                       sprintf(an, "A += %s;"       , reg);
        acase STYLE_NEWCALM:
            if (psl & PSL_WC) sprintf(ENDOF(mn), "ADDC     %s,r0", reg);
            else              sprintf(ENDOF(mn), "ADD      %s,r0", reg);
                                                                                       sprintf(an, "r0 += %s;"      , reg);
        acase STYLE_IEEE:
            if (psl & PSL_WC) sprintf(ENDOF(mn), "ADDC   .0,%s" , reg);
            else              sprintf(ENDOF(mn), "ADD    .0,%s" , reg);
                                                                                       sprintf(an, ".0 += %s;"      , reg);
        adefault:
                              sprintf(ENDOF(mn), "ADDZ    %s"   , reg);                sprintf(an, "r0 += %s;"      , reg);
        }
        ccmode = CCMODE_ARITHMETIC;
    acase 0x84:
    case 0x85:
    case 0x86:
    case 0x87:
        dec_to_hex(OPERAND);
        switch (style)
        {
        case STYLE_OLDCALM:
            if (psl & PSL_WC) sprintf(ENDOF(mn), "ADDC     %s,#%s", reg, v);
            else              sprintf(ENDOF(mn), "ADD      %s,#%s", reg, v);
        acase STYLE_NEWCALM:
            if (psl & PSL_WC) sprintf(ENDOF(mn), "ADDC     #%s,%s", v, reg);
            else              sprintf(ENDOF(mn), "ADD      #%s,%s", v, reg);
        acase STYLE_IEEE:
            if (psl & PSL_WC) sprintf(ENDOF(mn), "ADDC   %s,#%s"  , reg, v);
            else              sprintf(ENDOF(mn), "ADD    %s,#%s"  , reg, v);
        adefault:
                              sprintf(ENDOF(mn), "ADDI,%s %s"     , reg, v);
        }
        if (OPERAND == 1)                                                              sprintf(an, "%s++;"          , reg);
        else                                                                           sprintf(an, "%s += %s;"      , reg, v_pse);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x88:
    case 0x89:
    case 0x8A:
    case 0x8B:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM:
            if (psl & PSL_WC) sprintf(ENDOF(mn), "ADDC     %s,%s", reg, addressstring);
            else              sprintf(ENDOF(mn), "ADD      %s,%s", reg, addressstring);
        acase STYLE_NEWCALM:
            if (psl & PSL_WC) sprintf(ENDOF(mn), "ADDC     %s,%s", addressstring, reg);
            else              sprintf(ENDOF(mn), "ADD      %s,%s", addressstring, reg);
        acase STYLE_IEEE:
            if (psl & PSL_WC) sprintf(ENDOF(mn), "ADDC   %s,%s"  , reg, addressstring);
            else              sprintf(ENDOF(mn), "ADD    %s,%s"  , reg, addressstring);
        adefault:
                              sprintf(ENDOF(mn), "ADDR,%s %s"    , reg, addressstring);
        }
                                                                                       sprintf(an, "%s += *(%s);"   , reg, pseudostring);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x8C:
    case 0x8D:
    case 0x8E:
    case 0x8F:
        ABS_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM:
            if (psl & PSL_WC) sprintf(ENDOF(mn), "ADDC     %s,%s", reg, addressstring);
            else              sprintf(ENDOF(mn), "ADD      %s,%s", reg, addressstring);
        acase STYLE_NEWCALM:
            if (psl & PSL_WC) sprintf(ENDOF(mn), "ADDC     %s,%s", addressstring, reg);
            else              sprintf(ENDOF(mn), "ADD      %s,%s", addressstring, reg);
        acase STYLE_IEEE:
            if (psl & PSL_WC) sprintf(ENDOF(mn), "ADDC   %s,%s"  , reg, addressstring);
            else              sprintf(ENDOF(mn), "ADD    %s,%s"  , reg, addressstring);
        adefault:
                              sprintf(ENDOF(mn), "ADDA,%s %s"    , reg, addressstring);
        }
                                                                                       sprintf(an, "%s += *(%s);"   , reg, pseudostring);
        ccmode = CCMODE_ARITHMETIC;
    acase 0x92:
        switch (style)
        {
        case STYLE_OLDCALM:
                              strcat(       mn , "LOAD     U,A");
            if (supercpu)                                                              strcpy(an, "U &= %10000000; U |= (A & %01111111);");
            else                                                                       strcpy(an, "U &= %10000000; U |= (A & %01100111);");
        acase STYLE_NEWCALM:
                              strcat(       mn , "MOVE     r0,PSU");
            if (supercpu)                                                              strcpy(an, "PSU &= %10000000; PSU |= (r0 & %01111111);");
            else                                                                       strcpy(an, "PSU &= %10000000; PSU |= (r0 & %01100111);");
        acase STYLE_IEEE:
                              strcat(       mn , "MOV    .U,.0");
            if (supercpu)                                                              strcpy(an, ".U &= %10000000; .U |= (.0 & %01111111);");
            else                                                                       strcpy(an, ".U &= %10000000; .U |= (.0 & %01100111);");
        adefault:
                              strcat(       mn , "LPSU"); // or "LPSU    r0"
            if (supercpu)                                                              strcpy(an, "PSU &= %10000000; PSU |= (r0 & %01111111);");
            else                                                                       strcpy(an, "PSU &= %10000000; PSU |= (r0 & %01100111);");
        }

        if (!disassembling)
        {   sprintf(ENDOF(en), ";%s %s (%s) bit\n",                   (r[0] & 0x40) ? "Set" : "Clear", flagname[NAME_F ].shorter[style], flagname[NAME_F ].longer[style]);
            sprintf(ENDOF(en), ";%s %s (%s) bit (now %sinhibited)\n", (r[0] & 0x20) ? "Set" : "Clear", flagname[NAME_II].shorter[style], flagname[NAME_II].longer[style], (r[0] & 0x20) ? "" : "dis");
            if (supercpu)
            {   if (r[0] & 0x10) strcat(en,   ";Set User Flag #1\n");
                else             strcat(en, ";Clear User Flag #1\n");
                if (r[0] &  0x8) strcat(en,   ";Set User Flag #2\n");
                else             strcat(en, ";Clear User Flag #2\n");
            }
            sprintf(ENDOF(en), ";Set %s (%s) to %d\n", flagname[NAME_SP].shorter[style], flagname[NAME_SP].longer[style], (int) (r[0] & 0x7)); // SP
        }
    acase 0x93:
        switch (style)
        {
        case  STYLE_OLDCALM:  strcat(       mn , "LOAD     L,A");                      strcpy(an, "L = A;"   );
        acase STYLE_NEWCALM:  strcat(       mn , "MOVE     r0,PSL");                   strcpy(an, "PSL = r0;");
        acase STYLE_IEEE:     strcat(       mn , "MOV    .L,.0");                      strcpy(an, ".L = .0;" );
        adefault:             strcat(       mn , "LPSL"); /* or "LPSL    r0" */        strcpy(an, "PSL = r0;");
        }

        if (!disassembling)
        {   switch (r[0] & 0xC0)
            {
            case  0xC0: strcat(en, ";CC (Condition Code) is now UN (%11)!\n");
            acase 0x80: strcat(en, ";CC (Condition Code) is now LT (%10)\n");
            acase 0x40: strcat(en, ";CC (Condition Code) is now GT (%01)\n");
            acase 0x00: strcat(en, ";CC (Condition Code) is now EQ (%00)\n");
            }

            sprintf(ENDOF(en), ";%s %s (%s) bit\n",                (r[0] & 0x20) ? "Set" : "Clear", flagname[NAME_IDC].shorter[style], flagname[NAME_IDC].longer[style]);
            sprintf(ENDOF(en), ";%s %s (%s) bit (now %s..%s)",     (r[0] & 0x10) ? "Set" : "Clear", flagname[NAME_RS ].shorter[style], flagname[NAME_RS ].longer[style],
                                                                   (r[0] & 0x10) ? flagname[NAME_R4].shorter[style] : flagname[NAME_R1].shorter[style],
                                                                   (r[0] & 0x10) ? flagname[NAME_R6].shorter[style] : flagname[NAME_R3].shorter[style]);
            sprintf(ENDOF(en), ";%s %s (%s) bit\n",                (r[0] &  0x8) ? "Set" : "Clear", flagname[NAME_WC ].shorter[style], flagname[NAME_WC ].longer[style]);
            sprintf(ENDOF(en), ";%s %s (%s) bit\n",                (r[0] &  0x4) ? "Set" : "Clear", flagname[NAME_OVF].shorter[style], flagname[NAME_OVF].longer[style]);
            sprintf(ENDOF(en), ";%s %s (%s) bit (now %ssigned)\n", (r[0] &  0x2) ? "Set" : "Clear", flagname[NAME_COM].shorter[style], flagname[NAME_COM].longer[style], (r[0] &  0x2) ? "un" : "");
            sprintf(ENDOF(en), ";%s %s (%s) bit\n",                (r[0] &  0x1) ? "Set" : "Clear", flagname[NAME_C  ].shorter[style], flagname[NAME_C  ].longer[style]);
        }

        ccmode = CCMODE_UNDEFINED;
    acase 0x94:
    case 0x95:
    case 0x96:
    case 0x97:
        switch (style)
        {
        case  STYLE_OLDCALM:         sprintf(ENDOF(mn), "DA       %s", reg);
        acase STYLE_NEWCALM:         sprintf(ENDOF(mn), "DAA      %s", reg);
        acase STYLE_IEEE:            sprintf(ENDOF(mn), "ADJ    %s"  , reg);
        adefault:                    sprintf(ENDOF(mn), "DAR,%s"     , reg);
        }
                                                                                       sprintf(an, "%s = BCD(%s);", reg, reg);
        ccmode = CCMODE_UNDEFINED;
    acase 0x98:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_SIGNETICS1:      sprintf(ENDOF(mn), "BCFR,eq %s" , addressstring);
        acase STYLE_SIGNETICS2:
            switch (ccmode)
            {
            case  CCMODE_UNDEFINED:  sprintf(ENDOF(mn), "BCFR,eq %s" , addressstring);
            acase CCMODE_COMPARE:    sprintf(ENDOF(mn), "BNER    %s" , addressstring); // Branch if Not Equal, Relative
            acase CCMODE_ARITHMETIC: sprintf(ENDOF(mn), "BNZR    %s" , addressstring); // Branch if Not Zero, Relative
            acase CCMODE_TEST:       sprintf(ENDOF(mn), "BCFR,eq %s" , addressstring);
            }
        acase STYLE_OLDCALM:
        case  STYLE_NEWCALM:         sprintf(ENDOF(mn), "JUMP,NE  %s", addressstring);
        acase STYLE_IEEE:            sprintf(ENDOF(mn), "BNE    %s"  , addressstring);
        }
                                                                                       sprintf(an, "%sgoto %s;", ccfalse[style][BRANCHCODE], pseudostring);
    acase 0x99:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_SIGNETICS1:      sprintf(ENDOF(mn), "BCFR,gt %s" , addressstring);
        acase STYLE_SIGNETICS2:
            switch (ccmode)
            {
            case  CCMODE_UNDEFINED:  sprintf(ENDOF(mn), "BCFR,gt %s" , addressstring);
            acase CCMODE_COMPARE:    sprintf(ENDOF(mn), "BNHR    %s" , addressstring); // Branch if Not Higher, Relative
            acase CCMODE_ARITHMETIC: sprintf(ENDOF(mn), "BNPR    %s" , addressstring); // Branch if Not Positive, Relative
            acase CCMODE_TEST:       sprintf(ENDOF(mn), "BCFR,gt %s" , addressstring); // we should warn the user about this!
            }
        acase STYLE_OLDCALM:
        case  STYLE_NEWCALM:         sprintf(ENDOF(mn), "JUMP,LE  %s", addressstring);
        acase STYLE_IEEE:            sprintf(ENDOF(mn), "BLE    %s"  , addressstring);
        }
                                                                                       sprintf(an, "%sgoto %s;", ccfalse[style][BRANCHCODE], pseudostring);
    acase 0x9A:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_SIGNETICS1:      sprintf(ENDOF(mn), "BCFR,lt %s" , addressstring);
        acase STYLE_SIGNETICS2:
            switch (ccmode)
            {
            case  CCMODE_UNDEFINED:  sprintf(ENDOF(mn), "BCFR,lt %s" , addressstring);
            acase CCMODE_COMPARE:    sprintf(ENDOF(mn), "BNLR    %s" , addressstring); // Branch if Not Lower, Relative
            acase CCMODE_ARITHMETIC: sprintf(ENDOF(mn), "BNMR    %s" , addressstring); // Branch if Not Minus, Relative
            acase CCMODE_TEST:       sprintf(ENDOF(mn), "BCFR,lt %s" , addressstring);
            }
        acase STYLE_OLDCALM:
        case  STYLE_NEWCALM:         sprintf(ENDOF(mn), "JUMP,GE  %s", addressstring);
        acase STYLE_IEEE:            sprintf(ENDOF(mn), "BGE    %s"  , addressstring);
        }
                                                                                       sprintf(an, "%sgoto %s;", ccfalse[style][BRANCHCODE], pseudostring);
    acase 0x9B:
        ZERO_EA_TRACE(TRUE);
        switch (style)
        {
        case  STYLE_OLDCALM:
        case  STYLE_NEWCALM:         sprintf(ENDOF(mn), "JUMP     %s", addressstring);
        acase STYLE_IEEE:            sprintf(ENDOF(mn), "BR     %s"  , addressstring);
        adefault:                    sprintf(ENDOF(mn), "ZBRR    %s" , addressstring); // or "ZBRR,un "
        }
                                                                                       sprintf(an, "goto %s;"  , pseudostring);
    acase 0x9C:
        BRA_EA_TRACE(TRUE);
        switch (style)
        {
        case  STYLE_SIGNETICS1:      sprintf(ENDOF(mn), "BCFA,eq %s" , addressstring);
        acase STYLE_SIGNETICS2:
            switch (ccmode)
            {
            case  CCMODE_UNDEFINED:  sprintf(ENDOF(mn), "BCFA,eq %s" , addressstring);
            acase CCMODE_COMPARE:    sprintf(ENDOF(mn), "BNEA    %s" , addressstring); // Branch if Not Equal, Absolute
            acase CCMODE_ARITHMETIC: sprintf(ENDOF(mn), "BNZA    %s" , addressstring); // Branch if Not Zero, Absolute
            acase CCMODE_TEST:       sprintf(ENDOF(mn), "BCFA,eq %s" , addressstring);
            }
        acase STYLE_OLDCALM:
        case  STYLE_NEWCALM:         sprintf(ENDOF(mn), "JUMP,NE  %s", addressstring);
        acase STYLE_IEEE:            sprintf(ENDOF(mn), "BNE    %s"  , addressstring);
        }
                                                                                       sprintf(an, "%sgoto %s;", ccfalse[style][BRANCHCODE], pseudostring);
    acase 0x9D:
        BRA_EA_TRACE(TRUE);
        switch (style)
        {
        case  STYLE_SIGNETICS1:      sprintf(ENDOF(mn), "BCFA,gt %s" , addressstring);
        acase STYLE_SIGNETICS2:
            switch (ccmode)
            {
            case  CCMODE_UNDEFINED:  sprintf(ENDOF(mn), "BCFA,gt %s" , addressstring);
            acase CCMODE_COMPARE:    sprintf(ENDOF(mn), "BNHA    %s" , addressstring); // Branch if Not Higher, Absolute
            acase CCMODE_ARITHMETIC: sprintf(ENDOF(mn), "BNPA    %s" , addressstring); // Branch if Not Positive, Absolute
            acase CCMODE_TEST:       sprintf(ENDOF(mn), "BCFA,gt %s" , addressstring); // we should warn the user about this!
            }
        acase STYLE_OLDCALM:
        case  STYLE_NEWCALM:         sprintf(ENDOF(mn), "JUMP,LE  %s", addressstring);
        acase STYLE_IEEE:            sprintf(ENDOF(mn), "BLE    %s"  , addressstring);
        }
                                                                                       sprintf(an, "%sgoto %s;", ccfalse[style][BRANCHCODE], pseudostring);
    acase 0x9E:
        BRA_EA_TRACE(TRUE);
        switch (style)
        {
        case  STYLE_SIGNETICS1:      sprintf(ENDOF(mn), "BCFA,lt %s" , addressstring);
        acase STYLE_SIGNETICS2:
            switch (ccmode)
            {
            case  CCMODE_UNDEFINED:  sprintf(ENDOF(mn), "BCFA,lt %s" , addressstring);
            acase CCMODE_COMPARE:    sprintf(ENDOF(mn), "BNLA    %s" , addressstring); // Branch if Not Lower, Absolute
            acase CCMODE_ARITHMETIC: sprintf(ENDOF(mn), "BNMA    %s" , addressstring); // Branch if Not Minus, Absolute
            acase CCMODE_TEST:       sprintf(ENDOF(mn), "BCFA,lt %s" , addressstring);
            }
        acase STYLE_OLDCALM:
        case  STYLE_NEWCALM:         sprintf(ENDOF(mn), "JUMP,GE  %s", addressstring);
        acase STYLE_IEEE:            sprintf(ENDOF(mn), "BGE    %s"  , addressstring);
        }
                                                                                       sprintf(an, "%sgoto %s;", ccfalse[style][BRANCHCODE], pseudostring);
    acase 0x9F:
        BRA_EA_TRACE(FALSE);
        switch (style)
        {
        case  STYLE_OLDCALM:         sprintf(ENDOF(mn), "JUMP     (%s)+%s", reg, addressstring);
        acase STYLE_NEWCALM:         sprintf(ENDOF(mn), "JUMP     U^{%s}+%s", reg, &addressstring[2]);
        acase STYLE_IEEE:            sprintf(ENDOF(mn), "BR     %s(%s)"   , addressstring, reg);
        adefault:                    sprintf(ENDOF(mn), "BXA,%s  %s"      , reg, addressstring);
        }
                                                                                       sprintf(an, "goto %s + %s;", pseudostring, reg);
    acase 0xA0:
        switch (style)
        {
        case  STYLE_OLDCALM:
            if (psl & PSL_WC)      { strcat(       mn , "SUBB     A,A");               strcpy( an, "A -= A;"  ); }
            else                   { strcat(       mn , "SUB      A,A");               strcpy( an, "A = 0;"   ); }
        acase STYLE_NEWCALM:
            if (psl & PSL_WC)      { strcat(       mn , "SUBC     r0,r0");             strcpy( an, "r0 -= r0;"); }
            else                   { strcat(       mn , "SUB      r0,r0");             strcpy( an, "r0 = 0;"  ); }
        acase STYLE_IEEE:
            if (psl & PSL_WC)      { strcat(       mn , "SUBC   .0,.0");               strcpy( an, ".0 -= .0;"); }
            else                   { strcat(       mn , "SUB    .0,.0");               strcpy( an, ".0 = 0;"  ); }
        adefault:
                                     strcat(       mn , "SUBZ    r0"  );
            if (psl & PSL_WC)                                                          strcpy( an, "r0 -= r0;");
            else                                                                       strcpy( an, "r0 = 0;"  );
        }
        ccmode = CCMODE_ARITHMETIC;
    acase 0xA1:
    case 0xA2:
    case 0xA3:
        switch (style)
        {
        case  STYLE_OLDCALM:
            if (psl & PSL_WC)        sprintf(ENDOF(mn), "SUBB     A,%s", reg);
            else                     sprintf(ENDOF(mn), "SUB      A,%s", reg);
                                                                                       sprintf(an, "A -= %s;"       , reg);
        acase STYLE_NEWCALM:
            if (psl & PSL_WC)        sprintf(ENDOF(mn), "SUBC     %s.r0", reg);
            else                     sprintf(ENDOF(mn), "SUB      %s,r0", reg);
                                                                                       sprintf(an, "r0 -= %s;"      , reg);
        acase STYLE_IEEE:
            if (psl & PSL_WC)        sprintf(ENDOF(mn), "SUBC   .0,%s" , reg);
            else                     sprintf(ENDOF(mn), "SUB    .0,%s" , reg);
                                                                                       sprintf(an, ".0 -= %s;"      , reg);
        adefault:
                                     sprintf(ENDOF(mn), "SUBZ    %s"   , reg);         sprintf(an, "r0 -= %s;"      , reg);
        }
        ccmode = CCMODE_ARITHMETIC;
    acase 0xA4:
    case 0xA5:
    case 0xA6:
    case 0xA7:
        dec_to_hex(OPERAND);
        switch (style)
        {
        case  STYLE_OLDCALM:
            if (psl & PSL_WC)        sprintf(ENDOF(mn), "SUBB     %s,#%s", reg, v);
            else                     sprintf(ENDOF(mn), "SUB      %s,#%s", reg, v);
        acase STYLE_NEWCALM:
            if (psl & PSL_WC)        sprintf(ENDOF(mn), "SUBC     #%s,%s", v, reg);
            else                     sprintf(ENDOF(mn), "SUB      #%s,%s", v, reg);
        acase STYLE_IEEE:
            if (psl & PSL_WC)        sprintf(ENDOF(mn), "SUBC   %s,#%s"  , reg, v);
            else                     sprintf(ENDOF(mn), "SUB    %s,#%s"  , reg, v);
        adefault:
                                     sprintf(ENDOF(mn), "SUBI,%s %s"     , reg, v);
        }
        if (OPERAND == 1)                                                              sprintf(an, "%s--;"          , reg);
        else                                                                           sprintf(an, "%s -= %s;"      , reg, v_pse);
        ccmode = CCMODE_ARITHMETIC;
    acase 0xA8:
    case 0xA9:
    case 0xAA:
    case 0xAB:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM:
            if (psl & PSL_WC)        sprintf(ENDOF(mn), "SUBB     %s,%s", reg, addressstring);
            else                     sprintf(ENDOF(mn), "SUB      %s,%s", reg, addressstring);
        acase STYLE_NEWCALM:
            if (psl & PSL_WC)        sprintf(ENDOF(mn), "SUBC     %s,%s", addressstring, reg);
            else                     sprintf(ENDOF(mn), "SUB      %s,%s", addressstring, reg);
        acase STYLE_IEEE:
            if (psl & PSL_WC)        sprintf(ENDOF(mn), "SUBC   %s,%s"  , reg, addressstring);
            else                     sprintf(ENDOF(mn), "SUB    %s,%s"  , reg, addressstring);
        adefault:
                                     sprintf(ENDOF(mn), "SUBR,%s %s"    , reg, addressstring);
        }                                                                              sprintf(an, "%s -= *(%s);"   , reg, pseudostring);
        ccmode = CCMODE_ARITHMETIC;
    acase 0xAC:
    case 0xAD:
    case 0xAE:
    case 0xAF:
        ABS_EA_TRACE();
        switch (style)
        {
        case  STYLE_NEWCALM:
            if (psl & PSL_WC)        sprintf(ENDOF(mn), "SUBB     %s,%s", reg, addressstring);
            else                     sprintf(ENDOF(mn), "SUB      %s,%s", reg, addressstring);
        acase STYLE_OLDCALM:
            if (psl & PSL_WC)        sprintf(ENDOF(mn), "SUBC     %s,%s", addressstring, reg);
            else                     sprintf(ENDOF(mn), "SUB      %s,%s", addressstring, reg);
        acase STYLE_IEEE:
            if (psl & PSL_WC)        sprintf(ENDOF(mn), "SUBC   %s,%s"  , reg, addressstring);
            else                     sprintf(ENDOF(mn), "SUB    %s,%s"  , reg, addressstring);
        adefault:
                                     sprintf(ENDOF(mn), "SUBA,%s %s"    , reg, addressstring);
        }                                                                              sprintf(an, "%s -= *(%s);"   , reg, pseudostring);
        ccmode = CCMODE_ARITHMETIC;
    acase 0xB0:
    case 0xB1:
    case 0xB2:
    case 0xB3:
        switch (style)
        {
        case  STYLE_OLDCALM:         sprintf(ENDOF(mn), "LOAD     $CTRL,%s", reg);
        acase STYLE_NEWCALM:         sprintf(ENDOF(mn), "MOVE     %s,$CTRL", reg);
        acase STYLE_IEEE:            sprintf(ENDOF(mn), "OUT    %s,CTRL"   , reg);
        adefault:                    sprintf(ENDOF(mn), "WRTC,%s"          , reg);
        }                                                                              sprintf(an, "IOPORT(CTRL) = %s;", reg);
    acase 0xB4:
        dec_to_bin(OPERAND);
        dec_to_hex(OPERAND);
        switch (style)
        {
        case STYLE_OLDCALM:
        case STYLE_NEWCALM:
            switch (OPERAND)
            {
            case  0x20:              strcat(       mn , "TEST     IOF");
            acase 0x40:              strcat(       mn , "TEST     OUTPUT");
            acase 0x80:              strcat(       mn , "TEST     INPUT");
            adefault:                if (style == STYLE_OLDCALM)
                                     {   sprintf(ENDOF(mn), "TEST     U,#%s", v);
                                     } else
                                     {   sprintf(ENDOF(mn), "TEST     PSU:#%s", v);
            }                        }
            if (style == STYLE_OLDCALM)                                                sprintf(an, "CC = (U & %s == %s) ? EQ : LT;", v_bin, v_bin);
            else                                                                       sprintf(an, "CC = (PSU & %s == %s) ? EQ : LT;", v_bin, v_bin);
        acase STYLE_IEEE:            sprintf(ENDOF(mn), "TEST   .U,#%s", v);           sprintf(an, ".CC = (.U & %s == %s) ? EQ : LT;", v_bin, v_bin);
        adefault:
            strcat(mn, "TPSU    ");
            if ((OPERAND & 0x1F) == 0x00)
            {   started = FALSE;
                if ((OPERAND & 0x80) == 0x80) {                               strcat(mn, "S"  ); started = TRUE; }
                if ((OPERAND & 0x40) == 0x40) { if (started) strcat(mn, "+"); strcat(mn, "F"  ); started = TRUE; }
                if ((OPERAND & 0x20) == 0x20) { if (started) strcat(mn, "+"); strcat(mn, "II" );                 }
            } else
            {                        strcat(       mn , v);
            }                                                                          sprintf(an, "CC = (PSU & %s == %s) ? EQ : LT;", v_bin, v_bin);
        }

        if (OPERAND & 0x80) sprintf(ENDOF(en), ";Test %s (%s) bit\n", flagname[NAME_S ].shorter[style], flagname[NAME_S ].longer[style]);
        if (OPERAND & 0x40) sprintf(ENDOF(en), ";Test %s (%s) bit\n", flagname[NAME_F ].shorter[style], flagname[NAME_F ].longer[style]);
        if (OPERAND & 0x20) sprintf(ENDOF(en), ";Test %s (%s) bit\n", flagname[NAME_II].shorter[style], flagname[NAME_II].longer[style]);
        if (supercpu)
        {   if (OPERAND & 0x10) strcat(en, ";Test User Flag #1\n");
            if (OPERAND &  0x8) strcat(en, ";Test User Flag #2\n");
        }
        if (OPERAND &  0x7)
        {   if ((OPERAND & 0x7) == 0x7)
            {   sprintf(    ENDOF(en), ";Test %s (%s)!\n",               flagname[NAME_SP].shorter[style], flagname[NAME_SP].longer[style]);
            } else
            {   if ((OPERAND & 0x4) == 0x4)
                {   sprintf(ENDOF(en), ";Test high bit of %s (%s)!\n",   flagname[NAME_SP].shorter[style], flagname[NAME_SP].longer[style]);
                }
                if ((OPERAND & 0x2) == 0x2)
                {   sprintf(ENDOF(en), ";Test middle bit of %s (%s)!\n", flagname[NAME_SP].shorter[style], flagname[NAME_SP].longer[style]);
                }
                if ((OPERAND & 0x1) == 0x1)
                {   sprintf(ENDOF(en), ";Test low bit of %s (%s)!\n",    flagname[NAME_SP].shorter[style], flagname[NAME_SP].longer[style]);
        }   }   }

        ccmode = CCMODE_TEST;
    acase 0xB5:
        dec_to_bin(OPERAND);
        dec_to_hex(OPERAND);
        switch (style)
        {
        case STYLE_OLDCALM:
        case STYLE_NEWCALM:
            switch (OPERAND)
            {
            case  0x01:                     strcat(mn , "TEST     CARRY");
            acase 0x02:                     strcat(mn , "TEST     LOGICOMP");
            acase 0x04:                     strcat(mn , "TEST     OVERFLOW");
            acase 0x08:                     strcat(mn , "TEST     WITHCARRY");
            acase 0x10:                     strcat(mn , "TEST     BANK"); // BANK1 is acceptable as a synonym of BANK
            acase 0x20:                     strcat(mn , "TEST     HALFCARRY");
            adefault:
                if (style == STYLE_OLDCALM)
                {                    sprintf(ENDOF(mn), "TEST     L,#%s", v);
                } else
                {                    sprintf(ENDOF(mn), "TEST     PSL:#%s", v);
            }   }
            if (style == STYLE_OLDCALM)
            {                                                                                  sprintf(an, "CC = (L & %s == %s) ? EQ : LT;", v_bin, v_bin);
            } else
            {                                                                                  sprintf(an, "CC = (PSL & %s == %s) ? EQ : LT;", v_bin, v_bin);
            }
        acase STYLE_IEEE:            sprintf(ENDOF(mn), "TEST   .U,#%s", v);                   sprintf(an, ".CC = (.U & %s == %s) ? EQ : LT;", v_bin, v_bin);
        adefault:
            strcat(mn, "TPSL    ");
            if ((OPERAND & 0xC0) == 0x00)
            {   started = FALSE;
                if ((OPERAND & 0x20) == 0x20) {                               strcat(mn, "IDC"); started = TRUE; }
                if ((OPERAND & 0x10) == 0x10) { if (started) strcat(mn, "+"); strcat(mn, "RS" ); started = TRUE; }
                if ((OPERAND & 0x08) == 0x08) { if (started) strcat(mn, "+"); strcat(mn, "WC" ); started = TRUE; }
                if ((OPERAND & 0x04) == 0x04) { if (started) strcat(mn, "+"); strcat(mn, "OVF"); started = TRUE; }
                if ((OPERAND & 0x02) == 0x02) { if (started) strcat(mn, "+"); strcat(mn, "COM"); started = TRUE; }
                if ((OPERAND & 0x01) == 0x01) { if (started) strcat(mn, "+"); strcat(mn, "C"  );                 }
            } else
            {                               strcat(mn , v);
            }                                                                                  sprintf(an, "CC = (PSL & %s == %s) ? EQ : LT;", v_bin, v_bin);
        }

        if (OPERAND & 0xC0)
        {   switch (OPERAND & 0xC0)
            {
            case  0xC0: strcat(en, ";Test CC (Condition Code)!\n");
            acase 0x80: strcat(en, ";Test high bit of CC (Condition Code)!\n");
            acase 0x40: strcat(en, ";Test low bit of CC (Condition Code)!\n");
        }   }
        if (OPERAND & 0x20) sprintf(ENDOF(en), ";Test %s (%s) bit\n", flagname[NAME_IDC].shorter[style], flagname[NAME_IDC].longer[style]);
        if (OPERAND & 0x10) sprintf(ENDOF(en), ";Test %s (%s) bit\n", flagname[NAME_RS ].shorter[style], flagname[NAME_RS ].longer[style]);
        if (OPERAND &  0x8) sprintf(ENDOF(en), ";Test %s (%s) bit\n", flagname[NAME_WC ].shorter[style], flagname[NAME_WC ].longer[style]);
        if (OPERAND &  0x4) sprintf(ENDOF(en), ";Test %s (%s) bit\n", flagname[NAME_OVF].shorter[style], flagname[NAME_OVF].longer[style]);
        if (OPERAND &  0x2) sprintf(ENDOF(en), ";Test %s (%s) bit\n", flagname[NAME_COM].shorter[style], flagname[NAME_COM].longer[style]);
        if (OPERAND &  0x1) sprintf(ENDOF(en), ";Test %s (%s) bit\n", flagname[NAME_C  ].shorter[style], flagname[NAME_C  ].longer[style]);
    acase 0xB8:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM:
        case  STYLE_NEWCALM:  sprintf(ENDOF(mn), "CALL,NE  %s", addressstring);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "CALLNE %s"  , addressstring);
        adefault:             sprintf(ENDOF(mn), "BSFR,EQ %s" , addressstring);
        }                                                                                 sprintf(an, "%sgosub %s;", ccfalse[style][BRANCHCODE], pseudostring);
    acase 0xB9:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM:
        case  STYLE_NEWCALM:  sprintf(ENDOF(mn), "CALL,LE  %s", addressstring);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "CALLLE %s"  , addressstring);
        adefault:             sprintf(ENDOF(mn), "BSFR,GT %s" , addressstring);
        }                                                                                 sprintf(an, "%sgosub %s;", ccfalse[style][BRANCHCODE], pseudostring);
    acase 0xBA:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM:
        case  STYLE_NEWCALM:  sprintf(ENDOF(mn), "CALL,GE  %s", addressstring);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "CALLGE %s"  , addressstring);
        adefault:             sprintf(ENDOF(mn), "BSFR,LT %s" , addressstring);
        }                                                                                 sprintf(an, "%sgosub %s;", ccfalse[style][BRANCHCODE], pseudostring);
    acase 0xBB:
        ZERO_EA_TRACE(TRUE);
        switch (style)
        {
        case  STYLE_OLDCALM:
        case  STYLE_NEWCALM:  sprintf(ENDOF(mn), "CALL     %s", addressstring);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "CALL   %s"  , addressstring);
        adefault:             sprintf(ENDOF(mn), "ZBSR    %s" , addressstring); // or "ZBSR,un "
        }                                                                                 sprintf(an, "gosub %s;"  , pseudostring);
    acase 0xBC:
        BRA_EA_TRACE(TRUE);
        switch (style)
        {
        case  STYLE_OLDCALM:
        case  STYLE_NEWCALM:  sprintf(ENDOF(mn), "CALL,NE  %s", addressstring);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "CALLNE %s"  , addressstring);
        adefault:             sprintf(ENDOF(mn), "BSFA,EQ %s" , addressstring);
        }                                                                                 sprintf(an, "%sgosub %s;", ccfalse[style][BRANCHCODE], pseudostring);
    acase 0xBD:
        BRA_EA_TRACE(TRUE);
        switch (style)
        {
        case  STYLE_OLDCALM:
        case  STYLE_NEWCALM:  sprintf(ENDOF(mn), "CALL,LE  %s", addressstring);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "CALLLE %s"  , addressstring);
        adefault:             sprintf(ENDOF(mn), "BSFA,GT %s" , addressstring);
        }                                                                                 sprintf(an, "%sgosub %s;", ccfalse[style][BRANCHCODE], pseudostring);
    acase 0xBE:
        BRA_EA_TRACE(TRUE);
        switch (style)
        {
        case  STYLE_OLDCALM:
        case  STYLE_NEWCALM:  sprintf(ENDOF(mn), "CALL,GE  %s", addressstring);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "CALLGE %s"  , addressstring);
        adefault:             sprintf(ENDOF(mn), "BSFA,LT %s" , addressstring);
        }                                                                                 sprintf(an, "%sgosub %s;", ccfalse[style][BRANCHCODE], pseudostring);
    acase 0xBF:
        BRA_EA_TRACE(FALSE);
        switch (style)
        {
        case  STYLE_OLDCALM:  sprintf(ENDOF(mn), "CALL     (%s)+%s", reg, addressstring);
        acase STYLE_NEWCALM:  sprintf(ENDOF(mn), "CALL     U^{%s}+%s", reg, &addressstring[2]);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "CALL   %s(%s)"   , addressstring, reg);
        adefault:             sprintf(ENDOF(mn), "BSXA,%s %s"      , reg, addressstring);
        }                                                                                 sprintf(an, "gosub %s + %s;", pseudostring, reg);
    acase 0xC0:               strcat(       mn , "NOP");                                  strcpy( an, ";");
    acase 0xC1:
    case 0xC2:
    case 0xC3:
        switch (style)
        {
        case  STYLE_OLDCALM:  sprintf(ENDOF(mn), "LOAD     %s,A" , reg);                  sprintf(an, "%s = A;" , reg);
        acase STYLE_NEWCALM:  sprintf(ENDOF(mn), "MOVE     r0,%s", reg);                  sprintf(an, "%s = r0;", reg);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "MOV    %s,.0"  , reg);                  sprintf(an, "%s = .0;", reg);
        adefault:             sprintf(ENDOF(mn), "STRZ    %s"    , reg);                  sprintf(an, "%s = r0;", reg);
        }
        ccmode = CCMODE_ARITHMETIC;
    acase 0xC8:
    case 0xC9:
    case 0xCA:
    case 0xCB:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM:  sprintf(ENDOF(mn), "LOAD     %s,%s", addressstring, reg);
        acase STYLE_NEWCALM:  sprintf(ENDOF(mn), "MOVE     %s,%s", reg, addressstring);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "ST     %s,%s"  , reg, addressstring);
        adefault:             sprintf(ENDOF(mn), "STRR,%s %s"    , reg, addressstring);
        }                                                                                 sprintf(an, "*(%s) = %s;", pseudostring, reg);
    acase 0xCC:
    case 0xCD:
    case 0xCE:
    case 0xCF:
        ABS_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM:  sprintf(ENDOF(mn), "LOAD     %s,%s", addressstring, reg);
        acase STYLE_NEWCALM:  sprintf(ENDOF(mn), "MOVE     %s,%s", reg, addressstring);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "ST     %s,%s"  , reg, addressstring);
        adefault:             sprintf(ENDOF(mn), "STRA,%s %s"    , reg, addressstring);
        }                                                                                 sprintf(an, "*(%s) = %s;", pseudostring, reg);
    acase 0xD0:
    case 0xD1:
    case 0xD2:
    case 0xD3:
        switch (style)
        {
        case STYLE_OLDCALM:
        case STYLE_NEWCALM:
            if (psl & PSL_WC)
            {   if (psl & PSL_C)
                {             sprintf(ENDOF(mn), "RLC      %s", reg);
                } else
                {             sprintf(ENDOF(mn), "SL       %s", reg); // or ASL
            }   }
            else              sprintf(ENDOF(mn), "RL       %s", reg);
        acase STYLE_IEEE:
            if (psl & PSL_WC)
            {   if (psl & PSL_C)
                {             sprintf(ENDOF(mn), "ROLC   %s"  , reg);
                } else
                {             sprintf(ENDOF(mn), "SHL    %s"  , reg); // or SHLA
            }   }
            else              sprintf(ENDOF(mn), "ROL    %s"  , reg);
        adefault:             sprintf(ENDOF(mn), "RRL,%s"     , reg);
        }                                                                                 sprintf(an, "%s <<= 1;", reg);
        ccmode = CCMODE_ARITHMETIC;
    acase 0xD4:
    case 0xD5:
    case 0xD6:
    case 0xD7:
        dec_to_hex(OPERAND);
        switch (style)
        {
        case  STYLE_OLDCALM:  sprintf(ENDOF(mn), "LOAD     $%s,%s", v, reg);
        acase STYLE_NEWCALM:  sprintf(ENDOF(mn), "MOVE     %s,$%s", reg, v);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "OUT    %s,%s"   , reg, v);
        adefault:             sprintf(ENDOF(mn), "WRTE,%s %s"     , reg, v);
        }                                                                                 sprintf(an, "IOPORT(%s) = %s;", v_pse, reg);
    acase 0xD8:
    case 0xD9:
    case 0xDA:
    case 0xDB:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM:
            if (OPERAND == 0) sprintf(ENDOF(mn), "INC      %s"   , reg);
            else              sprintf(ENDOF(mn), "INCJ,NE  %s,%s", reg, addressstring);
        acase STYLE_NEWCALM:
            if (OPERAND == 0) sprintf(ENDOF(mn), "INC      %s"   , reg);
            else              sprintf(ENDOF(mn), "IJ,NE    %s,%s", reg, addressstring);
        acase STYLE_IEEE:
            if (OPERAND == 0) sprintf(ENDOF(mn), "INC    %s"     , reg);
            else              sprintf(ENDOF(mn), "IBNZ   %s,%s"  , reg, addressstring);
        adefault:             sprintf(ENDOF(mn), "BIRR,%s %s"    , reg, addressstring);
        }
        if (OPERAND == 0)                                                                 sprintf(an, "%s++;", reg);
        else                                                                              sprintf(an, "if (++%s != 0) goto %s;", reg, pseudostring);
    acase 0xDC:
    case 0xDD:
    case 0xDE:
    case 0xDF:
        BRA_EA_TRACE(TRUE);
        switch (style)
        {
        case STYLE_OLDCALM:
         /* if (OPERAND == 0) sprintf(ENDOF(mn), "INC      %s"   , reg); commented out so our disassemblies can be equivalently reassembled
            else */           sprintf(ENDOF(mn), "INCJ,NE  %s,%s", reg, addressstring);
        acase STYLE_NEWCALM:
         /* if (OPERAND == 0) sprintf(ENDOF(mn), "INC      %s"   , reg); commented out so our disassemblies can be equivalently reassembled
            else */           sprintf(ENDOF(mn), "IJ,NE    %s,%s", reg, addressstring);
        acase STYLE_IEEE:
         /* if (OPERAND == 0) sprintf(ENDOF(mn), "INC    %s"     , reg); commented out so our disassemblies can be equivalently reassembled
            else */           sprintf(ENDOF(mn), "IBNZ   %s,%s"  , reg, addressstring);
        adefault:             sprintf(ENDOF(mn), "BIRA,%s %s"    , reg, addressstring);
        }
        if (OPERAND == 0)                                                                 sprintf(an, "%s++;"                  , reg);
        else                                                                              sprintf(an, "if (++%s != 0) goto %s;", reg, pseudostring);
    acase 0xE0:
    case 0xE1:
    case 0xE2:
    case 0xE3:
        switch (style)
        {
        case  STYLE_OLDCALM:  sprintf(ENDOF(mn), "COMP     A,%s", reg);                   sprintf(an, "compare A against %s;" , reg);
        acase STYLE_NEWCALM:  sprintf(ENDOF(mn), "COMP     %s,r0", reg);                  sprintf(an, "compare r0 against %s;", reg);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "CMP    .0,%s" , reg);                   sprintf(an, "compare .0 against %s;", reg);
        adefault:             sprintf(ENDOF(mn), "COMZ    %s"   , reg);                   sprintf(an, "compare r0 against %s;", reg);
        }
        ccmode = CCMODE_COMPARE;
    acase 0xE4:
    case 0xE5:
    case 0xE6:
    case 0xE7:
        dec_to_hex(OPERAND);
        switch (style)
        {
        case  STYLE_OLDCALM:  sprintf(ENDOF(mn), "COMP     %s,#%s", reg, v);
        acase STYLE_NEWCALM:  sprintf(ENDOF(mn), "COMP     #%s,%s", v, reg);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "CMP    %s,#%s"  , reg, v);
        adefault:             sprintf(ENDOF(mn), "COMI,%s %s"     , reg, v);
        }                                                                                 sprintf(an, "compare %s against %s", reg, v_pse);
                                                                                          if (ISQWERTY)
                                                                                          {   sprintf(ENDOF(an), " [%s]", asciiname_short[OPERAND]);
                                                                                          } elif (machine != INTERTON)
                                                                                          {   sprintf(ENDOF(an), " ['%c']", guestchar(OPERAND));
                                                                                          }
        ccmode = CCMODE_COMPARE;
    acase 0xE8:
    case 0xE9:
    case 0xEA:
    case 0xEB:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM:  sprintf(ENDOF(mn), "COMP     %s,%s", reg, addressstring);
        acase STYLE_NEWCALM:  sprintf(ENDOF(mn), "COMP     %s,%s", addressstring, reg);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "CMP    %s,%s"  , reg, addressstring);
        adefault:             sprintf(ENDOF(mn), "COMR,%s %s"    , reg, addressstring);
        }                                                                                 sprintf(an, "compare %s against *(%s)", reg, pseudostring);
        ccmode = CCMODE_COMPARE;
    acase 0xEC:
    case 0xED:
    case 0xEE:
    case 0xEF:
        ABS_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM:  sprintf(ENDOF(mn), "COMP     %s,%s", reg, addressstring);
        acase STYLE_NEWCALM:  sprintf(ENDOF(mn), "COMP     %s,%s", addressstring, reg);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "CMP    %s,%s"  , reg, addressstring);
        adefault:             sprintf(ENDOF(mn), "COMA,%s %s"    , reg, addressstring);
        }                                                                                 sprintf(an, "compare %s against *(%s)", reg, pseudostring);
        ccmode = CCMODE_COMPARE;
    acase 0xF0:
    case 0xF1:
    case 0xF2:
    case 0xF3:
        switch (style)
        {
        case  STYLE_OLDCALM:  sprintf(ENDOF(mn), "LOAD     $DATA,%s", reg);
        acase STYLE_NEWCALM:  sprintf(ENDOF(mn), "MOVE     %s,$DATA", reg);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "OUT    %s,DATA"   , reg);
        adefault:             sprintf(ENDOF(mn), "WRTD,%s"          , reg);
        }                                                                                 sprintf(an, "IOPORT(DATA) = %s;", reg);
    acase 0xF4:
    case 0xF5:
    case 0xF6:
    case 0xF7:
        dec_to_hex(OPERAND);
        dec_to_bin(OPERAND);
        switch (style)
        {
        case  STYLE_OLDCALM:  sprintf(ENDOF(mn), "TEST     %s,#%s", reg, v);              sprintf(an, "CC = (%s & %s == %s) ? EQ : LT;" , reg, v_bin, v_bin);
        acase STYLE_NEWCALM:  sprintf(ENDOF(mn), "TEST     %s:#%s", reg, v);              sprintf(an, "CC = (%s & %s == %s) ? EQ : LT;" , reg, v_bin, v_bin);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "TEST   %s,#%s"  , reg, v);              sprintf(an, ".CC = (%s & %s == %s) ? EQ : LT;", reg, v_bin, v_bin);
        adefault:             sprintf(ENDOF(mn), "TMI,%s %s"      , reg, v);              sprintf(an, "CC = (%s & %s == %s) ? EQ : LT;" , reg, v_bin, v_bin);
        }
        ccmode = CCMODE_TEST;
    acase 0xF8:
    case 0xF9:
    case 0xFA:
    case 0xFB:
        REL_EA_TRACE();
        switch (style)
        {
        case  STYLE_OLDCALM:
            if (OPERAND == 0) sprintf(ENDOF(mn), "DEC      %s"   , reg);
            else              sprintf(ENDOF(mn), "DECJ,NE  %s,%s", reg, addressstring);
        acase STYLE_NEWCALM:
            if (OPERAND == 0) sprintf(ENDOF(mn), "DEC      %s"   , reg);
            else              sprintf(ENDOF(mn), "DJ,NE    %s,%s", reg, addressstring);
        acase STYLE_IEEE:
            if (OPERAND == 0) sprintf(ENDOF(mn), "DEC    %s"     , reg);
            else              sprintf(ENDOF(mn), "DBNZ   %s,%s"  , reg, addressstring);
        adefault:             sprintf(ENDOF(mn), "BDRR,%s %s"    , reg, addressstring);
        }
        if (OPERAND == 0)                                                                 sprintf(an, "%s--;"                  , reg);
        else                                                                              sprintf(an, "if (--%s != 0) goto %s;", reg, pseudostring);
    acase 0xFC:
    case 0xFD:
    case 0xFE:
    case 0xFF:
        BRA_EA_TRACE(TRUE);
        switch (style)
        {
        case  STYLE_OLDCALM:
         /* if (OPERAND == 0) sprintf(ENDOF(mn), "DEC      %s"   , reg); commented out so our disassemblies can be equivalently reassembled
            else */           sprintf(ENDOF(mn), "DECJ,NE  %s,%s", reg, addressstring);
        acase STYLE_NEWCALM:
         /* if (OPERAND == 0) sprintf(ENDOF(mn), "DEC      %s"   , reg); commented out so our disassemblies can be equivalently reassembled
            else */           sprintf(ENDOF(mn), "DJ,NE    %s,%s", reg, addressstring);
        acase STYLE_IEEE:
         /* if (OPERAND == 0) sprintf(ENDOF(mn), "DEC    %s"     , reg); commented out so our disassemblies can be equivalently reassembled
            else */           sprintf(ENDOF(mn), "DBNZ   %s,%s"  , reg, addressstring);
        adefault:             sprintf(ENDOF(mn), "BDRA,%s %s"    , reg, addressstring);
        }
        if (OPERAND == 0)                                                                 sprintf(an, "%s--;"                  , reg);
        else                                                                              sprintf(an, "if (--%s != 0) goto %s;", reg, pseudostring);
    adefault: // $90, $91, $B6, $B7, $C4..$C7
        switch (style)
        {
        case  STYLE_OLDCALM:
        case  STYLE_NEWCALM:  sprintf(ENDOF(mn), "DATA     16'%02X", opcode);
        acase STYLE_IEEE:     sprintf(ENDOF(mn), "DATA   H'%02X"  , opcode);
        adefault:             sprintf(ENDOF(mn), "DB      $%02X" , opcode);
        }                                                                                 strcpy( an, "?");
        ccmode = CCMODE_UNDEFINED;
    }

    if (!full)
    {   return;
    }

    duration = table_duration_2650[opcode];
    maybe = FALSE;
    if ((opcode & 0x0F) >= 8 && (OPERAND & 0x80))
    {   // indirect
        if
        (   (opcode & 0x10) == 0
         ||  opcode         == 0x1B // BCTR,un %00011011
         ||  opcode         == 0x1F // BCTA,un %00011111
         ||  opcode         == 0x3B // BSTR,un %00111011
         ||  opcode         == 0x3F // BSTA,un %00111111
         ||  opcode         == 0x9B // ZBRR    %10011011
         ||  opcode         == 0x9F // BXA,r3  %10011111
         ||  opcode         == 0xBB // ZBSR    %10111011
         ||  opcode         == 0xBF // BSXA,un %10111111
        )
        {   // unconditional
            duration += 6;
        } else
        {   // conditional
            maybe = TRUE;
    }   }

    length = strlen(mn);
    if (disassembling || verbosity == 0)
    {   for (i = length; i <= 56; i++)
        {   mn2[i - length] = ' ';
        }
        mn2[57 - length] = ';';
        mn2[58 - length] = EOS;
    } else
    {   for (i = length; i <= 42; i++)
        {   mn2[i - length] = ' ';
        }
        mn2[43 - length] = ';';
        longcomma(scale_time(cycles_2650), &mn2[44 - length]);
        mn2[57 - length] = ' ';
        mn2[58 - length] = EOS;
    }

    switch (timeunit)
    {
    case TIMEUNIT_CYCLES:
        if (maybe)
        {   sprintf(ENDOF(mn2), "2+%d,%d $%04X ",  duration / 3                    , table_size_2650[opcode], iar);
        } else
        {   sprintf(ENDOF(mn2), "  %d,%d $%04X ",  duration / 3                    , table_size_2650[opcode], iar);
        }
    acase TIMEUNIT_CLOCKS:
        if (maybe)
        {   sprintf(ENDOF(mn2), "6+%d,%d $%04X ",  duration                        , table_size_2650[opcode], iar); // there are no 6+12 cycle instructions
        } else
        {   sprintf(ENDOF(mn2), " %2d,%d $%04X ",  duration                        , table_size_2650[opcode], iar);
        }
    acase TIMEUNIT_PIXELS:
        if (machine == CD2650)
        {   if (maybe)
            {   sprintf(ENDOF(mn2), "%2d+%3d,%d $%04X ", 6 * ppc, duration * ppc, table_size_2650[opcode], iar);
            } else
            {   sprintf(ENDOF(mn2),  "   %3d,%d $%04X ",          duration * ppc, table_size_2650[opcode], iar);
        }   }
        else
        {   if (maybe)
            {   sprintf(ENDOF(mn2), "%2d+%2d,%d $%04X ", 6 * ppc, duration * ppc, table_size_2650[opcode], iar);
            } else
            {   sprintf(ENDOF(mn2),  "   %2d,%d $%04X ",          duration * ppc, table_size_2650[opcode], iar);
    }   }   }

    hex1(byte1, opcode);
    if (table_size_2650[opcode] == 3)
    {   hex1(byte2, OPERAND);
        hex1(byte3, memory[WRAPMEM(2)]);
        sprintf(ENDOF(mn2), "%s %s %s", byte1, byte2, byte3);
    } elif (table_size_2650[opcode] == 2)
    {   hex1(byte2, OPERAND);
        sprintf(ENDOF(mn2), "%s %s   ", byte1, byte2);
    } else
    {   // assert(table_size_2650[opcode] == 1);
        sprintf(ENDOF(mn2), "%s      ", byte1);
}   }

MODULE void ABS_EA_TRACE(void)
{   addr = tea = (iar & PAGE) + (((OPERAND << 8) + memory[iar + 2]) & NONPAGE); // what about mirroring?

    if (OPERAND & 0x80)
    {   strcpy(pseudostring, "*");
    } else
    {   pseudostring[0] = EOS;
    }
    forcedollar = TRUE;
    number_to_friendly(tea, ENDOF(pseudostring), FALSE, 0);
    forcedollar = FALSE;
    switch (OPERAND & 0x60)
    {
    case  0x20: sprintf(ENDOF(pseudostring), " + ++%s", reg);
    acase 0x40: sprintf(ENDOF(pseudostring), " + --%s", reg);
    acase 0x60: sprintf(ENDOF(pseudostring), " + %s"  , reg);
    }

    switch (style)
    {
    case STYLE_OLDCALM: // eg. (-B)+@1234
        switch (OPERAND & 0x60)
        {
        case  0x00: addressstring[0] = EOS;
        acase 0x20: sprintf(addressstring, "(+%s)+", reg);
        acase 0x40: sprintf(addressstring, "(-%s)+", reg);
        acase 0x60: sprintf(addressstring, "(%s)+" , reg);
        }
        if (OPERAND & 0x80)
        {   strcat(addressstring, "@");
        }
        number_to_friendly(tea, ENDOF(addressstring), FALSE, 0);
    acase STYLE_NEWCALM: // eg. U^{-r1}+{1234}
        switch (OPERAND & 0x60)
        {
        case  0x00: sprintf(addressstring, "U^"           );
        acase 0x20: sprintf(addressstring, "U^{+%s}+", reg);
        acase 0x40: sprintf(addressstring, "U^{-%s}+", reg);
        acase 0x60: sprintf(addressstring, "U^{%s}+" , reg);
        }
        if (OPERAND & 0x80)
        {   strcat(addressstring, "{");
        }
        number_to_friendly(tea, ENDOF(addressstring), FALSE, 0);
        if (OPERAND & 0x80)
        {   strcat(addressstring, "}");
        }
    acase STYLE_IEEE: // eg. */1234(.1)
        if (OPERAND & 0x80)
        {   strcpy(addressstring, "/@");
        } else
        {   strcpy(addressstring, "/");
        }
        number_to_friendly(tea, ENDOF(addressstring), FALSE, 0);
        switch (OPERAND & 0x60)
        {
        case  0x20: sprintf(ENDOF(addressstring), "(+%s)" , reg);
        acase 0x40: sprintf(ENDOF(addressstring), "(-%s)" , reg);
        acase 0x60: sprintf(ENDOF(addressstring), "(%s)" , reg);
        }
    adefault: // eg. *1234,r1-
        if (OPERAND & 0x80)
        {   strcat(addressstring, "*");
        } else
        {   addressstring[0] = EOS;
        }
        number_to_friendly(tea, ENDOF(addressstring), FALSE, 0);
        switch (OPERAND & 0x60)
        {
        case  0x20: sprintf(ENDOF(addressstring), ",%s+", reg);
        acase 0x40: sprintf(ENDOF(addressstring), ",%s-", reg);
        acase 0x60: sprintf(ENDOF(addressstring), ",%s" , reg);
    }   }

    if (OPERAND & 0x80) // indirect
    {   tea = memory[addr] << 8;
        if ((++addr & NONPAGE) == 0)
        {   addr -= PLEN; /* page wrap */
        }
        tea = (tea + memory[addr]) & AMSK;
    }

    switch (OPERAND & 0x60)
    {
    case  0x20: tea = (tea & PAGE) + ((tea + r[rr] + 1) & NONPAGE);
    acase 0x40: tea = (tea & PAGE) + ((tea + r[rr] - 1) & NONPAGE);
    acase 0x60: tea = (tea & PAGE) + ((tea + r[rr]    ) & NONPAGE);
    }

    if (!disassembling && (OPERAND & 0xE0)) // indirect and/or indexed
    {   number_to_friendly(tea, (STRPTR) friendly, FALSE, 0);
        sprintf(ENDOF(addressstring), " [%s]", friendly);

        forcedollar = TRUE;
        number_to_friendly(tea, (STRPTR) friendly, FALSE, 0);
        forcedollar = FALSE;
        sprintf(ENDOF(pseudostring ), " [%s]", friendly);
    }

    if (OPERAND & 0x60)
    {   // rr = 0;
        switch (style)
        {
        case  STYLE_OLDCALM: reg[0] = 'A'; reg[1] = EOS;
        acase STYLE_IEEE:    reg[0] = '.'; reg[1] = '0'; reg[2] = EOS;
        adefault:            reg[0] = 'r'; reg[1] = '0'; reg[2] = EOS;
} } }

MODULE void REL_EA_TRACE(void)
{   tea = (iar & PAGE) + ((iar + 2 + relative_2650[OPERAND]) & NONPAGE); // what about mirroring?

    if (OPERAND & 0x80)
    {   switch (style)
        {
        case  STYLE_OLDCALM: strcpy(addressstring, ".+@");
        acase STYLE_NEWCALM: strcpy(addressstring, "R^{");
        acase STYLE_IEEE:    strcpy(addressstring, "$@" );
        adefault:            strcpy(addressstring, "*"  );
        }
        number_to_friendly(tea, (STRPTR) friendly, FALSE, 0);
        strcat(addressstring, friendly);
        if (style == STYLE_NEWCALM)
        {   strcat(addressstring, "}");
        }

        strcpy(pseudostring, "*");
        forcedollar = TRUE;
        number_to_friendly(tea, (STRPTR) friendly, FALSE, 0);
        forcedollar = FALSE;
        strcat(pseudostring, friendly);

        addr = (int) tea;
        tea = memory[addr] << 8;
        if ((++addr & NONPAGE) == 0)
        {   addr -= PLEN; /* page wrap */
        }
        tea = (tea + memory[addr]) & AMSK;

        number_to_friendly(tea, (STRPTR) friendly, FALSE, 0);
        sprintf(ENDOF(addressstring), " [%s]", friendly);

        forcedollar = TRUE;
        number_to_friendly(tea, (STRPTR) friendly, FALSE, 0);
        forcedollar = FALSE;
        sprintf(ENDOF(pseudostring ), " [%s]", friendly);
    } else
    {   switch (style)
        {
        case  STYLE_OLDCALM: strcpy(addressstring, ".+");
        acase STYLE_NEWCALM: strcpy(addressstring, "R^");
        acase STYLE_IEEE:    strcpy(addressstring, "$" );
        adefault:            addressstring[0] = EOS;
        }

        number_to_friendly(tea, (STRPTR) friendly, FALSE, 0);
        strcat(addressstring, friendly);

        forcedollar = TRUE;
        number_to_friendly(tea, (STRPTR) friendly, FALSE, 0);
        forcedollar = FALSE;
        strcpy(pseudostring, friendly);
}   }

MODULE void BRA_EA_TRACE(FLAG full)
{   tea = ((OPERAND << 8) + memory[WRAPMEM(2)]) & AMSK; // what about mirroring?

    if (OPERAND & 0x80) // indirect addressing?
    {   switch (style)
        {
        case  STYLE_OLDCALM: strcpy(addressstring, "@"  );
        acase STYLE_NEWCALM: strcpy(addressstring, "U^{");
        acase STYLE_IEEE:    strcpy(addressstring, "/@" );
        adefault:            strcpy(addressstring, "*"  );
        }
        number_to_friendly(tea, friendly, FALSE, 0);
        strcat(addressstring, friendly);
        if (style == STYLE_NEWCALM)
        {   strcat(addressstring, "}");
        }

        strcpy(pseudostring, "*");
        forcedollar = TRUE;
        number_to_friendly(tea, friendly, FALSE, 0);
        forcedollar = FALSE;
        strcat(pseudostring, friendly);

        addr = (int) tea;
        tea = memory[addr] << 8;
        if ((++addr & NONPAGE) == 0)
        {   addr -= PLEN; /* page wrap */
        }
        tea = (tea + memory[addr]) & AMSK;

        if (full)
        {   number_to_friendly(tea, (STRPTR) friendly, FALSE, 0);
            sprintf(ENDOF(addressstring), " [%s]", friendly);

            forcedollar = TRUE;
            number_to_friendly(tea, (STRPTR) friendly, FALSE, 0);
            forcedollar = FALSE;
            sprintf(ENDOF(pseudostring ), " [%s]", friendly);
    }   }
    else
    {   switch (style)
        {
        case  STYLE_NEWCALM: strcpy(addressstring, "U^");
        acase STYLE_IEEE:    strcpy(addressstring, "/" );
        adefault:            addressstring[0] = EOS;
        }
        number_to_friendly(tea, friendly, FALSE, 0);
        strcat(addressstring, friendly);

        forcedollar = TRUE;
        number_to_friendly(tea, friendly, FALSE, 0);
        forcedollar = FALSE;
        strcpy(pseudostring, friendly);
}   }

MODULE void ZERO_EA_TRACE(FLAG full)
{   tea = (relative_2650[OPERAND] & NONPAGE);

    // what about mirroring?

    if (OPERAND & 0x80)
    {   switch (style)
        {
        case  STYLE_OLDCALM: strcpy(addressstring, "0+@");
        acase STYLE_NEWCALM: strcpy(addressstring, "0+{");
        acase STYLE_IEEE:    strcpy(addressstring, "!@" );
        adefault:            strcpy(addressstring, "*"  );
        }
        number_to_friendly(tea, ENDOF(addressstring), FALSE, 0);
        if (style == STYLE_NEWCALM)
        {   strcat(addressstring, "}");
        }

        strcpy(pseudostring, "*");
        forcedollar = TRUE;
        number_to_friendly(tea, ENDOF(pseudostring), FALSE, 0);
        forcedollar = FALSE;

        addr = (int) tea;
        tea = memory[addr] << 8;
        if ((++addr & NONPAGE) == 0)
        {   addr -= PLEN; /* page wrap */
        }
        tea = (tea + memory[addr]) & AMSK;

        if (full)
        {   number_to_friendly(tea, (STRPTR) friendly, FALSE, 0);
            sprintf(ENDOF(addressstring), " [%s]", friendly);

            forcedollar = TRUE;
            number_to_friendly(tea, (STRPTR) friendly, FALSE, 0);
            forcedollar = FALSE;
            sprintf(ENDOF(pseudostring ), " [%s]", friendly);
    }   }
    else
    {   switch (style)
        {
        case  STYLE_OLDCALM:
        case  STYLE_NEWCALM: strcpy(addressstring, "0+");
        acase STYLE_IEEE:    strcpy(addressstring, "!" );
        adefault:            addressstring[0] = EOS;
        }
        number_to_friendly(tea, ENDOF(addressstring), FALSE, 0);

        forcedollar = TRUE;
        number_to_friendly(tea, pseudostring, FALSE, 0);
        forcedollar = FALSE;
}   }

EXPORT void cpu_setup(void)
{   int i;

    for (i = 0; i < 8; i++)
    {   mn[i] = ' ';
    }
    mn[8] = EOS;

    byte1[2] =
    byte2[2] =
    byte3[2] = EOS;
}

EXPORT void view_next_2650(void)
{   DISCARD getfriendly(iar);
    zprintf
    (   TEXTPEN_TRACE,
        LLL
        (   MSG_NEXTINSTRUCTION,
            "Next instruction is %s at %s.\n\n"
        ),
        opcodes[style][memory[iar]].name,
        friendly
    );
}

EXPORT void saytrace(void)
{   if (verbosity == 1)
    {   zprintf(TEXTPEN_TRACE, "%s", tn);
        if (disassembling)
        {   zprintf(TEXTPEN_TRACE, "\n");
    }   }
    else
    {   zprintf(TEXTPEN_TRACE, "%s", mn);
        if (verbosity == 0)
        {   zprintf(TEXTPEN_TRACE2, "%s\n", mn2);
            return;
        }
        if (DisHandle)
        {   zprintf(TEXTPEN_TRACE2, "%s ", mn2);
        } else
        {   zprintf(TEXTPEN_TRACE2, "%s\n;", mn2);
        }
        if (tea == OUTOFRANGE)
        {   zprintf(TEXTPEN_PSEUDOCODE, "%s\n", an);
        } else
        {   zprintf(TEXTPEN_PSEUDOCODE, "%s // $%X is ", an, tea);
            if (tea != mirror_r[tea] || tea != mirror_w[tea])
            {   zprintf(TEXTPEN_PSEUDOCODE, "mirror\n");
            } elif (memflags[tea] & ASIC)
            {   zprintf(TEXTPEN_PSEUDOCODE, "hardware register\n");
            } elif (memflags[tea] & (NOREAD & NOWRITE))
            {   zprintf(TEXTPEN_PSEUDOCODE, "unmapped\n");
            } elif (memflags[tea] & NOWRITE)
            {   if (memflags[tea] & BIOS)
                {   zprintf(TEXTPEN_PSEUDOCODE, "BIOS ROM\n");
                } else
                {   zprintf(TEXTPEN_PSEUDOCODE, "game ROM\n");
            }   }
            elif (memflags[tea] & NOREAD)
            {   zprintf(TEXTPEN_PSEUDOCODE, "write-only\n"); // should never happen (as they are all ASICs)
            } elif (memflags[tea] & BIOS)
            {   zprintf(TEXTPEN_PSEUDOCODE, "BIOS RAM\n");
            } else
            {   zprintf(TEXTPEN_PSEUDOCODE, "user RAM\n");
        }   }
        if (en[0])
        {   zprintf(TEXTPEN_PSEUDOCODE, "%s", en);
    }   }
    if (verbosity == 2 && (memflags[iar] & COMMENTED))
    {   show_comments(iar);
    }

    tea = OUTOFRANGE;
}

EXPORT void disassemble_2650(int address1, int address2, FLAG quiet)
{   ULONG savedcycles = cycles_2650;
    UWORD savediar    = iar;
    UBYTE savedpsl    = psl;

    if (verbosity == 1 && !quiet)
    {   zprintf
        (   TEXTPEN_TRACE,
            "Loca InOpAd Mnemoni XRU IAdd  IV EAdd\n" \
            "---- ------ ------- --- ----- -- ----\n"
        );
    }

    cycles_2650 = 0;
    iar         = address1;
    psl         = 0; // why?

    do
    {   opcode = memory[iar];
        GET_RR;
        tracecpu_2650(TRUE, quiet);
        if (!quiet)
        {   saytrace();
        }

        cycles_2650 += duration;
        iar         += table_size_2650[opcode]; // we deliberately allow it to flow onto the next page
    } while (iar <= address2);
    if (!quiet)
    {   nextdis   = iar;
    }
    cycles_2650   = savedcycles;
    iar           = savediar;
    psl           = savedpsl;
}

#define DISFLAG_OPCODE          1              // currently both are
#define DISFLAG_OPERAND         DISFLAG_OPCODE // treated the same
#define DISFLAG_INDIRECT        2
#define DISFLAG_CALCULATED      4
#define DISFLAG_BREAK           8
#define DISFLAG_ILLEGAL        16
#define DISFLAG_INDETERMINATE  32
#define DISFLAG_CODELABEL      64
#define DISFLAG_DATA          128 // aka DISFLAG_DATALABEL
#define DISFLAG_POINTER       256 // aka DISFLAG_POINTERLABEL

EXPORT void disgame(int address1, int address2, STRPTR filename)
{   FLAG  done;
    UBYTE preservepsl;
    UWORD preserveiar;
    int   i,
          intaddr,
          threads = 0;

#ifdef WIN32
    hurry = TRUE;
    cls();
#endif

    preserveiar = iar;
    preservepsl = psl;

    // 1st pass

    memset(disflag   , 0, 32768 * sizeof(UWORD));
    memset(threadlist, 0, 32768 * sizeof(UWORD));

    threadlist[threads++] = 0;
    disflag[0] |= DISFLAG_CODELABEL;

    switch (machine)
    {
    case INTERTON:
    case MALZAK:
    case ELEKTOR:
        intaddr = 0x0003;
    acase ZACCARIA:
        if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
        {   intaddr = 0x0003;
        } else
        {   // assert(memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN);
            intaddr = 0x000A;
        }
    acase INSTRUCTOR:
        intaddr = 0x0007;
    acase PHUNSY:
        intaddr = 0x1D;
    adefault:
        intaddr = OUTOFRANGE;
    }
    if (intaddr != OUTOFRANGE)
    {   threadlist[threads++] = intaddr;
        disflag[intaddr] |= DISFLAG_CODELABEL;
    }
    switch (machine)
    {
    case ELEKTOR:
        threadlist[threads++] = 0x903;
        disflag[0x903] |= DISFLAG_CODELABEL;
    }

    if (startaddr_h || startaddr_l)
    {   threadlist[threads++] = (startaddr_h * 256) + startaddr_l;
        disflag[(startaddr_h * 256) + startaddr_l] |= DISFLAG_CODELABEL;
    }

    if (userlabels)
    {   for (i = 0; i < userlabels; i++)
        {   if (symlabel[i].kind == CODE)
            {   threadlist[threads++] = symlabel[i].address;
    }   }   }
    // there are no code sections in PVI equivalents
    if (machines[machine].firstequiv != -1)
    {   for (i = machines[machine].firstequiv; i <= machines[machine].lastequiv; i++)
        {   if (equivalents[i].kind == CODE)
            {   threadlist[threads++] = equivalents[i].address;
    }   }   }
    if (machine == BINBUG)
    {   if (firstdosequiv != -1)
        {   for (i = firstdosequiv; i <= lastdosequiv; i++)
            {   if (equivalents[i].kind == CODE)
                {   threadlist[threads++] = equivalents[i].address;
        }   }   }
        for (i = FIRSTACOSEQUIV; i <= LASTACOSEQUIV; i++)
        {   if (equivalents[i].kind == CODE)
            {   threadlist[threads++] = equivalents[i].address;
    }   }   }
    if (whichgame != -1 && known[whichgame].firstequiv != -1)
    {   for (i = FIRSTGAMEEQUIV + known[whichgame].firstequiv; i <= FIRSTGAMEEQUIV + known[whichgame].lastequiv; i++)
        {   if (equivalents[i].kind == CODE)
            {   threadlist[threads++] = equivalents[i].address;
    }   }   }

    disassembling = 2; // means DISGAME command
    psl = 0; // so that GET_RR always assumes the primary register bank (R1..R3 rather than R4..R6)
    while (threads)
    {   iar = threadlist[--threads];
        done = FALSE;

        do
        {   if (disflag[iar] & DISFLAG_OPCODE) // already done, no need to redisassemble
            {   done = TRUE;
            } else
            {   opcode = memory[iar];
                disflag[iar] |= DISFLAG_OPCODE;
                /* if (table_size_2650[opcode] >= 2)
                {   disflag[iar + 1] |= DISFLAG_OPERAND; // not strictly necessary
                    if (table_size_2650[opcode] >= 3)
                    {   disflag[iar + 2] |= DISFLAG_OPERAND; // not strictly necessary
                }   } */

                GET_RR;
                switch (opcode)
                {
                case  0x00:                                  // LODZ,r0
                    disflag[iar] |= DISFLAG_INDETERMINATE;
                acase 0x08: case 0x09: case 0x0A: case 0x0B: // LODR,rn
                case  0x28: case 0x29: case 0x2A: case 0x2B: // EORR,rn
                case  0x48: case 0x49: case 0x4A: case 0x4B: // ANDR,rn
                case  0x68: case 0x69: case 0x6A: case 0x6B: // IORR,rn
                case  0x88: case 0x89: case 0x8A: case 0x8B: // ADDR,rn
                case  0xA8: case 0xA9: case 0xAA: case 0xAB: // SUBR,rn
                case  0xC8: case 0xC9: case 0xCA: case 0xCB: // STRR,rn
                case  0xE8: case 0xE9: case 0xEA: case 0xEB: // COMR,rn
                    tea = (iar & PAGE) + ((iar + 2 + relative_2650[OPERAND]) & NONPAGE);
                    if (OPERAND & 0x80) // indirect
                    {   disflag[tea] |= DISFLAG_POINTER;
                    } else
                    {   disflag[tea] |= DISFLAG_DATA;
                    }
                acase 0x0C: case 0x0D: case 0x0E: case 0x0F: // LODA,rn
                case  0x2C: case 0x2D: case 0x2E: case 0x2F: // EORA,rn
                case  0x4C: case 0x4D: case 0x4E: case 0x4F: // ANDA,rn
                case  0x6C: case 0x6D: case 0x6E: case 0x6F: // IORA,rn
                case  0x8C: case 0x8D: case 0x8E: case 0x8F: // ADDA,rn
                case  0xAC: case 0xAD: case 0xAE: case 0xAF: // SUBA,rn
                case  0xCC: case 0xCD: case 0xCE: case 0xCF: // STRA,rn
                case  0xEC: case 0xED: case 0xEE: case 0xEF: // COMA,rn
                    if ((OPERAND & 0x60) == 0x00) // non-indexed
                    {   tea = (iar & PAGE) + (((OPERAND << 8) + memory[iar + 2]) & NONPAGE);
                        if (OPERAND & 0x80) // indirect
                        {   disflag[tea] |= DISFLAG_POINTER;
                        } else
                        {   disflag[tea] |= DISFLAG_DATA;
                    }   }
                acase 0x10: case 0x11:                       // LDPL    and STPL
                    if (supercpu)
                    {   tea = ((OPERAND << 8) + memory[iar + 2]) & AMSK;
                        if (OPERAND & 0x80) // indirect
                        {   disflag[tea] |= DISFLAG_POINTER;
                        } else
                        {   disflag[tea] |= DISFLAG_DATA;
                    }   }
                    else
                    {   disflag[iar] |= DISFLAG_ILLEGAL;
                        done = TRUE;
                    }
                acase 0x17: case 0x37: case 0x40:            // RETC,un and RETE,un and HALT
                    disflag[iar] |= DISFLAG_BREAK;
                    done = TRUE;
                acase 0x18: case 0x19: case 0x1A:            // BCTR,cc
                case  0x38: case 0x39: case 0x3A: case 0x3B: // BSTR,cc
                case  0x58: case 0x59: case 0x5A: case 0x5B: // BRNR,rn
                case  0x78: case 0x79: case 0x7A: case 0x7B: // BSNR,rn
                case  0x98: case 0x99: case 0x9A:            // BCFR,cc
                case  0xB8: case 0xB9: case 0xBA:            // BSFR,cc
                case  0xD8: case 0xD9: case 0xDA: case 0xDB: // BIRR,rn
                case  0xF8: case 0xF9: case 0xFA: case 0xFB: // BDRR,rn
                    REL_EA_TRACE();
                    if (OPERAND & 0x80)
                    {   disflag[iar] |= DISFLAG_INDIRECT;
                        if ((memflags[tea] & NOWRITE) && (memflags[tea + 1] & NOWRITE))
                        {   disflag[tea] |= DISFLAG_CODELABEL;
                            threadlist[threads++] = (UWORD) tea;
                    }   }
                     else
                    {   disflag[tea] |= DISFLAG_CODELABEL;
                        threadlist[threads++] = (UWORD) tea;
                    }
                acase 0x1B:                                  // BCTR,un
                    REL_EA_TRACE();
                    if (OPERAND & 0x80)
                    {   disflag[iar] |= DISFLAG_INDIRECT;
                        if ((memflags[tea] & NOWRITE) && (memflags[tea + 1] & NOWRITE))
                        {   disflag[tea] |= DISFLAG_CODELABEL;
                            threadlist[threads++] = (UWORD) tea;
                    }   }
                    else
                    {   disflag[tea] |= DISFLAG_CODELABEL;
                        threadlist[threads++] = (UWORD) tea;
                    }
                    disflag[iar] |= DISFLAG_BREAK;
                    done = TRUE;
                acase 0x1C: case 0x1D: case 0x1E:            // BCTA,cc
                case  0x3C: case 0x3D: case 0x3E: case 0x3F: // BSTA,cc
                case  0x5C: case 0x5D: case 0x5E: case 0x5F: // BRNA,rn
                case  0x7C: case 0x7D: case 0x7E: case 0x7F: // BSNA,rn
                case  0x9C: case 0x9D: case 0x9E:            // BCFA,cc
                case  0xBC: case 0xBD: case 0xBE:            // BSFA,cc
                case  0xDC: case 0xDD: case 0xDE: case 0xDF: // BIRA,rn
                case  0xFC: case 0xFD: case 0xFE: case 0xFF: // BDRA,rn
                    BRA_EA_TRACE(TRUE);
                    if (OPERAND & 0x80)
                    {   disflag[iar] |= DISFLAG_INDIRECT;
                        if ((memflags[tea] & NOWRITE) && (memflags[tea + 1] & NOWRITE))
                        {   disflag[tea] |= DISFLAG_CODELABEL;
                            threadlist[threads++] = (UWORD) tea;
                    }   }
                    else
                    {   disflag[tea] |= DISFLAG_CODELABEL;
                        threadlist[threads++] = (UWORD) tea;
                    }
                acase 0x1F:                                  // BCTA,un
                    BRA_EA_TRACE(TRUE);
                    if (OPERAND & 0x80)
                    {   disflag[iar] |= DISFLAG_INDIRECT;
                        if ((memflags[tea] & NOWRITE) && (memflags[tea + 1] & NOWRITE))
                        {   disflag[tea] |= DISFLAG_CODELABEL;
                            threadlist[threads++] = (UWORD) tea;
                    }   }
                    else
                    {   disflag[tea] |= DISFLAG_CODELABEL;
                        threadlist[threads++] = (UWORD) tea;
                    }
                    disflag[iar] |= DISFLAG_BREAK;
                    done = TRUE;
                acase 0x90: case 0x91: case 0xB6: case 0xB7: // illegal
                case  0xC4: case 0xC5: case 0xC6: case 0xC7: // instructions
                    disflag[iar] |= DISFLAG_ILLEGAL;
                    done = TRUE;
                acase 0x9B:                                  // ZBRR
                    ZERO_EA_TRACE(TRUE);
                    if (OPERAND & 0x80)
                    {   disflag[iar] |= DISFLAG_INDIRECT;
                        if ((memflags[tea] & NOWRITE) && (memflags[tea + 1] & NOWRITE))
                        {   disflag[tea] |= DISFLAG_CODELABEL;
                            threadlist[threads++] = (UWORD) tea;
                    }   }
                    else
                    {   disflag[tea] |= DISFLAG_CODELABEL;
                        threadlist[threads++] = (UWORD) tea;
                    }
                    disflag[iar] |= DISFLAG_BREAK;
                    done = TRUE;
                acase 0x9F:                                  // BXA,r3
                    if (OPERAND & 0x80)
                    {   disflag[iar] |= DISFLAG_INDIRECT;
                        // we don't thread it because of the use of r3
                    }
                    disflag[iar] |= DISFLAG_BREAK | DISFLAG_CALCULATED;
                    done = TRUE;
                acase 0xBB:                                  // ZBSR
                    ZERO_EA_TRACE(TRUE);
                    if (OPERAND & 0x80)
                    {   disflag[iar] |= DISFLAG_INDIRECT;
                        if ((memflags[tea] & NOWRITE) && (memflags[tea + 1] & NOWRITE))
                        {   disflag[tea] |= DISFLAG_CODELABEL;
                            threadlist[threads++] = (UWORD) tea;
                    }   }
                    else
                    {   disflag[tea] |= DISFLAG_CODELABEL;
                        threadlist[threads++] = (UWORD) tea;
                    }
                acase 0xBF:                                  // BSXA,r3
                    if (OPERAND & 0x80)
                    {   disflag[iar] |= DISFLAG_INDIRECT;
                        // we don't thread it because of the use of r3
                    }
                    disflag[iar] |= DISFLAG_CALCULATED;
                }
                iar += table_size_2650[opcode];
        }   }
        while (!done);
    }

    // 2nd pass

    for (i = 0; i <= 32767; i++)
    {   if (disflag[i] & DISFLAG_CODELABEL)
        {   // assert(disflag[i] & DISFLAG_OPCODE);
            if (number_to_friendly(i, (STRPTR) friendly, FALSE, 0) == EOS)
            {   if (machine != INSTRUCTOR && i == 0)
                {   adduserlabel("BOOT", (UWORD) i, CODE);
                } elif (i == intaddr)
                {   adduserlabel("INTERRUPT", (UWORD) i, CODE);
                } elif (i == (startaddr_h * 256) + startaddr_l)
                {   adduserlabel("GAME", (UWORD) i, CODE);
                } else
                {   sprintf((char*) friendly, "L%04X", i);
                    adduserlabel((STRPTR) friendly, (UWORD) i, CODE);
        }   }   }
        elif (disflag[i] & DISFLAG_DATA)
        {   if (number_to_friendly(i, (STRPTR) friendly, FALSE, 0) == EOS)
            {   {   sprintf((char*) friendly, "X%04X", i);
                    adduserlabel((STRPTR) friendly, (UWORD) i, DATA);
        }   }   }
        elif (disflag[i] & DISFLAG_POINTER)
        {   if (number_to_friendly(i, (STRPTR) friendly, FALSE, 0) == EOS)
            {   {   sprintf((char*) friendly, "P%04X", i);
                    adduserlabel((STRPTR) friendly, (UWORD) i, POINTER);
    }   }   }   }

    // This pass identifies used labels so we don't put them in the EQUate list

    for (i = 0; i < EQUIVALENTS; i++)
    {   equivalents[i].listed = FALSE;
    }
    for (i = 0; i < SYMLABELS_MAX; i++)
    {   symlabel[i].listed = FALSE;
    }
    for (i = address1; i <= address2; i++)
    {   if (disflag[i] & (DISFLAG_CODELABEL | DISFLAG_DATA))
        {   DISCARD number_to_friendly(i, (STRPTR) friendly, FALSE, 0);
            if (foundequiv >= ONE_MILLION)
            {   symlabel[foundequiv - ONE_MILLION].listed = TRUE;
            } elif (foundequiv != -1)
            {   equivalents[foundequiv].listed = TRUE;
    }   }   }

    if (filename[0])
    {   fixextension(filekind[KIND_ASM].extension, filename, TRUE, "");
        cd_projects();
        if (!(DisHandle = fopen(filename, "wt")))
        {   zprintf(TEXTPEN_ERROR, "Can't open %s for output!\n", filename);
            // we still emit to the screen anyway
    }   }

    datestamp2();
    if (DisHandle)
    {   zprintf(TEXTPEN_CLIOUTPUT, ";%s\n", filename);
    }
    zprintf(TEXTPEN_CLIOUTPUT, ";Disassembled by %s at %s\n", hostmachines[machine].titlebartext, datetimestring);
    zprintf(TEXTPEN_CLIOUTPUT, ";Platform: %s\n", memmapinfo[memmap].name);
    zprintf(TEXTPEN_CLIOUTPUT, ";Size:     %d bytes ($%04X..$%04X)\n\n", (int) address2 - address1 + 1, address1, address2);

    if (style == STYLE_OLDCALM || style == STYLE_NEWCALM)
    {   doasmhdr(".EQU");
        zprintf(TEXTPEN_TRACE, "\n        .LOC    $%X\n\n", address1);
    } else
    {   doasmhdr("EQU");
        if (style == STYLE_IEEE)
        {   zprintf(TEXTPEN_TRACE, "\n        ORG    $%X\n\n", address1);
        } else
        {   zprintf(TEXTPEN_TRACE, "\n        ORG   $%X\n\n", address1);
    }   }

    iar = address1;
    do
    {   if (disflag[iar] & DISFLAG_OPCODE)
        {   disassemble_2650(iar, iar + table_size_2650[memory[iar]] - 1, FALSE);
            if (disflag[iar] & DISFLAG_INDETERMINATE)
            {   zprintf(TEXTPEN_COMMENT, ";Warning: indeterminate instruction!\n");
            }
            if (disflag[iar] & DISFLAG_INDIRECT)
            {   zprintf(TEXTPEN_COMMENT, ";Warning: indirect branch!\n");
            }
            if (disflag[iar] & DISFLAG_CALCULATED)
            {   zprintf(TEXTPEN_COMMENT, ";Warning: calculated branch!\n");
            }
            if (disflag[iar] & DISFLAG_BREAK) // this one must be last
            {   zprintf(TEXTPEN_COMMENT, "\n");
                // zprintf(TEXTPEN_COMMENT, ";Note: unconditional break.\n");
            }
            iar += table_size_2650[memory[iar]];
        } else
        {   if (number_to_friendly(iar, (STRPTR) friendly, FALSE, 0) != EOS)
            {   zprintf(TEXTPEN_LABEL, "%s: ;$%X\n", friendly, iar);
            }
            /* if
            (   (disflag[iar    ] & DISFLAG_ILLEGAL)
             || iar == 32767
             || (disflag[iar + 1] & (DISFLAG_OPCODE | DISFLAG_OPERAND))
            )
            { */
                switch (style)
                {
                case STYLE_SIGNETICS1:
                case STYLE_SIGNETICS2:
                    zprintf
                    (   TEXTPEN_TRACE,
                        "        DB      $%02X",
                        memory[iar]
                    );
                    if (verbosity == 2)
                    {   zprintf
                        (   TEXTPEN_TRACE2,
                            "                                      ;  0,1 $%04X %02X\n",
                            iar,
                            memory[iar]
                        );
                    }
                case STYLE_OLDCALM:
                case STYLE_NEWCALM:
                    zprintf
                    (   TEXTPEN_TRACE,
                        "        .DATA    16'%02X",
                        memory[iar]
                    );
                    if (verbosity == 2)
                    {   zprintf
                        (   TEXTPEN_TRACE2,
                            "                                     ;  0,1 $%04X %02X\n",
                            iar,
                            memory[iar]
                        );
                    }
                acase STYLE_IEEE:
                    zprintf
                    (   TEXTPEN_TRACE,
                        "        DATA   H'%02X",
                        memory[iar]
                    );
                    if (verbosity == 2)
                    {   zprintf
                        (   TEXTPEN_TRACE2,
                            "                                       ;  0,1 $%04X %02X\n",
                            iar,
                            memory[iar]
                        );
                    }
                }
                if (verbosity == 2)
                {   show_comments(iar);
                }
                if (disflag[iar] & DISFLAG_ILLEGAL)
                {   zprintf(TEXTPEN_COMMENT, ";Warning: illegal instruction!\n");
                }
                iar++;
         /* } else
            {   if (style == STYLE_CALM)
                {   zprintf
                    (   TEXTPEN_TRACE,
                        "        .16     $%02X%02X",
                        memory[iar    ],
                        memory[iar + 1]
                    );
                } else
                {   zprintf
                    (   TEXTPEN_TRACE,
                        "        DW      $%02X%02X",
                        memory[iar    ],
                        memory[iar + 1]
                    );
                }
                if (verbosity)
                {   zprintf
                    (   TEXTPEN_TRACE2,
                        "                                    ;  0,2 $%04X %02X %02X\n",
                        iar,
                        memory[iar    ],
                        memory[iar + 1]
                    );
                }
                iar += 2;
            } */
    }   }
    while (iar <= address2);

    if (style == STYLE_OLDCALM || style == STYLE_NEWCALM)
    {   zprintf(TEXTPEN_TRACE, "\n        .END     $%04X\n", (startaddr_h * 256) + startaddr_l);
    } elif (style == STYLE_IEEE)
    {   zprintf(TEXTPEN_TRACE, "\n        END    $%04X\n"  , (startaddr_h * 256) + startaddr_l);
    } else
    {   zprintf(TEXTPEN_TRACE, "\n        END     $%04X\n" , (startaddr_h * 256) + startaddr_l);
    }

    if (filename[0])
    {   cd_progdir();
        if (DisHandle)
        {   DISCARD fclose(DisHandle);
            DisHandle = NULL;
    }   }

    disassembling = 0; // means tracing (or nothing)
    iar = preserveiar;
    psl = preservepsl;

    zprintf(TEXTPEN_CLIOUTPUT, "\n");
#ifdef WIN32
    hurry = FALSE;
    zprintf(TEXTPEN_DEFAULT, "");
#endif
}

EXPORT void show_data_comment(int whichaddr, int mode, STRPTR stringptr)
{   FAST FLAG first;
    FAST int  i;

/* Mode 0: came from disasm.c
   Mode 1: came from = command
   Mode 2: came from make_#?tip() */

    // machine-specific data comments
    if (machines[machine].firstdatacomment != -1)
    {   first = TRUE;
        for (i = machines[machine].firstdatacomment; i <= machines[machine].lastdatacomment; i++)
        {   first = do_data_comment(i, whichaddr, mode, first, stringptr);
    }   }

    if (machine == BINBUG)
    {   if (firstdosdatacomment != -1)
        {   for (i = firstdosdatacomment; i <= lastdosdatacomment; i++)
            {   DISCARD do_data_comment(i, whichaddr, mode, FALSE, stringptr);
        }   }

        for (i = FIRSTACOSDATACOMMENT; i <= LASTACOSDATACOMMENT; i++)
        {   DISCARD do_data_comment(i, whichaddr, mode, FALSE, stringptr);
    }   }

    // game-specific data comments
    if (whichgame != -1 && known[whichgame].firstdatacomment != -1)
    {   for (i = known[whichgame].firstdatacomment + FIRSTGAMEDATACOMMENT; i <= known[whichgame].lastdatacomment + FIRSTGAMEDATACOMMENT; i++)
        {   DISCARD do_data_comment(i, whichaddr, mode, FALSE, stringptr);
}   }   }

MODULE FLAG do_data_comment(int whichcomment, int whichaddr, int mode, FLAG first, STRPTR stringptr)
{   FAST    int  j, k;
    PERSIST TEXT tempstring[256 + 1]; // PERSISTent so as not to blow the stack

    switch (mode)
    {
    case 0:
        if (datacomment[whichcomment].first == whichaddr)
        {   j = 0;
            do
            {   k = 0;
                do
                {   tempstring[k++] = datacomment[whichcomment].text[j++];
                } while (tempstring[k - 1] != '\n' && tempstring[k - 1] != EOS);
                if (tempstring[k - 1] == '\n')
                {   tempstring[k] = EOS; // don't increment k
                }
                zprintf(TEXTPEN_COMMENT, ";%s", tempstring);
            } while (tempstring[k - 1] != EOS);
            zprintf(TEXTPEN_COMMENT, "\n");
        }
    acase 1:
        if
        (   whichaddr >= datacomment[whichcomment].first
         && (   (datacomment[whichcomment].last >=  0 && whichaddr <= datacomment[whichcomment].last     )
             || (datacomment[whichcomment].last == -1 && whichaddr == datacomment[whichcomment].first    )
             || (datacomment[whichcomment].last == -2 && whichaddr <= datacomment[whichcomment].first + 1)
        )   )
        {   strcpy((char*) tempstring, datacomment[whichcomment].text);
            if (first)
            {   extracomment(whichcomment, whichaddr, tempstring);
                first = FALSE;
            }
            zprintf(TEXTPEN_CLIOUTPUT, "%s\n", tempstring);
        }
    acase 2:
        if
        (   whichaddr >= datacomment[whichcomment].first
         && (   (datacomment[whichcomment].last >=  0 && whichaddr <= datacomment[whichcomment].last     )
             || (datacomment[whichcomment].last == -1 && whichaddr == datacomment[whichcomment].first    )
             || (datacomment[whichcomment].last == -2 && whichaddr <= datacomment[whichcomment].first + 1)
        )   )
        {   sprintf(ENDOF(stringptr), "\n%s", datacomment[whichcomment].text);
    }   }

    return first;
}

EXPORT STRPTR labelstring2(TEXT whichkind)
{   // User-friendly strings
    switch (whichkind)
    {
    case CODE:    return LLL(MSG_CODE   , "code"   );
    case DATA:    return LLL(MSG_DATA   , "data"   );
    case POINTER: return LLL(MSG_POINTER, "pointer");
    default:      return LLL(MSG_UNKNOWN, "unknown");
}   }

EXPORT void doasmhdr(STRPTR equname)
{   FLAG first;
    int  i;

    first = TRUE;
    for (i = FIRSTCPUEQUIV; i <= LASTCPUEQUIV; i++)
    {   if (!equivalents[i].listed)
        {   if (first)
            {   first = FALSE;
                zprintf(TEXTPEN_COMMENT, ";CPU Equates--------------------------------------------------------------\n");
            }
            zprintf(    TEXTPEN_TRACE, "%-17s %s $%04X ;%s\n", equivalents[i].name, equname, (int) equivalents[i].address, labelstring2(equivalents[i].kind));
            // don't show data comments as these equates are not addresses
    }   }
    if (machines[machine].pvis)
    {   first = TRUE;
        for (i = FIRSTPVIEQUIV; i < FIRSTPVIEQUIV + (33 * machines[machine].pvis); i++)
        {   if (!equivalents[i].listed)
            {   if (first)
                {   first = FALSE;
                    zprintf(TEXTPEN_COMMENT, ";PVI Equates--------------------------------------------------------------\n");
                }
                zprintf(    TEXTPEN_TRACE, "%-17s %s $%04X ;%s\n", equivalents[i].name, equname, (int) pvibase + equivalents[i].address, labelstring2(equivalents[i].kind));
                show_data_comment((int) pvibase + equivalents[i].address, 0, NULL);
    }   }   }
    if (machines[machine].firstequiv != -1)
    {   first = TRUE;
        for (i = machines[machine].firstequiv; i <= machines[machine].lastequiv; i++)
        {   if (!equivalents[i].listed)
            {   if (first)
                {   first = FALSE;
                    zprintf(TEXTPEN_COMMENT, ";Machine Equates----------------------------------------------------------\n");
                }
                zprintf(    TEXTPEN_TRACE, "%-17s %s $%04X ;%s\n", equivalents[i].name, equname, (int)           equivalents[i].address, labelstring2(equivalents[i].kind));
                show_data_comment((int) equivalents[i].address, 0, NULL);
    }   }   }
    if (machine == BINBUG)
    {   if (firstdosequiv != -1)
        {   first = TRUE;
            for (i = firstdosequiv; i <= lastdosequiv; i++)
            {   if (!equivalents[i].listed)
                {   if (first)
                    {   first = FALSE;
                        zprintf(TEXTPEN_COMMENT, ";DOS Equates--------------------------------------------------------------\n");
                    }
                    zprintf(    TEXTPEN_TRACE, "%-17s %s $%04X ;%s\n", equivalents[i].name, equname, (int)           equivalents[i].address, labelstring2(equivalents[i].kind));
                    show_data_comment((int) equivalents[i].address, 0, NULL);
        }   }   }
        first = TRUE;
        for (i = FIRSTACOSEQUIV; i <= LASTACOSEQUIV; i++)
        {   if (!equivalents[i].listed)
            {   if (first)
                {   first = FALSE;
                    zprintf(TEXTPEN_COMMENT, ";ACOS Equates-------------------------------------------------------------\n");
                }
                zprintf(    TEXTPEN_TRACE, "%-17s %s $%04X ;%s\n", equivalents[i].name, equname, (int)           equivalents[i].address, labelstring2(equivalents[i].kind));
                show_data_comment((int) equivalents[i].address, 0, NULL);
    }   }   }
    if (whichgame != -1 && known[whichgame].firstequiv != -1)
    {   first = TRUE;
        for (i = FIRSTGAMEEQUIV + known[whichgame].firstequiv; i <= FIRSTGAMEEQUIV + known[whichgame].lastequiv; i++)
        {   if (!equivalents[i].listed)
            {   if (first)
                {   first = FALSE;
                    zprintf(TEXTPEN_COMMENT, ";Game Equates-------------------------------------------------------------\n");
                }
                zprintf(    TEXTPEN_TRACE, "%-17s %s $%04X ;%s\n", equivalents[i].name, equname, (int)           equivalents[i].address, labelstring2(equivalents[i].kind));
                show_data_comment((int) equivalents[i].address, 0, NULL);
    }   }   }
    if (userlabels)
    {   first = TRUE;
        for (i = 0; i < userlabels; i++)
        {   if (!symlabel[i].listed)
            {   if (first)
                {   first = FALSE;
                    zprintf(TEXTPEN_COMMENT, ";User Equates-------------------------------------------------------------\n");
                }
                zprintf(    TEXTPEN_TRACE, "%-17s %s $%04X ;%s\n",    symlabel[i].name, equname, (int)           symlabel[i].address,    labelstring2(   symlabel[i].kind));
}   }   }   }

MODULE void show_code_comment(int whichaddr)
{   int i;

    // machine-specific code comments
    if (machines[machine].firstcodecomment != -1)
    {   for (i = machines[machine].firstcodecomment; i <= machines[machine].lastcodecomment; i++)
        {   if (whichaddr == codecomment[i].address)
            {   zprintf(TEXTPEN_COMMENT, ";%s\n", codecomment[i].text);
    }   }   }
    if (machine == BINBUG)
    {   if (firstdoscodecomment != -1)
        {   for (i = firstdoscodecomment; i <= lastdoscodecomment; i++)
            {   if (whichaddr == codecomment[i].address)
                {   zprintf(TEXTPEN_COMMENT, ";%s\n", codecomment[i].text);
        }   }   }
        for (i = FIRSTACOSCODECOMMENT; i <= LASTACOSCODECOMMENT; i++)
        {   if (whichaddr == codecomment[i].address)
            {   zprintf(TEXTPEN_COMMENT, ";%s\n", codecomment[i].text);
    }   }   }

    // game-specific code comments
    if (whichgame != -1 && known[whichgame].firstcodecomment != -1)
    {   // assert(known[whichgame].lastcodecomment != -1);
        for (i = known[whichgame].firstcodecomment; i <= known[whichgame].lastcodecomment; i++)
        {   if (whichaddr == codecomment[i].address)
            {   zprintf(TEXTPEN_COMMENT, ";%s\n", codecomment[i].text);
                break; // for speed
}   }   }   }

MODULE void show_comments(int whichaddr)
{   show_code_comment(whichaddr);
    switch (machine)
    {
    case  ELEKTOR:              elektor_biosdetails(       whichaddr);
    acase PIPBUG:  case BINBUG: pipbin_biosdetails(        whichaddr);
    acase CD2650:               cd2650_biosdetails1(       whichaddr);
    acase PHUNSY:               phunsy_biosdetails(        whichaddr);
    acase SELBST:               selbst_biosdetails((memory[whichaddr    ] * 256)
                                                  + memory[whichaddr + 1]);
    acase TWIN:                 twin_biosdetails(          whichaddr);
    }
    show_data_comment(whichaddr, 0, NULL);
}

EXPORT void table_trace(void)
{   FAST    int   i;
    PERSIST UBYTE oldr[7] = { 0, 0, 0, 0, 0, 0, 0 },
                  oldpsu   = 0,
                  oldpsl   = 0;
    PERSIST UWORD oldiar   = 0;

    zprintf
    (   TEXTPEN_GREEN,
        "    "
    );

    for (i = 0; i < 7; i++)
    {   if (oldr[i] != r[i])
        {   zprintf(TEXTPEN_RED, "%02X ", r[i]);
            oldr[i] = r[i];
        } else
        {   zprintf(TEXTPEN_GREEN, "%02X ", r[i]);
    }   }

    if (oldpsu != psu)
    {   zprintf(TEXTPEN_RED, "%02X ", psu);
        oldpsu = psu;
    } else
    {   zprintf(TEXTPEN_GREEN, "%02X ", psu);
    }
    if (oldpsl != psl)
    {   zprintf(TEXTPEN_RED, "%02X ", psl);
        oldpsl = psl;
    } else
    {   zprintf(TEXTPEN_GREEN, "%02X ", psl);
    }

    oldiar += table_size_2650[opcode];

    if (oldiar != iar)
    {   zprintf(TEXTPEN_RED, "%04X\n", iar);
        oldiar = iar;
    } else
    {   zprintf(TEXTPEN_GREEN, "%04X\n", iar);
}   }

EXPORT void update_opcodes_engine(void)
{   if (supercpu)
    {   table_size_2650[0x10]               =
        table_size_2650[0x11]               =
        table_duration_2650[0x00]           =
        table_duration_2650[0x01]           =
        table_duration_2650[0x02]           =
        table_duration_2650[0x03]           = // LODZ
        table_duration_2650[0x20]           =
        table_duration_2650[0x21]           =
        table_duration_2650[0x22]           =
        table_duration_2650[0x23]           = // EORZ
        table_duration_2650[0x40]           = // HALT (not confirmed on real hardware but seems likely)
        table_duration_2650[0x41]           =
        table_duration_2650[0x42]           =
        table_duration_2650[0x43]           = // ANDZ
        table_duration_2650[0x60]           =
        table_duration_2650[0x61]           =
        table_duration_2650[0x62]           =
        table_duration_2650[0x63]           = // IORZ
        table_duration_2650[0x80]           =
        table_duration_2650[0x81]           =
        table_duration_2650[0x82]           =
        table_duration_2650[0x83]           = // ADDZ
        table_duration_2650[0xA0]           =
        table_duration_2650[0xA1]           =
        table_duration_2650[0xA2]           =
        table_duration_2650[0xA3]           = // SUBZ
        table_duration_2650[0xC0]           = // NOP (confirmed on real hardware)
        table_duration_2650[0xC1]           =
        table_duration_2650[0xC2]           =
        table_duration_2650[0xC3]           = // STRZ
        table_duration_2650[0xE0]           =
        table_duration_2650[0xE1]           =
        table_duration_2650[0xE2]           =
        table_duration_2650[0xE3]           = 3; // COMZ
        table_duration_2650[0x10]           =
        table_duration_2650[0x11]           = 12;

        switch (style)
        {
        case STYLE_OLDCALM:
            opcodes[style][0x10].cc         = "CC = (*(abs) & $C0) >> 6;";
            opcodes[style][0x10].name       =
            opcodes[style][0x11].name       = "LOAD L";
            opcodes[style][0x10].format     = ",abs";
            opcodes[style][0x11].format     = "LOAD abs,L";
            opcodes[style][0x10].pseudocode = "L = *(abs);";
            opcodes[style][0x11].pseudocode = "*(abs) = L;";
            opcodes[style][0x10].fullname   = "Load L with *(abs)";
            opcodes[style][0x11].fullname   = "Load *(abs) with L";
            opcodes[style][0x10].wflags     =
            opcodes[style][0x11].rflags     = "CC, H, B, W, V, LOGICOMP, C";
            opcodes[style][0x12].rflags     =
            opcodes[style][0xB4].rflags     = "INPUT, OUTPUT, IOF, UF1, UF2, STACK";
            opcodes[style][0x74].wflags     =
            opcodes[style][0x76].wflags     =
            opcodes[style][0x92].wflags     = "OUTPUT, IOF, UF1, UF2, STACK";
            opcodes[style][0x74].pseudocode = "U &= ~(imm & %01111111);";
            opcodes[style][0x76].pseudocode = "U |= imm & %01111111;";
            opcodes[style][0x92].pseudocode = "U = (A & %01111111);";
        acase STYLE_NEWCALM:
            opcodes[style][0x10].cc         = "CC = (*(abs) & $C0) >> 6;";
            opcodes[style][0x10].name       =
            opcodes[style][0x11].name       = "MOVE";
            opcodes[style][0x10].format     = "MOVE abs,PSL";
            opcodes[style][0x11].format     = "MOVE PSL,abs";
            opcodes[style][0x10].pseudocode = "PSL = *(abs);";
            opcodes[style][0x11].pseudocode = "*(abs) = PSL;";
            opcodes[style][0x10].fullname   = "Load PSL with *(abs)";
            opcodes[style][0x11].fullname   = "Load *(abs) with PSL";
            opcodes[style][0x10].wflags     =
            opcodes[style][0x11].rflags     = "CC, H, B, W, V, LOGICOMP, C";
            opcodes[style][0x12].rflags     =
            opcodes[style][0xB4].rflags     = "INPUT, OUTPUT, IOF, UF1, UF2, STACK";
            opcodes[style][0x74].wflags     =
            opcodes[style][0x76].wflags     =
            opcodes[style][0x92].wflags     = "OUTPUT, IOF, UF1, UF2, STACK";
            opcodes[style][0x74].pseudocode = "PSU &= ~(imm & %01111111);";
            opcodes[style][0x76].pseudocode = "PSU |= imm & %01111111;";
            opcodes[style][0x92].pseudocode = "PSU = (r0 & %01111111);";
        acase STYLE_IEEE:
            opcodes[style][0x10].cc         = "CC = (*(abs) & $C0) >> 6;";
            opcodes[style][0x10].name       = "LD L";
            opcodes[style][0x11].name       = "ST L";
            opcodes[style][0x10].format     = ",abs";
            opcodes[style][0x11].format     = ",abs";
            opcodes[style][0x10].pseudocode = ".L = *(abs);";
            opcodes[style][0x11].pseudocode = "*(abs) = .L;";
            opcodes[style][0x10].fullname   = "Load .L with *(abs)";
            opcodes[style][0x11].fullname   = "Store .L into *(abs)";
            opcodes[style][0x10].wflags     =
            opcodes[style][0x11].rflags     = "CC, H, B, W, V, LOGICOMP, C";
            opcodes[style][0x12].rflags     =
            opcodes[style][0xB4].rflags     = "INPUT, OUTPUT, I, UF1, UF2, STACK";
            opcodes[style][0x74].wflags     =
            opcodes[style][0x76].wflags     =
            opcodes[style][0x92].wflags     = "OUTPUT, I, UF1, UF2, STACK";
            opcodes[style][0x74].pseudocode = ".U &= ~(imm & %01111111);";
            opcodes[style][0x76].pseudocode = ".U |= imm & %01111111;";
            opcodes[style][0x92].pseudocode = ".U = (A & %01111111);";
        adefault:
            opcodes[style][0x10].cc         = "CC = (*($aaaa) & $C0) >> 6;";
            opcodes[style][0x10].name       = "LDPL";
            opcodes[style][0x11].name       = "STPL";
            opcodes[style][0x10].format     =
            opcodes[style][0x11].format     = "$aaaa";
            opcodes[style][0x10].pseudocode = "PSL = *($aaaa);";
            opcodes[style][0x11].pseudocode = "*($aaaa) = PSL;";
            opcodes[style][0x10].fullname   = "Load Program Status, Lower, Absolute";
            opcodes[style][0x11].fullname   = "Store Program Status, Lower, Absolute";
            opcodes[style][0x10].wflags     =
            opcodes[style][0x11].rflags     = "CC, IDC, RS, WC, OVF, COM, C";
            opcodes[style][0x12].rflags     =
            opcodes[style][0xB4].rflags     = "S, F, II, UF1, UF2, SP";
            opcodes[style][0x74].wflags     =
            opcodes[style][0x76].wflags     =
            opcodes[style][0x92].wflags     = "F, II, UF1, UF2, SP";
            opcodes[style][0x74].pseudocode = "PSU &= ~($vv & %01111111);"; // CPSU
            opcodes[style][0x76].pseudocode = "PSU |= $vv & %01111111;"; // PPSU
            opcodes[style][0x92].pseudocode = "PSU = (r0 & %01111111);"; // LPSU
    }   }
    else
    {   psu &= 0xE7; // clear UF1 and UF2

        table_size_2650[0x10]                = (machine == INTERTON) ? 3 : 1;
        table_size_2650[0x11]                = 1;
        table_duration_2650[0x00]            =
        table_duration_2650[0x01]            =
        table_duration_2650[0x02]            =
        table_duration_2650[0x03]            = // LODZ
        table_duration_2650[0x10]            = // LDPL
        table_duration_2650[0x11]            = // STPL
        table_duration_2650[0x20]            =
        table_duration_2650[0x21]            =
        table_duration_2650[0x22]            =
        table_duration_2650[0x23]            = // EORZ
        table_duration_2650[0x40]            = // HALT
        table_duration_2650[0x41]            =
        table_duration_2650[0x42]            =
        table_duration_2650[0x43]            = // ANDZ
        table_duration_2650[0x60]            =
        table_duration_2650[0x61]            =
        table_duration_2650[0x62]            =
        table_duration_2650[0x63]            = // IORZ
        table_duration_2650[0x80]            =
        table_duration_2650[0x81]            =
        table_duration_2650[0x82]            =
        table_duration_2650[0x83]            = // ADDZ
        table_duration_2650[0xA0]            =
        table_duration_2650[0xA1]            =
        table_duration_2650[0xA2]            =
        table_duration_2650[0xA3]            = // SUBZ
        table_duration_2650[0xC0]            = // NOP
        table_duration_2650[0xC1]            =
        table_duration_2650[0xC2]            =
        table_duration_2650[0xC3]            = // STRZ
        table_duration_2650[0xE0]            =
        table_duration_2650[0xE1]            =
        table_duration_2650[0xE2]            =
        table_duration_2650[0xE3]            = 6; // COMZ

        opcodes[style][0x10].name       =
        opcodes[style][0x11].name       = "-------";
        opcodes[style][0x10].format     =
        opcodes[style][0x11].format     = "";
        opcodes[style][0x10].pseudocode =
        opcodes[style][0x11].pseudocode = "?";
        opcodes[style][0x10].fullname   =
        opcodes[style][0x11].fullname   = "-";
        opcodes[style][0x10].cc         = "Undefined";
        opcodes[style][0x10].wflags     =
        opcodes[style][0x11].rflags     = "None";

        switch (style)
        {
        case STYLE_OLDCALM:
            opcodes[style][0x74].pseudocode = "U &= ~(imm & %01100111);";
            opcodes[style][0x76].pseudocode = "U |= imm & %01100111;";
            opcodes[style][0x92].pseudocode = "U = (A & %01100111);";
            opcodes[style][0x12].rflags     =
            opcodes[style][0xB4].rflags     = "INPUT, OUTPUT, IOF, STACK";
            opcodes[style][0x74].wflags     =
            opcodes[style][0x76].wflags     =
            opcodes[style][0x92].wflags     = "OUTPUT, IOF, STACK";
        acase STYLE_NEWCALM:
            opcodes[style][0x74].pseudocode = "PSU &= ~(imm & %01100111);";
            opcodes[style][0x76].pseudocode = "PSU |= imm & %01100111;";
            opcodes[style][0x92].pseudocode = "PSU = (r0 & %01100111);";
            opcodes[style][0x12].rflags     =
            opcodes[style][0xB4].rflags     = "INPUT, OUTPUT, IOF, SP";
            opcodes[style][0x74].wflags     =
            opcodes[style][0x76].wflags     =
            opcodes[style][0x92].wflags     = "OUTPUT, IOF, SP";
        acase STYLE_IEEE:
            opcodes[style][0x74].pseudocode = ".U &= ~(imm & %01100111);";
            opcodes[style][0x76].pseudocode = ".U |= imm & %01100111;";
            opcodes[style][0x92].pseudocode = ".U = (.0 & %01100111);";
            opcodes[style][0x12].rflags     =
            opcodes[style][0xB4].rflags     = "INPUT, OUTPUT, I, STACK";
            opcodes[style][0x74].wflags     =
            opcodes[style][0x76].wflags     =
            opcodes[style][0x92].wflags     = "OUTPUT, I, STACK";
        adefault:
            opcodes[style][0x74].pseudocode = "PSU &= ~($vv & %01100111);";
            opcodes[style][0x76].pseudocode = "PSU |= $vv & %01100111;";
            opcodes[style][0x92].pseudocode = "PSU = (r0 & %01100111);";
            opcodes[style][0x12].rflags     =
            opcodes[style][0xB4].rflags     = "S, F, II, SP";
            opcodes[style][0x74].wflags     =
            opcodes[style][0x76].wflags     =
            opcodes[style][0x92].wflags     = "F, II, SP";
}   }   }

MODULE void extracomment(int whichcomment, int address, STRPTR stringptr)
{   switch (whichcomment)
    {
    case  355: sprintf(ENDOF(stringptr), " (X=%d,Y=%d)", (address & 0xF),  (address - 0x1800) / 16      ); // upper display (Arcadia)
    acase 382: sprintf(ENDOF(stringptr), " (X=%d,Y=%d)", (address & 0xF), ((address - 0x1A00) / 16) + 13); // lower display (Arcadia)
}   }

EXPORT void dec_to_bin(UBYTE number)
{   int i,
        length;

    length = strlen(binchars[style]);
    strcpy(v_bin, binchars[style]);
    for (i = 0; i < 8; i++)
    {   v_bin[length + i] = (number & (0x80 >> i)) ? '1' : '0';
    }
    v_bin[length + 8] = EOS;
}

EXPORT void dec_to_hex(UBYTE number)
{   if (number <= 9)
    {   v[0] = v_pse[0] = '0' + number;
        v[1] = v_pse[1] = EOS;
    } else
    {   if (style == STYLE_SIGNETICS1 || style == STYLE_SIGNETICS2)
        {   sprintf(v, "$%X", (unsigned int) number); // could use 'H' but $ is better
        } else
        {   sprintf(v, "%s%X", hexchars[style], (unsigned int) number);
        }
        sprintf(v_pse, "$%X", (unsigned int) number);
}   }
