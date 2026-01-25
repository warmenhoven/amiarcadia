/* To write a tile:
 we do a WRTE to I/O port $60 to set malzak_x
  (the X-coordinate of where to put the tile)
 we do a STRA $aa to $16y0
  where y is the Y-coordinate of where to put the tile
  (ie. the Y-coordinate is chosen by the high nybble of the low byte of the address)
  and $aa is the tile contents to write (modulo 32)

INCLUDES--------------------------------------------------------------- */

#ifdef AMIGA
    #include "amiga.h"
#endif
#ifdef WIN32
    #include "ibm.h"
#endif

#include <stdio.h>  // for FILE*
#include <string.h> // for memset()

#include "da.h"
#include "malzak.h"

// DEFINES----------------------------------------------------------------

#define ALPHANUMERIC 1
#define CONTIGUOUS   2

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT int    offsetsprites = 0,
              malzak_x,
              malzak_y,
              malzak_bank1, // text data bank
              malzak_bank2; // terrain data bank
EXPORT UBYTE  malzak_field[16][16];

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT UBYTE                bgcoll,
                            coinops_colltable[3][PVI_RASTLINES][PVI_XSIZE],
                            memory[32768],
                            memflags[32768];
IMPORT UWORD                mirror_r[32768],
                            mirror_w[32768];
IMPORT ULONG                analog,
                            frames,
                            region,
                            swapped;
IMPORT int                  coinop_1p,
                            coinop_2p,
                            coinop_coina,
                            coinop_coinb,
                            coinop_joy1left,
                            coinop_joy1right,
                            coinop_joy1up,
                            coinop_joy1down,
                            coinop_joy2left,
                            coinop_joy2right,
                            coinop_button1,
                            coinop_button2,
                            coinop_button3,
                            coinop_button4,
                            collx,
                            colly,
                            console_start,
                            console_a,
                            console_b,
                            console_reset,
                            cpl,
                            game,
                            key1,
                            key2,
                            key3,
                            key4,
                            layout,
                            machine,
                            memmap,
                            offset,
                            p1bgcol[4],
                            p2bgcol[4],
                            p1rumble,
                            p2rumble,
                            p1sprcol[6],
                            p2sprcol[6],
                            pvibase,
                            teletype;
IMPORT struct MachineStruct machines[MACHINES];
IMPORT UBYTE*               IOBuffer;
IMPORT ASCREEN              screen[BOXWIDTH][BOXHEIGHT];

// MODULE VARIABLES-------------------------------------------------------

MODULE UBYTE                held_char,
                            lastchar;
MODULE FLAG                 conceal,
                            m_double_height,
                            m_flash,
                            m_graphics,
                            skipnextrow;
MODULE int                  m_bg, m_fg;

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void malzak_drawtext(UBYTE code, int x, int y);
MODULE void malzak_drawtile(UBYTE code, int x, int y);
MODULE void draw_teletext(void);

// CODE-------------------------------------------------------------------

