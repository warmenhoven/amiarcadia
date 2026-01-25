MODULE const int tr_pos[] =
{ 9, //   $20
  1, // ! $21
  2, // " $22
  3, // # $23
  4, // $ $24
  4, // % $25
  5, // & $26
  8, // ' $27
  7, // ( $28
  8, // ) $29
  6, // * $2A
  8, // + $2B
  6, // , $2C
  5, // - $2D
  7, // . $2E
  8, // / $2F
  8, // 0 $30
  1, // 1 $31
  2, // 2 $32
  3, // 3 $33
  4, // 4 $34
  4, // 5 $35
  5, // 6 $36
  5, // 7 $37
  6, // 8 $38
  7, // 9 $39
  8, // : $3A
  8, // ; $3B
  0, //   $3C
  8, // = $3D
  0, //   $3E
  8, // ? $3F
  2, // @ $40
  1, // A $41
  4, // B $42
  3, // C $43
  3, // D $44
  3, // E $45
  4, // F $46
  4, // G $47
  5, // H $48
  6, // I $49
  5, // J $4A
  6, // K $4B
  7, // L $4C
  5, // M $4D
  5, // N $4E
  7, // O $4F
  8, // P $50
  1, // Q $51
  4, // R $52
  2, // S $53
  4, // T $54
  5, // U $55
  4, // V $56
  2, // W $57
  2, // X $58
  5, // Y $59
  1, // Z $5A
  0, //   $5B
  0, //   $5C
  0, //   $5D
  0, //   $5E
  0, //   $5F
  0, //   $60
  1, // a $61
  4, // b $62
  3, // c $63
  3, // d $64
  3, // e $65
  4, // f $66
  4, // g $67
  5, // h $68
  6, // i $69
  5, // j $6A
  6, // k $6B
  7, // l $6C
  5, // m $6D
  5, // n $6E
  7, // o $6F
  8, // p $70
  1, // q $71
  4, // r $72
  2, // s $73
  4, // t $74
  5, // u $75
  4, // v $76
  2, // w $77
  2, // x $78
  5, // y $79
  1, // z $7A
};

