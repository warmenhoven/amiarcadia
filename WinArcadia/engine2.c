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

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef AMIGA
    #include <intuition/intuition.h>
    #define ALL_REACTION_CLASSES
    #define ALL_REACTION_MACROS
    #include <reaction/reaction.h>
    #include <proto/graphics.h>
    #include <proto/intuition.h>
    #include <proto/locale.h>
#endif

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"
#ifdef AMIGA
    #include "amiga.h"
#endif

#include "overlays.h"

// DEFINES----------------------------------------------------------------

// #define SHOWCRCS
// enable this to show CRC32s and CRC64s for audited and loaded files

#define BADFRAMES 50
// how many consecutive missed frames are needed for autoframeskip to
// increase the frame skipping.

#ifdef WIN32
// #define USEACTIVISIONSAMPLES
/* whether to randomly play the ambient samples from Activision 15 Games Pack:
   1. Pausing, turning sound off, etc. doesn't stop the sample playing.
   2. A new sample can be started before the old one has finished.
   3. Some samples should not be played before some other samples (eg.
      girlfriend samples are order-dependent). */
#define SECSPERSAMPLE (60 * 60) // about once per hour
#endif

#ifdef AMIGA
    #if defined(__SASC) || defined(__VBCC__)
        #define htons(x) (x)
    #endif
    #ifdef __amigaos4__
        #include <sys/socket.h>
        #include <netdb.h>
        #include <errno.h>
    #endif
    #ifdef __MORPHOS__
        #define _SYS_SOCKET_H_
        #include <sys/socket.h>
        #include <proto/socket.h>
        #include <arpa/inet.h>
        #include <errno.h>
    #endif
    #if !defined(__amigaos4__) && !defined(__MORPHOS__)
        IMPORT int errno;
    #endif
#endif

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       FLAG                     consoleopened            = FALSE;
EXPORT       UBYTE                    chan_volume[GUESTCHANNELS],
                                      cheevomem[12 * KILOBYTE],
                                      circle,
                                      coinignore,
                                      cosversion;
EXPORT       TEXT                     autotext[GAMEINFOLINES][80 + 1],
                                      bpnf_string[LABELLIMIT + 1] = "$0",
                                      datetimestring[40 + 1],
                                      pgmtext[6 * KILOBYTE];
EXPORT       UWORD                    loadaddress              = 0x900;
EXPORT       ULONG                    redrawtime               =  0,
                                      waittill                 =  0; // must be initialized!
EXPORT       int                      base                     = BASE_HEX,
                                      chan_status[GUESTCHANNELS],
                                      cheevosize               =  0,
                                      clicked                  = -1,
                                      cpl,
                                      filesize,
                                      firstrow                 = 26,
                                      found_dirs,
                                      key1                     =  2,
                                      key2                     =  1,
                                      key3                     =  3,
                                      key4                     =  0,
                                      lastrow                  = -1,
                                      nextinst,
                                      p1bgcol[4],
                                      p2bgcol[4],
                                      p1sprcol[6],
                                      p2sprcol[6],
                                      paddleup                 = -1,
                                      paddledown               = -1,
                                      paddleleft               = -1,
                                      paddleright              = -1,
                                      ppc,
                                      runtoframe               = FALSE,
                                      speedup                  = SPEED_4QUARTERS,
                                      udcflips                 =  0,
                                      usecsperframe[2],
                                      whichkeyrect,
                                      whichoverlay;
EXPORT       float                    chan_hertz[GUESTCHANNELS];

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       FLAG                     allglyphs,
                                      bangercharging,
                                      inframe,
                                      halted,
                                      multisprite[4],
                                      need[SUBWINDOWS],
                                      protect[4];
IMPORT       SBYTE                    galaxia_scrolly;
IMPORT       UBYTE                    abeff1, abeff2,
                                      aw_dips1, aw_dips2,
                                      awga_collide,
                                      banked,
                                      binbug_joyunit,
                                      digitleds[DIGITLEDS],
                                      g_bank1[0x400],
                                      g_bank2[16],
                                      ga_dips,
                                      glow,
                                      ininterrupt,
                                      lb_bank,
                                      lb_dips,
                                      lb_spritecode,
                                      lb_spritecolour,
                                      lb_spriteenable,
                                      lb_spritex,
                                      lb_spritey,
                                      lb_tone,
                                      lz_dips2, lz_dips3,
                                      malzak_field[16][16],
                                      memory[32768],
                                      memory_effects[512],
                                      mux,
                                      neg1, neg2,
                                      psl,
                                      psu,
                                      q_bank[16][16384],
                                      r[7],
                                      rate,
                                      s_tapeport,
                                      s_toggles,
                                      selbst_char,
                                      squeal,
                                      startaddr_h,
                                      startaddr_l,
                                      ticks,
                                      u_bank[16][2048],
                                      useshell,
                                      vdu[MAX_VDUCOLUMNS][MAX_VDUROWS];
IMPORT       UBYTE*                   IOBuffer;
IMPORT       UWORD                    iar,
                                      keypads[2][NUMKEYS],
                                      mirror_r[32768],
                                      mirror_w[32768],
                                      ras[8];
IMPORT       ULONG                    analog,
                                      arcadia_viewcontrolsas,
                                      autoframeskip,
                                      bangfrom,
                                      pong8550_viewcontrolsas,
                                      pong8600_viewcontrolsas,
                                      collisions,
                                      coverage[32768],
                                      crc64_h,
                                      crc64_l,
                                      cycles_2650,
                                      downframes,
                                      elektor_bigctrls,
                                      fractionator,
                                      frames,
                                      interton_bigctrls,
                                      lb_snd,
                                      mikit_bigctrls,
                                      oldcycles,
                                      paused,
                                      pencolours[COLOURSETS][PENS],
                                      region,
                                      sound,
                                      totalframes,
                                      tt_kybdtill,
                                      turbo,
                                      si50_bigctrls,
                                      swapped;
IMPORT       char                     mn[256 + 1];
IMPORT       int                      ambient,
                                      autocoin,
                                      ax[2], // analog paddle X-coords
                                      ay[4], // analog paddle Y-coords
                                      bangercharge,
                                      belling[2],
                                      binbug_baudrate,
                                      binbug_biosver,
                                      binbug_dosver,
                                      cd2650_biosver,
                                      cd2650_vdu,
                                      cheevos,
                                      climode,
                                      coinop_1p,
                                      coinop_2p,
                                      coinop_coina,
                                      coinop_coinb,
                                      coinop_button1,
                                      coinop_button2,
                                      coinop_button3,
                                      coinop_button4,
                                      coinop_joy1left,
                                      coinop_joy1right,
                                      coinop_joy1up,
                                      coinop_joy1down,
                                      coinop_joy2left,
                                      coinop_joy2right,
                                      colourlog,
                                      configmachine,
                                      cpb,
                                      crippled,
                                      currentgame,
                                      darkenbg,
                                      elektor_biosver,
                                      exactspeed,
                                      fastselbst,
                                      filter,
                                      firstdoscodecomment, lastdoscodecomment,
                                      flagline,
                                      foundgames,
                                      frameskip,
                                      game,
                                      generate,
                                      hostvolume,
                                      interrupt_2650,
                                      lastparse,
                                      logfile,
                                      loop,
                                      machine,
                                      malzak_bank1,
                                      malzak_bank2,
                                      malzak_x,
                                      malzak_y,
                                      memmap,
                                      offset,
                                      papertapemode[2],
                                      phunsy_biosver,
                                      pipbug_baudrate,
                                      pipbug_biosver,
                                      pipbug_periph,
                                      pipbug_vdu,
                                      pong_variant,
                                      post,
                                      queuepos,
                                      quiet,
                                      recmode,
                                      recsize,
                                      rotating,
                                      score[2],
                                      selbst_biosver,
                                      sensegame,
                                      sensitivity[2],
                                      serializemode,
                                      showbuiltin,
                                      showleds,
                                      showstatusbars[2],
                                      sidebar[FOUNDGAMES_MAX],
                                      size,
                                      slice_2650,
                                      sortby,
                                      speakoffset,
                                      spriteflip,
                                      spriteflips,
                                      style,
                                      supercpu,
                                      s_io,
                                      s_is,
                                      s_id,
                                      table_duration_2650[256],
                                      table_size_2650[256],
                                      tapeframe,
                                      tapemode,
                                      timeunit,
                                      trainer_lives,
                                      trainer_time,
                                      trainer_invincibility,
                                      udcflip,
                                      vdu_x, vdu_y,
                                      verbosity,
                                      viewspeedas,
                                      whichgame,
                                      wsm,
                                      ZIPBufferSize;
IMPORT       MEMFLAG                  memflags[ALLTOKENS];
IMPORT       STRPTR                   timeunitstr1,
                                      timeunitstr2,
                                      ZIPBufferPtr;
IMPORT const STRPTR                   longname[CPUTIPS][2],
                                      varianthelp[VARIANTS];
IMPORT const UBYTE                    rpm2fc[120 + 1],
                                      table_opcolours_2650[2][256];
IMPORT       TEXT                     file_game[MAX_PATH + 1],
                                      fn_asm[MAX_PATH + 1],
                                      fn_game[MAX_PATH + 1],
                                      fn_tape[4][MAX_PATH + 1],
                                      friendly[FRIENDLYLENGTH + 1],
                                      gtempstring[256 + 1],
                                      path_games[MAX_PATH + 1],
                                      StatusText[4][80 + 1],
                                      thequeue[QUEUESIZE];
IMPORT const TEXT                     astrowars_chars[256 + 1],
                                      arcadia_chars[64 + 1],
                                      cd2650_chars[128 + 1],
                                      elektor_chars1[256 + 1],
                                      galaxia_chars[128 + 1],
                                      instructor_chars1[256 + 1],
                                      lb_chars[256 + 1],
                                      table_addrmode_2650[2][256 + 1];
IMPORT const int                      speedupnum[SPEED_MAX - SPEED_MIN + 1];
IMPORT       ASCREEN                  screen[BOXWIDTH][BOXHEIGHT];
IMPORT       FILE*                    DisHandle;
IMPORT       struct AuditStruct       auditlist[FOUNDGAMES_MAX];
IMPORT       struct DriveStruct       drive[DRIVES_MAX];
IMPORT       struct HiScoreStruct     hiscore[HISCORES];
IMPORT       struct HostMachineStruct hostmachines[MACHINES];
IMPORT       struct IOPortStruct      ioport[258];
IMPORT       struct KeyNameStruct     keyname[SCANCODES];
IMPORT       struct KindStruct        filekind[KINDS];
IMPORT       struct MachineStruct     machines[MACHINES];
IMPORT       struct MemMapInfoStruct  memmapinfo[MEMMAPS];
IMPORT       struct MonitorStruct     monitor[MONITORGADS];
IMPORT       struct OpcodeStruct      opcodes[3][256];
IMPORT const struct CodeCommentStruct codecomment[];
IMPORT const struct KnownStruct       known[KNOWNGAMES];
#ifdef AMIGA
    IMPORT       LONG                 emupens[EMUBRUSHES],
                                      pens[PENS];
    IMPORT       ULONG                pending;
    IMPORT       int                  throb;
    IMPORT       struct Catalog*      CatalogPtr;
    IMPORT       struct Library*      SocketBase;
    IMPORT       struct Gadget*       gadgets[GIDS + 1];
    IMPORT       struct Window       *MainWindowPtr,
                                     *SubWindowPtr[SUBWINDOWS];
    IMPORT       Object*              images[IMAGES];
    IMPORT const int                  memmap_to_smlimage[MEMMAPS];
    IMPORT const struct KeyHelpStruct keyhelp[36];
#endif
#ifdef WIN32
    IMPORT       FLAG                 hurry,
                                      quitting;
    IMPORT       TEXT                 consolestring[OUTPUTLENGTH + 1],
                                      file_bkgrnd[MAX_PATH + 1],
                                      fn_bkgrnd[MAX_PATH + 1],
                                      path_bkgrnd[MAX_PATH + 1];
    IMPORT       UBYTE                sky;
    IMPORT       int                  CatalogPtr, // APTR doesn't work
                                      consolebg,
                                      enhancestars,
                                      language,
                                      memmap_to_smlimage[MEMMAPS],
                                      outputpos,
                                      textcolour;
    IMPORT       HICON                smlicon;
    IMPORT       HWND                 hStatusBar,
                                      MainWindowPtr,
                                      RichTextGadget,
                                      SubWindowPtr[SUBWINDOWS];
    IMPORT       UBYTE*               pixelubyte;
    IMPORT       ULONG*               pixelulong;
    IMPORT       CHARFORMAT2          fgformat;
    IMPORT const struct KeyHelpStruct keyhelp[40];
#endif

IMPORT void (* drawpixel) (int x, int y, int colour);

// MODULE VARIABLES-------------------------------------------------------

MODULE TEXT        replystring[1000];
MODULE ULONG       oldfpscyc,
                   newtime,
                   oldfpstime;
MODULE int         badframes,
                   bpnf_address,
                   cursor,
                   found_matched,
                   found_unmatched,
                   found_loadables,
                   found_disks,
                   found_tapes,
                   found_unloadables,
                   foundsnaps,
                   foundicons,
                   found_unknowns,
                   startoffset,
                   wantmemmap;
MODULE FILE*       LogfileHandle = NULL;
#ifdef WIN32
MODULE UBYTE       colourtext[OUTPUTLENGTH + 1];
#endif

/* MODULE STRUCTURES------------------------------------------------------

(see overlays.h)

MODULE FUNCTIONS------------------------------------------------------- */

MODULE int aof_readblock(int localsize);
MODULE int iof_readblock(int localsize);
MODULE FLAG autosense(void);
MODULE FLAG isok(int start, int end);
#ifdef WIN32
    MODULE void textpen(int colour);
#endif
MODULE int resolvegame(void);
MODULE int identify(STRPTR passedname);
MODULE void sortbyname(void);
MODULE void sortbymachine(void);
MODULE __inline void swapgames(int game1, int game2);
// MODULE void quicksort_bymachine(int m, int n);
// MODULE void quicksort_byname(int m, int n);
MODULE void write_log_string(STRPTR string);

// CODE-------------------------------------------------------------------

