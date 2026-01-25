// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
#endif
#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
#endif

#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"
#ifdef AMIGA
    #include <proto/locale.h> // GetCatalogStr()
#endif

#include <stdio.h>
#include <stdlib.h>           // calloc(), EXIT_SUCCESS, EXIT_FAILURE
#include <string.h>           // strcpy()

#include "aa.h"
#include "phunsy.h"

// DEFINES----------------------------------------------------------------

#define TILTILT
// whether to tilt (italicize) the TIL311 display

#define TILCOLOUR                 PINK
// which colour to use for the TIL311 foreground

#define MDCR_SHOWTIME
// whether to show MDCR position and length as times (otherwise as CPU cycles)

#define MDCRREVSTATE_LGAP         0
#define MDCRREVSTATE_DATA         1
#define MDCRREVSTATE_SGAP         2
#define MDCRREVSTATE_BNUM         3

// port c inputs
#define TTY_IN       (1<<0)         // serial data in from tty (300 or 110 bps)
#define PAR_TTYN     (1<<1)         // if '1' keyboard and screen else tty in/out for user interface
#define BPS_300      (1<<2)         // if '1' 300 bps else 110 bps tty
#define MDCR_WEN     (1<<3)         // Write enable
#define MDCR_CIP     (1<<4)         // Cassette in position
#define MDCR_BET     (1<<5)         // Begin/End of Tape
#define MDCR_RDA     (1<<6)         // Read data
#define MDCR_RDC     (1<<7)         // Read clock

// port d outputs
#define TTY_OUT      (1<<0)         // serial data out to tty (300 or 110 bps)
#define SPEAKER      (1<<1)         // output to a small speaker
#define KEYB_ACK     (1<<2)         // Clears the keyboard flip-flop, portd bit 8 is set to '1' indicating no key
#define MDCR_TST     (1<<3)         // Test CIP ??? Clears the CIP flip-flop
#define MDCR_REV     (1<<4)         // Reverse tape
#define MDCR_FWD     (1<<5)         // Forward tape
#define MDCR_WCD     (1<<6)         // Write command
#define MDCR_WDA     (1<<7)         // Write data

#define MDCR_LEADER               333334     // 1 second
#define MDCR_BLOCK                233333     // 700 ms total data block time
#define MDCR_BLOCKDATLEN          115111     // length of data block in cpu cycles
#define MDCR_DATA                 25000      // 75 ms offset to data in each block
#define MDCR_BLOCKNUMLEN          1333       // length of blocknumber in cpu cycles
#define MDCR_BIT                  56         // 6000 bps

#define DOCPU                 \
if (cpux == nextinst)         \
{   oldcycles = cycles_2650;  \
    checkstep();              \
    mdcr_exec();              \
    do_tape();                \
    one_instruction();        \
    nextinst += (cycles_2650 - oldcycles) * 8; \
    if (nextinst >= 512)      \
    {   nextinst -= 512;      \
}   }
// nextinst range is -128..383

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       FLAG                     capslock,
                                      inframe;
IMPORT       UBYTE                    digitleds[DIGITLEDS],
                                      guestkey,
                                      keys_column[7],
                                      memory[32768],
                                      OutputBuffer[18],
                                      psu,
                                      startaddr_h, startaddr_l,
                                      vdu_fgc,
                                      vdu_bgc;
IMPORT       TEXT                     file_game[MAX_PATH + 1],
                                      fn_tape[4][MAX_PATH + 1],
                                      path_tapes[MAX_PATH + 1];
IMPORT       UBYTE*                   IOBuffer;
IMPORT       ASCREEN                  screen[BOXWIDTH][BOXHEIGHT];
IMPORT       UWORD                    iar,
                                      keypads[2][NUMKEYS],
                                      mirror_r[32768],
                                      mirror_w[32768],
                                      oldiar,
                                      ras[8];
IMPORT       ULONG                    cycles_2650,
                                      oldcycles,
                                      oldtapecycle,
                                      verbosetape;
IMPORT       int                      absxmin,
                                      cpux,
                                      cpuy,
                                      drawcorners,
                                      drawmode,
                                      drawunlit,
                                      editscreen,
                                      framebased,
                                      game,
                                      interrupt_2650,
                                      inverse,
                                      justdone,
                                      logmonitor,
                                      lowercase,
                                      machine,
                                      nextinst,
                                      offset,
                                      queuekeystrokes,
                                      queuepos,
                                      phunsy_biosver,
                                      recmode,
                                      scrnaddr,
                                      showleds,
                                      slice_2650,
                                      usemargins,
                                      whichgame,
                                      whichkeyrect;
IMPORT       MEMFLAG                  memflags[ALLTOKENS];
IMPORT       FILE*                    MacroHandle;
IMPORT       struct IOPortStruct      ioport[258];
IMPORT       struct KindStruct        filekind[KINDS];
IMPORT       struct KnownStruct       known[KNOWNGAMES];
IMPORT       struct MachineStruct     machines[MACHINES];
IMPORT const TEXT                     led_chars[128 + 1];
IMPORT const struct CodeCommentStruct codecomment[];

#ifdef AMIGA
    IMPORT   struct Catalog*          CatalogPtr;
#endif
#ifdef WIN32
    IMPORT   int                      CatalogPtr; // APTR doesn't work
#endif

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       UBYTE                    banked,
                                      ticks,
                                      rate,
                                      q_bank[16][16384],
                                      u_bank[16][2048];
EXPORT       int                      mdcrblock,
                                      mdcrblocks,
                                      mdcrfwdstate,
                                      mdcrrevstate,
                                      mdcrlength,
                                      mdcrpos,
                                      mdcrstate;

// MODULE VARIABLES-------------------------------------------------------

MODULE       UBYTE                    mdcr_bits;
MODULE       unsigned char            mdcrchecksum,
                                      mdcrwritebyte;
MODULE       int                      mdcrdatapos,
                                      mdcrbitcounter,
                                      mdcrwritebittime,
                                      mdcrportdoutold,
                                      mdcrblockpos; // position within a block

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void draw_phunsy(void);
MODULE void phunsy_emuinput(void);
MODULE void phunsy_drawdigit(int position);
MODULE void phunsy_drawdot(int position);
MODULE void phunsy_runcpu(void);
MODULE void run_cpu(int until);

// CODE-------------------------------------------------------------------

EXPORT void phunsy_setmemmap(void)
{   int i, j;

    // assert(memmap == MEMMAP_PHUNSY);

    game = FALSE;
    nextinst = -128;
    ticks = rate = 0;

    if (phunsy_biosver == PHUNSY_MINIMONITOR)
    {   for (i = 0; i <= 0x7FFF; i++)
        {   memory[i] = 0;
        }
        memory[0] = 0x1F; // BCTA,un
        memory[1] = 0x1F; // $1F
        memory[2] = 0x3F; // $  3F
        for (i = 0; i < 512; i++)
        {   memory[0x1E00 + i] = phunsy_minibios[i];
        }

        for (i = 0; i < 32 * KILOBYTE; i++)
        {   memflags[i] = 0;

            if   (i >= 0x1000 && i <= 0x17FF) memflags[i] |= VISIBLE;
            elif (i >= 0x1DF0 && i <= 0x1DFD) memflags[i] |= BIOSRAM;
            elif (i >= 0x1E00 && i <= 0x1FFF) memflags[i] |= BIOS | NOWRITE;
            elif (i >= 0x2000               ) memflags[i] |= NOREAD | NOWRITE;

            mirror_r[i] =
            mirror_w[i] = (UWORD) i;
        }

        machines[PHUNSY].firstequiv       =
        machines[PHUNSY].lastequiv        =
        machines[PHUNSY].firstcodecomment =
        machines[PHUNSY].lastcodecomment  =
        machines[PHUNSY].firstdatacomment =
        machines[PHUNSY].lastdatacomment  = -1;
    } else
    {   // assert(phunsy_biosver == PHUNSY_PHUNSY);
        for (i = 0; i < 16; i++)
        {   for (j = 0; j <=  0x7FF; j++)
            {   u_bank[i][j] = 0;
            }
            for (j = 0; j <= 0x3FFF; j++)
            {   q_bank[i][j] = 0;
        }   }
        for (i = 0x800; i <= 0xBFF; i++)
        {   memory[i] = 0xFF; // this matches the memory dump from MX-SYST.BIN
        }
        for (i = 0xC00; i <= 0x7FFF; i++)
        {   memory[i] = 0;
        }
        for (i = 0; i <= 0x7FF; i++)
        {   memory[i]    = phunsy_fullbios[i];
            u_bank[1][i] = pdcr[i];
            u_bank[2][i] = dass[i];
            u_bank[3][i] = labhnd[i];
        }

        if (known[whichgame].complang == CL_MWB)
        {   for (i = 0; i <= 0x1983; i++)
            {   memory[0x4000 + i] = mwb[i];
            }
#ifdef VERBOSE
            zprintf(TEXTPEN_VERBOSE, "Loading MWB...\n");
#endif
        }

        for (i = 0; i < 32 * KILOBYTE; i++)
        {   memflags[i] = 0;

            if   (               i <=  0x7FF) memflags[i] |= BIOS | NOWRITE;
            elif (i >=  0xEE0 && i <=  0xFFF) memflags[i] |= BIOSRAM;
            elif (i >= 0x1000 && i <= 0x17FF) memflags[i] |= VISIBLE;
            elif (i >= 0x1800 && i <= 0x1FFF) memflags[i] |= BANKU;
            elif (i >= 0x4000               ) memflags[i] |= BANKQ;

            mirror_r[i] =
            mirror_w[i] = (UWORD) i;
        }

        machines[PHUNSY].firstequiv       =  976;
        machines[PHUNSY].lastequiv        = 1428;
        machines[PHUNSY].firstcodecomment = FIRSTPHUNSYCODECOMMENT;
        machines[PHUNSY].lastcodecomment  = LASTNONMDCRCODECOMMENT;
        machines[PHUNSY].firstdatacomment = FIRSTPHUNSYDATACOMMENT;
        machines[PHUNSY].lastdatacomment  = LASTNONMDCRDATACOMMENT;
        for (i = machines[machine].firstcodecomment; i <= machines[machine].lastcodecomment; i++)
        {   memflags[codecomment[i].address] |= COMMENTED;
    }   }

    whichkeyrect = KEYRECT_PHUNSY;

    banked      = 0; // U0, Q0
    ramtoubank(0);
    ramtoqbank(0);

    mdcrpos     =
    mdcrblock   = 0;
    mdcrstate   = MDCRSTATE_IDLE;
#ifdef MDCR_BUILTIN
    mdcrblocks  = 128;
#else
    mdcrblocks  = 0;
#endif
    mdcrlength  = (MDCR_LEADER * 2) + (mdcrblocks * MDCR_BLOCK);
}

