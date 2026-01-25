// INCLUDES---------------------------------------------------------------

#ifdef AMIGA
    #include "amiga.h"
#endif
#ifdef WIN32
    #include "ibm.h"
    #define EXEC_TYPES_H
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "aa.h"
#define CATCOMP_NUMBERS
#define CATCOMP_BLOCK
#include "aa_strings.h"
#ifdef AMIGA
    #include <proto/locale.h>  // GetCatalogStr()
#endif

#include "tr.h"

// DEFINES----------------------------------------------------------------

#define TRKEY_CLASS               1
#define TRKEY_GAME                2
#define TRKEY_SPEED               3
#define TRKEY_ACCURACY            4
// these must all be unused ASCII values

// MODULE ARRAYS----------------------------------------------------------

// (See also tr.h)

MODULE const int tr_speeds[5] = { 75, 30, 20, 10, 5 };

// EXPORTED VARIABLES-----------------------------------------------------

EXPORT TEXT    tr_digits[8 + 1],
               tr_gameletter;
EXPORT FLAG    tr_underline[8];
EXPORT int     since,
               stage,
               tr_accuracy,
               tr_class,
               tr_errors,
               tr_level,
               tr_remaining,
               tr_textcursor,
               tr_wpm;

// IMPORTED VARIABLES-----------------------------------------------------

IMPORT UBYTE   OutputBuffer[18],
               vdu_fgc,
               vdu_bgc;
IMPORT UWORD   console[4];
IMPORT ULONG   cycles_2650,
               frames;
IMPORT int     console_start,
               console_a,
               console_b,
               console_reset,
               inverse,
               machine,
               offset,
               ponghertz,
               queuepos,
               recmode,
               sgc,
               whichkeyrect;
IMPORT TEXT    gtempstring[256 + 1],
               thequeue[QUEUESIZE];
IMPORT FLAG    capslock,
               inframe;
IMPORT UBYTE*  IOBuffer;
IMPORT FILE*   MacroHandle;
#ifdef WIN32
    IMPORT ULONG          pencolours[COLOURSETS][PENS];
    IMPORT HWND           RichTextGadget;
#endif
#ifdef AMIGA
    IMPORT UBYTE          bytepens[PENS];
    IMPORT struct Window* SubWindowPtr[SUBWINDOWS];
#endif

// MODULE VARIABLES-------------------------------------------------------

MODULE int   tr_game,
             tr_lives,
             tr_score,
             tr_screencursor,
             tr_usergame;
MODULE FLAG  tr_arrow[4];

// MODULE FUNCTIONS-------------------------------------------------------

MODULE void class_getnewletter(void);
MODULE void game_getnewletter(void);
MODULE void tr_alternate(void);
MODULE void tr_changepixel(int x, int y, int colour);
MODULE void tr_changebgpixel(int x, int y, int colour);
MODULE FLAG checkword(int screenstart, int reallength, int ideallength);

// CODE-------------------------------------------------------------------

