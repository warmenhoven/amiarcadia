// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
    #include <string.h>            // for eg.memset()
    #include <stdlib.h>            // for eg. EXIT_FAILURE
#endif
#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
#endif

#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"
#ifdef AMIGA
    #ifdef __amigaos4__
        #include <classes/requester.h> // REQIMAGE_QUESTION
    #endif
    #include <proto/locale.h>      // GetCatalogStr()
#endif

#include <stdio.h>
#include <ctype.h>                 // toupper()

#include "aa.h"
#include "cd2650.h"

// DEFINES----------------------------------------------------------------

// #define CD2650_RAWSCREEN
// enable this for cd2650_viewscreen() to show the screen from the
// machine's "viewpoint" (ie. a column at a time), instead of from the
// user's (ie. a row at a time).

#define DOCPU                 \
if (cpux == nextinst)         \
{   oldcycles = cycles_2650;  \
    checkstep();              \
    do_tape();                \
    one_instruction();        \
    nextinst += (cycles_2650 - oldcycles) * (fastcd2650 ? 8 : 12); \
    if (nextinst >= 904)      \
    {   nextinst -= 904;      \
}   }

#define CD2650_FD1771_COMMAND  0x80 // used for command AND status!
#define CD2650_FD1771_STATUS   0x80 // used for command AND status!
#define CD2650_FD1771_TRACK    0x81
#define CD2650_FD1771_SECTOR   0x82
#define CD2650_FD1771_DATA     0x83
#define CD2650_FD1771_CONTROL  0x84 // bit 6 is INTREQ

// MODULE VARIABLES-------------------------------------------------------

MODULE       int                  usedsize;

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       FLAG                 capslock,
                                  inframe,
                                  multimode,
                                  yank;
IMPORT       TEXT                 asciiname_short[259][3 + 1],
                                  gtempstring[256 + 1],
                                  path_disks[MAX_PATH + 1];
IMPORT       ASCREEN              screen[BOXWIDTH][BOXHEIGHT];
IMPORT       UBYTE                cosversion,
                                  drivestatus,
                                  guestkey,
                                  startaddr_h,
                                  startaddr_l,
                                  memory[32768],
                                  opcode,
                                  r[7],
                                  reqsector,
                                  tone,
                                  trackreg,
                                  vdu_bgc,
                                  vdu_fgc;
IMPORT       UBYTE*               IOBuffer;
IMPORT       UWORD                hostkey,
                                  iar,
                                  mirror_r[32768],
                                  mirror_w[32768],
                                  oldiar;
IMPORT       ULONG                curdrive,
                                  cycles_2650,
                                  ea,
                                  frames,
                                  oldcycles,
                                  timeoutat,
                                  verbosedisk;
IMPORT       int                  ambient,
                                  belling[2],
                                  cpux,
                                  cpuy,
                                  debugdrive,
                                  drawmode,
                                  drive_mode,
                                  editscreen,
                                  framebased,
                                  game,
                                  keymap,
                                  language,
                                  log_illegal,
                                  logmonitor,
                                  lowercase,
                                  machine,
                                  memmap,
                                  n1,
                                  nextinst,
                                  queuekeystrokes,
                                  queuepos,
                                  post,
                                  region,
                                  scrnaddr,
                                  serializemode,
                                  slice_2650,
                                  stepdir,
                                  tapemode,
                                  trace,
                                  verbosetape,
                                  watchreads,
                                  watchwrites,
                                  whichgame;
IMPORT       MEMFLAG              memflags[ALLTOKENS];
IMPORT       struct DriveStruct   drive[DRIVES_MAX];
IMPORT       struct MachineStruct machines[MACHINES];
IMPORT       struct PrinterStruct printer[2];
IMPORT       struct RTCStruct     rtc;
IMPORT const struct CodeCommentStruct codecomment[];

#ifdef AMIGA
    IMPORT   struct Catalog*      CatalogPtr;
    IMPORT   struct Window*       SubWindowPtr[SUBWINDOWS];
#endif
#ifdef WIN32
    IMPORT   int                  CatalogPtr; // APTR doesn't work
    IMPORT   struct LangStruct    langs[LANGUAGES];
    IMPORT   HWND                 MainWindowPtr,
                                  SubWindowPtr[SUBWINDOWS];
#endif

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       FLAG                 sectordone;
EXPORT       UBYTE                cd2650_chars_bmp[128][10];
EXPORT       ULONG                cd2650_downtill;
EXPORT       int                  cd2650_biosver    = CD2650_SUPERVISOR,
                                  cd2650_dosver     = DOS_CDDOS,
                                  cd2650_userdrives = 4,
                                  cd2650_vdu        = VDU_LOWERCASE,
                                  fastcd2650        = FALSE,
                                  viewingdrive      = 0;

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void cd2650_runcpu(void);
MODULE void draw_cd2650(void);
MODULE void run_cpu(int until);

// CODE-------------------------------------------------------------------

EXPORT void cd2650_viewscreen(void)
{   int   x, y;
    TEXT  rowchars[80 + 1];
#ifndef CD2650_RAWSCREEN
    int   i;
#endif

    rowchars[16] = EOS;

#ifdef CD2650_RAWSCREEN
    for (x = 0; x < 80; x++)
    {   for (y = 0; y < 16; y++)
        {   rowchars[y] = cd2650_chars[memory[0x1000 + y + (x * 16)] & 0x7F];
        }
        zprintf
        (   TEXTPEN_VIEW,
            "$%04X: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %s\n",
            (unsigned int) (0x1000 + (x * 16)),
            memory[0x1000 + (x * 16)],
            memory[0x1001 + (x * 16)],
            memory[0x1002 + (x * 16)],
            memory[0x1003 + (x * 16)],
            memory[0x1004 + (x * 16)],
            memory[0x1005 + (x * 16)],
            memory[0x1006 + (x * 16)],
            memory[0x1007 + (x * 16)],
            memory[0x1008 + (x * 16)],
            memory[0x1009 + (x * 16)],
            memory[0x100A + (x * 16)],
            memory[0x100B + (x * 16)],
            memory[0x100C + (x * 16)],
            memory[0x100D + (x * 16)],
            memory[0x100E + (x * 16)],
            memory[0x100F + (x * 16)],
            rowchars
        );
    }
    zprintf(TEXTPEN_VIEW, "\n");
#else
    for (i = 0; i < 5; i++)
    {   for (y = 0; y < 16; y++)
        {   for (x = 0; x < 16; x++)
            {   rowchars[x] = cd2650_chars[memory[0x1000 + y + (i * 256) + (x * 16)] & 0x7F];
            }
            zprintf
            (   TEXTPEN_VIEW,
                "$%04X: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %s\n",
                (unsigned int) (0x1000 + (i * 256) + y),
                memory[0x1000 + (i * 256) + y],
                memory[0x1010 + (i * 256) + y],
                memory[0x1020 + (i * 256) + y],
                memory[0x1030 + (i * 256) + y],
                memory[0x1040 + (i * 256) + y],
                memory[0x1050 + (i * 256) + y],
                memory[0x1060 + (i * 256) + y],
                memory[0x1070 + (i * 256) + y],
                memory[0x1080 + (i * 256) + y],
                memory[0x1090 + (i * 256) + y],
                memory[0x10A0 + (i * 256) + y],
                memory[0x10B0 + (i * 256) + y],
                memory[0x10C0 + (i * 256) + y],
                memory[0x10D0 + (i * 256) + y],
                memory[0x10E0 + (i * 256) + y],
                memory[0x10F0 + (i * 256) + y],
                rowchars
            );
        }
        zprintf(TEXTPEN_VIEW, "\n");
    }
#endif
}

EXPORT void cd2650_setmemmap(void)
{   int address,
        biosend = 0, // initialized to avoid a spurious SAS/C warning
        i, j;

    game = FALSE;
    machines[CD2650].cpf = fastcd2650 ? 29832.0 : 19888.0;
    nextinst = 0;

    switch (cd2650_biosver)
    {
    case CD2650_IPL:
        for (i =     0; i <=  0x179; i++) memory[i] = ipl[i];
        biosend = 0x179;
        for (i = 0x17A; i <=  0xFFF; i++) memory[i] = 0;
        if (cd2650_dosver == DOS_P1DOS)
        {   machines[CD2650].firstequiv   = FIRSTP1DOSEQUIV;
            machines[CD2650].lastequiv    = LASTP1DOSEQUIV;
        } else
        {   machines[CD2650].firstequiv   =
            machines[CD2650].lastequiv    = -1;
        }
        machines[CD2650].firstcodecomment =
        machines[CD2650].lastcodecomment  =
        machines[CD2650].firstdatacomment =
        machines[CD2650].lastdatacomment  = -1;
        cd2650_vdu = VDU_ASCII;
    acase CD2650_POPMON:
        for (i =     0; i <=  0xBFF; i++) memory[i] = popmon_bios[i];
        biosend = 0xBFF;
        for (i = 0xC00; i <=  0xFFF; i++) memory[i] = 0;
        machines[CD2650].firstequiv       = FIRSTPOPMONEQUIV;
        machines[CD2650].lastequiv        = LASTPOPMONEQUIV;
        machines[CD2650].firstcodecomment =
        machines[CD2650].lastcodecomment  =
        machines[CD2650].firstdatacomment =
        machines[CD2650].lastdatacomment  = -1;
    acase CD2650_SUPERVISOR:
        for (i =     0; i <=  0x3FE; i++) memory[i] = cd2650_bios[i];
        biosend = 0x3FE;
        for (i = 0x3FF; i <=  0xFFF; i++) memory[i] = 0;
        machines[CD2650].firstequiv       = FIRSTCD2650EQUIV;
        machines[CD2650].lastequiv        = LASTCD2650EQUIV;
        machines[CD2650].firstcodecomment = 528;
        machines[CD2650].lastcodecomment  = 920;
        machines[CD2650].firstdatacomment = 121;
        machines[CD2650].lastdatacomment  = 130;
    }
    for (i = 0x1000; i <= 0x14FF; i++) memory[i] = 0x20;
    for (i = 0x1500; i <= 0x5FFF; i++) memory[i] = 0;
    if (!post && cd2650_biosver == CD2650_IPL && cd2650_dosver != DOS_NOCD2650DOS)
    {   for (i = 0x6000; i <= 0x7AFF; i++)
        {   memory[i] = cd2650_dos[cd2650_dosver][i - 0x6000];
        }
        iar = 0x6000;
    } else
    {   for (i = 0x6000; i <= 0x7AFF; i++)
        {   memory[i] = 0;
    }   }
    for (i = 0x7B00; i <= 0x7FFF; i++) memory[i] = 0;

    for (address = 0; address < 32 * KILOBYTE; address++)
    {   memflags[address] = 0;
        if     (address <=  0xFFF)
        {   memflags[address] |= NOWRITE;
        } elif (address <= 0x14FF)
        {   memflags[address] |= VISIBLE;
        } elif (address >= 0x1800 && address <= 0x1FFF)
        {   memflags[address] |= NOREAD | NOWRITE;
        }

        mirror_r[address] =
        mirror_w[address] = (UWORD) address;
    }

    for (i = machines[machine].firstcodecomment; i <= machines[machine].lastcodecomment; i++)
    {   memflags[codecomment[i].address] |= COMMENTED;
    }
    for (i = 0; i <= biosend; i++)
    {   memflags[i] |= BIOS;
    }
    if (cd2650_dosver == DOS_P1DOS)
    {   for (i = 0x6000; i <= 0x7AFF; i++)
        {   memflags[i] |= BIOS;
    }   }

    cd2650_updatecharset();

    curdrive            = 0;
    if (debugdrive >= machines[machine].drives)
    {   debugdrive = machines[machine].drives - 1;
    }

    stepdir             = 0;
    drive_mode          = DRIVEMODE_IDLE;
    sectordone          = FALSE;
    timeoutat           = (ULONG) -1;
    for (i = 0; i < machines[machine].drives; i++)
    {   for (j = 0; j < CD2650_DISKSIZE / 8; j++)
        {   drive[i].flags[j] = 0;
        }
        drive[i].track       =
        drive[i].blockoffset = 0;
        drive[i].sector      = 1;
        drive[i].spinning    =
        drive[i].inserted    = FALSE;
    }
    trackreg = 0;

    if (cd2650_biosver == CD2650_IPL)
    {   cd2650_create_disk(0); // because it won't even boot without DOS available on disk
}   }