MODULE void malzak_drawtile(UBYTE code, int x, int y)
{   FAST int   inverted,
               xx, yy;
    FAST UBYTE colour;

/* The imagery is laid out in memory as follows for each 16*16 tile:
(0,0), (1,0),
(0,1), (1,1).

We are assuming, for no reason:
$00..$1F: YELLOW
$20..$2F: RED
$30..$3B: GREEN
$3C..$3F: RED
$40..$5F: BLUE
$60..$7F: GREEN
$80..$FF: RED */

    switch (code)
    {
    case 0x00:
    case 0x40:
    case 0x60:
    case 0x80:
    case 0xA0:
    case 0xC0:
    case 0xE0:
        return; // for speed
    case 0x01: //lint -fallthrough
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
    case 0x0E:
    case 0x0F:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
    case 0x1F:
        colour = YELLOW;
        inverted = FALSE;
    acase 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x26:
    case 0x27:
    case 0x28:
    case 0x29:
    case 0x2A:
    case 0x2B:
    case 0x2C:
    case 0x2D:
    case 0x2E:
    case 0x2F:
    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
    case 0x34:
    case 0x35:
    case 0x36:
    case 0x37:
    case 0x38:
    case 0x39:
    case 0x3A:
    case 0x3B:
        colour = GREEN;
        inverted = FALSE;
        code &= 0x1F;
    acase 0x25:
        colour = GREEN;
        inverted = TRUE;
        code &= 0x1F;
    acase 0x3C:
    case 0x3D:
    case 0x3E:
    case 0x3F:
        colour = RED; // 2nd colour
        inverted = FALSE;
        code &= 0x1F;
    acase 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4A:
    case 0x4B:
    case 0x4C:
    case 0x4D:
    case 0x4E:
    case 0x4F:
    case 0x50:
    case 0x51:
    case 0x52:
    case 0x53:
    case 0x54:
    case 0x55:
    case 0x56:
    case 0x57:
    case 0x58:
    case 0x59:
    case 0x5A:
    case 0x5B:
    case 0x5C:
    case 0x5D:
    case 0x5E:
    case 0x5F:
        colour = BLUE;
        inverted = FALSE;
        code &= 0x1F;
    acase 0x61:
    case 0x62:
    case 0x63:
    case 0x64:
    case 0x65:
    case 0x66:
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x6A:
    case 0x6B:
    case 0x6C:
    case 0x6D:
    case 0x6E:
    case 0x6F:
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7A:
    case 0x7B:
    case 0x7C:
    case 0x7D:
    case 0x7E:
    case 0x7F:
        colour = GREEN; // 1st colour
        inverted = FALSE;
        code &= 0x1F;
    adefault:
        // assert(code >= 0x81);
        // assert(code <= 0xFF);
        colour = RED;
        if (code >= 0xEC && code <= 0xEE)
        {   // also $EF, but that causes problems with
            // Malzak glyph on title screen
            code &= 0x3F;
            inverted = FALSE;
        } else
        {   if (code == 0xC5)
            {   inverted = TRUE;
            } else
            {   inverted = FALSE;
            }
            code &= 0x1F;
    }   }

    for (yy = 0; yy < 16; yy++)
    {   for (xx = 0; xx < 8; xx++)
        {   if
            (   x + xx >= 0
             && x + xx <  machines[machine].width
             && y + yy >= 0
             && y + yy <  machines[machine].height
            )
            {   if (m_tiles[(code * 32) + (yy * 2)] & (1 << xx))
                {   if (!inverted)
                    {   changepixel(x + xx, y + yy, colour);
                        coinops_colltable[0][y + yy][x + xx + HIDDEN_X] |= 8;
                }   }
                else
                {   if (inverted)
                    {   changepixel(x + xx, y + yy, colour);
                        coinops_colltable[0][y + yy][x + xx + HIDDEN_X] |= 8;
        }   }   }   }

        for (xx = 0; xx < 8; xx++)
        {   if
            (   x + xx + 8 >= 0
             && x + xx + 8 <  machines[machine].width
             && y + yy     >= 0
             && y + yy     <  machines[machine].height
            )
            {   if (m_tiles[(code * 32) + (yy * 2) + 1] & (1 << xx))
                {   if (!inverted)
                    {   changepixel(x + xx + 8, y + yy, colour);
                        coinops_colltable[0][y + yy][x + xx + 8 + HIDDEN_X] |= 8;
                }   }
                else
                {   if (inverted)
                    {   changepixel(x + xx + 8, y + yy, colour);
                        coinops_colltable[0][y + yy][x + xx + 8 + HIDDEN_X] |= 8;
}   }   }   }   }   }

EXPORT void switchbank(int whichbank)
{   int i;

    switch (whichbank)
    {
    case 1:
        for (i = 0; i <= 0x3FF; i++)
        {   memory[0xC00 + i]  = bank1[malzak_bank1][i];
        }
    acase 2:
        for (i = 0; i <= 0x5FF; i++) // $4400..$49FF
        {   memory[0x4400 + i] = bank2[malzak_bank2][i];
}   }   }

