// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include <proto/graphics.h>
    #include <proto/locale.h> // for GetCatalogStr()

    #include "amiga.h"
    #include <string.h>
    #include <stdlib.h> // eg. for rand()
    #include <math.h>
#endif
#ifdef WIN32
    #include "ibm.h"
    #include "resource.h"
    #define EXEC_TYPES_H
#endif

#ifndef PI
    #define PI   3.14159265358979323846
#endif
#ifndef PI_2
    #define PI_2 1.5707963267948966
#endif

#include <stdio.h>

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"

#include "pipbug.h"

// DEFINES----------------------------------------------------------------

// #define CONTROLYAW
// whether to use magnetic X slider to instead control plane yaw
// (for testing purposes)

// #define VERBOSEINDUSTRIAL
// whether to show info about what is happening with the industrial/
// scientific peripherals

#define TILT_RAD        ((-9.6) * PI / 180.0)
#define POLESTRENGTH  600 // magnetic field strength at poles (in milligauss)

#define PERFORATEEUY
// whether to show perforations between "pages" (currently every 32 rows)
#define PERFORATIONCOLOUR BLACK

// #define NUMERICDEMO
// whether to show numbered rows as EUY demo (otherwise quick brown fox)

#define USECLE
// whether to use Command Line Extensions, etc.

// #define COLOURPIPBUG
// whether to have different colours for different characters like
// old versions of Ami/WinPIPBUG did

// Define these to notice VHSDOS calls when running MicroDOS, and vice versa.
#define LOGVHSDOS      if (binbug_dosver != DOS_NOBINBUGDOS)
#define LOGMICRODOS    if (binbug_dosver != DOS_NOBINBUGDOS)
// Define these to only notice calls for the DOS that is actually running.
// #define LOGVHSDOS   if (binbug_dosver == DOS_VHSDOS)
// #define LOGMICRODOS if (binbug_dosver == DOS_MICRODOS)

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       UBYTE                pitchangle,
                                  rotorspeed,
                                  windspeed,
                                  winddirection,
                                  yawsensordir,
                                  sensormode,
                                  tt_kybdcode,
                                  tt_scrncode;
EXPORT       UWORD                linearx,
                                  lineary,
                                  linearu,
                                  linearv,
                                  magneticx,
                                  magneticy,
                                  magneticz,
                                  planepitch,
                                  planeroll;
EXPORT       ULONG                cpb,
                                  tt_kybdtill,
                                  tt_scrntill;
EXPORT       int                  fastpipbug   = FALSE,
                                  blink        = TRUE,
                                  pipbug_vdu   = VDU_LCVDU_WIDE,
                                  pipbug_charwidth,
                                  pipbug_charheight,
                                  pipbug_charwidth2,
                                  pipbug_charheight2,
                                  pipbug_periph = PERIPH_PRINTER,
                                  printerwidth_full,
                                  printerwidth_view,
                                  printerheight_full,
                                  printerheight_view,
                                  printerrows_full,
                                  vdu_columns,
                                  vdu_rows_total,
                                  vdu_rows_visible,
                                  vdu_scrolly,
                                  vdu_x, vdu_y;
EXPORT       double               mechpower_kw,
                                  elecpower_kw;

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       FLAG                 ignore_cout,
                                  inframe,
                                  paperreaderenabled,
                                  paperpunchenabled;
IMPORT       UBYTE                blank,
                                  cosversion,
                                  digitleds[DIGITLEDS],
                                  jx[2], jy[2],
                                  opcode,
                                  memory[32768],
                                  psu,
                                  r[7],
                                  startaddr_h, startaddr_l,
                                  sx[2], sy[2],
                                  tt_scrncode,
                                  vdu[MAX_VDUCOLUMNS][MAX_VDUROWS],
                                  vdu_fgc, vdu_bgc;
IMPORT       ASCREEN              screen[BOXWIDTH][BOXHEIGHT];
IMPORT       UWORD                iar,
                                  guestpitch[GUESTCHANNELS],
                                  mirror_r[32768],
                                  mirror_w[32768],
                                  ras[8];
IMPORT       ULONG                cycles_2650,
                                  declinate,
                                  eachpage,
                                  frames,
                                  narrowprinter,
                                  oldcycles,
                                  pending,
                                  samplewhere,
                                  sound,
                                  swapped;
IMPORT       int                  absxmin, absymin,
                                  ambient,
                                  belling[2],
                                  binbug_biosver,
                                  binbug_dosver,
                                  colourset,
                                  drawmode,
                                  drawunlit,
                                  frameskip,
                                  game,
                                  interrupt_2650,
                                  machine,
                                  memmap,
                                  offset,
                                  pipbug_baudrate,
                                  pipbug_biosver,
                                  post,
                                  prtscrollx, prtscrolly,
                                  prtunit,
                                  queuepos,
                                  recmode,
                                  serializemode,
                                  showleds,
                                  size,
                                  slice_2650,
                                  tt_kybdstate,
                                  tt_scrn,
                                  usemargins,
                                  usespeech,
                                  whichgame,
                                  whichkeyrect;
IMPORT       struct DriveStruct   drive[DRIVES_MAX];
IMPORT       struct IOPortStruct  ioport[258];
IMPORT       struct KindStruct    filekind[KINDS];
IMPORT       struct MachineStruct machines[MACHINES];
IMPORT       struct PrinterStruct printer[2];
IMPORT const struct KnownStruct   known[KNOWNGAMES];
IMPORT const struct CodeCommentStruct codecomment[];
IMPORT       TEXT                 asciiname_long[259][9 + 1],
                                  asciiname_short[259][3 + 1],
                                  fn_game[MAX_PATH + 1],
                                  fn_project[MAX_PATH + 1], // complete pathname of the ASCII file
                                  gtempstring[256 + 1],
                                  path_projects[MAX_PATH + 1],
                                  ProjectBuffer[PROJECTSIZE],
                                  thequeue[QUEUESIZE];
IMPORT const TEXT                 led_chars[128 + 1];
IMPORT       RECT                 therect;
IMPORT       MEMFLAG              memflags[ALLTOKENS];
#ifdef AMIGA
    IMPORT   struct Catalog*      CatalogPtr;
    IMPORT   struct Gadget*       gadgets[GIDS + 1];
    IMPORT   struct Window*       SubWindowPtr[SUBWINDOWS];
    IMPORT   struct PaletteStruct pencolours[COLOURSETS][PENS];
    IMPORT   struct RastPort      wpa8canvasrastport[CANVASES];
    IMPORT   UBYTE                bytepens[PENS];
    IMPORT   LONG                 emupens[EMUBRUSHES];
    IMPORT   UBYTE               *canvasdisplay[CANVASES],
                                 *canvasbyteptr[CANVASES][CANVASHEIGHT];
#endif
#ifdef WIN32
    IMPORT   int                  CatalogPtr; // APTR doesn't work
    IMPORT   HWND                 SubWindowPtr[SUBWINDOWS];
    IMPORT   ULONG                pencolours[COLOURSETS][PENS];
    IMPORT   ULONG*               canvasdisplay[CANVASES];
IMPORT       struct
{   BITMAPINFOHEADER Header;
    DWORD            Colours[3];
} CanvasBitMapInfo[CANVASES];
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE       TEXT                 dosname[14 + 1];
MODULE       int                  printercolumns;
MODULE       double               earthx,
                                  earthy,
                                  earthz,
                                  planex,
                                  planey,
                                  planez,
                                  planepitch_deg,
                                  planepitch_rad,
                                  planeroll_deg,
                                  planeroll_rad,
                                  planeyaw_deg,
                                  planeyaw_rad;

/* MODULE STRUCTURES------------------------------------------------------

(none)

MODULE FUNCTIONS------------------------------------------------------- */

#ifdef COLOURPIPBUG
MODULE int pipbug_getcolour(       UBYTE whichchar);
#else
MODULE int pipbug_getcolour(UNUSED UBYTE whichchar);
#endif
MODULE void getdosname(void);
MODULE void update_wind(void);
MODULE void draw_line(int x1, int y1, int x2, int y2, ULONG colour);
MODULE void rotate_vector(void);

// CODE-------------------------------------------------------------------

EXPORT void pipbug_setmemmap(void)
{   int address,
        i,
        mirror;

    game = FALSE;
    machines[PIPBUG].cpf = fastpipbug ? 40000.0 : 20000.0;
    pipbug_changebios(FALSE);

    switch (memmap)
    {
    case MEMMAP_ARTEMIS:
        memset(&memory[0x1000], 0, 0x7000); // $1000..$7FFF
    acase MEMMAP_PIPBUG1:
        memset(&memory[0x400], 0, 0x7C00); // $400..$7FFF
#ifdef USECLE
        for (i = 0; i < 561; i++)
        {   memory[0x3000 + i] = pipbug_cle[i];
        }
        for (i = 0; i < (int) P_EPROMSIZE; i++)
        {   memory[0x3C00 + i] = pipbug_eprom[i];
        }
        memory[0x59] = 0x30; // these are
        memory[0x5A] = 0x00; // for CLE
#endif
    acase MEMMAP_PIPBUG2:
        memset(&memory[0x800], 0, 0x7800); // $800..$7FFF
    }

    for (address = 0; address < 32 * KILOBYTE; address++)
    {   memflags[address] = 0;
        mirror            = address;

        switch (memmap)
        {
        case MEMMAP_PIPBUG1:
            if    (address <=  0x3FF                     ) memflags[address] |= NOWRITE | BIOS;
            elif  (address >= 0x3000 && address <= 0x41FF) memflags[address] |= NOWRITE;
         // elif  (address >= 0x5800 && address <= 0x6CFF) memflags[address] |= NOREAD | NOWRITE;
            // Linearisatie expects RAM at $6D00..$7FFF
        acase MEMMAP_PIPBUG2:
            if    (address <=  0x7FF                     ) memflags[address] |= NOWRITE | BIOS; // we consider PIPLA to be part of the BIOS
            elif ((address >=  0x880 && address <=  0xBFF)
               || (address >= 0x1000 && address <= 0x1EFF)
               ||  address >= 0x2000                     ) memflags[address] |= NOWRITE | NOREAD;
            if (address >= 0x1F00 && address <= 0x1FFF)
            {   mirror -= 0x1000; // $1Fxx -> $0Fxx
            }
        acase MEMMAP_ARTEMIS:
            if    (address <=  0xFFF                     ) memflags[address] |= NOWRITE | BIOS;
        }

        mirror_r[address] =
        mirror_w[address] = (UWORD) mirror;
    }

    for (i = machines[machine].firstcodecomment; i <= machines[machine].lastcodecomment; i++)
    {   memflags[codecomment[i].address] |= COMMENTED;
    }

    for (i = 0; i < 13; i++)
    {   digitleds[i] = 0xF;
    }

    blank = PIPBUG_BLANK;
}

EXPORT void pipbug_emulate(void)
{   FAST int   x, xx, xlimit,
               y, yy, ylimit;
    FAST UBYTE firstbit,
               t,
               tempfgc,
               tempvdu;

    inframe = TRUE;

    digitleds[0] =
    digitleds[1] =
    digitleds[2] =
    digitleds[3] = 0; // 0..3 are transient
    // 4..12 are persistent

    pipbin_readtty();

    if (fastpipbug)
    {   // 2MHz = 2,000,000 clocks per second
        // 2,000,000 / 50 = 40,000
        slice_2650 += 40000;
    } else
    {   // 1MHz = 1,000,000 clocks per second
        // 1,000,000 / 50 = 20,000
        slice_2650 += 20000;
    }
    pipbin_runcpu();

    if (frameskip != FRAMESKIP_INFINITE && frames % (ULONG) frameskip == 0)
    {   xlimit = machines[machine].width;
        ylimit = machines[machine].height - (showleds ? 43 : 0);
        if (usemargins && pipbug_vdu == VDU_RADIOBULLETIN)
        {   xlimit -= 168; // 518 -> 350
            ylimit -=  57; // 313 -> 256
        }
        for (x = absxmin; x < absxmin + xlimit; x++)
        {   for (y = absymin; y < absymin + ylimit; y++)
            {   changebgpixel(x, y, vdu_bgc);
        }   }

        for (x = 0; x < vdu_columns; x++)
        {   for (y = 0; y < vdu_rows_visible; y++)
            {   if (pipbug_vdu == VDU_ELEKTERMINAL)
                {   tempvdu = vdu[x][(vdu_scrolly + y) % vdu_rows_total];
                } else
                {   tempvdu = vdu[x][y];
                }
                tempfgc = pipbug_getcolour(tempvdu);
                if (tempvdu == 0x80)
                {   tempvdu = 0x20; // so screen doesn't get covered with crap
                }

                for (yy = 0; yy < pipbug_charheight2; yy++)
                {   if   (pipbug_vdu == VDU_ELEKTERMINAL ) t =  elekterminal_chars[tempvdu & 0x7F][yy];
                    elif (pipbug_vdu == VDU_SVT100       ) t =        svt100_chars[tempvdu & 0x7F][yy];
                    elif (pipbug_vdu == VDU_VT100        ) t =         vt100_chars[tempvdu & 0x7F][yy];
                    elif (pipbug_vdu == VDU_RADIOBULLETIN) t = radiobulletin_chars[tempvdu & 0x7F][yy];
                    else                                   t =         lcvdu_chars[tempvdu & 0x7F][yy];

                    if
                    (   vdu_x == x
                     && (   (pipbug_vdu == VDU_ELEKTERMINAL && vdu_y - vdu_scrolly == y)
                         || (pipbug_vdu != VDU_ELEKTERMINAL && vdu_y               == y)
                    )   )
                    {   if (pipbug_vdu == VDU_RADIOBULLETIN)
                        {   t = 0x1F; // solid white block
                        } elif (!blink)
                        {   if (pipbug_vdu == VDU_VT100)
                            {   if (yy == 9) t = 0xFE; // white underline
                            } else
                            {   t = 0x1F; // solid grey block
                                tempfgc = GREY5;
                        }   }
                        elif
                        (   (pipbug_vdu == VDU_SVT100 && frames % 25 >= 12) // ~1/4 of a second inverse, ~1/4rd of a second underlined (2 Hz rate)
                         || (pipbug_vdu == VDU_VT100  && frames % 50 >= 25) // ~1/2 of a second inverse, ~1/2rd of a second normal
                         || (   (   pipbug_vdu == VDU_ELEKTERMINAL
                                 || pipbug_vdu == VDU_LCVDU_NARROW
                                 || pipbug_vdu == VDU_LCVDU_WIDE
                                )                     && frames % 32 >= 16) // ~1/3 of a second inverse, ~1/3rd of a second normal (matches CP1002)
                        )
                        {   if (pipbug_vdu == VDU_SVT100 && yy == 7)
                            {   t = 0x1F; // underline
                            } else
                            {   t = 0; // empty
                        }   }
                        else
                        {   t = (pipbug_vdu == VDU_VT100) ? 0xFE : 0x1F; // solid white block
                    }   }
                    elif (tempvdu < 0x20)
                    {   t = ~t;
                    } elif (tempvdu >= 0x80)
                    {   if (!blink)
                        {   if (pipbug_vdu != VDU_VT100)
                            {   tempfgc = GREY5;
                        }   }
                        elif
                        (   (pipbug_vdu == VDU_SVT100 && frames % 25 >= 12) // ~1/4 of a second inverse, ~1/4rd of a second underlined (2 Hz rate)
                         || (pipbug_vdu == VDU_VT100  && frames % 50 >= 25) // ~1/2 of a second inverse, ~1/2rd of a second normal
                         || (   (   pipbug_vdu == VDU_ELEKTERMINAL
                                 || pipbug_vdu == VDU_LCVDU_NARROW
                                 || pipbug_vdu == VDU_LCVDU_WIDE
                                 || pipbug_vdu == VDU_RADIOBULLETIN         // real RBVDU never flashes the cursor
                                 )                    && frames % 32 >= 16) // ~1/3 of a second inverse, ~1/3rd of a second normal (matches CP1002)
                        )
                        {   t = 0; // empty
                    }   }

                    firstbit = 0x01 << (pipbug_charwidth2 - 1);
                    for (xx = 0; xx < pipbug_charwidth2; xx++)
                    {   if ((t & (firstbit >> xx)) && (x * pipbug_charwidth) + xx < machines[PIPBUG].width)
                        {   changepixel
                            (   (x * pipbug_charwidth ) + xx + ((pipbug_vdu == VDU_ELEKTERMINAL) ? 2 : 0) + absxmin,
                                (y * pipbug_charheight) + yy + ((pipbug_vdu == VDU_ELEKTERMINAL) ? 2 : 0) + absymin,
                                tempfgc
                            );
        }   }   }   }   }

        pipbug_redrawleds();
    }

    update_wind();

    if (drawmode)
    {   pipbug_drawhelpgrid();
    }
    wa_checkinput();
    endofframe(vdu_bgc);
}

EXPORT void pipbin_runcpu(void)
{   FAST ULONG endcycle;

    // assert(slice_2650 >= 1);

    endcycle = cycles_2650 + slice_2650;
    if (endcycle < cycles_2650)
    {   // cycle counter will overflow, so we need to use the slow method
        while (slice_2650 >= 1)
        {   oldcycles = cycles_2650;
            checkstep();
            pipbin_io();
            one_instruction();
            slice_2650 -= (cycles_2650 - oldcycles);
    }   }
    else
    {   // cycle counter will not overflow, so we can use a faster method
        oldcycles = cycles_2650;
        while (cycles_2650 < endcycle)
        {   checkstep();
            pipbin_io();
            one_instruction();
        }
        slice_2650 -= (cycles_2650 - oldcycles);
}   }

EXPORT void pipbin_io(void)
{   // assert(machine == PIPBUG || machine == BINBUG);

    do_tape();

    if (tt_kybdstate && cycles_2650 >= tt_kybdtill)
    {   /* tt_kybdstate is:
             0:     marking time (idle)
             1:     start bit (0)
             2.. 9: payload
            10..11: stop bits (1)
            12+:    marking time (busy) */

        tt_kybdtill += cpb;
        tt_kybdstate++;
        switch (tt_kybdstate)
        {
        case   2: if (tt_kybdcode &   1) psu |= PSU_S; else psu &= ~(PSU_S);
        acase  3: if (tt_kybdcode &   2) psu |= PSU_S; else psu &= ~(PSU_S);
        acase  4: if (tt_kybdcode &   4) psu |= PSU_S; else psu &= ~(PSU_S);
        acase  5: if (tt_kybdcode &   8) psu |= PSU_S; else psu &= ~(PSU_S);
        acase  6: if (tt_kybdcode &  16) psu |= PSU_S; else psu &= ~(PSU_S);
        acase  7: if (tt_kybdcode &  32) psu |= PSU_S; else psu &= ~(PSU_S);
        acase  8: if (tt_kybdcode &  64) psu |= PSU_S; else psu &= ~(PSU_S);
        acase  9: if (tt_kybdcode & 128) psu |= PSU_S; else psu &= ~(PSU_S);
        acase 10:                        psu |= PSU_S; // stop bit
        acase 11:                        psu |= PSU_S; // stop bit
        acase 23: tt_kybdstate = 0;      psu |= PSU_S; // any less will cause problems for fast typists
    }   }

    if (tt_scrn && cycles_2650 >= tt_scrntill)
    {   // assert(machine == PIPBUG);

        tt_scrntill += cpb;
        tt_scrn++;
        switch (tt_scrn)
        {
        case   2: if (psu & PSU_F) tt_scrncode |=   1;
        acase  3: if (psu & PSU_F) tt_scrncode |=   2;
        acase  4: if (psu & PSU_F) tt_scrncode |=   4;
        acase  5: if (psu & PSU_F) tt_scrncode |=   8;
        acase  6: if (psu & PSU_F) tt_scrncode |=  16;
        acase  7: if (psu & PSU_F) tt_scrncode |=  32;
        acase  8: if (psu & PSU_F) tt_scrncode |=  64;
        acase  9: if (psu & PSU_F) tt_scrncode |= 128;
                  thecout(tt_scrncode);
                  tt_scrn = 0;
}   }   }

