// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
#endif
#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
    #include "resource.h"
#endif

#include <stdio.h>
#include <stdlib.h>            // rand()
#include <string.h>
#include <ctype.h>             // toupper()

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"
#ifdef AMIGA
    #include <proto/locale.h>  // GetCatalogStr()
#endif

#include "twin.h"

// DEFINES----------------------------------------------------------------

// #define TWINRS232
// whether to use preliminary RS232 support

#ifdef TWINRS232
    #define KYBDINTERRUPT 6 // 4 means TTY, 6 means RS-232
    #define SCRNINTERRUPT 7 // 5 means TTY, 7 means RS-232
#else
    #define KYBDINTERRUPT 4 // 4 means TTY, 6 means RS-232
    #define SCRNINTERRUPT 5 // 5 means TTY, 7 means RS-232
#endif

// #define INTERRUPTCOLOURS
// whether to change fgc to show why interrupts are inhibited

#define WHICHDOS TWIN_SDOS20
// default DOS

// #define EMULATETIMER
// whether to emulate the interval timer
// (causes problems with SDOS 4.2 and (if date is supplied by user) TOS)

#define GPIO 0
// we should allow user to choose any of $00..$B9,$BC..$BF,$C1..$D9 for this

// #define USEBYTES
// whether to give locations on disk as bytes instead of clusters

// #define EMULATEKEYBOARD
// whether to emulate TWIN keyboard interrupt inhibiting
// (causes problems with SDOS 4.2 after pressing DEL key)

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       FLAG            drive_rst,
                             priflag[32],
                             twin_escaped;
EXPORT       UBYTE           drive_control,
                             mastermem[16 * KILOBYTE],
                             other_ininterrupt,
                             other_psl,
                             other_psu,
                             other_r[7];
EXPORT       UWORD           other_iar,
                             other_ras[8];
EXPORT       int             other_slice,
                             prtunit        = 0,
                             prtscrollx,
                             prtscrolly,
                             twin_dosver    = WHICHDOS,
                             whichcpu;

EXPORT const UWORD fileoffset[78] = // offset of the "file block bitmap" for each file
{ 768+(41* 0)+(5* 0),
  768+(41* 1)+(5* 0),
  768+(41* 2)+(5* 0),
  768+(41* 3)+(5* 1),
  768+(41* 4)+(5* 1),
  768+(41* 5)+(5* 1),
  768+(41* 6)+(5* 2),
  768+(41* 7)+(5* 2),
  768+(41* 8)+(5* 2),
  768+(41* 9)+(5* 3),
  768+(41*10)+(5* 3),
  768+(41*11)+(5* 3),
  768+(41*12)+(5* 4),
  768+(41*13)+(5* 4),
  768+(41*14)+(5* 4),
  768+(41*15)+(5* 5),
  768+(41*16)+(5* 5),
  768+(41*17)+(5* 5),
  768+(41*18)+(5* 6),
  768+(41*19)+(5* 6),
  768+(41*20)+(5* 6),
  768+(41*21)+(5* 7),
  768+(41*22)+(5* 7),
  768+(41*23)+(5* 7),
  768+(41*24)+(5* 8),
  768+(41*25)+(5* 8),
  768+(41*26)+(5* 8),
  768+(41*27)+(5* 9),
  768+(41*28)+(5* 9),
  768+(41*29)+(5* 9),
  768+(41*30)+(5*10),
  768+(41*31)+(5*10),
  768+(41*32)+(5*10),
  768+(41*33)+(5*11),
  768+(41*34)+(5*11),
  768+(41*35)+(5*11),
  768+(41*36)+(5*12),
  768+(41*37)+(5*12),
  768+(41*38)+(5*12),
  768+(41*39)+(5*13),
  768+(41*40)+(5*13),
  768+(41*41)+(5*13),
  768+(41*42)+(5*14),
  768+(41*43)+(5*14),
  768+(41*44)+(5*14),
  768+(41*45)+(5*15),
  768+(41*46)+(5*15),
  768+(41*47)+(5*15),
  768+(41*48)+(5*16),
  768+(41*49)+(5*16),
  768+(41*50)+(5*16),
  768+(41*51)+(5*17),
  768+(41*52)+(5*17),
  768+(41*53)+(5*17),
  768+(41*54)+(5*18),
  768+(41*55)+(5*18),
  768+(41*56)+(5*18),
  768+(41*57)+(5*19),
  768+(41*58)+(5*19),
  768+(41*59)+(5*19),
  768+(41*60)+(5*20),
  768+(41*61)+(5*20),
  768+(41*62)+(5*20),
  768+(41*63)+(5*21),
  768+(41*64)+(5*21),
  768+(41*65)+(5*21),
  768+(41*66)+(5*22),
  768+(41*67)+(5*22),
  768+(41*68)+(5*22),
  768+(41*69)+(5*23),
  768+(41*70)+(5*23),
  768+(41*71)+(5*23),
  768+(41*72)+(5*24),
  768+(41*73)+(5*24),
  768+(41*74)+(5*24),
  768+(41*75)+(5*26),
  768+(41*76)+(5*26),
  768+(41*77)+(5*26),
};

EXPORT const STRPTR pristring[32] =
{ "reset",
  "master RAM parity error",
  "paper tape reader",
  "slave RAM parity error",
  "teletype in",
  "teletype out",
  "RS-232 in",
  "RS-232 out",
  "not assigned",
  "interval timer",
  "printer",
  "floppy disk",
  "not assigned",
  "PROM program #1",
  "PROM program #2",
  "not assigned",
  "slave SVC #1",
  "slave SVC #2",
  "slave SVC #3",
  "slave SVC #4",
  "slave SVC #5",
  "slave SVC #6",
  "debug SVC #1",
  "debug SVC #2",
  "breakpoint #1",
  "breakpoint #2",
  "single cycle",
  "halt",
  "diagnostic control",
  "not assigned",
  "not assigned",
  "not assigned",
};

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       FLAG            ignore_cout,
                             inframe,
                             paperreaderenabled,
                             paperpunchenabled;
IMPORT       TEXT            asciiname_short[259][3 + 1],
                             gtempstring[256 + 1],
                             path_disks[MAX_PATH + 1],
                             thequeue[QUEUESIZE];
IMPORT       UBYTE           blank,
                             cosversion,
                             guestkey,
                             ininterrupt,
                             memory[32768],
                             PapertapeBuffer[2][PAPERTAPEMAX],
                             papertapebyte[2],
                             psl,
                             psu,
                             r[7],
                             startaddr_h, startaddr_l,
                             vdu[MAX_VDUCOLUMNS][MAX_VDUROWS];
IMPORT const UBYTE           lcvdu_chars[128][8];
IMPORT       UWORD           iar,
                             mirror_r[32768],
                             mirror_w[32768],
                             oldiar,
                             ras[8];
IMPORT       ULONG           curdrive,
                             cycles_2650,
                             eachpage,
                             frames,
                             oldcycles,
                             papertapelength[2],
                             papertapewhere[2],
                             pencolours[COLOURSETS][PENS],
                             verbosedisk;
IMPORT       int             ambient,
                             belling[2],
                             blink,
                             colourset,
                             debugdrive,
                             diskbyte,
                             drawmode,
                             drive_mode,
                             frameskip,
                             game,
                             interrupt_2650,
                             inverse,
                             machine,
                             offset,
                             papertapemode[2],
                             post,
                             queuekeystrokes,
                             queuepos,
                             randomizememory,
                             serializemode,
                             slice_2650,
                             trace,
                             vdu_x, vdu_y,
                             watchreads,
                             watchwrites,
                             whichkeyrect;
IMPORT       UBYTE*          IOBuffer;
IMPORT       MEMFLAG         memflags[ALLTOKENS];
IMPORT       struct DriveStruct       drive[DRIVES_MAX];
IMPORT       struct HostMachineStruct hostmachines[MACHINES];
IMPORT       struct IOPortStruct      ioport[258];
IMPORT       struct MachineStruct     machines[MACHINES];
IMPORT       struct PrinterStruct     printer[2];
IMPORT       struct RTCStruct         rtc;
#ifdef WIN32
    IMPORT   int             CatalogPtr;
    IMPORT   HWND            SubWindowPtr[SUBWINDOWS];
    IMPORT   ULONG*          canvasdisplay[CANVASES];
#endif
#ifdef AMIGA
    IMPORT   struct Catalog* CatalogPtr;
    IMPORT   struct Window*  SubWindowPtr[SUBWINDOWS];
    IMPORT   UBYTE           bytepens[PENS];
    IMPORT   UBYTE          *canvasbyteptr[CANVASES][CANVASHEIGHT],
                            *canvasdisplay[CANVASES];
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE       UBYTE              crtstatus,
                                slavemem[32 * KILOBYTE];
MODULE       MEMFLAG            masterflags[16 * KILOBYTE],
                                slaveflags[32 * KILOBYTE];
MODULE       int                drive_stage,
                                freesize;
MODULE       struct DriveStruct tempdrive;

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void generate_interrupt(int level);
MODULE void twin_runcpu(void);
MODULE void c306_printline(void);
MODULE FLAG parse_mod(UBYTE* modptr, FLAG showing, FLAG doing);
MODULE void tell_cluster(int cluster);
MODULE void tell_clusters(int first, int second);

// CODE-------------------------------------------------------------------

EXPORT void twin_setmemmap(void)
{   int address,
        i, j;

    game = FALSE;
    whichcpu = 0; // master

    for (address     =     0; address <=   0xFF; address++)
    {   mastermem[address] = bootstrap[address];
    }
    if (!post && twin_dosver != TWIN_NODOS)
    {   switch (twin_dosver)
        {
        case  TWIN_EXOS:   exos_to_ram();
        acase TWIN_SDOS20:
        case  TWIN_SDOS40:
        case  TWIN_SDOS42: sdos_to_ram();
        acase TWIN_TOS:    tos_to_ram();
        acase TWIN_UDOS:   udos_to_ram();
    }   }
    elif (randomizememory)
    {   for (address = 0x100; address <= 0x3FFF; address++)
        {   mastermem[address] = rand() & 0xFF; // ie. rand() % 256
    }   }
    else
    {   for (address = 0x100; address <= 0x3FFF; address++)
        {   mastermem[address] = 0;
    }   }
    if (randomizememory)
    {   for (address =     0; address <= 0x7FFF; address++)
        {   slavemem[ address] = rand() & 0xFF; // ie. rand() % 256
    }   }
    else
    {   for (address =     0; address <= 0x7FFF; address++)
        {   slavemem[ address] = 0;
    }   }

    for (address =      0; address <=   0xFF; address++) masterflags[address] = NOWRITE | BIOS;
    for (address =  0x100; address <= 0x1FFF; address++) masterflags[address] = 0;
    for (address = 0x2000; address <= 0x201D; address++) masterflags[address] = BIOS;
    for (address = 0x201E; address <= 0x3FFF; address++) masterflags[address] = 0;
    for (address =      0; address <= 0x7FFF; address++)  slaveflags[address] = 0;

    for (address = 0; address < 32 * KILOBYTE; address++)
    {   mirror_r[address] = mirror_w[address] = (UWORD) address;
    }

    memcpy( memory                 , mastermem  , 16 * KILOBYTE);
    memcpy(&memory[  16 * KILOBYTE],  slavemem  , 16 * KILOBYTE); // we start on bank 0

    memcpy( memflags               , masterflags, 16 * KILOBYTE * sizeof(MEMFLAG));
    memcpy(&memflags[16 * KILOBYTE],  slaveflags, 16 * KILOBYTE * sizeof(MEMFLAG)); // we start on bank 0

    if (twin_dosver == TWIN_SDOS20)
    {   machines[TWIN].firstequiv = FIRSTTWINEQUIV;
        machines[TWIN].lastequiv  = LASTSDOS20EQUIV;
    } else
    {   machines[TWIN].firstequiv = FIRSTTWINEQUIV;
        machines[TWIN].lastequiv  = LASTTWINEQUIV;
    }

    blank = TWIN_BLANK;
    whichkeyrect = KEYRECT_TWIN;

    curdrive            = 0;
    if (debugdrive >= machines[machine].drives)
    {   debugdrive = machines[machine].drives - 1;
    }

    for (i = 0; i < machines[machine].drives; i++)
    {   for (j = 0; j < TWIN_DISKSIZE / 8; j++)
        {   drive[i].flags[j] = 0;
        }
        drive[i].track       =
        drive[i].sector      = 0;
        drive[i].inserted    = FALSE;
    }

    twin_create_disk(0); // because it won't even boot without DOS available on disk
}

EXPORT void twin_emulate(void)
{   FLAG  inverted;
    UBYTE fgc,
          bgc,
          t;
    int   x, y,
          xx, yy;

    inframe = TRUE;

#ifdef TWINRS232
    memory[0x1013] = 0x2B;
    memory[0x1014] = 0xD8;
    memory[0x1016] = 0x2D;
    memory[0x1017] = 0x86;
#endif

    slice_2650 += 20800; // 9.984 MHz master clock / 8 = 1.248 MHz CPU clock
    twin_runcpu();

    if (queuekeystrokes)
    {   if (guestkey == NC)
        {   guestkey = get_guest_key();
            if (guestkey != NC)
            {   if (whichcpu == 1)
                {   slave_to_master();
                }
                crtstatus |= CRT_DA;
                generate_interrupt(KYBDINTERRUPT);
    }   }   }
    else
    {   guestkey = get_guest_key();
        if (guestkey != NC)
        {   if (whichcpu == 1)
            {   slave_to_master();
            }
            crtstatus |= CRT_DA;
            generate_interrupt(KYBDINTERRUPT);
    }   }

#ifdef EMULATETIMER
    if ((ioport[0xEC].contents & 1) && frames % 6 == 0) // default interval timer (100 ms)
    {   generate_interrupt(9);
    }
#endif

    if (inverse)
    {   fgc = BLACK;
        bgc = WHITE;
    } else
    {
#ifdef INTERRUPTCOLOURS
     /* if ( (ioport[0xE9].contents & CRT_TTY) && !(psu & PSU_II)) */ bgc = BLACK;  // OK
        if ( (ioport[0xE9].contents & CRT_TTY) &&  (psu & PSU_II))    bgc = RED;    // bad II
        if (!(ioport[0xE9].contents & CRT_TTY) && !(psu & PSU_II))    bgc = GREEN;  //          bad crtcontrol
        if (!(ioport[0xE9].contents & CRT_TTY) &&  (psu & PSU_II))    bgc = YELLOW; // bad II + bad crtcontrol
     /* Red   is the colour of bad interrupts
        Green is the colour of bad crtcontrol */
#else
        bgc = BLACK;
#endif
        fgc = WHITE;
    }

    if (frameskip != FRAMESKIP_INFINITE && frames % (ULONG) frameskip == 0)
    {   for (y = 0; y < 25; y++)
        {   for (x = 0; x < 80; x++)
            {   t = vdu[x][y] & 0x7F;
                if   (t <= 0x1F) { inverted = TRUE; t += 0x20; } // $00..$1F -> $20..$3F
                elif (t >= 0x60) { inverted = TRUE; t -= 0x20; } // $60..$7F -> $40..$5F
                else               inverted = FALSE;

                for (yy = 0; yy < 10; yy++)
                {   for (xx = 0; xx < 7; xx++)
                    {   if
                        (   xx >= 1
                         && xx <= 5
                         && yy <= 6
                         && (   ( inverted && !(lcvdu_chars[t][yy] & (0x20 >> xx)))
                             || (!inverted &&  (lcvdu_chars[t][yy] & (0x20 >> xx)))
                        )   )
                        {   changepixel((x * 7) + xx, (y * 10) + yy, fgc);
                        } else
                        {   changebgpixel((x * 7) + xx, (y * 10) + yy, bgc);
        }   }   }   }   }

        if (!blink || frames % 20 < 10)
        {   // draw cursor
            if (inverse)
            {   fgc = BLACK;
            } else
            {   fgc = WHITE;
            }
            for (yy = 0; yy < 2; yy++)
            {   for (xx = 0; xx < 5; xx++)
                {   changepixel((vdu_x * 7) + 1 + xx, (vdu_y * 10) + 7 + yy, fgc);
    }   }   }   }

    if (drawmode)
    {   twin_drawhelpgrid();
    }
    wa_checkinput();
    endofframe(bgc);
}

