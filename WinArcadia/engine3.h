EXPORT const STRPTR pongvarname[VARIANTS] =
{ "AY-3-8500/8550: Tennis",
  "AY-3-8500/8550: Soccer/Hockey",
  "AY-3-8500/8550: Handicap",
  "AY-3-8500/8550: Squash/Handball",
  "AY-3-8500/8550: Practice",
  "AY-3-8500/8550: Rifle #1",
  "AY-3-8500/8550: Rifle #2",
  "AY-3-8600: Tennis",
  "AY-3-8600: Hockey",
  "AY-3-8600: Soccer",
  "AY-3-8600: Squash",
  "AY-3-8600: Practice",
  "AY-3-8600: Gridball",
  "AY-3-8600: Basketball",
  "AY-3-8600: Basketball Practice",
  "AY-3-8600: 1 Player Target",
  "AY-3-8600: 2 Player Target"
};

EXPORT const struct KeyTableStruct keytable[16] =
{   { A_P1MIDDLEKEYS, 0x01, IE_P1MIDDLEKEYS, 0x10, "0",  "+/2"   , 27, 30, DAPTER_0 }, //  0
    { A_P1LEFTKEYS,   0x08, IE_P1LEFTKEYS,   0x80, "1",  "RCAS/D",  8, 11, DAPTER_1 }, //  1
    { A_P1MIDDLEKEYS, 0x08, IE_P1MIDDLEKEYS, 0x80, "2",  "WCAS/E",  0,  1, DAPTER_2 }, //  2
    { A_P1RIGHTKEYS,  0x08, IE_P1RIGHTKEYS,  0x80, "3",  "C/F"   , 10, 13, DAPTER_3 }, //  3
    { A_P1LEFTKEYS,   0x04, IE_P1LEFTKEYS,   0x40, "4",  "BP/9"  , 14, 17, DAPTER_4 }, //  4
    { A_P1MIDDLEKEYS, 0x04, IE_P1MIDDLEKEYS, 0x40, "5",  "REG/A" , 15, 18, DAPTER_5 }, //  5
    { A_P1RIGHTKEYS,  0x04, IE_P1RIGHTKEYS,  0x40, "6",  "8/B"   , 16, 19, DAPTER_6 }, //  6
    { A_P1LEFTKEYS,   0x02, IE_P1LEFTKEYS,   0x20, "7",  "PC/5"  , 20, 23, DAPTER_7 }, //  7
    { A_P1MIDDLEKEYS, 0x02, IE_P1MIDDLEKEYS, 0x20, "8",  "MEM/6" , 21, 24, DAPTER_8 }, //  8
    { A_P1RIGHTKEYS,  0x02, IE_P1RIGHTKEYS,  0x20, "9",  "4/7"   , 22, 25, DAPTER_9 }, //  9
    { A_P1LEFTKEYS,   0x01, IE_P1LEFTKEYS,   0x10, "Cl", "-/1"   , 26, 29, DAPTER_C }, // 10
    { A_P1RIGHTKEYS,  0x01, IE_P1RIGHTKEYS,  0x10, "En", "0/3"   , 28, 31, DAPTER_E }, // 11
    { A_P1PALLADIUM,  0x08, 0,                  0, "x1", "?"     ,  2,  5, 0        }, // 12
    { A_P1PALLADIUM,  0x04, 0,                  0, "x2", "?"     ,  3,  6, 0        }, // 13
    { A_P1PALLADIUM,  0x02, 0,                  0, "x3", "?"     ,  4,  7, 0        }, // 14
    { A_P1PALLADIUM,  0x01, 0,                  0, "x4", "?"     ,  9, 12, 0        }  // 15
};

/* Codes are:
A = Arcadia
I = Interton
E = Elektor
W = Astro Wars
G = Galaxia/Laser Battle/Lazarian
M = Malzak */

