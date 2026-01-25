// INCLUDES---------------------------------------------------------------

#include "ibm.h"

#include <commctrl.h>
#include <Shlobj.h>    // for eg. SHBrowseForFolder()

#include "resource.h"

#include <stdio.h>

#define EXEC_TYPES_H
#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"

// DEFINES----------------------------------------------------------------

#define SOMEREDEFINEGADS 8

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT FLAG  preserve;
EXPORT TEXT  temppath_games[MAX_PATH + 1];
EXPORT WCHAR voicename[80 + 1] = L"";

EXPORT struct AssociationStruct assoc[ASSOCIATIONS] = {
{ TRUE , IDC_8SVX, KIND_8SVX }, //  0 8SV,8SVX
{ TRUE , IDC_AIFF, KIND_AIFF }, //  1 AIF,AIFF
{ TRUE , IDC_AOF , KIND_AOF  }, //  2 AOF,OBJ
{ TRUE , IDC_ASM , KIND_ASM  }, //  3 ASM
{ TRUE , IDC_BIN , KIND_BIN  }, //  4 BIN
{ FALSE, IDC_BMP , KIND_BMP  }, //  5 BMP
{ TRUE , IDC_BPNF, KIND_BPNF }, //  6 BPNF
{ TRUE , IDC_CMD , KIND_CMD  }, //  7 CMD
{ TRUE , IDC_COR , KIND_COR  }, //  8 COR
{ TRUE , IDC_COS , KIND_COS  }, //  9 COS
{ TRUE , IDC_EOF , KIND_EOF  }, // 10 EOF
{ TRUE , IDC_HEX , KIND_HEX  }, // 11 HEX
{ TRUE , IDC_IMAG, KIND_IMAG }, // 12 IMAG
{ TRUE , IDC_IMG , KIND_IMG  }, // 13 IMG
{ TRUE , IDC_MDCR, KIND_MDCR }, // 14 MDCR
{ TRUE , IDC_MOD , KIND_MOD  }, // 15 MOD
{ TRUE , IDC_PAP , KIND_PAP  }, // 16 PAP
{ TRUE , IDC_RAW , KIND_RAW  }, // 17 RAW
{ FALSE, IDC_SMS , KIND_SMS  }, // 18 SMS
{ TRUE , IDC_SYM , KIND_SYM  }, // 19 SYM
{ TRUE , IDC_TVC , KIND_TVC  }, // 20 TVC
{ TRUE , IDC_TWIN, KIND_TWIN }, // 21 TWIN
{ FALSE, IDC_WAV , KIND_WAV  }, // 22 WAV
{ FALSE, IDC_ZIP , KIND_ZIP  }, // 23 ZIP
};

EXPORT struct KindStruct filekind[KINDS] =
{ { ".ilbm", ".lbm",
    "IFF ILBM (*.ILBM, *.LBM)\0*.ILBM;*.LBM\0All files (*.*)\0*.*\0",
    FALSE,
    NULL,
    NULL
  },
  { ".acbm", NULL,
    "IFF ACBM (*.ACBM)\0*.ACBM\0All files (*.*)\0*.*\0",
    FALSE,
    NULL,
    NULL
  },
  { ".bmp", NULL,
    "BMP (*.BMP)\0*.BMP\0All files (*.*)\0*.*\0",
    FALSE,
    "bmp_ext",
    "Bitmap",
  },
  { ".pcx", NULL,
    "PCX (*.PCX)\0*.PCX\0All files (*.*)\0*.*\0",
    FALSE,
    NULL,
    NULL
  },
  { ".gif", NULL,
    "GIF (*.GIF)\0*.GIF\0All files (*.*)\0*.*\0",
    FALSE,
    NULL,
    NULL
  },
  { ".tif", ".tiff",
    "TIFF (*.TIF, *.TIFF)\0*.TIF;*.TIFF\0All files (*.*)\0*.*\0",
    FALSE,
    NULL,
    NULL
  },
  { ".png", ".apng",
    "PNG (*.PNG, *.APNG)\0*.PNG;*.APNG\0All files (*.*)\0*.*\0",
    FALSE,
    NULL,
    NULL
  },
  { ".bin", NULL,
    "Arcadia/Interton/SI50/PHUNSY Program (*.BIN)\0*.BIN\0All files (*.*)\0*.*\0",
    TRUE,
    "bin_ext",
    "Program"
  },
  { ".cos", NULL,
    "Saved State (*.COS)\0*.COS\0All files (*.*)\0*.*\0",
    TRUE,
    "cos_ext",
    "COmmon State format"
  },
  { ".cor", NULL,
    "Recording (*.COR)\0*.COR\0All files (*.*)\0*.*\0",
    TRUE,
    "cor_ext",
    "COmmon Recording format"
  },
  { ".anim", NULL,
    "IFF ANIM (*.ANIM)\0*.ANIM\0All files (*.*)\0*.*\0",
    FALSE,
    NULL,
    NULL
  },
  { ".zip", NULL,
    "ZIP Archive (*.ZIP)\0*.ZIP\0All files (*.*)\0*.*\0",
    TRUE,
    "zip_ext",
    "ZIP archived Signetics-based game"
  },
  { ".aof", ".obj",
    "Absolute Object Format (*.AOF, *.OBJ)\0*.AOF;*.OBJ\0All files (*.*)\0*.*\0",
    TRUE,
    "aof_ext",
    "Signetics Absolute Object Format"
  },
  { ".eof", NULL,
    "Elektor Object Format (*.EOF)\0*.EOF\0All files (*.*)\0*.*\0",
    TRUE,
    "eof_ext",
    "Elektor Object Format"
  },
  { ".hex", NULL,
    "Intel Hex Format (*.HEX)\0*.HEX\0All files (*.*)\0*.*\0",
    TRUE,
    "hex_ext",
    "Intel Hex Format"
  },
  { ".wav", NULL,
    "RIFF WAVe (*.WAV)\0*.WAV\0All files (*.*)\0*.*\0",
    FALSE,
    "wav_ext",
    "RIFF WAVe"
  },
  { ".psg", NULL,
    "Programmable Sound Generator output (*.PSG)\0*.PSG\0All files (*.*)\0*.*\0",
    FALSE,
    "psg_ext",
    "AY-3-8910 Programmable Sound Generator output"
  },
  { ".ym", NULL,
    "Programmable Sound Generator output (*.YM)\0*.YM\0All files (*.*)\0*.*\0",
    FALSE,
    "ym_ext",
    "AY-3-8910 Yamaha Music"
  },
  { ".asm", ".2650",
    "Assembler source code (*.ASM, *.2650)\0*.ASM;*.2650\0All files (*.*)\0*.*\0",
    FALSE,
    "asm_ext",
    "2650 assembler source code"
  },
  { ".8svx", ".8sv",
    "IFF 8SVX (*.8SVX, *.8SV)\0*.8SVX;*.8SV\0All files (*.*)\0*.*\0",
    FALSE,
    "8svx_ext",
    "8-bit sampled sound",
  },
  { ".aiff", ".aif",
    "Audio IFF (*.AIFF, *.AIF)\0*.AIFF;*.AIF\0All files (*.*)\0*.*\0",
    FALSE,
    "aiff_ext",
    "Audio IFF sampled sound",
  },
  { ".mng", NULL,
    "MNG (*.MNG)\0*.MNG\0All files (*.*)\0*.*\0",
    FALSE,
    NULL,
    NULL
  },
  { ".avi", NULL,
    "RIFF AVI Animation (*.AVI)\0*.AVI\0All files (*.*)\0*.*\0",
    FALSE,
    NULL,
    NULL
  },
  { ".mid", ".midi",
    "Musical Instrument Digital Interface (*.MID, *.MIDI)\0*.MID;*.MIDI\0All files (*.*)\0*.*\0",
    FALSE,
    NULL,
    NULL
  },
  { ".smus", NULL,
    "IFF SMUS (*.SMUS)\0*.SMUS\0All files (*.*)\0*.*\0",
    FALSE,
    NULL,
    NULL
  },
  { ".txt", NULL,
    "ASCII text (*.TXT)\0*.TXT\0All files (*.*)\0*.*\0",
    FALSE,
    NULL,
    NULL
  },
  { ".tvc", NULL,
    "Elektor Program (*.TVC)\0*.TVC\0All files (*.*)\0*.*\0",
    TRUE,
    "tvc_ext",
    "Elektor TV Games Computer program"
  },
  { ".mdcr", NULL,
    "Mini Digital Cassette Recorder tape (*.MDCR)\0*.MDCR\0All files (*.*)\0*.*\0",
    FALSE,
    "mdcr_ext",
    "Philips Mini Digital Cassette Recorder tape"
  },
  { ".raw", NULL,
    "BINBUG/CD2650 floppy disk (*.RAW)\0*.RAW\0All files (*.*)\0*.*\0",
    FALSE,
    "raw_ext",
    "BINBUG/CD2650 floppy disk"
  },
  { ".cmd", NULL,
    "BINBUG command (*.CMD)\0*.CMD\0All files (*.*)\0*.*\0",
    FALSE,
    "cmd_ext",
    "BINBUG command"
  },
  { ".img", NULL,
    "TWIN floppy disk (*.IMG)\0*.IMG\0All files (*.*)\0*.*\0",
    FALSE,
    "img_ext",
    "TWIN floppy disk"
  },
  { ".twin", NULL,
    "TWIN floppy disk (*.TWIN)\0*.TWIN\0All files (*.*)\0*.*\0",
    FALSE,
    "twin_ext",
    "TWIN floppy disk"
  },
  { ".mod", NULL,
    "TWIN command (*.MOD)\0*.MOD\0All files (*.*)\0*.*\0",
    FALSE,
    "mod_ext",
    "TWIN command"
  },
  { ".sym", NULL,
    "Symbol table (*.SYM)\0*.SYM\0All files (*.*)\0*.*\0",
    FALSE,
    "sym_ext",
    "Symbol table"
  },
  { ".imag", NULL,
    "CD2650 command (*.IMAG)\0*.SYM\0All files (*.*)\0*.*\0",
    FALSE,
    "imag_ext",
    "CD2650 command"
  },
  { ".ico", ".info",
    "Icon (*.ICO, *.INFO)\0*.ICO;*.INFO\0All files (*.*)\0*.*\0",
    FALSE,
    "ico_ext",
    "Icon"
  },
  { ".16sv", NULL,
    "IFF 16SV (*.16SV)\0*.16SV\0All files (*.*)\0*.*\0",
    FALSE,
    "16sv_ext",
    "16-bit sampled sound",
  },
  { ".bpnf", ".bnpf",
    "Begin Plus Negative Finish (*.BPNF, *.BNPF)\0*.BPNF;*.BNPF\0All files (*.*)\0*.*\0",
    FALSE,
    "bpnf_ext",
    "Begin Plus Negative Finish file"
  },
  { ".sms", NULL,
    "PROM burner file (*.SMS)\0*.SMS\0All files (*.*)\0*.*\0",
    FALSE,
    "sms_ext",
    "PROM burner file",
  },
  { ".pap", NULL,
    "Papertape (*.PAP)\0*.PAP\0All files (*.*)\0*.*\0",
    FALSE,
    "pap_ext",
    "Papertape",
} };
// filekind[] *must* be fully populated (on both hosts)!

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       FLAG                      aborting,
                                       incli,
                                       modal,
                                       opening,
                                       preserve,
                                       skiphostname,
                                       win7,
                                       yank;
IMPORT       TEXT                      bpnf_string[LABELLIMIT + 1],
                                       fn_bkgrnd[MAX_PATH + 1],
                                       fromstr[5 + 1 + 1],
                                       gtempstring[256 + 1],
                                       hostname[256 + 1],
                                       joyname[2][MAX_PATH],
                                       path_bkgrnd[MAX_PATH + 1],
                                       path_disks[MAX_PATH + 1],
                                       path_games[MAX_PATH + 1],
                                       path_projects[MAX_PATH + 1],
                                       path_screenshots[MAX_PATH + 1],
                                       path_scripts[MAX_PATH + 1],
                                       path_tapes[MAX_PATH + 1],
                                       ProgName[MAX_PATH + 1],
                                       tostr[17 + 1 + 1];
IMPORT       UBYTE                     button[2][8];
IMPORT       UWORD                     console[4],
                                       keypads[2][NUMKEYS],
                                       temp_console[4],
                                       temp_keypads[2][NUMKEYS];
IMPORT       ULONG                     confirmable,
                                       showpalladiumkeys1,
                                       showpalladiumkeys2,
                                       sound,
                                       speech_rate,
                                       swapped,
                                       usespeech,
                                       viewpadsas;
IMPORT       STRPTR                    rexxwhere;
IMPORT       int                       binbug_biosver,
                                       bitrate,
                                       CatalogPtr,
                                       colourset,
                                       hostvolume,
                                       language,
                                       machine,
                                       memmap,
                                       nexttod,
                                       reassociate,
                                       recmode,
                                       samplerate,
                                       sensitivity[2],
                                       showpointers[2],
                                       showtod,
                                       speakable,
                                       tolimit,
                                       tonum,
                                       whichgame,
                                       wsm;
IMPORT       HBRUSH                    hBrush[EMUBRUSHES];
IMPORT       HFONT                     hBoldFont;
IMPORT       HINSTANCE                 InstancePtr;
IMPORT       HWND                      ListeningWindowPtr,
                                       MainWindowPtr,
                                       SubWindowPtr[SUBWINDOWS];
IMPORT       struct HiScoreStruct      hiscore[HISCORES];
IMPORT       struct HostMachineStruct  hostmachines[MACHINES];
IMPORT       struct LangStruct         langs[LANGUAGES];
IMPORT       struct KeyNameStruct      keyname[SCANCODES];
IMPORT       struct MachineStruct      machines[MACHINES];
IMPORT       UBYTE*                    pixelubyte;
IMPORT       ULONG*                    pixelulong;
IMPORT const UWORD                     default_console[4],
                                       default_keypads[2][NUMKEYS];
IMPORT const struct KnownStruct        known[KNOWNGAMES];
IMPORT const struct MemMapToStruct     memmap_to[MEMMAPS];
IMPORT const struct TODStruct          tods[TIPSOFDAYS];
#ifdef WIN32
    IMPORT   TEXT                      file_bkgrnd[MAX_PATH + 1];
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE       FLAG                      bpnf_ok,
                                       confirmed,
                                       pokeok;
MODULE       TEXT                      temppath_disks[MAX_PATH + 1],
                                       temppath_projects[MAX_PATH + 1],
                                       temppath_screenshots[MAX_PATH + 1],
                                       temppath_scripts[MAX_PATH + 1],
                                       temppath_tapes[MAX_PATH + 1],
                                       SysInfoName[MAX_PATH + 1];
MODULE       UBYTE                     tempbutton[2][8];
MODULE       int                       newbitrate,
                                       newsamplerate,
                                       newspeechrate,
                                       newusespeech,
                                       newvolume,
                                       side,
                                       whichkey;
MODULE       STRPTR                    editstring;
MODULE       HWND                      ModalWindowPtr    = NULL,
                                       RedefineWindowPtr = NULL;
#ifdef WIN32
    MODULE   TEXT                      tempfile_bkgrnd[MAX_PATH + 1],
                                       temppath_bkgrnd[MAX_PATH + 1];
#endif