EXPORT void phunsy_emulate(void)
{   FAST UBYTE fgc,
               t;
    FAST int   kx, x, x1, x2,
               ky, y, y1, y2;

    inframe = TRUE;

    if (showleds)
    {   // this segment display is persistent, not transient
        phunsy_drawdot(0); phunsy_drawdigit(0);
        phunsy_drawdot(1); phunsy_drawdigit(1);
        phunsy_drawdot(2); phunsy_drawdigit(2);
        phunsy_drawdot(3); phunsy_drawdigit(3);
        phunsy_drawdot(4); phunsy_drawdigit(4);
        phunsy_drawdot(5); phunsy_drawdigit(5);
        phunsy_drawdot(6);
    }

    if (framebased)
    {   // 1,001,600 cycles per second / 50 frames per second = 20032 cycles per frame

        slice_2650 += 10016;
        phunsy_runcpu();
        if (ticks)
        {   ticks--;
            if (ticks == 0)
            {   ticks = rate;
                interrupt_2650 = TRUE;
                checkinterrupt();
        }   }

        slice_2650 += 10016;
        phunsy_runcpu();
        if (ticks)
        {   ticks--;
            if (ticks == 0)
            {   ticks = rate;
                interrupt_2650 = TRUE;
                checkinterrupt();
        }   }

        // Our ticks are only approximate (ideally they would occur based on the precise cycle of the
        // initiating WRTE command).

        draw_phunsy();
    } else
    {   cpux = cpuy = 0;
        if (phunsy_biosver == PHUNSY_PHUNSY && ticks) // start/end of frame
        {   ticks--;
            if (ticks == 0)
            {   ticks = rate;
                interrupt_2650 = TRUE;
                checkinterrupt();
        }   }

        for (cpuy = 0; cpuy <= 255; cpuy++)
        {   breakrastline();
            if (cpuy == 156 && phunsy_biosver == PHUNSY_PHUNSY && ticks)
            {   ticks--;
                if (ticks == 0)
                {   ticks = rate;
                    interrupt_2650 = TRUE;
                    checkinterrupt();
            }   }
            y1 = cpuy / 8;
            y2 = cpuy % 8;
            run_cpu(128);
            for (cpux = 128; cpux < 512; cpux++)
            {   if (cpuy == 156 && cpux == 256 && ticks) // halfway through frame
                {   ticks--;
                    if (ticks == 0)
                    {   ticks = rate;
                        interrupt_2650 = TRUE;
                        checkinterrupt();
                }   }
                x1 = (cpux - 128) / 6;
                x2 = (cpux - 128) % 6;
#ifdef SHOWCHARSET
                if (x1 < 16 && y1 < 16) t = (y1 * 16) + x1; else
#endif
                t = memory[0x1000 + (y1 * 64) + x1];
                if (t >= 128)
                {   if ((t & 0x70) == 0)
                    {   fgc = vdu_bgc;
                    } elif ((t & 0x70) == 0x70)
                    {   fgc = vdu_fgc;
                    } else
                    {   if (inverse)
                        {   fgc = 22 - ((t & 0x70) >> 4); // 1..6 -> 21..16
                        } else
                        {   fgc = 15 + ((t & 0x70) >> 4); // 1..6 -> 16..21
                    }   }
                    t &= 0x0F;
                    if (phunsy_gfx[t][y2] & (0x20 >> x2))
                    {   changethisabspixel(fgc);
                    } else
                    {   changethisabspixel(vdu_bgc);
                }   }
                else
                {   if (t == 0)
                    {   t = 0x20; // maybe we shouldn't be doing this?
                    } elif (t <= 0x1F)
                    {   t += 0x40;
                    } elif (t >= 0x40 && t <= 0x5F)
                    {   t -= 0x40;
                    }
                    if (phunsy_chars[t][y2] & (0x20 >> x2))
                    {   changethisabspixel(vdu_fgc);
                    } else
                    {   changethisabspixel(vdu_bgc);
                }   }
                DOCPU;
        }   }
        for (cpuy = 256; cpuy <= 312; cpuy++)
        {   breakrastline();
            run_cpu(512);
    }   }

    if (editscreen)
    {   kx =  scrnaddr           % 64;
        ky = (scrnaddr - 0x1000) / 64;
        x1 = (kx * 6) - 1;
        x2 = (kx * 6) + 6;
        y1 = (ky * 8) - 1;
        y2 = (ky * 8) + 8;
        for (x = x1; x <= x2; x++)
        {   for (y = y1; y <= y2; y++)
            {   if
                (   (x == x1 || x == x2 || y == y1 || y == y2)
                 &&  x >=  0
                 &&  x < 384
                 &&  y >=  0
                 &&  y < 256
                )
                {   changepixel(x, y, RED);
    }   }   }   }

    wa_checkinput();
    phunsy_emuinput();

    if (drawmode)
    {   phunsy_drawhelpgrid();
    }
    endofframe(vdu_bgc);
}

EXPORT void phunsy_writeport(int port, UBYTE data)
{   switch (port)
    {
    case 0x12:
        ioport[port].contents = data;
        phunsy_drawdigit(4);
        phunsy_drawdigit(5);
    acase 0x11:
        ioport[port].contents = data;
        phunsy_drawdigit(2);
        phunsy_drawdigit(3);
    acase 0x10:
        ioport[port].contents = data;
        phunsy_drawdigit(0);
        phunsy_drawdigit(1);
    acase 0x13:
        ioport[port].contents = data;
        phunsy_drawdot(1);
        phunsy_drawdot(2);
        phunsy_drawdot(3);
        phunsy_drawdot(4);
        phunsy_drawdot(5);
        phunsy_drawdot(6);
    acase 126: // $7E
        ticks = rate = data;
    acase 127: // write to logfile, a la PHUNSY simulator
        zprintf(TEXTPEN_LOG, "%c", data);
    acase PORTC:
        if ((data & 0xF0) != (banked & 0xF0))
        {   ramtoubank((banked & 0xF0) >> 4);
            ubanktoram((data   & 0xF0) >> 4);
        }
        if ((data & 0x0F) != (banked & 0x0F))
        {   ramtoqbank( banked & 0x0F      );
            qbanktoram( data   & 0x0F      );
        }
        banked = data;
    acase PORTD:
        mdcr_bits = data;
        if (whichgame == PIANOPOS && iar == 0x1827)
        {   playsound(FALSE); // rest
    }   }

    logport(port, data, TRUE);
}

EXPORT UBYTE phunsy_readport(int port)
{   TRANSIENT UBYTE t          = 0; // inititialized to avoid spurious SAS/C compiler warnings
    PERSIST   UBYTE lastkey;
    PERSIST   int   localfresh = 0;

    switch (port)
    {
    case 0x10:
        if   (keys_column[0] & 1) /* 0        */ t = 0x00;
        elif (keys_column[0] & 2) /* 1        */ t = 0x01;
        elif (keys_column[0] & 4) /* 2        */ t = 0x02;
        elif (keys_column[0] & 8) /* 3        */ t = 0x03;
        elif (keys_column[1] & 1) /* 4        */ t = 0x04;
        elif (keys_column[1] & 2) /* 5        */ t = 0x05;
        elif (keys_column[1] & 4) /* 6        */ t = 0x06;
        elif (keys_column[1] & 8) /* 7        */ t = 0x07;
        elif (keys_column[2] & 1) /* 8        */ t = 0x08;
        elif (keys_column[2] & 2) /* 9        */ t = 0x09;
        elif (keys_column[2] & 4) /* A        */ t = 0x0A;
        elif (keys_column[2] & 8) /* B        */ t = 0x0B;
        elif (keys_column[3] & 1) /* C        */ t = 0x0C;
        elif (keys_column[3] & 2) /* D        */ t = 0x0D;
        elif (keys_column[3] & 4) /* E        */ t = 0x0E;
        elif (keys_column[3] & 8) /* F        */ t = 0x0F;
        elif (keys_column[4] & 1) /* D->M     */ t = 0x10;
        elif (keys_column[4] & 2) /* M->D     */ t = 0x11;
        elif (keys_column[4] & 4) /* Go to    */ t = 0x12;
        elif (keys_column[4] & 8) /* Clear    */ t = 0x13;
        // The sixth column is handled elsewhere.
        else                                   t = 0xFF;
    acase PORTC:
        t = (UBYTE) (ioport[PORTC].contents | MDCR_WEN | PAR_TTYN);
;   acase PORTD:
        if (queuekeystrokes)
        {   switch (localfresh)
            {
            case 0:
                t = get_guest_key();
                if (t != NC)
                {   lastkey = t;
                    localfresh = 1;
                }
            acase 1:
                t = lastkey;
                localfresh = 2;
            acase 2:
                t = 0x80;
                localfresh = 0;
        }   }
        else
        {   t = guestkey;
        }
    adefault:
        t = 0;
    }

    logport(port, t, FALSE);
    return t;
}

EXPORT void ramtoubank(int bank)
{   int i;

    if (phunsy_biosver == PHUNSY_MINIMONITOR)
    {   return;
    }

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Memory -> U bank #%d.\n", bank);
#endif
    for (i = 0; i <= 0x7FF; i++)
    {   u_bank[bank][i] = memory[0x1800 + i];
}   }

EXPORT void ramtoqbank(int bank)
{   int i;

    if (phunsy_biosver == PHUNSY_MINIMONITOR)
    {   return;
    }

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Memory -> Q bank #%d.\n", bank);
#endif
    for (i = 0; i <= 0x3FFF; i++)
    {   q_bank[bank][i] = memory[0x4000 + i];
}   }

