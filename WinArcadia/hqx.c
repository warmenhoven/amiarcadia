// INCLUDES---------------------------------------------------------------

#include <stdio.h>

#include "ibm.h"
#include "aa.h"

// DEFINES----------------------------------------------------------------

#define MASK_2     0x0000FF00
#define MASK_13    0x00FF00FF
#define MASK_RGB   0x00FFFFFF
#define MASK_ALPHA 0xFF000000
#define Ymask      0x00FF0000
#define Umask      0x0000FF00
#define Vmask      0x000000FF
#define trY        0x00300000
#define trU        0x00000700
#define trV        0x00000006

// HQ2x

#define PIXEL00_0     d1 = w5;
#define PIXEL00_10    d1 = Interp1(w5, w1);
#define PIXEL00_11    d1 = Interp1(w5, w4);
#define PIXEL00_12    d1 = Interp1(w5, w2);
#define PIXEL00_20    d1 = Interp2(w5, w4, w2);
#define PIXEL00_21    d1 = Interp2(w5, w1, w2);
#define PIXEL00_22    d1 = Interp2(w5, w1, w4);
#define PIXEL00_60    d1 = Interp6(w5, w2, w4);
#define PIXEL00_61    d1 = Interp6(w5, w4, w2);
#define PIXEL00_70    d1 = Interp7(w5, w4, w2);
#define PIXEL00_90    d1 = Interp9(w5, w4, w2);
#define PIXEL00_100   d1 = Interp10(w5, w4, w2);
#define PIXEL01_0     d2 = w5;
#define PIXEL01_10    d2 = Interp1(w5, w3);
#define PIXEL01_11    d2 = Interp1(w5, w2);
#define PIXEL01_12    d2 = Interp1(w5, w6);
#define PIXEL01_20    d2 = Interp2(w5, w2, w6);
#define PIXEL01_21    d2 = Interp2(w5, w3, w6);
#define PIXEL01_22    d2 = Interp2(w5, w3, w2);
#define PIXEL01_60    d2 = Interp6(w5, w6, w2);
#define PIXEL01_61    d2 = Interp6(w5, w2, w6);
#define PIXEL01_70    d2 = Interp7(w5, w2, w6);
#define PIXEL01_90    d2 = Interp9(w5, w2, w6);
#define PIXEL01_100   d2 = Interp10(w5, w2, w6);
#define PIXEL10_0     d3 = w5;
#define PIXEL10_10    d3 = Interp1(w5, w7);
#define PIXEL10_11    d3 = Interp1(w5, w8);
#define PIXEL10_12    d3 = Interp1(w5, w4);
#define PIXEL10_20    d3 = Interp2(w5, w8, w4);
#define PIXEL10_21    d3 = Interp2(w5, w7, w4);
#define PIXEL10_22    d3 = Interp2(w5, w7, w8);
#define PIXEL10_60    d3 = Interp6(w5, w4, w8);
#define PIXEL10_61    d3 = Interp6(w5, w8, w4);
#define PIXEL10_70    d3 = Interp7(w5, w8, w4);
#define PIXEL10_90    d3 = Interp9(w5, w8, w4);
#define PIXEL10_100   d3 = Interp10(w5, w8, w4);
#define PIXEL11_0     d4 = w5;
#define PIXEL11_10    d4 = Interp1(w5, w9);
#define PIXEL11_11    d4 = Interp1(w5, w6);
#define PIXEL11_12    d4 = Interp1(w5, w8);
#define PIXEL11_20    d4 = Interp2(w5, w6, w8);
#define PIXEL11_21    d4 = Interp2(w5, w9, w8);
#define PIXEL11_22    d4 = Interp2(w5, w9, w6);
#define PIXEL11_60    d4 = Interp6(w5, w8, w6);
#define PIXEL11_61    d4 = Interp6(w5, w6, w8);
#define PIXEL11_70    d4 = Interp7(w5, w6, w8);
#define PIXEL11_90    d4 = Interp9(w5, w6, w8);
#define PIXEL11_100   d4 = Interp10(w5, w6, w8);

// HQ3x

#define PIXEL00_1M  d1 = Interp1(w5, w1);
#define PIXEL00_1U  d1 = Interp1(w5, w2);
#define PIXEL00_1L  d1 = Interp1(w5, w4);
#define PIXEL00_2   d1 = Interp2(w5, w4, w2);
#define PIXEL00_4   d1 = Interp4(w5, w4, w2);
#define PIXEL00_5   d1 = Interp5(w4, w2);
#define PIXEL00_C   d1 = w5;

#define PIXEL01_1   d2 = Interp1(w5, w2);
#define PIXEL01_3   d2 = Interp3(w5, w2);
#define PIXEL01_6   d2 = Interp1(w2, w5);
#define PIXEL01_C   d2 = w5;

#define PIXEL02_1M  d3 = Interp1(w5, w3);
#define PIXEL02_1U  d3 = Interp1(w5, w2);
#define PIXEL02_1R  d3 = Interp1(w5, w6);
#define PIXEL02_2   d3 = Interp2(w5, w2, w6);
#define PIXEL02_4   d3 = Interp4(w5, w2, w6);
#define PIXEL02_5   d3 = Interp5(w2, w6);
#define PIXEL02_C   d3 = w5;

#define PIXEL10_1   d4 = Interp1(w5, w4);
#define PIXEL10_3   d4 = Interp3(w5, w4);
#define PIXEL10_6   d4 = Interp1(w4, w5);
#define PIXEL10_C   d4 = w5;

#define PIXEL11     d5 = w5;

#define PIXEL12_1   d6 = Interp1(w5, w6);
#define PIXEL12_3   d6 = Interp3(w5, w6);
#define PIXEL12_6   d6 = Interp1(w6, w5);
#define PIXEL12_C   d6 = w5;

#define PIXEL20_1M  d7 = Interp1(w5, w7);
#define PIXEL20_1D  d7 = Interp1(w5, w8);
#define PIXEL20_1L  d7 = Interp1(w5, w4);
#define PIXEL20_2   d7 = Interp2(w5, w8, w4);
#define PIXEL20_4   d7 = Interp4(w5, w8, w4);
#define PIXEL20_5   d7 = Interp5(w8, w4);
#define PIXEL20_C   d7 = w5;

#define PIXEL21_1   d8 = Interp1(w5, w8);
#define PIXEL21_3   d8 = Interp3(w5, w8);
#define PIXEL21_6   d8 = Interp1(w8, w5);
#define PIXEL21_C   d8 = w5;

#define PIXEL22_1M  d9 = Interp1(w5, w9);
#define PIXEL22_1D  d9 = Interp1(w5, w8);
#define PIXEL22_1R  d9 = Interp1(w5, w6);
#define PIXEL22_2   d9 = Interp2(w5, w6, w8);
#define PIXEL22_4   d9 = Interp4(w5, w6, w8);
#define PIXEL22_5   d9 = Interp5(w6, w8);
#define PIXEL22_C   d9 = w5;

// HQ4x

#define HQ4X_PIXEL00_0     d1 = w5;
#define HQ4X_PIXEL00_11    d1 = Interp1(w5, w4);
#define HQ4X_PIXEL00_12    d1 = Interp1(w5, w2);
#define HQ4X_PIXEL00_20    d1 = Interp2(w5, w2, w4);
#define HQ4X_PIXEL00_50    d1 = Interp5(w2, w4);
#define HQ4X_PIXEL00_80    d1 = Interp8(w5, w1);
#define HQ4X_PIXEL00_81    d1 = Interp8(w5, w4);
#define HQ4X_PIXEL00_82    d1 = Interp8(w5, w2);
#define HQ4X_PIXEL01_0     d2 = w5;
#define HQ4X_PIXEL01_10    d2 = Interp1(w5, w1);
#define HQ4X_PIXEL01_12    d2 = Interp1(w5, w2);
#define HQ4X_PIXEL01_14    d2 = Interp1(w2, w5);
#define HQ4X_PIXEL01_21    d2 = Interp2(w2, w5, w4);
#define HQ4X_PIXEL01_31    d2 = Interp3(w5, w4);
#define HQ4X_PIXEL01_50    d2 = Interp5(w2, w5);
#define HQ4X_PIXEL01_60    d2 = Interp6(w5, w2, w4);
#define HQ4X_PIXEL01_61    d2 = Interp6(w5, w2, w1);
#define HQ4X_PIXEL01_82    d2 = Interp8(w5, w2);
#define HQ4X_PIXEL01_83    d2 = Interp8(w2, w4);
#define HQ4X_PIXEL02_0     d3 = w5;
#define HQ4X_PIXEL02_10    d3 = Interp1(w5, w3);
#define HQ4X_PIXEL02_11    d3 = Interp1(w5, w2);
#define HQ4X_PIXEL02_13    d3 = Interp1(w2, w5);
#define HQ4X_PIXEL02_21    d3 = Interp2(w2, w5, w6);
#define HQ4X_PIXEL02_32    d3 = Interp3(w5, w6);
#define HQ4X_PIXEL02_50    d3 = Interp5(w2, w5);
#define HQ4X_PIXEL02_60    d3 = Interp6(w5, w2, w6);
#define HQ4X_PIXEL02_61    d3 = Interp6(w5, w2, w3);
#define HQ4X_PIXEL02_81    d3 = Interp8(w5, w2);
#define HQ4X_PIXEL02_83    d3 = Interp8(w2, w6);
#define HQ4X_PIXEL03_0     d4 = w5;
#define HQ4X_PIXEL03_11    d4 = Interp1(w5, w2);
#define HQ4X_PIXEL03_12    d4 = Interp1(w5, w6);
#define HQ4X_PIXEL03_20    d4 = Interp2(w5, w2, w6);
#define HQ4X_PIXEL03_50    d4 = Interp5(w2, w6);
#define HQ4X_PIXEL03_80    d4 = Interp8(w5, w3);
#define HQ4X_PIXEL03_81    d4 = Interp8(w5, w2);
#define HQ4X_PIXEL03_82    d4 = Interp8(w5, w6);
#define HQ4X_PIXEL10_0     d5 = w5;
#define HQ4X_PIXEL10_10    d5 = Interp1(w5, w1);
#define HQ4X_PIXEL10_11    d5 = Interp1(w5, w4);
#define HQ4X_PIXEL10_13    d5 = Interp1(w4, w5);
#define HQ4X_PIXEL10_21    d5 = Interp2(w4, w5, w2);
#define HQ4X_PIXEL10_32    d5 = Interp3(w5, w2);
#define HQ4X_PIXEL10_50    d5 = Interp5(w4, w5);
#define HQ4X_PIXEL10_60    d5 = Interp6(w5, w4, w2);
#define HQ4X_PIXEL10_61    d5 = Interp6(w5, w4, w1);
#define HQ4X_PIXEL10_81    d5 = Interp8(w5, w4);
#define HQ4X_PIXEL10_83    d5 = Interp8(w4, w2);
#define HQ4X_PIXEL11_0     d6 = w5;
#define HQ4X_PIXEL11_30    d6 = Interp3(w5, w1);
#define HQ4X_PIXEL11_31    d6 = Interp3(w5, w4);
#define HQ4X_PIXEL11_32    d6 = Interp3(w5, w2);
#define HQ4X_PIXEL11_70    d6 = Interp7(w5, w4, w2);
#define HQ4X_PIXEL12_0     d7 = w5;
#define HQ4X_PIXEL12_30    d7 = Interp3(w5, w3);
#define HQ4X_PIXEL12_31    d7 = Interp3(w5, w2);
#define HQ4X_PIXEL12_32    d7 = Interp3(w5, w6);
#define HQ4X_PIXEL12_70    d7 = Interp7(w5, w6, w2);
#define HQ4X_PIXEL13_0     d8 = w5;
#define HQ4X_PIXEL13_10    d8 = Interp1(w5, w3);
#define HQ4X_PIXEL13_12    d8 = Interp1(w5, w6);
#define HQ4X_PIXEL13_14    d8 = Interp1(w6, w5);
#define HQ4X_PIXEL13_21    d8 = Interp2(w6, w5, w2);
#define HQ4X_PIXEL13_31    d8 = Interp3(w5, w2);
#define HQ4X_PIXEL13_50    d8 = Interp5(w6, w5);
#define HQ4X_PIXEL13_60    d8 = Interp6(w5, w6, w2);
#define HQ4X_PIXEL13_61    d8 = Interp6(w5, w6, w3);
#define HQ4X_PIXEL13_82    d8 = Interp8(w5, w6);
#define HQ4X_PIXEL13_83    d8 = Interp8(w6, w2);
#define HQ4X_PIXEL20_0     d9 = w5;
#define HQ4X_PIXEL20_10    d9 = Interp1(w5, w7);
#define HQ4X_PIXEL20_12    d9 = Interp1(w5, w4);
#define HQ4X_PIXEL20_14    d9 = Interp1(w4, w5);
#define HQ4X_PIXEL20_21    d9 = Interp2(w4, w5, w8);
#define HQ4X_PIXEL20_31    d9 = Interp3(w5, w8);
#define HQ4X_PIXEL20_50    d9 = Interp5(w4, w5);
#define HQ4X_PIXEL20_60    d9 = Interp6(w5, w4, w8);
#define HQ4X_PIXEL20_61    d9 = Interp6(w5, w4, w7);
#define HQ4X_PIXEL20_82    d9 = Interp8(w5, w4);
#define HQ4X_PIXEL20_83    d9 = Interp8(w4, w8);
#define HQ4X_PIXEL21_0     d10 = w5;
#define HQ4X_PIXEL21_30    d10 = Interp3(w5, w7);
#define HQ4X_PIXEL21_31    d10 = Interp3(w5, w8);
#define HQ4X_PIXEL21_32    d10 = Interp3(w5, w4);
#define HQ4X_PIXEL21_70    d10 = Interp7(w5, w4, w8);
#define HQ4X_PIXEL22_0     d11 = w5;
#define HQ4X_PIXEL22_30    d11 = Interp3(w5, w9);
#define HQ4X_PIXEL22_31    d11 = Interp3(w5, w6);
#define HQ4X_PIXEL22_32    d11 = Interp3(w5, w8);
#define HQ4X_PIXEL22_70    d11 = Interp7(w5, w6, w8);
#define HQ4X_PIXEL23_0     d12 = w5;
#define HQ4X_PIXEL23_10    d12 = Interp1(w5, w9);
#define HQ4X_PIXEL23_11    d12 = Interp1(w5, w6);
#define HQ4X_PIXEL23_13    d12 = Interp1(w6, w5);
#define HQ4X_PIXEL23_21    d12 = Interp2(w6, w5, w8);
#define HQ4X_PIXEL23_32    d12 = Interp3(w5, w8);
#define HQ4X_PIXEL23_50    d12 = Interp5(w6, w5);
#define HQ4X_PIXEL23_60    d12 = Interp6(w5, w6, w8);
#define HQ4X_PIXEL23_61    d12 = Interp6(w5, w6, w9);
#define HQ4X_PIXEL23_81    d12 = Interp8(w5, w6);
#define HQ4X_PIXEL23_83    d12 = Interp8(w6, w8);
#define HQ4X_PIXEL30_0     d13 = w5;
#define HQ4X_PIXEL30_11    d13 = Interp1(w5, w8);
#define HQ4X_PIXEL30_12    d13 = Interp1(w5, w4);
#define HQ4X_PIXEL30_20    d13 = Interp2(w5, w8, w4);
#define HQ4X_PIXEL30_50    d13 = Interp5(w8, w4);
#define HQ4X_PIXEL30_80    d13 = Interp8(w5, w7);
#define HQ4X_PIXEL30_81    d13 = Interp8(w5, w8);
#define HQ4X_PIXEL30_82    d13 = Interp8(w5, w4);
#define HQ4X_PIXEL31_0     d14 = w5;
#define HQ4X_PIXEL31_10    d14 = Interp1(w5, w7);
#define HQ4X_PIXEL31_11    d14 = Interp1(w5, w8);
#define HQ4X_PIXEL31_13    d14 = Interp1(w8, w5);
#define HQ4X_PIXEL31_21    d14 = Interp2(w8, w5, w4);
#define HQ4X_PIXEL31_32    d14 = Interp3(w5, w4);
#define HQ4X_PIXEL31_50    d14 = Interp5(w8, w5);
#define HQ4X_PIXEL31_60    d14 = Interp6(w5, w8, w4);
#define HQ4X_PIXEL31_61    d14 = Interp6(w5, w8, w7);
#define HQ4X_PIXEL31_81    d14 = Interp8(w5, w8);
#define HQ4X_PIXEL31_83    d14 = Interp8(w8, w4);
#define HQ4X_PIXEL32_0     d15 = w5;
#define HQ4X_PIXEL32_10    d15 = Interp1(w5, w9);
#define HQ4X_PIXEL32_12    d15 = Interp1(w5, w8);
#define HQ4X_PIXEL32_14    d15 = Interp1(w8, w5);
#define HQ4X_PIXEL32_21    d15 = Interp2(w8, w5, w6);
#define HQ4X_PIXEL32_31    d15 = Interp3(w5, w6);
#define HQ4X_PIXEL32_50    d15 = Interp5(w8, w5);
#define HQ4X_PIXEL32_60    d15 = Interp6(w5, w8, w6);
#define HQ4X_PIXEL32_61    d15 = Interp6(w5, w8, w9);
#define HQ4X_PIXEL32_82    d15 = Interp8(w5, w8);
#define HQ4X_PIXEL32_83    d15 = Interp8(w8, w6);
#define HQ4X_PIXEL33_0     d16 = w5;
#define HQ4X_PIXEL33_11    d16 = Interp1(w5, w6);
#define HQ4X_PIXEL33_12    d16 = Interp1(w5, w8);
#define HQ4X_PIXEL33_20    d16 = Interp2(w5, w8, w6);
#define HQ4X_PIXEL33_50    d16 = Interp5(w8, w6);
#define HQ4X_PIXEL33_80    d16 = Interp8(w5, w9);
#define HQ4X_PIXEL33_81    d16 = Interp8(w5, w6);
#define HQ4X_PIXEL33_82    d16 = Interp8(w5, w8);

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT ULONG*                  RGBtoYUV = NULL;

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT ULONG                   pencolours[COLOURSETS][PENS];
IMPORT ASCREEN                 screen[BOXWIDTH][BOXHEIGHT];
IMPORT int                     colourset,
                               enhancestars,
                               fastsize,
                               machine,
                               memmap,
                               rotating,
                               widthheight;
IMPORT ULONG                  *display;
                              *stars;
IMPORT struct MachineStruct    machines[MACHINES];

// MODULE FUNCTIONS-------------------------------------------------------

MODULE ULONG Interp1( ULONG c1, ULONG c2          );
MODULE ULONG Interp2( ULONG c1, ULONG c2, ULONG c3);
MODULE ULONG Interp3( ULONG c1, ULONG c2          );
MODULE ULONG Interp4( ULONG c1, ULONG c2, ULONG c3);
MODULE ULONG Interp5( ULONG c1, ULONG c2          );
MODULE ULONG Interp6( ULONG c1, ULONG c2, ULONG c3);
MODULE ULONG Interp7( ULONG c1, ULONG c2, ULONG c3);
MODULE ULONG Interp8( ULONG c1, ULONG c2          );
MODULE ULONG Interp9( ULONG c1, ULONG c2, ULONG c3);
MODULE ULONG Interp10(ULONG c1, ULONG c2, ULONG c3);
MODULE ULONG Interpolate_2(ULONG c1, int w1, ULONG c2, int w2, int s);
MODULE ULONG Interpolate_3(ULONG c1, int w1, ULONG c2, int w2, ULONG c3, int w3, int s);
MODULE int yuv_diff(ULONG yuv1, ULONG yuv2);
MODULE int Diff(ULONG c1, ULONG c2);

// CODE-------------------------------------------------------------------

MODULE ULONG Interp1( ULONG c1, ULONG c2          ) { return Interpolate_2(c1,  3, c2, 1, 2       ); }
MODULE ULONG Interp2( ULONG c1, ULONG c2, ULONG c3) { return Interpolate_3(c1,  2, c2, 1, c3, 1, 2); }
MODULE ULONG Interp3( ULONG c1, ULONG c2          ) { return Interpolate_2(c1,  7, c2, 1, 3       ); }
MODULE ULONG Interp4( ULONG c1, ULONG c2, ULONG c3) { return Interpolate_3(c1,  2, c2, 7, c3, 7, 4); }
MODULE ULONG Interp5( ULONG c1, ULONG c2          ) { return Interpolate_2(c1,  1, c2, 1, 1       ); }
MODULE ULONG Interp6( ULONG c1, ULONG c2, ULONG c3) { return Interpolate_3(c1,  5, c2, 2, c3, 1, 3); }
MODULE ULONG Interp7( ULONG c1, ULONG c2, ULONG c3) { return Interpolate_3(c1,  6, c2, 1, c3, 1, 3); }
MODULE ULONG Interp8( ULONG c1, ULONG c2          ) { return Interpolate_2(c1,  5, c2, 3, 3       ); }
MODULE ULONG Interp9( ULONG c1, ULONG c2, ULONG c3) { return Interpolate_3(c1,  2, c2, 3, c3, 3, 3); }
MODULE ULONG Interp10(ULONG c1, ULONG c2, ULONG c3) { return Interpolate_3(c1, 14, c2, 1, c3, 1, 4); }

MODULE ULONG Interpolate_2(ULONG c1, int w1, ULONG c2, int w2, int s)
{   if (c1 == c2)
    {   return c1;
    }

    return
        (((((c1 & MASK_ALPHA) >> 24) * w1 + ((c2 & MASK_ALPHA) >> 24) * w2) << (24-s)) & MASK_ALPHA) +
        ((((c1 & MASK_2) * w1 + (c2 & MASK_2) * w2) >> s) & MASK_2)	+
        ((((c1 & MASK_13) * w1 + (c2 & MASK_13) * w2) >> s) & MASK_13);
}

MODULE ULONG Interpolate_3(ULONG c1, int w1, ULONG c2, int w2, ULONG c3, int w3, int s)
{   return
        (((((c1 & MASK_ALPHA) >> 24) * w1 + ((c2 & MASK_ALPHA) >> 24) * w2 + ((c3 & MASK_ALPHA) >> 24) * w3) << (24-s)) & MASK_ALPHA) +
        ((((c1 & MASK_2) * w1 + (c2 & MASK_2) * w2 + (c3 & MASK_2) * w3) >> s) & MASK_2) +
        ((((c1 & MASK_13) * w1 + (c2 & MASK_13) * w2 + (c3 & MASK_13) * w3) >> s) & MASK_13);
}

MODULE int yuv_diff(ULONG yuv1, ULONG yuv2)
{   /* Test if there is difference in color */

    return (   (abs((yuv1 & Ymask) - (yuv2 & Ymask)) > trY)
            || (abs((yuv1 & Umask) - (yuv2 & Umask)) > trU)
            || (abs((yuv1 & Vmask) - (yuv2 & Vmask)) > trV)
           );
}

MODULE int Diff(ULONG c1, ULONG c2)
{   return yuv_diff(RGBtoYUV[c1], RGBtoYUV[c2]);
}

EXPORT void hqxInit(void)
{   /* Initalize RGB to YUV lookup table */
    ULONG c, r, g, b, y, u, v;

    if (RGBtoYUV)
    {   return; // already done
    }

    busypointer();
    if (!(RGBtoYUV = malloc(16777216 * 4)))
    {   normalpointer();
        rq("Out of memory!");
    }

    for (c = 0; c < 16777216; c++)
    {   r = (c & 0xFF0000) >> 16;
        g = (c & 0x00FF00) >>  8;
        b =  c & 0x0000FF       ;
        y = (ULONG) ( 0.299 * r + 0.587 * g + 0.114 * b);
        u = (ULONG) (-0.169 * r - 0.331 * g + 0.5   * b) + 128;
        v = (ULONG) ( 0.5   * r - 0.419 * g - 0.081 * b) + 128;
        RGBtoYUV[c] = (y << 16) + (u << 8) + v;
    }
    normalpointer();
}

