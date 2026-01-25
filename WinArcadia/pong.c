// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
#endif
#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
    #include "resource.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#include "aa.h"

// DEFINES----------------------------------------------------------------

// #define TEAMCOLOURS
// whether to differentiate teammates via colour
// (otherwise they are identical)

#define PONGRUMBLE       10 // rumble duration, in frames
#define RIFLEHITRUMBLE   15 // rumble duration, in frames
#define RIFLEMISSRUMBLE   5 // rumble duration, in frames

#define CYANBLUE         CYAN
#define CYANGREEN        CYAN

#define rtballx          (   ltballx + ((memmap == MEMMAP_8550 && pong_machine == PONGMACHINE_1976) ? 0 : 1))
#define newrtballx       (newltballx + ((memmap == MEMMAP_8550 && pong_machine == PONGMACHINE_1976) ? 0 : 1))

#define FACE_LT   0 // faces left
#define FACE_RT   1 // faces right
#define FACE_EI   2 // either
#define FACE_LF   3 // faces left but can shoot forward
#define FACE_RF   4 // faces left but can shoot forward

#define DIR_VERT  0
#define DIR_HORIZ 1

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT       int                  angles         = ANGLES_4,
                                  batvalue       = 1,
                                  gridy[2],
                                  gx[2], gy[2],
                                  lockhoriz      = TRUE,
                                  playerid       = FALSE,
                                  players        = PLAYERS_2,
                                  pong_start,
                                  pong_a,
                                  pong_b,
                                  pong_reset,
                                  ponghertz      = PONGSPEED_SLOW,
                                  ponglt, pongrt,
                                  pongspeed      = 0,
                                  score[2],
                                  serving        = SERVING_AUTO,
                                  robotspeed[2]  = { 0, 0 },
                                  pong_variant   = VARIANT_8550_PRACTICE;

EXPORT struct VariantStruct variantinfo[VARIANTS] = {
// AY-3-8500/8550
{ DARKGREEN, YELLOW,   PURPLE,    BLUE   , 2, { { 25-2,29-2,29-2,62-2,0,FACE_RT,45-2,6},                                                                                                                            { 99-2,64-2,97-2,97-2,1,FACE_LT,81-2,6} } }, // VARIANT_8550_TENNIS
{ BLUE,      CYANBLUE, DARKBLUE,  RED    , 4, { { 25-2,29-2,29-2,29-2,0,FACE_RT,63-2,3}, { 42-2,29-2,45-2,97-2,1,FACE_LF,63-2,3}, { 82-2,29-2,79-2,97-2,0,FACE_RF,63-2,3},                                          { 99-2,97-2,97-2,97-2,1,FACE_LT,63-2,3} } }, // VARIANT_8550_SOCCER
{ DARKRED,   ORANGE,   DARKGREEN, PINK   , 5, { { 25-2,29-2,29-2,29-2,0,FACE_RT,63-2,3}, { 42-2,29-2,45-2,97-2,1,FACE_LT,63-2,3}, { 82-2,29-2,79-2,97-2,0,FACE_RT,63-2,3}, { 84-2,29-2,81-2,97-2,1,FACE_LT,63-2,3}, { 99-2,97-2,97-2,97-2,1,FACE_LT,63-2,3} } }, // VARIANT_8550_HANDICAP (arbitrary colours)
{ PURPLE,    PINK,     BLUE,      YELLOW , 2, {                                                                                   { 82-2,29-2,79-2,97-2,0,FACE_LT,63-2,3}, { 84-2,29-2,81-2,97-2,1,FACE_LT,63-2,3}                                          } }, // VARIANT_8550_SQUASH
{ CYANGREEN, GREEN,    BROWN,     BLUE   , 1, {                                                                                                                            { 84-2,29-2,81-2,97-2,0,FACE_LT,63-2,3}                                          } }, // VARIANT_8550_PRACTICE
{ BROWN,     PINK,     DARKRED,   ORANGE , 0, { {  0  , 0  ,   0,   0,0,      0, 0,  0}                                                                                                                                                                     } }, // VARIANT_8550_RIFLE1   (arbitrary colours)
{ ORANGE,    RED,      DARKBLUE,  GREEN  , 0, { {  0  , 0  ,   0,   0,0,      0, 0,  0}                                                                                                                                                                     } }, // VARIANT_8550_RIFLE2   (arbitrary colours)
// AY-3-8600
{ DARKGREEN, YELLOW,   PURPLE,    BLUE   , 2, { {  0  ,29-2,29-2,62-2,0,FACE_RT,45-2,6}, {  0  ,64-2,97-2,97-2,1,FACE_LT,81-2,6}                                                                                                                            } }, // VARIANT_8600_TENNIS
{ BLUE,      CYANBLUE, DARKBLUE,  RED    , 4, { {  0  ,38-2,38-2,38-2,0,FACE_RT,63-2,3}, {  0  ,29-2,45-2,97-2,1,FACE_LF,63-2,3}, {  0  ,29-2,79-2,97-2,0,FACE_RF,63-2,3}, {  0  ,88-2,88-2,88-2,1,FACE_LT,63-2,3}                                          } }, // VARIANT_8600_HOCKEY
{ DARKGREEN, YELLOW,   PURPLE,    BLUE   , 4, { {  0  ,29-2,29-2,29-2,0,FACE_RT,63-2,3}, {  0  ,29-2,45-2,97-2,1,FACE_LF,63-2,3}, {  0  ,29-2,79-2,97-2,0,FACE_RF,63-2,3}, {  0  ,97-2,97-2,97-2,1,FACE_LT,63-2,3}                                          } }, // VARIANT_8600_SOCCER
{ PURPLE,    PINK,     BLUE,      YELLOW , 2, { {  0  ,29-2,79-2,97-2,0,FACE_LT,63-2,3}, {  0  ,29-2,81-2,97-2,1,FACE_LT,63-2,3}                                                                                                                            } }, // VARIANT_8600_SQUASH
{ CYANGREEN, GREEN,    BROWN,     BLUE   , 1, { {  0  ,29-2,81-2,97-2,0,FACE_LT,63-2,3}                                                                                                                                                                     } }, // VARIANT_8600_PRACTICE
{ BLUE,      CYANBLUE, DARKBLUE,  RED    , 0, { {  0  , 0  , 0  , 0  ,0,      0, 0  ,0}                                                                                                                                                                     } }, // VARIANT_8600_GRIDBALL
{ PURPLE,    PINK,     BLUE,      YELLOW , 2, { {  0  ,29-2,45-2,97-2,0,FACE_RT,63-2,3}, {  0  ,29-2,79-2,97-2,1,FACE_LT,63-2,3}                                                                                                                            } }, // VARIANT_8600_BASKETBALL
{ CYANGREEN, GREEN,    BROWN,     BLUE   , 1, { {  0  ,29-2,45-2,97-2,0,FACE_LT,63-2,3}                                                                                                                                                                     } }, // VARIANT_8600_BBPRACTICE
{ CYANGREEN, GREEN,    BROWN,     BLUE   , 0, { {  0  , 0  , 0  , 0  ,0,      0, 0  ,0}                                                                                                                                                                     } }, // VARIANT_8600_TARGET1
{ GREEN,     YELLOW,   PURPLE,    BLUE   , 0, { {  0  , 0  , 0  , 0  ,0,      0, 0  ,0}                                                                                                                                                                     } }, // VARIANT_8600_TARGET2
};

EXPORT const STRPTR varianthelp[VARIANTS] = {
// VARIANT_8550_TENNIS
"The ball will serve automatically towards the side which has just missed." \
" This sequence is repeated until a score of 15 is reached by one side, whereupon the game is stopped." \
" The ball will still bounce around but no further 'hits' or 'scores' can be made."
, // VARIANT_8550_SOCCER
"With this game each participant has a 'goalkeeper' and a 'forward'." \
" The layout is such that the 'goalkeeper' is in his normal position and the 'forward' can be positioned in any part of the playing area."
, // VARIANT_8550_HANDICAP
""
, // VARIANT_8550_SQUASH
"There are two players who alternately hit the ball into the court." \
" The right hand player is the one that hits first; it is then the left hand player's turn." \
" Each player is enabled alternately to ensure that the proper sequence of play is followed." \
" The ball is coloured to the colour of the bat of the next player who's to hit the ball."
, // VARIANT_8550_PRACTICE
"This game is similar to squash except that there is only one player." \
" The left score counts misses; the right score counts hits." \
" The game ends when either 15 misses occur or when 15 consecutive hits are made without a miss." \
" The right score is reset if it is not 15 and a miss occurs."
, // VARIANT_8550_RIFLE1
"This game has a large target which bounces randomly about the screen." \
" A photocell in the rifle is aimed at the target." \
" When the trigger is pulled, the shot counter is incremented and, if the rifle is on target, the hit counter is incremented, a hit noise is generated and the target is blanked for a short period." \
" After 15 shots the score appears but the game can still continue without additional scoring."
, // VARIANT_8550_RIFLE2
"In this game, the ball traverses the screen from left to right under control of the manual serve button." \
" Otherwise the game is as described for rifle game #1."
, // VARIANT_8600_TENNIS
"Each player can only move around his side of the court." \
" The game will start when the player whose turn it is to serve, depresses his service button." \
" The service will automatically change every 5 points scored." \
" At service the ball will move away from the service point with a random angle but always toward the net."
, // VARIANT_8600_HOCKEY
"The forwards of both sides have feedom to move over the entire playing area." \
" The goal keepers will be locked in the horizontal axis in front of their respective goals but will move in the vertical axis in the same manner as the forwards.\r\n" \
"  The game starts when both players have depressed their service buttons." \
" The ball will move away from the face off point with a randomly selected angle in either direction."
, // VARIANT_8600_SOCCER
"The motion of the players is as in the hockey game." \
" The game will start when the loser of the previous goal depresses his service button." \
" The ball will move away from the kickoff point with a randomly selected angle but always towards the goal of the winner of the previous goal."
, // VARIANT_8600_SQUASH
"Each player can move over the whole court." \
" The game will start when the player whose service it is, depresses his service button." \
" The ball moves off with a random angle toward the front wall." \
" The colour of the ball will change to the colour code of the next player to hit the ball." \
" Should the wrong player intercept or be hit by the ball it will be considered a fault." \
" Points will only be given if won on player's own service." \
" Points won on opponent's serve will only cause a service change."
, // VARIANT_8600_PRACTICE
"This game is a single player squash." \
" The right score counts the number of successive hits in the current game (to a maximum of 15), the left score the number of volleys played."
, // VARIANT_8600_GRIDBALL
"Each player has 3 sets of vertically moving barriers to block the ball from approaching his end and opening in the barriers to permit the ball to advance toward the opponent's end."
" The game starts when both players have depressed their service buttons." \
" The ball moves away from the face off point with a random angle in either direction."
, // VARIANT_8600_BASKETBALL
"The players must deflect the ball and cause it to enter the top of the goal to score." \
" The game starts when both players depress the service buttons." \
" The ball moves from the serve point with a random angle in either direction."
, // VARIANT_8600_BBPRACTICE
"The right counter displays the number of hits the player makes without scoring while the left counter shows the number of baskets made." \
" Play starts when the right serve button is depressed."
, // VARIANT_8600_TARGET1
"The single player target game is a game in which the player moves a cursor displayed on the TV screen over a moving target and depresses the serve button for shots to be taken." \
" The player has only one \"shot\" for each traversal of the screen by the target." \
" Additional \"shots\" are ignored after the first shot on each traverse." \
" The right score indicates the number of hits made and the left score indicates the misses." \
" A count is made on either score on each traverse of the target across the screen." \
" The target disappears when hit and the game ends when either of the scores displayed reaches 15."
, // VARIANT_8600_TARGET2
"The two player target game follows the same rules as the single player game except that both players control their own cursors and shot buttons." \
" The left score displayed is for the left player and the right score displayed is for the right player." \
" The target can only be shot at once on each traverse by either player but only recognizes the first hit." \
" The first player to reach a score of 15 wins the game."
};

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT       FLAG                 inframe,
                                  lmb;
IMPORT       ASCREEN              screen[BOXWIDTH][BOXHEIGHT];
IMPORT       UWORD                console[4],
                                  keypads[2][NUMKEYS];
IMPORT       UBYTE                OutputBuffer[18],
                                  sx[2],
                                  sy[2];
IMPORT       UBYTE*               IOBuffer;
IMPORT       ULONG                analog,
                                  autofire[2],
                                  collisions,
                                  cycles_2650,
                                  downframes,
                                  frames,
                                  jff[2],
                                  pong_machine,
                                  region,
                                  swapped,
                                  totalframes;
