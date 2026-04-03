// INCLUDES---------------------------------------------------------------

#define MAJORVERSION   0x04
#define MINORVERSION   0x50
#define INTEGERVERSION "4.50" // for RetroAchievements

#include <jni.h>

// DEFINES----------------------------------------------------------------

#define EXPORT
#define IMPORT    extern
#define MODULE    static
#define PERSIST   static
#define FAST      static
#define TRANSIENT auto
#define acase     break; case
#define adefault  break; default
#define elif      else if
#define FALSE     0
#define TRUE      (~0)
#define KILOBYTE  1024
#define EOS       0
#define DISCARD   (void)
typedef signed   char  FLAG;
typedef signed   char  SBYTE;
typedef unsigned char  UBYTE;
typedef          char  TEXT;
typedef signed   short SWORD;
typedef unsigned short UWORD;
typedef signed   long  SLONG;
typedef unsigned long  ULONG;
typedef char*          STRPTR;
typedef const char*    CONST_STRPTR;
typedef unsigned char  ASCREEN;

#define BOXWIDTH           164
#define BOXHEIGHT          269 // NTSC is only 226
#define MAXBOXWIDTH        240
#define MAXBOXHEIGHT       269
#define COINOP_BOXWIDTH    240
#define COINOP_BOXHEIGHT   240
#define UVI_HIDELEFT         6 // can be up to 14
#define PVI_HIDELEFT        11
#define PVI_RASTLINES      312
#define USG_XMARGIN         49
#define USG_YMARGIN         n1
#define HIDDEN_X            16 // for coin-ops
#define PVI_XSIZE          256 // 227 is unsuitable for Galaxia

// Arcadia
#define A_GOLFPOS1              0
#define BASEBALLPOS             1
#define ESCAPEPOS               2
#define MACROSSPOS              3
#define PARASHOOTERPOS          4
#define ROBOTKILLERPOS          5
#define SPACESQUADRON1POS       6
#define SPACESQUADRON2POS       7
#define SPACEVULTURESPOS        8
#define TETRISPOS               9
#define _2DSOCCERPOS           11
#define _3DSOCCERAPOS          12
#define _3DSOCCERBPOS          13
#define ALIENINVPOS            14
#define ASTROINVPOS            15
#define AUTORACEPOS            16
#define A_BASKETBALLPOS        17
#define BATTLEPOS              18
#define A_BLACKJACKPOS         19
#define A_BOWLINGPOS           20
#define A_BOXINGPOS            21
#define BRAINQUIZPOS           22
#define BREAKAWAYPOS           23
#define A_CAPTUREPOS           24
#define CATTRAXPOS             25
#define A_CIRCUSPOS            26
#define A_COMBATPOS            27
#define CRAZYCLIMBERPOS        28
#define CRAZYGOBBLERPOS        29
#define DORAEMONPOS            30
#define DRSLUMPPOS             31
#define FUNKYFISHPOS           32
#define GRIDIRON1POS           33
#define HOBOPOS1               34
#define A_HORSERACINGPOS       35
#define JUMPBUG1POS            36
#define JUMPBUG2POS            37
#define JUNGLERPOS             38
#define MISSILEWARPOS          39
#define MONACOPOS              40
#define NIBBLEMENPOS           41
#define OCEANBATTLEPOS         42
#define PLEIADESPOS            43
#define R2DTANKPOS             44
#define REDCLASHPOS            45
#define ROUTE16POS             46
#define SPACEATTACKAPOS        47
#define SPACEATTACKBPOS        48
#define SPACEATTACKCPOS        49
#define SPACEMISSIONPOS        50
#define SPIDERSPOS             51
#define SPACERAIDERSPOS        52
#define STARCHESSPOS           53
#define SUPERGOBBLERPOS        54
#define TANKSALOTPOS           55
#define TENNISPOS              56
#define THEENDPOS1             57
#define TURTLESPOS             58
#define VIDLEXEGPOS            59
#define VIDLEXGEPOS            60
#define ALIENINV1POS           61
#define ALIENINV2POS           62
#define ALIENINV3POS           63
#define ALIENINV4POS           64
#define A_GOLFPOS2             65
#define STARCHESSENHPOS        66
#define MOTHERSHIPPOS          78
#define THEENDPOS2             90
#define HOBOPOS2               91
#define _3DSOCCERENHPOS        93
#define GRIDIRON2POS           94
#define SUPERBUG1POS           97
#define GUNDAMPOS              98
#define JTRON121POS            99
#define SUPERBUG2POS          102
#define FROGGER1POS           104
#define _3DATTACKPOS          105
#define IKANASHIPOS           111
#define ARCADIASTUBPOS        118
#define ASTROINVODPOS         119
#define AUTORACEODPOS         120
#define A_COMBATODPOS         121
#define A_GOLFODPOS           122
#define REDCLASHODPOS         123
#define _2DSOCCERODPOS        124
#define SPIDERSODPOS          125
#define JOURNEYPOS            126
#define SPACEBUSTERPOS        127
#define JTRON10POS            128
#define JTRON11POS            129
#define FROGGER2POS           452
#define FROGGER3POS           453
#define HOMERUNPOS            456
// Interton
#define SUPERINVPOS           131
#define INTERTONSTUBPOS       132
#define INTELLIGENCE1POS      133
#define SUPERMAZEPOS          134
#define MONSTERMANPOS         135
#define COMECOMEPOS           136
#define COMEFRUTASPOS1        137
#define COMEFRUTASPOS2        139
#define KABOOM1POS            141
#define KABOOM2POS            142
#define _4INAROWPOS           144
#define AIRSEABATTLEPOS       145
#define BACKGAMMONPOS         146
#define I_BLACKJACKPOS        147
#define I_BOWLINGPOS          148
#define I_BOXINGPOS           149
#define I_CAPTUREPOS          150
#define CASINOPOS             152
#define CHESS1POS             153
#define CHESS2POS             154
#define I_CIRCUSPOS           155
#define COCKPITPOS            156
#define MASTERMINDPOS         157
#define I_COMBATBPOS          159
#define DRAUGHTSPOS           160
#define I_GOLFPOS             161
#define CARRACESPOS           162
#define HEADONPOS             163
#define HIPPODROMEPOS         164
#define HUNTINGPOS            165
#define HYPERSPACEPOS         166
#define INVADERAPOS           167
#define INVADERBPOS           168
#define LASERATTACKPOS        169
#define I_MATH1POS            170
#define I_MATH2POS            171
#define METROPOLISPOS         172
#define MOTOCROSSPOS          173
#define MUSICALGAMESPOS       174
#define PADDLEGAMESPOS        175
#define PINBALLBPOS           177
#define SOCCERBPOS            181
#define SOLITAIREPOS          182
#define SPACEWARPOS           183
#define SUPERSPACEPOS         184
#define MEMORY1POS            185
#define WINTERSPORTSPOS       186
#define LEAPFROGPOS           188
#define SUPERKNOCKOUTPOS      193
#define AIRSEAATTACKPOS       194
#define COMEFRUTASPOS3        201
#define I_TETRISPOS1          206
#define I_TETRISPOS2          207
#define I_TETRISPOS3          208
// Elektor
#define MAZESPOS              236
#define CIRCLEDRIVEPOS1       237
#define COSMICADVENTUREPOS1   249
#define COSMICADVENTUREPOS2   253
#define ROCKETSHOOTINGPOS     263
#define SPACESHOOTOUTPOS      268
#define UFOSHOOTINGPOS        286
#define E_PINBALLPOS          290
#define HELICOPTERPOS         291
#define ATTACKFROMSPACEPOS    295
#define BURSTINGBALLOONSPOS   297
#define ASTEROIDSPOS          303
#define OMEGALANDINGPOS       304
#define BREAKOUTPOS           305
#define TINYTIMPOS            306
#define SUBMARINESRACINGPOS   311
#define CIRCLEDRIVEPOS2       312
#define QUEENPOS              354
#define SPACEBATTLEPOS        355
#define E_MATH1POS            356
#define PENALTYPOS            391
#define MOONLANDING1POS       392
#define MOONLANDING2POS       393
#define E_CARRACESPOS         397
#define ENTERPRISE1POS1       402
#define ENTERPRISE1POS2       403
#define HAMISH1POS1           404
#define LAUNCHINGPOS1         405
#define LAUNCHINGPOS2         409
#define HAMISH1POS2           411
#define HAMISH1POS3           415
#define ENTERPRISE3POS        423
#define CHICKEN1POS           424
#define CHICKEN2POS           425
#define CHICKEN3POS           438
// all
#define KNOWNGAMES           (456 + 1)