EXPORT void ubanktoram(int bank)
{   int i;

    if (phunsy_biosver == PHUNSY_MINIMONITOR)
    {   return;
    }

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "U bank #%d -> memory.\n", bank);
#endif
    for (i = 0; i <= 0x7FF; i++)
    {   memory[0x1800 + i] = u_bank[bank][i];
    }

    if (bank == 0 || bank >= 4) // banks U0, U4..U15 (RAM)
    {   ;
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Bank is U%X (RAM).\n", bank);
#endif
        for (i = 0x1800; i <= 0x1FFF; i++)
        {   memflags[i] &= ~(NOWRITE | BIOS); // RAM
    }   }
    else // banks U1..U3 (EPROM)
    {   ;
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Bank is U%X (EPROM, BIOS).\n", bank);
#endif
        for (i = 0x1800; i <= 0x1FFF; i++)
        {   memflags[i] |= NOWRITE | BIOS;
    }   }

    if (bank == 1)
    {   machines[PHUNSY].lastcodecomment = LASTMDCRCODECOMMENT;
        for (i = FIRSTMDCRCODECOMMENT; i <= LASTMDCRCODECOMMENT; i++)
        {   memflags[codecomment[i].address] |= COMMENTED;
    }   }
    else
    {   machines[PHUNSY].lastcodecomment = LASTNONMDCRCODECOMMENT;
        for (i = FIRSTMDCRCODECOMMENT; i <= LASTMDCRCODECOMMENT; i++)
        {   memflags[codecomment[i].address] &= ~(COMMENTED);
    }   }

    update_memory(TRUE);
}

EXPORT void qbanktoram(int bank)
{   int i;

    if (phunsy_biosver == PHUNSY_MINIMONITOR)
    {   return;
    }

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Q bank #%d -> memory.\n", bank);
#endif
    for (i = 0; i <= 0x3FFF; i++)
    {   memory[0x4000 + i] = q_bank[bank][i];
    }

    // banks Q0..Q15 (RAM)
    // Q0 does need to be RAM!
#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Bank is Q%X (RAM).\n", bank);
#endif

    update_memory(TRUE);
}

EXPORT void phunsy_viewscreen(void)
{   TRANSIENT       int    i,
                           y;
    PERSIST   const STRPTR bcd_to_ascii = "0123456789ABCDEF";

    // assert(machine == PHUNSY);

    for (y = 0; y < 32; y++)
    {   for (i = 0; i < 4; i++)
        {   dumprow(0x1000 + (y * 64) + (i * 16));
        }
        zprintf(TEXTPEN_VIEW, "\n");
    }

    zprintf
    (   TEXTPEN_VIEW,
        "%02X %02X %02X %02X  %c%c%c%c%c%c%c%c%c%c%c%c%c\n\n",
        ioport[0x10].contents,
        ioport[0x11].contents,
        ioport[0x12].contents,
        ioport[0x13].contents,                             ( memory[oldiar] == 0x40) ? '.' : ' ',
        bcd_to_ascii[(ioport[0x10].contents & 0xF0) >> 4], ((ioport[0x13].contents & 7) == 1) ? '.' : ' ',
        bcd_to_ascii[ ioport[0x10].contents & 0x0F      ], ((ioport[0x13].contents & 7) == 2) ? '.' : ' ',
        bcd_to_ascii[(ioport[0x11].contents & 0xF0) >> 4], ((ioport[0x13].contents & 7) == 3) ? '.' : ' ',
        bcd_to_ascii[ ioport[0x11].contents & 0x0F      ], ((ioport[0x13].contents & 7) == 4) ? '.' : ' ',
        bcd_to_ascii[(ioport[0x12].contents & 0xF0) >> 4], ((ioport[0x13].contents & 7) == 5) ? '.' : ' ',
        bcd_to_ascii[ ioport[0x12].contents & 0x0F      ], ((ioport[0x13].contents & 7) == 6) ? '.' : ' '
    );
}

EXPORT float phunsy_getpianopitch(UWORD value)
{   PERSIST const float phunsy_pitch[13 + 1] = {
    (const float)    0.00,
    (const float)  261.63, //  1: middle C
    (const float)  277.18, //  2: middle C#
    (const float)  293.66, //  3: middle D
    (const float)  311.13, //  4: middle D#
    (const float)  329.63, //  5: middle E
    (const float)  349.23, //  6: middle F
    (const float)  369.99, //  7: middle F#
    (const float)  392.00, //  8: middle G
    (const float)  415.30, //  9: middle G#
    (const float)  440.00, // 10: middle A
    (const float)  466.16, // 11: middle A#
    (const float)  493.88, // 12: middle B
    (const float)  523.25  // 13:   high C
};

    if (value > 13)
    {   return 0.0;
    } // implied else
    return phunsy_pitch[value];
}

EXPORT float phunsy_getbelmachpitch(UWORD value)
{   PERSIST const float phunsy_pitch[62] = {
    (const float) ( 233.08 / 2.0), // $00: vlow   A#
    (const float) ( 246.94 / 2.0), // $01: vlow   B
    (const float) ( 261.63 / 2.0), // $02: low    C
    (const float) ( 277.18 / 2.0), // $03: low    C#
    (const float) ( 293.66 / 2.0), // $04: low    D
    (const float) ( 311.13 / 2.0), // $05: low    D#
    (const float) ( 329.63 / 2.0), // $06: low    E
    (const float) ( 349.23 / 2.0), // $07: low    F
    (const float) ( 369.99 / 2.0), // $08: low    F#
    (const float) ( 392.00 / 2.0), // $09: low    G
    (const float) ( 415.30 / 2.0), // $0A: low    G#
    (const float)   220.00,        // $0B: low    A
    (const float)   233.08,        // $0C: low    A#
    (const float)   246.94,        // $0D: low    B
    (const float)   261.63,        // $0E: middle C
    (const float)   277.18,        // $0F: middle C#
    (const float)   293.66,        // $10: middle D
    (const float)   311.13,        // $11: middle D#
    (const float)   329.63,        // $12: middle E
    (const float)   349.23,        // $13: middle F
    (const float)   369.99,        // $14: middle F#
    (const float)   392.00,        // $15: middle G
    (const float)   415.30,        // $16: middle G#
    (const float)   440.00,        // $17: middle A
    (const float)   466.16,        // $18: middle A#
    (const float)   493.88,        // $19: middle B
    (const float) ( 523.25 * 1.0), // $1A:  high  C
    (const float) ( 554.37 * 1.0), // $1B:  high  C#
    (const float) ( 587.32 * 1.0), // $1C:  high  D
    (const float) ( 622.25 * 1.0), // $1D:  high  D#
    (const float) ( 659.26 * 1.0), // $1E:  high  E
    (const float) ( 698.46 * 1.0), // $1F:  high  F
    (const float) ( 739.98 * 1.0), // $20:  high  F#
    (const float) ( 783.99 * 1.0), // $21:  high  G
    (const float) ( 830.60 * 1.0), // $22:  high  G#
    (const float) ( 880.00 * 1.0), // $23:  high  A
    (const float) ( 932.32 * 1.0), // $24:  high  A#
    (const float) ( 987.80 * 1.0), // $25:  high  B
    (const float) ( 523.25 * 2.0), // $26: vhigh  C
    (const float) ( 554.37 * 2.0), // $27: vhigh  C#
    (const float) ( 587.32 * 2.0), // $28: vhigh  D
    (const float) ( 622.25 * 2.0), // $29: vhigh  D#
    (const float) ( 659.26 * 2.0), // $2A: vhigh  E
    (const float) ( 698.46 * 2.0), // $2B: vhigh  F
    (const float) ( 739.98 * 2.0), // $2C: vhigh  F#
    (const float) ( 783.99 * 2.0), // $2D: vhigh  G
    (const float) ( 830.60 * 2.0), // $2E: vhigh  G#
    (const float) ( 880.00 * 2.0), // $2F: vhigh  A
    (const float) ( 932.32 * 2.0), // $30: vhigh  A#
    (const float) ( 987.80 * 2.0), // $31: vhigh  B
    (const float) ( 523.25 * 3.0), // $32: xhigh  C
    (const float) ( 554.37 * 3.0), // $33: xhigh  C#
    (const float) ( 587.32 * 3.0), // $34: xhigh  D
    (const float) ( 622.25 * 3.0), // $35: xhigh  D#
    (const float) ( 659.26 * 3.0), // $36: xhigh  E
    (const float) ( 698.46 * 3.0), // $37: xhigh  F
    (const float) ( 739.98 * 3.0), // $38: xhigh  F#
    (const float) ( 783.99 * 3.0), // $39: xhigh  G
    (const float) ( 830.60 * 3.0), // $3A: xhigh  G#
    (const float) ( 880.00 * 3.0), // $3B: xhigh  A
    (const float) ( 932.32 * 3.0), // $3C: xhigh  A#
    (const float) ( 987.80 * 3.0), // $3D: xhigh  B
};

    return (float) (phunsy_pitch[value] / 2.0);
}