MODULE const struct
{   UBYTE rearr[4];
    DWORD hGad[2];
} rearrange[48] = {
{ { 1,2,3,4 }, { IDC_1STRA1234, IDC_2NDRA1234 } },
{ { 1,2,4,3 }, { IDC_1STRA1243, IDC_2NDRA1243 } },
{ { 1,3,2,4 }, { IDC_1STRA1324, IDC_2NDRA1324 } },
{ { 1,3,4,2 }, { IDC_1STRA1342, IDC_2NDRA1342 } },
{ { 1,4,2,3 }, { IDC_1STRA1423, IDC_2NDRA1423 } },
{ { 1,4,3,2 }, { IDC_1STRA1432, IDC_2NDRA1432 } },
{ { 2,1,3,4 }, { IDC_1STRA2134, IDC_2NDRA2134 } },
{ { 2,1,4,3 }, { IDC_1STRA2143, IDC_2NDRA2143 } },
{ { 2,3,1,4 }, { IDC_1STRA2314, IDC_2NDRA2314 } },
{ { 2,3,4,1 }, { IDC_1STRA2341, IDC_2NDRA2341 } },
{ { 2,4,1,3 }, { IDC_1STRA2413, IDC_2NDRA2413 } },
{ { 2,4,3,1 }, { IDC_1STRA2431, IDC_2NDRA2431 } },
{ { 3,1,2,4 }, { IDC_1STRA3124, IDC_2NDRA3124 } },
{ { 3,1,4,2 }, { IDC_1STRA3142, IDC_2NDRA3142 } },
{ { 3,2,1,4 }, { IDC_1STRA3214, IDC_2NDRA3214 } },
{ { 3,2,4,1 }, { IDC_1STRA3241, IDC_2NDRA3241 } },
{ { 3,4,1,2 }, { IDC_1STRA3412, IDC_2NDRA3412 } },
{ { 3,4,2,1 }, { IDC_1STRA3421, IDC_2NDRA3421 } },
{ { 4,1,2,3 }, { IDC_1STRA4123, IDC_2NDRA4123 } },
{ { 4,1,3,2 }, { IDC_1STRA4132, IDC_2NDRA4132 } },
{ { 4,2,1,3 }, { IDC_1STRA4213, IDC_2NDRA4213 } },
{ { 4,2,3,1 }, { IDC_1STRA4231, IDC_2NDRA4231 } },
{ { 4,3,1,2 }, { IDC_1STRA4312, IDC_2NDRA4312 } },
{ { 4,3,2,1 }, { IDC_1STRA4321, IDC_2NDRA4321 } },
{ { 5,6,7,8 }, { IDC_1STRA5678, IDC_2NDRA5678 } },
{ { 5,6,8,7 }, { IDC_1STRA5687, IDC_2NDRA5687 } },
{ { 5,7,6,8 }, { IDC_1STRA5768, IDC_2NDRA5768 } },
{ { 5,7,8,6 }, { IDC_1STRA5786, IDC_2NDRA5786 } },
{ { 5,8,6,7 }, { IDC_1STRA5867, IDC_2NDRA5867 } },
{ { 5,8,7,6 }, { IDC_1STRA5876, IDC_2NDRA5876 } },
{ { 6,5,7,8 }, { IDC_1STRA6578, IDC_2NDRA6578 } },
{ { 6,5,8,7 }, { IDC_1STRA6587, IDC_2NDRA6587 } },
{ { 6,7,5,8 }, { IDC_1STRA6758, IDC_2NDRA6758 } },
{ { 6,7,8,5 }, { IDC_1STRA6785, IDC_2NDRA6785 } },
{ { 6,8,5,7 }, { IDC_1STRA6857, IDC_2NDRA6857 } },
{ { 6,8,7,5 }, { IDC_1STRA6875, IDC_2NDRA6875 } },
{ { 7,5,6,8 }, { IDC_1STRA7568, IDC_2NDRA7568 } },
{ { 7,5,8,6 }, { IDC_1STRA7586, IDC_2NDRA7586 } },
{ { 7,6,5,8 }, { IDC_1STRA7658, IDC_2NDRA7658 } },
{ { 7,6,8,5 }, { IDC_1STRA7685, IDC_2NDRA7685 } },
{ { 7,8,5,6 }, { IDC_1STRA7856, IDC_2NDRA7856 } },
{ { 7,8,6,5 }, { IDC_1STRA7865, IDC_2NDRA7865 } },
{ { 8,5,6,7 }, { IDC_1STRA8567, IDC_2NDRA8567 } },
{ { 8,5,7,6 }, { IDC_1STRA8576, IDC_2NDRA8576 } },
{ { 8,6,5,7 }, { IDC_1STRA8657, IDC_2NDRA8657 } },
{ { 8,6,7,5 }, { IDC_1STRA8675, IDC_2NDRA8675 } },
{ { 8,7,5,6 }, { IDC_1STRA8756, IDC_2NDRA8756 } },
{ { 8,7,6,5 }, { IDC_1STRA8765, IDC_2NDRA8765 } }
};

MODULE struct
{   const int redef[2],
              gameinfo[2];
} ctrlgads[ALLREDEFINEGADS] = {
{ { IDC_KEY_START   , IDC_KEY_START   }, { -1       , -1        } },
{ { IDC_KEY_A       , IDC_KEY_A       }, { -1       , -1        } },
{ { IDC_KEY_B       , IDC_KEY_B       }, { -1       , -1        } },
{ { IDC_KEY_RESET   , IDC_KEY_RESET   }, { -1       , -1        } },
{ { -1              , IDC_P1_X1       }, { -1       , IDC_LT_X1 } },
{ { -1              , IDC_P1_X2       }, { -1       , IDC_LT_X2 } },
{ { -1              , IDC_P1_X3       }, { -1       , IDC_LT_X3 } },
{ { -1              , IDC_P1_X4       }, { -1       , IDC_LT_X4 } },
{ { -1              , IDC_P2_X1       }, { -1       , IDC_RT_X1 } },
{ { -1              , IDC_P2_X2       }, { -1       , IDC_RT_X2 } },
{ { -1              , IDC_P2_X3       }, { -1       , IDC_RT_X3 } },
{ { -1              , IDC_P2_X4       }, { -1       , IDC_RT_X4 } },
{ { IDC_P1_1ST      , IDC_P1_1ST      }, { -1       , -1        } },
{ { IDC_P1_2ND      , IDC_P1_2ND      }, { -1       , -1        } },
{ { IDC_P1_3RD      , IDC_P1_3RD      }, { -1       , -1        } },
{ { IDC_P1_4TH      , IDC_P1_4TH      }, { -1       , -1        } },
{ { IDC_P2_1ST      , IDC_P2_1ST      }, { -1       , -1        } },
{ { IDC_P2_2ND      , IDC_P2_2ND      }, { -1       , -1        } },
{ { IDC_P2_3RD      , IDC_P2_3RD      }, { -1       , -1        } },
{ { IDC_P2_4TH      , IDC_P2_4TH      }, { -1       , -1        } },
{ { IDC_P1_LEFT     , IDC_P1_LEFT     }, { -1       , -1        } },
{ { IDC_P1_RIGHT    , IDC_P1_RIGHT    }, { -1       , -1        } },
{ { IDC_P1_UP       , IDC_P1_UP       }, { -1       , -1        } },
{ { IDC_P1_DOWN     , IDC_P1_DOWN     }, { -1       , -1        } },
{ { IDC_P1_UPLEFT   , IDC_P1_UPLEFT   }, { -1       , -1        } },
{ { IDC_P1_UPRIGHT  , IDC_P1_UPRIGHT  }, { -1       , -1        } },
{ { IDC_P1_DOWNLEFT , IDC_P1_DOWNLEFT }, { -1       , -1        } },
{ { IDC_P1_DOWNRIGHT, IDC_P1_DOWNRIGHT}, { -1       , -1        } },
{ { IDC_P2_LEFT     , IDC_P2_LEFT     }, { -1       , -1        } },
{ { IDC_P2_RIGHT    , IDC_P2_RIGHT    }, { -1       , -1        } },
{ { IDC_P2_UP       , IDC_P2_UP       }, { -1       , -1        } },
{ { IDC_P2_DOWN     , IDC_P2_DOWN     }, { -1       , -1        } },
{ { IDC_P2_UPLEFT   , IDC_P2_UPLEFT   }, { -1       , -1        } },
{ { IDC_P2_UPRIGHT  , IDC_P2_UPRIGHT  }, { -1       , -1        } },
{ { IDC_P2_DOWNLEFT , IDC_P2_DOWNLEFT }, { -1       , -1        } },
{ { IDC_P2_DOWNRIGHT, IDC_P2_DOWNRIGHT}, { -1       , -1        } },
{ { IDC_P1_1        , IDC_P1_1        }, { IDC_LT_1 , IDC_LT_1  } },
{ { IDC_P1_2        , IDC_P1_2        }, { IDC_LT_2 , IDC_LT_2  } },
{ { IDC_P1_3        , IDC_P1_3        }, { IDC_LT_3 , IDC_LT_3  } },
{ { IDC_P1_4        , IDC_P1_4        }, { IDC_LT_4 , IDC_LT_4  } },
{ { IDC_P1_5        , IDC_P1_5        }, { IDC_LT_5 , IDC_LT_5  } },
{ { IDC_P1_6        , IDC_P1_6        }, { IDC_LT_6 , IDC_LT_6  } },
{ { IDC_P1_7        , IDC_P1_7        }, { IDC_LT_7 , IDC_LT_7  } },
{ { IDC_P1_8        , IDC_P1_8        }, { IDC_LT_8 , IDC_LT_8  } },
{ { IDC_P1_9        , IDC_P1_9        }, { IDC_LT_9 , IDC_LT_9  } },
{ { IDC_P1_C        , IDC_P1_C        }, { IDC_LT_CL, IDC_LT_CL } },
{ { IDC_P1_0        , IDC_P1_0        }, { IDC_LT_0 , IDC_LT_0  } },
{ { IDC_P1_E        , IDC_P1_E        }, { IDC_LT_EN, IDC_LT_EN } },
{ { IDC_P2_1        , IDC_P2_1        }, { IDC_RT_1 , IDC_RT_1  } },
{ { IDC_P2_2        , IDC_P2_2        }, { IDC_RT_2 , IDC_RT_2  } },
{ { IDC_P2_3        , IDC_P2_3        }, { IDC_RT_3 , IDC_RT_3  } },
{ { IDC_P2_4        , IDC_P2_4        }, { IDC_RT_4 , IDC_RT_4  } },
{ { IDC_P2_5        , IDC_P2_5        }, { IDC_RT_5 , IDC_RT_5  } },
{ { IDC_P2_6        , IDC_P2_6        }, { IDC_RT_6 , IDC_RT_6  } },
{ { IDC_P2_7        , IDC_P2_7        }, { IDC_RT_7 , IDC_RT_7  } },
{ { IDC_P2_8        , IDC_P2_8        }, { IDC_RT_8 , IDC_RT_8  } },
{ { IDC_P2_9        , IDC_P2_9        }, { IDC_RT_9 , IDC_RT_9  } },
{ { IDC_P2_C        , IDC_P2_C        }, { IDC_RT_CL, IDC_RT_CL } },
{ { IDC_P2_0        , IDC_P2_0        }, { IDC_RT_0 , IDC_RT_0  } },
{ { IDC_P2_E        , IDC_P2_E        }, { IDC_RT_EN, IDC_RT_EN } }
};

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void refreshkeys(void);
MODULE void subreq(void);
MODULE void ghost_soundgads(HWND hwnd);
MODULE void ghost_resetpaths(HWND hwnd);
MODULE FLAG zgetpath(STRPTR buffer);
MODULE void update_rearrange(void);
MODULE void drawhiscores(void);

MODULE BOOL CALLBACK       AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK        BPNFDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK     ConfirmDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK        EditDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK    HiScoresDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK    HostNameDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK   ListeningDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK       PathsDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK RedefineSubDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK    RegisterDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK        REXXDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK       SoundDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
MODULE BOOL CALLBACK    TipOfDayDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

// CODE-------------------------------------------------------------------

MODULE BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   HKEY hKey;
    int  localsize = MAX_PATH;
 // TEXT portstr[80 + 1];

    switch (Message)
    {
    case WM_INITDIALOG:
        DISCARD SetWindowText(hwnd, LLL( MSG_HAIL_ABOUT2,        "About WinArcadia"));
        setdlgtext(hwnd, IDC_DESC,       MSG_DESC,               "An emulator of Signetics-based machines");
        setdlgtext(hwnd, IDC_TRANSLATOR, MSG_TRANSLATOR,         "English translation by James Jacobs");
        setdlgtext(hwnd, IDC_SYSINFO,    MSG_SYSTEM_INFORMATION, "System information...");
        setdlgtext(hwnd, IDOK,           MSG_OK2,                "&OK");
        setfont(   hwnd, IDC_ABOUT);
        DISCARD SetDlgItemText(hwnd, IDC_ABOUT,      TITLEBARTEXT);
        DISCARD SetDlgItemText(hwnd, IDL_DATE,       LONGDATE);
/*      sprintf
        (   portstr,
            LLL( MSG_PORTER, "%s port by %s"),
            "Windows",
            "James Jacobs"
        );
        DISCARD SetDlgItemText(hwnd, IDC_PORTER, LLL(MSG_PORTER, portstr)); */

        if (RegOpenKeyEx
        (   HKEY_LOCAL_MACHINE,
            "SOFTWARE\\Microsoft\\Shared Tools\\MSInfo",
            0,
            KEY_READ,
            &hKey
        ) == ERROR_SUCCESS)
        {   if (RegQueryValueEx
            (   hKey,
                "Path",
                0,
                NULL,
                SysInfoName,
                (LPDWORD) &localsize
            ) == ERROR_SUCCESS && SysInfoName[0])
            {   EnableWindow(GetDlgItem(hwnd, IDC_SYSINFO), TRUE);
            }
            DISCARD RegCloseKey(hKey);
        }

        DISCARD SendMessage(GetDlgItem(hwnd, IDC_ABOUT), WM_SETFONT, (WPARAM) hBoldFont, MAKELPARAM(TRUE, 0));

        DISCARD SetFocus(GetDlgItem(hwnd, IDOK));
        return FALSE; // must be FALSE so that our SetFocus()sing is respected
    case WM_CLOSE: // no need for acase
        clearkybd();
        DISCARD EndDialog(hwnd, 0);
        return TRUE;
    case WM_DRAWITEM: // no need for acase
        drawitem((struct tagDRAWITEMSTRUCT*) lParam, FALSE);
        return TRUE;
    case WM_COMMAND: // no need for acase
        switch (LOWORD(wParam))
        {
        case IDC_LINK1:
        case IDC_AMIGANSOFTWARE:
            openurl("http://amigan.1emu.net/releases/");
        acase IDC_LINK2:
            openurl("mailto:amigansoftware@gmail.com");
        acase IDC_LINK3:
        case IDC_EA2001CENTRAL:
            openurl("http://amigan.yatho.com/");
        acase IDC_SYSINFO:
            openurl(SysInfoName);
        acase IDOK:
            clearkybd();
            DISCARD EndDialog(hwnd, 0);
        }
        return TRUE;
    default: // no need for adefault
        return FALSE;
}   }

EXPORT void rq(STRPTR text)
{   TEXT hailstring[80 + 1];

    sound_off(FALSE);
    modal = TRUE;
    settitle();

    sprintf(hailstring, "WinArcadia: %s", LLL(MSG_HAIL_FATALERROR, "Fatal Error"));
    MessageBoxEx(MainWindowPtr, text, hailstring, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL, (WORD) MAKELANGID(langs[language].lang, langs[language].sublang)); // button is still always in English for some reason

    sound_on(TRUE);
    modal = FALSE;
    settitle();

    cleanexit(EXIT_FAILURE);
}
EXPORT void say(STRPTR text)
{   TEXT hailstring[80 + 1];

    sound_off(FALSE);
    modal = TRUE;
    settitle();

    sprintf(hailstring, "WinArcadia: %s", LLL(MSG_HAIL_WARNING, "Warning"));
    MessageBoxEx(MainWindowPtr, text, hailstring, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL, (WORD) MAKELANGID(langs[language].lang, langs[language].sublang)); // button is still always in English for some reason

    sound_on(TRUE);
    modal = FALSE;
    settitle();
}
EXPORT void saymessage(STRPTR text)
{   TEXT hailstring[80 + 1];

    sound_off(FALSE);
    modal = TRUE;
    settitle();

    sprintf(hailstring, "WinArcadia: %s", LLL(MSG_HAIL_MESSAGE, "Message"));
    MessageBoxEx(MainWindowPtr, text, hailstring, MB_OK | MB_ICONINFORMATION | MB_APPLMODAL, (WORD) MAKELANGID(langs[language].lang, langs[language].sublang)); // button is still always in English for some reason

    sound_on(TRUE);
    modal = FALSE;
    settitle();
}

EXPORT int asl
(   STRPTR hail,
    STRPTR pattern,
    FLAG   save,
    STRPTR startdir,
    STRPTR startfile,
    STRPTR pathname
)
{   OPENFILENAME ofn;

    strcpy(pathname, startfile);
 /* zprintf(TEXTPEN_VERBOSE, "startdir  is %s!\n", startdir);
    zprintf(TEXTPEN_VERBOSE, "startfile is %s!\n", startfile);
    zprintf(TEXTPEN_VERBOSE, "pathname  is %s!\n", pathname); */

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize     = sizeof(ofn);
    ofn.hwndOwner       = MainWindowPtr;
    ofn.lpstrTitle      = hail;
    ofn.lpstrFilter     = pattern;
    ofn.lpstrInitialDir = startdir;
    ofn.lpstrFile       = pathname;
    ofn.nMaxFile        = MAX_PATH;
    if (save)
    {   ofn.Flags       = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    } else
    {   ofn.Flags       = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    }

    sound_off(FALSE);
    if (!showpointers[wsm])
    {   showpointers[wsm] = TRUE;
        docommand(MENUITEM_POINTER);
    }
    clearkybd();
    opening = TRUE; // we don't bother saying "(close subwindow)" for ASL requesters
    if
    (   ( save && GetSaveFileName(&ofn))
     || (!save && GetOpenFileName(&ofn))
    )
    {   opening = FALSE;
        sound_on(TRUE);
        return EXIT_SUCCESS;
    } else
    {   opening = FALSE;
        sound_on(TRUE);
        return EXIT_FAILURE;
}   }

EXPORT FLAG listen_open(void)
{   opening = modal = TRUE;
    settitle();
    ListeningWindowPtr = CreateDialog
    (   InstancePtr,                    // handle to application instance
        MAKEINTRESOURCE(IDD_LISTENING), // identifies dialog box template name
        MainWindowPtr,
        ListeningDlgProc
    );
    opening = modal = FALSE;
    settitle();

    return TRUE;
}

EXPORT void hostnamewindow(void)
{   if (skiphostname) // called from REXX with hostname already supplied
    {   return;
    }

    opening = modal = TRUE;
    settitle();
    DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_HOSTNAME), MainWindowPtr, HostNameDlgProc);
    opening = modal = FALSE;
    settitle();
}