#define OVERLAYS             (278 + 1)
#define     ARCADIA_OVERLAY     0
#define     ELEKTOR_OVERLAY     1
#define    INTERTON_OVERLAY    15
#define       BLANK_OVERLAY   149
#define   ASTROWARS_OVERLAY   160
#define     MALZAK1_OVERLAY   161
#define     MALZAK2_OVERLAY   173
#define LASERBATTLE_OVERLAY   197
#define     GALAXIA_OVERLAY   231

#define NUMKEYS                28 // counting from 1 (last array index is NUMKEYS-1)

#define PSU_S            128 // %10000000 read-only
#define PSU_F             64 // %01000000
#define PSU_II            32 // %00100000
#define PSU_SP             7 // %00000111
#define PSU_WRITABLE_A  0x67 // %01100111

#define PSL_CC           192 // %11000000
#define PSL_IDC           32
#define PSL_RS            16
#define PSL_WC             8
#define PSL_OVF            4
#define PSL_COM            2
#define PSL_C              1

#define A_SPRITE0Y       6384 // $18F0 R/W
#define A_SPRITE0X       6385 // $18F1 R/W
#define A_SPRITE1Y       6386 // $18F2 R/W
#define A_SPRITE1X       6387 // $18F3 R/W
#define A_SPRITE2Y       6388 // $18F4 R/W
#define A_SPRITE2X       6389 // $18F5 R/W
#define A_SPRITE3Y       6390 // $18F6 R/W
#define A_SPRITE3X       6391 // $18F7 R/W
#define A_VSCROLL        6396 // $18FC -/W?
#define A_PITCH          6397 // $18FD -/W?
#define A_VOLUME         6398 // $18FE -/W?
#define A_CHARLINE       6399 // $18FF (dma)
#define A_P1LEFTKEYS     6400 // $1900
#define A_P1MIDDLEKEYS   6401 // $1901
#define A_P1RIGHTKEYS    6402 // $1902
/* #define A_P1PALLADIUM 6403 // $1903
#define A_P2LEFTKEYS     6404 // $1904
#define A_P2MIDDLEKEYS   6405 // $1905
#define A_P2RIGHTKEYS    6406 // $1906
#define A_P2PALLADIUM    6407 // $1907 */
#define A_CONSOLE        6408 // $1908
#define A_GFXMODE        6648 // $19F8
#define A_BGCOLOUR       6649 // $19F9
#define A_SPRITES23CTRL  6650 // $19FA
#define A_SPRITES01CTRL  6651 // $19FB
#define A_BGCOLLIDE      6652 // $19FC
#define A_SPRITECOLLIDE  6653 // $19FD
#define A_P2PADDLE       6654 // $19FE
#define A_P1PADDLE       6655 // $19FF