EXPORT void malzak_setmemmap(void)
{   int   i, address, mirror;
    FILE* TheLocalHandle; // LocalHandle is a variable of winbase.h

    game = FALSE;
    // assert(memmap == MEMMAP_MALZAK1 || memmap == MEMMAP_MALZAK2);

    for (i = 0x1000; i <= 0x7FFF; i++)
    {   memory[i] = 0; // important
    }

    if (memmap == MEMMAP_MALZAK1)
    {   for (i = 0; i <= 0xFFF; i++)
        {   memory[i] = m1_bios[i];                   // $0000..$0FFF -> $0000..$0FFF (4K)
        }
        for (i = 0; i <= 0x7FF; i++)
        {   memory[0x2000 + i] = m1_bios[0x1000 + i]; // $1000..$17FF -> $2000..$27FF (2K)
        }
        for (i = 0; i <= 0x9FF; i++)
        {   memory[0x4000 + i] = m1_bios[0x1800 + i]; // $1800..$21FF -> $4000..$49FF (2.5K)
    }   }
    elif (memmap == MEMMAP_MALZAK2)
    {   for (i = 0; i <= 0xBFF; i++)
        {   memory[         i] = m2_bios[         i]; // $0000..$0BFF -> $0000..$0BFF (3K)
        }
        // $C00..$FFF (1K) is banked (so 1K*2)
        for (i = 0; i <= 0xBFF; i++)
        {   memory[0x2000 + i] = m2_bios[0xC00  + i]; // $0C00..$17FF -> $2000..$2BFF (3K)
        }
        for (i = 0; i <= 0x3FF; i++)
        {   memory[0x4000 + i] = m2_bios[0x1800 + i]; // $1800..$1BFF -> $4000..$43FF (1K)
        }
        // $4400..$49FF (1.5K) is probably banked (so 1.5K*2)
        for (i = 0; i <= 0x3FF; i++)
        {   memory[0x6000 + i] = m2_bios[0x1C00 + i]; // $1C00..$1FFF -> $6000..$63FF (1K)
        }

        malzak_bank1 = 0;
        malzak_bank2 = 1;
        switchbank(1);
        switchbank(2);

        // if (!malzak_loadnvram())
        {   for (i = 0; i <= 255; i++)
            {   memory[0x1700 + i] = default_nvram[i];
            }
            memory[0x14CC] = default_nvram[256];
    }   }

    for (address = 0; address < 32 * KILOBYTE; address++)
    {   memflags[address] = 0;
        mirror            = address;

        if
        (   (address <= 0x0FFF)                      // ROM
         || (address >= 0x2000 && address <= 0x2FFF) // ROM
         || (address >= 0x4000 && address <= 0x4FFF) // ROM
         || (address >= 0x6000 && address <= 0x6FFF) // ROM
        )
        {   memflags[address] |= NOWRITE;
        }

        if
        (
        // legal:        $1400..$142D                   hardware registers, RAM, h/w regs, RAM, h/w regs
            (address >= 0x142E && address <= 0x143F) // unmapped
        // legal:        $1440..$146D                   hardware registers, RAM
         || (address >= 0x146E && address <= 0x147F) // unmapped
        // legal:        $1480..$14AD                   grid, RAM
         || (address >= 0x14AE && address <= 0x14BF) // unmapped
        // legal:        $14C0..$14C3                   hardware registers
         || (address >= 0x14C4 && address <= 0x14C5) // unused
        // legal:        $14C6..$14C9                   hardware registers
         || (address >= 0x14CA && address <= 0x14FF) // R/- hardware registers, unused, mirrors (not occurring), semi-mirrors
        // legal:        $1500..$152D                   hardware registers, RAM, h/w regs, RAM, h/w regs
         || (address >= 0x152E && address <= 0x153F) // unmapped
        // legal:        $1540..$156D                   hardware registers, RAM
         || (address >= 0x156E && address <= 0x157F) // unmapped
        // legal:        $1580..$15AD                   grid, RAM
         || (address >= 0x15AE && address <= 0x15BF) // unmapped
        // legal:        $15C0..$15C3                   hardware registers
         || (address >= 0x15C4 && address <= 0x15C5) // unused
        // legal:        $15C6..$15C9                   hardware registers
         || (address >= 0x15CA && address <= 0x15FF) // R/- hardware registers, unused, mirrors (not occurring), semi-mirrors
        )
        {   memflags[address] |= NOWRITE;
        }

        if
        (
    // legal:            $1400              ..$142D     hardware registers, RAM, h/w regs, RAM, h/w regs
            (address >= 0x142E && address <= 0x143F) // unmapped
    // legal:            $1440              ..$146D     hardware registers, RAM
         || (address >= 0x146E && address <= 0x147F) // unmapped
    // legal:            $1480              ..$14AD     grid, RAM
         || (address >= 0x14AE && address <= 0x14C9) // unmapped, -/W (write-only) and -/- (unusable) hardware registers
    // legal:            $14CA              ..$14CD     R/- (read-only) hardware registers
         || (address >= 0x14CE && address <= 0x14D9) // unused, mirrors (not occurring)
    // legal:            $14DA              ..$14DB     semi-mirrors
         || (address >= 0x14DC && address <= 0x14E9) // mirrors (not occurring)
    // legal:            $14EA              ..$14EB     semi-mirrors
         || (address >= 0x14EC && address <= 0x14F9) // mirrors (not occurring)
    // legal:            $14FA              ..$14FB     semi-mirrors
         || (address >= 0x14FC && address <= 0x14FF) // mirrors (not occurring)
    // legal:            $1500              ..$152D     hardware registers, RAM, h/w regs, RAM, h/w regs
         || (address >= 0x152E && address <= 0x153F) // unmapped
    // legal:            $1540              ..$156D     hardware registers, RAM
         || (address >= 0x156E && address <= 0x157F) // unmapped
    // legal:            $1580              ..$15AD     grid, RAM
         || (address >= 0x15AE && address <= 0x15C9) // unmapped, -/W (write-only) and -/- (unusable) hardware registers
    // legal:            $15CA              ..$15CD     R/- (read-only) hardware registers
         || (address >= 0x15CE && address <= 0x15D9) // unused, mirrors (not occurring)
    // legal:            $15DA              ..$15DB     semi-mirrors
         || (address >= 0x15DC && address <= 0x15E9) // mirrors (not occurring)
    // legal:            $15EA              ..$15EB     semi-mirrors
         || (address >= 0x15EC && address <= 0x15F9) // mirrors (not occurring)
    // legal:            $15FA              ..$15FB     semi-mirrors
         || (address >= 0x15FC && address <= 0x15FF) // mirrors (not occurring)
        )
        {   memflags[address] |= NOREAD;
        }

        if
        (   (address >= 0x14CA && address <= 0x14CB)
         || (address >= 0x14DA && address <= 0x14DB)
         || (address >= 0x14EA && address <= 0x14EB)
         || (address >= 0x14FA && address <= 0x14FB)
         || (address >= 0x15CA && address <= 0x15CB)
         || (address >= 0x15DA && address <= 0x15DB)
         || (address >= 0x15EA && address <= 0x15EB)
         || (address >= 0x15FA && address <= 0x15FB)
        )
        {   memflags[address] |= READONCE; // semi-mirrors
        }

        if (address >= 0x1600 && address <= 0x16FF)
        {   memflags[address] |= FIELD;
        }

        if
        (   address == 0x1400 + PVI_PITCH
         || address == 0x1500 + PVI_PITCH
        )
        {   memflags[address] |= AUDIBLE;
        }

        if (address & 0x1000) // $3xxx -> $1xxx, $5xxx -> $1xxx, $7xxx -> $1xxx
        {   mirror = (address & 0x0FFF) | 0x1000;
        }

        mirror_r[address] =
        mirror_w[address] = (UWORD) mirror;
    }

    patchrom();

    pvibase          = 0x1400;
    key1             =
    key2             =
    key3             =
    key4             = 2;
    layout           = W8;
    bgcoll           = 0;
    analog           =
    swapped          = FALSE;
    for (i = 0; i < 4; i++)
    {   p1bgcol[i]  =
        p2bgcol[i]  = 0;
    }
    for (i = 0; i < 6; i++)
    {   p1sprcol[i] =
        p2sprcol[i] = 0;
    }
    p1bgcol[0] = 1;

#ifdef MALZAK_885KHZ
    set_cpl(227 / 4); // 56.75
    machines[machine].cpf = (int) ((227.0 / 4.0) * 312.0);
#endif
#ifdef MALZAK_1180KHZ
    set_cpl(227 / 3); // 75.6'
    machines[machine].cpf = (int) ((227.0 / 3.0) * 312.0);
#endif
    region = REGION_PAL;
}