EXPORT struct MonitorStruct monitor[MONITORGADS] = {
{ A_CONSOLE,                  }, //   0 A----- ARCADIA_FIRSTMONGAD
{ A_GFXMODE,                  }, //     A-----
{ A_BGCOLLIDE,                }, //     A-----
{ A_P1PADDLE,                 }, //     A-----
{ A_P2PADDLE,                 }, //     A-----
{ A_PITCH,                    }, //   5 A-----
{ A_BGCOLOUR,                 }, //     A-----
{ A_SPRITECOLLIDE,            }, //     A-----
{ A_P1LEFTKEYS,               }, //     A-----
{ A_P2LEFTKEYS,               }, //     A-----
{ A_VOLUME,                   }, //  10 A-----
{ A_VSCROLL,                  }, //     A-----
{ A_SPRITES01CTRL,            }, //     A-----
{ A_P1MIDDLEKEYS,             }, //     A-----
{ A_P2MIDDLEKEYS,             }, //     A-----
{ A_CHARLINE,                 }, //     A-----
{ A_SPRITES23CTRL,            }, //     A-----
{ A_P1RIGHTKEYS,              }, //     A-----
{ A_P2RIGHTKEYS,              }, //     A-----
{ A_P1PALLADIUM,              }, //     A-----
{ A_P2PALLADIUM,              }, //  20 A-----
{ 0x18D0,                     }, //  21 A-----
{ 0x18D1,                     }, //     A-----
{ 0x18D2,                     }, //     A-----
{ 0x18D3,                     }, //     A-----
{ 0x18D4,                     }, //     A-----
{ 0x18D5,                     }, //     A-----
{ 0x18D6,                     }, //     A-----
{ 0x18D7,                     }, //     A-----
{ 0x18D8,                     }, //     A-----
{ 0x18D9,                     }, //  30 A-----
{ 0x18DA,                     }, //     A-----
{ 0x18DB,                     }, //     A-----
{ 0x18DC,                     }, //     A-----
{ 0x18DD,                     }, //     A-----
{ 0x18DE,                     }, //     A-----
{ 0x18DF,                     }, //     A-----
{ 0x18E0,                     }, //     A-----
{ 0x18E1,                     }, //     A-----
{ 0x18E2,                     }, //     A-----
{ 0x18E3,                     }, //  40 A-----
{ 0x18E4,                     }, //     A-----
{ 0x18E5,                     }, //     A-----
{ 0x18E6,                     }, //     A-----
{ 0x18E7,                     }, //     A-----
{ 0x18E8,                     }, //     A-----
{ 0x18E9,                     }, //     A-----
{ 0x18EA,                     }, //     A-----
{ 0x18EB,                     }, //     A-----
{ 0x18EC,                     }, //     A-----
{ 0x18ED,                     }, //  50 A-----
{ 0x18EE,                     }, //     A-----
{ 0x18EF,                     }, //     A-----
{ 0x18F8,                     }, //     A-----
{ 0x18F9,                     }, //     A-----
{ 0x18FA,                     }, //     A-----
{ 0x18FB,                     }, //     A-----
{ 0x1AD0,                     }, //     A-----
{ 0x1AD1,                     }, //     A-----
{ 0x1AD2,                     }, //     A-----
{ 0x1AD3,                     }, //  60 A-----
{ 0x1AD4,                     }, //     A-----
{ 0x1AD5,                     }, //     A-----
{ 0x1AD6,                     }, //     A-----
{ 0x1AD7,                     }, //     A-----
{ 0x1AD8,                     }, //     A-----
{ 0x1AD9,                     }, //     A-----
{ 0x1ADA,                     }, //     A-----
{ 0x1ADB,                     }, //     A-----
{ 0x1ADC,                     }, //     A-----
{ 0x1ADD,                     }, //  70 A-----
{ 0x1ADE,                     }, //     A-----
{ 0x1ADF,                     }, //     A-----
{ 0x1AE0,                     }, //     A-----
{ 0x1AE1,                     }, //     A-----
{ 0x1AE2,                     }, //     A-----
{ 0x1AE3,                     }, //     A-----
{ 0x1AE4,                     }, //     A-----
{ 0x1AE5,                     }, //     A-----
{ 0x1AE6,                     }, //     A-----
{ 0x1AE7,                     }, //  80 A-----
{ 0x1AE8,                     }, //     A-----
{ 0x1AE9,                     }, //     A-----
{ 0x1AEA,                     }, //     A-----
{ 0x1AEB,                     }, //     A-----
{ 0x1AEC,                     }, //     A-----
{ 0x1AED,                     }, //     A-----
{ 0x1AEE,                     }, //     A-----
{ 0x1AEF,                     }, //     A-----
{ 0x1AF0,                     }, //     A-----
{ 0x1AF1,                     }, //  90 A-----
{ 0x1AF2,                     }, //     A-----
{ 0x1AF3,                     }, //     A-----
{ 0x1AF4,                     }, //     A-----
{ 0x1AF5,                     }, //     A-----
{ 0x1AF6,                     }, //     A-----
{ 0x1AF7,                     }, //     A-----
{ 0x1AF8,                     }, //     A-----
{ 0x1AF9,                     }, //     A-----
{ 0x1AFA,                     }, //     A-----
{ 0x1AFB,                     }, // 100 A-----
{ 0x1AFC,                     }, //     A-----
{ 0x1AFD,                     }, //     A-----
{ 0x1AFE,                     }, //     A-----
{ 0x1AFF,                     }, //     A-----
{ A_SPRITE0X,                 }, // 105 A-----
{ A_SPRITE0Y,                 }, //     A-----
{ A_SPRITE1X,                 }, //     A-----
{ A_SPRITE1Y,                 }, //     A-----
{ A_SPRITE2X,                 }, //     A-----
{ A_SPRITE2Y,                 }, // 110 A-----
{ A_SPRITE3X,                 }, //     A-----
{ A_SPRITE3Y,                 }, // 112 A----- ARCADIA_LASTMONGAD
{ PVI_P1PADDLE,               }, // 113 -IEWGM PVI1ST_FIRSTMONGAD
{ PVI_P2PADDLE,               }, //     -IEWGM
{ PVI_PITCH,                  }, // 115 -IEWGM
{ PVI_SIZES,                  }, //     -IEWGM
{ PVI_SCORECTRL,              }, //     -IEWGM
{ PVI_SCORELT,                }, //     -IEWGM
{ PVI_SCORERT,                }, //     -IEWGM
{ PVI_BGCOLOUR,               }, // 120 -IEWGM
{ PVI_BGCOLLIDE,              }, //     -IEWGM
{ PVI_SPRITECOLLIDE,          }, //     -IEWGM
{ PVI_SPR01COLOURS,           }, //     -IEWGM
{ PVI_SPR23COLOURS,           }, //     -IEWGM
{ PVI_SPRITE0AX,              }, // 125 -IEWGM
{ PVI_SPRITE0AY,              }, //     -IEWGM
{ PVI_SPRITE1AX,              }, //     -IEWGM
{ PVI_SPRITE1AY,              }, //     -IEWGM
{ PVI_SPRITE2AX,              }, //     -IEWGM
{ PVI_SPRITE2AY,              }, // 130 -IEWGM
{ PVI_SPRITE3AX,              }, //     -IEWGM
{ PVI_SPRITE3AY,              }, //     -IEWGM
{ PVI_SPRITE0BX,              }, //     -IEWGM
{ PVI_SPRITE0BY,              }, //     -IEWGM
{ PVI_SPRITE1BX,              }, // 135 -IEWGM
{ PVI_SPRITE1BY,              }, //     -IEWGM
{ PVI_SPRITE2BX,              }, //     -IEWGM
{ PVI_SPRITE2BY,              }, //     -IEWGM
{ PVI_SPRITE3BX,              }, //     -IEWGM
{ PVI_SPRITE3BY,              }, // 140 -IEWGM PVI1ST_LASTMONGAD
{ E_CASIN,                    }, // 141 --E--- E_FIRSTMONGAD
{ E_CASOUT,                   }, // 142 --E---
{ IE_P1LEFTKEYS,              }, // 143 -IE--- I_FIRSTMONGAD
{ IE_P1MIDDLEKEYS,            }, //     -IE---
{ IE_P1RIGHTKEYS,             }, //     -IE---
{ IE_CONSOLE,                 }, //     -IE---
{ IE_P2LEFTKEYS,              }, //     -IE---
{ IE_P2MIDDLEKEYS,            }, //     -IE---
{ IE_P2RIGHTKEYS,             }, //     -IE---
{ IE_NOISE,                   }, // 150 -IE--- IE_LASTMONGAD
{ 0x1F0E,                     }, //     -I---- INTERTON_FIRSTMONGAD
{ 0x1F0F,                     }, //     -I----
{ 0x1F1E,                     }, //     -I----
{ 0x1F1F,                     }, //     -I----
{ 0x1F4E,                     }, // 155 -I----
{ 0x1F4F,                     }, //     -I----
{ 0x1F50,                     }, //     -I----
{ 0x1F51,                     }, //     -I----
{ 0x1F52,                     }, //     -I----
{ 0x1F53,                     }, // 160 -I----
{ 0x1F54,                     }, //     -I----
{ 0x1F55,                     }, //     -I----
{ 0x1F56,                     }, //     -I----
{ 0x1F57,                     }, //     -I----
{ 0x1F58,                     }, //     -I----
{ 0x1F59,                     }, //     -I----
{ 0x1F5A,                     }, //     -I----
{ 0x1F5B,                     }, //     -I----
{ 0x1F5C,                     }, //     -I----
{ 0x1F5D,                     }, // 170 -I----
{ 0x1F5E,                     }, //     -I----
{ 0x1F5F,                     }, //     -I----
{ 0x1F60,                     }, //     -I----
{ 0x1F61,                     }, //     -I----
{ 0x1F62,                     }, //     -I----
{ 0x1F63,                     }, //     -I----
{ 0x1F64,                     }, //     -I----
{ 0x1F65,                     }, //     -I----
{ 0x1F66,                     }, //     -I----
{ 0x1F67,                     }, // 180 -I----
{ 0x1F68,                     }, //     -I----
{ 0x1F69,                     }, //     -I----
{ 0x1F6A,                     }, //     -I----
{ 0x1F6B,                     }, //     -I----
{ 0x1F6C,                     }, //     -I----
{ 0x1F6D,                     }, //     -I----
{ 0x1FAD,                     }, // 187 -I---- INTERTON_LASTMONGAD
{ 0x100 + PVI_P1PADDLE,       }, // 188 ----GM PVI2ND_FIRSTMONGAD
{ 0x100 + PVI_P2PADDLE,       }, //     ----GM
{ 0x100 + PVI_PITCH,          }, // 190 ----GM
{ 0x100 + PVI_SIZES,          }, //     ----GM
{ 0x100 + PVI_SCORECTRL,      }, //     ----GM
{ 0x100 + PVI_SCORELT,        }, //     ----GM
{ 0x100 + PVI_SCORERT,        }, //     ----GM
{ 0x100 + PVI_BGCOLOUR,       }, //     ----GM
{ 0x100 + PVI_BGCOLLIDE,      }, //     ----GM
{ 0x100 + PVI_SPRITECOLLIDE,  }, //     ----GM
{ 0x100 + PVI_SPR01COLOURS,   }, //     ----GM
{ 0x100 + PVI_SPR23COLOURS,   }, //     ----GM
{ 0x100 + PVI_SPRITE0AX,      }, // 200 ----GM
{ 0x100 + PVI_SPRITE0AY,      }, //     ----GM
{ 0x100 + PVI_SPRITE1AX,      }, //     ----GM
{ 0x100 + PVI_SPRITE1AY,      }, //     ----GM
{ 0x100 + PVI_SPRITE2AX,      }, //     ----GM
{ 0x100 + PVI_SPRITE2AY,      }, //     ----GM
{ 0x100 + PVI_SPRITE3AX,      }, //     ----GM
{ 0x100 + PVI_SPRITE3AY,      }, //     ----GM
{ 0x100 + PVI_SPRITE0BX,      }, //     ----GM
{ 0x100 + PVI_SPRITE0BY,      }, //     ----GM
{ 0x100 + PVI_SPRITE1BX,      }, // 210 ----GM
{ 0x100 + PVI_SPRITE1BY,      }, //     ----GM
{ 0x100 + PVI_SPRITE2BX,      }, //     ----GM
{ 0x100 + PVI_SPRITE2BY,      }, //     ----GM
{ 0x100 + PVI_SPRITE3BX,      }, //     ----GM
{ 0x100 + PVI_SPRITE3BY,      }, // 215 ----GM PVI2ND_LASTMONGAD
{ 0x200 + PVI_P1PADDLE,       }, // 216 ----GM PVI3RD_FIRSTMONGAD
{ 0x200 + PVI_P2PADDLE,       }, //     ----GM
{ 0x200 + PVI_PITCH,          }, //     ----GM
{ 0x200 + PVI_SIZES,          }, //     ----GM
{ 0x200 + PVI_SCORECTRL,      }, // 220 ----GM
{ 0x200 + PVI_SCORELT,        }, //     ----GM
{ 0x200 + PVI_SCORERT,        }, //     ----GM
{ 0x200 + PVI_BGCOLOUR,       }, //     ----GM
{ 0x200 + PVI_BGCOLLIDE,      }, //     ----GM
{ 0x200 + PVI_SPRITECOLLIDE,  }, //     ----GM
{ 0x200 + PVI_SPR01COLOURS,   }, //     ----GM
{ 0x200 + PVI_SPR23COLOURS,   }, //     ----GM
{ 0x200 + PVI_SPRITE0AX,      }, //     ----GM
{ 0x200 + PVI_SPRITE0AY,      }, //     ----GM
{ 0x200 + PVI_SPRITE1AX,      }, // 230 ----GM
{ 0x200 + PVI_SPRITE1AY,      }, //     ----GM
{ 0x200 + PVI_SPRITE2AX,      }, //     ----GM
{ 0x200 + PVI_SPRITE2AY,      }, //     ----GM
{ 0x200 + PVI_SPRITE3AX,      }, //     ----GM
{ 0x200 + PVI_SPRITE3AY,      }, //     ----GM
{ 0x200 + PVI_SPRITE0BX,      }, //     ----GM
{ 0x200 + PVI_SPRITE0BY,      }, //     ----GM
{ 0x200 + PVI_SPRITE1BX,      }, //     ----GM
{ 0x200 + PVI_SPRITE1BY,      }, //     ----GM
{ 0x200 + PVI_SPRITE2BX,      }, // 240 ----GM
{ 0x200 + PVI_SPRITE2BY,      }, //     ----GM
{ 0x200 + PVI_SPRITE3BX,      }, //     ----GM
{ 0x200 + PVI_SPRITE3BY,      }, // 243 ----GM PVI3RD_LASTMONGAD
{ PSG_AMPLITUDEA1,            }, // 244 --E--- PSGS_FIRSTMONGAD
{ PSG_AMPLITUDEB1,            }, //     --E---
{ PSG_AMPLITUDEC1,            }, //     --E---
{ PSG_PITCHD1,                }, //     --E---
{ PSG_MIXER1,                 }, //     --E---
{ PSG_SHAPE1,                 }, //     --E---
{ PSG_PORTA1,                 }, // 250 --E---
{ PSG_PORTB1,                 }, //     --E---
{ PSG_AMPLITUDEA2,            }, //     --E---
{ PSG_AMPLITUDEB2,            }, //     --E---
{ PSG_AMPLITUDEC2,            }, //     --E---
{ PSG_PITCHD2,                }, //     --E---
{ PSG_MIXER2,                 }, //     --E---
{ PSG_SHAPE2,                 }, //     --E---
{ PSG_PORTA2,                 }, //     --E---
{ PSG_PORTB2,                 }, // 259 --E---
{ PSG_PITCHA1_L,              }, // 260 (FIRSTDOUBLEBYTEPSG)
{ PSG_PITCHA2_L,              },
{ PSG_PITCHB1_L,              },
{ PSG_PITCHB2_L,              },
{ PSG_PITCHC1_L,              },
{ PSG_PITCHC2_L,              },
{ PSG_PERIOD1_L,              },
{ PSG_PERIOD2_L,              }, // 267 (LASTDDOUBLEBYTEPSG) (PSGS_LASTMONGAD)
{ TOKEN_R0,                   }, // 268 (CPU_FIRSTMONGAD)
{ TOKEN_R1,                   },
{ TOKEN_R2,                   }, // 270
{ TOKEN_R3,                   },
{ TOKEN_R4,                   },
{ TOKEN_R5,                   },
{ TOKEN_R6,                   },
{ TOKEN_RAS0,                 },
{ TOKEN_RAS1,                 },
{ TOKEN_RAS2,                 },
{ TOKEN_RAS3,                 },
{ TOKEN_RAS4,                 },
{ TOKEN_RAS5,                 }, // 280
{ TOKEN_RAS6,                 },
{ TOKEN_RAS7,                 },
{ OUTOFRANGE,                 }, //     TOKEN_S
{ OUTOFRANGE,                 }, //     TOKEN_F
{ OUTOFRANGE,                 }, //     TOKEN_II
{ OUTOFRANGE,                 }, //     TOKEN_USERFLAG1
{ OUTOFRANGE,                 }, //     TOKEN_USERFLAG2
{ TOKEN_SP,                   },
{ TOKEN_CC,                   },
{ OUTOFRANGE,                 }, // 290 TOKEN_IDC
{ OUTOFRANGE,                 }, //     TOKEN_RS
{ OUTOFRANGE,                 }, //     TOKEN_WC
{ OUTOFRANGE,                 }, //     TOKEN_OVF
{ OUTOFRANGE,                 }, //     TOKEN_COM
{ OUTOFRANGE,                 }, //     TOKEN_C
{ TOKEN_IAR,                  },
{ OUTOFRANGE,                 }, //     TOKEN_OPCODE
{ TOKEN_R0,                   },
{ TOKEN_R1,                   },
{ TOKEN_R2,                   }, // 300
{ TOKEN_R3,                   },
{ TOKEN_R4,                   },
{ TOKEN_R5,                   },
{ TOKEN_R6,                   },
{ TOKEN_RAS0,                 },
{ TOKEN_RAS1,                 },
{ TOKEN_RAS2,                 },
{ TOKEN_RAS3,                 },
{ TOKEN_RAS4,                 },
{ TOKEN_RAS5,                 }, // 310
{ TOKEN_RAS6,                 },
{ TOKEN_RAS7,                 },
{ OUTOFRANGE,                 }, //     TOKEN_S
{ OUTOFRANGE,                 }, //     TOKEN_F
{ OUTOFRANGE,                 }, //     TOKEN_II
{ OUTOFRANGE,                 }, //     TOKEN_USERFLAG1
{ OUTOFRANGE,                 }, //     TOKEN_USERFLAG2
{ TOKEN_SP,                   },
{ TOKEN_CC,                   },
{ OUTOFRANGE,                 }, // 320 TOKEN_IDC
{ OUTOFRANGE,                 }, //     TOKEN_RS
{ OUTOFRANGE,                 }, //     TOKEN_WC
{ OUTOFRANGE,                 }, //     TOKEN_OVF
{ OUTOFRANGE,                 }, //     TOKEN_COM
{ OUTOFRANGE,                 }, //     TOKEN_C
{ TOKEN_IAR,                  },
{ OUTOFRANGE,                 }, // 327 TOKEN_OPCODE
{ OUTOFRANGE,                 }, // 328 TOKEN_CLOCK
{ OUTOFRANGE,                 }, //     TOKEN_FRAME
{ OUTOFRANGE,                 }, // 330 TOKEN_TIME
{ OUTOFRANGE,                 }, //     TOKEN_X
{ OUTOFRANGE,                 }, //     TOKEN_Y
{ OUTOFRANGE,                 }, //     TOKEN_WPM_TR
{ OUTOFRANGE,                 }, //     TOKEN_ACCURACY
{ OUTOFRANGE,                 }, //     TOKEN_ERRORS
{ OUTOFRANGE,                 }, // 336 TOKEN_EXPECTING
};

