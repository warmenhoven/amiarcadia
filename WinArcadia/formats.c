// INCLUDES---------------------------------------------------------------

#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
    #include "resource.h"
    #include <commctrl.h>
    #include <io.h>
    #include <fcntl.h>
    #include <sys/stat.h>

    typedef unsigned char bool;
    #include "RA_Interface.h"
#endif

#include <stdio.h>
#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <string.h>

#ifdef AMIGA
    #include <exec/lists.h>
    #include <intuition/intuition.h>
    #include <classes/arexx.h>
#endif

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"
#ifdef AMIGA
    #include "amiga.h"
    #include <proto/locale.h>        // GetCatalogStr()
#endif

// DEFINES----------------------------------------------------------------

#define LOOPABLE
// whether to generate animations that are designed to be played back in a
// loopable manner

#define DOUBLEWIDTH
// whether to ever widen screenshots

// #define UNCOMPRESSEDILBMS
// whether ILBMs should be uncompressed (otherwise compressed)
// (printer ILBMs are always uncompressed)

// #define KEEPTEMPFILES
// enable this if you want to keep the single-channel temporary WAV files
// (otherwise they get deleted after generating the combined WAV/8SVX/AIFF(s))

#define TRANSPOSE           1.0 // 1.0=100%. Transpose tones up/down.

#define SCREENPTR(x,y)  *(screenptr + (x * dupbpl) + y)

#define GIFTABLESIZE    (32 * 3) // for 32 colours
#define PNGTABLESIZE    (32 * 3) // for 32 colours

#define HT_KEY_MASK     0x1FFF   /* 13 bit key mask */
#define HT_GET_KEY(x)   ((x) >> 12)
#define HT_GET_CODE(x)  ((x) & 0x0FFF)
#define HT_PUT_KEY(x)   ((x) << 12)
#define HT_PUT_CODE(x)  ((x) & 0x0FFF)
#define LZ_MAX_CODE     4095     /* Largest 12 bit code */
#define FLUSH_OUTPUT    4096     /* Impossible code = flush */
#define FIRST_CODE      4097     /* Impossible code = first */
#define HT_SIZE         8192     /* 13 bit hash table size */
#define IMAGE_SAVING    0        /* file_state = processing */
#define IMAGE_COMPLETE  1        /* finished writing */

typedef struct
{   int depth,
        clear_code, eof_code,
        running_code, running_bits,
        max_code_plus_one,
        prev_code, current_code,
        stack_ptr,
        shift_state;
    unsigned long shift_data;
    unsigned long pixel_count;
    int           file_state, position, bufsize;
    unsigned char buf[256];
    unsigned long hash_table[HT_SIZE];
} GifEncoder;

#define MAXRUN_SKIP   127

/* Packer modes. */

#define DUMP            0
#define RUN             1

/* Minimum data run size, maximum data run size and maximum cache size. */

#define MINRUN          3
#define MAXRUN_PACKER 128
#define MAXDAT        128

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT FLAG                     midiplaying[GUESTCHANNELS];
EXPORT TEXT                     fn_oldgame[MAX_PATH + 1] = "", // complete pathname
                                fn_project[MAX_PATH + 1] = "", // complete pathname
                                fn_screenshot[MAX_PATH + 1],   // complete pathname
                                ProjectBuffer[PROJECTSIZE];
EXPORT UBYTE                    smusnote[GUESTCHANNELS],
                                smusplaying[GUESTCHANNELS],
                                smusvolume[GUESTCHANNELS],
                                SoundBuffer[GUESTCHANNELS][SOUNDLENGTH * 2];
EXPORT ULONG                    animframe                = (ULONG) -1,
                                mtrk[GUESTCHANNELS],
                                miditime[GUESTCHANNELS],
                                smusduration,
                                smustime[GUESTCHANNELS],
                                strk[GUESTCHANNELS];
EXPORT int                      bgwidth, bgwidthp, bgheight,
                                headerlength,
                                otl, // original tape length
                                serializemode,
                                SoundLength[GUESTCHANNELS], // in bytes (for 8-bit sound) or words (for 16-bit sound)
                                tapeframe,
                                tapekind;
EXPORT size_t                   png_data_size;
EXPORT UBYTE                   *pixelubyte      = NULL,
                               *TapeCopy        = NULL;
EXPORT ULONG*                   pixelulong      = NULL;
EXPORT FILE                    *ANIMHandle      = NULL,
                               *GIFHandle       = NULL,
                               *MIDIHandle      = NULL,
                               *MIDITrackHandle[GUESTCHANNELS],
                               *MNGHandle       = NULL,
                               *PNGHandle       = NULL,
                               *SMUSHandle      = NULL,
                               *SMUSTrackHandle[GUESTCHANNELS],
                               *TapeHandle      = NULL,
                               *WAVHandle[TOTALCHANNELS];
#ifdef WIN32
    EXPORT TEXT                 file_bkgrnd[MAX_PATH + 1],
                                path_bkgrnd[MAX_PATH + 1];
#endif

EXPORT const struct VolumeStruct volume_3to16[8] = {
//16-bit- ----up---- ---down--
{     0, 128 +   0, 128 -   0 }, //  0: +/-   0.0
{  4681, 128 +  18, 128 -  18 }, //  1: +/-  18.14286
{  9362, 128 +  36, 128 -  36 }, //  2: +/-  36.28571
{ 14043, 128 +  54, 128 -  54 }, //  3: +/-  54.42857
{ 18724, 128 +  73, 128 -  73 }, //  4: +/-  72.57143
{ 23405, 128 +  91, 128 -  91 }, //  5: +/-  90.71429
{ 28086, 128 + 109, 128 - 109 }, //  6: +/- 108.8571
{ 32767, 128 + 127, 128 - 127 }, //  7: +/- 127.0
}, volume_4to16[16] = {
{     0, 128 +   0, 128 -   0 }, //  0: +/-   0.0     or     0.0
{  2184, 128 +   8, 128 -   8 }, //  1: +/-   8.46667 or  2184.46667
{  4369, 128 +  17, 128 -  17 }, //  2: +/-  16.93333 or  4368.93333
{  6553, 128 +  25, 128 -  25 }, //  3: +/-  25.4     or  6553.4
{  8738, 128 +  34, 128 -  34 }, //  4: +/-  33.86667 or  8737.86667
{ 10922, 128 +  42, 128 -  42 }, //  5: +/-  42.33333 or 10922.33333
{ 13107, 128 +  51, 128 -  51 }, //  6: +/-  50.8     or 13106.8
{ 15291, 128 +  59, 128 -  59 }, //  7: +/-  59.26667 or 15291.26667
{ 17476, 128 +  68, 128 -  68 }, //  8: +/-  67.73333 or 17475.73333
{ 19660, 128 +  76, 128 -  76 }, //  9: +/-  76.2     or 19660.2
{ 21845, 128 +  85, 128 -  85 }, // 10: +/-  84.66667 or 21844.66667
{ 24029, 128 +  93, 128 -  93 }, // 11: +/-  93.13333 or 24029.13333
{ 26214, 128 + 102, 128 - 102 }, // 12: +/- 101.6     or 26213.6
{ 28398, 128 + 110, 128 - 110 }, // 13: +/- 110.06667 or 28398.06667
{ 30583, 128 + 119, 128 - 119 }, // 14: +/- 118.53333 or 30582.53333
{ 32767, 128 + 127, 128 - 127 }, // 15: +/- 127.0     or 32767.0
};

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT int                      aifffile,
                                apnganims,
                                avianims,
                                bitrate,
                                colourset,
                                crippled,
                                esvxfile,
                                game,
                                generate,
                                gifanims,
                                hostvolume,
                                iffanims,
                                machine,
                                memmap,
                                midifile,
                                mnganims,
                                offset,
                                papertapemode[2],
                                pipbug_vdu,
                                printerwidth_full,
                                printerheight_full,
                                printerrows_full,
                                prtunit,
                                psgfile,
                                recmode,
                                rotate,
                                rotating,
                                samplerate,
                                score[2],
                                smusfile,
                                tapemode,
                                vdu_columns,
                                vdu_rows_total,
                                wavfile,
                                wide,
                                ymfile;
IMPORT double                   samplewhere_f;
IMPORT FLAG                     guestplaying[TOTALCHANNELS],
                                tr_underline[8];
IMPORT TEXT                     autotext[GAMEINFOLINES][80 + 1],
                                file_game[MAX_PATH + 1],
                                fn_game[MAX_PATH + 1], // the entire pathname (path and file)
                                fn_save[MAX_PATH + 1],
                                fn_tape[4][MAX_PATH + 1],
                                letters[8 + 1],
                                path_games[MAX_PATH + 1],
                                path_projects[MAX_PATH + 1],
                                path_screenshots[MAX_PATH + 1],
                                path_tapes[MAX_PATH + 1],
                                tr_digits[8 + 1];
IMPORT UBYTE                    banked,
                                blank,
                                cosversion,
                                digitleds[DIGITLEDS],
                                memory[32768],
                                oldtapebyte,
                                PapertapeBuffer[2][PAPERTAPEMAX],
                                SilenceBuffer8[MAXSAMPLERATE],
                                tapebyte,
                                vdu[MAX_VDUCOLUMNS][MAX_VDUROWS];
IMPORT ULONG                    cycles_2650,
                                cycles_prev,
                                frames,
                                oldtapecycle,
                                papertapelength[2],
                                papertapewhere[2],
                                region,
                                tapelength,
                                tape_hz,
                                samplewhere;
IMPORT ASCREEN                  screen[BOXWIDTH][BOXHEIGHT];
IMPORT UBYTE*                   IOBuffer;
IMPORT FILE                    *DisHandle,
                               *MacroHandle;
IMPORT struct HostMachineStruct hostmachines[MACHINES];
IMPORT struct KindStruct        filekind[KINDS];
IMPORT struct MachineStruct     machines[MACHINES];
IMPORT struct MemMapInfoStruct  memmapinfo[MEMMAPS];
IMPORT struct PrinterStruct     printer[2];
IMPORT struct SampleStruct      samp[SAMPLES];
IMPORT const TEXT               arcadia_chars[64 + 1],
                                cd2650_chars[128 + 1],
                                led_chars[128 + 1];
#ifdef AMIGA
    IMPORT struct Catalog*      CatalogPtr;
#endif
#ifdef WIN32
    IMPORT int                  CatalogPtr, // APTR doesn't work
                                cheevos;
    IMPORT TEXT                 fn_bkgrnd[MAX_PATH + 1];
    IMPORT UBYTE                SilenceBuffer16[MAXSAMPLERATE * 2];
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE FLAG                     freshanim,
                                widening;
MODULE ASCREEN                  dupscreen[COINOP_BOXWIDTH][COINOP_BOXHEIGHT];
MODULE ASCREEN*                 screenptr;
MODULE TEXT                     asmstring[128 + 1],
                                fn_oldscreenshot[MAX_PATH + 1];
MODULE BYTE                     Buffer[MAXDAT + 1];
MODULE UBYTE                    oldpsg[2][GUESTCHANNELS],
                                planarscreen[5][5][BOXHEIGHT * (((BOXWIDTH * 2) / 8) + 1)],
                                planarrow[((BOXWIDTH * 2) / 8) + 1],
                                PSGBuffer[29],
                                upvol_8, dnvol_8,
                                wideline[BOXWIDTH * 2],
                                YMBuffer[16];
MODULE SWORD                    linebytes    = 40,
                                op_count,
                                uniq_count;
MODULE SLONG                    datastart;
MODULE ULONG                    mngframes,
                                pngframes,
                                startframe,
                                wavcycle[TOTALCHANNELS],
                                wavoffset[TOTALCHANNELS];
MODULE FILE                    *PapertapeHandle[2] = { NULL, NULL },
                               *PSGHandle[2]       = { NULL, NULL },
                               *YMHandle[2]        = { NULL, NULL };
MODULE unsigned char*           uniq;
MODULE int                      dupbpl,
                                oldscreenflip,
                                PackedBytes,
                                picwidth,
                                screenflip,
                                ssbpl;
#ifdef WIN32
    MODULE UBYTE                upvol_h,  dnvol_h,
                                upvol_l,  dnvol_l;
    MODULE SWORD                upvol_16, dnvol_16;
#endif

// MODULE ARRAYS----------------------------------------------------------

MODULE UBYTE mng_header[70] = {
0x8A, 'M', 'N', 'G',0x0D,0x0A,0x1A,0x0A, //  0.. 7 MNG header
  0 ,  0 ,  0 ,  28,                     //  8..11 MHDR length
 'M', 'H', 'D', 'R',                     // 12..15 MHDR name
  0 ,  0 ,  0 ,  0 ,                     // 16..19  MHDR: width
  0 ,  0 ,  0 ,  0 ,                     // 20..23  MHDR: height
  0 ,  0 ,  0 ,  0 ,                     // 24..27  MHDR: frames per second (done later)
  0 ,  0 ,  0 ,  0 ,                     // 28..31  MHDR: layer count (done at end)
  0 ,  0 ,  0 ,  0 ,                     // 32..35  MHDR: frame count (done at end)
  0 ,  0 ,  0 ,  0 ,                     // 36..39  MHDR: play time, in ticks (done at end)
  0 ,  0 ,  0 ,  65,                     // 40..43  MHDR: simplicity profile
  0 ,  0 ,  0 ,  0 ,                     // 44..47 MHDR CRC (done at end)
  0 ,  0 ,  0 ,  10,                     // 48..51 TERM length
 'T', 'E', 'R', 'M',                     // 52..55 TERM name
  3,                                     // 56      TERM: termination action
  2,                                     // 57      TERM: action after iterations
  0 ,  0 ,  0 ,  1 ,                     // 58..61  TERM: delay
0x7F,0xFF,0xFF,0xFF,                     // 62..65  TERM: iterations
  0 ,  0 ,  0 ,  0 ,                     // 66..69 TERM CRC
}, wav_header[44] = {
 'R',  'I',  'F',  'F', // ChunkID         0.. 3 "RIFF"
0x00, 0x00, 0x00, 0x00, // ChunkSize       4.. 7   l8r
 'W',  'A',  'V',  'E', // Format          8..11 "WAVE"

 'f',  'm',  't',  ' ', // Subchunk1ID    12..15 "fmt "
0x10, 0x00, 0x00, 0x00, // Subchunk1Size  16..19    16
0x01, 0x00,             // AudioFormat    20..21     1 PCM
            0x01, 0x00, // NumChannels    22..23     1 mono
0x11, 0x2B, 0x00, 0x00, // SampleRate     24..27 11025 (changed later)
0x11, 0x2B, 0x00, 0x00, // ByteRate       28..31 11025 (changed later)
0x01, 0x00,             // BlockAlign     32..33     1 bytes per sample (changed later)
            0x08, 0x00, // BitsPerSample  34..35     8 (changed later)

 'd',  'a',  't',  'a', // Subchunk2ID    36..39 "data"
0x00, 0x00, 0x00, 0x00  // Subchunk2Size  40..43   l8r
}, esvx_header[48] = {
 'F',  'O',  'R',  'M', // FORM ID            0.. 3 "FORM"
0x00, 0x00, 0x00, 0x00, // FORM Size          4.. 7   l8r
 '8',  'S',  'V',  'X', // Format             8..11 "8SVX" (can be changed to 16SV in some circumstances)
 'V',  'H',  'D',  'R', // ChunkID           12..15 "VHDR"
0x00, 0x00, 0x00, 0x14, // ChunkSize         16..19    20
0x00, 0x00, 0x00, 0x00, // oneShotHiSamples  20..23   l8r
0x00, 0x00, 0x00, 0x00, // repeatHiSamples   24..27     0
0x00, 0x00, 0x00, 0x00, // samplesPerHiCycle 28..31     0
            0x2B, 0x11, // samplesPerSec     32..33 11025 (done later)
                  0x01, // ctOctave          34         1
                  0x00, // sCompression      35         0
0x00, 0x01, 0x00, 0x00, // volume            36..39   max

 'B',  'O',  'D',  'Y', // ChunkID           40..43 "BODY"
0x00, 0x00, 0x00, 0x00, // ChunkSize         44..47   l8r
}, aiff_header[54] =
{   'F', 'O', 'R', 'M',                                //  0 "FORM"
    0x00,0x00,0x00,0x00,                               //  4 size (done later)
    'A', 'I', 'F', 'F',                                //  8 "AIFF"
    'C', 'O', 'M', 'M',                                // 12 "COMM"
    0x00,0x00,0x00,0x12,                               // 16 size
    0x00,0x01,                                         // 20 mono-channel
    0x00,0x00,0x00,0x00,                               // 22 # of samples (done later)
    0x00,0x08,                                         // 26 bitrate (done later)
    0x40,0x0C,0xAC,0x44,0x00,0x00,0x00,0x00,0x00,0x00, // 28 # of samples per second (done later)
    'S', 'S', 'N', 'D',                                // 38 "SSND"
    0x00,0x00,0x00,0x00,                               // 42 size (done later)
    0x00,0x00,0x00,0x00,                               // 46 offset
    0x00,0x00,0x00,0x00,                               // 50 block size
};

MODULE const STRPTR scrnasmhdr[2] = {
/* ARCADIA
   $1800..$18CF UPPERSCRN 208 bytes FIRST
   $18F0..$18F7 POSITIONS   8 bytes SECOND
   $18FC..$18FE MISC        3 bytes THIRD
   $1980..$19BF IMAGERY    64 bytes FOURTH
   $19F8..$19FB WRITEONLY   4 bytes FIFTH
   $1A00..$1ACF LOWERSCRN 208 bytes FIRST */
"        ORG     0\n\n"\
"        BCTA,un BOOT\n\n"\
"        RETE,un\n\n"\
"BOOT:\n"\
"        LODI,r1 208\n"\
"FIRST:\n"\
"        LODA,r0 UPPERSCRN-1,r1\n"\
"        STRA,r0 $1800-1,r1\n"\
"        LODA,r0 LOWERSCRN-1,r1\n"\
"        STRA,r0 $1A00-1,r1\n"\
"        BDRR,r1 FIRST\n\n"\
"        LODI,r1 8\n"\
"SECOND:\n"\
"        LODA,r0 POSITIONS-1,r1\n"\
"        STRA,r0 $18F0-1,r1\n"\
"        BDRR,r1 SECOND\n\n"\
"        LODI,r1 3\n"\
"THIRD:\n"\
"        LODA,r0 MISC-1,r1\n"\
"        STRA,r0 $18FC-1,r1\n"\
"        BDRR,r1 THIRD\n\n"\
"        LODI,r1 64\n"\
"FOURTH:\n"\
"        LODA,r0 IMAGERY-1,r1\n"\
"        STRA,r0 $1980-1,r1\n"\
"        BDRR,r1 FOURTH\n\n"\
"        LODI,r1 4\n"\
"FIFTH:\n"\
"        LODA,r0 WRITEONLY-1,r1\n"\
"        STRA,r0 $19F8-1,r1\n"\
"        BDRR,r1 FIFTH\n\n"\
"        HALT\n\n",
/* INTERTON
   $1E80        NOISEDATA   1 byte  n/a
   $1F00..$1F0D SPR0DATA   14 bytes FIRST
   $1F10..$1F1D SPR1DATA   14 bytes FIRST
   $1F20..$1F2D SPR2DATA   14 bytes FIRST
   $1F40..$1F4D SPR3DATA   14 bytes FIRST
   $1F80..$1FAC GRID       45 bytes SECOND
   $1FC0..$1FC3 MISC1       4 bytes THIRD
   $1FC6..$1FC9 MISC2       4 bytes THIRD */
"        BCTA,un BOOT\n\n"\
"        RETE,un\n\n"\
"BOOT:\n"\
"        LODA,r0 NOISEDATA\n" \
"        STRA,r0 $1E80\n"\
"        LODI,r1 14\n"\
"FIRST:\n"\
"        LODA,r0 SPR0DATA-1,r1\n"\
"        STRA,r0 $1F00-1,r1\n"\
"        LODA,r0 SPR1DATA-1,r1\n"\
"        STRA,r0 $1F10-1,r1\n"\
"        LODA,r0 SPR2DATA-1,r1\n"\
"        STRA,r0 $1F20-1,r1\n"\
"        LODA,r0 SPR3DATA-1,r1\n"\
"        STRA,r0 $1F40-1,r1\n"\
"        BDRR,r1 FIRST\n\n"\
"        LODI,r1 45\n"\
"SECOND:\n"\
"        LODA,r0 GRID-1,r1\n"\
"        STRA,r0 $1F80-1,r1\n"\
"        BDRR,r1 SECOND\n\n"\
"        LODI,r1 4\n"\
"THIRD:\n"\
"        LODA,r0 MISC1-1,r1\n"\
"        STRA,r0 $1FC0-1,r1\n"\
"        LODA,r0 MISC2-1,r1\n"\
"        STRA,r0 $1FC6-1,r1\n"\
"        BDRR,r1 THIRD\n\n"\
"        HALT\n\n"
};

/* MODULE STRUCTURES------------------------------------------------------

(None)

MODULE FUNCTIONS------------------------------------------------------- */

MODULE void write_gif_line(FILE* file, GifEncoder* encoder, int width);
MODULE int lookup_gif_hash(unsigned long* hash_table, unsigned long key);
MODULE void write_gif_code(FILE* file, GifEncoder* encoder, int code);
MODULE int gif_hash_key(unsigned long key);
MODULE void add_gif_hash_entry(unsigned long* hash_table, unsigned long key, int code);
MODULE void write_gif_byte(FILE* file, GifEncoder* encoder, int ch);
MODULE void init_gif_encoder(FILE* file, GifEncoder* encoder, int depth);
MODULE void flush_gif_encoder(FILE* file, GifEncoder* encoder);
MODULE void write_ihdr_and_plte(FILE* filehandle);
MODULE LONG PackRow(BYTE* Source, BYTE* Destination, LONG RowSize);
MODULE BYTE* PutDump(BYTE* Destination, LONG Count);
MODULE BYTE* PutRun(BYTE* Destination, LONG Count, BYTE Char);
MODULE SWORD skip_count_plane(UBYTE *in, UBYTE *last_in, SWORD next_line, SWORD rows);
MODULE SWORD skip_count_line(UBYTE* in, UBYTE* last_in, SWORD count);
MODULE UBYTE* skip_comp_plane(UBYTE* in, UBYTE* last_in, UBYTE* out, SWORD next_line, SWORD rows);
MODULE UBYTE* skip_comp_line(UBYTE* in, UBYTE* last_in, UBYTE* out, SWORD count);
MODULE SWORD vskip(UBYTE* in, UBYTE* last_in, SWORD count);
MODULE SWORD vsame(UBYTE* in, SWORD count);
MODULE unsigned char* flush_uniq(unsigned char* stuff);
MODULE int copy_line_to_chars(unsigned char* in, unsigned char* out, int linebytes, int count);
MODULE void combine(int kind, ULONG smallest);
MODULE void write_dlta(int newerscreen, int olderscreen);
MODULE LONG extended2long(UBYTE* ex);
MODULE void write_samples(int guestchan, int samples);
MODULE void write_silence(int guestchan, int samples);
MODULE void emit_dbs(STRPTR labelstring, int howmany, int address);
MODULE void writeulong(ULONG value, UBYTE* ptr);
MODULE void set_volume(int guestvolume);

// CODE-------------------------------------------------------------------

