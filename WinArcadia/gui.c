// INCLUDES---------------------------------------------------------------

#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
    #include "resource.h"
    #include <commctrl.h>
    #include <richedit.h>
    typedef unsigned char bool;
    #include "RA_Interface.h"
#endif

#include <stdio.h>
#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <string.h>

#ifdef AMIGA
    #include <exec/types.h>
    #define ALL_REACTION_CLASSES
    #define ALL_REACTION_MACROS
    #include <reaction/reaction.h>
#endif

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"
#ifdef AMIGA
    #include "amiga.h"
#endif

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT const struct MenuStruct menuinfo1[MENUITEMS] = {
{ "PROJECT.RESETTOGAME"            , 0                        , ""                                               }, //   0
{ "PROJECT.JUMPTOBIOS"             , 0                        , ""                                               },
{ "PROJECT.REINITIALIZE"           , 0                        , ""                                               },
{ "PROJECT.OPEN"                   , 0                        , ""                                               },
{ "PROJECT.REVERT"                 , 0                        , ""                                               },
{ "PROJECT.QUICKLOAD"              , 0                        , ""                                               },
{ "PROJECT.SAVECOS"                , 0                        , ""                                               },
{ "PROJECT.SAVESCRN.ACBM"          , 0                        , ""                                               },
{ "PROJECT.SAVESCRN.ILBM"          , 0                        , ""                                               },
{ "PROJECT.SAVESCRN.BMP"           , 0                        , ""                                               },
{ "PROJECT.SAVESCRN.GIF"           , 0                        , ""                                               }, //  10
{ "PROJECT.SAVESCRN.PCX"           , 0                        , ""                                               },
{ "PROJECT.SAVESCRN.PNG"           , 0                        , ""                                               },
{ "PROJECT.SAVESCRN.TIFF"          , 0                        , ""                                               },
{ "PROJECT.SAVESCRN.ASCII"         , 0                        , ""                                               },
{ "PROJECT.SAVESCRN.ASM"           , 0                        , ""                                               },
{ "PROJECT.QUICKSAVE"              , 0                        , ""                                               },
{ "PROJECT.AUDIT"                  , 0                        , ""                                               },
{ "PROJECT.REGISTER"               , 0                        , ""                                               },
{ "TOOLS.SERVER"                   , 0                        , ""                                               },
{ "TOOLS.CLIENT"                   , 0                        , ""                                               }, //  20
{ "PROJECT.RECENT1"                , 0                        , ""                                               },
{ "PROJECT.RECENT2"                , 0                        , ""                                               },
{ "PROJECT.RECENT3"                , 0                        , ""                                               },
{ "PROJECT.RECENT4"                , 0                        , ""                                               },
{ "PROJECT.RECENT5"                , 0                        , ""                                               },
{ "PROJECT.RECENT6"                , 0                        , ""                                               },
{ "PROJECT.RECENT7"                , 0                        , ""                                               },
{ "PROJECT.RECENT8"                , 0                        , ""                                               },
{ "PROJECT.ICONIFY"                , 0                        , ""                                               },
{ "PROJECT.UNICONIFY"              , 0                        , ""                                               }, //  30
{ "PROJECT.QUIT"                   , 0                        , ""                                               },
{ "EDIT.COPYSCREEN"                , 0                        , ""                                               },
{ "EDIT.COPYTEXT"                  , 0                        , ""                                               },
{ "EDIT.PASTETEXT"                 , 0                        , ""                                               },
{ "EDIT.EMPTYCLIPBOARD"            , 0                        , ""                                               },
{ "VIEW.HIGHSCORES"                , 0                        , ""                                               },
{ "VIEW.DEBUGGER"                  , 0                        , ""                                               },
{ "VIEW.MENUBAR"                   , 0                        , ""                                               },
{ "VIEW.POINTER"                   , 0                        , ""                                               },
{ "VIEW.SIDEBAR"                   , 0                        , ""                                               }, //  40
{ "VIEW.STATUSBAR"                 , 0                        , ""                                               },
{ "VIEW.TITLEBAR"                  , 0                        , ""                                               },
{ "VIEW.TOOLBAR"                   , 0                        , ""                                               },
{ "VIEW.BUILTINGAMES"              , 0                        , ""                                               },
{ ""                               , 0                        , ""                                               }, // MENUFAKE_SORTBY
{ ""                               , 0                        , ""                                               }, // MENUFAKE_SPEEDINDICATOR
{ "MACRO.STARTRECORDING"           , 0                        , ""                                               },
{ "MACRO.RESTARTPLAYBACK"          , 0                        , ""                                               },
{ "MACRO.STOP"                     , 0                        , ""                                               },
{ "MACRO.LOOP"                     , 0                        , ""                                               }, //  50
{ ""                               , MSG_MENUHELP_WARN        , "Assembler warnings on/off"                      }, // MENUITEM_WARN
{ "MACRO.ANIMS.ANIM"               , 0                        , ""                                               },
{ "MACRO.ANIMS.GIF"                , 0                        , ""                                               },
{ "MACRO.ANIMS.MNG"                , 0                        , ""                                               },
{ "MACRO.ANIMS.APNG"               , 0                        , ""                                               },
{ "MACRO.SOUNDS.8SVX"              , 0                        , ""                                               },
{ "MACRO.SOUNDS.AIFF"              , 0                        , ""                                               },
{ "MACRO.SOUNDS.SMUS"              , 0                        , ""                                               },
{ "MACRO.SOUNDS.MIDI"              , 0                        , ""                                               },
{ "MACRO.SOUNDS.PSG"               , 0                        , ""                                               }, //  60
{ "MACRO.SOUNDS.WAV"               , 0                        , ""                                               },
{ "MACRO.SOUNDS.YM"                , 0                        , ""                                               },
{ "MACRO.RUNREXX"                  , 0                        , ""                                               },
{ "MACRO.REPEATREXX"               , 0                        , ""                                               },
{ ""                               , MSG_MENUHELP_QUOTES      , "Send message to other player"                   }, // MENUITEM_QUOTES
{ ""                               , MSG_MENUHELP_CLEARCOV    , "Clear coverage report"                          }, // MENUITEM_CLEARCOV
{ ""                               , MSG_MENUHELP_CLS         , "Clear screen (of console window)"               }, // MENUITEM_CLS
{ ""                               , MSG_MENUHELP_HELP        , "View help"                                      }, // MENUITEM_HELP
{ ""                               , MSG_MENUHELP_SAY         , "Speak message"                                  }, // MENUITEM_SAY
{ ""                               , MSG_MENUHELP_SYSTEM      , "Quit emulator"                                  }, // MENUITEM_SYSTEM          70
{ ""                               , MSG_MENUHELP_ASTERISK    , "Execute host OS command"                        }, // MENUITEM_ASTERISK
{ ""                               , MSG_MENUHELP_ASM         , "Assemble source code"                           }, // MENUITEM_ASM
{ ""                               , MSG_MENUHELP_DELETE      , "Delete file from floppy disk"                   }, // MENUITEM_DELETE
{ ""                               , MSG_MENUHELP_DIR         , "List files on floppy disk"                      }, // MENUITEM_DIR
{ ""                               , MSG_MENUHELP_DISGAME     , "Disassemble and show address range"             }, // MENUITEM_DISGAME
{ ""                               , MSG_MENUHELP_EDIT        , "Edit source code"                               }, // MENUITEM_EDIT
{ ""                               , MSG_MENUHELP_EXTRACT     , "Extract files from floppy disk"                 }, // MENUITEM_EXTRACT
{ ""                               , MSG_MENUHELP_LOADBIN     , "Load binary to start-addr from filename"        }, // MENUITEM_LOADBIN
{ ""                               , MSG_MENUHELP_SAVEAOF     , "Save address range as filename (AOF/EOF)"       }, // MENUITEM_SAVEAOF
{ ""                               , MSG_MENUHELP_SAVEBIN     , "Save address range as filename (BIN)"           }, // MENUITEM_SAVEBIN         80
{ ""                               , MSG_MENUHELP_SAVEBPNF    , "Save address range as filename (BPNF)"          }, // MENUITEM_SAVEBPNF
{ ""                               , MSG_MENUHELP_SAVECMD     , "Save address range as filename (CMD/IMAG/MOD)"  }, // MENUITEM_SAVECMD
{ ""                               , MSG_MENUHELP_SAVEHEX     , "Save address range as filename (Intel hex)"     }, // MENUITEM_SAVEHEX
{ ""                               , MSG_MENUHELP_SAVESMS     , "Save address $0 to end-addr as filename (SMS)"  }, // MENUITEM_SAVESMS
{ ""                               , MSG_MENUHELP_SAVETVC     , "Save address range as filename (TVC)"           }, // MENUITEM_SAVETVC
{ ""                               , 0                        , ""                                               }, // MENUFAKE_DRIVE
{ ""                               , MSG_MENUHELP_DOKE        , "Change word at address to value"                }, // MENUITEM_DOKE
{ ""                               , MSG_MENUHELP_POKE        , "Change byte at address to value"                }, // MENUITEM_POKE
{ ""                               , MSG_MENUHELP_FPOKE       , "Change byte on disk to value"                   }, // MENUITEM_FPOKE
{ ""                               , MSG_MENUHELP_WRITEPORT   , "Write to I/O port"                              }, // MENUITEM_WRITEPORT       90
{ ""                               , MSG_MENUHELP_EQUALS      , "List any or all known labels"                   }, // MENUITEM_EQUALS
{ ""                               , MSG_MENUHELP_COVER       , "View coverage report"                           }, // MENUITEM_COVER
{ ""                               , MSG_MENUHELP_PEEK        , "Display address range as data"                  }, // MENUITEM_PEEK
{ ""                               , MSG_MENUHELP_DIS         , "Disassemble address range as code"              }, // MENUITEM_DIS
{ ""                               , MSG_MENUHELP_ERROR       , "Show error message"                             }, // MENUITEM_ERROR
{ ""                               , MSG_MENUHELP_FPEEK       , "Display address range from disk"                }, // MENUITEM_FPEEK
{ ""                               , MSG_MENUHELP_HISTORY     , "View command history"                           }, // MENUITEM_HISTORY
{ ""                               , MSG_MENUHELP_IM          , "View address range as imagery"                  }, // MENUITEM_IM
{ ""                               , MSG_MENUHELP_READPORT    , "Read from I/O port"                             }, // MENUITEM_READPORT
{ ""                               , MSG_MENUHELP_VIEW_BASIC  , "View BASIC variables"                           }, // MENUITEM_VIEW_BASIC     100
{ ""                               , MSG_MENUHELP_VIEW_BIOS   , "View BIOS RAM"                                  }, // MENUITEM_VIEW_BIOS
{ ""                               , MSG_MENUHELP_VIEW_CPU    , "View CPU status"                                }, // MENUITEM_VIEW_CPU
{ ""                               , MSG_MENUHELP_VIEW_PSG    , "View PSGs/TMSes status"                         }, // MENUITEM_VIEW_PSG
{ ""                               , MSG_MENUHELP_VIEW_RAM    , "View RAM"                                       }, // MENUITEM_VIEW_RAM
{ ""                               , MSG_MENUHELP_VIEW_SCRN   , "View screen contents"                           }, // MENUITEM_VIEW_SCRN
{ ""                               , MSG_MENUHELP_VIEW_UDG    , "View user-defined graphics"                     }, // MENUITEM_VIEW_UDG
{ ""                               , MSG_MENUHELP_VIEW_XVI    , "View UVI/PVI status"                            }, // MENUITEM_VIEW_XVI
{ ""                               , MSG_MENUHELP_L_A         , "Log illegal memory accesses on/off"             }, // MENUITEM_L_A
{ ""                               , MSG_MENUHELP_L_B         , "Log BIOS calls on/off"                          }, // MENUITEM_L_B
{ ""                               , MSG_MENUHELP_L_C         , "Log inefficient code on/off"                    }, // MENUITEM_L_C            110
{ ""                               , MSG_MENUHELP_L_I         , "Log illegal instructions on/off"                }, // MENUITEM_L_I
{ ""                               , MSG_MENUHELP_L_N         , "Log interrupts on/off"                          }, // MENUITEM_L_N
{ ""                               , MSG_MENUHELP_L_S         , "Log subroutine calls on/off"                    }, // MENUITEM_L_S
{ ""                               , MSG_MENUHELP_PL          , "Pause after logging on/off"                     }, // MENUITEM_PL
{ ""                               , MSG_MENUHELP_T           , "Trace CPU on/off"                               }, // MENUITEM_T
{ ""                               , MSG_MENUHELP_G           , "Pause/unpause & optionally set breakpoint"      }, // MENUITEM_G
{ ""                               , MSG_MENUHELP_GI          , "Generate an interrupt"                          }, // MENUITEM_GI
{ ""                               , MSG_MENUHELP_I           , "Ignore next instruction"                        }, // MENUITEM_I
{ ""                               , MSG_MENUHELP_JUMP        , "Jump to address"                                }, // MENUITEM_JUMP
{ ""                               , MSG_MENUHELP_O           , "Step over subroutine"                           }, // MENUITEM_O              120
{ ""                               , MSG_MENUHELP_S           , "Step next instruction"                          }, // MENUITEM_S
{ ""                               , MSG_MENUHELP_R           , "Run to rasterline number"                       }, // MENUITEM_R
{ ""                               , MSG_MENUHELP_R_F         , "Run to next frame"                              }, // MENUITEM_R_F
{ ""                               , MSG_MENUHELP_R_I         , "Run to next interrupt"                          }, // MENUITEM_R_I
{ ""                               , MSG_MENUHELP_R_L         , "Run to end of loop"                             }, // MENUITEM_R_L
{ ""                               , MSG_MENUHELP_R_R         , "Run to next rasterline"                         }, // MENUITEM_R_R
{ ""                               , MSG_MENUHELP_R_S         , "Run to end of subroutine"                       }, // MENUITEM_R_S
{ ""                               , MSG_MENUHELP_BP          , "Add code breakpoints to address range"          }, // MENUITEM_BP
{ ""                               , MSG_MENUHELP_BC          , "Clear code breakpoints from address range"      }, // MENUITEM_BC
{ ""                               , MSG_MENUHELP_BL          , "List all code breakpoints"                      }, // MENUITEM_BL             130
{ ""                               , MSG_MENUHELP_FP          , "Add disk watchpoints to address range"          }, // MENUITEM_FP
{ ""                               , MSG_MENUHELP_FC          , "Clear disk watchpoints from address range"      }, // MENUITEM_FC
{ ""                               , MSG_MENUHELP_FL          , "List all disk watchpoints"                      }, // MENUITEM_FL
{ ""                               , MSG_MENUHELP_IP          , "Add I/O port watchpoints to port range"         }, // MENUITEM_IP
{ ""                               , MSG_MENUHELP_IC          , "Clear I/O port watchpoints from port range"     }, // MENUITEM_IC
{ ""                               , MSG_MENUHELP_IL          , "List all I/O port watchpoints"                  }, // MENUITEM_IL
{ ""                               , MSG_MENUHELP_WP          , "Add data watchpoints to address range"          }, // MENUITEM_WP
{ ""                               , MSG_MENUHELP_WC          , "Clear data watchpoints from address range"      }, // MENUITEM_WC
{ ""                               , MSG_MENUHELP_WL          , "List all data watchpoints"                      }, // MENUITEM_WL
{ ""                               , MSG_MENUHELP_PB          , "Pause after breakpoints/watchpoints on/off"     }, // MENUITEM_PB             140
{ ""                               , MSG_MENUHELP_WR          , "Watch reads on/off"                             }, // MENUITEM_WR
{ ""                               , 0                        , ""                                               }, // MENUFAKE_WW
{ ""                               , MSG_MENUHELP_CLEARSYM    , "Clear symbol(s)"                                }, // MENUITEM_CLEARSYM
{ ""                               , MSG_MENUHELP_DEFSYM      , "Define symbol"                                  }, // MENUITEM_DEFSYM
{ ""                               , MSG_MENUHELP_LOADSYM     , "Load symbol table from filename"                }, // MENUITEM_LOADSYM
{ ""                               , MSG_MENUHELP_SAVESYM     , "Save symbol table as filename"                  }, // MENUITEM_SAVESYM
{ ""                               , MSG_MENUHELP_COMP        , "Compare memory blocks"                          }, // MENUITEM_COMP
{ ""                               , MSG_MENUHELP_COPY        , "Copy memory from source to target"              }, // MENUITEM_COPY
{ ""                               , MSG_MENUHELP_FILL        , "Fill address range with value"                  }, // MENUITEM_FILL
{ ""                               , MSG_MENUHELP_FIND        , "Find value in address range"                    }, // MENUITEM_FIND           150
{ ""                               , MSG_MENUHELP_REL         , "Calculate relative offset of target-addr"       }, // MENUITEM_REL
{ ""                               , MSG_MENUHELP_SWAP        , "Swap memory blocks"                             }, // MENUITEM_SWAP
{ ""                               , MSG_MENUHELP_TRAIN       , "Make trainer"                                   }, // MENUITEM_TRAIN
{ ""                               , 0                        , ""                                               }, // MENUFAKE_BASE
{ ""                               , 0                        , ""                                               }, // MENUFAKE_CPU
{ ""                               , 0                        , ""                                               }, // MENUITEM_N
{ ""                               , 0                        , ""                                               }, // MENUFAKE_TU
{ ""                               , 0                        , ""                                               }, // MENUFAKE_VERBOSITY
{ ""                               , MSG_MENUHELP_GR          , "Use guide ray on/off"                           }, // MENUITEM_GR
{ ""                               , 0                        , ""                                               }, // MENUFAKE_DRAW           160
{ ""                               , 0                        , ""                                               }, // MENUFAKE_SPR
{ "DEBUG.DEBUGGER"                 , 0                        , ""                                               }, // MENUITEM_TOOLSDEBUGGER
{ "EDIT.MEMORY"                    , 0                        , ""                                               }, // MENUITEM_EDITMEMORY
{ "TOOLS.MUSIC"                    , 0                        , ""                                               }, // MENUITEM_MUSIC
{ "EDIT.SCREEN"                    , 0                        , ""                                               }, // MENUITEM_SCREENEDITOR
{ "EDIT.SPRITES"                   , 0                        , ""                                               }, // MENUITEM_SPRITEVIEWER
{ "TOOLS.CPU"                      , 0                        , ""                                               }, // MENUITEM_VIEWMONITOR_CPU
{ "TOOLS.PSG"                      , 0                        , ""                                               }, // MENUITEM_VIEWMONITOR_PSGS
{ "TOOLS.XVI"                      , 0                        , ""                                               }, // MENUITEM_VIEWMONITOR_XVI
{ "TOOLS.RETROACHIEVEMENTS"        , 0                        , ""                                               }, // MENUITEM_CHEEVOS1       170
{ "PERIPHERALS.CONTROLS"           , 0                        , ""                                               }, // MENUITEM_CONTROLS
{ "PERIPHERALS.DIPSWITCHES"        , 0                        , ""                                               }, // MENUITEM_DIPSWITCHES
{ ""                               , 0                        , ""                                               }, // MENUFAKE_LEFT
{ "PERIPHERALS.LEFT.AUTOFIRE"      , 0                        , ""                                               }, // MENUITEM_AUTOFIRE1
{ "PERIPHERALS.LEFT.REQUIREBUTTON" , 0                        , ""                                               }, // MENUITEM_REQUIREBUTTON1
{ "PERIPHERALS.LEFT.FORCEFEEDBACK" , 0                        , ""                                               }, // MENUITEM_FORCEFEEDBACK1
{ ""                               , 0                        , ""                                               }, // MENUFAKE_RIGHT
{ "PERIPHERALS.RIGHT.AUTOFIRE"     , 0                        , ""                                               }, // MENUITEM_AUTOFIRE2
{ "PERIPHERALS.RIGHT.REQUIREBUTTON", 0                        , ""                                               }, // MENUITEM_REQUIREBUTTON2
{ "PERIPHERALS.RIGHT.FORCEFEEDBACK", 0                        , ""                                               }, // MENUITEM_FORCEFEEDBACK2 180
{ "PERIPHERALS.TAPEDECK"           , 0                        , ""                                               }, // MENUITEM_TAPEDECK
{ "PERIPHERALS.FLOPPYDRIVE"        , 0                        , ""                                               }, // MENUITEM_FLOPPYDRIVE
{ "PERIPHERALS.PAPERTAPE"          , 0                        , ""                                               }, // MENUITEM_PAPERTAPE
{ "PERIPHERALS.PRINTER"            , 0                        , ""                                               }, // MENUITEM_PRINTER
{ ""                               , 0                        , ""                                               }, // MENUFAKE_ELEKTORBIOS
{ ""                               , 0                        , ""                                               }, // MENUFAKE_PIPBUGBIOS
{ ""                               , 0                        , ""                                               }, // MENUFAKE_BINBUGBIOS
{ ""                               , 0                        , ""                                               }, // MENUFAKE_CD2650BIOS
{ ""                               , 0                        , ""                                               }, // MENUFAKE_PHUNSYBIOS
{ ""                               , 0                        , ""                                               }, // MENUFAKE_SELBSTBIOS     190
{ "EDIT.PALETTE"                   , 0                        , ""                                               }, // MENUITEM_PALETTE
{ "SETTINGS.COLOURS.ARTEFACTS"     , 0                        , ""                                               }, // MENUITEM_UNDITHER
{ "SETTINGS.COLOURS.DARKENBG"      , 0                        , ""                                               }, // MENUITEM_DARKENBG
{ "SETTINGS.COLOURS.FLAGLINE"      , 0                        , ""                                               }, // MENUITEM_FLAGLINE
{ "SETTINGS.COLOURS.INVERSE"       , 0                        , ""                                               }, // MENUITEM_INVERSE
{ ""                               , 0                        , ""                                               }, // MENUFAKE_COLOURSET
{ ""                               , 0                        , ""                                               }, // MENUFAKE_BINBUGDOS
{ ""                               , 0                        , ""                                               }, // MENUFAKE_TWINDOS
{ ""                               , 0                        , ""                                               }, // MENUFAKE_CD2650DOS
{ "SETTINGS.EMULATOR.AUTOSAVE"     , 0                        , ""                                               }, // MENUFAKE_AUTOSAVE       200
{ "SETTINGS.EMULATOR.UPDATES"      , 0                        , ""                                               }, // MENUITEM_STARTUPUPDATES
{ "SETTINGS.EMULATOR.CONFIRMQUIT"  , 0                        , ""                                               }, // MENUITEM_CONFIRM
{ "SETTINGS.EMULATOR.ICONS"        , 0                        , ""                                               }, // MENUITEM_CREATEICONS
{ "SETTINGS.EMULATOR.CONSOLE"      , 0                        , ""                                               }, // MENUITEM_CONSOLE
{ "SETTINGS.EMULATOR.IDENTIFY"     , 0                        , ""                                               }, // MENUITEM_EMUID
{ "SETTINGS.EMULATOR.POST"         , 0                        , ""                                               }, // MENUITEM_POST
{ "SETTINGS.EMULATOR.RANDOMIZE"    , 0                        , ""                                               }, // MENUITEE_RANDOMIZE
{ "SETTINGS.EMULATOR.SENSEGAME"    , 0                        , ""                                               }, // MENUITEM_SENSEGAME
{ "SETTINGS.EMULATOR.SHOWTIPS"     , 0                        , ""                                               }, // MENUITEM_SHOWTOD
{ "SETTINGS.EMULATOR.STUBS"        , 0                        , ""                                               }, // MENUITEM_USESTUBS       210
{ ""                               , 0                        , ""                                               }, // MENUFAKE_LOGTOFILE
{ "SETTINGS.EMULATOR.PATHS"        , 0                        , ""                                               }, // MENUITEM_PATHS
{ ""                               , 0                        , ""                                               }, // MENUFAKE_FILTER
{ "SETTINGS.FILTERS.SCANLINES"     , 0                        , ""                                               }, // MENUITEM_SCANLINES
{ "SETTINGS.FILTERS.STRETCHWINDOWED",0                        , ""                                               }, // MENUITEM_STRETCH43
{ ""                               , 0                        , ""                                               }, // MENUFAKE_STRETCHING
{ ""                               , 0                        , ""                                               }, // MENUFAKE_SIZE
{ "SETTINGS.GRAPHICS.DEJITTER"     , 0                        , ""                                               }, // MENUITEM_DEJITTER
{ "SETTINGS.GRAPHICS.SKIES"        , 0                        , ""                                               }, // MENUITEM_SKIES
{ "SETTINGS.GRAPHICS.INTERSECTIONS", 0                        , ""                                               }, // MENUITEM_CORNERS        220
{ "SETTINGS.GRAPHICS.FULLSCREEN"   , 0                        , ""                                               }, // MENUITEM_FULLSCREEN
{ "SETTINGS.GRAPHICS.NARROW"       , 0                        , ""                                               }, // MENUITEM_NARROW
{ ""                               , 0                        , ""                                               }, // spare                   223
{ "SETTINGS.GRAPHICS.UNLIT"        , 0                        , ""                                               }, // MENUITEM_UNLIT
{ "SETTINGS.GRAPHICS.SHOWLEDS"     , 0                        , ""                                               }, // MENUITEM_SHOWLEDS
{ "SETTINGS.INPUT.ANALOG"          , 0                        , ""                                               }, // MENUITEM_ANALOG
{ "SETTINGS.INPUT.POSITIONS"       , 0                        , ""                                               }, // MENUITEM_SHOWPOSITIONS
{ "SETTINGS.INPUT.SPRINGS"         , 0                        , ""                                               }, // MENUITEM_SPRINGLOADED
{ "SETTINGS.INPUT.SWAPPED"         , 0                        , ""                                               }, // MENUITEM_SWAPPED
{ "SETTINGS.INPUT.CALIBRATE"       , 0                        , ""                                               }, // MENUITEM_CALIBRATE      230
{ "SETTINGS.INPUT.REARRANGE"       , 0                        , ""                                               }, // MENUITEM_REARRANGE
{ "SETTINGS.INPUT.REDEFINE"        , 0                        , ""                                               }, // MENUITEM_REDEFINEKEYS
{ "SETTINGS.INPUT.SENSITIVITY"     , 0                        , ""                                               }, // MENUITEM_SENSITIVITY
{ "SETTINGS.INPUT.LOWERCASE"       , 0                        , ""                                               }, // MENUITEM_LOWERCASEINPUT
{ "SETTINGS.INPUT.CONFINE"         , 0                        , ""                                               }, // MENUITEM_CONFINE
{ "SETTINGS.INPUT.AUTOCOIN"        , 0                        , ""                                               }, // MENUITEM_AUTOCOIN
{ "SETTINGS.INPUT.GUESTRMB"        , 0                        , ""                                               }, // MENUITEM_GUESTRMB
{ "SETTINGS.INPUT.QUEUEKEYSTROKES" , 0                        , ""                                               }, // MENUITEM_QUEUEKEYSTROKES
{ ""                               , 0                        , ""                                               }, // MENUFAKE_KEYMAP
{ ""                               , 0                        , ""                                               }, // MENUFAKE_LANGUAGE       240
{ ""                               , 0                        , ""                                               }, // MENUFAKE_MACHINE
{ "SETTINGS.SOUND.ENABLED"         , 0                        , ""                                               }, // MENUITEM_SOUND
{ "SETTINGS.SOUND.ADJUST"          , 0                        , ""                                               }, // MENUITEM_ADJUSTSOUND
{ "SETTINGS.SOUND.AMBIENT"         , 0                        , ""                                               }, // MENUITEM_AMBIENT
{ "SETTINGS.SOUND.FILTERED"        , 0                        , ""                                               }, // MENUITEM_FILTERED
{ "SETTINGS.SOUND.RETUNE"          , 0                        , ""                                               }, // MENUITEM_RETUNE
{ "SETTINGS.SOUND.OUTPUT"          , 0                        , ""                                               }, // MENUITEM_SOUNDOUTPUT
{ "SETTINGS.SPEED.ADJUST"          , 0                        , ""                                               }, // MENUITEM_ADJUSTSPEED
{ "SETTINGS.SPEED.PAUSED"          , 0                        , ""                                               }, // MENUITEM_PAUSED
{ "SETTINGS.SPEED.AUTOPAUSE"       , 0                        , ""                                               }, // MENUITEM_AUTOPAUSE      250
{ "SETTINGS.SPEED.LIMITREFRESHES"  , 0                        , ""                                               }, // MENUITEM_LIMITREFRESHES
{ "SETTINGS.SPEED.TURBO"           , 0                        , ""                                               }, // MENUITEM_TURBO
{ ""                               , 0                        , ""                                               }, // MENUFAKE_REGION
{ ""                               , 0                        , ""                                               }, // MENUFAKE_PRIORITY
{ "SETTINGS.SPRITES.COLLISIONS"    , 0                        , ""                                               }, // MENUITEM_COLLISIONS
{ ""                               , 0                        , ""                                               }, // MENUFAKE_DEMULTIPLEX
{ "SETTINGS.TRAINERS.CHEATLIVES"   , 0                        , ""                                               }, // MENUITEM_CHEATLIVES
{ "SETTINGS.TRAINERS.CHEATTIME"    , 0                        , ""                                               }, // MENUITEM_CHEATTIME
{ "SETTINGS.TRAINERS.INVINCIBILITY", 0                        , ""                                               }, // MENUITEM_INVINCIBILITY
{ "SETTINGS.TRAINERS.SKIPLEVEL"    , 0                        , ""                                               }, // MENUITEM_LEVELSKIP      260
{ "SETTINGS.GRAPHICS.BLANKING"     , 0                        , ""                                               }, // MENUITEM_USEMARGINS
{ "SETTINGS.GRAPHICS.BLINK"        , 0                        , ""                                               }, // MENUITEM_BLINK
{ "SETTINGS.GRAPHICS.INVERTFLASHED", 0                        , ""                                               }, // MENUITEM_COOMER
{ "SETTINGS.GRAPHICS.ROTATE"       , 0                        , ""                                               }, // MENUITEM_ROTATE
{ ""                               , 0                        , ""                                               }, // MENUFAKE_PIPBUGVDU
{ ""                               , 0                        , ""                                               }, // MENUFAKE_BINBUGVDU
{ ""                               , 0                        , ""                                               }, // MENUFAKE_CD2650VDU
{ "HELP.GAMEINFO"                  , 0                        , ""                                               }, // MENUITEM_GAMEINFO
{ "HELP.HOSTPADS"                  , 0                        , ""                                               }, // MENUITEM_HOSTPADS
{ "HELP.HOSTKYBD"                  , 0                        , ""                                               }, // MENUITEM_HOSTKYBD       270
{ "HELP.OPCODES"                   , 0                        , ""                                               }, // MENUITEM_OPCODES
{ "HELP.CODINGGUIDE"               , 0                        , ""                                               }, // MENUITEM_CODINGGUIDE
{ "HELP.GAMINGGUIDE"               , 0                        , ""                                               }, // MENUITEM_GAMINGGUIDE
{ "HELP.MANUAL"                    , 0                        , ""                                               }, // MENUITEM_MANUAL
{ "HELP.UPDATE"                    , 0                        , ""                                               }, // MENUITEM_UPDATE
{ "HELP.REACTION"                  , 0                        , ""                                               }, // MENUITEM_REACTION
{ "HELP.ABOUT"                     , 0                        , ""                                               }, // MENUITEM_ABOUT
{ ""                               , 0                        , ""                                               }, // MENUMENU_PROJECT
{ ""                               , 0                        , ""                                               }, // MENUMENU_EDIT
{ ""                               , 0                        , ""                                               }, // MENUMENU_VIEW           280
{ ""                               , 0                        , ""                                               }, // MENUMENU_MACRO
{ ""                               , 0                        , ""                                               }, // MENUMENU_DEBUG
{ ""                               , 0                        , ""                                               }, // MENUMENU_TOOLS
{ ""                               , 0                        , ""                                               }, // MENUMENU_PERIPHERALS
{ ""                               , 0                        , ""                                               }, // MENUMENU_SETTINGS
{ ""                               , 0                        , ""                                               }, // MENUMENU_HELP
{ ""                               , 0                        , ""                                               }, // MENUMENU_DEBUG_GENERAL
{ ""                               , 0                        , ""                                               }, // MENUMENU_DEBUG_FILE
{ ""                               , 0                        , ""                                               }, // MENUMENU_DEBUG_EDIT
{ ""                               , 0                        , ""                                               }, // MENUMENU_DEBUG_VIEW     290
{ ""                               , 0                        , ""                                               }, // MENUMENU_DEBUG_LOG
{ ""                               , 0                        , ""                                               }, // MENUMENU_DEBUG_RUN
{ ""                               , 0                        , ""                                               }, // MENUMENU_DEBUG_BP
{ ""                               , 0                        , ""                                               }, // MENUMENU_DEBUG_SYMBOLS
{ ""                               , 0                        , ""                                               }, // MENUMENU_DEBUG_TOOLS
{ ""                               , 0                        , ""                                               }, // MENUMENU_DEBUG_OPTIONS
{ ""                               , 0                        , ""                                               }, // MENUMENU_DEBUG_GRAPHICS
{ ""                               , 0                        , ""                                               }, // MENUMENU_LEFT
{ ""                               , 0                        , ""                                               }, // MENUMENU_RIGHT
{ ""                               , 0                        , ""                                               }, // MENUMENU_BIOS           300
{ ""                               , 0                        , ""                                               }, // MENUMENU_DOS
{ ""                               , 0                        , ""                                               }, // MENUMENU_FILTERS
{ ""                               , 0                        , ""                                               }, // MENUMENU_MACHINE
{ ""                               , 0                        , ""                                               }, // MENUMENU_SPRITES
{ ""                               , 0                        , ""                                               }, // MENUMENU_TRAINERS
{ ""                               , 0                        , ""                                               }, // MENUMENU_VDU            306
// new ones
{ ""                               , 0                        , ""                                               }, // MENUFAKE_FRAMEBASED     307
{ ""                               , MSG_MENUHELP_INJECT      , "Inject file onto floppy disk"                   }, // MENUITEM_INJECT         308
{ ""                               , MSG_MENUHELP_RENAME      , "Rename file on floppy disk"                     }, // MENUITEM_RENAME         309
{ ""                               , 0                        , ""                                               }, // MENUMENU_DEBUG_DISK     310
{ "SETTINGS.SPEED.EXACT"           , 0                        , ""                                               }, // MENUITEM_EXACTSPEED     311
{ "MACRO.GENERATE"                 , 0                        , ""                                               }, // MENUITEM_GENERATE       312
{ ""                               , 0                        , ""                                               }, // MENUMENU_MACRO_ANIMS    313
{ ""                               , 0                        , ""                                               }, // MENUMENU_MACRO_SOUNDS   314
{ "SETTINGS.GRAPHICS.BEZEL"        , 0                        , ""                                               }, // MENUITEM_BEZEL          315
{ ""                               , 0                        , ""                                               }, // MENUITEM_CHEEVOS2       316
{ "SETTINGS.INPUT.ERASEDELETED"    , 0                        , ""                                               }, // MENUITEM_ERASEDEL       317
{ "PERIPHERALS.INDUSTRIAL"         , 0                        , ""                                               }, // MENUITEM_INDUSTRIAL     318
}, menuinfo2[MENUOPTS] = {
{ "VIEW.SORTBY.NAME"               , 0                        , ""                                               }, // MENUOPT_SORTBYNAME        0
{ "VIEW.SORTBY.MACHINE"            , 0                        , ""                                               }, // MENUOPT_SORTBYMACHINE
{ "VIEW.INDICATOR.FPS"             , 0                        , ""                                               }, // MENUOPT_INDICATOR_FPS
{ "VIEW.INDICATOR.KHZ"             , 0                        , ""                                               }, // MENUOPT_INDICATOR_KHZ
{ "VIEW.INDICATOR.PERCENTAGE"      , 0                        , ""                                               }, // MENUOPT_INDICATOR_PERCENT
{ ""                               , MSG_MENUHELP_DRIVE_0     , "Change debugger drive to unit 0"                }, // MENUOPT_DRIVE_0
{ ""                               , MSG_MENUHELP_DRIVE_1     , "Change debugger drive to unit 1"                }, // MENUOPT_DRIVE_1
{ ""                               , MSG_MENUHELP_WATCH_NONE  , "Watch no writes"                                }, // MENUOPT_WW_NONE
{ ""                               , MSG_MENUHELP_WATCH_SOME  , "Watch some writes"                              }, // MENUOPT_WW_SOME
{ ""                               , MSG_MENUHELP_WATCH_ALL   , "Watch all writes"                               }, // MENUOPT_WW_ALL
{ ""                               , MSG_MENUHELP_BASE_BINARY , "Set default input base to binary"               }, // MENUOPT_BASE_BINARY      10
{ ""                               , MSG_MENUHELP_BASE_OCTAL  , "Set default input base to octal"                }, // MENUOPT_BASE_OCTAL
{ ""                               , MSG_MENUHELP_BASE_DECIMAL, "Set default input base to decimal"              }, // MENUOPT_BASE_DECIMAL
{ ""                               , MSG_MENUHELP_BASE_HEX    , "Set default input base to hexadecimal"          }, // MENUOPT_BASE_HEX
{ ""                               , MSG_MENUHELP_CPU_0       , "Set CPU to 2650/2650A"                          }, // MENUOPT_CPU_0
{ ""                               , MSG_MENUHELP_CPU_1       , "Set CPU to 2650B"                               }, // MENUOPT_CPU_1
{ ""                               , MSG_MENUHELP_N_0         , "Set notation to non-extended Signetics"         }, // MENUOPT_N_0
{ ""                               , MSG_MENUHELP_N_1         , "Set notation to extended Signetics"             }, // MENUOPT_N_1
{ ""                               , MSG_MENUHELP_N_2         , "Set notation to old CALM"                       }, // MENUOPT_N_2
{ ""                               , MSG_MENUHELP_N_3         , "Set notation to new CALM"                       }, // MENUOPT_N_3
{ ""                               , MSG_MENUHELP_TU_0        , "Set time unit to cycles"                        }, // MENUOPT_TU_0             20
{ ""                               , MSG_MENUHELP_TU_1        , "Set time unit to clocks"                        }, // MENUOPT_TU_1
{ ""                               , MSG_MENUHELP_TU_2        , "Set time unit to pixels"                        }, // MENUOPT_TU_2
{ ""                               , MSG_MENUHELP_VB_0        , "Set verbosity to minimum"                       }, // MENUOPT_VB_0
{ ""                               , MSG_MENUHELP_VB_1        , "Set table tracing mode"                         }, // MENUOPT_VB_1
{ ""                               , MSG_MENUHELP_VB_2        , "Set verbosity to maximum"                       }, // MENUOPT_VB_2
{ ""                               , MSG_MENUHELP_DRAW_0      , "Draw normally"                                  }, // MENUOPT_DRAW_0
{ ""                               , MSG_MENUHELP_DRAW_1      , "Show grid (most machines) or Teletext grid (Malzak)"                                      }, // MENUOPT_DRAW_1
{ ""                               , MSG_MENUHELP_DRAW_2      , "Show collision table (Arcadia/Interton/Elektor) or playfield grid (Malzak)"               }, // MENUOPT_DRAW_2
{ ""                               , MSG_MENUHELP_DRAW_3      , "Show CHARLINE table (Arcadia) or DMA read table (Interton/Elektor) or both grids (Malzak)"}, // MENUOPT_DRAW_3
{ ""                               , MSG_MENUHELP_DRAW_4      , "Show DMA write table (Interton/Elektor)"        }, // MENUOPT_DRAW_4           30
{ ""                               , MSG_MENUHELP_SPR_0       , "Set sprite mode to invisible"                   }, // MENUOPT_SPR_0
{ ""                               , MSG_MENUHELP_SPR_1       , "Set sprite mode to visible"                     }, // MENUOPT_SPR_1
{ ""                               , MSG_MENUHELP_SPR_2       , "Set sprite mode to numbered"                    }, // MENUOPT_SPR_2
{ "PERIPHERALS.LEFT.MOUSE"         , 0                        , ""                                               },
{ "PERIPHERALS.LEFT.KEYBOARD"      , 0                        , ""                                               },
{ "PERIPHERALS.LEFT.1STDJOY"       , 0                        , ""                                               },
{ "PERIPHERALS.LEFT.2NDDJOY"       , 0                        , ""                                               },
{ "PERIPHERALS.LEFT.2NDAJOY"       , 0                        , ""                                               },
{ "PERIPHERALS.LEFT.1STDPAD"       , 0                        , ""                                               },
{ "PERIPHERALS.LEFT.2NDDPAD"       , 0                        , ""                                               }, // MENUOPT_LEFT_2NDDPAD     40
{ "PERIPHERALS.LEFT.1STAPAD"       , 0                        , ""                                               },
{ "PERIPHERALS.LEFT.2NDAPAD"       , 0                        , ""                                               },
{ "PERIPHERALS.RIGHT.MOUSE"        , 0                        , ""                                               },
{ "PERIPHERALS.RIGHT.KEYBOARD"     , 0                        , ""                                               },
{ "PERIPHERALS.RIGHT.1STDJOY"      , 0                        , ""                                               },
{ "PERIPHERALS.RIGHT.2NDDJOY"      , 0                        , ""                                               },
{ "PERIPHERALS.RIGHT.2NDAJOY"      , 0                        , ""                                               },
{ "PERIPHERALS.RIGHT.1STDPAD"      , 0                        , ""                                               },
{ "PERIPHERALS.RIGHT.2NDDPAD"      , 0                        , ""                                               },
{ "PERIPHERALS.RIGHT.1STAPAD"      , 0                        , ""                                               }, // MENUOPT_RIGHT_1STAPAD    50
{ "PERIPHERALS.RIGHT.2NDAPAD"      , 0                        , ""                                               },
{ "SETTINGS.BIOS.PHILIPS"          , 0                        , ""                                               },
{ "SETTINGS.BIOS.HOBBYMODULE"      , 0                        , ""                                               },
{ "SETTINGS.BIOS.ARTEMIS"          , 0                        , ""                                               },
{ "SETTINGS.BIOS.HYBUG"            , 0                        , ""                                               },
{ "SETTINGS.BIOS.PIPBUG1"          , 0                        , ""                                               },
{ "SETTINGS.BIOS.PIPBUG2"          , 0                        , ""                                               },
{ "SETTINGS.BIOS.BINBUG35"         , 0                        , ""                                               },
{ "SETTINGS.BIOS.BINBUG36"         , 0                        , ""                                               },
{ "SETTINGS.BIOS.BINBUG61"         , 0                        , ""                                               }, // MENUOPT_BINBUG61BIOS     60
{ "SETTINGS.BIOS.GBUG"             , 0                        , ""                                               },
{ "SETTINGS.BIOS.MIKEBUG"          , 0                        , ""                                               },
{ "SETTINGS.BIOS.MYBUG"            , 0                        , ""                                               },
{ "SETTINGS.BIOS.IPL"              , 0                        , ""                                               },
{ "SETTINGS.BIOS.POPMON"           , 0                        , ""                                               },
{ "SETTINGS.BIOS.SUPERVISOR"       , 0                        , ""                                               },
{ "SETTINGS.BIOS.MINIMONITOR"      , 0                        , ""                                               },
{ "SETTINGS.BIOS.PHUNSY"           , 0                        , ""                                               },
{ "SETTINGS.BIOS.SELBSTNONE"       , 0                        , ""                                               },
{ "SETTINGS.BIOS.SELBST09"         , 0                        , ""                                               }, // MENUOPT_09BIOS           70
{ "SETTINGS.BIOS.SELBST10"         , 0                        , ""                                               },
{ "SETTINGS.BIOS.SELBST20"         , 0                        , ""                                               },
{ "SETTINGS.COLOURS.AMBER"         , 0                        , ""                                               },
{ "SETTINGS.COLOURS.GREEN"         , 0                        , ""                                               },
{ "SETTINGS.COLOURS.GREY"          , 0                        , ""                                               },
{ "SETTINGS.COLOURS.PURE"          , 0                        , ""                                               },
{ "SETTINGS.COLOURS.PVI"           , 0                        , ""                                               },
{ "SETTINGS.COLOURS.UVI"           , 0                        , ""                                               },
{ "SETTINGS.DOS.MICRODOS"          , 0                        , ""                                               },
{ "SETTINGS.DOS.VHSDOS"            , 0                        , ""                                               }, // MENUOPT_VHSDOS           80
{ "SETTINGS.DOS.NOBINBUGDOS"       , 0                        , ""                                               },
{ "SETTINGS.DOS.EXOS"              , 0                        , ""                                               },
{ "SETTINGS.DOS.SDOS20"            , 0                        , ""                                               },
{ "SETTINGS.DOS.SDOS40"            , 0                        , ""                                               },
{ "SETTINGS.DOS.SDOS42"            , 0                        , ""                                               },
{ "SETTINGS.DOS.TOS"               , 0                        , ""                                               },
{ "SETTINGS.DOS.UDOS"              , 0                        , ""                                               },
{ "SETTINGS.DOS.NOTWINDOS"         , 0                        , ""                                               },
{ "SETTINGS.DOS.CDDOS"             , 0                        , ""                                               },
{ "SETTINGS.DOS.P1DOS"             , 0                        , ""                                               }, // MENUOPT_P1DOS            90
{ "SETTINGS.DOS.NOCD2650DOS"       , 0                        , ""                                               },
{ "SETTINGS.EMULATOR.APPENDLOG"    , 0                        , ""                                               },
{ "SETTINGS.EMULATOR.IGNORELOG"    , 0                        , ""                                               },
{ "SETTINGS.EMULATOR.REPLACELOG"   , 0                        , ""                                               },
{ "SETTINGS.FILTERS.3D"            , 0                        , ""                                               },
{ "SETTINGS.FILTERS.HQX"           , 0                        , ""                                               },
{ "SETTINGS.FILTERS.SCALE2X"       , 0                        , ""                                               },
{ "SETTINGS.FILTERS.NONE"          , 0                        , ""                                               },
{ "SETTINGS.FILTERS.UNSTRETCHED"   , 0                        , ""                                               },
{ "SETTINGS.FILTERS.STRETCH43"     , 0                        , ""                                               }, // MENUOPT_STRETCH43       100
{ "SETTINGS.FILTERS.STRETCHTOFIT"  , 0                        , ""                                               },
{ "SETTINGS.GRAPHICS.1XSIZE"       , 0                        , ""                                               },
{ "SETTINGS.GRAPHICS.2XSIZE"       , 0                        , ""                                               },
{ "SETTINGS.GRAPHICS.3XSIZE"       , 0                        , ""                                               },
{ "SETTINGS.GRAPHICS.4XSIZE"       , 0                        , ""                                               },
{ "SETTINGS.GRAPHICS.5XSIZE"       , 0                        , ""                                               },
{ "SETTINGS.GRAPHICS.6XSIZE"       , 0                        , ""                                               },
{ "SETTINGS.INPUT.HOST"            , 0                        , ""                                               },
{ "SETTINGS.INPUT.EA78UT4"         , 0                        , ""                                               },
{ "SETTINGS.INPUT.TELETYPE"        , 0                        , ""                                               }, // MENUOPT_KEYMAP_TT       110
{ "SETTINGS.LANGUAGE.ENG"          , 0                        , ""                                               },
{ "SETTINGS.LANGUAGE.HOL"          , 0                        , ""                                               },
{ "SETTINGS.LANGUAGE.FRA"          , 0                        , ""                                               },
{ "SETTINGS.LANGUAGE.GER"          , 0                        , ""                                               },
{ "SETTINGS.LANGUAGE.GRE"          , 0                        , ""                                               },
{ "SETTINGS.LANGUAGE.ITA"          , 0                        , ""                                               },
{ "SETTINGS.LANGUAGE.POL"          , 0                        , ""                                               },
{ "SETTINGS.LANGUAGE.RUS"          , 0                        , ""                                               },
{ "SETTINGS.LANGUAGE.SPA"          , 0                        , ""                                               },
{ "SETTINGS.MACHINE.ARCADIA"       , 0                        , ""                                               }, // MENUOPT_ARCADIA         120
{ "SETTINGS.MACHINE.INTERTON"      , 0                        , ""                                               },
{ "SETTINGS.MACHINE.ELEKTOR"       , 0                        , ""                                               },
{ "SETTINGS.MACHINE.PIPBUG"        , 0                        , ""                                               },
{ "SETTINGS.MACHINE.BINBUG"        , 0                        , ""                                               },
{ "SETTINGS.MACHINE.INSTRUCTOR"    , 0                        , ""                                               },
{ "SETTINGS.MACHINE.TWIN"          , 0                        , ""                                               },
{ "SETTINGS.MACHINE.CD2650"        , 0                        , ""                                               },
{ "SETTINGS.MACHINE.PHUNSY"        , 0                        , ""                                               },
{ "SETTINGS.MACHINE.SELBST"        , 0                        , ""                                               },
{ "SETTINGS.MACHINE.MIKIT"         , 0                        , ""                                               }, // MENUOPT_MIKIT           130
{ "SETTINGS.MACHINE.ASTROWARS"     , 0                        , ""                                               },
{ "SETTINGS.MACHINE.GALAXIA"       , 0                        , ""                                               },
{ "SETTINGS.MACHINE.LASERBATTLE"   , 0                        , ""                                               },
{ "SETTINGS.MACHINE.LAZARIAN"      , 0                        , ""                                               },
{ "SETTINGS.MACHINE.MALZAK1"       , 0                        , ""                                               },
{ "SETTINGS.MACHINE.MALZAK2"       , 0                        , ""                                               },
{ "SETTINGS.MACHINE.8550"          , 0                        , ""                                               },
{ "SETTINGS.MACHINE.8600"          , 0                        , ""                                               },
{ "SETTINGS.MACHINE.TYPERIGHT"     , 0                        , ""                                               },
{ "SETTINGS.SOUND.AHI"             , 0                        , ""                                               }, // MENUOPT_SOUND_AHI       140
{ "SETTINGS.SOUND.PAULA"           , 0                        , ""                                               },
{ "SETTINGS.SPEED.NTSC"            , 0                        , ""                                               },
{ "SETTINGS.SPEED.PAL"             , 0                        , ""                                               },
{ "SETTINGS.SPEED.HIGH"            , 0                        , ""                                               },
{ "SETTINGS.SPEED.NORMAL"          , 0                        , ""                                               },
{ "SETTINGS.SPEED.LOW"             , 0                        , ""                                               },
{ "SETTINGS.VDU.ELEKTERMINAL"      , 0                        , ""                                               },
{ "SETTINGS.VDU.NARROW"            , 0                        , ""                                               },
{ "SETTINGS.VDU.WIDE"              , 0                        , ""                                               },
{ "SETTINGS.VDU.RADIOBULLETIN"     , 0                        , ""                                               }, // MENUOPT_RADIOBULLETIN   150
{ "SETTINGS.VDU.SVT100"            , 0                        , ""                                               }, // MENUOPT_SVT100          151
{ "SETTINGS.VDU.VT100"             , 0                        , ""                                               }, // MENUOPT_VT100           152
{ "SETTINGS.VDU.ASCII"             , 0                        , ""                                               },
{ "SETTINGS.VDU.CHESSMEN"          , 0                        , ""                                               },
{ "SETTINGS.VDU.LOWERCASE"         , 0                        , ""                                               }, // MENUOPT_LOWERCASEVDU    155
// new ones
{ ""                               , MSG_MENUHELP_DRIVE_2     , "Change debugger drive to unit 2"                }, // MENUOPT_DRIVE_2                 156
{ ""                               , MSG_MENUHELP_DRIVE_3     , "Change debugger drive to unit 3"                }, // MENUOPT_DRIVE_3                 157
{ "SETTINGS.SPRITES.MULTIPLEX"     , 0                        , ""                                               }, // MENUOPT_DEMULTIPLEX_MULTIPLEX   158
{ "SETTINGS.SPRITES.TRANSPARENT"   , 0                        , ""                                               }, // MENUOPT_DEMULTIPLEX_TRANSPARENT 159
{ "SETTINGS.SPRITES.OPAQUE"        , 0                        , ""                                               }, // MENUOPT_DEMULTIPLEX_OPAQUE      160
{ "SETTINGS.EMULATOR.FRAMEBASED"   , 0                        , ""                                               }, // MENUOPT_FRAMEBASED              161
{ "SETTINGS.EMULATOR.PIXELBASED"   , 0                        , ""                                               }, // MENUOPT_PIXELBASED              162
{ ""                               , MSG_MENUHELP_N_4         , "Set notation to IEEE-694"                       }, // MENUOPT_N_4                     163
};

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT     FLAG                       d3d,
                                      modal,
                                      rmb;