EXPORT UBYTE twin_readport(int port)
{   FAST UBYTE oldtrack,
               oldsector,
               t;
    FAST int   oldmode,
               oldoffset;

    // assert(machine == TWIN);

    if (whichcpu == 0)
    {   oldtrack  = drive[curdrive].track;
        oldsector = drive[curdrive].sector;
        oldmode   = drive_mode;
        oldoffset = drive[curdrive].blockoffset;

        switch (port)
        {
        case 0xD0: // HSPT control port ("PCPT" = "Papertape Control PorT")
            t = 0x7F; // means OK
        acase 0xD1: // HSPT data port ("PDPT = "Papertape Data PorT")
            if (papertapemode[1] == TAPEMODE_PLAY)
            {   papertapebyte[1] = t = PapertapeBuffer[1][papertapewhere[1]];
                papertapewhere[1]++;
                if (papertapewhere[1] == papertapelength[1])
                {   papertape_stop(1); // this calls update_papertape()
                } else
                {   update_papertape(1, FALSE);
            }   }
            else
            {   t = 0; // blank (unpunched) papertape
            }
        acase 0xE8:
            if (guestkey == NC)
            {   t = 0xFF;
            } else
            {   t = (UBYTE) guestkey;
                guestkey = NC;
            }
            crtstatus &= ~(CRT_DA);
        acase 0xE9:
            t = crtstatus;
            /* bits 7..5: unused
               bit  4   : parity error
               bit  3   : framing error
               bit  2   : data overrun
               bit  1   : transmit buffer empty
               bit  0   : data available */
        acase 0xEA:
            t = drive_rst ? 0x7F : 0xFF;
            /* bit  7   : flag (1=controller has data or can accept data)
               bit  1   : printer OK
               bit  0   : printer ready */
        acase 0xEB:
            if (drive_control == IND && drive_mode == DRIVEMODE_READING && drive_stage == 4 && !drive_rst)
            {   if (!drive[curdrive].inserted)
                {   t = 0x0; // guess
                    if (verbosedisk)
                    {   zprintf(TEXTPEN_VERBOSE, LLL(MSG_EMPTYREAD,         "Attempted to read from empty drive %d!\n"       ),                         curdrive);
                }   }
                elif (drive[curdrive].track >= 77)
                {   t = 0x0; // guess
                    if (verbosedisk)
                    {   zprintf(TEXTPEN_VERBOSE, LLL(MSG_ILLEGALTRACKREAD,  "Attempted to read from track %d on drive %d!\n" ), drive[curdrive].track,  curdrive);
                }   }
                elif (drive[curdrive].sector >= 32)
                {   t = 0x0; // guess
                    if (verbosedisk)
                    {   zprintf(TEXTPEN_VERBOSE, LLL(MSG_ILLEGALSECTORREAD, "Attempted to read from sector %d on drive %d!\n"), drive[curdrive].sector, curdrive);
                }   }
                else
                {   get_disk_byte(curdrive, TRUE);
                    t = drive[curdrive].contents[diskbyte];
                    if
                    (
#ifdef VERBOSEDISKREADS
                        verbosedisk
                     ||
#endif
                        (   (drive[curdrive].flags[diskbyte / 8] & (1 << (diskbyte % 8)))
                         && watchreads
                      // && conditional(&fp[address], data, TRUE)
                    )   )
                    {   zprintf
                        (   TEXTPEN_LOG,
                            LLL
                            (   MSG_READFLOPPY2,
                               "Instruction at $%X read $%02X [%s] from floppy byte $%X. Previous IAR/PC was $%X.\n\n"
                            ), // we should say which drive too
                            (int) iar,
                            (int) t,
                                  asciiname_short[t],
                                  diskbyte,
                            (int) oldiar
                        );
                        if
                        (   (drive[curdrive].flags[diskbyte / 8] & (1 << (diskbyte % 8)))
                         && watchreads
                      // && conditional(&fp[address], data, TRUE)
                        )
                        {   set_pause(TYPE_BP);
                    }   }

                    if (ambient && drive[curdrive].blockoffset == 0)
                    {   play_sample(SAMPLE_GRIND);
                    }
                    if (drive[curdrive].blockoffset == 127)
                    {   drive[curdrive].blockoffset = 0;
                        sound_stop(GUESTCHANNELS + SAMPLE_GRIND);
                        if (drive[curdrive].sector == 31)
                        {   drive[curdrive].sector = 0;
                            drive[curdrive].track++;
                            if (ambient)
                            {   play_sample(SAMPLE_STEP);
                        }   }
                        else
                        {   drive[curdrive].sector++;
                    }   }
                    else
                    {   drive[curdrive].blockoffset++;
                    }
                    generate_interrupt(11);
                    get_disk_byte(curdrive, FALSE);
                    drive[curdrive].viewstart = diskbyte;
            }   }
            else
            {   t = drive[curdrive].writeprotect ? 8 : 0;
                if (drive[curdrive].track >= 77 || drive[curdrive].sector >= 32)
                {   t |= 0x20;
            }   }
        adefault:
            t = 0;
    }   }
    else // slave
    {   t = 0;
    }

    logport(port, t, FALSE);

    if
    (   whichcpu == 0 // master
     && (   drive[curdrive].track       != oldtrack
         || drive[curdrive].sector      != oldsector
         || drive_mode                  != oldmode
         || drive[curdrive].blockoffset != oldoffset)
    )
    {   update_floppydrive(1, curdrive);
    }

    return t;
}

EXPORT void twin_writeport(int port, UBYTE data)
{   FAST UBYTE oldtrack,
               oldsector;
    FAST int   oldmode,
               oldoffset;

    if (whichcpu == 0) // master
    {   oldtrack  = drive[curdrive].track;
        oldsector = drive[curdrive].sector;
        oldmode   = drive_mode;
        oldoffset = drive[curdrive].blockoffset;

        switch (port)
        {
     /* case 0xD0: // HSPT control port ("PCPT" = "Papertape Control PorT")
            ; // HSPT driver writes $18 here to "turn off HSPT" and then $88 here immediately afterwards to "enable and drive left"
        break;
     */ case 0xE0: // "slave command byte" (p. 5-4 of manual)
            ; // ioport[0xE0].contents = data;
            // We assume they are enabled when bit 4 is set but the manual is ambiguous.
        acase 0xE8: // RS-232C?
            thecout((UBYTE) (data & 0x7F));
            crtstatus |= CRT_TBMT;
            generate_interrupt(SCRNINTERRUPT);
        acase 0xE9:
            ioport[0xE9].contents = data;
            /* "The status byte which controls the TTY paper tape reader, the UART parity cicruits, and enables the TTY interrupts".
               bits 7..3: unused
               bit  2   : enable TTY interrupts
               bit  1   : parity select
               bit  0:  : TTY paper tape reader */
        acase 0xEA:
            switch (data & 7)
            {
            case PTS:
                drive_rst     = FALSE;
                drive_control = data & 7;
                if ((ioport[0xEB].contents & 0x7F) == CR)
                {   c306_printline();
                } else
                {   printer[prtunit].queue[printer[prtunit].queuepos++] = ioport[0xEB].contents;
                    if (printer[prtunit].queuepos == 132)
                    {   c306_printline();
                }   }
                generate_interrupt(10);
                if (data == 0x22)
                {   drive[curdrive].blockoffset = 0;
                }
            acase RST:
                drive_rst     = TRUE;
            adefault:
                drive_rst     = FALSE;
                drive_control = data & 7;
            }
        acase 0xEB:
            switch (drive_control)
            {
            case SIO:
                switch (data)
                {
                case 0x0E: // Write Formatted
                    drive_stage = 1;
                    set_drive_mode(DRIVEMODE_WRITING);
                acase 0x88: // Normal Read
                    drive_stage = 1;
                    set_drive_mode(DRIVEMODE_READING);
                adefault:
                    drive_stage = 0;
                    set_drive_mode(DRIVEMODE_IDLE);
                }
         /* acase IND:
                ; sometimes $11 is written here
         */ acase OTD:
                switch (drive_stage)
                {
                case  1: curdrive               = data; drive_stage = 2; // if (verbosedisk) zprintf(TEXTPEN_VERBOSE, "Drive is %d.\n" , data);
                acase 2: drive[curdrive].sector = data; drive_stage = 3; // if (verbosedisk) zprintf(TEXTPEN_VERBOSE, "Sector is %d.\n", data);
                acase 3: drive[curdrive].track  = data; drive_stage = 4; // if (verbosedisk) zprintf(TEXTPEN_VERBOSE, "Track is %d.\n" , data);
                    generate_interrupt(11);
                acase 4:
                    if (drive_mode == DRIVEMODE_WRITING)
                    {   if (!drive[curdrive].inserted)
                        {   if (verbosedisk)
                            {   zprintf(TEXTPEN_VERBOSE, LLL(MSG_EMPTYWRITE,         "Attempted to write to empty drive %d!\n"       ),                         curdrive);
                        }   }
                        elif (drive[curdrive].track >= 77)
                        {   if (verbosedisk)
                            {   zprintf(TEXTPEN_VERBOSE, LLL(MSG_ILLEGALTRACKWRITE,  "Attempted to write to track %d on drive %d!\n" ), drive[curdrive].track,  curdrive);
                        }   }
                        elif (drive[curdrive].sector >= 32)
                        {   if (verbosedisk)
                            {   zprintf(TEXTPEN_VERBOSE, LLL(MSG_ILLEGALSECTORWRITE, "Attempted to write to sector %d on drive %d!\n"), drive[curdrive].sector, curdrive);
                        }   }
                        else
                        {   get_disk_byte(curdrive, TRUE);
                            if
                            (
#ifdef VERBOSEDISKWRITES
                                verbosedisk
                             ||
#endif
                                (   (drive[curdrive].flags[diskbyte / 8] & (1 << (diskbyte % 8)))
                                 && watchwrites != WATCH_NONE
                                 && (watchwrites == WATCH_ALL || (!drive[curdrive].writeprotect && data != drive[curdrive].contents[diskbyte]))
                              // && conditional(&fp[address], data, TRUE)
                            )   )
                            {   zprintf
                                (   TEXTPEN_LOG,
                                    LLL
                                    (   MSG_WROTEFLOPPY2,
                                        "Instruction at $%X wrote $%02X [%s] to floppy byte $%X. Previous IAR/PC was $%X.\n\n"
                                    ), // we should say which drive too
                                    (int) iar,
                                    (int) data,
                                          asciiname_short[data],
                                          diskbyte,
                                    (int) oldiar
                                );
                                if
                                (   (drive[curdrive].flags[diskbyte / 8] & (1 << (diskbyte % 8)))
                                 && watchwrites != WATCH_NONE
                                 && (watchwrites == WATCH_ALL || (!drive[curdrive].writeprotect && data != drive[curdrive].contents[diskbyte]))
                              // && conditional(&fp[address], data, TRUE)
                                )
                                {   set_pause(TYPE_BP);
                            }   }
                            if (!drive[curdrive].writeprotect)
                            {   drive[curdrive].contents[diskbyte] = data;
                            }
                            if (ambient && drive[curdrive].blockoffset == 0)
                            {   play_sample(SAMPLE_GRIND);
                            }
                            if (drive[curdrive].blockoffset == 127)
                            {   drive[curdrive].blockoffset = 0;
                                sound_stop(GUESTCHANNELS + SAMPLE_GRIND);
                                if (drive[curdrive].sector == 31)
                                {   drive[curdrive].sector = 0;
                                    drive[curdrive].track++;
                                    if (ambient)
                                    {   play_sample(SAMPLE_STEP);
                                }   }
                                else
                                {   drive[curdrive].sector++;
                            }   }
                            else
                            {   drive[curdrive].blockoffset++;
                            }
                            generate_interrupt(11);
                            get_disk_byte(curdrive, FALSE);
                            drive[curdrive].viewstart = diskbyte;
                }   }   }
            acase PTS:
                ; // ioport[0xEB].contents = data;
            }
        acase 0xEC:
            ; // enable/disable interval timer
            ; // bit 0: clear=disabled, set=enabled
        acase 0xED:
            ; // master memory protect
        acase 0xEE:
            switch (ioport[0xEE].contents & 1) // old bank
            {
            case  0: memcpy( slavemem                 , &memory[    16 * KILOBYTE], 16 * KILOBYTE                ); // master $4000..$7FFF -> slave $0000..$3FFF
                     memcpy( slaveflags               , &memflags[  16 * KILOBYTE], 16 * KILOBYTE * sizeof(MEMFLAG));
            acase 1: memcpy(&slavemem[  16 * KILOBYTE], &memory[    16 * KILOBYTE], 16 * KILOBYTE                ); // master $4000..$7FFF -> slave $4000..$7FFF
                     memcpy(&slaveflags[16 * KILOBYTE], &memflags[  16 * KILOBYTE], 16 * KILOBYTE * sizeof(MEMFLAG));
            }
            switch (data & 1) // new bank
            {
            case  0: memcpy(&memory[    16 * KILOBYTE],  slavemem                 , 16 * KILOBYTE                ); // slave $0000..$3FFF -> master $4000..$7FFF
                     memcpy(&memflags[  16 * KILOBYTE],  slaveflags               , 16 * KILOBYTE * sizeof(MEMFLAG));
            acase 1: memcpy(&memory[    16 * KILOBYTE], &slavemem[  16 * KILOBYTE], 16 * KILOBYTE                ); // slave $4000..$7FFF -> master $4000..$7FFF
                     memcpy(&memflags[  16 * KILOBYTE], &slaveflags[16 * KILOBYTE], 16 * KILOBYTE * sizeof(MEMFLAG));
            }
            // ioport[0xEE].contents = data;
        // $F8..$FF are debug I/O ports (not emulated)
        }

        // if curdrive has changed above we might do unnecessary refreshing
        if (drive[curdrive].track != oldtrack || drive[curdrive].sector != oldsector)
        {   update_floppydrive(2, curdrive);
        } elif (drive_mode != oldmode || drive[curdrive].blockoffset != oldoffset)
        {   update_floppydrive(1, curdrive);
    }   }
    else
    {   switch (port)
        {
        case  0xF2: /* slave_to_master(); */ generate_interrupt(21); // SVC 6
        acase 0xF3: /* slave_to_master(); */ generate_interrupt(20); // SVC 5
        acase 0xF4: /* slave_to_master(); */ generate_interrupt(19); // SVC 4
        acase 0xF5: /* slave_to_master(); */ generate_interrupt(18); // SVC 3
        acase 0xF6: /* slave_to_master(); */ generate_interrupt(17); // SVC 2
        acase 0xF7: /* slave_to_master(); */ generate_interrupt(16); // SVC 1
    }   }

    logport(port, data, TRUE);
}

