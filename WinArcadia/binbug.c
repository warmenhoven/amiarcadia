// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
    #include <string.h>
    #include <stdlib.h>       // rand()
    #include <proto/locale.h> // GetCatalogStr()
#endif
#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
#endif

#include <stdio.h>
#include <ctype.h>            // toupper()

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"

#include "binbug.h"

// DEFINES----------------------------------------------------------------

// #define NINTHPIXEL
// whether eg. the 8th X-pixel is duplicated into the 9th X-pixel

#define BINBUG_FD1771_COMMAND  0xC // used for command AND status!
#define BINBUG_FD1771_STATUS   0xC // used for command AND status!
#define BINBUG_FD1771_TRACK    0xD
#define BINBUG_FD1771_SECTOR   0xE
#define BINBUG_FD1771_DATA     0xF
#define BINBUG_FD1771_CONTROL 0x10

#define DOCPU                 \
if (cpux == nextinst)         \
{   oldcycles = cycles_2650;  \
    checkstep();              \
    pipbin_io();              \
    one_instruction();        \
    if (fastbinbug) nextinst += (cycles_2650 - oldcycles) * 6; else nextinst += (cycles_2650 - oldcycles) * 12; \
    if (nextinst >= 768)      \
    {   nextinst -= 768;      \
}   }
// nextinst range is -128..383

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       UBYTE                    binbug_joyunit;
EXPORT       int                      binbug_baudrate   = BINBUG_BAUDRATE_150,
                                      binbug_dosver     = DOS_VHSDOS,
                                      binbug_userdrives = 1, // to avoid needing a disk in 2nd drive when loading Adventure game
                                      coomer            = FALSE,
                                      fastbinbug        = FALSE,
                                      firstdosequiv,       lastdosequiv,
                                      firstdoscodecomment, lastdoscodecomment,
                                      firstdosdatacomment, lastdosdatacomment;

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       FLAG                     capslock,
                                      inframe,
                                      lmb, mmb, rmb,
                                      multimode,
                                      paperreaderenabled,
                                      paperpunchenabled;
IMPORT       TEXT                     asciiname_short[259][3 + 1],
                                      file_game[MAX_PATH + 1],
                                      gtempstring[256 + 1],
                                      path_disks[MAX_PATH + 1];
IMPORT       UBYTE                    cosversion,
                                      drivestatus,
                                      glow,
                                      keys_column[7],
                                      memory[32768],
                                      psu,
                                      reqsector,
                                      startaddr_h,
                                      startaddr_l,
                                      tapebyte,
                                      tapeskewage,
                                      trackreg,
                                      tt_kybdcode;
IMPORT       UWORD                    guestpitch[GUESTCHANNELS],
                                      iar,
                                      keypads[2][NUMKEYS],
                                      mirror_r[32768],
                                      mirror_w[32768],
                                      oldiar;
IMPORT       ULONG                    autofire[2],
                                      cpb,
                                      curdrive,
                                      cycles_2650,
                                      downframes,
                                      frames,
                                      jff[2],
                                      oldcycles,
                                      region,
                                      swapped,
                                      timeoutat,
                                      totalframes,
                                      tt_kybdtill,
                                      verbosedisk,
                                      verbosetape;
IMPORT       int                      ambient,
                                      ax[2],
                                      ay[4],
                                      belling[2],
                                      binbug_biosver,
                                      cpux,
                                      cpuy,
                                      debugdrive,
                                      drawmode,
                                      drawunlit,
                                      drive_mode,
                                      editscreen,
                                      framebased,
                                      game,
                                      hostcontroller[2],
                                      inverse,
                                      log_illegal,
                                      lowercase,
                                      machine,
                                      n1,
                                      nextinst,
                                      queuepos,
                                      requirebutton[2],
                                      scrnaddr,
                                      serializemode,
                                      slice_2650,
                                      stepdir,
                                      tapemode,
                                      tt_kybdstate,
                                      usemargins,
                                      viewingdrive,
                                      watchreads,
                                      watchwrites,
                                      whichgame,
                                      whichkeyrect;
IMPORT       struct DriveStruct       drive[DRIVES_MAX];
IMPORT       struct IOPortStruct      ioport[258];
IMPORT       struct KindStruct        filekind[KINDS];
IMPORT       struct MachineStruct     machines[MACHINES];
IMPORT       struct PrinterStruct     printer[2];
IMPORT       struct RTCStruct         rtc;
IMPORT const struct CodeCommentStruct codecomment[];
IMPORT       ASCREEN                  screen[BOXWIDTH][BOXHEIGHT];
IMPORT       MEMFLAG                  memflags[ALLTOKENS];
IMPORT       UBYTE*                   IOBuffer;
#ifdef WIN32
    IMPORT   int                      CatalogPtr;
    IMPORT   HWND                     SubWindowPtr[SUBWINDOWS];
#endif
#ifdef AMIGA
    IMPORT   struct Catalog*          CatalogPtr;
    IMPORT   struct Window*           SubWindowPtr[SUBWINDOWS];
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE       FLAG                     diskerror;
MODULE       UBYTE                    bgc         = BLACK,
                                      CMDBuffer[254],
                                      playerfire[2],
                                      ProgBuffer[254 * (BINBUG_DISKBLOCKS - 10)];
MODULE       UWORD                    chunkblock[BINBUG_DISKBLOCKS];

// MODULE FUNCTIONS-------------------------------------------------------

MODULE int bcd_to_normal(UBYTE value);
MODULE void binbug_playerinput(int source, int dest);
MODULE UWORD followchain(int startblock, int endblock, int thesize, int bamtype, FLAG isprog, FLAG quiet, int whichdrive);
MODULE UWORD showchunks(int numblocks);
MODULE void draw_binbug(void);
MODULE void run_cpu(int until);

// EXPORTED CODE----------------------------------------------------------

EXPORT void binbug_changebios(FLAG user)
{   int i;

 // tape_eject();
    if (user)
    {   reopen_subwindow(SUBWINDOW_TAPEDECK);
    }

    switch (binbug_biosver)
    {
    case BINBUG_GBUG:
        for (i = 0; i <= 0x3FF; i++)
        {   memory[i] = gbug[i];
        }
        memory[  0x438] =  0xFF;    // EDITFL
        memflags[0x438] |= NOWRITE; // GBUG writes $00 here at startup, but we want it to stay $FF so the editor is enabled
        for (i = 0x5800; i <= 0x5A67; i++)
        {   memory[  i] =  gbug[i - 0x5800 + 0x400];
            memflags[i] |= BIOS;
        }
    acase BINBUG_35:
    case BINBUG_36:
        for (i = 0; i <= 0x3FF; i++)
        {   memory[i] = binbug_bios[0][i];
        }
        memflags[0x438] &= ~(NOWRITE);
        for (i = 0x5800; i <= 0x5A67; i++)
        {   memflags[i] &= ~(BIOS);
        }
    acase BINBUG_MIKEBUG:
    case BINBUG_MYBUG:
        for (i = 0; i <= 0x3FF; i++)
        {   memory[i] = binbug_bios[1][i];
        }
        memflags[0x438] &= ~(NOWRITE);
        for (i = 0x5800; i <= 0x5A67; i++)
        {   memflags[i] &= ~(BIOS);
        }
    acase BINBUG_61:
        for (i = 0; i <= 0x3FF; i++)
        {   memory[i] = binbug_bios[2][i];
        }
        memflags[0x438] &= ~(NOWRITE);
        for (i = 0x5800; i <= 0x5A67; i++)
        {   memflags[i] &= ~(BIOS);
    }   }

    switch (binbug_biosver)
    {
    case BINBUG_35:
        memory[ 0xF7] = 0x01;
        memory[ 0xF8] = 0x6D;
        memory[0x16D] = 0x06;
        memory[0x16E] = 0xF7;
        memory[0x16F] = 0x0E;
        memory[0x170] = 0x63;
        memory[0x171] = 0x09;
        memory[0x172] = 0xC1;
        memory[0x173] = 0x3F;
        memory[0x174] = 0x02;
        memory[0x175] = 0x79;
        memory[0x176] = 0xDA;
        memory[0x177] = 0x77;
        memory[0x178] = 0x9B;
        memory[0x179] = 0x22;
        memory[0x17A] = 0xC0;
    acase BINBUG_36:
        memory[ 0xF7] = 0x00;
        memory[ 0xF8] = 0x1D;
        memory[0x16D] = 0x0D;
        memory[0x16E] = 0x04;
        memory[0x16F] = 0x0D;
        memory[0x170] = 0x3F;
        memory[0x171] = 0x02;
        memory[0x172] = 0x69;
        memory[0x173] = 0x0D;
        memory[0x174] = 0x04;
        memory[0x175] = 0x0E;
        memory[0x176] = 0x3F;
        memory[0x177] = 0x02;
        memory[0x178] = 0x69;
        memory[0x179] = 0x9B;
        memory[0x17A] = 0x22;
    acase BINBUG_MIKEBUG:
        memory[ 0x4F] = 0x84;
        memory[ 0x50] = 0x35;
        memory[ 0xF8] = 0x08;
        memory[0x172] = 0x0E;
    acase BINBUG_MYBUG:
        memory[ 0x4F] = 0x40;
        memory[ 0x50] = 0x09;
        memory[ 0xF8] = 0x06;
        memory[0x172] = 0x0C;
    }

    for (i = 0; i <= 0x7FFF; i++)
    {   memflags[i] &= ~(COMMENTED);
    }

    switch (binbug_biosver)
    {
    case BINBUG_36:
        machines[BINBUG].firstcodecomment = FIRSTBINBUG36CODECOMMENT;
        machines[BINBUG].lastcodecomment  = LASTBINBUG36CODECOMMENT;
        machines[BINBUG].firstdatacomment = FIRSTBINBUG36DATACOMMENT;
        machines[BINBUG].lastdatacomment  = LASTBINBUG36DATACOMMENT;
    acase BINBUG_61:
        machines[BINBUG].firstcodecomment = FIRSTBINBUG61CODECOMMENT;
        machines[BINBUG].lastcodecomment  = LASTBINBUG61CODECOMMENT;
        machines[BINBUG].firstdatacomment = FIRSTBINBUG61DATACOMMENT;
        machines[BINBUG].lastdatacomment  = LASTBINBUG61DATACOMMENT;
    acase BINBUG_GBUG:
        machines[BINBUG].firstcodecomment = FIRSTGBUGCODECOMMENT;
        machines[BINBUG].lastcodecomment  = LASTGBUGCODECOMMENT;
        machines[BINBUG].firstdatacomment = FIRSTGBUGDATACOMMENT;
        machines[BINBUG].lastdatacomment  = LASTGBUGDATACOMMENT;
    adefault:
        machines[BINBUG].firstcodecomment =
        machines[BINBUG].lastcodecomment  =
        machines[BINBUG].firstdatacomment =
        machines[BINBUG].lastdatacomment  = -1;
    }
    if (machines[machine].firstcodecomment != -1)
    {   for (i = machines[machine].firstcodecomment; i <= machines[machine].lastcodecomment; i++)
        {   memflags[codecomment[i].address] |= COMMENTED;
    }   }

    switch (binbug_biosver)
    {
    case BINBUG_35:
    case BINBUG_36:
        machines[BINBUG].firstequiv       = FIRSTBINBUGEQUIV;
        machines[BINBUG].lastequiv        = LASTBINBUGEQUIV;
    acase BINBUG_61:
        machines[BINBUG].firstequiv       = FIRSTBINBUG61EQUIV;
        machines[BINBUG].lastequiv        = LASTBINBUG61EQUIV;
    acase BINBUG_GBUG:
        machines[BINBUG].firstequiv       = FIRSTGBUGEQUIV;
        machines[BINBUG].lastequiv        = LASTGBUGEQUIV;
    acase BINBUG_MIKEBUG:
    case BINBUG_MYBUG:
        machines[BINBUG].firstequiv       = FIRSTNONBINBUGEQUIV;
        machines[BINBUG].lastequiv        = LASTNONBINBUGEQUIV;
    }

    switch (binbug_dosver)
    {
    case DOS_MICRODOS:
        for (i = 0; i <= 0x7E0; i++)
        {   memory[0x6800 + i] = microdos[i]; // $0..$7E0 -> $6800..$6FE0
        }
        for (i = 0x6FE1; i <= 0x6FFF; i++)
        {   memory[         i] =           0;
        }
        firstdosequiv       = FIRSTMICRODOSEQUIV;
        lastdosequiv        =  LASTMICRODOSEQUIV;
        firstdoscodecomment = FIRSTMICRODOSCODECOMMENT;
        lastdoscodecomment  =  LASTMICRODOSCODECOMMENT;
        firstdosdatacomment = FIRSTMICRODOSDATACOMMENT;
        lastdosdatacomment  =  LASTMICRODOSDATACOMMENT;
    acase DOS_VHSDOS:
        for (i = 0; i <= 0x7F9; i++)
        {   memory[0x6800 + i] = vhsdos[  i]; // $0..$7F9 -> $6800..$6FF9
        }
        for (i = 0x6FFA; i <= 0x6FFF; i++)
        {   memory[         i] =           0;
        }
        firstdosequiv       = FIRSTVHSDOSEQUIV;
        lastdosequiv        =  LASTVHSDOSEQUIV;
        firstdoscodecomment = FIRSTVHSDOSCODECOMMENT;
        lastdoscodecomment  =  LASTVHSDOSCODECOMMENT;
        firstdosdatacomment = FIRSTVHSDOSDATACOMMENT;
        lastdosdatacomment  =  LASTVHSDOSDATACOMMENT;
    acase DOS_NOBINBUGDOS:
        for (i = 0x6800; i <= 0x6FFF; i++)
        {   memory[         i] =           0;
        }
        firstdosequiv       =
        lastdosequiv        =
        firstdoscodecomment =
        lastdoscodecomment  =
        firstdosdatacomment =
        lastdosdatacomment  = -1;
    }
    if (firstdoscodecomment != -1)
    {   for (i = firstdoscodecomment; i <= lastdoscodecomment; i++)
        {   memflags[codecomment[i].address] |= COMMENTED;
    }   }
    for (i = FIRSTACOSCODECOMMENT; i <= LASTACOSCODECOMMENT; i++)
    {   memflags[codecomment[i].address] |= COMMENTED;
    }

    if (binbug_dosver != DOS_NOBINBUGDOS)
    {   memory[0x70FC] = 0xFF; // date valid flag ($01 doesn't work)
        read_rtc();
        memory[0x70FD] = ((rtc.day   / 10) << 4) | (rtc.day   % 10);
        memory[0x70FE] = ((rtc.month / 10) << 4) | (rtc.month % 10);
        rtc.year %= 100;
        memory[0x70FF] = ((rtc.year  / 10) << 4) | (rtc.year  % 10);
    }

    binbug_changebaud();

    if (user)
    {   reopen_subwindow(SUBWINDOW_SPEED);
}   }