EXPORT void phunsy_biosdetails(int ea)
{   FLAG note = FALSE;

    if (phunsy_biosver == PHUNSY_MINIMONITOR)
    {   return;
    }

    // assert(machine == PHUNSY);

    switch (ea)
    {
    case    0x40: /* CMTBS */              zprintf(TEXTPEN_LOG, ";Function: Compare two byte value with TBSR\n");
    acase   0x54: /* ICTBS */              zprintf(TEXTPEN_LOG, ";Function: Increment two byte value\n");
    acase   0x6A: /* DCTBS */              zprintf(TEXTPEN_LOG, ";Function: Decrement two byte value\n");
    acase   0x80: /* PSTBS */              zprintf(TEXTPEN_LOG, ";Function: Push two byte value\n");
    acase   0x91: /* PLTBS */              zprintf(TEXTPEN_LOG, ";Function: Pull two byte value\n");
    acase   0xA3: /* ADTBS */              zprintf(TEXTPEN_LOG, ";Function: Add two byte value\n");
    acase   0xB9: /* SBTBS */              zprintf(TEXTPEN_LOG, ";Function: Subtract two byte value\n");
    acase   0xCD: /* TINPR */              zprintf(TEXTPEN_LOG, ";Function: Test input routine (get char from input buffer)\n");
    acase   0xDB: /* TEHEX */              zprintf(TEXTPEN_LOG, ";Function: Test if hex\n" \
                                                                ";Comments: Returns CC=eq if space. Returns CC=gt if hex. Returns CC=lt if other\n");
    acase   0xF4: /* GTHEX */              zprintf(TEXTPEN_LOG, ";Function: Get hex value from input buffer\n" \
                                                                ";Comments: Returns CC=eq if no hex found. Updates the input buffer pointer\n");
    acase  0x12B: /* WRTBS */              zprintf(TEXTPEN_LOG, ";Function: Output hex word\n");
    acase  0x133: /* WCHEX */              zprintf(TEXTPEN_LOG, ";Function: Output hex byte\n");
    acase  0x14B: /* INPRT */              zprintf(TEXTPEN_LOG, ";Function: Get char from user\n" \
                                                                ";Comments: R2 indicates max characters. R3 is input buffer pointer\n");
    acase  0x18A: /* CHESC */              zprintf(TEXTPEN_LOG, ";Function: Check if Escape key was pressed\n");
    acase  0x196: /* ERROR */              zprintf(TEXTPEN_LOG, ";Function: Write error to screen and quit current action\n");
    acase  0x1AE: /* TYPWR */              zprintf(TEXTPEN_LOG, ";Function: 'Typewriter': echo everything until Escape pressed\n");
    acase  0x1B7: /* MONML */              zprintf(TEXTPEN_LOG, ";Function: Monitor main loop\n");
    acase  0x289: // CHHXD
    case   0x293: /* CHHXDA */             zprintf(TEXTPEN_LOG, ";Function: Check if the correct hex values were entered\n" \
                                                                ";Comments: R3 has the info that indicates required address values (see MNCMDB table)\n" \
                                                                ";Parts:    1. ($289..$292): Check if end address is not less than start address\n" \
                                                                ";          2. ($293..$2B7): Check if target address does not cause an address larger than 32767\n");
    acase  0x2B8: /* INMEM */              zprintf(TEXTPEN_LOG, ";Function: Inspect memory\n");
    acase  0x301: /* CGMEM */              zprintf(TEXTPEN_LOG, ";Function: Change memory\n");
    acase  0x320: /* MOMEM */              zprintf(TEXTPEN_LOG, ";Function: Move memory\n");
    acase  0x35D: /* VYMEM */              zprintf(TEXTPEN_LOG, ";Function: Verify memory\n");
    acase  0x3A8: /* WRCAS */              zprintf(TEXTPEN_LOG, ";Function: Write cassette\n" \
                                                                ";Comments: This should be compatible with Apple ][ tape format\n");
    acase  0x446: /* IDCRT */              zprintf(TEXTPEN_LOG, ";Function: Increment address for cassette routines\n");
    acase  0x460: /* RDCAS */              zprintf(TEXTPEN_LOG, ";Function: Read data from cassette\n" \
                                                                ";Comments: This should be compatible with Apple ][ tape format\n");
    acase  0x50E: /* MDCRP */              zprintf(TEXTPEN_LOG, ";Function: Set U-bank 1 and continue at MDCR software\n");
    acase  0x515: /* SETUP */              zprintf(TEXTPEN_LOG, ";Function: Select U-bank\n");
    acase  0x52D: /* SETQP */              zprintf(TEXTPEN_LOG, ";Function: Select Q-bank\n");
    acase  0x53E: /* GOMEM */              zprintf(TEXTPEN_LOG, ";Function: Go execute stuff at entered address\n");
    acase  0x547: /* DPBNK */              zprintf(TEXTPEN_LOG, ";Function: Display banks\n");
    acase  0x550: /* PRMEM */              zprintf(TEXTPEN_LOG, ";Function: Preset memory area\n");
    acase  0x56D: /* IDENT */              zprintf(TEXTPEN_LOG, ";Function: Print monitor version, etc.\n");
    acase  0x5AE: /* MINIT */              zprintf(TEXTPEN_LOG, ";Function: Monitor initialization after reset\n");
    acase  0x5E0: /* CHINP */              zprintf(TEXTPEN_LOG, ";Function: Check for key input\n");
    acase  0x5E6: /* KEYIN */              zprintf(TEXTPEN_LOG, ";Function: Wait for keyboard entry and get it\n");
    acase  0x5FA: /* WCHAR */              zprintf(TEXTPEN_LOG, ";Function: Screen routines\n" \
                                                                ";Autodoc:\n" \
                                                                ";  The screen memory is located from $1000..$17FF.\n" \
                                                                ";  It is output as a video signal to a monitor in black & white.\n" \
                                                                ";  There are 32 lines of 64 characters.\n" \
                                                                ";  Use of registers:\n" \
                                                                ";    R0        returns the character that appears from under the cursor\n" \
                                                                ";    R1..R3    are left unchanged\n");
    acase  0x7AB: /* SROUT */              zprintf(TEXTPEN_LOG, ";Function: Serial user output\n");
    acase  0x7D8: /* SERIN */              zprintf(TEXTPEN_LOG, ";Function: Serial user input\n");
    acase 0x183E: /* GENAM */              zprintf(TEXTPEN_LOG, ";Function: Get command or filename\n");
    acase 0x18B6: /* REDER */ note = TRUE; zprintf(TEXTPEN_LOG, ";Error:    Read error\n");
    acase 0x18BA: /* CASER */ note = TRUE; zprintf(TEXTPEN_LOG, ";Error:    Cassette error\n");
    acase 0x18BE: /* WRPER */ note = TRUE; zprintf(TEXTPEN_LOG, ";Error:    Cassette write protected\n");
    acase 0x18C2: /* INPER */ note = TRUE; zprintf(TEXTPEN_LOG, ";Error:    Input error\n");
    acase 0x18C6: /* NICER */ note = TRUE; zprintf(TEXTPEN_LOG, ";Error:    Not in catalog\n");
    acase 0x18CA: /* CFLER */ note = TRUE; zprintf(TEXTPEN_LOG, ";Error:    Catalog full\n");
    acase 0x18CE: /* RUNER */ note = TRUE; zprintf(TEXTPEN_LOG, ";Error:    Run error\n");
    acase 0x18D2: /* TLGER */ note = TRUE; zprintf(TEXTPEN_LOG, ";Error:    Too long\n");
    acase 0x18D6: /* LKDER */ note = TRUE; zprintf(TEXTPEN_LOG, ";Error:    Locked\n");
    acase 0x18DA: /* JSTER */ note = TRUE; zprintf(TEXTPEN_LOG, ";Error:    Just an error\n");
    acase 0x199E: /* CHECA */              zprintf(TEXTPEN_LOG, ";Function: Check if cassette still present\n");
    acase 0x1A18: /* TBKTC */              zprintf(TEXTPEN_LOG, ";Function: Temporary block to cassette\n");
    acase 0x1A3C: /* CCOUT */              zprintf(TEXTPEN_LOG, ";Function: Write byte to cassette\n");
    acase 0x1A54: /* CCHIN */              zprintf(TEXTPEN_LOG, ";Function: Read byte from cassette\n");
    acase 0x1A67: /* CRCYC */              zprintf(TEXTPEN_LOG, ";Function: Cassette read cycle\n");
    acase 0x1A6B: /* CTTBK */              zprintf(TEXTPEN_LOG, ";Function: Read one block of cassette to temporary block\n");
    acase 0x1A90: /* RFBNR */              zprintf(TEXTPEN_LOG, ";Function: Read first block number\n");
    acase 0x1A9D: /* RBKNR */              zprintf(TEXTPEN_LOG, ";Function: Read block number\n");
    acase 0x1ACA: /* RFSTB */              zprintf(TEXTPEN_LOG, ";Function: Read first block (= catalog)\n");
    acase 0x1AE8: /* WFSTB */              zprintf(TEXTPEN_LOG, ";Function: Write first block (= catalog)\n");
    acase 0x1AFC: /* CWCYC */              zprintf(TEXTPEN_LOG, ";Function: Cassette write cycle\n");
    acase 0x1B15: /* OBFWD */              zprintf(TEXTPEN_LOG, ";Function: One block forward\n");
    acase 0x1B22: /* FNDBK */              zprintf(TEXTPEN_LOG, ";Function: Find block\n");
    acase 0x1B77: /* FNDGP */              zprintf(TEXTPEN_LOG, ";Function: Find gap\n");
    acase 0x1B99: /* EXTP1 */              zprintf(TEXTPEN_LOG, ";Function: External program\n" \
                                                                ";Comments: This is executed in RAM to read or store to other banks\n");
    acase 0x1BC4: /* PLFDT */              zprintf(TEXTPEN_LOG, ";Function: Pull file data from catalog block\n");
    acase 0x1BDE: /* PSFDT */              zprintf(TEXTPEN_LOG, ";Function: Push file data to catalog block\n");
    acase 0x1BFB: /* MTCAS */              zprintf(TEXTPEN_LOG, ";Function: Memory to cassette\n");
    acase 0x1C42: /* CASTM */              zprintf(TEXTPEN_LOG, ";Function: Cassette to memory\n");
    acase 0x1C63: /* CATAL */              zprintf(TEXTPEN_LOG, ";Function: Catalog\n");
    acase 0x1CDA: /* LOFNR */              zprintf(TEXTPEN_LOG, ";Function: Compare filename with catalog and return first block index number\n");
    acase 0x1D16: /* GTPRM */              zprintf(TEXTPEN_LOG, ";Function: Get file parameters from input buffer\n");
    acase 0x1D6D: /* LOADR */              zprintf(TEXTPEN_LOG, ";Function: Load file from cassette\n");
    acase 0x1D8A: /* UNLOR */              zprintf(TEXTPEN_LOG, ";Function: Unlock file\n");
    acase 0x1DA2: /* LOCKR */              zprintf(TEXTPEN_LOG, ";Function: Lock file\n");
    acase 0x1DBA: /* RENAM */              zprintf(TEXTPEN_LOG, ";Function: Rename file\n");
    acase 0x1DFE: /* CHFLK */              zprintf(TEXTPEN_LOG, ";Function: Check if file is locked\n");
    acase 0x1E0B: /* DELET */              zprintf(TEXTPEN_LOG, ";Function: Delete file\n");
    acase 0x1E3A: /* CALNB */              zprintf(TEXTPEN_LOG, ";Function: Calculate number of blocks for file\n");
    acase 0x1E45: /* CLRPM */              zprintf(TEXTPEN_LOG, ";Function: Clear file parameters (4 bytes)\n");
    acase 0x1E4E: /* SAVRT */              zprintf(TEXTPEN_LOG, ";Function: Save file\n");
    acase 0x1F6F: /* CNRUB */              zprintf(TEXTPEN_LOG, ";Function: Calculate number of used blocks\n");
    acase 0x1FD4: /* PRPAR */              zprintf(TEXTPEN_LOG, ";Function: Prepare parameters\n");
    acase 0x1FED: /* SKPKM */              zprintf(TEXTPEN_LOG, ";Function: Skip until comma\n");
    acase 0x1FF5: /* SKPPK */              zprintf(TEXTPEN_LOG, ";Function: Skip until semicolon\n");
    }

    if (note)
    {   zprintf
        (   TEXTPEN_LOG,
            ";Function: Error entry\n" \
            ";Autodoc:\n" \
            ";  Use of registers:\n" \
            ";    R0: bit 7:        if 0, also print word 0, ERROR\n" \
            ";                      if 1, do not print ERROR, just message\n" \
            ";        bits 6..4:    number of words\n" \
            ";        bits 0..3:    number of first word\n"
        );
}   }