EXPORT void savescreenshot(int kind, FLAG wantasl, FLAG clip, FLAG animating)
{   TRANSIENT ULONG      amount,
                         crc;
    TRANSIENT UBYTE*     TheBody;
    TRANSIENT FILE*      TheLocalHandle = NULL; // initialized to avoid spurious SAS/C compiler warnings
// LocalHandle is a variable of winbase.h
    TRANSIENT int        bodysize,
                         count,
                         iffsize,
                         i, j,
                         length,
                         x, y;
    TRANSIENT UBYTE      last,
                         t;
    TRANSIENT TEXT       annochunk[8],
                         commentstring[50 + 1];
    PERSIST   GifEncoder encoder; // PERSISTent so as not to blow the stack
    PERSIST   BYTE       PackBuffer[(((BOXWIDTH * 2) / 8) + 1) * 2 * BOXHEIGHT * 5];
    FAST      int        totalbytes,
                         whichplane;

PERSIST UBYTE anim_header[12] =
{    'F',  'O',  'R',  'M', 0xFF, 0xFF, 0xFF, 0xFF,   // "FORMxxxx"
     'A',  'N',  'I',  'M'                            // "ANIM"
}, iff_header[48 + 96] =
{    'F',  'O',  'R',  'M', 0xFF, 0xFF, 0xFF, 0xFF,   // "FORMxxxx"
     'I',  'L',  'B',  'M',  'B',  'M',  'H',  'D',   // "ILBMBMHD"
    0x00, 0x00, 0x00, 0x14, 0xFF, 0xFF, 0xFF, 0xFF,   // "xxxx1234"
    0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,   // "56789012"
    0x00, 0x00, 0x01, 0x01, 0xFF, 0xFF, 0xFF, 0xFF,   // "34567890"
     'C',  'M',  'A',  'P', 0x00, 0x00, 0x00, 0x60,   // "CMAPxxxx"
    // R     G     B
    0x00, 0x00, 0x00, // colour  0
    0x00, 0x00, 0x00, // colour  1
    0x00, 0x00, 0x00, // colour  2
    0x00, 0x00, 0x00, // colour  3
    0x00, 0x00, 0x00, // colour  4
    0x00, 0x00, 0x00, // colour  5
    0x00, 0x00, 0x00, // colour  6
    0x00, 0x00, 0x00, // colour  7
    0x00, 0x00, 0x00, // colour  8
    0x00, 0x00, 0x00, // colour  9
    0x00, 0x00, 0x00, // colour 10
    0x00, 0x00, 0x00, // colour 11
    0x00, 0x00, 0x00, // colour 12
    0x00, 0x00, 0x00, // colour 13
    0x00, 0x00, 0x00, // colour 14
    0x00, 0x00, 0x00, // colour 15
    0x00, 0x00, 0x00, // colour 16
    0x00, 0x00, 0x00, // colour 17
    0x00, 0x00, 0x00, // colour 18
    0x00, 0x00, 0x00, // colour 19
    0x00, 0x00, 0x00, // colour 20
    0x00, 0x00, 0x00, // colour 21
    0x00, 0x00, 0x00, // colour 22
    0x00, 0x00, 0x00, // colour 23
    0x00, 0x00, 0x00, // colour 24
    0x00, 0x00, 0x00, // colour 25
    0x00, 0x00, 0x00, // colour 26
    0x00, 0x00, 0x00, // colour 27
    0x00, 0x00, 0x00, // colour 28
    0x00, 0x00, 0x00, // colour 29
    0x00, 0x00, 0x00, // colour 30
    0x00, 0x00, 0x00, // colour 31
}, body_header[8] =
{    'B',  'O',  'D',  'Y', 0x00, 0xFF, 0xFF, 0xFF, // "BODYxxxx"
}, sctl_header[24] =
{    'S',  'C',  'T',  'L', // chunk ID
    0x00, 0x00, 0x00, 0x10, // chunk size
                      0x01, // command
                      0x40, // volume
                0x00, 0x01, // sound number
                0x00, 0x01, // repeats
                0x00, 0x01, // channelMask
                0x2B, 0x11, // frequency
                0x00, 0x00, // flags
    0x00, 0x00, 0x00, 0x00, // pad
}, bmp_header[150] =
{   // bytes  0..  1 are "BM"
    // bytes  2..  5 is  the size of the entire file (little-endian)
    // bytes  6..  9 are reserved
    // bytes 10.. 13 is  the offset to the bits (little-endian)
    // byte  14.. 17 is  a constant (structure size) (little-endian)
    // bytes 18.. 21 is  the width of the bitmap (little-endian)
    // bytes 22.. 25 is  the height of the bitmap (little-endian)
    // bytes 26.. 27 is  a constant
    // bytes 28.. 29 is  the bits per pixel
    // bytes 30.. 33 is  the compression
    // bytes 34.. 37 is  the size (zero is allowed)
    // bytes 38.. 41 is  X-pixels per metre
    // bytes 42.. 45 is  Y-pixels per metre
    // bytes 46.. 49 is  the number of colours used (ie. 24)
    // bytes 50.. 53 is  the number of important colours (ie. 24)
    // bytes 54..149 are colours 0..23 (4 bytes each)
    0x42, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // "BMzzzz00"
    0x00, 0x00, 0x96, 0x00, 0x00, 0x00, 0x28, 0x00, // "00yyyy*0"
    0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, // "00wwwwhh"
    0x00, 0x00, 0x01, 0x00, 0x08, 0x00, 0x00, 0x00, // "hh1080cc"
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x0B, // "ccssssxx"
    0x00, 0x00, 0x13, 0x0B, 0x00, 0x00, 0x18, 0x00, // "xxyyyycc"
    0x00, 0x00, 0x18, 0x00, 0x00, 0x00,             // "cciiii"
    // B     G     R
    0x00, 0x00, 0x00, 0x00, // colour  0
    0x00, 0x00, 0x00, 0x00, // colour  1
    0x00, 0x00, 0x00, 0x00, // colour  2
    0x00, 0x00, 0x00, 0x00, // colour  3
    0x00, 0x00, 0x00, 0x00, // colour  4
    0x00, 0x00, 0x00, 0x00, // colour  5
    0x00, 0x00, 0x00, 0x00, // colour  6
    0x00, 0x00, 0x00, 0x00, // colour  7
    0x00, 0x00, 0x00, 0x00, // colour  8
    0x00, 0x00, 0x00, 0x00, // colour  9
    0x00, 0x00, 0x00, 0x00, // colour 10
    0x00, 0x00, 0x00, 0x00, // colour 11
    0x00, 0x00, 0x00, 0x00, // colour 12
    0x00, 0x00, 0x00, 0x00, // colour 13
    0x00, 0x00, 0x00, 0x00, // colour 14
    0x00, 0x00, 0x00, 0x00, // colour 15
    0x00, 0x00, 0x00, 0x00, // colour 16
    0x00, 0x00, 0x00, 0x00, // colour 17
    0x00, 0x00, 0x00, 0x00, // colour 18
    0x00, 0x00, 0x00, 0x00, // colour 19
    0x00, 0x00, 0x00, 0x00, // colour 20
    0x00, 0x00, 0x00, 0x00, // colour 21
    0x00, 0x00, 0x00, 0x00, // colour 22
    0x00, 0x00, 0x00, 0x00  // colour 23
}, pcx_header[128] =
{   0x0A,                   //   0      manufacturer
    0x05,                   //   1      version
    0x01,                   //   2      encoding
    0x08,                   //   3      bits per pixel
       0,    0,             //   4..  5 X-min
       0,    0,             //   6..  7 Y-min
     255,  255,             //   8..  9 X-max (done later)
     255,  255,             //  10.. 11 Y-max (done later)
     255,  255,             //  12.. 13 X-res (done later)
     255,  255,             //  14.. 15 Y-res (done later)
    // R     G     B
    0x00, 0x00, 0x00,       //  16.. 18 bytes 16..63 are EGA colour table
    0x00, 0x00, 0x00,       //  19.. 21
    0x00, 0x00, 0x00,       //  22.. 24
    0x00, 0x00, 0x00,       //  25.. 27
    0x00, 0x00, 0x00,       //  28.. 30
    0x00, 0x00, 0x00,       //  31.. 33
    0x00, 0x00, 0x00,       //  34.. 36
    0x00, 0x00, 0x00,       //  37.. 39
    0x00, 0x00, 0x00,       //  40.. 42
    0x00, 0x00, 0x00,       //  43.. 45
    0x00, 0x00, 0x00,       //  46.. 48
    0x00, 0x00, 0x00,       //  49.. 51
    0x00, 0x00, 0x00,       //  52.. 54
    0x00, 0x00, 0x00,       //  55.. 57
    0x00, 0x00, 0x00,       //  58.. 60
    0x00, 0x00, 0x00,       //  61.. 63
       0,                   //  64      reserved
       1,                   //  65      planes
    0xFF, 0xFF,             //  66.. 67 bytes per line per plane (ie. width) (done later)
    0x00, 0x00,             //  68.. 69 palette type
    0x00, 0x00,             //  70.. 71 horizontal screen size
    0x00, 0x00,             //  72.. 73 vertical screen size
    0x00, 0x00, 0x00, 0x00, //  74.. 77 bytes 74..127 are reserved
    0x00, 0x00, 0x00, 0x00, //  78.. 81
    0x00, 0x00, 0x00, 0x00, //  82.. 85
    0x00, 0x00, 0x00, 0x00, //  86.. 89
    0x00, 0x00, 0x00, 0x00, //  90.. 93
    0x00, 0x00, 0x00, 0x00, //  94.. 97
    0x00, 0x00, 0x00, 0x00, //  98..101
    0x00, 0x00, 0x00, 0x00, // 102..105
    0x00, 0x00, 0x00, 0x00, // 106..109
    0x00, 0x00, 0x00, 0x00, // 110..113
    0x00, 0x00, 0x00, 0x00, // 114..117
    0x00, 0x00, 0x00, 0x00, // 118..121
    0x00, 0x00, 0x00, 0x00, // 122..125
    0x00, 0x00              // 126..127
}, gif_header[13] =
{   0x47, 0x49, 0x46, 0x38, 0x00, 0x61, //  0..  5 "GIF8xa"
    0x00, 0x00,                         //  6..  7 width
    0x00, 0x00,                         //  8..  9 height
    0xC4,                               // 10      colour depth, etc. ($C4 means 5 bitplanes)
    0x00,                               // 11      background colour index
    0x00                                // 12      pixel aspect ratio
}, png_header[8] = {
0x89, 'P', 'N', 'G',0x0D,0x0A,0x1A,0x0A  //  0.. 7 PNG header
}, netscape[19] =
{   0x21,       // GIF Extension code
    0xFF,       // Application Extension Label
    0x0B,       // Length of Application Block
    'N','E','T','S','C','A','P','E','2','.','0',
    0x03,       // Length of Data Sub-Block
    0x01,
    0x01, 0x00, // iterations (0 means infinite)
    0x00,       // Data Sub-block Terminator.
}, tiff_header[198] =
// Header...
{   'I', 'I',                           //  0..  1 endianness
    0x2A, 0x00,                         //  2..  3 magic number
    0x08, 0x00, 0x00, 0x00,             //  4..  7 offset of first block
// Block...
    0x0E, 0x00,                         //  8..  9 number of fields in this block (14)
    // 1st field
                  0xFE, 0x00,           // 10.. 11   NewSubfileType ID (254)
                  0x04, 0x00,           // 12.. 13   ULONG ID (4)
      0x01, 0x00, 0x00, 0x00,           // 14.. 17   number of values
      0x00, 0x00, 0x00, 0x00,           // 18.. 21   actual value
    // 2nd field
                  0x00, 0x01,           // 22.. 23   ImageWidth ID (256)
                  0x03, 0x00,           // 24.. 25   UWORD ID (3)
      0x01, 0x00, 0x00, 0x00,           // 26.. 29   number of values
      0x00, 0x00, 0x00, 0x00,           // 30.. 33   actual value
    // 3rd field
                  0x01, 0x01,           // 34.. 35   ImageLength ID (257)
                  0x03, 0x00,           // 36.. 37   UWORD ID (3)
      0x01, 0x00, 0x00, 0x00,           // 38.. 41   number of values
      0x00, 0x00, 0x00, 0x00,           // 42.. 45   actual value
    // 4th field
                  0x02, 0x01,           // 46.. 47   BitsPerSample ID (258)
                  0x03, 0x00,           // 48.. 49   UWORD ID (3)
      0x01, 0x00, 0x00, 0x00,           // 50.. 53   number of values
      0x04, 0x00, 0x00, 0x00,           // 54.. 57   actual value (for 16 colours currently)
    // 5th field
                  0x03, 0x01,           // 58.. 59   Compression ID (259)
                  0x03, 0x00,           // 60.. 61   UWORD ID (3)
      0x01, 0x00, 0x00, 0x00,           // 62.. 65   number of values
      0x01, 0x00, 0x00, 0x00,           // 66.. 69   actual value (means uncompressed)
    // 6th field
                  0x06, 0x01,           // 70.. 71   PhotometricInterpretation ID (262)
                  0x03, 0x00,           // 72.. 73   UWORD ID (3)
      0x01, 0x00, 0x00, 0x00,           // 74.. 77   number of values
      0x03, 0x00, 0x00, 0x00,           // 78.. 81   actual value (means palette colour)
    // 7th field
                  0x11, 0x01,           // 82.. 83   StripOffsets ID (273)
                  0x04, 0x00,           // 84.. 85   ULONG ID (3)
      0x01, 0x00, 0x00, 0x00,           // 86.. 89   number of values
        38,    1,    0,    0,           // 90.. 93   location of data (38+(1*256)=294)
    // 8th field
                  0x15, 0x01,           // 94.. 95   SamplesPerPixel ID (277)
                  0x03, 0x00,           // 96.. 97   UWORD ID (3)
      0x01, 0x00, 0x00, 0x00,           // 98..101   number of values
      0x01, 0x00, 0x00, 0x00,          // 102..105   actual value
    // 9th field
                  0x16, 0x01,          // 106..107   RowsPerStrip ID (278)
                  0x04, 0x00,          // 108..109   ULONG ID (4)
      0x01, 0x00, 0x00, 0x00,          // 110..113   number of values
      0x00, 0x00, 0x00, 0x00,          // 114..117   actual value (done later)
    // 10th field
                  0x17, 0x01,          // 118..119   StripByteCounts ID (279)
                  0x04, 0x00,          // 120..121   ULONG ID (4)
      0x01, 0x00, 0x00, 0x00,          // 122..125   number of values
      0x00, 0x00, 0x00, 0x00,          // 126..129   actual value (done later)
    // 11th field
                  0x1A, 0x01,          // 130..131   XResolution ID (282)
                  0x05, 0x00,          // 132..133   RATIONAL ID (5)
      0x01, 0x00, 0x00, 0x00,          // 134..137   number of values
       182,    0,    0,    0,          // 138..141   position of actual value
    // 12th field
                  0x1B, 0x01,          // 142..143   YResolution ID (283)
                  0x05, 0x00,          // 144..145   RATIONAL ID (5)
      0x01, 0x00, 0x00, 0x00,          // 146..149   number of values
       190,    0,    0,    0,          // 150..153   position of actual value
    // 13th field
                  0x28, 0x01,          // 154..155   ResolutionUnit ID (296)
                  0x03, 0x00,          // 156..157   UWORD ID (3)
      0x01, 0x00, 0x00, 0x00,          // 158..161   number of values
      0x01, 0x00, 0x00, 0x00,          // 162..165   actual value (means no absolute unit)
    // 14th field
                  0x40, 0x01,          // 166..167   ColorMap ID (320)
                  0x03, 0x00,          // 168..169   UWORD ID (3)
      0x30, 0x00, 0x00, 0x00,          // 170..173   number of values
       198,    0,    0,    0,          // 174..177   position of first actual value
// end of block
    0x00, 0x00, 0x00, 0x00,                         // 178..181 end of block
// data for XResolution
    0x48, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // 182..189
// data for YResolution
    0x48, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // 190..197
};

    // assert(GUESTCOLOURS == 24);

    if (wantasl)
    {   if (game)
        {   strcpy((char*) fn_screenshot, (const char*) file_game);
        } else
        {   fn_screenshot[0] = EOS;
        }
        fixextension(filekind[kind].extension, (STRPTR) fn_screenshot, TRUE, ""); // so we have extension in ASL requester
        if (asl
        (   filekind[kind].hail,
            filekind[kind].pattern,
            TRUE,
            (STRPTR) path_screenshots,
            (STRPTR) fn_screenshot, // this fortunately doesn't contain the path right now
            (STRPTR) fn_screenshot
        ) != EXIT_SUCCESS)
        {   return;
        }
        fixextension(filekind[kind].extension, (STRPTR) fn_screenshot, TRUE, ""); // so we have extension even if user overtypes it in ASL requester
#ifdef SETPATHS
        break_pathname((STRPTR) fn_screenshot, (STRPTR) path_screenshots, NULL);
#endif
    }

    /* Possible enhancements:
     (a) could use compression (for BMP and TIFF, because we already
         support this for GIF, ILBM and PCX, and ACBM doesn't support compression).
     (b) more detailed/specific error messages.
     (c) check ALL possible errors (eg. calloc(), Close(), etc.).
     (d) perhaps omit left/right/bottom "overscan" margins (eg. from
         Arcadia screenshots).
     (e) Most files could be saved using less than 5 planes.

    ACBM files *do* have pad bits at the end of each row (based on Personal
    Paint 6.4 behaviour). */

    if (kind != KIND_MNG && kind != KIND_PNG && (memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) && rotate)
    {   // assert(COINOP_BOXWIDTH == COINOP_BOXHEIGHT);
        for (x = 0; x < COINOP_BOXWIDTH; x++)
        {   for (y = 0; y < COINOP_BOXHEIGHT; y++)
            {   dupscreen[x][y] = screen[y][COINOP_BOXWIDTH - 1 - x];
        }   }
        screenptr = &dupscreen[0][0];
        dupbpl = COINOP_BOXHEIGHT;
    } else
    {   screenptr = &screen[0][0];
        dupbpl = BOXHEIGHT;
    }

    if (clip)
    {   clip_open(TRUE);
    } else
    {   if (!(TheLocalHandle = fopen((const char*) fn_screenshot, "wb")))
        {   beep();
            return;
    }   }

    picwidth = machines[machine].width;
    widening = FALSE;
#ifdef DOUBLEWIDTH
    if (wide == 2)
    {   picwidth *= 2;
        widening = TRUE;
    }
#endif

    switch (kind)
    {
    case KIND_PCX:
        // header
        pcx_header[ 8] = (UBYTE) ((picwidth                 - 1) % 256);
        pcx_header[ 9] = (UBYTE) ((picwidth                 - 1) / 256);
        pcx_header[10] = (UBYTE) ((machines[machine].height - 1) % 256);
        pcx_header[11] = (UBYTE) ((machines[machine].height - 1) / 256);
        pcx_header[12] =
        pcx_header[66] = (UBYTE) ( picwidth                      % 256);
        pcx_header[13] =
        pcx_header[67] = (UBYTE) ( picwidth                      / 256);
        pcx_header[14] = (UBYTE) ( machines[machine].height      % 256);
        pcx_header[15] = (UBYTE) ( machines[machine].height      / 256);
        for (i = 0; i < 16; i++)
        {   pcx_header[16 + (i * 3)] = getred(  colourset, i);
            pcx_header[17 + (i * 3)] = getgreen(colourset, i);
            pcx_header[18 + (i * 3)] = getblue( colourset, i);
        }
        DISCARD fwrite(pcx_header, (size_t) 128, 1, TheLocalHandle);

        // body
        TheBody = calloc((unsigned int) (picwidth * machines[machine].height), 1); // should check return code!
        i = 0;
        for (y = 0; y < machines[machine].height; y++)
        {   count = 1;
            last = (UBYTE) SCREENPTR(0, y);

            /* Encode one line */
            for (x = 1; x < picwidth; x++)
            {   if (widening)
                {   t = (UBYTE) SCREENPTR(x / 2, y);
                } else
                {   t = (UBYTE) SCREENPTR(x    , y);
                }

                if (t == last)
                {   count++;
                    if (count == 63)
                    {   TheBody[i++] = (UBYTE) (count | 0xC0);
                        TheBody[i++] = last;
                        count = 0;
                }   }
                else
                {   if (count)
                    {   if ((count == 1) && ((last & 0xC0) != 0xC0))
                        {   TheBody[i++] = last;
                        } else
                        {   TheBody[i++] = (UBYTE) (count | 0xC0);
                            TheBody[i++] = last;
                    }   }
                    last = t;
                    count = 1;
            }   }

            if (count)
            {   if ((count == 1) && ((last & 0xC0) != 0xC0))
                {   TheBody[i++] = last;
                } else
                {   TheBody[i++] = (UBYTE) (count | 0xC0);
                    TheBody[i++] = last;
        }   }   }
        DISCARD fwrite(TheBody, (size_t) i, 1, TheLocalHandle);
        free(TheBody);
        // TheBody = NULL;

        // palette
        TheBody    = calloc(769, 1);
        TheBody[0] = 0x0C;
        for (i = 0; i < 24; i++)
        {   TheBody[1 + (i * 3)] = getred(colourset, i);
            TheBody[2 + (i * 3)] = getgreen(colourset, i);
            TheBody[3 + (i * 3)] = getblue(colourset, i);
        }
        DISCARD fwrite(TheBody, (size_t) 769, 1, TheLocalHandle);
        free(TheBody);
        // TheBody = NULL;
    acase KIND_GIF:
        sprintf((char*) commentstring, "Generated by %s", hostmachines[machine].titlebartext);
        length = strlen((const char*) commentstring);

        // header
        gif_header[  4] = animating ? '9' : '7';
        gif_header[  6] = picwidth  % 256;
        gif_header[  7] = picwidth  / 256;
        gif_header[  8] = machines[machine].height % 256;
        gif_header[  9] = machines[machine].height / 256;

        bodysize =
            13                         // gif_header[]
          + GIFTABLESIZE               // palette table
          + length                     // comment length
          + 4                          // for comment
          + (animating ? (19 + 8) : 0) // 27 extra bytes if it's an animated GIF (netscape + animation blocks)
          + 10;                        // image descriptor block
        TheBody = calloc((unsigned int) bodysize, 1); // should check return code

        for (i = 0; i < 13; i++)
        {   TheBody[i] = gif_header[i];
        }

        // palette
        for (i = 0; i < 24; i++)
        {   TheBody[13 + (i * 3)] = getred(colourset, i);
            TheBody[14 + (i * 3)] = getgreen(colourset, i);
            TheBody[15 + (i * 3)] = getblue(colourset, i);
        }

        i = 13 + GIFTABLESIZE;

        if (animating)
        {
#ifdef LOOPABLE
            netscape[16] = 0;
#else
            netscape[16] = 1;
#endif
            for (j = 0; j < 19; j++)
            {   TheBody[i++] = netscape[j];
        }   }

        TheBody[i++] = 0x21; // GIF Extension code
        TheBody[i++] = 0xFE; // Comment Label
        TheBody[i++] = length; // Length of Comment
         for (j = 0; j < length; j++)
         {   TheBody[i++] = commentstring[j];
         }
        TheBody[i++] = 0x00; // Data Sub-block Terminator.

        if (animating)
        {   // animation block
            TheBody[i++] = 0x21; // GIF Extension code
            TheBody[i++] = 0xF9; // Graphic Control Label
            TheBody[i++] = 0x04; // Length of Application Block
             TheBody[i++] = 0x00; // flags
             TheBody[i++] = 0x02; // delay (2/100ths of a sec)
             TheBody[i++] = 0x00;
             TheBody[i++] = 0x00; // transparent colour index
            TheBody[i++] = 0x00; // Data Sub-block Terminator.
        }

        // image descriptor block
        TheBody[i++] = 0x2C; // block code
        TheBody[i++] = 0;
        TheBody[i++] = 0; // left
        TheBody[i++] = 0;
        TheBody[i++] = 0; // top
        TheBody[i++] = gif_header[6];
        TheBody[i++] = gif_header[7];
        TheBody[i++] = gif_header[8];
        TheBody[i++] = gif_header[9];
        TheBody[i++] = 0;
        // assert(i == bodysize);
        DISCARD fwrite(TheBody, (size_t) i, 1, TheLocalHandle);
        free(TheBody);
        // TheBody = NULL;

        // body
        init_gif_encoder(TheLocalHandle, &encoder, 5); // 5 bitplanes
        for (y = 0; y < machines[machine].height; y++)
        {   i = 0;
            for (x = 0; x < machines[machine].width; x++)
            {   wideline[i++] = SCREENPTR(x, y);
                if (widening)
                {   wideline[i++] = SCREENPTR(x, y);
            }   }
            write_gif_line(TheLocalHandle, &encoder, picwidth);
        }
        flush_gif_encoder(TheLocalHandle, &encoder);

        if (!animating)
        {   putc(0x3B, TheLocalHandle);
        }
    acase KIND_TIFF:
        tiff_header[ 30] = picwidth                 % 256; // ImageWidth
        tiff_header[ 31] = picwidth                 / 256;
        tiff_header[ 42] = machines[machine].height % 256; // ImageLength
        tiff_header[ 43] = machines[machine].height / 256;
        tiff_header[114] = machines[machine].height % 256; // RowsPerStrip
        tiff_header[115] = machines[machine].height / 256;
        amount = picwidth * machines[machine].height / 2;
        ulong_to_le(&tiff_header[126], amount); // StripByteCounts
        DISCARD fwrite(tiff_header, (size_t) 198, 1, TheLocalHandle);

        TheBody = malloc((unsigned int) (16 * 6));
        // palette
        for (i = 0; i < 16; i++)
        {   TheBody[     (i * 2)] = TheBody[     (i * 2) + 1] = getred(colourset, i);
        }
        for (i = 0; i < 16; i++)
        {   TheBody[32 + (i * 2)] = TheBody[32 + (i * 2) + 1] = getgreen(colourset, i);
        }
        for (i = 0; i < 16; i++)
        {   TheBody[64 + (i * 2)] = TheBody[64 + (i * 2) + 1] = getblue(colourset, i);
        }
        DISCARD fwrite(TheBody, (size_t) (16 * 6), 1, TheLocalHandle);
        free(TheBody);
        // TheBody = NULL;

        TheBody = malloc((unsigned int) amount);
        i = 0;
        for (y = 0; y < machines[machine].height; y++)
        {   if (widening)
            {   for (x = 0; x < machines[machine].width; x++)
                {   t = (UBYTE) SCREENPTR(x, y);
                    if (t >= 16) t = 0;
                    TheBody[i] = (t << 4) | t;
                    i++;
            }   }
            else
            {   for (x = 0; x < machines[machine].width; x += 2)
                {   t = (UBYTE) SCREENPTR(x, y);
                    if (t >= 16) t = 0;
                    TheBody[i] = t << 4;
                    t = (UBYTE) SCREENPTR(x + 1, y);
                    if (t >= 16) t = 0;
                    TheBody[i] |= t;
                    i++;
        }   }   }
        DISCARD fwrite(TheBody, (size_t) amount, 1, TheLocalHandle);
        free(TheBody);
        // TheBody = NULL;
    acase KIND_MNG:
        mng_header[18] = (UBYTE) (picwidth    / 256);
        mng_header[19] = (UBYTE) (picwidth    % 256);
        mng_header[22] = (UBYTE) (machines[machine].height / 256);
        mng_header[23] = (UBYTE) (machines[machine].height % 256);
        mng_header[27] = (region == REGION_NTSC) ? 60 : 50;
     // crc = crc32(&mng_header[12], 32); // MHDR CRC
     // writeulong(crc, &mng_header[44]);

#ifdef LOOPABLE
      /*  0  ,  0 ,  0 ,  10,    48..51 TERM length
         'T' , 'E', 'R', 'M',    52..55 TERM name
          3  ,                   56      TERM: termination action
          2  ,                   57      TERM: action after iterations
          0  ,  0 ,  0 ,  1 ,    58..61  TERM: delay
        0x7F,0xFF,0xFF,0xFF,    62..65  TERM: iterations
          0  ,  0 ,  0 ,  0 ,    66..69 TERM CRC */
        mng_header[51] = 10;
        mng_header[56] =  3;
        mng_header[57] =  2; // because 57..60 get overwritten when looping
        mng_header[58] =
        mng_header[59] =
        mng_header[60] =  0;
        crc = crc32(&mng_header[52], 14); // TERM CRC
        writeulong(crc, &mng_header[66]);
        DISCARD fwrite(mng_header, 70, 1, TheLocalHandle);
#else
      /*  0  ,  0 ,  0 ,  1 ,    48..51 TERM length
         'T' , 'E', 'R', 'M',    52..55 TERM name
          0  ,                   56      TERM: termination action
          0  ,  0 ,  0 ,  0 ,    57..60 TERM CRC */
        mng_header[51] =  1;
        mng_header[56] =  0;
        crc = crc32(&mng_header[52],  5); // TERM CRC
        writeulong(crc, &mng_header[57]);
        DISCARD fwrite(mng_header, 61, 1, TheLocalHandle);
#endif

        write_ihdr_and_plte(TheLocalHandle);
        // assert(animating);
        mngframes = 0;
        write_idat_or_fdat(TheLocalHandle, FALSE, KIND_MNG, TRUE);
        write_iend(TheLocalHandle);
    acase KIND_PNG:
        DISCARD fwrite(png_header, 8, 1, TheLocalHandle);

        write_ihdr_and_plte(TheLocalHandle);

        if (animating)
        {   write_actl_start(TheLocalHandle);
            pngframes = 0;
            write_fctl(TheLocalHandle);
            write_idat_or_fdat(TheLocalHandle, FALSE, KIND_PNG, TRUE);
        } else
        {   write_idat_or_fdat(TheLocalHandle, FALSE, KIND_PNG, FALSE);
            write_iend(TheLocalHandle);
        }
    acase KIND_BMP:
        // header
        ulong_to_le(&bmp_header[2], 182 + (picwidth * machines[machine].height));
        bmp_header[18] = (UBYTE) (picwidth  % 256);
        bmp_header[19] = (UBYTE) (picwidth  / 256);
        bmp_header[22] = (UBYTE) (machines[machine].height % 256);
        bmp_header[23] = (UBYTE) (machines[machine].height / 256);
        for (i = 0; i < 24; i++)
        {   bmp_header[54 + (i * 4)] = getblue(colourset, i);
            bmp_header[55 + (i * 4)] = getgreen(colourset, i);
            bmp_header[56 + (i * 4)] = getred(colourset, i);
        }

        // body
        TheBody = calloc((unsigned int) (150 + (picwidth * machines[machine].height)), 1); // should check return code!
        for (i = 0; i < 150; i++)
        {   TheBody[i] = bmp_header[i];
        }
        i = 150;
        for (y = machines[machine].height - 1; y >= 0; y--)
        {   for (x = 0; x < machines[machine].width; x++)
            {   TheBody[i++] = (UBYTE) SCREENPTR(x, y);
                if (widening)
                {   TheBody[i++] = (UBYTE) SCREENPTR(x, y);
        }   }   }

        if (clip)
        {   clip_write((STRPTR) &TheBody[14], 150 - 14 + (picwidth * machines[machine].height), CF_DIB);
        } else
        {   DISCARD fwrite(TheBody, (size_t) (150      + (picwidth * machines[machine].height)), 1, TheLocalHandle);
        }
        free(TheBody);
        // TheBody = NULL; */
    acase KIND_ILBM:
    case KIND_ACBM:
    case KIND_IFFANIM:
        // header
        if (kind == KIND_IFFANIM)
        {   // assert(!clip);
            DISCARD fwrite(anim_header, (size_t) 12, 1, TheLocalHandle);
        }

        if (kind == KIND_ACBM)
        {   iff_header[ 8] = 'A';
            iff_header[ 9] = 'C';
         // iff_header[10] = 'B';
         // iff_header[11] = 'M';
            body_header[0] = 'A';
            body_header[1] = 'B';
            body_header[2] = 'I';
            body_header[3] = 'T';
        } else
        {   // assert(kind == KIND_ILBM || kind == KIND_IFFANIM);
            iff_header[ 8] = 'I';
            iff_header[ 9] = 'L';
         // iff_header[10] = 'B';
         // iff_header[11] = 'M';
            body_header[0] = 'B';
            body_header[1] = 'O';
            body_header[2] = 'D';
            body_header[3] = 'Y';
        }

        iff_header[20] =
        iff_header[36] = (UBYTE) (picwidth                 / 256);
        iff_header[21] =
        iff_header[37] = (UBYTE) (picwidth                 % 256);
        iff_header[22] =
        iff_header[38] = (UBYTE) (machines[machine].height / 256);
        iff_header[23] =
        iff_header[39] = (UBYTE) (machines[machine].height % 256);
        iff_header[30] = 0; // no compression
        ssbpl = picwidth / 8;
        if (picwidth % 8)
        {   ssbpl++;
        }
        if (ssbpl % 2)
        {   ssbpl++;
        }
        iffsize = 152 - 8 + (ssbpl * 5 * machines[machine].height); // for 5 bitplanes
        if (kind == KIND_ACBM)
        {   iffsize += (int) strlen("Generated by ")
                    +  (int) strlen(hostmachines[machine].titlebartext)
                    +  1;
        }
        if (iffsize & 1)
        {   iffsize++;
        }
        ulong_to_be(&iff_header[ 4], iffsize);
        iffsize = ssbpl * 5 * machines[machine].height; // for 5 bitplanes
        ulong_to_be(&body_header[4], iffsize);
        for (i = 0; i < 24; i++)
        {   iff_header[48 + (i * 3)] = getred(colourset, i);
            iff_header[49 + (i * 3)] = getgreen(colourset, i);
            iff_header[50 + (i * 3)] = getblue(colourset, i);
        }

        // body
        switch (kind)
        {
        case KIND_ACBM:
            if (clip)
            {   clip_write((STRPTR) iff_header,           144, 0);
                clip_write((STRPTR) body_header,            8, 0);
            } else
            {   DISCARD fwrite(     iff_header,  (size_t) 144, 1, TheLocalHandle);
                DISCARD fwrite(     body_header, (size_t)   8, 1, TheLocalHandle);
            }
            TheBody = calloc((size_t) (5 * ssbpl * machines[machine].height), 1); // for 5 bitplanes
            // should check return code!
            for (i = 0; i < 5; i++) // for 5 bitplanes
            {   for (y = 0; y < machines[machine].height; y++)
                {   for (x = 0; x < picwidth; x++)
                    {   if (widening)
                        {   t = SCREENPTR(x / 2, y);
                        } else
                        {   t = SCREENPTR(x    , y);
                        }
                        if (t & (1 << i))
                        {   TheBody[(i * machines[machine].height * ssbpl)
                                  + (y * ssbpl)
                                  + (x / 8)] |= 0x80 >> (x % 8);
            }   }   }   }
            if (clip)
            {   clip_write((STRPTR) TheBody,          ssbpl * 5 * machines[machine].height, 0                ); // for 5 bitplanes
            } else
            {   DISCARD fwrite(     TheBody, (size_t) ssbpl * 5 * machines[machine].height, 1, TheLocalHandle); // for 5 bitplanes
            }
            free(TheBody);
            // TheBody = NULL;

            // annotation
            annochunk[0] = 'A';
            annochunk[1] =
            annochunk[2] = 'N';
            annochunk[3] = 'O';
            annochunk[4] =
            annochunk[5] =
            annochunk[6] = 0;
            annochunk[7] = (UBYTE) (
                           (strlen("Generated by "))
                         + strlen(hostmachines[machine].titlebartext)
                         + 1);
            if (clip)
            {   clip_write((STRPTR) annochunk                         ,                                              8, 0);
                clip_write((STRPTR) "Generated by "                   , strlen("Generated by ")                       , 0);
                clip_write(         hostmachines[machine].titlebartext, strlen(hostmachines[machine].titlebartext) + 1, 0);                 // we deliberately include the NUL terminator
                if (annochunk[7] & 1)
                {   clip_write(     "\0"                              ,                                              1, 0);                 // pad byte
                }
            } else
            {   DISCARD fwrite(     annochunk                         ,                                              8, 1, TheLocalHandle);
                DISCARD fwrite(     "Generated by "                   , strlen("Generated by ")                       , 1, TheLocalHandle);
                DISCARD fwrite(     hostmachines[machine].titlebartext, strlen(hostmachines[machine].titlebartext) + 1, 1, TheLocalHandle); // we deliberately include the NUL terminator
                if (annochunk[7] & 1)
                {   DISCARD fwrite( "\0"                              ,                                              1, 1, TheLocalHandle); // pad byte
            }   }
            // ANNOtation strings *are* NULL-terminated (based on behaviour of OctaMED Pro 5.0 and SoundBox 2.9 beta)
        acase KIND_ILBM:
        case KIND_IFFANIM:
#ifdef UNCOMPRESSEDILBMS
            if (kind == KIND_ILBM)
            {   iff_header[30] = 0; // compression
                if (clip)
                {   clip_write((STRPTR) iff_header,  144, 0);
                    clip_write((STRPTR) body_header,   8, 0);
                } else
                {   DISCARD fwrite(iff_header,  (size_t) 144, 1, TheLocalHandle);
                    DISCARD fwrite(body_header, (size_t)   8, 1, TheLocalHandle);
                }
                TheBody = calloc((size_t) (5 * ssbpl * machines[machine].height), 1); // for 5 bitplanes
                // should check return code!
                for (y = 0; y < machines[machine].height; y++)
                {   for (i = 0; i < 5; i++) // for 5 bitplanes
                    {   for (x = 0; x < picwidth; x++)
                        {   if (widening)
                            {   t = SCREENPTR(x / 2, y);
                            } else
                            {   t = SCREENPTR(x,     y);
                            }
                            if (t & (1 << i))
                            {   TheBody[(y * 5 * ssbpl) // for 5 bitplanes
                                       +    (i * ssbpl)
                                       +    (x /  8)] |= 0x80 >> (x % 8);
                }   }   }   }
                if (clip)
                {   clip_write((STRPTR) TheBody, ssbpl * 5 * machines[machine].height, 0); // for 5 bitplanes
                } else
                {   DISCARD fwrite(TheBody, (size_t) ssbpl * 5 * machines[machine].height, 1, TheLocalHandle); // for 5 bitplanes
                }
                free(TheBody);
                // TheBody = NULL;
            } else
#endif
            {   totalbytes = 0;
                for (y = 0; y < machines[machine].height; y++)
                {   for (whichplane = 0; whichplane < 5; whichplane++)
                    {   memset(planarrow, 0, ssbpl);
                        for (x = 0; x < picwidth; x++)
                        {   if (widening)
                            {   t = SCREENPTR(x / 2, y);
                            } else
                            {   t = SCREENPTR(x,     y);
                            }
                            if (t & (1 << whichplane))
                            {   planarrow[(x / 8)] |= 0x80 >> (x % 8);
                        }   }

                        PackedBytes = PackRow
                        (   (BYTE*) planarrow,
                            &PackBuffer[totalbytes],
                            ssbpl
                        );
                        totalbytes += PackedBytes;
                }   }

                iffsize = 152 - 8 + totalbytes;
                if (iffsize & 1)
                {   iffsize++;
                }
                if (kind == KIND_IFFANIM && esvxfile)
                {   iffsize += 24;
                }
                ulong_to_be(&iff_header[ 4], iffsize);
                iff_header[30] = 1; // compression
                ulong_to_be(&body_header[4], totalbytes);
                if (clip)
                {   clip_write((STRPTR) iff_header,             144, 0);
                    clip_write((STRPTR) body_header,              8, 0);
                    clip_write((STRPTR) PackBuffer,      totalbytes, 0);
                } else
                {   DISCARD fwrite(iff_header,  (size_t)        144, 1, TheLocalHandle);
                    if (kind == KIND_IFFANIM && esvxfile)
                    {   fwrite(    sctl_header, (size_t)         24, 1, TheLocalHandle);
                    }
                    DISCARD fwrite(body_header, (size_t)          8, 1, TheLocalHandle);
                    DISCARD fwrite(PackBuffer,  (size_t) totalbytes, 1, TheLocalHandle);
                }
                if (totalbytes & 1)
                {   if (clip)
                    {   clip_write("\0", 1, 0); // pad byte
                    } else
                    {   DISCARD fwrite("\0", 1, 1, TheLocalHandle); // pad byte
                }   }

                if (kind == KIND_IFFANIM)
                {   for (whichplane = 0; whichplane < 5; whichplane++)
                    {   memset(planarscreen[0][whichplane], 0, ssbpl * machines[machine].height);
                        for (y = 0; y < machines[machine].height; y++)
                        {   for (x = 0; x < picwidth; x++)
                            {   if (widening)
                                {   t = SCREENPTR(x / 2, y);
                                } else
                                {   t = SCREENPTR(x,     y);
                                }
                                if (t & (1 << whichplane))
                                {   planarscreen[0][whichplane][(y * ssbpl) + (x / 8)] |= 0x80 >> (x % 8);
                        }   }   }
                        memcpy(planarscreen[1][whichplane], planarscreen[0][whichplane], ssbpl * machines[machine].height);
                        memcpy(planarscreen[2][whichplane], planarscreen[0][whichplane], ssbpl * machines[machine].height);
                        memcpy(planarscreen[3][whichplane], planarscreen[0][whichplane], ssbpl * machines[machine].height);
                        screenflip    = 0;
                        oldscreenflip = 2;
                        freshanim = TRUE;
    }   }   }   }   }

    if (clip)
    {   clip_close();
    } elif (animating)
    {   switch (kind)
        {
        case  KIND_GIF:     GIFHandle  = TheLocalHandle;
        acase KIND_MNG:     MNGHandle  = TheLocalHandle;
        acase KIND_PNG:     PNGHandle  = TheLocalHandle;
        acase KIND_IFFANIM: ANIMHandle = TheLocalHandle;
    }   }
    else
    {   DISCARD fclose(TheLocalHandle);
        // TheLocalHandle = NULL;
    }

#ifdef AMIGA
    writeicon(kind, (STRPTR) fn_screenshot);
#endif
}

