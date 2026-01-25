// INCLUDES---------------------------------------------------------------

#include "da.h"

#include <stdlib.h> // for rand()

// DEFINES----------------------------------------------------------------

#define NONPAGE            0x1FFF // mask page offset
#define PLEN               0x2000 // page length
#define PAGE               0x6000 // mask page
#define AMSK               0x7FFF // mask address range
#define WRAPMEM(x)         ((iar & PAGE) + ((iar + x) & NONPAGE))
#define BRANCHCODE         (opcode & 3)
#define CCFIELD            ((psl & PSL_CC) >> 6)
#define OPERAND            (memory[WRAPMEM(1)])
#define gosub              pushras(); branch(OPERAND & 0x80);
#define zgoto              branch(OPERAND & 0x80);
#define ZERO_BYTES         ;
#define ONE_BYTE           iar = WRAPMEM(1);
#define TWO_BYTES          iar = WRAPMEM(2);
#define THREE_BYTES        iar = WRAPMEM(3);
#define REG_CYCLES         cycles_2650 +=  6;
#define SIX_CYCLES         cycles_2650 +=  6;
#define NINE_CYCLES        cycles_2650 +=  9;
#define TWELVE_CYCLES      cycles_2650 += 12;
#define FIFTEEN_CYCLES     cycles_2650 += 15;
#define EIGHTEEN_CYCLES    cycles_2650 += 18;
#define GET_RR             rr = (opcode & 3); if (rr && (psl & PSL_RS)) rr += 3;

// EXPORTED TABLES--------------------------------------------------------

EXPORT int table_size_2650[256] =
{   1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, //   0.. 15 (LOD)
    3, 3, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, //  16.. 31 (SPSW/RETC/BCTR/BCTA)
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, //  32.. 47 (EOR)
    1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, //  48.. 63 (REDC/RETE/BSTR/BSTA)
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, //  64.. 79 (HALT/AND)
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, //  80.. 95 (RRR/REDE/BRNR/BRNA)
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, //  96..111 (IOR)
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, // 112..127 (REDD/CPSW/PPSW/BSNR/BSNA)
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, // 128..143 (ADD)
    1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, // 144..159 (LPSW/DAR/BCFR/ZBRR/BCFA/BXA)
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, // 160..175 (SUB)
    1, 1, 1, 1, 2, 2, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, // 176..191 (WRTC/TPSW/BSFR/ZBSR/BSFA/BSXA)
    1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, // 192..207 (NOP/STR)
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, // 208..223 (RRL/WRTE/BIRR/BIRA)
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, // 224..239 (COM)
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3  // 240..255 (WRTD/TMI/BDRR/BDRA)
};

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       int                      machine,
                                      malzak_x,
                                      memmap,
                                      psgbase;
IMPORT       UBYTE                    g_bank1[1024],
                                      g_bank2[16],
                                      lb_bank,
                                      malzak_field[16][16],
                                      memory_effects[512];
IMPORT       UWORD                    mirror_r[32768],
                                      mirror_w[32768];
IMPORT       ULONG                    oldcycles;
IMPORT       struct MachineStruct     machines[MACHINES];

// MODULE VARIABLES-------------------------------------------------------

MODULE SWORD  res;
MODULE UBYTE  before, after;
MODULE int    addr;

MODULE const int relative_2650[0x100] =
{     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
     16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
     32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
     48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    -64,-63,-62,-61,-60,-59,-58,-57,-56,-55,-54,-53,-52,-51,-50,-49,
    -48,-47,-46,-45,-44,-43,-42,-41,-40,-39,-38,-37,-36,-35,-34,-33,
    -32,-31,-30,-29,-28,-27,-26,-25,-24,-23,-22,-21,-20,-19,-18,-17,
    -16,-15,-14,-13,-12,-11,-10, -9, -8, -7, -6, -5, -4, -3, -2, -1,
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
     16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
     32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
     48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    -64,-63,-62,-61,-60,-59,-58,-57,-56,-55,-54,-53,-52,-51,-50,-49,
    -48,-47,-46,-45,-44,-43,-42,-41,-40,-39,-38,-37,-36,-35,-34,-33,
    -32,-31,-30,-29,-28,-27,-26,-25,-24,-23,-22,-21,-20,-19,-18,-17,
    -16,-15,-14,-13,-12,-11,-10, -9, -8, -7, -6, -5, -4, -3, -2, -1
};

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT UBYTE  psu,
              psl,
              memflags[32768],
              memory[32768],
              opcode,
              rr, // register (0..6)
              r[7]; // register is a reserved word
EXPORT UWORD  iar,
              mirror_r[32768],
              mirror_w[32768],
              ras[8];
EXPORT ULONG  cycles_2650,
              ea;
EXPORT int    interrupt_2650,
              slice_2650;

// MODULE FUNCTIONS-------------------------------------------------------

MODULE __inline void set_cc(UBYTE n);
MODULE __inline void branch(int indirect);
MODULE void compare(UBYTE first, UBYTE second);
MODULE UBYTE add(UBYTE dest, UBYTE source);
MODULE UBYTE subtract(UBYTE dest, UBYTE source);
MODULE void ABS_EA(void);
MODULE void BRA_EA(void);
MODULE void REL_EA(void);
MODULE void ZERO_EA(void);
MODULE UBYTE cpuread_2650(int address);
MODULE __inline void cpuwrite_2650(int address, UBYTE data);

// CODE-------------------------------------------------------------------

EXPORT void cpu_2650(void)
{   FAST ULONG endcycle;

    // assert(slice_2650 >= 1);

    endcycle = cycles_2650 + slice_2650;
    if (endcycle < cycles_2650)
    {   // cycle counter will overflow, so we need to use the slow method
        while (slice_2650 >= 1)
        {   oldcycles = cycles_2650;
            one_instruction();
            slice_2650 -= (cycles_2650 - oldcycles);
    }   }
    else
    {   // cycle counter will not overflow, so we can use a faster method
        oldcycles = cycles_2650;
        while (cycles_2650 < endcycle)
        {   one_instruction();
        }
        slice_2650 -= (cycles_2650 - oldcycles);
}   }

