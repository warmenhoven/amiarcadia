// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
#endif
#ifdef WIN32
    #include "ibm.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aa.h"

// DEFINES----------------------------------------------------------------

#ifndef SEEK_SET
    #define SEEK_SET              0
#endif
#ifndef SEEK_CUR
    #define SEEK_CUR              1
#endif
#ifndef SEEK_END
    #define SEEK_END              2
#endif
#define MAX_MEM_LEVEL             9
#define DEF_MEM_LEVEL             8
#define MAX_WBITS                 15
#define DEF_WBITS                 MAX_WBITS
#define Z_STREAM_ERROR            (-2)
#define Z_DATA_ERROR              (-3)
#define Z_MEM_ERROR               (-4)
#define Z_BUF_ERROR               (-5)
#define Z_VERSION_ERROR           (-6)
#define Z_OK                      0
#define Z_NULL                    0
#define Z_STREAM_END              1
#define Z_NEED_DICT               2
#define Z_SYNC_FLUSH              2
#define Z_FINISH                  4
#define Z_DEFLATED                8
#define UNZ_ERRNO                 (-1)
#define UNZ_OK                    (0)
#define UNZ_EOF                   (0)
#define UNZ_END_OF_LIST_OF_FILE   (-100)
#define UNZ_PARAMERROR            (-102)
#define UNZ_BADZIPFILE            (-103)
#define UNZ_INTERNALERROR         (-104)
#define UNZ_CRCERROR              (-105)
#define PRESET_DICT               0x20
#define F_OPEN(name, mode)        fopen((name), (mode)
#define ZALLOC(strm, items, size) (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)         (*((strm)->zfree))((strm)->opaque, (void*)(addr))
#define TRY_FREE(s, p)            { if (p) ZFREE(s, p); }
#define MANY                      1440
#define UPDBITS {s->bitb=b;s->bitk=k;}
#define UPDIN {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;}
#define UPDOUT {s->write=q;}
#define UPDATE {UPDBITS UPDIN UPDOUT}
#define LEAVE {UPDATE return inflate_flush(s,z,r);}
#define LOADIN {p=z->next_in;n=z->avail_in;b=s->bitb;k=s->bitk;}
#define NEEDBYTE {if(n)r=Z_OK;else LEAVE}
#define NEXTBYTE (n--,*p++)
#define ZNEEDBITS(j) { while (k < (j)) { NEEDBYTE; b |= ((ULONG) NEXTBYTE) << k; k += 8; } }
#define DUMPBITS(j) {b>>=(j);k-=(j);}
#define WAVAIL (UINT)(q<s->read?s->read-q-1:s->end-q)
#define LOADOUT {q=s->write;m=(UINT)WAVAIL;}
#define WRAP {if(q==s->end&&s->read!=s->window){q=s->window;m=(UINT)WAVAIL;}}
#define FLUSH {UPDOUT r=inflate_flush(s,z,r); LOADOUT}
#define NEEDOUT {if(m==0){WRAP if(m==0){FLUSH WRAP if(m==0) LEAVE}}r=Z_OK;}
#define OUTBYTE(a) {*q++=(UBYTE)(a);m--;}
#define LOAD {LOADIN LOADOUT}
#define BMAX 15
#define UNZ_BUFSIZE (16384)
#define UNZ_MAXFILENAMEINZIP (256)
#define ALLOC(size) (malloc(size))
#define TRYFREE(p) {if (p) free(p);}
#define SIZECENTRALDIRITEM (0x2e)
#define SIZEZIPLOCALHEADER (0x1e)
#define exop word.what.Exop
#define bits word.what.Bits
#define BUFREADCOMMENT (0x400)
#define BASE 65521L
#define NMAX 5552
#define DO1b(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2b(buf,i)  DO1b(buf,i); DO1b(buf,i+1);
#define DO4b(buf,i)  DO2b(buf,i); DO2b(buf,i+2);
#define DO8b(buf,i)  DO4b(buf,i); DO4b(buf,i+4);
#define DO16b(buf)   DO8b(buf, 0);
#define C0 *p++ = 0;
#define C2 C0 C0 C0 C0
#define C4 C2 C2 C2 C2
#define GRABBITS(j) {while(k<(j)){b|=((ULONG)NEXTBYTE)<<k;k+=8;}}
#define UNGRAB {c=z->avail_in-n;c=(k>>3)<c?k>>3:c;n+=c;p-=c;k-=c<<3;}
#define NEEDBYTE2 {if(z->avail_in==0)return r;r=f;}
#define NEXTBYTE2 (z->avail_in--,z->total_in++,*z->next_in++)

// TYPEDEFS---------------------------------------------------------------

typedef void* (*alloc_func) (void* opaque, UINT items, UINT size);
typedef void  (*free_func)  (void* opaque, void* address);
typedef struct z_stream_s
{   UBYTE*                 next_in;
    UINT                   avail_in;
    ULONG                  total_in;
    UBYTE*                 next_out;
    UINT                   avail_out;
    ULONG                  total_out;
    char*                  msg;
    struct internal_state* state;
    alloc_func             zalloc;
    free_func              zfree;
    void*                  opaque;
    int                    data_type;
    ULONG                  adler;
    ULONG                  reserved;
} z_stream;
typedef void* gzFile;
typedef struct unz_global_info_s
{   ULONG number_entry;
    ULONG size_comment;
} unz_global_info;
typedef ULONG (*check_func)(ULONG check, const UBYTE *buf, UINT len);
typedef struct inflate_huft_s inflate_huft;
typedef struct inflate_blocks_state inflate_blocks_statef;
struct inflate_huft_s {
  union {
    struct {
      UBYTE Exop;
      UBYTE Bits;
    } what;
    UINT pad;
  } word;
  UINT base;
};
typedef struct inflate_codes_state inflate_codes_statef;
typedef enum {
      TYPE,
      LENS,
      STORED,
      TABLE,
      BTREE,
      DTREE,
      CODES,
      DRY,
      DONE,
      BAD}
inflate_block_mode;
struct inflate_blocks_state {
  inflate_block_mode  mode;

  union {
    UINT left;
    struct {
      UINT table;
      UINT index;
      UINT *blens;
      UINT bb;
      inflate_huft *tb;
    } trees;
    struct {
      inflate_codes_statef
         *codes;
    } decode;
  } sub;
  UINT last;

  UINT bitk;
  ULONG bitb;
  inflate_huft *hufts;
  UBYTE *window;
  UBYTE *end;
  UBYTE *read;
  UBYTE *write;
  check_func checkfn;
  ULONG check;
};
typedef struct unz_file_info_internal_s
{   ULONG offset_curfile;
} unz_file_info_internal;
typedef struct
{
        char  *read_buffer;
        z_stream stream;
        ULONG pos_in_zipfile;
        ULONG stream_initialised;
        ULONG offset_local_extrafield;
        UINT  size_local_extrafield;
        ULONG pos_local_extrafield;
        ULONG crc32;
        ULONG crc32_wait;
        ULONG rest_read_compressed;
        ULONG rest_read_uncompressed;
        FILE* file;
        ULONG compression_method;
        ULONG byte_before_the_zipfile;
} file_in_zip_read_info_s;
typedef struct
{
        FILE* file;
        unz_global_info gi;
        ULONG byte_before_the_zipfile;
        ULONG num_file;
        ULONG pos_in_central_dir;
        ULONG current_file_ok;
        ULONG central_pos;
        ULONG size_central_dir;
        ULONG offset_central_dir;
        unz_file_info cur_file_info;
        unz_file_info_internal cur_file_info_internal;
    file_in_zip_read_info_s* pfile_in_zip_read; } unz_s;
typedef enum {
      START,
      LEN,
      LENEXT,
      DIST,
      DISTEXT,
      COPY,
      LIT,
      WASH,
      END,
      BADCODE}
inflate_codes_mode;
struct inflate_codes_state {
  inflate_codes_mode mode;

  UINT len;
  union {
    struct {
      inflate_huft *tree;
      UINT need;
    } code;
    UINT lit;
    struct {
      UINT get;
      UINT dist;
    } copy;
  } sub;

  UBYTE lbits;
  UBYTE dbits;
  inflate_huft *ltree;
  inflate_huft *dtree;
};
typedef enum {
      METHOD,
      ZFLAG,
      DICT4,
      DICT3,
      DICT2,
      DICT1,
      DICT0,
      BLOCKS,
      CHECK4,
      CHECK3,
      CHECK2,
      CHECK1,
      DONE2,
      BAD2}
inflate_mode;
struct internal_state {
  inflate_mode  mode;

  union {
    UINT method;
    struct {
      ULONG was;
      ULONG need;
    } check;
    UINT marker;
  } sub;

  int  nowrap;
  UINT wbits;
  inflate_blocks_statef
    *blocks;
};

// FUNCTIONS--------------------------------------------------------------

MODULE ULONG adler32(ULONG adler, const UBYTE* buf, UINT len);
MODULE int inflate(z_stream* strm, int flush);
MODULE int inflateEnd(z_stream* strm);
MODULE int inflateReset(z_stream* strm);
MODULE int inflateInit(z_stream* z, int w, int stream_size);
MODULE inflate_codes_statef* inflate_codes_new(UINT, UINT, inflate_huft*, inflate_huft*, z_stream*);
MODULE int inflate_codes(inflate_blocks_statef*, z_stream*, int);
MODULE void inflate_codes_free(inflate_codes_statef*, z_stream*);
MODULE int inflate_fast(UINT, UINT, inflate_huft*, inflate_huft*, inflate_blocks_statef*, z_stream*);
MODULE inflate_blocks_statef* inflate_blocks_new(z_stream* z, check_func c, UINT w);
MODULE int inflate_blocks(inflate_blocks_statef*, z_stream*, int);
MODULE void inflate_blocks_reset(inflate_blocks_statef*, z_stream*, ULONG*);
MODULE int inflate_blocks_free(inflate_blocks_statef*, z_stream*);
MODULE int inflate_trees_bits(UINT*, UINT*, inflate_huft**, inflate_huft*, z_stream*);
MODULE int inflate_trees_dynamic(UINT, UINT, UINT*, UINT*, UINT*, inflate_huft**, inflate_huft**, inflate_huft*, z_stream*);
MODULE int inflate_trees_fixed(UINT*, UINT*, inflate_huft**, inflate_huft**);
MODULE int inflate_flush(inflate_blocks_statef*, z_stream*, int);
MODULE int unzlocal_CheckCurrentFileCoherencyHeader(unz_s* s, UINT* piSizeVar, ULONG* poffset_local_extrafield, UINT* psize_local_extrafield);
MODULE int unzlocal_getByte(FILE* fin, int* pi);
MODULE int unzlocal_getShort(FILE* fin, ULONG* pX);
MODULE int unzlocal_getLong(FILE* fin, ULONG* pX);
MODULE int unzlocal_GetCurrentFileInfoInternal(unzFile file, unz_file_info* pfile_info, unz_file_info_internal* pfile_info_internal, char* szFileName, ULONG fileNameBufferSize, void* extraField, ULONG extraFieldBufferSize, char* szComment, ULONG commentBufferSize);
MODULE ULONG unzlocal_SearchCentralDir(FILE* fin);
MODULE void* zcalloc(void* opaque, unsigned items, unsigned size);
MODULE void zcfree(UNUSED void* opaque, void* ptr);

