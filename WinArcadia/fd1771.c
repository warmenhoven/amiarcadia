// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
    #include <string.h>
    #include <stdlib.h> // eg. for rand()
    #include <proto/locale.h> // GetCatalogStr()
#endif
#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
#endif

#include <stdio.h>

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"

/* DEFINES----------------------------------------------------------------

(None)

EXPORTED VARIABLES----------------------------------------------------- */

EXPORT FLAG                 multimode;
EXPORT UBYTE                drivedata,
                            drivestatus,
                            reqsector,
                            trackreg;
EXPORT ULONG                curdrive,
                            timeoutat;
EXPORT int                  drive_idmode,
                            stepdir;

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT FLAG                 outofrange,
                            sectordone;
IMPORT TEXT                 asciiname_short[259][3 + 1];
IMPORT UWORD                iar,
                            oldiar;
IMPORT ULONG                cycles_2650,
                            frames,
                            region,
                            verbosedisk;
IMPORT int                  ambient,
                            cd2650_dosver,
                            cpuy,
                            diskbyte,
                            drive_mode,
                            log_illegal,
                            machine,
                            watchreads,
                            watchwrites;
IMPORT struct DriveStruct   drive[DRIVES_MAX];
IMPORT struct MachineStruct machines[MACHINES];
#ifdef WIN32
    IMPORT int              CatalogPtr;
#endif
#ifdef AMIGA
    IMPORT struct Catalog*  CatalogPtr;
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE int                  lasttype,    // ideally we should store...
                            lastsubtype; // ...these in COS/COR files

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void fd_type1(UBYTE data, FLAG full);
MODULE void fd_type2(UBYTE data, FLAG full);
MODULE void setdrivestatus(UBYTE value, int type, int subtype);
MODULE void type1_head(UBYTE data);
MODULE void type2_head(UBYTE data);

// CODE-------------------------------------------------------------------

// Type I Commands--------------------------------------------------------

EXPORT void fd1771_restore(UBYTE data) // aka seek to track 0
{   RESETTIMEOUT;

    if (verbosedisk)
    {   zprintf(TEXTPEN_LOG, "Restore command ($%02X):\n", data);
        fd_type1(data, FALSE);
    }

    type1_head(data);

    if (drive[curdrive].track)
    {   if (ambient)
        {   play_sample(SAMPLE_STEP);
        }
        // stepdir is not modified by restores
        // if (!turbodisk) delay 12/20/40 ms (per step)
    }
    trackreg = drive[curdrive].track = 0;

    setdrivestatus(0x00, 1, 1);
}

EXPORT void fd1771_seek(UBYTE data) // aka seek to track n
{   RESETTIMEOUT;

    if (verbosedisk)
    {   zprintf(TEXTPEN_LOG, "Seek command ($%02X):\n", data);
        fd_type1(data, FALSE);
        zprintf(TEXTPEN_LOG, " Desired track:           %d\n", drivedata);
        if (trackreg != drive[curdrive].track)
        {   zprintf(TEXTPEN_LOG, "Track register ($%02X) does not agree with actual head position ($%02X)!\n", trackreg, drive[curdrive].track);
    }   }

    type1_head(data);

    drive[curdrive].track = trackreg = drivedata; // it works even if the drive is empty
    if (drive[curdrive].track != drivedata)
    {   if (ambient)
        {   play_sample(SAMPLE_STEP);
        }
        // stepdir is not modified by seeks
        // if (!turbodisk) delay 12/20/40 ms (per step)
    }

    setdrivestatus(0x00, 1, 1);
}