EXPORT void configure(void)
{   int i,
        newmachine;

    if (whichgame != -1)
    {   newmachine = known[whichgame].machine;
    } else
    {   newmachine = machine;
    }

    switch (newmachine)
    {
    case INTERTON:
        if
        (   whichgame == I_TETRISPOS1
         || whichgame == I_TETRISPOS2
         || whichgame == I_TETRISPOS3
        )
        {   machines[newmachine].digipos[0] = 0x0C;
            machines[newmachine].digipos[1] = 0x34;
            machines[newmachine].digipos[2] = 0x58;
        } else
        {   machines[newmachine].digipos[0] =    1;
            machines[newmachine].digipos[1] =  112;
            machines[newmachine].digipos[2] =  225;
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
        {   machines[newmachine].digipos[0] =   0;
            machines[newmachine].digipos[1] =   1;
         // machines[newmachine].digipos[2] = 225;
        } else
        {   machines[newmachine].digipos[0] =   1;
            machines[newmachine].digipos[1] = 111;
         // machines[newmachine].digipos[2] = 225;
    }   }

    ppc = machines[newmachine].ppc;
    if (whichgame == -1)
    {   downframes      = 4;
        totalframes     = 16;
        if (machine == ARCADIA || machine == INTERTON)
        {   set_cpl(227);
        } elif (machine == ELEKTOR)
        {   set_cpl(226);
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
        paddleup        =
        paddledown      =
        paddleleft      =
        paddleright     = -1;
        firstrow        = 26;
        lastrow         = -1;
        udcflips        =
        spriteflips     = 0;
        for (i = 0; i < 4; i++)
        {   protect[    i] =
            multisprite[i] = FALSE;
        }
        pipbug_periph   = PERIPH_PRINTER;
        // we deliberately leave region, flagline, darkenbg, s_io, s_is, s_id alone

        if (machine == MALZAK || machine == ZACCARIA || machine == PONG)
        {   changemachine(newmachine, memmap,                      FALSE, FALSE, FALSE);
        } else
        {   changemachine(newmachine, machines[newmachine].memmap, FALSE, FALSE, FALSE);
        }
        whichoverlay    = memmapinfo[memmap].overlay;
    } else
    {   switch (newmachine)
        {
        case ELEKTOR: elektor_biosver = known[whichgame].bios;
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
        acase CD2650: cd2650_biosver  = known[whichgame].bios;
                      cd2650_vdu      = known[whichgame].flagline;
        acase PHUNSY: phunsy_biosver  = known[whichgame].bios;
        acase SELBST: selbst_biosver  = known[whichgame].bios;
        }
        changemachine(known[whichgame].machine, known[whichgame].memmap, FALSE, FALSE, FALSE);

        switch (machine)
        {
        case ARCADIA:
            if (known[whichgame].bios != REGION_ANY)
            {   region    = (ULONG) known[whichgame].bios;
            }
            flagline      =         known[whichgame].flagline;
        acase INTERTON:
            darkenbg      =         known[whichgame].flagline;
        acase ELEKTOR:
            darkenbg      =         known[whichgame].flagline;
            if (whichgame == SUBMARINESRACINGPOS)
            {   ppc       = 2;
            }
        acase INSTRUCTOR:
            s_id          = (int)   known[whichgame].spriteflips;
            s_is          = (int)   known[whichgame].udcflips;
            s_io          =         known[whichgame].flagline;
        acase PHUNSY:
            startaddr_h   = (UBYTE) (known[whichgame].startaddr / 256);
            startaddr_l   = (UBYTE) (known[whichgame].startaddr % 256);
        acase SELBST:
            fastselbst    =         known[whichgame].flagline ? FALSE : TRUE;
            machines[SELBST].cpf = fastselbst ? 20000.0 : 0.02;
            // is update_speed() needed?
        }
        sensitivity[0]    =
        sensitivity[1]    = (int)   known[whichgame].sensitivity;
        analog            = (ULONG) known[whichgame].analog;
        if (machine != PIPBUG)
        {   swapped       = (ULONG) known[whichgame].swapped;
        }
        downframes        = (ULONG) known[whichgame].downframes;
        totalframes       = (ULONG) known[whichgame].totalframes;
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
        paddleup          = (int)   known[whichgame].up;
        paddledown        = (int)   known[whichgame].down;
        paddleleft        = (int)   known[whichgame].left;
        paddleright       = (int)   known[whichgame].right;
        if (known[whichgame].firstcodecomment != -1)
        {   // assert(known[whichgame].lastcodecomment != -1);
            for (i = known[whichgame].firstcodecomment; i <= known[whichgame].lastcodecomment; i++)
            {   memflags[codecomment[i].address] |= COMMENTED;
        }   }
        whichoverlay      = (int)   known[whichgame].whichoverlay;
        if ((machine == ARCADIA || machine == INTERTON || machine == ELEKTOR) && known[whichgame].cpl)
        {   set_cpl(        (int)   known[whichgame].cpl);
        } else
        {   set_cpl(                227);
        }

        make_stars();

        if (machine != PIPBUG && machine != INSTRUCTOR) // because these machines use these known[] structure fields for other purposes
        {   if (known[whichgame].demultiplex || known[whichgame].spriteflips || known[whichgame].udcflips)
            {   firstrow    =  known[whichgame].firstrow;
                lastrow     =  known[whichgame].lastrow;
                udcflips    =  known[whichgame].udcflips;
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
        }   }   }

#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Autosensed game #%d.\n", whichgame);
#endif
}   }

EXPORT void generate_autotext(void)
{   int i, j,
        thesize = 0; // initialized to avoid spurious SAS/C compiler warnings
#ifdef AMIGA
    int theimage;
#endif

    strcpy((char*) autotext[AUTOLINE_MACHINE], memmapinfo[memmap].name);

    if (machines[machine].coinop || machine == PONG || machine == TYPERIGHT)
    {   whichgame = -1;
        strcpy((char*) autotext[AUTOLINE_GAMENAME ], memmapinfo[memmap].name);
        strcpy((char*) autotext[AUTOLINE_CREDITS  ], memmapinfo[memmap].credits);
        strcpy((char*) autotext[AUTOLINE_REFERENCE], "-");
        strcpy((char*) autotext[AUTOLINE_CRC32    ], LLL(MSG_ENGINE_NOTAPPLICABLE, "n/a"));
        if (machines[machine].coinop)
        {   strcpy
            (   (char*) autotext[AUTOLINE_COMPLANG],
                LLL(
                    MSG_MACHINECODE, "Machine code"
            )   );
            thesize = memmapinfo[memmap].thesize;
        } elif (machine == PONG)
        {   strcpy((char*) autotext[AUTOLINE_COMPLANG], "Microcode");
        } elif (machine == TYPERIGHT)
        {   strcpy((char*) autotext[AUTOLINE_COMPLANG], LLL(MSG_ENGINE_NOTAPPLICABLE, "n/a"));
    }   }
    elif (whichgame == -1)
    {   strcpy((char*) autotext[AUTOLINE_GAMENAME ], "-");
        strcpy((char*) autotext[AUTOLINE_CREDITS  ], "?");
        strcpy((char*) autotext[AUTOLINE_REFERENCE], "?");
        strcpy((char*) autotext[AUTOLINE_SIZE     ], "?");
        strcpy((char*) autotext[AUTOLINE_CRC32    ], "?");
        strcpy((char*) autotext[AUTOLINE_COMPLANG ], "?");
    } else
    {
#ifdef WIN32
        if (language == LANGUAGE_RUS && whichgame == TETRISPOS)
        {   strcpy((char*) autotext[AUTOLINE_GAMENAME], "ÊÕò¡Ì±");
        } else
#endif
        strcpy((char*) autotext[AUTOLINE_GAMENAME ], known[whichgame].name);
        strcpy((char*) autotext[AUTOLINE_CREDITS  ], known[whichgame].credits);
        strcpy((char*) autotext[AUTOLINE_REFERENCE], known[whichgame].reference);
        thesize = known[whichgame].size;
        sprintf
        (   (char*) autotext[AUTOLINE_CRC32],
            "$%08lX/$%08lX%08lX", // MorphOS version apparently needs %lX not %X
            known[whichgame].crc32,
            known[whichgame].crc64_h,
            known[whichgame].crc64_l
        );
        switch (known[whichgame].complang)
        {
        case CL_MISC:
            strcpy
            (   (char*) autotext[AUTOLINE_COMPLANG],
                LLL(
                    MSG_MISCELLANEOUS, "Miscellaneous"
            )   );
        acase CL_MC:
            strcpy
            (   (char*) autotext[AUTOLINE_COMPLANG],
                LLL(
                    MSG_MACHINECODE,   "Machine code"
            )   );
        acase CL_TEXT:  strcpy((char*) autotext[AUTOLINE_COMPLANG], "ASCII text");
        acase CL_MWB:   strcpy((char*) autotext[AUTOLINE_COMPLANG], "MicroWorld BASIC");
        acase CL_FORTH: strcpy((char*) autotext[AUTOLINE_COMPLANG], "FORTH");
        acase CL_12KB:  strcpy((char*) autotext[AUTOLINE_COMPLANG], "12K BASIC");
        acase CL_MB:    strcpy((char*) autotext[AUTOLINE_COMPLANG], "2650 Micro BASIC");
        acase CL_C2001: strcpy((char*) autotext[AUTOLINE_COMPLANG], "Compiler 2001");
        acase CL_8KB13: strcpy((char*) autotext[AUTOLINE_COMPLANG], "8K BASIC 1.3");
        acase CL_TCT:   strcpy((char*) autotext[AUTOLINE_COMPLANG], "TCT BASIC");
        adefault: // should never happen
            strcpy((char*) autotext[AUTOLINE_COMPLANG], "?");
    }   }

    if (machine == PONG || machine == TYPERIGHT)
    {   strcpy((char*) autotext[AUTOLINE_SIZE     ], LLL(MSG_ENGINE_NOTAPPLICABLE, "n/a"));
        strcpy((char*) autotext[AUTOLINE_STARTADDR], LLL(MSG_ENGINE_NOTAPPLICABLE, "n/a"));
    } else
    {   if (whichgame != -1 || machines[machine].coinop)
        {   if (thesize % 256)
            {   sprintf
                (   (char*) autotext[AUTOLINE_SIZE],
                    LLL(
                        MSG_ENGINE_BYTES,
                        "%d bytes"
                    ),
                    (long int) thesize
                );
            } else
            {   if (thesize / KILOBYTE >= 1 || thesize == 0)
                {   sprintf((char*) autotext[AUTOLINE_SIZE], "%ld", (long int) (thesize / KILOBYTE));
                } else
                {   autotext[AUTOLINE_SIZE][0] = EOS;
                }
                switch (thesize % KILOBYTE)
                {
                case  256: strcat((char*) autotext[AUTOLINE_SIZE], allglyphs ? "¼" : ".25");
                acase 512: strcat((char*) autotext[AUTOLINE_SIZE], allglyphs ? "½" : ".5" ); // English (1252 or 28605) and Greek (1253) have ½ (at $BD) but Polish (1250) and Russian (1251) don't
                acase 768: strcat((char*) autotext[AUTOLINE_SIZE], allglyphs ? "¾" : ".75");
                }
                strcat((char*) autotext[AUTOLINE_SIZE], LLL(MSG_ENGINE_KILOBYTES, "K"));
        }   }

        sprintf
        (   (char*) autotext[AUTOLINE_STARTADDR],
            "$%X",
            (unsigned int) ((startaddr_h * 256) + startaddr_l)
        );
    }

    j = 0;
    for (i = 0; i <= (int) strlen(overlays[whichoverlay][32]); i++)
    {
#ifdef WIN32
        if (overlays[whichoverlay][32][i] == '\n')
        {   pgmtext[j++] = '\r';
        }
#endif
        pgmtext[j++] = overlays[whichoverlay][32][i];
    }

    if (memmap == MEMMAP_8550 || memmap == MEMMAP_8600)
    {
#ifdef AMIGA
        strcat((char*) pgmtext, "\n\n");
#endif
#ifdef WIN32
        strcat(        pgmtext, "\r\n\r\n");
#endif
        strcat((char*) pgmtext, varianthelp[pong_variant]);
    }

    if (showstatusbars[wsm] && MainWindowPtr)
    {
#ifdef AMIGA
        theimage = getsmallimage1(whichgame, memmap);
        if (!images[theimage])
        {   load_images(theimage, theimage, FALSE);
        }
        SetGadgetAttrs(gadgets[GID_MA_BU1], MainWindowPtr, NULL, GA_Image, images[theimage], TAG_DONE); // this autorefreshes
#endif
#ifdef WIN32
        if (hStatusBar)
        {   SendMessage(hStatusBar, SB_SETICON, 0, (LPARAM) smlicon);
        }
#endif
        dogamename();
}   }

EXPORT void project_reset(FLAG keepalive)
{   if (!keepalive)
    {   NetPlay(NET_OFF);
        updatemenu(MENUITEM_SERVER);
        updatemenu(MENUITEM_CLIENT);
    }

    macro_stop();
    engine_reset();

#ifdef WIN32
    if (cheevos)
    {
#ifdef LOGCHEEVOS
        zprintf(TEXTPEN_VERBOSE, "RA_OnReset()\n");
#endif
        RA_OnReset();
    }
#endif
}

EXPORT UBYTE ReadByte(void)
{   UBYTE result;

    result = IOBuffer[offset++];
    return result;
}
EXPORT UBYTE ReadByteAt(int where)
{   UBYTE result;

    offset = where;
    result = IOBuffer[offset++];
    return result;
}
EXPORT UWORD ReadWord(void)
{   UWORD result;

    // big endian 16-bit word
    result = (UWORD) (256 * IOBuffer[offset])
                          + IOBuffer[offset + 1];
    offset += 2;
    return result;
}
EXPORT UWORD ReadWordAt(int where)
{   UWORD result;

    offset = where;
    // big endian 16-bit word
    result = (UWORD) (256 * IOBuffer[offset])
                          + IOBuffer[offset + 1];
    offset += 2;
    return result;
}
EXPORT ULONG ReadLong(void)
{   ULONG result;

    // big endian 32-bit longword
    result = (ULONG) (16777216 * IOBuffer[offset    ])
                   + (   65536 * IOBuffer[offset + 1])
                   + (     256 * IOBuffer[offset + 2])
                   +             IOBuffer[offset + 3];
    offset += 4;

    return result;
}
EXPORT void WriteByte(UBYTE what)
{   IOBuffer[offset++] = what;
}
EXPORT void WriteWord(UWORD what)
{   // big endian 16-bit word
    IOBuffer[offset    ] = (UBYTE) (what / 256);
    IOBuffer[offset + 1] = (UBYTE) (what % 256);
    offset += 2;
}
EXPORT void WriteLong(ULONG what)
{   // big endian 32-bit word
    IOBuffer[offset    ] = (UBYTE)  (what / 16777216);
    IOBuffer[offset + 1] = (UBYTE) ((what % 16777217) / 65536);
    IOBuffer[offset + 2] = (UBYTE) ((what %    65536) /   256);
    IOBuffer[offset + 3] = (UBYTE)  (what %      256);
    offset += 4;
}

MODULE FLAG autosense(void)
{   int   i;
    ULONG thecrc32; // crc32() is a function name

    if (filesize < 1 || filesize > LARGESTKNOWN)
    {   whichgame = -1;
        return FALSE;
    }

    thecrc32 = crc32(IOBuffer, filesize);
    crc64(IOBuffer, filesize);
#ifdef SHOWCRCS
    zprintf
    (   TEXTPEN_VERBOSE,
        "File size: %d. CRC32: $%08X. CRC64: $%08X%08X\n",
        filesize,
        thecrc32,
        crc64_h,
        crc64_l
    );
#endif

    for (i = 0; i < KNOWNGAMES; i++)
    {   if
        (   known[i].size        == filesize
         && known[i].crc32       == thecrc32
         && known[i].crc64_h     == crc64_h
         && known[i].crc64_l     == crc64_l
        )
        {   whichgame = i;
            return TRUE; // for speed
    }   }

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Unknown game.\n");
#endif

    whichgame = -1;
    return FALSE;
}

EXPORT int bin2int(STRPTR thestring, int length)
{   int i, j,
        value = 0;

    // assert(strlen(thestring) <= 33);

    for (i = length - 1, j = 0; i >= 0; i--, j++)
    {   if (thestring[i] == '-')
        {   if (i == 0)
            {   value = -value;
            } else
            {   return OUTOFRANGE;
        }   }
        else
        {   if (thestring[i] == '1' || thestring[i] == '#')
            {   value |= (1 << j);
            } elif (thestring[i] != '0' && thestring[i] != '.')
            {   return OUTOFRANGE;
    }   }   }

    return value;
}

EXPORT int oct2int(STRPTR thestring, int length)
{   int i, j,
        value = 0;

    // this assumes the string is in range and only has valid characters (0..7)

    length = strlen(thestring);
    for (i = length - 1, j = 0; i >= 0; i--, j++)
    {   if (thestring[i] == '-')
        {   if (i == 0)
            {   value = -value;
            } else
            {   return OUTOFRANGE;
        }   }
        else
        {   value += (thestring[i] - '0') * (1 << (j * 3));
    }   }

    return value;
}

EXPORT FLAG IsZip(STRPTR filename)
{   UBYTE buf[2];
    FILE* fp;

    if ((fp = fopen(filename, "rb")))
    {   DISCARD fread(buf, 2, 1, fp);
        fclose(fp);
        return (FLAG) (memcmp(buf, "PK", 2) == 0);
    }

    return FALSE;
}

EXPORT FLAG loadzip(STRPTR passedname, FLAG auditing, FLAG quiet)
{   TRANSIENT unzFile       fp;
    TRANSIENT unz_file_info info;
    TRANSIENT TEXT          buf[3 + 1];
    TRANSIENT TEXT*         ptr;
    TRANSIENT FLAG          found;
    PERSIST   TEXT          filename_buf[MAX_PATH + 1],
                            tempstring[80 + 1];

    // assert(IsZip(passedname));

    info.uncompressed_size = 0; // to avoid spurious SAS/C compiler warnings

    if (!(fp = (unzFile) unzOpen(passedname)))
    {   if (!quiet) say(LLL(MSG_CANTOPENZIP, "Can't open ZIP archive!"));
        return FALSE;
    } // implied else
    if (unzGoToFirstFile(fp) != 0) // UNZ_OK
    {   DISCARD unzClose(fp);
        if (!quiet)
        {   sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "ZIP");
            say((STRPTR) tempstring);
        }
        return FALSE;
    } // implied else

    found = FALSE;
    for (;;)
    {   // Get file descriptor and analyse
        if (unzGetCurrentFileInfo(fp, &info, (char*) filename_buf, 0x100, NULL, 0, NULL, 0) != 0) // UNZ_OK
        {   break; // implied else
        }
        ptr = (TEXT*) strchr((const char*) filename_buf, '.');
        if (!ptr)
        {   break;
        }
        ptr++; buf[0] = (TEXT) toupper(*ptr);
        ptr++; buf[1] = (TEXT) toupper(*ptr);
        ptr++; buf[2] = (TEXT) toupper(*ptr);
        buf[3] = 0;

        if
        (   !stricmp((const char*) buf, "BIN")
         || !stricmp((const char*) buf, "TVC")
         || (   !auditing
             && (   !stricmp((const char*) buf, "AOF")
                 || !stricmp((const char*) buf, "COS")
                 || !stricmp((const char*) buf, "COR")
                 || !stricmp((const char*) buf, "EOF")
        )   )   )
        {   found = TRUE;
            break;
        }

        // No match so let's try the next file
        if (unzGoToNextFile(fp) != 0) // UNZ_OK
        {   break;
    }   }

    if (!found)
    {   DISCARD unzClose(fp);
        if (!quiet)
        {   say(LLL(MSG_EMPTYZIP, "No Signetics files found in ZIP archive!"));
        }
        return FALSE;
    } // implied else

    if (unzOpenCurrentFile(fp) != 0) // UNZ_OK
    {   DISCARD unzClose(fp);
        if (!quiet)
        {   sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "ZIP");
            say((STRPTR) tempstring);
        }
        return FALSE;
    } // implied else

    ZIPBufferSize = (int) (info.uncompressed_size);
    ZIPBufferPtr  = malloc((unsigned int) ZIPBufferSize);

    if (unzReadCurrentFile(fp, ZIPBufferPtr, (unsigned int) ZIPBufferSize) != (int) ZIPBufferSize)
    {   DISCARD unzCloseCurrentFile(fp);
        DISCARD unzClose(fp);
        if (!quiet)
        {   sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "ZIP");
        }
        return FALSE;
    } // implied else

    DISCARD unzCloseCurrentFile(fp);
    DISCARD unzClose(fp);
    return TRUE;
}

EXPORT void audit_coinops(void)
{   int i;

    foundgames        =
    found_matched     =
    found_unmatched   =
    found_loadables   =
    found_disks       =
    found_tapes       =
    found_unloadables =
    foundsnaps        =
    foundicons        =
    found_dirs        =
    found_unknowns    = 0;

#ifdef WIN32
    strcpy(auditlist[0].gamename, "\\ (");
#endif
#ifdef AMIGA
    strcpy((char*) auditlist[0].gamename, ": (");
#endif
    strcat
    (   (char*) auditlist[0].gamename,
        LLL(
            MSG_ROOT,
            "root"
        )
    );
    strcat((char*) auditlist[0].gamename, ")");

    for (i = 0; i < BUILTIN_MAX; i++)
    {   auditlist[i].filename[0] = EOS;
        auditlist[i].whichgame   = -1;
        sidebar[i]               = i;
    }

    auditlist[0].machine     = -1;
    auditlist[0].glyph       = IMAGE_ROOT;
    auditlist[0].memmap      = -2;

#ifdef WIN32
    strcpy(        auditlist[1].gamename, ".. (");
#endif
#ifdef AMIGA
    strcpy((char*) auditlist[1].gamename, "/ (");
#endif
    strcat
    (   (char*) auditlist[1].gamename,
        LLL(
            MSG_PARENT,
            "parent"
        )
    );
    strcat((char*) auditlist[1].gamename, ")");
    auditlist[1].machine     = -1;
    auditlist[1].glyph       = IMAGE_PARENT;
    auditlist[1].memmap      = -2;

    if (showbuiltin)
    {   strcpy((char*) auditlist[2].gamename, "Astro Wars");
        auditlist[2].machine     = ZACCARIA;
        auditlist[2].glyph       = memmap_to_smlimage[MEMMAP_ASTROWARS];
        auditlist[2].memmap      = MEMMAP_ASTROWARS;

        strcpy((char*) auditlist[3].gamename, "Galaxia");
        auditlist[3].machine     = ZACCARIA;
        auditlist[3].glyph       = memmap_to_smlimage[MEMMAP_GALAXIA];
        auditlist[3].memmap      = MEMMAP_GALAXIA;

        strcpy((char*) auditlist[4].gamename, "Laser Battle");
        auditlist[4].machine     = ZACCARIA;
        auditlist[4].glyph       = memmap_to_smlimage[MEMMAP_LASERBATTLE];
        auditlist[4].memmap      = MEMMAP_LASERBATTLE;

        strcpy((char*) auditlist[5].gamename, "Lazarian");
        auditlist[5].machine     = ZACCARIA;
        auditlist[5].glyph       = memmap_to_smlimage[MEMMAP_LAZARIAN];
        auditlist[5].memmap      = MEMMAP_LAZARIAN;

        strcpy((char*) auditlist[6].gamename, "Malzak 1");
        auditlist[6].machine     = MALZAK;
        auditlist[6].glyph       = memmap_to_smlimage[MEMMAP_MALZAK1];
        auditlist[6].memmap      = MEMMAP_MALZAK1;

        strcpy((char*) auditlist[7].gamename, "Malzak 2");
        auditlist[7].machine     = MALZAK;
        auditlist[7].glyph       = memmap_to_smlimage[MEMMAP_MALZAK2];
        auditlist[7].memmap      = MEMMAP_MALZAK2;

        strcpy((char*) auditlist[8].gamename, "AY-3-8500/8550");
        auditlist[8].machine     = PONG;
        auditlist[8].glyph       = memmap_to_smlimage[MEMMAP_8550];
        auditlist[8].memmap      = MEMMAP_8550;

        strcpy((char*) auditlist[9].gamename, "AY-3-8600");
        auditlist[9].machine     = PONG;
        auditlist[9].glyph       = memmap_to_smlimage[MEMMAP_8600];
        auditlist[9].memmap      = MEMMAP_8600;

        foundgames = BUILTIN_MAX;
    } else
    {   foundgames = BUILTIN_MIN;
}   }

EXPORT void auditfile(STRPTR passedname, int passedsize, FLAG user)
{   TRANSIENT UBYTE* AuditBufferPtr /* = NULL */ ;
    TRANSIENT FILE*  TheLocalHandle /* = NULL */ ; // LocalHandle is a variable of winbase.h
    TRANSIENT int    i,
                     kind,
                     length;
    TRANSIENT FLAG   found;
    TRANSIENT ULONG  crc; // crc32() is a function name
#ifdef WIN32
    TRANSIENT STRPTR localfilepart;
    PERSIST   TEXT   tempstring[MAX_PATH + 1];
#endif
#ifdef AMIGA
    TRANSIENT int    j;
#endif

    if (foundgames >= FOUNDGAMES_MAX)
    {   return;
    }

    if (IsZip(passedname))
    {   if (!loadzip(passedname, TRUE, TRUE))
        {   return;
        }
        passedsize     = ZIPBufferSize;
        AuditBufferPtr = (UBYTE*) ZIPBufferPtr;
        if (passedsize < 1 || passedsize > LARGESTKNOWN)
        {   found_unknowns++;
            if (user)
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s is unknown.\n", passedname);
            }
            goto DONE;
    }   }
    else
    {   if (passedsize < 1 || passedsize > LARGESTKNOWN)
        {   found_unknowns++;
            if (user)
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s is unknown.\n", passedname);
            }
            return;
        } // implied else
        if (!(TheLocalHandle = fopen((char*) passedname, "rb"))) // just cast for lint
        {   return;
        }
        AuditBufferPtr = malloc((unsigned int) passedsize);
        if (!AuditBufferPtr)
        {   return;
        }
        if (fread(AuditBufferPtr, (size_t) passedsize, 1, TheLocalHandle) != 1)
        {   free(AuditBufferPtr);
            fclose(TheLocalHandle);
            // TheLocalHandle = NULL;
            return;
        }
        fclose(TheLocalHandle);
        // TheLocalHandle = NULL;
    }

    kind = identify(passedname);
    switch (kind)
    {
    case KIND_AOF:
    case KIND_BIN:
    case KIND_TVC:
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Calculating CRCs for %d bytes of \"%s\".\n", passedsize, passedname);
#endif
        crc = crc32(AuditBufferPtr, passedsize);
        crc64(AuditBufferPtr, passedsize);
#ifdef SHOWCRCS
        zprintf(TEXTPEN_VERBOSE, "0x%08X, 0x%08X, 0x%08X, \"%s\"\n", crc, crc64_h, crc64_l, passedname);
#endif
        found = FALSE;
        for (i = 0; i < KNOWNGAMES; i++)
        {   if
            (   known[i].size    == passedsize
             && known[i].crc32   == crc
             && known[i].crc64_h == crc64_h
             && known[i].crc64_l == crc64_l
            )
            {   strcpy((char*) auditlist[foundgames].filename, passedname);
                strcpy((char*) auditlist[foundgames].gamename, known[i].name); // we don't bother looking for underscores in these
                auditlist[foundgames].whichgame = (SWORD) i;
                auditlist[foundgames].machine   = known[i].machine;
                auditlist[foundgames].memmap    = known[i].memmap;
                auditlist[foundgames].glyph     = getsmallimage2(auditlist[foundgames].whichgame, auditlist[foundgames].memmap);
                sidebar[foundgames]             = foundgames;
                foundgames++;
                found_matched++;
                found = TRUE;
                break; // for speed
        }   }
        if (!found)
        {   found_unmatched++;
            if (user)
            {   zprintf(TEXTPEN_CLIOUTPUT, "%s is an unknown AOF/BIN/TVC.\n", passedname);
        }   }
    acase KIND_CMD:
    case KIND_EOF:
    case KIND_IMAG:
    case KIND_HEX:
    case KIND_MOD:
    case KIND_ASM:
    case KIND_BPNF:
    case KIND_SMS:
    case KIND_SYM:
        found_loadables++;
    acase KIND_IMG:
    case KIND_RAW:
    case KIND_TWIN:
        found_disks++;
    acase KIND_8SVX:
    case KIND_16SV:
    case KIND_AIFF:
    case KIND_MDCR:
    case KIND_WAV:
    case KIND_PAP:
        found_tapes++;
    acase KIND_ILBM:
    case KIND_ACBM:
    case KIND_BMP:
    case KIND_PCX:
    case KIND_GIF:
    case KIND_TIFF:
    case KIND_PNG:
    case KIND_IFFANIM:
    case KIND_PSG:
    case KIND_YM:
    case KIND_MNG:
    case KIND_AVIANIM:
    case KIND_MIDI:
    case KIND_SMUS:
    case KIND_ASCII:
        found_unloadables++;
    acase KIND_ICON:
        foundicons++;
    acase KIND_COS:
    case KIND_COR:
        length = strlen(passedname);
        for (i = 0; i < length; i++)
        {   if (passedname[i] >= 'a' && passedname[i] <= 'z')
            {   passedname[i] -= 32; // convert to uppercase
        }   }
        strcpy((char*) auditlist[foundgames].filename, passedname);
#ifdef AMIGA
        strcpy((char*) auditlist[foundgames].gamename, FilePart(passedname));

        // now duplicate the first '_', if any, because ReAction treats it as an underline
        length = strlen((const char*) auditlist[foundgames].gamename);
        for (i = 0; i < length; i++)
        {   if (auditlist[foundgames].gamename[i] == '_')
            {   for (j = length; j >= i; j--)
                {   auditlist[foundgames].gamename[j + 1] = auditlist[foundgames].gamename[j];
                }
                break;
        }   }
#endif
#ifdef WIN32
        DISCARD GetFullPathName
        (   passedname,
            MAX_PATH,
            tempstring,
            &localfilepart
        );
        strcpy(auditlist[foundgames].gamename, localfilepart);
#endif

        // we should probably check it's a supported COS/COR version
        auditlist[foundgames].memmap    = AuditBufferPtr[3];
        auditlist[foundgames].machine   = memmapinfo[auditlist[foundgames].memmap].machine;
        if
        (   auditlist[foundgames].machine == PONG
         || auditlist[foundgames].machine == TYPERIGHT
        )
        {   auditlist[foundgames].whichgame = -1;
        } else
        {   auditlist[foundgames].whichgame = (AuditBufferPtr[36] * 256)
                                            +  AuditBufferPtr[37];
        }
        auditlist[foundgames].glyph     = getsmallimage2(auditlist[foundgames].whichgame, auditlist[foundgames].memmap);
        sidebar[foundgames]             = foundgames;
        foundgames++;
        foundsnaps++;
    acase KIND_NIL:
        found_unknowns++;
        if (user)
        {   zprintf(TEXTPEN_CLIOUTPUT, "%s is unknown.\n", passedname); // Unicode pathnames can cause endless loops of this?
    }   }

DONE:
    // assert(AuditBufferPtr);
    free(AuditBufferPtr);
    // AuditBufferPtr = NULL;
}

EXPORT void alloc_iobuffer(ULONG thesize)
{   // If, for example, we were playing an EAR and then a BIN was dragged
    // over, we will need to free the old IOBuffer

    if (!thesize)
    {   return;
    }

    free_iobuffer();
    IOBuffer = (UBYTE*) malloc(thesize);
#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Allocated I/O buffer.\n");
#endif
}

EXPORT void free_iobuffer(void)
{   if (!IOBuffer)
    {   return;
    }

    free(IOBuffer);
    IOBuffer = NULL;
#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Freed I/O buffer.\n");
#endif
}

EXPORT void endofframe(UNUSED int newsky)
{   int i,
        resolved;
#if defined(WIN32) && defined(USEACTIVISIONSAMPLES)
PERSIST const STRPTR outfn[11] = {
   "Sounds\\OUT01.WAV", // phone: girlfriend (1st)
   "Sounds\\OUT02.WAV", // door:  brother
   "Sounds\\OUT03.WAV", // door:  sister
   "Sounds\\OUT04.WAV", // door:  brother
// "Sounds\\OUT05.WAV", // phone: friend (has Master of the Lamps reference)
   "Sounds\\OUT06.WAV", // door:  mother
   "Sounds\\OUT07.WAV", // phone: girlfriend (2nd)
   "Sounds\\OUT08.WAV", // door:  father
   "Sounds\\OUT09.WAV", // TV:    station close
   "Sounds\\OUT10.WAV", // phone: girlfriend (3rd)
   "Sounds\\OUT11.WAV", // TV:    station open
};
#endif
    FAST FLAG ok;

#ifndef BENCHMARK_EMU
    if (frameskip != FRAMESKIP_INFINITE && frames % (ULONG) frameskip == 0)
    {   updatescreen();
    }
#endif
#ifndef EMULATE_CPU
    cycles_2650 += (ULONG) machines[machine].cpf;
#endif

#ifdef WIN32
    sky = enhancestars ? newsky : ((UBYTE) -1);
#endif
#ifdef AMIGA
    end_samples(); // it is only exact to the nearest frame
#endif

    frames++;
    if ((tapemode == TAPEMODE_PLAY || tapemode == TAPEMODE_RECORD) && frames % 3 == 0)
    {   if (tapeframe == 5)
        {   tapeframe = 0;
        } else
        {   tapeframe++;
    }   }

    inframe = FALSE;
    if (crippled)
    {   uncripple();
    }

    resolved = resolvegame();
    if (resolved != -1)
    {   hiscore[resolved].playedmicro += (region == REGION_PAL) ? 20000 : 16667;
        if (hiscore[resolved].playedmicro >= ONE_MILLION)
        {   hiscore[resolved].playedmicro -= ONE_MILLION;
            hiscore[resolved].played++;
        }
        if
        (   (machine == ARCADIA || machines[machine].coinop || machine == PONG)
         && collisions
         && speedup >= SPEED_4QUARTERS
         && recmode != RECMODE_PLAY
         && !paused
         && (machine != ARCADIA || (whichgame != -1 && known[whichgame].hiscore != -1))
         && (machine == PONG || (!trainer_lives && !trainer_time && !trainer_invincibility))
        )
        {   updatehiscores(resolved);
    }   }

    if (autocoin && machines[machine].coinop)
    {   do_autocoin();
    }

    if (recmode == RECMODE_PLAY && offset >= recsize)
    {   if (loop && offset > startoffset)
        {   macro_restartplayback();
        } else
        {   macro_stop();
    }   }

    for (i = 0; i < GUESTCHANNELS; i++)
    {   if (chan_status[i] == CHAN_PLAY)
        {   play_any(i, chan_hertz[i], chan_volume[i]);
            chan_status[i] = CHAN_OK;
        } elif (chan_status[i] == CHAN_PLAYTHENSTOP)
        {   play_any(i, chan_hertz[i], chan_volume[i]);
            chan_status[i] = CHAN_STOP;
        } elif (chan_status[i] == CHAN_STOP)
        {   sound_stop(i);
            chan_status[i] = CHAN_OK;
    }   }

#if defined(WIN32) && defined(USEACTIVISIONSAMPLES)
    if (sound && hostvolume && ambient && !(rand() % (((region == REGION_PAL) ? 50 : 60) * SECSPERSAMPLE)))
    {   DISCARD PlaySound(outfn[rand() % 11], NULL, SND_ASYNC);
    }
#endif

    ok = TRUE;
    if (belling[0])
    {   belling[0]--;
        if (!belling[0])
        {   ok = FALSE;
    }   }
    if (belling[1])
    {   belling[1]--;
        if (!belling[1])
        {   ok = FALSE;
    }   }
    if (!ok)
    {   playsound(FALSE);
    }

    if (runtoframe)
    {   clearruntos();
        zprintf
        (   TEXTPEN_DEBUG,
            LLL(
                MSG_XVI_REACHEDFRAME,
                "Reached next frame (%d).\n\n"
            ),
            (long int) frames
        );
        emu_pause();
    }

#ifdef AMIGA
    if (clicked != -1)
    {   handle_keyup(clicked);
        clicked = -1;
    }
#endif
#ifdef WIN32
    if (cheevos)
    {
#ifdef LOGALLCHEEVOS
        zprintf(TEXTPEN_VERBOSE, "RA_DoAchievementsFrame()\n");
#endif
        RA_DoAchievementsFrame();
    }
#endif

    process_code();

    update_monitor(FALSE);
}