IMPORT       int                  ax[2],
                                  ay[4],
                                  colourset,
                                  console_start,
                                  console_a,
                                  console_b,
                                  console_reset,
                                  hostcontroller[2],
                                  key1,
                                  key2,
                                  key3,
                                  key4,
                                  machine,
                                  memmap,
                                  offset,
                                  p1rumble,
                                  p2rumble,
                                  recmode,
                                  requirebutton[2],
                                  retune,
                                  sensitivity[2],
                                  serializemode,
                                  showpositions,
                                  springloaded,
                                  spritemode,
                                  usemargins,
                                  wheremouse,
                                  whichgame;
IMPORT       FILE*                    MacroHandle;
IMPORT       struct MachineStruct     machines[MACHINES];
IMPORT       struct HostMachineStruct hostmachines[MACHINES];
#ifdef WIN32
    IMPORT   HWND                     SubWindowPtr[SUBWINDOWS];
#endif
#ifdef AMIGA
    IMPORT   struct Window*           SubWindowPtr[SUBWINDOWS];
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE       int                  angle,
                                  ballheight,
                                  ballinplay,
                                  bally,
                                  balldeltax,
                                  balldeltay,
                                  batsize[2]    = { 28, 28 },
                                  batx[5],
                                  bgc,
                                  blanktarget,
                                  botbaty[4],
                                  fgc,
                                  fresh_start   = TRUE,
                                  fresh_a       = TRUE,
                                  fresh_b       = TRUE,
                                  fresh_reset   = TRUE,
                                  hits,
                                  ltballx,
                                  ltc,
                                  newltballx,
                                  newbally,
                                  nowspeed,
                                  oldbatx[5],
                                  rifle1x,
                                  rifle1y,
                                  rifle2x,
                                  rifle2y,
                                  server,
                                  sgc,
                                  rtc, // "right colour" (different from the "real-time clock" variable)
                                  topbaty[4],
                                  whoseturn,
                                  y1, y2;
MODULE       ULONG                pongstop;
MODULE       UBYTE                playerfire[2],
                                  pongscreen[128][312],
                                  shot[2];
MODULE       FLAG                 gameover,
                                  freshfire[2],
                                  served[2],
                                  teamleft,
                                  teamright;

// MODULE ARRAYS----------------------------------------------------------

MODULE const TEXT gridball_big[228 + 1] =
  /* big gap       (20)            */ "...................."                                                       \
  /* big barrier   (74)            */ "##########################################################################" \
  /* big gap       (20)            */ "...................."                                                       \
  /* big barrier   (74)            */ "##########################################################################" \
  /* small gap     ( 4) (PAL only) */ "...."                                                                       \
  /* small barrier (36) (PAL only) */ "####################################",
gridball_sml[228 + 1] =
  /* medium gap    (10)            */ ".........."                            \
  /* medium barrier(37)            */ "#####################################" \
  /* medium gap    (10)            */ ".........."                            \
  /* medium barrier(37)            */ "#####################################" \
  /* medium gap    (10)            */ ".........."                            \
  /* medium barrier(37)            */ "#####################################" \
  /* medium gap    (10)            */ ".........."                            \
  /* medium barrier(37)            */ "#####################################" \
  /* small gap     ( 4) (PAL only) */ "...."                                  \
  /* small barrier (36) (PAL only) */ "####################################";

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void pong_drawdigit(int whichbat, int startx, int value);
MODULE __inline void pong_emuinput(void);
MODULE void pong_playerinput(int source, int dest);
MODULE void newball1(void);
MODULE void newball2(void);
MODULE void makesound(int pitch);
MODULE void calcangle(int whichbat);
MODULE void pong_drawpixel(int x, int y, int colour);
MODULE void pong_drawbgpixel(int x, int y, int colour);
MODULE void pong_drawbouncepixel(int x, int y, int colour, FLAG visible, int direction);
MODULE void pong_drawscorepixel(int x, int y, int colour, int player);
MODULE void drawfield(void);
MODULE void stopgame(void);
MODULE void lt_hit_rt(int batx, int oldbatx);
MODULE FLAG lt_hit_lt(int batx, int oldbatx);
MODULE FLAG rt_hit_lt(int batx, int oldbatx);
MODULE void rt_hit_rt(int batx, int oldbatx);
MODULE void lt_hit_either(int batx, int oldbatx);
MODULE void rt_hit_either(int batx, int oldbatx);
MODULE void checkrifle(int player);
MODULE UBYTE predict(int xmin, int xmax, int ymin, int ymax);
MODULE void position_bats(void);
MODULE void collide_bats(void);
MODULE void fixangle(void);
MODULE void calcspeed(void);

// CODE-------------------------------------------------------------------

EXPORT void pong_simulate(void)
{   PERSIST   UBYTE ballcolour;
    FAST      UBYTE prediction,
                    tgc; // teammate colour
    TRANSIENT int   i,
                    leadingx, leadingy,
                    x,    y,
                    xmin, xmax,
                    ymin, ymax;

    inframe = TRUE;

    if (region == REGION_PAL)
    {   y1 =  44;
        y2 = 275;
    } else
    {   y1 =  42;
        y2 = 233;
    }

    if (memmap == MEMMAP_8600)
    {                      teamleft =        teamright = FALSE;
    } else
    {   // assert(memmap == MEMMAP_8550);
        switch (players)
        {
        case  PLAYERS_2:   teamleft =        teamright = FALSE;
        acase PLAYERS_3LT: teamleft = TRUE;  teamright = FALSE;
        acase PLAYERS_3RT: teamleft = FALSE; teamright = TRUE;
        acase PLAYERS_4:   teamleft =        teamright = TRUE;
    }   }

    if (frames >= pongstop) // not ==
    {   ponghertz = 0;
        playsound(FALSE);
    }

    if
    (   pong_variant == VARIANT_8550_RIFLE1
     || pong_variant == VARIANT_8550_RIFLE2
     || pong_variant == VARIANT_8600_TARGET1
     || pong_variant == VARIANT_8600_TARGET2
    )
    {   ballheight = 7; // ie. -7..+7
    } elif (memmap == MEMMAP_8550 && pong_machine == PONGMACHINE_1976)
    {   ballheight = 1; // ie. -1..+1
    } else
    {   ballheight = 2; // ie. -2..+2
    }

    if (colourset == GREYSCALE)
    {   fgc = ltc = WHITE;
        sgc = GREY1;
        if (memmap == MEMMAP_8550 && playerid)
        {   bgc = GREY1;
            rtc = BLACK;
        } else
        {   bgc = BLACK;
            rtc = WHITE;
    }   }
    else
    {   fgc = variantinfo[pong_variant].fgc;
        bgc = variantinfo[pong_variant].bgc;
        ltc = variantinfo[pong_variant].ltc;
        rtc = variantinfo[pong_variant].rtc;
        sgc = BLACK;
    }

    drawfield();

    if
    (   pong_variant != VARIANT_8550_RIFLE1
     && pong_variant != VARIANT_8550_RIFLE2
     && pong_variant != VARIANT_8600_TARGET1
     && pong_variant != VARIANT_8600_TARGET2
    )
    {   position_bats();
        collide_bats();
        for (i = 0; i < variantinfo[pong_variant].bats; i++)
        {   oldbatx[i] = batx[i];
    }   }

    if (ballinplay)
    {   ymin = bally - ballheight;
        ymax = bally + ballheight;
        if
        (   pong_variant == VARIANT_8550_RIFLE1
         || pong_variant == VARIANT_8550_RIFLE2
         || pong_variant == VARIANT_8600_TARGET1
         || pong_variant == VARIANT_8600_TARGET2
        )
        {   xmin = ltballx - 1;
            xmax = rtballx + 1;
        } else
        {   xmin = ltballx;
            xmax = rtballx;
        }
        if (balldeltax >= 1)
        {   leadingx = xmax;
        } else
        {   leadingx = xmin;
        }
        if (balldeltay >= 1)
        {   leadingy = ymax;
        } else
        {   leadingy = ymin;
        }

        // leadingx/y = the x/y-coord of the current "front" of the ball
        // we check an area in front of this, from 1..delta
        prediction = predict(leadingx, leadingx + balldeltax, leadingy, leadingy + balldeltay);
        if (prediction == 1)
        {   switch (pong_variant)
            {
            case VARIANT_8600_BBPRACTICE:
                score[0]++;
            acase VARIANT_8550_TENNIS:
            case VARIANT_8550_SOCCER:
            case VARIANT_8550_HANDICAP:
            case VARIANT_8600_TENNIS:
            case VARIANT_8600_GRIDBALL:
            case VARIANT_8600_HOCKEY:
            case VARIANT_8600_BASKETBALL:
                score[1]++;
            acase VARIANT_8600_SOCCER:
                score[1]++;
                server = 0; // loser serves
            }
            makesound(TONE_SCORE);
            if (score[0] < 15 && score[1] < 15)
            {   newball1();
            } else
            {   stopgame();
        }   }
        elif (prediction == 2)
        {   switch (pong_variant)
            {
            case VARIANT_8550_SQUASH:
                if (whoseturn == 0)
                {   score[1]++;
                } else
                {   // assert(whoseturn == 1);
                    score[0]++;
                }
            acase VARIANT_8600_SQUASH:
                if (whoseturn == 0)
                {   if (server == 1) score[1]++; else server = 1;
                } else
                {   // assert(whoseturn == 1);
                    if (server == 0) score[0]++; else server = 0;
                }
            acase VARIANT_8600_TARGET1:
                if (shot[0] < 2)
                {   score[0]++;
                }
            acase VARIANT_8550_TENNIS:
            case VARIANT_8550_SOCCER:
            case VARIANT_8550_HANDICAP:
            case VARIANT_8600_TENNIS:
            case VARIANT_8600_HOCKEY:
            case VARIANT_8600_GRIDBALL:
            case VARIANT_8600_BASKETBALL:
                score[0]++;
            acase VARIANT_8600_SOCCER:
                score[0]++;
                server = 1; // loser serves
            acase VARIANT_8550_PRACTICE:
            case VARIANT_8600_PRACTICE:
                score[0]++;
                score[1] = 0;
            }
            makesound(TONE_SCORE);
            if (score[0] < 15 && score[1] < 15)
            {   newball1();
            } else
            {   stopgame();
        }   }
        else
        {   if (prediction & 4) // vertical wall (horizontal bouncing)
            {   balldeltax = -balldeltax;
                makesound(TONE_BOUNCE);
            }
            if (prediction & 8) // horizontal wall (vertical bouncing)
            {   balldeltay = -balldeltay;
                makesound(TONE_BOUNCE);
        }   }

        newltballx = ltballx + balldeltax;
        newbally   =   bally + balldeltay;
        fixangle();
    }

    if
    (   !gameover
     && !ballinplay
     && (   (   memmap == MEMMAP_8550
             && (   serving == SERVING_AUTO
                 || (pong_b && fresh_b)
            )   )
         || (   memmap == MEMMAP_8600
             && (   (server != 2 && served[server]        )
                 || (server == 2 && served[0] && served[1])
                 ||  server == 3
    )   )   )   )
    {   newball2();
    }

    rifle1x = gx[swapped ? 1 : 0];
    rifle1y = gy[swapped ? 1 : 0];
    rifle2x = gx[swapped ? 0 : 1];
    rifle2y = gy[swapped ? 0 : 1];
    if (usemargins)
    {   rifle1x /= 2; // convert to pong pixels
        rifle2x /= 2; // convert to pong pixels
    } else
    {   rifle1x--;    // to make up for the extra half a "pong pixel"
        rifle1x /= 2; // convert to pong pixels
        rifle1x += (27-2) - PONGXMARGIN;
        rifle2x--;    // to make up for the extra half a "pong pixel"
        rifle2x /= 2; // convert to pong pixels
        rifle2x += (27-2) - PONGXMARGIN;
        if (region == REGION_PAL)
        {   rifle1y += 44 - PONGYMARGIN;
            rifle2y += 44 - PONGYMARGIN;
        } else
        {   rifle1y += 42 - PONGYMARGIN;
            rifle2y += 42 - PONGYMARGIN;
    }   }

    if
    (   (   pong_variant == VARIANT_8550_RIFLE1
         || pong_variant == VARIANT_8550_RIFLE2
         || pong_variant == VARIANT_8600_TARGET1
         || pong_variant == VARIANT_8600_TARGET2
        )
     && ballinplay
    )
    {   if (blanktarget)
        {   if (memmap == MEMMAP_8550)
            {   blanktarget--;
        }   }
        else
        {   checkrifle(0);
            if (pong_variant == VARIANT_8600_TARGET2)
            {   checkrifle(1);
            }
            if (score[0] >= 15 || score[1] >= 15)
            {   ballinplay = FALSE;
    }   }   }

    ltballx += balldeltax;
    bally   += balldeltay;

    if (spritemode != SPRITEMODE_INVISIBLE)
    {   // bats
        for (i = 0; i < variantinfo[pong_variant].bats; i++)
        {   if (variantinfo[pong_variant].batinfo[i].player == 0)
            {   for (y = topbaty[0]; y <= botbaty[0]; y++)
                {   pong_drawpixel(batx[i], y, ltc);
                }
                if (teamleft)
                {   tgc = ltc;
#ifdef TEAMCOLOURS
                    if (tgc == ORANGE) tgc = PINK; elif (tgc == PINK) tgc = ORANGE; elif (tgc < 8) tgc += 8; else tgc -= 8;
#endif
                    for (y = topbaty[2]; y <= botbaty[2]; y++)
                    {   pong_drawpixel(batx[i], y, tgc);
            }   }   }
            else
            {   for (y = topbaty[1]; y <= botbaty[1]; y++)
                {   pong_drawpixel(batx[i], y, rtc);
                }
                if (teamright)
                {   tgc = rtc;
#ifdef TEAMCOLOURS
                    if (tgc == ORANGE) tgc = PINK; elif (tgc == PINK) tgc = ORANGE; elif (tgc < 8) tgc += 8; else tgc -= 8;
#endif
                    for (y = topbaty[3]; y <= botbaty[3]; y++)
                    {   pong_drawpixel(batx[i], y, tgc);
        }   }   }   }

        if (spritemode == SPRITEMODE_NUMBERED)
        {   if (pong_variant == VARIANT_8550_SQUASH || pong_variant == VARIANT_8600_SQUASH)
            {   for (y = topbaty[whoseturn]; y <= botbaty[whoseturn]; y += 2)
                {   pong_drawpixel(batx[whoseturn], y, colourset == GREYSCALE ? GREY1 : WHITE);
                }
                if (whoseturn == 0 && teamleft)
                {   for (y = topbaty[2]; y <= botbaty[2]; y += 2)
                    {   pong_drawpixel(batx[0], y, colourset == GREYSCALE ? GREY1 : WHITE);
                }   }
                elif (whoseturn == 1 && teamright)
                {   for (y = topbaty[3]; y <= botbaty[3]; y += 2)
                    {   pong_drawpixel(batx[1], y, colourset == GREYSCALE ? GREY1 : WHITE);
            }   }   }
            if (pong_variant == VARIANT_8600_TENNIS || pong_variant == VARIANT_8600_SQUASH)
            {   pong_drawpixel(batx[server], topbaty[server] - 2, colourset == GREYSCALE ? GREY1 : WHITE);
                pong_drawpixel(batx[server], botbaty[server] + 2, colourset == GREYSCALE ? GREY1 : WHITE);
        }   }

        if (ballinplay)
        {   switch (pong_variant)
            {
            case VARIANT_8550_SQUASH:
                if (pong_machine != PONGMACHINE_8550)
                {   ballcolour = WHITE;
                } elif (whoseturn == 0)
                {   ballcolour = ltc;
                } else
                {   // assert(whoseturn == 1);
                    ballcolour = rtc;
                }
            acase VARIANT_8600_SQUASH:
                if (whoseturn == 0)
                {   ballcolour = ltc;
                } else
                {   // assert(whoseturn == 1);
                    ballcolour = rtc;
                }
            adefault:
                ballcolour = WHITE;
            }

            switch (pong_variant)
            {
            case VARIANT_8550_TENNIS:
            case VARIANT_8550_SOCCER:
            case VARIANT_8550_HANDICAP:
            case VARIANT_8550_SQUASH:
            case VARIANT_8550_PRACTICE:
            case VARIANT_8600_TENNIS:
            case VARIANT_8600_HOCKEY:
            case VARIANT_8600_SOCCER:
            case VARIANT_8600_SQUASH:
            case VARIANT_8600_PRACTICE:
            case VARIANT_8600_GRIDBALL:
            case VARIANT_8600_BASKETBALL:
            case VARIANT_8600_BBPRACTICE:
                // 2*5 or 1*3
                for (x = ltballx; x <= rtballx; x++)
                {   for (y = bally - ballheight; y <= bally + ballheight; y++)
                    {   pong_drawpixel(x, y, ballcolour);
                }   }
            acase VARIANT_8550_RIFLE1:
            case VARIANT_8550_RIFLE2:
            case VARIANT_8600_TARGET1:
            case VARIANT_8600_TARGET2:
                if (!blanktarget)
                {   // 4*15
                    for (x = ltballx - 1; x <= rtballx + 1; x++)
                    {   for (y = bally - ballheight; y <= bally + ballheight; y++)
                        {   pong_drawpixel(x, y, ballcolour);
    }   }   }   }   }   }

    if (pong_variant == VARIANT_8600_TARGET1 || pong_variant == VARIANT_8600_TARGET2)
    {   // 7*21
        for (x = rifle1x - 3; x <= rifle1x + 3; x++)
        {   for (y = rifle1y - 1; y <= rifle1y + 1; y++)
            {   if (x >= 27-2 && x <= 99-22 && y >= y1 && y <= y2)
                {   pong_drawpixel(x, y, ltc); // horizontal bar of crosshair
        }   }   }
        for (y = rifle1y - 10; y <= rifle1y + 10; y++)
        {   if (rifle1x >= 27-2 && rifle1x <= 99-2 && y >= y1 && y <= y2)
            {   pong_drawpixel(rifle1x, y, ltc); // vertical bar of crosshair
        }   }

        if (pong_variant == VARIANT_8600_TARGET2)
        {   for (x = rifle2x - 3; x <= rifle2x + 3; x++)
            {   for (y = rifle2y - 1; y <= rifle2y + 1; y++)
                {   if (x >= 27-2 && x <= 99-2 && y >= y1 && y<= y2)
                    {   pong_drawpixel(x, y, rtc); // horizontal bar of crosshair
            }   }   }
            for (y = rifle2y - 10; y <= rifle2y + 10; y++)
            {   if (rifle2x >= 27-2 && rifle2x <= 99-2 && y >= y1 && y <= y2)
                {   pong_drawpixel(rifle2x, y, rtc); // vertical bar of crosshair
    }   }   }   }

    if (showpositions)
    {   if (rifle1x >= 27-2 && rifle1x <= 99-2 && rifle1y >= y1 && rifle1y <= y2)
        {   pong_drawpixel(rifle1x, rifle1y, ltc); // or perhaps use RED
        }
        if
        (   (   pong_variant == VARIANT_8550_TENNIS
             || pong_variant == VARIANT_8550_SOCCER
             || pong_variant == VARIANT_8550_HANDICAP
             || pong_variant == VARIANT_8550_SQUASH
             || pong_variant == VARIANT_8600_TENNIS
             || pong_variant == VARIANT_8600_HOCKEY
             || pong_variant == VARIANT_8600_SOCCER
             || pong_variant == VARIANT_8600_SQUASH
             || pong_variant == VARIANT_8600_GRIDBALL
             || pong_variant == VARIANT_8600_BASKETBALL
             || pong_variant == VARIANT_8600_TARGET2
            ) // ie. the 2-player games
         && rifle1x >= 27-2
         && rifle1x <= 99-2
         && rifle1y >= y1
         && rifle1y <= y2
        )
        {   pong_drawpixel(rifle2x, rifle2y, rtc); // or perhaps use BLUE
    }   }

    wa_checkinput(); // everything must be drawn first so magnifier works correctly
    pong_emuinput();

    cycles_2650++; // so it ticks over enough to make eg. WAV recordings possible
    endofframe(bgc);
}