EXPORT void listen_close(void)
{   if (!ListeningWindowPtr) // important!
    {   return;
    }

    DestroyWindow(ListeningWindowPtr);
    ListeningWindowPtr = NULL;

    sound_on(TRUE);
}

EXPORT void project_register(void)
{   sound_off(FALSE);
    opening = modal = TRUE;
    settitle();
    DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_REGISTER), MainWindowPtr, RegisterDlgProc);
    opening = modal = FALSE;
    settitle();
    sound_on(TRUE);
}

EXPORT FLAG associate(int kind)
{   TRANSIENT HKEY   hKey;
    TRANSIENT DWORD* lRtn;
    TRANSIENT LONG   rc;
    TRANSIENT STRPTR extension[2];
    TRANSIENT int    extensions,
                     i;
    PERSIST   TEXT   errormsg[256 + 1],
                     tempstring[MAX_PATH + 7 + 1]; // both PERSISTent so as not to blow the stack

    extension[0] = filekind[kind].extension;
    if (filekind[kind].extension2)
    {   extensions = 2;
        extension[1] = filekind[kind].extension2;
    } else
    {   extensions = 1;
    }

    for (i = 0; i < extensions; i++)
    {   // create the new registry key for the file extension
        rc = RegCreateKeyEx
        (   HKEY_CLASSES_ROOT,
            extension[i],
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS,
            0,
            (PHKEY) &hKey,
            (LPDWORD) &lRtn
        );
        if (rc != ERROR_SUCCESS)
        {   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, rc, 0, errormsg, 80, NULL);
            zprintf(TEXTPEN_ERROR, "RegCreateKeyEx(%s) error: %s", extension[i], errormsg);
            return FALSE;
        }
        // set "(default)" string value to identifier
        rc = RegSetValueEx
        (   hKey,
            NULL,
            0,
            REG_SZ,
            filekind[kind].extname,
            strlen(filekind[kind].extname) + 1
        );
        if (rc != ERROR_SUCCESS)
        {   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, rc, 0, errormsg, 80, NULL);
            zprintf(TEXTPEN_ERROR, "RegSetValueEx(%s) error: %s", extension[i], errormsg);
            DISCARD RegCloseKey(hKey);
            return FALSE;
        }
        DISCARD RegCloseKey(hKey);

        // create the new registry key, the file extension identifier
        rc = RegCreateKeyEx
        (   HKEY_CLASSES_ROOT,
            filekind[kind].extname,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS,
            0,
            (PHKEY) &hKey,
            (LPDWORD) &lRtn
        );
        if (rc != ERROR_SUCCESS)
        {   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, rc, 0, errormsg, 80, NULL);
            zprintf(TEXTPEN_ERROR, "RegCreateKeyEx(%s) error: %s", extension[i], errormsg);
            return FALSE;
        }
        // set "(default)" string value to description of the file type
        rc = RegSetValueEx
        (   hKey,
            NULL,
            0,
            REG_SZ,
            filekind[kind].desc,
            strlen(filekind[kind].desc) + 1
        );
        if (rc != ERROR_SUCCESS)
        {   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, rc, 0, errormsg, 80, NULL);
            zprintf(TEXTPEN_ERROR, "RegSetValueEx(%s) error: %s", extension[i], errormsg);
            DISCARD RegCloseKey(hKey);
            return FALSE;
        }
        DISCARD RegCloseKey(hKey);

        strcpy(tempstring, filekind[kind].extname);
        strcat(tempstring, "\\DefaultIcon");
        // create the new registry key, the default icon key within the identifier key
        rc = RegCreateKeyEx
        (   HKEY_CLASSES_ROOT,
            tempstring,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS,
            0,
            (PHKEY) &hKey,
            (LPDWORD) &lRtn
        );
        if (rc != ERROR_SUCCESS)
        {   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, rc, 0, errormsg, 80, NULL);
            zprintf(TEXTPEN_ERROR, "RegCreateKeyEx(%s) error: %s", extension[i], errormsg);
            return FALSE;
        }
        strcpy(tempstring, ProgName);
        // set "(default)" string value to the full path name of the icon that
        // will be associated with this filetype
        rc = RegSetValueEx
        (   hKey,
            NULL,
            0,
            REG_SZ,
            tempstring,
            strlen(tempstring) + 1
        );
        if (rc != ERROR_SUCCESS)
        {   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, rc, 0, errormsg, 80, NULL);
            zprintf(TEXTPEN_ERROR, "RegSetValueEx(%s) error: %s", extension[i], errormsg);
            DISCARD RegCloseKey(hKey);
            return FALSE;
        }
        DISCARD RegCloseKey(hKey);

        strcpy(tempstring, filekind[kind].extname);
        strcat(tempstring, "\\shell");
        // create the new registry key, the "shell" key within the identifier key
        rc = RegCreateKeyEx
        (   HKEY_CLASSES_ROOT,
            tempstring,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS,
            0,
            (PHKEY) &hKey,
            (LPDWORD) &lRtn
        );
        if (rc != ERROR_SUCCESS)
        {   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, rc, 0, errormsg, 80, NULL);
            zprintf(TEXTPEN_ERROR, "RegCreateKeyEx(%s) error: %s", extension[i], errormsg);
            return FALSE;
        }
        DISCARD RegCloseKey(hKey);

        strcat(tempstring, "\\open"); // now foo_ext\shell\open
        // create the new registry key, the "open" command key within the
        // shell key
        rc = RegCreateKeyEx
        (   HKEY_CLASSES_ROOT,
            tempstring,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS,
            0,
            (PHKEY) &hKey,
            (LPDWORD) &lRtn
        );
        if (rc != ERROR_SUCCESS)
        {   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, rc, 0, errormsg, 80, NULL);
            zprintf(TEXTPEN_ERROR, "RegCreateKeyEx(%s) error: %s", extension[i], errormsg);
            return FALSE;
        }
        DISCARD RegCloseKey(hKey);

        strcat(tempstring, "\\command"); // now foo_ext\shell\open\command
        // create the new registry key, the "command" key within the "open"
        // command key
        rc = RegCreateKeyEx
        (   HKEY_CLASSES_ROOT,
            tempstring,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS,
            0,
            (PHKEY) &hKey,
            (LPDWORD) &lRtn
        );
        if (rc != ERROR_SUCCESS)
        {   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, rc, 0, errormsg, 80, NULL);
            zprintf(TEXTPEN_ERROR, "RegCreateKeyEx(%s) error: %s", extension[i], errormsg);
            return FALSE;
        }
        strcpy(tempstring, "\"");        // now "
        strcat(tempstring, ProgName);    // now "C:\E\S\WinArcadia\WinArcadia.exe
        strcat(tempstring, "\" \"%1\""); // now "C:\E\S\WinArcadia\WinArcadia.exe" "%1"
        // set "(default)" string value of the "command" key to the command
        // line to be used to open the file
        rc = RegSetValueEx
        (   hKey,
            NULL,
            0,
            REG_SZ,
            tempstring,
            strlen(tempstring) + 1
        );
        if (rc != ERROR_SUCCESS)
        {   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, rc, 0, errormsg, 80, NULL);
            zprintf(TEXTPEN_ERROR, "RegSetValueEx(%s) error: %s", extension[i], errormsg);
            DISCARD RegCloseKey(hKey);
            return FALSE;
        }
        DISCARD RegCloseKey(hKey);

        zprintf(TEXTPEN_CLIOUTPUT, "Associated %s.\n", extension[i]);
    }

    return TRUE;
}

MODULE void disassociate(int kind)
{
#ifdef DISASSOCIATE
    HKEY hKey1, // HKEY_CLASSES_ROOT/foo_ext, HKEY_CLASSES_ROOT/.FOO
         hKey2, // HKEY_CLASSES_ROOT/foo_ext/shell
         hKey3; // HKEY_CLASSES_ROOT/foo_ext/shell/open

    if (RegOpenKeyEx(HKEY_CLASSES_ROOT, filekind[kind].extname, 0, KEY_ALL_ACCESS, &hKey1) == ERROR_SUCCESS)
    {
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Opened HKEY_CLASSES_ROOT/%s\n", filekind[kind].extname);
#endif
        if (RegDeleteKey(hKey1, "DefaultIcon") == ERROR_SUCCESS)
        {   ;
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "Deleted HKEY_CLASSES_ROOT/%s/DefaultIcon\n", filekind[kind].extname);
#endif
        } else
        {   ;
#ifdef VERBOSE
            zprintf(TEXTPEN_ERROR, "Can't delete HKEY_CLASSES_ROOT/%s/DefaultIcon\n", filekind[kind].extname);
#endif
        }
        if (RegOpenKeyEx(hKey1, "shell", 0, KEY_ALL_ACCESS, &hKey2) == ERROR_SUCCESS)
        {
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "Opened HKEY_CLASSES_ROOT/%s/shell\n", filekind[kind].extname);
#endif
            if (RegOpenKeyEx(hKey2, "open", 0, KEY_ALL_ACCESS, &hKey3) == ERROR_SUCCESS)
            {
#ifdef VERBOSE
                zprintf(TEXTPEN_VERBOSE, "Opened HKEY_CLASSES_ROOT/%s/shell/open\n", filekind[kind].extname);
#endif
                if (RegDeleteKey(hKey3, "command") == ERROR_SUCCESS)
                {   ;
#ifdef VERBOSE
                    zprintf(TEXTPEN_VERBOSE, "Deleted HKEY_CLASSES_ROOT/%s/shell/open/command\n", filekind[kind].extname);
#endif
                } else
                {   ;
#ifdef VERBOSE
                    zprintf(TEXTPEN_ERROR, "Can't delete HKEY_CLASSES_ROOT/%s/shell/open/command\n", filekind[kind].extname);
#endif
                }
                if (RegCloseKey(hKey3) == ERROR_SUCCESS)
                {   ;
#ifdef VERBOSE
                    zprintf(TEXTPEN_VERBOSE, "Closed HKEY_CLASSES_ROOT/%s/shell/open\n", filekind[kind].extname);
#endif
                } else
                {   ;
#ifdef VERBOSE
                    zprintf(TEXTPEN_ERROR, "Can't close HKEY_CLASSES_ROOT/%s/shell/open\n", filekind[kind].extname);
#endif
            }   }
            if (RegDeleteKey(hKey2, "open") == ERROR_SUCCESS)
            {   ;
#ifdef VERBOSE
                zprintf(TEXTPEN_VERBOSE, "Deleted HKEY_CLASSES_ROOT/%s/shell/open\n", filekind[kind].extname);
#endif
            } else
            {   ;
#ifdef VERBOSE
                zprintf(TEXTPEN_ERROR, "Can't delete HKEY_CLASSES_ROOT/%s/shell/open\n", filekind[kind].extname);
#endif
            }
            if (RegCloseKey(hKey2) == ERROR_SUCCESS)
            {   ;
#ifdef VERBOSE
                zprintf(TEXTPEN_VERBOSE, "Closed HKEY_CLASSES_ROOT/%s/shell\n", filekind[kind].extname);
#endif
            } else
            {   ;
#ifdef VERBOSE
                zprintf(TEXTPEN_ERROR, "Can't close HKEY_CLASSES_ROOT/%s/shell\n", filekind[kind].extname);
#endif
        }   }
        else
        {   ;
#ifdef VERBOSE
            zprintf(TEXTPEN_ERROR, "Can't open HKEY_CLASSES_ROOT/%s/shell\n", filekind[kind].extname);
#endif
        }
        if (RegDeleteKey(hKey1, "shell") == ERROR_SUCCESS)
        {   ;
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "Deleted HKEY_CLASSES_ROOT/%s/shell\n", filekind[kind].extname);
#endif
        } else
        {   ;
#ifdef VERBOSE
            zprintf(TEXTPEN_ERROR, "Can't delete HKEY_CLASSES_ROOT/%s/shell\n", filekind[kind].extname);
#endif
        }
        if (RegCloseKey(hKey1) == ERROR_SUCCESS)
        {   ;
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "Closed HKEY_CLASSES_ROOT/%s\n", filekind[kind].extname);
#endif
        } else
        {   ;
#ifdef VERBOSE
            zprintf(TEXTPEN_ERROR, "Can't close HKEY_CLASSES_ROOT/%s\n", filekind[kind].extname);
#endif
    }   }
    else
    {   ;
#ifdef VERBOSE
        zprintf(TEXTPEN_ERROR, "Can't open HKEY_CLASSES_ROOT/%s\n", filekind[kind].extname);
#endif
    }
    if (RegDeleteKey(HKEY_CLASSES_ROOT, filekind[kind].extname) == ERROR_SUCCESS)
    {   ;
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Deleted HKEY_CLASSES_ROOT/%s\n", filekind[kind].extname);
#endif
    } else
    {
#ifdef VERBOSE
        zprintf(TEXTPEN_ERROR, "Can't delete HKEY_CLASSES_ROOT/%s\n", filekind[kind].extname);
#endif
    }

    if (RegOpenKeyEx(HKEY_CLASSES_ROOT, filekind[kind].extension, 0, KEY_ALL_ACCESS, &hKey1) == ERROR_SUCCESS)
    {
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Opened HKEY_CLASSES_ROOT/%s\n", filekind[kind].extension);
#endif
        if (RegDeleteKey(hKey1, "(Default)") == ERROR_SUCCESS)
        {   ;
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "Deleted HKEY_CLASSES_ROOT/%s/(Default)\n", filekind[kind].extension);
#endif
        } else
        {   ;
#ifdef VERBOSE
            zprintf(TEXTPEN_ERROR, "Can't delete HKEY_CLASSES_ROOT/%s/(Default)\n", filekind[kind].extension);
#endif
        }
        if (RegCloseKey(hKey1) == ERROR_SUCCESS)
        {   ;
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "Closed HKEY_CLASSES_ROOT/%s\n", filekind[kind].extension);
#endif
        } else
        {   ;
#ifdef VERBOSE
            zprintf(TEXTPEN_ERROR, "Can't close HKEY_CLASSES_ROOT/%s\n", filekind[kind].extension);
#endif
    }   }
    else
    {   ;
#ifdef VERBOSE
        zprintf(TEXTPEN_ERROR, "Can't open HKEY_CLASSES_ROOT/%s\n", filekind[kind].extension);
#endif
    }
    if (RegDeleteKey(HKEY_CLASSES_ROOT, filekind[kind].extension) == ERROR_SUCCESS)
    {   ;
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Deleted HKEY_CLASSES_ROOT/%s\n", filekind[kind].extension);
#endif
    } else
    {   ;
#ifdef VERBOSE
        zprintf(TEXTPEN_ERROR, "Can't delete HKEY_CLASSES_ROOT/%s\n", filekind[kind].extension);
#endif
    }
#else
    ;
#endif
}

EXPORT void doassociations(void)
{   FLAG success = TRUE;
    int  i;

    for (i = 0; i < ASSOCIATIONS; i++)
    {   if (assoc[i].flag)
        {   if (!associate(assoc[i].kind))
            {   success = FALSE;
        }   }
        else
        {   disassociate(assoc[i].kind);
    }   }

    if (win7 && !success)
    {   zprintf(TEXTPEN_ERROR, LLL(MSG_ADMIN, "You need to run the emulator as an administrator to do this.\n"));
}   }

EXPORT void view_hiscores(void)
{   sound_off(FALSE);
    opening = modal = TRUE;
    settitle();
    if (machines[machine].coinop)
    {   DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_HISCORES_COINOP   ), MainWindowPtr, HiScoresDlgProc);
    } elif (machine == ARCADIA)
    {   DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_HISCORES_ARCADIA  ), MainWindowPtr, HiScoresDlgProc);
    } elif (machine == PONG)
    {   DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_HISCORES_PONG     ), MainWindowPtr, HiScoresDlgProc);
    } elif (machine == TYPERIGHT)
    {   DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_HISCORES_TYPERIGHT), MainWindowPtr, HiScoresDlgProc);
    }
    opening = modal = FALSE;
    settitle();
    sound_on(TRUE);
}

EXPORT void datestamp(int which)
{   SYSTEMTIME TheTime;

    GetLocalTime(&TheTime);
    sprintf
    (   hiscore[which].thedatetime,
        "%d%c%02d %d%c%d%c%02d",
        (int) TheTime.wHour,
        langs[language].timesep,  //   1
        (int) TheTime.wMinute,
        (int) (yank ? TheTime.wMonth : TheTime.wDay  ), // 2
        langs[language].datesep,  //   1
        (int) (yank ? TheTime.wDay   : TheTime.wMonth), // 2
        langs[language].datesep,  //   1
        (int) (TheTime.wYear % 100)
    );
}

