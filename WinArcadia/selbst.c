// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
    #include <stdlib.h> // eg. for rand()
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
#ifdef AMIGA
    #include <proto/locale.h>  // GetCatalogStr()
#endif

#include "selbst.h"

// DEFINES----------------------------------------------------------------

// #define USEETI641
// whether to use the ETI-641 character set instead of the Selbstbaucomputer character set

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT UBYTE                      selbst_char;
EXPORT int                        fastselbst = TRUE;

EXPORT const int selbst_digitxloc[6] =
{ 211, 244, 278, 311, 344, 377 };

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT ASCREEN                    screen[BOXWIDTH][BOXHEIGHT];
IMPORT FLAG                       inframe;
IMPORT UBYTE                      blank,
                                  digitleds[DIGITLEDS],
                                  memory[32768],
                                  keys_column[7],
                                  glow,
                                  OutputBuffer[18],
                                  psu,
                                  startaddr_h, startaddr_l,
                                  tone,
                                  vdu[MAX_VDUCOLUMNS][MAX_VDUROWS],
                                  vdu_bgc,
                                  vdu_fgc;
IMPORT UWORD                      console[4],
                                  iar,
                                  keypads[2][NUMKEYS],
                                  mirror_r[32768],
                                  mirror_w[32768];
IMPORT ULONG                      frames,
                                  verbosetape;
IMPORT int                        blink,
                                  drawmode,
                                  drawunlit,
                                  frameskip,
                                  game,
                                  machine,
                                  memmap,
                                  offset,
                                  queuepos,
                                  recmode,
                                  selbst_biosver,
                                  slice_2650,
                                  tapemode,
                                  vdu_x, vdu_y,
                                  whichgame,
                                  whichkeyrect;
IMPORT UBYTE*                     IOBuffer;
IMPORT FILE*                      MacroHandle;
IMPORT       MEMFLAG              memflags[ALLTOKENS];
IMPORT const TEXT                 led_chars[128 + 1];
IMPORT const struct CodeCommentStruct codecomment[];
IMPORT       struct MachineStruct machines[MACHINES];
#ifdef AMIGA
    IMPORT   struct Catalog*      CatalogPtr;
#endif
#ifdef WIN32
    IMPORT   int                  CatalogPtr; // APTR doesn't work
#endif
#ifdef VERBOSE
    IMPORT   TEXT                 asciiname_long[259][9 + 1];
#endif

/* MODULE VARIABLES-------------------------------------------------------

(See selbst.h)

MODULE FUNCTIONS------------------------------------------------------- */

MODULE void selbst_emuinput(void);
MODULE void selbst_cout(UBYTE command, TEXT thechar);

// CODE-------------------------------------------------------------------

MODULE void selbst_emuinput(void)
{   UBYTE t;

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
        t |= (UBYTE) (0x1 * KeyDown(keypads[0][ 1])); // C
        t |= (UBYTE) (0x2 * KeyDown(keypads[0][ 4])); // 8
        t |= (UBYTE) (0x4 * KeyDown(keypads[0][ 7])); // 4
        t |= (UBYTE) (0x8 * KeyDown(keypads[0][10])); // 0
        keys_column[0] = t;

        t = 0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[0][ 2])); // D
        t |= (UBYTE) (0x2 * KeyDown(keypads[0][ 5])); // 9
        t |= (UBYTE) (0x4 * KeyDown(keypads[0][ 8])); // 5
        t |= (UBYTE) (0x8 * KeyDown(keypads[0][11])); // 1
        keys_column[1] = t;

        t = 0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[0][ 3])); // E
        t |= (UBYTE) (0x2 * KeyDown(keypads[0][ 6])); // A
        t |= (UBYTE) (0x4 * KeyDown(keypads[0][ 9])); // 6
        t |= (UBYTE) (0x8 * KeyDown(keypads[0][12])); // 2
        keys_column[2] = t;

        t = 0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[1][ 1])); // F
        t |= (UBYTE) (0x2 * KeyDown(keypads[1][ 4])); // B
        t |= (UBYTE) (0x4 * KeyDown(keypads[1][ 7])); // 7
        t |= (UBYTE) (0x8 * KeyDown(keypads[1][10])); // 3
        keys_column[3] = t;

        t = 0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[1][ 2])); // CMD
        t |= (UBYTE) (0x2 * KeyDown(keypads[1][ 5])); // RUN
        t |= (UBYTE) (0x4 * KeyDown(keypads[1][ 8])); // GOTO
        t |= (UBYTE) (0x8 * KeyDown(keypads[1][11])); // RST
        keys_column[4] = t;

        t = 0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[1][ 3])); // FLAG
        t |= (UBYTE) (0x2 * KeyDown(keypads[1][ 6])); // MON
        t |= (UBYTE) (0x4 * KeyDown(keypads[1][ 9])); // PC
        t |= (UBYTE) (0x8 * KeyDown(keypads[1][12])); // NXT
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

    if (keys_column[4] & 8)
    {   project_reset(FALSE);
    }
    if (keys_column[5] & 1)
    {   psu |= PSU_F;
    } else
    {   psu &= ~(PSU_F);
}   }