EXPORT void binbug_view_udgs(void)
{   int  x, y,
         whichsprite1, whichsprite2;
    TEXT bits[8 + 1];

    // assert(machine == BINBUG);

    bits[8] = EOS;

    for (whichsprite1 = 0; whichsprite1 < 128; whichsprite1 += 4)
    {   for (y = 0; y < 16; y++)
        {   for (whichsprite2 = 0; whichsprite2 < 4; whichsprite2++)
            {   for (x = 0; x < 8; x++)
                {   if (memory[0x7000 + ((whichsprite1 + whichsprite2) * 16) + y] & (0x80 >> x))
                    {   bits[x] = '#';
                    } else
                    {   bits[x] = '.';
                }   }

                if (y == 0)
                {   zprintf
                    (   TEXTPEN_VIEW,
                        "  UDG%03d: %s ",
                        whichsprite1 + whichsprite2,
                        bits
                   );
                } else
                {   zprintf
                    (   TEXTPEN_VIEW,
                        "          %s ",
                        bits
                    );
            }   }
            zprintf(TEXTPEN_VIEW, "\n");
        }

        zprintf(TEXTPEN_VIEW, "\n");
}   }

EXPORT void binbug_setmemmap(void)
{   int address,
        i, j,
        mirror;

    game = FALSE;
    machines[BINBUG].cpf = fastbinbug ? 40000.0 : 20000.0;
    nextinst = 0;

    for (i =  0x400; i <= 0x57FF; i++)
    {   memory[i] = 0;
    }
    for (i = 0x5C00; i <= 0x77FF; i++)
    {   memory[i] = 0;
    }
    for (i = 0x7800; i <= 0x7BFF; i++)
    {   memory[i] = 0x20;
    }
    for (i = 0x7C00; i <= 0x7FFF; i++)
    {   memory[i] = 0xE0; // for white text
    }

    for (address = 0; address < 32 * KILOBYTE; address++)
    {   memflags[address] = 0;
        mirror            = address;

        if
        (    address <=  0x3FF
         || (address >= 0x5800 && address <= 0x5BFF)
         || (address >= 0x6000 && address <= 0x63FF)
         || (address >= 0x6800 && address <= 0x6FFF)
        )
        {   memflags[address] |= NOWRITE;
        } elif (address >= 0x7800)
        {   memflags[address] |= VISIBLE;
        } elif (address >= 0x4000 && address <= 0x53FF)
        {   memflags[address] |= NOREAD | NOWRITE;
        }
        if
        (   (address >= 0x5400 && address <= 0x57FF) // GBUG's editor needs $57A6..$57AF to be RAM
         || (address >= 0x6400 && address <= 0x67FF) // ACOS          needs $6400..$67FF to be a mirror of $0400..$07FF
        )
        {   mirror = address & 0x07FF;
        }
        mirror_r[address] =
        mirror_w[address] = (UWORD) mirror;
    }

    for (i = 0; i <= 0x3FF; i++)
    {   memflags[i] |= BIOS;
    }
    for (i = 0x6000; i <= 0x6FFF; i++)
    {   memflags[i] |= BIOS; // maybe don't consider $6800..$6FFF to be BIOS if no DOS is in use
    }

    for (i = 0; i <= 0x3FF; i++)
    {   memory[0x6000 + i] = bios_acos[i];
    }
    patch_acos();
    for (i = 0; i <= 0x32F; i++)
    {   memory[0x6400 + i] = driver[i];
    }

    whichkeyrect = KEYRECT_BINBUG;

    curdrive            = 0;
    if (debugdrive >= machines[machine].drives)
    {   debugdrive = machines[machine].drives - 1;
    }

    stepdir             = 0;
    drive_mode          = DRIVEMODE_IDLE;
    timeoutat           = (ULONG) -1;
    for (i = 0; i < machines[machine].drives; i++)
    {   for (j = 0; j < BINBUG_DISKSIZE / 8; j++)
        {   drive[i].flags[j] = 0;
        }
        drive[i].track       =
        drive[i].blockoffset = 0;
        drive[i].sector      = 1;
        drive[i].spinning    =
        drive[i].inserted    = FALSE;
    }
    trackreg = 0;

    binbug_changebios(FALSE); // this also handles COMMENTED flag
}

EXPORT void binbug_emulate(void)
{   FAST UBYTE fgc,
               t,
               tempvdu;
    FAST int   colour,
               i,
               kx, x, x1, x2,
               ky, y, y1, y2;

    inframe = TRUE;

    ReadJoystick(0);
    ReadJoystick(1);
    if (swapped)
    {   binbug_playerinput(1, 0);
        binbug_playerinput(0, 1);
    } else
    {   binbug_playerinput(0, 0);
        binbug_playerinput(1, 1);
    }
    pipbin_readtty();
    keys_column[0] = keys_column[1] = keys_column[2] = keys_column[3] = 0;
    domouse();

    if (framebased)
    {   if (fastbinbug)
        {   // 2MHz = 2,000,000 cycles per second
            // 2,000,000 / 50 = 40,000
            slice_2650 += 40000;
        } else
        {   // 1MHz = 1,000,000 cycles per second / 50 = 20,000
            slice_2650 += 20000;
        }
        pipbin_runcpu();
        draw_binbug();
    } else
    {   for (cpuy = 0; cpuy <= 255; cpuy++)
        {   breakrastline();
            y1 = cpuy / 16;
            y2 = cpuy % 16;
            for (cpux = 0; cpux <= 575; cpux++)
            {   x1 = cpux / 9;
                x2 = cpux % 9;
#ifdef SHOWCHARSET
                if (x1 < 16 && y1 < 16) tempvdu = (y1 * 16) + x1; else
#endif
                tempvdu = memory[0x7800 + (y1 * 64) + x1];
                if (memory[0x7C00 + (y1 * 64) + x1] & 4)
                {   t = memory[0x7000 + (tempvdu * 16) + y2];
                } elif (memory[0x7C00 + (y1 * 64) + x1] & 2)
                {   t = dg640_gfx[tempvdu][y2];
                } else
                {   t = dg640_chars[tempvdu & 0x7F][y2];
                    if (tempvdu & 0x80)
                    {   t = ~t;
                }   }
                if ((memory[0x7C00 + (y1 * 64) + x1] & 1) && (frames % 50) < 25)
                {   if (coomer)
                    {   t = ~t; // inverse
                    } else
                    {   t = 0; // blank
                }   }

#ifdef NINTHPIXEL
                if ((x2 <= 7 && (t & (0x80 >> x2))) || (x2 == 8 && (t & 1)))
#else
                if (x2 <= 7 && (t & (0x80 >> x2)))
#endif
                {   fgc = inverse ? BLACK : WHITE;
                    changepixel(cpux, cpuy, fgc);
                } else
                {   bgc = inverse ? WHITE : BLACK;
                    changepixel(cpux, cpuy, bgc);
                }
                DOCPU;
            }
            run_cpu(768);
        }
        for (cpuy = 256; cpuy <= 311; cpuy++)
        {   breakrastline();
            run_cpu(768);
        }
        cpuy = 312;
        breakrastline();
        run_cpu(384);
        nextinst -= 384; // skip the second half of the last rastline
    }

    if (editscreen)
    {   kx =  scrnaddr           % 64;
        ky = (scrnaddr - 0x7800) / 64;
        x1 = (kx *  9) -  1;
        x2 = (kx *  9) +  9;
        y1 = (ky * 16) -  1;
        y2 = (ky * 16) + 16;
        for (x = x1; x <= x2; x++)
        {   for (y = y1; y <= y2; y++)
            {   if
                (   (x == x1 || x == x2 || y == y1 || y == y2)
                 &&  x >=  0
                 &&  x < 576
                 &&  y >=  0
                 &&  y < 256
                )
                {   changepixel(x, y, RED);
    }   }   }   }

    for (i = 0; i < 8; i++)
    {   if (glow & (128 >> i))
        {   colour       = RED;
        } elif (drawunlit)
        {   colour       = DARKBLUE;
        } else
        {   colour       = BLACK;
        }
        drawglow(207 + (usemargins ? 96 : 0) + (i * 21), machines[BINBUG].height - 17, colour);
    }

    fd1771_spindown();
    if (drawmode)
    {   binbug_drawhelpgrid();
    }
    wa_checkinput();
    endofframe(bgc);
}

