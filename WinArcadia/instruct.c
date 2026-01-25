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

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"
#ifdef AMIGA
    #include <proto/locale.h>  // GetCatalogStr()
#endif

#include "instruct.h"

/* DEFINES----------------------------------------------------------------

(none)

EXPORTED VARIABLES----------------------------------------------------- */

EXPORT       UBYTE                    keys_column[7],
                                      s_tapeport    = 0,
                                      s_toggles     = 0;
EXPORT       int                      s_id          = INTDIR_DEFAULT,
                                      s_is          = INTSEL_DEFAULT,
                                      s_io          = PARALLEL_DEFAULT;

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       FLAG                     inframe;
IMPORT       UBYTE                    digitleds[DIGITLEDS],
                                      memory[32768],
                                      opcode,
                                      OutputBuffer[18],
                                      glow,
                                      psu,
                                      psl,
                                      r[7],
                                      startaddr_h, startaddr_l;
IMPORT       ASCREEN                  screen[BOXWIDTH][BOXHEIGHT];
IMPORT       UWORD                    console[4],
                                      iar,
                                      keypads[2][NUMKEYS],
                                      mirror_r[32768],
                                      mirror_w[32768];
IMPORT       ULONG                    frames,
                                      si50_bigctrls,
                                      verbosetape;
IMPORT       int                      console_start,
                                      console_a,
                                      console_b,
                                      console_reset,
                                      drawunlit,
                                      frameskip,
                                      game,
                                      machine,
                                      memmap,
                                      interrupt_2650,
                                      offset,
                                      randomizememory,
                                      recmode,
                                      slice_2650;
IMPORT       MEMFLAG                  memflags[ALLTOKENS];
IMPORT       UBYTE*                   IOBuffer;
IMPORT       FILE*                    MacroHandle;
IMPORT const TEXT                     led_chars[128 + 1];
IMPORT       struct HostMachineStruct hostmachines[MACHINES];
IMPORT       struct IOPortStruct      ioport[258];
IMPORT       struct MachineStruct     machines[MACHINES];
IMPORT const struct CodeCommentStruct codecomment[];

#ifdef AMIGA
    IMPORT   struct Catalog*          CatalogPtr;
    IMPORT   struct Window*           SubWindowPtr[SUBWINDOWS];
#endif
#ifdef WIN32
    IMPORT   int                      CatalogPtr; // APTR doesn't work
    IMPORT   ULONG                    pencolours[COLOURSETS][PENS];
    IMPORT   HWND                     SubWindowPtr[SUBWINDOWS];
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE       UBYTE                    s_position = 0;

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void si50_emuinput(void);

// CODE-------------------------------------------------------------------

MODULE void si50_emuinput(void)
{   FAST UBYTE t;

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
        t = IOBuffer[offset++];
        keys_column[6] = (UBYTE) ((t & 0xF0) >> 4);
        s_id          = (t & 8) >> 3; // 0/8 -> 0/1
        s_is          = (t & 4) >> 2; // 0/4 -> 0/1
        s_io          = (t & 3);      // 0..2
        s_toggles = IOBuffer[offset++];
    } else
    {   // assert(recmode == RECMODE_NORMAL || recmode == RECMODE_RECORD);

        t =  0;
        t |= (UBYTE) (0x1 * KeyDown(console[0]));
        if (console_start)
        {   t |= 1;
        }
        t |= (UBYTE) (0x2 * KeyDown(console[1]));
        if (console_a)
        {   t |= 2;
        }
        t |= (UBYTE) (0x4 * KeyDown(console[2]));
        if (console_b)
        {   t |= 4;
        }
        t |= (UBYTE) (0x8 * KeyDown(console[3]));
        if (console_reset)
        {   t |= 8;
        }
        keys_column[0] = t;

        t =  0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[0][1])); // "WCAS" key
        t |= (UBYTE) (0x2 * KeyDown(keypads[0][4])); // "RCAS" key
        t |= (UBYTE) (0x4 * KeyDown(keypads[0][7])); // "STEP" key
        t |= (UBYTE) (0x8 * KeyDown(keypads[0][10])); // "RUN" key
        keys_column[1] = t;

        t =  0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[0][2])); // "BKPT" key
        t |= (UBYTE) (0x2 * KeyDown(keypads[0][5])); // "REG" key
        t |= (UBYTE) (0x4 * KeyDown(keypads[0][8])); // "MEM" key
        t |= (UBYTE) (0x8 * KeyDown(keypads[0][11])); // "ENT NXT" key
        keys_column[2] = t;

        t =  0;
        t |= (UBYTE) (0x8 * KeyDown(keypads[0][3])); // "C" key
        t |= (UBYTE) (0x4 * KeyDown(keypads[0][6])); // "8" key
        t |= (UBYTE) (0x2 * KeyDown(keypads[0][9])); // "4" key
        t |= (UBYTE) (0x1 * KeyDown(keypads[0][12])); // "0" key
        keys_column[3] = t;

        t =  0;
        t |= (UBYTE) (0x8 * KeyDown(keypads[1][1])); // "D" key
        t |= (UBYTE) (0x4 * KeyDown(keypads[1][4])); // "9" key
        t |= (UBYTE) (0x2 * KeyDown(keypads[1][7])); // "5" key
        t |= (UBYTE) (0x1 * KeyDown(keypads[1][10])); // "1" key
        keys_column[4] = t;

        t =  0;
        t |= (UBYTE) (0x8 * KeyDown(keypads[1][2])); // "E" key
        t |= (UBYTE) (0x4 * KeyDown(keypads[1][5])); // "A" key
        t |= (UBYTE) (0x2 * KeyDown(keypads[1][8])); // "6" key
        t |= (UBYTE) (0x1 * KeyDown(keypads[1][11])); // "2" key
        keys_column[5] = t;

        t =  0;
        t |= (UBYTE) (0x8 * KeyDown(keypads[1][3])); // "F" key
        t |= (UBYTE) (0x4 * KeyDown(keypads[1][6])); // "B" key
        t |= (UBYTE) (0x2 * KeyDown(keypads[1][9])); // "7" key
        t |= (UBYTE) (0x1 * KeyDown(keypads[1][12])); // "3" key
        keys_column[6] = t;

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
        OutputBuffer[3] = (UBYTE) (((keys_column[6] & 0x0F) << 4)
                        |  ((ULONG) s_id         << 3) // 0/1 -> 0/8
                        |  ((ULONG) s_is         << 2) // 0/1 -> 0/4
                        |           s_io);             // 0..2
        OutputBuffer[4] =   s_toggles;