EXPORT float pipbug_getpitch(UWORD value)
{   PERSIST const float pipbug_pitch[] = {
    (const float)   82.40, // $30:   vlow E
    (const float)   87.30, // $31:   vlow F
    (const float)   92.49, // $32:   vlow F#
    (const float)   98.00, // $33:   vlow G
    (const float)  103.82, // $34:   vlow G#
    (const float)  110.00, // $35:   vlow A
    (const float)  116.54, // $36:   vlow A#
    (const float)  123.47, // $37:   vlow B
    (const float)  130.81, // $38:    low C
    (const float)  138.59, // $39:    low C#
    (const float)  146.83, // $3A:    low D
    (const float)  155.56, // $3B:    low D#
    (const float)    0.00, // $3C
    (const float)    0.00, // $3D
    (const float)    0.00, // $3E
    (const float)    0.00, // $3F
    (const float)  164.81, // $40:    low E
    (const float)  174.61, // $41:    low F
    (const float)  184.99, // $42:    low F#
    (const float)  196.00, // $43:    low G
    (const float)  207.65, // $44:    low G#
    (const float)  220.00, // $45:    low A
    (const float)  233.08, // $46:    low A#
    (const float)  246.94, // $47:    low B
    (const float)  261.63, // $48: middle C
    (const float)  277.18, // $49: middle C#
    (const float)  293.66, // $4A: middle D
    (const float)  311.13, // $4B: middle D#
    (const float)    0.00, // $4C
    (const float)    0.00, // $4D
    (const float)    0.00, // $4E
    (const float)    0.00, // $4F
    (const float)  329.63, // $50: middle E
    (const float)  349.23, // $51: middle F
    (const float)  369.99, // $52: middle F#
    (const float)  392.00, // $53: middle G
    (const float)  415.30, // $54: middle G#
    (const float)  440.00, // $55: middle A
    (const float)  466.16, // $56: middle A#
    (const float)  493.88, // $57: middle B
    (const float)  523.25, // $58:   high C
    (const float)  554.37, // $59:   high C#
    (const float)  587.33, // $5A:   high D
    (const float)  622.25, // $5B:   high D#
    (const float)    0.00, // $5C
    (const float)    0.00, // $5D
    (const float)    0.00, // $5E
    (const float)    0.00, // $5F
    (const float)  659.26, // $60:   high E
    (const float)  698.46, // $61:   high F
    (const float)  739.99, // $62:   high F#
    (const float)  783.99, // $63:   high G
    (const float)  830.61, // $64:   high G#
    (const float)  880.00, // $65:   high A
    (const float)  932.30, // $66:   high A#
    (const float)  987.80, // $67:   high B
    (const float) 1046.50, // $68:  vhigh C
    (const float) 1108.70, // $69:  vhigh C#
    (const float) 1174.70, // $6A:  vhigh D
    (const float) 1244.50, // $6B:  vhigh D#
    (const float)    0.00, // $6C
    (const float)    0.00, // $6D
    (const float)    0.00, // $6E
    (const float)    0.00, // $6F
    (const float) 1318.50, // $70:  vhigh E
    (const float) 1396.90, // $71:  vhigh F
    (const float) 1480.00, // $72:  vhigh F#
    (const float) 1568.00, // $73:  vhigh G
    (const float) 1661.22, // $74:  vhigh G#
    (const float) 1760.00, // $75:  vhigh A
    (const float) 1864.60, // $76:  vhigh A#
    (const float) 1975.60, // $77:  vhigh B
    (const float) 2093.00, // $78:  xhigh C
    (const float) 2217.40, // $79:  xhigh C#
    (const float) 2349.40, // $7A:  xhigh D
    (const float) 2489.00  // $7B:  xhigh D#
};

    if (value < 0x30 || value > 0x7B)
    {   return 0.0;
    } else
    {   return(pipbug_pitch[value - 0x30]);
}   }

EXPORT UBYTE pipbug_readport(int port)
{   FAST UBYTE t;

    // assert(machine == PIPBUG);

    switch (port)
    {
    case 2:
        if (pipbug_periph == PERIPH_FURNACE)
        {   t = ioport[2].contents; // field current
        }
    acase 5:
        if (pipbug_periph == PERIPH_LINEARISATIE)
        {   switch (sensormode)
            {
            case  0x2C: t =  linearx & 0xFF;
            acase 0x2D: t =  lineary & 0xFF;
            acase 0x2E: t =  linearu & 0xFF;
            acase 0x2F: t =  linearv & 0xFF;
        }   }
    acase 6:
        if (pipbug_periph == PERIPH_LINEARISATIE)
        {   switch (sensormode)
            {
            case  0x2C: t = (linearx & 0x0F00) >> 8;
            acase 0x2D: t = (lineary & 0x0F00) >> 8;
            acase 0x2E: t = (linearu & 0x0F00) >> 8;
            acase 0x2F: t = (linearv & 0x0F00) >> 8;
        }   }
    acase 25:
        t = 0x80; // ETI-641 printer is ready
    acase 255:
        if (pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   t = ioport[255].contents;
        } else
        {   t = 0; // guess
        }
    acase PORTC:
        if (pipbug_periph == PERIPH_MAGNETOMETER)
        {   switch (sensormode & 0x0F)
            {
            case  0x01: t = (UBYTE) ((magneticx  & 0x0FF0) >> 4);
#ifdef VERBOSEINDUSTRIAL
                        zprintf(TEXTPEN_VERBOSE, "Read MSB of X.\n");
#endif
            acase 0x02: t = (UBYTE) ((magneticy  & 0x0FF0) >> 4);
#ifdef VERBOSEINDUSTRIAL
                        zprintf(TEXTPEN_VERBOSE, "Read MSB of Y.\n");
#endif
            acase 0x03: t = (UBYTE) ((magneticz  & 0x0FF0) >> 4);
#ifdef VERBOSEINDUSTRIAL
                        zprintf(TEXTPEN_VERBOSE, "Read MSB of Z.\n");
#endif
            acase 0x04: t = (UBYTE) ((planepitch & 0x0FF0) >> 4);
#ifdef VERBOSEINDUSTRIAL
                        zprintf(TEXTPEN_VERBOSE, "Read MSB of plane pitch.\n");
#endif
            acase 0x05: t = (UBYTE) ((planeroll  & 0x0FF0) >> 4);
#ifdef VERBOSEINDUSTRIAL
                        zprintf(TEXTPEN_VERBOSE, "Read MSB of plane roll.\n");
#endif
            adefault:   t = 0;
            }
            t ^= 0xFF; // it is expected to be inverted
        }
    acase PORTD:
        if (pipbug_periph == PERIPH_MAGNETOMETER)
        {   switch (sensormode & 0x0F)
            {
            case  0x01: t = (UBYTE) ((magneticx  & 0x000F) << 4);
#ifdef VERBOSEINDUSTRIAL
                        zprintf(TEXTPEN_VERBOSE, "Read LSN of X.\n");
#endif
            acase 0x02: t = (UBYTE) ((magneticy  & 0x000F) << 4);
#ifdef VERBOSEINDUSTRIAL
                        zprintf(TEXTPEN_VERBOSE, "Read LSN of Y.\n");
#endif
            acase 0x03: t = (UBYTE) ((magneticz  & 0x000F) << 4);
#ifdef VERBOSEINDUSTRIAL
                        zprintf(TEXTPEN_VERBOSE, "Read LSN of Z.\n");
#endif
            acase 0x04: t = (UBYTE) ((planepitch & 0x000F) << 4);
#ifdef VERBOSEINDUSTRIAL
                        zprintf(TEXTPEN_VERBOSE, "Read LSN of plane pitch.\n");
#endif
            acase 0x05: t = (UBYTE) ((planeroll  & 0x000F) << 4);
#ifdef VERBOSEINDUSTRIAL
                        zprintf(TEXTPEN_VERBOSE, "Read LSN of plane roll.\n");
#endif
            adefault:   t = 0;
            }
            t ^= 0xFF; // it is expected to be inverted. And low bit, at least, must be set
        } elif (pipbug_periph == PERIPH_FURNACE)
        {   switch (ioport[PORTC].contents & 0x0F) // Wind Furnace Controller
            {
            case  0:  t = rotorspeed;
#ifdef VERBOSEINDUSTRIAL
                      zprintf(TEXTPEN_VERBOSE, "Read rotor speed of %d.\n", rotorspeed);
#endif
            acase 1:  t = pitchangle;
#ifdef VERBOSEINDUSTRIAL
                      zprintf(TEXTPEN_VERBOSE, "Read blade pitch angle of %d.\n", pitchangle);
#endif
            acase 2:  t = windspeed;
#ifdef VERBOSEINDUSTRIAL
                      zprintf(TEXTPEN_VERBOSE, "Read wind speed of %d.\n", windspeed);
#endif
            acase 3:  t = yawsensordir;  // guess
#ifdef VERBOSEINDUSTRIAL
                      zprintf(TEXTPEN_VERBOSE, "Read yaw sensor direction of %d.\n", yawsensordir);
#endif
            acase 4:  t = winddirection; // guess
#ifdef VERBOSEINDUSTRIAL
                      zprintf(TEXTPEN_VERBOSE, "Read wind direction of %d.\n", winddirection);
#endif
            adefault: t = 0x0;
        }   }
        elif (pipbug_periph == PERIPH_PRINTER)
        {   t = 0x80; // EA printer is ready
        }
        // Radio Bulletin machine (at least) also uses this (not yet implemented)
    adefault:
        t = 0;
    }

    logport(port, t, FALSE);
    return t;
}

EXPORT void pipbug_writeport(int port, UBYTE data)
{   switch (port)
    {
    case 0:
        if (pipbug_periph == PERIPH_FURNACE)
        {   ioport[0].contents = data;
#ifdef VERBOSEINDUSTRIAL
            zprintf(TEXTPEN_VERBOSE, "Wrote $%02X to blade pitch motor speed.\n", data);
#endif
        }
    acase 1:
        if (pipbug_periph == PERIPH_FURNACE)
        {   ioport[1].contents = data;
#ifdef VERBOSEINDUSTRIAL
            zprintf(TEXTPEN_VERBOSE, "Wrote $%02X to hub yaw motor speed.\n", data);
#endif
        } else // if (pipbug_periph == PERIPH_MAGNETOMETER)
        {   // LED MSB (data is inverted)
            drawdigit(4, (UBYTE)  ((255 - data) & 0x0F));
            ioport[1].contents = data;
        }
    acase 2:
        if (pipbug_periph == PERIPH_FURNACE)
        {   ioport[2].contents = data;
#ifdef VERBOSEINDUSTRIAL
            zprintf(TEXTPEN_VERBOSE, "Wrote $%02X to generator field current.\n", data);
#endif
        } else // if (pipbug_periph == PERIPH_MAGNETOMETER)
        {   // LED LSB (data is inverted)
            drawdigit(5, (UBYTE) (((255 - data) & 0xF0) >> 4));
            drawdigit(6, (UBYTE)  ((255 - data) & 0x0F));
            ioport[2].contents = data;
        }
    acase 4:
        switch (pipbug_periph)
        {
        case PERIPH_FURNACE:
            ioport[4].contents = data;
#ifdef VERBOSEINDUSTRIAL
            zprintf(TEXTPEN_VERBOSE, "Wrote $%02X to hub yaw motor mode.\n", data);
#endif
        acase PERIPH_LINEARISATIE:
            if (data == 0x08)
            {   sensormode = 0x08;
                linearu &= 0x0F00;
                linearu |= ioport[5].contents;
            } elif (data == 0x04)
            {   sensormode = 0x04;
                linearv &= 0x0F00;
                linearv |= ioport[5].contents;
            } elif (data == 0x0C)
            {   if (sensormode == 0x08)
                {   linearu &= 0x00FF;
                    linearu |= ((ioport[6].contents & 0x0F) << 8);
                } elif (sensormode == 0x04)
                {   linearv &= 0x00FF;
                    linearv |= ((ioport[6].contents & 0x0F) << 8);
            }   }
            elif (data >= 0x2C && data <= 0x2F)
            {   sensormode = data;
            }
        acase PERIPH_MAGNETOMETER:
            // plane pitch angle (aka theta, ie. an O with a horizontal line through it) - positive for nose up, negative for nose down
            drawdigit(8, (UBYTE) (((255 - data) & 0xF0) >> 4));
            drawdigit(9, (UBYTE)  ((255 - data) & 0x0F));
            ioport[4].contents = data;
        }
    acase 5:
        if (pipbug_periph == PERIPH_LINEARISATIE)
        {   ioport[5].contents = data;
        }
    acase 6:
        if (pipbug_periph == PERIPH_LINEARISATIE)
        {   ioport[6].contents = data & 0x0F;
        }
    acase 8:
        if (pipbug_periph == PERIPH_MAGNETOMETER)
        {   // plane roll angle (aka phi, ie. an O with a vertical line through it) - positive for clockwise (right)?
            drawdigit(11, (UBYTE) (((255 - data) & 0xF0) >> 4));
            drawdigit(12, (UBYTE)  ((255 - data) & 0x0F));
            ioport[8].contents = data;
        }
    acase 16:
        if (pipbug_periph == PERIPH_MAGNETOMETER)
        {   // sign bits
            if (data == 0x03 || data == 0x08)
            {   drawsegment(7, 6, RED); // negative plane pitch
            } else
            {   drawsegment(7, 6, UNLIT); // positive plane pitch
            }
            if (data == 0x00 || data == 0x08)
            {   drawsegment(10, 6, RED); // negative plane roll
            } else
            {   drawsegment(10, 6, UNLIT); // positive plane roll
            }
            ioport[16].contents = data;
        }
    acase 25: // ETI-641 printer
        euy_printchar(data, TRUE);
    acase 255:
        if (pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   ioport[255].contents = data;
        }
    acase PORTC:
#ifdef VERBOSEINDUSTRIAL
        if (pipbug_periph == PERIPH_FURNACE)
        {   switch (data & 0x0F)
            {
            case  0: zprintf(TEXTPEN_VERBOSE, "Requesting rotor speed.\n");
            acase 1: zprintf(TEXTPEN_VERBOSE, "Requesting blade pitch angle.\n");
            acase 2: zprintf(TEXTPEN_VERBOSE, "Requesting wind speed.\n");
            acase 3: zprintf(TEXTPEN_VERBOSE, "Requesting yaw sensor direction.\n");
            acase 4: zprintf(TEXTPEN_VERBOSE, "Requesting wind direction.\n");
        }   }
#endif
        ioport[PORTC].contents = data; // used by Vector Magnetometer. Also, high bit enables papertape reader
    acase PORTD:
        if (pipbug_periph == PERIPH_MAGNETOMETER)
        {   sensormode = data;
        } elif
        (   pipbug_biosver == PIPBUG_PIPBUG2BIOS
         || whichgame == RYTMONPOS // "The baudot routines in this ROM are timed for 45.45-baud and are output via bit 7 of port D. The program allows simultaneous printing to the VDU screen via the normal flag."
        )
        {   ;
        } else
        {   if (pipbug_periph == PERIPH_PRINTER)
            {   euy_printchar(data, FALSE); // EA printer
            }

            if (data & 0x80) drawdigit(0, (UBYTE) (data & 0x0F));
            if (data & 0x40) drawdigit(1, (UBYTE) (data & 0x0F));
            if (data & 0x20) drawdigit(2, (UBYTE) (data & 0x0F));
            if (data & 0x10) drawdigit(3, (UBYTE) (data & 0x0F));

            // Radio Bulletin machine (at least) uses bit 0 (at least) to control whether the LEDs update (=1) or are frozen (=0) (not yet implemented)
    }   }

    logport(port, data, TRUE);
}

EXPORT void pipbug_viewscreen(void)
{   TEXT rowchars[16 + 1];
    int  x, xx, y;

    if (machine != PIPBUG)
    {   return;
    }

    rowchars[16] = EOS;

    for (y = 0; y < vdu_rows_total; y++)
    {   for (xx = 0; xx < vdu_columns; xx += 16)
        {   for (x = 0; x < 16; x++)
            {   if ((vdu[xx + x][y] & 0x7F) <= 31) rowchars[x] =
#ifdef AMIGA
(TEXT) '·';
#endif
#ifdef WIN32
'.';
#endif
                else rowchars[x] = (vdu[xx + x][y] & 0x7F);
            }

            if (xx == 0)
            {   zprintf
                (   TEXTPEN_VIEW,
                    "%2d: %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X %02X %02X %s\n",
                    y,
                    vdu[ 0][y],
                    vdu[ 1][y],
                    vdu[ 2][y],
                    vdu[ 3][y],
                    vdu[ 4][y],
                    vdu[ 5][y],
                    vdu[ 6][y],
                    vdu[ 7][y],
                    vdu[ 8][y],
                    vdu[ 9][y],
                    vdu[10][y],
                    vdu[11][y],
                    vdu[12][y],
                    vdu[13][y],
                    vdu[14][y],
                    vdu[15][y],
                    rowchars
                );
            } else
            {   zprintf
                (   TEXTPEN_VIEW,
                    "    %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X %02X %02X %s\n",
                    vdu[xx +  0][y],
                    vdu[xx +  1][y],
                    vdu[xx +  2][y],
                    vdu[xx +  3][y],
                    vdu[xx +  4][y],
                    vdu[xx +  5][y],
                    vdu[xx +  6][y],
                    vdu[xx +  7][y],
                    vdu[xx +  8][y],
                    vdu[xx +  9][y],
                    vdu[xx + 10][y],
                    vdu[xx + 11][y],
                    vdu[xx + 12][y],
                    vdu[xx + 13][y],
                    vdu[xx + 14][y],
                    vdu[xx + 15][y],
                    rowchars
                );
        }   }
        zprintf(TEXTPEN_VIEW, "\n");
    }

    zprintf
    (   TEXTPEN_VIEW,
        "%02X %02X %02X %02X  %02X %02X %02X  %c%c%c%c%c%c%c%c %c%c%c%c%c%c\n\n",
        digitleds[0],
        digitleds[1],
        digitleds[2],
        digitleds[3],
        digitleds[4],
        digitleds[5],
        digitleds[6],
        led_chars[digitleds[0] & 0x7F], (digitleds[0] & 0x80) ? '.' : ' ',
        led_chars[digitleds[1] & 0x7F], (digitleds[1] & 0x80) ? '.' : ' ',
        led_chars[digitleds[2] & 0x7F], (digitleds[2] & 0x80) ? '.' : ' ',
        led_chars[digitleds[3] & 0x7F], (digitleds[3] & 0x80) ? '.' : ' ',
        led_chars[digitleds[4] & 0x7F], (digitleds[4] & 0x80) ? '.' : ' ',
        led_chars[digitleds[5] & 0x7F], (digitleds[5] & 0x80) ? '.' : ' ',
        led_chars[digitleds[6] & 0x7F], (digitleds[6] & 0x80) ? '.' : ' '
    );
}