#define IE_NOISE         0x1E80
#define IE_P1LEFTKEYS    0x1E88
#define IE_P1MIDDLEKEYS  0x1E89
#define IE_P1RIGHTKEYS   0x1E8A
#define IE_CONSOLE       0x1E8B
/* #define IE_P2LEFTKEYS 0x1E8C
#define IE_P2MIDDLEKEYS  0x1E8D
#define IE_P2RIGHTKEYS   0x1E8E */

#define E_RANDOM2          0x1D20
#define E_LINEIN0          0x1D9C
#define E_LINEIN1          0x1D9D
#define E_LINEIN2          0x1D9E
#define E_LINEIN3          0x1D9F
#define E_LINEIN4          0x1DBC
#define E_LINEIN5          0x1DBD
#define E_LINEIN6          0x1DBE
#define E_LINEIN7          0x1DBF // aka CASIN
// #define E_CASIN         0x1DBF // aka LINEIN7
#define E_LINEOUT0         0x1DDC
#define E_LINEOUT1         0x1DDD
#define E_LINEOUT2         0x1DDE
#define E_LINEOUT3         0x1DDF
#define E_LINEOUT4         0x1DFC
#define E_LINEOUT5         0x1DFD
#define E_LINEOUT6         0x1DFE
#define E_LINEOUT7         0x1DFF // aka CASOUT
// #define E_CASOUT        0x1DFF // aka LINEOUT7
#define E_RANDOM1G         0x1FAF
#define E_RANDOM1E         0x1FFF

#define PVI_SPRITE0          0x00
// #define PVI_SPRITE0AX     0x0A
#define PVI_SPRITE0BX        0x0B
#define PVI_SPRITE0AY        0x0C
#define PVI_SPRITE0BY        0x0D
#define PVI_SPRITE1          0x10
// #define PVI_SPRITE1AX     0x1A
#define PVI_SPRITE1BX        0x1B
#define PVI_SPRITE1AY        0x1C
#define PVI_SPRITE1BY        0x1D
#define PVI_SPRITE2          0x20
// #define PVI_SPRITE2AX     0x2A
#define PVI_SPRITE2BX        0x2B
#define PVI_SPRITE2AY        0x2C
#define PVI_SPRITE2BY        0x2D
#define PVI_SPRITE3          0x40
// #define PVI_SPRITE3AX     0x4A
#define PVI_SPRITE3BX        0x4B
#define PVI_SPRITE3AY        0x4C
#define PVI_SPRITE3BY        0x4D
#define PVI_VERTGRID         0x80
#define PVI_HORIZ1           0xA8
/* #define PVI_HORIZ2           0xA9
#define PVI_HORIZ3           0xAA
#define PVI_HORIZ4           0xAB
#define PVI_HORIZ5           0xAC */
#define PVI_SIZES            0xC0
#define PVI_SPR01COLOURS     0xC1
#define PVI_SPR23COLOURS     0xC2
#define PVI_SCORECTRL        0xC3
#define PVI_BGCOLOUR         0xC6
#define PVI_PITCH            0xC7
#define PVI_SCORELT          0xC8
#define PVI_SCORERT          0xC9
#define PVI_BGCOLLIDE        0xCA
#define PVI_SPRITECOLLIDE    0xCB
#define PVI_BGCOLLIDE_M2     0xDA
#define PVI_SPRITECOLLIDE_M2 0xDB
#define PVI_BGCOLLIDE_M3     0xEA
#define PVI_SPRITECOLLIDE_M3 0xEB
#define PVI_BGCOLLIDE_M4     0xFA
#define PVI_SPRITECOLLIDE_M4 0xFB
#define PVI_P1PADDLE         0xCC
#define PVI_P2PADDLE         0xCD

#define PSG_PITCHA1_L        0x00
#define PSG_PITCHA1_H        0x01
#define PSG_PITCHB1_L        0x02
#define PSG_PITCHB1_H        0x03
#define PSG_PITCHC1_L        0x04
#define PSG_PITCHC1_H        0x05
#define PSG_PITCHD1          0x06
#define PSG_MIXER1           0x07
#define PSG_AMPLITUDEA1      0x08
#define PSG_AMPLITUDEB1      0x09
#define PSG_AMPLITUDEC1      0x0A
/* #define PSG_PERIOD1_L     0x0B
#define PSG_PERIOD1_H        0x0C
#define PSG_SHAPE1           0x0D */
#define PSG_PORTA1           0x0E
// #define PSG_PORTB1        0x0F
#define PSG_PITCHA2_L        0x10
#define PSG_PITCHA2_H        0x11
#define PSG_PITCHB2_L        0x12
#define PSG_PITCHB2_H        0x13
#define PSG_PITCHC2_L        0x14
#define PSG_PITCHC2_H        0x15
#define PSG_PITCHD2          0x16
#define PSG_MIXER2           0x17
#define PSG_AMPLITUDEA2      0x18
#define PSG_AMPLITUDEB2      0x19
#define PSG_AMPLITUDEC2      0x1A
/* #define PSG_PERIOD2_L     0x1B
#define PSG_PERIOD2_H        0x1C
#define PSG_SHAPE2           0x1D
#define PSG_PORTA2           0x1E
#define PSG_PORTB2           0x1F */

#define REGION_NTSC             0
#define REGION_PAL              1
#define REGION_ANY              2

// memflags[] bitfield
#define NOREAD                   0x1
#define NOWRITE                  0x2
#define READONCE                 0x4
#define AUDIBLE                  0x8
#define RANDOM                  0x10
#define BANKED                  0x20 // for Zaccaria. Only Astro Wars and Galaxia actually use it
#define FIELD                   0x40 // for Malzak
#define SPECIALREAD             (RANDOM | READONCE | NOREAD)