MODULE BOOL CALLBACK EditDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   switch (Message)
    {
    case WM_INITDIALOG:
        DISCARD SetWindowText( hwnd, LLL(MSG_HAIL_EDIT, "Edit Register/Memory"));
        DISCARD SetDlgItemText(hwnd, IDC_CHANGE_ADDRESS, editstring);
        DISCARD SetDlgItemText(hwnd, IDC_CHANGE_FROM, fromstr);
        DISCARD SetDlgItemText(hwnd, IDC_CHANGE_TO,   tostr);
        setdlgtext(hwnd, IDL_CHANGE, MSG_CHANGE, "Change");
        setdlgtext(hwnd, IDL_FROM,   MSG_FROM,   "from");
        setdlgtext(hwnd, IDL_TO,     MSG_TO,     "to");

        // DISCARD SetForegroundWindow(hwnd); doesn't work

        DISCARD SetFocus(GetDlgItem(hwnd, IDC_CHANGE_TO));
        return TRUE;
    case WM_CLOSE: // no need for acase
        DISCARD GetWindowText(GetDlgItem(hwnd, IDC_CHANGE_TO), tostr, sizeof(tostr));

        pokeok = TRUE;
        clearkybd();
        DISCARD EndDialog(hwnd, 0);

        return TRUE;
    case WM_COMMAND: // no need for acase
        if (LOWORD (wParam) == IDOK)
        {   DISCARD GetWindowText(GetDlgItem(hwnd, IDC_CHANGE_TO), tostr, sizeof(tostr));

            pokeok = TRUE;
            clearkybd();
            DISCARD EndDialog(hwnd, 0);
            return TRUE;
        }
        if (LOWORD (wParam) == IDCANCEL) // eg Esc key
        {   pokeok = FALSE;
            clearkybd();
            DISCARD EndDialog(hwnd, 0);
            return TRUE;
        }

        return FALSE;
    default: // no need for adefault
        return FALSE;
}   }

MODULE BOOL CALLBACK BPNFDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   PAINTSTRUCT localps;

    switch (Message)
    {
    case WM_INITDIALOG:
        SetWindowText( hwnd, LLL(MSG_HAIL_BPNF, "Choose Load Address"));

        SetDlgItemText(hwnd, IDC_ADDRESS, bpnf_string);
    return TRUE;
    case WM_CLOSE: // no need for acase
        clearkybd();
        DISCARD EndDialog(hwnd, 0);
    return TRUE;
    case WM_DRAWITEM: // no need for acase
        drawitem((struct tagDRAWITEMSTRUCT*) lParam, FALSE);
    return TRUE;
    case WM_COMMAND: // no need for acase
        if (LOWORD(wParam) == IDOK)
        {   clearkybd();
            DISCARD EndDialog(hwnd, 0);
            bpnf_ok = TRUE;
            return TRUE;
        } elif (LOWORD(wParam) == IDCANCEL)
        {   clearkybd();
            DISCARD EndDialog(hwnd, 0);
            bpnf_ok = FALSE;
            return TRUE;
        }
    return FALSE;
    case WM_PAINT: // no need for acase
        DISCARD BeginPaint(hwnd, &localps);
        DISCARD EndPaint(hwnd, &localps);
    adefault:
    return FALSE;
    }

    return FALSE;
}

MODULE BOOL CALLBACK HiScoresDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   PAINTSTRUCT localps;

    switch (Message)
    {
    case WM_INITDIALOG:
        switch (machine)
        {
        case ARCADIA:
            DISCARD SetWindowText(hwnd, LLL( MSG_HAIL_HISCORES,          "Emerson Arcadia 2001 High Scores"));
        acase ZACCARIA:
        case MALZAK:
            DISCARD SetWindowText(hwnd, LLL( MSG_HAIL_COINOPHISCORES,    "Coin-Op High Scores"));
        acase PONG:
            DISCARD SetWindowText(hwnd, LLL( MSG_HAIL_PONGHISCORES,      "Pong High Scores"));
            setdlgtext(hwnd, IDL_PRACTICE,   MSG_VARIANT_PRACTICE,       "Practice");
            setdlgtext(hwnd, IDL_RIFLE1,     MSG_VARIANT_RIFLE1,         "Rifle #1");
            setdlgtext(hwnd, IDL_RIFLE2,     MSG_VARIANT_RIFLE2,         "Rifle #2");
            setdlgtext(hwnd, IDL_PRACTICE2,  MSG_VARIANT_PRACTICE,       "Practice");
            setdlgtext(hwnd, IDL_BBPRACTICE, MSG_VARIANT_BBPRACTICE,     "Basketball Practice");
            setdlgtext(hwnd, IDL_TARGET1,    MSG_VARIANT_TARGET1,        "1-Player Target");
        acase TYPERIGHT:
            DISCARD SetWindowText(hwnd, LLL( MSG_HAIL_TYPERIGHTHISCORES, "Type-right High Scores"));
        }
        setdlgtext(    hwnd, IDL_SCORE1,      MSG_SCORE,                 "Score");
        setdlgtext(    hwnd, IDL_ACHIEVEDON1, MSG_ACHIEVEDON,            "Achieved on");
        setdlgtext(    hwnd, IDL_ACHIEVEDBY1, MSG_ACHIEVEDBY,            "Achieved by");
        setdlgtext(    hwnd, IDL_PLAYEDFOR1,  MSG_PLAYEDFOR,             "Played for");
        if (machine == ARCADIA || machine == PONG)
        {   setdlgtext(hwnd, IDL_SCORE2,      MSG_SCORE,                 "Score");
            setdlgtext(hwnd, IDL_ACHIEVEDON2, MSG_ACHIEVEDON,            "Achieved on");
            setdlgtext(hwnd, IDL_ACHIEVEDBY2, MSG_ACHIEVEDBY,            "Achieved by");
            setdlgtext(hwnd, IDL_PLAYEDFOR2,  MSG_PLAYEDFOR,             "Played for");
        }
        setdlgtext(    hwnd, IDC_CLEARHS,     MSG_CLEARDATA,             "Clear data");
        setdlgtext(    hwnd, IDOK,            MSG_OK2,                   "&OK");

        ModalWindowPtr = hwnd;
        drawhiscores();

        if (enableclearhs())
        {   EnableWindow(GetDlgItem(hwnd, IDC_CLEARHS), TRUE);
        }
    return TRUE;
    case WM_CLOSE: // no need for acase
        clearkybd();
        DISCARD EndDialog(hwnd, 0);
    return TRUE;
    case WM_DRAWITEM: // no need for acase
        drawitem((struct tagDRAWITEMSTRUCT*) lParam, FALSE);
    return TRUE;
    case WM_COMMAND: // no need for acase
        if (LOWORD(wParam) == IDC_CLEARHS)
        {   clearhs();
            drawhiscores();
            EnableWindow(GetDlgItem(hwnd, IDC_CLEARHS), FALSE);
        } elif
        (   LOWORD(wParam) == IDOK
         || LOWORD(wParam) == IDCANCEL
        )
        {   clearkybd();
            DISCARD EndDialog(hwnd, 0);

            return TRUE;
        }
    return FALSE;
    case WM_PAINT: // no need for acase
        DISCARD BeginPaint(hwnd, &localps);
        DISCARD EndPaint(hwnd, &localps);
    adefault:
    return FALSE;
    }

    return FALSE;
}

MODULE BOOL CALLBACK HostNameDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   switch (Message)
    {
    case WM_INITDIALOG:
        DISCARD SetWindowText(hwnd, LLL(MSG_HAIL_HOSTNAME, "Enter Hostname"));
        DISCARD SetDlgItemText(hwnd, IDC_HOSTNAME, hostname);

        return TRUE;
    case WM_CLOSE: // no need for acase
        DISCARD GetWindowText(GetDlgItem(hwnd, IDC_HOSTNAME), hostname, sizeof(hostname));
        clearkybd();
        DISCARD EndDialog(hwnd, 0);

        return TRUE;
    case WM_COMMAND: // no need for acase
        if (LOWORD (wParam) == IDOK)
        {   DISCARD GetWindowText(GetDlgItem(hwnd, IDC_HOSTNAME), hostname, sizeof(hostname));
            clearkybd();
            DISCARD EndDialog(hwnd, 0);

            return TRUE;
        } elif (LOWORD (wParam) == IDCANCEL)
        {   hostname[0] = EOS;
            clearkybd();
            DISCARD EndDialog(hwnd, 0);

            return TRUE;
        }

        return FALSE;
    default: // no need for adefault
        return FALSE;
}   }

MODULE BOOL CALLBACK ListeningDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   switch (Message)
    {
    case WM_INITDIALOG:
        DISCARD SetWindowText(hwnd, LLL(MSG_HAIL_LISTENING, "Listening..."));

        return TRUE;
    case WM_ACTIVATE: // no need for acase
        do_autopause(wParam, lParam);
        return 0;
    case WM_CLOSE: // no need for acase
 // case WM_KEYDOWN:
        DestroyWindow(hwnd);
        ListeningWindowPtr = NULL;
        aborting = TRUE;
        return TRUE;
    default: // no need for adefault
        return FALSE;
}   }

EXPORT BOOL CALLBACK RearrangeDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   int i, j;
    
    switch (Message)
    {
    case WM_INITDIALOG:
        for (i = 0; i < 2; i++)
        {   for (j = 0; j < 8; j++)
            {   tempbutton[i][j] = button[i][j];
        }   }

        DISCARD SetWindowText(     hwnd,             LLL(MSG_HAIL_REARRANGE, "Rearrange Gamepad Buttons"));
        setfont(hwnd, IDL_1STPAD);
        setfont(hwnd, IDL_2NDPAD);
        if (joyname[0][0])
        {   DISCARD SetDlgItemText(hwnd, IDL_1STPAD, joyname[0]                     );
        } else
        {   DISCARD SetDlgItemText(hwnd, IDL_1STPAD, LLL(MSG_1STPAD2, "1st gamepad"));
        }
        if (joyname[1][0])
        {   DISCARD SetDlgItemText(hwnd, IDL_2NDPAD, joyname[1]                     );
        } else
        {   DISCARD SetDlgItemText(hwnd, IDL_2NDPAD, LLL(MSG_2NDPAD2, "2nd gamepad"));
        }

        for (i = 0; i < 24; i++)
        {   if
            (   button[0][0] == rearrange[i].rearr[0]
             && button[0][1] == rearrange[i].rearr[1]
             && button[0][2] == rearrange[i].rearr[2]
            )
            {   // assert(button[0][3] == rearrange[i].rearr[3]);
                DISCARD CheckRadioButton
                (   hwnd,
                    IDC_1STRA1234,
                    IDC_1STRA4321,
                    rearrange[i].hGad[0]
                );
            }
            if
            (   button[1][0] == rearrange[i].rearr[0]
             && button[1][1] == rearrange[i].rearr[1]
             && button[1][2] == rearrange[i].rearr[2]
            )
            {   // assert(button[1][3] == rearrange[i].rearr[3]);
                DISCARD CheckRadioButton
                (   hwnd,
                    IDC_2NDRA1234,
                    IDC_2NDRA4321,
                    rearrange[i].hGad[1]
                );
        }   }
        for (i = 24; i < 48; i++)
        {   if
            (   button[0][4] == rearrange[i].rearr[0]
             && button[0][5] == rearrange[i].rearr[1]
             && button[0][6] == rearrange[i].rearr[2]
            )
            {   // assert(button[0][7] == rearrange[i].rearr[3]);
                DISCARD CheckRadioButton
                (   hwnd,
                    IDC_1STRA5678,
                    IDC_1STRA8765,
                    rearrange[i].hGad[0]
                );
            }
            if
            (   button[1][4] == rearrange[i].rearr[0]
             && button[1][5] == rearrange[i].rearr[1]
             && button[1][6] == rearrange[i].rearr[2]
            )
            {   // assert(button[1][7] == rearrange[i].rearr[3]);
                DISCARD CheckRadioButton
                (   hwnd,
                    IDC_2NDRA5678,
                    IDC_2NDRA8765,
                    rearrange[i].hGad[1]
                );
        }   }

        setdlgtext(hwnd, IDC_RESETREARRANGE, MSG_RESETTODEFAULTS2, "&Reset to defaults");
        setdlgtext(hwnd, IDOK              , MSG_OK2             , "&OK");
        setdlgtext(hwnd, IDCANCEL          , MSG_CANCEL2         , "&Cancel");

        ModalWindowPtr = hwnd;
        update_rearrange();

        return TRUE;
    case WM_DRAWITEM: // no need for acase
        drawitem((struct tagDRAWITEMSTRUCT*) lParam, FALSE);
        return TRUE;
    case WM_CLOSE:
        reopen_subwindow(SUBWINDOW_HOSTPADS);
        clearkybd();
        DISCARD EndDialog(hwnd, 0);
        return TRUE;
    case WM_DESTROY:
        ModalWindowPtr = NULL;
        return FALSE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_RESETREARRANGE:
            for (i = 0; i < 2; i++)
            {   for (j = 0; j < 8; j++)
                {   tempbutton[i][j] = j + 1;
            }   }
            update_rearrange();
        acase IDOK:
            for (i = 0; i < 2; i++)
            {   for (j = 0; j < 8; j++)
                {   button[i][j] = tempbutton[i][j];
            }   }
        //lint -fallthrough
        case IDCANCEL:
            reopen_subwindow(SUBWINDOW_HOSTPADS);
            clearkybd();
            DISCARD EndDialog(hwnd, 0);
            return TRUE;
        default: // adefault not required
            for (i = 0; i < 24; i++)
            {   if (LOWORD(wParam) == rearrange[i].hGad[0])
                {   tempbutton[0][0] = rearrange[i].rearr[0];
                    tempbutton[0][1] = rearrange[i].rearr[1];
                    tempbutton[0][2] = rearrange[i].rearr[2];
                    tempbutton[0][3] = rearrange[i].rearr[3];
                    update_rearrange();
                }
                if (LOWORD(wParam) == rearrange[i].hGad[1])
                {   tempbutton[1][0] = rearrange[i].rearr[0];
                    tempbutton[1][1] = rearrange[i].rearr[1];
                    tempbutton[1][2] = rearrange[i].rearr[2];
                    tempbutton[1][3] = rearrange[i].rearr[3];
                    update_rearrange();
            }   }
            for (i = 24; i < 48; i++)
            {   if (LOWORD(wParam) == rearrange[i].hGad[0])
                {   tempbutton[0][4] = rearrange[i].rearr[0];
                    tempbutton[0][5] = rearrange[i].rearr[1];
                    tempbutton[0][6] = rearrange[i].rearr[2];
                    tempbutton[0][7] = rearrange[i].rearr[3];
                    update_rearrange();
                }
                if (LOWORD(wParam) == rearrange[i].hGad[1])
                {   tempbutton[1][4] = rearrange[i].rearr[0];
                    tempbutton[1][5] = rearrange[i].rearr[1];
                    tempbutton[1][6] = rearrange[i].rearr[2];
                    tempbutton[1][7] = rearrange[i].rearr[3];
                    update_rearrange();
        }   }   }
        return FALSE;
    default: // no need for adefault
        return FALSE;
}   }

EXPORT BOOL CALLBACK RedefineDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT int   i, j,
                    mousex, mousey;
    TRANSIENT LONG  gid;
    PERSIST   int   redef_x   = -1,
                    redef_y   = -1;
    PERSIST   POINT thepoint;
    PERSIST   RECT  localrect,
                    redefinerect[SOMEREDEFINEGADS];
    PERSIST   HICON localhicon;

    switch (Message)
    {
    case WM_INITDIALOG:
        RedefineWindowPtr = hwnd;

        DISCARD SetWindowText(hwnd, LLL(MSG_HAIL_REDEFINEKEYS, "Redefine Keys"));

        setfont(hwnd, IDC_KEY_START);
        setfont(hwnd, IDC_KEY_A);
        setfont(hwnd, IDC_KEY_B);
        setfont(hwnd, IDC_KEY_RESET);
        setfont(hwnd, IDC_F1);
        setfont(hwnd, IDC_F2);
        setfont(hwnd, IDC_F3);
        setfont(hwnd, IDC_F4);
        for (i = 0; i < ALLREDEFINEGADS; i++)
        {   if (ctrlgads[i].redef[showpalladiumkeys2 ? 1 : 0] != -1)
            {   setfont(hwnd, ctrlgads[i].redef[showpalladiumkeys2 ? 1 : 0]);
        }   }

        setdlgtext(hwnd, IDC_RESETREDEFINE,      MSG_RESETTODEFAULTS2,  "&Reset to defaults");
        setdlgtext(hwnd, IDOK,                   MSG_OK2,               "&OK");
        setdlgtext(hwnd, IDCANCEL,               MSG_CANCEL2,           "&Cancel");
        setdlgtext(hwnd, IDC_LABEL_LEFT,         MSG_LEFTCONTROLLER,    "Left Controller");
        setdlgtext(hwnd, IDC_LABEL_RIGHT,        MSG_RIGHTCONTROLLER,   "Right Controller");
        setdlgtext(hwnd, IDL_CONSOLE,            MSG_KYBDLEGEND_3,      "Console");
        sprintf(gtempstring, "%s:", machines[machine].consolekeyname[0]);
        SetDlgItemText(hwnd, IDC_F1,             gtempstring);
        sprintf(gtempstring, "%s:", machines[machine].consolekeyname[1]);
        SetDlgItemText(hwnd, IDC_F2,             gtempstring);
        sprintf(gtempstring, "%s:", machines[machine].consolekeyname[2]);
        SetDlgItemText(hwnd, IDC_F3,             gtempstring);
        sprintf(gtempstring, "%s:", machines[machine].consolekeyname[3]);
        SetDlgItemText(hwnd, IDC_F4,             gtempstring);
        setdlgtext(hwnd, IDC_SHOWPALLADIUMKEYS2, MSG_SHOWPALLADIUMKEYS, "Show Palladium keys?");

        localhicon = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon), IMAGE_ICON, 32, 32, 0);
        DISCARD SendMessage(GetDlgItem(hwnd, IDL_REDEFINEGLYPH), STM_SETICON, (WPARAM) localhicon, (LPARAM) 0);

        if (!preserve)
        {   for (i = 0; i < 2; i++)
            {   for (j = 0; j < NUMKEYS; j++)
                {   temp_keypads[i][j] = keypads[i][j];
            }   }
            for (i = 0; i < 4; i++)
            {   temp_console[i] = console[i];
        }   }

        enablegads();
        refreshkeys();

        for (i = 0; i < SOMEREDEFINEGADS; i++)
        {   GetWindowRect(GetDlgItem(hwnd, IDC_P1_1ST + i), &localrect);
            thepoint.x             = localrect.left;
            thepoint.y             = localrect.top;
            DISCARD ScreenToClient(hwnd, &thepoint);
            redefinerect[i].left   = thepoint.x;
            redefinerect[i].top    = thepoint.y;
            thepoint.x             = localrect.right;
            thepoint.y             = localrect.bottom;
            DISCARD ScreenToClient(hwnd, &thepoint);
            redefinerect[i].right  = thepoint.x;
            redefinerect[i].bottom = thepoint.y;
        }
        // If they move the subwindow later, it doesn't matter.

        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_SHOWPALLADIUMKEYS2),
            BM_SETCHECK,
            showpalladiumkeys2 ? BST_CHECKED : BST_UNCHECKED,
            0
        );

        if (redef_x != -1)
        {   // assert(redef_y != -1);
            DISCARD SetWindowPos
            (   hwnd,
                HWND_TOP,
                redef_x,
                redef_y,
                0,
                0,
                SWP_NOCOPYBITS | SWP_NOSIZE
            );
        }

        return TRUE;
    case WM_DRAWITEM: // no need for acase
        drawitem((struct tagDRAWITEMSTRUCT*) lParam, FALSE);
        return TRUE;
    case WM_LBUTTONDBLCLK: // no need for acase
    case WM_LBUTTONDOWN:
        mousex = (int) LOWORD(lParam); // pixels -> dialog units
        mousey = (int) HIWORD(lParam); // pixels -> dialog units