EXPORT void selbst_setmemmap(void)
{   int i, address, mirror;

    game = FALSE;
    machines[SELBST].cpf = fastselbst ? 20000.0 : 0.02;

    switch (selbst_biosver)
    {
    case SELBST_BIOS00:
        machines[SELBST].firstequiv       =
        machines[SELBST].lastequiv        =
        machines[SELBST].firstcodecomment =
        machines[SELBST].lastcodecomment  =
        machines[SELBST].firstdatacomment =
        machines[SELBST].lastdatacomment  = -1;
        for (i = 0x0; i <= 0x7FF; i++) // 2K
        {   memory[i] = 0;
        }
    acase SELBST_BIOS09:
        machines[SELBST].firstequiv       = FIRST09EQUIV;
        machines[SELBST].lastequiv        = LAST09EQUIV;
        machines[SELBST].firstcodecomment = FIRSTCODECOMMENT09;
        machines[SELBST].lastcodecomment  = LASTCODECOMMENT09;
        machines[SELBST].firstdatacomment = FIRSTDATACOMMENT09;
        machines[SELBST].lastdatacomment  = LASTDATACOMMENT09;
        for (i = 0x0; i <= 0x7FF; i++) // 2K
        {   memory[i] = selbst_bios[0][i];
        }
    acase SELBST_BIOS10:
        machines[SELBST].firstequiv       = FIRST10EQUIV;
        machines[SELBST].lastequiv        = LAST10EQUIV;
        machines[SELBST].firstcodecomment = FIRSTCODECOMMENT10;
        machines[SELBST].lastcodecomment  = LASTCODECOMMENT10;
        machines[SELBST].firstdatacomment = FIRSTDATACOMMENT10;
        machines[SELBST].lastdatacomment  = LASTDATACOMMENT10;
        for (i = 0x0; i <= 0x7FF; i++) // 2K
        {   memory[i] = selbst_bios[1][i];
        }
    acase SELBST_BIOS20:
        machines[SELBST].firstequiv       = FIRST20EQUIV;
        machines[SELBST].lastequiv        = LAST20EQUIV;
        machines[SELBST].firstcodecomment = FIRSTCODECOMMENT20;
        machines[SELBST].lastcodecomment  = LASTCODECOMMENT20;
        machines[SELBST].firstdatacomment = FIRSTDATACOMMENT20;
        machines[SELBST].lastdatacomment  = LASTDATACOMMENT20;
        for (i = 0x0; i <= 0x7FF; i++) // 2K
        {   memory[i] = selbst_bios[2][i];
    }   }

    for (i = 0x800; i <= 0x7FFF; i++)
    {   memory[i] = 0;
    }

    for (address = 0; address < 32 * KILOBYTE; address++)
    {   memflags[address] = 0;
        mirror                 = address;

        if (address <= 0x7FF)
        {   if (selbst_biosver != SELBST_BIOS00)
            {   memflags[address] |= BIOS | NOWRITE;
        }   }
        elif (address >= 0x2000)
        {   memflags[address] |= NOREAD | NOWRITE;
        }

     /* if (mirror != address)
        {   memflags[address] |= MIRRORED_R | MIRRORED_W;
        } */
        mirror_r[address] =
        mirror_w[address] = (UWORD) mirror;
    }

    if (machines[machine].firstcodecomment != -1)
    {   for (i = machines[machine].firstcodecomment; i <= machines[machine].lastcodecomment; i++)
        {   memflags[codecomment[i].address] |= COMMENTED;
    }   }

    blank = PIPBUG_BLANK;
    whichkeyrect = KEYRECT_SELBST;
}

