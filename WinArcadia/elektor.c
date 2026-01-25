// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
    #include <string.h>
    #include <stdlib.h>
    #include <proto/locale.h> // GetCatalogStr()
#endif
#ifdef WIN32
    #include "ibm.h"
#endif

#include <stdio.h>

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#ifdef WIN32
    #define EXEC_TYPES_H
    #include "aa_strings.h"
    #undef EXEC_TYPES_H
#endif
#ifdef AMIGA
    #include "aa_strings.h"
    #include <proto/locale.h>        // GetCatalogStr()
#endif

#include "elektor.h"

// DEFINES----------------------------------------------------------------

// #define WATCHENVELOPES
// to put watchpoints on writes to PSG envelope bits
// (for detection of which, if any, games use PSG envelopes)

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       TEXT                 letters[8 + 1];

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       FLAG                 bangercharging;
IMPORT       TEXT                 gtempstring[256 + 1];
IMPORT       UBYTE                memory[32768],
                                  psl, psu,
                                  r[7],
                                  startaddr_h,
                                  startaddr_l;
IMPORT       UWORD                iar,
                                  mirror_r[32768],
                                  mirror_w[32768];
IMPORT       int                  cpl,
                                  elektor_biosver,
                                  game,
                                  machine,
                                  memmap,
                                  nextinst,
                                  offset,
                                  pvibase,
                                  verbosity,
                                  watchwrites,
                                  whichgame;
IMPORT       MEMFLAG              memflags[ALLTOKENS];
IMPORT       UBYTE*               IOBuffer;
IMPORT       struct ConditionalStruct wp[ALLTOKENS];
IMPORT       struct KnownStruct   known[KNOWNGAMES + 1];
IMPORT       struct MachineStruct machines[MACHINES + 1];
IMPORT       struct NoteStruct    notes[NOTES + 1];
IMPORT const struct CodeCommentStruct codecomment[];

#ifdef AMIGA
    IMPORT   struct Catalog*      CatalogPtr;
#endif
#ifdef WIN32
    IMPORT   int                  CatalogPtr; // APTR doesn't work
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE       int                  cursor;
MODULE       UBYTE                EOFBuffer[6 + 255 + 1];

/* MODULE FUNCTIONS---------------------------------------------------- */

MODULE UBYTE eof_makebcc(int start, int end, UBYTE* BufferPtr);
MODULE int eof_readblock(int localsize);

/* CODE---------------------------------------------------------------- */

