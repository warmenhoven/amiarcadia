// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
#endif
#ifdef WIN32
    #include "ibm.h"
#endif

#include <stdio.h>
#include <stdlib.h> // for rand()

#include "aa.h"
#include "interton.h"

// DEFINES----------------------------------------------------------------

// (none)

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT FLAG                 bangercharging;
IMPORT UBYTE                memory[32768];
IMPORT UWORD                mirror_r[32768],
                            mirror_w[32768];
IMPORT int                  cpl,
                            filesize,
                            game,
                            memmap,
                            nextinst,
                            pvibase,
                            randomizememory,
                            usestubs;
IMPORT MEMFLAG              memflags[ALLTOKENS];
IMPORT struct MachineStruct machines[MACHINES];

// CODE-------------------------------------------------------------------

EXPORT void interton_setmemmap(void)
{   int i, address;

    // assert(!crippled);
    // assert(!inframe);
    game = FALSE;
    bangercharging = FALSE;

    pvibase = 0x1F00;

    if (randomizememory)
    {   for (i = 0; i <= 0x7FFF; i++)
        {   memory[i] = rand() & 0xFF; // ie. rand() % 256
    }   }
    else
    {   for (i = 0; i <= 0x7FFF; i++)
        {   memory[i] = 0;
    }   }
    memory[pvibase + PVI_P1PADDLE] =
    memory[pvibase + PVI_P2PADDLE] = machines[INTERTON].digipos[1];

    if (usestubs)
    {   for (i = 0; i < 0x11F; i++) // 287 bytes
        {   memory[i] = i_bios[i];
    }   }

    for (address = 0; address < 32 * KILOBYTE; address++)
    {   memflags[address] = 0;
        mirror_r[address] =
        mirror_w[address] = (UWORD) address;

        // this is only responsible for $0000..$15FF and $1800..$1DFF
        // regions
        switch (memmap)
        {
        case MEMMAP_A: // 2K ROM + 0K RAM
            if
            (    address <= 0x15FF
             || (address >= 0x1800 && address <= 0x1DFF)
            )
            {   memflags[address] |= NOWRITE;
            }
            if
            (   (address >=  0x800 && address <= 0x15FF)
             || (address >= 0x1800 && address <= 0x1DFF)
            )
            {   memflags[address] |= NOREAD;
            }
            // $0000..$07FF: ROM
            // $0800..$15FF: unused
            // $1600..$17FF: mirror of $1E00..$1FFF, as usual
        acase MEMMAP_B: // 4K ROM + 0K RAM
            if
            (    address <= 0x15FF
             || (address >= 0x1800 && address <= 0x1DFF)
            )
            {   memflags[address] |= NOWRITE;
            }
            if
            (   (address >= 0x1000 && address <= 0x15FF)
             || (address >= 0x1800 && address <= 0x1DFF)
            )
            {   memflags[address] |= NOREAD;
            }
            // $0000..$0FFF: ROM
            // $1000..$15FF: unused
            // $1600..$17FF: mirror of $1E00..$1FFF, as usual
        acase MEMMAP_C: // 4K ROM + 1K RAM
            if (address <= 0x0FFF)
            {   memflags[address] |= NOWRITE;
            }
            // we are allowed to read everything (ROM, cartridge RAM,
            // mirror of noise/input/PVI, mirror of cartridge RAM)
            // $0000..$0FFF: ROM
            // $1000..$15FF: cartridge RAM, and mirror of cartridge RAM
            // $1600..$17FF: mirror of $1E00..$1FFF, as usual
            // $1800..$1DFF: mirror of cartridge RAM
        acase MEMMAP_D: // 6K ROM + 1K RAM
            if (address <= 0x15FF)
            {   memflags[address] |= NOWRITE; // $0000..$15FF: 5.5K of ROM
            }
            // we are allowed to read everything (ROM, cartridge RAM,
            // mirror of noise/input/PVI, mirror of cartridge RAM)
            // $0000..$15FF: ROM
            // $1600..$17FF: mirror of $1E00..$1FFF, as usual
            // $1800..$1DFF: cartridge RAM, and mirror of cartridge RAM
        }

        if
        (
            (address >= 0x1E00 && address <= 0x1E7F) // unmapped
        // legal:        $1E80                          hardware register
         || (address >= 0x1E81 && address <= 0x1EFF) // unknown, R/- hardware registers, unknown
         ||  address >= 0x2000
        )
        {   memflags[address] |= NOWRITE;
        }

        if
        (   (address >= 0x1E00 && address <= 0x1E7F) // unmapped
    // legal:            $1E80                          hardware register
         || (address >= 0x1E81 && address <= 0x1E87) // unknown
    // legal:            $1E88              ..$1E8E     R/- hardware registers
         || (address >= 0x1E8F && address <= 0x1EFF) // unknown
         ||  address >= 0x2000
        )
        {   memflags[address] |= NOREAD;
        }

        if (address == IE_NOISE)
        {   memflags[address] |= AUDIBLE;
        }

        /* An address such as $77FF will be a 3-level mirror!
        $77FF -> $17FF after primary   mirroring
        $17FF -> $1FFF after secondary mirroring
        $1FFF -> $1FCF after tertiary  mirroring */

        // primary write mirroring
        if (mirror_r[address] >= 0x2000)
        // $2000..$3FFF: mirror of $0000..$1FFF
        // $4000..$5FFF: mirror of $0000..$1FFF
        // $6000..$7FFF: mirror of $0000..$1FFF
        {   mirror_r[address] &= 0x1FFF;
        }
        // secondary read mirroring
        if (mirror_r[address] >= 0x1600 && mirror_r[address] <= 0x17FF) // mirror of $1E00..$1FFF
        {   mirror_r[address] += 0x800;
        }
        // tertiary read mirroring
        // mirrors of $1C00..$1C09 and $1C0C..$1C0F.
        // $1C0A..$1C0B are semi-mirrored.
        if
        (   (mirror_r[address] >= 0x1FD0 && mirror_r[address] <= 0x1FD9)
         || (mirror_r[address] >= 0x1FDC && mirror_r[address] <= 0x1FE9)
         || (mirror_r[address] >= 0x1FEC && mirror_r[address] <= 0x1FF9)
         || (mirror_r[address] >= 0x1FFC && mirror_r[address] <= 0x1FFE) // $1FFF is a randomizer
        )
        {   mirror_r[address] &= 0xFFCF; // apply mask of %1111111111001111
        }

        // primary write mirroring
        if (mirror_w[address] >= 0x2000)
        // $2000..$3FFF: mirror of $0000..$1FFF
        // $4000..$5FFF: mirror of $0000..$1FFF
        // $6000..$7FFF: mirror of $0000..$1FFF
        {   mirror_w[address] &= 0x1FFF;
        }
        // secondary write mirroring
        if (mirror_w[address] >= 0x1600 && mirror_w[address] <= 0x17FF) // mirror of $1E00..$1FFF
        {   mirror_w[address] += 0x800;
        }
        // tertiary write mirroring
        // mirrors of $1C00..$1C09 and $1C0C..$1C0F.
        // $1C0A..$1C0B are semi-mirrored.
        if
        (   (mirror_w[address] >= 0x1FD0 && mirror_w[address] <= 0x1FD9)
         || (mirror_w[address] >= 0x1FDC && mirror_w[address] <= 0x1FE9)
         || (mirror_w[address] >= 0x1FEC && mirror_w[address] <= 0x1FF9)
         || (mirror_w[address] >= 0x1FFC && mirror_w[address] <= 0x1FFE) // $1FFF is a randomizer
        )
        {   mirror_w[address] &= 0xFFCF; // apply mask of %1111111111001111
        }

        ie_inputmirrors(address);

        if (memmap == MEMMAP_C)
        {   if (mirror_r[address] >= 0x1400 && mirror_r[address] <= 0x15FF) // mirror of $1000..$11FF
            {   mirror_r[address] -= 0x400;
            } elif (mirror_r[address] >= 0x1800 && mirror_r[address] <= 0x1BFF) // mirror of $1000..$13FF
            {   mirror_r[address] -= 0x800;
            } elif (mirror_r[address] >= 0x1C00 && mirror_r[address] <= 0x1DFF) // mirror of $1400..$15FF (which is itself a mirror of $1000..$11FF)
            {   mirror_r[address] -= 0xC00; // 2-level mirror, resolved completely here
            }

            if (mirror_w[address] >= 0x1400 && mirror_w[address] <= 0x15FF) // mirror of $1000..$11FF
            {   mirror_w[address] -= 0x400;
            } elif (mirror_w[address] >= 0x1800 && mirror_w[address] <= 0x1BFF) // mirror of $1000..$13FF
            {   mirror_w[address] -= 0x800;
            } elif (mirror_w[address] >= 0x1C00 && mirror_w[address] <= 0x1DFF) // mirror of $1400..$15FF (which is itself a mirror of $1000..$11FF)
            {   mirror_w[address] -= 0xC00; // 2-level mirror, resolved completely here
        }   }
        elif (memmap == MEMMAP_D)
        {   if (mirror_r[address] >= 0x1C00 && mirror_r[address] <= 0x1DFF) // mirror of $1800..$19FF
            {   mirror_r[address] -= 0x400;
            }

            if (mirror_w[address] >= 0x1C00 && mirror_w[address] <= 0x1DFF) // mirror of $1800..$19FF
            {   mirror_w[address] -= 0x400;
    }   }   }

    memflags[IE_NOISE       ] |= ASIC;
    memflags[IE_P1LEFTKEYS  ] |= ASIC;
    memflags[IE_P1MIDDLEKEYS] |= ASIC;
    memflags[IE_P1RIGHTKEYS ] |= ASIC;
    memflags[IE_CONSOLE     ] |= ASIC;
    memflags[IE_P2LEFTKEYS  ] |= ASIC;
    memflags[IE_P2MIDDLEKEYS] |= ASIC;
    memflags[IE_P2RIGHTKEYS ] |= ASIC;

    memory[IE_NOISE] = 0x04; // Come Come never writes to NOISE, but expects tones to be audible anyway

    pvi_memmap();

    nextinst = 0;
    set_cpl(227);

    set_retuning();
}

EXPORT void interton_serializerom(void)
{   int i;

    // assert(machine == INTERTON);
    // assert(serializemode == SERIALIZE_READ);
    // assert(filesize < 6 * KILOBYTE);

    for (i = 0; i < filesize; i++)
    {   serialize_byte(&memory[i]); // $0000..$17FF -> $0000..$17FF
}   }

EXPORT void interton_reset(void)
{   int i;

    for (i = 0x1800; i <= 0x1FFF; i++)
    {   memory[i] = 0;
    }
    memory[IE_NOISE] = 0x04; // Come Come never writes to NOISE, but expects tones to be audible anyway
}
