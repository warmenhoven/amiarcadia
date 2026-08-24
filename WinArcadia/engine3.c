// INCLUDES---------------------------------------------------------------

#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
    #include "resource.h"
    #include <commctrl.h>
    typedef unsigned char bool;
    #include "RA_Interface.h"
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
    #include <conio.h>
#endif

#ifdef AMIGA
    #include <math.h>

    #include <intuition/intuition.h>
    #include <intuition/intuitionbase.h>
    #define ALL_REACTION_CLASSES
    #define ALL_REACTION_MACROS
    #include <reaction/reaction.h>
    #include <proto/graphics.h>
    #include <proto/intuition.h>
    #include <proto/locale.h>
    #include <gadgets/colorwheel.h>
    #include <gadgets/gradientslider.h>
    #ifndef __MORPHOS__
         #include <gadgets/clock.h>
    #endif
#endif

#ifndef PI
    #define PI 3.141592653589793
#endif

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"
#ifdef AMIGA
    #include "amiga.h"
#endif

#include "engine3.h"

// DEFINES----------------------------------------------------------------

// #define DISKLINE
// whether to show exact head position as a radial line (but leaves crap
// onscreen in some situations) (WinArcadia only)

// #define TESTSECTORLAYOUT
// whether to show sectorpoint coords for fine-tuning of values
// (WinArcadia only)

// #defined COLOURSTATUSBAR
// whether to colourize the (entire) status bar according to the floppy
// drive mode (WinArcadia only)

// #define SHOWEDGES
// whether to show the edges of the papertape

// #define MONOPCODECOLOURS
// whether to show the colour of the current instruction in the monitor
// by filling the opcode gadget background (disabled by default as the
// flashing would annoy some users) (AmiArcadia only)

// #define TOURNAMENTHISCORES
// whether high scores should be cleared to Basque Tournament records
// (otherwise to zero)

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT UBYTE   guestkey                 = NC,
               ininterrupt,
               startaddr_h,
               startaddr_l;
EXPORT FLAG    halted                   = FALSE,
               paperreaderenabled,
               paperpunchenabled,
               reghost                  = FALSE,
               resetdisabled;
EXPORT TEXT    Clock2650Str[COMMASTRLEN],
               controltip[5][64 + 1],
               decpoint                 = '.',
               fn_tape[4][MAX_PATH + 1] = { "", "", "" },
               FrameStr[COMMASTRLEN],
               groupsep                 = ',',
               gtempstring[256 + 1],
               imgbits[8 + 1],
               interpretstr[1024 + 1],
               papertapetitlestring[40 + MAX_PATH + 1],
               sprad[16][5 + 1],
               spritename[SPRITENAMELENGTH],
               tapeposstring[13 + 1], // enough for "MMM:SS.123456"
               tapetitlestring[40 + MAX_PATH + 1],
               TimeStr[11 + 1]          = "hh:mm:ss.uu",
               TRString[4][13 + 1],
               XStr[4 + 1]              = "", // CD2650 needs numbers >= 1000 at times
               YStr[3 + 1]              = "";
EXPORT UWORD   hostkey                  = (UWORD) -1,
               screen_iar[BOXWIDTH][BOXHEIGHT];
EXPORT ULONG   oldcycles,
               regionlevel;
EXPORT int     belling[2],
               drive_mode,
               firstrow                 = 26,
               lastrow                  = -1,
               lastcodecomment          = -1,
               master_x, master_y,
               slave_x, slave_y,
               n1, n2, n3, n4,
               palettepen               = 0,
               p1bgcol[4],
               p2bgcol[4],
               p1sprcol[6],
               p2sprcol[6],
               pvibase,
               queuepos                 = 0,
               rastn                    = INVALIDRASTER,
               runtorastline            = FALSE,
               runtointerrupt           = FALSE,
               runtoloopend             = FALSE,
               tt_kybdstate,
               udgflips                 = 0,
               whichkeyrect,
               whose[2]                 = { 3, 3 }, // probably don't need...
               whosemouse               =   3     , // ...to initialize these
               ZIPBufferSize;
EXPORT STRPTR  ZIPBufferPtr;
EXPORT struct DriveStruct drive[DRIVES_MAX];
EXPORT const int guest_to_ansi_colour[8] =
{   7, // white
    3, // yellow
    5, // purple
    1, // red
    6, // cyan
    2, // green
    4, // blue
    7, // black (-> white)
};

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       ASCREEN                  screen[BOXWIDTH][BOXHEIGHT];
IMPORT       int                      absxmin, absxmax,
                                      absymin, absymax,
                                      ambient,
                                      ax[2], // analog paddle X-coords
                                      ay[4], // analog paddle Y-coords
                                      binbug_baudrate,
                                      binbug_biosver,
                                      cd2650_biosver,
                                      cd2650_vdu,
                                      colourset,
                                      cpl,
                                      cpux,
                                      cpuy,
                                      crippled,
                                      darkenbg,
                                      drawmode,
                                      editscreen,
                                      elektor_biosver,
                                      exactspeed,
                                      fastselbst,
                                      filesize,
                                      flagline,
                                      fullscreen,
                                      game,
                                      guestrmb,
                                      interrupt_2650,
                                      inverse,
                                      key1,
                                      key2,
                                      key3,
                                      key4,
                                      keymap,
                                      limitrefreshes,
                                      lowercase,
                                      machine,
                                      memmap,
                                      nextinst,
                                      offset,
                                      other_slice,
                                      otl,
                                      papertapemode[2],
                                      pipbug_baudrate,
                                      pipbug_biosver,
                                      pipbug_periph,
                                      pipbug_vdu,
                                      phunsy_biosver,
                                      ponghertz,
                                      post,
                                      ppc,
                                      queuekeystrokes,
                                      recmode,
                                      regionstart,
                                      rotating,
                                      si50_id,
                                      si50_io,
                                      si50_is,
                                      selbst_biosver,
                                      sensitivity[2],
                                      showdebugger[2],
                                      showleds,
                                      showstatusbars[2],
                                      size,
                                      slice_2650,
                                      speedup,
                                      spriteflip,
                                      spriteflips,
                                      sprviewcolour,
                                      stage,
                                      style,
                                      supercpu,
                                      tapekind,
                                      tapemode,
                                      timeunit,
                                      tr_accuracy,
                                      tr_class,
                                      tr_errors,
                                      tr_level,
                                      tr_remaining,
                                      tr_textcursor,
                                      tr_wpm,
                                      trace,
                                      tt_scrn,
                                      udcflip,
                                      usecsperframe[2],
                                      usemargins,
                                      userlabels,
                                      usestubs,
                                      viewingdrive,
                                      viewingsprite,
                                      wheremusicmouse[2],
                                      whichcpu,
                                      whichgame,
                                      whichoverlay,
                                      whose[2],
                                      whosemouse,
                                      wide,
                                      wsm;
IMPORT       FLAG                     bangercharging,
                                      multisprite[4],
                                      priflag[32],
                                      protect[4],
                                      softrept;
IMPORT       SBYTE                    galaxia_scrolly;
IMPORT       UBYTE                    acca,
                                      accb,
                                      awga_collide,
                                      banked,
                                      binbug_joyunit,
                                      blank,
                                      bp_2nd[32768],
                                      button[2][8],
                                      cc,
                                      cd2650_chars_bmp[128][10],
                                      coinignore,
                                      coverage_io[258],
                                      editcolour,
                                      glow,
                                      guestvolume[GUESTCHANNELS],
                                      KeyMatrix[SCANCODES / 8],
                                      keys_column[7],
                                      memory[32768],
                                      oldkeys[KEYS],
                                      other_ininterrupt,
                                      PapertapeBuffer[2][PAPERTAPEMAX],
                                      psg_reg1,
                                      psg_reg2,
                                      psl, other_psl,
                                      psu, other_psu,
                                      q_bank[16][16384],
                                      r[7], other_r[7],
                                      s_tapeport,
                                      tapebyte,
                                      tapeskewage,
                                      tone,
                                      tt_kybdcode,
                                      u_bank[16][2048],
                                      vdu[MAX_VDUCOLUMNS][MAX_VDUROWS];
IMPORT       UWORD                    console[4],
                                      guestpitch[GUESTCHANNELS],
                                      iar, other_iar,
                                      ix,
                                      keypads[2][NUMKEYS],
                                      mirror_r[32768],
                                      mirror_w[32768],
                                      oldiar,
                                      pc,
                                      ras[8], other_ras[8],
                                      sp,
                                      temp_console[4],
                                      temp_keypads[2][NUMKEYS];
IMPORT       ULONG                    analog,
                                      arcadia_bigctrls,
                                      arcadia_viewcontrolsas,
                                      asicreads[32768],
                                      asicwrites[32768],
                                      coverage[32768],
                                      cpb,
                                      cycles_2650,
                                      elektor_bigctrls,
                                      frames,
                                      interton_bigctrls,
                                      inverttape,
                                      keyframes[SCANCODES],
                                      lb_snd,
                                      mikit_bigctrls,
                                      papertapelength[2],
                                      papertapeprotect[2],
                                      papertapewhere[2],
                                      paused,
                                      pong_machine,
                                      pong8550_viewcontrolsas,
                                      pong8600_viewcontrolsas,
                                      region,
                                      samplewhere,
                                      si50_bigctrls,
                                      swapped,
                                      tape_hz,
                                      tapewriteprotect,
                                      tt_kybdtill,
                                      tt_scrntill,
                                      turbo,
                                      viewmemas,
                                      viewmemas2,
                                      viewpadsas;
IMPORT       char                     mn[1024 + 1];
IMPORT       double                   samplewhere_f;
IMPORT       TEXT                     asciiname_short[259][3 + 1],
                                      asciiname_long[259][9 + 1],
                                      datatip[1024 + 1],
                                      file_game[MAX_PATH + 1],
                                      fn_asm[MAX_PATH + 1],
                                      fn_game[MAX_PATH + 1],
                                      MemoryStr[MEMGADGETS][2 + 1],
                                      olduserinput[HISTORYLINES][MAX_PATH + 80 + 1],
                                      path_disks[MAX_PATH + 1],
                                      path_games[MAX_PATH + 1],
                                      path_projects[MAX_PATH + 1],
                                      thequeue[QUEUESIZE],
                                      tr_gameletter,
                                      userinput[MAX_PATH + 80 + 1];
IMPORT       STRPTR                   colournames[GUESTCOLOURS + 1];
IMPORT       MEMFLAG                  memflags[ALLTOKENS];
IMPORT       UBYTE                   *IOBuffer,
                                     *TapeCopy;
IMPORT       FILE                    *MacroHandle,
                                     *TapeHandle;
IMPORT       struct CanvasStruct      canvas[CANVASES];
IMPORT       struct ConditionalStruct wp[ALLTOKENS];
IMPORT       struct HiScoreStruct     hiscore[HISCORES];
IMPORT       struct HostMachineStruct hostmachines[MACHINES];
IMPORT       struct IdealStruct       idealfreq_ntsc[256],
                                      idealfreq_pal[256];
IMPORT       struct IOPortStruct      ioport[258];
IMPORT       struct KeyNameStruct     keyname[SCANCODES];
IMPORT       struct MachineStruct     machines[MACHINES];
IMPORT       struct MemMapInfoStruct  memmapinfo[MEMMAPS];
IMPORT       struct OpcodeStruct      opcodes_2650[2][256];
IMPORT       struct RTCStruct         rtc;
IMPORT       struct SubWindowStruct   subwin[SUBWINDOWS];
IMPORT       struct VariantStruct     variantinfo[VARIANTS];
IMPORT const TEXT                     arcadia_chars[64 + 1],
                                      elektor_chars1[256 + 1],
                                      pswbit[STYLES][9 + 1];
IMPORT const UBYTE                    arcadia_pdg[2][64][8],
                                      dg640_chars[128][DG640_CHARHEIGHT],
                                      from_a[2][24],
                                      phunsy_chars[128][8],
                                      phunsy_gfx[16][8],
                                      table_opcolours_2650[2][256],
                                      tr_chars[91][7];
IMPORT const UWORD                    pvi_spritedata[4];
IMPORT const ULONG                    defpencolours[COLOURSETS][GUESTCOLOURS];
IMPORT const struct CodeCommentStruct codecomment[];
IMPORT const struct KeyInfoStruct     keyinfo[KEYINFOS][KEYS];
IMPORT const struct KindStruct        filekind[KINDS];
IMPORT const struct KnownStruct       known[KNOWNGAMES];
IMPORT const struct LBNoteStruct      lb_note[13 + 1];
IMPORT const int                      from_astrowars_spr[8],
                                      from_galaxia_spr[8],
                                      from_lb_spr[8],
                                      from_malzak_spr[8],
                                      memmap_to_smlimage[MEMMAPS],
                                      to_galaxia_spr[8];
IMPORT const STRPTR                   ccstring[4][4],
                                      classes[4][4],
                                      overlays[OVERLAYS][33];
#ifdef AMIGA
    IMPORT       FLAG                  capslock;
    IMPORT       UBYTE                 bytepens[PENS];
    IMPORT       LONG                  emupens[EMUBRUSHES],
                                       guestpens[GUESTCOLOURS];
    IMPORT       ULONG                 longpens[PENS],
                                       pending,
                                       viewpadsas2;
    IMPORT       int                   crippled,
                                       foundgames,
                                       morphos,
                                       pipbug_vdu,
                                       showsidebars[2],
                                       throb;
    IMPORT       struct Catalog*       CatalogPtr;
    IMPORT       struct ClassLibrary*  ClockBase;
    IMPORT       struct IntuitionBase* IntuitionBase;
    IMPORT       struct Window        *MagnifierWindowPtr,
                                      *MainWindowPtr;
    IMPORT       struct PaletteStruct  pencolours[COLOURSETS][PENS];
    IMPORT       struct Gadget*        gadgets[GIDS + 1];
    IMPORT       struct Image*         images[IMAGES];
    IMPORT const int                   guest_to_emupen[8];
#endif
#ifdef WIN32
    IMPORT       FLAG                  capslock,
                                       incli,
                                       repaintmemmap;
    IMPORT       UBYTE                 fgtable[BOXHEIGHT][BOXWIDTH];
    IMPORT       int                   bezel,
                                       CatalogPtr, // APTR doesn't work
                                       cheevos,
                                       joys;
    IMPORT       float                 dividend;
    IMPORT       HBRUSH                hBrush[EMUBRUSHES],
                                       hSpriteBrush[9];
    IMPORT       HDC                   MusicRastPtr,
                                       StaveRastPtr;
    IMPORT       HICON                 bigicon,
                                       cpuicon[2],
                                       smlicon;
    IMPORT       HMENU                 MenuPtr;
    IMPORT       HWND                  hStatusBar,
                                       MagnifierWindowPtr,
                                       MainWindowPtr;
    IMPORT       UINT                  storedcode,
                                       storedaltcode,
                                       storedmenu1,
                                       storedmenu2;
    IMPORT       ULONG                 pencolours[COLOURSETS][PENS];
    IMPORT       STRPTR                colourname[8];
    IMPORT       HINSTANCE             InstancePtr;
    IMPORT const STRPTR                waveshape[16];
    IMPORT const struct MemMapToStruct memmap_to[MEMMAPS];
#endif

IMPORT void (* drawpixel) (int x, int y, int colour);

// MODULE VARIABLES-------------------------------------------------------

MODULE       UBYTE  memorycontents[MEMGADGETS],
                    spritecontents[16];
MODULE       SBYTE  newpsgnote[7],
                    oldpsgnote[7];
MODULE       ULONG  monitorcycles_2650,
                    monitorframes;
MODULE       int    monitorx, monitory;
#if defined(AMIGA) && !defined(__MORPHOS__)
    MODULE   ULONG  monitorsecs;
#endif
#ifdef WIN32
    MODULE   FLAG   capslockdown = FALSE;
#endif

/* MODULE STRUCTURES------------------------------------------------------

(None)

MODULE ARRAYS---------------------------------------------------------- */

MODULE const STRPTR hypergates[4] =
{ "Hyperspace and gates",
  "Hyperspace only",
  "Gates only",
  "Neither feature"
}, openings[4] =
{ "both openings",
  "top opening",
  "bottom opening",
  "neither opening"
}, mines[9] =
{ "blocked by obstacles",
  "go over obstacles",
  "destroy obstacles",
  "blckd by obstcls and vis mines",
  "blckd by obstcls but dstry vis mines",
  "go over obstacls but dstry vis mines",
  "blckd by obstcls and invis mines",
  "blckd by obstcls but dstry invis mines",
  "go over obstcls but dstry invis mines",
}, players[4] =
{ "human only",
  "human vs. human",
  "human vs. cnsle",
  "human vs. human"
}, balls[4] =
{ "normal",
  "guided",
  "catch",
  "pntrtng"
}, numplayers[3] =
{ "Human vs. console game",
  "Human vs. human game",
  "Console vs. console demo"
}, airseaattack[5] =
{ "Ship vs. large submarine",
  "Ship vs. small submarine",
  "Submarine vs. aeroplane",
  "Shooting gallery",
  "Shooting gallery"
}, airseabattle[5] =
{ "Ship vs. large submarine",
  "Ship vs. small submarine",
  "Rocket launcher vs. aeroplane",
  "Shooting gallery",
  "Shooting gallery"
}, carraces[5] =
{ "Grand prix",
  "Rally",
  "Slalom (1st track)",
  "Slalom (2nd track)",
  "Slalom (3rd track)"
}, features[20] =
{ "no features",
  "no features",
  "controlled depth charge",
  "controlled depth charge",
  "no features",
  "no features",
  "controlled depth charge",
  "controlled depth charge",
  "no features",
  "controlled speed",
  "controlled missiles",
  "controlled speed and missiles",
  "no features",
  "no features",
  "controlled missiles",
  "controlled missiles",
  "rebounding missiles",
  "rebounding missiles",
  "controlled rebounding missiles",
  "controlled rebounding missiles",
}, headon[9] =
{ "Black human vs. 1 purple console. Avoid car",
  "Black human vs. 2 purple consoles. Avoid cars",
  "Red human vs. 1 purple console. Hit car",
  "Red human vs. 2 purple consoles. Hit cars",
  "Red hmn vs. 1 ylw cnsle, then grn hmn vs. 1 ylw cnsle. Avoid car",
  "Red hmn vs. 2 ylw cnsls, then grn hmn vs. 2 ylw cnsls. Avoid cars",
  "Red hmn vs. 1 blue cnsle, then grn hmn vs. 1 blue cnsle. Hit car",
  "Red hmn vs. 2 blue cnsls, then grn hmn vs. 2 blue cnsls. Hit cars",
  "Black human vs. blue human. Alternating"
}, hunting1[4] =
{ "No movement",
  "Move bullet",
  "Move man",
  "Turn man"
}, hunting2[8] =
{ "unltd normal shots + time",
  "unltd normal shots + time",
  "unltd normal shots + time",
  "unltd rflcting shots + time",
  "unltd normal shots, ltd time",
  "20 normal shots, unltd time",
  "20 normal shots, ltd time",
  "20 rflctng shots, ltd time"
}, shootgal[4] =
{ "unlimited normal shots",
  "unlimited normal shots",
  "20 normal shots",
  "20 reflecting shots"
}, invaders[4] =
{ "blue",
  "black",
  "red",
  "purple"
}, olympics1[12] =
{ "tennis singles",
  "tennis doubles",
  "football",
  "ice hockey",
  "table tennis",
  "volleyball",
  "basketball 1",
  "basketball 2",
  "pinball",
  "wipe-out",
  "pinball",
  "wipe-out"
}, olympics2[5] =
{ "amateur",
  "progress",
  "professional",
  "trick 1",
  "trick 2"
}, planetdefender[4] =
{ "Yellow",
  "Red",
  "Green",
  "White"
}, superinvaders[15] =
{ "auto launching + detonation",
  "auto launching + detonation",
  "mnl launching but auto detonation",
  "mnl launching but auto detonation",
  "auto launching but mnl detonation",
  "auto launching but mnl detonation",
  "auto launching + detonation",
  "auto launching + detonation",
  "mnl launching but auto detonation",
  "mnl launching but auto detonation",
  "auto launching but mnl detonation",
  "auto launching but mnl detonation",
  "auto launching + detonation",
  "mnl launching but auto detonation",
  "auto launching but mnl detonation"
}, i_combat[26] =
{ "Tanks, day, clear, long range",              // game  1
  "Tanks, day, absrbnt brcds #2, long range",
  "Tanks, day, absrbnt brcds #3, long range",
  "Tanks, day, absrbnt brcds #4, long range",
  "Tanks, day, minefields #5, long range",      // game  5
  "Tanks, day, minefields #6, long range",
  "Tanks, day, rbndng brcds #2, long range",
  "Tanks, day, rbndng brcds #4, long range",
  "Tanks, night, clear, long range",
  "Tanks, night, absrbnt brcds #2, long range", // game 10
  "Tanks, night, absrbnt brcds #3, long range",
  "Tanks, night, absrbnt brcds #4, long range",
  "Tanks, night, rbndng brcds #2, long range",
  "Tanks, night, rbndng brcds #4, long range",
  "Tanks, day, clear, short range",             // game 15
  "Tanks, night, clear, short range",
  "Single biplanes, clouds #5, long range",
  "Double biplanes, clouds #5, long range",
  "Single biplanes, clouds #6, long range",
  "Double biplanes, clouds #6, long range",     // game 20
  "Single biplanes, clouds #5, short range",
  "Single jets, clouds #5, long range",
  "Double jets, clouds #5, long range",
  "Single jets, clouds #6, long range",
  "Double jets, clouds #6, long range",         // game 25
  "Single jets, clouds #6, short range"         // game 26
}, musicalgames[8] =
{ "Free play",
  "Plays \"When the Saints Go Marching In\"",
  "Simon, 1 human copying console, 4 notes",
  "Simon, 2 humans copying console, 4 notes",
  "Simon, 1 human copying console, 6 notes",
  "Simon, 2 humans copying console, 6 notes",
  "Simon, 2 humans copying 2 humans, 4 notes",
  "Simon, 2 humans copying 2 humans, 6 notes"
}, wintersports[8] =
{ "Ski jump, no wind, 1 player",
  "Ski jump, no wind, 2 players",
  "Ski jump, steady wind force from varying direction, 1 player",
  "Ski jump, steady wind force from varying direction, 2 players",
  "Ski jump, varying wind force + direction, 1 player",
  "Ski jump, varying wind force + direction, 2 players",
  "Slalom, 1 player",
  "Slalom, 1 player", // this is no mistake
}, treasurehunt[4] =
{ "Normal",
  "Normal",
  "Pictorial symbols",
  "Arrows + numerals"
}, challenge[7] =
{ "left human vs. right human",
  "easy console vs. human",
  "normal console vs. human",
  "hard console vs. human",
  "human vs. easy console",
  "human vs. normal console",
  "human vs. hard console"
}, spacewar[4] =
{ "Enemy attack",
  "Space docking manoeuvre",
  "Duel in outer space",
  "Double attack"
}, monsterman[3] =
{ "easy",
  "normal",
  "hard"
}, supermaze[6] =
{ "Normal maze",
  "Hidden maze",
  "Invisible maze",
  "Changing maze",
  "Moving maze",
  "Cat and mouse"
}, i_bowling[16] =
{ "Toppling in Twos, 2 alternating players, 9 pins",
  "Toppling with Figures, 2 alternating players, 1/3/4/1/2/3 pins",
  "Foxhunting, 2 alternating players, 9 pins",
  "Foxhunting with Figures, 2 alternating players, 1/3/4/1/2/3 pins",
  "Headpin, 2 alternating players, 9 pins",
  "Bowling, 2 alternating players, 10 pins",
  "Tandem, 2 simultaneous players, 9 pins",
  "Toppling with Figures, 2 simultaneous players, 1/3/4/1/2/3 pins",
  "Foxhunting, 2 simultaneous players, 9 pins",
  "Foxhunting with Figures, 2 simultaneous players, 1/3/4/1/2/3 pins",
  "Headpin Simultaneous, 2 simultaneous players, 9 pins",
  "Tandem Bowling, 2 simultaneous players, 10 pins",
  "Toppling, 1 player, 9 pins",
  "Toppling with Figures, 1 player, 1/3/4/1/2/3 pins",
  "Headpin, 1 player, 9 pins",
  "Bowling, 1 player, 10 pins"
}, shootout[3] =
{ "2 players, unlimited",
  "2 players, 6",
  "1 player, 99"
}, casino[3] =
{ "Roulette, 1/2 players",
  "One-Armed Bandit, 1 player",
  "One-Armed Bandit, 2 players",
}, draughts[15] =
{ "Normal rules, human vs. human, must take men",
  "Normal rules, human vs. human, can take men",
  "Normal rules, human vs. easy console, must take men",
  "Normal rules, human vs. easy console, can take men",
  "Normal rules, human vs. normal console, must take men",
  "Normal rules, human vs. normal console, can take men",
  "Normal rules, human vs. hard console, must take men",
  "Normal rules, human vs. hard console, can take men",
  "Inverted rules, human vs. easy console, must take men",
  "Inverted rules, human vs. easy console, can take men",
  "Inverted rules, human vs. normal console, must take men",
  "Inverted rules, human vs. normal console, can take men",
  "Inverted rules, human vs. hard console, must take men",
  "Inverted rules, human vs. hard console, can take men",
  "Board editor"
}, backgammon[4] =
{ "Human (rolled by console) vs. human (rolled by console)",   // "4"
  "Human (rolled by console) vs. console (rolled by console)", // "1"
  "Human (rolled by human) vs. console (rolled by console)",   // "2"
  "Human (rolled by human) vs. console (rolled by human)"      // "3"
}, horseracing[4] =
{ "no obstacles",
  "water jump",
  "double oxer",
  "changing obstacles"
}, i_capture[3] =
{ "human vs. human",
  "human vs. console",
  "console vs. human"
}, chess[4] =
{ "normal",
  "random",
  "forwards",
  "backwards"
};

// MODULE FUNCTIONS-------------------------------------------------------

MODULE int buttontranslate(int player, int which);
MODULE void calcrunningtime(void);
MODULE void tape_foundcolon(UBYTE value);
MODULE void tape_loaded(void);
MODULE void tape_read(UBYTE value);
MODULE void tape_rawread(UBYTE value);
MODULE void tape_rawstart(void);
MODULE void tape_rawreadbit(void);
MODULE void tape_wrotecolon(TEXT colon);
MODULE void tape_wrotebyte(UBYTE value);
#ifdef WIN32
    MODULE void getextrauviinfo(int gid);
    MODULE void getextrapviinfo(int gid);
    MODULE void getextraintertonelektorinfo(int gid);
    MODULE void getextrapsginfo(int gid);
#endif

// music subwindow support
MODULE void ghost_notes(void);

#ifdef AMIGA
    MODULE FLAG alt(void);
#endif

// CODE-------------------------------------------------------------------

EXPORT void change_machine(int whichmachine, int whichmemmap, FLAG user)
{   int  i,
         oldwide,
         oldwidth, oldheight,
         x, y;
#ifdef WIN32
    int  consoleid;
    FLAG oldcheevos;
#endif

    if (user && crippled)
    {   return; // important!
    }

#ifdef WIN32
    if (cheevos)
    {
#ifdef LOGCHEEVOS
        zprintf(TEXTPEN_VERBOSE, "RA_ConfirmLoadNewRom(FALSE)\n");
#endif
        if (!RA_ConfirmLoadNewRom(FALSE))
        {   return;
    }   }

    storedcode    =
    storedaltcode =  0;
    storedmenu1   =
    storedmenu2   = -1;
#endif

    if (user)
    {   filesize  =  0;
        game      = FALSE;
        whichgame = -1;
        if (usestubs)
        {   switch (whichmachine)
            {
            case  ARCADIA:  whichgame  = ARCADIASTUBPOS;
            acase INTERTON: whichgame  = INTERTONSTUBPOS;
    }   }   }
    elif (whichgame == 65535)
    {   whichgame = -1; // important!
    }

    for (x = 0; x < BOXWIDTH; x++)
    {   for (y = 0; y < BOXHEIGHT; y++)
        {   screen[x][y] = GREY1;
#ifdef WIN32
            fgtable[y][x] = 0;
#endif
    }   }

    oldwidth   = machines[machine].width;
    oldheight  = machines[machine].height;
    oldwide    = wide;

    malzak_savenvram();

    if (memmap != whichmemmap)
    {   macro_stop();
        sound_off(FALSE);
        ff_off();
        printer_savepartial(0);
        printer_savepartial(1);
        drawmode = 0;

        machine   = whichmachine;
        memmap    = whichmemmap;
        if (machine != ARCADIA && machine != PONG)
        {   region = (ULONG) machines[machine].region;
        }
        wide = machines[machine].wide;
        calc_margins(); // this sets machines[machine].width and machines[machine].height
    }

#ifdef WIN32
    oldcheevos = cheevos;
    if (machine != ARCADIA && machine != INTERTON && machine != ELEKTOR)
    {   cheevos = FALSE;
    }
#endif

    // now reconfigure the machine according to sensed game

    switch (machine)
    {
    case INTERTON:
        if
        (   whichgame == I_TETRISPOS1
         || whichgame == I_TETRISPOS2
         || whichgame == I_TETRISPOS3
        )
        {   machines[machine].digipos[0] = 0x0C;
            machines[machine].digipos[1] = 0x34;
            machines[machine].digipos[2] = 0x58;
        } else
        {   machines[machine].digipos[0] =    1;
            machines[machine].digipos[1] =  112;
            machines[machine].digipos[2] =  225;
        }
    acase ELEKTOR:
        if
        (   whichgame == ENTERPRISE1POS1
         || whichgame == ENTERPRISE1POS2
         || whichgame == HAMISH1POS1
         || whichgame == HAMISH1POS2
         || whichgame == HAMISH1POS3
         || whichgame == LAUNCHINGPOS1
         || whichgame == LAUNCHINGPOS2
        )
        {   machines[machine].digipos[0] =   0;
            machines[machine].digipos[1] =   1;
         // machines[machine].digipos[2] = 225;
        } else
        {   machines[machine].digipos[0] =   1;
            machines[machine].digipos[1] = 111;
         // machines[machine].digipos[2] = 225;
    }   }

    ppc = machines[machine].ppc;
    if (whichgame == -1)
    {   if (machine == ARCADIA || machine == INTERTON)
        {   cpl = 227;
        } elif (machine == ELEKTOR)
        {   cpl = 226;
        }
        for (i = 0; i < 6; i++)
        {   p1sprcol[i] =
            p2sprcol[i] = 0;
        }
        for (i = 0; i < 4; i++)
        {   p1bgcol[i]  =
            p2bgcol[i]  = 0;
        }
        key1            = 2;
        key2            = 1;
        key3            = 3;
        key4            = 0;
        sensitivity[0]  =
        sensitivity[1]  = SENSITIVITY_DEFAULT;
        firstrow        = 26;
        lastrow         = -1;
        udgflips        =
        spriteflips     = 0;
        for (i = 0; i < 4; i++)
        {   protect[    i] =
            multisprite[i] = FALSE;
        }
        pipbug_periph   = PERIPH_PRINTER;
        whichoverlay    = memmapinfo[memmap].overlay;
        /* We don't set these:
            elektor_biosver
            pipbug_biosver
            pipbug_baudrate
            pipbug_vdu
            binbug_biosver
            binbug_baudrate
            cpb
            cd2650_biosver
            cd2650_vdu
            phunsy_biosver
            selbst_biosver
            region
            flagline
            darkenbg
            si50_io
            si50_is
            si50_id
            fastselbst */
    } else
    {   switch (machine)
        {
        case ARCADIA:
            if (known[whichgame].bios != REGION_ANY)
            {   region    = (ULONG) known[whichgame].bios;
            }
            flagline      =         known[whichgame].flagline;
        acase INTERTON:
            darkenbg      =         known[whichgame].flagline;
        acase ELEKTOR:
            elektor_biosver =       known[whichgame].bios;
            darkenbg      =         known[whichgame].flagline;
            if (whichgame == SUBMARINESRACINGPOS)
            {   ppc       = 2;
            }
        acase PIPBUG: pipbug_biosver  = known[whichgame].bios;
                      if (known[whichgame].swapped == PIPBUG_BAUDRATE_ANY)
                      {   switch (pipbug_biosver)
                          {
                          case  PIPBUG_ARTEMIS:     pipbug_baudrate = PIPBUG_BAUDRATE_4800;
                          acase PIPBUG_HYBUG:       pipbug_baudrate = PIPBUG_BAUDRATE_300;
                          acase PIPBUG_PIPBUG1BIOS: pipbug_baudrate = BAUDRATE_DEFAULTPIPBUG1;
                          acase PIPBUG_PIPBUG2BIOS: pipbug_baudrate = BAUDRATE_DEFAULTPIPBUG2;
                      }   }
                      else
                      {   pipbug_baudrate = known[whichgame].swapped;
                      }
                      if (known[whichgame].flagline != VDU_ANY)
                      {   pipbug_vdu      = known[whichgame].flagline;
                      }
                      switch (pipbug_vdu)
                      {
                      case  VDU_ELEKTERMINAL:  docommand2(MENUOPT_ELEKTERMINAL);
                      acase VDU_LCVDU_NARROW:  docommand2(MENUOPT_LCVDU_NARROW);
                      acase VDU_LCVDU_WIDE:    docommand2(MENUOPT_LCVDU_WIDE);
                      acase VDU_RADIOBULLETIN: docommand2(MENUOPT_RADIOBULLETIN);
                      acase VDU_SVT100:        docommand2(MENUOPT_SVT100);
                      acase VDU_VT100:         docommand2(MENUOPT_VT100);
                      }
                      switch (whichgame)
                      {
                      case  FURNACEPOS1:
                      case  FURNACEPOS2:
                      case  FURNACEPOS3:      pipbug_periph = PERIPH_FURNACE;
                      acase LINEARISATIEPOS1:
                      case  LINEARISATIEPOS2: pipbug_periph = PERIPH_LINEARISATIE;
                      acase MAGNETOMETERPOS:  pipbug_periph = PERIPH_MAGNETOMETER;
                      adefault:               pipbug_periph = PERIPH_PRINTER;
                      }
        acase BINBUG: binbug_biosver  = known[whichgame].bios;
                      if (known[whichgame].swapped == BINBUG_BAUDRATE_ANY)
                      {   switch (binbug_biosver)
                          {
                          case BINBUG_61:  binbug_baudrate = BINBUG_BAUDRATE_150;
                                           cpb             = 6667; // 6666.6' CPU cycles per teletype bit (150 baud at 1 MHz or 300 baud at 2 MHz)
                          adefault:        binbug_baudrate = BINBUG_BAUDRATE_300;
                                           cpb             = 3333; // 3333.3' CPU cycles per teletype bit (300 baud at 1 MHz or 600 baud at 2 MHz)
                      }   }
                      else
                      {   binbug_baudrate = known[whichgame].swapped;
                      }
        acase INSTRUCTOR:
            si50_id        = (int)    known[whichgame].spriteflips;
            si50_is        = (int)    known[whichgame].udgflips;
            si50_io        =          known[whichgame].flagline;
        acase CD2650:
            cd2650_biosver =          known[whichgame].bios;
            cd2650_vdu     =          known[whichgame].flagline;
        acase PHUNSY:
            phunsy_biosver =          known[whichgame].bios;
            startaddr_h    = (UBYTE) (known[whichgame].startaddr / 256);
            startaddr_l    = (UBYTE) (known[whichgame].startaddr % 256);
        acase SELBST:
            selbst_biosver =          known[whichgame].bios;
            fastselbst     =          known[whichgame].flagline ? FALSE : TRUE;
            machines[SELBST].cpf = fastselbst ? 20000.0 : 0.02;
            // is update_speed() needed?
        }
        sensitivity[0]    =
        sensitivity[1]    = (int)   known[whichgame].sensitivity;
        analog            = (ULONG) known[whichgame].analog;
        if (machine != PIPBUG)
        {   swapped       = (ULONG) known[whichgame].swapped;
        }
        key1              = (int)   known[whichgame].key1;
        key2              = (int)   known[whichgame].key2;
        key3              = (int)   known[whichgame].key3;
        key4              = (int)   known[whichgame].key4;
        for (i = 0; i < 4; i++)
        {   p1bgcol[i]    = (int)   known[whichgame].p1bgcol[i];
            p2bgcol[i]    = (int)   known[whichgame].p2bgcol[i];
        }
        for (i = 0; i < 6; i++)
        {   p1sprcol[i]   = (int)   known[whichgame].p1sprcol[i];
            p2sprcol[i]   = (int)   known[whichgame].p2sprcol[i];
        }
        if (known[whichgame].firstcodecomment != -1)
        {   // assert(known[whichgame].lastcodecomment != -1);
            for (i = known[whichgame].firstcodecomment; i <= known[whichgame].lastcodecomment; i++)
            {   memflags[codecomment[i].address] |= COMMENTED;
        }   }
        whichoverlay      = (int)   known[whichgame].whichoverlay;
        if ((machine == ARCADIA || machine == INTERTON || machine == ELEKTOR) && known[whichgame].cpl)
        {   cpl           = (int)   known[whichgame].cpl;
        } else
        {   cpl           = 227;
        }

        if (machine != PIPBUG && machine != INSTRUCTOR) // because these machines use these known[] structure fields for other purposes
        {   if (known[whichgame].demultiplex || known[whichgame].spriteflips || known[whichgame].udgflips)
            {   firstrow    =  known[whichgame].firstrow;
                lastrow     =  known[whichgame].lastrow;
                udgflips    =  known[whichgame].udgflips;
                spriteflips =  known[whichgame].spriteflips;
                if (machine == INTERTON || machine == ELEKTOR)
                {   multisprite[0] = (known[whichgame].demultiplex & 0x80) ? TRUE : FALSE;
                    multisprite[1] = (known[whichgame].demultiplex & 0x40) ? TRUE : FALSE;
                    multisprite[2] = (known[whichgame].demultiplex & 0x20) ? TRUE : FALSE;
                    multisprite[3] = (known[whichgame].demultiplex & 0x10) ? TRUE : FALSE;
                    protect[    0] = (known[whichgame].demultiplex & 0x08) ? FALSE : TRUE;
                    protect[    1] = (known[whichgame].demultiplex & 0x04) ? FALSE : TRUE;
                    protect[    2] = (known[whichgame].demultiplex & 0x02) ? FALSE : TRUE;
                    protect[    3] = (known[whichgame].demultiplex & 0x01) ? FALSE : TRUE;
                } elif (machine == ARCADIA)
                {   protect[0]     = (known[whichgame].demultiplex & 0x08) ? TRUE : FALSE;
                    protect[1]     = (known[whichgame].demultiplex & 0x04) ? TRUE : FALSE;
                    protect[2]     = (known[whichgame].demultiplex & 0x02) ? TRUE : FALSE;
                    protect[3]     = (known[whichgame].demultiplex & 0x01) ? TRUE : FALSE;
            }   }
            else
            {   spriteflips = 0;
                udgflips = 0;
        }   }

#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Autosensed game #%d.\n", whichgame);
#endif
    }

    if (machines[machine].drives < 1)
    {   drive[0].inserted = FALSE;
    }
    if (machines[machine].drives < 2)
    {   drive[1].inserted = FALSE;
    }
    if (machines[machine].drives < 3)
    {   drive[2].inserted = FALSE;
    }
    if (machines[machine].drives < 4)
    {   drive[3].inserted = FALSE;
    }

    edit_screen_sanitize();

    // for nothing
    idealfreq_ntsc[0x18].hertz     = idealfreq_pal[0x18].hertz     = HZ_DS4;
    idealfreq_ntsc[0x18].name      = idealfreq_pal[0x18].name      = "D#4";
    idealfreq_ntsc[0x18].whichnote = idealfreq_pal[0x18].whichnote = NOTE_DS4;
    idealfreq_ntsc[0x18].midi      = idealfreq_pal[0x18].midi      = MIDI_DS4;
    notes[NOTE_DS4].ntscval = notes[NOTE_DS4].palval = 0x18;

    // for Super Bug
    idealfreq_ntsc[0x19].hertz     = idealfreq_pal[0x19].hertz     = HZ_D4;
    idealfreq_ntsc[0x19].name      = idealfreq_pal[0x19].name      = "d4";
    idealfreq_ntsc[0x19].whichnote = idealfreq_pal[0x19].whichnote = NOTE_D4;
    idealfreq_ntsc[0x19].midi      = idealfreq_pal[0x19].midi      = MIDI_D4;

    // for Super Bug
    idealfreq_ntsc[0x1B].hertz     = idealfreq_pal[0x1B].hertz     = HZ_CS4;
    idealfreq_ntsc[0x1B].name      = idealfreq_pal[0x1B].name      = "C#4";
    idealfreq_ntsc[0x1B].whichnote = idealfreq_pal[0x1B].whichnote = NOTE_CS4;
    idealfreq_ntsc[0x1B].midi      = idealfreq_pal[0x1B].midi      = MIDI_CS4;
    notes[NOTE_CS4].ntscval = notes[NOTE_CS4].palval = 0x1B;

    // for American Football
    idealfreq_ntsc[0x28].hertz     = idealfreq_pal[0x28].hertz     = HZ_G3;
    idealfreq_ntsc[0x28].name      = idealfreq_pal[0x28].name      = "g3";
    idealfreq_ntsc[0x28].whichnote = idealfreq_pal[0x28].whichnote = NOTE_G3;
    idealfreq_ntsc[0x28].midi      = idealfreq_pal[0x28].midi      = MIDI_G3;

    if (MainWindowPtr)
    {   close_subwindows(FALSE); // must be done before foo_setmemmap()!
    }

    switch (machine)
    {
    case  ARCADIA:     arcadia_setmemmap();
    acase INTERTON:   interton_setmemmap();
    acase ELEKTOR:     elektor_setmemmap();
    acase PIPBUG:       pipbug_setmemmap();
    acase BINBUG:       binbug_setmemmap();
    acase INSTRUCTOR:     si50_setmemmap();
    acase TWIN:           twin_setmemmap();
    acase CD2650:       cd2650_setmemmap();
    acase PHUNSY:       phunsy_setmemmap();
    acase PONG:           pong_setmemmap();
    acase SELBST:       selbst_setmemmap();
    acase MIKIT:         mikit_setmemmap();
    acase TYPERIGHT:        tr_setmemmap();
    acase MALZAK:       malzak_setmemmap();
    acase ZACCARIA:
        switch (memmap)
        {
        case  MEMMAP_ASTROWARS:                         astrowars_setmemmap();
        acase MEMMAP_GALAXIA:                           galaxia_setmemmap();
        acase MEMMAP_LASERBATTLE: case MEMMAP_LAZARIAN: lb_setmemmap();
    }   }
    fix_keyrects();
    if (exactspeed)
    {   usecsperframe[REGION_NTSC] = (int) (1000000.0 / 60.0                              );
        usecsperframe[REGION_PAL ] = (int) (1000000.0 / 50.0                              );
    } else
    {   usecsperframe[REGION_NTSC] = (int) (1000000.0 / machines[machine].fps[REGION_NTSC]);
        usecsperframe[REGION_PAL ] = (int) (1000000.0 / machines[machine].fps[REGION_PAL ]);
    }

    drawpixelroutine(); // must be before resize()!
    if
    (   MainWindowPtr
     && (   oldwide   != wide
         || oldwidth  != machines[machine].width
         || oldheight != machines[machine].height
    )   )
    {   resize(size, TRUE);
    }

#ifdef WIN32
    bigicon = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon           ), IMAGE_ICON, 32, 32, 0);
    smlicon = LoadImage(InstancePtr, MAKEINTRESOURCE(getsmallimage1(whichgame, memmap)), IMAGE_ICON, 16, 16, 0);
    if (MainWindowPtr)
    {   SendMessage(MainWindowPtr, WM_SETICON, ICON_BIG,   (LPARAM) bigicon);
        SendMessage(MainWindowPtr, WM_SETICON, ICON_SMALL, (LPARAM) smlicon);
    }