EXPORT void pipbin_biosdetails(int ea)
{   // assert(machine == PIPBUG || machine == BINBUG);

    if
    (   (machine == PIPBUG                                                               && (ea >= 0x6800 || pipbug_biosver == PIPBUG_HYBUG))
     || (machine == BINBUG && binbug_biosver != BINBUG_36 && binbug_biosver != BINBUG_61 &&  ea <   0x400                                   )
    )
    {   return;
    }

    switch (ea)
    {
    case 0x17: // INCRT
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf(TEXTPEN_LOG, ";Subr that increments TEMP - must precede STRT\n");
        }
    acase 0x1D: // EBUG
        if (machine != PIPBUG || pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Error command handler\n" \
                ";Input register:     None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0\n" \
                ";Subroutines called: COUT, CRLF, LINE, ALTE, BKPT, CLR, DUMP, GOTO, LOAD, SREG\n" \
                ";BIOS:               PIPBUG and BINBUG\n"
            );
            if (machine == BINBUG && binbug_biosver == BINBUG_61)
            {   zprintf
                (   TEXTPEN_LOG,
                    ";Notes:\n" \
                    ";Input line and jump to routine depending on first character of command.\n" \
                    ";DOS commands W and K are also supported. Final branch is indirectly\n" \
                    ";through ACON in RAM which may be altered to extend command table.\n"
                );
        }   }
    acase 0x1F: // MBUG
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Main command handler\n" \
                ";Input register:     None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0\n" \
                ";Subroutines called: COUT, CRLF, LINE, ALTE, BKPT, CLR, DUMP, GOTO, LOAD, SREG\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x21: // STRT
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf(TEXTPEN_LOG, ";Subr that stores double precision (R1,R2) into TEMP");
        }
    acase 0x26: // LKUP
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Function:           Subr to verify and translate hex character to bin\n" \
                ";Input register:     ASCII hex character in R0\n" \
                ";                    An illegal char causes return to PIPBUG\n" \
                ";Return code:        4-bit binary value in R0, right justified\n" \
                ";Registers used:     R0\n" \
                ";Summary:            Converts the hex character in register 0\n" \
                ";                    into a 4-bit binary value\n" \
            );
        }
    acase 0x34: // ABRT
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";This routine will abort the current cmd\n" \
                ";The stack is reset to the user prog level\n"
            );
        }
    acase 0x38: // EBUG
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf(TEXTPEN_LOG, ";Command handler\n");
        }
    acase 0x5B: // LINE
        if (machine != PIPBUG || pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Input a command line into buffer\n" \
                ";Function:           Inputs up to 20 characters from the keyboard into the line buffer\n" \
                ";Comments:           CODE is 1=CR, 2=LF, 3=message+CR, 4=message+LF\n" \
                ";                    DELete is used for entry corrections\n" \
                ";                    CR or LF terminates the routine\n" \
                ";Input register:     None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0, R1, R3\n" \
                ";Subroutines called: CHIN, COUT (, CRLF)\n" \
                ";Nesting depth:      3\n" \
                ";BIOS:               PIPBUG and BINBUG\n"
            );
            if (machine == BINBUG && binbug_biosver == BINBUG_61)
            {   zprintf
                (   TEXTPEN_LOG,
                    ";Notes:\n" \
                    ";DEL will delete the last character entered and output a BS to erase it\n" \
                    ";on the terminal. CODE indicates type of line entered.\n" \
                    ";CODE = 1 if CR\n" \
                    ";       2 if LF\n" \
                    ";       3 if CR and data\n" \
                    ";       4 if LF and data\n"
                );
        }   }
    acase 0x7E: // HADD
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Function:           Hex 16-bit add\n" \
                ";Input registers:    R1, R2\n" \
                ";Return code:        Gives result in TEMP\n" \
                ";Registers used:     R1, R2, TEMP\n"
            );
        }
    acase 0x8A: // CRLF
        if (machine != PIPBUG || pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Carriage return and line feed\n" \
                ";Function:           Outputs a carriage return and line feed to VDU\n" \
                ";Input register:     None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0\n" \
                ";Subroutines called: COUT\n" \
                ";Nesting depth:      3\n" \
                ";BIOS:               PIPBUG and BINBUG\n"
            );
        }
    acase 0x90: // HADD
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf(TEXTPEN_LOG, ";PROM binary tape: 0=8-bit, 1=LSN, 2=MSN\n");
        }
    acase 0xA4: // STRT
        if (machine != PIPBUG || pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Store double precision into TEMP\n" \
                ";Function:           Stores the number in R1 and R2 into TEMP and TEMP+1\n" \
                ";Input registers:    R1, R2\n" \
                ";Return code:        None\n" \
                ";Registers used:     R1, R2\n" \
                ";Subroutines called: None\n" \
                ";Nesting depth:      1\n" \
                ";BIOS:               PIPBUG and BINBUG\n"
            );
        }
    acase 0xAB:
        if (machine == BINBUG) // INCT
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Increment TEMP\n" \
                ";Function:           Adds 2-byte number stored at TEMP and TEMP+1 to\n" \
                ";                    R1 and R2 (with carry) and stores result in\n" \
                ";                    TEMP and TEMP+1\n" \
                ";Nesting depth:      1\n" \
                ";BIOS:               BINBUG only\n"
            );
        } else // ALTE
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Display and alter memory\n" \
                ";Input registers:    None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0..R2\n" \
                ";Subroutines called: GNUM, STRT, BOUT, FORM, LINE\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0xB9: // ALTE
        if (machine == BINBUG && binbug_biosver == BINBUG_61)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Display and alter memory\n" \
                ";BIOS:               BINBUG 6.1 only\n" \
                ";Notes:\n" \
                ";Access memory at specified address and if hex number is entered then\n" \
                ";replace memory contents. If command line is terminated by LF then\n" \
                ";display next memory location.\n"
            );
        }
    acase 0xC9: // ZBRR *GPAR -> IGPAR
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf(TEXTPEN_LOG, ";Get parameters\n");
        }
    acase 0xD0: // ZBRR *ADOUT -> IADO
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf(TEXTPEN_LOG, ";Address out - types out address in R1 and R2\n");
        }
    acase 0xDA: // ZBRR *LINE -> ILIN
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Input a cmd line into buffer\n" \
                ";CODE is the end of line character: LF, CR, or up-arrow\n"
            );
        }
    acase 0xEF: // SREG
        if (machine == BINBUG && binbug_biosver == BINBUG_61)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Display and set register stack\n" \
                ";BIOS:               BINBUG 6.1 only\n" \
                ";Notes:              SA will display all registers\n"
            );
        }
    acase 0xF4: // SREG
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Selectively display and alter registers\n" \
                ";Input registers:    None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0, R2\n" \
                ";Subroutines called: GNUM, BOUT, FORM, LINE\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x115: // ALTE
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf(TEXTPEN_LOG, ";Display and alter memory\n");
        }
    acase 0x12B: // BK01
        if (machine == BINBUG && binbug_biosver == BINBUG_61)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Entry for 1st breakpoint via vector\n" \
                ";Input registers:    None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0..R6\n" \
                ";Subroutines called: CLBK, BOUT\n" \
                ";BIOS:               BINBUG 6.1 only\n" \
                ";Notes:\n" \
                ";Breakpoint vector branches indirectly through the ACON at $1B to\n" \
                ";this routine.\n"
            );
        }
    acase 0x131: // GOTO
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Goto address\n" \
                ";Input registers:    None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0..R6\n" \
                ";Subroutines called: GNUM, STRT\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x14E: // SREG
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf(TEXTPEN_LOG, ";Selectively display and alter registers\n");
        }
    acase 0x15D: // SEE
        if (machine == BINBUG && binbug_biosver == BINBUG_61)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Display contents of all registers\n" \
                ";BIOS:               BINBUG 6.1 only\n"
            );
        }
    acase 0x160: // BK01
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Entry for 1st breakpoint via vector\n" \
                ";Input registers:    None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0..R6\n" \
                ";Subroutines called: CLBK, BOUT\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x16A: // CLBK
        if (machine == BINBUG && binbug_biosver == BINBUG_61)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Clear single breakpoint\n" \
                ";BIOS:               BINBUG 6.1 only\n"
            );
        }
    acase 0x16E: // BK02
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Entry for 2nd breakpoint via vector\n" \
                ";Input registers:    None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0..R6\n" \
                ";Subroutines called: CLBK, BOUT\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x186: // CLR
        if (machine == BINBUG && binbug_biosver == BINBUG_61)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Clear single breakpoint\n" \
                ";BIOS:               BINBUG 6.1 only\n"
                ";Notes:\n" \
                ";BP clear is inhibited after reset but may be cleared by\n" \
                ";re-executing BP vector or G18C.\n"
            );
        }
    acase 0x187: // GOTO
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf(TEXTPEN_LOG, ";Goto address\n");
        }
    acase 0x190: // BKPT
        if (machine == BINBUG && binbug_biosver == BINBUG_61)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Set breakpoint vector at address specified\n" \
                ";BIOS:               BINBUG 6.1 only\n"
                ";Notes:\n" \
                ";The contents of the two memory locations are saved and\n" \
                ";replaced by ZBRR *$1B to vector to the BP service routine.\n"
            );
        }
    acase 0x1AB:
        if (machine == PIPBUG)
        {   if (pipbug_biosver == PIPBUG_PIPBUG2BIOS) // BK01
            {   zprintf(TEXTPEN_LOG, ";Breakpoint runtime code\n");
            } else // CLBK
            {   zprintf
                (   TEXTPEN_LOG,
                    ";Long name:          Clear a breakpoint\n" \
                    ";Input registers:    R2\n" \
                    ";Return code:        None\n" \
                    ";Registers used:     R0, R2, R3\n" \
                    ";Subroutines called: None\n" \
                    ";BIOS:               PIPBUG only\n"
                );
        }   }
    acase 0x1B9: // BK02
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf(TEXTPEN_LOG, ";Breakpoint runtime code\n");
        }
    acase 0x1BB: // LPTR
        if (machine == BINBUG && binbug_biosver == BINBUG_61)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Advanced video driver routine\n" \
                ";BIOS:               BINBUG 6.1 only\n"
                ";Notes:\n" \
                ";Decodes the following control characters:\n" \
                ";CR  ($0D) = Position to left of line\n" \
                ";LF  ($0A) = Scroll one line\n" \
                ";BS  ($08) = Back one position\n" \
                ";FF  ($0C) = Clear screen and enter page mode\n" \
                ";HT  ($09) = Advance to next tab stop\n" \
                ";SO  ($0E) = Clear screen\n" \
                ";ESC ($1B) = Select scroll mode\n"
            );
        }
    acase 0x1CA: // CLR
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Clear a breakpoint\n" \
                ";Comments:           MARK indicates if set. HADR+LADR is bkpt addr. HDAT+LDAT is 2 bytes\n" \
                ";Input registers:    None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0, R2\n" \
                ";Subroutines called: NOK, CLBK\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x1D7: // NOK
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Check range on breakpoint number\n" \
                ";Input registers:    None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R2\n" \
                ";Subroutines called: GNUM\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x1E5: // BKPT
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Set breakpoint and clear any existing breakpoint\n" \
                ";Comments:           MARK indicates if set. HADR+LADR is bkpt addr. HDAT+LDAT is 2 bytes\n" \
                ";Input registers:    None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0..R3\n" \
                ";Subroutines called: NOK, CLBK, GNUM, STRT\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x1F5: // CLBK
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Subr to clear a bkpt\n" \
                ";Like many subr has rel...\n"
            );
        }
    acase 0x214: // CLR
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Clear breakpoint\n" \
                ";Notes:\n" \
                ";MARK indicates if set\n" \
                ";HADR+LADR is bpkt address, HDAT+LDAT is two by...\n"
            );
        }
    acase 0x224: // BIN
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Input two hex characters and form as byte in R1\n" \
                ";Input register:     None\n" \
                ";Return code:        R1\n" \
                ";Registers used:     R0, R1, R3\n" \
                ";Subroutines called: CHIN, LKUP, CBCC\n" \
                ";Nesting depth:      3\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x23D: // CBCC
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Calculate the BCC character, exclusive OR and rotate left\n" \
                ";Input register:     R1\n" \
                ";Return code:        R0\n" \
                ";Registers used:     R0, R1\n" \
                ";Subroutines called: None\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x246: // LKUP
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Look up ASCII character in hex value table\n" \
                ";Function:           Converts an ASCII character in R0 into a hex value in R3\n" \
                ";Comments:           Generates an error message if a character is not hex\n" \
                ";Input register:     R0\n" \
                ";Return code:        R3\n" \
                ";Registers used:     R0, R3\n" \
                ";Subroutines called: None\n" \
                ";Nesting depth:      1\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x250: // ABRT
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Abort exit from any level of subroutine\n" \
                ";Comments:           Use RAS pointer since it is possible that a breakpoint\n" \
                ";                    program is using it\n" \
                ";Input register:     None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0\n" \
                ";Subroutines called: CBCC, COUT\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x267: // ZBRR *BIN -> IBIN
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Reads two hex characters from the terminal,\n" \
                ";and converts them into a binary byte value in R1.\n" \
                ";Summary: Input two hex chars and form as byte in R1\n"
            ); // PIPBUG 2 manual, p. 4 is wrong (says R0 instead of R1)
        }
    acase 0x269: // BOUT
        if (machine != PIPBUG || pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Output byte in R1 in hex\n" \
                ";Input register:     None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0, R1\n" \
                ";Subroutines called: CBCC, COUT\n" \
                ";Nesting depth:      3\n" \
                ";BIOS:               PIPBUG and BINBUG\n"
            );
        }
    acase 0x279: // CBCC
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf(TEXTPEN_LOG, ";Calculate the BCC character, EOR and then rotate\n");
        }
    acase 0x27D: // AGAP
        if (machine != PIPBUG || pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Function:           Outputs R3 spaces\n" \
                ";Nesting depth:      3\n" \
                ";BIOS:               PIPBUG and BINBUG\n"
            );
        }
    acase 0x281: // ZBRR *BOUT -> IBOU
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Takes the binary value in register 1 and converts it into\n" \
                ";two hex characters which are printed on the terminal.\n" \
                ";Register zero information is lost.\n" \
                ";Summary: Byte in R1 output in hex\n"
            );
        }
    acase 0x286: // CHIN
        if (machine != PIPBUG || pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          "
            );
            if (machine == BINBUG && binbug_biosver == BINBUG_61)
            {   zprintf(TEXTPEN_LOG, "Keyboard input routine");
            } else
            {   zprintf(TEXTPEN_LOG, "110 baud input for papertape and characters (1 MHz clock)");
            }
            zprintf
            (   TEXTPEN_LOG,
                "\n;Function:           An ASCII character is input to R0 from the keyboard\n" \
                ";Input register:     None\n" \
                ";Return codes:       R0, R4\n" \
                ";Registers used:     R0, R4, R5\n" \
                ";Subroutines called: DLAY, DLY\n" \
                ";Nesting depth:      3\n" \
                ";BIOS:               PIPBUG and BINBUG\n"
            );
        }
    acase 0x28D: // CHNG aka CHING
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Converts the binary value in register 0 into two hex\n" \
                ";characters which are returned in registers 1 and 2.\n" \
                ";Summary: Subr to convert binary to hex\n" \
            );
        }
    acase 0x2A6: // ZBRR *CHIN -> ICHI
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Inputs a byte to register 0 from the terminal\n" \
                ";Subr to input a character into R0, using R1 and R2\n"
            );
        }
    acase 0x2A8: // DLAY
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Delay for one bit time\n" \
                ";Input register:     None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0\n" \
                ";Subroutines called: None\n" \
                ";Nesting depth:      1\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x2AD: // DLY
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Delay for half a bit time\n" \
                ";Input register:     R0\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0\n" \
                ";Subroutines called: None\n" \
                ";Nesting depth:      1\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x2B4: // COUT
        if (machine != PIPBUG || pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Output character"
            );
            if (machine == BINBUG && binbug_biosver == BINBUG_61)
            {   zprintf(TEXTPEN_LOG, " in R0 to VDU or to external routine");
            }
            zprintf
            (   TEXTPEN_LOG,
                "\n;Function:           The byte in R0 is output as an ASCII character\n" \
                ";Input register:     R0 (%s)\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0, R4, R5\n" \
                ";Subroutines called: DLAY\n" \
                ";Nesting depth:      2\n" \
                ";BIOS:               PIPBUG and BINBUG\n",
                asciiname_long[r[0]]
            );
        }
    acase 0x2C2: // DELAY
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Delay loop for one bit time, using R3\n" \
                ";Based on a 1MHz clock, using the count in...\n"
            );
        }
    acase 0x2CE: // ZBRR *COUT -> ICOU
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Outputs the byte in register 0 to the terminal\n" \
                ";Output a char from R0, using R1\n" \
                ";Assume carry can be destroyed\n"
            );
        }
    acase 0x2CF: // LKUP
        if (machine == BINBUG && binbug_biosver == BINBUG_61)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Convert hex digit in R0 to binary in R3\n" \
                ";BIOS:               BINBUG 6.1 only\n"
            );
        }
    acase 0x2DB: // GNUM
        if (machine != PIPBUG || pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Get a number from the buffer into R1..R2\n" \
                ";Comments:           It ignores leading zeroes and correctly\n" \
                ";                    interprets a 1..4 character entry\n" \
                ";Input register:     None\n" \
                ";Return codes:       R1, R2\n" \
                ";Registers used:     R0..R3\n" \
                ";Subroutines called: LKUP\n" \
                ";Nesting depth:      2\n" \
                ";BIOS:               PIPBUG and BINBUG\n"
            );
        }
    acase 0x2EA: // ZBRR *GNUM -> IGNU
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Get a number from the buffer into R1-R2\n" \
                ";Start with the binary nybbles R1=AB, R2=CD\n" \
                ";Read next hex char and convert to nybble E\n" \
                ";Shift into addr being built R1=BC, R2=DE\n"
            );
        }
    acase 0x310: // DUMP
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Dump to papertape in object format\n" \
                ";Input register:     None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0..R3\n" \
                ";Subroutines called: GNUM, STRT, GAP, CRLF, COUT, BOUT, STRT\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x318: // GOTO
        if (machine == BINBUG && binbug_biosver == BINBUG_61)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          GOTO program and execute\n" \
                ";BIOS:               BINBUG 6.1 only\n" \
                ";Notes:              Restores all registers first\n"
            );
        }
    acase 0x31F: // DUMP
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf(TEXTPEN_LOG, ";Dump to paper tape in object format\n");
        }
    acase 0x341: // XCMD
        if (machine == BINBUG && binbug_biosver == BINBUG_61)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Extended command processor\n" \
                ";BIOS:               BINBUG 6.1 only\n" \
                ";Notes:\n" \
                ";To check for existence of VHS data DOS\n" \
                ";and implement W and K commands.\n"
            );
        }
    acase 0x35B: // FORM
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Output 3 blanks\n" \
                ";Input register:     None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0, R3\n" \
                ";Subroutines called: COUT\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x35F: // GAP
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Output 50 blanks\n" \
                ";Input register:     None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0, R3\n" \
                ";Subroutines called: COUT\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x360: // FORM
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf(TEXTPEN_LOG, ";Subrs for outputting blanks\n");
        }
    acase 0x399: // LOAD
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf(TEXTPEN_LOG, ";Load from paper tape in object format\n");
        }
    acase 0x3B5:
        if (machine == PIPBUG && pipbug_biosver != PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";Long name:          Load from papertape in object format\n" \
                ";Input register:     None\n" \
                ";Return code:        None\n" \
                ";Registers used:     R0, R1, R3\n" \
                ";Subroutines called: CHIN, BIN\n" \
                ";BIOS:               PIPBUG only\n"
            );
        }
    acase 0x3E6: // SYNCH
        if (machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS)
        {   zprintf
            (   TEXTPEN_LOG,
                ";This code will auto-synch its baud rate\n" \
                ";On startup or reset of PIPBUG, type a 'U'\n" \
                ";BAUD contains the delay count\n"
            );
        }
    acase 0x40E: // MAIN
        // assert(machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS);
        zprintf(TEXTPEN_LOG, ";Restart after error here\n");
    acase 0x49F: // OPCD
        // assert(machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS);
        zprintf(TEXTPEN_LOG, ";Have instruction - not pseudo-op\n");
    acase 0x4D5: // ARLOOP
        // assert(machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS);
        zprintf(TEXTPEN_LOG, ";Check for index and/or indirect\n");
    acase 0x4F9: // CHKBYT
        // assert(machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS);
        zprintf(TEXTPEN_LOG, ";Check for label as address\n");
    acase 0x569: // DEBLK
        // assert(machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS);
        zprintf(TEXTPEN_LOG, ";Bypass blanks in buff\n");
    acase 0x579: // FNDS
        // assert(machine == PIPBUG && pipbug_biosver == PIPBUG_PIPBUG2BIOS);
        zprintf(TEXTPEN_LOG, ";Find symbol pointed to by R3\n");
    acase 0x6400:
        // assert(machine == BINBUG);
        zprintf
        (   TEXTPEN_LOG,
            ";Description:        Normal entry point\n" \
            ";BIOS:               Graphics driver\n" \
            ";Notes:              Assumes all necessary values have\n" \
            "                     been poked into storage locations.\n"
        );
    acase 0x645B:
        // assert(machine == BINBUG);
        zprintf
        (   TEXTPEN_LOG,
            ";Description:        Reserved function handler\n" \
            ";BIOS:               Graphics driver\n"
        );
    acase 0x645D:
        // assert(machine == BINBUG);
        zprintf
        (   TEXTPEN_LOG,
            ";Description:        Screen window function\n" \
            ";BIOS:               Graphics driver\n" \
            ";Notes:\n" \
            ";Uses (X1,Y1) & (X2,Y2) to define opposite corners\n" \
            ";of a screen area to be set, cleared, or inverted.\n"
        );
    acase 0x6485:
        // assert(machine == BINBUG);
        zprintf
        (   TEXTPEN_LOG,
            ";Description:        Screen clear and protect\n" \
            ";BIOS:               Graphics driver\n"
        );
    acase 0x64B6:
        // assert(machine == BINBUG);
        zprintf
        (   TEXTPEN_LOG,
            ";Description:        Set a point on screen @ R1,R2\n" \
            ";BIOS:               Graphics driver\n"
        );
    acase 0x64BE:
        // assert(machine == BINBUG);
        zprintf
        (   TEXTPEN_LOG,
            ";Description:        Clear point on screen\n" \
            ";BIOS:               Graphics driver\n"
        );
    acase 0x64C8:
        // assert(machine == BINBUG);
        zprintf
        (   TEXTPEN_LOG,
            ";Description:        Invert a point on screen (reverse it)\n" \
            ";BIOS:               Graphics driver\n"
        );
    acase 0x64D0:
        // assert(machine == BINBUG);
        zprintf
        (   TEXTPEN_LOG,
            ";Description:        Test state of point on screen\n" \
            ";BIOS:               Graphics driver\n" \
            ";Return code:\n" \
            ";If point \"on\" returns R0=$FF, CC=LT\n" \
            ";If point \"off\" returns R0=$00, CC=EQ\n" \
            ";If point out of range, returns \"off\" state conditions.\n"
        );
    acase 0x64E3:
        // assert(machine == BINBUG);
        zprintf
        (   TEXTPEN_LOG,
            ";Description:        Coordinate to address conversion\n" \
            ";BIOS:               Graphics driver\n" \
            ";Notes:\n" \
            ";Converts X,Y coords in R1,R2 to VDU memory address & bitmask.\n" \
            ";Coordinates 0,0 are at bottom-left of VDU.\n" \
            ";Coords greater than XMAX & YMAX return a bitmask of $00 & CC=EQ.\n"
        );
    acase 0x6534:
        // assert(machine == BINBUG);
        zprintf
        (   TEXTPEN_LOG,
            ";Description:        Direction test subroutine\n" \
            ";BIOS:               Graphics driver\n" \
            ";Notes:\n" \
            ";Used by WINDOW, LINE & RECTANGLE functions\n" \
            ";Tests X1 against X2 to give DELTAX & XINCR\n" \
            ";Tests Y1 against Y2 to give DELTAY & YINCR\n" \
            ";Returns with (X1,Y1) in R1,R2\n"
        );
    acase 0x656D:
        // assert(machine == BINBUG);
        zprintf
        (   TEXTPEN_LOG,
            ";Description:        Line function\n" \
            ";BIOS:               Graphics driver\n" \
            ";Notes:\n" \
            ";Plots straight lines from (X1,Y1) to (X2,Y2)\n" \
            ";Maximum X or Y coordinate = 127\n"
        );
    acase 0x65F1:
        // assert(machine == BINBUG);
        zprintf
        (   TEXTPEN_LOG,
            ";Description:        Rectangle function\n" \
            ";BIOS:               Graphics driver\n" \
            ";Notes:\n" \
            ";Plots a rectangle using coords (X1,Y1) &\n" \
            ";(X2,Y2) as diagonally opposite corners\n"
        );
    acase 0x662A:
        // assert(machine == BINBUG);
        zprintf
        (   TEXTPEN_LOG,
            ";Description:        Ellipse and circle function\n" \
            ";BIOS:               Graphics driver\n" \
            ";Inputs:\n" \
            ";R1 contains X1 centre co-ordinate\n" \
            ";R2 contains Y1 centre co-ordinate\n" \
            ";X2 contains horiz width/2\n" \
            ";Y2 contains vert height/2\n"
        );
    acase 0x66F0:
        // assert(machine == BINBUG);
        zprintf
        (   TEXTPEN_LOG,
            ";Description:        Divide by 128 subroutine\n" \
            ";BIOS:               Graphics driver\n"
        );
    acase 0x6707:
        // assert(machine == BINBUG);
        zprintf
        (   TEXTPEN_LOG,
            ";Description:        16 bit * 8 bit binary multiplication subroutine\n" \
            ";BIOS:               Graphics driver\n" \
            ";Inputs:\n" \
            ";R0    = OPR1 = Multiplier\n" \
            ";R1,R2 = OPR2 = Multiplicand\n"
        );
    acase 0x683D: // DOS
        // assert(machine == BINBUG);
        zprintf
        (   TEXTPEN_LOG,
            ";Description:        Cold entry point - initializes RAM\n" \
            ";BIOS:               MicroDOS and VHS DOS\n"
        );
    acase 0x686D: // VDU0
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Initialize VDU stream 0\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6879: // SBFP
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Initialize stream buffer pointers\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6893: // ENTER
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Normal entry point\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x68BA: // CHAIN
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Find file, load & execute it\n" \
                ";Nesting depth:      4\n" \
                ";Notes:              Uses stream 20\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6917: // OPNR
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   getdosname();
            zprintf
            (   TEXTPEN_LOG,
                ";Description:        Open a file for read\n" \
                ";Input:              Name is at *BUFF_DOS; use stream in R1\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      5\n" \
                ";BIOS:               VHS DOS\n" \
                ";Name:               \"%s\"\n" \
                ";Stream:             %d\n",
                dosname,
                r[1] / 16
            );
        }
    acase 0x692A: // ASGN
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   getdosname();
            zprintf
            (   TEXTPEN_LOG,
                ";Description:        Assign filespec (eg. LPT: or n:NAME.EXT) to a stream\n" \
                ";Input:              Spec is at *BUFF_DOS; stream # is in R1\n" \
                ";Registers:          Only R1 saved\n" \
                ";Nesting depth:      2\n" \
                ";Notes:              Clear stream buffer; set IDNT, DRV, DBPT, (CODE non-disc)\n" \
                ";BIOS:               MicroDOS\n" \
                ";Name:               \"%s\"\n" \
                ";Stream:             %d\n",
                dosname,
                r[1] / 16
            );
        }
    acase 0x694C: // ASGN
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   getdosname();
            zprintf
            (   TEXTPEN_LOG,
                ";Description:        Assign filespec (eg. LPT: or NAME.EXT) to a stream\n" \
                ";Input:              Spec is at *BUFF_DOS; stream # is in R1\n" \
                ";Registers:          Only R1 saved\n" \
                ";Nesting depth:      2\n" \
                ";BIOS:               VHS DOS\n" \
                ";Name:               \"%s\"\n" \
                ";Stream:             %d\n",
                dosname,
                r[1] / 16
            );
        }
    acase 0x699B: // ASDRV
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Assign drive R0 or explicitly specified drive\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x69B4: // ASNAME
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Parse name into buffer\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x69C0: // PSN
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Subr to parse filespec\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x69CF: // OPNW
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   getdosname();
            zprintf
            (   TEXTPEN_LOG,
                ";Description:        Open a file for writes\n" \
                ";Input:              Name is at *BUFF_DOS; stream # is in R1\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      5\n" \
                ";Notes:              Can't open >1 file for writes (yet)\n" \
                ";BIOS:               VHS DOS\n" \
                ";Name:               \"%s\"\n" \
                ";Stream:             %d\n",
                dosname,
                r[1] / 16
            );
        }
    acase 0x69F5: // PSED
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Default extension\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6A01: // GETSEP
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Get char; test if separator\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6A16: // OPNR
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   getdosname();
            zprintf
            (   TEXTPEN_LOG,
                ";Description:        Open a file for read\n" \
                ";Input:              Name is at *BUFF_DOS; use stream in R1\n" \
                ";Registers:          Saves R1..R3\n" \
                ";BIOS:               MicroDOS\n" \
                ";Name:               \"%s\"\n" \
                ";Stream:             %d\n",
                dosname,
                r[1] / 16
            );
        }
    acase 0x6A26: // OPNRF
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Open after successful FIND\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6A41: // OPNW
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   getdosname();
            zprintf
            (   TEXTPEN_LOG,
                ";Description:        Open a file for writes\n" \
                ";Input:              Name is at *BUFF_DOS; stream # is in R1\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      4\n" \
                ";Notes:              Can't open >1 file for writes\n" \
                ";BIOS:               MicroDOS\n" \
                ";Name:               \"%s\"\n" \
                ";Stream:             %d\n",
                dosname,
                r[1] / 16
            );
        }
    acase 0x6A4C: // OPREM
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        File already exists - remove\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6A82: // OPREM
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        File already exists - remove\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6ABA: // NDAT
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Transfer DENT to directory buffer\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6AB1: // PUTCH
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Put one byte to a file opened for writes\n" \
                ";Input:              Byte is in R0; R1 has str #\n" \
                ";Registers:          Saves R0..R3\n" \
                ";Nesting depth:      3 if disk\n" \
                ";BIOS:               VHS DOS\n" \
                ";Output byte:        $%02X (%s)\n" \
                ";Stream:             %d\n",
                r[0], asciiname_short[r[0]],
                r[1] / 16
            );
        }
    acase 0x6AD9: // PUTCH
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Put one byte to a file opened for writes\n" \
                ";Input:              Byte is in R0; R1 has str #\n" \
                ";Registers:          Saves R0..R3\n" \
                ";Nesting depth:      3 if disk\n" \
                ";BIOS:               MicroDOS\n"
                ";Output byte:        $%02X (%s)\n" \
                ";Stream:             %d\n",
                r[0], asciiname_short[r[0]],
                r[1] / 16
            );
        }
    acase 0x6AF1: // CLSW
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Close a file that was opened for writes\n" \
                ";Input:              First puts one char from R0\n" \
                ";Registers:          Only R1 saved\n" \
                ";Nesting depth:      4 if disk\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6B16: // GETCH
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Get one byte from file opened for reads or writes\n" \
                ";Input:              Stream # must be in R1\n" \
                ";Return code:        Returns byte in R0\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      3 if disk\n" \
                ";BIOS:               MicroDOS\n" \
                ";Stream:             %d\n",
                r[1] / 16
            );
        }
    acase 0x6B2B: // CLSW
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Close a file that was opened for writes\n" \
                ";Registers:          Only R1 saved\n" \
                ";Nesting depth:      3 if disk\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6B7A: // CLSR
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Close a file that was opened for reads\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6B7F: // GETCH
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Get one byte from file opened for reads or writes\n" \
                ";Input:              Stream # must be in R1\n" \
                ";Return code:        Returns byte in R0\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      3\n" \
                ";BIOS:               VHS DOS\n" \
                ";Stream:             %d\n",
                r[1] / 16
            );
        }
    acase 0x6B96: // CHAIN
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Find file, load & execute it\n" \
                ";Nesting depth:      5\n" \
                ";Notes:              MUST use stream 2\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6BA3: // CLSR
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Close a file that was opened for reads\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6BA8: // DELENT
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Delete directory entry just found\n" \
                ";Registers:          Only R1 saved\n" \
                ";Nesting depth:      3\n" \
                ";Notes:              CAUTION - doesn't check for open file yet!\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6BE8: // GDEN
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Get directory entry\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6BF8: // SWAP
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Swap start or end sectors in freelist and DENT\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6C0B: // WRSC
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Write a sector from buffer to disk\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      2 if disk, or M+1 if non-disk,\n" \
                ";                    where M is level of service routine\n" \
                ";BIOS:               MicroDOS\n"
                ";Stream:             %d\n" \
                ";Desired track:      %d\n" \
                ";Desired sector:     %d\n",
                r[1] / 16,
                memory[0x7007 + r[1]],
                memory[0x7008 + r[1]]
            );
        }
    acase 0x6C0C: // FNEXT
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Find next file satisfying a wildcard spec\n" \
                ";Registers:          Only R1 preserved\n" \
                ";Nesting depth:      4\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6C15: // FIND
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Find first file satisfying a wildcard spec\n" \
                ";Registers:          Only R1 preserved\n" \
                ";Nesting depth:      4\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6C66: // STDR
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Move to start of directory\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      2\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6C6C: // FIRST
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Get first sector of found file to buffer\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6C72: // RDS1
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Move to track R2, sector R3\n" \
                ";BIOS:               MicroDOS\n" \
                ";Desired track:      %d\n" \
                ";Desired sector:     %d\n",
                r[2],
                r[3]
            );
        }
    acase 0x6C79: // RDNXSC
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Read next sector\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      2\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6C84: // RDND
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Read non-disk device\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6C8E: // RDSC
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Read one sector from disk to buffer\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      2 if disk, or M+1 if non-disk,\n" \
                ";                    where M is level of service routine\n" \
                ";BIOS:               MicroDOS\n" \
                ";Stream:             %d\n" \
                ";Desired track:      %d\n" \
                ";Desired sector:     %d\n",
                r[1] / 16,
                memory[0x7007 + r[1]],
                memory[0x7008 + r[1]]
            );
        }
    acase 0x6C92: // STDR
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Move to start of directory\n" \
                ";Registers:          Only R1 preserved\n" \
                ";Nesting depth:      2\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6C98: // FIRST
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Get first sector of found file to buffer\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6CA2: // STD1
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Move to track R2, sector R3\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6CAE: // RDNXSC
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Read next sector\n" \
                ";Input:              TMP must be pointing to buffer links\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      2\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6CBB: // RDSC
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Read one sector from disk to buffer\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      2 if disk, or M+1 if non-disk,\n" \
                ";                    where M is level of service routine\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6CC6: // WBSY
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Wait for drive not busy\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6CDB: // DRIVE
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Select drive given in R0 on str R1\n" \
                ";Registers:          Saves R1\n" \
                ";Nesting depth:      1\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6CF4: // WBSY
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Wait for drive not busy\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6CF6: // SK
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Seek to requested track\n" \
                ";BIOS:               MicroDOS\n" \
                ";Requested track:    %d\n",
                memory[0x7007 + r[1]]
            );
        }
    acase 0x6CFB: // WRSC
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Write a sector from buffer to disk\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      2 if disk, or M+1 if non-disk,\n" \
                ";                    where M is level of service routine\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6D65: // DRIVE
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Select drive given in R0\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      1\n" \
                ";BIOS:               VHS DOS\n" \
                ";Desired drive:      %d",
                r[0]
            );
        }
    acase 0x6D6B: // FIND
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   getdosname();
            zprintf
            (   TEXTPEN_LOG,
                ";Description:        Find first file satisfying a spec at *BUFF_DOS\n" \
                ";Returns:            R0 = 0 non-disc; -1 not found; 1 found\n" \
                ";Registers:          Only R1 preserved\n" \
                ";Nesting depth:      3\n" \
                ";BIOS:               MicroDOS\n" \
                ";Name:               \"%s\"\n" \
                ";Stream:             %d\n",
                dosname,
                r[1] / 16
            );
        }
    acase 0x6D82: // FNEXT
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Find next file satisfying a wildcard spec\n" \
                ";Returns:            R0 = -1 non-disc or not found; 1 found\n" \
                ";Registers:          Only R1 preserved\n" \
                ";Nesting depth:      3\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6D86: // NFD
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Print \"Not found...\", return if Escape typed\n" \
                ";Registers:          Saves R1, R2\n" \
                ";Nesting depth:      M+2, where M is level of service routine\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6D90: // GDEN
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Get next non-deleted directory entry\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6D9B: // SK
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Seek to requested track\n" \
                ";BIOS:               VHS DOS\n" \
                ";Stream:             %d\n" \
                ";Desired track:      %d",
                r[1] / 16,
                memory[0x7007 + r[1]]
            );
        }
    acase 0x6DBC: // ERR
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Error handling routine\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6DF7: // PNAM
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Print filename from DENT (Directory ENTry)\n" \
                ";Input:              Use str in R1 (can write to disk, etc.)\n" \
                ";Registers:          Saves R1, R2\n" \
                ";Nesting depth:      M+1, where M is level of service routine\n" \
                ";BIOS:               MicroDOS\n" \
                ";Stream:             %d\n",
                r[1] / 16
            );
        }
    acase 0x6E01: // WRND
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Write buffer to non-disk dev\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6E0F: // DOUT
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Print R2&R3 as decimal to stream R1\n" \
                ";Registers:          R2&R3 NOT preserved\n" \
                ";Nesting depth:      M+2, where M is level of service routine\n" \
                ";BIOS:               MicroDOS\n" \
                ";Stream:             %d\n",
                ";Output bytes:       $%02X%02X (%d)\n",
                r[1] / 16,
                r[2], r[3], (int) ((r[2] * 256) + r[3])
            );
        }
    acase 0x6E12: // PNAM
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Print filename from DENT (Directory ENTry)\n" \
                ";Input:              Use str in R1 (can write to disk, etc.)\n" \
                ";Registers:          Saves R1, R2\n" \
                ";Nesting depth:      M+1, where M is level of service routine\n" \
                ";BIOS:               VHS DOS\n" \
                ";Stream:             %d\n",
                r[1] / 16
            );
        }
    acase 0x6E29: // DOUT
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Print R2&R3 as decimal to stream R1\n" \
                ";Registers:          R2&R3 NOT preserved\n" \
                ";Nesting depth:      M+2, where M is level of service routine\n" \
                ";BIOS:               VHS DOS\n",
                r[1] / 16,
                r[2], r[3], (int) ((r[2] * 256) + r[3])
            );
        }
    acase 0x6E45: // ITEM
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Locate next item in buffer\n" \
                ";Registers:          Saves R1, R2\n" \
                ";Nesting depth:      1\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6E5F: // DEC
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Return CC=EQ if R0 dec char\n" \
                ";Registers:          Returns R0 unchanged\n" \
                ";BIOS:               VHS DOS\n" \
                ";Input byte:         %s\n",
                asciiname_long[r[0]]
            );
        }
    acase 0x6E67: // ITEM
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Locate next item in buffer\n" \
                ";Registers:          Saves R1, R2\n" \
                ";Nesting depth:      1\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6E6B: // PRT
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Accept line of chars, put to BUF\n" \
                ";Registers:          No regs saved\n" \
                ";Nesting depth:      M+2, where M is levels of COUT (normally M=3)\n" \
                ";Notes:              Handles various control chars\n" \
                ";                    Converts to upper case in buffer\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6E8D: // PRT
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Accept line of chars, put to BUF\n" \
                ";Registers:          No regs saved\n" \
                ";Nesting depth:      M+2, where M is levels of COUT (M=3 in CBUG)\n" \
                ";Notes:              Handles various control chars\n" \
                ";                    Converts to upper case in buffer\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6EB9: // CRLF
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Put CR,LF on stream R1\n" \
                ";BIOS:               MicroDOS\n" \
                ";Stream:             %d",
                r[1] / 16
            );
        }
    acase 0x6EBF: // IO
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Do one byte of I/O on stream R1\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      Same as service routine\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6EDC: // CRLF
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Put CR,LF on stream R1\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6EE2: // IO
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Do one byte of I/O on stream R1\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      Same as service routine\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6EEE: // RDND
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Read non-disk device\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6EFC: // WRND
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Write buffer to non-disk dev\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6F0D:
        // assert(machine == BINBUG);
        switch (binbug_dosver)
        {
        case DOS_MICRODOS: // HXOUT
            zprintf
            (   TEXTPEN_LOG,
                ";Description:        Print R0 as hex on str R1\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      M+1, where M is level of service routine\n" \
                ";Notes:              Equivalent to monitor BOUT\n" \
                ";BIOS:               MicroDOS\n" \
                ";Output byte:        $%02X\n" \
                ";Stream:             %d",
                r[0],
                r[1] / 16
            );
        acase DOS_VHSDOS: // PADR
            zprintf
            (   TEXTPEN_LOG,
                ";Description:        Print R0&R2 as hex address\n" \
                ";Registers:          Saves R1, R3\n" \
                ";Nesting depth:      M+2, where M is level of service routine\n" \
                ";BIOS:               VHS DOS\n" \
                ";Output bytes:       $%02X%02X\n",
                r[0], r[2]
            );
        }
    acase 0x6F10: // HXOUT
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Print R0 as hex on str R1\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      M+1, where M is level of service routine\n" \
                ";Notes:              Equivalent to monitor BOUT\n" \
                ";BIOS:               VHS DOS\n" \
                ";Stream:             %d" \
                ";Output bytes:       $%02X%02X\n",
                r[1] / 16,
                r[0], r[2]
            );
        }
    acase 0x6F26: // GNMB
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Get number from buffer to XGOTO\n" \
                ";Returns:            CC = EQ if valid HEX number\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      2\n" \
                ";Notes:              Equivalent to monitor GNUM\n" \
                ";BIOS:               MicroDOS\n"
            );
        }
    acase 0x6F29: // GNMB
        // assert(machine == BINBUG);
        LOGVHSDOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Get number from buffer to XGOTO\n" \
                ";Registers:          Saves R1..R3\n" \
                ";Nesting depth:      2\n" \
                ";Notes:              Equivalent to monitor GNUM\n" \
                ";BIOS:               VHS DOS\n"
            );
        }
    acase 0x6F7A: // DVRS
        // assert(machine == BINBUG);
        LOGMICRODOS
        {   zprintf
            (   TEXTPEN_LOG,
                ";Description:        Return version number\n" \
                ";Notes:              CC = GT\n" \
                ";BIOS:               MicroDOS\n"
            );
}   }   }