/* We only support recording/playing back s_id/intselector/io/toggles
   once per frame, but in theory they could get altered by the user, and then
   read by the game, many times per frame. */

        DISCARD fwrite(OutputBuffer, 5, 1, MacroHandle);
        // should really check return code of fwrite()
        // 5 bytes per frame * 60 frames per second = 300 bytes/sec.
    }

    if (console_start) console_start--;
    if (console_a    ) console_a--;
    if (console_b    ) console_b--;
    if (console_reset) console_reset--;
}

EXPORT void si50_viewscreen(void)
{   // assert(machine == INSTRUCTOR);

    zprintf
    (   TEXTPEN_VIEW,
        "Ports:        %02X %02X %02X %02X %02X %02X %02X %02X  %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
        digitleds[0],
        digitleds[1],
        digitleds[2],
        digitleds[3],
        digitleds[4],
        digitleds[5],
        digitleds[6],
        digitleds[7],
        led_chars[digitleds[0] & 0x7F], (digitleds[0] & 0x80) ? '.' : ' ',
        led_chars[digitleds[1] & 0x7F], (digitleds[1] & 0x80) ? '.' : ' ',
        led_chars[digitleds[2] & 0x7F], (digitleds[2] & 0x80) ? '.' : ' ',
        led_chars[digitleds[3] & 0x7F], (digitleds[3] & 0x80) ? '.' : ' ',
        led_chars[digitleds[4] & 0x7F], (digitleds[4] & 0x80) ? '.' : ' ',
        led_chars[digitleds[5] & 0x7F], (digitleds[5] & 0x80) ? '.' : ' ',
        led_chars[digitleds[6] & 0x7F], (digitleds[6] & 0x80) ? '.' : ' ',
        led_chars[digitleds[7] & 0x7F], (digitleds[7] & 0x80) ? '.' : ' '
    );
    zprintf
    (   TEXTPEN_VIEW,
        "$17D1..$17D8: %02X %02X %02X %02X %02X %02X %02X %02X  %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n\n",
        memory[0x17D1],
        memory[0x17D2],
        memory[0x17D3],
        memory[0x17D4],
        memory[0x17D5],
        memory[0x17D6],
        memory[0x17D7],
        memory[0x17D8],
        instructor_chars1[memory[0x17D1]], instructor_chars2[memory[0x17D1]],
        instructor_chars1[memory[0x17D2]], instructor_chars2[memory[0x17D2]],
        instructor_chars1[memory[0x17D3]], instructor_chars2[memory[0x17D3]],
        instructor_chars1[memory[0x17D4]], instructor_chars2[memory[0x17D4]],
        instructor_chars1[memory[0x17D5]], instructor_chars2[memory[0x17D5]],
        instructor_chars1[memory[0x17D6]], instructor_chars2[memory[0x17D6]],
        instructor_chars1[memory[0x17D7]], instructor_chars2[memory[0x17D7]],
        instructor_chars1[memory[0x17D8]], instructor_chars2[memory[0x17D8]]
    );
}

