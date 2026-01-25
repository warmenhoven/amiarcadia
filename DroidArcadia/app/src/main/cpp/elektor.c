// INCLUDES---------------------------------------------------------------

#include "da.h"
#include "elektor.h"

/* DEFINES----------------------------------------------------------------

(None)

EXPORTED VARIABLES----------------------------------------------------- */

EXPORT       TEXT                 letters[8 + 1];

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       FLAG                 bangercharging;
IMPORT       UBYTE                memflags[32768],
                                  memory[32768],
                                  startaddr_h,
                                  startaddr_l;
IMPORT       UWORD                mirror_r[32768],
                                  mirror_w[32768];
IMPORT       int                  cpl,
                                  elektor_biosver,
                                  game,
                                  machine,
                                  memmap,
                                  nextinst,
                                  psgbase,
                                  pvibase,
                                  traceverbose;
IMPORT       UBYTE*               IOBuffer;
IMPORT       struct MachineStruct machines[MACHINES + 1];

#ifdef AMIGA
    IMPORT   struct Catalog*      CatalogPtr;
#endif
#ifdef WIN32
    IMPORT   int                  CatalogPtr; // APTR doesn't work
#endif

/* MODULE VARIABLES-------------------------------------------------------

(None)

MODULE FUNCTIONS----------------------------------------------------------

(None)

CODE------------------------------------------------------------------- */

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

    game = FALSE;
    bangercharging = FALSE;

    pvibase = 0x1F00;

    if (elektor_biosver == ELEKTOR_PHILIPS)
    {   for (i = 0; i <= 0x7FF; i++)
        {   memory[i] = e_bios_philips[i];
    }   }
    else
    {   // assert(elektor_biosver == ELEKTOR_HOBBYMODULE);
        for (i = 0; i <= 0x7FF; i++)
        {   memory[i] = e_bios_hobbymodule[i];
    }   }

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
    memory[0x1F00 + PVI_P1PADDLE] = // important for Helicopter
    memory[0x1F00 + PVI_P2PADDLE] = machines[ELEKTOR].digipos[1];
    
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
            {   memflags[address] |= RANDOM;
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

    psgbase = 0x1D00;

    nextinst = 0;
    set_cpl(226); // when using 227, bottom left pair of BIOS digits are not always visible

    set_retuning();
}