MODULE void pong_drawdigit(int whichbat, int startx, int value)
{   FAST    int   thecolour,
                  x, y;
    PERSIST const UBYTE pong_digitglyph[10][5] = {
    { 7, // ###
      5, // #.#
      5, // #.#
      5, // #.#
      7  // ###
    },
    { 1, // ..#
      1, // ..#
      1, // ..#
      1, // ..#
      1  // ..#
    },
    { 7, // ###
      1, // ..#
      7, // ###
      4, // #..
      7  // ###
    },
    { 7, // ###
      1, // ..#
      7, // ###
      1, // ..#
      7  // ###
    },
    { 5, // #.#
      5, // #.#
      7, // ###
      1, // ..#
      1  // ..#
    },
    { 7, // ###
      4, // #..
      7, // ###
      1, // ..#
      7  // ###
    },
    { 7, // ###
      4, // #..
      7, // ###
      5, // #.#
      7  // ###
    },
    { 7, // ###
      1, // ..#
      1, // ..#
      1, // ..#
      1  // ..#
    },
    { 7, // ###
      5, // #.#
      7, // ###
      5, // #.#
      7  // ###
    },
    { 7, // ###
      5, // #.#
      7, // ###
      1, // ..#
      7  // ###
    } };

    if (memmap == MEMMAP_8550)
    {   if (pong_machine != PONGMACHINE_8550)
        {   thecolour = fgc;
        } elif (whichbat == 0)
        {   thecolour = ltc;
        } else
        {   thecolour = rtc;
        }
        for (x = 0; x <= 2; x++)
        {   for (y = 0; y <= 4; y++)
            {   if (pong_digitglyph[value][y] & (4 >> x))
                {   if (region == REGION_PAL)
                    {   pong_drawpixel(startx + (x * 2)    , 50 + (y * 6), thecolour);
                        pong_drawpixel(startx + (x * 2) + 1, 50 + (y * 6), thecolour);
                        pong_drawpixel(startx + (x * 2)    , 51 + (y * 6), thecolour);
                        pong_drawpixel(startx + (x * 2) + 1, 51 + (y * 6), thecolour);
                        pong_drawpixel(startx + (x * 2)    , 52 + (y * 6), thecolour);
                        pong_drawpixel(startx + (x * 2) + 1, 52 + (y * 6), thecolour);
                        pong_drawpixel(startx + (x * 2)    , 53 + (y * 6), thecolour);
                        pong_drawpixel(startx + (x * 2) + 1, 53 + (y * 6), thecolour);
                        pong_drawpixel(startx + (x * 2)    , 54 + (y * 6), thecolour);
                        pong_drawpixel(startx + (x * 2) + 1, 54 + (y * 6), thecolour);
                        pong_drawpixel(startx + (x * 2)    , 55 + (y * 6), thecolour);
                        pong_drawpixel(startx + (x * 2) + 1, 55 + (y * 6), thecolour);
                    } else
                    {   // assert(region == REGION_NTSC);
                        pong_drawpixel(startx + (x * 2)    , 48 + (y * 4), thecolour);
                        pong_drawpixel(startx + (x * 2) + 1, 48 + (y * 4), thecolour);
                        pong_drawpixel(startx + (x * 2)    , 49 + (y * 4), thecolour);
                        pong_drawpixel(startx + (x * 2) + 1, 49 + (y * 4), thecolour);
                        pong_drawpixel(startx + (x * 2)    , 50 + (y * 4), thecolour);
                        pong_drawpixel(startx + (x * 2) + 1, 50 + (y * 4), thecolour);
                        pong_drawpixel(startx + (x * 2)    , 51 + (y * 4), thecolour);
                        pong_drawpixel(startx + (x * 2) + 1, 51 + (y * 4), thecolour);
    }   }   }   }   }
    else
    {   // assert(memmap == MEMMAP_8600);
        if (whichbat == 0)
        {   thecolour = ltc;
        } else
        {   thecolour = rtc;
        }
        for (x = 0; x <= 2; x++)
        {   for (y = 0; y <= 4; y++)
            {   if (pong_digitglyph[value][y] & (4 >> x))
                {   if (region == REGION_PAL)
                    {   pong_drawpixel(startx +  x         , 50 + (y * 3), thecolour);
                        pong_drawpixel(startx +  x         , 51 + (y * 3), thecolour);
                        pong_drawpixel(startx +  x         , 52 + (y * 3), thecolour);
                    } else
                    {   // assert(region == REGION_NTSC);
                        pong_drawpixel(startx +  x         , 48 + (y * 2), thecolour);
                        pong_drawpixel(startx +  x         , 49 + (y * 2), thecolour);
}   }   }   }   }   }