EXPORT FLAG twin_load_disk(FLAG wantasl, int whichdrive)
{   UBYTE t;
    UWORD tt;
    FILE* DiskHandle /* = NULL */ ;
    int   disksize,
          i,
          whichsector,
          whichtrack;

    // asl() turns sound off and on for us
    if (wantasl)
    {   if (asl
        (   LLL(MSG_HAIL_INSERTDISK, "Insert Disk"),
#ifdef WIN32
            "TWIN Floppy Disk (*.IMG, *.TWIN)\0*.IMG;*.TWIN\0All Files (*.*)\0*.*\0",
#endif
#ifdef AMIGA
            "#?.(IMG|TWIN)",
#endif
            FALSE,
            (STRPTR) path_disks, // starting directory
#ifdef WIN32
            drive[whichdrive].fn_disk, // starting filename
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
    if (disksize != 77 * 128 * 32 && disksize != 77 * 130 * 32)
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
    if (disksize == 77 * 128 * 32)
    {   if (fread(drive[whichdrive].contents, disksize, 1, DiskHandle) != 1)
        {   DISCARD fclose(DiskHandle);
            // DiskHandle = NULL;
            say("Can't read from file!");
            return FALSE;
    }   }
    else
    {   // assert(disksize == 77 * 130 * 32);
        for (whichtrack = 0; whichtrack < TWIN_TRACKS; whichtrack++)
        {   for (whichsector = 0; whichsector < TWIN_SECTORS; whichsector++)
            {   if (fread(&tt, 2, 1, DiskHandle) != 1)
                {   DISCARD fclose(DiskHandle);
                    // DiskHandle = NULL;
                    say("Can't read from file!");
                    return FALSE;
                } // implied else
#ifdef WIN32
                t = tt & 0x00FF;
#endif
#ifdef AMIGA
                t = (tt & 0xFF00) >> 8;
#endif
                if (t != whichtrack)
                {   zprintf(TEXTPEN_VERBOSE, "Warning: bad track number in TWIN file at byte $%X (calculated %d vs. stored %d)!\n", (whichtrack * 4160) + (whichsector * 130), whichtrack, t);
                }
#ifdef WIN32
                t = (tt & 0xFF00) >> 8;
#endif
#ifdef AMIGA
                t = tt & 0x00FF;
#endif
                if (t != whichsector)
                {   zprintf(TEXTPEN_VERBOSE, "Warning: bad sector number in TWIN file at byte $%X (calculated %d vs. stored %d)!\n", (whichtrack * 4160) + (whichsector * 130) + 1, whichsector, t);
                }
                if (fread(&drive[whichdrive].contents[(whichtrack * 4096) + (whichsector * 128)], 128, 1, DiskHandle) != 1)
                {   DISCARD fclose(DiskHandle);
                    // DiskHandle = NULL;
                    say("Can't read from file!");
                    return FALSE;
    }   }   }   }
    DISCARD fclose(DiskHandle);
    // DiskHandle = NULL;

    for (i = 0; i < TWIN_DISKSIZE / 8; i++)
    {   drive[whichdrive].flags[i] = 0;
    }

    drive[whichdrive].inserted = TRUE;
    twin_dir_disk(TRUE, whichdrive);
    if (!SubWindowPtr[SUBWINDOW_FLOPPYDRIVE])
    {   open_floppydrive(FALSE);
    }

    return TRUE;
}

EXPORT void twin_create_disk(int whichdrive)
{   int i;

    memset(drive[whichdrive].contents, 0, TWIN_DISKSIZE);

    if (twin_dosver != TWIN_NODOS)
    {   // assert(strlen(hostmachines[TWIN].titlebartext <= 48);
        strcpy((STRPTR) &drive[whichdrive].contents[3], hostmachines[TWIN].titlebartext);
        strupr((STRPTR) &drive[whichdrive].contents[3]);
        drive[whichdrive].contents[3 + strlen(hostmachines[TWIN].titlebartext)] = CR; // replace NUL with CR (important for LDIR command)

        // This puts DOS on the disk...
        drive[whichdrive].contents[  2] =    1;
        drive[whichdrive].contents[ 90] =
        drive[whichdrive].contents[ 91] = 0xFF;
        switch (twin_dosver)
        {
        case TWIN_EXOS:
            drive[whichdrive].contents[128] =  'E';
            drive[whichdrive].contents[129] =  'X';
            drive[whichdrive].contents[130] =  'O';
            drive[whichdrive].contents[131] =  'S';
            drive[whichdrive].contents[132] =
            drive[whichdrive].contents[133] =  ' ';
        acase TWIN_SDOS20:
        case TWIN_SDOS40:
        case TWIN_SDOS42:
            drive[whichdrive].contents[128] =  'S';
            drive[whichdrive].contents[129] =  'D';
            drive[whichdrive].contents[130] =  'O';
            drive[whichdrive].contents[131] =  'S';
            drive[whichdrive].contents[132] =
            drive[whichdrive].contents[133] =  ' ';
        acase TWIN_TOS:
            drive[whichdrive].contents[128] =  'T';
            drive[whichdrive].contents[129] =  'O';
            drive[whichdrive].contents[130] =  'S';
            drive[whichdrive].contents[131] =
            drive[whichdrive].contents[132] =
            drive[whichdrive].contents[133] =  ' ';
        acase TWIN_UDOS:
            drive[whichdrive].contents[128] =  'U';
            drive[whichdrive].contents[129] =  'D';
            drive[whichdrive].contents[130] =  'O';
            drive[whichdrive].contents[131] =  'S';
            drive[whichdrive].contents[132] =
            drive[whichdrive].contents[133] =  ' ';
        }
        drive[whichdrive].contents[134] =
        drive[whichdrive].contents[135] =  ' ';
        drive[whichdrive].contents[(6 * 128)     ] =
        drive[whichdrive].contents[(6 * 128) +  1] = 0xFF;
        drive[whichdrive].contents[(6 * 128) + 39] =
        drive[whichdrive].contents[(6 * 128) + 40] =  128;

        switch (twin_dosver)
        {
        case  TWIN_EXOS:   exos_to_disk(whichdrive);
        acase TWIN_SDOS20:
        case  TWIN_SDOS40:
        case  TWIN_SDOS42: sdos_to_disk(whichdrive);
        acase TWIN_TOS:    tos_to_disk(whichdrive);
        acase TWIN_UDOS:   udos_to_disk(whichdrive);
    }   }

    for (i = 0; i < TWIN_DISKSIZE / 8; i++)
    {   drive[whichdrive].flags[i] = 0;
    }

    drive[whichdrive].fn_disk[0] = EOS;
    drive[whichdrive].inserted = TRUE;
    twin_dir_disk(TRUE, whichdrive);
}

EXPORT void twin_dir_disk(FLAG quiet, int whichdrive)
{   FLAG  first,
          ok;
    int   i, j,
          started,
          badclusters,
          fileclusters,
          numfiles,
          usedclusters,
          where;

    if (!drive[whichdrive].inserted)
    {   return;
    }

    /* $0000..$0007F: disk ID, etc.
       $0080..$002EF: filenames ( 8*78 bytes)
       $02F0..$002FF: unused
       $0300..$00FFF: bitmaps   (41*78 bytes) (5 padding bytes after each set of 3 files)
       $1000..$04FFF: DOS        (clusters  0.. 15)
       $5000..$4CFFF: user files (clusters 16..303) */

    for (i = 0; i < 78; i++)
    {   zstrncpy((STRPTR) drive[whichdrive].filename[i], (STRPTR) &drive[whichdrive].contents[0x80 + (i * 8)], 8);
        for (j = 7; j >= 0; j--)
        {   if (drive[whichdrive].filename[i][j] == ' ')
            {   drive[whichdrive].filename[i][j] = EOS;
            } else
            {   break;
        }   }
        if (drive[whichdrive].filename[i][0] == 0x0F)
        {   drive[whichdrive].filename[i][0] = '@';
        }

        drive[whichdrive].filesize[i] = (       drive[whichdrive].contents[fileoffset[i] + 38] * 256)
                                      +         drive[whichdrive].contents[fileoffset[i] + 39];
        drive[whichdrive].filesize[i] *= 128; // convert sectors to bytes
        drive[whichdrive].filesize[i] -= (128 - drive[whichdrive].contents[fileoffset[i] + 40]);
        if (drive[whichdrive].filesize[i] < 0)
        {   drive[whichdrive].filesize[i] = 0;
    }   }

    for (i = 0; i < 304; i++)
    {   drive[whichdrive].bam[i] = BAM_FREE;
    }
    for (i = 0; i < 78; i++)
    {   for (j = 0; j < 304; j++)
        {   if (drive[whichdrive].contents[fileoffset[i] + (j / 8)] & (0x80 >> (j % 8)))
            {   drive[whichdrive].bam[j] = BAM_FILE;
    }   }   }
    for (i = 0; i < 304; i++)
    {   if (drive[whichdrive].contents[90 + (i / 8)] & (0x80 >> (i % 8)))
        {   drive[whichdrive].bam[i] = BAM_FILE;
    }   }
    usedclusters = 0;
    for (i = 0; i < 304; i++)
    {   if (drive[whichdrive].bam[i] == BAM_FILE)
        {   usedclusters++;
    }   }
    badclusters = 0;
    for (i = 0; i < 304; i++)
    {   if (drive[whichdrive].contents[52 + (i / 8)] & (0x80 >> (i % 8)))
        {   drive[whichdrive].bam[i] = BAM_LOST;
            badclusters++;
    }   }
    freesize = (304 - badclusters - usedclusters) * 1024;

    if (!quiet)
    {   zprintf
        (   TEXTPEN_DISK,
            "Millisecond ID:  %d\n" \
            "Number of files: %d",
            (drive[whichdrive].contents[0] * 256) + drive[whichdrive].contents[1], // not sure of the format
            drive[whichdrive].contents[2]
        );
        numfiles = 0;
        for (i = 0; i < 78; i++)
        {   if (drive[whichdrive].filename[i][0] != EOS)
            {   numfiles++;
        }   }
        if (numfiles != drive[whichdrive].contents[2])
        {   zprintf(TEXTPEN_DISK, " (!)");
        }
        zprintf
        (   TEXTPEN_DISK,
            "\n" \
            "Disk identifier: \"%s\"\n\n",
            &drive[whichdrive].contents[3]
        );

#ifdef USEBYTES
        zprintf(TEXTPEN_DISK, "Filename    Size Bytes\n" \
                              "-------- ------- -----\n" \
                              "(Bad)     %6d ",
                badclusters * 1024);
#else
        zprintf(TEXTPEN_DISK, "Filename    Size Clusters\n" \
                              "-------- ------- --------\n" \
                              "(Bad)     %6d ",
                badclusters * 1024);
#endif
        started = -1;
        first = TRUE;
        for (i = 0; i < 304; i++)
        {   if (drive[whichdrive].contents[52 + (i / 8)] & (0x80 >> (i % 8)))
            {   if (started == -1)
                {   started = i;
            }   }
            else
            {   if (started != -1)
                {   if (first)
                    {   first = FALSE;
                    } else
                    {   zprintf(TEXTPEN_DISK, ", ");
                    }
                    if (started == i - 1)
                    {   tell_cluster(started);
                    } else
                    {   tell_clusters(started, i - 1);
                    }
                    started = -1;
        }   }   }
        if (started != -1)
        {   if (first)
            {   first = FALSE;
            } else
            {   zprintf(TEXTPEN_DISK, ", ");
            }
            tell_clusters(started, 303);
            // started = -1;
        }
        if (first)
        {   zprintf(TEXTPEN_DISK, "None\n");
        } else
        {   zprintf(TEXTPEN_DISK, "\n");
        }

        zprintf(TEXTPEN_DISK, "(Used)    %6d ", usedclusters * 1024); // aka "(Master)"
        started = -1;
        first = TRUE;
        for (i = 0; i < 304; i++)
        {   if (drive[whichdrive].contents[90 + (i / 8)] & (0x80 >> (i % 8)))
            {   if (started == -1)
                {   started = i;
            }   }
            else
            {   if (started != -1)
                {   if (first)
                    {   first = FALSE;
                    } else
                    {   zprintf(TEXTPEN_DISK, ", ");
                    }
                    if (started == i - 1)
                    {   tell_cluster(started);
                    } else
                    {   tell_clusters(started, i - 1);
                    }
                    started = -1;
        }   }   }
        if (started != -1)
        {   if (first)
            {   first = FALSE;
            } else
            {   zprintf(TEXTPEN_DISK, ", ");
            }
            tell_clusters(started, 303);
            // started = -1;
        }
        if (first)
        {   zprintf(TEXTPEN_DISK, "None\n");
        } else
        {   zprintf(TEXTPEN_DISK, "\n");
        }

        for (i = 0; i < 78; i++)
        {   if (drive[whichdrive].filename[i][0] != EOS)
            {   zprintf(TEXTPEN_DISK, "%8s %7d", drive[whichdrive].filename[i], drive[whichdrive].filesize[i]);
                fileclusters = 0;
                for (j = 0; j < 304; j++)
                {   if (drive[whichdrive].contents[fileoffset[i] + (j / 8)] & (0x80 >> (j % 8)))
                    {   fileclusters++;
                }   }
                if
                (   drive[whichdrive].filesize[i] < (fileclusters * 1024) - 1023
                 || drive[whichdrive].filesize[i] >  fileclusters * 1024
                 || drive[whichdrive].contents[fileoffset[i] + 40] > 128 // having more than 128 bytes in last sector is impossible
                )
                {   zprintf(TEXTPEN_DISK, "!");
                } else
                {   zprintf(TEXTPEN_DISK, " ");
                }

                for (j = 0; j < 304; j++)
                {   if (drive[whichdrive].contents[fileoffset[i] + (j / 8)] & (0x80 >> (j % 8)))
                    {   drive[whichdrive].firstcluster[i] = j;
                        break;
                }   }

                first = TRUE;
                started = -1;
                for (j = 0; j < 304; j++)
                {   if (drive[whichdrive].contents[fileoffset[i] + (j / 8)] & (0x80 >> (j % 8)))
                    {   if (started == -1)
                        {   started = j;
                    }   }
                    else
                    {   if (started != -1)
                        {   if (first)
                            {   first = FALSE;
                            } else
                            {   zprintf(TEXTPEN_DISK, ", ");
                            }
                            if (started == j - 1)
                            {   tell_cluster(started);
                            } else
                            {   tell_clusters(started, j - 1);
                            }
                            started = -1;
                }   }   }
                if (started != -1)
                {   if (first)
                    {   first = FALSE;
                    } else
                    {   zprintf(TEXTPEN_DISK, ", ");
                    }
                    tell_clusters(started, 303);
                    // started = -1;
                }
                if (first)
                {   zprintf(TEXTPEN_DISK, "None\n");
                } else
                {   zprintf(TEXTPEN_DISK, "\n");
                }

                where = 4096 + (drive[whichdrive].firstcluster[i] * 1024);
                if
                (   verbosedisk
                 && drive[whichdrive].contents[where    ] == 'M'
                 && drive[whichdrive].contents[where + 1] == 'O'
                 && drive[whichdrive].contents[where + 2] == 'D'
                )
                {   parse_mod(&drive[whichdrive].contents[where], TRUE, FALSE);
        }   }   }

        zprintf
        (   TEXTPEN_DISK,
            "\n" \
            "%6d bytes used by files.\n" \
            "  4096 bytes reserved for file list.\n" \
            "%6d bad bytes.\n" \
            "%6d bytes free.\n" \
            "%6d bytes total.\n\n",
            usedclusters * 1024,
            badclusters * 1024,
            freesize,
            TWIN_DISKSIZE
        );

        if (1) // (verbosedisk)
        {   ok = TRUE;
            for (i = 0; i < 304; i++)
            {   usedclusters = 0;
                for (j = 0; j < 78; j++)
                {   if (drive[whichdrive].contents[fileoffset[j] + (i / 8)] & (0x80 >> (i % 8)))
                    {   usedclusters++;
                }   }
                if (drive[whichdrive].contents[90 + (i / 8)] & (0x80 >> (i % 8)))
                {   if (usedclusters == 0)
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_CLUSTERUNDERUSED, "Cluster %d is marked as allocated but is not used by any file!\n"), i);
                        ok = FALSE;
                    } else
                    {   if (usedclusters >= 2)
                        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_CLUSTEROVERUSED2, "Cluster %d is used by %d files!\n"), i, usedclusters);
                            ok = FALSE;
                        }
                        if (drive[whichdrive].contents[52 + (i / 8)] & (0x80 >> (i % 8)))
                        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_BADCLUSTERUSED, "Bad cluster %d is in use!\n"), i);
                            ok = FALSE;
                }   }   }
                else
                {   if (usedclusters >= 1)
                    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_CLUSTEROVERUSED1, "Cluster %d is not marked as allocated but is used by %d file(s)!\n"), i, usedclusters);
                        ok = FALSE;
                        if (drive[whichdrive].contents[52 + (i / 8)] & (0x80 >> (i % 8)))
                        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_BADCLUSTERUSED, "Bad cluster %d is in use!\n"), i);
            }   }   }   }
            if (!ok)
            {   zprintf(TEXTPEN_CLIOUTPUT, "\n");
    }   }   }

    update_floppydrive(3, whichdrive);
}