/*****************************
* PHUNSY hardware simulation *
******************************

MDCR info
total tape length is 1+128*0.7+1 = 93 s is 31,000,000 cpu cycles
the cassette length is 128 blocks of 700ms = 233,333 cpu cycles / block.
first block starts at 1s = 333,333 cpu cycles
block header to block is 75ms = 25,000 cpu cycles
1 bit time is 166.666 µs = 55.555 cpu cycles
----
block number starts at  n * 233,333           cycles
data block   starts at (n * 233,333) + 25,000 cycles

If both fwd and rev are asserted, the tape goes forward.
*/

EXPORT void mdcr_exec(void)
{   FAST int    i;

    switch (mdcrstate)
    {
    case MDCRSTATE_FWD:
        if (!(mdcr_bits & MDCR_FWD))
        {   mdcrstate = MDCRSTATE_IDLE;
            if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR tape stopped. MDCR state: Forward -> idle\n");
            break;
        }
        mdcrpos += justdone;
        if (mdcrpos >= mdcrlength)
        {   mdcrpos = mdcrlength;
            mdcrstate = MDCRSTATE_ATEND;
            if (verbosetape) zprintf(TEXTPEN_TAPE, "End of MDCR tape reached. MDCR state: Forward -> At end\n");
            break;
        }
        switch (mdcrfwdstate)
        {
        case MDCRFWDSTATE_INIT:
            if (!(mdcr_bits & MDCR_WCD))
            {   mdcrfwdstate = MDCRFWDSTATE_IDLE;
                if (verbosetape) zprintf(TEXTPEN_TAPE, "End of write. MDCR forward state: Init -> idle\n");
            }
        acase MDCRFWDSTATE_WRITE:
            if (!(mdcr_bits & MDCR_WCD))
            {   mdcrfwdstate = MDCRFWDSTATE_IDLE;
                if (verbosetape) zprintf(TEXTPEN_TAPE, "End of write. MDCR forward state: Write -> idle\n");
            } elif (mdcrpos < 100)
            {   mdcrfwdstate = MDCRFWDSTATE_INIT;
                if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR forward state: Write -> init. Clearing tape!\n");
                for (i = 0; i < 32768; i++)
                {   cassette[i] = 0;
            }   }
            elif (mdcr_bits & MDCR_WDA) // detected 1st bit, 1st bit is always inverse of 0
            {   mdcrfwdstate     = MDCRFWDSTATE_WRITE_ACTIVE;
                mdcrwritebittime = 0;
                mdcrbitcounter   = 1;
                mdcrwritebyte    = 0;
                if (verbosetape) zprintf(TEXTPEN_TAPE, "Detected first bit. MDCR forward state: Write -> write active\n");
            }
        acase MDCRFWDSTATE_WRITE_ACTIVE :
            if (!(mdcr_bits & MDCR_WCD)) // end of write
            {   mdcrfwdstate = MDCRFWDSTATE_IDLE;
                if (verbosetape) zprintf(TEXTPEN_TAPE, "End of write. MDCR forward state: Write active -> idle\n");
            }
            mdcrwritebittime += justdone;
            if (mdcrwritebittime < (MDCR_BIT * 3 / 4))
            {   mdcrportdoutold = mdcr_bits;
                break;
            } else
            {   if ((mdcrportdoutold ^ mdcr_bits) & MDCR_WDA) // detected new bit
                {   mdcrwritebyte >>= 1;
                    if (!(mdcr_bits & MDCR_WDA)) // bits are inverted
                    {   mdcrwritebyte |= 0x80;
                    }
                    mdcrwritebittime = 0;
                    mdcrbitcounter++;
                    if ((mdcrbitcounter % 8) == 0) // byte sampled?
                    {   if ((mdcrbitcounter / 8) < 2)  // $AA for synchronisation
                        {   break;
                        } elif ((mdcrbitcounter / 8) < 3)  // checksum
                        {   mdcrchecksum = -mdcrwritebyte;
                            break;
                        } elif ((mdcrbitcounter / 8) < 259)  // data
                        {   cassette[(mdcrblock * 256) + (mdcrbitcounter / 8) - 3] = mdcrwritebyte;
                            if (verbosetape) zprintf(TEXTPEN_TAPE, "Writing.\n");
                            mdcrchecksum += mdcrwritebyte;
                            break;
                        } else
                        {   // options |= CASSETTE_CHANGED;
                            break;  // post $AA byte
            }   }   }   }
        acase MDCRFWDSTATE_IDLE:
            if ((mdcr_bits & MDCR_WCD) != 0)
            {   mdcrfwdstate = MDCRFWDSTATE_WRITE;
                if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR tape started. MDCR forward state: Idle -> write\n");
            }
            if (mdcrpos >= MDCR_LEADER)
            {   if (((mdcrpos - MDCR_LEADER) % MDCR_BLOCK) <= 5) // max instruction cycle time
                {   mdcrblock      = (mdcrpos - MDCR_LEADER) / MDCR_BLOCK;
                    mdcrfwdstate   = MDCRFWDSTATE_NUMREAD_A;
                    mdcrdatapos    = 0;
                    mdcrbitcounter = 0;
                    if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR forward state: Idle -> number read A\n");
                    break;
                }
                if (mdcrpos >= MDCR_LEADER + MDCR_DATA)
                {   if (((mdcrpos - MDCR_LEADER - MDCR_DATA) % MDCR_BLOCK) <= 5)
                    {   mdcrblock      = (mdcrpos - MDCR_LEADER) / MDCR_BLOCK;
                        mdcrfwdstate   = MDCRFWDSTATE_BLOCKREAD_A;
                        mdcrdatapos    = 0;
                        mdcrbitcounter = 0;
                        mdcrchecksum   = 0;
                        for (i = 0; i < 256; i++)
                        {   mdcrchecksum += cassette[(256 * mdcrblock) + i];
                            // if (verbosetape) zprintf(TEXTPEN_TAPE, "Reading checksum.\n");
                        }
                        if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR forward state: Idle -> block read A\n");
                        break;
            }   }   }
        acase MDCRFWDSTATE_NUMREAD_A:
            if (mdcrdatapos >= (MDCR_BIT / 2))
            {   ioport[PORTC].contents |= MDCR_RDC;
                if (mdcrbitcounter >= 24)
                {   mdcrfwdstate = MDCRFWDSTATE_IDLE;
                    if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR forward state: Number read A -> idle\n");
                    break;
                }
                ioport[PORTC].contents |= MDCR_RDA; // data is inverted
                if ((mdcrbitcounter < 8) || (mdcrbitcounter > 15))
                {   if ((0xAA & (1 << (mdcrbitcounter % 8))) != 0)
                        ioport[PORTC].contents &= ~MDCR_RDA;
                } else
                {   if ((mdcrblock & (1 << (mdcrbitcounter % 8))) != 0)
                        ioport[PORTC].contents &= ~MDCR_RDA;
                }
                mdcrfwdstate = MDCRFWDSTATE_NUMREAD_B;
                mdcrdatapos -= MDCR_BIT / 2;
                // if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR forward state: Number read A -> number read B\n");
            }
            mdcrdatapos += justdone;
        acase MDCRFWDSTATE_NUMREAD_B:
            if (mdcrdatapos >= (MDCR_BIT / 2))
            {   ioport[PORTC].contents &= ~MDCR_RDC; // data is read by PHUNSY on falling edge of RDC
                mdcrfwdstate =  MDCRFWDSTATE_NUMREAD_A;
                mdcrdatapos  -= MDCR_BIT / 2;
                mdcrbitcounter++;
                // if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR forward state: Number read B -> number read A\n");
            }
            mdcrdatapos += justdone;
        acase MDCRFWDSTATE_BLOCKREAD_A:
            if (mdcrdatapos >= (MDCR_BIT / 2))
            {   ioport[PORTC].contents |= MDCR_RDC;
                if (mdcrbitcounter >= 2072)
                {   mdcrfwdstate = MDCRFWDSTATE_IDLE;
                    if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR forward state: Block read A -> idle\n");
                    break;
                }
                ioport[PORTC].contents |= MDCR_RDA; // data is inverted
                switch (mdcrbitcounter / 8)
                {
                case   0:
                case 258:
                    if ((0xAA & (1 << (mdcrbitcounter % 8))) != 0)
                        ioport[PORTC].contents &= ~MDCR_RDA;
                acase 1:
                    if ((mdcrchecksum & (1 << (mdcrbitcounter % 8))) != 0)
                        ioport[PORTC].contents &= ~MDCR_RDA;
                adefault:
                    if ((cassette[(256 * mdcrblock) + (mdcrbitcounter / 8) - 2] & (1 << (mdcrbitcounter % 8))) != 0)
                        ioport[PORTC].contents &= ~MDCR_RDA;
                    // if (verbosetape) zprintf(TEXTPEN_TAPE, "Reading.\n");
                }
                mdcrfwdstate = MDCRFWDSTATE_BLOCKREAD_B;
                mdcrdatapos -= MDCR_BIT / 2;
                // if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR forward state: Block read A -> block read B\n");
            }
            mdcrdatapos += justdone;
        acase MDCRFWDSTATE_BLOCKREAD_B:
            if (mdcrdatapos >= (MDCR_BIT / 2))
            {   ioport[PORTC].contents &= ~MDCR_RDC; // data is read by PHUNSY on falling edge of RDC
                mdcrfwdstate = MDCRFWDSTATE_BLOCKREAD_A;
                mdcrdatapos -= MDCR_BIT / 2;
                mdcrbitcounter++;
                // if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR forward state: Block read B -> block read A\n");
            }
            mdcrdatapos += justdone;
        }
    acase MDCRSTATE_REV:
        if (!(mdcr_bits & MDCR_REV)) // tape stop
        {   mdcrstate = MDCRSTATE_IDLE;
            if (verbosetape) zprintf(TEXTPEN_TAPE, "Tape stopped. MDCR state: Reverse -> Idle\n");
            break;
        }
        mdcrpos -= justdone;
        if (mdcrpos < MDCR_LEADER)
        {   mdcrblock = 0;
        } else
        {   mdcrblock = (mdcrpos - MDCR_LEADER) / MDCR_BLOCK; // actually not required for PHUNSY MDCR sofware
        }
        // toggle data clock in reverse so the MDCR software can find gaps
        if (mdcrpos <= MDCR_LEADER)
        {   if (verbosetape && mdcrrevstate != MDCRREVSTATE_LGAP) zprintf(TEXTPEN_TAPE, "MDCR reverse state: L gap\n");
            mdcrrevstate = MDCRREVSTATE_LGAP;
            ioport[PORTC].contents |= MDCR_RDC;
        } else
        {   mdcrblockpos = (mdcrpos - MDCR_LEADER) % MDCR_BLOCK;
            switch (mdcrrevstate)
            {
            case MDCRREVSTATE_DATA:
                if ((mdcrblockpos % 56) < 28)
                    ioport[PORTC].contents |= MDCR_RDC;
                else
                    ioport[PORTC].contents &= ~MDCR_RDC;
                if (mdcrblockpos < MDCR_DATA)
                {   ioport[PORTC].contents |= MDCR_RDC;
                    mdcrrevstate = MDCRREVSTATE_SGAP;
                    if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR reverse state: Data -> S gap\n");
                }
            acase MDCRREVSTATE_SGAP:
                if (mdcrblockpos < MDCR_BLOCKNUMLEN)
                {   mdcrrevstate = MDCRREVSTATE_BNUM;
                    if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR reverse state: S gap -> B number\n");
                }
            acase MDCRREVSTATE_BNUM:
                if ((mdcrblockpos % 56) < 28)
                    ioport[PORTC].contents |= MDCR_RDC;
                else
                    ioport[PORTC].contents &= ~MDCR_RDC;
                if (mdcrblockpos > (MDCR_DATA + MDCR_BLOCKDATLEN))
                {   ioport[PORTC].contents |= MDCR_RDC;
                    mdcrrevstate = MDCRREVSTATE_LGAP;
                    if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR reverse state: B number -> L gap\n");
                }
            adefault: // including MDCRREVSTATE_LGAP
                if (mdcrblockpos < (MDCR_DATA + MDCR_BLOCKDATLEN))
                {   mdcrrevstate = MDCRREVSTATE_DATA;
                    if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR reverse state: L gap -> data\n");
        }   }   }
        if (mdcrpos <= 0)
        {   mdcrpos = 0;
            mdcrstate = MDCRSTATE_ATSTART;
            if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR state: Reverse -> at start\n");
        }
    acase MDCRSTATE_ATSTART:
        if (mdcr_bits & MDCR_REV)
        {   ioport[PORTC].contents |= MDCR_BET;
        } else
        {   ioport[PORTC].contents &= ~MDCR_BET;
            mdcrstate = MDCRSTATE_IDLE;
            if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR state: At start -> idle\n");
        }
    acase MDCRSTATE_ATEND:
        if (mdcr_bits & MDCR_FWD)
        {   ioport[PORTC].contents |= MDCR_BET;
        } else
        {   ioport[PORTC].contents &= ~MDCR_BET;
            mdcrstate = MDCRSTATE_IDLE;
            if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR state: At end -> idle\n");
        }
    adefault: // including MDCRSTATE_IDLE
        mdcrfwdstate = MDCRFWDSTATE_IDLE;
        if (mdcr_bits & MDCR_FWD)
        {   mdcrstate = MDCRSTATE_FWD;
            if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR state: Idle -> forward. Forward state: Idle\n");
        } elif (mdcr_bits & MDCR_REV)
        {   mdcrstate = MDCRSTATE_REV;
            if (verbosetape) zprintf(TEXTPEN_TAPE, "MDCR state: Idle -> reverse\n"); // there is no MDCRREVSTATE_IDLE
}   }   }