MODULE void getdosname(void)
{   int addr,
        i;

    addr = (memory[0x7070] * 256)
         +  memory[0x7071];
    i = 0;
    while (i < 14 && memory[addr + i] != 0x0D && memory[addr + i] != 0)
    {   dosname[i] = memory[addr + i];
        i++;
    }
    dosname[i] = EOS;
}

EXPORT void pipbug_update_miniglows(void)
{   int i;

    if (machine != PIPBUG || !SubWindowPtr[SUBWINDOW_CONTROLS])
    {   return;
    }

    drawctrlglow(    589                           ,  61,                                               TRUE         ); // power
    drawctrlglow(    321                           , 473, (FLAG) ((opcode != 0x40)                    ? TRUE : FALSE)); // run
    for (i = 0; i < 15; i++)
    {   drawctrlglow(557 - ((i / 4) * 8) - (17 * i), 411, (FLAG) ((iar                    & (1 << i)) ? TRUE : FALSE));
    }
    for (i = 0; i < 8; i++)
    {   drawctrlglow(557 - ((i / 4) * 8) - (17 * i), 444, (FLAG) ((memory[iar]            & (1 << i)) ? TRUE : FALSE));
        drawctrlglow(145                 - (18 * i), 411, (FLAG) ((ioport[PORTD].contents & (1 << i)) ? TRUE : FALSE));
        drawctrlglow(145                 - (18 * i), 444, (FLAG) ((ioport[PORTC].contents & (1 << i)) ? TRUE : FALSE));
}   }

