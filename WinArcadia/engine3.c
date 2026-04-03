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
               outofrange,
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
               diskbyte,
               drive_mode,
               lastcodecomment          = -1,
               master_x, master_y,
               slave_x, slave_y,
               n1, n2, n3, n4,
               palettepen               = 0,
               pvibase,
               queuepos                 = 0,
               rastn                    = INVALIDRASTER,
               runtorastline            = FALSE,
               runtointerrupt           = FALSE,
               runtoloopend             = FALSE,
               tt_kybdstate,
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
                                      binbug_biosver,
                                      cd2650_biosver,
                                      colourset,
                                      cpl,
                                      cpux,
                                      cpuy,
                                      drawmode,
                                      editscreen,
                                      exactspeed,
                                      filesize,
                                      flagline,
                                      frac[4],
                                      fullscreen,
                                      game,
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
                                      pipbug_biosver,
                                      pipbug_vdu,
                                      phunsy_biosver,
                                      ponghertz,
                                      post,
                                      queuekeystrokes,
                                      recmode,
                                      regionstart,
                                      rotate,
                                      rotating,
                                      selbst_biosver,
                                      showdebugger[2],
                                      showleds,
                                      showstatusbars[2],
                                      size,
                                      slice_2650,
                                      speedup,
                                      spriteflip,
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
                                      verbosity,
                                      viewingsprite,
                                      wheremusicmouse[2],
                                      whichcpu,
                                      whichgame,
                                      whichkeyrect,
                                      whichoverlay,
                                      wide,
                                      wsm;
IMPORT       FLAG                     bangercharging,
                                      priflag[32],
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
                                      sp;
IMPORT       ULONG                    arcadia_bigctrls,
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
                                      viewmemas2;
IMPORT       char                     mn[80 + 1];
IMPORT       double                   samplewhere_f;
IMPORT       TEXT                     asciiname_short[259][3 + 1],
                                      asciiname_long[259][9 + 1],
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
IMPORT       struct VariantStruct     variantinfo[VARIANTS];
IMPORT const TEXT                     arcadia_chars[64 + 1],
                                      elektor_chars1[256 + 1],
                                      pswbit[STYLES][9 + 1];
IMPORT const UBYTE                    arcadia_pdg[2][64][8],
                                      dg640_chars[128][DG640_CHARHEIGHT],
                                      from_a[16],
                                      phunsy_chars[128][8],
                                      phunsy_gfx[16][8],
                                      table_opcolours_2650[2][256],
                                      tr_chars[91][7];
IMPORT const UWORD                    pvi_spritedata[4];
IMPORT const ULONG                    defpencolours[COLOURSETS][GUESTCOLOURS];
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
    IMPORT       LONG                  emupens[EMUBRUSHES];
    IMPORT       ULONG                 longpens[PENS],
                                       pending;
    IMPORT       int                   crippled,
                                       foundgames,
                                       morphos,
                                       pipbug_vdu,
                                       showsidebars[2],
                                       throb;
    IMPORT       UBYTE                *canvasbyteptr[CANVASES][CANVASHEIGHT],
                                      *canvasdisplay[CANVASES];
    IMPORT       struct Catalog*       CatalogPtr;
    IMPORT       struct ClassLibrary*  ClockBase;
    IMPORT       struct IntuitionBase* IntuitionBase;
    IMPORT       struct Window        *MagnifierWindowPtr,
                                      *MainWindowPtr,
                                      *SubWindowPtr[SUBWINDOWS];
    IMPORT       struct PaletteStruct  pencolours[COLOURSETS][PENS];
    IMPORT       struct Gadget        *gadgets[GIDS + 1],
                                      *kybdgad[KYBDGADS];
    IMPORT       struct Image*         images[IMAGES];
    IMPORT       struct RastPort       wpa8canvasrastport[CANVASES];
    IMPORT const int                   guest_to_emupen[8];
#endif
#ifdef WIN32
    IMPORT       FLAG                  capslock,
                                       incli,
                                       repaintmemmap;
    IMPORT       int                   bezel,
                                       CatalogPtr, // APTR doesn't work
                                       cheevos;
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
                                       MainWindowPtr,
                                       SubWindowPtr[SUBWINDOWS];
    IMPORT       UINT                  storedcode;
    IMPORT       ULONG                 pencolours[COLOURSETS][PENS];
    IMPORT       HINSTANCE             InstancePtr;
    IMPORT const struct MemMapToStruct memmap_to[MEMMAPS];
    IMPORT       ULONG*                canvasdisplay[CANVASES];
IMPORT       struct
{   BITMAPINFOHEADER Header;
    DWORD            Colours[3];
} CanvasBitMapInfo[CANVASES];
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

// MODULE ARRAYS----------------------------------------------------------

(None)

MODULE FUNCTIONS------------------------------------------------------- */

MODULE void calcrunningtime(void);
MODULE void tape_foundcolon(UBYTE value);
MODULE void tape_loaded(void);
MODULE void tape_read(UBYTE value);
MODULE void tape_rawread(UBYTE value);
MODULE void tape_rawstart(void);
MODULE void tape_rawreadbit(void);
MODULE void tape_wrotecolon(TEXT colon);
MODULE void tape_wrotebyte(UBYTE value);

// music subwindow support
MODULE void ghost_notes(void);

#ifdef AMIGA
    MODULE FLAG alt(void);
#endif

// CODE-------------------------------------------------------------------