EXPORT void cd2650_emulate(void)
{   FAST UBYTE t;
    FAST int   kx, x, x1, x2,
               ky, y, y1, y2;

    inframe = TRUE;

    if (framebased)
    {   slice_2650 += fastcd2650 ? 29832 : 19888;
        cd2650_runcpu();
        draw_cd2650();
    } else
    {   for (cpuy = 0; cpuy <= 191; cpuy++)
        {   breakrastline();
            y1 = cpuy / 12;
            y2 = cpuy % 12;
            if (y2 >= 10)
            {   for (cpux = 0; cpux <= 639; cpux++)
                {   changebgpixel(cpux, cpuy, vdu_bgc);
                    DOCPU;
                }
                run_cpu(904);
            } else
            {   for (cpux = 0; cpux <= 639; cpux++)
                {   x1 = cpux /  8;
                    x2 = cpux %  8;
#ifdef SHOWCHARSET
                    if (x1 < 16 && y1 < 16) t = (y1 * 16) + x1; else
#endif
                    t  = memory[0x1000 + (x1 * 16) + y1] & 0x7F;
                    if (cd2650_chars_bmp[t][y2] & (0x80 >> x2))
                    {   changepixel(cpux, cpuy, vdu_fgc);
                    } else
                    {   changebgpixel(cpux, cpuy, vdu_bgc);
                    }
                    DOCPU;
                }
                run_cpu(904);
        }   }

        for (cpuy = 192; cpuy <= 263; cpuy++)
        {   breakrastline();
            run_cpu(904);
    }   }

    if (editscreen)
    {   kx = (scrnaddr - 0x1000) / 16;
        ky =  scrnaddr           % 16;
        x1 = (kx *  8) -  1;
        x2 = (kx *  8) +  8;
        y1 = (ky * 12) -  1;
        y2 = (ky * 12) + 12;
        for (x = x1; x <= x2; x++)
        {   for (y = y1; y <= y2; y++)
            {   if
                (   (x == x1 || x == x2 || y == y1 || y == y2)
                 && x >= 0
                 && x < 640
                 && y >= 0
                 && y < 192
                )
                {   changepixel(x, y, RED);
    }   }   }   }

    fd1771_spindown();
    if (drawmode)
    {   cd2650_drawhelpgrid();
    }
    wa_checkinput();
    endofframe(vdu_bgc);
}