MODULE void write_gif_line(FILE* file, GifEncoder* encoder, int width)
{   int            current_code, new_code,
                   i,
                   x = 0;
    unsigned long  new_key;
    unsigned char  pixval;

    if (encoder->current_code == FIRST_CODE)
        current_code = wideline[x++];
    else
        current_code = encoder->current_code;

    while (x < width)
    {   pixval = wideline[x++];

        new_key = (((unsigned long) current_code) << 8) + pixval;
        if ((new_code = lookup_gif_hash(encoder->hash_table, new_key)) >= 0)
        {   current_code = new_code;
        } else
        {   write_gif_code(file, encoder, current_code);
            current_code = pixval;

            if (encoder->running_code >= LZ_MAX_CODE)
            {   write_gif_code(file, encoder, encoder->clear_code);
                encoder->running_code = encoder->eof_code + 1;
                encoder->running_bits = encoder->depth + 1;
                encoder->max_code_plus_one = 1 << encoder->running_bits;
                for (i = 0; i < HT_SIZE; i++)
                {   encoder->hash_table[i] = 0xFFFFFFFF;
            }   }
            else
            {   add_gif_hash_entry(encoder->hash_table, new_key, encoder->running_code++);
    }   }   }

    encoder->current_code = current_code;
}
MODULE int lookup_gif_hash(unsigned long *hash_table, unsigned long key)
{   int hkey = gif_hash_key(key);
    unsigned long htkey;

    while ((htkey = HT_GET_KEY(hash_table[hkey])) != 0xFFFFFL)
    {   if (key == htkey)
        {   return (int) HT_GET_CODE(hash_table[hkey]);
        }
        hkey = (hkey + 1) & HT_KEY_MASK;
    }
    return -1;
}
MODULE void write_gif_code(FILE* file, GifEncoder* encoder, int code)
{   if (code == FLUSH_OUTPUT)
    {   while (encoder->shift_state > 0)
        {   write_gif_byte(file, encoder, encoder->shift_data & 0xff);
            encoder->shift_data >>= 8;
            encoder->shift_state -= 8;
        }
        encoder->shift_state = 0;
        write_gif_byte(file, encoder, FLUSH_OUTPUT);
    } else
    {   encoder->shift_data |= ((long) code) << encoder->shift_state;
        encoder->shift_state += encoder->running_bits;
        while (encoder->shift_state >= 8)
        {   write_gif_byte(file, encoder, encoder->shift_data & 0xff);
            encoder->shift_data >>= 8;
            encoder->shift_state -= 8;
    }   }

    if (encoder->running_code >= encoder->max_code_plus_one && code <= 4095)
    {   encoder->max_code_plus_one = 1 << ++encoder->running_bits;
}   }
MODULE int gif_hash_key(unsigned long key)
{   return (int) (((key >> 12) ^ key) & HT_KEY_MASK);
}
MODULE void add_gif_hash_entry(unsigned long* hash_table, unsigned long key, int code)
{   int hkey = gif_hash_key(key);

    while (HT_GET_KEY(hash_table[hkey]) != 0xFFFFFL)
    {   hkey = (hkey + 1) & HT_KEY_MASK;
    }
    hash_table[hkey] = HT_PUT_KEY(key) | HT_PUT_CODE(code);
}
MODULE void write_gif_byte(FILE* file, GifEncoder* encoder, int ch)
{   unsigned char *buf = encoder->buf;

    if (encoder->file_state == IMAGE_COMPLETE)
        return;

    if (ch == FLUSH_OUTPUT)
    {   if (encoder->bufsize)
        {   putc(encoder->bufsize, file);
            fwrite(buf, 1, encoder->bufsize, file);
            encoder->bufsize = 0;
        }
        putc(0, file);
        encoder->file_state = IMAGE_COMPLETE;
    } else
    {   if (encoder->bufsize == 255)
        {   putc(encoder->bufsize, file);
            fwrite(buf, 1, encoder->bufsize, file);
            encoder->bufsize = 0;
        }
        buf[encoder->bufsize++] = ch;
}   }
MODULE void init_gif_encoder(FILE* file, GifEncoder* encoder, int depth)
{   int i;

    /* if (depth < 2)
    {   depth = 2; // should never happen anyway
    } */

    encoder->file_state        = IMAGE_SAVING;
    encoder->position          = 0;
    encoder->bufsize           = 0;
    encoder->buf[0]            = 0;
    encoder->depth             = depth;
    encoder->clear_code        = 1 << depth;
    encoder->eof_code          = encoder->clear_code + 1;
    encoder->running_code      = encoder->eof_code + 1;
    encoder->running_bits      = depth + 1;
    encoder->max_code_plus_one = 1 << encoder->running_bits;
    encoder->current_code      = FIRST_CODE;
    encoder->shift_state       = 0;
    encoder->shift_data        = 0;

    DISCARD putc(depth, file);

    for (i = 0; i < HT_SIZE; i++)
    {   encoder->hash_table[i] = 0xFFFFFFFF;
    }

    write_gif_code(file, encoder, encoder->clear_code);
}
MODULE void flush_gif_encoder(FILE* file, GifEncoder* encoder)
{   write_gif_code(file, encoder, encoder->current_code);
    write_gif_code(file, encoder, encoder->eof_code);
    write_gif_code(file, encoder, FLUSH_OUTPUT);
}

EXPORT void writegifframe(void)
{   TRANSIENT int        i = 0,
                         x, y;
    TRANSIENT UBYTE      TheBody[18];
    PERSIST   GifEncoder encoder; // PERSISTent so as not to blow the stack
    FAST      ASCREEN*   screenptr;
    FAST      int        dupbpl;

    // assert(gifanims);

    // animation block
    TheBody[i++] = 0x21; // GIF Extension code
    TheBody[i++] = 0xF9; // Graphic Control Label
    TheBody[i++] = 0x04; // Length of Application Block
     TheBody[i++] = 0x04; // disposal method
     TheBody[i++] = 0x02; // delay (2/100ths of a sec)
     TheBody[i++] = 0x00;
     TheBody[i++] = 0x00; // transparent colour index
    TheBody[i++] = 0x00; // Data Sub-block Terminator.

    // image descriptor block
    TheBody[i++] = 0x2C; // separator
    TheBody[i++] =    0;
    TheBody[i++] =    0; // left
    TheBody[i++] =    0;
    TheBody[i++] =    0; // top
    TheBody[i++] = picwidth  % 256;
    TheBody[i++] = picwidth  / 256;
    TheBody[i++] = machines[machine].height % 256;
    TheBody[i++] = machines[machine].height / 256;
    TheBody[i++] =    0; // terminator
    DISCARD fwrite(TheBody, (size_t) i, 1, GIFHandle);

    if ((memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) && rotate)
    {   // assert(COINOP_BOXWIDTH == COINOP_BOXHEIGHT);
        for (x = 0; x < COINOP_BOXWIDTH; x++)
        {   for (y = 0; y < COINOP_BOXHEIGHT; y++)
            {   dupscreen[x][y] = screen[y][COINOP_BOXWIDTH - 1 - x];
        }   }
        screenptr = &dupscreen[0][0];
        dupbpl = COINOP_BOXHEIGHT;
    } else
    {   screenptr = &screen[0][0];
        dupbpl = BOXHEIGHT;
    }

    // body
    init_gif_encoder(GIFHandle, &encoder, 5); // 5 bitplanes
    for (y = 0; y < machines[machine].height; y++)
    {   i = 0;
        for (x = 0; x < machines[machine].width; x++)
        {   wideline[i++] = SCREENPTR(x, y);
            if (widening)
            {   wideline[i++] = SCREENPTR(x, y);
        }   }
        write_gif_line(GIFHandle, &encoder, picwidth);
    }
    flush_gif_encoder(GIFHandle, &encoder);
}

EXPORT void writeapngframe(void)
{   // assert(apnganims);

    write_fctl(PNGHandle);
    write_idat_or_fdat(PNGHandle, TRUE, KIND_PNG, TRUE);
}

EXPORT void writemngframe(void)
{   // assert(mnganims);

    write_ihdr_and_plte(MNGHandle);
    write_idat_or_fdat(MNGHandle, FALSE, KIND_MNG, TRUE);
    write_iend(MNGHandle);
}

MODULE void writeulong(ULONG value, UBYTE* ptr)
{   ptr[0] = (UBYTE) ( value / 16777216         );
    ptr[1] = (UBYTE) ((value % 16777216) / 65536);
    ptr[2] = (UBYTE) ((value %    65536) /   256);
    ptr[3] = (UBYTE) ( value %      256         );
}

EXPORT void write_idat_or_fdat(FILE* filehandle, FLAG is_fdat, int kind, FLAG animating)
{   TRANSIENT int        x, y;
    TRANSIENT ULONG      crc;
    TRANSIENT void*      pPNG_data;
    TRANSIENT UBYTE*     pImage;
    FAST      ASCREEN*   screenptr;
    FAST      int        dupbpl;
    PERSIST   UBYTE      idat[8] = {
  0 ,  0 ,  0 ,  0 , // 0..3 IDAT length
 'I', 'D', 'A', 'T', // 4..7 IDAT name
}, fdat[12] = {
  0 ,  0 ,  0 ,  0 , // 0.. 3 fdAT length
 'f', 'd', 'A', 'T', // 4.. 7 fdAT name
  0 ,  0 ,  0 ,  0 , // 8..11 fdAT: sequence number
}, idat_or_fdat_crc[4];

    if ((memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) && rotate)
    {   // assert(COINOP_BOXWIDTH == COINOP_BOXHEIGHT);
        for (x = 0; x < COINOP_BOXWIDTH; x++)
        {   for (y = 0; y < COINOP_BOXHEIGHT; y++)
            {   dupscreen[x][y] = screen[y][COINOP_BOXWIDTH - 1 - x];
        }   }
        screenptr = &dupscreen[0][0];
        dupbpl = COINOP_BOXHEIGHT;
    } else
    {   screenptr = &screen[0][0];
        dupbpl = BOXHEIGHT;
    }

#ifdef TRUECOLOUR
    pImage = (UBYTE*) malloc(picwidth * machines[machine].height * 3);
#else
    pImage = (UBYTE*) malloc(picwidth * machines[machine].height);
#endif

    for (y = 0; y < machines[machine].height; y++)
    {   if (widening)
        {   for (x = 0; x < machines[machine].width; x++)
            {
#ifdef TRUECOLOUR
    #ifdef WIN32
        #ifdef MNGPLAY
                pImage[(y * machines[machine].width * 6) + (x * 6) + 2] =
                pImage[(y * machines[machine].width * 6) + (x * 6) + 5] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)]    &   0xFF0000) >> 16);
                pImage[(y * machines[machine].width * 6) + (x * 6) + 0] =
                pImage[(y * machines[machine].width * 6) + (x * 6) + 3] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)]    &   0x0000FF)      );
        #else
                pImage[(y * machines[machine].width * 6) + (x * 6) + 0] =
                pImage[(y * machines[machine].width * 6) + (x * 6) + 3] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)]    &   0xFF0000) >> 16);
                pImage[(y * machines[machine].width * 6) + (x * 6) + 2] =
                pImage[(y * machines[machine].width * 6) + (x * 6) + 5] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)]    &   0x0000FF)      );
        #endif
                pImage[(y * machines[machine].width * 6) + (x * 6) + 1] =
                pImage[(y * machines[machine].width * 6) + (x * 6) + 4] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)]    &   0x00FF00) >>  8);
    #endif
    #ifdef AMIGA
        #ifdef MNGPLAY
                pImage[(y * machines[machine].width * 6) + (x * 6) + 2] =
                pImage[(y * machines[machine].width * 6) + (x * 6) + 5] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)].red   & 0xFF000000) >> 24);
                pImage[(y * machines[machine].width * 6) + (x * 6) + 0] =
                pImage[(y * machines[machine].width * 6) + (x * 6) + 3] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)].blue  & 0xFF000000) >> 24);
        #else
                pImage[(y * machines[machine].width * 6) + (x * 6) + 0] =
                pImage[(y * machines[machine].width * 6) + (x * 6) + 3] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)].red   & 0xFF000000) >> 24);
                pImage[(y * machines[machine].width * 6) + (x * 6) + 2] =
                pImage[(y * machines[machine].width * 6) + (x * 6) + 5] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)].blue  & 0xFF000000) >> 24);
        #endif
                pImage[(y * machines[machine].width * 6) + (x * 6) + 1] =
                pImage[(y * machines[machine].width * 6) + (x * 6) + 4] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)].green & 0xFF000000) >> 24);
    #endif
#else
                pImage[(y * picwidth) + (x * 2)    ] =
                pImage[(y * picwidth) + (x * 2) + 1] = SCREENPTR(x, y);
#endif
        }   }
        else
        {   for (x = 0; x < picwidth; x++)
            {
#ifdef TRUECOLOUR
    #ifdef WIN32
        #ifdef MNGPLAY
                pImage[(y * picwidth                * 3) + (x * 3) + 2] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)]       &   0xFF0000) >> 16);
                pImage[(y * picwidth                * 3) + (x * 3) + 0] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)]       &   0x0000FF)      );
        #else
                pImage[(y * picwidth                * 3) + (x * 3) + 0] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)]       &   0xFF0000) >> 16);
                pImage[(y * picwidth                * 3) + (x * 3) + 2] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)]       &   0x0000FF)      );
        #endif
                pImage[(y * picwidth                * 3) + (x * 3) + 1] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)]       &   0x00FF00) >>  8);
    #endif
    #ifdef AMIGA
        #ifdef MNGPLAY
                pImage[(y * picwidth                * 3) + (x * 3) + 2] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)].red   & 0xFF000000) >> 24);
                pImage[(y * picwidth                * 3) + (x * 3) + 0] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)].blue  & 0xFF000000) >> 24);
        #else
                pImage[(y * picwidth                * 3) + (x * 3) + 0] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)].red   & 0xFF000000) >> 24);
                pImage[(y * picwidth                * 3) + (x * 3) + 2] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)].blue  & 0xFF000000) >> 24);
        #endif
                pImage[(y * picwidth                * 3) + (x * 3) + 1] = (UBYTE) ((pencolours[colourset][SCREENPTR(x, y)].green & 0xFF000000) >> 24);
    #endif
#else
                pImage[(y * picwidth) +  x         ] = SCREENPTR(x, y);
#endif
    }   }   }

    if ((pPNG_data = write_png(pImage, picwidth, machines[machine].height)))
    {   if (is_fdat)
        {   writeulong(png_data_size + 4, &fdat[0]);
            writeulong(pngframes, &fdat[8]);
            DISCARD fwrite(fdat, 12, 1, filehandle);
            DISCARD fwrite(pPNG_data, png_data_size, 1, filehandle);
            crc = crc32(&fdat[4], 8);
            crc = crc32_cont(pPNG_data, png_data_size, crc);
            if (kind == KIND_PNG && animating)
            {   pngframes++;
        }   }
        else
        {   writeulong(png_data_size, &idat[0]);
            DISCARD fwrite(idat, 8, 1, filehandle);
            DISCARD fwrite(pPNG_data, png_data_size, 1, filehandle);
            crc = crc32(&idat[4], 4);
            crc = crc32_cont(pPNG_data, png_data_size, crc);
            if (kind == KIND_MNG)
            {   // assert(animating);
                mngframes++;
        }   }
        free(pPNG_data);
        // pPNG_data = NULL;
        writeulong(crc, idat_or_fdat_crc);
        DISCARD fwrite(idat_or_fdat_crc, 4, 1, filehandle);
        free(pImage);
        // pImage = NULL;
    } else
    {   say("Can't write to PNG file!");
        free(pImage);
        // pImage = NULL;
        macro_stop();
        // perhaps we should delete the incomplete PNG file
}   }

EXPORT void write_iend(FILE* filehandle)
{   PERSIST UBYTE iend[12] = {
   0 ,  0 ,  0 ,  0 , //  0.. 3 IEND length
  'I', 'E', 'N', 'D', //  4.. 7 IEND name
 0xae,0x42,0x60,0x82  //  8..11 IEND CRC
};

    DISCARD fwrite(iend, 12, 1, filehandle);
}
EXPORT void write_mend(FILE* filehandle)
{   TRANSIENT ULONG crc;
    PERSIST   UBYTE mend[12] = {
   0 ,  0 ,  0 ,  0 , //  0.. 3 MEND length
  'M', 'E', 'N', 'D', //  4.. 7 MEND name
   0 ,  0 ,  0 ,  0   //  8..11 MEND CRC
};

    crc = crc32(&mend[4], 4);
    writeulong(crc, &mend[8]);
    DISCARD fwrite(mend, 12, 1, filehandle);
}

EXPORT void write_actl_start(FILE* filehandle)
{   PERSIST UBYTE actl[20] = {
   0 ,  0 ,  0 ,  8 , //  0.. 3 acTL length
  'a', 'c', 'T', 'L', //  4.. 7 acTL name
   0 ,  0,   0 ,  0 , //  8..11  acTL: number of frames
   0 ,  0,   0,   1 , // 12..15  acTL: number of loops (0 means infinite)
   0 ,  0,   0 ,  0 , // 16..19 acTL CRC
};

#ifdef LOOPABLE
    actl[15] = 0;
#else
    actl[15] = 1;
#endif

    DISCARD fwrite(actl, 20, 1, filehandle);
}

EXPORT void write_actl_end(FILE* filehandle)
{   TRANSIENT ULONG crc;
    PERSIST   UBYTE actl[20] = {
   0 ,  0 ,  0 ,  8 , //  0.. 3 acTL length
  'a', 'c', 'T', 'L', //  4.. 7 acTL name
   0 ,  0,   0 ,  0 , //  8..11  acTL: number of frames
   0 ,  0,   0,   0 , // 12..15  acTL: number of loops (0 means infinite)
   0 ,  0,   0 ,  0 , // 16..19 acTL CRC
};

#ifdef LOOPABLE
    actl[15] = 0;
#else
    actl[15] = 1;
#endif

    writeulong((pngframes + 2) / 2, &actl[8]);
    crc = crc32(&actl[4], 12); // acTL CRC
    writeulong(crc, &actl[16]);

    DISCARD fseek(filehandle, 33 + (12 + (3 * 24)), SEEK_SET);
    fwrite(actl, 20, 1, filehandle);
}

EXPORT void write_fctl(FILE* filehandle)
{   TRANSIENT ULONG crc;
    PERSIST   UBYTE fctl[38] = {
   0 ,  0 ,  0,  26 , //  0.. 3 fcTL length
  'f', 'c', 'T', 'L', //  4.. 7 fcTL ID
   0 ,  0 ,  0 ,  0 , //  8..11  sequence number
   0 ,  0 ,  0 ,  0 , // 12..15  width
   0 ,  0 ,  0 ,  0 , // 16..19  height
   0 ,  0 ,  0 ,  0 , // 20..23  X-offset
   0 ,  0 ,  0 ,  0 , // 24..27  Y-offset
   0 ,  2 ,  0,   0 , // 28..31  delay (2/100ths of a sec)
   0 ,  0 ,           // 32..33  operations
   0 ,  0 ,  0 ,  0 , // 34..37 fcTL CRC
};

    writeulong(pngframes, &fctl[8]);
    fctl[14] =  picwidth    / 256;
    fctl[15] =  picwidth    % 256;
    fctl[18] =  machines[machine].height / 256;
    fctl[19] =  machines[machine].height % 256;
    crc = crc32(&fctl[4], 30);
    writeulong(crc, &fctl[34]);
    DISCARD fwrite(fctl, 38, 1, filehandle);
    pngframes++;
}