EXPORT void changemachine(int whichmachine, int whichmemmap, FLAG user, FLAG force, FLAG same)
{   int  i,
         oldwide,
         oldwidth, oldheight,
         x, y;
    FLAG resizeit = FALSE;
#ifdef WIN32
    int  consoleid,
         oldmemmap;
    FLAG oldcheevos;

    if (cheevos && force)
    {
#ifdef LOGCHEEVOS
        zprintf(TEXTPEN_VERBOSE, "RA_ConfirmLoadNewRom(FALSE)\n");
#endif
        if (!RA_ConfirmLoadNewRom(FALSE))
        {   return;
    }   }
#endif

    if (whichgame == 65535)
    {   whichgame = -1; // important!
    }

    malzak_savenvram();
    if (memmap != whichmemmap || force)
    {   macro_stop();
        sound_off(FALSE);
        ff_off();
        printer_savepartial(0);
        printer_savepartial(1);

        drawmode = 0;

        oldwidth  = machines[machine].width;
        oldheight = machines[machine].height;
        oldwide   = wide;

        machine   = whichmachine;
#ifdef WIN32
        oldmemmap = memmap;
#endif
        memmap    = whichmemmap;
        if (machine != ARCADIA && machine != PONG)
        {   region = (ULONG) machines[machine].region;
        }
        if (user)
        {   wide = machines[machine].wide;
        }

        calc_margins(); // this sets machines[machine].width and machines[machine].height

        if
        (   machines[machine].width  != oldwidth
         || machines[machine].height != oldheight
         || wide                     != oldwide
#ifdef WIN32
         || (   BEZELABLE
             && bezel
             && machine == ZACCARIA
             && memmap != oldmemmap
            ) // they have different-sized bezels
#endif
        )
        {   resizeit = TRUE;
    }   }

#ifdef WIN32
    oldcheevos = cheevos;
    if (machine != ARCADIA && machine != INTERTON && machine != ELEKTOR)
    {   cheevos = FALSE;
    }
#endif

    if (user || force)
    {   filesize  =  0;
        whichgame = -1;
        if (usestubs)
        {   switch (machine)
            {
            case  ARCADIA:  whichgame  = ARCADIASTUBPOS;
            acase INTERTON: whichgame  = INTERTONSTUBPOS;
        }   }
        configure();
        generate_autotext();
    }

    for (x = 0; x < BOXWIDTH; x++)
    {   for (y = 0; y < BOXHEIGHT; y++)
        {   screen[x][y] = GREY1;
    }   }

    for (i = 0; i < 258; i++)
    {   coverage_io[i] = 0;
    }
    for (i = 0; i < 32768; i++)
    {   asicreads[i]  =
        asicwrites[i] = 0;
    }
    clearcoverage();

    if (machine != BINBUG && machine != CD2650 && machine != TWIN)
    {   drive[0].inserted = FALSE;
    }
    if (machine != TWIN)
    {   drive[1].inserted = FALSE;
    }

    edit_screen_sanitize();

    if (memmapinfo[memmap].rotate)
    {   rotating = rotate ? TRUE : FALSE;
    } else
    {   rotating = FALSE;
    }
    drawpixelroutine();

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

    if (MainWindowPtr && !same)
    {   close_subwindows(FALSE); // must be done before foo_setmemmap()!
        if (resizeit)
        {   resize(size, FALSE);
        }
        // Don't call redrawscreen() here because we have changed
        // machines[machine].width/height, etc. but not yet closed/
        // reopened the window with the new dimensions.
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

#ifdef WIN32
    bigicon = LoadImage(InstancePtr, MAKEINTRESOURCE(memmap_to[memmap].icon           ), IMAGE_ICON, 32, 32, 0);
    smlicon = LoadImage(InstancePtr, MAKEINTRESOURCE(getsmallimage1(whichgame, memmap)), IMAGE_ICON, 16, 16, 0);
    if (MainWindowPtr)
    {   SendMessage(MainWindowPtr, WM_SETICON, ICON_BIG,   (LPARAM) bigicon);
        SendMessage(MainWindowPtr, WM_SETICON, ICON_SMALL, (LPARAM) smlicon);
    }
#endif

    if (user)
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

    glow = 0;
    if (user || force)
    {   engine_reset();
    }
    draw_margins();
    generate_autotext();
    printers_reset();
    make_stars(); // this can't be done until sourcewidth/sourceheight are set properly
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
        if (force)
        {
#ifdef LOGCHEEVOS
            zprintf(TEXTPEN_VERBOSE, "RA_ActivateGame(0)\n");
#endif
            RA_ActivateGame(0);
    }   }
    elif (oldcheevos)
    {   remove_cheevos(TRUE);
    }
    updatemenu(MENUITEM_CHEEVOS2);
#endif
}

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
    rc = parse_bytes(0);
    if (rc != 3)
    {   free_iobuffer();
        if (rc != 1)
        {   return rc; // 0 (failure) or 2 (success but don't reset, eg. 8SVX/AIFF/ASM/BMP/IMAG/IMG/MDCR/RAW/TWIN/WAV)
    }   }

    // updatemenus(); is already done by parse_bytes()
    // updatesmlgads(); is already done by parse_bytes()
    updatebiggads();
    if (SubWindowPtr[SUBWINDOW_DIPS])
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

    if (!SubWindowPtr[SUBWINDOW_MONITOR_CPU])
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
                SetGadgetAttrs(gadgets[GID_CP_CL1], SubWindowPtr[SUBWINDOW_MONITOR_CPU], NULL, CLOCKGA_Time, secs, TAG_DONE); // this autorefreshes
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
            {   TRString[3][1] = (TEXT) '¶';
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
            {   TRString[3][ideallength] = (TEXT) '¶';
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
        tracecpu_2650(FALSE, TRUE);
        setmonitorbutton(SUBWINDOW_MONITOR_CPU, &mn[8],                      MONGAD_OPCODE);
#ifdef AMIGA
    #ifdef MONOPCODECOLOURS
        SetGadgetAttrs
        (   monitor[MONGAD_OPCODE].gadget, SubWindowPtr[SUBWINDOW_MONITOR_CPU], NULL,
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
        {   SendMessage(GetDlgItem(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDC_MASTERGLYPH), STM_SETIMAGE, IMAGE_ICON, (LPARAM) cpuicon[1]);
            SendMessage(GetDlgItem(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDC_SLAVEGLYPH ), STM_SETIMAGE, IMAGE_ICON, (LPARAM) cpuicon[0]);
        } else
        {   SendMessage(GetDlgItem(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDC_MASTERGLYPH), STM_SETIMAGE, IMAGE_ICON, (LPARAM) cpuicon[0]);
            SendMessage(GetDlgItem(SubWindowPtr[SUBWINDOW_MONITOR_CPU], IDC_SLAVEGLYPH ), STM_SETIMAGE, IMAGE_ICON, (LPARAM) cpuicon[1]);
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
        DrawImage(SubWindowPtr[SUBWINDOW_MONITOR_CPU]->RPort, images[IMAGE_GLYPH_TWIN_DIM ], 0, 0);
        DrawImage(SubWindowPtr[SUBWINDOW_MONITOR_CPU]->RPort, images[IMAGE_GLYPH_TWIN_GLOW], 0, 0);
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
        setmonitorbutton(SUBWINDOW_MONITOR_CPU,          &mn[8],                      MONGAD_SLAVE_OPCODE);
#ifdef AMIGA
    #ifdef MONOPCODECOLOURS
        SetGadgetAttrs
        (   monitor[MONGAD_SLAVE_OPCODE].gadget, SubWindowPtr[SUBWINDOW_MONITOR_CPU], NULL,
            BUTTON_BackgroundPen, (ULONG) emupens[guest_to_emupen[table_opcolours_2650[supercpu][memory[tt]]]],
        TAG_DONE); // button gadgets autorefresh themselves
    #endif
#endif
}   }

EXPORT void update_monitor_psgs(FLAG force)
{   FAST int   i;
    FAST UWORD newval;

    if (!SubWindowPtr[SUBWINDOW_MONITOR_PSGS])
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
        }   }
        else
        {   if (memory[monitor[i].addr] != monitor[i].contents || force)
            {   monitor[i].contents = memory[monitor[i].addr];
                hex1((STRPTR) monitor[i].string, (UBYTE) monitor[i].contents);
                monitor[i].string[2] = EOS;
                setmonitorbutton(SUBWINDOW_MONITOR_PSGS, (STRPTR) monitor[i].string, i);
}   }   }   }