MODULE __inline void pong_emuinput(void)
{   FAST UBYTE t;
    FAST FLAG  update;

    ReadJoystick(0);
    ReadJoystick(1);
    // must always do pong_playerinput(foo, 0) then pong_playerinput(foo, 1)
    if (swapped)
    {   pong_playerinput(1, 0);
        pong_playerinput(0, 1);
    } else
    {   pong_playerinput(0, 0);
        pong_playerinput(1, 1);
    }
    p1rumble =
    p2rumble = 0;

    if (recmode == RECMODE_PLAY)
    {   t           = IOBuffer[offset++]; // 14
        pong_start  = (t & 1) ? TRUE : FALSE;
        pong_a      = (t & 2) ? TRUE : FALSE;
        pong_b      = (t & 4) ? TRUE : FALSE;
        pong_reset  = (t & 8) ? TRUE : FALSE;
        fresh_start =
        fresh_a     =
        fresh_b     =
        fresh_reset = TRUE;

        update      = FALSE;
        // bit 7 of byte 14 is currently unused
        switch (t & 0x60)
        {
        case  0x00: if (players != PLAYERS_2         ) { players = PLAYERS_2;          update = TRUE; }
        acase 0x20: if (players != PLAYERS_3LT       ) { players = PLAYERS_3LT;        update = TRUE; }
        acase 0x40: if (players != PLAYERS_3RT       ) { players = PLAYERS_3RT;        update = TRUE; }
        acase 0x60: if (players != PLAYERS_4         ) { players = PLAYERS_4;          update = TRUE; }
        }
        switch (t & 0x10)
        {
        case  0x00: if (pong_machine != 0            ) { pong_machine = 0;             update = TRUE; }
        acase 0x10: if (pong_machine != 1            ) { pong_machine = 1;             update = TRUE; }
        }
        t           = IOBuffer[offset++]; // 15
        switch (t & 0xC0)
        {
        case  0x00: if (pongspeed != PONGSPEED_SLOW  ) { pongspeed = PONGSPEED_SLOW;   update = TRUE; }
        acase 0x40: if (pongspeed != PONGSPEED_FAST  ) { pongspeed = PONGSPEED_FAST;   update = TRUE; }
        acase 0x80: if (pongspeed != PONGSPEED_RANDOM) { pongspeed = PONGSPEED_RANDOM; update = TRUE; }
        }
        switch (t & 0x30)
        {
        case  0x00: if (angles    != ANGLES_2        ) { angles    = ANGLES_2;         update = TRUE; }
        acase 0x10: if (angles    != ANGLES_4        ) { angles    = ANGLES_4;         update = TRUE; }
        acase 0x20: if (angles    != ANGLES_RANDOM   ) { angles    = ANGLES_RANDOM;    update = TRUE; }
        }
        switch (t & 0x08)
        {
        case  0x00: if (lockhoriz != 0               ) { lockhoriz = 0;                update = TRUE; }
        acase 0x08: if (lockhoriz != 1               ) { lockhoriz = 1;                update = TRUE; }
        }
        switch (t & 0x04)
        {
        case  0x00: if (serving   != 0               ) { serving   = 0;                update = TRUE; }
        acase 0x04: if (serving   != 1               ) { serving   = 1;                update = TRUE; }
        }
        if    ((t & 0x03)         != batvalue        ) { batvalue  = (t & 0x03);       update = TRUE; updatebatsize(); }
        if (update)
        {   pong_updatedips();
    }   }
    else
    {   // assert(recmode == RECMODE_NORMAL || recmode == RECMODE_RECORD);

        pong_start = pong_a = pong_b = pong_reset = FALSE;

        domouse();

        if (KeyDown(console[0]) || (jff[0] & JOYSTART) || (jff[1] & JOYSTART) || console_start || wheremouse == 0)
        {   if (fresh_start)
            {   fresh_start = FALSE;
                pong_start  = TRUE;
        }   }
        else
        {   fresh_start     = TRUE;
        }
        if (KeyDown(console[1]) || (jff[0] & JOYA    ) || (jff[1] & JOYA    ) || console_a     || wheremouse == 1)
        {   if (fresh_a)
            {   fresh_a     = FALSE;
                pong_a      = TRUE;
        }   }
        else
        {   fresh_a         = TRUE;
        }
        if (KeyDown(console[2]) || (jff[0] & JOYB    ) || (jff[1] & JOYB    ) || console_b     || wheremouse == 2)
        {   if (fresh_b)
            {   fresh_b     = FALSE;
                pong_b      = TRUE;
        }   }
        else
        {   fresh_b         = TRUE;
        }
        if (KeyDown(console[3])                                               || console_reset || wheremouse == 3)
        {   if (fresh_reset)
            {   fresh_reset = FALSE;
                pong_reset  = TRUE;
        }   }
        else
        {   fresh_reset     = TRUE;
        }

        if (recmode == RECMODE_RECORD)
        {   // assert(MacroHandle);

            OutputBuffer[ 0] = ax[0];         OutputBuffer[ 8] = ax[1];
            OutputBuffer[ 1] = ay[0];         OutputBuffer[ 9] = ay[1];
            OutputBuffer[ 2] = playerfire[0]; OutputBuffer[10] = playerfire[1];
            OutputBuffer[ 3] = gx[0] / 256;   OutputBuffer[11] = gx[1] / 256;
            OutputBuffer[ 4] = gx[0] % 256;   OutputBuffer[12] = gx[1] % 256;
            OutputBuffer[ 5] = gy[0] / 256;   OutputBuffer[13] = gy[1] / 256;
            OutputBuffer[ 6] = gy[0] % 256;   OutputBuffer[14] = gy[1] % 256;
            OutputBuffer[ 7] = ay[2];         OutputBuffer[15] = ay[3];

            t =  0;                  // bit  7 is currently unused
            t |= players      << 5;  // bits 6..5
            t |= pong_machine << 4;  // bit  4
            t |= pong_reset ? 8 : 0; // bit  3
            t |= pong_b     ? 4 : 0; // bit  2
            t |= pong_a     ? 2 : 0; // bit  1
            t |= pong_start ? 1 : 0; // bit  0
            OutputBuffer[16] = t;

            t =  0;
            t |= pongspeed    << 6;  // bits 7..6
            t |= angles       << 4;  // bits 5..4
            t |= lockhoriz    << 3;  // bit  3
            t |= serving      << 2;  // bit  2
            t |= batvalue         ;  // bits 1..0
            OutputBuffer[17] = t;

            DISCARD fwrite(OutputBuffer, 18, 1, MacroHandle);
            // should really check return code of fwrite()
            // 18 bytes per frame * 50 frames per second =  900 bytes/sec (for PAL )
            // 18 bytes per frame * 60 frames per second = 1080 bytes/sec (for NTSC)
    }   }

    if (console_start) console_start--;
    if (console_a    ) console_a--;
    if (console_b    ) console_b--;
    if (console_reset) console_reset--;

    if (pong_start)
    {   pong_newgame();
    }
    if (pong_a)
    {   if (pong_variant == 6 && memmap == MEMMAP_8550)
        {   pong_variant = 0;
        } elif (pong_variant == 16 && memmap == MEMMAP_8600)
        {   pong_variant = 7;
        } else
        {   pong_variant++;
        }
        dogamename();
        generate_autotext();
        reopen_subwindow(SUBWINDOW_GAMEINFO);
        pong_updatedips();
    }
    if (pong_reset)
    {   if
        (   (memmap == MEMMAP_8550 && batvalue == 1)
         || (memmap == MEMMAP_8600 && batvalue == 3)
        )
        {   batvalue = 0;
        } else
        {   batvalue++;
        }
        updatebatsize();
        pong_updatedips();
}   }

MODULE void pong_playerinput(int source, int dest)
{   FAST FLAG  moved;
    FAST UBYTE t;
    FAST ULONG jg;
    FAST int   centrebally, centrebaty;

    // dest is which side   (0 or 1) you want to set the registers of.
    // source is which side (0 or 1) you want to use to do it.

    if (recmode == RECMODE_PLAY)
    {   t = IOBuffer[offset++];                    // 0 or  8
        ax[dest    ] =  sx[dest] = t;
        t = IOBuffer[offset++];                    // 1 or  9
        ay[dest    ] =  sy[dest] = t;
        playerfire[dest] = IOBuffer[offset++];     // 2 or 10
        gx[dest    ] =  IOBuffer[offset++] * 256;  // 3 or 11
        gx[dest    ] += IOBuffer[offset++];        // 4 or 12
        gy[dest    ] =  IOBuffer[offset++] * 256;  // 5 or 13
        gy[dest    ] += IOBuffer[offset++];        // 6 or 14
        ay[dest + 2] =  IOBuffer[offset++];        // 7 or 15
    } else
    {   // assert(recmode == RECMODE_NORMAL || recmode == RECMODE_RECORD);

        if
        (   robotspeed[dest] >= 1
         && pong_variant != VARIANT_8550_RIFLE1
         && pong_variant != VARIANT_8550_RIFLE2
         && pong_variant != VARIANT_8600_TARGET1
         && pong_variant != VARIANT_8600_TARGET2
         && pong_variant != VARIANT_8600_GRIDBALL
        )
        {   if (ballinplay)
            {   if ((pong_variant != VARIANT_8550_SQUASH && pong_variant != VARIANT_8600_SQUASH) || whoseturn == dest)
                {   centrebally = bally + 2;
                    if (robotspeed[dest] == ROBOTSPEED_INFINITE)
                    {   ay[dest] = centrebally - ((region == REGION_PAL) ? 32 : 10);
                    } else
                    {   centrebaty = topbaty[dest] + ((botbaty[dest] - topbaty[dest] + 1) / 2);
                        if (centrebaty < centrebally)
                        {   ay[dest] += robotspeed[dest]; // move down
                            if (ay[dest] > 255) ay[dest] = 255;
                        } elif (centrebaty > centrebally)
                        {   ay[dest] -= robotspeed[dest]; // move up
                            if (ay[dest] <   0) ay[dest] =   0;
            }   }   }   }
            else
            {   playerfire[dest] = 1;
        }   }
        else
        {   if
            (   hostcontroller[source] == CONTROLLER_1STDJOY
             || hostcontroller[source] == CONTROLLER_1STDPAD
          // || hostcontroller[source] == CONTROLLER_1STAJOY
             || hostcontroller[source] == CONTROLLER_1STAPAD
            )
            {   jg = jff[0];
            } elif
            (   hostcontroller[source] == CONTROLLER_2NDDJOY
             || hostcontroller[source] == CONTROLLER_2NDDPAD
             || hostcontroller[source] == CONTROLLER_2NDAJOY
             || hostcontroller[source] == CONTROLLER_2NDAPAD
            )
            {   jg = jff[1];
            } else
            {   jg = 0;
            }

            playerfire[dest] = 0;
            if (memmap == MEMMAP_8600 && autofire[source])
            {   if
                (   (   !requirebutton[source]
                     || (jg & (JOYFIRE1 | DAPTER_TOP))
                     || KeyDown(keypads[source][0])
                     || (hostcontroller[source] == CONTROLLER_TRACKBALL && lmb)
                    )
                 && ((frames % totalframes) < downframes)
                )
                {   // 1st firebutton
                    playerfire[dest] = 1;
            }   }
            elif
            (   (jg & (JOYFIRE1 | DAPTER_TOP))
             || KeyDown(keypads[source][0])
             || KeyDown(keypads[source][2])
             || (hostcontroller[source] == CONTROLLER_TRACKBALL && lmb)
            )
            {   // 1st firebutton
                playerfire[dest] = 1;
            }

            if (teamleft && dest == 0)
            {   moved = FALSE;
                if ((jg & JOYFIRE4) || KeyDown(keypads[source][ 8]) || (jff[source] & DAPTER_8)) // "8"
                {   moved = TRUE;
                    ay[2] -= sensitivity[source];
                    if (ay[2] < 0)
                    {   ay[2] = 0;
                }   }
                if ((jg & (JOYFIRE2 | JOYFIRE3)) || KeyDown(keypads[source][11]) || (jff[source] & DAPTER_0)) // "0"
                {   moved = TRUE;
                    ay[2] += sensitivity[source];
                    if (ay[2] > 255)
                    {   ay[2] = 255;
                }   }
                if (!moved && springloaded)
                {   ay[2] = machines[machine].digipos[1];
            }   }
            if (teamright && dest == 1)
            {   moved = FALSE;
                if ((jg & JOYFIRE4) || KeyDown(keypads[source][ 8]) || (jff[source] & DAPTER_8)) // "8"
                {   moved = TRUE;
                    ay[3] -= sensitivity[source];
                    if (ay[3] < 0)
                    {   ay[3] = 0;
                }   }
                if ((jg & (JOYFIRE2 | JOYFIRE3)) || KeyDown(keypads[source][11]) || (jff[source] & DAPTER_0)) // "0"
                {   moved = TRUE;
                    ay[3] += sensitivity[source];
                    if (ay[3] > 255)
                    {   ay[3] = 255;
                }   }
                if (!moved && springloaded)
                {   ay[3] = machines[machine].digipos[1];
            }   }

            engine_dopaddle(source, dest);
    }   }

    if (playerfire[0]) served[0] = TRUE;
    if (playerfire[1]) served[1] = TRUE;
}