MODULE void write_ihdr_and_plte(FILE* filehandle)
{   FAST    ULONG crc;
#ifndef TRUECOLOUR
    FAST    int   i;
#endif
    PERSIST UBYTE ihdr_and_plte[109] = {
// IHDR
0x00,0x00,0x00, 13 ,                    //   0..  3 IHDR length
 'I', 'H', 'D', 'R',                    //   4..  7 IHDR name
  0 ,  0 ,  0 ,  0 ,                    //   8.. 11  IHDR: width
  0 ,  0 ,  0 ,  0 ,                    //  12.. 15  IHDR: height
  8 ,                                   //  16       IHDR: depth
#ifdef TRUECOLOUR
  2 ,                                   //  17       IHDR: colour type (2 means 24-bit)
#else
  3 ,                                   //  17       IHDR: colour type (3 means indexed)
#endif
  0 ,                                   //  18       IHDR: compression
  0 ,                                   //  19       IHDR: filter
  0 ,                                   //  20       IHDR: interlace
  0 ,  0 ,  0 ,  0 ,                    //  21.. 24 IHDR CRC
// PLTE
  0 ,  0 ,  0 ,24*3,                    //  25.. 28 PLTE length
 'P', 'L', 'T', 'E',                    //  29.. 32 PLTE name
                                        //  33..104  PLTE: palettes
                                        // 105..108 PLTE CRC
};

        ihdr_and_plte[10] = (UBYTE) (picwidth    / 256);
        ihdr_and_plte[11] = (UBYTE) (picwidth    % 256);
        ihdr_and_plte[14] = (UBYTE) (machines[machine].height / 256);
        ihdr_and_plte[15] = (UBYTE) (machines[machine].height % 256);
        crc = crc32(&ihdr_and_plte[4], 17); // IHDR CRC
        writeulong(crc, &ihdr_and_plte[21]);

#ifdef TRUECOLOUR
        DISCARD fwrite(ihdr_and_plte, 25, 1, filehandle);
#else
        // palette (72 bytes)
        for (i = 0; i < 24; i++)
        {
#ifdef MNGPLAY
            ihdr_and_plte[33 + (i * 3)] = getblue( colourset, i);
            ihdr_and_plte[34 + (i * 3)] = getgreen(colourset, i);
            ihdr_and_plte[35 + (i * 3)] = getred(  colourset, i);
#else
            ihdr_and_plte[33 + (i * 3)] = getred(  colourset, i);
            ihdr_and_plte[34 + (i * 3)] = getgreen(colourset, i);
            ihdr_and_plte[35 + (i * 3)] = getblue( colourset, i);
#endif
        }
        crc = crc32(&ihdr_and_plte[29], 4 + (24 * 3)); // PLTE CRC (4 bytes)
        writeulong(crc, &ihdr_and_plte[105]);
        DISCARD fwrite(ihdr_and_plte, 109, 1, filehandle);
#endif
}

EXPORT void write_mhdr_end(FILE* filehandle)
{   ULONG crc;

    // assert(MNGHandle);

    writeulong(mngframes, &mng_header[28]);
    writeulong(mngframes, &mng_header[32]);
    writeulong(mngframes, &mng_header[36]);
    crc = crc32(&mng_header[12], 32); // MHDR CRC
    writeulong(crc, &mng_header[44]);
    DISCARD fseek(filehandle, 28, SEEK_SET);
    DISCARD fwrite(&mng_header[28], 20, 1, filehandle); // rewrite bytes 28..47 of file
}

EXPORT void finalize_anim(FILE* filehandle)
{   FLAG   ok;
    UBYTE  temp[4];
    long   filepos;
    int    animsize,
           esvxsize;
    UBYTE *ANIMBuffer,
          *ESVXBuffer;
    FILE*  LocalHandle;

#ifdef LOOPABLE
    write_dlta(3, oldscreenflip);
    write_dlta(4, oldscreenflip);
#endif

    filepos = ftell(filehandle);
    DISCARD fseek(filehandle, 4, SEEK_SET);
    writeulong(filepos - 8, temp);
    DISCARD fwrite(temp, 4, 1, filehandle); // rewrite bytes 4..7 of file
    fclose(ANIMHandle);
    ANIMHandle = NULL;

    if (esvxfile && bitrate == 8) // because probably only 8SVX (not 16SV) are supported in ANIMs
    {   ok = FALSE;
        strcpy((char*) fn_save, (const char*) fn_game);
        fixextension(filekind[KIND_8SVX].extension, (STRPTR) fn_save, TRUE, "");
        esvxsize = getsize((STRPTR) fn_save);
        if (esvxsize && (ESVXBuffer = malloc(esvxsize)))
        {   if ((LocalHandle = fopen((const char*) fn_save, "rb")))
            {   if (fread(ESVXBuffer, (size_t) esvxsize, 1, LocalHandle) == 1)
                {   ok = TRUE;
                }
                fclose(LocalHandle);
            }
            if (ok)
            {   ok = FALSE;
                strcpy((char*) fn_save, (const char*) fn_game);
                fixextension(filekind[KIND_IFFANIM].extension, (STRPTR) fn_save, TRUE, "");
                animsize = getsize((STRPTR) fn_save);
                if (animsize && (ANIMBuffer = malloc(animsize)))
                {   if ((LocalHandle = fopen((const char*) fn_save, "rb")))
                    {   if (fread(ANIMBuffer, (size_t) animsize, 1, LocalHandle) == 1)
                        {   ok = TRUE;
                        }
                        fclose(LocalHandle);
                    }
                    if (ok && (LocalHandle = fopen((const char*) fn_save, "wb")))
                    {   ulong_to_be(   &ANIMBuffer[ 4], animsize - 8 + esvxsize);
                        DISCARD fwrite(&ANIMBuffer[ 0], (size_t)            12, 1, LocalHandle);
                        DISCARD fwrite(&ESVXBuffer[ 0], (size_t)      esvxsize, 1, LocalHandle);
                        DISCARD fwrite(&ANIMBuffer[12], (size_t) animsize - 12, 1, LocalHandle);
                        fclose(LocalHandle);
                    }
                    free(ANIMBuffer);
            }   }
            free(ESVXBuffer);
}   }   }

EXPORT void writeanimframe(void)
{   FAST UBYTE t;
    FAST LONG  whichplane;
    FAST int   x, y;

    for (whichplane = 0; whichplane < 5; whichplane++)
    {   memset(planarscreen[screenflip][whichplane], 0, ssbpl * machines[machine].height);
        if ((memmap == MEMMAP_ASTROWARS || memmap == MEMMAP_GALAXIA) && rotate)
        {   // assert(COINOP_BOXWIDTH == COINOP_BOXHEIGHT);
            for (x = 0; x < COINOP_BOXWIDTH; x++)
            {   for (y = 0; y < COINOP_BOXHEIGHT; y++)
                {   dupscreen[x][y] = screen[y][COINOP_BOXWIDTH - 1 - x];
            }   }
            screenptr = &dupscreen[0][0];
            dupbpl = COINOP_BOXHEIGHT;
        } else
        {   screenptr = &screen[0][0];
            dupbpl = BOXHEIGHT;
        }

        for (y = 0; y < machines[machine].height; y++)
        {   for (x = 0; x < picwidth; x++)
            {   if (widening)
                {   t = SCREENPTR(x / 2, y);
                } else
                {   t = SCREENPTR(x,     y);
                }
                if (t & (1 << whichplane))
                {   planarscreen[screenflip][whichplane][(y * ssbpl) + (x / 8)] |= 0x80 >> (x % 8);
    }   }   }   }

    if (freshanim)
    {   freshanim = FALSE;
        memcpy(planarscreen[4][0], planarscreen[screenflip][0], ssbpl * machines[machine].height);
        memcpy(planarscreen[4][1], planarscreen[screenflip][1], ssbpl * machines[machine].height);
        memcpy(planarscreen[4][2], planarscreen[screenflip][2], ssbpl * machines[machine].height);
        memcpy(planarscreen[4][3], planarscreen[screenflip][3], ssbpl * machines[machine].height);
        memcpy(planarscreen[4][4], planarscreen[screenflip][4], ssbpl * machines[machine].height);
    }

    write_dlta(screenflip, oldscreenflip);
}

MODULE void write_dlta(int newerscreen, int olderscreen)
{   FAST LONG   DeltaCount = 64,
                i,
                PlaneCount[5],
                whichplane;
    FAST UBYTE *DeltaPtr,
               *newerplane,
               *olderplane;
    FAST LONG*  DeltaData;
#ifdef WIN32
    FAST ULONG  endian;
#endif
    PERSIST UBYTE  anhd[68] = {
     'F',  'O',  'R',  'M', 0xFF, 0xFF, 0xFF, 0xFF,   // "FORMxxxx"
     'I',  'L',  'B',  'M',  'A',  'N',  'H',  'D',   // "ILBMANHD"
    0x00, 0x00, 0x00, 0x28, // chunk size
    0x05,                   // operation
    0x00,                   // XOR mask
    0x00, 0x00,             // XOR width
    0x00, 0x00,             // XOR height
    0x00, 0x00,             // XOR x
    0x00, 0x00,             // XOR y
    0x00, 0x00, 0x00, 0x00, // abstime
    0x00, 0x00, 0x00, 0x01, // reltime
    0x00,                   // interleave
    0x00,                   // pad byte
    0x00, 0x00, 0x00, 0x00, // bits
    0x00, 0x00, 0x00, 0x00, // reserved
    0x00, 0x00, 0x00, 0x00, // reserved
    0x00, 0x00, 0x00, 0x00, // reserved
    0x00, 0x00, 0x00, 0x00, // reserved
     'D',  'L',  'T',  'A', // chunk ID
    0xFF, 0xFF, 0xFF, 0xFF  // chunk size (filled later)
    };

    for (whichplane = 0; whichplane < 5; whichplane++)
    {   newerplane             = planarscreen[newerscreen][whichplane];
        olderplane             = planarscreen[olderscreen][whichplane];
        PlaneCount[whichplane] = 0;
        i                      = ssbpl * machines[machine].height;

        while (i--)
        {   if (*newerplane++ != *olderplane++)
            {   PlaneCount[whichplane] =  skip_count_plane(planarscreen[newerscreen][whichplane], planarscreen[olderscreen][whichplane], (SWORD) ssbpl, (SWORD) machines[machine].height);
                DeltaCount             += PlaneCount[whichplane];
                break;
    }   }   }

    DeltaData  = (LONG*) calloc(DeltaCount, 1);
    DeltaPtr   = (UBYTE*) &DeltaData[16];
    DeltaCount = 64;
    for (whichplane = 0; whichplane < 5; whichplane++)
    {   if (PlaneCount[whichplane])
        {
#ifdef WIN32
            endian = ((DeltaCount & 0xFF000000) >> 24)
                   | ((DeltaCount & 0x00FF0000) >>  8)
                   | ((DeltaCount & 0x0000FF00) <<  8)
                   | ((DeltaCount & 0x000000FF) << 24);
            DeltaData[whichplane] =  endian;
#endif
#ifdef AMIGA
            DeltaData[whichplane] =  DeltaCount;
#endif
            DeltaCount            += PlaneCount[whichplane];
            DeltaPtr              =  skip_comp_plane(planarscreen[newerscreen][whichplane], planarscreen[olderscreen][whichplane], DeltaPtr, (SWORD) ssbpl, (SWORD) machines[machine].height);
    }   }

    switch (screenflip)
    {
    case  0: screenflip = 1; oldscreenflip = 2;
    acase 1: screenflip = 2; oldscreenflip = 0;
    acase 2: screenflip = 0; oldscreenflip = 1;
    }

    if (DeltaCount & 1)
    {   writeulong(68 - 8 + DeltaCount + 1, &anhd[4]);
    } else
    {   writeulong(68 - 8 + DeltaCount    , &anhd[4]);
    }
    writeulong(DeltaCount, &anhd[64]);
    DISCARD fwrite(anhd,                       68, 1, ANIMHandle);
    DISCARD fwrite(DeltaData, (size_t) DeltaCount, 1, ANIMHandle);
    free(DeltaData);
    if (DeltaCount & 1)
    {   DISCARD fwrite("\0", 1, 1, ANIMHandle); // pad byte
}   }

/* skip_count_plane() - figure out what size this plane will compress
   to using vertical-byte-run-length-with-skips encoding */

MODULE SWORD skip_count_plane(UBYTE *in, UBYTE *last_in, SWORD next_line, SWORD rows)
{   SWORD comp_count,
          i;

    linebytes  = next_line;
    comp_count = 0;
    i          = next_line;
    while (--i >= 0)
    {   comp_count += skip_count_line(in, last_in, rows);
        in++;
        last_in++;
    }
    return comp_count;
}

/* skip_count_line() - figure out what size this column will compress
   to using vertical-byte-run-length-with-skips encoding */

MODULE SWORD skip_count_line(UBYTE* in, UBYTE* last_in, SWORD count)
{   SWORD a_run,
          comp_count = 1, /* one for the op count */
          local_count,
          run_length,
          uniq_count = 0;

    if (vskip(in, last_in, count) == count) /* skip whole column? */
    {   return 1;
    }
    for (;;)
    {   if (count <= 0)
        {   break;
        }
        local_count = (count < MAXRUN_SKIP ? count : MAXRUN_SKIP);
        a_run = 0;
        if ((run_length = vskip(in, last_in, local_count)) > 1)
        {   count -= run_length;
            if (count > 0)  /* the last skip disappears */
            {   comp_count += 1;
            }
            a_run = 1;
        } elif ((run_length = vsame(in, local_count)) > 3)
        {   count -= run_length;
            a_run = 1;
            comp_count += 3;
        }
        if (a_run)
        {   in += run_length * linebytes;
            last_in += run_length * linebytes;
            if (uniq_count > 0)
            {   comp_count += uniq_count + 1;
                uniq_count = 0;
        }   }
        else
        {   in += linebytes;
            last_in += linebytes;
            uniq_count++;
            count -= 1;
            if (uniq_count == MAXRUN_SKIP)
            {   comp_count += uniq_count + 1;
                uniq_count = 0;
    }   }   }
    if (uniq_count != 0)
    {   comp_count += uniq_count + 1;
    }
    return comp_count;
}

/* skip_comp_plane() - Compress "in" into "out" using vertical-byte-run-
   with-skips encodeing. Return pointer to "out"'s next free space. */

MODULE UBYTE* skip_comp_plane(UBYTE* in, UBYTE* last_in, UBYTE* out, SWORD next_line, SWORD rows)
{   SWORD i;

    linebytes = next_line;
    i = next_line;
    while (--i >= 0)
    {   out = skip_comp_line(in, last_in, out, rows);
        in++;
        last_in++;
    }
    return out;
}

/* skip_comp_line() - Compress "in" into "out" using vertical-byte-run-
   with-skips encodeing. Return pointer to "out"'s next free space. */

MODULE UBYTE* skip_comp_line(UBYTE* in, UBYTE* last_in, UBYTE* out, SWORD count)
{   UBYTE* stuffit;
    SWORD  a_run,
           local_count,
           run_length;

/* if can skip over whole column, then compact a bit by just setting the
   "op count" for this column to zero */

    if (vskip(in, last_in, count) == count) /* skip whole column? */
    {   *out++ = 0;
        return out;
    }

    op_count = 0;       /* haven't done any ops yet */

/* initialize variables which keep track of how many uniq bytes we've gone
   past, and where uniq run started. */

    uniq_count = 0;
    uniq = in;

    stuffit = out + 1;      /* skip past "op-count" slot in out array */
    for (;;)
    {   if (count <= 0)
        {   break;
        }
        local_count = (count < MAXRUN_SKIP ? count : MAXRUN_SKIP);
        a_run = 0;      /* first assume not a skip or a same run */
        /* see how much could skip from here. Two or more is worth skipping! */
        if ((run_length = vskip(in, last_in, local_count)) > 1)
        {   a_run = 1;
            count -= run_length;
            stuffit = flush_uniq(stuffit);  /* flush pending "uniq" run */
            if (count > 0)  /* last skip vanishes */
            {   op_count++;
                *stuffit++ = (UBYTE) run_length;
        }   }
        /* four or more of the same byte in a row compresses too */
        elif ((run_length = vsame(in, local_count)) > 3)
        {   a_run = 1;
            count -= run_length;
            op_count++;
            stuffit = flush_uniq(stuffit);  /* flush pending "uniq" run */
            *stuffit++ = 0;
            *stuffit++ = (UBYTE) run_length;
            *stuffit++ = *in;
        }

        /* if it's a run of some sort update in and last_in pointer, and
           reset the uniq pointer to the current position */

        if (a_run)
        {   in += run_length * linebytes;
            last_in += run_length * linebytes;
            uniq = in;

        /* otherwise just continue accumulating stuff in uniq for later
           flushing or immediate if it get's past MAXRUN_SKIP */

        } else
        {   in += linebytes;
            last_in += linebytes;
            uniq_count++;
            count -= 1;
            if (uniq_count == MAXRUN_SKIP)
            {   stuffit = flush_uniq(stuffit);
                uniq = in;
    }   }   }

    // if came to end of column within a uniq-run still have to flush it

    if (uniq_count != 0)
    {   stuffit = flush_uniq(stuffit);
    }

    // and stuff the first byte of this (compressed) column with the op_count

    *out = (UBYTE) op_count;
    return stuffit;
}

/* count up how many in a column are the same between in and last_in ...
   ie how big of a "skip" can we go on. */

MODULE SWORD vskip(UBYTE* in, UBYTE* last_in, SWORD count)
{   SWORD skips;

    skips = 0;
    while (--count >= 0)
    {   if (*in != *last_in)
        {   break;
        }
        in += linebytes;
        last_in += linebytes;
        skips++;
    }
    return skips;
}

/* vsame() - count up how many in a row (vertically) are the same as the
   first one...ie. how big of a "same" op we can have */

MODULE SWORD vsame(UBYTE* in, SWORD count)
{   unsigned char c;
    SWORD         same;

    c = *in;
    in += linebytes;
    --count;
    same = 1;
    while (--count >= 0)
    {   if (*in != c)
        {   break;
        }
        same++;
        in += linebytes;
    }
    return same;
}

/* flush_uniq() - write out the "uniq" run that's been accumulating while
   we've been looking for skips and "same" runs. */

MODULE unsigned char* flush_uniq(unsigned char* stuff)
{   if (uniq_count > 0)
    {   op_count++;
        *stuff++ = (uniq_count | 0x80);
        copy_line_to_chars(uniq, stuff, linebytes, uniq_count);
        stuff += uniq_count;
        uniq_count = 0;
    }
    return stuff;
}

MODULE int copy_line_to_chars(unsigned char* in, unsigned char* out, int linebytes, int count)
{   while (count--)
    {   *out = *in;
        out++;
        in += linebytes;
    }
    return 0;
}

/*
 * Flush out the data dump.
 */

MODULE BYTE* PutDump(BYTE* Destination, LONG Count)
{   register BYTE* Source = Buffer;

    *Destination++   = Count - 1;
     PackedBytes    += Count + 1;

    while (Count--)
    {   *Destination++ = *Source++;
    }

    return Destination;
}

/*
 * Flush out a run of data bytes.
 */

MODULE BYTE* PutRun(BYTE* Destination, LONG Count, BYTE Char)
{   *Destination++ =  -(Count - 1);
    *Destination++ =  Char;
     PackedBytes   += 2;

    return Destination;
}

/*
 * Compress a single row of bytes.
 */

MODULE LONG PackRow(BYTE* Source, BYTE* Destination, LONG RowSize)
{   SWORD Buffered = 1,
          RunStart = 0;
    BYTE  Mode     = DUMP,
          LastChar,
          Char;

    PackedBytes = 0;

    Buffer[0] = LastChar = Char = *Source++;

    RowSize--;

    while (RowSize--)
    {   Buffer[Buffered++] = Char = *Source++;

        if (Mode)
        {   if ((Char != LastChar) || (Buffered - RunStart > MAXRUN_PACKER))
            {   Destination = PutRun(Destination, Buffered - 1 - RunStart, LastChar);
                Buffer[0]   = Char;
                Buffered    = 1;
                RunStart    = 0;
                Mode        = DUMP;
        }   }
        else
        {   if (Buffered > MAXDAT)
            {   Destination = PutDump(Destination, Buffered - 1);
                Buffer[0]   = Char;
                Buffered    = 1;
                RunStart    = 0;
            } else
            {   if (Char == LastChar)
                {   if (Buffered - RunStart >= MINRUN)
                    {   if (RunStart)
                        {   Destination = PutDump(Destination, RunStart);
                        }
                        Mode = RUN;
                    } else
                    {   if (!RunStart)
                        {   Mode = RUN;
                }   }   }
                else
                {   RunStart = Buffered - 1;
        }   }   }

        LastChar = Char;
    }

    if (Mode)
    {   PutRun(Destination, Buffered - RunStart, LastChar);
    } else
    {   PutDump(Destination, Buffered);
    }

    return PackedBytes;
}

EXPORT void sound_startrecording(void)
{   FAST int  i, j;
    FAST TEXT tempstring[7 + 1];

    for (i = 0; i < TOTALCHANNELS; i++)
    {   // assert(!WAVHandle[i]);
        guestplaying[i] = FALSE;
        if (memmapinfo[memmap].channelused[i] == 1)
        {   sprintf((char*) tempstring, "!%d.wav", i);
            if ((WAVHandle[i] = fopen((const char*) tempstring, "wb")))
            {   ulong_to_le(&wav_header[24], samplerate);
                ulong_to_le(&wav_header[28], samplerate * bitrate / 8);
                uword_to_le(&wav_header[32], (UWORD) (bitrate / 8));
                uword_to_le(&wav_header[34], (UWORD) bitrate);
                fwrite(wav_header, 44, 1, WAVHandle[i]);
                wavoffset[i] = 0;
                wavcycle[i] = cycles_2650;
            } else
            {   for (j = 0; j < TOTALCHANNELS; j++)
                {   if (WAVHandle[j])
                    {   fclose(WAVHandle[j]);
                        WAVHandle[j] = NULL;
                }   }
                say("Can't open temporary output file!");
                return;
}   }   }   }

EXPORT void sound_stoprecording(void)
{   int   i;
    TEXT  tempstring[7 + 1];
    ULONG smallest;
#ifdef KEEPTEMPFILES
    FLAG  truncated;
#ifdef WIN32
    int  fh;
#endif
#ifdef AMIGA
    BPTR fh;
#endif
#endif

    smallest = (ULONG) -1;
    for (i = 0; i < TOTALCHANNELS; i++)
    {   if (WAVHandle[i])
        {   write_wav(i, TRUE);
            if (wavoffset[i] & 1)
            {
#ifdef KEEPTEMPFILES
                truncated = TRUE;
#endif
                wavoffset[i]--; // so we don't have to worry about pad bytes
            }
#ifdef KEEPTEMPFILES
            else
            {
                truncated = FALSE;
            }
#endif

            ulong_to_le(&wav_header[ 4], 36 + wavoffset[i]);
            DISCARD fseek(WAVHandle[i],  4, SEEK_SET);
            fwrite(&wav_header[ 4], 4, 1, WAVHandle[i]);

            // we already did 24..35 at the start

            ulong_to_le(&wav_header[40],      wavoffset[i]);
            DISCARD fseek(WAVHandle[i], 40, SEEK_SET);
            fwrite(&wav_header[40], 4, 1, WAVHandle[i]);

            fclose(WAVHandle[i]);
            WAVHandle[i] = NULL;

#ifdef KEEPTEMPFILES
            if (truncated)
            {   // The emulator can deal with odd-sized WAVs but they aren't conformant to spec, so we truncate them.
                sprintf(tempstring, "!%d.wav", i);
#ifdef WIN32
                if ((fh = _open(tempstring, _O_RDWR | _O_CREAT, _S_IREAD | _S_IWRITE)) != -1)
                {   DISCARD _chsize(fh, 44 + wavoffset[i]);
                    _close(fh);
                }
#endif
#ifdef AMIGA
                    if ((fh = Open(tempstring, MODE_OLDFILE)))
                {   DISCARD SetFileSize(fh, 44 + wavoffset[i], OFFSET_BEGINNING);
                    Close(fh);
                }
#endif
            }
#endif

            if (wavoffset[i] < smallest)
            {   smallest = wavoffset[i];
    }   }   }

    combine(KIND_8SVX, smallest);
    combine(KIND_AIFF, smallest);
    combine(KIND_WAV,  smallest);

#ifndef KEEPTEMPFILES
    for (i = 0; i < TOTALCHANNELS; i++)
    {   if (memmapinfo[memmap].channelused[i] == 1)
        {   sprintf((char*) tempstring, "!%d.wav", i);
            DISCARD remove((const char*) tempstring);
    }   }
#endif
}

MODULE void combine(int kind, ULONG smallest)
{   TRANSIENT FILE  *CombinedHandle,
                    *SingleHandle;
    TRANSIENT UBYTE *CombinedUBYTEBuffer,
                    *SingleUBYTEBuffer[TOTALCHANNELS];
    TRANSIENT SWORD *SingleSWORDBuffer[TOTALCHANNELS],
                    *CombinedSWORDBuffer;
    TRANSIENT SBYTE *CombinedSBYTEBuffer;
    TRANSIENT int    combined;
    TRANSIENT ULONG  i, j;
    PERSIST   TEXT   tempstring[80 + 1];

    if
    (   smallest == 0
     || (kind == KIND_8SVX && !esvxfile)
     || (kind == KIND_AIFF && !aifffile)
     || (kind == KIND_WAV  && !wavfile )
    )
    {   return;
    }

    for (i = 0; i < TOTALCHANNELS; i++)
    {   SingleUBYTEBuffer[i] = NULL;
    }

    for (i = 0; i < TOTALCHANNELS; i++)
    {   if (memmapinfo[memmap].channelused[i] == 1)
        {   sprintf((char*) tempstring, "!%d.wav", (int) i);
            if ((SingleHandle = fopen((const char*) tempstring, "rb")))
            {   DISCARD fseek(SingleHandle, 44, SEEK_SET);
                SingleUBYTEBuffer[i] = malloc(smallest); // should check return code!
                SingleSWORDBuffer[i] = (SWORD*) SingleUBYTEBuffer[i];
                if (fread(SingleUBYTEBuffer[i], (size_t) smallest, 1, SingleHandle) != 1)
                {   fclose(SingleHandle);
                    sprintf((char*) tempstring, "Can't read %d bytes from temporary file \"!%d.wav\"!", (int) smallest, (int) i);
                    say((STRPTR) tempstring);
                    goto DONE;
                }
                fclose(SingleHandle);
            } else
            {   sprintf((char*) tempstring, "Can't reopen temporary file \"!%d.wav\"!", (int) i);
                say((STRPTR) tempstring);
                goto DONE;
    }   }   }

    strcpy((char*) fn_save, (const char*) fn_game);
    if (bitrate == 16 && kind == KIND_8SVX)
    {   fixextension(filekind[KIND_16SV].extension, (STRPTR) fn_save, TRUE, "");
    } else
    {   fixextension(filekind[kind     ].extension, (STRPTR) fn_save, TRUE, "");
    }
    if (!(CombinedHandle = fopen((const char*) fn_save, "wb")))
    {   say("Can't open file for output!");
        goto DONE;
    }

    switch (kind)
    {
    case KIND_8SVX: // or 16SV
        ulong_to_be(&esvx_header[ 4], 40 + smallest);
        if (bitrate == 8)
        {   esvx_header[ 8] = '8';
            esvx_header[ 9] = 'S';
            esvx_header[10] = 'V';
            esvx_header[11] = 'X';
        } else
        {   esvx_header[ 8] = '1';
            esvx_header[ 9] = '6';
            esvx_header[10] = 'S';
            esvx_header[11] = 'V';
        }
        ulong_to_be(&esvx_header[20],      smallest);
        esvx_header[32] = samplerate / 256;
        esvx_header[33] = samplerate % 256;
        ulong_to_be(&esvx_header[44],      smallest);
        DISCARD fwrite(esvx_header, 48, 1, CombinedHandle);
    acase KIND_AIFF:
        ulong_to_be(&aiff_header[ 4], 46 + smallest);
        ulong_to_be(&aiff_header[22],      smallest);
        aiff_header[27] = bitrate;
        switch (samplerate)
        {
        case  11025: aiff_header[29] = 0xC;
        acase 22050: aiff_header[29] = 0xD;
        acase 44100: aiff_header[29] = 0xE;
        }
        ulong_to_be(&aiff_header[42],  8 + smallest);
        DISCARD fwrite(aiff_header, 54, 1, CombinedHandle);
    acase KIND_WAV:
        ulong_to_le(&wav_header[  4], 36 + smallest);
        ulong_to_le(&wav_header[ 24],      samplerate);
        ulong_to_le(&wav_header[ 28],      samplerate * bitrate / 8);
        uword_to_le(&wav_header[ 32], (UWORD) (bitrate / 8));
        uword_to_le(&wav_header[ 34], (UWORD) bitrate);
        ulong_to_le(&wav_header[ 40],      smallest);
        DISCARD fwrite(wav_header,  44, 1, CombinedHandle);
    }

    CombinedUBYTEBuffer = malloc(smallest); // should check return code
    CombinedSBYTEBuffer = (SBYTE*) CombinedUBYTEBuffer;
    CombinedSWORDBuffer = (SWORD*) CombinedUBYTEBuffer;

    if (bitrate == 8)
    {   for (i = 0; i < smallest; i++)
        {   combined = 0;
            for (j = 0; j < TOTALCHANNELS; j++)
            {   if (memmapinfo[memmap].channelused[j] == 1)
                {   combined += SingleUBYTEBuffer[j][i] - 128;
            }   }
            if (combined < -128)
            {   combined = -128;
            } elif (combined > 127)
            {   combined = 127;
            }
            if (kind == KIND_WAV)
            {   CombinedUBYTEBuffer[i] = combined + 128;
            } else
            {   CombinedSBYTEBuffer[i] = combined;
    }   }   }
    else
    {   for (i = 0; i < smallest / 2; i++)
        {   combined = 0;
            for (j = 0; j < TOTALCHANNELS; j++)
            {   if (memmapinfo[memmap].channelused[j] == 1)
                {
#ifdef WIN32
                    combined += SingleSWORDBuffer[j][i];
#endif
#ifdef AMIGA
                    combined += (((SingleSWORDBuffer[j][i] & 0xFF00) >> 8)
                             |   ((SingleSWORDBuffer[j][i] & 0x00FF) << 8));
#endif
            }   }
            if (combined < -32768)
            {   combined = -32768;
            } elif (combined > 32767)
            {   combined = 32767;
            }
#ifdef WIN32
            if (kind == KIND_8SVX || kind == KIND_AIFF) // convert little-endian in RAM to big-endian for file output
#endif
#ifdef AMIGA
            if (kind != KIND_8SVX && kind != KIND_AIFF) // convert big-endian in RAM to little-endian for file output
#endif
            {   CombinedSWORDBuffer[i] = ((combined & 0xFF00) >> 8)
                                       | ((combined & 0x00FF) << 8);
            } else
            {   CombinedSWORDBuffer[i] = combined;
    }   }   }

    DISCARD fwrite(CombinedUBYTEBuffer, smallest, 1, CombinedHandle);
    fclose(CombinedHandle);
    free(CombinedUBYTEBuffer);

DONE:
    for (i = 0; i < TOTALCHANNELS; i++)
    {   if (SingleUBYTEBuffer[i])
        {   free(SingleUBYTEBuffer[i]);
            // SingleUBYTEBuffer[i] = NULL;
    }   }

#ifdef AMIGA
    writeicon(kind, (STRPTR) fn_game);
#endif
}