#endif

    updatepointer(FALSE, TRUE);
    setselection();

#ifdef WIN32
    if (fullscreen && MainWindowPtr)
    {   clearscreen();
    }
#endif
    for (x = 0; x < MAX_VDUCOLUMNS; x++)
    {   for (y = 0; y < MAX_VDUROWS; y++)
        {   vdu[x][y] = blank;
    }   }
    fixupcolours();

    engine_reset();
    draw_margins();
    periph_init();
    clearkybd();
    refreshkybd();
    sound_on(TRUE);

#ifdef WIN32
    if (cheevos)
    {   switch (machine)
        {
        case  ARCADIA:                 consoleid = 73;
        acase INTERTON:                consoleid = 74;
        acase ELEKTOR:                 consoleid = 75;
     // acase MALZAK:   case ZACCARIA: consoleid = 27;
        }
#ifdef LOGCHEEVOS
        zprintf
        (   TEXTPEN_VERBOSE,
            "RA_SetConsoleID(%d)\n" \
            "RA_ClearMemoryBanks()\n" \
            "RA_InstallMemoryBank()\n",
            consoleid
        );
#endif
        RA_SetConsoleID(consoleid);
        RA_ClearMemoryBanks();
        switch (machine)
        {
        case  ARCADIA:  RA_InstallMemoryBank(0, AByteReader , AByteWriter ,  0x300); // $1800..$1AFF ( 768 bytes)
        acase INTERTON: RA_InstallMemoryBank(0, IByteReader1, IByteWriter1,  0x400); // $1800..$1BFF (1024 bytes)
                        RA_InstallMemoryBank(1, IByteReader2, IByteWriter2,  0x200); // $1E00..$1FFF ( 512 bytes)
        acase ELEKTOR:  RA_InstallMemoryBank(0, EByteReader , EByteWriter , 0x1800); // $0800..$1FFF (6144 bytes)
        }
#ifdef LOGCHEEVOS
        zprintf(TEXTPEN_VERBOSE, "RA_ActivateGame(0)\n");
#endif
        RA_ActivateGame(0);
    }
    elif (oldcheevos)
    {   remove_cheevos(TRUE);
    }
    updatemenu(MENUITEM_CHEEVOS2);
#endif

    generate_autotext(); // to update glyph and game name on status bar
    setselection();

    if (user)
    {   set_filename();
        update_opcodes(); // because our INTERTON emulation has a different length for the $10 opcode
        updatemenus();
        update_toolbar();
        settitle();
#ifdef WIN32
        free_display();
        calc_size();
        make_display();
#endif
        redrawscreen(); // needed for eg. AmiPIPBUG dividers
        reopen_subwindows();
}   }

EXPORT void engine_reset(void)
{   int i;

    // joysticks
    ax[0]           =
    ax[1]           =
    ay[0]           =
    ay[1]           =
    ay[2]           =
    ay[3]           = machines[machine].digipos[1];

    sound_reset();

    // coin-ops
    awga_collide    =
    coinignore      = 0;
    galaxia_scrolly = 0;
    // don't touch malzak_bank!

    clearruntos();
    clearcoverage();
    for (i = 0; i < 258; i++)
    {   ioport[i].contents = 0; // this must precede twin_reset()
    }
    if (machine == PIPBUG)
    {   ioport[16].contents = 7; // to avoid negative sign
    }

    for (i = 0; i <= 6; i++)
    {   r[i]           =
        other_r[i]     = 0;
    }
    for (i = 0; i <= 7; i++)
    {   ras[i]         =
        other_ras[i]   = 0;
    }
    psu                =
    psl                =
    other_psu          =
    other_psl          = 0;
    iar                = // this must precede foo_reset()
    other_iar          = 0;
    ininterrupt        =
    other_ininterrupt  = 8;
    slice_2650         =
    other_slice        = 0;
    halted             = FALSE;
    interrupt_2650     = FALSE;
    frames             =
    cycles_2650        =
    oldcycles          =
    nextinst           = 0;
    paperreaderenabled =
    paperpunchenabled  = TRUE;
    belling[0]         =
    belling[1]         = 0;

    switch (machine)
    {
    case  ARCADIA:    arcadia_reset();
    acase INTERTON:  interton_reset();
    acase ELEKTOR:    elektor_reset();
    acase PIPBUG:      pipbug_reset();
    acase BINBUG:      binbug_reset();
    acase INSTRUCTOR:    si50_reset();
    acase TWIN:          twin_reset();
    acase CD2650:      cd2650_reset();
    acase PHUNSY:      phunsy_reset();
    acase SELBST:      selbst_reset();
    acase MIKIT:        mikit_reset();
    acase PONG:          pong_newgame();
    acase TYPERIGHT:       tr_reset();
    }

    whichcpu = 0; // this must succeed twin_reset()
    oldiar = iar;
    update_monitor(FALSE);
    reset_fps();
}

EXPORT int engine_load(FLAG silent)
{   TRANSIENT int   rc;
    TRANSIENT ULONG prev_region;
    TRANSIENT FILE* TheLocalHandle /* = NULL */ ; // LocalHandle is a variable of winbase.h
    PERSIST   TEXT  tempstring[MAX_PATH + 80];

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Engine is attempting to load \"%s\"...\n", fn_game);
#endif

    if (IsZip((STRPTR) fn_game))
    {   if (!loadzip((STRPTR) fn_game, FALSE, silent))
        {   return 0;
        }
        filesize = ZIPBufferSize;
        IOBuffer = (UBYTE*) ZIPBufferPtr;
    } else
    {   filesize = (int) getsize((STRPTR) fn_game);
        alloc_iobuffer((ULONG) filesize);

        if (!(TheLocalHandle = fopen((char*) fn_game, "rb"))) // just cast for lint
        {   sprintf
            (   (char*) tempstring,
                "%s \"%s\"!\n\n",
                LLL(MSG_ERROR_CANTLOAD, "Can't load"),
                fn_game
            ); // maybe say "Can't open \"%s\" for reading!\n\n" instead
            return 0;
        }
        if (fread(IOBuffer, (size_t) filesize, 1, TheLocalHandle) != 1)
        {   fclose(TheLocalHandle);
            // TheLocalHandle = NULL;
            return 0;
        }
        fclose(TheLocalHandle);
        // TheLocalHandle = NULL;
    }

    prev_region = region;
    rc = parse_bytes();
    if (rc != 3)
    {   free_iobuffer();
        if (rc != 1)
        {   return rc; // 0 (failure) or 2 (success but don't reset, eg. 8SVX/AIFF/ASM/BMP/IMAG/IMG/MDCR/RAW/TWIN/WAV)
    }   }

    // updatemenus(); is already done by parse_bytes()
    // update_toolbar(); is already done by parse_bytes()
    if (subwin[SUBWINDOW_DIPS].hwnd)
    {   switch (machine)
        {
        case INSTRUCTOR:
            si50_updatedips(FALSE);
        acase MALZAK:
            // assert(memmap == MEMMAP_MALZAK2);
            malzak2_updatedips();
        acase ZACCARIA:
            zaccaria_updatedips();
        acase PONG:
            pong_updatedips();
    }   }
    settitle();
    if ((machine == ARCADIA || machine == PONG) && region != prev_region && MainWindowPtr)
    {   docommand(MENUFAKE_REGION); // to close and reopen window. Needed even in full-screen mode.
    } else
    {   calc_margins();
    }
    redrawscreen();
    sound_off(TRUE);
    sound_on(TRUE);
    reopen_subwindows();

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Engine has successfully loaded \"%s\".\n", fn_game);
#endif

    return 1; // success
}

EXPORT void update_monitor(FLAG force)
{   update_controls();
    update_memory(force);
    update_music();
    update_tapedeck(force);
    update_monitor_cpu(force);
    update_monitor_psgs(force);
    update_monitor_xvi(force);
    update_industrial(force);
    update_variant(force);
}

EXPORT void update_monitor_cpu(FLAG force)
{   FAST    UBYTE t;
    FAST    UWORD tt;
    FAST    int   i;
    PERSIST int   monitor_cpu,
                  monitor_errors;
    PERSIST TEXT  monitor_wpm[13 + 1],
                  monitor_accuracy[13 + 1],
                  monitor_expecting[13 + 1];

    if (!subwin[SUBWINDOW_MONITOR_CPU].hwnd)
    {   return;
    }

    if (frames != monitorframes || force)
    {
#if defined(AMIGA) && !defined(__MORPHOS__)
        if (ClockBase)
        {   float fsecs;
            int   secs;

            if (exactspeed)
            {   fsecs = (float) frames / ((region == REGION_NTSC) ? 60.0 : 50.0);
            } else
            {   fsecs = (float) frames / machines[machine].fps[region];
            }
            secs  = (int  ) fsecs;

            if (secs != (int) monitorsecs || force)
            {   monitorsecs = secs;
                SetGadgetAttrs(gadgets[GID_CP_CL1], subwin[SUBWINDOW_MONITOR_CPU].hwnd, NULL, CLOCKGA_Time, secs, TAG_DONE); // this autorefreshes
        }   }
#endif
        monitorframes = frames;
        longcomma(monitorframes, (STRPTR) FrameStr);
        setmonitorstring(SUBWINDOW_MONITOR_CPU, monitor[MONGAD_FRAME].gadget, (STRPTR) FrameStr);
        calcrunningtime();
        setmonitorstring(SUBWINDOW_MONITOR_CPU, monitor[MONGAD_TIME ].gadget, (STRPTR) TimeStr);
    }

    if (machine == TYPERIGHT)
    {   if (stage == STAGE_CLASS6_PLAY || stage == STAGE_CLASS8_PLAY || stage == STAGE_CLASS68_WAITRESULTS)
        {   if (tr_textcursor == 0)
            {   tr_wpm      = 0;
                tr_accuracy = 100;
            } else
            {   int secs,
                    words;

                secs = tr_remaining / 50;
                if (tr_remaining % 50)
                {   secs++;
                }
                words = tr_textcursor / (5 + 1);
                if (tr_textcursor % (5 + 1))
                {   words++;
                }
                tr_wpm = 60 * words / secs;
                tr_accuracy = 100 - (100 * tr_errors / tr_textcursor);
            }
            sprintf((char*) TRString[0], "%d", tr_wpm);
            sprintf((char*) TRString[1], "%d", tr_accuracy);
        } else
        {   TRString[0][0] =
            TRString[1][0] = '-';
            TRString[0][1] =
            TRString[1][1] = EOS;
        }
        if (force || strcmp((const char*) TRString[0], (const char*) monitor_wpm))
        {   strcpy((char*) monitor_wpm, (const char*) TRString[0]);
            setmonitorstring(SUBWINDOW_MONITOR_CPU, monitor[MONGAD_WPM_TR   ].gadget, (STRPTR) TRString[0]);
        }
        if (force || strcmp((const char*) TRString[1], (const char*) monitor_accuracy))
        {   strcpy((char*) monitor_accuracy, (const char*) TRString[1]);
            setmonitorstring(SUBWINDOW_MONITOR_CPU, monitor[MONGAD_ACCURACY ].gadget, (STRPTR) TRString[1]);
        }

        sprintf((char*) TRString[2], "%d", tr_errors);
        if (tr_errors != monitor_errors || force)
        {   monitor_errors = tr_errors;
            setmonitorstring(SUBWINDOW_MONITOR_CPU, monitor[MONGAD_ERRORS   ].gadget, (STRPTR) TRString[2]);
        }

        switch (stage)
        {
        case STAGE_GAME_PLAY:
        case STAGE_CLASS1_PLAY:
            if (tr_gameletter == EOS)
            {   TRString[3][0] = '-';
                TRString[3][1] = EOS;
            } else
            {   TRString[3][0] =
                TRString[3][2] = QUOTE;
                TRString[3][1] = tr_gameletter;
                TRString[3][3] = EOS;
            }
        acase STAGE_CLASS6_PLAY:
            TRString[3][0] =
            TRString[3][2] = QUOTE;
            TRString[3][1] = classes[tr_class - 6][tr_level - 1][tr_textcursor];
            if (TRString[3][1] == LF)
            {   TRString[3][1] = (TEXT) '�';
            }
            TRString[3][3] = EOS;
        acase STAGE_CLASS8_PLAY:
        {   int ideallength;

            TRString[3][0] = QUOTE;
            ideallength = 0;
            do
            {   ideallength++;
            } while
            (   ideallength < 8
             && classes[tr_class - 6][tr_level - 1][tr_textcursor + ideallength - 1] != ' '
             && classes[tr_class - 6][tr_level - 1][tr_textcursor + ideallength - 1] != LF
             && classes[tr_class - 6][tr_level - 1][tr_textcursor + ideallength - 1] != EOS
            );
            zstrncpy((STRPTR) &TRString[3][1], &classes[tr_class - 6][tr_level - 1][tr_textcursor], ideallength);
            if (TRString[3][ideallength] == LF)
            {   TRString[3][ideallength] = (TEXT) '�';
            }
            strcat((char*) TRString[3], "\'");
        }
        adefault:
            TRString[3][0] = '-';
            TRString[3][1] = EOS;
        }
        if (force || strcmp((const char*) TRString[3], (const char*) monitor_expecting))
        {   strcpy((char*) monitor_expecting, (const char*) TRString[3]);
            setmonitorstring(SUBWINDOW_MONITOR_CPU, monitor[MONGAD_EXPECTING].gadget, (STRPTR) TRString[3]);
        }

        return;
    }

    for (i = 0; i < 7; i++)
    {   t = (whichcpu == 0) ? r[i] : other_r[i];
        if (t != monitor[MONGAD_R0 + i].contents || force)
        {   monitor[MONGAD_R0 + i].contents = t;
            hex1((STRPTR) monitor[MONGAD_R0 + i].string, t);
            monitor[MONGAD_R0 + i].string[2] = EOS;
            setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_R0   + i].string, MONGAD_R0 + i);
    }   }
    for (i = 0; i < 8; i++)
    {   tt = (whichcpu == 0) ? ras[i] : other_ras[i];
        if (tt != monitor[MONGAD_RAS0 + i].contents || force)
        {   monitor[MONGAD_RAS0 + i].contents = tt;
            hex2((STRPTR) monitor[MONGAD_RAS0 + i].string, tt);
            monitor[MONGAD_RAS0 + i].string[4] = EOS;
            setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_RAS0 + i].string, MONGAD_RAS0 + i);
    }   }
    t = ((whichcpu == 0) ? psu : other_psu) & PSU_S;
    if (t != monitor[MONGAD_S].contents || force)
    {   monitor[MONGAD_S].contents = t;
        monitor[MONGAD_S].string[0] = t ? pswbit[style][0] : pswbit[style][0] + 32;
        monitor[MONGAD_S].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_S       ].string, MONGAD_S);
    }
    t = ((whichcpu == 0) ? psu : other_psu) & PSU_F;
    if (t != monitor[MONGAD_F].contents || force)
    {   monitor[MONGAD_F].contents = t;
        monitor[MONGAD_F].string[0] = t ? pswbit[style][1] : pswbit[style][1] + 32;
        monitor[MONGAD_F].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_F       ].string, MONGAD_F);
    }
    t = ((whichcpu == 0) ? psu : other_psu) & PSU_II;
    if (t != monitor[MONGAD_II].contents || force)
    {   monitor[MONGAD_II].contents = t;
        monitor[MONGAD_II].string[0] = t ? pswbit[style][2] : pswbit[style][2] + 32;
        monitor[MONGAD_II].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_II      ].string, MONGAD_II);
    }
    t = ((whichcpu == 0) ? psu : other_psu) & PSU_UF1;
    if (t != monitor[MONGAD_UF1].contents || force)
    {   monitor[MONGAD_UF1].contents = t;
        monitor[MONGAD_UF1].string[0] = t ? '1' : '.';
        monitor[MONGAD_UF1].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_UF1     ].string, MONGAD_UF1);
    }
    t = ((whichcpu == 0) ? psu : other_psu) & PSU_UF2;
    if (t != monitor[MONGAD_UF2].contents || force)
    {   monitor[MONGAD_UF2].contents = t;
        monitor[MONGAD_UF2].string[0] = t ? '2' : '.';
        monitor[MONGAD_UF2].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_UF2     ].string, MONGAD_UF2);
    }
    t = ((whichcpu == 0) ? psu : other_psu) & PSU_SP;
    if (t != monitor[MONGAD_SP].contents || force)
    {   monitor[MONGAD_SP].contents = t;
        monitor[MONGAD_SP].string[0] = '0' + t;
        monitor[MONGAD_SP].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SP      ].string, MONGAD_SP);
        setrasgads(0);
    }

    t = ((whichcpu == 0) ? psl : other_psl) & PSL_CC;
    if (t != monitor[MONGAD_CC ].contents || force)
    {   monitor[MONGAD_CC ].contents = t;
        strcpy((char*) monitor[MONGAD_CC].string, (const char*) (ccstring[style][t >> 6])); // %11000000 -> %00000011
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_CC      ].string, MONGAD_CC);
    }
    t = ((whichcpu == 0) ? psl : other_psl) & PSL_IDC;
    if (t != monitor[MONGAD_IDC].contents || force)
    {   monitor[MONGAD_IDC].contents = t;
        monitor[MONGAD_IDC].string[0] = t ? pswbit[style][3] : pswbit[style][3] + 32;
        monitor[MONGAD_IDC].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_IDC     ].string, MONGAD_IDC);
    }
    t = ((whichcpu == 0) ? psl : other_psl) & PSL_RS;
    if (t != monitor[MONGAD_RS ].contents || force)
    {   monitor[MONGAD_RS ].contents = t;
        monitor[MONGAD_RS ].string[0] = t ? pswbit[style][4] : pswbit[style][4] + 32;
        monitor[MONGAD_RS ].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_RS      ].string, MONGAD_RS);
    }
    t = ((whichcpu == 0) ? psl : other_psl) & PSL_WC;
    if (t != monitor[MONGAD_WC ].contents || force)
    {   monitor[MONGAD_WC ].contents = t;
        monitor[MONGAD_WC ].string[0] = t ? pswbit[style][5] : pswbit[style][5] + 32;
        monitor[MONGAD_WC ].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_WC      ].string, MONGAD_WC);
    }
    t = ((whichcpu == 0) ? psl : other_psl) & PSL_OVF;
    if (t != monitor[MONGAD_OVF].contents || force)
    {   monitor[MONGAD_OVF].contents = t;
        monitor[MONGAD_OVF].string[0] = t ? pswbit[style][6] : pswbit[style][6] + 32;
        monitor[MONGAD_OVF].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_OVF     ].string, MONGAD_OVF);
    }
    t = ((whichcpu == 0) ? psl : other_psl) & PSL_COM;
    if (t != monitor[MONGAD_COM].contents || force)
    {   monitor[MONGAD_COM].contents = t;
        monitor[MONGAD_COM].string[0] = t ? pswbit[style][7] : pswbit[style][7] + 32;
        monitor[MONGAD_COM].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_COM     ].string, MONGAD_COM);
    }
    t = ((whichcpu == 0) ? psl : other_psl) & PSL_C;
    if (t != monitor[MONGAD_C  ].contents || force)
    {   monitor[MONGAD_C  ].contents = t;
        monitor[MONGAD_C  ].string[0] = t ? pswbit[style][8] : pswbit[style][8] + 32;
        monitor[MONGAD_C  ].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_C       ].string, MONGAD_C);
    }

    tt = (whichcpu == 0) ? iar : other_iar;
    if (tt != monitor[MONGAD_IAR].contents || force)
    {   monitor[MONGAD_IAR].contents = tt;
        hex2((STRPTR) monitor[MONGAD_IAR].string, tt);
        monitor[MONGAD_IAR].string[4] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_IAR     ].string, MONGAD_IAR);

        // maybe we should always refresh the "instruction" gadget (in case of eg. self-modifying code)
        if (whichcpu == 0) // because it won't work correctly otherwise
        {   tracecpu_2650(FALSE, TRUE);
        }
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, mn, MONGAD_OPCODE);
#ifdef AMIGA
    #ifdef MONOPCODECOLOURS
        SetGadgetAttrs
        (   monitor[MONGAD_OPCODE].gadget, subwin[SUBWINDOW_MONITOR_CPU].hwnd, NULL,
            BUTTON_BackgroundPen, (ULONG) emupens[guest_to_emupen[table_opcolours_2650[supercpu][memory[tt]]]],
        TAG_DONE); // button gadgets autorefresh themselves
    #endif
#endif
    }

    if (monitorcycles_2650 != cycles_2650 || force)
    {   monitorcycles_2650 = cycles_2650;
        longcomma(scale_time(monitorcycles_2650), (STRPTR) Clock2650Str);
        setmonitorstring(SUBWINDOW_MONITOR_CPU, monitor[MONGAD_CLOCK].gadget, (STRPTR) Clock2650Str);
    }

    if (machine != TWIN)
    {   if (monitorx != cpux || force)
        {   monitorx = cpux;
            DISCARD stcl_d((char*) XStr, (long) monitorx); // must be a signed operation!
            setmonitorstring(SUBWINDOW_MONITOR_CPU, monitor[MONGAD_X].gadget, (STRPTR) XStr);
        }
        if (monitory != cpuy || force)
        {   monitory = cpuy;
            DISCARD stcl_d((char*) YStr, (long) monitory); // must be a signed operation!
            setmonitorstring(SUBWINDOW_MONITOR_CPU, monitor[MONGAD_Y].gadget, (STRPTR) YStr);
        }

        return;
    } // implied else

    if (whichcpu != monitor_cpu || force)
    {
#ifdef WIN32
        if (whichcpu == 0)
        {   SendMessage(GetDlgItem(subwin[SUBWINDOW_MONITOR_CPU].hwnd, IDC_MASTERGLYPH), STM_SETIMAGE, IMAGE_ICON, (LPARAM) cpuicon[1]);
            SendMessage(GetDlgItem(subwin[SUBWINDOW_MONITOR_CPU].hwnd, IDC_SLAVEGLYPH ), STM_SETIMAGE, IMAGE_ICON, (LPARAM) cpuicon[0]);
        } else
        {   SendMessage(GetDlgItem(subwin[SUBWINDOW_MONITOR_CPU].hwnd, IDC_MASTERGLYPH), STM_SETIMAGE, IMAGE_ICON, (LPARAM) cpuicon[0]);
            SendMessage(GetDlgItem(subwin[SUBWINDOW_MONITOR_CPU].hwnd, IDC_SLAVEGLYPH ), STM_SETIMAGE, IMAGE_ICON, (LPARAM) cpuicon[1]);
        }
#endif
#ifdef AMIGA
        if (whichcpu == 0)
        {   images[IMAGE_GLYPH_TWIN_GLOW]->LeftEdge = master_x;
            images[IMAGE_GLYPH_TWIN_GLOW]->TopEdge  = master_y;
            images[IMAGE_GLYPH_TWIN_DIM ]->LeftEdge = slave_x;
            images[IMAGE_GLYPH_TWIN_DIM ]->TopEdge  = slave_y;
        } else
        {   images[IMAGE_GLYPH_TWIN_DIM ]->LeftEdge = master_x;
            images[IMAGE_GLYPH_TWIN_DIM ]->TopEdge  = master_y;
            images[IMAGE_GLYPH_TWIN_GLOW]->LeftEdge = slave_x;
            images[IMAGE_GLYPH_TWIN_GLOW]->TopEdge  = slave_y;
        }
        DrawImage(subwin[SUBWINDOW_MONITOR_CPU].hwnd->RPort, images[IMAGE_GLYPH_TWIN_DIM ], 0, 0);
        DrawImage(subwin[SUBWINDOW_MONITOR_CPU].hwnd->RPort, images[IMAGE_GLYPH_TWIN_GLOW], 0, 0);
#endif
        monitor_cpu = whichcpu;
    }

    for (i = 0; i < 7; i++)
    {   t = (whichcpu == 1) ? r[i] : other_r[i];
        if (t != monitor[MONGAD_SLAVE_R0 + i].contents || force)
        {   monitor[MONGAD_SLAVE_R0 + i].contents = t;
            hex1((STRPTR) monitor[MONGAD_SLAVE_R0 + i].string, t);
            monitor[MONGAD_SLAVE_R0 + i].string[2] = EOS;
            setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SLAVE_R0   + i].string, MONGAD_SLAVE_R0 + i);
    }   }
    for (i = 0; i < 8; i++)
    {   tt = (whichcpu == 1) ? ras[i] : other_ras[i];
        if (tt != monitor[MONGAD_SLAVE_RAS0 + i].contents || force)
        {   monitor[MONGAD_SLAVE_RAS0 + i].contents = tt;
            hex2((STRPTR) monitor[MONGAD_SLAVE_RAS0 + i].string, tt);
            monitor[MONGAD_SLAVE_RAS0 + i].string[4] = EOS;
            setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SLAVE_RAS0 + i].string, MONGAD_SLAVE_RAS0 + i);
    }   }
    t = ((whichcpu == 1) ? psu : other_psu) & PSU_S;
    if (t != monitor[MONGAD_SLAVE_S].contents || force)
    {   monitor[MONGAD_SLAVE_S].contents = t;
        monitor[MONGAD_SLAVE_S].string[0] = t ? pswbit[style][0] : pswbit[style][0] + 32;
        monitor[MONGAD_SLAVE_S].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SLAVE_S       ].string, MONGAD_SLAVE_S);
    }
    t = ((whichcpu == 1) ? psu : other_psu) & PSU_F;
    if (t != monitor[MONGAD_SLAVE_F].contents || force)
    {   monitor[MONGAD_SLAVE_F].contents = t;
        monitor[MONGAD_SLAVE_F].string[0] = t ? pswbit[style][1] : pswbit[style][1] + 32;
        monitor[MONGAD_SLAVE_F].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SLAVE_F       ].string, MONGAD_SLAVE_F);
    }
    t = ((whichcpu == 1) ? psu : other_psu) & PSU_II;
    if (t != monitor[MONGAD_SLAVE_II].contents || force)
    {   monitor[MONGAD_SLAVE_II].contents = t;
        monitor[MONGAD_SLAVE_II].string[0] = t ? pswbit[style][2] : pswbit[style][2] + 32;
        monitor[MONGAD_SLAVE_II].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SLAVE_II      ].string, MONGAD_SLAVE_II);
    }
    t = ((whichcpu == 1) ? psu : other_psu) & PSU_UF1;
    if (t != monitor[MONGAD_SLAVE_UF1].contents || force)
    {   monitor[MONGAD_SLAVE_UF1].contents = t;
        monitor[MONGAD_SLAVE_UF1].string[0] = t ? '1' : '.';
        monitor[MONGAD_SLAVE_UF1].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SLAVE_UF1     ].string, MONGAD_SLAVE_UF1);
    }
    t = ((whichcpu == 1) ? psu : other_psu) & PSU_UF2;
    if (t != monitor[MONGAD_SLAVE_UF2].contents || force)
    {   monitor[MONGAD_SLAVE_UF2].contents = t;
        monitor[MONGAD_SLAVE_UF2].string[0] = t ? '2' : '.';
        monitor[MONGAD_SLAVE_UF2].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SLAVE_UF2     ].string, MONGAD_SLAVE_UF2);
    }
    t = ((whichcpu == 1) ? psu : other_psu) & PSU_SP;
    if (t != monitor[MONGAD_SLAVE_SP].contents || force)
    {   monitor[MONGAD_SLAVE_SP].contents = t;
        monitor[MONGAD_SLAVE_SP].string[0] = '0' + t;
        monitor[MONGAD_SLAVE_SP].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SLAVE_SP      ].string, MONGAD_SLAVE_SP);
        setrasgads(1);
    }

    t = ((whichcpu == 1) ? psl : other_psl) & PSL_CC;
    if (t != monitor[MONGAD_SLAVE_CC ].contents || force)
    {   monitor[MONGAD_SLAVE_CC ].contents = t;
        strcpy((char*) monitor[MONGAD_SLAVE_CC].string, (STRPTR) (ccstring[style][t >> 6])); // %11000000 -> %00000011
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SLAVE_CC      ].string, MONGAD_SLAVE_CC);
    }
    t = ((whichcpu == 1) ? psl : other_psl) & PSL_IDC;
    if (t != monitor[MONGAD_SLAVE_IDC].contents || force)
    {   monitor[MONGAD_SLAVE_IDC].contents = t;
        monitor[MONGAD_SLAVE_IDC].string[0] = t ? pswbit[style][3] : pswbit[style][3] + 32;
        monitor[MONGAD_SLAVE_IDC].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SLAVE_IDC     ].string, MONGAD_SLAVE_IDC);
    }
    t = ((whichcpu == 1) ? psl : other_psl) & PSL_RS;
    if (t != monitor[MONGAD_SLAVE_RS ].contents || force)
    {   monitor[MONGAD_SLAVE_RS ].contents = t;
        monitor[MONGAD_SLAVE_RS ].string[0] = t ? pswbit[style][4] : pswbit[style][4] + 32;
        monitor[MONGAD_SLAVE_RS ].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SLAVE_RS      ].string, MONGAD_SLAVE_RS);
    }
    t = ((whichcpu == 1) ? psl : other_psl) & PSL_WC;
    if (t != monitor[MONGAD_SLAVE_WC ].contents || force)
    {   monitor[MONGAD_SLAVE_WC ].contents = t;
        monitor[MONGAD_SLAVE_WC ].string[0] = t ? pswbit[style][5] : pswbit[style][5] + 32;
        monitor[MONGAD_SLAVE_WC ].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SLAVE_WC      ].string, MONGAD_SLAVE_WC);
    }
    t = ((whichcpu == 1) ? psl : other_psl) & PSL_OVF;
    if (t != monitor[MONGAD_SLAVE_OVF].contents || force)
    {   monitor[MONGAD_SLAVE_OVF].contents = t;
        monitor[MONGAD_SLAVE_OVF].string[0] = t ? pswbit[style][6] : pswbit[style][6] + 32;
        monitor[MONGAD_SLAVE_OVF].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SLAVE_OVF     ].string, MONGAD_SLAVE_OVF);
    }
    t = ((whichcpu == 1) ? psl : other_psl) & PSL_COM;
    if (t != monitor[MONGAD_SLAVE_COM].contents || force)
    {   monitor[MONGAD_SLAVE_COM].contents = t;
        monitor[MONGAD_SLAVE_COM].string[0] = t ? pswbit[style][7] : pswbit[style][7] + 32;
        monitor[MONGAD_SLAVE_COM].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SLAVE_COM     ].string, MONGAD_SLAVE_COM);
    }
    t = ((whichcpu == 1) ? psl : other_psl) & PSL_C;
    if (t != monitor[MONGAD_SLAVE_C  ].contents || force)
    {   monitor[MONGAD_SLAVE_C  ].contents = t;
        monitor[MONGAD_SLAVE_C  ].string[0] = t ? pswbit[style][8] : pswbit[style][8] + 32;
        monitor[MONGAD_SLAVE_C  ].string[1] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SLAVE_C       ].string, MONGAD_SLAVE_C);
    }

    tt = (whichcpu == 1) ? iar : other_iar;
    if (tt != monitor[MONGAD_SLAVE_IAR].contents || force)
    {   monitor[MONGAD_SLAVE_IAR].contents = tt;
        hex2((STRPTR) monitor[MONGAD_SLAVE_IAR].string, tt);
        monitor[MONGAD_SLAVE_IAR].string[4] = EOS;
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, (STRPTR) monitor[MONGAD_SLAVE_IAR     ].string, MONGAD_SLAVE_IAR);

        // maybe we should always refresh the "instruction" gadget (in case of eg. self-modifying code)
        if (whichcpu == 1) // because it won't work correctly otherwise
        {   tracecpu_2650(FALSE, TRUE);
        }
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, mn, MONGAD_SLAVE_OPCODE);
#ifdef AMIGA
    #ifdef MONOPCODECOLOURS
        SetGadgetAttrs
        (   monitor[MONGAD_SLAVE_OPCODE].gadget, subwin[SUBWINDOW_MONITOR_CPU].hwnd, NULL,
            BUTTON_BackgroundPen, (ULONG) emupens[guest_to_emupen[table_opcolours_2650[supercpu][memory[tt]]]],
        TAG_DONE); // button gadgets autorefresh themselves
    #endif