EXPORT void cd2650_writeport(int port, UBYTE data)
{   FAST int   oldmode,
               oldoffset;
    FAST UBYTE oldtrack,
               oldsector,
               oldstatus;

    // assert(machine == CD2650);

    oldtrack  = drive[curdrive].track;
    oldsector = drive[curdrive].sector;
    oldstatus = drivestatus;
    oldmode   = drive_mode;
    oldoffset = drive[curdrive].blockoffset;

    switch (port)
    {
/*  case PORTC:
        // BIOS writes:
        //  %00000000 for tape on
        //  %00001000 for tape off
        if (data == 8)
        {   // start tape
        } elif (data == 0)
        {   // stop tape
        }
    break; */
    case 6:
        play_bell(0); // Star Trek uses this port for sound
        // ideally this should be done at the correct volume, and only after a "slight delay" (20msec?)
        // and should use an alarm (not bell) sound
    acase CD2650_FD1771_COMMAND:
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
        acase 0x80: // writing $D0 to port $80 (CD2650_FD1771_CMD) means "cancel any commands and disable all interrupts"
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
    acase CD2650_FD1771_TRACK:
        fd1771_write_trackreg(data);
    acase CD2650_FD1771_SECTOR:
        fd1771_write_reqsector(data);
    acase CD2650_FD1771_DATA: // $83
        fd1771_write_datareg(data);
    acase CD2650_FD1771_CONTROL: // $84
        if (data < cd2650_userdrives)
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
                    {   zprintf(TEXTPEN_VERBOSE, "Spun down drive #%d due to drive change.\n", curdrive);
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
    acase 0xFC:
    case 0xFD:
        euy_printchar(data, TRUE);
    acase PORTD:
        if (whichgame == CD2650_MORSEPOS)
        {   // Morse Code writes:
            //  %00001111 for tone on
            //  %00000111 for tone off
            if (data == 0x0F)
            {   tone = 1;
            } else
            {   tone = 0;
            }
            playsound(FALSE);
        } else
        {   if (data & 8)
            {   if (verbosetape)
                {   zprintf(TEXTPEN_VERBOSE, "Tape motor has been turned on.\n");
                }
                if (tapemode == TAPEMODE_STOP)
                {   if (iar == 0x1FD) // Dump
                    {   tape_record();
                    } elif (iar == 0x28D) // Load/Verify
                    {   tape_play();
                    } else
                    {
#ifdef WIN32
                        if (MessageBoxA
                        (   MainWindowPtr,
                            LLL(MSG_RECORDORPLAY3, "Record (otherwise play)?"),
                            LLL(MSG_QUESTION     , "Question"                ),
                            MB_YESNO | MB_ICONQUESTION
                        ) == IDYES)
#endif
#ifdef AMIGA
                        if (easyrequest
                        (   LLL(MSG_QUESTION     , "Question"                ),
                            LLL(MSG_RECORDORPLAY1, "Record or play?"         ),
                            LLL(MSG_RECORDORPLAY2, "Record|Play"             ), // underscores don't work here
                            REQIMAGE_QUESTION
                        ) == 1) // 0 means right (play), 1 means left (record)
#endif
                        {   tape_record();
                        } else
                        {   tape_play();
            }   }   }   }
            else
            {   if (verbosetape)
                {   zprintf(TEXTPEN_VERBOSE, "Tape motor has been turned off.\n");
                }
                if (tapemode > TAPEMODE_STOP)
                {   tape_stop();
    }   }   }   }

    logport(port, data, TRUE);

    if
    (   drive[curdrive].track  != oldtrack
     || drive[curdrive].sector != oldsector
    )
    {   if (viewingdrive != (int) curdrive)
        {   viewingdrive = curdrive;
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
        {   viewingdrive = curdrive;
            update_floppydrive(3, viewingdrive);
        } else
        {   update_floppydrive(1, viewingdrive);
}   }   }

EXPORT UBYTE cd2650_readport(int port)
{   FAST      int   oldmode,
                    oldoffset;
    FAST      UBYTE oldtrack,
                    oldsector,
                    oldstatus,
                    t;

    // assert(machine == CD2650);

    oldtrack  = drive[curdrive].track;
    oldsector = drive[curdrive].sector;
    oldstatus = drivestatus;
    oldmode   = drive_mode;
    oldoffset = drive[curdrive].blockoffset;

    switch (port)
    {
    case PORTD:
        if (queuekeystrokes)
        {   t = get_guest_key();
            if (t != NC)
            {   cd2650_downtill = cycles_2650 + ((ULONG) machines[CD2650].cpf / 2);
        }   }
        else
        {   t = guestkey;
            guestkey = NC; // fixes CDChess.aof. High bit must be set
        }
    acase CD2650_FD1771_STATUS:
        t = fd1771_read_status();
    acase CD2650_FD1771_TRACK:
        t = trackreg;
        if (verbosedisk)
        {   zprintf(TEXTPEN_LOG, "Code at $%X read %d from track register.\n", iar, t);
        }
    acase CD2650_FD1771_SECTOR:
        t = drive[curdrive].sector;
        if (verbosedisk)
        {   zprintf(TEXTPEN_LOG, "Code at $%X read %d from sector register.\n", iar, t);
        }
    acase CD2650_FD1771_DATA:
        t = fd1771_read_datareg();
    acase CD2650_FD1771_CONTROL: // $84
        t = fd1771_read_controlreg();
    acase 0xFC: // printer
        t = 0;
    acase 0xFD: // printer
        t = 2;
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
        {   viewingdrive = curdrive;
            update_floppydrive(3, viewingdrive);
        } else
        {   update_floppydrive(1, viewingdrive);
    }   }

    return t;
}

EXPORT void cd2650_biosdetails1(int ea)
{   // assert(machine == CD2650);

    if (cd2650_biosver == CD2650_SUPERVISOR)
    {   switch (ea)
        {
        case     0x0: /* INIT */ zprintf(TEXTPEN_LOG, ";Comments: R0, R1, R2 destroyed\n");
        acase   0x24: /* LFCR */ zprintf(TEXTPEN_LOG, ";Comments: R0, R1, R3 destroyed\n");
        acase   0x83: /* RETU */ zprintf(TEXTPEN_LOG, ";Comments: Have your program branch here if you want to return to the supervisor\n");
        acase   0xAC: /* BPSV */ zprintf(TEXTPEN_LOG, ";Function: Breakpoint service routine\n");
        acase   0xBB: /* CLRB */ zprintf(TEXTPEN_LOG, ";Function: Entry point for clear\n");
        acase   0xDB: /* WRAD */ zprintf(TEXTPEN_LOG, ";Comments: All regs destroyed\n");
        acase  0x122: /* INSP */ zprintf(TEXTPEN_LOG, ";Function: Save all reg values in memory\n");
        acase  0x185: /* ADDR */ zprintf(TEXTPEN_LOG, ";Comments: Address placed in $17FA..$17FB. R0, R1, R3 destroyed\n");
        acase  0x19A: /* TABL */ zprintf(TEXTPEN_LOG, ";Comments: Byte starts and ends in R3. R0 destroyed\n");
        acase  0x1B6: /* INHX */ zprintf(TEXTPEN_LOG, ";Comments: Assembled byte in R3. R0, R1 destroyed\n");
        acase  0x24F: /* SERO */ zprintf(TEXTPEN_LOG, ";Comments: R0, R2 destroyed\n");
        acase  0x37C: /* ERRR */ zprintf(TEXTPEN_LOG, ";Function: Write \"ERROR\"\n");
        acase  0x3C0: /* COMD */ zprintf(TEXTPEN_LOG, ";Comments: R3 destroyed\n");
}   }   }

EXPORT void cd2650_biosdetails2(int ea)
{   // assert(machine == CD2650);

    if (cd2650_dosver == DOS_P1DOS)
    {   switch (ea)
        {
        case  0x6004: zprintf(TEXTPEN_LOG, "Function:  Terminate the program and return to DOS\n"\
        /* DOS_Command                  */ "Category:  Error handling\n"\
                                           "Type:      Code section\n"\
                                           "Input:     None\n"\
                                           "Result:    None (does not return)\n");
        acase 0x6006: zprintf(TEXTPEN_LOG, "Function:  Terminate the program with \"Invalid command\"\n"\
        /* DOS_InvalidCmdRetry          */ "           and return to DOS\n"\
                                           "Category:  Error handling\n"\
                                           "Type:      Code section\n"\
                                           "Input:     None\n"\
                                           "Result:    None (does not return)\n");
        acase 0x6008: zprintf(TEXTPEN_LOG, "Function:  Read a filename\n"\
        /* DOS_ParseFilename            */ "Category:  Command line\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     CmdLinePtr = pointer to position in CmdLine\n"\
                                           "           CmdLine = command line string\n"\
                                           "Result:    CC = EQ on success, CC = LT on end-of-line, CC = GT on error\n"\
                                           "           ParsedFilename/ParsedType/FileUnit = full filename\n"\
                                           "Preserves: None\n");
        acase 0x600A: zprintf(TEXTPEN_LOG, "Function:  Read a (hexa)decimal number\n"\
        /* DOS_ParseNumber              */ "Category:  Command line\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     CmdLinePtr = pointer to position in CmdLine\n"\
                                           "           CmdLine = command line string\n"\
                                           "Result:    CC = EQ on success, CC = LT on end-of-line, CC = GT on error\n"\
                                           "           R0 = high byte of value of the string\n"\
                                           "           R1 = low  byte of value of the string\n");
        acase 0x600C: zprintf(TEXTPEN_LOG, "Function:  Read any single letter\n"\
        /* DOS_ParseNextCmdChar         */ "Category:  Command line\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     CmdLinePtr = pointer to position in CmdLine\n"\
                                           "           CmdLine = command line, zero-padded\n"\
                                           "Result:    R0 = the read character, or zero if end-of-line\n"\
                                           "Preserves: R1, R3\n");
        acase 0x600E: zprintf(TEXTPEN_LOG, "Function:  Copy file information into the Open Files Block\n"\
        /* DOS_OpenFileByName           */ "Category:  File manipulation\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     ParsedFilename/ParsedUnit/FileUnit = name of the file\n"\
                                           "Result:    CC = EQ on success, CC = LT if the file is not present in\n"\
                                           "           the Open Files Block\n"\
                                           "           R1 = error code, on error\n");
        acase 0x6010: zprintf(TEXTPEN_LOG, "Function:  Remove file information from the Open Files Block\n"\
        /* DOS_CloseFileByName          */ "Category:  File manipulation\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     ParsedFilename/ParsedUnit/FileUnit = name of the file\n"\
                                           "Result:    CC = EQ on success, CC = LT if the file is not present in the\n"\
                                           "           the Open Files Block\n");
        acase 0x6012: zprintf(TEXTPEN_LOG, "Function:  Test whether the current user has change permission on a file\n"\
        /* DOS_CkChangeCodeByFP         */ "Category:  Access permissions\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     OpenFilePtr\n"\
                                           "Result:    CC = EQ on success, CC = GT on failure (access denied)\n"\
                                           "           R1 = error code, on error\n");
        acase 0x6014: zprintf(TEXTPEN_LOG, "Function:  Test whether the current user has inspect permission on a file\n"\
        /* DOS_CkInspectCodeByFP        */ "Category:  Access permissions\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     OpenFilePtr\n"\
                                           "Result:    CC = EQ on success, CC = GT on failure (access denied)\n"\
                                           "           R1 = error code, on error\n");
        acase 0x6016: zprintf(TEXTPEN_LOG, "Function:  Test whether the current user has change permission on a file,\n"\
        /* DOS_QyChangeCodeByFP         */ "           asking for the codeword if necessary\n"\
                                           "Category:  Access permissions\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     OpenFilePtr\n"\
                                           "Result:    CC = EQ on success, CC = GT on failure (access denied)\n"\
                                           "           R1 = error code, on error\n");
        acase 0x6018: zprintf(TEXTPEN_LOG, "Function:  Test whether the current user has inspect permission on a file,\n"\
        /* DOS_QyInspectCodeByFP        */ "           asking for the codeword if necessary\n"\
                                           "Category:  Access permissions\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     OpenFilePtr\n"\
                                           "Result:    CC = EQ on success, CC = GT on failure (access denied)\n"\
                                           "           R1 = error code, on error\n");
        acase 0x601A: zprintf(TEXTPEN_LOG, "Function:  Create a new file and allocate space to it\n"\
        /* DOS_AllocByName              */ "Category:  File manipulation\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     R0 ($%02X) = high byte of the number of blocks\n"\
                                           "           R1 ($%02X) = low  byte of the number of blocks\n"\
                                           "           ParsedFilename/ParsedUnit/FileUnit = name of the new file\n"\
                                           "Result:    CC = EQ on success, CC = GT on error\n"\
                                           "           R1 = error code, on error\n", r[0], r[1]);
        acase 0x601C: zprintf(TEXTPEN_LOG, "Function:  Reduce the size of a file\n"\
        /* DOS_DeallocByName            */ "Category:  File manipulation\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     R0 ($%02X) = high byte of the new size (number of blocks)\n"\
                                           "           R1 ($%02X) = low  byte of the new size (number of blocks)\n"\
                                           "           ParsedFilename/ParsedUnit/FileUnit = name of the file\n"\
                                           "Result:    CC = EQ on success, CC = GT on error\n"\
                                           "           R1 = error code, on error\n", r[0], r[1]);
        acase 0x601E: zprintf(TEXTPEN_LOG, "Function:  Read or write from a file\n"\
        /* DOS_RWByName                 */ "Category:  File manipulation\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     ParsedFilename/ParsedUnit/FileUnit = name of the file\n"\
                                           "           TargetAddr = address to read from or write into\n"\
                                           "           R3 ($%02X) = high byte of starting block within file for operation\n"\
                                           "           R0 ($%02X) = low  byte of starting block within file for operation\n"\
                                           "           R1 ($%02X) = requested operation (IORead = 1, IOWrite = -1)\n"\
                                           "           R2 ($%02X) = number of blocks to read or write\n"\
                                           "Result:    CC = EQ on success, CC = GT on error\n"\
                                           "           R1 = error code, on error\n", r[3], r[0], r[1], r[2]);
        acase 0x6020: zprintf(TEXTPEN_LOG, "Function:  Rename a file\n"\
        /* DOS_RenameFileByName         */ "Category:  File manipulation\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     PFName2/PFTyp2/FileUnit/FileUnit = current name of the file\n"\
                                           "           ParsedFilename/ParsedUnit/FileUnit = new name of the file\n"\
                                           "Result:    CC = EQ on success, CC = GT on error\n"\
                                           "           R1 = error code, on error\n");
        acase 0x6022: zprintf(TEXTPEN_LOG, "Function:  Remove a file from disk and free the space allocated to it\n"\
        /* DOS_DeleteFileByName         */ "Category:  File manipulation\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     ParsedFilename/ParsedUnit/FileUnit = name of the file\n"\
                                           "Result:    CC = EQ on success, CC = GT on error\n"\
                                           "           R1 = error code, on error\n");
        acase 0x6024: zprintf(TEXTPEN_LOG, "Function:  Load and/or execute a memory image\n"\
        /* DOS_LoadImageByName          */ "Category:  File manipulation\n"\
                                           "Type:      Subroutine or code section\n"\
                                           "Input:     ParsedFilename/FileUnit = name of the image file\n"\
                                           "           R0 ($%02X) = $00 for run, $FF for load\n"\
                                           "Result:    CC = EQ on success, CC = GT on error\n"\
                                           "           R1 = error code, on error\n", r[0]);
        acase 0x6026: zprintf(TEXTPEN_LOG, "Function:  Compute the size of a file\n"\
        /* DOS_FileLengthByFP           */ "Category:  File manipulation\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     OpenFilePtr\n"\
                                           "Result:    R0 = high byte of number of blocks\n"\
                                           "           R3 = low  byte of number of blocks\n");
        acase 0x6028: zprintf(TEXTPEN_LOG, "Function:  Set either change or inspect code on a file\n"\
        /* DOS_SetCodeByName            */ "Category:  Access permissions\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     ParsedFilename/ParsedUnit/FileUnit = name of the file\n"\
                                           "           R0 ($%02X) = requested code to change (OFBK_OCcode or OFBK_OIcode)\n"\
                                           "           Codeword = the prepared codeword\n"\
                                           "           (zeroes, high bits set, set to $FF, or plain text)\n"\
                                           "Result:    CC = EQ on success, CC = GT on failure (access denied)\n"\
                                           "           R1 = error code, on error\n", r[0]);
        acase 0x602A: zprintf(TEXTPEN_LOG, "Function:  Set either change or inspect code on a file,\n"\
        /* DOS_QueryCodeByName          */ "           based on the user's input\n"\
                                           "Category:  Access permissions\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     ParsedFilename/ParsedUnit/FileUnit = name of the file\n"\
                                           "           R0 ($%02X) = requested code to change (OFBK_OCcode or OFBK_OIcode)\n"\
                                           "Result:    None\n", r[0]);
        acase 0x602C: zprintf(TEXTPEN_LOG, "Function:  Display a single character\n"\
        /* DOS_WriteChar                */ "Category:  Console I/O\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     R0 ($%02X) = ASCII character to display\n"\
                                           "Result:    None\n"\
                                           "Preserves: R0..R3\n", r[0]);
        acase 0x602E: zprintf(TEXTPEN_LOG, "Function:  Move the cursor to the left\n"\
        /* DOS_DoBackspace              */ "Category:  Console I/O\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     None\n"\
                                           "Returns:   None\n"\
                                           "Preserves: R1, R2\n");
        acase 0x6030: zprintf(TEXTPEN_LOG, "Function:  Display a byte as two hex digits\n"\
        /* DOS_WriteHexByte             */ "Category:  Console I/O\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     R0 ($%02X) = byte to write\n"\
                                           "Returns:   None\n"
                                           "Preserves: R2, R3\n", r[0]);
        acase 0x6032: zprintf(TEXTPEN_LOG, "Function:  Display a decimal value\n"\
        /* DOS_WriteDecimal             */ "Category:  Console I/O\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     R0 ($%02X) = number to write\n"\
                                           "Returns:   None\n"\
                                           "Preserves: R2, R3\n", r[0]);
        acase 0x6034: zprintf(TEXTPEN_LOG, "Function:  Move the cursor position\n"\
        /* DOS_SetCursor                */ "Category:  Console I/O\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     R2 ($%02X) = high byte of new address of the cursor\n"\
                                           "           R1 ($%02X) = low  byte of new address of the cursor\n"\
                                           "Returns:   None\n"
                                           "Preserves: R1..R3\n"\
                                           "Extra:     Resolved screen coordinate is X=%d,Y=%d\n",
                                              r[2],         r[1],
                                           (((r[2] * 256) + r[1]) - 0x1000) / 16,
                                           (((r[2] * 256) + r[1]) - 0x1000) % 16);
        acase 0x6036: zprintf(TEXTPEN_LOG, "Function:  Move the cursor to the next line\n"\
        /* DOS_Newline                  */ "Category:  Console I/O\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     None\n"\
                                           "Returns:   None\n"
                                           "Preserves: R1\n");
        acase 0x6038: zprintf(TEXTPEN_LOG, "Function:  Read a single character without displaying it\n"\
        /* DOS_ReadKbdNoecho            */ "Category:  Console I/O\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     None\n"\
                                           "Returns:   R0 = ASCII value of the key that was pressed\n"\
                                           "Preserves: R1..R3\n");
        acase 0x603A: zprintf(TEXTPEN_LOG, "Function:  Read a single character from the keyboard\n"\
        /* DOS_ReadKbdEcho              */ "Category:  Console I/O\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     None\n"\
                                           "Returns:   R0 = ASCII value of the key that was pressed\n"\
                                           "Preserves: R1..R3\n");
        acase 0x603C: zprintf(TEXTPEN_LOG, "Function:  Read a string from the keyboard\n"\
        /* DOS_ReadKbdLine              */ "Category:  Console I/O\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     R0 ($%02X) = high byte of address of a buffer to hold the text line\n"\
                                           "           R1 ($%02X) = low  byte of address of a buffer to hold the text line\n"\
                                           "           R2 ($%02X) = maximum length of the entry\n"\
                                           "Result:    CC = EQ on success, CC = LT on cancel (Escape)\n"\
                                           "Preserves: R3\n", r[0], r[1], r[2]);
        acase 0x603E: zprintf(TEXTPEN_LOG, "Function:  Read a string from the keyboard without revealing it\n"\
        /* DOS_ReadKbdCodeword          */ "Category:  Console I/O\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     R0 ($%02X) = high byte of address of a buffer to hold the text line\n"\
                                           "           R1 ($%02X) = low  byte of address of a buffer to hold the text line\n"\
                                           "           R2 ($%02X) = maximum length of the entry\n"\
                                           "Result:    CC = EQ on success, CC = LT on cancel (Escape)\n"\
                                           "Preserves: None\n", r[0], r[1], r[2]);
        acase 0x6040: zprintf(TEXTPEN_LOG, "Function:  Write a DOS error message and return to DOS\n"\
        /* DOS_WrErr_Close_Cmdloop      */ "Category:  Error handling\n"\
                                           "Type:      Code section\n"\
                                           "Input:     R1 ($%02X) = error number (errno_NoAccs .. errno_TooSmall)\n"\
                                           "Result:    None (does not return)\n", r[1]);
        acase 0x6042: zprintf(TEXTPEN_LOG, "Function:  Write a DOS error message\n"\
        /* DOS_WriteErrorStr            */ "Category:  Error handling\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     R1 ($%02X) = error number (errno_NoAccs .. errno_TooSmall)\n"\
                                           "Result:    None\n", r[1]);
        acase 0x6044: zprintf(TEXTPEN_LOG, "Function:  Read a drive number\n"\
        /* DOS_ParseUnit                */ "Category:  Command line\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     CmdLinePtr = pointer to position in CmdLine\n"\
                                           "           CmdLine = command line string\n"\
                                           "Result:    CC = EQ on success, CC = LT on end-of-line, CC = GT on error\n"\
                                           "           R0 = drive number\n");
        acase 0x6046: zprintf(TEXTPEN_LOG, "Function:  Write a character to the printer\n"\
        /* DOS_Print                    */ "Category:  Device I/O\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     R0 ($%02X) = Character to print\n"\
                                           "Returns:   None\n"\
                                           "Preserves: None\n", r[0]);
        acase 0x6048: zprintf(TEXTPEN_LOG, "Function:  Blank the entire screen\n"\
        /* DOS_ClearScreen              */ "Category:  Console I/O\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     None\n"\
                                           "Returns:   None\n"\
                                           "Preserves: None\n");
        acase 0x604A: zprintf(TEXTPEN_LOG, "Function:  Replace the contents of a file\n"\
        /* DOS_ReattachByName           */ "Category:  File manipulation\n"\
                                           "Type:      Subroutine\n"\
                                           "Input:     ParsedFilename/ParsedUnit/FileUnit = name of the file\n"\
                                           "           PFName2/PFTyp2/FileUnit/FileUnit = file from which to take contents\n"\
                                           "Result:    CC = EQ on success, CC = GT on error\n"\
                                           "           R1 = error code, on error\n");
        }

        switch (ea)
        {
        // code sections
        case  0x6004:
        case  0x6006:
        case  0x6040:
            if
            (   (opcode & 0xF0) == 0x30 // BSTx
             || (opcode & 0xF0) == 0x70 // BSNx
             || (opcode & 0xF0) == 0xB0 // BSFx/ZBSR/BSXA
            )
            {   zprintf(TEXTPEN_LOG, "Code section vectored at $%X has been branched to as a subroutine!\n", ea);
            }
        // subroutines
        acase 0x6008:
        case  0x600A:
        case  0x600C:
        case  0x600E:
        case  0x6010:
        case  0x6012:
        case  0x6014:
        case  0x6016:
        case  0x6018:
        case  0x601A:
        case  0x601C:
        case  0x601E:
        case  0x6020:
        case  0x6022:
        case  0x6026:
        case  0x6028:
        case  0x602A:
        case  0x602C:
        case  0x602E:
        case  0x6030:
        case  0x6032:
        case  0x6034:
        case  0x6036:
        case  0x6038:
        case  0x603A:
        case  0x603C:
        case  0x603E:
        case  0x6042:
        case  0x6044:
        case  0x6046:
        case  0x6048:
        case  0x604A:
            if
            (   (opcode & 0xF0) != 0x30 // BSTx
             && (opcode & 0xF0) != 0x70 // BSNx
             && (opcode & 0xF0) != 0xB0 // BSFx/ZBSR/BSXA
            )
            {   zprintf(TEXTPEN_LOG, "Subroutine vectored at $%X has been jumped to!\n", ea);
}   }   }   }

EXPORT void cd2650_drawhelpgrid(void)
{   int x,  y,
        xx, yy,
        startx,
        starty;

    for (x = 0; x < 80; x++)
    {   for (y = 0; y < 16; y++)
        {   startx = x *  8;
            starty = y * 12;
            for (xx = 0; xx < 8; xx++)
            {   for (yy = 0; yy < 12; yy++)
                {   if (xx == 0 || xx == 8 - 1 || yy == 0 || yy == 12 - 1)
                    {   changepixel(startx + xx, starty + yy, GREY1);
}   }   }   }   }   }

EXPORT FLAG cd2650_edit_screen(UWORD code)
{   FLAG capitalize;
    int  i,
         kx, ky;

    kx = (scrnaddr - 0x1000) / 16;
    ky =  scrnaddr           % 16;

    switch (code)
    {
    case  SCAN_UP:               if (shift()) ky =  0; elif (ky ==  0) ky = 15; else ky--;
    acase SCAN_DOWN:             if (shift()) ky = 15; elif (ky == 15) ky =  0; else ky++;
    acase SCAN_LEFT:             if (shift()) kx =  0; elif (kx ==  0) kx = 79; else kx--;
    acase SCAN_RIGHT:            if (shift()) kx = 79; elif (kx == 79) kx =  0; else kx++;
    acase SCAN_BACKSPACE:        if (kx == 0 && ky == 0)
                                 {   kx = 79;
                                     ky = 15;
                                 } elif (kx == 0)
                                 {   kx = 79;
                                     ky--;
                                 } else
                                 {   kx--;
                                 }
                                 scrnaddr = 0x1000 + (kx * 16) + ky;
                                 memory[scrnaddr] = 0x20;
    acase SCAN_DEL:              if (shift())
                                 {   for (i = 0x1000; i <= 0x14FF; i++)
                                     {   memory[i] = 0x20;
                                 }   }
                                 else
                                 {   memory[scrnaddr] = 0x20;
                                 }
    acase SCAN_AE: case SCAN_NE: kx = 0;
                                 if (ky == 15)
                                 {   ky = 0;
                                 } else
                                 {   ky++;
                                 }
#ifdef WIN32
    acase SCAN_HOME:             kx =  0; if (shift()) ky =  0;
    acase SCAN_END:              kx = 79; if (shift()) ky = 15;
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
        if (lowercase && !capslock && !shift() && cd2650_biosver == CD2650_IPL)
        {   capitalize = FALSE;
        } else
        {   capitalize = TRUE;
        }
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
        acase SCAN_A2: memory[scrnaddr] = shift() ? 0x00 : 0x32; // @
        acase SCAN_A3: memory[scrnaddr] = shift() ? 0x23 : 0x33; // #
        acase SCAN_A4: memory[scrnaddr] = shift() ? 0x24 : 0x34; // $
        acase SCAN_A5: memory[scrnaddr] = shift() ? 0x25 : 0x35; // %
        acase SCAN_A6: memory[scrnaddr] = shift() ? 0x1E : 0x36; // ^ or up arrow
        acase SCAN_A7: memory[scrnaddr] = shift() ? 0x26 : 0x37; // '
        acase SCAN_A8: memory[scrnaddr] = shift() ? 0x2A : 0x38; // *
        acase SCAN_A9: memory[scrnaddr] = shift() ? 0x28 : 0x39; // (
        acase SCAN_A0: memory[scrnaddr] = shift() ? 0x29 : 0x30; // )
        acase SCAN_A:  memory[scrnaddr] = capitalize ? 0x41 : 0x61;
        acase SCAN_B:  memory[scrnaddr] = capitalize ? 0x42 : 0x62;
        acase SCAN_C:  memory[scrnaddr] = capitalize ? 0x43 : 0x63;
        acase SCAN_D:  memory[scrnaddr] = capitalize ? 0x44 : 0x64;
        acase SCAN_E:  memory[scrnaddr] = capitalize ? 0x45 : 0x65;
        acase SCAN_F:  memory[scrnaddr] = capitalize ? 0x46 : 0x66;
        acase SCAN_G:  memory[scrnaddr] = capitalize ? 0x47 : 0x67;
        acase SCAN_H:  memory[scrnaddr] = capitalize ? 0x48 : 0x68;
        acase SCAN_I:  memory[scrnaddr] = capitalize ? 0x49 : 0x69;
        acase SCAN_J:  memory[scrnaddr] = capitalize ? 0x4A : 0x6A;
        acase SCAN_K:  memory[scrnaddr] = capitalize ? 0x4B : 0x6B;
        acase SCAN_L:  memory[scrnaddr] = capitalize ? 0x4C : 0x6C;
        acase SCAN_M:  memory[scrnaddr] = capitalize ? 0x4D : 0x6D;
        acase SCAN_N:  memory[scrnaddr] = capitalize ? 0x4E : 0x6E;
        acase SCAN_O:  memory[scrnaddr] = capitalize ? 0x4F : 0x6F;
        acase SCAN_P:  memory[scrnaddr] = capitalize ? 0x50 : 0x70;
        acase SCAN_Q:  memory[scrnaddr] = capitalize ? 0x51 : 0x71;
        acase SCAN_R:  memory[scrnaddr] = capitalize ? 0x52 : 0x72;
        acase SCAN_S:  memory[scrnaddr] = capitalize ? 0x53 : 0x73;
        acase SCAN_T:  memory[scrnaddr] = capitalize ? 0x54 : 0x74;
        acase SCAN_U:  memory[scrnaddr] = capitalize ? 0x55 : 0x75;
        acase SCAN_V:  memory[scrnaddr] = capitalize ? 0x56 : 0x76;
        acase SCAN_W:  memory[scrnaddr] = capitalize ? 0x57 : 0x77;
        acase SCAN_X:  memory[scrnaddr] = capitalize ? 0x58 : 0x78;
        acase SCAN_Y:  memory[scrnaddr] = capitalize ? 0x59 : 0x79;
        acase SCAN_Z:  memory[scrnaddr] = capitalize ? 0x5A : 0x7A;
        acase SCAN_OB:         memory[scrnaddr] =                     shift()  ? 0x5B : 0x1B;
        acase SCAN_BACKSLASH:  memory[scrnaddr] =                     shift()  ? 0x5C : 0x1C;
        acase SCAN_CB:         memory[scrnaddr] =                     shift()  ? 0x5D : 0x1D;
        acase SCAN_SPACEBAR:   memory[scrnaddr] =                     shift()  ? 0x5F : 0x20;
        acase SCAN_APOSTROPHE: memory[scrnaddr] =                     shift()  ? 0x22 : 0x27;
        acase SCAN_AS:         memory[scrnaddr] =                                       0x2A;
        acase SCAN_PL:         memory[scrnaddr] =                                       0x2B;
        acase SCAN_COMMA:      memory[scrnaddr] =                     shift()  ? 0x3C : 0x2C;
        acase SCAN_AMINUS:     memory[scrnaddr] =                     shift()  ? 0x1F : 0x2D;
        acase SCAN_MI:         memory[scrnaddr] =                                       0x2D;
        acase SCAN_FULLSTOP:   memory[scrnaddr] =                     shift()  ? 0x3E : 0x2E;
        acase SCAN_ND:         memory[scrnaddr] =                                       0x2E;
        acase SCAN_SLASH:      memory[scrnaddr] =                     shift()  ? 0x3F : 0x2F;
        acase SCAN_SL:         memory[scrnaddr] =                                       0x2F;
        acase SCAN_SEMICOLON:  memory[scrnaddr] =                     shift()  ? 0x3A : 0x3B;
        acase SCAN_EQUALS:     memory[scrnaddr] =                     shift()  ? 0x2B : 0x3D;
        acase SCAN_BACKTICK:   memory[scrnaddr] =                     shift()  ? 0x5E : 0x40;
#ifdef AMIGA
        acase SCAN_OP:         memory[scrnaddr] =                                       0x28;
        acase SCAN_CP:         memory[scrnaddr] =                                       0x29;
#endif
        adefault:              return FALSE;
        }
        if (kx == 79 && ky == 15)
        {   kx = ky = 0;
        } elif (kx == 79)
        {   kx = 0;
            ky++;
        } else
        {   kx++;
    }   }
    scrnaddr = 0x1000 + (kx * 16) + ky;

    return TRUE; // return code is whether we ate the keystroke
}

EXPORT void cd2650_create_disk(int whichdrive)
{   int i;

    memset(drive[whichdrive].contents, 0, CD2650_DISKSIZE);

    read_rtc();

    if (cd2650_dosver == DOS_P1DOS)
    {   for (i = 0; i <= 0x277; i++)
        {   drive[whichdrive].contents[i] = p1dos[i];
    }   }
    else
    {   drive[whichdrive].contents[ 0] = 'D';
        drive[whichdrive].contents[ 1] = 'O';
        drive[whichdrive].contents[ 2] = 'S';
        drive[whichdrive].contents[ 8] = 'S';
        drive[whichdrive].contents[ 9] = 'Y';
        drive[whichdrive].contents[10] = 'S';
        drive[whichdrive].contents[12] =
        drive[whichdrive].contents[20] = 0xFF; // means supervisor
        drive[whichdrive].contents[28] =    2; // start track
        drive[whichdrive].contents[30] =    4; // end   track
        drive[whichdrive].contents[29] =    1; // start sector
        drive[whichdrive].contents[31] =    9; // end   sector
        drive[whichdrive].contents[40] = drive[whichdrive].contents[48] = '0' + ( rtc.month        / 10);
        drive[whichdrive].contents[41] = drive[whichdrive].contents[49] = '0' + ( rtc.month        % 10);
        drive[whichdrive].contents[42] = drive[whichdrive].contents[50] = '-';
        drive[whichdrive].contents[43] = drive[whichdrive].contents[51] = '0' + ( rtc.day          / 10);
        drive[whichdrive].contents[44] = drive[whichdrive].contents[52] = '0' + ( rtc.day          % 10);
        drive[whichdrive].contents[45] = drive[whichdrive].contents[53] = '-';
        drive[whichdrive].contents[46] = drive[whichdrive].contents[54] = '0' + ((rtc.year  % 100) / 10);
        drive[whichdrive].contents[47] = drive[whichdrive].contents[55] = '0' + ((rtc.year  % 100) % 10);

        drive[whichdrive].contents[64 +  0] = 'U';
        drive[whichdrive].contents[64 +  1] = 'S';
        drive[whichdrive].contents[64 +  2] = 'E';
        drive[whichdrive].contents[64 +  3] = 'R';
        drive[whichdrive].contents[64 +  8] = 'N';
        drive[whichdrive].contents[64 +  9] = 'A';
        drive[whichdrive].contents[64 + 10] = 'M';
        drive[whichdrive].contents[64 + 11] = 'E';
        drive[whichdrive].contents[64 + 12] =
        drive[whichdrive].contents[64 + 20] = 0xFF; // means supervisor
        drive[whichdrive].contents[64 + 28] =
        drive[whichdrive].contents[64 + 30] =    5; // track  (track 1, sector 8 can't be used!)
        drive[whichdrive].contents[64 + 29] =
        drive[whichdrive].contents[64 + 31] =    1; // sector (track 1, sector 8 can't be used!)
        drive[whichdrive].contents[64 + 40] = drive[whichdrive].contents[64 + 48] = '0' + ( rtc.month        / 10);
        drive[whichdrive].contents[64 + 41] = drive[whichdrive].contents[64 + 49] = '0' + ( rtc.month        % 10);
        drive[whichdrive].contents[64 + 42] = drive[whichdrive].contents[64 + 50] = '-';
        drive[whichdrive].contents[64 + 43] = drive[whichdrive].contents[64 + 51] = '0' + ( rtc.day          / 10);
        drive[whichdrive].contents[64 + 44] = drive[whichdrive].contents[64 + 52] = '0' + ( rtc.day          % 10);
        drive[whichdrive].contents[64 + 45] = drive[whichdrive].contents[64 + 53] = '-';
        drive[whichdrive].contents[64 + 46] = drive[whichdrive].contents[64 + 54] = '0' + ((rtc.year  % 100) / 10);
        drive[whichdrive].contents[64 + 47] = drive[whichdrive].contents[64 + 55] = '0' + ((rtc.year  % 100) % 10);
    }

    if (cd2650_dosver != DOS_NOCD2650DOS)
    {   memcpy(&drive[whichdrive].contents[2 * CD2650_TRACKSIZE], cd2650_dos[cd2650_dosver], 6912); // 6.75K (tracks 2..4)
    }

    for (i = 0; i < CD2650_DISKSIZE / 8; i++)
    {   drive[whichdrive].flags[i] = 0;
    }

    drive[whichdrive].fn_disk[0] = EOS;
    drive[whichdrive].inserted   = TRUE;
    cd2650_dir_disk(TRUE, whichdrive);
}

EXPORT void cd2650_dir_disk(FLAG quiet, int whichdrive)
{   int  i, j,
         startblock,
         endblock,
         thesize;
    TEXT datecreated[8 + 1],
         datemodified[8 + 1],
         modifier[8 + 1],
         readlevel[13 + 1], // enough for "User 12345678"
         writelevel[13 + 1]; // enough for "User 12345678"

    if (!drive[whichdrive].inserted)
    {   return;
    }

    for (i =  0; i <                18; i++)
    {   drive[whichdrive].bam[i] = BAM_DIR;
    }
    for (i = 18; i < CD2650_DISKBLOCKS; i++)
    {   drive[whichdrive].bam[i] = BAM_FREE;
    }

    if (!quiet)
    {   zprintf
        (   TEXTPEN_DISK,
            "Filename.Extn  Size Created  Modified Read          Write         Modified by\n" \
            "------------- ----- -------- -------- ------------- ------------- -----------\n"
        );
    }

    usedsize = 0;
    for (i = 0; i < 64; i++)
    {   if (drive[whichdrive].contents[i * 64] == EOS)
        {   drive[whichdrive].filename[i][0] = EOS;
        } else
        {   zstrncpy(drive[whichdrive].filename[i], (const char*) &drive[whichdrive].contents[ i * 64      ], 8);
            zstrncpy(drive[whichdrive].fileext[ i], (const char*) &drive[whichdrive].contents[(i * 64) +  8], 4);
            if (drive[whichdrive].contents[(i * 64) + 32])
            {   zstrncpy(modifier,         (const char*) &drive[whichdrive].contents[(i * 64) + 32], 8);
            } else
            {   strcpy(modifier, "None");
            }

            startblock = (drive[whichdrive].contents[(i * 64) + 28] * CD2650_SECTORS) // track
                       +  drive[whichdrive].contents[(i * 64) + 29] - 1;              // sector
            endblock   = (drive[whichdrive].contents[(i * 64) + 30] * CD2650_SECTORS) // track
                       +  drive[whichdrive].contents[(i * 64) + 31] - 1;              // sector
            if
            (   startblock                       >  endblock
             || drive[whichdrive].contents[(i * 64) + 28] >= CD2650_TRACKS  // track
             || drive[whichdrive].contents[(i * 64) + 29] >  CD2650_SECTORS // sector
             || drive[whichdrive].contents[(i * 64) + 30] >= CD2650_TRACKS  // track
             || drive[whichdrive].contents[(i * 64) + 31] >  CD2650_SECTORS // sector
            )
            {   thesize = 0;
            } else
            {   thesize = (endblock - startblock + 1) * CD2650_BLOCKSIZE;
            }
            usedsize += thesize;
            for (j = startblock; j <= endblock; j++)
            {   drive[whichdrive].bam[j] = BAM_FILE;
            }

            switch (drive[whichdrive].contents[(i * 64) + 20])
            {
            case    0: strcpy(readlevel, "Public");
            acase 255: strcpy(readlevel, "Supervisor");
            adefault:
                if (drive[whichdrive].contents[(i * 64) + 20] <= 0x7F)
                {   strcpy(readlevel, "Code ");
                    strncat(readlevel, (const char*) &drive[whichdrive].contents[(i * 64) + 20], 8);
                } else
                {   strcpy(readlevel, "User ");
                    for (j = 0; j < 8; j++)
                    {   readlevel[5 + j] = drive[whichdrive].contents[(i * 64) + 20 + j] & 0x7F;
                    }
                    readlevel[13] = EOS;
            }   }
            switch (drive[whichdrive].contents[(i * 64) + 20])
            {
            case    0: strcpy(writelevel, "Public");
            acase 255: strcpy(writelevel, "Supervisor");
            adefault:
                if (drive[whichdrive].contents[(i * 64) + 12] <= 0x7F)
                {   strcpy(writelevel, "Code ");
                    strncat(writelevel, (const char*) &drive[whichdrive].contents[(i * 64) + 12], 8);
                } else
                {   strcpy(writelevel, "User ");
                    for (j = 0; j < 8; j++)
                    {   writelevel[5 + j] = drive[whichdrive].contents[(i * 64) + 20 + j] & 0x7F;
                    }
                    writelevel[13] = EOS;
            }   }

            if (yank)
            {   datecreated[ 0] = drive[whichdrive].contents[(i * 64) + 48]; // tens of month
                datecreated[ 1] = drive[whichdrive].contents[(i * 64) + 49]; // ones of month
                datecreated[ 3] = drive[whichdrive].contents[(i * 64) + 51]; // tens of day
                datecreated[ 4] = drive[whichdrive].contents[(i * 64) + 52]; // ones of day
                datemodified[0] = drive[whichdrive].contents[(i * 64) + 40]; // tens of month
                datemodified[1] = drive[whichdrive].contents[(i * 64) + 41]; // ones of month
                datemodified[3] = drive[whichdrive].contents[(i * 64) + 43]; // tens of day
                datemodified[4] = drive[whichdrive].contents[(i * 64) + 44]; // ones of day
            } else
            {   datecreated[ 0] = drive[whichdrive].contents[(i * 64) + 51]; // tens of day
                datecreated[ 1] = drive[whichdrive].contents[(i * 64) + 52]; // ones of day
                datecreated[ 3] = drive[whichdrive].contents[(i * 64) + 48]; // tens of month
                datecreated[ 4] = drive[whichdrive].contents[(i * 64) + 49]; // ones of month
                datemodified[0] = drive[whichdrive].contents[(i * 64) + 43]; // tens of day
                datemodified[1] = drive[whichdrive].contents[(i * 64) + 44]; // ones of day
                datemodified[3] = drive[whichdrive].contents[(i * 64) + 40]; // tens of month
                datemodified[4] = drive[whichdrive].contents[(i * 64) + 41]; // ones of month
            }
            datecreated[ 6] = drive[whichdrive].contents[(i * 64) + 54]; // tens of year
            datecreated[ 7] = drive[whichdrive].contents[(i * 64) + 55]; // ones of year
            datemodified[6] = drive[whichdrive].contents[(i * 64) + 46]; // tens of year
            datemodified[7] = drive[whichdrive].contents[(i * 64) + 47]; // ones of year
#ifdef WIN32
            datecreated[ 2] =
            datecreated[ 5] =
            datemodified[2] =
            datemodified[5] = langs[language].datesep;
#endif
#ifdef AMIGA
            datecreated[ 2] = drive[whichdrive].contents[(i * 64) + 50];
            datecreated[ 5] = drive[whichdrive].contents[(i * 64) + 53];
            datemodified[2] = drive[whichdrive].contents[(i * 64) + 42];
            datemodified[5] = drive[whichdrive].contents[(i * 64) + 45];
#endif
            datecreated[ 8] = datemodified[8] = EOS;
            if (datecreated[ 0] == EOS) strcpy(datecreated,  "--------");
            if (datemodified[0] == EOS) strcpy(datemodified, "--------");

            if (!quiet)
            {   zprintf
                (   TEXTPEN_DISK,
                    "%-8s.%-4s %5d %s %s %-13s %-13s %-8s\n",
                    drive[whichdrive].filename[i],
                    drive[whichdrive].fileext[i],
                    thesize,
                    datecreated,
                    datemodified,
                    readlevel,
                    writelevel,
                    modifier
                );

                if
                (   startblock                       >  endblock
                 || drive[whichdrive].contents[(i * 64) + 28] >= CD2650_TRACKS  // track
                 || drive[whichdrive].contents[(i * 64) + 29] == 0              // sector
                 || drive[whichdrive].contents[(i * 64) + 29] >  CD2650_SECTORS // sector
                 || drive[whichdrive].contents[(i * 64) + 30] >= CD2650_TRACKS  // track
                 || drive[whichdrive].contents[(i * 64) + 31] == 0              // sector
                 || drive[whichdrive].contents[(i * 64) + 31] >  CD2650_SECTORS // sector
                )
                {   zprintf
                    (   TEXTPEN_ERROR,
                        "Bad block range (tracks %d..%d, sectors %d..%d)!\n",
                        drive[whichdrive].contents[(i * 64) + 28], // track
                        drive[whichdrive].contents[(i * 64) + 30], // track
                        drive[whichdrive].contents[(i * 64) + 29], // sector
                        drive[whichdrive].contents[(i * 64) + 31]  // sector
                    );
    }   }   }   }

    if (!quiet)
    {   zprintf
        (   TEXTPEN_DISK,
            "\n%5d bytes used by files.\n" \
            "%5d bytes reserved for file list.\n" \
            "%5d bytes free.\n" \
            "%5d bytes total.\n\n",
            usedsize,
            18 * CD2650_BLOCKSIZE,
            CD2650_DISKSIZE - usedsize - (18 * CD2650_BLOCKSIZE),
            CD2650_DISKSIZE
        );
    }

    viewingdrive = whichdrive;
    update_floppydrive(3, viewingdrive);
}

EXPORT FLAG cd2650_load_disk(FLAG wantasl, int whichdrive)
{   FILE* DiskHandle /* = NULL */ ;
    int   disksize,
          i;

    // asl() turns sound off and on for us
    if (wantasl)
    {   if (asl
        (   LLL(MSG_HAIL_INSERTDISK, "Insert Disk"),
#ifdef WIN32
            "CD2650 floppy disk (*.RAW)\0*.RAW\0" \
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
    if (disksize != CD2650_DISKSIZE)
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

    for (i = 0; i < CD2650_DISKSIZE / 8; i++)
    {   drive[whichdrive].flags[i] = 0;
    }

    drive[whichdrive].inserted = TRUE;
    cd2650_dir_disk(TRUE, whichdrive);
    if (!SubWindowPtr[SUBWINDOW_FLOPPYDRIVE])
    {   open_floppydrive(FALSE);
    }

    return TRUE;
}

EXPORT FLAG cd2650_extract_file(int whichfile)
{   int   startblock, endblock,
          startbyte,  endbyte;
    FILE* TheLocalHandle;

    sprintf((char*) gtempstring, "%s.%s", drive[debugdrive].filename[whichfile], drive[debugdrive].fileext[whichfile]);
    startblock = (drive[debugdrive].contents[(whichfile * 64) + 28] * CD2650_SECTORS) // track
               +  drive[debugdrive].contents[(whichfile * 64) + 29];                  // sector
    endblock   = (drive[debugdrive].contents[(whichfile * 64) + 30] * CD2650_SECTORS) // track
               +  drive[debugdrive].contents[(whichfile * 64) + 31];                  // sector
    startbyte  =  (startblock  ) * CD2650_BLOCKSIZE;
    endbyte    = ((endblock + 1) * CD2650_BLOCKSIZE) - 1;

    if ((TheLocalHandle = fopen((const char*) gtempstring, "wb")))
    {   DISCARD fwrite(&drive[debugdrive].contents[startbyte], (size_t) (endbyte - startbyte + 1), 1, TheLocalHandle);
        DISCARD fclose(TheLocalHandle);
        // TheLocalHandle = NULL;
        return TRUE;
    } else
    {   return FALSE;
}   }

EXPORT FLAG load_imag(int localsize)
{   int chunksize,
        cursor,
        i,
        loadaddr;

    cursor = 0;
    while (cursor < localsize)
    {   if (cursor + 4 > localsize)
        {   zprintf(TEXTPEN_ERROR, "Warning: incomplete chunk header at end of file.\n");
            zprintf(TEXTPEN_ERROR, "Warning: no end-of-file chunk found.\n\n");
            return TRUE;
        }
        loadaddr  = (IOBuffer[cursor    ] * 256) + IOBuffer[cursor + 1];
        chunksize = (IOBuffer[cursor + 2] * 256) + IOBuffer[cursor + 3];
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
            {   zprintf(TEXTPEN_CLIOUTPUT, "Successfully loaded IMAG file.\n\n");
            }
            return TRUE;
        } // implied else
        if (chunksize > 32768)
        {   zprintf(TEXTPEN_ERROR, "Warning: excessively long chunk.\n");
        }
        cursor += 4;
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

EXPORT FLAG save_imag(STRPTR filename, int startaddr, int endaddr, int progstart)
{   FILE* IMAGHandle;
    UBYTE IMAGBuffer[4];
    int   chunksize;

    // assert(startaddr <= endaddr);

#ifdef VERBOSE
    zprintf(TEXTPEN_CLIOUTPUT, "Saving %s from $%X..$%X as IMAG...\n", filename, startaddr, endaddr);
#endif

    if (!(IMAGHandle = fopen(filename, "wb")))
    {   zprintf(TEXTPEN_CLIOUTPUT, "Can't open \"%s\" for writing!\n", filename);
        return FALSE;
    } // implied else

    chunksize = endaddr - startaddr + 1;

    IMAGBuffer[0] = (UBYTE) (startaddr / 256);
    IMAGBuffer[1] = (UBYTE) (startaddr % 256);
    IMAGBuffer[2] = (UBYTE) (chunksize / 256);
    IMAGBuffer[3] = (UBYTE) (chunksize % 256);
    DISCARD fwrite(IMAGBuffer, 4, 1, IMAGHandle);

    DISCARD fwrite(&memory[startaddr], (size_t) chunksize, 1, IMAGHandle);

    IMAGBuffer[0] = (UBYTE) (progstart / 256);
    IMAGBuffer[1] = (UBYTE) (progstart % 256);
    IMAGBuffer[2] =
    IMAGBuffer[3] = 0;
    DISCARD fwrite(IMAGBuffer, 4, 1, IMAGHandle);

    DISCARD fclose(IMAGHandle);
    // IMAGHandle = NULL;

    return TRUE;
}

EXPORT void cd2650_updatecharset(void)
{   int i, j;

    if (cd2650_vdu == VDU_ASCII)
    {
#ifdef GREEKCHARS
        for (i = 0; i < 32; i++)
        {   for (j = 0; j < 8; j++)
            {   cd2650_chars_bmp[i][j] = cd2650_greek[i][j];
        }   }
#else
        for (i = 0; i < 32; i++)
        {   for (j = 0; j < 10; j++)
            {   cd2650_chars_bmp[i][j] = ~cd2650_chars_defbmp[i][j];
        }   }
#endif
        for (i = 32; i < 128; i++)
        {   for (j = 0; j < 10; j++)
            {   cd2650_chars_bmp[i][j] = cd2650_chars_defbmp[i][j];
    }   }   }
    else
    {   for (i = 0; i < 128; i++)
        {   for (j = 0; j < 10; j++)
            {   cd2650_chars_bmp[i][j] = cd2650_chars_defbmp[i][j];
        }   }
        if (cd2650_vdu == VDU_CHESSMEN)
        {   for (i =  0; i < 16; i++)
            {   for (j = 0; j < 10; j++)
                {   cd2650_chars_bmp[0x40 + i][j] = chessman[i][j];                   // chessmen  into $40..$4F
            }   }
            for (i = 16; i < 32; i++)
            {   for (j = 0; j < 10; j++)
                {   cd2650_chars_bmp[0x40 + i][j] = cd2650_chars_defbmp[0x60 + i][j]; // lowercase into $50..$5F
        }   }   }
        else
        {   for (i = 0; i < 32; i++)
            {   for (j = 0; j < 10; j++)
                {   cd2650_chars_bmp[0x40 + i][j] = cd2650_chars_defbmp[0x60 + i][j]; // lowercase into $40..$5F
}   }   }   }   }

EXPORT void cd2650_changebios(void)
{   if (cd2650_biosver == CD2650_IPL && cd2650_dosver != DOS_NOCD2650DOS) // if using DOS
    {   cd2650_vdu = VDU_ASCII;
    } else
    {   if (cd2650_vdu == VDU_ASCII)
        {   cd2650_vdu = VDU_LOWERCASE;
    }   }
    changemachine(CD2650, memmap, TRUE, TRUE, TRUE); // calls cd2650_updatecharset()
    updatemenus();
    engine_reset();
    redrawscreen(); // important!
    reopen_subwindows();
}

EXPORT void cd2650_delete_file(int whichfile, int whichdrive)
{   int startblock, endblock,
        startbyte,  endbyte;

    startblock = (drive[whichdrive].contents[(whichfile * 64) + 28] * CD2650_SECTORS) // track
               +  drive[whichdrive].contents[(whichfile * 64) + 29];                  // sector
    endblock   = (drive[whichdrive].contents[(whichfile * 64) + 30] * CD2650_SECTORS) // track
               +  drive[whichdrive].contents[(whichfile * 64) + 31];                  // sector
    startbyte  =  (startblock  ) * CD2650_BLOCKSIZE;
    endbyte    = ((endblock + 1) * CD2650_BLOCKSIZE) - 1;

    memset(&drive[whichdrive].contents[whichfile * 64], 0,           64                      ); // clear directory entry
    memset(&drive[whichdrive].contents[startbyte     ], 0, (size_t) (endbyte - startbyte + 1)); // clear actual file
}

EXPORT FLAG cd2650_rename_file(int whichfile, STRPTR newname)
{   TEXT filepart[8 + 1],
         extpart[ 4 + 1];
    int  i, j,
         thelength;

    for (i = 0; i < 8; i++)
    {   if (newname[i] == '.' || newname[i] == EOS)
        {   goto DONE1;
        }
        if
        (   (newname[i] >= '0' && newname[i] <= '9')
         || (newname[i] >= 'A' && newname[i] <= 'Z')
         || (newname[i] >= 'a' && newname[i] <= 'z')
        )
        {   ; // OK
        } else
        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_ILLEGALFILENAME, "Illegal character '%c' in filename!\n\n"), newname[i]);
            return FALSE;
    }   }
    if (newname[8] != '.' && newname[8] != EOS)
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FILENAMEISTOOLONG, "Filename is too long!\n\n"));
        return FALSE;
    }