EXPORT void update_monitor_xvi(FLAG force)
{   FAST int i;

    if (!SubWindowPtr[SUBWINDOW_MONITOR_XVI])
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
        }   }
        if (machines[machine].pvis >= 2)
        {   for (i = PVI2ND_FIRSTMONGAD; i <= PVI2ND_LASTMONGAD; i++) // 2nd PVI
            {   if (memory[pvibase + monitor[i].addr] != monitor[i].contents || force)
                {   monitor[i].contents = memory[pvibase + monitor[i].addr];
                    hex1((STRPTR) monitor[i].string, (UBYTE) monitor[i].contents);
                    setmonitorbutton(SUBWINDOW_MONITOR_XVI, (STRPTR) monitor[i].string, i);
            }   }
            if (machines[machine].pvis >= 3)
            {   for (i = PVI3RD_FIRSTMONGAD; i <= PVI3RD_LASTMONGAD; i++) // 3rd PVI
                {   if (memory[pvibase + monitor[i].addr] != monitor[i].contents || force)
                    {   monitor[i].contents = memory[pvibase + monitor[i].addr];
                        hex1((STRPTR) monitor[i].string, (UBYTE) monitor[i].contents);
                        setmonitorbutton(SUBWINDOW_MONITOR_XVI, (STRPTR) monitor[i].string, i);
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
        }   }
    acase INTERTON:
        for (i = I_FIRSTMONGAD; i <= IE_LASTMONGAD; i++)
        {   if (memory[monitor[i].addr] != monitor[i].contents || force)
            {   monitor[i].contents = memory[monitor[i].addr];
                hex1((STRPTR) monitor[i].string, (UBYTE) monitor[i].contents);
                setmonitorbutton(SUBWINDOW_MONITOR_XVI, (STRPTR) monitor[i].string, i);
        }   }
        for (i = INTERTON_FIRSTMONGAD; i <= INTERTON_LASTMONGAD; i++)
        {   if (memory[monitor[i].addr] != monitor[i].contents || force)
            {   monitor[i].contents = memory[monitor[i].addr];
                hex1((STRPTR) monitor[i].string, (UBYTE) monitor[i].contents);
                setmonitorbutton(SUBWINDOW_MONITOR_XVI, (STRPTR) monitor[i].string, i);
        }   }
    acase ELEKTOR:
        for (i = E_FIRSTMONGAD; i <= IE_LASTMONGAD; i++)
        {   if (memory[monitor[i].addr] != monitor[i].contents || force)
            {   monitor[i].contents = memory[monitor[i].addr];
                hex1((STRPTR) monitor[i].string, (UBYTE) monitor[i].contents);
                setmonitorbutton(SUBWINDOW_MONITOR_XVI, (STRPTR) monitor[i].string, i);
    }   }   }

    do_axes();
}

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

    if (!SubWindowPtr[SUBWINDOW_SPRITES])
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
        {   sprviewcolour = from_a[7 - sprviewcolour];
        } else
        {   sprviewcolour = from_a[    sprviewcolour];
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
        {   SetGadgetAttrs(gadgets[GID_SR_PL1], SubWindowPtr[SUBWINDOW_SPRITES], NULL, PALETTE_Colour, sprviewcolour, GA_Disabled, colourable ? FALSE : TRUE, TAG_DONE); // this refreshes automatically
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
        {   DISCARD RedrawWindow(GetDlgItem(SubWindowPtr[SUBWINDOW_SPRITES], IDC_COLOUR0 + i), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }   }

    // clear_preview(SubWindowPtr[SUBWINDOW_SPRITES]);
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
        {   sprviewcolour = from_a[7 - newcolour];
        } else
        {   sprviewcolour = from_a[    newcolour];
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

    if (!SubWindowPtr[SUBWINDOW_MUSIC])
    {   return;
    }

    for (i = 0; i < 7; i++)
    {   newpsgnote[i] = NOTE_REST;
    }

#ifdef WIN32
    MusicRastPtr = GetDC(GetDlgItem(SubWindowPtr[SUBWINDOW_MUSIC], IDC_MUSIC));
    StaveRastPtr = GetDC(GetDlgItem(SubWindowPtr[SUBWINDOW_MUSIC], IDC_STAVE));
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
    ReleaseDC(GetDlgItem(SubWindowPtr[SUBWINDOW_MUSIC], IDC_MUSIC), MusicRastPtr);
    ReleaseDC(GetDlgItem(SubWindowPtr[SUBWINDOW_MUSIC], IDC_STAVE), StaveRastPtr);
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
    (   !SubWindowPtr[SUBWINDOW_MONITOR_XVI]
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

    redraw_axes();
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
        {   DRAWAXESPIXEL(nowx, nowy, colour);
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
        {   DRAWAXESPIXEL(nowx, nowy, colour);
            if (err >= 0)
            {   err -= dy2;
                nowx += ix;
            }
            err += dx2;
            nowy += iy;
    }   }

    // draw knob
    //                           DRAWAXESPIXEL(x2    , y2    , colour); // centre
    if (x2 > 0)
    {                            DRAWAXESPIXEL(x2 - 1, y2    , colour); // west
        if (y2 > 0)              DRAWAXESPIXEL(x2 - 1, y2 - 1, colour); // northwest
        if (y2 < AXESHEIGHT - 1) DRAWAXESPIXEL(x2 - 1, y2 + 1, colour); // southwest
    }
    if (x2 < AXESWIDTH - 1)
    {                            DRAWAXESPIXEL(x2 + 1, y2    , colour); // east
        if (y2 > 0)              DRAWAXESPIXEL(x2 + 1, y2 - 1, colour); // northeast
        if (y2 < AXESHEIGHT - 1) DRAWAXESPIXEL(x2 + 1, y2 + 1, colour); // southeast
    }
    if (y2 > 0)                  DRAWAXESPIXEL(x2    , y2 - 1, colour); // north
    if (y2 < AXESHEIGHT - 1)     DRAWAXESPIXEL(x2    , y2 + 1, colour); // south
}

EXPORT int getsmallimage2(int thegame, int thememmap)
{   switch (thegame)
    {
    case  _3DATTACKPOS:                              return IMAGE_FIRSTGAME +  0;
    acase _3DSOCCERAPOS:     case _3DSOCCERBPOS:     return IMAGE_FIRSTGAME +  1;
    acase HOMERUNPOS:                                return IMAGE_FIRSTGAME +  2;
    acase ALIENINVPOS:       case ALIENINV1POS:      case ALIENINV2POS: case ALIENINV3POS: case ALIENINV4POS:
                                                     return IMAGE_FIRSTGAME +  3;
    acase ASTROINVPOS:       case ASTROINVODPOS:     return IMAGE_FIRSTGAME +  4;
    acase AUTORACEPOS:       case AUTORACEODPOS:     return IMAGE_FIRSTGAME +  5;
    acase BASEBALLPOS:                               return IMAGE_FIRSTGAME +  6;
    acase A_BASKETBALLPOS:                           return IMAGE_FIRSTGAME +  7;
    acase BATTLEPOS:                                 return IMAGE_FIRSTGAME +  8;
    acase A_BLACKJACKPOS:                            return IMAGE_FIRSTGAME +  9;
    acase A_BOWLINGPOS:                              return IMAGE_FIRSTGAME + 10;
    acase A_BOXINGPOS:                               return IMAGE_FIRSTGAME + 11;
    acase BRAINQUIZPOS:                              return IMAGE_FIRSTGAME + 12;
    acase BREAKAWAYPOS:                              return IMAGE_FIRSTGAME + 13;
    acase A_CAPTUREPOS:                              return IMAGE_FIRSTGAME + 14;
    acase CATTRAXPOS:                                return IMAGE_FIRSTGAME + 15;
    acase A_CIRCUSPOS:                               return IMAGE_FIRSTGAME + 16;
    acase A_COMBATPOS:       case A_COMBATODPOS:     return IMAGE_FIRSTGAME + 17;
    acase CRAZYCLIMBERPOS:                           return IMAGE_FIRSTGAME + 18;
    acase CRAZYGOBBLERPOS:                           return IMAGE_FIRSTGAME + 19;
    acase VIDLEXEGPOS:       case VIDLEXGEPOS:       return IMAGE_FIRSTGAME + 20;
    acase DORAEMONPOS:                               return IMAGE_FIRSTGAME + 21;
    acase DRSLUMPPOS:                                return IMAGE_FIRSTGAME + 22;
    acase ESCAPEPOS:                                 return IMAGE_FIRSTGAME + 23;
    acase GRIDIRON1POS:      case GRIDIRON2POS:      return IMAGE_FIRSTGAME + 24;
    acase FROGGER1POS:       case FROGGER2POS:       case FROGGER3POS:
                                                     return IMAGE_FIRSTGAME + 25;
    acase FUNKYFISHPOS:                              return IMAGE_FIRSTGAME + 26;
    acase A_GOLFPOS1:        case A_GOLFPOS2:        case A_GOLFODPOS:
                                                     return IMAGE_FIRSTGAME + 27;
    acase GUNDAMPOS:                                 return IMAGE_FIRSTGAME + 28;
    acase HOBOPOS1:          case HOBOPOS2:          return IMAGE_FIRSTGAME + 29;
    acase A_HORSERACINGPOS:                          return IMAGE_FIRSTGAME + 30;
    acase JOURNEYPOS:                                return IMAGE_FIRSTGAME + 31;
    acase JUMPBUG1POS:       case JUMPBUG2POS:       return IMAGE_FIRSTGAME + 32;
    acase JUNGLERPOS:                                return IMAGE_FIRSTGAME + 33;
    acase MACROSSPOS:                                return IMAGE_FIRSTGAME + 34;
    acase MISSILEWARPOS:                             return IMAGE_FIRSTGAME + 35;
    acase MONACOPOS:                                 return IMAGE_FIRSTGAME + 36;
    acase NIBBLEMENPOS:      case SUPERGOBBLERPOS:   return IMAGE_FIRSTGAME + 37;
    acase OCEANBATTLEPOS:                            return IMAGE_FIRSTGAME + 38;
    acase PARASHOOTERPOS:                            return IMAGE_FIRSTGAME + 39;
    acase PLEIADESPOS:                               return IMAGE_FIRSTGAME + 40;
    acase R2DTANKPOS:                                return IMAGE_FIRSTGAME + 41;
    acase REDCLASHPOS:       case REDCLASHODPOS:     return IMAGE_FIRSTGAME + 42;
    acase ROBOTKILLERPOS:                            return IMAGE_FIRSTGAME + 43;
    acase ROUTE16POS:                                return IMAGE_FIRSTGAME + 44;
    acase _2DSOCCERPOS:      case _2DSOCCERODPOS:    return IMAGE_FIRSTGAME + 45;
    acase SPACEATTACKAPOS:   case SPACEATTACKBPOS:   case SPACEATTACKCPOS:
                                                     return IMAGE_FIRSTGAME + 46;
    acase SPACEBUSTERPOS:                            return IMAGE_FIRSTGAME + 47;
    acase SPACEMISSIONPOS:                           return IMAGE_FIRSTGAME + 48;
    acase SPACERAIDERSPOS:                           return IMAGE_FIRSTGAME + 49;
    acase SPACESQUADRON1POS: case SPACESQUADRON2POS: return IMAGE_FIRSTGAME + 50;
    acase SPACEVULTURESPOS:  case MOTHERSHIPPOS:     return IMAGE_FIRSTGAME + 51;
    acase SPIDERSPOS:        case SPIDERSODPOS:      return IMAGE_FIRSTGAME + 52;
    acase STARCHESSPOS:      case STARCHESSENHPOS:   return IMAGE_FIRSTGAME + 53;
    acase SUPERBUG1POS:      case SUPERBUG2POS:      return IMAGE_FIRSTGAME + 54;
    acase TANKSALOTPOS:                              return IMAGE_FIRSTGAME + 55;
    acase TENNISPOS:                                 return IMAGE_FIRSTGAME + 56;
    acase THEENDPOS1:        case THEENDPOS2:        return IMAGE_FIRSTGAME + 57;
    acase TURTLESPOS:                                return IMAGE_FIRSTGAME + 58; // ARCADIAGLYPHS-1
    adefault:                                        return memmap_to_smlimage[thememmap];
}   }

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
            *whichemubrush = EMUBRUSH_YELLOW;
        } else
        {   *whichemupen   = EMUPEN_GREEN;
            *whichemubrush = EMUBRUSH_GREEN;
    }   }

    return TRUE;
}