EXPORT FLAG twin_extract_file(int whichfile, int whichdrive)
{   int   fileclusters,
          i,
          length,
          startbyte, endbyte,
          whichcluster;
    FLAG  exactmode;
    FILE* TheLocalHandle;

    fileclusters = 0;
    for (i = 0; i < 304; i++)
    {   if (drive[whichdrive].contents[fileoffset[whichfile] + (i / 8)] & (0x80 >> (i % 8)))
        {   fileclusters++;
    }   }
    if
    (   drive[whichdrive].filesize[whichfile] < (fileclusters * 1024) - 1023
     || drive[whichdrive].filesize[whichfile] >  fileclusters * 1024
    )
    {   exactmode = FALSE;
    } else
    {   exactmode = TRUE;
    }

    strcpy((char*) gtempstring, (const char*) drive[whichdrive].filename[whichfile]);
    length = strlen((const char*) gtempstring);
    for (i = 0; i < length; i++)
    {   if (gtempstring[i] == ';')
        {   gtempstring[i] = '.';
    }   }

    if ((TheLocalHandle = fopen((const char*) gtempstring, "wb")))
    {   whichcluster = 0;
        for (i = 0; i < 304; i++)
        {   if (drive[whichdrive].contents[fileoffset[whichfile] + (i / 8)] & (0x80 >> (i % 8)))
            {   whichcluster++;
                startbyte = 4096 + (i * 1024);
                if (exactmode && whichcluster == fileclusters && drive[whichdrive].filesize[i] % 1024)
                {   endbyte = 4096 + (i * 1024) + (drive[whichdrive].filesize[i] % 1024) - 1;
                } else
                {   endbyte = 4096 + (i * 1024) +                                  1023;
                }
                if (verbosedisk)
                {   zprintf(TEXTPEN_VERBOSE, "Extracting $%X..$%X...\n", startbyte, endbyte);
                }
                DISCARD fwrite(&drive[whichdrive].contents[startbyte], (size_t) (endbyte - startbyte + 1), 1, TheLocalHandle);
        }   }
        DISCARD fclose(TheLocalHandle);
        // TheLocalHandle = NULL;
        return TRUE;
    } else
    {   return FALSE;
}   }

EXPORT void twin_drawhelpgrid(void)
{   int x,  y,
        xx, yy,
        startx,
        starty;

    for (x = 0; x < 80; x++)
    {   for (y = 0; y < 25; y++)
        {   startx = x *  7;
            starty = y * 10;
            for (xx = 0; xx < 7; xx++)
            {   for (yy = 0; yy < 10; yy++)
                {   if (xx == 0 || xx == 6 || yy == 0 || yy == 9)
                    {   changepixel(startx + xx, starty + yy, GREY1);
}   }   }   }   }   }

EXPORT void twin_viewscreen(void)
{   TEXT rowchars[16 + 1];
    int  x, xx, y;

    if (machine != TWIN)
    {   return;
    }

    rowchars[16] = EOS;

    for (y = 0; y < 25; y++)
    {   for (xx = 0; xx < 80; xx += 16)
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
}   }

MODULE void generate_interrupt(int level)
{   if (whichcpu == 1)
    {   slave_to_master();
    }
    interrupt_2650 = TRUE;
    priflag[level] = TRUE;
    // don't call checkinterrupt() yet, as we might be in the middle of executing an instruction
}

EXPORT void twin_serialize_cos(void)
{   int i,
        x, y;

    serialize_byte_int(&whichcpu);
    serialize_byte(&ioport[0xEE].contents);

    if (serializemode == SERIALIZE_WRITE)
    {   if (whichcpu == 0)
        {   memcpy(          mastermem              ,  memory                 , 16 * KILOBYTE);
            switch (ioport[0xEE].contents & 1)
            {
            case  0: memcpy( slavemem               , &memory[  16 * KILOBYTE], 16 * KILOBYTE);
            acase 1: memcpy(&slavemem[16 * KILOBYTE], &memory[  16 * KILOBYTE], 16 * KILOBYTE);
        }   }
        else
        {   memcpy(          slavemem               ,  memory                 , 32 * KILOBYTE);
        }
        // no need to worry about memflags, they are not part of the COS/COR anyway
    }

    for (i = 0; i <= 0x3FFF; i++)
    {   serialize_byte(&mastermem[i]);
    }
    for (i = 0; i <= 0x7FFF; i++)
    {   serialize_byte(&slavemem[i]);

    }
    if (serializemode == SERIALIZE_READ)
    {   for (i =     0; i <=   0xFF; i++)
        {   masterflags[i] = NOWRITE | BIOS;
        }
        for (i = 0x100; i <= 0x3FFF; i++)
        {   masterflags[i] = 0;
        }
        for (i =     0; i <= 0x7FFF; i++)
        {   slaveflags[i] = 0;
        }
        if (whichcpu == 0) // master
        {   memcpy(          memory                 ,  mastermem                , 16 * KILOBYTE);
            memcpy(          memflags               ,  masterflags              , 16 * KILOBYTE * sizeof(MEMFLAG));
            switch (ioport[0xEE].contents & 1)
            {
            case  0: memcpy(&memory[  16 * KILOBYTE],  slavemem                 , 16 * KILOBYTE);
                     memcpy(&memflags[16 * KILOBYTE],  slaveflags               , 16 * KILOBYTE * sizeof(MEMFLAG));
            acase 1: memcpy(&memory[  16 * KILOBYTE], &slavemem[  16 * KILOBYTE], 16 * KILOBYTE);
                     memcpy(&memflags[16 * KILOBYTE], &slaveflags[16 * KILOBYTE], 16 * KILOBYTE * sizeof(MEMFLAG));
        }   }
        else // slave
        {   memcpy(          memory                 ,  slavemem                 , 32 * KILOBYTE);
            memcpy(          memflags               ,  slaveflags               , 32 * KILOBYTE * sizeof(MEMFLAG));
    }   }

    for (y = 0; y < 25; y++)
    {   for (x = 0; x < 80; x++)
        {   serialize_byte(&vdu[x][y]);
    }   }

    serialize_byte((UBYTE*) &drive[0].inserted);
    if (drive[0].inserted)
    {   for (i = 0; i < TWIN_DISKSIZE; i++)
        {   serialize_byte(&drive[0].contents[i]);
    }   }
    serialize_byte((UBYTE*) &drive[1].inserted);
    if (drive[1].inserted)
    {   for (i = 0; i < TWIN_DISKSIZE; i++)
        {   serialize_byte(&drive[1].contents[i]);
    }   }

    for (i = 0; i < 2; i++)
    {   serialize_byte(&drive[i].track);
        serialize_byte(&drive[i].sector);
        serialize_long((ULONG*) &drive[i].blockoffset);
        serialize_byte_int((int*) &drive[i].writeprotect);
    }
    serialize_byte_int((int*) &curdrive);
    serialize_byte_int(&drive_mode);
    serialize_byte_int(&drive_stage);
    serialize_byte(&crtstatus);
    serialize_byte(&ioport[0xE0].contents);
    serialize_byte(&ioport[0xE9].contents);
    serialize_byte(&ioport[0xEA].contents);
    serialize_byte(&ioport[0xEB].contents);
    serialize_byte(&ioport[0xEC].contents);
    serialize_byte((UBYTE*) &drive_rst);
    serialize_byte(&drive_control);
    offset += 2; // spare
    serialize_word(&other_iar);
    serialize_byte(&other_psu);
    serialize_byte(&other_psl);
    for (i = 0; i < 8; i++)
    {   serialize_word(&other_ras[i]);
    }
    for (i = 0; i < 7; i++)
    {   serialize_byte(&other_r[i]);
    }
    serialize_byte(&other_ininterrupt);
    // don't bother about halted
    serialize_long((ULONG*) &other_slice);
    serialize_byte((UBYTE*) &twin_escaped);
    for (i = 0; i < 32; i++)
    {   serialize_byte((UBYTE*) &priflag[i]);
    }
    serialize_byte((UBYTE*) &paperreaderenabled);
    serialize_byte((UBYTE*) &paperpunchenabled);
    if (cosversion >= 40)
    {   serialize_byte_int(&belling[0]);
        serialize_byte_int(&belling[1]);
    } else
    {   belling[0] = belling[1] = 0;
    }

    if (serializemode == SERIALIZE_READ)
    {   drive[0].fn_disk[0] =
        drive[1].fn_disk[0] = EOS;
        twin_dir_disk(TRUE, 0);
        twin_dir_disk(TRUE, 1);
}   }

MODULE void twin_runcpu(void)
{   FAST ULONG endcycle;

    // assert(slice_2650 >= 1);

    endcycle = cycles_2650 + slice_2650;
    if (endcycle < cycles_2650)
    {   // cycle counter will overflow, so we need to use the slow method
        while (slice_2650 >= 1)
        {   oldcycles = cycles_2650;
            checkstep();
            one_instruction();
            checkinterrupt();
            slice_2650 -= (cycles_2650 - oldcycles);
    }   }
    else
    {   // cycle counter will not overflow, so we can use a faster method
        oldcycles = cycles_2650;
        while (cycles_2650 < endcycle)
        {   checkstep();
            one_instruction();
            checkinterrupt();
        }
        slice_2650 -= (cycles_2650 - oldcycles);
}   }

EXPORT void master_to_slave(void)
{   UBYTE t;
    UWORD tt;
    int   i,
          temp;

    if (trace)
    {   zprintf(TEXTPEN_TRACE, LLL(MSG_MASTERTOSLAVE, "Switching from master IAR $%X to slave IAR $%X!\n\n"), iar, other_iar);
    }

    // Switch from master to slave CPU

    whichcpu = 1;

    for (i = 0; i <= 6; i++)
    {   t = r[i]; r[i] = other_r[i]; other_r[i] = t;
    }
    t  = psu; psu = other_psu; other_psu = t;
    t  = psl; psl = other_psl; other_psl = t;
    t  = ininterrupt; ininterrupt = other_ininterrupt; other_ininterrupt = t;
    for (i = 0; i <= 6; i++)
    {   tt = ras[i]; ras[i] = other_ras[i]; other_ras[i] = tt;
    }
    tt = iar; iar = other_iar; other_iar = tt;
    interrupt_2650 = FALSE;
    temp = slice_2650; slice_2650 = other_slice; other_slice = temp;

    // preserve master memory
    memcpy( mastermem                         ,  memory         , 16 * KILOBYTE);
    memcpy( masterflags                       ,  memflags       , 16 * KILOBYTE * sizeof(MEMFLAG));
    switch (ioport[0xEE].contents & 1)
    {
    case  0: memcpy( slavemem         , &memory[  16384], 16 * KILOBYTE);
             memcpy( slaveflags       , &memflags[16384], 16 * KILOBYTE * sizeof(MEMFLAG));
    acase 1: memcpy(&slavemem[  16384], &memory[  16384], 16 * KILOBYTE);
             memcpy(&slaveflags[16384], &memflags[16384], 16 * KILOBYTE * sizeof(MEMFLAG));
    }

    // set up slave memory
    memcpy( memory                             ,  slavemem      , 32 * KILOBYTE);
    memcpy( memflags                           ,  slaveflags    , 32 * KILOBYTE * sizeof(MEMFLAG));

    update_monitor_cpu(TRUE);
    update_memory(TRUE);

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Now running slave CPU.\n");
#endif
}

EXPORT void slave_to_master(void)
{   UBYTE t;
    UWORD tt;
    int   i,
          temp;

    // Switch from slave to master CPU

    if (trace)
    {   zprintf(TEXTPEN_TRACE, LLL(MSG_SLAVETOMASTER, "Switching from slave IAR $%X to master IAR $%X!\n\n"), iar, other_iar);
    }

    whichcpu = 0;

    for (i = 0; i <= 6; i++)
    {   t = r[i]; r[i] = other_r[i]; other_r[i] = t;
    }
    t  = psu; psu = other_psu; other_psu = t;
    t  = psl; psl = other_psl; other_psl = t;
    t  = ininterrupt; ininterrupt = other_ininterrupt; other_ininterrupt = t;
    for (i = 0; i <= 6; i++)
    {   tt = ras[i]; ras[i] = other_ras[i]; other_ras[i] = tt;
    }
    tt = iar; iar = other_iar; other_iar = tt;
    interrupt_2650 = FALSE;
    temp = slice_2650; slice_2650 = other_slice; other_slice = temp;

    // preserve slave memory
    memcpy( slavemem               ,  memory                 , 32 * KILOBYTE);
    memcpy( slaveflags             ,  memflags               , 32 * KILOBYTE * sizeof(MEMFLAG));

    // set up master memory
    memcpy( memory                 ,  mastermem              , 16 * KILOBYTE);
    memcpy( memflags               ,  masterflags            , 16 * KILOBYTE * sizeof(MEMFLAG));
    switch (ioport[0xEE].contents & 1)
    {
    case  0: memcpy(&memory[  16384], slavemem          , 16 * KILOBYTE);
             memcpy(&memflags[16384], slaveflags        , 16 * KILOBYTE * sizeof(MEMFLAG));
    acase 1: memcpy(&memory[  16384], &slavemem[  16384], 16 * KILOBYTE);
             memcpy(&memflags[16384], &slaveflags[16384], 16 * KILOBYTE * sizeof(MEMFLAG));
    }

    update_monitor_cpu(TRUE);
    update_memory(TRUE);

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Now running master CPU.\n");
#endif
}