EXPORT void pipbug_changebios(FLAG user)
{   // tape_eject();
    if (user)
    {   reopen_subwindow(SUBWINDOW_TAPEDECK);
    }

    /*             110    300    4800
       Artemis      N      N      Y
       HYBUG        N      Y      N
       PIPBUG 1     Y      Y      Y
       PIPBUG 2     Y      Y      N

    if (iar >= 0x2A8 && iar <= 0x2B0) // in case we are in the middle of executing the routine we want to replace
    {   r[0] = 0xE5;
        pullras();
    } */

    switch (pipbug_biosver)
    {
    case PIPBUG_ARTEMIS:
        memcpy(memory, artemis, 4089); // $0..$FF8
        machines[PIPBUG].firstequiv       =
        machines[PIPBUG].lastequiv        =
        machines[PIPBUG].firstcodecomment =
        machines[PIPBUG].lastcodecomment  =
        machines[PIPBUG].firstdatacomment =
        machines[PIPBUG].lastdatacomment  = -1;
        memmap = MEMMAP_ARTEMIS;
        if (user) pipbug_setmemmap(); // needed for updating COMMENTED fields at least
    acase PIPBUG_HYBUG:
        memcpy(memory, hybug, 1024); // $0..$3FF
        machines[PIPBUG].firstequiv       =
        machines[PIPBUG].lastequiv        =
        machines[PIPBUG].firstcodecomment =
        machines[PIPBUG].lastcodecomment  =
        machines[PIPBUG].firstdatacomment =
        machines[PIPBUG].lastdatacomment  = -1;
        memmap = MEMMAP_PIPBUG1;
        if (user) pipbug_setmemmap(); // needed for updating COMMENTED fields at least
    acase PIPBUG_PIPBUG1BIOS:
        memcpy(memory, pipbug, 1024); // $0..$3FF
        machines[PIPBUG].firstequiv       = FIRSTPIPBUG1EQUIV;
        machines[PIPBUG].lastequiv        = LASTPIPBUG1EQUIV;
        machines[PIPBUG].firstcodecomment = FIRSTPIPBUG1CODECOMMENT;
        machines[PIPBUG].lastcodecomment  = LASTPIPBUG1CODECOMMENT;
        machines[PIPBUG].firstdatacomment = FIRSTPIPBUG1DATACOMMENT;
        machines[PIPBUG].lastdatacomment  = LASTPIPBUG1DATACOMMENT;
        memmap = MEMMAP_PIPBUG1;
        if (user) pipbug_setmemmap(); // needed for updating COMMENTED fields at least
    acase PIPBUG_PIPBUG2BIOS:
        memcpy(memory, pipbug2, 2048); // $0..$7FF
        machines[PIPBUG].firstequiv       = FIRSTPIPBUG2EQUIV;
        machines[PIPBUG].lastequiv        = LASTPIPBUG2EQUIV;
        machines[PIPBUG].firstcodecomment = FIRSTPIPBUG2CODECOMMENT;
        machines[PIPBUG].lastcodecomment  = LASTPIPBUG2CODECOMMENT;
        machines[PIPBUG].firstdatacomment = FIRSTPIPBUG2DATACOMMENT;
        machines[PIPBUG].lastdatacomment  = LASTPIPBUG2DATACOMMENT;
        memmap = MEMMAP_PIPBUG2;
        if (user) pipbug_setmemmap(); // needed for updating COMMENTED fields at least
    }

    pipbug_changebaud();

    if (user)
    {   reopen_subwindow(SUBWINDOW_SPEED);
}   }

EXPORT void pipbug_changebaud(void)
{   // assert(machine == PIPBUG);

    switch (pipbug_biosver)
    {
    case PIPBUG_ARTEMIS:
        pipbug_baudrate = PIPBUG_BAUDRATE_4800;
    acase PIPBUG_PIPBUG1BIOS:
        switch (pipbug_baudrate)
        {
        case  PIPBUG_BAUDRATE_110:  memory[0x2A8] = 0x20; // DLAY: EORZ,r0
                                    memory[0x2A9] = 0xF8; //       BDRR,r0
                                    memory[0x2AA] = 0x7E; //               $
                                    memory[0x2AD] = 0xF8; // DLY:  BDRR,r0
                                    memory[0x2AE] = 0x7E; //               $
                                 // memory[0x2AF] = 0x04; //       LODI,r0
                                    memory[0x2B0] = 0xE5; //               $E5
        acase PIPBUG_BAUDRATE_300:  memory[0x2A8] = 0x20; // DLAY: EORZ,r0
                                    memory[0x2A9] = 0xA4; //       SUBI,r0
                                    memory[0x2AA] = 0x7E; //               $7E
                                    memory[0x2AD] = 0xA4; // DLY:  SUBI,r0
                                    memory[0x2AE] = 0x7E; //               $7E
                                 // memory[0x2AF] = 0x04; //       LODI,r0
                                    memory[0x2B0] = 0xE5; //               $E5
                                    // see EA Feb 1979, p. 69
        acase PIPBUG_BAUDRATE_4800: memory[0x2A8] = 0xC0; // DLAY: NOP
                                    memory[0x2A9] = 0x04; //       LODI,r0
                                    memory[0x2AA] = 0x07; //               $07
                                    memory[0x2AD] =       // DLY:  NOP
                                    memory[0x2AE] = 0xC0; //       NOP
                                 // memory[0x2AF] = 0x04; //       LODI,r0
                                    memory[0x2B0] = 0x07; //               $07
        }
    acase PIPBUG_PIPBUG2BIOS:
        if (pipbug_baudrate == PIPBUG_BAUDRATE_4800)
        {   pipbug_baudrate = BAUDRATE_DEFAULTPIPBUG2;
        }
        switch (pipbug_baudrate)
        {
        case  PIPBUG_BAUDRATE_110: memory[0x816] = 249;
        acase PIPBUG_BAUDRATE_300: memory[0x816] =  89;
        }
    acase PIPBUG_HYBUG:
        pipbug_baudrate = PIPBUG_BAUDRATE_300;
    }

    switch (pipbug_baudrate)
    {
    case  PIPBUG_BAUDRATE_110:  cpb = 9091; // 9090.90' clocks per teletype bit (for  110 baud at 1 MHz or  220 baud at 2 MHz) (1,000,000 /  110 = 9090.90')
    acase PIPBUG_BAUDRATE_300:  cpb = 3333; // 3333.3'  clocks per teletype bit (for  300 baud at 1 MHz or  600 baud at 2 MHz) (1,000,000 /  300 = 3333.3' )
    acase PIPBUG_BAUDRATE_4800: cpb =  208; //  208.3'  clocks per teletype bit (for 4800 baud at 1 MHz or 9600 baud at 2 MHz) (1,000,000 / 4800 =  208.3' )
}   }

EXPORT void pipbin_readtty(void)
{   PERSIST FLAG  breaking = FALSE;
    FAST    UBYTE t;

    // assert(machine == PIPBUG || machine == BINBUG);

#ifdef WIN32
    if (recmode == RECMODE_NORMAL && KeyDown(SCAN_END))
#endif
#ifdef AMIGA
    if (recmode == RECMODE_NORMAL && KeyDown(SCAN_HELP))
#endif
    {   psu &= ~(PSU_S);
        breaking = TRUE;
        return;
    } elif (breaking)
    {   psu |= PSU_S;
        breaking = FALSE;
        return;
    }

    if (tt_kybdstate == 0)
    {   t = get_guest_key();
        if (t != NC)
        {   tt_kybdcode  = t;
            tt_kybdstate = 1; // start bit
            tt_kybdtill  = cycles_2650 + cpb;
            psu &= ~(PSU_S);
#ifdef KEYCLICKS
            if (ambient)
            {   play_sample(SAMPLE_CLICK);
            }
#endif
}   }   }

EXPORT void pipbug_serialize_cos(void)
{   int i,
        oldvdu,
        x, y;

    for (i = 0; i <= 0x7FFF; i++)
    {   serialize_byte(&memory[i]);
    }
    serialize_byte_int(&pipbug_biosver);
    serialize_byte_int(&pipbug_baudrate);
    pipbug_changebaud();
    serialize_byte(&ioport[0].contents);
    serialize_byte(&ioport[1].contents);
    serialize_byte(&ioport[2].contents);
    serialize_byte(&ioport[4].contents);
    serialize_byte(&ioport[5].contents);
    serialize_byte(&ioport[6].contents);
    serialize_byte(&ioport[8].contents);
    serialize_byte(&ioport[16].contents);
    serialize_byte(&ioport[255].contents);
    serialize_byte(&ioport[PORTC].contents);
    serialize_byte(&ioport[PORTD].contents); // not really needed
    for (y = 0; y < 64; y++)
    {   for (x = 0; x < 80; x++)
        {   serialize_byte(&vdu[x][y]);
    }   }
    serialize_byte_int(&vdu_scrolly);
    serialize_byte_int(&tt_kybdstate);
    serialize_byte_int(&tt_scrn);
    serialize_byte(    &tt_kybdcode);
    serialize_byte(    &tt_scrncode);
    serialize_long(    &tt_kybdtill);
    serialize_long(    &tt_scrntill);
    oldvdu = pipbug_vdu;
    serialize_byte_int(&pipbug_vdu);
    if (cosversion < 42 && pipbug_vdu >= 3)
    {   pipbug_vdu++; // 3..4 -> 4..5
    }
    serialize_byte((UBYTE*) &paperreaderenabled);
    serialize_byte((UBYTE*) &paperpunchenabled);
    if (cosversion >= 42)
    {   serialize_byte_int(&belling[0]);
    }
    serialize_byte(&rotorspeed);
    serialize_byte(&pitchangle);
    serialize_byte(&yawsensordir);
    serialize_byte(&windspeed);
    serialize_byte(&winddirection);
    serialize_byte_int(&pipbug_periph);
    serialize_byte(&sensormode);
    serialize_word(&linearx);
    serialize_word(&lineary);
    serialize_word(&linearu);
    serialize_word(&linearv);
    serialize_word(&magneticx);
    serialize_word(&magneticy);
    serialize_word(&magneticz);
    serialize_word(&planepitch);
    serialize_word(&planeroll);

    if (serializemode == SERIALIZE_READ)
    {   if (pipbug_vdu != oldvdu)
        {   calc_margins();
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
        }
        queuepos = 0; // so it doesn't insert a 'U'
}   }