EXPORT void generate_controlstip(int localkey)
{   FAST FLAG done;
    FAST int  whichplayer;

    whichplayer = keyinfo[whichkeyrect][localkey].player;
    if (whichplayer != -1 && swapped && (machine == ARCADIA || machines[machine].pvis || machine == PONG)) // INSTRUCTOR, etc. don't allow swapping
    {   whichplayer = whichplayer ? 0 : 1;
    }

    if (keyinfo[whichkeyrect][localkey].to_overlay == -1)
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
            strcpy((char*) controltip[3], LLL(MSG_REVERSE, "Forward"));
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
            strcpy((char*) controltip[2], keyname[keypads[whichplayer][keyinfo[whichkeyrect][localkey].to_keypad]].name);
        } elif
        (   ((machine == ARCADIA || machine == INTERTON || machine == ELEKTOR) && (localkey == 29 || localkey == 33))
         || (machine == ZACCARIA && localkey == 3)
         || (machine == MALZAK && localkey == 9)
        )
        {   strcpy((char*) controltip[0], LLL(MSG_KEY_LT, "Lt"));
            strcpy((char*) controltip[3], LLL(MSG_KEY_LT, "Lt"));
            strcpy((char*) controltip[2], keyname[keypads[whichplayer][keyinfo[whichkeyrect][localkey].to_keypad]].name);
        } elif
        (   ((machine == ARCADIA || machine == INTERTON || machine == ELEKTOR) && (localkey == 30 || localkey == 34))
         || ((memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN) && localkey == 6)
         || (machine == MALZAK && localkey == 7)
        )
        {   strcpy((char*) controltip[0], LLL(MSG_KEY_DN, "Dn"));
            strcpy((char*) controltip[3], LLL(MSG_KEY_DN, "Dn"));
            strcpy((char*) controltip[2], keyname[keypads[whichplayer][keyinfo[whichkeyrect][localkey].to_keypad]].name);
        } elif
        (   ((machine == ARCADIA || machine == INTERTON || machine == ELEKTOR) && (localkey == 31 || localkey == 35))
         || (machine == ZACCARIA && localkey == 4)
         || (machine == MALZAK && localkey == 5)
        )
        {   strcpy((char*) controltip[0], LLL(MSG_KEY_RT, "Rt"));
            strcpy((char*) controltip[3], LLL(MSG_KEY_RT, "Rt"));
            strcpy((char*) controltip[2], keyname[keypads[whichplayer][keyinfo[whichkeyrect][localkey].to_keypad]].name);
        } elif (machine == MALZAK)
        {   switch (localkey)
            {
            case  4:
                strcpy((char*) controltip[0], LLL(MSG_UPRIGHT, "UpRt"));
                strcpy((char*) controltip[3], LLL(MSG_UPRIGHT, "UpRt"));
                strcpy((char*) controltip[2], keyname[keypads[whichplayer][keyinfo[whichkeyrect][localkey].to_keypad]].name);
            acase 6:
                strcpy((char*) controltip[0], LLL(MSG_DOWNRIGHT, "DnRt"));
                strcpy((char*) controltip[3], LLL(MSG_DOWNRIGHT, "DnRt"));
                strcpy((char*) controltip[2], keyname[keypads[whichplayer][keyinfo[whichkeyrect][localkey].to_keypad]].name);
            acase 8:
                strcpy((char*) controltip[0], LLL(MSG_DOWNLEFT, "DnLt"));
                strcpy((char*) controltip[3], LLL(MSG_DOWNLEFT, "DnLt"));
                strcpy((char*) controltip[2], keyname[keypads[whichplayer][keyinfo[whichkeyrect][localkey].to_keypad]].name);
            acase 10:
                strcpy((char*) controltip[0], LLL(MSG_UPLEFT, "UpLt"));
                strcpy((char*) controltip[3], LLL(MSG_UPLEFT, "UpLt"));
                strcpy((char*) controltip[2], keyname[keypads[whichplayer][keyinfo[whichkeyrect][localkey].to_keypad]].name);
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
    } else
    {   strcpy((char*) controltip[0], overlays[              whichoverlay][keyinfo[whichkeyrect][localkey].to_overlay]);
        strcpy((char*) controltip[1], overlays[memmapinfo[memmap].overlay][keyinfo[whichkeyrect][localkey].to_overlay]);
        if (machine == BINBUG)
        {   strcpy((char*) controltip[2], (const char*) controltip[0]);
        } else
        {   strcpy((char*) controltip[2], keyname[keypads[whichplayer][keyinfo[whichkeyrect][localkey].to_keypad]].name);
        }

        if
        (   key1 != keyinfo[whichkeyrect][localkey].to_keypad
         && key2 != keyinfo[whichkeyrect][localkey].to_keypad
         && key3 != keyinfo[whichkeyrect][localkey].to_keypad
         && key4 != keyinfo[whichkeyrect][localkey].to_keypad
        )
        {   strcpy((char*) controltip[3], "-");
            strcpy((char*) controltip[4], "-");
            return;
        }

        if (key1 == keyinfo[whichkeyrect][localkey].to_keypad)
        {   strcat((char*) controltip[2], ", ");
            strcat((char*) controltip[2], keyname[keypads[whichplayer][ 0]].name);
        }
        if (key2 == keyinfo[whichkeyrect][localkey].to_keypad)
        {   strcat((char*) controltip[2], ", ");
            strcat((char*) controltip[2], keyname[keypads[whichplayer][21]].name);
        }
        if (key3 == keyinfo[whichkeyrect][localkey].to_keypad)
        {   strcat((char*) controltip[2], ", ");
            strcat((char*) controltip[2], keyname[keypads[whichplayer][22]].name);
        }
        if (key4 == keyinfo[whichkeyrect][localkey].to_keypad)
        {   strcat((char*) controltip[2], ", ");
            strcat((char*) controltip[2], keyname[keypads[whichplayer][23]].name);
        }

        controltip[3][0] = EOS;
        done = FALSE;
        if (key1 == keyinfo[whichkeyrect][localkey].to_keypad)
        {   done = TRUE;
#ifdef WIN32
            sprintf(controltip[3], "%d", button[whichplayer][0]);
#endif
#ifdef AMIGA
            strcpy((char*) controltip[3], LLL(MSG_LABEL_RED, "Red"));
#endif
        }
        if (key2 == keyinfo[whichkeyrect][localkey].to_keypad)
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
        if (key3 == keyinfo[whichkeyrect][localkey].to_keypad)
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
        if (key4 == keyinfo[whichkeyrect][localkey].to_keypad)
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
        (   key1 != keyinfo[whichkeyrect][localkey].to_keypad
         && key2 != keyinfo[whichkeyrect][localkey].to_keypad
         && key3 != keyinfo[whichkeyrect][localkey].to_keypad
        )
        {   strcpy((char*) controltip[4], "-");
            return;
        }

        controltip[4][0] = EOS;
        done = FALSE;
        if (key1 == keyinfo[whichkeyrect][localkey].to_keypad)
        {   done = TRUE;
            strcpy((char*) controltip[4], LLL(MSG_LEFT, "Left"));
        }
        if (key2 == keyinfo[whichkeyrect][localkey].to_keypad)
        {   if (done)
            {   strcat((char*) controltip[4], ", ");
            }
            done = TRUE;
            sprintf((char*) ENDOF(controltip[4]), LLL(MSG_MIDDLE, "Middle"));
        }
        if (key3 == keyinfo[whichkeyrect][localkey].to_keypad)
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

    update_floppydrive(3, whichdrive);
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
        {   DRAWWAVEFORMPIXEL(x, y, colour);
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
                {   DRAWWAVEFORMPIXEL(x, y / 2, EMURGBPEN_GREY);
            }   }
            elif (t > ts)
            {   for (y = ts + 1; y <= t; y += 2)
                {   DRAWWAVEFORMPIXEL(x, y / 2, EMURGBPEN_GREY);
        }   }   }

        xf = samplewhere_f / scaleby;
        x = (int) xf;
        if (x >= WAVEFORMWIDTH)
        {   x = WAVEFORMWIDTH - 1;
        }
        for (y = 0; y < WAVEFORMHEIGHT; y++)
        {   DRAWWAVEFORMPIXEL(x, y, EMURGBPEN_BLACK);
    }   }

    for (x = 0; x < WAVEFORMWIDTH; x++)
    {   DRAWWAVEFORMPIXEL(x, ts / 2, EMURGBPEN_BLACK);
    }

    redraw_waveform();
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
        {   DRAWROLLPIXEL(whichunit, x, y, colour);
    }   }
    for (y = 0; y < 9; y++)
    {   if ((int) papertapewhere[whichunit] - 4 + y < 0 || (int) papertapewhere[whichunit] - 4 + y >= (int) papertapelength[whichunit])
        {
#ifdef SHOWEDGES
            for (yy = 0; yy < 20; yy++)
            {   for (xx = 0; xx < ROLLWIDTH; xx++)
                {   DRAWROLLPIXEL(whichunit, xx, (y * 20) + yy, EMURGBPEN_BLACK);
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
                                DRAWROLLPIXEL(whichunit, (x * 20) + ((x >= 5) ? 46 : 26) + xx, (y * 20) + 6 + yy, EMURGBPEN_BLACK);
#else
                                DRAWROLLPIXEL(whichunit, (x * 20) +                  26  + xx, (y * 20) + 6 + yy, EMURGBPEN_BLACK);
#endif
                }   }   }   }
                else // hollow circle
                {   for (yy = 0; yy < 10; yy++)
                    {   for (xx = 0; xx < 10; xx++)
                        {   if (chad[yy][xx] == '#')
                            {
#ifdef PAPERTAPESPROCKETS
                                DRAWROLLPIXEL(whichunit, (x * 20) + ((x >= 5) ? 46 : 26) + xx, (y * 20) + 6 + yy, EMURGBPEN_GREY);
#else
                                DRAWROLLPIXEL(whichunit, (x * 20) +                  26  + xx, (y * 20) + 6 + yy, EMURGBPEN_GREY);
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
                        DRAWROLLPIXEL(whichunit, 208 + (xx * 2), (y * 20) + 4 + (yy * 2), EMURGBPEN_BLACK);
                        DRAWROLLPIXEL(whichunit, 209 + (xx * 2), (y * 20) + 4 + (yy * 2), EMURGBPEN_BLACK);
                        DRAWROLLPIXEL(whichunit, 208 + (xx * 2), (y * 20) + 5 + (yy * 2), EMURGBPEN_BLACK);
                        DRAWROLLPIXEL(whichunit, 209 + (xx * 2), (y * 20) + 5 + (yy * 2), EMURGBPEN_BLACK);
#else
                        DRAWROLLPIXEL(whichunit, 188 + (xx * 2), (y * 20) + 4 + (yy * 2), EMURGBPEN_BLACK);
                        DRAWROLLPIXEL(whichunit, 189 + (xx * 2), (y * 20) + 4 + (yy * 2), EMURGBPEN_BLACK);
                        DRAWROLLPIXEL(whichunit, 188 + (xx * 2), (y * 20) + 5 + (yy * 2), EMURGBPEN_BLACK);
                        DRAWROLLPIXEL(whichunit, 189 + (xx * 2), (y * 20) + 5 + (yy * 2), EMURGBPEN_BLACK);
#endif
            }   }   }

#ifdef PAPERTAPESPROCKETS
            for (yy = 0; yy < 6; yy++)
            {   for (xx = 0; xx < 6; xx++)
                {   if (sprockethole[yy][xx] == '#')
                    {   DRAWROLLPIXEL(whichunit, 128 + xx, (y * 20) + 8 + yy, EMURGBPEN_BLACK);
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
                {   DRAWROLLPIXEL(whichunit, 8 + xx, 86 + yy, colour);
    }   }   }   }

#ifndef SHOWEDGES
    if (papertapewhere[whichunit] < 4)
    {   for (x = 0; x < ROLLWIDTH; x += 2)
        {   DRAWROLLPIXEL(whichunit, x, ((                         4 - papertapewhere[whichunit]    ) * 20) + 1, EMURGBPEN_BLACK);
    }   }
    if (papertapewhere[whichunit] + 4 >= papertapelength[whichunit])
    {   for (x = 0; x < ROLLWIDTH; x += 2)
        {   DRAWROLLPIXEL(whichunit, x, ((papertapelength[whichunit] - papertapewhere[whichunit] + 4) * 20) + 1, EMURGBPEN_BLACK);
    }   }
#endif

    redraw_roll(whichunit);
}