MODULE int resolvegame(void)
{   switch (machine)
    {
    case ARCADIA:
        if (whichgame != -1)          return known[whichgame].hiscore;
    acase ZACCARIA:
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:        return HS_ASTROWARS_POS;
        case MEMMAP_GALAXIA:          return HS_GALAXIA_POS;
        case MEMMAP_LASERBATTLE:      return HS_LASERBATTLE_POS;
        case MEMMAP_LAZARIAN:         return HS_LAZARIAN_POS;
        }
    acase MALZAK:
        switch (memmap)
        {
        case MEMMAP_MALZAK1:          return HS_MALZAK1_POS;
        case MEMMAP_MALZAK2:          return HS_MALZAK2_POS;
        }
    acase PONG:
        switch (pong_variant)
        {
        case VARIANT_8550_PRACTICE:   return HS_PRACTICE_POS;
        case VARIANT_8550_RIFLE1:     return HS_RIFLE1_POS;
        case VARIANT_8550_RIFLE2:     return HS_RIFLE2_POS;
        case VARIANT_8600_PRACTICE:   return HS_PRACTICE2_POS;
        case VARIANT_8600_BBPRACTICE: return HS_BBPRACTICE_POS;
        case VARIANT_8600_TARGET1:    return HS_TARGET1_POS;
    }   }

    return -1;
}

EXPORT void wait_first(void)
{   FAST ULONG waitfor;

    newtime = thetime();

    if (!turbo)
    {   if (ctrl() & KeyDown(SCAN_APOSTROPHE))
        {   waitfor = ONE_MILLION;
        } else
        {   waitfor = usecsperframe[region];
            switch (speedup)
            {
            case  SPEED_1QUARTER:   /*  25% */ waitfor *= 4;
            acase SPEED_2QUARTERS:  /*  50% */ waitfor *= 2;
            acase SPEED_3QUARTERS:  /*  75% */ waitfor =  (waitfor * 4) / 3; // each frame takes 133.3'% of its normal time. Ie. in 4 frames worth of normal time, we've done 3 frames
            acase SPEED_5QUARTERS:  /* 125% */ waitfor =  (waitfor * 4) / 5; // each frame takes  80   % of its normal time. Ie. in 4 frames worth of normal time, we've done 5 frames
            acase SPEED_6QUARTERS:  /* 150% */ waitfor =  (waitfor * 2) / 3; // each frame takes  66.6'% of its normal time. Ie. in 4 frames worth of normal time, we've done 6 frames
            acase SPEED_8QUARTERS:  /* 200% */ waitfor /= 2;
            acase SPEED_16QUARTERS: /* 400% */ waitfor /= 4;
            acase SPEED_32QUARTERS: /* 800% */ waitfor /= 8;
        }   }
        waittill += waitfor;

        if (waittill < newtime) // already too late
        {   waittill = newtime + waitfor;
}   }   }

EXPORT void wait_second(void)
{   thesleep(0);
    newtime = thetime();

    if (!turbo)
    {   if
        (   autoframeskip
         && frameskip < FRAMESKIP_MAX
         && newtime   > waittill
        )
        {   badframes++;
            if (badframes >= BADFRAMES)
            {   badframes = 0;
                frameskip++;
                if (SubWindowPtr[SUBWINDOW_SPEED])
                {
#ifdef WIN32
                    sprintf(gtempstring, "%d", frameskip);
                    st_set(SUBWINDOW_SPEED, IDC_FRAMESKIP_NUM);
#endif
#ifdef AMIGA
                    SetGadgetAttrs(gadgets[GID_SD_IN2], SubWindowPtr[SUBWINDOW_SPEED], NULL, INTEGER_Number, (ULONG) frameskip, TAG_DONE); // this autorefreshes
#endif
                    sl_set(SUBWINDOW_SPEED, IDC_FRAMESKIP, frameskip);
        }   }   }
        else
        {   badframes = 0;
        }

        while (newtime < waittill)
        {   if (newtime + 1000 >= waittill) // 1 millisecond
            {   thesleep(990); // not 1000! 999 seems OK but we give it some margin just in case
            } else
            {   thesleep(0); // with thesleep(1) we get occasional slowdown
            }
            newtime = thetime();
    }   }

    if (newtime - oldfpstime >= ONE_MILLION)
    {   update_fps();
}   }

EXPORT void update_fps(void)
{   FAST double amount,
                elapsedcyc,
                elapsedtime,
                idealpersec,
                localfps;
    FAST ULONG  ulongval;

    if (!MainWindowPtr || !showstatusbars[wsm])
    {   return;
    }
#ifdef WIN32
    // assert(hStatusBar);
#endif

    if (machine == PONG || machine == TYPERIGHT)
    {   elapsedcyc = (double) (frames      - oldfpscyc ); // elapsed frames since last update (in microseconds)
        oldfpscyc  = frames;
    } else
    {   elapsedcyc = (double) (cycles_2650 - oldfpscyc ); // elapsed cycles since last update
        oldfpscyc  = cycles_2650;
    }
    elapsedtime    = (double) (newtime     - oldfpstime); // elapsed time   since last update (in microseconds)
    oldfpstime     = newtime;

    if (elapsedcyc == 0.0 || elapsedtime == 0.0)
    {   StatusText[2][0] = '-';
        StatusText[2][1] = EOS;
    } else
    {   elapsedtime /= 1000000.0;                       // convert from microseconds to seconds
        elapsedcyc  /= elapsedtime;                     // scale elapsedcyc as if it was for one second
        if (exactspeed)
        {   localfps = (region == REGION_NTSC) ? 60.0 : 50.0;
        } else
        {   localfps = machines[machine].fps[region];
        }

        switch (viewspeedas)
        {
        case VIEWAS_FPS:
            switch (machine)
            {
            case ARCADIA:
            case INTERTON:
            case ELEKTOR:
                idealpersec = machines[machine].cpf * localfps * cpl / 227;
            adefault:
                idealpersec = machines[machine].cpf * localfps;
            }
            amount =  elapsedcyc / idealpersec;
            amount *= localfps;
         // sprintf(StatusText[2], "%f", amount);
            amount += 0.5; // so we round to the nearest integer instead of always down
            ulongval = (ULONG) amount;
            if (ulongval > 99999)
            {   strcpy((char*) StatusText[2], "#####");
            } else
            {   sprintf((char*) StatusText[2], "%u", (unsigned int) ulongval); // ULONG and unsigned int aren't considered equivalent by GCC 9
            }
        acase VIEWAS_KHZ:
            if (machine == PONG || machine == TYPERIGHT)
            {   StatusText[2][0] = '-';
                StatusText[2][1] = EOS;
            } else
            {   ulongval = (ULONG) elapsedcyc;
                if (ulongval >= ONE_BILLION)
                {   strcpy((char*) StatusText[2], ">= 1 GHz");
                } elif (ulongval >= ONE_MILLION) // 1-999 MHz
                {   sprintf
                    (   (char*) StatusText[2],
                        "%u,%03u.%01u", // eg. 123,456.7   is 123.4567 MHz
                        (unsigned int)  (ulongval / ONE_MILLION),
                        (unsigned int) ((ulongval % ONE_MILLION) / 1000),
                        (unsigned int) ((ulongval % 1000       ) /  100)
                    );
                } else // < 1 MHz
                {   sprintf
                    (   (char*) StatusText[2],
                        "%u.%03u",      // eg.     123.456 is 123.456  KHz
                        (unsigned int)  (ulongval /        1000),
                        (unsigned int)  (ulongval %        1000)
                    );
                    // ULONG and unsigned int aren't considered equivalent by GCC 9
            }   }
        acase VIEWAS_PERCENTS:
            switch (machine)
            {
            case ARCADIA:
            case INTERTON:
            case ELEKTOR:
                idealpersec = machines[machine].cpf * localfps * cpl / 227;
            adefault:
                idealpersec = machines[machine].cpf * localfps;
            }
            amount = elapsedcyc / idealpersec;
            amount *= 100.0;
         // sprintf(StatusText[2], "%f%%", amount);

            amount += 0.5; // so we round to the nearest integer instead of always down
            ulongval = (ULONG) amount;
            if (ulongval > 9999)
            {   strcpy( (char*) StatusText[2], "####%");
            } else
            {   sprintf((char*) StatusText[2], "%u%%", (unsigned int) ulongval); // ULONG and unsigned int aren't considered equivalent by GCC 9
    }   }   }

    sprintf
    (   (char*) &StatusText[2][strlen((const char*) StatusText[2])],
        " %s ",
        (STRPTR) LLL(MSG_OF, "of")
    );

    if (turbo)
    {   strcat((char*) StatusText[2], LLL( MSG_TURBO2, "Turbo"));
    } else
    {   switch (viewspeedas)
        {
        case VIEWAS_FPS:
            if (ctrl() && KeyDown(SCAN_APOSTROPHE))
            {   strcat(StatusText[2], "1");
            } else
            {   sprintf
                (   (char*) ENDOF(StatusText[2]),
                    "%0.2f",
                    (localfps * speedupnum[speedup]) / 100
                );
            }
        acase VIEWAS_KHZ:
            if (machine == ARCADIA || machine == INTERTON || machine == ELEKTOR)
            {   if (ctrl() && KeyDown(SCAN_APOSTROPHE))
                {   sprintf
                    (   (char*) ENDOF(StatusText[2]),
                        "%0.3f", // so, effectively showing it as a whole number of hertz
                        (                                 machines[machine].cpf  /   1000.0)   * cpl  / 227
                    );
                } else
                {   sprintf
                    (   (char*) ENDOF(StatusText[2]),
                        "%0.3f", // so, effectively showing it as a whole number of hertz
                        (((localfps * machines[machine].cpf) / 100000.0) * speedupnum[speedup] * cpl) / 227
                    );
            }   }
            else
            {   if (ctrl() && KeyDown(SCAN_APOSTROPHE))
                {   sprintf
                    (   (char*) ENDOF(StatusText[2]),
                        "%f",
                                                          machines[machine].cpf  /   1000.0
                    );
                } else
                {   sprintf
                    (   (char*) ENDOF(StatusText[2]),
                        "%f",
                        ((localfps                      * machines[machine].cpf) / 100000.0) * speedupnum[speedup]
                    );
            }   }
        acase VIEWAS_PERCENTS:
            if (ctrl() && KeyDown(SCAN_APOSTROPHE))
            {   strcat(StatusText[2], "2%");
            } else
            {   sprintf
                (   (char*) ENDOF(StatusText[2]),
                    "%d%%",
                    speedupnum[speedup]
                );
    }   }   }

#ifdef AMIGA
    SetGadgetAttrs(gadgets[GID_MA_ST2], MainWindowPtr, NULL, STRINGA_TextVal, StatusText[2], TAG_DONE); // this autorefreshes
#endif
#ifdef WIN32
    SendMessage(hStatusBar, SB_SETTEXT, 3, (LPARAM) StatusText[2]);
#endif
}

EXPORT int readnum_hex(STRPTR input, int length)
{   int i,
        value;

    lastparse = BASE_HEX;

    for (i = 0; i < length; i++)
    {   if
        (   (input[i] >= 'a' && input[i] <= 'f')
         || (input[i] >= 'A' && input[i] <= 'F')
         || (input[i] >= '0' && input[i] <= '9')
         ||  input[i] == '-'
        )
        {   ;
        } else
        {   return OUTOFRANGE;
    }   }

    DISCARD stch_l(input, (void*) &value); // hexstring -> number
    return value;
}

EXPORT int readnum_decimal(STRPTR input, int length)
{   int  i,
         value;
    TEXT tempstring[80 + 1];

    lastparse = BASE_DECIMAL;

    for (i = 0; i < length; i++)
    {   if ((input[i] < '0' || input[i] > '9') && input[i] != '-')
        {   return OUTOFRANGE;
    }   }

    strcpy(tempstring, input);
    tempstring[length] = EOS;
    DISCARD stcd_l(tempstring, (void*) &value); // decstring -> number

    return value;
}

EXPORT int readnum_binary(STRPTR input, int length)
{   int i;

    lastparse = BASE_BINARY;

    for (i = 0; i < length; i++)
    {   if
        (   input[i] != '0'
         && input[i] != '1'
         && input[i] != '.'
         && input[i] != '#'
         && input[i] != '-'
        )
        {   return OUTOFRANGE;
    }   }
    return bin2int(input, length);
}

EXPORT int readnum_octal(STRPTR input, int length)
{   int i;

    lastparse = BASE_OCTAL;

    for (i = 0; i < length; i++)
    {   if (input[i] != '-' && (input[i] < '0' || input[i] > '7'))
        {   return OUTOFRANGE;
    }   }

    return oct2int(input, length);
}

EXPORT int readnum_char(TEXT input)
{   int  i;
    TEXT uppercase;

    lastparse = BASE_CHAR;

    switch (machine)
    {
    case ARCADIA:
        uppercase = toupper(input);
        for (i = 0; i < 64; i++)
        {   if (uppercase == arcadia_chars[i])
            {   return i;
        }   }
    acase ELEKTOR:
        uppercase = toupper(input);
        for (i = 0; i < 256; i++)
        {   if (uppercase == elektor_chars1[i]) // what about elektor_chars2[]?
            {   return i;
        }   }
    acase INSTRUCTOR:
        if (input == '.')
        {   return 0x1A;
        }
        uppercase = toupper(input);
        for (i = 0; i <= 0x1C; i++)
        {   if (uppercase == instructor_chars1[i])
            {   return i;
        }   }
    acase ZACCARIA:
        uppercase = toupper(input);
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:
            for (i = 0; i < 256; i++)
            {   if (uppercase == astrowars_chars[i])
                {   return i;
            }   }
        acase MEMMAP_GALAXIA:
            for (i = 0; i < 128; i++)
            {   if (uppercase == galaxia_chars[i])
                {   return i;
            }   }
        acase MEMMAP_LASERBATTLE:
        case MEMMAP_LAZARIAN:
            for (i = 0; i < 256; i++)
            {   if (uppercase == lb_chars[i])
                {   return i;
        }   }   }
    adefault: // eg. PIPBUG, BINBUG, CD2650, MALZAK, PHUNSY, SELBST, MIKIT
        return input; // ASCII
    }

    return OUTOFRANGE;
}

#ifdef WIN32
EXPORT void zprintf(int whichcolour, const char* format, ...)
#endif
#ifdef AMIGA
EXPORT void zprintf(UNUSED int whichcolour, const char* format, ...)
#endif
{   TRANSIENT va_list list;
    PERSIST   char    string[16384]; // PERSISTent so as not to blow the stack
    FAST      int     i,
                      length;
#ifdef WIN32
    FAST      UBYTE   currenttextpen;
    FAST      int     j,
                      prevpos;
PERSIST const int textpen_to_rgb[] =
{
// on black background
  RGB( 64,  64,  64), // 0          black (not used)
  RGB(255,   0,   0), // 1          TEXTPEN_RED
  RGB(  0, 255,   0), // 2          TEXTPEN_GREEN
  RGB(255, 255,   0), // 3          TEXTPEN_YELLOW
  RGB(128, 128, 255), // 4          TEXTPEN_BLUE
  RGB(255,   0, 255), // 5          purple
  RGB(  0, 255, 255), // 6          cyan
  RGB(255, 255, 255), // 7          white
  RGB(255, 128,   0), // 8          orange
  RGB(128, 128, 128), // 9          grey
// on white background
  RGB(  0,   0,   0), // 0+TEXTPENS black (not used)
  RGB(192,   0,   0), // 1+TEXTPENS TEXTPEN_RED
  RGB(  0, 128,   0), // 2+TEXTPENS TEXTPEN_GREEN
  RGB( 96,  96,   0), // 3+TEXTPENS TEXTPEN_YELLOW
  RGB(  0,   0, 255), // 4+TEXTPENS TEXTPEN_BLUE
  RGB(128,   0, 128), // 5+TEXTPENS purple
  RGB(  0, 128, 128), // 6+TEXTPENS cyan
  RGB(128, 128, 128), // 7+TEXTPENS grey
  RGB(255, 160,   0), // 8+TEXTPENS orange
  RGB( 64,  64,  64), // 9+TEXTPENS dark grey
};
#endif

    // Be aware that this function isn't callable recursively
    // (due to the use of persistent variables).

#ifdef AMIGA
    if (quiet)
    {   return;
    }
    consoleopened = TRUE;
#endif

    string[0] = EOS;
    if (!LogfileHandle && logfile != LOGFILE_IGNORE)
    {   logfile_open(TRUE);
        write_log_string(string);
    }

    va_start(list, format);
    DISCARD vsprintf((char*) ENDOF(string), format, list);
    va_end(list);

#ifdef WIN32
    if
    (   ( climode && !consoleopened)
     || (!climode && !SubWindowPtr[SUBWINDOW_OUTPUT])
    )
    {   open_output(TRUE);
    }
    if (colourlog && textcolour != whichcolour)
    {   textpen(whichcolour);
    }
#endif

    length = strlen(string);
    if (LogfileHandle)
    {   DISCARD fwrite(string, length, 1, LogfileHandle);
    }
    if (DisHandle)
    {   DISCARD fwrite(string, length, 1, DisHandle);
    }

#ifdef WIN32
    if (climode)
    {   printf(string);
        return;
    }

    if (outputpos + length >= OUTPUTLENGTH)
    {   cls();
    }

    j = outputpos;
    for (i = 0; i < length; i++)
    {   switch (string[i])
        {
        case  0xA1: /*      161  */ consolestring[j++] = '!'; // inverted question mark
        acase 0xAF: /*      175  */ consolestring[j++] = '_'; // overscore
        acase '·':  /* $B7 (183) */ consolestring[j++] = '.';
        acase '¾':  /* $BC (188) */ consolestring[j  ] = '3'; consolestring[j + 1] = '/'; consolestring[j + 2] = '4'; j += 3; // 3/4
        acase '½':  /* $BD (189) */ consolestring[j  ] = '1'; consolestring[j + 1] = '/'; consolestring[j + 2] = '2'; j += 3; // 1/2
        acase '¼':  /* $BE (190) */ consolestring[j  ] = '1'; consolestring[j + 1] = '/'; consolestring[j + 2] = '4'; j += 3; // 1/4
        adefault:
            if (string[i] == CR || (string[i] >= 180 && !(string[i] & 1)) || string[i] >= 192)
            {   ;
            } else
            {   consolestring[j++] = string[i];
    }   }   }
    consolestring[j] = EOS;
    memset(&colourtext[outputpos], textcolour, j - outputpos);
    outputpos = j;

    if (!hurry)
    {   DISCARD SendMessage
        (   RichTextGadget,
            WM_SETTEXT,
            0,
            (LPARAM) consolestring
        );
        DISCARD SendMessage
        (   RichTextGadget,
            WM_VSCROLL,
            SB_BOTTOM,
            0
        );
        DISCARD SendMessage
        (   RichTextGadget,
            EM_SCROLLCARET,
            0,
            0
        );

        if (colourlog)
        {   currenttextpen = (UBYTE) -1;
            prevpos = 0;
            for (i = 0; i < outputpos; i++)
            {   if (colourtext[i] != currenttextpen)
                {   fgformat.crTextColor = textpen_to_rgb[currenttextpen + ((consolebg == 1) ? TEXTPENS : 0)];
                    if (consolebg == 0) // black
                    {   fgformat.crBackColor = RGB(  0,   0,   0);
                    } else
                    {   // assert(consolebg == 1); // white
                        fgformat.crBackColor = RGB(255, 255, 255);
                    }
                    DISCARD SendMessage
                    (   RichTextGadget,
                        EM_SETSEL,
                        prevpos,
                        i
                    );
                    DISCARD SendMessage
                    (   RichTextGadget,
                        EM_SETCHARFORMAT,
                        SCF_SELECTION,
                        (LPARAM) &fgformat
                    );
                    currenttextpen = colourtext[i];
                    prevpos = i;
            }   }
            fgformat.crTextColor = textpen_to_rgb[currenttextpen + ((consolebg == 1) ? TEXTPENS : 0)];
            if (consolebg == 0) // black
            {   fgformat.crBackColor = RGB(  0,   0,   0);
            } else
            {   // assert(consolebg == 1); // white
                fgformat.crBackColor = RGB(255, 255, 255);
            }
            DISCARD SendMessage
            (   RichTextGadget,
                EM_SETSEL,
                prevpos,
                outputpos
            );
            DISCARD SendMessage
            (   RichTextGadget,
                EM_SETCHARFORMAT,
                SCF_SELECTION,
                (LPARAM) &fgformat
            );
        }

        if (!quitting)
        {   wa_checkinput();
            process_code();
    }   }
#endif
#ifdef AMIGA
    i = 0;
    // puts() always write newlines, it is shit!
    while (string[i])
    {   putchar(string[i]);
        i++; // this must be separate to the above line (for VBCC, at least)!
    }
    aa_checkinput();
#endif
}

EXPORT void logfile_open(FLAG quiet)
{   PERSIST char string[256 + 1]; // PERSISTent so as not to blow the stack
    // If we allowed the user to specify their own filename, string[]
    // would need to be enlarged (to MAX_PATH + enough for the rest of the
    // string).

    // assert(logfile != LOGFILE_IGNORE);

    cd_projects();

    if (!(LogfileHandle = fopen(LOGFILENAME, ((logfile == LOGFILE_REPLACE) ? "w" : "a"))))
    {   logfile = LOGFILE_IGNORE;
        updatemenu(MENUFAKE_LOGTOFILE);
    }

    if (!quiet) // otherwise caller does it
    {   write_log_string(string);
        zprintf
        (   TEXTPEN_LOG,
            string
        );
    }

    cd_progdir();
}

EXPORT void logfile_close(void)
{   if (LogfileHandle)
    {   datestamp2();
        zprintf
        (   TEXTPEN_LOG,
            LLL(MSG_LOGCLOSED, "Closed %s at %s.\n\n"),
            LOGFILENAME,
            datetimestring
        );
        // order-dependent!
        DISCARD fclose(LogfileHandle); // this takes about a second on Win8.1 for some reason
        LogfileHandle = NULL;
}   }