EXPORT void create_mdcr(void)
{   int   i;
    FILE* MDCRHandle /* = NULL */ ;

    if (game)
    {   strcpy((char*) fn_tape[1], (const char*) file_game);
    } else
    {   fn_tape[1][0] = EOS;
    }
    fixextension(filekind[KIND_MDCR].extension, (STRPTR) fn_tape[1], TRUE, ""); // so we have extension in ASL requester

    if (asl
    (   filekind[KIND_MDCR].hail,
        filekind[KIND_MDCR].pattern,
        TRUE,
        (STRPTR) path_tapes,
        (STRPTR) fn_tape[1], // this fortunately doesn't contain the path right now
        (STRPTR) fn_tape[1]
    ) != EXIT_SUCCESS)
    {   return;
    }
    fixextension(filekind[KIND_MDCR].extension, (STRPTR) fn_tape[1], TRUE, ""); // so we have extension even if user overtypes it in ASL requester
#ifdef SETPATHS
    break_pathname((STRPTR) fn_tape[1], (STRPTR) path_tapes, NULL);
#endif

    if (!(MDCRHandle = fopen((const char*) fn_tape[1], "wb+")))
    {   say("Can't create file!");
        return;
    }
    for (i = 0; i < 32 * KILOBYTE; i++)
    {   cassette[i] = 0;
    }
    DISCARD fwrite(cassette, 32 * KILOBYTE, 1, MDCRHandle); // should check return code
    fclose(MDCRHandle);
    // MDCRHandle = NULL;

    mdcrpos        =
    mdcrblock      = 0;
    mdcrstate      = MDCRSTATE_IDLE;
    update_tapedeck(FALSE);

    insert_mdcr(FALSE);
}

EXPORT FLAG insert_mdcr(FLAG wantasl)
{   int   thesize;
    FILE* MDCRHandle = NULL;

    // asl() turns sound off and on for us
    if (wantasl)
    {   if (asl
        (   (STRPTR) LLL(
                MSG_HAIL_INSERTTAPE,
                "Insert MDCR Tape"
            ),
#ifdef WIN32
            "Mini Digital Cassette Recorder (*.MDCR)\0*.MDCR\0" \
            "All Files (*.*)\0*.*\0",
#endif
#ifdef AMIGA
            "#?.MDCR",
#endif
            FALSE,
            (STRPTR) path_tapes,  // starting directory
#ifdef WIN32
            fn_tape[1],           // starting filename
#endif
#ifdef AMIGA
    #ifdef __amigaos4__
            (STRPTR) FilePart((CONST_STRPTR) fn_tape[1]), // starting filename
    #else
            FilePart((const char*) fn_tape[1]), // starting filename
    #endif
#endif
            (STRPTR) fn_tape[1]   // where to store complete result pathname
        ) != EXIT_SUCCESS)
        {   return FALSE;
        }
#ifdef SETPATHS
        break_pathname((STRPTR) fn_tape[1], (STRPTR) path_tapes, NULL);
#endif
    }

    if
    (   (!(thesize = getsize((STRPTR) fn_tape[1])))
     || (!(MDCRHandle = fopen((const char*) fn_tape[1], "rb")))
    )
    {   say("Can't open file for input!");
        goto ABORT;
    }

    if (thesize % 256 || thesize > 32 * KILOBYTE)
    {   say("File is wrong size!");
        goto ABORT;
    }

    if (fread(cassette, (size_t) thesize, 1, MDCRHandle) != 1)
    {   say("Can't read 32K from file!");
        goto ABORT;
    }

    mdcrblocks     = thesize / 256;
    mdcrlength     = (MDCR_LEADER * 2) + (mdcrblocks * 256);
    mdcrpos        =
    mdcrblock      = 0;
    mdcrstate      = MDCRSTATE_IDLE;
    update_tapedeck(FALSE);

    fclose(MDCRHandle);
    // MDCRHandle = NULL;
    return TRUE;

ABORT:
    if (MDCRHandle)
    {   fclose(MDCRHandle);
        // MDCRHandle = NULL;
    }
    return FALSE;
}

EXPORT FLAG update_mdcr(void)
{   FILE* MDCRHandle /* = NULL */ ;

    if (!(MDCRHandle = fopen((const char*) fn_tape[1], "wb")))
    {   say("Can't open file for output!");
        return FALSE;
    } // implied else

    DISCARD fwrite(cassette, mdcrblocks * 256, 1, MDCRHandle); // should check return code
    DISCARD fclose(MDCRHandle);
    // MDCRHandle = NULL;
    return TRUE;
}