EXPORT void view_psgs(void)
{   TRANSIENT       float  hertz;
    FAST            TEXT   tonestring[80 + 1];
    PERSIST   const STRPTR waveshape[16] = {
  "\\_______________",                // %0000 single decay then off
  "\\_______________",                // %0001 single decay then off
  "\\_______________",                // %0010 single decay then off
  "\\_______________",                // %0011 single decay then off
  "/_______________",                 // %0100 single attack then off
  "/_______________",                 // %0101 single attack then off
  "/_______________",                 // %0110 single attack then off
  "/_______________",                 // %0111 single attack then off
  "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\", // %1000 repeated decay
  "\\_______________",                // %1001 single decay then off
  "\\/\\/\\/\\/\\/\\/\\/\\/",         // %1010 repeated decay-attack
  "\\\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"", // %1011 single decay then hold
  "////////////////",                 // %1100 repeated attack
  "/\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"",  // %1101 single attack then hold
  "/\\/\\/\\/\\/\\/\\/\\/\\",         // %1110 repeated attack-decay
  "/_______________"                  // %1111 single attack then off
};

    if (memmap != MEMMAP_F)
    {   return;
    }

    zprintf
    (   TEXTPEN_VIEW,
        "---------------PSG #1---------------- " \
        "---------------PSG #2----------------\n"
    );
    zprintf
    (   TEXTPEN_VIEW,
        "PITCHA1:      %04X PITCHD1:        %02X PITCHA2:      %04X PITCHD2:        %02X\n",
        (memory[PSG_PITCHA1_H] * 256) + memory[PSG_PITCHA1_L],
         memory[PSG_PITCHD1],
        (memory[PSG_PITCHA2_H] * 256) + memory[PSG_PITCHA2_L],
         memory[PSG_PITCHD2]
    );
    zprintf
    (   TEXTPEN_VIEW,
        "PITCHB1:      %04X MIXER1:         %02X PITCHB2:      %04X MIXER2:         %02X\n",
        (memory[PSG_PITCHB1_H] * 256) + memory[PSG_PITCHB1_L],
         memory[PSG_MIXER1],
        (memory[PSG_PITCHB2_H] * 256) + memory[PSG_PITCHB2_L],
         memory[PSG_MIXER2]
    );
    zprintf
    (   TEXTPEN_VIEW,
        "PITCHC1:      %04X PERIOD1:      %04X PITCHC2:      %04X PERIOD2:      %04X\n",
        (memory[PSG_PITCHC1_H] * 256) + memory[PSG_PITCHC1_L],
        (memory[PSG_PERIOD1_H] * 256) + memory[PSG_PERIOD1_L],
        (memory[PSG_PITCHC2_H] * 256) + memory[PSG_PITCHC2_L],
        (memory[PSG_PERIOD2_H] * 256) + memory[PSG_PERIOD2_L]
    );
    zprintf
    (   TEXTPEN_VIEW,
        "AMPLITUDEA1:    %02X SHAPE1:         %02X AMPLITUDEA2:    %02X SHAPE2:         %02X\n",
        memory[PSG_AMPLITUDEA1],
        memory[PSG_SHAPE1     ],
        memory[PSG_AMPLITUDEA2],
        memory[PSG_SHAPE2     ]
    );
    zprintf
    (   TEXTPEN_VIEW,
        "AMPLITUDEB1:    %02X PORTA1:         %02X AMPLITUDEB2:    %02X PORTA2:         %02X\n",
        memory[PSG_AMPLITUDEB1],
        memory[PSG_PORTA1     ],
        memory[PSG_AMPLITUDEB2],
        memory[PSG_PORTA2     ]
    );
    zprintf
    (   TEXTPEN_VIEW,
        "AMPLITUDEC1:    %02X PORTB1:         %02X AMPLITUDEC2:    %02X PORTB2:         %02X\n",
        memory[PSG_AMPLITUDEC1],
        memory[PSG_PORTB1     ],
        memory[PSG_AMPLITUDEC2],
        memory[PSG_PORTB2     ]
    );

    if (verbosity >= 1)
    {   zprintf
        (   TEXTPEN_VIEW,
            "                 " \
            "-----------PSG #1----------- " \
            "-----------PSG #2------------\n"
        );

        sprintf
        (   (char*) tonestring,
            LLL(
                MSG_TONEPITCH,
                "Tone %s pitch:"
            ),
            "A"
        );
        hertz = ((memory[PSG_PITCHA1_H] & 0x0F) == 0 && memory[PSG_PITCHA1_L] == 0)
              ? (float)  110837.0
              : (float) (110837.0 / (((memory[PSG_PITCHA1_H] & 0x0F) * 256) + memory[PSG_PITCHA1_L]));
        sprintf
        (   (char*) gtempstring,
            "%-16s %6.3f %s (note %s) ",
            tonestring,
            hertz,
            LLL( MSG_HERTZ, "Hz"),
            notes[hertz_to_note(hertz)].name
        );
        hertz = ((memory[PSG_PITCHA2_H] & 0x0F) == 0 && memory[PSG_PITCHA2_L] == 0)
              ? (float)  110837.0
              : (float) (110837.0 / (((memory[PSG_PITCHA2_H] & 0x0F) * 256) + memory[PSG_PITCHA2_L]));
        zprintf
        (   TEXTPEN_VIEW,
            "%-45s %6.3f %s (note %s)\n",
            gtempstring,
            hertz,
            LLL( MSG_HERTZ, "Hz"),
            notes[hertz_to_note(hertz)].name
        );

        sprintf
        (   (char*) tonestring,
            LLL(
                MSG_TONEPITCH,
                "Tone %s pitch:"
            ),
            "B"
        );
        hertz = ((memory[PSG_PITCHB1_H] & 0x0F) == 0 && memory[PSG_PITCHB1_L] == 0)
              ? (float)  110837.0
              : (float) (110837.0 / (((memory[PSG_PITCHB1_H] & 0x0F) * 256) + memory[PSG_PITCHB1_L]));
        sprintf
        (   (char*) gtempstring,
            "%-16s %6.3f %s (note %s) ",
            tonestring,
            hertz,
            LLL( MSG_HERTZ, "Hz"),
            notes[hertz_to_note(hertz)].name
        );
        hertz = ((memory[PSG_PITCHB2_H] & 0x0F) == 0 && memory[PSG_PITCHB2_L] == 0)
              ? (float)  110837.0
              : (float) (110837.0 / (((memory[PSG_PITCHB2_H] & 0x0F) * 256) + memory[PSG_PITCHB2_L]));
        zprintf
        (   TEXTPEN_VIEW,
            "%-45s %6.3f %s (note %s)\n",
            gtempstring,
            hertz,
            LLL( MSG_HERTZ, "Hz"),
            notes[hertz_to_note(hertz)].name
        );

        sprintf
        (   (char*) tonestring,
            LLL(
                MSG_TONEPITCH,
                "Tone %s pitch:"
            ),
            "C"
        );
        hertz = ((memory[PSG_PITCHC1_H] & 0x0F) == 0 && memory[PSG_PITCHC1_L] == 0)
              ? (float)  110837.0
              : (float) (110837.0 / (((memory[PSG_PITCHC1_H] & 0x0F) * 256) + memory[PSG_PITCHC1_L]));
        sprintf
        (   (char*) gtempstring,
            "%-16s %6.3f %s (note %s) ",
            tonestring,
            hertz,
            LLL( MSG_HERTZ, "Hz"),
            notes[hertz_to_note(hertz)].name
        );
        hertz = ((memory[PSG_PITCHC2_H] & 0x0F) == 0 && memory[PSG_PITCHC2_L] == 0)
              ? (float)  110837.0
              : (float) (110837.0 / (((memory[PSG_PITCHC2_H] & 0x0F) * 256) + memory[PSG_PITCHC2_L]));
        zprintf
        (   TEXTPEN_VIEW,
            "%-45s %6.3f %s (note %s)\n",
            gtempstring,
            hertz,
            LLL( MSG_HERTZ, "Hz"),
            notes[hertz_to_note(hertz)].name
        );

        hertz = ((memory[PSG_PITCHD1]   & 0x1F) == 0)
              ? (float)  110837.0
              : (float) (110837.0 / (memory[PSG_PITCHD1] & 0x1F));
        sprintf
        (   (char*) gtempstring,
            "%-16s %6.3f %s (note %s) ",
            LLL(
                MSG_NOISESPITCH,
                "Noises pitch:"
            ),
            hertz,
            LLL( MSG_HERTZ, "Hz"),
            notes[hertz_to_note(hertz)].name
        );
        hertz = ((memory[PSG_PITCHD2]   & 0x1F) == 0)
              ? (float)  110837.0
              : (float) (110837.0 / (memory[PSG_PITCHD2] & 0x1F));
        zprintf
        (   TEXTPEN_VIEW,
            "%-45s %6.3f %s (note %s)\n",
            gtempstring,
            hertz,
            LLL( MSG_HERTZ, "Hz"),
            notes[hertz_to_note(hertz)].name
        );

        zprintf
        (   TEXTPEN_VIEW,
            "%-16s A=%-6s, B=%-6s, C=%-6s A=%-6s, B=%-6s, C=%-6s\n",
            LLL(
                MSG_TONECHANNELS,
                "Tone channels:"
            ),
            (memory[PSG_MIXER1] & 0x01) ? LLL( MSG_ENGINE_OFF3, "off") : LLL( MSG_ENGINE_ON3, "on"),
            (memory[PSG_MIXER1] & 0x02) ? LLL( MSG_ENGINE_OFF3, "off") : LLL( MSG_ENGINE_ON3, "on"),
            (memory[PSG_MIXER1] & 0x04) ? LLL( MSG_ENGINE_OFF3, "off") : LLL( MSG_ENGINE_ON3, "on"),
            (memory[PSG_MIXER2] & 0x01) ? LLL( MSG_ENGINE_OFF3, "off") : LLL( MSG_ENGINE_ON3, "on"),
            (memory[PSG_MIXER2] & 0x02) ? LLL( MSG_ENGINE_OFF3, "off") : LLL( MSG_ENGINE_ON3, "on"),
            (memory[PSG_MIXER2] & 0x04) ? LLL( MSG_ENGINE_OFF3, "off") : LLL( MSG_ENGINE_ON3, "on")
        );
        zprintf
        (   TEXTPEN_VIEW,
            "%-16s A=%-6s, B=%-6s, C=%-6s A=%-6s, B=%-6s, C=%-6s\n",
            LLL(
                MSG_NOISECHANNELS,
                "Noise channels:"
            ),
            (memory[PSG_MIXER1] & 0x08) ? LLL( MSG_ENGINE_OFF3, "off") : LLL( MSG_ENGINE_ON3, "on"),
            (memory[PSG_MIXER1] & 0x10) ? LLL( MSG_ENGINE_OFF3, "off") : LLL( MSG_ENGINE_ON3, "on"),
            (memory[PSG_MIXER1] & 0x20) ? LLL( MSG_ENGINE_OFF3, "off") : LLL( MSG_ENGINE_ON3, "on"),
            (memory[PSG_MIXER2] & 0x08) ? LLL( MSG_ENGINE_OFF3, "off") : LLL( MSG_ENGINE_ON3, "on"),
            (memory[PSG_MIXER2] & 0x10) ? LLL( MSG_ENGINE_OFF3, "off") : LLL( MSG_ENGINE_ON3, "on"),
            (memory[PSG_MIXER2] & 0x20) ? LLL( MSG_ENGINE_OFF3, "off") : LLL( MSG_ENGINE_ON3, "on")
        );
        zprintf
        (   TEXTPEN_VIEW,
            "%-16s A=%-6s, B=%-6s           A=%s, B=%s\n",
            LLL(
                MSG_IOPORTS,
                "I/O ports:"
            ),
            (memory[PSG_MIXER1] & 0x40) ? LLL( MSG_OUTPUT, "output") : LLL( MSG_DISABLED2, "disbld"),
            (memory[PSG_MIXER1] & 0x80) ? LLL( MSG_OUTPUT, "output") : LLL( MSG_DISABLED2, "disbld"),
            (memory[PSG_MIXER2] & 0x40) ? LLL( MSG_OUTPUT, "output") : LLL( MSG_DISABLED2, "disbld"),
            (memory[PSG_MIXER2] & 0x80) ? LLL( MSG_OUTPUT, "output") : LLL( MSG_DISABLED2, "disbld")
        );
        zprintf
        (   TEXTPEN_VIEW,
            "%-16s A=%6d, B=%6d, C=%6d A=%6d, B=%6d, C=%d %s 15\n", // we would like to say "of 15" for PSG #1 as well but we don't have enough space
            LLL(
                MSG_CHANNELVOLS,
                "Channel volumes:"
            ),
            (int) (memory[PSG_AMPLITUDEA1] & 0x0F),
            (int) (memory[PSG_AMPLITUDEB1] & 0x0F),
            (int) (memory[PSG_AMPLITUDEC1] & 0x0F),
            (int) (memory[PSG_AMPLITUDEA2] & 0x0F),
            (int) (memory[PSG_AMPLITUDEB2] & 0x0F),
            (int) (memory[PSG_AMPLITUDEC2] & 0x0F),
            LLL(
                MSG_OF,
                "of"
            )
        );
        zprintf
        (   TEXTPEN_VIEW,
            "%-16s A=%-6s, B=%-6s, C=%-6s A=%-6s, B=%-6s, C=%-6s\n",
            LLL(
                MSG_AMPLITUDEMODES,
                "Amplitude modes:"
            ),
            (memory[PSG_AMPLITUDEA1] & 0x10) ? LLL( MSG_AMPMODE_ENVELOPE, "envlpe") : LLL( MSG_AMPMODE_NORMAL, "normal"),
            (memory[PSG_AMPLITUDEB1] & 0x10) ? LLL( MSG_AMPMODE_ENVELOPE, "envlpe") : LLL( MSG_AMPMODE_NORMAL, "normal"),
            (memory[PSG_AMPLITUDEC1] & 0x10) ? LLL( MSG_AMPMODE_ENVELOPE, "envlpe") : LLL( MSG_AMPMODE_NORMAL, "normal"),
            (memory[PSG_AMPLITUDEA2] & 0x10) ? LLL( MSG_AMPMODE_ENVELOPE, "envlpe") : LLL( MSG_AMPMODE_NORMAL, "normal"),
            (memory[PSG_AMPLITUDEB2] & 0x10) ? LLL( MSG_AMPMODE_ENVELOPE, "envlpe") : LLL( MSG_AMPMODE_NORMAL, "normal"),
            (memory[PSG_AMPLITUDEC2] & 0x10) ? LLL( MSG_AMPMODE_ENVELOPE, "envlpe") : LLL( MSG_AMPMODE_NORMAL, "normal")
        );
        zprintf
        (   TEXTPEN_VIEW,
            "%s\n %-15s   %4.3f %-17s   %4.3f %s\n",
            LLL(
                MSG_ENVELOPE,
                "Envelope:"
            ),
            LLL(
                MSG_PERIOD,
                "Period:"
            ),
            (memory[PSG_PERIOD1_H] == 0 && memory[PSG_PERIOD1_L] == 0) ? (float) 6927.313 : (float) (6927.313 / ((memory[PSG_PERIOD1_H] * 256) + memory[PSG_PERIOD1_L])),
            LLL( MSG_HERTZ, "Hz"),
            (memory[PSG_PERIOD2_H] == 0 && memory[PSG_PERIOD2_L] == 0) ? (float) 6927.313 : (float) (6927.313 / ((memory[PSG_PERIOD2_H] * 256) + memory[PSG_PERIOD2_L])),
            LLL( MSG_HERTZ, "Hz")
        );
        zprintf
        (   TEXTPEN_VIEW,
            " %-15s %s             %s\n",
            LLL(
                MSG_SHAPE,
                "Shape:"
            ),
            waveshape[memory[PSG_SHAPE1] & 0x0F],
            waveshape[memory[PSG_SHAPE2] & 0x0F]
        );
        zprintf
        (   TEXTPEN_VIEW,
            " %-15s %-28s %s\n",
            LLL(
                MSG_HOLD,
                "Hold:"
            ),
            (memory[PSG_SHAPE1] & 0x01) ? LLL( MSG_ENGINE_ON3, "on") : LLL( MSG_ENGINE_OFF3, "off"),
            (memory[PSG_SHAPE2] & 0x01) ? LLL( MSG_ENGINE_ON3, "on") : LLL( MSG_ENGINE_OFF3, "off")
        );
        zprintf
        (   TEXTPEN_VIEW,
            " %-15s %-28s %s\n",
            LLL(
                MSG_ALTERNATE,
                "Alternate:"
            ),
            (memory[PSG_SHAPE1] & 0x02) ? LLL( MSG_ENGINE_ON3, "on") : LLL( MSG_ENGINE_OFF3, "off"),
            (memory[PSG_SHAPE2] & 0x02) ? LLL( MSG_ENGINE_ON3, "on") : LLL( MSG_ENGINE_OFF3, "off")
        );
        zprintf
        (   TEXTPEN_VIEW,
            " %-15s %-28s %s\n",
            LLL(
                MSG_ATTACK,
                "Attack:"
            ),
            (memory[PSG_SHAPE1] & 0x04) ? LLL( MSG_ENGINE_ON3, "on") : LLL( MSG_ENGINE_OFF3, "off"),
            (memory[PSG_SHAPE2] & 0x04) ? LLL( MSG_ENGINE_ON3, "on") : LLL( MSG_ENGINE_OFF3, "off")
        );
        zprintf
        (   TEXTPEN_VIEW,
            " %-15s %-28s %s\n",
            LLL(
                MSG_CONTINUE,
                "Continue:"
            ),
            (memory[PSG_SHAPE1] & 0x08) ? LLL( MSG_ENGINE_ON3, "on") : LLL( MSG_ENGINE_OFF3, "off"),
            (memory[PSG_SHAPE2] & 0x08) ? LLL( MSG_ENGINE_ON3, "on") : LLL( MSG_ENGINE_OFF3, "off")
        );
    }

    zprintf(TEXTPEN_VIEW, "\n");
}