EXPORT void si50_setmemmap(void)
{   int i,
        address;

    game = FALSE;

    if (randomizememory)
    {   for (i =      0; i <= 0x17FF; i++)
        {   memory[i] = rand() & 0xFF; // ie. rand() % 256
        }
        for (i = 0x2000; i <= 0x7FFF; i++)
        {   memory[i] = rand() & 0xFF; // ie. rand() % 256
    }   }
    else
    {   for (i =      0; i <= 0x17FF; i++)
        {   memory[i] = 0;
        }
        for (i = 0x2000; i <= 0x7FFF; i++)
        {   memory[i] = 0;
    }   }
    for (i = 0; i <=  0x7FF; i++) // 2K
    {   memory[0x1800 + i] = si50_bios[i];
    }

    for (address = 0; address < 32 * KILOBYTE; address++)
    {   memflags[address] = 0;

        if (memmap == MEMMAP_N)
        {   if
            (             //    $0..               $1FF:    user RAM
                (address >=  0x200 && address <=  0xFFE) // unused
                          //  $FFF:                         I/O port
             || (address >= 0x1000 && address <= 0x177F) // unused
                          // $1780..              $17FF:    user RAM, monitor RAM
             || (address >= 0x1800 && address <= 0x7FFF) // monitor ROM, unused
            )
            {   memflags[address] |= NOWRITE;
            }
            if
            (             //    $0..               $1FF:    user RAM
                (address >=  0x200 && address <=  0xFFE) // unused
                          //  $FFF:                         I/O port
             || (address >= 0x1000 && address <= 0x177F) // unused
                          // $1780..              $1FFF:    user RAM, monitor RAM, monitor ROM
             || (address >= 0x2000 && address <= 0x7FFF) // unused
            )
            {   memflags[address] |= NOREAD;
        }   }
        else
        {   // assert(memmap == MEMMAP_O);
            if
            (             //    $0..               $FFF:    user RAM, expansion RAM, I/O port
                (address >= 0x1000 && address <= 0x177F) // unused
                          // $1780..              $17FF:    user RAM, monitor RAM
             || (address >= 0x1800 && address <= 0x1FFF) // monitor ROM
                          // $2000..              $7FFF:    expansion RAM
            )
            {   memflags[address] |= NOWRITE;
            }
            if
            (             //    $0..               $FFF:    user RAM, expansion RAM, I/O port
                (address >= 0x1000 && address <= 0x177F) // unused
                          // $1780..              $7FFF:    user RAM, monitor RAM, monitor ROM, expansion RAM
            )
            {   memflags[address] |= NOREAD;
        }   }

        if (address >= 0x17D1 && address <= 0x17D8)
        {   memflags[address] |= VISIBLE;
        }

        mirror_r[address] = mirror_w[address] = (UWORD) address;
    }

    for (i = machines[machine].firstcodecomment; i <= machines[machine].lastcodecomment; i++)
    {   memflags[codecomment[i].address] |= COMMENTED;
    }
    for (i = 0x1800; i <= 0x1FFF; i++) // 2K
    {   memflags[i] |= BIOS;
    }
    memflags[S_IOPORT] |= BANKED;
}

EXPORT void si50_emulate(void)
{   FAST UBYTE old_column0;
    FAST int   colour,
               i;

    inframe = TRUE;

    // this is to emulate the transience of the segment display
    digitleds[0] =
    digitleds[1] =
    digitleds[2] =
    digitleds[3] =
    digitleds[4] =
    digitleds[5] =
    digitleds[6] =
    digitleds[7] = 0;

    slice_2650 += 17900;
    cpu_2650_tapable();

    old_column0 = keys_column[0];

    wa_checkinput();
    si50_emuinput();

    if (keys_column[0] & 1)
    {   psu |= PSU_S;
    } else
    {   psu &= ~(PSU_S);
    }
    if ((keys_column[0] & 2) && !(old_column0 & 2))
    {   if (s_is == INTSEL_KYBD)
        {   interrupt_2650 = TRUE;
            checkinterrupt();
    }   }
    if ((keys_column[0] & 4) && !(old_column0 & 4))
    {   iar  = 0x1800;
        // "jump to BIOS" command also calls macro_stop(), but we (deliberately) don't.
    }
    if ((keys_column[0] & 8) && !(old_column0 & 8))
    {   iar  = 0;
        // "reset to game" command also calls macro_stop(), but we (deliberately) don't.
    }

    if (s_is == INTSEL_ACLINE)
    {   interrupt_2650 = TRUE;
        checkinterrupt();
    }

    if (recmode == RECMODE_PLAY)
    {   si50_updatedips(FALSE);
    }

    if (frameskip != FRAMESKIP_INFINITE && frames % (ULONG) frameskip == 0)
    {   drawsegments(0);
        drawsegments(1);
        drawsegments(2);
        drawsegments(3);
        drawsegments(4);
        drawsegments(5);
        drawsegments(6);
        drawsegments(7);

        for (i = 0; i < 8; i++)
        {   if (glow & (128 >> i))
            {   colour       = RED;
            } elif (drawunlit)
            {   colour       = DARKBLUE;
            } else
            {   colour       = BLACK;
            }
#ifdef WIDEINSTRUCTOR
            drawglow(20 + (i * 33), 49, colour);
#else
            drawglow(14 + (i * 21), 49, colour);
#endif
    }   }

    endofframe(BLACK);
}