DONE1:
    zstrncpy(filepart, newname, i);

    if (newname[i] == '.')
    {   for (j = 1; j <= 4; j++)
        {   if (newname[i + j] == EOS)
            {   zstrncpy(extpart, &newname[i + 1], j);
                goto DONE2;
            }
            if
            (   (newname[i + j] >= '0' && newname[i + j] <= '9')
             || (newname[i + j] >= 'A' && newname[i + j] <= 'Z')
             || (newname[i + j] >= 'a' && newname[i + j] <= 'z')
             ||  newname[i + j] == '$'
            )
            {   ; // OK
            } else
            {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_ILLEGALEXTENSION, "Illegal character '%c' in extension!\n\n"), newname[i + j]);
                return FALSE;
        }   }
        if (newname[i + 4 + 1] != EOS)
        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_EXTENSIONISTOOLONG, "Extension is too long!\n\n"));
            return FALSE;
        }
        zstrncpy(extpart, &newname[i + 1], j);
    } else
    {   extpart[0] = EOS;
    }

DONE2:
    memset(&drive[debugdrive].contents[whichfile * 64], 0, 12);
    thelength = strlen(filepart);
    for (i = 0; i < thelength; i++)
    {   drive[debugdrive].contents[(whichfile * 64)     + i] = toupper(filepart[i]);
    }
    thelength = strlen(extpart);
    for (i = 0; i < thelength; i++)
    {   drive[debugdrive].contents[(whichfile * 64) + 8 + i] = toupper(extpart[i]);
    }

    return TRUE;
}