EXPORT void one_instruction(void)
{   FAST UBYTE nextopcode;

    // iar = mirror_r[iar];
    opcode = memory[iar];

    switch (opcode)
    {
    case 0:                                                 // LODZ r0
        // hopefully this is the same behaviour as on a genuine console
        set_cc(r[0]);
        ONE_BYTE;
        REG_CYCLES;
    acase 1:                                                // LODZ r1
    case 2:                                                 // LODZ r2
    case 3:                                                 // LODZ r3
        GET_RR;
        r[0] = r[rr];
        set_cc(r[0]);
        ONE_BYTE;
        REG_CYCLES;
    acase 4:                                                // LODI,r0
        r[0] = OPERAND;
        set_cc(r[0]);
        TWO_BYTES;
        SIX_CYCLES;
    acase 5:                                                // LODI,r1
    case 6:                                                 // LODI,r2
    case 7:                                                 // LODI,r3
        GET_RR;
        r[rr] = OPERAND;
        set_cc(r[rr]);
        TWO_BYTES;
        SIX_CYCLES;
    acase 8:                                                // LODR,r0
        REL_EA();
        r[0] = cpuread_2650((int) ea);
        set_cc(r[0]);
        TWO_BYTES;
        NINE_CYCLES;
    acase 9:                                                // LODR,r1
    case 10:                                                // LODR,r2
    case 11:                                                // LODR,r3
        REL_EA();
        GET_RR;
        r[rr] = cpuread_2650((int) ea);
        set_cc(r[rr]);
        TWO_BYTES;
        NINE_CYCLES;
    acase 12:                                               // LODA,r0
    //lint -fallthrough
    case 13:                                                // LODA,r1
    case 14:                                                // LODA,r2
    case 15:                                                // LODA,r3
        ABS_EA(); // calls GET_RR for us
        r[rr] = cpuread_2650((int) ea);
        set_cc(r[rr]);
        if (OPERAND & 0x80)
        {   EIGHTEEN_CYCLES;
        } else
        {   TWELVE_CYCLES;
        }
        THREE_BYTES;
    acase 16:                                               // LDPL addr
        if (machine == INTERTON)
        {   THREE_BYTES;
        } else
        {   ONE_BYTE;
        }
        SIX_CYCLES; // guess
    acase 17:                                               // STPL addr
        ONE_BYTE;
        SIX_CYCLES; // guess
    acase 18:                                               // SPSU
        r[0] = psu;
        set_cc(psu);
        ONE_BYTE;
        SIX_CYCLES;
    acase 19:                                               // SPSL
        r[0] = psl;
        set_cc(psl);
        ONE_BYTE;
        SIX_CYCLES;
    acase 20:                                               // RETC,eq
    case 21:                                                // RETC,gt
    case 22:                                                // RETC,lt
        if (BRANCHCODE == CCFIELD)
        {   pullras();
        } else
        {   ONE_BYTE;
        }
        NINE_CYCLES;
    acase 23:                                               // RETC,un
        pullras();
        NINE_CYCLES;
    acase 24:                                               // BCTR,eq
    case 25:                                                // BCTR,gt
    case 26:                                                // BCTR,lt
        if (BRANCHCODE == CCFIELD)
        {   REL_EA();
            ea = mirror_r[ea];
            zgoto;
        } else
        {   TWO_BYTES;
            NINE_CYCLES;
        }
    acase 27:                                               // BCTR,un
        REL_EA();
        ea = mirror_r[ea];
        zgoto;
    acase 28:                                               // BCTA,eq
    case 29:                                                // BCTA,gt
    case 30:                                                // BCTA,lt
        if (BRANCHCODE == CCFIELD)
        {   BRA_EA();
            zgoto;
        } else
        {   THREE_BYTES;
            NINE_CYCLES;
        }
    acase 31:                                               // BCTA,un
        BRA_EA();
        zgoto;
    acase 32:                                               // EORZ r0
        r[0] = 0;
        psl &= ~(PSL_CC); // clear CC bits of PSL (ie. CC = "eq")
        ONE_BYTE;
        REG_CYCLES;
    acase 33:                                               // EORZ r1
    case 34:                                                // EORZ r2
    case 35:                                                // EORZ r3
        GET_RR;
        r[0] ^= r[rr];
        set_cc(r[0]);
        ONE_BYTE;
        REG_CYCLES;
    acase 36:                                               // EORI,r0
        r[0] ^= OPERAND;
        set_cc(r[0]);
        TWO_BYTES;
        SIX_CYCLES;
    acase 37:                                               // EORI,r1
    case 38:                                                // EORI,r2
    case 39:                                                // EORI,r3
        GET_RR;
        r[rr] ^= OPERAND;
        set_cc(r[rr]);
        TWO_BYTES;
        SIX_CYCLES;
    acase 40:                                               // EORR,r0
        REL_EA();
        r[0] ^= cpuread_2650((int) ea);
        set_cc(r[0]);
        TWO_BYTES;
        NINE_CYCLES;
    acase 41:                                               // EORR,r1
    case 42:                                                // EORR,r2
    case 43:                                                // EORR,r3
        REL_EA();
        GET_RR;
        r[rr] ^= cpuread_2650((int) ea);
        set_cc(r[rr]);
        TWO_BYTES;
        NINE_CYCLES;
    acase 44:                                               // EORA,r0
    //lint -fallthrough
    case 45:                                                // EORA,r1
    case 46:                                                // EORA,r2
    case 47:                                                // EORA,r3
        ABS_EA(); // calls GET_RR for us
        r[rr] ^= cpuread_2650((int) ea);
        set_cc(r[rr]);
        if (OPERAND & 0x80)
        {   EIGHTEEN_CYCLES;
        } else
        {   TWELVE_CYCLES;
        }
        THREE_BYTES;
    acase 48:                                               // REDC r0
    case 49:                                                // REDC r1
    case 50:                                                // REDC r2
    case 51:                                                // REDC r3
        GET_RR;
        r[rr] = readport(PORTC);
        set_cc(r[rr]);
        ONE_BYTE;
        SIX_CYCLES;
    acase 52:                                               // RETE,eq
    case 53:                                                // RETE,gt
    case 54:                                                // RETE,lt
        if (BRANCHCODE == CCFIELD)
        {   psu &= ~(PSU_II); // clear II bit (PSU &= %11011111)
            pullras();
            NINE_CYCLES;
            checkinterrupt();
        } else
        {   ONE_BYTE;
            NINE_CYCLES;
        }
    acase 55:                                               // RETE,un
        psu &= ~(PSU_II); // clear II bit (PSU &= %11011111)
        pullras();
        NINE_CYCLES;
        checkinterrupt();
    acase 56:                                               // BSTR,eq
    case 57:                                                // BSTR,gt
    case 58:                                                // BSTR,lt
        if (BRANCHCODE == CCFIELD)
        {   REL_EA();
            ea = mirror_r[ea];
            gosub;
        } else
        {   TWO_BYTES;
            NINE_CYCLES;
        }
    acase 59:                                               // BSTR,un
        REL_EA();
        ea = mirror_r[ea];
        gosub;
    acase 60:                                               // BSTA,eq
    case 61:                                                // BSTA,gt
    case 62:                                                // BSTA,lt
        if (BRANCHCODE == CCFIELD)
        {   BRA_EA();
            gosub;
        } else
        {   THREE_BYTES;
            NINE_CYCLES;
        }
    acase 63:                                               // BSTA,un
        BRA_EA();
        gosub;
    acase 64:                                               // HALT
        ZERO_BYTES;
        REG_CYCLES;
    acase 65:                                               // ANDZ r1
    case 66:                                                // ANDZ r2
    case 67:                                                // ANDZ r3
        GET_RR;
        r[0] &= r[rr];
        set_cc(r[0]);
        ONE_BYTE;
        REG_CYCLES;
    acase 68:                                               // ANDI,r0
        r[0] &= OPERAND;
        set_cc(r[0]);
        TWO_BYTES;
        SIX_CYCLES;
    acase 69:                                               // ANDI,r1
    case 70:                                                // ANDI,r2
    case 71:                                                // ANDI,r3
        GET_RR;
        r[rr] &= OPERAND;
        set_cc(r[rr]);
        TWO_BYTES;
        SIX_CYCLES;
    acase 72:                                               // ANDR,r0
        REL_EA();
        r[0] &= cpuread_2650((int) ea);
        set_cc(r[0]);
        TWO_BYTES;
        NINE_CYCLES;
    acase 73:                                               // ANDR,r1
    case 74:                                                // ANDR,r2
    case 75:                                                // ANDR,r3
        REL_EA();
        GET_RR;
        r[rr] &= cpuread_2650((int) ea);
        set_cc(r[rr]);
        TWO_BYTES;
        NINE_CYCLES;
    acase 76:                                               // ANDA,r0
    //lint -fallthrough
    case 77:                                                // ANDA,r1
    case 78:                                                // ANDA,r2
    case 79:                                                // ANDA,r3
        ABS_EA(); // calls GET_RR for us
        r[rr] &= cpuread_2650((int) ea);
        set_cc(r[rr]);
        if (OPERAND & 0x80)
        {   EIGHTEEN_CYCLES;
        } else
        {   TWELVE_CYCLES;
        }
        THREE_BYTES;
    acase 80:                                               // RRR,r0
    case 81:                                                // RRR,r1
    case 82:                                                // RRR,r2
    case 83:                                                // RRR,r3
        GET_RR;
        before = r[rr];
        after = (before & 0xFE) >> 1;
        if (psl & PSL_WC)
        {   if (psl & PSL_C)
            {   after |= 0x80;
            }
            r[rr] = after;
            psl &= 0x1A; // ccdR,WoMc, Clear CC, IDC, OVF, C; preserve RS, WC, COM
            if (before & 1)
            {   psl |= PSL_C;
            }
            if (after & 0x20)
            {   psl |= PSL_IDC;
            }
            if ((before & 0x80) != (after & 0x80) && before <= 0x7F)
            {   psl |= PSL_OVF;
        }   }
        else
        {   if (before & 1)
            {   after |= 0x80;
            }
            r[rr] = after;
            psl &= 0x3F; // ccDR,WOMC. Clear CC; preserve IDC, RS, WC, OVF, COM, C (OVF and C bit preservation confirmed on real machine)
        }
        if (r[rr] >= 128)
        {   psl |= 0x80; // CC = %10 (LT)
        } elif (r[rr] > 0)
        {   psl |= 0x40; // CC = %01 (GT)
        }
        ONE_BYTE;
        SIX_CYCLES;
    acase 84:                                               // REDE,r0
    case 85:                                                // REDE,r1
    case 86:                                                // REDE,r2
    case 87:                                                // REDE,r3
        GET_RR;
        r[rr] = readport(OPERAND);
        set_cc(r[rr]);
        TWO_BYTES;
        NINE_CYCLES;
    acase 88:                                               // BRNR,r0
    case 89:                                                // BRNR,r1
    case 90:                                                // BRNR,r2
    case 91:                                                // BRNR,r3
        GET_RR;
        if (r[rr])
        {   REL_EA();
            ea = mirror_r[ea];
            zgoto;
        } else
        {   TWO_BYTES;
            NINE_CYCLES;
        }
    acase 92:                                               // BRNA,r0
    case 93:                                                // BRNA,r1
    case 94:                                                // BRNA,r2
    case 95:                                                // BRNA,r3
        GET_RR;
        if (r[rr])
        {   BRA_EA();
            zgoto;
        } else
        {   THREE_BYTES;
            NINE_CYCLES;
        }
    acase 96:                                               // IORZ r0
        // r[0] |= r[0]; is not needed
        set_cc(r[0]);
        ONE_BYTE;
        REG_CYCLES;
    acase 97:                                               // IORZ r1
    case 98:                                                // IORZ r2
    case 99:                                                // IORZ r3
        GET_RR;
        r[0] |= r[rr];
        set_cc(r[0]);
        ONE_BYTE;
        REG_CYCLES;
    acase 100:                                              // IORI,r0
        r[0] |= OPERAND;
        set_cc(r[0]);
        TWO_BYTES;
        SIX_CYCLES;
    acase 101:                                              // IORI,r1
    case 102:                                               // IORI,r2
    case 103:                                               // IORI,r3
        GET_RR;
        r[rr] |= OPERAND;
        set_cc(r[rr]);
        TWO_BYTES;
        SIX_CYCLES;
    acase 104:                                              // IORR,r0
        REL_EA();
        r[0] |= cpuread_2650((int) ea);
        set_cc(r[0]);
        TWO_BYTES;
        NINE_CYCLES;
    acase 105:                                              // IORR,r1
    case 106:                                               // IORR,r2
    case 107:                                               // IORR,r3
        REL_EA();
        GET_RR;
        r[rr] |= cpuread_2650((int) ea);
        set_cc(r[rr]);
        TWO_BYTES;
        NINE_CYCLES;
    acase 108:                                              // IORA,r0
    //lint -fallthrough
    case 109:                                               // IORA,r1
    case 110:                                               // IORA,r2
    case 111:                                               // IORA,r3
        ABS_EA(); // calls GET_RR for us
        r[rr] |= cpuread_2650((int) ea);
        set_cc(r[rr]);
        if (OPERAND & 0x80)
        {   EIGHTEEN_CYCLES;
        } else
        {   TWELVE_CYCLES;
        }
        THREE_BYTES;
    acase 112:                                              // REDD r0
    case 113:                                               // REDD r1
    case 114:                                               // REDD r2
    case 115:                                               // REDD r3
        GET_RR;
        r[rr] = readport(PORTD);
        set_cc(r[rr]);
        ONE_BYTE;
        SIX_CYCLES;
    acase 116:                                              // CPSU
        psu &= (~(OPERAND & PSU_WRITABLE_A));
        TWO_BYTES;
        NINE_CYCLES;
        checkinterrupt();
    acase 117:                                              // CPSL
        psl &= (~OPERAND);
        TWO_BYTES;
        NINE_CYCLES;
    acase 118:                                              // PPSU
        if (!(psu & PSU_II) && (OPERAND & PSU_II))
        {   interrupt_2650 = FALSE; // any pending interrupts are cleared
        }
        psu |= (OPERAND & PSU_WRITABLE_A);
        TWO_BYTES;
        NINE_CYCLES;
        // checkinterrupt() isn't needed as this instruction can only ever inhibit interrupts, not disinhibit them
    acase 119:                                              // PPSL
        psl |= OPERAND;
        TWO_BYTES;
        NINE_CYCLES;
    acase 120:                                              // BSNR,r0
        if (r[0])
        {   REL_EA();
            ea = mirror_r[ea];
            gosub;
        } else
        {   TWO_BYTES;
            NINE_CYCLES;
        }
    acase 121:                                              // BSNR,r1
    case 122:                                               // BSNR,r2
    case 123:                                               // BSNR,r3
        GET_RR;
        if (r[rr])
        {   REL_EA();
            ea = mirror_r[ea];
            gosub;
        } else
        {   TWO_BYTES;
            NINE_CYCLES;
        }
    acase 124:                                              // BSNA,r0
        if (r[0])
        {   BRA_EA();
            gosub;
        } else
        {   THREE_BYTES;
            NINE_CYCLES;
        }
    acase 125:                                              // BSNA,r1
    case 126:                                               // BSNA,r2
    case 127:                                               // BSNA,r3
        GET_RR;
        if (r[rr])
        {   BRA_EA();
            gosub;
        } else
        {   THREE_BYTES;
            NINE_CYCLES;
        }
    acase 128:                                              // ADDZ r0
        r[0] = add(r[0], r[0]);
        ONE_BYTE;
        REG_CYCLES;
    acase 129:                                              // ADDZ r1
    case 130:                                               // ADDZ r2
    case 131:                                               // ADDZ r3
        GET_RR;
        r[0] = add(r[0], r[rr]);
        ONE_BYTE;
        REG_CYCLES;
    acase 132:                                              // ADDI,r0
    case 133:                                               // ADDI,r1
    case 134:                                               // ADDI,r2
    case 135:                                               // ADDI,r3
        GET_RR;
        r[rr] = add(r[rr], OPERAND);
        TWO_BYTES;
        SIX_CYCLES;
    acase 136:                                              // ADDR,r0
        REL_EA();
        r[0] = add(r[0], cpuread_2650((int) ea));
        TWO_BYTES;
        NINE_CYCLES;
    acase 137:                                              // ADDR,r1
    case 138:                                               // ADDR,r2
    case 139:                                               // ADDR,r3
        REL_EA();
        GET_RR;
        r[rr] = add(r[rr], cpuread_2650((int) ea));
        TWO_BYTES;
        NINE_CYCLES;
    acase 140:                                              // ADDA,r0
    //lint -fallthrough
    case 141:                                               // ADDA,r1
    case 142:                                               // ADDA,r2
    case 143:                                               // ADDA,r3
        ABS_EA(); // calls GET_RR for us
        r[rr] = add(r[rr], cpuread_2650((int) ea));
        if (OPERAND & 0x80)
        {   EIGHTEEN_CYCLES;
        } else
        {   TWELVE_CYCLES;
        }
        THREE_BYTES;
    acase 146:                                              // LPSU
        if (!(psu & PSU_II) && (r[0] & PSU_II))
        {   interrupt_2650 = FALSE; // any pending interrupts are cleared
        }
        psu &= 0x80; // clear all except Sense bit
        psu |= (r[0] & PSU_WRITABLE_A);
        ONE_BYTE;
        SIX_CYCLES;
        checkinterrupt();
    acase 147:                                              // LPSL
        psl = r[0];
        ONE_BYTE;
        SIX_CYCLES;
    acase 148:                                              // DAR  r0
        if ((psl & PSL_C) == 0)
        {   // add ten to the tens digit (the high nybble)
            r[0] += 0xA0; // 160. overflow OK. %1010,0000
        }
        if ((psl & PSL_IDC) == 0)
        {   // OR the high nybble with the (low nybble + 10)
            r[0] = (r[0] & 0xF0) | ((r[0] + 0x0A) & 0x0F);
        }
        set_cc(r[0]);
        ONE_BYTE;
        NINE_CYCLES;
    acase 149:                                              // DAR  r1
    case 150:                                               // DAR  r2
    case 151:                                               // DAR  r3
        GET_RR;
        if ((psl & PSL_C) == 0)
        {   // add ten to the tens digit (the high nybble)
            r[rr] += 0xA0; // 160. overflow OK. %1010,0000
        }
        if ((psl & PSL_IDC) == 0)
        {   // OR the high nybble with the (low nybble + 10)
            r[rr] = (r[rr] & 0xF0) | ((r[rr] + 0x0A) & 0x0F);
        }
        set_cc(r[rr]);
        ONE_BYTE;
        NINE_CYCLES;
    acase 152:                                              // BCFR,eq
    case 153:                                               // BCFR,gt
    case 154:                                               // BCFR,lt
        if (BRANCHCODE != CCFIELD)
        {   REL_EA();
            ea = mirror_r[ea];
            zgoto;
        } else
        {   TWO_BYTES;
            NINE_CYCLES;
        }
    acase 155:                                              // ZBRR
        ZERO_EA();
        zgoto;
    acase 156:                                              // BCFA,eq
    case 157:                                               // BCFA,gt
    case 158:                                               // BCFA,lt
        if (BRANCHCODE != CCFIELD)
        {   BRA_EA();
            zgoto;
        } else
        {   THREE_BYTES;
            NINE_CYCLES;
        }
    acase 159:                                              // BXA,r3
        BRA_EA();
        if (psl & PSL_RS)
        {   ea = (ea + r[6]) & AMSK;
        } else
        {   ea = (ea + r[3]) & AMSK;
        }
        zgoto;
    acase 160:                                              // SUBZ r0
        // This is just as small and fast as EORZ r0, so we don't log it as inefficient.
        // Also, its effects can be different (eg. borrow can be applied).
        r[0] = subtract(r[0], r[0]); // not just r[0] = 0; because we need to set PSW bits
        ONE_BYTE;
        REG_CYCLES;
    acase 161:                                              // SUBZ r1
    case 162:                                               // SUBZ r2
    case 163:                                               // SUBZ r3
        GET_RR;
        r[0] = subtract(r[0], r[rr]);
        ONE_BYTE;
        REG_CYCLES;
    acase 164:                                              // SUBI,r0
    case 165:                                               // SUBI,r1
    case 166:                                               // SUBI,r2
    case 167:                                               // SUBI,r3
        GET_RR;
        r[rr] = subtract(r[rr], OPERAND);
        TWO_BYTES;
        SIX_CYCLES;
    acase 168:                                              // SUBR,r0
        REL_EA();
        r[0] = subtract(r[0], cpuread_2650((int) ea));
        TWO_BYTES;
        NINE_CYCLES;
    acase 169:                                              // SUBR,r1
    case 170:                                               // SUBR,r2
    case 171:                                               // SUBR,r3
        REL_EA();
        GET_RR;
        r[rr] = subtract(r[rr], cpuread_2650((int) ea));
        TWO_BYTES;
        NINE_CYCLES;
    acase 172:                                              // SUBA,r0
    //lint -fallthrough
    case 173:                                               // SUBA,r1
    case 174:                                               // SUBA,r2
    case 175:                                               // SUBA,r3
        ABS_EA(); // this calls GET_RR for us
        r[rr] = subtract(r[rr], cpuread_2650((int) ea));
        if (OPERAND & 0x80)
        {   EIGHTEEN_CYCLES;
        } else
        {   TWELVE_CYCLES;
        }
        THREE_BYTES;
    acase 176:                                              // WRTC r0
    case 177:                                               // WRTC r1
    case 178:                                               // WRTC r2
    case 179:                                               // WRTC r3
        GET_RR;
        writeport(PORTC, r[rr]);
        ONE_BYTE;
        SIX_CYCLES;
    acase 180:                                              // TPSU
        psl &= 0x3F; // set CC to %00xxxxxx ("eq")
        if ((psu & OPERAND) < OPERAND)
        {   psl |= 0x80; // set CC to %10xxxxxx ("lt")
        }
        TWO_BYTES;
        NINE_CYCLES;
    acase 181:                                              // TPSL
        psl &= 0x3F; // set CC to %00xxxxxx ("eq")
        if ((psl & OPERAND) < OPERAND)
        {   psl |= 0x80; // set CC to %10xxxxxx ("lt")
        }
        TWO_BYTES;
        NINE_CYCLES;
    acase 184:                                              // BSFR,eq
    case 185:                                               // BSFR,gt
    case 186:                                               // BSFR,lt
        if (BRANCHCODE != CCFIELD)
        {   REL_EA();
            ea = mirror_r[ea];
            gosub;
        } else
        {   TWO_BYTES;
            NINE_CYCLES;
        }
    acase 187:                                              // ZBSR
        ZERO_EA();
        // we are making an assumption that the USE monitor switches to the
        // main register bank when these functions are called; this assumption is
        // probably not true.
        gosub;
    acase 188:                                              // BSFA,eq
    case 189:                                               // BSFA,gt
    case 190:                                               // BSFA,lt
        if (BRANCHCODE != CCFIELD)
        {   BRA_EA();
            gosub;
        } else
        {   THREE_BYTES;
            NINE_CYCLES;
        }
    acase 191:                                              // BSXA,r3
        BRA_EA();
        if (psl & PSL_RS)
        {   ea = (ea + r[6]) & AMSK;
        } else
        {   ea = (ea + r[3]) & AMSK;
        }
        gosub;
    acase 192:                                              // NOP
        ONE_BYTE;
        REG_CYCLES;
    acase 193:                                              // STRZ r1
    case 194:                                               // STRZ r2
    case 195:                                               // STRZ r3
        GET_RR;
        r[rr] = r[0];
        set_cc(r[rr]);
        ONE_BYTE;
        REG_CYCLES;
    acase 200:                                              // STRR,r0
        REL_EA();
        cpuwrite_2650((int) ea, r[0]);
        TWO_BYTES;
        NINE_CYCLES;
    acase 201:                                              // STRR,r1
    case 202:                                               // STRR,r2
    case 203:                                               // STRR,r3
        REL_EA();
        GET_RR;
        cpuwrite_2650((int) ea, r[rr]);
        TWO_BYTES;
        NINE_CYCLES;
    acase 204:                                              // STRA,r0
    case 205:                                               // STRA,r1
    case 206:                                               // STRA,r2
    case 207:                                               // STRA,r3
        ABS_EA(); // this calls GET_RR for us
        cpuwrite_2650((int) ea, r[rr]);
        if (OPERAND & 0x80)
        {   EIGHTEEN_CYCLES;
        } else
        {   TWELVE_CYCLES;
        }
        THREE_BYTES;
    acase 208:                                              // RRL,r0
    case 209:                                               // RRL,r1
    case 210:                                               // RRL,r2
    case 211:                                               // RRL,r3
        GET_RR;
        before = r[rr];
        after = (before & 0x7F) << 1;
        if (psl & PSL_WC)
        {   if (psl & PSL_C)
            {   after |= 1;
            }
            r[rr] = after;
            psl &= 0x1A; // ccdR,WoMc, Clear CC, IDC, OVF, C; preserve RS, WC, COM
            if (before & 0x80)
            {   psl |= PSL_C;
            }
            if (after & 0x20)
            {   psl |= PSL_IDC;
            }
            if ((before & 0x80) != (after & 0x80) && (before <= 0x7F || before >= 0xC0))
            {   psl |= PSL_OVF;
        }   }
        else
        {   if (before & 0x80)
            {   after |= 1;
            }
            r[rr] = after;
            psl &= 0x3F; // ccDRWOMC. Clear CC; preserve IDC, RS, WC, OVF, COM, C (OVF & C bit preservation confirmed on real machine)
        }
        if (r[rr] >= 128)
        {   psl |= 0x80; // CC = %10 (LT)
        } elif (r[rr] > 0)
        {   psl |= 0x40; // CC = %01 (GT)
        }
        ONE_BYTE;
        SIX_CYCLES;
    acase 212:                                              // WRTE r0
    case 213:                                               // WRTE r1
    case 214:                                               // WRTE r2
    case 215:                                               // WRTE r3
        GET_RR;
        writeport(OPERAND, r[rr]);
        TWO_BYTES;
        NINE_CYCLES;
    acase 216:                                              // BIRR,r0
    case 217:                                               // BIRR,r1
    case 218:                                               // BIRR,r2
    case 219:                                               // BIRR,r3
        GET_RR;
        if (++r[rr]) // overflow OK
        {   REL_EA();
            ea = mirror_r[ea];
            zgoto;
        } else
        {   TWO_BYTES;
            NINE_CYCLES;
        }
    acase 220:                                              // BIRA,r0
    case 221:                                               // BIRA,r1
    case 222:                                               // BIRA,r2
    case 223:                                               // BIRA,r3
        GET_RR;
        if (++r[rr]) // overflow OK
        {   BRA_EA();
            zgoto;
        } else
        {   THREE_BYTES;
            NINE_CYCLES;
        }
    acase 224:                                              // COMZ r0
        psl &= ~(PSL_CC); // clear CC bits of PSL
        ONE_BYTE;
        REG_CYCLES;
    acase 225:                                              // COMZ r1
    case 226:                                               // COMZ r2
    case 227:                                               // COMZ r3
        GET_RR;
        compare(r[0], r[rr]);
        ONE_BYTE;
        REG_CYCLES;
    acase 228:                                              // COMI,r0
        compare(r[0], OPERAND);
        TWO_BYTES;
        SIX_CYCLES;
    acase 229:                                              // COMI,r1
    case 230:                                               // COMI,r2
    case 231:                                               // COMI,r3
        GET_RR;
        compare(r[rr], OPERAND);
        TWO_BYTES;
        SIX_CYCLES;
    acase 232:                                              // COMR,r0
        REL_EA();
        compare(r[0], cpuread_2650((int) ea));
        TWO_BYTES;
        NINE_CYCLES;
    acase 233:                                              // COMR,r1
    case 234:                                               // COMR,r2
    case 235:                                               // COMR,r3
        REL_EA();
        GET_RR;
        compare(r[rr], cpuread_2650((int) ea));
        TWO_BYTES;
        NINE_CYCLES;
    acase 236:                                              // COMA,r0
    case 237:                                               // COMA,r1
    case 238:                                               // COMA,r2
    case 239:                                               // COMA,r3
        ABS_EA(); // this calls GET_RR for us
        compare(r[rr], cpuread_2650((int) ea));
        if (OPERAND & 0x80)
        {   EIGHTEEN_CYCLES;
        } else
        {   TWELVE_CYCLES;
        }
        THREE_BYTES;
    acase 240:                                              // WRTD r0
    case 241:                                               // WRTD r1
    case 242:                                               // WRTD r2
    case 243:                                               // WRTD r3
        GET_RR;
        writeport(PORTD, r[rr]);
        ONE_BYTE;
        SIX_CYCLES;
    acase 244:                                              // TMI,r0
        psl &= 0x3F; // set CC to %00 ("eq")
        if ((r[0] & OPERAND) < OPERAND)
        {   psl |= 0x80; // set CC to %10 ("lt")
        }
        TWO_BYTES;
        NINE_CYCLES;
    acase 245:                                              // TMI,r1
    case 246:                                               // TMI,r2
    case 247:                                               // TMI,r3
        GET_RR;
        psl &= 0x3F; // set CC to %00 ("eq")
        if ((r[rr] & OPERAND) < OPERAND)
        {   psl |= 0x80; // set CC to %10 ("lt")
        }
        TWO_BYTES;
        NINE_CYCLES;
    acase 248:                                              // BDRR,r0
    case 249:                                               // BDRR,r1
    case 250:                                               // BDRR,r2
    case 251:                                               // BDRR,r3
        GET_RR;
        if (--r[rr]) // underflow OK
        {   REL_EA();
            ea = mirror_r[ea];
            zgoto;
        } else
        {   TWO_BYTES;
            NINE_CYCLES;
        }
    acase 252:                                              // BDRA,r0
    case 253:                                               // BDRA,r1
    case 254:                                               // BDRA,r2
    case 255:                                               // BDRA,r3
        GET_RR;
        if (--r[rr]) // underflow OK
        {   BRA_EA();
            zgoto;
        } else
        {   THREE_BYTES;
            NINE_CYCLES;
        }
    acase 0x90:
    case 0x91:
        ONE_BYTE;
        SIX_CYCLES;
    acase 0xB6:
    case 0xB7:
    case 0xC4:
    case 0xC5:
    case 0xC6:
    case 0xC7:
        TWO_BYTES;
        SIX_CYCLES;
}   }