EXPORT const UBYTE tr_chars[91][7] = { {
0x00, // .....   $20
0x00, // .....
0x00, // .....
0x00, // .....
0x00, // .....
0x00, // .....
0x00, // .....
}, {
0x20, // ..#.. ! $21
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
0x00, // .....
0x00, // .....
0x20, // ..#..
}, {
0x50, // .#.#. " $22
0x50, // .#.#.
0x50, // .#.#.
0x00, // .....
0x00, // .....
0x00, // .....
0x00, // .....
}, {
0x50, // .#.#. # $23
0x50, // .#.#.
0xF8, // #####
0x50, // .#.#.
0xF8, // #####
0x50, // .#.#.
0x50, // .#.#.
}, {
0x20, // ..#.. $ $24
0x78, // .####
0xA0, // #.#..
0x70, // .###.
0x28, // ..#.#
0xF0, // ####.
0x20, // ..#..
}, {
0xC0, // ##... % $25
0xC8, // ##..#
0x10, // ...#.
0x20, // ..#..
0x40, // .#...
0x98, // #..##
0x18, // ...##
}, {
0x60, // .##.. & $26
0x90, // #..#.
0xA0, // #.#..
0x40, // .#...
0xA8, // #.#.#
0x90, // #..#.
0x68, // .##.#
}, {
0x60, // .##.. ' $27
0x20, // ..#..
0x40, // .#...
0x00, // .....
0x00, // .....
0x00, // .....
0x00, // .....
}, {
0x10, // ...#. ( $28
0x20, // ..#..
0x40, // .#...
0x40, // .#...
0x40, // .#...
0x20, // ..#..
0x10, // ...#.
}, {
0x40, // .#... ) $29
0x20, // ..#..
0x10, // ...#.
0x10, // ...#.
0x10, // ...#.
0x20, // ..#..
0x40, // .#...
}, {
0x00, // ..... * $2A
0x20, // ..#..
0xA8, // #.#.#
0x70, // .###.
0xA8, // #.#.#
0x20, // ..#..
0x00, // .....
}, {
0x00, // ..... + $2B
0x20, // ..#..
0x20, // ..#..
0xF8, // #####
0x20, // ..#..
0x20, // ..#..
0x00, // .....
}, {
0x00, // ..... , $2C
0x00, // .....
0x00, // .....
0x00, // .....
0x60, // .##..
0x20, // ..#..
0x40, // .#...
}, {
0x00, // ..... - $2D
0x00, // .....
0x00, // .....
0xF8, // #####
0x00, // .....
0x00, // .....
0x00, // .....
}, {
0x00, // ..... . $2E
0x00, // .....
0x00, // .....
0x00, // .....
0x00, // .....
0x60, // .##..
0x60, // .##..
}, {
0x00, // ..... / $2F
0x08, // ....#
0x10, // ...#.
0x20, // ..#..
0x40, // .#...
0x80, // #.....
0x00, // .....
}, {
0x70, // .###. 0 $30
0x88, // #...#
0x98, // #..##
0xA8, // #.#.#
0xC8, // ##..#
0x88, // #...#
0x70, // .###.
}, {
0x20, // ..#.. 1 $31
0x60, // .##..
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
0x70, // .###.
}, {
0x70, // .###. 2 $32
0x88, // #...#
0x08, // ....#
0x10, // ...#.
0x20, // ..#..
0x40, // .#...
0xF8, // #####
}, {
0xF8, // ##### 3 $33
0x10, // ...#.
0x20, // ..#..
0x10, // ...#.
0x08, // ....#
0x88, // #...#
0x70, // .###.
}, {
0x10, // ...#. 4 $34
0x30, // ..##.
0x50, // .#.#.
0x90, // #..#.
0xF8, // #####
0x10, // ...#.
0x10, // ...#.
}, {
0xF8, // ##### 5 $35
0x80, // #....
0xF0, // ####.
0x08, // ....#
0x08, // ....#
0x88, // #...#
0x70, // .###.
}, {
0x30, // ..##. 6 $36
0x40, // .#...
0x80, // #....
0xF0, // ####.
0x88, // #...#
0x88, // #...#
0x70, // .###.
}, {
0xF8, // ##### 7 $37
0x08, // ....#
0x10, // ...#.
0x20, // ..#..
0x40, // .#...
0x40, // .#...
0x40, // .#...
}, {
0x70, // .###. 8 $38
0x88, // #...#
0x88, // #...#
0x70, // .###.
0x88, // #...#
0x88, // #...#
0x70, // .###.
}, {
0x70, // .###. 9 $39
0x88, // #...#
0x88, // #...#
0x78, // .####
0x08, // ....#
0x10, // ...#.
0x60, // .##..
}, {
0x00, // ..... : $3A
0x60, // .##..
0x60, // .##..
0x00, // .....
0x60, // .##..
0x60, // .##..
0x00, // .....
}, {
0x00, // ..... ; $3B
0x60, // .##..
0x60, // .##..
0x00, // .....
0x60, // .##..
0x20, // ..#..
0x40, // .#...
}, {
0xF8, // ##### < $3C
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
}, {
0x00, // ..... = $3D
0x00, // .....
0xF8, // #####
0x00, // .....
0xF8, // #####
0x00, // .....
0x00, // .....
}, {
0xF8, // ##### > $3E
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
}, {
0x70, // .###. ? $3F
0x88, // #...#
0x08, // ....#
0x10, // ...#.
0x20, // ..#..
0x00, // .....
0x20, // ..#..
}, {
0x70, // .###. @ $40
0x88, // #...#
0xB8, // #.###
0xA8, // #.#.#
0xB8, // #.###
0x80, // #....
0x70, // .###.
}, {
0x70, // .###. A $41
0x88, // #...#
0x88, // #...#
0x88, // #...#
0xF8, // #####
0x88, // #...#
0x88, // #...#
}, {
0xF0, // ####. B $42
0x88, // #...#
0x88, // #...#
0xF0, // ####.
0x88, // #...#
0x88, // #...#
0xF0, // ####.
}, {
0x70, // .###. C $43
0x88, // #...#
0x80, // #....
0x80, // #....
0x80, // #....
0x88, // #...#
0x70, // .###.
}, {
0xE0, // ###.. D 3 $44
0x90, // #..#.
0x88, // #...#
0x88, // #...#
0x88, // #...#
0x90, // #..#.
0xE0, // ###..
}, {
0xF8, // ##### E $45
0x80, // #....
0x80, // #....
0xF0, // ####.
0x80, // #....
0x80, // #....
0xF8, // #####
}, {
0xF8, // ##### F $46
0x80, // #....
0x80, // #....
0xF0, // ####.
0x80, // #....
0x80, // #....
0x80, // #....
}, {
0x70, // .###. G $47
0x88, // #...#
0x80, // #....
0xB8, // #.###
0x88, // #...#
0x88, // #...#
0x78, // .####
}, {
0x88, // #...# H $48
0x88, // #...#
0x88, // #...#
0xF8, // #####
0x88, // #...#
0x88, // #...#
0x88, // #...#
}, {
0x70, // .###. I $49
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
0x70, // .###.
}, {
0x38, // ..### J $4A
0x10, // ...#.
0x10, // ...#.
0x10, // ...#.
0x10, // ...#.
0x90, // #..#.
0x60, // .##..
}, {
0x88, // #...# K $4B
0x90, // #..#.
0xA0, // #.#..
0xC0, // ##...
0xA0, // #.#..
0x90, // #..#.
0x88, // #...#
}, {
0x80, // #.... L $4C
0x80, // #....
0x80, // #....
0x80, // #....
0x80, // #....
0x80, // #....
0xF8, // #####
}, {
0x88, // #...# M $4D
0xD8, // ##.##
0xA8, // #.#.#
0xA8, // #.#.#
0x88, // #...#
0x88, // #...#
0x88, // #...#
}, {
0x88, // #...# N $4E
0x88, // #...#
0xC8, // ##..#
0xA8, // #.#.#
0x98, // #..##
0x88, // #...#
0x88, // #...#
}, {
0x70, // .###. O $4F
0x88, // #...#
0x88, // #...#
0x88, // #...#
0x88, // #...#
0x88, // #...#
0x70, // .###.
}, {
0xF0, // ####. P $50
0x88, // #...#
0x88, // #...#
0xF0, // ####.
0x80, // #....
0x80, // #....
0x80, // #....
}, {
0x70, // .###. Q $51
0x88, // #...#
0x88, // #...#
0x88, // #...#
0xA8, // #.#.#
0x90, // #..#.
0x68, // .##.#
}, {
0xF0, // ####. R $52
0x88, // #...#
0x88, // #...#
0xF0, // ####.
0xA0, // #.#..
0x90, // #..#.
0x88, // #...#
}, {
0x70, // .###. S $53
0x88, // #...#
0x80, // #....
0x70, // .###.
0x08, // ....#
0x88, // #...#
0x70, // .###.
}, {
0xF8, // ##### T $54
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
}, {
0x88, // #...# U $55
0x88, // #...#
0x88, // #...#
0x88, // #...#
0x88, // #...#
0x88, // #...#
0x70, // .###.
}, {
0x88, // #...# V $56
0x88, // #...#
0x88, // #...#
0x88, // #...#
0x88, // #...#
0x50, // .#.#.
0x20, // ..#..
}, {
0x88, // #...# W $57
0x88, // #...#
0x88, // #...#
0xA8, // #.#.#
0xA8, // #.#.#
0xA8, // #.#.#
0x50, // .#.#.
}, {
0x88, // #...# X $58
0x88, // #...#
0x50, // .#.#.
0x20, // ..#..
0x50, // .#.#.
0x88, // #...#
0x88, // #...#
}, {
0x88, // #...# Y $59
0x88, // #...#
0x88, // #...#
0x50, // .#.#.
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
}, {
0xF8, // ##### Z $5A
0x08, // ....#
0x10, // ...#.
0x20, // ..#..
0x40, // .#...
0x80, // #....
0xF8, // #####
}, {
0xF8, // ##### [ $5B
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
}, {
0xF8, // ##### \ $5C
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
}, {
0xF8, // ##### ] $5D
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
}, {
0xF8, // ##### ^ $5E
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
}, {
0xF8, // ##### _ $5F
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
}, {
0xF8, // ..... ` $60
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
0xF8, // #####
}, {
0x00, // ..... a $61
0x00, // .....
0x70, // .###.
0x08, // ....#
0x78, // .####
0x88, // #...#
0x78, // .####
}, {
0x80, // #.... b $62
0x80, // #....
0xF0, // ####.
0x88, // #...#
0x88, // #...#
0x88, // #...#
0xF0, // ####.
}, {
0x00, // ..... c $63
0x00, // .....
0x78, // .####
0x80, // #....
0x80, // #....
0x80, // #....
0x78, // .####
}, {
0x08, // ....# d $64
0x08, // ....#
0x78, // .####
0x88, // #...#
0x88, // #...#
0x88, // #...#
0x78, // .####
}, {
0x00, // ..... e $65
0x00, // .....
0x70, // .###.
0x88, // #...#
0xF8, // #####
0x80, // #....
0x70, // .###.
}, {
0x18, // ...## f $66
0x20, // ..#..
0xF8, // #####
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
}, {
0x00, // ..... g $67
0x00, // .....
0x78, // .####
0x88, // #...#
0x78, // .####
0x08, // ....#
0x70, // .###.
}, {
0x80, // #.... h $68
0x80, // #....
0xB0, // #.##.
0xC8, // ##..#
0x88, // #...#
0x88, // #...#
0x88, // #...#
}, {
0x20, // ..#.. i $69
0x00, // .....
0x60, // .##..
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
0x70, // .###.
}, {
0x10, // ...#. j $6A
0x00, // .....
0x30, // ..##.
0x10, // ...#.
0x10, // ...#.
0x90, // #..#.
0x60, // .##..
}, {
0x80, // #.... k $6B
0x80, // #....
0x90, // #..#.
0xA0, // #.#..
0xC0, // ##...
0xA0, // #.#..
0x90, // #..#.
}, {
0x60, // .##.. l $6C
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
0x20, // ..#..
0x70, // .###.
}, {
0x00, // ..... m $6D
0x00, // .....
0xF0, // ####.
0xA8, // #.#.#
0xA8, // #.#.#
0xA8, // #.#.#
0xA8, // #.#.#
}, {
0x00, // ..... n $6E
0x00, // .....
0xB0, // #.##.
0xC8, // ##..#
0x88, // #...#
0x88, // #...#
0x88, // #...#
}, {
0x00, // ..... o $6F
0x00, // .....
0x70, // .###.
0x88, // #...#
0x88, // #...#
0x88, // #...#
0x70, // .###.
}, {
0x00, // ..... p $70
0x00, // .....
0xF0, // ####.
0x88, // #...#
0xF0, // ####.
0x80, // #....
0x80, // #....
}, {
0x00, // ..... q $71
0x00, // .....
0x78, // .####
0x88, // #...#
0x78, // .####
0x08, // ....#
0x08, // ....#
}, {
0x00, // ..... r $72
0x00, // .....
0xB0, // #.##.
0xC0, // ##...
0x80, // #....
0x80, // #....
0x80, // #....
}, {
0x00, // ..... s $73
0x00, // .....
0x78, // .####
0x80, // #....
0x70, // .###.
0x08, // ....#
0xF0, // ####.
}, {
0x20, // ..#.. t $74
0x20, // ..#..
0xF8, // #####
0x20, // ..#...
0x20, // ..#..
0x20, // ..#..
0x18, // ...##
}, {
0x00, // ..... u $75
0x00, // .....
0x88, // #...#
0x88, // #...#
0x88, // #...#
0x98, // #..##
0x68, // .##.#
}, {
0x00, // ..... v $76
0x00, // .....
0x88, // #...#
0x88, // #...#
0x88, // #...#
0x50, // .#.#.
0x20, // ..#..
}, {
0x00, // ..... w $77
0x00, // .....
0x88, // #...#
0x88, // #...#
0xA8, // #.#.#
0xA8, // #.#.#
0x50, // .#.#.
}, {
0x00, // ..... x $78
0x00, // .....
0x88, // #...#
0x50, // .#.#.
0x20, // ..#..
0x50, // .#.#.
0x88, // #...#
}, {
0x00, // ..... y $79
0x00, // .....
0x88, // #...#
0x88, // #...#
0x78, // .####
0x08, // ....#
0x70, // .###.
}, {
0x00, // ..... z $7A
0x00, // .....
0xF8, // #####
0x10, // ...#.
0x20, // ..#..
0x40, // .#...
0xF8, // #####
} };