EXPORT UBYTE binbug_readport(int port)
{   FAST UBYTE oldtrack,
               oldsector,
               oldstatus,
               t;
    FAST int   oldmode,
               oldoffset;

    // assert(machine == BINBUG);

    oldtrack  = drive[curdrive].track;
    oldsector = drive[curdrive].sector;
    oldstatus = drivestatus;
    oldmode   = drive_mode;
    oldoffset = drive[curdrive].blockoffset;

    switch (port)
    {
    case 1: // PPI port "B"
        if
        (   ioport[3].contents == 0x82
         || ioport[3].contents == 0x83
         || ioport[3].contents == 0x8A
         || ioport[3].contents == 0x8B
         || ioport[3].contents == 0x92
         || ioport[3].contents == 0x93
         || ioport[3].contents == 0x9A
         || ioport[3].contents == 0x9B
        )
        {   if (whichgame == HEXKEYBOARDPOS)
            {   switch (ioport[0].contents)
                {
                case  1:  t = keys_column[0];
                acase 2:  t = keys_column[1];
                acase 4:  t = keys_column[2];
                acase 8:  t = keys_column[3];
                adefault: t = 0; // Hex Keyboard program does its first read of IOPORT($01) before writing anything to IOPORT($00)
                }
                t = ((t & 0x08) >> 3)
                  | ((t & 0x04) >> 1)
                  | ((t & 0x02) << 1)
                  | ((t & 0x01) << 3);
            } else // eg. Keyboard Display program
            {   t = tt_kybdcode;
        }   }
        else // port is not usable for input
        {   t = 0;
        }
    acase 5:
        t = tt_kybdcode;
        tt_kybdcode = 0xFF;
    acase 9:
        t = 0;
        switch (playerfire[0])
        {
        case  1: // t |= 0x00; // %xx,000,xxx
        acase 2:    t |= 0x08; // %xx,001,xxx
        acase 3:    t |= 0x10; // %xx,010,xxx
        acase 4:    t |= 0x18; // %xx,011,xxx
        adefault:   t |= 0x38; // %xx,111,xxx
        }
        switch (playerfire[1])
        {
        case  1: // t |= 0x00; // %xx,xxx,000
        acase 2:    t |= 0x01; // %xx,xxx,001
        acase 3:    t |= 0x02; // %xx,xxx,010
        acase 4:    t |= 0x03; // %xx,xxx,011
        adefault:   t |= 0x07; // %xx,xxx,111
        }
    acase BINBUG_FD1771_STATUS: // $0C
        t = fd1771_read_status();
    acase BINBUG_FD1771_TRACK: // $0D
        t = trackreg;
        if (verbosedisk)
        {   zprintf(TEXTPEN_LOG, "Code at $%X read %d from track register.\n", iar, t);
        }
    acase BINBUG_FD1771_SECTOR: // $0E
        t = drive[curdrive].sector;
        if (verbosedisk)
        {   zprintf(TEXTPEN_LOG, "Code at $%X read %d from sector register.\n", iar, t);
        }
    acase BINBUG_FD1771_DATA: // $0F
        t = fd1771_read_datareg();
    acase BINBUG_FD1771_CONTROL: // $10
        t = fd1771_read_controlreg();
    acase 25:
        t = 0x80; // ETI-641 printer is ready
    acase 0x32:
        t = ioport[0x32].contents & 0x0F;
        switch (binbug_baudrate)
        {
        case  BINBUG_BAUDRATE_150:  t |= 0x00; // do nothing
        acase BINBUG_BAUDRATE_300:  t |= 0x10;
        acase BINBUG_BAUDRATE_1200: t |= 0x20;
        acase BINBUG_BAUDRATE_2400: t |= 0x30;
        }
        if (tapemode == TAPEMODE_PLAY)
        {   if (tapebyte < tapeskewage)
            {   t &= 0x7F;
            } else
            {   t |= 0x80;
            }
            // It may be that we should invert the saving logic instead of the loading logic
            // (without any genuine BINBUG tape dumps it is hard to know).
        }
    acase 0x35:
        t = tt_kybdcode;
        tt_kybdcode = 0xFF;
    acase 0xEF:
        switch (binbug_joyunit)
        {
        case  0:  t = 127 - (ax[1] / 2);
        acase 1:  t =        ay[1] / 2 ;
        acase 2:  t = 127 - (ax[0] / 2);
        acase 3:  t =        ay[0] / 2 ;
        adefault: t = 0;
        }
    acase PORTD:
        t = 0x80; // EA printer is ready
    adefault:
        t = 0;
    }

    logport(port, t, FALSE);

    if
    (   drive[curdrive].track       != oldtrack
     || drive[curdrive].sector      != oldsector
     || drivestatus                 != oldstatus
     || drive_mode                  != oldmode
     || drive[curdrive].blockoffset != oldoffset
    )
    {   if (viewingdrive != (int) curdrive)
        {   viewingdrive = (int) curdrive;
            update_floppydrive(3, viewingdrive);
        } else
        {   update_floppydrive(1, viewingdrive);
    }   }

    return t;
}

EXPORT void binbug_writeport(int port, UBYTE data)
{   FAST UBYTE oldtrack,
               oldsector,
               oldstatus;
    FAST int   oldmode,
               oldoffset;

    // assert(machine == BINBUG);

    oldtrack  = drive[curdrive].track;
    oldsector = drive[curdrive].sector;
    oldstatus = drivestatus;
    oldmode   = drive_mode;
    oldoffset = drive[curdrive].blockoffset;

    switch (port)
    {
    case 0: // port "A" of PPI
        if
        (   (ioport[3].contents >= 0x80 && ioport[3].contents <= 0x83)
         || (ioport[3].contents >= 0x88 && ioport[3].contents <= 0x8B)
        )
        {   if (whichgame != HEXKEYBOARDPOS)
            {   glow = data;
        }   }
        // else port is not usable for output
    acase 2: // port "C" of PPI
        ;
    acase 3: // control port of PPI
        ;
    acase 6:
        ; // parallel keyboard games echo the user's input to this port
    acase BINBUG_FD1771_COMMAND: // $0C
        if (verbosedisk)
        {   zprintf(TEXTPEN_LOG, "Code at $%X issued ", iar);
        }
        switch (data)
        {
        case  0x00: case 0x01: case 0x02: case 0x03:
        case  0x04: case 0x05: case 0x06: case 0x07:
        case  0x08: case 0x09: case 0x0A: case 0x0B:
        case  0x0C: case 0x0D: case 0x0E: case 0x0F:
            fd1771_restore(data);
        acase 0x10: case 0x11: case 0x12: case 0x13:
        case  0x14: case 0x15: case 0x16: case 0x17:
        case  0x18: case 0x19: case 0x1A: case 0x1B:
        case  0x1C: case 0x1D: case 0x1E: case 0x1F:
            fd1771_seek(data);
        acase 0x20: case 0x21: case 0x22: case 0x23:
        case  0x24: case 0x25: case 0x26: case 0x27:
        case  0x28: case 0x29: case 0x2A: case 0x2B:
        case  0x2C: case 0x2D: case 0x2E: case 0x2F:
        case  0x30: case 0x31: case 0x32: case 0x33:
        case  0x34: case 0x35: case 0x36: case 0x37:
        case  0x38: case 0x39: case 0x3A: case 0x3B:
        case  0x3C: case 0x3D: case 0x3E: case 0x3F:
            fd1771_step(data);
        acase 0x40: case 0x41: case 0x42: case 0x43:
        case  0x44: case 0x45: case 0x46: case 0x47:
        case  0x48: case 0x49: case 0x4A: case 0x4B:
        case  0x4C: case 0x4D: case 0x4E: case 0x4F:
        case  0x50: case 0x51: case 0x52: case 0x53:
        case  0x54: case 0x55: case 0x56: case 0x57:
        case  0x58: case 0x59: case 0x5A: case 0x5B:
        case  0x5C: case 0x5D: case 0x5E: case 0x5F:
            fd1771_step_in(data);
        acase 0x60: case 0x61: case 0x62: case 0x63:
        case  0x64: case 0x65: case 0x66: case 0x67:
        case  0x68: case 0x69: case 0x6A: case 0x6B:
        case  0x6C: case 0x6D: case 0x6E: case 0x6F:
        case  0x70: case 0x71: case 0x72: case 0x73:
        case  0x74: case 0x75: case 0x76: case 0x77:
        case  0x78: case 0x79: case 0x7A: case 0x7B:
        case  0x7C: case 0x7D: case 0x7E: case 0x7F:
            fd1771_step_out(data);
        acase 0x80:
        case  0x84:
        case  0x88:
        case  0x8C:
        case  0x90:
        case  0x94:
        case  0x98:
        case  0x9C:
            fd1771_read_sector(data);
        acase 0xA0: case 0xA1: case 0xA2: case 0xA3:
        case  0xA4: case 0xA5: case 0xA6: case 0xA7:
        case  0xA8: case 0xA9: case 0xAA: case 0xAB:
        case  0xAC: case 0xAD: case 0xAE: case 0xAF:
        case  0xB0: case 0xB1: case 0xB2: case 0xB3:
        case  0xB4: case 0xB5: case 0xB6: case 0xB7:
        case  0xB8: case 0xB9: case 0xBA: case 0xBB:
        case  0xBC: case 0xBD: case 0xBE: case 0xBF:
            fd1771_write_sector(data);
        acase 0xC4:
            fd1771_read_address(data);
        acase 0xD0:
        case  0xD1:
        case  0xD2:
        case  0xD3:
        case  0xD4:
        case  0xD5:
        case  0xD6:
        case  0xD7:
        case  0xD8:
        case  0xD9:
        case  0xDA:
        case  0xDB:
        case  0xDC:
        case  0xDD:
        case  0xDE:
        case  0xDF:
            fd1771_force_interrupt(data);
        acase 0xE4:
        case  0xE5:
            fd1771_read_track(data);
        acase 0xF4:
            fd1771_write_track(data);
        adefault:
            if (verbosedisk)
            {   zprintf(TEXTPEN_LOG, "undefined command ($%02x)!\n", data);
        }   }
        if (verbosedisk)
        {   zprintf(TEXTPEN_LOG, "\n");
        }
    acase BINBUG_FD1771_TRACK: // $0D
        fd1771_write_trackreg(data);
    acase BINBUG_FD1771_SECTOR: // $0E
        fd1771_write_reqsector(data);
    acase BINBUG_FD1771_DATA: // $0F
        fd1771_write_datareg(data);
    acase BINBUG_FD1771_CONTROL: // $10
        if (data < binbug_userdrives)
        {   if (verbosedisk)
            {   zprintf(TEXTPEN_VERBOSE, "Code at $%X changed drive unit from %d to %d!\n", iar, curdrive, data);
            }
            if (curdrive != data)
            {   if (drive[curdrive].spinning || drive[curdrive].headloaded)
                {   set_drive_mode(DRIVEMODE_IDLE);
                    sound_stop(GUESTCHANNELS + SAMPLE_GRIND);
                    sound_stop(GUESTCHANNELS + SAMPLE_SPIN);
                    drive[curdrive].headloaded = drive[curdrive].spinning = FALSE;
                    update_floppydrive(1, curdrive);
                    timeoutat = (ULONG) -1;
                    if (verbosedisk)
                    {   zprintf(TEXTPEN_LOG, "Floppy drive #%d has unloaded head and spun down due to drive change.\n", curdrive);
                }   }
                curdrive = data;
        }   }
        elif (verbosedisk)
        {   zprintf(TEXTPEN_VERBOSE, "Code at $%X attempted to change drive unit from %d to %d!\n", iar, curdrive, data);
        }
        RESETTIMEOUT;
        if (viewingdrive != (int) curdrive)
        {   viewingdrive = curdrive;
            update_floppydrive(3, viewingdrive);
        }
    acase 25: // ETI-641 printer
        euy_printchar(data, TRUE);
    acase 0x32:
        if ((data & 1) != (ioport[0x32].contents & 1))
        {   if (data & 1)
            {   if (verbosetape)
                {   zprintf(TEXTPEN_VERBOSE, "Output tape motor has been turned on.\n");
                }
                /* if (tapemode == TAPEMODE_STOP)
                {   tape_record();
                } */
            } else
            {   if (verbosetape)
                {   zprintf(TEXTPEN_VERBOSE, "Output tape motor has been turned off.\n");
                }
                if (tapemode > TAPEMODE_STOP)
                {   tape_stop();
        }   }   }
        if ((data & 2) != (ioport[0x32].contents & 2))
        {   if (data & 2)
            {   if (verbosetape)
                {   zprintf(TEXTPEN_VERBOSE, "Input tape motor has been turned on.\n");
                }
                /* if (tapemode == TAPEMODE_STOP)
                {   tape_play();
                } */
            } else
            {   if (verbosetape)
                {   zprintf(TEXTPEN_VERBOSE, "Input tape motor has been turned off.\n");
                }
                if (tapemode > TAPEMODE_STOP)
                {   tape_stop();
        }   }   }
    acase 0x33:
        ; // it writes $98 here when resetting
    acase 0xEF:
        binbug_joyunit = data;
    acase PORTC: // BINBUG 3.6 (at least) doesn't seem to actually write to this port
        ioport[PORTC].contents = data; // high bit enables papertape reader
    acase PORTD:
        euy_printchar(data, FALSE); // EA printer
    }

    logport(port, data, TRUE);

    if
    (   drive[curdrive].track  != oldtrack
     || drive[curdrive].sector != oldsector
    )
    {   if (viewingdrive != (int) curdrive)
        {   viewingdrive = (int) curdrive;
            update_floppydrive(3, viewingdrive);
        } else
        {   update_floppydrive(2, viewingdrive);
    }   }
    elif
    (   drivestatus                 != oldstatus
     || drive_mode                  != oldmode
     || drive[curdrive].blockoffset != oldoffset
    )
    {   if (viewingdrive != (int) curdrive)
        {   viewingdrive = (int) curdrive;
            update_floppydrive(3, viewingdrive);
        } else
        {   update_floppydrive(1, viewingdrive);
}   }   }