EXPORT void malzak_drawscreen(void)
{   FAST int ssx, ssy,
             x, y;

    for (x = 0; x < 240; x++)
    {   for (y = 0; y < 240; y++)
        {   changebgpixel(x, y, BLACK);
    }   }
    draw_teletext();

/* malzak_y is the inverted size of the top margin. Ie.
 low values of malzak_y mean the display starts towards the bottom (  0 would mean start at row       0)
high values of malzak_y mean the display starts towards the top    (255 would mean start at row    -255)

    During play, malzak_y is always 0 at the time that the screen is redrawn. */

    for (y = 0; y < 16; y++)
    {   for (x = 0; x < 16; x++)
        {   // malzak_field[][] is a grid of bytes, containing the codes for each tile
            ssx = ((479 + (x * 16) - malzak_x) % 256) - 15 + offsetsprites;
            ssy =         (y * 16) - malzak_y;
            malzak_drawtile(malzak_field[x][y], ssx, ssy);
}   }   }

EXPORT UBYTE malzak_readport(int port)
{   FAST UBYTE t;
 // FAST int   i;

    // assert(machine == MALZAK);

    switch (port)
    {
    case 0:
        if (bgcoll)
        {   /* for (i = 0; i < 8; i++)
            {   if (bgcoll & (0x80 >> i))
                {   if (i == 0) zprintf(TEXTPEN_VERBOSE, "Sprite #%d hit background at %d,%d on frame %d!\n", i, collx, colly, frames);
            }   } */
            t = ((colly / 16) * 16) | (((malzak_x + 32 + collx) % 256) / 16);
        } else
        {   t = 0;
        }
    acase 0x80:
    case 0xA0:
        t = 0;
        if (coinop_joy1up)
        {   t |= 0x80; // set bit 7 (%10000000)
        }
        if (coinop_joy1left)
        {   t |= 0x40; // set bit 6 (%01000000)
        }
        if (coinop_joy1right)
        {   t |= 0x20; // set bit 5 (%00100000)
        }
        if (coinop_button1)
        {   t |= 0x10; // set bit 4 (%00010000)
        }
        if (coinop_2p)
        {   t |= 0x08; // set bit 3 (%00001000)
        }
        if (coinop_1p)
        {   t |= 0x04; // set bit 2 (%00000100)
        }
        if (coinop_joy1down)
        {   t |= 0x02; // set bit 1 (%00000010)
        }
        if (coinop_coina)
        {   t |= 0x01; // set bit 0 (%00000001)
        }
    adefault: // eg. $20, $65
        t = 0;
    }

    return t;
}