EXPORT void selbst_emulate(void)
{   FAST int   before,
               colour,
               i,
               x, xx,
               y, yy, yyy;
    FAST UBYTE t,
               tempfgc,
               tempvdu;

    inframe = TRUE;

    // this is to emulate the transience of the segment display
    digitleds[0] =
    digitleds[1] =
    digitleds[2] =
    digitleds[3] =
    digitleds[4] =
    digitleds[5] = 0;

    if (fastselbst)
    {   // 1MHz = 1,000,000 cycles per second
        // 1,000,000 ÷ 50 = 20,000

        if (whichgame == DICEPOS || whichgame == DICEGAMEPOS || whichgame == LOTTOPOS || whichgame == SELBST_MORSEPOS) // better random numbers
        {   psu &= ~(PSU_S); // Sense bit does 50 complete cycles per second
            before = (rand() % 19999) + 1; // 1..19999
            slice_2650 += before;
            cpu_2650_tapable();
            psu |=   PSU_S ; // Sense bit does 50 complete cycles per second
            slice_2650 += 20000 - before;
            cpu_2650_tapable();
        } else // more regular timing
        {   psu &= ~(PSU_S); // Sense bit does 50 complete cycles per second
            slice_2650 += 10000;
            cpu_2650_tapable();
            psu |=   PSU_S ; // Sense bit does 50 complete cycles per second
            slice_2650 += 10000;
            cpu_2650_tapable();
    }   }
    else
    {   if (frames % 50 == 0)
        {   slice_2650++;
        }
        if (slice_2650 >= 1)
        {   psu ^= PSU_S;
            cpu_2650_tapable();
    }   }

    wa_checkinput();
    selbst_emuinput();

    if (frameskip != FRAMESKIP_INFINITE && frames % (ULONG) frameskip == 0)
    {   for (x = 0; x < machines[machine].width; x++)
        {   for (y = 0; y < 128; y++)
            {   changebgpixel(x, y, vdu_bgc);
        }   }

        tempfgc = vdu_fgc;

        for (x = 0; x < MAX_VDUCOLUMNS; x++)
        {   yyy = 0;
            for (y = 0; y < 24; y++)
            {   tempvdu = vdu[x][y];

#ifdef USEETI641
                for (xx = 0; xx < 8; xx++)
                {   t = eti641_chars[tempvdu & 0x7F][xx] ^ 0xFF;
                    for (yy = 0; yy < 8; yy++)
                    {   if (t & (0x80 >> yy))
                        {   changepixel((x * 8) + xx, (y * 8) + yy, vdu_fgc);
                        } else
                        {   changepixel((x * 8) + xx, (y * 8) + yy, vdu_bgc);
                }   }   }
#else
                for (yy = 0; yy < 8; yy++)
                {   t = selbst_gfx[(tempvdu * 8) + yy];
                    if (vdu_x == x && vdu_y == y)
                    {   if (!blink || frames % 50 < 25)
                        {   t = ~t; // inverted
                        } elif (yy == 7)
                        {   t = 0xFF; // ######## underline
                    }   }
                    for (xx = 0; xx < SELBST_CHARWIDTH; xx++)
                    {   if ((t & (0x80 >> xx)) && (x * SELBST_CHARWIDTH) + xx < SELBST_BOXWIDTH)
                        {   changepixel((x * SELBST_CHARWIDTH) + xx, yyy, tempfgc);
                    }   }
                    yyy++;
                }
#endif
        }   }

        drawsegments(0);
        drawsegments(1);
        drawsegments(2);
        drawsegments(3);
        drawsegments(4);
        drawsegments(5);

        for (i = 0; i < 8; i++)
        {   if (glow & (128 >> i))
            {   switch (i)
                {
                case  0: case 5: colour = RED;    // must be RED              (for traffic lights)
                acase 1: case 6: colour = ORANGE; // must be ORANGE or YELLOW (for traffic lights)
                acase 2: case 7: colour = GREEN;  // must be GREEN            (for traffic lights)
                adefault:        colour = YELLOW; // can be anything
            }   }
            elif (drawunlit)
            {   colour       = DARKBLUE;
            } else
            {   colour       = BLACK;
            }
            drawglow(164 + 14 + (i * 21), 130 + 41, colour);
    }   }

    if (drawmode)
    {   selbst_drawhelpgrid();
    }
    endofframe(vdu_bgc);
}