EXPORT void fd1771_step(UBYTE data) // step in/out (once)
{   RESETTIMEOUT;

    if (verbosedisk)
    {   zprintf(TEXTPEN_LOG, "Step command ($%02X):\n", data);
        fd_type1(data, TRUE);
        if (trackreg != drive[curdrive].track)
        {   zprintf(TEXTPEN_LOG, "Track register ($%02X) does not agree with actual head position ($%02X)!\n", trackreg, drive[curdrive].track);
    }   }

    if (stepdir == 0)
    {   if (verbosedisk)
        {   zprintf(TEXTPEN_LOG, "Code at $%X wants to step in an undefined direction!\n\n", iar);
        }
        stepdir = ((rand() % 2) * 2) - 1; // -1 or +1. We should really put this into COR files for reliable playback
    }

    type1_head(data);

    if (stepdir == 1) // up/inwards
    {   if (drive[curdrive].track < ((machine == BINBUG) ? BINBUG_TRACKS : CD2650_TRACKS) - 1)
        {   drive[curdrive].track++;
            if (ambient)
            {   play_sample(SAMPLE_STEP);
        }   }
        elif (log_illegal)
        {   zprintf(TEXTPEN_LOG, "Code at $%X attempted to step above track %d!\n\n", iar, drive[curdrive].track);
        }
        if (data & 0x10)
        {   trackreg++; // overflow is OK
    }   }
    elif (stepdir == -1) // down/outwards
    {   if (drive[curdrive].track > 0)
        {   drive[curdrive].track--;
            if (ambient)
            {   play_sample(SAMPLE_STEP);
            }
            if (data & 0x10)
            {   trackreg--; // underflow is OK
        }   }
        elif (log_illegal)
        {   zprintf(TEXTPEN_LOG, "Code at $%X attempted to step below track zero!\n\n", iar);
    }   }

    setdrivestatus(0x00, 1, 1);
    // if (!turbodisk) delay 12/20/40 ms
}

EXPORT void fd1771_step_in(UBYTE data) // aka step up (once)
{   RESETTIMEOUT;

    if (verbosedisk)
    {   zprintf(TEXTPEN_LOG, "Step In command ($%02X):\n", data);
        fd_type1(data, TRUE);
        if (trackreg != drive[curdrive].track)
        {   zprintf(TEXTPEN_LOG, "Track register ($%02X) does not agree with actual head position ($%02X)!\n", trackreg, drive[curdrive].track);
    }   }

    type1_head(data);

    stepdir = 1; // up/inwards

    if (drive[curdrive].track < ((machine == BINBUG) ? BINBUG_TRACKS : CD2650_TRACKS) - 1)
    {   drive[curdrive].track++;
        if (ambient)
        {   play_sample(SAMPLE_STEP);
    }   }
    elif (log_illegal)
    {   zprintf(TEXTPEN_LOG, "Code at $%X attempted to step above track %d!\n\n", iar, drive[curdrive].track);
    }
    if (data & 0x10)
    {   trackreg++; // overflow is OK
    }

    setdrivestatus(0x00, 1, 1);
    // if (!turbodisk) delay 12/20/40 ms
}

EXPORT void fd1771_step_out(UBYTE data) // aka step down (once)
{   RESETTIMEOUT;

    if (verbosedisk)
    {   zprintf(TEXTPEN_LOG, "Step Out command ($%02X):\n", data);
        fd_type1(data, TRUE);
        if (trackreg != drive[curdrive].track)
        {   zprintf(TEXTPEN_LOG, "Track register ($%02X) does not agree with actual head position ($%02X)!\n", trackreg, drive[curdrive].track);
    }   }

    type1_head(data);

    stepdir = -1; // down/outwards

    if (drive[curdrive].track > 0)
    {   drive[curdrive].track--;
        if (ambient)
        {   play_sample(SAMPLE_STEP);
        }
        if (data & 0x10)
        {   trackreg--; // underflow is OK
        }
        // if (!turbodisk) delay 12/20/40 ms
    } elif (log_illegal)
    {   zprintf(TEXTPEN_LOG, "Code at $%X attempted to step below track zero!\n\n", iar);
    }

    setdrivestatus(0x00, 1, 1);
    // if (!turbodisk) delay 12/20/40 ms
}

// Type II Commands-------------------------------------------------------