#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "X: %d, Y: %d.\n", mousex, mousey);
#endif

        j = SOMEREDEFINEGADS;
        for (i = 0; i < SOMEREDEFINEGADS; i++)
        {   if
            (   mousex >= redefinerect[i].left
             && mousex <= redefinerect[i].right
             && mousey >= redefinerect[i].top
             && mousey <= redefinerect[i].bottom
            )
            {   j = i;
                break;
        }   }

        switch (j)
        {
        case 0:
            if
            (   machine == ARCADIA
             || machines[machine].pvis
             || machine == PONG
            ) // excludes INSTRUCTOR, PHUNSY, SELBST, MIKIT, TYPERIGHT
            {   side = 0; whichkey =  0; // IDC_P1_1ST
                subreq();
            }
        acase 1:
            if
            (   machine == ARCADIA
             || machine == INTERTON
             || machine == ELEKTOR
             || memmap == MEMMAP_LASERBATTLE
             || memmap == MEMMAP_LAZARIAN
            ) // excludes INSTRUCTOR, Astro Wars, Galaxia, MALZAK, PONG, PHUNSY, SELBST, MIKIT, TYPERIGHT
            {   side = 0; whichkey = 21; // IDC_P1_2ND
                subreq();
            }
        acase 2:
            if
            (   machine == ARCADIA
             || machine == INTERTON
             || machine == ELEKTOR
             || memmap == MEMMAP_LASERBATTLE
             || memmap == MEMMAP_LAZARIAN
            ) // excludes INSTRUCTOR, Astro Wars, Galaxia, MALZAK, PONG, PHUNSY, SELBST, MIKIT, TYPERIGHT
            {   side = 0; whichkey = 22; // IDC_P1_3RD
                subreq();
            }
        acase 3:
            if
            (   machine == ARCADIA
             || machine == INTERTON
             || machine == ELEKTOR
             || memmap == MEMMAP_LASERBATTLE
             || memmap == MEMMAP_LAZARIAN
            ) // excludes INSTRUCTOR, Astro Wars, Galaxia, MALZAK, PONG, PHUNSY, SELBST, MIKIT, TYPERIGHT
            {   side = 0; whichkey = 23; // IDC_P1_4TH
                subreq();
            }
        acase 4:
            if
            (   machine == ARCADIA
             || machine == INTERTON
             || machine == ELEKTOR
             || memmap  == MEMMAP_ASTROWARS
             || memmap  == MEMMAP_GALAXIA
             || machine == MALZAK
             || machine == PONG
            ) // excludes INSTRUCTOR, Laser Battle, PHUNSY, SELBST, MIKIT, TYPERIGHT
            {   side = 1; whichkey =  0; // IDC_P2_1ST
                subreq();
            }
        acase 5:
            if
            (   machine == ARCADIA
             || machine == INTERTON
             || machine == ELEKTOR
            ) // excludes INSTRUCTOR, coin-ops, PONG, PHUNSY, SELBST, MIKIT, TYPERIGHT
            {   side = 1; whichkey = 21; // IDC_P2_2ND
                subreq();
            }
        acase 6:
            if
            (   machine == ARCADIA
             || machine == INTERTON
             || machine == ELEKTOR
            ) // excludes INSTRUCTOR, coin-ops, PONG, PHUNSY, SELBST, MIKIT, TYPERIGHT
            {   side = 1; whichkey = 22; // IDC_P2_3RD
                subreq();
            }
        acase 7:
            if
            (   machine == ARCADIA
             || machine == INTERTON
             || machine == ELEKTOR
            ) // excludes INSTRUCTOR, coin-ops, PONG, PHUNSY, SELBST, MIKIT, TYPERIGHT
            {   side = 1; whichkey = 23; // IDC_P2_4TH
                subreq();
        }   }
    acase WM_CTLCOLORSTATIC:
        gid = GetWindowLong((HWND) lParam, GWL_ID);

        if (    gid == IDC_P1_1ST || gid == IDC_P2_1ST)
        {   SetBkColor((HDC) wParam, EMUPEN_RED);
            return (LRESULT) hBrush[EMUBRUSH_RED];
        } elif (gid == IDC_P1_2ND || gid == IDC_P2_2ND)
        {   SetBkColor((HDC) wParam, EMUPEN_BLUE);
            return (LRESULT) hBrush[EMUBRUSH_BLUE];
        } elif (gid == IDC_P1_3RD || gid == IDC_P2_3RD)
        {   SetBkColor((HDC) wParam, EMUPEN_GREEN);
            return (LRESULT) hBrush[EMUBRUSH_GREEN];
        } elif (gid == IDC_P1_4TH || gid == IDC_P2_4TH)
        {   SetBkColor((HDC) wParam, EMUPEN_YELLOW);
            return (LRESULT) hBrush[EMUBRUSH_YELLOW];
        } else return TRUE;
    case WM_CLOSE: // no need for acase
        for (i = 0; i < 2; i++)
        {   for (j = 0; j < NUMKEYS; j++)
            {   keypads[i][j] = temp_keypads[i][j];
        }   }
        for (i = 0; i < 4; i++)
        {   console[i] = temp_console[i];
        }

        GetWindowRect(RedefineWindowPtr, &localrect); // window relative to screen
        redef_x = localrect.left;
        redef_y = localrect.top;

        clearkybd();
        DISCARD EndDialog(hwnd, 0);
        return TRUE;
    case WM_DESTROY: // no need for acase
        RedefineWindowPtr = NULL;

        close_subwindows(FALSE);
        reopen_subwindows(); // to refresh contents of eg. game info and host keyboard subwindows

        return TRUE;
    case WM_COMMAND: // no need for acase
        switch (LOWORD(wParam))
        {
        case IDOK:
            for (i = 0; i < 2; i++)
            {   for (j = 0; j < NUMKEYS; j++)
                {   keypads[i][j] = temp_keypads[i][j];
            }   }
            for (i = 0; i < 4; i++)
            {   console[i] = temp_console[i];
            }
        case IDCANCEL: //lint -fallthrough
            clearkybd();
            DISCARD EndDialog(hwnd, 0);
            return TRUE;
        case IDC_RESETREDEFINE: // no need for acase
            for (i = 0; i < 2; i++)
            {   for (j = 0; j < NUMKEYS; j++)
                {   temp_keypads[i][j] = default_keypads[i][j];
            }   }
            for (i = 0; i < 4; i++)
            {   temp_console[i] = default_console[i];
            }
            refreshkeys();
        acase IDC_P1_1:
            side = 0; whichkey = 1;
            subreq();
        acase IDC_P1_2:
            side = 0; whichkey = 2;
            subreq();
        acase IDC_P1_3:
            side = 0; whichkey = 3;
            subreq();
        acase IDC_P1_4:
            side = 0; whichkey = 4;
            subreq();
        acase IDC_P1_5:
            side = 0; whichkey = 5;
            subreq();
        acase IDC_P1_6:
            side = 0; whichkey = 6;
            subreq();
        acase IDC_P1_7:
            side = 0; whichkey = 7;
            subreq();
        acase IDC_P1_8:
            side = 0; whichkey = 8;
            subreq();
        acase IDC_P1_9:
            side = 0; whichkey = 9;
            subreq();
        acase IDC_P1_C:
            side = 0; whichkey = 10;
            subreq();
        acase IDC_P1_0:
            side = 0; whichkey = 11;
            subreq();
        acase IDC_P1_E:
            side = 0; whichkey = 12;
            subreq();
        acase IDC_P1_X1:
            side = 0; whichkey = 13;
            subreq();
        acase IDC_P1_X2:
            side = 0; whichkey = 14;
            subreq();
        acase IDC_P1_X3:
            side = 0; whichkey = 15;
            subreq();
        acase IDC_P1_X4:
            side = 0; whichkey = 16;
            subreq();
        acase IDC_P1_UPLEFT:
            side = 0; whichkey = 24;
            subreq();
        acase IDC_P1_UP:
            side = 0; whichkey = 17;
            subreq();
        acase IDC_P1_UPRIGHT:
            side = 0; whichkey = 25;
            subreq();
        acase IDC_P1_LEFT:
            side = 0; whichkey = 19;
            subreq();
        acase IDC_P1_RIGHT:
            side = 0; whichkey = 20;
            subreq();
        acase IDC_P1_DOWNLEFT:
            side = 0; whichkey = 26;
            subreq();
        acase IDC_P1_DOWN:
            side = 0; whichkey = 18;
            subreq();
        acase IDC_P1_DOWNRIGHT:
            side = 0; whichkey = 27;
            subreq();
        acase IDC_P2_1:
            side = 1; whichkey = 1;
            subreq();
        acase IDC_P2_2:
            side = 1; whichkey = 2;
            subreq();
        acase IDC_P2_3:
            side = 1; whichkey = 3;
            subreq();
        acase IDC_P2_4:
            side = 1; whichkey = 4;
            subreq();
        acase IDC_P2_5:
            side = 1; whichkey = 5;
            subreq();
        acase IDC_P2_6:
            side = 1; whichkey = 6;
            subreq();
        acase IDC_P2_7:
            side = 1; whichkey = 7;
            subreq();
        acase IDC_P2_8:
            side = 1; whichkey = 8;
            subreq();
        acase IDC_P2_9:
            side = 1; whichkey = 9;
            subreq();
        acase IDC_P2_C:
            side = 1; whichkey = 10;
            subreq();
        acase IDC_P2_0:
            side = 1; whichkey = 11;
            subreq();
        acase IDC_P2_E:
            side = 1; whichkey = 12;
            subreq();
        acase IDC_P2_X1:
            side = 1; whichkey = 13;
            subreq();
        acase IDC_P2_X2:
            side = 1; whichkey = 14;
            subreq();
        acase IDC_P2_X3:
            side = 1; whichkey = 15;
            subreq();
        acase IDC_P2_X4:
            side = 1; whichkey = 16;
            subreq();
        acase IDC_P2_UPLEFT:
            side = 1; whichkey = 24;
            subreq();
        acase IDC_P2_UP:
            side = 1; whichkey = 17;
            subreq();
        acase IDC_P2_UPRIGHT:
            side = 1; whichkey = 25;
            subreq();
        acase IDC_P2_LEFT:
            side = 1; whichkey = 19;
            subreq();
        acase IDC_P2_RIGHT:
            side = 1; whichkey = 20;
            subreq();
        acase IDC_P2_DOWNLEFT:
            side = 1; whichkey = 26;
            subreq();
        acase IDC_P2_DOWN:
            side = 1; whichkey = 18;
            subreq();
        acase IDC_P2_DOWNRIGHT:
            side = 1; whichkey = 27;
            subreq();
        acase IDC_KEY_START:
            side = 2; whichkey = 0;
            subreq();
        acase IDC_KEY_A:
            side = 2; whichkey = 1;
            subreq();
        acase IDC_KEY_B:
            side = 2; whichkey = 2;
            subreq();
        acase IDC_KEY_RESET:
            side = 2; whichkey = 3;
            subreq();
        acase IDC_SHOWPALLADIUMKEYS2:
            if (SendMessage(GetDlgItem(hwnd, IDC_SHOWPALLADIUMKEYS2), BM_GETCHECK, 0, 0) == BST_CHECKED)
            {   showpalladiumkeys2 = TRUE;
            } else
            {   showpalladiumkeys2 = FALSE;
            }

            GetWindowRect(RedefineWindowPtr, &localrect); // window relative to screen
            redef_x = localrect.left;
            redef_y = localrect.top;

            clearkybd();
            DISCARD EndDialog(hwnd, 0);
            preserve = TRUE;
            if (showpalladiumkeys2)
            {   DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_REDEFINE_LARGE), MainWindowPtr, RedefineDlgProc);
            } else
            {   DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_REDEFINE_SMALL), MainWindowPtr, RedefineDlgProc);
            }
        adefault:
            return FALSE;
        }
        return TRUE;
    }

    return FALSE;
}

MODULE BOOL CALLBACK RedefineSubDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   ULONG code;

    switch (Message)
    {
    case WM_INITDIALOG:
        DISCARD SetWindowText(hwnd, LLL(MSG_HAIL_REDEFINEKEY, "Redefine key..."));
        clearkybd();
    return FALSE;
    case WM_KEYUP: // WM_KEYDOWN causes problems with cursor down and with cursor right
    // no need for acase
        code = (lParam & 0x01FF0000) >> 16; // 0..511
        if (code == 1) // Esc
        {   clearkybd();
            DISCARD EndDialog(hwnd, 0);
            return TRUE;
        } elif (!keyname[code].reserved)
        {   if (side == 2)
            {   temp_console[      whichkey] = (UWORD) code;
            } else
            {   temp_keypads[side][whichkey] = (UWORD) code;
            }
            clearkybd();
            DISCARD EndDialog(hwnd, 0);
            return TRUE;
        }
    return FALSE;
    default: // no need for adefault
    return FALSE;
}   }