EXPORT void tr_simulate(void)
{   FLAG  started;
    UBYTE inchar,
          t;
    int   i,
          ideallength,
          words,
          secs,
          x, xx, y;

    inframe = TRUE;
    ponghertz = 0;

    if (recmode == RECMODE_PLAY)
    {   inchar = IOBuffer[offset++];
    } else
    {   // assert(recmode == RECMODE_NORMAL || recmode == RECMODE_RECORD);

        if (KeyDown(console[0])) console_start = 2;
        if (KeyDown(console[1])) console_a     = 2;
        if (KeyDown(console[2])) console_b     = 2;
        if (KeyDown(console[3])) console_reset = 2;
        domouse();

        if   (console_start    ) inchar        = TRKEY_CLASS;
        elif (console_a        ) inchar        = TRKEY_GAME;
        elif (console_b        ) inchar        = TRKEY_SPEED;
        elif (console_reset    ) inchar        = TRKEY_ACCURACY;
        elif (queuepos)
        {   inchar = (UBYTE) thequeue[0];
            if (inchar > 0x7A || (inchar >= 0x20 && tr_pos[inchar - 0x20] == 0) || inchar == CR)
            {   inchar = EOS;
            }
            queuepos--;
            if (queuepos)
            {   for (i = 0; i < queuepos; i++)
                {   thequeue[i] = thequeue[i + 1];
        }   }   }
        else
        {   inchar = EOS;
    }   }

    if (recmode == RECMODE_RECORD)
    {   // assert(MacroHandle);

        OutputBuffer[0] = inchar;
        DISCARD fwrite(OutputBuffer, 1, 1, MacroHandle);
        // should really check return code of fwrite()
        // 1 byte per frame * 50 frames per second = 50 bytes/sec.
    }

    if (stage != STAGE_BOOT)
    {   if   (inchar == TRKEY_CLASS)
        {   stage = STAGE_CLASS_SELECTLESSON;
            strcpy((char*) tr_digits, " 1 - 9  ");
            tr_arrow[0] = TRUE;
            tr_arrow[1] = FALSE;
            for (i = 0; i < 8; i++)
            {   tr_underline[i] = FALSE;
        }   }
        elif (inchar == TRKEY_GAME)
        {   stage = STAGE_GAME_SELECT;
            strcpy((char*) tr_digits, " 1 - 2  ");
            tr_arrow[0] = FALSE;
            tr_arrow[1] = TRUE;
            for (i = 0; i < 8; i++)
            {   tr_underline[i] = FALSE;
    }   }   }

    switch (stage)
    {
    case STAGE_BOOT:
        tr_errors = 0;
        tr_remaining++;
        if (tr_remaining > 40)
        {   stage = STAGE_MENU;
        } else
        {   if   (tr_remaining ==  5) { ponghertz = LOWBEEP; strcpy((char*) tr_digits, "*       "); }
            elif (tr_remaining == 10) { ponghertz = LOWBEEP; strcpy((char*) tr_digits, "**      "); }
            elif (tr_remaining == 15) { ponghertz = LOWBEEP; strcpy((char*) tr_digits, "***     "); }
            elif (tr_remaining == 20) { ponghertz = LOWBEEP; strcpy((char*) tr_digits, "****    "); }
            elif (tr_remaining == 25) { ponghertz = LOWBEEP; strcpy((char*) tr_digits, "*****   "); }
            elif (tr_remaining == 30) { ponghertz = LOWBEEP; strcpy((char*) tr_digits, "******  "); }
            elif (tr_remaining == 35) { ponghertz = LOWBEEP; strcpy((char*) tr_digits, "******* "); }
            elif (tr_remaining == 40) { ponghertz = LOWBEEP; strcpy((char*) tr_digits, "********"); }
        }
    acase STAGE_MENU:
        tr_errors = 0;
        tr_alternate();
    acase STAGE_CLASS_SELECTLESSON:
        if (inchar >= '1' && inchar <= '9')
        {   sprintf((char*) tr_digits, "LESSON %c", inchar);
            tr_class = inchar - '0';
            stage = STAGE_CLASS_WAITLESSON;
            tr_remaining = 50;
        }
    acase STAGE_CLASS_WAITLESSON:
        if (--tr_remaining == 0)
        {   if (tr_class <= 5)
            {   stage = STAGE_CLASS1_PLAY;

                for (i = 0; i < 8; i++)
                {   tr_underline[i] = TRUE;
                }

                tr_remaining = 50 * 55; // 50 frames per second * 55 seconds = 2750 frames
                tr_score = 0;
                strcpy((char*) tr_digits, "        ");
                class_getnewletter();
            } else
            {   strcpy((char*) tr_digits, " 1 - 4  ");
                stage = STAGE_CLASS6_SELECTLEVEL;
        }   }
    acase STAGE_CLASS1_MUSIC:
        tr_remaining++;
        if (tr_remaining == 10)
        {   stage = STAGE_MENU;
        } else
        {   ponghertz = HIGHBEEP;
        }
    acase STAGE_CLASS1_PLAY:
        if (--tr_remaining == 0)
        {   for (i = 0; i < 8; i++)
            {   tr_underline[i] = FALSE;
            }
            sprintf((char*) tr_digits, "%8d", tr_score);
            hiscore_register(HS_CLASS1_POS + tr_class - 1, tr_score);
            stage = STAGE_CLASS1_MUSIC;
            tr_remaining = 0;
        } else
        {   if (inchar == tr_gameletter)
            {   tr_digits[tr_pos[tr_gameletter - 0x20] - 1] = ' ';
                tr_score++;
                class_getnewletter();
            } elif (inchar != EOS)
            {   ponghertz = HIGHBEEP;
                tr_errors++;
        }   }
    acase STAGE_GAME_SELECT:
        if (inchar >= '1' && inchar <= '2')
        {   strcpy((char*) tr_digits, "        ");
            tr_game = tr_usergame = inchar - '0';
            stage = STAGE_GAME_MUSIC;
            tr_remaining = 0;
        }
    acase STAGE_GAME_MUSIC:
        tr_remaining++;
        if (tr_remaining > 50)
        {   stage = STAGE_GAME_WAIT;
            tr_remaining = 0;
            tr_lives = 3;
            tr_score = 0;
            sprintf((char*) tr_digits, "%d %6d", tr_lives, tr_score);
            hiscore_register(HS_GAME1_POS + tr_usergame - 1, tr_score);
        } else
        {   if   (tr_remaining ==  5) { ponghertz = LOWBEEP; }
            elif (tr_remaining == 10) { ponghertz = LOWBEEP; }
            elif (tr_remaining == 15) { ponghertz = LOWBEEP; }
            elif (tr_remaining == 20) { ponghertz = LOWBEEP; }
            elif (tr_remaining == 25) { ponghertz = LOWBEEP; }
        }
    acase STAGE_GAME_WAIT:
        tr_remaining++;
        if (tr_remaining == 75)
        {   stage = STAGE_GAME_START;
            tr_remaining = 0;
            strcpy((char*) tr_digits, "        ");
        } else
        {   if   (tr_remaining == 10) { ponghertz = LOWBEEP; }
            elif (tr_remaining == 15) { ponghertz = LOWBEEP; }
            elif (tr_remaining == 20) { ponghertz = LOWBEEP; }
        }
    acase STAGE_GAME_START:
        tr_remaining++;
        if (tr_remaining == 10)
        {   stage = STAGE_GAME_PLAY;
            tr_remaining = 0;
            strcpy((char*) tr_digits, "        ");
            game_getnewletter();
        } else
        {   ponghertz = HIGHBEEP;
        }
    acase STAGE_GAME_PLAY:
        if (tr_gameletter == EOS && tr_remaining >= 30 * 50)
        {   tr_score += 100;
            if (tr_lives >= 0 && tr_lives <= 9 && tr_score >= 0 && tr_score <= 999999) // should always be true
            {   tr_digits[0] = '0' + tr_lives; // this is all to avoid a spurious GCC warning
                tr_digits[1] = ' ';
                started = FALSE;
                if (tr_score / 100000)
                {   started = TRUE;
                    tr_digits[2] = '0' +  (tr_score / 100000);
                } else
                {   tr_digits[2] = ' ';
                }
                if (started || ((tr_score % 100000) /  10000))
                {   started = TRUE;
                    tr_digits[3] = '0' + ((tr_score %  100000) / 10000);
                } else
                {   tr_digits[3] = ' ';
                }
                if (started || ((tr_score %  10000) /   1000))
                {   started = TRUE;
                    tr_digits[4] = '0' + ((tr_score %   10000) /  1000);
                } else
                {   tr_digits[4] = ' ';
                }
                if (started || ((tr_score %   1000) /    100))
                {   started = TRUE;
                    tr_digits[5] = '0' + ((tr_score %    1000) /   100);
                } else
                {   tr_digits[5] = ' ';
                }
                if (started || ((tr_score %    100) /     10))
                {   tr_digits[6] = '0' + ((tr_score %     100) /    10);
                } else
                {   tr_digits[6] = ' ';
                }
                tr_digits[    7] = '0' +  (tr_score %      10);
                tr_digits[    8] = EOS;
                hiscore_register(HS_GAME1_POS + tr_usergame - 1, tr_score);
            } else
            {   sprintf((char*) tr_digits, "# ######");
            }
            tr_game++;
            stage = STAGE_GAME_START;
            tr_remaining = tr_errors = 0;
        } else
        {   if (inchar == tr_gameletter || inchar == tolower(tr_gameletter))
            {   for (i = 0; i < 8; i++)
                {   if (tr_digits[i] == tr_gameletter)
                    {   tr_digits[i] = ' ';
                        tr_score++;
                        break;
                }   }
                tr_gameletter = EOS;
                for (i = 0; i < 8; i++)
                {   if (tr_digits[i] != ' ')
                    {   tr_gameletter = tr_digits[i];
                        break;
            }   }   }
            elif (inchar != EOS)
            {   ponghertz = HIGHBEEP;
                tr_errors++;
                if (tr_score >= 1)
                {   tr_score--;
            }   }

            tr_remaining++;
            if (tr_remaining % tr_speeds[tr_game - 1] == 0)
            {   game_getnewletter();
        }   }
    acase STAGE_CLASS6_SELECTLEVEL:
        if (inchar >= '1' && inchar <= '4')
        {   sprintf((char*) tr_digits, "LEVEL  %c", inchar);
            tr_level = inchar - '0';
            stage = STAGE_CLASS6_WAITLEVEL;
            tr_remaining = 50;
        }
    acase STAGE_CLASS6_WAITLEVEL:
        if (--tr_remaining == 0)
        {   tr_underline[0] = TRUE;
            strcpy((char*) tr_digits, "        ");
            tr_textcursor = tr_screencursor = tr_errors = since = 0;
            stage = (tr_class >= 8) ? STAGE_CLASS8_PLAY : STAGE_CLASS6_PLAY;
            cls();
            zprintf(TEXTPEN_CLIOUTPUT, "%s\n", classes[tr_class - 6][tr_level - 1]);
#ifdef WIN32
            DISCARD SendMessage
            (   RichTextGadget,
                WM_VSCROLL,
                SB_TOP,
                0
            );
#endif
            ponghertz = HIGHBEEP;
            // tr_remaining = 0;
        }
    acase STAGE_CLASS6_PLAY:
        tr_remaining++;
        if (inchar != EOS)
        {   if (inchar >= 0x20)
            {   tr_digits[tr_screencursor] = inchar;
            }
            if (inchar == classes[tr_class - 6][tr_level - 1][tr_textcursor])
            {   tr_textcursor++;
                tr_underline[tr_screencursor] = FALSE;
                if (inchar == LF)
                {   tr_screencursor = 0;
                    strcpy((char*) tr_digits, "        ");
                } else
                {   if (tr_screencursor == 7)
                    {   tr_digits[0] = tr_digits[1];
                        tr_digits[1] = tr_digits[2];
                        tr_digits[2] = tr_digits[3];
                        tr_digits[3] = tr_digits[4];
                        tr_digits[4] = tr_digits[5];
                        tr_digits[5] = tr_digits[6];
                        tr_digits[6] = tr_digits[7];
                        tr_digits[7] = ' ';
                    } else
                    {   tr_screencursor++;
                }   }
                tr_underline[tr_screencursor] = TRUE;
                if (classes[tr_class - 6][tr_level - 1][tr_textcursor] == EOS)
                {   for (i = 0; i < 8; i++)
                    {   tr_underline[i] = FALSE;
                    }

                    secs = tr_remaining / 50;
                    if (tr_remaining % 50)
                    {   secs++;
                    }
                    words = tr_textcursor / (5 + 1);
                    if (tr_textcursor % (5 + 1))
                    {   words++;
                    }
                    tr_wpm = 60 * words / secs;

                    tr_accuracy = 100 - (100 * tr_errors / tr_textcursor);

                    sprintf((char*) tr_digits, "%4d WPM", tr_wpm);
                    hiscore_register(HS_CLASS6LEVEL1_POS + ((tr_class - 6) * 4) + tr_level - 1, tr_wpm);
                    stage = STAGE_CLASS68_WAITRESULTS;
            }   }
            else
            {   tr_errors++;
                ponghertz = HIGHBEEP;
        }   }
    acase STAGE_CLASS8_PLAY:
        tr_remaining++;
        if (inchar == HT && tr_screencursor == 7)
        {   inchar = ' ';
        }
        switch (inchar)
        {
        case EOS:
        break;
        case BS: // ie. left
            if (tr_screencursor == 0 || since == 0)
            {   ponghertz = HIGHBEEP; // we don't consider it an error. Presumably the real machine doesn't either.
            } else
            {   tr_underline[tr_screencursor] = FALSE;
                tr_screencursor--;
                tr_underline[tr_screencursor] = TRUE;
                since--;
            }
        acase HT: // ie. right
            tr_underline[tr_screencursor] = FALSE;
            tr_screencursor++;
            tr_underline[tr_screencursor] = TRUE;
            since++;
        acase LF:
            ideallength = 0;
            while
            (   classes[tr_class - 6][tr_level - 1][tr_textcursor + ideallength] != ' '
             && classes[tr_class - 6][tr_level - 1][tr_textcursor + ideallength] != LF
             && classes[tr_class - 6][tr_level - 1][tr_textcursor + ideallength] != EOS
            )
            {   ideallength++;
            }
            checkword(tr_screencursor - since, since, ideallength);
            tr_textcursor += ideallength + 1;
            since = 0;
            tr_underline[tr_screencursor] = FALSE;
            tr_screencursor = 0;
            tr_underline[tr_screencursor] = TRUE;
            strcpy((char*) tr_digits, "        ");
        adefault:
            if (inchar >= 0x20)
            {   if (inchar == ' ' && since == 0 && classes[tr_class - 6][tr_level - 1][tr_textcursor] != ' ')
                {   ;
                } else
                {   since++;
                    if (since == 8 || inchar == ' ')
                    {   tr_digits[tr_screencursor] = inchar;
                        checkword(tr_screencursor - since + 1, since, since);
                        tr_textcursor += since;
                        since = 0;
                }   }

                if (tr_screencursor == 7)
                {   tr_digits[0] = tr_digits[1];
                    tr_digits[1] = tr_digits[2];
                    tr_digits[2] = tr_digits[3];
                    tr_digits[3] = tr_digits[4];
                    tr_digits[4] = tr_digits[5];
                    tr_digits[5] = tr_digits[6];
                    tr_digits[6] = inchar;
                    tr_digits[7] = ' ';
                } else
                {   tr_underline[tr_screencursor] = FALSE;
                    tr_digits[tr_screencursor] = inchar;
                    tr_screencursor++;
                    tr_underline[tr_screencursor] = TRUE;
        }   }   }

        if (classes[tr_class - 6][tr_level - 1][tr_textcursor] == EOS)
        {   for (i = 0; i < 8; i++)
            {   tr_underline[i] = FALSE;
            }

            secs = tr_remaining / 50;
            if (tr_remaining % 50)
            {   secs++;
            }
            words = tr_textcursor / (5 + 1);
            if (tr_textcursor % (5 + 1))
            {   words++;
            }
            tr_wpm = 60 * words / secs;

            tr_accuracy = 100 - (100 * tr_errors / tr_textcursor);

            sprintf((char*) tr_digits, "%4d WPM", tr_wpm);
            hiscore_register(HS_CLASS6LEVEL1_POS + ((tr_class - 6) * 4) + tr_level - 1, tr_wpm);
            stage = STAGE_CLASS68_WAITRESULTS;
        }
    acase STAGE_CLASS68_WAITRESULTS:
        tr_alternate();

        switch (inchar)
        {
        case TRKEY_ACCURACY:
            sprintf((char*) tr_digits, "%5d%%  ", tr_accuracy);
        acase TRKEY_SPEED:
            sprintf((char*) tr_digits, "%4d WPM", tr_wpm);
    }   }

    for (y = 0; y <  2; y++)
    {   for (x = 0; x < 64; x++)
        {   tr_changebgpixel(x, y, vdu_bgc);
    }   }
    for (y = 2; y <  9; y++)
    {   for (x = 0; x < 8; x++)
        {   tr_changebgpixel(      x     , y, vdu_bgc);
            tr_changebgpixel(13 + (x * 7), y, vdu_bgc);
            tr_changebgpixel(14 + (x * 7), y, vdu_bgc);
    }   }
    for (y = 9; y < 14; y++)
    {   for (x = 0; x < 64; x++)
        {   tr_changebgpixel(x, y, vdu_bgc);
    }   }

    for (x = 0; x < 8; x++)
    {   t = tr_digits[x] - 0x20;
        for (xx = 0; xx < 5; xx++)
        {   for (y = 0; y < 7; y++)
            {   if (tr_chars[t][y] & (0x80 >> xx))
                {   tr_changepixel(  8 + (x * 7) + xx, y + 2, vdu_fgc);
                } else
                {   tr_changebgpixel(8 + (x * 7) + xx, y + 2, vdu_bgc);
    }   }   }   }

    for (i = 0; i < 8; i++)
    {   if (tr_underline[i])
        {   for (xx = 0; xx < 5; xx++)
            {   tr_changepixel(8 + (i * 7) + xx, 11, vdu_fgc);
    }   }   }

    for (i = 0; i < 4; i++)
    {   if (tr_arrow[i])
        {   tr_changepixel(4, 1 + (i * 3), vdu_fgc);
            tr_changepixel(3, 2 + (i * 3), vdu_fgc);
            tr_changepixel(4, 2 + (i * 3), vdu_fgc);
            tr_changepixel(4, 3 + (i * 3), vdu_fgc);
    }   }

    playsound(FALSE);

    cycles_2650++; // so it ticks over enough to make eg. WAV recordings possible
    wa_checkinput();
    endofframe(vdu_bgc);

    if (console_start) console_start--;
    if (console_a    ) console_a--;
    if (console_b    ) console_b--;
    if (console_reset) console_reset--;
}