IMPORT     TEXT                       autotext[GAMEINFOLINES][80 + 1],
                                      file_game[MAX_PATH + 1],
                                      fn_game[MAX_PATH + 1],
                                      fn_oldgame[MAX_PATH + 1],
                                      fn_script[MAX_PATH + 1],
                                      path_games[MAX_PATH + 1],
                                      recent[8][MAX_PATH + 1],
                                      userinput[MAX_PATH + 80 + 1];
IMPORT     UBYTE                      memory[32768],
                                      psu;
IMPORT     UWORD                      iar,
                                      ras[8];
IMPORT     ULONG                      analog,
                                      autofire[2],
                                      collisions,
                                      confirmable,
                                      demultiplex,
                                      keymap,
                                      paused,
                                      region,
                                      showpalladiumkeys2,
                                      sound,
                                      swapped,
                                      turbo,
                                      waittill;
IMPORT     int                        aifffile,
                                      ambient,
                                      apnganims,
                                      autocoin,
                                      autopause,
                                      autosave,
                                      base,
                                      binbug_biosver,
                                      binbug_dosver,
                                      blink,
                                      cd2650_biosver,
                                      cd2650_dosver,
                                      cd2650_vdu,
                                      colourset,
                                      confine,
                                      connected,
                                      coomer,
                                      cpuy,
                                      crippled,
                                      darkenbg,
                                      debugdrive,
                                      dejitter,
                                      drawcorners,
                                      drawmode,
                                      drawunlit,
                                      echoconsole,
                                      editscreen,
                                      enhancestars,
                                      elektor_biosver,
                                      emuid,
                                      erasedel,
                                      esvxfile,
                                      exactspeed,
                                      filter,
                                      flagline,
                                      framebased,
                                      fullscreen,
                                      game,
                                      generate,
                                      gifanims,
                                      gridy[2],
                                      guestrmb,
                                      hostcontroller[2],
                                      iffanims,
                                      inverse,
                                      limitrefreshes,
                                      logfile,
                                      logreadwrites,
                                      logbios,
                                      loginefficient,
                                      log_illegal,
                                      log_interrupts,
                                      logsubroutines,
                                      loop,
                                      lowercase,
                                      machine,
                                      memmap,
                                      midifile,
                                      mnganims,
                                      pausebreak,
                                      pauselog,
                                      phunsy_biosver,
                                      pipbug_biosver,
                                      pipbug_vdu,
                                      post,
                                      psgfile,
                                      queuekeystrokes,
                                      randomizememory,
                                      scanlines,
                                      rastn,
                                      recmode,
                                      requirebutton[2],
                                      retune,
                                      rotate,
                                      rotating,
                                      runtoframe,
                                      runtointerrupt,
                                      runtoloopend,
                                      selbst_biosver,
                                      sensegame,
                                      showbuiltin,
                                      showdebugger[2],
                                      showleds,
                                      showmenubars[2],
                                      showpointers[2],
                                      showpositions,
                                      showtitlebars[2],
                                      showtod,
                                      showtoolbars[2],
                                      showsidebars[2],
                                      showstatusbars[2],
                                      size,
                                      smusfile,
                                      sortby,
                                      speakable,
                                      springloaded,
                                      spritemode,
                                      startupupdates,
                                      stretching,
                                      style,
                                      supercpu,
                                      tapemode,
                                      timeunit,
                                      trace,
                                      trainer_invincibility,
                                      trainer_lives,
                                      trainer_time,
                                      twin_dosver,
                                      undither,
                                      usecsperframe[2],
                                      useff[2],
                                      useguideray,
                                      usemargins,
                                      usespeech,
                                      usestubs,
                                      verbosity,
                                      viewspeedas,
                                      warn,
                                      watchreads,
                                      watchwrites,
                                      wavfile,
                                      whichgame,
                                      wide,
                                      winleftx,
                                      wintopy,
                                      wsm,
                                      ymfile;
IMPORT       FILE                    *ANIMHandle,
                                     *GIFHandle,
                                     *MNGHandle,
                                     *PNGHandle;
IMPORT       struct DriveStruct       drive[DRIVES_MAX];
IMPORT       struct KnownStruct       known[KNOWNGAMES];
IMPORT       struct MachineStruct     machines[MACHINES];
IMPORT       struct MemMapInfoStruct  memmapinfo[MEMMAPS];
IMPORT       ASCREEN                  screen[BOXWIDTH][BOXHEIGHT];
IMPORT       MEMFLAG                  memflags[ALLTOKENS];
IMPORT const UBYTE                    cd2650_chars_defbmp[96][10],
                                      chessman[16][10];
#ifdef WIN32
    IMPORT       FLAG                 incli,
                                      opening,
                                      preserve,
                                      unit[2];
    IMPORT       int                  bezel,
                                      cheevos,
                                      language,
                                      priority,
                                      realwide,
                                      stretch43;
    IMPORT       HINSTANCE            InstancePtr;
    IMPORT       HMENU                MenuPtr;
    IMPORT       HWND                 hDebugger,
                                      hToolbar,
                                      MainWindowPtr,
                                      RichTextGadget,
                                      SubWindowPtr[SUBWINDOWS];
    IMPORT       CHARFORMAT2          fgformat;
#endif
#ifdef AMIGA
    IMPORT       FLAG                 broken_wi,
                                      gotallpens,
                                      guideexists,
                                      ng;
    IMPORT       ULONG                pending;
    IMPORT       int                  addedserver,
                                      icons,
                                      led,
                                      morphos,
                                      soundoutput,
                                      windowed_size,
                                      windowed_wide;
    IMPORT       Object*              images[IMAGES];
    IMPORT       struct Gadget*       gadgets[GIDS + 1];
    IMPORT       struct Menu*         MenuPtr;
    IMPORT       struct Node*         SmallSpeedBarNodePtr[SMALLGADGETS + 1];
    IMPORT       struct Library      *AmigaGuideBase,
                                     *OpenURLBase,
                                     *SocketBase;
    IMPORT       struct Window       *MainWindowPtr,
                                     *SubWindowPtr[SUBWINDOWS];
#endif

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void changecolours2(void);

// CODE-------------------------------------------------------------------