// SUPPORT FUNCTIONS------------------------------------------------------

MODULE __inline void set_cc(UBYTE n) // set CC
{   psl &= ~(PSL_CC); // clear CC bits of PSL

    if (n >= 128)
    {   psl |= 0x80; // CC = %10
    } elif (n > 0)
    {   psl |= 0x40; // CC = %01
}   }

EXPORT void pushras(void)
{   FAST int i,
             sp;

    // store address
    ras[psu & PSU_SP] = (iar & PAGE) + ((iar + table_size_2650[opcode]) & NONPAGE);

    // increment SP
    if ((psu & PSU_SP) == 7) // overflow
    {   psu &= ~(PSU_SP); // set SP to 0
    } else
    {   psu++;
}   }

EXPORT void pullras(void)
{   FAST int i,
             sp;

    // decrement SP
    if ((psu & PSU_SP) == 0) // underflow
    {   psu |= PSU_SP; // set SP to 7
    } else
    {   psu--;
    }

    iar = ras[psu & PSU_SP];
}

MODULE __inline void branch(int indirect)
{   /* page 0 is $0000..$1FFF
       page 1 is $2000..$3FFF
       page 2 is $4000..$5FFF
       page 3 is $6000..$7FFF
    ippaaaaa aaaaaaaa
    i = indirect flag
    p = page
    a = address on page */

    iar = (UWORD) ea;
    if (indirect)
    {   FIFTEEN_CYCLES;
    } else
    {   NINE_CYCLES;
}   }