#define ARCADIA                 0
#define INTERTON                1
#define ELEKTOR                 2
#define ZACCARIA                3
#define MALZAK                  4
#define MACHINES                5

#define MEMMAP_A                0 // Interton                    (2K ROM + 0K RAM)
#define MEMMAP_B                1 // Interton                    (4K ROM + 0K RAM)
#define MEMMAP_C                2 // Interton                    (4K ROM + 1K RAM)
#define MEMMAP_D                3 // Interton                    (6K ROM + 1K RAM)
#define MEMMAP_E                4 // Elektor                     (basic)
#define MEMMAP_F                5 // Elektor                     (expanded)
#define MEMMAP_G                6 // Arcadia    ( 512 bytes RAM) (Emerson)
// #define MEMMAP_H             7 // Arcadia    (1024 bytes RAM) (Tele-Fever)
#define MEMMAP_I                8 // Arcadia    ( 512 bytes RAM) (Palladium)
#define MEMMAP_ASTROWARS        9
#define MEMMAP_GALAXIA         10
#define MEMMAP_LASERBATTLE     11
#define MEMMAP_LAZARIAN        12
#define MEMMAP_MALZAK1         13
#define MEMMAP_MALZAK2         14
#define MEMMAPS                15

#define MEMMAP_ARCADIA        MEMMAP_G // default Arcadia memory map you prefer
#ifdef BASICMODE
    #define MEMMAP_ELEKTOR    MEMMAP_E
#else
    #define MEMMAP_ELEKTOR    MEMMAP_F
#endif

#define KABOOMCLICKS
// whether to play the click sample instead of a very short tone
// (sounds more authentic)

#define CHAN_OK                 0
#define CHAN_PLAY               1
#define CHAN_STOP               2
#define CHAN_PLAYTHENSTOP       3

#define EFLAG                TRUE // default flagline for most Elektor games

#define PREVSPRITES            56 // for Interton/Elektor demultiplexing

// #define A_OFFSET_UPPERSCREEN 0x1800
// #define A_OFFSET_LOWERSCREEN 0x1A00
#define A_OFFSET_SPRITES     0x1980
// #define A_OFFSET_UDCS        0x19A0

#define SENSITIVITY_VLOW        1
#define SENSITIVITY_LOW         2
#define SENSITIVITY_MEDIUM      3
#define SENSITIVITY_HIGH        4
#define SENSITIVITY_VHIGH       5
// #define SENSITIVITY_MIN      1
#define SENSITIVITY_DEFAULT     3
// #define SENSITIVITY_MAX      5

// These are for Interton/Elektor, not Arcadia!
// Interton/Elektor colours are referred to in inverted RGB format!
#define BLACK              7 // 0,0,0 = 0
#define BLUE               6 // 0,0,1 = 1
#define GREEN              5 // 0,1,0 = 2 (different to Arcadia!)
#define CYAN               4 // 0,1,1 = 3 (different to Arcadia!)
#define RED                3 // 1,0,0 = 4 (different to Arcadia!)
#define PURPLE             2 // 1,0,1 = 5 (different to Arcadia!)
#define YELLOW             1 // 1,1,0 = 6
#define WHITE              0 // 1,1,1 = 7

#define GREY1              8
#define BROWN              9
#define DARKPURPLE        10
#define DARKRED           11
#define DARKCYAN          12
#define DARKGREEN         13
#define DARKBLUE          14
#define DARKBLACK         15
/* #define GREY2          16
#define GREY3             17
#define GREY4             18
#define GREY5             19
#define GREY6             20
#define GREY7             21 */
#define ORANGE            22
/* #define PINK           23
#define DARKORANGE        DARKRED
#define DARKPINK          DARKRED */
#define GUESTCOLOURS      24
#define NOSKY             GUESTCOLOURS

#define DUMP_GOOD       0
#define DUMP_GOODOBS    DUMP_GOOD
#define DUMP_GOODENH    DUMP_GOOD
#define DUMP_OK         1
#define DUMP_OKOBS      DUMP_OK
// #define DUMP_OKENH   DUMP_OK
#define DUMP_SUSPECT    2
#define DUMP_BAD        3
// #define DUMP_BADOBS  DUMP_BAD

// computer languages
// #define CL_MISC   0 // miscellaneous
#define CL_MC     1 // machine code
// #define CL_TEXT   2 // text
// #define CL_MWB    3 // MicroWorld BASIC (PHUNSY)
// #define CL_LOTEL  4 // ?
// #define CL_FORTH  5
// #define CL_12KB   6 // 12K BASIC (CD2650)
// #define CL_MB     7 // Micro BASIC (PIPBUG)
#define CL_C2001  8 // Compiler 2001 aka Lightning BASIC (Arcadia)
// #define CL_8KB13  9 // 8K BASIC 1.3 (CD2650). Nothing known that uses 8K BASIC 1.0
// #define CL_TCT   10 // TCT BASIC (PIPBUG)
// #define CL_DATA  CL_MISC

#define KIND_BIN                0
#define KIND_COS                1
#define KIND_COR                2
#define KIND_TVC                3
// #define KINDS                4 // counting from 1
// #define KIND_NIL             (KINDS + 1)

#define ELEKTOR_PHILIPS         0
#define ELEKTOR_HOBBYMODULE     1