EXPORT void mdcr_show(STRPTR thestring, int theval)
{
#ifdef MDCR_SHOWTIME
    sprintf(thestring, "%d.%06d", (theval * 3) / ONE_MILLION, (theval * 3) % ONE_MILLION);
#else
    if (mdcrpos >= ONE_MILLION)
    {   sprintf
        (   thestring,
            "%d,%03d,%03d",
             theval / ONE_MILLION,
            (theval % ONE_MILLION) / 1000,
             theval                % 1000
        );
    } elif (mdcrpos >= 1000)
    {   sprintf
        (   thestring,
            "%d,%03d",
            theval / 1000,
            theval % 1000
        );
    } else
    {   sprintf(thestring, "%d", theval);
    }
#endif
}

EXPORT void phunsy_drawhelpgrid(void)
{   int x,  y,
        xx, yy,
        startx,
        starty;

    for (x = 0; x < 64; x++)
    {   for (y = 0; y < 32; y++)
        {   startx = x * 6;
            starty = y * 8;
            for (xx = 0; xx < 6; xx++)
            {   for (yy = 0; yy < 8; yy++)
                {   if (xx == 0 || xx == 6 - 1 || yy == 0 || yy == 8 - 1)
                    {   changepixel(128 - absxmin + startx + xx, starty + yy, GREY1);
}   }   }   }   }   }

MODULE void phunsy_emuinput(void)
{   FLAG  ready;
    UBYTE t;

    if
    (   keys_column[0]
     || keys_column[1]
     || keys_column[2]
     || keys_column[3]
     || keys_column[4]
     || keys_column[5]
    )
    {   ready = FALSE;
    } else
    {   ready = TRUE;
    }

    if (recmode == RECMODE_PLAY)
    {   t = IOBuffer[offset++];
        keys_column[0] = (UBYTE) ((t & 0xF0) >> 4);
        keys_column[1] = (UBYTE)  (t & 0x0F)      ;
        t = IOBuffer[offset++];
        keys_column[2] = (UBYTE) ((t & 0xF0) >> 4);
        keys_column[3] = (UBYTE)  (t & 0x0F)      ;
        t = IOBuffer[offset++];
        keys_column[4] = (UBYTE) ((t & 0xF0) >> 4);
        keys_column[5] = (UBYTE)  (t & 0x0F)      ;
    } else
    {   // assert(recmode == RECMODE_NORMAL || recmode == RECMODE_RECORD);

        t = 0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[0][ 1])); // 0
        t |= (UBYTE) (0x2 * KeyDown(keypads[0][ 4])); // 1
        t |= (UBYTE) (0x4 * KeyDown(keypads[0][ 7])); // 2
        t |= (UBYTE) (0x8 * KeyDown(keypads[0][10])); // 3
        keys_column[0] = t;

        t = 0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[0][ 2])); // 4
        t |= (UBYTE) (0x2 * KeyDown(keypads[0][ 5])); // 5
        t |= (UBYTE) (0x4 * KeyDown(keypads[0][ 8])); // 6
        t |= (UBYTE) (0x8 * KeyDown(keypads[0][11])); // 7
        keys_column[1] = t;

        t = 0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[0][ 3])); // 8
        t |= (UBYTE) (0x2 * KeyDown(keypads[0][ 6])); // 9
        t |= (UBYTE) (0x4 * KeyDown(keypads[0][ 9])); // A
        t |= (UBYTE) (0x8 * KeyDown(keypads[0][12])); // B
        keys_column[2] = t;

        t = 0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[1][ 1])); // C
        t |= (UBYTE) (0x2 * KeyDown(keypads[1][ 4])); // D
        t |= (UBYTE) (0x4 * KeyDown(keypads[1][ 7])); // E
        t |= (UBYTE) (0x8 * KeyDown(keypads[1][10])); // F
        keys_column[3] = t;

        t = 0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[1][ 2])); // D->M
        t |= (UBYTE) (0x2 * KeyDown(keypads[1][ 5])); // M->D
        t |= (UBYTE) (0x4 * KeyDown(keypads[1][ 8])); // G
        t |= (UBYTE) (0x8 * KeyDown(keypads[1][11])); // Cl
        keys_column[4] = t;

        t = 0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[1][ 3])); // Reset
        t |= (UBYTE) (0x2 * KeyDown(keypads[1][ 6])); // Halt
        t |= (UBYTE) (0x4 * KeyDown(keypads[1][ 9])); // DumpCass
        t |= (UBYTE) (0x8 * KeyDown(keypads[1][12])); // LoadCass
        keys_column[5] = t;

        domouse();
    }

    if (recmode == RECMODE_RECORD)
    {   // assert(MacroHandle);

        OutputBuffer[0] = ((keys_column[0] & 0x0F) << 4)
                        |  (keys_column[1] & 0x0F);
        OutputBuffer[1] = ((keys_column[2] & 0x0F) << 4)
                        |  (keys_column[3] & 0x0F);
        OutputBuffer[2] = ((keys_column[4] & 0x0F) << 4)
                        |  (keys_column[5] & 0x0F);

        DISCARD fwrite(OutputBuffer, 3, 1, MacroHandle);
        // should really check return code of fwrite()
        // 3 bytes per frame * 50 frames per second = 150 bytes/sec.
    }

    if (!ready || phunsy_biosver != PHUNSY_MINIMONITOR)
    {   return;
    }

    if
    (   keys_column[0]
     || keys_column[1]
     || keys_column[2]
     || keys_column[3]
     || keys_column[4]
     || keys_column[5]
    )
    {   interrupt_2650 = TRUE;
        checkinterrupt();
}   }

MODULE void phunsy_drawdigit(int position)
{   TRANSIENT       UBYTE colour,
                          t = 0;
    TRANSIENT       int   controlsx,
                          x, xx, xxx,
                          y, yy;
    PERSIST   const UBYTE til311[16][7] = {
{ 0x6, // .##.
  0x9, // #..#
  0x9, // #..#
  0x9, // #..#
  0x9, // #..#
  0x9, // #..#
  0x6, // .##.
},
{ 0x1, // ...#
  0x1, // ...#
  0x1, // ...#
  0x1, // ...#
  0x1, // ...#
  0x1, // ...#
  0x1, // ...#
},
{ 0xE, // ###.
  0x1, // ,,,#
  0x1, // ,,,#
  0x6, // ,##.
  0x8, // #...
  0x8, // #...
  0xF, // ####
},
{ 0xE, // ###.
  0x1, // ,,,#
  0x1, // ,,,#
  0x6, // ,##.
  0x1, // ...#
  0x1, // ...#
  0xE, // ###.
},
{ 0x8, // #...
  0x9, // #..#
  0x9, // #..#
  0xF, // ####
  0x1, // ...#
  0x1, // ...#
  0x1, // ...#
},
{ 0x1, // ####
  0x1, // #...
  0x1, // #...
  0x1, // ###.
  0x1, // ...#
  0x1, // ...#
  0x1, // ###.
},
{ 0x6, // .##.
  0x8, // #...
  0x8, // #...
  0xE, // ###.
  0x9, // #..#
  0x9, // #..#
  0x6, // .##.
},
{ 0xF, // ####
  0x1, // ...#
  0x1, // ...#
  0x1, // ...#
  0x1, // ...#
  0x1, // ...#
  0x1, // ...#
},
{ 0x6, // .##.
  0x9, // #..#
  0x9, // #..#
  0x6, // .##.
  0x9, // #..#
  0x9, // #..#
  0x6, // .##.
},
{ 0x6, // .##.
  0x9, // #..#
  0x9, // #..#
  0x7, // .###
  0x1, // ...#
  0x1, // ...#
  0x6, // .##.
},
{ 0x6, // .##.
  0x9, // #..#
  0x9, // #..#
  0xF, // ####
  0x9, // #..#
  0x9, // #..#
  0x9, // #..#
},
{ 0xE, // ###.
  0x9, // #..#
  0x9, // #..#
  0xE, // ###.
  0x9, // #..#
  0x9, // #..#
  0xE, // ###.
},
{ 0x7, // .###
  0x8, // #...
  0x8, // #...
  0x8, // #...
  0x8, // #...
  0x8, // #...
  0x7, // .###
},
{ 0xE, // ###.
  0x9, // #..#
  0x9, // #..#
  0x9, // #..#
  0x9, // #..#
  0x9, // #..#
  0xE, // ###.
},
{ 0xF, // ####
  0x8, // #...
  0x8, // #...
  0xE, // ###.
  0x8, // #...
  0x8, // #...
  0xF, // ####
},
{ 0xF, // ####
  0x8, // #...
  0x8, // #...
  0xE, // ###.
  0x8, // #...
  0x8, // #...
  0x8, // #...
},
};

    // assert(machine == PHUNSY);
    x         = ((machines[PHUNSY].width  - (43 * 4)) / 2) + (28 * position);
    y         =   machines[PHUNSY].height -  34;
    controlsx = (9 * position) + 1;

    switch (position)
    {
    case  0: t = (ioport[0x10].contents & 0xF0) >> 4;
    acase 1: t =  ioport[0x10].contents & 0x0F;
    acase 2: t = (ioport[0x11].contents & 0xF0) >> 4;
    acase 3: t =  ioport[0x11].contents & 0x0F;
    acase 4: t = (ioport[0x12].contents & 0xF0) >> 4;
    acase 5: t =  ioport[0x12].contents & 0x0F;
    }

    for (yy = 0; yy < 7; yy++)
    {   for (xx = 0; xx < 4; xx++)
        {   if
            (   (yy != 0 && yy != 3 && yy != 6)
             && (xx != 0 && xx != 3)
            )
            {   continue;
            } // implied else

            if (til311[t][yy] & (8 >> xx))
            {   colour = TILCOLOUR;
            } else
            {   colour = drawunlit ? DARKBLUE : BLACK;
            }

#ifdef TILTILT
            if (yy <= 1) xxx = 1; elif (yy >= 5) xxx = -1; else xxx = 0;
#else
            xxx = 0;
#endif

            if (drawcorners)
            {   changepixel(x + (xx * 4)     + xxx, y + (yy * 4)    , colour);
                changepixel(x + (xx * 4) + 1 + xxx, y + (yy * 4)    , colour);
                changepixel(x + (xx * 4) + 2 + xxx, y + (yy * 4)    , colour);
                changepixel(x + (xx * 4) + 3 + xxx, y + (yy * 4)    , colour);
                changepixel(x + (xx * 4)     + xxx, y + (yy * 4) + 1, colour);
                changepixel(x + (xx * 4) + 1 + xxx, y + (yy * 4) + 1, colour);
                changepixel(x + (xx * 4) + 2 + xxx, y + (yy * 4) + 1, colour);
                changepixel(x + (xx * 4) + 3 + xxx, y + (yy * 4) + 1, colour);
                changepixel(x + (xx * 4)     + xxx, y + (yy * 4) + 2, colour);
                changepixel(x + (xx * 4) + 1 + xxx, y + (yy * 4) + 2, colour);
                changepixel(x + (xx * 4) + 2 + xxx, y + (yy * 4) + 2, colour);
                changepixel(x + (xx * 4) + 3 + xxx, y + (yy * 4) + 2, colour);
                changepixel(x + (xx * 4)     + xxx, y + (yy * 4) + 3, colour);
                changepixel(x + (xx * 4) + 1 + xxx, y + (yy * 4) + 3, colour);
                changepixel(x + (xx * 4) + 2 + xxx, y + (yy * 4) + 3, colour);
                changepixel(x + (xx * 4) + 3 + xxx, y + (yy * 4) + 3, colour);
            } else
            {   changepixel(x + (xx * 4)     + xxx, y + (yy * 4)    , BLACK);
                changepixel(x + (xx * 4) + 1 + xxx, y + (yy * 4)    , BLACK);
                changepixel(x + (xx * 4) + 2 + xxx, y + (yy * 4)    , BLACK);
                changepixel(x + (xx * 4) + 3 + xxx, y + (yy * 4)    , BLACK);
                changepixel(x + (xx * 4)     + xxx, y + (yy * 4) + 1, BLACK);
                changepixel(x + (xx * 4) + 3 + xxx, y + (yy * 4) + 1, BLACK);
                changepixel(x + (xx * 4)     + xxx, y + (yy * 4) + 2, BLACK);
                changepixel(x + (xx * 4) + 3 + xxx, y + (yy * 4) + 2, BLACK);
                changepixel(x + (xx * 4)     + xxx, y + (yy * 4) + 3, BLACK);
                changepixel(x + (xx * 4) + 1 + xxx, y + (yy * 4) + 3, BLACK);
                changepixel(x + (xx * 4) + 2 + xxx, y + (yy * 4) + 3, BLACK);
                changepixel(x + (xx * 4) + 3 + xxx, y + (yy * 4) + 3, BLACK);

                changepixel(x + (xx * 4) + 1 + xxx, y + (yy * 4) + 1, colour);
                changepixel(x + (xx * 4) + 2 + xxx, y + (yy * 4) + 1, colour);
                changepixel(x + (xx * 4) + 1 + xxx, y + (yy * 4) + 2, colour);
                changepixel(x + (xx * 4) + 2 + xxx, y + (yy * 4) + 2, colour);
            }

            drawcontrolspixel(controlsx + xx, yy + 1, colour);
}   }   }