EXPORT void monob(int y)
{   TRANSIENT TEXT whichletter;
    TRANSIENT int  i, x;

PERSIST const TEXT  elektor_chars2[28 + 1] = "?_!NlTi:.BDHJKQRTUVXYZ!?MMWW";
// SAS/C says "initializer data truncated" if it is only [28].
PERSIST const UBYTE elektor_bmaps[28][6] = {
// from chapter 11
{ 0xE, // ###. ? ($5F)
  0x2, // ..#.
  0x2, // ..#.
  0xE, // ###.
  0x8, // #...
  0x8  // #...
},
{ 0x0, // .... _ ($8F)
  0x0, // ....
  0x0, // ....
  0x0, // ....
  0x0, // ....
  0xA  // #.#.
},
{ 0x4, // .#.. ! ($A2)
  0x4, // .#..
  0x0, // ....
  0x4, // .#..
  0x0, // ....
  0x0  // ....
},
{ 0x0, // .... N ($AA)
  0xE, // ###.
  0xA, // #.#.
  0xA, // #.#.
  0xA, // #.#.
  0xA  // #.#.
},
{ 0x8, // #... l ($BB)
  0x8, // #...
  0x8, // #...
  0x8, // #...
  0x8, // #...
  0x8  // #...
},
{ 0xE, // ###. T ($BC)
  0xE, // ###.
  0x4, // .#..
  0x4, // .#..
  0x4, // .#..
  0x4  // .#..
},
{ 0x4, // .#.. i ($DF)
  0x0, // ....
  0xC, // ##..
  0x4, // .#..
  0x4, // .#..
  0x4  // .#..
},
{ 0xC, // ##.. : ($E6)
  0xC, // ##..
  0x0, // ....
  0x0, // ....
  0xC, // ##..
  0xC  // ##..
},
{ 0x0, // .... . ($F7)
  0x0, // ....
  0x0, // ....
  0x0, // ....
  0x0, // ....
  0x4  // .#..
},
// from chapter 17
{ 0xE, // ###. B
  0xA, // #.#.
  0xC, // ##..
  0xA, // #.#.
  0xA, // #.#.
  0xE  // ###.
},
{ 0xC, // ##.. D
  0xA, // #.#.
  0xA, // #.#.
  0xA, // #.#.
  0xA, // #.#.
  0xC  // ##..
},
{ 0xA, // #.#. H
  0xA, // #.#.
  0xE, // ###.
  0xA, // #.#.
  0xA, // #.#.
  0xA  // #.#.
},
{ 0x2, // ..#. J
  0x2, // ..#.
  0x2, // ..#.
  0x2, // ..#.
  0x2, // ..#.
  0xE  // ###.
},
{ 0xA, // #.#. K
  0xA, // #.#.
  0xC, // ##..
  0xA, // #.#.
  0xA, // #.#.
  0xA  // #.#.
},
{ 0xE, // ###. Q
  0xA, // #.#.
  0xA, // #.#.
  0xA, // #.#.
  0xA, // #.#.
  0xF  // ####
},
{ 0xE, // ###. R
  0xA, // #.#.
  0xC, // ##..
  0xA, // #.#.
  0xA, // #.#.
  0xA  // #.#.
},
{ 0xE, // ###. T
  0x4, // .#..
  0x4, // .#..
  0x4, // .#..
  0x4, // .#..
  0x4  // .#..
},
{ 0xA, // #.#. U
  0xA, // #.#.
  0xA, // #.#.
  0xA, // #.#.
  0xA, // #.#.
  0xE  // ###.
},
{ 0xA, // #.#. V
  0xA, // #.#.
  0xA, // #.#.
  0xA, // #.#.
  0xA, // #.#.
  0x4  // .#..
},
{ 0xA, // #.#. X
  0xA, // #.#.
  0x4, // .#..
  0xA, // #.#.
  0xA, // #.#.
  0xA  // #.#.
},
{ 0xA, // #.#. Y
  0xA, // #.#.
  0xA, // #.#.
  0x4, // .#..
  0x4, // .#..
  0x4  // .#..
},
{ 0xE, // ###. Z
  0x2, // ..#.
  0x4, // .#..
  0x8, // #...
  0x8, // #...
  0xE  // ###.
},
{ 0x4, // .#.. !
  0x4, // .#..
  0x4, // .#..
  0x4, // .#..
  0x0, // ....
  0x4  // .#..
},
{ 0xE, // ###. ?
  0x2, // ..#.
  0xE, // ###.
  0x8, // #...
  0x0, // ....
  0x8  // #...
},
{ 0x6, // .##. M (left  half)
  0x4, // .#.#
  0x4, // .#..
  0x4, // .#..
  0x4, // .#..
  0x4  // .#..
},
{ 0xC, // ##.. M (right half)
  0x4, // .#..
  0x4, // .#..
  0x4, // .#..
  0x4, // .#..
  0x4  // .#..
},
{ 0x4, // .#.. W (left  half)
  0x4, // .#..
  0x4, // .#..
  0x4, // .#..
  0x5, // .#.#
  0x2  // ..#.
},
{ 0x4, // .#.. W (right half)
  0x4, // .#..
  0x4, // .#..
  0x4, // .#..
  0x4, // .#..
  0x8  // #...
} };

    // assert(machine == ELEKTOR);

    for (x = 0; x < 4; x++) // 4 sprites per row
    {   whichletter = '#';
        for (i = 0; i < 256; i += 2)
        {   // check left letter against left ROM letters
            if
            (   ((memory[0x800 + (y * 24) + (x * 6)]) & 0xE0)       == ((memory[0x27B + ((i / 2) * 6)]) & 0xE0)
             && ((memory[0x801 + (y * 24) + (x * 6)]) & 0xE0)       == ((memory[0x27C + ((i / 2) * 6)]) & 0xE0)
             && ((memory[0x802 + (y * 24) + (x * 6)]) & 0xE0)       == ((memory[0x27D + ((i / 2) * 6)]) & 0xE0)
             && ((memory[0x803 + (y * 24) + (x * 6)]) & 0xE0)       == ((memory[0x27E + ((i / 2) * 6)]) & 0xE0)
             && ((memory[0x804 + (y * 24) + (x * 6)]) & 0xE0)       == ((memory[0x27F + ((i / 2) * 6)]) & 0xE0)
             && ((memory[0x805 + (y * 24) + (x * 6)]) & 0xE0)       == ((memory[0x280 + ((i / 2) * 6)]) & 0xE0)
            )
            {   whichletter = elektor_chars1[i];
                break; // for speed
            }

            // check left letter against right ROM letters
            if
            (   ((memory[0x800 + (y * 24) + (x * 6)]) & 0xE0) >> 4  == ((memory[0x27B + ((i / 2) * 6)]) & 0x0E)
             && ((memory[0x801 + (y * 24) + (x * 6)]) & 0xE0) >> 4  == ((memory[0x27C + ((i / 2) * 6)]) & 0x0E)
             && ((memory[0x802 + (y * 24) + (x * 6)]) & 0xE0) >> 4  == ((memory[0x27D + ((i / 2) * 6)]) & 0x0E)
             && ((memory[0x803 + (y * 24) + (x * 6)]) & 0xE0) >> 4  == ((memory[0x27E + ((i / 2) * 6)]) & 0x0E)
             && ((memory[0x804 + (y * 24) + (x * 6)]) & 0xE0) >> 4  == ((memory[0x27F + ((i / 2) * 6)]) & 0x0E)
             && ((memory[0x805 + (y * 24) + (x * 6)]) & 0xE0) >> 4  == ((memory[0x280 + ((i / 2) * 6)]) & 0x0E)
            )
            {   whichletter = elektor_chars1[i + 1];
                break; // for speed
        }   }
        if (whichletter == '#')
        {   // check left letter against accidentals and modifieds
            for (i = 0; i < 28; i++)
            {   if
                (   ((memory[0x800 + (y * 24) + (x * 6)]) & 0xE0) >> 4 == elektor_bmaps[i][0]
                 && ((memory[0x801 + (y * 24) + (x * 6)]) & 0xE0) >> 4 == elektor_bmaps[i][1]
                 && ((memory[0x802 + (y * 24) + (x * 6)]) & 0xE0) >> 4 == elektor_bmaps[i][2]
                 && ((memory[0x803 + (y * 24) + (x * 6)]) & 0xE0) >> 4 == elektor_bmaps[i][3]
                 && ((memory[0x804 + (y * 24) + (x * 6)]) & 0xE0) >> 4 == elektor_bmaps[i][4]
                 && ((memory[0x805 + (y * 24) + (x * 6)]) & 0xE0) >> 4 == elektor_bmaps[i][5]
                )
                {   whichletter = elektor_chars2[i];
                    break; // for speed
        }   }   }

        letters[x * 2] = whichletter;

        whichletter = '#';
        for (i = 0; i < 256; i += 2)
        {   // check right letter against left ROM letters
            if
            (   ((memory[0x800 + (y * 24) + (x * 6)]) & 0x0E)       == ((memory[0x27B + ((i / 2) * 6)]) & 0xE0) >> 4
             && ((memory[0x801 + (y * 24) + (x * 6)]) & 0x0E)       == ((memory[0x27C + ((i / 2) * 6)]) & 0xE0) >> 4
             && ((memory[0x802 + (y * 24) + (x * 6)]) & 0x0E)       == ((memory[0x27D + ((i / 2) * 6)]) & 0xE0) >> 4
             && ((memory[0x803 + (y * 24) + (x * 6)]) & 0x0E)       == ((memory[0x27E + ((i / 2) * 6)]) & 0xE0) >> 4
             && ((memory[0x804 + (y * 24) + (x * 6)]) & 0x0E)       == ((memory[0x27F + ((i / 2) * 6)]) & 0xE0) >> 4
             && ((memory[0x805 + (y * 24) + (x * 6)]) & 0x0E)       == ((memory[0x280 + ((i / 2) * 6)]) & 0xE0) >> 4
            )
            {   whichletter = elektor_chars1[i];
                break;
            }

            // check right letter against right ROM letters
            if
            (   ((memory[0x800 + (y * 24) + (x * 6)]) & 0x0E)       == ((memory[0x27B + ((i / 2) * 6)]) & 0x0E)
             && ((memory[0x801 + (y * 24) + (x * 6)]) & 0x0E)       == ((memory[0x27C + ((i / 2) * 6)]) & 0x0E)
             && ((memory[0x802 + (y * 24) + (x * 6)]) & 0x0E)       == ((memory[0x27D + ((i / 2) * 6)]) & 0x0E)
             && ((memory[0x803 + (y * 24) + (x * 6)]) & 0x0E)       == ((memory[0x27E + ((i / 2) * 6)]) & 0x0E)
             && ((memory[0x804 + (y * 24) + (x * 6)]) & 0x0E)       == ((memory[0x27F + ((i / 2) * 6)]) & 0x0E)
             && ((memory[0x805 + (y * 24) + (x * 6)]) & 0x0E)       == ((memory[0x280 + ((i / 2) * 6)]) & 0x0E)
            )
            {   whichletter = elektor_chars1[i + 1];
                break;
        }   }
        if (whichletter == '#')
        {   // check right letter against accidentals and modifieds
            for (i = 0; i < 28; i++)
            {   if
                (   ((memory[0x800 + (y * 24) + (x * 6)]) & 0x0E)   == elektor_bmaps[i][0]
                 && ((memory[0x801 + (y * 24) + (x * 6)]) & 0x0E)   == elektor_bmaps[i][1]
                 && ((memory[0x802 + (y * 24) + (x * 6)]) & 0x0E)   == elektor_bmaps[i][2]
                 && ((memory[0x803 + (y * 24) + (x * 6)]) & 0x0E)   == elektor_bmaps[i][3]
                 && ((memory[0x804 + (y * 24) + (x * 6)]) & 0x0E)   == elektor_bmaps[i][4]
                 && ((memory[0x805 + (y * 24) + (x * 6)]) & 0x0E)   == elektor_bmaps[i][5]
                )
                {   whichletter = elektor_chars2[i];
                    break; // for speed
        }   }   }

        letters[(x * 2) + 1] = whichletter;
    }

    letters[8] = 0;
}