EXPORT UBYTE selbst_readport(int port)
{   FAST UBYTE t;

    // assert(machine == SELBST);

    switch (port)
    {
    case 7: // ASCII keyboard
        t = get_guest_key();
    acase 0xA: // for eg. VU-Meter
        t = (UBYTE) (rand() % 256);
    acase 0x17: // hex keypad
        if   (keys_column[0] & 1) /* C    */ t = 0x8C;
        elif (keys_column[0] & 2) /* 8    */ t = 0x88;
        elif (keys_column[0] & 4) /* 4    */ t = 0x84;
        elif (keys_column[0] & 8) /* 0    */ t = 0x80;
        elif (keys_column[1] & 1) /* D    */ t = 0x8D;
        elif (keys_column[1] & 2) /* 9    */ t = 0x89;
        elif (keys_column[1] & 4) /* 5    */ t = 0x85;
        elif (keys_column[1] & 8) /* 1    */ t = 0x81;
        elif (keys_column[2] & 1) /* E    */ t = 0x8E;
        elif (keys_column[2] & 2) /* A    */ t = 0x8A;
        elif (keys_column[2] & 4) /* 6    */ t = 0x86;
        elif (keys_column[2] & 8) /* 2    */ t = 0x82;
        elif (keys_column[3] & 1) /* F    */ t = 0x8F;
        elif (keys_column[3] & 2) /* B    */ t = 0x8B;
        elif (keys_column[3] & 4) /* 7    */ t = 0x87;
        elif (keys_column[3] & 8) /* 3    */ t = 0x83;
        elif (keys_column[4] & 1) /* CMD  */ t = 0xE0;
        elif (keys_column[4] & 2) /* RUN  */ t = 0xC0;
        elif (keys_column[4] & 4) /* GOTO */ t = 0xA0;
     // elif (keys_column[4] & 8) /* RST  */ t =    0;
     // elif (keys_column[5] & 1) /* FLAG */ t = 0xF0;
        elif (keys_column[5] & 2) /* MON  */ t = 0xD0;
        elif (keys_column[5] & 4) /* PC   */ t = 0xB0;
        elif (keys_column[5] & 8) /* NXT  */ t = 0x90;
        else                               t =    0;
    adefault:
        t = 0;
    }

    logport(port, t, FALSE);
    return t;
}