#define FIRSTGUESTTONE    0
#define TONE_1STXVI       0 // all
#define TONE_2NDXVI       1
#define TONE_3RDXVI       2
#define TONE_1STLB1       3 // Laser Battle & Lazarian
#define TONE_A1           3 // Expanded Elektor
#define TONE_1STLB2       4 // Laser Battle & Lazarian
#define TONE_B1           4 // Expanded Elektor
#define TONE_2NDLB1       5 // Laser Battle & Lazarian
#define TONE_C1           5 // Expanded Elektor
#define TONE_2NDLB2       6 // Laser Battle & Lazarian
#define TONE_A2           6 // Expanded Elektor
#define TONE_B2           7 // Expanded Elektor
#define TONE_C2           8 // Expanded Elektor
#define LASTGUESTTONE     8
// #define TONECHANNELS  (LASTGUESTTONE - FIRSTGUESTTONE + 1)
#define FIRSTGUESTNOISE   9
#define NOISE_1STXVI      9 // Arcadia, Interton, expanded Elektor
#define NOISE_A1         10 // Expanded Elektor
#define NOISE_B1         11 // Expanded Elektor
#define NOISE_C1         12 // Expanded Elektor
#define NOISE_A2         13 // Expanded Elektor
#define NOISE_B2         14 // Expanded Elektor
#define NOISE_C2         15 // Expanded Elektor
#define LASTGUESTNOISE   15
// #define NOISECHANNELS (LASTGUESTNOISE - FIRSTGUESTNOISE + 1)
#define GUESTCHANNELS    16
#define SAMPLE_EXPLOSION  0 // Interton, expanded Elektor
#define SAMPLE_SHOOT      1 // Astro Wars, Galaxia
#define SAMPLE_BANG       2 // Astro Wars, Galaxia
#define SAMPLE_COIN       3 // coin-ops
#define SAMPLE_CLICK      4 // Interton, expanded Elektor
#define SAMPLE_CHEEVOS    5 // Arcadia, Interton, Elektor
#define SAMPLES           6 // counting from 1
#define TOTALCHANNELS    (GUESTCHANNELS + SAMPLES)
#define MAXSAMPLERATE 11025
#define SOUNDLENGTH      (MAXSAMPLERATE * 2) // enough for two seconds (we only use just over one second though)
                         // also must be >= LEN_#?

#define SERIALIZE_READ          0
#define SERIALIZE_WRITE         1

#define HZ_REST (float)   0.000
// #define HZ_A0  (float)   27.500
// #define HZ_AS0 (float)   29.135
#define HZ_B0  (float)   30.868
#define HZ_C1  (float)   32.703
#define HZ_CS1 (float)   34.648
#define HZ_D1  (float)   36.708
#define HZ_DS1 (float)   38.891
#define HZ_E1  (float)   41.203
#define HZ_F1  (float)   43.654
#define HZ_FS1 (float)   46.249
#define HZ_G1  (float)   48.999
#define HZ_GS1 (float)   51.913
#define HZ_A1  (float)   55.000
#define HZ_AS1 (float)   58.270
#define HZ_B1  (float)   61.735
#define HZ_C2  (float)   65.406
#define HZ_CS2 (float)   69.296
#define HZ_D2  (float)   73.416
#define HZ_DS2 (float)   77.782
#define HZ_E2  (float)   82.407
#define HZ_F2  (float)   87.307
#define HZ_FS2 (float)   92.499
#define HZ_G2  (float)   97.999
#define HZ_GS2 (float)  103.826
#define HZ_A2  (float)  110.000
#define HZ_AS2 (float)  116.541
#define HZ_B2  (float)  123.471
#define HZ_C3  (float)  130.813
#define HZ_CS3 (float)  138.591
#define HZ_D3  (float)  146.832
#define HZ_DS3 (float)  155.563
#define HZ_E3  (float)  164.814
#define HZ_F3  (float)  174.614
#define HZ_FS3 (float)  184.997
#define HZ_G3  (float)  195.998
#define HZ_GS3 (float)  207.652
#define HZ_A3  (float)  220.000
#define HZ_AS3 (float)  233.082
#define HZ_B3  (float)  246.942
#define HZ_C4  (float)  261.626
#define HZ_CS4 (float)  277.183
#define HZ_D4  (float)  293.665
#define HZ_DS4 (float)  311.127
#define HZ_E4  (float)  329.628
#define HZ_F4  (float)  349.238
#define HZ_FS4 (float)  369.994
#define HZ_G4  (float)  391.995
#define HZ_GS4 (float)  415.305
#define HZ_A4  (float)  440.000
#define HZ_AS4 (float)  466.164
#define HZ_B4  (float)  493.883
#define HZ_C5  (float)  523.251
#define HZ_CS5 (float)  554.365
// #define HZ_D5  (float)  587.330
#define HZ_DS5 (float)  622.254
#define HZ_E5  (float)  659.255
#define HZ_F5  (float)  698.456
// #define HZ_FS5 (float)  739.989
#define HZ_G5  (float)  783.991
// #define HZ_GS5 (float)  830.609
#define HZ_A5  (float)  880.000
// #define HZ_AS5 (float)  932.328
#define HZ_B5  (float)  987.767
// #define HZ_C6  (float) 1046.502
#define HZ_CS6 (float) 1108.731
// #define HZ_D6  (float) 1174.659
// #define HZ_DS6 (float) 1244.508
#define HZ_E6  (float) 1318.510
// #define HZ_F6  (float) 1396.913
// #define HZ_FS6 (float) 1479.978
#define HZ_G6  (float) 1567.982
// #define HZ_GS6 (float) 1661.219
// #define HZ_A6  (float) 1760.000
// #define HZ_AS6 (float) 1864.655
#define HZ_B6  (float) 1975.533
// #define HZ_C7  (float) 2093.005
// #define HZ_CS7 (float) 2217.461
// #define HZ_D7  (float) 2349.318
// #define HZ_DS7 (float) 2489.016
#define HZ_E7  (float) 2637.020
// #define HZ_F7  (float) 2793.826
// #define HZ_FS7 (float) 2959.955
// #define HZ_G7  (float) 3135.963
// #define HZ_GS7 (float) 3322.438
// #define HZ_A7  (float) 3520.000
// #define HZ_AS7 (float) 3729.310
#define HZ_B7  (float) 3951.066
// #define HZ_C8  (float) 4186.009
// #define HZ_CS8 (float) 4434.922
// #define HZ_D8  (float) 4698.636
// #define HZ_DS8 (float) 4978.032
// #define HZ_E8  (float) 5274.041
// #define HZ_F8  (float) 5587.652
// #define HZ_FS8 (float) 5919.911
// #define HZ_G8  (float) 6271.927
// #define HZ_GS8 (float) 6644.875
// #define HZ_A8  (float) 7040.000
// #define HZ_AS8 (float) 7458.620
// #define HZ_B8  (float) 7902.133
// #define HZ_C9  (float) 8372.018