MODULE void newball1(void)
{   ballinplay =
    served[0]  =
    served[1]  = FALSE;

    if
    (   pong_variant == VARIANT_8600_TENNIS
     &&  score[0] + score[1]      != 0
     && (score[0] + score[1]) % 5 == 0
    )
    {   if (server == 0)
        {   server = 1;
        } else
        {   server = 0;
}   }   }

MODULE void newball2(void)
{   UBYTE t;

    ltballx        = 63-2;
    bally          = (region == REGION_PAL ? 160 : 138);
    balldeltax     =
    balldeltay     = 0;
    hits           = 0;
    ballinplay     = TRUE;
    blanktarget    = 0;
    freshfire[0]   =
    freshfire[1]   = FALSE;
    shot[0]        =
    shot[1]        = 0;

    calcspeed();

    // horizontal
    switch (pong_variant)
    {
    case VARIANT_8600_TENNIS:
    case VARIANT_8600_SOCCER:
        if (server == 0)
        {   ltballx = 32-2;
            balldeltax = 1;
        } else
        {   // assert(server == 1);
            ltballx = 93-2;
            balldeltax = -1;
        }
    acase VARIANT_8550_PRACTICE:
    case VARIANT_8550_SQUASH:
    case VARIANT_8600_PRACTICE:
    case VARIANT_8600_SQUASH:
        ltballx     = 93-2;
        balldeltax  = -1;
    acase VARIANT_8550_TENNIS:
    case VARIANT_8550_SOCCER:
    case VARIANT_8550_HANDICAP:
    case VARIANT_8550_RIFLE1:
    case VARIANT_8600_HOCKEY:
    case VARIANT_8600_GRIDBALL:
    case VARIANT_8600_BASKETBALL:
    case VARIANT_8600_BBPRACTICE:
        t = getrandom(2);
        balldeltax = t ? -1 : 1;
    acase VARIANT_8550_RIFLE2:
    case VARIANT_8600_TARGET1:
    case VARIANT_8600_TARGET2:
        t = getrandom((region == REGION_PAL) ? 204 : 164);
        bally = t + (region == REGION_PAL ? 58 : 56); // 58..261 (PAL) or 56..219 (NTSC)
        ltballx = 32-2;
        balldeltax = 1;
    }
    fixx();

    // vertical
    angle = getrandom(4);
    if (angle >= 2)
    {   angle++; // 0..1, 3..4
    }
    if (memmap == MEMMAP_8550 && angles == ANGLES_2)
    {   if (angle == 0)
        {   angle = 1;
        } elif (angle == 4)
        {   angle = 3;
    }   }
    fixy();
}

MODULE void makesound(int pitch)
{   ponghertz = pitch;
    pongstop  = frames + 2;
    playsound(FALSE);

    if (pitch == TONE_BAT)
    {   hits++;
}   }

MODULE void calcangle(int whichbat)
{   FAST int teamsize;

    teamsize = batsize[whichbat & 1];

    switch (memmap)
    {
    case MEMMAP_8550:
        if (angles == ANGLES_RANDOM)
        {   angle = getrandom(4);
            if (angle >= 2)
            {   angle++; // 0..1, 3..4
        }   }
        elif (teamsize == 14)
        {   if   (newbally <= topbaty[whichbat] +  2) angle = 0; //  0.. 2
            elif (newbally <= topbaty[whichbat] +  6) angle = 1; //  3.. 6
            elif (newbally <= topbaty[whichbat] + 10) angle = 3; //  7..10
            else                                      angle = 4; // 11..13
        } else
        {   // assert(batsize[whichbat] == 28);
            if   (newbally <= topbaty[whichbat] +  6) angle = 0; //  0.. 6
            elif (newbally <= topbaty[whichbat] + 13) angle = 1; //  7..13
            elif (newbally <= topbaty[whichbat] + 20) angle = 3; // 14..20
            else                                      angle = 4; // 21..27
        }

        if (angles == ANGLES_2)
        {   if (angle == 0)
            {   angle = 1;
            } elif (angle == 4)
            {   angle = 3;
        }   }
    acase MEMMAP_8600:
        if (teamsize == 14)
        {   if   (newbally <= topbaty[whichbat] +  2) angle = 0; //  0.. 2 (3 rows)
            elif (newbally <= topbaty[whichbat] +  5) angle = 1; //  3.. 5 (3 rows)
            elif (newbally <= topbaty[whichbat] +  7) angle = 2; //  6.. 7 (2 rows)
            elif (newbally <= topbaty[whichbat] + 10) angle = 3; //  8..10 (3 rows)
            else                                      angle = 4; // 11..13 (3 rows)
        } else
        {   // assert(batsize[whichbat] == 28);
            if   (newbally <= topbaty[whichbat] +  4) angle = 0; //  0.. 4 (5 rows)
            elif (newbally <= topbaty[whichbat] + 10) angle = 1; //  5..10 (6 rows)
            elif (newbally <= topbaty[whichbat] + 16) angle = 2; // 11..16 (6 rows)
            elif (newbally <= topbaty[whichbat] + 20) angle = 3; // 17..22 (6 rows)
            else                                      angle = 4; // 23..27 (5 rows)
    }   }
    fixy();
}

MODULE void lt_hit_rt(int batx, int oldbatx)
{   if (ltballx > oldbatx && newltballx <= batx)
    {   if (newbally + 2 >= topbaty[0] && newbally - 2 <= botbaty[0])
        {   balldeltax = -balldeltax;
            calcspeed();
            fixx();
            // assert(balldeltax > 0);
            calcangle(0);
            makesound(TONE_BAT);
            p1rumble = PONGRUMBLE;
        }
        elif (teamleft && newbally + 2 >= topbaty[2] && newbally - 2 <= botbaty[2])
        {   balldeltax = -balldeltax;
            calcspeed();
            fixx();
            // assert(balldeltax > 0);
            calcangle(2);
            makesound(TONE_BAT);
            p1rumble = PONGRUMBLE;
}   }   }

MODULE FLAG lt_hit_lt(int batx, int oldbatx)
{   if (rtballx < oldbatx && newrtballx >= batx)
    {   if (newbally + 2 >= topbaty[0] && newbally - 2 <= botbaty[0])
        {   balldeltax = -balldeltax;
            calcspeed();
            fixx();
            // assert(balldeltax < 0);
            calcangle(0);
            makesound(TONE_BAT);
            p1rumble = PONGRUMBLE;
            return TRUE;
        } // implied else
        if (teamleft && newbally + 2 >= topbaty[2] && newbally - 2 <= botbaty[2])
        {   balldeltax = -balldeltax;
            calcspeed();
            fixx();
            // assert(balldeltax < 0);
            calcangle(2);
            makesound(TONE_BAT);
            p1rumble = PONGRUMBLE;
            return TRUE;
    }   }

    return FALSE;
}

MODULE FLAG rt_hit_lt(int batx, int oldbatx)
{   if (rtballx < oldbatx && newrtballx >= batx)
    {   if (newbally + 2 >= topbaty[1] && newbally - 2 <= botbaty[1])
        {   balldeltax = -balldeltax;
            calcspeed();
            fixx();
            // assert(balldeltax < 0);
            calcangle(1);
            makesound(TONE_BAT);
            p2rumble = PONGRUMBLE;
            return TRUE;
        } // implied else
        if (teamright && newbally + 2 >= topbaty[3] && newbally - 2 <= botbaty[3])
        {   balldeltax = -balldeltax;
            calcspeed();
            fixx();
            // assert(balldeltax < 0);
            calcangle(3);
            makesound(TONE_BAT);
            p2rumble = PONGRUMBLE;
            return TRUE;
    }   }

    return FALSE;
}

MODULE void rt_hit_rt(int batx, int oldbatx)
{   if (ltballx > oldbatx && newltballx <= batx)
    {   if (newbally + 2 >= topbaty[1] && newbally - 2 <= botbaty[1])
        {   balldeltax = -balldeltax;
            calcspeed();
            fixx();
            // assert(balldeltax > 0);
            calcangle(1);
            makesound(TONE_BAT);
            p2rumble = PONGRUMBLE;
        } elif (teamright && newbally + 2 >= topbaty[3] && newbally - 2 <= botbaty[3])
        {   balldeltax = -balldeltax;
            calcspeed();
            fixx();
            // assert(balldeltax > 0);
            calcangle(3);
            makesound(TONE_BAT);
            p2rumble = PONGRUMBLE;
}   }   }

MODULE void lt_hit_either(int batx, int oldbatx)
{   if ((ltballx > oldbatx && newltballx <= batx) || (rtballx < oldbatx && newrtballx >= batx))
    {   if (newbally + 2 >= topbaty[0] && newbally - 2 <= botbaty[0])
        {   balldeltax = -balldeltax;
            calcspeed();
            fixx();
            calcangle(0);
            makesound(TONE_BAT);
            p1rumble = PONGRUMBLE;
        } elif (teamleft && newbally + 2 >= topbaty[2] && newbally - 2 <= botbaty[2])
        {   balldeltax = -balldeltax;
            calcspeed();
            fixx();
            calcangle(2);
            makesound(TONE_BAT);
            p1rumble = PONGRUMBLE;
}   }   }

MODULE void lt_hit_forwards(int batx, int oldbatx)
{   if ((ltballx > oldbatx && newltballx <= batx) || (rtballx < oldbatx && newrtballx >= batx))
    {   if (newbally + 2 >= topbaty[0] && newbally - 2 <= botbaty[0])
        {   if (ltballx > oldbatx && newltballx <= batx)
            {   balldeltax = -balldeltax;
            }
            calcspeed();
            fixx();
            calcangle(0);
            makesound(TONE_BAT);
            p1rumble = PONGRUMBLE;
        } elif (teamleft && newbally + 2 >= topbaty[2] && newbally - 2 <= botbaty[2])
        {   if (ltballx > oldbatx && newltballx <= batx)
            {   balldeltax = -balldeltax;
            }
            calcspeed();
            fixx();
            calcangle(2);
            makesound(TONE_BAT);
            p1rumble = PONGRUMBLE;
}   }   }

MODULE void rt_hit_either(int batx, int oldbatx)
{   if ((ltballx > oldbatx && newltballx <= batx) || (rtballx < oldbatx && newrtballx >= batx))
    {   if (newbally + 2 >= topbaty[1] && newbally - 2 <= botbaty[1])
        {   balldeltax = -balldeltax;
            calcspeed();
            fixx();
            calcangle(1);
            makesound(TONE_BAT);
            p2rumble = PONGRUMBLE;
        } elif (teamright && newbally + 2 >= topbaty[3] && newbally - 2 <= botbaty[3])
        {   balldeltax = -balldeltax;
            calcspeed();
            fixx();
            calcangle(3);
            makesound(TONE_BAT);
            p2rumble = PONGRUMBLE;
}   }   }

MODULE void rt_hit_forwards(int batx, int oldbatx)
{   if ((ltballx > oldbatx && newltballx <= batx) || (rtballx < oldbatx && newrtballx >= batx))
    {   if (newbally + 2 >= topbaty[1] && newbally - 2 <= botbaty[1])
        {   if (rtballx < oldbatx && newrtballx >= batx)
            {   balldeltax = -balldeltax;
            }
            calcspeed();
            fixx();
            calcangle(1);
            makesound(TONE_BAT);
            p2rumble = PONGRUMBLE;
        } elif (teamright && newbally + 2 >= topbaty[3] && newbally - 2 <= botbaty[3])
        {   if (rtballx < oldbatx && newrtballx >= batx)
            {   balldeltax = -balldeltax;
            }
            calcspeed();
            fixx();
            calcangle(3);
            makesound(TONE_BAT);
            p2rumble = PONGRUMBLE;
}   }   }

MODULE void pong_drawpixel(int x, int y, int colour)
{   if (usemargins)
    {   x *= 2;
    } else
    {   if (memmap == MEMMAP_8550 && pong_machine == PONGMACHINE_1976)
        {   x -= (23-2) - PONGXMARGIN;
        } else
        {   x -= (27-2) - PONGXMARGIN;
        }
        x *= 2;
        x++; // to compensate for having an odd width
        if (region == REGION_PAL) y -= 44 - PONGYMARGIN; else y -= 42 - PONGYMARGIN;
    }

    if (x >=  0 && x < machines[machine].width     && y >= 0 && y < machines[machine].height)
    {   changepixel(x    , y, colour);
    }
    if (x >= -1 && x < machines[machine].width - 1 && y >= 0 && y < machines[machine].height)
    {   changepixel(x + 1, y, colour);
}   }