EXPORT void binbug_viewscreen(void)
{   int i,
        where,
        y;

    // assert(machine == BINBUG);

    for (y = 0; y < 16; y++)
    {   dumprow(0x7800 + (y * 64));
        dumprow(0x7810 + (y * 64));
        dumprow(0x7820 + (y * 64));
        dumprow(0x7830 + (y * 64));
        zprintf(TEXTPEN_VIEW, "\n");
    }

    zprintf(TEXTPEN_VIEW, "Colour attribute RAM:\n");
    for (y = 0; y < 16; y++)
    {   for (i = 0; i < 4; i++)
        {   where = 0x7C00 + (y * 64) + (i * 16);
            zprintf
            (   TEXTPEN_VIEW, // ideally, we would show the text in the correct colours
                "$%04X: %02X %02X %02X %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X %02X %02X %02X\n",
                where,
                memory[where     ],
                memory[where +  1],
                memory[where +  2],
                memory[where +  3],
                memory[where +  4],
                memory[where +  5],
                memory[where +  6],
                memory[where +  7],
                memory[where +  8],
                memory[where +  9],
                memory[where + 10],
                memory[where + 11],
                memory[where + 12],
                memory[where + 13],
                memory[where + 14],
                memory[where + 15]
            );
        }
        zprintf(TEXTPEN_VIEW, "\n");
}   }

EXPORT void patch_acos(void)
{   if (fastbinbug)
    {   memory[0x631E] = 0x86; // ADDI,r2 8
        memory[0x631F] = 0x08;
        memory[0x6327] = 0x6E; // LODI,r0 $6E
        memory[0x633A] = 0x2E; // LODI,r0 $2E
        memory[0x6340] = 0x66; // DB      $66, $4A, $30, $18
        memory[0x6341] = 0x4A;
        memory[0x6342] = 0x30;
        memory[0x6343] =
        memory[0x634B] = 0x18; // LODI,r0 $18
        memory[0x6356] = 0xEF; // LODI,r2 $EF
        memory[0x6358] = 0x18; // LODI,r0 $18
        memory[0x63AF] = 0x2A; // COMI,r0 $2A
        memory[0x63BC] = 0x1D; // COMI,r0 $1D
        memory[0x63C3] = 0x11; // COMI,r0 $11
        memory[0x63CE] = 0x29; // COMI,r0 $29
    } else
    {   memory[0x631E] = 0xA6; // SUBI,r2 2
        memory[0x631F] = 0x02;
        memory[0x6327] = 0x32; // LODI,r0 $32
        memory[0x633A] = 0x12; // LODI,r0 $12
        memory[0x6340] = 0x32; // DB      $32, $24, $17, $0B
        memory[0x6341] = 0x24;
        memory[0x6342] = 0x17;
        memory[0x6343] =
        memory[0x634B] = 0x0B; // LODI,r0 $0B
        memory[0x6356] = 0x89; // LODI,r2 $89
        memory[0x6358] = 0x0B; // LODI,r0 $0B
        memory[0x63AF] = 0x14; // COMI,r0 $14
        memory[0x63BC] = 0x0D; // COMI,r0 $0D
        memory[0x63C3] = 0x07; // COMI,r0 $07
        memory[0x63CE] = 0x13; // COMI,r0 $13
}   }

EXPORT void do_postamble(void)
{   // assert(machine == BINBUG);

    zprintf
    (   TEXTPEN_TAPE,
        "Postamble is:\n" \
        " Execute address:          $%02X%02X\n",
        memory[0x45E], memory[0x45F]
    );
    zprintf
    (   TEXTPEN_TAPE,
        " File identification:      $%02X (",
        memory[0x45D]
    );
    switch (memory[0x45D] & 0xF0)
    {
    case  0x00:
        zprintf(TEXTPEN_TAPE, "program");
    acase 0x10:
    case  0x20:
    case  0x30:
    case  0x40:
    case  0x50:
    case  0x60:
    case  0x70:
        zprintf(TEXTPEN_TAPE, "data");
    acase 0x80:
        zprintf(TEXTPEN_TAPE, "BASIC source");
    acase 0x90:
        zprintf(TEXTPEN_TAPE, "assembler source");
    acase 0xA0:
        zprintf(TEXTPEN_TAPE, "text");
    adefault: // $B0..$FF
        zprintf(TEXTPEN_TAPE, "unassigned text type");
    }
    zprintf
    (   TEXTPEN_TAPE,
        " file)\n" \
        " # of data bytes in block: %d\n",
        memory[0x45C]
    );
    zprintf
    (   TEXTPEN_TAPE,
        " Store address:            $%02X%02X\n",
        memory[0x45A], memory[0x45B]
    );
    zprintf
    (   TEXTPEN_TAPE,
        " Filename:                 %c%c%c%c%c%c%c%c\n",
        (memory[0x459] >= ' ') ? memory[0x459] : BLANKASCII,
        (memory[0x458] >= ' ') ? memory[0x458] : BLANKASCII,
        (memory[0x457] >= ' ') ? memory[0x457] : BLANKASCII,
        (memory[0x456] >= ' ') ? memory[0x456] : BLANKASCII,
        (memory[0x455] >= ' ') ? memory[0x455] : BLANKASCII,
        (memory[0x454] >= ' ') ? memory[0x454] : BLANKASCII,
        (memory[0x453] >= ' ') ? memory[0x453] : BLANKASCII,
        (memory[0x452] >= ' ') ? memory[0x452] : BLANKASCII
    );
    zprintf
    (   TEXTPEN_TAPE,
        " Unassigned:               %d\n",
        memory[0x451]
    );
    zprintf
    (   TEXTPEN_TAPE,
        " Block number in file:     %d\n",
        memory[0x450]
    );
    zprintf
    (   TEXTPEN_TAPE,
        " 16-bit CRC word:          $%02X%02X\n",
        memory[0x44E], memory[0x44F]
    );
}

// MODULE CODE------------------------------------------------------------

MODULE void binbug_playerinput(int source, int dest)
{   FAST ULONG jg;

    // dest is which side   (0 or 1) you want to set the registers of.
    // source is which side (0 or 1) you want to use to do it.

    // This doesn't currently support gameplay recording/playback.

    if
    (   hostcontroller[source] == CONTROLLER_1STDJOY
     || hostcontroller[source] == CONTROLLER_1STDPAD
  // || hostcontroller[source] == CONTROLLER_1STAJOY
     || hostcontroller[source] == CONTROLLER_1STAPAD
    )
    {   jg = jff[0];
    } elif
    (   hostcontroller[source] == CONTROLLER_2NDDJOY
     || hostcontroller[source] == CONTROLLER_2NDDPAD
     || hostcontroller[source] == CONTROLLER_2NDAJOY
     || hostcontroller[source] == CONTROLLER_2NDAPAD
    )
    {   jg = jff[1];
    } else
    {   jg = 0;
    }

    playerfire[dest] = 0;
    if (autofire[source])
    {   if
        (   (   !requirebutton[source]
             || (jg & (JOYFIRE1 | DAPTER_1))
             || KeyDown(keypads[source][0])
             || (hostcontroller[source] == CONTROLLER_TRACKBALL && lmb)
            )
         && ((frames % totalframes) < downframes)
        )
        {   // 1st firebutton
            playerfire[dest] = 1;
    }   }
    elif
    (   (jg & (JOYFIRE1 | DAPTER_1))
     || KeyDown(keypads[source][ 0]) // 1st firebutton
     || (hostcontroller[source] == CONTROLLER_TRACKBALL && lmb)
    )
    {   // 1st firebutton
        playerfire[dest] = 1;
    } elif
    (   (jg & (JOYFIRE2 | DAPTER_2))
     || KeyDown(keypads[source][21]) // 2nd firebutton
     || (hostcontroller[source] == CONTROLLER_TRACKBALL && mmb)
    )
    {   // 2nd firebutton
        playerfire[dest] = 2;
    } elif
    (   (jg & (JOYFIRE3 | DAPTER_3))
     || KeyDown(keypads[source][22]) // 3rd firebutton
     || (hostcontroller[source] == CONTROLLER_TRACKBALL && rmb)
    )
    {   // 3rd firebutton
        playerfire[dest] = 3;
    } elif
    (   (jg & (JOYFIRE4 | DAPTER_4))
     || KeyDown(keypads[source][23]) // 4th firebutton
    )
    {   // 4th firebutton
        playerfire[dest] = 4;
    }

    engine_dopaddle(source, dest);
}

EXPORT void yesno(UBYTE data)
{   if (data) zprintf(TEXTPEN_LOG, " Yes\n"); else zprintf(TEXTPEN_LOG, " No\n");
}
EXPORT void noyes(UBYTE data)
{   if (data) zprintf(TEXTPEN_LOG, " No\n"); else zprintf(TEXTPEN_LOG, " Yes\n");
}

EXPORT void binbug_dir_disk(FLAG quiet, int whichdrive)
{   UBYTE expected,
          found;
    int   freesize,
          i, k,
          startblock,
          endblock,
          loadaddr,
          reservedsize,
          thesize,
          usedsize,
          where,
          whichtrack,
          whichsector;

    if (!drive[whichdrive].inserted)
    {   return;
    }

    for (i = 0; i < BINBUG_DISKBLOCKS; i++)
    {   drive[whichdrive].bam[i] = BAM_LOST;
    }

    reservedsize = BINBUG_SECTORS * BINBUG_USERBLOCKSIZE;

    if (!quiet)
    {   zprintf
        (   TEXTPEN_DISK,
            "Tracks on disk:    %d\n" \
            "Sectors per track: %d\n" \
            "Disk identity:     \"%c%c%c\"\n\n" \
            "Filename  .Ext  Start..Finish  Size Type   Date",
            drive[whichdrive].contents[8],
            drive[whichdrive].contents[9],
            drive[whichdrive].contents[10],
            drive[whichdrive].contents[11],
            drive[whichdrive].contents[12]
        );
        zprintf
        (   TEXTPEN_DISK,
            verbosedisk ?
            "\n" \
            "-------------------------------------------------\n" \
            "(File List)    $    %d..$  9FF  %d n/a  %2d/%2d/%2d\n" :
            "   StAdr\n" \
            "-------------------------------------------------------\n" \
            "(File List)    $    %d..$  9FF  %d n/a  %2d/%2d/%2d  n/a\n",
            256 - BINBUG_USERBLOCKSIZE,
            reservedsize,
            bcd_to_normal(drive[whichdrive].contents[13]),
            bcd_to_normal(drive[whichdrive].contents[14]),
            bcd_to_normal(drive[whichdrive].contents[15])
        );
    }
    DISCARD followchain(0, 9, reservedsize, BAM_DIR, FALSE, quiet, whichdrive);

    startblock =  (drive[whichdrive].contents[2] *  10) + (drive[whichdrive].contents[3] - 1);
    endblock   =  (drive[whichdrive].contents[4] *  10) + (drive[whichdrive].contents[5] - 1);
    freesize   = ((drive[whichdrive].contents[6] * 256) +  drive[whichdrive].contents[7]) * BINBUG_USERBLOCKSIZE;
    if (!quiet)
    {   zprintf
        (   TEXTPEN_DISK,
            "(Free Area)    $%5X..$%5X %5d n/a  %2d/%2d/%2d",
            (startblock * 256) + 256 - BINBUG_USERBLOCKSIZE,
            (endblock   * 256) + 255,
            freesize,
            bcd_to_normal(drive[whichdrive].contents[13]),
            bcd_to_normal(drive[whichdrive].contents[14]),
            bcd_to_normal(drive[whichdrive].contents[15])
        );
        if (verbosedisk)
        {   zprintf(TEXTPEN_DISK, "\n");
        } else
        {   zprintf(TEXTPEN_DISK, "  n/a\n");
    }   }
    DISCARD followchain(startblock, endblock, freesize, BAM_FREE, FALSE, quiet, whichdrive);

    where    =
    usedsize = 0;
    for (k = 0; k < 10; k++)
    {   where += 16; // skip identity section
        for (i = 0; i < 10; i++)
        {   if (drive[whichdrive].contents[where] == EOS || drive[whichdrive].contents[where] >= 0x80) // eg. $E5 or $FF
            {   drive[whichdrive].filename[(k * 10) + i][whichdrive] = EOS;
                where += 24;
                continue;
            } // implied else
            zstrncpy((STRPTR) drive[whichdrive].filename[(k * 10) + i], (const char*) &drive[whichdrive].contents[where     ], 10);
            zstrncpy((STRPTR) drive[whichdrive].fileext[ (k * 10) + i], (const char*) &drive[whichdrive].contents[where + 10],  3);
            startblock =   (drive[whichdrive].contents[where + 13] * 10) + drive[whichdrive].contents[where + 14] - 1;
            endblock   =   (drive[whichdrive].contents[where + 15] * 10) + drive[whichdrive].contents[where + 16] - 1;
            thesize    =  ((drive[whichdrive].contents[where + 17] * 10) + drive[whichdrive].contents[where + 18]) * BINBUG_USERBLOCKSIZE;
            usedsize   += thesize;
            if (!quiet)
            {   zprintf
                (   TEXTPEN_DISK,
                    "%-10s.%-3s $%5X..$%5X %5d $%02X  %2d/%2d/%2d ",
                    drive[whichdrive].filename[(k * 10) + i],
                    drive[whichdrive].fileext[ (k * 10) + i],
                    (startblock * 256) + 256 - BINBUG_USERBLOCKSIZE,
                    (endblock   * 256) + 255,
                    thesize,
                    drive[whichdrive].contents[where + 19],
                    bcd_to_normal(drive[whichdrive].contents[where + 21]),
                    bcd_to_normal(drive[whichdrive].contents[where + 22]),
                    bcd_to_normal(drive[whichdrive].contents[where + 23])
                );
            }
            if
            (   drive[whichdrive].fileext[(k * 10) + i][whichdrive] == 'C'
             && drive[whichdrive].fileext[(k * 10) + i][1] == 'M'
             && drive[whichdrive].fileext[(k * 10) + i][2] == 'D'
            )
            {   if (!quiet && verbosedisk)
                {   zprintf(TEXTPEN_DISK, "\n");
                }
                loadaddr = followchain(startblock, endblock, thesize, BAM_FILE, TRUE, quiet, whichdrive);
                if (!quiet && !verbosedisk)
                {   if (diskerror)
                    {   zprintf(TEXTPEN_DISK, "Start address: $%X\n", loadaddr);
                    } else
                    {   zprintf(TEXTPEN_DISK, "$%4X\n", loadaddr);
            }   }   }
            else
            {   if (!quiet)
                {   if (verbosedisk)
                    {   zprintf(TEXTPEN_DISK, "\n");
                    } else
                    {   zprintf(TEXTPEN_DISK, " n/a\n");
                }   }
                DISCARD followchain(startblock, endblock, thesize, BAM_FILE, FALSE, quiet, whichdrive);
            }
            where += 24;
    }   }

    if (!quiet)
    {   zprintf
        (   TEXTPEN_DISK,
            "\n%6d bytes used by files.\n" \
            "%6d bytes reserved for file list.\n" \
            "%6d bytes free.\n" \
            "%6d bytes in lost blocks.\n" \
            "%6d bytes total.\n\n",
            usedsize,
            reservedsize,
            freesize,
            DISKSIZE_USER - usedsize - reservedsize - freesize,
            DISKSIZE_USER
        );

        if (verbosedisk)
        {   for (whichtrack = 0; whichtrack < BINBUG_TRACKS; whichtrack++)
            {   for (whichsector = 1; whichsector <= BINBUG_SECTORS; whichsector++)
                {   expected = (whichsector == BINBUG_SECTORS) ? (whichtrack + 1) : whichtrack;
                    found = drive[whichdrive].contents[(whichtrack * BINBUG_TRACKSIZE) + ((whichsector - 1) * BINBUG_BLOCKSIZE)];
                    if (found != expected && found != 0xFF) // we should check that $FF is the last block of a file, and that non-$FFs are non-last blocks
                    {   zprintf
                        (   TEXTPEN_ERROR,
                            "Track %d, sector %d has an unexpected 'track of next block' number in header (found %d vs. expected %d)!\n",
                            whichtrack,
                            whichsector,
                            found,
                            expected
                        );
                    }
                    expected = (whichsector == BINBUG_SECTORS) ? 1 : (whichsector + 1);
                    found = drive[whichdrive].contents[(whichtrack * BINBUG_TRACKSIZE) + ((whichsector - 1) * BINBUG_BLOCKSIZE) + 1];
                    if (found != expected)
                    {   zprintf
                        (   TEXTPEN_ERROR,
                            "Track %d, sector %d has an unexpected 'sector of next block' number in header (found %d vs. expected %d)!\n",
                            whichtrack,
                            whichsector,
                            found,
                            expected
                        );
    }   }   }   }   }

    viewingdrive = whichdrive;
    update_floppydrive(3, viewingdrive);
}