EXPORT void ulong_to_be(UBYTE* stringptr, ULONG value)
{   *(stringptr + 0) = (UBYTE)  (value / 16777216);
    *(stringptr + 1) = (UBYTE) ((value % 16777216) / 65536);
    *(stringptr + 2) = (UBYTE) ((value %    65536) /   256);
    *(stringptr + 3) = (UBYTE)  (value %      256);
}
EXPORT void ulong_to_le(UBYTE* stringptr, ULONG value)
{   *(stringptr + 3) = (UBYTE)  (value / 16777216);
    *(stringptr + 2) = (UBYTE) ((value % 16777216) / 65536);
    *(stringptr + 1) = (UBYTE) ((value %    65536) /   256);
    *(stringptr + 0) = (UBYTE)  (value %      256);
}
EXPORT void uword_to_le(UBYTE* stringptr, UWORD value)
{   *(stringptr + 1) = (UBYTE)  (value /      256);
    *(stringptr + 0) = (UBYTE)  (value %      256);
}

EXPORT void write_wav(int guestchan, FLAG silencing)
{   FAST ULONG  cps, // cycles per second
                loops,
                samples,
                since;
    FAST double cpsf,
                samples_d,
                secs;
    FAST int    i;

    if
    (   !WAVHandle[guestchan]
     || cycles_2650 <= wavcycle[guestchan]
     || (silencing && guestplaying[guestchan])
    )
    {   return;
    }

    since     =  cycles_2650 - wavcycle[guestchan];
    cpsf      =  ((region == REGION_NTSC) ? 60.0 : 50.0) * machines[machine].cpf;
    cps       =  (ULONG) cpsf;
    secs      =  (double) since;
    secs      /= (double) cps;
    samples_d =  secs * (double) samplerate;
    samples   =  (ULONG) samples_d;

    wavoffset[guestchan] += samples * (bitrate / 8);
    wavcycle[ guestchan] =  cycles_2650;

    if (samples <= 0)
    {   return;
    }

    if (silencing)
    {   write_silence(    guestchan, samples);
    } elif (guestchan >= GUESTCHANNELS)
    {   if (samples > samp[guestchan - GUESTCHANNELS].bodysize)
        {   write_samples(guestchan,           samp[guestchan - GUESTCHANNELS].bodysize); // we have played all of the sample
            write_silence(guestchan, samples - samp[guestchan - GUESTCHANNELS].bodysize); // and at least some silence afterwards
        } else
        {   write_samples(guestchan, samples);                                            // we have played some of the sample
    }   }
#ifdef WIN32
    elif (bitrate == 16)
    {   if (SoundLength[guestchan])
        {   loops = samples / SoundLength[guestchan];
            if (loops)
            {   for (i = 0; i < (int) loops; i++)
                {   fwrite(SoundBuffer[guestchan],            SoundLength[guestchan]  * 2, 1, WAVHandle[guestchan]);
            }   }
            if (samples % SoundLength[ guestchan])
            {   fwrite(    SoundBuffer[guestchan], (samples % SoundLength[guestchan]) * 2, 1, WAVHandle[guestchan]);
        }   }
        else
        {   fwrite(        SoundBuffer[guestchan],  samples                           * 2, 1, WAVHandle[guestchan]);
    }   }
#endif
    else
    {   // assert(bitrate == 8);
        if (SoundLength[guestchan])
        {   loops = samples / SoundLength[guestchan];
            if (loops)
            {   for (i = 0; i < (int) loops; i++)
                {   fwrite(SoundBuffer[guestchan],            SoundLength[guestchan]     , 1, WAVHandle[guestchan]);
            }   }
            if (samples % SoundLength[guestchan])
            {   fwrite(    SoundBuffer[guestchan],  samples % SoundLength[guestchan]     , 1, WAVHandle[guestchan]);
        }   }
        else
        {   fwrite(        SoundBuffer[guestchan],  samples                              , 1, WAVHandle[guestchan]);
}   }   }

#define NUMLOOPS        32 // was 8
#define ONETONECYCLE(x) (samplerate / ((x) * 2.0))
EXPORT void generate_tone(int guestchan, double hertz, int guestvolume)
{   FAST double cyclefloat,
                stopfloat;
    FAST int    innerstop,
                outerstop,
                i, j, k,
                oneframe,
                stopint;

    // assert(memmapinfo[memmap].channelused[guestchan]);

    set_volume(guestvolume);

    j = 0;

    if (memmap == MEMMAP_GALAXIA && guestchan == TONE_B1) // throbber
    {   // assert(region == REGION_PAL);
        oneframe = samplerate / 50;
        outerstop = 0;

        if (bitrate == 8)
        {   for (i = 1; i <= 12; i++)
            {   if (i <= 7) // 1.. 7 -> 220..244
                {   hertz = 216.0 + (i * 4.0);
                } else      // 8..12 -> 240..224
                {   hertz = 272.0 - (i * 4.0);
                }
                hertz *= TRANSPOSE;
                innerstop = (int) ONETONECYCLE(hertz);
                outerstop += oneframe;
                do
                {   for (k = 0; k < innerstop; k++)
                    {   SoundBuffer[guestchan][j++] = upvol_8;
                    }
                    for (k = 0; k < innerstop; k++)
                    {   SoundBuffer[guestchan][j++] = dnvol_8;
                }   }
                while (j < outerstop);
            }
            SoundLength[guestchan] = j;
        }
#ifdef WIN32
        else
        {   // assert(bitrate == 16);
            for (i = 0; i < 12; i++)
            {   if (i <= 6) // 0.. 6 -> 220..244
                {   hertz = 220.0 + (i * 4);
                } else      // 7..11 -> 240..224
                {   hertz = 268.0 - (i * 4);
                }
                hertz *= TRANSPOSE;
                innerstop = (int) ONETONECYCLE(hertz);
                outerstop += oneframe * 2;
                do
                {   for (k = 0; k < innerstop; k++)
                    {   SoundBuffer[guestchan][j++] = upvol_l;
                        SoundBuffer[guestchan][j++] = upvol_h;
                    }
                    for (k = 0; k < innerstop; k++)
                    {   SoundBuffer[guestchan][j++] = dnvol_l;
                        SoundBuffer[guestchan][j++] = dnvol_h;
                }   }
                while (j < outerstop);
            }
            SoundLength[guestchan] = j / 2; // in samples
        }
#endif

        return;
    } // implied else

    hertz      *= TRANSPOSE;
    cyclefloat =  ONETONECYCLE(hertz);
    k          =  0;
    stopfloat  =  cyclefloat;
    stopint    =  (int) stopfloat;

    if (bitrate == 8)
    {   for (i = 0; i < NUMLOOPS; i++)
        {   do
            {   SoundBuffer[guestchan][j++] = upvol_8;
            } while (j < stopint);
            stopfloat += cyclefloat;
            stopint   =  (int) stopfloat;
            do
            {   SoundBuffer[guestchan][j++] = dnvol_8;
            } while (j < stopint);
            stopfloat += cyclefloat;
            stopint   =  (int) stopfloat;
        }
        SoundLength[guestchan] = j;
    }
#ifdef WIN32
    else
    {   // assert(bitrate == 16);
        for (i = 0; i < NUMLOOPS; i++)
        {   do
            {   SoundBuffer[guestchan][j++] = upvol_l;
                SoundBuffer[guestchan][j++] = upvol_h;
                k++;
            } while (k < stopint);
            stopfloat += cyclefloat;
            stopint   =  (int) stopfloat;
            do
            {   SoundBuffer[guestchan][j++] = dnvol_l;
                SoundBuffer[guestchan][j++] = dnvol_h;
                k++;
            } while (k < stopint);
            stopfloat += cyclefloat;
            stopint   =  (int) stopfloat;
        }
        SoundLength[guestchan] = k; // in samples
    }
#endif

#ifdef DEBUGSOUND
    zprintf(TEXTPEN_VERBOSE, "Generated %d samples (%d msec) at %f Hz ($%02X) on frame %d.\n", SoundLength[guestchan], (SoundLength[guestchan] * 1000) / samplerate, hertz, memory[A_PITCH], frames);
#endif
}

EXPORT void generate_noise(int guestchan, double hertz, int guestvolume)
{   FAST    double cyclefloat,
                   stopfloat;
    FAST    int    i, j,
                   stopint;
    FAST    UBYTE  newbit,
                   outvol_8;
    PERSIST UWORD  noise = 0xF0; // this should maybe get reinitialized when the machine is reset? and loaded/saved from cos/cor files?
#ifdef WIN32
    FAST    UBYTE  outvol_h,
                   outvol_l;
#endif

    set_volume(guestvolume);

    cyclefloat  = samplerate / (hertz * 2.0);
    i           =
    j           = 0;
    stopfloat   = cyclefloat;
    stopint     = (int) stopfloat;

    if (bitrate == 8)
    {   do
        {   newbit   = ((noise & 1) ^ ((noise & 0x10) ? 1 : 0));
            if (noise & 1)
            {   outvol_8 = upvol_8;
            } else
            {   outvol_8 = dnvol_8;
            }
            noise >>= 1;
            if (newbit)
            {   noise |= 0x100;
            }
            do
            {   SoundBuffer[guestchan][i++] = outvol_8;
            } while (i < stopint);
            stopfloat += cyclefloat;
            stopint   =  (int) stopfloat;
        } while (i < samplerate);
        SoundLength[guestchan] = i; // in samples
    }
#ifdef WIN32
    else
    {   // assert(bitrate == 16);
        do
        {   newbit   = ((noise & 1) ^ ((noise & 0x10) ? 1 : 0));
            outvol_h = (noise & 1) ? upvol_h : dnvol_h;
            outvol_l = (noise & 1) ? upvol_l : dnvol_l;
            noise >>= 1;
            if (newbit)
            {   noise |= 0x100;
            }
            do
            {   SoundBuffer[guestchan][i++] = outvol_l;
                SoundBuffer[guestchan][i++] = outvol_h;
                j++;
            } while (j < stopint);
            stopfloat += cyclefloat;
            stopint   =  (int) stopfloat;
        } while (j < samplerate);
        SoundLength[guestchan] = j; // in samples
    }
#endif
}

EXPORT void create_papertape(int whichunit)
{   // assert(!PapertapeHandle[whichunit]);
    // assert(papertapemode[whichunit] == TAPEMODE_NONE);

    if (game)
    {   strcpy((char*) fn_tape[2 + whichunit], (const char*) file_game);
        /* if (machine == TWIN)
        {   if (whichunit == 0)
            {   strcat((char*) fn_tape[2], "-TTYR");
            } else
            {   strcat((char*) fn_tape[3], "-HSPT");
        }   } */
    } else
    {   fn_tape[2 + whichunit][0] = EOS;
    }
    fixextension(filekind[KIND_PAP].extension, (STRPTR) fn_tape[2 + whichunit], TRUE, ""); // so we have extension in ASL requester

    if (asl
    (   filekind[KIND_PAP].hail,
        filekind[KIND_PAP].pattern,
        TRUE,
        (STRPTR) path_tapes,
        (STRPTR) fn_tape[2 + whichunit], // this fortunately doesn't contain the path right now
        (STRPTR) fn_tape[2 + whichunit]
    ) != EXIT_SUCCESS)
    {   return;
    }
    fixextension(filekind[KIND_PAP].extension, (STRPTR) fn_tape[2 + whichunit], TRUE, ""); // so we have extension even if user overtypes it in ASL requester
#ifdef SETPATHS
    break_pathname((STRPTR) fn_tape[2 + whichunit], (STRPTR) path_tapes, NULL);
#endif

    if (!(PapertapeHandle[whichunit] = fopen((const char*) fn_tape[2 + whichunit], "wb+")))
    {   say("Can't create file!");
        return;
    }

    papertapelength[whichunit] =
    papertapewhere[ whichunit] = 0;
    papertapemode[  whichunit] = TAPEMODE_STOP;
    update_papertape(whichunit, FALSE);
}

EXPORT void create_tape(void)
{   // assert(!TapeHandle);
    // assert(tapemode == TAPEMODE_NONE);

    if (game)
    {   strcpy((char*) fn_tape[0], (const char*) file_game);
    } else
    {   fn_tape[0][0] = EOS;
    }
    fixextension(filekind[tapekind].extension, (STRPTR) fn_tape[0], TRUE, ""); // so we have extension in ASL requester

    if (asl
    (   filekind[tapekind].hail,
        filekind[tapekind].pattern,
        TRUE,
        (STRPTR) path_tapes,
        (STRPTR) fn_tape[0], // this fortunately doesn't contain the path right now
        (STRPTR) fn_tape[0]
    ) != EXIT_SUCCESS)
    {   return;
    }
    fixextension(filekind[tapekind].extension, (STRPTR) fn_tape[0], TRUE, ""); // so we have extension even if user overtypes it in ASL requester
#ifdef SETPATHS
    break_pathname((STRPTR) fn_tape[0], (STRPTR) path_tapes, NULL);
#endif

    if (!(TapeHandle = fopen((const char*) fn_tape[0], "wb+")))
    {   say("Can't create file!");
        return;
    }
    switch (tapekind)
    {
    case KIND_8SVX:
        headerlength = 48;
        ulong_to_be(&esvx_header[ 4], headerlength - 8);
        esvx_header[ 8] = '8';
        esvx_header[ 9] = 'S';
        esvx_header[10] = 'V';
        esvx_header[11] = 'X';
        ulong_to_be(&esvx_header[20], 0);
        esvx_header[32] = SAVE_HZ / 256;
        esvx_header[33] = SAVE_HZ % 256;
        ulong_to_be(&esvx_header[44], 0);
        DISCARD fwrite(esvx_header, headerlength, 1, TapeHandle); // should check return code
    acase KIND_AIFF:
        headerlength = 54;
        ulong_to_be(&aiff_header[ 4], headerlength - 8);
        ulong_to_be(&aiff_header[22], 0);
        aiff_header[27] = 8;
        aiff_header[29] = SAVE_HZ_SANE;
        ulong_to_be(&aiff_header[42], 0);
        DISCARD fwrite(aiff_header, headerlength, 1, TapeHandle); // should check return code
    acase KIND_WAV:
        headerlength = 44;
        ulong_to_le(&wav_header[ 4], headerlength - 8);
        ulong_to_le(&wav_header[24], SAVE_HZ);
        ulong_to_le(&wav_header[28], SAVE_HZ);
        wav_header[32] = 1;
        wav_header[34] = 8;
        ulong_to_le(&wav_header[40], 0);
        DISCARD fwrite(wav_header, headerlength, 1, TapeHandle); // should check return code
    }

    tape_hz        = SAVE_HZ;
    settapespeed();

    tapelength     =
    otl            = 0;
    samplewhere_f  = 0.0;
    samplewhere    = 0;
    tapemode       = TAPEMODE_STOP;
    update_tapedeck(FALSE);
}

EXPORT FLAG load_papertape(FLAG wantasl, int whichunit)
{   // assert(papertapemode[whichunit] == TAPEMODE_NONE);

    // asl() turns sound off and on for us
    if (wantasl)
    {   if (asl
        (   (STRPTR) LLL(
                MSG_HAIL_INSERTPAPERTAPE,
                "Insert Papertape"
            ),
#ifdef WIN32
            "All files (*.*)\0*.*\0",
#endif
#ifdef AMIGA
            "#?",
#endif
            FALSE,
            (STRPTR) path_tapes,    // starting directory
#ifdef WIN32
            fn_tape[2 + whichunit], // starting filename
#endif
#ifdef AMIGA
    #ifdef __amigaos4__
            (STRPTR) FilePart((CONST_STRPTR) fn_tape[2 + whichunit]), // starting filename
    #else
            (STRPTR) FilePart((const char* ) fn_tape[2 + whichunit]), // starting filename
    #endif
#endif
            (STRPTR) fn_tape[2 + whichunit] // where to store complete result pathname
        ) != EXIT_SUCCESS)
        {   return FALSE;
        }
#ifdef SETPATHS
        break_pathname((STRPTR) fn_tape[2 + whichunit], (STRPTR) path_tapes, NULL);
#endif
    }

    papertapelength[whichunit] = getsize((STRPTR) fn_tape[2 + whichunit]); // zero-length is OK
    if (papertapelength[whichunit] > PAPERTAPEMAX)
    {   say((STRPTR) LLL(MSG_ENGINE_TOOLARGE2, "File is too large!\n\n")); // we would prefer it without the newlines
        return FALSE;
    }
    if (!(PapertapeHandle[whichunit] = fopen((const char*) fn_tape[2 + whichunit], "rb+")))
    {   say("Can't open file for input!");
        return FALSE;
    }
    if (papertapelength[whichunit] >= 1)
    {   if (fread(PapertapeBuffer[whichunit], papertapelength[whichunit], 1, PapertapeHandle[whichunit]) != 1)
        {   DISCARD fclose(PapertapeHandle[whichunit]);
            PapertapeHandle[whichunit] = NULL;
            say("Can't read from file!");
            return FALSE;
        }
        fseek(PapertapeHandle[whichunit], 0, SEEK_SET); // go back to start
    }

    papertapemode[ whichunit] = TAPEMODE_STOP;
    papertapewhere[whichunit] = 0;
    update_papertape(whichunit, FALSE);
    return TRUE;
}

EXPORT FLAG load_tape(FLAG wantasl)
{   UBYTE TapeHeaderBuffer[58];

    // assert(tapemode == TAPEMODE_NONE);

    // asl() turns sound off and on for us
    if (wantasl)
    {   if (asl
        (   (STRPTR) LLL(
                MSG_HAIL_INSERTTAPE,
                "Insert Tape"
            ),
#ifdef WIN32
            // "All supported formats" is first so that it is the default
            "All supported formats (*.WAV, *.IFF, *.8SVX, *.8SV, *.AIFF, *.AIF)\0*.WAV;*.IFF;*.8SVX;*.8SV;*.AIFF;*.AIF\0" \
            "RIFF WAV (*.WAV)\0*.WAV\0" \
            "IFF 8SVX (*.IFF, *.8SVX, *.8SV)\0*.IFF;*.8SVX;*.8SV\0" \
            "IFF AIFF (*.IFF, *.AIFF, *.AIF)\0*.IFF;*.AIFF;*.AIF\0" \
            "All files (*.*)\0*.*\0",
#endif
#ifdef AMIGA
            "#?.(IFF|8SVX|8SV|AIFF|AIF|WAV)",
#endif
            FALSE,
            (STRPTR) path_tapes,  // starting directory
#ifdef WIN32
            fn_tape[0],           // starting filename
#endif
#ifdef AMIGA
    #ifdef __amigaos4__
            (STRPTR) FilePart((CONST_STRPTR) fn_tape[0]), // starting filename
    #else
            (STRPTR) FilePart((const char* ) fn_tape[0]), // starting filename
    #endif
#endif
            (STRPTR) fn_tape[0]   // where to store complete result pathname
        ) != EXIT_SUCCESS)
        {   return FALSE;
        }
#ifdef SETPATHS
        break_pathname((STRPTR) fn_tape[0], (STRPTR) path_tapes, NULL);
#endif
    }

    /* This makes more assumptions about the input file
       (eg. chunk order) than we would like. */

    if
    (   (!(tapelength = getsize((STRPTR) fn_tape[0])))
     || (!(TapeHandle = fopen((const char*) fn_tape[0], "rb+")))
    )
    {   say("Can't open file for input!");
        goto ABORT;
    }
    if (fread(TapeHeaderBuffer, 20, 1, TapeHandle) != 1)
    {   say("File is too short!");
        goto ABORT;
    }

    if
    (   TapeHeaderBuffer[ 0] == 'F'
     && TapeHeaderBuffer[ 1] == 'O'
     && TapeHeaderBuffer[ 2] == 'R'
     && TapeHeaderBuffer[ 3] == 'M'
     && TapeHeaderBuffer[ 8] == '8'
     && TapeHeaderBuffer[ 9] == 'S'
     && TapeHeaderBuffer[10] == 'V'
     && TapeHeaderBuffer[11] == 'X'
     && TapeHeaderBuffer[12] == 'V'
     && TapeHeaderBuffer[13] == 'H'
     && TapeHeaderBuffer[14] == 'D'
     && TapeHeaderBuffer[15] == 'R'
    )
    {   // we could verify that the length is correct

        headerlength = 48;
        if (fread(&TapeHeaderBuffer[20], headerlength - 20, 1, TapeHandle) != 1)
        {   say("File is too short!");
            goto ABORT;
        }

        /* if
        (   TapeHeaderBuffer[24] != 0
         || TapeHeaderBuffer[25] != 0
         || TapeHeaderBuffer[26] != 0
         || TapeHeaderBuffer[27] != 0
        )
        {   say("Number of repeated samples must be 0!");
            goto ABORT;
        }
        if
        (   TapeHeaderBuffer[28] != 0
         || TapeHeaderBuffer[29] != 0
         || TapeHeaderBuffer[30] != 0
         || TapeHeaderBuffer[31] != 0
        )
        {   say("Samples per cycle must be 0!");
            goto ABORT;
        } */
        if (TapeHeaderBuffer[34] != 1)
        {   say("Number of octaves must be 1!");
            goto ABORT;
        }
        if (TapeHeaderBuffer[35] != 0)
        {   say("Compression must be 0!");
            goto ABORT;
        }

        tape_hz  = (TapeHeaderBuffer[32] * 256)
                 +  TapeHeaderBuffer[33];
        tapekind = KIND_8SVX;
    } elif
    (   TapeHeaderBuffer[ 0] == 'F'
     && TapeHeaderBuffer[ 1] == 'O'
     && TapeHeaderBuffer[ 2] == 'R'
     && TapeHeaderBuffer[ 3] == 'M'
     && TapeHeaderBuffer[ 8] == 'A'
     && TapeHeaderBuffer[ 9] == 'I'
     && TapeHeaderBuffer[10] == 'F'
     && TapeHeaderBuffer[11] == 'F'
     && TapeHeaderBuffer[12] == 'C'
     && TapeHeaderBuffer[13] == 'O'
     && TapeHeaderBuffer[14] == 'M'
     && TapeHeaderBuffer[15] == 'M'
    )
    {   // we could verify that the length is correct

        headerlength = 54;
        if (fread(&TapeHeaderBuffer[20], headerlength - 20, 1, TapeHandle) != 1)
        {   say("File is too short!");
            goto ABORT;
        }

        if (TapeHeaderBuffer[20] != 0 || TapeHeaderBuffer[21] != 1)
        {   say("Number of channels must be 1!");
            goto ABORT;
        }
        if (TapeHeaderBuffer[26] != 0 || TapeHeaderBuffer[27] != 8)
        {   say("Bits per sample must be 8!");
            goto ABORT;
        }

        tape_hz  = extended2long(&TapeHeaderBuffer[28]);

        tapekind = KIND_AIFF;
    } elif
    (   TapeHeaderBuffer[ 0] == 'R'
     && TapeHeaderBuffer[ 1] == 'I'
     && TapeHeaderBuffer[ 2] == 'F'
     && TapeHeaderBuffer[ 3] == 'F'
     && TapeHeaderBuffer[ 8] == 'W'
     && TapeHeaderBuffer[ 9] == 'A'
     && TapeHeaderBuffer[10] == 'V'
     && TapeHeaderBuffer[11] == 'E'
     && TapeHeaderBuffer[12] == 'f'
     && TapeHeaderBuffer[13] == 'm'
     && TapeHeaderBuffer[14] == 't'
     && TapeHeaderBuffer[15] == ' '
    )
    {   // we could verify that the length is correct

        if (TapeHeaderBuffer[16] == 16)
        {   headerlength = 44;
        } elif (TapeHeaderBuffer[16] == 18) // we assume if it has an extended FMT chunk it will also have a FACT chunk
        {   headerlength = 58;
        } else
        {   say("Unsupported file format!");
            goto ABORT;
        }
        if (fread(&TapeHeaderBuffer[20], headerlength - 20, 1, TapeHandle) != 1)
        {   say("File is too short!");
            goto ABORT;
        }
        if
        (   headerlength == 58
         && (   TapeHeaderBuffer[38] != 'f'
             || TapeHeaderBuffer[39] != 'a'
             || TapeHeaderBuffer[40] != 'c'
             || TapeHeaderBuffer[41] != 't'
        )   )
        {   say("Unsupported file format!");
            goto ABORT;
        }
        if (TapeHeaderBuffer[20] != 1 || TapeHeaderBuffer[21] != 0)
        {   say("Compression must be 1!");
            goto ABORT;
        }
        if (TapeHeaderBuffer[22] != 1 || TapeHeaderBuffer[23] != 0)
        {   say("Number of channels must be 1!");
            goto ABORT;
        }
        if (TapeHeaderBuffer[34] != 8 || TapeHeaderBuffer[35] != 0)
        {   say("Bits per sample must be 8!");
            goto ABORT;
        }

        tape_hz  =  TapeHeaderBuffer[24]
                 + (TapeHeaderBuffer[25] *      256)
                 + (TapeHeaderBuffer[26] *    65536)
                 + (TapeHeaderBuffer[27] * 16777216);
        tapekind = KIND_WAV;
    } else
    {   say("Unsupported file format!");
        goto ABORT;
    }

#ifdef VERBOSE
    zprintf(TEXTPEN_VERBOSE, "Sample rate is %d Hz.\n", tape_hz);
#endif
    if (tape_hz < 11025 || tape_hz > 48000)
    {   say("Unsupported sample rate!");
        goto ABORT;
    }

    tapelength -= headerlength;
    settapespeed();

    samplewhere_f = 0.0;
    samplewhere   = 0;
    tapemode      = TAPEMODE_STOP;
    tapeframe     = 0;

    otl = tapelength;
    if (otl)
    {   if (!((TapeCopy = malloc(tapelength))))
        {   rq("Out of memory!");
        }
        DISCARD fread(TapeCopy, tapelength, 1, TapeHandle);
        fseek(TapeHandle, headerlength, SEEK_SET);
    }

    update_tapedeck(FALSE);
    return TRUE;

ABORT:
    if (TapeHandle)
    {   DISCARD fclose(TapeHandle);
        TapeHandle = NULL;
    }
    return FALSE;
}