#endif
}   }

EXPORT void update_monitor_psgs(FLAG force)
{   FAST int   i;
    FAST UWORD newval;

    if (!subwin[SUBWINDOW_MONITOR_PSGS].hwnd)
    {   return;
    }

    // assert(memmap == MEMMAP_F);

    for (i = PSGS_FIRSTMONGAD; i <= PSGS_LASTMONGAD; i++)
    {   if (i >= FIRSTDOUBLEBYTEPSG && i <= LASTDOUBLEBYTEPSG)
        {   newval =  memory[ monitor[i].addr     ]
                   + (memory[(monitor[i].addr + 1)] * 256); // little-endian
            if (newval != monitor[i].contents || force)
            {   monitor[i].contents = newval;
                hex2((STRPTR) monitor[i].string, monitor[i].contents);
                monitor[i].string[4] = EOS;
                setmonitorbutton(SUBWINDOW_MONITOR_PSGS, (STRPTR) monitor[i].string, i);
#ifdef WIN32
                getextrapsginfo(i);
#endif
        }   }
        else
        {   if (memory[monitor[i].addr] != monitor[i].contents || force)
            {   monitor[i].contents = memory[monitor[i].addr];
                hex1((STRPTR) monitor[i].string, (UBYTE) monitor[i].contents);
                monitor[i].string[2] = EOS;
                setmonitorbutton(SUBWINDOW_MONITOR_PSGS, (STRPTR) monitor[i].string, i);
#ifdef WIN32
                getextrapsginfo(i);
#endif
}   }   }   }

EXPORT void update_monitor_xvi(FLAG force)
{   FAST int      i;
#ifdef WIN32
    FAST FLAG     flagging,
                  ok;
    FAST TOOLINFO ti;
#endif

    if (!subwin[SUBWINDOW_MONITOR_XVI].hwnd)
    {   return;
    }

    // assert(machine == ARCADIA || machines[machine].pvis || machine == PONG);
    // ie. not PIPBUG, BINBUG, INSTRUCTOR, CD2650, PHUNSY, SELBST, MIKIT

    if (machines[machine].pvis >= 1)
    {   for (i = PVI1ST_FIRSTMONGAD; i <= PVI1ST_LASTMONGAD; i++)
        {   if (memory[pvibase + monitor[i].addr] != monitor[i].contents || force)
            {   monitor[i].contents = memory[pvibase + monitor[i].addr];
                hex1((STRPTR) monitor[i].string, (UBYTE) monitor[i].contents);
                setmonitorbutton(SUBWINDOW_MONITOR_XVI, (STRPTR) monitor[i].string, i);
#ifdef WIN32
                getextrapviinfo(i);
#endif
        }   }
        if (machines[machine].pvis >= 2)
        {   for (i = PVI2ND_FIRSTMONGAD; i <= PVI2ND_LASTMONGAD; i++) // 2nd PVI
            {   if (memory[pvibase + monitor[i].addr] != monitor[i].contents || force)
                {   monitor[i].contents = memory[pvibase + monitor[i].addr];
                    hex1((STRPTR) monitor[i].string, (UBYTE) monitor[i].contents);
                    setmonitorbutton(SUBWINDOW_MONITOR_XVI, (STRPTR) monitor[i].string, i);
#ifdef WIN32
                    getextrapviinfo(i);
#endif
            }   }
            if (machines[machine].pvis >= 3)
            {   for (i = PVI3RD_FIRSTMONGAD; i <= PVI3RD_LASTMONGAD; i++) // 3rd PVI
                {   if (memory[pvibase + monitor[i].addr] != monitor[i].contents || force)
                    {   monitor[i].contents = memory[pvibase + monitor[i].addr];
                        hex1((STRPTR) monitor[i].string, (UBYTE) monitor[i].contents);
                        setmonitorbutton(SUBWINDOW_MONITOR_XVI, (STRPTR) monitor[i].string, i);
#ifdef WIN32
                        getextrapviinfo(i);
#endif
    }   }   }   }   }

    switch (machine)
    {
    case ARCADIA:
        for (i = ARCADIA_FIRSTMONGAD; i <= ARCADIA_LASTMONGAD; i++)
        {   if
            (   memory[monitor[i].addr] != monitor[i].contents
             || force
            )
            {   monitor[i].contents = memory[monitor[i].addr];
                hex1((STRPTR) monitor[i].string, (UBYTE) monitor[i].contents);
                setmonitorbutton(SUBWINDOW_MONITOR_XVI, (STRPTR) monitor[i].string, i);
#ifdef WIN32
                getextrauviinfo(i);
#endif
        }   }
    acase INTERTON:
        for (i = INTERTON_FIRSTMONGAD; i <= INTERTON_LASTMONGAD; i++)
        {   if (memory[monitor[i].addr] != monitor[i].contents || force)
            {   monitor[i].contents = memory[monitor[i].addr];
                hex1((STRPTR) monitor[i].string, (UBYTE) monitor[i].contents);
                setmonitorbutton(SUBWINDOW_MONITOR_XVI, (STRPTR) monitor[i].string, i);
#ifdef WIN32
                getextraintertonelektorinfo(i);
#endif
        }   }
    acase ELEKTOR:
        for (i = ELEKTOR_FIRSTMONGAD; i <= ELEKTOR_LASTMONGAD; i++)
        {   if (memory[monitor[i].addr] != monitor[i].contents || force)
            {   monitor[i].contents = memory[monitor[i].addr];
                hex1((STRPTR) monitor[i].string, (UBYTE) monitor[i].contents);
                setmonitorbutton(SUBWINDOW_MONITOR_XVI, (STRPTR) monitor[i].string, i);
#ifdef WIN32
                getextraintertonelektorinfo(i);
#endif
    }   }   }

    do_axes();
}