EXPORT struct NoteStruct notes[NOTES + 1] = {
{ "A0" , HZ_A0 ,   8,  10,   8,  11, EMUBRUSH_WHITE,    0,    0, STAVE_A2, FALSE }, //  0
{ "A#0", HZ_AS0,  11,  13,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_A2, TRUE  },
{ "B0" , HZ_B0 ,  14,  16,  13,  16, EMUBRUSH_WHITE, 0xFE, 0xFC, STAVE_B2, FALSE },
{ "C1" , HZ_C1 ,  18,  20,  18,  21, EMUBRUSH_WHITE, 0xF0, 0xEE, STAVE_C3, FALSE },
{ "C#1", HZ_CS1,  21,  23,   0,   0, EMUBRUSH_BLACK, 0xE2, 0xE0, STAVE_C3, TRUE  },
{ "D1" , HZ_D1 ,  24,  25,  23,  26, EMUBRUSH_WHITE, 0xD6, 0xD4, STAVE_D3, FALSE },
{ "D#1", HZ_DS1,  26,  28,   0,   0, EMUBRUSH_BLACK, 0xC9, 0xC8, STAVE_D3, TRUE  },
{ "E1" , HZ_E1 ,  29,  31,  28,  31, EMUBRUSH_WHITE, 0xBE, 0xBD, STAVE_E3, FALSE },
{ "F1" , HZ_F1 ,  33,  35,  33,  36, EMUBRUSH_WHITE, 0xB3, 0xB2, STAVE_F3, FALSE },
{ "F#1", HZ_FS1,  36,  38,   0,   0, EMUBRUSH_BLACK, 0xA9, 0xA8, STAVE_F3, TRUE  },
{ "G1" , HZ_G1 ,  39,  40,  38,  41, EMUBRUSH_WHITE, 0xA0, 0x9E, STAVE_G2, FALSE },
{ "G#1", HZ_GS1,  41,  43,   0,   0, EMUBRUSH_BLACK, 0x97, 0x95, STAVE_G2, TRUE  },
{ "A1" , HZ_A1 ,  44,  45,  43,  46, EMUBRUSH_WHITE, 0x8E, 0x8D, STAVE_A2, FALSE },
{ "A#1", HZ_AS1,  46,  48,   0,   0, EMUBRUSH_BLACK, 0x86, 0x85, STAVE_A2, TRUE  },
{ "B1" , HZ_B1 ,  49,  51,  48,  51, EMUBRUSH_WHITE, 0x7E, 0x7E, STAVE_B2, FALSE },
{ "C2" , HZ_C2 ,  53,  55,  53,  56, EMUBRUSH_WHITE, 0x77, 0x76, STAVE_C3, FALSE },
{ "C#2", HZ_CS2,  56,  58,   0,   0, EMUBRUSH_BLACK, 0x70, 0x70, STAVE_C3, TRUE  },
{ "D2" , HZ_D2 ,  59,  60,  58,  61, EMUBRUSH_WHITE, 0x6B, 0x69, STAVE_D3, FALSE },
{ "D#2", HZ_DS2,  61,  63,   0,   0, EMUBRUSH_BLACK, 0x64, 0x64, STAVE_D3, TRUE  },
{ "E2" , HZ_E2 ,  64,  66,  63,  66, EMUBRUSH_WHITE, 0x5F, 0x5E, STAVE_E3, FALSE },
{ "F2" , HZ_F2 ,  68,  70,  68,  71, EMUBRUSH_WHITE, 0x59, 0x58, STAVE_F3, FALSE },
{ "F#2", HZ_FS2,  71,  73,   0,   0, EMUBRUSH_BLACK, 0x54, 0x53, STAVE_F3, TRUE  },
{ "G2" , HZ_G2 ,  74,  75,  73,  76, EMUBRUSH_WHITE, 0x4F, 0x4F, STAVE_G2, FALSE }, // lowest "untransposed" on stave
{ "G#2", HZ_GS2,  76,  78,   0,   0, EMUBRUSH_BLACK, 0x4B, 0x4A, STAVE_G2, TRUE  },
{ "A2" , HZ_A2 ,  79,  80,  78,  81, EMUBRUSH_WHITE, 0x46, 0x46, STAVE_A2, FALSE },
{ "A#2", HZ_AS2,  81,  83,   0,   0, EMUBRUSH_BLACK, 0x42, 0x42, STAVE_A2, TRUE  },
{ "B2" , HZ_B2 ,  84,  86,  83,  86, EMUBRUSH_WHITE, 0x3F, 0x3F, STAVE_B2, FALSE },
{ "C3" , HZ_C3 ,  88,  90,  88,  91, EMUBRUSH_WHITE, 0x3B, 0x3B, STAVE_C3, FALSE },
{ "C#3", HZ_CS3,  91,  93,   0,   0, EMUBRUSH_BLACK, 0x38, 0x37, STAVE_C3, TRUE  },
{ "D3" , HZ_D3 ,  94,  95,  93,  96, EMUBRUSH_WHITE, 0x35, 0x34, STAVE_D3, FALSE },
{ "D#3", HZ_DS3,  96,  98,   0,   0, EMUBRUSH_BLACK, 0x31, 0x31, STAVE_D3, TRUE  },
{ "E3" , HZ_E3 ,  99, 101,  98, 101, EMUBRUSH_WHITE, 0x2F, 0x2E, STAVE_E3, FALSE },
{ "F3" , HZ_F3 , 103, 105, 103, 106, EMUBRUSH_WHITE, 0x2C, 0x2C, STAVE_F3, FALSE },
{ "F#3", HZ_FS3, 106, 108,   0,   0, EMUBRUSH_BLACK, 0x2A, 0x29, STAVE_F3, TRUE  },
{ "G3" , HZ_G3 , 109, 110, 108, 111, EMUBRUSH_WHITE, 0x27, 0x27, STAVE_G3, FALSE },
{ "G#3", HZ_GS3, 111, 113,   0,   0, EMUBRUSH_BLACK, 0x25, 0x25, STAVE_G3, TRUE  },
{ "A3" , HZ_A3 , 114, 115, 113, 116, EMUBRUSH_WHITE, 0x23, 0x23, STAVE_A3, FALSE },
{ "A#3", HZ_AS3, 116, 118,   0,   0, EMUBRUSH_BLACK, 0x21, 0x21, STAVE_A3, TRUE  },
{ "B3" , HZ_B3 , 119, 121, 118, 121, EMUBRUSH_WHITE, 0x1F, 0x1F, STAVE_B3, FALSE },
{ "C4" , HZ_C4 , 123, 125, 123, 126, EMUBRUSH_WHITE, 0x1D, 0x1D, STAVE_C4, FALSE },
{ "C#4", HZ_CS4, 126, 128,   0,   0, EMUBRUSH_BLACK, 0x1B, 0x1B, STAVE_C4, TRUE  }, // $1C instead of $1B on Interton/Elektor
{ "D4" , HZ_D4 , 129, 130, 128, 131, EMUBRUSH_WHITE, 0x1A, 0x1A, STAVE_D4, FALSE },
{ "D#4", HZ_DS4, 131, 133,   0,   0, EMUBRUSH_BLACK, 0x18, 0x18, STAVE_D4, TRUE  }, // $19 instead of $18 on Interton/Elektor
{ "E4" , HZ_E4 , 134, 136, 133, 136, EMUBRUSH_WHITE, 0x17, 0x17, STAVE_E4, FALSE },
{ "F4" , HZ_F4 , 138, 140, 138, 141, EMUBRUSH_WHITE, 0x15, 0x15, STAVE_F4, FALSE },
{ "F#4", HZ_FS4, 141, 143,   0,   0, EMUBRUSH_BLACK, 0x14, 0x14, STAVE_F4, TRUE  },
{ "G4" , HZ_G4 , 144, 145, 143, 146, EMUBRUSH_WHITE, 0x13, 0x13, STAVE_G4, FALSE },
{ "G#4", HZ_GS4, 146, 148,   0,   0, EMUBRUSH_BLACK, 0x12, 0x12, STAVE_G4, TRUE  },
{ "A4" , HZ_A4 , 149, 150, 148, 151, EMUBRUSH_WHITE, 0x11, 0x11, STAVE_A4, FALSE },
{ "A#4", HZ_AS4, 151, 153,   0,   0, EMUBRUSH_BLACK, 0x10, 0x10, STAVE_A4, TRUE  },
{ "B4" , HZ_B4 , 154, 156, 153, 156, EMUBRUSH_WHITE, 0x0F, 0x0F, STAVE_B4, FALSE }, //  50
{ "C5" , HZ_C5 , 158, 160, 158, 161, EMUBRUSH_WHITE, 0x0E, 0x0E, STAVE_C5, FALSE },
{ "C#5", HZ_CS5, 161, 163,   0,   0, EMUBRUSH_BLACK, 0x0D, 0x0D, STAVE_C5, TRUE  },
{ "D5" , HZ_D5 , 164, 165, 163, 166, EMUBRUSH_WHITE,    0,    0, STAVE_D5, FALSE },
{ "D#5", HZ_DS5, 166, 168,   0,   0, EMUBRUSH_BLACK, 0x0C, 0x0C, STAVE_D5, TRUE  },
{ "E5" , HZ_E5 , 169, 171, 168, 171, EMUBRUSH_WHITE, 0x0B, 0x0B, STAVE_E5, FALSE },
{ "F5" , HZ_F5 , 173, 175, 173, 176, EMUBRUSH_WHITE, 0x0A, 0x0A, STAVE_F5, FALSE },
{ "F#5", HZ_FS5, 176, 178,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_F5, TRUE  }, // highest "untransposed" on stave
{ "G5" , HZ_G5 , 179, 180, 178, 181, EMUBRUSH_WHITE, 0x09, 0x09, STAVE_G4, FALSE },
{ "G#5", HZ_GS5, 181, 183,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_G4, TRUE  },
{ "A5" , HZ_A5 , 184, 185, 183, 186, EMUBRUSH_WHITE, 0x08, 0x08, STAVE_A4, FALSE },
{ "A#5", HZ_AS5, 186, 188,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_A4, TRUE  },
{ "B5" , HZ_B5 , 189, 191, 188, 191, EMUBRUSH_WHITE, 0x07, 0x07, STAVE_B4, FALSE },
{ "C6" , HZ_C6 , 193, 195, 193, 196, EMUBRUSH_WHITE,    0,    0, STAVE_C5, FALSE },
{ "C#6", HZ_CS6, 196, 198,   0,   0, EMUBRUSH_BLACK, 0x06, 0x06, STAVE_C5, TRUE  },
{ "D6" , HZ_D6 , 199, 200, 198, 201, EMUBRUSH_WHITE,    0,    0, STAVE_D5, FALSE },
{ "D#6", HZ_DS6, 201, 203,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_D5, TRUE  },
{ "E6" , HZ_E6 , 204, 206, 203, 206, EMUBRUSH_WHITE, 0x05, 0x05, STAVE_E5, FALSE },
{ "F6" , HZ_F6 , 208, 210, 208, 211, EMUBRUSH_WHITE,    0,    0, STAVE_F5, FALSE },
{ "F#6", HZ_FS6, 211, 213,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_F5, TRUE  },
{ "G6" , HZ_G6 , 214, 215, 213, 216, EMUBRUSH_WHITE, 0x04, 0x04, STAVE_G4, FALSE },
{ "G#6", HZ_GS6, 216, 218,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_G4, TRUE  },
{ "A6" , HZ_A6 , 219, 220, 218, 221, EMUBRUSH_WHITE,    0,    0, STAVE_A4, FALSE },
{ "A#6", HZ_AS6, 221, 223,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_A4, TRUE  },
{ "B6" , HZ_B6 , 224, 226, 223, 226, EMUBRUSH_WHITE, 0x03, 0x03, STAVE_B4, FALSE },
{ "C7" , HZ_C7 , 228, 230, 228, 231, EMUBRUSH_WHITE,    0,    0, STAVE_C5, FALSE },
{ "C#7", HZ_CS7, 231, 233,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_C5, TRUE  },
{ "D7" , HZ_D7 , 234, 235, 233, 236, EMUBRUSH_WHITE,    0,    0, STAVE_D5, FALSE },
{ "D#7", HZ_DS7, 236, 238,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_D5, TRUE  },
{ "E7" , HZ_E7 , 239, 241, 238, 241, EMUBRUSH_WHITE, 0x02, 0x02, STAVE_E5, FALSE },
{ "F7" , HZ_F7 , 243, 245, 243, 246, EMUBRUSH_WHITE,    0,    0, STAVE_F5, FALSE },
{ "F#7", HZ_FS7, 246, 248,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_F5, TRUE  },
{ "G7" , HZ_G7 , 249, 250, 248, 251, EMUBRUSH_WHITE,    0,    0, STAVE_G4, FALSE },
{ "G#7", HZ_GS7, 251, 253,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_G4, TRUE  },
{ "A7" , HZ_A7 , 254, 255, 253, 256, EMUBRUSH_WHITE,    0,    0, STAVE_A4, FALSE },
{ "A#7", HZ_AS7, 256, 258,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_A4, TRUE  },
{ "B7" , HZ_B7 , 259, 261, 258, 261, EMUBRUSH_WHITE, 0x01, 0x01, STAVE_B4, FALSE },
{ "C8" , HZ_C8 , 263, 265, 263, 266, EMUBRUSH_WHITE,    0,    0, STAVE_C5, FALSE },
{ "C#8", HZ_CS8, 266, 268,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_C5, TRUE  },
{ "D8" , HZ_D8 , 269, 270, 268, 271, EMUBRUSH_WHITE,    0,    0, STAVE_D5, FALSE },
{ "D#8", HZ_DS8, 271, 273,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_D5, TRUE  },
{ "E8" , HZ_E8 , 274, 276, 273, 276, EMUBRUSH_WHITE,    0,    0, STAVE_E5, FALSE },
{ "F8" , HZ_F8 , 278, 280, 278, 281, EMUBRUSH_WHITE,    0,    0, STAVE_F5, FALSE },
{ "F#8", HZ_FS8, 281, 283,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_F5, TRUE  },
{ "G8" , HZ_G8 , 284, 285, 283, 286, EMUBRUSH_WHITE,    0,    0, STAVE_G4, FALSE },
{ "G#8", HZ_GS8, 286, 288,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_G4, TRUE  },
{ "A8" , HZ_A8 , 289, 290, 288, 291, EMUBRUSH_WHITE,    0,    0, STAVE_A4, FALSE },
{ "A#8", HZ_AS8, 291, 293,   0,   0, EMUBRUSH_BLACK,    0,    0, STAVE_A4, TRUE  },
{ "B8" , HZ_B8 , 294, 296, 293, 296, EMUBRUSH_WHITE,    0,    0, STAVE_B4, FALSE },
{ "C9" , HZ_C9 , 298, 301, 298, 301, EMUBRUSH_WHITE,    0,    0, STAVE_C5, FALSE },
{ "rest",HZ_REST,  0,   0,   0,   0, 0             ,    0,    0,        0, 0     }, // 100
};