EXPORT void view_svc(int svc)
{   int where;

    /* Currently, we show this before running the SVC. Therefore, we only
       show parts of the SRB (inputs not outputs).

       Ideally, we should also show the outputs (ie. the results of the
       operation) after running the SVC. */

    // assert(whichcpu == 0);

    where = (memory[0x4040 + (svc * 2)] * 256)
          +  memory[0x4041 + (svc * 2)]
          +  0x4000;

    zprintf
    (   TEXTPEN_LOG,
        "SVC number:             %d (slave extended I/O port $%02X)\n" \
        "Master priority:        %d\n" \
        "Slave priority:         %d\n" \
        "SRB pointer:            $%04X in master RAM\n" \
        "                        $%04X in slave RAM\n" \
        "SVC function code:      $%02X (",
        svc,
        0xF8 - svc,
        15 + svc,
        svc - 1,
        where,
        where - 0x4000,
        memory[where]
    );
    switch (memory[where] & 0x1F)
    {
    case     1: zprintf(TEXTPEN_LOG, "Read (srbFnRead)");
    acase    2: zprintf(TEXTPEN_LOG, "Write (srbFnWrite)");
    acase    3: zprintf(TEXTPEN_LOG, "Close device or file on channel (srbFnClose)");
    acase    4: zprintf(TEXTPEN_LOG, "Rewind file on channel (srbFnRewind)");
    acase    5: zprintf(TEXTPEN_LOG, "Delete file on channel (srbFnDelete)");
    acase    6: zprintf(TEXTPEN_LOG, "Rename file on channel (srbFnRename)");
    acase    7: zprintf(TEXTPEN_LOG, "srbFnDirectIO (for the floppy disk device)");
    acase 0x10: zprintf(TEXTPEN_LOG, "Assign channel to device or file (srbFnAssign)");
    acase 0x11: zprintf(TEXTPEN_LOG, "Get time (milliseconds (srbFnTime)");
    acase 0x12: zprintf(TEXTPEN_LOG, "Get overlay addresses (srbFnOvlAddr");
    acase 0x13: zprintf(TEXTPEN_LOG, "Get parameter (srbFnParam)");
    acase 0x14: zprintf(TEXTPEN_LOG, "Get device type (srbFnDevType)");
    acase 0x15: zprintf(TEXTPEN_LOG, "Get device status (srbFnDevStatus)");
    acase 0x16: zprintf(TEXTPEN_LOG, "Get last console input char (srbFnLastChar)");
    acase 0x17: zprintf(TEXTPEN_LOG, "Load overlay (srbFnLoadOvl)");
    acase 0x18: zprintf(TEXTPEN_LOG, "Execute overlay (srbFnExecOvl)");
    acase 0x19: zprintf(TEXTPEN_LOG, "Suspend execution (srbFnSuspend)");
    acase 0x1A: zprintf(TEXTPEN_LOG, "Exit (srbFnExit)");
    acase 0x1B: zprintf(TEXTPEN_LOG, "srbFnPause");
    acase 0x1C: zprintf(TEXTPEN_LOG, "srbFnSlvParam");
    acase 0x1D: if (twin_dosver == TWIN_TOS) /* TOS 1.0 */ zprintf(TEXTPEN_LOG, "srbFnClkTime");
                else                                       zprintf(TEXTPEN_LOG, "srbFnSlvMem");
    acase 0x1E: if (twin_dosver == TWIN_TOS) /* TOS 1.0 */ zprintf(TEXTPEN_LOG, "srbFnClkDate");
    acase 0x1F: zprintf(TEXTPEN_LOG, "Abort (srbFnAbort)");
    adefault:   zprintf(TEXTPEN_LOG, "Undefined command");
    }

    zprintf(TEXTPEN_LOG, "Block/special? %s\n", (memory[where] & 0x20) ? "Yes" : "No");
    zprintf(TEXTPEN_LOG, "Binary?        %s\n", (memory[where] & 0x40) ? "Yes" : "No");
    zprintf(TEXTPEN_LOG, "Proceed?       %s\n", (memory[where] & 0x80) ? "Yes" : "No");

    zprintf
    (   TEXTPEN_LOG,
        ")\n" \
        "Logical channel number: %d",
        memory[where + 1]
    );
    if (memory[where + 1] > 7)
    {   zprintf(TEXTPEN_LOG, "!");
    }
    zprintf
    (   TEXTPEN_LOG,
        "\n" \
        "I/O buffer length:      %d bytes\n" \
        "I/O buffer pointer:     $%02X%02X in master RAM\n" \
        "                        $%02X%02X in slave RAM\n\n",
        memory[where + 5],
        memory[where + 6] + 0x40,
        memory[where + 7],
        memory[where + 6],
        memory[where + 7]
    );
}

MODULE void c306_printline(void)
{   TRANSIENT       FLAG belled,
                         elongated_char,
                         elongated_line;
    TRANSIENT       TEXT thechar;
    TRANSIENT       int  charwidth,
                         condensed_line,
                         elevens         = 0,
                         i,
                         twentytwos      = 0,
                         x, y;
    PERSIST   const int  ribboncolour[4] = { BLACK, RED, DARKGREEN, BLUE };

/* C306    ASCII   array
$00..$1F $00..$1F -------- control codes
$40..$7F $20..$5F $00..$3F ' !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_'
$60..$7F $60..$7F $00..$3F                                 '@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_'
*/

    elongated_line = belled = FALSE;
    condensed_line = printer[prtunit].condensed;
    for (i = 0; i < printer[prtunit].queuepos; i++)
    {   if (printer[prtunit].queue[i] & 0x80)
        {   elongated_char  =  TRUE;
            printer[prtunit].queue[i] &= 0x7F;
        } else
        {   elongated_char  =  FALSE;
        }
        if (printer[prtunit].queue[i] >= 0x20 && printer[prtunit].queue[i] <= 0x5F)
        {   thechar     = printer[prtunit].queue[i] - 0x20;
        } elif (printer[prtunit].queue[i] >= 0x60)
        {   thechar     = printer[prtunit].queue[i] - 0x40;
        } else
        {   // assert(printer[prtunit].queue[i] <= 0x1F);
            if (printer[prtunit].queue[i] == BEL)
            {   belled = TRUE;
            } elif (printer[prtunit].queue[i] == 0x0E) // Shift Out (SO)
            {   elongated_line = TRUE;
            } elif (printer[prtunit].queue[i] == 0x12)
            {   condensed_line = condensed_line ? 0 : 1;
            }
            continue;
        }
        printer[prtunit].pdu[printer[prtunit].pdu_y][printer[prtunit].pdu_x++] = printer[prtunit].queue[i];

        if (condensed_line)
        {   if (elongated_char || elongated_line)
            {   charwidth = 14;
            } else
            {   charwidth =  7;
        }   }
        else
        {   if (elongated_char || elongated_line)
            {   charwidth = 22;
            } else
            {   charwidth = 11;
        }   }

        if (printer[prtunit].x + charwidth > C306PRINTERX + (PRINTERX * C306COLUMNS))
        {
#ifdef AMIGA
            printer[prtunit].pdu[printer[prtunit].pdu_y][printer[prtunit].pdu_x++] = LF;
#endif
            printer[prtunit].pdu[printer[prtunit].pdu_y][printer[prtunit].pdu_x++] = CR;
            printer[prtunit].pdu[printer[prtunit].pdu_y][printer[prtunit].pdu_x  ] = EOS;
            printer[prtunit].x = C306PRINTERX;
            printer[prtunit].pdu_x = 0;
            printer_scroll();
        }

        switch (charwidth)
        {
        case 7:
            for (y = 0; y < 7; y++)
            {   for (x = 0; x < 5; x++)
                {   if (c306_5x7[(int) thechar][y] & ( 0x10 >> x))
                    {   printer_changepixel(prtunit, C306XMARGIN + printer[prtunit].x +  x         , C306HEIGHT_FULL - C306PRINTERY + 2 + y, ribboncolour[printer[prtunit].ribbon]);
                    } else
                    {   printer_changepixel(prtunit, C306XMARGIN + printer[prtunit].x +  x         , C306HEIGHT_FULL - C306PRINTERY + 2 + y, C306C_PAPER);
            }   }   }
        acase 11:
            for (y = 0; y < 7; y++)
            {   for (x = 0; x < 9; x++)
                {   if (c306_9x7[(int) thechar][y] & (0x100 >> x))
                    {   printer_changepixel(prtunit, C306XMARGIN + printer[prtunit].x +  x         , C306HEIGHT_FULL - C306PRINTERY + 2 + y, ribboncolour[printer[prtunit].ribbon]);
                    } else
                    {   printer_changepixel(prtunit, C306XMARGIN + printer[prtunit].x +  x         , C306HEIGHT_FULL - C306PRINTERY + 2 + y, C306C_PAPER);
            }   }   }
            elevens++;
        acase 14:
            for (y = 0; y < 7; y++)
            {   for (x = 0; x < 5; x++)
                {   if (c306_5x7[(int) thechar][y] & ( 0x10 >> x))
                    {   printer_changepixel(prtunit, C306XMARGIN + printer[prtunit].x + (x * 2)    , C306HEIGHT_FULL - C306PRINTERY + 2 + y, ribboncolour[printer[prtunit].ribbon]);
                        printer_changepixel(prtunit, C306XMARGIN + printer[prtunit].x + (x * 2) + 1, C306HEIGHT_FULL - C306PRINTERY + 2 + y, ribboncolour[printer[prtunit].ribbon]);
                    } else
                    {   printer_changepixel(prtunit, C306XMARGIN + printer[prtunit].x + (x * 2)    , C306HEIGHT_FULL - C306PRINTERY + 2 + y, C306C_PAPER);
                        printer_changepixel(prtunit, C306XMARGIN + printer[prtunit].x + (x * 2) + 1, C306HEIGHT_FULL - C306PRINTERY + 2 + y, C306C_PAPER);
            }   }   }
        acase 22:
            for (y = 0; y < 7; y++)
            {   for (x = 0; x < 9; x++)
                {   if (c306_9x7[(int) thechar][y] & (0x100 >> x))
                    {   printer_changepixel(prtunit, C306XMARGIN + printer[prtunit].x + (x * 2)    , C306HEIGHT_FULL - C306PRINTERY + 2 + y, ribboncolour[printer[prtunit].ribbon]);
                        printer_changepixel(prtunit, C306XMARGIN + printer[prtunit].x + (x * 2) + 1, C306HEIGHT_FULL - C306PRINTERY + 2 + y, ribboncolour[printer[prtunit].ribbon]);
                    } else
                    {   printer_changepixel(prtunit, C306XMARGIN + printer[prtunit].x + (x * 2)    , C306HEIGHT_FULL - C306PRINTERY + 2 + y, C306C_PAPER);
                        printer_changepixel(prtunit, C306XMARGIN + printer[prtunit].x + (x * 2) + 1, C306HEIGHT_FULL - C306PRINTERY + 2 + y, C306C_PAPER);
            }   }   }
            twentytwos++;
        }
        printer[prtunit].x += charwidth;
        if (charwidth == 11 || charwidth == 22)
        // Each character in these modes is actually 11.55 (or 22.6) pixels wide, so that
        // 80 (or 40) of them are actually 924 pixels (8") wide rather than 880 (~7.6").
        {   if (charwidth == 22 || elevens % 2)
            {   printer[prtunit].x++; // takes care of the 0.5 pixels (1 every 2 "elevens" worth)
                if (((elevens * 2) + twentytwos) % 10 == 0) // takes care of the 0.05 pixels (1 every 20 "elevens" worth)
                {   printer[prtunit].x++;
    }   }   }   }

    if (printer[prtunit].x > C306PRINTERX)
    {
#ifdef AMIGA
        printer[prtunit].pdu[printer[prtunit].pdu_y][printer[prtunit].pdu_x++] = LF;
#endif
        printer[prtunit].pdu[printer[prtunit].pdu_y][printer[prtunit].pdu_x++] = CR;
        printer[prtunit].pdu[printer[prtunit].pdu_y][printer[prtunit].pdu_x  ] = EOS;
        printer[prtunit].x = C306PRINTERX;
        printer[prtunit].pdu_x = 0;
        printer_scroll();
    }

    if (ambient)
    {   play_sample(SAMPLE_PRINTER); // but we just wrote up to two lines, not one character
    }
    if (belled)
    {   play_bell(1);
    }
    update_printer();

    printer[prtunit].pdu_x = 0;
    if (printer[prtunit].pdu_y < C306ROWS - 1)
    {   printer[prtunit].pdu_y++;
    }
    printer[prtunit].queuepos = 0;

    if (ioport[0xEA].contents & 0x20)
    {   generate_interrupt(10); // we assume this is done after a line has been printed, and
                                // not, for example, after each character has been received.
}   }

EXPORT void c306_margins(int whichprinter)
{   int x, xx, y;

    // horizontal perforation
    if (printer[whichprinter].sprocketrow == C306ROWS - 1)
    {   for (x = 0; x < C306WIDTH_FULL; x += 2)
        {   printer_changepixel(whichprinter,                  x                                            , C306HEIGHT_FULL - C306PRINTERY + 10, GREY1);
    }   }

    // vertical perforations
    for (y = ((printer[whichprinter].sprocketrow % 2) ? 1 : 0); y < 11; y += 2)
    {   printer_changepixel(    whichprinter,                                              C306XMARGIN + 1  , C306HEIGHT_FULL - C306PRINTERY +  y, GREY1);
        printer_changepixel(    whichprinter,                            C306WIDTH_FULL - (C306XMARGIN + 1) , C306HEIGHT_FULL - C306PRINTERY +  y, GREY1);
    }

    // sprockets
    if (printer[whichprinter].sprocketrow % 3 == 1)
    {   for (xx = 0; xx < 2; xx++)
        {   for (x = 4; x <=  6; x++) printer_changepixel(whichprinter, 10 + x + (xx * (C306WIDTH_FULL - C306XMARGIN - 2)), C306HEIGHT_FULL - C306PRINTERY +  0, GREY1); // ....###
            for (x = 2; x <=  8; x++) printer_changepixel(whichprinter, 10 + x + (xx * (C306WIDTH_FULL - C306XMARGIN - 2)), C306HEIGHT_FULL - C306PRINTERY +  1, GREY1); // ..#######
            for (x = 1; x <=  9; x++) printer_changepixel(whichprinter, 10 + x + (xx * (C306WIDTH_FULL - C306XMARGIN - 2)), C306HEIGHT_FULL - C306PRINTERY +  2, GREY1); // .#########
            for (x = 1; x <=  9; x++) printer_changepixel(whichprinter, 10 + x + (xx * (C306WIDTH_FULL - C306XMARGIN - 2)), C306HEIGHT_FULL - C306PRINTERY +  3, GREY1); // .#########
            for (x = 0; x <= 10; x++) printer_changepixel(whichprinter, 10 + x + (xx * (C306WIDTH_FULL - C306XMARGIN - 2)), C306HEIGHT_FULL - C306PRINTERY +  4, GREY1); // ###########
            for (x = 0; x <= 10; x++) printer_changepixel(whichprinter, 10 + x + (xx * (C306WIDTH_FULL - C306XMARGIN - 2)), C306HEIGHT_FULL - C306PRINTERY +  5, GREY1); // ###########
            for (x = 0; x <= 10; x++) printer_changepixel(whichprinter, 10 + x + (xx * (C306WIDTH_FULL - C306XMARGIN - 2)), C306HEIGHT_FULL - C306PRINTERY +  6, GREY1); // ###########
            for (x = 1; x <=  9; x++) printer_changepixel(whichprinter, 10 + x + (xx * (C306WIDTH_FULL - C306XMARGIN - 2)), C306HEIGHT_FULL - C306PRINTERY +  7, GREY1); // .#########
            for (x = 1; x <=  9; x++) printer_changepixel(whichprinter, 10 + x + (xx * (C306WIDTH_FULL - C306XMARGIN - 2)), C306HEIGHT_FULL - C306PRINTERY +  8, GREY1); // .#########
            for (x = 2; x <=  8; x++) printer_changepixel(whichprinter, 10 + x + (xx * (C306WIDTH_FULL - C306XMARGIN - 2)), C306HEIGHT_FULL - C306PRINTERY +  9, GREY1); // ..#######
            for (x = 4; x <=  6; x++) printer_changepixel(whichprinter, 10 + x + (xx * (C306WIDTH_FULL - C306XMARGIN - 2)), C306HEIGHT_FULL - C306PRINTERY + 10, GREY1); // ....###
}   }   }