EXPORT const STRPTR classes[4][4] = {
{ // 6.1
"a lad had a gal\n" \
"a gal had a dad\n" \
"a lad asks a gal's dad\n" \
"a sad lad laffs a lass laff\n" \
"alas; all lads shall fall\n" \
"dads all add; as lads ask dads\n" \
"gals shall dash as lads ask\n" \
"a glass lass has a sad salad\n" \
"gals shall lag as fads jag\n" \
"lads dads has glad gals\n",
// 6.2
"sploosh was the spleesh of the splash\n" \
"gloosh was the gleesh of the glass\n" \
"the great splooshes and splashes\n" \
"wet all our flash sashes\n" \
"while the sploosher just splooshed its way past\n" \
"i shall walk tall the short guy said\n" \
"through great grey surf or hot shot lead\n" \
"will i trip or talk out loud\n" \
"or wear that purple durple shroud\n" \
"or will i just type out this dull stuff\n",
// 6.3
"we are now allowed to use all the keys\n" \
"we can use any old letter we want\n" \
"that is why it is much easier now\n" \
"before it was difficult to think of words\n" \
"because we could not use many letters\n" \
"but now we can and everything is much easier\n" \
"so coming up with ten lines of words\n" \
"can now be done in no time at all\n" \
"even if everything is kept very simple\n" \
"is it not nice to know that someone is happy\n",
// 6.4
"Seventy-five % of US$3999 isn't quite $3000.\n" \
"1220 IF X=Y+789 THEN GOTO 550\n" \
"12 cartons (1 doz/ct) cost @ $35.00 FOB\n" \
"Ain't really ain't in the dictionary\n" \
"B&W film isn't able to give color prints\n" \
"10-9-8-7-6-5-4-3-2-1-0, Blast off!\n" \
"7/8 of 1% of the original 55% of 1/2 the sample\n" \
"HYDROGEN is # 1 on the charts.\n" \
"I said \"A WOP BOP, A LOO BOP, A WHAM, BAM, BOOM!\"\n" \
"if 11111000000 IS 1984 in binary, what is 1985?\n",
}, {
// 7.1
"The red fox is quick.\n" \
"The brown dog is lazy.\n" \
"The fox jumps over the dog.\n" \
"The red fox jumps over the lazy brown dog.\n",
// 7.2
"If 4 chickens cost $100,\n" \
"then a chicken costs $25 @.\n" \
"He said \"Everything I said was a lie\",\n" \
"so was this statement the truth or not?\n" \
"Once you can type well, you can type anything\n" \
"from a letter to a computer program.\n" \
"3/4 of the quick red fox jumped over about\n" \
"7/8 of the lazy brown dog.\n",
// 7.3
"3/8 of 1/2 is 3/4 of the whole things, isn't it?\n" \
"If you use B&W film, it's difficult to get color photographs from\n" \
"your D&P shop.\n" \
"'Made in Mississippi by Mississippi maid.' is a sentence which\n" \
"uses alliteration as a mnemonic device.\n" \
"E=MC squared is a formula which describes the product of M (the\n" \
"mass of an object) multiplied by C (the speed of light) squared and\n" \
"expressed as E (the energy given off).\n" \
"The longer these sentences are, and the more difficult they are to\n" \
"type, the better they are for you because, instead of utilizing\n" \
"only 1/2 of your typing talent, they force you to put out 100% effort\n" \
"and thereby improve yourself immensely.\n",
// 7.4
"The following sentences are all connected in content to give you an\n" \
"idea of what it is like to type out letters and articles which are more\n" \
"than simple unconnected thoughts and words.\n" \
"This second sentence would really follow the first and be tucked\n" \
"right up to the end of it if this was a normal paragraph, but it isn't\n" \
"... so it isn't, but instead is put out on it's own like someone who\n" \
"has no friends.\n" \
"Nevertheless, it all kind of makes sense in one way or another when\n" \
"you really sit down and think about it, if you have the time, but\n" \
"many people don't and so they just keep on typing and turning\n" \
"pages and never truly understand how wonderful these sentences in\n" \
"Level 4 are.\n" \
"You do understand, though, I can tell how incredibly sensitive and\n" \
"intelligent you are just from feeling the way you read this page,\n" \
"so I know for a fact that you will appreciate the way that Level 4\n" \
"has been carefully constructed for your edification.\n" \
"This is the last sentence in Level 4 and let me take this opportunity\n" \
"to tell you how much we've enjoyed having you in this level even\n" \
"though it seems you couldn't stay very long, but you have done very\n" \
"well in your short stay and now are set to move on ... so good\n" \
"luck ... and possibly some day when you've become a master typist\n" \
"you might just want to come back to Level 4 for a little visit.\n",
}, {
// 8.1
"Now you've graduated up to the point where you can do whole\n" \
"paragraphs at once. Congratulations! Once you get through this\n" \
"section, you will likely be able to type virtually anything that comes\n" \
"your way smoothly and confidently. Nothing will slow you down.\n" \
"You'll be able to type like a machine gun and mow down any kind\n" \
"of typing work that you come across. Of course, you have to finish this\n" \
"section first. That's the hard part. But even as you type, you can be\n" \
"thinking about that time ... not too far in the future ... when you\n" \
"will be SUPER TYPIST!\n",
// 8.2
"The trick to typing long passages is simply to let your eyes go over\n" \
"the material and as you see a letter, your fingers type it. Simple as\n" \
"that. Once you have your fingers trained to hit the right keys, all you\n" \
"need to do is think the letter, and presto ... it's typed. So when\n" \
"you get to be a great typist, instead of just a trainee, you can sail\n" \
"along and even type words like \"oogle\" that don't make sense or that\n" \
"come from another language without even a pause, because you don't\n" \
"really read it anymore. All you have to do by that time is look at it\n" \
"and it gets typed.\n",
// 8.3
"To get you fully trained and ready for the real world of typing where\n" \
"a lot of things don't make sense, this paragraph will be mostly\n" \
"nonsense. Like: ugga mugga shugga wugga, hugga tugga shugga\n" \
"bugga. Noogles and boogles guggle blogg ruckamuck wahoo.\n" \
"As you can see, you still depend heavily on reading the material.\n" \
"You'll find that it was much harder to type nonsense than to type\n" \
"things you can understand. Which means, of course, that you are not\n" \
"ready yet, but must type reams and reams of more nonsense. Since\n" \
"this book is only short and contains limited exercises, we recommend\n" \
"that you get your nonsense examples from your local newspaper.\n" \
"You will find it is full of it.\n",
// 8.4
"Mugwomp, glugga chug chug. Noogies? Whap whap whap. Ricky-\n" \
"ticky falonga. Boofie-woofie tonga tonga. And if you think that is\n" \
"difficult, just imagine typing up chemical formulae and tough\n" \
"scientific data that means absolutely nothing to man nor beast. You\n" \
"have to be nuts to want to type. Unless you want to do interesting\n" \
"things like computer programming. You have to type to do that.\n" \
"Or to be an astronaut. Or a writer of clever books like this. Or a lion\n" \
"tamer. (Well, maybe not a lion tamer.) But there are a lot of careers\n" \
"where you have to type. So, perhaps, all people who want to learn to\n" \
"type are not nuts. (They only appear to be)\n",
}, {
// 9.1
"What's typing got to do\n" \
"with the real world, anyway?\n" \
"\n" \
"These days, only wimps and nerds don't know how to type. All the\n" \
"good guys learn typing. They have to. Otherwise all their friends\n" \
"will laugh at them and put tomatoes down the backs of their shirts.\n" \
"\n" \
"For example, dashing, romantic, rich computer programmers all have\n" \
"to know how to type. So do rough, tough cops. And airline pilots.\n" \
"And firemen. And film directors. And novelists. And reporters. And\n" \
"astronauts. And engineers. And advertising guys. And forest rangers.\n" \
"And business people. And scientists. And everybody that's important\n" \
"or neat. (Well, nearly everybody, anyway)\n" \
"\n" \
"So you have two choices. You can close this book now, and be a\n" \
"miserable, mewling, disgusting wimp for the rest of your life. Or\n" \
"you can go through these easy lessons, learn to type, and become\n" \
"attractive, rich, successful, happy and fulfilled.\n" \
"\n" \
"The choice is yours.\n",
// 9.2
"THINGS YOU NEED TO KNOW\n" \
"\n" \
"SHIFT the SHIFT key is the one that lets you shift between capital\n" \
"letters and small letters. If you press the SHIFT key plus the \"A\"\n" \
"key, you will type a capital \"A\". If you type the \"A\" key without\n" \
"pressing the SHIFT key, you will type a small letter \"a\".\n" \
"\n" \
"For keys other than letters, the character which will be typed if you\n" \
"press the SHIFT key is clearly marked on top. For example the\n" \
"number 1 key shows a \"!\" on top of the 1. This means that if you\n" \
"press the 1 key and the SHIFT key together, you will type a \"!\".\n" \
"If you don't press the SHIFT key, you will type a \"1\".\n" \
"\n" \
"LOCK This key is related to the SHIFT key. If you press the LOCK\n" \
"key, it will lock all your typing into the SHIFT mode. SO ALL YOUR\n" \
"LETTERS WILL BE CAPITAL LETTERS AND INSTEAD OF\n" \
"TYPING NUMBERS, YOU WILL TYPE !@#$%-&*().\n" \
"EVERYTHING WILL CONTINUE THIS WAY UNTIL YOU PRESS\n" \
"THE SHIFT KEY. Then everything will be back to normal.\n",
// 9.3
"100 REM MARK SIX GENERATOR PROGRAM\n" \
"105 PRINT TAB( 3) : PRINT \"MARK SIX NUMBER\": PRINT\n" \
"120 DIM T(36)\n" \
"135 REM SET UP ONE DIMENSION ARRAY FLAG\n" \
"140 FOR I = 1 TO 36\n" \
"150 T(I) = 0\n" \
"160 NEXT I\n" \
"170 REM RANDOM NUMBER GENERATOR\n" \
"175 FOR J = 1 TO 5\n" \
"180 FOR K = 1 TO 6\n" \
"190 A = INT (RND (1) * 36 + 1)\n" \
"195 REM TEST FOR REPETITION\n" \
"200 IF T(A) = 1 THEN 190\n" \
"210 T(A) = 1\n" \
"230 NEXT K\n" \
"240 REM PRINTOUT ONE SET\n" \
"242 FOR U = 1 TO 36\n" \
"244 IF T(U) = 1 THEN PRINT \" \"U;\n" \
"245 T(U) = 0\n" \
"246 NEXT U\n" \
"248 PRINT\n" \
"250 NEXT J\n" \
"255 PRINT : PRINT\n" \
"260 INPUT \"ANOTHER SET OF NUMBERS......\";N$\n" \
"265 PRINT : PRINT\n" \
"270 IF LEFT$ (N$,1) = \"Y\" THEN RUN\n",
// 9.4
"January 15, 19AA\n" \
"\n" \
"Mr. Fred Glomph\n" \
"Sales Manager\n" \
"Fly-By-Night Gizmo Co.\n" \
"25 Seedy Street\n" \
"Caveat Emtorville\n" \
"\n" \
"Dear Mr. Glomph,\n" \
"\n" \
"Your \"EVERLAST\" dining set lasted only 3 days. It disintegrated.\n" \
"Not only that, but the remains of it dissolved my table, fell on the\n" \
"floor and caused extensive discoloration. And Aunt Maude, while\n" \
"cleaning it up, got some on her hands and immediately fell into a fit.\n" \
"While she was thrashing around the floor, she kicked our wee son\n" \
"Mortimer in the eye and he, too, is in the hospital. In our rush to take\n" \
"these two casualties to the hospital, my wife left a cigarette burning\n" \
"in the living room and, upon our return, we discovered that our house\n" \
"had burned down. Seeing this and realizing that our insurance had run\n" \
"out, my poor wife, distraught, wandered into the road and she was\n" \
"struck down by a passing bus ... number 49, if I recall.\n" \
"\n" \
"Legal consultation has brought to light the fact that you have covered\n" \
"yourself totally and will assume no responsibility whatsoever for my\n" \
"predicament. Therefore I am sure you will understand fully when I\n" \
"tell you that the paper in this letter has been saturated with Anthrax\n" \
"46, a fast-acting nerve toxin which has no known antidote. Just\n" \
"touching the paper is enough to give you a lethal dose.\n" \
"\n" \
"Before the convulsions begin, I hope you will authorize the replacement\n" \
"\"EVERLAST\" dining set to which I am entitled under your warranty.\n" \
"After all, there is no sense in bearing a grudge.\n" \
"\n" \
"Sincerely,\n" \
"\n" \
"Wadsworth Wort\n",
} };