EXPORT void updatemenu(int which)
{   if (!MainWindowPtr)
    {   return; // for speed
    }

#ifdef AMIGA
    if (!MenuPtr)
    {   return; // for speed
    }
    // can't do this for IBM-PC because pop-up menu still needs updating
#endif

    switch (which)
    {
    // "Project" menu (mostly)
    case MENUITEM_RESET:
        ghost(which, crippled);
    acase MENUITEM_RESETTOMONITOR:
        ghost(which,
            machine != ELEKTOR
         && machine != PIPBUG
         && machine != BINBUG
         && machine != INSTRUCTOR
         && machine != CD2650
         && machine != PHUNSY
         && machine != SELBST
         && machine != MIKIT
        ); // eg. ARCADIA, INTERTON, coin-ops
    acase MENUITEM_REINITIALIZE:
    case MENUITEM_OPEN:
        ghost(which, crippled);
    acase MENUITEM_REVERT:
        ghost(which, crippled || !game);
    acase MENUITEM_QUICKLOAD:
        ghost(which, crippled);
    acase MENUITEM_SAVESNP:
        ghost(which, crippled);
    acase MENUITEM_SAVEASCII:
        ghost(which,
            machine != ARCADIA
         && machine != ELEKTOR
         && machine != PIPBUG
         && machine != BINBUG
         && machine != INSTRUCTOR
         && machine != TWIN
         && machine != CD2650
         && machine != PHUNSY
         && machine != SELBST
         && machine != MIKIT
         && machine != TYPERIGHT
         && machine != PONG
         && !machines[machine].coinop
        ); // eg. INTERTON
    acase MENUITEM_SAVEASM:
        ghost(which, machine != ARCADIA && machine != INTERTON && machine != ELEKTOR);
    acase MENUITEM_QUICKSAVE:
        ghost(which, crippled);
    acase MENUITEM_SERVER:
        ghost(which,
            (   machine != ARCADIA
             && machine != INTERTON
             && machine != ELEKTOR
            )
         || crippled
#ifdef AMIGA
         || !SocketBase
#endif
        );
        tick(which, connected == NET_SERVER);
    acase MENUITEM_CLIENT:
        ghost(which,
            (   machine != ARCADIA
             && machine != INTERTON
             && machine != ELEKTOR
            )
         || crippled
#ifdef AMIGA
         || !SocketBase
#endif
        );
        tick(which, connected == NET_CLIENT);
#ifdef AMIGA
    acase MENUITEM_ICONIFY:
        ghost(which, wsm != 0);
#endif
    // "Edit" menu
    acase MENUITEM_COPYTEXT:
        ghost(which,
            machine != ARCADIA
         && machine != ELEKTOR
         && machine != PIPBUG
         && machine != BINBUG
         && machine != INSTRUCTOR
         && machine != TWIN
         && machine != CD2650
         && machine != PHUNSY
         && machine != SELBST
         && machine != MIKIT
         && machine != TYPERIGHT
         && machine != PONG
         && !machines[machine].coinop
        ); // eg. INTERTON
    acase MENUITEM_PASTETEXT:
        ghost(which,
            (machine != PIPBUG
          && machine != BINBUG
          && machine != TWIN
          && machine != CD2650
          && machine != PHUNSY
          && machine != SELBST
          && machine != TYPERIGHT
            )
         || recmode == RECMODE_PLAY
        ); // eg. ARCADIA, INTERTON, ELEKTOR, INSTRUCTOR, SELBST, MIKIT, PONG, coin-ops
    // "View" menu
    acase MENUITEM_VIEWHISCORES:
        ghost(which, !machines[machine].hiscores); // eg. INTERTON, ELEKTOR, PIPBUG, INSTRUCTOR, CD2650, PHUNSY, SELBST, MIKIT
    acase MENUITEM_VIEWDEBUGGER:
#ifdef WIN32
        ghost(which, (cheevos && RA_HardcoreModeIsActive()));
#endif
#ifdef AMIGA
        ghost(which, fullscreen);
#endif
        tick(which, showdebugger[wsm]);
#ifdef WIN32
    acase MENUITEM_MENUBAR:
        tick(which, showmenubars[wsm]);
#endif
    acase MENUITEM_POINTER:
        tick(which, showpointers[wsm]);
    acase MENUITEM_TITLEBAR:
#ifdef AMIGA
        ghost(which, fullscreen);
#endif
        tick(which, showtitlebars[wsm]);
    acase MENUITEM_TOOLBAR:
#ifdef AMIGA
        ghost(which, fullscreen);
#endif
        tick(which, showtoolbars[wsm]);
    acase MENUITEM_SIDEBAR:
#ifdef AMIGA
        ghost(which, fullscreen);
#endif
        tick(which, showsidebars[wsm]);
    acase MENUITEM_STATUSBAR:
#ifdef AMIGA
        ghost(which, fullscreen);
#endif
        tick(which, showstatusbars[wsm]);
    acase MENUITEM_BUILTIN:
        ghost(which, fullscreen);
        tick(which, showbuiltin);
    acase MENUFAKE_SORTBY:
        if (sortby == SORTBYNAME)
        {     checkmx(MENUOPT_SORTBYNAME,    MENUOPT_SORTBYNAME, MENUOPT_SORTBYMACHINE);
            uncheckmx(MENUOPT_SORTBYMACHINE);
        } else
        {   // assert(sortby == SORTBYMACHINE);
            uncheckmx(MENUOPT_SORTBYNAME);
              checkmx(MENUOPT_SORTBYMACHINE, MENUOPT_SORTBYNAME, MENUOPT_SORTBYMACHINE);
        }
    acase MENUFAKE_SPEEDINDICATOR:
        switch (viewspeedas)
        {
        case VIEWAS_FPS:
            uncheckmx(MENUOPT_INDICATOR_KHZ);
            uncheckmx(MENUOPT_INDICATOR_PERCENT);
              checkmx(MENUOPT_INDICATOR_FPS,     MENUOPT_INDICATOR_FPS, MENUOPT_INDICATOR_PERCENT);
        acase VIEWAS_KHZ:
            uncheckmx(MENUOPT_INDICATOR_FPS);
            uncheckmx(MENUOPT_INDICATOR_PERCENT);
              checkmx(MENUOPT_INDICATOR_KHZ,     MENUOPT_INDICATOR_FPS, MENUOPT_INDICATOR_PERCENT);
        acase VIEWAS_PERCENTS:
            uncheckmx(MENUOPT_INDICATOR_FPS);
            uncheckmx(MENUOPT_INDICATOR_KHZ);
              checkmx(MENUOPT_INDICATOR_PERCENT, MENUOPT_INDICATOR_FPS, MENUOPT_INDICATOR_PERCENT);
        }
    // "Macros" menu
    acase MENUITEM_STARTRECORDING:
        ghost(MENUITEM_STARTRECORDING , crippled || recmode == RECMODE_RECORD);
    acase MENUITEM_RESTARTPLAYBACK:
        ghost(MENUITEM_RESTARTPLAYBACK, crippled || recmode != RECMODE_PLAY  );
    acase MENUITEM_STOP:
        ghost(MENUITEM_STOP           , crippled || recmode == RECMODE_NORMAL);
    acase MENUITEM_LOOP:
        tick(which, loop);
    acase MENUITEM_APNG:
        ghost(which, recmode != RECMODE_NORMAL);
        tick(which, apnganims);
    acase MENUITEM_MNG:
        ghost(which, recmode != RECMODE_NORMAL);
        tick(which, mnganims);
    acase MENUITEM_IFFANIMS:
        ghost(which, recmode != RECMODE_NORMAL);
        tick(which, iffanims);
    acase MENUITEM_GIFANIMS:
        ghost(which, recmode != RECMODE_NORMAL);
        tick(which, gifanims);
    acase MENUITEM_GENERATE8SVX:
        ghost(which, recmode != RECMODE_NORMAL || memmapinfo[memmap].channels == 0);
        tick(which, esvxfile);
    acase MENUITEM_GENERATEAIFF:
        ghost(which, recmode != RECMODE_NORMAL || memmapinfo[memmap].channels == 0);
        tick(which, aifffile);
    acase MENUITEM_GENERATEMIDI:
        ghost(which, recmode != RECMODE_NORMAL || memmapinfo[memmap].channels == 0);
        tick(which, midifile);
    acase MENUITEM_GENERATEPSG:
        ghost(which, recmode != RECMODE_NORMAL || memmap != MEMMAP_F);
        tick(which, psgfile);
    acase MENUITEM_GENERATESMUS:
        ghost(which, recmode != RECMODE_NORMAL || memmapinfo[memmap].channels == 0);
        tick(which, smusfile);
    acase MENUITEM_GENERATEWAV:
        ghost(which, recmode != RECMODE_NORMAL || memmapinfo[memmap].channels == 0);
        tick(which, wavfile);
    acase MENUITEM_GENERATEYM:
        ghost(which, recmode != RECMODE_NORMAL || memmap != MEMMAP_F);
        tick(which, ymfile);
    acase MENUITEM_GENERATE:
        ghost(which, recmode != RECMODE_NORMAL);
        tick(which, generate);
#ifdef WIN32
    acase MENUITEM_RUNREXX:
        ghost(which, (cheevos && RA_HardcoreModeIsActive()));
#endif
    acase MENUITEM_REPEATREXX:
        ghost(which,
            fn_script[0] == EOS
#ifdef WIN32
         || (cheevos && RA_HardcoreModeIsActive())
#endif
        );
    // "Debug|General �" submenu
    acase MENUITEM_QUOTES:
        ghost(which, !(connected == NET_SERVER || connected == NET_CLIENT));
    acase MENUITEM_CLEARCOV:
        ghost(which, !allowable(FALSE));
    acase MENUITEM_SAY:
        ghost(which, !(sound && speakable));
    // "Debug|File �" submenu
    acase MENUITEM_ASM:
    case MENUITEM_DISGAME:
    case MENUITEM_LOADBIN:
    case MENUITEM_SAVEAOF:
    case MENUITEM_SAVEBIN:
    case MENUITEM_SAVEBPNF:
    case MENUITEM_SAVEHEX:
    case MENUITEM_SAVESMS:
        ghost(which, !allowable(FALSE));
    acase MENUITEM_SAVECMD:\
        ghost(which, !HASDISK);
    acase MENUITEM_SAVETVC:
        ghost(which, machine != ELEKTOR);
    // "Debug|Disk �" submenu
    acase MENUITEM_DELETE:
    case  MENUITEM_DIR:
    case  MENUITEM_EXTRACT:
    case  MENUITEM_INJECT:
    case  MENUITEM_RENAME:
        ghost(which, !HASDISK);
    acase MENUFAKE_DRIVE:
        enable2(MENUOPT_DRIVE_0, machines[machine].drives >= 1);
        enable2(MENUOPT_DRIVE_1, machines[machine].drives >= 2);
        enable2(MENUOPT_DRIVE_2, machines[machine].drives >= 3);
        enable2(MENUOPT_DRIVE_3, machines[machine].drives >= 4);
        switch (debugdrive)
        {
        case 0:
              checkmx(MENUOPT_DRIVE_0, MENUOPT_DRIVE_0, MENUOPT_DRIVE_3);
            uncheckmx(MENUOPT_DRIVE_1);
            uncheckmx(MENUOPT_DRIVE_2);
            uncheckmx(MENUOPT_DRIVE_3);
        acase 1:
            uncheckmx(MENUOPT_DRIVE_0);
              checkmx(MENUOPT_DRIVE_1, MENUOPT_DRIVE_0, MENUOPT_DRIVE_3);
            uncheckmx(MENUOPT_DRIVE_2);
            uncheckmx(MENUOPT_DRIVE_3);
        acase 2:
            uncheckmx(MENUOPT_DRIVE_0);
            uncheckmx(MENUOPT_DRIVE_1);
              checkmx(MENUOPT_DRIVE_2, MENUOPT_DRIVE_0, MENUOPT_DRIVE_3);
            uncheckmx(MENUOPT_DRIVE_3);
        acase 3:
            uncheckmx(MENUOPT_DRIVE_0);
            uncheckmx(MENUOPT_DRIVE_1);
            uncheckmx(MENUOPT_DRIVE_2);
              checkmx(MENUOPT_DRIVE_3, MENUOPT_DRIVE_0, MENUOPT_DRIVE_3);
        }
    // "Debug|Edit �" submenu
    acase MENUITEM_DOKE:
    case MENUITEM_POKE:
    case MENUITEM_WRITEPORT:
        ghost(which, !allowable(FALSE));
    acase MENUITEM_FPOKE:
        ghost(which, (!HASDISK || !drive[debugdrive].inserted));
    // "Debug|View �" submenu
    acase MENUITEM_EQUALS:
    case MENUITEM_COVER:
    case MENUITEM_PEEK:
    case MENUITEM_DIS:
        ghost(which, !allowable(FALSE));
    acase MENUITEM_ERROR:
        ghost(which, machine != TWIN);
    acase MENUITEM_FPEEK:
        ghost(which, (!HASDISK || !drive[debugdrive].inserted));
    acase MENUITEM_IM:
    case MENUITEM_READPORT:
        ghost(which, !allowable(FALSE));
    acase MENUITEM_VIEW_BASIC:
        ghost(which, machine != PIPBUG && machine != CD2650);
    acase MENUITEM_VIEW_BIOS:
        ghost(which,
            machine != ELEKTOR
         && machine != PIPBUG
         && (machine != TWIN || twin_dosver == TWIN_NODOS)
         && machine != INSTRUCTOR
         && machine != CD2650
         && machine != PHUNSY
         && machine != SELBST
        );
    acase MENUITEM_VIEW_CPU:
        ghost(which, !allowable(FALSE));
    acase MENUITEM_VIEW_PSG:
        ghost(which, !(memmap == MEMMAP_F || memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN));
    acase MENUITEM_VIEW_RAM:
        ghost(which, !allowable(FALSE));
    acase MENUITEM_VIEW_SCRN:
        ghost(which, !(
            machine == ARCADIA
         || machines[machine].pvis
         || machine == PIPBUG
         || machine == BINBUG
         || machine == INSTRUCTOR
         || machine == TWIN
         || machine == CD2650
         || machine == PHUNSY
         || machine == SELBST
         || machine == MIKIT
         || machine == TYPERIGHT
        ));
    acase MENUITEM_VIEW_UDG:
        ghost(which, !(machine == ARCADIA || machines[machine].pvis || machine == BINBUG));
    acase MENUITEM_VIEW_XVI:
        ghost(which, !(machine == ARCADIA || machines[machine].pvis));
    // "Debug|Log �" submenu
    acase MENUITEM_L_A:
        ghost(which, !allowable(FALSE));
        tick(which, logreadwrites);
    acase MENUITEM_L_B:     
        ghost(which, !(
            machine == ELEKTOR
         || machine == PIPBUG
         || machine == BINBUG
         || machine == INSTRUCTOR
         || machine == TWIN
         || machine == CD2650
         || machine == PHUNSY
         || machine == SELBST
        ));
        tick(which, logbios);
    acase MENUITEM_L_C:
        ghost(which, !allowable(FALSE));
        tick(which, loginefficient);
    acase MENUITEM_L_I:
        ghost(which, !allowable(FALSE));
        tick(which, log_illegal);
    acase MENUITEM_L_N:
        ghost(which, !allowable(FALSE));
        tick(which, log_interrupts);
    acase MENUITEM_L_S:
        ghost(which, !allowable(FALSE));
        tick(which, logsubroutines);
    acase MENUITEM_PL:
        ghost(which, !allowable(FALSE));
        tick( which, pauselog);
    acase MENUITEM_T:
        ghost(which, !allowable(FALSE));
        tick(which, trace);
    // "Debug|Run �" submenu
    acase MENUITEM_GI:
        ghost(which, (machines[machine].pvis == 0 && machine != INSTRUCTOR && machine != PHUNSY));
    acase MENUITEM_I:
        ghost(which, !allowable(FALSE));
    acase MENUITEM_JUMP:
        ghost(which, !allowable(FALSE));
    acase MENUITEM_O:
    case MENUITEM_S:
        ghost(which, !allowable(FALSE));
    acase MENUITEM_R: // run to rastline <n>
        tick(which, (rastn != INVALIDRASTER));
        ghost(which, !(
            machine == ARCADIA
         || machines[machine].pvis
         || machine == BINBUG
         || machine == CD2650
         || machine == PHUNSY
        ));
    acase MENUITEM_R_F:
        tick(which, runtoframe);
    acase MENUITEM_R_I:
        tick(which, runtointerrupt);
        ghost(which, !(
            machines[machine].pvis
         || machine == INSTRUCTOR
         || machine == TWIN
         || machine == PHUNSY
         || machine == BINBUG
        ));
    acase MENUITEM_R_L:
        ghost(which, !allowable(FALSE));
        tick(which, runtoloopend);
    acase MENUITEM_R_R: // run to next rastline
        tick(which, (rastn == cpuy + 1));
        ghost(which, !(
            machine == ARCADIA
         || machines[machine].pvis
         || machine == BINBUG
         || machine == CD2650
         || machine == PHUNSY
        ));
    acase MENUITEM_R_S:
        ghost(which, !allowable(FALSE));
        tick(which, (   (((psu & PSU_SP) == 0) && (memflags[ras[                 7]] & BREAKPOINT)) 
                     || (((psu & PSU_SP) != 0) && (memflags[ras[(psu & PSU_SP) - 1]] & BREAKPOINT)) 
            )       );
    // "Debug|Breakpoints �" submenu
    acase MENUITEM_BC:
    case MENUITEM_BL:
    case MENUITEM_BP:
        ghost(which, !allowable(FALSE));
    acase MENUITEM_FC:
    case MENUITEM_FP:
    case MENUITEM_FL:
        ghost(which, !HASDISK);
    acase MENUITEM_IC:
    case MENUITEM_IL:
    case MENUITEM_IP:
    case MENUITEM_WC:
    case MENUITEM_WL:
    case MENUITEM_WP:
        ghost(which, !allowable(FALSE));
    acase MENUITEM_PB:
        ghost(which, !allowable(FALSE));
        tick( which, pausebreak);
    acase MENUITEM_WR:
        ghost(which, !allowable(FALSE));
        tick( which, watchreads);
    acase MENUFAKE_WW:
        enable2(MENUOPT_WW_NONE, allowable(FALSE));
        enable2(MENUOPT_WW_SOME, allowable(FALSE));
        enable2(MENUOPT_WW_ALL,  allowable(FALSE));
        switch (watchwrites)
        {
        case WATCH_NONE:
              checkmx(MENUOPT_WW_NONE, MENUOPT_WW_NONE, MENUOPT_WW_ALL);
            uncheckmx(MENUOPT_WW_SOME);
            uncheckmx(MENUOPT_WW_ALL);
        acase WATCH_SOME:
            uncheckmx(MENUOPT_WW_NONE);
              checkmx(MENUOPT_WW_SOME, MENUOPT_WW_NONE, MENUOPT_WW_ALL);
            uncheckmx(MENUOPT_WW_ALL);
        acase WATCH_ALL:
            uncheckmx(MENUOPT_WW_NONE);
            uncheckmx(MENUOPT_WW_SOME);
              checkmx(MENUOPT_WW_ALL,  MENUOPT_WW_NONE, MENUOPT_WW_ALL);
        }
    // "Debug|Symbols �" submenu
    acase MENUITEM_CLEARSYM:
    case MENUITEM_DEFSYM:
    case MENUITEM_LOADSYM:
    case MENUITEM_SAVESYM:
        ghost(which, !allowable(FALSE));
    // "Debug|Tools �" submenu
    acase MENUITEM_COMP:
    case MENUITEM_COPY:
    case MENUITEM_FILL:
    case MENUITEM_FIND:
    case MENUITEM_REL:
    case MENUITEM_SWAP:
    case MENUITEM_TRAIN:
        ghost(which, !allowable(FALSE));
    // "Debug|Options �" submenu
    acase MENUFAKE_BASE:
        enable2(MENUOPT_BASE_BINARY,  allowable(FALSE));
        enable2(MENUOPT_BASE_OCTAL,   allowable(FALSE));
        enable2(MENUOPT_BASE_DECIMAL, allowable(FALSE));
        enable2(MENUOPT_BASE_HEX,     allowable(FALSE));
        switch (base)
        {
        case  0: // binary
              checkmx(MENUOPT_BASE_BINARY,  MENUOPT_BASE_BINARY, MENUOPT_BASE_HEX);
            uncheckmx(MENUOPT_BASE_OCTAL);
            uncheckmx(MENUOPT_BASE_DECIMAL);
            uncheckmx(MENUOPT_BASE_HEX);
        acase 1: // octal
            uncheckmx(MENUOPT_BASE_BINARY);
              checkmx(MENUOPT_BASE_OCTAL,   MENUOPT_BASE_BINARY, MENUOPT_BASE_HEX);
            uncheckmx(MENUOPT_BASE_DECIMAL);
            uncheckmx(MENUOPT_BASE_HEX);
        acase 2: // decimal
            uncheckmx(MENUOPT_BASE_BINARY);
            uncheckmx(MENUOPT_BASE_OCTAL);
              checkmx(MENUOPT_BASE_DECIMAL, MENUOPT_BASE_BINARY, MENUOPT_BASE_HEX);
            uncheckmx(MENUOPT_BASE_HEX);
        acase 3: // hex
            uncheckmx(MENUOPT_BASE_BINARY);
            uncheckmx(MENUOPT_BASE_OCTAL);
            uncheckmx(MENUOPT_BASE_DECIMAL);
              checkmx(MENUOPT_BASE_HEX,     MENUOPT_BASE_BINARY, MENUOPT_BASE_HEX);
        }
    acase MENUFAKE_CPU:
        enable2(MENUOPT_CPU_0, machine != PONG && machine != TYPERIGHT);
        enable2(MENUOPT_CPU_1, machine != PONG && machine != TYPERIGHT);
        if (supercpu == 0)
        {     checkmx(MENUOPT_CPU_0, MENUOPT_CPU_0, MENUOPT_CPU_1);
            uncheckmx(MENUOPT_CPU_1);
        } else
        {   uncheckmx(MENUOPT_CPU_0);
              checkmx(MENUOPT_CPU_1, MENUOPT_CPU_0, MENUOPT_CPU_1);
        }
    acase MENUFAKE_N:
        enable2(MENUOPT_N_0, allowable(FALSE));
        enable2(MENUOPT_N_1, allowable(FALSE));
        enable2(MENUOPT_N_2, allowable(FALSE));
        enable2(MENUOPT_N_3, allowable(FALSE));
        enable2(MENUOPT_N_4, allowable(FALSE));
        switch (style)
        {
        case STYLE_SIGNETICS1:
              checkmx(MENUOPT_N_0, MENUOPT_N_0, MENUOPT_N_4);
            uncheckmx(MENUOPT_N_1);
            uncheckmx(MENUOPT_N_2);
            uncheckmx(MENUOPT_N_3);
            uncheckmx(MENUOPT_N_4);
        acase STYLE_SIGNETICS2:
            uncheckmx(MENUOPT_N_0);
              checkmx(MENUOPT_N_1, MENUOPT_N_0, MENUOPT_N_4);
            uncheckmx(MENUOPT_N_2);
            uncheckmx(MENUOPT_N_3);
            uncheckmx(MENUOPT_N_4);
        acase STYLE_OLDCALM:
            uncheckmx(MENUOPT_N_0);
            uncheckmx(MENUOPT_N_1);
              checkmx(MENUOPT_N_2, MENUOPT_N_0, MENUOPT_N_4);
            uncheckmx(MENUOPT_N_3);
            uncheckmx(MENUOPT_N_4);
        acase STYLE_NEWCALM:
            uncheckmx(MENUOPT_N_0);
            uncheckmx(MENUOPT_N_1);
            uncheckmx(MENUOPT_N_2);
              checkmx(MENUOPT_N_3, MENUOPT_N_0, MENUOPT_N_4);
            uncheckmx(MENUOPT_N_4);
        acase STYLE_IEEE:
            uncheckmx(MENUOPT_N_0);
            uncheckmx(MENUOPT_N_1);
            uncheckmx(MENUOPT_N_2);
            uncheckmx(MENUOPT_N_3);
              checkmx(MENUOPT_N_4, MENUOPT_N_0, MENUOPT_N_4);
        }
    acase MENUFAKE_TU:
        enable2(MENUOPT_TU_0, allowable(FALSE));
        enable2(MENUOPT_TU_1, allowable(FALSE));
        enable2(MENUOPT_TU_2, allowable(FALSE));
        switch (timeunit)
        {
        case TIMEUNIT_CYCLES:
              checkmx(MENUOPT_TU_0, MENUOPT_TU_0, MENUOPT_TU_2);
            uncheckmx(MENUOPT_TU_1);
            uncheckmx(MENUOPT_TU_2);
        acase TIMEUNIT_CLOCKS:
            uncheckmx(MENUOPT_TU_0);
              checkmx(MENUOPT_TU_1, MENUOPT_TU_0, MENUOPT_TU_2);
            uncheckmx(MENUOPT_TU_2);
        acase TIMEUNIT_PIXELS:
            uncheckmx(MENUOPT_TU_0);
            uncheckmx(MENUOPT_TU_1);
              checkmx(MENUOPT_TU_2, MENUOPT_TU_0, MENUOPT_TU_2);
        }
    acase MENUFAKE_VERBOSITY:
        enable2(MENUOPT_VB_0, allowable(FALSE));
        enable2(MENUOPT_VB_1, allowable(FALSE));
        enable2(MENUOPT_VB_2, allowable(FALSE));
        switch (verbosity)
        {
        case  0:
              checkmx(MENUOPT_VB_0, MENUOPT_VB_0, MENUOPT_VB_2);
            uncheckmx(MENUOPT_VB_1);
            uncheckmx(MENUOPT_VB_2);
        acase 1:
            uncheckmx(MENUOPT_VB_0);
              checkmx(MENUOPT_VB_1, MENUOPT_VB_0, MENUOPT_VB_2);
            uncheckmx(MENUOPT_VB_2);
        acase 2:
            uncheckmx(MENUOPT_VB_0);
            uncheckmx(MENUOPT_VB_1);
              checkmx(MENUOPT_VB_2, MENUOPT_VB_0, MENUOPT_VB_2);
        }
    acase MENUITEM_GR:
        ghost(which, !(
            machine == ARCADIA
         || machines[machine].pvis
         || machine == BINBUG
         || machine == CD2650
         || machine == PHUNSY
        ));
        tick(which, useguideray);
    acase MENUITEM_WARN:
        ghost(which, machine == PONG || machine == TYPERIGHT);
        tick(which, warn);
    // "Debug|Graphics �" submenu
    acase MENUFAKE_DRAW:
        enable2(MENUOPT_DRAW_0, machine != INSTRUCTOR && machine != MIKIT    && machine != PONG    && machine != TYPERIGHT);
        enable2(MENUOPT_DRAW_1, machine != INSTRUCTOR && machine != MIKIT    && machine != PONG    && machine != TYPERIGHT);
        enable2(MENUOPT_DRAW_2, machine == ARCADIA    || machine == INTERTON || machine == ELEKTOR || machine == MALZAK   );
        enable2(MENUOPT_DRAW_3, machine == ARCADIA    || machine == INTERTON || machine == ELEKTOR || machine == MALZAK   );
        enable2(MENUOPT_DRAW_4,                          machine == INTERTON || machine == ELEKTOR                        );
        switch (drawmode)
        {
        case  0:
              checkmx(MENUOPT_DRAW_0, MENUOPT_DRAW_0, MENUOPT_DRAW_4);
            uncheckmx(MENUOPT_DRAW_1);
            uncheckmx(MENUOPT_DRAW_2);
            uncheckmx(MENUOPT_DRAW_3);
            uncheckmx(MENUOPT_DRAW_4);
        acase 1:
            uncheckmx(MENUOPT_DRAW_0);
              checkmx(MENUOPT_DRAW_1, MENUOPT_DRAW_0, MENUOPT_DRAW_4);
            uncheckmx(MENUOPT_DRAW_2);
            uncheckmx(MENUOPT_DRAW_3);
            uncheckmx(MENUOPT_DRAW_4);
        acase 2:
            uncheckmx(MENUOPT_DRAW_0);
            uncheckmx(MENUOPT_DRAW_1);
              checkmx(MENUOPT_DRAW_2, MENUOPT_DRAW_0, MENUOPT_DRAW_4);
            uncheckmx(MENUOPT_DRAW_3);
            uncheckmx(MENUOPT_DRAW_4);
        acase 3:
            uncheckmx(MENUOPT_DRAW_0);
            uncheckmx(MENUOPT_DRAW_1);
            uncheckmx(MENUOPT_DRAW_2);
              checkmx(MENUOPT_DRAW_3, MENUOPT_DRAW_0, MENUOPT_DRAW_4);
            uncheckmx(MENUOPT_DRAW_4);
        acase 4:
            uncheckmx(MENUOPT_DRAW_0);
            uncheckmx(MENUOPT_DRAW_1);
            uncheckmx(MENUOPT_DRAW_2);
            uncheckmx(MENUOPT_DRAW_3);
              checkmx(MENUOPT_DRAW_4, MENUOPT_DRAW_0, MENUOPT_DRAW_4);
        }
    acase MENUFAKE_SPR:
        enable2(MENUOPT_SPR_0, !(machine != ARCADIA && machines[machine].pvis == 0 && machine != PONG));
        enable2(MENUOPT_SPR_1, !(machine != ARCADIA && machines[machine].pvis == 0 && machine != PONG));
        enable2(MENUOPT_SPR_2, !(machine != ARCADIA && machines[machine].pvis == 0 && machine != PONG));
        switch (spritemode)
        {
        case SPRITEMODE_INVISIBLE:
              checkmx(MENUOPT_SPR_0, MENUOPT_SPR_0, MENUOPT_SPR_2);
            uncheckmx(MENUOPT_SPR_1);
            uncheckmx(MENUOPT_SPR_2);
        acase SPRITEMODE_VISIBLE:
            uncheckmx(MENUOPT_SPR_0);
              checkmx(MENUOPT_SPR_1, MENUOPT_SPR_0, MENUOPT_SPR_2);
            uncheckmx(MENUOPT_SPR_2);
        acase SPRITEMODE_NUMBERED:
            uncheckmx(MENUOPT_SPR_0);
            uncheckmx(MENUOPT_SPR_1);
              checkmx(MENUOPT_SPR_2, MENUOPT_SPR_0, MENUOPT_SPR_2);
        }
    // "Tools" menu
    acase MENUITEM_CONTROLS:
        ghost(which,
            (   machine != ARCADIA
             && machine != INTERTON
             && machine != ELEKTOR
             && machine != PIPBUG
             && machine != BINBUG
             && machine != INSTRUCTOR
             && machine != TWIN
             && machine != ZACCARIA
             && machine != MALZAK
             && machine != PHUNSY
             && machine != PONG
             && machine != SELBST
             && machine != MIKIT
             && machine != TYPERIGHT
            ) // eg. CD2650
#ifdef AMIGA
         || SubWindowPtr[SUBWINDOW_CONTROLS]
#endif
        );
#ifdef WIN32
        tick(which, SubWindowPtr[SUBWINDOW_CONTROLS] != NULL);
#endif
    acase MENUITEM_DIPSWITCHES:
        ghost(which,
            (   memmap  != MEMMAP_MALZAK2
             && machine != INSTRUCTOR
             && machine != ZACCARIA
             && machine != PONG
            ) // eg. ARCADIA, INTERTON, ELEKTOR, PIPBUG, BINBUG, CD2650, Malzak 1, SELBST, MIKIT, TYPERIGHT
#ifdef AMIGA
         || SubWindowPtr[SUBWINDOW_DIPS]
#endif
        );
#ifdef WIN32
        tick(which, SubWindowPtr[SUBWINDOW_DIPS] != NULL);
#endif
    acase MENUITEM_FLOPPYDRIVE:
#ifdef AMIGA
        ghost(which, (machine != TWIN && machine != BINBUG && machine != CD2650) || SubWindowPtr[SUBWINDOW_FLOPPYDRIVE]);
#endif
#ifdef WIN32
        ghost(which,  machine != TWIN && machine != BINBUG && machine != CD2650);
        tick(which, SubWindowPtr[SUBWINDOW_FLOPPYDRIVE] != NULL);
#endif
    acase MENUITEM_EDITMEMORY:
#ifdef AMIGA
        ghost(which, machine == PONG || machine == TYPERIGHT || SubWindowPtr[SUBWINDOW_MEMORY]);
#endif
#ifdef WIN32
        ghost(which, machine == PONG || machine == TYPERIGHT || (cheevos && RA_HardcoreModeIsActive()));
        tick(which, SubWindowPtr[SUBWINDOW_MEMORY] != NULL);
#endif
    acase MENUITEM_MUSIC:
        ghost
        (   which,
            (   machine != ARCADIA
             && machines[machine].pvis == 0
             && whichgame != SI50_THEMEPOS
             && machine != PONG
             && machine != TYPERIGHT
            )
#ifdef AMIGA
         || SubWindowPtr[SUBWINDOW_MUSIC]
#endif
        );
#ifdef WIN32
        tick(which, SubWindowPtr[SUBWINDOW_MUSIC] != NULL);
#endif
    acase MENUITEM_PALETTE:
#ifdef AMIGA
        ghost(which, !gotallpens || SubWindowPtr[SUBWINDOW_PALETTE] != NULL);
#endif
#ifdef WIN32
        tick(which, SubWindowPtr[SUBWINDOW_PALETTE] != NULL);
#endif
    acase MENUITEM_PRINTER:
#ifdef AMIGA
        ghost(which, (machine != PIPBUG && machine != BINBUG && machine != TWIN && machine != CD2650) || SubWindowPtr[SUBWINDOW_PRINTER] != NULL);
#endif
#ifdef WIN32
        ghost(which, (machine != PIPBUG && machine != BINBUG && machine != TWIN && machine != CD2650));
        tick(which, SubWindowPtr[SUBWINDOW_PRINTER] != NULL);
#endif
    acase MENUITEM_SPRITEVIEWER:
        ghost
        (   which,
            (machine != ARCADIA && machines[machine].pvis == 0 && machine != BINBUG && machine != CD2650 && machine != PHUNSY)
        // eg. PIPBUG, INSTRUCTOR, TWIN, SELBST, MIKIT, PONG, TYPERIGHT
#ifdef AMIGA
         || SubWindowPtr[SUBWINDOW_SPRITES] != NULL
#endif
#ifdef WIN32
         || (cheevos && RA_HardcoreModeIsActive())
#endif
        );
#ifdef WIN32
        tick(which, SubWindowPtr[SUBWINDOW_SPRITES] != NULL);
#endif
    acase MENUITEM_TAPEDECK:
#ifdef AMIGA
        ghost(which, (!TAPABLE) || SubWindowPtr[SUBWINDOW_TAPEDECK]);
#endif
#ifdef WIN32
        ghost(which, (!TAPABLE) || (cheevos && RA_HardcoreModeIsActive()));
        tick(which, SubWindowPtr[SUBWINDOW_TAPEDECK] != NULL);
#endif
    acase MENUITEM_INDUSTRIAL:
#ifdef AMIGA
        ghost(which, machine != PIPBUG || SubWindowPtr[SUBWINDOW_INDUSTRIAL]);
#endif
#ifdef WIN32
        ghost(which, machine != PIPBUG);
        tick(which, SubWindowPtr[SUBWINDOW_INDUSTRIAL] != NULL);
#endif
    acase MENUITEM_PAPERTAPE:
#ifdef AMIGA
        ghost(which, !HASPAPERTAPE || SubWindowPtr[SUBWINDOW_PAPERTAPE]);
#endif
#ifdef WIN32
        ghost(which, !HASPAPERTAPE);
        tick(which, SubWindowPtr[SUBWINDOW_PAPERTAPE] != NULL);
#endif
    acase MENUITEM_TOOLSDEBUGGER:
        ghost(which, !showdebugger[wsm]);
#ifdef WIN32
        tick(which, incli);
#endif
    acase MENUITEM_SCREENEDITOR:
        ghost(which,
#ifdef WIN32
            (cheevos && RA_HardcoreModeIsActive())
         ||
#endif
            (   machine != ARCADIA
             && machine != INTERTON
             && machine != ELEKTOR
             && machine != BINBUG
             && machine != CD2650
             && machine != PHUNSY
             && machine != ZACCARIA
        )   );
        tick(which, editscreen);
    acase MENUITEM_VIEWMONITOR_CPU:
#ifdef AMIGA
        ghost(which, machine == PONG || SubWindowPtr[SUBWINDOW_MONITOR_CPU] != NULL);
#endif
#ifdef WIN32
        ghost(which, machine == PONG || (cheevos && RA_HardcoreModeIsActive()));
        tick(which, SubWindowPtr[SUBWINDOW_MONITOR_CPU] != NULL);
#endif
    acase MENUITEM_VIEWMONITOR_PSGS:
#ifdef AMIGA
        ghost(which, memmap != MEMMAP_F || SubWindowPtr[SUBWINDOW_MONITOR_PSGS]);
#endif
#ifdef WIN32
        ghost(which, memmap != MEMMAP_F || (cheevos && RA_HardcoreModeIsActive()));
        tick(which, SubWindowPtr[SUBWINDOW_MONITOR_PSGS] != NULL);
#endif
    acase MENUITEM_VIEWMONITOR_XVI:
        ghost(which,
            (machine != ARCADIA && !machines[machine].pvis && machine != PONG && machine != BINBUG)
#ifdef AMIGA
         || SubWindowPtr[SUBWINDOW_MONITOR_XVI]
#endif
#ifdef WIN32
         || (cheevos && RA_HardcoreModeIsActive())
#endif
        );
#ifdef WIN32
        tick(which, SubWindowPtr[SUBWINDOW_MONITOR_XVI] != NULL);
#endif
    // "Settings" menu
    acase MENUFAKE_LEFT:
        enable2(MENUOPT_LEFT_TRACKBALL, machine == ARCADIA || machines[machine].pvis || machine == PONG || machine == BINBUG);
        enable2(MENUOPT_LEFT_NONE,      machine == ARCADIA || machines[machine].pvis || machine == PONG || machine == BINBUG);
        enable2(MENUOPT_LEFT_1STDJOY,  (machine == ARCADIA || machines[machine].pvis || machine == PONG || machine == BINBUG)
#ifdef WIN32
        && unit[0]
#endif
        );
        enable2(MENUOPT_LEFT_2NDDJOY,  (machine == ARCADIA || machines[machine].pvis || machine == PONG || machine == BINBUG)
#ifdef WIN32
        && unit[1]
#endif
        );
#ifdef AMIGA
        enable2(MENUOPT_LEFT_2NDAJOY, addedserver && (machine == ARCADIA || machines[machine].pvis || machine == PONG || machine == BINBUG));
        enable2(MENUOPT_LEFT_1STAPAD, morphos     && (machine == ARCADIA || machines[machine].pvis || machine == PONG || machine == BINBUG));
        enable2(MENUOPT_LEFT_2NDAPAD, morphos     && (machine == ARCADIA || machines[machine].pvis || machine == PONG || machine == BINBUG));
#endif
        if (hostcontroller[0] == CONTROLLER_TRACKBALL)
            checkmx(   MENUOPT_LEFT_TRACKBALL, MENUOPT_LEFT_TRACKBALL, MENUOPT_LEFT_NONE);
        else uncheckmx(MENUOPT_LEFT_TRACKBALL);
        if (hostcontroller[0] == CONTROLLER_1STDJOY)
            checkmx(   MENUOPT_LEFT_1STDJOY, MENUOPT_LEFT_TRACKBALL, MENUOPT_LEFT_NONE);
        else uncheckmx(MENUOPT_LEFT_1STDJOY);
        if (hostcontroller[0] == CONTROLLER_2NDDJOY)
            checkmx(   MENUOPT_LEFT_2NDDJOY, MENUOPT_LEFT_TRACKBALL, MENUOPT_LEFT_NONE);
        else uncheckmx(MENUOPT_LEFT_2NDDJOY);
        if (hostcontroller[0] == CONTROLLER_1STDPAD)
            checkmx(   MENUOPT_LEFT_1STDPAD, MENUOPT_LEFT_TRACKBALL, MENUOPT_LEFT_NONE);
        else uncheckmx(MENUOPT_LEFT_1STDPAD);
        if (hostcontroller[0] == CONTROLLER_2NDDPAD)
            checkmx(   MENUOPT_LEFT_2NDDPAD, MENUOPT_LEFT_TRACKBALL, MENUOPT_LEFT_NONE);
        else uncheckmx(MENUOPT_LEFT_2NDDPAD);
        if (hostcontroller[0] == CONTROLLER_2NDAJOY)
            checkmx(   MENUOPT_LEFT_2NDAJOY, MENUOPT_LEFT_TRACKBALL, MENUOPT_LEFT_NONE);
        else uncheckmx(MENUOPT_LEFT_2NDAJOY);
        if (hostcontroller[0] == CONTROLLER_1STAPAD)
            checkmx(   MENUOPT_LEFT_1STAPAD, MENUOPT_LEFT_TRACKBALL, MENUOPT_LEFT_NONE);
        else uncheckmx(MENUOPT_LEFT_1STAPAD);
        if (hostcontroller[0] == CONTROLLER_2NDAPAD)
            checkmx(   MENUOPT_LEFT_2NDAPAD, MENUOPT_LEFT_TRACKBALL, MENUOPT_LEFT_NONE);
        else uncheckmx(MENUOPT_LEFT_2NDAPAD);
        if (hostcontroller[0] == CONTROLLER_NONE)
            checkmx(   MENUOPT_LEFT_NONE,    MENUOPT_LEFT_TRACKBALL, MENUOPT_LEFT_NONE);
        else uncheckmx(MENUOPT_LEFT_NONE);
    acase MENUITEM_AUTOFIRE1:
        ghost(which,
            machines[machine].pvis == 0
         && machine != ARCADIA
         && machine != BINBUG
         && memmap  != MEMMAP_8600
             ); // eg. PIPBUG, INSTRUCTOR, CD2650, PHUNSY, AY-3-8550
        tick(which, (int) autofire[0]);
    acase MENUITEM_REQUIREBUTTON1:
        ghost(which,
            machine != ARCADIA
         && machine != BINBUG
         && machines[machine].pvis == 0
         && memmap  != MEMMAP_8600
        ); // eg. PIPBUG, INSTRUCTOR, CD2650, PHUNSY, AY-3-8550
        tick(which, requirebutton[0]);
    acase MENUITEM_FORCEFEEDBACK1:
        ghost(which,
#ifdef AMIGA
            !morphos
         ||
#endif
            (   machine != ARCADIA
             && machine != INTERTON
             && machine != ELEKTOR
             && machine != MALZAK
             && memmap  != MEMMAP_GALAXIA
             && machine != PONG
        )   ); // eg. PIPBUG, BINBUG, INSTRUCTOR, CD2650, Astro Wars, Laser Battle, PHUNSY
        tick(which, useff[0]);
    acase MENUFAKE_RIGHT:
        enable2(MENUOPT_RIGHT_TRACKBALL, machine == ARCADIA || machines[machine].pvis || machine == PONG || machine == BINBUG);
        enable2(MENUOPT_RIGHT_NONE,      machine == ARCADIA || machines[machine].pvis || machine == PONG || machine == BINBUG);
        enable2(MENUOPT_RIGHT_1STDJOY,  (machine == ARCADIA || machines[machine].pvis || machine == PONG || machine == BINBUG)
#ifdef WIN32
        && unit[0]
#endif
        );
        enable2(MENUOPT_RIGHT_2NDDJOY,  (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR || memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA || machine == MALZAK || machine == PONG || machine == BINBUG)
#ifdef WIN32
        && unit[1]
#endif
        );
#ifdef AMIGA
        enable2(MENUOPT_RIGHT_2NDAJOY, addedserver && (machine == ARCADIA || machines[machine].pvis || machine == PONG || machine == BINBUG));
        enable2(MENUOPT_RIGHT_1STAPAD, morphos     && (machine == ARCADIA || machines[machine].pvis || machine == PONG || machine == BINBUG));
        enable2(MENUOPT_RIGHT_2NDAPAD, morphos     && (machine == ARCADIA || machines[machine].pvis || machine == PONG || machine == BINBUG));
#endif
        if (hostcontroller[1] == CONTROLLER_TRACKBALL)
            checkmx(   MENUOPT_RIGHT_TRACKBALL, MENUOPT_RIGHT_TRACKBALL, MENUOPT_RIGHT_NONE);
        else uncheckmx(MENUOPT_RIGHT_TRACKBALL);
        if (hostcontroller[1] == CONTROLLER_1STDJOY)
            checkmx(   MENUOPT_RIGHT_1STDJOY, MENUOPT_RIGHT_TRACKBALL, MENUOPT_RIGHT_NONE);
        else uncheckmx(MENUOPT_RIGHT_1STDJOY);
        if (hostcontroller[1] == CONTROLLER_2NDDJOY)
            checkmx(   MENUOPT_RIGHT_2NDDJOY, MENUOPT_RIGHT_TRACKBALL, MENUOPT_RIGHT_NONE);
        else uncheckmx(MENUOPT_RIGHT_2NDDJOY);
        if (hostcontroller[1] == CONTROLLER_1STDPAD)
            checkmx(   MENUOPT_RIGHT_1STDPAD, MENUOPT_RIGHT_TRACKBALL, MENUOPT_RIGHT_NONE);
        else uncheckmx(MENUOPT_RIGHT_1STDPAD);
        if (hostcontroller[1] == CONTROLLER_2NDDPAD)
            checkmx(   MENUOPT_RIGHT_2NDDPAD, MENUOPT_RIGHT_TRACKBALL, MENUOPT_RIGHT_NONE);
        else uncheckmx(MENUOPT_RIGHT_2NDDPAD);
        if (hostcontroller[1] == CONTROLLER_2NDAJOY)
            checkmx(   MENUOPT_RIGHT_2NDAJOY, MENUOPT_RIGHT_TRACKBALL, MENUOPT_RIGHT_NONE);
        else uncheckmx(MENUOPT_RIGHT_2NDAJOY);
        if (hostcontroller[1] == CONTROLLER_1STAPAD)
            checkmx(   MENUOPT_RIGHT_1STAPAD, MENUOPT_RIGHT_TRACKBALL, MENUOPT_RIGHT_NONE);
        else uncheckmx(MENUOPT_RIGHT_1STAPAD);
        if (hostcontroller[1] == CONTROLLER_2NDAPAD)
            checkmx(   MENUOPT_RIGHT_2NDAPAD, MENUOPT_RIGHT_TRACKBALL, MENUOPT_RIGHT_NONE);
        else uncheckmx(MENUOPT_RIGHT_2NDAPAD);
        if (hostcontroller[1] == CONTROLLER_NONE)
            checkmx(   MENUOPT_RIGHT_NONE,    MENUOPT_RIGHT_TRACKBALL, MENUOPT_RIGHT_NONE);
        else uncheckmx(MENUOPT_RIGHT_NONE);
    acase MENUITEM_AUTOFIRE2:
        ghost(which,
            machines[machine].pvis == 0
         && machine != ARCADIA
         && machine != BINBUG
         && memmap  != MEMMAP_8600
             ); // eg. PIPBUG, INSTRUCTOR, CD2650, PHUNSY, AY-3-8550
        tick(which, (int) autofire[1]);
    acase MENUITEM_REQUIREBUTTON2:
        ghost(which,
            machine != ARCADIA
         && machine != BINBUG
         && machines[machine].pvis == 0
         && memmap  != MEMMAP_8600
        ); // eg. PIPBUG, INSTRUCTOR, CD2650, PHUNSY, AY-3-8550
        tick(which, requirebutton[1]);
    acase MENUITEM_FORCEFEEDBACK2:
        ghost(which,
#ifdef AMIGA
            !morphos
         ||
#endif
            (   machine != ARCADIA
             && machine != INTERTON
             && machine != ELEKTOR
             && machine != MALZAK
             && memmap  != MEMMAP_GALAXIA
             && machine != PONG
        )   ); // eg. PIPBUG, BINBUG, INSTRUCTOR, CD2650, Astro Wars, Laser Battle, PHUNSY
        tick(which, useff[1]);
    // "Settings|BIOS �" submenu
    acase MENUFAKE_ELEKTORBIOS:
        enable2(MENUOPT_PHILIPS,     (machine == ELEKTOR && recmode == RECMODE_NORMAL));
        enable2(MENUOPT_HOBBYMODULE, (machine == ELEKTOR && recmode == RECMODE_NORMAL));
        switch (elektor_biosver)
        {
        case ELEKTOR_PHILIPS:
              checkmx(MENUOPT_PHILIPS       , MENUOPT_PHILIPS        , MENUOPT_HOBBYMODULE);
            uncheckmx(MENUOPT_HOBBYMODULE);
        acase ELEKTOR_HOBBYMODULE:
            uncheckmx(MENUOPT_PHILIPS);
              checkmx(MENUOPT_HOBBYMODULE   , MENUOPT_PHILIPS        , MENUOPT_HOBBYMODULE);
        }
    acase MENUFAKE_PIPBUGBIOS:
        enable2(MENUOPT_ARTEMIS,     (machine == PIPBUG && recmode == RECMODE_NORMAL));
        enable2(MENUOPT_HYBUG,       (machine == PIPBUG && recmode == RECMODE_NORMAL));
        enable2(MENUOPT_PIPBUG1BIOS, (machine == PIPBUG && recmode == RECMODE_NORMAL));
        enable2(MENUOPT_PIPBUG2BIOS, (machine == PIPBUG && recmode == RECMODE_NORMAL));
        switch (pipbug_biosver)
        {
        acase PIPBUG_ARTEMIS:
              checkmx(MENUOPT_ARTEMIS,      MENUOPT_ARTEMIS, MENUOPT_PIPBUG2BIOS);
            uncheckmx(MENUOPT_HYBUG);
            uncheckmx(MENUOPT_PIPBUG1BIOS);
            uncheckmx(MENUOPT_PIPBUG2BIOS);
        acase PIPBUG_HYBUG:
            uncheckmx(MENUOPT_ARTEMIS);
              checkmx(MENUOPT_HYBUG,        MENUOPT_ARTEMIS, MENUOPT_PIPBUG2BIOS);
            uncheckmx(MENUOPT_PIPBUG1BIOS);
            uncheckmx(MENUOPT_PIPBUG2BIOS);
        acase PIPBUG_PIPBUG1BIOS:
            uncheckmx(MENUOPT_ARTEMIS);
            uncheckmx(MENUOPT_HYBUG);
              checkmx(MENUOPT_PIPBUG1BIOS,  MENUOPT_ARTEMIS, MENUOPT_PIPBUG2BIOS);
            uncheckmx(MENUOPT_PIPBUG2BIOS);
        acase PIPBUG_PIPBUG2BIOS:
            uncheckmx(MENUOPT_ARTEMIS);
            uncheckmx(MENUOPT_HYBUG);
            uncheckmx(MENUOPT_PIPBUG1BIOS);
              checkmx(MENUOPT_PIPBUG2BIOS,  MENUOPT_ARTEMIS, MENUOPT_PIPBUG2BIOS);
        }
    acase MENUFAKE_BINBUGBIOS:
        enable2(MENUOPT_BINBUG35BIOS, (machine == BINBUG && recmode == RECMODE_NORMAL));
        enable2(MENUOPT_BINBUG36BIOS, (machine == BINBUG && recmode == RECMODE_NORMAL));
        enable2(MENUOPT_BINBUG61BIOS, (machine == BINBUG && recmode == RECMODE_NORMAL));
        enable2(MENUOPT_GBUG,         (machine == BINBUG && recmode == RECMODE_NORMAL));
        enable2(MENUOPT_MIKEBUG,      (machine == BINBUG && recmode == RECMODE_NORMAL));
        enable2(MENUOPT_MYBUG,        (machine == BINBUG && recmode == RECMODE_NORMAL));
        switch (binbug_biosver)
        {
        case  BINBUG_35:
              checkmx(MENUOPT_BINBUG35BIOS, MENUOPT_BINBUG35BIOS, MENUOPT_MYBUG);
            uncheckmx(MENUOPT_BINBUG36BIOS);
            uncheckmx(MENUOPT_BINBUG61BIOS);
            uncheckmx(MENUOPT_GBUG);
            uncheckmx(MENUOPT_MIKEBUG);
            uncheckmx(MENUOPT_MYBUG);
        acase BINBUG_36:
            uncheckmx(MENUOPT_BINBUG35BIOS);
              checkmx(MENUOPT_BINBUG36BIOS, MENUOPT_BINBUG35BIOS, MENUOPT_MYBUG);
            uncheckmx(MENUOPT_BINBUG61BIOS);
            uncheckmx(MENUOPT_GBUG);
            uncheckmx(MENUOPT_MIKEBUG);
            uncheckmx(MENUOPT_MYBUG);
        acase BINBUG_61:
            uncheckmx(MENUOPT_BINBUG35BIOS);
            uncheckmx(MENUOPT_BINBUG36BIOS);
              checkmx(MENUOPT_BINBUG61BIOS, MENUOPT_BINBUG35BIOS, MENUOPT_MYBUG);
            uncheckmx(MENUOPT_GBUG);
            uncheckmx(MENUOPT_MIKEBUG);
            uncheckmx(MENUOPT_MYBUG);
        acase BINBUG_GBUG:
            uncheckmx(MENUOPT_BINBUG35BIOS);
            uncheckmx(MENUOPT_BINBUG36BIOS);
            uncheckmx(MENUOPT_BINBUG61BIOS);
              checkmx(MENUOPT_GBUG,         MENUOPT_BINBUG35BIOS, MENUOPT_MYBUG);
            uncheckmx(MENUOPT_MIKEBUG);
            uncheckmx(MENUOPT_MYBUG);
        acase BINBUG_MIKEBUG:
            uncheckmx(MENUOPT_BINBUG35BIOS);
            uncheckmx(MENUOPT_BINBUG36BIOS);
            uncheckmx(MENUOPT_BINBUG61BIOS);
            uncheckmx(MENUOPT_GBUG);
              checkmx(MENUOPT_MIKEBUG,      MENUOPT_BINBUG35BIOS, MENUOPT_MYBUG);
            uncheckmx(MENUOPT_MYBUG);
        acase BINBUG_MYBUG:
            uncheckmx(MENUOPT_BINBUG35BIOS);
            uncheckmx(MENUOPT_BINBUG36BIOS);
            uncheckmx(MENUOPT_BINBUG61BIOS);
            uncheckmx(MENUOPT_GBUG);
            uncheckmx(MENUOPT_MIKEBUG);
              checkmx(MENUOPT_MYBUG,        MENUOPT_BINBUG35BIOS, MENUOPT_MYBUG);
        }
    acase MENUFAKE_CD2650BIOS:
        enable2(MENUOPT_BIOS_IPL,        (machine == CD2650 && recmode == RECMODE_NORMAL));
        enable2(MENUOPT_BIOS_POPMON,     (machine == CD2650 && recmode == RECMODE_NORMAL));
        enable2(MENUOPT_BIOS_SUPERVISOR, (machine == CD2650 && recmode == RECMODE_NORMAL));
        switch (cd2650_biosver)
        {
        case CD2650_IPL:
              checkmx(MENUOPT_BIOS_IPL,        MENUOPT_BIOS_IPL, MENUOPT_BIOS_SUPERVISOR);
            uncheckmx(MENUOPT_BIOS_POPMON);
            uncheckmx(MENUOPT_BIOS_SUPERVISOR);
        acase CD2650_POPMON:
            uncheckmx(MENUOPT_BIOS_IPL);
              checkmx(MENUOPT_BIOS_POPMON    , MENUOPT_BIOS_IPL, MENUOPT_BIOS_SUPERVISOR);
            uncheckmx(MENUOPT_BIOS_SUPERVISOR);
        acase CD2650_SUPERVISOR:
            uncheckmx(MENUOPT_BIOS_IPL);
            uncheckmx(MENUOPT_BIOS_POPMON);
              checkmx(MENUOPT_BIOS_SUPERVISOR, MENUOPT_BIOS_IPL, MENUOPT_BIOS_SUPERVISOR);
        }
    acase MENUFAKE_PHUNSYBIOS:
        enable2(MENUOPT_BIOS_MINIMONITOR, (machine == PHUNSY && recmode == RECMODE_NORMAL));
        enable2(MENUOPT_BIOS_PHUNSY,      (machine == PHUNSY && recmode == RECMODE_NORMAL));
        switch (phunsy_biosver)
        {
        case PHUNSY_MINIMONITOR:
              checkmx(MENUOPT_BIOS_MINIMONITOR, MENUOPT_BIOS_MINIMONITOR, MENUOPT_BIOS_PHUNSY);
            uncheckmx(MENUOPT_BIOS_PHUNSY);
        acase PHUNSY_PHUNSY:
            uncheckmx(MENUOPT_BIOS_MINIMONITOR);
              checkmx(MENUOPT_BIOS_PHUNSY,      MENUOPT_BIOS_MINIMONITOR, MENUOPT_BIOS_PHUNSY);
        }
    acase MENUFAKE_SELBSTBIOS:
        enable2(MENUOPT_00BIOS,           (machine == SELBST && recmode == RECMODE_NORMAL));
        enable2(MENUOPT_09BIOS,           (machine == SELBST && recmode == RECMODE_NORMAL));
        enable2(MENUOPT_10BIOS,           (machine == SELBST && recmode == RECMODE_NORMAL));
        enable2(MENUOPT_20BIOS,           (machine == SELBST && recmode == RECMODE_NORMAL));
        switch (selbst_biosver)
        {
        case SELBST_BIOS00:
              checkmx(MENUOPT_00BIOS, MENUOPT_00BIOS, MENUOPT_20BIOS);
            uncheckmx(MENUOPT_09BIOS);
            uncheckmx(MENUOPT_10BIOS);
            uncheckmx(MENUOPT_20BIOS);
        acase SELBST_BIOS09:
            uncheckmx(MENUOPT_00BIOS);
              checkmx(MENUOPT_09BIOS, MENUOPT_00BIOS, MENUOPT_20BIOS);
            uncheckmx(MENUOPT_10BIOS);
            uncheckmx(MENUOPT_20BIOS);
        acase SELBST_BIOS10:
            uncheckmx(MENUOPT_00BIOS);
            uncheckmx(MENUOPT_09BIOS);
              checkmx(MENUOPT_10BIOS, MENUOPT_00BIOS, MENUOPT_20BIOS);
            uncheckmx(MENUOPT_20BIOS);
        acase SELBST_BIOS20:
            uncheckmx(MENUOPT_00BIOS);
            uncheckmx(MENUOPT_09BIOS);
            uncheckmx(MENUOPT_10BIOS);
              checkmx(MENUOPT_20BIOS, MENUOPT_00BIOS, MENUOPT_20BIOS);
        }
    // "Settings|DOS �" submenu
    acase MENUFAKE_BINBUGDOS:
        enable2(MENUOPT_MICRODOS,    machine == BINBUG);
        enable2(MENUOPT_VHSDOS,      machine == BINBUG);
        enable2(MENUOPT_NOBINBUGDOS, machine == BINBUG);
        switch (binbug_dosver)
        {
        case DOS_MICRODOS:
              checkmx(MENUOPT_MICRODOS,    MENUOPT_MICRODOS, MENUOPT_NOBINBUGDOS);
            uncheckmx(MENUOPT_VHSDOS);
            uncheckmx(MENUOPT_NOBINBUGDOS);
        acase DOS_VHSDOS:
            uncheckmx(MENUOPT_MICRODOS);
              checkmx(MENUOPT_VHSDOS,      MENUOPT_MICRODOS, MENUOPT_NOBINBUGDOS);
            uncheckmx(MENUOPT_NOBINBUGDOS);
        acase DOS_NOBINBUGDOS:
            uncheckmx(MENUOPT_MICRODOS);
            uncheckmx(MENUOPT_VHSDOS);
              checkmx(MENUOPT_NOBINBUGDOS, MENUOPT_MICRODOS, MENUOPT_NOBINBUGDOS);
        }
    acase MENUFAKE_CD2650DOS:
        enable2(MENUOPT_CDDOS,       machine == CD2650);
        enable2(MENUOPT_P1DOS,       machine == CD2650);
        enable2(MENUOPT_NOCD2650DOS, machine == CD2650);
        switch (cd2650_dosver)
        {
        case DOS_CDDOS:
              checkmx(MENUOPT_CDDOS,       MENUOPT_CDDOS, MENUOPT_NOCD2650DOS);
            uncheckmx(MENUOPT_P1DOS);
            uncheckmx(MENUOPT_NOCD2650DOS);
        acase DOS_P1DOS:
            uncheckmx(MENUOPT_CDDOS);
              checkmx(MENUOPT_P1DOS,       MENUOPT_CDDOS, MENUOPT_NOCD2650DOS);
            uncheckmx(MENUOPT_NOCD2650DOS);
        acase DOS_NOCD2650DOS:
            uncheckmx(MENUOPT_CDDOS);
            uncheckmx(MENUOPT_P1DOS);
              checkmx(MENUOPT_NOCD2650DOS, MENUOPT_CDDOS, MENUOPT_NOCD2650DOS);
        }
    acase MENUFAKE_TWINDOS:
        enable2(MENUOPT_DOS_EXOS     , machine == TWIN);
        enable2(MENUOPT_DOS_SDOS20   , machine == TWIN);
        enable2(MENUOPT_DOS_SDOS40   , machine == TWIN);
        enable2(MENUOPT_DOS_SDOS42   , machine == TWIN);
        enable2(MENUOPT_DOS_TOS      , machine == TWIN);
        enable2(MENUOPT_DOS_UDOS     , machine == TWIN);
        enable2(MENUOPT_DOS_NOTWINDOS, machine == TWIN);
        switch (twin_dosver)
        {
        case 0:
              checkmx(MENUOPT_DOS_EXOS,      MENUOPT_DOS_EXOS, MENUOPT_DOS_NOTWINDOS);
            uncheckmx(MENUOPT_DOS_SDOS20);
            uncheckmx(MENUOPT_DOS_SDOS40);
            uncheckmx(MENUOPT_DOS_SDOS42);
            uncheckmx(MENUOPT_DOS_TOS);
            uncheckmx(MENUOPT_DOS_UDOS);
            uncheckmx(MENUOPT_DOS_NOTWINDOS);
        acase 1:
            uncheckmx(MENUOPT_DOS_EXOS);
              checkmx(MENUOPT_DOS_SDOS20,    MENUOPT_DOS_EXOS, MENUOPT_DOS_NOTWINDOS);
            uncheckmx(MENUOPT_DOS_SDOS40);
            uncheckmx(MENUOPT_DOS_SDOS42);
            uncheckmx(MENUOPT_DOS_TOS);
            uncheckmx(MENUOPT_DOS_UDOS);
            uncheckmx(MENUOPT_DOS_NOTWINDOS);
        acase 2:
            uncheckmx(MENUOPT_DOS_EXOS);
            uncheckmx(MENUOPT_DOS_SDOS20);
              checkmx(MENUOPT_DOS_SDOS40,    MENUOPT_DOS_EXOS, MENUOPT_DOS_NOTWINDOS);
            uncheckmx(MENUOPT_DOS_SDOS42);
            uncheckmx(MENUOPT_DOS_TOS);
            uncheckmx(MENUOPT_DOS_UDOS);
            uncheckmx(MENUOPT_DOS_NOTWINDOS);
        acase 3:
            uncheckmx(MENUOPT_DOS_EXOS);
            uncheckmx(MENUOPT_DOS_SDOS20);
            uncheckmx(MENUOPT_DOS_SDOS40);
              checkmx(MENUOPT_DOS_SDOS42,    MENUOPT_DOS_EXOS, MENUOPT_DOS_NOTWINDOS);
            uncheckmx(MENUOPT_DOS_TOS);
            uncheckmx(MENUOPT_DOS_UDOS);
            uncheckmx(MENUOPT_DOS_NOTWINDOS);
        acase 4:
            uncheckmx(MENUOPT_DOS_EXOS);
            uncheckmx(MENUOPT_DOS_SDOS20);
            uncheckmx(MENUOPT_DOS_SDOS40);
            uncheckmx(MENUOPT_DOS_SDOS42);
              checkmx(MENUOPT_DOS_TOS,       MENUOPT_DOS_EXOS, MENUOPT_DOS_NOTWINDOS);
            uncheckmx(MENUOPT_DOS_UDOS);
            uncheckmx(MENUOPT_DOS_NOTWINDOS);
        acase 5:
            uncheckmx(MENUOPT_DOS_EXOS);
            uncheckmx(MENUOPT_DOS_SDOS20);
            uncheckmx(MENUOPT_DOS_SDOS40);
            uncheckmx(MENUOPT_DOS_SDOS42);
            uncheckmx(MENUOPT_DOS_TOS);
              checkmx(MENUOPT_DOS_UDOS,      MENUOPT_DOS_EXOS, MENUOPT_DOS_NOTWINDOS);
            uncheckmx(MENUOPT_DOS_NOTWINDOS);
        acase 6:
            uncheckmx(MENUOPT_DOS_EXOS);
            uncheckmx(MENUOPT_DOS_SDOS20);
            uncheckmx(MENUOPT_DOS_SDOS40);
            uncheckmx(MENUOPT_DOS_SDOS42);
            uncheckmx(MENUOPT_DOS_TOS);
            uncheckmx(MENUOPT_DOS_UDOS);
              checkmx(MENUOPT_DOS_NOTWINDOS, MENUOPT_DOS_EXOS, MENUOPT_DOS_NOTWINDOS);
        }
    // "Settings|Colours �" submenu
    acase MENUITEM_UNDITHER:
        ghost(which, machine != ARCADIA && machine != INTERTON);
        tick(which, undither);
    acase MENUITEM_DARKENBG:
        ghost(which, machine != INTERTON && machine != ELEKTOR);
        tick(which, darkenbg);
    acase MENUITEM_FLAGLINE:
        ghost(which, machine != ARCADIA);
        tick(which, flagline);
    acase MENUFAKE_COLOURSET:
        enable2(MENUOPT_AMBER      , SubWindowPtr[SUBWINDOW_PALETTE] == NULL);
        enable2(MENUOPT_GREEN      , SubWindowPtr[SUBWINDOW_PALETTE] == NULL);
        enable2(MENUOPT_GREYSCALE  , SubWindowPtr[SUBWINDOW_PALETTE] == NULL);
        enable2(MENUOPT_PURECOLOURS, SubWindowPtr[SUBWINDOW_PALETTE] == NULL);
        enable2(MENUOPT_PVICOLOURS , SubWindowPtr[SUBWINDOW_PALETTE] == NULL);
        enable2(MENUOPT_UVICOLOURS , SubWindowPtr[SUBWINDOW_PALETTE] == NULL);
        switch (colourset)
        {
        case AMBERSCALE:
              checkmx(MENUOPT_AMBER,       MENUOPT_AMBER, MENUOPT_UVICOLOURS);
            uncheckmx(MENUOPT_GREEN);
            uncheckmx(MENUOPT_GREYSCALE);
            uncheckmx(MENUOPT_PURECOLOURS);
            uncheckmx(MENUOPT_PVICOLOURS);
            uncheckmx(MENUOPT_UVICOLOURS);
        acase GREENSCALE:
            uncheckmx(MENUOPT_AMBER);
              checkmx(MENUOPT_GREEN,       MENUOPT_AMBER, MENUOPT_UVICOLOURS);
            uncheckmx(MENUOPT_GREYSCALE);
            uncheckmx(MENUOPT_PURECOLOURS);
            uncheckmx(MENUOPT_PVICOLOURS);
            uncheckmx(MENUOPT_UVICOLOURS);
        acase GREYSCALE:
            uncheckmx(MENUOPT_AMBER);
            uncheckmx(MENUOPT_GREEN);
              checkmx(MENUOPT_GREYSCALE  , MENUOPT_AMBER, MENUOPT_UVICOLOURS);
            uncheckmx(MENUOPT_PURECOLOURS);
            uncheckmx(MENUOPT_PVICOLOURS);
            uncheckmx(MENUOPT_UVICOLOURS);
        acase PURECOLOURS:
            uncheckmx(MENUOPT_AMBER);
            uncheckmx(MENUOPT_GREEN);
            uncheckmx(MENUOPT_GREYSCALE);
              checkmx(MENUOPT_PURECOLOURS, MENUOPT_AMBER, MENUOPT_UVICOLOURS);
            uncheckmx(MENUOPT_PVICOLOURS);
            uncheckmx(MENUOPT_UVICOLOURS);
        acase PVICOLOURS:
            uncheckmx(MENUOPT_AMBER);
            uncheckmx(MENUOPT_GREEN);
            uncheckmx(MENUOPT_GREYSCALE);
            uncheckmx(MENUOPT_PURECOLOURS);
              checkmx(MENUOPT_PVICOLOURS , MENUOPT_AMBER, MENUOPT_UVICOLOURS);
            uncheckmx(MENUOPT_UVICOLOURS);
        acase UVICOLOURS:
            uncheckmx(MENUOPT_AMBER);
            uncheckmx(MENUOPT_GREEN);
            uncheckmx(MENUOPT_GREYSCALE);
            uncheckmx(MENUOPT_PURECOLOURS);
            uncheckmx(MENUOPT_PVICOLOURS);
              checkmx(MENUOPT_UVICOLOURS , MENUOPT_AMBER, MENUOPT_UVICOLOURS);
        }
    acase MENUITEM_INVERSE:
        tick(which, inverse);
        ghost(which,
            machine != PIPBUG
         && machine != BINBUG
         && machine != TWIN
         && machine != CD2650
         && machine != PHUNSY
         && machine != SELBST
         && machine != TYPERIGHT
        );
    // "Settings|Emulator �" submenu
    acase MENUITEM_AUTOSAVE:
        tick(which, autosave);
    acase MENUITEM_STARTUPUPDATES:
        tick(which, startupupdates);
    acase MENUITEM_CONFIRM:
        tick(which, confirmable);
#ifdef AMIGA
    acase MENUITEM_CREATEICONS:
        tick(which, icons);
#endif
    acase MENUITEM_CONSOLE:
        ghost(which,
             machine != PIPBUG
          && machine != BINBUG
          && machine != SELBST
             );
        tick(which, echoconsole);
#ifdef WIN32
    acase MENUITEM_CHEEVOS1:
        ghost(which, machine != ARCADIA && machine != INTERTON && machine != ELEKTOR);
        tick(which, cheevos);
    acase MENUITEM_CHEEVOS2:
        ghost(which, machine != ARCADIA && machine != INTERTON && machine != ELEKTOR);
#endif
    acase MENUITEM_EMUID:
        ghost(which, machine == PONG || machine == TYPERIGHT);
        tick(which, emuid);
    acase MENUITEM_POST:
        ghost(which, machine != TWIN && (machine != PIPBUG || pipbug_biosver != PIPBUG_PIPBUG2BIOS) && !machines[machine].coinop);
        tick(which, post);
    acase MENUITEM_RANDOMIZE:
        ghost(which,
            (machine != ARCADIA && machine != INTERTON && machine != INSTRUCTOR)
#ifdef WIN32
         || cheevos
#endif
        );
        tick(which, randomizememory);
    acase MENUITEM_SENSEGAME:
        tick(which, sensegame);
    acase MENUITEM_SHOWTOD:
        tick(which, showtod);
    acase MENUITEM_USESTUBS:
        ghost(which, machine != ARCADIA && machine != INTERTON);
        tick(which, usestubs);
    acase MENUFAKE_FRAMEBASED:
        enable2(MENUOPT_FRAMEBASED, machine == ARCADIA || machine == BINBUG || machine == CD2650 || machine == PHUNSY);
        enable2(MENUOPT_PIXELBASED, machine == ARCADIA || machine == BINBUG || machine == CD2650 || machine == PHUNSY);
        if (framebased)
        {     checkmx(MENUOPT_FRAMEBASED, MENUOPT_FRAMEBASED, MENUOPT_PIXELBASED);
            uncheckmx(MENUOPT_PIXELBASED);
        } else
        {   uncheckmx(MENUOPT_FRAMEBASED);
              checkmx(MENUOPT_PIXELBASED, MENUOPT_FRAMEBASED, MENUOPT_PIXELBASED);
        }
    acase MENUFAKE_LOGTOFILE:
        switch (logfile)
        {
        case LOGFILE_APPEND:
              checkmx(MENUOPT_APPEND,  MENUOPT_APPEND, MENUOPT_REPLACE);
            uncheckmx(MENUOPT_IGNORE);
            uncheckmx(MENUOPT_REPLACE);
        acase LOGFILE_IGNORE:
            uncheckmx(MENUOPT_APPEND);
              checkmx(MENUOPT_IGNORE,  MENUOPT_APPEND, MENUOPT_REPLACE);
            uncheckmx(MENUOPT_REPLACE);
        acase LOGFILE_REPLACE:
            uncheckmx(MENUOPT_APPEND);
            uncheckmx(MENUOPT_IGNORE);
              checkmx(MENUOPT_REPLACE, MENUOPT_APPEND, MENUOPT_REPLACE);
        }
    // "Settings|Filters �" submenu
    acase MENUFAKE_FILTER:
        enable2(MENUOPT_FILTER_SCALE2X, (size >= 2 && size <= 4));
#ifdef WIN32
        enable2(MENUOPT_FILTER_3D,      d3d);
        enable2(MENUOPT_FILTER_HQX,     (size >= 2 && size <= 4));
        switch (filter)
        {
        case FILTER_3D:
              checkmx(MENUOPT_FILTER_3D,      MENUOPT_FILTER_3D, MENUOPT_FILTER_NONE);
            uncheckmx(MENUOPT_FILTER_HQX);
            uncheckmx(MENUOPT_FILTER_SCALE2X);
            uncheckmx(MENUOPT_FILTER_NONE);
        acase FILTER_HQX:
            uncheckmx(MENUOPT_FILTER_3D);
              checkmx(MENUOPT_FILTER_HQX,     MENUOPT_FILTER_3D, MENUOPT_FILTER_NONE);
            uncheckmx(MENUOPT_FILTER_SCALE2X);
            uncheckmx(MENUOPT_FILTER_NONE);
        acase FILTER_SCALE2X:
            uncheckmx(MENUOPT_FILTER_3D);
            uncheckmx(MENUOPT_FILTER_HQX);
              checkmx(MENUOPT_FILTER_SCALE2X, MENUOPT_FILTER_3D, MENUOPT_FILTER_NONE);
            uncheckmx(MENUOPT_FILTER_NONE);
        acase FILTER_NONE:
            uncheckmx(MENUOPT_FILTER_3D);
            uncheckmx(MENUOPT_FILTER_HQX);
            uncheckmx(MENUOPT_FILTER_SCALE2X);
              checkmx(MENUOPT_FILTER_NONE,    MENUOPT_FILTER_3D, MENUOPT_FILTER_NONE);
        }
#endif
#ifdef AMIGA
        switch (filter)
        {
        case FILTER_SCALE2X:
              checkmx(MENUOPT_FILTER_SCALE2X, MENUOPT_FILTER_SCALE2X, MENUOPT_FILTER_NONE);
            uncheckmx(MENUOPT_FILTER_NONE);
        acase FILTER_NONE:
            uncheckmx(MENUOPT_FILTER_SCALE2X);
              checkmx(MENUOPT_FILTER_NONE,    MENUOPT_FILTER_SCALE2X, MENUOPT_FILTER_NONE);
        }
#endif
#ifdef WIN32
    acase MENUFAKE_STRETCHING:
        enable2(MENUOPT_UNSTRETCHED,  filter != FILTER_3D);
        enable2(MENUOPT_STRETCH43,    filter != FILTER_3D);
        enable2(MENUOPT_STRETCHTOFIT, filter != FILTER_3D);
        switch (stretching)
        {
        case 0:
              checkmx(MENUOPT_UNSTRETCHED,  MENUOPT_UNSTRETCHED, MENUOPT_STRETCHTOFIT);
            uncheckmx(MENUOPT_STRETCH43);
            uncheckmx(MENUOPT_STRETCHTOFIT);
        acase 1:
            uncheckmx(MENUOPT_UNSTRETCHED);
              checkmx(MENUOPT_STRETCH43,    MENUOPT_UNSTRETCHED, MENUOPT_STRETCHTOFIT);
            uncheckmx(MENUOPT_STRETCHTOFIT);
        acase 2:
            uncheckmx(MENUOPT_UNSTRETCHED);
            uncheckmx(MENUOPT_STRETCH43);
              checkmx(MENUOPT_STRETCHTOFIT, MENUOPT_UNSTRETCHED, MENUOPT_STRETCHTOFIT);
        }
#endif
    // "Settings|Graphics �" submenu
    acase MENUFAKE_SIZE:
        enable2(MENUOPT_1XSIZE, !fullscreen);
        enable2(MENUOPT_2XSIZE, !fullscreen);
        enable2(MENUOPT_3XSIZE, !fullscreen);
        enable2(MENUOPT_4XSIZE, !fullscreen);
        enable2(MENUOPT_5XSIZE, !fullscreen);
        enable2(MENUOPT_6XSIZE, !fullscreen);
        if (size == 1)
            checkmx(   MENUOPT_1XSIZE, MENUOPT_1XSIZE, MENUOPT_6XSIZE);
        else uncheckmx(MENUOPT_1XSIZE);
        if (size == 2)
            checkmx(   MENUOPT_2XSIZE, MENUOPT_1XSIZE, MENUOPT_6XSIZE);
        else uncheckmx(MENUOPT_2XSIZE);
        if (size == 3)
            checkmx(   MENUOPT_3XSIZE, MENUOPT_1XSIZE, MENUOPT_6XSIZE);
        else uncheckmx(MENUOPT_3XSIZE);
        if (size == 4)
            checkmx(   MENUOPT_4XSIZE, MENUOPT_1XSIZE, MENUOPT_6XSIZE);
        else uncheckmx(MENUOPT_4XSIZE);
        if (size == 5)
            checkmx(   MENUOPT_5XSIZE, MENUOPT_1XSIZE, MENUOPT_6XSIZE);
        else uncheckmx(MENUOPT_5XSIZE);
        if (size == 6)
            checkmx(   MENUOPT_6XSIZE, MENUOPT_1XSIZE, MENUOPT_6XSIZE);
        else uncheckmx(MENUOPT_6XSIZE);
    acase MENUITEM_USEMARGINS:
        ghost(which, crippled ||
        (   machine != ARCADIA
         && machine != INTERTON
         && machine != ELEKTOR
         && (machine != PIPBUG || pipbug_vdu != VDU_RADIOBULLETIN)
         && machine != BINBUG
         && machine != TWIN
         && machine != CD2650
         && machine != PHUNSY
         && machine != PONG
        ));
        tick(which, usemargins);
    acase MENUITEM_FULLSCREEN:
        tick(which, fullscreen);
    acase MENUITEM_NARROW:
        ghost(which,
            fullscreen
         || GIFHandle
         || ANIMHandle
         || PNGHandle
         || MNGHandle
        );
        tick(which, wide == 1);
    acase MENUITEM_DEJITTER:
        ghost(which, machine != ARCADIA && machine != INTERTON && machine != ELEKTOR);
        tick(which, dejitter);
#ifdef WIN32
    acase MENUITEM_SKIES:
        ghost(which, filter == FILTER_3D || filter == FILTER_HQX);
        tick(which, enhancestars);
#endif
    acase MENUITEM_UNLIT:
        ghost(which,
            machine != PIPBUG
         && machine != BINBUG
         && machine != INSTRUCTOR
         && machine != PHUNSY
         && machine != SELBST
         && machine != MIKIT
        ); // eg. ARCADIA, INTERTON, ELEKTOR, CD2650, coin-ops...
        tick(which, drawunlit);
    acase MENUITEM_CORNERS:
        ghost(which,
            machine != INTERTON
         && machine != ELEKTOR
         && machine != PIPBUG
         && machine != INSTRUCTOR
         && machine != PHUNSY
         && machine != SELBST
         && machine != MIKIT
        ); // eg. ARCADIA, BINBUG, coin-ops, CD2650, PONG
        tick(which, drawcorners);
    acase MENUITEM_SHOWLEDS:
        ghost(which,
            machine != PIPBUG
         && machine != BINBUG
         && machine != PHUNSY
         && machine != SELBST
        );
        tick(which, showleds);
    acase MENUITEM_ROTATE:
        ghost(which, memmapinfo[memmap].rotate ? FALSE : TRUE);
        // eg. ARCADIA, INTERTON, ELEKTOR, INSTRUCTOR, PIPBUG, BINBUG, Laser Battle, MALZAK, CD2650, PHUNSY, PONG, TYPERIGHT
        tick(which, rotate);
    acase MENUITEM_SCANLINES:
        tick(which, scanlines);
        ghost(which, machine == INSTRUCTOR || machine == MIKIT || filter == FILTER_3D || size == 1);
#ifdef WIN32
    acase MENUITEM_STRETCH43:
        tick(which, stretch43);
#endif
    // "Settings|Input �" submenu
    acase MENUITEM_ANALOG:
        ghost(which,
            machine != ARCADIA
         && machine != INTERTON
         && machine != ELEKTOR
         && machine != BINBUG
             ); // eg. PIPBUG, INSTRUCTOR, CD2650, coin-ops, PHUNSY
        tick(which, (int) analog);
#ifdef WIN32
    acase MENUITEM_CALIBRATE:
        ghost(which,
            machine != ARCADIA
         && machine != BINBUG
         && machines[machine].pvis == 0
         && machine != PONG
        ); // eg. PIPBUG, INSTRUCTOR, CD2650, PHUNSY, TYPERIGHT
#endif
    acase MENUITEM_SENSITIVITY:
        ghost(which,
            (   machine != ARCADIA
             && machine != INTERTON
             && machine != ELEKTOR
             && machine != BINBUG
             && machine != PONG
            )
         || !analog
        );
    acase MENUITEM_SPRINGLOADED:
        ghost(which,
            (   machine != ARCADIA
             && machine != INTERTON
             && machine != ELEKTOR
             && machine != BINBUG
             && machine != PONG
            )
         || !analog
        ); // eg. PIPBUG, INSTRUCTOR, CD2650, coin-ops, PHUNSY
        tick(which, (int) springloaded);
    acase MENUITEM_LOWERCASEINPUT:
        ghost(which,
             machine != PIPBUG
         &&  machine != BINBUG
         && (machine != CD2650 || cd2650_biosver != CD2650_IPL)
         &&  machine != PHUNSY
         &&  machine != SELBST
        );
        tick(which, lowercase);
    acase MENUITEM_CONFINE:
        ghost(which,
            (   hostcontroller[0] != CONTROLLER_TRACKBALL
             && hostcontroller[1] != CONTROLLER_TRACKBALL
        )   );
        tick(which, confine);
    acase MENUITEM_ERASEDEL:
        ghost(which, machine != TWIN && (machine != PIPBUG || pipbug_biosver != PIPBUG_PIPBUG2BIOS));
        tick(which, (int) erasedel);
    acase MENUITEM_AUTOCOIN:
        ghost(which, !machines[machine].coinop);
        tick(which, autocoin);
    acase MENUITEM_GUESTRMB:
#ifdef AMIGA
        ghost(which, broken_wi);
#endif
        // we mostly don't ghost this one, just so it is as easy as possible for the user to turn off if necessary
        tick(which, guestrmb);
    acase MENUITEM_QUEUEKEYSTROKES:
        ghost(which, !ISQWERTY || machine == TYPERIGHT);
        tick(which, queuekeystrokes);
    acase MENUITEM_SHOWPOSITIONS:
        ghost(which, machine != PONG);
        tick(which, showpositions);
    acase MENUITEM_SWAPPED:
        ghost(which, !machines[machine].joystick);
        tick(which, (int) swapped);
    acase MENUITEM_REARRANGE:
        ghost(which, !machines[machine].joystick);
    acase MENUITEM_REDEFINEKEYS:
        ghost(which,
            machine != ARCADIA
         && machines[machine].pvis == 0
         && machine != INSTRUCTOR
         && machine != PHUNSY
         && machine != PONG
         && machine != SELBST
         && machine != MIKIT
         && machine != TYPERIGHT
        ); // eg. PIPBUG, BINBUG, CD2650
    acase MENUFAKE_KEYMAP:
        enable2(MENUOPT_KEYMAP_HOST, ISQWERTY);
        enable2(MENUOPT_KEYMAP_EA  , ISQWERTY);
        enable2(MENUOPT_KEYMAP_TT  , ISQWERTY);
        switch (keymap)
        {
        case KEYMAP_HOST:
              checkmx(MENUOPT_KEYMAP_HOST, MENUOPT_KEYMAP_HOST, MENUOPT_KEYMAP_TT);
            uncheckmx(MENUOPT_KEYMAP_EA);
            uncheckmx(MENUOPT_KEYMAP_TT);
        acase KEYMAP_EA:
            uncheckmx(MENUOPT_KEYMAP_HOST);
              checkmx(MENUOPT_KEYMAP_EA  , MENUOPT_KEYMAP_HOST, MENUOPT_KEYMAP_TT);
            uncheckmx(MENUOPT_KEYMAP_TT);
        acase KEYMAP_TT:
            uncheckmx(MENUOPT_KEYMAP_HOST);
            uncheckmx(MENUOPT_KEYMAP_EA);
              checkmx(MENUOPT_KEYMAP_TT  , MENUOPT_KEYMAP_HOST, MENUOPT_KEYMAP_TT);
        }
#ifdef WIN32
    // "Options|Language �" submenu
    acase MENUFAKE_LANGUAGE:
        if (language == LANGUAGE_ENG) checkmx(MENUOPT_ENG, MENUOPT_ENG, MENUOPT_SPA); else uncheckmx(MENUOPT_ENG);
        if (language == LANGUAGE_HOL) checkmx(MENUOPT_HOL, MENUOPT_ENG, MENUOPT_SPA); else uncheckmx(MENUOPT_HOL);
        if (language == LANGUAGE_FRA) checkmx(MENUOPT_FRA, MENUOPT_ENG, MENUOPT_SPA); else uncheckmx(MENUOPT_FRA);
        if (language == LANGUAGE_GER) checkmx(MENUOPT_GER, MENUOPT_ENG, MENUOPT_SPA); else uncheckmx(MENUOPT_GER);
        if (language == LANGUAGE_GRE) checkmx(MENUOPT_GRE, MENUOPT_ENG, MENUOPT_SPA); else uncheckmx(MENUOPT_GRE);
        if (language == LANGUAGE_ITA) checkmx(MENUOPT_ITA, MENUOPT_ENG, MENUOPT_SPA); else uncheckmx(MENUOPT_ITA);
        if (language == LANGUAGE_POL) checkmx(MENUOPT_POL, MENUOPT_ENG, MENUOPT_SPA); else uncheckmx(MENUOPT_POL);
        if (language == LANGUAGE_RUS) checkmx(MENUOPT_RUS, MENUOPT_ENG, MENUOPT_SPA); else uncheckmx(MENUOPT_RUS);
        if (language == LANGUAGE_SPA) checkmx(MENUOPT_SPA, MENUOPT_ENG, MENUOPT_SPA); else uncheckmx(MENUOPT_SPA);
#endif
    // "Settings|Machine �" submenu
    acase MENUFAKE_MACHINE:
        enable2(MENUOPT_ARCADIA,     !crippled);
        enable2(MENUOPT_INTERTON,    !crippled);
        enable2(MENUOPT_ELEKTOR,     !crippled);
        enable2(MENUOPT_PIPBUG,      !crippled);
        enable2(MENUOPT_BINBUG,      !crippled);
        enable2(MENUOPT_INSTRUCTOR,  !crippled);
        enable2(MENUOPT_TWIN,        !crippled);
        enable2(MENUOPT_CD2650,      !crippled);
        enable2(MENUOPT_ASTROWARS,   !crippled);
        enable2(MENUOPT_GALAXIA,     !crippled);
        enable2(MENUOPT_LASERBATTLE, !crippled);
        enable2(MENUOPT_LAZARIAN,    !crippled);
        enable2(MENUOPT_MALZAK1,     !crippled);
        enable2(MENUOPT_MALZAK2,     !crippled);
        enable2(MENUOPT_PHUNSY,      !crippled);
        enable2(MENUOPT_SELBST,      !crippled);
        enable2(MENUOPT_MIKIT,       !crippled);
        enable2(MENUOPT_8550,        !crippled);
        enable2(MENUOPT_8600,        !crippled);
        enable2(MENUOPT_TYPERIGHT,   !crippled);
        checkmx(memmapinfo[memmap].menuopt, MENUOPT_ARCADIA, MENUOPT_TYPERIGHT);
        if (memmapinfo[memmap].menuopt != MENUOPT_ARCADIA    ) uncheckmx(MENUOPT_ARCADIA    );
        if (memmapinfo[memmap].menuopt != MENUOPT_INTERTON   ) uncheckmx(MENUOPT_INTERTON   );
        if (memmapinfo[memmap].menuopt != MENUOPT_ELEKTOR    ) uncheckmx(MENUOPT_ELEKTOR    );
        if (memmapinfo[memmap].menuopt != MENUOPT_INSTRUCTOR ) uncheckmx(MENUOPT_INSTRUCTOR );
        if (memmapinfo[memmap].menuopt != MENUOPT_TWIN       ) uncheckmx(MENUOPT_TWIN       );
        if (memmapinfo[memmap].menuopt != MENUOPT_PIPBUG     ) uncheckmx(MENUOPT_PIPBUG     );
        if (memmapinfo[memmap].menuopt != MENUOPT_BINBUG     ) uncheckmx(MENUOPT_BINBUG     );
        if (memmapinfo[memmap].menuopt != MENUOPT_CD2650     ) uncheckmx(MENUOPT_CD2650     );
        if (memmapinfo[memmap].menuopt != MENUOPT_ASTROWARS  ) uncheckmx(MENUOPT_ASTROWARS  );
        if (memmapinfo[memmap].menuopt != MENUOPT_GALAXIA    ) uncheckmx(MENUOPT_GALAXIA    );
        if (memmapinfo[memmap].menuopt != MENUOPT_LASERBATTLE) uncheckmx(MENUOPT_LASERBATTLE);
        if (memmapinfo[memmap].menuopt != MENUOPT_LAZARIAN   ) uncheckmx(MENUOPT_LAZARIAN   );
        if (memmapinfo[memmap].menuopt != MENUOPT_MALZAK1    ) uncheckmx(MENUOPT_MALZAK1    );
        if (memmapinfo[memmap].menuopt != MENUOPT_MALZAK2    ) uncheckmx(MENUOPT_MALZAK2    );
        if (memmapinfo[memmap].menuopt != MENUOPT_PHUNSY     ) uncheckmx(MENUOPT_PHUNSY     );
        if (memmapinfo[memmap].menuopt != MENUOPT_SELBST     ) uncheckmx(MENUOPT_SELBST     );
        if (memmapinfo[memmap].menuopt != MENUOPT_MIKIT      ) uncheckmx(MENUOPT_MIKIT      );
        if (memmapinfo[memmap].menuopt != MENUOPT_8550       ) uncheckmx(MENUOPT_8550       );
        if (memmapinfo[memmap].menuopt != MENUOPT_8600       ) uncheckmx(MENUOPT_8600       );
        if (memmapinfo[memmap].menuopt != MENUOPT_TYPERIGHT  ) uncheckmx(MENUOPT_TYPERIGHT  );
    // "Settings|Speed �" submenu
    acase MENUITEM_PAUSED:
        tick(which, (int) paused);
    acase MENUITEM_AUTOPAUSE:
        tick(which, autopause);
    acase MENUFAKE_REGION:
        enable2(MENUOPT_NTSC, (recmode == RECMODE_NORMAL && (machine == ARCADIA || machine == PONG)));
        enable2(MENUOPT_PAL , (recmode == RECMODE_NORMAL && (machine == ARCADIA || machine == PONG)));
        if (region == REGION_NTSC)
        {     checkmx(MENUOPT_NTSC, MENUOPT_NTSC, MENUOPT_PAL);
            uncheckmx(MENUOPT_PAL);
        } else
        {   // assert(region == REGION_PAL);
            uncheckmx(MENUOPT_NTSC);
              checkmx(MENUOPT_PAL,  MENUOPT_NTSC, MENUOPT_PAL);
        }
    acase MENUITEM_EXACTSPEED:
        tick(which, (int) exactspeed);
    acase MENUITEM_LIMITREFRESHES:
        tick(which, limitrefreshes);
    acase MENUITEM_TURBO:
        tick(which, (int) turbo);
#ifdef WIN32
    acase MENUFAKE_PRIORITY:
        switch (priority)
        {
        case PRI_HIGH:
            checkmx(MENUOPT_HIGHPRI,   MENUOPT_HIGHPRI, MENUOPT_LOWPRI);
            uncheckmx(MENUOPT_NORMALPRI);
            uncheckmx(MENUOPT_LOWPRI);
        acase PRI_NORMAL:
            uncheckmx(MENUOPT_HIGHPRI);
            checkmx(MENUOPT_NORMALPRI, MENUOPT_HIGHPRI, MENUOPT_LOWPRI);
            uncheckmx(MENUOPT_LOWPRI);
        acase PRI_LOW:
            uncheckmx(MENUOPT_HIGHPRI);
            uncheckmx(MENUOPT_NORMALPRI);
            checkmx(MENUOPT_LOWPRI,    MENUOPT_HIGHPRI, MENUOPT_LOWPRI);
        }
#endif
    acase MENUITEM_ADJUSTSPEED:
#ifdef AMIGA
        ghost(which, SubWindowPtr[SUBWINDOW_SPEED] != NULL);
#endif
#ifdef WIN32
        tick(which, SubWindowPtr[SUBWINDOW_SPEED] != NULL);
#endif
    // "Settings|Sprites �" submenu
    acase MENUITEM_COLLISIONS:
        ghost(which,
            (machines[machine].pvis == 0 && machine != ARCADIA && machine != PONG) // eg. PIPBUG, BINBUG, INSTRUCTOR, CD2650, PHUNSY
#ifdef WIN32
         || (cheevos && RA_HardcoreModeIsActive())
#endif
        );
        tick(which, (int) collisions);
    acase MENUFAKE_DEMULTIPLEX:
        enable2(MENUOPT_DEMULTIPLEX_MULTIPLEX  , machine == ARCADIA || machine == INTERTON || machine == ELEKTOR);
        enable2(MENUOPT_DEMULTIPLEX_TRANSPARENT, machine == ARCADIA || machine == INTERTON || machine == ELEKTOR);
        enable2(MENUOPT_DEMULTIPLEX_OPAQUE     , machine == ARCADIA || machine == INTERTON || machine == ELEKTOR);
        switch (demultiplex)
        {
        case 0:
              checkmx(MENUOPT_DEMULTIPLEX_MULTIPLEX  , MENUOPT_DEMULTIPLEX_MULTIPLEX, MENUOPT_DEMULTIPLEX_OPAQUE);
            uncheckmx(MENUOPT_DEMULTIPLEX_TRANSPARENT);
            uncheckmx(MENUOPT_DEMULTIPLEX_OPAQUE);
        acase 1:
            uncheckmx(MENUOPT_DEMULTIPLEX_MULTIPLEX);
              checkmx(MENUOPT_DEMULTIPLEX_TRANSPARENT, MENUOPT_DEMULTIPLEX_MULTIPLEX, MENUOPT_DEMULTIPLEX_OPAQUE);
            uncheckmx(MENUOPT_DEMULTIPLEX_OPAQUE);
        acase 2:
            uncheckmx(MENUOPT_DEMULTIPLEX_MULTIPLEX);
            uncheckmx(MENUOPT_DEMULTIPLEX_TRANSPARENT);
              checkmx(MENUOPT_DEMULTIPLEX_OPAQUE     , MENUOPT_DEMULTIPLEX_MULTIPLEX, MENUOPT_DEMULTIPLEX_OPAQUE);
        }
    // "Settings|Sound �" submenu
    acase MENUITEM_SOUND:
        tick(which, (int) sound);
    acase MENUITEM_AMBIENT:
        ghost(which,
            machine != PIPBUG
         && machine != BINBUG
         && machine != TWIN
         && machine != CD2650
         && machine != SELBST
         && !machines[machine].coinop
        );
        tick(which, ambient);
#ifdef AMIGA
    acase MENUITEM_FILTERED:
        ghost(which, (int) ng);
        tick(which, led);
#endif
    acase MENUITEM_RETUNE:
        ghost(which,
            machine != ARCADIA
         && machines[machine].pvis == 0
         && machine != PONG
         && machine != MIKIT
        ); // eg. PIPBUG, BINBUG, INSTRUCTOR, CD2650, PHUNSY, SELBST
        tick(which, retune);
#ifdef AMIGA
    acase MENUITEM_SOUNDOUTPUT:
        switch (soundoutput)
        {
        case SOUNDOUTPUT_AHI:
              checkmx(MENUOPT_SOUND_AHI,   MENUOPT_SOUND_AHI, MENUOPT_SOUND_PAULA);
            uncheckmx(MENUOPT_SOUND_PAULA);
        acase SOUNDOUTPUT_PAULA:
            uncheckmx(MENUOPT_SOUND_AHI);
              checkmx(MENUOPT_SOUND_PAULA, MENUOPT_SOUND_AHI, MENUOPT_SOUND_PAULA);
        }
#endif
    // "Settings|Trainers �" submenu
    acase MENUITEM_CHEATLIVES:
        ghost(which, machine != ARCADIA && machine != INTERTON && machine != ELEKTOR && !machines[machine].coinop);
        tick(which, trainer_lives);
    acase MENUITEM_CHEATTIME:
        ghost(which, machine != ARCADIA && machine != INTERTON && machine != ELEKTOR && !machines[machine].coinop && machine != PIPBUG && machine != BINBUG);
        tick(which, trainer_time);
    acase MENUITEM_INVINCIBILITY:
        ghost(which, machine != ARCADIA && machine != INTERTON && machine != ELEKTOR && !machines[machine].coinop);
        tick(which, trainer_invincibility);
    acase MENUITEM_LEVELSKIP:
        ghost
        (   which,
            (machine != ARCADIA && memmap != MEMMAP_ASTROWARS && memmap != MEMMAP_GALAXIA
#ifdef ADVENTURETRAINER
                                                                                          && memmap != MEMMAP_BINBUG
#endif
                                                                                                                    )
#ifdef WIN32
         || (cheevos && RA_HardcoreModeIsActive())
#endif
        );
    // "Settings|VDU �" submenu
    acase MENUFAKE_PIPBUGVDU:
        enable2(MENUOPT_ELEKTERMINAL,  machine == PIPBUG);
        enable2(MENUOPT_LCVDU_NARROW,  machine == PIPBUG);
        enable2(MENUOPT_LCVDU_WIDE,    machine == PIPBUG);
        enable2(MENUOPT_RADIOBULLETIN, machine == PIPBUG);
        enable2(MENUOPT_SVT100,        machine == PIPBUG);
        enable2(MENUOPT_VT100,         machine == PIPBUG);
        switch (pipbug_vdu)
        {
        case VDU_ELEKTERMINAL:
              checkmx(MENUOPT_ELEKTERMINAL, MENUOPT_ELEKTERMINAL, MENUOPT_VT100);
            uncheckmx(MENUOPT_LCVDU_NARROW);
            uncheckmx(MENUOPT_LCVDU_WIDE);
            uncheckmx(MENUOPT_RADIOBULLETIN);
            uncheckmx(MENUOPT_SVT100);
            uncheckmx(MENUOPT_VT100);
        acase VDU_LCVDU_NARROW:
            uncheckmx(MENUOPT_ELEKTERMINAL);
              checkmx(MENUOPT_LCVDU_NARROW, MENUOPT_ELEKTERMINAL, MENUOPT_VT100);
            uncheckmx(MENUOPT_LCVDU_WIDE);
            uncheckmx(MENUOPT_RADIOBULLETIN);
            uncheckmx(MENUOPT_SVT100);
            uncheckmx(MENUOPT_VT100);
        acase VDU_LCVDU_WIDE:
            uncheckmx(MENUOPT_ELEKTERMINAL);
            uncheckmx(MENUOPT_LCVDU_NARROW);
              checkmx(MENUOPT_LCVDU_WIDE,   MENUOPT_ELEKTERMINAL, MENUOPT_VT100);
            uncheckmx(MENUOPT_RADIOBULLETIN);
            uncheckmx(MENUOPT_SVT100);
            uncheckmx(MENUOPT_VT100);
        acase VDU_RADIOBULLETIN:
            uncheckmx(MENUOPT_ELEKTERMINAL);
            uncheckmx(MENUOPT_LCVDU_NARROW);
            uncheckmx(MENUOPT_LCVDU_WIDE);
              checkmx(MENUOPT_RADIOBULLETIN,MENUOPT_ELEKTERMINAL, MENUOPT_VT100);
            uncheckmx(MENUOPT_SVT100);
            uncheckmx(MENUOPT_VT100);
        acase VDU_SVT100:
            uncheckmx(MENUOPT_ELEKTERMINAL);
            uncheckmx(MENUOPT_LCVDU_NARROW);
            uncheckmx(MENUOPT_LCVDU_WIDE);
            uncheckmx(MENUOPT_RADIOBULLETIN);
              checkmx(MENUOPT_SVT100,       MENUOPT_ELEKTERMINAL, MENUOPT_VT100);
            uncheckmx(MENUOPT_VT100);
        acase VDU_VT100:
            uncheckmx(MENUOPT_ELEKTERMINAL);
            uncheckmx(MENUOPT_LCVDU_NARROW);
            uncheckmx(MENUOPT_LCVDU_WIDE);
            uncheckmx(MENUOPT_RADIOBULLETIN);
            uncheckmx(MENUOPT_SVT100);
              checkmx(MENUOPT_VT100,        MENUOPT_ELEKTERMINAL, MENUOPT_VT100);
        }
    acase MENUFAKE_CD2650VDU:
        enable2(MENUOPT_ASCIIVDU,     machine == CD2650);
        enable2(MENUOPT_CHESSMEN,     machine == CD2650);
        enable2(MENUOPT_LOWERCASEVDU, machine == CD2650);
        switch (cd2650_vdu)
        {
        case  VDU_ASCII:
              checkmx(MENUOPT_ASCIIVDU    , MENUOPT_ASCIIVDU, MENUOPT_LOWERCASEVDU);
            uncheckmx(MENUOPT_CHESSMEN);
            uncheckmx(MENUOPT_LOWERCASEVDU);
        acase VDU_CHESSMEN:
            uncheckmx(MENUOPT_ASCIIVDU);
              checkmx(MENUOPT_CHESSMEN    , MENUOPT_ASCIIVDU, MENUOPT_LOWERCASEVDU);
            uncheckmx(MENUOPT_LOWERCASEVDU);
        acase VDU_LOWERCASE:
            uncheckmx(MENUOPT_ASCIIVDU);
            uncheckmx(MENUOPT_CHESSMEN);
              checkmx(MENUOPT_LOWERCASEVDU, MENUOPT_ASCIIVDU, MENUOPT_LOWERCASEVDU);
        }
#ifdef WIN32
    acase MENUITEM_BEZEL:
        ghost(which, !BEZELABLE);
        tick(which, bezel);
#endif
    acase MENUITEM_BLINK:
        ghost(which, machine != PIPBUG && machine != TWIN && machine != SELBST);
        tick(which, blink);
    acase MENUITEM_COOMER:
        ghost(which, machine != BINBUG);
        tick(which, coomer);
    // "Help" menu
    acase MENUITEM_GAMEINFO:
        ghost(which,
            (   whichgame == -1
             && !strcmp((const char*) autotext[AUTOLINE_GAMENAME], "-")
            )
#ifdef AMIGA
             || SubWindowPtr[SUBWINDOW_GAMEINFO]
#endif
            );
#ifdef WIN32
            tick(which, SubWindowPtr[SUBWINDOW_GAMEINFO] != NULL);
#endif
    acase MENUITEM_HOSTPADS:
        ghost(which,
            (   machine != ARCADIA
             && machine != PONG
             && !machines[machine].pvis
            )
#ifdef AMIGA
         || SubWindowPtr[SUBWINDOW_HOSTPADS]
#endif
        );
#ifdef WIN32
        tick(which, SubWindowPtr[SUBWINDOW_HOSTPADS] != NULL);
#endif
    acase MENUITEM_HOSTKYBD:
#ifdef AMIGA
        ghost(which, SubWindowPtr[SUBWINDOW_HOSTKYBD] != NULL);
#endif
#ifdef WIN32
        tick(which, SubWindowPtr[SUBWINDOW_HOSTKYBD] != NULL);
#endif
    acase MENUITEM_OPCODES:
#ifdef AMIGA
        ghost(which, machine == PONG || machine == TYPERIGHT || SubWindowPtr[SUBWINDOW_OPCODES]);
#endif
#ifdef WIN32
        ghost(which, machine == PONG || machine == TYPERIGHT);
        tick(which, SubWindowPtr[SUBWINDOW_OPCODES] != NULL);
#endif
    acase MENUITEM_CODINGGUIDE:
#ifdef AMIGA
        ghost(which, machine == PONG || machine == TYPERIGHT || !OpenURLBase);
#endif
#ifdef WIN32
        ghost(which, machine == PONG || machine == TYPERIGHT);
#endif
    acase MENUITEM_GAMINGGUIDE:
#ifdef AMIGA
        ghost(which, machine == PONG || !OpenURLBase);
#endif
#ifdef WIN32
        ghost(which, machine == PONG);
#endif
#ifdef AMIGA
    acase MENUITEM_MANUAL:
        ghost(which, !guideexists || (morphos && !AmigaGuideBase));
    acase MENUITEM_UPDATE:
        ghost(which, !SocketBase);
#endif
}   }