EXPORT void twin_prtdemo(void)
{   strcpy((char*) printer[prtunit].queue, "DEFAULT CONDENSATION, UNELONGATED:");
    printer[prtunit].queuepos = strlen((const char*) printer[prtunit].queue);
    printer[prtunit].queue[printer[prtunit].queuepos++] = CR;
    printer[prtunit].queue[printer[prtunit].queuepos  ] = EOS; // not really needed
    c306_printline();

    strcpy((char*) printer[prtunit].queue, " !\"#$%&'()*+,-./01234567890:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_");
    printer[prtunit].queuepos = strlen((const char*) printer[prtunit].queue);
    printer[prtunit].queue[printer[prtunit].queuepos++] = CR;
    printer[prtunit].queue[printer[prtunit].queuepos  ] = EOS; // not really needed
    c306_printline();

    printer[prtunit].queue[0] = 0x0E; // Shift Out (expand)
    strcpy((char*) &printer[prtunit].queue[1], "DEFAULT CONDENSATION, ELONGATED:");
    printer[prtunit].queuepos = strlen((const char*) printer[prtunit].queue);
    printer[prtunit].queue[printer[prtunit].queuepos++] = CR;
    printer[prtunit].queue[printer[prtunit].queuepos  ] = EOS; // not really needed
    c306_printline();

    printer[prtunit].queue[0] = 0x0E; // Shift Out (expand)
    strcpy((char*) &printer[prtunit].queue[1], " !\"#$%&'()*+,-./01234567890:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_");
    printer[prtunit].queuepos = strlen((const char*) printer[prtunit].queue);
    printer[prtunit].queue[printer[prtunit].queuepos++] = CR;
    printer[prtunit].queue[printer[prtunit].queuepos  ] = EOS; // not really needed
    c306_printline();

    printer[prtunit].queue[0] = 0x12; // Device Control 2 (alternate character set)
    strcpy((char*) &printer[prtunit].queue[1], "ALTERNATE CONDENSATION, UNELONGATED:");
    printer[prtunit].queuepos = strlen((const char*) printer[prtunit].queue);
    printer[prtunit].queue[printer[prtunit].queuepos++] = CR;
    printer[prtunit].queue[printer[prtunit].queuepos  ] = EOS; // not really needed
    c306_printline();

    printer[prtunit].queue[0] = 0x12; // Device Control 2 (alternate character set)
    strcpy((char*) &printer[prtunit].queue[1], " !\"#$%&'()*+,-./01234567890:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_");
    printer[prtunit].queuepos = strlen((const char*) printer[prtunit].queue);
    printer[prtunit].queue[printer[prtunit].queuepos++] = CR;
    printer[prtunit].queue[printer[prtunit].queuepos  ] = EOS; // not really needed
    c306_printline();

    printer[prtunit].queue[0] = 0x12; // Device Control 2 (alternate character set)
    printer[prtunit].queue[1] = 0x0E; // Shift Out (expand)
    strcpy((char*) &printer[prtunit].queue[2], "ALTERNATE CONDENSATION, ELONGATED:");
    printer[prtunit].queuepos = strlen((const char*) printer[prtunit].queue);
    printer[prtunit].queue[printer[prtunit].queuepos++] = CR;
    printer[prtunit].queue[printer[prtunit].queuepos  ] = EOS; // not really needed
    c306_printline();

    printer[prtunit].queue[0] = 0x12; // Device Control 2 (alternate character set)
    printer[prtunit].queue[1] = 0x0E; // Shift Out (expand)
    strcpy((char*) &printer[prtunit].queue[2], " !\"#$%&'()*+,-./01234567890:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_");
    printer[prtunit].queuepos = strlen((const char*) printer[prtunit].queue);
    printer[prtunit].queue[printer[prtunit].queuepos++] = CR;
    printer[prtunit].queue[printer[prtunit].queuepos  ] = EOS; // not really needed
    c306_printline();
}

EXPORT void change_printer(void)
{   int x, y;

    // assert(prtunit >= 0);
    // assert(prtunit <= 1);

    for (y = 0; y < PRINTERHEIGHT_VIEW; y++)
    {   for (x = 0; x < PRINTERWIDTH_VIEW; x++)
        {
#ifdef WIN32
            canvasdisplay[CANVAS_PRINTER][( y * PRINTERWIDTH_VIEW) + x] = pencolours[colourset][printer[prtunit].scrn[y + prtscrolly][x + prtscrollx]];
#endif
#ifdef AMIGA
            *(canvasbyteptr[CANVAS_PRINTER][y]                     + x) = bytepens[             printer[prtunit].scrn[y + prtscrolly][x + prtscrollx]];
#endif
}   }   }

EXPORT void twin_swapdisks(void)
{   // assert(machine == TWIN);

    tempdrive = drive[0];
    drive[0] = drive[1];
    drive[1] = tempdrive;

    update_floppydrive(3, 0);
    update_floppydrive(3, 1);
}

EXPORT FLAG load_mod(int localsize)
{   if (localsize < 128)
    {   zprintf(TEXTPEN_ERROR, "File is too short!\n\n");
        return FALSE;
    }

    return parse_mod(IOBuffer, (FLAG) (verbosedisk ? TRUE : FALSE), TRUE);
}

EXPORT FLAG save_mod(STRPTR filename, int startaddr, int endaddr, int progstart)
{   TRANSIENT FLAG  enslave;
    TRANSIENT int   fullsectors,
                    i,
                    remainder,
                    thelength;
    TRANSIENT FILE* MODHandle;
    PERSIST   UBYTE MODBuffer[128];

    // assert(startaddr <= endaddr);

#ifdef VERBOSE
    zprintf(TEXTPEN_CLIOUTPUT, "Saving %s from $%X..$%X as MOD...\n", filename, startaddr, endaddr);
#endif

    if ((startaddr & 0x6000) != (endaddr & 0x6000))
    {   zprintf(TEXTPEN_ERROR, "Start and end address must currently be on same page!\n");
    }
    if (endaddr - startaddr + 1 >= 8 * KILOBYTE)
    {   zprintf(TEXTPEN_ERROR, "MOD generation is currently limited to <= 8K!\n");
    }

    if (!(MODHandle = fopen(filename, "wb")))
    {   zprintf(TEXTPEN_CLIOUTPUT, "Can't open \"%s\" for writing!\n", filename);
        return FALSE;
    } // implied else

    if (whichcpu == 1)
    {   enslave = TRUE;
        slave_to_master();
    } else
    {   enslave = FALSE;
    }

    MODBuffer[0] = 'M';
    MODBuffer[1] = 'O';
    MODBuffer[2] = 'D';
    if   (startaddr >= 0x1500 && startaddr <= 0x187F) MODBuffer[3] = 1; // 1st overlay area
    elif (startaddr >= 0x1880 && startaddr <= 0x1BFF) MODBuffer[3] = 2; // 2nd overlay area
    elif (startaddr >= 0x4000                       ) MODBuffer[3] = 3; // slave RAM
    else                                              MODBuffer[3] = 0; // master RAM
    if (startaddr >= 0x4000)
    {   MODBuffer[4] = (startaddr - 0x4000) / 256;
        MODBuffer[5] = (startaddr - 0x4000) % 256;
        MODBuffer[6] = (  endaddr - 0x4000) / 256;
        MODBuffer[7] = (  endaddr - 0x4000) % 256;
        MODBuffer[8] = (progstart - 0x4000) / 256; // we are assuming that this is
        MODBuffer[9] = (progstart - 0x4000) % 256; // from the slave's perspective
    } else
    {   MODBuffer[4] =  startaddr           / 256;
        MODBuffer[5] =  startaddr           % 256;
        MODBuffer[6] =    endaddr           / 256;
        MODBuffer[7] =    endaddr           % 256;
        MODBuffer[8] =  progstart           / 256;
        MODBuffer[9] =  progstart           % 256;
    }
    for (i = 11; i <= 127; i++)
    {   MODBuffer[i] = 0;
    }
    thelength = strlen(hostmachines[TWIN].titlebartext);
    // assert(thelength <= 21);
    strcpy((char*) &MODBuffer[10], hostmachines[TWIN].titlebartext);
    MODBuffer[10 + thelength] = CR; // $0D

    MODBuffer[32] = 0;               // set bank 0
    MODBuffer[33] = startaddr / 256; // this is from the
    MODBuffer[34] = startaddr % 256; // master's perspective

    fullsectors = (endaddr - startaddr + 1) / 128;
    // assert(fullsectors <= 64);
    remainder   = (endaddr - startaddr + 1) % 128;
    if (fullsectors >= 1)
    {   MODBuffer[35] = (((fullsectors - 1) & 0x20) >> 5)
                      | (((fullsectors - 1) & 0x1F) << 3)
                      | 2;
        MODBuffer[36] = 128; // length of each sector
        if (remainder >= 1)
        {   MODBuffer[37] = 0x02;      // means 1 sector
            MODBuffer[38] = remainder; // length of sector
            MODBuffer[39] = 0x04;      // end
        } else
        {   MODBuffer[37] = 0x04;      // end
    }   }
    elif (remainder >= 1)
    {   MODBuffer[    35] = 0x02;      // means 1 sector
        MODBuffer[    36] = remainder; // length of sector
        MODBuffer[    37] = 0x04;      // end
    }

    DISCARD fwrite(MODBuffer, 128, 1, MODHandle);

    if (endaddr - startaddr + 1 >= 1)
    {   DISCARD fwrite(&memory[startaddr], endaddr - startaddr + 1, 1, MODHandle);
    }

    DISCARD fclose(MODHandle);
    // MODHandle = NULL;

    if (enslave)
    {   master_to_slave();
    }
    return TRUE;
}

MODULE FLAG parse_mod(UBYTE* modptr, FLAG showing, FLAG doing)
{   FLAG   enslave,
           rc;
    UBYTE  t;
    UBYTE* dataptr;
    int    eachsize,
           howmany,
           i, j,
           loadaddr = 0, // initialized to avoid a spurious SAS/C warning
           whichbank;

    if
    (   modptr[0] != 'M'
     || modptr[1] != 'O'
     || modptr[2] != 'D'
    )
    {   zprintf(TEXTPEN_ERROR, "Invalid file header!\n\n");
        return FALSE;
    }

    if (whichcpu == 1)
    {   enslave = TRUE;
        slave_to_master();
    } else
    {   enslave = FALSE;
    }

    if (showing)
    {   zprintf(TEXTPEN_VERBOSE, " Load address:  ");
    }
    switch (modptr[3])
    {
    case  1:
        if (showing) zprintf(TEXTPEN_VERBOSE, "Master RAM $1500..$187F (1st overlay area)");
        if (doing)
        {   loadaddr = (IOBuffer[4] * 256)
                     +  IOBuffer[5];
            startaddr_h = IOBuffer[6];
            startaddr_l = IOBuffer[7];
        }
    acase 2:
        if (showing) zprintf(TEXTPEN_VERBOSE, "Master RAM $1880..$1BFF (2nd overlay area)");
        if (doing)
        {   loadaddr = (IOBuffer[4] * 256)
                     +  IOBuffer[5];
            startaddr_h = IOBuffer[6];
            startaddr_l = IOBuffer[7];
        }
    acase 3:
        if (showing) zprintf(TEXTPEN_VERBOSE, "Slave RAM");
        if (doing)
        {   loadaddr    = (IOBuffer[4] * 256) // slave's
                        +  IOBuffer[5];       // perspective
            loadaddr    += 16384;             // slave -> master
            startaddr_h =  IOBuffer[6];
            startaddr_l =  IOBuffer[7];
            startaddr_h += 16384 / 256;        // slave -> master
        }
    adefault:
        if (showing) zprintf(TEXTPEN_VERBOSE, "Master RAM");
        if (doing)
        {   loadaddr = (IOBuffer[4] * 256)
                     +  IOBuffer[5];
            startaddr_h = IOBuffer[6];
            startaddr_l = IOBuffer[7];
    }   }
    if (showing)
    {   zprintf
        (   TEXTPEN_VERBOSE,
            " ($%02X%02X..$%02X%02X)\n" \
            " Start address: $%02X%02X\n" \
            " Identity:      \"",
            modptr[4],
            modptr[5],
            modptr[6],
            modptr[7],
            modptr[8],
            modptr[9]
        );
        for (i = 0x0A; i <= 0x1F; i++)
        {   if
            (   modptr[i] == EOS
             || modptr[i] == 0x0D // CR
            )
            {   break;
            }
            zprintf(TEXTPEN_VERBOSE, "%c", modptr[i]);
        }
        zprintf(TEXTPEN_VERBOSE, "\"\n Load commands:\n");
    }

    i = 0x20;
    dataptr = &modptr[128];
    do
    {   t = modptr[i];
        if ((t & 0x06) == 0) // Set Base
        {   if (showing)
            {   zprintf
                (   TEXTPEN_VERBOSE,
                    "  Set Base:  bank of %d, dest base addr of $%X (in master's view).\n",
                    (t & 0xC0) >> 6,
                    (modptr[i + 1] * 256) + modptr[i + 2]
                );
            }
            if (doing)
            {   whichbank = (t & 0xC0) >> 6;
                if (whichbank != 0)
                {   zprintf
                    (   TEXTPEN_ERROR,
                        "  Only bank 0 is currently supported!\n"
                    );
                    rc = FALSE;
                    goto DONE;
                } // implied else
                loadaddr = (modptr[i + 1] * 256) + modptr[i + 2];
            }
            i += 3;
        } elif ((t & 0x06) == 2) // Read Data
        {   if (showing)
            {   zprintf
                (   TEXTPEN_VERBOSE,
                    "  Read Data: %d sectors, each of %d bytes.\n",
                    (   ((t & 0xF8) >> 3)
                      | ((t & 0x01) << 5)
                    ) + 1,
                    modptr[i + 1]
                );
            }
            if (doing)
            {   howmany  = (((t & 0xF8) >> 3) | ((t & 0x01) << 5)) + 1,
                eachsize = modptr[i + 1];
                for (j = 0; j < howmany; j++)
                {   memcpy(&memory[loadaddr], dataptr, eachsize);
                    loadaddr += eachsize;
                    dataptr  += eachsize;
                }
            }
            i += 2;
        } elif ((t & 0x06) == 4) // Load End
        {   if (showing)
            {   zprintf(TEXTPEN_VERBOSE, "  Load End.\n");
            }
            // i++;
            rc = TRUE;
            goto DONE;
        } else
        {   zprintf(TEXTPEN_VERBOSE, "  Unexpected value!\n");
            // i++;
            rc = FALSE;
            goto DONE;
    }   }
    while (i < 128);

    zprintf(TEXTPEN_VERBOSE, "  No Load End command found!\n");
    rc = FALSE;

DONE:
    if (enslave)
    {   master_to_slave();
    }
    return rc;
}