EXPORT void cd2650_view_basic(void)
{   PERSIST   TEXT   contents[255 + 1],
                     varname[21 + 1]; // 15 + $( + 3 digit number + ) + EOS
    TRANSIENT int    arraysize,
                     i, j,
                     here,
                     next,
                     varnamelength;
    TRANSIENT UBYTE  varkind;

    // assert(machine == CD2650);

    i = memory[0x2004] * 256;
    zprintf(TEXTPEN_CLIOUTPUT, "Addr. Name                  Contents = Value\n" \
            "----- --------------------- -------- = -----\n");

    while ((memory[i] & 0xF) != 0)
    {   varkind       =   memory[i] & 0xC0;
        if (varkind == 0xC0 || (memory[i] & 0x30))
        {   zprintf(TEXTPEN_CLIOUTPUT, "!?\n");
            break;
        }

        varnamelength =   memory[i] & 0x0F;
        next          =  (memory[i + 1] * 256) + memory[i + 2];
        here          =  i;
        i             += 3;

        for (j = 0; j < varnamelength; j++)
        {   varname[j] = memory[i++]; // variable names are stored in ASCII format, not CD2650 format!
        }
        varname[varnamelength] = EOS;
        if (varkind & 0x80) // string
        {   arraysize = memory[i++];
            for (j = 0; j < arraysize; j++)
            {   contents[j] = cd2650_chars[memory[i++] & 0x7F];
            }
            contents[arraysize] = EOS;

            sprintf((char*) &varname[varnamelength], "$(%d)", arraysize);
            zprintf
            (   TEXTPEN_CLIOUTPUT,
                "$%04X %-21s '%s'\n",
                here,
                varname,
                contents
            );
        } elif (varkind & 0x40) // array
        {   arraysize = (next - i) / 4;
            zprintf(TEXTPEN_CLIOUTPUT, "$%04X %s(%d):\n", here, varname, arraysize);

            j = 1;
            do
            {   sprintf((char*) &varname[varnamelength], "(%d)", j++);
                zprintf
                (   TEXTPEN_CLIOUTPUT,
                    "$%04X %-21s %02X%02X%02X%02X = ",
                    i,
                    varname,
                    memory[i],
                    memory[i + 1],
                    memory[i + 2],
                    memory[i + 3]
                );
                print_float(i);

                i += 4;
            } while (i < next);
        } else
        {   zprintf
            (   TEXTPEN_CLIOUTPUT,
                "$%04X %-21s %02X%02X%02X%02X = ",
                here,
                varname,
                memory[i],
                memory[i + 1],
                memory[i + 2],
                memory[i + 3]
            );
            print_float(i);
        }

        i = next;
    }

    zprintf
    (   TEXTPEN_CLIOUTPUT,
        "$%04X Top of stack\n\n",
        (memory[0x32CF] * 256) + memory[0x32D0]
    );
}