EXPORT void selbst_writeport(int port, UBYTE data)
{   switch (port)
    {
    case 0x09:
        if
        (   whichgame == METRONOMPOS
         || whichgame == SELBST_MORSEPOS
         || whichgame == OSCILLATORPOS
        )
        {   if (data == 0xFF)
            {   tone = 1;
            } else
            {   tone = 0;
            }
            playsound(FALSE);
        } else
        {   glow = data;
        }
    acase 0x10:
        digitleds[5] = data;
        drawsegments(5);
    acase 0x11:
        digitleds[4] = data;
        drawsegments(4);
    acase 0x12:
        digitleds[3] = data;
        drawsegments(3);
    acase 0x13:
        digitleds[2] = data;
        drawsegments(2);
    acase 0x14:
        digitleds[1] = data;
        drawsegments(1);
    acase 0x15:
        digitleds[0] = data;
        drawsegments(0);
    acase 0x1B: // VDU command port
        selbst_cout(data, selbst_char);
    acase 0x1C: // VDU data port
        selbst_char = data;
    acase PORTD:
        if (data & 8)
        {   if (verbosetape)
            {   zprintf(TEXTPEN_VERBOSE, "Tape motor has been turned on.\n");
            }
            if (tapemode == TAPEMODE_STOP)
            {   ; // could be either tape_play() or tape_record() so we do nothing
        }   }
        else
        {   if (verbosetape)
            {   zprintf(TEXTPEN_VERBOSE, "Tape motor has been turned off.\n");
            }
            if (tapemode > TAPEMODE_STOP)
            {   tape_stop();
    }   }   }

    logport(port, data, TRUE);
}