MODULE UWORD followchain(int startblock, int endblock, int thesize, int bamtype, FLAG isprog, FLAG quiet, int whichdrive)
{   int curtrk,
        cursec,
        i,
        localaddr,
        newtrk,
        newsec,
        numblocks;

    diskerror = FALSE;

    if (startblock < 0 || startblock >= BINBUG_DISKBLOCKS)
    {   if (!quiet)
        {   zprintf(TEXTPEN_DISK, "Invalid start block!\n");
        }
        diskerror = TRUE;
        return 0;
    }
    if (endblock < 0 || endblock >= BINBUG_DISKBLOCKS)
    {   if (!quiet)
        {   zprintf(TEXTPEN_DISK, "Invalid end block!\n");
        }
        diskerror = TRUE;
        return 0;
    }
    if (thesize < 0 || thesize > (BINBUG_DISKBLOCKS - BINBUG_SECTORS) * BINBUG_USERBLOCKSIZE)
    {   if (!quiet)
        {   zprintf(TEXTPEN_DISK, "Excessive length!\n");
        }
        diskerror = TRUE;
        return 0;
    }

    curtrk =  startblock / BINBUG_SECTORS;
    cursec = (startblock % BINBUG_SECTORS) + 1;
    numblocks = 1;
    do
    {   localaddr = (curtrk * BINBUG_TRACKSIZE) + ((cursec - 1) * BINBUG_BLOCKSIZE);
        if (drive[whichdrive].bam[localaddr / 256] != BAM_LOST)
        {   if (!quiet)
            {   zprintf(TEXTPEN_DISK, "Block at $%Xxx is used more than once!\n", (curtrk * 10) + cursec - 1);
            }
            diskerror = TRUE;
            break;
        } // implied else
        drive[whichdrive].bam[localaddr / 256] = bamtype;
        newtrk = drive[whichdrive].contents[localaddr    ];
        if (newtrk > 39 && newtrk != 255)
        {   if (!quiet)
            {   zprintf(TEXTPEN_DISK, "Block at $%Xxx has an invalid next track field!\n", (curtrk * 10) + cursec - 1);
            }
            diskerror = TRUE;
            break;
        }
        newsec = drive[whichdrive].contents[localaddr + 1];
        if (newsec < 1 || newsec > 10)
        {   if (!quiet)
            {   zprintf(TEXTPEN_DISK, "Block at $%Xxx has an invalid next sector field!\n", (curtrk * 10) + cursec - 1);
            }
            diskerror = TRUE;
            break;
        }
        if (curtrk == newtrk && cursec == newsec)
        {   if (!quiet)
            {   zprintf(TEXTPEN_DISK, "Block at $%Xxx has a circular reference!\n", (curtrk * 10) + cursec - 1);
            }
            diskerror = TRUE;
            break;
        }
        if (isprog)
        {   for (i = 0; i < 254; i++)
            {   ProgBuffer[((numblocks - 1) * 254) + i] = drive[whichdrive].contents[localaddr + 2 + i];
            }
            chunkblock[numblocks - 1] = localaddr;
        }
        if (!quiet && verbosedisk)
        {   if (newtrk == 0xFF) // file end marker
            {   zprintf
                (   TEXTPEN_VERBOSE,
                    " $%5Xxx: Next block:  None\n",
                    localaddr / 256
                );
            } else
            {   zprintf
                (   TEXTPEN_VERBOSE,
                    " $%5Xxx: Next block: $%3Xxx\n",
                    localaddr / 256,
                    (newtrk * 10) + newsec - 1
                );
        }   }
        if (newtrk == 0xFF) // file end marker
        {   break;
        } // implied else
        curtrk = newtrk;
        cursec = newsec;
        numblocks++;
    } while (numblocks <= BINBUG_DISKBLOCKS - 10);
    if (numblocks > BINBUG_DISKBLOCKS - 10)
    {   zprintf(TEXTPEN_DISK, "Too many blocks!\n");
        diskerror = TRUE;
    }
    if (numblocks * BINBUG_USERBLOCKSIZE != thesize)
    {   zprintf(TEXTPEN_DISK, "Incorrect size (stored %d vs. calculated %d bytes)!\n", thesize, numblocks * BINBUG_USERBLOCKSIZE);
        diskerror = TRUE;
    }
    if (curtrk != endblock / 10)
    {   zprintf(TEXTPEN_DISK, "Incorrect end track (stored %d vs. calculated %d)!\n", endblock / 10, curtrk);
        diskerror = TRUE;
    }
    if (cursec != (endblock % 10) + 1)
    {   zprintf(TEXTPEN_DISK, "Incorrect end sector (stored %d vs. calculated %d!)!\n", (endblock % 10) + 1, cursec);
        diskerror = TRUE;
    }

    if (!quiet && isprog)
    {   return showchunks(numblocks);
    } // implied else
    return 0;
}

MODULE UWORD showchunks(int numblocks)
{   int chunksize,
        curoff,
        destaddr,
        origoff;

    // assert(!quiet);

    curoff = 0;
    while (curoff < numblocks * 254)
    {   destaddr  = (ProgBuffer[curoff    ] * 256)
                  +  ProgBuffer[curoff + 1];
        if (destaddr > 0x7FFF)
        {   zprintf(TEXTPEN_DISK, LLL(MSG_INVALIDLOADSTARTADDR, "Invalid load start address of $%X!\n"), destaddr            );
            diskerror = TRUE;
        }
        chunksize =  ProgBuffer[curoff + 2];
        if (destaddr + chunksize > 0x8000)
        {   zprintf(TEXTPEN_DISK, LLL(MSG_INVALIDLOADENDADDR,   "Invalid load end address of $%X!\n"  ), destaddr + chunksize);
            diskerror = TRUE;
        }
        if (chunksize > 0xFB)
        {   zprintf(TEXTPEN_DISK, LLL(MSG_UNEXPECTEDCHUNKSIZE,  "Unexpected chunk size of %d!\n"      ),            chunksize);
            diskerror = TRUE;
        }

        origoff = ((curoff / 254) * 256)
                +  (curoff % 254);
        if (chunksize == 0)
        {   if (verbosedisk)
            {   zprintf
                (   TEXTPEN_VERBOSE,
                    "           Start addr:  $%4X        found at $%5X..$%X+\n",
                    destaddr,
                    chunkblock[origoff / 256] + (origoff % 256) + 2,
                    chunkblock[origoff / 256] + (origoff % 256) + 3
                );
            }
            return (UWORD) destaddr;
        }
        if (verbosedisk)
        {   zprintf
            (   TEXTPEN_VERBOSE,
                "           Chunk addr:  $%4X..$%4X found at $%5X+\n",
                destaddr,
                destaddr + chunksize - 1,
                chunkblock[origoff / 256] + (origoff % 256) + 2
            );
        }
        curoff += 3 + chunksize;
    }

    return 0;
}

EXPORT FLAG binbug_load_disk(FLAG wantasl, int whichdrive)
{   FILE* DiskHandle /* = NULL */ ;
    int   disksize,
          i;

    // asl() turns sound off and on for us
    if (wantasl)
    {   if (asl
        (   LLL(MSG_HAIL_INSERTDISK, "Insert Disk"),
#ifdef WIN32
            "BINBUG floppy disk (*.RAW)\0*.RAW\0" \
            "All files (*.*)\0*.*\0",
#endif
#ifdef AMIGA
            "#?.RAW",
#endif
            FALSE,
            (STRPTR) path_disks,  // starting directory
#ifdef WIN32
            drive[whichdrive].fn_disk,     // starting filename
#endif
#ifdef AMIGA
    #ifdef __amigaos4__
            (STRPTR) FilePart((CONST_STRPTR) drive[whichdrive].fn_disk), // starting filename
    #else
            FilePart((const char*) drive[whichdrive].fn_disk), // starting filename
    #endif
#endif
            (STRPTR) drive[whichdrive].fn_disk // where to store complete result pathname
        ) != EXIT_SUCCESS)
        {   cd_progdir();
            return FALSE;
        }
#ifdef SETPATHS
        break_pathname((STRPTR) drive[whichdrive].fn_disk, (STRPTR) path_disks, NULL);
#endif
    }

    disksize = getsize((STRPTR) drive[whichdrive].fn_disk);
    if (disksize != BINBUG_DISKSIZE && disksize != 37 * 256 * 10)
    {   say("File is wrong size!");
        cd_progdir();
        return FALSE;
    }
    if (!(DiskHandle = fopen((const char*) drive[whichdrive].fn_disk, "rb")))
    {   say("Can't open file for input!");
        cd_progdir();
        return FALSE;
    }
    cd_progdir();
    if (fread(drive[whichdrive].contents, disksize, 1, DiskHandle) != 1)
    {   DISCARD fclose(DiskHandle);
        // DiskHandle = NULL;
        say("Can't read from file!");
        return FALSE;
    }
    DISCARD fclose(DiskHandle);
    // DiskHandle = NULL;

    if (disksize < BINBUG_DISKSIZE)
    {   extend_disk(whichdrive);
    }

    for (i = 0; i < BINBUG_DISKSIZE / 8; i++)
    {   drive[whichdrive].flags[i] = 0;
    }

    drive[whichdrive].inserted = TRUE;
    binbug_dir_disk(TRUE, whichdrive);
    if (!SubWindowPtr[SUBWINDOW_FLOPPYDRIVE])
    {   open_floppydrive(FALSE);
    }

    return TRUE;
}

