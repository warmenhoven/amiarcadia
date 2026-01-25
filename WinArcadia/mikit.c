// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
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

// DEFINES----------------------------------------------------------------

// (None)

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT const int mikit_digitxloc[6] = { 474, 503, 531, 559, 603, 631 };

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT ASCREEN                    screen[BOXWIDTH][BOXHEIGHT];
IMPORT FLAG                       inframe;
IMPORT UBYTE                      digitleds[DIGITLEDS],
                                  memory[32768],
                                  keys_column[7],
                                  glow,
                                  OutputBuffer[18],
                                  startaddr_h, startaddr_l,
                                  tone;
IMPORT UWORD                      console[4],
                                  iar,
                                  keypads[2][NUMKEYS],
                                  mirror_r[32768],
                                  mirror_w[32768];
IMPORT ULONG                      frames;
IMPORT int                        drawunlit,
                                  frameskip,
                                  game,
                                  machine,
                                  memmap,
                                  offset,
                                  recmode,
                                  slice_2650;
IMPORT MEMFLAG                    memflags[ALLTOKENS];
IMPORT UBYTE*                     IOBuffer;
IMPORT FILE*                      MacroHandle;
IMPORT const TEXT                 led_chars[128 + 1];
IMPORT const struct CodeCommentStruct codecomment[];
IMPORT       struct MachineStruct machines[MACHINES];
#ifdef AMIGA
    IMPORT struct Catalog*        CatalogPtr;
    IMPORT struct Window*         SubWindowPtr[SUBWINDOWS];
#endif
#ifdef WIN32
    IMPORT int                    CatalogPtr; // APTR doesn't work
    IMPORT ULONG                  pencolours[COLOURSETS][PENS];
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE const UBYTE mikit_bios[30] =
{ 0x04, 0x20,       // LODI,R0 $40
  0xCC, 0x04, 0x02, // STRA,r0 $0402
  0xCC, 0x04, 0x03, // STRA,r0 $0403
  0xCC, 0x04, 0x04, // STRA,r0 $0404
  0xCC, 0x04, 0x06, // STRA,r0 $0406
  0xCC, 0x04, 0x07, // STRA,r0 $0407
// $11:
  0x30,             // REDC,r0
  0xB0,             // WRTC,r0
  0xE4, 0xFF,       // COMI,r0 255
  0x98, 0x02,       // BCFR,eq $19
  0x04, 0x20,       // LODI,r0 $20
// $19:
  0xCC, 0x04, 0x05, // STRA,r0 $0405
  0x1B, 0x73        // BCTR,un $11
};

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void mikit_emuinput(void);

// CODE-------------------------------------------------------------------

MODULE void mikit_emuinput(void)
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
        t |= (UBYTE) (0x1 * KeyDown(keypads[0][ 1])); // BLANK
        t |= (UBYTE) (0x2 * KeyDown(keypads[0][ 4])); // +
        t |= (UBYTE) (0x4 * KeyDown(keypads[0][ 7])); // H
        t |= (UBYTE) (0x8 * KeyDown(keypads[0][10])); // L
        keys_column[0] = t;

        t = 0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[0][ 2])); // R
        t |= (UBYTE) (0x2 * KeyDown(keypads[0][ 5])); // G
        t |= (UBYTE) (0x4 * KeyDown(keypads[0][ 8])); // P
        t |= (UBYTE) (0x8 * KeyDown(keypads[0][11])); // S
        keys_column[1] = t;

        t = 0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[0][ 3])); // C
        t |= (UBYTE) (0x2 * KeyDown(keypads[0][ 6])); // 8
        t |= (UBYTE) (0x4 * KeyDown(keypads[0][ 9])); // 4
        t |= (UBYTE) (0x8 * KeyDown(keypads[0][12])); // 0
        keys_column[2] = t;

        t = 0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[1][ 1])); // D
        t |= (UBYTE) (0x2 * KeyDown(keypads[1][ 4])); // 9
        t |= (UBYTE) (0x4 * KeyDown(keypads[1][ 7])); // 5
        t |= (UBYTE) (0x8 * KeyDown(keypads[1][10])); // 1
        keys_column[3] = t;

        t = 0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[1][ 2])); // E
        t |= (UBYTE) (0x2 * KeyDown(keypads[1][ 5])); // A
        t |= (UBYTE) (0x4 * KeyDown(keypads[1][ 8])); // 6
        t |= (UBYTE) (0x8 * KeyDown(keypads[1][11])); // 2
        keys_column[4] = t;

        t = 0;
        t |= (UBYTE) (0x1 * KeyDown(keypads[1][ 3])); // F
        t |= (UBYTE) (0x2 * KeyDown(keypads[1][ 6])); // B
        t |= (UBYTE) (0x4 * KeyDown(keypads[1][ 9])); // 7
        t |= (UBYTE) (0x8 * KeyDown(keypads[1][12])); // 3
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
}   }