EXPORT const struct TODStruct tods[TIPSOFDAYS] = {
{ MSG_TOD20,  "\"Help|Host keyboard...\" (for keyboard users) and \"Help|Host"                \
              " gamepads/mouse...\" (for gamepad/mouse users) are the easiest ways to see"    \
              " what keys to use for your game. \"Help|Game information...\" may also be"     \
              " useful."                                                                      },
{ MSG_TOD1,   "You can change the selected game in the sidebar with Ctrl+Up/Down."            },
{ MSG_TOD2,   "In the memory editor and real-time monitor, you can click with the"            \
              " middle mouse button on most buttons to set or clear a data watchpoint."       },
{ MSG_TOD3,   "In the memory editor and real-time monitor, you can click with the"            \
              " left mouse button on most buttons to edit (poke) their contents."             },
{ MSG_TOD4,   "In the controls subwindow, you can control the machine by clicking"            \
              " with the left mouse button on the relevant part of the picture."              },
{ MSG_TOD5,   "In the debugger, typing = will show a list of all known labels."               },
{ MSG_TOD6,   "In the debugger, you can use the Tab key to autocomplete a label"              \
              " that you are typing."                                                         },
{ MSG_TOD7,   "In the memory editor subwindow, you can click on the memory map to"            \
              " change the view to the appropriate memory region."                            },
{ MSG_TOD8,   "You can drag the right edge of the window to enlarge the width of the"         \
              " sidebar."                                                                     },
{ MSG_TOD9,   "Shift+Tab will activate the debugger gadget."                                  },
{ MSG_TOD10,  "In the debugger, pressing the cursor up/down keys will allow access to"        \
              " the previous twenty commands entered."                                        },
{ MSG_TOD11,  "Holding the Ctrl key down shows various information on the status bar"         \
              " about what is under the mouse pointer."                                       },
{ MSG_TOD12,  "Holding the Ctrl and Left Shift keys down shows a magnified view of"           \
              " what is under the mouse pointer."                                             },
// spare
{ MSG_TOD14,  "You can set a masked watchpoint in the debugger by specifying"                 \
              " WP <address>:<mask> , eg. WP $1234:56 ."                                      },
{ MSG_TOD15,  "In the debugger, giving a ? after a command will tell the usage, for"          \
              " most commands. Eg. REL ? to see the usage of the REL command."                },
{ MSG_TOD16,  "You can centre the paddles with Alt+C."                                        },
{ MSG_TOD17,  "When generating an IFF ANIM, if \"Macros|Generate IFF 8SVX files?\" is"        \
              " also on, the generated IFF ANIM will also have embedded sound data."          },
{ MSG_TOD18,  "In the sprite editor, double-clicking on a colour will open the"               \
              " palette editor."                                                              },
{ MSG_TOD19,  "Ctrl+' will make the emulator run at 1 FPS while held down."                   },
// MSG_TOD20 is at the start of the array
#ifdef WIN32
{ MSG_TODW1,  "You can drag and drop a file onto the main window to load it, or drag and"     \
              " drop a directory onto the main window to change the sidebar to that"          \
              " directory."                                                                   },
{ MSG_TODW2,  "In 3D mode, you can move the camera with Ctrl+PgUp/PgDn, and zoom in/out"      \
              " with Ctrl+Home/End."                                                          },
{ MSG_TODW3,  "While browsing the Debug submenus, the status bar shows a description of"      \
              " each command as you hover over it."                                           },
{ MSG_TODW4,  "Clicking on the eye candy will toggle it on or off."                           },
{ MSG_TODW5,  "You can use \"Options|Graphics|Enhance skies?\" and"                           \
              " \"Options|Emulator|Paths...\" to use any uncompressed 24-bit BMP file as the" \
              " background."                                                                  },
#endif
#ifdef AMIGA
{ MSG_TODA1,  "You can drag and drop a file onto the main window to load it, or drag and"     \
              " drop a directory onto the main window to change the sidebar to that"          \
              " directory. You can also drag and drop onto the app icon."                     },
{ MSG_TODA2,  "On the status bar, you can click the \"Speed:\" button to cycle through"       \
              " the FPS/kHz/percentage speed display formats. Holding Shift while doing so"   \
              " will cycle backwards."                                                        },
{ MSG_TODA3,  "On the status bar, an animated mouth will appear instead of the machine"       \
              " glyph (on the far left of the status bar) while speaking."                    },
{ MSG_TODA4,  "In some circumstances, not all of the toolbar buttons will be visible."        \
              " You can drag (left and) right over the toolbar while holding down the"        \
              " Shift key and the left mouse button, to scroll the other buttons into"        \
              " view."                                                                        },
{ MSG_TODA5,  "Pressing the Help key while browsing the menu will show a brief"               \
              " description of debugger commands, or open the user manual on the"             \
              " appropriate page."                                                            },
{ MSG_TODA6,  "In the floppy drives subwindow, you can click on the disk blocks overview"     \
              " to change which block is shown in the block contents grid."                   },
#endif
};