MODULE void class_getnewletter(void)
{   TRANSIENT       TEXT   newletter;
    PERSIST   const STRPTR classletters[5] =
    { "asdfghjkl;'",
      "qwertyuiop=",
      "zxcvbnm,./",
      "1234567890",
      "asdfghjkl;'zxcvbnm,./qwertyuiop=1234567890ASDFGHJKL:\"ZXCVBNM?QWERTYUIOP+!@#$%-&*()",
    };

    if (recmode == RECMODE_PLAY)
    {   // assert(IOBuffer);
        newletter = IOBuffer[offset++];
    } else
    {   do
        {   newletter = classletters[tr_class - 1][rand() % strlen(classletters[tr_class - 1])];
        } while (newletter == tr_gameletter);
        if (recmode == RECMODE_RECORD)
        {   // assert(MacroHandle);
            OutputBuffer[0] = newletter;
            DISCARD fwrite(OutputBuffer, 1, 1, MacroHandle);
            // should really check return code of fwrite()
    }   }

    tr_gameletter = newletter;
    tr_digits[tr_pos[tr_gameletter - 0x20] - 1] = tr_gameletter;
}

EXPORT void tr_setmemmap(void)
{   whichkeyrect = KEYRECT_TYPERIGHT;
}

EXPORT void tr_reset(void)
{   int i;

    // keyboard
    queuepos = 0;

    // screen
    strcpy((char*) tr_digits, "        ");
    for (i = 0; i < 8; i++)
    {   tr_underline[i] = FALSE;
    }
    for (i = 0; i < 2; i++)
    {   tr_arrow[i] = FALSE;
    }
    tr_update_capslock();

    // game
    stage        = STAGE_BOOT;
    tr_remaining = 0;
}