EXPORT void selbst_viewscreen(void)
{   TEXT rowchars[16 + 1];
    int  x, xx, y;

    // assert(machine == SELBST);

    rowchars[16] = EOS;

    for (y = 0; y < 24; y++)
    {   for (xx = 0; xx < MAX_VDUCOLUMNS; xx += 16)
        {   for (x = 0; x < 16; x++)
            {   if (vdu[xx + x][y] <= 31) rowchars[x] =
#ifdef AMIGA
(TEXT) '·';
#endif
#ifdef WIN32
'.';
#endif
                else rowchars[x] = vdu[xx + x][y];
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
        "%02X %02X %02X %02X %02X %02X  %c%c%c%c%c%c%c%c%c%c%c%c\n\n",
        digitleds[0],
        digitleds[1],
        digitleds[2],
        digitleds[3],
        digitleds[4],
        digitleds[5],
        led_chars[digitleds[0] & 0x7F], (digitleds[0] & 0x80) ? '.' : ' ',
        led_chars[digitleds[1] & 0x7F], (digitleds[1] & 0x80) ? '.' : ' ',
        led_chars[digitleds[2] & 0x7F], (digitleds[2] & 0x80) ? '.' : ' ',
        led_chars[digitleds[3] & 0x7F], (digitleds[3] & 0x80) ? '.' : ' ',
        led_chars[digitleds[4] & 0x7F], (digitleds[4] & 0x80) ? '.' : ' ',
        led_chars[digitleds[5] & 0x7F], (digitleds[5] & 0x80) ? '.' : ' '
    );
}

MODULE void selbst_cout(UBYTE command, TEXT thechar)
{   int   x, y;
    UBYTE temp[64];

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "COUT: Command: $%02X. Data: %s.\n", command, asciiname_long[thechar]);
#endif

    if (command & 0x80)
    {   return;
    } // implied else
    command &= 0xF;

/*: "home cursor"   (0) = "page erase & home cursor" (8)
    "return cursor" (1) = "erase to end of line & return cursor" (9)
    "roll screen"   (2) = "linefeed" (10) */

    switch (command)
    {
    case 0: // home
        vdu_x = vdu_y = 0;
    acase 1: // cr (aka return cursor)
        vdu_x = 0;
    acase 2: // roll screen
        for (x = 0; x < 64; x++)
        {   temp[x] = vdu[x][0];
        }
        for (y = 0; y <= 14; y++)
        {   for (x = 0; x < 64; x++)
            {   vdu[x][y] = vdu[x][y + 1];
        }   }
        for (x = 0; x < 64; x++)
        {   vdu[x][15] = temp[x];
        }
    acase 3: // nop
    case 11:
        ;
    acase 4: // left
    case 12:
        if (vdu_x > 0) // what should happen if vdu_x == 0?
        {   vdu_x--;
        }
    acase 5: // erase this line
    case 13:
        for (x = 0; x < 64; x++)
        {   vdu[x][vdu_y] = blank;
        }
    acase 6: // up
    case 14:
        if (vdu_y > 0) // what should happen if vdu_y == 0?
        {   vdu_y--;
        }
    acase 7: // right
        if (vdu_x < 63) // what should happen if vdu_x == 63?
        {   vdu_x++;
        }
    acase 8: // cls (page erase and cursor home to top left)
        for (x = 0; x < 64; x++)
        {   for (y = 0; y < 16; y++)
            {   vdu[x][y] = thechar;
        }   }
        vdu_x = vdu_y = 0;
    acase 9: // erase to end of line and return cursor
        if (vdu_x < 63)
        {   for (x = vdu_x + 1; x <= 63; x++)
            {   vdu[x][vdu_y] = 0;
        }   }
        vdu_x = 0;
    acase 0xA: // lf
        thecout(LF);
    acase 0xF:
        thecout(thechar);
}   }

EXPORT void selbst_biosdetails(int ea)
{   // assert(machine == SELBST);

    if (ea == OUTOFRANGE)
    {   return; // for speed
    }

    switch (selbst_biosver)
    {
    case SELBST_BIOS00:
        return; // for speed
    case SELBST_BIOS09:
    case SELBST_BIOS10:
        switch (ea)
        {
        case 0x3: // HEXDEZ
        case 0x5E9: // HEXBCD
        case 0x61F:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x5E9)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x61F)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           4-digit hex to 5-digit packed BCD number\n" \
                ";Input:              *($82B..$82C)\n" \
                ";Return code:        *($82E..$830)\n" \
                ";Registers written:  R0..R3, PSL\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0x5: // CONV
        case 0x230: // CONVT
        case 0x22C:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x230)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x22C)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Convert 1 byte into 7-segment LED representation for output\n" \
                ";Input:              R3\n" \
                ";Return code:        R3\n" \
                ";Registers written:  R3\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0x7: // SEP
        case 0x207: // SEPNIB
        case 0x203:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x207)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x203)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Separate 1 byte into the right nybbles of 2 bytes\n" \
                ";Input:              *($801)\n" \
                ";Return code:        *($801..$802)\n" \
                ";Registers written:  R3\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0x9: // COMB
        case 0x21E: // COMNIB
        case 0x21A:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x21E)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x21A)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Combine the right nybbles of 2 bytes into 1 byte\n" \
                ";Input:              *($801..$802)\n" \
                ";Return code:        *($801)\n" \
                ";Registers written:  R3\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0xB: // DIS
        case 0xE5: // DISP
        case 0xDD:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0xE5)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0xDD)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Shows the contents of $80D..$812 on the display\n" \
                ";Input:              *($80D..$812)\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0xD: // KIN
        case 0xC4: // KIN1
        case 0xBC:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0xC4)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0xBC)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Asks the keyboard for input until a key is depressed\n" \
                ";Input:              None\n" \
                ";Return code:        *($800)\n" \
                ";Registers written:  R0, R3\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0xF: // INIT
        case 0x235: // INIT1
        case 0x28A:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x235)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x28A)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Sets all registers to zero\n" \
                ";Input:              None\n" \
                ";Return code:        None\n" \
                ";Registers written:  All\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0x11: // SAVE
        case 0x24E: // INIT2
        case 0x2A1:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x24E)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x2A1)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Saves the contents of all registers\n" \
                ";Input:              None\n" \
                ";Return code:        None\n" \
                ";Registers written:  None\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0x13: // RECAL
        case 0x272: // INIT3
        case 0x2C5:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x272)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x2C5)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Recalls the contents of all registers\n" \
                ";Input:              None\n" \
                ";Return code:        None\n" \
                ";Registers written:  All\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0x15: // CDIS
        case 0x296: // CDISP
        case 0x2E9:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x296)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x2E9)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Clears the display memory (from $80D..$812)\n" \
                ";Input:              None\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0, R1\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0x19: // LODAT
        case 0x149: // LOAD
        case 0x140:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x149)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x140)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Allows entering a 2-, 4- or 5-digit number via keyboard and stores it in 3 bytes\n" \
                ";Input:              *($82D)\n" \
                ";Return code:        *($81A..$81C)\n" \
                ";Registers written:  R0..R3\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0x1B: // ADDSUB
        case 0x6A0: // ADSB
        case 0x6D6:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x6A0)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x6D6)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Addition/subtraction of negative/positive 4-digit hex numbers (2 bytes each)\n" \
                ";Comments:           The COM bit in the PSL indicates if subtraction (0) or addition (1) is desired\n" \
                ";Input:              *($827..$828)=first number, *($829..$82A)=second number, PSL\n" \
                ";Return code:        *($82B..$82C)\n" \
                ";Registers written:  R0..R1, PSL\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0x1D: // MULT
        case 0x6CD: // MPYS
        case 0x703:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x6CD)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x703)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Multiply two 2-digit positive/negative hex numbers (each 1 byte long)\n" \
                ";Input:              *($827), *($829)\n" \
                ";Return code:        *($82B..$82C)\n" \
                ";Registers written:  R0..R3, PSL\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0x1F: // DIV
        case 0x700: // DIVS
        case 0x736:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x700)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x736)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Divide 4-digit hex dividend (2 bytes) by 2-digit hex divisor (1 byte)\n" \
                ";Comments:           Positive/negative dividend is lost, negative divisor becomes positive\n" \
                ";Input:              *($827..$828)=dividend, *($829)=divisor\n" \
                ";Return code:        *($82B..$82C)=result, R1=remainder\n" \
                ";Registers written:  R0..R3, PSL\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0x21: // DEZHEX
        case 0x649: // BCDHEX
        case 0x67F:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x649)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x67F)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Convert 5-digit packed BCD number with sign to 4-digit hex number\n" \
                ";Input:              *($82E..$830)\n" \
                ";Return code:        *($82B..$82C)\n" \
                ";Registers written:  R0..R3, PSL\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0x25: // HEXD
        case 0x774: // HEXD1
        case 0x7AA:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x774)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x7AA)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Print 4-digit hex number as a 5-digit packed BCD number with sign\n" \
                ";Input:              *($82B..$82C)\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0..R3, PSL\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0x27: // DHEX
        case 0x79D: // DHEX1
        case 0x7D3:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x79D)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x7D3)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Print 5-digit packed BCD number with sign as a 4-digit hex number\n" \
                ";Input:              *($82E..$830)\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0..R3, PSL\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0x29: // HEX
        case 0x79F: // DHEX2
        case 0x7D5:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x79F)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x7D5)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Print 4-digit hex number\n" \
                ";Input:              *($82B..$82C)\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0..R3\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        acase 0x2B: // DEZ
        case 0x776: // HEXD2
        case 0x7AC:
            if
            (   (selbst_biosver != SELBST_BIOS09 && ea == 0x776)
             || (selbst_biosver != SELBST_BIOS10 && ea == 0x7AC)
            )
            {   break;
            }
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Print 5-digit packed BCD number with sign\n" \
                ";Input:              *($82B..$82C)\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0..R3\n" \
                ";BIOS:               V0.9, V1.0\n"
            );
        }
    acase SELBST_BIOS20:
        switch (ea)
        {
        case 0x14: // KIN
        case 0x384: // KBIN
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Fetches value from the ASCII keyboard and stores it in R3\n" \
                ";Input:              None\n" \
                ";Return code:        R3\n" \
                ";Registers written:  R3\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x16: // WBL
        case 0x16A: // WRBL
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Writes a blank to the display\n" \
                ";Input:              None\n" \
                ";Return code:        None\n" \
                ";Registers written:  None\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x18: // WCH
        case 0x425: // WCHR
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Writes the character in R3 to the display\n" \
                ";Input:              R3\n" \
                ";Return code:        None\n" \
                ";Registers written:  None\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x1A: // INPHX
        case 0x1B6: // INHX
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Inputs 2 hex digits via keyboard and stores them in R3\n" \
                ";Input:              None\n" \
                ";Return code:        R3\n" \
                ";Registers written:  R0, R1, R3\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x1C: // OUTHX
        case 0x4F: // HXO
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Outputs R2 as 2 hex digits\n" \
                ";Input:              R2\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0..R3\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x1E: // LINEF
        case 0x45: // LFC
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Outputs a linefeed\n" \
                ";Input:              None\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0, R3\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x20: // PUT
        case 0x2C1: // SERO
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Outputs R3 to cassette\n" \
                ";Input:              R3\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0, R2, R3\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x22: // GET
        case 0x35E: // SERI
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Inputs from cassette to R3\n" \
                ";Input:              None\n" \
                ";Return code:        R3\n" \
                ";Registers written:  R0, R2, R3\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x24: // HXTAB
        case 0x19A: // TABL
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Converts the hex digit in R3 from ASCII to binary\n" \
                ";Input:              None\n" \
                ";Return code:        R3\n" \
                ";Registers written:  R0, R3\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x26: // ADRES
        case 0x185: // ADDR
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Inputs, outputs, and stores one address\n" \
                ";Input:              None\n" \
                ";Return code:        None?\n" \
                ";Registers written:  R0, R3\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x28: // CONTR
        case 0x48C: // CR
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Executes the control function stored in R0\n" \
                ";Input:              None\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0..R3\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x2A: // HO
        case 0x4E5: // HOME1
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Cursor home\n" \
                ";Input:              None\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0..R3\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x2C: // CURSL
        case 0x4EE: // CURL3
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Cursor left\n" \
                ";Input:              None\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0..R3\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x2E: // CURSR
        case 0x52B: // CURR1
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Cursor right\n" \
                ";Input:              None\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0..R3\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x30: // CURSU
        case 0x536: // CUUP1
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Cursor up\n" \
                ";Input:              None\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0..R3\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x32: // CURSD
        case 0x4C2: // LF1
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Cursor down\n" \
                ";Input:              None\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0..R3\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x34: // SCCLR
        case 0x4CA: // CLR2
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Clear screen, cursor home\n" \
                ";Input:              None\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0..R3\n" \
                ";BIOS:               V2.0\n"
            );
        acase 0x36: // BACK
        case 0x51A: // BACK1
            zprintf
            (   TEXTPEN_LOG,
                ";Function:           Backspace\n" \
                ";Input:              None\n" \
                ";Return code:        None\n" \
                ";Registers written:  R0..R3\n" \
                ";BIOS:               V2.0\n"
            );
}   }   }

EXPORT void selbst_drawhelpgrid(void)
{   int x,  y,
        xx, yy,
        startx,
        starty;

    for (x = 0; x < 64; x++)
    {   for (y = 0; y < 16; y++)
        {   startx = x * 8;
            starty = y * 8;
            for (xx = 0; xx < 8; xx++)
            {   for (yy = 0; yy < 8; yy++)
                {   if (xx == 0 || xx == 8 - 1 || yy == 0 || yy == 8 - 1)
                    {   changepixel(startx + xx, starty + yy, GREY1);
}   }   }   }   }   }

EXPORT void selbst_reset(void)
{   int i;

    // keyboard
    queuepos = 0;

    // screen
    vdu_x = vdu_y = 0;

    // sound
    tone = 0;

    // LED digits
    for (i = 0; i < DIGITLEDS; i++)
    {   digitleds[i] = 0;
    }

    // game
    if (game)
    {   iar = (startaddr_h * 256) + startaddr_l;
    } else
    {   iar = 0;
}   }