#define W0               0 // 0-way
#define W2H              1 // 2-way horizontal
#define W2V              2 // 2-way vertical
#define W4O              3 // 4-way orthagonal
#define W4D              4 // 4-way diagonal
#define W8               5 // 8-way
#define WAYS_ANALOG      6
#define WA               WAYS_ANALOG

#define SPEED_4QUARTERS  3

#define PORTC          256
#define PORTD          257

//  0..50: Arcadia
// 51..56: Coin-ops
#define HS_ASTROWARS_POS    51
#define HS_GALAXIA_POS      52
#define HS_LASERBATTLE_POS  53
#define HS_LAZARIAN_POS     54
#define HS_MALZAK1_POS      55
#define HS_MALZAK2_POS      56
#define HISCORES            57 // ie. [0..56]

// EXPORTED STRUCTURES----------------------------------------------------

EXPORT struct KnownStruct
{   UBYTE  machine,
           memmap;
    ULONG  crc32,
           crc64_h,
           crc64_l;
    UWORD  size;
    SBYTE  key1,
           key2,
           key3,
           key4,
           up,
           down,
           left,
           right;
    FLAG   analog;
    UBYTE  sensitivity,
           demultiplex,
           spriteflips; // PIPBUG: pipbug_biosver, SI50: s_id
    SBYTE  udcflips,    // SI50: s_is
           firstrow,
           lastrow;
    FLAG   swapped;
    int    flagline;    // Arcadia: flagline, Interton/Elektor: darkenbg, PIPBUG: pipbug_baudrate, BINBUG: vdu, SI50: s_io, CD2650: vdu
    UBYTE  downframes,
           totalframes;
    UBYTE  dump;
    SBYTE  p1sprcol[6],
           p1bgcol[4],  // not UBYTE!
           p2sprcol[6],
           p2bgcol[4];  // not UBYTE!
    STRPTR ggnode,
           name,
           credits,
           reference;
    SWORD  whichoverlay;
    SBYTE  hiscore;     // not UBYTE!
    SWORD  firstequiv,
           lastequiv,
           firstcodecomment,
           lastcodecomment,
           firstdatacomment,
           lastdatacomment;
    UBYTE  complang;
    UWORD  startaddr;
    UBYTE  banked;
    ULONG  the1staddr;
    UWORD  the1stsize;
    ULONG  the2ndaddr;
    UWORD  the2ndsize;
    UBYTE  cpl,
           bios;
};

EXPORT struct MachineStruct
{ // initialized
    int    width,
           height,
           memmap,
           region;
    int    keys;
    UBYTE  readonce,
           cosversion;
    double cpf;
    int    pvis,
           digits;
    UBYTE  digipos[3];
    int    pvibase;
    FLAG   coinop;
    STRPTR consolekeyname[4];
    int    hiscores;
};

EXPORT struct HiScoreStruct
{   UWORD hipos[4];
    UBYTE length;
    SBYTE nextdigit; // must be signed!
    ULONG score;
    TEXT  date[8 + 1]; // enough for "31/12/99"
};

EXPORT struct MemMapInfoStruct
{   const int    machine;
    CONST_STRPTR name;
    const int    overlay;
    CONST_STRPTR credits;
};

// EXPORTED FUNCTIONS-----------------------------------------------------

// 2650.c
EXPORT void cpu_2650(void);
EXPORT void one_instruction(void);
EXPORT void pullras(void);
EXPORT void pushras(void);
EXPORT void checkinterrupt(void);

// arcadia.c
EXPORT void uvi(void);
EXPORT void uviwrite(UWORD address, UBYTE data);
EXPORT void arcadia_setmemmap(void);
EXPORT void arcadia_serializerom(void);

// interton.c
EXPORT void interton_setmemmap(void);
EXPORT void interton_serializerom(void);

// elektor.c
EXPORT void elektor_setmemmap(void);