MODULE void compare(UBYTE first, UBYTE second)
{   int d;

    psl &= 0x3F; // clear CC
    if (psl & PSL_COM) // unsigned compare
    {   d = (UBYTE) first - (UBYTE) second;
    } else
    {   d = (SBYTE) first - (SBYTE) second;
    }
    if (d < 0 )
    {   psl |= 0x80; // %10 = lt
    } elif (d > 0)
    {   psl |= 0x40; // %01 = gt
}   }

// Add source to destination
MODULE UBYTE add(UBYTE dest, UBYTE source)
{   FAST UBYTE dest_nybble;
    FAST int   dest_int;

    before      =   dest;
    dest_int    =   (int) before + (int) source;
    dest_nybble =  (before & 0x0F) + (source & 0x0F); // "overflow" (>= 16) is OK
    dest        +=  source; // overflow is OK
    if ((psl & PSL_WC) && (psl & PSL_C))
    {   dest++; // overflow is OK
        dest_nybble++; // "overflow" (>= 16) is OK
        dest_int++;
    }
    psl &= 0x1A; // ccdR,WoMc. Clear CC, IDC, OVF, C; preserve RS, WC, COM (OVF behaviour confirmed on real machine)
    if (dest_int >= 256)
    {   psl |= PSL_C; // means a carry was generated
    }
    if (dest_nybble >= 16)
    {   psl |= PSL_IDC;
    }
    if ((before & 0x80) == (source & 0x80) && (before & 0x80) != (dest & 0x80))
    {   psl |= PSL_OVF;
    }
    if (dest >= 128)
    {   psl |= 0x80; // CC = %10 (LT)
    } elif (dest > 0)
    {   psl |= 0x40; // CC = %01 (GT)
    }

    return dest;
}