MODULE BOOL CALLBACK RegisterDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   int i,
        tempreg;

    switch (Message)
    {
    case WM_INITDIALOG:
        DISCARD SetWindowText(hwnd, LLL(MSG_HAIL_REGISTERFILETYPES, "Register Filetypes"));

        setdlgtext(hwnd, IDC_REASSOCIATE   , MSG_REASSOCIATE, "Reassociate these automatically at startup?");
        setdlgtext(hwnd, IDC_REGISTERALL   , MSG_ALL        , "All");
        setdlgtext(hwnd, IDC_REGISTERINVERT, MSG_INVERT     , "Invert");
        setdlgtext(hwnd, IDC_REGISTERNONE  , MSG_NONE       , "None");
        setdlgtext(hwnd, IDOK              , MSG_OK2        , "&OK");
        setdlgtext(hwnd, IDCANCEL          , MSG_CANCEL2    , "&Cancel");

        for (i = 0; i < ASSOCIATIONS; i++)
        {   if (assoc[i].flag)
            {   DISCARD SendMessage(GetDlgItem(hwnd, assoc[i].gid), BM_SETCHECK, BST_CHECKED, 0);
        }   }
        if (reassociate) DISCARD SendMessage(GetDlgItem(hwnd, IDC_REASSOCIATE), BM_SETCHECK, BST_CHECKED, 0);

        return TRUE;
    case WM_DRAWITEM: // no need for acase
        drawitem((struct tagDRAWITEMSTRUCT*) lParam, FALSE);
        return TRUE;
    case WM_CLOSE: // no need for acase
        goto DONE;
    case WM_COMMAND: // no need for acase
        switch (LOWORD(wParam))
        {
        case IDOK:
        goto DONE;
        case IDCANCEL: // no need for acase
            clearkybd();
            DISCARD EndDialog(hwnd, 0);
        return TRUE;
        case IDC_REGISTERALL: // no need for acase
            for (i = 0; i < ASSOCIATIONS; i++)
            {   SendMessage(GetDlgItem(hwnd, assoc[i].gid), BM_SETCHECK, BST_CHECKED, 0);
            }
        acase IDC_REGISTERINVERT:
            for (i = 0; i < ASSOCIATIONS; i++)
            {   tempreg = SendMessage(GetDlgItem(hwnd, assoc[i].gid), BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
                SendMessage(GetDlgItem(hwnd, assoc[i].gid), BM_SETCHECK, tempreg ? BST_UNCHECKED : BST_CHECKED, 0);
            }
        acase IDC_REGISTERNONE:
            for (i = 0; i < ASSOCIATIONS; i++)
            {   SendMessage(GetDlgItem(hwnd, assoc[i].gid), BM_SETCHECK, BST_UNCHECKED, 0);
        }   }
        return TRUE;
    }

    return FALSE;

DONE:
    for (i = 0; i < ASSOCIATIONS; i++)
    {   assoc[i].flag = SendMessage(GetDlgItem(hwnd, assoc[i].gid), BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
    }
    doassociations();

    if (SendMessage(GetDlgItem(hwnd, IDC_REASSOCIATE), BM_GETCHECK, 0, 0) == BST_CHECKED)
    {   reassociate = TRUE;
    } else
    {   reassociate = FALSE;
    }

    clearkybd();
    DISCARD EndDialog(hwnd, 0);

    return TRUE;
}

EXPORT BOOL CALLBACK SensitivityDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   PERSIST int defsensitivity,
                tempsensitivity[2];

    switch (Message)
    {
    case WM_INITDIALOG:
        if (whichgame == -1)
        {   defsensitivity = SENSITIVITY_DEFAULT;
        } else
        {   defsensitivity = known[whichgame].sensitivity;
        }
        tempsensitivity[0] = sensitivity[0];
        tempsensitivity[1] = sensitivity[1];

        SetWindowText(hwnd, LLL(               MSG_HAIL_SENSITIVITY, "Sensitivity"));

        setdlgtext(hwnd, IDL_LTSENSITIVITY   , MSG_KYBDLEGEND_1,     "Left player");
        setdlgtext(hwnd, IDL_LOW1            , MSG_LOW,              "Low");
        setdlgtext(hwnd, IDL_HIGH1           , MSG_HIGH,             "High");

        setdlgtext(hwnd, IDL_RTSENSITIVITY   , MSG_KYBDLEGEND_2,     "Right player");
        setdlgtext(hwnd, IDL_LOW2            , MSG_LOW,              "Low");
        setdlgtext(hwnd, IDL_HIGH2           , MSG_HIGH,             "High");

        setdlgtext(hwnd, IDC_RESETSENSITIVITY, MSG_RESETTODEFAULTS2, "&Reset to defaults");
        setdlgtext(hwnd, IDOK,                 MSG_OK2,              "&OK");
        setdlgtext(hwnd, IDCANCEL,             MSG_CANCEL2,          "&Cancel");

        SendMessage(GetDlgItem(hwnd, IDC_SENSITIVITY1), TBM_SETRANGE, FALSE, MAKELONG(SENSITIVITY_MIN, SENSITIVITY_MAX));
        SendMessage(GetDlgItem(hwnd, IDC_SENSITIVITY1), TBM_SETPOS,   TRUE,  tempsensitivity[0]);
        SendMessage(GetDlgItem(hwnd, IDC_SENSITIVITY2), TBM_SETRANGE, FALSE, MAKELONG(SENSITIVITY_MIN, SENSITIVITY_MAX));
        SendMessage(GetDlgItem(hwnd, IDC_SENSITIVITY2), TBM_SETPOS,   TRUE,  tempsensitivity[1]);

        if (tempsensitivity[0] != defsensitivity || tempsensitivity[1] != defsensitivity)
        {   EnableWindow(GetDlgItem(hwnd, IDC_RESETSENSITIVITY), TRUE);
        } else
        {   EnableWindow(GetDlgItem(hwnd, IDC_RESETSENSITIVITY), FALSE);
        }

        return TRUE;
    case WM_DRAWITEM: // no need for acase
        drawitem((struct tagDRAWITEMSTRUCT*) lParam, FALSE);
        return TRUE;
    case WM_CLOSE: // no need for acase
        sensitivity[0] = tempsensitivity[0];
        sensitivity[1] = tempsensitivity[1];

        clearkybd();
        DISCARD EndDialog(hwnd, 0);
        return TRUE;
    case WM_HSCROLL: // no need for acase
        if (lParam == (long) GetDlgItem(hwnd, IDC_SENSITIVITY1))
        {   tempsensitivity[0] = SendMessage(GetDlgItem(hwnd, IDC_SENSITIVITY1), TBM_GETPOS, 0, 0);
            if (tempsensitivity[0] != defsensitivity || tempsensitivity[1] != defsensitivity)
            {   EnableWindow(GetDlgItem(hwnd, IDC_RESETSENSITIVITY), TRUE);
            } else
            {   EnableWindow(GetDlgItem(hwnd, IDC_RESETSENSITIVITY), FALSE);
        }   }
        elif (lParam == (long) GetDlgItem(hwnd, IDC_SENSITIVITY2))
        {   tempsensitivity[1] = SendMessage(GetDlgItem(hwnd, IDC_SENSITIVITY2), TBM_GETPOS, 0, 0);
            if (tempsensitivity[0] != defsensitivity || tempsensitivity[1] != defsensitivity)
            {   EnableWindow(GetDlgItem(hwnd, IDC_RESETSENSITIVITY), TRUE);
            } else
            {   EnableWindow(GetDlgItem(hwnd, IDC_RESETSENSITIVITY), FALSE);
        }   }
        return TRUE;
    case WM_COMMAND: // no need for acase
        switch (LOWORD(wParam))
        {
        case IDC_RESETSENSITIVITY:
            tempsensitivity[0] =
            tempsensitivity[1] = defsensitivity;
            SendMessage(GetDlgItem(hwnd, IDC_SENSITIVITY1), TBM_SETPOS, TRUE, tempsensitivity[0]);
            SendMessage(GetDlgItem(hwnd, IDC_SENSITIVITY2), TBM_SETPOS, TRUE, tempsensitivity[1]);
            EnableWindow(GetDlgItem(hwnd, IDC_RESETSENSITIVITY), FALSE);
            return TRUE;
        case IDOK:
            sensitivity[0] = tempsensitivity[0];
            sensitivity[1] = tempsensitivity[1];
        // lint -fallthrough
        case IDCANCEL:
            clearkybd();
            DISCARD EndDialog(hwnd, 0);
            return TRUE;
        }
        return FALSE;
    default: // no need for adefault
        return FALSE;
}   }

EXPORT FLAG project_bpnf(void)
{   sound_off(FALSE);
    opening = modal = TRUE;
    settitle();
    DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_BPNF), MainWindowPtr, BPNFDlgProc);
    opening = modal = FALSE;
    settitle();
    sound_on(TRUE);
    return bpnf_ok;
}

EXPORT FLAG debug_edit(STRPTR what, FLAG quiet, FLAG doke)
{   if (!allowable(FALSE))
    {   return TRUE;
    }

    editstring = what;
    if (!poke_start(editstring, quiet, doke))
    {   return FALSE;
    }

    sound_off(FALSE);
    opening = modal = TRUE;
    settitle();
    DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_EDIT), MainWindowPtr, EditDlgProc);

    if (pokeok)
    {   tonum = parse_expression(tostr, tolimit, FALSE);
        poke_end(quiet, doke);
    } elif (!quiet)
    {   zprintf(TEXTPEN_CLIOUTPUT, "Cancelled.\n\n");
    }

    opening = modal = FALSE;
    settitle();
    sound_on(TRUE);
    update_monitor(FALSE);
    return TRUE;
}

MODULE BOOL CALLBACK SoundDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   TRANSIENT int  index,
                   voice;
    PERSIST   TEXT tempstring[40 + 1];

    switch (Message)
    {
    case WM_INITDIALOG:
        SetWindowText(hwnd, LLL(            MSG_HAIL_ADJUSTSOUND, "Adjust Sound"   ));

        setdlgtext(hwnd, IDL_VOLUME       , MSG_SOUND_VOLUME    , "Volume"         );
        setdlgtext(hwnd, IDL_SILENT       , MSG_SILENT          , "Silent"         );
        setdlgtext(hwnd, IDL_LOUD         , MSG_LOUD            , "Loud"           );
        setdlgtext(hwnd, IDL_BPS          , MSG_BPS             , "Bits per sample");
        setdlgtext(hwnd, IDL_SAMPLERATE   , MSG_SAMPLERATE      , "Sample rate"    );
        setdlgtext(hwnd, IDL_SPEECH       , MSG_SOUND_SPEECH    , "Speech"         );
        setdlgtext(hwnd, IDC_SPEECHENABLED, MSG_SOUND_ENABLED   , "Enabled?"       );
        setdlgtext(hwnd, IDL_VOICE        , MSG_VOICE           , "Voice:"         );
        setdlgtext(hwnd, IDL_RATE         , MSG_RATE            , "Rate:"          );

        setfont(hwnd, IDC_11025HZ);
        sprintf(tempstring, "11025 %s", LLL(MSG_HERTZ, "Hz"));
        DISCARD SetDlgItemText(hwnd, IDC_11025HZ, tempstring);
        setfont(hwnd, IDC_22050HZ);
        sprintf(tempstring, "22050 %s", LLL(MSG_HERTZ, "Hz"));
        DISCARD SetDlgItemText(hwnd, IDC_22050HZ, tempstring);
        setfont(hwnd, IDC_44100HZ);
        sprintf(tempstring, "44100 %s", LLL(MSG_HERTZ, "Hz"));
        DISCARD SetDlgItemText(hwnd, IDC_44100HZ, tempstring);

        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_VOLUME),
            TBM_SETRANGE,
            FALSE,
            MAKELONG(HOSTVOLUME_MIN, HOSTVOLUME_MAX)
        );
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_RATE),
            TBM_SETRANGE,
            FALSE,
            MAKELONG(0, 20)
        );

        newbitrate    = bitrate;
        newsamplerate = samplerate;
        newvolume     = hostvolume;
        newusespeech  = usespeech;
        newspeechrate = speech_rate;

        if (newbitrate == 8) DISCARD CheckRadioButton(hwnd, IDC_8BIT, IDC_16BIT, IDC_8BIT);
        else                 DISCARD CheckRadioButton(hwnd, IDC_8BIT, IDC_16BIT, IDC_16BIT);

        switch (newsamplerate)
        {
        case  11025: DISCARD CheckRadioButton(hwnd, IDC_11025HZ, IDC_44100HZ, IDC_11025HZ);
        acase 22050: DISCARD CheckRadioButton(hwnd, IDC_11025HZ, IDC_44100HZ, IDC_22050HZ);
        acase 44100: DISCARD CheckRadioButton(hwnd, IDC_11025HZ, IDC_44100HZ, IDC_44100HZ);
        }
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_VOLUME),
            TBM_SETPOS,
            TRUE,
            newvolume
        );
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_SPEECHENABLED),
            BM_SETCHECK,
            newusespeech ? BST_CHECKED : BST_UNCHECKED,
            0
        );
        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_RATE),
            TBM_SETPOS,
            TRUE,
            newspeechrate
        );

        voice = list_voices(hwnd);
        if (voice == -1)
        {   voice = 0; // we should really get the name of the default or current voice
        }
        DISCARD SendMessage(GetDlgItem(hwnd, IDC_VOICE), LB_SETCURSEL, (WPARAM) voice, 0); // ordinal number in list

        if (recmode != RECMODE_NORMAL)
        {   EnableWindow(GetDlgItem(hwnd, IDC_8BIT   ), FALSE);
            EnableWindow(GetDlgItem(hwnd, IDC_16BIT  ), FALSE);
            EnableWindow(GetDlgItem(hwnd, IDC_11025HZ), FALSE);
            EnableWindow(GetDlgItem(hwnd, IDC_22050HZ), FALSE);
            EnableWindow(GetDlgItem(hwnd, IDC_44100HZ), FALSE);
        }
        ghost_soundgads(hwnd);

        return TRUE;
    case WM_DRAWITEM: // no need for acase
        drawitem((struct tagDRAWITEMSTRUCT*) lParam, FALSE);
        return TRUE;
    case WM_CLOSE: // no need for acase
        usespeech   = newusespeech;
        speech_rate = newspeechrate;
        bitrate     = newbitrate;
        samplerate  = newsamplerate;
        hostvolume  = newvolume;

        clearkybd();
        DISCARD EndDialog(hwnd, 0);

        return TRUE;
    case WM_HSCROLL: // no need for acase
        if (lParam == (long) GetDlgItem(hwnd, IDC_RATE))
        {   newspeechrate = SendMessage
            (   GetDlgItem(hwnd, IDC_RATE),
                TBM_GETPOS,
                0,
                0
            );
            ghost_soundgads(hwnd);
        } elif (lParam == (long) GetDlgItem(hwnd, IDC_VOLUME))
        {   newvolume = SendMessage
            (   GetDlgItem(hwnd, IDC_VOLUME),
                TBM_GETPOS,
                0,
                0
            );
            ghost_soundgads(hwnd);
        }
        return TRUE;
    case WM_COMMAND: // no need for acase
        switch (LOWORD(wParam))
        {
        case IDC_SPEECHENABLED:
            if (SendMessage(GetDlgItem(hwnd, IDC_SPEECHENABLED), BM_GETCHECK, 0, 0) == BST_CHECKED)
            {   newusespeech = TRUE;
            } else
            {   newusespeech = FALSE;
            }
            ghost_soundgads(hwnd);
        acase IDC_VOICE:
            if (HIWORD(wParam) == LBN_SELCHANGE)
            {   index = SendMessage(GetDlgItem(hwnd, IDC_VOICE), LB_GETCURSEL, 0, 0);
                if (index != LB_ERR)
                {   SendMessageW(GetDlgItem(hwnd, IDC_VOICE), LB_GETTEXT, index, (LPARAM) voicename);
            }   }
        acase IDC_RESETSOUND:
            newbitrate    = 8;
            newsamplerate = 11025;
            newusespeech  = FALSE;
            newspeechrate = 10;
            newvolume     = HOSTVOLUME_DEFAULT;

            ghost_soundgads(hwnd);

            CheckRadioButton(hwnd, IDC_8BIT, IDC_16BIT, (newbitrate == 8) ? (IDC_8BIT) : (IDC_16BIT));
            switch (newsamplerate)
            {
            case  11025: DISCARD CheckRadioButton(hwnd, IDC_11025HZ, IDC_44100HZ, IDC_11025HZ);
            acase 22050: DISCARD CheckRadioButton(hwnd, IDC_11025HZ, IDC_44100HZ, IDC_22050HZ);
            acase 44100: DISCARD CheckRadioButton(hwnd, IDC_11025HZ, IDC_44100HZ, IDC_44100HZ);
            }
            DISCARD SendMessage
            (   GetDlgItem(hwnd, IDC_VOLUME),
                TBM_SETPOS,
                TRUE,
                newvolume
            );
            DISCARD SendMessage
            (   GetDlgItem(hwnd, IDC_SPEECHENABLED),
                BM_SETCHECK,
                newusespeech ? BST_CHECKED : BST_UNCHECKED,
                0
            );
            DISCARD SendMessage
            (   GetDlgItem(hwnd, IDC_RATE),
                TBM_SETPOS,
                TRUE,
                newspeechrate
            );
        acase IDC_8BIT:    newbitrate    =     8; ghost_soundgads(hwnd);
        acase IDC_16BIT:   newbitrate    =    16; ghost_soundgads(hwnd);
        acase IDC_11025HZ: newsamplerate = 11025; ghost_soundgads(hwnd);
        acase IDC_22050HZ: newsamplerate = 22050; ghost_soundgads(hwnd);
        acase IDC_44100HZ: newsamplerate = 44100; ghost_soundgads(hwnd);
        acase IDOK: // eg. ENTER key
            hostvolume  = newvolume;
            bitrate     = newbitrate;
            samplerate  = newsamplerate;
            speech_rate = newspeechrate;
            usespeech   = newusespeech;

            clearkybd();
            DISCARD EndDialog(hwnd, 0);
        acase IDCANCEL: // eg. Esc key
            clearkybd();
            DISCARD EndDialog(hwnd, 0);
            return TRUE;
        } // implied else
        return FALSE;
    default: // no need for adefault
        return FALSE;
}   }

MODULE void ghost_soundgads(HWND hwnd)
{   if
    (   newbitrate    == 8
     && newsamplerate == 11025
     && newusespeech  == FALSE
     && newspeechrate == 10
     && newvolume     == HOSTVOLUME_DEFAULT
    )
    {   EnableWindow(GetDlgItem(hwnd, IDC_RESETSOUND), FALSE);
    } else
    {   EnableWindow(GetDlgItem(hwnd, IDC_RESETSOUND), TRUE);
    }
    EnableWindow(GetDlgItem(hwnd, IDC_SPEECHENABLED), speakable ? TRUE : FALSE);
    EnableWindow(GetDlgItem(hwnd, IDC_RATE), (speakable && newusespeech) ? TRUE : FALSE);
}

EXPORT void adjust_sound(void)
{   int guestchan;

    sound_off(FALSE);
    opening = modal = TRUE;
    settitle();
    DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_ADJUSTSOUND), MainWindowPtr, SoundDlgProc);
    opening = modal = FALSE;
    settitle();
    for (guestchan = 0; guestchan < GUESTCHANNELS; guestchan++)
    {   sound_close(guestchan);
    }
    open_channels(GUESTCHANNELS);
    sound_on(TRUE);
    speech_setrate((int) (speech_rate - 10));
}