EXPORT void apply_hq2x(void)
{   FAST int   flag,
               newx, newy,
               pattern,
               x, y;
    FAST ULONG d1, d2, d3, d4,
               pp,
               w1, w2, w3, w4, w5, w6, w7, w8, w9,
               yuv1, yuv2;

    hqxInit();

    for (x = 0; x < machines[machine].width; x++)
    {   for (y = 0; y < machines[machine].height; y++)
        {   if (rotating)
            {   newx = y;
                newy = widthheight - x;
                w1 = pencolours[colourset][screen[WEST(newx)][SOUTH(newy)]];
                w2 = pencolours[colourset][screen[WEST(newx)][      newy ]];
                w3 = pencolours[colourset][screen[WEST(newx)][NORTH(newy)]];
                w4 = pencolours[colourset][screen[     newx ][SOUTH(newy)]];
                w5 = pencolours[colourset][screen[     newx ][      newy ]];
                w6 = pencolours[colourset][screen[     newx ][NORTH(newy)]];
                w7 = pencolours[colourset][screen[EAST(newx)][SOUTH(newy)]];
                w8 = pencolours[colourset][screen[EAST(newx)][      newy ]];
                w9 = pencolours[colourset][screen[EAST(newx)][NORTH(newy)]];
            } else
            {   w1 = pencolours[colourset][screen[WEST(   x)][NORTH(   y)]];
                w2 = pencolours[colourset][screen[        x ][NORTH(   y)]];
                w3 = pencolours[colourset][screen[EAST(   x)][NORTH(   y)]];
                w4 = pencolours[colourset][screen[WEST(   x)][         y ]];
                w5 = pencolours[colourset][screen[        x ][         y ]];
                w6 = pencolours[colourset][screen[EAST(   x)][         y ]];
                w7 = pencolours[colourset][screen[WEST(   x)][SOUTH(   y)]];
                w8 = pencolours[colourset][screen[        x ][SOUTH(   y)]];
                w9 = pencolours[colourset][screen[EAST(   x)][SOUTH(   y)]];
            }

            pattern = 0;
            flag = 1;
            yuv1 = RGBtoYUV[w5];

            if (w1 != w5) { yuv2 = RGBtoYUV[w1]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w2 != w5) { yuv2 = RGBtoYUV[w2]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w3 != w5) { yuv2 = RGBtoYUV[w3]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w4 != w5) { yuv2 = RGBtoYUV[w4]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w6 != w5) { yuv2 = RGBtoYUV[w6]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w7 != w5) { yuv2 = RGBtoYUV[w7]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w8 != w5) { yuv2 = RGBtoYUV[w8]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w9 != w5) { yuv2 = RGBtoYUV[w9]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;

            switch (pattern)
            {
            case 0:
            case 1:
            case 4:
            case 32:
            case 128:
            case 5:
            case 132:
            case 160:
            case 33:
            case 129:
            case 36:
            case 133:
            case 164:
            case 161:
            case 37:
            case 165:
                PIXEL00_20 PIXEL01_20
                PIXEL10_20 PIXEL11_20
            acase 2:
            case 34:
            case 130:
            case 162:
                PIXEL00_22 PIXEL01_21
                PIXEL10_20 PIXEL11_20
            acase 16:
            case 17:
            case 48:
            case 49:
                PIXEL00_20 PIXEL01_22
                PIXEL10_20 PIXEL11_21
            acase 64:
            case 65:
            case 68:
            case 69:
                PIXEL00_20 PIXEL01_20
                PIXEL10_21 PIXEL11_22
            acase 8:
            case 12:
            case 136:
            case 140:
                PIXEL00_21 PIXEL01_20
                PIXEL10_22 PIXEL11_20
            acase 3:
            case 35:
            case 131:
            case 163:
                PIXEL00_11 PIXEL01_21
                PIXEL10_20 PIXEL11_20
            acase 6:
            case 38:
            case 134:
            case 166:
                PIXEL00_22 PIXEL01_12
                PIXEL10_20 PIXEL11_20
            acase 20:
            case 21:
            case 52:
            case 53:
                PIXEL00_20 PIXEL01_11
                PIXEL10_20 PIXEL11_21
            acase 144:
            case 145:
            case 176:
            case 177:
                PIXEL00_20 PIXEL01_22
                PIXEL10_20 PIXEL11_12
            acase 192:
            case 193:
            case 196:
            case 197:
                PIXEL00_20 PIXEL01_20
                PIXEL10_21 PIXEL11_11
            acase 96:
            case 97:
            case 100:
            case 101:
                PIXEL00_20 PIXEL01_20
                PIXEL10_12 PIXEL11_22
            acase 40:
            case 44:
            case 168:
            case 172:
                PIXEL00_21 PIXEL01_20
                PIXEL10_11 PIXEL11_20
            acase 9:
            case 13:
            case 137:
            case 141:
                PIXEL00_12 PIXEL01_20
                PIXEL10_22 PIXEL11_20
            acase 18:
            case 50:
                PIXEL00_22
                if (Diff(w2, w6))
                {   PIXEL01_10
                } else
                {   PIXEL01_20
                }
                PIXEL10_20
                PIXEL11_21
            acase 80:
            case 81:
                PIXEL00_20
                PIXEL01_22
                PIXEL10_21
                if (Diff(w6, w8))
                {   PIXEL11_10
                } else
                {   PIXEL11_20
                }
            acase 72:
            case 76:
                PIXEL00_21
                PIXEL01_20
                if (Diff(w8, w4))
                {   PIXEL10_10
                } else
                {   PIXEL10_20
                }
                PIXEL11_22
            acase 10:
            case 138:
                if (Diff(w4, w2))
                {   PIXEL00_10
                } else
                {   PIXEL00_20
                }
                PIXEL01_21
                PIXEL10_22
                PIXEL11_20
            acase 66:
                PIXEL00_22 PIXEL01_21
                PIXEL10_21 PIXEL11_22
            acase 24:
                PIXEL00_21 PIXEL01_22
                PIXEL10_22 PIXEL11_21
            acase 7:
            case 39:
            case 135:
                PIXEL00_11 PIXEL01_12
                PIXEL10_20 PIXEL11_20
            acase 148:
            case 149:
            case 180:
                PIXEL00_20 PIXEL01_11
                PIXEL10_20 PIXEL11_12
            acase 224:
            case 228:
            case 225:
                PIXEL00_20 PIXEL01_20
                PIXEL10_12 PIXEL11_11
            acase 41:
            case 169:
            case 45:
                PIXEL00_12 PIXEL01_20
                PIXEL10_11 PIXEL11_20
            acase 22:
            case 54:
                PIXEL00_22
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                PIXEL10_20
                PIXEL11_21
            acase 208:
            case 209:
                PIXEL00_20
                PIXEL01_22
                PIXEL10_21
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 104:
            case 108:
                PIXEL00_21
                PIXEL01_20
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                PIXEL11_22
            acase 11:
            case 139:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                PIXEL01_21
                PIXEL10_22
                PIXEL11_20
            acase 19:
            case 51:
                if (Diff(w2, w6))
                {   PIXEL00_11
                    PIXEL01_10
                } else
                {   PIXEL00_60
                    PIXEL01_90
                }
                PIXEL10_20
                PIXEL11_21
            acase 146:
            case 178:
                PIXEL00_22
                if (Diff(w2, w6))
                {   PIXEL01_10
                    PIXEL11_12
                } else
                {   PIXEL01_90
                    PIXEL11_61
                }
                PIXEL10_20
            acase 84:
            case 85:
                PIXEL00_20
                if (Diff(w6, w8))
                {   PIXEL01_11
                    PIXEL11_10
                } else
                {   PIXEL01_60
                    PIXEL11_90
                }
                PIXEL10_21
            acase 112:
            case 113:
                PIXEL00_20
                PIXEL01_22
                if (Diff(w6, w8))
                {   PIXEL10_12
                    PIXEL11_10
                } else
                {   PIXEL10_61
                    PIXEL11_90
                }
            acase 200:
            case 204:
                PIXEL00_21
                PIXEL01_20
                if (Diff(w8, w4))
                {   PIXEL10_10
                    PIXEL11_11
                } else
                {   PIXEL10_90
                    PIXEL11_60
                }
            acase 73:
            case 77:
                if (Diff(w8, w4))
                {   PIXEL00_12
                    PIXEL10_10
                } else
                {   PIXEL00_61
                    PIXEL10_90
                }
                PIXEL01_20
                PIXEL11_22
            acase 42:
            case 170:
                if (Diff(w4, w2))
                {   PIXEL00_10
                    PIXEL10_11
                } else
                {   PIXEL00_90
                    PIXEL10_60
                }
                PIXEL01_21
                PIXEL11_20
            acase 14:
            case 142:
                if (Diff(w4, w2))
                {   PIXEL00_10
                    PIXEL01_12
                } else
                {   PIXEL00_90
                    PIXEL01_61
                }
                PIXEL10_22
                PIXEL11_20
            acase 67:
                PIXEL00_11 PIXEL01_21
                PIXEL10_21 PIXEL11_22
            acase 70:
                PIXEL00_22 PIXEL01_12
                PIXEL10_21 PIXEL11_22
            acase 28:
                PIXEL00_21 PIXEL01_11
                PIXEL10_22 PIXEL11_21
            acase 152:
                PIXEL00_21 PIXEL01_22
                PIXEL10_22 PIXEL11_12
            acase 194:
                PIXEL00_22 PIXEL01_21
                PIXEL10_21 PIXEL11_11
            acase 98:
                PIXEL00_22 PIXEL01_21
                PIXEL10_12 PIXEL11_22
            acase 56:
                PIXEL00_21 PIXEL01_22
                PIXEL10_11 PIXEL11_21
            acase 25:
                PIXEL00_12 PIXEL01_22
                PIXEL10_22 PIXEL11_21
            acase 26:
            case 31:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                PIXEL10_22
                PIXEL11_21
            acase 82:
            case 214:
                PIXEL00_22
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                PIXEL10_21
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 88:
            case 248:
                PIXEL00_21
                PIXEL01_22
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 74:
            case 107:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                PIXEL01_21
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                PIXEL11_22
            acase 27:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                PIXEL01_10
                PIXEL10_22
                PIXEL11_21
            acase 86:
                PIXEL00_22
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                PIXEL10_21
                PIXEL11_10
            acase 216:
                PIXEL00_21
                PIXEL01_22
                PIXEL10_10
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 106:
                PIXEL00_10
                PIXEL01_21
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                PIXEL11_22
            acase 30:
                PIXEL00_10
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                PIXEL10_22
                PIXEL11_21
            acase 210:
                PIXEL00_22
                PIXEL01_10
                PIXEL10_21
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 120:
                PIXEL00_21
                PIXEL01_22
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                PIXEL11_10
            acase 75:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                PIXEL01_21
                PIXEL10_10
                PIXEL11_22
            acase 29:
                PIXEL00_12 PIXEL01_11
                PIXEL10_22 PIXEL11_21
            acase 198:
                PIXEL00_22 PIXEL01_12
                PIXEL10_21 PIXEL11_11
            acase 184:
                PIXEL00_21 PIXEL01_22
                PIXEL10_11 PIXEL11_12
            acase 99:
                PIXEL00_11 PIXEL01_21
                PIXEL10_12 PIXEL11_22
            acase 57:
                PIXEL00_12 PIXEL01_22
                PIXEL10_11 PIXEL11_21
            acase 71:
                PIXEL00_11 PIXEL01_12
                PIXEL10_21 PIXEL11_22
            acase 156:
                PIXEL00_21 PIXEL01_11
                PIXEL10_22 PIXEL11_12
            acase 226:
                PIXEL00_22 PIXEL01_21
                PIXEL10_12 PIXEL11_11
            acase 60:
                PIXEL00_21 PIXEL01_11
                PIXEL10_11 PIXEL11_21
            acase 195:
                PIXEL00_11 PIXEL01_21
                PIXEL10_21 PIXEL11_11
            acase 102:
                PIXEL00_22 PIXEL01_12
                PIXEL10_12 PIXEL11_22
            acase 153:
                PIXEL00_12 PIXEL01_22
                PIXEL10_22 PIXEL11_12
            acase 58:
                if (Diff(w4, w2))
                {   PIXEL00_10
                } else
                {   PIXEL00_70
                }
                if (Diff(w2, w6))
                {   PIXEL01_10
                } else
                {   PIXEL01_70
                }
                PIXEL10_11
                PIXEL11_21
            acase 83:
                PIXEL00_11
                if (Diff(w2, w6))
                {   PIXEL01_10
                } else
                {   PIXEL01_70
                }
                PIXEL10_21
                if (Diff(w6, w8))
                {   PIXEL11_10
                } else
                {   PIXEL11_70
                }
            acase 92:
                PIXEL00_21
                PIXEL01_11
                if (Diff(w8, w4))
                {   PIXEL10_10
                } else
                {   PIXEL10_70
                }
                if (Diff(w6, w8))
                {   PIXEL11_10
                } else
                {   PIXEL11_70
                }
            acase 202:
                if (Diff(w4, w2))
                {   PIXEL00_10
                } else
                {   PIXEL00_70
                }
                PIXEL01_21
                if (Diff(w8, w4))
                {   PIXEL10_10
                } else
                {   PIXEL10_70
                }
                PIXEL11_11
            acase 78:
                if (Diff(w4, w2))
                {   PIXEL00_10
                } else
                {   PIXEL00_70
                }
                PIXEL01_12
                if (Diff(w8, w4))
                {   PIXEL10_10
                } else
                {   PIXEL10_70
                }
                PIXEL11_22
            acase 154:
                if (Diff(w4, w2))
                {   PIXEL00_10
                } else
                {   PIXEL00_70
                }
                if (Diff(w2, w6))
                {   PIXEL01_10
                } else
                {   PIXEL01_70
                }
                PIXEL10_22
                PIXEL11_12
            acase 114:
                PIXEL00_22
                if (Diff(w2, w6))
                {   PIXEL01_10
                } else
                {   PIXEL01_70
                }
                PIXEL10_12
                if (Diff(w6, w8))
                {   PIXEL11_10
                } else
                {   PIXEL11_70
                }
            acase 89:
                PIXEL00_12
                PIXEL01_22
                if (Diff(w8, w4))
                {   PIXEL10_10
                } else
                {   PIXEL10_70
                }
                if (Diff(w6, w8))
                {   PIXEL11_10
                } else
                {   PIXEL11_70
                }
            acase 90:
                if (Diff(w4, w2))
                {   PIXEL00_10
                } else
                {   PIXEL00_70
                }
                if (Diff(w2, w6))
                {   PIXEL01_10
                } else
                {   PIXEL01_70
                }
                if (Diff(w8, w4))
                {   PIXEL10_10
                } else
                {   PIXEL10_70
                }
                if (Diff(w6, w8))
                {   PIXEL11_10
                } else
                {   PIXEL11_70
                }
            acase 55:
            case 23:
                if (Diff(w2, w6))
                {   PIXEL00_11
                    PIXEL01_0
                } else
                {   PIXEL00_60
                    PIXEL01_90
                }
                PIXEL10_20
                PIXEL11_21
            acase 182:
            case 150:
                PIXEL00_22
                if (Diff(w2, w6))
                {   PIXEL01_0
                    PIXEL11_12
                } else
                {   PIXEL01_90
                    PIXEL11_61
                }
                PIXEL10_20
            acase 213:
            case 212:
                PIXEL00_20
                if (Diff(w6, w8))
                {   PIXEL01_11
                    PIXEL11_0
                } else
                {   PIXEL01_60
                    PIXEL11_90
                }
                PIXEL10_21
            acase 241:
            case 240:
                PIXEL00_20
                PIXEL01_22
                if (Diff(w6, w8))
                {   PIXEL10_12
                    PIXEL11_0
                } else
                {   PIXEL10_61
                    PIXEL11_90
                }
            acase 236:
            case 232:
                PIXEL00_21
                PIXEL01_20
                if (Diff(w8, w4))
                {   PIXEL10_0
                    PIXEL11_11
                } else
                {   PIXEL10_90
                    PIXEL11_60
                }
            acase 109:
            case 105:
                if (Diff(w8, w4))
                {   PIXEL00_12
                    PIXEL10_0
                } else
                {   PIXEL00_61
                    PIXEL10_90
                }
                PIXEL01_20
                PIXEL11_22
            acase 171:
            case 43:
                if (Diff(w4, w2))
                {   PIXEL00_0
                    PIXEL10_11
                } else
                {   PIXEL00_90
                    PIXEL10_60
                }
                PIXEL01_21
                PIXEL11_20
            acase 143:
            case 15:
                if (Diff(w4, w2))
                {   PIXEL00_0
                    PIXEL01_12
                } else
                {   PIXEL00_90
                    PIXEL01_61
                }
                PIXEL10_22
                PIXEL11_20
            acase 124:
                PIXEL00_21
                PIXEL01_11
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                PIXEL11_10
            acase 203:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                PIXEL01_21
                PIXEL10_10
                PIXEL11_11
            acase 62:
                PIXEL00_10
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                PIXEL10_11
                PIXEL11_21
            acase 211:
                PIXEL00_11
                PIXEL01_10
                PIXEL10_21
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 118:
                PIXEL00_22
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                PIXEL10_12
                PIXEL11_10
            acase 217:
                PIXEL00_12
                PIXEL01_22
                PIXEL10_10
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 110:
                PIXEL00_10
                PIXEL01_12
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                PIXEL11_22
            acase 155:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                PIXEL01_10
                PIXEL10_22
                PIXEL11_12
            acase 188:
                PIXEL00_21 PIXEL01_11
                PIXEL10_11 PIXEL11_12
            acase 185:
                PIXEL00_12 PIXEL01_22
                PIXEL10_11 PIXEL11_12
            acase 61:
                PIXEL00_12 PIXEL01_11
                PIXEL10_11 PIXEL11_21
            acase 157:
                PIXEL00_12 PIXEL01_11
                PIXEL10_22 PIXEL11_12
            acase 103:
                PIXEL00_11 PIXEL01_12
                PIXEL10_12 PIXEL11_22
            acase 227:
                PIXEL00_11 PIXEL01_21
                PIXEL10_12 PIXEL11_11
            acase 230:
                PIXEL00_22 PIXEL01_12
                PIXEL10_12 PIXEL11_11
            acase 199:
                PIXEL00_11 PIXEL01_12
                PIXEL10_21 PIXEL11_11
            acase 220:
                PIXEL00_21
                PIXEL01_11
                if (Diff(w8, w4))
                {   PIXEL10_10
                } else
                {   PIXEL10_70
                }
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 158:
                if (Diff(w4, w2))
                {   PIXEL00_10
                } else
                {   PIXEL00_70
                }
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                PIXEL10_22
                PIXEL11_12
            acase 234:
                if (Diff(w4, w2))
                {   PIXEL00_10
                } else
                {   PIXEL00_70
                }
                PIXEL01_21
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                PIXEL11_11
            acase 242:
                PIXEL00_22
                if (Diff(w2, w6))
                {   PIXEL01_10
                } else
                {   PIXEL01_70
                }
                PIXEL10_12
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 59:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                if (Diff(w2, w6))
                {   PIXEL01_10
                } else
                {   PIXEL01_70
                }
                PIXEL10_11
                PIXEL11_21
            acase 121:
                PIXEL00_12
                PIXEL01_22
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                if (Diff(w6, w8))
                {   PIXEL11_10
                } else
                {   PIXEL11_70
                }
            acase 87:
                PIXEL00_11
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                PIXEL10_21
                if (Diff(w6, w8))
                {   PIXEL11_10
                } else
                {   PIXEL11_70
                }
            acase 79:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                PIXEL01_12
                if (Diff(w8, w4))
                {   PIXEL10_10
                } else
                {   PIXEL10_70
                }
                PIXEL11_22
            acase 122:
                if (Diff(w4, w2))
                {   PIXEL00_10
                } else
                {   PIXEL00_70
                }
                if (Diff(w2, w6))
                {   PIXEL01_10
                } else
                {   PIXEL01_70
                }
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                if (Diff(w6, w8))
                {   PIXEL11_10
                } else
                {   PIXEL11_70
                }
            acase 94:
                if (Diff(w4, w2))
                {   PIXEL00_10
                } else
                {   PIXEL00_70
                }
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                if (Diff(w8, w4))
                {   PIXEL10_10
                } else
                {   PIXEL10_70
                }
                if (Diff(w6, w8))
                {   PIXEL11_10
                } else
                {   PIXEL11_70
                }
            acase 218:
                if (Diff(w4, w2))
                {   PIXEL00_10
                } else
                {   PIXEL00_70
                }
                if (Diff(w2, w6))
                {   PIXEL01_10
                } else
                {   PIXEL01_70
                }
                if (Diff(w8, w4))
                {   PIXEL10_10
                } else
                {   PIXEL10_70
                }
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 91:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                if (Diff(w2, w6))
                {   PIXEL01_10
                } else
                {   PIXEL01_70
                }
                if (Diff(w8, w4))
                {   PIXEL10_10
                } else
                {   PIXEL10_70
                }
                if (Diff(w6, w8))
                {   PIXEL11_10
                } else
                {   PIXEL11_70
                }
            acase 229:
                PIXEL00_20 PIXEL01_20
                PIXEL10_12 PIXEL11_11
            acase 167:
                PIXEL00_11 PIXEL01_12
                PIXEL10_20 PIXEL11_20
            acase 173:
                PIXEL00_12 PIXEL01_20
                PIXEL10_11 PIXEL11_20
            acase 181:
                PIXEL00_20 PIXEL01_11
                PIXEL10_20 PIXEL11_12
            acase 186:
                if (Diff(w4, w2))
                {   PIXEL00_10
                } else
                {   PIXEL00_70
                }
                if (Diff(w2, w6))
                {   PIXEL01_10
                } else
                {   PIXEL01_70
                }
                PIXEL10_11
                PIXEL11_12
            acase 115:
                PIXEL00_11
                if (Diff(w2, w6))
                {   PIXEL01_10
                } else
                {   PIXEL01_70
                }
                PIXEL10_12
                if (Diff(w6, w8))
                {   PIXEL11_10
                } else
                {   PIXEL11_70
                }
            acase 93:
                PIXEL00_12
                PIXEL01_11
                if (Diff(w8, w4))
                {   PIXEL10_10
                } else
                {   PIXEL10_70
                }
                if (Diff(w6, w8))
                {   PIXEL11_10
                } else
                {   PIXEL11_70
                }
            acase 206:
                if (Diff(w4, w2))
                {   PIXEL00_10
                } else
                {   PIXEL00_70
                }
                PIXEL01_12
                if (Diff(w8, w4))
                {   PIXEL10_10
                } else
                {   PIXEL10_70
                }
                PIXEL11_11
            acase 205:
            case 201:
                PIXEL00_12
                PIXEL01_20
                if (Diff(w8, w4))
                {   PIXEL10_10
                } else
                {   PIXEL10_70
                }
                PIXEL11_11
            acase 174:
            case 46:
                if (Diff(w4, w2))
                {   PIXEL00_10
                } else
                {   PIXEL00_70
                }
                PIXEL01_12
                PIXEL10_11
                PIXEL11_20
            acase 179:
            case 147:
                PIXEL00_11
                if (Diff(w2, w6))
                {   PIXEL01_10
                } else
                {   PIXEL01_70
                }
                PIXEL10_20
                PIXEL11_12
            acase 117:
            case 116:
                PIXEL00_20
                PIXEL01_11
                PIXEL10_12
                if (Diff(w6, w8))
                {   PIXEL11_10
                } else
                {   PIXEL11_70
                }
            acase 189:
                PIXEL00_12 PIXEL01_11
                PIXEL10_11 PIXEL11_12
            acase 231:
                PIXEL00_11 PIXEL01_12
                PIXEL10_12 PIXEL11_11
            acase 126:
                PIXEL00_10
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                PIXEL11_10
            acase 219:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                PIXEL01_10
                PIXEL10_10
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 125:
                if (Diff(w8, w4))
                {   PIXEL00_12
                    PIXEL10_0
                } else
                {   PIXEL00_61
                    PIXEL10_90
                }
                PIXEL01_11
                PIXEL11_10
            acase 221:
                PIXEL00_12
                if (Diff(w6, w8))
                {   PIXEL01_11
                    PIXEL11_0
                } else
                {   PIXEL01_60
                    PIXEL11_90
                }
                PIXEL10_10
            acase 207:
                if (Diff(w4, w2))
                {   PIXEL00_0
                    PIXEL01_12
                } else
                {   PIXEL00_90
                    PIXEL01_61
                }
                PIXEL10_10
                PIXEL11_11
            acase 238:
                PIXEL00_10
                PIXEL01_12
                if (Diff(w8, w4))
                {   PIXEL10_0
                    PIXEL11_11
                } else
                {   PIXEL10_90
                    PIXEL11_60
                }
            acase 190:
                PIXEL00_10
                if (Diff(w2, w6))
                {   PIXEL01_0
                    PIXEL11_12
                } else
                {   PIXEL01_90
                    PIXEL11_61
                }
                PIXEL10_11
            acase 187:
                if (Diff(w4, w2))
                {   PIXEL00_0
                    PIXEL10_11
                } else
                {   PIXEL00_90
                    PIXEL10_60
                }
                PIXEL01_10
                PIXEL11_12
            acase 243:
                PIXEL00_11
                PIXEL01_10
                if (Diff(w6, w8))
                {   PIXEL10_12
                    PIXEL11_0
                } else
                {   PIXEL10_61
                    PIXEL11_90
                }
            acase 119:
                if (Diff(w2, w6))
                {   PIXEL00_11
                    PIXEL01_0
                } else
                {   PIXEL00_60
                    PIXEL01_90
                }
                PIXEL10_12
                PIXEL11_10
            acase 237:
            case 233:
                PIXEL00_12
                PIXEL01_20
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_100
                }
                PIXEL11_11
            acase 175:
            case 47:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_100
                }
                PIXEL01_12
                PIXEL10_11
                PIXEL11_20
            acase 183:
            case 151:
                PIXEL00_11
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_100
                }
                PIXEL10_20
                PIXEL11_12
            acase 245:
            case 244:
                PIXEL00_20
                PIXEL01_11
                PIXEL10_12
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_100
                }
            acase 250:
                PIXEL00_10
                PIXEL01_10
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 123:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                PIXEL01_10
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                PIXEL11_10
            acase 95:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                PIXEL10_10
                PIXEL11_10
            acase 222:
                PIXEL00_10
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                PIXEL10_10
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 252:
                PIXEL00_21
                PIXEL01_11
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_100
                }
            acase 249:
                PIXEL00_12
                PIXEL01_22
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_100
                }
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 235:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                PIXEL01_21
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_100
                }
                PIXEL11_11
            acase 111:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_100
                }
                PIXEL01_12
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                PIXEL11_22
            acase 63:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_100
                }
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                PIXEL10_11
                PIXEL11_21
            acase 159:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_100
                }
                PIXEL10_22
                PIXEL11_12
            acase 215:
                PIXEL00_11
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_100
                }
                PIXEL10_21
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 246:
                PIXEL00_22
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                PIXEL10_12
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_100
                }
            acase 254:
                PIXEL00_10
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_100
                }
            acase 253:
                PIXEL00_12
                PIXEL01_11
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_100
                }
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_100
                }
            acase 251:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                PIXEL01_10
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_100
                }
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 239:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_100
                }
                PIXEL01_12
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_100
                }
                PIXEL11_11
            acase 127:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_100
                }
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_20
                }
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_20
                }
                PIXEL11_10
            acase 191:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_100
                }
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_100
                }
                PIXEL10_11
                PIXEL11_12
            acase 223:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_20
                }
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_100
                }
                PIXEL10_10
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_20
                }
            acase 247:
                PIXEL00_11
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_100
                }
                PIXEL10_12
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_100
                }
            acase 255:
                if (Diff(w4, w2))
                {   PIXEL00_0
                } else
                {   PIXEL00_100
                }
                if (Diff(w2, w6))
                {   PIXEL01_0
                } else
                {   PIXEL01_100
                }
                if (Diff(w8, w4))
                {   PIXEL10_0
                } else
                {   PIXEL10_100
                }
                if (Diff(w6, w8))
                {   PIXEL11_0
                } else
                {   PIXEL11_100
            }   }

            switch (fastsize)
            {
            case 2: // 2*2
                pp = (x * 2) + (y * 4 * machines[machine].width);
                COPYDISP2(    pp                                    , d1);
                COPYDISP2(    pp + 1                                , d2);
                COPYDISP2(    pp +     (machines[machine].width * 2), d3);
                COPYDISP2(    pp + 1 + (machines[machine].width * 2), d4);
            acase 3: // 4*2
                pp = (x * 4) + (y * 8 * machines[machine].width);
                COPYWIDEDISP2(pp                                    , d1);
                COPYWIDEDISP2(pp + 2                                , d2);
                COPYWIDEDISP2(pp +     (machines[machine].width * 4), d3);
                COPYWIDEDISP2(pp + 2 + (machines[machine].width * 4), d4);
}   }   }   }