EXPORT void elektor_setmemmap(void)
{   TRANSIENT       int    i, address;
    PERSIST   const UWORD  mirrors_input[64] = {
E_LINEIN0,  E_LINEIN1,  E_LINEIN2,  E_LINEIN3,  // $1D80..$1D83
E_LINEIN0,  E_LINEIN1,  E_LINEIN2,  E_LINEIN3,  // $1D84..$1D87
E_LINEIN0,  E_LINEIN1,  E_LINEIN2,  E_LINEIN3,  // $1D88..$1D8B
E_LINEIN0,  E_LINEIN1,  E_LINEIN2,  E_LINEIN3,  // $1D8C..$1D8F
E_LINEIN0,  E_LINEIN1,  E_LINEIN2,  E_LINEIN3,  // $1D90..$1D93
E_LINEIN0,  E_LINEIN1,  E_LINEIN2,  E_LINEIN3,  // $1D94..$1D97
E_LINEIN0,  E_LINEIN1,  E_LINEIN2,  E_LINEIN3,  // $1D98..$1D9B
E_LINEIN0,  E_LINEIN1,  E_LINEIN2,  E_LINEIN3,  // $1D9C..$1D9F
E_LINEIN4,  E_LINEIN5,  E_LINEIN6,  E_LINEIN7,  // $1DA0..$1DA3
E_LINEIN4,  E_LINEIN5,  E_LINEIN6,  E_LINEIN7,  // $1DA4..$1DA7
E_LINEIN4,  E_LINEIN5,  E_LINEIN6,  E_LINEIN7,  // $1DA8..$1DAB
E_LINEIN4,  E_LINEIN5,  E_LINEIN6,  E_LINEIN7,  // $1DAC..$1DAF
E_LINEIN4,  E_LINEIN5,  E_LINEIN6,  E_LINEIN7,  // $1DB0..$1DB3
E_LINEIN4,  E_LINEIN5,  E_LINEIN6,  E_LINEIN7,  // $1DB4..$1DB7
E_LINEIN4,  E_LINEIN5,  E_LINEIN6,  E_LINEIN7,  // $1DB8..$1DBB
E_LINEIN4,  E_LINEIN5,  E_LINEIN6,  E_LINEIN7   // $1DBC..$1DBF
}, mirrors_output[64] = {
E_LINEOUT0, E_LINEOUT1, E_LINEOUT2, E_LINEOUT3, // $1DC0..$1DC3
E_LINEOUT0, E_LINEOUT1, E_LINEOUT2, E_LINEOUT3, // $1DC4..$1DC7
E_LINEOUT0, E_LINEOUT1, E_LINEOUT2, E_LINEOUT3, // $1DC8..$1DCB
E_LINEOUT0, E_LINEOUT1, E_LINEOUT2, E_LINEOUT3, // $1DCC..$1DCF
E_LINEOUT0, E_LINEOUT1, E_LINEOUT2, E_LINEOUT3, // $1DD0..$1DD3
E_LINEOUT0, E_LINEOUT1, E_LINEOUT2, E_LINEOUT3, // $1DD4..$1DD7
E_LINEOUT0, E_LINEOUT1, E_LINEOUT2, E_LINEOUT3, // $1DD8..$1DDB
E_LINEOUT0, E_LINEOUT1, E_LINEOUT2, E_LINEOUT3, // $1DDC..$1DDF
E_LINEOUT4, E_LINEOUT5, E_LINEOUT6, E_LINEOUT7, // $1DE0..$1DE3
E_LINEOUT4, E_LINEOUT5, E_LINEOUT6, E_LINEOUT7, // $1DE4..$1DE7
E_LINEOUT4, E_LINEOUT5, E_LINEOUT6, E_LINEOUT7, // $1DE8..$1DEB
E_LINEOUT4, E_LINEOUT5, E_LINEOUT6, E_LINEOUT7, // $1DEC..$1DEF
E_LINEOUT4, E_LINEOUT5, E_LINEOUT6, E_LINEOUT7, // $1DF0..$1DF3
E_LINEOUT4, E_LINEOUT5, E_LINEOUT6, E_LINEOUT7, // $1DF4..$1DF7
E_LINEOUT4, E_LINEOUT5, E_LINEOUT6, E_LINEOUT7, // $1DF8..$1DFB
E_LINEOUT4, E_LINEOUT5, E_LINEOUT6, E_LINEOUT7  // $1DFC..$1DFF
};

    // assert(!crippled);
    // assert(!inframe);
    game = FALSE;
    bangercharging = FALSE;

    pvibase = 0x1F00;

    if (elektor_biosver == ELEKTOR_PHILIPS)
    {   for (i = 0; i <= 0x7FF; i++)
        {   memory[i] = e_bios_philips[i];
        }

        machines[ELEKTOR].firstequiv       = FIRSTPHILIPSEQUIV;
        machines[ELEKTOR].lastequiv        = LASTPHILIPSEQUIV;
        machines[ELEKTOR].firstcodecomment = FIRSTPHILIPSCODECOMMENT;
        machines[ELEKTOR].lastcodecomment  = LASTPHILIPSCODECOMMENT;
        machines[ELEKTOR].firstdatacomment = FIRSTPHILIPSDATACOMMENT;
        machines[ELEKTOR].lastdatacomment  = LASTPHILIPSDATACOMMENT;
    } else
    {   // assert(elektor_biosver == ELEKTOR_HOBBYMODULE);
        for (i = 0; i <= 0x7FF; i++)
        {   memory[i] = e_bios_hobbymodule[i];
        }

        machines[ELEKTOR].firstequiv       = FIRSTHOBBYMODULEEQUIV;
        machines[ELEKTOR].lastequiv        = LASTHOBBYMODULEEQUIV;
        machines[ELEKTOR].firstcodecomment = FIRSTHOBBYMODULECODECOMMENT;
        machines[ELEKTOR].lastcodecomment  = LASTHOBBYMODULECODECOMMENT;
        machines[ELEKTOR].firstdatacomment = FIRSTHOBBYMODULEDATACOMMENT;
        machines[ELEKTOR].lastdatacomment  = LASTHOBBYMODULEDATACOMMENT;
    }

    for (i = 0x800; i <= 0xFFF; i++)
    {   memory[i] = 0;
    }
    if (memmap == MEMMAP_E)
    {   for (i = 0x1000; i <= 0x15FF; i++)
        {   memory[i] = 0xFF; // unimportant, illegal reads don't actually look at the stored value
            /* "If extended memory is not available, the value
                retrieved from address $1000 will be $FF." - "TV Games
                Computer" book, p. 185. */
        }
        for (i = 0x2000; i <= 0x7FFF; i++)
        {   memory[i] = 0; // not necessarily correct
    }   }
    else
    {   // assert(memmap == MEMMAP_F);
        for (i = 0x1000; i <= 0x15FF; i++)
        {   memory[i] = 0;
    }   }

    for (i = 0x1600; i <= 0x1FFF; i++)
    {   memory[i] = 0;
    }
    memory[pvibase + PVI_P1PADDLE] = // important for Helicopter
    memory[pvibase + PVI_P2PADDLE] = machines[ELEKTOR].digipos[1];

    if (memmap == MEMMAP_F)
    {   for (i = 0; i < 512; i++)
        {   memory[0x2000 + i] = e_calculator[i];
        }
        for (i = 0x2200; i <= 0x7FFF; i++)
        {   memory[i] = 0xFF; // to simulate "blank" EPROM
    }   }

    for (address = 0; address < 32 * KILOBYTE; address++)
    {   memflags[address] = 0;
        mirror_r[address] =
        mirror_w[address] = (UWORD) address;

        if (memmap == MEMMAP_E)
        {   if
            (
        // legal:               $0..               $FFF     ROM, monitor RAM, user RAM
                (address >= 0x1000 && address <= 0x1DBE)
        // legal:            $1DBF                          CASIN
             || (address >= 0x1DC0 && address <= 0x1E87)
        // legal:            $1E88..              $1E8E     R/- hardware registers
             || (address >= 0x1E8F && address <= 0x1EFF) // unmapped, mirrors
        // legal:            $1F00..              $1F2D     R/W hardware registers, user RAM
             || (address >= 0x1F2E && address <= 0x1F3F) // unmapped
        // legal:            $1F40..              $1F6D     R/W hardware registers, user RAM
             || (address >= 0x1F6E && address <= 0x1F7F) // unmapped
        // legal:            $1F80..              $1FAD     R/W hardware registers, user RAM
             || (address >= 0x1FAE && address <= 0x1FC9) // unmapped, -/W hardware registers
        // legal:            $1FCA              ..$1FCD     R/- hardware registers
             || (address >= 0x1FCE && address <= 0x1FD9) // unused, mirrors (not occurring)
        // legal:            $1FDA              ..$1FDB     semi-mirrors
             || (address >= 0x1FDC && address <= 0x1FE9) // mirrors (not occurring)
        // legal:            $1FEA              ..$1FEB     semi-mirrors
             || (address >= 0x1FEC && address <= 0x1FF9) // mirrors (not occurring)
        // legal:            $1FFA              ..$1FFB     semi-mirrors
             ||  address >= 0x1FFC                       // mirrors (not occurring)
            )
            {   memflags[address] |= NOREAD;
            }

            if
            (    address <=  0x7FF                       // ROM
        // legal:             $800..               $FFF     monitor RAM, user RAM
             || (address >= 0x1000 && address <= 0x1DFE) // unused, mirrors, I/O, input and output lines (and mirrors thereof), unmapped, R/- hardware regs, unmapped, mirrors
        // legal:            $1DFF                       // CASOUT
             || (address >= 0x1E00 && address <= 0x1E7F)
        // legal:            $1E80                       // NOISE
             || (address >= 0x1E81 && address <= 0x1EFF)
        // legal:            $1F00..              $1F2D     R/W hardware regs, user RAM
             || (address >= 0x1F2E && address <= 0x1F3F) // unmapped
        // legal:            $1F40..              $1F6D     R/W hardware regs, user RAM
             || (address >= 0x1F6E && address <= 0x1F7F) // unmapped
        // legal:            $1F80..              $1FAD     R/W hardware regs, user RAM
             || (address >= 0x1FAE && address <= 0x1FBF) // unmapped
        // legal:            $1FC0..              $1FC3     -/W hardware regs
             || (address >= 0x1FC4 && address <= 0x1FC5) // unused
        // legal:            $1FC6..              $1FC9     -/W hardware regs
             ||  address >= 0x1FCA                       // R/- hardware registers, unused, semi-mirrors, unused
            )
            {   memflags[address] |= NOWRITE;
        }   }
        else
        {   // assert(memmap == MEMMAP_F);

            if
            (
        // legal:               $0..              $1BFF     ROM, monitor RAM, user RAM
        // legal:            $1C00..              $1C7F     RLG load/save routine
                (address >= 0x1C80 && address <= 0x1CFF) // mirrors, PSGs (-/W)
        // legal:            $1D00..              $1D20  // PSGs, 2nd German randomizer
             || (address >= 0x1D21 && address <= 0x1DBE) // I/O, input and output lines (and mirrors thereof)
        // legal:            $1DBF                          CASIN
             || (address >= 0x1DC0 && address <= 0x1DFF) // I/O, input and output lines (and mirrors thereof)
        // legal:            $1DFF..              $1E7F     RLG load/save routine
             || (address >= 0x1E80 && address <= 0x1E87) // noise register (assuming -/W), unmapped
        // legal:            $1E88..              $1E8E     R/- hardware registers
             || (address >= 0x1E8F && address <= 0x1EFF) // unmapped, mirrors
        // legal:            $1F00..              $1F2D     R/W hardware registers, user RAM
             || (address >= 0x1F2E && address <= 0x1F3F) // unmapped
        // legal:            $1F40..              $1F6D     R/W hardware registers, user RAM
             || (address >= 0x1F6E && address <= 0x1F7F) // unmapped
        // legal:            $1F80..              $1FAD     R/W hardware registers, user RAM
             ||  address == 0x1FAE                       // unmapped
        // legal:            $1FAF                          1st German randomizer
             || (address >= 0x1FB0 && address <= 0x1FC9) // unmapped, -/W hardware registers
        // legal:            $1FCA              ..$1FCD     R/- hardware registers
             || (address >= 0x1FCE && address <= 0x1FD9) // unused, mirrors (not occurring)
        // legal:            $1FDA              ..$1FDB     semi-mirrors
             || (address >= 0x1FDC && address <= 0x1FE9) // mirrors (not occurring)
        // legal:            $1FEA              ..$1FEB     semi-mirrors
             || (address >= 0x1FEC && address <= 0x1FF9) // mirrors (not occurring)
        // legal:            $1FFA              ..$1FFB     semi-mirrors
             || (address >= 0x1FFC && address <= 0x1FFE) // mirrors (not occurring)
        // legal:            $1FFF                          1st English randomizer
        // legal:            $2000..$7FFF                   RLG games storage area
            )
            {   memflags[address] |= NOREAD;
            }

            if
            (    address <=  0x7FF                       // ROM
        // legal:             $800..              $1BFF     monitor RAM, user RAM
             || (address >= 0x1C00 && address <= 0x1CFF) // EPROM (RLG load/save routine), mirrors
        // legal:            $1D00..              $1D0E     PSG #0
             ||  address == 0x1D0F                       // PORTB1
        // legal:            $1D10..              $1D1D     PSG #1
             || (address >= 0x1D1E && address <= 0x1DFE) // PORTA2, PORTB2, randomizer #2, I/O, input and output lines (and mirrors thereof)
        // legal:            $1DFF                          CASOUT
             || (address >= 0x1E00 && address <= 0x1E7F) // EPROM (RLG load/save routine)
        // legal:            $1E80                          noise register
             || (address >= 0x1E81 && address <= 0x1EFF) // unmapped, R/- hardware regs, unmapped, mirrors
        // legal:            $1F00..              $1F2D     R/W hardware regs, user RAM
             || (address >= 0x1F2E && address <= 0x1F3F) // unmapped
        // legal:            $1F40..              $1F6D     R/W hardware regs, user RAM
             || (address >= 0x1F6E && address <= 0x1F7F) // unmapped
        // legal:            $1F80..              $1FAD     R/W hardware regs, user RAM
             || (address >= 0x1FAE && address <= 0x1FBF) // unmapped
        // legal:            $1FC0..              $1FC3     -/W hardware regs
             || (address >= 0x1FC4 && address <= 0x1FC5) // unused
        // legal:            $1FC6..              $1FC9     -/W hardware regs
             || (address >= 0x1FCA && address <= 0x7FFF) // R/- hardware registers, unused, randomizer, semi-mirrors, EPROM (games storage area)
            )
            {   memflags[address] |= NOWRITE;
            }

            /* Writes to PORTB1, PORTA2, PORTB2 are considered illegal,
               even though these areas are write-only. This is because
               no conceivable game would write there. */

            if
            (   address == E_RANDOM1E
             || address == E_RANDOM1G
             || address == E_RANDOM2
            )
            {   memflags[address] |= BANKED;
        }   }

        if
        (   (address >= 0x1FCA && address <= 0x1FCB)
         || (address >= 0x1FDA && address <= 0x1FDB)
         || (address >= 0x1FEA && address <= 0x1FEB)
         || (address >= 0x1FFA && address <= 0x1FFB)
        )
        {   memflags[address] |= READONCE;
        }

        if
        (   address == 0x1F00 + PVI_PITCH
         || address == IE_NOISE
         || (memmap == MEMMAP_F && address >= 0x1D00 && address <= 0x1D1F) // not really every byte in this range
        )
        {   memflags[address] |= AUDIBLE;
        }

// Mirroring--------------------------------------------------------------

        if (memmap == MEMMAP_E)
        {   // primary mirroring, must go first
            if (mirror_r[address] >= 0x2000)
            {   mirror_r[address] &= 0x1FFF;
            }
            if (mirror_w[address] >= 0x2000)
            {   mirror_w[address] &= 0x1FFF;
            }

            // secondary mirroring
            if (mirror_r[address] >= 0x1600 && mirror_r[address] <= 0x17FF) // mirror of $1E00..$1FFF
            {   mirror_r[address] += 0x800;
            } elif (mirror_r[address] >= 0x1800 && mirror_r[address] <= 0x1CFF)
            {   // $18xx -> $1Dxx
                // $19xx -> $1Dxx
                // $1Axx -> $1Dxx
                // $1Bxx -> $1Dxx
                // $1Cxx -> $1Dxx
                mirror_r[address] &= 0xF8FF; // %1111100011111111
                mirror_r[address] |= 0x0500; // %0000010111111111
            }
            if (mirror_w[address] >= 0x1600 && mirror_w[address] <= 0x17FF) // mirror of $1E00..$1FFF
            {   mirror_w[address] += 0x800;
            } elif (mirror_w[address] >= 0x1800 && mirror_w[address] <= 0x1CFF)
            {   // $18xx -> $1Dxx
                // $19xx -> $1Dxx
                // $1Axx -> $1Dxx
                // $1Bxx -> $1Dxx
                // $1Cxx -> $1Dxx
                mirror_w[address] &= 0xF8FF; // %1111100011111111
                mirror_w[address] |= 0x0500; // %0000010111111111
        }   }

        // tertiary mirroring

        // read mirrors of $1F00..$1F09 and $1F0C..$1F0F.
        // $1F0A..$1F0B are semi-mirrored.
        if
        (   (mirror_r[address] >= 0x1FD0 && mirror_r[address] <= 0x1FD9)
         || (mirror_r[address] >= 0x1FDC && mirror_r[address] <= 0x1FE9)
         || (mirror_r[address] >= 0x1FEC && mirror_r[address] <= 0x1FF9)
         || (mirror_r[address] >= 0x1FFC && mirror_r[address] <= 0x1FFE) // $1FFF is a randomizer
        )
        {   mirror_r[address] &= 0xFFCF; // apply mask of %1111111111001111
        }

        if (mirror_r[address] >= 0x1C80 && mirror_r[address] <= 0x1CFF) // mirror of $1D80..$1DFF
        {   mirror_r[address] += 0x100;
        }

        // write mirrors of $1F00..$1F09 and $1F0C..$1F0F.
        // $1F0A..$1F0B are semi-mirrored.
        if
        (   (mirror_w[address] >= 0x1FD0 && mirror_w[address] <= 0x1FD9)
         || (mirror_w[address] >= 0x1FDC && mirror_w[address] <= 0x1FE9)
         || (mirror_w[address] >= 0x1FEC && mirror_w[address] <= 0x1FF9)
         || (mirror_w[address] >= 0x1FFC && mirror_w[address] <= 0x1FFE) // $1FFF is a randomizer
        )
        {   mirror_w[address] &= 0xFFCF; // apply mask of %1111111111001111
        }

        if (mirror_w[address] >= 0x1C80 && mirror_w[address] <= 0x1CFF) // mirror of $1D80..$1DFF
        {   mirror_w[address] += 0x100;
        }

        ie_inputmirrors(address);

        if (mirror_r[address] >= 0x1D80 && mirror_r[address] <= 0x1DBF)
        {   mirror_r[address] = mirrors_input[mirror_r[address] - 0x1D80];
        }
        if (mirror_w[address] >= 0x1DC0 && mirror_r[address] <= 0x1DFF)
        {   mirror_w[address] = mirrors_output[mirror_w[address] - 0x1DC0];
    }   }

/* Unresolved issues:
 how should writes to $1FFF be handled?
  a) mirror them: they will resolve to $1FCF; or...
  b) handle them as (illegal) writes to randomizer #1
  we're handling them as (b) currently.
 we're assuming $1D20 is mirrored at $1C20, but it might not be.
*/

    if (memory[0x73E] == 0x19) // buggy version of monitor
    {   mirror_r[0x199C] = mirror_w[0x199C] = 0x1D9C;
        mirror_r[0x199D] = mirror_w[0x199D] = 0x1D9D;
        mirror_r[0x199E] = mirror_w[0x199E] = 0x1D9E;
        mirror_r[0x199F] = mirror_w[0x199F] = 0x1D9F;
        mirror_r[0x19BC] = mirror_w[0x19BC] = 0x1DBC;
        mirror_r[0x19BD] = mirror_w[0x19BD] = 0x1DBD;
        mirror_r[0x19BE] = mirror_w[0x19BE] = 0x1DBE;
        mirror_r[0x19BF] = mirror_w[0x19BF] = 0x1DBF;
    } // elif (memory[0x73E] != 0x1D) assert(0); // not fixed version either, so panic!

    for (i = machines[machine].firstcodecomment; i <= machines[machine].lastcodecomment; i++)
    {   memflags[codecomment[i].address] |= COMMENTED;
    }
    for (i = 0; i <= 0x7FF; i++)
    {   memflags[i] |= BIOS;
    }

    memflags[IE_NOISE       ] |= ASIC;
    memflags[IE_P1LEFTKEYS  ] |= ASIC;
    memflags[IE_P1MIDDLEKEYS] |= ASIC;
    memflags[IE_P1RIGHTKEYS ] |= ASIC;
    memflags[IE_CONSOLE     ] |= ASIC;
    memflags[IE_P2LEFTKEYS  ] |= ASIC;
    memflags[IE_P2MIDDLEKEYS] |= ASIC;
    memflags[IE_P2RIGHTKEYS ] |= ASIC;

#ifdef WATCHENVELOPES
    if (memmap == MEMMAP_F)
    {   memflags[PSG_AMPLITUDEA1]        |= WATCHPOINT;
        memflags[PSG_AMPLITUDEB1]        |= WATCHPOINT;
        memflags[PSG_AMPLITUDEC1]        |= WATCHPOINT;
        memflags[PSG_AMPLITUDEA2]        |= WATCHPOINT;
        memflags[PSG_AMPLITUDEB2]        |= WATCHPOINT;
        memflags[PSG_AMPLITUDEC2]        |= WATCHPOINT;
        wp[      PSG_AMPLITUDEA1].the2nd =
        wp[      PSG_AMPLITUDEB1].the2nd =
        wp[      PSG_AMPLITUDEC1].the2nd =
        wp[      PSG_AMPLITUDEA2].the2nd =
        wp[      PSG_AMPLITUDEB2].the2nd =
        wp[      PSG_AMPLITUDEC2].the2nd =  COND_MASK;
        wp[      PSG_AMPLITUDEA1].the3rd =
        wp[      PSG_AMPLITUDEB1].the3rd =
        wp[      PSG_AMPLITUDEC1].the3rd =
        wp[      PSG_AMPLITUDEA2].the3rd =
        wp[      PSG_AMPLITUDEB2].the3rd =
        wp[      PSG_AMPLITUDEC2].the3rd =  0x10;
        watchwrites                      =  WATCH_ALL;
    }
#endif

    nextinst = 0;
    set_cpl(226); // when using 227, bottom left pair of BIOS digits are not always visible

    set_retuning();
}