EXPORT void fd1771_read_sector(UBYTE data)
{   RESETTIMEOUT;

    if (verbosedisk)
    {   zprintf(TEXTPEN_LOG, "Read Sector command:\n");
        fd_type2(data, FALSE);
        zprintf(TEXTPEN_LOG, " Track register:          %d\n", trackreg);
        if (trackreg != drive[curdrive].track)
        {   zprintf(TEXTPEN_LOG, "Track register ($%02X) does not agree with actual head position ($%02X)!\n", trackreg, drive[curdrive].track);
        }
        zprintf(TEXTPEN_LOG, " Desired sector:          %d\n", reqsector);
    }

    drive_idmode = 0;
    type2_head(data); // load head if requested
    multimode = (data & 0x10) ? TRUE : FALSE;
    set_drive_mode(DRIVEMODE_READING);
    drive[curdrive].blockoffset = 0;
    drive[curdrive].sector      = reqsector;
    if (drive[curdrive].inserted)
    {   get_disk_byte(curdrive, TRUE);
        if (outofrange)
        {   setdrivestatus(0x10, 2, 1); // ready, record not found, no DRQ, not busy
        } else
        {   if (ambient)
            {   play_sample(SAMPLE_GRIND);
            }
            setdrivestatus(0x02, 2, 1); // ready, record found, DRQ, not busy. Should be $03 (ie. busy) but that stops Adventure game from working
    }   }
    else
    {   setdrivestatus(0x80, 2, 1); // not ready, no DRQ, not busy
}   }

EXPORT void fd1771_write_sector(UBYTE data)
{   RESETTIMEOUT;

    if (verbosedisk)
    {   zprintf(TEXTPEN_LOG, "Write Sector command:\n");
        fd_type2(data, TRUE);
        zprintf(TEXTPEN_LOG, " Track register:          %d\n", trackreg);
        if (trackreg != drive[curdrive].track)
        {   zprintf(TEXTPEN_LOG, "Track register ($%02X) does not agree with actual head position ($%02X)!\n", trackreg, drive[curdrive].track);
        }
        zprintf(TEXTPEN_LOG, " Desired sector:          %d\n", drive[curdrive].sector);
        zprintf(TEXTPEN_LOG, " Desired data:            %d\n", drivedata);
    }

    type2_head(data); // load head if requested
    multimode = (data & 0x10) ? TRUE : FALSE;
    set_drive_mode(DRIVEMODE_WRITING);
    drive[curdrive].blockoffset = 0;
    drive[curdrive].sector      = reqsector;
    if (drive[curdrive].inserted)
    {   get_disk_byte(curdrive, TRUE);
        if (outofrange)
        {   setdrivestatus(0x10, 2, 2); // ready, record not found, no DRQ, not busy
        } else
        {   if (ambient)
            {   play_sample(SAMPLE_GRIND);
            }
            if (drive[curdrive].writeprotect)
            {   setdrivestatus(0x40, 2, 2); // write protected, record found, no DRQ, not busy
            } else
            {   setdrivestatus(0x02, 2, 2); // write enabled, record found, DRQ, busy. // ready, record found, DRQ, not busy. Should be $03 (ie. busy) but that might stop Adventure game from working
    }   }   }
    else
    {   setdrivestatus(0x80, 2, 2); // not ready, no DRQ, not busy
}   }