EXPORT void dosidebar(void)
{   int done = FALSE,
        i,
        length;

    switch (auditlist[sidebar[currentgame]].memmap)
    {
    case -2:
        if (sidebar[currentgame] == 0)
        {   strcpy((char*) path_games, STRING_ROOT);
        } elif (sidebar[currentgame] == 1)
        {   if (path_games[strlen((const char*) path_games) - 1] != CHAR_PARENT)
            {   strcat((char*) path_games, STRING_PARENT);
            }
            length = strlen((const char*) path_games);
            if (length >= 2)
            {   for (i = length - 2; i >= 0; i--)
                {   if (path_games[i] == CHAR_PARENT || path_games[i] == CHAR_ROOT)
                    {   path_games[i + 1] = EOS;
                        done = TRUE;
                        break;
            }   }   }
            else
            {   done = TRUE;
            }
            if (!done)
            {   path_games[0] = EOS;
        }   }
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Root/parent. %s!\n", path_games);
#endif
        audit(FALSE);
        setselection();
    acase -1:
#ifdef WIN32
        if (path_games[strlen(path_games) - 1] != CHAR_PARENT)
        {   strcat(path_games, STRING_PARENT);
        }
        strcat(path_games, auditlist[sidebar[currentgame]].filename);
#endif
#ifdef AMIGA
        DISCARD AddPart((STRPTR) path_games, (const char*) auditlist[sidebar[currentgame]].filename, MAX_PATH + 1);
#endif
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Subdirectory. %s!\n", path_games);
#endif
        audit(FALSE);
        setselection();
    adefault:
        if (auditlist[sidebar[currentgame]].filename[0])
        {   strcpy((char*) fn_game, (const char*) auditlist[sidebar[currentgame]].filename);
            break_pathname((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game);
            if (engine_load(FALSE))
            {   add_recent();
            } else
            {   beep();
        }   }
        elif (memmap == auditlist[sidebar[currentgame]].memmap)
        {   docommand(MENUITEM_REINITIALIZE);
        } else
        {   switch (auditlist[sidebar[currentgame]].memmap)
            {
            case  MEMMAP_ASTROWARS:   command_changemachine(ZACCARIA, MEMMAP_ASTROWARS);
            acase MEMMAP_GALAXIA:     command_changemachine(ZACCARIA, MEMMAP_GALAXIA);
            acase MEMMAP_LASERBATTLE: command_changemachine(ZACCARIA, MEMMAP_LASERBATTLE);
            acase MEMMAP_LAZARIAN:    command_changemachine(ZACCARIA, MEMMAP_LAZARIAN);
            acase MEMMAP_MALZAK1:     command_changemachine(MALZAK  , MEMMAP_MALZAK1);
            acase MEMMAP_MALZAK2:     command_changemachine(MALZAK  , MEMMAP_MALZAK2);
            acase MEMMAP_8550:        command_changemachine(PONG    , MEMMAP_8550);
            acase MEMMAP_8600:        command_changemachine(PONG    , MEMMAP_8600);
    }   }   }

#ifdef AMIGA
    updatesidebartips();
#endif
}

EXPORT int load_aof(int localsize, FLAG intel)
{   FLAG foundeof = FALSE;
    int  rc       =     1;

    cursor = 0;
    while (cursor < localsize)
    {   if (intel)
        {   rc = iof_readblock(localsize);
        } else
        {   rc = aof_readblock(localsize);
        }

        if (rc == -1)
        {   foundeof = TRUE;
            break;
        } elif (rc >= 0)
        {   break;
    }   }

    if (rc <= 0)
    {   if (intel)
        {   zprintf(TEXTPEN_CLIOUTPUT, "Successfully loaded %s file.\n\n", "Intel HEX");
        }
#ifdef VERBOSE
        else
        {   zprintf(TEXTPEN_CLIOUTPUT, "Successfully loaded %s file.\n\n", "AOF");
        }
#endif
        if (!foundeof)
        {   zprintf(TEXTPEN_ERROR, "Warning: no end-of-file block found.\n");
            return 1;
        } else
        {   return 2;
    }   }
    else
    {   zprintf(TEXTPEN_ERROR, "Failed to load %s file!\n\n", intel ? "Intel HEX" : "AOF");
        return 0;
}   }

MODULE int aof_readblock(int localsize)
{   UBYTE blocklength,
          ourbcc, // *MUST* be UBYTE!
          theirbcc;
    int   i,
          loadaddr;

/* For normal AOF files:
   0:                                        $3A (':') (block header)
   1                  .. 4:                  load/start address
   5                  .. 6:                  block length
   7                  .. 8:                  1st BCC
   9                  .. 8+(block length*2): data
   9+(block length*2) ..10+(block length*2): 2nd BCC

    Return codes are:
       -1  = successful, EOF block
       -2  = successful, data block
        0  = end of file
        1+ = unsuccessful */

    if (cursor == localsize)
    {   return 0;
    }

    if (IOBuffer[cursor] == ':')
    {   if (!isok(cursor + 1, cursor + 6))
        {   zprintf(TEXTPEN_ERROR, "Error: illegal character found in bytes %d..%d!\n", cursor + 1, cursor + 6);
            return 1;
        }

        loadaddr = (UWORD)
        (   (getdigit(IOBuffer[cursor + 1]) * 4096)
          + (getdigit(IOBuffer[cursor + 2]) *  256)
          + (getdigit(IOBuffer[cursor + 3]) *   16)
          +  getdigit(IOBuffer[cursor + 4])
        );
        if (loadaddr >= 0x8000)
        {   zprintf(TEXTPEN_ERROR, "Error: address is out of range!\n");
            return 1;
        }
        blocklength = (UBYTE)
        (   (getdigit(IOBuffer[cursor + 5]) *   16)
          +  getdigit(IOBuffer[cursor + 6])
        );
        if (blocklength == 0) // should we warn the user if blocksize > 30? or at least if it is > 510?
        {   startaddr_h = (UBYTE) (loadaddr / 256);
            startaddr_l = (UBYTE) (loadaddr % 256);
#ifdef VERBOSE
            zprintf(TEXTPEN_CLIOUTPUT, "Found end-of-file block. Start address is $%04X.\n", loadaddr);
#endif
            return -1;
        } // implied else

        if (!isok(cursor + 7, cursor + 10 + (blocklength * 2)))
        {   zprintf(TEXTPEN_ERROR, "Error: illegal character found in bytes %d..%d!\n", cursor + 7, cursor + 10 + (blocklength * 2));
            return 1;
        }

        ourbcc = aof_makebcc(cursor + 1, cursor + 6, IOBuffer);
        theirbcc = (UBYTE)
        (   (getdigit(IOBuffer[cursor + 7]) * 16)
          +  getdigit(IOBuffer[cursor + 8])
        );
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Our header  BCC is $%02X. Their header  BCC is $%02X.\n", ourbcc, theirbcc);
#endif
        if (ourbcc != theirbcc)
        {   zprintf(TEXTPEN_ERROR, "Error: BCC is incorrect!\n");
            return 1;
        }

#ifdef VERBOSE
        zprintf(TEXTPEN_CLIOUTPUT, "Loading $%04X..$%04X.\n", loadaddr, loadaddr + blocklength - 1);
#endif
        if ((loadaddr & 0x6000) != ((loadaddr + blocklength - 1) & 0x6000))
        {   zprintf(TEXTPEN_ERROR, "Warning: Block $%04X..$%04X crosses a page boundary!\n", loadaddr, loadaddr + blocklength - 1);
        }
        for (i = 0; i < blocklength; i++)
        {   memory[loadaddr + i] = (UBYTE)
            (   (getdigit(IOBuffer[cursor +  9 + (i * 2)]) * 16)
              +  getdigit(IOBuffer[cursor + 10 + (i * 2)])
            );
        }

        ourbcc = aof_makebcc(cursor + 9, cursor + 8 + (blocklength * 2), IOBuffer);
        theirbcc = (UBYTE)
        (   (getdigit(IOBuffer[cursor +  9 + (blocklength * 2)]) * 16)
          +  getdigit(IOBuffer[cursor + 10 + (blocklength * 2)])
        );
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Our payload BCC is $%02X. Their payload BCC is $%02X.\n", ourbcc, theirbcc);
#endif
        if (ourbcc != theirbcc)
        {   zprintf(TEXTPEN_ERROR, "Error: BCC is incorrect!\n");
            return 1;
        }

        cursor += 11 + (blocklength * 2);
        while
        (   (   IOBuffer[cursor] == LF
             || IOBuffer[cursor] == CR
             || IOBuffer[cursor] == ' '
            )
         && cursor < localsize
        )
        {   cursor++;
        }
        return -2;
    } else
    {   zprintf(TEXTPEN_ERROR, "Error: block header not found!\n");
        return 1;
}   }

MODULE int iof_readblock(int localsize)
{   UBYTE blocklength,
          blocktype,
          ourbcc, // *MUST* be UBYTE!
          theirbcc;
    int   i,
          loadaddr;

/* For normal Intel HEX files:
   0:                                        $3A (':') (block header)
   1                  .. 2:                  block length
   3                  .. 6:                  load address
   7                  .. 8:                  block type
   9                  .. 8+(block length*2): data
   9+(block length*2) ..10+(block length*2): checksum

    Return codes are:
       -1  = successful, EOF block
       -2  = successful, data block
        0  = end of file
        1+ = unsuccessful */

    if (cursor == localsize)
    {   return 0;
    }

    if (IOBuffer[cursor] == ':')
    {   if (!isok(cursor + 1, cursor + 10))
        {   zprintf(TEXTPEN_ERROR, "Error: illegal character found in bytes %d..%d!\n", cursor + 1, cursor + 10);
            return 1;
        }

        blocklength = (UBYTE)
        (   (getdigit(IOBuffer[cursor + 1]) *   16)
          +  getdigit(IOBuffer[cursor + 2])
        );

        loadaddr = (UWORD)
        (   (getdigit(IOBuffer[cursor + 3]) * 4096)
          + (getdigit(IOBuffer[cursor + 4]) *  256)
          + (getdigit(IOBuffer[cursor + 5]) *   16)
          +  getdigit(IOBuffer[cursor + 6])
        );
        if (loadaddr >= 0x8000)
        {   zprintf(TEXTPEN_ERROR, "Error: address is out of range!\n");
            return 1;
        }

        blocktype = (UBYTE)
        (   (getdigit(IOBuffer[cursor + 7]) *   16)
          +  getdigit(IOBuffer[cursor + 8])
        );
        if (blocklength == 0 && blocktype <= 1)
        {   zprintf(TEXTPEN_CLIOUTPUT, "Found end-of-file block.\n", loadaddr);
            return -1;
        } // implied else

        if (blocktype != 0)
        {   zprintf(TEXTPEN_ERROR, "Unrecognized block type $%02X!\n", blocktype);
            return -1;
        } // implied else

        if (!isok(cursor + 11, cursor + 10 + (blocklength * 2)))
        {   zprintf(TEXTPEN_ERROR, "Error: illegal character found in bytes %d..%d!\n", cursor + 10, cursor + 10 + (blocklength * 2));
            return 1;
        }

        zprintf(TEXTPEN_CLIOUTPUT, "Loading $%04X..$%04X.\n", loadaddr, loadaddr + blocklength - 1);
        for (i = 0; i < blocklength; i++)
        {   memory[loadaddr + i] = (UBYTE)
            (   (getdigit(IOBuffer[cursor +  9 + (i * 2)]) * 16)
              +  getdigit(IOBuffer[cursor + 10 + (i * 2)])
            );
        }

        ourbcc = iof_makebcc(cursor + 1, cursor + 8 + (blocklength * 2), IOBuffer);
        theirbcc = (UBYTE)
        (   (getdigit(IOBuffer[cursor +  9 + (blocklength * 2)]) * 16)
          +  getdigit(IOBuffer[cursor + 10 + (blocklength * 2)])
        );
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Our checksum is $%02X. Their checksum is $%02X.\n", ourbcc, theirbcc);
#endif
        if (ourbcc != theirbcc)
        {   zprintf(TEXTPEN_ERROR, "Error: Checksum is incorrect!\n");
            return 1;
        }

        cursor += 11 + (blocklength * 2);
        while
        (   (   IOBuffer[cursor] == LF
             || IOBuffer[cursor] == CR
             || IOBuffer[cursor] == ' '
            )
         && cursor < localsize
        )
        {   cursor++;
        }
        return -2;
    } else
    {   zprintf(TEXTPEN_ERROR, "Error: block header not found!\n");
        return 1;
}   }

MODULE FLAG isok(int start, int end)
{   int i;

    for (i = start; i <= end; i++)
    {   if
        (   (IOBuffer[i] < '0' || IOBuffer[i] > '9')
         && (IOBuffer[i] < 'A' || IOBuffer[i] > 'F')
         && (IOBuffer[i] < 'a' || IOBuffer[i] > 'f') // not really strictly OK
        )
        {   return FALSE;
    }   }

    return TRUE;
}

EXPORT UBYTE getdigit(UBYTE data)
{   if (data >= '0' && data <= '9')
    {   return (UBYTE) (data - '0');
    } elif (data >= 'A' && data <= 'F')
    {   return (UBYTE) (data - 'A' + 10);
    } elif (data >= 'a' && data <= 'f')
    {   return (UBYTE) (data - 'a' + 10);
    } else // should never happen
    {   // assert(0);
        return 0; // to avoid spurious GCC compiler warnings
}   }

EXPORT void serialize_cos(void)
{   int i, j,
        x, y;

    if (serializemode == SERIALIZE_WRITE)
    {   cosversion = machines[machine].cosversion;
    }

    offset = 4;
    serialize_byte_int(&ax[0]); // 4
    serialize_byte_int(&ay[0]); // 5
    serialize_byte_int(&ax[1]); // 6
    serialize_byte_int(&ay[1]); // 7

    if (machine != PONG && machine != TYPERIGHT)
    {   serialize_word(&iar);                //  8.. 9
        serialize_byte(&psu);                //     10
        serialize_byte(&psl);                //     11
        for (i = 0; i < 8; i++)
        {   serialize_word(&ras[i]);         // 12..27
        }
        for (i = 0; i < 7; i++)
        {   serialize_byte(&r[i]);           // 28..34
        }
        serialize_byte_int(&interrupt_2650); //     35
        if (serializemode == SERIALIZE_WRITE && !game)
        {   if (machine == INSTRUCTOR)
            {   startaddr_h = 0x18;
                startaddr_l = 0x00;
            } elif (machine == MIKIT)
            {   startaddr_h =
                startaddr_l = 0x00;
        }   }
        serialize_word_int(&whichgame);      // 36..37
        offset += 2;                         // 38..39
        serialize_byte(&startaddr_h);
        serialize_byte(&startaddr_l);
        serialize_long(&frames);
        serialize_long(&cycles_2650);
        serialize_long(&oldcycles);
        serialize_word_int(&nextinst);
        serialize_byte((UBYTE*) &halted);
        serialize_byte(&ininterrupt);
        serialize_long((ULONG*) &slice_2650);
        serialize_byte_int(&vdu_x);
        serialize_byte_int(&vdu_y);
        serialize_byte(&glow);
        for (i = 0; i < ((machine == PIPBUG) ? 13 : 8); i++)
        {   serialize_byte(&digitleds[i]);
        }

#ifdef VERBOSE
        view_cpu_2650(FALSE);
        if (machine != ARCADIA && machine != INTERTON && machine != ELEKTOR && machine != CD2650)
        {   zprintf
            (   TEXTPEN_VERBOSE,
                "2650 overcalc:     %d cycle(s)\n",
                -slice_2650
            );
        }
        zprintf
        (   TEXTPEN_VERBOSE,
            "2650 interrupting? %s\n\n",
            (interrupt_2650 ? "Yes" : "No")
        );
#endif
    }

    switch (machine)
    {
    case ARCADIA:
        serialize_long((ULONG*) &fractionator);
        serialize_byte_int((int*) &region);
    //lint -fallthrough
    case INTERTON:
        serialize_long((ULONG*) &cheevosize);
        for (i =      0; i <= 0x7FFF; i++) /*  32K   */ serialize_byte(&memory[i]);
        serialize_long((ULONG*) &nextinst);
        serialize_long((ULONG*) &cpl);
        if (serializemode == SERIALIZE_READ)
        {   set_cpl(cpl); // not really needed for INTERTON (only for ARCADIA)
        }
    acase ELEKTOR:
        for (i =  0x800; i <= 0x1FFF; i++) /*   6K   */ serialize_byte(&memory[i]);
        serialize_long((ULONG*) &nextinst);
        serialize_long((ULONG*) &cpl);
        if (serializemode == SERIALIZE_READ)
        {   set_cpl(cpl); // not really needed for ELEKTOR
        }
        serialize_byte_int(&elektor_biosver);
        serialize_long((ULONG*) &cheevosize);
        for (i = 0; i < cheevosize; i++)
        {   serialize_byte(&cheevomem[i]);
        }
    acase PIPBUG:
        pipbug_serialize_cos();
    acase BINBUG:
        binbug_serialize_cos();
    acase INSTRUCTOR:
        for (i =    0x0; i <= 0x17FF; i++) /*   6K   */ serialize_byte(&memory[i]);
        for (i = 0x2000; i <= 0x7FFF; i++) /*  24K   */ serialize_byte(&memory[i]);
        serialize_byte(&s_toggles);
        serialize_byte_int(&s_io);
        serialize_byte_int(&s_id);
        serialize_byte_int(&s_is);
        serialize_byte(&s_tapeport);
        serialize_byte(&ioport[0xFD].contents);
        serialize_byte(&ioport[0xFC].contents);
    acase TWIN:
        if (serializemode == SERIALIZE_READ)
        {   for (i = 0; i < 2; i++)
            {   for (j = 0; j < TWIN_DISKSIZE / 8; j++)
                {   drive[i].flags[j] = 0;
        }   }   }
        twin_serialize_cos();
    acase CD2650:
        cd2650_serialize_cos();
    acase MALZAK:
        serialize_coinops();
        for (i = 0x1000; i <= 0x1FFF; i++)
        {   serialize_byte(&memory[i]);
        }
        /* DIPs are at memory[$14CC] and thus get done above.
           malzak2_updatedips() gets done for us by engine_load() */
        for (x = 0; x < 16; x++)
        {   for (y = 0; y < 16; y++)
            {   serialize_byte(&malzak_field[x][y]); // 16*16=256
        }   }
        serialize_byte_int(&malzak_bank1);
        serialize_byte_int(&malzak_bank2);
        serialize_byte_int(&malzak_x);
        serialize_byte_int(&malzak_y);
    acase ZACCARIA:
        serialize_coinops();
        switch (memmap)
        {
        case MEMMAP_ASTROWARS:
        case MEMMAP_GALAXIA:
            for (i = 0x1400; i <= 0x1FFF; i++) /* 3K  */ serialize_byte(&memory[i]);
            for (i =      0; i <=  0x3FF; i++) /* 1K  */ serialize_byte(&g_bank1[i]);
            for (i =      0; i <=     15; i++) /* 16b */ serialize_byte(&g_bank2[i]);
            serialize_byte(         &ioport[PORTD].contents);
            serialize_byte(         &awga_collide);
            serialize_byte((UBYTE*) &galaxia_scrolly); // not used on Astro Wars
            if (memmap == MEMMAP_ASTROWARS)
            {   serialize_byte(&aw_dips1);
                serialize_byte(&aw_dips2);
                // zaccaria_updatedips() gets done for us by engine_load()
            } else
            {   // assert(memmap == MEMMAP_GALAXIA);
                serialize_byte(&ga_dips); // zaccaria_updatedips() gets done for us by engine_load()
            }
        acase MEMMAP_LASERBATTLE:
        case MEMMAP_LAZARIAN:
            if (memmap == MEMMAP_LAZARIAN)
            {   serialize_byte(&lz_dips2);
                serialize_byte(&lz_dips3); // zaccaria_updatedips() gets done for us by engine_load()
            } else
            {   // assert(memmap == MEMMAP_LASERBATTLE);
                serialize_byte(&lb_dips ); // zaccaria_updatedips() gets done for us by engine_load()
                offset++;
            }
            serialize_byte(&lb_bank);
            serialize_byte(&mux);
            serialize_byte(&lb_spritecode);
            serialize_byte(&lb_spritecolour);
            serialize_byte(&lb_spriteenable);
            serialize_byte(&lb_spritex);
            serialize_byte(&lb_spritey);
            serialize_byte(&lb_tone);
            serialize_long(&lb_snd);
            for (i = 0x1500; i <= 0x1FFF; i++) /* 3K */ serialize_byte(&memory[i]);
            serialize_byte(&circle);
            for (i = 0; i < 512; i++)
            {   serialize_byte(&memory_effects[i]);
            }
            serialize_byte(&abeff1);
            serialize_byte(&abeff2);
            serialize_byte(&useshell);
            serialize_byte(&neg1);
            serialize_byte(&neg2);
        }
    acase PHUNSY:
        serialize_byte_int(&phunsy_biosver);
        switch (phunsy_biosver)
        {
        case PHUNSY_MINIMONITOR:
            for (i =      0; i <= 0x1DFF; i++) serialize_byte(&memory[i]);
        acase PHUNSY_PHUNSY:
            // 4K + (13 * 2K) + 8K + (15 * 16K) = 278K
            for (i =  0x800; i <= 0x17FF; i++) serialize_byte(&memory[i]);
            for (i = 0; i <= 15; i++)
            {   if (i >= 1 && i <= 3)
                {   continue;
                }
                for (j = 0; j <=  0x7FF; j++)  serialize_byte(&u_bank[i][j]);
            }
            for (i = 1; i <= 15; i++)
            {   for (j = 0; j <= 0x3FFF; j++)  serialize_byte(&q_bank[i][j]);
            }
            for (i = 0x2000; i <= 0x3FFF; i++) serialize_byte(&memory[i]);
            for (i = 1; i <= 3; i++)
            {   for (j = 0; j <= 0x7FF; j++)   serialize_byte(&u_bank[i][j]);
            }
            for (i = 0; i <= 0x3FFF; i++)      serialize_byte(&q_bank[0][i]);
        }
        serialize_byte(&ticks);
        serialize_byte(&rate);
        serialize_byte(&banked);
        serialize_long((ULONG*) &nextinst);
        serialize_byte(&ioport[0x10].contents);
        serialize_byte(&ioport[0x11].contents);
        serialize_byte(&ioport[0x12].contents);
        serialize_byte(&ioport[0x13].contents);
    acase PONG:
        pong_serialize_cos();
    acase SELBST:
        for (i =  0x800; i <= 0x1FFF; i++) /* 6K   */ serialize_byte(&memory[i]);
        for (y = 0; y < 16; y++)
        {   for (x = 0; x < 64; x++)
            {   serialize_byte(&vdu[x][y]);
        }   }
        serialize_byte_int(&selbst_biosver);
        serialize_byte(&selbst_char);
    acase MIKIT:
        for (i =  0x400; i <= 0x7FF; i++) /* 1K    */ serialize_byte(&memory[i]);
    acase TYPERIGHT:
        tr_serialize_cos();
    }

    // assert(offset <= MAX_STATESIZE);

    startoffset = offset;
#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Saved state is %d bytes.\n", offset);
#endif
}

EXPORT void serialize_byte(UBYTE* var)
{   if (serializemode == SERIALIZE_WRITE)
    {   WriteByte(*var);
    } else
    {   // assert(serializemode == SERIALIZE_READ);
        *var = ReadByte();
}   }
EXPORT void serialize_word(UWORD* var)
{   if (serializemode == SERIALIZE_WRITE)
    {   WriteWord(*var);
    } else
    {   // assert(serializemode == SERIALIZE_READ);
        *var = ReadWord();
}   }
EXPORT void serialize_long(ULONG* var)
{   if (serializemode == SERIALIZE_WRITE)
    {   WriteLong(*var);
    } else
    {   // assert(serializemode == SERIALIZE_READ);
        *var = ReadLong();
}   }
EXPORT void serialize_byte_int(int* var)
{   if (serializemode == SERIALIZE_WRITE)
    {   WriteByte((UBYTE) (*var));
    } else
    {   // assert(serializemode == SERIALIZE_READ);
        *var = (int) ReadByte();
}   }
EXPORT void serialize_byte_ulong(ULONG* var)
{   if (serializemode == SERIALIZE_WRITE)
    {   WriteByte((UBYTE) (*var));
    } else
    {   // assert(serializemode == SERIALIZE_READ);
        *var = (ULONG) ReadByte();
}   }
EXPORT void serialize_word_int(int* var)
{   if (serializemode == SERIALIZE_WRITE)
    {   WriteWord((UWORD) (*var));
    } else
    {   // assert(serializemode == SERIALIZE_READ);
        *var = (int) ReadWord();
        if (*var == 0xFFFF)
        {   *var = -1;
}   }   }

EXPORT void makeaudittext(STRPTR outputstring)
{   sprintf
    (   outputstring,
        LLL(MSG_AUDITTEXT,
        "In directory \"%s\":\n\n" \
        "%d known AOF/BIN/TVC file(s).\n" \
        "%d unknown AOF/BIN/TVC file(s).\n" \
        "%d state(s)/recording(s).\n" \
        "%d ASM/BPNF/CMD/EOF/HEX/IMAG/MOD/SMS/SYM file(s).\n" \
        "%d IMG/RAW/TWIN file(s).\n" \
        "%d 8SVX/16SV/AIFF/MDCR/WAV file(s).\n" \
        "%d screenshot/animation/MIDI/PSG/SMUS/TXT/YM file(s).\n" \
        "%d icon(s).\n" \
        "%d unknown file(s).\n" \
        "%d directory/ies.\n"),
        path_games,
        found_matched,
        found_unmatched,
        foundsnaps,
        found_loadables,
        found_disks,
        found_tapes,
        found_unloadables,
        foundicons,
        found_unknowns,
        found_dirs
    );
}

EXPORT void sortaudit(void)
{   if (sortby == SORTBYNAME)
    {   sortbymachine();
        sortbyname();
    } else
    {   // assert(sortby == SORTBYMACHINE);
        sortbyname();
        sortbymachine();
}   }

MODULE void sortbyname(void)
{   int i, j;

 // quicksort_byname(0, foundgames - 1); // rough quicksort

    for (i = 0; i < foundgames - 1; i++) // fine bubble sort
    {   for (j = 2; j < foundgames - i - 1; j++)
        {   if
            (   stricmp
                (   (const char*) auditlist[sidebar[j    ]].gamename,
                    (const char*) auditlist[sidebar[j + 1]].gamename
                ) > 0
            )
            {   swapgames(j, j + 1);
}   }   }   }

MODULE void sortbymachine(void)
{   int i, j;

 // quicksort_bymachine(0, foundgames - 1); // rough quicksort

    for (i = 0; i < foundgames - 1; i++) // fine bubble sort
    {   for (j = 2; j < foundgames - i - 1; j++)
        {   if
            (   auditlist[sidebar[j    ]].machine
              > auditlist[sidebar[j + 1]].machine
            )
            {   swapgames(j, j + 1);
}   }   }   }

MODULE __inline void swapgames(int game1, int game2)
{   FAST int temp;

    temp           = sidebar[game1];
    sidebar[game1] = sidebar[game2];
    sidebar[game2] = temp;
}

/* MODULE void quicksort_bymachine(int m, int n)
{   int i, j, k,
        key;

    if (m < n)
    {   k = (m + n) / 2; // k is pivot
        swapgames(m, k);
        key = auditlist[sidebar[m]].machine;
        i = m + 1;
        j = n;
        while (i <= j)
        {   while ((i <= n) && (auditlist[sidebar[i]].machine <= key))
            {   i++;
            }
            while ((j >= m) && (auditlist[sidebar[j]].machine >  key))
            {   j--;
            }
            if (i < j)
            {   swapgames(i, j);
        }   }
        // swap two elements
        swapgames(m, j);
        // recursively sort the lesser list
        quicksort_bymachine(m,     j - 1);
        quicksort_bymachine(j + 1, n    );
}   }

MODULE void quicksort_byname(int m, int n)
{   int    i, j, k;
    STRPTR key;

    if (m < n)
    {   k = (m + n) / 2; // k is pivot
        swapgames(m, k);
        key = auditlist[sidebar[m]].gamename;
        i = m + 1;
        j = n;
        while (i <= j)
        {   while ((i <= n) && (stricmp(auditlist[sidebar[i]].gamename, key) <= 0))
            {   i++;
            }
            while ((j >= m) && (stricmp(auditlist[sidebar[j]].gamename, key) >  0))
            {   j--;
            }
            if (i < j)
            {   swapgames(i, j);
        }   }
        // swap two elements
        swapgames(m, j);
        // recursively sort the lesser list
        quicksort_byname(m,     j - 1);
        quicksort_byname(j + 1, n    );
}   } */

#ifdef CHECKBLOCKS
EXPORT void checkblocks(void)
{   TRANSIENT int i, j,
                  ourbcc; // ourbcc needs to be int not UBYTE!

#define BCCBLOCKS 7 // 12 for Whitlock tape 26-6

    PERSIST const struct
    {   UWORD addr;
        UBYTE length,
              theirbcc;
    } blocks[BCCBLOCKS] = {
/*    { 0x900, 16, 0x58 }, // these are for Whitlock tape 26-6.
      { 0x910, 16, 0x73 },
      { 0x920, 16, 0x35 },
      { 0x930, 16, 0xBA },
      { 0x940, 16, 0x91 },
      { 0x950, 16, 0xF8 },
      { 0x960, 16, 0x0D },
      { 0x970, 16, 0x91 },
      { 0x980, 16, 0xD7 },
      { 0x990, 16, 0xE3 },
      { 0x9A0, 16, 0x17 },
      { 0x9B0, 13, 0x13 },
*/
      { 0x1F80, 16, 0x1C }, // these are for Brinkmann RoutinenA-4.
      { 0x1F90, 16, 0x1D },
      { 0x1FA0, 14, 0x88 },
      {  0x8C0, 16, 0x72 },
      {  0x8D0, 16, 0x71 },
      {  0x8E0, 16, 0xC1 },
      {  0x8F0,  7, 0xCA },
    };

    for (i = 0; i < BCCBLOCKS; i++)
    {   ourbcc = 0;
        for (j = 0; j < blocks[i].length; j++)
        {   ourbcc ^= memory[blocks[i].addr + j];
            if (ourbcc >= 128)
            {   ourbcc = ((ourbcc & 127) * 2) + 1;
            } else ourbcc <<= 1;
        }
        if (ourbcc == blocks[i].theirbcc)
        {   zprintf
            (   TEXTPEN_VERBOSE,
                "Block $%04X..$%04X is OK (BCC is $%02X).\n",
                (int) blocks[i].addr,
                (int) blocks[i].addr + blocks[i].length - 1,
                ourbcc
            );
        } else
        {   zprintf
            (   TEXTPEN_ERROR,
                "Block $%04X..$%04X is bad (stored BCC is $%02X, calculated BCC is $%02X).\n",
                (int) blocks[i].addr,
                (int) (blocks[i].addr + blocks[i].length - 1),
                (int) blocks[i].theirbcc,
                ourbcc
            );
    }   }

    zprintf(TEXTPEN_VERBOSE, "\n");
}
#endif

EXPORT void fliplog(int* thevar)
{   zprintf(TEXTPEN_CLIOUTPUT, " ");
    if (*thevar)
    {   *thevar = FALSE;
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            LLL(
                MSG_ENGINE_OFF,
                "off"
        )   );
    } else
    {   *thevar = TRUE;
        zprintf
        (   TEXTPEN_CLIOUTPUT,
            LLL(
                MSG_ENGINE_ON,
                "on"
        )   );
    }
    zprintf(TEXTPEN_CLIOUTPUT, ".\n\n");
}