EXPORT void elektor_biosdetails(int ea)
{   FLAG note = FALSE;

    // assert(machine == ELEKTOR);

if (elektor_biosver == ELEKTOR_HOBBYMODULE)
{   switch (ea)
    {
    case 0x57: // OUTPUT1
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Display of character images on screen (monitor settings)\n" \
            ";Input:          Character images in monitor scratch-pad ($800..$88F)\n" \
            ";Comments:       Call during vertical reset interrupt routine\n" \
            ";Registers used: R0..R2 bank 0\n" \
            ";Return code:    None\n" \
            ";Nesting depth:  1\n"
        );
    acase 0x6F: // OUTPUT2
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Display of character images on screen (user-defined)\n" \
            ";Input:          Character images in monitor scratch-pad ($800..$88F)\n" \
            ";Comments:       Call during vertical reset interrupt routine\n" \
            ";Registers used: R0..R2 bank 0\n" \
            ";Return code:    None\n" \
            ";Nesting depth:  1\n"
        );
    acase 0x14E: // KEYSCN
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Keyboard scan routine\n" \
            ";Input:          MSB set to 0 in MKBST ($89F) or RKBST ($89E) or LKBST ($89D)\n" \
            ";Comments:       Call during vertical reset interrupt routine\n" \
            ";Registers used: R0..R3 bank 0\n" \
            ";Return code:    MKBST ($89F): joined keyboard\n" \
            ";                LKBST ($89D): left keyboard\n" \
            ";                RKBST ($89E): right keyboard\n" \
            ";Nesting depth:  2\n"
        );
    acase 0x1D0: // CONVRT
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Line to image conversion routine\n" \
            ";Input:          $890..$897: eight character codes\n" \
            ";Comments:       Output is character images in monitor\n" \
            ";                scratch displayable via subroutine OUTPUT\n" \
            ";Registers used: R0..R3 bank 0\n" \
            ";Return code:    None" \
            ";Nesting depth:  2\n"
        );
    acase 0x575: // CLRSCR
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Clear screen routine\n" \
            ";Input:          None\n" \
            ";Comments:       PVI objects cleared\n" \
            ";Registers used: R0, R2 bank 0\n" \
            ";Return code:    None" \
            ";Nesting depth:  1\n"
        );
    acase 0x28A: // SCROLL6
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Scroll up of character images one line position (6 lines)\n" \
            ";Input:          None\n" \
            ";Registers used: R0..R2 bank 0\n" \
            ";Return code:    None" \
            ";Nesting depth:  1\n"
        );
    acase 0x28C: // SCROLL15
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Scroll up of character images one line position (1..5 lines)\n" \
            ";Input:          R1 = $00/$E8/$D0/$B8/$A0 for 1..5 lines respectively\n" \
            ";Registers used: R0..R2 bank 0\n" \
            ";Return code:    None" \
            ";Nesting depth:  1\n"
        );
    }
} else
{   switch (ea)
    {
    case 0x3:
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    Interrupt address $0003\n"
        );
    acase 0xA:
        zprintf
        (   TEXTPEN_LOG,
            ";Autodoc:\n" \
"; At reset the system enters the monitor. Initiation is done after power\n" \
"; up or if reset + initiate key.\n"
        );
    acase 0x4B:
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    Entry for monitor interrupts via RAM address\n"
        );
    acase 0x4E:
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    Monitor screen refresh\n"
        );
    acase 0x55: // DISPLAY6LINES
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Display six lines\n" \
            ";Initial data:   set COM bit\n" \
            ";Comments:       wait for VRLE\n" \
            ";Registers used: R0..R2\n" \
            ";Input register: None\n" \
            ";Return code:    None\n"
        );
    acase 0x7A: // FREEFR2
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Display last line only\n" \
            ";Initial data:   set COM bit, VC objects, R1=$78, R2=$00\n" \
            ";Registers used: R0..R2\n"
        );
    acase 0x9E: // CLEARDUPS
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Clear duplicates\n" \
            ";Comments:       VOD 1..4 is $FE\n" \
            ";Registers used: R0\n" \
            ";Input register: None\n" \
            ";Return code:    None\n"
        );
    acase 0xA0: // CLEARVODDUPS
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Load VOD duplicates\n" \
            ";Initial data:   data in R0\n" \
            ";Registers used: R0\n" \
            ";Input register: R0\n" \
            ";Return code:    None\n"
        );
    acase 0xBD:
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    Vertical reset with key entry\n"
        );
    acase 0x13E:
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    Status save routine\n"
        );
    acase 0x161: // SAVE1
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Initiate PVI\n" \
            ";Registers used: R0..R2\n" \
            ";Input register: None\n" \
            ";Return code:    None\n"
        );
    acase 0x16E: // CLROBJ
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Clear PVI objects\n" \
            ";Comments:       $1F00..$1F4D is $00\n" \
            ";Registers used: R0, R2\n" \
            ";Input register: None\n" \
            ";Return code:    None\n"
        );
    acase 0x16F: // LOADOBJECTS
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Load objects\n" \
            ";Initial data:   data in R0 to $1F00..$1F4D\n" \
            ";Registers used: R0, R2\n" \
            ";Input register: R0\n" \
            ";Return code:    None\n"
        );
    acase 0x171: // CLROBJ_ALT
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Set objects\n" \
            ";Initial data:   data in R0, range is $1F00..$1F00+R2\n" \
            ";Registers used: R0, R2\n" \
            ";Input regs:     R0, R2\n" \
            ";Return code:    None\n"
        );
    acase 0x181: // KBSCAN
    case 0x183: // KBSCAN_ALT
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Keyboard scan\n" \
            ";Initial data:   clear $089F\n" \
            ";Comments:       see chapter 11\n" \
            ";Registers used: R0..R2\n" \
            ";Input register: R0\n" \
            ";Return code:    R1\n" \
            ";Autodoc:\n" \
"; To get a keyboard scan the upper bit of the keyboard status word must\n"  \
"; be reset.\n" \
";   The keyboard returns with a new key address in the lower four (RKBST\n" \
"; LKBST) or five (MKBST) bits of the status word. The status is valid if\n" \
"; the sign bit is set.\n"
        );
    acase 0x20E: // LINE
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Translate MLINE to display\n" \
            ";Comments:       see Table 17 for codes\n" \
            ";Registers used: R0..R3\n" \
            ";Input:          *($890..$897)\n" \
            ";Output:         *($800..$88F)\n"
            ";Category:       Character and line image routines\n" \
            ";Description:    Mon. routine to convert an 8 char line to the\n" \
            ";                screen image memory field MONOB\n"
        ); // aka "load MLINE"
    acase 0x2CF: // SCROLL
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Scroll and load 8 spaces in MLINE\n" \
            ";Registers used: R0..R2\n" \
            ";Input:          *($818..$88F)\n" \
            ";Output:         *($800..$88F)\n" \
            ";Category:       Screen routines\n"
        ); // aka "scroll, 8 spaces to MLINE"
    acase 0x2D9: // SCROL3
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Load 8 spaces to MLINE\n" \
            ";Registers used: R0, R2\n" \
            ";Input register: None\n" \
            ";Return code:    None\n" \
            ";Category:       Screen routines\n"
        );
    acase 0x2E3: // NEWMES
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Scroll, message to MLINE\n" \
            ";Initial data:   R2 is message code\n" \
            ";Registers used: R0..R3\n" \
            ";Category:       Screen routines\n" \
            ";Description:    Output of message to a new line\n"
        );
        note = TRUE;
    acase 0x2EA: // MESSAG
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Output message to MLINE\n" \
            ";Initial data:   R2 is message code\n" \
            ";Registers used: R0, R2, R3\n" \
            ";Category:       Screen routines\n" \
";Autodoc:\n" \
"; Message of 4 characters to beginning of line routine.\n" \
";   Use of registers:\n" \
";     R0    XX\n" \
";     R3    index in MLINE\n" \
";     R2    index in message\n"
        );
        note = TRUE;
    acase 0x31D: // ASMDAT
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Assemble data\n" \
            ";Initial data:   clear $08A7, enter successive digits ($00..$0F) in $08A1\n" \
            ";Comments:       result shifts left in $08A2 (=R2) and $08A3 (=R1)\n" \
            ";Registers used: R0..R3\n" \
            ";Category:       Routines for input and display of constants\n"
            ";Description:    Assemble value\n" \
            ";Autodoc:\n" \