EXPORT void twin_logport(int port, UBYTE data, FLAG write)
{
PERSIST const STRPTR foursome[5][4] = {
{ "1st",
  "2nd",
  "3rd",
  "4th"
},
{ "%00=mode 0, normal",
  "%01=mode 1, user I/O, system (slave) memory",
  "%10=special instructions only",
  "%11=mode 2, full user mode, user IO and memory"
},
{ "%00=no protect, 16K",
  "%01=not used (Slave block diagram says: 16 K protect, bit 14 override)",
  "%10=32K (base 0K or base 32K only), bit 15 override",
  "%11=16K"
},
{ "%00=0K",
  "%01=16K",
  "%10=32K",
  "%11=48K"
},
{ "%00 (IND)",
  "%01 (PTS)",
  "%10 (OTD)",
  "%11 (SIO)"
},
};

    switch (port)
    {
    case 0xC0:
        zprintf
        (   TEXTPEN_VERBOSE,
            "Hardware analyzer control status:\n" \
            " R/W polarity:                            %s\n" \
            " M/IO polarity:                           %s\n" \
            " Fetch don't care:                        %s\n" \
            " Arm stop on address compare plus count:  %s\n" \
            " Trace mode:                              %s\n" \
            " Access mode:                             %s\n",
            (data & 0x80) ? "%1=Set" : "%0=Unset",
            (data & 0x20) ? "%1=Set" : "%0=Unset",
            (data & 0x08) ? "%1=Set" : "%0=Unset",
            (data & 0x04) ? "%1=Set" : "%0=Unset",
            (data & 0x02) ? "%1=Set" : "%0=Unset",
            (data & 0x01) ? "%1=Set" : "%0=Unset"
        );
    acase 0xE0:
    case  0xE1:
    case  0xE2:
    case  0xE3:
        zprintf
        (   TEXTPEN_VERBOSE,
            "%s slave command:\n" \
            " Active/inactive:                         %s\n" \
            " User mode, ICE mode:                     %s\n" \
            " Interrupt enable:                        %s\n" \
            " Memory protect block size:               %s\n" \
            " Memory protect base:                     %s\n",
            foursome[0][ port & 0x03      ],               // bits 1..0 (of port)
            (data & 0x80) ? "%=1=Active"  : "%0=Inactive", // bit  7    (of data)
            foursome[1][(data & 0x60) >> 5],               // bits 6..5 (of data)
            (data & 0x10) ? "%=1=Enabled" : "%0=Disabled", // bit  4    (of data)
            foursome[2][(data & 0x0C) >> 2],               // bits 3..2 (of data)
            foursome[3][ data & 0x03      ]                // bits 1..0 (of data)
        );
    acase 0xE8:
        zprintf(TEXTPEN_VERBOSE, "Console data\n");
    acase 0xE9:
        if (!write)
        {   zprintf
            (   TEXTPEN_VERBOSE,
                "Console status:\n" \
                " Parity error?                            %s\n" \
                " Framing error?                           %s\n" \
                " Data overrun?                            %s\n" \
                " Transmit buffer empty?                   %s\n" \
                " Data available?                          %s\n",
                (data & 0x10) ? "%1=Yes"     : "%0=No",
                (data & 0x08) ? "%1=Yes"     : "%0=No",
                (data & 0x04) ? "%1=Yes"     : "%0=No",
                (data & 0x02) ? "%1=Yes"     : "%0=No",
                (data & 0x01) ? "%1=Yes"     : "%0=No"
            );
        } else
        {   zprintf
            (   TEXTPEN_VERBOSE,
                "Console control:\n" \
                " Teletype interrupts:                     %s\n" \
                " Parity select?                           %s\n" \
                " Paper tape reader on?                    %s\n",
                (data & 0x04) ? "%1=Enabled" : "%0=Disabled",
                (data & 0x02) ? "%1=Yes"     : "%0=No",
                (data & 0x01) ? "%1=Yes"     : "%0=No" // one byte at a time
            );
        }
    acase 0xEA:
        if (!write)
        {   zprintf
            (   TEXTPEN_VERBOSE,
                "Disk/printer status:\n" \
                " Controller has data or can accept data?  %s\n" \
                " Printer fault?                           %s\n" \
                " Printer ready?                           %s\n",
                (data & 0x80) ? "%1=Yes"     : "%0=No",
                (data & 0x02) ? "%1=Yes"     : "%0=No", // power off, out of paper, disconnected
                (data & 0x01) ? "%1=Yes"     : "%0=No"
            );
        } else
        {   zprintf
            (   TEXTPEN_VERBOSE,
                "Disk/printer control:\n" \
                " Printer interrupt:                       %s\n" \
                " Disk interrupt:                          %s\n" \
                " C1..C0:                                  %s\n" \
                " Strobe?                                  %s\n",
                (data & 0x20) ? "%1=Enabled" : "%0=Disabled",
                (data & 0x10) ? "%1=Enabled" : "%0=Disabled",
                foursome[4][(data & 0x06) >> 1],
                (data & 0x01) ? "%1=No"      : "%0=Yes"
            );
        }
    acase 0xEB:
        if (!write)
        {   zprintf
            (   TEXTPEN_VERBOSE,
                "Disk status (read after Start I/O, Reset or Printer strobe):\n" \
                " Illegal operation?                       %s\n" \
                " Error recovery?                          %s\n" \
                " Write protect?                           %s\n" \
                " Device busy?                             %s\n" \
                " Unit check?                              %s\n",
                (data & 0x20) ? "%1=Yes"     : "%0=No",
                (data & 0x10) ? "%1=Yes"     : "%0=No",
                (data & 0x08) ? "%1=Yes"     : "%0=No",
                (data & 0x04) ? "%1=Yes"     : "%0=No",
                (data & 0x02) ? "%1=Yes"     : "%0=No"
            );
        } else
        {   zprintf(TEXTPEN_VERBOSE, "Disk/printer data\n");
        }
    acase 0xEC: zprintf(TEXTPEN_VERBOSE, "Interval timer control\n");
    acase 0xED: zprintf(TEXTPEN_VERBOSE, "Master memory protect\n");
    acase 0xEE: zprintf(TEXTPEN_VERBOSE, "Common memory bank\n");
    acase 0xF2: zprintf(TEXTPEN_VERBOSE, "SVC6\n");
    acase 0xF3: zprintf(TEXTPEN_VERBOSE, "SVC5\n");
    acase 0xF4: zprintf(TEXTPEN_VERBOSE, "SVC4\n");
    acase 0xF5: zprintf(TEXTPEN_VERBOSE, "SVC3\n");
    acase 0xF6: zprintf(TEXTPEN_VERBOSE, "SVC2\n");
    acase 0xF7: zprintf(TEXTPEN_VERBOSE, "SVC1\n");
    acase 0xF8:
        zprintf
        (   TEXTPEN_VERBOSE,
            "Debug control:\n" \
            " Debugging active/sequencer:              %s\n" \
            " Tracing active, single cycle?            %s\n" \
            " 2nd write breakpoint:                    %s\n" \
            " 2nd read  breakpoint:                    %s\n" \
            " 1st write breakpoint:                    %s\n" \
            " 1st read  breakpoint:                    %s\n",
            (data & 0x20) ? "%1=Disabled" : "%0=Enabled",
            (data & 0x10) ? "%1=No"       : "%0=Yes",
            (data & 0x08) ? "%1=Off"      : "%0=On",
            (data & 0x04) ? "%1=Off"      : "%0=On",
            (data & 0x02) ? "%1=Off"      : "%0=On",
            (data & 0x01) ? "%1=Off"      : "%0=On"
        );
    acase 0xF9:
        zprintf
        (   TEXTPEN_VERBOSE,
            "Debug command:\n",
            " Intrpt mask (tracing & brkpnts intrpts): %s\n" \
            " Forced jump:                             %s\n" \
            " Forced interrupt (unused):               %s\n" \
            " Forced reset:                            %s\n",
            (data & 0x08) ? "%1=Off"      : "%0=On ($F7)",
            (data & 0x04) ? "%1=Off"      : "%0=On ($FB)",
            (data & 0x02) ? "%1=Off"      : "%0=On ($FD)",
            (data & 0x01) ? "%1=Off"      : "%0=On ($FE)"
        );
    acase 0xFA:
        zprintf(TEXTPEN_VERBOSE, "Low byte of jump address\n");
    acase 0xFB:
        zprintf(TEXTPEN_VERBOSE, "High byte of jump address\n");
    acase 0xFC:
        if (write)
        {   zprintf(TEXTPEN_VERBOSE, "Low byte of 1st breakpoint address\n");
        } else
        {   zprintf(TEXTPEN_VERBOSE, "Low byte of Last Program Counter\n");
        }
    acase 0xFD:
        if (write)
        {   zprintf(TEXTPEN_VERBOSE, "High byte of 1st breakpoint address\n");
        } else
        {   zprintf(TEXTPEN_VERBOSE, "High byte of Last Program Counter\n");
        }
    acase 0xFE:
        if (write)
        {   zprintf(TEXTPEN_VERBOSE, "Low byte of 2nd breakpoint address\n");
        } else
        {   zprintf(TEXTPEN_VERBOSE, "Low byte of Next Program Counter\n");
        }
    acase 0xFF:
        if (write)
        {   zprintf(TEXTPEN_VERBOSE, "High byte of 2nd breakpoint address\n");
        } else
        {   zprintf(TEXTPEN_VERBOSE, "High byte of Next Program Counter\n");
        }
    adefault:
        if (port == GPIO + 5 && !write)
        {   zprintf
            (   TEXTPEN_VERBOSE,
                "GPIO RS-232 status:\n" \
                " CTS (Clear To Send):                     %s\n" \
                " DSR (Data Set Ready):                    %s\n" \
                " Carrier detect:                          %s\n" \
                " Ring indicator:                          %s\n",
                (data & 0x80) ? "%1=Set" : "%0=Unset",
                (data & 0x40) ? "%1=Set" : "%0=Unset",
                (data & 0x20) ? "%1=On"  : "%0=Off",
                (data & 0x10) ? "%1=On"  : "%0=Off"
            );
        } elif (port == GPIO + 6)
        {   if (!write)
            {   zprintf
                (   TEXTPEN_VERBOSE,
                    "GPIO teletype/RS-232 status:\n" \
                    " Parity error?                            %s\n" \
                    " Framing error?                           %s\n" \
                    " Data overrun?                            %s\n" \
                    " Transmit buffer empty?                   %s\n" \
                    " Data available?                          %s\n",
                    (data & 0x10) ? "%1=Yes" : "%0=No",
                    (data & 0x08) ? "%1=Yes" : "%0=No",
                    (data & 0x04) ? "%1=Yes" : "%0=No",
                    (data & 0x02) ? "%1=Yes" : "%0=No",
                    (data & 0x01) ? "%1=Yes" : "%0=No"
                );
            } else
            {   zprintf
                (   TEXTPEN_VERBOSE,
                    "GPIO teletype/RS-232 control:\n" \
                    " RTS (Request To Send)     (RS-232 only): %s\n" \
                    " DTR (Data Terminal Ready) (RS-232 only): %s\n" \
                    " Send restraint            (RS-232 only): %s\n" \
                    " Originate/answer mode     (RS-232 only): %s\n" \
                    " Local mode                (RS-232 only): %s\n" \
                    " Teletype interrupts:                     %s\n" \
                    " Parity select:                           %s\n" \
                    " Paper tape reader:                       %s\n",
                    (data & 0x80) ? "%1=Set"     : "%0=Unset",
                    (data & 0x40) ? "%1=Set"     : "%0=Unset",
                    (data & 0x20) ? "%1=Set"     : "%0=Unset",
                    (data & 0x10) ? "%1=Answer"  : "%0=Originate",
                    (data & 0x08) ? "%1=Set"     : "%0=Unset",
                    (data & 0x04) ? "%1=Enabled" : "%0=Disabled",
                    (data & 0x02) ? "%1=Even"    : "%0=Odd",
                    (data & 0x01) ? "%1=On"      : "%0=Off" // one byte at a time
                );
        }   }
        else
        {   zprintf(TEXTPEN_VERBOSE, "Unknown port!\n");
}   }   }

MODULE void tell_cluster(int cluster)
{
#ifdef USEBYTES
    zprintf(TEXTPEN_DISK, "$%X-$%X", 4096 + (cluster * 1024), 4096 + (cluster * 1024) + 1023);
#else
    zprintf(TEXTPEN_DISK, "%d"     ,         cluster                                        );
#endif
}

MODULE void tell_clusters(int first, int second)
{
#ifdef USEBYTES
    zprintf(TEXTPEN_DISK, "$%X-$%X", 4096 + (first   * 1024), 4096 + (second  * 1024) + 1023);
#else
    zprintf(TEXTPEN_DISK, "%d-%d"  ,         first          ,         second                );
#endif
}

EXPORT void twin_biosdetails(int ea)
{   switch (ea)
    {
    acase 0x136A: zprintf(TEXTPEN_LOG, "Set up DCB from the FCB\n");
    acase 0x136C: zprintf(TEXTPEN_LOG, "Set channel I/O complete\n");
    acase 0x2000: zprintf(TEXTPEN_LOG, "API_SupervisorCall\n");      // the main entry into DOS service functions. The calling program supplies the address of the Service Request Block (SRB) in registers R2R3. The SRB contains the return address. Called as a routine, using a branch instruction.
    acase 0x2003: zprintf(TEXTPEN_LOG, "API_IntSaveState\n"\
                                       "Save registers\n");          // saves all registers, for later restore. Called as a subroutine.
    acase 0x2006: zprintf(TEXTPEN_LOG, "API_IntRestrState\n"\
                                       "Restore registers\n");       // restore previously saved registers. Called as a subroutine.
    acase 0x2009: zprintf(TEXTPEN_LOG, "API_ConvertASCtoNum\n");     // converts a string representing a hexadecimal number into a binary number. Called as a subroutine.
    acase 0x200C: zprintf(TEXTPEN_LOG, "API_FileReadyIOComplete\n"); // indicates that file operations are complete and the file can be closed.
    acase 0x200F: zprintf(TEXTPEN_LOG, "API_Dispatcher\n");          // the job scheduler. Not normally called; programs and modules suspend or abort using Supervisor calls, which will run the dispatcher automatically.
    acase 0x2012: zprintf(TEXTPEN_LOG, "API_QueueError\n");          // report a module ID and error number. DOS will display the error number on the console when no other job is scheduled by the dispatcher.
    acase 0x2015: zprintf(TEXTPEN_LOG, "API_LookupDeviceName\n");    // parse a device name (such as CONI or LPT1).
    acase 0x2018: zprintf(TEXTPEN_LOG, "API_ParseCommand\n");        // parse and run a DOS command.
    acase 0x201B: zprintf(TEXTPEN_LOG, "API_SkipToCmdBuff1Param\n"); // used to parse the commandline parameters.
}   }

EXPORT void twin_delete_file(int whichfile, int whichdrive)
{   int i,
        startbyte, endbyte;

    drive[whichdrive].contents[2]--;                                   // decrement number of files on disk
    memset(&drive[whichdrive].contents[0x80 + (whichfile * 8)], 0, 8); // clear filename

    for (i = 0; i < 304; i++)
    {   if (drive[whichdrive].contents[fileoffset[whichfile] + (i / 8)] & (0x80 >> (i % 8)))
        {   drive[whichdrive].contents[90 + (i / 8)] &= ~(0x80 >> (i % 8)); // clear global bitmap
            startbyte = 4096 + (i * 1024);
            endbyte   = 4096 + (i * 1024) + 1023;
            memset(&drive[whichdrive].contents[startbyte], 0, (size_t) (endbyte - startbyte + 1)); // clear actual data
    }   }

    memset(&drive[whichdrive].contents[fileoffset[whichfile]], 0, 41); // clear local bitmap and size
}

EXPORT FLAG twin_rename_file(int whichfile, int whichdrive, STRPTR newname)
{   int i,
        thelength;

    thelength = strlen(newname);

    if (thelength > 8)
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FILENAMEISTOOLONG, "Filename is too long!\n\n"));
        return FALSE;
    }

    for (i = 0; i < thelength; i++)
    {   drive[    whichdrive].contents[0x80 + (whichfile * 8) + i] = toupper(newname[i]);
    }
    if (thelength < 8)
    {   for (i = thelength; i < 8; i++)
        {   drive[whichdrive].contents[0x80 + (whichfile * 8) + i] = ' ';
    }   }

    return TRUE;
}

EXPORT void twin_reset(void)
{   int i;

    // keyboard
    guestkey              = NC;
    twin_escaped          = FALSE;
    queuepos              = 0;

    // screen
    vdu_x = vdu_y         = 0;
    crtstatus             = CRT_TBMT; // clear DA, set TBMT
    ignore_cout           = FALSE;

    // floppy drives
    drive_mode            = DRIVEMODE_IDLE;
    drive[0].blockoffset  =
    drive[1].blockoffset  = 0;
    curdrive              = 0;
    ioport[0xEA].contents = drive_control = RST;
    drive_stage           = 0;

    // interrupts
    for (i = 0; i < 32; i++)
    {   priflag[i]        = FALSE;
    }

    if (whichcpu == 1)
    {   slave_to_master();
    }

    if (!post)
    {   iar  = 0x100;
        r[0] =  0x10;
        r[1] = r[2] = r[3] = r[4] = r[5] = r[6] = psu = 0;
        psl  =  0x60; // CC = GT, IDC
        ras[0] = 0xDA;
        ras[1] = ras[2] = ras[3] = ras[4] = ras[5] = ras[6] = ras[7] = 0;
    } else
    {   iar = 0;
}   }