// Subtract source (subtrahend) from destination (minuend)
MODULE UBYTE subtract(UBYTE dest, UBYTE source)
{   FAST SBYTE dest_nybble;
    FAST int   dest_int;

    before      =   dest;
    dest_int    =   (int) before - (int) source;
    dest_nybble =  (before & 0x0F) - (source & 0x0F);
    dest        -= source; // underflow is OK
    if ((psl & PSL_WC) && !(psl & PSL_C))
    {   dest--; // underflow is OK
        dest_nybble--;
        dest_int--;
    }
    psl &= 0x1A; // ccdR,WoMc, Clear CC, IDC, OVF, C; preserve RS, WC, COM (OVF behaviour confirmed on real machine)
    if (dest_int >= 0)
    {   psl |= PSL_C;   // means a borrow was *not* generated
    }
    if (dest_nybble >= 0)
    {   psl |= PSL_IDC; // means a borrow was *not* generated
    }
    if ((before & 0x80) != (source & 0x80) && (before & 0x80) != (dest & 0x80))
    {   psl |= PSL_OVF;
    }
    if (dest >= 128)
    {   psl |= 0x80; // CC = %10
    } elif (dest > 0)
    {   psl |= 0x40; // CC = %01
    }

    return dest;
}

/* ABS_EA(), etc. don't use cpuread_2650(), because they simply calculate an
   address (ie. a number), they don't actually look at the contents of
   that address. */