// android.c
EXPORT void drawpixel(int x, int y, int colour);
EXPORT void drawbgpixel(int x, int y, int colour);
EXPORT void changemachine(int whichmachine, int whichmemmap, FLAG user, FLAG force, FLAG same);
EXPORT void changepixel(int x, int y, int colour);
EXPORT void changebgpixel(int x, int y, int colour);
EXPORT int parse_bytes(FLAG reconfigure);
EXPORT void unautosense(void);
EXPORT void generate_tone(int guestchan, double hertz);
EXPORT void generate_noise(int guestchan, double hertz);
EXPORT void set_volumes(int guestchan, int volume);
EXPORT void playsound(FLAG force);
EXPORT void engine_reset(void);
EXPORT void update_margins(void);
EXPORT void play_any(int guestchan, float hertz, int volume);
EXPORT void configure(FLAG same, FLAG all);
EXPORT void generate_crc32table(void);
EXPORT ULONG crc32(const UBYTE* address, int thesize);
// EXPORT void crc64(UBYTE* address, int thesize);
EXPORT void generate_autotext(void);
EXPORT void patchrom(void);
EXPORT void serialize_cos(void);
EXPORT void serialize_byte(UBYTE* var);
EXPORT void serialize_byte_int(int* var);
EXPORT void serialize_word(UWORD* var);
EXPORT void serialize_word_int(int* var);
EXPORT void serialize_long(ULONG* var);
EXPORT UBYTE ReadByte(void);
EXPORT UBYTE ReadByteAt(int where);
EXPORT UWORD ReadWord(void);
EXPORT UWORD ReadWordAt(int where);
EXPORT ULONG ReadLong(void);
EXPORT void WriteByte(UBYTE what);
EXPORT void WriteWord(UWORD what);
EXPORT void WriteLong(ULONG what);
EXPORT UBYTE readport(int port);
EXPORT void writeport(int port, UBYTE data);
EXPORT void command_changemachine(int whichmachine, int whichmemmap);
EXPORT void play_sample(int sample);
EXPORT void sound_stop(int sample);
EXPORT void set_cpl(int newcpl);

// pvi.c
EXPORT void changeabspixel(int x, int y, int colour);
EXPORT void changethisabspixel(int colour);
EXPORT void changethisbgpixel(int colour);
EXPORT void changerelpixel(int x, int y, int colour);
EXPORT void pvi_vblank(void);
EXPORT void pviwrite(signed int address, UBYTE data, FLAG ispvi);
EXPORT void newpvi(void);
EXPORT void ie_inputmirrors(int address);
EXPORT void pvi_memmap(void);
EXPORT void set_retuning(void);

// coin-ops.c
EXPORT void coinop_emuinput(void);
EXPORT void oldpvi(void);
EXPORT void serialize_coinops(void);

// malzak.c
EXPORT void malzak_drawscreen(void);
EXPORT void switchbank(int whichbank);
EXPORT void malzak_setmemmap(void);
EXPORT UBYTE malzak_readport(int port);
EXPORT void malzak_writeport(int port, UBYTE data);
EXPORT void malzak_emuinput(void);

// zaccaria.c
EXPORT void astrowars_setmemmap(void);
EXPORT void galaxia_setmemmap(void);
EXPORT void astrowars_drawscreen(void);
EXPORT void galaxia_drawscreen(void);
EXPORT UBYTE zaccaria_readport(int port);
EXPORT void zaccaria_writeport(int port, UBYTE data);
EXPORT void zaccaria_emuinput(void);
EXPORT void lb_setmemmap(void);
EXPORT void lb_drawscreen(void);

