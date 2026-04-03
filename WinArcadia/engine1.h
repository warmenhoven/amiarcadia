/* DEFINES----------------------------------------------------------------

(None)

EXPORTED STRUCTURES---------------------------------------------------- */

EXPORT const UWORD default_console[4] =
{   SCAN_F1,
    SCAN_F2,
    SCAN_F3,
    SCAN_F4
};

#ifdef AMIGA
EXPORT const UWORD default_keypads[2][NUMKEYS] =
{ { SCAN_SPACEBAR,
    SCAN_A1, SCAN_A2,   SCAN_A3,
    SCAN_Q,  SCAN_W,    SCAN_E,
    SCAN_A,  SCAN_S,    SCAN_D,
    SCAN_Z,  SCAN_X,    SCAN_C,
    SCAN_A4, SCAN_R,    SCAN_F,    SCAN_V,
    SCAN_UP, SCAN_DOWN, SCAN_LEFT, SCAN_RIGHT,
    SCAN_A5, SCAN_A6,   SCAN_A7,
    UNASSIGNED, UNASSIGNED, UNASSIGNED, UNASSIGNED
  },
  /* ( ) / *
     7 8 9 -
     4 5 6 +
     1 2 3 E
     -0- . n */
  { SCAN_N0,
    SCAN_OP, SCAN_CP,  SCAN_SL,
    SCAN_N7, SCAN_N8,  SCAN_N9,
    SCAN_N4, SCAN_N5,  SCAN_N6,
    SCAN_N1, SCAN_N2,  SCAN_N3,
    SCAN_AS, SCAN_MI,  SCAN_PL,   SCAN_NE,
    SCAN_I , SCAN_K,   SCAN_J,    SCAN_L,
    SCAN_ND, SCAN_DEL, SCAN_HELP,
    SCAN_U,  SCAN_O,   SCAN_M,    SCAN_FULLSTOP
} };
#endif
#ifdef WIN32
EXPORT const UWORD default_keypads[2][NUMKEYS] =
{ { SCAN_SPACEBAR,                                 //  0     (1st firebutton)
    SCAN_A1, SCAN_A2,   SCAN_A3,                   //  1.. 3 (1st row)
    SCAN_Q,  SCAN_W,    SCAN_E,                    //  4.. 6 (2nd row)
    SCAN_A,  SCAN_S,    SCAN_D,                    //  7.. 9 (3rd row)
    SCAN_Z,  SCAN_X,    SCAN_C,                    // 10..12 (4th row)
    SCAN_A4, SCAN_R,    SCAN_F,    SCAN_V,         // 13..16 (Palladium keys)
    SCAN_UP, SCAN_DOWN, SCAN_LEFT, SCAN_RIGHT,     // 17..20 (orthagonal paddles)
    SCAN_A5, SCAN_A6,   SCAN_A7,                   // 21..23 (2nd..4th firebuttons)
    UNASSIGNED, UNASSIGNED, UNASSIGNED, UNASSIGNED // 24..27 (diagonal paddles)
  },
  /* N / * -
     7 8 9 +
     4 5 6 +
     1 2 3 E
     -0- . n */
  { SCAN_N0,                                       //  0     (1st firebutton)
    SCAN_NL, SCAN_SL, SCAN_AS,                     //  1.. 3 (1st row)
    SCAN_N7, SCAN_N8, SCAN_N9,                     //  4.. 6 (2nd row)
    SCAN_N4, SCAN_N5, SCAN_N6,                     //  7.. 9 (3rd row)
    SCAN_N1, SCAN_N2, SCAN_N3,                     // 10..12 (4th row)
    SCAN_MI, SCAN_PL, SCAN_NE,   SCAN_ND,          // 13..16 (Palladium keys)
    SCAN_I , SCAN_K,  SCAN_J,    SCAN_L,           // 17..20 (orthagonal paddles)
    SCAN_DEL,SCAN_END,SCAN_PGDN,                   // 21..23 (2nd..4th firebuttons)
    SCAN_U,  SCAN_O,  SCAN_M,    SCAN_FULLSTOP     // 24..27 (diagonal paddles)
} };
#endif