EXPORT void euy_printchar(UBYTE thechar, FLAG eti)
{   int  needx,
         x, y;
    TEXT asciichar;

/* EUY     ASCII   array
$00..$1F $00..$1F -------- control codes
$40..$5F $20..$3F $00..$1F ' !"#$%&'()*+,-./0123456789:;<=>?'
$80..$9F $40..$5F $20..$3F '@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_'
*/

    if (thechar == 0 || thechar == 0x40)
    {
#ifdef VERBOSE
        zprintf(TEXTPEN_PRINTER, "Ignoring EUY character $%02X.\n", thechar);
#endif
        return; // real I/O interface might do something different
    }

    if (eti)
    {   asciichar = thechar & 0x7F;
    } else
    {   if     (thechar <= 0x1F)
        {   asciichar = thechar;        // $00..$1F -> $00..$1F
        } elif (thechar >= 0x60 && thechar <= 0x9F)
        {   asciichar = thechar - 0x40; // $60..$9F -> $20..$5F
        } else
        {
#ifdef VERBOSE
            zprintf(TEXTPEN_PRINTER, "Ignoring EUY character $%02X.\n", thechar);
#endif
            return; // real I/O interface might do something different
    }   }
#ifdef VERBOSE
    zprintf(TEXTPEN_PRINTER, "Printing EUY character '%c' ($%02X).\n", asciichar, thechar);
#endif

    if (narrowprinter)
    {   needx = PRINTERX;
    } else
    {   needx = PRINTERX * 2;
    }
    if (asciichar == LF || printer[0].x + needx >= EUYWIDTH)
    {   printer[0].x = EUYPRINTERX;
#ifdef WIN32
        printer[0].pdu[printer[0].pdu_y][printer[0].pdu_x++] = CR;
#endif
        printer[0].pdu[printer[0].pdu_y][printer[0].pdu_x++] = LF;
        printer[0].pdu[printer[0].pdu_y][printer[0].pdu_x  ] = EOS;
        printer[0].pdu_x = 0;
        if (printer[0].pdu_y < EUYROWS - 1)
        {   printer[0].pdu_y++;
        }
        printer_scroll();
        if (asciichar < 0x20)
        {   update_printer(); // as we won't execute the update_printer() at the end of this routine
    }   }

    if (asciichar >= 0x20)
    {   thechar = asciichar - 0x20;
    } else
    {   return;
    }

    printer[0].pdu[printer[0].pdu_y][printer[0].pdu_x++] = asciichar;

    if (narrowprinter)
    {   for (y = 0; y < 7; y++)
        {   for (x = 0; x < 5; x++)
            {   if (printer[0].x + x < EUYWIDTH && (euy[thechar][y] & (0x10 >> x)))
                {   printer_changepixel(0, printer[0].x +  x         , EUYHEIGHT - 1 - EUYPRINTERY + y + 3, BLACK);
                } else
                {   printer_changepixel(0, printer[0].x +  x         , EUYHEIGHT - 1 - EUYPRINTERY + y + 3, EUY_PAPER);
    }   }   }   }
    else
    {   for (y = 0; y < 7; y++)
        {   for (x = 0; x < 5; x++)
            {   if (printer[0].x + (x * 2) + 1 < EUYWIDTH && (euy[thechar][y] & (0x10 >> x)))
                {   printer_changepixel(0, printer[0].x + (x * 2)    , EUYHEIGHT - 1 - EUYPRINTERY + y + 3, BLACK);
                    printer_changepixel(0, printer[0].x + (x * 2) + 1, EUYHEIGHT - 1 - EUYPRINTERY + y + 3, BLACK);
                } else
                {   printer_changepixel(0, printer[0].x + (x * 2)    , EUYHEIGHT - 1 - EUYPRINTERY + y + 3, EUY_PAPER);
                    printer_changepixel(0, printer[0].x + (x * 2) + 1, EUYHEIGHT - 1 - EUYPRINTERY + y + 3, EUY_PAPER);
    }   }   }   }
    printer[0].x += needx;

    if (ambient && asciichar != ' ')
    {   play_sample(SAMPLE_PRINTER);
    }
    update_printer();
}

EXPORT void printer_scroll(void)
{   int x, y;

    if (printer[prtunit].sprocketrow == printerrows_full - 1)
    {   if (eachpage)
        {
#ifdef WIN32
            printer_copygfx(FALSE, KIND_BMP, FALSE, prtunit);
#endif
#ifdef AMIGA
            printer_copygfx(FALSE, KIND_ILBM, FALSE, prtunit);
#endif
            edit_savetext(FALSE, FALSE, prtunit, FALSE);
        }
        printer[prtunit].sprocketrow = 0;
        printer[prtunit].page++;
    } else
    {   printer[prtunit].sprocketrow++;
    }

    for (y = 0; y < printerheight_full - printer[prtunit].y; y++)
    {   for (x = 0; x < printerwidth_full; x++)
        {   printer_changepixel(prtunit, x, y, printer[prtunit].scrn[y + printer[prtunit].y][x]);
    }   }
    if (machine == TWIN)
    {   for (y = C306HEIGHT_FULL - C306PRINTERY; y < C306HEIGHT_FULL; y++)
        {   for (x = 0; x < C306WIDTH_FULL; x++)
            {   printer_changepixel(prtunit, x, y, C306C_PAPER);
        }   }
        c306_margins(prtunit);
    } else
    {   for (y = EUYHEIGHT - EUYPRINTERY; y < EUYHEIGHT; y++)
        {   for (x = 0; x < EUYWIDTH; x++)
            {   printer_changepixel(prtunit, x, y, EUY_PAPER);
        }   }
        if (prtunit == 0)
        {   euy_margins();
    }   }

    for (y = 0; y < printerrows_full - 1; y++)
    {   for (x = 0; x < printercolumns + 3; x++)
        {   printer[prtunit].pdu[y][x] = printer[prtunit].pdu[y + 1][x];
    }   }
#ifdef WIN32
    printer[prtunit].pdu[printer[prtunit].pdu_y][0] = CR;
    printer[prtunit].pdu[printer[prtunit].pdu_y][1] = LF;
    printer[prtunit].pdu[printer[prtunit].pdu_y][2] = EOS;
#endif
#ifdef AMIGA
    printer[prtunit].pdu[printer[prtunit].pdu_y][0] = LF;
    printer[prtunit].pdu[printer[prtunit].pdu_y][1] = EOS;
#endif
    if (printer[prtunit].top > 0)
    {   printer[prtunit].top--;
}   }

EXPORT void printer_changepixel(int whichprinter, int x, int y, int colour)
{   // assert(x >= 0);
    // assert(x <  printerwidth_full);
    // assert(y >= 0);
    // assert(y <  printerheight_full);

    if
    (   x - prtscrollx >= 0
     && x - prtscrolly <  printerwidth_view
     && y - prtscrolly >= 0
     && y - prtscrolly <  printerheight_view
    )
    {
#ifdef WIN32
        canvasdisplay[CANVAS_PRINTER][( (y - prtscrolly) * PRINTERWIDTH_VIEW) + x - prtscrollx] = pencolours[colourset][colour];
#endif
#ifdef AMIGA
        *(canvasbyteptr[CANVAS_PRINTER][(y - prtscrolly)]                     + x - prtscrollx) = bytepens[             colour];
#endif
    }
    printer[whichprinter].scrn[                       y ]                     [ x]              =                       colour ;
}

EXPORT void printer_eject(int whichprinter)
{   int x, y;

    printer_savepartial(whichprinter);

    printer[whichprinter].page++;
    printer[whichprinter].sprocketrow = 0;
    printer[whichprinter].pdu_y =
    printer[whichprinter].top   = printerrows_full - 1;
    if (machine == TWIN)
    {   for (y = 0; y < C306HEIGHT_FULL - C306PRINTERY; y++)
        {   for (x = 0; x < C306WIDTH_FULL; x++)
            {   printer_changepixel(whichprinter, x, y, BLACK);
        }   }
        for (y = C306HEIGHT_FULL - C306PRINTERY; y < C306HEIGHT_FULL; y++)
        {   for (x = 0; x < C306WIDTH_FULL; x++)
            {   printer_changepixel(whichprinter, x, y, C306C_PAPER);
        }   }
        c306_margins(whichprinter);
    } else
    {   for (y = 0; y < EUYHEIGHT - EUYPRINTERY; y++)
        {   for (x = 0; x < EUYWIDTH; x++)
            {   printer_changepixel(whichprinter, x, y, BLACK);
        }   }
        for (y = EUYHEIGHT - EUYPRINTERY; y < EUYHEIGHT; y++)
        {   for (x = 0; x < EUYWIDTH; x++)
            {   printer_changepixel(whichprinter, x, y, EUY_PAPER);
        }   }
        if (whichprinter == 0)
        {   euy_margins();
    }   }

    for (y = 0; y < printerrows_full; y++)
    {
#ifdef WIN32
        printer[whichprinter].pdu[y][0] = CR;
        printer[whichprinter].pdu[y][1] = LF;
        printer[whichprinter].pdu[y][2] = EOS;
#endif
#ifdef AMIGA
        printer[whichprinter].pdu[y][0] = LF;
        printer[whichprinter].pdu[y][1] = EOS;
#endif
}   }

EXPORT void pipbug_drawhelpgrid(void)
{   int x,  y,
        xx, yy,
        startx,
        starty;

    for (x = 0; x < vdu_columns; x++)
    {   for (y = 0; y < vdu_rows_visible; y++)
        {   startx = x * pipbug_charwidth;
            starty = y * pipbug_charheight;
            for (xx = 0; xx < pipbug_charwidth; xx++)
            {   for (yy = 0; yy < pipbug_charheight; yy++)
                {   if (xx == 0 || xx == pipbug_charwidth - 1 || yy == 0 || yy == pipbug_charheight - 1)
                    {   changepixel(absxmin + startx + xx, absymin + starty + yy, GREY1);
}   }   }   }   }   }

#ifdef COLOURPIPBUG
MODULE int pipbug_getcolour(UBYTE whichchar)
{   if   ( whichchar >= 0x80                                                              ) return CYAN;
    elif ((whichchar >= 'A' && whichchar <= 'Z') || (whichchar >= 'a' && whichchar <= 'z')) return GREEN;
    elif ( whichchar >= '0' && whichchar <= '9'                                           ) return YELLOW;
    elif ( whichchar == '*'                                                               ) return PURPLE;
    else                                                                                    return RED;
}
#else
MODULE int pipbug_getcolour(UNUSED UBYTE whichchar)
{   if (!blink && whichchar >= 0x80 && pipbug_vdu != VDU_RADIOBULLETIN && pipbug_vdu != VDU_VT100)
    {   return GREY5;
    } else
    {   return vdu_fgc;
}   }
#endif

EXPORT void pipbin_prtdemo(void)
{   TRANSIENT int  i;
#ifdef NUMERICDEMO
    TRANSIENT TEXT demostring[12 + 1];
    TRANSIENT int  thelength;
    PERSIST   int  j = 0;

    sprintf(demostring, "%d\x0D\x0A", j++);
    thelength = strlen(demostring);
    for (i = 0; i < thelength; i++)
    {   euy_printchar(demostring[i], TRUE);
    }
#else
    TRANSIENT TEXT demostring[46 + 1] = "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG.\x0D\x0A";

    for (i = 0; i < 46; i++)
    {   euy_printchar(demostring[i], TRUE);
    }
#endif
}

EXPORT void pipbug_view_basic(void)
{   int i;

    // This assumes 2650 Micro BASIC (by Alan Peek).

    // assert(machine == PIPBUG);

    zprintf(TEXTPEN_CLIOUTPUT, "Addresses  Name Contents = Value\n" \
                               "---------- ---- -------- = -----\n");

    for (i = 0x464; i <= 0x496; i += 2)
    {   zprintf
        (   TEXTPEN_CLIOUTPUT,
            "$%X..$%X %c       $%02X%02X = %5d\n",
            i,
            i + 1,
            'A' + ((i - 0x464) / 2),
            memory[i    ],
            memory[i + 1],
            (memory[i    ] * 256) + memory[i + 1]
        );
    }

    zprintf
    (   TEXTPEN_CLIOUTPUT,
        "\n"
    );
}

EXPORT void elekterminal_scroll(int delta)
{   // assert(scroll != 0);

    if (delta > 0)
    {   vdu_scrolly += delta;
        if (vdu_scrolly >= vdu_rows_total)
        {   vdu_scrolly -= vdu_rows_total;
    }   }
    else
    {   if (vdu_scrolly < -delta)
        {   vdu_scrolly += vdu_rows_total;
        }
        vdu_scrolly += delta;
}   }

EXPORT void reset_vdu(FLAG full)
{   int x, y;

    // assert(machine == PIPBUG);

    if (full)
    {   for (y = 0; y < MAX_VDUROWS; y++)
        {   for (x = 0; x < MAX_VDUCOLUMNS; x++)
            {   vdu[x][y] = blank;
    }   }   }

    vdu_x = 0;
    vdu_scrolly = 48;
    switch (pipbug_vdu)
    {
    case VDU_ELEKTERMINAL:
        vdu_y       = 48;
    acase VDU_RADIOBULLETIN:
        vdu_y       = 15;
    adefault:
        vdu_y       =  0;
}   }

EXPORT void pipbug_reset(void)
{   int i;

    // keyboard
    tt_kybdstate    = 0;
    tt_kybdtill     = 0;
    queuepos        = 0;

    // screen
    tt_scrn         = 0;
    tt_scrntill     = 0;
    ignore_cout     = FALSE;
    reset_vdu(FALSE);

    // keyboard & screen
    psu |= PSU_S | PSU_F;
    pipbug_changebaud(); // probably not needed?
    if (pipbug_biosver == PIPBUG_PIPBUG2BIOS && !post)
    {   thequeue[queuepos++] = 'U';
    }

    // LED digits
    for (i = 0; i < DIGITLEDS; i++)
    {   digitleds[i] = 0;
    }

    // sound
    guestpitch[TONE_1STXVI] = 0;
    playsound(FALSE);

    // game
    if (game)
    {   iar = (startaddr_h * 256) + startaddr_l;
    } else
    {   iar = 0;
}   }

EXPORT void euy_margins(void)
{
#ifdef PERFORATEEUY
    int x;

    if (printer[0].sprocketrow == EUYROWS - 1)
    {   // horizontal perforation
        for (x = 0; x < EUYWIDTH; x += 2)
        {   printer_changepixel(0, x, EUYHEIGHT - EUYPRINTERY + 11, PERFORATIONCOLOUR);
    }   }
#else
    ;
#endif
}

EXPORT void printer_savepartial(int whichprinter)
{   if
    (   eachpage
     && (   (machine == TWIN && whichprinter <= 1)
         || ((machine == PIPBUG || machine == BINBUG || machine == CD2650) && whichprinter == 0)
        )
     && (   printer[whichprinter].pdu_x != 0
         || printer[whichprinter].sprocketrow != 0
    )   )
    {
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Page %d of printer %d needs saving.\n", printer[whichprinter.page, whichprinter);
#endif
#ifdef WIN32
        printer_copygfx(FALSE, KIND_BMP, FALSE, whichprinter);
#endif
#ifdef AMIGA
        printer_copygfx(FALSE, KIND_ILBM, FALSE, whichprinter);
#endif
        edit_savetext(FALSE, FALSE, whichprinter, TRUE);
}   }

EXPORT void printers_reset(void)
{   int i;
#ifdef AMIGA
    int y;
#endif

    prtscrollx             = 0;
    if (machine == TWIN)
    {   printerwidth_full  = C306WIDTH_FULL;
        printerwidth_view  = C306WIDTH_VIEW;
        printerheight_full = C306HEIGHT_FULL;
        printerheight_view = C306HEIGHT_VIEW;
        printercolumns     = C306COLUMNS;
        printerrows_full   = C306ROWS;
        prtscrolly         = C306HEIGHT_FULL - C306HEIGHT_VIEW;
    } else
    {   printerwidth_full  =
        printerwidth_view  = EUYWIDTH;
        printerheight_full =
        printerheight_view = EUYHEIGHT;
        printercolumns     = EUYCOLUMNS;
        printerrows_full   = EUYROWS;
        prtscrolly         =
        prtunit            = 0;
    }

    for (i = 0; i < 2; i++)
    {   if (machine == TWIN)
        {   printer[i].x   = C306PRINTERX;
            printer[i].y   = C306PRINTERY;
        } else
        {   printer[i].x   = EUYPRINTERX;
            printer[i].y   = EUYPRINTERY;
        }
        printer[i].page        = // printer_eject() will increment it to 1
        printer[i].queuepos    =
        printer[i].pdu_x       = 0;
        // sprocketrow, pdu_y, etc. are done by printer_eject()
    }

#ifdef AMIGA
    for (y = 0; y < PRINTERHEIGHT_VIEW; y++)
    {   canvasbyteptr[CANVAS_PRINTER][y] = &canvasdisplay[CANVAS_PRINTER][GFXACCESS(0, y, printerwidth_view)];
    }
#endif

    printer_eject(0);
    printer_eject(1);
}

#define TIPSPEEDRATIO   7.0   // dimensionless
#define ROTORRADIUS     4.953 // in metres
#define AIRDENSITY      1.214 // Amherst, MA at ~90 m elevation
#define CP              0.35  // WF-1 reasonable aerodynamic efficiency
#define FIELDVOLTMAX  994.5   // in mV
#define MAX_RPM       180.0   // realistic mechanical limit
#define MAX_POWER      25.0   // 25 kW generator limit
MODULE void update_wind(void)
{   FAST int    number;
    FAST double yaw_error_deg,
                yaw_error_rad,
                effective_wind,
                pitch_factor,
                corrected_wind,
                rpm_target,
                rotor_area;

    if (pitchangle > 90)
    {   pitchangle = 90;
    }
    // pitch can move at about 3 degrees (3 sensor units) per 1 second (estimate), ie. 1 degree (1 sensor unit) per 1/3 second
    if (ioport[0].contents & 0x7F) // blade pitch motor speed
    {   number = (17 * 127) / (ioport[0].contents & 0x7F); // every 17 frames (~1/3 second) or slower
        if (!(frames % number))
        {   if   (ioport[0].contents & 0x80) // clockwise (feather)
            {   if (pitchangle < 90) pitchangle++;
            } else // anticlockwise (unfeather)
            {   if (pitchangle >  0) pitchangle--;
    }   }   }

    // yaw can move at about 1.4 degrees (1 sensor unit) per 2 seconds (estimate)
    if ((ioport[1].contents & 0x7F) && (ioport[4].contents & 1)) // hub yaw motor speed and drive
    {   number = (100 * 127) / (ioport[1].contents & 0x7F); // every 100 frames (2 seconds) or slower
        if (!(frames % number))
        {   if   (ioport[1].contents & 0x80) // clockwise
            {   yawsensordir++; // overflow is OK
            } else // anticlockwise
            {   yawsensordir--; // underflow is OK
    }   }   }
    yaw_error_deg = (double) ((winddirection / 255.0) * 360.0) - (double) ((yawsensordir / 255.0) * 360.0);
    // assert(yaw_error_deg >= -360);
    // assert(yaw_error_deg <=  360);
    if   (yaw_error_deg < -180.0) yaw_error_deg += 360.0; // -360..-180 ->    0..180
    elif (yaw_error_deg >  180.0) yaw_error_deg -= 360.0; // +180..+360 -> -180..  0
    yaw_error_rad     = yaw_error_deg * (PI / 180.0);
    effective_wind    = windspeed * cos(yaw_error_rad);
    if (effective_wind < 0.0) // wind behind turbine
    {
#ifdef VERBOSEINDUSTRIAL
        zprintf(TEXTPEN_VERBOSE, "Negative effective wind speed on frame %d (%.3f mph)!\n", frames, effective_wind);
#endif
        effective_wind = 0.0;
    }
    pitch_factor      = 1.0 - ((double) pitchangle / 90.0);
    corrected_wind    = effective_wind * pitch_factor;
    rpm_target        = (TIPSPEEDRATIO * corrected_wind * 60) / (2 * PI * ROTORRADIUS);
    if (rpm_target < 0.0)
    {
#ifdef VERBOSEINDUSTRIAL
        zprintf(TEXTPEN_VERBOSE, "Turbine attempted to spin backwards on frame %d (%.3f rpm)!\n", frames, rpm_target);
#endif
        rpm_target = 0.0;
    } elif (rpm_target > MAX_RPM)
    {
#ifdef VERBOSEINDUSTRIAL
        zprintf(TEXTPEN_VERBOSE, "Turbine overspeed on frame %d (%.3f rpm vs. %.3f rpm)!\n", frames, rpm_target, MAX_RPM);
#endif
        rpm_target = MAX_RPM;
    }
    rotorspeed        = (UBYTE) rpm_target;

    rotor_area        = PI * ROTORRADIUS * ROTORRADIUS;
    mechpower_kw      = 0.0005 * AIRDENSITY * rotor_area * corrected_wind * corrected_wind * corrected_wind * CP;

    elecpower_kw      = ((3.9 * (double) ioport[2].contents) / FIELDVOLTMAX) * mechpower_kw;
    if (elecpower_kw > MAX_POWER)
    {
#ifdef VERBOSEINDUSTRIAL
        zprintf(TEXTPEN_VERBOSE, "Turbine overvoltage on frame %d (%.3f kW vs. %.3f kW)!\n", frames, elecpower_kw, MAX_POWER);
#endif
        elecpower_kw = MAX_POWER;
}   }