EXPORT void tape_stop(void)
{   if (tapemode <= TAPEMODE_STOP)
    {   return;
    }

    if (tapemode == TAPEMODE_RECORD)
    {   // assert(TapeHandle);

        // "The Amiga sound hardware requires an even number of samples in each one-shot and repeat waveform." - IFF 8SVX specification.
        if (samplewhere == tapelength && tapelength % 2) // to pad to even length
        {   DISCARD fwrite(&tapebyte, 1, 1, TapeHandle);
            tapelength++;
            samplewhere++;
            samplewhere_f = (double) samplewhere;
        }

        if (TapeCopy)
        {   free(TapeCopy);
            TapeCopy = NULL;
        }
        otl = tapelength;
        if (tapelength)
        {   if (!((TapeCopy = malloc(tapelength))))
            {   rq("Out of memory!");
            }
            fseek(TapeHandle, headerlength, SEEK_SET);
            DISCARD fread(TapeCopy, tapelength, 1, TapeHandle);
            fseek(TapeHandle, headerlength + samplewhere, SEEK_SET);
    }   }

    tapemode = TAPEMODE_STOP;
    update_tapedeck(FALSE);
}

EXPORT void tape_eject(void)
{   UBYTE buffer[4];
    int   localsize;

    if (tapemode == TAPEMODE_NONE)
    {   return;
    }

    // assert(TapeHandle);

    tape_stop();

    localsize = tapelength + headerlength - 8;
    switch (tapekind)
    {
    case KIND_8SVX:
        ulong_to_be(buffer, localsize);
        DISCARD fseek(TapeHandle, 4, SEEK_SET);
        DISCARD fwrite(buffer, 4, 1, TapeHandle);

        ulong_to_be(buffer, tapelength);
        DISCARD fseek(TapeHandle, 20, SEEK_SET);
        DISCARD fwrite(buffer, 4, 1, TapeHandle);

        DISCARD fseek(TapeHandle, headerlength - 4, SEEK_SET);
        DISCARD fwrite(buffer, 4, 1, TapeHandle);
    acase KIND_AIFF:
        ulong_to_be(buffer, localsize);
        DISCARD fseek(TapeHandle, 4, SEEK_SET);
        DISCARD fwrite(buffer, 4, 1, TapeHandle);

        ulong_to_be(buffer, tapelength);
        DISCARD fseek(TapeHandle, 22, SEEK_SET);
        DISCARD fwrite(buffer, 4, 1, TapeHandle);

        ulong_to_be(buffer, tapelength + 8);
        DISCARD fseek(TapeHandle, 42, SEEK_SET);
        DISCARD fwrite(buffer, 4, 1, TapeHandle);
    acase KIND_WAV:
        ulong_to_le(buffer, localsize);
        DISCARD fseek(TapeHandle, 4, SEEK_SET);
        DISCARD fwrite(buffer, 4, 1, TapeHandle);

        ulong_to_le(buffer, tapelength);
        if (headerlength == 58)
        {   DISCARD fseek(TapeHandle, 46, SEEK_SET);
            DISCARD fwrite(buffer, 4, 1, TapeHandle);
        }

        DISCARD fseek(TapeHandle, headerlength - 4, SEEK_SET);
        DISCARD fwrite(buffer, 4, 1, TapeHandle);
    }

    DISCARD fclose(TapeHandle);
    TapeHandle = NULL;

    if (TapeCopy)
    {   free(TapeCopy);
        TapeCopy = NULL;
    }
    otl           =
    samplewhere   =
    tapelength    = 0;
    samplewhere_f = 0.0;

    tapemode      = TAPEMODE_NONE;
    update_tapedeck(TRUE);
}

EXPORT void tape_record(void)
{   // assert(tapemode == TAPEMODE_STOP);

    tapemode = TAPEMODE_RECORD;
    cycles_prev = cycles_2650;
    update_tapedeck(TRUE);
}

EXPORT void tape_play(void)
{   // assert(tapemode == TAPEMODE_STOP);

    tapemode     = TAPEMODE_PLAY;
    cycles_prev  =
    oldtapecycle = cycles_2650;
    oldtapebyte  = 0;
    update_tapedeck(TRUE);
}

EXPORT void papertape_stop(int whichunit)
{   if (papertapemode[whichunit] <= TAPEMODE_STOP)
    {   return;
    }

    papertapemode[whichunit] = TAPEMODE_STOP;
    update_papertape(whichunit, FALSE);
}

EXPORT void papertape_eject(int whichunit)
{   if (papertapemode[whichunit] == TAPEMODE_NONE)
    {   return;
    }

    // assert(PapertapeHandle[whichunit]);

    papertape_stop(whichunit);

    if (papertapelength[whichunit])
    {   DISCARD fwrite(PapertapeBuffer[whichunit], papertapelength[whichunit], 1, PapertapeHandle[whichunit]);
    }
    DISCARD fclose(PapertapeHandle[whichunit]);
    PapertapeHandle[whichunit] = NULL;

    papertapewhere[ whichunit] =
    papertapelength[whichunit] = 0;
    papertapemode[  whichunit] = TAPEMODE_NONE;
    update_papertape(whichunit, TRUE);
}

EXPORT void papertape_record(int whichunit)
{   // assert(papertapemode[whichunit] == TAPEMODE_STOP);

    papertapemode[whichunit] = TAPEMODE_RECORD;
    update_papertape(whichunit, TRUE);
}

EXPORT void papertape_play(int whichunit)
{   // assert(papertapemode[whichunit] == TAPEMODE_STOP);

    papertapemode[whichunit] = TAPEMODE_PLAY;
    update_papertape(whichunit, TRUE);
}

MODULE LONG extended2long(UBYTE* ex)
{   unsigned long   mantissa;
    short                   exponent;
    long                    sign;

    exponent = (ex[0] *      256)
             +  ex[1];
    // We only need 32 bits precision
    mantissa = (ex[2] * 16777216)
             + (ex[3] *    65536)
             + (ex[4] *      256)
             +  ex[5];

    // Is the mantissa positive or negative?
    if (exponent & (1 << 15))
        sign = -1;
    else
        sign =  1;

    // Unbias the exponent (strip the sign bit; the
    // exponent is 15 bits wide)
    exponent &= ~(1 << 15);
    exponent -= ((1 << 14) - 1);

    // If the exponent is negative, set the mantissa to zero.
    // We cannot represent integer values between 0 and 1.
    if (exponent < 0)
    {       mantissa = 0;
    } else
    {   // We used only the upper 32 bits of the mantissa,
        // which is what we have to make up for. Subtracting
        // 31 from the exponent is actually dividing by
        // 2^32.
        exponent -= 31;

        // If the exponent is not negative, then the value
        // the number represents will be larger than the
        // original 64 bits of the mantissa would hold.
        if (exponent > 0)
            mantissa =   0x7FFFFFFF; // == MAXINT
        else
            mantissa >>= -exponent;  // Keep the integer part of the number
    }

    return (sign * (long) mantissa);
}

/* 76543210 7654 3210 7654 3210 7654 3210
   ----3333 3332 2222 2211 1111 1000 0000 */
EXPORT int variablelength(ULONG input, UBYTE* output)
{   if (input <= 0x0000007F)
    {   output[0] = (UBYTE)    input;
        return 1;
    }
    if (input <= 0x00003FFF)
    {   output[0] = (UBYTE) (((input & 0x0003F80) >>  7) | 0x80);
        output[1] = (UBYTE)   (input & 0x000007F);
        return 2;
    }
    if (input <= 0x001FFFFF)
    {   output[0] = (UBYTE) (((input & 0x01FC000) >> 14) | 0x80);
        output[1] = (UBYTE) (((input & 0x0003F80) >>  7) | 0x80);
        output[2] = (UBYTE)   (input & 0x000007F);
        return 3;
    }
    if (input <= 0x0FFFFFFF)
    {   output[0] = (UBYTE) (((input & 0xFE00000) >> 21) | 0x80);
        output[1] = (UBYTE) (((input & 0x01FC000) >> 14) | 0x80);
        output[2] = (UBYTE) (((input & 0x0003F80) >>  7) | 0x80);
        output[3] = (UBYTE)   (input & 0x000007F);
        return 4;
    }
    return 0; // out of range
}

EXPORT void midi_startrecording(void)
{   TRANSIENT int i,
                  length;
PERSIST UBYTE MIDIHeader[14] =
{ 'M',  'T',  'h',  'd', // file header
   0 ,   0 ,    0,    6, // length
                0,    1, // format
                0,    0, // tracks (done later)
                0,    0, // speed (done later) (25 for PAL or 30 for NTSC)
}, TrackHeader[8] =
{ 'M',  'T',  'r',  'k', // track header
   0 ,   0 ,    0,    0, // length (done later)
};

    // assert(midifile);

    strcpy((char*) fn_save, (const char*) fn_game);
    fixextension(filekind[KIND_MIDI].extension, (STRPTR) fn_save, TRUE, "");
    length = strlen((const char*) fn_save);
    MIDIHeader[11] = 0;
    for (i = 0; i < GUESTCHANNELS; i++)
    {   if (memmapinfo[memmap].channelused[i])
        {   MIDIHeader[11]++;
    }   }
    MIDIHeader[13] = (region == REGION_NTSC) ? 30 : 25;
    if ((MIDIHandle = fopen((const char*) fn_save, "wb")))
    {   DISCARD fwrite(MIDIHeader, 14, 1, MIDIHandle);
    }

    for (i = 0; i < GUESTCHANNELS; i++)
    {   if (memmapinfo[memmap].channelused[i])
        {   sprintf((char*) &fn_save[length], "-%d", i);
            if ((MIDITrackHandle[i] = fopen((const char*) fn_save, "wb")))
            {   DISCARD fwrite(TrackHeader, 8, 1, MIDITrackHandle[i]);
            }
            mtrk[i] = 0;
            miditime[i] = frames;
            midiplaying[i] = FALSE;
}   }   }

EXPORT void finalize_midi(void)
{   UBYTE  channelsreallyused = 0,
           MIDIBuffer[4];
    ULONG  localsize;
    UBYTE* memptr;
    int    i,
           length;

    // assert(midifile);
    // assert(MIDIHandle);

    strcpy((char*) fn_save, (const char*) fn_game);
    fixextension(filekind[KIND_MIDI].extension, (STRPTR) fn_save, TRUE, "");
    length = strlen((const char*) fn_save);

    for (i = 0; i < GUESTCHANNELS; i++)
    {   if (memmapinfo[memmap].channelused[i])
        {   channelsreallyused++;
            // assert(MIDITrackHandle[i]);
            fseek(MIDITrackHandle[i], 4, SEEK_SET);
            writeulong(mtrk[i], MIDIBuffer);
            DISCARD fwrite(MIDIBuffer, 4, 1, MIDITrackHandle[i]);
            DISCARD fclose(MIDITrackHandle[i]);
            MIDITrackHandle[i] = NULL;
            sprintf((char*) &fn_save[length], "-%d", i);
            if (mtrk[i])
            {   localsize = getsize((STRPTR) fn_save);
                if ((MIDITrackHandle[i] = fopen((const char*) fn_save, "rb")))
                {   if ((memptr = malloc(localsize)))
                    {   if (fread(memptr, localsize, 1, MIDITrackHandle[i]) == 1)
                        {   fwrite(memptr, localsize, 1, MIDIHandle);
                        }
                        free(memptr);
                        // memptr = NULL;
                    }
                    DISCARD fclose(MIDITrackHandle[i]);
                    MIDITrackHandle[i] = NULL;
            }   }
            else
            {   channelsreallyused--;
            }
            remove((const char*) fn_save);
    }   }

    fseek(MIDIHandle, 11, SEEK_SET);
    fwrite(&channelsreallyused, 1, 1, MIDIHandle);
    DISCARD fclose(MIDIHandle);
    MIDIHandle = NULL;
}

EXPORT void smus_startrecording(void)
{   TRANSIENT int i,
                  length;
PERSIST UBYTE SMUSHeader[24] =
{ 'F',  'O',  'R',  'M',
   0 ,   0 ,   0,    0 , // length (done later)
  'S',  'M',  'U',  'S',
  'S',  'H',  'D',  'R',
   0 ,   0 ,   0 ,   4 , // length
   0 ,   0 ,             // tempo (done later)
             0x7F,       // volume (127/127)
                     0 , // number of tracks (done later)
}, TrackHeader[8] =
{ 'T',  'R',  'A',  'K', // track header
   0 ,   0 ,   0 ,   0 , // length (done later)
};

    // assert(smusfile);

    strcpy((char*) fn_save, (const char*) fn_game);
    fixextension(filekind[KIND_SMUS].extension, (STRPTR) fn_save, TRUE, "");
    length = strlen((const char*) fn_save);

    if (region == REGION_NTSC)
    {   SMUSHeader[20] = (450 * 128) / 256; // at 450 bpm, each tick (128th note) lasts for 4.16' msecs (1/4 NTSC frame)
        SMUSHeader[21] = (450 * 128) % 256;
    } else
    {   SMUSHeader[20] = (375 * 128) / 256; // at 375 bpm, each tick (128th note) lasts for 5     msecs (1/4 PAL  frame)
        SMUSHeader[21] = (375 * 128) % 256;
    }
    SMUSHeader[23] = 0;
    for (i = 0; i < GUESTCHANNELS; i++)
    {   if (memmapinfo[memmap].channelused[i])
        {   SMUSHeader[23]++;
    }   }
    if ((SMUSHandle = fopen((const char*) fn_save, "wb")))
    {   DISCARD fwrite(SMUSHeader, 24, 1, SMUSHandle);
    }

    for (i = 0; i < GUESTCHANNELS; i++)
    {   if (memmapinfo[memmap].channelused[i])
        {   sprintf((char*) &fn_save[length], "-%d", i);
            if ((SMUSTrackHandle[i] = fopen((const char*) fn_save, "wb")))
            {   DISCARD fwrite(TrackHeader, 8, 1, SMUSTrackHandle[i]);
            }
            strk[i]        = 0;
            smusplaying[i] = 0;
            smusvolume[i]  = 127;
            smustime[i]    = frames;
}   }   }

EXPORT void finalize_smus(void)
{   UBYTE  channelsreallyused = 0,
           SMUSBuffer[4];
    ULONG  localsize,
           totalsize;
    UBYTE* memptr;
    int    i,
           length;

    // assert(smusfile);
    // assert(SMUSHandle);

    for (i = 0; i < GUESTCHANNELS; i++)
    {   if (smusplaying[i] != 0)
        {   // assert(memmapinfo[memmap].channelused[i]);
            // write duration of old note/rest
            smusduration = (frames - smustime[i]) * 4;
            SMUSBuffer[0] = ticks2smus();
            if (smusduration && smusnote[i] != 0x80)
            {   SMUSBuffer[0] |= 0x40; // tied note
            }
            DISCARD fwrite(SMUSBuffer, 1, 1, SMUSTrackHandle[i]);
            strk[i]++;
#ifdef DEBUGSMUS
            zprintf(TEXTPEN_VERBOSE, "Frame %d, channel #%2d: duration       $%02X\n", frames, i, SMUSBuffer[0]);
#endif
            while (smusduration)
            {   SMUSBuffer[0] = smusnote[i];
                SMUSBuffer[1] = ticks2smus();
                if (smusduration && smusnote[i] != 0x80)
                {   SMUSBuffer[1] |= 0x40; // tied note
                }
                DISCARD fwrite(SMUSBuffer, 2, 1, SMUSTrackHandle[i]);
                strk[i] += 2;
#ifdef DEBUGSMUS
                zprintf(TEXTPEN_VERBOSE, "Frame %d, channel #%2d: note pitch $%02X\n", frames, i, SMUSBuffer[0]);
                zprintf(TEXTPEN_VERBOSE, "Frame %d, channel #%2d: duration       $%02X\n", frames, i, SMUSBuffer[1]);
#endif
    }   }   }

    strcpy((char*) fn_save, (const char*) fn_game);
    fixextension(filekind[KIND_SMUS].extension, (STRPTR) fn_save, TRUE, "");
    length = strlen((const char*) fn_save);

    totalsize = 24;
    for (i = 0; i < GUESTCHANNELS; i++)
    {   if (memmapinfo[memmap].channelused[i])
        {   channelsreallyused++;
            // assert(SMUSTrackHandle[i]);
            fseek(SMUSTrackHandle[i], 4, SEEK_SET);
            writeulong(strk[i], SMUSBuffer);
            DISCARD fwrite(SMUSBuffer, 4, 1, SMUSTrackHandle[i]);
            DISCARD fclose(SMUSTrackHandle[i]);
            SMUSTrackHandle[i] = NULL;
            sprintf((char*) &fn_save[length], "-%d", i);
            if (strk[i] && smusplaying[i] == 2)
            {   localsize = getsize((STRPTR) fn_save);
                // assert(localsize % 2 == 0);
                if ((SMUSTrackHandle[i] = fopen((const char*) fn_save, "rb")))
                {   if ((memptr = malloc(localsize)))
                    {   if (fread(memptr, localsize, 1, SMUSTrackHandle[i]) == 1)
                        {   fwrite(memptr, localsize, 1, SMUSHandle);
                        }
                        free(memptr);
                        // memptr = NULL;
                    }
                    DISCARD fclose(SMUSTrackHandle[i]);
                    SMUSTrackHandle[i] = NULL;
                }
                totalsize += localsize;
            } else
            {   channelsreallyused--;
            }
            remove((const char*) fn_save);
    }   }

    fseek(SMUSHandle, 4, SEEK_SET);
    writeulong(totalsize - 8, SMUSBuffer);
    fwrite(SMUSBuffer, 4, 1, SMUSHandle);
    fseek(SMUSHandle, 23, SEEK_SET);
    fwrite(&channelsreallyused, 1, 1, SMUSHandle);
    DISCARD fclose(SMUSHandle);
    SMUSHandle = NULL;
}

EXPORT UBYTE ticks2smus(void)
{   // Those marked "*" are the only ones supported by Aegis Sonix.
    // Dotted whole notes (192 ticks) are presumably possible but we don't use them.
    if (smusduration >= 128) { smusduration -= 128; return  0; } //        whole     note* (128 ticks)
    if (smusduration >=  96) { smusduration -=  96; return  9; } // dotted half      note* ( 96 ticks)
    if (smusduration >=  64) { smusduration -=  64; return  1; } //        half      note* ( 64 ticks)
    if (smusduration >=  48) { smusduration -=  48; return 10; } // dotted quarter   note* ( 48 ticks)
    if (smusduration >=  32) { smusduration -=  32; return  2; } //        quarter   note* ( 32 ticks)
    if (smusduration >=  24) { smusduration -=  24; return 11; } // dotted eighth    note* ( 24 ticks)
    if (smusduration >=  16) { smusduration -=  16; return  3; } //        eighth    note* ( 16 ticks)
    if (smusduration >=  12) { smusduration -=  12; return 12; } // dotted sixteenth note  ( 12 ticks)
    if (smusduration >=   8) { smusduration -=   8; return  4; } //        sixteenth note* (  8 ticks)
    if (smusduration >=   6) { smusduration -=   6; return 13; } // dotted 32nd      note  (  6 ticks)
    if (smusduration >=   4) { smusduration -=   4; return  5; } //        32nd      note  (  4 ticks)
    if (smusduration >=   3) { smusduration -=   3; return 14; } // dotted 32nd      note  (  3 ticks)
    if (smusduration >=   2) { smusduration -=   2; return  6; } //        64th      note  (  2 ticks)
    if (smusduration >=   1) { smusduration--;      return  7; } //        128th     note  (  1 tick )
    return 7; // should never happen
}

EXPORT void printer_copygfx(FLAG clip, int kind, FLAG wantasl, int whichprinter)
{   TRANSIENT TEXT   suffix[1 + 10 + 1];
    TRANSIENT UBYTE  t                 = 0; // initialized to avoid a spurious SAS/C warning
    TRANSIENT int    headersize,
                     i,
                     iffsize,
                     linewidth         = 0, // initialized to avoid a spurious SAS/C warning
                     plane,
                     planeoffset,
                     planes            = 0, // initialized to avoid a spurious SAS/C warning
                     ssbpl             = 0, // initialized to avoid a spurious SAS/C warning
                     x, y;
    TRANSIENT FILE*  TheLocalHandle    = NULL    ; // initialized to avoid a spurious SAS/C warning
    TRANSIENT UBYTE* TheBody        /* = NULL */ ;
PERSIST UBYTE prtbmp_header[78] =
{   // bytes  0..  1 are "BM"
    // bytes  2..  5 is  the size of the entire file (little-endian)
    // bytes  6..  9 are reserved
    // bytes 10.. 13 is  the offset to the bits (little-endian)
    // byte  14.. 17 is  a constant (structure size) (little-endian)
    // bytes 18.. 21 is  the width of the bitmap (little-endian)
    // bytes 22.. 25 is  the height of the bitmap (little-endian)
    // bytes 26.. 27 is  a constant
    // bytes 28.. 29 is  the bits per pixel (ie. 1 or 4)
    // bytes 30.. 33 is  the compression
    // bytes 34.. 37 is  the size (zero is allowed)
    // bytes 38.. 41 is  X-pixels per metre
    // bytes 42.. 45 is  Y-pixels per metre
    // bytes 46.. 49 is  the number of colours used (ie. 6)
    // bytes 50.. 53 is  the number of important colours
    // bytes 54.. 77 are colours 0..5 (4 bytes each)
    0x42, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // "BMzzzz00"
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, // "00yyyy*0"
    0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, // "00wwwwhh"
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, // "hh1080cc"
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x0B, // "ccssssxx"
    0x00, 0x00, 0x13, 0x0B, 0x00, 0x00, 0x06, 0x00, // "xxyyyycc"
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,             // "cciiii"
    // B     G     R
    0x00, 0x00, 0x00, 0x00, // colour  0
    0x00, 0x00, 0x00, 0x00, // colour  1
    0x00, 0x00, 0x00, 0x00, // colour  2
    0x00, 0x00, 0x00, 0x00, // colour  3
    0x00, 0x00, 0x00, 0x00, // colour  4
    0x00, 0x00, 0x00, 0x00, // colour  5
}, prtiff_header[72] =
{    'F',  'O',  'R',  'M', 0xFF, 0xFF, 0xFF, 0xFF, // "FORMxxxx"
     'I',  'L',  'B',  'M',  'B',  'M',  'H',  'D', // "ILBMBMHD"
    0x00, 0x00, 0x00, 0x14, 0xFF, 0xFF, 0xFF, 0xFF, // "xxxx1234"
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // "56789012"
    0x00, 0x00, 0x01, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, // "34567890"
     'C',  'M',  'A',  'P', 0x00, 0x00, 0x00, 0x00, // "CMAPxxxx"
    // R     G     B
    0x00, 0x00, 0x00, // colour  0
    0x00, 0x00, 0x00, // colour  1
    0x00, 0x00, 0x00, // colour  2
    0x00, 0x00, 0x00, // colour  3
    0x00, 0x00, 0x00, // colour  4
    0x00, 0x00, 0x00, // colour  5
    0x00, 0x00, 0x00, // colour  6
    0x00, 0x00, 0x00, // colour  7
}, body_header[8] =
{    'B',  'O',  'D',  'Y', 0x00, 0xFF, 0xFF, 0xFF, // "BODYxxxx"
};

    if (kind == KIND_BMP)
    {   prtbmp_header[    54] = getblue(colourset, GREY1    ); prtbmp_header[55] = getgreen(colourset, GREY1    ); prtbmp_header[56] = getred(colourset, GREY1); // paper or sprockets/perforations
        prtbmp_header[    58] = getblue(colourset, BLACK    ); prtbmp_header[59] = getgreen(colourset, BLACK    ); prtbmp_header[60] = getred(colourset, BLACK); // ink
        if (machine == TWIN)
        {   headersize        = 78; // 6 colours
            prtbmp_header[28] =  4; // bits per pixel
            prtbmp_header[46] =  6; // colours used
            linewidth         = printerwidth_full / 2;
            if (printerwidth_full % 2)
            {   linewidth++;
            }
            linewidth         = ROUNDTO4(linewidth);
            prtbmp_header[62] = getblue(colourset, WHITE    ); prtbmp_header[63] = getgreen(colourset, WHITE    ); prtbmp_header[64] = getred(colourset, WHITE); // paper
            prtbmp_header[66] = getblue(colourset, RED      ); prtbmp_header[67] = getgreen(colourset, RED      ); prtbmp_header[68] = getred(colourset, RED);
            prtbmp_header[70] = getblue(colourset, DARKGREEN); prtbmp_header[71] = getgreen(colourset, DARKGREEN); prtbmp_header[72] = getred(colourset, DARKGREEN);
            prtbmp_header[74] = getblue(colourset, BLUE     ); prtbmp_header[75] = getgreen(colourset, BLUE     ); prtbmp_header[76] = getred(colourset, BLUE);
        } else
        {   headersize        = 62; // 2 colours
            prtbmp_header[28] =  1; // bits per pixel
            prtbmp_header[46] =  2; // colours used
            linewidth         = printerwidth_full / 8;
            if (printerwidth_full % 8)
            {   linewidth++;
            }
            linewidth         = ROUNDTO4(linewidth);
        }
        prtbmp_header[    10] = (UBYTE) headersize;
        ulong_to_le(&prtbmp_header[2], headersize + (linewidth * printerheight_full));
        prtbmp_header[18] = (UBYTE) (printerwidth_full  % 256);
        prtbmp_header[19] = (UBYTE) (printerwidth_full  / 256);
        prtbmp_header[22] = (UBYTE) (printerheight_full % 256);
        prtbmp_header[23] = (UBYTE) (printerheight_full / 256);
    } else
    {   if (kind == KIND_ILBM)
        {   prtiff_header[ 8] = 'I';
            prtiff_header[ 9] = 'L';
            body_header[   0] = 'B';
            body_header[   1] = 'O';
            body_header[   2] = 'D';
            body_header[   3] = 'Y';
        } else
        {   // assert(kind == KIND_ACBM);
            prtiff_header[ 8] = 'A';
            prtiff_header[ 9] = 'C';
            body_header[   0] = 'A';
            body_header[   1] = 'B';
            body_header[   2] = 'I';
            body_header[   3] = 'T';
        }
        prtiff_header[    48] = getred(colourset, GREY1); prtiff_header[49] = getgreen(colourset, GREY1); prtiff_header[50] = getblue(colourset, GREY1);
        prtiff_header[    51] = getred(colourset, BLACK); prtiff_header[52] = getgreen(colourset, BLACK); prtiff_header[53] = getblue(colourset, BLACK);
        if (machine == TWIN)
        {   planes     =  3;
            headersize = 72; // 8 colours
            ssbpl = printerwidth_full / 8;
            if (printerwidth_full % 8)
            {   ssbpl++;
            }
            if (ssbpl % 2)
            {   ssbpl++;
            }
            prtiff_header[47] = 8 * 3; // size of CMAP chunk
            prtiff_header[54] = getred(colourset, WHITE    ); prtiff_header[55] = getgreen(colourset, WHITE    ); prtiff_header[56] = getblue(colourset, WHITE);
            prtiff_header[57] = getred(colourset, RED      ); prtiff_header[58] = getgreen(colourset, RED      ); prtiff_header[59] = getblue(colourset, RED  );
            prtiff_header[60] = getred(colourset, DARKGREEN); prtiff_header[61] = getgreen(colourset, DARKGREEN); prtiff_header[62] = getblue(colourset, DARKGREEN);
            prtiff_header[63] = getred(colourset, BLUE     ); prtiff_header[64] = getgreen(colourset, BLUE     ); prtiff_header[65] = getblue(colourset, BLUE );
        } else
        {   planes     =  1;
            headersize = 54; // 2 colours
            ssbpl = printerwidth_full / 8;
            if (printerwidth_full % 8)
            {   ssbpl++;
            }
            if (ssbpl % 2)
            {   ssbpl++;
            }
            prtiff_header[47] = 2 * 3; // size of CMAP chunk
        }
        prtiff_header[28] = planes;
        prtiff_header[20] =
        prtiff_header[36] = (UBYTE) (printerwidth_full        / 256);
        prtiff_header[21] =
        prtiff_header[37] = (UBYTE) (printerwidth_full        % 256);
        prtiff_header[22] =
        prtiff_header[38] = (UBYTE) (printerheight_full       / 256);
        prtiff_header[23] =
        prtiff_header[39] = (UBYTE) (printerheight_full       % 256);
        prtiff_header[30] = 0; // no compression
        iffsize = headersize + (ssbpl * planes * printerheight_full);
        ulong_to_be(&prtiff_header[ 4], iffsize);
        iffsize =               ssbpl * planes * printerheight_full ;
        ulong_to_be(&body_header[   4], iffsize);
    }

    if (clip)
    {   clip_open(TRUE);
        if (kind == KIND_BMP)
        {   clip_write((STRPTR) prtbmp_header, headersize, CF_DIB);
        } else
        {   clip_write((STRPTR) prtiff_header,     headersize, 0);
            clip_write((STRPTR) body_header,                8, 0);
    }   }
    else
    {   if (wantasl)
        {   if (game)
            {   strcpy((char*) fn_screenshot, (const char*) file_game);
            } else
            {   fn_screenshot[0] = EOS;
            }
            fixextension(filekind[kind].extension, (STRPTR) fn_screenshot, TRUE, ""); // so we have extension in ASL requester
            if (asl
            (   filekind[kind].hail,
                filekind[kind].pattern,
                TRUE,
                (STRPTR) path_screenshots,
                (STRPTR) fn_screenshot, // this fortunately doesn't contain the path right now
                (STRPTR) fn_screenshot
            ) != EXIT_SUCCESS)
            {   return;
            }
            fixextension(filekind[kind].extension, (STRPTR) fn_screenshot, TRUE, ""); // so we have extension even if user overtypes it in ASL requester
#ifdef SETPATHS
            break_pathname((STRPTR) fn_screenshot, (STRPTR) path_screenshots, NULL);
#endif
        } else
        {   if (game)
            {   strcpy(fn_screenshot, file_game);
            } else
            {   if (machine == TWIN)
                {   if (whichprinter == 0) strcpy(fn_screenshot, "LPT1");
                    else                   strcpy(fn_screenshot, "LPT2");
                } else
                {                          strcpy(fn_screenshot, "Printer");
            }   }
            suffix[0] = '-';
            sprintf(&suffix[1], "%d", printer[whichprinter].page);
            fixextension(filekind[kind].extension, fn_screenshot, TRUE, suffix);
            cd_screenshots();
        }

        if (!(TheLocalHandle = fopen((const char*) fn_screenshot, "wb")))
        {   beep();
            if (!wantasl)
            {   cd_progdir();
            }
            return;
        }

        if (kind == KIND_BMP)
        {   DISCARD fwrite(prtbmp_header, (size_t) headersize, 1, TheLocalHandle);
        } else
        {   DISCARD fwrite(prtiff_header, (size_t) headersize, 1, TheLocalHandle);
            DISCARD fwrite(body_header,   (size_t)          8, 1, TheLocalHandle);
    }   }

    if (kind == KIND_BMP)
    {   TheBody = calloc((unsigned int) (linewidth * printerheight_full), 1); // should check return code!
        if (machine == TWIN)
        {   for (y = printerheight_full - 1; y >= 0; y--)
            {   i = ((printerheight_full - 1 - y) * linewidth);
                for (x = 0; x < printerwidth_full; x++)
                {   switch (printer[whichprinter].scrn[y][x])
                    {
                    case  GREY1:     t = 0;
                    acase BLACK:     t = 1;
                    acase WHITE:     t = 2;
                    acase RED:       t = 3;
                    acase DARKGREEN: t = 4;
                    acase BLUE:      t = 5;
                    }
                    if (!(x & 1))
                    {   TheBody[i  ] =  t << 4;
                    } else
                    {   TheBody[i++] |= t;
        }   }   }   }
        else // eg. PIPBUG, BINBUG, CD2650
        {   for (y = printerheight_full - 1; y >= 0; y--)
            {   i = ((printerheight_full - 1 - y) * linewidth);
                for (x = 0; x < printerwidth_full; x++)
                {   if (printer[whichprinter].scrn[y][x] == BLACK)
                    {   TheBody[i + (x / 8)] |= (0x80 >> (x % 8));
        }   }   }   }

        if (clip)
        {   clip_write((STRPTR) TheBody, linewidth * printerheight_full, CF_DIB);
        } else
        {   DISCARD fwrite(TheBody, (size_t) (linewidth * printerheight_full), 1, TheLocalHandle);
    }   }
    else
    {   TheBody = calloc((size_t) (planes * ssbpl * printerheight_full), 1); // should check return code!
        if (machine == TWIN)
        {   if (kind == KIND_ACBM)
            {   for (plane = 0; plane < planes; plane++)
                {   planeoffset = plane * ssbpl * printerheight_full;
                    for (y = 0; y < printerheight_full; y++)
                    {   for (x = 0; x < printerwidth_full; x++)
                        {   switch (printer[whichprinter].scrn[y][x])
                            {
                            case  GREY1:     t = 0;
                            acase BLACK:     t = 1;
                            acase WHITE:     t = 2;
                            acase RED:       t = 3;
                            acase DARKGREEN: t = 4;
                            acase BLUE:      t = 5;
                            }
                            if (t & (1 << plane))
                            {   TheBody[planeoffset + (y * ssbpl) + (x / 8)] |= 0x80 >> (x % 8);
            }   }   }   }   }
            else
            {   for (y = 0; y < printerheight_full; y++)
                {   for (plane = 0; plane < planes; plane++)
                    {   for (x = 0; x < printerwidth_full; x++)
                        {   switch (printer[whichprinter].scrn[y][x])
                            {
                            case  GREY1:     t = 0;
                            acase BLACK:     t = 1;
                            acase WHITE:     t = 2;
                            acase RED:       t = 3;
                            acase DARKGREEN: t = 4;
                            acase BLUE:      t = 5;
                            }
                            if (t & (1 << plane))
                            {   TheBody[(y * ssbpl * planes) + (plane * ssbpl) + (x / 8)] |= 0x80 >> (x % 8);
        }   }   }   }   }   }
        else
        {   for (y = 0; y < printerheight_full; y++)
            {   for (x = 0; x < printerwidth_full; x++)
                {   if (printer[whichprinter].scrn[y][x] == BLACK)
                    {   TheBody[(y * ssbpl) + (x / 8)] |= 0x80 >> (x % 8);
        }   }   }   }

        if (clip)
        {   clip_write((STRPTR) TheBody,          ssbpl * planes * printerheight_full, 0);
        } else
        {   DISCARD fwrite(     TheBody, (size_t) ssbpl * planes * printerheight_full, 1, TheLocalHandle);
    }   }

    free(TheBody);
    // TheBody = NULL;

    if (clip)
    {   clip_close();
    } else
    {   DISCARD fclose(TheLocalHandle);
        // TheLocalHandle = NULL;

#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Saved page %d of printer %d graphics as \"%s\".\n", printer[whichprinter].page, whichprinter, fn_screenshot);
#endif

        if (!wantasl)
        {   cd_progdir();
}   }   }