EXPORT void malzak_writeport(int port, UBYTE data)
{   switch (port)
    {
    case 0x40: /* possibly used for SN76477 sound?
    Bit  7:    unknown
    Bit  6:    set high after checking the selected version
                (Malzak 2 only)
    Bit  5:    unused?
    Bit  4:    set high, then low, upon death
    Bits 3..1: are all set high upon death, until the game continues
    Bit  1:    set high on boot and on the title screens and during demo
    Bit  0:    constantly set high during paid gameplay */
        if (data & 0x40)
        {   if (malzak_bank1 == 0)
            {   malzak_bank1 = 1; // means Malzak 2
                switchbank(1);
        }   }
        else
        {   if (malzak_bank1 == 1)
            {   malzak_bank1 = 0; // means Malzak 1
                switchbank(1);
        }   }
    acase 0x60:
        malzak_x = data;
 /* acase 0xA0:
        game writes an echo of port $80 here */
    acase 0xC0:
        malzak_y = data;
}   }

EXPORT void malzak_emuinput(void)
{   TRANSIENT UBYTE t;
    TRANSIENT int   old_coina;
    PERSIST   FLAG  holding = FALSE;

        old_coina        = coinop_coina;

        coinop_1p        =
        coinop_2p        =
        coinop_coina     =
        coinop_coinb     =
        coinop_joy1left  =
        coinop_joy1right =
        coinop_joy1up    =
        coinop_joy1down  =
        coinop_joy2left  =
        coinop_joy2right =
        coinop_button1   =
        coinop_button2   =
        coinop_button3   =
        coinop_button4   = 0;

        coinop_emuinput();
        p1rumble =
        p2rumble = 0;

        if (console_start)
        {   coinop_1p |= 1;
        }
        if (console_a)
        {   coinop_2p |= 1;
        }
        if (console_b) // active low
        {   coinop_coina |= 1;
        } elif (old_coina & 1)
        {   play_sample(SAMPLE_COIN);
        }

        if (console_reset)
        {   holding = TRUE;
        } elif (holding)
        {   holding = FALSE;

            if (memmap == MEMMAP_MALZAK2)
            {   switch (memory[0x1400 + PVI_P1PADDLE])
                {
                case 0xF0:  // state "1", change to "2"
                    memory[0x1400 + PVI_P1PADDLE] = 0x90;
                acase 0x90: // state "2", change to "3"
                    memory[0x1400 + PVI_P1PADDLE] = 0x70;
                acase 0x70: // state "3", change to "4"
                    memory[0x1400 + PVI_P1PADDLE] = 0x00;
                adefault:   // state "4" (eg. $00), change to "1"
                    memory[0x1400 + PVI_P1PADDLE] = 0xF0;
        }   }   }

    if (console_start) console_start--;
    if (console_a    ) console_a--;
    if (console_b    ) console_b--;
    if (console_reset) console_reset--;
}