EXPORT void make_opcodetip(int i, STRPTR stringptr)
{   FAST UBYTE  category;
    FAST TEXT   formatstring[80 + 1],
                indirectstr[6 + 1];
    FAST STRPTR addrmode,
                categoryptr;
    FAST int    duration;

    category = table_opcolours_2650[supercpu][i];
    switch (category)
    {
#ifdef THOLIN
    case  RED:    categoryptr =                            "Unused";
    acase BLUE:   categoryptr = (STRPTR) LLL(MSG_LEGEND10, "Branch");
    acase GREEN:  categoryptr =                            "Arithmetic";
    acase YELLOW: categoryptr =                            "Subroutine";
    acase PURPLE: categoryptr =                            "Logical";
    acase ORANGE: categoryptr =                            "Load/Store";
    acase GREY1:  categoryptr =                            "Status Reg.";
    acase CYAN:   categoryptr =                            "I/O";
    acase WHITE:  categoryptr =                            "Misc.";
    acase DKBLUE: categoryptr =                            "Compare";
#else
    case  RED:    categoryptr = (STRPTR) LLL(MSG_LEGEND8,  "Arithmetic, transfer");
    acase BLUE:   categoryptr = (STRPTR) LLL(MSG_LEGEND9,  "I/O, PSW, mixed");
    acase GREEN:  categoryptr = (STRPTR) LLL(MSG_LEGEND10, "Branch");
    acase YELLOW: categoryptr = (STRPTR) LLL(MSG_LEGEND11, "Special");
    acase PURPLE: categoryptr = (STRPTR) LLL(MSG_LEGEND12, "Illegal");
#endif
    }

    if (style != STYLE_CALM)
    {   if (i == 0x9F || i == 0xBF) // BXA/BSXA
        {   strcpy((char*) formatstring, opcodes[style][i].format);
        } elif (opcodes[style][i].format[0])
        {   sprintf((char*) formatstring, "%s %s", opcodes[style][i].name, opcodes[style][i].format);
        } else
        {   strcpy((char*) formatstring, opcodes[style][i].name);
    }   }
    else
    {   // assert(style == STYLE_CALM);
        if ((i == 0x11 && supercpu) || (i >= 0xC8 && i <= 0xCF) || (i >= 0xD4 && i <= 0xD7))
        {   strcpy((char*) formatstring, opcodes[style][i].format);
        } elif (opcodes[style][i].format[0])
        {   sprintf((char*) formatstring, "%s%s", opcodes[style][i].name, opcodes[style][i].format);
        } else
        {   strcpy((char*) formatstring, opcodes[style][i].name);
    }   }

    switch (table_addrmode_2650[supercpu][i])
    {
    case  'A': addrmode = (STRPTR) LLL(MSG_ABSOLUTE,  "Absolute");
    acase 'G': addrmode = (STRPTR) LLL(MSG_REGISTER,  "Register");
    acase 'I': addrmode = (STRPTR) LLL(MSG_IMMEDIATE, "Immediate");
    acase 'R': addrmode = (STRPTR) LLL(MSG_RELATIVE,  "Relative");
    acase 'T': addrmode = (STRPTR) LLL(MSG_IMPLICIT,  "Implicit");
    acase '-': addrmode = (STRPTR) LLL(MSG_LEGEND12,  "Illegal"); // it relies on MSG_LEGEND12 being "Illegal"
    }

    if ((i & 0x0F) >= 8)
    {   switch (timeunit)
        {
        case  TIMEUNIT_CYCLES: strcpy( (char*) indirectstr, "(+2)");
        acase TIMEUNIT_CLOCKS: strcpy( (char*) indirectstr, "(+6)");
        acase TIMEUNIT_PIXELS: sprintf((char*) indirectstr, "(+%d)", 6 * ppc);
    }   }
    else
    {   indirectstr[0] = EOS;
    }
    duration = (int) scale_time(table_duration_2650[i]);

    sprintf
    (   stringptr,
        LLL
        (   MSG_OPCODETOOLTIP,
            "%s\n"                  \
            "Opcode: $%02X\n"       \
            "Name: %s\n"            \
            "Pseudocode: %s\n"      \
            "Condition code: %s\n"  \
            "Size: %d byte(s)\n"    \
            "Duration: %d%s %s\n" \
            "Flags read: %s\n"      \
            "Flags written: %s\n"   \
            "Category: %s\n"        \
            "Addressing mode: %s"
        ), // PSW bits aren't all flags, so we should not call them flags
        formatstring,
        i,
        opcodes[style][i].fullname,
        opcodes[style][i].pseudocode,
        opcodes[style][i].cc,
        table_size_2650[i],
        duration,
        indirectstr,
        timeunitstr2,
        opcodes[style][i].rflags,
        opcodes[style][i].wflags,
        categoryptr,
        addrmode
    );
}

EXPORT void make_memorytip(int address, STRPTR stringptr)
{   FAST int i,
             oldverbosity;

    DISCARD getfriendly(address);
    sprintf
    (   stringptr,
        "%s %s\n",
        LLL(MSG_ADDRESS, "Address:"),
        friendly
    );
    if (mirror_r[address] != address)
    {   DISCARD getfriendly(mirror_r[address]);
        sprintf(ENDOF(stringptr), LLL(MSG_READMIRROR,  "Read mirror of %s." ), friendly);
        strcat(stringptr, "\n");
    }
    if (mirror_w[address] != address)
    {   DISCARD getfriendly(mirror_w[address]);
        sprintf(ENDOF(stringptr), LLL(MSG_WRITEMIRROR, "Write mirror of %s."), friendly);
        strcat(stringptr, "\n");
    }

    sprintf(ENDOF(stringptr), "%s: $%02X\n", LLL(MSG_CONTENTS, "Contents"), memory[address]);
    oldverbosity = verbosity;
    verbosity = 2;
    disassemble_2650(address, address, TRUE);
    verbosity = oldverbosity;
    strcat(stringptr, &mn[8]);

    // machine-specific code comments
    if (machines[machine].firstcodecomment != -1)
    {   for (i = machines[machine].firstcodecomment; i <= machines[machine].lastcodecomment; i++)
        {   if (address == codecomment[i].address)
            {   strcat(stringptr, "\n");
                strcat(stringptr, codecomment[i].text);
    }   }   }
    if (machine == BINBUG)
    {   if (firstdoscodecomment != -1)
        {   for (i = firstdoscodecomment; i <= lastdoscodecomment; i++)
            {   if (address == codecomment[i].address)
                {   strcat(stringptr, "\n");
                    strcat(stringptr, codecomment[i].text);
        }   }   }
        for (i = FIRSTACOSCODECOMMENT; i <= LASTACOSCODECOMMENT; i++)
        {   if (address == codecomment[i].address)
            {   strcat(stringptr, "\n");
                strcat(stringptr, codecomment[i].text);
    }   }   }

    // game-specific code comments
    if (whichgame != -1 && known[whichgame].firstcodecomment != -1)
    {   // assert(known[whichgame].lastcodecomment != -1);
        for (i = known[whichgame].firstcodecomment; i <= known[whichgame].lastcodecomment; i++)
        {   if (address == codecomment[i].address)
            {   strcat(stringptr, "\n");
                strcat(stringptr, codecomment[i].text);
                break; // for speed
    }   }   }

    show_data_comment(address, 2, stringptr);
}

EXPORT void make_monitortip(int address, STRPTR stringptr)
{   DISCARD getfriendly(address);
    strcpy(stringptr, (const char*) friendly);
    show_data_comment(address, 2, stringptr);
}

EXPORT void make_overlaytip(int i, STRPTR stringptr)
{   int extrakey;

    if (i == 36 || i == 37)
    {   i = 0;
    } elif (i == 38 || i == 39)
    {   i = 1;
    }

    if     (keyhelp[i].seq == key1)
    {   extrakey = keypads[keyhelp[i].player][ 0];
    } elif (keyhelp[i].seq == key2)
    {   extrakey = keypads[keyhelp[i].player][21];
    } elif (keyhelp[i].seq == key3)
    {   extrakey = keypads[keyhelp[i].player][22];
    } elif (keyhelp[i].seq == key4)
    {   extrakey = keypads[keyhelp[i].player][23];
    } else
    {   extrakey = -1;
    }

    if (extrakey == -1)
    {   sprintf
        (   stringptr,
            LLL(
                MSG_OVERLAYTIP1,
                "Overlay: %s\n"     \
                "Guest key: %s\n"   \
                "Host key: %s"
            ),
            overlays[whichoverlay][i],
            overlays[memmapinfo[memmap].overlay][i],
            keyname[keypads[keyhelp[i].player][keyhelp[i].pos]].name
        );
    } else
    {   sprintf
        (   stringptr,
            LLL(
                MSG_OVERLAYTIP2,
                "Overlay: %s\n"     \
                "Guest key: %s\n"   \
                "Host keys: %s, %s"
            ),
            overlays[whichoverlay][i],
            overlays[memmapinfo[memmap].overlay][i],
            keyname[keypads[keyhelp[i].player][keyhelp[i].pos]].name,
            keyname[extrakey].name
        );
}   }

EXPORT void make_overlayspecialtip(int left, int right, int id, STRPTR stringptr)
{   sprintf
    (   stringptr,
        LLL
        (   MSG_OVERLAYTIP3,
            "Guest key: %s\n\n"     \
            "Left player:\n"        \
            "Overlay: %s\n"         \
            "Host keys: %s, %s\n\n" \
            "Right player:\n"       \
            "Overlay: %s\n"         \
            "Host keys: %s, %s"
        ),
        overlays[memmapinfo[memmap].overlay][left],
        overlays[              whichoverlay][left],
        keyname[keypads[keyhelp[left].player][keyhelp[left].pos]].name,
        id == 32 ? keyname[keypads[0][0]].name : keyname[keypads[0][id - 12]].name, // 32 -> 0, 33..35 -> 21..23
        overlays[whichoverlay][right],
        keyname[keypads[keyhelp[right].player][keyhelp[right].pos]].name,
        id == 32 ? keyname[keypads[1][0]].name : keyname[keypads[1][id - 12]].name  // 32 -> 0, 33..35 -> 21..23
    );
}

EXPORT void enablegads(void)
{   int i;

    for (i = 0; i < ALLREDEFINEGADS; i++)
    {   if (memmapinfo[memmap].gadenabled[i])
        {   enablegad(i, TRUE);
        } else
        {   enablegad(i, FALSE);
}   }   }