// ** build effective address with absolute addressing (non-branch) **
// called by LODA, EORA, ANDA, IORA, SUBA, ADDA, STRA, COMA
// ABS_EA() uses GET_RR; the others don't!
MODULE void ABS_EA(void)
{   /* build effective address within current 8K page */

    OPERAND = memory[WRAPMEM(1)];
    ea = (iar & PAGE) + (((OPERAND << 8) + memory[WRAPMEM(2)]) & NONPAGE);
    GET_RR;

    if (OPERAND & 0x80) /* indirect addressing? */
    {   addr = (int) mirror_r[ea]; // needed for eg. Interton BOWLING, etc.
        /* build indirect 32K address */
        ea = memory[addr] << 8;
        if ((++addr & NONPAGE) == 0)
        {   addr -= PLEN; /* page wrap */
        }
        ea = (ea + memory[addr]) & AMSK;
    }

    /* check indexed addressing modes */
    switch (OPERAND & 0x60)
    {
    case 0x20: /* auto increment indexed */
        r[rr]++;
        ea = (ea & PAGE) + ((ea + r[rr]) & NONPAGE);
        rr = 0; /* absolute addressing reg is R0 */
    acase 0x40: /* auto decrement indexed */
        r[rr]--;
        ea = (ea & PAGE) + ((ea + r[rr]) & NONPAGE);
        rr = 0; /* absolute addressing reg is R0 */
    acase 0x60: /* indexed */
        ea = (ea & PAGE) + ((ea + r[rr]) & NONPAGE);
        rr = 0; /* absolute addressing reg is R0 */
 // adefault: // eg. $00 (not indexed)
}   }