EXPORT void binbug_create_disk(int whichdrive)
{   int i, j;

    memset(drive[whichdrive].contents, 0xE5, BINBUG_DISKSIZE);

    for (i = 0; i < BINBUG_TRACKS; i++)
    {   for (j = 0; j < 10; j++) // sector
        {   drive[whichdrive].contents[(i * BINBUG_TRACKSIZE) + (j * BINBUG_BLOCKSIZE)    ] = (j == (BINBUG_SECTORS - 1)) ? (i + 1) :  i     ; // track  of next block
            drive[whichdrive].contents[(i * BINBUG_TRACKSIZE) + (j * BINBUG_BLOCKSIZE) + 1] = (j == (BINBUG_SECTORS - 1)) ?      1  : (j + 2); // sector of next block
    }   }
    drive[whichdrive].contents[  0x900] =
    drive[whichdrive].contents[0x18F00] = 0xFF;

    drive[whichdrive].contents[ 2] =
    drive[whichdrive].contents[ 3] =  1;
    drive[whichdrive].contents[ 4] = BINBUG_TRACKS - 1;
    drive[whichdrive].contents[ 5] = 10;
    drive[whichdrive].contents[ 6] = (BINBUG_DISKBLOCKS - 10) / 256;
    drive[whichdrive].contents[ 7] = (BINBUG_DISKBLOCKS - 10) % 256;
    drive[whichdrive].contents[ 8] = BINBUG_TRACKS;
    drive[whichdrive].contents[ 9] = 10; // sectors per track
#ifdef WIN32
    drive[whichdrive].contents[10] = 'W';
#endif
#ifdef AMIGA
    drive[whichdrive].contents[10] = 'A';
#endif
    drive[whichdrive].contents[11] = 'A';
    drive[whichdrive].contents[12] = '!';
    if (memory[0x70FC] == 0xFF) // if date is valid
    {   drive[whichdrive].contents[13] = memory[0x70FD]; // day
        drive[whichdrive].contents[14] = memory[0x70FE]; // month
        drive[whichdrive].contents[15] = memory[0x70FF]; // year
    } else
    {   drive[whichdrive].contents[13] =                 // day
        drive[whichdrive].contents[14] =                 // month
        drive[whichdrive].contents[15] = 0;              // year
    }

    for (i = 0; i < BINBUG_DISKSIZE / 8; i++)
    {   drive[whichdrive].flags[i] = 0;
    }

    drive[whichdrive].fn_disk[0] = EOS;
    drive[whichdrive].inserted   = TRUE;
    binbug_dir_disk(TRUE, whichdrive);
}

EXPORT FLAG binbug_extract_file(int whichfile)
{   int   curblock,
          i,
          startblock, endblock,
          startbyte,  endbyte;
    FLAG  source = FALSE;
    FILE* TheLocalHandle;

    sprintf((char*) gtempstring, "%s.%s", drive[debugdrive].filename[whichfile], drive[debugdrive].fileext[whichfile]);
    startbyte   = (whichfile * 24) + (((whichfile / 10) + 1) * 16);
    startblock  =  (drive[debugdrive].contents[startbyte + 13] * 10) + (drive[debugdrive].contents[startbyte + 14] - 1);
    endblock    =  (drive[debugdrive].contents[startbyte + 15] * 10) + (drive[debugdrive].contents[startbyte + 16] - 1);

    if ((TheLocalHandle = fopen((const char*) gtempstring, "wb")))
    {   startbyte = (startblock * 256) + 2;
        if (!stricmp((const char*) drive[debugdrive].fileext[whichfile], "SRC") || !stricmp((const char*) drive[debugdrive].fileext[whichfile], "TXT"))
        {   source = TRUE;
            if (drive[debugdrive].contents[startbyte] == STX)
            {   startbyte++; // skip STX
        }   }
        curblock = startblock;
        for (;;)
        {   if (verbosedisk)
            {   zprintf(TEXTPEN_VERBOSE, "Extracting $%Xxx...\n", curblock);
            }
            endbyte = (curblock * 256) + 255;
            if (source && curblock == endblock)
            {   for (i = startbyte; i <= endbyte; i++)
                {   if (drive[debugdrive].contents[i] == ETX)
                    {   endbyte = i - 1;
                        break;
            }   }   }
            if (startbyte <= endbyte)
            {   DISCARD fwrite(&drive[debugdrive].contents[startbyte], (size_t) (endbyte - startbyte + 1), 1, TheLocalHandle);
            }
            if (curblock == endblock)
            {   break;
            }
            curblock = (drive[debugdrive].contents[ curblock * 256     ] * 10)
                     + (drive[debugdrive].contents[(curblock * 256) + 1] -  1);
            startbyte = (curblock * 256) + 2;
        }
        DISCARD fclose(TheLocalHandle);
        // TheLocalHandle = NULL;
        return TRUE;
    } else
    {   return FALSE;
}   }

EXPORT void binbug_drawhelpgrid(void)
{   int x,  y,
        xx, yy,
        startx,
        starty;

    for (x = 0; x < 64; x++)
    {   for (y = 0; y < 16; y++)
        {   startx = x * DG640_CHARWIDTH;
            starty = y * DG640_CHARHEIGHT;
            for (xx = 0; xx < DG640_CHARWIDTH; xx++)
            {   for (yy = 0; yy < DG640_CHARHEIGHT; yy++)
                {   if (xx == 0 || xx == DG640_CHARWIDTH - 1 || yy == 0 || yy == DG640_CHARHEIGHT - 1)
                    {   changepixel(startx + xx, starty + yy, GREY1);
}   }   }   }   }   }

EXPORT void extend_disk(int whichdrive)
{   int i, j;

    memset(drive[whichdrive].contents + 94720, 0xE5, BINBUG_DISKSIZE - 94720);
    for (i = 37; i < BINBUG_TRACKS; i++)
    {   for (j = 0; j < 10; j++) // sector
        {   drive[whichdrive].contents[(i * BINBUG_TRACKSIZE) + (j * BINBUG_BLOCKSIZE)    ] = (j == (BINBUG_SECTORS - 1)) ? (i + 1) :  i     ; // track  of next block
            drive[whichdrive].contents[(i * BINBUG_TRACKSIZE) + (j * BINBUG_BLOCKSIZE) + 1] = (j == (BINBUG_SECTORS - 1)) ?      1  : (j + 2); // sector of next block
    }   }
    drive[whichdrive].contents[0x18F00] = 0xFF;
}

EXPORT FLAG load_cmd(int localsize)
{   int chunksize,
        cursor,
        i,
        loadaddr;

    cursor = 0;
    while (cursor < localsize)
    {   if (cursor + 3 > localsize)
        {   zprintf(TEXTPEN_ERROR, "Warning: incomplete chunk header at end of file.\n");
            zprintf(TEXTPEN_ERROR, "Warning: no end-of-file chunk found.\n\n");
            return TRUE;
        }
        loadaddr  = (IOBuffer[cursor    ] * 256) + IOBuffer[cursor + 1];
        chunksize =  IOBuffer[cursor + 2];
        if (loadaddr > 0x7FFF)
        {   zprintf(TEXTPEN_ERROR, "Invalid chunk header (invalid chunk load address)!\n");
            // don't return here, we will return just below
        }
        if (loadaddr + chunksize > 0x8000)
        {   zprintf(TEXTPEN_ERROR, "Invalid chunk header (invalid chunk end address)!\n\n");
            return FALSE;
        }
        if (chunksize == 0)
        {   startaddr_h = loadaddr / 256;
            startaddr_l = loadaddr % 256;
            /* if (cursor + 3 < localsize)
            {   zprintf(TEXTPEN_ERROR, "Warning: junk found after end-of-file chunk.\n\n"); // it's quite normal for this to be the case
            } else */
            {   zprintf(TEXTPEN_CLIOUTPUT, "Successfully loaded CMD file.\n\n");
            }
            return TRUE;
        } // implied else
        if (chunksize > 0xFB)
        {   zprintf(TEXTPEN_ERROR, "Warning: excessively long chunk.\n");
        }
        cursor += 3;
        if (cursor + chunksize > localsize)
        {   chunksize = localsize - cursor;
            zprintf(TEXTPEN_ERROR, "Warning: truncated chunk.\n");
        }
        for (i = 0; i < chunksize; i++)
        {   memory[loadaddr + i] = IOBuffer[cursor + i];
        }
        cursor += chunksize;
    }

    zprintf(TEXTPEN_ERROR, "Warning: no end-of-file chunk found.\n\n");
    return TRUE;
}

EXPORT FLAG save_cmd(STRPTR filename, int startaddr, int endaddr, int progstart)
{   FILE* CMDHandle;
    int   chunksize,
          cursor,
          i,
          tempstart,
          tempend;

    // assert(startaddr <= endaddr);

#ifdef VERBOSE
    zprintf(TEXTPEN_CLIOUTPUT, "Saving %s from $%X..$%X as CMD...\n", filename, startaddr, endaddr);
#endif

    if (!(CMDHandle = fopen(filename, "wb")))
    {   zprintf(TEXTPEN_CLIOUTPUT, "Can't open \"%s\" for writing!\n", filename);
        return FALSE;
    } // implied else

    tempstart = startaddr;
    do
    {   chunksize = endaddr - tempstart + 1;
        if (chunksize > 0xFB)
        {   chunksize = 0xFB;
        }
        tempend = tempstart + chunksize - 1;

        CMDBuffer[0] = (UBYTE) (tempstart / 256);
        CMDBuffer[1] = (UBYTE) (tempstart % 256);
        CMDBuffer[2] = chunksize;
        cursor = 3;
        for (i = tempstart; i <= tempend; i++)
        {   CMDBuffer[cursor++] = memory[i];
        }
        DISCARD fwrite(CMDBuffer, (size_t) cursor, 1, CMDHandle);
        tempstart += chunksize;
    } while (tempstart <= endaddr);

    CMDBuffer[0] = (UBYTE) (progstart / 256);
    CMDBuffer[1] = (UBYTE) (progstart % 256);
    CMDBuffer[2] = 0;
    DISCARD fwrite(CMDBuffer, 3, 1, CMDHandle);

    DISCARD fclose(CMDHandle);
    // CMDHandle = NULL;

    return TRUE;
}