MODULE void write_samples(int guestchan, int samples)
{   FAST int   i;
    FAST UBYTE byteval[4];
#ifdef WIN32
    FAST UWORD wordval[4];
#endif

    switch (bitrate)
    {
    case 8:
        switch (samplerate)
        {
        case 11025:
#ifdef WIN32
            fwrite(samp[guestchan - GUESTCHANNELS].bodybase, samples, 1, WAVHandle[guestchan]);
#endif
#ifdef AMIGA
            for (i = 0; i < samples; i++)
            {   byteval[0] = samp[guestchan - GUESTCHANNELS].bodybase[i] ^ 0x80; // SBYTE in .8svx file -> UBYTE in .wav file
                fwrite(byteval, 1, 1, WAVHandle[guestchan]);
            }
#endif
        acase 22050:
            for (i = 0; i < samples; i++)
            {   byteval[0] = samp[guestchan - GUESTCHANNELS].bodybase[i];
#ifdef AMIGA
                byteval[0] ^= 0x80; // SBYTE in .8svx file -> UBYTE in .wav file
#endif
                byteval[1] = byteval[0];
                fwrite(byteval, 2, 1, WAVHandle[guestchan]);
            }
        acase 44100:
            for (i = 0; i < samples; i++)
            {   byteval[0] = samp[guestchan - GUESTCHANNELS].bodybase[i];
                byteval[1] =
                byteval[2] =
                byteval[3] = byteval[0];
                fwrite(byteval, 4, 1, WAVHandle[guestchan]);
        }   }
#ifdef WIN32 // 16-bit sound is not supported on AmiArcadia
    acase 16:
        // This code is little-endian only.
        switch (samplerate)
        {
        case 11025:
            for (i = 0; i < samples; i++)
            {   byteval[0] = samp[guestchan - GUESTCHANNELS].bodybase[i];
                wordval[0] = ((byteval[0] << 8) | byteval[0]) ^ 0x80; // 0..255 -> -32768..32767
                fwrite(wordval, 2, 1, WAVHandle[guestchan]);
            }
        acase 22050:
            for (i = 0; i < samples; i++)
            {   byteval[0] = samp[guestchan - GUESTCHANNELS].bodybase[i];
                wordval[0] = ((byteval[0] << 8) | byteval[0]) ^ 0x80; // 0..255 -> -32768..32767
                wordval[1] = wordval[0];
                fwrite(wordval, 4, 1, WAVHandle[guestchan]);
            }
        acase 44100:
            for (i = 0; i < samples; i++)
            {   byteval[0] = samp[guestchan - GUESTCHANNELS].bodybase[i];
                wordval[0] = ((byteval[0] << 8) | byteval[0]) ^ 0x80; // 0..255 -> -32768..32767
                wordval[1] =
                wordval[2] =
                wordval[3] = wordval[0];
                fwrite(wordval, 8, 1, WAVHandle[guestchan]);
        }   }
#endif
}   }

MODULE void write_silence(int guestchan, int samples)
{   switch (bitrate)
    {
    case 8:
        while (samples >= 1)
        {   if (samples >= SILENCEBUFSIZE)
            {   fwrite(SilenceBuffer8, SILENCEBUFSIZE, 1, WAVHandle[guestchan]); // write one second of silence
                samples -= SILENCEBUFSIZE;
            } else
            {   fwrite(SilenceBuffer8, samples       , 1, WAVHandle[guestchan]); // write a fraction of a second of silence
                samples = 0;
        }   }
#ifdef WIN32
    acase 16:
        while (samples >= 1)
        {   if (samples >= SILENCEBUFSIZE) // only * 1 here because samples is assuming 8 bits
            {   fwrite(SilenceBuffer16, SILENCEBUFSIZE * 2, 1, WAVHandle[guestchan]); // write one second of silence
                samples -= SILENCEBUFSIZE;
            } else
            {   fwrite(SilenceBuffer16, samples        * 2, 1, WAVHandle[guestchan]); // write a fraction of a second of silence
                samples = 0;
        }   }
#endif
}   }

EXPORT void edit_savetext(FLAG clip, FLAG wantasl, int whichprinter, FLAG partial)
{   int   i, j,
          lastx, firsty,
          x, y;
    TEXT  tempstring[64 + 1];
    FLAG  done;
    FILE* TheLocalHandle /* = NULL */ ;

    if (machine == INTERTON)
    {   return;
    }

    ProjectBuffer[0] = EOS;

    if (whichprinter != -1)
    {   // assert(machine == BINBUG || machine == PIPBUG || machine == CD2650 || machine == TWIN);
        if (partial && printer[whichprinter].page >= 2)
        {   for (y = printerrows_full - 1 - printer[whichprinter].sprocketrow; y < printerrows_full; y++)
            {   strcat((char*) ProjectBuffer, (const char*) printer[whichprinter].pdu[y]);
        }   }
        else
        {   for (y = printer[whichprinter].top; y < printerrows_full; y++)
            {   strcat((char*) ProjectBuffer, (const char*) printer[whichprinter].pdu[y]);
    }   }   }
    else
    {   switch (machine)
        {
        case ARCADIA:
            for (i = 0x1800; i < 0x18D0; i += 16)
            {   tempstring[1] = EOS;
                for (j = 0; j < 16; j++)
                {   tempstring[0] = arcadia_chars[memory[i + j] % 64];
                    strcat((char*) ProjectBuffer, (const char*) tempstring);
                }
                do_eol((STRPTR) tempstring);
                strcat((char*) ProjectBuffer, (const char*) tempstring);
            }

            if
            (   (memory[A_GFXMODE ] & 0x40)
             && (memory[A_BGCOLOUR] & 0x80)
            )
            {   for (i = 0x1A00; i < 0x1AD0; i += 16)
                {   tempstring[1] = EOS;
                    for (j = 0; j < 16; j++)
                    {   tempstring[0] = arcadia_chars[memory[i + j] % 64];
                        strcat((char*) ProjectBuffer, (const char*) tempstring);
                    }
                    do_eol((STRPTR) tempstring);
                    strcat((char*) ProjectBuffer, (const char*) tempstring);
            }   }
        acase ELEKTOR:
            for (y = 0; y < 6; y++)
            {   monob(y);
                strcat((char*) ProjectBuffer, (const char*) letters);
                do_eol((STRPTR) tempstring);
                strcat((char*) ProjectBuffer, (const char*) tempstring);
            }
        acase PIPBUG:
            i = 0;
            for (y = 0; y < vdu_rows_total; y++)
            {   for (x = 0; x < vdu_columns; x++)
                {   if (vdu[x][y] != blank)
                    {   ProjectBuffer[i++] = vdu[x][y];
                }   }
#ifdef WIN32
                ProjectBuffer[i++] = CR;
#endif
                ProjectBuffer[i++] = LF;
            }
            j = i;
            for (i = 0; i < ((pipbug_vdu == VDU_LCVDU_NARROW) ? 7 : 13); i++)
            {   if (i == 4 || i == 7 || i == 10)
                {   ProjectBuffer[j++] = ' ';
                }
                ProjectBuffer[j++] = led_chars[digitleds[i] & 0x7F];
                if (digitleds[i] & 0x80)
                {   ProjectBuffer[j++] = '.';
            }   }
            ProjectBuffer[j] = EOS;
        acase BINBUG:
            i = 0;
            for (y = 0; y < 16; y++)
            {   for (x = 0; x < 64; x++)
                {   ProjectBuffer[i++] = memory[0x7800 + (y * 64) + x];
                }
#ifdef WIN32
                ProjectBuffer[i++] = CR;
#endif
                ProjectBuffer[i++] = LF;
            }
        acase INSTRUCTOR:
        case SELBST:
        case MIKIT:
            ProjectBuffer[0] = led_chars[digitleds[0] & 0x7F];
            i = 1;
            if (machine != MIKIT && digitleds[0] & 0x80) ProjectBuffer[i++] = '.';
            ProjectBuffer[i++] = led_chars[digitleds[1] & 0x7F];
            if (machine != MIKIT && digitleds[1] & 0x80) ProjectBuffer[i++] = '.';
            ProjectBuffer[i++] = led_chars[digitleds[2] & 0x7F];
            if (machine != MIKIT && digitleds[2] & 0x80) ProjectBuffer[i++] = '.';
            ProjectBuffer[i++] = led_chars[digitleds[3] & 0x7F];
            if (machine != MIKIT && digitleds[3] & 0x80) ProjectBuffer[i++] = '.';
            if (machine == INSTRUCTOR || machine == SELBST || machine == MIKIT)
            {   if (machine == MIKIT)
                {   ProjectBuffer[i++] = ' ';
                }
                ProjectBuffer[i++] = led_chars[digitleds[4] & 0x7F];
                if (machine != MIKIT && digitleds[4] & 0x80) ProjectBuffer[i++] = '.';
                ProjectBuffer[i++] = led_chars[digitleds[5] & 0x7F];
                if (machine != MIKIT && digitleds[5] & 0x80) ProjectBuffer[i++] = '.';
                if (machine == INSTRUCTOR)
                {   ProjectBuffer[i++] = led_chars[digitleds[6] & 0x7F];
                    if (digitleds[6] & 0x80) ProjectBuffer[i++] = '.';
                    ProjectBuffer[i++] = led_chars[digitleds[7] & 0x7F];
                    if (digitleds[7] & 0x80) ProjectBuffer[i++] = '.';
            }   }
            ProjectBuffer[i] = EOS;
        acase TWIN:
            i = 0;
            firsty = -1;
            for (y = 0; y < 25; y++)
            {   for (x = 0; x < 80; x++)
                {   if (vdu[x][y] != TWIN_BLANK)
                    {   firsty = y;
                        goto DONE;
            }   }   }

DONE:
            if (firsty >= 0)
            {   for (y = firsty; y < 25; y++)
                {   lastx = -1;
                    for (x = 79; x >= 0; x--)
                    {   if (vdu[x][y] != TWIN_BLANK)
                        {   lastx = x;
                            break;
                    }   }
                    if (lastx >= 0)
                    {   for (x = 0; x <= lastx; x++)
                        {   ProjectBuffer[i++] = vdu[x][y];
                    }   }
#ifdef WIN32
                    ProjectBuffer[i++] = CR;
#endif
                    ProjectBuffer[i++] = LF;
            }   }
            ProjectBuffer[i] = EOS;
        acase TYPERIGHT:
            for (i = 0; i < 8; i++)
            {   if (tr_digits[i] == ' ' && tr_underline[i])
                {   ProjectBuffer[i] = '_';
                } else
                {   ProjectBuffer[i] = tr_digits[i];
            }   }
            ProjectBuffer[8] = EOS;
        acase CD2650:
            i = 0;
            for (y = 0; y < 16; y++)
            {   done = FALSE;
                for (x = 0; x < 80; x++)
                {   if (memory[0x1000 + (x * 16) + y])
                    {   ProjectBuffer[i++] = cd2650_chars[memory[0x1000 + (x * 16) + y] & 0x7F];
                        done = TRUE;
                }   }
                if (y < 15 && done)
                {
#ifdef WIN32
                    ProjectBuffer[i++] = CR;
#endif
                    ProjectBuffer[i++] = LF;
            }   }
            ProjectBuffer[i] = EOS;
        acase PHUNSY:
            i = 0;
            for (y = 0; y < 32; y++)
            {   done = FALSE;
                for (x = 0; x < 64; x++)
                {   if (memory[0x1000 + (y * 64) + x])
                    {   ProjectBuffer[i++] = memory[0x1000 + (y * 64) + x];
                        done = TRUE;
                }   }
                if (done)
                {
#ifdef WIN32
                    ProjectBuffer[i++] = CR;
#endif
                    ProjectBuffer[i++] = LF;
            }   }
            for (j = 0; j < 6; j++)
            {   ProjectBuffer[i++] = led_chars[digitleds[j] & 0x7F];
                if (digitleds[j] & 0x80)
                {   ProjectBuffer[i++] = '.';
            }   }
#ifdef WIN32
            ProjectBuffer[i++] = CR;
#endif
            ProjectBuffer[i++] = LF;
            ProjectBuffer[i  ] = EOS;
        acase ZACCARIA:
            if (rotating)
            {   for (i = 0x1800; i < 0x1820; i++)
                {   for (j = 0; j < 32; j++)
                    {   tempstring[j] = guestchar(memory[i + ((31 - j) * 32)]);
                    }
                    tempstring[32] = EOS;
                    strcat((char*) ProjectBuffer, (const char*) tempstring);
                    do_eol((STRPTR) tempstring);
                    strcat((char*) ProjectBuffer, (const char*) tempstring);
            }   }
            else
            {   for (i = 0x1800; i < 0x1C00; i += 32)
                {   for (j = 0; j < 32; j++)
                    {   tempstring[j] = guestchar(memory[i + j]);
                    }
                    tempstring[32] = EOS;
                    strcat((char*) ProjectBuffer, (const char*) tempstring);
                    do_eol((STRPTR) tempstring);
                    strcat((char*) ProjectBuffer, (const char*) tempstring);
            }   }
        acase MALZAK:
            for (i = 0x1800; i < 0x1E00; i += 64)
            {   tempstring[64] = EOS;
                for (j = 0; j < 64; j++)
                {   tempstring[j] = guestchar(memory[i + j]);
                }
                for (j = 63; j >= 0; j--)
                {   if (tempstring[j] == ' ')
                    {   tempstring[j] = EOS;
                    } else
                    {   break;
                }   }
                strcat((char*) ProjectBuffer, (const char*) tempstring);
                do_eol((STRPTR) tempstring);
                strcat((char*) ProjectBuffer, (const char*) tempstring);
            }
        acase PONG:
            sprintf((char*) tempstring, "%2d %2d", score[0], score[1]);
            strcpy((char*) ProjectBuffer, (const char*) tempstring);
            do_eol((STRPTR) tempstring);
            strcat((char*) ProjectBuffer, (const char*) tempstring);
    }   }

    if (clip)
    {
#ifdef WIN32
        clip_open(TRUE);
        clip_write(ProjectBuffer, strlen(ProjectBuffer), CF_TEXT);
        clip_close();
#endif
#ifdef AMIGA
        edit_copytext_amiga();
#endif
    } else
    {   if (wantasl)
        {   if (game)
            {   strcpy((char*) fn_project, (const char*) fn_game);
            } else
            {   fn_project[0] = EOS;
            }
            fixextension(filekind[KIND_ASCII].extension, (STRPTR) fn_project, TRUE, ""); // so we have extension in ASL requester
            if (asl
            (   filekind[KIND_ASCII].hail,
                filekind[KIND_ASCII].pattern,
                TRUE,
                (STRPTR) path_projects,
                (STRPTR) fn_project, // this fortunately doesn't contain the path right now
                (STRPTR) fn_project
            ) != EXIT_SUCCESS)
            {   return;
            }
            fixextension(filekind[KIND_ASCII].extension, (STRPTR) fn_project, TRUE, ""); // so we have extension even if user overtypes it in ASL requester
#ifdef SETPATHS
            break_pathname((STRPTR) fn_project, (STRPTR) path_projects, NULL);
#endif
            if (!((TheLocalHandle = fopen((const char*) fn_project, "wt"))))
            {   say("Can't open file for output!");
                return;
        }   }
        else
        {   if (game)
            {   strcpy(fn_project, file_game);
            } else
            {   if (machine == TWIN)
                {   if (whichprinter == 0) strcpy(fn_project, "LPT1");
                    else                   strcpy(fn_project, "LPT2");
                } else
                {                          strcpy(fn_project, "Printer");
            }   }
            fixextension(filekind[KIND_ASCII].extension, fn_project, TRUE, "");
            cd_projects();
            if (printer[whichprinter].page == 1)
            {   if (!((TheLocalHandle = fopen((const char*) fn_project, "wb"))))
                {   say("Can't open file for output!");
                    cd_progdir();
                    return;
                }
#ifdef VERBOSE
                zprintf(TEXTPEN_VERBOSE, "Opened \"%s\" for overwriting.\n", fn_project);
#endif
            }
            else
            {   if (!((TheLocalHandle = fopen((const char*) fn_project, "ab"))))
                {   say("Can't open file for appending!");
                    cd_progdir();
                    return;
                }
#ifdef VERBOSE
                zprintf(TEXTPEN_VERBOSE, "Opened \"%s\" for appending.\n", fn_project);
#endif
        }   }

        DISCARD fwrite(ProjectBuffer, strlen((const char*) ProjectBuffer), 1, TheLocalHandle);
        fclose(TheLocalHandle);
        // TheLocalHandle = NULL;

#ifdef VERBOSE
        if (partial)
        {   zprintf(TEXTPEN_VERBOSE, "Saved partial page %d of printer %d text as \"%s\".\n", printer[whichprinter].page, whichprinter, fn_project);
        } else
        {   zprintf(TEXTPEN_VERBOSE, "Saved full page %d of printer %d text as \"%s\".\n", printer[whichprinter].page, whichprinter, fn_project);
        }
#endif

        if (!wantasl)
        {   cd_progdir();
}   }   }

EXPORT void saveasmscrnshot(void)
{   int i, j;

    if (game)
    {   strcpy((char*) fn_project, (const char*) file_game);
    } else
    {   fn_project[0] = EOS;
    }
    fixextension(filekind[KIND_ASM].extension, (STRPTR) fn_project, TRUE, ""); // so we have extension in ASL requester
    if (asl
    (   filekind[KIND_ASM].hail,
        filekind[KIND_ASM].pattern,
        TRUE,
        (STRPTR) path_projects,
        (STRPTR) fn_project, // this fortunately doesn't contain the path right now
        (STRPTR) fn_project
    ) != EXIT_SUCCESS)
    {   return;
    }
    fixextension(filekind[KIND_ASM].extension, (STRPTR) fn_project, TRUE, ""); // so we have extension even if user overtypes it in ASL requester
#ifdef SETPATHS
    break_pathname((STRPTR) fn_project, (STRPTR) path_projects, NULL);
#endif

    // assert(!DisHandle);
    if (!(DisHandle = fopen((const char*) fn_project, "wb")))
    {   beep();
        return;
    }

    doasmhdr("EQU");
    zprintf(TEXTPEN_TRACE, "\n");
    if (machine == INTERTON)
    {   zprintf(TEXTPEN_TRACE, "        ORG     0\n\n");
    } elif (machine == ELEKTOR)
    {   zprintf(TEXTPEN_TRACE, "        ORG     $900\n\n");
    }
    zprintf(TEXTPEN_TRACE, scrnasmhdr[machine]);

    switch (machine)
    {
    case ARCADIA:
        strcpy(asmstring, "UPPERSCRN:\n");
        zprintf(TEXTPEN_TRACE, asmstring);
        for (i = 0; i < 13; i++)
        {   strcpy(asmstring, "        DB      ");
            for (j = 0; j < 15; j++)
            {   sprintf(&asmstring[16 + ( j * 4)], "$%02X,", memory[0x1800 + (i * 16) +  j]);
            }
            sprintf(    &asmstring[16 + (15 * 4)], "$%02X\n", memory[0x1800 + (i * 16) + 15]);
            zprintf(TEXTPEN_TRACE, asmstring);
        }

        emit_dbs("POSITIONS",  8, 0x18F0);

        emit_dbs("MISC",       4, 0x18FC);

        strcpy(asmstring, "IMAGERY:\n");
        zprintf(TEXTPEN_TRACE, asmstring);
        for (i = 0; i < 8; i++)
        {   strcpy(asmstring, "        DB      ");
            for (j = 0; j < 7; j++)
            {   sprintf(&asmstring[16 + (j * 4)], "$%02X,", memory[0x1980 + (i * 8) + j]);
            }
            sprintf(    &asmstring[16 + (7 * 4)], "$%02X\n", memory[0x1980 + (i * 8) + 7]);
            zprintf(TEXTPEN_TRACE, asmstring);
        }

        emit_dbs("WRITEONLY",  3, 0x19F8);

        strcpy(asmstring, "LOWERSCRN:\n");
        zprintf(TEXTPEN_TRACE, asmstring);
        for (i = 0; i < 13; i++)
        {   strcpy(asmstring, "        DB      ");
            for (j = 0; j < 15; j++)
            {   sprintf(&asmstring[16 + ( j * 4)], "$%02X,", memory[0x1A00 + (i * 16) +  j]);
            }
            sprintf(    &asmstring[16 + (15 * 4)], "$%02X\n", memory[0x1A00 + (i * 16) + 15]);
            zprintf(TEXTPEN_TRACE, asmstring);
        }
    acase INTERTON:
    case ELEKTOR:
        emit_dbs("NOISEDATA",  1, 0x1E80);
        emit_dbs("SPR0DATA" , 14, 0x1F00);
        emit_dbs("SPR1DATA" , 14, 0x1F10);
        emit_dbs("SPR2DATA" , 14, 0x1F20);
        emit_dbs("SPR3DATA" , 14, 0x1F40);
        emit_dbs("GRID"     , 45, 0x1F80);
        emit_dbs("MISC1"    ,  4, 0x1FC0);
        emit_dbs("MISC2"    ,  4, 0x1FC6);
    }

    strcpy(asmstring, "\n        END\n");
    zprintf(TEXTPEN_TRACE, asmstring);

    DISCARD fclose(DisHandle);
    DisHandle = NULL;

#ifdef AMIGA
    writeicon(KIND_ASM, (STRPTR) fn_project);
#endif
}