"; (FSCRM+1) is shifted into (FSCRM+2,FSCRM+3).\n" \
";   At exit (R2,R1) is also the new value.\n"
        );
    acase 0x328: // ASSMC
        zprintf
        (   TEXTPEN_LOG,
            ";Category:       Routines for input and display of constants\n"
            ";Autodoc:\n" \
"; This routine adds one hex constant in FSCRM+1 to a double byte hex value\n" \
"; in R2 and R1.\n" \
";   Use of registers:\n" \
";     R0    new hex\n" \
";     R1    lower hex\n" \
";     R2    upper hex\n" \
";     R3    index\n" \
        );
    acase 0x340: // RESDAT
        zprintf
        (   TEXTPEN_LOG,
            ";Category:       Routines for input and display of constants\n" \
            ";Description:    Reset data assembly locations\n"
        );
    acase 0x34E: // OUTADD
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Output double byte to MLINE\n" \
            ";Initial data:   data in R2/R1\n" \
            ";Comments:       last positions\n" \
            ";Registers used: R0..R3\n" \
            ";Category:       Routines for input and display of constants\n" \
            ";Autodoc:\n" \
"; Use of registers:\n" \
";     R0    XX\n" \
";     R1    lower byte\n" \
";     R2    upper byte\n" \
";     R3    position of data in MLINE\n"
        );
    acase 0x350: // OUTADR
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Output double byte to MLINE\n" \
            ";Initial data:   data in R2/R1, position in R3\n" \
            ";Registers used: R0..R3\n" \
            ";Category:       Routines for input and display of constants\n"
        );
    acase 0x354: // OUTBYT
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Output single byte to MLINE\n" \
            ";Initial data:   data in R1, position in R3\n" \
            ";Registers used: R0, R1, R3\n"
            ";Category:       Routines for input and display of constants\n" \
            ";Autodoc:\n" \