EXPORT UBYTE si50_readport(int port)
{   FAST UBYTE t;

    // assert(machine == INSTRUCTOR);

    switch (port)
    {
    case 0x07:
        if (s_io == PARALLEL_EXTENDED)
        {   t = s_toggles;
        }
    acase 0xFC:
        t = ioport[0xFC].contents;
    acase 0xFD:
        t = ioport[0xFD].contents;
    acase 0xFE:
        if (frames % 2)
        {   switch (s_position)
            {
            case 0: // they want the 4th column
                t = (UBYTE) (keys_column[3] ^ 0xFF);
            acase 1: // they want the 5th column
                t = (UBYTE) (keys_column[4] ^ 0xFF);
            acase 2: // they want the 6th column
                t = (UBYTE) (keys_column[5] ^ 0xFF);
            acase 3: // they want the 7th column
                t = (UBYTE) (keys_column[6] ^ 0xFF);
            acase 4: // they want the 2nd column
                t = (UBYTE) (keys_column[1] ^ 0xFF);
            acase 5: // they want the 3rd column
                t = (UBYTE) (keys_column[2] ^ 0xFF);
            adefault: // don't know
                t = 0xFF; // t = (UBYTE) (keys_column[0] ^ 0xFF);
        }   }
        else
        {   t = 0xFF;
        }
    acase PORTD:
        if (s_io == PARALLEL_NONEXTENDED)
        {   t = s_toggles;
        }
    adefault:
        t = 0;
    }

    logport(port, t, FALSE);
    return t;
}