EXPORT void patchrom(void)
{   int i;

    switch (memmap)
    {
    case MEMMAP_ASTROWARS:
        // ADDI,r0 $01 (patient) vs. LODI,r0 $55 (impatient)
        memory[ 0x198] = post                  ? 0x84 : 0x04;
        memory[ 0x199] = post                  ? 0x01 : 0x55;

        memory[ 0xEB2] = trainer_lives         ? 0x00 : 0x01;     // SUBI,r0 1             -> SUBI,r0 0

        memory[0x25FF] = trainer_time          ? 0x00 : 0x02; // fuel

        memory[ 0x93B] = trainer_invincibility ? 0x00 : 0x22; // ships
        memory[0x11A4] = trainer_invincibility ? 0x00 : 0x22; // bombs
        memory[0x2548] = trainer_invincibility ? 0x00 : 0x22; // bullets
    acase MEMMAP_GALAXIA:
        // ADDI,r0 $01 (patient) vs. LODI,r0 $55 (impatient)
        memory[ 0x179] = post                  ? 0x84 : 0x04;
        memory[ 0x17A] = post                  ? 0x01 : 0x55;

        memory[ 0x6F1] = trainer_lives         ? 0x00 : 0x01;     // SUBI,r0 1             -> SUBI,r0 0

        memory[0x284F] = trainer_invincibility ? 0x00 : 0x26; // bullets
        memory[0x2934] = trainer_invincibility ? 0x1B : 0x98; // ships

        memory[0x27BB] = trainer_time          ? 0xC0 : 0x16;     // RETC,lt               -> NOP       (energy)
    acase MEMMAP_LASERBATTLE:
        // ADDI,r0 $01 (patient) vs. LODI,r0 $55 (impatient)
        memory[ 0x269] = post                  ? 0x84 : 0x04;
        memory[ 0x26A] = post                  ? 0x01 : 0x55;

        memory[0x2715] = trainer_time          ? 0x00 : 0x02;     // SUBI,r0 2             -> SUBI,r0 0 (fuel)

        memory[0x520D] = trainer_lives         ? 0x00 : 0x01;     // SUBI,r0 1             -> SUBI,r0 0

        memory[0x50DC] = trainer_invincibility ? 0x20 : 0x0C;     // LODA,r0 $5C4B         -> EORZ    r0
        memory[0x50DD] = trainer_invincibility ? 0x17 : 0x1C;     //                          RETC,un
    acase MEMMAP_LAZARIAN:
        // ADDI,r0 $01 (patient) vs. LODI,r0 $55 (impatient)
        memory[ 0x281] = post                  ? 0x84 : 0x04;
        memory[ 0x282] = post                  ? 0x01 : 0x55;

        memory[0x30A2] = trainer_invincibility ? 0x20 : 0x0D;     // LODA,r1 $3C01         -> EORZ    r0
        memory[0x30A3] = trainer_invincibility ? 0x17 : 0x1C;     //                          RETC,un

        memory[0x31A8] = trainer_lives         ? 0x00 : 0x01;     // SUBI,r0 1             -> SUBI,r0 0

        memory[0x46E2] = trainer_time          ? 0x00 : 0x02;     // SUBI,r0 2             -> SUBI,r0 0 (fuel)
    acase MEMMAP_MALZAK1:
        // STRA,r0 $1800 (patient) vs. BCTA,un $00A0 (impatient)
        memory[  0x52] = post                  ? 0xCC : 0x1F;
        memory[  0x53] = post                  ? 0x18 : 0x00;
        memory[  0x54] = post                  ? 0x00 : 0xA0;

        memory[0x24AD] = trainer_lives         ? 0x00 : 0x01;     // SUBI,r0 1             -> SUBI,r0 0

        memory[0x27A5] = trainer_time          ? 0x00 : 0x02;     // SUBI,r0 2             -> SUBI,r0 0 (fuel)

        memory[0x41FD] = trainer_invincibility ? 0x42 : 0x24;
        memory[0x41FE] = trainer_invincibility ? 0x09 : 0x00;
    acase MEMMAP_MALZAK2:
        // LODI,r0 $00 (patient) vs. LODI,r0 $01 (impatient)
        memory[  0x84] = post                  ? 0x00 : 0x01;

        memory[0x24AD] = trainer_lives         ? 0x00 : 0x01;     // SUBI,r0 1             -> SUBI,r0 0

        memory[0x279E] = trainer_time          ? 0xA4 : 0xAC;     // SUBA,r0 $30CF         -> SUBI,r0 0 (fuel)
        memory[0x279F] = trainer_time          ? 0x00 : 0x10;
        memory[0x27A0] = trainer_time          ? 0xA4 : 0xCF;     //                          SUBI,r0 0
        memory[0x27A1] = trainer_time          ? 0x00 : 0xAC;     // SUBA,r0 $30CF
        memory[0x27A2] = trainer_time          ? 0xA4 : 0x10;     //                          SUBI,r0 0
        memory[0x27A3] = trainer_time          ? 0x00 : 0xCF;

        memory[0x41FF] = trainer_invincibility ? 0x42 : 0x24;
        memory[0x4200] = trainer_invincibility ? 0x09 : 0x00;
    adefault:
        switch (whichgame)
        {
        case _2DSOCCERPOS: // time
        case _2DSOCCERODPOS:
            memory[ 0x62D] = trainer_time          ? 0xC0 : 0x84; // ADDI,r0 $FF           -> NOP
            memory[ 0x62E] = trainer_time          ? 0xC0 : 0xFF; //                          NOP
            memory[ 0x62F] = trainer_time          ? 0xC0 : 0x94; // DAR,r0                -> NOP
        acase _3DATTACKPOS:
            memory[ 0x97C] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $9AE          -> NOP
            memory[ 0x97D] = trainer_invincibility ? 0xC0 : 0x30; //                          NOP
            memory[ 0x985] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $9AE          -> NOP
            memory[ 0x986] = trainer_invincibility ? 0xC0 : 0x27; //                          NOP
            memory[ 0x98C] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $9AE          -> NOP
            memory[ 0x98D] = trainer_invincibility ? 0xC0 : 0x20; //                          NOP
            memory[ 0x995] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $9AE          -> NOP
            memory[ 0x996] = trainer_invincibility ? 0xC0 : 0x17; //                          NOP
            memory[ 0x99C] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $9AE          -> NOP
            memory[ 0x99D] = trainer_invincibility ? 0xC0 : 0x10; //                          NOP
            memory[ 0x9AA] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $9AE          -> NOP
            memory[ 0x9AB] = trainer_invincibility ? 0xC0 : 0x02; //                          NOP
            memory[ 0x9C0] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $9AE          -> NOP
            memory[ 0x9C1] = trainer_invincibility ? 0xC0 : 0x6C; //                          NOP
         // memory[ 0xA1F] = trainer_invincibility ? 0xC0 : 0x1D; // BCTA,gt $9AE          -> NOP
         // memory[ 0xA20] = trainer_invincibility ? 0xC0 : 0x09; //                          NOP
         // memory[ 0xA21] = trainer_invincibility ? 0xC0 : 0xAE; //                          NOP

            memory[ 0x15D] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0

            memory[ 0xCC2] = trainer_time          ? 0x00 : 0xFF; // ADDI,r0 $FF           -> ADDI,r0 0
        acase _3DSOCCERAPOS: // time
        case _3DSOCCERBPOS:
        case _3DSOCCERENHPOS:
            memory[ 0x181] = trainer_time          ? 0x00 : 0xFF; // ADDI,r0 $FF           -> ADDI,r0 0
        acase AIRSEAATTACKPOS:
            memory[ 0x60D] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0 (player 1)
            memory[ 0x65A] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0 (player 2)
        acase AIRSEABATTLEPOS:
            memory[ 0x60F] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0 (player 1)
            memory[ 0x65C] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0 (player 2)
        acase ALIENINVPOS:
        case ALIENINV1POS:
        case ALIENINV2POS:
        case ALIENINV3POS:
        case ALIENINV4POS:
            memory[ 0x82A] = trainer_lives         ? 0x00 : 0xFF; // ADDI,r1 $FF           -> ADDI,r1 0

            memory[ 0x7DC] = trainer_invincibility ? 0xC0 : 0x98; // BCFR,eq $7E8          -> NOP
            memory[ 0x7DD] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x7E5] = trainer_invincibility ? 0x1F : 0x1C; // BCTA,eq $1D6          -> BCTA,un $1D6

            memory[  0xE3] = trainer_time          ? 0xC0 : 0xCC; // STRA,r0 PITCH         -> NOP
            memory[  0xE4] = trainer_time          ? 0xC0 : 0x18; //                          NOP
            memory[  0xE5] = trainer_time          ? 0xC0 : 0xFD; //                          NOP
            memory[ 0xBFB] = trainer_time          ? 0x17 : 0x0C; // LODA,r0 $18F8         -> RETC,un
        acase ASTEROIDSPOS:
            memory[ 0x976] = trainer_invincibility ? 0x3F : 0x0F; // LODA,r3 SPRITECOLLIDE -> BSTA,un $1446
            memory[ 0x977] = trainer_invincibility ? 0x14 : 0x1F;
            memory[ 0x978] = trainer_invincibility ? 0x46 : 0xCB;
            memory[0x1446] = trainer_invincibility ? 0x0F : 0x00; //                          LODA,r3 SPRITECOLLIDE
            memory[0x1447] = trainer_invincibility ? 0x1F : 0x00;
            memory[0x1448] = trainer_invincibility ? 0xCB : 0x00;
            memory[0x1449] = trainer_invincibility ? 0x47 : 0x00; //                          ANDI,r3 $07
            memory[0x144A] = trainer_invincibility ? 0x07 : 0x00;
            memory[0x144B] = trainer_invincibility ? 0x17 : 0x00; //                          RETC,un
        acase PIPBUG_ASTROTREKPOS:
        case BINBUG_ASTROTREKPOS1:
        case BINBUG_ASTROTREKPOS2:
            memory[ 0x876] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase ASTROINVPOS:
        case ASTROINVODPOS:
            memory[ 0x174] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
        acase ATTACKFROMSPACEPOS:
            memory[ 0xBB2] = trainer_invincibility ? 0x1B : 0x98; // BCFR,eq $BA9          -> BCTR,un $BA9
        acase A_BOXINGPOS:
            memory[ 0x6B7] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase A_GOLFPOS1:
        case A_GOLFPOS2:
        case A_GOLFODPOS:
            memory[ 0xDC1] = trainer_time          ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
        acase A_BASKETBALLPOS:
            memory[ 0x112] = trainer_time          ? 0x00 : 0x01; // SUBI,r1 1             -> SUBI,r1 0
        acase BREAKAWAYPOS:
            memory[ 0x356] = trainer_lives         ? 0x00 : 0xFF; // ADDI,r0 $FF           -> ADDI,r0 0
        acase BREAKOUTPOS:
            memory[ 0xF2C] = trainer_lives         ? 0xC0 : 0xF9; // BDRR,r1 $F2E          -> NOP
            memory[ 0xF2D] = trainer_lives         ? 0xC0 : 0x00; //                          NOP
        acase BURSTINGBALLOONSPOS:
            memory[ 0xC2D] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase E_CARRACESPOS:
            memory[ 0x9BC] = trainer_time          ? 0x66 : 0x67; // ADDI,r2 $67           -> ADDI,r2 $66

            memory[ 0x947] = trainer_invincibility ? 0x00 : 0x15; // BCTR,lt $95D          -> BCTR,lt $948
            memory[ 0x94F] = trainer_invincibility ? 0x00 : 0x0D; // BCTR,gt $95D          -> BCTR,gt $950
            memory[ 0xCD4] = trainer_invincibility ? 0x1F : 0x9C; // BCFA,eq $91B          -> BCTA,un $91B
        acase CATTRAXPOS:
            memory[ 0x21C] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
        acase CHICKEN1POS:
        case  CHICKEN2POS:
        case  CHICKEN3POS:
            memory[ 0x90A] = trainer_invincibility ? 0x00 : 0x15; // ANDI,r3 $15           -> ANDI,r3 0
        acase A_CIRCUSPOS:
            memory[ 0xDEA] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase COSMICADVENTUREPOS1:
        case COSMICADVENTUREPOS2:
            memory[ 0xDC1] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0 (for 1st level)
            memory[0x1218] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0 (for 2nd level)

            memory[0x12AC] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0 (for 2nd level)
            memory[0x19D8] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0 (for 3rd level)
        acase CIRCLEDRIVEPOS1:
        case CIRCLEDRIVEPOS2:
            memory[ 0x960] = trainer_invincibility ? 0x46 : 0x6A; // IORR,r2 *$963         -> ANDI,r2 $F8
            memory[ 0x961] = trainer_invincibility ? 0xF8 : 0x81;
        acase COMECOMEPOS:
            memory[ 0xB62] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase COMEFRUTASPOS1:
        case COMEFRUTASPOS2:
        case COMEFRUTASPOS3:
            memory[ 0xB6E] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase CRAZYGOBBLERPOS:
            memory[ 0x2DD] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase CRAZYCLIMBERPOS:
            memory[ 0xDA3] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase DORAEMONPOS:
            memory[ 0x210] = trainer_lives         ? 0xC0 : 0x20; // EORZ    r0            -> NOP
        acase DRSLUMPPOS:
            memory[0x2D97] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0

            memory[ 0x703] = trainer_time          ? 0x00 : 0x01; // LODI,r0 1             -> LODI,r0 0
        acase ENTERPRISE3POS:
            memory[0x114C] = trainer_time          ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
        acase ESCAPEPOS:
            memory[ 0x1FA] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0

            memory[ 0x25D] = trainer_invincibility ? 0x64 : 0x44; // ANDI,r0 2             -> IORI,r0 2
            memory[ 0x277] = trainer_invincibility ? 0x64 : 0x44; // ANDI,r0 4             -> IORI,r0 4
            memory[ 0x27D] = trainer_invincibility ? 0x64 : 0x44; // ANDI,r0 $20           -> IORI,r0 $20

            memory[ 0x733] = trainer_time          ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
        acase FROGGER1POS:
            memory[ 0x662] = trainer_invincibility ? 0xC0 : 0x3F; // BSTA,un $A7A          -> NOP snake
            memory[ 0x663] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x664] = trainer_invincibility ? 0xC0 : 0x7A; //                          NOP
            memory[ 0x67C] = trainer_invincibility ? 0xC0 : 0x3C; // BSTA,eq $A7A          -> NOP north
            memory[ 0x67D] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x67E] = trainer_invincibility ? 0xC0 : 0x7A; //                          NOP
            memory[ 0x687] = trainer_invincibility ? 0xC0 : 0xBC; // BSFA,eq $A7A          -> NOP south
            memory[ 0x688] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x689] = trainer_invincibility ? 0xC0 : 0x7A; //                          NOP
            memory[ 0x8FF] = trainer_invincibility ? 0xC0 : 0x3F; // BSTA,un $A7A          -> NOP not-home
            memory[ 0x900] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x901] = trainer_invincibility ? 0xC0 : 0x7A; //                          NOP

            memory[ 0x96A] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0xA91] = trainer_lives         ? 0x1F : 0x0C; // LODA,r0 $18C2         -> BCTA,un $AB9
            memory[ 0xA92] = trainer_lives         ? 0x0A : 0x18;
            memory[ 0xA93] = trainer_lives         ? 0xB9 : 0xC2;
        acase FROGGER2POS:
            memory[ 0x663] = trainer_invincibility ? 0xC0 : 0x3F; // BSTA,un $ADD          -> NOP snake
            memory[ 0x664] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x665] = trainer_invincibility ? 0xC0 : 0xDD; //                          NOP
            memory[ 0x67D] = trainer_invincibility ? 0xC0 : 0x3C; // BSTA,eq $ADD          -> NOP north
            memory[ 0x67E] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x67F] = trainer_invincibility ? 0xC0 : 0xDD; //                          NOP
            memory[ 0x688] = trainer_invincibility ? 0xC0 : 0xBC; // BSFA,eq $ADD          -> NOP south
            memory[ 0x689] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x68A] = trainer_invincibility ? 0xC0 : 0xDD; //                          NOP
            memory[ 0x962] = trainer_invincibility ? 0xC0 : 0x3F; // BSTA,un $ADD          -> NOP not-home
            memory[ 0x963] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x964] = trainer_invincibility ? 0xC0 : 0xDD; //                          NOP

            memory[ 0x9CD] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0xAFC] = trainer_lives         ? 0x1F : 0x0C; // LODA,r0 $18C2         -> BCTA,un DIEROUTINE_END ($B24)
            memory[ 0xAFD] = trainer_lives         ? 0x0B : 0x18;
            memory[ 0xAFE] = trainer_lives         ? 0x24 : 0xC2;
        acase FROGGER3POS:
            memory[ 0x663] = trainer_invincibility ? 0xC0 : 0x3F; // BSTA,un $ADD          -> NOP snake
            memory[ 0x664] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x665] = trainer_invincibility ? 0xC0 : 0xDD; //                          NOP
            memory[ 0x67D] = trainer_invincibility ? 0xC0 : 0x3C; // BSTA,eq $ADD          -> NOP north
            memory[ 0x67E] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x67F] = trainer_invincibility ? 0xC0 : 0xDD; //                          NOP
            memory[ 0x688] = trainer_invincibility ? 0xC0 : 0xBC; // BSFA,eq $ADD          -> NOP south
            memory[ 0x689] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x68A] = trainer_invincibility ? 0xC0 : 0xDD; //                          NOP
            memory[ 0x962] = trainer_invincibility ? 0xC0 : 0x3F; // BSTA,un $ADD          -> NOP not-home
            memory[ 0x963] = trainer_invincibility ? 0xC0 : 0x0A; //                          NOP
            memory[ 0x964] = trainer_invincibility ? 0xC0 : 0xDD; //                          NOP

            memory[ 0x9CD] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0xAF5] = trainer_lives         ? 0x1F : 0x0C; // LODA,r0 $18C2         -> BCTA,un DIEROUTINE_END ($B1D)
            memory[ 0xAF6] = trainer_lives         ? 0x0B : 0x18;
            memory[ 0xAF7] = trainer_lives         ? 0x1D : 0xC2;
        acase FUNKYFISHPOS:
            memory[  0xDD] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0x7AD] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0xBC3] = trainer_invincibility ? 0x1B : 0x9A; // BCFR,lt $BCA          -> BCTR,un $BCA
            memory[ 0xCF7] = trainer_invincibility ? 0x00 : 0x12; // BCTR,un $D0A          -> BCTR,un $CF8
        acase GUNDAMPOS:
            memory[ 0x5C2] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
            memory[ 0xC00] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase HEADONPOS:
            memory[  0xDE] = trainer_invincibility ? 0x00 : 0x38; // ANDI,r3 $38           -> ANDI,r3 0
        acase HELICOPTERPOS:
            memory[ 0xD80] = trainer_time          ? 0x02 : 0x82; // ADDZ    r2            -> LODZ    r2
        acase HOBOPOS1:
        case  HOBOPOS2:
            memory[ 0x1B4] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
            memory[ 0x1D8] = trainer_lives         ? 0x00 : 0x01; // ADDI,r2 1             -> ADDI,r2 0

            memory[ 0x65C] = trainer_invincibility ? 0x00 : 0x3C; // LODI,r0 $3C           -> LODI,r0 $00  (level 3)
            memory[ 0x830] = trainer_invincibility ? 0x17 : 0x0C; // LODA,r0 SPRITE0Y      -> RETC,un      (level 1)
            memory[ 0x9A0] = trainer_invincibility ? 0x17 : 0x20; // EORZ    r0            -> RETC,un      (level 2)
        acase HOMERUNPOS:
            memory[ 0x233] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase I_BOXINGPOS:
            memory[ 0x100] = trainer_time          ? 0x00 : 0x01; // SUBI,r2 1             -> SUBI,r2 0
        acase I_CIRCUSPOS:
            memory[ 0x3DF] = trainer_lives         ? 0x00 : 0x01; // SUBI,r1 1             -> SUBI,r1 0
        acase I_GOLFPOS:
            memory[ 0x787] = trainer_time          ? 0x66 : 0x67; // LODI,r0 $67           -> LODI,r0 $66
        acase I_MATH1POS:
            memory[ 0x306] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase I_MATH2POS:
            memory[ 0x2DC] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase E_MATH1POS:
            memory[ 0xD06] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase IKANASHIPOS:
            memory[ 0x275] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase INVADERAPOS:
            memory[ 0x377] = trainer_lives         ? 0x00 : 0x01; // SUBI,r2 1             -> SUBI,r2 0
        acase INVADERBPOS:
            memory[ 0x36C] = trainer_lives         ? 0x00 : 0x01; // SUBI,r2 1             -> SUBI,r2 0
        acase JOURNEYPOS:
            memory[0x2408] = trainer_lives         ? 0x00 : 0x01; // SUBI,r1 1             -> SUBI,r1 0

            memory[ 0x196] = trainer_invincibility ? 0x00 : 0x01; // BCTR,eq $198          -> BCTR,eq $197
        acase JUMPBUG1POS:
            memory[ 0x16B] = trainer_lives         ? 0x00 : 0x01;
            memory[ 0x190] = trainer_lives         ? 0x00 : 0x01;

            memory[ 0x354] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $35C          -> NOP
            memory[ 0x355] = trainer_invincibility ? 0xC0 : 0x06; //                          NOP
            memory[ 0x358] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $35C          -> NOP
            memory[ 0x359] = trainer_invincibility ? 0xC0 : 0x02; //                          NOP
            memory[ 0x4CD] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $4DB          -> NOP
            memory[ 0x4CE] = trainer_invincibility ? 0xC0 : 0x0C; //                          NOP
            memory[ 0x4D1] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $4DB          -> NOP
            memory[ 0x4D2] = trainer_invincibility ? 0xC0 : 0x08; //                          NOP

            memory[ 0x394] = trainer_time          ? 0x00 : 0xFF;
        acase JUMPBUG2POS:
            memory[ 0x149] = trainer_lives         ? 0x00 : 0x01;

            memory[ 0x305] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $30D          -> NOP
            memory[ 0x306] = trainer_invincibility ? 0xC0 : 0x06; //                          NOP
            memory[ 0x309] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $30D          -> NOP
            memory[ 0x30A] = trainer_invincibility ? 0xC0 : 0x02; //                          NOP
            memory[ 0x31C] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $324          -> NOP
            memory[ 0x31D] = trainer_invincibility ? 0xC0 : 0x06; //                          NOP
            memory[ 0x320] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $324          -> NOP
            memory[ 0x321] = trainer_invincibility ? 0xC0 : 0x02; //                          NOP
            memory[ 0x47E] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $48C          -> NOP
            memory[ 0x47F] = trainer_invincibility ? 0xC0 : 0x0C; //                          NOP
            memory[ 0x482] = trainer_invincibility ? 0xC0 : 0x18; // BCTR,eq $48C          -> NOP
            memory[ 0x483] = trainer_invincibility ? 0xC0 : 0x08; //                          NOP

            memory[ 0x345] = trainer_time          ? 0x00 : 0xFF;
        acase JUNGLERPOS:
            memory[ 0x149] = trainer_lives         ? 0x00 : 0x01;
        acase LASERATTACKPOS:
            memory[ 0x2E8] = trainer_lives         ? 0xC0 : 0xC8; // STRR,r0 *$2D9         -> NOP
            memory[ 0x2E9] = trainer_lives         ? 0xC0 : 0xEF; //                          NOP

            memory[ 0x4AD] = trainer_invincibility ? 0x00 : 0x01; // LODI,r0 1             -> LODI,r0 0
        acase LEAPFROGPOS:
            memory[ 0x4A0] = trainer_lives         ? 0x00 : 0x24; // IORI,r2 $24           -> IORI,r2 0

            memory[ 0x9CF] = trainer_time          ? 0xC0 : 0xD1; // RRL     r1            -> NOP
        acase MACROSSPOS:
            memory[ 0x21D] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
            memory[ 0x5E2] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[0x2946] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase MISSILEWARPOS:
            memory[ 0xE8B] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
            memory[ 0xE8F] = trainer_time          ? 0x17 : 0x04; // LODI,r0 $50           -> RETC,un
        acase MONACOPOS:
            memory[ 0x72F] = trainer_time          ? 0x00 : 0xFF; // ADDI,r0 $FF           -> ADDI,r0 0
            memory[ 0x730] = trainer_time          ? 0xC0 : 0x94; // DAR,r0                -> NOP
            memory[ 0x740] = trainer_time          ? 0x00 : 0xFF; // ADDI,r1 $FF           -> ADDI,r1 0
        acase MOONLANDING1POS:
        case MOONLANDING2POS:
            memory[ 0xA89] = trainer_time          ? 0x04 : 0x03; // LODZ    r3            -> LODI,r0 $F0
            memory[ 0xA8A] = trainer_time          ? 0xF0 : 0x24; // EORI,r0 $FF
            memory[ 0xA8B] = trainer_time          ? 0xCC : 0xFF; //                       -> STRA,r0 $1F52
            memory[ 0xA8C] = trainer_time          ? 0x1F : 0xC8; // STRR,r0 *$A71
            memory[ 0xA8D] = trainer_time          ? 0x52 : 0xE3;
            memory[ 0xA8E] = trainer_time          ? 0xCC : 0x5B; // BRNR,r3 $A89          -> STRA,r0 $1F52
            memory[ 0xA8F] = trainer_time          ? 0x1F : 0x79;
            memory[ 0xA90] = trainer_time          ? 0xC8 : 0xCB; // STRR,r3 *$A71
            memory[ 0xA91] = trainer_time          ? 0x1F : 0xDF; //                       -> BCTA,un $A66
            memory[ 0xA92] = trainer_time          ? 0x0A : 0x1F; // BCTA,un $A00
            memory[ 0xA93] = trainer_time          ? 0x66 : 0x0A;

            memory[ 0xC31] = trainer_time          ? 0xC0 : 0xD8; // BIRR,r0 $C33          -> NOP
            memory[ 0xC32] = trainer_time          ? 0xC0 : 0x00; //                          NOP

            memory[ 0xC56] = trainer_invincibility ? 0x00 : 0x0A; // BCTR,eq $C61          -> BCTR,eq $C57
        acase OMEGALANDINGPOS:
            // player-enemy collisions
            memory[ 0xF00] = trainer_invincibility ? 0x1B : 0x18; // BCTR,eq $F13          -> BCTR,un $F13

            if (memmap == MEMMAP_F) // because our patch is in expanded memory
            {   // player-building collisions
                memory[ 0xF19] = trainer_invincibility ? 0x1F : 0xCF; // STRA,r3 $1F5F         -> BCTA,un $1000
                memory[ 0xF1A] = trainer_invincibility ? 0x10 : 0x1F;
                memory[ 0xF1B] = trainer_invincibility ? 0x00 : 0x5F;
                memory[0x1000] = trainer_invincibility ? 0x47 : 0x00; //                          ANDI,r3 $7F
                memory[0x1001] = trainer_invincibility ? 0x7F : 0x00;
                memory[0x1002] = trainer_invincibility ? 0xCF : 0x00; //                          STRA,r3 $1F5F
                memory[0x1003] = trainer_invincibility ? 0x1F : 0x00;
                memory[0x1004] = trainer_invincibility ? 0x5F : 0x00;
                memory[0x1005] = trainer_invincibility ? 0x1F : 0x00; //                          BCTA,un $1FC
                memory[0x1006] = trainer_invincibility ? 0x0F : 0x00;
                memory[0x1007] = trainer_invincibility ? 0x1C : 0x00;
            }
        acase PARASHOOTERPOS:
            memory[ 0x269] = trainer_lives         ? 0x00 : 0x01; // SUBI,r1 1             -> SUBI,r1 0
        acase E_PINBALLPOS:
            memory[0x1359] = trainer_lives         ? 0x00 : 0x10; // SUBI,r1 $10           -> SUBI,r1 $00
        acase PLEIADESPOS:
            memory[ 0x6A8] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0x683] = memory[0xCE3] = trainer_invincibility ? 0x04 : 0x0C; // LODA,r0 $18D5 -> LODI,r0 1
            memory[ 0x684] = memory[0xCE4] = trainer_invincibility ? 0x01 : 0x18;
            memory[ 0x685] = memory[0xCE5] = trainer_invincibility ? 0xC0 : 0xD5; //                  NOP
        acase R2DTANKPOS:
            memory[ 0x4D4] = trainer_lives         ? 0x00 : 0x01; // ADDI,r1 1             -> ADDI,r1 0 (player 1)
            memory[ 0x4F0] = trainer_lives         ? 0x00 : 0x01; // ADDI,r1 1             -> ADDI,r1 0 (player 2)
        acase REDCLASHPOS:
        case REDCLASHODPOS:
            memory[ 0x4C0] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0x500] = trainer_invincibility ? 0x00 : 0x70; // LODI,r0 $70           -> LODI,r0 0
        acase ROBOTKILLERPOS:
            memory[ 0x24B] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0

            memory[ 0x2AE] = trainer_invincibility ? 0x64 : 0x44; // ANDI,r0 2             -> IORI,r0 2
            memory[ 0x2C8] = trainer_invincibility ? 0x64 : 0x44; // ANDI,r0 4             -> IORI,r0 4
            memory[ 0x2CE] = trainer_invincibility ? 0x64 : 0x44; // ANDI,r0 $20           -> IORI,r0 $20

            memory[ 0x73D] = trainer_time          ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
        acase ROCKETSHOOTINGPOS:
            memory[ 0xBED] = trainer_time          ? 0x00 : 0x01; // SUBI,r6 1             -> SUBI,r6 0
        acase ROUTE16POS:
            memory[ 0xA7A] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase SPACEATTACKAPOS:
            memory[ 0x18B] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0x2EF] = trainer_invincibility ? 0x00 : 0x04; // TMI,r2  $04           -> TMI,r2  $00
            memory[ 0x2F5] = trainer_invincibility ? 0x00 : 0x10; // TMI,r2  $10           -> TMI,r2  $00

            memory[ 0x310] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase SPACEATTACKBPOS:
            memory[ 0x1C5] =                         0x00       ; // BCTR,eq $1EC          -> BCTR,eq $1C6 (compatibility patch)

            memory[ 0x1F9] = trainer_invincibility ? 0x00 : 0x04; // TMI,r2  $04           -> TMI,r2  $00
            memory[ 0x1FF] = trainer_invincibility ? 0x00 : 0x10; // TMI,r2  $10           -> TMI,r2  $00

            memory[ 0x779] = trainer_time          ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0

            memory[ 0x7CB] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase SPACEATTACKCPOS:
            memory[ 0x1C5] =                         0x00       ; // BCTR,eq $1EC          -> BCTR,eq $1C6 (compatibility patch)

            memory[ 0x1F9] = trainer_invincibility ? 0x00 : 0x04; // TMI,r2  $04           -> TMI,r2  $00
            memory[ 0x1FF] = trainer_invincibility ? 0x00 : 0x10; // TMI,r2  $10           -> TMI,r2  $00

            memory[ 0x21A] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase SPACEBATTLEPOS:
            memory[ 0x923] = trainer_time          ? 0x1F : 0xFC; // BDRA,r0 $8D0          -> BCTA,un $8D0
            memory[ 0x936] = trainer_time          ? 0x1F : 0xFC; // BDRA,r0 $8D8          -> BCTA,un $8D8
        acase SPACEBUSTERPOS:
            memory[ 0x2D2] = trainer_invincibility ? 0x1B : 0x98; // BCFR,eq $2D7          -> BCTR,un $2D7

            memory[ 0x473] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase SPACEMISSIONPOS:
            memory[ 0x692] = trainer_time          ? 0x00 : 0xFF; // ADDI,r0 255           -> ADDI,r0 0
            memory[ 0x693] = trainer_time          ? 0xC0 : 0x94; // DAR                   -> NOP
        acase SPACESHOOTOUTPOS:
            memory[ 0xC06] = trainer_time          ? 0x00 : 0x01; // SUBI,r6 1             -> SUBI,r6 0
        acase SPACEVULTURESPOS:
            memory[ 0x6FE] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase SPACEWARPOS: // Interton
            memory[ 0x15C] = trainer_time          ? 0x00 : 0xFF; // ADDI,r0 255           -> ADDI,r0 0
            memory[ 0x15D] = trainer_time          ? 0xC0 : 0x94; // DAR                   -> NOP
        acase NIBBLEMENPOS:
        case SUPERGOBBLERPOS:
            memory[ 0x281] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
        acase SPACERAIDERSPOS:
            memory[ 0x10C] = trainer_time          ? 0x00 : 0xFF; // ADDI,r1 $FF           -> ADDI,r0 0

            memory[ 0x1A5] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        // execution flows through here
        case SPACESQUADRON1POS:
        case SPACESQUADRON2POS:
            if (whichgame != SPACERAIDERSPOS)
            {   memory[ 0x1A8] = trainer_lives     ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
            }

            if (trainer_invincibility)
            {   if (whichgame == SPACERAIDERSPOS)
                {   memory[ 0x752] = 0x1F; // BCTA,un $2040
                    memory[ 0x753] = 0x20;
                    memory[ 0x754] = 0x40;
                    memory[0x2054] = 0x07; // BCTA,un $755
                    memory[0x2055] = 0x55;
                } else
                {   memory[ 0x869] = 0x1F; // BCTA,un $2040
                    memory[ 0x86A] = 0x20;
                    memory[ 0x86B] = 0x40;
                    memory[0x2054] = 0x08; // BCTA,un $86C
                    memory[0x2055] = 0x6C;

                }
                memory[0x2040] = 0x0E; // LODA,r2 $1AB4
                memory[0x2041] = 0x1A;
                memory[0x2042] = 0xB4;
                memory[0x2043] = 0x66; // IORI,r2 $03
                memory[0x2044] = 0x03;
                memory[0x2045] = 0xCE; // STRA,r2 $1AB4
                memory[0x2046] = 0x1A;
                memory[0x2047] = 0xB4;
                memory[0x2048] = 0x0E; // LODA,r2 $1AB5
                memory[0x2049] = 0x1A;
                memory[0x204A] = 0xB5;
                memory[0x204B] = 0x66; // IORI,r2 $1F
                memory[0x204C] = 0x1F;
                memory[0x204D] = 0xCE; // STRA,r2 $1AB5
                memory[0x204E] = 0x1A;
                memory[0x204F] = 0xB5;
                memory[0x2050] = 0x0E; // LODA,r2 P1PADDLE
                memory[0x2051] = 0x19;
                memory[0x2052] = 0xFF;
                memory[0x2053] = 0x1F; // BCTA,un
            } else
            {   if (whichgame == SPACERAIDERSPOS)
                {   memory[ 0x752] = 0x0E; // LODA,r2 P1PADDLE
                    memory[ 0x753] = 0x19;
                    memory[ 0x754] = 0xFF;
                } else
                {   memory[ 0x869] = 0x0E; // LODA,r2 P1PADDLE
                    memory[ 0x86A] = 0x19;
                    memory[ 0x86B] = 0xFF;
                }
                for (i = 0x2040; i <= 0x2055; i++)
                {   memory[i] = 0;
            }   }
        acase SPIDERSPOS:
        case SPIDERSODPOS:
            memory[ 0x22A] = trainer_lives         ? 0x00 : 0x01; // ADDI,r2 1             -> ADDI,r2 0
        acase SUBMARINESRACINGPOS:
            memory[ 0x9AF] = trainer_lives         ? 0x66 : 0x67; // ADDI,r3 $67           -> ADDI,r3 $66 (for Racing)

            memory[ 0xDA2] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0   (for Submarines)
        acase SUPERBUG1POS:
        case SUPERBUG2POS:
            memory[ 0xAC3] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0

            memory[ 0x3A1] = trainer_invincibility ? 0xC0 : 0x9C;
            memory[ 0x3A2] = trainer_invincibility ? 0xC0 : 0x0A;
            memory[ 0x3A3] = trainer_invincibility ? 0xC0 : 0x91;
            memory[ 0x47E] = trainer_invincibility ? 0xC0 : 0x1C;
            memory[ 0x47F] = trainer_invincibility ? 0xC0 : 0x0A;
            memory[ 0x480] = trainer_invincibility ? 0xC0 : 0x91;
            memory[ 0x483] = trainer_invincibility ? 0xC0 : 0x1C;
            memory[ 0x484] = trainer_invincibility ? 0xC0 : 0x0A;
            memory[ 0x485] = trainer_invincibility ? 0xC0 : 0x91;

            memory[ 0x321] = trainer_time          ? 0x00 : 0x01; // SUBI,r2 1             -> SUBI,r2 0
        acase SUPERINVPOS:
            memory[  0x80] = trainer_time          ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0
        acase INTELLIGENCE1POS:
            memory[ 0x1EA] = trainer_time          ? 0x00 : 0x01;
        acase SUPERMAZEPOS:
            memory[ 0x1EE] = trainer_time          ? 0x00 : 0x01;
        acase MAZESPOS:
            memory[ 0xAEA] = trainer_time          ? 0x00 : 0x01;
        acase SUPERKNOCKOUTPOS:
            memory[ 0x545] = trainer_lives         ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase TANKSALOTPOS:
            memory[ 0x247] = trainer_lives         ? 0x00 : 0x01; // ADDI,r0 1             -> ADDI,r0 0

            memory[ 0x2CE] = trainer_invincibility ? 0x11 : 0x05;
            memory[ 0x2CF] = trainer_invincibility ? 0x1B : 0xFF;
            memory[ 0x5D8] = trainer_invincibility ? 0x19 : 0x00;
        acase TARGETPOS:
            memory[ 0x5E9] = trainer_time          ? 0x00 : 0x01; // SUBI,r0 1             -> SUBI,r0 0
        acase THEENDPOS1:
        case THEENDPOS2:
            memory[ 0x173] = trainer_lives         ? 0x00 : 0x01; // ADDI,r2 1             -> ADDI,r2 0
        acase TINYTIMPOS:
            memory[ 0xD46] = trainer_time          ? 0x00 : 0x01;
        acase TURTLESPOS:
            memory[ 0x153] = trainer_lives         ? 0x00 : 0x01; // ADDI,r2 1             -> ADDI,r2 0

            memory[ 0x386] = trainer_invincibility ? 0x00 : 0x05;

            memory[ 0x64A] = trainer_time          ? 0x00 : 0xFF; // ADDI,r0 $FF           -> ADDI,r0 0 // energy
        acase UFOSHOOTINGPOS:
            memory[ 0x9E6] = trainer_time          ? 0x00 : 0x01; // LODI,r1 1             -> LODI,r1 0 // ammo