EXPORT UBYTE fd1771_read_datareg(void)
{   FAST UBYTE t;

    if
    (   drive_idmode
     && drive[curdrive].inserted
     && drive[curdrive].headloaded
     && drive_mode == DRIVEMODE_READING
    )
    {   switch (drive_idmode)
        {
        case  1: t = drive[curdrive].track;
                 if (verbosedisk) zprintf(TEXTPEN_LOG, "Returning track number");
        acase 2: t = 0; // side number
                 if (verbosedisk) zprintf(TEXTPEN_LOG, "Returning side number");
        acase 3: t = drive[curdrive].sector;
                 if (verbosedisk) zprintf(TEXTPEN_LOG, "Returning sector number");
        acase 4: t = ((machine == BINBUG) ? (BINBUG_BLOCKSIZE) : (CD2650_BLOCKSIZE)) / 16;
                 if (verbosedisk) zprintf(TEXTPEN_LOG, "Returning sector length");
        acase 5: t = 0; // CRC 1
                 if (verbosedisk) zprintf(TEXTPEN_LOG, "Returning 1st CRC");
        acase 6: t = 0; // CRC 2
                 if (verbosedisk) zprintf(TEXTPEN_LOG, "Returning 2nd CRC");
        }
        if (verbosedisk)
        {   zprintf(TEXTPEN_LOG, " ($%02X).\n", t);
        }
        if (drive_idmode == 6)
        {   drive_idmode = 0;
        } else
        {   drive_idmode++;
        }
        return t;
    }

    RESETTIMEOUT;

    get_disk_byte(curdrive, TRUE);
    if
    (   drive[curdrive].inserted
     && drive[curdrive].spinning
     && drive[curdrive].headloaded
     && drive_mode == DRIVEMODE_READING
     && !outofrange
    )
    {   t = drive[curdrive].contents[diskbyte];
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
                (   MSG_READFLOPPY,
                    "Instruction at $%X (raster %d) read $%02X [%s] from floppy byte $%X. Previous IAR/PC was $%X.\n\n"
                ),
                (int) iar,
                (int) cpuy,
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
        if (drive[curdrive].blockoffset == 255)
        {   drive[curdrive].blockoffset = 0;
            sectordone = TRUE; // only important for CD2650
            if (drive[curdrive].sector == ((machine == BINBUG) ? BINBUG_SECTORS : CD2650_SECTORS)) // always stops at index pulse even in multimode
            {   drive[curdrive].sector = 1;
                set_drive_mode(DRIVEMODE_IDLE);
                setdrivestatus(0x02, 2, 1); // ready, record found, DRQ, not busy
                sound_stop(GUESTCHANNELS + SAMPLE_GRIND);
            } else
            {   drive[curdrive].sector++;
                if (multimode)
                {   setdrivestatus(0x03, 2, 1); // ready, record found, DRQ, busy
                } else
                {   set_drive_mode(DRIVEMODE_IDLE);
                    setdrivestatus(0x02, 2, 1); // ready, record found, DRQ, not busy
                    sound_stop(GUESTCHANNELS + SAMPLE_GRIND);
        }   }   }
        else
        {   drive[curdrive].blockoffset++;
            sectordone = FALSE; // only important for CD2650
            setdrivestatus(0x03, 2, 1); // ready, record found, DRQ, busy
        }
        get_disk_byte(curdrive, FALSE);
        drive[curdrive].viewstart = diskbyte;
        update_floppydrive(1, curdrive);
    } elif (drive_mode != DRIVEMODE_READING)
    {   if (verbosedisk)
        {   zprintf(TEXTPEN_ERROR, "Reading data register in wrong mode!\n");
        }
        t = 0; // really should return whatever was the last byte read/written
        // don't update status bits
    } elif (outofrange)
    {   t = 0;
        setdrivestatus(0x10, 2, 1); // ready, record not found, no DRQ
        if (verbosedisk)
        {   zprintf(TEXTPEN_ERROR, "Reading from invalid track %d and/or sector %d!\n", drive[curdrive].track, drive[curdrive].sector);
    }   }
    else
    {   t = 0;
        setdrivestatus(0x80, 2, 1); // not ready, no DRQ
        if (verbosedisk)
        {   if (!drive[curdrive].spinning)
            {   zprintf(TEXTPEN_ERROR, "Reading from non-spinning disk!\n");
            }
            if (!drive[curdrive].inserted)
            {   zprintf(TEXTPEN_ERROR, "Reading from empty drive!\n");
            }
            if (!drive[curdrive].headloaded)
            {   zprintf(TEXTPEN_ERROR, "Reading without head loaded!\n");
    }   }   }

    return t;
}