EXPORT void twin_inject_file(STRPTR thefilename)
{   int   i,
          numsectors,
          remaining,
          thelength,
          thesize,
          whichfile;
    TEXT  filepart[8 + 1];
    FILE* TheLocalHandle /* = NULL */ ;

    cd_projects();

    thelength = strlen(thefilename);
    if (thelength > 8)
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FILENAMEISTOOLONG, "Filename is too long!\n\n"));
        goto END;
    }
    strcpy(filepart, thefilename);

    thesize = getsize(thefilename);
    if (thesize > freesize)
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_DISKISTOOFULL, "Disk is too full!\n\n"));
        goto END;
    }

    whichfile = -1;
    for (i = 0; i < 78; i++)
    {   if (drive[curdrive].filename[i][0] != EOS)
        {   drive[curdrive].filename[i][8] = EOS; // to avoid a spurious GCC warning
            if (!stricmp((const char*) filepart, (const char*) drive[curdrive].filename[i]))
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

    remaining = thesize;
    for (i = 0; i < 304; i++)
    {   if (drive[curdrive].bam[i] == BAM_FREE)
        {   drive[curdrive].contents[                   90 + (i / 8)] |= (0x80 >> (i % 8));
            drive[curdrive].contents[fileoffset[whichfile] + (i / 8)] |= (0x80 >> (i % 8));
            if (remaining > 1024)
            {   DISCARD fread(&drive[curdrive].contents[4096 + (i * 1024)],      1024 , 1, TheLocalHandle);
                remaining -= 1024;
            } else
            {   DISCARD fread(&drive[curdrive].contents[4096 + (i * 1024)], remaining , 1, TheLocalHandle);
                if (remaining < 1024)
                {   memset(   &drive[curdrive].contents[4096 + (i * 1024) + remaining], 0, 1024 - remaining); // clear unused area
                }
                goto DONE;
    }   }   }
    zprintf(TEXTPEN_CLIOUTPUT, "No more free blocks (corrupt disk?)!\n\n");

DONE:
    DISCARD fclose(TheLocalHandle);
    // TheLocalHandle = NULL;

    drive[curdrive].contents[2]++; // number of files on disk

    memset(&drive[curdrive].contents[0x80 + (whichfile * 8)], ' ', 8); // unused bytes must be spaces
    for (i = 0; i < thelength; i++)
    {   drive[0].contents[0x80 + (whichfile * 8) + i] = toupper(filepart[i]);
    }

    numsectors = thesize / 128;
    if (thesize == 0)
    {   numsectors++;
        drive[curdrive].contents[fileoffset[whichfile] + 40] =                0; // bytes used in last sector
    } elif (thesize % 128)
    {   numsectors++;
        drive[curdrive].contents[fileoffset[whichfile] + 40] = thesize    % 128; // bytes used in last sector
    } else
    {   drive[curdrive].contents[fileoffset[whichfile] + 40] =              128; // bytes used in last sector
    }
    drive[    curdrive].contents[fileoffset[whichfile] + 38] = numsectors / 256; // sectors used by file
    drive[    curdrive].contents[fileoffset[whichfile] + 39] = numsectors % 256; // (big-endian word)

    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
    twin_dir_disk(FALSE, curdrive); // we only need a quiet one, the loud one is done just as a courtesy
  
END:
    cd_progdir();
}

EXPORT void twin_view_dos(void)
{   int i,
        localoffset = 0; // initialized to avoid a spurious SAS/C warning

    // assert(twin_dosver != TWIN_NODOS);

    // JCB (always $2050..$20D0 but we don't show $20D0)

    zprintf
    (   TEXTPEN_VIEW,
        "Jobs ($2050..$20CF):\n" \
        "Job  CurSta PrvSta Asgd Actv Wtng Cplt JobWtng Vec Bank Ofs SAddr SRBRt JobRt\n" \
        "---  ------ ------ ---- ---- ---- ---- ------- --- ---- --- ----- ----- -----\n"
    );

    for (i = 0; i < 4; i++) // job number
    {   zprintf
        (   TEXTPEN_VIEW,
            "  %d  ",
            i
        );
        switch (memory[0x2050 + i])
        {
        case  0:  zprintf(TEXTPEN_VIEW, "Idle  ");
        acase 1:  zprintf(TEXTPEN_VIEW, "Loaded");
        acase 2:  zprintf(TEXTPEN_VIEW, "Ready ");
        acase 3:  zprintf(TEXTPEN_VIEW, "Runnng");
        acase 4:  zprintf(TEXTPEN_VIEW, "Waitng");
        acase 5:  zprintf(TEXTPEN_VIEW, "SRB OK");
        acase 6:  zprintf(TEXTPEN_VIEW, "Suspnd");
        acase 7:  zprintf(TEXTPEN_VIEW, "Abortg");
        acase 8:  zprintf(TEXTPEN_VIEW, "Paused");
        adefault: zprintf(TEXTPEN_VIEW, "?     ");
        }
        zprintf(TEXTPEN_VIEW, " ");
        switch (memory[0x2054 + i])
        {
        case  0:  zprintf(TEXTPEN_VIEW, "Idle  ");
        acase 1:  zprintf(TEXTPEN_VIEW, "Loaded");
        acase 2:  zprintf(TEXTPEN_VIEW, "Ready ");
        acase 3:  zprintf(TEXTPEN_VIEW, "Runnng");
        acase 4:  zprintf(TEXTPEN_VIEW, "Waitng");
        acase 5:  zprintf(TEXTPEN_VIEW, "SRB OK");
        acase 6:  zprintf(TEXTPEN_VIEW, "Suspnd");
        acase 7:  zprintf(TEXTPEN_VIEW, "Abortg");
        acase 8:  zprintf(TEXTPEN_VIEW, "Paused");
        adefault: zprintf(TEXTPEN_VIEW, "?     ");
        }
        zprintf
        (   TEXTPEN_VIEW,
            "  $%02X  $%02X  $%02X  $%02X     %3d $%02X  %3d %3d $%02X%02X $%02X%02X $%02X%02X \n",
            memory[0x2058 +  i     ], // assigned channels
            memory[0x205C +  i     ], // active channels
            memory[0x2060 +  i     ], // waiting for channels
            memory[0x2064 +  i     ], // complete channels
            memory[0x2068 +  i     ], // waiting for job #
            memory[0x206C +  i     ], // command vector table index
            memory[0x2070 +  i     ], // bank
            memory[0x2074 +  i     ], // module start offset
            memory[0x2078 + (i * 2)], // module start address high byte
            memory[0x2079 + (i * 2)], // module start address low  byte
            memory[0x2080 + (i * 2)], // SRB return address high byte
            memory[0x2081 + (i * 2)], // SRB return address high byte
            memory[0x2088 + (i * 2)], // job wait return address high byte
            memory[0x2089 + (i * 2)]  // job wait return address low  byte
        );
    }

    zprintf
    (   TEXTPEN_VIEW,
        "\n     ----File index per channel-----  ---Device index per channel----\n" \
        "Job  Ch0 Ch1 Ch2 Ch3 Ch4 Ch5 Ch6 Ch7  Ch0 Ch1 Ch2 Ch3 Ch4 Ch5 Ch6 Ch7\n" \
        "---  --- --- --- --- --- --- --- ---  --- --- --- --- --- --- --- ---\n"
    );

    for (i = 0; i < 4; i++) // job number
    {   zprintf
        (   TEXTPEN_VIEW,
            "  %d  %3d %3d %3d %3d %3d %3d %3d %3d  %3d %3d %3d %3d %3d %3d %3d %3d\n",
            i,
            memory[0x2090 + i],
            memory[0x2094 + i],
            memory[0x2098 + i],
            memory[0x209C + i],
            memory[0x20A0 + i],
            memory[0x20A4 + i],
            memory[0x20A8 + i],
            memory[0x20AC + i],
            memory[0x20B0 + i],
            memory[0x20B4 + i],
            memory[0x20B8 + i],
            memory[0x20BC + i],
            memory[0x20C0 + i],
            memory[0x20C4 + i],
            memory[0x20C8 + i],
            memory[0x20CC + i]
        );
    }

    // FCB (always $2150..$2345)

    zprintf
    (   TEXTPEN_VIEW,
        "\nFile handles ($2150..$2345):\n" \
        "Fil Type FStat Job Bank Chan SVC DStat OStat Done BfAdr SRBAd SRBRt FilRt Cmd\n" \
        "--- ---- ----- --- ---- ---- --- ----- ----- ---- ----- ----- ----- ----- ---\n"
    );

    for (i = 0; i < 22; i++)
    {   zprintf
        (   TEXTPEN_VIEW,
            " %2d ",
            i
        );
        if (i < 8)
        {   zprintf(TEXTPEN_VIEW, "Real ");
        } else
        {   zprintf(TEXTPEN_VIEW, "Virt ");
        }
        switch (memory[0x2150]) // file status
        {
        case  0:  zprintf(TEXTPEN_VIEW, "Free ");
        acase 1:  zprintf(TEXTPEN_VIEW, "Asgnd");
        acase 2:  zprintf(TEXTPEN_VIEW, "Open ");
        acase 3:  zprintf(TEXTPEN_VIEW, "Busy ");
        acase 4:  zprintf(TEXTPEN_VIEW, "Ready");
        acase 5:  zprintf(TEXTPEN_VIEW, "EOF/D");
        acase 6:  zprintf(TEXTPEN_VIEW, "Abort");
        adefault: zprintf(TEXTPEN_VIEW, "?    ");
        }
        zprintf
        (   TEXTPEN_VIEW,
            " %3d%c %3d  %3d $%02X   $%02X   $%02X  %3d $%02X%02X $%02X%02X $%02X%02X $%02X%02X $%02X \n",
             memory[0x2166 +  i     ], // job
            (memory[0x2166 +  i     ] <= 3) ? ' ' : '!',
             memory[0x217C +  i     ], // bank
             memory[0x2192 +  i     ], // channel
             memory[0x21A8 +  i     ], // SVC
             memory[0x21BE +  i     ], // device status
             memory[0x21D4 +  i     ], // I/O operation status
             memory[0x21EA +  i     ], // done
             memory[0x2200 + (i * 2)], // buffer high byte
             memory[0x2201 + (i * 2)], // buffer low  byte
             memory[0x222C + (i * 2)], // SRB ad high byte
             memory[0x222D + (i * 2)], // SRB ad low  byte
             memory[0x2258 + (i * 2)], // SRB rt high byte
             memory[0x2259 + (i * 2)], // SRB rt low  byte
             memory[0x2284 + (i * 2)], // file return addr high byte
             memory[0x2285 + (i * 2)], // file return addr low  byte
             memory[0x22B0 +  i     ]  // file command
        );
    }

    zprintf
    (   TEXTPEN_VIEW,
        "\nFil Drv Sec Trk IOBuf Seq BytToDo LenSec SecToDo LenLst BlkByt BlkBit Bitmap\n" \
        "--- --- --- --- ----- --- ------- ------ ------- ------ ------ ------ ------\n"
    );

    for (i = 0; i < 8; i++)
    {   zprintf
        (   TEXTPEN_VIEW,
            " %2d %3d %3d %3d $%02X%02X %3d     %3d  $%02X%02X   $%02X%02X    %3d    %3d    %3d  $%02X%02X\n",
            i,
            memory[0x22C6 +  i     ], // drive number
            memory[0x22CE +  i     ], // sector
            memory[0x22D6 +  i     ], // track
            memory[0x22DE + (i * 2)], // I/O buffer high byte
            memory[0x22DF + (i * 2)], // I/O buffer low  byte
            memory[0x22EE +  i     ], // sequence number
            memory[0x22F6 +  i     ], // to do
            memory[0x22FE + (i * 2)], // length in sectors high byte
            memory[0x22FF + (i * 2)], // length in sectors low  byte
            memory[0x230E + (i * 2)], // sectors to do high byte
            memory[0x230F + (i * 2)], // sectors to do low  byte
            memory[0x231E +  i     ], // length of last sector
            memory[0x2326 +  i     ], // block byte
            memory[0x232E +  i     ], // block bit
            memory[0x2336 + (i * 2)], // bitmap high byte
            memory[0x2337 + (i * 2)]  // bitmap low  byte
        );
    }

    switch (twin_dosver) // DCB
    {
    case  TWIN_EXOS:   localoffset = 0x235C;
    acase TWIN_TOS:    
    case  TWIN_SDOS20:
    case  TWIN_SDOS40:
    case  TWIN_SDOS42: 
    case  TWIN_UDOS:   localoffset = 0x2346; // $2346..$24A2 but we don't show $24A2
    }

    zprintf
    (   TEXTPEN_VIEW,
        "\nDevices ($%X..$%X):\n" \
        "Dev Rdyns Sta.Done ToDo Ech Actv SVC Bnk IOAdr RgAdr DID Avl Typ HnAdr  Name\n" \
        "--- ----- --- ---- ---- --- ---- --- --- ----- ----- --- --- --- ----- ------\n",
        localoffset,
        localoffset + 347
    );

    for (i = 0; i < 16; i++)
    {   zprintf
        (   TEXTPEN_VIEW,
            " %2d ",
            i
        );
        switch (memory[localoffset + i]) // readiness
        {
        case  0: zprintf(TEXTPEN_VIEW, "Ready");
        acase 1: zprintf(TEXTPEN_VIEW, "Busy ");
        acase 2: zprintf(TEXTPEN_VIEW, "Down ");
        acase 3: zprintf(TEXTPEN_VIEW, "?    ");
        }
        zprintf
        (   TEXTPEN_VIEW,
            " $%02X  %3d  %3d %3d  %3d $%02X $%02X $%02X%02X $%02X%02X $%02X $%02X $%02X $%02X%02X ",
            memory[localoffset + 0x10 +  i     ], // status
            memory[localoffset + 0x20 +  i     ], // done
            memory[localoffset + 0x30 +  i     ], // to do
            memory[localoffset + 0x40 +  i     ], // echo count
            memory[localoffset + 0x50 +  i     ], // active file index
            memory[localoffset + 0x60 +  i     ], // SVC function code
            memory[localoffset + 0x70 +  i     ], // bank
            memory[localoffset + 0x80 + (i * 2)], // I/O address high byte         ($23C6.
            memory[localoffset + 0x81 + (i * 2)], // I/O address low  byte                .$23E5)
            memory[localoffset + 0xA0 + (i * 2)], // ring buffer address high byte ($23E6.
            memory[localoffset + 0xA1 + (i * 2)], // ring buffer address low  byte        .$2405)
            memory[localoffset + 0xC0 +  i     ], // device ID
            memory[localoffset + 0xD0 +  i     ], // availability
            memory[localoffset + 0xE0 +  i     ], // type
         // memory[localoffset + 0xF0 + (i * 3)], // handler address opcode        ($2436
            memory[localoffset + 0xF1 + (i * 3)], // handler address low  byte           ..
            memory[localoffset + 0xF2 + (i * 3)]  // handler address high byte             $2465)
        );
        if (i == 15)
        {   zprintf(TEXTPEN_VIEW, "(Disk)\n");
        } else
        {   zprintf
            (   TEXTPEN_VIEW,
                "\"%c%c%c%c\"\n",
                memory[localoffset + 0x120 + (i * 4)], // name 1st character        ($2466
                memory[localoffset + 0x121 + (i * 4)], // name 2nd character              .
                memory[localoffset + 0x122 + (i * 4)], // name 3rd character               .
                memory[localoffset + 0x123 + (i * 4)]  // name 4th character                $24A1)
            );
    }   }

    switch (twin_dosver) // DskCB
    {
    case  TWIN_EXOS:   localoffset = 0x24B9;
    acase TWIN_TOS:    
    case  TWIN_SDOS20:
    case  TWIN_UDOS:   localoffset = 0x24A3; // $24A3..$24D2
    acase TWIN_SDOS40:
    case  TWIN_SDOS42: localoffset = 0x24B7;
    }

    zprintf
    (   TEXTPEN_VIEW,
        "\nFloppy disk drives ($%X..$%X):\n" \
        "Drive Status  Files    ID Bitmap\n" \
        "----- ------- ----- ----- ------\n",
        localoffset,
        localoffset + 47
    );

    for (i = 0; i < 8; i++) // drive number
    {   zprintf
        (   TEXTPEN_VIEW,
            "    %d ",
            i
        );
        switch (memory[localoffset + i])
        {
        case  0:  zprintf(TEXTPEN_VIEW, "Invalid");
        acase 1:  zprintf(TEXTPEN_VIEW, "Valid  ");
        acase 2:  zprintf(TEXTPEN_VIEW, "Unavail");
        adefault: zprintf(TEXTPEN_VIEW, "?      ");
        }
        zprintf
        (   TEXTPEN_VIEW,
            "   %3d %5d  $%02X%02X\n",
             memory[localoffset +    8 +  i     ], // number of files
            (memory[localoffset + 0x10 + (i * 2)] * 256)
          +  memory[localoffset + 0x11 + (i * 2)], // not sure of the format
             memory[localoffset + 0x20 + (i * 2)], // bitmap high byte
             memory[localoffset + 0x21 + (i * 2)]  // bitmap low  byte
        );
    }

    zprintf(TEXTPEN_VIEW, "\n");
}