EXPORT float basic_to_float(int start)
{   ULONG      intpart,
               fracpart;
    signed int exponent;
    double     value;
PERSIST const ULONG  mask[24] =
{ 0x7FFFFF, //  0
  0x3FFFFF,
  0x1FFFFF,
  0x0FFFFF,
  0x07FFFF,
  0x03FFFF,
  0x01FFFF,
  0x00FFFF,
  0x007FFF,
  0x003FFF,
  0x001FFF, // 10
  0x000FFF,
  0x0007FF,
  0x0003FF,
  0x0001FF,
  0x0000FF,
  0x00007F,
  0x00003F,
  0x00001F,
  0x00000F,
  0x000007, // 20
  0x000003,
  0x000001,
  0x000000  // 23
};

    if (memory[start] == 0x80 && (memory[start + 1] & 0x7F) == 0 && memory[start + 2] == 0 && memory[start + 3] == 0) // +0 or -0
    {   return 0.0f;
    } // implied else

    exponent = memory[start] & 0x7F;
    if (memory[start] & 0x80)
    {   exponent -= 128;
    }
    intpart = fracpart = ((memory[start + 1] & 0x7F) * 65536)
                       + ( memory[start + 2]         *   256)
                       +   memory[start + 3];
    if (exponent > 23)                 // huge  value (increased integer part, 0 for     fractional part)
    {   intpart  <<=   exponent  - 23;
        fracpart =   0;
    } elif (exponent < 0)              // small value (0 for     integer part, decreased fractional part)
    {   intpart  =   0;
        fracpart >>= (-exponent);
    } elif (exponent > 0)              // large value (decreased integer part, increased fractional part)
    {   intpart  >>= (23 - exponent);
        fracpart &=  mask[exponent];
        fracpart <<= exponent;
    } else                             // medium value (0.5..1)
    {   intpart  =   0;
    }
    value =  (double) (fracpart / 8388608.0);
    value += (double) intpart;

    if (memory[start + 1] & 0x80)
    {   value = -value;
    }

    return (float) value;
}