EXPORT void si50_writeport(int port, UBYTE data)
{   PERSIST int zeroed[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    switch (port)
    {
    case 0x07:
        if (s_io == PARALLEL_EXTENDED)
        {   glow = data;
        }
        if (verbosetape && (iar == 0x1EBC || iar == 0x1F07))
        {   zprintf(TEXTPEN_TAPE, "%c", r[0]);
        }
    acase 0xF8:
        s_tapeport = data;
    acase 0xF9:
        if (s_position == 8)
        {   return; // this is maybe supposed to clear the entire display or something? (which we haven't implemented)
        }

        if
        (   data == 0
         && (   (s_position == 0 && zeroed[s_position] < 2) // for some reason the USE BIOS likes to write zeroes *twice* to this position
             || zeroed[s_position] < 1 // and only *once* to all the other positions
        )   )
        {   zeroed[s_position]++;
            return;
        }
        zeroed[s_position] = 0;
\
        digitleds[s_position] = data;
        drawsegments(s_position);
    acase 0xFA:
        switch (data)
        {
        case  0x80: s_position = 7;
        acase 0x40: s_position = 6;
        acase 0x20: s_position = 5;
        acase 0x10: s_position = 4;
        acase 0x08: s_position = 3;
        acase 0x04: s_position = 2;
        acase 0x02: s_position = 1;
        acase 0x01: s_position = 0;
        adefault:   s_position = 8; // eg. data of $00 (which does happen)
        }
    acase 0xFB:
        ioport[0xFC].contents = iar % 256;
        ioport[0xFD].contents = iar / 256;
    // PORTC is handled by 2650 core
    acase PORTD:
        if (s_io == PARALLEL_NONEXTENDED)
        {   glow = data;
    }   }

    logport(port, data, TRUE);
}

EXPORT void instructor_biosdetails(int ea)
{   int address,
        alt;

    // assert(machine == INSTRUCTOR);

    if (psl & PSL_RS) alt = 3; else alt = 0;

    switch (ea)
    {
    case 0x1800: // SAVRG
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Save all registers upon entry to program\n" \
            ";Registers used:     R0..R6, PSW\n" \
            ";Subroutines called: None\n" \
            ";RAM used:           UREG:    R0\n" \
            ";                    UREG+ 1: R1\n" \
            ";                    UREG+ 2: R2\n" \
            ";                    UREG+ 3: R3\n" \
            ";                    UREG+ 4: R4\n" \
            ";                    UREG+ 5: R5\n" \
            ";                    UREG+ 6: R6\n" \
            ";                    UREG+ 7: PSU\n" \
            ";                    UREG+ 8: PSL\n" \
            ";                    UREG+ 9: PPSL instruction opcode\n" \
            ";                    UREG+10: PSL\n" \
            ";                    UREG+11: RETC,un instruction opcode\n"
        );
    acase 0x182E: // BEG
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Program entry routine\n" \
            ";Notes:              Decides how reached entry point of program:\n" \
            ";                    1: Power on\n" \
            ";                    2: Single step\n" \
            ";                    3: Monitor pushbutton\n" \
            ";                    4: Breakpoint\n"
        );
    acase 0x1847: // INIT
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Keyboard monitor routine\n" \
            ";Registers used:     R0..R2: scratch\n" \
            ";                    R3:     not used\n" \
            ";Subroutines called: MOV:    move data to display\n" \
            ";                    DISPLY: display message\n" \
            ";RAM used:           PWRON:  power on flag\n" \
            ";                    SSF:    single step flag\n" \
            ";                    BPF:    breakpoint flag\n" \
            ";                    BPL:    breakpoint location\n"
        );
    acase 0x18B4: // SSTEP
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Single step routines\n" \
            ";Registers used:     R0..R3: scratch\n" \
            ";Subroutines called: RLADR:  restore last address register\n" \
            ";RAM used:           LADR:   last address register\n" \
            ";                    T3:     temp register\n" \
            ";                    TEMP:   temp register\n" \
            ";                    SCTCH:  scratch register\n" \
            ";Notes:              Processor transfers control to user program after\n" \
            ";                    computing the number of OPREQs till the next\n" \
            ";                    instruction fetch\n"
        );
    acase 0x197A: // BRKPT
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Breakpoint and single step runtime code\n" \
            ";Registers used:     R0\n" \
            ";Subroutines called: DISLSD: prepare binary data for display\n" \
            ";RAM used:           DISBUF: display buffer\n" \
            ";                    BPF:    breakpoint flag\n" \
            ";                    BPL:    breakpoint location\n" \
            ";                    BPD:    data for breakpoint location\n" \
            ";                    LADR:   copy of last address register\n" \
            ";                    SSF:    single step flag\n" \
            ";Notes:              When entered at single step, single step flag is\n" \
            ";                    cleared and display is ' ADDR DD'\n" \
            ";                    When entered at breakpoint and breakpoint is set\n" \
            ";                    and matches breakpoint register, the display is\n" \
            ";                    '-ADDR DD'\n"
        );
    acase 0x1A08: // PTCH
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Patch memory routine\n" \
            ";Registers used:     R0..R3: scratch\n" \
            ";Subroutines called: GAD:    get address parameter\n" \
            ";                    GNP:    get number parameter\n" \
            ";                    ROT:    rotate R0 1 nibble left\n" \
            ";                    BRKPT4: set up display 6&7\n" \
            ";                    BRKPT6: set up display 3&4\n" \
            ";                    BRKPT7: set up display 1&2\n" \
            ";RAM used:           MEM:    indirect address memory pointer\n" \
            ";                    ALTF:   alter flag = 3 or 5 for patch\n"
        );
    acase 0x1A0C: // ALTER
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Display and alter memory routine\n" \
            ";Registers used:     R0..R3: scratch\n" \
            ";Subroutines called: GAD:    get address parameter\n" \
            ";                    GNP:    get number parameter\n" \
            ";                    ROT:    rotate R0 1 nibble left\n" \
            ";                    BRKPT4: set up display 6&7\n" \
            ";                    BRKPT6: set up display 3&4\n" \
            ";                    BRKPT7: set up display 1&2\n" \
            ";RAM used:           MEM:    indirect address memory pointer\n" \
            ";                    ALTF:   alter flag = 1 for display and alter\n"
        );
    acase 0x1A76: // DISLSI
        zprintf
        (   TEXTPEN_LOG,
            ";Byte to split:      $%02X\n" \
            ";Stack required:     1\n" \
            ";Input register:     R0\n" \
            ";Scratch registers:  None\n" \
            ";Return codes:       R0/R%d\n",
            r[0],
            alt + 1
        );
    acase 0x1A7E: // REG
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Display and alter registers command\n" \
            ";Function:           Allows the user to examine and alter R0..R6, PSW, PC\n" \
            ";Registers used:     R0..R3: scratch\n" \
            ";Subroutines called: MOVE:   move data to DISBUF\n" \
            ";                    GNP:    get numeric parameters\n" \
            ";                    ROT:    rotate a nibble\n" \
            ";                    GNPA:   display and get numeric parameters\n" \
            ";                    BRKPT4: set display 6&7\n" \
            ";                    SCBP2:  set display 4&5\n" \
            ";RAM used:           DISBUF: display buffer\n" \
            ";                    UREG:   user registers\n" \
            ";                    LADR:   last address register PC counter\n" \
            ";                    T2:     temp register\n" \
            ";Notes:              This command also provides entry point to alternate\n" \
            ";                    functions:\n" \
            ";                    Reg 9: not defined\n" \
            ";                    Reg A: adjust cassette command\n" \
            ";                    Reg B: not defined\n" \
            ";                    Reg D: not defined\n" \
            ";                    Reg E: not defined\n" \
            ";                    Reg F: enter the fast patch mode\n"
        );
    acase 0x1B04: // GAD
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Get numeric parameters\n" \
            ";Function:           Gets either 2 or 4 digit numeric parameters\n" \
            ";Input parameters:   R0:\n" \
            ";                    bit 0 = 0: double byte\n" \
            ";                    bit 0 = 1: single byte data to be returned\n" \
            ";                    bit 1 = 0: requires function key depression to exit\n" \
            ";                    bit 1 = 1: when set with bit 0, exit is after entry\n" \
            ";                               of third digit of single byte data\n" \
            ";                    bit 2 = 1: when set with bit 0, exit is after second\n" \
            ";                               digit of single byte data\n" \
            ";Output parameters:  R0 = LSB of double byte data or single byte data\n" \
            ";                    R1 = MSB of double byte data or 0 for single byte data\n" \
            ";                    R2 = function key pressed code\n" \
            ";                    R3 = 0:     data returned in R0 (LSB), R1 (MSB)\n" \
            ";                    R3 = not 0: no data returned. R0, R1 = 0\n" \
            ";Scratch registers:  R0..R3\n" \
            ";Subroutines called: DISPLY: Display and read keyboard\n" \
            ";                    CLR:    blank digit display\n" \
            ";RAM used:           T1:     save entry flag\n" \
            ";                    DISBUF: bits 4 thru 7\n" \
            ";Notes:              Single byte data uses display buffer 5 thru 7\n" \
            ";                    Double byte data uses display buffer 4 thru 7\n" \
            ";                    Other digits of DISBUF must be initialized on entry\n" \
            ";                    Returns when function key depressed\n"
        );
    acase 0x1BAC: // RCAS
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Read cassette command\n" \
            ";Function:           This is the hex object loader.\n" \
            ";                    This routine requests a file ID and then loads\n" \
            ";                    2650 hex object modules into memory\n" \
            ";Scratch registers:  All\n" \
            ";Subroutines called: IN:  cassette input routine\n" \
            ";                    MOV: move data to display buffer\n" \
            ";                    GNP: get numeric parameters\n"
        );
    acase 0x1C8B: // WCAS4
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Write cassette command\n" \
            ";Function:           This routine writes 2650 hex format to cassette tape\n"
            ";Scratch registers:  R0..R3\n" \
            ";Subroutines called: OUT:  write char to tape\n" \
            ";                    HOUT: convert binary to ASCII hex\n" \
            ";                    INK:  increment pointer mem\n" \
            ";RAM used:           BCC:  block check char\n" \
            ";                    MEM:  pointer\n" \
            ";                    BAD:  program start address\n" \
            ";                    SAD:  dump stop address\n" \
            ";                    FID:  file ID flag and storage\n" \
            ";Notes:              This routine punches a hex format tape:\n" \
            ";                    Leader16ID:AddrCTBCAADDCCRR........BC\n"
        );
    acase 0x1D61: // SCBP
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Set or clear breakpoint\n" \
            ";Subroutines called: MOV:    move data to DISBUF\n" \
            ";                    GNPA:   display and get address data\n" \
            ";                    ROT:    rotate a nibble\n" \
            ";                    SCBP2:  set DISBUF 4&5\n" \
            ";                    BRKPT4: set DISBUF 6&7\n" \
            ";                    DISLSD: convert to binary for display\n" \
            ";RAM used:           BPF:    breakpoint flag\n" \
            ";                    BPL:    location of breakpoint\n" \
            ";                    BPD:    data to be restored in breakpoint location\n" \
            ";Notes:              To set breakpoint, enter address and depress\n" \
            ";                    function key.\n" \
            ";                    To clear breakpoint, depress function key.\n"
        );
    acase 0x1DB6: // MOVI
        address = (r[alt + 1] * 256) + r[alt + 2] + 1; // we should handle wrapping
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Move 8 bytes of data pointed to in R1\n" \
            ";                    and R2 to DISBUF\n" \
            ";Message address:    $%04X\n" \
            ";Message contents:   \"%c%c%c%c%c%c%c%c\"\n" \
            ";Stack required:     0\n" \
            ";Registers used:     R0: scratch\n" \
            ";                    R1: hi address byte of data\n" \
            ";                    R2: lo address byte of data\n" \
            ";                    R3: not used\n" \
            ";Subroutines called: None\n" \
            ";RAM used:           T:  temp indirect address\n" \
            ";Return code:        None\n",
            address,
            instructor_chars1[memory[address    ]],
            instructor_chars1[memory[address + 1]],
            instructor_chars1[memory[address + 2]],
            instructor_chars1[memory[address + 3]],
            instructor_chars1[memory[address + 4]],
            instructor_chars1[memory[address + 5]],
            instructor_chars1[memory[address + 6]],
            instructor_chars1[memory[address + 7]],
            alt + 1,
            alt + 2
        );
    acase 0x1DC7: // DLOOP
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Keyboard scan and display routine\n" \
            ";Input parameters:   R0 =      0: normal operation\n" \
            ";                    R0 = 1..127: go thru scan once and exit\n" \
            ";                    R0 =    $80: turn on decimal point for entry mode\n" \
            ";Registers used:     R0:     scratch\n" \
            ";                    R1/R4:  keyboard flags\n" \
            ";                    R2/R5:  digit select\n" \
            ";                    R3/R6:  digit pointer\n" \
            ";Subroutines called: None\n" \
            ";RAM used:           DISBUF: temp indirect address\n" \
            ";                    KFLG:   keyboard flag\n" \
            ";Return code:        R0 = key pressed code\n" \
            ";Notes:              To use this routine, place data to be displayed\n" \
            ";                    in DISBUF\n"
        );
    acase 0x1E13: // DISPLI
        zprintf(TEXTPEN_LOG, "Mode:               $%02X (", r[0]);
        switch (r[alt + 3])
        {
        case 0:
            zprintf(TEXTPEN_LOG, "wait for key with unlit dot");
        acase 1:
            zprintf(TEXTPEN_LOG, "flash once");
        acase 0x80:
            zprintf(TEXTPEN_LOG, "wait for key with lit dot");
        adefault:
            zprintf(TEXTPEN_LOG, "illegal!", r[0]);
        }
        zprintf(TEXTPEN_LOG, ")\n");

        address = (r[alt + 1] * 256) + r[alt + 2] + 1; // we should handle wrapping
        zprintf
        (   TEXTPEN_LOG,
            ";Message address:    $%04X\n" \
            ";Message contents:   \"%c%c%c%c%c%c%c%c\"\n" \
            ";Stack required:     0\n" \
            ";Input registers:    R0/R%d/R%d\n" \
            ";Scratch registers:  R%d/R%d/R%d\n" \
            ";Return code:        R0\n",
            address,
            instructor_chars1[memory[address    ]],
            instructor_chars1[memory[address + 1]],
            instructor_chars1[memory[address + 2]],
            instructor_chars1[memory[address + 3]],
            instructor_chars1[memory[address + 4]],
            instructor_chars1[memory[address + 5]],
            instructor_chars1[memory[address + 6]],
            instructor_chars1[memory[address + 7]],
            alt + 1,
            alt + 2,
            alt + 1,
            alt + 2,
            alt + 3
        );
    acase 0x1E59: // GO
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Goto routine\n" \
            ";Registers used:     R0..R3: scratch\n" \
            ";                    R4..R6: restored\n" \
            ";                    PSW:    restored\n" \
            ";Subroutines called: None\n" \
            ";RAM used:           SSF:    single step flag\n" \
            ";                    BPF:    breakpoint flag\n" \
            ";                    BPL:    breakpoint location\n" \
            ";                    BPD:    breakpoint data\n" \
            ";                    LADR:   indirect address to jump thru\n"
        );
    acase 0x1E77: // RESTRG
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Restore registers before going to user program\n" \
            ";Registers used:     R0..R6, PSW\n" \
            ";Subroutines called: UREG+ 9: restore PSL\n" \
            ";RAM used:           UREG:    R0\n" \
            ";                    UREG+ 1: R1\n" \
            ";                    UREG+ 2: R2\n" \
            ";                    UREG+ 3: R3\n" \
            ";                    UREG+ 4: R4\n" \
            ";                    UREG+ 5: R5\n" \
            ";                    UREG+ 6: R6\n" \
            ";                    UREG+ 7: PSU\n" \
            ";                    UREG+ 8: PSL\n" \
            ";                    UREG+ 9: PPSL instruction opcode\n" \
            ";                    UREG+10: PSL\n" \
            ";                    UREG+11: RETC,un instruction opcode\n"
        );
    acase 0x1EA9: // SAVR0
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Subroutine to save R1/R4, R2/R5, R3/R6\n" \
            ";Registers used:     R1/R4: saved in SAVREG+1\n" \
            ";                    R2/R5: saved in SAVREG+2\n" \
            ";                    R3/R6: saved in SAVREG+3\n" \
            ";Subroutines called: None\n" \
            ";RAM used:           SAVREG+1\n" \
            ";                    SAVREG+2\n" \
            ";                    SAVREG+3\n"
        );
    acase 0x1EB3: // RESTR0
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Subroutine to restore R1/R4, R2/R5, R3/R6\n" \
            ";Registers used:     R1/R4: restored to value in SAVREG+1\n" \
            ";                    R2/R5: restored to value in SAVREG+2\n" \
            ";                    R3/R6: restored to value in SAVREG+3\n" \
            ";Subroutines called: None\n" \
            ";RAM used:           SAVREG+1\n" \
            ";                    SAVREG+2\n" \
            ";                    SAVREG+3\n"
        );
    acase 0x1EBA: // OUTT
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Write one byte to the cassette in SIMCA format\n" \
            ";Registers used:     R0:     byte to be output\n" \
            ";                    R0..R3: scratch\n" \
            ";Subroutines called: SAVR0:  saves R1..R3\n" \
            ";                    RESTR0: restores R1..R3\n" \
            ";RAM used:           TEMP:   temporary storage\n"
        );
    acase 0x1EF6: // INN
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Read one byte from the cassette in SIMCA format\n" \
            ";Registers used:     R1..R3: scratch\n" \
            ";                    R0:     returns with data byte in R0\n" \
            ";Subroutines called: SAVR0:  saves R1..R3\n" \
            ";                    RESTR0: restores R1..R3\n" \
            ";RAM used:           TEMP:   temporary storage\n"
        );
    acase 0x1F0A: // GBIT
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Subroutine to get the next bit from cassette\n" \
            ";Notes:              Bit is returned as least significant bit of R0\n"
        );
    acase 0x1F32: // TCAS
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:          Subroutine to test cassette reads\n" \
            ";Registers used:     R0..R3: scratch\n" \
            ";RAM used:           TEMP:   temporary storage\n" \
            ";Notes:              Used to set levels on playback\n"
        );
    acase 0x1FD5: // USRDSI
        zprintf(TEXTPEN_LOG, ";Mode:               $%02X (", r[alt + 3]);
        switch (r[alt + 3])
        {
        case 0:
            zprintf(TEXTPEN_LOG, "wait for key with unlit dot");
        acase 1:
            zprintf(TEXTPEN_LOG, "flash once");
        acase 0x80:
            zprintf(TEXTPEN_LOG, "wait for key with lit dot");
        adefault:
            zprintf(TEXTPEN_LOG, "illegal!");
        }
        zprintf(TEXTPEN_LOG, ")\n");

        address = (r[alt + 1] * 256) + r[alt + 2] + 1; // we should handle wrapping
        zprintf
        (   TEXTPEN_LOG,
            ";Message address:    $%04X\n" \
            ";Message contents:   \"%c%c%c%c%c%c%c%c\"\n" \
            ";Stack required:     2\n" \
            ";Input registers:    R%d/R%d/R%d\n" \
            ";Scratch registers:  None\n" \
            ";Return code:        R0\n",
            address,
            instructor_chars1[memory[address    ]],
            instructor_chars1[memory[address + 1]],
            instructor_chars1[memory[address + 2]],
            instructor_chars1[memory[address + 3]],
            instructor_chars1[memory[address + 4]],
            instructor_chars1[memory[address + 5]],
            instructor_chars1[memory[address + 6]],
            instructor_chars1[memory[address + 7]],
            alt + 1,
            alt + 2,
            alt + 3
        );
}   }