EXPORT int get_viewing_cluster(int whichdrive)
{   int viewingtrack, viewingsector;

    switch (machine)
    {
    case BINBUG:
    return drive[whichdrive].viewstart / BINBUG_BLOCKSIZE; // zero-based
    case TWIN:
        viewingtrack  =  drive[whichdrive].viewstart / TWIN_TRACKSIZE;
        viewingsector = (drive[whichdrive].viewstart % TWIN_TRACKSIZE) / TWIN_BLOCKSIZE;
    return (viewingtrack == 0) ? (-1) : (((viewingtrack - 1) * 4) + (viewingsector / 8));
    case CD2650:
    return drive[whichdrive].viewstart / CD2650_BLOCKSIZE; // zero-based
    default:
        // assert(0);
    return 0; // should never happen
}   }

EXPORT void get_disk_byte(int whichdrive, FLAG withoffset)
{   outofrange = FALSE;

    switch (machine)
    {
    case BINBUG:
        diskbyte = (drive[whichdrive].track * BINBUG_TRACKSIZE) + ((drive[whichdrive].sector - 1) * BINBUG_BLOCKSIZE);
        if
        (   drive[whichdrive].sector == 0
         || drive[whichdrive].sector >  BINBUG_SECTORS
         || drive[whichdrive].track  >= BINBUG_TRACKS
        )
        {   outofrange = TRUE;
        }
    acase TWIN:
        diskbyte = (drive[whichdrive].track *   TWIN_TRACKSIZE) + ( drive[whichdrive].sector      *   TWIN_BLOCKSIZE);
        if
        (   drive[whichdrive].sector >= TWIN_SECTORS
         || drive[whichdrive].track  >= TWIN_TRACKS
        )
        {   outofrange = TRUE;
        }
    acase CD2650:
        diskbyte = (drive[whichdrive].track * CD2650_TRACKSIZE) + ((drive[whichdrive].sector - 1) * CD2650_BLOCKSIZE);
        if
        (   drive[whichdrive].sector == 0
         || drive[whichdrive].sector >  CD2650_SECTORS
         || drive[whichdrive].track  >= CD2650_TRACKS
        )
        {   outofrange = TRUE;
        }
    adefault:
        ; // assert(0);
    }

    if (withoffset)
    {   diskbyte += drive[whichdrive].blockoffset;
}   }