// VARIABLES--------------------------------------------------------------

MODULE UINT fixed_bl = 9;
MODULE UINT fixed_bd = 5;
MODULE inflate_huft fixed_tl[] = {
    {{{96,7}},256}, {{{0,8}},80}, {{{0,8}},16}, {{{84,8}},115},
    {{{82,7}},31}, {{{0,8}},112}, {{{0,8}},48}, {{{0,9}},192},
    {{{80,7}},10}, {{{0,8}},96}, {{{0,8}},32}, {{{0,9}},160},
    {{{0,8}},0}, {{{0,8}},128}, {{{0,8}},64}, {{{0,9}},224},
    {{{80,7}},6}, {{{0,8}},88}, {{{0,8}},24}, {{{0,9}},144},
    {{{83,7}},59}, {{{0,8}},120}, {{{0,8}},56}, {{{0,9}},208},
    {{{81,7}},17}, {{{0,8}},104}, {{{0,8}},40}, {{{0,9}},176},
    {{{0,8}},8}, {{{0,8}},136}, {{{0,8}},72}, {{{0,9}},240},
    {{{80,7}},4}, {{{0,8}},84}, {{{0,8}},20}, {{{85,8}},227},
    {{{83,7}},43}, {{{0,8}},116}, {{{0,8}},52}, {{{0,9}},200},
    {{{81,7}},13}, {{{0,8}},100}, {{{0,8}},36}, {{{0,9}},168},
    {{{0,8}},4}, {{{0,8}},132}, {{{0,8}},68}, {{{0,9}},232},
    {{{80,7}},8}, {{{0,8}},92}, {{{0,8}},28}, {{{0,9}},152},
    {{{84,7}},83}, {{{0,8}},124}, {{{0,8}},60}, {{{0,9}},216},
    {{{82,7}},23}, {{{0,8}},108}, {{{0,8}},44}, {{{0,9}},184},
    {{{0,8}},12}, {{{0,8}},140}, {{{0,8}},76}, {{{0,9}},248},
    {{{80,7}},3}, {{{0,8}},82}, {{{0,8}},18}, {{{85,8}},163},
    {{{83,7}},35}, {{{0,8}},114}, {{{0,8}},50}, {{{0,9}},196},
    {{{81,7}},11}, {{{0,8}},98}, {{{0,8}},34}, {{{0,9}},164},
    {{{0,8}},2}, {{{0,8}},130}, {{{0,8}},66}, {{{0,9}},228},
    {{{80,7}},7}, {{{0,8}},90}, {{{0,8}},26}, {{{0,9}},148},
    {{{84,7}},67}, {{{0,8}},122}, {{{0,8}},58}, {{{0,9}},212},
    {{{82,7}},19}, {{{0,8}},106}, {{{0,8}},42}, {{{0,9}},180},
    {{{0,8}},10}, {{{0,8}},138}, {{{0,8}},74}, {{{0,9}},244},
    {{{80,7}},5}, {{{0,8}},86}, {{{0,8}},22}, {{{192,8}},0},
    {{{83,7}},51}, {{{0,8}},118}, {{{0,8}},54}, {{{0,9}},204},
    {{{81,7}},15}, {{{0,8}},102}, {{{0,8}},38}, {{{0,9}},172},
    {{{0,8}},6}, {{{0,8}},134}, {{{0,8}},70}, {{{0,9}},236},
    {{{80,7}},9}, {{{0,8}},94}, {{{0,8}},30}, {{{0,9}},156},
    {{{84,7}},99}, {{{0,8}},126}, {{{0,8}},62}, {{{0,9}},220},
    {{{82,7}},27}, {{{0,8}},110}, {{{0,8}},46}, {{{0,9}},188},
    {{{0,8}},14}, {{{0,8}},142}, {{{0,8}},78}, {{{0,9}},252},
    {{{96,7}},256}, {{{0,8}},81}, {{{0,8}},17}, {{{85,8}},131},
    {{{82,7}},31}, {{{0,8}},113}, {{{0,8}},49}, {{{0,9}},194},
    {{{80,7}},10}, {{{0,8}},97}, {{{0,8}},33}, {{{0,9}},162},
    {{{0,8}},1}, {{{0,8}},129}, {{{0,8}},65}, {{{0,9}},226},
    {{{80,7}},6}, {{{0,8}},89}, {{{0,8}},25}, {{{0,9}},146},
    {{{83,7}},59}, {{{0,8}},121}, {{{0,8}},57}, {{{0,9}},210},
    {{{81,7}},17}, {{{0,8}},105}, {{{0,8}},41}, {{{0,9}},178},
    {{{0,8}},9}, {{{0,8}},137}, {{{0,8}},73}, {{{0,9}},242},
    {{{80,7}},4}, {{{0,8}},85}, {{{0,8}},21}, {{{80,8}},258},
    {{{83,7}},43}, {{{0,8}},117}, {{{0,8}},53}, {{{0,9}},202},
    {{{81,7}},13}, {{{0,8}},101}, {{{0,8}},37}, {{{0,9}},170},
    {{{0,8}},5}, {{{0,8}},133}, {{{0,8}},69}, {{{0,9}},234},
    {{{80,7}},8}, {{{0,8}},93}, {{{0,8}},29}, {{{0,9}},154},
    {{{84,7}},83}, {{{0,8}},125}, {{{0,8}},61}, {{{0,9}},218},
    {{{82,7}},23}, {{{0,8}},109}, {{{0,8}},45}, {{{0,9}},186},
    {{{0,8}},13}, {{{0,8}},141}, {{{0,8}},77}, {{{0,9}},250},
    {{{80,7}},3}, {{{0,8}},83}, {{{0,8}},19}, {{{85,8}},195},
    {{{83,7}},35}, {{{0,8}},115}, {{{0,8}},51}, {{{0,9}},198},
    {{{81,7}},11}, {{{0,8}},99}, {{{0,8}},35}, {{{0,9}},166},
    {{{0,8}},3}, {{{0,8}},131}, {{{0,8}},67}, {{{0,9}},230},
    {{{80,7}},7}, {{{0,8}},91}, {{{0,8}},27}, {{{0,9}},150},
    {{{84,7}},67}, {{{0,8}},123}, {{{0,8}},59}, {{{0,9}},214},
    {{{82,7}},19}, {{{0,8}},107}, {{{0,8}},43}, {{{0,9}},182},
    {{{0,8}},11}, {{{0,8}},139}, {{{0,8}},75}, {{{0,9}},246},
    {{{80,7}},5}, {{{0,8}},87}, {{{0,8}},23}, {{{192,8}},0},
    {{{83,7}},51}, {{{0,8}},119}, {{{0,8}},55}, {{{0,9}},206},
    {{{81,7}},15}, {{{0,8}},103}, {{{0,8}},39}, {{{0,9}},174},
    {{{0,8}},7}, {{{0,8}},135}, {{{0,8}},71}, {{{0,9}},238},
    {{{80,7}},9}, {{{0,8}},95}, {{{0,8}},31}, {{{0,9}},158},
    {{{84,7}},99}, {{{0,8}},127}, {{{0,8}},63}, {{{0,9}},222},
    {{{82,7}},27}, {{{0,8}},111}, {{{0,8}},47}, {{{0,9}},190},
    {{{0,8}},15}, {{{0,8}},143}, {{{0,8}},79}, {{{0,9}},254},
    {{{96,7}},256}, {{{0,8}},80}, {{{0,8}},16}, {{{84,8}},115},
    {{{82,7}},31}, {{{0,8}},112}, {{{0,8}},48}, {{{0,9}},193},
    {{{80,7}},10}, {{{0,8}},96}, {{{0,8}},32}, {{{0,9}},161},
    {{{0,8}},0}, {{{0,8}},128}, {{{0,8}},64}, {{{0,9}},225},
    {{{80,7}},6}, {{{0,8}},88}, {{{0,8}},24}, {{{0,9}},145},
    {{{83,7}},59}, {{{0,8}},120}, {{{0,8}},56}, {{{0,9}},209},
    {{{81,7}},17}, {{{0,8}},104}, {{{0,8}},40}, {{{0,9}},177},
    {{{0,8}},8}, {{{0,8}},136}, {{{0,8}},72}, {{{0,9}},241},
    {{{80,7}},4}, {{{0,8}},84}, {{{0,8}},20}, {{{85,8}},227},
    {{{83,7}},43}, {{{0,8}},116}, {{{0,8}},52}, {{{0,9}},201},
    {{{81,7}},13}, {{{0,8}},100}, {{{0,8}},36}, {{{0,9}},169},
    {{{0,8}},4}, {{{0,8}},132}, {{{0,8}},68}, {{{0,9}},233},
    {{{80,7}},8}, {{{0,8}},92}, {{{0,8}},28}, {{{0,9}},153},
    {{{84,7}},83}, {{{0,8}},124}, {{{0,8}},60}, {{{0,9}},217},
    {{{82,7}},23}, {{{0,8}},108}, {{{0,8}},44}, {{{0,9}},185},
    {{{0,8}},12}, {{{0,8}},140}, {{{0,8}},76}, {{{0,9}},249},
    {{{80,7}},3}, {{{0,8}},82}, {{{0,8}},18}, {{{85,8}},163},
    {{{83,7}},35}, {{{0,8}},114}, {{{0,8}},50}, {{{0,9}},197},
    {{{81,7}},11}, {{{0,8}},98}, {{{0,8}},34}, {{{0,9}},165},
    {{{0,8}},2}, {{{0,8}},130}, {{{0,8}},66}, {{{0,9}},229},
    {{{80,7}},7}, {{{0,8}},90}, {{{0,8}},26}, {{{0,9}},149},
    {{{84,7}},67}, {{{0,8}},122}, {{{0,8}},58}, {{{0,9}},213},
    {{{82,7}},19}, {{{0,8}},106}, {{{0,8}},42}, {{{0,9}},181},
    {{{0,8}},10}, {{{0,8}},138}, {{{0,8}},74}, {{{0,9}},245},
    {{{80,7}},5}, {{{0,8}},86}, {{{0,8}},22}, {{{192,8}},0},
    {{{83,7}},51}, {{{0,8}},118}, {{{0,8}},54}, {{{0,9}},205},
    {{{81,7}},15}, {{{0,8}},102}, {{{0,8}},38}, {{{0,9}},173},
    {{{0,8}},6}, {{{0,8}},134}, {{{0,8}},70}, {{{0,9}},237},
    {{{80,7}},9}, {{{0,8}},94}, {{{0,8}},30}, {{{0,9}},157},
    {{{84,7}},99}, {{{0,8}},126}, {{{0,8}},62}, {{{0,9}},221},
    {{{82,7}},27}, {{{0,8}},110}, {{{0,8}},46}, {{{0,9}},189},
    {{{0,8}},14}, {{{0,8}},142}, {{{0,8}},78}, {{{0,9}},253},
    {{{96,7}},256}, {{{0,8}},81}, {{{0,8}},17}, {{{85,8}},131},
    {{{82,7}},31}, {{{0,8}},113}, {{{0,8}},49}, {{{0,9}},195},
    {{{80,7}},10}, {{{0,8}},97}, {{{0,8}},33}, {{{0,9}},163},
    {{{0,8}},1}, {{{0,8}},129}, {{{0,8}},65}, {{{0,9}},227},
    {{{80,7}},6}, {{{0,8}},89}, {{{0,8}},25}, {{{0,9}},147},
    {{{83,7}},59}, {{{0,8}},121}, {{{0,8}},57}, {{{0,9}},211},
    {{{81,7}},17}, {{{0,8}},105}, {{{0,8}},41}, {{{0,9}},179},
    {{{0,8}},9}, {{{0,8}},137}, {{{0,8}},73}, {{{0,9}},243},
    {{{80,7}},4}, {{{0,8}},85}, {{{0,8}},21}, {{{80,8}},258},
    {{{83,7}},43}, {{{0,8}},117}, {{{0,8}},53}, {{{0,9}},203},
    {{{81,7}},13}, {{{0,8}},101}, {{{0,8}},37}, {{{0,9}},171},
    {{{0,8}},5}, {{{0,8}},133}, {{{0,8}},69}, {{{0,9}},235},
    {{{80,7}},8}, {{{0,8}},93}, {{{0,8}},29}, {{{0,9}},155},
    {{{84,7}},83}, {{{0,8}},125}, {{{0,8}},61}, {{{0,9}},219},
    {{{82,7}},23}, {{{0,8}},109}, {{{0,8}},45}, {{{0,9}},187},
    {{{0,8}},13}, {{{0,8}},141}, {{{0,8}},77}, {{{0,9}},251},
    {{{80,7}},3}, {{{0,8}},83}, {{{0,8}},19}, {{{85,8}},195},
    {{{83,7}},35}, {{{0,8}},115}, {{{0,8}},51}, {{{0,9}},199},
    {{{81,7}},11}, {{{0,8}},99}, {{{0,8}},35}, {{{0,9}},167},
    {{{0,8}},3}, {{{0,8}},131}, {{{0,8}},67}, {{{0,9}},231},
    {{{80,7}},7}, {{{0,8}},91}, {{{0,8}},27}, {{{0,9}},151},
    {{{84,7}},67}, {{{0,8}},123}, {{{0,8}},59}, {{{0,9}},215},
    {{{82,7}},19}, {{{0,8}},107}, {{{0,8}},43}, {{{0,9}},183},
    {{{0,8}},11}, {{{0,8}},139}, {{{0,8}},75}, {{{0,9}},247},
    {{{80,7}},5}, {{{0,8}},87}, {{{0,8}},23}, {{{192,8}},0},
    {{{83,7}},51}, {{{0,8}},119}, {{{0,8}},55}, {{{0,9}},207},
    {{{81,7}},15}, {{{0,8}},103}, {{{0,8}},39}, {{{0,9}},175},
    {{{0,8}},7}, {{{0,8}},135}, {{{0,8}},71}, {{{0,9}},239},
    {{{80,7}},9}, {{{0,8}},95}, {{{0,8}},31}, {{{0,9}},159},
    {{{84,7}},99}, {{{0,8}},127}, {{{0,8}},63}, {{{0,9}},223},
    {{{82,7}},27}, {{{0,8}},111}, {{{0,8}},47}, {{{0,9}},191},
    {{{0,8}},15}, {{{0,8}},143}, {{{0,8}},79}, {{{0,9}},255}
  };