EXPORT void update_industrial(FLAG force)
{   PERSIST UBYTE  oldfieldcurrent,
                   oldpitchspeed,
                   oldpitchangle,
                   oldrotorspeed,
                   oldwindspeed,
                   oldwinddirection,
                   oldyawmotor,
                   oldyawmotormode,
                   oldyawsensordir;
    PERSIST UWORD  oldmagnetx,
                   oldmagnety,
                   oldmagnetz,
                   oldplanepitch,
                   oldplaneroll,
                   oldlinearx,
                   oldlineary,
                   oldlinearu,
                   oldlinearv;
    PERSIST double oldmechpower,
                   oldelecpower;
    FAST    double inclination_rad,
                   latitude_deg,
                   latitude_rad;

    if (!SubWindowPtr[SUBWINDOW_INDUSTRIAL])
    {   return;
    }

    switch (pipbug_periph)
    {
    case PERIPH_FURNACE:
        if
        (   ioport[0].contents != oldpitchspeed
         || ioport[1].contents != oldyawmotor
         || ioport[4].contents != oldyawmotormode
         || rotorspeed         != oldrotorspeed
         || pitchangle         != oldpitchangle
         || yawsensordir       != oldyawsensordir
         || windspeed          != oldwindspeed
         || winddirection      != oldwinddirection
         || force
        )
        {   redraw_furnace();
        }

        if ((ioport[0].contents & 0x7F) != (oldpitchspeed & 0x7F) || force)
        {   sl_set(SUBWINDOW_INDUSTRIAL, IDC_PITCHSPEED, ioport[0].contents & 0x7F);
            if ((ioport[0].contents & 0x7F) == 0 || (ioport[0].contents & 0x7F) == 0x7F)
            {   sprintf(gtempstring,  "%3.1f", ((ioport[0].contents & 0x7F) / 127.0) * 100.0);
            } else
            {   sprintf(gtempstring, "~%3.1f", ((ioport[0].contents & 0x7F) / 127.0) * 100.0);
            }
            st_set(SUBWINDOW_INDUSTRIAL, IDC_PITCHSPEEDTEXT);
        }
        if ((ioport[0].contents & 0x80) != (oldpitchspeed & 0x80) || force)
        {   ch_set(SUBWINDOW_INDUSTRIAL, IDC_PITCHDIRECTION, (ioport[0].contents & 0x80) ? 1 : 0);
        }
        oldpitchspeed = ioport[0].contents;

        if ((ioport[1].contents & 0x7F) != (oldyawmotor & 0x7F) || force)
        {   sl_set(SUBWINDOW_INDUSTRIAL, IDC_YAWSPEED, ioport[1].contents & 0x7F);
            if ((ioport[1].contents & 0x7F) == 0 || (ioport[1].contents & 0x7F) == 0x7F)
            {   sprintf(gtempstring,  "%3.1f", ((ioport[1].contents & 0x7F) / 127.0) * 100.0);
            } else
            {   sprintf(gtempstring, "~%3.1f", ((ioport[1].contents & 0x7F) / 127.0) * 100.0);
            }
            st_set(SUBWINDOW_INDUSTRIAL, IDC_YAWSPEEDTEXT);
        }
        if ((ioport[1].contents & 0x80) != (oldyawmotor & 0x80) || force)
        {   ch_set(SUBWINDOW_INDUSTRIAL, IDC_YAWMOTORDIR, (ioport[1].contents & 0x80) ? 1 : 0);
        }
        oldyawmotor = ioport[1].contents;

        if ((ioport[4].contents & 1) != oldyawmotormode || force)
        {   ch_set(SUBWINDOW_INDUSTRIAL, IDC_YAWMOTORMODE, (ioport[4].contents & 0x01) ? 1 : 0);
        }
        oldyawmotormode = ioport[4].contents;

        if (ioport[2].contents != oldfieldcurrent || force)
        {   sl_set(SUBWINDOW_INDUSTRIAL, IDC_FIELDCURRENT, ioport[2].contents);
            sprintf(gtempstring, "%3.1f", 3.9 * ioport[2].contents);
            st_set(SUBWINDOW_INDUSTRIAL, IDC_FIELDCURRENTTEXT);
            oldfieldcurrent = ioport[2].contents;
        }

        if (rotorspeed != oldrotorspeed || force)
        {   sprintf(gtempstring, "%d", rotorspeed);
            st_set(SUBWINDOW_INDUSTRIAL, IDC_ROTORSPEED);
            oldrotorspeed = rotorspeed;
        }

        if (pitchangle != oldpitchangle || force)
        {   sl_set(SUBWINDOW_INDUSTRIAL, IDC_PITCHANGLE, pitchangle);
            sprintf(gtempstring, "%d", pitchangle);
            st_set(SUBWINDOW_INDUSTRIAL, IDC_PITCHANGLETEXT);
            oldpitchangle = pitchangle;
        }

        if (yawsensordir != oldyawsensordir || force)
        {   sl_set(SUBWINDOW_INDUSTRIAL, IDC_YAWSENSORDIR, yawsensordir);
            if (yawsensordir == 0 || yawsensordir == 255)
            {   sprintf(gtempstring, "%d", (int) ((yawsensordir / 255.0) * 360.0));
            } else
            {   sprintf(gtempstring, "~%d", (int) ((yawsensordir / 255.0) * 360.0));
            }
            st_set(SUBWINDOW_INDUSTRIAL, IDC_YAWSENSORDIRTEXT);
            oldyawsensordir = yawsensordir;
        }

        if (windspeed != oldwindspeed || force)
        {   sl_set(SUBWINDOW_INDUSTRIAL, IDC_WINDSPEED, windspeed);
            sprintf(gtempstring, "%d", windspeed);
            st_set(SUBWINDOW_INDUSTRIAL, IDC_WINDSPEEDTEXT);
            oldwindspeed = windspeed;
        }

        if (winddirection != oldwinddirection || force)
        {   sl_set(SUBWINDOW_INDUSTRIAL, IDC_WINDDIRECTION, winddirection);
            if (winddirection == 0 || winddirection == 255)
            {   sprintf(gtempstring, "%d", (int) ((winddirection / 255.0) * 360.0));
            } else
            {   sprintf(gtempstring, "~%d", (int) ((winddirection / 255.0) * 360.0));
            }
            st_set(SUBWINDOW_INDUSTRIAL, IDC_WINDDIRECTIONTEXT);
            oldwinddirection = winddirection;
        }

        if (mechpower_kw != oldmechpower || force)
        {   sprintf(gtempstring, "%.3f", mechpower_kw);
            st_set(SUBWINDOW_INDUSTRIAL, IDC_MECHPOWER);
            oldmechpower = mechpower_kw;
        }

        if (elecpower_kw != oldelecpower || force)
        {   sprintf(gtempstring, "%.3f", elecpower_kw);
            st_set(SUBWINDOW_INDUSTRIAL, IDC_ELECPOWER);
            oldelecpower = elecpower_kw;
        }
    acase PERIPH_LINEARISATIE:
        if
        (   linearx != oldlinearx
         || lineary != oldlineary
         || linearu != oldlinearu
         || linearv != oldlinearv
         || force
        )
        {   redraw_furnace();
        }

        if (linearx != oldlinearx || force)
        {   sl_set(SUBWINDOW_INDUSTRIAL, IDC_LINEARX, linearx);
            sprintf(gtempstring, "%+.3f", (linearx - 2048.0) * 0.488);
            st_set(SUBWINDOW_INDUSTRIAL, IDC_LINEARXTEXT);
            oldlinearx = linearx;
        }
        if (lineary != oldlineary || force)
        {   sl_set(SUBWINDOW_INDUSTRIAL, IDC_LINEARY, lineary);
            sprintf(gtempstring, "%+.3f", (lineary - 2048.0) * 0.488);
            st_set(SUBWINDOW_INDUSTRIAL, IDC_LINEARYTEXT);
            oldlineary = lineary;
        }
        if (linearu != oldlinearu || force)
        {   sl_set(SUBWINDOW_INDUSTRIAL, IDC_LINEARU, linearu);
            sprintf(gtempstring, "%+.3f", (linearu - 2048.0) * 0.488);
            st_set(SUBWINDOW_INDUSTRIAL, IDC_LINEARUTEXT);
            oldlinearu = linearu;
        }
        if (linearv != oldlinearv || force)
        {   sl_set(SUBWINDOW_INDUSTRIAL, IDC_LINEARV, linearv);
            sprintf(gtempstring, "%+.3f", (linearv - 2048.0) * 0.488);
            st_set(SUBWINDOW_INDUSTRIAL, IDC_LINEARVTEXT);
            oldlinearv = linearv;
        }
    acase PERIPH_MAGNETOMETER:
        if
        (   magneticx     != oldmagnetx
         || magneticy     != oldmagnety
         || magneticz     != oldmagnetz
         || planepitch    != oldplanepitch
         || planeroll     != oldplaneroll
         || force
        )
        {   planex          = (magneticx  - 2048.0) * 0.292;
            planey          = (magneticy  - 2048.0) * 0.292;
            planez          = (magneticz  - 2048.0) * 0.292;
            planepitch_deg  = (planepitch - 2048.0) * 0.35156;
            planepitch_rad  = planepitch_deg * PI / 180.0;
            planeroll_deg   = (planeroll  - 2048.0) * 0.35156;
            planeroll_rad   = planeroll_deg * PI / 180.0;
            rotate_vector();
#ifdef CONTROLYAW
            planeyaw_deg    = (magneticx  - 2048.0) * 0.35156;
            planeyaw_rad    = planeyaw_deg * PI / 180.0;
#else
            planeyaw_rad    = atan2(earthy, earthx);
            planeyaw_deg    = planeyaw_rad * 180.0 / PI;
#endif
            redraw_furnace();

            sprintf(gtempstring, "%f", earthx);
            st_set(SUBWINDOW_INDUSTRIAL, IDC_EARTHX);
            sprintf(gtempstring, "%f", earthy);
            st_set(SUBWINDOW_INDUSTRIAL, IDC_EARTHY);
            sprintf(gtempstring, "%f", earthz);
            st_set(SUBWINDOW_INDUSTRIAL, IDC_EARTHZ);
            sprintf(gtempstring, "%+.5f", planeyaw_deg);
            st_set(SUBWINDOW_INDUSTRIAL, IDC_PLANEYAW);

            inclination_rad = atan2(earthz, sqrt((earthx * earthx) + (earthy * earthy)));
            latitude_rad    = inclination_rad;
            latitude_deg    = latitude_rad * 180.0 / PI;
            if (latitude_deg > 0)
            {   sprintf(gtempstring, "%f %s",  latitude_deg, LLL(MSG_NORTH, "N"));
            } elif (latitude_deg < 0)
            {   sprintf(gtempstring, "%f %s", -latitude_deg, LLL(MSG_SOUTH, "S"));
            } else
            {   sprintf(gtempstring, "0.0");
            }
            st_set(SUBWINDOW_INDUSTRIAL, IDC_LATITUDE1);

            if (earthz < -POLESTRENGTH)
            {   latitude_deg = -90.0; // 90 degrees south
            } elif (earthz > POLESTRENGTH)
            {   latitude_deg =  90.0; // 90 degrees north
            } else
            {   latitude_rad = asin(earthz / POLESTRENGTH); // argument to asin() must be in -1..+1 range
                latitude_deg = latitude_rad * 180.0 / PI;
            }
            if (latitude_deg > 0)
            {   sprintf(gtempstring, "%f %s",  latitude_deg, LLL(MSG_NORTH, "N"));
            } elif (latitude_deg < 0)
            {   sprintf(gtempstring, "%f %s", -latitude_deg, LLL(MSG_SOUTH, "S"));
            } else
            {   sprintf(gtempstring, "0.0");
            }
            st_set(SUBWINDOW_INDUSTRIAL, IDC_LATITUDE2);
        }

        if (magneticx != oldmagnetx || force)
        {   sl_set(SUBWINDOW_INDUSTRIAL, IDC_MAGNETICX, magneticx);
#ifdef USEMV
            sprintf(gtempstring, "%+.3f", (magneticx * 1.221)); // mV ($FFFF is 4999.995 mV)
#else
            sprintf(gtempstring, "%+.3f", planex);
#endif
            st_set(SUBWINDOW_INDUSTRIAL, IDC_MAGNETICXTEXT);
            oldmagnetx = magneticx;
        }
        if (magneticy != oldmagnety || force)
        {   sl_set(SUBWINDOW_INDUSTRIAL, IDC_MAGNETICY, magneticy);
#ifdef USEMV
            sprintf(gtempstring, "%+.3f", (magneticy * 1.221)); // mV ($FFFF is 4999.995 mV)
#else
            sprintf(gtempstring, "%+.3f", planey);
#endif
            st_set(SUBWINDOW_INDUSTRIAL, IDC_MAGNETICYTEXT);
            oldmagnety = magneticy;
        }
        if (magneticz != oldmagnetz || force)
        {   sl_set(SUBWINDOW_INDUSTRIAL, IDC_MAGNETICZ, magneticz);
#ifdef USEMV
            sprintf(gtempstring, "%+.3f", (magneticz * 1.221)); // mV ($FFFF is 4999.995 mV)
#else
            sprintf(gtempstring, "%+.3f", planez);
#endif
            st_set(SUBWINDOW_INDUSTRIAL, IDC_MAGNETICZTEXT);
            oldmagnetz = magneticz;
        }
        if (planepitch != oldplanepitch || force)
        {   sl_set(SUBWINDOW_INDUSTRIAL, IDC_PLANEPITCH, planepitch);
            sprintf(gtempstring, "%+.5f", (planepitch - 2048.0) * 0.35156);
            st_set(SUBWINDOW_INDUSTRIAL, IDC_PLANEPITCHTEXT);
            oldplanepitch = planepitch;
        }
        if (planeroll != oldplaneroll || force)
        {   sl_set(SUBWINDOW_INDUSTRIAL, IDC_PLANEROLL, planeroll);
            sprintf(gtempstring, "%+.5f", (planeroll - 2048.0) * 0.35156);
            st_set(SUBWINDOW_INDUSTRIAL, IDC_PLANEROLLTEXT);
            oldplaneroll = planeroll;
}   }   }

EXPORT void pipbug_redrawleds(void)
{   int i;

    for (i = 0 ; i < 4; i++)
    {   drawsegments(i);
    }   

    drawdigit(       4, (UBYTE)  ((255 - ioport[1].contents) & 0x0F));
    drawdigit(       5, (UBYTE) (((255 - ioport[2].contents) & 0xF0) >> 4));
    drawdigit(       6, (UBYTE)  ((255 - ioport[2].contents) & 0x0F));

    if (pipbug_vdu != VDU_LCVDU_NARROW)
    {   if (drawunlit)
        {   drawsegment( 7, 6, (ioport[16].contents == 0x03 || ioport[16].contents == 0x08) ? RED : UNLIT);
            drawsegment(10, 6, (ioport[16].contents == 0x00 || ioport[16].contents == 0x08) ? RED : UNLIT);
        }

        drawdigit(   8, (UBYTE) (((255 - ioport[4].contents) & 0xF0) >> 4));
        drawdigit(   9, (UBYTE)  ((255 - ioport[4].contents) & 0x0F));

        drawdigit(  11, (UBYTE) (((255 - ioport[8].contents) & 0xF0) >> 4));
        drawdigit(  12, (UBYTE)  ((255 - ioport[8].contents) & 0x0F));
}   }