EXPORT void apply_hq3x(void)
{   FAST int   flag,
               newx, newy,
               pattern,
               x, y;
    FAST ULONG d1, d2, d3, d4, d5, d6, d7, d8, d9,
               pp,
               w1, w2, w3, w4, w5, w6, w7, w8, w9,
               yuv1, yuv2;

    hqxInit();

    for (x = 0; x < machines[machine].width; x++)
    {   for (y = 0; y < machines[machine].height; y++)
        {   if (rotating)
            {   newx = y;
                newy = widthheight - x;
                w1 = pencolours[colourset][screen[WEST(newx)][SOUTH(newy)]];
                w2 = pencolours[colourset][screen[WEST(newx)][      newy ]];
                w3 = pencolours[colourset][screen[WEST(newx)][NORTH(newy)]];
                w4 = pencolours[colourset][screen[     newx ][SOUTH(newy)]];
                w5 = pencolours[colourset][screen[     newx ][      newy ]];
                w6 = pencolours[colourset][screen[     newx ][NORTH(newy)]];
                w7 = pencolours[colourset][screen[EAST(newx)][SOUTH(newy)]];
                w8 = pencolours[colourset][screen[EAST(newx)][      newy ]];
                w9 = pencolours[colourset][screen[EAST(newx)][NORTH(newy)]];
            } else
            {   w1 = pencolours[colourset][screen[WEST(   x)][NORTH(   y)]];
                w2 = pencolours[colourset][screen[        x ][NORTH(   y)]];
                w3 = pencolours[colourset][screen[EAST(   x)][NORTH(   y)]];
                w4 = pencolours[colourset][screen[WEST(   x)][         y ]];
                w5 = pencolours[colourset][screen[        x ][         y ]];
                w6 = pencolours[colourset][screen[EAST(   x)][         y ]];
                w7 = pencolours[colourset][screen[WEST(   x)][SOUTH(   y)]];
                w8 = pencolours[colourset][screen[        x ][SOUTH(   y)]];
                w9 = pencolours[colourset][screen[EAST(   x)][SOUTH(   y)]];
            }

            pattern = 0;
            flag = 1;
            yuv1 = RGBtoYUV[w5];

            if (w1 != w5) { yuv2 = RGBtoYUV[w1]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w2 != w5) { yuv2 = RGBtoYUV[w2]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w3 != w5) { yuv2 = RGBtoYUV[w3]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w4 != w5) { yuv2 = RGBtoYUV[w4]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w6 != w5) { yuv2 = RGBtoYUV[w6]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w7 != w5) { yuv2 = RGBtoYUV[w7]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w8 != w5) { yuv2 = RGBtoYUV[w8]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w9 != w5) { yuv2 = RGBtoYUV[w9]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;

            PIXEL11

            switch (pattern)
            {
            case 0:
            case 1:
            case 4:
            case 32:
            case 128:
            case 5:
            case 132:
            case 160:
            case 33:
            case 129:
            case 36:
            case 133:
            case 164:
            case 161:
            case 37:
            case 165:
                PIXEL00_2  PIXEL01_1 PIXEL02_2
                PIXEL10_1            PIXEL12_1
                PIXEL20_2  PIXEL21_1 PIXEL22_2
            acase 2:
            case 34:
            case 130:
            case 162:
                PIXEL00_1M PIXEL01_C PIXEL02_1M
                PIXEL10_1            PIXEL12_1
                PIXEL20_2  PIXEL21_1 PIXEL22_2
            acase 16:
            case 17:
            case 48:
            case 49:
                PIXEL00_2  PIXEL01_1 PIXEL02_1M
                PIXEL10_1            PIXEL12_C
                PIXEL20_2  PIXEL21_1 PIXEL22_1M
            acase 64:
            case 65:
            case 68:
            case 69:
                PIXEL00_2  PIXEL01_1 PIXEL02_2
                PIXEL10_1            PIXEL12_1
                PIXEL20_1M PIXEL21_C PIXEL22_1M
            acase 8:
            case 12:
            case 136:
            case 140:
                PIXEL00_1M PIXEL01_1 PIXEL02_2
                PIXEL10_C            PIXEL12_1
                PIXEL20_1M PIXEL21_1 PIXEL22_2
            acase 3:
            case 35:
            case 131:
            case 163:
                PIXEL00_1L PIXEL01_C PIXEL02_1M
                PIXEL10_1            PIXEL12_1
                PIXEL20_2  PIXEL21_1 PIXEL22_2
            acase 6:
            case 38:
            case 134:
            case 166:
                PIXEL00_1M PIXEL01_C PIXEL02_1R
                PIXEL10_1            PIXEL12_1
                PIXEL20_2  PIXEL21_1 PIXEL22_2
            acase 20:
            case 21:
            case 52:
            case 53:
                PIXEL00_2  PIXEL01_1 PIXEL02_1U
                PIXEL10_1            PIXEL12_C
                PIXEL20_2  PIXEL21_1 PIXEL22_1M
            acase 144:
            case 145:
            case 176:
            case 177:
                PIXEL00_2  PIXEL01_1 PIXEL02_1M
                PIXEL10_1            PIXEL12_C
                PIXEL20_2  PIXEL21_1 PIXEL22_1D
            acase 192:
            case 193:
            case 196:
            case 197:
                PIXEL00_2  PIXEL01_1 PIXEL02_2
                PIXEL10_1            PIXEL12_1
                PIXEL20_1M PIXEL21_C PIXEL22_1R
            acase 96:
            case 97:
            case 100:
            case 101:
                PIXEL00_2  PIXEL01_1 PIXEL02_2
                PIXEL10_1            PIXEL12_1
                PIXEL20_1L PIXEL21_C PIXEL22_1M
            acase 40:
            case 44:
            case 168:
            case 172:
                PIXEL00_1M PIXEL01_1 PIXEL02_2
                PIXEL10_C            PIXEL12_1
                PIXEL20_1D PIXEL21_1 PIXEL22_2
            acase 9:
            case 13:
            case 137:
            case 141:
                PIXEL00_1U PIXEL01_1 PIXEL02_2
                PIXEL10_C            PIXEL12_1
                PIXEL20_1M PIXEL21_1 PIXEL22_2
            acase 18:
            case 50:
                PIXEL00_1M
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_1M
                    PIXEL12_C
                } else
                {   PIXEL01_3
                    PIXEL02_4
                    PIXEL12_3
                }
                PIXEL10_1
                PIXEL20_2
                PIXEL21_1
                PIXEL22_1M
            acase 80:
            case 81:
                PIXEL00_2
                PIXEL01_1
                PIXEL02_1M
                PIXEL10_1
                PIXEL20_1M
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL21_C
                    PIXEL22_1M
                } else
                {   PIXEL12_3
                    PIXEL21_3
                    PIXEL22_4
                }
            acase 72:
            case 76:
                PIXEL00_1M
                PIXEL01_1
                PIXEL02_2
                PIXEL12_1
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_1M
                    PIXEL21_C
                } else
                {   PIXEL10_3
                    PIXEL20_4
                    PIXEL21_3
                }
                PIXEL22_1M
            acase 10:
            case 138:
                if (Diff(w4, w2))
                {   PIXEL00_1M
                    PIXEL01_C
                    PIXEL10_C
                } else
                {   PIXEL00_4
                    PIXEL01_3
                    PIXEL10_3
                }
                PIXEL02_1M
                PIXEL12_1
                PIXEL20_1M
                PIXEL21_1
                PIXEL22_2
            acase 66:
                PIXEL00_1M PIXEL01_C PIXEL02_1M
                PIXEL10_1            PIXEL12_1
                PIXEL20_1M PIXEL21_C PIXEL22_1M
            acase 24:
                PIXEL00_1M PIXEL01_1 PIXEL02_1M
                PIXEL10_C            PIXEL12_C
                PIXEL20_1M PIXEL21_1 PIXEL22_1M
            acase 7:
            case 39:
            case 135:
                PIXEL00_1L PIXEL01_C PIXEL02_1R
                PIXEL10_1            PIXEL12_1
                PIXEL20_2  PIXEL21_1 PIXEL22_2
            acase 148:
            case 149:
            case 180:
                PIXEL00_2  PIXEL01_1 PIXEL02_1U
                PIXEL10_1            PIXEL12_C
                PIXEL20_2  PIXEL21_1 PIXEL22_1D
            acase 224:
            case 228:
            case 225:
                PIXEL00_2  PIXEL01_1 PIXEL02_2
                PIXEL10_1            PIXEL12_1
                PIXEL20_1L PIXEL21_C PIXEL22_1R
            acase 41:
            case 169:
            case 45:
                PIXEL00_1U PIXEL01_1 PIXEL02_2
                PIXEL10_C            PIXEL12_1
                PIXEL20_1D PIXEL21_1 PIXEL22_2
            acase 22:
            case 54:
                PIXEL00_1M
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_C
                    PIXEL12_C
                } else
                {   PIXEL01_3
                    PIXEL02_4
                    PIXEL12_3
                }
                PIXEL10_1
                PIXEL20_2
                PIXEL21_1
                PIXEL22_1M
            acase 208:
            case 209:
                PIXEL00_2
                PIXEL01_1
                PIXEL02_1M
                PIXEL10_1
                PIXEL20_1M
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL12_3
                    PIXEL21_3
                    PIXEL22_4
                }
            acase 104:
            case 108:
                PIXEL00_1M
                PIXEL01_1
                PIXEL02_2
                PIXEL12_1
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_C
                    PIXEL21_C
                } else
                {   PIXEL10_3
                    PIXEL20_4
                    PIXEL21_3
                }
                PIXEL22_1M
            acase 11:
            case 139:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                    PIXEL10_C
                } else
                {   PIXEL00_4
                    PIXEL01_3
                    PIXEL10_3
                }
                PIXEL02_1M
                PIXEL12_1
                PIXEL20_1M
                PIXEL21_1
                PIXEL22_2
            acase 19:
            case 51:
                if (Diff(w2, w6))
                {   PIXEL00_1L
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL12_C
                } else
                {   PIXEL00_2
                    PIXEL01_6
                    PIXEL02_5
                    PIXEL12_1
                }
                PIXEL10_1
                PIXEL20_2
                PIXEL21_1
                PIXEL22_1M
            acase 146:
            case 178:
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_1M
                    PIXEL12_C
                    PIXEL22_1D
                } else
                {   PIXEL01_1
                    PIXEL02_5
                    PIXEL12_6
                    PIXEL22_2
                }
                PIXEL00_1M
                PIXEL10_1
                PIXEL20_2
                PIXEL21_1
            acase 84:
            case 85:
                if (Diff(w6, w8))
                {   PIXEL02_1U
                    PIXEL12_C
                    PIXEL21_C
                    PIXEL22_1M
                } else
                {   PIXEL02_2
                    PIXEL12_6
                    PIXEL21_1
                    PIXEL22_5
                }
                PIXEL00_2
                PIXEL01_1
                PIXEL10_1
                PIXEL20_1M
            acase 112:
            case 113:
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL20_1L
                    PIXEL21_C
                    PIXEL22_1M
                } else
                {   PIXEL12_1
                    PIXEL20_2
                    PIXEL21_6
                    PIXEL22_5
                }
                PIXEL00_2
                PIXEL01_1
                PIXEL02_1M
                PIXEL10_1
            acase 200:
            case 204:
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_1M
                    PIXEL21_C
                    PIXEL22_1R
                } else
                {   PIXEL10_1
                    PIXEL20_5
                    PIXEL21_6
                    PIXEL22_2
                }
                PIXEL00_1M
                PIXEL01_1
                PIXEL02_2
                PIXEL12_1
            acase 73:
            case 77:
                if (Diff(w8, w4))
                {   PIXEL00_1U
                    PIXEL10_C
                    PIXEL20_1M
                    PIXEL21_C
                } else
                {   PIXEL00_2
                    PIXEL10_6
                    PIXEL20_5
                    PIXEL21_1
                }
                PIXEL01_1
                PIXEL02_2
                PIXEL12_1
                PIXEL22_1M
            acase 42:
            case 170:
                if (Diff(w4, w2))
                {   PIXEL00_1M
                    PIXEL01_C
                    PIXEL10_C
                    PIXEL20_1D
                } else
                {   PIXEL00_5
                    PIXEL01_1
                    PIXEL10_6
                    PIXEL20_2
                }
                PIXEL02_1M
                PIXEL12_1
                PIXEL21_1
                PIXEL22_2
            acase 14:
            case 142:
                if (Diff(w4, w2))
                {   PIXEL00_1M
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_C
                } else
                {   PIXEL00_5
                    PIXEL01_6
                    PIXEL02_2
                    PIXEL10_1
                }
                PIXEL12_1
                PIXEL20_1M
                PIXEL21_1
                PIXEL22_2
            acase 67:
                PIXEL00_1L PIXEL01_C PIXEL02_1M
                PIXEL10_1            PIXEL12_1
                PIXEL20_1M PIXEL21_C PIXEL22_1M
            acase 70:
                PIXEL00_1M PIXEL01_C PIXEL02_1R
                PIXEL10_1            PIXEL12_1
                PIXEL20_1M PIXEL21_C PIXEL22_1M
            acase 28:
                PIXEL00_1M PIXEL01_1 PIXEL02_1U
                PIXEL10_C            PIXEL12_C
                PIXEL20_1M PIXEL21_1 PIXEL22_1M
            acase 152:
                PIXEL00_1M PIXEL01_1 PIXEL02_1M
                PIXEL10_C            PIXEL12_C
                PIXEL20_1M PIXEL21_1 PIXEL22_1D
            acase 194:
                PIXEL00_1M PIXEL01_C PIXEL02_1M
                PIXEL10_1            PIXEL12_1
                PIXEL20_1M PIXEL21_C PIXEL22_1R
            acase 98:
                PIXEL00_1M PIXEL01_C PIXEL02_1M
                PIXEL10_1            PIXEL12_1
                PIXEL20_1L PIXEL21_C PIXEL22_1M
            acase 56:
                PIXEL00_1M PIXEL01_1 PIXEL02_1M
                PIXEL10_C            PIXEL12_C
                PIXEL20_1D PIXEL21_1 PIXEL22_1M
            acase 25:
                PIXEL00_1U PIXEL01_1 PIXEL02_1M
                PIXEL10_C            PIXEL12_C
                PIXEL20_1M PIXEL21_1 PIXEL22_1M
            acase 26:
            case 31:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL10_C
                } else
                {   PIXEL00_4
                    PIXEL10_3
                }
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_C
                    PIXEL12_C
                } else
                {   PIXEL02_4
                    PIXEL12_3
                }
                PIXEL20_1M
                PIXEL21_1
                PIXEL22_1M
            acase 82:
            case 214:
                PIXEL00_1M
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_C
                } else
                {   PIXEL01_3
                    PIXEL02_4
                }
                PIXEL10_1
                PIXEL12_C
                PIXEL20_1M
                if (Diff(w6, w8))
                {   PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL21_3
                    PIXEL22_4
                }
            acase 88:
            case 248:
                PIXEL00_1M
                PIXEL01_1
                PIXEL02_1M
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_C
                } else
                {   PIXEL10_3
                    PIXEL20_4
                }
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL22_C
                } else
                {   PIXEL12_3
                    PIXEL22_4
                }
            acase 74:
            case 107:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                } else
                {   PIXEL00_4
                    PIXEL01_3
                }
                PIXEL02_1M
                PIXEL10_C
                PIXEL12_1
                if (Diff(w8, w4))
                {   PIXEL20_C
                    PIXEL21_C
                } else
                {   PIXEL20_4
                    PIXEL21_3
                }
                PIXEL22_1M
            acase 27:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                    PIXEL10_C
                } else
                {   PIXEL00_4
                    PIXEL01_3
                    PIXEL10_3
                }
                PIXEL02_1M
                PIXEL12_C
                PIXEL20_1M
                PIXEL21_1
                PIXEL22_1M
            acase 86:
                PIXEL00_1M
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_C
                    PIXEL12_C
                } else
                {   PIXEL01_3
                    PIXEL02_4
                    PIXEL12_3
                }
                PIXEL10_1
                PIXEL20_1M
                PIXEL21_C
                PIXEL22_1M
            acase 216:
                PIXEL00_1M
                PIXEL01_1
                PIXEL02_1M
                PIXEL10_C
                PIXEL20_1M
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL12_3
                    PIXEL21_3
                    PIXEL22_4
                }
            acase 106:
                PIXEL00_1M
                PIXEL01_C
                PIXEL02_1M
                PIXEL12_1
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_C
                    PIXEL21_C
                } else
                {   PIXEL10_3
                    PIXEL20_4
                    PIXEL21_3
                }
                PIXEL22_1M
            acase 30:
                PIXEL00_1M
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_C
                    PIXEL12_C
                } else
                {   PIXEL01_3
                    PIXEL02_4
                    PIXEL12_3
                }
                PIXEL10_C
                PIXEL20_1M
                PIXEL21_1
                PIXEL22_1M
            acase 210:
                PIXEL00_1M
                PIXEL01_C
                PIXEL02_1M
                PIXEL10_1
                PIXEL20_1M
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL12_3
                    PIXEL21_3
                    PIXEL22_4
                }
            acase 120:
                PIXEL00_1M
                PIXEL01_1
                PIXEL02_1M
                PIXEL12_C
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_C
                    PIXEL21_C
                } else
                {   PIXEL10_3
                    PIXEL20_4
                    PIXEL21_3
                }
                PIXEL22_1M
            acase 75:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                    PIXEL10_C
                } else
                {   PIXEL00_4
                    PIXEL01_3
                    PIXEL10_3
                }
                PIXEL02_1M
                PIXEL12_1
                PIXEL20_1M
                PIXEL21_C
                PIXEL22_1M
            acase 29:
                PIXEL00_1U PIXEL01_1 PIXEL02_1U
                PIXEL10_C            PIXEL12_C
                PIXEL20_1M PIXEL21_1 PIXEL22_1M
            acase 198:
                PIXEL00_1M PIXEL01_C PIXEL02_1R
                PIXEL10_1            PIXEL12_1
                PIXEL20_1M PIXEL21_C PIXEL22_1R
            acase 184:
                PIXEL00_1M PIXEL01_1 PIXEL02_1M
                PIXEL10_C            PIXEL12_C
                PIXEL20_1D PIXEL21_1 PIXEL22_1D
            acase 99:
                PIXEL00_1L PIXEL01_C PIXEL02_1M
                PIXEL10_1            PIXEL12_1
                PIXEL20_1L PIXEL21_C PIXEL22_1M
            acase 57:
                PIXEL00_1U PIXEL01_1 PIXEL02_1M
                PIXEL10_C            PIXEL12_C
                PIXEL20_1D PIXEL21_1 PIXEL22_1M
            acase 71:
                PIXEL00_1L PIXEL01_C PIXEL02_1R
                PIXEL10_1            PIXEL12_1
                PIXEL20_1M PIXEL21_C PIXEL22_1M
            acase 156:
                PIXEL00_1M PIXEL01_1 PIXEL02_1U
                PIXEL10_C            PIXEL12_C
                PIXEL20_1M PIXEL21_1 PIXEL22_1D
            acase 226:
                PIXEL00_1M PIXEL01_C PIXEL02_1M
                PIXEL10_1            PIXEL12_1
                PIXEL20_1L PIXEL21_C PIXEL22_1R
            acase 60:
                PIXEL00_1M PIXEL01_1 PIXEL02_1U
                PIXEL10_C            PIXEL12_C
                PIXEL20_1D PIXEL21_1 PIXEL22_1M
            acase 195:
                PIXEL00_1L PIXEL01_C PIXEL02_1M
                PIXEL10_1            PIXEL12_1
                PIXEL20_1M PIXEL21_C PIXEL22_1R
            acase 102:
                PIXEL00_1M PIXEL01_C PIXEL02_1R
                PIXEL10_1            PIXEL12_1
                PIXEL20_1L PIXEL21_C PIXEL22_1M
            acase 153:
                PIXEL00_1U PIXEL01_1 PIXEL02_1M
                PIXEL10_C            PIXEL12_C
                PIXEL20_1M PIXEL21_1 PIXEL22_1D
            acase 58:
                if (Diff(w4, w2))
                {   PIXEL00_1M
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_1M
                } else
                {   PIXEL02_2
                }
                PIXEL10_C
                PIXEL12_C
                PIXEL20_1D
                PIXEL21_1
                PIXEL22_1M
            acase 83:
                PIXEL00_1L
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_1M
                } else
                {   PIXEL02_2
                }
                PIXEL10_1
                PIXEL12_C
                PIXEL20_1M
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_1M
                } else
                {   PIXEL22_2
                }
            acase 92:
                PIXEL00_1M
                PIXEL01_1
                PIXEL02_1U
                PIXEL10_C
                PIXEL12_C
                if (Diff(w8, w4))
                {   PIXEL20_1M
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_1M
                } else
                {   PIXEL22_2
                }
            acase 202:
                if (Diff(w4, w2))
                {   PIXEL00_1M
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                PIXEL02_1M
                PIXEL10_C
                PIXEL12_1
                if (Diff(w8, w4))
                {   PIXEL20_1M
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                PIXEL22_1R
            acase 78:
                if (Diff(w4, w2))
                {   PIXEL00_1M
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                PIXEL02_1R
                PIXEL10_C
                PIXEL12_1
                if (Diff(w8, w4))
                {   PIXEL20_1M
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                PIXEL22_1M
            acase 154:
                if (Diff(w4, w2))
                {   PIXEL00_1M
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_1M
                } else
                {   PIXEL02_2
                }
                PIXEL10_C
                PIXEL12_C
                PIXEL20_1M
                PIXEL21_1
                PIXEL22_1D
            acase 114:
                PIXEL00_1M
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_1M
                } else
                {   PIXEL02_2
                }
                PIXEL10_1
                PIXEL12_C
                PIXEL20_1L
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_1M
                } else
                {   PIXEL22_2
                }
            acase 89:
                PIXEL00_1U
                PIXEL01_1
                PIXEL02_1M
                PIXEL10_C
                PIXEL12_C
                if (Diff(w8, w4))
                {   PIXEL20_1M
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_1M
                } else
                {   PIXEL22_2
                }
            acase 90:
                if (Diff(w4, w2))
                {   PIXEL00_1M
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_1M
                } else
                {   PIXEL02_2
                }
                PIXEL10_C
                PIXEL12_C
                if (Diff(w8, w4))
                {   PIXEL20_1M
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_1M
                } else
                {   PIXEL22_2
                }
            acase 55:
            case 23:
                if (Diff(w2, w6))
                {   PIXEL00_1L
                    PIXEL01_C
                    PIXEL02_C
                    PIXEL12_C
                } else
                {   PIXEL00_2
                    PIXEL01_6
                    PIXEL02_5
                    PIXEL12_1
                }
                PIXEL10_1
                PIXEL20_2
                PIXEL21_1
                PIXEL22_1M
            acase 182:
            case 150:
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_C
                    PIXEL12_C
                    PIXEL22_1D
                } else
                {   PIXEL01_1
                    PIXEL02_5
                    PIXEL12_6
                    PIXEL22_2
                }
                PIXEL00_1M
                PIXEL10_1
                PIXEL20_2
                PIXEL21_1
            acase 213:
            case 212:
                if (Diff(w6, w8))
                {   PIXEL02_1U
                    PIXEL12_C
                    PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL02_2
                    PIXEL12_6
                    PIXEL21_1
                    PIXEL22_5
                }
                PIXEL00_2
                PIXEL01_1
                PIXEL10_1
                PIXEL20_1M
            acase 241:
            case 240:
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL20_1L
                    PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL12_1
                    PIXEL20_2
                    PIXEL21_6
                    PIXEL22_5
                }
                PIXEL00_2
                PIXEL01_1
                PIXEL02_1M
                PIXEL10_1
            acase 236:
            case 232:
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_C
                    PIXEL21_C
                    PIXEL22_1R
                } else
                {   PIXEL10_1
                    PIXEL20_5
                    PIXEL21_6
                    PIXEL22_2
                }
                PIXEL00_1M
                PIXEL01_1
                PIXEL02_2
                PIXEL12_1
            acase 109:
            case 105:
                if (Diff(w8, w4))
                {   PIXEL00_1U
                    PIXEL10_C
                    PIXEL20_C
                    PIXEL21_C
                } else
                {   PIXEL00_2
                    PIXEL10_6
                    PIXEL20_5
                    PIXEL21_1
                }
                PIXEL01_1
                PIXEL02_2
                PIXEL12_1
                PIXEL22_1M
            acase 171:
            case 43:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                    PIXEL10_C
                    PIXEL20_1D
                } else
                {   PIXEL00_5
                    PIXEL01_1
                    PIXEL10_6
                    PIXEL20_2
                }
                PIXEL02_1M
                PIXEL12_1
                PIXEL21_1
                PIXEL22_2
            acase 143:
            case 15:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_C
                } else
                {   PIXEL00_5
                    PIXEL01_6
                    PIXEL02_2
                    PIXEL10_1
                }
                PIXEL12_1
                PIXEL20_1M
                PIXEL21_1
                PIXEL22_2
            acase 124:
                PIXEL00_1M
                PIXEL01_1
                PIXEL02_1U
                PIXEL12_C
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_C
                    PIXEL21_C
                } else
                {   PIXEL10_3
                    PIXEL20_4
                    PIXEL21_3
                }
                PIXEL22_1M
            acase 203:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                    PIXEL10_C
                } else
                {   PIXEL00_4
                    PIXEL01_3
                    PIXEL10_3
                }
                PIXEL02_1M
                PIXEL12_1
                PIXEL20_1M
                PIXEL21_C
                PIXEL22_1R
            acase 62:
                PIXEL00_1M
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_C
                    PIXEL12_C
                } else
                {   PIXEL01_3
                    PIXEL02_4
                    PIXEL12_3
                }
                PIXEL10_C
                PIXEL20_1D
                PIXEL21_1
                PIXEL22_1M
            acase 211:
                PIXEL00_1L
                PIXEL01_C
                PIXEL02_1M
                PIXEL10_1
                PIXEL20_1M
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL12_3
                    PIXEL21_3
                    PIXEL22_4
                }
            acase 118:
                PIXEL00_1M
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_C
                    PIXEL12_C
                } else
                {   PIXEL01_3
                    PIXEL02_4
                    PIXEL12_3
                }
                PIXEL10_1
                PIXEL20_1L
                PIXEL21_C
                PIXEL22_1M
            acase 217:
                PIXEL00_1U
                PIXEL01_1
                PIXEL02_1M
                PIXEL10_C
                PIXEL20_1M
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL12_3
                    PIXEL21_3
                    PIXEL22_4
                }
            acase 110:
                PIXEL00_1M
                PIXEL01_C
                PIXEL02_1R
                PIXEL12_1
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_C
                    PIXEL21_C
                } else
                {   PIXEL10_3
                    PIXEL20_4
                    PIXEL21_3
                }
                PIXEL22_1M
            acase 155:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                    PIXEL10_C
                } else
                {   PIXEL00_4
                    PIXEL01_3
                    PIXEL10_3
                }
                PIXEL02_1M
                PIXEL12_C
                PIXEL20_1M
                PIXEL21_1
                PIXEL22_1D
            acase 188:
                PIXEL00_1M PIXEL01_1 PIXEL02_1U
                PIXEL10_C            PIXEL12_C
                PIXEL20_1D PIXEL21_1 PIXEL22_1D
            acase 185:
                PIXEL00_1U PIXEL01_1 PIXEL02_1M
                PIXEL10_C            PIXEL12_C
                PIXEL20_1D PIXEL21_1 PIXEL22_1D
            acase 61:
                PIXEL00_1U PIXEL01_1 PIXEL02_1U
                PIXEL10_C            PIXEL12_C
                PIXEL20_1D PIXEL21_1 PIXEL22_1M
            acase 157:
                PIXEL00_1U PIXEL01_1 PIXEL02_1U
                PIXEL10_C            PIXEL12_C
                PIXEL20_1M PIXEL21_1 PIXEL22_1D
            acase 103:
                PIXEL00_1L PIXEL01_C PIXEL02_1R
                PIXEL10_1            PIXEL12_1
                PIXEL20_1L PIXEL21_C PIXEL22_1M
            acase 227:
                PIXEL00_1L PIXEL01_C PIXEL02_1M
                PIXEL10_1            PIXEL12_1
                PIXEL20_1L PIXEL21_C PIXEL22_1R
            acase 230:
                PIXEL00_1M PIXEL01_C PIXEL02_1R
                PIXEL10_1            PIXEL12_1
                PIXEL20_1L PIXEL21_C PIXEL22_1R
            acase 199:
                PIXEL00_1L PIXEL01_C PIXEL02_1R
                PIXEL10_1            PIXEL12_1
                PIXEL20_1M PIXEL21_C PIXEL22_1R
            acase 220:
                PIXEL00_1M
                PIXEL01_1
                PIXEL02_1U
                PIXEL10_C
                if (Diff(w8, w4))
                {   PIXEL20_1M
                } else
                {   PIXEL20_2
                }
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL12_3
                    PIXEL21_3
                    PIXEL22_4
                }
            acase 158:
                if (Diff(w4, w2))
                {   PIXEL00_1M
                } else
                {   PIXEL00_2
                }
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_C
                    PIXEL12_C
                } else
                {   PIXEL01_3
                    PIXEL02_4
                    PIXEL12_3
                }
                PIXEL10_C
                PIXEL20_1M
                PIXEL21_1
                PIXEL22_1D
            acase 234:
                if (Diff(w4, w2))
                {   PIXEL00_1M
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                PIXEL02_1M
                PIXEL12_1
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_C
                    PIXEL21_C
                } else
                {   PIXEL10_3
                    PIXEL20_4
                    PIXEL21_3
                }
                PIXEL22_1R
            acase 242:
                PIXEL00_1M
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_1M
                } else
                {   PIXEL02_2
                }
                PIXEL10_1
                PIXEL20_1L
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL12_3
                    PIXEL21_3
                    PIXEL22_4
                }
            acase 59:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                    PIXEL10_C
                } else
                {   PIXEL00_4
                    PIXEL01_3
                    PIXEL10_3
                }
                if (Diff(w2, w6))
                {   PIXEL02_1M
                } else
                {   PIXEL02_2
                }
                PIXEL12_C
                PIXEL20_1D
                PIXEL21_1
                PIXEL22_1M
            acase 121:
                PIXEL00_1U
                PIXEL01_1
                PIXEL02_1M
                PIXEL12_C
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_C
                    PIXEL21_C
                } else
                {   PIXEL10_3
                    PIXEL20_4
                    PIXEL21_3
                }
                if (Diff(w6, w8))
                {   PIXEL22_1M
                } else
                {   PIXEL22_2
                }
            acase 87:
                PIXEL00_1L
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_C
                    PIXEL12_C
                } else
                {   PIXEL01_3
                    PIXEL02_4
                    PIXEL12_3
                }
                PIXEL10_1
                PIXEL20_1M
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_1M
                } else
                {   PIXEL22_2
                }
            acase 79:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                    PIXEL10_C
                } else
                {   PIXEL00_4
                    PIXEL01_3
                    PIXEL10_3
                }
                PIXEL02_1R
                PIXEL12_1
                if (Diff(w8, w4))
                {   PIXEL20_1M
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                PIXEL22_1M
            acase 122:
                if (Diff(w4, w2))
                {   PIXEL00_1M
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_1M
                } else
                {   PIXEL02_2
                }
                PIXEL12_C
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_C
                    PIXEL21_C
                } else
                {   PIXEL10_3
                    PIXEL20_4
                    PIXEL21_3
                }
                if (Diff(w6, w8))
                {   PIXEL22_1M
                } else
                {   PIXEL22_2
                }
            acase 94:
                if (Diff(w4, w2))
                {   PIXEL00_1M
                } else
                {   PIXEL00_2
                }
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_C
                    PIXEL12_C
                } else
                {   PIXEL01_3
                    PIXEL02_4
                    PIXEL12_3
                }
                PIXEL10_C
                if (Diff(w8, w4))
                {   PIXEL20_1M
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_1M
                } else
                {   PIXEL22_2
                }
            acase 218:
                if (Diff(w4, w2))
                {   PIXEL00_1M
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_1M
                } else
                {   PIXEL02_2
                }
                PIXEL10_C
                if (Diff(w8, w4))
                {   PIXEL20_1M
                } else
                {   PIXEL20_2
                }
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL12_3
                    PIXEL21_3
                    PIXEL22_4
                }
            acase 91:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                    PIXEL10_C
                } else
                {   PIXEL00_4
                    PIXEL01_3
                    PIXEL10_3
                }
                if (Diff(w2, w6))
                {   PIXEL02_1M
                } else
                {   PIXEL02_2
                }
                PIXEL12_C
                if (Diff(w8, w4))
                {   PIXEL20_1M
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_1M
                } else
                {   PIXEL22_2
                }
            acase 229:
                PIXEL00_2  PIXEL01_1 PIXEL02_2
                PIXEL10_1            PIXEL12_1
                PIXEL20_1L PIXEL21_C PIXEL22_1R
            acase 167:
                PIXEL00_1L PIXEL01_C PIXEL02_1R
                PIXEL10_1            PIXEL12_1
                PIXEL20_2  PIXEL21_1 PIXEL22_2
            acase 173:
                PIXEL00_1U PIXEL01_1 PIXEL02_2
                PIXEL10_C            PIXEL12_1
                PIXEL20_1D PIXEL21_1 PIXEL22_2
            acase 181:
                PIXEL00_2  PIXEL01_1 PIXEL02_1U
                PIXEL10_1            PIXEL12_C
                PIXEL20_2  PIXEL21_1 PIXEL22_1D
            acase 186:
                if (Diff(w4, w2))
                {   PIXEL00_1M
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_1M
                } else
                {   PIXEL02_2
                }
                PIXEL10_C
                PIXEL12_C
                PIXEL20_1D
                PIXEL21_1
                PIXEL22_1D
            acase 115:
                PIXEL00_1L
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_1M
                } else
                {   PIXEL02_2
                }
                PIXEL10_1
                PIXEL12_C
                PIXEL20_1L
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_1M
                } else
                {   PIXEL22_2
                }
            acase 93:
                PIXEL00_1U
                PIXEL01_1
                PIXEL02_1U
                PIXEL10_C
                PIXEL12_C
                if (Diff(w8, w4))
                {   PIXEL20_1M
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_1M
                } else
                {   PIXEL22_2
                }
            acase 206:
                if (Diff(w4, w2))
                {   PIXEL00_1M
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                PIXEL02_1R
                PIXEL10_C
                PIXEL12_1
                if (Diff(w8, w4))
                {   PIXEL20_1M
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                PIXEL22_1R
            acase 205:
            case 201:
                PIXEL00_1U
                PIXEL01_1
                PIXEL02_2
                PIXEL10_C
                PIXEL12_1
                if (Diff(w8, w4))
                {   PIXEL20_1M
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                PIXEL22_1R
            acase 174:
            case 46:
                if (Diff(w4, w2))
                {   PIXEL00_1M
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                PIXEL02_1R
                PIXEL10_C
                PIXEL12_1
                PIXEL20_1D
                PIXEL21_1
                PIXEL22_2
            acase 179:
            case 147:
                PIXEL00_1L
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_1M
                } else
                {   PIXEL02_2
                }
                PIXEL10_1
                PIXEL12_C
                PIXEL20_2
                PIXEL21_1
                PIXEL22_1D
            acase 117:
            case 116:
                PIXEL00_2
                PIXEL01_1
                PIXEL02_1U
                PIXEL10_1
                PIXEL12_C
                PIXEL20_1L
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_1M
                } else
                {   PIXEL22_2
                }
            acase 189:
                PIXEL00_1U PIXEL01_1 PIXEL02_1U
                PIXEL10_C            PIXEL12_C
                PIXEL20_1D PIXEL21_1 PIXEL22_1D
            acase 231:
                PIXEL00_1L PIXEL01_C PIXEL02_1R
                PIXEL10_1            PIXEL12_1
                PIXEL20_1L PIXEL21_C PIXEL22_1R
            acase 126:
                PIXEL00_1M
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_C
                    PIXEL12_C
                } else
                {   PIXEL01_3
                    PIXEL02_4
                    PIXEL12_3
                }
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_C
                    PIXEL21_C
                } else
                {   PIXEL10_3
                    PIXEL20_4
                    PIXEL21_3
                }
                PIXEL22_1M
            acase 219:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                    PIXEL10_C
                } else
                {   PIXEL00_4
                    PIXEL01_3
                    PIXEL10_3
                }
                PIXEL02_1M
                PIXEL20_1M
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL12_3
                    PIXEL21_3
                    PIXEL22_4
                }
            acase 125:
                if (Diff(w8, w4))
                {   PIXEL00_1U
                    PIXEL10_C
                    PIXEL20_C
                    PIXEL21_C
                } else
                {   PIXEL00_2
                    PIXEL10_6
                    PIXEL20_5
                    PIXEL21_1
                }
                PIXEL01_1
                PIXEL02_1U
                PIXEL12_C
                PIXEL22_1M
            acase 221:
                if (Diff(w6, w8))
                {   PIXEL02_1U
                    PIXEL12_C
                    PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL02_2
                    PIXEL12_6
                    PIXEL21_1
                    PIXEL22_5
                }
                PIXEL00_1U
                PIXEL01_1
                PIXEL10_C
                PIXEL20_1M
            acase 207:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_C
                } else
                {   PIXEL00_5
                    PIXEL01_6
                    PIXEL02_2
                    PIXEL10_1
                }
                PIXEL12_1
                PIXEL20_1M
                PIXEL21_C
                PIXEL22_1R
            acase 238:
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_C
                    PIXEL21_C
                    PIXEL22_1R
                } else
                {   PIXEL10_1
                    PIXEL20_5
                    PIXEL21_6
                    PIXEL22_2
                }
                PIXEL00_1M
                PIXEL01_C
                PIXEL02_1R
                PIXEL12_1
            acase 190:
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_C
                    PIXEL12_C
                    PIXEL22_1D
                } else
                {   PIXEL01_1
                    PIXEL02_5
                    PIXEL12_6
                    PIXEL22_2
                }
                PIXEL00_1M
                PIXEL10_C
                PIXEL20_1D
                PIXEL21_1
            acase 187:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                    PIXEL10_C
                    PIXEL20_1D
                } else
                {   PIXEL00_5
                    PIXEL01_1
                    PIXEL10_6
                    PIXEL20_2
                }
                PIXEL02_1M
                PIXEL12_C
                PIXEL21_1
                PIXEL22_1D
            acase 243:
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL20_1L
                    PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL12_1
                    PIXEL20_2
                    PIXEL21_6
                    PIXEL22_5
                }
                PIXEL00_1L
                PIXEL01_C
                PIXEL02_1M
                PIXEL10_1
            acase 119:
                if (Diff(w2, w6))
                {   PIXEL00_1L
                    PIXEL01_C
                    PIXEL02_C
                    PIXEL12_C
                } else
                {   PIXEL00_2
                    PIXEL01_6
                    PIXEL02_5
                    PIXEL12_1
                }
                PIXEL10_1
                PIXEL20_1L
                PIXEL21_C
                PIXEL22_1M
            acase 237:
            case 233:
                PIXEL00_1U
                PIXEL01_1
                PIXEL02_2
                PIXEL10_C
                PIXEL12_1
                if (Diff(w8, w4))
                {   PIXEL20_C
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                PIXEL22_1R
            acase 175:
            case 47:
                if (Diff(w4, w2))
                {   PIXEL00_C
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                PIXEL02_1R
                PIXEL10_C
                PIXEL12_1
                PIXEL20_1D
                PIXEL21_1
                PIXEL22_2
            acase 183:
            case 151:
                PIXEL00_1L
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_C
                } else
                {   PIXEL02_2
                }
                PIXEL10_1
                PIXEL12_C
                PIXEL20_2
                PIXEL21_1
                PIXEL22_1D
            acase 245:
            case 244:
                PIXEL00_2
                PIXEL01_1
                PIXEL02_1U
                PIXEL10_1
                PIXEL12_C
                PIXEL20_1L
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_C
                } else
                {   PIXEL22_2
                }
            acase 250:
                PIXEL00_1M
                PIXEL01_C
                PIXEL02_1M
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_C
                } else
                {   PIXEL10_3
                    PIXEL20_4
                }
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL22_C
                } else
                {   PIXEL12_3
                    PIXEL22_4
                }
            acase 123:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                } else
                {   PIXEL00_4
                    PIXEL01_3
                }
                PIXEL02_1M
                PIXEL10_C
                PIXEL12_C
                if (Diff(w8, w4))
                {   PIXEL20_C
                    PIXEL21_C
                } else
                {   PIXEL20_4
                    PIXEL21_3
                }
                PIXEL22_1M
            acase 95:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL10_C
                } else
                {   PIXEL00_4
                    PIXEL10_3
                }
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_C
                    PIXEL12_C
                } else
                {   PIXEL02_4
                    PIXEL12_3
                }
                PIXEL20_1M
                PIXEL21_C
                PIXEL22_1M
            acase 222:
                PIXEL00_1M
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_C
                } else
                {   PIXEL01_3
                    PIXEL02_4
                }
                PIXEL10_C
                PIXEL12_C
                PIXEL20_1M
                if (Diff(w6, w8))
                {   PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL21_3
                    PIXEL22_4
                }
            acase 252:
                PIXEL00_1M
                PIXEL01_1
                PIXEL02_1U
                PIXEL12_C
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_C
                } else
                {   PIXEL10_3
                    PIXEL20_4
                }
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_C
                } else
                {   PIXEL22_2
                }
            acase 249:
                PIXEL00_1U
                PIXEL01_1
                PIXEL02_1M
                PIXEL10_C
                if (Diff(w8, w4))
                {   PIXEL20_C
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL22_C
                } else
                {   PIXEL12_3
                    PIXEL22_4
                }
            acase 235:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                } else
                {   PIXEL00_4
                    PIXEL01_3
                }
                PIXEL02_1M
                PIXEL10_C
                PIXEL12_1
                if (Diff(w8, w4))
                {   PIXEL20_C
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                PIXEL22_1R
            acase 111:
                if (Diff(w4, w2))
                {   PIXEL00_C
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                PIXEL02_1R
                PIXEL10_C
                PIXEL12_1
                if (Diff(w8, w4))
                {   PIXEL20_C
                    PIXEL21_C
                } else
                {   PIXEL20_4
                    PIXEL21_3
                }
                PIXEL22_1M
            acase 63:
                if (Diff(w4, w2))
                {   PIXEL00_C
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_C
                    PIXEL12_C
                } else
                {   PIXEL02_4
                    PIXEL12_3
                }
                PIXEL10_C
                PIXEL20_1D
                PIXEL21_1
                PIXEL22_1M
            acase 159:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL10_C
                } else
                {   PIXEL00_4
                    PIXEL10_3
                }
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_C
                } else
                {   PIXEL02_2
                }
                PIXEL12_C
                PIXEL20_1M
                PIXEL21_1
                PIXEL22_1D
            acase 215:
                PIXEL00_1L
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_C
                } else
                {   PIXEL02_2
                }
                PIXEL10_1
                PIXEL12_C
                PIXEL20_1M
                if (Diff(w6, w8))
                {   PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL21_3
                    PIXEL22_4
                }
            acase 246:
                PIXEL00_1M
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_C
                } else
                {   PIXEL01_3
                    PIXEL02_4
                }
                PIXEL10_1
                PIXEL12_C
                PIXEL20_1L
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_C
                } else
                {   PIXEL22_2
                }
            acase 254:
                PIXEL00_1M
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_C
                } else
                {   PIXEL01_3
                    PIXEL02_4
                }
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_C
                } else
                {   PIXEL10_3
                    PIXEL20_4
                }
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL12_3
                    PIXEL21_3
                    PIXEL22_2
                }
            acase 253:
                PIXEL00_1U
                PIXEL01_1
                PIXEL02_1U
                PIXEL10_C
                PIXEL12_C
                if (Diff(w8, w4))
                {   PIXEL20_C
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_C
                } else
                {   PIXEL22_2
                }
            acase 251:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                } else
                {   PIXEL00_4
                    PIXEL01_3
                }
                PIXEL02_1M
                if (Diff(w8, w4))
                {   PIXEL10_C
                    PIXEL20_C
                    PIXEL21_C
                } else
                {   PIXEL10_3
                    PIXEL20_2
                    PIXEL21_3
                }
                if (Diff(w6, w8))
                {   PIXEL12_C
                    PIXEL22_C
                } else
                {   PIXEL12_3
                    PIXEL22_4
                }
            acase 239:
                if (Diff(w4, w2))
                {   PIXEL00_C
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                PIXEL02_1R
                PIXEL10_C
                PIXEL12_1
                if (Diff(w8, w4))
                {   PIXEL20_C
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                PIXEL22_1R
            acase 127:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL01_C
                    PIXEL10_C
                } else
                {   PIXEL00_2
                    PIXEL01_3
                    PIXEL10_3
                }
                if (Diff(w2, w6))
                {   PIXEL02_C
                    PIXEL12_C
                } else
                {   PIXEL02_4
                    PIXEL12_3
                }
                if (Diff(w8, w4))
                {   PIXEL20_C
                    PIXEL21_C
                } else
                {   PIXEL20_4
                    PIXEL21_3
                }
                PIXEL22_1M
            acase 191:
                if (Diff(w4, w2))
                {   PIXEL00_C
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_C
                } else
                {   PIXEL02_2
                }
                PIXEL10_C
                PIXEL12_C
                PIXEL20_1D
                PIXEL21_1
                PIXEL22_1D
            acase 223:
                if (Diff(w4, w2))
                {   PIXEL00_C
                    PIXEL10_C
                } else
                {   PIXEL00_4
                    PIXEL10_3
                }
                if (Diff(w2, w6))
                {   PIXEL01_C
                    PIXEL02_C
                    PIXEL12_C
                } else
                {   PIXEL01_3
                    PIXEL02_2
                    PIXEL12_3
                }
                PIXEL20_1M
                if (Diff(w6, w8))
                {   PIXEL21_C
                    PIXEL22_C
                } else
                {   PIXEL21_3
                    PIXEL22_4
                }
            acase 247:
                PIXEL00_1L
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_C
                } else
                {   PIXEL02_2
                }
                PIXEL10_1
                PIXEL12_C
                PIXEL20_1L
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_C
                } else
                {   PIXEL22_2
                }
            acase 255:
                if (Diff(w4, w2))
                {   PIXEL00_C
                } else
                {   PIXEL00_2
                }
                PIXEL01_C
                if (Diff(w2, w6))
                {   PIXEL02_C
                } else
                {   PIXEL02_2
                }
                PIXEL10_C
                PIXEL12_C
                if (Diff(w8, w4))
                {   PIXEL20_C
                } else
                {   PIXEL20_2
                }
                PIXEL21_C
                if (Diff(w6, w8))
                {   PIXEL22_C
                } else
                {   PIXEL22_2
            }   }

            switch (fastsize)
            {
            case 4: // 3*3
                pp = (x * 3) + (y * 9 * machines[machine].width);
                COPYDISP2(    pp                                     , d1);
                COPYDISP2(    pp + 1                                 , d2);
                COPYDISP2(    pp + 2                                 , d3);
                COPYDISP2(    pp +     (machines[machine].width *  3), d4);
                COPYDISP2(    pp + 1 + (machines[machine].width *  3), d5);
                COPYDISP2(    pp + 2 + (machines[machine].width *  3), d6);
                COPYDISP2(    pp +     (machines[machine].width *  6), d7);
                COPYDISP2(    pp + 1 + (machines[machine].width *  6), d8);
                COPYDISP2(    pp + 2 + (machines[machine].width *  6), d9);
            acase 5: // 6*3
                pp = (x * 6) + (y * 18 * machines[machine].width);
                COPYWIDEDISP2(pp                                     , d1);
                COPYWIDEDISP2(pp + 2                                 , d2);
                COPYWIDEDISP2(pp + 4                                 , d3);
                COPYWIDEDISP2(pp +     (machines[machine].width *  6), d4);
                COPYWIDEDISP2(pp + 2 + (machines[machine].width *  6), d5);
                COPYWIDEDISP2(pp + 4 + (machines[machine].width *  6), d6);
                COPYWIDEDISP2(pp +     (machines[machine].width * 12), d7);
                COPYWIDEDISP2(pp + 2 + (machines[machine].width * 12), d8);
                COPYWIDEDISP2(pp + 4 + (machines[machine].width * 12), d9);
}   }   }   }