MODULE void tr_changepixel(int x, int y, int colour)
{
#ifdef BIGTYPERIGHT
    changepixel( x * 2     ,  y * 2     , colour);
    changepixel((x * 2) + 1,  y * 2     , colour);
    changepixel( x * 2     , (y * 2) + 1, colour);
    changepixel((x * 2) + 1, (y * 2) + 1, colour);
#else
    changepixel( x         ,  y         , colour);
#endif

    drawcontrolspixel(x, y, colour);
}

MODULE void tr_changebgpixel(int x, int y, int colour)
{
#ifdef BIGTYPERIGHT
    changebgpixel( x * 2     ,  y * 2     , colour);
    changebgpixel((x * 2) + 1,  y * 2     , colour);
    changebgpixel( x * 2     , (y * 2) + 1, colour);
    changebgpixel((x * 2) + 1, (y * 2) + 1, colour);
#else
    changebgpixel( x         ,  y         , colour);
#endif

    drawcontrolspixel(x, y, colour);
}

EXPORT void tr_update_capslock(void)
{   tr_arrow[3] = capslock ? TRUE : FALSE;
}

MODULE void game_getnewletter(void)
{   int i;

    if (tr_digits[0] != ' ')
    {   tr_lives--;
        ponghertz = HIGHBEEP;
        if (tr_lives >= 0 && tr_lives <= 9 && tr_score >= 0 && tr_score <= 999999) // should always be true
        {   sprintf((char*) tr_digits, "%d %6d", tr_lives, tr_score);
            hiscore_register(HS_GAME1_POS + tr_usergame - 1, tr_score);
        } else
        {   sprintf((char*) tr_digits, "# ######");
        }
        if (tr_lives)
        {   stage = STAGE_GAME_WAIT;
            tr_remaining = 0;
        } else
        {   stage = STAGE_MENU;
    }   }
    else
    {   tr_digits[0] = tr_digits[1];
        tr_digits[1] = tr_digits[2];
        tr_digits[2] = tr_digits[3];
        tr_digits[3] = tr_digits[4];
        tr_digits[4] = tr_digits[5];
        tr_digits[5] = tr_digits[6];
        tr_digits[6] = tr_digits[7];

        if (recmode == RECMODE_PLAY)
        {   // assert(IOBuffer);
            tr_digits[7] = IOBuffer[offset++];
        } else
        {   if (tr_game < 5 && tr_remaining >= 30 * 50) // 30 seconds
            {   tr_digits[7] = ' ';
            } else
            {   tr_digits[7] = 'A' + (rand() % 26);
            }
            if (recmode == RECMODE_RECORD)
            {   // assert(MacroHandle);
                OutputBuffer[0] = tr_digits[7];
                DISCARD fwrite(OutputBuffer, 1, 1, MacroHandle);
                // should really check return code of fwrite()
        }   }

        tr_gameletter = EOS;
        for (i = 0; i < 8; i++)
        {   if (tr_digits[i] != ' ')
            {   tr_gameletter = tr_digits[i];
                break;
}   }   }   }

