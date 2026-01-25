EXPORT const struct DataCommentStruct datacomment[] = {
// Elektor BIOS (0..97)
{    0x6,    0x7, "1st vector for breakpoint entry"                                              }, // BVEC1
{    0x8,    0x9, "2nd vector for breakpoint entry"                                              }, // BVEC2
{   0xAD,   0xBC, "PVI parameters during monitor display. Monitor parameters for the objects"    }, // MPOBJ1
{  0x122,  0x13D, "Key code function table"                                                      }, // KBFTBL
{  0x122,     -1, "\"-\" key"                                                                    },
{  0x126,     -1, "\"+\" key"                                                                    },
{  0x12E,     -1, "Not used: key ignored"                                                        },
{  0x131,     -1, "Not used: key ignored"                                                        },
{  0x177,  0x180, "PVI control parameters during monitor mode"                                   }, // PVIPAR
{  0x177,     -1, "Size"                                                                         },
{  0x178,     -1, "ObCol1/2"                                                                     },
{  0x179,     -1, "ObCol3/4"                                                                     },
{  0x17A,     -1, "Form/Pos"                                                                     },
{  0x17B,  0x17C, "XX"                                                                           },
{  0x17D,     -1, "BgCtl"                                                                        },
{  0x17E,     -1, "Snd"                                                                          },
{  0x17F,     -1, "Scor12"                                                                       },
{  0x180,     -1, "Scor34"                                                                       },
{  0x27B,  0x2CE, "Image table for character display"                                            }, // IMGTBL
{  0x27B,  0x280, "0,1    $00,$01"                                                               },
{  0x281,  0x286, "2,3    $02,$03"                                                               },
{  0x287,  0x28C, "4,5    $04,$05"                                                               },
{  0x28D,  0x292, "6,7    $06,$07"                                                               },
{  0x293,  0x298, "8,9    $08,$09"                                                               },
{  0x299,  0x29E, "A,B    $0A,$0B"                                                               },
{  0x29F,  0x2A4, "C,D    $0C,$0D"                                                               },
{  0x2A5,  0x2AA, "E,F    $0E,$0F"                                                               },
{  0x2AB,  0x2B0, "G,L    $10,$11"                                                               },
{  0x2B1,  0x2B6, "I,N    $12,$13"                                                               },
{  0x2B7,  0x2BC, "P,R    $14,$15"                                                               },
{  0x2BD,  0x2C2, "=,     $16,$17    space"                                                      },
{  0x2C3,  0x2C8, "+,-    $18,$19"                                                               },
{  0x2C9,  0x2CE, ":,X    $1A,$1B"                                                               },
{  0x2F5,  0x2F8, "\"PC =\"    R2: $04"                                                          },
{  0x2F9,  0x2FC, "\"AD =\"    R2: $08"                                                          },
{  0x2FD,  0x300, "\"R  =\"    R2: $0C"                                                          },
{  0x301,  0x304, "\"BP1=\"    R2: $10"                                                          },
{  0x305,  0x308, "\"BP2=\"    R2: $14"                                                          },
{  0x309,  0x30C, "\"BEG=\"    R2: $18"                                                          },
{  0x30D,  0x310, "\"END=\"    R2: $1C"                                                          },
{  0x311,  0x314, "\"SAD=\"    R2: $20"                                                          },
{  0x315,  0x318, "\"FIL=\"    R2: $24"                                                          },
{  0x319,  0x31C, "\"IIII\"    R2: $28"                                                          },
{  0x537,  0x53A, "Start up"                                                                     },
{  0x53B,  0x53C, "Indirect address"                                                             },
{  0x53D,  0x53E, "PPSL"                                                                         },
{  0x53F,     -1, "BCTA"                                                                         },
{  0x800,  0x88F, "Monitor object images"                                                        }, // MONOB
{  0x800,  0x817, "Shape data, first text line"                                                  },
{  0x818,  0x82F, "Shape data, second text line"                                                 },
{  0x830,  0x847, "Shape data, third text line"                                                  },
{  0x848,  0x85F, "Shape data, fourth text line"                                                 },
{  0x860,  0x877, "Shape data, fifth text line"                                                  },
{  0x878,  0x88F, "Shape data, sixth text line"                                                  },
{  0x890,  0x897, "Text line. 'Monitor line': eight character codes for display"                 }, // MLINE
{  0x898,     -1, "Function sequence indicator (BP1/2).\n"                                       \
                  "Silence counter for read cassette.\n"                                         \
                  "BP function indicator/RCAS silence counter"                                   }, // FSEQ/SILENC
{  0x899,     -1, "+, - enter key memory"                                                        }, // ENTM
{  0x89A,     -1, "Monitor function index"                                                       }, // MFUNC
{  0x89B,  0x89C, "Scratch RAM for keyboard routine"                                             }, // MSCR
{  0x89C,     -1, "Text line"                                                                    }, // MLINE
{  0x89D,     -1, "Right keyboard status"                                                        }, // RKBST
{  0x89E,     -1, "Left keyboard status"                                                         }, // LKBST
{  0x89F,     -1, "Monitor keyboard status. Combined keyboard status"                            }, // MKBST
{  0x8A0,  0x8AB, "Function scratch memory, used by memory and data routines"                    }, // FSCRM
{  0x8A2,  0x8A3, "Use of FSCRM during cassette operations: start address of write file"         }, // SADR
{  0x8A4,  0x8A5, "Use of FSCRM during cassette operations: memory begin address"                }, // BEGA
{  0x8A8,  0x8A9, "Use of FSCRM during cassette operations: memory end address"                  }, // ENDA
{  0x8AA,     -1, "Use of FSCRM during cassette operations: check char for read and write"       }, // BCC
{  0x8AB,     -1, "Use of FSCRM during cassette operations: block byte count for read and write" }, // MCNT
{  0x8AC,  0x8B4, "2650 register status. Save registers, PSU and PSL area"                       }, // REGM
{  0x8B5,  0x8B6, "Breakpoint address 1"                                                         }, // BK1
{  0x8B7,  0x8B8, "Breakpoint address 2"                                                         }, // BK2
{  0x8B9,  0x8BD, "Space for startup program"                                                    }, // INTADR
{  0x8BE,  0x8BF, "2650 PC. Start address"                                                       }, // PC.
{ 0x1D00, 0x1D01, "12-bit tone A1 tuning (----,321076543210):\n"                                 \
                  "$1D01: bits 7..4: unused\n"                                                   \
                  "$1D01: bits 3..0: high 4 bits\n"                                              \
                  "$1D00: bits 7..0: low 8 bits"                                                 }, // 74
{ 0x1D02, 0x1D03, "12-bit tone B1 tuning (----,321076543210):\n"                                 \
                  "$1D03: bits 7..4: unused\n"                                                   \
                  "$1D03: bits 3..0: high 4 bits\n"                                              \
                  "$1D02: bits 7..0: low 8 bits"                                                 }, // 75
{ 0x1D04, 0x1D05, "12-bit tone C1 tuning (----,321076543210):\n"                                 \
                  "$1D05: bits 7..4: unused\n"                                                   \
                  "$1D05: bits 3..0: high 4 bits\n"                                              \
                  "$1D04: bits 7..0: low 8 bits"                                                 }, // 76
{ 0x1D06,     -1, "bits 7..5: unused\n"                                                          \
                  "bits 4..0: noise period control"                                              }, // 77
{ 0x1D07,     -1, "(--,543,210):\n"                                                              \
                  "bit 7: IOB1 disable (unused)\n"                                               \
                  "bit 6: IOA1 disable (0=maximum PVI volume, 1=use $1D0E for PVI volume)\n"     \
                  "bit 5: noise C1 disable\n"                                                    \
                  "bit 4: noise B1 disable\n"                                                    \
                  "bit 3: noise A1 disable\n"                                                    \
                  "bit 2: tone C1 disable\n"                                                     \
                  "bit 1: tone B1 disable\n"                                                     \
                  "bit 0: tone A1 disable"                                                       }, // 78
{ 0x1D08,     -1, "(---,4,3210):\n"                                                              \
                  "bits 7..5: unused\n"                                                          \
                  "bit 4: channel A1 amplitude mode (0=logarithmic, 1=envelope)\n"               \
                  "bits 3..0: channel A1 amplitude (in logarithmic mode)"                        }, // 79
{ 0x1D09,     -1, "(---,4,3210):\n"                                                              \
                  "bits 7..5: unused\n"                                                          \
                  "bit 4: channel B1 amplitude mode (0=logarithmic, 1=envelope)\n"               \
                  "bits 3..0: channel B1 amplitude (in logarithmic mode)"                        }, // 80
{ 0x1D0A,     -1, "(---,4,3210):\n"                                                              \
                  "bits 7..5: unused\n"                                                          \
                  "bit 4: channel C1 amplitude mode (0=logarithmic, 1=envelope)\n"               \
                  "bits 3..0: channel C1 amplitude (in logarithmic mode)"                        }, // 81
{ 0x1D0B, 0x1D0C, "16-bit envelope 1 period:\n"                                                  \
                  "$1D0C: high 8 bits\n"                                                         \
                  "$1D0B: low 8 bits"                                                            }, // 82
{ 0x1D0D,     -1, "Envelope 1 shape (----,3210):\n"                                              \
                  "bits 7..4: unused\n"                                                          \
                  "bit 3: continue\n"                                                            \
                  "bit 2: attack\n"                                                              \
                  "bit 1: alternate\n"                                                           \
                  "bit 0: hold"                                                                  }, // 83
{ 0x1D0E,     -1, "I/O port A1 data store: 8-bit parallel I/O on port A1 (----,3210):\n"         \
                  "bits 7..4: unused\n"                                                          \
                  "bits 3..0: PVI volume ($00=silent..$0F=maximum) if bit 6 of $1D07 is set"     }, // 84
{ 0x1D0F,     -1, "I/O port B1 data store: 8-bit parallel I/O on port B1 (unused)"               }, // 85
{ 0x1D10, 0x1D11, "12-bit tone A2 tuning (----,321076543210):\n"                                 \
                  "$1D11: bits 7..4: unused\n"                                                   \
                  "$1D11: bits 3..0: high 4 bits\n"                                              \
                  "$1D10: bits 7..0: low 8 bits"                                                 }, // 86
{ 0x1D12, 0x1D13, "12-bit tone B2 tuning (----,321076543210):\n"                                 \
                  "$1D13: bits 7..4: unused\n"                                                   \
                  "$1D13: bits 3..0: high 4 bits\n"                                              \
                  "$1D12: bits 7..0: low 8 bits"                                                 }, // 87
{ 0x1D14, 0x1D15, "12-bit tone C2 tuning (----,321076543210):\n"                                 \
                  "$1D15: bits 7..4: unused\n"                                                   \
                  "$1D15: bits 3..0: high 4 bits\n"                                              \
                  "$1D14: bits 7..0: low 8 bits"                                                 }, // 88
{ 0x1D16,     -1, "(---,43210):\n"                                                               \
                  "bits 7..5: unused\n"                                                          \
                  "bits 4..0: noise period control"                                              }, // 89
{ 0x1D17,     -1, "(--,543,210):\n"                                                              \
                  "bit 7: IOB2 disable (unused)\n"                                               \
                  "bit 6: IOA2 disable (unused)\n"                                               \
                  "bit 5: noise C2 disable\n"                                                    \
                  "bit 4: noise B2 disable\n"                                                    \
                  "bit 3: noise A2 disable\n"                                                    \
                  "bit 2: tone C2 disable\n"                                                     \
                  "bit 1: tone B2 disable\n"                                                     \
                  "bit 0: tone A2 disable"                                                       }, // 90
{ 0x1D18,     -1, "(---,4,3210):\n"                                                              \
                  "bits 7..5: unused\n"                                                          \
                  "bit 4: channel A2 amplitude mode (0=logarithmic, 1=envelope)\n"               \
                  "bits 3..0: channel A2 amplitude (in logarithmic mode)"                        }, // 91
{ 0x1D19,     -1, "(---,4,3210):\n"                                                              \
                  "bits 7..5: unused\n"                                                          \
                  "bit 4: channel B2 amplitude mode (0=logarithmic, 1=envelope)\n"               \
                  "bits 3..0: channel B2 amplitude (in logarithmic mode)"                        }, // 92
{ 0x1D1A,     -1, "(---,4,3210):\n"                                                              \
                  "bits 7..5: unused\n"                                                          \
                  "bit 4: channel C2 amplitude mode (0=logarithmic, 1=envelope)\n"               \
                  "bits 3..0: channel C2 amplitude (in logarithmic mode)"                        }, // 93
{ 0x1D1B, 0x1D1C, "16-bit envelope 1 period:\n"                                                  \
                  "$1D0C: high 8 bits\n"                                                         \
                  "$1D0B: low 8 bits"                                                            }, // 94
{ 0x1D1D,     -1, "Envelope 2 shape (----,3210):\n"                                              \
                  "bits 7..4: unused\n"                                                          \
                  "bit 3: continue\n"                                                            \
                  "bit 2: attack\n"                                                              \
                  "bit 1: alternate\n"                                                           \
                  "bit 0: hold"                                                                  }, // 95
{ 0x1D1E,     -1, "I/O port A2 data store: 8-bit parallel I/O on port A2 (unused)"               }, // 96
{ 0x1D1F,     -1, "I/O port B2 data store: 8-bit parallel I/O on port B2 (unused)"               }, // 97
// Interton/Elektor hardware (98..120)
// ¾=3/4, ½=1/2, ¼=1/4.
// Ideally, when allglyphs is FALSE, we should ideally show a version of
// the below string without using the ¼ and ¾ characters (½ is OK).
{ 0x1E80,     -1, "(76,5,4,3,2,--):\n"                                                           \
                  "bits 7..6: volume: %00=full, %01=¾, %10=½, %11=¼\n"                           \
                  "bit 5: inverted grid/background colours on/off\n"                             \
                  "bit 4: start explosion (>= 20 msec)\n"                                        \
                  "bit 3: noise on/off\n"                                                        \
                  "bit 2: PVI tone on/off\n"                                                     \
                  "bits 1..0: unmapped"                                                          },
{ 0x1E88,     -1, "(7654,----):\n"                                                               \
                  "bit 7: p1 (left) '1' button\n"                                                \
                  "bit 6: p1 (left) '4' button\n"                                                \
                  "bit 5: p1 (left) '7' button\n"                                                \
                  "bit 4: p1 (left) 'Clear' button\n"                                            \
                  "bits 3..0: unused"                                                            },
{ 0x1E89,     -1, "(7654,----):\n"                                                               \
                  "bit 7: p1 (left) '2' button\n"                                                \
                  "bit 6: p1 (left) '5' button\n"                                                \
                  "bit 5: p1 (left) '8' button\n"                                                \
                  "bit 4: p1 (left) '0' button\n"                                                \
                  "bits 3..0: unused"                                                            },
{ 0x1E8A,     -1, "(7654,----):\n"                                                               \
                  "bit 7: p1 (left) '3' button\n"                                                \
                  "bit 6: p1 (left) '6' button\n"                                                \
                  "bit 5: p1 (left) '9' button\n"                                                \
                  "bit 4: p1 (left) 'Enter' button\n"                                            \
                  "bits 3..0: unused"                                                            },
{ 0x1E8B,     -1, "(76,------):\n"                                                               \
                  "bit 7: 'SELECT' button\n"                                                     \
                  "bit 6: 'START' button\n"                                                      \
                  "bits 5..0: unused"                                                            },
{ 0x1E8C,     -1, "(7654,----):\n"                                                               \
                  "bit 7: p2 (right) '1' button\n"                                               \
                  "bit 6: p2 (right) '4' button\n"                                               \
                  "bit 5: p2 (right) '7' button\n"                                               \
                  "bit 4: p2 (right) 'Clear' button\n"                                           \
                  "bits 3..0: unused"                                                            },
{ 0x1E8D,     -1, "(7654,----):\n"                                                               \
                  "bit 7: p2 (right) '2' button\n"                                               \
                  "bit 6: p2 (right) '5' button\n"                                               \
                  "bit 5: p2 (right) '8' button\n"                                               \
                  "bit 4: p2 (right) '0' button\n"                                               \
                  "bits 3..0: unused"                                                            },
{ 0x1E8E,     -1, "(7654,----):\n"                                                               \
                  "bit 7: p2 (right) '3' button\n"                                               \
                  "bit 6: p2 (right) '6' button\n"                                               \
                  "bit 5: p2 (right) '9' button\n"                                               \
                  "bit 4: p2 (right) 'Enter' button\n"                                           \
                  "bits 3..0: unused"                                                            },
{ 0x1F00, 0x1F09, "Sprite #0 imagery"                                                            },
{ 0x1F10, 0x1F19, "Sprite #1 imagery"                                                            },
{ 0x1F20, 0x1F29, "Sprite #2 imagery"                                                            },
{ 0x1F40, 0x1F49, "Sprite #3 imagery"                                                            },
{ 0x1F80, 0x1FA7, "Vertical grid"                                                                },
{ 0x1FA8, 0x1FAC, "Horizontal grid"                                                              },
{ 0x1FC0,     -1, "(76,54,32,10):\n"                                                             \
                  "bits 7..6: sprite #3 size (%00..%11)\n"                                       \
                  "bits 5..4: sprite #2 size (%00..%11)\n"                                       \
                  "bits 3..2: sprite #1 size (%00..%11)\n"                                       \
                  "bits 1..0: sprite #0 size (%00..%11)"                                         },
{ 0x1FC1,     -1, "(--,543,210):\n"                                                              \
                  "bits 7..6: unused\n"                                                          \
                  "bits 5..3: colour of sprite #0 (inverted RGB)\n"                              \
                  "bits 2..0: colour of sprite #1 (inverted RGB)"                                },
{ 0x1FC2,     -1, "(--,543,210):\n"                                                              \
                  "bits 7..6: unused\n"                                                          \
                  "bits 5..3: colour of sprite #2 (inverted RGB)\n"                              \
                  "bits 2..0: colour of sprite #3 (inverted RGB)"                                },
{ 0x1FC3,     -1, "(------,1,0):\n"                                                              \
                  "bits 7..2: unused\n"                                                          \
                  "bit 1: score format (0=2 groups of 2 digits, 1=1 group of 4 digits)\n"        \
                  "bit 0: score position (0=top of screen, 1=bottom of screen)"                  },
{ 0x1FC6,     -1, "(-,654,3,210):\n"                                                             \
                  "bit 7: unused\n"                                                              \
                  "bits 6..4: grid colour (RGB)\n"                                               \
                  "bit 3: grid/background enable flag (0=off, 1=on)\n"                           \
                  "bits 2..0: background colour (RGB) (black if bit 3 is clear)"                 },
{ 0x1FC8,     -1, "(7654,3210):\n"                                                               \
                  "bits 7..4: BCD value of 1st digit\n"                                          \
                  "bits 3..0: BCD value of 2nd digit"                                            },
{ 0x1FC9,     -1, "(7654,3210):\n"                                                               \
                  "bits 7..4: BCD value of 3rd digit\n"                                          \
                  "bits 3..0: BCD value of 4th digit"                                            },
{ 0x1FCA,     -1, "(7654,3210):\n"                                                               \
                  "Read-once!\n"                                                                 \
                  "bits 7..4: sprites #0..#3 collisions with bkgrnd\n"                           \
                  "bits 3..0: sprites #0..#3 displays complete"                                  },
{ 0x1FCB,     -1, "(-,6,543210):\n"                                                              \
                  "Read-once!\n"                                                                 \
                  "bit 7: unused\n"                                                              \
                  "bit 6: vertical reset flag\n"                                                 \
                  "bit 5: sprites #0/#1 collision\n"                                             \
                  "bit 4: sprites #0/#2 collision\n"                                             \
                  "bit 3: sprites #0/#3 collision\n"                                             \
                  "bit 2: sprites #1/#2 collision\n"                                             \
                  "bit 1: sprites #1/#3 collision\n"                                             \
                  "bit 0: sprites #2/#3 collision"                                               },
// CD2650 BIOS (121..130)
{   0x11,   0x12, "Originating address for brkpt retn. Breakpoint return address=BPSV"           },
{  0x1A6,  0x1B5, "\"0123456789ABCDEF\". Literal values for hex conv."                           }, // HTBL
{ 0x17EA,     -1, "Length storage location"                                                      }, // LENT
{ 0x17F4,     -1, "First breakpoint address"                                                     }, // BKP1
{ 0x17F5,     -1, "Second breakpoint address"                                                    }, // BKP2
{ 0x17F6,     -1, "Breakpoint byte 1 save pos"                                                   }, // BPD1
{ 0x17F7,     -1, "Breakpoint byte 2 save pos"                                                   }, // BPD2
{ 0x17F9,     -1, "Sumcheck storage location"                                                    }, // SUMC
{ 0x17FA, 0x17FB, "Address"                                                                      }, // ADD1/2
{ 0x17FE, 0x17FF, "Cursor pointer"                                                               }, // CUR1/2
// PHUNSY BIOS/MDCR (131..249)
{  0x245,  0x254, "\"-:=>GIMPQRSTUVWX\""                                                         }, // 131 (FIRSTPHUNSYDATACOMMENT)
{  0x255,  0x266, "Bits indicating command properties:\n"                                        \
                  "Bit 0: command, causes COMRES to be cleared\n"                                \
                  "Bit 1: start address required\n"                                              \
                  "Bit 2: end address required\n"                                                \
                  "Bit 3: target address required"                                               },
{  0x257,     -1, "- start address is entered"                                                   },
{  0x258,     -1, ": start address and command"                                                  },
{  0x259,     -1, "= command only, no data required"                                             },
{  0x25A,     -1, "> end address is entered"                                                     },
{  0x25B,     -1, "G start address and command"                                                  },
{  0x25C,     -1, "I end address and command"                                                    },
{  0x25D,     -1, "M target address and command"                                                 },
{  0x25E,     -1, "P target address and command"                                                 }, // 140
{  0x25F,     -1, "Q start address and command"                                                  },
{  0x260,     -1, "R start address and command"                                                  },
{  0x261,     -1, "S command only"                                                               },
{  0x262,     -1, "T command only"                                                               },
{  0x263,     -1, "U start address and command"                                                  },
{  0x264,     -1, "V target address and command"                                                 },
{  0x265,     -1, "W end address and command"                                                    },
{  0x266,     -1, "X command only"                                                               },
{  0x269,     -1, "Set start address"                                                            },
{  0x26B,     -1, "Change memory"                                                                }, // 150
{  0x26D,     -1, "Display bank"                                                                 },
{  0x26F,     -1, "Set end address"                                                              },
{  0x271,     -1, "Go execute"                                                                   },
{  0x273,     -1, "Inspect memory"                                                               },
{  0x275,     -1, "Move memory"                                                                  },
{  0x277,     -1, "Preset memory"                                                                },
{  0x279,     -1, "Set/execute at Q bank"                                                        },
{  0x27B,     -1, "Read cassette"                                                                },
{  0x27D,     -1, "Error message"                                                                },
{  0x27F,     -1, "Go to MDCR software"                                                          }, // 160
{  0x281,     -1, "Set/execute at U bank"                                                        },
{  0x283,     -1, "Verify memory"                                                                },
{  0x285,     -1, "Write cassette"                                                               },
{  0x287,     -1, "Identify, prints monitor ID"                                                  },
{  0x57D,  0x59D,  "\"*** PHUNSY MONITOR 04-00 *** '82 \""                                       },
{  0x5A0,  0x5AD, "Initial address constants for RAM"                                            },
{  0x653,  0x664, "Control characters"                                                           },
{  0x655,     -1, "Reset screen"                                                                 },
{  0x656,     -1, "Clear cursor to end of line"                                                  },
{  0x657,     -1, "Clear cursor to end of screen"                                                }, // 170
{  0x658,     -1, "Bell"                                                                         },
{  0x659,     -1, "Move cursor left 1 position"                                                  },
{  0x65A,     -1, "Clear screen"                                                                 },
{  0x65B,     -1, "Carriage return"                                                              },
{  0x65C,     -1, "Move cursor to leftmost position"                                             },
{  0x65D,     -1, "Move cursor to rightmost position"                                            },
{  0x65E,     -1, "Move cursor to top of screen"                                                 },
{  0x65F,     -1, "Move cursor to bottom of screen"                                              },
{  0x660,     -1, "Move cursor one position right"                                               },
{  0x661,     -1, "Move cursor one position right"                                               }, // 180
{  0x662,     -1, "Move cursor one position left"                                                },
{  0x663,     -1, "Move cursor one position lower"                                               },
{  0x664,     -1, "Move cursor one position higher"                                              },
{  0x665,  0x676, "Index to control routines"                                                    },
{  0xC00,  0xCFF, "First block, catalog"                                                         }, // FSTBLK
{  0xD00,  0xDFF, "Temp 256 bytes data block"                                                    }, // TMPBLK
{  0xE00,  0xEDF, "224 bytes not used by the monitor"                                            },
{  0xE00,     -1, "Checksum"                                                                     }, // CHECKS
{  0xE01,     -1, "Cassette block number"                                                        }, // BLKNUM
{  0xE02,  0xE05, "Run branch (routine placed in RAM)"                                           }, // 190 RUNBR
{  0xE06,     -1, "Not used"                                                                     },
{  0xE07,     -1, "Catalog index (file number x8)"                                               }, // CATINX
{  0xE08,  0xE17, "These 16 bytes must stay together in the same order"                          },
{  0xE10,  0xE17, "Note these 8 bytes are in reverse order on cassette"                          },
{  0xE10,     -1, "Part"                                                                         }, // XOFXRS
{  0xE11,     -1, "Extra address (bank info)"                                                    }, // EADRES
{  0xE12,     -1, "File language"                                                                }, // LANRES
{  0xE13,     -1, "First block of file on cassette (0 if entry is free)"                         }, // FBLKOF
{  0xE14,  0xE15, "File size"                                                                    }, // FILS
{  0xE16,  0xE17, "File address"                                                                 }, // 200 FILA
{  0xE18,     -1, "Memory to temp block (routine placed in RAM)"                                 }, // MYTTB
{  0xE3E,     -1, "Number of matching files"                                                     }, // MATNF
{  0xE3F,  0xE4E, "Catalog index of matching files"                                              }, // CIMATF
{  0xEE4,  0xEE5, "Command address"                                                              }, // CMAD
{  0xEE6,  0xEE7, "Indirect escape return address"                                               }, // ERET
{  0xEE8,  0xEE9, "Indirect output address - pointer to character output routine"                }, // IOAD
{  0xEEA,  0xEEB, "Indirect input address - pointer to character input routine"                  }, // IIAD
{  0xEEC,  0xEED, "Indirect check input address - key pressed?"                                  }, // ICAD
{  0xEEE,     -1, "Baud rate"                                                                    }, // BRTRES
{  0xEEF,     -1, "Cursor position"                                                              }, // 210 CURRES
{  0xEF0,     -1, "Screen border upper"                                                          }, // UPSTRE
{  0xEF1,     -1, "Screen border left"                                                           }, // LESTRE
{  0xEF2,     -1, "Screen border lower"                                                          }, // LOENRE
{  0xEF3,     -1, "Screen border right"                                                          }, // RIENRE
{  0xEF4,  0xEF5, "Cursor position"                                                              }, // CURS
{  0xEF6,     -1, "Command information reserved byte"                                            }, // COMRES
{  0xEF7,     -1, "Control port out storage"                                                     }, // CPORES
{  0xEF8,  0xEF9, "Start address"                                                                }, // STAD
{  0xEFA,  0xEFB, "End address"                                                                  }, // ENAD
{  0xEFC,  0xEFD, "Target address"                                                               }, // 220 TOAD
{  0xEFE,  0xEFF, "Two bytes register"                                                           }, // TBSR
{  0xF00,  0xFFF, "Input RAM"                                                                    },
{  0xF00,     -1, "This byte is index"                                                           }, // 223 INPRAM (LASTNONMDCRDATACOMMENT)
{ 0x186B, 0x186E, "\"CAT \""                                                                     },
{ 0x186F, 0x1875, "\"DELETE \""                                                                  },
{ 0x1876, 0x187A, "\"INIT \""                                                                    },
{ 0x187B, 0x187F, "\"LOAD \""                                                                    },
{ 0x1880, 0x1884, "\"LOCK \""                                                                    },
{ 0x1885, 0x188B, "\"RENAME \""                                                                  },
{ 0x188C, 0x188F, "\"REW \""                                                                     }, // 230
{ 0x1890, 0x1893, "\"RUN \""                                                                     },
{ 0x1894, 0x1898, "\"SAVE \""                                                                    },
{ 0x1899, 0x189F, "\"UNLOCK \""                                                                  },
{ 0x192E, 0x1933, "0: \"ERROR \""                                                                },
{ 0x1934, 0x193C, "1: \"CASSETTE \""                                                             },
{ 0x193D, 0x1942, "2: \"WRITE \""                                                                },
{ 0x1943, 0x194C, "3: \"PROTECTED \""                                                            },
{ 0x194D, 0x1951, "4: \"READ \""                                                                 },
{ 0x1952, 0x1957, "5: \"INPUT \""                                                                },
{ 0x1958, 0x195B, "6: \"NOT \""                                                                  }, // 240
{ 0x195C, 0x195E, "7: \"IN \""                                                                   },
{ 0x195F, 0x1966, "8: \"CATALOG \""                                                              },
{ 0x1967, 0x196B, "9: \"FULL \""                                                                 },
{ 0x196C, 0x196F, "10: \"RUN \""                                                                 },
{ 0x1970, 0x1973, "11: \"TOO \""                                                                 },
{ 0x1974, 0x1978, "12: \"LONG \""                                                                },
{ 0x1979, 0x197F, "13: \"LOCKED \""                                                              },
{ 0x1F81, 0x1F94, "\"PHUNSY MDCR-006 1982\""                                                     },
{ 0x1F97, 0x1FBD, "\"CN:FILENAME:BEGA:ENDA:L:FB:LN:EA:NR:BU:\""                                  }, // 249 (LASTMDCRDATACOMMENT)
// PIPBUG 1 BIOS (250) (PIPBUG_FIRST/LASTDATACOMMENT)
{  0x259,  0x268, "\"0123456789ABCDEF\""                                                         },
// BINBUG BIOS (251..266) (BINBUG_FIRST..LASTDATACOMMENT)
{  0x400,  0x408, "Register & PSW stack"                                                         },
{  0x409,     -1, "Output switch"                                                                },
{  0x40A,  0x40C, "PSL restore instr."                                                           },
{  0x40D,  0x40E, "16-bit address store"                                                         },
{  0x413,  0x426, "Input buffer"                                                                 },
{  0x427,     -1, "Buffer pointer"                                                               },
{  0x42A,     -1, "Buffer status"                                                                },
{  0x42B,     -1, "Checksum scratch"                                                             },
{  0x42C,     -1, "BP flag. FF >= BP set."                                                       },
{  0x42D,     -1, "Bytes which were replaced"                                                    },
{  0x42E,     -1, "by a BP vector."                                                              },
{  0x42F,     -1, "BP address."                                                                  },
{  0x431,  0x432, "Indirect pointer to extended command processor. Normally $001D"               },
{  0x433,  0x437, "VDU driver scratch"                                                           },
{  0x438,     -1, "VDU cursor count. RAM not cleared from here on"                               },
{  0x43B,  0x43C, "Indir to external COUT"                                                       },
// Selbstbaucomputer V0.9 BIOS (267..283) (FIRST..LASTDATACOMMENT09)
{  0x800,  0x802, "Input/output memory for conversion routines"                                  }, // DATA1/DATA2/DATA3
{  0x803,     -1, "Internal buffer for KIN routine"                                              }, // KINBUF
{  0x804,  0x80C, "Register storage for the breakpoint routine"                                  }, // RSAVE
{  0x80D,  0x812, "Display buffer. This area is displayed through the DIS routine"               }, // DISBUF
{  0x813,  0x814, "Internal memory"                                                              },
{  0x815,  0x81C, "Internal job memory"                                                          }, // WBUF
{  0x81D,  0x81E, "Contains the address in the program counter"                                  }, // PCH/PCL
{  0x81F,  0x820, "Contains the start address for the cassette routine"                          }, // ADRS
{  0x821,     -1, "Contains the block length for the cassette routine"                           },
{  0x822,     -1, "Checksum byte for the cassette routine"                                       }, // SUM
{  0x823,  0x824, "Contains the breakpoint address"                                              }, // BPADR
{  0x825,  0x826, "Contains the data at the breakpoint address"                                  }, // BPINH
{  0x827,  0x82A, "Input storage for arithmetic operations"                                      }, // OPR1/OPR2
{  0x82B,  0x82C, "Output storage for arithmetic operations"                                     }, // RSLT
{  0x82D,     -1, "Status byte for arithmetic operations and for data entry"                     }, // STAT
{  0x82E,  0x830, "Memory for decimal digits"                                                    }, // BPADR
{  0x831,  0x8FF, "Free for user purposes"                                                       },
// Selbstbaucomputer V2.0 BIOS (284..304) (FIRST..LASTDATACOMMENT20)
{  0x801,  0x802, "Storage memory"                                                               }, // SVR1/SVDT
{  0x803,     -1, "Temporary data storage"                                                       }, // TDAS
{  0x804,     -1, "Saved R0 register"                                                            }, // SVR0
{  0x805,     -1, "Saved R1 bank 0 register"                                                     }, // SR10
{  0x806,     -1, "Saved R2 bank 0 register"                                                     }, // SR20
{  0x807,     -1, "Saved R3 bank 0 register"                                                     }, // SR30
{  0x808,     -1, "Saved R1 bank 1 register"                                                     }, // SR11
{  0x809,     -1, "Saved R2 bank 1 register"                                                     }, // SR21
{  0x80A,     -1, "Saved R3 bank 1 register"                                                     }, // SR31
{  0x80B,     -1, "Saved PSL"                                                                    }, // PSLS
{  0x80C,     -1, "Saved PSU"                                                                    }, // PSUS
{  0x80D,     -1, "R0 storage"                                                                   }, // SR0
{  0x80E,     -1, "R1 storage"                                                                   }, // SR1
{  0x80F,     -1, "R2 storage"                                                                   }, // SR2
{  0x810,     -1, "R3 stroage"                                                                   }, // SR3
{  0x814,     -1, "Horizontal position"                                                          }, // HPOS
{  0x81F,  0x820, "Cassette address"                                                             }, // ADD1/ADD2
{  0x821,     -1, "Block length"                                                                 }, // LENT
{  0x822,     -1, "Checksum for cassette"                                                        }, // SUMC
{  0x823,  0x824, "Breakpoint address"                                                           }, // BKP1/BKP2
{  0x825,  0x826, "Breakpoint data storage"                                                      }, // BPD1/BPD2
// Instructor BIOS (305..354)
{ 0x17C0, 0x17C7, "8 byte scratch area"                                                          }, // SCTCH
{ 0x17C6, 0x17C7, "Temp storage"                                                                 }, // TEMP
{ 0x17C8, 0x17C9, "Stop address for WCAS"                                                        }, // EAD
{ 0x17CA, 0x17CB, "Beginning address for WCAS"                                                   }, // BAD
{ 0x17CC,     -1, "Data to be restored in break loc"                                             }, // BPD
{ 0x17CD, 0x17CE, "Address of breakpoint loc"                                                    }, // BPL
{ 0x17CF,     -1, "Breakpoint set flag"                                                          }, // BPF
{ 0x17D0,     -1, "Single step set flag"                                                         }, // SSF
{ 0x17D1, 0x17D8, "8 byte display register"                                                      }, // DISBUF
{ 0x17D9, 0x17DC, "A place to save R0 thru R3 of one bank"                                       }, // SAVREG
{ 0x17DD, 0x17DE, "Address for alter or patch command"                                           }, // MEM
{ 0x17DF, 0x17E0, "File ID flag and file ID"                                                     }, // FID
{ 0x17E1,     -1, "Block check char"                                                             }, // BCC
{ 0x17E2,     -1, "Save units digit"                                                             }, // BSTT
{ 0x17E3, 0x17E7, "Temp registers"                                                               }, // T, T1, T2, T3
{ 0x17E8, 0x17E9, "Copy of last address register"                                                }, // LADR
{ 0x17EA, 0x17EB, "Save location for LADR"                                                       }, // SLADR
{ 0x17EC, 0x17ED, "Kbd scan flags"                                                               }, // KFLG
{ 0x17EE,     -1, "Kbd debounce count"                                                           }, //
{ 0x17EF,     -1, "Display and alter flag"                                                       }, // ALTF
{ 0x17F0,     -1, "Restore registers flag"                                                       }, // RESTF
{ 0x17F1,     -1, "Interrupt Inhibit flag"                                                       }, // IFLG
{ 0x17F2, 0x17FD, "Storage for user registers"                                                   }, // UREG
{ 0x17FE, 0x17FF, "When power on these loc contain $5946"                                        }, // PWRON
{ 0x18A4, 0x18B3, "Command address table"                                                        },
{ 0x1F68, 0x1FC4, "Hex lookup table for hex to seven segment"                                    },
{ 0x1F68, 0x1F77, "This table contains the values for lighting the segments for 0..9 and A..F"   }, // SEGTBL
{ 0x1F78, 0x1F84, "Segment data for symbols P L U R H O = blank J - . Y N"                       },
{ 0x1F85, 0x1F8C, "This table contains the display Error"                                        }, // ERROR
{ 0x1F8D, 0x1F94, "This table contains the display Ad="                                          }, // ADR
{ 0x1F95, 0x1F9C, "This table contains the display HELLO"                                        }, // HELLO
{ 0x1F9D, 0x1FA4, "This table contains the display bP="                                          }, // BPEQ
{ 0x1FA5, 0x1FAC, "This table contains the display r="                                           }, // REQ
{ 0x1FAD, 0x1FB4, "This table contains the display PC="                                          }, // PCEQ
{ 0x1FB5, 0x1FBC, "This table contains the display F="                                           }, // FEQ
{ 0x1FBD, 0x1FC4, "This table contains the display LAd="                                         }, // LADEQ
{ 0x1FC5, 0x1FD4, "This table is the ASCII look up table"                                        }, // ASCII
{ 0x1FD6, 0x1FD7, "User entry to display routines"                                               }, // USRDSP
{ 0x1FD8, 0x1FD9, "Error message"                                                                }, // ERR
{ 0x1FDA, 0x1FDB, "Set DISBUF6,7 with contents of R0"                                            }, // BRKPT4
{ 0x1FDC, 0x1FDD, "Display and keyboard routine"                                                 }, // DISPLY
{ 0x1FDE, 0x1FDF, "Cassette input routine"                                                       }, // IN
{ 0x1FE0, 0x1FE1, "Cassette output"                                                              }, // OUT
{ 0x1FE2, 0x1FE3, "Cassette binary to ASCII hex output"                                          }, // HOUT
{ 0x1FE4, 0x1FE5, "Convert byte to nibble"                                                       }, // DISLSD
{ 0x1FE6, 0x1FE7, "Rotate a nibble"                                                              }, // ROT
{ 0x1FE8, 0x1FE9, "Carriage return and line feed"                                                }, // CRLF
{ 0x1FFA, 0x1FFB, "Get numbers"                                                                  }, // GNP
{ 0x1FFC, 0x1FFD, "Get numbes and display"                                                       }, // GNPA
{ 0x1FFE, 0x1FFF, "Move data to DISBUF"                                                          }, // MOV
// Arcadia hardware (355..382)
{ 0x1800, 0x18CF, "Upper display"                                                                }, // 355 is used literally in extracomment()
{ 0x18FD,     -1, "(7,6543210):\n"                                                               \
                  "bit 7: 0=normal mode, 1=\"board\" mode\n"                                     \
                  "bits 6..0: pitch"                                                             },
{ 0x18FE,     -1, "(765,4,3,210):\n"                                                             \
                  "bits 7..5: horizontal scrolling (0..7)\n"                                     \
                  "bit 4: noise on/off\n"                                                        \
                  "bit 3: tones on/off\n"                                                        \
                  "bits 2..0: volume (0..7)"                                                     },
{ 0x1900,     -1, "(----,3210):\n"                                                               \
                  "bits 7..4: unused\n"                                                          \
                  "bit 3: p1 (left) '1' button\n"                                                \
                  "bit 2: p1 (left) '4' button\n"                                                \
                  "bit 1: p1 (left) '7' button\n"                                                \
                  "bit 0: p1 (left) 'Clear' button"                                              },
{ 0x1901,     -1, "(----,3210):\n"                                                               \
                  "bits 7..4: unused\n"                                                          \
                  "bit 3: p1 (left) '2' button\n"                                                \
                  "bit 2: p1 (left) '5' button\n"                                                \
                  "bit 1: p1 (left) '8' button\n"                                                \
                  "bit 0: p1 (left) '0' button"                                                  },
{ 0x1902,     -1, "(----,3210):\n"                                                               \
                  "bits 7..4: unused\n"                                                          \
                  "bit 3: p1 (left) '3' button\n"                                                \
                  "bit 2: p1 (left) '6' button\n"                                                \
                  "bit 1: p1 (left) '9' button\n"                                                \
                  "bit 0: p1 (left) 'Enter' button"                                              },
{ 0x1903,     -1, "(----,3210):\n"                                                               \
                  "bits 7..4: unused\n"                                                          \
                  "bit 3: p1 (left) Palladium button 'x4'\n"                                     \
                  "bit 2: p1 (left) Palladium button 'x3'\n"                                     \
                  "bit 1: p1 (left) Palladium button 'x2'\n"                                     \
                  "bit 0: p1 (left) Palladium button 'x1'"                                       },
{ 0x1904,     -1, "(----,3210):\n"                                                               \
                  "bits 7..4: unused\n"                                                          \
                  "bit 3: p2 (right) '1' button\n"                                               \
                  "bit 2: p2 (right) '4' button\n"                                               \
                  "bit 1: p2 (right) '7' button\n"                                               \
                  "bit 0: p2 (right) 'Clear' button"                                             },
{ 0x1905,     -1, "(----,3210):\n"                                                               \
                  "bits 7..4: unused\n"                                                          \
                  "bit 3: p2 (right) '2' button\n"                                               \
                  "bit 2: p2 (right) '5' button\n"                                               \
                  "bit 1: p2 (right) '8' button\n"                                               \
                  "bit 0: p2 (right) '0' button"                                                 },
{ 0x1906,     -1, "(----,3210):\n"                                                               \
                  "bits 7..4: unused\n"                                                          \
                  "bit 3: p2 (right) '3' button\n"                                               \
                  "bit 2: p2 (right) '6' button\n"                                               \
                  "bit 1: p2 (right) '9' button\n"                                               \
                  "bit 0: p2 (right) 'Enter' button"                                             },
{ 0x1907,     -1, "(----,3210):\n"                                                               \
                  "bits 7..4: unused\n"                                                          \
                  "bit 3: p2 (right) Palladium button 'x4'\n"                                    \
                  "bit 2: p2 (right) Palladium button 'x3'\n"                                    \
                  "bit 1: p2 (right) Palladium button 'x2'\n"                                    \
                  "bit 0: p2 (right) Palladium button 'x1'"                                      },
{ 0x1908,     -1, "(-----,210):\n"                                                               \
                  "bits 7..3: unused\n"                                                          \
                  "bit 2: B button\n"                                                            \
                  "bit 1: A button\n"                                                            \
                  "bit 0: START button"                                                          },
{ 0x1980, 0x1987, "Sprite #0 imagery"                                                            },
{ 0x1988, 0x198F, "Sprite #1 imagery"                                                            },
{ 0x1990, 0x1997, "Sprite #2 imagery"                                                            },
{ 0x1998, 0x199F, "Sprite #3 imagery"                                                            },
{ 0x19A0, 0x19A7, "UDG #0 imagery"                                                               },
{ 0x19A8, 0x19AF, "UDG #1 imagery"                                                               },
{ 0x19B0, 0x19B7, "UDG #2 imagery"                                                               },
{ 0x19B8, 0x19BF, "UDG #3 imagery"                                                               },
{ 0x19F8,     -1, "(7,6,543,210):\n"                                                             \
                  "bit 7: 0=normal mode, 1=block graphics mode\n"                                \
                  "bit 6: 0=13 character rows, 1=26 character rows\n"                            \
                  "bits 5..3: in board mode, 1st frgrnd colour (inverted GRB)\n"                 \
                  "bits 2..0: in board mode, inner bkgrnd colour (inverted GRB)"                 },
{ 0x19F9,     -1, "(7,6,543,210):\n"                                                             \
                  "bit 7: 0=low-res mode, 1=high-res mode\n"                                     \
                  "bit 6: paddle interpolation\n"                                                \
                  "bits 5..3: colours of tile set 0\n"                                           \
                  "bits 2..0: (inner) bkgrnd colour (inverted GRB)"                              },
{ 0x19FA,     -1, "(7,6,543,210):\n"                                                             \
                  "bit 7: 0=sprite #2 double-height, 1=sprite #2 normal\n"                       \
                  "bit 6: 0=sprite #3 double-height, 1=sprite #3 normal\n"                       \
                  "bits 5..3: colours of sprite #2\n"                                            \
                  "bits 2..0: colours of sprite #3"                                              },
{ 0x19FB,     -1, "(7,6,543,210):\n"                                                             \
                  "bit 7: 0=sprite #0 double-height, 1=sprite #0 normal\n"                       \
                  "bit 6: 0=sprite #1 double-height, 1=sprite #1 normal\n"                       \
                  "bits 5..3: colours of sprite #0\n"                                            \
                  "bits 2..0: colours of sprite #1"                                              },
{ 0x19FC,     -1, "Read-once! (----,3210):\n"                                                    \
                  "bits 7..4: unused\n"                                                          \
                  "bits 3..0: sprites #3..#0 collisions with bkgrnd (active low)"                },
{ 0x19FD,     -1, "Read-once! (--,543210):\n"                                                    \
                  "bits 7..6: unused\n"                                                          \
                  "bit 5: sprites #2/#3 collision (active low)\n"                                \
                  "bit 4: sprites #1/#3 collision (active low)\n"                                \
                  "bit 3: sprites #1/#2 collision (active low)\n"                                \
                  "bit 2: sprites #0/#3 collision (active low)\n"                                \
                  "bit 1: sprites #0/#2 collision (active low)\n"                                \
                  "bit 0: sprites #0/#1 collision (active low)"                                  },
{ 0x19FE,     -2, "Read-once!"                                                                   },
{ 0x1A00, 0x1ACF, "Lower display"                                                                }, // 382 is used literally in extracomment()
// GBUG (383..412)
{  0x259,  0x268, "0123456789ABCDEF"                                                             }, // 383
{  0x400,     -1, "Temp R0"                                                                      },
{  0x401,     -1, "Temp R1"                                                                      },
{  0x402,     -1, "Temp R2"                                                                      },
{  0x403,     -1, "Temp R3"                                                                      },
{  0x404,     -1, "Temp R1'"                                                                     },
{  0x405,     -1, "Temp R2'"                                                                     },
{  0x406,     -1, "Temp R3'"                                                                     },
{  0x407,     -1, "Temp PSU"                                                                     },
{  0x408,     -1, "Temp PSL"                                                                     },
{  0x409,     -1, "Extended output flag"                                                         },
{  0x40A,     -1, "$77 = PPSL"                                                                   },
{  0x40B,     -1, "Value for PPSL"                                                               },
{  0x40C,     -1, "$1F = BCTA,UN"                                                                },
{  0x42C,     -1, "Break point flag"                                                             },
{  0x42D,     -1, "Break point data 1"                                                           },
{  0x42E,     -1, "Break point data 2"                                                           },
{  0x42F,  0x430, "Break point address"                                                          },
{  0x431,     -1, "Extended commands or error"                                                   }, // 401
{  0x432,     -1, "Extended commands low address"                                                },
{  0x437,     -1, "No. of frozen lines"                                                          },
{  0x438,     -1, "EDIT5800 loaded in RAM"                                                       },
{  0x439,  0x43A, "Cursor address"                                                               },
{  0x43B,  0x43C, "Indirect printer output address"                                              },
{  0x43E,     -1, "Quick scroll flag"                                                            },
{  0x43F,     -1, "VDU flag"                                                                     },
{ 0x57FC,     -1, "Start of edited line\n"                                                       \
                  "Out column #"                                                                 },
{ 0x57FD,     -1, "Turnaround CHaRacter\n"                                                       \
                  "CR/LF turnaround"                                                             },
{ 0x57FE, 0x57FF, "Edit in progress flag address"                                                \
                  "Flag/end of edit line"                                                        },
{ 0x7700, 0x77EF, "Keyboard buffer area for 2651"                                                }, // 412
// VHS DOS 2.6a (BINBUG) (413..475)
{ 0x6803,     -2, "Normal entry point"         }, // 412 (FIRSTVHSDOSDATACOMMENT)
{ 0x6805,     -2, "Open seq file for read"     },
{ 0x6807,     -2, "Get next char"              },
{ 0x6809,     -2, "Chain to a file"            },
{ 0x680B,     -2, "Open for writing"           },
{ 0x680D,     -2, "Put next char"              },
{ 0x680F,     -2, "Close read stream"          },
{ 0x6811,     -2, "Put char; close file"       },
{ 0x6813,     -2, "Find file"                  },
{ 0x6815,     -2, "Find next file this spec"   },
{ 0x6817,     -2, "General I/O routine"        },
{ 0x6819,     -2, "Select drive R0"            },
{ 0x681B,     -2, "Move buf ptr to next item"  },
{ 0x681D,     -2, "Print R2,R3 as decimal"     },
{ 0x681F,     -2, "Accept a line of chars"     },
{ 0x6821,     -2, "Put $0D,$0A on any stream"  },
{ 0x6823,     -2, "Move to start of dir"       },
{ 0x6825,     -2, "Move to Tk R2, Sc R3"       },
{ 0x6827,     -2, "Print file name just found" },
{ 0x6829,     -2, "Read a sector"              },
{ 0x682B,     -2, "Write a sector"             },
{ 0x682D,     -2, "Hexoot (BOUT from R0)"      },
{ 0x682F,     -2, "Get NuMBer to XGOTO"        },
{ 0x6831,     -2, "Print ADdRess R0,R2"        },
{ 0x6833,     -2, "Is R0 DECimal?"             },
{ 0x6835,     -2, "Not found routine"          },
{ 0x6837,     -2, "Delay, wait for busy low"   },
{ 0x6839,     -2, "Assign dev to a stream"     },
{ 0x683B,     -2, "Read next sector"           },
{ 0x6F7F, 0x6F8B, "Device table"               },
{ 0x6F7F,     -4, "VDU"                        },
{ 0x6F83,     -4, "Keyboard"                   },
{ 0x6F87,     -4, "Printer (LPT:)"             },
{ 0x6FF8,     -2, "Default sys,work drives"    },
{ 0x7000,     -3, "Stream identity, 3 chars"   },
{ 0x7003,     -2, "Addr of service routine"    },
{ 0x7005,     -1, "Program Buffer PoinTer"     },
{ 0x7006,     -1, "Bit 1,0=drv #; bit 2=side"  },
{ 0x7007,     -1, "Track #"                    },
{ 0x7008,     -1, "Sector #"                   },
{ 0x7009,     -1, "Directory sector"           },
{ 0x700A,     -1, "Dir byte"                   },
{ 0x700B,     -1, "Length (for writes)"        },
{ 0x700C,     -1, "Length (for writes)"        },
{ 0x700D,     -1, "File type (0=ordinary)"     },
{ 0x700E,     -1, "Device Buffer PoinTer"      },
{ 0x700F,     -1, "High byte of buffer addr"   },
{ 0x7070,     -2, "Line buffer pointer"        },
{ 0x7072,    -40, "Normal line buffer"         },
{ 0x70B2,     -4, "Track info for dual drive"  },
{ 0x70B6,    -18, "Directory ENTry copy"       },
{ 0x70CE,     -4, "Scratch"                    },
{ 0x70D2, 0x70D6, "Save regs"                  },
{ 0x70D7,     -1, "Sys drive (0 or 1)"         },
{ 0x70D8,     -1, "Work drv"                   },
{ 0x70DB,     -2, "Disk buffer pointer"        },
{ 0x70DD,     -2, "Res of GNUM"                },
{ 0x70DF,     -1, "Spare"                      },
{ 0x70E0,     -1, "Error info"                 },
{ 0x70E4,     -1, "Retry count"                },
{ 0x70E6,     -1, "Current drive"              },
{ 0x70FC,     -1, "Date valid flag"            },
{ 0x70FD,     -3, "DD,MM,YY in BCD"            }, // 475 (LASTVHSDOSDATACOMMENT)
{ 0x6803,     -2, "Normal entry point"         }, // 476 (FIRSTMICRODOSDATACOMMENT)
{ 0x6805,     -2, "Open seq file for read"     },
{ 0x6807,     -2, "Get next char"              },
{ 0x6809,     -2, "Chain to a file"            },
{ 0x680B,     -2, "Open for writing"           },
{ 0x680D,     -2, "Put next char"              }, // 481
{ 0x680F,     -2, "Close read stream"          },
{ 0x6811,     -2, "Put char; close file"       },
{ 0x6813,     -2, "Find file"                  },
{ 0x6815,     -2, "Find next file this spec"   },
{ 0x6817,     -2, "General I/O routine"        },
{ 0x6819,     -2, "Select drive R0"            },
{ 0x681B,     -2, "Move buf ptr to next item"  },
{ 0x681D,     -2, "Print R2,R3 as decimal"     },
{ 0x681F,     -2, "Accept a line of chars"     },
{ 0x6821,     -2, "Put $0D,$0A on any stream"  }, // 491
{ 0x6823,     -2, "Start of dir read"          },
{ 0x6825,     -2, "Read sec at Tk R2, Sc R3"   },
{ 0x6827,     -2, "Print file name just found" },
{ 0x6829,     -2, "Read a sector"              },
{ 0x682B,     -2, "Write a sector"             },
{ 0x682D,     -2, "Hexoot (BOUT from R0)"      },
{ 0x682F,     -2, "Get NuMBer to XGOTO"        },
{ 0x6831,     -2, "Print ID message"           },
{ 0x6833,     -2, "DOS version"                },
{ 0x6835,     -2, "Not found routine"          }, // 501
{ 0x6837,     -2, "Delete entry"               },
{ 0x6839,     -2, "Assign dev to a stream"     },
{ 0x683B,     -2, "Read next sector"           },
{ 0x6F81, 0X6F8D, "Device table"               },
{ 0x6F81,     -4, "VDU"                        },
{ 0x6F85,     -4, "Keyboard"                   },
{ 0x6F89,     -4, "Printer (LPT:)"             },
{ 0x7000,     -3, "Stream identity, 3 chars"   },
{ 0x7003,     -2, "Addr of service routine"    },
{ 0x7005,     -1, "Program Buffer PoinTer"     }, // 511
{ 0x7006,     -1, "Bit 1,0=drv #; bit 2=side"  },
{ 0x7007,     -1, "Track #"                    },
{ 0x7008,     -1, "Sector #"                   },
{ 0x7009,     -1, "Directory sector"           },
{ 0x700A,     -1, "Dir byte"                   },
{ 0x700B,     -1, "Length (for writes)"        },
{ 0x700C,     -1, "Length (for writes)"        },
{ 0x700D,     -1, "File type (0=ordinary)"     },
{ 0x700E,     -1, "Device Buffer PoinTer"      },
{ 0x700F,     -1, "High byte of buffer addr"   }, // 521
{ 0x7070,     -2, "Line buffer pointer"        },
{ 0x7072,    -40, "Normal line buffer"         },
{ 0x70B2,     -4, "Track info for dual drive"  },
{ 0x70B6,    -18, "Directory ENTry copy"       },
{ 0x70CE,     -4, "Scratch"                    },
{ 0x70D4,     -1, "Retry count"                },
{ 0x70D5,     -1, "Current drive"              },
{ 0x70D7,     -1, "Sys drive"                  },
{ 0x70D8,     -1, "Work dry"                   },
{ 0x70DB,     -2, "Disk buffer pointer"        }, // 531
{ 0x70DD,     -2, "Res of GNUM"                },
{ 0x70DF,     -1, "Error drive"                },
{ 0x70E0,     -1, "Error type"                 },
{ 0x70E1,     -1, "Error stream"               },
{ 0x70E2,     -1, "Error track"                },
{ 0x70E3,     -1, "Error sector"               },
{ 0x70E4,     -1, "1771 error status"          },
{ 0x70E5,     -1, "Error handling flag"        },
{ 0x70F0,     -3, "Printer COUT"               },
{ 0x70F3,     -3, "Printer Initialise"         }, // 541
{ 0x70F6,     -3, "Printer Terminate"          },
{ 0x70F9,     -3, "Test for break"             },
{ 0x70FC,     -1, "Date valid flag"            },
{ 0x70FD,     -3, "DD,MM,YY in BCD"            }, // 545 (LASTMICRODOSDATACOMMENT)
// ACOS (BINBUG) (546..592)
// ACOS itself
{  0x446,     -1, "Tape control:\n" \
                  "bit 7: no load\n" \
                  "bit 6: no block count\n" \
                  "bit 5: ignore error S+D\n" \
                  "bit 3: invert load phase\n" \
                  "bit 2: drive load\n" \
                  "bit 1: drive dump"          }, // 546 (FIRSTACOSDATACOMMENT)
{  0x447,     -1, "File type:\n" \
                  "Upper nybble:\n" \
                  "0 = object\n" \
                  "1..7 = data\n" \
                  "8 = source (assembler)\n" \
                  "9 = source (BASIC)\n" \
                  "$A..$F = source\n" \
                  "Lower nybble:\n" \
                  "bit 1: first block\n" \
                  "bit 0: last block"          },
{  0x448,  0x449, "CRC generate buffer"        },
{  0x44A,  0x44D, "Scratch pad"                },
{  0x44E,  0x45F, "Tape postamble"             },
{  0x44E,  0x44F, "CRC"                        },
{  0x450,     -1, "Block number"               },
{  0x452,  0x459, "Filename"                   },
{  0x45A,  0x45B, "Address"                    },
{  0x45C,     -1, "No. of bytes"               },
{  0x45D,     -1, "File ID"                    },
{  0x45E,  0x45F, "Execute address"            },
{  0x460,  0x55F, "Buffer"                     },
{ 0x60D4, 0x60DC, "Reserved"                   },
{ 0x6340,     -4, "1MHz timing constants"      },
// graphics driver
{ 0x6402,     -1, "Version number"             },
{ 0x6403, 0x6408, "Storage area for external access" },
{ 0x6403,     -1, "Function number"            },
{ 0x6404,     -2, "Start co-ordinate store"    },
{ 0x6406,     -2, "End co-ordinate store"      },
{ 0x6408,     -1, "Tested bit value"           },
{ 0x6409,     -1, "Internal use function index\n" \
                  "Functn no. * 3"             },
{ 0x641F, 0x645A, "Function jump table"        },
{ 0x64B4,     -2, "BINBUG scroll protect store"},
{ 0x64DC, 0x64E2, "Local storage for single point operations" },
{ 0x64DC,     -2, "Coords of point"            },
{ 0x64DE,     -2, "Pointer to VDU"             },
{ 0x64E0,     -2, "Pointer to VDU attributes"  },
{ 0x64E2,     -1, "Mask for \"bit within byte\""},
{ 0x652C, 0x6533, "Bit-mask table"             },
{ 0x65AC, 0x65B2, "Local variables used in line routine" },
{ 0x65AC,     -2, "Intermediate X & Y coords"  },
{ 0x65AE,     -1, "ABS(X2-X1)"                 },
{ 0x65AF,     -1, "ABS(Y2-Y1)"                 },
{ 0x65B0,     -1, "Horiz. direction flag"      },
{ 0x65B1,     -1, "Vertical direction flag"    },
{ 0x65B2,     -1, "Slope prediction counter"   },
{ 0x66E4, 0x66EF, "Local storage for circle routine" },
{ 0x66E4,     -1, "Loop counter store"         },
{ 0x66E6,     -3, "Sine value store"           },
{ 0x66E9,     -3, "Cosine store"               },
{ 0x66EC,     -2, "First plot point store"     },
{ 0x66EE,     -2, "Tentative new plot point"   },
{ 0x6703,     -3, "Input operand storage"      },
{ 0x6706,     -1, "Multiply loop count"        },
{ 0x7800,     -1, "Base of VDU"                },
{ 0x7C00, 0x7FFF, "VDU attribute RAM"          }, // 592 (LASTACOSDATACOMMENT)
// BINBUG 6.1 (593..618)
{   0x1B,     -2, "Breakpoint vectors indirectly through this ACON" }, // 593 (FIRSTBINBUG61DATACOMMENT)
{  0x259,  0x268, "ASCII list of hex digits"   },
{  0x2CC,  0x2CE, "Initial delay constants"    },
{  0x2CC,     -1, "150/300 baud"               },
{  0x2CD,     -1, "300/600 baud"               },
{  0x2CE,     -1, "1200/2400 baud"             },
{  0x3FF,     -1, "Version"                    },
{  0x400,  0x43C, "BINBUG SCRATCH AREA"        },
{  0x400,     -9, "Register & PSW stack"       },
{  0x409,     -1, "Output switch"              },
{  0x40A,     -3, "PSL restore instr."         },
{  0x40D,     -2, "16-bit address store"       },
{  0x413,    -20, "Input buffer"               },
{  0x427,     -1, "Buffer pointer"             },
{  0x42B,     -1, "Keyboard flag; 0 = Parallel"},
{  0x42C,     -1, "BP flag. $FF = BP set."     },
{  0x42D,  0x42E, "Serial I/O delay constants" },
{  0x42D,     -1, "1/2 MHz clock:\n" \
                  "$DD=150/300 baud\n" \
                  "$6E=300/600 baud\n" \
                  "$36=600/1200 baud\n" \
                  "$1A=1200/2400 baud\n" \
                  "$0C=2400/4800 baud"         },
{  0x42D,     -1, "1/2 MHz clock:\n" \
                  "$DA=150/300 baud\n" \
                  "$6B=300/600 baud\n" \
                  "$34=600/1200 baud\n" \
                  "$18=1200/2400 baud\n" \
                  "$0A=2400/4800 baud"         },
{  0x431,     -2, "Indirect pointer to extended command processor\n" \
                  "Normally $001D (EBUG)"      },
{  0x433,     -1, "Scroll flag - pxxfnnnn\n" \
                  "p: 1=page mode, 0=scroll mode\n" \
                  "f: 1=fast scroll, 0=normal scroll\n" \
                  "nnnn: number of unscrolled lines at top" },
{  0x434,  0x43F, "RAM not cleared from here on" },
{  0x434,     -2, "Bytes which were replaced by BP vector" },
{  0x436,     -2, "BP address"                 },
{  0x439,     -2, "Current cursor location"    },
{  0x43B,     -2, "Indirect to external COUT"  }, // 618 (LASTBINBUG61DATACOMMENT)
// PIPBUG 2 (619..631)
{    0x3,   0x16, "Address vector for the ZBRR and ZBSR instructions" }, // 619 (FIRSTPIPBUG2DATACOMMENT)
{    0x3,     -2, "Breakpoint one"             },
{    0x5,     -2, "Breakpoint two"             },
{    0x7,     -2, "Char output"                },
{    0x9,     -2, "Char input"                 },
{    0xB,     -2, "Byte output"                },
{    0xD,     -2, "Byte input"                 },
{    0xF,     -2, "Get a number subroutine"    },
{   0x11,     -2, "Get a cmd line"             },
{   0x13,     -2, "Address out"                },
{   0x15,     -2, "Get parameters"             },
{  0x112,  0x114, "End of line chars"          },
{  0x5CF,  0x7F5, "Tables"                     }, // 631 (LASTPIPBUG2DATACOMMENT)
// FIRSTGAMEDATACOMMENT (632)
// Examples 1 & 2 (Hobby Module) (0..1 + FIRSTGAMEDATACOMMENT)
{  0x912,  0x91F, "DATA table for object and duplicate"                                          },
{  0x938,  0x963, "Data table"                                                                   },
// Example 4 (Hobby Module) (2..6 + FIRSTGAMEDATACOMMENT)
{  0xFBA,  0xFBB, "PVI obj ind adr"                                                              }, // OBJP
{  0xFBC,  0xFBD, "Mat ind adr"                                                                  }, // OBJC
{  0xFBE,  0xFBF, "Flag adr"                                                                     }, // FLAG
{  0xFC0,  0xFC1, "Obj ind adr"                                                                  }, // VEC
{  0xFC2,  0xFC3, "Seq ind adr"                                                                  }, // SQAD
// Example 5 (Hobby Module) (7..15 + FIRSTGAMEDATACOMMENT)
{  0x906,     -1, "Countdown byte of flash mod"                                                  }, // FLASH
{  0x968,     -1, "Must be + or - key"                                                           }, // KEYTIM
{  0x971,     -1, "Save selected PVI address"                                                    }, // REG
{  0x983,     -1, "Count of reg. accessed by K..."                                               }, // INDEX
{  0x985,     -1, "Save of last select key val"                                                  }, // PREVK
{  0x9C7,  0x9E2, "Table containing max no of PVI regs(index) access..."                         }, // TBL1
{  0x9E3,  0x9FE, "Table containing addrs of 1st of PVI regs accesse..."                         }, // TBL2
{  0xA00,  0xACF, "Duplicate of PVI re..."                                                       }, // DUP
{ 0x1E88,     -1, "Keyboard matrix"                                                              }, // KEYB
// Billboard (SI50) (16..18 + FIRSTGAMEDATACOMMENT)
{  0x100,     -1, "Least significant byte of message pointer"                                    }, // PNTR
{  0x101,  0x11E, "This is the initial message in the buffer"                                    }, // MSG
{  0x11F,     -1, "End of message flag"                                                          },
// Clock (SI50) (19..28 + FIRSTGAMEDATACOMMENT)
{  0x100,  0x107, "Memory area for display buffer"                                               },
{  0x100,     -1, "Tens of hours"                                                                }, // THRS
{  0x101,     -1, "Unit hours"                                                                   }, // UHRS
{  0x102,     -1, "Tens of minutes"                                                              }, // TMIN
{  0x103,     -1, "Unit minutes"                                                                 }, // UMIN
{  0x104,     -1, "Tens of seconds"                                                              }, // TSEC
{  0x105,     -1, "Unit seconds"                                                                 }, // USEC
{  0x106,     -1, "Blank space"                                                                  }, // SPACE
{  0x107,     -1, "A or P symbol"                                                                }, // AMPM
{  0x108,     -1, "60ths of sec counter"                                                         }, // FRAC
// Olympics (Interton) (29..438 + FIRSTGAMEDATACOMMENT)
{  0x1B9,  0x1C4, "Calculated values for ball trajectories"                                      },
{  0x232,  0x262, "Values for collision computations"                                            },
{  0x32E,  0x34F, "Pin deletion and reflection by pin play"                                      },
{  0x350,  0x371, "Values for scattering by the pin edges"                                       },
{  0x600,     -1, "Value for determination of the play limit, Game 1"                            },
{  0x601,     -1, "Value for determination of the play limit, Game 2"                            },
{  0x602,     -1, "Value for determination of the play limit, Game 3"                            },
{  0x603,     -1, "Value for determination of the play limit, Game 4"                            },
{  0x604,     -1, "Value for determination of the play limit, Game 5"                            },
{  0x605,     -1, "Value for determination of the play limit, Game 6"                            },
{  0x606,     -1, "Value for determination of the play limit, Game 7"                            },
{  0x607,     -1, "Value for determination of the play limit, Game 8"                            },
{  0x608,     -1, "Value for determination of the play limit, Game 9"                            },
{  0x609,     -1, "Value for determination of the play limit, Game 10"                           },
{  0x60A,     -1, "Value for determination of the play limit, Game 11"                           },
{  0x60B,     -1, "Value for determination of the play limit, Game 12"                           },
{  0x60C,  0x611, "Values for determination of the bat's initial state, Games 1 & 3"             },
{  0x612,  0x617, "Values for determination of the bat's initial state, Games 2 & 4"             },
{  0x618,  0x61D, "Values for determination of the bat's initial state, Game 6"                  },
{  0x61E,  0x623, "Values for determination of the bat's initial state, Game 5"                  },
{  0x624,  0x629, "Values for determination of the bat's initial state, Games 7 & 8"             },
{  0x62A,  0x62F, "Values for determination of the bat's initial state, Games 9 & 10"            },
{  0x630,  0x635, "Values for determination of the bat's initial state, Game 11"                 },
{  0x636,  0x63B, "Values for determination of the bat's initial state, Game 12"                 },
{  0x63C,     -1, "Value for coordinate correction, Game 1"                                      },
{  0x63D,     -1, "Value for coordinate correction, Game 2"                                      },
{  0x63E,     -1, "Value for coordinate correction, Game 3"                                      },
{  0x63F,     -1, "Value for coordinate correction, Game 4"                                      },
{  0x640,     -1, "Value for coordinate correction, Game 5"                                      },
{  0x641,     -1, "Value for coordinate correction, Game 6"                                      },
{  0x642,     -1, "Value for coordinate correction, Game 7"                                      },
{  0x643,     -1, "Value for coordinate correction, Game 8"                                      },
{  0x644,     -1, "Value for coordinate correction, Game 9"                                      },
{  0x645,     -1, "Value for coordinate correction, Game 10"                                     },
{  0x646,     -1, "Value for coordinate correction, Game 11"                                     },
{  0x647,     -1, "Value for coordinate correction, Game 12"                                     },
{  0x648,     -1, "Foreground colour white, background colour blue, Game 1"                      },
{  0x649,     -1, "Foreground colour yellow, background colour red, Game 2"                      },
{  0x64A,     -1, "Foreground colour white, background colour red, Game 3"                       },
{  0x64B,     -1, "Foreground colour white, background colour purple, Game 4"                    },
{  0x64C,     -1, "Foreground colour red, background colour white, Game 5"                       },
{  0x64D,     -1, "Foreground colour black, background colour purple, Game 6"                    },
{  0x64E,     -1, "Foreground colour white, background colour blue, Game 7"                      },
{  0x64F,     -1, "Foreground colour red, background colour blue, Game 8"                        },
{  0x650,     -1, "Foreground colour black, background colour purple, Game 9"                    },
{  0x651,     -1, "Foreground colour yellow, background colour black, Game 10"                   },
{  0x652,     -1, "Foreground colour black, background colour green, Game 11"                    },
{  0x653,     -1, "Foreground colour purple, background colour white, Game 12"                   },
{  0x654,     -1, "Left bat: yellow, right bat: yellow, Game 1"                                  },
{  0x655,     -1, "Ball: green, Game 1"                                                          },
{  0x656,     -1, "Left bat: black, right bat: blue, Game 2"                                     },
{  0x657,     -1, "Ball: white, Game 2"                                                          },
{  0x658,     -1, "Left bat: blue, right bat: blue, Game 3"                                      },
{  0x659,     -1, "Ball: yellow, Game 3"                                                         },
{  0x65A,     -1, "Left bat: green, right bat: yellow, Game 4"                                   },
{  0x65B,     -1, "Ball: yellow, Game 4"                                                         },
{  0x65C,     -1, "Left bat: purple, right bat: purple, Game 5"                                  },
{  0x65D,     -1, "Ball: black, Game 5"                                                          },
{  0x65E,     -1, "Left bat: yellow, right bat: yellow, Game 6"                                  },
{  0x65F,     -1, "Ball: white, Game 6"                                                          },
{  0x660,     -1, "Left bat: yellow, right bat: white, Game 7"                                   },
{  0x661,     -1, "Ball: grün, Game 7"                                                           },
{  0x662,     -1, "Left bat: black, right bat: white, Game 8"                                    },
{  0x663,     -1, "Ball: yellow, Game 8"                                                         },
{  0x664,     -1, "No left bat, right bat: yellow, Game 9"                                       },
{  0x665,     -1, "Ball: yellow, Game 9"                                                         },
{  0x666,     -1, "No left bat, right bat: black, Game 10"                                       },
{  0x667,     -1, "Ball: red, Game 10"                                                           },
{  0x668,     -1, "Left bat: purple, right bat: purple, Game 11"                                 },
{  0x669,     -1, "Ball: white, Game 11"                                                         },
{  0x66A,     -1, "Left bat: red, right bat: green, Game 12"                                     },
{  0x66B,     -1, "Ball: green, Game 12"                                                         },
{  0x66C,     -1, "Correction value for playfield and collisions, Game 1"                        },
{  0x66D,     -1, "Correction value for playfield and collisions, Game 2"                        },
{  0x66E,     -1, "Correction value for playfield and collisions, Game 3"                        },
{  0x66F,     -1, "Correction value for playfield and collisions, Game 4"                        },
{  0x670,     -1, "Correction value for playfield and collisions, Game 5"                        },
{  0x671,     -1, "Correction value for playfield and collisions, Game 6"                        },
{  0x672,     -1, "Correction value for playfield and collisions, Game 7"                        },
{  0x673,     -1, "Correction value for playfield and collisions, Game 8"                        },
{  0x674,     -1, "Correction value for playfield and collisions, Game 9"                        },
{  0x675,     -1, "Correction value for playfield and collisions, Game 10"                       },
{  0x676,     -1, "Correction value for playfield and collisions, Game 11"                       },
{  0x677,     -1, "Correction value for playfield and collisions, Game 12"                       },
{  0x678,     -1, "Value for playfields 1 and 2 (left narrow)"                                   },
{  0x679,     -1, "Value for playfields 1 and 2 (right narrow)"                                  },
{  0x67A,     -1, "Value for playfields 1 and 2 (left wide)"                                     },
{  0x67B,     -1, "Value for playfields 1 and 2 (right wide)"                                    },
{  0x67C,     -1, "Value for playfields 1 and 2 (left narrow)"                                   },
{  0x67D,     -1, "Value for playfields 1 and 2 (right narrow)"                                  },
{  0x67E,     -1, "Value for playfields 1 and 2 (left wide)"                                     },
{  0x67F,     -1, "Value for playfields 1 and 2 (right wide)"                                    },
{  0x680,     -1, "Value for playfields 1 and 2 (left narrow)"                                   },
{  0x681,     -1, "Value for playfields 1 and 2 (right narrow)"                                  },
{  0x682,     -1, "Value for playfields 1 and 2 (left wide)"                                     },
{  0x683,     -1, "Value for playfields 1 and 2 (right wide)"                                    },
{  0x684,     -1, "Value for playfields 1 and 2 (left narrow)"                                   },
{  0x685,     -1, "Value for playfields 1 and 2 (right narrow)"                                  },
{  0x686,     -1, "Value for playfields 1 and 2 (left wide)"                                     },
{  0x687,     -1, "Value for playfields 1 and 2 (right wide)"                                    },
{  0x688,     -1, "Value for playfields 1 and 2 (left narrow)"                                   },
{  0x689,     -1, "Value for playfields 1 and 2 (right narrow)"                                  },
{  0x68A,     -1, "Value for playfields 1 and 2 (left wide)"                                     },
{  0x68B,     -1, "Value for playfields 1 and 2 (right wide)"                                    },
{  0x68C,     -1, "Value for playfields 1 and 2 (left narrow)"                                   },
{  0x68D,     -1, "Value for playfields 1 and 2 (right narrow)"                                  },
{  0x68E,     -1, "Value for playfields 1 and 2 (left wide)"                                     },
{  0x68F,     -1, "Value for playfields 1 and 2 (right wide)"                                    },
{  0x690,     -1, "Value for playfields 1 and 2 (left narrow)"                                   },
{  0x691,     -1, "Value for playfields 1 and 2 (right narrow)"                                  },
{  0x692,     -1, "Value for playfields 1 and 2 (left wide)"                                     },
{  0x693,     -1, "Value for playfields 1 and 2 (right wide)"                                    },
{  0x694,     -1, "Value for playfields 1 and 2 (left narrow)"                                   },
{  0x695,     -1, "Value for playfields 1 and 2 (right narrow)"                                  },
{  0x696,     -1, "Value for playfields 1 and 2 (left wide)"                                     },
{  0x697,     -1, "Value for playfields 1 and 2 (right wide)"                                    },
{  0x698,     -1, "Value for playfields 1 and 2 (left narrow)"                                   },
{  0x699,     -1, "Value for playfields 1 and 2 (right narrow)"                                  },
{  0x69A,     -1, "Value for playfields 1 and 2 (left wide)"                                     },
{  0x69B,     -1, "Value for playfields 1 and 2 (right wide)"                                    },
{  0x69C,  0x6A0, "Values for enhancement of the limit points"                                   },
{  0x6A1,     -1, "Value for playfields 3 and 4 (left narrow)"                                   },
{  0x6A2,     -1, "Value for playfields 3 and 4 (right narrow)"                                  },
{  0x6A3,     -1, "Value for playfields 3 and 4 (left wide)"                                     },
{  0x6A4,     -1, "Value for playfields 3 and 4 (right wide)"                                    },
{  0x6A5,     -1, "Value for playfields 3 and 4 (left narrow)"                                   },
{  0x6A6,     -1, "Value for playfields 3 and 4 (right narrow)"                                  },
{  0x6A7,     -1, "Value for playfields 3 and 4 (left wide)"                                     },
{  0x6A8,     -1, "Value for playfields 3 and 4 (right wide)"                                    },
{  0x6A9,     -1, "Value for playfields 3 and 4 (left narrow)"                                   },
{  0x6AA,     -1, "Value for playfields 3 and 4 (right narrow)"                                  },
{  0x6AB,     -1, "Value for playfields 3 and 4 (left wide)"                                     },
{  0x6AC,     -1, "Value for playfields 3 and 4 (right wide)"                                    },
{  0x6AD,     -1, "Value for playfields 3 and 4 (left narrow)"                                   },
{  0x6AE,     -1, "Value for playfields 3 and 4 (right narrow)"                                  },
{  0x6AF,     -1, "Value for playfields 3 and 4 (left wide)"                                     },
{  0x6B0,     -1, "Value for playfields 3 and 4 (right wide)"                                    },
{  0x6B1,     -1, "Value for playfields 3 and 4 (left narrow)"                                   },
{  0x6B2,     -1, "Value for playfields 3 and 4 (right narrow)"                                  },
{  0x6B3,     -1, "Value for playfields 3 and 4 (left wide)"                                     },
{  0x6B4,     -1, "Value for playfields 3 and 4 (right wide)"                                    },
{  0x6B5,     -1, "Value for playfields 3 and 4 (left narrow)"                                   },
{  0x6B6,     -1, "Value for playfields 3 and 4 (right narrow)"                                  },
{  0x6B7,     -1, "Value for playfields 3 and 4 (left wide)"                                     },
{  0x6B8,     -1, "Value for playfields 3 and 4 (right wide)"                                    },
{  0x6B9,     -1, "Value for playfields 3 and 4 (left narrow)"                                   },
{  0x6BA,     -1, "Value for playfields 3 and 4 (right narrow)"                                  },
{  0x6BB,     -1, "Value for playfields 3 and 4 (left wide)"                                     },
{  0x6BC,     -1, "Value for playfields 3 and 4 (right wide)"                                    },
{  0x6BD,     -1, "Value for playfields 3 and 4 (left narrow)"                                   },
{  0x6BE,     -1, "Value for playfields 3 and 4 (right narrow)"                                  },
{  0x6BF,     -1, "Value for playfields 3 and 4 (left wide)"                                     },
{  0x6C0,     -1, "Value for playfields 3 and 4 (right wide)"                                    },
{  0x6C1,     -1, "Value for playfields 3 and 4 (left narrow)"                                   },
{  0x6C2,     -1, "Value for playfields 3 and 4 (right narrow)"                                  },
{  0x6C3,     -1, "Value for playfields 3 and 4 (left wide)"                                     },
{  0x6C4,     -1, "Value for playfields 3 and 4 (right wide)"                                    },
{  0x6C5,  0x6C9, "Values for enhancement of the limit points"                                   },
{  0x6CA,     -1, "Value for playfields 5 and 6 (left narrow)"                                   },
{  0x6CB,     -1, "Value for playfields 5 and 6 (right narrow)"                                  },
{  0x6CC,     -1, "Value for playfields 5 and 6 (left wide)"                                     },
{  0x6CD,     -1, "Value for playfields 5 and 6 (right wide)"                                    },
{  0x6CE,     -1, "Value for playfields 5 and 6 (left narrow)"                                   },
{  0x6CF,     -1, "Value for playfields 5 and 6 (right narrow)"                                  },
{  0x6D0,     -1, "Value for playfields 5 and 6 (left wide)"                                     },
{  0x6D1,     -1, "Value for playfields 5 and 6 (right wide)"                                    },
{  0x6D2,     -1, "Value for playfields 5 and 6 (left narrow)"                                   },
{  0x6D3,     -1, "Value for playfields 5 and 6 (right narrow)"                                  },
{  0x6D4,     -1, "Value for playfields 5 and 6 (left wide)"                                     },
{  0x6D5,     -1, "Value for playfields 5 and 6 (right wide)"                                    },
{  0x6D6,     -1, "Value for playfields 5 and 6 (left narrow)"                                   },
{  0x6D7,     -1, "Value for playfields 5 and 6 (right narrow)"                                  },
{  0x6D8,     -1, "Value for playfields 5 and 6 (left wide)"                                     },
{  0x6D9,     -1, "Value for playfields 5 and 6 (right wide)"                                    },
{  0x6DA,     -1, "Value for playfields 5 and 6 (left narrow)"                                   },
{  0x6DB,     -1, "Value for playfields 5 and 6 (right narrow)"                                  },
{  0x6DC,     -1, "Value for playfields 5 and 6 (left wide)"                                     },
{  0x6DD,     -1, "Value for playfields 5 and 6 (right wide)"                                    },
{  0x6DE,     -1, "Value for playfields 5 and 6 (left narrow)"                                   },
{  0x6DF,     -1, "Value for playfields 5 and 6 (right narrow)"                                  },
{  0x6E0,     -1, "Value for playfields 5 and 6 (left wide)"                                     },
{  0x6E1,     -1, "Value for playfields 5 and 6 (right wide)"                                    },
{  0x6E2,     -1, "Value for playfields 5 and 6 (left narrow)"                                   },
{  0x6E3,     -1, "Value for playfields 5 and 6 (right narrow)"                                  },
{  0x6E4,     -1, "Value for playfields 5 and 6 (left wide)"                                     },
{  0x6E5,     -1, "Value for playfields 5 and 6 (right wide)"                                    },
{  0x6E6,     -1, "Value for playfields 5 and 6 (left narrow)"                                   },
{  0x6E7,     -1, "Value for playfields 5 and 6 (right narrow)"                                  },
{  0x6E8,     -1, "Value for playfields 5 and 6 (left wide)"                                     },
{  0x6E9,     -1, "Value for playfields 5 and 6 (right wide)"                                    },
{  0x6EA,     -1, "Value for playfields 5 and 6 (left narrow)"                                   },
{  0x6EB,     -1, "Value for playfields 5 and 6 (right narrow)"                                  },
{  0x6EC,     -1, "Value for playfields 5 and 6 (left wide)"                                     },
{  0x6ED,     -1, "Value for playfields 5 and 6 (right wide)"                                    },
{  0x6EE,  0x6F2, "Values for enhancement of the limit points"                                   },
{  0x6F3,     -1, "Value for playfields 7 and 8 (right narrow)"                                  },
{  0x6F4,     -1, "Value for playfields 7 and 8 (left wide)"                                     },
{  0x6F5,     -1, "Value for playfields 7 and 8 (right wide)"                                    },
{  0x6F6,     -1, "Value for playfields 7 and 8 (left narrow)"                                   },
{  0x6F7,     -1, "Value for playfields 7 and 8 (right narrow)"                                  },
{  0x6F8,     -1, "Value for playfields 7 and 8 (left wide)"                                     },
{  0x6F9,     -1, "Value for playfields 7 and 8 (right wide)"                                    },
{  0x6FA,     -1, "Value for playfields 7 and 8 (left narrow)"                                   },
{  0x6FB,     -1, "Value for playfields 7 and 8 (right narrow)"                                  },
{  0x6FC,     -1, "Value for playfields 7 and 8 (left wide)"                                     },
{  0x6FD,     -1, "Value for playfields 7 and 8 (right wide)"                                    },
{  0x6FE,     -1, "Value for playfields 7 and 8 (left narrow)"                                   },
{  0x6FF,     -1, "Value for playfields 7 and 8 (right narrow)"                                  },
{  0x700,     -1, "Value for playfields 7 and 8 (left wide)"                                     },
{  0x701,     -1, "Value for playfields 7 and 8 (right wide)"                                    },
{  0x702,     -1, "Value for playfields 7 and 8 (left narrow)"                                   },
{  0x703,     -1, "Value for playfields 7 and 8 (right narrow)"                                  },
{  0x704,     -1, "Value for playfields 7 and 8 (left wide)"                                     },
{  0x705,     -1, "Value for playfields 7 and 8 (right wide)"                                    },
{  0x706,     -1, "Value for playfields 7 and 8 (left narrow)"                                   },
{  0x707,     -1, "Value for playfields 7 and 8 (right narrow)"                                  },
{  0x708,     -1, "Value for playfields 7 and 8 (left wide)"                                     },
{  0x709,     -1, "Value for playfields 7 and 8 (right wide)"                                    },
{  0x70A,     -1, "Value for playfields 7 and 8 (left narrow)"                                   },
{  0x70B,     -1, "Value for playfields 7 and 8 (right narrow)"                                  },
{  0x70C,     -1, "Value for playfields 7 and 8 (left wide)"                                     },
{  0x70D,     -1, "Value for playfields 7 and 8 (right wide)"                                    },
{  0x70E,     -1, "Value for playfields 7 and 8 (left narrow)"                                   },
{  0x70F,     -1, "Value for playfields 7 and 8 (right narrow)"                                  },
{  0x710,     -1, "Value for playfields 7 and 8 (left wide)"                                     },
{  0x711,     -1, "Value for playfields 7 and 8 (right wide)"                                    },
{  0x712,     -1, "Value for playfields 7 and 8 (left narrow)"                                   },
{  0x713,     -1, "Value for playfields 7 and 8 (right narrow)"                                  },
{  0x714,     -1, "Value for playfields 7 and 8 (left wide)"                                     },
{  0x715,     -1, "Value for playfields 7 and 8 (right wide)"                                    },
{  0x716,     -1, "Value for playfields 7 and 8 (left narrow)"                                   },
{  0x717,  0x71B, "Values for enhancement of the limit points"                                   },
{  0x71C,     -1, "Value for playfield 9 (right narrow)"                                         },
{  0x71D,     -1, "Value for playfield 9 (left wide)"                                            },
{  0x71E,     -1, "Value for playfield 9 (right wide)"                                           },
{  0x71F,     -1, "Value for playfield 9 (left narrow)"                                          },
{  0x720,     -1, "Value for playfield 9 (right narrow)"                                         },
{  0x721,     -1, "Value for playfield 9 (left wide)"                                            },
{  0x722,     -1, "Value for playfield 9 (right wide)"                                           },
{  0x723,     -1, "Value for playfield 9 (left narrow)"                                          },
{  0x724,     -1, "Value for playfield 9 (right narrow)"                                         },
{  0x725,     -1, "Value for playfield 9 (left wide)"                                            },
{  0x726,     -1, "Value for playfield 9 (right wide)"                                           },
{  0x727,     -1, "Value for playfield 9 (left narrow)"                                          },
{  0x728,     -1, "Value for playfield 9 (right narrow)"                                         },
{  0x729,     -1, "Value for playfield 9 (left wide)"                                            },
{  0x72A,     -1, "Value for playfield 9 (right wide)"                                           },
{  0x72B,     -1, "Value for playfield 9 (left narrow)"                                          },
{  0x72C,     -1, "Value for playfield 9 (right narrow)"                                         },
{  0x72D,     -1, "Value for playfield 9 (left wide)"                                            },
{  0x72E,     -1, "Value for playfield 9 (right wide)"                                           },
{  0x72F,     -1, "Value for playfield 9 (left narrow)"                                          },
{  0x730,     -1, "Value for playfield 9 (right narrow)"                                         },
{  0x731,     -1, "Value for playfield 9 (left wide)"                                            },
{  0x732,     -1, "Value for playfield 9 (right wide)"                                           },
{  0x733,     -1, "Value for playfield 9 (left narrow)"                                          },
{  0x734,     -1, "Value for playfield 9 (right narrow)"                                         },
{  0x735,     -1, "Value for playfield 9 (left wide)"                                            },
{  0x736,     -1, "Value for playfield 9 (right wide)"                                           },
{  0x737,     -1, "Value for playfield 9 (left narrow)"                                          },
{  0x738,     -1, "Value for playfield 9 (right narrow)"                                         },
{  0x739,     -1, "Value for playfield 9 (left wide)"                                            },
{  0x73A,     -1, "Value for playfield 9 (right wide)"                                           },
{  0x73B,     -1, "Value for playfield 9 (left narrow)"                                          },
{  0x73C,     -1, "Value for playfield 9 (right narrow)"                                         },
{  0x73D,     -1, "Value for playfield 9 (left wide)"                                            },
{  0x73E,     -1, "Value for playfield 9 (right wide)"                                           },
{  0x73F,     -1, "Value for playfield 9 (left narrow)"                                          },
{  0x740,  0x744, "Values for enhancement of the limit points"                                   },
{  0x745,     -1, "Value for playfield 10 (right narrow)"                                        },
{  0x746,     -1, "Value for playfield 10 (left wide)"                                           },
{  0x747,     -1, "Value for playfield 10 (right wide)"                                          },
{  0x748,     -1, "Value for playfield 10 (left narrow)"                                         },
{  0x749,     -1, "Value for playfield 10 (right narrow)"                                        },
{  0x74A,     -1, "Value for playfield 10 (left wide)"                                           },
{  0x74B,     -1, "Value for playfield 10 (right wide)"                                          },
{  0x74C,     -1, "Value for playfield 10 (left narrow)"                                         },
{  0x74D,     -1, "Value for playfield 10 (right narrow)"                                        },
{  0x74E,     -1, "Value for playfield 10 (left wide)"                                           },
{  0x74F,     -1, "Value for playfield 10 (right wide)"                                          },
{  0x750,     -1, "Value for playfield 10 (left narrow)"                                         },
{  0x751,     -1, "Value for playfield 10 (right narrow)"                                        },
{  0x752,     -1, "Value for playfield 10 (left wide)"                                           },
{  0x753,     -1, "Value for playfield 10 (right wide)"                                          },
{  0x754,     -1, "Value for playfield 10 (left narrow)"                                         },
{  0x755,     -1, "Value for playfield 10 (right narrow)"                                        },
{  0x756,     -1, "Value for playfield 10 (left wide)"                                           },
{  0x757,     -1, "Value for playfield 10 (right wide)"                                          },
{  0x758,     -1, "Value for playfield 10 (left narrow)"                                         },
{  0x759,     -1, "Value for playfield 10 (right narrow)"                                        },
{  0x75A,     -1, "Value for playfield 10 (left wide)"                                           },
{  0x75B,     -1, "Value for playfield 10 (right wide)"                                          },
{  0x75C,     -1, "Value for playfield 10 (left narrow)"                                         },
{  0x75D,     -1, "Value for playfield 10 (right narrow)"                                        },
{  0x75E,     -1, "Value for playfield 10 (left wide)"                                           },
{  0x75F,     -1, "Value for playfield 10 (right wide)"                                          },
{  0x760,     -1, "Value for playfield 10 (left narrow)"                                         },
{  0x761,     -1, "Value for playfield 10 (right narrow)"                                        },
{  0x762,     -1, "Value for playfield 10 (left wide)"                                           },
{  0x763,     -1, "Value for playfield 10 (right wide)"                                          },
{  0x764,     -1, "Value for playfield 10 (left narrow)"                                         },
{  0x765,     -1, "Value for playfield 10 (right narrow)"                                        },
{  0x766,     -1, "Value for playfield 10 (left wide)"                                           },
{  0x767,     -1, "Value for playfield 10 (right wide)"                                          },
{  0x768,     -1, "Value for playfield 10 (left narrow)"                                         },
{  0x769,  0x76D, "Values for enhancement of the limit points"                                   },
{  0x76E,     -1, "Value for playfield 11 (right narrow)"                                        },
{  0x76F,     -1, "Value for playfield 11 (left wide)"                                           },
{  0x770,     -1, "Value for playfield 11 (right wide)"                                          },
{  0x771,     -1, "Value for playfield 11 (left narrow)"                                         },
{  0x772,     -1, "Value for playfield 11 (right narrow)"                                        },
{  0x773,     -1, "Value for playfield 11 (left wide)"                                           },
{  0x774,     -1, "Value for playfield 11 (right wide)"                                          },
{  0x775,     -1, "Value for playfield 11 (left narrow)"                                         },
{  0x776,     -1, "Value for playfield 11 (right narrow)"                                        },
{  0x777,     -1, "Value for playfield 11 (left wide)"                                           },
{  0x778,     -1, "Value for playfield 11 (right wide)"                                          },
{  0x779,     -1, "Value for playfield 11 (left narrow)"                                         },
{  0x77A,     -1, "Value for playfield 11 (right narrow)"                                        },
{  0x77B,     -1, "Value for playfield 11 (left wide)"                                           },
{  0x77C,     -1, "Value for playfield 11 (right wide)"                                          },
{  0x77D,     -1, "Value for playfield 11 (left narrow)"                                         },
{  0x77E,     -1, "Value for playfield 11 (right narrow)"                                        },
{  0x77F,     -1, "Value for playfield 11 (left wide)"                                           },
{  0x780,     -1, "Value for playfield 11 (right wide)"                                          },
{  0x781,     -1, "Value for playfield 11 (left narrow)"                                         },
{  0x782,     -1, "Value for playfield 11 (right narrow)"                                        },
{  0x783,     -1, "Value for playfield 11 (left wide)"                                           },
{  0x784,     -1, "Value for playfield 11 (right wide)"                                          },
{  0x785,     -1, "Value for playfield 11 (left narrow)"                                         },
{  0x786,     -1, "Value for playfield 11 (right narrow)"                                        },
{  0x787,     -1, "Value for playfield 11 (left wide)"                                           },
{  0x788,     -1, "Value for playfield 11 (right wide)"                                          },
{  0x789,     -1, "Value for playfield 11 (left narrow)"                                         },
{  0x78A,     -1, "Value for playfield 11 (right narrow)"                                        },
{  0x78B,     -1, "Value for playfield 11 (left wide)"                                           },
{  0x78C,     -1, "Value for playfield 11 (right wide)"                                          },
{  0x78D,     -1, "Value for playfield 11 (left narrow)"                                         },
{  0x78E,     -1, "Value for playfield 11 (right narrow)"                                        },
{  0x78F,     -1, "Value for playfield 11 (left wide)"                                           },
{  0x790,     -1, "Value for playfield 11 (right wide)"                                          },
{  0x791,     -1, "Value for playfield 11 (left narrow)"                                         },
{  0x792,  0x796, "Values for enhancement of the limit points"                                   },
{  0x797,     -1, "Value for playfield 12 (right narrow)"                                        },
{  0x798,     -1, "Value for playfield 12 (left wide)"                                           },
{  0x799,     -1, "Value for playfield 12 (right wide)"                                          },
{  0x79A,     -1, "Value for playfield 12 (left narrow)"                                         },
{  0x79B,     -1, "Value for playfield 12 (right narrow)"                                        },
{  0x79C,     -1, "Value for playfield 12 (left wide)"                                           },
{  0x79D,     -1, "Value for playfield 12 (right wide)"                                          },
{  0x79E,     -1, "Value for playfield 12 (left narrow)"                                         },
{  0x79F,     -1, "Value for playfield 12 (right narrow)"                                        },
{  0x7A0,     -1, "Value for playfield 12 (left wide)"                                           },
{  0x7A1,     -1, "Value for playfield 12 (right wide)"                                          },
{  0x7A2,     -1, "Value for playfield 12 (left narrow)"                                         },
{  0x7A3,     -1, "Value for playfield 12 (right narrow)"                                        },
{  0x7A4,     -1, "Value for playfield 12 (left wide)"                                           },
{  0x7A5,     -1, "Value for playfield 12 (right wide)"                                          },
{  0x7A6,     -1, "Value for playfield 12 (left narrow)"                                         },
{  0x7A7,     -1, "Value for playfield 12 (right narrow)"                                        },
{  0x7A8,     -1, "Value for playfield 12 (left wide)"                                           },
{  0x7A9,     -1, "Value for playfield 12 (right wide)"                                          },
{  0x7AA,     -1, "Value for playfield 12 (left narrow)"                                         },
{  0x7AB,     -1, "Value for playfield 12 (right narrow)"                                        },
{  0x7AC,     -1, "Value for playfield 12 (left wide)"                                           },
{  0x7AD,     -1, "Value for playfield 12 (right wide)"                                          },
{  0x7AE,     -1, "Value for playfield 12 (left narrow)"                                         },
{  0x7AF,     -1, "Value for playfield 12 (right narrow)"                                        },
{  0x7B0,     -1, "Value for playfield 12 (left wide)"                                           },
{  0x7B1,     -1, "Value for playfield 12 (right wide)"                                          },
{  0x7B2,     -1, "Value for playfield 12 (left narrow)"                                         },
{  0x7B3,     -1, "Value for playfield 12 (right narrow)"                                        },
{  0x7B4,     -1, "Value for playfield 12 (left wide)"                                           },
{  0x7B5,     -1, "Value for playfield 12 (right wide)"                                          },
{  0x7B6,     -1, "Value for playfield 12 (left narrow)"                                         },
{  0x7B7,     -1, "Value for playfield 12 (right narrow)"                                        },
{  0x7B8,     -1, "Value for playfield 12 (left wide)"                                           },
{  0x7B9,     -1, "Value for playfield 12 (right wide)"                                          },
{  0x7BA,     -1, "Value for playfield 12 (left narrow)"                                         },
{  0x7BB,  0x7BF, "Values for enhancement of the limit points"                                   },
{  0x7C0,  0x7C1, "Values for points generation"                                                 },
{  0x7C2,  0x7C5, "Values for starting point"                                                    },
{  0x7C6,     -1, "Bat shape for Game 1"                                                         },
{  0x7C7,     -1, "Bat shape for Game 2"                                                         },
{  0x7C8,     -1, "Bat shape for Game 3"                                                         },
{  0x7C9,     -1, "Bat shape for Game 4"                                                         },
{  0x7CA,     -1, "Bat shape for Game 5"                                                         },
{  0x7CB,     -1, "Bat shape for Game 6"                                                         },
{  0x7CC,     -1, "Bat shape for Game 7"                                                         },
{  0x7CD,     -1, "Bat shape for Game 8"                                                         },
{  0x7CE,     -1, "Bat shape for Game 9"                                                         },
{  0x7CF,     -1, "Bat shape for Game 10"                                                        },
{  0x7D0,     -1, "Bat shape for Game 11"                                                        },
{  0x7D1,     -1, "Bat shape for Game 12"                                                        },
{  0x7D2,  0x7DF, "Widths of bats"                                                               },
{  0x7E0,  0x7E6, "Bat shapes for Game 6"                                                        },
{  0x7E7,  0x7EF, "Bat distances for Games 2 and 4"                                              },
{  0x7F0,     -1, "Description of numerical display for Game 1"                                  },
{  0x7F1,     -1, "Description of numerical display for Game 2"                                  },
{  0x7F2,     -1, "Description of numerical display for Game 3"                                  },
{  0x7F3,     -1, "Description of numerical display for Game 4"                                  },
{  0x7F4,     -1, "Description of numerical display for Game 5"                                  },
{  0x7F5,     -1, "Description of numerical display for Game 6"                                  },
{  0x7F6,     -1, "Description of numerical display for Game 7"                                  },
{  0x7F7,     -1, "Description of numerical display for Game 8"                                  },
{  0x7F8,     -1, "Description of numerical display for Game 9"                                  },
{  0x7F9,     -1, "Description of numerical display for Game 10"                                 },
{  0x7FA,     -1, "Description of numerical display for Game 11"                                 },
{  0x7FB,     -1, "Description of numerical display for Game 12"                                 },
{  0x7FC,  0x7FF, "Free"                                                                         },
// Crap Game (SI50) (439..466 + FIRSTGAMEDATACOMMENT)
{    0x2,    0x3, "Roll the dice"                                                                },
{    0x4,    0x5, "Display message"                                                              },
{    0x6,    0x7, "Assemble roll"                                                                },
{    0x8,    0x9, "Decode the roll"                                                              },
{    0xA,    0xB, "Buy into game"                                                                },
{    0xC,    0xD, "Calc. win/loss"                                                               },
{    0xE,     -1, "Current bet"                                                                  },
{    0xF,     -1, "Chips on hand"                                                                },
{   0x41,     -1, "PASS 1 IDENT constant loc."                                                   },
{   0xCD,   0xCE, "*NEWGAME"                                                                     },
{   0xCF,     -1, "Filler data"                                                                  },
{  0x105,  0x10C, "CHIPS MESSAGE: \"U BUY = \""                                                  },
{  0x11D,     -1, "Filler data"                                                                  },
{  0x11E,  0x125, "Decode roll fix to expand - documented in DCDROL subroutine"                  },
{  0x181,     -1, "Saved 1st roll of dice"                                                       },
{  0x182,     -1, "Value of dice added from current roll"                                        },
{  0x1A2,  0x1A9, "\"ROLL X X\" message\n"                                                       \
                  "Variables into ROLL+5, ROLL+7 = indiv. dice"                                  },
{  0x1AF,     -1, "Loc. for dice table const. ind."                                              },
{  0x1B0,     -1, "Loc. for roll dice delay"                                                     },
{  0x1DB,  0x1FF, "DICE TaBle holds all possible dice combinations + 1 extra 7\n"                \
                  "Access by RLDICE subroutine only"                                             },
{ 0x1780, 0x1787, "\"PLACE BET\""                                                                },
{ 0x1788, 0x178F, "\"XX BET Y\"\nXX = chips on hand\nY = current bet"                            },
{ 0x1790, 0x1797, "\"PASS 1 =\""                                                                 },
{ 0x1798, 0x179F, "\"X--X =YY\"\nX+X = indiv. dice\nYY = added dice"                             },
{ 0x17A0, 0x17A7, "\"YOU BEAT\""                                                                 },
{ 0x17A8, 0x17AF, "\"THEHOUSE\""                                                                 },
{ 0x17B0, 0x17B7, "\"U PAY X\"\n"                                                                \
                  "X = bet he lost"                                                              },
{ 0x17B8, 0x17BF, "\"SHOOT XX\"\n"                                                               \
                  "XX = the point player must make to win on second or greater pass"             },
// Train (SI50) (467..487)
{    0x6,     -1, "Temporary speed storage"                                                      },
{    0x7,    0x8, "Switchyard intpt. address"                                                    },
{   0x38,   0x3F, "Original message at start-up: \".2650 UP.\""                                  },
{   0x40,   0x47, "8-byte temporary data table for SWYARD motion and direction control"          },
{   0x80,   0xFF, "128-byte table (16*8) reserved for train consist selection"                   },
{   0x80,   0x87, "CONSIST 0\nHigh Speed Engine only"                                            },
{   0x88,   0x8F, "CONSIST 1\nHS Engine + 3 full freight cars (FC)"                              },
{   0x90,   0x97, "CONSIST 2\nHSE + 1 FC + 2 Empty Cars (EC)"                                    },
{   0x98,   0x9F, "CONSIST 3\nHSE + 1 FC + 1 EC + 1 half full car (½) + 1 Caboose (C)"           },
{   0xA0,   0xA7, "CONSIST 4\nHSE + 2 FC + 1 C"                                                  },
{   0xA8,   0xAF, "CONSIST 5\nHSE + 3 C"                                                         },
{   0xB0,   0xB7, "CONSIST 6\nHSE + 3 EC + C"                                                    },
{   0xB8,   0xBF, "CONSIST 7\nLow speed engine (LSE) plus 3 C"                                   },
{   0xC0,   0xC7, "CONSIST 8\nLSE + 2 EC + C"                                                    },
{   0xC8,   0xCF, "CONSIST 9\nLSE + 2 ½ + C"                                                     },
{   0xD0,   0xD7, "CONSIST 10\nLSE + 2 EC + C"                                                   },
{   0xD8,   0xDF, "CONSIST 11\nLSE + 3 FC + C"                                                   },
{   0xE0,   0xE7, "CONSIST 12\nLSE (alone)"                                                      },
{   0xE8,   0xEF, "CONSIST 13\nMine engine (M) alone"                                            },
{   0xF0,   0xF7, "CONSIST 14\nElectric M + 4 C"                                                 },
{   0xF8,   0xFF, "CONSIST 15\nM + 3 C"                                                          },
// Beat the Odds (SI50) (original and variation) (488..489)
{   0x70,   0x77, "\"ROLL =__\" message"                                                         },
{   0x78,   0x7F, "\"YOU HOPE\" message"                                                         },
// Beat the Odds (SI50) (variation only) (490..491)
{   0x8E,   0x95, "Message is \"PLACE BET\"\nDepress any hex or function key to start roll"      },
{   0xAE,   0xB5, "Message \"ROLL =__\" flashes for ½ sec then program exits to roll for odds"   },
// 8K BASIC (CD2650) (492..559)
{ 0x2006, 0x2014, "'BLOCKS LEFT:  '"},
{ 0x2015, 0x201F, "'COMMAND:  '"},
{ 0x2020, 0x2023, "' OK'"},
{ 0x2024, 0x202C, "'TOO MUCH'"},
{ 0x202D, 0x203A, "'SUMHECK ERROR'"}, // sic
{ 0x2735,     -1, "Keep this in for the PRINT routine"},
{ 0x293A, 0x293F, "'LET.'"},
{ 0x2940, 0x2947, "'PRINT.'"},
{ 0x2948, 0x294C, "'IF.'"},
{ 0x294D, 0x2953, "'GOTO.'"},
{ 0x2954, 0x295B, "'GOSUB.'"},
{ 0x295C, 0x2964, "'RETURN.'"},
{ 0x2965, 0x296A, "'FOR.'"},
{ 0x296B, 0x2971, "'NEXT.'"},
{ 0x2972, 0x2979, "'INPUT.'"},
{ 0x297A, 0x2980, "'DATA.'"},
{ 0x2981, 0x2988, "'ERASE.'"},
{ 0x2989, 0x298F, "'READ.'"},
{ 0x2990, 0x2996, "'PEEK.'"},
{ 0x2997, 0x299D, "'POKE.'"},
{ 0x299E, 0x29A5, "'EXTIN.'"},
{ 0x29A6, 0x29AE, "'EXTOUT.'"},
{ 0x29AF, 0x29B4, "'DIM.'"},
{ 0x29B5, 0x29BE, "'RESTORE.'"},
{ 0x29BF, 0x29C5, "'CALL.'"},
{ 0x29C6, 0x29CC, "'STOP.'"},
{ 0x29CE, 0x29D4, "'DATA.'"},
{ 0x29D5, 0x29D9, "'TO.'"},
{ 0x29DA, 0x29E0, "'STEP.'"},
{ 0x29E1, 0x29E7, "'STOP.'"},
{ 0x30ED, 0x30F1, "'NEST.'"},
{ 0x30F2, 0x30FB, "'OPTION   .'"},
{ 0x30FC, 0x3103, "'STK OVF.'"},
{ 0x3104, 0x3110, "'LEN MISMATCH.'"},
{ 0x3111, 0x3115, "'LN #.'"},
{ 0x3116, 0x311B, "' *** .'"},
{ 0x311C, 0x3126, "' EXECUTING.'"},
{ 0x3127, 0x3130, "' ERROR ***.'"},
{ 0x3133, 0x313F, "'NO STOP STMT.'"},
{ 0x3116, 0x3148, "'STEP MODE'"},
{ 0x314C, 0x315B, "'BKPT AT LINE # .'"},
{ 0x315C, 0x3167, "'OUT OF DATA.'"},
{ 0x3168, 0x316B, "'ARG.'"},
{ 0x316C, 0x3178, "'INSPECT VAR .'"},
{ 0x317A, 0x3181, "'STOPPED.'"},
{ 0x3182, 0x318E, "'LINE NOT FND.'"},
{ 0x318F, 0x319C, "'TYPE MISMATCH.'"},
{ 0x319D, 0x31A3, "'SYNTAX.'"},
{ 0x31A4, 0x31B0, "'TOO MANY OPS.'"},
{ 0x31B1, 0x31BC, "'DVD BY ZERO.'"},
{ 0x31BD, 0x31C0, "'OVF.'"},
{ 0x31C1, 0x31C7, "'VAR NM.'"},
{ 0x3668, 0x366D, "'EXP.'"},
{ 0x366E, 0x3673, "'COS.'"},
{ 0x3674, 0x3679, "'SIN.'"},
{ 0x367A, 0x367F, "'TAN.'"},
{ 0x3680, 0x3685, "'ABS.'"},
{ 0x3686, 0x368B, "'SGN.'"},
{ 0x368C, 0x3691, "'RND.'"},
{ 0x3692, 0x3697, "'SQR.'"},
{ 0x3698, 0x369E, "'INT.'"},
{ 0x369F, 0x36A3, "'FRC.'"},
{ 0x36A4, 0x36A8, "'LN.'"},
{ 0x36A9, 0x36AE, "'LOG.'"},
{ 0x3DA6, 0x3DBD, "SINFAC"},
{ 0x3DBE, 0x3DD5, "COSFAC"},
{ 0x3F16, 0x3F2D, "LNFAC"},
{ 0x3F7F, 0x3F96, "EXPFAC"},
// Dragster (Elektor) (560..587 + FIRSTGAMEDATACOMMENT)
{ 0x1F0E,     -1, "Erasure store for R0 from main program"                                       },
{ 0x1F0F,     -1, "Joystick data"                                                                },
{ 0x1F1E,     -1, "Gengenzeige (left and right half)"                                            },
{ 0x1F1F,     -1, "Flags:\n"                                                                     \
                  "bit 7: Key depressed flag left\n"                                             \
                  "bit 6: Key depressed flag right\n"                                            \
                  "bit 5: Round counter flag left\n"                                             \
                  "bit 4: Round counter flag right\n"                                            \
                  "bit 3: Colour inversion\n"                                                    \
                  "bit 2: Stop flag left player\n"                                               \
                  "bit 1: Stop flag right player\n"                                              \
                  "bit 0: Start flag"                                                            },
{ 0x1F4E,     -1, "Main clock (right part)"                                                      },
{ 0x1F4F,     -1, "Main clock (left part)"                                                       },
{ 0x1F50,     -1, "Route clock left (right part)"                                                },
{ 0x1F51,     -1, "Route clock left (left part)"                                                 },
{ 0x1F52,     -1, "Route clock right (right part)"                                               },
{ 0x1F53,     -1, "Route clock right (left part)"                                                },
{ 0x1F54,     -1, "Universal counter (erasable, ?)"                                              },
{ 0x1F55,     -1, "Timing counter (before)"                                                      },
{ 0x1F56,     -1, "Offset Dragster (left + right half):\n"                                       \
                  "0 = Dragster\n"                                                               \
                  "1 = Stall\n"                                                                  \
                  "2 = Blown\n"                                                                  \
                  "3 = ?\n"                                                                      \
                  "4 = Gear"                                                                     },
{ 0x1F57,     -1, "Dragster position left"                                                       },
{ 0x1F58,     -1, "Dragster position right"                                                      },
{ 0x1F59,     -1, "Speed position left"                                                          },
{ 0x1F5A,     -1, "Speed position right"                                                         },
{ 0x1F5B,     -1, "Inversion flag:\n"                                                            \
                  "$00 = normal\n"                                                               \
                  "$FF = inverted"                                                               },
{ 0x1F5C,     -1, "Left half: Blink counter in combination with $1F55 -> $D0 = 1 Minute\n"       \
                  "Right half: GAME VERSION COUNTER"                                             },
{ 0x1F5D,     -1, "Back wheel offset (right + left half):\n"                                     \
                  "? = 0\n"                                                                      \
                  "? = 1\n"                                                                      \
                  "off = 2"                                                                      },
{ 0x1F5E,     -1, "Duplicate height (left + right half):\n"                                      \
                  "0 = normal\n"                                                                 \
                  "1 = writing"                                                                  },
{ 0x1F5F,     -1, "Round counter (left + right half)"                                            },
{ 0x1F60,     -1, "Start traffic lights colours"                                                 },
{ 0x1F61,     -1, "Marker delay left"                                                            },
{ 0x1F62,     -1, "Marker delay right"                                                           },
{ 0x1F63,     -1, "Dragster ? left"                                                              },
{ 0x1F64,     -1, "Dragster ? right"                                                             },
{ 0x1F65,     -1, "Dragster delay (right and left halves)"                                       }, // 587 (+ FIRSTGAMEDATACOMMENT)
// PIPLA Example (PIPBUG2) (588..590 + FIRSTGAMEDATACOMMENT)
{  0xC22,     -1, "Reserved for space code $20"                                                  }, // 588 (+ FIRSTGAMEDATACOMMENT)
{  0xC23,     -1, "Reserved for CR code $0D"                                                     },
{  0xC24,     -1, "Reserved for text length + 2 in hex"                                          }, // 590 (+ FIRSTGAMEDATACOMMENT)
// Theme Music (SI50) (591..592 + FIRSTGAMEDATACOMMENT)
{   0x32,     -1, "LED word byte"                                                                }, // 591 (+ FIRSTGAMEDATACOMMENT)
{  0x100,  0x13F, "32 notes in theme"                                                            }, // 592 (+ FIRSTGAMEDATACOMMENT)
// Binary Floating Point Routines (PIPBUG) (593..609 + FIRSTGAMEDATACOMMENT)
{  0x440,  0x445, "Program area"                                                                 }, // 593 (+ FIRSTGAMEDATACOMMENT)
{  0x440,  0x441, "Indirect address of operand 1"                                                },
{  0x442,  0x443, "Indirect address of operand 2"                                                },
{  0x444,  0x445, "Indirect address of result"                                                   },
{  0x50D,  0x50F, "Illegal value of result"                                                      },
{  0x720,  0x720, "Operation char; +,-,*,:"                                                      }, // yes, they use a colon as a solidus
{  0x780,  0x790, "Scratchpad area"                                                              },
{  0x780,     -1, "Rounding constant"                                                            },
{  0x781,  0x782, "Indirect address"                                                             },
{  0x783,     -1, "Flag"                                                                         },
{  0x784,  0x787, "Operand 1 scratchpad area"                                                    },
{  0x788,     -1, "Sign flag"                                                                    },
{  0x789,  0x790, "Operand 2 scratchpad area"                                                    },
{  0x7C0,  0x7CB, "Operands area"                                                                },
{  0x7C0,  0x7C3, "Operand 1"                                                                    },
{  0x7C4,  0x7C7, "Operand 2"                                                                    },
{  0x7C8,  0x7CB, "Result"                                                                       }, // 609 (+ FIRSTGAMEDATACOMMENT)
// 2650 Micro BASIC (PIPBUG) (610..617 + FIRSTGAMEDATACOMMENT)
{  0x5A9,  0x5B0, "* / table"                                                                    }, // 610 (+ FIRSTGAMEDATACOMMENT)
{  0x5A9,     -2, "Multiplier"                                                                   },
{  0x5AB,     -2, "Multiplicand"                                                                 },
{  0x67D,     -2, "1"                                                                            },
{  0x67F,     -2, "10"                                                                           },
{  0x681,     -2, "100"                                                                          },
{  0x683,     -2, "1000"                                                                         },
{  0x685,     -2, "10000"                                                                        }, // 617 (+ FIRSTGAMEDATACOMMENT)
// Prometheus (PIPBUG) (618..678 + FIRSTGAMEDATACOMMENT)
{ 0x24B2,     -2, "Assembler directives"                                                         }, // 618 (+ FIRSTGAMEDATACOMMENT)
{ 0x24B4,     -2, "Diverse 1 byte instructies"                                                   },
{ 0x24B6,     -2, "1 byte register instr (RRL,r1)"                                               },
{ 0x24B8,     -2, "Immediate addressing"                                                         },
{ 0x24BA,     -2, "Relatieve adressering"                                                        },
{ 0x24BC,     -2, "Absolute adressering load"                                                    },
{ 0x24BE,     -2, "Absolute adressering branch"                                                  },
{ 0x24C0,     -2, "Zero instructies"                                                             },
{ 0x24C2,     -2, "Prog Stat Word instr (2 bytes)"                                               },
{ 0x24C4,     -2, "ZBRR en ZBSR instructies"                                                     },
{ 0x24C6,     -2, "BXA en BSXA instructies"                                                      },
{ 0x2BC9,     -2, "LF, CR"                                                                       },
{ 0x3199, 0x31A0, "10, 100, 1000..."                                                             },
{ 0x379C, 0x37A0, "R0 EQU 0"                                                                     },
{ 0x37A1, 0x37A5, "R1 EQU 1"                                                                     },
{ 0x37A6, 0x37AA, "R2 EQU 2"                                                                     },
{ 0x37AB, 0x37AF, "R3 EQU 3"                                                                     },
{ 0x37B0, 0x37B4, "WC EQU H'08'"                                                                 },
{ 0x37B5, 0x37B9, "RS EQU H'10'"                                                                 },
{ 0x37BA, 0x37BE, "COM EQU H'02'"                                                                },
{ 0x37BF, 0x37C3, "CAR EQU H'01'"                                                                },
{ 0x37C4, 0x37C8, "SENS EQU H'80'"                                                               },
{ 0x37C9, 0x37CD, "FLAG EQU H'40'"                                                               },
{ 0x37CE, 0x37D2, "II EQU H'20'"                                                                 },
{ 0x37D3, 0x37D7, "IDC EQU H'20'"                                                                },
{ 0x37D8, 0x37DC, "OVF EQU H'04'"                                                                },
{ 0x37DD, 0x37E1, "Z EQU 0"                                                                      },
{ 0x37E2, 0x37E6, "P EQU 1"                                                                      },
{ 0x37E7, 0x37EB, "N EQU 2"                                                                      },
{ 0x37EC, 0x37F0, "EQ EQU 0"                                                                     },
{ 0x37F1, 0x37F5, "GT EQU 1"                                                                     },
{ 0x37F6, 0x37FA, "LT EQU 2"                                                                     },
{ 0x37FB, 0x37FF, "UN EQU 3"                                                                     },
{ 0x3800, 0x3F21, "Label buffer"                                                                 },
{ 0x3F23,     -1, "Print flag"                                                                   },
{ 0x3F25, 0x3F28, "Error buffer"                                                                 },
{ 0x3F2A, 0x3F2D, "Niet compressed label"                                                        },
{ 0x3F2E, 0x3F30, "Compressed label"                                                             },
{ 0x3F33,     -1, "Aantal assembly errors (bin)"                                                 },
{ 0x3F34,     -2, "Start adres object code"                                                      },
{ 0x3F36,     -1, "Page count"                                                                   },
{ 0x3F3D,     -2, "Max aantal labels"                                                            },
{ 0x3F3F,     -2, "Last label adres"                                                             },
{ 0x3F45, 0x3F6C, "Title buffer"                                                                 },
{ 0x3F74,     -1, "Nr of lines per pag (counting down)"                                          },
{ 0x3F7A,     -1, "Aantal obj bytes of een regel"                                                },
{ 0x3F7B, 0x3FC2, "Source code buffer"                                                           },
{ 0x3FC3,     -1, "Aantal char in BUF5"                                                          },
{ 0x3FC6,     -1, "Aantal bytes in code"                                                         },
{ 0x3FC7,     -1, "Byte 1"                                                                       },
{ 0x3FC8,     -1, "Byte 2"                                                                       },
{ 0x3FC9,     -1, "Byte 3"                                                                       },
{ 0x3FCA, 0x3FD6, "Object code buffer"                                                           },
{ 0x3FD7,     -2, "Relatief adres?"                                                              },
{ 0x3FD9,     -1, "= $FF als - voorkomt"                                                         },
{ 0x3FDB,     -1, "Lengte string"                                                                },
{ 0x3FDC,     -1, "Source pointer"                                                               },
{ 0x3FDE,     -1, "= $01 als * voorkomt"                                                         },
{ 0x3FDF,     -1, "= $FF bij < , = $00 bij > , anders = $01"                                     },
{ 0x3FE0,     -1, "String control: 0=bin, 1=hex, 2=oct, 3=de..."                                 },
{ 0x3FE1,     -1, "MSB decimaal byte"                                                            }, // 678 (+ FIRSTGAMEDATACOMMENT)
};