EXPORT void updatesmlgad(int which, ULONG state, FLAG now)
{   if (!MainWindowPtr || !showtoolbars[wsm])
    {   return; // important!
    }

#ifdef AMIGA
    if (now)
    {   detachsmlgads();
    }
#endif

    switch (which)
    {
    case GADPOS_AUTOFIRE1:
    case GADPOS_AUTOFIRE2:
        if
        (   machine == ARCADIA
         || machine == BINBUG
         || machines[machine].pvis
         || memmap == MEMMAP_8600
        )
        {   setbutton(which, TRUE,  (FLAG) state);
        } else
        {   setbutton(which, FALSE, (FLAG) state);
        }
    acase GADPOS_COLLISIONS:
        if
        (   (machines[machine].pvis || machine == ARCADIA || machine == PONG)
#ifdef WIN32
         && (!cheevos || !RA_HardcoreModeIsActive())
#endif
        )
        {   setbutton(which, TRUE,  (FLAG) state);
        } else
        {   setbutton(which, FALSE, (FLAG) state);
        }
    acase GADPOS_SWAPPED:
        if (machines[machine].pvis || machine == ARCADIA || machine == BINBUG || machine == PONG)
        {   setbutton(which, TRUE,  (FLAG) state);
        } else
        {   setbutton(which, FALSE, (FLAG) state);
        }
    acase GADPOS_PAUSED:
    case GADPOS_SOUND:
    case GADPOS_TURBO:
        setbutton(which, TRUE, (FLAG) state);
    acase GADPOS_RECORD:
        if (!crippled && (recmode == RECMODE_PLAY || recmode == RECMODE_NORMAL))
        {   setbutton(which, TRUE,  FALSE);
        } elif (!crippled && recmode == RECMODE_RECORD)
        {   setbutton(which, FALSE, TRUE);
        } else
        {   setbutton(which, FALSE, FALSE);
        }
    acase GADPOS_PLAY:
        if (!crippled && recmode == RECMODE_PLAY)
        {   setbutton(which, TRUE,  TRUE);
        } else
        {   setbutton(which, FALSE, FALSE);
        }
    acase GADPOS_STOP:
        if (!crippled && (recmode == RECMODE_RECORD || recmode == RECMODE_PLAY))
        {   setbutton(which, TRUE,  FALSE);
        } elif (!crippled && recmode == RECMODE_NORMAL)
        {   setbutton(which, FALSE, TRUE);
        } else
        {   setbutton(which, FALSE, FALSE);
    }   }

#ifdef AMIGA
    if (now)
    {   attachsmlgads();
    }
#endif
}