MODULE void pong_drawbgpixel(int x, int y, int colour)
{   if (usemargins)
    {   x *= 2;
    } else
    {   if (memmap == MEMMAP_8550 && pong_machine == PONGMACHINE_1976)
        {   x -= (23-2) - PONGXMARGIN;
        } else
        {   x -= (27-2) - PONGXMARGIN;
        }
        x *= 2;
        x++; // to compensate for having an odd width
        if (region == REGION_PAL) y -= 44 - PONGYMARGIN; else y -= 42 - PONGYMARGIN;
    }

    if (x >=  0 && x < machines[machine].width     && y >= 0 && y < machines[machine].height)
    {   changebgpixel(x    , y, colour);
    }
    if (x >= -1 && x < machines[machine].width - 1 && y >= 0 && y < machines[machine].height)
    {   changebgpixel(x + 1, y, colour);
}   }

MODULE void pong_drawbouncepixel(int x, int y, int colour, FLAG visible, int direction)
{   pongscreen[x][y] = (direction == DIR_VERT) ? 3 : 4;
    if (visible)
    {   pong_drawpixel(x, y, colour);
    } elif (spritemode == SPRITEMODE_NUMBERED)
    {   pong_drawpixel(x, y, colour);
}   }

MODULE void pong_drawscorepixel(int x, int y, int colour, int player)
{   pongscreen[x][y] = player + 1;
    if (spritemode == SPRITEMODE_NUMBERED)
    {   pong_drawpixel(x, y, colour);
}   }

EXPORT void pong_newgame(void)
{   int i;

    gameover         = FALSE;
    whoseturn        = 1;
    score[0]         =
    score[1]         = 0;
    ay[0]            =
    ay[1]            = 128;
    pongstop         = 0;
    ltballx          = 63-2;
    bally            = (region == REGION_PAL ? 160 : 138);
    balldeltax       =
    balldeltay       =
/*  playerfire[0]    =
    playerfire[1]    = FALSE; */
    served[0]        =
    served[1]        = FALSE;
    ballinplay       = FALSE; // important!
    gridy[0]         =
    gridy[1]         = 0;

    for (i = 0; i < variantinfo[pong_variant].bats; i++)
    {   if (memmap == MEMMAP_8550 && pong_machine == PONGMACHINE_1976)
        {   batx[i] = oldbatx[i] = variantinfo[pong_variant].batinfo[i].x_76;
        } else
        {   batx[i] = oldbatx[i] = variantinfo[pong_variant].batinfo[i].x_77;
    }   }
    if
    (   pong_variant == VARIANT_8600_HOCKEY
     || pong_variant == VARIANT_8600_GRIDBALL
     || pong_variant == VARIANT_8600_BASKETBALL
    )
    {   server = 2; // both players must press fire
    } elif
    (   pong_variant == VARIANT_8600_TARGET1
     || pong_variant == VARIANT_8600_TARGET2
    )
    {   server = 3; // autoserve
    } else // eg. 8600-Tennis, 8600-Soccer, 8600-BBPractice
    {   server = 0; // the left player always starts
}   }

MODULE void drawfield(void)
{   TRANSIENT     int yy;
    FAST          int x, xx1, xx2,
                      y, yy1, yy2;
    PERSIST const int gridlines[6] =
    { 32-2, 44-2, 56-2, 69-2, 81-2, 93-2 };

    // background

    yy1 = y1;
    yy2 = y2;
    xx1 = ponglt;
    xx2 = pongrt;
    if (!usemargins)
    {   xx1 -= PONGXMARGIN;
        xx2 += PONGXMARGIN;
        yy1 -= PONGYMARGIN;
        yy2 += PONGYMARGIN;
    }
    for (y = yy1; y <= yy2; y++)
    {   for (x = xx1; x <= xx2; x++)
        {   pongscreen[x][y] = 0;
            pong_drawbgpixel(x, y, bgc);
    }   }

    /* PAL    NTSC
        44     42
        45     43
        46     44
       101     80
       102     81
       129     99
       130    100
       131    111
       135    115
       136    116
       137    117
       160    138
       181    156
       182    157
       183    158
       273    231
       274    232
       275    233

    pongscreen[][] contents are:
        0 = empty
        1 = 1st player's goal to defend
        2 = 2nd player's goal to defend
        3 = vertical   wall (bounces horizontally)
        4 = horizontal wall (bounces vertically)

    top/bottom */
    switch (pong_variant)
    {
    case VARIANT_8550_RIFLE1:
    case VARIANT_8550_RIFLE2:
    case VARIANT_8600_TARGET1:
    case VARIANT_8600_TARGET2:
        // top/bottom (invisible)
        for (x = ponglt + 1; x < pongrt; x++) // or, x <= pongrt - 1
        {   pong_drawbouncepixel(    x     , region == REGION_PAL ?  44 :  42, GREY1, FALSE, DIR_HORIZ);
            pong_drawbouncepixel(    x     , region == REGION_PAL ?  45 :  43, GREY1, FALSE, DIR_HORIZ);
            pong_drawbouncepixel(    x     , region == REGION_PAL ? 274 : 232, GREY1, FALSE, DIR_HORIZ);
            pong_drawbouncepixel(    x     , region == REGION_PAL ? 275 : 233, GREY1, FALSE, DIR_HORIZ);
        }
    acase VARIANT_8550_TENNIS:
    case VARIANT_8550_SOCCER:
    case VARIANT_8550_HANDICAP:
    case VARIANT_8550_PRACTICE:
    case VARIANT_8550_SQUASH:
        // top/bottom (dotted)
        for (x = ponglt; x <= pongrt; x += 2)
        {   pong_drawbouncepixel(    x     , region == REGION_PAL ?  44 :  42, fgc,   TRUE,  DIR_HORIZ);
            pong_drawbouncepixel(    x     , region == REGION_PAL ?  45 :  43, fgc,   TRUE,  DIR_HORIZ);
            pong_drawbouncepixel(    x     , region == REGION_PAL ? 274 : 232, fgc,   TRUE,  DIR_HORIZ);
            pong_drawbouncepixel(    x     , region == REGION_PAL ? 275 : 233, fgc,   TRUE,  DIR_HORIZ);
            if (x < pongrt) // or, if (x + 1 <= pongrt)
            {   pong_drawbouncepixel(x + 1 , region == REGION_PAL ?  44 :  42, GREY1, FALSE, DIR_HORIZ);
                pong_drawbouncepixel(x + 1 , region == REGION_PAL ?  45 :  43, GREY1, FALSE, DIR_HORIZ);
                pong_drawbouncepixel(x + 1 , region == REGION_PAL ? 274 : 232, GREY1, FALSE, DIR_HORIZ);
                pong_drawbouncepixel(x + 1 , region == REGION_PAL ? 275 : 233, GREY1, FALSE, DIR_HORIZ);
        }   }
    acase VARIANT_8600_TENNIS:
    case VARIANT_8600_SOCCER:
    case VARIANT_8600_HOCKEY:
    case VARIANT_8600_SQUASH:
    case VARIANT_8600_PRACTICE:
    case VARIANT_8600_BASKETBALL:
    case VARIANT_8600_BBPRACTICE:
    case VARIANT_8600_GRIDBALL:
        // top/bottom (solid)
        for (x = ponglt; x <= pongrt; x++)
        {   pong_drawbouncepixel(    x     , region == REGION_PAL ?  44 :  42, fgc,   TRUE,  DIR_HORIZ);
            pong_drawbouncepixel(    x     , region == REGION_PAL ?  45 :  43, fgc,   TRUE,  DIR_HORIZ);
            pong_drawbouncepixel(    x     , region == REGION_PAL ? 274 : 232, fgc,   TRUE,  DIR_HORIZ);
            pong_drawbouncepixel(    x     , region == REGION_PAL ? 275 : 233, fgc,   TRUE,  DIR_HORIZ);
    }   }

    // left/right
    switch (pong_variant)
    {
    case VARIANT_8550_RIFLE1:
        // left/right (invisible)
        for (y = (region == REGION_PAL ? 44 : 42); y <= (region == REGION_PAL ? 275 : 233); y++)
        {   pong_drawbouncepixel(    ponglt, y    , GREY1, FALSE, DIR_VERT);
            pong_drawbouncepixel(    pongrt, y    , GREY1, FALSE, DIR_VERT);
        }
    acase VARIANT_8550_SOCCER:
    case VARIANT_8550_HANDICAP:
        // arms
        for (y = (region == REGION_PAL ? 46 : 44); y <= (region == REGION_PAL ? 96 : 82); y += 2)
        {   pong_drawbouncepixel(    ponglt, y    , fgc,   TRUE , DIR_VERT);
            pong_drawbouncepixel(    pongrt, y    , fgc,   TRUE , DIR_VERT);
            if (y < (region == REGION_PAL ? 96 : 82))
            {   pong_drawbouncepixel(ponglt, y + 1, GREY1, FALSE, DIR_VERT);
                pong_drawbouncepixel(pongrt, y + 1, GREY1, FALSE, DIR_VERT);
        }   }
        // soccer goal
        for (y = (region == REGION_PAL ? 97 : 83); y <= (region == REGION_PAL ? 222 : 192); y++)
        {   pong_drawscorepixel(     ponglt, y    , ltc,   0);
            pong_drawscorepixel(     pongrt, y    , rtc,   1);
        }
        // legs
        for (y = (region == REGION_PAL ? 223 : 193); y <= (region == REGION_PAL ? 273 : 231); y += 2)
        {   pong_drawbouncepixel(    ponglt, y    , fgc,   TRUE , DIR_VERT);
            pong_drawbouncepixel(    pongrt, y    , fgc,   TRUE , DIR_VERT);
            if (y < (region == REGION_PAL ? 273 : 231))
            {   pong_drawbouncepixel(ponglt, y + 1, GREY1, FALSE, DIR_VERT);
                pong_drawbouncepixel(pongrt, y + 1, GREY1, FALSE, DIR_VERT);
        }   }
    acase VARIANT_8600_SOCCER:
        // arms
        for (y = (region == REGION_PAL ? 46 : 44); y <= (region == REGION_PAL ? 96 : 82); y++)
        {   pong_drawbouncepixel(    ponglt, y    , fgc,   TRUE,  DIR_VERT);
            pong_drawbouncepixel(    pongrt, y    , fgc,   TRUE,  DIR_VERT);
        }
        // soccer goal
        for (y = (region == REGION_PAL ? 97 : 83); y <= (region == REGION_PAL ? 222 : 192); y++)
        {   pong_drawscorepixel(     ponglt, y    , ltc,   0);
            pong_drawscorepixel(     pongrt, y    , rtc,   1);
        }
        // legs
        for (y = (region == REGION_PAL ? 223 : 193); y <= (region == REGION_PAL ? 273 : 231); y++)
        {   pong_drawbouncepixel(    ponglt, y    , fgc,   TRUE,  DIR_VERT);
            pong_drawbouncepixel(    pongrt, y    , fgc,   TRUE,  DIR_VERT);
        }
    acase VARIANT_8550_SQUASH:
    case VARIANT_8550_PRACTICE:
        // left (stripy)/right (scoring)
        for (y = (region == REGION_PAL ? 46 : 44); y <= (region == REGION_PAL ? 273 : 231); y += 2)
        {   pong_drawbouncepixel(    ponglt, y    , fgc,   TRUE,  DIR_VERT);
            pong_drawscorepixel(     pongrt, y    , rtc,   1);
            if (y < (region == REGION_PAL ? 273 : 231))
            {   pong_drawbouncepixel(ponglt, y + 1, GREY1, FALSE, DIR_VERT);
                pong_drawscorepixel( pongrt, y + 1, rtc,   1);
        }   }
    acase VARIANT_8600_SQUASH:
    case VARIANT_8600_PRACTICE:
        // left (solid)/right (scoring)
        for (y = (region == REGION_PAL ? 46 : 44); y <= (region == REGION_PAL ? 273 : 231); y++)
        {   pong_drawbouncepixel(    ponglt, y    , fgc,   TRUE, DIR_VERT);
            pong_drawscorepixel(     pongrt, y    , rtc,   1);
        }
    acase VARIANT_8550_TENNIS:
    case VARIANT_8600_TENNIS:
    case VARIANT_8600_GRIDBALL:
        // left/right (scoring)
        for (y = (region == REGION_PAL ? 46 : 44); y <= (region == REGION_PAL ? 273 : 231); y++)
        {   pong_drawscorepixel(     ponglt, y    , ltc,   0);
            pong_drawscorepixel(     pongrt, y    , rtc,   1);
        }
    acase VARIANT_8600_HOCKEY:
    case VARIANT_8600_BASKETBALL:
    case VARIANT_8600_BBPRACTICE:
        // left/right (solid)
        for (y = (region == REGION_PAL ? 46 : 44); y <= (region == REGION_PAL ? 273 : 231); y++)
        {   pong_drawbouncepixel(    ponglt, y    , fgc,   TRUE, DIR_VERT);
            pong_drawbouncepixel(    pongrt, y    , fgc,   TRUE, DIR_VERT);
        }
    acase VARIANT_8550_RIFLE2:
    case VARIANT_8600_TARGET1:
    case VARIANT_8600_TARGET2:
        // right (scoring)
        for (y = (region == REGION_PAL ? 44 : 42); y <= (region == REGION_PAL ? 275 : 233); y++)
        {   pong_drawscorepixel(     pongrt, y    , rtc,   1);
    }   }

    if
    (   pong_variant == VARIANT_8550_TENNIS
     || pong_variant == VARIANT_8550_SOCCER
     || pong_variant == VARIANT_8550_HANDICAP
     || pong_variant == VARIANT_8600_TENNIS
     || pong_variant == VARIANT_8600_HOCKEY
     || pong_variant == VARIANT_8600_SOCCER
    )
    {   // net
        if (region == REGION_PAL)
        {   for (y = 46; y <= 273; y += 8)
            {   pong_drawpixel(63-2, y + 2, fgc);
                pong_drawpixel(63-2, y + 3, fgc);
                pong_drawpixel(63-2, y + 4, fgc);
                pong_drawpixel(63-2, y + 5, fgc);
        }   }
        else
        {   for (y = 44; y <= 231; y++)
            {   pong_drawpixel(63-2, y    , fgc);
    }   }   }

    switch (pong_variant)
    {
    case VARIANT_8600_HOCKEY:
        // hockey goal
        for (x = 32-2; x <= 35-2; x++)
        {   pong_drawbouncepixel(x     , region == REGION_PAL ? 135 : 115, fgc, TRUE, DIR_HORIZ);
            pong_drawbouncepixel(x     , region == REGION_PAL ? 136 : 116, fgc, TRUE, DIR_HORIZ);
            pong_drawbouncepixel(x     , region == REGION_PAL ? 182 : 157, fgc, TRUE, DIR_HORIZ);
            pong_drawbouncepixel(x     , region == REGION_PAL ? 183 : 158, fgc, TRUE, DIR_HORIZ);
            pong_drawbouncepixel(x + 59, region == REGION_PAL ? 135 : 115, fgc, TRUE, DIR_HORIZ);
            pong_drawbouncepixel(x + 59, region == REGION_PAL ? 136 : 116, fgc, TRUE, DIR_HORIZ);
            pong_drawbouncepixel(x + 59, region == REGION_PAL ? 182 : 157, fgc, TRUE, DIR_HORIZ);
            pong_drawbouncepixel(x + 59, region == REGION_PAL ? 183 : 158, fgc, TRUE, DIR_HORIZ);
        }
        for (y = (region == REGION_PAL ? 137 : 117); y <= (region == REGION_PAL ? 181 : 156); y++)
        {   pong_drawbouncepixel(32-2,   y, fgc, TRUE, DIR_VERT);
            pong_drawscorepixel( 33-2,   y, ltc, 0);
            pong_drawscorepixel( 93-2,   y, rtc, 1);
            pong_drawbouncepixel(94-2,   y, fgc, TRUE, DIR_VERT);
        }
    acase VARIANT_8600_BASKETBALL:
        // right basketball hoop
        for (x = 93-2; x <= 98-2; x++)
        {   pong_drawscorepixel( x     , region == REGION_PAL ? 101 : 80, rtc, 1);
            pong_drawscorepixel( x     , region == REGION_PAL ? 102 : 81, rtc, 1);
        }
        for (y = (region == REGION_PAL ? 101 : 80); y <= (region == REGION_PAL ? 131 : 101); y++)
        {   pong_drawbouncepixel(92-2  , y, fgc, TRUE, DIR_VERT);
        }
        for (x = 93-2; x <= 98-2; x++)
        {   pong_drawbouncepixel(x     , region == REGION_PAL ? 130 : 100, fgc, TRUE, DIR_HORIZ);
            pong_drawbouncepixel(x     , region == REGION_PAL ? 131 : 101, fgc, TRUE, DIR_HORIZ);
        }
    //lint -fallthrough
    case VARIANT_8600_BBPRACTICE: // not acase!
        // left basketball hoop
        for (x = 28-2; x <= 33-2; x++)
        {   pong_drawscorepixel( x     , region == REGION_PAL ? 101 : 80, ltc, 0);
            pong_drawscorepixel( x     , region == REGION_PAL ? 102 : 81, ltc, 0);
        }
        for (y = (region == REGION_PAL ? 101 : 80); y <= (region == REGION_PAL ? 131 : 101); y++)
        {   pong_drawbouncepixel(34-2  , y, fgc, TRUE, DIR_VERT);
        }
        for (x = 28-2; x <= 33-2; x++)
        {   pong_drawbouncepixel(x     , region == REGION_PAL ? 130 : 100, fgc, TRUE, DIR_HORIZ);
            pong_drawbouncepixel(x     , region == REGION_PAL ? 131 : 101, fgc, TRUE, DIR_HORIZ);
        }
    acase VARIANT_8600_GRIDBALL:
        gridy[0] += (ay[0] - 128) / 12;
        if   (gridy[0] <   0)
        {   gridy[0] += (region == REGION_PAL ? 227 : 188);
        } elif (gridy[0] > (region == REGION_PAL ? 227 : 188))
        {   gridy[0] -= (region == REGION_PAL ? 227 : 188);
        }
        gridy[1] += (ay[1] - 128) / 12;
        if   (gridy[1] <   0)
        {   gridy[1] += (region == REGION_PAL ? 227 : 188);
        } elif (gridy[1] > (region == REGION_PAL ? 227 : 188))
        {   gridy[1] -= (region == REGION_PAL ? 227 : 188);
        }

        for (x = 0; x < 6; x++)
        {   if (region == REGION_NTSC)
            {   for (y = 0; y < 188; y++)
                {   if
                    (   (batsize[x % 2] == 28 && gridball_big[y] == (TEXT) '#')
                     || (batsize[x % 2] == 14 && gridball_sml[y] == '#')
                    )
                    {   yy = 44 + y + gridy[x % 2];
                        if (yy > 231) yy -= 188;
                        pong_drawpixel(gridlines[x], yy, (x % 2) ? rtc : ltc);
                        pongscreen[gridlines[x]][yy] = 3;
            }   }   }
            else
            {   // assert(region == REGION_PAL);
                for (y = 0; y < 228; y++)
                {   if
                    (   (batsize[x % 2] == 28 && gridball_big[y] == '#')
                     || (batsize[x % 2] == 14 && gridball_sml[y] == '#')
                    )
                    {   yy = 46 + y + gridy[x % 2];
                        if (yy > 273) yy -= 228;
                        pong_drawpixel(gridlines[x], yy, (x % 2) ? rtc : ltc);
                        pongscreen[gridlines[x]][yy] = 3;
    }   }   }   }   }

    // score
    if (memmap == MEMMAP_8600)
    {   if (score[0] / 10 >= 1) { pong_drawdigit(0, 48-2, score[0] / 10); }
                                  pong_drawdigit(0, 52-2, score[0] % 10);
        if (score[1] / 10 >= 1) { pong_drawdigit(1, 69-2, score[1] / 10); }
                                  pong_drawdigit(1, 73-2, score[1] % 10);
    } elif (pong_machine == PONGMACHINE_1976)
    {   if (score[0] / 10 >= 1) { pong_drawdigit(0, 44-2, score[0] / 10); }
                                  pong_drawdigit(0, 52-2, score[0] % 10);
        if (score[1] / 10 >= 1) { pong_drawdigit(1, 66-2, score[1] / 10); }
                                  pong_drawdigit(1, 74-2, score[1] % 10);
    } else
    {   if (score[0] / 10 >= 1) { pong_drawdigit(0, 46-2, score[0] / 10); }
                                  pong_drawdigit(0, 54-2, score[0] % 10);
        if (score[1] / 10 >= 1) { pong_drawdigit(1, 64-2, score[1] / 10); }
                                  pong_drawdigit(1, 72-2, score[1] % 10);
}   }