"; Use of registers:\n" \
";     R0    scr byte\n" \
";     R1    data byte\n" \
";     R3    MLINE index\n"
        );
    acase 0x35E: // NIBL
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Split register\n" \
            ";Byte to split:  $%02X\n" \
            ";Initial data:   R1=$XY\n" \
            ";Comments:       then R0=$0X, R1=$0Y\n" \
            ";Registers used: R0, R1\n" \
            ";Input register: R1" \
            ";Return codes:   R0, R1\n" \
            ";Category:       Routines for input and display of constants\n",
            r[1]
        );
    acase 0x368: // OUTNAD
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    Output of address being entered\n" \
            ";Category:       Routines for input and display of constants\n"
        );
    acase 0x36C: // OUTADS
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    Output of address (R1,R2) with leading spaces (FSCRM+6)\n" \
            ";Category:       Routines for input and display of constants\n"
        );
    acase 0x377: // SPACES
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    Output of leading spaces\n" \
            ";Category:       Routines for input and display of constants\n"
        );
    acase 0x379: // LDSPCS2MLINE
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Load spaces to MLINE\n" \
            ";Initial data:   first position in R2, number of spaces in R1\n" \
            ";Registers used: R0..R2\n"
        );
    acase 0x381: // STDATA
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    Store data in memory\n"
        );
    acase 0x39F: // INCADR
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Increment double byte\n" \
            ";Initial data:   data in $08A4, $08A5\n" \
            ";Registers used: R0..R1\n" \
            ";Description:    Increment address (FSCRM+4,FSCRM+5)\n"
        );
    acase 0x3B0: // REG01
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    Register display and alter\n"
        );
    acase 0x40C: // MEM01
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    Memory display and alter\n"
        );
    acase 0x42B: // ADDRDTA2MLINE
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Address + data to MLINE\n" \
            ";Initial data:   address in $08A4, $08A5\n" \
            ";Registers used: R0..R3\n"
        );
    acase 0x470: // OUT3BYTS2SCRN
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Output three bytes to screen\n" \
            ";Initial data:   data in $08A4, $08A5 to first positions, $08A3 to right\n" \
            ";Registers used: R0..R3\n"
        );
    acase 0x480: // MEMBYT
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Output single byte to screen\n" \
            ";Initial data:   data in R1, position in R3\n" \
            ";Registers used: R0, R1, R3\n"
        );
    acase 0x4A9: // BKSC
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    Breakpoint set and clear\n"
        );
    acase 0x50E: // GO0
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    Go to address function\n"
        );
    acase 0x529: // GO3
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Output double byte to screen\n" \
            ";Initial data:   data in R1/R2\n" \
            ";Comments:       last positions\n" \
            ";Registers used: R0..R3\n"
        );
    acase 0x52F: // GO1
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Assemble data on screen\n" \
            ";Initial data:   clear $08A7, enter successive digits ($00..$0F) in $08A1\n" \
            ";Comments:       result shifts left in $08A2 (=R2) and $08A3 (=R1)\n" \
            ";Registers used: R0..R3\n"
        );
    acase 0x594: // BREAK1
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    1st breakpoint entry from user program\n"
        );
    acase 0x5B2: // BREAK2
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    2nd breakpoint entry from user program\n"
        );
    acase 0x5E8: // WCAS0
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    Cassette write\n" \
            ";Autodoc:\n" \
"; Begin address:    FSCRM+4,5\n" \
"; End address:      FSCRM+7,8\n" \
"; Start address:    FSCRM+2,3\n" \
"; File no.:         FSCRM+1\n"
        );
    acase 0x602: // WCAS3
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Scroll, message to screen\n" \
            ";Initial data:   R2 is message code\n" \
            ";Registers used: R0..R3\n"
        );
        note = TRUE;
    acase 0x6F7: // PULSE
        zprintf
        (   TEXTPEN_LOG,
            ";Long name:      Delay\n" \
            ";Initial data:   delay is (R3 data) * 200 µsec\n" \
            ";Registers used: R1, R3\n"
        );
    acase 0x758: // RCAS0
        zprintf
        (   TEXTPEN_LOG,
            ";Description:    Read cassette\n" \
            ";Autodoc:\n" \
"; Begin address:    FSCRM+4,5\n" \
"; End address:      FSCRM+7,8\n" \
"; Start address:    FSCRM+2,3\n" \
"; File no.:         FSCRM+1\n"
        );
    }

    if (note)
    {   zprintf
        (   TEXTPEN_LOG,
            ";Message code data is:\n" \
            "; $04='PC=',  $08='AD=',  $0C='R=',   $10='BP1=', $14='BP2',\n" \
            "; $18='BEG=', $1C='END=', $20='SAD=', $24='FIL=', $28='IIII'\n"
        );
}   }
}