MODULE void tr_alternate(void)
{   if (frames % 80 < 40)
    {   tr_arrow[0] = TRUE;
        tr_arrow[1] = FALSE;
    } else
    {   tr_arrow[0] = FALSE;
        tr_arrow[1] = TRUE;
}   }

EXPORT void tr_serialize_cos(void)
{   int i;

    serialize_byte((UBYTE*) &tr_gameletter);
    serialize_byte_int(&stage             );
    serialize_byte_int(&tr_class          );
    serialize_byte_int(&tr_game           );
    serialize_byte_int(&tr_level          );
    serialize_byte_int(&tr_lives          );
    serialize_byte_int(&tr_screencursor   );
    serialize_byte_int(&tr_accuracy       );
    serialize_long((ULONG*) &tr_remaining );
    serialize_long((ULONG*) &tr_score     );
    serialize_long((ULONG*) &tr_errors    );
    serialize_long((ULONG*) &tr_textcursor);
    serialize_long((ULONG*) &tr_wpm       );
    for (i = 0; i < 8; i++)
    {   serialize_byte((UBYTE*) &tr_digits[i]);
        serialize_byte((UBYTE*) &tr_underline[i]);
    }
    serialize_byte((UBYTE*) &tr_arrow[0]  );
    serialize_byte((UBYTE*) &tr_arrow[1]  );
    serialize_byte_int(&tr_usergame       );
    serialize_byte_int(&since             );

    // perhaps we should serialize capslock and/or frames variables?
}