EXPORT void print_float(int start)
{   signed int exponent;
    float      value;

    exponent = memory[start] & 0x7F;
    if (memory[start] & 0x80)
    {   exponent -= 128;
    }
    if (exponent > 31)
    {   if (memory[start + 1] & 0x80)
        {   zprintf(TEXTPEN_CLIOUTPUT, "        ~-2^%d\n", exponent);
        } else
        {   zprintf(TEXTPEN_CLIOUTPUT, "         ~2^%d\n", exponent);
    }   }
    elif (memory[start] == 0x80 && (memory[start + 1] & 0x7F) == 0 && memory[start + 2] == 0 && memory[start + 3] == 0) // +0 or -0
    {   if (memory[start + 1] & 0x80)
        {   zprintf(TEXTPEN_CLIOUTPUT, "         -0.000000\n");
        } else
        {   zprintf(TEXTPEN_CLIOUTPUT, "          0.000000\n");
    }   }
    else
    {   value = basic_to_float(start);

        if (value < 0.000001f)
        {   if (memory[start + 1] & 0x80)
            {   zprintf(TEXTPEN_CLIOUTPUT, "       > -0.000001\n");
            } else
            {   zprintf(TEXTPEN_CLIOUTPUT, "        < 0.000001\n");
        }   }
        else
        {   if (memory[start + 1] & 0x80)
            {   zprintf(TEXTPEN_CLIOUTPUT, "%18f\n", -value);
            } else
            {   zprintf(TEXTPEN_CLIOUTPUT, "%18f\n",  value);
}   }   }   }

EXPORT FLAG zisnormal(int start) // isnormal() is a reserved function
{   if
    (    memory[start    ] == 0x80
     &&  memory[start + 1] == 0x00
     &&  memory[start + 2] == 0x00
     &&  memory[start + 3] == 0x00
    )
    {    return TRUE; // zero is OK
    } elif
    (   (memory[start + 1] >= 0x40 && memory[start + 1] <= 0x7F) // positive
     ||  memory[start + 1] >= 0xC0                               // negative
    )
    {    return TRUE; // OK
    } else
    {    return FALSE;
}   }

EXPORT void float_to_basic(float number, int start)
{   float mantissa,
          value;
    int   i, j,
          mantissa_i;
    UBYTE exponent,
          mantissa1, mantissa2, mantissa3;
    FLAG  neg;

    if (number < 0.0f)
    {   number = -number;
        neg = TRUE;
    } else
    {   neg = FALSE;
    }

    for (i = -65; i <= -1; i++)
    {   value = 1.0f;
        for (j = 1; j <= -i; j++)
        {   value /= 2.0f;
        }
        if (number < value)
        {   if (i == -65)
            {   exponent = 0x80;
                mantissa = 0.0f;
                goto DONE;
            }
            exponent = (UBYTE) i;
            value = 8388608.0f;
            for (j = 1; j <= -i; j++)
            {   value *= 2.0f;
            }
            mantissa = number * value;
            goto DONE;
    }   }
    for (i = 0; i <= 63; i++)
    {   value = 0.5f;
        for (j = 0; j <= i; j++)
        {   value *= 2.0f;
        }
        if (number < value)
        {   exponent = (UBYTE) i;
            value = 16777216.0f;
            for (j = 0; j <= i; j++)
            {   value /= 2.0f;
            }
            mantissa = number * value;
            goto DONE;
    }   }

    zprintf(TEXTPEN_ERROR, "Illegal number!\n");
    return;

DONE:
    mantissa_i = (int) mantissa;
    mantissa1 =  mantissa_i / 65536;
    mantissa2 = (mantissa_i % 65536) / 256;
    mantissa3 = (mantissa_i % 65536) % 256;
    if (neg)
    {   mantissa1 |= 0x80;
        number = -number;
    } else
    {   mantissa1 &= 0x7F;
    }

    zprintf(TEXTPEN_VERBOSE, "Overriding BASIC with $%02X%02X%02X%02X (%f).\n", exponent, mantissa1, mantissa2, mantissa3, number);
    memory[start    ] = exponent;
    memory[start + 1] = mantissa1;
    memory[start + 2] = mantissa2;
    memory[start + 3] = mantissa3;
}