MODULE inflate_huft fixed_td[] = {
    {{{80,5}},1}, {{{87,5}},257}, {{{83,5}},17}, {{{91,5}},4097},
    {{{81,5}},5}, {{{89,5}},1025}, {{{85,5}},65}, {{{93,5}},16385},
    {{{80,5}},3}, {{{88,5}},513}, {{{84,5}},33}, {{{92,5}},8193},
    {{{82,5}},9}, {{{90,5}},2049}, {{{86,5}},129}, {{{192,5}},24577},
    {{{80,5}},2}, {{{87,5}},385}, {{{83,5}},25}, {{{91,5}},6145},
    {{{81,5}},7}, {{{89,5}},1537}, {{{85,5}},97}, {{{93,5}},24577},
    {{{80,5}},4}, {{{88,5}},769}, {{{84,5}},49}, {{{92,5}},12289},
    {{{82,5}},13}, {{{90,5}},3073}, {{{86,5}},193}, {{{192,5}},24577}
  };
MODULE int huft_build(
    UINT *,
    UINT,
    UINT,
    const UINT *,
    const UINT *,
    inflate_huft **,
    UINT *,
    inflate_huft *,
    UINT *,
    UINT *);
MODULE const UINT cplens[31] = {
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
        35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};

MODULE const UINT cplext[31] = {
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
        3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 112, 112};
MODULE const UINT cpdist[30] = {
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
        257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
        8193, 12289, 16385, 24577};
MODULE const UINT cpdext[30] = {
        0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
        7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
        12, 12, 13, 13};
MODULE const char unz_copyright[] =
   " unzip 0.15 Copyright 1998 Gilles Vollant ";
MODULE UINT inflate_mask[17] = {
    0x0000,
    0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
    0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};