EXPORT void updatesmlgads(void)
{   if (!MainWindowPtr)
    {   return; // important!
    }

#ifdef AMIGA
    if (!showtoolbars[wsm])
    {   return;
    }
    detachsmlgads();
#endif

    // controllers
    updatesmlgad(GADPOS_AUTOFIRE1  , autofire[0], FALSE);
    updatesmlgad(GADPOS_AUTOFIRE2  , autofire[1], FALSE);
    updatesmlgad(GADPOS_SWAPPED    , swapped    , FALSE);
    // sound
    updatesmlgad(GADPOS_SOUND      , sound      , FALSE);
    // speed
    updatesmlgad(GADPOS_PAUSED     , paused     , FALSE);
    updatesmlgad(GADPOS_TURBO      , turbo      , FALSE);
    // sprites
    updatesmlgad(GADPOS_COLLISIONS , collisions , FALSE);
    // macros
    updatesmlgad(GADPOS_RECORD     , 0          , FALSE);
    updatesmlgad(GADPOS_PLAY       , 0          , FALSE);
    updatesmlgad(GADPOS_STOP       , 0          , FALSE);

    if (machine == PONG)
    {   machines[PONG].consolekeyimage[0] = (memmap == MEMMAP_8550) ? IMAGE_START2 : IMAGE_START;
    }

#ifdef WIN32
    DISCARD SendMessage(hToolbar, TB_CHANGEBITMAP, (UINT) ID_CONSOLE_START, machines[machine].consolekeyimage[0]);
    DISCARD SendMessage(hToolbar, TB_CHANGEBITMAP, (UINT) ID_CONSOLE_A,     machines[machine].consolekeyimage[1]);
    DISCARD SendMessage(hToolbar, TB_CHANGEBITMAP, (UINT) ID_CONSOLE_B,     machines[machine].consolekeyimage[2]);
    DISCARD SendMessage(hToolbar, TB_CHANGEBITMAP, (UINT) ID_CONSOLE_RESET, machines[machine].consolekeyimage[3]);

    DISCARD SendMessage(hToolbar, TB_SETSTATE    , (UINT) ID_CONSOLE_START, (recmode != RECMODE_PLAY && memmapinfo[memmap].gadenabled[0]) ? TBSTATE_ENABLED : 0);
    DISCARD SendMessage(hToolbar, TB_SETSTATE    , (UINT) ID_CONSOLE_A    , (recmode != RECMODE_PLAY && memmapinfo[memmap].gadenabled[1]) ? TBSTATE_ENABLED : 0);
    DISCARD SendMessage(hToolbar, TB_SETSTATE    , (UINT) ID_CONSOLE_B    , (recmode != RECMODE_PLAY && memmapinfo[memmap].gadenabled[2]) ? TBSTATE_ENABLED : 0);
    DISCARD SendMessage(hToolbar, TB_SETSTATE    , (UINT) ID_CONSOLE_RESET, (recmode != RECMODE_PLAY && memmapinfo[memmap].gadenabled[3]) ? TBSTATE_ENABLED : 0);
#endif
#ifdef AMIGA
    SetSpeedButtonNodeAttrs(SmallSpeedBarNodePtr[GADPOS_START], SBNA_Image, images[machines[machine].consolekeyimage[0]], TAG_DONE);
    SetSpeedButtonNodeAttrs(SmallSpeedBarNodePtr[GADPOS_A    ], SBNA_Image, images[machines[machine].consolekeyimage[1]], TAG_DONE);
    SetSpeedButtonNodeAttrs(SmallSpeedBarNodePtr[GADPOS_B    ], SBNA_Image, images[machines[machine].consolekeyimage[2]], TAG_DONE);
    SetSpeedButtonNodeAttrs(SmallSpeedBarNodePtr[GADPOS_RESET], SBNA_Image, images[machines[machine].consolekeyimage[3]], TAG_DONE);

    SetSpeedButtonNodeAttrs(SmallSpeedBarNodePtr[GADPOS_START], SBNA_Disabled, (recmode != RECMODE_PLAY && memmapinfo[memmap].gadenabled[0]) ? FALSE : TRUE, TAG_DONE);
    SetSpeedButtonNodeAttrs(SmallSpeedBarNodePtr[GADPOS_A    ], SBNA_Disabled, (recmode != RECMODE_PLAY && memmapinfo[memmap].gadenabled[1]) ? FALSE : TRUE, TAG_DONE);
    SetSpeedButtonNodeAttrs(SmallSpeedBarNodePtr[GADPOS_B    ], SBNA_Disabled, (recmode != RECMODE_PLAY && memmapinfo[memmap].gadenabled[2]) ? FALSE : TRUE, TAG_DONE);
    SetSpeedButtonNodeAttrs(SmallSpeedBarNodePtr[GADPOS_RESET], SBNA_Disabled, (recmode != RECMODE_PLAY && memmapinfo[memmap].gadenabled[3]) ? FALSE : TRUE, TAG_DONE);

    attachsmlgads();
#endif
}