EXPORT FLAG save_eof(STRPTR filename, int startaddr, int endaddr, int progstart, int preferredsize)
{   FILE* EOFHandle;
    int   blocksize,
          i,
          tempstart,
          tempend;

    // assert(startaddr <= endaddr);

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Saving %s from $%X..$%X as EOF...\n", filename, startaddr, endaddr);
#endif

    if (!(EOFHandle = fopen(filename, "wb")))
    {   zprintf(TEXTPEN_CLIOUTPUT, "Can't open \"%s\" for writing!\n", filename); // or TEXTPEN_ERROR?
        return FALSE;
    } // implied else

    tempstart = startaddr;
    do
    {   blocksize = endaddr - tempstart + 1;
        if (blocksize > preferredsize)
        {   blocksize = preferredsize;
        }
        tempend = tempstart + blocksize - 1;
        if ((tempstart & 0x6000) != (tempend & 0x6000)) // to avoid crossing an 8K page boundary
        {   tempend = (tempstart & 0x6000) | 0x1FFF;
            blocksize = tempend - tempstart + 1;
        }

        EOFBuffer[0] = 'L';
        EOFBuffer[1] = 0x01;
        EOFBuffer[2] = (UBYTE) (tempstart / 256);
        EOFBuffer[3] = (UBYTE) (tempstart % 256);
        EOFBuffer[4] = (UBYTE) blocksize;
        EOFBuffer[5] = eof_makebcc(0, 4, EOFBuffer);
        for (i = tempstart; i <= tempend; i++)
        {   EOFBuffer[6 + i] = memory[i];
        }
        EOFBuffer[6 + blocksize] = eof_makebcc(6, 6 + blocksize - 1, EOFBuffer);
        DISCARD fwrite(EOFBuffer, (size_t) 7 + blocksize, 1, EOFHandle);

        tempstart += blocksize;
    } while (tempstart <= endaddr);

    // final block
    EOFBuffer[0] = 'L';
    EOFBuffer[1] = 0x01; // file number
    EOFBuffer[2] = (UBYTE) (progstart / 256);
    EOFBuffer[3] = (UBYTE) (progstart % 256);
    EOFBuffer[4] = 0; // length
    EOFBuffer[5] = eof_makebcc(0, 4, EOFBuffer);
    DISCARD fwrite(EOFBuffer, (size_t) 6, 1, EOFHandle);

    DISCARD fclose(EOFHandle);
    // EOFHandle = NULL;

    return TRUE;
}

MODULE UBYTE eof_makebcc(int start, int end, UBYTE* BufferPtr)
{   UBYTE ourbcc = 0;
    int   i;

    for (i = start; i <= end; i++)
    {   ourbcc ^= BufferPtr[i];
        if (ourbcc >= 128)
        {   ourbcc = ((ourbcc & 127) * 2) + 1;
        } else ourbcc <<= 1;
    }
    return ourbcc;
}

MODULE int eof_readblock(int localsize)
{   UBYTE blocklength,
          filenum,
          ourbcc, // *MUST* be UBYTE!
          theirbcc;
    int   i,
          loadaddr;

/* For Elektor files:
   0:                                        $4C ('L') (block header)
   1:                                        file number
   2                  .. 3:                  load/start address
   4:                                        block length
   5:                                        1st BCC
   6                  .. 5 + block length  : data
   6 + block length                        : 2nd BCC

    Return codes are:
       -1  = successful, EOF block
       -2  = successful, data block
        0  = end of file
        1+ = unsuccessful */

    if (cursor == localsize)
    {   return 0;
    }
    if (IOBuffer[cursor] != 'L') // 0
    {   zprintf(TEXTPEN_ERROR, "Error: block header not found at offset %d!\n", cursor);
        return 1;
    }
    filenum = IOBuffer[cursor + 1]; // 1
    if (filenum < 1 || filenum > 15)
    {   zprintf(TEXTPEN_ERROR, "Error: file number is out of range!\n");
        return 1;
    }
    loadaddr = (IOBuffer[cursor + 2] * 256) + IOBuffer[cursor + 3]; // 2..3
    if (loadaddr >= 0x8000)
    {   zprintf(TEXTPEN_ERROR, "Error: address is out of range!\n");
        return 1;
    }
    blocklength     = IOBuffer[cursor + 4]; // 4
    if (blocklength == 0) // should we warn the user if blocklength > 16?
    {   startaddr_h = (UBYTE) (loadaddr / 256); // 2
        startaddr_l = (UBYTE) (loadaddr % 256); // 3
        zprintf(TEXTPEN_CLIOUTPUT, "Found end-of-file block. Start address is $%04X.\n", loadaddr);
        return -1;
    }
    if (cursor + 6 + blocklength > localsize)
    {   zprintf(TEXTPEN_ERROR, "Error: incorrect length!\n");
        return 1;
    }

    ourbcc = eof_makebcc(cursor    , cursor + 4, IOBuffer);
    theirbcc = IOBuffer[cursor + 5];
#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Our header  BCC is $%02X. Their header  BCC is $%02X.\n", ourbcc, theirbcc);
#endif
    if (ourbcc != theirbcc)
    {   zprintf(TEXTPEN_ERROR, "Error: BCC is incorrect!\n");
        return 1;
    }

    ourbcc = eof_makebcc(cursor + 6, cursor + 5 + blocklength, IOBuffer);
    theirbcc = IOBuffer[cursor + 6 + blocklength];
#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Our payload BCC is $%02X. Their payload BCC is $%02X.\n", ourbcc, theirbcc);
#endif
    if (ourbcc != theirbcc)
    {   zprintf(TEXTPEN_ERROR, "Error: BCC is incorrect!\n");
        return 1;
    }

    zprintf(TEXTPEN_CLIOUTPUT, "Loading $%04X..$%04X.\n", loadaddr, loadaddr + blocklength - 1);
    if ((loadaddr & 0x6000) != ((loadaddr + blocklength - 1) & 0x6000))
    {   zprintf(TEXTPEN_ERROR, "Warning: Block $%04X..$%04X crosses a page boundary!\n", loadaddr, loadaddr + blocklength - 1);
    }
    for (i = 0; i < blocklength; i++)
    {   memory[loadaddr + i] = IOBuffer[cursor + 6 + i];
    }
    cursor += 7 + blocklength;
    return -2;
}

EXPORT int load_eof(int localsize)
{   FLAG foundeof = FALSE;
    int  rc       =     1;

    cursor = 0;
    while (cursor < localsize)
    {   rc = eof_readblock(localsize);

        if (rc == -1)
        {   foundeof = TRUE;
            break;
        } elif (rc >= 0)
        {   break;
    }   }

    if (rc <= 0)
    {   zprintf(TEXTPEN_CLIOUTPUT, "Successfully loaded EOF file.\n\n");
        if (!foundeof)
        {   zprintf(TEXTPEN_ERROR, "Warning: no end-of-file block found.\n");
            return 1;
        } else
        {   return 2;
    }   }
    else
    {   zprintf(TEXTPEN_ERROR, "Failed to load EOF file!\n\n");
        return 0;
}   }

EXPORT void elektor_reset(void)
{   int i;

    /* $8A2/$8A3          = start address of game
       $8B9/$8BA (INTADR) = address of game interrupt vector
       $8BE/$8BF (PC)     = start address of game

       There are 3 feasible possibilities for starting the monitor:
       iar =     0; // gives black background with white "00 00" at top
       iar =  0x23; // gives blue  background with yellow "IIII" at bottom
       iar = 0x540; // gives blue  background */

    if (game)
    {   // make $8B9..$8BA = $0903 (important!)
        memory[0x8B9] =  0x09;
        memory[0x8BA] =  0x03;
        memory[0x8BD] =  0x1F; // BCTA,UN
        memory[0x8BE] =  startaddr_h;
        memory[0x8BF] =  startaddr_l;
        iar           =  0x8BD;

        /* The below lines can be used instead of the above lines, but it won't work with Jagen...
        memory[0x8A2] = startaddr_h; // SADR_H
        memory[0x8A3] = startaddr_l; // SADR_L
        iar = 0x540; */
    } else
    {   iar = 0x23; // gives blue  background with yellow "IIII" at bottom
    }

    for (i = 0x1D00; i <= 0x1D1F; i++)
    {   memory[i] = 0; // PSGs are cleared at reset
    }
    if (whichgame != -1 && known[whichgame].hiscore)
    {   psu |= PSU_II;
    }
    if (whichgame == OMEGALANDINGPOS)
    {   psl |= PSL_COM;
    }
    playsound(FALSE);
}

EXPORT int hertz_to_note(float hertz)
{   float midpoint;
    int   i;

    for (i = 0; i < NOTES - 1; i++)
    {   midpoint = notes[i].hertz + ((notes[i + 1].hertz - notes[i].hertz) / (float) 2.0);
        if (hertz < midpoint)
        {   return i;
    }   }

    return NOTES - 1;
}