EXPORT void si50_update_miniglows(void)
{   int i;

    if (machine != INSTRUCTOR || !SubWindowPtr[SUBWINDOW_CONTROLS])
    {   return;
    }

    for (i = 0; i < 8; i++)
    {   if (si50_bigctrls)
        {   if (i >= 4)
            {   drawctrlglow(69 + (19 * i), 319, (FLAG) ((glow & (128 >> i)) ? TRUE : FALSE));
            } else
            {   drawctrlglow(72 + (18 * i), 319, (FLAG) ((glow & (128 >> i)) ? TRUE : FALSE));
        }   }
        else
        {   drawctrlglow(    94 + ( 8 * i), 227, (FLAG) ((glow & (128 >> i)) ? TRUE : FALSE));
    }   }

    if (si50_bigctrls)
    {   drawctrlglow(276, 320, (FLAG) ((opcode != 0x40) ? TRUE : FALSE));
        drawctrlglow(277, 351, (FLAG) (psu & PSU_F)                    );
    } else
    {   drawctrlglow(182, 228, (FLAG) ((opcode != 0x40) ? TRUE : FALSE));
        drawctrlglow(182, 242, (FLAG) (psu & PSU_F)                    );
}   }

EXPORT void si50_reset(void)
{   int i;

    // LED digits
    for (i = 0; i < DIGITLEDS; i++)
    {   digitleds[i] = 0;
    }

    // tape deck
    s_tapeport    = 0;

    // game
    if (game)
    {   iar = (startaddr_h * 256) + startaddr_l;
    } else
    {   iar = 0x1800;
}   }