EXPORT void handle_menu(int command)
{   int i;

    switch (command)
    {
    // "Project" menu
    case MENUITEM_RESET:
        project_reset(FALSE);
    acase MENUITEM_RESETTOMONITOR:
        docommand(command);
    acase MENUITEM_REINITIALIZE:
        docommand(MENUITEM_REINITIALIZE);
    acase MENUITEM_OPEN:
        project_open();
    acase MENUITEM_REVERT:
    case MENUITEM_QUICKLOAD:
        docommand(command);
    acase MENUITEM_SAVESNP:
        project_save(KIND_COS);
    acase MENUITEM_SAVEACBM:  savescreenshot(KIND_ACBM, TRUE, FALSE, FALSE);
    acase MENUITEM_SAVEILBM:  savescreenshot(KIND_ILBM, TRUE, FALSE, FALSE);
    acase MENUITEM_SAVEBMP:   savescreenshot(KIND_BMP , TRUE, FALSE, FALSE);
    acase MENUITEM_SAVEGIF:   savescreenshot(KIND_GIF , TRUE, FALSE, FALSE);
    acase MENUITEM_SAVEPCX:   savescreenshot(KIND_PCX , TRUE, FALSE, FALSE);
    acase MENUITEM_SAVEPNG:   savescreenshot(KIND_PNG , TRUE, FALSE, FALSE);
    acase MENUITEM_SAVETIFF:  savescreenshot(KIND_TIFF, TRUE, FALSE, FALSE);
    acase MENUITEM_SAVEASCII: edit_savetext(FALSE, TRUE, -1, FALSE);
    acase MENUITEM_SAVEASM:
        saveasmscrnshot();
    acase MENUITEM_QUICKSAVE:
        docommand(command);
    acase MENUITEM_AUDIT:
        audit(TRUE);
#ifdef WIN32
    acase MENUITEM_REGISTER:
        project_register();
#endif
    acase MENUITEM_SERVER:
    case MENUITEM_CLIENT:
        docommand(command);
    acase MENUITEM_RECENT1: strcpy((char*) fn_game, (const char*) &recent[0][3]); engine_load(TRUE);
    acase MENUITEM_RECENT2: strcpy((char*) fn_game, (const char*) &recent[1][3]); engine_load(TRUE);
    acase MENUITEM_RECENT3: strcpy((char*) fn_game, (const char*) &recent[2][3]); engine_load(TRUE);
    acase MENUITEM_RECENT4: strcpy((char*) fn_game, (const char*) &recent[3][3]); engine_load(TRUE);
    acase MENUITEM_RECENT5: strcpy((char*) fn_game, (const char*) &recent[4][3]); engine_load(TRUE);
    acase MENUITEM_RECENT6: strcpy((char*) fn_game, (const char*) &recent[5][3]); engine_load(TRUE);
    acase MENUITEM_RECENT7: strcpy((char*) fn_game, (const char*) &recent[6][3]); engine_load(TRUE);
    acase MENUITEM_RECENT8: strcpy((char*) fn_game, (const char*) &recent[7][3]); engine_load(TRUE);
    acase MENUITEM_ICONIFY:
        iconify();
#ifdef WIN32
    acase MENUITEM_UNICONIFY:
        uniconify(FALSE);
#endif
    acase MENUITEM_QUIT:
        if (confirm())
        {
#ifdef AMIGA
            pending |= PENDING_QUIT;
#endif
#ifdef WIN32
            cleanexit(EXIT_SUCCESS);
#endif
        }
    // "Edit" menu
    acase MENUITEM_COPYSCRN:
#ifdef AMIGA
        savescreenshot(KIND_ILBM, FALSE, TRUE, FALSE);
#endif
#ifdef WIN32
        savescreenshot(KIND_BMP, FALSE, TRUE, FALSE);
#endif
    acase MENUITEM_COPYTEXT:
        edit_savetext(TRUE, FALSE, -1, FALSE);
    acase MENUITEM_PASTETEXT:
        edit_pastetext();
    acase MENUITEM_EMPTYCLIPBOARD:
        emptyclipboard();
    // "View" menu
    acase MENUITEM_VIEWHISCORES:
        view_hiscores();
    acase MENUITEM_VIEWDEBUGGER:
        flipbool(&showdebugger[wsm], command, TRUE);
    acase MENUITEM_MENUBAR:
        flipbool(&showmenubars[wsm], command, TRUE);
    acase MENUITEM_POINTER:
        flipbool(&showpointers[wsm], command, TRUE);
    acase MENUITEM_TITLEBAR:
        flipbool(&showtitlebars[wsm], command, TRUE);
    acase MENUITEM_TOOLBAR:
        flipbool(&showtoolbars[wsm], command, TRUE);
    acase MENUITEM_SIDEBAR:
        docommand(MENUITEM_SIDEBAR);
    acase MENUITEM_STATUSBAR:
        flipbool(&showstatusbars[wsm], command, TRUE);
    acase MENUITEM_BUILTIN:
        flipbool(&showbuiltin, command, FALSE);
        audit(FALSE);
    // "Macros" menu
    acase MENUITEM_STARTRECORDING:
        project_save(KIND_COR);
    acase MENUITEM_RESTARTPLAYBACK:
        macro_restartplayback();
    acase MENUITEM_STOP:
        macro_stop();
    acase MENUITEM_GENERATE:
        flipbool(&generate, command, TRUE);
    acase MENUITEM_LOOP:
        flipbool(&loop, command, TRUE);
    acase MENUITEM_APNG:
        flipbool(&apnganims, command, FALSE);
    acase MENUITEM_MNG:
        flipbool(&mnganims, command, FALSE);
    acase MENUITEM_IFFANIMS:
        flipbool(&iffanims, command, FALSE);
    acase MENUITEM_GIFANIMS:
        flipbool(&gifanims, command, FALSE);
    acase MENUITEM_GENERATE8SVX:
        flipbool(&esvxfile, command, FALSE);
    acase MENUITEM_GENERATEAIFF:
        flipbool(&aifffile, command, FALSE);
    acase MENUITEM_GENERATEMIDI:
        flipbool(&midifile, command, FALSE);
    acase MENUITEM_GENERATEPSG:
        flipbool(&psgfile, command, FALSE);
    acase MENUITEM_GENERATESMUS:
        flipbool(&smusfile, command, FALSE);
    acase MENUITEM_GENERATEWAV:
        flipbool(&wavfile, command, FALSE);
    acase MENUITEM_GENERATEYM:
        flipbool(&ymfile, command, FALSE);
    acase MENUITEM_RUNREXX:
        macro_rexx(TRUE, FALSE);
    acase MENUITEM_REPEATREXX:
        macro_rexx(FALSE, FALSE);
    // "Debug �|General" submenu
    acase MENUITEM_QUOTES:     debugger_partial("\""        );
    acase MENUITEM_CLEARCOV:   debugger_full(   "CLEARCOV"  );
    acase MENUITEM_CLS:        debugger_full(   "CLS"       );
    acase MENUITEM_HELP:       debugger_partial("HELP "     );
    acase MENUITEM_SAY:        debugger_partial("SAY "      );
    acase MENUITEM_SYSTEM:     debugger_partial("SYSTEM"    ); // partial for safety
    // "Debug �|File" submenu
    acase MENUITEM_ASTERISK:   debugger_partial("*"         );
    acase MENUITEM_ASM:        debugger_partial("ASM "      );
    acase MENUITEM_DISGAME:    debugger_partial("DISGAME "  );
    acase MENUITEM_EDIT:       debugger_partial("EDIT "     );
    acase MENUITEM_LOADBIN:    debugger_partial("LOADBIN "  );
    acase MENUITEM_SAVEBIN:    debugger_partial("SAVEBIN "  );
    acase MENUITEM_SAVEAOF:    if   (machine == ELEKTOR) debugger_partial("SAVEEOF " );
                               else                      debugger_partial("SAVEAOF " );
    acase MENUITEM_SAVEBPNF:   debugger_partial("SAVEBPNF " );
    acase MENUITEM_SAVECMD:    if   (machine == BINBUG ) debugger_partial("SAVECMD " );
                               elif (machine == TWIN   ) debugger_partial("SAVEMOD " );
                               elif (machine == CD2650 ) debugger_partial("SAVEIMAG ");
    acase MENUITEM_SAVEHEX:    debugger_partial("SAVEHEX "  );
    acase MENUITEM_SAVESMS:    debugger_partial("SAVESMS "  );
    acase MENUITEM_SAVETVC:    debugger_partial("SAVETVC "  );
    // "Debug �|Disk" submenu
    acase MENUITEM_DELETE:     debugger_partial("DELETE "   );
    acase MENUITEM_DIR:        debugger_full(   "DIR"       );
    acase MENUITEM_EXTRACT:    debugger_partial("EXTRACT "  );
    acase MENUITEM_INJECT:     debugger_partial("INJECT "   );
    acase MENUITEM_RENAME:     debugger_partial("RENAME "   );
    // "Debug �|Edit" submenu
    acase MENUITEM_DOKE:       debugger_partial("DOKE "     );
    acase MENUITEM_POKE:       debugger_partial("POKE "     );
    acase MENUITEM_FPOKE:      debugger_partial("FPOKE "    );
    acase MENUITEM_WRITEPORT:  debugger_partial("WRITEPORT ");
    // "Debug �|View" submenu
    acase MENUITEM_EQUALS:     debugger_partial("= "        );
    acase MENUITEM_COVER:      debugger_partial("COVER "    );
    acase MENUITEM_PEEK:       debugger_partial("PEEK "     );
    acase MENUITEM_DIS:        debugger_partial("DIS "      );
    acase MENUITEM_ERROR:      debugger_partial("ERROR "    );
    acase MENUITEM_FPEEK:      debugger_partial("FPEEK "    );
    acase MENUITEM_HISTORY:    debugger_full(   "HISTORY"   );
    acase MENUITEM_IM:         debugger_partial("IM "       );
    acase MENUITEM_READPORT:   debugger_partial("READPORT " );
    acase MENUITEM_VIEW_BASIC: debugger_full(   "VIEW BASIC");
    acase MENUITEM_VIEW_BIOS:  debugger_full(   "VIEW BIOS" );
    acase MENUITEM_VIEW_CPU:   debugger_full(   "VIEW CPU"  );
    acase MENUITEM_VIEW_PSG:   debugger_full(   "VIEW PSG"  );
    acase MENUITEM_VIEW_RAM:   debugger_full(   "VIEW RAM"  );
    acase MENUITEM_VIEW_SCRN:  debugger_full(   "VIEW SCRN" );
    acase MENUITEM_VIEW_UDG:   debugger_full(   "VIEW UDG"  );
    acase MENUITEM_VIEW_XVI:   debugger_full(   "VIEW XVI"  );
    // "Debug �|Log" submenu
    acase MENUITEM_L_A:        debugger_full(   "L A"       );
    acase MENUITEM_L_B:        debugger_full(   "L B"       );
    acase MENUITEM_L_C:        debugger_full(   "L C"       );
    acase MENUITEM_L_I:        debugger_full(   "L I"       );
    acase MENUITEM_L_N:        debugger_full(   "L N"       );
    acase MENUITEM_L_S:        debugger_full(   "L S"       );
    acase MENUITEM_PL:         debugger_full(   "PL"        );
    acase MENUITEM_T:          debugger_full(   "T"         );
    // "Debug �|Run" submenu
    acase MENUITEM_G:          if (machine == PONG || machine == TYPERIGHT)
                               {   debugger_full("G");
                               } else
                               {   debugger_partial("G ");
                               }
    acase MENUITEM_GI:         debugger_full(   "GI"        );
    acase MENUITEM_I:          debugger_full(   "I"         );
    acase MENUITEM_JUMP:       debugger_partial("JUMP "     );
    acase MENUITEM_O:          debugger_full(   "O"         );
    acase MENUITEM_S:          debugger_full(   "S"         );
    acase MENUITEM_R:          debugger_partial("R "        );
    acase MENUITEM_R_F:        debugger_full(   "R F"       );
    acase MENUITEM_R_I:        debugger_full(   "R I"       );
    acase MENUITEM_R_L:        debugger_full(   "R L"       );
    acase MENUITEM_R_R:        debugger_full(   "R R"       );
    acase MENUITEM_R_S:        debugger_full(   "R S"       );
    // "Debug �|Breakpoints" submenu
    acase MENUITEM_BP:         debugger_partial("BP "       );
    acase MENUITEM_BC:         debugger_partial("BC "       );
    acase MENUITEM_BL:         debugger_full(   "BL"        );
    acase MENUITEM_FP:         debugger_partial("FP "       );
    acase MENUITEM_FC:         debugger_partial("FC "       );
    acase MENUITEM_FL:         debugger_full(   "FL"        );
    acase MENUITEM_IP:         debugger_partial("IP "       );
    acase MENUITEM_IC:         debugger_partial("IC "       );
    acase MENUITEM_IL:         debugger_full(   "IL"        );
    acase MENUITEM_WP:         debugger_partial("WP "       );
    acase MENUITEM_WC:         debugger_partial("WC "       );
    acase MENUITEM_WL:         debugger_full(   "WL"        );
    acase MENUITEM_PB:         debugger_full(   "PB"        );
    acase MENUITEM_WR:         debugger_full(   "WR"        );
    // "Debug �|Symbols" submenu
    acase MENUITEM_CLEARSYM:   debugger_partial("CLEARSYM " );
    acase MENUITEM_DEFSYM:     debugger_partial("DEFSYM "   );
    acase MENUITEM_LOADSYM:    debugger_partial("LOADSYM "  );
    acase MENUITEM_SAVESYM:    debugger_partial("SAVESYM "  );
    // "Debug �|Tools" submenu
    acase MENUITEM_COMP:       debugger_partial("COMP "     );
    acase MENUITEM_COPY:       debugger_partial("COPY "     );
    acase MENUITEM_FILL:       debugger_partial("FILL "     );
    acase MENUITEM_FIND:       debugger_partial("FIND "     );
    acase MENUITEM_REL:        debugger_partial("REL "      );
    acase MENUITEM_SWAP:       debugger_partial("SWAP "     );
    acase MENUITEM_TRAIN:      debugger_partial("TRAIN "    );
    // "Debug �|Options" submenu
    acase MENUITEM_GR:         debugger_full(   "GR"        );
    acase MENUITEM_WARN:       debugger_full(   "WARN"      );
    // "Tools" menu
    acase MENUITEM_CONTROLS:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_CONTROLS])
        {   close_subwindow(SUBWINDOW_CONTROLS);
        } else
        {   view_controls();
        }
#endif
#ifdef AMIGA
        view_controls();
#endif
    acase MENUITEM_DIPSWITCHES:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_DIPS])
        {   close_subwindow(SUBWINDOW_DIPS);
        } else
        {   edit_dips();
        }
#endif
#ifdef AMIGA
        edit_dips();
#endif
    acase MENUITEM_FLOPPYDRIVE:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_FLOPPYDRIVE])
        {   close_subwindow(SUBWINDOW_FLOPPYDRIVE);
        } else
        {   open_floppydrive(TRUE);
        }
#endif
#ifdef AMIGA
        open_floppydrive(TRUE);
#endif
    acase MENUITEM_EDITMEMORY:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_MEMORY])
        {   close_subwindow(SUBWINDOW_MEMORY);
        } else
        {   edit_memory();
        }
#endif
#ifdef AMIGA
        edit_memory();
#endif
    acase MENUITEM_MUSIC:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_MUSIC])
        {   close_subwindow(SUBWINDOW_MUSIC);
        } else
        {   tools_music();
        }
#endif
#ifdef AMIGA
        tools_music();
#endif
    acase MENUITEM_PALETTE:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_PALETTE])
        {   close_subwindow(SUBWINDOW_PALETTE);
        } else
        {   edit_palette();
        }
#endif
#ifdef AMIGA
        edit_palette();
#endif
    acase MENUITEM_PRINTER:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_PRINTER])
        {   close_subwindow(SUBWINDOW_PRINTER);
        } else
        {   tools_printer();
        }
#endif
#ifdef AMIGA
        tools_printer();
#endif
    acase MENUITEM_SPRITEVIEWER:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_SPRITES])
        {   close_subwindow(SUBWINDOW_SPRITES);
        } else
        {   open_spriteeditor();
        }
#endif
#ifdef AMIGA
        open_spriteeditor();
#endif
    acase MENUITEM_TAPEDECK:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_TAPEDECK])
        {   close_subwindow(SUBWINDOW_TAPEDECK);
        } else
        {   open_tapedeck();
        }
#endif
#ifdef AMIGA
        open_tapedeck();
#endif
    acase MENUITEM_INDUSTRIAL:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_INDUSTRIAL])
        {   close_subwindow(SUBWINDOW_INDUSTRIAL);
        } else
        {   open_industrial();
        }
#endif
#ifdef AMIGA
        open_industrial();
#endif
    acase MENUITEM_PAPERTAPE:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_PAPERTAPE])
        {   close_subwindow(SUBWINDOW_PAPERTAPE);
        } else
        {   open_papertape();
        }
#endif
#ifdef AMIGA
        open_papertape();
#endif
    acase MENUITEM_TOOLSDEBUGGER:
        docommand(command);
    acase MENUITEM_SCREENEDITOR:
        docommand(command);
    acase MENUITEM_VIEWMONITOR_CPU:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_MONITOR_CPU])
        {   close_subwindow(SUBWINDOW_MONITOR_CPU);
        } else
        {   view_monitor(SUBWINDOW_MONITOR_CPU);
        }
#endif
#ifdef AMIGA
        view_monitor(SUBWINDOW_MONITOR_CPU);