#define FURNACECENTREX  64
#define FURNACECENTREY  64
#define LINEARCENTREX   64
#define LINEARCENTREY   64
#define VECTORCENTREX   32
#define VECTORCENTREY   32
#define EARTHCENTREX    96
#define EARTHCENTREY    32
#define SIDECENTREX     32
#define SIDECENTREY     96
#define TOPCENTREX      64
#define TOPCENTREY      64
#define BACKCENTREX     96
#define BACKCENTREY     96
EXPORT void redraw_furnace(void)
{   FAST double  a1_deg,    a1_rad,
                 angle_deg, angle_rad,
                 arrowlen,
                 mag;
    FAST int     x1, x2, x3, x4,
                 y1, y2, y3, y4;
#ifdef WIN32
    FAST HDC     IndustrialRastPtr;
    FAST ULONG   windcolour;
#endif
#ifdef AMIGA
    FAST UBYTE   windcolour;
#endif

    for (y1 = 0; y1 < INDUSTRIALHEIGHT; y1++)
    {   for (x1 = 0; x1 < INDUSTRIALWIDTH; x1++)
        {   DRAWINDUSTRIALPIXEL(x1, y1, EMURGBPEN_BLACK);
    }   }

    switch (pipbug_periph)
    {
    case PERIPH_FURNACE:
        // Hub------------------------------------------------------------

        angle_deg = (yawsensordir / 255.0) * 360.0;
        angle_rad = angle_deg * PI / 180.0;
        x1        = FURNACECENTREX + (int) (24.0 * cos(angle_rad));
        y1        = FURNACECENTREY + (int) (24.0 * sin(angle_rad));
        x2        = FURNACECENTREX - (int) (24.0 * cos(angle_rad));
        y2        = FURNACECENTREY - (int) (24.0 * sin(angle_rad));
        draw_line(x1, y1, x2, y2, EMURGBPEN_WHITE);

        // Blades---------------------------------------------------------

        a1_deg    = (double) pitchangle;
        a1_rad    = a1_deg * PI / 180.0;
        draw_line(x1, y1, x1 + (int) (10.0 * cos(angle_rad - a1_rad)), y1 + (int) (10.0 * sin(angle_rad - a1_rad)), EMURGBPEN_PURPLE);
        draw_line(x2, y2, x2 - (int) (10.0 * cos(angle_rad + a1_rad)), y2 - (int) (10.0 * sin(angle_rad + a1_rad)), EMURGBPEN_PURPLE);

        // Rotor----------------------------------------------------------

        a1_deg    = angle_deg + 270.0;
        a1_rad    = a1_deg * PI / 180.0;
        arrowlen  = (rotorspeed / MAX_RPM) * 24.0;
        x1        = FURNACECENTREX;
        y1        = FURNACECENTREY;
        x2        = FURNACECENTREX + (int) (arrowlen * cos(a1_rad));
        y2        = FURNACECENTREY + (int) (arrowlen * sin(a1_rad));
        if (arrowlen >= 1.0)
        {   draw_line(x1, y1, x2, y2, EMURGBPEN_GREEN);
        }

        // Hub yaw motor--------------------------------------------------

        arrowlen  = ((ioport[1].contents & 0x7F) / 127.0) * 24.0;
        if (ioport[1].contents & 0x80) // clockwise
        {   x1    = FURNACECENTREX - (int) (24.0 * cos(angle_rad));
            y1    = FURNACECENTREY - (int) (24.0 * sin(angle_rad));
        } else // anticlockwise
        {   x1    = FURNACECENTREX + (int) (24.0 * cos(angle_rad));
            y1    = FURNACECENTREY + (int) (24.0 * sin(angle_rad));
        }
        if (ioport[4].contents & 1) // drive
        {   x2        = x1 - (int) (arrowlen * sin(angle_rad));
            y2        = y1 + (int) (arrowlen * cos(angle_rad));
            if (arrowlen >= 1.0)
            {   draw_line(x1, y1, x2, y2, EMURGBPEN_YELLOW);
        }   }
        a1_deg    = angle_deg + 45;
        a1_rad    = a1_deg * PI / 180.0;
        x3        = x1 - (int) (10.0 * sin(a1_rad));
        y3        = y1 + (int) (10.0 * cos(a1_rad));
        draw_line(x1, y1, x3, y3, EMURGBPEN_YELLOW);
        a1_deg    = angle_deg - 45;
        a1_rad    = a1_deg * PI / 180.0;
        x4        = x1 - (int) (10.0 * sin(a1_rad));
        y4        = y1 + (int) (10.0 * cos(a1_rad));
        draw_line(x1, y1, x4, y4, EMURGBPEN_YELLOW);
        if (!(ioport[4].contents & 1)) // damp
        {   draw_line(x3, y3, x4, y4, EMURGBPEN_YELLOW);
        }

        // Blade pitch motor----------------------------------------------

        if (ioport[1].contents & 0x80) // clockwise
        {   a1_deg = angle_deg + 90;
            a1_rad = a1_deg * PI / 180.0;
            x1     = FURNACECENTREX + (int) (24.0 * cos(angle_rad));
            y1     = FURNACECENTREY + (int) (24.0 * sin(angle_rad));
        } else // anticlockwise
        {   a1_deg = angle_deg - 90;
            a1_rad = a1_deg * PI / 180.0;
            x1     = FURNACECENTREX - (int) (24.0 * cos(angle_rad));
            y1     = FURNACECENTREY - (int) (24.0 * sin(angle_rad));
        }

        arrowlen   = ((ioport[0].contents & 0x7F) / 127.0) * 24.0;
        if (ioport[0].contents & 0x80) // feathering
        {   a1_deg = angle_deg + 270;
        } else
        {   a1_deg = angle_deg +  90;
        }
        a1_rad     = a1_deg * PI / 180.0;
        x2         = x1 - (int) (arrowlen * cos(a1_rad));
        y2         = y1 - (int) (arrowlen * sin(a1_rad));
        if (arrowlen >= 1.0)
        {   draw_line(x1, y1, x2, y2, EMURGBPEN_CYAN);
        }

        if (ioport[0].contents & 0x80) // feathering
        {   a1_deg = angle_deg + 225;
        } else
        {   a1_deg = angle_deg +  45;
        }
        a1_rad     = a1_deg * PI / 180.0;
        x2         = x1 - (int) (10.0 * cos(a1_rad));
        y2         = y1 - (int) (10.0 * sin(a1_rad));
        draw_line(x1, y1, x2, y2, EMURGBPEN_CYAN);
        if (ioport[0].contents & 0x80) // feathering
        {   a1_deg = angle_deg + 315;
        } else
        {   a1_deg = angle_deg + 135;
        }
        a1_rad     = a1_deg * PI / 180.0;
        x2         = x1 - (int) (10.0 * cos(a1_rad));
        y2         = y1 - (int) (10.0 * sin(a1_rad));
        draw_line(x1, y1, x2, y2, EMURGBPEN_CYAN);

        // Wind-----------------------------------------------------------

        if (windspeed < 50)
        {   arrowlen   = (double) (windspeed / 2.0);
            windcolour = EMURGBPEN_BLUE;
        } else
        {   arrowlen   = 25;
            windcolour = EMURGBPEN_RED;
        }
        angle_deg = (winddirection / 255.0) * 360.0;
        angle_rad = angle_deg * PI / 180.0;
        x1 = FURNACECENTREX + (int) (32.0 * sin(angle_rad)); // more central point
        y1 = FURNACECENTREY - (int) (32.0 * cos(angle_rad));
        x2 = x1 + (int) (arrowlen * sin(angle_rad)); // less central point
        y2 = y1 - (int) (arrowlen * cos(angle_rad));
        if (arrowlen >= 1.0)
        {   draw_line(x1, y1, x2, y2, windcolour);
        }

        a1_deg     = angle_deg + 135;
        a1_rad     = a1_deg * PI / 180.0;
        x3 = x1 - (int) (10.0 * sin(a1_rad));
        y3 = y1 + (int) (10.0 * cos(a1_rad));
        draw_line(x1, y1, x3, y3, windcolour);

        a1_deg     = angle_deg - 135;
        a1_rad     = a1_deg * PI / 180.0;
        x4 = x1 - (int) (10.0 * sin(a1_rad));
        y4 = y1 + (int) (10.0 * cos(a1_rad));
        draw_line(x1, y1, x4, y4, windcolour);
    acase PERIPH_LINEARISATIE:
        for (y1 = -2; y1 <= 2; y1++)
        {   for (x1 = -2; x1 <= 2; x1++)
            {   x2 = LINEARCENTREX + x1 + (int) ((linearx - 2048.0) * 0.488 * 0.064);
                y2 = LINEARCENTREY + y1 - (int) ((lineary - 2048.0) * 0.488 * 0.064);
                DRAWINDUSTRIALPIXEL(x2, y2, EMURGBPEN_RED);
        }   }
        for (y1 = -2; y1 <= 2; y1++)
        {   for (x1 = -2; x1 <= 2; x1++)
            {   x2 = LINEARCENTREX + x1 + (int) ((linearu - 2048.0) * 0.488 * 0.064);
                y2 = LINEARCENTREY + y1 - (int) ((linearv - 2048.0) * 0.488 * 0.064);
                DRAWINDUSTRIALPIXEL(x2, y2, EMURGBPEN_BLUE);
        }   }
    acase PERIPH_MAGNETOMETER:
        // Plane (side view)----------------------------------------------

        // fuselage: pitch only
        x1 = SIDECENTREX - (int) (24.0 * cos(planepitch_rad));
        y1 = SIDECENTREY - (int) (24.0 * sin(planepitch_rad));
        x2 = SIDECENTREX + (int) (24.0 * cos(planepitch_rad));
        y2 = SIDECENTREY + (int) (24.0 * sin(planepitch_rad));
        draw_line(SIDECENTREX, SIDECENTREY, x1, y1, EMURGBPEN_YELLOW);
        draw_line(SIDECENTREX, SIDECENTREY, x2, y2, EMURGBPEN_DARKBLUE);

        // wings: pitch and roll
        angle_rad = planepitch_rad + PI / 2.0;
        arrowlen = 18.0 * sin(planeroll_rad);
        x3 = (int) (arrowlen * cos(angle_rad));
        y3 = (int) (arrowlen * sin(angle_rad));
        x1 = SIDECENTREX - x3;
        y1 = SIDECENTREY - y3;
        x2 = SIDECENTREX + x3;
        y2 = SIDECENTREY + y3;
        draw_line(SIDECENTREX, SIDECENTREY, x1, y1, EMURGBPEN_RED); // positive roll (0..180) means left wing up, right wing down
        draw_line(SIDECENTREX, SIDECENTREY, x2, y2, EMURGBPEN_GREEN);

        // Plane (top view)-----------------------------------------------

        // fuselage: yaw only
        x1 = (int) ( 24.0 * sin(planeyaw_rad));
        y1 = (int) (-24.0 * cos(planeyaw_rad));
        draw_line(TOPCENTREX, TOPCENTREY, TOPCENTREX + x1, TOPCENTREY + y1, EMURGBPEN_YELLOW);
        draw_line(TOPCENTREX, TOPCENTREY, TOPCENTREX - x1, TOPCENTREY - y1, EMURGBPEN_DARKBLUE);

        // wings: yaw and roll
        arrowlen = 18.0 * cos(planeroll_rad);
        x1 = (int) (arrowlen * cos(planeyaw_rad));
        y1 = (int) (arrowlen * sin(planeyaw_rad));
        draw_line(TOPCENTREX, TOPCENTREY, TOPCENTREX + x1, TOPCENTREY + y1, EMURGBPEN_GREEN); // right wing is green
        draw_line(TOPCENTREX, TOPCENTREY, TOPCENTREX - x1, TOPCENTREY - y1, EMURGBPEN_RED); // left wing is red

        // Plane (back view)----------------------------------------------

        // fuselage: pitch and roll
        angle_rad = planeroll_rad + PI / 2.0;
        arrowlen = 24.0 * sin(planepitch_rad);
        x3 = (int) (arrowlen * cos(angle_rad));
        y3 = (int) (arrowlen * sin(angle_rad));
        x1 = BACKCENTREX - x3;
        y1 = BACKCENTREY - y3;
        x2 = BACKCENTREX + x3;
        y2 = BACKCENTREY + y3;
        draw_line(BACKCENTREX, BACKCENTREY, x1, y1, EMURGBPEN_YELLOW);
        draw_line(BACKCENTREX, BACKCENTREY, x2, y2, EMURGBPEN_DARKBLUE);

        // wings: roll only
        x3 = (int) (18.0 * cos(planeroll_rad));
        y3 = (int) (18.0 * sin(planeroll_rad));
        x1 = BACKCENTREX - x3;
        y1 = BACKCENTREY - y3;
        x2 = BACKCENTREX + x3;
        y2 = BACKCENTREY + y3;
        draw_line(BACKCENTREX, BACKCENTREY, x1, y1, EMURGBPEN_RED);
        draw_line(BACKCENTREX, BACKCENTREY, x2, y2, EMURGBPEN_GREEN);

        // Magnetometer coordinates for plane-----------------------------

        angle_rad = atan2(planey, planex);
        angle_deg = angle_rad * 180 / PI;
        mag = sqrt((planex * planex) + (planey * planey));
        arrowlen = (mag / 600.0) * 24.0;
        x2 = VECTORCENTREX + (int) (arrowlen * cos(angle_rad));
        y2 = VECTORCENTREY - (int) (arrowlen * sin(angle_rad));
        if (arrowlen >= 1.0)
        {   draw_line(VECTORCENTREX, VECTORCENTREY, x2, y2, EMURGBPEN_YELLOW);
        }

        a1_deg = angle_deg + 135;
        a1_rad = a1_deg * PI / 180.0;
        x3     = x2 + (int) (5.0 * cos(a1_rad));
        y3     = y2 - (int) (5.0 * sin(a1_rad));
        draw_line(x2, y2, x3, y3, EMURGBPEN_YELLOW);

        a1_deg = angle_deg - 135;
        a1_rad = a1_deg * PI / 180.0;
        x3     = x2 + (int) (5.0 * cos(a1_rad));
        y3     = y2 - (int) (5.0 * sin(a1_rad));
        draw_line(x2, y2, x3, y3, EMURGBPEN_YELLOW);

        arrowlen = ((planez / 600.0) * 24.0);
        if (arrowlen <= -1.0)
        {   draw_line(VECTORCENTREX, VECTORCENTREY, VECTORCENTREX, VECTORCENTREY - (int) arrowlen, EMURGBPEN_RED);
        } elif (arrowlen >= 1.0)
        {   draw_line(VECTORCENTREX, VECTORCENTREY, VECTORCENTREX, VECTORCENTREY - (int) arrowlen, EMURGBPEN_DARKBLUE);
        }

        // Magnetometer coordinates for Earth-----------------------------

        angle_rad = atan2(earthy, earthx);
        angle_deg = angle_rad * 180 / PI;
        mag = sqrt((earthx * earthx) + (earthy * earthy));
        arrowlen = (mag / 600.0) * 24.0;
        x2 = EARTHCENTREX + (int) (arrowlen * cos(angle_rad));
        y2 = EARTHCENTREY - (int) (arrowlen * sin(angle_rad));
        if (arrowlen >= 1.0)
        {   draw_line(EARTHCENTREX, EARTHCENTREY, x2, y2, EMURGBPEN_WHITE);
        }

        a1_deg = angle_deg + 135;
        a1_rad = a1_deg * PI / 180.0;
        x3     = x2 + (int) (5.0 * cos(a1_rad));
        y3     = y2 - (int) (5.0 * sin(a1_rad));
        draw_line(x2, y2, x3, y3, EMURGBPEN_WHITE);

        a1_deg = angle_deg - 135;
        a1_rad = a1_deg * PI / 180.0;
        x3     = x2 + (int) (5.0 * cos(a1_rad));
        y3     = y2 - (int) (5.0 * sin(a1_rad));
        draw_line(x2, y2, x3, y3, EMURGBPEN_WHITE);

        arrowlen = ((earthz / 600.0) * 24.0);
        if (declinate)
        {   x1 = (int) (arrowlen * sin(TILT_RAD));
            y1 = (int) (arrowlen * cos(TILT_RAD));
        } else
        {   x1 = (int)  0;
            y1 = (int)  arrowlen;
        }
        if (arrowlen <= -1.0)
        {   draw_line(EARTHCENTREX, EARTHCENTREY, EARTHCENTREX - x1, EARTHCENTREY - y1, EMURGBPEN_RED);
        } elif (arrowlen >= 1.0)
        {   draw_line(EARTHCENTREX, EARTHCENTREY, EARTHCENTREX - x1, EARTHCENTREY - y1, EMURGBPEN_DARKBLUE);
    }   }

#ifdef WIN32
    IndustrialRastPtr = GetDC(GetDlgItem(SubWindowPtr[SUBWINDOW_INDUSTRIAL], IDC_WFC));
    DISCARD StretchDIBits
    (   IndustrialRastPtr,
        0,              // dest leftx
        0,              // dest topy
        INDUSTRIALWIDTH,   // dest width
        INDUSTRIALHEIGHT,  // dest height
        0,              // source leftx
        0,              // source topy
        INDUSTRIALWIDTH,   // source width
        INDUSTRIALHEIGHT,  // source height
        canvasdisplay[CANVAS_INDUSTRIAL], // pointer to the bits
        (const struct tagBITMAPINFO*) &CanvasBitMapInfo[CANVAS_INDUSTRIAL], // pointer to BITMAPINFO structure
        DIB_RGB_COLORS, // format of data
        SRCCOPY         // blit mode
    );
    ReleaseDC(GetDlgItem(SubWindowPtr[SUBWINDOW_INDUSTRIAL], IDC_WFC), IndustrialRastPtr);
#endif
#ifdef AMIGA
    DISCARD WritePixelArray8
    (   SubWindowPtr[SUBWINDOW_INDUSTRIAL]->RPort,
        (ULONG) (gadgets[GID_IN_SP1]->LeftEdge                       ), // dest leftx
        (ULONG) (gadgets[GID_IN_SP1]->TopEdge                        ), // dest topy
        (ULONG) (gadgets[GID_IN_SP1]->LeftEdge + INDUSTRIALWIDTH  - 1), // dest rightx
        (ULONG) (gadgets[GID_IN_SP1]->TopEdge  + INDUSTRIALHEIGHT - 1), // dest bottomy
        canvasdisplay[CANVAS_INDUSTRIAL],
        &wpa8canvasrastport[CANVAS_INDUSTRIAL]
    );
#endif
}

MODULE void draw_line(int x1, int y1, int x2, int y2, ULONG colour)
{   FAST int e2,
             dx,
             dy,
             sx,
             sy,
             err;

    dx  = abs(x2 - x1);
    dy  = abs(y2 - y1);
    sx  = (x1 < x2) ? 1 : -1;
    sy  = (y1 < y2) ? 1 : -1;
    err = dx - dy;

    for (;;)
    {   DRAWINDUSTRIALPIXEL(x1, y1, colour);

        if (x1 == x2 && y1 == y2)
        {   break;
        }

        e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx)  { err += dx; y1 += sy; }
}   }

EXPORT void industrial_reset(void)
{   switch (pipbug_periph)
    {
    case PERIPH_FURNACE:
        ioport[0].contents = // blade pitch motor speed of 0, direction of 0
        ioport[1].contents = // hub   yaw   motor speed of 0, direction of 0
        ioport[2].contents = // generator field current of 0
        ioport[4].contents = // hub   yaw   motor mode  of 0
        pitchangle         =
        yawsensordir       =
        windspeed          =
        winddirection      = 0;
        // calculated ones
        rotorspeed         = 0;
        mechpower_kw       =
        elecpower_kw       = 0.0;
        update_industrial(FALSE);
    acase PERIPH_LINEARISATIE:
        linearx = lineary = linearu = linearv = 2048;
        update_industrial(FALSE);
    acase PERIPH_MAGNETOMETER:
        magneticx = magneticy = magneticz = planepitch = planeroll = 2048;
        update_industrial(TRUE);
}   }

// Applies the roll and pitch angles to rotate the magnetometer's magnetic field components from the aircraft frame to the Earth frame
MODULE void rotate_vector(void)
{   // Compute rotation matrix for roll and pitch
    double cos_phi,
           sin_phi,
           cos_theta,
           sin_theta;

    cos_phi   = cos(planeroll_rad),
    sin_phi   = sin(planeroll_rad);
    cos_theta = cos(planepitch_rad),
    sin_theta = sin(planepitch_rad);

    // rotation matrix for pitch and roll (no yaw)
    earthx =  cos_theta * planex + sin_theta * (sin_phi * planey + cos_phi * planez);
    earthy =  cos_phi   * planey - sin_phi                                 * planez ;
    earthz = -sin_theta * planex + cos_theta * (sin_phi * planey + cos_phi * planez);

    if (declinate)
    {   earthx = (earthx * cos(TILT_RAD)) - (earthy * sin(TILT_RAD));
        earthy = (earthx * sin(TILT_RAD)) + (earthy * cos(TILT_RAD));
}   }