EXPORT void mikit_setmemmap(void)
{   int i, address, mirror;

    game = FALSE;

    for (i = 0; i < 30; i++)
    {   memory[i] = mikit_bios[i];
    }
    for (i = 30; i <= 0x3FF; i++)
    {   memory[i] = 0x17; // RETC,un
    }
    for (i = 0x400; i <= 0x7FFF; i++)
    {   memory[i] = 0;
    }

    for (address = 0; address < 32 * KILOBYTE; address++)
    {   memflags[address] = 0;
        mirror                 = address;

        if (address <= 0x3FF)
        {   memflags[address] |= BIOS;
        }
        if
        (   address <= 0x400
         || address >= 0x800
        )
        {   memflags[address] |= NOWRITE;
        }
        if
        (   address >= 0x800
        )
        {   memflags[address] |= NOREAD;
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
}   }   }

EXPORT void mikit_emulate(void)
{   FAST int colour,
             i;

    inframe = TRUE;

    // 1MHz = 1,000,000 cycles per second
    // 1,000,000 ÷ 50 = 20,000
    slice_2650 += 20000;
    cpu_2650_untapable();

    wa_checkinput();
    mikit_emuinput();

    if (frameskip != FRAMESKIP_INFINITE && frames % (ULONG) frameskip == 0)
    {   for (i = 0; i < 6; i++)
        {   drawdigit(i, (UBYTE) (memory[0x402 + i] & 0x7F));
        }

        for (i = 0; i < 8; i++)
        {   if (glow & (128 >> i))
            {   colour   = RED;
            } elif (drawunlit)
            {   colour       = DARKBLUE;
            } else
            {   colour       = BLACK;
            }
            drawglow(14 + (i * 21), 49, colour);
    }   }

    endofframe(BLACK);
}

EXPORT UBYTE mikit_readport(int port)
{   FAST UBYTE t;

    // assert(machine == MIKIT);

    switch (port)
    {
    case PORTC:
        // These values are all just guesses and probably wrong...
        if   (keys_column[0] == 1) /* BLANK */ t = ' ';
        elif (keys_column[0] == 2) /* +     */ t = '+';
        elif (keys_column[0] == 4) /* H     */ t = 'H';
        elif (keys_column[0] == 8) /* L     */ t = 'L';
        elif (keys_column[1] == 1) /* R     */ t = 'R';
        elif (keys_column[1] == 2) /* G     */ t = 'G';
        elif (keys_column[1] == 4) /* P     */ t = 'P';
        elif (keys_column[1] == 8) /* S     */ t = 'S';
        elif (keys_column[2] == 8) /* 0     */ t = '0';
        elif (keys_column[2] == 4) /* 4     */ t = '4';
        elif (keys_column[2] == 2) /* 8     */ t = '8';
        elif (keys_column[2] == 1) /* C     */ t = 'C';
        elif (keys_column[3] == 8) /* 1     */ t = '1';
        elif (keys_column[3] == 4) /* 5     */ t = '5';
        elif (keys_column[3] == 2) /* 9     */ t = '9';
        elif (keys_column[3] == 1) /* D     */ t = 'D';
        elif (keys_column[4] == 8) /* 2     */ t = '2';
        elif (keys_column[4] == 4) /* 6     */ t = '6';
        elif (keys_column[4] == 2) /* A     */ t = 'A';
        elif (keys_column[4] == 1) /* E     */ t = 'E';
        elif (keys_column[5] == 8) /* 3     */ t = '3';
        elif (keys_column[5] == 4) /* 7     */ t = '7';
        elif (keys_column[5] == 2) /* B     */ t = 'B';
        elif (keys_column[5] == 1) /* F     */ t = 'F';
        else                                   t = 0xFF;
    adefault:
        t = 0;
    }

    logport(port, t, FALSE);
    return t;
}

EXPORT void mikit_writeport(int port, UBYTE data)
{   switch (port)
    {
    case PORTC:
        glow = data;
    }

    logport(port, data, TRUE);
}

EXPORT void mikit_viewscreen(void)
{   // assert(machine == MIKIT);

    zprintf
    (   TEXTPEN_VIEW,
        "%02X %02X %02X %02X %02X %02X  %c%c%c%c %c%c\n\n",
        digitleds[0],
        digitleds[1],
        digitleds[2],
        digitleds[3],
        digitleds[4],
        digitleds[5],
        led_chars[digitleds[0] & 0x7F],
        led_chars[digitleds[1] & 0x7F],
        led_chars[digitleds[2] & 0x7F],
        led_chars[digitleds[3] & 0x7F],
        led_chars[digitleds[4] & 0x7F],
        led_chars[digitleds[5] & 0x7F]
    );
}

EXPORT float mikit_retune(UBYTE hertz)
{   switch (hertz)
    {
    case  0x83: // 131
        return (float) 130.81; // low C
    acase 0x93: // 147
        return (float) 146.83; // low D
    acase 0xA5: // 165
        return (float) 164.81; // low E
    acase 0xAF: // 175
        return (float) 174.61; // low F
    acase 0xC4: // 196
        return (float) 196.00; // low G
    acase 0xDC: // 220
        return (float) 220.00; // low A
    acase 0xF7: // 247
        return (float) 246.94; // low B
    acase 0xFF: // 255
        return (float) 261.63; // middle C
    adefault:
        /* We could do proper handling of all possible input values,
        but the ones above are the only ones that are actually used. */
        return (float) hertz;
}   }

EXPORT void mikit_reset(void)
{   int i;

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