EXPORT void pong_setmemmap(void)
{   // assert(memmap == MEMMAP_8550 || memmap == MEMMAP_8600);

#ifdef WIN32
    switch (memmap)
    {
    case  MEMMAP_8550: hostmachines[machine].controls = IDD_CONTROLS_8550;
    acase MEMMAP_8600: hostmachines[machine].controls = IDD_CONTROLS_8600;
    }
#endif
    keynames_from_overlay();

    key1                  = 2; // fire rifle
    analog                = TRUE;
    sensitivity[0]        =
    sensitivity[1]        = SENSITIVITY_VHIGH;
    pong_start            =
    pong_a                =
    pong_b                =
    pong_reset            = FALSE;

    ballinplay            =
    gameover              = FALSE;

    if (memmap == MEMMAP_8550)
    {   key2              =
        key3              = 0; // down
        key4              = 8; // up
        if (pong_variant < 0 || pong_variant > 6)
        {   pong_variant = VARIANT_8550_PRACTICE;
        }
        if (batvalue >= 1)
        {   batvalue = 1;
    }   }
    else
    {   // assert(memmap == MEMMAP_8600);
        key2              =
        key3              =
        key4              = 2; // fire rifle
        if (pong_variant < 7 || pong_variant > 16)
        {   pong_variant = VARIANT_8600_PRACTICE;
        }
        if (pongspeed == PONGSPEED_RANDOM)
        {   pongspeed = PONGSPEED_SLOW;
    }   }

    updatebatsize();
    // pong_updatedips(); causes a crash because the old DIPs subwindow
    // (for eg. the other Pong chip) is still open at this point
}

MODULE void stopgame(void)
{   ltballx    = 63-2;
    bally      = region == REGION_PAL ? 160 : 138;

    balldeltax =
    balldeltay = 0;

    ballinplay = FALSE;
    gameover   = TRUE;
}

MODULE void checkrifle(int player)
{   int x, y;

    if (player == 0)
    {   x = rifle1x;
        y = rifle1y;
    } else
    {   x = rifle2x;
        y = rifle2y;
    }

    if (playerfire[player])
    {   if (freshfire[player])
        {   freshfire[player] = FALSE;
            if (shot[player] && pong_variant != VARIANT_8550_RIFLE1)
            {   // zprintf(TEXTPEN_VERBOSE, "Already shot!\n");
                // we could make a noise, the real machine probably doesn't though
            } else
            {   if (player == 0 && (pong_variant == VARIANT_8550_RIFLE1 || pong_variant == VARIANT_8550_RIFLE2))
                {   score[1]++;
                }
                if
                (   x >= ltballx - 1
                 && x <= rtballx + 1
                 && y >= bally - ballheight
                 && y <= bally + ballheight
                )
                {
#ifdef VERBOSE
                    zprintf(TEXTPEN_VERBOSE, "Hit");
#endif
                    makesound(TONE_BAT);
                    if (pong_variant == VARIANT_8600_TARGET1 || player == 1)
                    {   score[1]++; // hits counter
                    } else
                    {   score[0]++; // p1 score in 2-player target
                    }
                    blanktarget = REGION_PAL ? 50 : 60;
                    if (player == 0)
                    {   p1rumble = RIFLEHITRUMBLE;
                    } else
                    {   p2rumble = RIFLEHITRUMBLE;
                    }
                    shot[player] = 2;
                } else
                {
#ifdef VERBOSE
                    zprintf(TEXTPEN_VERBOSE, "Missed");
#endif
                    makesound(TONE_SCORE);
                    shot[player] = 1;
                    if (player == 0)
                    {   p1rumble = RIFLEMISSRUMBLE;
                    } else
                    {   p2rumble = RIFLEMISSRUMBLE;
                }   }
#ifdef VERBOSE
                zprintf(TEXTPEN_VERBOSE, "! Bullet at %d,%d vs. target box %d..%d,%d..%d.\n", x, y, ltballx - 1, rtballx + 1, bally - ballheight, bally + ballheight);
#endif
    }   }   }
    else
    {   freshfire[player] = TRUE;
}   }