MODULE void draw_teletext(void)
{   FAST int   x, y;
    FAST UBYTE data;

    skipnextrow = FALSE;

    for (y = 0; y < 25; y++)
    {   if (skipnextrow)
        {   skipnextrow = FALSE;
            continue;
        } // implied else

        m_fg = 7;
        m_bg = 0;
        m_graphics = FALSE;
        m_flash = FALSE;
        m_double_height = FALSE;
        conceal = FALSE;
        held_char = ' ';

        for (x = 0; x < 42; x++)
        {   data = memory[0x1800 + (y * 64) + x] & 0x7F;
            if (data <= 0x1F)
            {   switch (data)
                {
                case 0:
                    m_bg       = 0;
                acase 1: // red
                case 2: // green
                case 3: // yellow
                case 4: // blue
                case 5: // purple
                case 6: // cyan
                case 7: // white
                    m_graphics = FALSE;
                    m_fg       = data & 0x07;
                    conceal    = FALSE;
                acase 8:        m_flash = TRUE;
                acase 9:        m_flash = FALSE;
                acase 0xC:      m_double_height = FALSE;
                acase 0xD:      m_double_height = TRUE;
                                if (memmap == MEMMAP_MALZAK2 && y == 3) m_bg = 7;
                                skipnextrow = TRUE;
                acase 0x10: // graphics black
                case 0x11: // graphics red
                case 0x12: // graphics green
                case 0x13: // graphics yellow
                case 0x14: // graphics blue
                case 0x15: // graphics purple
                case 0x16: // graphics cyan
                case 0x17: // graphics white
                    m_graphics = TRUE;
                    m_fg       = data & 0x07;
                    conceal    = FALSE;
                acase 0x18:
                    conceal    = TRUE;
                acase 0x1C:
                    m_bg       = 0;
                acase 0x1D:
                    if (memmap != MEMMAP_MALZAK2 || y != 3) m_bg = m_fg;
                acase 0x1E:
                    held_char  = lastchar;
                acase 0x1F:
                    held_char  = ' ';
                adefault:
                    ; // zprintf(TEXTPEN_VERBOSE, "Unsupported Teletext control code $%02X!\n", data);
                }
                data = held_char;
            }
            if
            (   conceal
             || (m_flash && (frames % 50) > 38)
            )
            {   data = ' ';
            }
            lastchar = data;
            malzak_drawtext
            (   data,
                x * 6,
                y * 10
            );
}   }   }

MODULE void malzak_drawtext(UBYTE code, int x, int y)
{   FAST int   bgc,
               fgc,
               xx, yy;
    FAST UBYTE t;

    fgc = from_malzak[m_fg];
    bgc = from_malzak[m_bg];

    if (m_double_height)
    {   for (yy = 0; yy < 10; yy++)
        {   for (xx = 0; xx < 6; xx++)
            {   if
                (   x +  xx          >= COINOP_BOXWIDTH
                 || y + (yy * 2) + 1 >= COINOP_BOXHEIGHT
                )
                {   continue;
                } // implied else

                if (!m_graphics || !(code & 0x20))
                {   t = m_chars[     ((code - 32) * 10) + yy];
                } else
                {   t = m_contiguous[((code - 32) * 10) + yy];
                }
                if (t & (0x20 >> xx))
                {   changepixel(x + xx, y + (yy * 2)    , fgc);
                    changepixel(x + xx, y + (yy * 2) + 1, fgc);
                } else
                {   changepixel(x + xx, y + (yy * 2)    , bgc);
                    changepixel(x + xx, y + (yy * 2) + 1, bgc);
    }   }   }   }
    else
    {   for (yy = 0; yy < 10; yy++)
        {   for (xx = 0; xx < 6; xx++)
            {   if
                (   x + xx >= COINOP_BOXWIDTH
                 || y + yy >= COINOP_BOXHEIGHT
                )
                {   continue;
                } // implied else

                if (!m_graphics || !(code & 0x20))
                {   t = m_chars[     ((code - 32) * 10) + yy];
                } else
                {   t = m_contiguous[((code - 32) * 10) + yy];
                }
                if (t & (0x20 >> xx))
                {   changepixel(x + xx, y + yy, fgc);
                } else
                {   changepixel(x + xx, y + yy, bgc);
}   }   }   }   }