EXPORT void fd1771_write_datareg(UBYTE data)
{   RESETTIMEOUT;

    drivedata = data;
    if (drive_mode != DRIVEMODE_WRITING)
    {   if (verbosedisk)
        {   zprintf(TEXTPEN_LOG, "Code at $%X wrote $%02X to FD1771 data register.\n", iar, data);
        }
        return;
    }

    get_disk_byte(curdrive, TRUE);
    if
    (   drive[curdrive].inserted
     && drive[curdrive].spinning
     && drive[curdrive].headloaded
    )
    {   if
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
                (   MSG_WROTEFLOPPY,
                    "Instruction at $%X (raster %d) wrote $%02X [%s] to floppy byte $%X. Previous IAR/PC was $%X.\n\n"
                ),
                (int) iar,
                (int) cpuy,
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
        if (drive[curdrive].blockoffset == 255)
        {   drive[curdrive].blockoffset = 0;
            sectordone = TRUE; // only important for CD2650
            if (drive[curdrive].sector == ((machine == BINBUG) ? BINBUG_SECTORS : CD2650_SECTORS)) // always stops at index pulse even in multimode
            {   drive[curdrive].sector = 1;
                set_drive_mode(DRIVEMODE_IDLE);
                setdrivestatus(0x02, 2, 1); // ready, record found, DRQ, not busy
                sound_stop(GUESTCHANNELS + SAMPLE_GRIND);
            } else
            {   drive[curdrive].sector++;
                if (multimode)
                {   setdrivestatus(0x03, 2, 1); // ready, record found, DRQ, busy
                } else
                {   set_drive_mode(DRIVEMODE_IDLE);
                    setdrivestatus(0x02, 2, 1); // ready, record found, DRQ, not busy
                    sound_stop(GUESTCHANNELS + SAMPLE_GRIND);
        }   }   }
        else
        {   drive[curdrive].blockoffset++;
            sectordone = FALSE; // only important for CD2650
            setdrivestatus(0x03, 2, 1); // ready, record found, DRQ, busy
        }
        get_disk_byte(curdrive, FALSE);
        drive[curdrive].viewstart = diskbyte;
        update_floppydrive(1, curdrive);
    } elif (outofrange)
    {   setdrivestatus(0x10, 2, 1); // ready, record not found, no DRQ
        if (verbosedisk)
        {   zprintf(TEXTPEN_ERROR, "Writing to invalid track %d and/or sector %d!\n", drive[curdrive].track, drive[curdrive].sector);
    }   }
    else
    {   setdrivestatus(0x80, 2, 2); // not ready, no DRQ
        if (verbosedisk)
        {   if (!drive[curdrive].spinning)
            {   zprintf(TEXTPEN_ERROR, "Writing to non-spinning disk!\n");
            }
            if (!drive[curdrive].inserted)
            {   zprintf(TEXTPEN_ERROR, "Writing to empty drive!\n");
            }
            if (!drive[curdrive].headloaded)
            {   zprintf(TEXTPEN_ERROR, "Writing without head loaded!\n");
}   }   }   }

EXPORT UBYTE fd1771_read_controlreg(void)
{   UBYTE t;

    if (machine == CD2650)
    {   t = sectordone ? 0x40 : 0x80;
        sectordone = FALSE;
    } else
    {   // assert(machine == BINBUG);
        t = 0x80;
    }

#ifdef VERBOSEDISKCONTROL
    if (verbosedisk)
    {   zprintf(TEXTPEN_LOG, "Code at $%X read $%02X from control register.\n", iar, t);
    }
#endif

    return t;
}

// Type III Commands------------------------------------------------------

EXPORT void fd1771_read_address(UBYTE data)
{   RESETTIMEOUT;

    if (verbosedisk)
    {   // assert(data == 0xC4);
        zprintf(TEXTPEN_LOG, "Read Address command ($%02X).\n", data); // CD DOS & P1 DOS call this during initialization, to query the current track of the current drive
    }

    drive[curdrive].headloaded = TRUE;
    set_drive_mode(DRIVEMODE_READING);
    if (drive[curdrive].inserted)
    {   drive_idmode = 1;
        reqsector = drive[curdrive].sector;
        setdrivestatus(0x00, 3, 1);
    } else
    {   drive_idmode = 0;
        setdrivestatus(0x80, 3, 1); // not ready
    }
    /* Bits 6..5 are always 0
       Bit  4    is  ID not found
       Bit  3    is  CRC error
       Bit  2    is  Lost data
       Bit  1    is  DRQ
       Bit  0    is  Busy */
}