MODULE void emit_dbs(STRPTR labelstring, int howmany, int address)
{   int i;

    strcpy(asmstring, labelstring);
    strcat(asmstring, ":\n");
    zprintf(TEXTPEN_TRACE, asmstring);
    strcpy(asmstring, "        DB      ");
    if (howmany >= 2)
    {   for (i = 0; i < howmany - 1; i++)
        {   sprintf(&asmstring[16 + (           i  * 4)], "$%02X," , memory[address +       i    ]);
    }   }
    sprintf(        &asmstring[16 + ((howmany - 1) * 4)], "$%02X\n", memory[address + howmany - 1]);
    zprintf(TEXTPEN_TRACE, asmstring);
}

EXPORT FLAG load_sms(int localsize)
{   UBYTE t;
    int   cursor,
          i;

    if (IOBuffer[0] != 0x12)
    {   zprintf(TEXTPEN_ERROR, "Invalid header!\n\n");
        return FALSE;
    }

    cursor = 1;
    i = 0;
    while (cursor < localsize - 1)
    {   if (IOBuffer[cursor + 1] == QUOTE) // nybble only
        {   if (    IOBuffer[cursor] >= '0' && IOBuffer[cursor] <= '9')
            {   t = IOBuffer[cursor] - '0';
            } elif (IOBuffer[cursor] >= 'A' && IOBuffer[cursor] <= 'F')
            {   t = IOBuffer[cursor] - 'A' + 10;
            } elif (IOBuffer[cursor] >= 'a' && IOBuffer[cursor] <= 'f')
            {   t = IOBuffer[cursor] - 'a' + 10;
            } else
            {   zprintf(TEXTPEN_ERROR, "Unexpected character!\n\n");
                return FALSE;
            }
            memory[i++] = t;
            cursor += 2;
        } elif (cursor < localsize - 2 && IOBuffer[cursor + 2] == QUOTE) // entire byte
        {   if (    IOBuffer[cursor] >= '0' && IOBuffer[cursor] <= '9')
            {   t = IOBuffer[cursor] - '0';
            } elif (IOBuffer[cursor] >= 'A' && IOBuffer[cursor] <= 'F')
            {   t = IOBuffer[cursor] - 'A' + 10;
            } elif (IOBuffer[cursor] >= 'a' && IOBuffer[cursor] <= 'f')
            {   t = IOBuffer[cursor] - 'a' + 10;
            } else
            {   zprintf(TEXTPEN_ERROR, "Unexpected character!\n\n");
                return FALSE;
            }
            t <<= 4; // or, t *= 16;
            if (     IOBuffer[cursor + 1] >= '0' && IOBuffer[cursor + 1] <= '9')
            {   t += IOBuffer[cursor + 1] - '0';
            } elif ( IOBuffer[cursor + 1] >= 'A' && IOBuffer[cursor + 1] <= 'F')
            {   t += IOBuffer[cursor + 1] - 'A' + 10;
            } elif ( IOBuffer[cursor + 1] >= 'a' && IOBuffer[cursor + 1] <= 'f')
            {   t += IOBuffer[cursor + 1] - 'a' + 10;
            } else
            {   zprintf(TEXTPEN_ERROR, "Unexpected character!\n\n");
                return FALSE;
            }
            memory[i++] = t;
            cursor += 3;
        } else
        {   zprintf(TEXTPEN_ERROR, "No byte end marker found!\n\n");
            return FALSE;
    }   }

    // assert(cursor == localsize - 1);
    if (IOBuffer[cursor] != 0x14)
    {   zprintf(TEXTPEN_ERROR, "Invalid footer!\n\n");
        return FALSE;
    }

    zprintf(TEXTPEN_CLIOUTPUT, "Successfully loaded SMS file from $0..$%X.\n\n", i - 1);
    return TRUE;
}

EXPORT FLAG save_sms(STRPTR filename, int endaddr)
{   UBYTE SMSBuffer[3],
          t;
    FILE* SMSHandle;
    int   i;

    // assert(startaddr <= endaddr);

#ifdef VERBOSE
    zprintf(TEXTPEN_CLIOUTPUT, "Saving %s from $0..$%X as SMS...\n", filename, endaddr);
#endif

    if (!(SMSHandle = fopen(filename, "wb")))
    {   zprintf(TEXTPEN_CLIOUTPUT, "Can't open \"%s\" for writing!\n", filename);
        return FALSE;
    } // implied else

    SMSBuffer[0] = 0x12; // DC2 (Tape On)
    fwrite(SMSBuffer, 1, 1, SMSHandle);

    for (i = 0; i <= endaddr; i++)
    {   t = (memory[i] & 0xF0) >> 4;
        if (t)
        {   if (t >= 10)
            {   SMSBuffer[0] = 'A' + t - 10;
            } else
            {   SMSBuffer[0] = '0' + t     ;
            }
            t = memory[i] & 0x0F;
            if (t >= 10)
            {   SMSBuffer[1] = 'A' + t - 10;
            } else
            {   SMSBuffer[1] = '0' + t     ;
            }
            SMSBuffer[    2] = QUOTE;
            DISCARD fwrite(SMSBuffer, 3, 1, SMSHandle);
        } else
        {   t = memory[i] & 0x0F;
            if (t >= 10)
            {   SMSBuffer[0] = 'A' + t - 10;
            } else
            {   SMSBuffer[0] = '0' + t     ;
            }
            SMSBuffer[    1] = QUOTE;
            DISCARD fwrite(SMSBuffer, 2, 1, SMSHandle);
    }   }

    SMSBuffer[0] = 0x14; // DC4 (Tape Off)
    fwrite(SMSBuffer, 1, 1, SMSHandle);

    DISCARD fclose(SMSHandle);
    // SMSHandle = NULL;

    return TRUE;
}

EXPORT FLAG load_bpnf(int localsize, int startaddr)
{   UBYTE t;
    int   i, j,
          offset;

    offset = 0;
    j = startaddr;

    for (;;)
    {   if (offset > localsize - 10)
        {   zprintf(TEXTPEN_ERROR, "File is corrupt or truncated!\n\n");
            return FALSE;
        }

        if (IOBuffer[offset++] != 'B')
        {   zprintf(TEXTPEN_ERROR, "Unexpected character (expected 'B')!\n\n");
            return FALSE;
        }
        t = 0;
        for (i = 0; i < 8; i++)
        {   if (IOBuffer[offset] == 'P')
            {   t |= (0x80 >> i);
            } elif (IOBuffer[offset] != 'N')
            {   zprintf(TEXTPEN_ERROR, "Unexpected character (expected 'P' or 'N')!\n\n");
                return FALSE;
            }
            offset++;
        }
        if (IOBuffer[offset++] != 'F')
        {   zprintf(TEXTPEN_ERROR, "Unexpected character (expected 'F')!\n\n");
            return FALSE;
        }

        memory[j++] = t;

        if (offset == localsize)
        {   zprintf(TEXTPEN_CLIOUTPUT, "Successfully loaded BPNF file from $%X..$%X.\n\n", startaddr, j - 1);
            return TRUE;
        }

        if
        (   IOBuffer[offset] != ' '
         && IOBuffer[offset] != CR
         && IOBuffer[offset] != LF
         && IOBuffer[offset] != HT
         && IOBuffer[offset] != VT
         && IOBuffer[offset] != FF
        )
        {   zprintf(TEXTPEN_ERROR, "Unexpected character (expected whitespace)!\n\n");
            return FALSE;
        }

        while
        (   offset < localsize
         && (   IOBuffer[offset] == ' '
             || IOBuffer[offset] == CR
             || IOBuffer[offset] == LF
             || IOBuffer[offset] == HT
             || IOBuffer[offset] == VT
             || IOBuffer[offset] == FF
        )   )
        {   offset++;
        }

        if (offset == localsize)
        {   zprintf(TEXTPEN_ERROR, "Warning: extraneous whitespace at end of file.\n\n");
            zprintf(TEXTPEN_CLIOUTPUT, "Successfully loaded BPNF file from $%X..$%X.\n\n", startaddr, j - 1);
            return TRUE;
}   }   }

EXPORT FLAG save_bpnf(STRPTR filename, int startaddr, int endaddr)
{   UBYTE BPNFBuffer[11];
    FILE* BPNFHandle;
    int   i, j;

    // assert(startaddr <= endaddr);

#ifdef VERBOSE
    zprintf(TEXTPEN_CLIOUTPUT, "Saving %s from $%X..$%X as BPNF...\n", filename, startaddr, endaddr);
#endif

    if (!(BPNFHandle = fopen(filename, "wb")))
    {   zprintf(TEXTPEN_CLIOUTPUT, "Can't open \"%s\" for writing!\n", filename);
        return FALSE;
    } // implied else

    BPNFBuffer[ 0] = 'B';
    BPNFBuffer[ 9] = 'F';
    BPNFBuffer[10] = ' ';

    for (i = startaddr; i <= endaddr; i++)
    {   for (j = 0; j < 8; j++)
        {   BPNFBuffer[1 + j] = (memory[i] & (0x80 >> j)) ? 'P' : 'N';
        }
        DISCARD fwrite(BPNFBuffer, (i == endaddr) ? 10 : 11, 1, BPNFHandle);
    }

    DISCARD fclose(BPNFHandle);
    // BPNFHandle = NULL;

    return TRUE;
}

EXPORT void writepsgfile(void)
{   FAST int i, j, k;

    for (i = 0; i < 2; i++)
    {   if (PSGHandle[i])
        {   // assert(psgfile);

            j = 0;
            for (k = 0; k < 14; k++)
            {   if (memory[0x1D00 + (i * 16) + k] != oldpsg[i][k])
                {   PSGBuffer[j++] = k;
                    PSGBuffer[j++] = oldpsg[i][k] = memory[0x1D00 + (i * 16) + k];
            }   }
            PSGBuffer[j++] = 0xFF;
            DISCARD fwrite(PSGBuffer, j, 1, PSGHandle[i]);
}   }   }

EXPORT void writeymfile(void)
{   FAST int i, j;

    for (i = 0; i < 2; i++)
    {   if (YMHandle[i])
        {   // assert(ymfile);

            for (j = 0; j < 16; j++)
            {   YMBuffer[j] = memory[0x1D00 + (i * 16) + j];
            }
            DISCARD fwrite(YMBuffer, 16, 1, YMHandle[i]);
}   }   }

EXPORT void macro_stop(void)
{   int    i;
#ifdef INTERLEAVED
    int    j, k;
    UBYTE  tempbuffer[16];
    UBYTE* InterleavedBufferPtr[16];
#endif

 // tape_eject();

    if (recmode == RECMODE_NORMAL)
    {   return;
    }

    busypointer();

    if (recmode == RECMODE_PLAY)
    {   // don't call free_iobuffer() (causes crashes when looping)!
        ghost_dips(TRUE); // to unghost
    } else
    {   // assert(recmode == RECMODE_RECORD);
        // assert(MacroHandle);
        DISCARD fclose(MacroHandle);
        MacroHandle = NULL;
    }

    if
    (   (recmode == RECMODE_RECORD || (recmode == RECMODE_PLAY && generate))
     && (esvxfile || aifffile || wavfile)
    )
    {   sound_stoprecording(); // must be done before finalize_anim()!
    }

    if (ANIMHandle)
    {   finalize_anim(ANIMHandle);
    }

#ifdef WIN32
    CloseAVIAnim();
#endif

    if (GIFHandle)
    {   putc(0x3B, GIFHandle);
        fclose(GIFHandle);
        GIFHandle = NULL;
    }

    if (MNGHandle)
    {   write_mend(MNGHandle);
        write_mhdr_end(MNGHandle);
        fclose(MNGHandle);
        MNGHandle = NULL;
    }

    if (PNGHandle)
    {   write_iend(PNGHandle);
        write_actl_end(PNGHandle);
        fclose(PNGHandle);
        PNGHandle = NULL;
    }

    if (MIDIHandle)
    {   finalize_midi();
    }

    if (SMUSHandle)
    {   finalize_smus();
    }

    PSGBuffer[0] = 0xFF; // should be $FD but then AYEmul won't play it
    YMBuffer[0]  = 'E';
    YMBuffer[1]  = 'n';
    YMBuffer[2]  = 'd';
    YMBuffer[3]  = '!';
    ulong_to_be(&YMBuffer[4], animframe - startframe + 1);

    for (i = 0; i < 2; i++)
    {   if (PSGHandle[i])
        {   DISCARD fwrite(PSGBuffer, 1, 1, PSGHandle[i]);
            DISCARD fclose(PSGHandle[i]);
            PSGHandle[i] = NULL;
        }

        if (YMHandle[i])
        {   DISCARD fwrite(&YMBuffer[0], 4, 1, YMHandle[i]);
            DISCARD fseek(YMHandle[i], 0xC, SEEK_SET);
            DISCARD fwrite(&YMBuffer[4], 4, 1, YMHandle[i]);
            // no need to seek to end of file before closing it

#ifdef INTERLEAVED
            DISCARD fseek(YMHandle[i], datastart, SEEK_SET);
            for (j = 0; j < 16; j++)
            {   InterleavedBufferPtr[j] = malloc(animframe - startframe + 1); // we should check return code (eg. out of memory is possible)
            }
            // fflush() is not needed before switching to read mode because we have done an fseek()
            for (k = 0; k < (int) (animframe - startframe + 1); k++)
            {   DISCARD fread(tempbuffer, (size_t) 16, 1, YMHandle[i]);
                for (j = 0; j < 16; j++)
                {   InterleavedBufferPtr[j][k] = tempbuffer[j];
            }   }
            DISCARD fseek(YMHandle[i], datastart, SEEK_SET);
            // fflush() is not needed before switching to write mode because we have done an fseek()
            for (j = 0; j < 16; j++)
            {   DISCARD fwrite(&InterleavedBufferPtr[j][0], animframe - startframe + 1, 1, YMHandle[i]);
                DISCARD free(InterleavedBufferPtr[j]);
                // InterleavedBufferPtr[j] = NULL;
            }
#endif

            DISCARD fclose(YMHandle[i]);
            YMHandle[i] = NULL;
    }   }

    if (recmode == RECMODE_RECORD)
    {   audit(FALSE); // so the freshly made COR is added to sidebar
    }

    recmode = RECMODE_NORMAL;
    updatesmlgads();
    updatemenus();
    settitle();
    normalpointer();
}

EXPORT void macro_restartplayback(void)
{   offset = 0;
    DISCARD parse_bytes(1); // this sets recmode = RECMODE_PLAY for us
}

EXPORT void project_save(int kind)
{   if (crippled)
    {   return;
    }

    if (recmode == RECMODE_PLAY && kind == KIND_COR && (MacroHandle = fopen((const char*) fn_game, "wb")))
    {   DISCARD fwrite(IOBuffer, offset, 1, MacroHandle);
#ifdef VERBOSE
        zprintf(TEXTPEN_VERBOSE, "Wrote file %s, offset is %d...\n", fn_game, offset);
#endif
        recmode = RECMODE_RECORD;
        updatebiggads();
        updatesmlgads();
        updatemenus();
        ghost_dips(TRUE);
        return;
    }

    if (recmode != RECMODE_RECORD)
    {   macro_stop();
    }

    strcpy((char*) fn_oldgame, (const char*) fn_game);
    fixextension(filekind[kind].extension, (STRPTR) fn_game, TRUE, "");
    break_pathname((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game);
    if (asl
    (   filekind[kind].hail,
        filekind[kind].pattern,
        TRUE,
        (STRPTR) path_games,
        (STRPTR) file_game,
        (STRPTR) fn_game
    ) != EXIT_SUCCESS)
    {   updatesmlgad(GADPOS_RECORD, 0, TRUE);
        updatesmlgad(GADPOS_PLAY,   0, TRUE);
        updatesmlgad(GADPOS_STOP,   0, TRUE);
        return;
    }
    fixextension(filekind[kind].extension, (STRPTR) fn_game, FALSE, "");
    break_pathname((STRPTR) fn_game, (STRPTR) path_games, (STRPTR) file_game);
    engine_save(kind, TRUE);
    add_recent();

    if (kind == KIND_COR)
    {   macro_start();
        updatebiggads();
        updatesmlgads();
        updatemenus();
}   }

EXPORT void engine_save(int kind, FLAG updatetitle)
{   FILE* TheLocalHandle /* = NULL */ ;

    // kind can be KIND_COS or KIND_COR

    if (recmode == RECMODE_PLAY)
    {   macro_stop(); // in case we are eg. playing back a recording and do a state save
    }

    alloc_iobuffer((ULONG) MAX_STATESIZE);
    offset = 0;
    serializemode = SERIALIZE_WRITE;

    // magic byte (1 byte)
    WriteByte(    0xBF);  // byte 0. BSXA,r3
    cosversion = (int) machines[machine].cosversion;
    WriteByte((UBYTE) cosversion); // byte 1
    if (kind == KIND_COS)
    {   WriteByte(0); // byte 2.
    } else
    {   // assert(kind == KIND_COR);
        WriteByte(1); // byte 2.
    }
    WriteByte((UBYTE) memmap); // byte 3

    if (machine == PHUNSY)
    {   ramtoubank((banked & 0xF0) >> 4);
        ramtoqbank( banked & 0x0F      );
    }

    serialize_cos(); // bytes 4+

    if ((TheLocalHandle = fopen((const char*) fn_game, "wb")))
    {   DISCARD fwrite(IOBuffer, (size_t) offset, 1, TheLocalHandle);
        DISCARD fclose(TheLocalHandle);
        // TheLocalHandle = NULL;
    } else
    {   beep();
        free_iobuffer();
        return;
    }
    free_iobuffer();

#ifdef AMIGA
    writeicon(kind, (STRPTR) fn_game);
#endif

    game = TRUE;

    if (kind != KIND_COR)
    {   if (recmode == RECMODE_RECORD)
        {   strcpy((char*) fn_game, (const char*) fn_oldgame);
        }
        if (updatetitle)
        {   settitle();
        }
        return;
    }

    if (!(MacroHandle = fopen((const char*) fn_game, "a+b")))
    {   beep();
        return;
    }

    // assert(updatetitle == TRUE);
    recmode = RECMODE_RECORD;
    settitle();
    updatesmlgads();

#ifdef WIN32
    if (cheevos)
    {
#ifdef LOGCHEEVOS
        zprintf(TEXTPEN_VERBOSE, "RA_OnSaveState(%s)\n", fn_game);
#endif
        RA_OnSaveState(fn_game);
    }
#endif
}

#ifdef WIN32
EXPORT int load_bmp(const char* thefilename)
{   FILE* file;
    int   totalsize,
          x, y;
    UBYTE header[56];

    if (!((file = fopen(thefilename, "rb"))))
    {   say("Can't open BMP file for input!");
        return 1;
    }

    fread(header, 56, 1, file);
    if (header[0] != 'B' || header[1] != 'M')
    {   fclose(file);
        say("Invalid BMP header!");
        return 2;
    }

    if (header[28] != 24 || header[29] != 0)
    {   fclose(file);
        say("Only 24-bit images are supported!");
        return 3;
    }

    fseek
    (   file,
         header[10]
      + (header[11] *      256)
      + (header[12] *    65536)
      + (header[13] * 16777216),
        SEEK_SET
    );
    bgwidth   =  header[18]
              + (header[19] *      256)
              + (header[20] *    65536)
              + (header[21] * 16777216);
    if (bgwidth < 0)
    {   fclose(file);
        say("Invalid BMP width!");
        return 4;
    }

    bgheight  =  header[22]
              + (header[23] *      256)
              + (header[24] *    65536)
              + (header[25] * 16777216);
    if (bgwidth < 0)
    {   fclose(file);
        say("Invalid BMP height!");
        return 5;
    }

    if (header[30] != 0 || header[31] != 0 || header[32] != 0 || header[33] != 0)
    {   fclose(file);
        say("Only uncompressed images are supported!");
        return 6;
    }

    bgwidthp  = ROUNDTO4(bgwidth);
    totalsize = bgheight * bgwidthp * 4;
    if (!((pixelubyte = calloc(totalsize, 1))))
    {   fclose(file);
        say("Out of memory!");
        return 7;
    }

    pixelulong = (ULONG*) pixelubyte;
    for (y = 0; y < bgheight; y++)
    {   for (x = 0; x < bgwidth; x++)
        {   fread(&pixelubyte[((bgheight - 1 - y) * bgwidthp * 4) + (x * 4)], 1, 3, file);
        }
        if ((bgwidth * 3) % 4)
        {   fseek(file, 4 - ((bgwidth * 3) % 4), SEEK_CUR);
    }   }

    fclose(file);

    break_pathname((STRPTR) fn_bkgrnd, (STRPTR) path_bkgrnd, (STRPTR) file_bkgrnd);

    return 0;
}
#endif

EXPORT void macro_start(void)
{   TRANSIENT     int   i;
    PERSIST const UBYTE YMHeader[34] = {
//  0    1    2    3    4    5    6    7    8    9    A    B   C   D   E   F
   'Y', 'M', '6', '!', 'L', 'e', 'O', 'n', 'A', 'r', 'D', '!',                 // $00..$0B: header
                                                               0 , 0 , 0 , 0 , // $0C..$0F: # of frames in file (done later)
#ifdef INTERLEAVED
    0 ,  0 ,  0 ,  1 ,                                                         // $10..$13: song attributes (    interleaved)
#else
    0 ,  0 ,  0 ,  0 ,                                                         // $10..$13: song attributes (non-interleaved)
#endif
                        0 ,  0 ,                                               // $14..$15: # of digidrum samples in file
                                0x00,0x1B,0x0F,0x58,                           // $16..$19: YM master clock implementation in Hz (110,837*16=1,773,400)
                                                    0x00,0x32,                 // $1A..$1B: original player frame in Hz (50)
                                                               0 , 0 , 0 , 0 , // $1C..$1F: loop frame
    0 ,  0                                                                     // $20..$21: size of additional data
};

    // assert(recmode == RECMODE_PLAY || recmode == RECMODE_RECORD);

    animframe  = (ULONG) -1;
    startframe = frames;

    if (memmap == MEMMAP_F)
    {   if (psgfile)
        {   for (i = 0; i < 14; i++)
            {   oldpsg[0][i] =
                oldpsg[1][i] = 0;
            }
            PSGBuffer[0] = 'P';
            PSGBuffer[1] = 'S';
            PSGBuffer[2] = 'G';
            PSGBuffer[3] = 0x1A;
            for (i = 4; i <= 15; i++)
            {   PSGBuffer[i] = 0;
            }
            PSGBuffer[16] = 0xFF;

            strcpy((char*) fn_save, (const char*) fn_game);
            fixextension(filekind[KIND_PSG].extension, (STRPTR) fn_save, TRUE, "-1");
            if ((PSGHandle[0] = fopen((const char*) fn_save, "wb")))
            {   DISCARD fwrite(PSGBuffer, 17, 1, PSGHandle[0]);
            }

            strcpy((char*) fn_save, (const char*) fn_game);
            fixextension(filekind[KIND_PSG].extension, (STRPTR) fn_save, TRUE, "-2");
            if ((PSGHandle[1] = fopen((const char*) fn_save, "wb")))
            {   DISCARD fwrite(PSGBuffer, 17, 1, PSGHandle[1]);
        }   }

        if (ymfile)
        {   strcpy((char*) fn_save, (const char*) fn_game);
            fixextension(filekind[KIND_YM].extension,  (STRPTR) fn_save, TRUE, "-1");
            if ((YMHandle[0] = fopen((const char*) fn_save, "w+b")))
            {   DISCARD fwrite(YMHeader,                                                                              34, 1, YMHandle[0]); // header
                DISCARD fwrite(autotext[AUTOLINE_GAMENAME]       , strlen((const char*) autotext[AUTOLINE_GAMENAME]) + 1, 1, YMHandle[0]); // song
                DISCARD fwrite(autotext[AUTOLINE_CREDITS ]       , strlen((const char*) autotext[AUTOLINE_CREDITS ]) + 1, 1, YMHandle[0]); // author
                DISCARD fwrite("Generated by "                   , strlen("Generated by "                          )    , 1, YMHandle[0]); // comment
                DISCARD fwrite(hostmachines[machine].titlebartext, strlen(hostmachines[machine].titlebartext       ) + 1, 1, YMHandle[0]); // comment
            }

            datastart = ftell(YMHandle[0]); // this will be the same for both YM files

            strcpy((char*) fn_save, (const char*) fn_game);
            fixextension(filekind[KIND_YM].extension,  (STRPTR) fn_save, TRUE, "-2");
            if ((YMHandle[1] = fopen((const char*) fn_save, "w+b")))
            {   DISCARD fwrite(YMHeader,                                                                              34, 1, YMHandle[1]); // header
                DISCARD fwrite(autotext[AUTOLINE_GAMENAME]       , strlen((const char*) autotext[AUTOLINE_GAMENAME]) + 1, 1, YMHandle[1]); // song
                DISCARD fwrite(autotext[AUTOLINE_CREDITS ]       , strlen((const char*) autotext[AUTOLINE_CREDITS ]) + 1, 1, YMHandle[1]); // author
                DISCARD fwrite("Generated by "                   , strlen("Generated by "                          )    , 1, YMHandle[1]); // comment
                DISCARD fwrite(hostmachines[machine].titlebartext, strlen(hostmachines[machine].titlebartext       ) + 1, 1, YMHandle[1]); // comment
    }   }   }

    if (midifile)
    {   midi_startrecording();
    }
    if (smusfile)
    {   smus_startrecording();
    }
    if (esvxfile || aifffile || wavfile)
    {   sound_startrecording();
    }

#ifdef WIN32
    if (avianims)
    {   strcpy(fn_save, fn_game);
        fixextension(filekind[KIND_AVIANIM].extension, fn_save, TRUE, "");
        OpenAVIAnim(fn_save);
    }
#endif

    if (mnganims || apnganims || gifanims || iffanims)
    {   strcpy((char*) fn_oldscreenshot, (const char*) fn_screenshot);
    }
    if (mnganims)
    {   strcpy((char*) fn_screenshot, (const char*) fn_game);
        fixextension(filekind[KIND_MNG].extension, (STRPTR) fn_screenshot, TRUE, "");
        savescreenshot(KIND_MNG, FALSE, FALSE, TRUE);
    }
    if (apnganims)
    {   strcpy((char*) fn_screenshot, (const char*) fn_game);
        fixextension(filekind[KIND_PNG].extension, (STRPTR) fn_screenshot, TRUE, "");
        savescreenshot(KIND_PNG, FALSE, FALSE, TRUE);
    }
    if (gifanims)
    {   strcpy((char*) fn_screenshot, (const char*) fn_game);
        fixextension(filekind[KIND_GIF].extension, (STRPTR) fn_screenshot, TRUE, "");
        savescreenshot(KIND_GIF, FALSE, FALSE, TRUE);
    }
    if (iffanims)
    {   strcpy((char*) fn_screenshot, (const char*) fn_game);
        fixextension(filekind[KIND_IFFANIM].extension, (STRPTR) fn_screenshot, TRUE, "");
        savescreenshot(KIND_IFFANIM, FALSE, FALSE, TRUE);
    }
    if (mnganims || apnganims || gifanims || iffanims)
    {   strcpy((char*) fn_screenshot, (const char*) fn_oldscreenshot);
}   }

MODULE void set_volume(int guestvolume)
{   if (bitrate == 8)
    {   if (machines[machine].pvis)
        {   upvol_8  =  volume_4to16[guestvolume].up8;
            dnvol_8  =  volume_4to16[guestvolume].dn8;
        } else
        {   upvol_8  =  volume_3to16[guestvolume].up8;
            dnvol_8  =  volume_3to16[guestvolume].dn8;
    }   }
#ifdef WIN32
    else
    {   // assert(bitrate == 16);
        if (machines[machine].pvis)
        {   upvol_16 =  volume_4to16[guestvolume].up16;
            dnvol_16 = -volume_4to16[guestvolume].up16;
        } else
        {   upvol_16 =  volume_3to16[guestvolume].up16;
            dnvol_16 = -volume_3to16[guestvolume].up16;
    }   }
#endif

#ifdef WIN32
    if (bitrate == 16)
    {
#ifdef SCALEVOLUME
        upvol_16 = upvol_16 * hostvolume / 8;
        dnvol_16 = dnvol_16 * hostvolume / 8;
#endif
        upvol_h = (upvol_16 & 0xFF00) >> 8;
        upvol_l =  upvol_16 & 0x00FF;
        dnvol_h = (dnvol_16 & 0xFF00) >> 8;
        dnvol_l =  dnvol_16 & 0x00FF;
    } else
#endif
    {
#ifdef SCALEVOLUME
        if (bitrate == 8)
        {   upvol_8  = ((upvol_8 - 128) * hostvolume / 8) + 128;
            dnvol_8  = ((dnvol_8 - 128) * hostvolume / 8) + 128;
        }
#endif
}   }