// ** build effective address with absolute addressing (branch) **
// called by BCTA, BRNA, BCFA, BIRA, BDRA, BXA, BSXA, BSTA, BSNA, BSFA
MODULE void BRA_EA(void)
{   /* build address in 32K address space */

    OPERAND = memory[WRAPMEM(1)];
    ea = ((OPERAND << 8) + memory[WRAPMEM(2)]) & AMSK;
    ea = mirror_r[ea]; // all branches are considered to be reads

    if (OPERAND & 0x80) // indirect addressing?
    {   addr = (int) ea;
        /* build indirect 32K address */
        ea = memory[addr] << 8;

        if ((++addr & NONPAGE) == 0)
        {   addr -= PLEN; /* page wrap */
        }

        ea = (ea + memory[addr]) & AMSK;
        ea = mirror_r[ea]; // needed for eg. Spider's Web
}   }

// ** build effective add ress with relative addressing **
// called by LODR, BCTR, BSTR, EORR, ANDR, BRNR, IORR, BSNR, ADDR,
// BCFR, SUBR, BSFR, STRR, BIRR, COMR, BDRR
MODULE void REL_EA(void)
{   /* build effective address within current 8K page */

    OPERAND = memory[WRAPMEM(1)];
    ea = (iar & PAGE) + ((iar + 2 + relative_2650[OPERAND]) & NONPAGE);

/* Mirroring is not done here.
For relative branch instructions, mirroring is handled by the calling
routine (after calling this routine).
  For relative non-branch instructions, mirroring is handled by cpuread_2650().
*/

    if (OPERAND & 0x80) /* indirect bit set? */
    {   addr = (int) ea;
        /* build indirect 32K address */
        ea = memory[addr] << 8;
        if ((++addr & NONPAGE) == 0)
        {   addr -= PLEN; /* page wrap */
        }
        ea = (ea + memory[addr]) & AMSK;
}   }

// ** build effective address with zero relative addressing **
// called by ZBRR, ZBSR
MODULE void ZERO_EA(void)
{   /* build effective address from 0 */

    OPERAND = memory[WRAPMEM(1)];
    ea = (relative_2650[OPERAND] & NONPAGE);
    // ea = mirror_r[ea]; // all branches are considered to be reads
    // not really necessary, as there is no mirroring done of $0000..$007F
    // nor $7F80..$7FFF.

    if (OPERAND & 0x80) // indirect bit set?
    {   addr = (int) ea;
        /* build indirect 32K address */
        ea = memory[addr] << 8;
        if ((++addr & NONPAGE) == 0)
        {   addr -= PLEN; /* page wrap */
        }
        ea = (ea + memory[addr]) & AMSK;
}   }