#endif
    acase MENUITEM_VIEWMONITOR_PSGS:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_MONITOR_PSGS])
        {   close_subwindow(SUBWINDOW_MONITOR_PSGS);
        } else
        {   view_monitor(SUBWINDOW_MONITOR_PSGS);
        }
#endif
#ifdef AMIGA
        view_monitor(SUBWINDOW_MONITOR_PSGS);
#endif
    acase MENUITEM_VIEWMONITOR_XVI:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_MONITOR_XVI])
        {   close_subwindow(SUBWINDOW_MONITOR_XVI);
        } else
        {   view_monitor(SUBWINDOW_MONITOR_XVI);
        }
#endif
#ifdef AMIGA
        view_monitor(SUBWINDOW_MONITOR_XVI);
#endif
    // "Settings|Left controller �" submenu
    acase MENUITEM_AUTOFIRE1:
        flipbool((int*) &autofire[0], command, TRUE);
    acase MENUITEM_REQUIREBUTTON1:
        flipbool(&requirebutton[0], command, FALSE);
    acase MENUITEM_FORCEFEEDBACK1:
        flipbool(&useff[0], command, FALSE);
        if (!useff[0])
        {   ff_off();
        }
    // "Settings|Right controller �" submenu
    acase MENUITEM_AUTOFIRE2:
        flipbool((int*) &autofire[1], command, TRUE);
    acase MENUITEM_REQUIREBUTTON2:
        flipbool(&requirebutton[1], command, FALSE);
    acase MENUITEM_FORCEFEEDBACK2:
        flipbool(&useff[1], command, FALSE);
        if (!useff[1])
        {   ff_off();
        }
    // "Settings|Colours �" submenu
    acase MENUITEM_UNDITHER:
        flipbool(&undither, command, TRUE);
    acase MENUITEM_DARKENBG:
        flipbool(&darkenbg, command, TRUE);
    acase MENUITEM_FLAGLINE:
        flipbool(&flagline, command, TRUE);
    acase MENUITEM_INVERSE:
        flipbool(&inverse, command, FALSE);
        fixupcolours();
        redrawscreen();
    // "Settings|Emulator �" submenu
    acase MENUITEM_AUTOSAVE:
        flipbool(&autosave, command, FALSE);
    acase MENUITEM_CONFIRM:
        flipbool((int*) &confirmable, command, FALSE);
#ifdef AMIGA
    acase MENUITEM_CREATEICONS:
        flipbool(&icons, command, FALSE);
#endif
#ifdef WIN32
    acase MENUITEM_CHEEVOS1:
        flipbool(&cheevos, command, FALSE);
        if (cheevos)
        {   init_cheevos();
        } else
        {   remove_cheevos(TRUE);
        }
#endif
    acase MENUITEM_EMUID:
        flipbool(&emuid, command, FALSE);
    acase MENUITEM_POST:
        flipbool(&post, command, FALSE);
        if (machines[machine].coinop)
        {   patchrom();
        }
    acase MENUITEM_RANDOMIZE:
        flipbool(&randomizememory, command, FALSE);
    acase MENUITEM_SENSEGAME:
        flipbool(&sensegame, command, FALSE);
    acase MENUITEM_SHOWTOD:
        flipbool(&showtod, command, FALSE);
    acase MENUITEM_USESTUBS:
        flipbool(&usestubs, command, FALSE);
    acase MENUITEM_CONSOLE:
        flipbool(&echoconsole, command, FALSE);
    acase MENUITEM_PATHS:
        emu_paths();
    // "Settings|Graphics �" submenu
    acase MENUITEM_FULLSCREEN:
        flipbool(&fullscreen, command, TRUE);
    acase MENUITEM_NARROW:
#ifdef WIN32
        if (wide == 1)
        {   // it was checked, so now we uncheck it
            wide = realwide = 2;
        } else
        {   // assert(wide == 2);
            // it was unchecked, so now we check it
            wide = realwide = 1;
        }
        updatemenu(MENUITEM_NARROW);
        resize(size, FALSE);
#endif
#ifdef AMIGA
        if (ischecked_toggle(MENUITEM_NARROW))
        {   if (wide == 2)
            {   pending |= PENDING_NARROW;
        }   }
        else
        {   if (wide == 1)
            {   pending |= PENDING_WIDE;
        }   }
#endif
    acase MENUITEM_DEJITTER:
        flipbool(&dejitter, command, FALSE);
#ifdef WIN32
    acase MENUITEM_SKIES:
        flipbool(&enhancestars, command, FALSE);
        fixupcolours();
        drawpixelroutine();
        redrawscreen();
#endif
    acase MENUITEM_ROTATE:
        flipbool(&rotate, command, FALSE);
        if (memmapinfo[memmap].rotate)
        {   rotating = rotate ? TRUE : FALSE;
        } else
        {   rotating = FALSE;
        }
#ifdef WIN32
        // as you can see, we need to do more on IBM-PC than on Amiga
        // (as window might have changed dimensions if stretching is enabled)
        if (fullscreen)
        {   closewindow();
            winleftx =
            wintopy  = -1;
            openwindow(TRUE);
            updatepointer(FALSE, TRUE);
        } else
        {   closewindow();
            calc_margins();
            openwindow(TRUE);
            draw_margins();
            redrawscreen();
        }
#endif
#ifdef AMIGA
        drawpixelroutine();
        redrawscreen();
#endif
    acase MENUITEM_SCANLINES:
        flipbool(&scanlines, command, FALSE);
#ifdef AMIGA
        drawpixelroutine();
#endif
        redrawscreen();
#ifdef WIN32
    acase MENUITEM_STRETCH43:
        flipbool(&stretch43, command, FALSE);
        resize(size, TRUE);
#endif
    acase MENUITEM_UNLIT:
        flipbool(&drawunlit, command, TRUE);
    acase MENUITEM_CORNERS:
        flipbool(&drawcorners, command, TRUE);
    acase MENUITEM_SHOWLEDS:
        flipbool(&showleds, command, TRUE);
    // "Settings|Input �" submenu
    acase MENUITEM_ANALOG:
        flipbool((int*) &analog, command, FALSE);
        updatemenus(); // to ghost/unghost sensitivity and springloaded
#ifdef WIN32
    acase MENUITEM_CALIBRATE:
        calibrate();
#endif
    acase MENUITEM_SENSITIVITY:
#ifdef WIN32
        sound_off(FALSE);
        opening = modal = TRUE;
        settitle();
        DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_SENSITIVITY), MainWindowPtr, SensitivityDlgProc);
        opening = modal = FALSE;
        settitle();
        sound_on(TRUE);
#endif
#ifdef AMIGA
        input_sensitivity();
#endif
    acase MENUITEM_SPRINGLOADED:
        flipbool((int*) &springloaded, command, FALSE);
    acase MENUITEM_LOWERCASEINPUT:
        flipbool(&lowercase, command, FALSE);
    acase MENUITEM_CONFINE:
        flipbool(&confine, command, TRUE);
    acase MENUITEM_ERASEDEL:
        flipbool(&erasedel, command, TRUE);
    acase MENUITEM_AUTOCOIN:
        flipbool(&autocoin, command, FALSE);
    acase MENUITEM_GUESTRMB:
        flipbool(&guestrmb, command, TRUE);
        rmb = FALSE;
        updatepadnames();
    acase MENUITEM_QUEUEKEYSTROKES:
        flipbool(&queuekeystrokes, command, TRUE);
    acase MENUITEM_SHOWPOSITIONS:
        flipbool(&showpositions, command, FALSE);
    acase MENUITEM_SWAPPED:
        flipbool((int*) &swapped, command, TRUE);
    acase MENUITEM_REARRANGE:
    case MENUITEM_REDEFINEKEYS:
        docommand(command);
    // "Settings|Speed �" submenu
    acase MENUITEM_PAUSED:
#ifdef WIN32
        if (paused)
        {   emu_unpause();
        } else
        {   emu_pause();
        }
#endif
#ifdef AMIGA
        if (ischecked_toggle(MENUITEM_PAUSED))
        {   emu_pause();
        } else
        {   emu_unpause();
        }
#endif
    acase MENUITEM_AUTOPAUSE:
        flipbool(&autopause, command, FALSE);
    acase MENUITEM_EXACTSPEED:
        flipbool(&exactspeed, command, TRUE);
    acase MENUITEM_LIMITREFRESHES:
        flipbool(&limitrefreshes, command, FALSE);
    acase MENUITEM_TURBO:
        flipbool((int*) &turbo, command, TRUE);
    acase MENUITEM_ADJUSTSPEED:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_SPEED])
        {   close_subwindow(SUBWINDOW_SPEED);
        } else
        {   open_speed();
        }
#endif
#ifdef AMIGA
        open_speed();
#endif
    // "Settings|Sprites �" submenu
    acase MENUITEM_COLLISIONS:
        flipbool((int*) &collisions, command, TRUE);
    // "Settings|Sound �" submenu
    acase MENUITEM_SOUND:
        flipbool((int*) &sound, command, TRUE);
    acase MENUITEM_AMBIENT:
        flipbool(&ambient, command, FALSE);
        if (sound)
        {   if (ambient)
            {   if (machine == TWIN)
                {   play_sample(SAMPLE_SPIN);
                } elif (machine == BINBUG || machine == CD2650)
                {   for (i = 0; i < machines[machine].drives; i++)
                    {   if (drive[i].spinning)
                        {   play_sample(SAMPLE_SPIN);
                            break;
            }   }   }   }
            else
            {   for (i = GUESTCHANNELS; i < TOTALCHANNELS; i++)
                {   sound_stop(i);
        }   }   }
#ifdef AMIGA
    acase MENUITEM_FILTERED:
        flipbool(&led, command, FALSE);
#ifndef __amigaos4__
        if (!ng)
        {   updatefilter();
        }
#endif
#endif
    acase MENUITEM_RETUNE:
        flipbool(&retune, command, FALSE); // it doesn't affect the tone(s) currently playing, only future tones
    acase MENUITEM_ADJUSTSOUND:
        adjust_sound();
    // "Settings|Trainers �" submenu
    acase MENUITEM_CHEATLIVES:
        flipbool(&trainer_lives, command, FALSE);
        patchrom();
    acase MENUITEM_CHEATTIME:
        flipbool(&trainer_time, command, FALSE);
        patchrom();
    acase MENUITEM_INVINCIBILITY:
        flipbool(&trainer_invincibility, command, FALSE);
        patchrom();
    acase MENUITEM_LEVELSKIP:
        docommand(MENUITEM_LEVELSKIP);
    // "Settings|Graphics �" submenu (again)
    acase MENUITEM_USEMARGINS:
        flipbool(&usemargins, command, FALSE);
        calc_margins();
        draw_margins();
#ifdef WIN32
        updatemenu(MENUITEM_USEMARGINS);
        resize(size, TRUE);
        redrawscreen();
#endif
#ifdef AMIGA
        pending |= PENDING_RESIZE; // redrawscreen() will get called eventually
#endif
#ifdef WIN32
    acase MENUITEM_BEZEL:
        flipbool(&bezel, command, TRUE);
#endif
    acase MENUITEM_BLINK:
        flipbool(&blink, command, FALSE);
    acase MENUITEM_COOMER:
        flipbool(&coomer, command, FALSE);
    // "Help" menu
    acase MENUITEM_GAMEINFO:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_GAMEINFO])
        {   close_subwindow(SUBWINDOW_GAMEINFO);
        } else
        {   help_gameinfo();
        }
#endif
#ifdef AMIGA
        help_gameinfo();
#endif
    acase MENUITEM_HOSTPADS:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_HOSTPADS])
        {   close_subwindow(SUBWINDOW_HOSTPADS);
        } else
        {   help_hostpads();
        }
#endif
#ifdef AMIGA
        help_hostpads();
#endif
    acase MENUITEM_HOSTKYBD:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_HOSTKYBD])
        {   close_subwindow(SUBWINDOW_HOSTKYBD);
        } else
        {   help_hostkybd();
        }
#endif
#ifdef AMIGA
        help_hostkybd();
#endif
    acase MENUITEM_OPCODES:
#ifdef WIN32
        if (SubWindowPtr[SUBWINDOW_OPCODES])
        {   close_subwindow(SUBWINDOW_OPCODES);
        } else
        {   help_opcodes();
        }
#endif
#ifdef AMIGA
        help_opcodes();
#endif
    acase MENUITEM_CODINGGUIDE:
    case MENUITEM_GAMINGGUIDE:
        docommand(command);
    acase MENUITEM_MANUAL:
        help_manual(-1);
    acase MENUITEM_UPDATE:
        help_update(FALSE);
#ifdef AMIGA
    acase MENUITEM_REACTION:
        help_reaction();
#endif
    acase MENUITEM_ABOUT:
        help_about();
 /* adefault:
        zprintf(TEXTPEN_VERBOSE, "Unknown menu item: %d!\n", command); */
    }

#ifdef AMIGA
    updatepointer(FALSE, FALSE);
#endif
}

EXPORT void handle_menu2(int command)
{
#ifdef AMIGA
    if (!ischecked_mx(command))
    {   return;
    }
#endif

    switch (command)
    {
    // "View" menu
    case MENUOPT_SORTBYNAME:
        sortby = SORTBYNAME;
        updatemenu(MENUFAKE_SORTBY);
        if (showsidebars[wsm])
        {   buildlistbrowser();
        }
    acase MENUOPT_SORTBYMACHINE:
        sortby = SORTBYMACHINE;
        updatemenu(MENUFAKE_SORTBY);
        if (showsidebars[wsm])
        {   buildlistbrowser();
        }
    acase MENUOPT_INDICATOR_FPS:
        viewspeedas = VIEWAS_FPS;
        updatemenu(MENUFAKE_SPEEDINDICATOR);
        update_speed();
    acase MENUOPT_INDICATOR_KHZ:
        viewspeedas = VIEWAS_KHZ;
        updatemenu(MENUFAKE_SPEEDINDICATOR);
        update_speed();
    acase MENUOPT_INDICATOR_PERCENT:
        viewspeedas = VIEWAS_PERCENTS;
        updatemenu(MENUFAKE_SPEEDINDICATOR);
        update_speed();
    // "Debug|File �" submenu
    acase MENUOPT_DRIVE_0:         debugger_full("DRIVE 0");
    acase MENUOPT_DRIVE_1:         debugger_full("DRIVE 1");
    acase MENUOPT_DRIVE_2:         debugger_full("DRIVE 2");
    acase MENUOPT_DRIVE_3:         debugger_full("DRIVE 3");
    // "Debug|Breakpoints �" submenu
    acase MENUOPT_WW_NONE:         debugger_full("WW NONE");
    acase MENUOPT_WW_SOME:         debugger_full("WW SOME");
    acase MENUOPT_WW_ALL:          debugger_full("WW ALL" );
    // "Debug|Options �" submenu
    acase MENUOPT_BASE_BINARY:     debugger_full("BASE 2" );
    acase MENUOPT_BASE_OCTAL:      debugger_full("BASE 8" );
    acase MENUOPT_BASE_DECIMAL:    debugger_full("BASE 10");
    acase MENUOPT_BASE_HEX:        debugger_full("BASE 16");
    acase MENUOPT_CPU_0:           debugger_full("CPU 0"  );
    acase MENUOPT_CPU_1:           debugger_full("CPU 1"  );
    acase MENUOPT_N_0:             debugger_full("N 0"    );
    acase MENUOPT_N_1:             debugger_full("N 1"    );
    acase MENUOPT_N_2:             debugger_full("N 2"    );
    acase MENUOPT_N_3:             debugger_full("N 3"    );
    acase MENUOPT_N_4:             debugger_full("N 4"    );
    acase MENUOPT_TU_0:            debugger_full("TU 0"   );
    acase MENUOPT_TU_1:            debugger_full("TU 1"   );
    acase MENUOPT_TU_2:            debugger_full("TU 2"   );
    acase MENUOPT_VB_0:            debugger_full("VERBOSE 0");
    acase MENUOPT_VB_1:            debugger_full("VERBOSE 1");
    acase MENUOPT_VB_2:            debugger_full("VERBOSE 2");
    // "Debug|Graphics �" submenu
    acase MENUOPT_DRAW_0:          debugger_full("DRAW 0" );
    acase MENUOPT_DRAW_1:          debugger_full("DRAW 1" );
    acase MENUOPT_DRAW_2:          debugger_full("DRAW 2" );
    acase MENUOPT_DRAW_3:          debugger_full("DRAW 3" );
    acase MENUOPT_DRAW_4:          debugger_full("DRAW 4" );
    acase MENUOPT_SPR_0:           debugger_full("SPR 0"  );
    acase MENUOPT_SPR_1:           debugger_full("SPR 1"  );
    acase MENUOPT_SPR_2:           debugger_full("SPR 2"  );
    // "Settings" menu
    acase MENUOPT_LEFT_TRACKBALL:  hostcontroller[0] = CONTROLLER_TRACKBALL; docommand(MENUFAKE_LEFT);
    acase MENUOPT_LEFT_1STDJOY:    hostcontroller[0] = CONTROLLER_1STDJOY;   docommand(MENUFAKE_LEFT);
    acase MENUOPT_LEFT_2NDDJOY:    hostcontroller[0] = CONTROLLER_2NDDJOY;   docommand(MENUFAKE_LEFT);
    acase MENUOPT_LEFT_NONE:       hostcontroller[0] = CONTROLLER_NONE;      docommand(MENUFAKE_LEFT);
#ifdef AMIGA
    acase MENUOPT_LEFT_1STDPAD:    hostcontroller[0] = CONTROLLER_1STDPAD;   docommand(MENUFAKE_LEFT);
    acase MENUOPT_LEFT_2NDDPAD:    hostcontroller[0] = CONTROLLER_2NDDPAD;   docommand(MENUFAKE_LEFT);
    acase MENUOPT_LEFT_2NDAJOY:    hostcontroller[0] = CONTROLLER_2NDAJOY;   docommand(MENUFAKE_LEFT);
    acase MENUOPT_LEFT_1STAPAD:    hostcontroller[0] = CONTROLLER_1STAPAD;   docommand(MENUFAKE_LEFT);
    acase MENUOPT_LEFT_2NDAPAD:    hostcontroller[0] = CONTROLLER_2NDAPAD;   docommand(MENUFAKE_LEFT);
#endif
    acase MENUOPT_RIGHT_TRACKBALL: hostcontroller[1] = CONTROLLER_TRACKBALL; docommand(MENUFAKE_RIGHT);
    acase MENUOPT_RIGHT_1STDJOY:   hostcontroller[1] = CONTROLLER_1STDJOY;   docommand(MENUFAKE_RIGHT);
    acase MENUOPT_RIGHT_2NDDJOY:   hostcontroller[1] = CONTROLLER_2NDDJOY;   docommand(MENUFAKE_RIGHT);
    acase MENUOPT_RIGHT_NONE:      hostcontroller[1] = CONTROLLER_NONE;      docommand(MENUFAKE_RIGHT);
#ifdef AMIGA
    acase MENUOPT_RIGHT_1STDPAD:   hostcontroller[1] = CONTROLLER_1STDPAD;   docommand(MENUFAKE_RIGHT);
    acase MENUOPT_RIGHT_2NDDPAD:   hostcontroller[1] = CONTROLLER_2NDDPAD;   docommand(MENUFAKE_RIGHT);
    acase MENUOPT_RIGHT_2NDAJOY:   hostcontroller[1] = CONTROLLER_2NDAJOY;   docommand(MENUFAKE_RIGHT);
    acase MENUOPT_RIGHT_1STAPAD:   hostcontroller[1] = CONTROLLER_1STAPAD;   docommand(MENUFAKE_RIGHT);
    acase MENUOPT_RIGHT_2NDAPAD:   hostcontroller[1] = CONTROLLER_2NDAPAD;   docommand(MENUFAKE_RIGHT);
#endif
    // "Settings|BIOS/DOS �" submenu
    acase MENUOPT_PHILIPS:
        elektor_biosver = ELEKTOR_PHILIPS;
        changemachine(ELEKTOR, memmap, TRUE, TRUE, TRUE);
        close_subwindows(FALSE);
        reopen_subwindows();
        updatemenus();
        updatesmlgads();
        redrawscreen();
    acase MENUOPT_HOBBYMODULE:
        elektor_biosver = ELEKTOR_HOBBYMODULE;
        changemachine(ELEKTOR, memmap, TRUE, TRUE, TRUE);
        close_subwindows(FALSE);
        reopen_subwindows();
        updatemenus();
        updatesmlgads();
        redrawscreen();
    acase MENUOPT_ARTEMIS:
        pipbug_biosver = PIPBUG_ARTEMIS;
        pipbug_changebios(TRUE);
        updatemenus();
        engine_reset();
    acase MENUOPT_HYBUG:
        pipbug_biosver = PIPBUG_HYBUG;
        pipbug_changebios(TRUE);
        updatemenus();
        engine_reset();
    acase MENUOPT_PIPBUG1BIOS:
        pipbug_biosver = PIPBUG_PIPBUG1BIOS;
        pipbug_changebios(TRUE);
        updatemenus();
        engine_reset();
    acase MENUOPT_PIPBUG2BIOS:
        pipbug_biosver = PIPBUG_PIPBUG2BIOS;
        pipbug_changebios(TRUE);
        updatemenus();
        engine_reset();
    acase MENUOPT_BINBUG35BIOS:
        binbug_biosver = BINBUG_35;
        binbug_changebios(TRUE);
        updatemenus();
    acase MENUOPT_BINBUG36BIOS:
        binbug_biosver = BINBUG_36;
        binbug_changebios(TRUE);
        updatemenus();
    acase MENUOPT_BINBUG61BIOS:
        binbug_biosver = BINBUG_61;
        binbug_changebios(TRUE);
        updatemenus();
    acase MENUOPT_GBUG:
        binbug_biosver = BINBUG_GBUG;
        binbug_changebios(TRUE);
        updatemenus();
    acase MENUOPT_MIKEBUG:
        binbug_biosver = BINBUG_MIKEBUG;
        binbug_changebios(TRUE);
        updatemenus();
    acase MENUOPT_MYBUG:
        binbug_biosver = BINBUG_MYBUG;
        binbug_changebios(TRUE);
        updatemenus();
    acase MENUOPT_BIOS_IPL:
        cd2650_biosver = CD2650_IPL;
        cd2650_changebios(); // calls updatemenus() for us
    acase MENUOPT_BIOS_POPMON:
        cd2650_biosver = CD2650_POPMON;
        cd2650_changebios(); // calls updatemenus() for us
    acase MENUOPT_BIOS_SUPERVISOR:
        cd2650_biosver = CD2650_SUPERVISOR;
        cd2650_changebios(); // calls updatemenus() for us
    acase MENUOPT_BIOS_MINIMONITOR:
        phunsy_biosver = PHUNSY_MINIMONITOR;
        changemachine(PHUNSY, memmap, TRUE, TRUE, TRUE);
        close_subwindows(FALSE);
        reopen_subwindows();
        updatemenus();
        updatesmlgads();
        redrawscreen();
    acase MENUOPT_BIOS_PHUNSY:
        phunsy_biosver = PHUNSY_PHUNSY;
        changemachine(PHUNSY, memmap, TRUE, TRUE, TRUE);
        close_subwindows(FALSE);
        reopen_subwindows();
        updatemenus();
        updatesmlgads();
        redrawscreen();
    acase MENUOPT_00BIOS:
        selbst_biosver = SELBST_BIOS00;
        changemachine(SELBST, MEMMAP_SELBST, TRUE, TRUE, TRUE);
        close_subwindows(FALSE);
        reopen_subwindows();
        updatemenus();
        updatesmlgads();
        redrawscreen();
    acase MENUOPT_09BIOS:
        selbst_biosver = SELBST_BIOS09;
        changemachine(SELBST, MEMMAP_SELBST, TRUE, TRUE, TRUE);
        close_subwindows(FALSE);
        reopen_subwindows();
        updatemenus();
        updatesmlgads();
        redrawscreen();
    acase MENUOPT_10BIOS:
        selbst_biosver = SELBST_BIOS10;
        changemachine(SELBST, MEMMAP_SELBST, TRUE, TRUE, TRUE);
        close_subwindows(FALSE);
        reopen_subwindows();
        updatemenus();
        updatesmlgads();
        redrawscreen();
    acase MENUOPT_20BIOS:
        selbst_biosver = SELBST_BIOS20;
        changemachine(SELBST, MEMMAP_SELBST, TRUE, TRUE, TRUE);
        close_subwindows(FALSE);
        reopen_subwindows();
        updatemenus();
        updatesmlgads();
        redrawscreen();
    // "Settings|Colours �" submenu
    acase MENUOPT_AMBER:
        colourset = AMBERSCALE;
        changecolours2();
    acase MENUOPT_GREEN:
        colourset = GREENSCALE;
        changecolours2();
    acase MENUOPT_GREYSCALE:
        colourset = GREYSCALE;
        changecolours2();
    acase MENUOPT_PURECOLOURS:
        colourset = PURECOLOURS;
        changecolours2();
    acase MENUOPT_UVICOLOURS:
        colourset = UVICOLOURS;
        changecolours2();
    acase MENUOPT_PVICOLOURS:
        colourset = PVICOLOURS;
        changecolours2();
    // "Settings|DOS �" submenu
    acase MENUOPT_MICRODOS:
        binbug_dosver = DOS_MICRODOS;
        binbug_changebios(TRUE);
        updatemenus();
    acase MENUOPT_VHSDOS:
        binbug_dosver = DOS_VHSDOS;
        binbug_changebios(TRUE);
        updatemenus();
    acase MENUOPT_NOBINBUGDOS:
        binbug_dosver = DOS_NOBINBUGDOS;
        // don't change cd2650_biosver
        binbug_changebios(TRUE);
        updatemenus();
    acase MENUOPT_CDDOS:
        cd2650_dosver = DOS_CDDOS;
        cd2650_biosver = CD2650_IPL;
        cd2650_changebios(); // calls updatemenus() for us
    acase MENUOPT_P1DOS:
        cd2650_dosver = DOS_P1DOS;
        cd2650_biosver = CD2650_IPL;
        cd2650_changebios(); // calls updatemenus() for us
    acase MENUOPT_NOCD2650DOS:
        cd2650_dosver = DOS_NOCD2650DOS;
        if (cd2650_biosver == CD2650_IPL)
        {   cd2650_biosver = CD2650_SUPERVISOR;
        }
        cd2650_changebios(); // calls updatemenus() for us
    acase MENUOPT_DOS_EXOS:      twin_dosver = TWIN_EXOS;   twin_dir_disk(TRUE, 0);
    acase MENUOPT_DOS_SDOS20:    twin_dosver = TWIN_SDOS20; twin_dir_disk(TRUE, 0);
    acase MENUOPT_DOS_SDOS40:    twin_dosver = TWIN_SDOS40; twin_dir_disk(TRUE, 0);
    acase MENUOPT_DOS_SDOS42:    twin_dosver = TWIN_SDOS42; twin_dir_disk(TRUE, 0);
    acase MENUOPT_DOS_TOS:       twin_dosver = TWIN_TOS;    twin_dir_disk(TRUE, 0);
    acase MENUOPT_DOS_UDOS:      twin_dosver = TWIN_UDOS;   twin_dir_disk(TRUE, 0);
    acase MENUOPT_DOS_NOTWINDOS: twin_dosver = TWIN_NODOS;  twin_dir_disk(TRUE, 0);
    // "Settings|Emulator �" submenu
    acase MENUOPT_FRAMEBASED:
    case  MENUOPT_PIXELBASED:
        flipbool(&framebased, command, FALSE);
    acase MENUOPT_APPEND:
        if (logfile != LOGFILE_APPEND)
        {   logfile_close();
            logfile = LOGFILE_APPEND;
#ifdef AMIGA
            logfile_open(FALSE);
#endif
        }
        updatemenu(MENUFAKE_LOGTOFILE);
    acase MENUOPT_IGNORE:
        if (logfile != LOGFILE_IGNORE)
        {   logfile_close();
            logfile = LOGFILE_IGNORE;
        }
        updatemenu(MENUFAKE_LOGTOFILE);
    acase MENUOPT_REPLACE:
        if (logfile != LOGFILE_REPLACE)
        {   logfile_close();
            logfile = LOGFILE_REPLACE;
#ifdef AMIGA
            logfile_open(FALSE);
#endif
        }
        updatemenu(MENUFAKE_LOGTOFILE);
    // "Settings|Filters �" submenu
#ifdef WIN32
    acase MENUOPT_FILTER_3D:
        filter = FILTER_3D;
        updatemenus();
        calc_size();
        if (fullscreen)
        {   clearscreen(); // so that the grey border is repainted
        }
        redrawscreen();
    acase MENUOPT_FILTER_HQX:
        filter = FILTER_HQX;
        updatemenus();
        drawpixelroutine();
        redrawscreen();
#endif
    acase MENUOPT_FILTER_SCALE2X:
        filter = FILTER_SCALE2X;
#ifdef WIN32
        updatemenus();
#endif
        drawpixelroutine();
        redrawscreen();
    acase MENUOPT_FILTER_NONE:
        filter = FILTER_NONE;
#ifdef WIN32
        updatemenus();
#endif
        drawpixelroutine();
        redrawscreen();
#ifdef WIN32
    acase MENUOPT_UNSTRETCHED:
        stretching = STRETCHING_NONE;
        updatemenus();
        if (fullscreen)
        {   calc_size();
            drawpixelroutine();
            clearscreen();
            redrawscreen();
            updatepointer(FALSE, TRUE);
        }
    acase MENUOPT_STRETCH43:
        stretching = STRETCHING_43;
        updatemenus();
        if (fullscreen)
        {   calc_size();
            drawpixelroutine();
            clearscreen();
            redrawscreen();
            updatepointer(FALSE, TRUE);
        }
    acase MENUOPT_STRETCHTOFIT:
        stretching = STRETCHING_TOFIT;
        updatemenus();
        if (fullscreen)
        {   calc_size();
            drawpixelroutine();
            clearscreen();
            redrawscreen();
            updatepointer(FALSE, TRUE);
        }
#endif
    // "Settings|Graphics �" submenu
    acase MENUOPT_1XSIZE:
#ifdef WIN32
        resize(1, FALSE);
#endif
#ifdef AMIGA
        if (size != 1)
        {   pending |= PENDING_1XSIZE;
        }
#endif
    acase MENUOPT_2XSIZE:
#ifdef WIN32
        resize(2, FALSE);
#endif
#ifdef AMIGA
        if (size != 2)
        {   pending |= PENDING_2XSIZE;
        }
#endif
    acase MENUOPT_3XSIZE:
#ifdef WIN32
        resize(3, FALSE);
#endif
#ifdef AMIGA
        if (size != 3)
        {   pending |= PENDING_3XSIZE;
        }
#endif
    acase MENUOPT_4XSIZE:
#ifdef WIN32
        resize(4, FALSE);
#endif
#ifdef AMIGA
        if (size != 4)
        {   pending |= PENDING_4XSIZE;
        }
#endif
    acase MENUOPT_5XSIZE:
#ifdef WIN32
        resize(5, FALSE);
#endif
#ifdef AMIGA
        if (size != 5)
        {   pending |= PENDING_5XSIZE;
        }
#endif
    acase MENUOPT_6XSIZE:
#ifdef WIN32
        resize(6, FALSE);
#endif
#ifdef AMIGA
        if (size != 6)
        {   pending |= PENDING_6XSIZE;
        }
#endif
    // "Settings|Input �" submenu
    acase MENUOPT_KEYMAP_HOST:
        keymap = KEYMAP_HOST;
        if (SubWindowPtr[SUBWINDOW_HOSTKYBD])
        {   close_subwindow(SUBWINDOW_HOSTKYBD);
            help_hostkybd();
        }
    acase MENUOPT_KEYMAP_EA:
        keymap = KEYMAP_EA;
        if (SubWindowPtr[SUBWINDOW_HOSTKYBD])
        {   close_subwindow(SUBWINDOW_HOSTKYBD);
            help_hostkybd();
        }
    acase MENUOPT_KEYMAP_TT:
        keymap = KEYMAP_TT;
        if (SubWindowPtr[SUBWINDOW_HOSTKYBD])
        {   close_subwindow(SUBWINDOW_HOSTKYBD);
            help_hostkybd();
        }
#ifdef WIN32
    // "Settings|Language �" submenu
    acase MENUOPT_ENG:         freelanguage(); language = LANGUAGE_ENG; changelanguage();
    acase MENUOPT_HOL:         freelanguage(); language = LANGUAGE_HOL; changelanguage();
    acase MENUOPT_FRA:         freelanguage(); language = LANGUAGE_FRA; changelanguage();
    acase MENUOPT_GER:         freelanguage(); language = LANGUAGE_GER; changelanguage();
    acase MENUOPT_GRE:         freelanguage(); language = LANGUAGE_GRE; changelanguage();
    acase MENUOPT_ITA:         freelanguage(); language = LANGUAGE_ITA; changelanguage();
    acase MENUOPT_POL:         freelanguage(); language = LANGUAGE_POL; changelanguage();
    acase MENUOPT_RUS:         freelanguage(); language = LANGUAGE_RUS; changelanguage();
    acase MENUOPT_SPA:         freelanguage(); language = LANGUAGE_SPA; changelanguage();
#endif
    // "Settings|Machine �" submenu
    acase MENUOPT_ARCADIA:     command_changemachine(ARCADIA,    MEMMAP_ARCADIA);
    acase MENUOPT_INTERTON:    command_changemachine(INTERTON,   MEMMAP_D);
    acase MENUOPT_ELEKTOR:     command_changemachine(ELEKTOR,    MEMMAP_F);
    acase MENUOPT_PIPBUG:      command_changemachine(PIPBUG,     MEMMAP_PIPBUG1);
    acase MENUOPT_BINBUG:      command_changemachine(BINBUG,     MEMMAP_BINBUG);
    acase MENUOPT_INSTRUCTOR:  command_changemachine(INSTRUCTOR, MEMMAP_O);
    acase MENUOPT_TWIN:        command_changemachine(TWIN,       MEMMAP_TWIN);
    acase MENUOPT_CD2650:      command_changemachine(CD2650,     MEMMAP_CD2650);
    acase MENUOPT_ASTROWARS:   command_changemachine(ZACCARIA,   MEMMAP_ASTROWARS);
    acase MENUOPT_GALAXIA:     command_changemachine(ZACCARIA,   MEMMAP_GALAXIA);
    acase MENUOPT_LASERBATTLE: command_changemachine(ZACCARIA,   MEMMAP_LASERBATTLE);
    acase MENUOPT_LAZARIAN:    command_changemachine(ZACCARIA,   MEMMAP_LAZARIAN);
    acase MENUOPT_MALZAK1:     command_changemachine(MALZAK,     MEMMAP_MALZAK1);
    acase MENUOPT_MALZAK2:     command_changemachine(MALZAK,     MEMMAP_MALZAK2);
    acase MENUOPT_PHUNSY:      command_changemachine(PHUNSY,     MEMMAP_PHUNSY);
    acase MENUOPT_SELBST:      command_changemachine(SELBST,     MEMMAP_SELBST);
    acase MENUOPT_MIKIT:       command_changemachine(MIKIT,      MEMMAP_MIKIT);
    acase MENUOPT_8550:        command_changemachine(PONG,       MEMMAP_8550);
    acase MENUOPT_8600:        command_changemachine(PONG,       MEMMAP_8600);
    acase MENUOPT_TYPERIGHT:   command_changemachine(TYPERIGHT,  MEMMAP_TYPERIGHT);
    // "Settings|Speed �" submenu
    acase MENUOPT_NTSC:
        region = REGION_NTSC;
        docommand(MENUFAKE_REGION);
    acase MENUOPT_PAL:
        region = REGION_PAL;
        docommand(MENUFAKE_REGION);
#ifdef WIN32
    acase MENUOPT_HIGHPRI:
        priority = PRI_HIGH;
        docommand(MENUFAKE_PRIORITY);
    acase MENUOPT_NORMALPRI:
        priority = PRI_NORMAL;
        docommand(MENUFAKE_PRIORITY);
    acase MENUOPT_LOWPRI:
        priority = PRI_LOW;
        docommand(MENUFAKE_PRIORITY);
#endif
    // "Settings|Sound �" submenu
#ifdef AMIGA
    acase MENUOPT_SOUND_AHI:
        sound_off(TRUE);
        soundoutput = SOUNDOUTPUT_AHI;
        updatemenu(MENUITEM_SOUNDOUTPUT);
        changesoundoutput();
        sound_on(FALSE);
    acase MENUOPT_SOUND_PAULA:
        sound_off(TRUE);
        soundoutput = SOUNDOUTPUT_PAULA;
        updatemenu(MENUITEM_SOUNDOUTPUT);
        changesoundoutput();
        sound_on(FALSE);
#endif
    // "Settings|Sprites �" submenu
    acase MENUOPT_DEMULTIPLEX_MULTIPLEX:
        demultiplex = 0;
        redrawscreen();
    acase MENUOPT_DEMULTIPLEX_TRANSPARENT:
        demultiplex = 1;
        redrawscreen();
    acase MENUOPT_DEMULTIPLEX_OPAQUE:
        demultiplex = 2;
        redrawscreen();
    // "Settings|VDU �" submenu
    acase MENUOPT_ELEKTERMINAL:
        docommand2(MENUOPT_ELEKTERMINAL);
    acase MENUOPT_LCVDU_NARROW:
        docommand2(MENUOPT_LCVDU_NARROW);
    acase MENUOPT_LCVDU_WIDE:
        docommand2(MENUOPT_LCVDU_WIDE);
    acase MENUOPT_RADIOBULLETIN:
        docommand2(MENUOPT_RADIOBULLETIN);
    acase MENUOPT_SVT100:
        docommand2(MENUOPT_SVT100);
    acase MENUOPT_VT100:
        docommand2(MENUOPT_VT100);
    acase MENUOPT_ASCIIVDU:
        cd2650_vdu = VDU_ASCII;
        cd2650_updatecharset();
    acase MENUOPT_CHESSMEN:
        cd2650_vdu = VDU_CHESSMEN;
        cd2650_updatecharset();
    acase MENUOPT_LOWERCASEVDU:
        cd2650_vdu = VDU_LOWERCASE;
        cd2650_updatecharset();
    }

#ifdef AMIGA
    updatepointer(FALSE, FALSE);
#endif
}