EXPORT struct HiScoreStruct hiscore[HISCORES] = {
{ { 0x1800,      0,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, //  0. 3D Attack
{ { 0x1802, 0x180C,      0,      0 }, 2, 0x01, 0, "", 0, 0 }, //  1. 3D Soccer (p1, p2)
{ { 0x1825,      0,      0,      0 }, 2, 0x01, 0, "", 0, 0 }, //  2. Akane's Home Run
{ { 0x1816, 0x181D,      0,      0 }, 3, 0x01, 0, "", 0, 0 }, //  3. Alien Invaders (normal, high)
{ { 0x1805, 0x180A,      0,      0 }, 2, 0x01, 0, "", 0, 0 }, //  4. American Football (p1, p2)
{ { 0x1800, 0x180A,      0,      0 }, 6, 0x01, 0, "", 0, 0 }, //  5. Astro Invader (normal, high)
{ { 0x1810, 0x181E,      0,      0 }, 2, 0x01, 0, "", 0, 0 }, //  6. Baseball (home, visitors)
{ { 0x1831, 0x1839,      0,      0 }, 4, 0x01, 0, "", 0, 0 }, //  7. Basketball (p1, p2)
// Basketball might be less than 4 digits but is at least 1 digit
{ { 0x1814, 0x1819,      0,      0 }, 3, 0x01, 0, "", 0, 0 }, //  8. Boxing (p1, p2)
{ { 0x1811, 0x1816, 0x1800, 0x180B }, 4, 0x01, 0, "", 0, 0 }, //  9. Breakaway (games 1..4 normal, games 1..4 high, games 5..16 p1, games 5..16 p2)
{ { 0x1800, 0x180E, 0x1860, 0x186E }, 2, 0x01, 0, "", 0, 0 }, // 10. Capture (p1, p2, p1 previous, p2 previous)
{ { 0x1800, 0x1809,      0,      0 }, 6, 0x01, 0, "", 0, 0 }, // 11. Cunt Trax (normal, high)
{ { 0x1805, 0x180B,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 12. Circus (p1, p2)
// Capture has 6 more previous scores that we don't handle
{ { 0x1826, 0x1856, 0x1896,      0 }, 5, 0x01, 0, "", 0, 0 }, // 13. Crazy Clitlicker (normal, normal, high)
{ { 0x1810, 0x1819,      0,      0 }, 6, 0x01, 0, "", 0, 0 }, // 14. Cock Gobbler (normal, high)
{ { 0x1801,      0,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 15. Doraemon
{ { 0x1801, 0x180B,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 16. Dr. Slump (normal, high)
{ { 0x1800, 0x180B,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 17. Escape (normal, high)
{ { 0x18CB,      0,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 18. Frigger aka Sprogger :-)
{ { 0x1810, 0x181A,      0,      0 }, 6, 0x01, 0, "", 0, 0 }, // 19. Fucking Fish (high, normal)
{ { 0x1800, 0x180A,      0,      0 }, 6, 0x01, 0, "", 0, 0 }, // 20. Gundam (normal, high)
{ { 0x1810, 0x181B,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 21. Homo (normal, high)
{ { 0x1802,      0,      0,      0 }, 6, 0x01, 0, "", 0, 0 }, // 22. Journey
{ { 0x1801, 0x180B,      0,      0 }, 4, 0x01, 0, "", 0, 0 }, // 23. JTron (high, normal)
{ { 0x1810, 0x181B,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 24. Jump Bug (normal, high)
{ { 0x1800, 0x180B,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 25. Jungler (normal, high)
{ { 0x1800, 0x180B,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 26. Spackross (normal, high)
{ { 0x1801, 0x180A,      0,      0 }, 6, 0x01, 0, "", 0, 0 }, // 27. Missile War (normal, high)
// Missile War might be less (or more) than 6 digits but is at least 4 digits
{ { 0x1AC1, 0x1ACC,      0,      0 }, 2, 0x01, 0, "", 0, 0 }, // 28. Monaco Grand Prix (p1, p2)
{ { 0x1800, 0x1809,      0,      0 }, 6, 0x01, 0, "", 0, 0 }, // 29. Nibblemen/Super Cock Gobbler (normal, high)
{ { 0x1819, 0x188B,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 30. Pararooter (normal, high)
{ { 0x1800, 0x180B,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 31. Herpes (normal, high)
{ { 0x1800, 0x180B,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 32. R2D Skank (normal, high)
{ { 0x1802, 0x1809,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 33. Red Snatch (normal, high)
{ { 0x1800, 0x180B,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 34. Robot Killer (normal, high)
{ { 0x1801, 0x1807,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 35. Root 69 (normal, high)
{ { 0x1812, 0x181C,      0,      0 }, 2, 0x01, 0, "", 0, 0 }, // 36. 2D Wogball (p1, p2)
{ { 0x1800, 0x180A,      0,      0 }, 6, 0x01, 0, "", 0, 0 }, // 37. Space Attack/Space War (normal, high)
{ { 0x1806,      0,      0,      0 }, 4, 0x01, 0, "", 0, 0 }, // 38. Space Buster
{ { 0x180A, 0x1879,      0,      0 }, 4, 0x01, 0, "", 0, 0 }, // 39. Space Mission (normal, normal)
{ { 0x1800, 0x180C,      0,      0 }, 4, 0x01, 0, "", 0, 0 }, // 40. Space Rooters (normal, high)
{ { 0x1800, 0x180C,      0,      0 }, 4, 0x01, 0, "", 0, 0 }, // 41. Space Squadron (normal, high)
{ { 0x1AB0, 0x1ABC,      0,      0 }, 4, 0x01, 0, "", 0, 0 }, // 42. Space Vultures (normal, high)
{ { 0x1810, 0x181B,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 43. Spiders (normal, high)
{ { 0x1807,      0,      0,      0 }, 6, 0x01, 0, "", 0, 0 }, // 44. Super Buggery 1 (normal, high)
// Super Bug might be more than 6 digits
{ { 0x1807,      0,      0,      0 }, 6, 0x01, 0, "", 0, 0 }, // 45. Super Buggery 2 (normal, high)
{ { 0x1800, 0x180A,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 46. Spanks a Lot (normal, high)
{ { 0x1A07, 0x1A08,      0,      0 }, 4, 0x10, 0, "", 0, 0 }, // 47. Titris (Austrian) (p1, p2)
{ { 0x1A2B,      0,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 48. Titris (Japanese)
{ { 0x1810, 0x181B,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 49. The End (normal, high)
{ { 0x1800, 0x180A,      0,      0 }, 5, 0x01, 0, "", 0, 0 }, // 50. Turtles/Gerbils (normal, high)
// coin-ops
{ { 0x1B81, 0x1A61, 0x1921,      0 }, 5,  -32, 0, "", 0, 0 }, // 51. Astro Wank (p1, high, p2)
{ { 0x1B81, 0x1A41, 0x1901,      0 }, 5,  -32, 0, "", 0, 0 }, // 52. Galaxia (p1, high, p2)
{ { 0x1841, 0x1852, 0x1858,      0 }, 5, 0x01, 0, "", 0, 0 }, // 53. Laser Battle (p1, high, p2)
{ { 0x1841, 0x1852, 0x1858,      0 }, 5, 0x01, 0, "", 0, 0 }, // 54. Lesbian (p1, high, p2)
{ { 0x1843, 0x1850, 0x185D,      0 }, 6, 0x01, 0, "", 0, 0 }, // 55. Malzak 1 (p1, high, p2)
{ { 0x1843, 0x1850, 0x185D,      0 }, 6, 0x01, 0, "", 0, 0 }, // 56. Malzak 2 (p1, high, p2)
// Pong
{ {      0,      0,      0,      0 }, 2, 0x01, 0, "", 0, 0 }, // 57. Practice (AY-3-8550)
{ {      0,      0,      0,      0 }, 2, 0x01, 0, "", 0, 0 }, // 58. Rifle #1
{ {      0,      0,      0,      0 }, 2, 0x01, 0, "", 0, 0 }, // 59. Rifle #2
{ {      0,      0,      0,      0 }, 2, 0x01, 0, "", 0, 0 }, // 60. Practice (AY-3-8600)
{ {      0,      0,      0,      0 }, 2, 0x01, 0, "", 0, 0 }, // 61. Basketball Practice
{ {      0,      0,      0,      0 }, 2, 0x01, 0, "", 0, 0 }, // 62. 1-Player Target
};
/* HS_#?_POS must accord with the above table!
Unsupported:
{ 0x1854, 0x18C4, 0x1848, 0x18B8, 3, 0x01 }, // 3D Bowling (p1, p2, p1 previous, p2 previous)
3D Bowling has 2 more previous scores that we don't handle
unfortunately, 3D Bowling also uses $185x for other purposes
{ 0x18B3, 0x18BB,      0,      0, 2, 0x01 }, // Auto Race (p1, p2)
 Auto Race (each 2-player game is "first to 20")
 Battle
*Blackjack/Poker
 Brain Quiz (probably tiles are used for other purposes)
 Combat (each game is "first to 20")
 Dictionary
 Golf (but good scores are low)
*Grand Slam Tennis (but remember 0s are Os)
*Horse Racing
 Ocean Battle
 Star Chess
* = might be supportable */

EXPORT TEXT asciiname_short[259][3 + 1] =
{   "NUL", // $00 NULl
    "SOH", // $01 Start Of Heading
    "STX", // $02 Start of TeXt
    "ETX", // $03 End of TeXt
    "EOT", // $04 End Of Transmission
    "ENQ", // $05 ENQuiry
    "ACK", // $06 ACKnowledge
    "BEL", // $07 BELl
    "BS",  // $08 BackSpace
    "HT",  // $09 Horizontal Tab
    "LF",  // $0A Line Feed
    "VT",  // $0B Vertical Tab
    "ZFF", // $0C Form Feed (normally FF but that can cause confusion with $FF)
    "CR",  // $0D Carriage Return
    "SO",  // $0E Shift Out
    "SI",  // $0F Shift In
    "DLE", // $10 Data Link Escape
    "DC1", // $11 Device Control 1 (tape reader enabled)
    "DC2", // $12 Device Control 2 (tape punch enabled)
    "DC3", // $13 Device Control 3 (tape reader disabled)
    "DC4", // $14 Device Control 4 (tape punch disabled)
    "NAK", // $15 Negative AcKnowledge
    "SYN", // $16 SYNchonous idle
    "ETB", // $17 End of Transmission Block
    "CAN", // $18 CANcel
    "EM",  // $19 End of Medium
    "SUB", // $1A SUBstitute
    "ESC", // $1B ESCape
    "FS",  // $1C File Separator
    "GS",  // $1D Group Separator
    "RS",  // $1E Record Separator
    "US"   // $1F Unit Separator
// $20+ are initialized in engine_setup()
};

// MODULE STRUCTURES------------------------------------------------------

MODULE struct
{   const UWORD address;
    const UBYTE mask;
} a_keyemul[48] =
{   { A_CONSOLE,      0x01 }, // START
    { A_CONSOLE,      0x02 }, // A
    { A_CONSOLE,      0x04 }, // B
    { 0,                 0 }, // RESET
    { A_P1LEFTKEYS,   0x08 }, // p1 '1'
    { A_P1LEFTKEYS,   0x04 }, // p1 '4'
    { A_P1LEFTKEYS,   0x02 }, // p1 '7'
    { A_P1LEFTKEYS,   0x01 }, // p1 'Cl'
    { A_P1MIDDLEKEYS, 0x08 }, // p1 '2'
    { A_P1MIDDLEKEYS, 0x04 }, // p1 '5'
    { A_P1MIDDLEKEYS, 0x02 }, // p1 '8'
    { A_P1MIDDLEKEYS, 0x01 }, // p1 '0'
    { A_P1RIGHTKEYS,  0x08 }, // p1 '3'
    { A_P1RIGHTKEYS,  0x04 }, // p1 '6'
    { A_P1RIGHTKEYS,  0x02 }, // p1 '9'
    { A_P1RIGHTKEYS,  0x01 }, // p1 'En'
    { A_P2LEFTKEYS,   0x08 }, // p2 '1'
    { A_P2LEFTKEYS,   0x04 }, // p2 '4'
    { A_P2LEFTKEYS,   0x02 }, // p2 '7'
    { A_P2LEFTKEYS,   0x01 }, // p2 'Cl'
    { A_P2MIDDLEKEYS, 0x08 }, // p2 '2'
    { A_P2MIDDLEKEYS, 0x04 }, // p2 '5'
    { A_P2MIDDLEKEYS, 0x02 }, // p2 '8'
    { A_P2MIDDLEKEYS, 0x01 }, // p2 '0'
    { A_P2RIGHTKEYS,  0x08 }, // p2 '3'
    { A_P2RIGHTKEYS,  0x04 }, // p2 '6'
    { A_P2RIGHTKEYS,  0x02 }, // p2 '9'
    { A_P2RIGHTKEYS,  0x01 }, // p2 'En'
    { 0,                 0 }, // p1 up
    { 0,                 0 }, // p1 left
    { 0,                 0 }, // p1 down
    { 0,                 0 }, // p1 right
    { 0,                 0 }, // p2 up
    { 0,                 0 }, // p2 left
    { 0,                 0 }, // p2 down
    { 0,                 0 }, // p2 right
    { A_P1MIDDLEKEYS, 0x08 }, // p1 '2'
    { A_P1MIDDLEKEYS, 0x08 }, // p1 '2'
    { A_P2MIDDLEKEYS, 0x08 }, // p2 '2'
    { A_P2MIDDLEKEYS, 0x08 }, // p2 '2'
    { A_P1PALLADIUM,  0x01 }, // p1 x1
    { A_P1PALLADIUM,  0x02 }, // p1 x2
    { A_P1PALLADIUM,  0x04 }, // p1 x3
    { A_P1PALLADIUM,  0x08 }, // p1 x4
    { A_P2PALLADIUM,  0x01 }, // p2 x1
    { A_P2PALLADIUM,  0x02 }, // p2 x2
    { A_P2PALLADIUM,  0x04 }, // p2 x3
    { A_P2PALLADIUM,  0x08 }  // p2 x4
}, i_keyemul[40] =
{   { IE_CONSOLE,      0x40 }, // START
    { IE_CONSOLE,      0x80 }, // A
    { 0,                  0 }, // B
    { 0,                  0 }, // RESET
    { IE_P1LEFTKEYS,   0x80 }, // p1 '1'
    { IE_P1LEFTKEYS,   0x40 }, // p1 '4'
    { IE_P1LEFTKEYS,   0x20 }, // p1 '7'
    { IE_P1LEFTKEYS,   0x10 }, // p1 'Cl'
    { IE_P1MIDDLEKEYS, 0x80 }, // p1 '2'
    { IE_P1MIDDLEKEYS, 0x40 }, // p1 '5'
    { IE_P1MIDDLEKEYS, 0x20 }, // p1 '8'
    { IE_P1MIDDLEKEYS, 0x10 }, // p1 '0'
    { IE_P1RIGHTKEYS,  0x80 }, // p1 '3'
    { IE_P1RIGHTKEYS,  0x40 }, // p1 '6'
    { IE_P1RIGHTKEYS,  0x20 }, // p1 '9'
    { IE_P1RIGHTKEYS,  0x10 }, // p1 'En'
    { IE_P2LEFTKEYS,   0x80 }, // p2 '1'
    { IE_P2LEFTKEYS,   0x40 }, // p2 '4'
    { IE_P2LEFTKEYS,   0x20 }, // p2 '7'
    { IE_P2LEFTKEYS,   0x10 }, // p2 'Cl'
    { IE_P2MIDDLEKEYS, 0x80 }, // p2 '2'
    { IE_P2MIDDLEKEYS, 0x40 }, // p2 '5'
    { IE_P2MIDDLEKEYS, 0x20 }, // p2 '8'
    { IE_P2MIDDLEKEYS, 0x10 }, // p2 '0'
    { IE_P2RIGHTKEYS,  0x80 }, // p2 '3'
    { IE_P2RIGHTKEYS,  0x40 }, // p2 '6'
    { IE_P2RIGHTKEYS,  0x20 }, // p2 '9'
    { IE_P2RIGHTKEYS,  0x10 }, // p2 'En'
    { 0,                  0 }, // p1 up
    { 0,                  0 }, // p1 left
    { 0,                  0 }, // p1 down
    { 0,                  0 }, // p1 right
    { 0,                  0 }, // p2 up
    { 0,                  0 }, // p2 left
    { 0,                  0 }, // p2 down
    { 0,                  0 }, // p2 right
    { IE_P1MIDDLEKEYS, 0x80 }, // p1 '2'
    { IE_P1MIDDLEKEYS, 0x80 }, // p1 '2'
    { IE_P2MIDDLEKEYS, 0x80 }, // p1 '2'
    { IE_P2MIDDLEKEYS, 0x80 }  // p1 '2'
}, e_keyemul[36] =
{   { IE_CONSOLE,      0x40 }, // START
    { IE_CONSOLE,      0x80 }, // UC
    { IE_CONSOLE,      0x20 }, // LC
    { IE_CONSOLE,      0x10 }, // RESET
    { IE_P1LEFTKEYS,   0x80 }, // p1
    { IE_P1LEFTKEYS,   0x40 }, // p1
    { IE_P1LEFTKEYS,   0x20 }, // p1
    { IE_P1LEFTKEYS,   0x10 }, // p1
    { IE_P1MIDDLEKEYS, 0x80 }, // p1
    { IE_P1MIDDLEKEYS, 0x40 }, // p1
    { IE_P1MIDDLEKEYS, 0x20 }, // p1
    { IE_P1MIDDLEKEYS, 0x10 }, // p1
    { IE_P1RIGHTKEYS,  0x80 }, // p1
    { IE_P1RIGHTKEYS,  0x40 }, // p1
    { IE_P1RIGHTKEYS,  0x20 }, // p1
    { IE_P1RIGHTKEYS,  0x10 }, // p1
    { IE_P2LEFTKEYS,   0x80 }, // p2
    { IE_P2LEFTKEYS,   0x40 }, // p2
    { IE_P2LEFTKEYS,   0x20 }, // p2
    { IE_P2LEFTKEYS,   0x10 }, // p2
    { IE_P2MIDDLEKEYS, 0x80 }, // p2
    { IE_P2MIDDLEKEYS, 0x40 }, // p2
    { IE_P2MIDDLEKEYS, 0x20 }, // p2
    { IE_P2MIDDLEKEYS, 0x10 }, // p2
    { IE_P2RIGHTKEYS,  0x80 }, // p2
    { IE_P2RIGHTKEYS,  0x40 }, // p2
    { IE_P2RIGHTKEYS,  0x20 }, // p2
    { IE_P2RIGHTKEYS,  0x10 }, // p2
    { 0,                  0 }, // p1 up
    { 0,                  0 }, // p1 left
    { 0,                  0 }, // p1 down
    { 0,                  0 }, // p1 right
    { 0,                  0 }, // p2 up
    { 0,                  0 }, // p2 left
    { 0,                  0 }, // p2 down
    { 0,                  0 }  // p2 right
}, s_keyemul[28] =
{   { 0,    0x1 }, // SENS
    { 0,    0x2 }, // INT
    { 0,    0x4 }, // MON
    { 0,    0x8 }, // RST
    { 1,    0x1 }, // WCAS
    { 1,    0x2 }, // RCAS
    { 1,    0x4 }, // STEP
    { 1,    0x8 }, // RUN
    { 2,    0x1 }, // BKPT
    { 2,    0x2 }, // REG
    { 2,    0x4 }, // MEM
    { 2,    0x8 }, // ENT NXT
    { 3,    0x8 }, // C
    { 3,    0x4 }, // 8
    { 3,    0x2 }, // 4
    { 3,    0x1 }, // 0
    { 4,    0x8 }, // D
    { 4,    0x4 }, // 9
    { 4,    0x2 }, // 5
    { 4,    0x1 }, // 1
    { 5,    0x8 }, // E
    { 5,    0x4 }, // A
    { 5,    0x2 }, // 6
    { 5,    0x1 }, // 2
    { 6,    0x8 }, // F
    { 6,    0x4 }, // B
    { 6,    0x2 }, // 7
    { 6,    0x1 }  // 3
}, selbst_keyemul[24] =
{   { 0,    0x1 }, // C/BLANK
    { 0,    0x2 }, // 8/+
    { 0,    0x4 }, // 4/H
    { 0,    0x8 }, // 0/L
    { 1,    0x1 }, // D/R
    { 1,    0x2 }, // 9/G
    { 1,    0x4 }, // 5/P
    { 1,    0x8 }, // 1/S
    { 2,    0x1 }, // E/C
    { 2,    0x2 }, // A/8
    { 2,    0x4 }, // 6/4
    { 2,    0x8 }, // 2/0
    { 3,    0x1 }, // F/D
    { 3,    0x2 }, // B/9
    { 3,    0x4 }, // 7/5
    { 3,    0x8 }, // 3/1
    { 4,    0x1 }, // CMD/E
    { 4,    0x2 }, // RUN/A
    { 4,    0x4 }, // GOTO/6
    { 4,    0x8 }, // RST/2
    { 5,    0x1 }, // FLAG/F
    { 5,    0x2 }, // MON/B
    { 5,    0x4 }, // PC/7
    { 5,    0x8 }  // NXT/3
};

EXPORT struct IdealStruct idealfreq_ntsc[256] = {
{HZ_REST, "rest",NOTE_REST,MIDI_REST}, // $00
{ HZ_B7 , "B7" , NOTE_B7 , MIDI_B7  }, // $01
{ HZ_E7 , "E7" , NOTE_E7 , MIDI_E7  }, // $02
{ HZ_B6 , "B6" , NOTE_B6 , MIDI_B6  }, // $03
{ HZ_G6 , "G6" , NOTE_G6 , MIDI_G6  }, // $04
{ HZ_E6 , "E6" , NOTE_E6 , MIDI_E6  }, // $05
{ HZ_CS6, "C#6", NOTE_CS6, MIDI_CS6 }, // $06
{ HZ_B5 , "B5" , NOTE_B5 , MIDI_B5  }, // $07
{ HZ_A5 , "A5" , NOTE_A5 , MIDI_A5  }, // $08
{ HZ_G5 , "G5" , NOTE_G5 , MIDI_G5  }, // $09
{ HZ_F5 , "F5" , NOTE_F5 , MIDI_F5  }, // $0A
{ HZ_E5 , "E5" , NOTE_E5 , MIDI_E5  }, // $0B
{ HZ_DS5, "D#5", NOTE_DS5, MIDI_DS5 }, // $0C
{ HZ_CS5, "C#5", NOTE_CS5, MIDI_CS5 }, // $0D
{ HZ_C5 , "C5" , NOTE_C5 , MIDI_C5  }, // $0E
{ HZ_B4 , "B4" , NOTE_B4 , MIDI_B4  }, // $0F
{ HZ_AS4, "A#4", NOTE_AS4, MIDI_AS4 }, // $10
{ HZ_A4 , "A4" , NOTE_A4 , MIDI_A4  }, // $11
{ HZ_GS4, "G#4", NOTE_GS4, MIDI_GS4 }, // $12
{ HZ_G4 , "G4" , NOTE_G4 , MIDI_G4  }, // $13
{ HZ_FS4, "F#4", NOTE_FS4, MIDI_FS4 }, // $14
{ HZ_F4 , "F4" , NOTE_F4 , MIDI_F4  }, // $15
{ HZ_F4 , "f4" , NOTE_F4 , MIDI_F4  }, // $16
{ HZ_E4 , "E4" , NOTE_E4 , MIDI_E4  }, // $17
{ HZ_DS4, "D#4", NOTE_DS4, MIDI_DS4 }, // $18 e4 for Interton/Elektor
{ HZ_D4 , "d4" , NOTE_D4 , MIDI_D4  }, // $19 Super Bug expects this. d#4 for Interton/Elektor
{ HZ_D4 , "D4" , NOTE_D4 , MIDI_D4  }, // $1A
{ HZ_CS4, "C#4", NOTE_CS4, MIDI_CS4 }, // $1B d4 for Interton/Elektor
{ HZ_CS4, "c#4", NOTE_CS4, MIDI_CS4 }, // $1C Alien Invaders expects this
{ HZ_C4 , "C4" , NOTE_C4 , MIDI_C4  }, // $1D middle C
{ HZ_B3 , "b3" , NOTE_B3 , MIDI_B3  }, // $1E
{ HZ_B3 , "B3" , NOTE_B3 , MIDI_B3  }, // $1F
{ HZ_AS3, "a#3", NOTE_AS3, MIDI_AS3 }, // $20
{ HZ_AS3, "A#3", NOTE_AS3, MIDI_AS3 }, // $21
{ HZ_A3 , "a3" , NOTE_A3 , MIDI_A3  }, // $22
{ HZ_A3 , "A3" , NOTE_A3 , MIDI_A3  }, // $23
{ HZ_GS3, "g#3", NOTE_GS3, MIDI_GS3 }, // $24
{ HZ_GS3, "G#3", NOTE_GS3, MIDI_GS3 }, // $25
{ HZ_GS3, "g#3", NOTE_GS3, MIDI_GS3 }, // $26 Super Bug expects this?
{ HZ_G3 , "G3" , NOTE_G3 , MIDI_G3  }, // $27
{ HZ_G3 , "g3" , NOTE_G3 , MIDI_G3  }, // $28 American Football expects this. f#3 for Interton/Elektor
{ HZ_FS3, "f#3", NOTE_FS3, MIDI_FS3 }, // $29
{ HZ_FS3, "F#3", NOTE_FS3, MIDI_FS3 }, // $2A
{ HZ_F3 , "f3" , NOTE_F3 , MIDI_F3  }, // $2B
{ HZ_F3 , "F3" , NOTE_F3 , MIDI_F3  }, // $2C
{ HZ_F3 , "f3" , NOTE_F3 , MIDI_F3  }, // $2D
{ HZ_E3 , "e3" , NOTE_E3 , MIDI_E3  }, // $2E
{ HZ_E3 , "E3" , NOTE_E3 , MIDI_E3  }, // $2F
{ HZ_DS3, "d#3", NOTE_DS3, MIDI_DS3 }, // $30 arguable
{ HZ_DS3, "D#3", NOTE_DS3, MIDI_DS3 }, // $31
{ HZ_DS3, "d#3", NOTE_DS3, MIDI_DS3 }, // $32
{ HZ_D3 , "d3" , NOTE_D3 , MIDI_D3  }, // $33
{ HZ_D3 , "d3" , NOTE_D3 , MIDI_D3  }, // $34
{ HZ_D3 , "D3" , NOTE_D3 , MIDI_D3  }, // $35
{ HZ_D3 , "d3" , NOTE_D3 , MIDI_D3  }, // $36 arguable
{ HZ_CS3, "c#3", NOTE_CS3, MIDI_CS3 }, // $37
{ HZ_CS3, "C#3", NOTE_CS3, MIDI_CS3 }, // $38
{ HZ_CS3, "c#3", NOTE_CS3, MIDI_CS3 }, // $39 arguable
{ HZ_CS3, "c#3", NOTE_CS3, MIDI_CS3 }, // $3A Super Bug expects this
{ HZ_C3 , "C3" , NOTE_C3 , MIDI_C3  }, // $3B
{ HZ_C3 , "c3" , NOTE_C3 , MIDI_C3  }, // $3C
{ HZ_C3 , "c3" , NOTE_C3 , MIDI_C3  }, // $3D
{ HZ_B2 , "b2" , NOTE_B2 , MIDI_B2  }, // $3E
{ HZ_B2 , "B2" , NOTE_B2 , MIDI_B2  }, // $3F
{ HZ_AS2, "a#2", NOTE_AS2, MIDI_AS2 }, // $40 arguable
{ HZ_AS2, "a#2", NOTE_AS2, MIDI_AS2 }, // $41
{ HZ_AS2, "A#2", NOTE_AS2, MIDI_AS2 }, // $42
{ HZ_AS2, "a#2", NOTE_AS2, MIDI_AS2 }, // $43
{ HZ_AS2, "a#2", NOTE_AS2, MIDI_AS2 }, // $44 arguable
{ HZ_A2 , "a2" , NOTE_A2 , MIDI_A2  }, // $45
{ HZ_A2 , "A2" , NOTE_A2 , MIDI_A2  }, // $46
{ HZ_A2 , "a2" , NOTE_A2 , MIDI_A2  }, // $47
{ HZ_A2 , "a2" , NOTE_A2 , MIDI_A2  }, // $48
{ HZ_GS2, "g#2", NOTE_GS2, MIDI_GS2 }, // $49
{ HZ_GS2, "g#2", NOTE_GS2, MIDI_GS2 }, // $4A
{ HZ_GS2, "G#2", NOTE_GS2, MIDI_GS2 }, // $4B
{ HZ_GS2, "g#2", NOTE_GS2, MIDI_GS2 }, // $4C
{ HZ_GS2, "g#2", NOTE_GS2, MIDI_GS2 }, // $4D
{ HZ_G2 , "g2" , NOTE_G2 , MIDI_G2  }, // $4E
{ HZ_G2 , "G2" , NOTE_G2 , MIDI_G2  }, // $4F
{ HZ_G2 , "g2" , NOTE_G2 , MIDI_G2  }, // $50
{ HZ_G2 , "g2" , NOTE_G2 , MIDI_G2  }, // $51
{ HZ_G2 , "g2" , NOTE_G2 , MIDI_G2  }, // $52 American Football expects this
{ HZ_FS2, "f#2", NOTE_FS2, MIDI_FS2 }, // $53
{ HZ_FS2, "F#2", NOTE_FS2, MIDI_FS2 }, // $54
{ HZ_FS2, "f#2", NOTE_FS2, MIDI_FS2 }, // $55
{ HZ_FS2, "f#2", NOTE_FS2, MIDI_FS2 }, // $56 arguable
{ HZ_F2 , "f2" , NOTE_F2 , MIDI_F2  }, // $57
{ HZ_F2 , "f2" , NOTE_F2 , MIDI_F2  }, // $58
{ HZ_F2 , "F2" , NOTE_F2 , MIDI_F2  }, // $59
{ HZ_F2 , "f2" , NOTE_F2 , MIDI_F2  }, // $5A
{ HZ_F2 , "f2" , NOTE_F2 , MIDI_F2  }, // $5B
{ HZ_E2 , "e2" , NOTE_E2 , MIDI_E2  }, // $5C arguable
{ HZ_E2 , "e2" , NOTE_E2 , MIDI_E2  }, // $5D
{ HZ_E2 , "e2" , NOTE_E2 , MIDI_E2  }, // $5E
{ HZ_E2 , "E2" , NOTE_E2 , MIDI_E2  }, // $5F
{ HZ_E2 , "e2" , NOTE_E2 , MIDI_E2  }, // $60
{ HZ_E2 , "e2" , NOTE_E2 , MIDI_E2  }, // $61
{ HZ_DS2, "d#2", NOTE_DS2, MIDI_DS2 }, // $62
{ HZ_DS2, "d#2", NOTE_DS2, MIDI_DS2 }, // $63
{ HZ_DS2, "D#2", NOTE_DS2, MIDI_DS2 }, // $64
{ HZ_DS2, "d#2", NOTE_DS2, MIDI_DS2 }, // $65
{ HZ_DS2, "d#2", NOTE_DS2, MIDI_DS2 }, // $66
{ HZ_D2 , "d2" , NOTE_D2 , MIDI_D2  }, // $67
{ HZ_D2 , "d2" , NOTE_D2 , MIDI_D2  }, // $68
{ HZ_D2 , "d2" , NOTE_D2 , MIDI_D2  }, // $69
{ HZ_D2 , "d2" , NOTE_D2 , MIDI_D2  }, // $6A
{ HZ_D2 , "D2" , NOTE_D2 , MIDI_D2  }, // $6B
{ HZ_D2 , "d2" , NOTE_D2 , MIDI_D2  }, // $6C
{ HZ_D2 , "d2" , NOTE_D2 , MIDI_D2  }, // $6D
{ HZ_CS2, "c#2", NOTE_CS2, MIDI_CS2 }, // $6E
{ HZ_CS2, "c#2", NOTE_CS2, MIDI_CS2 }, // $6F
{ HZ_CS2, "C#2", NOTE_CS2, MIDI_CS2 }, // $70
{ HZ_CS2, "c#2", NOTE_CS2, MIDI_CS2 }, // $71
{ HZ_CS2, "c#2", NOTE_CS2, MIDI_CS2 }, // $72
{ HZ_CS2, "c#2", NOTE_CS2, MIDI_CS2 }, // $73
{ HZ_C2 , "c2" , NOTE_C2 , MIDI_C2  }, // $74
{ HZ_C2 , "c2" , NOTE_C2 , MIDI_C2  }, // $75
{ HZ_C2 , "c2" , NOTE_C2 , MIDI_C2  }, // $76
{ HZ_C2 , "C2" , NOTE_C2 , MIDI_C2  }, // $77
{ HZ_C2 , "c2" , NOTE_C2 , MIDI_C2  }, // $78
{ HZ_C2 , "c2" , NOTE_C2 , MIDI_C2  }, // $79
{ HZ_C2 , "c2" , NOTE_C2 , MIDI_C2  }, // $7A
{ HZ_B1 , "b1" , NOTE_B1 , MIDI_B1  }, // $7B
{ HZ_B1 , "b1" , NOTE_B1 , MIDI_B1  }, // $7C
{ HZ_B1 , "b1" , NOTE_B1 , MIDI_B1  }, // $7D
{ HZ_B1 , "B1" , NOTE_B1 , MIDI_B1  }, // $7E
{ HZ_B1 , "b1" , NOTE_B1 , MIDI_B1  }, // $7F
{ HZ_B1 , "b1" , NOTE_B1 , MIDI_B1  }, // $80
{ HZ_AS1, "a#1", NOTE_AS1, MIDI_AS1 }, // $81 arguable
{ HZ_AS1, "a#1", NOTE_AS1, MIDI_AS1 }, // $82
{ HZ_AS1, "a#1", NOTE_AS1, MIDI_AS1 }, // $83
{ HZ_AS1, "a#1", NOTE_AS1, MIDI_AS1 }, // $84
{ HZ_AS1, "a#1", NOTE_AS1, MIDI_AS1 }, // $85
{ HZ_AS1, "A#1", NOTE_AS1, MIDI_AS1 }, // $86
{ HZ_AS1, "a#1", NOTE_AS1, MIDI_AS1 }, // $87
{ HZ_AS1, "a#1", NOTE_AS1, MIDI_AS1 }, // $88
{ HZ_AS1, "a#1", NOTE_AS1, MIDI_AS1 }, // $89
{ HZ_A1 , "a1" , NOTE_A1 , MIDI_A1  }, // $8A arguable
{ HZ_A1 , "a1" , NOTE_A1 , MIDI_A1  }, // $8B
{ HZ_A1 , "a1" , NOTE_A1 , MIDI_A1  }, // $8C
{ HZ_A1 , "a1" , NOTE_A1 , MIDI_A1  }, // $8D
{ HZ_A1 , "A1" , NOTE_A1 , MIDI_A1  }, // $8E
{ HZ_A1 , "a1" , NOTE_A1 , MIDI_A1  }, // $8F
{ HZ_A1 , "a1" , NOTE_A1 , MIDI_A1  }, // $90
{ HZ_A1 , "a1" , NOTE_A1 , MIDI_A1  }, // $91
{ HZ_A1 , "a1" , NOTE_A1 , MIDI_A1  }, // $92
{ HZ_GS1, "g#1", NOTE_GS1, MIDI_GS1 }, // $93
{ HZ_GS1, "g#1", NOTE_GS1, MIDI_GS1 }, // $94
{ HZ_GS1, "g#1", NOTE_GS1, MIDI_GS1 }, // $95
{ HZ_GS1, "g#1", NOTE_GS1, MIDI_GS1 }, // $96
{ HZ_GS1, "G#1", NOTE_GS1, MIDI_GS1 }, // $97
{ HZ_GS1, "g#1", NOTE_GS1, MIDI_GS1 }, // $98
{ HZ_GS1, "g#1", NOTE_GS1, MIDI_GS1 }, // $99
{ HZ_GS1, "g#1", NOTE_GS1, MIDI_GS1 }, // $9A
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $9B arguable
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $9C
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $9D
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $9E
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $9F
{ HZ_G1 , "G1" , NOTE_G1 , MIDI_G1  }, // $A0
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $A1
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $A2
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $A3
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $A4 arguable
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $A5
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $A6
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $A7
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $A8
{ HZ_FS1, "F#1", NOTE_FS1, MIDI_FS1 }, // $A9
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $AA
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $AB
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $AC
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $AD
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $AE arguable
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $AF
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $B0
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $B1
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $B2
{ HZ_F1 , "F1" , NOTE_F1 , MIDI_F1  }, // $B3
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $B4
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $B5
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $B6
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $B7
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $B8
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $B9
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $BA
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $BB
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $BC
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $BD
{ HZ_E1 , "E1" , NOTE_E1 , MIDI_E1  }, // $BE
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $BF
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $C0
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $C1
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $C2
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $C3
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $C4
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $C5
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $C6
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $C7
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $C8
{ HZ_DS1, "D#1", NOTE_DS1, MIDI_DS1 }, // $C9
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $CA
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $CB
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $CC
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $CD
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $CE
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $CF
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D0
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D1
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D2
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D3
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D4
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D5
{ HZ_D1 , "D1" , NOTE_D1 , MIDI_D1  }, // $D6
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D7
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D8
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D9
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $DA
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $DB
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $DC arguable
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $DD
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $DE
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $DF
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $E0
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $E1
{ HZ_CS1, "C#1", NOTE_CS1, MIDI_CS1 }, // $E2
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $E3
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $E4
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $E5
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $E6
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $E7
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $E8
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $E9 arguable
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $EA
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $EB
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $EC
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $ED
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $EE
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $EF
{ HZ_C1 , "C1" , NOTE_C1 , MIDI_C1  }, // $F0
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $F1
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $F2
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $F3
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $F4
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $F5
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $F6
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $F7 arguable
{ HZ_B0 , "b0" , NOTE_B0 , MIDI_B0  }, // $F8
{ HZ_B0 , "b0" , NOTE_B0 , MIDI_B0  }, // $F9
{ HZ_B0 , "b0" , NOTE_B0 , MIDI_B0  }, // $FA
{ HZ_B0 , "b0" , NOTE_B0 , MIDI_B0  }, // $FB
{ HZ_B0 , "b0" , NOTE_B0 , MIDI_B0  }, // $FC
{ HZ_B0 , "b0" , NOTE_B0 , MIDI_B0  }, // $FD
{ HZ_B0 , "B0" , NOTE_B0 , MIDI_B0  }, // $FE
{ HZ_B0 , "b0" , NOTE_B0 , MIDI_B0  }, // $FF
}, idealfreq_pal[256] = {
{HZ_REST, "rest",NOTE_REST,MIDI_REST}, // $00
{ HZ_B7 , "B7" , NOTE_B7 , MIDI_B7  }, // $01
{ HZ_E7 , "E7" , NOTE_E7 , MIDI_E7  }, // $02
{ HZ_B6 , "B6" , NOTE_B6 , MIDI_B6  }, // $03
{ HZ_G6 , "G6" , NOTE_G6 , MIDI_G6  }, // $04
{ HZ_E6 , "E6" , NOTE_E6 , MIDI_E6  }, // $05
{ HZ_CS6, "C#6", NOTE_CS6, MIDI_CS6 }, // $06
{ HZ_B5 , "B5" , NOTE_B5 , MIDI_B5  }, // $07
{ HZ_A5 , "A5" , NOTE_A5 , MIDI_A5  }, // $08
{ HZ_G5 , "G5" , NOTE_G5 , MIDI_G5  }, // $09
{ HZ_F5 , "F5" , NOTE_F5 , MIDI_F5  }, // $0A
{ HZ_E5 , "E5" , NOTE_E5 , MIDI_E5  }, // $0B
{ HZ_DS5, "D#5", NOTE_DS5, MIDI_DS5 }, // $0C
{ HZ_CS5, "C#5", NOTE_CS5, MIDI_CS5 }, // $0D
{ HZ_C5 , "C5" , NOTE_C5 , MIDI_C5  }, // $0E
{ HZ_B4 , "B4" , NOTE_B4 , MIDI_B4  }, // $0F
{ HZ_AS4, "A#4", NOTE_AS4, MIDI_AS4 }, // $10
{ HZ_A4 , "A4" , NOTE_A4 , MIDI_A4  }, // $11
{ HZ_GS4, "G#4", NOTE_GS4, MIDI_GS4 }, // $12
{ HZ_G4 , "G4" , NOTE_G4 , MIDI_G4  }, // $13
{ HZ_FS4, "F#4", NOTE_FS4, MIDI_FS4 }, // $14
{ HZ_F4 , "F4" , NOTE_F4 , MIDI_F4  }, // $15
{ HZ_E4 , "e4" , NOTE_E4 , MIDI_E4  }, // $16 borderline
{ HZ_E4 , "E4" , NOTE_E4 , MIDI_E4  }, // $17
{ HZ_DS4, "D#4", NOTE_DS4, MIDI_DS4 }, // $18 e4 for Interton/Elektor
{ HZ_D4 , "d4" , NOTE_D4 , MIDI_D4  }, // $19 Super Bug expects this. d#4 for Interton/Elektor
{ HZ_D4 , "D4" , NOTE_D4 , MIDI_D4  }, // $1A
{ HZ_CS4, "C#4", NOTE_CS4, MIDI_CS4 }, // $1B d4 for Interton/Elektor
{ HZ_CS4, "c#4", NOTE_CS4, MIDI_CS4 }, // $1C Alien Invaders expects this
{ HZ_C4 , "C4" , NOTE_C4 , MIDI_C4  }, // $1D middle C
{ HZ_B3 , "b3" , NOTE_B3 , MIDI_B3  }, // $1E borderline
{ HZ_B3 , "B3" , NOTE_B3 , MIDI_B3  }, // $1F
{ HZ_AS3, "a#3", NOTE_AS3, MIDI_AS3 }, // $20 borderline
{ HZ_AS3, "A#3", NOTE_AS3, MIDI_AS3 }, // $21
{ HZ_A3 , "a3" , NOTE_A3 , MIDI_A3  }, // $22 borderline
{ HZ_A3 , "A3" , NOTE_A3 , MIDI_A3  }, // $23
{ HZ_GS3, "g#3", NOTE_GS3, MIDI_GS3 }, // $24 borderline
{ HZ_GS3, "G#3", NOTE_GS3, MIDI_GS3 }, // $25
{ HZ_GS3, "g#3", NOTE_GS3, MIDI_GS3 }, // $26 borderline
{ HZ_G3 , "G3" , NOTE_G3 , MIDI_G3  }, // $27
{ HZ_G3 , "g3" , NOTE_G3 , MIDI_G3  }, // $28 American Football expects this. f#3 for Interton/Elektor
{ HZ_FS3, "F#3", NOTE_FS3, MIDI_FS3 }, // $29
{ HZ_FS3, "f#3", NOTE_FS3, MIDI_FS3 }, // $2A
{ HZ_F3 , "f3" , NOTE_F3 , MIDI_F3  }, // $2B
{ HZ_F3 , "F3" , NOTE_F3 , MIDI_F3  }, // $2C
{ HZ_F3 , "f3" , NOTE_F3 , MIDI_F3  }, // $2D borderline
{ HZ_E3 , "E3" , NOTE_E3 , MIDI_E3  }, // $2E
{ HZ_E3 , "e3" , NOTE_E3 , MIDI_E3  }, // $2F
{ HZ_DS3, "d#3", NOTE_DS3, MIDI_DS3 }, // $30
{ HZ_DS3, "D#3", NOTE_DS3, MIDI_DS3 }, // $31
{ HZ_DS3, "d#3", NOTE_DS3, MIDI_DS3 }, // $32
{ HZ_D3 , "d3" , NOTE_D3 , MIDI_D3  }, // $33
{ HZ_D3 , "D3" , NOTE_D3 , MIDI_D3  }, // $34
{ HZ_D3 , "d3" , NOTE_D3 , MIDI_D3  }, // $35
{ HZ_CS3, "c#3", NOTE_CS3, MIDI_CS3 }, // $36
{ HZ_CS3, "C#3", NOTE_CS3, MIDI_CS3 }, // $37
{ HZ_CS3, "c#3", NOTE_CS3, MIDI_CS3 }, // $38
{ HZ_CS3, "c#3", NOTE_CS3, MIDI_CS3 }, // $39 borderline
{ HZ_C3 , "c3" , NOTE_C3 , MIDI_C3  }, // $3A
{ HZ_C3 , "C3" , NOTE_C3 , MIDI_C3  }, // $3B
{ HZ_C3 , "c3" , NOTE_C3 , MIDI_C3  }, // $3C
{ HZ_C3 , "c3" , NOTE_C3 , MIDI_C3  }, // $3D borderline
{ HZ_B2 , "b2" , NOTE_B2 , MIDI_B2  }, // $3E
{ HZ_B2 , "B2" , NOTE_B2 , MIDI_B2  }, // $3F
{ HZ_B2 , "b2" , NOTE_B2 , MIDI_B2  }, // $40
{ HZ_AS2, "a#2", NOTE_AS2, MIDI_AS2 }, // $41
{ HZ_AS2, "A#2", NOTE_AS2, MIDI_AS2 }, // $42
{ HZ_AS2, "a#2", NOTE_AS2, MIDI_AS2 }, // $43
{ HZ_AS2, "a#2", NOTE_AS2, MIDI_AS2 }, // $44 American Football expects this
{ HZ_A2 , "a2" , NOTE_A2 , MIDI_A2  }, // $45
{ HZ_A2 , "A2" , NOTE_A2 , MIDI_A2  }, // $46
{ HZ_A2 , "a2" , NOTE_A2 , MIDI_A2  }, // $47
{ HZ_A2 , "a2" , NOTE_A2 , MIDI_A2  }, // $48 American Football expects this
{ HZ_GS2, "g#2", NOTE_GS2, MIDI_GS2 }, // $49
{ HZ_GS2, "G#2", NOTE_GS2, MIDI_GS2 }, // $4A
{ HZ_GS2, "g#2", NOTE_GS2, MIDI_GS2 }, // $4B
{ HZ_GS2, "g#2", NOTE_GS2, MIDI_GS2 }, // $4C
{ HZ_G2 , "g2" , NOTE_G2 , MIDI_G2  }, // $4D
{ HZ_G2 , "g2" , NOTE_G2 , MIDI_G2  }, // $4E
{ HZ_G2 , "G2" , NOTE_G2 , MIDI_G2  }, // $4F
{ HZ_G2 , "g2" , NOTE_G2 , MIDI_G2  }, // $50
{ HZ_G2 , "g2" , NOTE_G2 , MIDI_G2  }, // $51
{ HZ_G2 , "g2" , NOTE_G2 , MIDI_G2  }, // $52 American Football expects this
{ HZ_FS2, "F#2", NOTE_FS2, MIDI_FS2 }, // $53
{ HZ_FS2, "f#2", NOTE_FS2, MIDI_FS2 }, // $54
{ HZ_FS2, "f#2", NOTE_FS2, MIDI_FS2 }, // $55
{ HZ_F2 , "f2" , NOTE_F2 , MIDI_F2  }, // $56
{ HZ_F2 , "f2" , NOTE_F2 , MIDI_F2  }, // $57
{ HZ_F2 , "F2" , NOTE_F2 , MIDI_F2  }, // $58
{ HZ_F2 , "f2" , NOTE_F2 , MIDI_F2  }, // $59
{ HZ_F2 , "f2" , NOTE_F2 , MIDI_F2  }, // $5A
{ HZ_F2 , "f2" , NOTE_F2 , MIDI_F2  }, // $5B borderline
{ HZ_E2 , "e2" , NOTE_E2 , MIDI_E2  }, // $5C
{ HZ_E2 , "e2" , NOTE_E2 , MIDI_E2  }, // $5D
{ HZ_E2 , "E2" , NOTE_E2 , MIDI_E2  }, // $5E
{ HZ_E2 , "e2" , NOTE_E2 , MIDI_E2  }, // $5F
{ HZ_E2 , "e2" , NOTE_E2 , MIDI_E2  }, // $60
{ HZ_E2 , "e2" , NOTE_E2 , MIDI_E2  }, // $61 borderline
{ HZ_DS2, "d#2", NOTE_DS2, MIDI_DS2 }, // $62
{ HZ_DS2, "d#2", NOTE_DS2, MIDI_DS2 }, // $63
{ HZ_DS2, "D#2", NOTE_DS2, MIDI_DS2 }, // $64
{ HZ_DS2, "d#2", NOTE_DS2, MIDI_DS2 }, // $65
{ HZ_DS2, "d#2", NOTE_DS2, MIDI_DS2 }, // $66
{ HZ_D2 , "d2" , NOTE_D2 , MIDI_D2  }, // $67
{ HZ_D2 , "d2" , NOTE_D2 , MIDI_D2  }, // $68
{ HZ_D2 , "D2" , NOTE_D2 , MIDI_D2  }, // $69
{ HZ_D2 , "d2" , NOTE_D2 , MIDI_D2  }, // $6A
{ HZ_D2 , "d2" , NOTE_D2 , MIDI_D2  }, // $6B
{ HZ_D2 , "d2" , NOTE_D2 , MIDI_D2  }, // $6C
{ HZ_CS2, "c#2", NOTE_CS2, MIDI_CS2 }, // $6D
{ HZ_CS2, "c#2", NOTE_CS2, MIDI_CS2 }, // $6E
{ HZ_CS2, "c#2", NOTE_CS2, MIDI_CS2 }, // $6F
{ HZ_CS2, "C#2", NOTE_CS2, MIDI_CS2 }, // $70
{ HZ_CS2, "c#2", NOTE_CS2, MIDI_CS2 }, // $71
{ HZ_CS2, "c#2", NOTE_CS2, MIDI_CS2 }, // $72
{ HZ_C2 , "c2" , NOTE_C2 , MIDI_C2  }, // $73 borderline
{ HZ_C2 , "c2" , NOTE_C2 , MIDI_C2  }, // $74
{ HZ_C2 , "c2" , NOTE_C2 , MIDI_C2  }, // $75
{ HZ_C2 , "C2" , NOTE_C2 , MIDI_C2  }, // $76
{ HZ_C2 , "c2" , NOTE_C2 , MIDI_C2  }, // $77
{ HZ_C2 , "c2" , NOTE_C2 , MIDI_C2  }, // $78
{ HZ_C2 , "c2" , NOTE_C2 , MIDI_C2  }, // $79
{ HZ_C2 , "c2" , NOTE_C2 , MIDI_C2  }, // $7A borderline
{ HZ_B1 , "b1" , NOTE_B1 , MIDI_B1  }, // $7B
{ HZ_B1 , "b1" , NOTE_B1 , MIDI_B1  }, // $7C
{ HZ_B1 , "b1" , NOTE_B1 , MIDI_B1  }, // $7D
{ HZ_B1 , "B1" , NOTE_B1 , MIDI_B1  }, // $7E
{ HZ_B1 , "b1" , NOTE_B1 , MIDI_B1  }, // $7F
{ HZ_B1 , "b1" , NOTE_B1 , MIDI_B1  }, // $80
{ HZ_B1 , "b1" , NOTE_B1 , MIDI_B1  }, // $81
{ HZ_AS1, "a#1", NOTE_AS1, MIDI_AS1 }, // $82
{ HZ_AS1, "a#1", NOTE_AS1, MIDI_AS1 }, // $83
{ HZ_AS1, "a#1", NOTE_AS1, MIDI_AS1 }, // $84
{ HZ_AS1, "A#1", NOTE_AS1, MIDI_AS1 }, // $85
{ HZ_AS1, "a#1", NOTE_AS1, MIDI_AS1 }, // $86
{ HZ_AS1, "a#1", NOTE_AS1, MIDI_AS1 }, // $87
{ HZ_AS1, "a#1", NOTE_AS1, MIDI_AS1 }, // $88
{ HZ_A1 , "a1" , NOTE_A1 , MIDI_A1  }, // $89 borderline
{ HZ_A1 , "a1" , NOTE_A1 , MIDI_A1  }, // $8A
{ HZ_A1 , "a1" , NOTE_A1 , MIDI_A1  }, // $8B
{ HZ_A1 , "a1" , NOTE_A1 , MIDI_A1  }, // $8C
{ HZ_A1 , "A1" , NOTE_A1 , MIDI_A1  }, // $8D
{ HZ_A1 , "a1" , NOTE_A1 , MIDI_A1  }, // $8E
{ HZ_A1 , "a1" , NOTE_A1 , MIDI_A1  }, // $8F
{ HZ_A1 , "a1" , NOTE_A1 , MIDI_A1  }, // $90
{ HZ_GS1, "g#1", NOTE_GS1, MIDI_GS1 }, // $91 borderline
{ HZ_GS1, "g#1", NOTE_GS1, MIDI_GS1 }, // $92
{ HZ_GS1, "g#1", NOTE_GS1, MIDI_GS1 }, // $93
{ HZ_GS1, "g#1", NOTE_GS1, MIDI_GS1 }, // $94
{ HZ_GS1, "G#1", NOTE_GS1, MIDI_GS1 }, // $95
{ HZ_GS1, "g#1", NOTE_GS1, MIDI_GS1 }, // $96
{ HZ_GS1, "g#1", NOTE_GS1, MIDI_GS1 }, // $97
{ HZ_GS1, "g#1", NOTE_GS1, MIDI_GS1 }, // $98
{ HZ_GS1, "g#1", NOTE_GS1, MIDI_GS1 }, // $99
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $9A
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $9B
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $9C
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $9D
{ HZ_G1 , "G1" , NOTE_G1 , MIDI_G1  }, // $9E
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $9F
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $A0
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $A1
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $A2
{ HZ_G1 , "g1" , NOTE_G1 , MIDI_G1  }, // $A3 borderline
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $A4
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $A5
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $A6
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $A7
{ HZ_FS1, "F#1", NOTE_FS1, MIDI_FS1 }, // $A8
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $A9
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $AA
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $AB
{ HZ_FS1, "f#1", NOTE_FS1, MIDI_FS1 }, // $AC
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $AD borderline
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $AE
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $AF
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $B0
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $B1
{ HZ_F1 , "F1" , NOTE_F1 , MIDI_F1  }, // $B2
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $B3
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $B4
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $B5
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $B6
{ HZ_F1 , "f1" , NOTE_F1 , MIDI_F1  }, // $B7
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $B8
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $B9
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $BA
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $BB
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $BC
{ HZ_E1 , "E1" , NOTE_E1 , MIDI_E1  }, // $BD
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $BE
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $BF
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $C0
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $C1
{ HZ_E1 , "e1" , NOTE_E1 , MIDI_E1  }, // $C2
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $C3
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $C4
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $C5
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $C6
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $C7
{ HZ_DS1, "D#1", NOTE_DS1, MIDI_DS1 }, // $C8
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $C9
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $CA
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $CB
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $CC
{ HZ_DS1, "d#1", NOTE_DS1, MIDI_DS1 }, // $CD
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $CE borderline
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $CF
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D0
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D1
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D2
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D3
{ HZ_D1 , "D1" , NOTE_D1 , MIDI_D1  }, // $D4
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D5
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D6
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D7
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D8
{ HZ_D1 , "d1" , NOTE_D1 , MIDI_D1  }, // $D9
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $DA borderline
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $DB
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $DB
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $DC
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $DE
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $DF
{ HZ_CS1, "C#1", NOTE_CS1, MIDI_CS1 }, // $E0
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $E1
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $E2
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $E3
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $E4
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $E5
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $E6
{ HZ_CS1, "c#1", NOTE_CS1, MIDI_CS1 }, // $E7 borderline
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $E8
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $E9
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $EA
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $EB
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $EC
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $ED
{ HZ_C1 , "C1" , NOTE_C1 , MIDI_C1  }, // $EE
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $EF
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $F0
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $F1
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $F2
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $F3
{ HZ_C1 , "c1" , NOTE_C1 , MIDI_C1  }, // $F4
{ HZ_B0 , "b0" , NOTE_B0 , NOTE_B0  }, // $F5 borderline
{ HZ_B0 , "b0" , NOTE_B0 , MIDI_B0  }, // $F6
{ HZ_B0 , "b0" , NOTE_B0 , MIDI_B0  }, // $F7
{ HZ_B0 , "b0" , NOTE_B0 , MIDI_B0  }, // $F8
{ HZ_B0 , "b0" , NOTE_B0 , MIDI_B0  }, // $F9
{ HZ_B0 , "b0" , NOTE_B0 , MIDI_B0  }, // $FA
{ HZ_B0 , "b0" , NOTE_B0 , MIDI_B0  }, // $FB
{ HZ_B0 , "B0" , NOTE_B0 , MIDI_B0  }, // $FC
{ HZ_B0 , "b0" , NOTE_B0 , MIDI_B0  }, // $FD
{ HZ_B0 , "b0" , NOTE_B0 , MIDI_B0  }, // $FE
{ HZ_B0 , "b0" , NOTE_B0 , MIDI_B0  }, // $FF
};

MODULE const ULONG crc64_table_h[256] =
{
0x00000000,0x42F0E1EB,0x85E1C3D7,0xC711223C,
0x49336645,0x0BC387AE,0xCCD2A592,0x8E224479,
0x9266CC8A,0xD0962D61,0x17870F5D,0x5577EEB6,
0xDB55AACF,0x99A54B24,0x5EB46918,0x1C4488F3,
0x663D78FF,0x24CD9914,0xE3DCBB28,0xA12C5AC3,
0x2F0E1EBA,0x6DFEFF51,0xAAEFDD6D,0xE81F3C86,
0xF45BB475,0xB6AB559E,0x71BA77A2,0x334A9649,
0xBD68D230,0xFF9833DB,0x388911E7,0x7A79F00C,
0xCC7AF1FF,0x8E8A1014,0x499B3228,0x0B6BD3C3,
0x854997BA,0xC7B97651,0x00A8546D,0x4258B586,
0x5E1C3D75,0x1CECDC9E,0xDBFDFEA2,0x990D1F49,
0x172F5B30,0x55DFBADB,0x92CE98E7,0xD03E790C,
0xAA478900,0xE8B768EB,0x2FA64AD7,0x6D56AB3C,
0xE374EF45,0xA1840EAE,0x66952C92,0x2465CD79,
0x3821458A,0x7AD1A461,0xBDC0865D,0xFF3067B6,
0x711223CF,0x33E2C224,0xF4F3E018,0xB60301F3,
0xDA050215,0x98F5E3FE,0x5FE4C1C2,0x1D142029,
0x93366450,0xD1C685BB,0x16D7A787,0x5427466C,
0x4863CE9F,0x0A932F74,0xCD820D48,0x8F72ECA3,
0x0150A8DA,0x43A04931,0x84B16B0D,0xC6418AE6,
0xBC387AEA,0xFEC89B01,0x39D9B93D,0x7B2958D6,
0xF50B1CAF,0xB7FBFD44,0x70EADF78,0x321A3E93,
0x2E5EB660,0x6CAE578B,0xABBF75B7,0xE94F945C,
0x676DD025,0x259D31CE,0xE28C13F2,0xA07CF219,
0x167FF3EA,0x548F1201,0x939E303D,0xD16ED1D6,
0x5F4C95AF,0x1DBC7444,0xDAAD5678,0x985DB793,
0x84193F60,0xC6E9DE8B,0x01F8FCB7,0x43081D5C,
0xCD2A5925,0x8FDAB8CE,0x48CB9AF2,0x0A3B7B19,
0x70428B15,0x32B26AFE,0xF5A348C2,0xB753A929,
0x3971ED50,0x7B810CBB,0xBC902E87,0xFE60CF6C,
0xE224479F,0xA0D4A674,0x67C58448,0x253565A3,
0xAB1721DA,0xE9E7C031,0x2EF6E20D,0x6C0603E6,
0xF6FAE5C0,0xB40A042B,0x731B2617,0x31EBC7FC,
0xBFC98385,0xFD39626E,0x3A284052,0x78D8A1B9,
0x649C294A,0x266CC8A1,0xE17DEA9D,0xA38D0B76,
0x2DAF4F0F,0x6F5FAEE4,0xA84E8CD8,0xEABE6D33,
0x90C79D3F,0xD2377CD4,0x15265EE8,0x57D6BF03,
0xD9F4FB7A,0x9B041A91,0x5C1538AD,0x1EE5D946,
0x02A151B5,0x4051B05E,0x87409262,0xC5B07389,
0x4B9237F0,0x0962D61B,0xCE73F427,0x8C8315CC,
0x3A80143F,0x7870F5D4,0xBF61D7E8,0xFD913603,
0x73B3727A,0x31439391,0xF652B1AD,0xB4A25046,
0xA8E6D8B5,0xEA16395E,0x2D071B62,0x6FF7FA89,
0xE1D5BEF0,0xA3255F1B,0x64347D27,0x26C49CCC,
0x5CBD6CC0,0x1E4D8D2B,0xD95CAF17,0x9BAC4EFC,
0x158E0A85,0x577EEB6E,0x906FC952,0xD29F28B9,
0xCEDBA04A,0x8C2B41A1,0x4B3A639D,0x09CA8276,
0x87E8C60F,0xC51827E4,0x020905D8,0x40F9E433,
0x2CFFE7D5,0x6E0F063E,0xA91E2402,0xEBEEC5E9,
0x65CC8190,0x273C607B,0xE02D4247,0xA2DDA3AC,
0xBE992B5F,0xFC69CAB4,0x3B78E888,0x79880963,
0xF7AA4D1A,0xB55AACF1,0x724B8ECD,0x30BB6F26,
0x4AC29F2A,0x08327EC1,0xCF235CFD,0x8DD3BD16,
0x03F1F96F,0x41011884,0x86103AB8,0xC4E0DB53,
0xD8A453A0,0x9A54B24B,0x5D459077,0x1FB5719C,
0x919735E5,0xD367D40E,0x1476F632,0x568617D9,
0xE085162A,0xA275F7C1,0x6564D5FD,0x27943416,
0xA9B6706F,0xEB469184,0x2C57B3B8,0x6EA75253,
0x72E3DAA0,0x30133B4B,0xF7021977,0xB5F2F89C,
0x3BD0BCE5,0x79205D0E,0xBE317F32,0xFCC19ED9,
0x86B86ED5,0xC4488F3E,0x0359AD02,0x41A94CE9,
0xCF8B0890,0x8D7BE97B,0x4A6ACB47,0x089A2AAC,
0x14DEA25F,0x562E43B4,0x913F6188,0xD3CF8063,
0x5DEDC41A,0x1F1D25F1,0xD80C07CD,0x9AFCE626
}, crc64_table_l[256] = {
0x00000000,0xA9EA3693,0x53D46D26,0xFA3E5BB5,
0x0E42ECDF,0xA7A8DA4C,0x5D9681F9,0xF47CB76A,
0x1C85D9BE,0xB56FEF2D,0x4F51B498,0xE6BB820B,
0x12C73561,0xBB2D03F2,0x41135847,0xE8F96ED4,
0x90E185EF,0x390BB37C,0xC335E8C9,0x6ADFDE5A,
0x9EA36930,0x37495FA3,0xCD770416,0x649D3285,
0x8C645C51,0x258E6AC2,0xDFB03177,0x765A07E4,
0x8226B08E,0x2BCC861D,0xD1F2DDA8,0x7818EB3B,
0x21C30BDE,0x88293D4D,0x721766F8,0xDBFD506B,
0x2F81E701,0x866BD192,0x7C558A27,0xD5BFBCB4,
0x3D46D260,0x94ACE4F3,0x6E92BF46,0xC77889D5,
0x33043EBF,0x9AEE082C,0x60D05399,0xC93A650A,
0xB1228E31,0x18C8B8A2,0xE2F6E317,0x4B1CD584,
0xBF6062EE,0x168A547D,0xECB40FC8,0x455E395B,
0xADA7578F,0x044D611C,0xFE733AA9,0x57990C3A,
0xA3E5BB50,0x0A0F8DC3,0xF031D676,0x59DBE0E5,
0xEA6C212F,0x438617BC,0xB9B84C09,0x10527A9A,
0xE42ECDF0,0x4DC4FB63,0xB7FAA0D6,0x1E109645,
0xF6E9F891,0x5F03CE02,0xA53D95B7,0x0CD7A324,
0xF8AB144E,0x514122DD,0xAB7F7968,0x02954FFB,
0x7A8DA4C0,0xD3679253,0x2959C9E6,0x80B3FF75,
0x74CF481F,0xDD257E8C,0x271B2539,0x8EF113AA,
0x66087D7E,0xCFE24BED,0x35DC1058,0x9C3626CB,
0x684A91A1,0xC1A0A732,0x3B9EFC87,0x9274CA14,
0xCBAF2AF1,0x62451C62,0x987B47D7,0x31917144,
0xC5EDC62E,0x6C07F0BD,0x9639AB08,0x3FD39D9B,
0xD72AF34F,0x7EC0C5DC,0x84FE9E69,0x2D14A8FA,
0xD9681F90,0x70822903,0x8ABC72B6,0x23564425,
0x5B4EAF1E,0xF2A4998D,0x089AC238,0xA170F4AB,
0x550C43C1,0xFCE67552,0x06D82EE7,0xAF321874,
0x47CB76A0,0xEE214033,0x141F1B86,0xBDF52D15,
0x49899A7F,0xE063ACEC,0x1A5DF759,0xB3B7C1CA,
0x7D3274CD,0xD4D8425E,0x2EE619EB,0x870C2F78,
0x73709812,0xDA9AAE81,0x20A4F534,0x894EC3A7,
0x61B7AD73,0xC85D9BE0,0x3263C055,0x9B89F6C6,
0x6FF541AC,0xC61F773F,0x3C212C8A,0x95CB1A19,
0xEDD3F122,0x4439C7B1,0xBE079C04,0x17EDAA97,
0xE3911DFD,0x4A7B2B6E,0xB04570DB,0x19AF4648,
0xF156289C,0x58BC1E0F,0xA28245BA,0x0B687329,
0xFF14C443,0x56FEF2D0,0xACC0A965,0x052A9FF6,
0x5CF17F13,0xF51B4980,0x0F251235,0xA6CF24A6,
0x52B393CC,0xFB59A55F,0x0167FEEA,0xA88DC879,
0x4074A6AD,0xE99E903E,0x13A0CB8B,0xBA4AFD18,
0x4E364A72,0xE7DC7CE1,0x1DE22754,0xB40811C7,
0xCC10FAFC,0x65FACC6F,0x9FC497DA,0x362EA149,
0xC2521623,0x6BB820B0,0x91867B05,0x386C4D96,
0xD0952342,0x797F15D1,0x83414E64,0x2AAB78F7,
0xDED7CF9D,0x773DF90E,0x8D03A2BB,0x24E99428,
0x975E55E2,0x3EB46371,0xC48A38C4,0x6D600E57,
0x991CB93D,0x30F68FAE,0xCAC8D41B,0x6322E288,
0x8BDB8C5C,0x2231BACF,0xD80FE17A,0x71E5D7E9,
0x85996083,0x2C735610,0xD64D0DA5,0x7FA73B36,
0x07BFD00D,0xAE55E69E,0x546BBD2B,0xFD818BB8,
0x09FD3CD2,0xA0170A41,0x5A2951F4,0xF3C36767,
0x1B3A09B3,0xB2D03F20,0x48EE6495,0xE1045206,
0x1578E56C,0xBC92D3FF,0x46AC884A,0xEF46BED9,
0xB69D5E3C,0x1F7768AF,0xE549331A,0x4CA30589,
0xB8DFB2E3,0x11358470,0xEB0BDFC5,0x42E1E956,
0xAA188782,0x03F2B111,0xF9CCEAA4,0x5026DC37,
0xA45A6B5D,0x0DB05DCE,0xF78E067B,0x5E6430E8,
0x267CDBD3,0x8F96ED40,0x75A8B6F5,0xDC428066,
0x283E370C,0x81D4019F,0x7BEA5A2A,0xD2006CB9,
0x3AF9026D,0x931334FE,0x692D6F4B,0xC0C759D8,
0x34BBEEB2,0x9D51D821,0x676F8394,0xCE85B507
};

EXPORT const STRPTR coveragename[32] =
{   "-----",
    "----W",
    "---R-",
    "---RW",
    "--O--",
    "--O-W",
    "--OR-",
    "--ORW",
    "-E---",
    "-E--W",
    "-E-R-",
    "-E-RW",
    "-EO--",
    "-EO-W",
    "-EOR-",
    "-EORW",
    "A----",
    "A---W",
    "A--R-",
    "A--RW",
    "A-O--",
    "A-O-W",
    "A-OR-",
    "A-ORW",
    "AE---",
    "AE--W",
    "AE-R-",
    "AE-RW",
    "AEO--",
    "AEO-W",
    "AEOR-",
    "AEORW",
};

EXPORT const struct KeyInfoStruct keyinfo[KEYINFOS][KEYS] = {
{ // 0 (small Emerson)
{ (360-11)/2, (236-11)/2, 24/2, 24/2, -1, -1, -1 }, // START
{ (294-11)/2, (235-11)/2, 24/2, 24/2, -1, -1, -1 }, // A
{ (327-11)/2, (236-11)/2, 24/2, 23/2, -1, -1, -1 }, // B
{ (261-11)/2, (236-11)/2, 23/2, 23/2, -1, -1, -1 }, // RESET
{ (132-11)/2, (107-11)/2, 25/2, 22/2,  8,  1,  0 }, // '1'
{ (125-11)/2, (133-11)/2, 25/2, 23/2, 14,  4,  0 }, // '4'
{ (119-11)/2, (160-11)/2, 24/2, 23/2, 20,  7,  0 }, // '7'
{ (112-11)/2, (188-11)/2, 25/2, 23/2, 26, 10,  0 }, // 'Cl'
{ (160-11)/2, (106-11)/2, 25/2, 22/2,  0,  2,  0 }, // '2'
{ (154-11)/2, (132-11)/2, 25/2, 22/2, 15,  5,  0 }, // '5'
{ (148-11)/2, (160-11)/2, 25/2, 23/2, 21,  8,  0 }, // '8'
{ (141-11)/2, (187-11)/2, 26/2, 24/2, 27, 11,  0 }, // '0'
{ (190-11)/2, (106-11)/2, 25/2, 22/2, 10,  3,  0 }, // '3'
{ (184-11)/2, (133-11)/2, 25/2, 22/2, 16,  6,  0 }, // '6'
{ (178-11)/2, (160-11)/2, 25/2, 23/2, 22,  9,  0 }, // '9'
{ (171-11)/2, (187-11)/2, 27/2, 24/2, 28, 12,  0 }, // 'En'
{ (532-11)/2, (117-11)/2, 27/2, 23/2, 11,  1,  1 }, // '1'
{ (529-11)/2, (145-11)/2, 27/2, 23/2, 17,  4,  1 }, // '4'
{ (526-11)/2, (174-11)/2, 27/2, 25/2, 23,  7,  1 }, // '7'
{ (523-11)/2, (204-11)/2, 28/2, 25/2, 29, 10,  1 }, // 'Cl'
{ (565-11)/2, (117-11)/2, 27/2, 23/2,  1,  2,  1 }, // '2'
{ (562-11)/2, (145-11)/2, 28/2, 23/2, 18,  5,  1 }, // '5'
{ (559-11)/2, (174-11)/2, 29/2, 25/2, 24,  8,  1 }, // '8'
{ (557-11)/2, (204-11)/2, 28/2, 25/2, 30, 11,  1 }, // '0'
{ (598-11)/2, (117-11)/2, 28/2, 23/2, 13,  3,  1 }, // '3'
{ (596-11)/2, (145-11)/2, 28/2, 24/2, 19,  6,  1 }, // '6'
{ (594-11)/2, (174-11)/2, 29/2, 26/2, 25,  9,  1 }, // '9'
{ (592-11)/2, (205-11)/2, 29/2, 25/2, 31, 12,  1 }, // 'En'
{ (122-11)/2, (242-11)/2, 20/2, 20/2, -1, 17,  0 }, // 28 p1 up
{ (100-11)/2, (265-11)/2, 20/2, 20/2, -1, 19,  0 }, // 29 p1 left
{ (122-11)/2, (288-11)/2, 20/2, 20/2, -1, 18,  0 }, // 30 p1 down
{ (144-11)/2, (265-11)/2, 20/2, 20/2, -1, 20,  0 }, // 31 p1 right
{ (552-11)/2, (265-11)/2, 21/2, 20/2, -1, 17,  1 }, // 32 p2 up
{ (527-11)/2, (289-11)/2, 21/2, 20/2, -1, 19,  1 }, // 33 p2 left
{ (552-11)/2, (316-11)/2, 21/2, 22/2, -1, 18,  1 }, // 34 p2 down
{ (578-11)/2, (289-11)/2, 23/2, 20/2, -1, 20,  1 }, // 35 p2 right
{      -1,         -1,      -1,   -1,  0,  2,  0 }, // 36 p1 '2'
{      -1,         -1,      -1,   -1,  0,  2,  0 }, // 37 p1 '2'
{      -1,         -1,      -1,   -1,  1,  2,  1 }, // 38 p2 '2'
{      -1,         -1,      -1,   -1,  1,  2,  1 }, // 39 p2 '2'
{      -1,         -1,      -1,   -1,  2, 13,  0 }, // 40 p1 x1
{      -1,         -1,      -1,   -1,  3, 14,  0 }, // 41 p1 x2
{      -1,         -1,      -1,   -1,  4, 15,  0 }, // 42 p1 x3
{      -1,         -1,      -1,   -1,  9, 16,  0 }, // 43 p1 x4
{      -1,         -1,      -1,   -1,  5, 13,  1 }, // 44 p2 x1
{      -1,         -1,      -1,   -1,  6, 14,  1 }, // 45 p2 x2
{      -1,         -1,      -1,   -1,  7, 15,  1 }, // 46 p2 x3
{      -1,         -1,      -1,   -1, 12, 16,  1 }, // 47 p2 x4
{ (263-11)/2, ( 80-11)/2,214/2, 45/2, -1, -1, -1 }, // 48 cartridge slot
{ (395-11)/2, (237-11)/2, 51/2, 26/2, -1, -1, -1 }, // 49 power switch
}, { // 1 (small MPT-03)
{ (410-11)/2, (157-11)/2, 26/2, 14/2, -1, -1, -1 }, // START
{ (338-11)/2, (157-11)/2, 27/2, 15/2, -1, -1, -1 }, // A
{ (374-11)/2, (157-11)/2, 26/2, 14/2, -1, -1, -1 }, // B
{ (324-11)/2, (237-11)/2, 74/2, 20/2, -1, -1, -1 }, // RESET
{ ( 70-11)/2, (200-11)/2, 27/2, 13/2,  8,  1,  0 }, // '1'
{ ( 68-11)/2, (225-11)/2, 27/2, 14/2, 14,  4,  0 }, // '4'
{ ( 66-11)/2, (251-11)/2, 28/2, 14/2, 20,  7,  0 }, // '7'
{ ( 64-11)/2, (278-11)/2, 27/2, 13/2, 26, 10,  0 }, // 'Cl'
{ (114-11)/2, (200-11)/2, 27/2, 13/2,  0,  2,  0 }, // '2'
{ (111-11)/2, (225-11)/2, 28/2, 14/2, 15,  5,  0 }, // '5'
{ (109-11)/2, (251-11)/2, 28/2, 14/2, 21,  8,  0 }, // '8'
{ (108-11)/2, (277-11)/2, 27/2, 14/2, 27, 11,  0 }, // '0'
{ (157-11)/2, (200-11)/2, 26/2, 13/2, 10,  3,  0 }, // '3'
{ (155-11)/2, (225-11)/2, 27/2, 14/2, 16,  6,  0 }, // '6'
{ (153-11)/2, (251-11)/2, 28/2, 14/2, 22,  9,  0 }, // '9'
{ (152-11)/2, (277-11)/2, 27/2, 14/2, 28, 12,  0 }, // 'En'
{ (537-11)/2, (197-11)/2, 28/2, 13/2, 11,  1,  1 }, // '1'
{ (538-11)/2, (223-11)/2, 28/2, 13/2, 17,  4,  1 }, // '4'
{ (540-11)/2, (249-11)/2, 28/2, 13/2, 23,  7,  1 }, // '7'
{ (541-11)/2, (276-11)/2, 27/2, 13/2, 29, 10,  1 }, // 'Cl'
{ (579-11)/2, (197-11)/2, 28/2, 13/2,  1,  2,  1 }, // '2'
{ (581-11)/2, (223-11)/2, 28/2, 13/2, 18,  5,  1 }, // '5'
{ (582-11)/2, (249-11)/2, 28/2, 13/2, 24,  8,  1 }, // '8'
{ (585-11)/2, (275-11)/2, 27/2, 13/2, 30, 11,  1 }, // '0'
{ (621-11)/2, (196-11)/2, 29/2, 14/2, 13,  3,  1 }, // '3'
{ (623-11)/2, (222-11)/2, 29/2, 14/2, 19,  6,  1 }, // '6'
{ (626-11)/2, (249-11)/2, 28/2, 13/2, 25,  9,  1 }, // '9'
{ (629-11)/2, (275-11)/2, 27/2, 13/2, 31, 12,  1 }, // 'En'
{ (129-11)/2, ( 90-11)/2, 10/2, 10/2, -1, 17,  0 }, // p1 up
{ (114-11)/2, (105-11)/2, 10/2, 10/2, -1, 19,  0 }, // p1 left
{ (129-11)/2, (117-11)/2, 10/2, 10/2, -1, 18,  0 }, // p1 down
{ (143-11)/2, (105-11)/2, 10/2, 10/2, -1, 20,  0 }, // p1 right
{ (577-11)/2, ( 90-11)/2, 10/2, 10/2, -1, 17,  1 }, // p2 up
{ (565-11)/2, (105-11)/2, 10/2, 10/2, -1, 19,  1 }, // p2 left
{ (577-11)/2, (117-11)/2, 10/2, 10/2, -1, 18,  1 }, // p2 down
{ (591-11)/2, (105-11)/2, 10/2, 10/2, -1, 20,  1 }, // p2 right
{ ( 71-11)/2, (174-11)/2, 29/2, 14/2,  0,  2,  0 }, // p1 '2'
{ (157-11)/2, (174-11)/2, 28/2, 14/2,  0,  2,  0 }, // p1 '2'
{ (536-11)/2, (171-11)/2, 26/2, 14/2,  1,  2,  1 }, // p2 '2'
{ (621-11)/2, (171-11)/2, 27/2, 14/2,  1,  2,  1 }, // p2 '2'
{      -1,         -1,      -1,   -1,  2, 13,  0 }, // p1 x1
{      -1,         -1,      -1,   -1,  3, 14,  0 }, // p1 x2
{      -1,         -1,      -1,   -1,  4, 15,  0 }, // p1 x3
{      -1,         -1,      -1,   -1,  9, 16,  0 }, // p1 x4
{      -1,         -1,      -1,   -1,  5, 13,  1 }, // p2 x1
{      -1,         -1,      -1,   -1,  6, 14,  1 }, // p2 x2
{      -1,         -1,      -1,   -1,  7, 15,  1 }, // p2 x3
{      -1,         -1,      -1,   -1, 12, 16,  1 }, // p2 x4
{ (261-11)/2, ( 61-11)/2,194/2, 36/2, -1, -1, -1 }, // 48 cartridge slot
{ (279-11)/2, (155-11)/2, 19/2, 46/2, -1, -1, -1 }, // 49 power switch
}, { // 2 (small Palladium)
{ (255-11)/2, (350-11)/2, 49/2, 31/2, -1, -1, -1 }, // START
{ (332-11)/2, (351-11)/2, 50/2, 31/2, -1, -1, -1 }, // A
{ (409-11)/2, (351-11)/2, 50/2, 31/2, -1, -1, -1 }, // B
{ (420-11)/2, (274-11)/2, 36/2, 52/2, -1, -1, -1 }, // RESET
{ ( 56-11)/2, (273-11)/2, 22/2, 16/2,  8,  1,  0 }, // '1'
{ ( 58-11)/2, (306-11)/2, 22/2, 14/2, 14,  4,  0 }, // '4'
{ ( 59-11)/2, (338-11)/2, 22/2, 14/2, 20,  7,  0 }, // '7'
{ ( 60-11)/2, (369-11)/2, 22/2, 14/2, 26, 10,  0 }, // 'Cl'
{ ( 88-11)/2, (207-11)/2, 22/2, 15/2,  0,  2,  0 }, // '2'
{ ( 92-11)/2, (306-11)/2, 22/2, 14/2, 15,  5,  0 }, // '5'
{ ( 94-11)/2, (338-11)/2, 22/2, 14/2, 21,  8,  0 }, // '8'
{ ( 95-11)/2, (370-11)/2, 22/2, 14/2, 27, 11,  0 }, // '0'
{ (127-11)/2, (273-11)/2, 22/2, 16/2, 10,  3,  0 }, // '3'
{ (128-11)/2, (306-11)/2, 22/2, 14/2, 16,  6,  0 }, // '6'
{ (130-11)/2, (339-11)/2, 22/2, 14/2, 22,  9,  0 }, // '9'
{ (131-11)/2, (370-11)/2, 22/2, 14/2, 28, 12,  0 }, // 'En'
{ (570-11)/2, (277-11)/2, 22/2, 14/2, 11,  1,  1 }, // '1'
{ (568-11)/2, (310-11)/2, 22/2, 14/2, 17,  4,  1 }, // '4'
{ (568-11)/2, (342-11)/2, 22/2, 14/2, 23,  7,  1 }, // '7'
{ (566-11)/2, (373-11)/2, 22/2, 14/2, 29, 10,  1 }, // 'Cl'
{ (608-11)/2, (211-11)/2, 22/2, 14/2,  1,  2,  1 }, // '2'
{ (603-11)/2, (310-11)/2, 22/2, 14/2, 18,  5,  1 }, // '5'
{ (603-11)/2, (342-11)/2, 22/2, 14/2, 24,  8,  1 }, // '8'
{ (602-11)/2, (372-11)/2, 22/2, 14/2, 30, 11,  1 }, // '0'
{ (641-11)/2, (277-11)/2, 22/2, 14/2, 13,  3,  1 }, // '3'
{ (639-11)/2, (309-11)/2, 22/2, 14/2, 19,  6,  1 }, // '6'
{ (638-11)/2, (342-11)/2, 22/2, 14/2, 25,  9,  1 }, // '9'
{ (636-11)/2, (372-11)/2, 22/2, 14/2, 31, 12,  1 }, // 'En'
{ ( 90-11)/2, (111-11)/2, 10/2, 10/2, -1, 17,  0 }, // p1 up
{ ( 75-11)/2, (125-11)/2, 10/2, 10/2, -1, 19,  0 }, // p1 left
{ ( 90-11)/2, (140-11)/2, 10/2, 10/2, -1, 18,  0 }, // p1 down
{ (105-11)/2, (125-11)/2, 10/2, 10/2, -1, 20,  0 }, // p1 right
{ (616-11)/2, (116-11)/2, 10/2, 10/2, -1, 17,  1 }, // p2 up
{ (601-11)/2, (130-11)/2, 10/2, 10/2, -1, 19,  1 }, // p2 left
{ (616-11)/2, (145-11)/2, 10/2, 10/2, -1, 18,  1 }, // p2 down
{ (631-11)/2, (130-11)/2, 10/2, 10/2, -1, 20,  1 }, // p2 right
{      -1,         -1,      -1,   -1,  0,  2,  0 }, // p1 '2'
{      -1,         -1,      -1,   -1,  0,  2,  0 }, // p1 '2'
{      -1,         -1,      -1,   -1,  1,  2,  1 }, // p2 '2'
{      -1,         -1,      -1,   -1,  1,  2,  1 }, // p2 '2'
{ ( 55-11)/2, (240-11)/2, 22/2, 16/2,  2, 13,  0 }, // p1 x1
{ ( 90-11)/2, (240-11)/2, 22/2, 16/2,  3, 14,  0 }, // p1 x2
{ (126-11)/2, (240-11)/2, 22/2, 16/2,  4, 15,  0 }, // p1 x3
{ ( 91-11)/2, (273-11)/2, 22/2, 16/2,  9, 16,  0 }, // p1 x4
{ (571-11)/2, (244-11)/2, 22/2, 14/2,  5, 13,  1 }, // p2 x1
{ (606-11)/2, (244-11)/2, 22/2, 14/2,  6, 14,  1 }, // p2 x2
{ (642-11)/2, (244-11)/2, 22/2, 14/2,  7, 15,  1 }, // p2 x3
{ (605-11)/2, (277-11)/2, 22/2, 14/2, 12, 16,  1 }, // p2 x4
{ (258-11)/2, ( 93-11)/2,200/2, 56/2, -1, -1, -1 }, // 48 cartridge slot
{ (260-11)/2, (274-11)/2, 37/2, 51/2, -1, -1, -1 }, // 49 power switch
}, { // 3 (big Emerson)
{ 360-11, 236-11, 24, 24, -1, -1, -1 }, // START
{ 294-11, 235-11, 24, 24, -1, -1, -1 }, // A
{ 327-11, 236-11, 24, 23, -1, -1, -1 }, // B
{ 261-11, 236-11, 23, 23, -1, -1, -1 }, // RESET
{ 132-11, 107-11, 25, 22,  8,  1,  0 }, // '1'
{ 125-11, 133-11, 25, 23, 14,  4,  0 }, // '4'
{ 119-11, 160-11, 24, 23, 20,  7,  0 }, // '7'
{ 112-11, 188-11, 25, 23, 26, 10,  0 }, // 'Cl'
{ 160-11, 106-11, 25, 22,  0,  2,  0 }, // '2'
{ 154-11, 132-11, 25, 22, 15,  5,  0 }, // '5'
{ 148-11, 160-11, 25, 23, 21,  8,  0 }, // '8'
{ 141-11, 187-11, 26, 24, 27, 11,  0 }, // '0'
{ 190-11, 106-11, 25, 22, 10,  3,  0 }, // '3'
{ 184-11, 133-11, 25, 22, 16,  6,  0 }, // '6'
{ 178-11, 160-11, 25, 23, 22,  9,  0 }, // '9'
{ 171-11, 187-11, 27, 24, 28, 12,  0 }, // 'En'
{ 532-11, 117-11, 27, 23, 11,  1,  1 }, // '1'
{ 529-11, 145-11, 27, 23, 17,  4,  1 }, // '4'
{ 526-11, 174-11, 27, 25, 23,  7,  1 }, // '7'
{ 523-11, 204-11, 28, 25, 29, 10,  1 }, // 'Cl'
{ 565-11, 117-11, 27, 23,  1,  2,  1 }, // '2'
{ 562-11, 145-11, 28, 23, 18,  5,  1 }, // '5'
{ 559-11, 174-11, 29, 25, 24,  8,  1 }, // '8'
{ 557-11, 204-11, 28, 25, 30, 11,  1 }, // '0'
{ 598-11, 117-11, 28, 23, 13,  3,  1 }, // '3'
{ 596-11, 145-11, 28, 24, 19,  6,  1 }, // '6'
{ 594-11, 174-11, 29, 26, 25,  9,  1 }, // '9'
{ 592-11, 205-11, 29, 25, 31, 12,  1 }, // 'En'
{ 122-11, 242-11, 20, 20, -1, 17,  0 }, // 28 p1 up
{ 100-11, 265-11, 20, 20, -1, 19,  0 }, // 29 p1 left
{ 122-11, 288-11, 20, 20, -1, 18,  0 }, // 30 p1 down
{ 144-11, 265-11, 20, 20, -1, 20,  0 }, // 31 p1 right
{ 552-11, 265-11, 21, 20, -1, 17,  1 }, // 32 p2 up
{ 527-11, 289-11, 21, 20, -1, 19,  1 }, // 33 p2 left
{ 552-11, 316-11, 21, 22, -1, 18,  1 }, // 34 p2 down
{ 578-11, 289-11, 23, 20, -1, 20,  1 }, // 35 p2 right
{     -1,     -1, -1, -1,  0,  2,  0 }, // 36 p1 '2'
{     -1,     -1, -1, -1,  0,  2,  0 }, // 37 p1 '2'
{     -1,     -1, -1, -1,  1,  2,  1 }, // 38 p2 '2'
{     -1,     -1, -1, -1,  1,  2,  1 }, // 39 p2 '2'
{     -1,     -1, -1, -1,  2, 13,  0 }, // 40 p1 x1
{     -1,     -1, -1, -1,  3, 14,  0 }, // 41 p1 x2
{     -1,     -1, -1, -1,  4, 15,  0 }, // 42 p1 x3
{     -1,     -1, -1, -1,  9, 16,  0 }, // 43 p1 x4
{     -1,     -1, -1, -1,  5, 13,  1 }, // 44 p2 x1
{     -1,     -1, -1, -1,  6, 14,  1 }, // 45 p2 x2
{     -1,     -1, -1, -1,  7, 15,  1 }, // 46 p2 x3
{     -1,     -1, -1, -1, 12, 16,  1 }, // 47 p2 x4
{ 263-11,  80-11,214, 45, -1, -1, -1 }, // 48 cartridge slot
{ 395-11, 237-11, 51, 26, -1, -1, -1 }, // 49 power switch
}, { // 4 (big MPT-03)
{ 410-11, 157-11, 26, 14, -1, -1, -1 }, // START
{ 338-11, 157-11, 27, 15, -1, -1, -1 }, // A
{ 374-11, 157-11, 26, 14, -1, -1, -1 }, // B
{ 324-11, 237-11, 74, 20, -1, -1, -1 }, // RESET
{  70-11, 200-11, 27, 13,  8,  1,  0 }, // '1'
{  68-11, 225-11, 27, 14, 14,  4,  0 }, // '4'
{  66-11, 251-11, 28, 14, 20,  7,  0 }, // '7'
{  64-11, 278-11, 27, 13, 26, 10,  0 }, // 'Cl'
{ 114-11, 200-11, 27, 13,  0,  2,  0 }, // '2'
{ 111-11, 225-11, 28, 14, 15,  5,  0 }, // '5'
{ 109-11, 251-11, 28, 14, 21,  8,  0 }, // '8'
{ 108-11, 277-11, 27, 14, 27, 11,  0 }, // '0'
{ 157-11, 200-11, 26, 13, 10,  3,  0 }, // '3'
{ 155-11, 225-11, 27, 14, 16,  6,  0 }, // '6'
{ 153-11, 251-11, 28, 14, 22,  9,  0 }, // '9'
{ 152-11, 277-11, 27, 14, 28, 12,  0 }, // 'En'
{ 537-11, 197-11, 28, 13, 11,  1,  1 }, // '1'
{ 538-11, 223-11, 28, 13, 17,  4,  1 }, // '4'
{ 540-11, 249-11, 28, 13, 23,  7,  1 }, // '7'
{ 541-11, 276-11, 27, 13, 29, 10,  1 }, // 'Cl'
{ 579-11, 197-11, 28, 13,  1,  2,  1 }, // '2'
{ 581-11, 223-11, 28, 13, 18,  5,  1 }, // '5'
{ 582-11, 249-11, 28, 13, 24,  8,  1 }, // '8'
{ 585-11, 275-11, 27, 13, 30, 11,  1 }, // '0'
{ 621-11, 196-11, 29, 14, 13,  3,  1 }, // '3'
{ 623-11, 222-11, 29, 14, 19,  6,  1 }, // '6'
{ 626-11, 249-11, 28, 13, 25,  9,  1 }, // '9'
{ 629-11, 275-11, 27, 13, 31, 12,  1 }, // 'En'
{ 129-11,  90-11, 10, 10, -1, 17,  0 }, // p1 up
{ 114-11, 105-11, 10, 10, -1, 19,  0 }, // p1 left
{ 129-11, 117-11, 10, 10, -1, 18,  0 }, // p1 down
{ 143-11, 105-11, 10, 10, -1, 20,  0 }, // p1 right
{ 577-11,  90-11, 10, 10, -1, 17,  1 }, // p2 up
{ 565-11, 105-11, 10, 10, -1, 19,  1 }, // p2 left
{ 577-11, 117-11, 10, 10, -1, 18,  1 }, // p2 down
{ 591-11, 105-11, 10, 10, -1, 20,  1 }, // p2 right
{  71-11, 174-11, 29, 14,  0,  2,  0 }, // p1 '2'
{ 157-11, 174-11, 28, 14,  0,  2,  0 }, // p1 '2'
{ 536-11, 171-11, 26, 14,  1,  2,  1 }, // p2 '2'
{ 621-11, 171-11, 27, 14,  1,  2,  1 }, // p2 '2'
{     -1,     -1, -1, -1,  2, 13,  0 }, // p1 x1
{     -1,     -1, -1, -1,  3, 14,  0 }, // p1 x2
{     -1,     -1, -1, -1,  4, 15,  0 }, // p1 x3
{     -1,     -1, -1, -1,  9, 16,  0 }, // p1 x4
{     -1,     -1, -1, -1,  5, 13,  1 }, // p2 x1
{     -1,     -1, -1, -1,  6, 14,  1 }, // p2 x2
{     -1,     -1, -1, -1,  7, 15,  1 }, // p2 x3
{     -1,     -1, -1, -1, 12, 16,  1 }, // p2 x4
{ 261-11,  61-11,194, 36, -1, -1, -1 }, // 48 cartridge slot
{ 279-11, 155-11, 19, 46, -1, -1, -1 }, // 49 power switch
}, { // 5 (big Palladium)
{ 255-11, 350-11, 49, 31, -1, -1, -1 }, // START
{ 332-11, 351-11, 50, 31, -1, -1, -1 }, // A
{ 409-11, 351-11, 50, 31, -1, -1, -1 }, // B
{ 420-11, 274-11, 36, 52, -1, -1, -1 }, // RESET
{  56-11, 273-11, 22, 16,  8,  1,  0 }, // '1'
{  58-11, 306-11, 22, 14, 14,  4,  0 }, // '4'
{  59-11, 338-11, 22, 14, 20,  7,  0 }, // '7'
{  60-11, 369-11, 22, 14, 26, 10,  0 }, // 'Cl'
{  88-11, 207-11, 22, 15,  0,  2,  0 }, // '2'
{  92-11, 306-11, 22, 14, 15,  5,  0 }, // '5'
{  94-11, 338-11, 22, 14, 21,  8,  0 }, // '8'
{  95-11, 370-11, 22, 14, 27, 11,  0 }, // '0'
{ 127-11, 273-11, 22, 16, 10,  3,  0 }, // '3'
{ 128-11, 306-11, 22, 14, 16,  6,  0 }, // '6'
{ 130-11, 339-11, 22, 14, 22,  9,  0 }, // '9'
{ 131-11, 370-11, 22, 14, 28, 12,  0 }, // 'En'
{ 570-11, 277-11, 22, 14, 11,  1,  1 }, // '1'
{ 568-11, 310-11, 22, 14, 17,  4,  1 }, // '4'
{ 568-11, 342-11, 22, 14, 23,  7,  1 }, // '7'
{ 566-11, 373-11, 22, 14, 29, 10,  1 }, // 'Cl'
{ 608-11, 211-11, 22, 14,  1,  2,  1 }, // '2'
{ 603-11, 310-11, 22, 14, 18,  5,  1 }, // '5'
{ 603-11, 342-11, 22, 14, 24,  8,  1 }, // '8'
{ 602-11, 372-11, 22, 14, 30, 11,  1 }, // '0'
{ 641-11, 277-11, 22, 14, 13,  3,  1 }, // '3'
{ 639-11, 309-11, 22, 14, 19,  6,  1 }, // '6'
{ 638-11, 342-11, 22, 14, 25,  9,  1 }, // '9'
{ 636-11, 372-11, 22, 14, 31, 12,  1 }, // 'En'
{  90-11, 111-11, 10, 10, -1, 17,  0 }, // p1 up
{  75-11, 125-11, 10, 10, -1, 19,  0 }, // p1 left
{  90-11, 140-11, 10, 10, -1, 18,  0 }, // p1 down
{ 105-11, 125-11, 10, 10, -1, 20,  0 }, // p1 right
{ 616-11, 116-11, 10, 10, -1, 17,  1 }, // p2 up
{ 601-11, 130-11, 10, 10, -1, 19,  1 }, // p2 left
{ 616-11, 145-11, 10, 10, -1, 18,  1 }, // p2 down
{ 631-11, 130-11, 10, 10, -1, 20,  1 }, // p2 right
{     -1,     -1, -1, -1,  0,  2,  0 }, // p1 '2'
{     -1,     -1, -1, -1,  0,  2,  0 }, // p1 '2'
{     -1,     -1, -1, -1,  1,  2,  1 }, // p2 '2'
{     -1,     -1, -1, -1,  1,  2,  1 }, // p2 '2'
{  55-11, 240-11, 22, 16,  2, 13,  0 }, // p1 x1
{  90-11, 240-11, 22, 16,  3, 14,  0 }, // p1 x2
{ 126-11, 240-11, 22, 16,  4, 15,  0 }, // p1 x3
{  91-11, 273-11, 22, 16,  9, 16,  0 }, // p1 x4
{ 571-11, 244-11, 22, 14,  5, 13,  1 }, // p2 x1
{ 606-11, 244-11, 22, 14,  6, 14,  1 }, // p2 x2
{ 642-11, 244-11, 22, 14,  7, 15,  1 }, // p2 x3
{ 605-11, 277-11, 22, 14, 12, 16,  1 }, // p2 x4
{ 258-11,  93-11,200, 56, -1, -1, -1 }, // 48 cartridge slot
{ 260-11, 274-11, 37, 51, -1, -1, -1 }, // 49 power switch
}, { // 6 (Interton Video 3000)
{ 383, 250, 37, 34, -1, -1, -1 }, //  0 START
{ 226, 101, 31, 86, -1, -1, -1 }, //  1 variant
{ 384, 214, 35, 35, -1, -1, -1 }, //  2 serve
{ 382,  77, 31, 33, -1, -1, -1 }, //  3 bat size
{ 383, 128, 32, 34, -1, -1, -1 }, //  4 angles
{ 381,  33, 30, 35, -1, -1, -1 }, //  5 speed
{ 384, 175, 32, 37, -1, -1, -1 }, //  6 serving
{ 228,  62, 31, 37, -1, -1, -1 }, //  7 reset
{  -1,  -1, -1, -1, -1, -1, -1 }, //  8 lock horizontal axis
{  -1,  -1, -1, -1, -1, -1, -1 }, //  9 sound
{  -1,  -1, -1, -1, -1, -1, -1 }, // 10 swap paddles
{ 100, 235, 10, 10, -1, 17,  0 }, // 11 p1 up
{ 100, 285, 10, 10, -1, 18,  0 }, // 12 p1 down
{ 531, 235, 10, 10, -1, 17,  1 }, // 13 p2 up
{ 531, 286, 10, 10, -1, 18,  1 }, // 14 p2 down
}, { // 7 (Sheen)
{ 168, 229, 34, 24, -1, -1, -1 }, //  0 START
{ 456, 124, 16, 77, -1, -1, -1 }, //  1 variant (game)
{  98, 229, 36, 24, -1, -1, -1 }, //  2 serve
{ 109, 187, 26, 14, -1, -1, -1 }, //  3 bat size
{ 180, 143, 25, 15, -1, -1, -1 }, //  4 angles
{ 113, 144, 26, 14, -1, -1, -1 }, //  5 speed
{ 177, 187, 25, 14, -1, -1, -1 }, //  6 serving
{ 117, 101, 25, 15, -1, -1, -1 }, //  7 reset
{  -1,  -1, -1, -1, -1, -1, -1 }, //  8 lock horizontal axis
{ 182, 101, 26, 15, -1, -1, -1 }, //  9 sound
{ 242, 235,154, 30, -1, -1, -1 }, // 10 swap paddles
{  -1,  -1, -1, -1, -1, 17,  0 }, // 11 p1 up
{  -1,  -1, -1, -1, -1, 18,  0 }, // 12 p1 down
{  -1,  -1, -1, -1, -1, 17,  1 }, // 13 p2 up
{  -1,  -1, -1, -1, -1, 18,  1 }, // 14 p2 down
}, { // 8 (Coleco Telstar Galaxy)
{ 232, 220, 24, 24, -1, -1, -1 }, //  0 START
{ 317, 184, 50, 50, -1, -1, -1 }, //  1 variant
{  -1,  -1, -1, -1, -1, -1, -1 }, //  2 serve
{  -1,  -1, -1, -1, -1, -1, -1 }, //  3 bat size
{  -1,  -1, -1, -1, -1, -1, -1 }, //  4 angles
{  -1,  -1, -1, -1, -1, -1, -1 }, //  5 speed
{  -1,  -1, -1, -1, -1, -1, -1 }, //  6 serving
{  -1,  -1, -1, -1, -1, -1, -1 }, //  7 reset
{ 240, 176, 16, 25, -1, -1, -1 }, //  8 lock horizontal axis
{  -1,  -1, -1, -1, -1, -1, -1 }, //  9 sound
{  -1,  -1, -1, -1, -1, -1, -1 }, // 10 swap paddles
{ 135,  80, 10, 10, -1, 17,  0 }, // 11 p1 up
{ 133, 136, 10, 10, -1, 18,  0 }, // 12 p1 down
{ 544,  82, 10, 10, -1, 17,  1 }, // 13 p2 up
{ 544, 137, 10, 10, -1, 18,  1 }, // 14 p2 down
}, { // 9 (Philco Telejogo ][)
{ 499, 160, 20, 18, -1, -1, -1 }, //  0 START
{ 449,  54, 51, 60, -1, -1, -1 }, //  1 variant (entire area is left 88-11, top 55-11, width 530, height 98)
{  -1,  -1, -1, -1, -1, -1, -1 }, //  2 serve
{ 416, 165, 59, 30, -1, -1, -1 }, //  3 bat sizes
{  -1,  -1, -1, -1, -1, -1, -1 }, //  4 angles
{ 377, 165, 17, 29, -1, -1, -1 }, //  5 speed
{  -1,  -1, -1, -1, -1, -1, -1 }, //  6 serving
{ 256, 164, 18, 29, -1, -1, -1 }, //  7 reset
{  -1,  -1, -1, -1, -1, -1, -1 }, //  8 lock horizontal axis
{  -1,  -1, -1, -1, -1, -1, -1 }, //  9 sound
{  -1,  -1, -1, -1, -1, -1, -1 }, // 10 swap paddles
{  -1,  -1, -1, -1, -1, 17,  0 }, // 11 p1 up
{  -1,  -1, -1, -1, -1, 18,  0 }, // 12 p1 down
{  -1,  -1, -1, -1, -1, 17,  1 }, // 13 p2 up
{  -1,  -1, -1, -1, -1, 18,  1 }, // 14 p2 down
}, { // 10 (Instructor 50) (small)
{ 246-11, 202-11, 12, 11, -1, -1, -1 }, // SENS
{ 246-11, 219-11, 12, 11, -1, -1, -1 }, // INT
{ 246-11, 236-11, 12, 11, -1, -1, -1 }, // MON
{ 245-11, 253-11, 12, 11, -1, -1, -1 }, // RESET
{ 265-11, 202-11, 12, 11,  8,  1,  0 }, // WCAS
{ 265-11, 219-11, 12, 11, 14,  4,  0 }, // RCAS
{ 265-11, 236-11, 12, 11, 20,  7,  0 }, // STEP
{ 265-11, 253-11, 12, 11, 26, 10,  0 }, // RUN
{ 283-11, 202-11, 12, 11,  0,  2,  0 }, // BKPT
{ 283-11, 219-11, 12, 11, 15,  5,  0 }, // REG
{ 283-11, 236-11, 12, 11, 21,  8,  0 }, // MEM
{ 283-11, 253-11, 12, 11, 27, 11,  0 }, // ENT/NXT
{ 336-11, 202-11, 12, 11, 10,  3,  0 }, // C
{ 336-11, 219-11, 12, 11, 16,  6,  0 }, // 8
{ 336-11, 236-11, 12, 11, 22,  9,  0 }, // 4
{ 336-11, 253-11, 12, 11, 28, 12,  0 }, // 0
{ 354-11, 202-11, 12, 11, 11,  1,  1 }, // D
{ 354-11, 219-11, 12, 11, 17,  4,  1 }, // 9
{ 354-11, 236-11, 12, 11, 23,  7,  1 }, // 5
{ 355-11, 253-11, 12, 11, 29, 10,  1 }, // 1
{ 374-11, 202-11, 12, 11,  1,  2,  1 }, // E
{ 374-11, 219-11, 12, 11, 18,  5,  1 }, // A
{ 374-11, 236-11, 12, 11, 24,  8,  1 }, // 6
{ 375-11, 253-11, 12, 11, 30, 11,  1 }, // 2
{ 393-11, 202-11, 12, 11, 13,  3,  1 }, // F
{ 393-11, 219-11, 12, 11, 19,  6,  1 }, // B
{ 393-11, 236-11, 12, 11, 25,  9,  1 }, // 7
{ 394-11, 253-11, 12, 11, 31, 12,  1 }, // 3
{  99-11, 246-11, 72, 38, -1, -1, -1 }, // DIP parallel (28)
{ 183-11, 261-11, 22, 22, -1, -1, -1 }, // DIP indirect (29)
{ 400-11, 160-11, 19, 30, -1, -1, -1 }, // phone/mic    (30)
{  87   , 130   ,267, 50, -1, -1, -1 }, // opcode table (31)
}, { // 11 (Instructor 50) (large)
{ 409-11, 264-28, 21, 19, -1, -1, -1 }, // SENS
{ 411-11, 301-28, 21, 19, -1, -1, -1 }, // INT
{ 413-11, 340-28, 21, 19, -1, -1, -1 }, // MON
{ 415-11, 380-28, 21, 19, -1, -1, -1 }, // RESET
{ 450-11, 264-28, 21, 19,  8,  1,  0 }, // WCAS
{ 453-11, 301-28, 21, 19, 14,  4,  0 }, // RCAS
{ 456-11, 340-28, 21, 19, 20,  7,  0 }, // STEP
{ 459-11, 380-28, 21, 19, 26, 10,  0 }, // RUN
{ 492-11, 264-28, 21, 19,  0,  2,  0 }, // BKPT
{ 495-11, 301-28, 21, 19, 15,  5,  0 }, // REG
{ 499-11, 340-28, 21, 19, 21,  8,  0 }, // MEM
{ 503-11, 380-28, 21, 19, 27, 11,  0 }, // ENT/NXT
{ 607-11, 264-28, 18, 19, 10,  3,  0 }, // C
{ 613-11, 301-28, 18, 19, 16,  6,  0 }, // 8
{ 619-11, 339-28, 18, 19, 22,  9,  0 }, // 4
{ 626-11, 378-28, 18, 19, 28, 12,  0 }, // 0
{ 647-11, 264-28, 18, 19, 11,  1,  1 }, // D
{ 654-11, 301-28, 18, 19, 17,  4,  1 }, // 9
{ 661-11, 339-28, 18, 19, 23,  7,  1 }, // 5
{ 668-11, 378-28, 18, 19, 29, 10,  1 }, // 1
{ 687-11, 264-28, 18, 19,  1,  2,  1 }, // E
{ 695-11, 301-28, 18, 19, 18,  5,  1 }, // A
{ 703-11, 338-28, 18, 19, 24,  8,  1 }, // 6
{ 711-11, 378-28, 18, 19, 30, 11,  1 }, // 2
{ 727-11, 264-28, 18, 19, 13,  3,  1 }, // F
{ 735-11, 300-28, 18, 19, 19,  6,  1 }, // B
{ 743-11, 337-28, 18, 19, 25,  9,  1 }, // 7
{ 753-11, 378-28, 18, 19, 31, 12,  1 }, // 3
{  68-11, 372-28,159, 75, -1, -1, -1 }, // DIP parallel (28)
{ 272-11, 394-28, 35, 49, -1, -1, -1 }, // DIP indirect (29)
{ 719-11, 179-28, 41, 57, -1, -1, -1 }, // phone/mic    (30)
{  63   , 124   ,577, 86, -1, -1, -1 }, // opcode table (31)
}, { // 12 (PIPBUG) (Elekterminal)
// 1st row
{  43, 150-134, 35, 35 }, //  0 1 !
{  98, 150-134, 35, 35 }, //  1 2 "
{ 152, 150-134, 35, 35 }, //  2 3 #
{ 210, 150-134, 35, 35 }, //  3 4 $
{ 265, 150-134, 35, 35 }, //  4 5 %
{ 322, 150-134, 35, 35 }, //  5 6 &
{ 380, 150-134, 35, 35 }, //  6 7 '
{ 438, 150-134, 35, 35 }, //  7 8 (
{ 495, 150-134, 35, 35 }, //  8 9 )
{ 551, 150-134, 35, 35 }, //  9 0 NUL
{ 607, 150-134, 35, 35 }, // 10 : *
{ 668, 150-134, 35, 35 }, // 11 - =
{ 723, 150-134, 35, 35 }, // 12 @ `
{ 781, 150-134, 37, 35 }, // 13 erase
{ 839, 150-134, 35, 35 }, // 14 PgUp
// 2nd row
{  14, 208-134, 35, 35 }, // 15 Esc
{  69, 208-134, 35, 35 }, // 16 Q
{ 125, 208-134, 35, 35 }, // 17 W
{ 180, 208-134, 35, 35 }, // 18 E
{ 236, 208-134, 35, 35 }, // 19 R
{ 294, 208-134, 35, 35 }, // 20 T
{ 349, 207-134, 35, 35 }, // 21 Y
{ 408, 207-134, 35, 35 }, // 22 U
{ 464, 208-134, 35, 35 }, // 23 I
{ 522, 208-134, 35, 35 }, // 24 O
{ 580, 208-134, 35, 35 }, // 25 P
{ 637, 207-134, 35, 35 }, // 26 [ {
{ 694, 207-134, 35, 35 }, // 27 ] }
{ 754, 207-134, 35, 35 }, // 28 LF aka down
{ 809, 207-134, 35, 35 }, // 29 CR
{ 869, 207-134, 35, 35 }, // 30 rub out aka res
// 3rd row
{  40, 264-134, 35, 35 }, // 31 CTRL
{  95, 264-134, 35, 35 }, // 32 A
{ 151, 264-134, 35, 35 }, // 33 S
{ 209, 264-134, 35, 35 }, // 34 D
{ 266, 264-134, 35, 35 }, // 35 F
{ 323, 265-134, 35, 35 }, // 36 G
{ 380, 265-134, 35, 35 }, // 37 H
{ 437, 265-134, 35, 35 }, // 38 J
{ 494, 265-134, 35, 35 }, // 39 K
{ 551, 265-134, 35, 35 }, // 40 L
{ 610, 266-134, 35, 35 }, // 41 ; +
{ 667, 266-134, 35, 35 }, // 42 home
{ 725, 266-134, 35, 35 }, // 43 left
{ 781, 266-134, 35, 35 }, // 44 right
{ 841, 266-134, 35, 35 }, // 45 PgDn
// 4th row
{  67, 322-134, 35, 35 }, // 46 Left SHIFT
{ 124, 322-134, 35, 35 }, // 47 Z
{ 180, 322-134, 35, 35 }, // 48 X
{ 238, 322-134, 35, 35 }, // 49 C
{ 294, 322-134, 35, 35 }, // 50 V
{ 351, 322-134, 35, 35 }, // 51 B
{ 408, 322-134, 35, 35 }, // 52 N
{ 466, 322-134, 35, 35 }, // 53 M
{ 522, 323-134, 35, 35 }, // 54 , <
{ 580, 323-134, 35, 35 }, // 55 . >
{ 639, 323-134, 35, 35 }, // 56 / ?
{ 696, 323-134, 35, 35 }, // 57 Right SHIFT
{ 751, 323-134, 35, 35 }, // 58 up
{ 811, 324-134, 37, 35 }, // 59 break
// 5th row
{ 157, 375-134,549, 42 }, // 60 spacebar
// other controls
{ 522,  68    , 25, 25 }, // 61 reset (this is hardcoded!)
}, { // 13 (TWIN)
// 1st row
{  57,  71, 30, 31 }, //  0 1
{ 106,  71, 31, 31 }, //  1 2
{ 157,  71, 30, 31 }, //  2 3
{ 206,  71, 30, 31 }, //  3 4
{ 255,  71, 30, 31 }, //  4 5
{ 303,  71, 30, 31 }, //  5 6
{ 351,  71, 30, 31 }, //  6 7
{ 399,  71, 30, 31 }, //  7 8
{ 446,  71, 30, 31 }, //  8 9
{ 494,  71, 30, 31 }, //  9 0
{ 542,  71, 30, 31 }, // 10 :
{ 590,  71, 30, 31 }, // 11 -
{ 637,  70, 33, 32 }, // 12 blank
// 2nd row
{  30, 119, 32, 32 }, // 13 ESC
{  80, 118, 31, 32 }, // 14 Q
{ 131, 118, 31, 32 }, // 15 W
{ 181, 118, 30, 32 }, // 16 E
{ 230, 118, 30, 32 }, // 17 R
{ 279, 118, 30, 32 }, // 18 T
{ 327, 118, 30, 32 }, // 19 Y
{ 371, 118, 30, 32 }, // 20 U
{ 422, 118, 30, 32 }, // 21 I
{ 470, 118, 30, 31 }, // 22 O
{ 518, 118, 30, 31 }, // 23 P
{ 564, 117, 33, 32 }, // 24 LF
{ 613, 116, 33, 32 }, // 25 RETURN
// 3rd row
{  42, 167, 32, 32 }, // 26 CTRL
{  94, 166, 31, 32 }, // 27 A
{ 144, 166, 30, 32 }, // 28 S
{ 193, 166, 31, 32 }, // 29 D
{ 242, 166, 31, 32 }, // 30 F
{ 290, 166, 31, 32 }, // 31 G
{ 339, 166, 31, 32 }, // 32 H
{ 386, 165, 31, 32 }, // 33 J
{ 434, 166, 31, 31 }, // 34 K
{ 483, 166, 30, 30 }, // 35 L
{ 532, 165, 30, 31 }, // 36 ;
{ 578, 164, 33, 32 }, // 37 RUB OUT
{ 626, 164, 33, 32 }, // 38 REPT
{ 675, 164, 32, 32 }, // 39 BREAK
// 4th row
{  65, 217, 33, 31 }, // 40 Left SHIFT
{ 117, 216, 30, 32 }, // 41 Z
{ 167, 216, 31, 32 }, // 42 X
{ 215, 215, 30, 32 }, // 43 C
{ 265, 214, 30, 32 }, // 44 V
{ 314, 214, 30, 32 }, // 45 B
{ 361, 214, 31, 32 }, // 46 N
{ 409, 214, 31, 32 }, // 47 M
{ 458, 214, 31, 32 }, // 48 ,
{ 507, 214, 31, 32 }, // 49 .
{ 556, 214, 30, 32 }, // 50 /
{ 601, 213, 34, 33 }, // 51 Right SHIFT
// 5th row
{ 167, 268,368, 26 }, // 52 Spacebar
// other controls
{   5,  18, 68, 40 }, // 53 brightness
{ 503,   8, 42, 39 }, // 54 power
}, { // 14 (MIKIT) (small)
{  25,  73, 37, 38,  8,  1,  0 }, // BLANK
{  25, 112, 37, 38, 14,  4,  0 }, // +
{  25, 151, 37, 38, 20,  7,  0 }, // H
{  25, 190, 37, 38, 26, 10,  0 }, // L
{  63,  73, 37, 38,  0,  2,  0 }, // R
{  63, 112, 37, 38, 15,  5,  0 }, // G
{  63, 151, 37, 38, 21,  8,  0 }, // P
{  63, 190, 37, 38, 27, 11,  0 }, // S
{ 101,  73, 37, 38, 10,  3,  0 }, // C
{ 101, 112, 37, 38, 16,  6,  0 }, // 8
{ 101, 151, 37, 38, 22,  9,  0 }, // 4
{ 101, 190, 37, 38, 28, 12,  0 }, // 0
{ 140,  73, 37, 38, 11,  1,  1 }, // D
{ 140, 112, 37, 38, 17,  4,  1 }, // 9
{ 140, 151, 37, 38, 23,  7,  1 }, // 5
{ 140, 190, 37, 38, 29, 10,  1 }, // 1
{ 179,  73, 37, 38,  1,  2,  1 }, // E
{ 179, 112, 37, 38, 18,  5,  1 }, // A
{ 179, 151, 37, 38, 24,  8,  1 }, // 6
{ 179, 190, 37, 38, 30, 11,  1 }, // 2
{ 218,  73, 37, 38, 13,  3,  1 }, // F
{ 218, 112, 37, 38, 19,  6,  1 }, // B
{ 218, 151, 37, 38, 25,  9,  1 }, // 7
{ 218, 190, 37, 38, 31, 12,  1 }, // 3
}, { // 15 (MIKIT) (big)
{ 425, 211, 37, 38,  8,  1,  0 }, // BLANK
{ 425, 250, 37, 38, 14,  4,  0 }, // +
{ 425, 289, 37, 38, 20,  7,  0 }, // H
{ 425, 328, 37, 38, 26, 10,  0 }, // L
{ 463, 211, 37, 38,  0,  2,  0 }, // R
{ 463, 250, 37, 38, 15,  5,  0 }, // G
{ 463, 289, 37, 38, 21,  8,  0 }, // P
{ 463, 328, 37, 38, 27, 11,  0 }, // S
{ 501, 211, 37, 38, 10,  3,  0 }, // C
{ 501, 250, 37, 38, 16,  6,  0 }, // 8
{ 501, 289, 37, 38, 22,  9,  0 }, // 4
{ 501, 328, 37, 38, 28, 12,  0 }, // 0
{ 540, 211, 37, 38, 11,  1,  1 }, // D
{ 540, 250, 37, 38, 17,  4,  1 }, // 9
{ 540, 289, 37, 38, 23,  7,  1 }, // 5
{ 540, 328, 37, 38, 29, 10,  1 }, // 1
{ 579, 211, 37, 38,  1,  2,  1 }, // E
{ 579, 250, 37, 38, 18,  5,  1 }, // A
{ 579, 289, 37, 38, 24,  8,  1 }, // 6
{ 579, 328, 37, 38, 30, 11,  1 }, // 2
{ 618, 211, 37, 38, 13,  3,  1 }, // F
{ 618, 250, 37, 38, 19,  6,  1 }, // B
{ 618, 289, 37, 38, 25,  9,  1 }, // 7
{ 618, 328, 37, 38, 31, 12,  1 }, // 3
}, { // 16 (INTERTON) (small)
{ 418/2, 261/2, 63/2, 63/2, -1, -1, -1 }, // 0 START
{ 348/2, 262/2, 63/2, 63/2, -1, -1, -1 }, // 1 A
{    -1,    -1,   -1,   -1, -1, -1, -1 }, // 2 B (unused)
{ 282/2, 262/2, 61/2, 63/2, -1, -1, -1 }, // 3 RESET
{  75/2, 197/2, 28/2, 15/2,  8,  1,  0 }, // '1'
{  70/2, 229/2, 29/2, 15/2, 14,  4,  0 }, // '4'
{  65/2, 262/2, 30/2, 16/2, 20,  7,  0 }, // '7'
{  59/2, 296/2, 31/2, 17/2, 26, 10,  0 }, // 'Cl'
{ 111/2, 197/2, 29/2, 15/2,  0,  2,  0 }, // '2'
{ 107/2, 228/2, 30/2, 16/2, 15,  5,  0 }, // '5'
{ 103/2, 261/2, 30/2, 16/2, 21,  8,  0 }, // '8'
{  98/2, 295/2, 31/2, 17/2, 27, 11,  0 }, // '0'
{ 149/2, 196/2, 28/2, 15/2, 10,  3,  0 }, // '3'
{ 145/2, 228/2, 29/2, 15/2, 16,  6,  0 }, // '6'
{ 141/2, 261/2, 30/2, 16/2, 22,  9,  0 }, // '9'
{ 137/2, 295/2, 31/2, 17/2, 28, 12,  0 }, // 'En'
{ 515/2, 191/2, 30/2, 15/2, 11,  1,  1 }, // '1'
{ 519/2, 223/2, 30/2, 16/2, 17,  4,  1 }, // '4'
{ 522/2, 256/2, 31/2, 17/2, 23,  7,  1 }, // '7'
{ 527/2, 291/2, 31/2, 17/2, 29, 10,  1 }, // 'Cl'
{ 552/2, 191/2, 30/2, 15/2,  1,  2,  1 }, // '2'
{ 557/2, 223/2, 30/2, 16/2, 18,  5,  1 }, // '5'
{ 561/2, 256/2, 32/2, 17/2, 24,  8,  1 }, // '8'
{ 567/2, 291/2, 31/2, 17/2, 30, 11,  1 }, // '0'
{ 590/2, 191/2, 30/2, 15/2, 13,  3,  1 }, // '3'
{ 595/2, 223/2, 30/2, 16/2, 19,  6,  1 }, // '6'
{ 600/2, 256/2, 32/2, 17/2, 25,  9,  1 }, // '9'
{ 607/2, 291/2, 31/2, 17/2, 31, 12,  1 }, // 'En'
{ 135/2,  83/2, 10/2, 10/2, -1, 17,  0 }, // p1 up
{ 119/2,  95/2, 10/2, 10/2, -1, 19,  0 }, // p1 left
{ 135/2, 107/2, 10/2, 10/2, -1, 18,  0 }, // p1 down
{ 152/2,  95/2, 10/2, 10/2, -1, 20,  0 }, // p1 right
{ 543/2,  77/2, 10/2, 10/2, -1, 17,  1 }, // p2 up
{ 527/2,  90/2, 10/2, 10/2, -1, 19,  1 }, // p2 left
{ 547/2, 104/2, 10/2, 10/2, -1, 18,  1 }, // p2 down
{ 562/2,  90/2, 10/2, 10/2, -1, 20,  1 }, // p2 right
{  79/2, 166/2, 28/2, 15/2,  0,  2,  0 }, // p1 '2'
{ 151/2, 165/2, 28/2, 15/2,  0,  2,  0 }, // p1 '2'
{ 511/2, 160/2, 29/2, 16/2,  1,  2,  1 }, // p2 '2'
{ 584/2, 160/2, 31/2, 16/2,  1,  2,  1 }, // p2 '2'
{ 239/2,  89/2,210/2, 38/2, -1, -1, -1 }, // 40 cartridge slot
{ 213/2, 262/2, 63/2, 63/2, -1, -1, -1 }, // 41 power switch
}, { // 17 (INTERTON) (big)
{ 418, 261, 63, 63, -1, -1, -1 }, // 0 START
{ 348, 262, 63, 63, -1, -1, -1 }, // 1 A
{  -1,  -1, -1, -1, -1, -1, -1 }, // 2 B (impossible)
{ 282, 262, 61, 63, -1, -1, -1 }, // 3 RESET
{  75, 197, 28, 15,  8,  1,  0 }, // '1'
{  70, 229, 29, 15, 14,  4,  0 }, // '4'
{  65, 262, 30, 16, 20,  7,  0 }, // '7'
{  59, 296, 31, 17, 26, 10,  0 }, // 'Cl'
{ 111, 197, 29, 15,  0,  2,  0 }, // '2'
{ 107, 228, 30, 16, 15,  5,  0 }, // '5'
{ 103, 261, 30, 16, 21,  8,  0 }, // '8'
{  98, 295, 31, 17, 27, 11,  0 }, // '0'
{ 149, 196, 28, 15, 10,  3,  0 }, // '3'
{ 145, 228, 29, 15, 16,  6,  0 }, // '6'
{ 141, 261, 30, 16, 22,  9,  0 }, // '9'
{ 137, 295, 31, 17, 28, 12,  0 }, // 'En'
{ 515, 191, 30, 15, 11,  1,  1 }, // '1'
{ 519, 223, 30, 16, 17,  4,  1 }, // '4'
{ 522, 256, 31, 17, 23,  7,  1 }, // '7'
{ 527, 291, 31, 17, 29, 10,  1 }, // 'Cl'
{ 552, 191, 30, 15,  1,  2,  1 }, // '2'
{ 557, 223, 30, 16, 18,  5,  1 }, // '5'
{ 561, 256, 32, 17, 24,  8,  1 }, // '8'
{ 567, 291, 31, 17, 30, 11,  1 }, // '0'
{ 590, 191, 30, 15, 13,  3,  1 }, // '3'
{ 595, 223, 30, 16, 19,  6,  1 }, // '6'
{ 600, 256, 32, 17, 25,  9,  1 }, // '9'
{ 607, 291, 31, 17, 31, 12,  1 }, // 'En'
{ 135,  83, 10, 10, -1, 17,  0 }, // p1 up
{ 119,  95, 10, 10, -1, 19,  0 }, // p1 left
{ 135, 107, 10, 10, -1, 18,  0 }, // p1 down
{ 152,  95, 10, 10, -1, 20,  0 }, // p1 right
{ 543,  77, 10, 10, -1, 17,  1 }, // p2 up
{ 527,  90, 10, 10, -1, 19,  1 }, // p2 left
{ 547, 104, 10, 10, -1, 18,  1 }, // p2 down
{ 562,  90, 10, 10, -1, 20,  1 }, // p2 right
{  79, 166, 28, 15,  0,  2,  0 }, // p1 '2'
{ 151, 165, 28, 15,  0,  2,  0 }, // p1 '2'
{ 511, 160, 29, 16,  1,  2,  1 }, // p2 '2'
{ 584, 160, 31, 16,  1,  2,  1 }, // p2 '2'
{ 239,  89,210, 38, -1, -1, -1 }, // 40 cartridge slot
{ 213, 262, 63, 63, -1, -1, -1 }, // 41 power switch
}, { // 18 ELEKTOR (small)
{  68/2, 149/2, 60/2, 80/2, -1, -1, -1 }, // START
{  68/2,  65/2, 60/2, 80/2, -1, -1, -1 }, // UC
{  68/2, 233/2, 60/2, 80/2, -1, -1, -1 }, // LC
{  68/2, 317/2, 60/2, 79/2, -1, -1, -1 }, // RESET
{ 190/2,  65/2, 60/2, 80/2,  8,  1,  0 }, // RCAS
{ 190/2, 149/2, 60/2, 80/2, 14,  4,  0 }, // BP1/2
{ 190/2, 233/2, 60/2, 80/2, 20,  7,  0 }, // PC
{ 190/2, 317/2, 60/2, 79/2, 26, 10,  0 }, // -
{ 250/2,  65/2, 60/2, 80/2,  0,  2,  0 }, // WCAS
{ 250/2, 149/2, 60/2, 80/2, 15,  5,  0 }, // REG
{ 250/2, 233/2, 60/2, 80/2, 21,  8,  0 }, // MEM
{ 250/2, 317/2, 60/2, 79/2, 27, 11,  0 }, // +
{ 310/2,  65/2, 60/2, 80/2, 10,  3,  0 }, // C
{ 310/2, 149/2, 60/2, 80/2, 16,  6,  0 }, // 8
{ 310/2, 233/2, 60/2, 80/2, 22,  9,  0 }, // 4
{ 310/2, 317/2, 60/2, 79/2, 28, 12,  0 }, // 0
{ 470/2,  62/2, 60/2, 80/2, 11,  1,  1 }, // D
{ 469/2, 146/2, 60/2, 80/2, 17,  4,  1 }, // 9
{ 468/2, 230/2, 60/2, 80/2, 23,  7,  1 }, // 5
{ 468/2, 314/2, 60/2, 78/2, 29, 10,  1 }, // 1
{ 530/2,  62/2, 60/2, 80/2,  1,  2,  1 }, // E
{ 529/2, 146/2, 60/2, 80/2, 18,  5,  1 }, // A
{ 528/2, 230/2, 60/2, 80/2, 24,  8,  1 }, // 6
{ 528/2, 314/2, 60/2, 78/2, 30, 11,  1 }, // 2
{ 590/2,  62/2, 60/2, 80/2, 13,  3,  1 }, // F
{ 589/2, 146/2, 60/2, 80/2, 19,  6,  1 }, // B
{ 588/2, 230/2, 60/2, 79/2, 25,  9,  1 }, // 7
{ 588/2, 313/2, 60/2, 77/2, 31, 12,  1 }, // 3
{    -1,    -1,   -1,   -1, -1, -1,  0 }, // p1 up
{    -1,    -1,   -1,   -1, -1, -1,  0 }, // p1 left
{    -1,    -1,   -1,   -1, -1, -1,  0 }, // p1 down
{    -1,    -1,   -1,   -1, -1, -1,  0 }, // p1 right
{    -1,    -1,   -1,   -1, -1, -1,  1 }, // p2 up
{    -1,    -1,   -1,   -1, -1, -1,  1 }, // p2 left
{    -1,    -1,   -1,   -1, -1, -1,  1 }, // p2 down
{    -1,    -1,   -1,   -1, -1, -1,  1 }, // p2 right
}, { // 19 BINBUG
{   9,  12, 60, 60,  8, -1, -1 }, // C
{   9,  81, 60, 60, 14, -1, -1 }, // 8
{   9, 152, 60, 60, 20, -1, -1 }, // 4
{   9, 220, 60, 60, 26, -1, -1 }, // 0
{  79,  11, 60, 60,  0, -1, -1 }, // D
{  79,  82, 60, 60, 15, -1, -1 }, // 9
{  79, 152, 60, 60, 21, -1, -1 }, // 5
{  79, 220, 60, 60, 27, -1, -1 }, // 1
{ 149,  12, 60, 60, 10, -1, -1 }, // E
{ 149,  83, 60, 60, 16, -1, -1 }, // A
{ 149, 151, 60, 60, 22, -1, -1 }, // 6
{ 149, 220, 60, 60, 28, -1, -1 }, // 2
{ 219,  12, 60, 60, 11, -1, -1 }, // F
{ 219,  81, 60, 60, 17, -1, -1 }, // B
{ 219, 151, 60, 60, 23, -1, -1 }, // 7
{ 219, 221, 60, 60, 29, -1, -1 }, // 3
}, { // 20 MALZAK
{ 160,  49, 11, 10,  0,  2,  0 }, //  0 Fire
{ 260,  34, 11, 10, -1, -1, -1 }, //  1 1UP
{ 321,  34, 11, 10, -1, -1, -1 }, //  2 2UP
{  54,  19,  9, 10, -1, 17,  0 }, //  3 up
{  74,  25,  9, 10, -1, 25,  0 }, //  4 up-right
{  84,  40,  9, 10, -1, 20,  0 }, //  5 right
{  70,  55,  9, 10, -1, 27,  0 }, //  6 down-right
{  45,  65,  9, 10, -1, 18,  0 }, //  7 down
{  24,  56,  9, 10, -1, 26,  0 }, //  8 down-left
{  17,  40,  9, 10, -1, 19,  0 }, //  9 left
{  32,  27,  9, 10, -1, 24,  0 }, // 10 up-left
}, { // 21 PHUNSY
{  10,  58, 35, 35,  8,  1,  0 }, // 0
{  10, 102, 35, 35, 14,  4,  0 }, // 1
{  10, 146, 35, 35, 20,  7,  0 }, // 2
{  10, 190, 35, 35, 26, 10,  0 }, // 3
{  54,  58, 35, 35,  0,  2,  0 }, // 4
{  54, 102, 35, 35, 15,  5,  0 }, // 5
{  54, 146, 35, 35, 21,  8,  0 }, // 6
{  54, 190, 35, 35, 27, 11,  0 }, // 7
{  98,  58, 35, 35, 10,  3,  0 }, // 8
{  98, 102, 35, 35, 16,  6,  0 }, // 9
{  98, 146, 35, 35, 22,  9,  0 }, // A
{  98, 190, 35, 35, 28, 12,  0 }, // B
{ 142,  58, 35, 35, 11,  1,  1 }, // C
{ 142, 102, 35, 35, 17,  4,  1 }, // D
{ 142, 146, 35, 35, 23,  7,  1 }, // E
{ 142, 190, 35, 35, 29, 10,  1 }, // F
{ 192,  58, 35, 35,  1,  2,  1 }, // D->M
{ 192, 102, 35, 35, 18,  5,  1 }, // M->D
{ 192, 146, 35, 35, 24,  8,  1 }, // G
{ 192, 190, 35, 35, 30, 11,  1 }, // Cl
{ 236,  58, 35, 35, 13,  3,  1 }, // Reset
{ 236, 102, 35, 35, 19,  6,  1 }, // Halt
{ 236, 146, 35, 35, 25,  9,  1 }, // DumpCass
{ 236, 190, 35, 35, 31, 12,  1 }, // LoadCass
}, { // 22 SELBST
{ 164, 108, 30, 25,  8,  1,  0 }, // C
{ 164, 152, 30, 25, 14,  4,  0 }, // 8
{ 164, 196, 30, 25, 20,  7,  0 }, // 4
{ 164, 239, 30, 25, 26, 10,  0 }, // 0
{ 209, 108, 30, 25,  0,  2,  0 }, // D
{ 209, 152, 30, 25, 15,  5,  0 }, // 9
{ 209, 196, 30, 25, 21,  8,  0 }, // 5
{ 209, 239, 30, 25, 27, 11,  0 }, // 1
{ 254, 108, 30, 25, 10,  3,  0 }, // E
{ 254, 152, 30, 25, 16,  6,  0 }, // A
{ 254, 196, 30, 25, 22,  9,  0 }, // 6
{ 254, 239, 30, 25, 28, 12,  0 }, // 2
{ 298, 108, 30, 25, 11,  1,  1 }, // F
{ 298, 152, 30, 25, 17,  4,  1 }, // B
{ 298, 196, 30, 25, 23,  7,  1 }, // 7
{ 298, 239, 30, 25, 29, 10,  1 }, // 3
{ 349, 108, 28, 24,  1,  2,  1 }, // CMD
{ 349, 152, 28, 24, 18,  5,  1 }, // RUN
{ 348, 196, 28, 24, 24,  8,  1 }, // GOTO
{ 348, 239, 28, 24, 30, 11,  1 }, // RST
{ 392, 108, 28, 24, 13,  3,  1 }, // FLAG
{ 392, 152, 28, 24, 19,  6,  1 }, // MON
{ 391, 196, 28, 24, 25,  9,  1 }, // PC
{ 390, 238, 28, 24, 31, 12,  1 }, // NXT
}, { // 23 Type-right
{ 341,  38, 13,  5, -1, -1, -1 }, //   0 Class
{ 342,  53, 13,  5, -1, -1, -1 }, //   1 Game
{ 368,  38, 13,  5, -1, -1, -1 }, //   2 Speed
{ 368,  53, 13,  5, -1, -1, -1 }, //   3 Accuracy
{  88,  91, 17, 19, -1, -1, -1 }, //   4 1
{ 118,  91, 17, 19, -1, -1, -1 }, //   5 2
{ 148,  90, 17, 19, -1, -1, -1 }, //   6 3
{ 179,  90, 17, 19, -1, -1, -1 }, //   7 4
{ 210,  90, 17, 19, -1, -1, -1 }, //   8 5
{ 240,  90, 17, 19, -1, -1, -1 }, //   9 6
{ 271,  89, 17, 19, -1, -1, -1 }, //  10 7
{ 301,  89, 17, 19, -1, -1, -1 }, //  11 8
{ 331,  89, 17, 19, -1, -1, -1 }, //  12 9
{ 362,  89, 17, 19, -1, -1, -1 }, //  13 0
{ 409, 117, 17, 19, -1, -1, -1 }, //  14 =
{ 392,  89, 17, 19, -1, -1, -1 }, //  15 Left arrow
{ 101, 119, 17, 19, -1, -1, -1 }, //  16 Q
{ 132, 119, 17, 19, -1, -1, -1 }, //  17 W
{ 163, 118, 17, 19, -1, -1, -1 }, //  18 E
{ 195, 118, 17, 19, -1, -1, -1 }, //  19 R
{ 225, 118, 17, 19, -1, -1, -1 }, //  20 T
{ 257, 118, 17, 19, -1, -1, -1 }, //  21 Y
{ 288, 118, 17, 19, -1, -1, -1 }, //  22 U
{ 319, 117, 17, 19, -1, -1, -1 }, //  23 I
{ 351, 117, 17, 19, -1, -1, -1 }, //  24 O
{ 380, 117, 17, 19, -1, -1, -1 }, //  25 P
{ 449, 116, 15, 48, -1, -1, -1 }, //  26 ENTER
{ 107, 149, 17, 19, -1, -1, -1 }, //  27 A
{ 139, 148, 17, 19, -1, -1, -1 }, //  28 S
{ 171, 148, 17, 19, -1, -1, -1 }, //  29 D
{ 203, 148, 17, 19, -1, -1, -1 }, //  30 F
{ 234, 147, 17, 19, -1, -1, -1 }, //  31 G
{ 266, 147, 17, 19, -1, -1, -1 }, //  32 H
{ 298, 147, 17, 19, -1, -1, -1 }, //  33 J
{ 328, 147, 17, 19, -1, -1, -1 }, //  34 K
{ 359, 146, 17, 19, -1, -1, -1 }, //  35 L
{ 388, 146, 17, 19, -1, -1, -1 }, //  36 ;
{ 418, 146, 17, 19, -1, -1, -1 }, //  37 '
{ 123, 178, 17, 19, -1, -1, -1 }, //  38 Z
{ 155, 178, 17, 19, -1, -1, -1 }, //  39 X
{ 187, 178, 17, 19, -1, -1, -1 }, //  40 C
{ 219, 177, 17, 19, -1, -1, -1 }, //  41 V
{ 252, 177, 17, 19, -1, -1, -1 }, //  42 B
{ 282, 177, 17, 19, -1, -1, -1 }, //  43 N
{ 315, 176, 17, 19, -1, -1, -1 }, //  44 M
{ 346, 176, 17, 19, -1, -1, -1 }, //  45 ,
{ 377, 175, 17, 19, -1, -1, -1 }, //  46 .
{ 408, 175, 17, 19, -1, -1, -1 }, //  47 /
{ 178, 207,181, 22, -1, -1, -1 }, //  48 space
{  75, 178, 34, 19, -1, -1, -1 }, //  49 LShft
{ 438, 175, 34, 19, -1, -1, -1 }, //  50 RShft
{  60, 149, 34, 19, -1, -1, -1 }, //  51 CL
}, { // 24 Astro Wars
{ 380,  20, 11, 11,  0,  2,  0 }, // 0 Fire
{ 434,  20, 11, 11, -1, -1, -1 }, // 1 1UP
{ 504,  20, 11, 11, -1, -1, -1 }, // 2 2UP
{ 128,  20, 11, 11, -1, 19,  0 }, // 3 left
{ 178,  20, 11, 11, -1, 20,  0 }, // 4 right
{   0,   9,101,102, -1, -1, -1 }, // 5 instructions
}, { // 25 Galaxia
{ 286,  21, 11, 10,  0,  2,  0 }, // 0 Fire
{ 368,  21, 11, 10, -1, -1, -1 }, // 1 1UP
{ 424,  21, 11, 10, -1, -1, -1 }, // 2 2UP
{ 129,  20, 11, 10, -1, 19,  0 }, // 3 left
{ 167,  20, 11, 10, -1, 20,  0 }, // 4 right
{  18,   9, 88, 95, -1, -1, -1 }, // 5 instructions
}, { // 26 Laser Battle & Lazarian
{ 284,  50, 13, 11, 20,  7,  0 }, //  0 left button
{ 392,  31, 10, 10, -1, -1, -1 }, //  1 1UP
{ 442,  31, 10, 10, -1, -1, -1 }, //  2 2UP
{ 159,  75, 10, 10, -1, 19,  0 }, //  3 left stick
{ 200,  75, 10, 10, -1, 20,  0 }, //  4 right stick
{ 180,  55, 10, 10, -1, 17,  0 }, //  5 up stick
{ 180,  95, 10, 10, -1, 18,  0 }, //  6 down stick
{ 307,  68, 14, 11, 21,  8,  0 }, //  7 down button
{ 328,  49, 14, 11, 22,  9,  0 }, //  8 right button
{ 304,  33, 14, 11, 15,  5,  0 }, //  9 up button
{  11,  18,126,106, -1, 19,  0 }, // 10 instructions
}, { // 27 ELEKTOR (big)
{  68, 149, 60, 80, -1, -1, -1 }, // START
{  68,  65, 60, 80, -1, -1, -1 }, // UC
{  68, 233, 60, 80, -1, -1, -1 }, // LC
{  68, 317, 60, 79, -1, -1, -1 }, // RESET
{ 190,  65, 60, 80,  8,  1,  0 }, // RCAS
{ 190, 149, 60, 80, 14,  4,  0 }, // BP1/2
{ 190, 233, 60, 80, 20,  7,  0 }, // PC
{ 190, 317, 60, 79, 26, 10,  0 }, // -
{ 250,  65, 60, 80,  0,  2,  0 }, // WCAS
{ 250, 149, 60, 80, 15,  5,  0 }, // REG
{ 250, 233, 60, 80, 21,  8,  0 }, // MEM
{ 250, 317, 60, 79, 27, 11,  0 }, // +
{ 310,  65, 60, 80, 10,  3,  0 }, // C
{ 310, 149, 60, 80, 16,  6,  0 }, // 8
{ 310, 233, 60, 80, 22,  9,  0 }, // 4
{ 310, 317, 60, 79, 28, 12,  0 }, // 0
{ 470,  62, 60, 80, 11,  1,  1 }, // D
{ 469, 146, 60, 80, 17,  4,  1 }, // 9
{ 468, 230, 60, 80, 23,  7,  1 }, // 5
{ 468, 314, 60, 78, 29, 10,  1 }, // 1
{ 530,  62, 60, 80,  1,  2,  1 }, // E
{ 529, 146, 60, 80, 18,  5,  1 }, // A
{ 528, 230, 60, 80, 24,  8,  1 }, // 6
{ 528, 314, 60, 78, 30, 11,  1 }, // 2
{ 590,  62, 60, 80, 13,  3,  1 }, // F
{ 589, 146, 60, 80, 19,  6,  1 }, // B
{ 588, 230, 60, 79, 25,  9,  1 }, // 7
{ 588, 313, 60, 77, 31, 12,  1 }, // 3
{  -1,  -1, -1, -1, -1, -1,  0 }, // p1 up
{  -1,  -1, -1, -1, -1, -1,  0 }, // p1 left
{  -1,  -1, -1, -1, -1, -1,  0 }, // p1 down
{  -1,  -1, -1, -1, -1, -1,  0 }, // p1 right
{  -1,  -1, -1, -1, -1, -1,  1 }, // p2 up
{  -1,  -1, -1, -1, -1, -1,  1 }, // p2 left
{  -1,  -1, -1, -1, -1, -1,  1 }, // p2 down
{  -1,  -1, -1, -1, -1, -1,  1 }, // p2 right
}, { // 28 (PIPBUG) (VT100)
// 1st row
{    546-7,  12, 28, 18 }, //  0 up
{    590-7,  12, 28, 18 }, //  1 down
{    627  ,  12, 29, 18 }, //  2 left
{    670  ,  12, 28, 18 }, //  3 right
// 2nd row
{     61-7,  59, 30, 23 }, //  4 Esc
{    103-7,  59, 30, 23 }, //  5 1 !
{    147-7,  59, 30, 23 }, //  6 2 "
{    191-7,  59, 30, 23 }, //  7 3 #
{    236-7,  59, 30, 23 }, //  8 4 $
{    281-7,  59, 30, 23 }, //  9 5 %
{    314  ,  59, 31, 25 }, // 10 6 ^
{    358  ,  59, 31, 25 }, // 11 7 &
{    411-7,  59, 29, 24 }, // 12 8 *
{    448  ,  59, 29, 24 }, // 13 9 (
{    493  ,  60, 29, 23 }, // 14 0 )
{    538  ,  60, 29, 23 }, // 15 - _
{    581  ,  59, 29, 24 }, // 16 = +
{    634-7,  59, 30, 23 }, // 17 ` ~
{    678-7,  59, 30, 22 }, // 18 backspace
{    723-7,  59, 30, 22 }, // 19 break
{    812  ,  59, 30, 23 }, // 20 numeric 7
{    865-7,  59, 30, 23 }, // 21 numeric 8
{    910-7,  59, 30, 23 }, // 22 numeric 9
{    955-7,  59, 30, 23 }, // 23 numeric -
// 3rd row
{     45  , 108, 56, 22 }, // 24 Tab
{    120-7, 107, 29, 23 }, // 25 Q
{    171-7, 107, 29, 23 }, // 26 W
{    211-7, 107, 29, 23 }, // 27 E
{    257-7, 107, 29, 23 }, // 28 R
{    300-7, 108, 29, 22 }, // 29 T
{    345-7, 108, 29, 22 }, // 30 Y
{    383  , 108, 29, 23 }, // 31 U
{    428  , 108, 29, 23 }, // 32 I
{    472  , 108, 29, 23 }, // 33 O
{    519  , 108, 28, 23 }, // 34 P
{    570-7, 109, 29, 22 }, // 35 [ {
{    615-7, 109, 29, 22 }, // 36 ] }
{    629-7, 111, 64, 63 }, // 37 CR
{    705-7, 110, 30, 22 }, // 38 Del
{    815  , 108, 30, 22 }, // 39 numeric 4
{    862  , 108, 30, 22 }, // 40 numeric 5
{    908  , 108, 30, 22 }, // 41 numeric 6
{    953  , 108, 30, 22 }, // 42 numeric ,
// 4th row
{     21-7, 153, 35, 20 }, // 43 CTRL
{     62  , 153, 32, 20 }, // 44 Caps Lock
{    141-7, 153, 30, 20 }, // 45 A
{    187-7, 153, 30, 20 }, // 46 S
{    230-7, 153, 30, 20 }, // 47 D
{    265  , 153, 30, 20 }, // 48 F
{    317-7, 153, 30, 20 }, // 49 G
{    357  , 153, 30, 20 }, // 50 H
{    402  , 153, 30, 20 }, // 51 J
{    445  , 154, 30, 20 }, // 52 K
{    490  , 154, 28, 20 }, // 53 L
{    534  , 154, 28, 20 }, // 54 ; :
{    579  , 154, 30, 20 }, // 55 ' "
{    700  , 154, 30, 21 }, // 56 \ |
{    825-7, 154, 30, 21 }, // 57 numeric 1
{    870-7, 154, 30, 21 }, // 58 numeric 2
{    920-7, 154, 30, 21 }, // 59 numeric 3
{    967-7, 155, 40, 60 }, // 60 numeric ENTER
// 5th row
{     17-7, 198, 34, 19 }, // 61 No Scroll
{     81-7, 198, 44, 18 }, // 62 Left SHIFT
{    155-7, 198, 30, 18 }, // 63 Z
{    206-7, 198, 30, 18 }, // 64 X
{    250-7, 198, 30, 18 }, // 65 C
{    295-7, 198, 30, 18 }, // 66 V
{    336  , 198, 29, 18 }, // 67 B
{    381  , 198, 28, 18 }, // 68 N
{    427  , 198, 28, 18 }, // 69 M
{    481-7, 198, 30, 18 }, // 70 , <
{    526-7, 198, 30, 18 }, // 71 . >
{    573-7, 198, 30, 18 }, // 72 / ?
{    637-7, 198, 42, 18 }, // 73 Right SHIFT
{    704  , 198, 29, 19 }, // 74 LF
{    829-7, 198, 72, 19 }, // 75 numeric 0
{    915  , 198, 32, 19 }, // 76 numeric .
// 6th row (spacebar)
{    148-7, 264,413, 23 }, // 77 spacebar
// other controls
{    522  ,  68, 25, 25 }, // 78 reset (this is hardcoded!)
}, { // 29 (PIPBUG) (Radio Bulletin)
// 1st row
{  12,      10, 32, 33 }, //  0 space
{  53,      10, 32, 33 }, //  1 !
{  94,      10, 32, 33 }, //  2 "
{ 135,      10, 32, 33 }, //  3 #
{ 176,      10, 32, 33 }, //  4 $
{ 229,      10, 32, 33 }, //  5 %
{ 270,      10, 32, 33 }, //  6 &
{ 311,      10, 32, 33 }, //  7 '
{ 352,      10, 32, 33 }, //  8 (
{ 393,      10, 32, 33 }, //  9 )
{ 446,      10, 32, 33 }, // 10 *
{ 487,      10, 32, 33 }, // 11 +
{ 528,      10, 32, 33 }, // 12 , `
{ 569,      10, 32, 33 }, // 13 -
{ 610,      10, 32, 33 }, // 14 .
{ 663,      10, 32, 33 }, // 15 /
{ 704,      10, 32, 33 }, // 16 LF
{ 745,      10, 32, 33 }, // 17 $0B
{ 786,      10, 32, 33 }, // 18 CR
{ 827,      10, 32, 33 }, // 19 BEL
// 2nd row
{  12,      52, 32, 33 }, // 20 0
{  53,      52, 32, 33 }, // 21 1
{  94,      52, 32, 33 }, // 22 2
{ 135,      52, 32, 33 }, // 23 3
{ 176,      52, 32, 33 }, // 24 4
{ 229,      52, 32, 33 }, // 25 5
{ 270,      52, 32, 33 }, // 26 6
{ 311,      52, 32, 33 }, // 27 7
{ 352,      52, 32, 33 }, // 28 8
{ 393,      52, 32, 33 }, // 29 9
{ 446,      52, 32, 33 }, // 30 :
{ 487,      52, 32, 33 }, // 31 ;
{ 528,      52, 32, 33 }, // 32 <
{ 569,      52, 32, 33 }, // 33 =
{ 610,      52, 32, 33 }, // 34 >
{ 663,      52, 32, 33 }, // 35 ?
{ 704,      52, 32, 33 }, // 36 $1A
{ 745,      52, 32, 33 }, // 37 $1B
{ 786,      52, 32, 33 }, // 38 $1D
{ 827,      52, 32, 33 }, // 39 $1F
// 3rd row
{  12,      94, 32, 33 }, // 40 @
{  53,      94, 32, 33 }, // 41 A
{  94,      94, 32, 33 }, // 42 B
{ 135,      94, 32, 33 }, // 43 C
{ 176,      94, 32, 33 }, // 44 D
{ 229,      94, 32, 33 }, // 45 E
{ 270,      94, 32, 33 }, // 46 F
{ 311,      94, 32, 33 }, // 47 G
{ 352,      94, 32, 33 }, // 48 H
{ 393,      94, 32, 33 }, // 49 I
{ 446,      94, 32, 33 }, // 50 J
{ 487,      94, 32, 33 }, // 51 K
{ 528,      94, 32, 33 }, // 52 L
{ 569,      94, 32, 33 }, // 53 M
{ 610,      94, 32, 33 }, // 54 N
{ 663,      94, 32, 33 }, // 55 O
{ 704,      94, 32, 33 }, // 56 $6A
{ 745,      94, 32, 33 }, // 57 $6B
{ 786,      94, 32, 33 }, // 58 $6D
{ 827,      94, 32, 33 }, // 59 $6F
// 4th row
{  12,     136, 32, 33 }, // 60 P
{  53,     136, 32, 33 }, // 61 Q
{  94,     136, 32, 33 }, // 62 R
{ 135,     136, 32, 33 }, // 63 S
{ 176,     136, 32, 33 }, // 64 T
{ 229,     136, 32, 33 }, // 65 U
{ 270,     136, 32, 33 }, // 66 V
{ 311,     136, 32, 33 }, // 67 W
{ 352,     136, 32, 33 }, // 68 X
{ 393,     136, 32, 33 }, // 69 Y
{ 446,     136, 32, 33 }, // 70 Z
{ 487,     136, 32, 33 }, // 71 [
{ 528,     136, 32, 33 }, // 72 backslash
{ 569,     136, 32, 33 }, // 73 ]
{ 610,     136, 32, 33 }, // 74 ^
{ 663,     136, 32, 33 }, // 75 _
{ 704,     136, 32, 33 }, // 76 $7A
{ 745,     136, 32, 33 }, // 77 $7B
{ 786,     136, 32, 33 }, // 78 $7D
{ 827,     136, 32, 33 }, // 79 DEL
} };