EXPORT void si50_updatedips(FLAG force)
{   PERSIST UBYTE old_s_toggles;
    PERSIST int old_s_id,
                old_s_is,
                old_s_io;

    if (machine != INSTRUCTOR || !SubWindowPtr[SUBWINDOW_DIPS])
    {   return;
    }

    if (force || s_id != old_s_id)
    {   old_s_id = s_id;
        ra_set(SUBWINDOW_DIPS, IDC_INTERRUPTS_DIRECT, IDC_INTERRUPTS_INDIRECT, s_id);
    }
    if (force || s_is != old_s_is)
    {   old_s_is = s_is;
        ra_set(SUBWINDOW_DIPS, IDC_INTSELECTOR_ACLINE, IDC_INTSELECTOR_KYBD, s_is);
    }
    if (force || s_io != old_s_io)
    {   old_s_io = s_io;
        ra_set(SUBWINDOW_DIPS, IDC_PARALLEL_MEMMAPPED, IDC_PARALLEL_NONEXTENDED, s_io);
    }

    if (force || (s_toggles & 128) != (old_s_toggles & 128)) bu_select(SUBWINDOW_DIPS, IDC_PARALLEL_BIT7, s_toggles & 128);
    if (force || (s_toggles &  64) != (old_s_toggles &  64)) bu_select(SUBWINDOW_DIPS, IDC_PARALLEL_BIT6, s_toggles &  64);
    if (force || (s_toggles &  32) != (old_s_toggles &  32)) bu_select(SUBWINDOW_DIPS, IDC_PARALLEL_BIT5, s_toggles &  32);
    if (force || (s_toggles &  16) != (old_s_toggles &  16)) bu_select(SUBWINDOW_DIPS, IDC_PARALLEL_BIT4, s_toggles &  16);
    if (force || (s_toggles &   8) != (old_s_toggles &   8)) bu_select(SUBWINDOW_DIPS, IDC_PARALLEL_BIT3, s_toggles &   8);
    if (force || (s_toggles &   4) != (old_s_toggles &   4)) bu_select(SUBWINDOW_DIPS, IDC_PARALLEL_BIT2, s_toggles &   4);
    if (force || (s_toggles &   2) != (old_s_toggles &   2)) bu_select(SUBWINDOW_DIPS, IDC_PARALLEL_BIT1, s_toggles &   2);
    if (force || (s_toggles &   1) != (old_s_toggles &   1)) bu_select(SUBWINDOW_DIPS, IDC_PARALLEL_BIT0, s_toggles &   1);
    old_s_toggles = s_toggles;
}