#define KXLIMIT 63
#define KYLIMIT 15
EXPORT FLAG binbug_edit_screen(UWORD code)
{   int i,
        kx, ky;

    kx =  scrnaddr           % 64;
    ky = (scrnaddr - 0x7800) / 64;

    switch (code)
    {
    case  SCAN_UP:               if (shift()) ky =  0; elif (ky ==  0) ky = KYLIMIT; else ky--;
    acase SCAN_DOWN:             if (shift()) ky = KYLIMIT; elif (ky == KYLIMIT) ky =  0; else ky++;
    acase SCAN_LEFT:             if (shift()) kx =  0; elif (kx ==  0) kx = KXLIMIT; else kx--;
    acase SCAN_RIGHT:            if (shift()) kx = KXLIMIT; elif (kx == KXLIMIT) kx =  0; else kx++;
    acase SCAN_BACKSPACE:        if (kx == 0 && ky == 0)
                                 {   kx = KXLIMIT;
                                     ky = KYLIMIT;
                                 } elif (kx == 0)
                                 {   kx = KXLIMIT;
                                     ky--;
                                 } else
                                 {   kx--;
                                 }
                                 scrnaddr = 0x7800 + kx + (ky * 64);
                                 memory[scrnaddr] = 0x20;
    acase SCAN_DEL:              if (shift())
                                 {   for (i = 0x7800; i <= 0x7BFF; i++)
                                     {   memory[i] = 0x20;
                                 }   }
                                 else
                                 {   memory[scrnaddr] = 0x20;
                                 }
    acase SCAN_AE: case SCAN_NE: kx = 0;
                                 if (ky == KYLIMIT)
                                 {   ky = 0;
                                 } else
                                 {   ky++;
                                 }
#ifdef WIN32
    acase SCAN_HOME:             kx =  0; if (shift()) ky =  0;
    acase SCAN_END:              kx = KXLIMIT; if (shift()) ky = KYLIMIT;
    acase SCAN_PGUP:             memory[scrnaddr]++; // overflow  is OK
    acase SCAN_PGDN:             memory[scrnaddr]--; // underflow is OK
#endif
#ifdef AMIGA
    acase SCAN_HELP:             if (shift())
                                 {   memory[scrnaddr]--; // underflow is OK
                                 } else
                                 {   memory[scrnaddr]++; // overflow  is OK
                                 }
#endif
    acase SCAN_ESCAPE:           editscreen = FALSE;
                                 updatemenus();
                                 settitle();
                              // dogamename();
    adefault:
        switch (code)
        {
        case  SCAN_LSHIFT:
        case  SCAN_RSHIFT:
        case  SCAN_CAPSLOCK:
            return FALSE;
        case  SCAN_N1: memory[scrnaddr] =                  0x31;
        acase SCAN_N2: memory[scrnaddr] =                  0x32;
        acase SCAN_N3: memory[scrnaddr] =                  0x33;
        acase SCAN_N4: memory[scrnaddr] =                  0x34;
        acase SCAN_N5: memory[scrnaddr] =                  0x35;
        acase SCAN_N6: memory[scrnaddr] =                  0x36;
        acase SCAN_N7: memory[scrnaddr] =                  0x37;
        acase SCAN_N8: memory[scrnaddr] =                  0x38;
        acase SCAN_N9: memory[scrnaddr] =                  0x39;
        acase SCAN_N0: memory[scrnaddr] =                  0x30;
        acase SCAN_A1: memory[scrnaddr] = shift() ? 0x21 : 0x31; // !
        acase SCAN_A2: memory[scrnaddr] = shift() ? 0x40 : 0x32; // @
        acase SCAN_A3: memory[scrnaddr] = shift() ? 0x23 : 0x33; // #
        acase SCAN_A4: memory[scrnaddr] = shift() ? 0x24 : 0x34; // $
        acase SCAN_A5: memory[scrnaddr] = shift() ? 0x25 : 0x35; // %
        acase SCAN_A6: memory[scrnaddr] = shift() ? 0x5E : 0x36; // ^
        acase SCAN_A7: memory[scrnaddr] = shift() ? 0x26 : 0x37; // '
        acase SCAN_A8: memory[scrnaddr] = shift() ? 0x2A : 0x38; // *
        acase SCAN_A9: memory[scrnaddr] = shift() ? 0x28 : 0x39; // (
        acase SCAN_A0: memory[scrnaddr] = shift() ? 0x29 : 0x30; // )
        acase SCAN_A:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x61 : 0x41;
        acase SCAN_B:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x62 : 0x42;
        acase SCAN_C:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x63 : 0x43;
        acase SCAN_D:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x64 : 0x44;
        acase SCAN_E:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x65 : 0x45;
        acase SCAN_F:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x66 : 0x46;
        acase SCAN_G:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x67 : 0x47;
        acase SCAN_H:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x68 : 0x48;
        acase SCAN_I:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x69 : 0x49;
        acase SCAN_J:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x6A : 0x4A;
        acase SCAN_K:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x6B : 0x4B;
        acase SCAN_L:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x6C : 0x4C;
        acase SCAN_M:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x6D : 0x4D;
        acase SCAN_N:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x6E : 0x4E;
        acase SCAN_O:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x6F : 0x4F;
        acase SCAN_P:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x70 : 0x50;
        acase SCAN_Q:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x71 : 0x51;
        acase SCAN_R:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x72 : 0x52;
        acase SCAN_S:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x73 : 0x53;
        acase SCAN_T:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x74 : 0x54;
        acase SCAN_U:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x75 : 0x55;
        acase SCAN_V:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x76 : 0x56;
        acase SCAN_W:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x77 : 0x57;
        acase SCAN_X:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x78 : 0x58;
        acase SCAN_Y:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x79 : 0x59;
        acase SCAN_Z:  memory[scrnaddr] = (lowercase && !capslock && !shift()) ? 0x7A : 0x5A;
        acase SCAN_OB:         memory[scrnaddr] =                     shift()  ? 0x7B : 0x5B;
        acase SCAN_BACKSLASH:  memory[scrnaddr] =                     shift()  ? 0x7C : 0x5C;
        acase SCAN_CB:         memory[scrnaddr] =                     shift()  ? 0x7D : 0x5D;
        acase SCAN_SPACEBAR:   memory[scrnaddr] =                     shift()  ? 0x7F : 0x20;
        acase SCAN_APOSTROPHE: memory[scrnaddr] =                     shift()  ? 0x22 : 0x27;
        acase SCAN_AS:         memory[scrnaddr] =                                       0x2A;
        acase SCAN_PL:         memory[scrnaddr] =                                       0x2B;
        acase SCAN_COMMA:      memory[scrnaddr] =                     shift()  ? 0x3C : 0x2C;
        acase SCAN_AMINUS:     memory[scrnaddr] =                     shift()  ? 0x5F : 0x2D;
        acase SCAN_MI:         memory[scrnaddr] =                                       0x2D;
        acase SCAN_FULLSTOP:   memory[scrnaddr] =                     shift()  ? 0x3E : 0x2E;
        acase SCAN_ND:         memory[scrnaddr] =                                       0x2E;
        acase SCAN_SLASH:      memory[scrnaddr] =                     shift()  ? 0x3F : 0x2F;
        acase SCAN_SL:         memory[scrnaddr] =                                       0x2F;
        acase SCAN_SEMICOLON:  memory[scrnaddr] =                     shift()  ? 0x3A : 0x3B;
        acase SCAN_EQUALS:     memory[scrnaddr] =                     shift()  ? 0x2B : 0x3D;
        acase SCAN_BACKTICK:   memory[scrnaddr] =                     shift()  ? 0x7E : 0x60;
#ifdef AMIGA
        acase SCAN_OP:         memory[scrnaddr] =                                       0x28;
        acase SCAN_CP:         memory[scrnaddr] =                                       0x29;
#endif
        adefault:              return FALSE;
        }
        if (kx == KXLIMIT && ky == KYLIMIT)
        {   kx = ky = 0;
        } elif (kx == KXLIMIT)
        {   kx = 0;
            ky++;
        } else
        {   kx++;
    }   }
    scrnaddr = 0x7800 + kx + (ky * 64);

    return TRUE; // return code is whether we ate the keystroke
}

EXPORT void binbug_delete_file(int whichfile, int whichdrive)
{   int curblock,
        freesize,
        gstartblock, gendblock,
        startblock,  endblock,
        startbyte,   endbyte;

    gstartblock = (drive[whichdrive].contents[2] *  10) + (drive[whichdrive].contents[3] - 1);
    gendblock   = (drive[whichdrive].contents[4] *  10) + (drive[whichdrive].contents[5] - 1);
    startbyte   = (whichfile * 24) + (((whichfile / 10) + 1) * 16);
    startblock  = (drive[whichdrive].contents[startbyte + 13] * 10) + (drive[whichdrive].contents[startbyte + 14] - 1);
    endblock    = (drive[whichdrive].contents[startbyte + 15] * 10) + (drive[whichdrive].contents[startbyte + 16] - 1);
    startbyte   = (startblock * 256) + 2;
    curblock    = startblock;
    freesize    = (drive[whichdrive].contents[6] * 256) +  drive[whichdrive].contents[7];

    for (;;)
    {   endbyte = (curblock * 256) + 255;
        if (startbyte <= endbyte)
        {   memset(&drive[whichdrive].contents[startbyte], 0xE5, (size_t) (endbyte - startbyte + 1));
            freesize++;
            if (curblock == gstartblock)
            {   gstartblock = curblock + 1;
            }
            if (curblock == gendblock)
            {   gendblock   = curblock - 1;
        }   }
        if (curblock == endblock)
        {   break;
        }
        curblock  = (drive[whichdrive].contents[ curblock * 256     ] * 10)
                  + (drive[whichdrive].contents[(curblock * 256) + 1] -  1);
        startbyte = (curblock * 256) + 2;
    }

    drive[whichdrive].contents[4] =  endblock  / 10;      // set end of...
    drive[whichdrive].contents[5] = (endblock  % 10) + 1; // ...free-chain
    drive[whichdrive].contents[6] = freesize / 256;
    drive[whichdrive].contents[7] = freesize % 256;

    // link first block of file to last block of free-chain
    drive[whichdrive].contents[ gendblock * 256     ] =  startblock / 10;
    drive[whichdrive].contents[(gendblock * 256) + 1] = (startblock % 10) + 1;

    memset(&drive[whichdrive].contents[(16 * ((whichfile / 10) + 1)) + (whichfile * 24)], 0, 24);
}

EXPORT FLAG binbug_rename_file(int whichfile, STRPTR newname)
{   int  i, j,
         thelength,
         where;
    TEXT filepart[10 + 1],
         extpart[  3 + 1];

    for (i = 0; i < 10; i++)
    {   if (newname[i] == '.' || newname[i] == EOS)
        {   goto DONE1;
    }   }
    if (newname[10] != '.' && newname[10] != EOS)
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FILENAMEISTOOLONG, "Filename is too long!\n\n"));
        return FALSE;
    }

DONE1:
    zstrncpy(filepart, newname, i);

    if (newname[i] == '.')
    {   for (j = 1; j <= 3; j++)
        {   if (newname[i + j] == EOS)
            {   zstrncpy(extpart, &newname[i + 1], j);
                goto DONE2;
        }   }
        if (newname[i + 3 + 1] != EOS)
        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_EXTENSIONISTOOLONG, "Extension is too long!\n\n"));
            return FALSE;
        }
        zstrncpy(extpart, &newname[i + 1], j);
    } else
    {   extpart[0] = EOS;
    }

DONE2:
    where = (((whichfile / 10) + 1) * 16) // identity sections
          +    whichfile            * 24;

    memset(&drive[debugdrive].contents[where], 0, 13);
    thelength = strlen(filepart);
    for (i = 0; i < thelength; i++)
    {   drive[debugdrive].contents[where      + i] = toupper(filepart[i]);
    }
    thelength = strlen(extpart);
    for (i = 0; i < thelength; i++)
    {   drive[debugdrive].contents[where + 10 + i] = toupper(extpart[ i]);
    }

    return TRUE;
}

EXPORT void binbug_changebaud(void)
{   // assert(machine == BINBUG);

    if (binbug_biosver != BINBUG_61)
    {   binbug_baudrate = BINBUG_BAUDRATE_300;
    }

    switch (binbug_baudrate)
    {
    case  BINBUG_BAUDRATE_150:  cpb = 6667; if (binbug_biosver == BINBUG_61) { memory[0x42D] = 0xDD; memory[0x42E] = 0xDA; } // 6666.6' CPU cycles per teletype bit ( 150 baud at 1 MHz or  300 baud at 2 MHz)
    acase BINBUG_BAUDRATE_300:  cpb = 3333; if (binbug_biosver == BINBUG_61) { memory[0x42D] = 0x6E; memory[0x42E] = 0x6B; } // 3333.3' CPU cycles per teletype bit ( 300 baud at 1 MHz or  600 baud at 2 MHz)
    acase BINBUG_BAUDRATE_1200: cpb =  833; if (binbug_biosver == BINBUG_61) { memory[0x42D] = 0x1A; memory[0x42E] = 0x18; } //  833.3' CPU cycles per teletype bit (1200 baud at 1 MHz or 2400 baud at 2 MHz)
    acase BINBUG_BAUDRATE_2400: cpb =  417; if (binbug_biosver == BINBUG_61) { memory[0x42D] = 0x0C; memory[0x42E] = 0x0A; } //  416.6' CPU cycles per teletype bit (2400 baud at 1 MHz or 4800 baud at 2 MHz)
}   }