// Java-accessible functions for MainActivity.java
JNIEXPORT jboolean   JNICALL Java_com_amigan_droidarcadia_MainActivity_canpause(         JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_MainActivity_cgetoptions1(     JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_MainActivity_cgetoptions2(     JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_MainActivity_cloadcos(         JNIEnv* env, jobject this, jint whichtype, jint length, jbyteArray ptr, jboolean reconfigure);
JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_MainActivity_clientidle(       JNIEnv* env, jobject this);
JNIEXPORT jbyteArray JNICALL Java_com_amigan_droidarcadia_MainActivity_csavecos(         JNIEnv* env, jobject this);
JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_MainActivity_csoundoff(        JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_MainActivity_doautofire(       JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_MainActivity_emulate(          JNIEnv* env, jobject this);
JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_MainActivity_endofframe(       JNIEnv* env, jobject this);
JNIEXPORT jstring    JNICALL Java_com_amigan_droidarcadia_MainActivity_getbadgegfxurl(   JNIEnv *env, jobject this);
JNIEXPORT jstring    JNICALL Java_com_amigan_droidarcadia_MainActivity_getgamegfxurl(    JNIEnv *env, jobject this);
// JNIEXPORT jint    JNICALL Java_com_amigan_droidarcadia_MainActivity_getcycles(        JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_MainActivity_getdigipos(       JNIEnv* env, jobject this, jint whichpos);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_MainActivity_getff(            JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_MainActivity_getfirebuttons(   JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_MainActivity_getgame(          JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_MainActivity_gethiscore(       JNIEnv* env, jobject this, jint whichscore);
JNIEXPORT jstring    JNICALL Java_com_amigan_droidarcadia_MainActivity_gethiscoredate(   JNIEnv* env, jobject this, jint whichscore);
JNIEXPORT jbyteArray JNICALL Java_com_amigan_droidarcadia_MainActivity_getkeyname(       JNIEnv* env, jobject this, jint whichkey);
JNIEXPORT jstring    JNICALL Java_com_amigan_droidarcadia_MainActivity_getmessage(       JNIEnv* env, jobject this);
JNIEXPORT jstring    JNICALL Java_com_amigan_droidarcadia_MainActivity_getprogressstr(   JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_MainActivity_getracolour(      JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_MainActivity_getsensitivity(   JNIEnv* env, jobject this);
JNIEXPORT jshortArray JNICALL Java_com_amigan_droidarcadia_MainActivity_getsoundbuffer(  JNIEnv* env, jobject this, jint whichchan);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_MainActivity_getstarting(      JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_MainActivity_getstopping(      JNIEnv* env, jobject this);
JNIEXPORT jstring    JNICALL Java_com_amigan_droidarcadia_MainActivity_gettrackerstr(    JNIEnv* env, jobject this);
JNIEXPORT jboolean   JNICALL Java_com_amigan_droidarcadia_MainActivity_isbadgegfx(       JNIEnv* env, jobject this);
JNIEXPORT jboolean   JNICALL Java_com_amigan_droidarcadia_MainActivity_isprogressing(    JNIEnv *env, jobject this);
JNIEXPORT jboolean   JNICALL Java_com_amigan_droidarcadia_MainActivity_istracking(       JNIEnv *env, jobject this);
JNIEXPORT jboolean   JNICALL Java_com_amigan_droidarcadia_MainActivity_jsetravars(       JNIEnv* env, jobject this, jstring jusername, jstring jpassword, jboolean jcheevos, jboolean jhardcore);
JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_MainActivity_jshutdownraclient(JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_MainActivity_loadgame(         JNIEnv* env, jobject this, jstring passedname, jint length, jbyteArray ptr);
JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_MainActivity_redrawscreen(     JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_MainActivity_setconsolekey(    JNIEnv* env, jobject this, jint whichkey);
JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_MainActivity_setframebuffer(   JNIEnv* env, jobject this, jobject oBuf);
JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_MainActivity_sethiscore(       JNIEnv* env, jobject this, jint whichscore, jint thescore, jstring thedate);
JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_MainActivity_setinput(         JNIEnv* env, jobject this, jint player, jint thekeypad, jint cx, jint cy);
// JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_MainActivity_cloadnvram(    JNIEnv* env, jobject this, jbyteArray ptr);
// JNIEXPORT jbyteArray JNICALL Java_com_amigan_droidarcadia_MainActivity_csavenvram(    JNIEnv* env, jobject this);

// Java-accessible functions for OptionsActivity.java
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_OptionsActivity_cgetoptions1(  JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_OptionsActivity_cgetoptions2(  JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_OptionsActivity_cloadcos(      JNIEnv* env, jobject this, jint whichtype, jint length, jbyteArray ptr, jboolean reconfigure);
JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_OptionsActivity_creset(        JNIEnv* env, jobject this);
JNIEXPORT jbyteArray JNICALL Java_com_amigan_droidarcadia_OptionsActivity_csavecos(      JNIEnv* env, jobject this);
JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_OptionsActivity_csoundoff(     JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_OptionsActivity_getdigipos(    JNIEnv* env, jobject this, jint whichpos);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_OptionsActivity_getfirebuttons(JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_OptionsActivity_getgame(       JNIEnv* env, jobject this);
JNIEXPORT jboolean   JNICALL Java_com_amigan_droidarcadia_OptionsActivity_jsetravars(    JNIEnv* env, jobject this, jstring jusername, jstring jpassword, jboolean jcheevos, jboolean jhardcore);
JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_OptionsActivity_redrawscreen(  JNIEnv* env, jobject this);
JNIEXPORT jboolean   JNICALL Java_com_amigan_droidarcadia_OptionsActivity_skiplevel(     JNIEnv* env, jobject this);
JNIEXPORT jbyteArray JNICALL Java_com_amigan_droidarcadia_OptionsActivity_savescreenshot(JNIEnv* env, jobject this);

// Java-accessible functions for GameInfoActivity.java
JNIEXPORT jbyteArray JNICALL Java_com_amigan_droidarcadia_GameInfoActivity_getautotext(  JNIEnv* env, jobject this, jint whichfield);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_GameInfoActivity_getbox(       JNIEnv* env, jobject this);
JNIEXPORT jbyteArray JNICALL Java_com_amigan_droidarcadia_GameInfoActivity_getgglink(    JNIEnv* env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_GameInfoActivity_getglyph(     JNIEnv* env, jobject this);

// Java-accessible functions for ArcadiaHiScoresActivity.java
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_ArcadiaHiScoresActivity_gethiscore(    JNIEnv* env, jobject this, jint whichscore);
JNIEXPORT jstring    JNICALL Java_com_amigan_droidarcadia_ArcadiaHiScoresActivity_gethiscoredate(JNIEnv* env, jobject this, jint whichscore);
JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_ArcadiaHiScoresActivity_sethiscore(    JNIEnv* env, jobject this, jint whichscore, jint thescore, jstring thedate);

// Java-accessible functions for CoinOpHiScoresActivity.java
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_CoinOpHiScoresActivity_gethiscore(    JNIEnv* env, jobject this, jint whichscore);
JNIEXPORT jstring    JNICALL Java_com_amigan_droidarcadia_CoinOpHiScoresActivity_gethiscoredate(JNIEnv* env, jobject this, jint whichscore);
JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_CoinOpHiScoresActivity_sethiscore(    JNIEnv* env, jobject this, jint whichscore, jint thescore, jstring thedate);

// Java-accessible functions for DIPsActivity.java
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_DIPsActivity_cgetdips(             JNIEnv* env, jobject this);
JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_DIPsActivity_csetdips(             JNIEnv* env, jobject this, jint thedips);

// Java-accessible functions for NetworkUtil.java
JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_NetworkUtil_chttpcallback(         JNIEnv* env, jclass this, jint statuscode, jstring content, jlong userdata, jstring errormessage);

// Java-accessible functions for RAActivity.java
JNIEXPORT jstring    JNICALL Java_com_amigan_droidarcadia_RAActivity_getgamegfxurl(          JNIEnv *env, jobject this);
JNIEXPORT jint       JNICALL Java_com_amigan_droidarcadia_RAActivity_getgameid(              JNIEnv *env, jobject this);
JNIEXPORT jboolean   JNICALL Java_com_amigan_droidarcadia_RAActivity_jsetravars(             JNIEnv* env, jobject this, jstring jusername, jstring jpassword, jboolean jcheevos, jboolean jhardcore);
JNIEXPORT void       JNICALL Java_com_amigan_droidarcadia_RAActivity_showachievements(       JNIEnv* env, jobject this);