EXPORT void fd1771_read_track(UBYTE data)
{   RESETTIMEOUT;

    zprintf(TEXTPEN_LOG, "Read Track command ($%02X) (unimplemented!):\n", data);
    zprintf(TEXTPEN_LOG, " Synchronize to AM?      "); noyes((UBYTE) (data & 1));

    setdrivestatus(0x00, 3, 2);
}

EXPORT void fd1771_write_track(UBYTE data)
{   RESETTIMEOUT;

    zprintf(TEXTPEN_LOG, "Write Track command ($%02X) (unimplemented!)\n", data);

    setdrivestatus(0x00, 3, 3);
}

// Type IV Command--------------------------------------------------------

EXPORT void fd1771_force_interrupt(UBYTE data)
{   RESETTIMEOUT;

    if (verbosedisk)
    {   zprintf(TEXTPEN_LOG, "Force Interrupt command ($%02X):\n", data);
        zprintf(TEXTPEN_LOG, " Not Ready to Ready transition?"); yesno((UBYTE) (data & 1));
        zprintf(TEXTPEN_LOG, " Ready to Not Ready transition?"); yesno((UBYTE) (data & 2));
        zprintf(TEXTPEN_LOG, " Index pulse?                  "); yesno((UBYTE) (data & 4));
        zprintf(TEXTPEN_LOG, " Immediate interrupt?          "); yesno((UBYTE) (data & 8));
    }

    if (!(drivestatus & 1))
    {   // assert(drive_mode = DRIVEMODE_IDLE);
        setdrivestatus(0x20, 1, 1);
    } else
    {   drive_mode = DRIVEMODE_IDLE;
        setdrivestatus((UBYTE) (drivestatus & 0xFE), 1, 1);
        update_floppydrive(2, curdrive);
}   }

EXPORT void fd1771_write_trackreg(UBYTE data)
{   if (verbosedisk)
    {   zprintf(TEXTPEN_LOG, "Code at $%X changed track register from %d to %d.\n", iar, trackreg, data);
        if (data >= ((machine == BINBUG) ? BINBUG_TRACKS : CD2650_TRACKS))
        {   zprintf(TEXTPEN_LOG, "Track register is out of range!\n");
    }   }
    trackreg = data;
}

EXPORT void fd1771_write_reqsector(UBYTE data)
{   if (verbosedisk)
    {   zprintf(TEXTPEN_LOG, "Code at $%X changed desired sector from %d to %d.\n", iar, reqsector, data);
        if (data < 1 || data > ((machine == BINBUG) ? 10 : 9))
        {   zprintf(TEXTPEN_LOG, "Desired sector is out of range!\n");
    }   }
    reqsector = data;
}

MODULE void fd_type1(UBYTE data, FLAG full)
{   zprintf(TEXTPEN_LOG, " Load head at beginning? "); yesno((UBYTE) (data & 8));
    zprintf(TEXTPEN_LOG, " Verify on last track?   "); yesno((UBYTE) (data & 4));
    zprintf(TEXTPEN_LOG, " Stepping motor rate:     "); // this is no mistake
    switch (data & 3)
    {
    case  0: case 1: zprintf(TEXTPEN_LOG, "12");
    acase 2:         zprintf(TEXTPEN_LOG, "20");
    acase 3:         zprintf(TEXTPEN_LOG, "40");
    }
    zprintf(TEXTPEN_LOG, " ms between steps\n");
    if (full)
    {   zprintf(TEXTPEN_LOG, " Update track register?   "); yesno((UBYTE) (data & 16));
}   }