EXPORT void binbug_serialize_cos(void)
{   int i,
        temp,
        whichdrive;

    for (i = 0; i <= 0x7FFF; i++)
    {   serialize_byte(&memory[i]);
    }
    serialize_byte_int(&binbug_biosver);
    serialize_byte_int(&binbug_baudrate);
    binbug_changebaud();
    serialize_byte_int(&tt_kybdstate);
    serialize_byte(    &tt_kybdcode);
    serialize_long(    &tt_kybdtill);
    temp = 0;
    serialize_byte_int(&temp);
    if (serializemode == SERIALIZE_READ)
    {   calc_margins(); // because vdu might have changed
        for (whichdrive = 0; whichdrive < machines[machine].drives; whichdrive++)
        {   for (i = 0; i < BINBUG_DISKSIZE / 8; i++)
            {   drive[whichdrive].flags[i] = 0;
    }   }   }
    serialize_byte(&binbug_joyunit);
    serialize_byte_int(&binbug_biosver);
    serialize_byte_int(&binbug_dosver);
    temp = 0;
    serialize_byte_int(&temp);
    for (whichdrive = 0; whichdrive < 2; whichdrive++)
    {   serialize_byte((UBYTE*) &drive[whichdrive].inserted);
        if (drive[whichdrive].inserted)
        {   for (i = 0; i < BINBUG_DISKSIZE; i++)
            {   serialize_byte(&drive[whichdrive].contents[i]);
        }   }
        serialize_byte(&drive[whichdrive].track);
        serialize_byte(&drive[whichdrive].sector);
        serialize_long((ULONG*) &drive[whichdrive].blockoffset);
        serialize_byte_int((int*) &drive[whichdrive].writeprotect);
    }
    serialize_byte_int((int*) &curdrive);
    serialize_byte_int(&drive_mode);
    serialize_byte_int(&stepdir);
    if (stepdir == 0xFF)
    {   stepdir = -1;
    }
    for (i = 0; i < machines[machine].drives; i++)
    {   serialize_byte((UBYTE*) &drive[i].spinning);
        serialize_byte((UBYTE*) &drive[i].headloaded);
    }
    serialize_byte(&reqsector); 
    serialize_byte(&trackreg);
    serialize_byte((UBYTE*) &multimode);
    serialize_byte_int(&binbug_userdrives);
    serialize_byte(&ioport[   0].contents);
    serialize_byte(&ioport[   3].contents);
    serialize_byte(&ioport[0x32].contents);
    serialize_byte(&ioport[PORTC].contents);
    serialize_byte(&glow); // not necessarily always the same as ioport[0].contents
    serialize_byte((UBYTE*) &paperreaderenabled);
    serialize_byte((UBYTE*) &paperpunchenabled);
    if (cosversion >= 43)
    {   serialize_byte_int(&belling[0]);
    } else
    {   belling[0] = 0;
    }
    serialize_long((ULONG*) &timeoutat);

    if (serializemode == SERIALIZE_READ)
    {   for (whichdrive = 0; whichdrive < machines[machine].drives; whichdrive++)
        {   drive[whichdrive].fn_disk[0] = EOS;
            binbug_dir_disk(TRUE, whichdrive);
}   }   }

EXPORT void binbug_reset(void)
{   // keyboard
    tt_kybdstate = 0;
    tt_kybdtill  = 0;
    queuepos     = 0;
    psu |= PSU_S | PSU_F;
    binbug_changebaud(); // probably not needed?

    // joystick
    binbug_joyunit = 0;

    reset_drives();

    // sound
    guestpitch[TONE_1STXVI] = 0;
    playsound(FALSE);

    if (game)
    {   iar = (startaddr_h * 256) + startaddr_l;
    } else
    {   iar = 0;
}   }

MODULE int bcd_to_normal(UBYTE value)
{   if
    (   (value & 0xF0) >= 0xA0
     || (value & 0x0F) >= 0x0A
    )
    {   return 0; // error
    }

    return (((value & 0xF0) >> 4) * 10)
          + ( value & 0x0F      );
}

MODULE void draw_binbug(void)
{   FAST int   x, xx,
               y, yy;
    FAST UBYTE colour,
               t,
               tempvdu;

    for (x = 0; x < 64; x++)
    {   for (y = 0; y < 16; y++)
        {   colour = inverse ? WHITE : BLACK;
            for (yy = 0; yy < DG640_CHARHEIGHT; yy++)
            {   for (xx = 0; xx < DG640_CHARWIDTH; xx++)
                {   if (screen[(x * DG640_CHARWIDTH) + xx][(y * DG640_CHARHEIGHT) + yy] != colour)
                    {   changepixel((x * DG640_CHARWIDTH) + xx, (y * DG640_CHARHEIGHT) + yy, colour);
    }   }   }   }   }

    for (x = 0; x < 64; x++)
    {   for (y = 0; y < 16; y++)
        {   tempvdu = memory[0x7800 + (y * 64) + x];
            colour = inverse ? BLACK : WHITE;
            for (yy = 0; yy < DG640_CHARHEIGHT; yy++)
            {   if (memory[0x7C00 + (y * 64) + x] & 4)
                {   t = memory[0x7000 + (tempvdu * 16) + yy];
                } elif (memory[0x7C00 + (y * 64) + x] & 2)
                {   t = dg640_gfx[tempvdu][yy];
                } else
                {   t = dg640_chars[tempvdu & 0x7F][yy];
                    if (tempvdu & 0x80)
                    {   t = ~t;
                }   }
                if ((memory[0x7C00 + (y * 64) + x] & 1) && (frames % 50) < 25)
                {   if (coomer)
                    {   t = ~t; // inverse
                    } else
                    {   t = 0; // blank
                }   }

                for (xx = 0; xx < 8; xx++)
                {   if (t & (0x80 >> xx))
                    {   changepixel((x * DG640_CHARWIDTH) + xx, (y * DG640_CHARHEIGHT) + yy, colour);
                }   }
#ifdef NINTHPIXEL
                if (t & 1)
                {   changepixel((x * DG640_CHARWIDTH) + 8, (y * DG640_CHARHEIGHT) + yy, colour); // 9th pixel
                }
#endif
}   }   }   }

EXPORT void binbug_inject_file(STRPTR thefilename)
{   TRANSIENT int   freesize,
                    freeblocks,
                    i, j,
                    startblock,
                    curblock,
                    numblocks,
                    remaining,
                    thelength,
                    thesize,
                    where,
                    whichfile;
    TRANSIENT TEXT  filepart[10 + 1],
                    extpart[3 + 1];
    TRANSIENT FILE* TheLocalHandle /* = NULL */ ;
    FAST      int   nextblock = -1; // initialized to avoid a spurious SAS/C warning

    cd_projects();

    for (i = 0; i < 10; i++)
    {   if (thefilename[i] == '.' || thefilename[i] == EOS)
        {   goto DONE1;
    }   }
    if (thefilename[10] != '.' && thefilename[10] != EOS)
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FILENAMEISTOOLONG, "Filename is too long!\n\n"));
        goto END;
    }

DONE1:
    zstrncpy(filepart, thefilename, i);

    if (thefilename[i] == '.')
    {   for (j = 1; j <= 3; j++)
        {   if (thefilename[i + j] == EOS)
            {   zstrncpy(extpart, &thefilename[i + 1], j);
                goto DONE2;
        }   }
        if (thefilename[i + 3 + 1] != EOS)
        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_EXTENSIONISTOOLONG, "Extension is too long!\n\n"));
            goto END;
        }
        zstrncpy(extpart, &thefilename[i + 1], j);
    } else
    {   extpart[0] = EOS;
    }

DONE2:
    thesize  = getsize(thefilename);
    freesize = ((drive[debugdrive].contents[6] * 256) + drive[debugdrive].contents[7]) * 254;
    if (thesize > freesize)
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_DISKISTOOFULL, "Disk is too full!\n\n"));
        goto END;
    }

    whichfile = -1;
    for (i = 0; i < 100; i++)
    {   if (drive[debugdrive].filename[i][debugdrive] != EOS)
        {   drive[debugdrive].filename[i][10] = EOS; // to avoid a spurious GCC warning
            if
            (   !stricmp((const char*) filepart, (const char*) drive[debugdrive].filename[i])
             && !stricmp((const char*) extpart,  (const char*) drive[debugdrive].fileext[ i])
            )
            {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FILEALREADYEXISTS, "File already exists on disk!\n\n"));
                goto END;
        }   }
        else
        {   if (whichfile == -1)
            {   whichfile = i;
    }   }   }
    if (whichfile == -1)
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NOFREEENTRIES, "No free entries in directory table!\n\n"));
        goto END;
    }

    if (!(TheLocalHandle = fopen(thefilename, "rb")))
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_CANTOPENFILEFORINPUT, "Can't open file \"%s\" for input!\n\n"), thefilename);
        goto END;
    }

    remaining  = thesize;
    startblock =
    curblock   = ( drive[debugdrive].contents[2] *  10) + (drive[debugdrive].contents[3] - 1);
    freeblocks = ((drive[debugdrive].contents[6] * 256) +  drive[debugdrive].contents[7]    );
    numblocks  = 0;
    for (;;)
    {   nextblock = (drive[debugdrive].contents[ curblock * 256     ] * BINBUG_SECTORS)
                  +  drive[debugdrive].contents[(curblock * 256) + 1]
                  - 1;
        if (remaining <= 254)
        {   drive[debugdrive].contents[curblock * 256] = 0xFF;
            DISCARD fread(&drive[debugdrive].contents[(curblock * 256) + 2], remaining, 1, TheLocalHandle);
            if (remaining < 254)
            {   memset(&drive[debugdrive].contents[(curblock * 256) + 2 + remaining], 0, 254 - remaining); // clear unused area
            }
            numblocks++;
            freeblocks--;
            goto DONE3;
        } else
        {   drive[debugdrive].contents[curblock * 256] = nextblock / BINBUG_SECTORS;
            DISCARD fread(&drive[debugdrive].contents[(curblock * 256) + 2], 254, 1, TheLocalHandle);
            remaining -= 254;
            numblocks++;
            freeblocks--;
            if (freeblocks == 0)
            {   zprintf(TEXTPEN_CLIOUTPUT, "No more free blocks (corrupt disk?)!\n\n");
                goto DONE3;
            }
            curblock = nextblock;
    }   }

DONE3:
    DISCARD fclose(TheLocalHandle);
    // TheLocalHandle = NULL;

    drive[debugdrive].contents[2] =  nextblock  / BINBUG_SECTORS;      // start track  of free area
    drive[debugdrive].contents[3] = (nextblock  % BINBUG_SECTORS) + 1; // start sector of free area
    drive[debugdrive].contents[6] =  freeblocks / 256;                 // high  word of number of free blocks
    drive[debugdrive].contents[7] =  freeblocks % 256;                 // low   word of number of free blocks

    where = (((whichfile / 10) + 1) * 16) // identity sections
          +    whichfile            * 24;

    memset(&drive[debugdrive].contents[where], 0, 24); // filename and extension in particular need clearing
    thelength = strlen(filepart);
    for (i = 0; i < thelength; i++)
    {   drive[debugdrive].contents[where      + i] = toupper(filepart[i]);
    }
    thelength = strlen(extpart);
    for (i = 0; i < thelength; i++)
    {   drive[debugdrive].contents[where + 10 + i] = toupper(extpart[ i]);
    }

    drive[debugdrive].contents[where + 13] =  startblock / BINBUG_SECTORS     ; // block -> track
    drive[debugdrive].contents[where + 14] = (startblock % BINBUG_SECTORS) + 1; // block -> sector
    drive[debugdrive].contents[where + 15] =  curblock   / BINBUG_SECTORS     ; // block -> track
    drive[debugdrive].contents[where + 16] = (curblock   % BINBUG_SECTORS) + 1; // block -> sector
    drive[debugdrive].contents[where + 17] =  numblocks  / 256                ; // high word
    drive[debugdrive].contents[where + 18] =  numblocks  % 256;               ; // low  word
    drive[debugdrive].contents[where + 19] = 0;                                 // type

    read_rtc();
    drive[debugdrive].contents[where + 21] = (( rtc.day          / 10) << 4) | ( rtc.day          % 10);
    drive[debugdrive].contents[where + 22] = (( rtc.month        / 10) << 4) | ( rtc.month        % 10);
    drive[debugdrive].contents[where + 23] = (((rtc.year  % 100) / 10) << 4) | ((rtc.year  % 100) % 10);

    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
    binbug_dir_disk(FALSE, debugdrive); // we only need a quiet one, the loud one is done just as a courtesy

END:
    cd_progdir();
}

MODULE void run_cpu(int until)
{   // This is a quicker equivalent to repeatedly incrementing cpux and calling do_cpu().

    cpux = nextinst;
    while (cpux < until)
    {   oldcycles = cycles_2650;
        checkstep();
        pipbin_io();
        one_instruction();
        nextinst += (cycles_2650 - oldcycles) * (fastbinbug ? 6 : 12); // in pixels
        cpux = nextinst;
    }
    if (nextinst >= 768)
    {   nextinst -= 768;
}   }