MODULE void changecolours2(void)
{   updatemenus();
    fixupcolours();
    if (machine == PONG)
    {   pong_updatedips(); // for ghosting of "player identification?"
    }
    if (SubWindowPtr[SUBWINDOW_SPRITES])
    {   close_subwindow(SUBWINDOW_SPRITES);
        open_spriteeditor();
    }
#ifdef WIN32
    make_stars();
#endif
    redrawscreen();
}

EXPORT void docommand(int which)
{   TRANSIENT int  i;
    PERSIST   TEXT ggstring[80 + 1]; // PERSISTent so as not to blow the stack

    switch (which)
    {
    // "Project" menu
    case MENUITEM_RESETTOMONITOR:
        switch (machine)
        {
        case ELEKTOR:
            iar = 0x23; // gives blue background with yellow "IIII" at bottom
        acase PIPBUG:
        case BINBUG:
        case CD2650:
        case SELBST:
        case MIKIT:
            iar = 0;
        acase INSTRUCTOR:
            iar = 0x1800;
        acase TWIN:
            iar = 0x100;
        acase PHUNSY:
            if (phunsy_biosver == PHUNSY_PHUNSY)
            {   iar = 0;
            } else
            {   // assert(phunsy_biosver == PHUNSY_MINIMONITOR);
                iar = 0x1FEC;
        }   }
        NetPlay(NET_OFF);
        updatemenu(MENUITEM_SERVER);
        updatemenu(MENUITEM_CLIENT);
    acase MENUITEM_REINITIALIZE:
        changemachine(machine, memmap, TRUE, TRUE, FALSE);
        updatemenus();
        updatebiggads();
        updatesmlgads();
        settitle();
        redrawscreen();
        reopen_subwindows();
    acase MENUITEM_REVERT:
        if (crippled || !game) return;
        macro_stop();
        DISCARD engine_load(FALSE); // arguably TRUE might be better
    acase MENUITEM_QUICKLOAD:
        if (crippled)
        {   return;
        }
        macro_stop();
        changefilepart((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game, "QUICKSAV.COS");
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Attempting to quickload %s...\n", fn_game);
#endif
        if (!engine_load(TRUE))
        {   beep();
        }
    acase MENUITEM_QUICKSAVE:
        if (crippled)
        {   return;
        }
        strcpy((char*) fn_oldgame, (const char*) fn_game);
        changefilepart((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game, "QUICKSAV.COS");
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Attempting to quicksave %s...\n", fn_game);
#endif
        engine_save(KIND_COS, TRUE);
    acase MENUITEM_SERVER:
        if (connected == NET_OFF)
        {   NetPlay(NET_SERVER);
            if (connected != NET_OFF)
            {   project_reset(TRUE); /* reset machine when netplay connected */
        }   }
        else
        {   NetPlay(NET_OFF);
        }
        updatemenu(MENUITEM_SERVER);
    acase MENUITEM_CLIENT:
        if (connected == NET_OFF)
        {   NetPlay(NET_CLIENT);
            if (connected != NET_OFF)
            {    project_reset(TRUE); /* reset machine when netplay connected */
        }   }
        else
        {   NetPlay(NET_OFF);
        }
        updatemenu(MENUITEM_CLIENT);
    // "View" menu
    acase MENUITEM_VIEWDEBUGGER:
        // no point doing updatemenu() yet,
        // as we're about to close the window
        closewindow();
        openwindow(TRUE);
#ifdef WIN32
        redrawscreen();
        updatepointer(FALSE, TRUE);
        if (showdebugger[wsm])
        {   debugger_enter();
        } else
        {   debugger_exit();
        }
#endif
#ifdef AMIGA
        if (showdebugger[wsm])
        {   ActivateLayoutGadget(gadgets[GID_MA_LY1], MainWindowPtr, NULL, (Object) gadgets[GID_MA_ST4]);
        }
#endif
        updatemenus();
#ifdef WIN32
    acase MENUITEM_MENUBAR:
        // no point doing updatemenu(),
        // as we're about to close the window
        closewindow();
        if (!fullscreen)
        {   winleftx -= 3;
        }
        openwindow(TRUE);
        redrawscreen();
        updatepointer(FALSE, TRUE);
#endif
    acase MENUITEM_POINTER:
        updatemenu(MENUITEM_POINTER);
        updatepointer(FALSE, TRUE);
    acase MENUITEM_SIDEBAR:
#ifdef WIN32
        flipbool(&showsidebars[wsm], MENUITEM_SIDEBAR, FALSE);
        // updatemenu(MENUITEM_AUDIT); is not needed
        closewindow();
        openwindow(TRUE);
        if (fullscreen && showsidebars[wsm] && !showpointers[wsm])
        {   showpointers[wsm] = TRUE;
            updatepointer(FALSE, TRUE);
            updatemenu(MENUITEM_POINTER);
        }
#endif
#ifdef AMIGA
        if (ischecked_toggle(MENUITEM_SIDEBAR))
        {   pending |= PENDING_SIDEBARON;
        } else
        {   pending |= PENDING_SIDEBAROFF;
        }
        // updatemenu(MENUITEM_AUDIT); is not needed
#endif
    acase MENUITEM_TITLEBAR:
        // no point in doing updatemenu(MENUITEM_TITLEBAR);
        closewindow();
#ifdef WIN32
        if (showtitlebars[wsm] && !fullscreen)
        {   winleftx -= 4;
        }
#endif
        openwindow(TRUE);
#ifdef WIN32
        redrawscreen();
        updatepointer(FALSE, TRUE);
#endif
    acase MENUITEM_TOOLBAR:
        closewindow();
#ifdef WIN32
        if (showtoolbars[wsm] && !fullscreen)
        {   winleftx -= 6;
        }
#endif
        openwindow(TRUE);
#ifdef WIN32
        redrawscreen();
        updatepointer(FALSE, TRUE);
#endif
    acase MENUITEM_STATUSBAR:
        // no point doing updatemenu(),
        // as we're about to close the window
        closewindow();
        openwindow(TRUE);
#ifdef WIN32
        redrawscreen();
        updatepointer(FALSE, TRUE);
#endif
    // "Tools" menu
    acase MENUITEM_TOOLSDEBUGGER:
        // assert(showdebugger[wsm]);
#ifdef AMIGA
        ActivateLayoutGadget(gadgets[GID_MA_LY1], MainWindowPtr, NULL, (Object) gadgets[GID_MA_ST4]);
#endif
#ifdef WIN32
        if (incli)
        {   userinput[0] = EOS;
            DISCARD SendMessage(hDebugger, WM_SETTEXT, 0, (LPARAM) userinput);
            debugger_exit();
        } else
        {   debugger_enter();
        }
#endif
    acase MENUITEM_SCREENEDITOR:
        flipbool(&editscreen, MENUITEM_SCREENEDITOR, FALSE);
        edit_screen_sanitize();
        settitle();
        if (!editscreen)
        {   dogamename();
        }
    // "Settings" menu
    acase MENUFAKE_LEFT:
        updatemenu(MENUFAKE_LEFT);
        updatemenu(MENUITEM_CONFINE);
        updatepointer(FALSE, TRUE);
#ifdef AMIGA
        setjoyports();
#endif
        ff_off(); // overkill, we only need to turn off one
#ifdef WIN32
        reopen_subwindow(SUBWINDOW_HOSTPADS);
#endif
    acase MENUITEM_AUTOFIRE1:
        updatesmlgad(GADPOS_AUTOFIRE1, autofire[0], TRUE);
        updatemenu(MENUITEM_AUTOFIRE1);
    acase MENUFAKE_RIGHT:
        updatemenu(MENUFAKE_RIGHT);
        updatemenu(MENUITEM_CONFINE);
        updatepointer(FALSE, TRUE);
#ifdef AMIGA
        setjoyports();
#endif
        ff_off(); // overkill, we only need to turn off one
#ifdef WIN32
        reopen_subwindow(SUBWINDOW_HOSTPADS);
#endif
    acase MENUITEM_AUTOFIRE2:
        updatesmlgad(GADPOS_AUTOFIRE2, autofire[1], TRUE);
        updatemenu(MENUITEM_AUTOFIRE2);
    // "Settings|Colours �" submenu
    acase MENUITEM_DARKENBG:
#ifdef WIN32
        make_stars();
#endif
        updatemenu(MENUITEM_DARKENBG);
    acase MENUITEM_FLAGLINE:
        updatemenu(MENUITEM_FLAGLINE);
    acase MENUITEM_UNDITHER:
        updatemenu(MENUITEM_UNDITHER);
    // "Settings|Graphics �" submenu
    acase MENUITEM_FULLSCREEN:
        // no point doing updatemenu(MENUITEM_FULLSCREEN),
        // as we're about to close the window
        if (fullscreen)
        {   wsm = 1;
        } else
        {   wsm = 0;
        }
        closewindow();
        winleftx =
        wintopy  = -1;
#ifdef AMIGA
        if (fullscreen)
        {   windowed_size = size;
            windowed_wide = wide;
        } else
        {   closescreen(1);
            size = windowed_size;
            wide = windowed_wide;
        }
#endif
        openwindow(TRUE);
        updatepointer(FALSE, TRUE);
    acase MENUITEM_UNLIT:
        updatemenu(MENUITEM_UNLIT);
    acase MENUITEM_CORNERS:
        updatemenu(MENUITEM_CORNERS);
    // "Settings|Input �" submenu
    acase MENUITEM_CONFINE:
        updatemenu(MENUITEM_CONFINE);
        updatepointer(FALSE, TRUE);
    acase MENUITEM_SWAPPED:
        updatesmlgad(GADPOS_SWAPPED, swapped, TRUE);
        updatemenu(MENUITEM_SWAPPED);
        close_subwindows(FALSE);
        reopen_subwindows(); // to refresh contents of eg. game info and host keyboard subwindows
    acase MENUITEM_REARRANGE:
#ifdef WIN32
        sound_off(FALSE);
        opening = modal = TRUE;
        settitle();
        DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_REARRANGE), MainWindowPtr, RearrangeDlgProc);
        opening = modal = FALSE;
        settitle();
        sound_on(TRUE);
#endif
#ifdef AMIGA
        input_rearrange();
#endif
    acase MENUITEM_REDEFINEKEYS:
#ifdef WIN32
        sound_off(FALSE);
        opening = modal = TRUE;
        settitle();
        preserve = FALSE;
        if (showpalladiumkeys2)
        {   DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_REDEFINE_LARGE), MainWindowPtr, RedefineDlgProc);
        } else
        {   DISCARD DialogBox(InstancePtr, MAKEINTRESOURCE(IDD_REDEFINE_SMALL), MainWindowPtr, RedefineDlgProc);
        }
        opening = modal = FALSE;
        settitle();
        sound_on(TRUE);
#endif
#ifdef AMIGA
        if (redefinekeys(FALSE))
        {   while (redefinekeys(TRUE));
        }
#endif
    // "Settings|Sound �" submenu
    acase MENUITEM_SOUND:
        if (sound)
        {   sound_on(FALSE);
        } else
        {   sound_off(TRUE);
            // We have to force it, because sound variable is already FALSE
        }
        updatesmlgad(GADPOS_SOUND, sound, TRUE);
        updatemenu(MENUITEM_SOUND);
    // "Settings|Speed �" submenu
    acase MENUFAKE_REGION:
        updatemenu(MENUFAKE_REGION);
        update_speed();
        switch (machine)
        {
        case PONG:
            gridy[0] = gridy[1] = 0;
        //lint -fallthrough
        case ARCADIA:
            closewindow();
            calc_margins();
            openwindow(TRUE);
            draw_margins();
            redrawscreen();
        }
    acase MENUITEM_SHOWLEDS:
        updatemenu(MENUITEM_SHOWLEDS);
        closewindow();
        calc_margins();
        openwindow(TRUE);
        draw_margins();
        redrawscreen();
    acase MENUITEM_EXACTSPEED:
        if (exactspeed)
        {   usecsperframe[REGION_NTSC] = (int) (1000000.0 / 60.0                              );
            usecsperframe[REGION_PAL ] = (int) (1000000.0 / 50.0                              );
        } else
        {   usecsperframe[REGION_NTSC] = (int) (1000000.0 / machines[machine].fps[REGION_NTSC]);
            usecsperframe[REGION_PAL ] = (int) (1000000.0 / machines[machine].fps[REGION_PAL ]);
        }
    acase MENUITEM_TURBO:
        if (!turbo)
        {   waittill = thetime(); // makes it effective immediately
        }
        updatesmlgad(GADPOS_TURBO, turbo, TRUE);
        updatemenu(MENUITEM_TURBO);
        settitle();
        update_speed();
#ifdef WIN32
    acase MENUFAKE_PRIORITY:
        updatemenu(MENUFAKE_PRIORITY);
        setpri();
#endif
    // "Settings|Sprites �" submenu
    acase MENUITEM_COLLISIONS:
        updatesmlgad(GADPOS_COLLISIONS, collisions, TRUE);
        updatemenu(MENUITEM_COLLISIONS);
    // "Settings|Trainers �" submenu
    acase MENUITEM_LEVELSKIP:
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:
            if ((memory[0x1401] & 0xF) < 4) iar = 0x131A;
        acase MEMMAP_GALAXIA:
            if (memory[0x1C00] & 0x40) // boss level
            {   iar = 0x29F1;
            } else
            {   memory[0x1D88] = memory[0x1D89] = 0;
                iar = 0x2B5E;
            }
#ifdef ADVENTURETRAINER
        acase MEMMAP_BINBUG:
            if (whichgame == -1)
            {   DISCARD debug_edit("$1E4F", TRUE, FALSE);
            }
#endif
        acase MEMMAP_G:
        case  MEMMAP_H:
        case  MEMMAP_I:
            switch (whichgame)
            {
            case  _3DATTACKPOS:                                                  memory[0x18DA] =   0x64;
            acase BREAKAWAYPOS:                                                  memory[0x18D5] =   0x01;
            acase CATTRAXPOS:                                                    memory[0x18E5] =   0x95;
            acase CRAZYGOBBLERPOS:                                               iar            =  0x227;
            acase ESCAPEPOS:                                                     iar            =  0x1DF;
            acase FROGGER1POS:                                                   memory[0x1802] = memory[0x1805] = memory[0x1808] = memory[0x180B] = memory[0x180E] = 0x78;
                                                                                 pushras(); iar =  0xAFF;
            acase FROGGER2POS:                                                   memory[0x1802] = memory[0x1805] = memory[0x1808] = memory[0x180B] = memory[0x180E] = 0x78;
                                                                                 pushras(); iar =  0xB4E; // LIVEROUTINE
            acase FROGGER3POS:                                                   memory[0x1802] = memory[0x1805] = memory[0x1808] = memory[0x180B] = memory[0x180E] = 0x78;
                                                                                 pushras(); iar =  0xB47; // LIVEROUTINE
            acase FUNKYFISHPOS:                                                  memory[0x1AD0] = memory[0x1AD1] = memory[0x1AD2] = memory[0x1AD3] = memory[0x1AD4] = 0;
            acase HOBOPOS1:         case HOBOPOS2:                               memory[A_SPRITE1X] = 0x9B;
                                                                                 memory[A_SPRITE1Y] = 0xCE;
            acase JUMPBUG1POS:                                                   iar            =  0x495;
            acase JUMPBUG2POS:                                                   iar            =  0x446;
            acase MONACOPOS:
                if (memory[0x1AF0] >= 2) // ie. if not already on level 2
                {   memory[0x1AF0] = 2; // minutes
                    memory[0x1AEF] = 0; // seconds (in packed BCD format)
                } // Note that this requires unlimited time trainer to be turned off, momentarily at least
            acase NIBBLEMENPOS:     case SUPERGOBBLERPOS:                        memory[0x18E5] =   0x9A;
            acase REDCLASHPOS:      case REDCLASHODPOS:                          iar            =  0x3F4;
            acase ROBOTKILLERPOS:                                                iar            =  0x230;
            acase SPACEATTACKAPOS:  case SPACEATTACKBPOS:  case SPACEATTACKCPOS:
                for (i = 0x1820; i <= 0x187F; i++)
                {   memory[i] = 0;
                }
                memory[0x18D4] = memory[0x18D5] = 0xE4;
                memory[0x18E3] = memory[0x18E4] = 0x32;
                memory[A_SPRITE3X] = 0;
            acase SPIDERSPOS:       case SPIDERSODPOS:                           iar            =  0x796;
            acase SUPERBUG1POS:     case SUPERBUG2POS:                           iar            =  0xA6A; // or iar = 0xA53, if you want to play end-of-level song first
            acase TURTLESPOS:                                                    iar            =  0x273;
        }   }
    // "Settings|VDU �" submenu
#ifdef WIN32
    acase MENUITEM_BEZEL:
        updatemenu(MENUITEM_BEZEL);
        closewindow();
        calc_margins();
        openwindow(TRUE);
        draw_margins();
        redrawscreen();
#endif
    // "Help" menu
    acase MENUITEM_CODINGGUIDE:
        switch (machine)
        {
        case  ARCADIA:               openurl("http://amigan.yatho.com/s-coding.html#arcadia");
        acase INTERTON:              openurl("http://amigan.yatho.com/s-coding.html#interton");
        acase ELEKTOR:               openurl("http://amigan.yatho.com/s-coding.html#elektor");
        acase PIPBUG: case BINBUG:   openurl("http://amigan.yatho.com/s-coding.html#pipbin");
        acase INSTRUCTOR:            openurl("http://amigan.yatho.com/s-coding.html#si50");
        acase TWIN:                  openurl("http://amigan.yatho.com/s-coding.html#twin");
        acase CD2650:                openurl("http://amigan.yatho.com/s-coding.html#cd2650");
        acase PHUNSY:                openurl("http://amigan.yatho.com/s-coding.html#phunsy");
        acase SELBST:                openurl("http://amigan.yatho.com/s-coding.html#selbst");
        acase MIKIT:                 openurl("http://amigan.yatho.com/s-coding.html#mikit");
        acase ZACCARIA: case MALZAK: openurl("http://amigan.yatho.com/s-coding.html#coin-ops");
        }
    acase MENUITEM_GAMINGGUIDE:
        switch (machine)
        {
        case  ARCADIA:               strcpy((char*) ggstring, "http://amigan.yatho.com/agg/");
        acase INTERTON:              strcpy((char*) ggstring, "http://amigan.yatho.com/igg/");
        acase ELEKTOR:               strcpy((char*) ggstring, "http://amigan.yatho.com/egg/");
        acase PIPBUG: case BINBUG:   openurl("http://amigan.yatho.com/s-coding.html#pipbin");
        acase INSTRUCTOR:            openurl("http://amigan.yatho.com/s-coding.html#si50");
        acase TWIN:                  openurl("http://amigan.yatho.com/s-coding.html#twin");
        acase CD2650:                openurl("http://amigan.yatho.com/s-coding.html#cd2650");
        acase PHUNSY:                openurl("http://amigan.yatho.com/s-coding.html#phunsy");
        acase SELBST:                openurl("http://amigan.yatho.com/s-coding.html#selbst");
        acase MIKIT:                 openurl("http://amigan.yatho.com/s-coding.html#mikit");
        acase ZACCARIA: case MALZAK: openurl("http://amigan.yatho.com/s-coding.html#coin-ops");
        acase TYPERIGHT:             openurl("http://amigan.1emu.net/aw/type-right.txt");
        }
        if
        (   machine == ARCADIA
         || machine == INTERTON
         || machine == ELEKTOR
        )
        {   if
            (   whichgame != -1
             && known[whichgame].ggnode[0]
            )
            {   strcat((char*) ggstring, "#");
                strcat((char*) ggstring, known[whichgame].ggnode);
            }
            openurl(ggstring);
}   }   }

EXPORT void docommand2(int which)
{   switch (which)
    {
    // "Settings" menu
    case MENUOPT_ELEKTERMINAL:
    case MENUOPT_LCVDU_NARROW:
    case MENUOPT_LCVDU_WIDE:
    case MENUOPT_RADIOBULLETIN:
    case MENUOPT_SVT100:
    case MENUOPT_VT100:
        switch (which)
        {
        case  MENUOPT_ELEKTERMINAL:  pipbug_vdu = VDU_ELEKTERMINAL;
        acase MENUOPT_LCVDU_NARROW:  pipbug_vdu = VDU_LCVDU_NARROW;
        acase MENUOPT_LCVDU_WIDE:    pipbug_vdu = VDU_LCVDU_WIDE;
        acase MENUOPT_RADIOBULLETIN: pipbug_vdu = VDU_RADIOBULLETIN;
        acase MENUOPT_SVT100:        pipbug_vdu = VDU_SVT100;
        acase MENUOPT_VT100:         pipbug_vdu = VDU_VT100;
        }
        if (pipbug_vdu != VDU_RADIOBULLETIN)
        {   usemargins = FALSE;
        }
        reset_vdu(TRUE);
        calc_margins();
        draw_margins();
        fixupcolours();
#ifdef WIN32
        updatemenu(MENUFAKE_PIPBUGVDU);
        resize(size, TRUE);
        redrawscreen();
#endif
#ifdef AMIGA
        pending |= PENDING_RESIZE; // redrawscreen() will get called eventually
#endif
        reopen_subwindow(SUBWINDOW_CONTROLS);
}   }

EXPORT void updatemenus(void)
{   int i;

    if
    (   !MainWindowPtr
#ifdef AMIGA
     || !MenuPtr
#endif
    )
    {   return;
    }

    ghost_menu(   MENUMENU_DEBUG         , MN_DEBUG      ,                    showdebugger[wsm]
#ifdef WIN32
                                                                                                && (!cheevos || !RA_HardcoreModeIsActive())
#endif
                                                                                                                                           );
    ghost_submenu(MENUMENU_DEBUG_DISK    , MN_DEBUG      , IN_DEBUG_DISK    , HASDISK);
    ghost_submenu(MENUMENU_DEBUG_EDIT    , MN_DEBUG      , IN_DEBUG_EDIT    , machine != PONG && machine != TYPERIGHT);
    ghost_submenu(MENUMENU_DEBUG_LOG     , MN_DEBUG      , IN_DEBUG_LOG     , machine != PONG && machine != TYPERIGHT);
    ghost_submenu(MENUMENU_DEBUG_BP      , MN_DEBUG      , IN_DEBUG_BP      , machine != PONG && machine != TYPERIGHT);
    ghost_submenu(MENUMENU_DEBUG_SYMBOLS , MN_DEBUG      , IN_DEBUG_SYMBOLS , machine != PONG && machine != TYPERIGHT);
    ghost_submenu(MENUMENU_DEBUG_TOOLS   , MN_DEBUG      , IN_DEBUG_TOOLS   , machine != PONG && machine != TYPERIGHT);
    ghost_submenu(MENUMENU_DEBUG_OPTIONS , MN_DEBUG      , IN_DEBUG_OPTIONS , machine != PONG && machine != TYPERIGHT);
    ghost_submenu(MENUMENU_DEBUG_GRAPHICS, MN_DEBUG      , IN_DEBUG_GRAPHICS, machine != INSTRUCTOR && machine != MIKIT && machine != PONG && machine != TYPERIGHT);
    ghost_submenu(MENUMENU_LEFT          , MN_PERIPHERALS, IN_LEFT          , machines[machine].joystick);
    ghost_submenu(MENUMENU_RIGHT         , MN_PERIPHERALS, IN_RIGHT         , machines[machine].joystick);
    ghost_submenu(MENUMENU_BIOS          , MN_SETTINGS   , IN_BIOS          , machine == ELEKTOR || machine == PIPBUG || machine == BINBUG || machine == CD2650 || machine == PHUNSY || machine == SELBST);
    ghost_submenu(MENUMENU_DOS           , MN_SETTINGS   , IN_DOS           , machine == BINBUG || machine == TWIN || machine == CD2650);
    ghost_submenu(MENUMENU_FILTERS       , MN_SETTINGS   , IN_FILTERS       , size >= 2
#ifdef WIN32
                                                                                        || (machine != INSTRUCTOR && machine != MIKIT && machine != TYPERIGHT) || d3d
#endif
                                                                                                                                                                     );
    ghost_submenu(MENUMENU_MACHINE       , MN_SETTINGS   , IN_MACHINE       , !crippled);
    ghost_submenu(MENUMENU_SPRITES       , MN_SETTINGS   , IN_SPRITES       , machine == ARCADIA || machine == PONG || machines[machine].pvis);
    ghost_submenu(MENUMENU_TRAINERS      , MN_SETTINGS   , IN_TRAINERS      , (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR || machines[machine].coinop || machine == PIPBUG || machine == BINBUG)
#ifdef WIN32
                                                                              && (!cheevos || !RA_HardcoreModeIsActive())
#endif
                                                                                                                    );
    ghost_submenu(MENUMENU_VDU           , MN_SETTINGS   , IN_VDU           , machine == PIPBUG || machine == CD2650);

    for (i = 0; i < MENUITEMS; i++)
    {   updatemenu(i);
}   }