EXPORT FLAG confirm(void)
{   if (!confirmable)
    {   return TRUE;
    } // implied else

    sound_off(FALSE);
    opening = modal = TRUE;
    settitle();
    DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_CONFIRM), MainWindowPtr, ConfirmDlgProc);
    opening = modal = FALSE;
    settitle();
    sound_on(TRUE);

    return confirmed;
}

MODULE BOOL CALLBACK ConfirmDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   HICON localhicon;

    switch (Message)
    {
    case WM_INITDIALOG:
        SetWindowText(hwnd, hostmachines[machine].titlebartext);
        setdlgtext(hwnd, IDL_CONFIRM,     MSG_CONFIRM,           "Really quit?");
        setdlgtext(hwnd, IDC_CONFIRM,     MSG_SETTINGS_CONFIRM3, "&Confirm on quit?");
        setdlgtext(hwnd, IDC_CONFIRM_YES, MSG_ENGINE_YES,        "&Yes");
        setdlgtext(hwnd, IDC_CONFIRM_NO,  MSG_ENGINE_NO,         "&No");

        // assert(confirmable);
        SendMessage(GetDlgItem(hwnd, IDC_CONFIRM), BM_SETCHECK, BST_CHECKED, 0);

        localhicon = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon), IMAGE_ICON, 32, 32, 0);
        DISCARD SendMessage(GetDlgItem(hwnd, IDL_GLYPH), STM_SETICON, (WPARAM) localhicon, (LPARAM) 0);

        return TRUE;
    case WM_DRAWITEM: // no need for acase
        drawitem((struct tagDRAWITEMSTRUCT*) lParam, FALSE);
        return TRUE;
    case WM_CLOSE: // no need for acase
        confirmed = TRUE;
        clearkybd();
        DISCARD EndDialog(hwnd, 0);
        return TRUE;
    case WM_COMMAND: // no need for acase
        switch (LOWORD(wParam))
        {
        case IDC_CONFIRM:
            if (SendMessage(GetDlgItem(hwnd, IDC_CONFIRM), BM_GETCHECK, 0, 0) == BST_CHECKED)
            {   confirmable = TRUE;
            } else
            {   confirmable = FALSE;
            }
        acase IDC_CONFIRM_YES:
        case IDOK:
            confirmed = TRUE;
            clearkybd();
            DISCARD EndDialog(hwnd, 0);
        return TRUE;
        case IDC_CONFIRM_NO: // no need for acase
        case IDCANCEL:
            confirmed = FALSE;
            clearkybd();
            DISCARD EndDialog(hwnd, 0);
        return TRUE;
    }   }

    return FALSE;
}