EXPORT void tr_viewscreen(void)
{   int  i;

    if (tr_arrow[0] || tr_arrow[1] || tr_arrow[2] || tr_arrow[3])
    {   zprintf(TEXTPEN_VIEW, "<%s\n", tr_digits);
    } else
    {   zprintf(TEXTPEN_VIEW, " %s\n", tr_digits);
    }

    for (i = 0; i < 8; i++)
    {   gtempstring[i] = tr_underline[i] ? '_' : ' ';
    }
    gtempstring[8] = EOS;
    zprintf(TEXTPEN_VIEW, " %s\n\n", gtempstring);
}

MODULE FLAG checkword(int screenstart, int reallength, int ideallength)
{   FAST int  i;
#ifdef VERBOSE
    FAST TEXT tempstring1[8 + 1],
              tempstring2[8 + 1];
#endif

    // assert(reallength  >= 1);
    // assert(reallength  <= 8);
    // assert(ideallength >= 1);
    // assert(ideallength <= 8);

#ifdef VERBOSE
    zstrncpy(tempstring1, &tr_digits[screenstart],                             reallength);
    zstrncpy(tempstring2, &classes[tr_class - 6][tr_level - 1][tr_textcursor], ideallength);
#endif

    if (reallength != ideallength)
    {   goto BAD;
    }

    for (i = 0; i < ideallength; i++)
    {   if (tr_digits[screenstart + i] != classes[tr_class - 6][tr_level - 1][tr_textcursor + i])
        {   goto BAD;
    }   }

#ifdef VERBOSE
    zprintf(TEXTPEN_LOG, "Good (\"%s\").\n", tempstring1);
#endif

    return TRUE;

BAD:
#ifdef VERBOSE
    zprintf(TEXTPEN_LOG, "Bad (heard \"%s\" vs. expected \"%s\").\n", tempstring1, tempstring2);
#endif

    ponghertz = HIGHBEEP;
    tr_errors++;
    return FALSE;
}