#ifdef ADVENTURETRAINER
        adefault:
            if (machine == BINBUG && whichgame == -1)
            {   if (trainer_time)
                {   memory[0x1E32] =                 0x01; // lamp is lit
                    memory[0x1E33] =                 0x40; // oil level in lamp
                    memory[0x1E86] =                 0xFF; // lamp is no longer in house
                    memory[0x1EB3] =                 0x80; // lamp is carried
                }
                memory[0x16EF] = trainer_time      ? 0x00 : 0x01; // SUBI,r0 $01           -> SUBI,r0 0 // oil
            }
#endif
}   }   }

EXPORT void keynames_from_overlay(void)
{   int i;

    machines[machine].keynames[0][ 2] = overlays[whichoverlay][ 0]; // 2
    machines[machine].keynames[1][ 2] = overlays[whichoverlay][ 1]; // 2
    machines[machine].keynames[0][13] = overlays[whichoverlay][ 2]; // x1
    machines[machine].keynames[0][14] = overlays[whichoverlay][ 3]; // x2
    machines[machine].keynames[0][15] = overlays[whichoverlay][ 4]; // x3
    machines[machine].keynames[1][13] = overlays[whichoverlay][ 5]; // x1
    machines[machine].keynames[1][14] = overlays[whichoverlay][ 6]; // x2
    machines[machine].keynames[1][15] = overlays[whichoverlay][ 7]; // x3
    machines[machine].keynames[0][ 1] = overlays[whichoverlay][ 8]; // 1
    machines[machine].keynames[0][16] = overlays[whichoverlay][ 9]; // x4
    machines[machine].keynames[0][ 3] = overlays[whichoverlay][10]; // 3
    machines[machine].keynames[1][ 1] = overlays[whichoverlay][11]; // 1
    machines[machine].keynames[1][16] = overlays[whichoverlay][12]; // x4
    machines[machine].keynames[1][ 3] = overlays[whichoverlay][13]; // 3
    machines[machine].keynames[0][ 4] = overlays[whichoverlay][14]; // 4
    machines[machine].keynames[0][ 5] = overlays[whichoverlay][15]; // 5
    machines[machine].keynames[0][ 6] = overlays[whichoverlay][16]; // 6
    machines[machine].keynames[1][ 4] = overlays[whichoverlay][17]; // 4
    machines[machine].keynames[1][ 5] = overlays[whichoverlay][18]; // 5
    machines[machine].keynames[1][ 6] = overlays[whichoverlay][19]; // 6
    machines[machine].keynames[0][ 7] = overlays[whichoverlay][20]; // 7
    machines[machine].keynames[0][ 8] = overlays[whichoverlay][21]; // 8
    machines[machine].keynames[0][ 9] = overlays[whichoverlay][22]; // 9
    machines[machine].keynames[1][ 7] = overlays[whichoverlay][23]; // 7
    machines[machine].keynames[1][ 8] = overlays[whichoverlay][24]; // 8
    machines[machine].keynames[1][ 9] = overlays[whichoverlay][25]; // 9
    machines[machine].keynames[0][10] = overlays[whichoverlay][26]; // Cl
    machines[machine].keynames[0][11] = overlays[whichoverlay][27]; // 0
    machines[machine].keynames[0][12] = overlays[whichoverlay][28]; // En
    machines[machine].keynames[1][10] = overlays[whichoverlay][29]; // Cl
    machines[machine].keynames[1][11] = overlays[whichoverlay][30]; // 0
    machines[machine].keynames[1][12] = overlays[whichoverlay][31]; // En

    for (i = 0; i < 2; i++)
    {   machines[machine].keynames[i][ 0] = LLL(MSG_1ST      , "1st" );
        machines[machine].keynames[i][17] = LLL(MSG_KEY_UP   , "Up"  );
        machines[machine].keynames[i][18] = LLL(MSG_KEY_DN   , "Dn"  );
        machines[machine].keynames[i][19] = LLL(MSG_KEY_LT   , "Lt"  );
        machines[machine].keynames[i][20] = LLL(MSG_KEY_RT   , "Rt"  );
        machines[machine].keynames[i][21] = LLL(MSG_2ND      , "2nd" );
        machines[machine].keynames[i][22] = LLL(MSG_3RD      , "3rd" );
        machines[machine].keynames[i][23] = LLL(MSG_4TH      , "4th" );
        machines[machine].keynames[i][24] = LLL(MSG_UPLEFT   , "UpLt");
        machines[machine].keynames[i][25] = LLL(MSG_UPRIGHT  , "UpRt");
        machines[machine].keynames[i][26] = LLL(MSG_DOWNLEFT , "DnLt");
        machines[machine].keynames[i][27] = LLL(MSG_DOWNRIGHT, "DnRt");
}   }

EXPORT void help_update(FLAG quiet)
{   double             oldver,
                       newver;
    STRPTR             message;
    int                hSocket,
                       i, j,
                       length;
    char               ip[15 + 1]; // enough for "208.115.246.164"
    TEXT               tempstring[80 + 1];
    struct sockaddr_in INetSocketAddr;
#ifndef __amigaos4__
    struct hostent*    HostAddr;
    struct in_addr**   addr_list;
#endif

#ifdef AMIGA
    // assert(SocketBase);
#endif

    sound_off(FALSE);
    busypointer();

#ifdef __amigaos4__
    strcpy(ip, "216.245.218.214"); // was 208.115.246.164
#else
    HostAddr = gethostbyname((const UBYTE*) "amigan.1emu.net");
    if (HostAddr)
    {   // Cast the h_addr_list to in_addr, since h_addr_list also has the IP address in long format only
        addr_list = (struct in_addr**) HostAddr->h_addr_list;
        for (i = 0; addr_list[i] != NULL; i++)
        {
#ifdef __MORPHOS__
            strcpy(ip, Inet_NtoA(addr_list[i]->s_addr));
#else
            strcpy(ip, (char*) Inet_NtoA(*addr_list[i]));
#endif
    }   }
    else
    {   strcpy(ip, "216.245.218.214"); // was 208.115.246.164
    }
#endif

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Checking for update at %s...\n", ip);
#endif

    hSocket = (int) socket(AF_INET, SOCK_STREAM, 0);
    if (hSocket == -1)
    {   normalpointer();
        say("Socket allocation failed!");
        return;
    }

    INetSocketAddr.sin_family      = AF_INET;
#ifdef WIN32
    INetSocketAddr.sin_port        = htons(80); // HTTP
#endif
#ifdef AMIGA
    INetSocketAddr.sin_len         = 16; // sizeof(INetSocketAddr)
    INetSocketAddr.sin_port        = 80; // HTTP
#endif
#ifdef __MORPHOS__
    INetSocketAddr.sin_addr.s_addr = (long unsigned int) inet_addr((const unsigned char*) ip);
#else
    INetSocketAddr.sin_addr.s_addr =                     inet_addr(                       ip);
#endif
    for (i = 0; i < 8; i++)
    {   INetSocketAddr.sin_zero[i] = 0;
    }

#ifdef __amigaos4__
    if (connect(hSocket, (      struct sockaddr*) &INetSocketAddr, 16) == -1)
#else
    if (connect(hSocket, (const struct sockaddr*) &INetSocketAddr, 16) == -1)
#endif
    {   DISCARD CloseSocket(hSocket);
        normalpointer();
        say("Socket connection failed!");
        return;
    }

#ifdef WIN32
    message = "GET /releases/wa.txt HTTP/1.1\r\nHost: amigan.1emu.net:80\r\n\r\n";
#endif
#ifdef AMIGA
    #ifdef __MORPHOS__
        message = "GET /releases/aa-mos.txt HTTP/1.1\r\nHost: amigan.1emu.net:80\r\n\r\n";
    #else
        #ifdef __amigaos4__
            message = "GET /releases/aa-os4.txt HTTP/1.1\r\nHost: amigan.1emu.net:80\r\n\r\n";
        #else
            message = "GET /releases/aa-os3.txt HTTP/1.1\r\nHost: amigan.1emu.net:80\r\n\r\n";
        #endif
    #endif
#endif

#ifdef __MORPHOS__
    if (send(hSocket, (const UBYTE*) message, strlen((const char*) message), 0) < 0)
#else
    if (send(hSocket, (      STRPTR) message, strlen(              message), 0) < 0)
#endif
    {   DISCARD CloseSocket(hSocket);
        normalpointer();
        say("Can't send query to server!");
        return;
    }

    length = (int) recv(hSocket, replystring, 1000, 0);
    if (length < 0)
    {   DISCARD CloseSocket(hSocket);
        normalpointer();
        say("Can't receive response from server!");
        return;
    } else
    {   replystring[length] = EOS;
    }

    // assert(SocketBase);
    // assert(hSocket != -1);
    DISCARD CloseSocket(hSocket);
    // hSocket = -1;
    normalpointer();

    length = strlen((const char*) replystring);
    i = length - 4;
    j = length;
    while (i >= 0)
    {   if (replystring[i] == CR && replystring[i + 1] == LF && replystring[i + 2] == CR && replystring[i + 3] == LF)
        {   j = i + 4;
            break;
        } else
        {   i--;
    }   }

    if (j == length)
    {   say("Invalid response from server!");
        return;
    } elif (replystring[j] == 239 && replystring[j + 1] == 187 && replystring[j + 2] == 191) // because sometimes it is prepended with "ï»¿"
    {   j += 3; // skip crap
    }

    if (replystring[j] < '0' || replystring[j] > '9')
    {   say("Can't download version file!");
        return;
    }

#ifdef WIN32
    oldver = zatof(DECIMALVERSION);
    newver = zatof(&replystring[j]);
#endif
#ifdef AMIGA
    oldver = atof(DECIMALVERSION);
    newver = atof((const char*) &replystring[j]);
#endif

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "V%.2f vs. V%.2f\n", (float) oldver, (float) newver);
#endif
    if (newver > oldver)
    {
#ifdef WIN32
        sprintf(        tempstring, "WinArcadia %.2f %s!", (float) newver, LLL(MSG_UPDATE_YES, "is available")); // we would prefer eg. 25.0 instead of 25.00
#endif
#ifdef AMIGA
        sprintf((char*) tempstring, "AmiArcadia %.2f %s!", (float) newver, LLL(MSG_UPDATE_YES, "is available")); // we would prefer eg. 25.0 instead of 25.00
#endif
        say((STRPTR) tempstring);
        openurl("http://amigan.1emu.net/releases/#amiarcadia");
    } elif (!quiet)
    {   saymessage(LLL(MSG_UPDATE_NO, "You are up to date."));
}   }

EXPORT int parse_bytes(int mode)
{   TRANSIENT int   banktouse,
                    bankoffset,
                    i, j,
                    kind,
                    newmachine,
                    newmemmap,
                    tempsize;
    PERSIST   TEXT  tempstring[80 + 1];

    // assert(!crippled);
    // assert(!inframe);

#ifdef WIN32
    if (cheevos && !RA_ConfirmLoadNewRom(FALSE))
    {   return 0;
    }
#endif

    serializemode = SERIALIZE_READ;
    recmode = RECMODE_NORMAL;

 /* TVC files are:
        0:  magic byte    (always $02)
     1..2:  load address  (where to load program to)
     3..4:  start address (where to point IAR to)
        5+: the program itself

    Magic bytes are:
    $02 = LODZ r2 = TVC
    $3A = BSTR,lt = AOF/HEX
    $4C = ANDA,r0 = EOF
    $BF = BSXA    = COS/COR
    all others    = Arcadia/Interton/SI50/PHUNSY BIN */

    kind = identify(fn_game);
    offset = 0;
    switch (kind)
    {
    case KIND_CMD:
        changemachine(BINBUG, MEMMAP_BINBUG, FALSE, FALSE, (FLAG) ((mode == 1) ? TRUE : FALSE));
        if (!load_cmd(filesize))
        {   sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "CMD");
            say((STRPTR) tempstring);
            return 0;
        }
        engine_reset();
    return 2;
    case KIND_IMAG:
        changemachine(CD2650, MEMMAP_CD2650, FALSE, FALSE, (FLAG) ((mode == 1) ? TRUE : FALSE));
        if (!load_imag(filesize))
        {   sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "IMAG");
            say((STRPTR) tempstring);
            return 0;
        }
        engine_reset();
    return 2;
    case KIND_MOD:
        changemachine(TWIN, MEMMAP_TWIN, FALSE, FALSE, (FLAG) ((mode == 1) ? TRUE : FALSE));
        if (!load_mod(filesize))
        {   sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "MOD");
            say((STRPTR) tempstring);
            return 0;
        }
        engine_reset();
    return 2;
    case KIND_EOF:
#ifdef WIN32
        if (cheevos && !RA_WarnDisableHardcore("load an EOF"))
        {   return FALSE;
        }
#endif
        if (machine != ELEKTOR)
        {   changemachine(ELEKTOR, MEMMAP_F, FALSE, FALSE, (FLAG) ((mode == 1) ? TRUE : FALSE));
        }
        if (load_eof(filesize) == 0)
        {   sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "EOF");
            say((STRPTR) tempstring);
            return FALSE;
        }
        engine_reset();
    return 2;
    case KIND_HEX:
        if (ReadByteAt(0) != ':')
        {   sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "HEX");
            return 0;
        }
#ifdef WIN32
        if (cheevos && !RA_WarnDisableHardcore("load a HEX"))
        {   return 0;
        }
#endif
        if (load_aof(filesize, TRUE) == 0)
        {   sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "HEX");
            say((STRPTR) tempstring);
            return 0;
        }
        engine_reset();
    return 2;
    case KIND_BPNF:
        if (ReadByteAt(0) != 'B')
        {   sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "BPNF");
            return 0;
        }
#ifdef WIN32
        if (cheevos && !RA_WarnDisableHardcore("load a BPNF"))
        {   return 0;
        }
#endif
        if (!project_bpnf())
        {   return 0;
        }
        bpnf_address = parse_expression(bpnf_string, MAX_ADDR, FALSE);
        if (bpnf_address == OUTOFRANGE)
        {   return 0;
        }
        if (!load_bpnf(filesize, bpnf_address))
        {   sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "BPNF");
            say((STRPTR) tempstring);
            return 0;
        }
        engine_reset();
    return 2;
    case KIND_SMS:
        if (ReadByteAt(0) != 0x12)
        {   sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "SMS");
            return 0;
        }
#ifdef WIN32
        if (cheevos && !RA_WarnDisableHardcore("load an SMS"))
        {   return 0;
        }
#endif
        if (!load_sms(filesize))
        {   sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "SMS");
            say((STRPTR) tempstring);
            return 0;
        }
        engine_reset();
    return 2;
    case KIND_SYM:
        loadsym((TEXT*) IOBuffer);
    return 2;
    case KIND_MDCR:
        if (machine == PHUNSY)
        {   macro_stop();
            strcpy((char*) fn_tape[1], (const char*) fn_game); // ie. MDCR unit
            DISCARD insert_mdcr(FALSE);
            if (!SubWindowPtr[SUBWINDOW_TAPEDECK])
            {   open_tapedeck();
            }
            return 2;
        } else
        {   // perhaps we should change to PHUNSY instead of producing an error message
            say((STRPTR) LLL(MSG_BADFORMAT, "This file format is not supported for this guest!"));
            return 0;
        }
    case KIND_RAW:
        if (machine == BINBUG || machine == CD2650)
        {   macro_stop();
            j = 0;
            for (i = 0; i < machines[machine].drives; i++)
            {   if (!drive[i].inserted)
                {   j = i;
                    break;
            }   }
            strcpy((char*) drive[j].fn_disk, (const char*) fn_game);
            load_disk(FALSE, j, TRUE);
            return 2;
        } else
        {   say((STRPTR) LLL(MSG_BADFORMAT, "This file format is not supported for this guest!"));
            return 0;
        }
    case KIND_IMG:
    case KIND_TWIN:
        if (machine == TWIN)
        {   macro_stop();
            if (drive[0].inserted && !drive[1].inserted)
            {   strcpy((char*) drive[1].fn_disk, (const char*) fn_game);
                load_disk(FALSE, 1, TRUE);
            } else
            {   strcpy((char*) drive[0].fn_disk, (const char*) fn_game);
                load_disk(FALSE, 0, TRUE);
            }
            return 2;
        } else
        {   say((STRPTR) LLL(MSG_BADFORMAT, "This file format is not supported for this guest!"));
            return 0;
        }
    case KIND_ASM:
        strcpy((char*) fn_asm, (const char*) fn_game);
        assemble();
    return 2;
    case KIND_WAV:
    case KIND_8SVX:
    case KIND_AIFF:
        if (TAPABLE)
        {   macro_stop();
            strcpy((char*) fn_tape[0], (const char*) fn_game);
            load_tape(FALSE);
            if (!SubWindowPtr[SUBWINDOW_TAPEDECK])
            {   open_tapedeck();
            }
            return 2;
        } else
        {   say((STRPTR) LLL(MSG_BADFORMAT, "This file format is not supported for this guest!"));
            return 0;
        }