MODULE void fd_type2(UBYTE data, FLAG full)
{   zprintf(TEXTPEN_LOG, " Multiple records?       "); yesno((UBYTE) (data & 16));
    zprintf(TEXTPEN_LOG, " IBM block length?       "); yesno((UBYTE) (data & 8));
    zprintf(TEXTPEN_LOG, " HDL, HLT, 10 msec delay?"); yesno((UBYTE) (data & 4));
    if (full)
    {   zprintf(TEXTPEN_LOG, " Data address mark:       $F");
        switch (data & 3)
        {
        case  0: zprintf(TEXTPEN_LOG, "B\n");
        acase 1: zprintf(TEXTPEN_LOG, "A\n");
        acase 2: zprintf(TEXTPEN_LOG, "9\n");
        acase 3: zprintf(TEXTPEN_LOG, "8\n");
}   }   }

EXPORT UBYTE fd1771_read_status(void)
{   if (verbosedisk)
    {   zprintf(TEXTPEN_LOG, "Code at $%X read $%02X from status register:\n", iar, drivestatus);
        showdrivestatus();
    }
    return drivestatus;
}

MODULE void setdrivestatus(UBYTE value, int type, int subtype)
{   if (type == 1)
    {   if (!drive[curdrive].inserted                                      ) value |= 0x80; // not ready
        if (drive[curdrive].headloaded                                     ) value |= 0x20; // head loaded
        if (drive[curdrive].track  == 0                                    ) value |= 0x04; // hardware sensor for track 0
        if (drive[curdrive].sector == 1 && drive[curdrive].blockoffset == 0) value |= 0x02; // hardware sensor for index pulse
    }

    drivestatus = value;
    lasttype    = type;
    lastsubtype = subtype;

    if (!verbosedisk)
    {   return;
    }

    zprintf(TEXTPEN_LOG, "Setting status register to $%02X:\n", value);
    showdrivestatus();
}

EXPORT void showdrivestatus(void)
{   switch (lasttype)
    {
    case 1:
        zprintf(    TEXTPEN_LOG, " Bit 7: Not ready?       "); yesno((UBYTE) (drivestatus & 0x80));
        zprintf(    TEXTPEN_LOG, " Bit 6: Write protected? "); yesno((UBYTE) (drivestatus & 0x40));
        zprintf(    TEXTPEN_LOG, " Bit 5: Head engaged?    "); yesno((UBYTE) (drivestatus & 0x20));
        zprintf(    TEXTPEN_LOG, " Bit 4: Seek error?      "); yesno((UBYTE) (drivestatus & 0x10));
        zprintf(    TEXTPEN_LOG, " Bit 3: CRC error?       "); yesno((UBYTE) (drivestatus & 0x08));
        zprintf(    TEXTPEN_LOG, " Bit 2: Track zero?      "); yesno((UBYTE) (drivestatus & 0x04));
        zprintf(    TEXTPEN_LOG, " Bit 1: Index?           "); yesno((UBYTE) (drivestatus & 0x02));
        zprintf(    TEXTPEN_LOG, " Bit 0: Busy?            "); yesno((UBYTE) (drivestatus & 0x01));
    acase 2: // Read Sector, Write Sector
        zprintf(    TEXTPEN_LOG, " Bit 7: Not ready?       "); yesno((UBYTE) (drivestatus & 0x80));
        if (lastsubtype == 1) // Read Sector
        {   zprintf(TEXTPEN_LOG, "  6..5: Record type:      %d\n", (drivestatus & 0x60) >> 5);
        } else
        {   // assert(lastsubtype == 2); // Write Sector
            zprintf(TEXTPEN_LOG, " Bit 6: Write protected? "); yesno((UBYTE) (drivestatus & 0x40));
            zprintf(TEXTPEN_LOG, " Bit 5: Write fault?     "); yesno((UBYTE) (drivestatus & 0x20));
        }
        zprintf(    TEXTPEN_LOG, " Bit 4: Record not found?"); yesno((UBYTE) (drivestatus & 0x10));
        zprintf(    TEXTPEN_LOG, " Bit 3: CRC error?       "); yesno((UBYTE) (drivestatus & 0x08));
        zprintf(    TEXTPEN_LOG, " Bit 2: Lost data?       "); yesno((UBYTE) (drivestatus & 0x04));
        zprintf(    TEXTPEN_LOG, " Bit 1: DRQ?             "); yesno((UBYTE) (drivestatus & 0x02));
        zprintf(    TEXTPEN_LOG, " Bit 0: Busy?            "); yesno((UBYTE) (drivestatus & 0x01));
    acase 3: // Read Address, Read Track, Write Track
        zprintf(    TEXTPEN_LOG, " Bit 7: Not ready?       "); yesno((UBYTE) (drivestatus & 0x80));
        if (lastsubtype == 3) // Write Track
        {   zprintf(TEXTPEN_LOG, " Bit 6: Write protected? "); yesno((UBYTE) (drivestatus & 0x40));
            zprintf(TEXTPEN_LOG, " Bit 5: Write fault?     "); yesno((UBYTE) (drivestatus & 0x20));
        } elif (lastsubtype == 1) // Read Address
        {   zprintf(TEXTPEN_LOG, " Bit 4: ID not found?    "); yesno((UBYTE) (drivestatus & 0x10));
            zprintf(TEXTPEN_LOG, " Bit 3: CRC error?       "); yesno((UBYTE) (drivestatus & 0x08));
        }
        zprintf(    TEXTPEN_LOG, " Bit 2: Lost data?       "); yesno((UBYTE) (drivestatus & 0x04));
        zprintf(    TEXTPEN_LOG, " Bit 1: DRQ?             "); yesno((UBYTE) (drivestatus & 0x02));
        zprintf(    TEXTPEN_LOG, " Bit 0: Busy?            "); yesno((UBYTE) (drivestatus & 0x01));
}   }