EXPORT void apply_hq4x(void)
{   FAST int   flag,
               newx, newy,
               pattern,
               x, y;
    FAST ULONG d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15, d16,
               pp,
               w1, w2, w3, w4, w5, w6, w7, w8, w9,
               yuv1, yuv2;

    hqxInit();

    for (x = 0; x < machines[machine].width; x++)
    {   for (y = 0; y < machines[machine].height; y++)
        {   if (rotating)
            {   newx = y;
                newy = widthheight - x;
                w1 = pencolours[colourset][screen[WEST(newx)][SOUTH(newy)]];
                w2 = pencolours[colourset][screen[WEST(newx)][      newy ]];
                w3 = pencolours[colourset][screen[WEST(newx)][NORTH(newy)]];
                w4 = pencolours[colourset][screen[     newx ][SOUTH(newy)]];
                w5 = pencolours[colourset][screen[     newx ][      newy ]];
                w6 = pencolours[colourset][screen[     newx ][NORTH(newy)]];
                w7 = pencolours[colourset][screen[EAST(newx)][SOUTH(newy)]];
                w8 = pencolours[colourset][screen[EAST(newx)][      newy ]];
                w9 = pencolours[colourset][screen[EAST(newx)][NORTH(newy)]];
            } else
            {   w1 = pencolours[colourset][screen[WEST(   x)][NORTH(   y)]];
                w2 = pencolours[colourset][screen[        x ][NORTH(   y)]];
                w3 = pencolours[colourset][screen[EAST(   x)][NORTH(   y)]];
                w4 = pencolours[colourset][screen[WEST(   x)][         y ]];
                w5 = pencolours[colourset][screen[        x ][         y ]];
                w6 = pencolours[colourset][screen[EAST(   x)][         y ]];
                w7 = pencolours[colourset][screen[WEST(   x)][SOUTH(   y)]];
                w8 = pencolours[colourset][screen[        x ][SOUTH(   y)]];
                w9 = pencolours[colourset][screen[EAST(   x)][SOUTH(   y)]];
            }

            pattern = 0;
            flag = 1;

            yuv1 = RGBtoYUV[w5];

            if (w1 != w5) { yuv2 = RGBtoYUV[w1]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w2 != w5) { yuv2 = RGBtoYUV[w2]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w3 != w5) { yuv2 = RGBtoYUV[w3]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w4 != w5) { yuv2 = RGBtoYUV[w4]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w6 != w5) { yuv2 = RGBtoYUV[w6]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w7 != w5) { yuv2 = RGBtoYUV[w7]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w8 != w5) { yuv2 = RGBtoYUV[w8]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;
            if (w9 != w5) { yuv2 = RGBtoYUV[w9]; if (yuv_diff(yuv1, yuv2)) { pattern |= flag; } } flag <<= 1;

            switch (pattern)
            {
            case 0:
            case 1:
            case 4:
            case 32:
            case 128:
            case 5:
            case 132:
            case 160:
            case 33:
            case 129:
            case 36:
            case 133:
            case 164:
            case 161:
            case 37:
            case 165:
                HQ4X_PIXEL00_20 HQ4X_PIXEL01_60 HQ4X_PIXEL02_60 HQ4X_PIXEL03_20
                HQ4X_PIXEL10_60 HQ4X_PIXEL11_70 HQ4X_PIXEL12_70 HQ4X_PIXEL13_60
                HQ4X_PIXEL20_60 HQ4X_PIXEL21_70 HQ4X_PIXEL22_70 HQ4X_PIXEL23_60
                HQ4X_PIXEL30_20 HQ4X_PIXEL31_60 HQ4X_PIXEL32_60 HQ4X_PIXEL33_20
            acase 2:
            case 34:
            case 130:
            case 162:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_10 HQ4X_PIXEL02_10 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_61 HQ4X_PIXEL11_30 HQ4X_PIXEL12_30 HQ4X_PIXEL13_61
                HQ4X_PIXEL20_60 HQ4X_PIXEL21_70 HQ4X_PIXEL22_70 HQ4X_PIXEL23_60
                HQ4X_PIXEL30_20 HQ4X_PIXEL31_60 HQ4X_PIXEL32_60 HQ4X_PIXEL33_20
            acase 16:
            case 17:
            case 48:
            case 49:
                HQ4X_PIXEL00_20 HQ4X_PIXEL01_60 HQ4X_PIXEL02_61 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_60 HQ4X_PIXEL11_70 HQ4X_PIXEL12_30 HQ4X_PIXEL13_10
                HQ4X_PIXEL20_60 HQ4X_PIXEL21_70 HQ4X_PIXEL22_30 HQ4X_PIXEL23_10
                HQ4X_PIXEL30_20 HQ4X_PIXEL31_60 HQ4X_PIXEL32_61 HQ4X_PIXEL33_80
            acase 64:
            case 65:
            case 68:
            case 69:
                HQ4X_PIXEL00_20 HQ4X_PIXEL01_60 HQ4X_PIXEL02_60 HQ4X_PIXEL03_20
                HQ4X_PIXEL10_60 HQ4X_PIXEL11_70 HQ4X_PIXEL12_70 HQ4X_PIXEL13_60
                HQ4X_PIXEL20_61 HQ4X_PIXEL21_30 HQ4X_PIXEL22_30 HQ4X_PIXEL23_61
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_10 HQ4X_PIXEL32_10 HQ4X_PIXEL33_80
            acase 8:
            case 12:
            case 136:
            case 140:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_61 HQ4X_PIXEL02_60 HQ4X_PIXEL03_20
                HQ4X_PIXEL10_10 HQ4X_PIXEL11_30 HQ4X_PIXEL12_70 HQ4X_PIXEL13_60
                HQ4X_PIXEL20_10 HQ4X_PIXEL21_30 HQ4X_PIXEL22_70 HQ4X_PIXEL23_60
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_61 HQ4X_PIXEL32_60 HQ4X_PIXEL33_20
            acase 3:
            case 35:
            case 131:
            case 163:
                HQ4X_PIXEL00_81 HQ4X_PIXEL01_31 HQ4X_PIXEL02_10 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_81 HQ4X_PIXEL11_31 HQ4X_PIXEL12_30 HQ4X_PIXEL13_61
                HQ4X_PIXEL20_60 HQ4X_PIXEL21_70 HQ4X_PIXEL22_70 HQ4X_PIXEL23_60
                HQ4X_PIXEL30_20 HQ4X_PIXEL31_60 HQ4X_PIXEL32_60 HQ4X_PIXEL33_20
            acase 6:
            case 38:
            case 134:
            case 166:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_10 HQ4X_PIXEL02_32 HQ4X_PIXEL03_82
                HQ4X_PIXEL10_61 HQ4X_PIXEL11_30 HQ4X_PIXEL12_32 HQ4X_PIXEL13_82
                HQ4X_PIXEL20_60 HQ4X_PIXEL21_70 HQ4X_PIXEL22_70 HQ4X_PIXEL23_60
                HQ4X_PIXEL30_20 HQ4X_PIXEL31_60 HQ4X_PIXEL32_60 HQ4X_PIXEL33_20
            acase 20:
            case 21:
            case 52:
            case 53:
                HQ4X_PIXEL00_20 HQ4X_PIXEL01_60 HQ4X_PIXEL02_81 HQ4X_PIXEL03_81
                HQ4X_PIXEL10_60 HQ4X_PIXEL11_70 HQ4X_PIXEL12_31 HQ4X_PIXEL13_31
                HQ4X_PIXEL20_60 HQ4X_PIXEL21_70 HQ4X_PIXEL22_30 HQ4X_PIXEL23_10
                HQ4X_PIXEL30_20 HQ4X_PIXEL31_60 HQ4X_PIXEL32_61 HQ4X_PIXEL33_80
            acase 144:
            case 145:
            case 176:
            case 177:
                HQ4X_PIXEL00_20 HQ4X_PIXEL01_60 HQ4X_PIXEL02_61 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_60 HQ4X_PIXEL11_70 HQ4X_PIXEL12_30 HQ4X_PIXEL13_10
                HQ4X_PIXEL20_60 HQ4X_PIXEL21_70 HQ4X_PIXEL22_32 HQ4X_PIXEL23_32
                HQ4X_PIXEL30_20 HQ4X_PIXEL31_60 HQ4X_PIXEL32_82 HQ4X_PIXEL33_82
            acase 192:
            case 193:
            case 196:
            case 197:
                HQ4X_PIXEL00_20 HQ4X_PIXEL01_60 HQ4X_PIXEL02_60 HQ4X_PIXEL03_20
                HQ4X_PIXEL10_60 HQ4X_PIXEL11_70 HQ4X_PIXEL12_70 HQ4X_PIXEL13_60
                HQ4X_PIXEL20_61 HQ4X_PIXEL21_30 HQ4X_PIXEL22_31 HQ4X_PIXEL23_81
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_10 HQ4X_PIXEL32_31 HQ4X_PIXEL33_81
            acase 96:
            case 97:
            case 100:
            case 101:
                HQ4X_PIXEL00_20 HQ4X_PIXEL01_60 HQ4X_PIXEL02_60 HQ4X_PIXEL03_20
                HQ4X_PIXEL10_60 HQ4X_PIXEL11_70 HQ4X_PIXEL12_70 HQ4X_PIXEL13_60
                HQ4X_PIXEL20_82 HQ4X_PIXEL21_32 HQ4X_PIXEL22_30 HQ4X_PIXEL23_61
                HQ4X_PIXEL30_82 HQ4X_PIXEL31_32 HQ4X_PIXEL32_10 HQ4X_PIXEL33_80
            acase 40:
            case 44:
            case 168:
            case 172:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_61 HQ4X_PIXEL02_60 HQ4X_PIXEL03_20
                HQ4X_PIXEL10_10 HQ4X_PIXEL11_30 HQ4X_PIXEL12_70 HQ4X_PIXEL13_60
                HQ4X_PIXEL20_31 HQ4X_PIXEL21_31 HQ4X_PIXEL22_70 HQ4X_PIXEL23_60
                HQ4X_PIXEL30_81 HQ4X_PIXEL31_81 HQ4X_PIXEL32_60 HQ4X_PIXEL33_20
            acase 9:
            case 13:
            case 137:
            case 141:
                HQ4X_PIXEL00_82 HQ4X_PIXEL01_82 HQ4X_PIXEL02_60 HQ4X_PIXEL03_20
                HQ4X_PIXEL10_32 HQ4X_PIXEL11_32 HQ4X_PIXEL12_70 HQ4X_PIXEL13_60
                HQ4X_PIXEL20_10 HQ4X_PIXEL21_30 HQ4X_PIXEL22_70 HQ4X_PIXEL23_60
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_61 HQ4X_PIXEL32_60 HQ4X_PIXEL33_20
            acase 18:
            case 50:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_10
                    HQ4X_PIXEL03_80
                    HQ4X_PIXEL12_30
                    HQ4X_PIXEL13_10
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL10_61
                HQ4X_PIXEL11_30
                HQ4X_PIXEL20_60
                HQ4X_PIXEL21_70
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL30_20
                HQ4X_PIXEL31_60
                HQ4X_PIXEL32_61
                HQ4X_PIXEL33_80
            acase 80:
            case 81:
                HQ4X_PIXEL00_20
                HQ4X_PIXEL01_60
                HQ4X_PIXEL02_61
                HQ4X_PIXEL03_80
                HQ4X_PIXEL10_60
                HQ4X_PIXEL11_70
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                HQ4X_PIXEL20_61
                HQ4X_PIXEL21_30
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_30
                    HQ4X_PIXEL23_10
                    HQ4X_PIXEL32_10
                    HQ4X_PIXEL33_80
                } else
                {   HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
            acase 72:
            case 76:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_61
                HQ4X_PIXEL02_60
                HQ4X_PIXEL03_20
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_70
                HQ4X_PIXEL13_60
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_10
                    HQ4X_PIXEL21_30
                    HQ4X_PIXEL30_80
                    HQ4X_PIXEL31_10
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_61
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 10:
            case 138:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_80
                    HQ4X_PIXEL01_10
                    HQ4X_PIXEL10_10
                    HQ4X_PIXEL11_30
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                    HQ4X_PIXEL11_0
                }
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_61
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_70
                HQ4X_PIXEL23_60
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_61
                HQ4X_PIXEL32_60
                HQ4X_PIXEL33_20
            acase 66:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_10 HQ4X_PIXEL02_10 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_61 HQ4X_PIXEL11_30 HQ4X_PIXEL12_30 HQ4X_PIXEL13_61
                HQ4X_PIXEL20_61 HQ4X_PIXEL21_30 HQ4X_PIXEL22_30 HQ4X_PIXEL23_61
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_10 HQ4X_PIXEL32_10 HQ4X_PIXEL33_80
            acase 24:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_61 HQ4X_PIXEL02_61 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_10 HQ4X_PIXEL11_30 HQ4X_PIXEL12_30 HQ4X_PIXEL13_10
                HQ4X_PIXEL20_10 HQ4X_PIXEL21_30 HQ4X_PIXEL22_30 HQ4X_PIXEL23_10
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_61 HQ4X_PIXEL32_61 HQ4X_PIXEL33_80
            acase 7:
            case 39:
            case 135:
                HQ4X_PIXEL00_81 HQ4X_PIXEL01_31 HQ4X_PIXEL02_32 HQ4X_PIXEL03_82
                HQ4X_PIXEL10_81 HQ4X_PIXEL11_31 HQ4X_PIXEL12_32 HQ4X_PIXEL13_82
                HQ4X_PIXEL20_60 HQ4X_PIXEL21_70 HQ4X_PIXEL22_70 HQ4X_PIXEL23_60
                HQ4X_PIXEL30_20 HQ4X_PIXEL31_60 HQ4X_PIXEL32_60 HQ4X_PIXEL33_20
            acase 148:
            case 149:
            case 180:
                HQ4X_PIXEL00_20 HQ4X_PIXEL01_60 HQ4X_PIXEL02_81 HQ4X_PIXEL03_81
                HQ4X_PIXEL10_60 HQ4X_PIXEL11_70 HQ4X_PIXEL12_31 HQ4X_PIXEL13_31
                HQ4X_PIXEL20_60 HQ4X_PIXEL21_70 HQ4X_PIXEL22_32 HQ4X_PIXEL23_32
                HQ4X_PIXEL30_20 HQ4X_PIXEL31_60 HQ4X_PIXEL32_82 HQ4X_PIXEL33_82
            acase 224:
            case 228:
            case 225:
                HQ4X_PIXEL00_20 HQ4X_PIXEL01_60 HQ4X_PIXEL02_60 HQ4X_PIXEL03_20
                HQ4X_PIXEL10_60 HQ4X_PIXEL11_70 HQ4X_PIXEL12_70 HQ4X_PIXEL13_60
                HQ4X_PIXEL20_82 HQ4X_PIXEL21_32 HQ4X_PIXEL22_31 HQ4X_PIXEL23_81
                HQ4X_PIXEL30_82 HQ4X_PIXEL31_32 HQ4X_PIXEL32_31 HQ4X_PIXEL33_81
            acase 41:
            case 169:
            case 45:
                HQ4X_PIXEL00_82 HQ4X_PIXEL01_82 HQ4X_PIXEL02_60 HQ4X_PIXEL03_20
                HQ4X_PIXEL10_32 HQ4X_PIXEL11_32 HQ4X_PIXEL12_70 HQ4X_PIXEL13_60
                HQ4X_PIXEL20_31 HQ4X_PIXEL21_31 HQ4X_PIXEL22_70 HQ4X_PIXEL23_60
                HQ4X_PIXEL30_81 HQ4X_PIXEL31_81 HQ4X_PIXEL32_60 HQ4X_PIXEL33_20
            acase 22:
            case 54:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL10_61
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_0
                HQ4X_PIXEL20_60
                HQ4X_PIXEL21_70
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL30_20
                HQ4X_PIXEL31_60
                HQ4X_PIXEL32_61
                HQ4X_PIXEL33_80
            acase 208:
            case 209:
                HQ4X_PIXEL00_20
                HQ4X_PIXEL01_60
                HQ4X_PIXEL02_61
                HQ4X_PIXEL03_80
                HQ4X_PIXEL10_60
                HQ4X_PIXEL11_70
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                HQ4X_PIXEL20_61
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
            acase 104:
            case 108:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_61
                HQ4X_PIXEL02_60
                HQ4X_PIXEL03_20
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_70
                HQ4X_PIXEL13_60
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_61
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 11:
            case 139:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_61
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_70
                HQ4X_PIXEL23_60
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_61
                HQ4X_PIXEL32_60
                HQ4X_PIXEL33_20
            acase 19:
            case 51:
                if (Diff(w2, w6))
                {   HQ4X_PIXEL00_81
                    HQ4X_PIXEL01_31
                    HQ4X_PIXEL02_10
                    HQ4X_PIXEL03_80
                    HQ4X_PIXEL12_30
                    HQ4X_PIXEL13_10
                } else
                {   HQ4X_PIXEL00_12
                    HQ4X_PIXEL01_14
                    HQ4X_PIXEL02_83
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL12_70
                    HQ4X_PIXEL13_21
                }
                HQ4X_PIXEL10_81
                HQ4X_PIXEL11_31
                HQ4X_PIXEL20_60
                HQ4X_PIXEL21_70
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL30_20
                HQ4X_PIXEL31_60
                HQ4X_PIXEL32_61
                HQ4X_PIXEL33_80
            acase 146:
            case 178:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_10
                    HQ4X_PIXEL03_80
                    HQ4X_PIXEL12_30
                    HQ4X_PIXEL13_10
                    HQ4X_PIXEL23_32
                    HQ4X_PIXEL33_82
                } else
                {   HQ4X_PIXEL02_21
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL12_70
                    HQ4X_PIXEL13_83
                    HQ4X_PIXEL23_13
                    HQ4X_PIXEL33_11
                }
                HQ4X_PIXEL10_61
                HQ4X_PIXEL11_30
                HQ4X_PIXEL20_60
                HQ4X_PIXEL21_70
                HQ4X_PIXEL22_32
                HQ4X_PIXEL30_20
                HQ4X_PIXEL31_60
                HQ4X_PIXEL32_82
            acase 84:
            case 85:
                HQ4X_PIXEL00_20
                HQ4X_PIXEL01_60
                HQ4X_PIXEL02_81
                if (Diff(w6, w8))
                {   HQ4X_PIXEL03_81
                    HQ4X_PIXEL13_31
                    HQ4X_PIXEL22_30
                    HQ4X_PIXEL23_10
                    HQ4X_PIXEL32_10
                    HQ4X_PIXEL33_80
                } else
                {   HQ4X_PIXEL03_12
                    HQ4X_PIXEL13_14
                    HQ4X_PIXEL22_70
                    HQ4X_PIXEL23_83
                    HQ4X_PIXEL32_21
                    HQ4X_PIXEL33_50
                }
                HQ4X_PIXEL10_60
                HQ4X_PIXEL11_70
                HQ4X_PIXEL12_31
                HQ4X_PIXEL20_61
                HQ4X_PIXEL21_30
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
            acase 112:
            case 113:
                HQ4X_PIXEL00_20
                HQ4X_PIXEL01_60
                HQ4X_PIXEL02_61
                HQ4X_PIXEL03_80
                HQ4X_PIXEL10_60
                HQ4X_PIXEL11_70
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                HQ4X_PIXEL20_82
                HQ4X_PIXEL21_32
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_30
                    HQ4X_PIXEL23_10
                    HQ4X_PIXEL30_82
                    HQ4X_PIXEL31_32
                    HQ4X_PIXEL32_10
                    HQ4X_PIXEL33_80
                } else
                {   HQ4X_PIXEL22_70
                    HQ4X_PIXEL23_21
                    HQ4X_PIXEL30_11
                    HQ4X_PIXEL31_13
                    HQ4X_PIXEL32_83
                    HQ4X_PIXEL33_50
                }
            acase 200:
            case 204:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_61
                HQ4X_PIXEL02_60
                HQ4X_PIXEL03_20
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_70
                HQ4X_PIXEL13_60
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_10
                    HQ4X_PIXEL21_30
                    HQ4X_PIXEL30_80
                    HQ4X_PIXEL31_10
                    HQ4X_PIXEL32_31
                    HQ4X_PIXEL33_81
                } else
                {   HQ4X_PIXEL20_21
                    HQ4X_PIXEL21_70
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_83
                    HQ4X_PIXEL32_14
                    HQ4X_PIXEL33_12
                }
                HQ4X_PIXEL22_31
                HQ4X_PIXEL23_81
            acase 73:
            case 77:
                if (Diff(w8, w4))
                {   HQ4X_PIXEL00_82
                    HQ4X_PIXEL10_32
                    HQ4X_PIXEL20_10
                    HQ4X_PIXEL21_30
                    HQ4X_PIXEL30_80
                    HQ4X_PIXEL31_10
                } else
                {   HQ4X_PIXEL00_11
                    HQ4X_PIXEL10_13
                    HQ4X_PIXEL20_83
                    HQ4X_PIXEL21_70
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_21
                }
                HQ4X_PIXEL01_82
                HQ4X_PIXEL02_60
                HQ4X_PIXEL03_20
                HQ4X_PIXEL11_32
                HQ4X_PIXEL12_70
                HQ4X_PIXEL13_60
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_61
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 42:
            case 170:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_80
                    HQ4X_PIXEL01_10
                    HQ4X_PIXEL10_10
                    HQ4X_PIXEL11_30
                    HQ4X_PIXEL20_31
                    HQ4X_PIXEL30_81
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_21
                    HQ4X_PIXEL10_83
                    HQ4X_PIXEL11_70
                    HQ4X_PIXEL20_14
                    HQ4X_PIXEL30_12
                }
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_61
                HQ4X_PIXEL21_31
                HQ4X_PIXEL22_70
                HQ4X_PIXEL23_60
                HQ4X_PIXEL31_81
                HQ4X_PIXEL32_60
                HQ4X_PIXEL33_20
            acase 14:
            case 142:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_80
                    HQ4X_PIXEL01_10
                    HQ4X_PIXEL02_32
                    HQ4X_PIXEL03_82
                    HQ4X_PIXEL10_10
                    HQ4X_PIXEL11_30
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_83
                    HQ4X_PIXEL02_13
                    HQ4X_PIXEL03_11
                    HQ4X_PIXEL10_21
                    HQ4X_PIXEL11_70
                }
                HQ4X_PIXEL12_32
                HQ4X_PIXEL13_82
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_70
                HQ4X_PIXEL23_60
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_61
                HQ4X_PIXEL32_60
                HQ4X_PIXEL33_20
            acase 67:
                HQ4X_PIXEL00_81 HQ4X_PIXEL01_31 HQ4X_PIXEL02_10 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_81 HQ4X_PIXEL11_31 HQ4X_PIXEL12_30 HQ4X_PIXEL13_61
                HQ4X_PIXEL20_61 HQ4X_PIXEL21_30 HQ4X_PIXEL22_30 HQ4X_PIXEL23_61
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_10 HQ4X_PIXEL32_10 HQ4X_PIXEL33_80
            acase 70:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_10 HQ4X_PIXEL02_32 HQ4X_PIXEL03_82
                HQ4X_PIXEL10_61 HQ4X_PIXEL11_30 HQ4X_PIXEL12_32 HQ4X_PIXEL13_82
                HQ4X_PIXEL20_61 HQ4X_PIXEL21_30 HQ4X_PIXEL22_30 HQ4X_PIXEL23_61
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_10 HQ4X_PIXEL32_10 HQ4X_PIXEL33_80
            acase 28:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_61 HQ4X_PIXEL02_81 HQ4X_PIXEL03_81
                HQ4X_PIXEL10_10 HQ4X_PIXEL11_30 HQ4X_PIXEL12_31 HQ4X_PIXEL13_31
                HQ4X_PIXEL20_10 HQ4X_PIXEL21_30 HQ4X_PIXEL22_30 HQ4X_PIXEL23_10
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_61 HQ4X_PIXEL32_61 HQ4X_PIXEL33_80
            acase 152:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_61 HQ4X_PIXEL02_61 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_10 HQ4X_PIXEL11_30 HQ4X_PIXEL12_30 HQ4X_PIXEL13_10
                HQ4X_PIXEL20_10 HQ4X_PIXEL21_30 HQ4X_PIXEL22_32 HQ4X_PIXEL23_32
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_61 HQ4X_PIXEL32_82 HQ4X_PIXEL33_82
            acase 194:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_10 HQ4X_PIXEL02_10 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_61 HQ4X_PIXEL11_30 HQ4X_PIXEL12_30 HQ4X_PIXEL13_61
                HQ4X_PIXEL20_61 HQ4X_PIXEL21_30 HQ4X_PIXEL22_31 HQ4X_PIXEL23_81
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_10 HQ4X_PIXEL32_31 HQ4X_PIXEL33_81
            acase 98:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_10 HQ4X_PIXEL02_10 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_61 HQ4X_PIXEL11_30 HQ4X_PIXEL12_30 HQ4X_PIXEL13_61
                HQ4X_PIXEL20_82 HQ4X_PIXEL21_32 HQ4X_PIXEL22_30 HQ4X_PIXEL23_61
                HQ4X_PIXEL30_82 HQ4X_PIXEL31_32 HQ4X_PIXEL32_10 HQ4X_PIXEL33_80
            acase 56:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_61 HQ4X_PIXEL02_61 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_10 HQ4X_PIXEL11_30 HQ4X_PIXEL12_30 HQ4X_PIXEL13_10
                HQ4X_PIXEL20_31 HQ4X_PIXEL21_31 HQ4X_PIXEL22_30 HQ4X_PIXEL23_10
                HQ4X_PIXEL30_81 HQ4X_PIXEL31_81 HQ4X_PIXEL32_61 HQ4X_PIXEL33_80
            acase 25:
                HQ4X_PIXEL00_82 HQ4X_PIXEL01_82 HQ4X_PIXEL02_61 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_32 HQ4X_PIXEL11_32 HQ4X_PIXEL12_30 HQ4X_PIXEL13_10
                HQ4X_PIXEL20_10 HQ4X_PIXEL21_30 HQ4X_PIXEL22_30 HQ4X_PIXEL23_10
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_61 HQ4X_PIXEL32_61 HQ4X_PIXEL33_80
            acase 26:
            case 31:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_0
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_61
                HQ4X_PIXEL32_61
                HQ4X_PIXEL33_80
            acase 82:
            case 214:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL10_61
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_0
                HQ4X_PIXEL20_61
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
            acase 88:
            case 248:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_61
                HQ4X_PIXEL02_61
                HQ4X_PIXEL03_80
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
            acase 74:
            case 107:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_61
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_61
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 27:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_61
                HQ4X_PIXEL32_61
                HQ4X_PIXEL33_80
            acase 86:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL10_61
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_0
                HQ4X_PIXEL20_61
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 216:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_61
                HQ4X_PIXEL02_61
                HQ4X_PIXEL03_80
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
            acase 106:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_61
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_61
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 30:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_0
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_61
                HQ4X_PIXEL32_61
                HQ4X_PIXEL33_80
            acase 210:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL10_61
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                HQ4X_PIXEL20_61
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
            acase 120:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_61
                HQ4X_PIXEL02_61
                HQ4X_PIXEL03_80
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 75:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_61
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_61
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 29:
                HQ4X_PIXEL00_82 HQ4X_PIXEL01_82 HQ4X_PIXEL02_81 HQ4X_PIXEL03_81
                HQ4X_PIXEL10_32 HQ4X_PIXEL11_32 HQ4X_PIXEL12_31 HQ4X_PIXEL13_31
                HQ4X_PIXEL20_10 HQ4X_PIXEL21_30 HQ4X_PIXEL22_30 HQ4X_PIXEL23_10
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_61 HQ4X_PIXEL32_61 HQ4X_PIXEL33_80
            acase 198:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_10 HQ4X_PIXEL02_32 HQ4X_PIXEL03_82
                HQ4X_PIXEL10_61 HQ4X_PIXEL11_30 HQ4X_PIXEL12_32 HQ4X_PIXEL13_82
                HQ4X_PIXEL20_61 HQ4X_PIXEL21_30 HQ4X_PIXEL22_31 HQ4X_PIXEL23_81
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_10 HQ4X_PIXEL32_31 HQ4X_PIXEL33_81
            acase 184:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_61 HQ4X_PIXEL02_61 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_10 HQ4X_PIXEL11_30 HQ4X_PIXEL12_30 HQ4X_PIXEL13_10
                HQ4X_PIXEL20_31 HQ4X_PIXEL21_31 HQ4X_PIXEL22_32 HQ4X_PIXEL23_32
                HQ4X_PIXEL30_81 HQ4X_PIXEL31_81 HQ4X_PIXEL32_82 HQ4X_PIXEL33_82
            acase 99:
                HQ4X_PIXEL00_81 HQ4X_PIXEL01_31 HQ4X_PIXEL02_10 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_81 HQ4X_PIXEL11_31 HQ4X_PIXEL12_30 HQ4X_PIXEL13_61
                HQ4X_PIXEL20_82 HQ4X_PIXEL21_32 HQ4X_PIXEL22_30 HQ4X_PIXEL23_61
                HQ4X_PIXEL30_82 HQ4X_PIXEL31_32 HQ4X_PIXEL32_10 HQ4X_PIXEL33_80
            acase 57:
                HQ4X_PIXEL00_82 HQ4X_PIXEL01_82 HQ4X_PIXEL02_61 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_32 HQ4X_PIXEL11_32 HQ4X_PIXEL12_30 HQ4X_PIXEL13_10
                HQ4X_PIXEL20_31 HQ4X_PIXEL21_31 HQ4X_PIXEL22_30 HQ4X_PIXEL23_10
                HQ4X_PIXEL30_81 HQ4X_PIXEL31_81 HQ4X_PIXEL32_61 HQ4X_PIXEL33_80
            acase 71:
                HQ4X_PIXEL00_81 HQ4X_PIXEL01_31 HQ4X_PIXEL02_32 HQ4X_PIXEL03_82
                HQ4X_PIXEL10_81 HQ4X_PIXEL11_31 HQ4X_PIXEL12_32 HQ4X_PIXEL13_82
                HQ4X_PIXEL20_61 HQ4X_PIXEL21_30 HQ4X_PIXEL22_30 HQ4X_PIXEL23_61
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_10 HQ4X_PIXEL32_10 HQ4X_PIXEL33_80
            acase 156:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_61 HQ4X_PIXEL02_81 HQ4X_PIXEL03_81
                HQ4X_PIXEL10_10 HQ4X_PIXEL11_30 HQ4X_PIXEL12_31 HQ4X_PIXEL13_31
                HQ4X_PIXEL20_10 HQ4X_PIXEL21_30 HQ4X_PIXEL22_32 HQ4X_PIXEL23_32
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_61 HQ4X_PIXEL32_82 HQ4X_PIXEL33_82
            acase 226:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_10 HQ4X_PIXEL02_10 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_61 HQ4X_PIXEL11_30 HQ4X_PIXEL12_30 HQ4X_PIXEL13_61
                HQ4X_PIXEL20_82 HQ4X_PIXEL21_32 HQ4X_PIXEL22_31 HQ4X_PIXEL23_81
                HQ4X_PIXEL30_82 HQ4X_PIXEL31_32 HQ4X_PIXEL32_31 HQ4X_PIXEL33_81
            acase 60:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_61 HQ4X_PIXEL02_81 HQ4X_PIXEL03_81
                HQ4X_PIXEL10_10 HQ4X_PIXEL11_30 HQ4X_PIXEL12_31 HQ4X_PIXEL13_31
                HQ4X_PIXEL20_31 HQ4X_PIXEL21_31 HQ4X_PIXEL22_30 HQ4X_PIXEL23_10
                HQ4X_PIXEL30_81 HQ4X_PIXEL31_81 HQ4X_PIXEL32_61 HQ4X_PIXEL33_80
            acase 195:
                HQ4X_PIXEL00_81 HQ4X_PIXEL01_31 HQ4X_PIXEL02_10 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_81 HQ4X_PIXEL11_31 HQ4X_PIXEL12_30 HQ4X_PIXEL13_61
                HQ4X_PIXEL20_61 HQ4X_PIXEL21_30 HQ4X_PIXEL22_31 HQ4X_PIXEL23_81
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_10 HQ4X_PIXEL32_31 HQ4X_PIXEL33_81
            acase 102:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_10 HQ4X_PIXEL02_32 HQ4X_PIXEL03_82
                HQ4X_PIXEL10_61 HQ4X_PIXEL11_30 HQ4X_PIXEL12_32 HQ4X_PIXEL13_82
                HQ4X_PIXEL20_82 HQ4X_PIXEL21_32 HQ4X_PIXEL22_30 HQ4X_PIXEL23_61
                HQ4X_PIXEL30_82 HQ4X_PIXEL31_32 HQ4X_PIXEL32_10 HQ4X_PIXEL33_80
            acase 153:
                HQ4X_PIXEL00_82 HQ4X_PIXEL01_82 HQ4X_PIXEL02_61 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_32 HQ4X_PIXEL11_32 HQ4X_PIXEL12_30 HQ4X_PIXEL13_10
                HQ4X_PIXEL20_10 HQ4X_PIXEL21_30 HQ4X_PIXEL22_32 HQ4X_PIXEL23_32
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_61 HQ4X_PIXEL32_82 HQ4X_PIXEL33_82
            acase 58:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_80
                    HQ4X_PIXEL01_10
                    HQ4X_PIXEL10_10
                    HQ4X_PIXEL11_30
                } else
                {   HQ4X_PIXEL00_20
                    HQ4X_PIXEL01_12
                    HQ4X_PIXEL10_11
                    HQ4X_PIXEL11_0
                }
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_10
                    HQ4X_PIXEL03_80
                    HQ4X_PIXEL12_30
                    HQ4X_PIXEL13_10
                } else
                {   HQ4X_PIXEL02_11
                    HQ4X_PIXEL03_20
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_12
                }
                HQ4X_PIXEL20_31
                HQ4X_PIXEL21_31
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL30_81
                HQ4X_PIXEL31_81
                HQ4X_PIXEL32_61
                HQ4X_PIXEL33_80
            acase 83:
                HQ4X_PIXEL00_81
                HQ4X_PIXEL01_31
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_10
                    HQ4X_PIXEL03_80
                    HQ4X_PIXEL12_30
                    HQ4X_PIXEL13_10
                } else
                {   HQ4X_PIXEL02_11
                    HQ4X_PIXEL03_20
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_12
                }
                HQ4X_PIXEL10_81
                HQ4X_PIXEL11_31
                HQ4X_PIXEL20_61
                HQ4X_PIXEL21_30
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_30
                    HQ4X_PIXEL23_10
                    HQ4X_PIXEL32_10
                    HQ4X_PIXEL33_80
                } else
                {   HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_11
                    HQ4X_PIXEL32_12
                    HQ4X_PIXEL33_20
                }
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
            acase 92:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_61
                HQ4X_PIXEL02_81
                HQ4X_PIXEL03_81
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_31
                HQ4X_PIXEL13_31
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_10
                    HQ4X_PIXEL21_30
                    HQ4X_PIXEL30_80
                    HQ4X_PIXEL31_10
                } else
                {   HQ4X_PIXEL20_12
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_20
                    HQ4X_PIXEL31_11
                }
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_30
                    HQ4X_PIXEL23_10
                    HQ4X_PIXEL32_10
                    HQ4X_PIXEL33_80
                } else
                {   HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_11
                    HQ4X_PIXEL32_12
                    HQ4X_PIXEL33_20
                }
            acase 202:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_80
                    HQ4X_PIXEL01_10
                    HQ4X_PIXEL10_10
                    HQ4X_PIXEL11_30
                } else
                {   HQ4X_PIXEL00_20
                    HQ4X_PIXEL01_12
                    HQ4X_PIXEL10_11
                    HQ4X_PIXEL11_0
                }
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_61
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_10
                    HQ4X_PIXEL21_30
                    HQ4X_PIXEL30_80
                    HQ4X_PIXEL31_10
                } else
                {   HQ4X_PIXEL20_12
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_20
                    HQ4X_PIXEL31_11
                }
                HQ4X_PIXEL22_31
                HQ4X_PIXEL23_81
                HQ4X_PIXEL32_31
                HQ4X_PIXEL33_81
            acase 78:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_80
                    HQ4X_PIXEL01_10
                    HQ4X_PIXEL10_10
                    HQ4X_PIXEL11_30
                } else
                {   HQ4X_PIXEL00_20
                    HQ4X_PIXEL01_12
                    HQ4X_PIXEL10_11
                    HQ4X_PIXEL11_0
                }
                HQ4X_PIXEL02_32
                HQ4X_PIXEL03_82
                HQ4X_PIXEL12_32
                HQ4X_PIXEL13_82
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_10
                    HQ4X_PIXEL21_30
                    HQ4X_PIXEL30_80
                    HQ4X_PIXEL31_10
                } else
                {   HQ4X_PIXEL20_12
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_20
                    HQ4X_PIXEL31_11
                }
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_61
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 154:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_80
                    HQ4X_PIXEL01_10
                    HQ4X_PIXEL10_10
                    HQ4X_PIXEL11_30
                } else
                {   HQ4X_PIXEL00_20
                    HQ4X_PIXEL01_12
                    HQ4X_PIXEL10_11
                    HQ4X_PIXEL11_0
                }
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_10
                    HQ4X_PIXEL03_80
                    HQ4X_PIXEL12_30
                    HQ4X_PIXEL13_10
                } else
                {   HQ4X_PIXEL02_11
                    HQ4X_PIXEL03_20
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_12
                }
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_32
                HQ4X_PIXEL23_32
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_61
                HQ4X_PIXEL32_82
                HQ4X_PIXEL33_82
            acase 114:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_10
                    HQ4X_PIXEL03_80
                    HQ4X_PIXEL12_30
                    HQ4X_PIXEL13_10
                } else
                {   HQ4X_PIXEL02_11
                    HQ4X_PIXEL03_20
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_12
                }
                HQ4X_PIXEL10_61
                HQ4X_PIXEL11_30
                HQ4X_PIXEL20_82
                HQ4X_PIXEL21_32
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_30
                    HQ4X_PIXEL23_10
                    HQ4X_PIXEL32_10
                    HQ4X_PIXEL33_80
                } else
                {   HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_11
                    HQ4X_PIXEL32_12
                    HQ4X_PIXEL33_20
                }
                HQ4X_PIXEL30_82
                HQ4X_PIXEL31_32
            acase 89:
                HQ4X_PIXEL00_82
                HQ4X_PIXEL01_82
                HQ4X_PIXEL02_61
                HQ4X_PIXEL03_80
                HQ4X_PIXEL10_32
                HQ4X_PIXEL11_32
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_10
                    HQ4X_PIXEL21_30
                    HQ4X_PIXEL30_80
                    HQ4X_PIXEL31_10
                } else
                {   HQ4X_PIXEL20_12
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_20
                    HQ4X_PIXEL31_11
                }
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_30
                    HQ4X_PIXEL23_10
                    HQ4X_PIXEL32_10
                    HQ4X_PIXEL33_80
                } else
                {   HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_11
                    HQ4X_PIXEL32_12
                    HQ4X_PIXEL33_20
                }
            acase 90:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_80
                    HQ4X_PIXEL01_10
                    HQ4X_PIXEL10_10
                    HQ4X_PIXEL11_30
                } else
                {   HQ4X_PIXEL00_20
                    HQ4X_PIXEL01_12
                    HQ4X_PIXEL10_11
                    HQ4X_PIXEL11_0
                }
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_10
                    HQ4X_PIXEL03_80
                    HQ4X_PIXEL12_30
                    HQ4X_PIXEL13_10
                } else
                {   HQ4X_PIXEL02_11
                    HQ4X_PIXEL03_20
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_12
                }
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_10
                    HQ4X_PIXEL21_30
                    HQ4X_PIXEL30_80
                    HQ4X_PIXEL31_10
                } else
                {   HQ4X_PIXEL20_12
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_20
                    HQ4X_PIXEL31_11
                }
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_30
                    HQ4X_PIXEL23_10
                    HQ4X_PIXEL32_10
                    HQ4X_PIXEL33_80
                } else
                {   HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_11
                    HQ4X_PIXEL32_12
                    HQ4X_PIXEL33_20
                }
            acase 55:
            case 23:
                if (Diff(w2, w6))
                {   HQ4X_PIXEL00_81
                    HQ4X_PIXEL01_31
                    HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL00_12
                    HQ4X_PIXEL01_14
                    HQ4X_PIXEL02_83
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL12_70
                    HQ4X_PIXEL13_21
                }
                HQ4X_PIXEL10_81
                HQ4X_PIXEL11_31
                HQ4X_PIXEL20_60
                HQ4X_PIXEL21_70
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL30_20
                HQ4X_PIXEL31_60
                HQ4X_PIXEL32_61
                HQ4X_PIXEL33_80
            acase 182:
            case 150:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_0
                    HQ4X_PIXEL23_32
                    HQ4X_PIXEL33_82
                } else
                {   HQ4X_PIXEL02_21
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL12_70
                    HQ4X_PIXEL13_83
                    HQ4X_PIXEL23_13
                    HQ4X_PIXEL33_11
                }
                HQ4X_PIXEL10_61
                HQ4X_PIXEL11_30
                HQ4X_PIXEL20_60
                HQ4X_PIXEL21_70
                HQ4X_PIXEL22_32
                HQ4X_PIXEL30_20
                HQ4X_PIXEL31_60
                HQ4X_PIXEL32_82
            acase 213:
            case 212:
                HQ4X_PIXEL00_20
                HQ4X_PIXEL01_60
                HQ4X_PIXEL02_81
                if (Diff(w6, w8))
                {   HQ4X_PIXEL03_81
                    HQ4X_PIXEL13_31
                    HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL03_12
                    HQ4X_PIXEL13_14
                    HQ4X_PIXEL22_70
                    HQ4X_PIXEL23_83
                    HQ4X_PIXEL32_21
                    HQ4X_PIXEL33_50
                }
                HQ4X_PIXEL10_60
                HQ4X_PIXEL11_70
                HQ4X_PIXEL12_31
                HQ4X_PIXEL20_61
                HQ4X_PIXEL21_30
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
            acase 241:
            case 240:
                HQ4X_PIXEL00_20
                HQ4X_PIXEL01_60
                HQ4X_PIXEL02_61
                HQ4X_PIXEL03_80
                HQ4X_PIXEL10_60
                HQ4X_PIXEL11_70
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                HQ4X_PIXEL20_82
                HQ4X_PIXEL21_32
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_0
                    HQ4X_PIXEL30_82
                    HQ4X_PIXEL31_32
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL22_70
                    HQ4X_PIXEL23_21
                    HQ4X_PIXEL30_11
                    HQ4X_PIXEL31_13
                    HQ4X_PIXEL32_83
                    HQ4X_PIXEL33_50
                }
            acase 236:
            case 232:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_61
                HQ4X_PIXEL02_60
                HQ4X_PIXEL03_20
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_70
                HQ4X_PIXEL13_60
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                    HQ4X_PIXEL32_31
                    HQ4X_PIXEL33_81
                } else
                {   HQ4X_PIXEL20_21
                    HQ4X_PIXEL21_70
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_83
                    HQ4X_PIXEL32_14
                    HQ4X_PIXEL33_12
                }
                HQ4X_PIXEL22_31
                HQ4X_PIXEL23_81
            acase 109:
            case 105:
                if (Diff(w8, w4))
                {   HQ4X_PIXEL00_82
                    HQ4X_PIXEL10_32
                    HQ4X_PIXEL20_0
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL00_11
                    HQ4X_PIXEL10_13
                    HQ4X_PIXEL20_83
                    HQ4X_PIXEL21_70
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_21
                }
                HQ4X_PIXEL01_82
                HQ4X_PIXEL02_60
                HQ4X_PIXEL03_20
                HQ4X_PIXEL11_32
                HQ4X_PIXEL12_70
                HQ4X_PIXEL13_60
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_61
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 171:
            case 43:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                    HQ4X_PIXEL11_0
                    HQ4X_PIXEL20_31
                    HQ4X_PIXEL30_81
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_21
                    HQ4X_PIXEL10_83
                    HQ4X_PIXEL11_70
                    HQ4X_PIXEL20_14
                    HQ4X_PIXEL30_12
                }
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_61
                HQ4X_PIXEL21_31
                HQ4X_PIXEL22_70
                HQ4X_PIXEL23_60
                HQ4X_PIXEL31_81
                HQ4X_PIXEL32_60
                HQ4X_PIXEL33_20
            acase 143:
            case 15:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL02_32
                    HQ4X_PIXEL03_82
                    HQ4X_PIXEL10_0
                    HQ4X_PIXEL11_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_83
                    HQ4X_PIXEL02_13
                    HQ4X_PIXEL03_11
                    HQ4X_PIXEL10_21
                    HQ4X_PIXEL11_70
                }
                HQ4X_PIXEL12_32
                HQ4X_PIXEL13_82
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_70
                HQ4X_PIXEL23_60
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_61
                HQ4X_PIXEL32_60
                HQ4X_PIXEL33_20
            acase 124:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_61
                HQ4X_PIXEL02_81
                HQ4X_PIXEL03_81
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_31
                HQ4X_PIXEL13_31
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 203:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_61
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_31
                HQ4X_PIXEL23_81
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
                HQ4X_PIXEL32_31
                HQ4X_PIXEL33_81
            acase 62:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_0
                HQ4X_PIXEL20_31
                HQ4X_PIXEL21_31
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL30_81
                HQ4X_PIXEL31_81
                HQ4X_PIXEL32_61
                HQ4X_PIXEL33_80
            acase 211:
                HQ4X_PIXEL00_81
                HQ4X_PIXEL01_31
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL10_81
                HQ4X_PIXEL11_31
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                HQ4X_PIXEL20_61
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
            acase 118:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL10_61
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_0
                HQ4X_PIXEL20_82
                HQ4X_PIXEL21_32
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL30_82
                HQ4X_PIXEL31_32
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 217:
                HQ4X_PIXEL00_82
                HQ4X_PIXEL01_82
                HQ4X_PIXEL02_61
                HQ4X_PIXEL03_80
                HQ4X_PIXEL10_32
                HQ4X_PIXEL11_32
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
            acase 110:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                HQ4X_PIXEL02_32
                HQ4X_PIXEL03_82
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_32
                HQ4X_PIXEL13_82
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_61
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 155:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_32
                HQ4X_PIXEL23_32
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_61
                HQ4X_PIXEL32_82
                HQ4X_PIXEL33_82
            acase 188:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_61 HQ4X_PIXEL02_81 HQ4X_PIXEL03_81
                HQ4X_PIXEL10_10 HQ4X_PIXEL11_30 HQ4X_PIXEL12_31 HQ4X_PIXEL13_31
                HQ4X_PIXEL20_31 HQ4X_PIXEL21_31 HQ4X_PIXEL22_32 HQ4X_PIXEL23_32
                HQ4X_PIXEL30_81 HQ4X_PIXEL31_81 HQ4X_PIXEL32_82 HQ4X_PIXEL33_82
            acase 185:
                HQ4X_PIXEL00_82 HQ4X_PIXEL01_82 HQ4X_PIXEL02_61 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_32 HQ4X_PIXEL11_32 HQ4X_PIXEL12_30 HQ4X_PIXEL13_10
                HQ4X_PIXEL20_31 HQ4X_PIXEL21_31 HQ4X_PIXEL22_32 HQ4X_PIXEL23_32
                HQ4X_PIXEL30_81 HQ4X_PIXEL31_81 HQ4X_PIXEL32_82 HQ4X_PIXEL33_82
            acase 61:
                HQ4X_PIXEL00_82 HQ4X_PIXEL01_82 HQ4X_PIXEL02_81 HQ4X_PIXEL03_81
                HQ4X_PIXEL10_32 HQ4X_PIXEL11_32 HQ4X_PIXEL12_31 HQ4X_PIXEL13_31
                HQ4X_PIXEL20_31 HQ4X_PIXEL21_31 HQ4X_PIXEL22_30 HQ4X_PIXEL23_10
                HQ4X_PIXEL30_81 HQ4X_PIXEL31_81 HQ4X_PIXEL32_61 HQ4X_PIXEL33_80
            acase 157:
                HQ4X_PIXEL00_82 HQ4X_PIXEL01_82 HQ4X_PIXEL02_81 HQ4X_PIXEL03_81
                HQ4X_PIXEL10_32 HQ4X_PIXEL11_32 HQ4X_PIXEL12_31 HQ4X_PIXEL13_31
                HQ4X_PIXEL20_10 HQ4X_PIXEL21_30 HQ4X_PIXEL22_32 HQ4X_PIXEL23_32
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_61 HQ4X_PIXEL32_82 HQ4X_PIXEL33_82
            acase 103:
                HQ4X_PIXEL00_81 HQ4X_PIXEL01_31 HQ4X_PIXEL02_32 HQ4X_PIXEL03_82
                HQ4X_PIXEL10_81 HQ4X_PIXEL11_31 HQ4X_PIXEL12_32 HQ4X_PIXEL13_82
                HQ4X_PIXEL20_82 HQ4X_PIXEL21_32 HQ4X_PIXEL22_30 HQ4X_PIXEL23_61
                HQ4X_PIXEL30_82 HQ4X_PIXEL31_32 HQ4X_PIXEL32_10 HQ4X_PIXEL33_80
            acase 227:
                HQ4X_PIXEL00_81 HQ4X_PIXEL01_31 HQ4X_PIXEL02_10 HQ4X_PIXEL03_80
                HQ4X_PIXEL10_81 HQ4X_PIXEL11_31 HQ4X_PIXEL12_30 HQ4X_PIXEL13_61
                HQ4X_PIXEL20_82 HQ4X_PIXEL21_32 HQ4X_PIXEL22_31 HQ4X_PIXEL23_81
                HQ4X_PIXEL30_82 HQ4X_PIXEL31_32 HQ4X_PIXEL32_31 HQ4X_PIXEL33_81
            acase 230:
                HQ4X_PIXEL00_80 HQ4X_PIXEL01_10 HQ4X_PIXEL02_32 HQ4X_PIXEL03_82
                HQ4X_PIXEL10_61 HQ4X_PIXEL11_30 HQ4X_PIXEL12_32 HQ4X_PIXEL13_82
                HQ4X_PIXEL20_82 HQ4X_PIXEL21_32 HQ4X_PIXEL22_31 HQ4X_PIXEL23_81
                HQ4X_PIXEL30_82 HQ4X_PIXEL31_32 HQ4X_PIXEL32_31 HQ4X_PIXEL33_81
            acase 199:
                HQ4X_PIXEL00_81 HQ4X_PIXEL01_31 HQ4X_PIXEL02_32 HQ4X_PIXEL03_82
                HQ4X_PIXEL10_81 HQ4X_PIXEL11_31 HQ4X_PIXEL12_32 HQ4X_PIXEL13_82
                HQ4X_PIXEL20_61 HQ4X_PIXEL21_30 HQ4X_PIXEL22_31 HQ4X_PIXEL23_81
                HQ4X_PIXEL30_80 HQ4X_PIXEL31_10 HQ4X_PIXEL32_31 HQ4X_PIXEL33_81
            acase 220:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_61
                HQ4X_PIXEL02_81
                HQ4X_PIXEL03_81
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_31
                HQ4X_PIXEL13_31
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_10
                    HQ4X_PIXEL21_30
                    HQ4X_PIXEL30_80
                    HQ4X_PIXEL31_10
                } else
                {   HQ4X_PIXEL20_12
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_20
                    HQ4X_PIXEL31_11
                }
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
            acase 158:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_80
                    HQ4X_PIXEL01_10
                    HQ4X_PIXEL10_10
                    HQ4X_PIXEL11_30
                } else
                {   HQ4X_PIXEL00_20
                    HQ4X_PIXEL01_12
                    HQ4X_PIXEL10_11
                    HQ4X_PIXEL11_0
                }
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL12_0
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_32
                HQ4X_PIXEL23_32
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_61
                HQ4X_PIXEL32_82
                HQ4X_PIXEL33_82
            acase 234:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_80
                    HQ4X_PIXEL01_10
                    HQ4X_PIXEL10_10
                    HQ4X_PIXEL11_30
                } else
                {   HQ4X_PIXEL00_20
                    HQ4X_PIXEL01_12
                    HQ4X_PIXEL10_11
                    HQ4X_PIXEL11_0
                }
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_61
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_31
                HQ4X_PIXEL23_81
                HQ4X_PIXEL32_31
                HQ4X_PIXEL33_81
            acase 242:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_10
                    HQ4X_PIXEL03_80
                    HQ4X_PIXEL12_30
                    HQ4X_PIXEL13_10
                } else
                {   HQ4X_PIXEL02_11
                    HQ4X_PIXEL03_20
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_12
                }
                HQ4X_PIXEL10_61
                HQ4X_PIXEL11_30
                HQ4X_PIXEL20_82
                HQ4X_PIXEL21_32
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
                HQ4X_PIXEL30_82
                HQ4X_PIXEL31_32
            acase 59:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_10
                    HQ4X_PIXEL03_80
                    HQ4X_PIXEL12_30
                    HQ4X_PIXEL13_10
                } else
                {   HQ4X_PIXEL02_11
                    HQ4X_PIXEL03_20
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_12
                }
                HQ4X_PIXEL11_0
                HQ4X_PIXEL20_31
                HQ4X_PIXEL21_31
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL30_81
                HQ4X_PIXEL31_81
                HQ4X_PIXEL32_61
                HQ4X_PIXEL33_80
            acase 121:
                HQ4X_PIXEL00_82
                HQ4X_PIXEL01_82
                HQ4X_PIXEL02_61
                HQ4X_PIXEL03_80
                HQ4X_PIXEL10_32
                HQ4X_PIXEL11_32
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_30
                    HQ4X_PIXEL23_10
                    HQ4X_PIXEL32_10
                    HQ4X_PIXEL33_80
                } else
                {   HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_11
                    HQ4X_PIXEL32_12
                    HQ4X_PIXEL33_20
                }
            acase 87:
                HQ4X_PIXEL00_81
                HQ4X_PIXEL01_31
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL10_81
                HQ4X_PIXEL11_31
                HQ4X_PIXEL12_0
                HQ4X_PIXEL20_61
                HQ4X_PIXEL21_30
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_30
                    HQ4X_PIXEL23_10
                    HQ4X_PIXEL32_10
                    HQ4X_PIXEL33_80
                } else
                {   HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_11
                    HQ4X_PIXEL32_12
                    HQ4X_PIXEL33_20
                }
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
            acase 79:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                HQ4X_PIXEL02_32
                HQ4X_PIXEL03_82
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_32
                HQ4X_PIXEL13_82
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_10
                    HQ4X_PIXEL21_30
                    HQ4X_PIXEL30_80
                    HQ4X_PIXEL31_10
                } else
                {   HQ4X_PIXEL20_12
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_20
                    HQ4X_PIXEL31_11
                }
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_61
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 122:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_80
                    HQ4X_PIXEL01_10
                    HQ4X_PIXEL10_10
                    HQ4X_PIXEL11_30
                } else
                {   HQ4X_PIXEL00_20
                    HQ4X_PIXEL01_12
                    HQ4X_PIXEL10_11
                    HQ4X_PIXEL11_0
                }
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_10
                    HQ4X_PIXEL03_80
                    HQ4X_PIXEL12_30
                    HQ4X_PIXEL13_10
                } else
                {   HQ4X_PIXEL02_11
                    HQ4X_PIXEL03_20
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_12
                }
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_30
                    HQ4X_PIXEL23_10
                    HQ4X_PIXEL32_10
                    HQ4X_PIXEL33_80
                } else
                {   HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_11
                    HQ4X_PIXEL32_12
                    HQ4X_PIXEL33_20
                }
            acase 94:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_80
                    HQ4X_PIXEL01_10
                    HQ4X_PIXEL10_10
                    HQ4X_PIXEL11_30
                } else
                {   HQ4X_PIXEL00_20
                    HQ4X_PIXEL01_12
                    HQ4X_PIXEL10_11
                    HQ4X_PIXEL11_0
                }
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL12_0
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_10
                    HQ4X_PIXEL21_30
                    HQ4X_PIXEL30_80
                    HQ4X_PIXEL31_10
                } else
                {   HQ4X_PIXEL20_12
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_20
                    HQ4X_PIXEL31_11
                }
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_30
                    HQ4X_PIXEL23_10
                    HQ4X_PIXEL32_10
                    HQ4X_PIXEL33_80
                } else
                {   HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_11
                    HQ4X_PIXEL32_12
                    HQ4X_PIXEL33_20
                }
            acase 218:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_80
                    HQ4X_PIXEL01_10
                    HQ4X_PIXEL10_10
                    HQ4X_PIXEL11_30
                } else
                {   HQ4X_PIXEL00_20
                    HQ4X_PIXEL01_12
                    HQ4X_PIXEL10_11
                    HQ4X_PIXEL11_0
                }
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_10
                    HQ4X_PIXEL03_80
                    HQ4X_PIXEL12_30
                    HQ4X_PIXEL13_10
                } else
                {   HQ4X_PIXEL02_11
                    HQ4X_PIXEL03_20
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_12
                }
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_10
                    HQ4X_PIXEL21_30
                    HQ4X_PIXEL30_80
                    HQ4X_PIXEL31_10
                } else
                {   HQ4X_PIXEL20_12
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_20
                    HQ4X_PIXEL31_11
                }
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
            acase 91:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_10
                    HQ4X_PIXEL03_80
                    HQ4X_PIXEL12_30
                    HQ4X_PIXEL13_10
                } else
                {   HQ4X_PIXEL02_11
                    HQ4X_PIXEL03_20
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_12
                }
                HQ4X_PIXEL11_0
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_10
                    HQ4X_PIXEL21_30
                    HQ4X_PIXEL30_80
                    HQ4X_PIXEL31_10
                } else
                {   HQ4X_PIXEL20_12
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_20
                    HQ4X_PIXEL31_11
                }
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_30
                    HQ4X_PIXEL23_10
                    HQ4X_PIXEL32_10
                    HQ4X_PIXEL33_80
                } else
                {   HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_11
                    HQ4X_PIXEL32_12
                    HQ4X_PIXEL33_20
                }
            acase 229:
                HQ4X_PIXEL00_20 HQ4X_PIXEL01_60 HQ4X_PIXEL02_60 HQ4X_PIXEL03_20
                HQ4X_PIXEL10_60 HQ4X_PIXEL11_70 HQ4X_PIXEL12_70 HQ4X_PIXEL13_60
                HQ4X_PIXEL20_82 HQ4X_PIXEL21_32 HQ4X_PIXEL22_31 HQ4X_PIXEL23_81
                HQ4X_PIXEL30_82 HQ4X_PIXEL31_32 HQ4X_PIXEL32_31 HQ4X_PIXEL33_81
            acase 167:
                HQ4X_PIXEL00_81 HQ4X_PIXEL01_31 HQ4X_PIXEL02_32 HQ4X_PIXEL03_82
                HQ4X_PIXEL10_81 HQ4X_PIXEL11_31 HQ4X_PIXEL12_32 HQ4X_PIXEL13_82
                HQ4X_PIXEL20_60 HQ4X_PIXEL21_70 HQ4X_PIXEL22_70 HQ4X_PIXEL23_60
                HQ4X_PIXEL30_20 HQ4X_PIXEL31_60 HQ4X_PIXEL32_60 HQ4X_PIXEL33_20
            acase 173:
                HQ4X_PIXEL00_82 HQ4X_PIXEL01_82 HQ4X_PIXEL02_60 HQ4X_PIXEL03_20
                HQ4X_PIXEL10_32 HQ4X_PIXEL11_32 HQ4X_PIXEL12_70 HQ4X_PIXEL13_60
                HQ4X_PIXEL20_31 HQ4X_PIXEL21_31 HQ4X_PIXEL22_70 HQ4X_PIXEL23_60
                HQ4X_PIXEL30_81 HQ4X_PIXEL31_81 HQ4X_PIXEL32_60 HQ4X_PIXEL33_20
            acase 181:
                HQ4X_PIXEL00_20 HQ4X_PIXEL01_60 HQ4X_PIXEL02_81 HQ4X_PIXEL03_81
                HQ4X_PIXEL10_60 HQ4X_PIXEL11_70 HQ4X_PIXEL12_31 HQ4X_PIXEL13_31
                HQ4X_PIXEL20_60 HQ4X_PIXEL21_70 HQ4X_PIXEL22_32 HQ4X_PIXEL23_32
                HQ4X_PIXEL30_20 HQ4X_PIXEL31_60 HQ4X_PIXEL32_82 HQ4X_PIXEL33_82
            acase 186:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_80
                    HQ4X_PIXEL01_10
                    HQ4X_PIXEL10_10
                    HQ4X_PIXEL11_30
                } else
                {   HQ4X_PIXEL00_20
                    HQ4X_PIXEL01_12
                    HQ4X_PIXEL10_11
                    HQ4X_PIXEL11_0
                }
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_10
                    HQ4X_PIXEL03_80
                    HQ4X_PIXEL12_30
                    HQ4X_PIXEL13_10
                } else
                {   HQ4X_PIXEL02_11
                    HQ4X_PIXEL03_20
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_12
                }
                HQ4X_PIXEL20_31
                HQ4X_PIXEL21_31
                HQ4X_PIXEL22_32
                HQ4X_PIXEL23_32
                HQ4X_PIXEL30_81
                HQ4X_PIXEL31_81
                HQ4X_PIXEL32_82
                HQ4X_PIXEL33_82
            acase 115:
                HQ4X_PIXEL00_81
                HQ4X_PIXEL01_31
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_10
                    HQ4X_PIXEL03_80
                    HQ4X_PIXEL12_30
                    HQ4X_PIXEL13_10
                } else
                {   HQ4X_PIXEL02_11
                    HQ4X_PIXEL03_20
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_12
                }
                HQ4X_PIXEL10_81
                HQ4X_PIXEL11_31
                HQ4X_PIXEL20_82
                HQ4X_PIXEL21_32
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_30
                    HQ4X_PIXEL23_10
                    HQ4X_PIXEL32_10
                    HQ4X_PIXEL33_80
                } else
                {   HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_11
                    HQ4X_PIXEL32_12
                    HQ4X_PIXEL33_20
                }
                HQ4X_PIXEL30_82
                HQ4X_PIXEL31_32
            acase 93:
                HQ4X_PIXEL00_82
                HQ4X_PIXEL01_82
                HQ4X_PIXEL02_81
                HQ4X_PIXEL03_81
                HQ4X_PIXEL10_32
                HQ4X_PIXEL11_32
                HQ4X_PIXEL12_31
                HQ4X_PIXEL13_31
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_10
                    HQ4X_PIXEL21_30
                    HQ4X_PIXEL30_80
                    HQ4X_PIXEL31_10
                } else
                {   HQ4X_PIXEL20_12
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_20
                    HQ4X_PIXEL31_11
                }
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_30
                    HQ4X_PIXEL23_10
                    HQ4X_PIXEL32_10
                    HQ4X_PIXEL33_80
                } else
                {   HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_11
                    HQ4X_PIXEL32_12
                    HQ4X_PIXEL33_20
                }
            acase 206:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_80
                    HQ4X_PIXEL01_10
                    HQ4X_PIXEL10_10
                    HQ4X_PIXEL11_30
                } else
                {   HQ4X_PIXEL00_20
                    HQ4X_PIXEL01_12
                    HQ4X_PIXEL10_11
                    HQ4X_PIXEL11_0
                }
                HQ4X_PIXEL02_32
                HQ4X_PIXEL03_82
                HQ4X_PIXEL12_32
                HQ4X_PIXEL13_82
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_10
                    HQ4X_PIXEL21_30
                    HQ4X_PIXEL30_80
                    HQ4X_PIXEL31_10
                } else
                {   HQ4X_PIXEL20_12
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_20
                    HQ4X_PIXEL31_11
                }
                HQ4X_PIXEL22_31
                HQ4X_PIXEL23_81
                HQ4X_PIXEL32_31
                HQ4X_PIXEL33_81
            acase 205:
            case 201:
                HQ4X_PIXEL00_82
                HQ4X_PIXEL01_82
                HQ4X_PIXEL02_60
                HQ4X_PIXEL03_20
                HQ4X_PIXEL10_32
                HQ4X_PIXEL11_32
                HQ4X_PIXEL12_70
                HQ4X_PIXEL13_60
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_10
                    HQ4X_PIXEL21_30
                    HQ4X_PIXEL30_80
                    HQ4X_PIXEL31_10
                } else
                {   HQ4X_PIXEL20_12
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_20
                    HQ4X_PIXEL31_11
                }
                HQ4X_PIXEL22_31
                HQ4X_PIXEL23_81
                HQ4X_PIXEL32_31
                HQ4X_PIXEL33_81
            acase 174:
            case 46:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_80
                    HQ4X_PIXEL01_10
                    HQ4X_PIXEL10_10
                    HQ4X_PIXEL11_30
                } else
                {   HQ4X_PIXEL00_20
                    HQ4X_PIXEL01_12
                    HQ4X_PIXEL10_11
                    HQ4X_PIXEL11_0
                }
                HQ4X_PIXEL02_32
                HQ4X_PIXEL03_82
                HQ4X_PIXEL12_32
                HQ4X_PIXEL13_82
                HQ4X_PIXEL20_31
                HQ4X_PIXEL21_31
                HQ4X_PIXEL22_70
                HQ4X_PIXEL23_60
                HQ4X_PIXEL30_81
                HQ4X_PIXEL31_81
                HQ4X_PIXEL32_60
                HQ4X_PIXEL33_20
            acase 179:
            case 147:
                HQ4X_PIXEL00_81
                HQ4X_PIXEL01_31
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_10
                    HQ4X_PIXEL03_80
                    HQ4X_PIXEL12_30
                    HQ4X_PIXEL13_10
                } else
                {   HQ4X_PIXEL02_11
                    HQ4X_PIXEL03_20
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_12
                }
                HQ4X_PIXEL10_81
                HQ4X_PIXEL11_31
                HQ4X_PIXEL20_60
                HQ4X_PIXEL21_70
                HQ4X_PIXEL22_32
                HQ4X_PIXEL23_32
                HQ4X_PIXEL30_20
                HQ4X_PIXEL31_60
                HQ4X_PIXEL32_82
                HQ4X_PIXEL33_82
            acase 117:
            case 116:
                HQ4X_PIXEL00_20
                HQ4X_PIXEL01_60
                HQ4X_PIXEL02_81
                HQ4X_PIXEL03_81
                HQ4X_PIXEL10_60
                HQ4X_PIXEL11_70
                HQ4X_PIXEL12_31
                HQ4X_PIXEL13_31
                HQ4X_PIXEL20_82
                HQ4X_PIXEL21_32
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_30
                    HQ4X_PIXEL23_10
                    HQ4X_PIXEL32_10
                    HQ4X_PIXEL33_80
                } else
                {   HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_11
                    HQ4X_PIXEL32_12
                    HQ4X_PIXEL33_20
                }
                HQ4X_PIXEL30_82
                HQ4X_PIXEL31_32
            acase 189:
                HQ4X_PIXEL00_82 HQ4X_PIXEL01_82 HQ4X_PIXEL02_81 HQ4X_PIXEL03_81
                HQ4X_PIXEL10_32 HQ4X_PIXEL11_32 HQ4X_PIXEL12_31 HQ4X_PIXEL13_31
                HQ4X_PIXEL20_31 HQ4X_PIXEL21_31 HQ4X_PIXEL22_32 HQ4X_PIXEL23_32
                HQ4X_PIXEL30_81 HQ4X_PIXEL31_81 HQ4X_PIXEL32_82 HQ4X_PIXEL33_82
            acase 231:
                HQ4X_PIXEL00_81 HQ4X_PIXEL01_31 HQ4X_PIXEL02_32 HQ4X_PIXEL03_82
                HQ4X_PIXEL10_81 HQ4X_PIXEL11_31 HQ4X_PIXEL12_32 HQ4X_PIXEL13_82
                HQ4X_PIXEL20_82 HQ4X_PIXEL21_32 HQ4X_PIXEL22_31 HQ4X_PIXEL23_81
                HQ4X_PIXEL30_82 HQ4X_PIXEL31_32 HQ4X_PIXEL32_31 HQ4X_PIXEL33_81
            acase 126:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_0
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 219:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
            acase 125:
                if (Diff(w8, w4))
                {   HQ4X_PIXEL00_82
                    HQ4X_PIXEL10_32
                    HQ4X_PIXEL20_0
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL00_11
                    HQ4X_PIXEL10_13
                    HQ4X_PIXEL20_83
                    HQ4X_PIXEL21_70
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_21
                }
                HQ4X_PIXEL01_82
                HQ4X_PIXEL02_81
                HQ4X_PIXEL03_81
                HQ4X_PIXEL11_32
                HQ4X_PIXEL12_31
                HQ4X_PIXEL13_31
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 221:
                HQ4X_PIXEL00_82
                HQ4X_PIXEL01_82
                HQ4X_PIXEL02_81
                if (Diff(w6, w8))
                {   HQ4X_PIXEL03_81
                    HQ4X_PIXEL13_31
                    HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL03_12
                    HQ4X_PIXEL13_14
                    HQ4X_PIXEL22_70
                    HQ4X_PIXEL23_83
                    HQ4X_PIXEL32_21
                    HQ4X_PIXEL33_50
                }
                HQ4X_PIXEL10_32
                HQ4X_PIXEL11_32
                HQ4X_PIXEL12_31
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
            acase 207:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL02_32
                    HQ4X_PIXEL03_82
                    HQ4X_PIXEL10_0
                    HQ4X_PIXEL11_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_83
                    HQ4X_PIXEL02_13
                    HQ4X_PIXEL03_11
                    HQ4X_PIXEL10_21
                    HQ4X_PIXEL11_70
                }
                HQ4X_PIXEL12_32
                HQ4X_PIXEL13_82
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_31
                HQ4X_PIXEL23_81
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
                HQ4X_PIXEL32_31
                HQ4X_PIXEL33_81
            acase 238:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                HQ4X_PIXEL02_32
                HQ4X_PIXEL03_82
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_32
                HQ4X_PIXEL13_82
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL21_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                    HQ4X_PIXEL32_31
                    HQ4X_PIXEL33_81
                } else
                {   HQ4X_PIXEL20_21
                    HQ4X_PIXEL21_70
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_83
                    HQ4X_PIXEL32_14
                    HQ4X_PIXEL33_12
                }
                HQ4X_PIXEL22_31
                HQ4X_PIXEL23_81
            acase 190:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_0
                    HQ4X_PIXEL23_32
                    HQ4X_PIXEL33_82
                } else
                {   HQ4X_PIXEL02_21
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL12_70
                    HQ4X_PIXEL13_83
                    HQ4X_PIXEL23_13
                    HQ4X_PIXEL33_11
                }
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL20_31
                HQ4X_PIXEL21_31
                HQ4X_PIXEL22_32
                HQ4X_PIXEL30_81
                HQ4X_PIXEL31_81
                HQ4X_PIXEL32_82
            acase 187:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                    HQ4X_PIXEL11_0
                    HQ4X_PIXEL20_31
                    HQ4X_PIXEL30_81
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_21
                    HQ4X_PIXEL10_83
                    HQ4X_PIXEL11_70
                    HQ4X_PIXEL20_14
                    HQ4X_PIXEL30_12
                }
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                HQ4X_PIXEL21_31
                HQ4X_PIXEL22_32
                HQ4X_PIXEL23_32
                HQ4X_PIXEL31_81
                HQ4X_PIXEL32_82
                HQ4X_PIXEL33_82
            acase 243:
                HQ4X_PIXEL00_81
                HQ4X_PIXEL01_31
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL10_81
                HQ4X_PIXEL11_31
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                HQ4X_PIXEL20_82
                HQ4X_PIXEL21_32
                if (Diff(w6, w8))
                {   HQ4X_PIXEL22_0
                    HQ4X_PIXEL23_0
                    HQ4X_PIXEL30_82
                    HQ4X_PIXEL31_32
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL22_70
                    HQ4X_PIXEL23_21
                    HQ4X_PIXEL30_11
                    HQ4X_PIXEL31_13
                    HQ4X_PIXEL32_83
                    HQ4X_PIXEL33_50
                }
            acase 119:
                if (Diff(w2, w6))
                {   HQ4X_PIXEL00_81
                    HQ4X_PIXEL01_31
                    HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL12_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL00_12
                    HQ4X_PIXEL01_14
                    HQ4X_PIXEL02_83
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL12_70
                    HQ4X_PIXEL13_21
                }
                HQ4X_PIXEL10_81
                HQ4X_PIXEL11_31
                HQ4X_PIXEL20_82
                HQ4X_PIXEL21_32
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL30_82
                HQ4X_PIXEL31_32
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 237:
            case 233:
                HQ4X_PIXEL00_82
                HQ4X_PIXEL01_82
                HQ4X_PIXEL02_60
                HQ4X_PIXEL03_20
                HQ4X_PIXEL10_32
                HQ4X_PIXEL11_32
                HQ4X_PIXEL12_70
                HQ4X_PIXEL13_60
                HQ4X_PIXEL20_0
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_31
                HQ4X_PIXEL23_81
                if (Diff(w8, w4))
                {   HQ4X_PIXEL30_0
                } else
                {   HQ4X_PIXEL30_20
                }
                HQ4X_PIXEL31_0
                HQ4X_PIXEL32_31
                HQ4X_PIXEL33_81
            acase 175:
            case 47:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                } else
                {   HQ4X_PIXEL00_20
                }
                HQ4X_PIXEL01_0
                HQ4X_PIXEL02_32
                HQ4X_PIXEL03_82
                HQ4X_PIXEL10_0
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_32
                HQ4X_PIXEL13_82
                HQ4X_PIXEL20_31
                HQ4X_PIXEL21_31
                HQ4X_PIXEL22_70
                HQ4X_PIXEL23_60
                HQ4X_PIXEL30_81
                HQ4X_PIXEL31_81
                HQ4X_PIXEL32_60
                HQ4X_PIXEL33_20
            acase 183:
            case 151:
                HQ4X_PIXEL00_81
                HQ4X_PIXEL01_31
                HQ4X_PIXEL02_0
                if (Diff(w2, w6))
                {   HQ4X_PIXEL03_0
                } else
                {   HQ4X_PIXEL03_20
                }
                HQ4X_PIXEL10_81
                HQ4X_PIXEL11_31
                HQ4X_PIXEL12_0
                HQ4X_PIXEL13_0
                HQ4X_PIXEL20_60
                HQ4X_PIXEL21_70
                HQ4X_PIXEL22_32
                HQ4X_PIXEL23_32
                HQ4X_PIXEL30_20
                HQ4X_PIXEL31_60
                HQ4X_PIXEL32_82
                HQ4X_PIXEL33_82
            acase 245:
            case 244:
                HQ4X_PIXEL00_20
                HQ4X_PIXEL01_60
                HQ4X_PIXEL02_81
                HQ4X_PIXEL03_81
                HQ4X_PIXEL10_60
                HQ4X_PIXEL11_70
                HQ4X_PIXEL12_31
                HQ4X_PIXEL13_31
                HQ4X_PIXEL20_82
                HQ4X_PIXEL21_32
                HQ4X_PIXEL22_0
                HQ4X_PIXEL23_0
                HQ4X_PIXEL30_82
                HQ4X_PIXEL31_32
                HQ4X_PIXEL32_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL33_20
                }
            acase 250:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
            acase 123:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 95:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_0
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 222:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_0
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
            acase 252:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_61
                HQ4X_PIXEL02_81
                HQ4X_PIXEL03_81
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_31
                HQ4X_PIXEL13_31
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_0
                HQ4X_PIXEL23_0
                HQ4X_PIXEL32_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL33_20
                }
            acase 249:
                HQ4X_PIXEL00_82
                HQ4X_PIXEL01_82
                HQ4X_PIXEL02_61
                HQ4X_PIXEL03_80
                HQ4X_PIXEL10_32
                HQ4X_PIXEL11_32
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                HQ4X_PIXEL20_0
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
                if (Diff(w8, w4))
                {   HQ4X_PIXEL30_0
                } else
                {   HQ4X_PIXEL30_20
                }
                HQ4X_PIXEL31_0
            acase 235:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_61
                HQ4X_PIXEL20_0
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_31
                HQ4X_PIXEL23_81
                if (Diff(w8, w4))
                {   HQ4X_PIXEL30_0
                } else
                {   HQ4X_PIXEL30_20
                }
                HQ4X_PIXEL31_0
                HQ4X_PIXEL32_31
                HQ4X_PIXEL33_81
            acase 111:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                } else
                {   HQ4X_PIXEL00_20
                }
                HQ4X_PIXEL01_0
                HQ4X_PIXEL02_32
                HQ4X_PIXEL03_82
                HQ4X_PIXEL10_0
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_32
                HQ4X_PIXEL13_82
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_61
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 63:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                } else
                {   HQ4X_PIXEL00_20
                }
                HQ4X_PIXEL01_0
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL10_0
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_0
                HQ4X_PIXEL20_31
                HQ4X_PIXEL21_31
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL30_81
                HQ4X_PIXEL31_81
                HQ4X_PIXEL32_61
                HQ4X_PIXEL33_80
            acase 159:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                HQ4X_PIXEL02_0
                if (Diff(w2, w6))
                {   HQ4X_PIXEL03_0
                } else
                {   HQ4X_PIXEL03_20
                }
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_0
                HQ4X_PIXEL13_0
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_32
                HQ4X_PIXEL23_32
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_61
                HQ4X_PIXEL32_82
                HQ4X_PIXEL33_82
            acase 215:
                HQ4X_PIXEL00_81
                HQ4X_PIXEL01_31
                HQ4X_PIXEL02_0
                if (Diff(w2, w6))
                {   HQ4X_PIXEL03_0
                } else
                {   HQ4X_PIXEL03_20
                }
                HQ4X_PIXEL10_81
                HQ4X_PIXEL11_31
                HQ4X_PIXEL12_0
                HQ4X_PIXEL13_0
                HQ4X_PIXEL20_61
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
            acase 246:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL10_61
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_0
                HQ4X_PIXEL20_82
                HQ4X_PIXEL21_32
                HQ4X_PIXEL22_0
                HQ4X_PIXEL23_0
                HQ4X_PIXEL30_82
                HQ4X_PIXEL31_32
                HQ4X_PIXEL32_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL33_20
                }
            acase 254:
                HQ4X_PIXEL00_80
                HQ4X_PIXEL01_10
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL10_10
                HQ4X_PIXEL11_30
                HQ4X_PIXEL12_0
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_0
                HQ4X_PIXEL23_0
                HQ4X_PIXEL32_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL33_20
                }
            acase 253:
                HQ4X_PIXEL00_82
                HQ4X_PIXEL01_82
                HQ4X_PIXEL02_81
                HQ4X_PIXEL03_81
                HQ4X_PIXEL10_32
                HQ4X_PIXEL11_32
                HQ4X_PIXEL12_31
                HQ4X_PIXEL13_31
                HQ4X_PIXEL20_0
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_0
                HQ4X_PIXEL23_0
                if (Diff(w8, w4))
                {   HQ4X_PIXEL30_0
                } else
                {   HQ4X_PIXEL30_20
                }
                HQ4X_PIXEL31_0
                HQ4X_PIXEL32_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL33_20
                }
            acase 251:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                HQ4X_PIXEL02_10
                HQ4X_PIXEL03_80
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_30
                HQ4X_PIXEL13_10
                HQ4X_PIXEL20_0
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
                if (Diff(w8, w4))
                {   HQ4X_PIXEL30_0
                } else
                {   HQ4X_PIXEL30_20
                }
                HQ4X_PIXEL31_0
            acase 239:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                } else
                {   HQ4X_PIXEL00_20
                }
                HQ4X_PIXEL01_0
                HQ4X_PIXEL02_32
                HQ4X_PIXEL03_82
                HQ4X_PIXEL10_0
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_32
                HQ4X_PIXEL13_82
                HQ4X_PIXEL20_0
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_31
                HQ4X_PIXEL23_81
                if (Diff(w8, w4))
                {   HQ4X_PIXEL30_0
                } else
                {   HQ4X_PIXEL30_20
                }
                HQ4X_PIXEL31_0
                HQ4X_PIXEL32_31
                HQ4X_PIXEL33_81
            acase 127:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                } else
                {   HQ4X_PIXEL00_20
                }
                HQ4X_PIXEL01_0
                if (Diff(w2, w6))
                {   HQ4X_PIXEL02_0
                    HQ4X_PIXEL03_0
                    HQ4X_PIXEL13_0
                } else
                {   HQ4X_PIXEL02_50
                    HQ4X_PIXEL03_50
                    HQ4X_PIXEL13_50
                }
                HQ4X_PIXEL10_0
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_0
                if (Diff(w8, w4))
                {   HQ4X_PIXEL20_0
                    HQ4X_PIXEL30_0
                    HQ4X_PIXEL31_0
                } else
                {   HQ4X_PIXEL20_50
                    HQ4X_PIXEL30_50
                    HQ4X_PIXEL31_50
                }
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_30
                HQ4X_PIXEL23_10
                HQ4X_PIXEL32_10
                HQ4X_PIXEL33_80
            acase 191:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                } else
                {   HQ4X_PIXEL00_20
                }
                HQ4X_PIXEL01_0
                HQ4X_PIXEL02_0
                if (Diff(w2, w6))
                {   HQ4X_PIXEL03_0
                } else
                {   HQ4X_PIXEL03_20
                }
                HQ4X_PIXEL10_0
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_0
                HQ4X_PIXEL13_0
                HQ4X_PIXEL20_31
                HQ4X_PIXEL21_31
                HQ4X_PIXEL22_32
                HQ4X_PIXEL23_32
                HQ4X_PIXEL30_81
                HQ4X_PIXEL31_81
                HQ4X_PIXEL32_82
                HQ4X_PIXEL33_82
            acase 223:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                    HQ4X_PIXEL01_0
                    HQ4X_PIXEL10_0
                } else
                {   HQ4X_PIXEL00_50
                    HQ4X_PIXEL01_50
                    HQ4X_PIXEL10_50
                }
                HQ4X_PIXEL02_0
                if (Diff(w2, w6))
                {   HQ4X_PIXEL03_0
                } else
                {   HQ4X_PIXEL03_20
                }
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_0
                HQ4X_PIXEL13_0
                HQ4X_PIXEL20_10
                HQ4X_PIXEL21_30
                HQ4X_PIXEL22_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL23_0
                    HQ4X_PIXEL32_0
                    HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL23_50
                    HQ4X_PIXEL32_50
                    HQ4X_PIXEL33_50
                }
                HQ4X_PIXEL30_80
                HQ4X_PIXEL31_10
            acase 247:
                HQ4X_PIXEL00_81
                HQ4X_PIXEL01_31
                HQ4X_PIXEL02_0
                if (Diff(w2, w6))
                {   HQ4X_PIXEL03_0
                } else
                {   HQ4X_PIXEL03_20
                }
                HQ4X_PIXEL10_81
                HQ4X_PIXEL11_31
                HQ4X_PIXEL12_0
                HQ4X_PIXEL13_0
                HQ4X_PIXEL20_82
                HQ4X_PIXEL21_32
                HQ4X_PIXEL22_0
                HQ4X_PIXEL23_0
                HQ4X_PIXEL30_82
                HQ4X_PIXEL31_32
                HQ4X_PIXEL32_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL33_20
                }
            acase 255:
                if (Diff(w4, w2))
                {   HQ4X_PIXEL00_0
                } else
                {   HQ4X_PIXEL00_20
                }
                HQ4X_PIXEL01_0
                HQ4X_PIXEL02_0
                if (Diff(w2, w6))
                {   HQ4X_PIXEL03_0
                } else
                {   HQ4X_PIXEL03_20
                }
                HQ4X_PIXEL10_0
                HQ4X_PIXEL11_0
                HQ4X_PIXEL12_0
                HQ4X_PIXEL13_0
                HQ4X_PIXEL20_0
                HQ4X_PIXEL21_0
                HQ4X_PIXEL22_0
                HQ4X_PIXEL23_0
                if (Diff(w8, w4))
                {   HQ4X_PIXEL30_0
                } else
                {   HQ4X_PIXEL30_20
                }
                HQ4X_PIXEL31_0
                HQ4X_PIXEL32_0
                if (Diff(w6, w8))
                {   HQ4X_PIXEL33_0
                } else
                {   HQ4X_PIXEL33_20
            }   }

            switch (fastsize)
            {
            case 6: // 4*4
                pp = (x *  4) + (y * 16 * machines[machine].width);
                COPYDISP2(    pp                                     , d1 );
                COPYDISP2(    pp + 1                                 , d2 );
                COPYDISP2(    pp + 2                                 , d3 );
                COPYDISP2(    pp + 3                                 , d4 );
                COPYDISP2(    pp +     (machines[machine].width *  4), d5 );
                COPYDISP2(    pp + 1 + (machines[machine].width *  4), d6 );
                COPYDISP2(    pp + 2 + (machines[machine].width *  4), d7 );
                COPYDISP2(    pp + 3 + (machines[machine].width *  4), d8 );
                COPYDISP2(    pp +     (machines[machine].width *  8), d9 );
                COPYDISP2(    pp + 1 + (machines[machine].width *  8), d10);
                COPYDISP2(    pp + 2 + (machines[machine].width *  8), d11);
                COPYDISP2(    pp + 3 + (machines[machine].width *  8), d12);
                COPYDISP2(    pp +     (machines[machine].width * 12), d13);
                COPYDISP2(    pp + 1 + (machines[machine].width * 12), d14);
                COPYDISP2(    pp + 2 + (machines[machine].width * 12), d15);
                COPYDISP2(    pp + 3 + (machines[machine].width * 12), d16);
            acase 7: // 8*4
                pp = (x * 8) + (y * 32 * machines[machine].width);
                COPYWIDEDISP2(pp                                     , d1 );
                COPYWIDEDISP2(pp + 2                                 , d2 );
                COPYWIDEDISP2(pp + 4                                 , d3 );
                COPYWIDEDISP2(pp + 6                                 , d4 );
                COPYWIDEDISP2(pp +     (machines[machine].width *  8), d5 );
                COPYWIDEDISP2(pp + 2 + (machines[machine].width *  8), d6 );
                COPYWIDEDISP2(pp + 4 + (machines[machine].width *  8), d7 );
                COPYWIDEDISP2(pp + 6 + (machines[machine].width *  8), d8 );
                COPYWIDEDISP2(pp +     (machines[machine].width * 16), d9 );
                COPYWIDEDISP2(pp + 2 + (machines[machine].width * 16), d10);
                COPYWIDEDISP2(pp + 4 + (machines[machine].width * 16), d11);
                COPYWIDEDISP2(pp + 6 + (machines[machine].width * 16), d12);
                COPYWIDEDISP2(pp +     (machines[machine].width * 24), d13);
                COPYWIDEDISP2(pp + 2 + (machines[machine].width * 24), d14);
                COPYWIDEDISP2(pp + 4 + (machines[machine].width * 24), d15);
                COPYWIDEDISP2(pp + 6 + (machines[machine].width * 24), d16);
}   }   }   }