EXPORT void checkinterrupt(void)
{   if (interrupt_2650 && !(psu & PSU_II))
    {   psu |= PSU_II; // set II bit (PSU |= %00100000)
        ras[psu & PSU_SP] = iar;
        switch (machine)
        {
        case INTERTON:
        case ELEKTOR:
        case MALZAK:
            ea = 0x0003;
        acase ZACCARIA:
            if (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA)
            {   ea = 0x0003;
            } else
            {   // assert(memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN);
                ea = 0x000A;
        }   }

        // increment SP
        if ((psu & PSU_SP) == 7) // overflow
        {   psu &= ~(PSU_SP); // set SP to 0
        } else
        {   psu++;
        }
        branch(FALSE);
        interrupt_2650 = FALSE;
}   }

MODULE UBYTE cpuread_2650(int address)
{   FAST FLAG  printed;
    FAST UBYTE t;

    // assert(address <= 32767);

    address = (int) mirror_r[address];

    if (memflags[address] & SPECIALREAD)
    {   if (memflags[address] & READONCE)
        {   t = memory[address];
            memory[address] = machines[machine].readonce;
        } elif (memflags[address] & NOREAD)
        /* It would be better to make a memread[32768] array, containing what
           values each read-only address returns. (Setting memory[] as
           appropriate wouldn't work, because of write-only registers.) */
        {   switch (machine)
            {
            case ARCADIA:
                t = 0xFF; // some really return eg. $40, $C0 instead of $FF
            acase INTERTON:
                // GRNDPRIX attempts to read from $1F6E..$1F70!
                // not sure whether NOISE ($1E80) is R/W or -/W! (assuming R/W)
                if (address == IE_NOISE)
                {   t = 0x0C; // assuming same behaviour as Elektor
                } elif
                (   // It would probably be better to handle these in setmemmap().
                    (address >= 0x1F30 && address <= 0x1F3F)
                 || (address >= 0x1F6E && address <= 0x1F7F)
                 || (address >= 0x1FAE && address <= 0x1FBF) // this region is $00 for Elektor, so perhaps also for Interton?
                )
                {   t = 0xFF; // same as how MESS V2 initializes these regions
                } else
                {   t = 0x00;
                    /* this is assumed rather than known behaviour, but:
                    Interton BLACKJAC expects the high nybble (at least) of $1E8F to be $0! */
                }
            acase ELEKTOR:
                if (address >= 0x1000 && address <= 0x15FF)
                {   // assert(memmap == MEMMAP_E);
                    t = 0xFF; // games which attempt to sense whether in mode "E"/"F" expect unavailable memory to return $FF
                } elif (address >= 0x1D80 && address <= 0x1DBE)
                {   t = 0x80;
                    // TESTER2.PGM proves that $1D80 reads as $80.
                } elif (address >= 0x1DC0 && address <= 0x1DFF)
                {   t = 0xC0;
                    // TESTER2.PGM proves that $1DC0 reads as $C0.
                } elif (address == IE_NOISE)
                {   t = 0x0C;
                    // TESTER2.PGM proves that NOISE reads as $0C.
                } elif
                (   (address >= 0x1E8F && address <= 0x1E97)
                 || (address >= 0x1E9C && address <= 0x1EA7)
                 || (address >= 0x1EAF && address <= 0x1EB7)
                 || (address >= 0x1EBC && address <= 0x1EC7)
                 || (address >= 0x1ECF && address <= 0x1ED7)
                 || (address >= 0x1EDC && address <= 0x1EE7)
                 || (address >= 0x1EEF && address <= 0x1EF7)
                 || (address >= 0x1EFC && address <= 0x1EFF)
                )
                {   t = 0x0F;
                    // TESTER2.PGM proves that $1E93 reads as $0F.
                } else
                {   // eg. $1FAE..$1FBF, $1FC0..$1FC9
                    // Elektor Labyrinth reads $1FB3..$1FB5 and expects them to be $00.
                    // Also, TESTER2.PGM proves that $1FB8 reads as $00.
                    // Elektor CarRace works best when SPRITES23CTRL reads as $00.
                    t = 0;
        }   }   }
        else
        {   // assert(  memflags & RANDOM   );
            // assert(!(memflags & READONCE));
            // assert(memmap == MEMMAP_F);
            // assert(address == E_RANDOM1E || address == E_RANDOM1G || address == E_RANDOM2);
            t = rand() % 256;
    }   }
    elif ((memflags[address] & BANKED) && !(psu & PSU_F))
    {   if (          address >= 0x1800 && address <= 0x1BFF)
        {   t = g_bank1[address - 0x1800];
        } elif (memmap == MEMMAP_ASTROWARS)
        {   // assert(address >= 0x1C00 && address <= 0x1CFF);
            t = g_bank2[(address - 0x1C00) & 0xF];
        } else
        {   // assert(memmap == MEMMAP_GALAXIA);
            // assert(address >= 0x1400 && address <= 0x14FF);
            t = g_bank2[(address - 0x1400) & 0xF]; // the game doesn't actually seem to ever do this
    }   }
    else
    {   t = memory[address];
    }

    return t;
}

MODULE __inline void cpuwrite_2650(int address, UBYTE data)
{   // assert(address <= 32767);

    address = (int) mirror_w[address];

    if (!(memflags[address] & NOWRITE))
    {   if (memflags[address] & BANKED) // Astro Wars, Galaxia, Laser Battle and Lazarian use this
        {   if (memmap == MEMMAP_LASERBATTLE || memmap == MEMMAP_LAZARIAN)
            {   // assert(address >= 0x1800);
                // assert(address <= 0x1BFF);
                if (lb_bank == 1)
                {   memory_effects[(address - 0x1800) & 0x1FF] = data;
                } else
                {   memory[address] = data;
            }   }
            elif (psu & PSU_F)
            {   if (memmap == MEMMAP_ASTROWARS && address >= 0x1C00 && address <= 0x1CFF)
                {   g_bank2[(address - 0x1C00) & 0xF] = data;
                } else
                {   memory[address] = data;
            }   }
            else
            {   if (address >= 0x1800 && address <= 0x1BFF)
                {   g_bank1[address - 0x1800] = data;
                } else
                {   memory[address] = data;
        }   }   }
        else
        {   memory[address] = data;
            if (memflags[address] & AUDIBLE)
            {   playsound(FALSE);
            } elif (memflags[address] & FIELD)
            {   malzak_field[malzak_x / 16][(address - 0x1600) / 16] = data;
}   }   }   }