MODULE void type1_head(UBYTE data)
{   if (data & 0x08)
    {   if (!drive[curdrive].spinning)
        {   drive[curdrive].spinning = TRUE;
            if (ambient)
            {   play_sample(SAMPLE_SPIN);
        }   }
        if (!drive[curdrive].headloaded)
        {   drive[curdrive].headloaded = TRUE; // it works even if drive is empty
            // play_ambient_sample(SAMPLE_CLUNK);
        }
        // if (!turbodisk) delay 10 ms
    } else
    {   // drive continues spinning for now if it was already
        drive[curdrive].headloaded = FALSE;
        // no delay
}   }

MODULE void type2_head(UBYTE data)
{   if (data & 0x04)
    {   if (!drive[curdrive].spinning)
        {   drive[curdrive].spinning = TRUE;
            if (ambient)
            {   play_sample(SAMPLE_SPIN);
        }   }
        if (!drive[curdrive].headloaded)
        {   drive[curdrive].headloaded = TRUE;
            // play_ambient_sample(SAMPLE_CLUNK);
        }
        // if (!turbodisk) delay 10 ms
    } else
    {   ; // drive continues spinning for now if it was already
        // head remains loaded if it was already
        // no delay
}   }

EXPORT void reset_drives(void)
{   int whichdrive;

    drive_mode           = DRIVEMODE_IDLE;
    drive_idmode         = 0;
    curdrive             = 0;
    stepdir              = 0;
    lasttype             =
    lastsubtype          = -1;
    timeoutat            = (ULONG) -1;
    sound_stop(GUESTCHANNELS + SAMPLE_GRIND);
    sound_stop(GUESTCHANNELS + SAMPLE_SPIN);
    for (whichdrive = 0; whichdrive < machines[machine].drives; whichdrive++)
    {   drive[whichdrive].blockoffset = 0;
        drive[whichdrive].spinning    = FALSE;
}   }

EXPORT void fd1771_spindown(void)
{   if (drive[curdrive].spinning && frames == timeoutat)
    {   set_drive_mode(DRIVEMODE_IDLE);
        sound_stop(GUESTCHANNELS + SAMPLE_GRIND);
        sound_stop(GUESTCHANNELS + SAMPLE_SPIN);
        drive[curdrive].headloaded = drive[curdrive].spinning = FALSE;
        update_floppydrive(1, curdrive);
        if (verbosedisk)
        {   zprintf(TEXTPEN_LOG, "Floppy drive #%d has unloaded head and spun down due to inactivity.\n", curdrive);
}   }   }