#ifdef WIN32
MODULE void getextrauviinfo(int gid)
{   FAST TOOLINFO ti;

    // assert(machine == ARCADIA);

    if (interpret_uvi(monitor[gid].addr))
    {   make_monitortip(monitor[gid].addr, datatip);
        strcat(datatip, "\n");
        strcat(datatip, interpretstr);

        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
        ti.hwnd     = subwin[SUBWINDOW_MONITOR_XVI].hwnd;
        ti.uId      = gid - ARCADIA_FIRSTMONGAD;
        ti.hinst    = InstancePtr;
        ti.lpszText = datatip; // this gets copied
        SendMessage(subwin[SUBWINDOW_MONITOR_XVI].tips, TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
}   }

MODULE void getextrapviinfo(int gid)
{   FAST TOOLINFO ti;

    // assert(machines[machine].pvis);

    if (interpret_pvis(pvibase + monitor[gid].addr))
    {   make_monitortip(pvibase + monitor[gid].addr, datatip);
        strcat(datatip, "\n");
        strcat(datatip, interpretstr);

        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
        ti.hwnd     = subwin[SUBWINDOW_MONITOR_XVI].hwnd;
        switch (machine)
        {
        case  INTERTON: ti.uId = INTERTON_MONGADS - PVI1ST_FIRSTMONGAD + gid;
        acase ELEKTOR:  ti.uId = ELEKTOR_MONGADS - PVI1ST_FIRSTMONGAD + gid;
        acase MALZAK:
        case  ZACCARIA: if (gid >= PVI2ND_FIRSTMONGAD)
                        {   ti.uId = PVI1ST_MONGADS - PVI2ND_FIRSTMONGAD + gid;
                        } else
                        {   ti.uId = gid - PVI1ST_FIRSTMONGAD;
        }               }
        ti.hinst    = InstancePtr;
        ti.lpszText = datatip; // this gets copied
        SendMessage(subwin[SUBWINDOW_MONITOR_XVI].tips, TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
}   }

MODULE void getextraintertonelektorinfo(int gid)
{   FAST TOOLINFO ti;

    // assert(machine == INTERTON || machine == ELEKTOR);

    if (interpret_intertonelektor(monitor[gid].addr))
    {   make_monitortip(monitor[gid].addr, datatip);
        strcat(datatip, "\n");
        strcat(datatip, interpretstr);

        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
        ti.hwnd     = subwin[SUBWINDOW_MONITOR_XVI].hwnd;
        if (machine == INTERTON)
        {   ti.uId  = gid - INTERTON_FIRSTMONGAD;
        } else
        {   // assert(machine == ELEKTOR);
            ti.uId  = gid - ELEKTOR_FIRSTMONGAD;
        }
        ti.hinst    = InstancePtr;
        ti.lpszText = datatip; // this gets copied
        SendMessage(subwin[SUBWINDOW_MONITOR_XVI].tips, TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
}   }

MODULE void getextrapsginfo(int gid)
{   FAST TOOLINFO ti;

    // assert(memmap == MEMMAP_F);

    if (interpret_psgs(monitor[gid].addr))
    {   make_monitortip(monitor[gid].addr, datatip);
        strcat(datatip, "\n");
        strcat(datatip, interpretstr);

        ti.cbSize   = sizeof(TOOLINFO);
        ti.uFlags   = TTF_SUBCLASS | TTF_CENTERTIP;
        ti.hwnd     = subwin[SUBWINDOW_MONITOR_PSGS].hwnd;
        ti.uId      = gid - PSGS_FIRSTMONGAD;
        ti.hinst    = InstancePtr;
        ti.lpszText = datatip; // this gets copied
        SendMessage(subwin[SUBWINDOW_MONITOR_PSGS].tips, TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
}   }
#endif

MODULE void calcrunningtime(void)
{   FAST int    hours,
                minutes,
                seconds,
                micros;
    FAST double fsecs,
                fmicros;

    if (exactspeed)
    {   fsecs = (double) frames / ((region == REGION_NTSC) ? 60.0 : 50.0); // how many seconds
    } else
    {   fsecs = (double) frames / machines[machine].fps[region]; // how many seconds
    }
    seconds   = (int)    fsecs;
    fmicros   =          fsecs - (double) seconds;
    micros    = (int)    (fmicros * 1000000.0);
    hours     = (seconds / 3600) % 100; // resets to 00 hours after 100 hours
    minutes   = (seconds % 3600) / 60;
    seconds   %= 60;

    TimeStr[0]  = (TEXT) ('0' + (hours   / 10)            );
    TimeStr[1]  = (TEXT) ('0' + (hours   % 10)            );
    TimeStr[3]  = (TEXT) ('0' + (minutes / 10)            );
    TimeStr[4]  = (TEXT) ('0' + (minutes % 10)            );
    TimeStr[6]  = (TEXT) ('0' + (seconds / 10)            );
    TimeStr[7]  = (TEXT) ('0' + (seconds % 10)            );
    TimeStr[8]  = decpoint;
    TimeStr[9]  = (TEXT) ('0' + (micros  / 100000)        );
    TimeStr[10] = (TEXT) ('0' + (micros  % 100000) / 10000);
}

EXPORT void zstrncpy(char* to, const char* from, size_t n)
{   DISCARD strncpy(to, from, n);
    to[n] = EOS;
}

EXPORT int spritebutton(int x, int y, int operation)
{   int rc = 2;

    // operation is 0 for clear
    // operation is 1 for set
    // operation is 2 for toggle

    if (machines[machine].pvis)
    {   if (x >= 0 && x <= 7 && y >= 0 && y <= 9)
        {   if (operation == 0 || (operation == 2 && (spritecontents[y] & (128 >> x))))
            {   memory[pvibase + (0x100 * (viewingsprite / 4)) + pvi_spritedata[viewingsprite % 4] + y] &= ~(128 >> x); // turn off
                rc = 0;
            } else
            {   memory[pvibase + (0x100 * (viewingsprite / 4)) + pvi_spritedata[viewingsprite % 4] + y] |=   128 >> x ; // turn on
                rc = 1;
            }
            update_spriteeditor(FALSE);
    }   }
    elif (machine == ARCADIA)
    {   if
        (   viewingsprite >= 56
         && viewingsprite <= 63
         && x >= 0
         && x <= 7
         && y >= 0
         && y <= 7
        )
        {   if (operation == 0 || (operation == 2 && (spritecontents[y] & (128 >> x))))
            {   memory[0x1980 + ((viewingsprite - 56) * 8) + y] &= ~(128 >> x); // turn off
                rc = 0;
            } else
            {   memory[0x1980 + ((viewingsprite - 56) * 8) + y] |=   128 >> x ; // turn on
                rc = 1;
            }
            update_spriteeditor(FALSE);
    }   }
    elif (machine == BINBUG)
    {   if (viewingsprite >= 128 && x >= 0 && x <= 7 && y >= 0 && y <= 15)
        {   if (operation == 0 || (operation == 2 && (spritecontents[y] & (128 >> x))))
            {   memory[0x7000 + ((viewingsprite - 128) * 16) + y] &= ~(128 >> x); // turn off
                rc = 0;
            } else
            {   memory[0x7000 + ((viewingsprite - 128) * 16) + y] |=   128 >> x ; // turn on
                rc = 1;
            }
            update_spriteeditor(FALSE);
    }   }

    return rc;
}

EXPORT void update_spriteeditor(FLAG force)
{   TRANSIENT FLAG  colourable = TRUE;
    FAST      int   oldsprviewcolour,
                    x, y;
    FAST      UBYTE t;
    FAST      TEXT  thetext[6 + 1];
#ifdef WIN32
    FAST      int   i, j;
    FAST      ULONG reversed;
#endif

    if (!subwin[SUBWINDOW_SPRITES].hwnd)
    {   return;
    }

    oldsprviewcolour = sprviewcolour;
    switch (machine)
    {
    case ARCADIA:
        switch (viewingsprite)
        {
        case  56:                      sprviewcolour = (memory[A_SPRITES01CTRL] & 0x38) >> 3;
        acase 57:                      sprviewcolour =  memory[A_SPRITES01CTRL] & 0x07;
        acase 58:                      sprviewcolour = (memory[A_SPRITES23CTRL] & 0x38) >> 3;
        acase 59:                      sprviewcolour =  memory[A_SPRITES23CTRL] & 0x07;
        adefault: /* 0..55, 60..127 */ sprviewcolour = WHITE; // we should look at the current foreground text colour and use that instead
                                       colourable = FALSE;
        }
        if (flagline && (psu & PSU_F))
        {   sprviewcolour = from_a[1][sprviewcolour];
        } else
        {   sprviewcolour = from_a[0][sprviewcolour];
        }
    acase INTERTON:
    case ELEKTOR:
    case ZACCARIA:
    case MALZAK:
        sprviewcolour = calcspritecolour(viewingsprite);
    acase BINBUG:
    case CD2650:
        sprviewcolour = WHITE; // perhaps we should show it inverted when in inverted mode
        colourable = FALSE;
    acase PHUNSY:
        if (viewingsprite < 128)
        {   sprviewcolour = WHITE; // perhaps we should show it inverted when in inverted mode
        } else
        {   switch (viewingsprite & 0x70)
            {
            case  0x00: sprviewcolour = BLACK;
            acase 0x10: sprviewcolour = GREY2;
            acase 0x20: sprviewcolour = GREY3;
            acase 0x30: sprviewcolour = GREY4;
            acase 0x40: sprviewcolour = GREY5;
            acase 0x50: sprviewcolour = GREY6;
            acase 0x60: sprviewcolour = GREY7;
            acase 0x70: sprviewcolour = WHITE;
        }   }
        colourable = FALSE;
    }
    if (sprviewcolour != oldsprviewcolour)
    {   force = TRUE;
    }

    if (force)
    {   makesprstring(viewingsprite);
        st_set2(SUBWINDOW_SPRITES, IDC_WHICHSPRITEDESC, spritename);
#ifdef AMIGA
        if (machine == ARCADIA || machines[machine].pvis)
        {   SetGadgetAttrs(gadgets[GID_SR_PL1], subwin[SUBWINDOW_SPRITES].hwnd, NULL, PALETTE_Colour, sprviewcolour, GA_Disabled, colourable ? FALSE : TRUE, TAG_DONE); // this refreshes automatically
        }
#endif
    }

#ifdef WIN32
    for (i = 0; i < 9; i++)
    {   // 0RGB -> 0BGR
        DeleteObject(hSpriteBrush[i]);
        if (machine == PHUNSY)
        {   switch (i)
            {
            case  0: j =  7; // black
            acase 1: j = 16; // grey #2
            acase 2: j = 17; // grey #3
            acase 3: j = 18; // grey #4
            acase 4: j = 19; // grey #5
            acase 5: j = 20; // grey #6
            acase 6: j = 21; // grey #7
            acase 7: j =  0; // white
            acase 8: j =  6; // blue
        }   }
        else
        {   j = i;
        }
        reversed = ((pencolours[colourset][j] & 0x00FF0000) >> 16)  // red
                 |  (pencolours[colourset][j] & 0x0000FF00)         // green
                 | ((pencolours[colourset][j] & 0x000000FF) << 16); // blue
        hSpriteBrush[i] = CreateSolidBrush(reversed);
    }
    if (machine == ARCADIA || machines[machine].pvis)
    {   for (i = 0; i < 8; i++)
        {   DISCARD RedrawWindow(GetDlgItem(subwin[SUBWINDOW_SPRITES].hwnd, IDC_COLOUR0 + i), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }   }

    // clear_preview(subwin[SUBWINDOW_SPRITES].hwnd);
    do_preview(force);
#endif

    switch (machine)
    {
    case ARCADIA:
        if (viewingsprite <= 55)
        {   for (y = 0; y < 8; y++)
            {   if (force)
                {   strcpy((char*) thetext, "-");
                    dospraddress(y, (STRPTR) thetext);
                }
                t = arcadia_pdg[0][viewingsprite     ][y];
                if (t != spritecontents[y] || force)
                {   for (x = 0; x < 8; x++)
                    {   if ((t & (128 >> x)) != (spritecontents[y] & (128 >> x)) || force)
                        {   setsprview(x, y, (t & (128 >> x)) ? sprviewcolour : GREY1);
                    }   }
                    hexify(y, t);
                    spritecontents[y] = t;
        }   }   }
        elif (viewingsprite >= 64)
        {   for (y = 0; y < 8; y++)
            {   if (force)
                {   strcpy((char*) thetext, "-");
                    dospraddress(y, (STRPTR) thetext);
                }
                t = arcadia_pdg[1][viewingsprite - 64][y];
                if (t != spritecontents[y] || force)
                {   for (x = 0; x < 8; x++)
                    {   if ((t & (128 >> x)) != (spritecontents[y] & (128 >> x)) || force)
                        {   setsprview(x, y, (t & (128 >> x)) ? sprviewcolour : GREY1);
                    }   }
                    hexify(y, t);
                    spritecontents[y] = t;
        }   }   }
        else
        {   // assert(viewingsprite >= 56 && viewingsprite <= 63);
            for (y = 0; y < 8; y++)
            {   if (force)
                {   sprintf((char*) thetext, "$%X", A_OFFSET_SPRITES + ((viewingsprite - 56) * 8) + y);
                    dospraddress(y, (STRPTR) thetext);
                }
                t = memory[0x1980 + ((viewingsprite - 56) * 8) + y];
                if (t != spritecontents[y] || force)
                {   for (x = 0; x < 8; x++)
                    {   if ((t & (128 >> x)) != (spritecontents[y] & (128 >> x)) || force)
                        {   setsprview(x, y, (t & (128 >> x)) ? sprviewcolour : GREY1);
                    }   }
                    hexify(y, t);
                    spritecontents[y] = t;
        }   }   }
    acase INTERTON:
    case ELEKTOR:
    case ZACCARIA:
    case MALZAK:
        for (y = 0; y < 10; y++)
        {   if (force)
            {   sprintf((char*) thetext, "$%X", pvibase + (0x100 * (viewingsprite / 4)) + pvi_spritedata[viewingsprite % 4] + y);
                dospraddress(y, (STRPTR) thetext);
            }

            t = memory[pvibase + (0x100 * (viewingsprite / 4)) + pvi_spritedata[viewingsprite % 4] + y];
            if (t != spritecontents[y] || force)
            {   for (x = 0; x < 8; x++)
                {   if ((t & (128 >> x)) != (spritecontents[y] & (128 >> x)) || force)
                    {   setsprview(x, y, (t & (128 >> x)) ? sprviewcolour : GREY1);
                }   }
                hexify(y, t);
                spritecontents[y] = t;
        }   }
    acase BINBUG:
        for (y = 0; y < 16; y++)
        {   if (force)
            {   if (viewingsprite < 128)
                {   dospraddress(y, "-");
                } else
                {   sprintf((char*) thetext, "$%X", 0x7000 + (16 * viewingsprite) + y);
                    dospraddress(y, (STRPTR) thetext);
            }   }

            if (viewingsprite < 128)
            {   t = dg640_chars[viewingsprite][y];
            } else
            {   t = memory[0x7000 + (16 * (viewingsprite - 128)) + y];
            }
            if (t != spritecontents[y] || force)
            {   for (x = 0; x < 8; x++)
                {   if ((t & (128 >> x)) != (spritecontents[y] & (128 >> x)) || force)
                    {   setsprview(x, y, (t & (128 >> x)) ? sprviewcolour : GREY1);
                }   }
                hexify(y, t);
                spritecontents[y] = t;
        }   }
    acase CD2650:
        for (y = 0; y < 8; y++)
        {   if (force)
            {   dospraddress(y, "-");
            }

            t = cd2650_chars_bmp[viewingsprite][y];
            if (t != spritecontents[y] || force)
            {   for (x = 0; x < 8; x++)
                {   if ((t & (128 >> x)) != (spritecontents[y] & (128 >> x)) || force)
                    {   setsprview(x, y, (t & (128 >> x)) ? sprviewcolour : GREY1);
                }   }
                hexify(y, t);
                spritecontents[y] = t;
        }   }
    acase PHUNSY:
        for (y = 0; y < 8; y++)
        {   if (force)
            {   dospraddress(y, "-");
            }

            if (viewingsprite < 128)
            {   t = phunsy_chars[viewingsprite][           y];
            } else
            {   t = phunsy_gfx[( viewingsprite - 128) / 8][y];
            }
            if (t != spritecontents[y] || force)
            {   for (x = 0; x < 6; x++)
                {   if ((t & (0x20 >> x)) != (spritecontents[y] & (0x20 >> x)) || force)
                    {   setsprview(x, y, (t & (0x20 >> x)) ? sprviewcolour : BLUE);
                }   }
                hexify(y, t);
                spritecontents[y] = t;
}   }   }   }

EXPORT void dosprcolour(int newcolour)
{   // assert(machine != BINBUG);

    if (machines[machine].pvis)
    {   sprviewcolour = newcolour;
        switch (machine)
        {
        case ZACCARIA:
            switch (memmap)
            {
            case MEMMAP_ASTROWARS:
                sprviewcolour = from_astrowars_spr[sprviewcolour];
            acase MEMMAP_GALAXIA:
            case MEMMAP_LASERBATTLE:
            case MEMMAP_LAZARIAN:
                sprviewcolour = to_galaxia_spr[sprviewcolour];
            }
        acase MALZAK:
            sprviewcolour = from_malzak_spr[sprviewcolour];
        }

        switch (viewingsprite % 4)
        {
        case  0: memory[pvibase + (0x100 * (viewingsprite / 4)) + PVI_SPR01COLOURS] &= 0xC7;
                 memory[pvibase + (0x100 * (viewingsprite / 4)) + PVI_SPR01COLOURS] |= (sprviewcolour << 3);
        acase 1: memory[pvibase + (0x100 * (viewingsprite / 4)) + PVI_SPR01COLOURS] &= 0xF8;
                 memory[pvibase + (0x100 * (viewingsprite / 4)) + PVI_SPR01COLOURS] |=  sprviewcolour      ;
        acase 2: memory[pvibase + (0x100 * (viewingsprite / 4)) + PVI_SPR23COLOURS] &= 0xC7;
                 memory[pvibase + (0x100 * (viewingsprite / 4)) + PVI_SPR23COLOURS] |= (sprviewcolour << 3);
        acase 3: memory[pvibase + (0x100 * (viewingsprite / 4)) + PVI_SPR23COLOURS] &= 0xF8;
                 memory[pvibase + (0x100 * (viewingsprite / 4)) + PVI_SPR23COLOURS] |=  sprviewcolour      ;
        }
        update_spriteeditor(TRUE);
    } else
    {   // assert(machine == ARCADIA);

        if (flagline && (psu & PSU_F))
        {   sprviewcolour = from_a[1][newcolour];
        } else
        {   sprviewcolour = from_a[0][newcolour];
        }
        switch (viewingsprite)
        {
        case  56: memory[A_SPRITES01CTRL] &= 0xC7;
                  memory[A_SPRITES01CTRL] |= (sprviewcolour << 3);
                  update_spriteeditor(TRUE);
        acase 57: memory[A_SPRITES01CTRL] &= 0xF8;
                  memory[A_SPRITES01CTRL] |= sprviewcolour;
                  update_spriteeditor(TRUE);
        acase 58: memory[A_SPRITES23CTRL] &= 0xC7;
                  memory[A_SPRITES23CTRL] |= (sprviewcolour << 3);
                  update_spriteeditor(TRUE);
        acase 59: memory[A_SPRITES23CTRL] &= 0xF8;
                  memory[A_SPRITES23CTRL] |= sprviewcolour;
                  update_spriteeditor(TRUE);
}   }   }

EXPORT void makesprstring(int which)
{   switch (machine)
    {
    case ARCADIA:
        if (which <= 55)
        {   sprintf
            (   (char*) spritename,
                LLL(
                    MSG_CHARPDCNUMBER,
                    "Character PDG #%d ('%c')"
                ),
                which,
                arcadia_chars[which]
            );
        } elif (which >= 56 && which <= 59)
        {   sprintf
            (   (char*) spritename,
                LLL(
                    MSG_SPRITENUMBER,
                    "Sprite #%d ('%c')"
                ),
                which - 56,
                arcadia_chars[which]
            );
        } elif (which >= 60 && which <= 63)
        {   sprintf
            (   (char*) spritename,
                LLL(
                    MSG_UDCNUMBER,
                    "UDG #%d ('%c')"
                ),
                which - 60,
                arcadia_chars[which]
            );
        } else
        {   // assert(which >= 64 && which <= 127);
            sprintf
            (   (char*) spritename,
                LLL(
                    MSG_BLOCKPDCNUMBER,
                    "Block PDG #%d ('%c')"
                ),
                which - 64,
                arcadia_chars[which - 64]
            );
        }
    acase BINBUG:
        if (which < 128)
        {   sprintf((char*) spritename, "PDG #%d", which);
        } else
        {   sprintf((char*) spritename, "UDG #%d", which - 128);
        }
    acase PHUNSY:
        if (which < 128)
        {   sprintf((char*) spritename, "(Character) PDG #%d", which);
        } else
        {   sprintf((char*) spritename, "(Block) PDG #%d (", which - 128);
            switch (which & 0x70)
            {
            case  0x00: strcat((char*) spritename, colournames[ 7]); // black
            acase 0x10: strcat((char*) spritename, colournames[16]); // grey #2
            acase 0x20: strcat((char*) spritename, colournames[17]); // grey #3
            acase 0x30: strcat((char*) spritename, colournames[18]); // grey #4
            acase 0x40: strcat((char*) spritename, colournames[19]); // grey #5
            acase 0x50: strcat((char*) spritename, colournames[20]); // grey #6
            acase 0x60: strcat((char*) spritename, colournames[21]); // grey #7
            acase 0x70: strcat((char*) spritename, colournames[ 0]); // white
            }
            strcat((char*) spritename, ")");
        }
    adefault: // coin-ops, CD2650
        sprintf
        (   (char*) spritename,
            LLL(
                MSG_NUMBER,
                "#%d"
            ),
            which
        );
}   }

EXPORT void update_music(void)
{   FAST int   i,
               value;
    FAST float hertz;
    FAST FLAG  firstchan;
    FAST UBYTE newpitch,
               newvol;
    FAST ULONG temp;

    if (!subwin[SUBWINDOW_MUSIC].hwnd)
    {   return;
    }

    for (i = 0; i < 7; i++)
    {   newpsgnote[i] = NOTE_REST;
    }

#ifdef WIN32
    MusicRastPtr = GetDC(GetDlgItem(subwin[SUBWINDOW_MUSIC].hwnd, IDC_MUSIC));
    StaveRastPtr = GetDC(GetDlgItem(subwin[SUBWINDOW_MUSIC].hwnd, IDC_STAVE));
#endif

    if (reghost)
    {   ghost_notes();
        reghost = FALSE;
    }

    if (memmap == MEMMAP_F)
    {   if
        (   ((memory[PSG_AMPLITUDEA1] & 0x1F) == 0)
         || ((memory[PSG_MIXER1     ] &    1) != 0)
        )
        {   newpsgnote[0] = NOTE_REST;
        } else
        {   value = ((memory[PSG_PITCHA1_H] & 0x0F) * 256) + memory[PSG_PITCHA1_L];
            hertz = (float) (110837.0 / value);
            newpsgnote[0] = hertz_to_note(hertz);
        }

        if
        (   ((memory[PSG_AMPLITUDEB1] & 0x1F) == 0)
         || ((memory[PSG_MIXER1     ] &    2) != 0)
        )
        {   newpsgnote[1] = NOTE_REST;
        } else
        {   value = ((memory[PSG_PITCHB1_H] & 0x0F) * 256) + memory[PSG_PITCHB1_L];
            hertz = (float) (110837.0 / value);
            newpsgnote[1] = hertz_to_note(hertz);
        }

        if
        (   ((memory[PSG_AMPLITUDEC1] & 0x1F) == 0)
         || ((memory[PSG_MIXER1     ] &    4) != 0)
        )
        {   newpsgnote[2] = NOTE_REST;
        } else
        {   value = ((memory[PSG_PITCHC1_H] & 0x0F) * 256) + memory[PSG_PITCHC1_L];
            hertz = (float) (110837.0 / value);
            newpsgnote[2] = hertz_to_note(hertz);
        }

        if
        (   ((memory[PSG_AMPLITUDEA2] & 0x1F) == 0)
         || ((memory[PSG_MIXER2     ] &    1) != 0)
        )
        {   newpsgnote[3] = NOTE_REST;
        } else
        {   value = ((memory[PSG_PITCHA2_H] & 0x0F) * 256) + memory[PSG_PITCHA2_L];
            hertz = (float) (110837.0 / value);
            newpsgnote[3] = hertz_to_note(hertz);
        }

        if
        (   ((memory[PSG_AMPLITUDEB2] & 0x1F) == 0)
         || ((memory[PSG_MIXER2     ] &    2) != 0)
        )
        {   newpsgnote[4] = NOTE_REST;
        } else
        {   value = ((memory[PSG_PITCHB2_H] & 0x0F) * 256) + memory[PSG_PITCHB2_L];
            hertz = (float) (110837.0 / value);
            newpsgnote[4] = hertz_to_note(hertz);
        }

        if
        (   ((memory[PSG_AMPLITUDEC2] & 0x1F) == 0)
         || ((memory[PSG_MIXER2     ] &    4) != 0)
        )
        {   newpsgnote[5] = NOTE_REST;
        } else
        {   value = ((memory[PSG_PITCHC2_H] & 0x0F) * 256) + memory[PSG_PITCHC2_L];
            hertz = (float) (110837.0 / value);
            newpsgnote[5] = hertz_to_note(hertz);
    }   }
    elif (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
    {   temp =  lb_snd & 0x00001FFF;
        firstchan = TRUE;
        for (i = 1; i <= 13; i++)
        {   if (temp & (1 << (i - 1)))
            {   if (firstchan)
                {   newpsgnote[0] = lb_note[i].low;
                    firstchan = FALSE;
                } else
                {   newpsgnote[1] = lb_note[i].low;
                    break;
        }   }   }

        temp = (lb_snd & 0x03FFE000) >> 12;
        firstchan = TRUE;
        for (i = 1; i <= 13; i++)
        {   if (temp & (1 << (i - 1)))
            {   if (firstchan)
                {   newpsgnote[2] = lb_note[i].high;
                    firstchan = FALSE;
                } else
                {   newpsgnote[3] = lb_note[i].high;
                    break;
    }   }   }   }

    switch (machine)
    {
    case ARCADIA:
        newpitch = memory[A_PITCH ] & 0x7F;
        newvol   = memory[A_VOLUME] & 0x1F; // not just volume
        if (!(newvol & 0x18) || !(newvol & 0x07)) // tone and noise are both off, or guest volume is zero
        {   newvol = 0;
        }
    acase INTERTON:
    case ELEKTOR:
        newpitch = memory[pvibase + PVI_PITCH];
        if (memmap == MEMMAP_E)
        {   newvol = 15;
        } else
        {   newvol = memory[IE_NOISE] & 0xCC; // not just volume
            if (!(newvol & 0x0C)) // tone and noise are both off
            {   newvol = 0;
        }   }
    acase INSTRUCTOR:
        newpitch = idealfreq_pal[guestpitch[TONE_1STXVI] - 1].midi;
        newvol   = 7;
    acase PONG:
        switch (ponghertz)
        {
        case  0:               newvol = 0; newpitch = NOTE_REST;
        acase TONE_BOUNCE:     newvol = 1; newpitch = NOTE_B4;
        acase TONE_BAT:        newvol = 1; newpitch = NOTE_B5;
        acase 1950: case 1952: newvol = 1; newpitch = NOTE_B6;
        }
    acase TYPERIGHT:
        switch (ponghertz)
        {
        case  0:               newvol = 0; newpitch = NOTE_REST;
        acase LOWBEEP:         newvol = 1; newpitch = NOTE_A6;
        acase HIGHBEEP:        newvol = 1; newpitch = NOTE_D7;
        }
    adefault:
        newpitch = memory[pvibase + PVI_PITCH];
        newvol   = 1;
    }

    if (newvol == 0)
    {   newpsgnote[6] = NOTE_REST; // rest
    } elif (machine == INSTRUCTOR || machine == PONG || machine == TYPERIGHT)
    {   newpsgnote[6] = newpitch;
    } else
    {   newpsgnote[6] = (region == REGION_NTSC) ? (idealfreq_ntsc[newpitch].whichnote) : (idealfreq_pal[newpitch].whichnote);
    }

    for (i = 0; i < 7; i++)
    {   if (newpsgnote[i] != oldpsgnote[i])
        {   drawupnote((i == 6) ? 0 : (i + 1), oldpsgnote[i]);
    }   }
    if (memmap == MEMMAP_F)
    {   drawdownnote(1, newpsgnote[0], EMUBRUSH_DARKCYAN);
        drawdownnote(2, newpsgnote[1], EMUBRUSH_DARKCYAN);
        drawdownnote(3, newpsgnote[2], EMUBRUSH_DARKCYAN);
        drawdownnote(4, newpsgnote[3], EMUBRUSH_DARKORANGE);
        drawdownnote(5, newpsgnote[4], EMUBRUSH_DARKORANGE);
        drawdownnote(6, newpsgnote[5], EMUBRUSH_DARKORANGE);
    } elif (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
    {   drawdownnote(1, newpsgnote[0], EMUBRUSH_DARKCYAN);
        drawdownnote(2, newpsgnote[1], EMUBRUSH_DARKCYAN);
        drawdownnote(3, newpsgnote[2], EMUBRUSH_DARKORANGE);
        drawdownnote(4, newpsgnote[3], EMUBRUSH_DARKORANGE);
    }
    switch (machine)
    {
    case ARCADIA:
        if   ((newvol & 0x18) == 0x18) drawdownnote(0, newpsgnote[6], EMUBRUSH_DARKPURPLE);
        elif ((newvol & 0x18) == 0x10) drawdownnote(0, newpsgnote[6], EMUBRUSH_DARKRED);
        elif ((newvol & 0x18) == 0x08) drawdownnote(0, newpsgnote[6], EMUBRUSH_DARKGREEN);
    acase INTERTON:
    case ELEKTOR:
        if (memmap == MEMMAP_E)
        {                                  drawdownnote(0, newpsgnote[6], EMUBRUSH_DARKGREEN);
        } else
        {   if   ((newvol & 0x0C) == 0x0C) drawdownnote(0, newpsgnote[6], EMUBRUSH_DARKPURPLE);
            elif ((newvol & 0x0C) == 0x08) drawdownnote(0, newpsgnote[6], EMUBRUSH_DARKRED);
            elif ((newvol & 0x0C) == 0x04) drawdownnote(0, newpsgnote[6], EMUBRUSH_DARKGREEN);
        }
    adefault: /* eg. ZACCARIA, PONG */     drawdownnote(0, newpsgnote[6], EMUBRUSH_DARKGREEN);
    }
    for (i = 0; i < 7; i++)
    {   oldpsgnote[i] = newpsgnote[i];
    }

#ifdef WIN32
    ReleaseDC(GetDlgItem(subwin[SUBWINDOW_MUSIC].hwnd, IDC_MUSIC), MusicRastPtr);
    ReleaseDC(GetDlgItem(subwin[SUBWINDOW_MUSIC].hwnd, IDC_STAVE), StaveRastPtr);
#endif
}

EXPORT void tools_music_engine(void)
{   int i;

    for (i = 0; i < 7; i++)
    {   oldpsgnote[i] = NOTE_REST;
    }

    for (i = 0; i < NOTES; i++)
    {   notes[i].ghosted = FALSE;
    }

#ifdef AMIGA
    ghost_notes();
#endif

    wheremusicmouse[0] = wheremusicmouse[1] = -2;

    update_music();
}

EXPORT void tapedeck_settitle(void)
{   int i,
        length,
        where;

    if (machine == PHUNSY)
    {   strcpy((char*) tapetitlestring, LLL(MSG_HAIL_TAPEDECKS, "Cassette Tape Decks"));
    } else
    {   strcpy((char*) tapetitlestring, LLL(MSG_HAIL_TAPEDECK,  "Cassette Tape Deck" ));
    }
    if (tapemode != TAPEMODE_NONE)
    {   where = -1;
        length = (int) strlen((const char*) fn_tape[0]);
        for (i = 0; i < length; i++)
        {   if
            (   fn_tape[0][i] == ':'
             || fn_tape[0][i] == '\\'
             || fn_tape[0][i] == '/'
            )
            {   where = i;
                // don't break, we want the LAST component, not the FIRST
        }   }
        strcat((char*) tapetitlestring, ": ");
        strcat((char*) tapetitlestring, (const char*) &fn_tape[0][where + 1]);
}   }

EXPORT void papertape_settitle(void)
{   int i, j, k,
        length;

    if (machine == TWIN)
    {   strcpy((char*) papertapetitlestring, LLL(MSG_HAIL_PAPERTAPES, "Papertape Units"));
    } else
    {   strcpy((char*) papertapetitlestring, LLL(MSG_HAIL_PAPERTAPE,  "Papertape Unit" ));
    }

    for (k = 0; k < ((machine == TWIN) ? 2 : 1); k++) // these parentheses are needed!
    {   if (papertapemode[k] != TAPEMODE_NONE && fn_tape[2 + k])
        {   length = strlen(fn_tape[2 + k]);
            j = 0;
            for (i = length - 1; i >= 0; i--)
            {   if (fn_tape[2 + k][i] == '\\' || fn_tape[2 + k][i] == '/' || fn_tape[2 + k][i] == ':')
                {   j = i + 1;
                    break;
            }   }
            if (k == 0 || papertapemode[0] == TAPEMODE_NONE || fn_tape[2][0] == EOS) // first
            {   strcat(papertapetitlestring, ": ");
            } else
            {   strcat(papertapetitlestring, " & ");
            }
            strcat(papertapetitlestring, &fn_tape[2 + k][j]);
}   }   }

EXPORT void enqueue(UWORD hostcode, TEXT guestcode)
{   if (machine == TWIN)
    {   switch (hostcode)
        {
        case SCAN_UP:    thequeue[queuepos++] = 0x1B; thequeue[queuepos++] = 'A'; return;
        case SCAN_DOWN:  thequeue[queuepos++] = 0x1B; thequeue[queuepos++] = 'B'; return;
        case SCAN_RIGHT: thequeue[queuepos++] = 0x1B; thequeue[queuepos++] = 'C'; return;
        case SCAN_LEFT:  thequeue[queuepos++] = 0x1B; thequeue[queuepos++] = 'D'; return;
#ifdef WIN32
        case SCAN_HOME:  thequeue[queuepos++] = 0x1B; thequeue[queuepos++] = 'H'; return;
#endif
    }   }
 /* if (machine == BINBUG)
    {   switch (guestcode)
            {
            case  '0': keys_column[0] |= 1;
            acase '1': keys_column[1] |= 1;
            acase '2': keys_column[2] |= 1;
            acase '3': keys_column[3] |= 1;
            acase '4': keys_column[0] |= 2;
            acase '5': keys_column[1] |= 2;
            acase '6': keys_column[2] |= 2;
            acase '7': keys_column[3] |= 2;
            acase '8': keys_column[0] |= 4;
            acase '9': keys_column[1] |= 4;
            acase 'A': keys_column[2] |= 4;
            acase 'B': keys_column[3] |= 4;
            acase 'C': keys_column[0] |= 8;
            acase 'D': keys_column[1] |= 8;
            acase 'E': keys_column[2] |= 8;
            acase 'F': keys_column[3] |= 8;
    }   } */

    if (machine == TYPERIGHT)
    {   if (keyframes[hostcode] == 1 || !KeyDown((UWORD) hostcode))
        {   thequeue[queuepos++] = guestcode;
    }   }
    elif (!queuekeystrokes)
    {   guestkey = guestcode;
        hostkey  = hostcode;
        if ((machine == PIPBUG || machine == BINBUG) && tt_kybdstate == 0)
        {   tt_kybdcode  = guestcode;
            tt_kybdstate = 1; // start bit
            tt_kybdtill  = cycles_2650 + cpb;
            psu &= ~(PSU_S);
    }   }
    else
    {   if
        (   (keyframes[hostcode] == 1) // initial press
         || ((machine != TWIN || softrept) && (keyframes[hostcode] - 1) % REPEATRATE == 0) // key repeat
        )
        {   thequeue[queuepos++] = guestcode;
}   }   }

EXPORT void verbosetape_load(void)
{   // assert(verbosetape);

    switch (machine)
    {
    case ELEKTOR:
        if (iar == 0xD4)
        {   if (oldiar == 0x26A)
            {   zprintf(TEXTPEN_TAPE, LLL(MSG_VERBOSETAPE1, "BIOS has finished loading from tape.\n"));
        }   }
        elif (iar == 0x722)
        {   gettapepos();
            zprintf
            (   TEXTPEN_TAPE,
                "Read bit %d as %d at tape position %d (%s).\n",
                memory[0x89B] - 1,
                memory[0x89C] & 1,
                samplewhere,
                tapeposstring
            );
        } elif (iar == 0x72F)
        {   tape_read(r[1]); // $8AA
        }
    acase PIPBUG:
        if (pipbug_biosver == PIPBUG_PIPBUG1BIOS)
        {   switch (iar)
            {
            case  0x292: tape_rawstart();
            acase 0x29B: tape_rawreadbit();
            acase 0x2A7: tape_rawread(r[0]);
            acase 0x3B8: tape_foundcolon(r[0]);
            acase 0x3D1: tape_loaded();
            acase 0x3F0: tape_read(r[0]); // $42C
        }   }
    acase BINBUG:
        if (binbug_biosver == BINBUG_61)
        {   if (iar == 0x63CB)
            {   tape_read(r[0]); // -1
            } elif (iar == 0x63FD)
            {   do_postamble();
        }   }
    acase CD2650:
        switch (iar)
        {
        case  0x249: tape_loaded();
        acase 0x2F1: tape_rawstart();
        acase 0x2FB: tape_rawreadbit();
        acase 0x29B: tape_foundcolon(r[3]);
        acase 0x303: tape_rawread(r[3]);
        acase 0x30E: tape_read(r[3]); // $17F9
}   }   }

EXPORT void verbosetape_save(void)
{   switch (machine)
    {
    case ELEKTOR:
        if (iar == 0xD4)
        {   if (oldiar == 0x6E9)
            {   zprintf(TEXTPEN_TAPE, LLL(MSG_VERBOSETAPE2, "BIOS has finished saving to tape.\n"));
        }   }
        elif (iar == 0x6D3)
        {   gettapepos();
            zprintf
            (   TEXTPEN_TAPE,
                LLL
                (   MSG_WROTETAPE,
                    "Wrote $%02X (%s) to tape position %d (%s).\n"
                ),
                (int) r[0],
                asciiname_short[r[0]],
                samplewhere,
                tapeposstring
            );
        } elif (iar == 0x6EC || iar == 0x6F0)
        {   gettapepos();
            zprintf
            (   TEXTPEN_TAPE,
                "Writing bit %d as %d at tape position %d (%s).\n",
                r[5] - 1,
                (iar == 0x6EC) ? 1 : 0,
                samplewhere,
                tapeposstring
            );
        }
    acase PIPBUG:
        if (pipbug_biosver == PIPBUG_PIPBUG1BIOS)
        {   switch (iar)
            {
            case  0x2D4: tape_wrotebyte(r[0]);
            acase 0x334: tape_wrotecolon(':');
            acase 0x358: zprintf(TEXTPEN_TAPE, LLL(MSG_VERBOSETAPE1, "BIOS has finished saving to tape.\n"));
        }   }
    acase BINBUG:
        if (binbug_biosver == BINBUG_61)
        {   if (iar == 0x6323)
            {   tape_wrotebyte(r[0]);
            } elif (iar == 0x6384)
            {   do_postamble();
        }   }
    acase CD2650:
        switch (iar)
        {
        case  0x217: tape_wrotecolon(':');
        acase 0x24C: zprintf(TEXTPEN_TAPE, LLL(MSG_VERBOSETAPE2, "BIOS has finished saving to tape.\n"));
        acase 0x26E: tape_wrotebyte(r[3]);
}   }   }

EXPORT void gettapepos(void)
{   FAST float rem,
               totalsecs_f;
    FAST int   mins,
               totalsecs;

    // Ideally we should format this string according to the user's locale.

    // assert(tape_hz > 0.0);
    totalsecs_f = (float) samplewhere_f / (float) tape_hz;
    totalsecs   = (int) totalsecs_f;
    mins        = totalsecs / 60;
    rem         = totalsecs_f - (float) (mins * 60.0);
    if (mins >= 0 && mins <= 999)
    {   if (rem < 10.0)
        {   sprintf((char*) tapeposstring, "%d:0%6f", mins, rem);
        } else
        {   sprintf((char*) tapeposstring, "%d:%6f", mins, rem);
    }   }
    else
    {   sprintf((char*) tapeposstring, "###:##.######");
}   }

MODULE void tape_foundcolon(UBYTE value)
{   gettapepos();
    if (value == ':')
    {   zprintf
        (   TEXTPEN_TAPE,
            LLL(MSG_TAPEFOUND1, "Found start character (':') at tape position %d (%s).\n"),
            samplewhere,
            tapeposstring
        );
    } else
    {   zprintf
        (   TEXTPEN_TAPE,
            LLL(MSG_TAPEFOUND2, "Searching for start character (':'), instead found %s at tape position %d (%s).\n"),
            asciiname_long[value],
            samplewhere,
            tapeposstring
        );
}   }

MODULE void tape_wrotecolon(TEXT colon)
{   gettapepos();
    zprintf
    (   TEXTPEN_TAPE,
        LLL(MSG_WROTESTART, "Wrote start character ('%c') at tape position %d (%s).\n"),
        colon,
        samplewhere,
        tapeposstring
    );
}

MODULE void tape_wrotebyte(UBYTE value)
{   gettapepos();
    zprintf
    (   TEXTPEN_TAPE,
        LLL
        (   MSG_WROTETAPE,
            "Wrote $%02X (%s) to tape position %d (%s).\n"
        ),
        (int) value,
        asciiname_short[value],
        samplewhere,
        tapeposstring
    );
}

MODULE void tape_loaded(void)
{   gettapepos();
    zprintf
    (   TEXTPEN_TAPE,
        LLL(MSG_TAPEEND, "*** Finished loading file at tape position %d (%s).\n"),
        samplewhere,
        tapeposstring
    );
}

MODULE void tape_read(UBYTE value)
{   gettapepos();
    zprintf
    (   TEXTPEN_TAPE,
        LLL(
            MSG_READTAPE,
            "Read $%02X (%s) from tape position %d (%s).\n"
        ),
        (int) value,
        asciiname_short[value], // better if we didn't show this for Elektor
        samplewhere,
        tapeposstring
    );
}

MODULE void tape_rawread(UBYTE value)
{   gettapepos();
    zprintf
    (   TEXTPEN_TAPE,
        "Finished reading character %s at tape position %d (%s).\n",
        asciiname_long[value],
        samplewhere,
        tapeposstring
    );

#ifdef VERBOSE
    if
    (   machine != PIPBUG
     || (value >= '0' && value <= '9')
     || (value >= 'A' && value <= 'F') // lowercase equivalents are illegal
     || value == ':'
     || value == ' '
     || value == CR
     || value == LF
     || value == DEL
    )
    {   ;
    } else
    {   zprintf
        (   TEXTPEN_VERBOSE,
            "*** Warning: character %s is out of range!\n",
            asciiname_long[value]
        );
    }
#endif
}

MODULE void tape_rawstart(void)
{   gettapepos();
    zprintf
    (   TEXTPEN_TAPE,
        LLL(MSG_TAPESTART, "Found start bit at tape position %d (%s).\n"),
        samplewhere,
        tapeposstring
    );
}

MODULE void tape_rawreadbit(void)
{   gettapepos();
    if (psu & 0x80)
    {   zprintf(TEXTPEN_TAPE, "Read %%1 (fast pulses) at tape position %d (%s).\n", samplewhere, tapeposstring);
    } else
    {   zprintf(TEXTPEN_TAPE, "Read %%0 (slow pulses) at tape position %d (%s).\n", samplewhere, tapeposstring);
}   }

EXPORT UBYTE getrandom(int modulo)
{   UBYTE t;

    if (recmode == RECMODE_PLAY)
    {   return loadbyte();
    } else
    {   t = (UBYTE) (rand() % modulo);
        if (recmode == RECMODE_RECORD)
        {   savebyte(t);
        }
        return t;
}   }

EXPORT FLAG is_drawable(void)
{   PERSIST ULONG drawwaittill = 0;
    FAST    ULONG drawtime;

    if
    (   limitrefreshes
     && (turbo || speedup > SPEED_4QUARTERS)
    )
    {   drawtime = thetime();
        if (drawtime < drawwaittill)
        {   if (drawwaittill - drawtime >= ONE_MILLION)
            {
#ifdef VERBOSE
                zprintf(TEXTPEN_VERBOSE, "Refresh rate was <1 FPS!\n");
#endif
                drawwaittill = drawtime + usecsperframe[region];
            }
            return FALSE;
        } // implied else
        drawwaittill = drawtime + usecsperframe[region];
    }

    return TRUE;
}

EXPORT FLAG nearby(int x1, int y1, int x2, int y2)
{   if (abs(x1 - x2) >= 3 || abs(y1 - y2) >= 3)
    {   return FALSE;
    } else
    {   return TRUE;
}   }

MODULE void ghost_notes(void)
{   int i;

#ifdef WIN32
    // assert(MusicRastPtr);
#endif

    if (memmap == MEMMAP_LAZARIAN || memmap == MEMMAP_LASERBATTLE)
    {   for (i = NOTE_A0; i <= NOTE_B3; i++)
        {   ghost_note(i);
        }
        for (i = NOTE_CS6; i <= NOTE_C9; i++)
        {   ghost_note(i);
    }   }
    else
    {   ghost_note(NOTE_A0);
        ghost_note(NOTE_AS0);
        if (machine == ARCADIA)
        {   for (i = NOTE_B0; i <= NOTE_AS1; i++)
            {   ghost_note(i);
        }   }
        ghost_note(NOTE_D5 );
        ghost_note(NOTE_FS5);
        ghost_note(NOTE_GS5);
        ghost_note(NOTE_AS5);
        ghost_note(NOTE_C6 );
        ghost_note(NOTE_D6 ); ghost_note(NOTE_DS6);
        ghost_note(NOTE_F6 ); ghost_note(NOTE_FS6);
        ghost_note(NOTE_GS6); ghost_note(NOTE_A6 ); ghost_note(NOTE_AS6);
        ghost_note(NOTE_C7 ); ghost_note(NOTE_CS7); ghost_note(NOTE_D7 ); ghost_note(NOTE_DS7);
        for (i = NOTE_F7; i <= NOTE_AS7; i++)
        {   ghost_note(i);
        }
        for (i = NOTE_C8; i <= NOTE_C9; i++)
        {   ghost_note(i);
    }   }

    if (memmap != MEMMAP_F)
    {   ghost_bar(6);
        ghost_bar(5);
        if (memmap != MEMMAP_LASERBATTLE && memmap != MEMMAP_LAZARIAN)
        {   ghost_bar(1);
            ghost_bar(2);
            ghost_bar(3);
            ghost_bar(4);
}   }   }

EXPORT void do_axes(void)
{   PERSIST int nx[2] = { 0, 0       },
                ny[4] = { 0, 0, 0, 0 };

    if
    (   !subwin[SUBWINDOW_MONITOR_XVI].hwnd
     || (   machine != ARCADIA
         && machine != INTERTON
         && machine != ELEKTOR
         && machine != PONG
         && machine != BINBUG
    )   )
    {   return;
    }

    drawaxesline(    48, 48, nx[0], ny[0], EMURGBPEN_WHITE);
    drawaxesline(    48, 48, nx[1], ny[1], EMURGBPEN_WHITE);
    if (memmap == MEMMAP_8550)
    {   drawaxesline(48, 48, nx[0], ny[2], EMURGBPEN_WHITE);
        drawaxesline(48, 48, nx[1], ny[3], EMURGBPEN_WHITE);
    }

    nx[0] = ax[0] * 3 / 8; // 0..255 -> 0..95
    nx[1] = ax[1] * 3 / 8; // 0..255 -> 0..95
    ny[0] = ay[0] * 3 / 8; // 0..255 -> 0..95
    ny[1] = ay[1] * 3 / 8; // 0..255 -> 0..95
    ny[2] = ay[2] * 3 / 8; // 0..255 -> 0..95
    ny[3] = ay[3] * 3 / 8; // 0..255 -> 0..95

    if
    (                           nearby(nx[0], ny[0], nx[1], ny[1])
     || (machine == PONG && (   nearby(nx[0], ny[0], 48,    ny[2])
                             || nearby(nx[0], ny[0], 48,    ny[3])
    )   )                   )
    {   drawaxesline(48, 48, nx[0], ny[0], EMURGBPEN_BLACK);
    } else
    {   drawaxesline(48, 48, nx[0], ny[0], EMURGBPEN_DARKRED);
    }
    if
    (                           nearby(nx[1], ny[1], nx[0], ny[0])
     || (machine == PONG && (   nearby(nx[1], ny[1], 48,    ny[2])
                             || nearby(nx[1], ny[1], 48,    ny[3])
    )   )                   )
    {   drawaxesline(48, 48, nx[1], ny[1], EMURGBPEN_BLACK);
    } else
    {   drawaxesline(48, 48, nx[1], ny[1], EMURGBPEN_DARKBLUE);
    }
    if (memmap == MEMMAP_8550)
    {   if
        (                       nearby(   48, ny[2], nx[0], ny[0])
                             || nearby(   48, ny[2], nx[1], ny[1])
                             || nearby(   48, ny[2], 48,    ny[3])
        )
        {   drawaxesline(48, 48, nx[0], ny[2], BLACK);
        } else
        {   drawaxesline(48, 48, nx[0], ny[2], EMURGBPEN_RED);
        }
        if
        (                       nearby(   48, ny[3], nx[0], ny[0])
                             || nearby(   48, ny[3], nx[1], ny[1])
                             || nearby(   48, ny[3], 48,    ny[2])
        )
        {   drawaxesline(48, 48, nx[1], ny[3], BLACK);
        } else
        {   drawaxesline(48, 48, nx[1], ny[3], EMURGBPEN_BLUE);
    }   }

    wpa8(CANVAS_AXES, 0, 0);
}

#ifdef AMIGA
EXPORT void drawaxesline(int x1, int y1, int x2, int y2, int colour)
#endif
#ifdef WIN32
EXPORT void drawaxesline(int x1, int y1, int x2, int y2, COLORREF colour)
#endif
{   FAST int dx,      // deltas
             dy,
             dx2,     // scaled deltas
             dy2,
             err,     // the error term
             i,       // looping variable
             ix,      // increase rate on the x axis
             iy,      // increase rate on the y axis
             nowx, nowy;

    // difference between starting and ending points
    dx = x2 - x1;
    dy = y2 - y1;

    // calculate direction of the vector and store in ix and iy
    if (dx >= 0)
    {   ix =  1;
    } elif (dx < 0)
    {   dx = -dx;
        ix = -1;
    } else
    {   ix = 0;
    }

    if (dy >= 0)
    {   iy =  1;
    } elif (dy < 0)
    {   dy = -dy;
        iy = -1;
    } else
    {   iy = 0;
    }

    // scale deltas and store in dx2 and dy2
    dx2 = dx * 2;
    dy2 = dy * 2;
    nowx = x1;
    nowy = y1;
    if (dx > dy) // dx is the major axis
    {   // initialize the error term
        err = dy2 - dx;
        for (i = 0; i <= dx; i++)
        {   DRAWAXES(nowx, nowy, colour);
            if (err >= 0)
            {   err -= dx2;
                nowy += iy;
            }
            err += dy2;
            nowx += ix;
    }   }
    else // dy is the major axis
    {   // initialize the error term
        err = dx2 - dy;
        for (i = 0; i <= dy; i++)
        {   DRAWAXES(nowx, nowy, colour);
            if (err >= 0)
            {   err -= dy2;
                nowx += ix;
            }
            err += dx2;
            nowy += iy;
    }   }

    // draw knob
    //                           DRAWAXES(x2    , y2    , colour); // centre
    if (x2 > 0)
    {                            DRAWAXES(x2 - 1, y2    , colour); // west
        if (y2 > 0)              DRAWAXES(x2 - 1, y2 - 1, colour); // northwest
        if (y2 < AXESHEIGHT - 1) DRAWAXES(x2 - 1, y2 + 1, colour); // southwest
    }
    if (x2 < AXESWIDTH - 1)
    {                            DRAWAXES(x2 + 1, y2    , colour); // east
        if (y2 > 0)              DRAWAXES(x2 + 1, y2 - 1, colour); // northeast
        if (y2 < AXESHEIGHT - 1) DRAWAXES(x2 + 1, y2 + 1, colour); // southeast
    }
    if (y2 > 0)                  DRAWAXES(x2    , y2 - 1, colour); // north
    if (y2 < AXESHEIGHT - 1)     DRAWAXES(x2    , y2 + 1, colour); // south
}

EXPORT int getsmallimage2(int thegame, int thememmap)
{   if
    (   thegame              == -1
     || known[thegame].glyph == -1
#ifdef AMIGA
     || known[thegame].glyph >= ARCADIAGLYPHS
#endif
    )
    {   return memmap_to_smlimage[thememmap];
    } // implied else

    return IMAGE_FIRSTGAME + known[thegame].glyph;
}

EXPORT FLAG getmoncolour(int whichmongad, ULONG* whichemupen, int* whichemubrush)
{   int addr;

    addr = getmonitoraddr(whichmongad);

    switch (whichmongad)
    {
    case  MONGAD_R0:
    case  MONGAD_R1:
    case  MONGAD_R2:
    case  MONGAD_R3:
    case  MONGAD_R4:
    case  MONGAD_R5:
    case  MONGAD_R6:
    case  MONGAD_SLAVE_R0:
    case  MONGAD_SLAVE_R1:
    case  MONGAD_SLAVE_R2:
    case  MONGAD_SLAVE_R3:
    case  MONGAD_SLAVE_R4:
    case  MONGAD_SLAVE_R5:
    case  MONGAD_SLAVE_R6:
        if (memflags[addr] & WATCHPOINT)
        {   *whichemupen   = EMUPEN_ORANGE;
            *whichemubrush = EMUBRUSH_ORANGE;
        } else
        {   *whichemupen   = EMUPEN_GREEN;
            *whichemubrush = EMUBRUSH_GREEN;
        }
    acase MONGAD_RAS0:
    case  MONGAD_RAS1:
    case  MONGAD_RAS2:
    case  MONGAD_RAS3:
    case  MONGAD_RAS4:
    case  MONGAD_RAS5:
    case  MONGAD_RAS6:
    case  MONGAD_RAS7:
        if
        (   (whichcpu == 0 && whichmongad - MONGAD_RAS0 == (      psu & PSU_SP))
         || (whichcpu == 1 && whichmongad - MONGAD_RAS0 == (other_psu & PSU_SP))
        )
        {   *whichemupen   = EMUPEN_WHITE;
            *whichemubrush = EMUBRUSH_WHITE;
        } else
        {   *whichemupen   = EMUPEN_GREY;   // grey so that the white one
            *whichemubrush = EMUBRUSH_GREY; // stands out in comparison
        }
    acase  MONGAD_SLAVE_RAS0:
    case  MONGAD_SLAVE_RAS1:
    case  MONGAD_SLAVE_RAS2:
    case  MONGAD_SLAVE_RAS3:
    case  MONGAD_SLAVE_RAS4:
    case  MONGAD_SLAVE_RAS5:
    case  MONGAD_SLAVE_RAS6:
    case  MONGAD_SLAVE_RAS7:
        if
        (   (whichcpu == 0 && whichmongad - MONGAD_RAS0 == (other_psu & PSU_SP))
         || (whichcpu == 1 && whichmongad - MONGAD_RAS0 == (      psu & PSU_SP))
        )
        {   *whichemupen   = EMUPEN_WHITE;
            *whichemubrush = EMUBRUSH_WHITE;
        } else
        {   *whichemupen   = EMUPEN_GREY;   // grey so that the white one
            *whichemubrush = EMUBRUSH_GREY; // stands out in comparison
        }
    acase MONGAD_S:
    case MONGAD_SLAVE_S:
        *whichemupen       = EMUPEN_BLUE;
        *whichemubrush     = EMUBRUSH_BLUE;
    acase MONGAD_IAR:
    case MONGAD_SLAVE_IAR:
        *whichemupen       = EMUPEN_YELLOW;
        *whichemubrush     = EMUBRUSH_YELLOW;
    acase MONGAD_UF1:
    case  MONGAD_UF2:
    case  MONGAD_SLAVE_UF1:
    case  MONGAD_SLAVE_UF2:
        if (supercpu)
        {   *whichemupen   = EMUPEN_GREEN;
            *whichemubrush = EMUBRUSH_GREEN;
        } else
        {   *whichemupen   = EMUPEN_RED;
            *whichemubrush = EMUBRUSH_RED;
        }
    acase MONGAD_F:
    case  MONGAD_II:
    case  MONGAD_SP:
    case  MONGAD_CC:
    case  MONGAD_IDC:
    case  MONGAD_RS:
    case  MONGAD_WC:
    case  MONGAD_OVF:
    case  MONGAD_COM:
    case  MONGAD_C:
    case  MONGAD_SLAVE_F:
    case  MONGAD_SLAVE_II:
    case  MONGAD_SLAVE_SP:
    case  MONGAD_SLAVE_CC:
    case  MONGAD_SLAVE_IDC:
    case  MONGAD_SLAVE_RS:
    case  MONGAD_SLAVE_WC:
    case  MONGAD_SLAVE_OVF:
    case  MONGAD_SLAVE_COM:
    case  MONGAD_SLAVE_C:
        *whichemupen       = EMUPEN_GREEN;
        *whichemubrush     = EMUBRUSH_GREEN;
    adefault:
        if (addr == OUTOFRANGE)
        {   return FALSE;
        } // implied else
        if (memflags[addr] & BREAKPOINT)
        {   *whichemupen   = EMUPEN_CYAN;
            *whichemubrush = EMUBRUSH_CYAN;
        } elif (memflags[addr] & WATCHPOINT)
        {   *whichemupen   = EMUPEN_ORANGE;
            *whichemubrush = EMUBRUSH_ORANGE;
        } elif
        (   mirror_r[addr] != addr
         || mirror_w[addr] != addr
        )
        {   *whichemupen   = EMUPEN_PURPLE;
            *whichemubrush = EMUBRUSH_PURPLE;
        } elif (memflags[addr] & NOWRITE)
        {   if (memflags[addr] & NOREAD)
            {   *whichemupen   = EMUPEN_RED;
                *whichemubrush = EMUBRUSH_RED;
            } else
            {   *whichemupen   = EMUPEN_BLUE;
                *whichemubrush = EMUBRUSH_BLUE;
        }   }
        elif (memflags[addr] & NOREAD)
        {   *whichemupen   = EMUPEN_YELLOW;
            *whichemubrush = EMUBRUSH_YELLOW;\
        } else
        {   *whichemupen   = EMUPEN_GREEN;
            *whichemubrush = EMUBRUSH_GREEN;
    }   }

    return TRUE;
}

EXPORT void generate_controlstip(int localkey)
{   FAST FLAG done;
    FAST int  thekeypad,
              whichplayer;

    whichplayer = keyinfo[whichkeyrect][localkey].player;
    if (whichplayer != -1 && swapped && (machine == ARCADIA || machines[machine].pvis || machine == PONG)) // INSTRUCTOR, etc. don't allow swapping
    {   whichplayer = whichplayer ? 0 : 1;
    }
    thekeypad = keyinfo[whichkeyrect][localkey].to_keypad;

    if (whichplayer == -1) // console keys, etc.
    {   strcpy((char*) controltip[4], "-");

        if ((machine == ARCADIA && localkey == 48) || (machine == INTERTON && localkey == 40))
        {   strcpy((char*) controltip[0], LLL(MSG_CARTRIDGESLOT, "Cartridge slot"));
            strcpy((char*) controltip[1], LLL(MSG_CARTRIDGESLOT, "Cartridge slot"));
#ifdef WIN32
            strcpy(        controltip[2], "Ctrl+O");
#endif
#ifdef AMIGA
            strcpy((char*) controltip[2], "Amiga-O");
#endif
            strcpy((char*) controltip[3], "-");
            return;
        }

        if
        (   (localkey == 0 && (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR || machine == INSTRUCTOR || machine == PONG))
         || (localkey == 1 &&  machines[machine].coinop)
        )
        {   strcpy((char*) controltip[0], machines[machine].consolekeyname[0]);
            strcpy((char*) controltip[2], keyname[console[0]].name);
#ifdef WIN32
            strcpy((char*) controltip[3], LLL(MSG_START, "START"));
#endif
#ifdef AMIGA
            strcpy((char*) controltip[3], LLL(MSG_KEY_PLAY_LONG, "Play"));
#endif
        } elif
        (   (localkey == 1 && (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR || machine == INSTRUCTOR || machine == PONG))
         || (localkey == 2 &&  machines[machine].coinop)
        )
        {   strcpy((char*) controltip[0], machines[machine].consolekeyname[1]);
            strcpy((char*) controltip[2], keyname[console[1]].name);
#ifdef WIN32
            if (button[0][4] == button[1][4])
            {   sprintf((char*) controltip[3], "%d", button[0][4]);
            } else
            {   sprintf
                (   (char*) controltip[3],
                    "%d (%s), %d (%s)", // don't use ampersand!
                    button[whichplayer][4],
                    LLL(MSG_1ST, "1st"),
                    button[whichplayer ? 0 : 1][4],
                    LLL(MSG_2ND, "2nd")
                );
            }
#endif
#ifdef AMIGA
            strcpy((char*) controltip[3], LLL(MSG_REVERSE, "Reverse"));
#endif
        } elif (localkey == 2 && (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR || machine == INSTRUCTOR || machine == PONG))
        {   strcpy((char*) controltip[0], machines[machine].consolekeyname[2]);
            strcpy((char*) controltip[2], keyname[console[2]].name);
#ifdef WIN32
            if (button[0][5] == button[1][5])
            {   sprintf((char*) controltip[3], "%d", button[0][5]);
            } else
            {   sprintf
                (   (char*) controltip[3],
                    "%d (%s), %d (%s)", // don't use ampersand!
                    button[whichplayer][5],
                    LLL(MSG_1ST, "1st"),
                    button[whichplayer ? 0 : 1][5],
                    LLL(MSG_2ND, "2nd")
                );
            }
#endif
#ifdef AMIGA
            strcpy((char*) controltip[3], LLL(MSG_FORWARD, "Forward"));
#endif
        } elif
        (   (localkey ==  3 && (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR || machine == INSTRUCTOR || machine == PONG))
         || (localkey ==  7 && machine == PONG)
         || (localkey == 49 && machine == ARCADIA)
         || (localkey == 41 && machine == INTERTON)
        )
        {   if (machine == ELEKTOR || machine == INSTRUCTOR || (machine == PONG && localkey == 3))
            {   strcpy((char*) controltip[0], machines[machine].consolekeyname[3]);
                strcpy((char*) controltip[2], keyname[console[3]].name);
                strcpy((char*) controltip[3], "-");
            } else
            {   strcpy((char*) controltip[0], LLL(MSG_RESET, "RESET"));
                strcpy((char*) controltip[2], "F5");
#ifdef WIN32
                strcpy(        controltip[3], LLL(MSG_SELECT, "SELECT"));
#else
                strcpy((char*) controltip[3], "-");
#endif
        }   }
        elif
        (   ((machine == ARCADIA || machine == INTERTON || machine == ELEKTOR) && (localkey == 28 || localkey == 32))
         || ((memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN) && localkey == 5)
         || (machine == MALZAK && localkey == 3)
        )
        {   strcpy((char*) controltip[0], LLL(MSG_KEY_UP, "Up"));
            strcpy((char*) controltip[3], LLL(MSG_KEY_UP, "Up"));
            strcpy((char*) controltip[2], keyname[keypads[whichplayer][thekeypad]].name);
        } elif
        (   ((machine == ARCADIA || machine == INTERTON || machine == ELEKTOR) && (localkey == 29 || localkey == 33))
         || (machine == ZACCARIA && localkey == 3)
         || (machine == MALZAK && localkey == 9)
        )
        {   strcpy((char*) controltip[0], LLL(MSG_KEY_LT, "Lt"));
            strcpy((char*) controltip[3], LLL(MSG_KEY_LT, "Lt"));
            strcpy((char*) controltip[2], keyname[keypads[whichplayer][thekeypad]].name);
        } elif
        (   ((machine == ARCADIA || machine == INTERTON || machine == ELEKTOR) && (localkey == 30 || localkey == 34))
         || ((memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN) && localkey == 6)
         || (machine == MALZAK && localkey == 7)
        )
        {   strcpy((char*) controltip[0], LLL(MSG_KEY_DN, "Dn"));
            strcpy((char*) controltip[3], LLL(MSG_KEY_DN, "Dn"));
            strcpy((char*) controltip[2], keyname[keypads[whichplayer][thekeypad]].name);
        } elif
        (   ((machine == ARCADIA || machine == INTERTON || machine == ELEKTOR) && (localkey == 31 || localkey == 35))
         || (machine == ZACCARIA && localkey == 4)
         || (machine == MALZAK && localkey == 5)
        )
        {   strcpy((char*) controltip[0], LLL(MSG_KEY_RT, "Rt"));
            strcpy((char*) controltip[3], LLL(MSG_KEY_RT, "Rt"));
            strcpy((char*) controltip[2], keyname[keypads[whichplayer][thekeypad]].name);
        } elif (machine == MALZAK)
        {   switch (localkey)
            {
            case  4:
                strcpy((char*) controltip[0], LLL(MSG_UPRIGHT, "UpRt"));
                strcpy((char*) controltip[3], LLL(MSG_UPRIGHT, "UpRt"));
                strcpy((char*) controltip[2], keyname[keypads[whichplayer][thekeypad]].name);
            acase 6:
                strcpy((char*) controltip[0], LLL(MSG_DOWNRIGHT, "DnRt"));
                strcpy((char*) controltip[3], LLL(MSG_DOWNRIGHT, "DnRt"));
                strcpy((char*) controltip[2], keyname[keypads[whichplayer][thekeypad]].name);
            acase 8:
                strcpy((char*) controltip[0], LLL(MSG_DOWNLEFT, "DnLt"));
                strcpy((char*) controltip[3], LLL(MSG_DOWNLEFT, "DnLt"));
                strcpy((char*) controltip[2], keyname[keypads[whichplayer][thekeypad]].name);
            acase 10:
                strcpy((char*) controltip[0], LLL(MSG_UPLEFT, "UpLt"));
                strcpy((char*) controltip[3], LLL(MSG_UPLEFT, "UpLt"));
                strcpy((char*) controltip[2], keyname[keypads[whichplayer][thekeypad]].name);
        }   }
        elif (machine == INSTRUCTOR)
        {   switch (localkey)
            {
            case  28:
            case  29:
                strcpy((char*) controltip[0], LLL(MSG_DIPSWITCHES, "DIP switches"));
#ifdef WIN32
                strcpy((char*) controltip[2], "Ctrl+Shift+2");
#endif
#ifdef AMIGA
                strcpy((char*) controltip[2], "Amiga-@");
#endif
                strcpy((char*) controltip[3], "-");
            acase 30:
                strcpy((char*) controltip[0], LLL(MSG_TAPEDECK, "Tape deck"));
#ifdef WIN32
                strcpy((char*) controltip[2], "Alt,T,T");
#endif
#ifdef AMIGA
                strcpy((char*) controltip[2], "Amiga-Shift-D");
#endif
                strcpy((char*) controltip[3], "-");
            acase 31:
                strcpy((char*) controltip[0], LLL(MSG_OPCODESHELP, "Opcodes help"));
#ifdef WIN32
                strcpy((char*) controltip[2], "Alt,H,O");
#endif
#ifdef AMIGA
                strcpy((char*) controltip[2], "Amiga-Shift-U");
#endif
                strcpy((char*) controltip[3], "-");
        }   }
        elif (machine == PONG)
        {   switch (localkey)
            {
            case  4:
                strcpy((char*) controltip[0], LLL(MSG_ANGLES       , "Angles"));
#ifdef WIN32
                strcpy((char*) controltip[2], "Ctrl+Shift+2");
#endif
#ifdef AMIGA
                strcpy((char*) controltip[2], "Amiga-@");
#endif
            acase 5:
                strcpy((char*) controltip[0], LLL(MSG_SPEED_MENU   , "Speed"));
#ifdef WIN32
                strcpy((char*) controltip[2], "Ctrl+Shift+2");
#endif
#ifdef AMIGA
                strcpy((char*) controltip[2], "Amiga-@");
#endif
            acase 6:
                strcpy((char*) controltip[0], LLL(MSG_SERVING      , "Serving"));
#ifdef WIN32
                strcpy((char*) controltip[2], "Ctrl+Shift+2");
#endif
#ifdef AMIGA
                strcpy((char*) controltip[2], "Amiga-@");
#endif
            acase 8:
                strcpy((char*) controltip[0], LLL(MSG_LOCKHORIZAXIS, "Lock horizontal axis?"));
#ifdef WIN32
                strcpy((char*) controltip[2], "Ctrl+Shift+2");
#endif
#ifdef AMIGA
                strcpy((char*) controltip[2], "Amiga-@");
#endif
            acase 9:
                strcpy((char*) controltip[0], LLL(MSG_SOUND_MENU, "Sound?"));
#ifdef WIN32
                strcpy((char*) controltip[2], "Ctrl+U");
#endif
#ifdef AMIGA
                strcpy((char*) controltip[2], "Amiga-U");
#endif
            acase 10:
                strcpy((char*) controltip[0], LLL(MSG_CONTROLLERS_SWAPPED2, "Controllers swapped?"));
#ifdef WIN32
                strcpy((char*) controltip[2], "Ctrl+J");
#endif
#ifdef AMIGA
                strcpy((char*) controltip[2], "Amiga-J");
#endif
            }
            strcpy((char*) controltip[3], "-");
        } elif
        (   ((memmap == MEMMAP_ASTROWARS   || memmap == MEMMAP_GALAXIA ) && localkey ==  5)
         || ((memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN) && localkey == 10)
        )
        {   strcpy((char*) controltip[0], LLL(MSG_INSTRUCTIONS, "Instructions"));
#ifdef WIN32
            strcpy((char*) controltip[2], "Ctrl+Shift+7");
#endif
#ifdef AMIGA
            strcpy((char*) controltip[2], "Amiga-&");
#endif
            strcpy((char*) controltip[3], "-");
        }
        strcpy((char*) controltip[1], (const char*) controltip[0]);
    } else // non-console keys/dirs
    {   if (keytable[thekeypad].overlay[whichplayer] == -1)
        {   strcpy((char*) controltip[0], machines[machine].keynames[whichplayer][thekeypad]);
            strcpy((char*) controltip[1], machines[machine].keynames[whichplayer][thekeypad]);
        } else
        {   strcpy((char*) controltip[0], overlays[whichoverlay][keytable[thekeypad].overlay[whichplayer]]);
            strcpy((char*) controltip[1], overlays[memmapinfo[memmap].overlay][keytable[thekeypad].overlay[whichplayer]]);
        }
        
        if (machine == BINBUG)
        {   strcpy((char*) controltip[2], (const char*) controltip[0]);
        } else
        {   strcpy((char*) controltip[2], keyname[keypads[whichplayer][thekeypad]].name);
        }

        if
        (   thekeypad == GUESTKEY_UP
         || thekeypad == GUESTKEY_DN
         || thekeypad == GUESTKEY_LT
         || thekeypad == GUESTKEY_RT
        )
        {   strcpy((char*) controltip[3], machines[machine].keynames[whichplayer][thekeypad]);
            strcpy((char*) controltip[4], "-");
            return;
        } // implied else
        if (thekeypad == GUESTKEY_UPLT)
        {   strcpy((char*) controltip[3], "Up+Lt");
            strcpy((char*) controltip[4], "-");
            return;
        } // implied else
        if (thekeypad == GUESTKEY_UPRT)
        {   strcpy((char*) controltip[3], "Up+Rt");
            strcpy((char*) controltip[4], "-");
            return;
        } // implied else
        if (thekeypad == GUESTKEY_DNLT)
        {   strcpy((char*) controltip[3], "Dn+Lt");
            strcpy((char*) controltip[4], "-");
            return;
        } // implied else
        if (thekeypad == GUESTKEY_DNRT)
        {   strcpy((char*) controltip[3], "Dn+Rt");
            strcpy((char*) controltip[4], "-");
            return;
        } // implied else
        if
        (   key1 != thekeypad
         && key2 != thekeypad
         && key3 != thekeypad
         && key4 != thekeypad
        )
        {   strcpy((char*) controltip[3], "-");
            strcpy((char*) controltip[4], "-");
            return;
        }

        if (key1 == thekeypad)
        {   strcat((char*) controltip[2], ", ");
            strcat((char*) controltip[2], keyname[keypads[whichplayer][ 0]].name);
        }
        if (key2 == thekeypad)
        {   strcat((char*) controltip[2], ", ");
            strcat((char*) controltip[2], keyname[keypads[whichplayer][21]].name);
        }
        if (key3 == thekeypad)
        {   strcat((char*) controltip[2], ", ");
            strcat((char*) controltip[2], keyname[keypads[whichplayer][22]].name);
        }
        if (key4 == thekeypad)
        {   strcat((char*) controltip[2], ", ");
            strcat((char*) controltip[2], keyname[keypads[whichplayer][23]].name);
        }

        controltip[3][0] = EOS;
        done = FALSE;
        if (key1 == thekeypad)
        {   done = TRUE;
#ifdef WIN32
            sprintf(controltip[3], "%d", button[whichplayer][0]);
#endif
#ifdef AMIGA
            strcpy((char*) controltip[3], LLL(MSG_LABEL_RED, "Red"));
#endif
        }
        if (key2 == thekeypad)
        {   if (done)
            {   strcat((char*) controltip[3], ", ");
            }
            done = TRUE;
#ifdef WIN32
            sprintf(ENDOF(controltip[3]), "%d", button[whichplayer][1]);
#endif
#ifdef AMIGA
            strcat((char*) controltip[3], LLL(MSG_LABEL_BLUE, "Blue"));
#endif
        }
        if (key3 == thekeypad)
        {   if (done)
            {   strcat((char*) controltip[3], ", ");
            }
            done = TRUE;
#ifdef WIN32
            sprintf(ENDOF(controltip[3]), "%d", button[whichplayer][2]);
#endif
#ifdef AMIGA
            strcat((char*) controltip[3], LLL(MSG_LABEL_GREEN, "Green"));
#endif
        }
        if (key4 == thekeypad)
        {   if (done)
            {   strcat((char*) controltip[3], ", ");
            }
            // done = TRUE;
#ifdef WIN32
            sprintf(ENDOF(controltip[3]), "%d", button[whichplayer][3]);
#endif
#ifdef AMIGA
            strcat((char*) controltip[3], LLL(MSG_COLOUR2_YELLOW, "Yellow"));
#endif
        }

        if
        (   key1 != thekeypad
         && key2 != thekeypad
         && key3 != thekeypad
        )
        {   strcpy((char*) controltip[4], "-");
            return;
        }

        controltip[4][0] = EOS;
        done = FALSE;
        if (key1 == thekeypad)
        {   done = TRUE;
            strcpy((char*) controltip[4], LLL(MSG_LEFT, "Left"));
        }
        if (key2 == thekeypad)
        {   if (done)
            {   strcat((char*) controltip[4], ", ");
            }
            done = TRUE;
            sprintf((char*) ENDOF(controltip[4]), LLL(MSG_MIDDLE, "Middle"));
        }
        if (key3 == thekeypad)
        {   if (done)
            {   strcat((char*) controltip[4], ", ");
            }
            // done = TRUE;
            sprintf((char*) ENDOF(controltip[4]), LLL(MSG_RIGHT, "Right"));
}   }   }

EXPORT FLAG wantellipse(int localkey)
{   if
    (   machine == BINBUG
     || machine == MALZAK
     || (   machine == PIPBUG
         && (   (pipbug_vdu == VDU_VT100                                    && localkey == 78)
             || (pipbug_vdu != VDU_VT100 && pipbug_vdu != VDU_RADIOBULLETIN && localkey == 61)
        )   )
     || (machine == PONG && whichkeyrect == 6 && (localkey == 0 || localkey == 2))
     || ((memmap == MEMMAP_ASTROWARS   || memmap == MEMMAP_GALAXIA )                                   && localkey !=  5)
     || ((memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN) && (localkey < 3 || localkey > 6) && localkey != 10)
    )
    {   return TRUE;
    } else
    {   return FALSE;
}   }

EXPORT FLAG load_disk(FLAG wantasl, int whichdrive, FLAG user)
{   FLAG rc;

    switch (machine)
    {
    case  TWIN:   rc = twin_load_disk(  wantasl, whichdrive);
    acase BINBUG: rc = binbug_load_disk(wantasl, whichdrive);
    acase CD2650: rc = cd2650_load_disk(wantasl, whichdrive);
    adefault:     return FALSE;
    }

    if (user)
    {   ; // play_ambient_sample(SAMPLE_INSERTDISK);
    }

    return rc;
}

EXPORT void dir_disk(FLAG quiet, int whichdrive)
{   switch (machine)
    {
    case  TWIN:   twin_dir_disk(  quiet, whichdrive);
    acase BINBUG: binbug_dir_disk(quiet, whichdrive);
    acase CD2650: cd2650_dir_disk(quiet, whichdrive);
}   }

EXPORT void update_disk(int whichdrive)
{   FILE* DiskHandle /* = NULL */ ;
    int   thekind;

    if (machine == BINBUG || machine == CD2650)
    {   thekind = KIND_RAW;
    } else
    {   // assert(machine == TWIN);
        thekind = KIND_IMG;
    }

    if (drive[whichdrive].fn_disk[0] == EOS)
    {   if (game)
        {   strcpy((char*) drive[whichdrive].fn_disk, (const char*) file_game);
        }
        fixextension(filekind[thekind].extension, (STRPTR) drive[whichdrive].fn_disk, TRUE, ""); // so we have extension in ASL requester

        if (asl
        (   filekind[thekind].hail,
            filekind[thekind].pattern,
            TRUE,
            (STRPTR) path_disks,
            (STRPTR) drive[whichdrive].fn_disk, // this fortunately doesn't contain the path right now
            (STRPTR) drive[whichdrive].fn_disk
        ) != EXIT_SUCCESS)
        {   drive[whichdrive].fn_disk[0] = EOS;
            return;
        }
        fixextension(filekind[thekind].extension, (STRPTR) drive[whichdrive].fn_disk, TRUE, ""); // so we have extension even if user overtypes it in ASL requester
/* #ifdef SETPATHS
        break_pathname(drive[whichdrive].fn_disk, path_disks, NULL);
        audit(FALSE); // assuming path_disks == path_games
#endif */
    } else
    {   fixextension(filekind[thekind].extension, (STRPTR) drive[whichdrive].fn_disk, TRUE, ""); // so eg. .TWIN changes to .IMG
    }

    if (!(DiskHandle = fopen((const char*) drive[whichdrive].fn_disk, "wb")))
    {   say("Can't open file for output!");
        return;
    }
    if (fwrite(drive[whichdrive].contents, machines[machine].disksize, 1, DiskHandle) != 1)
    {   say("Can't write to file!");
    }

    DISCARD fclose(DiskHandle);
    // DiskHandle = NULL;

    update_floppydrive(TRUE, whichdrive);
}

EXPORT FLAG enableclearhs(void)
{   int i;

    for (i = 0; i < machines[machine].hiscores; i++)
    {
#ifdef TOURNAMENTHISCORES
        if
        (   hiscore[machines[machine].firsthiscore + i].played
         || hiscore[machines[machine].firsthiscore + i].playedmicro
        )
        {   return TRUE;
        } // implied else
        if   (machine == ARCADIA && i ==  4) if (hiscore[ 4].score !=  8200) return TRUE; // Astro Invader
        elif (machine == ARCADIA && i == 10) if (hiscore[10].score != 11300) return TRUE; // Cat Trax
        elif (machine == ARCADIA && i == 16) if (hiscore[16].score !=  1240) return TRUE; // Escape
        elif (machine == ARCADIA && i == 23) if (hiscore[23].score !=  1740) return TRUE; // Jump Bug
        elif (machine == ARCADIA && i == 28) if (hiscore[28].score !=  4700) return TRUE; // Nibblemen
        elif (machine == ARCADIA && i == 40) if (hiscore[40].score !=   320) return TRUE; // Space Squadron
        else
#endif
        {   if (hiscore[machines[machine].firsthiscore + i].score)
            {   return TRUE;
    }   }   }

    return FALSE;
}

EXPORT void clearhs(void)
{   int i;

    for (i = 0; i < machines[machine].hiscores; i++)
    {   hiscore[machines[machine].firsthiscore + i].score          =
        hiscore[machines[machine].firsthiscore + i].played         =
        hiscore[machines[machine].firsthiscore + i].playedmicro    = 0;
        hiscore[machines[machine].firsthiscore + i].thedatetime[0] =
        hiscore[machines[machine].firsthiscore + i].username[0]    = '-';
        hiscore[machines[machine].firsthiscore + i].thedatetime[1] =
        hiscore[machines[machine].firsthiscore + i].username[1]    = EOS;
    }
#ifdef TOURNAMENTHISCORES
    if (machine == ARCADIA)
    {          hiscore[known[ASTROINVADERPOS ].hiscore].score =  8200;
        strcpy(hiscore[known[ASTROINVADERPOS ].hiscore].thedatetime,  "14/8/24");
        strcpy(hiscore[known[ASTROINVADERPOS ].hiscore].username, "Egoitz Campo");

               hiscore[known[CATTRAXPOS      ].hiscore].score = 11300;
        strcpy(hiscore[known[CATTRAXPOS      ].hiscore].thedatetime,  "14/8/24");
        strcpy(hiscore[known[CATTRAXPOS      ].hiscore].username, "Luis Getxo");

               hiscore[known[ESCAPEPOS       ].hiscore].score =  1240;
        strcpy(hiscore[known[ESCAPEPOS       ].hiscore].thedatetime,  "14/8/24");
        strcpy(hiscore[known[ESCAPEPOS       ].hiscore].username, "Samuel Otero");

               hiscore[known[JUMPBUGPOS1     ].hiscore].score =  1740;
        strcpy(hiscore[known[JUMPBUGPOS1     ].hiscore].thedatetime,  "14/8/24");
        strcpy(hiscore[known[JUMPBUGPOS1     ].hiscore].username, "Egoitz Campo");

               hiscore[known[NIBBLEMENPOS    ].hiscore].score =  4700;
        strcpy(hiscore[known[NIBBLEMENPOS    ].hiscore].thedatetime, "10/12/22");
        strcpy(hiscore[known[NIBBLEMENPOS    ].hiscore].username, "Jesus Carballeira");

               hiscore[known[SPACESQUADRONPOS].hiscore].score =   320;
        strcpy(hiscore[known[SPACESQUADRONPOS].hiscore].thedatetime,  "26/8/21");
        strcpy(hiscore[known[SPACESQUADRONPOS].hiscore].username, "Egoitz Campo");
    }
#endif
}

EXPORT void longcomma(ULONG value, STRPTR out)
{   ULONG number,
          thevalue = value,
          i;
    UBYTE calc;
    FLAG  started  = FALSE;

    /* With 32 bits the maximum possible is 4 gigabytes
       (about 4 billion bytes).

       " ,   ,   ,   "
        01234567890123

       Note that it never touches *(out + 13). */

    number = ONE_BILLION;
    for (i = 0; i <= 12; i++)
    {   if (i == 1 || (i >= 5 && ((i - 1) % 4) == 0))
        {   if (started)
            {   *(out + i) = groupsep;
            } else
            {   *(out + i) = ' ';
        }   }
        else
        {   if (thevalue >= number)
            {   calc = (UBYTE) (thevalue / number); // calc = (UBYTE) thevalue / number; was wrong!
            } else
            {   calc = 0;
            }
            if (calc || started || i == 12)
            {   *(out + i) = (char) ('0' + calc);
                started = TRUE;
                thevalue %= number;
            } else
            {   *(out + i) = ' ';
            }
            number /= 10;
}   }   }

EXPORT void fix_keyrects(void)
{   switch (machine)
    {
    case ARCADIA:
        if (arcadia_bigctrls)
        {   whichkeyrect = KEYRECT_FIRST_ARCADIA + arcadia_viewcontrolsas + 3;
#ifdef WIN32
            hostmachines[machine].controls = (arcadia_viewcontrolsas == 1) ? IDD_CONTROLS_MPT03_BIG : IDD_CONTROLS_ARCADIA_BIG;
#endif
        } else
        {   whichkeyrect = KEYRECT_FIRST_ARCADIA + arcadia_viewcontrolsas;
#ifdef WIN32
            hostmachines[machine].controls = (arcadia_viewcontrolsas == 1) ? IDD_CONTROLS_MPT03_SML : IDD_CONTROLS_ARCADIA_SML;
#endif
        }
    acase INTERTON:
        if (interton_bigctrls)
        {   whichkeyrect = KEYRECT_INTERTON_BIG;
#ifdef WIN32
            hostmachines[machine].controls = IDD_CONTROLS_INTERTON_BIG;
#endif
        } else
        {   whichkeyrect = KEYRECT_INTERTON_SML;
#ifdef WIN32
            hostmachines[machine].controls = IDD_CONTROLS_INTERTON_SML;
#endif
        }
    acase ELEKTOR:
        if (elektor_bigctrls)
        {   whichkeyrect = KEYRECT_ELEKTOR_BIG;
#ifdef WIN32
            hostmachines[machine].controls = IDD_CONTROLS_ELEKTOR_BIG;
#endif
        } else
        {   whichkeyrect = KEYRECT_ELEKTOR_SML;
#ifdef WIN32
            hostmachines[machine].controls = IDD_CONTROLS_ELEKTOR_SML;
#endif
        }
    acase PIPBUG:
        switch (pipbug_vdu)
        {
        case VDU_RADIOBULLETIN:
            whichkeyrect = KEYRECT_RADIOBULLETIN;
#ifdef WIN32
            hostmachines[machine].controls = IDD_CONTROLS_RADIOBULLETIN;
#endif
        acase VDU_VT100:
            whichkeyrect = KEYRECT_VT100;
#ifdef WIN32
            hostmachines[machine].controls = IDD_CONTROLS_VT100;
#endif
        adefault:
            whichkeyrect = KEYRECT_ELEKTERMINAL;
#ifdef WIN32
            hostmachines[machine].controls = IDD_CONTROLS_ELEKTERMINAL;
#endif
        }
    acase INSTRUCTOR:
        if (si50_bigctrls)
        {   whichkeyrect = KEYRECT_SI50_BIG;
#ifdef WIN32
            hostmachines[machine].controls = IDD_CONTROLS_SI50_BIG;
#endif
        } else
        {   whichkeyrect = KEYRECT_SI50_SML;
#ifdef WIN32
            hostmachines[machine].controls = IDD_CONTROLS_SI50_SML;
#endif
        }
    acase MIKIT:
        if (mikit_bigctrls)
        {   whichkeyrect = KEYRECT_MIKIT_BIG;
#ifdef WIN32
            hostmachines[machine].controls = IDD_CONTROLS_MIKIT_BIG;
#endif
        } else
        {   whichkeyrect = KEYRECT_MIKIT_SML;
#ifdef WIN32
            hostmachines[machine].controls = IDD_CONTROLS_MIKIT_SML;
#endif
        }
    acase PONG:
        if (memmap == MEMMAP_8550)
        {   whichkeyrect = KEYRECT_FIRST_8550 + pong8550_viewcontrolsas;
        } else
        {   // assert(memmap == MEMMAP_8600);
            whichkeyrect = KEYRECT_FIRST_8600 + pong8600_viewcontrolsas;
}   }   }

EXPORT void update_waveform(void)
{   FAST int    colour,
                x, y;
    FAST double scaleby,
                xf;
    FAST UBYTE  t, ts;

    ts = 255 - tapeskewage;

    switch (tapemode)
    {
    case  TAPEMODE_NONE:   colour = EMURGBPEN_WHITE;
    acase TAPEMODE_STOP:   colour = EMURGBPEN_GREEN;
    acase TAPEMODE_PLAY:   colour = EMURGBPEN_CYAN;
    acase TAPEMODE_RECORD: colour = tapewriteprotect ? EMURGBPEN_YELLOW : EMURGBPEN_RED;
    }

    for (y = 0; y < WAVEFORMHEIGHT; y++)
    {   for (x = 0; x < WAVEFORMWIDTH; x++)
        {   DRAWWAVE(x, WAVEFORMHEIGHT - 1 - y, colour);
    }   }

    if (tapemode != TAPEMODE_NONE && otl)
    {   // assert(TapeCopy);
        scaleby = (double) otl / (double) WAVEFORMWIDTH;

        for (x = 0; x < WAVEFORMWIDTH; x++)
        {   xf = (double) x * scaleby;
            t = TapeCopy[(int) xf];
            if (tapekind == KIND_8SVX || tapekind == KIND_AIFF)
            {   t ^= 0x80; // convert from signed to unsigned
            }
            if (inverttape)
            {   t ^= 0xFF; // ie. tapebyte = 255 - tapebyte;
            }
            if (t < ts)
            {   for (y = t; y < ts; y += 2)
                {   DRAWWAVE(x, WAVEFORMHEIGHT - 1 - (y / 2), EMURGBPEN_GREY);
            }   }
            elif (t > ts)
            {   for (y = ts + 1; y <= t; y += 2)
                {   DRAWWAVE(x, WAVEFORMHEIGHT - 1 - (y / 2), EMURGBPEN_GREY);
        }   }   }

        xf = samplewhere_f / scaleby;
        x = (int) xf;
        if (x >= WAVEFORMWIDTH)
        {   x = WAVEFORMWIDTH - 1;
        }
        for (y = 0; y < WAVEFORMHEIGHT; y++)
        {   DRAWWAVE(x, y, EMURGBPEN_BLACK);
    }   }

    for (x = 0; x < WAVEFORMWIDTH; x++)
    {   DRAWWAVE(x, WAVEFORMHEIGHT - 1 - (ts / 2), EMURGBPEN_BLACK);
    }

    wpa8(CANVAS_WAVE, 0, 0);
}

EXPORT void update_roll(int whichunit)
{   FAST int   colour,
               x, xx, y, yy;
    FAST UBYTE thechar;
FAST const TEXT arrow[10][10 + 1] = {
"##........",
"####......",
"######....",
"########..",
"##########",
"##########",
"########..",
"######....",
"####......",
"##........",
}, chad[10][10 + 1] = {
"....##....",
"..##--##..",
".#------#.",
".#------#.",
"#--------#",
"#--------#",
".#------#.",
".#------#.",
"..##--##..",
"....##....",
}, sprockethole[6][6 + 1] = {
"..##..",
".####.",
"######",
"######",
".####.",
"..##..",
};

    switch (papertapemode[whichunit])
    {
    case  TAPEMODE_NONE:   colour = EMURGBPEN_WHITE;
    acase TAPEMODE_STOP:   colour = EMURGBPEN_GREEN;
    acase TAPEMODE_PLAY:   colour = EMURGBPEN_CYAN;
    acase TAPEMODE_RECORD: colour = papertapeprotect[whichunit] ? EMURGBPEN_YELLOW : EMURGBPEN_RED;
    }

    for (y = 0; y < ROLLHEIGHT; y++)
    {   for (x = 0; x < ROLLWIDTH; x++)
        {   DRAWROLL(whichunit, x, y, colour);
    }   }
    for (y = 0; y < 9; y++)
    {   if ((int) papertapewhere[whichunit] - 4 + y < 0 || (int) papertapewhere[whichunit] - 4 + y >= (int) papertapelength[whichunit])
        {
#ifdef SHOWEDGES
            for (yy = 0; yy < 20; yy++)
            {   for (xx = 0; xx < ROLLWIDTH; xx++)
                {   DRAWROLL(whichunit, xx, (y * 20) + yy, EMURGBPEN_BLACK);
            }   }
#else
            ;
#endif
        } else
        {   thechar = PapertapeBuffer[whichunit][papertapewhere[whichunit] - 4 + y];
            for (x = 0; x < 8; x++)
            {   if (thechar & (0x80 >> x)) // punched circle
                {   for (yy = 0; yy < 10; yy++)
                    {   for (xx = 0; xx < 10; xx++)
                        {   if (chad[yy][xx] == '#' || chad[yy][xx] == '-')
                            {
#ifdef PAPERTAPESPROCKETS
                                DRAWROLL(whichunit, (x * 20) + ((x >= 5) ? 46 : 26) + xx, (y * 20) + 6 + yy, EMURGBPEN_BLACK);
#else
                                DRAWROLL(whichunit, (x * 20) +                  26  + xx, (y * 20) + 6 + yy, EMURGBPEN_BLACK);
#endif
                }   }   }   }
                else // hollow circle
                {   for (yy = 0; yy < 10; yy++)
                    {   for (xx = 0; xx < 10; xx++)
                        {   if (chad[yy][xx] == '#')
                            {
#ifdef PAPERTAPESPROCKETS
                                DRAWROLL(whichunit, (x * 20) + ((x >= 5) ? 46 : 26) + xx, (y * 20) + 6 + yy, EMURGBPEN_GREY);
#else
                                DRAWROLL(whichunit, (x * 20) +                  26  + xx, (y * 20) + 6 + yy, EMURGBPEN_GREY);
#endif
            }   }   }   }   }

            thechar &= 0x7F;
            if (thechar < ' ' || thechar > 'z')
            {   thechar = 0x3B; // solid block
            } else
            {   thechar -= 0x20;
            }
            for (yy = 0; yy < 7; yy++)
            {   for (xx = 0; xx < 5; xx++)
                {   if (tr_chars[thechar][yy] & (0x80 >> xx))
                    {
#ifdef PAPERTAPESPROCKETS
                        DRAWROLL(whichunit, 208 + (xx * 2), (y * 20) + 4 + (yy * 2), EMURGBPEN_BLACK);
                        DRAWROLL(whichunit, 209 + (xx * 2), (y * 20) + 4 + (yy * 2), EMURGBPEN_BLACK);
                        DRAWROLL(whichunit, 208 + (xx * 2), (y * 20) + 5 + (yy * 2), EMURGBPEN_BLACK);
                        DRAWROLL(whichunit, 209 + (xx * 2), (y * 20) + 5 + (yy * 2), EMURGBPEN_BLACK);
#else
                        DRAWROLL(whichunit, 188 + (xx * 2), (y * 20) + 4 + (yy * 2), EMURGBPEN_BLACK);
                        DRAWROLL(whichunit, 189 + (xx * 2), (y * 20) + 4 + (yy * 2), EMURGBPEN_BLACK);
                        DRAWROLL(whichunit, 188 + (xx * 2), (y * 20) + 5 + (yy * 2), EMURGBPEN_BLACK);
                        DRAWROLL(whichunit, 189 + (xx * 2), (y * 20) + 5 + (yy * 2), EMURGBPEN_BLACK);
#endif
            }   }   }

#ifdef PAPERTAPESPROCKETS
            for (yy = 0; yy < 6; yy++)
            {   for (xx = 0; xx < 6; xx++)
                {   if (sprockethole[yy][xx] == '#')
                    {   DRAWROLL(whichunit, 128 + xx, (y * 20) + 8 + yy, EMURGBPEN_BLACK);
            }   }   }
#endif
        }

#ifdef SHOWEDGES
        colour = (papertapewhere[whichunit] >= 0 && papertapewhere[whichunit] < papertapelength[whichunit]) ? EMURGBPEN_BLACK : EMURGBPEN_WHITE;
#else
        colour = EMURGBPEN_BLACK;
#endif
        for (yy = 0; yy < 10; yy++)
        {   for (xx = 0; xx < 10; xx++)
            {   if (arrow[yy][xx] == '#')
                {   DRAWROLL(whichunit, 8 + xx, 86 + yy, colour);
    }   }   }   }

#ifndef SHOWEDGES
    if (papertapewhere[whichunit] < 4)
    {   for (x = 0; x < ROLLWIDTH; x += 2)
        {   DRAWROLL(whichunit, x, ((                               4 - (int) papertapewhere[whichunit]    ) * 20) + 1, EMURGBPEN_BLACK);
    }   }
    if (papertapewhere[whichunit] + 4 >= papertapelength[whichunit])
    {   for (x = 0; x < ROLLWIDTH; x += 2)
        {   DRAWROLL(whichunit, x, (((int) papertapelength[whichunit] - (int) papertapewhere[whichunit] + 4) * 20) + 1, EMURGBPEN_BLACK);
    }   }
#endif

    redraw_roll(whichunit);
}

#ifdef WIN32
EXPORT void handle_keydown(UINT code)
#endif
#ifdef AMIGA
EXPORT void handle_keydown(UWORD code)
#endif
{   UWORD thekey; // usually but not always an ASCII character
    int   i,
          length;

#ifdef WIN32
    if (incli)
    {   return;
    } // implied else
    if (cheevos)
    {
#ifdef LOGCHEEVOS
        zprintf(TEXTPEN_VERBOSE, "RA_IsOverlayFullyVisible()\n");
#endif
        if (RA_IsOverlayFullyVisible())
        {   if (code == SCAN_ESCAPE)
            {   code = SCAN_PAUSE;
            } elif (code != SCAN_PAUSE)
            {   return;
    }   }   }
    code = scan_to_scan(code);
#endif
#ifdef AMIGA
    if
    (   showsidebars[wsm]
     && foundgames
     && !crippled
     && ctrl()
    )
    {   switch (code)
        {
        case NM_WHEEL_UP:
            pending |= PENDING_SIDEBARUP;
            return;
        acase NM_WHEEL_DOWN:
            pending |= PENDING_SIDEBARDOWN;
            return;
    }   }
#endif

    if (code == SCAN_CAPSLOCK)
    {
#ifdef WIN32
        if (!capslockdown)
        {   capslockdown = TRUE;
            capslock = capslock ? FALSE : TRUE;
        }
#endif
#ifdef AMIGA
        KeyMatrix[code / 8] |= (1 << (code % 8));
        if (morphos)
        {   capslock = capslock ? FALSE : TRUE;
        } else
        {   capslock = TRUE;
        }
#endif
        update_capslock();
        return;
    }

    if (editscreen && !paused && edit_screen((UWORD) code))
    {   return;
    }

#ifdef AMIGA
    if (subwin[SUBWINDOW_HOSTKYBD].hwnd)
    {   SetGadgetAttrs(gadgets[GID_FIRSTKYBDGAD + (code & 0x7F)], subwin[SUBWINDOW_HOSTKYBD].hwnd, NULL, GA_Selected, TRUE, TAG_DONE); // this autorefreshes
    }
#endif

    if
    (   ISQWERTY
     && !paused
     && recmode != RECMODE_PLAY
     && queuepos < QUEUESIZE
#ifdef AMIGA
     && !alt()
#endif
    )
    {   KeyMatrix[code / 8] |= (1 << (code % 8));
        keyframes[code]++;

        if (machine == TYPERIGHT)
        {   if (ctrl())
            {   thekey = NC;
            } elif (shift())
            {   if (code == SCAN_TAB && showdebugger[wsm])
                {   debugger_enter();
                    return;
                } // implied else
                thekey = (keymap == KEYMAP_HOST) ? keyname[code].shifted[  KEYMAP_HOST] : keyname[code].tr_shifted;
            } elif (capslock)
            {   thekey = (keymap == KEYMAP_HOST) ? keyname[code].capslock[ KEYMAP_HOST] : keyname[code].tr_shifted;
            } else
            {   thekey = (keymap == KEYMAP_HOST) ? keyname[code].unshifted[KEYMAP_HOST] : keyname[code].tr_unshifted;
            }
            if (thekey == CR)
            {   thekey = LF;
            } elif (thekey == '^')
            {   thekey = '-'; // Type-right has no concept of '^'
        }   }
        elif
        (   ctrl()
#ifdef AMIGA
         && code != SCAN_UP
         && code != SCAN_DOWN
         && code != SCAN_LEFT
         && code != SCAN_RIGHT
#endif
        )
        {   if (shift())
            {   thekey = keyname[code].ctrlshift;
            } else
            {   thekey = keyname[code].ctrl;
        }   }
        elif (shift())
        {   if (code == SCAN_TAB && showdebugger[wsm])
            {   debugger_enter();
                return;
            } // implied else
            thekey =  specialshift(keyname[code].shifted[keymap]);
        } elif (capslock || machine == TWIN || !lowercase || (machine == CD2650 && cd2650_biosver != CD2650_IPL))
        {   thekey =  keyname[code].capslock[keymap];
        } elif (machine == CD2650)
        {   thekey =  keyname[code].capslock[keymap] & 0x3F;
        } else
        {   thekey =  keyname[code].unshifted[keymap];
        }

#ifdef AMIGA
        if (machine == PIPBUG && pipbug_vdu == VDU_ELEKTERMINAL)
        {   switch (code)
            {
            case SCAN_PGUP: elekterminal_scroll(-16); return; // possible on OS4/MOS
            case SCAN_PGDN: elekterminal_scroll( 16); return; // possible on OS4/MOS
            case SCAN_UP:   elekterminal_scroll( -1); return;
            case SCAN_DOWN: elekterminal_scroll(  1); return;
        }   }
#endif

        switch (thekey)
        {
        case HI:
            if (machine != TYPERIGHT && !queuekeystrokes)
            {   queuekeystrokes = TRUE;
                updatemenu(MENUITEM_QUEUEKEYSTROKES);
            }
            length = strlen(hostmachines[machine].titlebartext);
            for (i = 0; i < length; i++)
            {   if (machine != TWIN && lowercase)
                {   thequeue[queuepos++] = hostmachines[machine].titlebartext[i];
                } else
                {   thequeue[queuepos++] = toupper(hostmachines[machine].titlebartext[i]);
            }   }
            thequeue[queuepos++] = CR;
        return;
        case DATE:
            if (machine != TYPERIGHT && !queuekeystrokes)
            {   queuekeystrokes = TRUE;
                updatemenu(MENUITEM_QUEUEKEYSTROKES);
            }
            read_rtc();
            thequeue[queuepos++] = '0' + ( rtc.month        / 10); // MM-DD-YY format
            thequeue[queuepos++] = '0' + ( rtc.month        % 10);
            thequeue[queuepos++] = '-';
            thequeue[queuepos++] = '0' + ( rtc.day          / 10);
            thequeue[queuepos++] = '0' + ( rtc.day          % 10);
            thequeue[queuepos++] = '-';
            thequeue[queuepos++] = '0' + ((rtc.year  % 100) / 10);
            thequeue[queuepos++] = '0' + ((rtc.year  % 100) % 10);
            thequeue[queuepos++] = CR;
        return;
        case BRK:
        return;
        case NC:
            ;
        acase ESC:
            if (machine != TYPERIGHT)
            {   enqueue((UWORD) code, (TEXT) thekey);
                return;
            }
        adefault:
            enqueue((UWORD) code, (TEXT) thekey);
            return;
    }   }

#ifdef WIN32
    if
    (   (!ctrl() && !shift())
     || code == SCAN_PGUP
     || code == SCAN_PGDN
     || code == SCAN_HOME
     || code == SCAN_END
     || code == SCAN_APOSTROPHE
     || code == SCAN_LCTRL || code == SCAN_RCTRL
     || (machine == PONG           && !ctrl() && shift() && code == console[1]) // special casing for Shift+guest "A"
     || (memmap  == MEMMAP_MALZAK2 && !ctrl() && shift() && code == console[3]) // special casing for Shift+guest "RESET"
    )
    {   KeyMatrix[code / 8] |= (1 << (code % 8));
    }
    storedcode = code;
#endif
#ifdef AMIGA
    if
    (   (!alt() && (machine == TWIN || !ctrl()))
     || (code & 0x7F) == SCAN_CTRL
     || (code & 0x7F) == SCAN_LSHIFT
     || (code & 0x7F) == SCAN_RSHIFT
     || (code & 0x7F) == SCAN_LALT
     || (code & 0x7F) == SCAN_RALT
    )
    {   if (code < KEYUP)
        {   KeyMatrix[code          / 8] |=  (1 << (code % 8));
            keyframes[code]++;
        } else
        {   KeyMatrix[(code & 0x7F) / 8] &= ~(1 << (code % 8));
            keyframes[code & 0x7F] = 0;
            if (!queuekeystrokes && code == hostkey)
            {   guestkey = NC;
                hostkey  = (UWORD) -1;
    }   }   }
#endif

    if (subwin[SUBWINDOW_HOSTKYBD].hwnd)
    {   updatekeynames(subwin[SUBWINDOW_HOSTKYBD].hwnd);
    }

#ifdef AMIGA
    switch (code)
    {
    case SCAN_ESCAPE:
        if (confirm())
        {   if (crippled)
            {   cleanexit(EXIT_SUCCESS);
            } else
            {   pending |= PENDING_QUIT;
        }   }
    acase SCAN_APOSTROPHE:
        KeyMatrix[SCAN_APOSTROPHE / 8] |= (1 << (SCAN_APOSTROPHE % 8)); // needed for calc_wait()
    acase SCAN_P: // paused
        if (paused)
        {   emu_unpause();
        } elif (machine != TYPERIGHT)
        {   emu_pause();
        }
    acase SCAN_PLAY:
        if (paused)
        {   emu_unpause();
        } else
        {   emu_pause();
        }
    acase SCAN_F1:
        if (ctrl())
        {   if (shift())
            {   change_machine(MIKIT,      MEMMAP_MIKIT      , TRUE);
            } else
            {   if (size != 1) pending |= PENDING_1XSIZE;
        }   }
    acase SCAN_F2:
        if (ctrl())
        {   if (shift())
            {   change_machine(ZACCARIA,   MEMMAP_ASTROWARS  , TRUE);
            } else
            {   if (size != 2) pending |= PENDING_2XSIZE;
        }   }
    acase SCAN_F3:
        if (ctrl())
        {   if (shift())
            {   change_machine(ZACCARIA,   MEMMAP_GALAXIA    , TRUE);
            } else
            {   if (size != 3) pending |= PENDING_3XSIZE;
        }   }
    acase SCAN_F4:
        if (ctrl())
        {   if (shift())
            {   change_machine(ZACCARIA,   MEMMAP_LASERBATTLE, TRUE);
            } else
            {   if (size != 4) pending |= PENDING_4XSIZE;
        }   }
    acase SCAN_F5:  // reset
        if (ctrl())
        {   if (shift())
            {   change_machine(MALZAK,     MEMMAP_LAZARIAN   , TRUE);
            } else
            {   if (size != 5) pending |= PENDING_5XSIZE;
        }   }
        elif (!crippled)
        {   project_reset(FALSE);
        }
    acase SCAN_F6:
        if (ctrl())
        {   if (shift())
            {   change_machine(MALZAK,     MEMMAP_MALZAK1    , TRUE);
            } else
            {   if (size != 6) pending |= PENDING_6XSIZE;
        }   }
    acase SCAN_F7:
        if (ctrl() && shift())
        {   change_machine(    MALZAK,     MEMMAP_MALZAK2    , TRUE);
        }
    acase SCAN_F8:
        if (ctrl() && shift())
        {   change_machine(    PONG,       MEMMAP_8550       , TRUE);
        }
    acase SCAN_F9:
        if (ctrl() && shift())
        {   change_machine(    PONG,       MEMMAP_8600       , TRUE);
        }
    acase SCAN_F10:
        if (ctrl() && shift())
        {   change_machine(    TYPERIGHT,  MEMMAP_TYPERIGHT  , TRUE);
        }
    acase SCAN_F11:
        if (!ctrl() && !shift())
        {   fullscreen = fullscreen ? FALSE : TRUE;
            docommand(MENUITEM_FULLSCREEN);
        }
    acase SCAN_FFWD:
        turbo = turbo ? FALSE : TRUE;
        docommand(MENUITEM_TURBO);
    acase SCAN_UP:
    case SCAN_LEFT:
        if
        (   ctrl()
         && showsidebars[wsm]
         && foundgames
         && !crippled
         && ((struct IntuitionBase*) IntuitionBase)->ActiveWindow == MainWindowPtr // we should probably Lock/UnlockIBase()
        )
        {   pending |= PENDING_SIDEBARUP;
        }
    acase SCAN_PREV:
        if
        (   showsidebars[wsm]
         && foundgames
         && !crippled
        )
        {   pending |= PENDING_SIDEBARUP;
        }
    acase SCAN_DOWN:
    case SCAN_RIGHT:
        if
        (   ctrl()
         && showsidebars[wsm]
         && foundgames
         && !crippled
         && ((struct IntuitionBase*) IntuitionBase)->ActiveWindow == MainWindowPtr // we should probably Lock/UnlockIBase()
        )
        {   pending |= PENDING_SIDEBARDOWN;
        }
    acase SCAN_NEXT:
        if
        (   showsidebars[wsm]
         && foundgames
         && !crippled
        )
        {   pending |= PENDING_SIDEBARDOWN;
        }
    acase SCAN_STOP:
        macro_stop();
    acase SCAN_RWND:
        macro_restartplayback();
#ifdef PROFILE
    acase SCAN_SLASH:
        printprofile();
#endif
    acase SCAN_MI:
        if (ctrl() && !shift() && speedup > SPEED_MIN)
        {   speedup--;
            update_speed();
        }
    acase SCAN_PL:
        if (ctrl() && !shift() && speedup < SPEED_MAX)
        {   speedup++;
            update_speed();
        }
    acase SCAN_TAB:
        if (!ISQWERTY || shift())
        {   if (showdebugger[wsm])
            {   ActivateWindow(MainWindowPtr);
                ActivateLayoutGadget(gadgets[GID_MA_LY1], MainWindowPtr, NULL, (Object) gadgets[GID_MA_ST4]);
        }   }
    acase SCAN_C:
        if (alt())
        {   ax[0] = ax[1] = ay[0] = ay[1] = ay[2] = ay[3] = machines[machine].digipos[1];
            do_axes();
        }
    acase SCAN_J:
        if (alt())
        {   swapped = swapped ? FALSE : TRUE;
            docommand(MENUITEM_SWAPPED);
        }
    acase SCAN_L:
        if (alt())
        {   docommand(MENUITEM_QUICKLOAD);
        }
    acase SCAN_S:
        if (alt())
        {   docommand(MENUITEM_QUICKSAVE);
        }
    acase SCAN_R:
        if (alt())
        {   project_reset(FALSE);
        }
    acase SCAN_W:
        if (alt())
        {   turbo = turbo ? FALSE : TRUE;
            docommand(MENUITEM_TURBO);
    }   }
#endif
}

#ifdef WIN32
EXPORT void handle_keyup(UINT code)
#endif
#ifdef AMIGA
EXPORT void handle_keyup(UWORD code)
#endif
{   code = scan_to_scan((int) code);

    KeyMatrix[code / 8] &= (255 - (1 << (code % 8)));
    keyframes[code] = 0;

#ifdef AMIGA
    if (code == SCAN_CAPSLOCK && !morphos)
    {   capslock = FALSE;
        update_capslock();
        return;
    }
#endif
#ifdef WIN32
    if (code == SCAN_CAPSLOCK)
    {   capslockdown = FALSE;
    }
#endif

    if
    (   !queuekeystrokes
     && (UWORD) code == hostkey
#ifdef AMIGA
     && !alt()
#endif
    )
    {   guestkey = NC;
        hostkey  = (UWORD) -1;
    }

#ifdef AMIGA
    if (subwin[SUBWINDOW_HOSTKYBD].hwnd)
    {   SetGadgetAttrs(gadgets[GID_FIRSTKYBDGAD + (code & 0x7F)], subwin[SUBWINDOW_HOSTKYBD].hwnd, NULL, GA_Selected, FALSE, TAG_DONE); // this autorefreshes
    }
#endif

    if (subwin[SUBWINDOW_HOSTKYBD].hwnd)
    {   updatekeynames(subwin[SUBWINDOW_HOSTKYBD].hwnd);
}   }

#ifdef AMIGA
MODULE FLAG alt(void)
{   if
    (   KeyDown(SCAN_LALT)
     || KeyDown(SCAN_RALT)
    )
    {   return TRUE;
    } else
    {   return FALSE;
}   }
#endif

EXPORT void play_ambient_sample(int whichsound)
{   if (ambient)
    {   play_sample(whichsound);
}   }

EXPORT void update_memory(FLAG force)
{   PERSIST UBYTE t;
    PERSIST int   i,
                  x, y,
                  whichaddr;
    PERSIST ULONG colour;
#ifdef WIN32
    PERSIST int   oldregionstart;
    PERSIST TEXT  tempstring[10 + 1]; // enough for "1234..7890"
    PERSIST HDC   MemMapRastPtr;
#endif
#ifdef AMIGA
    PERSIST int   oldregionlevel = -1;
#endif

    update_spriteeditor(force);

    if (!subwin[SUBWINDOW_MEMORY].hwnd)
    {   return;
    }

#ifdef WIN32
    if (repaintmemmap)
    {   repaintmemmap = FALSE;
        force = TRUE;
    }

    oldregionstart = regionstart;
    regionlevel = SendMessage
    (   GetDlgItem(subwin[SUBWINDOW_MEMORY].hwnd, IDC_MEMREGION),
        TBM_GETPOS,
        0,
        0
    );
    regionstart = regionlevel * MEMGADGETS;

    if (regionstart != oldregionstart || force)
    {   force = TRUE;

        sprintf(tempstring, "%02X00..%02XFF", regionstart / MEMGADGETS, regionstart / MEMGADGETS);
        DISCARD SetDlgItemText(subwin[SUBWINDOW_MEMORY].hwnd, IDL_REGIONDISPLAY, tempstring);
        for (i = 0; i < MEMGADGETS / 16; i++)
        {   sprintf(tempstring, "$%03Xx:", (regionstart / 16) + i);
            DISCARD SetDlgItemText(subwin[SUBWINDOW_MEMORY].hwnd, IDL_REGION0 + i, tempstring);
    }   }
#endif
#ifdef AMIGA
    if ((int) regionlevel != oldregionlevel)
    {   force = TRUE;
        oldregionlevel = regionlevel;
        SetGadgetAttrs
        (   gadgets[GID_ME_SL1], subwin[SUBWINDOW_MEMORY].hwnd, NULL,
            SLIDER_Level, regionlevel,
        TAG_DONE); // this autorefreshes
        regionstart = (int) (regionlevel * MEMGADGETS);
    }
    if ((int) regionlevel != oldregionlevel || force)
    {   sprintf((char*) gtempstring, "%04X..%04X", regionstart, regionstart + MEMGADGETS - 1);
        SetGadgetAttrs(gadgets[GID_ME_ST1], subwin[SUBWINDOW_MEMORY].hwnd, NULL, STRINGA_TextVal, gtempstring, TAG_DONE); // this autorefreshes
        for (i = 0; i < MEMGADGETS / 16; i++)
        {   sprintf((char*) gtempstring, "$%03Xx:", (regionstart / 16) + i);
            SetGadgetAttrs(gadgets[GID_FIRSTMEMROWGAD + i], subwin[SUBWINDOW_MEMORY].hwnd, NULL, STRINGA_TextVal, gtempstring, TAG_DONE); // this autorefreshes
    }   }
    if (force)
    {   update_memory_system();
    }
#endif

    for (i = 0; i < MEMGADGETS; i++)
    {   if
        (   mirror_r[regionstart + i] != regionstart + i
         && mirror_w[regionstart + i] != regionstart + i
        )
        {   t = memory[mirror_r[regionstart + i]];
        } else
        {   t = memory[         regionstart + i ];
        }
        if (t != memorycontents[i] || force)
        {   memorycontents[i] = t;
            if (viewmemas == 0)
            {   hex1(MemoryStr[i], memorycontents[i]);
                MemoryStr[i][2] = EOS;
            } else
            {   MemoryStr[i][0] = guestchar(memorycontents[i]);
                MemoryStr[i][1] = EOS;
            }
            setmemory(i, MemoryStr[i]);
    }   }

#ifdef WIN32
    update_memorytips();
#endif
#ifdef AMIGA
    if (force)
    {   make_memorytips();
    }
#endif

    if (force || viewmemas2 != 1) // contents or coverage
    {   switch (viewmemas2)
        {
        case 0: // contents
            for (y = 0; y < MEMMAPHEIGHT; y++)
            {   for (x = 0; x < MEMMAPWIDTH; x++)
                {   whichaddr = (y * MEMMAPWIDTH) + x;
                    if (memflags[whichaddr] & BREAKPOINT)
                    {   colour = EMURGBPEN_CYAN; // cyan (bp)
                    } elif (memflags[whichaddr] & WATCHPOINT)
                    {   colour = EMURGBPEN_ORANGE; // orange (wp)
                    } elif
                    (   mirror_r[whichaddr] != whichaddr
                     && mirror_w[whichaddr] != whichaddr
                    ) // purple (full mirror)
                    {
#ifdef WIN32
                        colour = 0x00010001 * memory[mirror_r[whichaddr]]; // purple (full mirror)
#endif
#ifdef AMIGA
                        if (memory[mirror_r[whichaddr]] == 0)
                        {   colour = EMURGBPEN_BLACK;
                        } elif (memory[mirror_r[whichaddr]] <= 0x7F)
                        {   colour = EMURGBPEN_DARKPURPLE;
                        } else
                        {   colour = EMURGBPEN_PURPLE;
                        }
#endif
                    } elif (memflags[whichaddr] & NOWRITE)
                    {
#ifdef WIN32
                        if (memflags[whichaddr] & NOREAD)
                        {   colour = 0x00010000 * memory[whichaddr]; // red (unmapped)
                        } else
                        {   colour =  0x00000001 * memory[whichaddr]; // blue (read-only)
                            colour += 0x00010100 * (memory[whichaddr] / 2);
                        }
#endif
#ifdef AMIGA
                        if (memflags[whichaddr] & NOREAD) // red (unmapped)
                        {   if (memory[whichaddr] == 0)
                            {   colour = EMURGBPEN_BLACK;
                            } elif (memory[whichaddr] <= 0x7F)
                            {   colour = EMURGBPEN_DARKRED;
                            } else
                            {   colour = EMURGBPEN_RED;
                        }   }
                        else // blue (read-only)
                        {   if (memory[whichaddr] == 0)
                            {   colour = EMURGBPEN_BLACK;
                            } elif (memory[whichaddr] <= 0x7F)
                            {   colour = EMURGBPEN_DARKBLUE;
                            } else
                            {   colour = EMURGBPEN_BLUE;
                        }   }
#endif
                    } elif (memflags[whichaddr] & NOREAD) // yellow (write-only)
                    {
#ifdef WIN32
                        colour = 0x00010100 * memory[whichaddr]; // yellow (write-only)
#endif
#ifdef AMIGA
                        if (memory[whichaddr] == 0)
                        {   colour = EMURGBPEN_BLACK;
                        } elif (memory[whichaddr] <= 0x7F)
                        {   colour = EMURGBPEN_DARKYELLOW;
                        } else
                        {   colour = EMURGBPEN_YELLOW;
                        }
#endif
                    } else // green (read/write)
                    {
#ifdef WIN32
                        colour = 0x00000100 * memory[whichaddr]; // green (read/write)
#endif
#ifdef AMIGA
                        if (memory[whichaddr] == 0)
                        {   colour = EMURGBPEN_BLACK;
                        } elif (memory[whichaddr] <= 0x7F)
                        {   colour = EMURGBPEN_DARKGREEN;
                        } else
                        {   colour = EMURGBPEN_GREEN;
                        }
#endif
                    }
                    DRAWMEMMAP(x, y, colour);
                    // perhaps red and purple should always be shown at full intensity?
            }   }
        acase 1: // memory map
            for (y = 0; y < MEMMAPHEIGHT; y++)
            {   for (x = 0; x < MEMMAPWIDTH; x++)
                {   whichaddr = (y * MEMMAPWIDTH) + x;
                    if (memflags[whichaddr] & BREAKPOINT)
                    {   colour = EMURGBPEN_CYAN; // cyan (bp)
                    } elif (memflags[whichaddr] & WATCHPOINT)
                    {   colour = EMURGBPEN_ORANGE; // orange (wp)
                    } elif
                    (   mirror_r[whichaddr] != whichaddr
                     || mirror_w[whichaddr] != whichaddr
                    )
                    {   colour = EMURGBPEN_PURPLE; // purple (full mirror)
                    } elif (memflags[whichaddr] & NOWRITE)
                    {   if (memflags[whichaddr] & NOREAD)
                        {   colour = EMURGBPEN_RED; // red (unmapped)
                        } else
                        {   colour = memory[whichaddr] ? EMURGBPEN_BLUE : EMURGBPEN_DARKBLUE; // blue (read-only)
                    }   }
                    elif (memflags[whichaddr] & NOREAD)
                    {   colour = EMURGBPEN_YELLOW; // yellow (write-only)
                    } else
                    {   colour = memory[whichaddr] ? EMURGBPEN_GREEN : EMURGBPEN_DARKGREEN; // green (read/write)
                    }
                    DRAWMEMMAP(x, y, colour);
            }   }
        acase 2: // coverage report
            for (y = 0; y < MEMMAPHEIGHT; y++)
            {   for (x = 0; x < MEMMAPWIDTH; x++)
                {   whichaddr = (y * MEMMAPWIDTH) + x;
                    if (coverage[whichaddr] & COVERAGE_WRITE)
                    {   if (coverage[whichaddr] & COVERAGE_READ)
                        {   colour = EMURGBPEN_GREEN;
                        } else
                        {   colour = EMURGBPEN_YELLOW;
                    }   }
                    elif (coverage[whichaddr] & COVERAGE_READ)
                    {   colour = EMURGBPEN_BLUE;
                    } elif (coverage[whichaddr] & (COVERAGE_OPCODE | COVERAGE_OPERAND))
                    {   colour = EMURGBPEN_CYAN;
                    } elif (coverage[whichaddr] & COVERAGE_ADDRESS)
                    {   colour = EMURGBPEN_PURPLE;
                    } else
                    {   colour = EMURGBPEN_BLACK;
                    }
                    DRAWMEMMAP(x, y, colour);
        }   }   }

        wpa8(CANVAS_MEMMAP, 0, 0);
}   }

EXPORT void update_magnifier(void)
{   FAST int magscalex, magscaley,
             finex, finey,
             guestx, newguestx,
             guesty, newguesty,
             remx, remy,
             x, y;
#ifdef WIN32
    FAST int hostx, hosty;
    FAST HDC MagnifierRastPtr;
#endif

    magscalex = size * 2 * wide;
    magscaley = size * 2;

#ifdef WIN32
    hosttoguestmouse(&hostx, &hosty, &guestx, &guesty, &remx, &remy);
    SetWindowPos(MagnifierWindowPtr, HWND_TOP, hostx - (MAGNIFIERWIDTH  / 2), hosty - (MAGNIFIERHEIGHT / 2), MAGNIFIERWIDTH, MAGNIFIERHEIGHT, SWP_NOSIZE);
#endif
#ifdef AMIGA
    hosttoguestmouse(NULL  , NULL  , &guestx, &guesty, &remx, &remy);
    ChangeWindowBox
    (   MagnifierWindowPtr,
        MainWindowPtr->LeftEdge + MainWindowPtr->MouseX - (MAGNIFIERWIDTH  / 2),
        MainWindowPtr->TopEdge  + MainWindowPtr->MouseY - (MAGNIFIERHEIGHT / 2),
        MAGNIFIERWIDTH,
        MAGNIFIERHEIGHT
    );
#endif

    guestx -= MAGNIFIERHEIGHT / magscalex / 2;
    guesty -= MAGNIFIERHEIGHT / magscaley / 2;
    newguesty = guesty;
    finey = remy * 2;
    for (y = 0; y < MAGNIFIERHEIGHT; y++)
    {   newguestx = guestx;
        finex = remx * 2;
        for (x = 0; x < MAGNIFIERWIDTH; x++)
        {   if
            (   newguestx >= 0
             && newguestx <  machines[machine].width
             && newguesty >= 0
             && newguesty <  machines[machine].height
            )
            {   if (rotating)
                {   DRAWMAGNIFIER(x, y, screen[newguesty][machines[machine].width - 1 - newguestx]);
                } else
                {   DRAWMAGNIFIER(x, y, screen[newguestx][                              newguesty]);
            }   }
            else
            {   DRAWMAGNIFIER(x, y, BORDERPEN);
            }
            if (finex == magscalex - 1)
            {   finex = 0;
                newguestx++;
            } else
            {   finex++;
        }   }
        if (finey == magscaley - 1)
        {   finey = 0;
            newguesty++;
        } else
        {   finey++;
    }   }

    wpa8(CANVAS_MAGNIFIER, 0, 0);
}

EXPORT int parse_hexbyte(void)
{   int number;

    if (strlen(gtempstring) != 2)
    {   return -1;
    }
    gtempstring[0] = toupper(gtempstring[0]);
    gtempstring[1] = toupper(gtempstring[1]);
    if     (gtempstring[0] >= 'A' && gtempstring[0] <= 'F')
    {   number =  (gtempstring[0] - 'A' + 10) * 16;
    } elif (gtempstring[0] >= '0' && gtempstring[0] <= '9')
    {   number =  (gtempstring[0] - '0'     ) * 16;
    } else return -1;
    if     (gtempstring[1] >= 'A' && gtempstring[1] <= 'F')
    {   number += (gtempstring[1] - 'A' + 10);
    } elif (gtempstring[1] >= '0' && gtempstring[1] <= '9')
    {   number += (gtempstring[1] - '0'     );
    } else return -1;

    return number;
}

EXPORT void update_slider(int which)
{   TRANSIENT ULONG number;
#ifdef AMIGA
    PERSIST   UWORD gradients[1 + 1] = { 0, (UWORD) ~0 };
#endif

    switch (which)
    {
    case 0: // Red--------------------------------------------------------
        number = (ULONG) getred(colourset, palettepen);
#ifdef WIN32
        sl_set(SUBWINDOW_PALETTE, IDC_RED, 255 - number);

        sprintf(gtempstring, "%d", number);
        st_set(SUBWINDOW_PALETTE, IDC_REDDEC);
#endif
#ifdef AMIGA
        sl_set(SUBWINDOW_PALETTE, IDC_RED, number);

        SetGadgetAttrs(gadgets[IDC_REDDEC], subwin[SUBWINDOW_PALETTE].hwnd, NULL, INTEGER_Number, number, TAG_DONE); // this autorefreshes
#endif
        sprintf((char*) gtempstring, "%02X", (unsigned int) number);
        st_set(SUBWINDOW_PALETTE, IDC_REDHEX);
    acase 1: // Green-----------------------------------------------------
        number = (ULONG) getgreen(colourset, palettepen);
#ifdef WIN32
        sl_set(SUBWINDOW_PALETTE, IDC_GREEN, 255 - number);

        sprintf(gtempstring, "%d", number);
        st_set(SUBWINDOW_PALETTE, IDC_GREENDEC);
#endif
#ifdef AMIGA
        sl_set(SUBWINDOW_PALETTE, IDC_GREEN, number);

        SetGadgetAttrs(gadgets[IDC_GREENDEC], subwin[SUBWINDOW_PALETTE].hwnd, NULL, INTEGER_Number, number, TAG_DONE); // this autorefreshes
#endif
        sprintf((char*) gtempstring, "%02X", (unsigned int) number);
        st_set(SUBWINDOW_PALETTE, IDC_GREENHEX);
    acase 2: // Blue------------------------------------------------------
        number = (ULONG) getblue(colourset, palettepen);
#ifdef WIN32
        sl_set(SUBWINDOW_PALETTE, IDC_BLUE, 255 - number);

        sprintf(gtempstring, "%d", number);
        st_set(SUBWINDOW_PALETTE, IDC_BLUEDEC);
#endif
#ifdef AMIGA
        sl_set(SUBWINDOW_PALETTE, IDC_BLUE, number);

        SetGadgetAttrs(gadgets[IDC_BLUEDEC], subwin[SUBWINDOW_PALETTE].hwnd, NULL, INTEGER_Number, number, TAG_DONE); // this autorefreshes
#endif
        sprintf((char*) gtempstring, "%02X", (unsigned int) number);
        st_set(SUBWINDOW_PALETTE, IDC_BLUEHEX);
#ifdef AMIGA
    acase 3: // Brightness------------------------------------------------
        GetAttr(WHEEL_Brightness, (Object*) gadgets[GID_PL_CW1], &number);
        number = (number & 0xFF000000) >> 24;
        sl_set(SUBWINDOW_PALETTE, GID_PL_SL4, number);

        gradients[0] = guestpens[palettepen];
        SetGadgetAttrs(gadgets[GID_PL_GS1], subwin[SUBWINDOW_PALETTE].hwnd, NULL, GRAD_PenArray, gradients, TAG_DONE); // this autorefreshes
#endif
}   }

EXPORT void checkcolours(void)
{   int i;

    resetdisabled = TRUE;
    for (i = 0; i < GUESTCOLOURS; i++)
    {   if
        (
#ifdef WIN32
            (pencolours[colourset][i]       & 0x00FFFFFF) !=   defpencolours[colourset][i]
#endif
#ifdef AMIGA
            (pencolours[colourset][i].red   & 0xFF000000) != ((defpencolours[colourset][i] & 0x00FF0000) <<  8)
         || (pencolours[colourset][i].green & 0xFF000000) != ((defpencolours[colourset][i] & 0x0000FF00) << 16)
         || (pencolours[colourset][i].blue  & 0xFF000000) != ((defpencolours[colourset][i] & 0x000000FF) << 24)
#endif
        )
        {   resetdisabled = FALSE;
            return; // for speed
}   }   }

EXPORT void play_bell(int kind)
{   // kind is 0 for machine bell, 1 for Centronics Model 306C printer bell

    if (kind == 0)
    {   belling[0] =   5; // 0.1 seconds (5 frames) (Radio Bulletin VDU)
    } else
    {   belling[1] = 120; // 2 seconds (120 frames) (Centronics Model 306C)
    }
    playsound(FALSE);
}

EXPORT void updatepadnames(void)
{   FAST int localmouse,
             localwhose[2];

    // For anything assigned to both players we currently show player 1's key names and overlays
    localwhose[0] = (whose[0]   == 2) ? 0 : whose[0];
    localwhose[1] = (whose[1]   == 2) ? 0 : whose[1];
    localmouse    = (whosemouse == 2) ? 0 : whosemouse;

    switch (viewpadsas)
    {
    case 0: // guest
    case 2: // overlays
#ifdef WIN32
        setpadtext(IDC_PADS_LT_AUP2, "");
        setpadtext(IDC_PADS_LT_ADN2, "");
        setpadtext(IDC_PADS_LT_ALT2, "");
        setpadtext(IDC_PADS_LT_ART2, "");
#endif

        if (whose[0] == 3)
        {   setpadtext(IDC_PADS_LT_DUP, "");
            setpadtext(IDC_PADS_LT_DDN, "");
            setpadtext(IDC_PADS_LT_DLT, "");
            setpadtext(IDC_PADS_LT_DRT, "");
#ifdef WIN32
            setpadtext(IDC_PADS_LT_A,   "");
            setpadtext(IDC_PADS_LT_B  , "");
            setpadtext(IDC_PADS_LT_1  , "");
            setpadtext(IDC_PADS_LT_2  , "");
            setpadtext(IDC_PADS_LT_3  , "");
            setpadtext(IDC_PADS_LT_4  , "");
            setpadtext(IDC_PADS_LT_5  , "");
            setpadtext(IDC_PADS_LT_6  , "");
            setpadtext(IDC_PADS_LT_7  , "");
            setpadtext(IDC_PADS_LT_8  , "");
            setpadtext(IDC_PADS_LT_11 , "");
            setpadtext(IDC_PADS_LT_12 , "");
            setpadtext(IDC_PADS_LT_AUP, "");
            setpadtext(IDC_PADS_LT_ADN, "");
            setpadtext(IDC_PADS_LT_ALT, "");
            setpadtext(IDC_PADS_LT_ART, "");
#endif
#ifdef AMIGA
            setpadtext(IDC_PADS_LT_START  , "");
            setpadtext(IDC_PADS_LT_REVERSE, "");
            setpadtext(IDC_PADS_LT_FORWARD, "");
            setpadtext(IDC_PADS_LT_RED    , "");
            setpadtext(IDC_PADS_LT_BLUE   , "");
            setpadtext(IDC_PADS_LT_GREEN  , "");
            setpadtext(IDC_PADS_LT_YELLOW , "");
#endif
        } else
        {
#ifdef WIN32
            setpadtext(IDC_PADS_LT_A  , machines[machine].consolekeyname[0]);
            setpadtext(IDC_PADS_LT_B  , "Reset");
            switch (button[localwhose[0]][4])
            {
            case  5: setpadtext(IDC_PADS_LT_5, machines[machine].consolekeyname[1]); // joy A
            acase 6: setpadtext(IDC_PADS_LT_5, machines[machine].consolekeyname[2]); // joy B
            acase 7: setpadtext(IDC_PADS_LT_5, LLL(MSG_AUTOFIRE, "Autofire"));
            acase 8: setpadtext(IDC_PADS_LT_5, LLL(MSG_PAUSE   , "Pause"));
            }
            switch (button[localwhose[0]][5])
            {
            case  5: setpadtext(IDC_PADS_LT_6, machines[machine].consolekeyname[1]); // joy A
            acase 6: setpadtext(IDC_PADS_LT_6, machines[machine].consolekeyname[2]); // joy B
            acase 7: setpadtext(IDC_PADS_LT_6, LLL(MSG_AUTOFIRE, "Autofire"));
            acase 8: setpadtext(IDC_PADS_LT_6, LLL(MSG_PAUSE   , "Pause"   ));
            }
            switch (button[localwhose[0]][6])
            {
            case  5: setpadtext(IDC_PADS_LT_7, machines[machine].consolekeyname[1]); // joy A
            acase 6: setpadtext(IDC_PADS_LT_7, machines[machine].consolekeyname[2]); // joy B
            acase 7: setpadtext(IDC_PADS_LT_7, LLL(MSG_AUTOFIRE, "Autofire"));
            acase 8: setpadtext(IDC_PADS_LT_7, LLL(MSG_PAUSE   , "Pause"   ));
            }
            switch (button[localwhose[0]][7])
            {
            case  5: setpadtext(IDC_PADS_LT_8, machines[machine].consolekeyname[1]); // joy A
            acase 6: setpadtext(IDC_PADS_LT_8, machines[machine].consolekeyname[2]); // joy B
            acase 7: setpadtext(IDC_PADS_LT_8, LLL(MSG_AUTOFIRE, "Autofire"));
            acase 8: setpadtext(IDC_PADS_LT_8, LLL(MSG_PAUSE   , "Pause"   ));
            }
#endif

            if (viewpadsas == 0 || whichoverlay == -1)
            {
#ifdef WIN32
                setpadtext(IDC_PADS_LT_1     , machines[machine].keynames[localwhose[0]][buttontranslate(0, 0)]);
                setpadtext(IDC_PADS_LT_2     , machines[machine].keynames[localwhose[0]][buttontranslate(0, 1)]);
                setpadtext(IDC_PADS_LT_3     , machines[machine].keynames[localwhose[0]][buttontranslate(0, 2)]);
                setpadtext(IDC_PADS_LT_4     , machines[machine].keynames[localwhose[0]][buttontranslate(0, 3)]);
                setpadtext(IDC_PADS_LT_11    , machines[machine].keynames[localwhose[0]][key1]);
                setpadtext(IDC_PADS_LT_12    , machines[machine].keynames[localwhose[0]][key1]);
#endif
#ifdef AMIGA
                setpadtext(IDC_PADS_LT_RED   , machines[machine].keynames[localwhose[0]][buttontranslate(0, 0)]);
                setpadtext(IDC_PADS_LT_BLUE  , machines[machine].keynames[localwhose[0]][buttontranslate(0, 1)]);
                setpadtext(IDC_PADS_LT_GREEN , machines[machine].keynames[localwhose[0]][buttontranslate(0, 2)]);
                setpadtext(IDC_PADS_LT_YELLOW, machines[machine].keynames[localwhose[0]][buttontranslate(0, 3)]);
#endif
            } else
            {
#ifdef WIN32
                setpadtext(IDC_PADS_LT_1     , overlays[whichoverlay][keytable[buttontranslate(0, 0)].overlay[localwhose[0]]]);
                setpadtext(IDC_PADS_LT_2     , overlays[whichoverlay][keytable[buttontranslate(0, 1)].overlay[localwhose[0]]]);
                setpadtext(IDC_PADS_LT_3     , overlays[whichoverlay][keytable[buttontranslate(0, 2)].overlay[localwhose[0]]]);
                setpadtext(IDC_PADS_LT_4     , overlays[whichoverlay][keytable[buttontranslate(0, 3)].overlay[localwhose[0]]]);
                setpadtext(IDC_PADS_LT_11    , overlays[whichoverlay][keytable[key1                 ].overlay[localwhose[0]]]);
                setpadtext(IDC_PADS_LT_12    , overlays[whichoverlay][keytable[key1                 ].overlay[localwhose[0]]]);
#endif
#ifdef AMIGA
                setpadtext(IDC_PADS_LT_RED   , overlays[whichoverlay][keytable[buttontranslate(0, 0)].overlay[localwhose[0]]]);
                setpadtext(IDC_PADS_LT_BLUE  , overlays[whichoverlay][keytable[buttontranslate(0, 1)].overlay[localwhose[0]]]);
                setpadtext(IDC_PADS_LT_GREEN , overlays[whichoverlay][keytable[buttontranslate(0, 2)].overlay[localwhose[0]]]);
                setpadtext(IDC_PADS_LT_YELLOW, overlays[whichoverlay][keytable[buttontranslate(0, 3)].overlay[localwhose[0]]]);
#endif
            }

#ifdef AMIGA
            setpadtext(IDC_PADS_LT_START  , machines[machine].consolekeyname[0]);
            setpadtext(IDC_PADS_LT_REVERSE, machines[machine].consolekeyname[1]);
            setpadtext(IDC_PADS_LT_FORWARD, machines[machine].consolekeyname[2]);
#endif

            if (whichgame == -1 || known[whichgame].paddleup == -1)
            {   setpadtext(IDC_PADS_LT_DUP, LLL(MSG_KEY_UP, "Up"));
#ifdef WIN32
                setpadtext(IDC_PADS_LT_AUP, LLL(MSG_KEY_UP, "Up"));
#endif
            } elif (viewpadsas == 0) // guest
            {   setpadtext(IDC_PADS_LT_DUP, machines[machine].keynames[localwhose[0]][known[whichgame].paddleup]);
#ifdef WIN32
                setpadtext(IDC_PADS_LT_AUP, machines[machine].keynames[localwhose[0]][known[whichgame].paddleup]);
#endif
            } else
            {   // assert(viewpadsas == 2);
                setpadtext(IDC_PADS_LT_DUP, overlays[whichoverlay][keytable[known[whichgame].paddleup].overlay[localwhose[0]]]);
#ifdef WIN32
                setpadtext(IDC_PADS_LT_AUP, overlays[whichoverlay][keytable[known[whichgame].paddleup].overlay[localwhose[0]]]);
#endif
            }
            if (whichgame == -1 || known[whichgame].paddledown == -1)
            {   setpadtext(IDC_PADS_LT_DDN, LLL(MSG_KEY_DN, "Dn"));
#ifdef WIN32
                setpadtext(IDC_PADS_LT_ADN, LLL(MSG_KEY_DN, "Dn"));
#endif
            } elif (viewpadsas == 0) // guest
            {   setpadtext(IDC_PADS_LT_DDN, machines[machine].keynames[localwhose[0]][known[whichgame].paddledown]);
#ifdef WIN32
                setpadtext(IDC_PADS_LT_ADN, machines[machine].keynames[localwhose[0]][known[whichgame].paddledown]);
#endif
            } else
            {   // assert(viewpadsas == 2);
                setpadtext(IDC_PADS_LT_DDN, overlays[whichoverlay][keytable[known[whichgame].paddledown].overlay[localwhose[0]]]);
#ifdef WIN32
                setpadtext(IDC_PADS_LT_ADN, overlays[whichoverlay][keytable[known[whichgame].paddledown].overlay[localwhose[0]]]);
#endif
            }
            if (whichgame == -1 || known[whichgame].paddleleft == -1)
            {   setpadtext(IDC_PADS_LT_DLT, LLL(MSG_KEY_LT, "Lt"));
#ifdef WIN32
                setpadtext(IDC_PADS_LT_ALT, LLL(MSG_KEY_LT, "Lt"));
#endif
            } elif (viewpadsas == 0) // guest
            {   setpadtext(IDC_PADS_LT_DLT, machines[machine].keynames[localwhose[0]][known[whichgame].paddleleft]);
#ifdef WIN32
                setpadtext(IDC_PADS_LT_ALT, machines[machine].keynames[localwhose[0]][known[whichgame].paddleleft]);
#endif
            } else
            {   // assert(viewpadsas == 2);
                setpadtext(IDC_PADS_LT_DLT, overlays[whichoverlay][keytable[known[whichgame].paddleleft].overlay[localwhose[0]]]);
#ifdef WIN32
                setpadtext(IDC_PADS_LT_ALT, overlays[whichoverlay][keytable[known[whichgame].paddleleft].overlay[localwhose[0]]]);
#endif
            }
            if (whichgame == -1 || known[whichgame].paddleright == -1)
            {   setpadtext(IDC_PADS_LT_DRT, LLL(MSG_KEY_RT, "Rt"));
#ifdef WIN32
                setpadtext(IDC_PADS_LT_ART, LLL(MSG_KEY_RT, "Rt"));
#endif
            } elif (viewpadsas == 0) // guest
            {   setpadtext(IDC_PADS_LT_DRT, machines[machine].keynames[localwhose[0]][known[whichgame].paddleright]);
#ifdef WIN32
                setpadtext(IDC_PADS_LT_ART, machines[machine].keynames[localwhose[0]][known[whichgame].paddleright]);
#endif
            } else
            {   // assert(viewpadsas == 2);
                setpadtext(IDC_PADS_LT_DRT, overlays[whichoverlay][keytable[known[whichgame].paddleright].overlay[localwhose[0]]]);
#ifdef WIN32
                setpadtext(IDC_PADS_LT_ART, overlays[whichoverlay][keytable[known[whichgame].paddleright].overlay[localwhose[0]]]);
#endif
        }   }

        if (whosemouse == 3)
        {   setpadtext(IDC_PADS_LTMOUSE, "");
            setpadtext(IDC_PADS_MDMOUSE, "");
            setpadtext(IDC_PADS_RTMOUSE, LLL(MSG_RIGHT, "Right"));
        } elif (viewpadsas == 0) // guest
        {   setpadtext(IDC_PADS_LTMOUSE, machines[machine].keynames[localmouse][key1]);
            setpadtext(IDC_PADS_MDMOUSE, machines[machine].keynames[localmouse][key2]);
            if (guestrmb)
            {   setpadtext(IDC_PADS_RTMOUSE, machines[machine].keynames[localmouse][key3]);
            } else
            {   setpadtext(IDC_PADS_RTMOUSE, LLL(MSG_RIGHT, "Right"));
        }   }
        else
        {   // assert(viewpadsas == 2); // overlays
            setpadtext(IDC_PADS_LTMOUSE, overlays[whichoverlay][keytable[key1].overlay[localmouse]]);
            setpadtext(IDC_PADS_MDMOUSE, overlays[whichoverlay][keytable[key2].overlay[localmouse]]);
            if (guestrmb)
            {   setpadtext(IDC_PADS_RTMOUSE, overlays[whichoverlay][keytable[key3].overlay[localmouse]]);
            } else
            {   setpadtext(IDC_PADS_RTMOUSE, LLL(MSG_RIGHT, "Right"));
        }   }

#ifdef WIN32
        if (joys < 2)
        {   return;
        }
#endif

#ifdef WIN32
        setpadtext(IDC_PADS_RT_AUP2, "");
        setpadtext(IDC_PADS_RT_ADN2, "");
        setpadtext(IDC_PADS_RT_ALT2, "");
        setpadtext(IDC_PADS_RT_ART2, "");
#endif

        if (whose[1] == 3)
        {   setpadtext(IDC_PADS_RT_DUP, "");
            setpadtext(IDC_PADS_RT_DDN, "");
            setpadtext(IDC_PADS_RT_DLT, "");
            setpadtext(IDC_PADS_RT_DRT, "");
#ifdef WIN32
            setpadtext(IDC_PADS_RT_A,   "");
            setpadtext(IDC_PADS_RT_B  , "");
            setpadtext(IDC_PADS_RT_1  , "");
            setpadtext(IDC_PADS_RT_2  , "");
            setpadtext(IDC_PADS_RT_3  , "");
            setpadtext(IDC_PADS_RT_4  , "");
            setpadtext(IDC_PADS_RT_5  , "");
            setpadtext(IDC_PADS_RT_6  , "");
            setpadtext(IDC_PADS_RT_7  , "");
            setpadtext(IDC_PADS_RT_8  , "");
            setpadtext(IDC_PADS_RT_11 , "");
            setpadtext(IDC_PADS_RT_12 , "");
            setpadtext(IDC_PADS_RT_AUP, "");
            setpadtext(IDC_PADS_RT_ADN, "");
            setpadtext(IDC_PADS_RT_ALT, "");
            setpadtext(IDC_PADS_RT_ART, "");
#endif
#ifdef AMIGA
            setpadtext(IDC_PADS_RT_START  , "");
            setpadtext(IDC_PADS_RT_REVERSE, "");
            setpadtext(IDC_PADS_RT_FORWARD, "");
            setpadtext(IDC_PADS_LT_RED    , "");
            setpadtext(IDC_PADS_LT_BLUE   , "");
            setpadtext(IDC_PADS_LT_GREEN  , "");
            setpadtext(IDC_PADS_LT_YELLOW , "");
#endif
        } else
        {
#ifdef WIN32
            setpadtext(IDC_PADS_RT_A  , machines[machine].consolekeyname[0]);
            setpadtext(IDC_PADS_RT_B  , "Reset");
            switch (button[localwhose[1]][4])
            {
            case  5: setpadtext(IDC_PADS_RT_5, machines[machine].consolekeyname[1]); // joy A
            acase 6: setpadtext(IDC_PADS_RT_5, machines[machine].consolekeyname[2]); // joy B
            acase 7: setpadtext(IDC_PADS_RT_5, LLL(MSG_AUTOFIRE, "Autofire"));
            acase 8: setpadtext(IDC_PADS_RT_5, LLL(MSG_PAUSE   , "Pause"));
            }
            switch (button[localwhose[1]][5])
            {
            case  5: setpadtext(IDC_PADS_RT_6, machines[machine].consolekeyname[1]); // joy A
            acase 6: setpadtext(IDC_PADS_RT_6, machines[machine].consolekeyname[2]); // joy B
            acase 7: setpadtext(IDC_PADS_RT_6, LLL(MSG_AUTOFIRE, "Autofire"));
            acase 8: setpadtext(IDC_PADS_RT_6, LLL(MSG_PAUSE   , "Pause"   ));
            }
            switch (button[localwhose[1]][6])
            {
            case  5: setpadtext(IDC_PADS_RT_7, machines[machine].consolekeyname[1]); // joy A
            acase 6: setpadtext(IDC_PADS_RT_7, machines[machine].consolekeyname[2]); // joy B
            acase 7: setpadtext(IDC_PADS_RT_7, LLL(MSG_AUTOFIRE, "Autofire"));
            acase 8: setpadtext(IDC_PADS_RT_7, LLL(MSG_PAUSE   , "Pause"   ));
            }
            switch (button[localwhose[1]][7])
            {
            case  5: setpadtext(IDC_PADS_RT_8, machines[machine].consolekeyname[1]); // joy A
            acase 6: setpadtext(IDC_PADS_RT_8, machines[machine].consolekeyname[2]); // joy B
            acase 7: setpadtext(IDC_PADS_RT_8, LLL(MSG_AUTOFIRE, "Autofire"));
            acase 8: setpadtext(IDC_PADS_RT_8, LLL(MSG_PAUSE   , "Pause"   ));
            }
#endif

            if (viewpadsas == 0 || whichoverlay == -1)
            {
#ifdef WIN32
                setpadtext(IDC_PADS_RT_1     , machines[machine].keynames[localwhose[1]][buttontranslate(1, 0)]);
                setpadtext(IDC_PADS_RT_2     , machines[machine].keynames[localwhose[1]][buttontranslate(1, 1)]);
                setpadtext(IDC_PADS_RT_3     , machines[machine].keynames[localwhose[1]][buttontranslate(1, 2)]);
                setpadtext(IDC_PADS_RT_4     , machines[machine].keynames[localwhose[1]][buttontranslate(1, 3)]);
                setpadtext(IDC_PADS_RT_11    , machines[machine].keynames[localwhose[1]][key1]);
                setpadtext(IDC_PADS_RT_12    , machines[machine].keynames[localwhose[1]][key1]);
#endif
#ifdef AMIGA
                setpadtext(IDC_PADS_RT_RED   , machines[machine].keynames[localwhose[1]][buttontranslate(1, 0)]);
                setpadtext(IDC_PADS_RT_BLUE  , machines[machine].keynames[localwhose[1]][buttontranslate(1, 1)]);
                setpadtext(IDC_PADS_RT_GREEN , machines[machine].keynames[localwhose[1]][buttontranslate(1, 2)]);
                setpadtext(IDC_PADS_RT_YELLOW, machines[machine].keynames[localwhose[1]][buttontranslate(1, 3)]);
#endif

            } else
            {
#ifdef WIN32
                setpadtext(IDC_PADS_RT_1     , overlays[whichoverlay][keytable[buttontranslate(1, 0)].overlay[localwhose[1]]]);
                setpadtext(IDC_PADS_RT_2     , overlays[whichoverlay][keytable[buttontranslate(1, 1)].overlay[localwhose[1]]]);
                setpadtext(IDC_PADS_RT_3     , overlays[whichoverlay][keytable[buttontranslate(1, 2)].overlay[localwhose[1]]]);
                setpadtext(IDC_PADS_RT_4     , overlays[whichoverlay][keytable[buttontranslate(1, 3)].overlay[localwhose[1]]]);
                setpadtext(IDC_PADS_RT_11    , overlays[whichoverlay][keytable[key1                 ].overlay[localwhose[1]]]);
                setpadtext(IDC_PADS_RT_12    , overlays[whichoverlay][keytable[key1                 ].overlay[localwhose[1]]]);
#endif
#ifdef AMIGA
                setpadtext(IDC_PADS_RT_RED   , overlays[whichoverlay][keytable[buttontranslate(1, 0)].overlay[localwhose[1]]]);
                setpadtext(IDC_PADS_RT_BLUE  , overlays[whichoverlay][keytable[buttontranslate(1, 1)].overlay[localwhose[1]]]);
                setpadtext(IDC_PADS_RT_GREEN , overlays[whichoverlay][keytable[buttontranslate(1, 2)].overlay[localwhose[1]]]);
                setpadtext(IDC_PADS_RT_YELLOW, overlays[whichoverlay][keytable[buttontranslate(1, 3)].overlay[localwhose[1]]]);
#endif
            }

#ifdef AMIGA
            setpadtext(IDC_PADS_RT_START  , machines[machine].consolekeyname[0]);
            setpadtext(IDC_PADS_RT_REVERSE, machines[machine].consolekeyname[1]);
            setpadtext(IDC_PADS_RT_FORWARD, machines[machine].consolekeyname[2]);
#endif

            if (whichgame == -1 || known[whichgame].paddleup == -1)
            {   setpadtext(IDC_PADS_RT_DUP, LLL(MSG_KEY_UP, "Up"));
#ifdef WIN32
                setpadtext(IDC_PADS_RT_AUP, LLL(MSG_KEY_UP, "Up"));
#endif
            } elif (viewpadsas == 0) // guest
            {   setpadtext(IDC_PADS_RT_DUP, machines[machine].keynames[localwhose[1]][known[whichgame].paddleup]);
#ifdef WIN32
                setpadtext(IDC_PADS_RT_AUP, machines[machine].keynames[localwhose[1]][known[whichgame].paddleup]);
#endif
            } else
            {   // assert(viewpadsas == 2);
                setpadtext(IDC_PADS_RT_DUP, overlays[whichoverlay][keytable[known[whichgame].paddleup].overlay[localwhose[1]]]);
#ifdef WIN32
                setpadtext(IDC_PADS_RT_AUP, overlays[whichoverlay][keytable[known[whichgame].paddleup].overlay[localwhose[1]]]);
#endif
            }
            if (whichgame == -1 || known[whichgame].paddledown == -1)
            {   setpadtext(IDC_PADS_RT_DDN, LLL(MSG_KEY_DN, "Dn"));
#ifdef WIN32
                setpadtext(IDC_PADS_RT_ADN, LLL(MSG_KEY_DN, "Dn"));
#endif
            } elif (viewpadsas == 0) // guest
            {   setpadtext(IDC_PADS_RT_DDN, machines[machine].keynames[localwhose[1]][known[whichgame].paddledown]);
#ifdef WIN32
                setpadtext(IDC_PADS_RT_ADN, machines[machine].keynames[localwhose[1]][known[whichgame].paddledown]);
#endif
            } else
            {   // assert(viewpadsas == 2);
                setpadtext(IDC_PADS_RT_DDN, overlays[whichoverlay][keytable[known[whichgame].paddledown].overlay[localwhose[1]]]);
#ifdef WIN32
                setpadtext(IDC_PADS_RT_ADN, overlays[whichoverlay][keytable[known[whichgame].paddledown].overlay[localwhose[1]]]);
#endif
            }
            if (whichgame == -1 || known[whichgame].paddleleft == -1)
            {   setpadtext(IDC_PADS_RT_DLT, LLL(MSG_KEY_LT, "Lt"));
#ifdef WIN32
                setpadtext(IDC_PADS_RT_ALT, LLL(MSG_KEY_LT, "Lt"));
#endif
            } elif (viewpadsas == 0) // guest
            {   setpadtext(IDC_PADS_RT_DLT, machines[machine].keynames[localwhose[1]][known[whichgame].paddleleft]);
#ifdef WIN32
                setpadtext(IDC_PADS_RT_ALT, machines[machine].keynames[localwhose[1]][known[whichgame].paddleleft]);
#endif
            } else
            {   // assert(viewpadsas == 2);
                setpadtext(IDC_PADS_RT_DLT, overlays[whichoverlay][keytable[known[whichgame].paddleleft].overlay[localwhose[1]]]);
#ifdef WIN32
                setpadtext(IDC_PADS_RT_ALT, overlays[whichoverlay][keytable[known[whichgame].paddleleft].overlay[localwhose[1]]]);
#endif
            }
            if (whichgame == -1 || known[whichgame].paddleright == -1)
            {   setpadtext(IDC_PADS_RT_DRT, LLL(MSG_KEY_RT, "Rt"));
#ifdef WIN32
                setpadtext(IDC_PADS_RT_ART, LLL(MSG_KEY_RT, "Rt"));
#endif
            } elif (viewpadsas == 0) // guest
            {   setpadtext(IDC_PADS_RT_DRT, machines[machine].keynames[localwhose[1]][known[whichgame].paddleright]);
#ifdef WIN32
                setpadtext(IDC_PADS_RT_ART, machines[machine].keynames[localwhose[1]][known[whichgame].paddleright]);
#endif
            } else
            {   // assert(viewpadsas == 2);
                setpadtext(IDC_PADS_RT_DRT, overlays[whichoverlay][keytable[known[whichgame].paddleright].overlay[localwhose[1]]]);
#ifdef WIN32
                setpadtext(IDC_PADS_RT_ART, overlays[whichoverlay][keytable[known[whichgame].paddleright].overlay[localwhose[1]]]);
#endif
        }   }
    acase 1: // host
        if (whosemouse == 3)
        {   setpadtext(IDC_PADS_LTMOUSE, "");
            setpadtext(IDC_PADS_MDMOUSE, "");
            setpadtext(IDC_PADS_RTMOUSE, LLL(MSG_RIGHT , "Right"));
        } else
        {   setpadtext(IDC_PADS_LTMOUSE, LLL(MSG_LEFT  , "Left"  ));
            setpadtext(IDC_PADS_MDMOUSE, LLL(MSG_MIDDLE, "Middle"));
            setpadtext(IDC_PADS_RTMOUSE, LLL(MSG_RIGHT , "Right" ));
        }

        if (whose[0] == 3)
        {   setpadtext(IDC_PADS_LT_DUP,  "");
            setpadtext(IDC_PADS_LT_DDN,  "");
            setpadtext(IDC_PADS_LT_DLT,  "");
            setpadtext(IDC_PADS_LT_DRT,  "");
#ifdef WIN32
            setpadtext(IDC_PADS_LT_A,    "");
            setpadtext(IDC_PADS_LT_B,    "");
            setpadtext(IDC_PADS_LT_1,    "");
            setpadtext(IDC_PADS_LT_2,    "");
            setpadtext(IDC_PADS_LT_3,    "");
            setpadtext(IDC_PADS_LT_4,    "");
            setpadtext(IDC_PADS_LT_5,    "");
            setpadtext(IDC_PADS_LT_6,    "");
            setpadtext(IDC_PADS_LT_7,    "");
            setpadtext(IDC_PADS_LT_8,    "");
            setpadtext(IDC_PADS_LT_11,   "");
            setpadtext(IDC_PADS_LT_12,   "");
            setpadtext(IDC_PADS_LT_AUP,  "");
            setpadtext(IDC_PADS_LT_ADN,  "");
            setpadtext(IDC_PADS_LT_ALT,  "");
            setpadtext(IDC_PADS_LT_ART,  "");
            setpadtext(IDC_PADS_LT_AUP2, "");
            setpadtext(IDC_PADS_LT_ADN2, "");
            setpadtext(IDC_PADS_LT_ALT2, "");
            setpadtext(IDC_PADS_LT_ART2, "");
#endif
#ifdef AMIGA
            setpadtext(IDC_PADS_LT_START  , "");
            setpadtext(IDC_PADS_LT_REVERSE, "");
            setpadtext(IDC_PADS_LT_FORWARD, "");
            setpadtext(IDC_PADS_LT_RED    , "");
            setpadtext(IDC_PADS_LT_BLUE   , "");
            setpadtext(IDC_PADS_LT_GREEN  , "");
            setpadtext(IDC_PADS_LT_YELLOW , "");
#endif
        } else
        {   setpadtext(IDC_PADS_LT_DUP,   LLL(MSG_KEY_UP, "Up"    ));
            setpadtext(IDC_PADS_LT_DDN,   LLL(MSG_KEY_DN, "Dn"    ));
            setpadtext(IDC_PADS_LT_DLT,   LLL(MSG_KEY_LT, "Lt"    ));
            setpadtext(IDC_PADS_LT_DRT,   LLL(MSG_KEY_RT, "Rt"    ));
#ifdef WIN32
            setpadtext(IDC_PADS_LT_A,     LLL(MSG_START2, "Start" ));
            setpadtext(IDC_PADS_LT_B,     LLL(MSG_SELECT, "SELECT")); // would be better in mixed case in this instance
            setpadtext(IDC_PADS_LT_1,     "1");
            setpadtext(IDC_PADS_LT_2,     "2");
            setpadtext(IDC_PADS_LT_3,     "3");
            setpadtext(IDC_PADS_LT_4,     "4");
            setpadtext(IDC_PADS_LT_5,     "5");
            setpadtext(IDC_PADS_LT_6,     "6");
            setpadtext(IDC_PADS_LT_7,     "7");
            setpadtext(IDC_PADS_LT_8,     "8");
            setpadtext(IDC_PADS_LT_11,    "11");
            setpadtext(IDC_PADS_LT_12,    "12");
            setpadtext(IDC_PADS_LT_AUP,   LLL(MSG_KEY_UP, "Up"    ));
            setpadtext(IDC_PADS_LT_ADN,   LLL(MSG_KEY_DN, "Dn"    ));
            setpadtext(IDC_PADS_LT_ALT,   LLL(MSG_KEY_LT, "Lt"    ));
            setpadtext(IDC_PADS_LT_ART,   LLL(MSG_KEY_RT, "Rt"    ));
            setpadtext(IDC_PADS_LT_AUP2,  LLL(MSG_KEY_UP, "Up"    ));
            setpadtext(IDC_PADS_LT_ADN2,  LLL(MSG_KEY_DN, "Dn"    ));
            setpadtext(IDC_PADS_LT_ALT2,  LLL(MSG_KEY_LT, "Lt"    ));
            setpadtext(IDC_PADS_LT_ART2,  LLL(MSG_KEY_RT, "Rt"    ));
#endif
#ifdef AMIGA
            setpadtext(IDC_PADS_LT_START, LLL(MSG_START2, "Start" ));
            if (viewpadsas2 == 0) // CD남
            {   setpadtext(IDC_PADS_LT_REVERSE, LLL(MSG_REVERSE       , "Reverse"));
                setpadtext(IDC_PADS_LT_FORWARD, LLL(MSG_FORWARD       , "Forward"));
                setpadtext(IDC_PADS_LT_RED    , LLL(MSG_LABEL_RED     , "Red"    ));
                setpadtext(IDC_PADS_LT_BLUE   , LLL(MSG_LABEL_BLUE    , "Blue"   ));
                setpadtext(IDC_PADS_LT_GREEN  , LLL(MSG_LABEL_GREEN   , "Green"  ));
                setpadtext(IDC_PADS_LT_YELLOW , LLL(MSG_COLOUR2_YELLOW, "Yellow" ));
            } else // Megadrive
            {   setpadtext(IDC_PADS_LT_REVERSE,                         "Y"       );
                setpadtext(IDC_PADS_LT_FORWARD,                         "Z"       );
                setpadtext(IDC_PADS_LT_RED    ,                         "A"       );
                setpadtext(IDC_PADS_LT_BLUE   ,                         "B"       );
                setpadtext(IDC_PADS_LT_GREEN  ,                         "C"       );
                setpadtext(IDC_PADS_LT_YELLOW ,                         "X"       );
            }
#endif
        }

#ifdef WIN32
        if (joys < 2)
        {   return;
        }
#endif

        if (whose[1] == 3)
        {   setpadtext(IDC_PADS_RT_DUP,  "");
            setpadtext(IDC_PADS_RT_DDN,  "");
            setpadtext(IDC_PADS_RT_DLT,  "");
            setpadtext(IDC_PADS_RT_DRT,  "");
#ifdef WIN32
            setpadtext(IDC_PADS_RT_A,    "");
            setpadtext(IDC_PADS_RT_B,    "");
            setpadtext(IDC_PADS_RT_1,    "");
            setpadtext(IDC_PADS_RT_2,    "");
            setpadtext(IDC_PADS_RT_3,    "");
            setpadtext(IDC_PADS_RT_4,    "");
            setpadtext(IDC_PADS_RT_5,    "");
            setpadtext(IDC_PADS_RT_6,    "");
            setpadtext(IDC_PADS_RT_7,    "");
            setpadtext(IDC_PADS_RT_8,    "");
            setpadtext(IDC_PADS_RT_11,   "");
            setpadtext(IDC_PADS_RT_12,   "");
            setpadtext(IDC_PADS_RT_AUP,  "");
            setpadtext(IDC_PADS_RT_ADN,  "");
            setpadtext(IDC_PADS_RT_ALT,  "");
            setpadtext(IDC_PADS_RT_ART,  "");
            setpadtext(IDC_PADS_RT_AUP2, "");
            setpadtext(IDC_PADS_RT_ADN2, "");
            setpadtext(IDC_PADS_RT_ALT2, "");
            setpadtext(IDC_PADS_RT_ART2, "");
#endif
#ifdef AMIGA
            setpadtext(IDC_PADS_RT_START  , "");
            setpadtext(IDC_PADS_RT_REVERSE, "");
            setpadtext(IDC_PADS_RT_FORWARD, "");
            setpadtext(IDC_PADS_RT_RED    , "");
            setpadtext(IDC_PADS_RT_BLUE   , "");
            setpadtext(IDC_PADS_RT_GREEN  , "");
            setpadtext(IDC_PADS_RT_YELLOW , "");
#endif
        } else
        {   setpadtext(IDC_PADS_RT_DUP,   LLL(MSG_KEY_UP, "Up"    ));
            setpadtext(IDC_PADS_RT_DDN,   LLL(MSG_KEY_DN, "Dn"    ));
            setpadtext(IDC_PADS_RT_DLT,   LLL(MSG_KEY_LT, "Lt"    ));
            setpadtext(IDC_PADS_RT_DRT,   LLL(MSG_KEY_RT, "Rt"    ));
#ifdef WIN32
            setpadtext(IDC_PADS_RT_A,     LLL(MSG_START2, "Start" ));
            setpadtext(IDC_PADS_RT_B,     LLL(MSG_SELECT, "SELECT")); // would be better in mixed case in this instance
            setpadtext(IDC_PADS_RT_1,     "1");
            setpadtext(IDC_PADS_RT_2,     "2");
            setpadtext(IDC_PADS_RT_3,     "3");
            setpadtext(IDC_PADS_RT_4,     "4");
            setpadtext(IDC_PADS_RT_5,     "5");
            setpadtext(IDC_PADS_RT_6,     "6");
            setpadtext(IDC_PADS_RT_7,     "7");
            setpadtext(IDC_PADS_RT_8,     "8");
            setpadtext(IDC_PADS_RT_11,    "11");
            setpadtext(IDC_PADS_RT_12,    "12");
            setpadtext(IDC_PADS_RT_AUP,   LLL(MSG_KEY_UP, "Up"    ));
            setpadtext(IDC_PADS_RT_ADN,   LLL(MSG_KEY_DN, "Dn"    ));
            setpadtext(IDC_PADS_RT_ALT,   LLL(MSG_KEY_LT, "Lt"    ));
            setpadtext(IDC_PADS_RT_ART,   LLL(MSG_KEY_RT, "Rt"    ));
            setpadtext(IDC_PADS_RT_AUP2,  LLL(MSG_KEY_UP, "Up"    ));
            setpadtext(IDC_PADS_RT_ADN2,  LLL(MSG_KEY_DN, "Dn"    ));
            setpadtext(IDC_PADS_RT_ALT2,  LLL(MSG_KEY_LT, "Lt"    ));
            setpadtext(IDC_PADS_RT_ART2,  LLL(MSG_KEY_RT, "Rt"    ));
#endif
#ifdef AMIGA
            setpadtext(IDC_PADS_RT_START, LLL(MSG_START2, "Start" ));
            if (viewpadsas2 == 0) // CD남
            {   setpadtext(IDC_PADS_RT_REVERSE, LLL(MSG_REVERSE       , "Reverse"));
                setpadtext(IDC_PADS_RT_FORWARD, LLL(MSG_FORWARD       , "Forward"));
                setpadtext(IDC_PADS_RT_RED    , LLL(MSG_LABEL_RED     , "Red"    ));
                setpadtext(IDC_PADS_RT_BLUE   , LLL(MSG_LABEL_BLUE    , "Blue"   ));
                setpadtext(IDC_PADS_RT_GREEN  , LLL(MSG_LABEL_GREEN   , "Green"  ));
                setpadtext(IDC_PADS_RT_YELLOW , LLL(MSG_COLOUR2_YELLOW, "Yellow" ));
            } else // Megadrive
            {   setpadtext(IDC_PADS_RT_REVERSE,                         "Y"       );
                setpadtext(IDC_PADS_RT_FORWARD,                         "Z"       );
                setpadtext(IDC_PADS_RT_RED    ,                         "A"       );
                setpadtext(IDC_PADS_RT_BLUE   ,                         "B"       );
                setpadtext(IDC_PADS_RT_GREEN  ,                         "C"       );
                setpadtext(IDC_PADS_RT_YELLOW ,                         "X"       );
            }
#endif
}   }   }

MODULE int buttontranslate(int player, int which)
{   switch (button[swapped ? (1 - player) : player][which])
    {
    acase 2: return key2;
    acase 3: return key3;
    acase 4: return key4;
    adefault: // eg. 1
        return key1;
}   }

EXPORT void set_filename(void)
{   switch (memmap)
    {
    case  MEMMAP_ASTROWARS:                          changefilepart((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game, "AstroWars.cos");
    acase MEMMAP_GALAXIA:                            changefilepart((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game, "Galaxia.cos");
    acase MEMMAP_LASERBATTLE:                        changefilepart((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game, "LaserBattle.cos");
    acase MEMMAP_LAZARIAN:                           changefilepart((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game, "Lazarian.cos");
    acase MEMMAP_MALZAK1:                            changefilepart((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game, "Malzak1.cos");
    acase MEMMAP_MALZAK2:                            changefilepart((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game, "Malzak2.cos");
    acase MEMMAP_8550:        if (pong_machine == 0) changefilepart((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game, "AY-3-8500.cos");
                              else                   changefilepart((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game, "AY-3-8550.cos");
    acase MEMMAP_8600:                               changefilepart((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game, "AY-3-8600.cos");
    acase MEMMAP_TYPERIGHT:                          changefilepart((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game, "Type-right.cos");
    adefault:                                        changefilepart((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game, "");
}   }

EXPORT void update_variant(FLAG force)
{   FAST    int    val;
    FAST    STRPTR digits;
    PERSIST UBYTE  t1, t2;
    PERSIST TEXT   thestring[80 + 1];

    if
    (   !subwin[SUBWINDOW_GAMEINFO].hwnd
     || (   machine != ARCADIA
         && machine != INTERTON
         && machine != ELEKTOR
    )   )
    {   return;
    }

    switch (whichgame)
    {
    // ARCADIA games------------------------------------------------------
    case BREAKAWAYPOS: // game-1 is $1AFC, opt-1 is $1AFD
        if (!force && t1 == memory[0x1AFC] && t2 == memory[0x1AFD])
        {   return;
        }
        t1 = memory[0x1AFC];
        t2 = memory[0x1AFD];
        sprintf
        (   thestring,
            "%s, %s %s paddle(s), %s, %s ball, %s walls",
            ( memory[0x1AFC] <=  3) ? "6 balls" : "5 mins",
            ( memory[0x1AFC] <= 11) ? "normal"  : "shrinking",
            ((memory[0x1AFD] &   3) <= 1) ? "long"  : "short",
            players[(memory[0x1AFC] & 15) / 4],
            balls[   memory[0x1AFC] &  3     ],
            ( memory[0x1AFD] &   1      ) ? "invis" : "vis"        );
    acase CATTRAXPOS: // game-1 is $18FB, opt-1 is $1AF5
        if (!force && t1 == memory[0x18FB] && t2 == memory[0x1AF5])
        {   return;
        }
        t1 = memory[0x18FB];
        t2 = memory[0x1AF5];
        sprintf
        (   thestring,
            "%s, %s red dog, %s",
            hypergates[memory[0x18FB] & 3],
            (memory[0x1AF5] > 3) ? "fast" : "slow",
            openings[memory[0x1AF5] & 3]
        );
    acase A_CIRCUSPOS: // game is $1AE4, player is bit 4 of $18DF
        if (!force && t1 == memory[0x1AE4] && t2 == (memory[0x18DF] & 0x10))
        {   return;
        }
        t1 = memory[0x1AE4];
        t2 = memory[0x18DF] & 0x10;
        sprintf
        (   thestring,
            "%s, %s balloons, %s platforms",
            (  memory[0x18DF]   & 0x10)       ? "2 players"  : "1 player",
            (((memory[0x1AE4] - 1) & 3) >= 2) ? "bounce off" : "pass through",
            (((memory[0x1AE4] - 1) & 7) >= 4) ? "has"        : "lacks"
        );
    acase A_COMBATPOS: // game is $18E6 (actual value rather than BCD)
    case A_COMBATODPOS:
        if (!force && t1 == memory[0x18E6])
        {   return;
        }
        t1 = memory[0x18E6];
        if (memory[0x18E6] >= 1 && memory[0x18E6] <= 72)
        {   sprintf
            (   thestring,
                "Tanks, %s %s %s missiles %s",
                (((memory[0x18E6] - 1) & 7) >= 4) ? "fast"  : "slow",
                (((memory[0x18E6] - 1) & 3) >= 2) ? "short" : "long",
                (((memory[0x18E6] - 1) & 3) >= 2) ? "bendy" : "strght",
                mines[((memory[0x18E6] - 1) / 8)]
            );
        } elif (memory[0x18E6] >= 73 && memory[0x18E6] <= 88)
        {   sprintf
            (   thestring,
                "Planes %c, %s %s %s missiles %s",
                (memory[0x18E6] >= 81) ? 'B' : 'A',
                (((memory[0x18E6] - 1) & 7) >= 4) ? "fast"  : "slow",
                (((memory[0x18E6] - 1) & 3) >= 2) ? "short" : "long",
                (((memory[0x18E6] - 1) & 3) >= 2) ? "bendy" : "strght",
                mines[1]
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase ESCAPEPOS: // game-1 is $18FB, opt-1 is $1AF4
        if (!force && t1 == memory[0x18FB] && t2 == memory[0x1AF4])
        {   return;
        }
        t1 = memory[0x18FB];
        t2 = memory[0x1AF4];
        sprintf
        (   thestring,
            "%d bullet(s), %d robots",
            memory[0x18FB] & 3,
            8 + ((memory[0x1AF4] & 3) * 4)
        );
    acase JUNGLERPOS: // game-1 is r5, opt-1 is r6
        if (!force && t1 == r[5] && t2 == r[6])
        {   return;
        }
        t1 = r[5];
        t2 = r[6];
        sprintf
        (   thestring,
            "%s mode, maze #%d, %d enemy/ies, %s speed",
            (r[5] <= 7) ? "Game" : "Demo",
            1 + (r[5] % 8),
            (r[6] >= 4) ? 2 : 1,
            ((r[6] & 3) >= 2) ? "fast" : "slow"
        );
    acase NIBBLEMENPOS: // game-1 is $18FB, opt-1 is $1AF5
    case SUPERGOBBLERPOS:
        if (!force && t1 == memory[0x18FB] && t2 == memory[0x1AF5])
        {   return;
        }
        t1 = memory[0x18FB];
        t2 = memory[0x1AF5];
        sprintf
        (   thestring,
            "%s, %s red ghost, %s",
            hypergates[memory[0x18FB] & 3],
            (memory[0x1AF5] > 3) ? "fast" : "slow",
            openings[memory[0x1AF5] & 3]
        );
    acase R2DTANKPOS: // game+$10 is $18EA, opt+$10 is $18E9
        if (!force && t1 == memory[0x18EA] && t2 == memory[0x18E9])
        {   return;
        }
        t1 = memory[0x18EA];
        t2 = memory[0x18E9];
        sprintf
        (   thestring,
            "%s, %d fences",
            (memory[0x18EA] >= 0x11 && memory[0x18EA] <= 0x13) ? (char*) numplayers[memory[0x18EA] - 0x11]      : "?",
            (memory[0x18E9] >= 0x11 && memory[0x18E9] <= 0x13) ?                  ((memory[0x18E9] - 0x11) * 2) : 0
        );
    acase ROBOTKILLERPOS: // game-1 is $18FB, opt-1 is $1AF4
        if (!force && t1 == memory[0x18FB] && t2 == memory[0x1AF4])
        {   return;
        }
        t1 = memory[0x18FB];
        t2 = memory[0x1AF4];
        sprintf
        (   thestring,
            "%d bullet(s), %s difficulty, %d robots",
             memory[0x18FB] & 3,
            (memory[0x1AF4] <= 3) ? "easy" : "hard",
            8 + ((memory[0x1AF4] & 3) * 4)
        );
    acase SUPERBUG1POS:
    case SUPERBUG2POS:
        if (!force && t1 == memory[0x1A0E]) // level-1 is $1A0E
        {   return;
        }
        t1 = memory[0x1A0E];
        sprintf
        (   thestring,
            "%d seconds per stage",
            50 - (memory[0x1A0E] * 10)
        );
    acase TANKSALOTPOS: // game-1 is $18FB, opt-1 is $1AF5
        if (!force && t1 == memory[0x18FB] && t2 == memory[0x1AF5])
        {   return;
        }
        t1 = memory[0x18FB];
        t2 = memory[0x1AF5];
        sprintf
        (   thestring,
            "%s warp man, %s gates, %d warp base positions",
            ((memory[0x18FB] & 3) <= 1) ? "Can" : "Can't",
            ((memory[0x18FB] & 7) <= 3) ? "has" : "lacks",
            ((memory[0x1AF5] & 3) <= 2) ? (4 - (memory[0x1AF5] & 3)) : 0
        );
    acase A_CAPTUREPOS: // level and limited time flag are $18E0
        if (!force && t1 == memory[0x18E0])
        {   return;
        }
        t1 = memory[0x18E0];
        if
        (   (memory[0x18E0] & 0x70) >= 0x10
         && (memory[0x18E0] & 0x70) <= 0x60
        )
        {   sprintf
            (   thestring,
                "%s, %d advantage(s), %s time",
                ((memory[0x18E0] & 0x70) == 0x60) ? "2 players" : "1 player",
                ((memory[0x18E0] & 0x70) == 0x60) ? 0           : (((memory[0x18E0] & 0x70) >> 4) - 1),
                ( memory[0x18E0] & 0x02         ) ? "limited"   : "unlimited"
            );
        } else
        {   strcpy(thestring, "?");
        }
    // INTERTON and ELEKTOR games-----------------------------------------
    acase AIRSEAATTACKPOS: // game is $1F5C (in BCD format)
        if (!force && t1 == memory[0x1F5C])
        {   return;
        }
        t1 = memory[0x1F5C];
        if
        (   (memory[0x1F5C] & 0xF0) <= 0x20
         && (memory[0x1F5C] & 0x0F) <= 0x09
        )
        {   val = (((memory[0x1F5C] & 0xF0) >> 4) * 10)
                +   (memory[0x1F5C] & 0x0F      )
                - 1;
            sprintf
            (   thestring,
                "%s, %s, %s",
                airseaattack[val / 4],
                features[    val    ],
                ((val & 1) || val == 8 || val == 10) ? "2 players" : "1 player"
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase AIRSEABATTLEPOS: // game is $1F5C (in BCD format)
        if (!force && t1 == memory[0x1F5C])
        {   return;
        }
        t1 = memory[0x1F5C];
        if
        (    memory[0x1F5C]         <= 0x20
         && (memory[0x1F5C] & 0x0F) <= 0x09
        )
        {   val = (((memory[0x1F5C] & 0xF0) >> 4) * 10)
                +   (memory[0x1F5C] & 0x0F      )
                - 1;
            sprintf
            (   thestring,
                "%s, %s, %s",
                airseabattle[val / 4],
                features[    val    ],
                ((val & 1) || val == 8 || val == 10) ? "2 players" : "1 player"
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase CARRACESPOS: // game-1 is $1F1E
    case GRANDPRIXPOS:
        if (!force && t1 == memory[0x1F1E])
        {   return;
        }
        t1 = memory[0x1F1E];
        if (memory[0x1F1E] <= 9)
        {   sprintf
            (   thestring,
                "%s, %s",
                carraces[memory[0x1F1E] / 2],
                (memory[0x1F1E] & 1) ? "1 player" : "2 players" 
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase HEADONPOS: // game-1 is $1F4E
        if (!force && t1 == memory[0x1F4E])
        {   return;
        }
        t1 = memory[0x1F4E];
        if (memory[0x1F4E] <= 8)
        {   sprintf
            (   thestring,
                "%s.",
                headon[memory[0x1F4E]]
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase I_HUNTINGPOS: // game is $1F5C (in BCD format)
    case E_HUNTINGPOS:
        if (!force && t1 == memory[0x1F5C])
        {   return;
        }
        t1 = memory[0x1F5C];
        if
        (    memory[0x1F5C]         <= 0x64
         && (memory[0x1F5C] & 0x0F) <= 0x09
        )
        {   val = (((memory[0x1F5C] & 0xF0) >> 4) * 10)
                +   (memory[0x1F5C] & 0x0F      )
                - 1;
            sprintf
            (   thestring,
                "%s, %s, %s fish, %d trgts, %s",
                hunting1[ val / 16],
                hunting2[(val % 16) / 2],
                (val % 16 <= 3) ? "no"         : ((val      <= 15) ? "right" : "lt+rt"),
                (val % 16 <= 1) ? 8            : ((val % 16 <=  3) ? 12      : 16            ),
                (val %  2     ) ? "2 players"  : "1 player"
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase _4INAROWPOS: // game-1 is $1F0E
        if (!force && t1 == memory[0x1F0E])
        {   return;
        }
        t1 = memory[0x1F0E];
        if (memory[0x1F0E] <= 13)
        {   sprintf
            (   thestring,
                "%s, %s, %s",
                (memory[0x1F0E] <= 6) ? "7*6"      : "8*6",
                (memory[0x1F0E] %  7) ? "1 player" : "2 players",
                challenge[memory[0x1F0E] % 7]
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase I_BOWLINGPOS: // game is $1F1E (in BCD format)
        if (!force && t1 == memory[0x1F1E])
        {   return;
        }
        t1 = memory[0x1F1E];
        if
        (   ( memory[0x1F1E]         <= 0x16)
         && ((memory[0x1F1E] & 0x0F) <= 0x09)
        )
        {   val = (((memory[0x1F1E] & 0xF0) >> 4) * 10)
                +   (memory[0x1F1E] & 0x0F      )
                - 1;
            strcpy(thestring, i_bowling[val]);
        } else
        {   strcpy(thestring, "?");
        }
    acase CASINOPOS: // game is $1F6A
        if (!force && t1 == memory[0x1F6A])
        {   return;
        }
        t1 = memory[0x1F6A];
        if (memory[0x1F6A] >= 1 && memory[0x1F6A] <= 3)
        {   strcpy(thestring, casino[memory[0x1F6A] - 1]);
        } else
        {   strcpy(thestring, "?");
        }
    acase I_CIRCUSPOS: // game-1 is lower nybble of $1F1E
        if (!force && t1 == (memory[0x1F1E] & 0x0F))
        {   return;
        }
        t1 = memory[0x1F1E] & 0x0F;
        sprintf
        (   thestring,
            "%s, %s, %s, %s",
            ( memory[0x1F1E]      & 0x08) ? "Non-moving" : "Moving",
            ( memory[0x1F1E]      & 0x04) ? "strips"     : "balloons",
            ( memory[0x1F1E]      & 0x02) ? "fast"       : "slow",
            ((memory[0x1F1E] + 1) & 0x02) ? "2 players"  : "1 player"
        );
    acase I_COMBATAPOS: // game is $1F0E (in BCD format)
    case I_COMBATBPOS:
        if (!force && t1 == memory[0x1F0E])
        {   return;
        }
        t1 = memory[0x1F0E];
        if
        (   ( memory[0x1F0E]         <= 0x26)
         && ((memory[0x1F0E] & 0x0F) <= 0x09)
        )
        {   val = (((memory[0x1F0E] & 0xF0) >> 4) * 10)
                +   (memory[0x1F0E] & 0x0F      )
                - 1;
            strcpy(thestring, i_combat[val]);
        } else
        {   strcpy(thestring, "?");
        }
    acase DRAUGHTSPOS: // game is $1F1E
        if (!force && t1 == memory[0x1F1E])
        {   return;
        }
        t1 = memory[0x1F1E];
        if (memory[0x1F1E] >= 1 && memory[0x1F1E] <= 15)
        {   strcpy(thestring, draughts[memory[0x1F1E] - 1]);
        } else
        {   strcpy(thestring, "?");
        }
    acase MASTERMINDPOS: // game is $1FA9 (in BCD format)
        if (!force && t1 == memory[0x1FA9])
        {   return;
        }
        t1 = memory[0x1FA9];
        if
        (   ( memory[0x1FA9]         <= 0x24)
         && ((memory[0x1FA9] & 0x0F) <= 0x09)
        )
        {   val = (((memory[0x1FA9] & 0xF0) >> 4) * 10)
                +   (memory[0x1FA9] & 0x0F      )
                - 1;
            sprintf
            (   thestring,
                "%s, %s duplicates, range 1-%d, %s",
                ( val < 12     ) ? "Figures"   : "Symbols",
                ((val % 12) < 6) ? "has"       : "lacks",
                ((val %  6) / 2) + 6, // 6..8
                ( val &  1     ) ? "2 players" : "1 player"
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase INVADERAPOS: // game-1 is $1F0F
    case INVADERBPOS:
        if (!force && t1 == memory[0x1F0F])
        {   return;
        }
        t1 = memory[0x1F0F];
        if (memory[0x1F0F] <= 15)
        {   sprintf
            (   thestring,
                "%s bases, %s sky, %s, %s missiles, %s",
                (memory[0x1F0F] & 8) ? "Lacks"     : "Has",
                invaders[memory[0x1F0F] / 4],
                (memory[0x1F0F] & 4) ? "fast"      : "slow",
                (memory[0x1F0F] & 2) ? "guided"    : "unguided",
                (memory[0x1F0F] & 1) ? "2 players" : "1 player"
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase MONSTERMANPOS: // game is $1F0F-1
        if (!force && t1 == memory[0x1F0F])
        {   return;
        }
        t1 = memory[0x1F0F];
        if (memory[0x1F0F] <= 11)
        {   sprintf
            (   thestring,
                "Spider can%s leave, %s, %s, %d insect(s)",
                (memory[0x1F0F] >= 4 && memory[0x1F0F] <= 7) ? "not" : "",
                monsterman[memory[0x1F0F] / 4],
                (memory[0x1F0F] & 2) ? "fast" : "slow",
                (((memory[0x1F0F] % 4) / 2) * 2) + 1
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase MUSICALGAMESPOS: // game-1 is $1F1E
        if (!force && t1 == memory[0x1F1E])
        {   return;
        }
        t1 = memory[0x1F1E];
        if (memory[0x1F1E] <= 7)
        {   strcpy(thestring, musicalgames[memory[0x1F1E]]);
        } else
        {   strcpy(thestring, "?");
        }
    acase PADDLEGAMESPOS: // game is $1FAD (in BCD format)
        if (!force && t1 == memory[0x1FAD])
        {   return;
        }
        t1 = memory[0x1FAD];
        if
        (   ( memory[0x1FAD]         <= 0x60)
         && ((memory[0x1FAD] & 0x0F) <= 0x09)
        )
        {   val = (((memory[0x1FAD] & 0xF0) >> 4) * 10)
                +   (memory[0x1FAD] & 0x0F      )
                - 1;
            sprintf
            (   thestring,
                "%s, %s, %s difficulty",
                (val >= 41 && val <= 50) ? "1 player" : "2 players",
                olympics1[val / 5],
                olympics2[val % 5]
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase PINBALLAPOS: // game-1 is $1F50
    case PINBALLBPOS:
        if (!force && t1 == memory[0x1F50])
        {   return;
        }
        t1 = memory[0x1F50];
        if (memory[0x1F50] <= 7)
        {   sprintf
            (   thestring,
                "%s flippers, %s, %s",
                (memory[0x1F50] & 0x04) ? "Double"    : "Single",
                (memory[0x1F50] & 0x02) ? "fast"      : "slow",
                (memory[0x1F50] & 0x01) ? "2 players" : "1 player"
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase PLANETDEFENDERPOS: // game is $1F50 (in BCD format)
        if (!force && t1 == memory[0x1F50])
        {   return;
        }
        t1 = memory[0x1F50];
        if
        (   ( memory[0x1F50]         <= 0x16)
         && ((memory[0x1F50] & 0x0F) <= 0x09)
        )
        {   val = (((memory[0x1F50] & 0xF0) >> 4) * 10)
                +   (memory[0x1F50] & 0x0F      )
                - 1;
            sprintf
            (   thestring,
                "%s msls, firing rate %d, %s cruise msls, %s trgt ctrl",
                planetdefender[val / 4],
                (val / 4) + 1,
                (val & 2) ? "smart" : "dumb",
                (val & 1) ? "fast"  : "slow"
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase SHOOTOUTPOS: // game-1 is $1F1E
        if (!force && t1 == memory[0x1F1E])
        {   return;
        }
        t1 = memory[0x1F1E];
        if (memory[0x1F1E] <= 23)
        {   sprintf
            (   thestring,
                "%s shots, can%s run after firing, %sdestructable %s",
                shootout[memory[0x1F1E] / 8],
                (memory[0x1F1E] & 4) ? ""      : "not",
                (memory[0x1F1E] & 2) ? ""      : "in",
                (memory[0x1F1E] & 1) ? "coach" : "cactus"
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase SPACEWARPOS: // game is (($1F67-1)*2)+1+$1F68
        if (!force && t1 == memory[0x1F67] && t2 == memory[0x1F68])
        {   return;
        }
        t1 = memory[0x1F67];
        t2 = memory[0x1F68];
        if
        (   memory[0x1F67] >= 1
         && memory[0x1F67] <= 4
         && memory[0x1F68] <= 1
        )
        {   val = ((memory[0x1F67] - 1) * 2) + memory[0x1F68];
            sprintf
            (   thestring,
                "%s, %s, %s",
                spacewar[val / 2],
                (val <= 1) ? "1 player" : "2 players",
                (val &  1) ? "fast"     : "slow"
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase SUPERINVPOS: // game is $1F6B (in BCD format)
        if (!force && t1 == memory[0x1F6B])
        {   return;
        }
        t1 = memory[0x1F6B];
        if
        (   ( memory[0x1F6B]         <= 0x15)
         && ((memory[0x1F6B] & 0x0F) <= 0x09)
        )
        {   val = (((memory[0x1F6B] & 0xF0) >> 4) * 10)
                +   (memory[0x1F6B] & 0x0F      )
                - 1;
            sprintf
            (   thestring,
                "%s scoring, %s, %s, %s",
                (val <= 11) ? "High"    : "Low",
                (val <=  5) ? "slow"    : "fast",
                superinvaders[val],
                (val &   1) ? "2 plyrs" : "1 plyr"
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase SUPERKNOCKOUTPOS: // game-1 is $1F5A
        if (!force && t1 == memory[0x1F5A])
        {   return;
        }
        t1 = memory[0x1F5A];
        if (memory[0x1F5A] <= 127)
        {   sprintf
            (   thestring,
                "%d bls, %s w-out, %s obstcls, %s inv wl, %s ctch-it, %s pk-brk, %s",
                (memory[0x1F5A] & 0x40) ? 10        : 5,
                (memory[0x1F5A] & 0x20) ? "has"     : "lks",
                (memory[0x1F5A] & 0x10) ? "has"     : "lks",
                (memory[0x1F5A] & 0x08) ? "has"     : "lks",
                (memory[0x1F5A] & 0x04) ? "has"     : "lks",
                (memory[0x1F5A] & 0x02) ? "has"     : "lks",
                (memory[0x1F5A] & 0x01) ? "2 plyrs" : "1 plyr"
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase SUPERMAZEPOS: // game-1 is $1F0F
        if (!force && t1 == memory[0x1F0F])
        {   return;
        }
        t1 = memory[0x1F0F];
        if (memory[0x1F0F] <= 23)
        {   sprintf
            (   thestring,
                "%s, %s, %s",
                supermaze[memory[0x1F0F] / 4],
                (memory[0x1F0F] & 2) ? "fast"      : "slow",
                (memory[0x1F0F] & 1) ? "2 players" : "1 player"
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase MEMORY1POS: // game is $1F1F (in BCD format)
        if (!force && t1 == memory[0x1F1F])
        {   return;
        }
        t1 = memory[0x1F1F];
        if
        (   ( memory[0x1F1F]         <= 0x16)
         && ((memory[0x1F1F] & 0x0F) <= 0x09)
        )
        {   val = (((memory[0x1F1F] & 0xF0) >> 4) * 10)
                +   (memory[0x1F1F] & 0x0F      )
                - 1;
            sprintf
            (   thestring,
                "%s, %s flag position, %d squares, %s",
                treasurehunt[val / 4],
                (val & 4) ? "variable"  : "fixed",
                (val & 2) ? 56          : 30,
                (val & 1) ? "2 players" : "1 player"
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase WINTERSPORTSPOS: // game-1 is $1F6B
        if (!force && t1 == memory[0x1F6B])
        {   return;
        }
        t1 = memory[0x1F6B];
        if (memory[0x1F6B] <= 7)
        {   strcpy(thestring, wintersports[memory[0x1F6B]]);
        } else
        {   strcpy(thestring, "?");
        }
    acase I_MATH1POS: // players and digits (and rows) are $1FAD, difficulty and operations and rows are $1FAC
    case E_MATH1POS:
        if (!force && t1 == memory[0x1FAD] && t2 == memory[0x1FAC])
        {   return;
        }
        t1 = memory[0x1FAD];
        t2 = memory[0x1FAC];
        sprintf
        (   thestring,
            "%s, ",
            (memory[0x1FAD] & 0x80) ? "2 players" : "1 player"
        );
        if
        (   (memory[0x1FAC] >=  4 && memory[0x1FAC] <=  5)
         || (memory[0x1FAC] >=  8 && memory[0x1FAC] <= 11)
         || (memory[0x1FAC] >= 16 && memory[0x1FAC] <= 23)
        )
        {   strcat(thestring, "easy, ");
        } elif
        (   (memory[0x1FAC] >=  6 && memory[0x1FAC] <=  7)
         || (memory[0x1FAC] >= 12 && memory[0x1FAC] <= 15)
         || (memory[0x1FAC] >= 24 && memory[0x1FAC] <= 31)
        )
        {   strcat(thestring, "hard, ");
        } else
        {   strcat(thestring, "?, ");
        }
        switch (memory[0x1FAD] & 0x7F) // 'Cl' key
        {
        case  0x23: case 0x2B: case 0x33: digits =   "1"; //   1+  1+  1+  1,   1+  1+  1,   1+  1
        acase 0x22: case 0x2A: case 0x32: digits =  "22"; //  22+ 22+ 22+ 22,  22+ 22+ 22,  22+ 22
        acase 0x21: case 0x29: case 0x31: digits = "333"; // 333+333+333+333, 333+333+333, 333+333
        adefault:                         digits =   "?";
        }
        switch (memory[0x1FAC]) // '0' key
        {
        case   4: case  6: sprintf(ENDOF(thestring), "%s+%s"      , digits, digits);
        acase  5: case  7: sprintf(ENDOF(thestring), "%s-%s"      , digits, digits);
        acase  8: case 12: sprintf(ENDOF(thestring), "%s+%s+%s"   , digits, digits, digits);
        acase  9: case 13: sprintf(ENDOF(thestring), "%s+%s-%s"   , digits, digits, digits);
        acase 10: case 14: sprintf(ENDOF(thestring), "%s-%s+%s"   , digits, digits, digits);
        acase 11: case 15: sprintf(ENDOF(thestring), "%s-%s-%s"   , digits, digits, digits);
        acase 16: case 24: sprintf(ENDOF(thestring), "%s+%s+%s+%s", digits, digits, digits, digits);
        acase 17: case 25: sprintf(ENDOF(thestring), "%s+%s+%s-%s", digits, digits, digits, digits);
        acase 18: case 26: sprintf(ENDOF(thestring), "%s+%s-%s+%s", digits, digits, digits, digits);
        acase 19: case 27: sprintf(ENDOF(thestring), "%s+%s-%s-%s", digits, digits, digits, digits);
        acase 20: case 28: sprintf(ENDOF(thestring), "%s-%s+%s+%s", digits, digits, digits, digits);
        acase 21: case 29: sprintf(ENDOF(thestring), "%s-%s+%s-%s", digits, digits, digits, digits);
        acase 22: case 30: sprintf(ENDOF(thestring), "%s-%s-%s+%s", digits, digits, digits, digits);
        acase 23: case 31: sprintf(ENDOF(thestring), "%s-%s-%s-%s", digits, digits, digits, digits);
        adefault:          strcat(thestring, "?");
        }
    acase I_MATH2POS:
        if (!force && t1 == memory[0x1FAD] && t2 == memory[0x1FAC])
        {   return;
        }
        t1 = memory[0x1FAD];
        t2 = memory[0x1FAC];
        sprintf
        (   thestring,
            "%s, ",
            (memory[0x1FAD] & 0x80) ? "2 players" : "1 player"
        );
        switch ((memory[0x1FAC] * 256) + (memory[0x1FAD] & 0x7F))
        {
        case  0x0431: strcat(thestring, "333*333");
        acase 0x0432: strcat(thestring, "22*22");
        acase 0x0433: strcat(thestring, "1*1");
        acase 0x0531: strcat(thestring, "333/333");
        acase 0x0532: strcat(thestring, "333/22");
        acase 0x0533: strcat(thestring, "22/1");
        acase 0x0629: strcat(thestring, "333+333*333");
        acase 0x062A: strcat(thestring, "22+22*22");
        acase 0x062B: strcat(thestring, "1+1*1");
        acase 0x0729: strcat(thestring, "333-333*333");
        acase 0x072A: strcat(thestring, "22-22*22");
        acase 0x072B: strcat(thestring, "1-1*1");
        acase 0x0829: strcat(thestring, "333*333+333");
        acase 0x082A: strcat(thestring, "22*22+22");
        acase 0x082B: strcat(thestring, "1*1+1");
        acase 0x0929: strcat(thestring, "333*333-333");
        acase 0x092A: strcat(thestring, "22*22-22");
        acase 0x092B: strcat(thestring, "1*1-1");
        acase 0x0A29: strcat(thestring, "333/333+333");
        acase 0x0A2A: strcat(thestring, "333/22+22");
        acase 0x0A2B: strcat(thestring, "22/1+1");
        acase 0x0B29: strcat(thestring, "333/333-333");
        acase 0x0B2A: strcat(thestring, "333/22-22");
        acase 0x0B2B: strcat(thestring, "22/1-1");
        acase 0x0C29: strcat(thestring, "333/333*333");
        acase 0x0C2A: strcat(thestring, "333/22*22");
        acase 0x0C2B: strcat(thestring, "22/1*1");
        acase 0x0D21: strcat(thestring, "333*333+333+333");
        acase 0x0D22: strcat(thestring, "22*22+22+22");
        acase 0x0D23: strcat(thestring, "1*1+1+1");
        acase 0x0E21: strcat(thestring, "333*333+333-333");
        acase 0x0E22: strcat(thestring, "22*22+22-22");
        acase 0x0E23: strcat(thestring, "1*1+1-1");
        acase 0x0F21: strcat(thestring, "333*333-333-333");
        acase 0x0F22: strcat(thestring, "22*22-22-22");
        acase 0x0F23: strcat(thestring, "1*1-1-1");
        acase 0x1021: strcat(thestring, "333/333+333+333");
        acase 0x1022: strcat(thestring, "333/22+22+22");
        acase 0x1023: strcat(thestring, "22/1+1+1");
        acase 0x1121: strcat(thestring, "333/333+333-333");
        acase 0x1122: strcat(thestring, "333/22+22-22");
        acase 0x1123: strcat(thestring, "22/1+1-1");
        acase 0x1221: strcat(thestring, "333/333+333*333");
        acase 0x1222: strcat(thestring, "333/22+22*22");
        acase 0x1223: strcat(thestring, "22/1+1*1");
        acase 0x1321: strcat(thestring, "333/333-333+333");
        acase 0x1322: strcat(thestring, "333/22-22+22");
        acase 0x1323: strcat(thestring, "22/1-1+1");
        acase 0x1421: strcat(thestring, "333/333-333*333");
        acase 0x1422: strcat(thestring, "333/22-22*22");
        acase 0x1423: strcat(thestring, "22/1-1*1");
        adefault:     strcat(thestring, "?");
        }
    acase SHOOTGALPOS: // game is $1F5C (in BCD format)
        if (!force && t1 == memory[0x1F5C])
        {   return;
        }
        t1 = memory[0x1F5C];
        if
        (    memory[0x1F5C]         <= 0x32
         && (memory[0x1F5C] & 0x0F) <= 0x09
        )
        {   val = (((memory[0x1F5C] & 0xF0) >> 4) * 10)
                +   (memory[0x1F5C] & 0x0F      )
                - 1;
            sprintf
            (   thestring,
                "%s, %s, %s fish, %d targets, %s",
                hunting1[ val / 8],
                shootgal[(val % 8) / 2],
                (val % 8 <= 1) ? "no" : ((val <= 7) ? "right" : "lt+rt"),
                (val % 8 <= 1) ? 8    : 16,
                (val % 2     ) ? "2 players" : "1 player"
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase BACKGAMMONPOS: // game is $1004 ("1".."3" are $01..$03, "4" is $00)
        if (!force && t1 == memory[0x1004])
        {   return;
        }
        t1 = memory[0x1004];
        if (memory[0x1004] <= 3)
        {   strcpy(thestring, backgammon[memory[0x1004]]);
        } else
        {   strcpy(thestring, "?");
        }
    acase I_CAPTUREPOS: // game is $1F6D ("01".."93" are $00..$1D)
        if (!force && t1 == memory[0x1F6D])
        {   return;
        }
        t1 = memory[0x1F6D];
        if (memory[0x1F6D] <= 0x1D)
        {   sprintf
            (   thestring,
                "%s time, %d advantage(s), %s",
                (memory[0x1F6D] <= 14) ? "Unlimited" : "Limited",
                (memory[0x1F6D] % 15) / 3,
                i_capture[memory[0x1F6D] % 3]
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase CHESS1POS: // intelligence is high nybble of $13D1, game is high nybble of $13D2
        if (!force && t1 == memory[0x13D1] && t2 == memory[0x13D2])
        {   return;
        }
        t1 = memory[0x13D1];
        t2 = memory[0x13D2];
        if
        (   (memory[0x13D1] & 0xF0) >= 0x10
         && (memory[0x13D1] & 0xF0) <= 0x60
         && (memory[0x13D2] & 0xF0) >= 0x10
         && (memory[0x13D2] & 0xF0) <= 0x40
        )
        {   sprintf
            (   thestring,
                "Intelligence of %d, %s mode",
                       (memory[0x13D1] & 0xF0) >> 4,
                chess[((memory[0x13D2] & 0xF0) >> 4) - 1]
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase CHESS2POS: // intelligence-1 is low nybble of $1B92, game is high nybble of $1B93
        if (!force && t1 == memory[0x1B92] && t2 == memory[0x1B93])
        {   return;
        }
        t1 = memory[0x1B92];
        t2 = memory[0x1B93];
        if
        (   (memory[0x1B92] & 0x0F) <= 0x08
         && (memory[0x1B93] & 0xF0) >= 0x10
         && (memory[0x1B93] & 0xF0) <= 0x40
        )
        {   sprintf
            (   thestring,
                "Intelligence of %d, %s mode",
                        (memory[0x1B92] & 0x0F)       + 1,
                chess[((memory[0x1B93] & 0xF0) >> 4) - 1]
            );
        } else
        {   strcpy(thestring, "?");
        }
    acase I_HORSERACINGPOS: // game is $1F65 ("1".."16" are $C1..$D0)
    case HIPPODROMEPOS:
        if (!force && t1 == memory[0x1F65])
        {   return;
        }
        t1 = memory[0x1F65];
        if (memory[0x1F65] >= 0xC1 && memory[0x1F65] <= 0xD0)
        {   sprintf
            (   thestring,
                "%s move, %s, %s",
                 (memory[0x1F65] <= 0xC8)      ? "Horses"   : "Obstacles",
                horseracing[((memory[0x1F65] - 0xC1) % 8) / 2],
                ((memory[0x1F65] -  0xC1) & 1) ? "1 player" : "2 players"
            );
        } else
        {   strcpy(thestring, "?");
        }
    adefault:
        if (!force)
        {   return;
        }
        strcpy(thestring, "-");
    }

    // zprintf(TEXTPEN_VERBOSE, "Variant text length is %d.\n", strlen(thestring));
    st_set2(SUBWINDOW_GAMEINFO, IDC_AUTOSENSE9, thestring);
}