EXPORT void pong_serialize_cos(void)
{   serialize_byte_int(&score[0]       );
    serialize_byte_int(&score[1]       );
    serialize_byte(&playerfire[0]      );
    serialize_byte(&playerfire[1]      );
    serialize_byte_int(&pong_start     );
    serialize_byte_int(&pong_a         );
    serialize_byte_int(&pong_b         );
    serialize_byte_int(&pong_reset     );
    serialize_byte_int((int*) &pong_machine);
    serialize_byte_int(&pong_variant   );
    serialize_byte_int(&angles         );
    serialize_byte_int(&pongspeed      );
    serialize_byte_int(&serving        );
    serialize_byte_int(&lockhoriz      );
    serialize_byte_int(&playerid       );
    serialize_byte_int(&batvalue       );
    serialize_byte_int((int*) &region  );
    serialize_byte_int(&ballinplay     );
    serialize_long((ULONG*) &ltballx   );
    serialize_long((ULONG*) &bally     );
    serialize_long((ULONG*) &balldeltax);
    serialize_long((ULONG*) &balldeltay);
    serialize_long((ULONG*) &hits      );
    serialize_long((ULONG*) &oldbatx[0]);
    serialize_long((ULONG*) &oldbatx[1]);
    serialize_long((ULONG*) &oldbatx[2]);
    serialize_long((ULONG*) &oldbatx[3]);
    serialize_long((ULONG*) &oldbatx[4]);
    serialize_byte((UBYTE*) &freshfire[0]);
    serialize_byte((UBYTE*) &freshfire[1]);
    serialize_byte(&shot[0]);
    serialize_byte(&shot[1]);
    serialize_long((ULONG*) &gridy[0]);
    serialize_long((ULONG*) &gridy[1]);
    serialize_byte_int(&server);
    serialize_byte((UBYTE*) &served[0]);
    serialize_byte((UBYTE*) &served[1]);
    serialize_byte((UBYTE*) &gameover);
    serialize_byte_int((int*) &region);
    serialize_word_int(&gx[0]);
    serialize_word_int(&gy[0]);
    serialize_word_int(&gx[1]);
    serialize_word_int(&gy[1]);
    serialize_byte_int(&ay[2]);
    serialize_byte_int(&ay[3]);

    if (serializemode == SERIALIZE_READ)
    {   updatebatsize();
        // pong_updatedips() gets done for us by engine_load()
        dogamename();
        calc_margins(); // because region might have changed
        draw_margins();
}   }

MODULE UBYTE predict(int xmin, int xmax, int ymin, int ymax)
{   /* balldeltax is always -2,-1,1,2
       balldeltay is always -5,-3,-2,-1,0,1,2,3,5 */

    int   temp,
          x, y;
    UBYTE t;

    if (xmin > xmax)
    {   temp = xmin;
        xmin = xmax;
        xmax = temp;
    }
    if (ymin > ymax)
    {   temp = ymin;
        ymin = ymax;
        ymax = temp;
    }

    for (x = xmin; x <= xmax; x++)
    {   for (y = ymin; y <= ymax; y++)
        {   if (pongscreen[x][y] == 1 || pongscreen[x][y] == 2)
            {   return pongscreen[x][y];
    }   }   }

    t = 0;
    for (x = xmin; x <= xmax; x++)
    {   for (y = ymin; y <= ymax; y++)
        {   if   (pongscreen[x][y] == 3) t |= 4;
            elif (pongscreen[x][y] == 4) t |= 8;
    }   }

    return t;
}

MODULE void position_bats(void)
{   int i,
        teamsize;

    for (i = 0; i < 4; i++)
    {   teamsize = batsize[i & 1];
        if (region == REGION_PAL)
        {   topbaty[i] = 160 + ay[i] - 128 - (teamsize / 2);
            botbaty[i] = topbaty[i] + teamsize - 1;
            if (topbaty[i] < 46)
            {   topbaty[i] = 46;
                botbaty[i] = topbaty[i] + teamsize - 1;
            } elif (botbaty[i] > 273)
            {   botbaty[i] = 273;
                topbaty[i] = botbaty[i] - teamsize + 1;
        }   }
        else
        {   // assert(region == REGION_NTSC);
            topbaty[i] = 138 + ay[i] - 128 - (teamsize / 2);
            botbaty[i] = topbaty[i] + teamsize - 1;
            if (topbaty[i] < 44)
            {   topbaty[i] = 44;
                botbaty[i] = topbaty[i] + teamsize - 1;
            } elif (botbaty[i] > 231)
            {   botbaty[i] = 231;
                topbaty[i] = botbaty[i] - teamsize + 1;
    }   }   }

    for (i = 0; i < variantinfo[pong_variant].bats; i++)
    {   if (lockhoriz || pong_machine != PONGMACHINE_8550)
        {   if (memmap == MEMMAP_8550 && pong_machine == PONGMACHINE_1976)
            {   batx[i] = variantinfo[pong_variant].batinfo[i].x_76;
            } else
            {   batx[i] = variantinfo[pong_variant].batinfo[i].x_77;
        }   }
        else
        {   batx[i] = variantinfo[pong_variant].batinfo[i].joycentre + ((ax[variantinfo[pong_variant].batinfo[i].player] - 128) / variantinfo[pong_variant].batinfo[i].joysensitivity);
            if (batx[i] < variantinfo[pong_variant].batinfo[i].xmin)
            {   batx[i] = variantinfo[pong_variant].batinfo[i].xmin;
            } elif (batx[i] > variantinfo[pong_variant].batinfo[i].xmax)
            {   batx[i] = variantinfo[pong_variant].batinfo[i].xmax;
            }
            if (batx[i] < oldbatx[i])
            {   batx[i] = oldbatx[i] - 1;
            } elif (batx[i] > oldbatx[i])
            {   batx[i] = oldbatx[i] + 1;
}   }   }   }

MODULE void collide_bats(void)
{   int i;

    newltballx = ltballx + balldeltax;
    newbally   =   bally + balldeltay;

    if (ballinplay && collisions)
    {   // bat collisions
        for (i = 0; i < variantinfo[pong_variant].bats; i++)
        {   if (variantinfo[pong_variant].batinfo[i].player == 0)
            {   switch (variantinfo[pong_variant].batinfo[i].facing)
                {
                case FACE_LT:
                    if (pong_variant != VARIANT_8550_SQUASH || whoseturn == 0)
                    {   if (lt_hit_lt(batx[i], oldbatx[i]))
                        {   if (pong_variant == VARIANT_8600_SQUASH && whoseturn == 1)
                            {   if (server == 1) score[1]++; else server = 1;
                                // make a sound?
                                if (score[0] < 15 && score[1] < 15)
                                {   newball1();
                                } else
                                {   stopgame();
                            }   }
                            else
                            {   whoseturn = 1; // for Squash
                                if
                                (   pong_variant == VARIANT_8550_PRACTICE
                                 || pong_variant == VARIANT_8600_PRACTICE
                                 || pong_variant == VARIANT_8600_BBPRACTICE
                                )
                                {   score[1]++;
                                    if (score[1] >= 15)
                                    {   stopgame();
                    }   }   }   }   }
                acase FACE_LF:
                case FACE_RF:
                    lt_hit_forwards(batx[i], oldbatx[i]);
                acase FACE_RT:
                    lt_hit_rt(batx[i], oldbatx[i]);
                acase FACE_EI:
                    lt_hit_either(batx[i], oldbatx[i]);
            }   }
            else
            {   switch (variantinfo[pong_variant].batinfo[i].facing)
                {
                case FACE_LT:
                    if (pong_variant != VARIANT_8550_SQUASH || whoseturn == 1)
                    {   if (rt_hit_lt(batx[i], oldbatx[i]))
                        {   if (pong_variant == VARIANT_8600_SQUASH && whoseturn == 0)
                            {   if (server == 0) score[0]++; else server = 0;
                                // make a sound?
                                if (score[0] < 15 && score[1] < 15)
                                {   newball1();
                                } else
                                {   stopgame();
                            }   }
                            else
                            {   whoseturn = 0; // for Squash
                    }   }   }
                acase FACE_RT:
                    rt_hit_rt(batx[i], oldbatx[i]);
                acase FACE_LF:
                case FACE_RF:
                    rt_hit_forwards(batx[i], oldbatx[i]);
                acase FACE_EI:
                    rt_hit_either(batx[i], oldbatx[i]);
}   }   }   }   }

EXPORT void fixx(void)
{   if
    (   nowspeed         == PONGSPEED_FAST
     && (   memmap       == MEMMAP_8550
         || pong_variant == VARIANT_8600_GRIDBALL
         || pong_variant == VARIANT_8600_TARGET1
         || pong_variant == VARIANT_8600_TARGET2
         || hits         >= 7 - 1
    )   )
    {   if (balldeltax == -1)
        {   balldeltax = -2;
        } elif (balldeltax == 1)
        {   balldeltax = 2;
    }   }
    else
    {   if (balldeltax == -2)
        {   balldeltax = -1;
        } elif (balldeltax == 2)
        {   balldeltax = 1;
}   }   }
EXPORT void fixy(void)
{   if
    (   nowspeed         == PONGSPEED_FAST
     && (   memmap       == MEMMAP_8550
         || pong_variant == VARIANT_8600_GRIDBALL
         || pong_variant == VARIANT_8600_TARGET1
         || pong_variant == VARIANT_8600_TARGET2
         || hits         >= 7 - 1
    )   )
    {   switch (angle)
        {
        case  0: balldeltay = -5;
        acase 1: balldeltay = -2;
        acase 2: balldeltay =  0;
        acase 3: balldeltay =  2;
        acase 4: balldeltay =  5;
    }   }
    else
    {   switch (angle)
        {
        case  0: balldeltay = -3;
        acase 1: balldeltay = -1;
        acase 2: balldeltay =  0;
        acase 3: balldeltay =  1;
        acase 4: balldeltay =  3;
}   }   }
MODULE void fixangle(void)
{   switch (balldeltay)
    {
    case  -5: case -3: angle = 0;
    acase -2: case -1: angle = 1;
    acase  0:          angle = 2;
    acase  1: case  2: angle = 3;
    acase  3: case  5: angle = 4;
}   }

EXPORT void updatebatsize(void)
{   switch (batvalue)
    {
    case 0:
        batsize[0] =
        batsize[1] = 14;
    acase 1:
        batsize[0] =
        batsize[1] = 28;
    acase 2:
        batsize[0] = 14;
        batsize[1] = 28;
    acase 3:
        batsize[0] = 28;
        batsize[1] = 14;
}   }

MODULE void calcspeed(void)
{   if (memmap == MEMMAP_8550 && pongspeed == PONGSPEED_RANDOM)
    {   nowspeed = getrandom(2);
    } else
    {   nowspeed = pongspeed;
}   }

EXPORT void pong_updatedips(void)
{   if (machine != PONG || !SubWindowPtr[SUBWINDOW_DIPS])
    {   return;
    }

    if (memmap == MEMMAP_8550)
    {   cb_enable(SUBWINDOW_DIPS, IDC_PLAYERID, (colourset == GREYSCALE) ? TRUE : FALSE);
        cb_set(SUBWINDOW_DIPS, IDC_PLAYERID, playerid ? TRUE : FALSE);

        ra_set(SUBWINDOW_DIPS, IDC_PONGMACHINE_1976, IDC_PONGMACHINE_8550, pong_machine);

        ra_set(SUBWINDOW_DIPS, IDC_SERVING_AUTO    , IDC_SERVING_MANUAL  , serving);

        ra_set(SUBWINDOW_DIPS, IDC_ANGLES_2        , IDC_ANGLES_RANDOM   , angles);

        ra_set(SUBWINDOW_DIPS, IDC_PLAYERS_2       , IDC_PLAYERS_4       , players);

        ra_set(SUBWINDOW_DIPS, IDC_8550_TENNIS     , IDC_8550_RIFLE2     , pong_variant);

        ra_set(SUBWINDOW_DIPS, IDC_SPEED_SLOW      , IDC_SPEED_RANDOM    , pongspeed);

        cb_enable(SUBWINDOW_DIPS, IDC_LOCKHORIZ, (pong_machine == PONGMACHINE_8550) ? TRUE : FALSE);
    } else
    {   ra_set(SUBWINDOW_DIPS, IDC_8600_TENNIS     , IDC_8600_TARGET2    , pong_variant - 7);

        ra_set(SUBWINDOW_DIPS, IDC_SPEED_SLOW      , IDC_SPEED_FAST      , pongspeed);
    }

    cb_set(SUBWINDOW_DIPS, IDC_LOCKHORIZ, lockhoriz ? TRUE : FALSE);

    ra_set(SUBWINDOW_DIPS, IDC_BATS_SHORT, IDC_BATS_TALLSHORT, batvalue);
}