EXPORT void cd2650_reset(void)
{   // keyboard
    queuepos        = 0;
    cd2650_downtill = 0;
    guestkey        = NC;
    hostkey         = (UWORD) -1;

    // screen
    memory[0x17FE]  = 0x10; // $1000
    memory[0x17FF]  = 0x00;
    memory[0x1000]  = 0x5C; // cursor

    reset_drives();
    if (cd2650_dosver == DOS_P1DOS)
    {   memory[0x60BE] = cd2650_userdrives - 1;
    }

    // game
    if (game)
    {   iar = (startaddr_h * 256) + startaddr_l;
    } else
    {   iar = 0;
}   }

EXPORT void cd2650_serialize_cos(void)
{   int i,
        whichdrive;

    for (i = 0x3FF; i <= 0x7FFF; i++)
    {   serialize_byte(&memory[i]);
    }
    if (serializemode == SERIALIZE_READ)
    {   for (whichdrive = 0; whichdrive < machines[machine].drives; whichdrive++)
        {   for (i = 0; i < CD2650_DISKSIZE / 8; i++)
            {   drive[whichdrive].flags[i] = 0;
    }   }   }
    serialize_byte_int(&cd2650_biosver);
    serialize_byte_int(&cd2650_vdu);
    for (whichdrive = 0; whichdrive < 4; whichdrive++)
    {   serialize_byte((UBYTE*) &drive[whichdrive].inserted);
        if (drive[whichdrive].inserted)
        {   for (i = 0; i < CD2650_DISKSIZE; i++)
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
    serialize_byte_int(&cd2650_userdrives);
    if (serializemode == SERIALIZE_READ && cd2650_dosver == DOS_P1DOS)
    {   memory[0x60BE] = cd2650_userdrives - 1;
    }
    serialize_byte((UBYTE*) &sectordone);
    serialize_long((ULONG*) &timeoutat);
    if (cosversion >= 43)
    {   serialize_byte_int(&belling[0]);
    } else
    {   belling[0] = 0;
    }

    if (serializemode == SERIALIZE_READ)
    {   for (whichdrive = 0; whichdrive < machines[machine].drives; whichdrive++)
        {   drive[whichdrive].fn_disk[0] = EOS;
            cd2650_dir_disk(TRUE, whichdrive);
}   }   }

MODULE void cd2650_runcpu(void)
{   FAST ULONG endcycle;

    // assert(slice_2650 >= 1);

    endcycle = cycles_2650 + slice_2650;
    if (endcycle < cycles_2650)
    {   // cycle counter will overflow, so we need to use the slow method
        while (slice_2650 >= 1)
        {   oldcycles = cycles_2650;
            checkstep();
            do_tape();
            one_instruction();
            slice_2650 -= (cycles_2650 - oldcycles);
    }   }
    else
    {   // cycle counter will not overflow, so we can use a faster method
        oldcycles = cycles_2650;
        while (cycles_2650 < endcycle)
        {   checkstep();
            do_tape();
            one_instruction();
        }
        slice_2650 -= (cycles_2650 - oldcycles);
}   }

MODULE void draw_cd2650(void)
{   FAST UBYTE t;
    FAST int   x1, x2,
               y1, y2;

    for (y1 = 0; y1 < 16; y1++)
    {   for (y2 = 0; y2 < 10; y2++)
        {   for (x1 = 0; x1 < 80; x1++)
            {
#ifdef SHOWCHARSET
                if (x1 < 16 && y1 < 16) t = (y1 * 16) + x1; else
#endif
                t = memory[0x1000 + (x1 * 16) + y1] & 0x7F;
                for (x2 = 0; x2 < 8; x2++)
                {   if (cd2650_chars_bmp[t][y2] & (0x80 >> x2))
                    {   changepixel(  (x1 * 8) + x2, (y1 * 12) + y2, vdu_fgc);
                    } else
                    {   changebgpixel((x1 * 8) + x2, (y1 * 12) + y2, vdu_bgc);
        }   }   }   }
        for (y2 = 10; y2 < 12; y2++)
        {   for (x1 = 0; x1 < 640; x1++)
            {   changebgpixel(x1, (y1 * 12) + y2, vdu_bgc);
}   }   }   }

EXPORT void cd2650_inject_file(STRPTR thefilename)
{   int   contiguous,
          i, j,
          startblock = 0, // initialized to avoid a spurious SAS/C warning
          endblock,
          thelength,
          thesize,
          whichfile;
    TEXT  filepart[8 + 1],
          extpart[4 + 1];
    FILE* TheLocalHandle /* = NULL */ ;

    cd_projects();

    for (i = 0; i < 8; i++)
    {   if (thefilename[i] == '.' || thefilename[i] == EOS)
        {   goto DONE1;
        }
        if
        (   (thefilename[i] >= '0' && thefilename[i] <= '9')
         || (thefilename[i] >= 'A' && thefilename[i] <= 'Z')
         || (thefilename[i] >= 'a' && thefilename[i] <= 'z')
        )
        {   ; // OK
        } else
        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_ILLEGALFILENAME, "Illegal character '%c' in filename!\n\n"), thefilename[i]);
            goto END;
    }   }
    if (thefilename[8] != '.' && thefilename[8] != EOS)
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_FILENAMEISTOOLONG, "Filename is too long!\n\n"));
        goto END;
    }

DONE1:
    zstrncpy(filepart, thefilename, i);

    if (thefilename[i] == '.')
    {   for (j = 1; j <= 4; j++)
        {   if (thefilename[i + j] == EOS)
            {   zstrncpy(extpart, &thefilename[i + 1], j);
                goto DONE2;
            }
            if
            (   (thefilename[i + j] >= '0' && thefilename[i + j] <= '9')
             || (thefilename[i + j] >= 'A' && thefilename[i + j] <= 'Z')
             || (thefilename[i + j] >= 'a' && thefilename[i + j] <= 'z')
             ||  thefilename[i + j] == '$'
            )
            {   ; // OK
            } else
            {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_ILLEGALEXTENSION, "Illegal character '%c' in extension!\n\n"), thefilename[i + j]);
                goto END;
        }   }
        if (thefilename[i + 4 + 1] != EOS)
        {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_EXTENSIONISTOOLONG, "Extension is too long!\n\n"));
            goto END;
        }
        zstrncpy(extpart, &thefilename[i + 1], j);
    } else
    {   extpart[0] = EOS;
    }

DONE2:
    thesize = getsize(thefilename);
    if (thesize > CD2650_DISKSIZE - usedsize - (18 * CD2650_BLOCKSIZE))
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_DISKISTOOFULL, "Disk is too full!\n\n"));
        goto END;
    }

    whichfile = -1;
    for (i = 0; i < 64; i++)
    {   if (drive[debugdrive].filename[i][0] != EOS)
        {   drive[debugdrive].filename[i][8] = EOS; // to avoid a spurious GCC warning
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

    contiguous = 0;
    for (i = 18; i < CD2650_DISKBLOCKS; i++)
    {   if (drive[debugdrive].bam[i] == BAM_FREE)
        {   if (contiguous == 0)
            {   startblock = i;
            }
            contiguous++;
            if (contiguous >= thesize / CD2650_BLOCKSIZE)
            {   endblock = i;
                goto DONE3;
        }   }
        else
        {   contiguous = 0;
    }   }
    zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_NOTCONTIGUOUS, "Not enough contiguous free blocks!\n\n"));
    goto END;

DONE3:
    if (!(TheLocalHandle = fopen(thefilename, "rb")))
    {   zprintf(TEXTPEN_CLIOUTPUT, LLL(MSG_CANTOPENFILEFORINPUT, "Can't open file \"%s\" for input!\n\n"), thefilename);
        goto END;
    }
    DISCARD fread(&drive[debugdrive].contents[startblock * CD2650_BLOCKSIZE], thesize, 1, TheLocalHandle);
    DISCARD fclose(TheLocalHandle);
    // TheLocalHandle = NULL;

    if (thesize < (endblock - startblock + 1) * CD2650_BLOCKSIZE)
    {   memset(&drive[debugdrive].contents[(startblock * CD2650_BLOCKSIZE) + thesize], 0, ((endblock - startblock + 1) * CD2650_BLOCKSIZE) - thesize) ; // clear unused area
    }

    memset(&drive[debugdrive].contents[whichfile * 64], 0, 64); // filename and extension in particular need clearing
    thelength = strlen(filepart);
    for (i = 0; i < thelength; i++)
    {   drive[debugdrive].contents[(whichfile * 64)     + i] = toupper(filepart[i]);
    }
    thelength = strlen(extpart);
    for (i = 0; i < thelength; i++)
    {   drive[debugdrive].contents[(whichfile * 64) + 8 + i] = toupper(extpart[i]);
    }
    drive[debugdrive].contents[(whichfile * 64) + 20] = 0; // public
    drive[debugdrive].contents[(whichfile * 64) + 28] =  startblock / CD2650_SECTORS     ; // block -> track
    drive[debugdrive].contents[(whichfile * 64) + 29] = (startblock % CD2650_SECTORS) + 1; // block -> sector
    drive[debugdrive].contents[(whichfile * 64) + 30] =  endblock   / CD2650_SECTORS     ; // block -> track
    drive[debugdrive].contents[(whichfile * 64) + 31] = (endblock   % CD2650_SECTORS) + 1; // block -> sector
    drive[debugdrive].contents[(whichfile * 64) + 32] = 0; // modifier

    read_rtc();
    drive[debugdrive].contents[(whichfile * 64) + 40] = drive[debugdrive].contents[(whichfile * 64) + 48] = '0' +  (rtc.month /  10)      ; // tens of month
    drive[debugdrive].contents[(whichfile * 64) + 41] = drive[debugdrive].contents[(whichfile * 64) + 49] = '0' +  (rtc.month %  10)      ; // ones of month
    drive[debugdrive].contents[(whichfile * 64) + 42] = drive[debugdrive].contents[(whichfile * 64) + 50] = '-';
    drive[debugdrive].contents[(whichfile * 64) + 43] = drive[debugdrive].contents[(whichfile * 64) + 51] = '0' +  (rtc.day   /  10)      ; // tens of day
    drive[debugdrive].contents[(whichfile * 64) + 44] = drive[debugdrive].contents[(whichfile * 64) + 52] = '0' +  (rtc.day   %  10)      ; // ones of day
    drive[debugdrive].contents[(whichfile * 64) + 45] = drive[debugdrive].contents[(whichfile * 64) + 53] = '-';
    drive[debugdrive].contents[(whichfile * 64) + 46] = drive[debugdrive].contents[(whichfile * 64) + 54] = '0' + ((rtc.year  % 100) / 10); // tens of year
    drive[debugdrive].contents[(whichfile * 64) + 47] = drive[debugdrive].contents[(whichfile * 64) + 55] = '0' + ((rtc.year  % 100) % 10); // ones of year

    zprintf(TEXTPEN_CLIOUTPUT, "%s.\n\n", LLL(MSG_OK, "OK"));
    cd2650_dir_disk(FALSE, debugdrive); // we only need a quiet one, the loud one is done just as a courtesy

END:
    cd_progdir();
}

MODULE void run_cpu(int until)
{   // This is a quicker equivalent to repeatedly incrementing cpux and calling do_cpu().

    cpux = nextinst;
    while (cpux < until)
    {   oldcycles = cycles_2650;
        checkstep();
        do_tape();
        one_instruction();
        nextinst += (cycles_2650 - oldcycles) * (fastcd2650 ? 8 : 12); // in pixels
        cpux = nextinst;
    }
    if (nextinst >= 904)
    {   nextinst -= 904;
}   }