MODULE void phunsy_drawdot(int position)
{   FAST UBYTE t;
    FAST int   colour,
               controlsx,
               x, y;

    // assert(machine == PHUNSY);

    t         = ioport[0x13].contents & 7;
    controlsx = (9 * position) - 2;
    x         = ((machines[PHUNSY].width  - (43 * 4)) / 2) + (28 * position) - 8;
#ifdef TILTILT
    x--;
#endif
    y         =   machines[PHUNSY].height -  10;

    if
    (   (position == 0 && memory[oldiar] == 0x40)
     || (position != 0 && t == position)
    )
    {   colour = TILCOLOUR;
    } else
    {   colour = drawunlit ? DARKBLUE : BLACK;
    }

    if (drawcorners)
    {   if (showleds)
        {   changepixel(x    , y    , colour);
            changepixel(x + 1, y    , colour);
            changepixel(x + 2, y    , colour);
            changepixel(x + 3, y    , colour);
            changepixel(x    , y + 1, colour);
            changepixel(x + 1, y + 1, colour);
            changepixel(x + 2, y + 1, colour);
            changepixel(x + 3, y + 1, colour);
            changepixel(x    , y + 2, colour);
            changepixel(x + 1, y + 2, colour);
            changepixel(x + 2, y + 2, colour);
            changepixel(x + 3, y + 2, colour);
            changepixel(x    , y + 3, colour);
            changepixel(x + 1, y + 3, colour);
            changepixel(x + 2, y + 3, colour);
            changepixel(x + 3, y + 3, colour);
        }
        drawcontrolspixel(controlsx, 7, colour);
    } else
    {   if (showleds)
        {   changepixel(x    , y    , BLACK);
            changepixel(x + 1, y    , BLACK);
            changepixel(x + 2, y    , BLACK);
            changepixel(x + 3, y    , BLACK);
            changepixel(x    , y + 1, BLACK);
            changepixel(x + 3, y + 1, BLACK);
            changepixel(x    , y + 2, BLACK);
            changepixel(x + 3, y + 2, BLACK);
            changepixel(x    , y + 3, BLACK);
            changepixel(x + 1, y + 3, BLACK);
            changepixel(x + 2, y + 3, BLACK);
            changepixel(x + 3, y + 3, BLACK);
            changepixel(x + 1, y + 1, colour);
            changepixel(x + 2, y + 1, colour);
            changepixel(x + 1, y + 2, colour);
            changepixel(x + 2, y + 2, colour);
        }
        drawcontrolspixel(controlsx, 7, colour);
}   }

#define KXLIMIT 63
#define KYLIMIT 31
EXPORT FLAG phunsy_edit_screen(UWORD code)
{   int i,
        kx, ky;

    kx =  scrnaddr           % 64;
    ky = (scrnaddr - 0x1000) / 64;

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
                                 scrnaddr = 0x1000 + kx + (ky * 64);
                                 memory[scrnaddr] = 0x20;
    acase SCAN_DEL:              if (shift())
                                 {   for (i = 0x1000; i <= 0x17FF; i++)
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
    scrnaddr = 0x1000 + kx + (ky * 64);

    return TRUE; // return code is whether we ate the keystroke
}

EXPORT void phunsy_reset(void)
{   int i;

    // keyboard
    queuepos = 0;

    // game
    if (phunsy_biosver == PHUNSY_MINIMONITOR)
    {   for (i = 0x1DF0; i <= 0x1DFD; i++)
        {   memory[i] = 0;
    }   }
    else
    {   // assert(phunsy_biosver == PHUNSY_PHUNSY);
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
        // afterwards.
        ras[0]        = 0x23E;
        psu           = 1; // ie. SP = 1;

        if (game)
        {   iar = (startaddr_h * 256) + startaddr_l;
        } else
        {   iar = 0;
    }   }

    // LED digits
    for (i = 0; i < DIGITLEDS; i++)
    {   digitleds[i] = 0x3F; // PHUNSY LED hardware doesn't allow blank digits
}   }

MODULE void draw_phunsy(void)
{   FAST int   x, xx, y, yy;
    FAST UBYTE fgc,
               imagery;

    // assert(machine == PHUNSY);

    // generate screen[][] and display[] based on memory[]

    for (x = 0; x < 64; x++)
    {   for (y = 0; y < 32; y++)
        {   imagery = memory[0x1000 + (y * 64) + x];
            if (imagery >= 128)
            {   if ((imagery & 0x70) == 0)
                {   fgc = vdu_bgc;
                } elif ((imagery & 0x70) == 0x70)
                {   fgc = vdu_fgc;
                } else
                {   if (inverse)
                    {   fgc = 22 - ((imagery & 0x70) >> 4); // 1..6 -> 21..16
                    } else
                    {   fgc = 15 + ((imagery & 0x70) >> 4); // 1..6 -> 16..21
                }   }
                imagery &= 0x0F;

                for (yy = 0; yy < 8; yy++)
                {   for (xx = 0; xx < 6; xx++)
                    {   if (phunsy_gfx[imagery][yy] & (0x80 >> xx))
                        {   changepixel(128 - absxmin + (x * 6) + xx, (y * 8) + yy, fgc);
                        } else
                        {   changepixel(128 - absxmin + (x * 6) + xx, (y * 8) + yy, vdu_bgc);
            }   }   }   }
            else
            {   if (imagery == 0)
                {   imagery = 0x20; // maybe we shouldn't be doing this?
                } elif (imagery <= 0x1F)
                {   imagery += 0x40;
                } elif (imagery >= 0x40 && imagery <= 0x5F)
                {   imagery -= 0x40;
                }
                for (yy = 0; yy < 8; yy++)
                {   for (xx = 0; xx < 6; xx++)
                    {   if (phunsy_chars[imagery][yy] & (0x20 >> xx))
                        {   changepixel(128 - absxmin + (x * 6) + xx, (y * 8) + yy, vdu_fgc);
                        } else
                        {   changepixel(128 - absxmin + (x * 6) + xx, (y * 8) + yy, vdu_bgc);
}   }   }   }   }   }   }

MODULE void phunsy_runcpu(void)
{   FAST ULONG endcycle;

    // assert(slice_2650 >= 1);

    endcycle = cycles_2650 + slice_2650;
    if (endcycle < cycles_2650)
    {   // cycle counter will overflow, so we need to use the slow method
        while (slice_2650 >= 1)
        {   oldcycles = cycles_2650;
            checkstep();
            mdcr_exec();
            do_tape();
            one_instruction();
            slice_2650 -= (cycles_2650 - oldcycles);
    }   }
    else
    {   // cycle counter will not overflow, so we can use a faster method
        oldcycles = cycles_2650;
        while (cycles_2650 < endcycle)
        {   checkstep();
            mdcr_exec();
            do_tape();
            one_instruction();
        }
        slice_2650 -= (cycles_2650 - oldcycles);
}   }

MODULE void run_cpu(int until)
{   // This is a quicker equivalent to repeatedly incrementing cpux and calling do_cpu().

    cpux = nextinst;
    while (cpux < until)
    {   oldcycles = cycles_2650;
        checkstep();
        mdcr_exec();
        do_tape();
        one_instruction();
        nextinst += (cycles_2650 - oldcycles) * 8; // in pixels
        cpux = nextinst;
    }
    if (nextinst >= 512)
    {   nextinst -= 512;
}   }