MODULE void refreshkeys(void)
{   FLAG ok;
    int  i,
         j;

    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_KEY_START  , keyname[scan_to_scan(temp_console[0])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_KEY_A      , keyname[scan_to_scan(temp_console[1])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_KEY_B      , keyname[scan_to_scan(temp_console[2])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_KEY_RESET  , keyname[scan_to_scan(temp_console[3])].name);

    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_2       , keyname[scan_to_scan(temp_keypads[0][ 2])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_1       , keyname[scan_to_scan(temp_keypads[0][ 1])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_3       , keyname[scan_to_scan(temp_keypads[0][ 3])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_4       , keyname[scan_to_scan(temp_keypads[0][ 4])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_5       , keyname[scan_to_scan(temp_keypads[0][ 5])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_6       , keyname[scan_to_scan(temp_keypads[0][ 6])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_7       , keyname[scan_to_scan(temp_keypads[0][ 7])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_8       , keyname[scan_to_scan(temp_keypads[0][ 8])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_9       , keyname[scan_to_scan(temp_keypads[0][ 9])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_C       , keyname[scan_to_scan(temp_keypads[0][10])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_0       , keyname[scan_to_scan(temp_keypads[0][11])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_E       , keyname[scan_to_scan(temp_keypads[0][12])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_UPLEFT  , keyname[scan_to_scan(temp_keypads[0][24])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_UP      , keyname[scan_to_scan(temp_keypads[0][17])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_UPRIGHT , keyname[scan_to_scan(temp_keypads[0][25])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_LEFT    , keyname[scan_to_scan(temp_keypads[0][19])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_RIGHT   , keyname[scan_to_scan(temp_keypads[0][20])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_DOWNLEFT, keyname[scan_to_scan(temp_keypads[0][26])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_DOWN    , keyname[scan_to_scan(temp_keypads[0][18])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_DOWNRIGHT,keyname[scan_to_scan(temp_keypads[0][27])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_1ST     , keyname[scan_to_scan(temp_keypads[0][ 0])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_2ND     , keyname[scan_to_scan(temp_keypads[0][21])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_3RD     , keyname[scan_to_scan(temp_keypads[0][22])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_4TH     , keyname[scan_to_scan(temp_keypads[0][23])].name);

    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_2       , keyname[scan_to_scan(temp_keypads[1][ 2])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_1       , keyname[scan_to_scan(temp_keypads[1][ 1])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_3       , keyname[scan_to_scan(temp_keypads[1][ 3])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_4       , keyname[scan_to_scan(temp_keypads[1][ 4])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_5       , keyname[scan_to_scan(temp_keypads[1][ 5])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_6       , keyname[scan_to_scan(temp_keypads[1][ 6])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_7       , keyname[scan_to_scan(temp_keypads[1][ 7])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_8       , keyname[scan_to_scan(temp_keypads[1][ 8])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_9       , keyname[scan_to_scan(temp_keypads[1][ 9])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_C       , keyname[scan_to_scan(temp_keypads[1][10])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_0       , keyname[scan_to_scan(temp_keypads[1][11])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_E       , keyname[scan_to_scan(temp_keypads[1][12])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_UPLEFT  , keyname[scan_to_scan(temp_keypads[1][24])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_UP      , keyname[scan_to_scan(temp_keypads[1][17])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_UPRIGHT , keyname[scan_to_scan(temp_keypads[1][25])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_LEFT    , keyname[scan_to_scan(temp_keypads[1][19])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_RIGHT   , keyname[scan_to_scan(temp_keypads[1][20])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_DOWNLEFT, keyname[scan_to_scan(temp_keypads[1][26])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_DOWN    , keyname[scan_to_scan(temp_keypads[1][18])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_DOWNRIGHT,keyname[scan_to_scan(temp_keypads[1][27])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_1ST     , keyname[scan_to_scan(temp_keypads[1][ 0])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_2ND     , keyname[scan_to_scan(temp_keypads[1][21])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_3RD     , keyname[scan_to_scan(temp_keypads[1][22])].name);
    DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_4TH     , keyname[scan_to_scan(temp_keypads[1][23])].name);

    if (showpalladiumkeys2)
    {   DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_X1  , keyname[scan_to_scan(temp_keypads[0][13])].name);
        DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_X2  , keyname[scan_to_scan(temp_keypads[0][14])].name);
        DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_X3  , keyname[scan_to_scan(temp_keypads[0][15])].name);
        DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P1_X4  , keyname[scan_to_scan(temp_keypads[0][16])].name);
        DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_X1  , keyname[scan_to_scan(temp_keypads[1][13])].name);
        DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_X2  , keyname[scan_to_scan(temp_keypads[1][14])].name);
        DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_X3  , keyname[scan_to_scan(temp_keypads[1][15])].name);
        DISCARD SetDlgItemText(RedefineWindowPtr, IDC_P2_X4  , keyname[scan_to_scan(temp_keypads[1][16])].name);
    }

    ok = TRUE;
    for (i = 0; i < 2; i++)
    {   for (j = 0; j < NUMKEYS; j++)
        {   if (temp_keypads[i][j] != default_keypads[i][j])
            {   ok = FALSE;
                break; // for speed
    }   }   }
    if (ok)
    {   for (i = 0; i < 4; i++)
        {   if (temp_console[i] != default_console[i])
            {   ok = FALSE;
                break; // for speed
    }   }   }

    if (ok) // all are defaults
    {   EnableWindow(GetDlgItem(RedefineWindowPtr, IDC_RESETREDEFINE), FALSE);
    } else // not all are defaults
    {   EnableWindow(GetDlgItem(RedefineWindowPtr, IDC_RESETREDEFINE), TRUE );
}   }

MODULE void subreq(void)
{   opening = TRUE; // don't worry about modal as this is a sub-subwindow
    DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_REDEFINESUB), RedefineWindowPtr, RedefineSubDlgProc);
    opening = FALSE;
    refreshkeys(); // this is overkill
}

EXPORT void enablegad(int whichgad, int enabled)
{   if (RedefineWindowPtr && ctrlgads[whichgad].redef[showpalladiumkeys2 ? 1 : 0] != -1)
    {   EnableWindow(GetDlgItem(RedefineWindowPtr, ctrlgads[whichgad].redef[showpalladiumkeys2 ? 1 : 0]), enabled);
    }
    if (SubWindowPtr[SUBWINDOW_GAMEINFO] && ctrlgads[whichgad].gameinfo[showpalladiumkeys1 ? 1 : 0] != -1)
    {   EnableWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_GAMEINFO], ctrlgads[whichgad].gameinfo[showpalladiumkeys1 ? 1 : 0]), enabled);
}   }

EXPORT void help_about(void)
{   sound_off(FALSE);
    opening = modal = TRUE;
    settitle();
    DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_ABOUT), MainWindowPtr, AboutDlgProc);
    opening = modal = FALSE;
    settitle();
    sound_on(TRUE);
}

EXPORT void emu_paths(void)
{   sound_off(FALSE);
    opening = modal = TRUE;
    settitle();
    DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_PATHS), MainWindowPtr, PathsDlgProc);
    opening = modal = FALSE;
    settitle();
    sound_on(TRUE);
}

MODULE BOOL CALLBACK PathsDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   PAINTSTRUCT localps;

    switch (Message)
    {
    case WM_INITDIALOG:
        DISCARD SetWindowText(hwnd, LLL(         , MSG_HAIL_PATHS , "Paths"));
        setdlgtext(hwnd, IDL_PATH_DISKS          , MSG_DISKS      , "Disks:");
        setdlgtext(hwnd, IDL_PATH_GAMES          , MSG_GAMES      , "Games:");
        setdlgtext(hwnd, IDL_PATH_PROJECTS       , MSG_PROJECTS   , "Projects:");
        setdlgtext(hwnd, IDL_PATH_SCREENSHOTS    , MSG_SCREENSHOTS, "Screenshots:");
        setdlgtext(hwnd, IDL_PATH_SCRIPTS        , MSG_SCRIPTS    , "Scripts:");
        setdlgtext(hwnd, IDL_PATH_TAPES          , MSG_TAPES      , "Tapes:");
        setdlgtext(hwnd, IDL_PATH_BKGRND         , MSG_BACKGROUND , "Background:");
        setdlgtext(hwnd, IDC_PATH_DISKSOPEN      , MSG_MENU_OPEN  , "Open...");
        setdlgtext(hwnd, IDC_PATH_GAMESOPEN      , MSG_MENU_OPEN  , "Open...");
        setdlgtext(hwnd, IDC_PATH_PROJECTSOPEN   , MSG_MENU_OPEN  , "Open...");
        setdlgtext(hwnd, IDC_PATH_SCREENSHOTSOPEN, MSG_MENU_OPEN  , "Open...");
        setdlgtext(hwnd, IDC_PATH_SCRIPTSOPEN    , MSG_MENU_OPEN  , "Open...");
        setdlgtext(hwnd, IDC_PATH_TAPESOPEN      , MSG_MENU_OPEN  , "Open...");
        setdlgtext(hwnd, IDC_PATH_DISKSASL       , MSG_EDIT2      , "Edit...");
        setdlgtext(hwnd, IDC_PATH_GAMESASL       , MSG_EDIT2      , "Edit...");
        setdlgtext(hwnd, IDC_PATH_PROJECTSASL    , MSG_EDIT2      , "Edit...");
        setdlgtext(hwnd, IDC_PATH_SCREENSHOTSASL , MSG_EDIT2      , "Edit...");
        setdlgtext(hwnd, IDC_PATH_SCRIPTSASL     , MSG_EDIT2      , "Edit...");
        setdlgtext(hwnd, IDC_PATH_TAPESASL       , MSG_EDIT2      , "Edit...");
        setdlgtext(hwnd, IDC_PATH_BKGRNDASL      , MSG_EDIT2      , "Edit...");

        strcpy(temppath_disks      , path_disks);
        strcpy(temppath_games      , path_games);
        strcpy(temppath_projects   , path_projects);
        strcpy(temppath_screenshots, path_screenshots);
        strcpy(temppath_scripts    , path_scripts);
        strcpy(temppath_tapes      , path_tapes);
        strcpy(tempfile_bkgrnd     , fn_bkgrnd);
        DISCARD SetDlgItemText(hwnd, IDC_PATH_DISKS      , temppath_disks);
        DISCARD SetDlgItemText(hwnd, IDC_PATH_GAMES      , temppath_games);
        DISCARD SetDlgItemText(hwnd, IDC_PATH_PROJECTS   , temppath_projects);
        DISCARD SetDlgItemText(hwnd, IDC_PATH_SCREENSHOTS, temppath_screenshots);
        DISCARD SetDlgItemText(hwnd, IDC_PATH_SCRIPTS    , temppath_scripts);
        DISCARD SetDlgItemText(hwnd, IDC_PATH_TAPES      , temppath_tapes);
        DISCARD SetDlgItemText(hwnd, IDC_PATH_BKGRND     , tempfile_bkgrnd);

        ghost_resetpaths(hwnd);

        DISCARD SetFocus(GetDlgItem(hwnd, IDC_PATH_GAMES));
        DISCARD SendMessage(GetDlgItem(hwnd, IDC_PATH_GAMES), EM_SETSEL, 0, -1);

        return FALSE; // FALSE so that our SetFocus()sing is respected
    case WM_CLOSE: // no need for acase
        GetWindowText(GetDlgItem(hwnd, IDC_PATH_DISKS      ), path_disks      , MAX_PATH);
        GetWindowText(GetDlgItem(hwnd, IDC_PATH_GAMES      ), path_games      , MAX_PATH);
        GetWindowText(GetDlgItem(hwnd, IDC_PATH_PROJECTS   ), path_projects   , MAX_PATH);
        GetWindowText(GetDlgItem(hwnd, IDC_PATH_SCREENSHOTS), path_screenshots, MAX_PATH);
        GetWindowText(GetDlgItem(hwnd, IDC_PATH_SCRIPTS    ), path_scripts    , MAX_PATH);
        GetWindowText(GetDlgItem(hwnd, IDC_PATH_TAPES      ), path_tapes      , MAX_PATH);
        GetWindowText(GetDlgItem(hwnd, IDC_PATH_BKGRND     ), fn_bkgrnd       , MAX_PATH);
        fix_paths();
        audit(FALSE);

        if (pixelubyte)
        {   free(pixelubyte);
            pixelubyte = NULL;
            pixelulong = NULL;
        }
        break_pathname(fn_bkgrnd, path_bkgrnd, file_bkgrnd);
        if (file_bkgrnd[0])
        {   DISCARD load_bmp(fn_bkgrnd);
        }
        make_stars();
        redrawscreen();

        clearkybd();
        DISCARD EndDialog(hwnd, 0);
        return TRUE;
    case WM_DRAWITEM: // no need for acase
        drawitem((struct tagDRAWITEMSTRUCT*) lParam, FALSE);
        return TRUE;
    case WM_COMMAND: // no need for acase
        switch (LOWORD(wParam))
        {
        case IDC_RESETPATHS:
            DISCARD SetDlgItemText(hwnd, IDC_PATH_DISKS      , PATH_DISKS);
            DISCARD SetDlgItemText(hwnd, IDC_PATH_GAMES      , PATH_GAMES);
            DISCARD SetDlgItemText(hwnd, IDC_PATH_PROJECTS   , PATH_PROJECTS);
            DISCARD SetDlgItemText(hwnd, IDC_PATH_SCREENSHOTS, PATH_SCREENSHOTS);
            DISCARD SetDlgItemText(hwnd, IDC_PATH_SCRIPTS    , PATH_SCRIPTS);
            DISCARD SetDlgItemText(hwnd, IDC_PATH_TAPES      , PATH_TAPES);
            DISCARD SetDlgItemText(hwnd, IDC_PATH_BKGRND     , PATH_BKGRNDS);
            EnableWindow(GetDlgItem(hwnd, IDC_RESETPATHS), FALSE);
        acase IDC_PATH_DISKSASL:
            if (zgetpath(temppath_disks))
            {   DISCARD SetDlgItemText(hwnd, IDC_PATH_DISKS      , temppath_disks);
            }
        acase IDC_PATH_GAMESASL:
            if (zgetpath(temppath_games))
            {   DISCARD SetDlgItemText(hwnd, IDC_PATH_GAMES      , temppath_games);
            }
        acase IDC_PATH_PROJECTSASL:
            if (zgetpath(temppath_projects))
            {   DISCARD SetDlgItemText(hwnd, IDC_PATH_PROJECTS   , temppath_projects);
            }
        acase IDC_PATH_SCREENSHOTSASL:
            if (zgetpath(temppath_screenshots))
            {   DISCARD SetDlgItemText(hwnd, IDC_PATH_SCREENSHOTS, temppath_screenshots);
            }
        acase IDC_PATH_SCRIPTSASL:
            if (zgetpath(temppath_scripts))
            {   DISCARD SetDlgItemText(hwnd, IDC_PATH_SCRIPTS    , temppath_scripts);
            }
        acase IDC_PATH_TAPESASL:
            if (zgetpath(temppath_tapes))
            {   DISCARD SetDlgItemText(hwnd, IDC_PATH_TAPES      , temppath_tapes);
            }
        acase IDC_PATH_BKGRNDASL:
            if (asl
            (   LLL(MSG_HAIL_BACKGROUND, "Choose Background Image"),
                "Bitmap (*.BMP)\0*.BMP\0" \
                "All Files (*.*)\0*.*\0",
                FALSE,
                (STRPTR) path_bkgrnd,    // starting directory
                tempfile_bkgrnd,         // starting filename
                (STRPTR) tempfile_bkgrnd // where to store complete result pathname
            ) == EXIT_SUCCESS)
            {   DISCARD SetDlgItemText(hwnd, IDC_PATH_BKGRND, tempfile_bkgrnd);
            }
        acase IDC_PATH_DISKSOPEN:
            DISCARD ShellExecute(MainWindowPtr, "explore", temppath_disks      , NULL, NULL, SW_SHOWNORMAL);
        acase IDC_PATH_GAMESOPEN:
            DISCARD ShellExecute(MainWindowPtr, "explore", temppath_games      , NULL, NULL, SW_SHOWNORMAL);
        acase IDC_PATH_PROJECTSOPEN:
            DISCARD ShellExecute(MainWindowPtr, "explore", temppath_projects   , NULL, NULL, SW_SHOWNORMAL);
        acase IDC_PATH_SCREENSHOTSOPEN:
            DISCARD ShellExecute(MainWindowPtr, "explore", temppath_screenshots, NULL, NULL, SW_SHOWNORMAL);
        acase IDC_PATH_SCRIPTSOPEN:
            DISCARD ShellExecute(MainWindowPtr, "explore", temppath_scripts    , NULL, NULL, SW_SHOWNORMAL);
        acase IDC_PATH_TAPESOPEN:
            DISCARD ShellExecute(MainWindowPtr, "explore", temppath_tapes      , NULL, NULL, SW_SHOWNORMAL);
        acase IDC_PATH_BKGRNDOPEN:
            DISCARD ShellExecute(MainWindowPtr, "explore", temppath_bkgrnd     , NULL, NULL, SW_SHOWNORMAL);
        acase IDC_PATH_DISKS:
            if (HIWORD(wParam) == EN_UPDATE)
            {   GetWindowText(GetDlgItem(hwnd, IDC_PATH_DISKS      ), temppath_disks      , MAX_PATH);
                ghost_resetpaths(hwnd);
            }
        acase IDC_PATH_GAMES:
            if (HIWORD(wParam) == EN_UPDATE)
            {   GetWindowText(GetDlgItem(hwnd, IDC_PATH_GAMES      ), temppath_games      , MAX_PATH);
                ghost_resetpaths(hwnd);
            }
        acase IDC_PATH_PROJECTS:
            if (HIWORD(wParam) == EN_UPDATE)
            {   GetWindowText(GetDlgItem(hwnd, IDC_PATH_PROJECTS   ), temppath_projects   , MAX_PATH);
                ghost_resetpaths(hwnd);
            }
        acase IDC_PATH_SCREENSHOTS:
            if (HIWORD(wParam) == EN_UPDATE)
            {   GetWindowText(GetDlgItem(hwnd, IDC_PATH_SCREENSHOTS), temppath_screenshots, MAX_PATH);
                ghost_resetpaths(hwnd);
            }
        acase IDC_PATH_SCRIPTS:
            if (HIWORD(wParam) == EN_UPDATE)
            {   GetWindowText(GetDlgItem(hwnd, IDC_PATH_SCRIPTS    ), temppath_scripts    , MAX_PATH);
                ghost_resetpaths(hwnd);
            }
        acase IDC_PATH_TAPES:
            if (HIWORD(wParam) == EN_UPDATE)
            {   GetWindowText(GetDlgItem(hwnd, IDC_PATH_TAPES      ), temppath_tapes      , MAX_PATH);
                ghost_resetpaths(hwnd);
            }
        acase IDC_PATH_BKGRND:
            if (HIWORD(wParam) == EN_UPDATE)
            {   GetWindowText(GetDlgItem(hwnd, IDC_PATH_BKGRND     ), tempfile_bkgrnd     , MAX_PATH);
                ghost_resetpaths(hwnd);
            }
        acase IDOK:
            GetWindowText(GetDlgItem(hwnd, IDC_PATH_DISKS      ), path_disks      , MAX_PATH);
            GetWindowText(GetDlgItem(hwnd, IDC_PATH_GAMES      ), path_games      , MAX_PATH);
            GetWindowText(GetDlgItem(hwnd, IDC_PATH_PROJECTS   ), path_projects   , MAX_PATH);
            GetWindowText(GetDlgItem(hwnd, IDC_PATH_SCREENSHOTS), path_screenshots, MAX_PATH);
            GetWindowText(GetDlgItem(hwnd, IDC_PATH_SCRIPTS    ), path_scripts    , MAX_PATH);
            GetWindowText(GetDlgItem(hwnd, IDC_PATH_TAPES      ), path_tapes      , MAX_PATH);
            GetWindowText(GetDlgItem(hwnd, IDC_PATH_BKGRND     ), fn_bkgrnd       , MAX_PATH);
            fix_paths();
            audit(FALSE);

            if (pixelubyte)
            {   free(pixelubyte);
                pixelubyte = NULL;
                pixelulong = NULL;
            }
            if (fn_bkgrnd[0])
            {   break_pathname(fn_bkgrnd, path_bkgrnd, file_bkgrnd);
                DISCARD load_bmp(fn_bkgrnd);
            }
            fixupcolours();
            make_stars();
            redrawscreen();

            clearkybd();
            DISCARD EndDialog(hwnd, 0);
            return TRUE;
        acase IDCANCEL:
            clearkybd();
            DISCARD EndDialog(hwnd, 0);
            return TRUE;
        }
        return FALSE;
    case WM_PAINT: // no need for acase
        DISCARD BeginPaint(hwnd, &localps);
        DISCARD EndPaint(hwnd, &localps);
    adefault:
        return FALSE;
    }

    return FALSE;
}

MODULE FLAG zgetpath(STRPTR buffer) // getpath() is reserved on Amiga
{   TRANSIENT FLAG         success;
    TRANSIENT BROWSEINFO   bi;
    TRANSIENT LPITEMIDLIST IDList;
    PERSIST   TEXT         oldbuffer[MAX_PATH + 1]; // PERSISTent so as not to blow the stack

    strcpy(oldbuffer, buffer);

    bi.hwndOwner        = MainWindowPtr;
    bi.pidlRoot         = NULL;
    bi.pszDisplayName   = buffer;
    bi.lpszTitle        = LLL(MSG_HAIL_PATH, "Choose Path");
    bi.ulFlags          = BIF_EDITBOX;
    bi.lpfn             = NULL;
    bi.lParam           = 0;
    bi.iImage           = 0;
    if ((IDList = SHBrowseForFolder(&bi)))
    {   if (SHGetPathFromIDList(IDList, buffer))
        {   success = TRUE;
        } else
        {   success = FALSE;
            strcpy(buffer, oldbuffer);
        }
        CoTaskMemFree(IDList);
        // IDList = NULL;

        return success;
    } // implied else

    // leave buffer alone
    return FALSE;
}

MODULE void ghost_resetpaths(HWND hwnd)
{   if
    (   !strcmp(temppath_disks      , PATH_DISKS)
     && !strcmp(temppath_games      , PATH_GAMES)
     && !strcmp(temppath_projects   , PATH_PROJECTS)
     && !strcmp(temppath_screenshots, PATH_SCREENSHOTS)
     && !strcmp(temppath_scripts    , PATH_SCRIPTS)
     && !strcmp(temppath_tapes      , PATH_TAPES)
     && !strcmp(temppath_bkgrnd     , PATH_BKGRNDS)
     && tempfile_bkgrnd[0] == EOS
    )
    {   EnableWindow(GetDlgItem(hwnd, IDC_RESETPATHS), FALSE);
    } else
    {   EnableWindow(GetDlgItem(hwnd, IDC_RESETPATHS), TRUE);
}   }

EXPORT void rexx_input(void)
{   sound_off(FALSE);
    opening = modal = TRUE;
    settitle();

    DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_REXX), MainWindowPtr, REXXDlgProc);

    opening = modal = FALSE;
    settitle();
    sound_on(TRUE);
}

MODULE BOOL CALLBACK REXXDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   switch (Message)
    {
    case WM_INITDIALOG:
        SetWindowText(hwnd, LLL(MSG_INPUTDATA, "Input Data to Script"));
        DISCARD SetFocus(GetDlgItem(hwnd, IDC_REXX));
        return TRUE;
    case WM_CLOSE: // no need for acase
        DISCARD GetWindowText(GetDlgItem(hwnd, IDC_REXX), rexxwhere, 256);
        clearkybd();
        DISCARD EndDialog(hwnd, 0);
        return TRUE;
    case WM_COMMAND: // no need for acase
        if (LOWORD (wParam) == IDOK)
        {   DISCARD GetWindowText(GetDlgItem(hwnd, IDC_REXX), rexxwhere, 256);
            clearkybd();
            DISCARD EndDialog(hwnd, 0);
            return TRUE;
        }
        if (LOWORD (wParam) == IDCANCEL) // eg Esc key
        {   rexxwhere[0] = EOS;
            clearkybd();
            DISCARD EndDialog(hwnd, 0);
            return TRUE;
        }

        return FALSE;
    default: // no need for adefault
        return FALSE;
}   }

EXPORT void showthetod(void)
{   // assert(showtod);

    sound_off(FALSE);
    opening = modal = TRUE;
    settitle();
    DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_TOD), MainWindowPtr, TipOfDayDlgProc);
    opening = modal = FALSE;
    settitle();
    sound_on(TRUE);

    if (nexttod == TIPSOFDAYS - 1)
    {   nexttod = 0;
    } else
    {   nexttod++;
}   }

MODULE BOOL CALLBACK TipOfDayDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{   FAST    HICON localhicon;
    PERSIST TEXT  tempstring[1024 + 1];

    switch (Message)
    {
    case WM_INITDIALOG:
        DISCARD SetWindowText(hwnd, LLL(MSG_TOD, "Tip of the Day"));
        setdlgtext(hwnd, IDC_SHOWTOD, MSG_EMULATOR_SHOWTOD2, "&Show tips at startup?");
        setdlgtext(hwnd, IDOK,        MSG_OK2,               "&OK");
        setdlgtext(hwnd, IDC_NEXTTIP, MSG_NEXTTOD2,          "&Next tip");

        localhicon = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon), IMAGE_ICON, 32, 32, 0);
        DISCARD SendMessage(GetDlgItem(hwnd, IDL_GLYPH), STM_SETICON, (WPARAM) localhicon, (LPARAM) 0);

        sprintf(tempstring, "#%d. %s", nexttod + 1, LLL(tods[nexttod].cat, tods[nexttod].str));
        SetDlgItemText(hwnd, IDL_TOD, tempstring);

        DISCARD SendMessage
        (   GetDlgItem(hwnd, IDC_SHOWTOD),
            BM_SETCHECK,
            showtod ? BST_CHECKED : BST_UNCHECKED,
            0
        );
    return TRUE;
    case WM_CLOSE: // no need for acase
        clearkybd();
        DISCARD EndDialog(hwnd, 0);
    return TRUE;
    case WM_DRAWITEM: // no need for acase
        switch (wParam)
        {
        case  IDOK: drawitem((struct tagDRAWITEMSTRUCT*) lParam, FALSE);
        }
    return TRUE;
    case WM_COMMAND: // no need for acase
        switch (LOWORD(wParam))
        {
        case IDC_SHOWTOD:
            if (SendMessage(GetDlgItem(hwnd, IDC_SHOWTOD), BM_GETCHECK, 0, 0) == BST_CHECKED)
            {   showtod = TRUE;
            } else
            {   showtod = FALSE;
            }
        acase IDC_NEXTTIP:
            if (nexttod == TIPSOFDAYS - 1)
            {   nexttod = 0;
            } else
            {   nexttod++;
            }
            sprintf(tempstring, "#%d. %s", nexttod + 1, LLL(tods[nexttod].cat, tods[nexttod].str));
            SetDlgItemText(hwnd, IDL_TOD, tempstring);
        acase IDOK:
        case IDCANCEL:
            clearkybd();
            DISCARD EndDialog(hwnd, 0);
        }
    return TRUE;
    }

    return FALSE;
}

MODULE void update_rearrange(void)
{   FLAG resettable = FALSE;
    int  i, j;

    for (i = 0; i < 24; i++)
    {   if
        (   tempbutton[0][0] == rearrange[i].rearr[0]
         && tempbutton[0][1] == rearrange[i].rearr[1]
         && tempbutton[0][2] == rearrange[i].rearr[2]
        )
        {   // assert(tempbutton[0][3] == rearrange[i].rearr[3]);
            DISCARD CheckRadioButton
            (   ModalWindowPtr,
                IDC_1STRA1234,
                IDC_1STRA4321,
                rearrange[i].hGad[0]
            );
        }
        if
        (   tempbutton[1][0] == rearrange[i].rearr[0]
         && tempbutton[1][1] == rearrange[i].rearr[1]
         && tempbutton[1][2] == rearrange[i].rearr[2]
        )
        {   // assert(tempbutton[1][3] == rearrange[i].rearr[3]);
            DISCARD CheckRadioButton
            (   ModalWindowPtr,
                IDC_2NDRA1234,
                IDC_2NDRA4321,
                rearrange[i].hGad[1]
            );
    }   }
    for (i = 24; i < 48; i++)
    {   if
        (   tempbutton[0][4] == rearrange[i].rearr[0]
         && tempbutton[0][5] == rearrange[i].rearr[1]
         && tempbutton[0][6] == rearrange[i].rearr[2]
        )
        {   // assert(tempbutton[0][7] == rearrange[i].rearr[3]);
            DISCARD CheckRadioButton
            (   ModalWindowPtr,
                IDC_1STRA5678,
                IDC_1STRA8765,
                rearrange[i].hGad[0]
            );
        }
        if
        (   tempbutton[1][4] == rearrange[i].rearr[0]
         && tempbutton[1][5] == rearrange[i].rearr[1]
         && tempbutton[1][6] == rearrange[i].rearr[2]
        )
        {   // assert(tempbutton[1][7] == rearrange[i].rearr[3]);
            DISCARD CheckRadioButton
            (   ModalWindowPtr,
                IDC_2NDRA5678,
                IDC_2NDRA8765,
                rearrange[i].hGad[1]
            );
    }   }

    for (i = 0; i < 2; i++)
    {   for (j = 0; j < 8; j++)
        {   if (tempbutton[i][j] != j + 1)
            {   resettable = TRUE;
                break; // for speed
    }   }   }
    EnableWindow(GetDlgItem(ModalWindowPtr, IDC_RESETREARRANGE), resettable);
}

MODULE void drawhiscores(void)
{   int  i, j, k,
         length;
    TEXT tempstring[14 + 1]; // enough for "HH:MM DD/MM/YY"

    for (i = 0; i < machines[machine].hiscores; i++)
    {   j = machines[machine].firsthiscore + i;
        tempstring[13] = EOS; // important!
        longcomma(hiscore[j].score, tempstring);
        DISCARD SetDlgItemText(ModalWindowPtr, IDC_SCORE0 + i, tempstring);

        length = strlen(hiscore[j].thedatetime);
        for (k = 0; k < length; k++)
        {   if (hiscore[j].thedatetime[k] == '/')
            {   hiscore[j].thedatetime[k] = langs[language].datesep;
        }   }
        SetDlgItemText(ModalWindowPtr, IDC_DATETIME0 + i, hiscore[j].thedatetime);

        if (hiscore[j].played >= 864000) // 10 days or more
        {   strcpy(tempstring, "#:##:##:##"); // d:hh:mm:ss
        } elif (hiscore[j].played >= 86400) // 1 day or more
        {   sprintf
            (   tempstring,
                "%d:%02d:%02d:%02d", // d:hh:mm:ss
                 hiscore[j].played / 86400,
                (hiscore[j].played % 86400) / 3600,
                (hiscore[j].played %  3600) /   60,
                 hiscore[j].played %    60
            );
        } elif (hiscore[j].played >= 3600) // 1 hour or more
        {   sprintf
            (   tempstring,
                "%2d:%02d:%02d", // H:mm:ss (H can be 1 or 2 digits)
                 hiscore[j].played /  3600,
                (hiscore[j].played %  3600) /   60,
                 hiscore[j].played %    60
            );
        } else
        {   sprintf
            (   tempstring,
                "%2d:%02d", // M:ss (M can be 1 or 2 digits)
                 hiscore[j].played /    60,
                 hiscore[j].played %    60
            );
        }
        SetDlgItemText(ModalWindowPtr, IDC_PLAYED0 + i, tempstring);

        SetDlgItemText(ModalWindowPtr, IDC_ACHIEVEDBY0 + i, hiscore[j].username);
}   }