MODULE const UINT border[] = {
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

// EXPORTED CODE----------------------------------------------------------

EXPORT int unzCloseCurrentFile(unzFile file)
{
        int err=UNZ_OK;
        unz_s* s;
        file_in_zip_read_info_s* pfile_in_zip_read_info;
        if (file==NULL)
                return UNZ_PARAMERROR;
        s=(unz_s*)file;
    pfile_in_zip_read_info=s->pfile_in_zip_read;
        if (pfile_in_zip_read_info==NULL)
                return UNZ_PARAMERROR;
        if (pfile_in_zip_read_info->rest_read_uncompressed == 0)
        {
                if (pfile_in_zip_read_info->crc32 != pfile_in_zip_read_info->crc32_wait)
                        err=UNZ_CRCERROR;
        }
        TRYFREE(pfile_in_zip_read_info->read_buffer);
        pfile_in_zip_read_info->read_buffer = NULL;
        if (pfile_in_zip_read_info->stream_initialised)
                inflateEnd(&pfile_in_zip_read_info->stream);
        pfile_in_zip_read_info->stream_initialised = 0;
        TRYFREE(pfile_in_zip_read_info);
    s->pfile_in_zip_read=NULL;
        return err;
}

EXPORT int unzReadCurrentFile(
        unzFile file,
        void* buf,
        unsigned len
) {
        int err=UNZ_OK;
        UINT iRead = 0;
        unz_s* s;
        file_in_zip_read_info_s* pfile_in_zip_read_info;
        if (file==NULL)
                return UNZ_PARAMERROR;
        s=(unz_s*)file;
        pfile_in_zip_read_info=s->pfile_in_zip_read;
        if (pfile_in_zip_read_info==NULL)
                return UNZ_PARAMERROR;
        if (pfile_in_zip_read_info->read_buffer == NULL)
                return UNZ_END_OF_LIST_OF_FILE;
        if (len==0)
                return 0;
        pfile_in_zip_read_info->stream.next_out = (UBYTE*)buf;
        pfile_in_zip_read_info->stream.avail_out = (UINT)len;
        if (len>pfile_in_zip_read_info->rest_read_uncompressed)
                pfile_in_zip_read_info->stream.avail_out =
                  (UINT)pfile_in_zip_read_info->rest_read_uncompressed;
        while (pfile_in_zip_read_info->stream.avail_out>0)
        {
                if ((pfile_in_zip_read_info->stream.avail_in==0) &&
            (pfile_in_zip_read_info->rest_read_compressed>0))
                {
                        UINT uReadThis = UNZ_BUFSIZE;
                        if (pfile_in_zip_read_info->rest_read_compressed<uReadThis)
                                uReadThis = (UINT)pfile_in_zip_read_info->rest_read_compressed;
                        if (uReadThis == 0)
                                return UNZ_EOF;
                        if (fseek(pfile_in_zip_read_info->file,
                      pfile_in_zip_read_info->pos_in_zipfile +
                         pfile_in_zip_read_info->byte_before_the_zipfile,SEEK_SET)!=0)
                                return UNZ_ERRNO;
                        if (fread(pfile_in_zip_read_info->read_buffer,uReadThis,1,
                         pfile_in_zip_read_info->file)!=1)
                                return UNZ_ERRNO;
                        pfile_in_zip_read_info->pos_in_zipfile += uReadThis;
                        pfile_in_zip_read_info->rest_read_compressed-=uReadThis;
                        pfile_in_zip_read_info->stream.next_in =
                (UBYTE*)pfile_in_zip_read_info->read_buffer;
                        pfile_in_zip_read_info->stream.avail_in = (UINT)uReadThis;
                }
                if (pfile_in_zip_read_info->compression_method==0)
                {
                        UINT uDoCopy,i ;
                        if (pfile_in_zip_read_info->stream.avail_out <
                            pfile_in_zip_read_info->stream.avail_in)
                                uDoCopy = pfile_in_zip_read_info->stream.avail_out ;
                        else
                                uDoCopy = pfile_in_zip_read_info->stream.avail_in ;
                        for (i=0;i<uDoCopy;i++)
                                *(pfile_in_zip_read_info->stream.next_out+i) =
                        *(pfile_in_zip_read_info->stream.next_in+i);
                        pfile_in_zip_read_info->crc32 = crc32(pfile_in_zip_read_info->stream.next_out, uDoCopy);
                        pfile_in_zip_read_info->rest_read_uncompressed-=uDoCopy;
                        pfile_in_zip_read_info->stream.avail_in -= uDoCopy;
                        pfile_in_zip_read_info->stream.avail_out -= uDoCopy;
                        pfile_in_zip_read_info->stream.next_out += uDoCopy;
                        pfile_in_zip_read_info->stream.next_in += uDoCopy;
            pfile_in_zip_read_info->stream.total_out += uDoCopy;
                        iRead += uDoCopy;
                }
                else
                {
                        ULONG uTotalOutBefore,uTotalOutAfter;
                        const UBYTE *bufBefore;
                        ULONG uOutThis;
                        int flush=Z_SYNC_FLUSH;
                        uTotalOutBefore = pfile_in_zip_read_info->stream.total_out;
                        bufBefore = pfile_in_zip_read_info->stream.next_out;
                                                err=inflate(&pfile_in_zip_read_info->stream,flush);
                        uTotalOutAfter = pfile_in_zip_read_info->stream.total_out;
                        uOutThis = uTotalOutAfter-uTotalOutBefore;
                        pfile_in_zip_read_info->crc32 = crc32((UBYTE*) bufBefore, uOutThis);
                        pfile_in_zip_read_info->rest_read_uncompressed -=
                uOutThis;
                        iRead += (UINT)(uTotalOutAfter - uTotalOutBefore);
                        if (err==Z_STREAM_END)
                                return (iRead==0) ? UNZ_EOF : (int) iRead;
                        if (err!=Z_OK)
                                break;
                }
        }
        if (err==Z_OK)
                return ((int) iRead);
        return err;
}

EXPORT int unzOpenCurrentFile(unzFile file)
{
    int err  ;
    int Store;
    UINT iSizeVar;
    unz_s* s;
    file_in_zip_read_info_s* pfile_in_zip_read_info;
    ULONG offset_local_extrafield;
    UINT  size_local_extrafield;
    if (file==NULL)
            return UNZ_PARAMERROR;
    s=(unz_s*)file;
    if (!s->current_file_ok)
            return UNZ_PARAMERROR;
    if (s->pfile_in_zip_read != NULL)
    unzCloseCurrentFile(file);
    if (unzlocal_CheckCurrentFileCoherencyHeader(s,&iSizeVar,
                            &offset_local_extrafield,&size_local_extrafield)!=UNZ_OK)
            return UNZ_BADZIPFILE;
    pfile_in_zip_read_info = (file_in_zip_read_info_s*) ALLOC(sizeof(file_in_zip_read_info_s));
    if (pfile_in_zip_read_info==NULL)
            return UNZ_INTERNALERROR;
    pfile_in_zip_read_info->read_buffer=(char*)ALLOC(UNZ_BUFSIZE);
    pfile_in_zip_read_info->offset_local_extrafield = offset_local_extrafield;
    pfile_in_zip_read_info->size_local_extrafield = size_local_extrafield;
    pfile_in_zip_read_info->pos_local_extrafield=0;
    if (pfile_in_zip_read_info->read_buffer==NULL)
    {
            TRYFREE(pfile_in_zip_read_info);
            return UNZ_INTERNALERROR;
    }
    pfile_in_zip_read_info->stream_initialised=0;
         Store = s->cur_file_info.compression_method==0;
    pfile_in_zip_read_info->crc32_wait=s->cur_file_info.crc;
    pfile_in_zip_read_info->crc32=0;
    pfile_in_zip_read_info->compression_method =
        s->cur_file_info.compression_method;
    pfile_in_zip_read_info->file=s->file;
    pfile_in_zip_read_info->byte_before_the_zipfile=s->byte_before_the_zipfile;
    pfile_in_zip_read_info->stream.total_out = 0;
    if (!Store)
    {   pfile_in_zip_read_info->stream.zalloc = (alloc_func) 0;
        pfile_in_zip_read_info->stream.zfree  = (free_func)  0;
        pfile_in_zip_read_info->stream.opaque = (void*)      0;
        err = inflateInit(&pfile_in_zip_read_info->stream, -MAX_WBITS, sizeof(z_stream));
        if (err == Z_OK)
            pfile_in_zip_read_info->stream_initialised = 1;
    }
    pfile_in_zip_read_info->rest_read_compressed =
        s->cur_file_info.compressed_size ;
    pfile_in_zip_read_info->rest_read_uncompressed =
        s->cur_file_info.uncompressed_size ;
    pfile_in_zip_read_info->pos_in_zipfile =
        s->cur_file_info_internal.offset_curfile + SIZEZIPLOCALHEADER +
                      iSizeVar;
    pfile_in_zip_read_info->stream.avail_in = (UINT)0;
    s->pfile_in_zip_read = pfile_in_zip_read_info;
    return UNZ_OK;
}

EXPORT int unzGoToNextFile(unzFile file)
{   unz_s* s;
    int    err;

    if (file==NULL)
    {   return UNZ_PARAMERROR;
    }
    s = (unz_s*) file;
    if (!s->current_file_ok)
    {   return UNZ_END_OF_LIST_OF_FILE;
    }
    if (s->num_file+1 == s->gi.number_entry)
    {   return UNZ_END_OF_LIST_OF_FILE;
    }
    s->pos_in_central_dir += SIZECENTRALDIRITEM
                          +  s->cur_file_info.size_filename
                          +  s->cur_file_info.size_file_extra
                          +  s->cur_file_info.size_file_comment;
    s->num_file++;
    err = unzlocal_GetCurrentFileInfoInternal
    (   file,
        &s->cur_file_info,
        &s->cur_file_info_internal,
        NULL,
        0,
        NULL,
        0,
        NULL,
        0
    );
    s->current_file_ok = (err == UNZ_OK);

    return err;
}

EXPORT int unzGetCurrentFileInfo(unzFile file, unz_file_info* pfile_info, char* szFileName, ULONG fileNameBufferSize, void* extraField, ULONG extraFieldBufferSize, char* szComment, ULONG commentBufferSize)
{   return unzlocal_GetCurrentFileInfoInternal
    (   file,
        pfile_info,
        NULL,
        szFileName,
        fileNameBufferSize,
        extraField,
        extraFieldBufferSize,
        szComment,
        commentBufferSize
    );
}

EXPORT int unzClose(unzFile file)
{   unz_s* s;

    if (file == NULL)
    {   return UNZ_PARAMERROR;
    }
    s = (unz_s*) file;
    if (s->pfile_in_zip_read != NULL)
    {   unzCloseCurrentFile(file);
    }
    fclose(s->file);
    TRYFREE(s);
    return UNZ_OK;
}

EXPORT int unzGoToFirstFile(unzFile file)
{   int    err;
    unz_s* s;

    if (file == NULL)
    {   return UNZ_PARAMERROR;
    }
    s = (unz_s*) file;
    s->pos_in_central_dir = s->offset_central_dir;
    s->num_file = 0;
    err = unzlocal_GetCurrentFileInfoInternal
    (   file,
        &s->cur_file_info,
        &s->cur_file_info_internal,
        NULL,
        0,
        NULL,
        0,
        NULL,
        0
    );
    s->current_file_ok = (err == UNZ_OK);
    return err;
}

EXPORT unzFile unzOpen(const char* path)
{   unz_s  us;
    unz_s* s;
    ULONG  central_pos,
           uL;
    FILE*  fin ;
    ULONG  number_disk;
    ULONG  number_disk_with_CD;
    ULONG  number_entry_CD;
    int    err = UNZ_OK;

    if (unz_copyright[0] != ' ')
        return NULL;

    fin = fopen(path,"rb");
    if (fin == NULL)
        return NULL;

    central_pos = unzlocal_SearchCentralDir(fin);
    if (central_pos == 0)                                                                           err = UNZ_ERRNO;
    if (fseek(fin,central_pos, SEEK_SET) != 0)                                                      err = UNZ_ERRNO;
    if (unzlocal_getLong(fin, &uL) != UNZ_OK)                                                       err = UNZ_ERRNO;
    if (unzlocal_getShort(fin, &number_disk) != UNZ_OK)                                             err = UNZ_ERRNO;
    if (unzlocal_getShort(fin, &number_disk_with_CD) != UNZ_OK)                                     err = UNZ_ERRNO;
    if (unzlocal_getShort(fin, &us.gi.number_entry) != UNZ_OK)                                      err = UNZ_ERRNO;
    if (unzlocal_getShort(fin, &number_entry_CD) != UNZ_OK)                                         err = UNZ_ERRNO;
    if ((number_entry_CD != us.gi.number_entry) || (number_disk_with_CD != 0) || (number_disk !=0)) err = UNZ_BADZIPFILE;
    if (unzlocal_getLong(fin, &us.size_central_dir) != UNZ_OK)                                      err = UNZ_ERRNO;
    if (unzlocal_getLong(fin, &us.offset_central_dir) != UNZ_OK)                                    err = UNZ_ERRNO;
    if (unzlocal_getShort(fin, &us.gi.size_comment) != UNZ_OK)                                      err = UNZ_ERRNO;
    if (central_pos < us.offset_central_dir + us.size_central_dir && err == UNZ_OK)                 err = UNZ_BADZIPFILE;

    if (err != UNZ_OK)
    {   fclose(fin);
        return NULL;
    }

    us.file = fin;
    us.byte_before_the_zipfile = central_pos - (us.offset_central_dir + us.size_central_dir);
    us.central_pos = central_pos;
    us.pfile_in_zip_read = NULL;
    s = (unz_s*) ALLOC(sizeof(unz_s));
    *s = us;
    unzGoToFirstFile((unzFile) s);
    return (unzFile) s;
}

// MODULE CODE------------------------------------------------------------

MODULE int unzlocal_getByte(FILE* fin, int* pi)
{   unsigned char c;
    int           err = fread(&c, 1, 1, fin);

    if (err==1)
    {   *pi = (int) c;
        return UNZ_OK;
    } else
    {   if (ferror(fin))
        {   return UNZ_ERRNO;
        } else
        {   return UNZ_EOF;
}   }   }

MODULE int unzlocal_getShort(FILE* fin, ULONG* pX)
{   ULONG x;
    int   i = 0;
    int   err;

    err = unzlocal_getByte(fin, &i);
    x = (ULONG) i;
    if (err == UNZ_OK)
        err = unzlocal_getByte(fin, &i);
    x += ((ULONG) i) << 8;
    if (err == UNZ_OK)
        *pX = x;
    else
        *pX = 0;

    return err;
}

MODULE int unzlocal_getLong(FILE* fin, ULONG* pX)
{   ULONG x;
    int   i = 0;
    int   err;

    err = unzlocal_getByte(fin, &i);
    x = (ULONG) i;
    if (err == UNZ_OK)
        err = unzlocal_getByte(fin, &i);
    x += ((ULONG) i) << 8;
    if (err == UNZ_OK)
        err = unzlocal_getByte(fin, &i);
    x += ((ULONG) i) << 16;
    if (err == UNZ_OK)
        err = unzlocal_getByte(fin, &i);
    x += ((ULONG) i) << 24;
    if (err == UNZ_OK)
        *pX = x;
    else
        *pX = 0;

    return err;
}

MODULE ULONG unzlocal_SearchCentralDir(FILE* fin)
{
        unsigned char* buf;
        ULONG uSizeFile;
        ULONG uBackRead;
        ULONG uMaxBack=0xffff;
        ULONG uPosFound=0;
        if (fseek(fin,0,SEEK_END) != 0)
                return 0;
        uSizeFile = ftell( fin );
        if (uMaxBack>uSizeFile)
                uMaxBack = uSizeFile;
        buf = (unsigned char*)ALLOC(BUFREADCOMMENT+4);
        if (buf==NULL)
                return 0;
        uBackRead = 4;
        while (uBackRead<uMaxBack)
        {
                ULONG uReadSize,uReadPos ;
                int i;
                if (uBackRead+BUFREADCOMMENT>uMaxBack)
                        uBackRead = uMaxBack;
                else
                        uBackRead+=BUFREADCOMMENT;
                uReadPos = uSizeFile-uBackRead ;
                uReadSize = ((BUFREADCOMMENT+4) < (uSizeFile-uReadPos)) ?
                     (BUFREADCOMMENT+4) : (uSizeFile-uReadPos);
                if (fseek(fin,uReadPos,SEEK_SET)!=0)
                        break;
                if (fread(buf,(UINT)uReadSize,1,fin)!=1)
                        break;
                for (i=(int)uReadSize-3; (i--)>0;)
                        if (((*(buf+i))==0x50) && ((*(buf+i+1))==0x4b) &&
                                ((*(buf+i+2))==0x05) && ((*(buf+i+3))==0x06))
                        {
                                uPosFound = uReadPos+i;
                                break;
                        }
                if (uPosFound!=0)
                        break;
        }
        TRYFREE(buf);
        return uPosFound;
}

MODULE void unzlocal_DosDateToTmuDate(ULONG ulDosDate, tm_unz* ptm)
{   ULONG uDate;

    uDate = (ULONG)(ulDosDate>>16);
    ptm->tm_mday = (UINT)(uDate&0x1f) ;
    ptm->tm_mon =  (UINT)((((uDate)&0x1E0)/0x20)-1) ;
    ptm->tm_year = (UINT)(((uDate&0x0FE00)/0x0200)+1980) ;
    ptm->tm_hour = (UINT) ((ulDosDate &0xF800)/0x800);
    ptm->tm_min =  (UINT) ((ulDosDate&0x7E0)/0x20) ;
    ptm->tm_sec =  (UINT) (2*(ulDosDate&0x1f)) ;
}

MODULE int unzlocal_GetCurrentFileInfoInternal(
        unzFile file,
        unz_file_info *pfile_info,
        unz_file_info_internal *pfile_info_internal,
        char *szFileName,
        ULONG fileNameBufferSize,
        void *extraField,
        ULONG extraFieldBufferSize,
        char *szComment,
        ULONG commentBufferSize
) {
        unz_s* s;
        unz_file_info file_info;
        unz_file_info_internal file_info_internal;
        int err=UNZ_OK;
        ULONG uMagic;
        long lSeek=0;
        if (file==NULL)
                return UNZ_PARAMERROR;
        s=(unz_s*)file;
        if (fseek(s->file,s->pos_in_central_dir+s->byte_before_the_zipfile,SEEK_SET)!=0)
                err=UNZ_ERRNO;

        if (err==UNZ_OK)
     {
        if (unzlocal_getLong(s->file,&uMagic) != UNZ_OK)
           err=UNZ_ERRNO;
        else if (uMagic!=0x02014b50)
           err=UNZ_BADZIPFILE;
     }
        if (unzlocal_getShort(s->file,&file_info.version) != UNZ_OK)
                err=UNZ_ERRNO;
        if (unzlocal_getShort(s->file,&file_info.version_needed) != UNZ_OK)
                err=UNZ_ERRNO;
        if (unzlocal_getShort(s->file,&file_info.flag) != UNZ_OK)
                err=UNZ_ERRNO;
        if (unzlocal_getShort(s->file,&file_info.compression_method) != UNZ_OK)
                err=UNZ_ERRNO;
        if (unzlocal_getLong(s->file,&file_info.dosDate) != UNZ_OK)
                err=UNZ_ERRNO;
    unzlocal_DosDateToTmuDate(file_info.dosDate,&file_info.tmu_date);
        if (unzlocal_getLong(s->file,&file_info.crc) != UNZ_OK)
                err=UNZ_ERRNO;
        if (unzlocal_getLong(s->file,&file_info.compressed_size) != UNZ_OK)
                err=UNZ_ERRNO;
        if (unzlocal_getLong(s->file,&file_info.uncompressed_size) != UNZ_OK)
                err=UNZ_ERRNO;
        if (unzlocal_getShort(s->file,&file_info.size_filename) != UNZ_OK)
                err=UNZ_ERRNO;
        if (unzlocal_getShort(s->file,&file_info.size_file_extra) != UNZ_OK)
                err=UNZ_ERRNO;
        if (unzlocal_getShort(s->file,&file_info.size_file_comment) != UNZ_OK)
                err=UNZ_ERRNO;
        if (unzlocal_getShort(s->file,&file_info.disk_num_start) != UNZ_OK)
                err=UNZ_ERRNO;
        if (unzlocal_getShort(s->file,&file_info.internal_fa) != UNZ_OK)
                err=UNZ_ERRNO;
        if (unzlocal_getLong(s->file,&file_info.external_fa) != UNZ_OK)
                err=UNZ_ERRNO;
        if (unzlocal_getLong(s->file,&file_info_internal.offset_curfile) != UNZ_OK)
                err=UNZ_ERRNO;
        lSeek+=file_info.size_filename;
        if ((err==UNZ_OK) && (szFileName!=NULL))
        {
                ULONG uSizeRead ;
                if (file_info.size_filename<fileNameBufferSize)
                {
                        *(szFileName+file_info.size_filename)='\0';
                        uSizeRead = file_info.size_filename;
                }
                else
                        uSizeRead = fileNameBufferSize;
                if ((file_info.size_filename>0) && (fileNameBufferSize>0))
                        if (fread(szFileName,(UINT)uSizeRead,1,s->file)!=1)
                                err=UNZ_ERRNO;
                lSeek -= uSizeRead;
        }
        if ((err==UNZ_OK) && (extraField!=NULL))
        {
                ULONG uSizeRead ;
                if (file_info.size_file_extra<extraFieldBufferSize)
                        uSizeRead = file_info.size_file_extra;
                else
                        uSizeRead = extraFieldBufferSize;
                if (lSeek!=0)
        {
           if (fseek(s->file,lSeek,SEEK_CUR) != 0)
              err = UNZ_ERRNO;
        }
                if ((file_info.size_file_extra>0) && (extraFieldBufferSize>0))
                        if (fread(extraField,(UINT)uSizeRead,1,s->file)!=1)
                                err=UNZ_ERRNO;

        }
        else

        if ((err==UNZ_OK) && (szComment!=NULL))
        {
                ULONG uSizeRead ;
                if (file_info.size_file_comment<commentBufferSize)
                {
                        *(szComment+file_info.size_file_comment)='\0';
                        uSizeRead = file_info.size_file_comment;
                }
                else
                        uSizeRead = commentBufferSize;
                if (lSeek!=0)
        {
           if (fseek(s->file,lSeek,SEEK_CUR) == 0)
              ;
           else
              err=UNZ_ERRNO;
        }
                if ((file_info.size_file_comment>0) && (commentBufferSize>0))
                        if (fread(szComment,(UINT)uSizeRead,1,s->file)!=1)
                                err=UNZ_ERRNO;
        }
        if ((err==UNZ_OK) && (pfile_info!=NULL))
                *pfile_info=file_info;
        if ((err==UNZ_OK) && (pfile_info_internal!=NULL))
                *pfile_info_internal=file_info_internal;
        return err;
}

MODULE int unzlocal_CheckCurrentFileCoherencyHeader(
        unz_s* s,
        UINT* piSizeVar,
        ULONG *poffset_local_extrafield,
        UINT  *psize_local_extrafield
) {
    ULONG uMagic,uData,uFlags;
    ULONG size_filename;
    ULONG size_extra_field;
    int err=UNZ_OK;
    *piSizeVar = 0;
    *poffset_local_extrafield = 0;
    *psize_local_extrafield = 0;
    if (fseek(s->file,s->cur_file_info_internal.offset_curfile +
                                                            s->byte_before_the_zipfile,SEEK_SET)!=0)
            return UNZ_ERRNO;
    if (err==UNZ_OK)
    {
        if (unzlocal_getLong(s->file,&uMagic) != UNZ_OK)
           err=UNZ_ERRNO;
        else if (uMagic!=0x04034b50)
           err=UNZ_BADZIPFILE;
    }
    if (unzlocal_getShort(s->file,&uData) != UNZ_OK)
            err=UNZ_ERRNO;
    if (unzlocal_getShort(s->file,&uFlags) != UNZ_OK)
            err=UNZ_ERRNO;
    if (unzlocal_getShort(s->file,&uData) != UNZ_OK)
            err=UNZ_ERRNO;
    else if ((err==UNZ_OK) && (uData!=s->cur_file_info.compression_method))
            err=UNZ_BADZIPFILE;
    if ((err==UNZ_OK) && (s->cur_file_info.compression_method!=0) &&
                     (s->cur_file_info.compression_method!=Z_DEFLATED))
    err=UNZ_BADZIPFILE;
    if (unzlocal_getLong(s->file,&uData) != UNZ_OK)
            err=UNZ_ERRNO;
    if (unzlocal_getLong(s->file,&uData) != UNZ_OK)
            err=UNZ_ERRNO;
    else if ((err==UNZ_OK) && (uData!=s->cur_file_info.crc) &&
                                  ((uFlags & 8)==0))
            err=UNZ_BADZIPFILE;
    if (unzlocal_getLong(s->file,&uData) != UNZ_OK)
            err=UNZ_ERRNO;
    else if ((err==UNZ_OK) && (uData!=s->cur_file_info.compressed_size) &&
                                                      ((uFlags & 8)==0))
            err=UNZ_BADZIPFILE;
    if (unzlocal_getLong(s->file,&uData) != UNZ_OK)
            err=UNZ_ERRNO;
    else if ((err==UNZ_OK) && (uData!=s->cur_file_info.uncompressed_size) &&
                                                      ((uFlags & 8)==0))
            err=UNZ_BADZIPFILE;
    if (unzlocal_getShort(s->file,&size_filename) != UNZ_OK)
            err=UNZ_ERRNO;
    else if ((err==UNZ_OK) && (size_filename!=s->cur_file_info.size_filename))
            err=UNZ_BADZIPFILE;
    *piSizeVar += (UINT)size_filename;
    if (unzlocal_getShort(s->file,&size_extra_field) != UNZ_OK)
            err=UNZ_ERRNO;
    *poffset_local_extrafield= s->cur_file_info_internal.offset_curfile +
                                                                    SIZEZIPLOCALHEADER + size_filename;
    *psize_local_extrafield = (UINT)size_extra_field;
    *piSizeVar += (UINT)size_extra_field;
    return err;
}

MODULE int huft_build
(UINT *b,
UINT n,
UINT s,
const UINT* d,
const UINT* e,
inflate_huft** t,
UINT *m,
inflate_huft* hp,
UINT *hn,
UINT *v)
{ UINT a;
  UINT c[BMAX+1];
  UINT f;
  int g;
  int h;
  register UINT i;
  register UINT j;
  register int k;
  int l;
  UINT mask;
  register UINT *p;
  inflate_huft *q;
  struct inflate_huft_s r;
  inflate_huft *u[BMAX];
  register int w;
  UINT x[BMAX+1];
  UINT *xp;
  int y;
  UINT z;

  r.base = 0; // r.base needs initializiation to avoid spurious GCC compiler warnings

  p = c;
  C4
  p = b;  i = n;
  do {
    c[*p++]++;
  } while (--i);
  if (c[0] == n)
  {
    *t = (inflate_huft *)Z_NULL;
    *m = 0;
    return Z_OK;
  }

  l = *m;
  for (j = 1; j <= BMAX; j++)
    if (c[j])
      break;
  k = j;
  if ((UINT)l < j)
    l = j;
  for (i = BMAX; i; i--)
    if (c[i])
      break;
  g = i;
  if ((UINT)l > i)
    l = i;
  *m = l;

  for (y = 1 << j; j < i; j++, y <<= 1)
    if ((y -= c[j]) < 0)
      return Z_DATA_ERROR;
  if ((y -= c[i]) < 0)
    return Z_DATA_ERROR;
  c[i] += y;

  x[1] = j = 0;
  p = c + 1;  xp = x + 2;
  while (--i) {
    *xp++ = (j += *p++);
  }

  p = b;  i = 0;
  do {
    if ((j = *p++) != 0)
      v[x[j]++] = i;
  } while (++i < n);
  n = x[g];

  x[0] = i = 0;
  p = v;
  h = -1;
  w = -l;
  u[0] = (inflate_huft *)Z_NULL;
  q = (inflate_huft *)Z_NULL;
  z = 0;

  for (; k <= g; k++)
  {
    a = c[k];
    while (a--)
    {

      while (k > w + l)
      {
        h++;
        w += l;

        z = g - w;
        z = z > (UINT) l ? (UINT) l : z;
        if ((f = 1 << (j = k - w)) > a + 1)
        {
          f -= a + 1;
          xp = c + k;
          if (j < z)
            while (++j < z)
            {
              if ((f <<= 1) <= *++xp)
                break;
              f -= *xp;
            }
        }
        z = 1 << j;

        if (*hn + z > MANY)
          return Z_MEM_ERROR;
        u[h] = q = hp + *hn;
        *hn += z;

        if (h)
        {
          x[h] = i;
          r.bits = (UBYTE)l;
          r.exop = (UBYTE)j;
          j = i >> (w - l);
          r.base = (UINT)(q - u[h-1] - j);
          u[h-1][j] = r;
        }
        else
          *t = q;
      }

      r.bits = (UBYTE)(k - w);
      if (p >= v + n)
        r.exop = 128 + 64;
      else if (*p < s)
      {
        r.exop = (UBYTE)(*p < 256 ? 0 : 32 + 64);
        r.base = *p++;
      }
      else
      {
        r.exop = (UBYTE)(e[*p - s] + 16 + 64);
        r.base = d[*p++ - s];
      }

      f = 1 << (k - w);
      for (j = i >> w; j < z; j += f)
        q[j] = r;

      for (j = 1 << (k - 1); i & j; j >>= 1)
        i ^= j;
      i ^= j;

      mask = (1 << w) - 1;
      while ((i & mask) != x[h])
      {
        h--;
        w -= l;
        mask = (1 << w) - 1;
      }
    }
  }

  return y != 0 && g != 1 ? Z_BUF_ERROR : Z_OK;
}

MODULE int inflate_trees_bits(UINT* c, UINT* bb, inflate_huft** tb, inflate_huft* hp, z_stream* z)
{   int   r;
    UINT  hn = 0;
    UINT* v;

    if ((v = (UINT*) ZALLOC(z, 19, sizeof(UINT))) == Z_NULL)
    {   return Z_MEM_ERROR;
    }
    r = huft_build(c, 19, 19, (UINT*) Z_NULL, (UINT*) Z_NULL, tb, bb, hp, &hn, v);
    if (r == Z_DATA_ERROR)
    {   z->msg = (char*) "oversubscribed dynamic bit lengths tree";
    } elif (r == Z_BUF_ERROR || *bb == 0)
    {   z->msg = (char*) "incomplete dynamic bit lengths tree";
        r = Z_DATA_ERROR;
    }
    ZFREE(z, v);
    return r;
}

MODULE int inflate_trees_dynamic(
UINT nl,
UINT nd,
UINT *c,
UINT *bl,
UINT *bd,
inflate_huft** tl,
inflate_huft** td,
inflate_huft *hp,
z_stream* z
) {
  int r;
  UINT hn = 0;
  UINT *v;

  if ((v = (UINT*)ZALLOC(z, 288, sizeof(UINT))) == Z_NULL)
    return Z_MEM_ERROR;

  r = huft_build(c, nl, 257, cplens, cplext, tl, bl, hp, &hn, v);
  if (r != Z_OK || *bl == 0)
  {
    if (r == Z_DATA_ERROR)
      z->msg = (char*)"oversubscribed literal/length tree";
    else if (r != Z_MEM_ERROR)
    {
      z->msg = (char*)"incomplete literal/length tree";
      r = Z_DATA_ERROR;
    }
    ZFREE(z, v);
    return r;
  }

  r = huft_build(c + nl, nd, 0, cpdist, cpdext, td, bd, hp, &hn, v);
  if (r != Z_OK || (*bd == 0 && nl > 257))
  {
    if (r == Z_DATA_ERROR)
      z->msg = (char*)"oversubscribed distance tree";
    else if (r == Z_BUF_ERROR) {
      z->msg = (char*)"incomplete distance tree";
      r = Z_DATA_ERROR;
    }
    else if (r != Z_MEM_ERROR)
    {
      z->msg = (char*)"empty distance tree with lengths";
      r = Z_DATA_ERROR;
    }
    ZFREE(z, v);
    return r;
  }

  ZFREE(z, v);
  return Z_OK;
}

MODULE int inflate_trees_fixed(UINT* bl, UINT* bd, inflate_huft** tl, inflate_huft** td)
{   *bl = fixed_bl;
    *bd = fixed_bd;
    *tl = fixed_tl;
    *td = fixed_td;

    return Z_OK;
}

MODULE void inflate_blocks_reset(inflate_blocks_statef* s, z_stream* z, ULONG* c)
{   if (c != Z_NULL)
    *c = s->check;
  if (s->mode == BTREE || s->mode == DTREE)
    ZFREE(z, s->sub.trees.blens);
  if (s->mode == CODES)
    inflate_codes_free(s->sub.decode.codes, z);
  s->mode = TYPE;
  s->bitk = 0;
  s->bitb = 0;
  s->read = s->write = s->window;
  if (s->checkfn != Z_NULL)
    z->adler = s->check = (*s->checkfn)(0L, (const UBYTE *)Z_NULL, 0);
}

MODULE inflate_blocks_statef* inflate_blocks_new(z_stream* z, check_func c, UINT w)
{   inflate_blocks_statef *s;
  if ((s = (inflate_blocks_statef *)ZALLOC
       (z,1,sizeof(struct inflate_blocks_state))) == Z_NULL)
    return s;
  if ((s->hufts =
       (inflate_huft *)ZALLOC(z, sizeof(inflate_huft), MANY)) == Z_NULL)
  {
    ZFREE(z, s);
    return Z_NULL;
  }
  if ((s->window = (UBYTE *)ZALLOC(z, 1, w)) == Z_NULL)
  {
    ZFREE(z, s->hufts);
    ZFREE(z, s);
    return Z_NULL;
  }
  s->end = s->window + w;
  s->checkfn = c;
  s->mode = TYPE;
  inflate_blocks_reset(s, z, Z_NULL);
  return s;
}

MODULE int inflate_blocks(inflate_blocks_statef* s, z_stream* z, int r)
{   UINT t;
  ULONG b;
  UINT k;
  UBYTE *p;
  UINT n;
  UBYTE *q;
  UINT m;

  LOAD

  while (1) switch (s->mode)
  {
    case TYPE:
      ZNEEDBITS(3)
      t = (UINT)b & 7;
      s->last = t & 1;
      switch (t >> 1)
      {
        case 0:
          DUMPBITS(3)
          t = k & 7;
          DUMPBITS(t)
          s->mode = LENS;
          break;
        case 1:
          {
            UINT bl, bd;
            inflate_huft *tl, *td;
            inflate_trees_fixed(&bl, &bd, &tl, &td);
            s->sub.decode.codes = inflate_codes_new(bl, bd, tl, td, z);
            if (s->sub.decode.codes == Z_NULL)
            {
              r = Z_MEM_ERROR;
              LEAVE
            }
          }
          DUMPBITS(3)
          s->mode = CODES;
          break;
        case 2:
          DUMPBITS(3)
          s->mode = TABLE;
          break;
        case 3:
          DUMPBITS(3)
          s->mode = BAD;
          z->msg = (char*)"invalid block type";
          r = Z_DATA_ERROR;
          LEAVE
      }
      break;
    case LENS:
      ZNEEDBITS(32)
      if ((((~b) >> 16) & 0xffff) != (b & 0xffff))
      {
        s->mode = BAD;
        z->msg = (char*)"invalid stored block lengths";
        r = Z_DATA_ERROR;
        LEAVE
      }
      s->sub.left = (UINT)b & 0xffff;
      b = k = 0;
      s->mode = s->sub.left ? STORED : (s->last ? DRY : TYPE);
      break;
    case STORED:
      if (n == 0)
        LEAVE
      NEEDOUT
      t = s->sub.left;
      if (t > n) t = n;
      if (t > m) t = m;
      memcpy(q, p, t);
      p += t;  n -= t;
      q += t;  m -= t;
      if ((s->sub.left -= t) != 0)
        break;
      s->mode = s->last ? DRY : TYPE;
      break;
    case TABLE:
      ZNEEDBITS(14)
      s->sub.trees.table = t = (UINT)b & 0x3fff;
      if ((t & 0x1f) > 29 || ((t >> 5) & 0x1f) > 29)
      {
        s->mode = BAD;
        z->msg = (char*)"too many length or distance symbols";
        r = Z_DATA_ERROR;
        LEAVE
      }
      t = 258 + (t & 0x1f) + ((t >> 5) & 0x1f);
      if ((s->sub.trees.blens = (UINT*) ZALLOC(z, t, sizeof(UINT))) == Z_NULL)
      {
        r = Z_MEM_ERROR;
        LEAVE
      }
      DUMPBITS(14)
      s->sub.trees.index = 0;
      s->mode = BTREE;
    case BTREE:
      while (s->sub.trees.index < 4 + (s->sub.trees.table >> 10))
      {
        ZNEEDBITS(3)
        s->sub.trees.blens[border[s->sub.trees.index++]] = (UINT)b & 7;
        DUMPBITS(3)
      }
      while (s->sub.trees.index < 19)
        s->sub.trees.blens[border[s->sub.trees.index++]] = 0;
      s->sub.trees.bb = 7;
      t = inflate_trees_bits(s->sub.trees.blens, &s->sub.trees.bb,
                             &s->sub.trees.tb, s->hufts, z);
      if (t != Z_OK)
      {
        ZFREE(z, s->sub.trees.blens);
        r = t;
        if (r == Z_DATA_ERROR)
          s->mode = BAD;
        LEAVE
      }
      s->sub.trees.index = 0;
      s->mode = DTREE;
    case DTREE:
      while (t = s->sub.trees.table,
             s->sub.trees.index < 258 + (t & 0x1f) + ((t >> 5) & 0x1f))
      {
        inflate_huft *h;
        UINT i, j, c;
        t = s->sub.trees.bb;
        ZNEEDBITS(t)
        h = s->sub.trees.tb + ((UINT)b & inflate_mask[t]);
        t = h->bits;
        c = h->base;
        if (c < 16)
        {
          DUMPBITS(t)
          s->sub.trees.blens[s->sub.trees.index++] = c;
        }
        else
        {
          i = c == 18 ? 7 : c - 14;
          j = c == 18 ? 11 : 3;
          ZNEEDBITS(t + i)
          DUMPBITS(t)
          j += (UINT)b & inflate_mask[i];
          DUMPBITS(i)
          i = s->sub.trees.index;
          t = s->sub.trees.table;
          if (i + j > 258 + (t & 0x1f) + ((t >> 5) & 0x1f) ||
              (c == 16 && i < 1))
          {
            ZFREE(z, s->sub.trees.blens);
            s->mode = BAD;
            z->msg = (char*)"invalid bit length repeat";
            r = Z_DATA_ERROR;
            LEAVE
          }
          c = c == 16 ? s->sub.trees.blens[i - 1] : 0;
          do {
            s->sub.trees.blens[i++] = c;
          } while (--j);
          s->sub.trees.index = i;
        }
      }
      s->sub.trees.tb = Z_NULL;
      {
        UINT bl, bd;
        inflate_huft *tl, *td;
        inflate_codes_statef *c;
        bl = 9;
        bd = 6;
        t = s->sub.trees.table;
        t = inflate_trees_dynamic(257 + (t & 0x1f), 1 + ((t >> 5) & 0x1f),
                                  s->sub.trees.blens, &bl, &bd, &tl, &td,
                                  s->hufts, z);
        ZFREE(z, s->sub.trees.blens);
        if (t != Z_OK)
        {
          if (t == (UINT)Z_DATA_ERROR)
            s->mode = BAD;
          r = t;
          LEAVE
        }
        if ((c = inflate_codes_new(bl, bd, tl, td, z)) == Z_NULL)
        {
          r = Z_MEM_ERROR;
          LEAVE
        }
        s->sub.decode.codes = c;
      }
      s->mode = CODES;
    case CODES:
      UPDATE
      if ((r = inflate_codes(s, z, r)) != Z_STREAM_END)
        return inflate_flush(s, z, r);
      r = Z_OK;
      inflate_codes_free(s->sub.decode.codes, z);
      LOAD
      if (!s->last)
      {
        s->mode = TYPE;
        break;
      }
      s->mode = DRY;
    case DRY:
      FLUSH
      if (s->read != s->write)
        LEAVE
      s->mode = DONE;
    case DONE:
      r = Z_STREAM_END;
      LEAVE
    case BAD:
      r = Z_DATA_ERROR;
      LEAVE
    default:
      r = Z_STREAM_ERROR;
      LEAVE
  }
}

MODULE int inflate_blocks_free(inflate_blocks_statef* s, z_stream* z)
{   inflate_blocks_reset(s, z, Z_NULL);
    ZFREE(z, s->window);
    ZFREE(z, s->hufts);
    ZFREE(z, s);

    return Z_OK;
}

MODULE inflate_codes_statef* inflate_codes_new(UINT bl, UINT bd, inflate_huft* tl, inflate_huft* td, z_stream* z)
{   inflate_codes_statef* c;

    if ((c = (inflate_codes_statef *) ZALLOC(z,1,sizeof(struct inflate_codes_state))) != Z_NULL)
    {   c->mode  = START;
        c->lbits = (UBYTE)bl;
        c->dbits = (UBYTE)bd;
        c->ltree = tl;
        c->dtree = td;
    }

    return c;
}

MODULE int inflate_codes(inflate_blocks_statef* s, z_stream* z, int r)
{   UINT j;
  inflate_huft *t;
  UINT e;
  ULONG b;
  UINT k;
  UBYTE *p;
  UINT n;
  UBYTE *q;
  UINT m;
  UBYTE *f;
  inflate_codes_statef *c = s->sub.decode.codes;

  LOAD

  while (1) switch (c->mode)
  {
    case START:
      if (m >= 258 && n >= 10)
      {
        UPDATE
        r = inflate_fast(c->lbits, c->dbits, c->ltree, c->dtree, s, z);
        LOAD
        if (r != Z_OK)
        {
          c->mode = r == Z_STREAM_END ? WASH : BADCODE;
          break;
        }
      }
      c->sub.code.need = c->lbits;
      c->sub.code.tree = c->ltree;
      c->mode = LEN;
    case LEN:
      j = c->sub.code.need;
      ZNEEDBITS(j)
      t = c->sub.code.tree + ((UINT)b & inflate_mask[j]);
      DUMPBITS(t->bits)
      e = (UINT)(t->exop);
      if (e == 0)
      {
        c->sub.lit = t->base;
        c->mode = LIT;
        break;
      }
      if (e & 16)
      {
        c->sub.copy.get = e & 15;
        c->len = t->base;
        c->mode = LENEXT;
        break;
      }
      if ((e & 64) == 0)
      {
        c->sub.code.need = e;
        c->sub.code.tree = t + t->base;
        break;
      }
      if (e & 32)
      {
        c->mode = WASH;
        break;
      }
      c->mode = BADCODE;
      z->msg = (char*)"invalid literal/length code";
      r = Z_DATA_ERROR;
      LEAVE
    case LENEXT:
      j = c->sub.copy.get;
      ZNEEDBITS(j)
      c->len += (UINT)b & inflate_mask[j];
      DUMPBITS(j)
      c->sub.code.need = c->dbits;
      c->sub.code.tree = c->dtree;
      c->mode = DIST;
    case DIST:
      j = c->sub.code.need;
      ZNEEDBITS(j)
      t = c->sub.code.tree + ((UINT)b & inflate_mask[j]);
      DUMPBITS(t->bits)
      e = (UINT)(t->exop);
      if (e & 16)
      {
        c->sub.copy.get = e & 15;
        c->sub.copy.dist = t->base;
        c->mode = DISTEXT;
        break;
      }
      if ((e & 64) == 0)
      {
        c->sub.code.need = e;
        c->sub.code.tree = t + t->base;
        break;
      }
      c->mode = BADCODE;
      z->msg = (char*)"invalid distance code";
      r = Z_DATA_ERROR;
      LEAVE
    case DISTEXT:
      j = c->sub.copy.get;
      ZNEEDBITS(j)
      c->sub.copy.dist += (UINT)b & inflate_mask[j];
      DUMPBITS(j)
      c->mode = COPY;
    case COPY:
      f = (UINT)(q - s->window) < c->sub.copy.dist ?
          s->end - (c->sub.copy.dist - (q - s->window)) :
          q - c->sub.copy.dist;
      while (c->len)
      {
        NEEDOUT
        OUTBYTE(*f++)
        if (f == s->end)
          f = s->window;
        c->len--;
      }
      c->mode = START;
      break;
    case LIT:
      NEEDOUT
      OUTBYTE(c->sub.lit)
      c->mode = START;
      break;
    case WASH:
      if (k > 7)
      {
        k -= 8;
        n++;
        p--;
      }
      FLUSH
      if (s->read != s->write)
        LEAVE
      c->mode = END;
    case END:
      r = Z_STREAM_END;
      LEAVE
    case BADCODE:
      r = Z_DATA_ERROR;
      LEAVE
    default:
      r = Z_STREAM_ERROR;
      LEAVE
  }
}

MODULE void inflate_codes_free(inflate_codes_statef* c, z_stream* z)
{   ZFREE(z, c);
}

MODULE int inflate_fast(UINT bl, UINT bd, inflate_huft* tl, inflate_huft* td, inflate_blocks_statef* s, z_stream* z)
{
  inflate_huft *t;
  UINT e;
  ULONG b;
  UINT k;
  UBYTE *p;
  UINT n;
  UBYTE *q;
  UINT m;
  UINT ml;
  UINT md;
  UINT c;
  UINT d;
  UBYTE *r;

  LOAD

  ml = inflate_mask[bl];
  md = inflate_mask[bd];

  do {

    GRABBITS(20)
    if ((e = (t = tl + ((UINT)b & ml))->exop) == 0)
    {
      DUMPBITS(t->bits)
      *q++ = (UBYTE)t->base;
      m--;
      continue;
    }
    do {
      DUMPBITS(t->bits)
      if (e & 16)
      {

        e &= 15;
        c = t->base + ((UINT)b & inflate_mask[e]);
        DUMPBITS(e)

        GRABBITS(15);
        e = (t = td + ((UINT)b & md))->exop;
        do {
          DUMPBITS(t->bits)
          if (e & 16)
          {

            e &= 15;
            GRABBITS(e)
            d = t->base + ((UINT)b & inflate_mask[e]);
            DUMPBITS(e)

            m -= c;
            if ((UINT)(q - s->window) >= d)
            {
              r = q - d;
              *q++ = *r++;  c--;
              *q++ = *r++;  c--;
            }
            else
            {
              e = d - (UINT)(q - s->window);
              r = s->end - e;
              if (c > e)
              {
                c -= e;
                do {
                  *q++ = *r++;
                } while (--e);
                r = s->window;
              }
            }
            do {
              *q++ = *r++;
            } while (--c);
            break;
          }
          else if ((e & 64) == 0)
          {
            t += t->base;
            e = (t += ((UINT)b & inflate_mask[e]))->exop;
          }
          else
          {
            z->msg = (char*)"invalid distance code";
            UNGRAB
            UPDATE
            return Z_DATA_ERROR;
          }
        } while (1);
        break;
      }
      if ((e & 64) == 0)
      {
        t += t->base;
        if ((e = (t += ((UINT)b & inflate_mask[e]))->exop) == 0)
        {
          DUMPBITS(t->bits)
          *q++ = (UBYTE)t->base;
          m--;
          break;
        }
      }
      else if (e & 32)
      {
        UNGRAB
        UPDATE
        return Z_STREAM_END;
      }
      else
      {
        z->msg = (char*)"invalid literal/length code";
        UNGRAB
        UPDATE
        return Z_DATA_ERROR;
      }
    } while (1);
  } while (m >= 258 && n >= 10);

  UNGRAB
  UPDATE
  return Z_OK;
}

MODULE int inflateReset(z_stream* z)
{
  if (z == Z_NULL || z->state == Z_NULL)
    return Z_STREAM_ERROR;
  z->total_in = z->total_out = 0;
  z->msg = Z_NULL;
  z->state->mode = z->state->nowrap ? BLOCKS : METHOD;
  inflate_blocks_reset(z->state->blocks, z, Z_NULL);
  return Z_OK;
}

MODULE int inflateEnd(z_stream* z)
{
  if (z == Z_NULL || z->state == Z_NULL || z->zfree == Z_NULL)
    return Z_STREAM_ERROR;
  if (z->state->blocks != Z_NULL)
    inflate_blocks_free(z->state->blocks, z);
  ZFREE(z, z->state);
  z->state = Z_NULL;
  return Z_OK;
}

MODULE int inflateInit(z_stream* z, int w, int stream_size)
{   if (stream_size != sizeof(z_stream))
      return Z_VERSION_ERROR;

  if (z == Z_NULL)
    return Z_STREAM_ERROR;
  z->msg = Z_NULL;
  if (z->zalloc == Z_NULL)
  {
    z->zalloc = zcalloc;
    z->opaque = (void*)0;
  }
  if (z->zfree == Z_NULL) z->zfree = zcfree;
  if ((z->state = (struct internal_state*)
       ZALLOC(z,1,sizeof(struct internal_state))) == Z_NULL)
    return Z_MEM_ERROR;
  z->state->blocks = Z_NULL;

  z->state->nowrap = 0;
  if (w < 0)
  {
    w = - w;
    z->state->nowrap = 1;
  }

  if (w < 8 || w > 15)
  {
    inflateEnd(z);
    return Z_STREAM_ERROR;
  }
  z->state->wbits = (UINT)w;

  if ((z->state->blocks =
      inflate_blocks_new(z, z->state->nowrap ? (check_func) Z_NULL : adler32, (UINT)1 << w))
      == Z_NULL)
  {
    inflateEnd(z);
    return Z_MEM_ERROR;
  }

  inflateReset(z);
  return Z_OK;
}

MODULE int inflate(z_stream* z, int f)
{   int r;
  UINT b;
  if (z == Z_NULL || z->state == Z_NULL || z->next_in == Z_NULL)
    return Z_STREAM_ERROR;
  f = f == Z_FINISH ? Z_BUF_ERROR : Z_OK;
  r = Z_BUF_ERROR;
  while (1) switch (z->state->mode)
  {
    case METHOD:
      NEEDBYTE2
      if (((z->state->sub.method = NEXTBYTE2) & 0xf) != Z_DEFLATED)
      {
        z->state->mode = BAD2;
        z->msg = (char*)"unknown compression method";
        z->state->sub.marker = 5;
        break;
      }
      if ((z->state->sub.method >> 4) + 8 > z->state->wbits)
      {
        z->state->mode = BAD2;
        z->msg = (char*)"invalid window size";
        z->state->sub.marker = 5;
        break;
      }
      z->state->mode = ZFLAG;
    case ZFLAG:
      NEEDBYTE2
      b = NEXTBYTE2;
      if (((z->state->sub.method << 8) + b) % 31)
      {
        z->state->mode = BAD2;
        z->msg = (char*)"incorrect header check";
        z->state->sub.marker = 5;
        break;
      }
      if (!(b & PRESET_DICT))
      {
        z->state->mode = BLOCKS;
        break;
      }
      z->state->mode = DICT4;
    case DICT4:
      NEEDBYTE2
      z->state->sub.check.need = (ULONG)NEXTBYTE2 << 24;
      z->state->mode = DICT3;
    case DICT3:
      NEEDBYTE2
      z->state->sub.check.need += (ULONG)NEXTBYTE2 << 16;
      z->state->mode = DICT2;
    case DICT2:
      NEEDBYTE2
      z->state->sub.check.need += (ULONG)NEXTBYTE2 << 8;
      z->state->mode = DICT1;
    case DICT1:
      NEEDBYTE2
      z->state->sub.check.need += (ULONG)NEXTBYTE2;
      z->adler = z->state->sub.check.need;
      z->state->mode = DICT0;
      return Z_NEED_DICT;
    case DICT0:
      z->state->mode = BAD2;
      z->msg = (char*)"need dictionary";
      z->state->sub.marker = 0;
      return Z_STREAM_ERROR;
    case BLOCKS:
      r = inflate_blocks(z->state->blocks, z, r);
      if (r == Z_DATA_ERROR)
      {
        z->state->mode = BAD2;
        z->state->sub.marker = 0;
        break;
      }
      if (r == Z_OK)
        r = f;
      if (r != Z_STREAM_END)
        return r;
      r = f;
      inflate_blocks_reset(z->state->blocks, z, &z->state->sub.check.was);
      if (z->state->nowrap)
      {
        z->state->mode = DONE2;
        break;
      }
      z->state->mode = CHECK4;
    case CHECK4:
      NEEDBYTE2
      z->state->sub.check.need = (ULONG)NEXTBYTE2 << 24;
      z->state->mode = CHECK3;
    case CHECK3:
      NEEDBYTE2
      z->state->sub.check.need += (ULONG)NEXTBYTE2 << 16;
      z->state->mode = CHECK2;
    case CHECK2:
      NEEDBYTE2
      z->state->sub.check.need += (ULONG)NEXTBYTE2 << 8;
      z->state->mode = CHECK1;
    case CHECK1:
      NEEDBYTE2
      z->state->sub.check.need += (ULONG)NEXTBYTE2;
      if (z->state->sub.check.was != z->state->sub.check.need)
      {
        z->state->mode = BAD2;
        z->msg = (char*)"incorrect data check";
        z->state->sub.marker = 5;
        break;
      }
      z->state->mode = DONE2;
    case DONE2:
      return Z_STREAM_END;
    case BAD2:
      return Z_DATA_ERROR;
    default:
      return Z_STREAM_ERROR;
  }
}

MODULE int inflate_flush(inflate_blocks_statef* s, z_stream* z, int r)
{
  UINT n;
  UBYTE *p;
  UBYTE *q;

  p = z->next_out;
  q = s->read;

  n = (UINT)((q <= s->write ? s->write : s->end) - q);
  if (n > z->avail_out) n = z->avail_out;
  if (n && r == Z_BUF_ERROR) r = Z_OK;

  z->avail_out -= n;
  z->total_out += n;

  if (s->checkfn != Z_NULL)
    z->adler = s->check = (*s->checkfn)(s->check, q, n);

  memcpy(p, q, n);
  p += n;
  q += n;

  if (q == s->end)
  {

    q = s->window;
    if (s->write == s->end)
      s->write = s->window;

    n = (UINT)(s->write - q);
    if (n > z->avail_out) n = z->avail_out;
    if (n && r == Z_BUF_ERROR) r = Z_OK;

    z->avail_out -= n;
    z->total_out += n;

    if (s->checkfn != Z_NULL)
      z->adler = s->check = (*s->checkfn)(s->check, q, n);

    memcpy(p, q, n);
    p += n;
    q += n;
  }

  z->next_out = p;
  s->read = q;

  return r;
}

MODULE ULONG adler32(ULONG adler, const UBYTE* buf, UINT len)
{
    unsigned long s1 = adler & 0xffff;
    unsigned long s2 = (adler >> 16) & 0xffff;
    int k;
    if (buf == Z_NULL) return 1L;
    while (len > 0) {
        k = len < NMAX ? len : NMAX;
        len -= k;
        while (k >= 16) {
            DO16b(buf);
            buf += 16;
            k -= 16;
        }
        if (k != 0) do {
            s1 += *buf++;
            s2 += s1;
        } while (--k);
        s1 %= BASE;
        s2 %= BASE;
    }
    return (s2 << 16) | s1;
}

MODULE void* zcalloc(void* opaque, unsigned items, unsigned size)
{   if (opaque) items += size - size;
    return (void*) calloc(items, size);
}

MODULE void zcfree(UNUSED void* opaque, void* ptr)
{   free(ptr);
}