EXPORT void set_cpl(int newcpl)
{   // assert(newcpl <= 227);

    cpl = newcpl;

    switch (cpl % 4)
    {
    case  0: frac[0] = frac[1] = frac[2] = frac[3] =  cpl / 4;
    acase 1: frac[0] = frac[1]           = frac[3] =  cpl / 4;
                                 frac[2]           = (cpl / 4) + 1;
    acase 2: frac[0]           = frac[2]           =  cpl / 4;
                       frac[1]           = frac[3] = (cpl / 4) + 1;
    acase 3: frac[0]                               =  cpl / 4;
                       frac[1] = frac[2] = frac[3] = (cpl / 4) + 1;
}   }

EXPORT void set_drive_mode(int newdrivemode)
{   drive_mode = newdrivemode;

    update_floppydrive(2, 0);
    update_floppydrive(2, 1);
    update_floppydrive(2, 2);
    update_floppydrive(2, 3);
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
    if (SubWindowPtr[SUBWINDOW_HOSTKYBD])
    {   SetGadgetAttrs(kybdgad[code & 0x7F], SubWindowPtr[SUBWINDOW_HOSTKYBD], NULL, GA_Selected, TRUE, TAG_DONE); // this autorefreshes
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
            thekey =  keyname[code].shifted[keymap];
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

    if (SubWindowPtr[SUBWINDOW_HOSTKYBD])
    {   updatekeynames(SubWindowPtr[SUBWINDOW_HOSTKYBD]);
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
            {   command_changemachine(MIKIT,      MEMMAP_MIKIT);
            } else
            {   if (size != 1) pending |= PENDING_1XSIZE;
        }   }
    acase SCAN_F2:
        if (ctrl())
        {   if (shift())
            {   command_changemachine(ZACCARIA,   MEMMAP_ASTROWARS);
            } else
            {   if (size != 2) pending |= PENDING_2XSIZE;
        }   }
    acase SCAN_F3:
        if (ctrl())
        {   if (shift())
            {   command_changemachine(ZACCARIA,   MEMMAP_GALAXIA);
            } else
            {   if (size != 3) pending |= PENDING_3XSIZE;
        }   }
    acase SCAN_F4:
        if (ctrl())
        {   if (shift())
            {   command_changemachine(ZACCARIA,   MEMMAP_LASERBATTLE);
            } else
            {   if (size != 4) pending |= PENDING_4XSIZE;
        }   }
    acase SCAN_F5:  // reset
        if (ctrl())
        {   if (shift())
            {   command_changemachine(MALZAK,     MEMMAP_LAZARIAN);
            } else
            {   if (size != 5) pending |= PENDING_5XSIZE;
        }   }
        elif (!crippled)
        {   project_reset(FALSE);
        }
    acase SCAN_F6:
        if (ctrl())
        {   if (shift())
            {   command_changemachine(MALZAK,     MEMMAP_MALZAK1);
            } else
            {   if (size != 6) pending |= PENDING_6XSIZE;
        }   }
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
    acase SCAN_F10:
        if (ctrl() && shift())
        {   command_changemachine(    TYPERIGHT,  MEMMAP_TYPERIGHT);
        }
    acase SCAN_F11:
        if (!ctrl() && !shift())
        {   fullscreen = fullscreen ? FALSE : TRUE;
            docommand(MENUITEM_FULLSCREEN);
        }
    acase SCAN_FFWD:
        // foo = ~foo; doesn't work!
        if (turbo) turbo = FALSE; else turbo = TRUE;
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
    if (SubWindowPtr[SUBWINDOW_HOSTKYBD])
    {   SetGadgetAttrs(kybdgad[code & 0x7F], SubWindowPtr[SUBWINDOW_HOSTKYBD], NULL, GA_Selected, FALSE, TAG_DONE); // this autorefreshes
    }
#endif

    if (SubWindowPtr[SUBWINDOW_HOSTKYBD])
    {   updatekeynames(SubWindowPtr[SUBWINDOW_HOSTKYBD]);
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

    if (!SubWindowPtr[SUBWINDOW_MEMORY])
    {   return;
    }

#ifdef WIN32
    if (repaintmemmap)
    {   repaintmemmap = FALSE;
        force = TRUE;
    }

    oldregionstart = regionstart;
    regionlevel = SendMessage
    (   GetDlgItem(SubWindowPtr[SUBWINDOW_MEMORY], IDC_MEMREGION),
        TBM_GETPOS,
        0,
        0
    );
    regionstart = regionlevel * MEMGADGETS;

    if (regionstart != oldregionstart || force)
    {   force = TRUE;

        sprintf(tempstring, "%02X00..%02XFF", regionstart / MEMGADGETS, regionstart / MEMGADGETS);
        DISCARD SetDlgItemText(SubWindowPtr[SUBWINDOW_MEMORY], IDL_REGIONDISPLAY, tempstring);
        for (i = 0; i < MEMGADGETS / 16; i++)
        {   sprintf(tempstring, "$%03Xx:", (regionstart / 16) + i);
            DISCARD SetDlgItemText(SubWindowPtr[SUBWINDOW_MEMORY], IDL_REGION0 + i, tempstring);
    }   }
#endif
#ifdef AMIGA
    if ((int) regionlevel != oldregionlevel)
    {   force = TRUE;
        oldregionlevel = regionlevel;
        SetGadgetAttrs
        (   gadgets[GID_ME_SL1], SubWindowPtr[SUBWINDOW_MEMORY], NULL,
            SLIDER_Level, regionlevel,
        TAG_DONE); // this autorefreshes
        regionstart = (int) (regionlevel * MEMGADGETS);
    }
    if ((int) regionlevel != oldregionlevel || force)
    {   sprintf((char*) gtempstring, "%04X..%04X", regionstart, regionstart + MEMGADGETS - 1);
        SetGadgetAttrs(gadgets[GID_ME_ST1], SubWindowPtr[SUBWINDOW_MEMORY], NULL, STRINGA_TextVal, gtempstring, TAG_DONE); // this autorefreshes
        for (i = 0; i < MEMGADGETS / 16; i++)
        {   sprintf((char*) gtempstring, "$%03Xx:", (regionstart / 16) + i);
            SetGadgetAttrs(gadgets[GID_FIRSTMEMROWGAD + i], SubWindowPtr[SUBWINDOW_MEMORY], NULL, STRINGA_TextVal, gtempstring, TAG_DONE); // this autorefreshes
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
                    DRAWMEMMAPPIXEL(x, y, colour);
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
                    DRAWMEMMAPPIXEL(x, y, colour);
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
                    DRAWMEMMAPPIXEL(x, y, colour);
        }   }   }

#ifdef WIN32
        MemMapRastPtr = GetDC(GetDlgItem(SubWindowPtr[SUBWINDOW_MEMORY], IDC_MEMMAP));
        DISCARD StretchDIBits
        (   MemMapRastPtr,
            0,              // dest leftx
            0,              // dest topy
            MEMMAPWIDTH,    // dest width
            MEMMAPHEIGHT,   // dest height
            0,              // source leftx
            0,              // source topy
            MEMMAPWIDTH,    // source width
            MEMMAPHEIGHT,   // source height
            canvasdisplay[CANVAS_MEMMAP], // pointer to the bits
            (const struct tagBITMAPINFO*) &CanvasBitMapInfo[CANVAS_MEMMAP], // pointer to BITMAPINFO structure
            DIB_RGB_COLORS, // format of data
            SRCCOPY         // blit mode
        );
        ReleaseDC(GetDlgItem(SubWindowPtr[SUBWINDOW_MEMORY], IDC_MEMMAP), MemMapRastPtr);
#endif
#ifdef AMIGA
        DISCARD WritePixelArray8
        (   SubWindowPtr[SUBWINDOW_MEMORY]->RPort,
            gadgets[GID_ME_SP1]->LeftEdge,
            gadgets[GID_ME_SP1]->TopEdge,
            gadgets[GID_ME_SP1]->LeftEdge + MEMMAPWIDTH  - 1,
            gadgets[GID_ME_SP1]->TopEdge  + MEMMAPHEIGHT - 1,
            canvasdisplay[CANVAS_MEMMAP],
            &wpa8canvasrastport[CANVAS_MEMMAP]
        );
#endif
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
                {   DRAWMAGNIFIERPIXEL(x, y, screen[newguesty][machines[machine].width - 1 - newguestx]);
                } else
                {   DRAWMAGNIFIERPIXEL(x, y, screen[newguestx][                              newguesty]);
            }   }
            else
            {   DRAWMAGNIFIERPIXEL(x, y, BORDERPEN);
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

#ifdef WIN32
    MagnifierRastPtr = GetDC(MagnifierWindowPtr);
    DISCARD StretchDIBits
    (   MagnifierRastPtr,
        0,                // dest leftx
        0,                // dest topy
        MAGNIFIERWIDTH,   // dest width
        MAGNIFIERHEIGHT,  // dest height
        0,                // source leftx
        0,                // source topy
        MAGNIFIERWIDTH,   // source width
        MAGNIFIERHEIGHT,  // source height
        canvasdisplay[CANVAS_MAGNIFIER], // pointer to the bits
        (const struct tagBITMAPINFO*) &CanvasBitMapInfo[CANVAS_MAGNIFIER], // pointer to BITMAPINFO structure
        DIB_RGB_COLORS,   // format of data
        SRCCOPY           // blit mode
    );
    ReleaseDC(MagnifierWindowPtr, MagnifierRastPtr);
#endif
#ifdef AMIGA
    DISCARD WritePixelArray8
    (   MagnifierWindowPtr->RPort,
        MagnifierWindowPtr->BorderLeft,
        MagnifierWindowPtr->BorderTop,
        MagnifierWindowPtr->BorderLeft + MAGNIFIERWIDTH  - 1,
        MagnifierWindowPtr->BorderTop  + MAGNIFIERHEIGHT - 1,
        canvasdisplay[CANVAS_MAGNIFIER],
        &wpa8canvasrastport[CANVAS_MAGNIFIER]
    );
#endif
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

EXPORT void update_sliders(void)
{   ULONG number;

    // Red----------------------------------------------------------------

    number = (ULONG) getred(colourset, palettepen);
#ifdef WIN32
    sl_set(SUBWINDOW_PALETTE, IDC_RED, 255 - number);

    sprintf(gtempstring, "%d", number);
    st_set(SUBWINDOW_PALETTE, IDC_REDDEC);
#endif
#ifdef AMIGA
    sl_set(SUBWINDOW_PALETTE, IDC_RED, number);

    SetGadgetAttrs(gadgets[IDC_REDDEC], SubWindowPtr[SUBWINDOW_PALETTE], NULL, INTEGER_Number, number, TAG_DONE); // this autorefreshes
#endif

    sprintf((char*) gtempstring, "%02X", (unsigned int) number);
    st_set(SUBWINDOW_PALETTE, IDC_REDHEX);

    // Green--------------------------------------------------------------

    number = (ULONG) getgreen(colourset, palettepen);
#ifdef WIN32
    sl_set(SUBWINDOW_PALETTE, IDC_GREEN, 255 - number);

    sprintf(gtempstring, "%d", number);
    st_set(SUBWINDOW_PALETTE, IDC_GREENDEC);
#endif
#ifdef AMIGA
    sl_set(SUBWINDOW_PALETTE, IDC_GREEN, number);

    SetGadgetAttrs(gadgets[IDC_GREENDEC], SubWindowPtr[SUBWINDOW_PALETTE], NULL, INTEGER_Number, number, TAG_DONE); // this autorefreshes
#endif

    sprintf((char*) gtempstring, "%02X", (unsigned int) number);
    st_set(SUBWINDOW_PALETTE, IDC_GREENHEX);

    // Blue---------------------------------------------------------------

    number = (ULONG) getblue(colourset, palettepen);
#ifdef WIN32
    sl_set(SUBWINDOW_PALETTE, IDC_BLUE, 255 - number);

    sprintf(gtempstring, "%d", number);
    st_set(SUBWINDOW_PALETTE, IDC_BLUEDEC);
#endif
#ifdef AMIGA
    sl_set(SUBWINDOW_PALETTE, IDC_BLUE, number);

    SetGadgetAttrs(gadgets[IDC_BLUEDEC], SubWindowPtr[SUBWINDOW_PALETTE], NULL, INTEGER_Number, number, TAG_DONE); // this autorefreshes
#endif

    sprintf((char*) gtempstring, "%02X", (unsigned int) number);
    st_set(SUBWINDOW_PALETTE, IDC_BLUEHEX);
}

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