#ifdef WIN32
    case KIND_BMP: // no need for acase
        if (filter != FILTER_3D && filter != FILTER_HQX)
        {   enhancestars = TRUE;
            updatemenu(MENUITEM_SKIES);
        }
        if (pixelubyte)
        {   free(pixelubyte);
            pixelubyte = NULL;
            pixelulong = NULL;
        }
        strcpy((char*) fn_bkgrnd, (const char*) fn_game);
        // assert(fn_bkgrnd);
        // assert(fn_bkgrnd[0]);
        break_pathname(fn_bkgrnd, path_bkgrnd, file_bkgrnd);
        if (file_bkgrnd[0])
        {   DISCARD load_bmp(fn_bkgrnd);
        }
        DISCARD load_bmp(fn_bkgrnd);
        if (filter != FILTER_3D && filter != FILTER_HQX)
        {   fixupcolours();
            make_stars();
            drawpixelroutine();
            redrawscreen();
        }
    return 2;
#endif
    case KIND_PAP: // no need for acase
        if (HASPAPERTAPE)
        {   macro_stop();
            if (machine == TWIN)
            {   if (papertapemode[0] != TAPEMODE_NONE && papertapemode[1] == TAPEMODE_NONE)
                {   strcpy((char*) fn_tape[3], (const char*) fn_game);
                    load_papertape(FALSE, 1);
                } else
                {   strcpy((char*) fn_tape[2], (const char*) fn_game);
                    load_papertape(FALSE, 0);
                }
                return 2;
            } else
            {   strcpy((char*) fn_tape[2], (const char*) fn_game);
                load_papertape(FALSE, 0);
            }
            if (!SubWindowPtr[SUBWINDOW_PAPERTAPE])
            {   open_papertape();
            }
            return 2;
        } else
        {   say((STRPTR) LLL(MSG_BADFORMAT, "This file format is not supported for this guest!"));
            return 0;
        }
    case KIND_TVC: // no need for acase
        if (ReadByteAt(0) != 2)
        {   sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "TVC");
            return 0;
        }
        changemachine(ELEKTOR, MEMMAP_F, FALSE, FALSE, (FLAG) ((mode == 1) ? TRUE : FALSE));
        startaddr_h  = ReadByteAt(3);
        startaddr_l  = ReadByteAt(4);
        cheevosize   = filesize;
        memcpy(cheevomem, IOBuffer, cheevosize);
    acase KIND_AOF:
        if (ReadByteAt(0) != ':')
        {   sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "AOF");
            return 0;
        }
#ifdef WIN32
        if (cheevos && !RA_WarnDisableHardcore("load an AOF"))
        {   return 0;
        }
#endif
    acase KIND_COS:
    case KIND_COR:
#ifdef WIN32
        if (cheevos && !RA_WarnDisableHardcore("load a state/recording"))
        {   return FALSE;
        }
#endif
        cosversion = ReadByteAt(1);
        newmemmap  = ReadByteAt(3);
        if (newmemmap >= MEMMAPS)
        {   sprintf((char*) tempstring, LLL(MSG_UNSUPPORTEDVER1, "Unsupported %s version (future guest?)!"), "COS/COR");
            say((STRPTR) tempstring);
            return FALSE;
        }
        newmachine = memmapinfo[newmemmap].machine;
        if (cosversion != machines[newmachine].cosversion)
        {   if
            (   (newmachine == PIPBUG && cosversion == 41)
             || (newmachine == BINBUG && cosversion == 42)
             || (newmachine == TWIN   && cosversion == 39)
             || (newmachine == CD2650 && cosversion == 42)
            )
            {   ;
            } else
            {   sprintf((char*) tempstring, LLL(MSG_UNSUPPORTEDVER2, "Unsupported %s version (obsolete/future version)!"), "COS/COR");
                say((STRPTR) tempstring);
                return FALSE;
        }   }
        switch (ReadByteAt(2))
        {
        case 0:
        case 1:
            ; // OK
        acase 2:
            say("DroidArcadia COS files are not supported!");
        return 0;
        default:
            sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "COS/COR");
            say((STRPTR) tempstring);
        return 0;
        }
        changemachine(memmapinfo[newmemmap].machine, newmemmap, FALSE, FALSE, (FLAG) ((mode == 1) ? TRUE : FALSE));
    acase KIND_BIN:
        if (filesize > 32 * KILOBYTE)
        {   say((STRPTR) LLL(MSG_ENGINE_TOOLARGE, "File is too large or corrupt!"));
            return FALSE;
        }
        startaddr_h = startaddr_l = 0;
        cheevosize = filesize;
    adefault: // eg. ILBM/ACBM/BMP/PCX/GIF/TIFF/PNG/IFFANIM/PSG/YM/MNG/AVIANIM/MIDI/SMUS/ASCII/ICON
        say((STRPTR) LLL(MSG_NOTFORLOADING, "This file format is not supported for loading!"));
    return 0;
    }

    // assert(kind == KIND_COS || kind == KIND_COR || kind == AOF || kind == BIN || kind == TVC);

#ifdef VERBOSE
    zprintf
    (   TEXTPEN_VERBOSE,
        "File type is %s.\n" \
        "Start address is $%02X%02X.\n",
        filekind[kind].extension,
        startaddr_h, startaddr_l
    );
#endif

    if (mode == 0)
    {   whichgame = -1;
        switch (kind)
        {
        case KIND_COS:
        case KIND_COR:
            if
            (   machine == ARCADIA
             || machine == INTERTON
             || machine == ELEKTOR
             || machine == PIPBUG
             || machine == BINBUG
             || machine == INSTRUCTOR
             || machine == CD2650
             || machine == PHUNSY
             || machine == SELBST
             || machine == MIKIT
            ) // not TWIN, PONG, TYPERIGHT, coin-ops (they have no games in known[])
            {   whichgame = (int) ReadWordAt(36);
                if (whichgame < 0 || whichgame >= KNOWNGAMES)
                {   whichgame = -1;
#ifdef VERBOSE
                    zprintf(TEXTPEN_VERBOSE, "Unknown game!\n");
#endif
            }   }
        acase KIND_BIN:
            if (sensegame)
            {   autosense(); // this sets whichgame but not machine
            }
            if (whichgame == -1)
            {   switch (machine)
                {
                case ARCADIA:
                case INTERTON:
                case INSTRUCTOR:
                case PHUNSY:
                    ; // OK
                adefault:
                    say((STRPTR) LLL(MSG_BADFORMAT, "This file format is not supported for this guest!"));
                return 0;
            }   }
        acase KIND_TVC:
            if (sensegame)
            {   autosense(); // this sets whichgame but not machine
            }
            if (whichgame == -1)
            {   wantmemmap = MEMMAP_F;
            }
        acase KIND_AOF:
            if (sensegame)
            {   autosense(); // this sets whichgame but not machine
            }
            if (whichgame == -1)
            {   switch (machine)
                {
                case PIPBUG:
                case BINBUG:
                case INSTRUCTOR:
                case CD2650:
                case SELBST:
                case MIKIT:
                    ; // OK
                adefault:
                    say((STRPTR) LLL(MSG_BADFORMAT, "This file format is not supported for this guest!"));
                return 0;
        }   }   }

        configure();
    }

    game = TRUE; // must be done before engine_reset()!

    switch (kind)
    {
    case KIND_BIN:
        switch (machine)
        {
        case ARCADIA:
            arcadia_serializerom();
        acase INTERTON:
            interton_serializerom();
        acase INSTRUCTOR:
        case SELBST:
            for (i = 0; i < filesize; i++)
            {   serialize_byte(&memory[i]);
            }
        acase PHUNSY:
            if (whichgame == -1)
            {   banked = 0x00;
                if (filesize > 8192)
                {   tempsize = 8192;
                } else
                {   tempsize = filesize;
                }
                startaddr_l = 0x00;
                if (phunsy_biosver == PHUNSY_PHUNSY)
                {   startaddr_h = 0x20;
                    for (i = 0; i < tempsize; i++)
                    {   serialize_byte(&memory[0x2000 + i]); // $0000..$1FFF -> $2000..$3FFF
                }   }
                else // mini-monitor
                {   startaddr_h = 0x00;
                    for (i = 0; i < tempsize; i++)
                    {   serialize_byte(&memory[         i]); // $0000..$1FFF -> $0000..$1FFF
            }   }   }
            else
            {   banked = known[whichgame].banked;
                startaddr_h = (UBYTE) (known[whichgame].startaddr / 256); // needed!
                startaddr_l = (UBYTE) (known[whichgame].startaddr % 256); // needed!
                if (known[whichgame].the1stsize == 0)
                {   tempsize = filesize;
                } else
                {   tempsize = known[whichgame].the1stsize;
                }
                if (known[whichgame].the1staddr >= UBANK0 && known[whichgame].the1staddr < UBANKF + UBANKSIZE)
                {   // assert(tempsize <= 2048);
                    banktouse  = (known[whichgame].the1staddr - UBANK0) / UBANKSIZE;
                    bankoffset = (known[whichgame].the1staddr - UBANK0) % UBANKSIZE;
#ifdef VERBOSE
                    zprintf(TEXTPEN_VERBOSE, "1st chunk: $%04X..$%04X of U%X.\n", 0x1800 + bankoffset, 0x1800 + bankoffset + tempsize, banktouse);
#endif
                    for (i = 0; i < tempsize; i++)
                    {   serialize_byte(&u_bank[banktouse][bankoffset + i]);
                }   }
                elif (known[whichgame].the1staddr >= QBANK0 && known[whichgame].the1staddr < QBANKF + QBANKSIZE)
                {   // assert(tempsize <= 16384);
                    banktouse  = (known[whichgame].the1staddr - QBANK0) / QBANKSIZE;
                    bankoffset = (known[whichgame].the1staddr - QBANK0) % QBANKSIZE;
#ifdef VERBOSE
                    zprintf(TEXTPEN_VERBOSE, "1st chunk: $%04X..$%04X of Q%X.\n", 0x4000 + bankoffset, 0x4000 + bankoffset + tempsize, banktouse);
#endif
                    for (i = 0; i < tempsize; i++)
                    {   serialize_byte(&q_bank[banktouse][bankoffset + i]);
                }   }
                else
                {
#ifdef VERBOSE
                    zprintf(TEXTPEN_VERBOSE, "1st chunk: $%04X..$%04X.\n", known[whichgame].the1staddr, known[whichgame].the1staddr + tempsize);
#endif
                    for (i = 0; i < tempsize; i++)
                    {   serialize_byte(&memory[known[whichgame].the1staddr + i]);
                }   }

                if (known[whichgame].the2ndsize)
                {   if (known[whichgame].the2ndaddr >= UBANK0 && known[whichgame].the2ndaddr < UBANKF + UBANKSIZE)
                    {   // assert(known[whichgame].the2ndsize <= 2048);
                        banktouse  = (known[whichgame].the2ndaddr - UBANK0) / UBANKSIZE;
                        bankoffset = (known[whichgame].the2ndaddr - UBANK0) % UBANKSIZE;
#ifdef VERBOSE
                        zprintf(TEXTPEN_VERBOSE, "2nd chunk: $%04X..$%04X of U%X.\n", 0x1800 + bankoffset, 0x1800 + bankoffset + known[whichgame].the2ndsize, banktouse);
#endif
                        for (i = 0; i < known[whichgame].the2ndsize; i++)
                        {   serialize_byte(&u_bank[banktouse][bankoffset + i]);
                    }   }
                    elif (known[whichgame].the2ndaddr >= QBANK0 && known[whichgame].the2ndaddr < QBANKF + QBANKSIZE)
                    {   // assert(known[whichgame].the2ndsize <= 16384);
                        banktouse  = (known[whichgame].the2ndaddr - QBANK0) / QBANKSIZE;
                        bankoffset = (known[whichgame].the2ndaddr - QBANK0) % QBANKSIZE;
#ifdef VERBOSE
                        zprintf(TEXTPEN_VERBOSE, "2nd chunk: $%04X..$%04X of Q%X.\n", 0x4000 + bankoffset, 0x4000 + bankoffset + known[whichgame].the2ndsize, banktouse);
#endif
                        for (i = 0; i < known[whichgame].the2ndsize; i++)
                        {   serialize_byte(&q_bank[banktouse][bankoffset + i]);
                    }   }
                    else
                    {
#ifdef VERBOSE
                        zprintf(TEXTPEN_VERBOSE, "2nd chunk: $%04X..$%04X.\n", known[whichgame].the2ndaddr, known[whichgame].the2ndaddr + known[whichgame].the2ndsize);
#endif
                        for (i = 0; i < known[whichgame].the2ndsize; i++)
                        {   serialize_byte(&memory[known[whichgame].the2ndaddr + i]);
            }   }   }   }

            memory[0xEE8] = 0x05;
            memory[0xEE9] = 0xFA;
            memory[0xEEA] = 0x05;
            memory[0xEEB] = 0xE6;
            memory[0xEEC] = 0x05;
            memory[0xEED] = 0xE0;
            memory[0xEEE] = 0xFE;
            memory[0xEEF] = 0x20;
            memory[0xEF2] = 0x1F;
            memory[0xEF3] = 0x3F;
            memory[0xEF4] = 0x10;
            memory[0xEF5] = 0x41;
            memory[0xEF7] = banked;
            memory[0xF01] = 0x0D; // CR
            // we call games as subroutines so they can return to the PHUNSY CLI
            // afterwards (eg. for COPY, although it still doesn't really work).
            ras[0]        = 0x23E;
            psu           = 1; // ie. SP = 1;

            ubanktoram((banked & 0xF0) >> 4);
            qbanktoram( banked & 0x0F      );
        }
        engine_reset();
        if (machine == PHUNSY && whichgame != -1 && known[whichgame].complang == CL_MWB)
        {   thequeue[queuepos++] = 'O';
            thequeue[queuepos++] = 'L';
            thequeue[queuepos++] = 'D';
            thequeue[queuepos++] = CR;
            thequeue[queuepos++] = 'R';
            thequeue[queuepos++] = 'U';
            thequeue[queuepos++] = 'N';
            thequeue[queuepos++] = CR;
        }
    acase KIND_TVC:
        // assert(machine == ELEKTOR);
        loadaddress = ReadWordAt(1);
        offset = 5;
        for (i = 0; i < filesize - 5; i++)
        {   if (!(memflags[loadaddress + i] & NOWRITE))
            {   memory[loadaddress + i] = ReadByte();
            } else
            {   DISCARD ReadByte();
        }   }
        engine_reset();

        // memory[$1FC0..$1FC9] = memory[$177..$180]
        // ie.:
        //  SIZES        = $AA
        //  SPR01COLOURS = $09
        //  SPR23COLOURS = $09
        //  SCORECTRL    = $00
        //  $1FC4        = $00
        //  $1FC5        = $00
        //  BGCOLOUR     = $19
        //  PITCH        = $00
        //  SCORELT      = $AA
        //  SCORERT      = $AA
        if (elektor_biosver == ELEKTOR_HOBBYMODULE)
        {   for (i = 0; i < 10; i++)
            {   memory[0x1FC0 + i] = memory[0x57E + i];
        }   }
        else
        {   for (i = 0; i < 10; i++)
            {   memory[0x1FC0 + i] = memory[0x177 + i];
        }   }

        memory[0x1F00 + PVI_SPRITE0BX] =
        memory[0x1F00 + PVI_SPRITE0BY] =
        memory[0x1F00 + PVI_SPRITE1BX] =
        memory[0x1F00 + PVI_SPRITE1BY] =
        memory[0x1F00 + PVI_SPRITE2BX] =
        memory[0x1F00 + PVI_SPRITE2BY] =
        memory[0x1F00 + PVI_SPRITE3BX] =
        memory[0x1F00 + PVI_SPRITE3BY] = 0xFE; // helps with Figure27.tvc

        // make $8B9..$8BA = $0903 (important!)
        memory[0x8B9] = 0x09;
        memory[0x8BA] = 0x03;
        memory[0x8BD] = 0x1F; // BCTA,UN
        memory[0x8BE] = startaddr_h;
        memory[0x8BF] = startaddr_l;
        iar  = 0x8BD;
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Setting IAR to $%X.\n", iar);
#endif
    acase KIND_AOF:
        if (load_aof(filesize, FALSE) == 0)
        {   game = FALSE;
            sprintf((char*) tempstring, LLL(MSG_CORRUPTFILE, "Corrupt %s file!"), "AOF");
            say((STRPTR) tempstring);
            return 0;
        }
        engine_reset();

        if
        (   whichgame == BIORHYTHM300POS
         || whichgame == PIPBUG_LIFEMCPOS
        )
        {   // assert(machine == PIPBUG);
            thequeue[queuepos++] = CR;
            // doesn't need any adjustment to tt_kybdtill
        } elif (whichgame != -1)
        {   switch (known[whichgame].complang)
            {
            case CL_MWB:
                if (machine == PIPBUG || machine == BINBUG)
                {   for (i = 0; i < 13; i++)
                    {   thequeue[queuepos++] = NC;
                    }
                    thequeue[queuepos++] = 'O';
                    thequeue[queuepos++] = CR;
                    for (i = 0; i < ((machine == PIPBUG) ? 13 : 30); i++)
                    {   thequeue[queuepos++] = NC;
                    }
                    thequeue[queuepos++] = 'R';
                    thequeue[queuepos++] = 'U';
                    thequeue[queuepos++] = 'N';
                 // thequeue[queuepos++] = CR;
                }
            acase CL_MB:
                if (machine == PIPBUG || machine == BINBUG)
                {   thequeue[queuepos++] = 0xFF;
                    thequeue[queuepos++] = 0xFF;
                    thequeue[queuepos++] = 'G';
                    thequeue[queuepos++] = '1';
                 // thequeue[queuepos++] = CR;
                    tt_kybdtill = cycles_2650 + ONE_MILLION;
                }
            acase CL_TCT:
                if (machine == PIPBUG || machine == BINBUG)
                {   thequeue[queuepos++] = 'R';
                    thequeue[queuepos++] = 'U';
                    thequeue[queuepos++] = 'N';
                 // thequeue[queuepos++] = CR;
                    tt_kybdtill = cycles_2650 + ONE_MILLION;
                }
            acase CL_8KB13:
                if (machine == CD2650)
                {   thequeue[queuepos++] = '!';  // to give it enough time (' ' and $FF don't work here)
                    thequeue[queuepos++] = 'R';
                    thequeue[queuepos++] = 0xFF; // to give it enough time
                    thequeue[queuepos++] = CR;
                    thequeue[queuepos++] = LF;
                    thequeue[queuepos++] = 0xFF; // to give it enough time
                    thequeue[queuepos++] = 'R';
        }   }   }
    acase KIND_COS:
    case  KIND_COR:
        serialize_cos();
        if (machine == PHUNSY)
        {   ubanktoram((banked & 0xF0) >> 4);
            qbanktoram( banked & 0x0F      );
        }
        calc_margins(); // because region might have changed
        draw_margins();
        sound_reset();
        clearcoverage();
        spriteflip         =
        udcflip            = 0;
        reset_fps();
    }

    if (kind == KIND_COR)
    {   recmode = RECMODE_PLAY;
        recsize = filesize;
        ghost_dips(FALSE);
        if (generate)
        {   macro_start();
    }   }

    generate_autotext();
    updatemenus();
    updatesmlgads();

#ifdef WIN32
    if (cheevos && kind == KIND_BIN || kind == KIND_TVC || kind == KIND_COS || kind == KIND_COR)
    {   if (machine == ELEKTOR)
        {   RA_OnLoadNewRom( cheevomem      , cheevosize);
        } elif (machine == INTERTON || cheevosize <= 4096)
        {   RA_OnLoadNewRom( memory         , cheevosize);
        } else
        {   // assert(machine == ARCADIA);
            memcpy(         &cheevomem[   0], &memory[          0],               4096);
            if (cheevosize <= 8192)
            {   memcpy(     &cheevomem[4096], &memory[     0x2000],  cheevosize - 4096);
            } else
            {   memcpy(     &cheevomem[4096], &memory[     0x2000],               4096);
                memcpy(     &cheevomem[8192], &memory[     0x4000], (cheevosize - 8192) <= 4096 ? (cheevosize - 8192) : 4096);
            }
#ifdef LOGCHEEVOS
            zprintf(TEXTPEN_VERBOSE, "2: RA_OnLoadNewRom(cheevomem, %d)\n", cheevosize);
#endif
            RA_OnLoadNewRom( cheevomem, cheevosize);
        }
        if (kind == KIND_COS || kind == KIND_COR)
        {
#ifdef LOGCHEEVOS
            zprintf(TEXTPEN_VERBOSE, "RA_OnLoadState(%s)\n", fn_game);
#endif
            RA_OnLoadState(fn_game);
    }   }
#endif

    patchrom(); // do this after RetroAchievements (so our patches don't mess up their hashing)

    if (kind == KIND_COR)
    {   return 3;
    } else
    {   return 1;
}   }

#ifdef WIN32
MODULE void textpen(int colour)
{   if (!colourlog || textcolour == colour)
    {   return;
    }

    textcolour = colour;
}
#endif

EXPORT void reset_fps(void)
{   newtime    =
    waittill   =
    oldfpstime = thetime();
    oldfpscyc  = cycles_2650;
    badframes  = 0;

    if
    (   !MainWindowPtr
     || !showstatusbars[wsm]
#ifdef WIN32
     || !hStatusBar
#endif
    )
    {   return;
    }

#ifdef AMIGA
    SetGadgetAttrs(gadgets[GID_MA_ST2], MainWindowPtr, NULL, STRINGA_TextVal, "-", TAG_DONE); // this autorefreshes
#endif
#ifdef WIN32
    SendMessage(hStatusBar, SB_SETTEXT, 3, (LPARAM) "-");
#endif
}

MODULE int identify(STRPTR passedname)
{   FAST int i,
             length1,
             length2;

    length1 = strlen((const char*) passedname);
    for (i = 0; i < KINDS; i++)
    {   length2 = strlen(filekind[i].extension);
        if (length1 >= length2 && !stricmp(&passedname[length1 - length2], filekind[i].extension))
        {   return i;
        }
        if (filekind[i].extension2)
        {   length2 = strlen(filekind[i].extension2);
            if (length1 >= length2 && !stricmp(&passedname[length1 - length2], filekind[i].extension2))
            {   return i;
    }   }   }

    return KIND_NIL;
}

EXPORT void sound_reset(void)
{   if ((machine == ARCADIA || machine == INTERTON || memmap == MEMMAP_F) && (memory[IE_NOISE] & 0x10))
    {   bangercharging = TRUE;
        bangfrom       = cycles_2650; // we should really load and save these variables in COS and COR files
    } else
    {   bangercharging = FALSE;
    }
    speakoffset = 0;
#ifdef AMIGA
    throb           = 0;
#endif
    squeal          = 0;
}

MODULE void write_log_string(STRPTR string)
{   if (LogfileHandle)
    {   datestamp2();
        if (logfile == LOGFILE_REPLACE)
        {   sprintf
            (   string,
                LLL(MSG_LOGREPLACED, "Opened %s for writing at %s.\n\n"), // we should say "reopened" instead of "opened" if it already exists
                LOGFILENAME,
                datetimestring
            );
        } else
        {   sprintf
            (   string,
                LLL(MSG_LOGOPENED, "Opened %s for appending at %s.\n\n"), // we should say "reopened" instead of "opened" if it already exists
                LOGFILENAME,
                datetimestring
            );
    }   }
    else
    {   sprintf
        (   string,
            LLL(MSG_CANTOPENLOG, "Can't open %s for writing!\n\n"),
            LOGFILENAME
        );
}   }
