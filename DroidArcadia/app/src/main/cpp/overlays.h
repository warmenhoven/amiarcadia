EXPORT const STRPTR overlays[OVERLAYS][33] = {
{ // 0. default (Arcadia)
        "2/Fi",                 "2/Fi",
"x1",   "x2",   "x3",   "x1",   "x2",   "x3",
"1",    "x4",   "3",    "1",    "x4",   "3",
"4",    "5",    "6",    "4",    "5",    "6",
"7",    "8",    "9",    "7",    "8",    "9",
"Clear","0",    "Enter","Clear","0",    "Enter",
""
},
{ // 1. default (Elektor)
        "WCAS",                 "E",
"-----","-----","-----","-----","-----","-----",
"RCAS", "-----","C",    "D",    "-----","F",
"BP",   "REG",  "8",    "9",    "A",    "B",
"PC",   "MEM",  "4",    "5",    "6",    "7",
"-",    "+",    "0",    "1",    "2",    "3",
""
},
{ // 2. default (Instructor)
        "BKPT",                 "E",
"-----","-----","-----","-----","-----","-----",
"WCAS", "-----","C",    "D",    "-----","F",
"RCAS", "REG",  "8",    "9",    "A",    "B",
"STEP", "MEM",  "4",    "5",    "6",    "7",
"RUN",  "NXT",  "0",    "1",    "2",    "3",
""
},
{ // 3. default (PIPBUG/CD2650)
      "n/a",            "n/a",
"n/a","n/a","n/a","n/a","n/a","n/a",
"n/a","n/a","n/a","n/a","n/a","n/a",
"n/a","n/a","n/a","n/a","n/a","n/a",
"n/a","n/a","n/a","n/a","n/a","n/a",
"n/a","n/a","n/a","n/a","n/a","n/a",
""
},
{ // 4. DRAUGHTS
        "Finis",                "Finis",
"-----","-----","-----","-----","-----","-----",
"Up+Lt","-----","Up+Rt","Up+Lt","-----","Up+Rt",
"Dn+Lt","Cancl","Dn+Rt","Dn+Lt","Cancl","Dn+Rt",
"C1",   "C2",   "C3",   "Cnfrm","Cnfrm","Cnfrm",
"N1",   "N2",   "N3",   "Next", "Next", "Next",
"SELECT (F2): choose game variant (1..15)\n" \
"START (F1): start game\n" \
"Normally (ie. variants 1..14):\n" \
"Cancl = Cancel move\n" \
"Finis = Finished move\n" \
"Up+Lt = move counter Up and Left\n" \
"Up+Rt = move counter Up and Right\n" \
"Dn+Lt = move counter Down and Left\n" \
"Dn+Rt = move counter Down and Right\n" \
"C1..C3/Cnfrm = Confirm computer's move\n" \
"N1..N3/Next = select Next counter\n" \
"In board editor (ie. variant 15):\n" \
"Up+Lt/Finis/Up+Rt = next\n" \
"C1 = red king\n" \
"C2/N2 = clear\n" \
"C3 = blue king\n" \
"N1 = red pawn\n" \
"N3 = blue pawn"
},
{ // 5. HEADON
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Up",   "-----","-----","Up",   "-----",
"Left", "-----","Right","Left", "-----","Right",
"-----","Down", "-----","-----","Down", "-----",
"SELECT (F2): select game variant:\n" \
"1..5 are 1 player. 6..9 are 2 players.\n" \
"1,3,5,7 have 1 computer car. 2,4,6,8 have 2. 9 has 0.\n" \
"1-2,5-6: dodge car(s). 3-4,7-8: hit car(s). 9: chase car of same colour as score.\n" \
"Left player is black or red. Right player is green or (game 9) blue. Computer is purple, yellow or (games 7..8) blue.\n" \
"START (F1): start game\n" \
"Up/Down/Left/Right: change lanes\n" \
"Paddle left/right: decrease/increase car speed while held"
},
{ // 6. MATH1/MATH2
        "8",                    "8",
"-----","-----","-----","-----","-----","-----",
"7",    "-----","9",    "7",    "-----","9",
"4",    "5",    "6",    "4",    "5",    "6",
"1",    "2",    "3",    "1",    "2",    "3",
"Clear","0",    "Enter","Clear","0",    "Enter",
"SELECT (F2): 1-player mode\n" \
"START (F1): 2-player mode\n" \
"Clear = cycle through difficulty levels\n" \
"0 = cycle through game options\n" \
"Enter = start game\n" \
"While playing:\n" \
"Enter = Enter (ie. finished)\n" \
"Clear = Clear (ie. redo)"
},
{ // 7. MUSICALG
        "Bb/Pu",                "Rs/Pu",
"-----","-----","-----","-----","-----","-----",
"A/Red","-----","B/Grn","A/Red","-----","Rs/Gr",
"F#/Cy","G/Yel","G#/Bl","F#/Cy","G/Yel","G#/Bl",
"Eb",   "E",    "F",    "Eb",   "E",    "F",
"C",    "C#",   "D",    "C",    "C#",   "D",
"SELECT (F2): cycle through game variants (1..8)\n" \
"START (F1): start game\n " \
"During playback, holding a note will transpose the pitch to the key one semitone higher than marked (eg. holding C will transpose to C#)\n" \
"During playback, holding the right player's left column buttons will adjust tempo as follows:\n" \
"C = slowest, Eb = slow, F#/Cy = fast, A/Red = fastest\n" \
"Pu/Grn/Cy/Yel/Bl = Purple/Green/Cyan/Yellow/Blue\n" \
"Rs = Rest"
},
{ // 8. Circledrive (patched)
        "Brake",                "-----",
"-----","-----","-----","-----","-----","-----",
"Start","-----","Accel","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"UC (F2): don't press this key!\n" \
"LC (F3): choose game speed (1..9)\n" \
"START (F1) or Start: start game\n" \
"Accel = Accelerate\n" \
"Paddle directions: change lanes\n" \
"Red squares: +2 points. Square turns blue.\n" \
"Blue squares: +1 point. Square stays blue, turns black or disappears.\n" \
"Black squares: -2 points.\n" \
"Left digits: game speed\n" \
"Right digits: score"
},
{ // 9. 3D Bowling
        "Bowl",                 "Bowl",
"-----","-----","-----","-----","-----","-----",
"Revie","-----","-----","Revie","-----","-----",
"-----","-----","-----","-----","-----","-----",
"HokLt","Durat","HokRt","HokLt","Durat","HokRt",
"BwlLt","-----","BwlRt","BwlLt","-----","BwlRt",
"A (F2): toggle between 1 and 2 players\n" \
"START (F1): start game\n" \
"HokLt = Hook Left\n" \
"HokRt = Hook Right\n" \
"Revie = Review score\n" \
"Durat = hook Duration\n" \
"BwlLt = move Bowler to his Left (up the screen)\n" \
"BwlRt = move Bowler to his Right (down the screen)"
},
{ // 10. Dictionary
        "Space",                "Clear",
"A",    "B",    "C",    "D",    "E",    "F",
"G",    "H",    "I",    "J",    "K",    "L",
"M",    "N",    "O",    "P",    "Q",    "R",
"S",    "T",    "U",    "V",    "W",    "X",
"Y",    "Z",    "Bkspc","Hold", "Phrse","Searc",
"Bkspc = Backspace\n" \
"Phrse = Phrase\n" \
"Searc = Search\n" \
"START (F1): translate"
},
{ // 11. American Football
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Kick", "-----","-----","Kick", "-----","-----",
"-----","-----","-----","-----","-----","-----",
"Pass", "-----","-----","Pass", "-----","-----",
"Scrum","-----","Goal", "Scrum","-----","Goal",
"START (F1): start game\n" \
"Goal = kick for Goal\n" \
"Kick = Kick off\n" \
"Pass = Pass ball\n" \
"Scrum = break Scrum\n" \
"Paddle directions: move players"
},
{ // 12. Astro Invader
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"Fire", "-----","Fire", "Pause","-----","Unpau",
"Fire", "Fire", "Fire", "Pause","-----","Unpau",
"Fire", "Fire", "Fire", "Pause","-----","Unpau",
"Fire", "Fire", "Fire", "Pause","-----","Unpau",
"START (F1): start game\n" \
"A (F2): start demo\n" \
"Unpau = Unpause\n" \
"Paddle left/right: move left/right"
},
{ // 13. Arcadia Basketball
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Ru/Dr","-----","-----","Ru/Dr","-----","-----",
"Sh/Bl","P1",   "-----","Sh/Bl","P1",   "-----",
"Pass", "P2",   "-----","Pass", "P2",   "-----",
"-----","P3",   "-----","-----","P3",   "-----",
"A (F2) then START (F1): start game\n" \
"Ru/Dr = Run/Dribble\n" \
"Sh/Bl = Shoot/Block\n" \
"Pass = Pass ball\n" \
"P# = Player # (hold down)\n" \
"Paddle directions: direction of action"
},
{ // 14. Super Bug
       "Fire",               "-----",
"Fire","Fire","Fire","-----","-----","-----",
"Fire","Fire","Fire","-----","-----","-----",
"Fire","Fire","Fire","-----","-----","-----",
"Fire","Fire","Fire","-----","-----","-----",
"Fire","Fire","Fire","-----","-----","-----",
"A (F2): choose difficulty (1=50 secs, 2=40 secs, 3=30 secs)\n" \
"START (F1): start game\n" \
"Paddle directions: move bug"
},
{ // 15. default (Interton)
        "2/Fi",                 "2/Fi",
"-----","-----","-----","-----","-----","-----",
"1",    "-----","3",    "1",    "-----","3",
"4",    "5",    "6",    "4",    "5",    "6",
"7",    "8",    "9",    "7",    "8",    "9",
"Clear","0",    "Enter","Clear","0",    "Enter",
""
},
{ // 16. Battle
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"Deact","-----","-----","Deact","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Activ","-----","-----","Activ","-----","-----",
"A (F2): toggle between 1st and 2nd levels\n" \
"B (F3): toggle \"birds\" on/off?\n" \
"START (F1): start game\n"
"Activ = Activate (enter tank)\n" \
"Deact = Deactivate (exit tank)\n" \
"Paddle directions: move man/tank"
},
{ // 17. Arcadia Boxing
        "Du/2",                 "Du/2",
"-----","-----","-----","-----","-----","-----",
"hF/1", "-----","hF/3", "hF/1", "-----","hF/3",
"hP",   "-----","hP",   "hP",   "-----","hP",
"mP",   "-----","mP",   "mP",   "-----","mP",
"lP/Re","-----","lP",   "lP/Re","-----","lP",
"1..3: choose men\n" \
"Re = boxer Ready\n" \
"F = feint\n" \
"P = punch\n" \
"Du = Duck\n" \
"h/m/l = high/medium/low\n" \
"Paddle left/right: move man"
},
{ // 18. Breakaway
        "Hold",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Fast", "Fast", "Fast", "-----","-----","-----",
"Fast", "Fast", "Fast", "-----","-----","-----",
"Fast", "Fast", "Fast", "-----","-----","-----",
"A (F2): choose \"game\" (1..16)\n" \
"B (F3): toggle flag line on/off (ie. invert colours)\n" \
"START (F1): start game\n" \
"Hold = Hold ball (eg. in game variant 3)\n" \
"Fast = Faster paddle movement while held down\n" \
"Paddle left/right: move paddle left/right or up/down\n\n" \
"The paddle can only deflect the ball forwards, not backwards."
},
{ // 19. Arcadia Capture
        "Finis",                "Finis",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Up",   "-----","-----","Up",   "-----",
"Left", "-----","Right","Left", "-----","Right",
"-----","Down", "-----","-----","Down", "-----",
"START (F1): start game\n" \
"A (F2); choose difficulty (1..6)\n" \
"1..5 are 1-player games, 6 is 2-player game\n" \
"B (F3): set time limits\n" \
"While setting time limits:\n" \
"Left/Right = decrease/increase minutes\n" \
"Up/Down = decrease/increase seconds\n" \
"During play:\n" \
"Up/Down/Left/Right = move cursor\n" \
"Finis = Finished"
},
{ // 20. Cat Trax, Nibblemen, Super Gobbler
        "Hyper",                "-----",
"-----","-----","-----","-----","-----","-----",
"Close","-----","Open", "Pause","-----","Unpau",
"Close","Hyper","Open", "Pause","-----","Unpau",
"Close","Hyper","Open", "Pause","-----","Unpau",
"Close","Hyper","Open", "Pause","-----","Unpau",
"A (F2): select \"game\" (1..4)\n" \
"B (F3): select \"option\" (1..8)\n" \
"START (F1): start game\n" \
"Close = Close gates\n" \
"Hyper = Hyperspace (teleport)\n" \
"Open = Open gates\n" \
"Unpau = Unpause\n" \
"Paddle directions: move"
},
{ // 21. Funky Fish
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"Fire", "-----","Fire", "Fire", "-----","Fire",
"Fire", "Fire", "Fire", "Fire", "Fire", "Fire",
"Fire", "Fire", "Fire", "Fire", "Fire", "Fire",
"Pause","Fire", "Unpau","Fire", "Fire", "Fire",
"A (F2): toggle between 1 and 2 players\n" \
"START (F1): start game\n" \
"Unpau = Unpause\n" \
"Player 1 is right controller (when unswapped) and right status area.\n" \
"Player 2 is left controller (when unswapped) and left status area."
},
{ // 22. Arcadia Golf
        "Hit",                  "Hit",
"-----","-----","-----","-----","-----","-----",
"-----","-----","Score","-----","-----","Score",
"Drive","3Wood","5Wood","Drive","3Wood","5Wood",
"3Iron","5Iron","7Iron","3Iron","5Iron","7Iron",
"9Iron","Wedge","Putte","9Iron","Wedge","Putte",
"A (F2): 1-player mode\n" \
"B (F3): 2-player mode\n" \
"START (F1): start game\n" \
"Hit = Hit ball\n" \
"Score = cumulative Score\n" \
"Drive = Driver\n" \
"Putte = Putter\n" \
"#Iron = # Iron\n" \
"#Wood = # Wood\n" \
"Paddle up/down: move golfer clockwise/anticlockwise"
},
{ // 23. Grand Slam Tennis
        "Step" ,                "Step" ,
"-----","-----","-----","-----","-----","-----",
"-----","-----","Hard" ,"-----","-----","Hard" ,
"-----","4"    ,"Mediu","-----","4"    ,"Mediu",
"-----","2"    ,"Gentl","-----","2"    ,"Gentl",
"Color","1"    ,"-----","Color","1"    ,"-----",
"Color = player colour, supposedly (has no apparent effect).\n" \
"A (F2): cycle through 2 players, 2 pro, 1 pro, L pro, 1 hit, R pro, 1 player modes\n" \
"B (F3): cycle through \"slow\", \"fast\" and \"easy\" modes\n" \
"START (F1): start game\n" \
"When the game is waiting for you to serve:\n" \
"Any button: serve\n" \
"During play:\n" \
"Hard/Mediu/Gentl = hard/medium/gentle\n" \
"1/2/4 = racket size\n" \
"Paddle direction: move player\n\n" \
"The players will swing automatically when near the ball."
},
{ // 24. Ocean Battle
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Subma","-----","Destr","Subma","-----","Destr",
"Lt/Su","Cruis","Rt/De","Lt/Su","Cruis","Rt/De",
"START (F1): start game\n" \
"Cruis = Cruiser\n" \
"Destr = Destroyer\n" \
"Subma = Submarine\n" \
"Lt/Su = move Left (on combat screen)/Submarine (on map screen)\n" \
"Rt/De = move Right (on combat screen)/Destroyer (on map screen)\n" \
"Paddle directions (on map screen): move selected ship\n" \
"Paddle directions (on combat screen): aim gun"
},
{ // 25. Horse Racing (Arcadia)
        "2/Yel",                "2/Yel",
"-----","-----","-----","-----","-----","-----",
"1/Wht","-----","3/Cyn","1/Wht","-----","3/Cyn",
"4/Grn","5/Mag","6/Red","4/Grn","5/Mag","6/Red",
"7/Blu","8/Blk","9",    "7/Blu","8/Blk","9",
"Repor","0",    "Enter","Repor","0",    "Enter",
"START (F1): skip title screen\n" \
"1/Wht = 1/White/Win/Whip\n" \
"2/Yel = 2/Yellow\n" \
"3/Cyn = 3/Cyan/Exacta/Whip\n" \
"4/Grn = 4/Green\n" \
"5/Mag = 5/Magenta\n" \
"6/Red = 6/Red\n" \
"7/Blu = 7/Blue\n" \
"8/Blk = 8/Black\n" \
"Repor = Report (while held)\n" \
"Only 4 horses run in each race; their colours and odds are shown at the bottom of the screen.\n" \
"Betting is as follows:\n"
"Press 1..6 for which player is betting, then Enter.\n" \
"Give bet amount (minimum is $2), then press Enter.\n" \
"Choose type of bet (Win or Exacta), then press Enter.\n" \
"Choose colour of 1st placed horse (winner), then press Enter.\n" \
"If an exacta bet, choose colour of 2nd placed horse, then press Enter.\n" \
"When done placing bets, press 0 then Enter.\n"
"During race:\n" \
"Whip = whip horse\n" \
"Paddle up/down: move horse"
},
{ // 26. Jungler
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"Close","-----","-----","-----","-----","-----",
"Close","Fire", "-----","-----","-----","-----",
"Open", "Fire", "-----","-----","-----","-----",
"Open", "Fire", "-----","-----","-----","-----",
"A (F2): cycle through \"games\" (1..9)\n" \
"B (F3): cycle through \"options\" (1..8)\n" \
"START (F1): start game\n" \
"Open = Open gates\n" \
"Close = Close gates\n" \
"Paddle directions: turn snake\n" \
"Paddle directions + Fire: shoot"
},
{ // 27. Monaco Grand Prix
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Accel","-----","-----","Accel","-----",
"Left", "-----","Right","Left", "-----","Right",
"-----","Brake","-----","-----","Brake","-----",
"START (F1): start game\n" \
"Accel = Accelerate\n" \
"Left/Right = turn Left/Right"
},
{ // 28. Alien Invaders
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): start/restart game\n" \
"Paddle left/right: move left/right"
},
{ // 29. Pleiades
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"Pause","-----","Unpau","-----","-----","-----",
"Pause","Fire", "Unpau","-----","-----","-----",
"Pause","Fire", "Unpau","-----","-----","-----",
"Pause","Fire", "Unpau","-----","-----","-----",
"START (F1)/B (F3): start game\n" \
"A (F2): start demo\n" \
"Unpau = Unpause\n" \
"Paddle left/right: move left/right\n" \
"For each life except the first, you are invincible until you have fired."
},
{ // 30. R2D Tank
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"Fire", "-----","Fire", "Fire", "-----","Fire",
"Fire", "Fire", "Fire", "Fire", "Fire", "Fire",
"-----","-----","-----","Fire", "Fire", "Fire",
"Pause","-----","Unpau","Fire", "Fire", "Fire",
"A (F2): select \"game\" (1 = 1 player, 2 = 2 players, 3 = demo)\n" \
"B (F3): select \"option\" (ie. playfield) (1..3)\n" \
"START (F1): start game\n" \
"Unpau = Unpause"
},
{ // 31. Arcadia 2D Soccer
        "Shoot",                "Shoot",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"1L+3L","Centr","1R+3R","1L+3L","Centr","1R+3R",
"2L",   "4L+4R","2R",   "2L",   "4L+4R","2R",
"-----","Goalk","-----","-----","Goalk","-----",
"START (F1): start game\n" \
"Centr = Centre\n" \
"Goalk = Goalkeeper\n" \
"Paddle up/down/left/right: move player(s) right/left/up/down (!)"
},
{ // 32. Space Attack
        "-----",                "Fire",
"-----","-----","-----","-----","-----","-----",
"Pause","-----","Unpau","-----","-----","-----",
"Pause","-----","Unpau","-----","Fire", "-----",
"Pause","-----","Unpau","-----","Fire", "-----",
"Pause","-----","Unpau","-----","Fire", "-----",
"A (F2): toggle between game and demo\n" \
"START (F1): start game/demo\n" \
"B (F3): start demo\n" \
"Unpau = Unpause"
"Paddle left/right: move ship left/right\n\n" \
"Note that your ship starts to the right of the visible screen; " \
"therefore move left at the start of each life."
},
{ // 33. Space Mission
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Bridg","Load", "Unloa","-----","-----","-----",
"Scan", "Launc","Steer","-----","-----","-----",
"START (F1): start game\n" \
"Bridg = Bridge\n" \
"Unloa = Unload\n" \
"Launc = Launch droneship\n" \
"Steer = Steer droneship\n" \
"Paddle directions: move view"
},
{ // 34. Space Raiders
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","Bomb", "Pause","-----","Unpau",
"-----","Fire", "Bomb", "Pause","-----","Unpau",
"-----","Fire", "Bomb", "Pause","-----","Unpau",
"-----","Fire", "Bomb", "Pause","-----","Unpau",
"START (F1): start game\n" \
"Bomb = drop Bomb (these can only destroy ground-based targets)\n" \
"Unpau = Unpause\n" \
"Paddle directions: move"
},
{ // 35. Space Vultures
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"Shiel","-----","-----","Pause","-----","Unpau",
"Shiel","Fire", "-----","Pause","-----","Unpau",
"Shiel","Fire", "-----","Pause","-----","Unpau",
"Shiel","Fire", "-----","Pause","-----","Unpau",
"START (F1) (while not playing): start at small hawks (with 26 small hawks)\n" \
"START (F1) (while playing small hawks): start at mother hawks\n" \
"START (F1) (while playing mother hawks): start at small hawks (with 0..6 small hawks)\n" \
"Shiel = Shield\n" \
"Unpau = Unpause\n" \
"Paddle directions: move\n" \
"Note that your ship starts to the left of the visible screen; " \
"therefore you must move right at the start of each life.\n" \
"Shield and vertical movement are only possible on 1st level (\"small hawks\").\n" \
"Yellow ship is vulnerable. Blue ship is invulnerable (shielded).\n" \
"Small hawks: 0..6 of these (depending on results of mother hawks stage), or 26 if starting a new game.\n"
"Mother hawks: always 3 of these.\n" \
"Game alternates between small hawks and mother hawks."
},
{ // 36. Spiders, The End, Turtles/Turpin
        "-----",                 "Fire",
"-----","-----","-----", "-----","-----","-----",
"Pause","-----","Unpau", "-----","-----","-----",
"Pause","-----","Unpau", "-----","Fire", "-----",
"Pause","-----","Unpau", "-----","Fire", "-----",
"Pause","-----","Unpau", "-----","Fire", "-----",
"START (F1): start game\n" \
"A (F2): start demo\n" \
"Unpau = Unpause\n" \
"Paddle up/left/right: move up/left/right"
},
{ // 37. Star Chess
        "Activ",                "Activ",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Move", "Up",   "Repor","Move", "Up",   "Repor",
"Left", "Clear","Right","Left", "Clear","Right",
"Fire", "Down", "Warp", "Fire", "Down", "Warp",
"START (F1): start game\n" \
"Activ = Activate\n" \
"Repor = Report\n" \
"Up/Down/Left/Right = move cursor\n\n" \
"Blue cursor: player 1 (left, green)'s turn\n" \
"Yellow cursor: player 2 (right, red)'s turn\n" \
"To move: move cursor to source piece, press Move, move cursor to destination, press Activ.\n" \
"To fire: move cursor to source source, press Fire, move cursor to destination piece, press Activ.\n" \
"For status: move cursor to piece, press Repor, press Activ. + are missiles, | are shields. Then press Clear.\n\n" \
"Starting (and maximum) missiles and shields are:\n" \
"Pawn: 2 missiles and shields.\n" \
"Rook/knight/bishop: 4 missiles and shields.\n" \
"King/queen: 7 missiles and shields."
},
{ // 38. Tanks a Lot
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"Close","-----","Man", "Pause", "-----","Unpau",
"Close","Fire", "Man", "Pause", "-----","Unpau",
"Open", "Fire", "Base","Pause", "-----","Unpau",
"Open", "Fire", "Base","Pause", "-----","Unpau",
"A (F2): cycle through \"games\" (1..4)\n" \
"B (F3): cycle through \"options\" (1..4)\n" \
"START (F1): start game\n" \
"Open = Open gate\n" \
"Close = Close gate\n" \
"Man = warp Man\n" \
"Base = warp Base\n" \
"Unpau = Unpause\n" \
"Paddle directions: move"
},
{ // 39. Tetris
        "Spin",                 "Spin",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Spin", "-----","-----","Spin", "-----",
"Left", "Spin", "Right","Left", "Spin", "Right",
"-----","Down", "-----","-----","Down", "-----",
"A (F2): toggle between 1 and 2 players\n" \
"B (F3): cycle through KEYPAD/KEYPAD, JOYSTICK/KEYPAD, KEYPAD/JOYSTICK, JOYSTICK/JOYSTICK modes\n" \
"Spin = Spin (turn, rotate) clockwise\n" \
"Paddle directions: manipulate piece (in JOYSTICK mode)"
},
{ // 40. Frogger 1.27, JTron 1.0-1.2
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Up",   "-----","-----","-----","-----",
"Left", "Down", "Right","-----","-----","-----",
"-----","Down", "-----","-----","-----","-----",
"START (F1): start game\n" \
"Up/Down/Left/Right: turn (JTron)/move (Frogger)"
},
{ // 41. Laser Attack
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Fire", "-----","-----","-----","-----",
"-----","Fire", "-----","-----","-----","-----",
"-----","Fire", "-----","-----","-----","-----",
"SELECT (F2): choose difficulty (1..5):\n" \
"1: easy\n" \
"2: medium\n" \
"3: hard\n" \
"4: very hard\n" \
"5: very easy\n" \
"START (F1): start game\n" \
"Paddle directions: move\n" \
"Scoring is:\n" \
"1st..2nd enemies: 1\n" \
"3rd..4th: 2\n" \
"5th..6th: 3\n" \
"7th..8th: 5\n" \
"9th+: 10\n" \
"Extra life every 100 points."
},
{ // 42. JTron 1.21
        "Start",                "-----",
"-----","-----","-----","-----","-----","-----",
"Start","-----","Start","-----","-----","-----",
"-----","Up",   "-----","-----","-----","-----",
"Left", "Down", "Right","-----","-----","-----",
"-----","Down", "-----","-----","-----","-----",
"START (F1) or Start: Start game\n" \
"Up/Down/Left/Right: turn"
},
{ // 43. 10-1-Invaders (aka "Aliens")
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Start","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Left", "Up",   "Right","-----","-----","-----",
"Score","-----","-----","-----","-----","-----",
"Up/Left/Right = shoot Up/Left/Right\n" \
"Score = show Score\n" \
"After starting, there is a delay before the enemies appear.\n\n" \
"Shoot the aliens. The higher up the screen they are, the more points you gain. You have three launches; losing the third ends the game."
},
{ // 44. 07-4-Amazone
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Finis","-----","-----","Finis","-----","-----",
"-----","Up",   "-----","-----","Up",   "-----",
"Left", "-----","Right","Left", "-----","Right",
"-----","Down", "-----","-----","Down", "-----",
"UC (F2): select play variant\n" \
"LC (F3): change time limit\n" \
"START (F1): start\n" \
"Finis = Finished\n" \
"Up/Down/Left/Right = move\n" \
"Each 'amazone' has the capabilities of both queen and knight in chess: it attacks squares along horizontal, vertical and diagonal lines, and also any square that can be reached by two horizontal moves followed by one vertical, and vice versa. The players alternately place their amazone on a square which is not under attack, and which has not been used earlier on in the game. A player loses when there is no legal square left to move, or when he runs out of time.\n" \
"In this version, the time display consists of the two vertical bars at the left and right of the scren; the score is indicated at the top of the screen. For practice, the game can be played against the computer."
},
{ // 45. 10-E-Galgenspiel, 10-F-Hangman
        "-----",                "Fast",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","Test", "Stop", "Resum",
"UC (F2): select word length (1 = any length, 4..8 = 4..8 letters)\n" \
"START (F1): start\n" \
"Fast = cycle Faster\n" \
"Stop = Stop letter cycling\n" \
"Resum = Resume letter cycling\n" \
"Test = Test letter\n\n" \
"Guess a word that is selected at random from an extensive vocabulary."
},
{ // 46. 09-4-Seawar
        "Fast",                 "Fast",
"-----","-----","-----","-----","-----","-----",
"72°WS","-----","72°ES","72°WN","-----","72°EN",
"54°WS","Slow", "54°ES","54°WN","Slow", "54°EN",
"36°WS","-----","36°ES","36°WN","-----","36°EN",
"18°WS","South","18°ES","18°WN","North","18°EN",
"#°XX = #° East/West from due North/South"
},
{ // 47. CODEBREA & MASTERMI (Interton)
        "8",                    "8",
"-----","-----","-----","-----","-----","-----",
"7",    "-----","-----","7",    "-----","-----", // 9 is not used
"4",    "5",    "6",    "4",    "5",    "6",
"1",    "2",    "3",    "1",    "2",    "3",
"Clear","Selec","Enter","Clear","Selec","Enter",
"SELECT (F2): choose game variant\n" \
"START (F1): start game\n" \
"Selec = Select position (ie. move cursor)"
},
{ // 48. 07-1-Mastermind
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","A",    "-----","-----","Test",
"-----","-----","B",    "-----","-----","-----",
"-----","-----","C",    "-----","-----","-----",
"Resta","-----","D/1",  "2",    "3",    "4",
"Resta = Restart\n" \
"Test = Test (ie. guess)\n" \
"A/B/C/D = set colour of 1st/2nd/3rd/4th symbol\n" \
"1/2/3/4 = change colour of 1st/2nd/3rd/4th symbol\n" \
"The object of this game is well known: guess an unknown (colour) code, on the basis of very limited 'feedback'. In this version, purple squares indicate how many correct colours are correctly positioned, and white squares indicate the remaining correct colours in the wrong position. All eight colours are used (black included!), and the same colour may appear more than once in a code.\n" \
"The computer plays the part of the 'passive' player: it sets up a code, and displays the result of each of your tries."
},
{ // 49. 11-7-Omega Landing
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): start\n" \
"Left/right on paddle: slow/fast\n\n" \
"Land your spacecraft in a hostile environment!"
},
{ // 50. 11-9-Tiny Tim
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Start","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Start = Start/restart\n" \
"Paddle directions: move\n\n" \
"Remove enough rocks and you reach the lovely girl!"
},
{ // 51. 09-6-Labyrinth
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Drop", "-----","-----","-----","-----","-----",
"-----","Up",   "-----","-----","-----","-----",
"Left", "-----","Right","-----","-----","-----",
"No",   "Down", "-----","-----","-----","-----",
"Startup questions are (yes/no):\n" \
"1st: use random object positions?\n" \
"2nd: can dragons flee?\n" \
"3rd: can dragons be resurrected?\n" \
"4th: do question marks teleport you to random location?\n" \
"START (F1): start/menu\n" \
"UC (F2): restart\n" \
"Drop = Drop object (hold down while moving away from object)\n" \
"Down = Down/yes"
},
{ // 52. 09-A-CardTrick
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"1",    "2",    "3",    "-----","-----","-----",
"1/2/3 = which heap"
},
{ // 53. 09-B-Awari
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","4",    "5",    "6",    "-----",
"-----","-----","-----","1",    "2",    "3",
"1 = human first/empty 1st pot\n" \
"2 = computer first/empty 2nd pot\n" \
"3/4/5/6 = empty 3rd/4th/5th/6th pot"
},
{ // 54. 09-C-UFO Shooting
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): start"
},
{ // 55. 09-D-Raster
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Start","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Fire", "-----","-----","-----","-----",
"-----","-----","Pause","-----","-----","-----",
"Pause = Pause (while held down)"
},
{ // 56. 09-E-Nim
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Swtch","-----","-----","Swtch","-----",
"Row",  "-----","Remov","Row",  "-----","Remov",
"UC (F2): toggle one/two players\n" \
"START (F1): select number of rows\n" \
"Row = change which Row\n" \
"Remov = Remove a match\n" \
"Swtch = Switch to opponent"
},
{ // 57. 09-F-Solitaire
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Up",   "-----","-----","-----","-----",
"Left", "-----","Right","-----","-----","-----",
"-----","Down", "-----","-----","-----","-----",
"Paddle directions: move cursor\n" \
"Up/Down/Left/Right = jump Up/Down/Left/Right"
},
{ // 58. 09-2-Offshore Fishing
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Start","-----","Varia","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Catch","-----","-----","Catch","-----",
"-----","-----","-----","-----","-----","-----",
"Varia = choose game Variant 1..4 (1 or 3 are 1 player, 2 or 4 are 2 players)\n" \
"Catch = Catch fish\n" \
"Paddle directions: move boat/line"
},
{ // 59. 09-7-Destroyer
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Start","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"UC (F2): jump to BIOS\n" \
"Paddle directions: move"
},
{ // 60. 09-8-Starship Enterprise
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Start","-----","-----","-----","-----","-----",
"-----","Up",   "-----","-----","-----","-----",
"Left", "Recha","Right","-----","-----","-----",
"-----","Down", "-----","-----","-----","-----",
"LC (F3): choose difficulty\n" \
"Up/Down/Left/Right = shoot Up/Down/Left/Right\n" \
"Recha = Recharge"
},
{ // 61. 09-9-Blackjack
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","8",    "9",    "-----","-----",
"-----","-----","4",    "5",    "6",    "7",
"Stand","Enter","0",    "1",    "2",    "3",
"LC (F3): double bet\n" \
"Enter = Enter/hit/new game"
},
{ // 62. 09-5-Memory
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"7",    "8",    "9",    "7",    "8",    "9",
"4",    "5",    "6",    "4",    "5",    "6",
"1",    "2",    "3",    "1",    "2",    "3",
"START (F1): start/help\n" \
"UC (F2): restart"
},
{ // 63. 11-F-Cosmic Adventure
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Fire", "-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Fire = Fire/start\n" \
"Paddle directions: move/turn\n\n" \
"A 3-part game: first, destroy 12 meteorites; second, land under enemy fire; third, find the nuclear reactor in the underground maze."
},
{ // 64. 11-D-Painting
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Start","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Paddle directions: move\n\n" \
"Colour the screen without being bitten."
},
{ // 65. 11-E-Submarines & Racing
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"UC (F2): choose which game (must be held down)\n" \
"START (F1): start (must be held down)\n" \
"Fire = Fire (submarine game only)\n" \
"Paddle left/right: move\n\n" \
"2 different games: in the first, the submarine tries to sink as many ships as possible; in the second, a racing car must avoid other cars on the track for as long as possible."
},
{ // 66. 03-3-MusicBox, 07-B-Piano
        "G",                    "Bb",
"-----","-----","-----","-----","-----","-----",
"F#",   "-----","G#",   "A",    "-----","B",
"C",    "C#",   "D",    "Eb",   "E",    "F",
"F#",   "G",    "G#",   "A",    "Bb",   "B",
"C",    "C#",   "D",    "Eb",   "E",    "F",
"UC (F2): rest\n" \
"START (F1): start/stop playing song\n" \
"Any desired melody can be keyed in, provided it fits in a two-octave range. When played back, the corresponding notes are displayed in step with the tune."
},
{ // 67. 03-5-Surround, 07-9-Surround
        "Up",                   "Up",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Left", "-----","Right","Left", "-----","Right",
"-----","Down", "-----","-----","Down", "-----",
"-----","-----","-----","-----","-----","-----",
"Wait at the title screen.\n" \
"UC (F2): reset\n" \
"START (F1): start\n" \
"Up/Down/Left/Right: turn\n" \
"The object of this game is to 'box in' the opponent. A point is lost when a player collides with the background, his opponent or even himself. Since the objects move at quite a speed, fast reactions are required to win!"
},
{ // 68. 07-8-Jackpot
        "Turn",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"R1",   "R2",   "R3",   "-----","-----","-----",
"H1",   "H2",   "H3",   "-----","-----","-----",
"Turn = Turn drums (or can use paddle for this)\n" \
"R1/R2/R3 = Release 1st/2nd/3rd drum\n" \
"H1/H2/H3 = Hold 1st/2nd/3rd drum\n" \
"START (F1): take win\n" \
"This is the well-known 'one-armed bandit'. The rotating drums, 'hold' and 'brake' options and prize display are all included. There are also two novel features. A car on the centre line always means 'no prize' - cars cost money! Furthermore, the 'score' at the top of the screen shows the total gain or loss (nearly always the latter...)."
},
{ // 69. 07-3-Reversi
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Turn", "-----","-----","-----","-----","-----",
"-----","Selec","-----","-----","Selec","-----",
"-----","-----","-----","-----","-----","-----",
"Horiz","Swtch","Verti","Horiz","Swtch","Verti",
"START (F1): start\n" \
"Turn = Turn counter\n" \
"Selec = Select counter\n" \
"Swtch = Switch to opponent\n" \
"Horiz/Verti = move cursor Horizontally/Vertically\n" \
"Basically, the object of this game is to capture as many of the opponent's pieces as possible. To do this, you place a piece on the board in such a way that one or more of the opponent's pieces are 'trapped' between two of yours. When captured, squares change to your colour - they are not removed from the board."
},
{ // 70. 10-6-Pilot
        "Start",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","Gas",  "-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","Easy", "Hard", "-----","-----",
"Easy/Hard = Easy/Hard difficulty\n" \
"Start = Start engines\n" \
"Gas = give Gas\n" \
"Paddle directions: fly plane\n\n" \
"Pilot an aeroplane through a complete sequence of take-off, flight and landing, on the basis of instrument readings and instructions from the ground. The instruments are an artificial horizon (in the centre of the screen), an altimeter (upper right) and an airspeed indicator (upper left). Furthermore, an indication of your progress during take-off, along the flight path and when landing is given in the lower half of the screen. To the left and right of this, arrows indicate any required course changes (or 'OK') and instructions to increase or decrease altitude. These instructions must be followed up immediately."
},
{ // 71. 10-5-Catapult
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"RESET to game (F5): press this first\n" \
"Fire = Fire (hold down longer to shoot further)\n\n" \
"Destroy your opponent's fortress by throwing rocks at it."
},
{ // 72. 10-3-Helicopter
        "Drop",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----", "8",   "-----","-----","-----",
"-----","-----", "4",   "5",    "6",    "7",
"-----","-----","-----","1",    "2",    "3",
"1..7 = number of crates\n" \
"8 = unlimited play\n" \
"START (F1): start\n" \
"Paddle directions: fly helicopter\n" \
"Drop = Drop (release) crate\n\n" \
"Transport crates across a mountain, by helicopter, and lower them carefully onto a truck. The number of crates can be selected in advance; the fuel supply is restricted accordingly."
},
{ // 73. 10-2-Pinball (aka "Flipper")
        "-----",                "Ball",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","Left", "-----","Right",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Ball = new Ball\n" \
"Left/Right = move Left/Right flipper\n\n" \
"Keep the ball in play for as long as possible. You have five balls for each game, as displayed at lower left."
},
{ // 74. Interton 4INAROW
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Move", "-----","Drop", "Move", "-----","Drop",
"SELECT (Interton)/UC (Elektor) (F2): choose game variant (1-14):\n" \
"1,8: 2 players (left=red, right=green); 2-7,9-14: 1 player (left=red, computer=green)\n" \
"2,5,9,12: easy; 3,6,10,13: medium; 4,7,11,14: hard\n" \
"1-7: small field; 8-14: large field\n" \
"1-4,8-11: red starts; 5-7,12-14: green starts\n" \
"START (F1): start game\n" \
"Move = Move cursor right\n" \
"Drop = Drop token"
},
{ // 75. 10-9-Rocket Hunting (aka "Space Hunt")
        "SOS",                  "-----",
"-----","-----","-----","-----","-----","-----",
"Start","-----","Shiel","-----","-----","-----",
"-----","Up",   "-----","-----","-----","-----",
"Left", "Phase","Right","-----","-----","-----",
"Remai","Down", "-----","-----","-----","-----",
"LC (F3): choose game duration\n" \
"UC (F2): end game\n" \
"Shiel = Shield on/off\n" \
"Phase = Phaser\n" \
"Remai = show Remaining enemies\n" \
"Up/Down/Left/Right = torpedo Up/Down/Left/Right\n" \
"Paddle directions: move\n\n" \
"Pass through several sectors of space. After eliminating all enemies in each sector, pass on to the next by moving off the right-hand edge of the screen. Enemies can be knocked out by ramming, provided your shields are raised (even then it is not recommended unless there is a starbase nearby). Unshielded enemies can be shot with torpedoes. Shielded enemies can only be killed by means of a phaser attack, but this is expensive on energy."
},
{ // 76. 10-A-Basketball
        "Go",                   "Go",
"-----","-----","-----","-----","-----","-----",
"Left", "-----","Right","Left", "-----","Right",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): start\n" \
"Go = Go back during horizontal movement\n" \
"Left/Right = affect Left/Right throw release angle\n" \
"Paddle directions: move\n\n" \
"To move vertically, you must first face vertically.\n" \
"If you move up while facing horizontally, you will throw instead.\n\n" \
"Score by landing the 'ball' in the 'basket'. Total playing time: five minutes. Players are moved horizontally by means of the joysticks. When a player is manoeuvered so that he appears facing forwards, vertical movement is also possible under joystick control (this corresponds to moving from one side of the field to the other)."
},
{ // 77. 10-7-Attack from Space (aka "Space Attack")
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Paddle directions: move\n\n" \
"Survive as many attack waves as possible, without being hit by the various missiles."
},
{ // 78. 11-B-Newton
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): start\n" \
"Paddle left/right: move left/right\n\n" \
"Catch as many apples as possible."
},
{ // 79. 11-5-Asteroids
        "Hyper",                "-----",
"-----","-----","-----","-----","-----","-----",
"Forwa","-----","Score","-----","-----","-----",
"Forwa","Hyper","Score","-----","-----","-----",
"Left", "Fire", "Right","-----","-----","-----",
"Left", "Fire", "Right","-----","-----","-----",
"START (F1): start\n" \
"Forwa = Forwards (ie. thrust)\n" \
"Left/Right = turn Left/Right\n" \
"Hyper = Hyperspace (teleport)\n" \
"Score = show Score\n\n"
"Try to survive for as long as possible by avoiding the asteroids."
},
{ // 80. Tester 3 (Arcadia)
        "Up-0",                 "Up-1",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Lt-0", "Dn-0", "Rt-0", "Lt-1", "Dn-1", "Rt-1",
"-----","Up-2", "-----","-----","Up-3", "-----",
"Lt-2", "Dn-2", "Rt-2", "Lt-3", "Dn-3", "Rt-3",
"Up-n/Dn-n/Lt-n/Rt-n = move sprite #n Up/Down/Left/Right (on appropriate screen)\n"\
"Up-0/Dn-0/Up-2/Dn-2 = advance to next screen"
},
{ // 81. 06-3-Rocket Shooting
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Up",   "-----","-----","-----","-----",
"Left", "Fire", "Right","-----","-----","-----",
"-----","Down", "-----","-----","-----","-----",
"LC (F3): jump to BIOS (don't press this key!)\n" \
"Up/Down/Left/Right = move Up/Down/Left/Right\n\n" \
"You cannot fire while moving the crosshair."
},
{ // 82. 09-1-Aggressor
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Paddle up/down: move"
},
{ // 83. 10-4-Hard Nut/Teaser
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----", "1",   "2",    "3",
"START (F1): start\n" \
"1/2/3 = desired grid coordinate\n" \
"Choose a + to change to a O.\n" \
"Hold desired X-location (column) until it appears.\n" \
"Now quickly hold desired Y-location (row) until it appears.\n\n" \
"Obtain a pattern consisting of a single square in the centre, surrounded by eight crosses."
},
{ // 84. 10-8-Reverse
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","8",    "-----","-----","-----",
"-----","-----","4",    "5",    "6",    "7",
"Numbe","-----","-----","1",    "2",    "3",
"Numbe = generate Number\n" \
"1..8 = reverse digits 1..X, eg.:\n" \
"1 will have no effect\n" \
"2 will reverse digits 1..2 (counting left to right)\n" \
"3 will reverse digits 1..3 (counting left to right)\n" \
"...\n" \
"8 will reverse all digits\n\n" \
"Obtain the correct numerical order (1..8) in the minimum number of steps. Set up the numbers in random order, by operating the \"-\" key at intervals of one or two seconds. Then reverse the order of any number of digits, counting from the left, by operating the corresponding number key (2...8). The number of steps required is indicated at the end of the game."
},
{ // 85. 10-B-Bursting Balloons
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Paddle down: new ball\n" \
"Paddle left/right: move\n\n" \
"Destroy the balloons (or 'ceiling bricks', if you prefer), by bouncing the missile off them. You have five missiles per game. Scoring: green = 1 point, blue = 5 points, red = 10 points. The 'highest score of the day' is displayed at the lower edge of the screen. When the last balloon is hit, a complete new set appears immediately, so that quite high scores can be achieved."
},
{ // 86. 11-8-Breakout
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Ball", "-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"LC (F3): adjust tens digit of game variant\n" \
"UC (F2): adjust ones digit of game variant\n" \
"START (F1): start\n" \
"Ball = new Ball\n" \
"Paddle left/right: move\n\n" \
"24 variations on 'Bursting Balloons' (ESS-010-B)."
},
{ // 87. 10-C-Für Elise
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): start\n\n" \
"Add a touch of 'culture' to the computer era."
},
{ // 88. Interton HORSERAC/HIPPODRO, Elektor 11-C-Horse Races
        "Jump",                 "Jump",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (Interton)/UC (Elektor) (F2): select game variant\n" \
"START (F1): start\n"
"Paddle up/down: regulate speed\n\n" \
"8 variations of show jumping."
},
{ // 89. 11-A-Horse Race/Jackpot
        "Rotat",                "-----",
"-----","-----","-----","-----","-----","-----",
"Score","-----","4th",  "-----","-----","-----",
"-----","-----","3rd/8","-----","-----","-----",
"-----","-----","2nd/4","5",    "6",    "7",
"Plays","-----","1st/0","1",    "2",    "3",
"UC (F2): play horse racing\n" \
"LC (F3): play jackpot\n" \
"Horse racing:\n" \
"1..8 = bet on horse\n" \
"START (F1): go\n" \
"Jackpot:\n" \
"Rotat (or left paddle right) = Rotate reels\n" \
"1st..4th = stop nth reel\n" \
"Score = show Score\n" \
"Plays = show Plays\n\n" \
"2 simple games of chance."
},
{ // 90. 11-1-Snakes and Ladders
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Start","-----","-----","Start","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Roll", "-----","-----","Roll", "-----",
"-----","-----","-----","-----","-----","-----",
"Left Start = human (left) vs. computer\n" \
"Left Start then Right Start = human (left) vs. human (right)\n" \
"Roll = Roll dice\n\n" \
"Reach square 100 before your opponent(s)."
},
{ // 91. 11-2-Molebasher
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Start","-----","Diffi","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Left", "-----","Right","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Diffi = choose Difficulty\n" \
"Start = Start\n" \
"START (F1): pause/unpause\n" \
"Left/Right = move Left/Right\n\n" \
"Catch as many moles as you can."
},
{ // 92. 11-3-Snap
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Snap", "-----","-----","Snap", "-----",
"-----","-----","-----","-----","-----","-----",
"UC (F2): adjust speed\n" \
"START (F1): start game\n" \
"LC (F3): restart game\n\n" \
"Try to beat your opponent in deciding whether two pictures on the screen are identical."
},
{ // 93. 11-4-Mazes
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"UC (F2): choose game variant\n" \
"START (F1): start game\n" \
"Paddle directions: move\n\n" \
"Escape from the maze before your time is up."\
},
{ // 94. Interton MOTOCROS, Elektor 11-6-Dragster
        "Accel",                "Accel",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (Interton)/UC (Elektor) (F2): choose game variant\n" \
"START (F1): start game\n" \
"Accel = Accelerate\n" \
"Paddle up+left = 1st gear\n" \
"Paddle down+left = 2nd gear\n" \
"Paddle up+right = 3rd gear\n" \
"Paddle down+right = 4th gear\n\n" \
"Complete nine laps in the shortest possible time."
},
{ // 95. Interton COMBATA/COMBATB
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant\n" \
"START (F1): start game\n" \
"Paddle up: accelerate\n" \
"Paddle left/right: steer left/right"
},
{ // 96. Car Races/Grand Prix (Interton)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant\n" \
"Paddle up + START (F1): start game\n" \
"Paddle up: accelerate\n" \
"Paddle left/right: steer left/right"
},
{ // 97. Interton BLACKJAC
        "Stand",                "Stand",
"-----","-----","-----","-----","-----","-----",
"Hit",  "-----","Doubl","Hit",  "-----","Doubl",
"Yes",  "-----","No",   "Yes",  "-----","No",
"Deal", "Tens", "Ones", "Deal", "Tens", "Ones",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): start 1-player game\n" \
"START (F1): start 2-player game\n" \
"Tens/Ones = adjust Tens/Ones digit of bet\n" \
"Deal = Deal cards\n" \
"Hit = Hit (ie. take another card)\n" \
"Stand = Stand (\"STAY\") (ie. don't take another card)\n" \
"Doubl = Double bet\n" \
"Yes/No = Yes/No to offers of insurance\n"
},
{ // 98. OLYMPICS
        "Incrs",                "Incrs",
"-----","-----","-----","-----","-----","-----",
"Angle","-----","Angle","Angle","-----","Angle",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant\n" \
"START (F1): start game\n" \
"Incrs = Increase ball speed (in some variants)\n" \
"Angle = change flying Angle (in some variants)\n" \
"Paddle directions: move"
},
{ // 99. AIRSEAAT/AIRSEABA
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant\n" \
"START (F1): start game\n" \
"Paddle left/right: move"
},
{ // 100. TREASURE
        "Show",                 "Show",
"-----","-----","-----","-----","-----","-----",
"Pass", "-----","Pass", "Pass", "-----","Pass",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant\n" \
"START (F1): start game\n" \
"Show = Show contents of square at cursor\n" \
"Paddle directions: move cursor"
},
{ // 101. SUPERMAZEPOS
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant (1..24)\n" \
"START (F1): start game\n" \
"Paddle directions: move"
},
{ // 102. WINTERSP
        "Start",                "Start",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant (1..7)\n" \
"START (F1): start game\n" \
"Start = Start\n" \
"On ramp:\n" \
"Paddle down = go faster\n" \
"In air:\n" \
"Paddle up/down/left/right = go faster/slower/left/right\n" \
"Slalom:\n" \
"Paddle directions: move skier"
},
{ // 103. HUNTING (Interton)/SHOOTGAL (Interton)/JAGEN (Elektor)
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant\n" \
"START (F1): start game\n" \
"Paddle left/right: turn/guide/move (depending on variant)"
},
{ // 104. Interton CHESS1/CHESS2
        "A/1",                  "A/1",
"-----","-----","-----","-----","-----","-----",
"EXCO", "-----","E/5",  "EXCO", "-----","E/5",
"EXEC", "B/2",  "F/6",  "EXEC", "B/2",  "F/6",
"Clear","C/3",  "G/7",  "Clear","C/3",  "G/7",
"COMP", "D/4",  "H/8",  "COMP", "D/4",  "H/8",
"SELECT (F2): choose computer intelligence\n" \
"START (F1): choose game variant (1..4)\n" \
"EXCO = EXecute COmmand\n" \
"EXEC = EXECUTE (record or play back)\n" \
"COMP = COMPuter (suggest move)"
},
{ // 105. Interton PINBALLA/PINBALLB
        "Launc",                "Launc",
"-----","-----","-----","-----","-----","-----",
"FlpLt","-----","FlpRt","FlpLt","-----","FlpRt",
"ObsLt","-----","ObsRt","ObsLt","-----","ObsRt",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant (1..8)\n" \
"START (F1): start game\n" \
"Launc = Launch ball\n" \
"FlpLt/FlpRt = move Left/Right Flipper\n" \
"ObsLt/ObsRt = move Obstacle Left/Right (while gate is open)\n" \
},
{ // 106. Interton CIRCUS
        "Start",                "Start",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant (1..16)\n" \
"START (F1): start game\n" \
"Start = Start (jump)\n" \
"Paddle left/right: move clown"
},
{ // 107. Interton BOXING
        "Punch",                "Punch",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): start game\n" \
"Punch = hard Punch/block\n" \
"Paddle up: soft punch with right hand\n" \
"Paddle down: soft punch with left hand\n" \
"Fire+paddle up: hard punch with right hand\n" \
"Fire+paddle down: hard punch with left hand\n" \
"Fire+paddle away = block\n" \
"Paddle left/right: move boxer"
},
{ // 108. Interton SPACEWAR
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant (1..8)\n" \
"START (F1): start game\n" \
"Fire = Fire bullets/rockets\n" \
"Paddle directions: move enemies"
},
{ // 109. CAPTURE (Interton)/Hocosoft REVERSI (Elektor)
        "Finis",                "Finis",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant (1..93)\n" \
"START (F1): start game\n" \
"Finis = Finish move\n" \
"Paddle directions: move cursor"
},
{ // 110. Interton SOCCERA/SOCCERB
        "Ki/St",                "Kick",
"-----","-----","-----","-----","-----","-----",
"11",   "-----","10",   "11",   "-----","10",
"9",    "8",    "7",    "9",    "8",    "7",
"6",    "5",    "4",    "6",    "5",    "4",
"3",    "2",    "1",    "3",    "2",    "1",
"Ki/St = Kick/Start game\n" \
"1..11 = move player 1..11\n" \
"Paddle directions: move player/steer ball"
},
{ // 111. Interton BOWLING
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant (1..16)\n" \
"START (F1): start game\n" \
"Paddle directions: move player"
},
{ // 112. Interton GOLF
        "Swing",                "Swing",
"-----","-----","-----","-----","-----","-----",
"30",   "-----","60",   "30",   "-----","60",
"80",   "100",  "120",  "80",   "100",  "120",
"130",  "150",  "165",  "130",  "150",  "165",
"175",  "200",  "250",  "175",  "200",  "250",
"SELECT (F2): toggle 1/2 players\n" \
"START (F1): start game\n" \
"30..250 = use club with this range\n" \
"Paddle directions: move player\n" \
"On putting green:\n" \
"Paddle directions: move player (before swing)/aim shot (during and briefly after swing)"
},
{ // 113. COCKPIT
        "Jump" ,                "-----",
"-----","-----","-----","-----","-----","-----",
"Close","-----","Open" ,"-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose difficulty (1..5)\n" \
"Takeoff:\n" \
"Open = Open throttle (ie. increase speed)\n" \
"Paddle down: take off\n" \
"Paddle left/right: steer\n" \
"Flight:\n" \
"Open+paddle down = ascend\n" \
"Close+paddle up = descend\n" \
"Ejecting: \n" \
"Jump = Jump from plane\n" \
"Paddle directions: move man\n" \
"Landing:\n" \
"Close = Close throttle (ie. decrease speed)"
},
{ // 114. METROPOL
        "Fast",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Confi","Stop", "Resta","-----","-----","-----",
"START (F1): start game\n" \
"Stop = Stop letter cycling\n" \
"Confi = Confirm\n" \
"Resta = Restart letter cycling\n" \
"Fast = cycle Faster"
},
{ // 115. Interton SOLITAIR
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Up",   "-----","-----","-----","-----",
"Left", "-----","Right","-----","-----","-----",
"-----","Down", "-----","-----","-----","-----",
"SELECT (F2): choose difficulty (1..3)\n" \
"START (F1): start game\n" \
"Paddle directions: move cursor\n" \
"Up/Down/Left/Right = jump Up/Down/Left/Right"
},
{ // 116. CASINO
        "Start",                "-----",
"-----","-----","-----","-----","-----","-----",
"Go",   "-----","Stop", "Go",   "-----","Stop",
"-----","-----","-----","-----","-----","-----",
"-----","-----","Score","-----","-----","Score",
"Bet", "10",  "1", "Be", "10",  "1",
"SELECT (F2): choose game variant (1..3)\n" \
"START (F1): start game\n" \
"For Roulette:\n" \
"Start = Start ball rolling\n" \
"Bet = Bet on this slot\n" \
"1/10 = +1/+10 to bet\n" \
"Score = show Score (wallet)\n" \
"For Jackpot (\"one-armed bandit\"):\n" \
"1/10 = +1/+10 to bet\n" \
"Paddle down = spin cylinders\n" \
"Go = restart middle cylinder when background is yellow\n" \
"Stop = Stop all cylinders when background is red"
},
{ // 117. INVADERA/INVADERB
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant (1..16)\n" \
"START (F1): start game\n" \
"Paddle left/right: move player/bullet"
},
{ // 118. SUPERINV
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant (1..15)\n" \
"START (F1): start game\n" \
"Paddle directions: move player/bullet"
},
{ // 119. BACKGAMM
        "Execu",                "Execu",
"-----","-----","-----","-----","-----","-----",
"Yes",  "-----","No",   "Yes",  "-----","No",
 "4",   "5",    "6",    "4",    "5",    "6",
 "1",   "2",    "3",    "1",    "2",    "3",
"Clear","Roll", "Set",  "Clear","Roll", "Set",
"SELECT (F2): choose game variant (1..4)\n" \
"START (F1): start/continue/restart game\n" \
"Yes/No = offer Double, or accept/refuse double\n" \
"Execu = Execute (confirm move)\n" \
"Clear = Clear (cancel)\n" \
"Roll = Roll dice\n" \
"Set = Set (ie. go to next counter (variants 1&4)) or change dice results (variants 2&3)\n" \
"1..6 = change dice results (variants 2&3) (after pressing Set)"
},
{ // 120. MONSTERM/SPIDERS-
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Color","-----","-----","-----","-----",
"SELECT (F2): choose game variant (1..12)\n" \
"START (F1): start game\n" \
"Color = toggle background Colour\n" \
"Paddle directions: move"
},
{ // 121. HYPERSPA
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","Stats","-----","-----","Stats",
"1",    "2",    "3",    "1",    "2",    "3",
"4",    "5",    "6",    "4",    "5",    "6",
"7",    "8",    "9",    "7",    "8",    "9",
"SELECT (F2): choose game variant (1..12)\n" \
"START (F1): start game\n" \
"Stats = Status\n" \
"1..9 = Hyperspace to quadrant 1..9\n" \
"Paddle directions: steer ship"
},
{ // 122. SUPERSPA
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant (1..12)\n" \
"START (F1): start game\n" \
"Paddle directions: steer ship/lower enemy\n" \
"Paddle up+down: drop enemy bomb (for right player)"
},
{ // 123. PICTURE 1.0
        "Color",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Color","-----","-----","-----","-----",
"-----","Color","-----","-----","-----","-----",
"-----","Color","-----","-----","-----","-----",
"Color = toggle sprite Colours"
},
{ // 124. PLANETDE
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Fire", "-----","-----","-----","-----",
"-----","Fire", "-----","-----","-----","-----",
"-----","Fire", "-----","-----","-----","-----",
"SELECT (F2): choose game variant (1-16):\n" \
"Odd/even: slow/fast cursor movement (supposedly)\n" \
"1-2,5-6,9-10,13-14: dumb cruise missiles (dots) (otherwise smart)\n" \
"Enemy firing rate (line/dot scoring):\n" \
"1-4: slowest (10/25), 5-8: slow (20/50), 9-12: fast (30/75), 13-16: fastest (30/75)\n" \
"START (F1): start game\n" \
"Paddle directions: move cursor\n" \
"35 player missiles available per round.\n"
"Cities are purple squares. Missile base is purple rectangle.\n" \
"1 missile is lost whenever missile base is hit.\n" \
"End of round bonus: 1 per missile remaining + 1 per city remaining\n" \
"Replacement city every 1000 points."
},
{ // 125. SHOOTOUT
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Fire", "-----","-----","Fire", "-----",
"-----","Fire", "-----","-----","Fire", "-----",
"-----","Fire", "-----","-----","Fire", "-----",
"SELECT (F2): choose game variant (1-24):\n" \
"Odd/even: cactus/coach\n" \
"1-2,5-6,9-10,13-14,17-18,21-22: indestructable cactus/coach (otherwise destructable)\n" \
"1-4,9-12,17-20: player is immobile after firing (otherwise mobile)\n" \
"1-16: 2 players (1-8: unlimited bullets. 9-16: 6 bullets each)\n"
"17-24: 1 player (99 bullets. 1 point per hit. Left=score, right=fired)\n" \
"START (F1): start game\n" \
"Paddle directions: move"
},
{ // 126. Mobile Soldier Gundam, Super Dimension Fortress Macross
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"A (F2): toggle game/demo mode\n" \
"START (F1): start game\n" \
"Paddle directions: move"
},
{ // 127. Space Squadron
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"Fire", "-----","Fire", "-----","-----","-----",
"Fire", "Fire", "Fire", "-----","-----","-----",
"Fire", "Fire", "Fire", "-----","-----","-----",
"Fire", "Fire", "Fire", "-----","-----","-----",
"A (F2): toggle game variant (1..2)\n" \
"START (F1): start game\n" \
"Paddle directions: move\n\n" \
"Mutant (white): 5 points\n" \
"Alien (yellow): 10 points\n\n" \
"Only mutants are shown on radar.\n" \
"Pressing START (F1) will zero your score but will not reset your lives (you should press RESET (F5) first)."
},
{ // 128. 3D Soccer A
        "Long",                 "Long",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Med.", "-----","-----","Med.", "-----",
"-----","Short","-----","-----","Short","-----",
"Pause","-----","Unpau","Pause","-----","Unpau",
"B (F3): start 1-player game\n" \
"START (F1): start 2-player game\n" \
"A (F2): start demo\n" \
"Long/Med./Short = Long/Medium/Short kick\n" \
"Unpau = Unpause\n" \
"Paddle up/down/left/right: move NE/SW/NW/SE"
},
{ // 129. 3D Soccer B
        "Kick1",                "Kick1",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Kick2","-----","-----","Kick2","-----",
"-----","Kick3","-----","-----","Kick3","-----",
"Pause","-----","Unpau","Pause","-----","Unpau",
"B (F3): start 1-player game\n" \
"START (F1): start 2-player game\n" \
"A (F2): start demo\n" \
"Kick1/2/3 = long/medium/short Kick\n" \
"Unpau = Unpause\n" \
"Paddle up/down/left/right: move NW/SE/SW/NE"
},
{ // 130. Auto Race
        "Brake",                "Brake",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"A (F2): change value\n" \
"B (F3): switch to next menu item\n" \
"START (F1): start game\n" \
"Paddle left/right: steer"
},
{ // 131. Baseball
        "T4",                   "T4",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","T3",   "M1",   "-----","T3",   "M1",
"-----","T2",   "M2",   "-----","T2",   "M2",
"Bat",  "T1",   "M3",   "Bat",  "T1",   "M3",
"START (F1): start game\n" \
"For batting team:\n" \
"T1 = batter Ready\n" \
"Bat = Bat\n" \
"M3 = run to next base\n" \
"For fielding team:\n" \
"Bat = pitch (throw)\n" \
"M1..M3+paddle = Move outfielder 1..3\n" \
"T1..T4 = Throw to baseman 1..4"
},
{ // 132. Arcadia Blackjack & Poker
        "2",                    "2",
"-----","-----","-----","-----","-----","-----",
 "1",   "-----","3",    "1",    "-----","3",
 "4",   "5",    "6",    "4",    "5",    "6",
 "7",   "8",    "9",    "7",    "8",    "9",
"Clear","0",    "Enter","Clear","0",    "Enter",
"A (F2): toggle 1/2 players\n" \
"B (F3): choose game\n" \
"START (F1): start game\n" \
"For Blackjack:\n" \
"7 = hit\n" \
"8 = double\n" \
"9 = stand\n" \
"0..9 = wallet/bet\n" \
"Cl = Clear/new wallet\n" \
"En = Enter/start game\n" \
"For Poker:\n" \
"0..9 = wallet/raise\n" \
"Clear = Clear/new wallet/view\n" \
"0 = drop (ie. fold)\n"
"Enter = Enter/start game/call"
},
{ // 133. Brain Quiz
        "2",                    "2",
"-----","-----","-----","-----","-----","-----",
"1",    "-----","3",    "1",    "-----","3",
"4",    "5",    "6",    "4",    "5",    "6",
"7",    "8",    "9",    "7",    "8",    "9",
"Clear","0",    "Enter","Clear","0",    "Enter",
"A (F2): choose game\n" \
"B (F3): choose game variant\n" \
"START (F1): start game\n" \
"For Mind Breaker:\n" \
"0..9 = number\n" \
"Clear = backspace\n" \
"Enter = Enter\n" \
"For Maxit:\n" \
"5/8/7/9 = move cursor up/down/left/right\n" \
"2 = finish move\n" \
"For Hangman:\n" \
"5/8/7/9 = move cursor up/down/left/right\n" \
"2 = select letter\n" \
"Clear = backspace\n" \
"Enter = Enter"
},
{ // 134. Arcadia Circus
        "Ju/Fl",                "Ju/Fl",
"Ju/Fl","Ju/Fl","Ju/Fl","Ju/Fl","Ju/Fl","Ju/Fl",
"Ju/Fl","Ju/Fl","Ju/Fl","Ju/Fl","Ju/Fl","Ju/Fl",
"Ju/Fl","Ju/Fl","Ju/Fl","Ju/Fl","Ju/Fl","Ju/Fl",
"Ju/Fl","Ju/Fl","Ju/Fl","Ju/Fl","Ju/Fl","Ju/Fl",
"Ju/Fl","Ju/Fl","Ju/Fl","Ju/Fl","Ju/Fl","Ju/Fl",
"A (F2): switch to 1-player mode, or cycle through 1-player variants\n" \
"B (F3): switch to 2-player mode, or cycle through 2-player variants\n" \
"START (F1): start game\n" \
"Ju/Fl = Jump/Flip (start/switch seesaw)\n" \
"Paddle left/right: move left/right"
},
{ // 135. Arcadia Combat
        "2/Fi",                 "2/Fi",
"-----","-----","-----","-----","-----","-----",
"1/Mo" ,"-----","3/Mo" ,"1/Mo" ,"-----","3/Mo",
"4/Mo" ,"5/Fi" ,"6/Mo" ,"4/Mo" ,"5/Fi" ,"6/Mo",
"7/Mo" ,"8/Fi" ,"9/Mo" ,"7/Mo" ,"8/Fi" ,"9/Mo",
"Cl/Mo","0/Fi" ,"En/Mo","Cl/Mo","0/Fi" ,"En/Mo",
"0..9 = choose game variant (1..88)\n" \
"Then press En to actually change to that game variant.\n" \
"START (F1): start game\n" \
"Cl = Clear"
"En = Enter"
"Fi = Fire\n" \
"Mo = Move forwards (tanks) or faster (planes)\n" \
"Paddle directions: steer vehicle/bullet"
},
{ // 136. Crazy Climber
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): skip title screen\n" \
"A (F2): choose game variant (1..4)\n" \
"START (F1): start game\n" \
"Paddle directions: move\n\n" \
"Curtains are green when being lowered, yellow when being raised."
},
{ // 137. Crazy Gobbler
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): start game\n" \
"Paddle directions: move"
},
{ // 138. Doraemon
        "Fly",                  "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"A (F2): choose game variant (Game 1/Game 2/Game 3/Demo)\n" \
"START (F1): start game\n" \
"Fly = Fly (use cross/helmet)\n" \
"Paddle directions: move"
},
{ // 139. Dr. Slump
        "Yell",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"A (F2): toggle game/demo mode\n" \
"B (F3): toggle level 1/2\n" \
"START (F1): start game\n" \
"Paddle directions + Yell = Yell (attack)\n" \
"Paddle directions: move"
},
{ // 140. Escape
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"Fire", "-----","Fire", "-----","-----","-----",
"Fire", "Fire", "Fire", "-----","-----","-----",
"Fire", "Fire", "Fire", "-----","-----","-----",
"Fire", "Fire", "Fire", "-----","-----","-----",
"A (F2): choose \"game\" (1..4)\n" \
"B (F3): choose \"option\" (1..4)\n" \
"START (F1): start game\n" \
"Paddle directions: move/aim"
},
{ // 141. Hobo
        "-----",                "Jump",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","Jump", "-----",
"-----","-----","-----","-----","Jump", "-----",
"-----","-----","-----","-----","Jump", "-----",
"START/B (F3): start game\n" \
"A (F2): start demo\n" \
"On level 1:\n" \
"Paddle up/down/left/right = move NE/SW/NW/SE\n" \
"On level 2:\n" \
"Paddle up/left/right = move N/NW/SE\n" \
"Jump = Jump (in diagonal corridors)\n" \
"On level 3:\n" \
"Paddle up/down/left/right: move up/down/left/right"
},
{ // 142. Jump Bug A
        "-----",                "Fire",
"-----","-----","-----","-----","-----","-----",
"Pause","-----","Unpau","-----","-----","-----",
"Pause","-----","Unpau","-----","-----","-----",
"Pause","-----","Unpau","-----","-----","-----",
"Pause","Level","Unpau","-----","-----","-----",
"A/B/START (F1): start game from level 1\n" \
"Level = change Level\n" \
"Unpau = Unpause\n" \
"Paddle up/down/left/right: move"
},
{ // 143. Jump Bug B
        "-----",                "Fire",
"-----","-----","-----","-----","-----","-----",
"Pause","-----","Unpau","-----","-----","-----",
"Pause","-----","Unpau","-----","-----","-----",
"Pause","-----","Unpau","-----","-----","-----",
"Pause","-----","Unpau","-----","-----","-----",
"A/B/START (F1): start game\n" \
"Unpau = Unpause\n" \
"Paddle up/down/left/right: move"
},
{ // 144. Missile War
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Left", "-----","Right","-----","-----","-----",
"Left", "-----","Right","-----","-----","-----",
"Left", "-----","Right","-----","-----","-----",
"Left", "-----","Right","-----","-----","-----",
"START (F1): start game\n" \
"Left/Right = fire missile from Left/Right silo\n" \
"Paddle directions: move cursor"
},
{ // 145. Red Clash
        "-----",                "Fire",
"-----","-----","-----","-----","-----","-----",
"Pause","-----","Unpau","-----","-----","-----",
"Pause","-----","Unpau","-----","Fire", "-----",
"Pause","-----","Unpau","-----","Fire", "-----",
"Pause","-----","Unpau","-----","Fire", "-----",
"START (F1): start game\n" \
"A (F2): start demo\n" \
"Unpau = Unpause\n" \
"Paddle directions: move"
},
{ // 146. Route 16
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","Pause","Lives","Unpau",
"-----","-----","-----","Pause","-----","Unpau",
"A (F2): cycle through difficulties (1..4)\n" \
"B (F3): cycle through Play/Trainer/Demo\n" \
"Lives: change number of Lives (2..5)\n" \
"START (F1): start game\n" \
"Unpau = Unpause\n" \
"Paddle directions: move\n" \
"Top row of screen is (left to right): score, high score, lives, fuel."
},
{ // 147. 06-2-PVI Programming, 07-C-PVI Programming
        "WCAS",                 "E",
"-----","-----","-----","-----","-----","-----",
"RCAS", "-----","C",    "D",    "-----","F",
"BP",   "REG",  "8",    "9",    "A",    "B",
"PC",   "MEM",  "4",    "5",    "6",    "7",
"-",    "+",    "0",    "1",    "2",    "3",
"The default mode is \"sprite editor\" (1) for ESS-006-2 and " \
"\"background editor\" (9) for ESS-007-C.\n" \
"UC (F2): change active sprite (1..4) (or switch to mode 9 (\"background editor\")\n" \
"For all modes except \"background editor\" mode:\n" \
"LC (F3): switch to \"memory scan\" mode\n" \
"For \"sprite editor\" mode:\n" \
"RCAS/WCAS/BP/REG/PC/MEM = select appropriate sprite zone\n" \
"For RCAS, WCAS, BP and REG zones:\n" \
"0..F = set appropriate pixel (or clear if - key is held)\n" \
"For PC and MEM zones:\n" \
"0/1 = change sprite colour/size\n" \
"2 = turn background on\n" \
"3/- = turn on/off sprite # shown at top right, and increment number\n" \
"4/5 = move all/duplicate sprites right (or left if - key is held)\n" \
"6/7 = move all/duplicate sprites down (or up if - key is held)\n" \
"(Sprite movement is slow for PC zone, fast for MEM zone.)\n" \
"8..F = set appropriate pixel (or clear if - key is held)\n" \
"For \"background editor\" mode:\n" \
"-/+/PC/MEM together with WCAS/REG = draw vertical bars\n" \
"BP = turn sound on\n" \
"While sound is on:\n" \
"-/+ = adjust sound pitch down/up\n" \
"any other key = turn sound off\n" \
"RCAS = switch to \"colourization\" mode\n" \
"WCAS/REG = select previous/next row\n" \
"0/1/4/5/8/9/C/D = adjust width of row contents\n" \
"E/F = select left/right side of 1st subrow (thin)\n" \
"A/B = select left/right side of 2nd subrow (thick)\n" \
"6/7 = select left/right side of 3rd subrow (thin)\n" \
"2/3 = select left/right side of 4th subrow (thick)\n" \
"When a subrow has been selected, the cursor moves left..right:\n" \
"-/+ = clear/set this column of this subrow\n" \
"PC/MEM = clear/set this side of this subrow\n" \
"For \"colourization\" mode:\n" \
"-/+ = change foreground/background colour\n" \
"For \"memory scan\" and \"character set viewer\" modes:\n" \
"PC = switch to \"relative address calculation\" mode\n" \
"For \"memory scan\" mode:\n" \
"+ = scroll down by one screenful\n" \
"RCAS = switch to \"character set viewer\" mode\n" \
"For \"character set viewer\" mode:\n" \
"RCAS = scroll down by one screenful\n" \
"+ = return to \"memory scan\" mode\n" \
"For \"relative address calculation\" mode:\n" \
"Enter source address, press +, enter destination address, press +.\n" \
"This is a rather complicated program, intended as an aid when developing your own games. It provides the possibility of programming objects and background shapes on the screen, so that the results can be judged more easily. A 'relative address' calculation routine is also included.\n" \
"This program has proved extremely useful in practice - once you get used to operating the keys!"
},
{ // 148. 03-1-Play with the PVI, Find the Angle, Space Scene
        "Right",                "Spr0X",
"-----","-----","-----","-----","-----","-----",
"Left" ,"-----","Horiz","Spr0" ,"-----","Spr0Y",
"BGCOL","SCOR1","SPR01","Spr1" ,"Spr1X","Spr1Y",
"PITCH","SCOR2","SPR23","Spr2" ,"Spr2X","Spr2Y",
"Dec"  ,"Inc"  ,"SIZES","Spr3" ,"Spr3X","Spr3Y",
"Left = enter left vertical grid editing mode (or advance address by 2 if already in that mode)\n" \
"Right = enter right vertical grid editing mode (or advance address by 2 if already in that mode)\n" \
"Horiz = enter horizontal grid editing mode (or advance address by 1 if already in that mode)\n" \
"Spr# = enter sprite # imagery editing mode (or advance address by 1 if already in that mode)\n" \
"Spr#X/Y = enter sprite # X/Y-coord editing mode (or switch between original/duplicate if already in that mode)\n" \
"BGCOL or P1 paddle right = enter BGCOLOUR editing mode\n" \
"SCOR1 = enter SCORELT editing mode (or SCORERT if already in SCORELT)\n" \
"PITCH = enter PITCH editing mode\n" \
"SCOR2 or P1 paddle up/down or P2 paddle up/down/left/right = enter SCORECTRL editing mode\n" \
"SPR01 = enter SPR01COLOURS editing mode\n" \
"SPR23 = enter SPR23COLOURS editing mode\n" \
"SIZES = enter SIZES editing mode\n" \
"Dec = decrement chosen value\n" \
"Inc or P1 paddle left = increment chosen value"
},
{ // 149. Blank (eg. 03-2-Clock, Table A/B, Hamish)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
""
},
{ // 150. 03-6-Locomotive
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Start","-----","-----","-----","-----","-----",
""
},
{ // 151. 06-1-Picture Pattern
        "Inver",                "E",
"-----","-----","-----","-----","-----","-----",
"Color","-----","C",    "D",    "-----","F",
"Incrs","Decrs","8",    "9",    "A",    "B",
"Narro","Widen","4",    "5",    "6",    "7",
"Left", "Right","0",    "1",    "2",    "3",
"0..9, A..F = choose picture\n" \
"Inver = Invert foreground/background colours (pictures 1..C)\n" \
"Color = adjust Colours (pictures 1..C and F)\n" \
"Incrs/Decrs = Increase/Decrease distance between vertical bars (picture 3)\n" \
"Narro/Widen = Narrow/Widen vertical bars (pictures 2 and 3)\n" \
"Left/Right = move vertical bars Left/Right (pictures 2, 3 and F)\n" \
"Incrs/Narro = move cyan/red bars away from each other (picture F)\n" \
"Decrs/Widen = move cyan/red bars towards each other (picture F)\n" \
"This is exactly what its name implies: a whole series of test patterns for a (colour) TV set. As such, it has proved extremely useful on numerous occasions..."
},
{ // 152. 07-A-Shapes
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Freez","-----","-----","-----","-----","-----",
"Pause","-----","-----","-----","-----","-----",
"Freez = Freeze (permanently)\n" \
"Pause = Pause (while held down)\n" \
"This has proved quite popular with the younger generation...Twenty-five different shapes appear in the 'garage', and then move out to the right. The object is to guess what they represent.\n" \
"We won't spoil your fun by telling you! The data for the shapes is stored in ten-byte groups from $A00 on."
},
{ // 153. 07-D-Disassembler
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"UC (F2): stop\n" \
"START (F1): restart\n" \
"The object here is quite simple: to make the decoding of an existing program much less laborious! The program to be examined is split up into one-, two- or three-byte instructions, and displayed accordingly.\n" \
"If should be noted that this program is stored from $8C0 to $8F6 and from $1F80 to $1FAD."
},
{ // 154. 07-F-Lotto
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Clear","-----","-----","-----","-----","-----",
"Draw", "Start","Quit", "-----","-----","-----",
"For main menu:\n" \
"Start = Start (go to draw screen)\n" \
"For draw (\"Ziehung\") screen:\n" \
"Draw = Draw next number and go to grid screen\n" \
"For grid screen:\n" \
"Start = return to draw screen (if 1..5 numbers drawn) " \
"or sort numbers (if all 6 numbers drawn)\n" \
"Quit = Quit\n" \
"Clear = Clear all numbers and go to draw screen\n" \
"Eg. To generate 6 sorted numbers:\n" \
"St, Dr, St, Dr, St, Dr, St, Dr, St, Dr, St, Dr, St\n" \
"This is intended primarily for our readers in West Germany. There, a game is played on the national TV networks that is similar to Bingo. You fill in six random numbers from 1 to 49 on a card, and send it in. At the end of the week, six numbers are 'drawn' in deathly silence - with umpteen million viewers holding their breath...If your numbers come up, you win.\n" \
"The only mental work involved is in trying to think up six numbers every week. The obvious way to avoid this is to get your home computer to do the job for you..."
},
{ // 155. 10-D-Editor
        "-----",                "E",
"-----","-----","-----","-----","-----","-----",
"-----","-----","C",    "D",    "-----", "F",
"BP",   "-----","8",    "9",    "A",     "B",
"PC",   "-----","4",    "5",    "6",     "7",
"-",    "+",    "0",    "1",    "2",     "3",
"1 = Load/Clear mode:\n" \
"Enter start address, press +, enter end address, press +, enter fill " \
"value (optionally), press +\n" \
"2 = Block transfer mode:\n" \
"Enter source start address, press +, enter source end address, press +, " \
"enter destination start address (optionally), press +, enter destination " \
"end address (if destination start address was omitted), press +. Press " \
"RESET to cancel, or wait for block transfer to occur.\n" \
"3 = Disassembler:\n" \
"Enter start address, press +. During disassembly, " \
"3/7/B/F/2/6/A/E = adjust speed (0 = stopped..E = fastest), " \
"- = data mode, + = instruction mode.\n" \
"4 = Find address mode:\n" \
"Enter desired address, press +, enter start address, press +. " \
"During data display, " \
"3/7/B/F/2/6/A/E = adjust speed (0 = stopped..E = fastest), " \
"- = data mode, + = instruction mode.\n" \
"5 = Find block mode:\n" \
"Enter start address of block, press +, enter end address of block, " \
"press +, enter start address to search from.\n" \
"6 = Doubled WCAS routine:\n" \
"Enter start address of 1st block, press +, enter end address of 1st " \
"block, press +, enter start address of 2nd block, press +, enter end " \
"address of 2nd block, press +, enter start address, press +, enter file " \
"number, press +.\n" \
"PC = return to monitor\n" \
"BP = return to editor\n\n" \
"This program consists of six independent routines that have proved useful when developing or modifying software.\n" \
"3: Disassembler. Enter the start address of the program to be examined."
},
{ // 156. 10-1x-Change Words for Hangman (aka "Hangman word entry")
        "C",                    "F",
"-----","-----","-----","-----","-----","-----",
"B",    "-----", "D",  "E",     "-----", "G",
"I",    "J",     "K",  "L",     "M",     "N",
"O",    "P",     "Q",  "R",     "S",     "T",
"U",    "V",     "W",  "X",     "Y",     "Z",
"UC (F2): A\n" \
"START (F1): H\n" \
"A..Z = enter relevant letter\n" \
"LC+B = Ä\n" \
"LC+C = Ö\n" \
"LC+D = Ü\n" \
"LC+I = run word list towards start\n" \
"LC+J = run word list towards end\n" \
"LC+K = illustrate the entire word\n" \
"LC+O = a word backwards\n" \
"LC+P = a word forwards\n" \
"LC+Q = delete the entire word\n" \
"LC+U = previous letter\n" \
"LC+V = next letter\n" \
"LC+W = delete this letter\n\n" \
"This program is loaded immediately after file ESS-010-E or ESS-010-F, and is used to compile a new vocabulary for 'Hangman'."
},
{ // 157. Table C
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","Up",   "-----",
"-----","-----","-----","Left", "-----","Right",
"-----","-----","-----","-----","Down", "-----",
"Up/Down/Left/Right = move Up/Down/Left/Right"
},
{ // 158. Table D/E
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","Fire",
"-----","-----","-----","-----","Up",   "-----",
"-----","-----","-----","Left", "-----","Right",
"-----","-----","-----","-----","Down", "-----",
"Up/Down/Left/Right = move Up/Down/Left/Right"
},
{ // 159. Table 20
        "Quit",                 "Nett",
"-----","-----","-----","-----","-----","-----",
"Quit", "----", "Nice", "Aardi","-----","Genti",
"Quit", "Quit", "Fun",  "Lol",  "Spass","Rigol",
"Quit", "Quit", "End",  "Einde","Ende", "Fin",
"Quit", "Quit", "Start","Begin","Anfan","Debut",
"An = Anfang\n" \
"Ri = Rigolade\n" \
"Aa = Aardig\n" \
"Ge = Gentile\n" \
"Quit = Quit to monitor\n\n" \
"C/8/4/0 column: English\n" \
"D/9/5/1 column: Dutch?\n" \
"E/A/6/2 column: German\n" \
"F/B/7/3 column: Italian?"
},
{ // 160. Astro Wars
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"F1: 1UP\n" \
"F2: 2UP\n" \
"F3: Coin A\n" \
"F4: Coin B\n" \
"F5: Reset\n" \
"Fire (space bar) = Fire bullet\n" \
"Paddle left/right: move ship\n\n" \
"Official instructions:\n" \
"* Insert coin(s).\n" \
"* Select one or two player game.\n" \
"* If two player game is selected, players play alternately.\n" \
"* Operate the control buttons to move your ship to the right or to the left.\n" \
"* Four independent groups will appear before the Master appears.\n" \
"* Watch your fuel supply.\n" \
"* You must destroy the Master before you can refuel.\n" \
"* If you lose all your ships the game is over."
},
{ // 161. Malzak 1
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"F1: 1UP\n" \
"F2: 2UP\n" \
"F3: Coin\n" \
"F5: Reset\n" \
"Fire (space bar) = Fire bullet/drop bomb\n" \
"Paddle directions: move ship"
},
{ // 162. PICTURE 1.1
        "Color",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","Inver","-----","-----","-----",
"-----","Color","Inver","-----","-----","-----",
"-----","Color","Inver","-----","-----","-----",
"-----","Color","Inver","-----","-----","-----",
"Color = toggle sprite Colours\n" \
"Inver = toggle Inverse video"
},
{ // 163. Vertical Scroll
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Up",   "-----","-----","-----","-----",
"-----","Down", "-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Up/Down = scroll screen"
},
{ // 164. Multiplex
        "Inver",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Inver = Invert colours\n"\
"Paddle directions: move car"
},
{ // 165. Sound 3.0
        "$06",                  "$02",
"$1C",  "$14",  "$0C",  "$18",  "$10",  "$08",
"$07",  "$04",  "$05",  "$03",  "$00",  "$01",
"$0F",  "$0E",  "$0D",  "$0B",  "$0A",  "$09",
"$17",  "$16",  "$15",  "$13",  "$12",  "$11",
"$1F",  "$1E",  "$1D",  "$1B",  "$1A",  "$19",
"START (F1): change volume\n"\
"A (F2): change octave\n"\
"B (F3): cycle through Tone/Noise/Both/Quiet\n"\
"$xx = play note $xx+octave"
},
{ // 166. Tester 1 and 2 (Arcadia)
        "Next",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Next", "-----","-----","-----","-----",
"-----","Next", "-----","-----","-----","-----",
"-----","Next", "-----","-----","-----","-----",
"Next = advance to Next screen"
},
{ // 167. Tester 1 (Elektor)
        "Next",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Up",   "-----","-----","-----","-----",
"Left", "Down", "Right","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Next = advance to Next screen (on pages 1 and 2) or toggle between imagery and coordinate views (on page 3)\n"\
"Up/Down/Left/Right = adjust duplicate sprite positions (on page 3)"
},
{ // 168. Tester 2 (Elektor)
        "Next",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Next = advance to Next screen"
},
{ /* 169. 4 Sprites, 4 UDCs, Animate, Boot Initialization, Character Show,
Colours, Horizontal Scroll, Music, Show Text, 8 Sprites, Target 1 and 2 */
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
""
},
{ // 170. BoxBoy (beta version)
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Fire = advance to next screen or get/drop box\n"\
"Paddle directions: move boy"
},
{ // 171. Read Paddles
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Paddle directions: adjust coordinates"
},
{ // 172. Parashooter
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): start game\n" \
"Paddle left/right: move left/right"
},
{ // 173. Malzak 2
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"F1: 1UP\n" \
"F2: 2UP\n" \
"F3: Coin\n" \
"F4: Change DIP switch\n" \
"F5: Reset\n" \
"Fire (space bar) = Fire bullet/drop bomb\n" \
"Paddle directions: move ship"
},
{ // 174. Penalty (Elfmeter)
        "JmpUp",                "-----",
"-----","-----","-----","-----","-----","-----",
"JmpLt","-----","JmpRt","-----","-----","-----",
"Up-Lt","Up"   ,"Up-Rt","-----","-----","-----",
"Left" ,"-----","Right","-----","-----","-----",
"Dn-Lt","-----","Dn-Rt","-----","-----","-----",
"UC (F2): choose game variant (1..4):\n" \
"1: 1 player (red vs. computer), slow?\n" \
"2: 2 players (red vs. cyan), slow?\n" \
"3: 1 player (red vs. computer), fast?\n" \
"4: 2 players (red vs. cyan), fast?\n" \
"START (F1): start game\n" \
"Red player is left. Cyan player is right.\n" \
"Left number is number of balls kicked.\n" \
"Right number is number of goals scored.\n" \
"For attacker:\n" \
"Paddle up/left/right: aim ball\n" \
"Paddle down: kick ball\n" \
"For defender:\n" \
"Keypad controls movement.\n" \
"JmpLt/JmpUp/JmpRt = Jump Up/Left/Right\n" \
"Up-Lt/Up-Rt/Dn-Lt/Dn-Rt = Up/Down-Left/Right"
},
{ // 175. Moonlanding for 1 (Mondflug)
        "Thrus",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"For level 1 (launchpad):\n" \
"Thrus = Thrust (launch rocket) (immediately when countdown reaches 0)\n" \
"For levels 2 and 4(space):\n" \
"Paddle directions: steer rocket\n" \
"For level 3 (moon):\n" \
"Thrus = fire Thrusters\n" \
"Paddle left/right: move astronaut left/right"
},
{ // 176. Moonlanding for 2 (Mondflug)
        "Thrus",                "8th",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","1st",  "-----","15th",
"-----","-----","-----","2nd",  "9th",  "16th",
"-----","-----","-----","3rd",  "10th", "17th",
"-----","-----","-----","4th",  "11th", "18th",
"START (F1): start game\n" \
"For level 1 (launchpad):\n" \
"Thrus = Thrust (launch rocket) (immediately when countdown reaches 0)\n" \
"For levels 2 and 4 (space):\n" \
"Left player:\n" \
"Paddle directions: steer rocket\n" \
"Right player:\n" \
"1st..18th: accelerate nth UFO (while held down)\n" \
"For level 3 (moon):\n" \
"Left player:\n" \
"Thrus = fire Thrusters\n" \
"Paddle left/right: move astronaut left/right\n" \
"Right player:\n" \
"1st..18th: aim meteors (1st is extreme left, 18th is extreme right)"
},
{ // 177. Car Race (Elektor)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): start game\n" \
"The paddle must be approximately centred at the time you press START (otherwise you will instantly die).\n" \
"Paddle left/right: steer car left/right\n" \
"Paddle backwards: accelerate\n" \
"Paddle forwards: brake\n" \
"SELECT (F2): return to title screen from \"ZIEL\" screen\n" \
"Left digits show number of collisions. Right digits show distance covered.\n" \
"Once the other cars have left you behind alone on-screen, it is impossible to catch them."
},
{ // 178. Disassembler (Elektor) (Peter Marschat version)
        "-----",                "E",
"-----","-----","-----","-----","-----","-----",
"-----","-----","C",    "D",    "-----","F",
"-----","-----","8",    "9",    "A",    "B",
"-----","-----","4",    "5",    "6",    "7",
"-----","Go",   "0",    "1",    "2",    "3",
"Type starting address and press Go.\n" \
"UC (F2): pause\n" \
"START (F1): unpause"
},
{ // 179. Ticker
        "Pause",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Pause = Pause scrolling (while held down)"
},
{ // 180. Sound and Colour
        "White",                "White",
"-----","-----","-----","-----","-----","-----",
"Cyan", "-----","-----","Cyan", "-----","-----",
"Green","Yello","-----","Green","Yello","-----",
"Blue", "Purpl","-----","Blue", "Purpl","-----",
"Black","Red",  "-----","Black","Red",  "-----",
"UC (F2): select number of players (1..6)\n" \
"START (F1): go\n" \
"Yello = Yellow\n" \
"Purpl = Purple"
},
{ // 181. Assembler (Elektor)
        "WCAS",                 "E",
"-----","-----","-----","-----","-----","-----",
"RCAS", "-----","C",    "D",    "-----","F",
"BP",   "REG",  "8",    "9",    "A",    "B",
"PC",   "MEM",  "4",    "5",    "6",    "7",
"-",    "+"    ,"0",    "1",    "2",    "3",
"Type starting address and press +.\n" \
"START (F1): skip to next instruction\n" \
"UC (F2): edit this instruction\n" \
"Now, choose the instruction family:\n" \
"RCAS: \"Arithm.\" (Arithmetic and transfer) (red+yellow)\n" \
"BP: \"IO+PS\" (I/O and mixed-status) (blue)\n" \
"PC: \"Branch\" (green)\n" \
"(These colours match the ones in the \"Help|Opcodes...\" window.)\n" \
"Now, choose the instruction subfamily:\n" \
"Arithmetic mode: E=IOR, F=COM, A=AND, B=STR, 6=EOR, 7=SUB, 2=LOD, 3=ADD\n" \
"IO+PS mode: E=TMI, F=PS, A=DAR, B=RED, 6=RRR, 7=WRT, 2=RRL, 3=RET\n" \
"Branch mode: E=BSN, F=BDR, A=BRN, B=BIR, 6=BST, 7=BSF, 2=BCT, 3=BCF\n" \
"For RET, now choose 0/8 for RETC/RETE.\n" \
"For RED/WRT, now choose 0/4/8 for Control/Data/Extended.\n" \
"For PS, now choose 1/5/9/C/D for CPS/PPS/SPS/TPS/LPS, and then WCAS/REG for PSU/PSL.\n" \
"Now, choose the addressing mode:\n" \
"C = absolute\n" \
"8 = relative\n" \
"4 = immediate\n" \
"0 = register\n" \
"Now, choose the register (1/5/9/D for r0/r1/r2/r3).\n" \
"Now, choose the condition code:\n" \
"D = un (always)\n" \
"9 = lt (less than)\n" \
"5 = gt (greater than)\n" \
"1 = eq (equal)\n" \
"Now, choose:the addressing mode:\n" \
"PC = indirect addressing\n" \
"- = direct addressing\n" \
"Now, choose the indexing mode:\n" \
"WCAS = indexing\n" \
"REG = indexing with decrement\n" \
"MEM = indexing with increment\n" \
"+ = no indexing\n" \
"Now, type the data bytes or address.\n" \
"Not all instructions require all these fields.\n" \
"At any time:\n" \
"START (F1): skip to next instruction\n" \
"UC (F2): restart editing of this instruction\n" \
"LC (F3): backspace\n" \
"For HALT, use ANDZ,r0.\n" \
"For NOP, use STRZ,r0.\n" \
"For ZBRR, use BCFR,un.\n" \
"For ZBRA, use BCFA,un.\n" \
"For ZBSR, use BSFR,un.\n" \
"For ZBSA, use BSFA,un."
},
{ // 182. Single Step
        "-----",                "E",
"-----","-----","-----","-----","-----","-----",
"-----","-----","C",    "D",    "-----","F",
"-----","-----","8",    "9",    "A",    "B",
"-----","Inter","4",    "5",    "6",    "7",
"Left", "Right","0",    "1",    "2",    "3",
"Pressing START (F1): run program while watching CPU\n" \
"Holding START (F1): run program while watching PVI (ie. output)\n" \
"UC (F2): step (execute) this instruction\n" \
"Inter: run Interrupt\n" \
"While paused (eg. after pressing UC):\n" \
"Left/Right: move cursor left/right\n" \
"0..F: change value under cursor"
},
{ // 183. default (PHUNSY)
        "4",                    "D->M",
"-----","-----","-----","-----","-----","-----",
"0",    "-----","8",    "D->M", "-----","Reset",
"1",    "5",    "9",    "M->D", "M->D", "Halt",
"2",    "6",    "A",    "GoTo", "GoTo", "DCass",
"3",    "7",    "B",    "Clear","Clear","LCass",
""
},
{ // 184. Musical Computer (Elektor)
        "low-A",                "mid-E",
"-----","-----","-----","-----","-----","-----",
"low-G","-----","mid-C","mid-D","-----","mid-G",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
""
},
{ // 185. Enterprise 2 (Elektor)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Paddle directions: move reticle"
},
{ // 186. Enterprise 3
        "Forwa",                "Pause",
"-----","-----","-----","-----","-----","-----",
"Slow" ,"-----","-----","Slow", "-----","Resta",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Forwa = Move forwards while held down\n" \
"Resta = Restart\n" \
"Time is shown on left, score is shown on right."
},
{ // 187. Knock Out Objects
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Paddle directions = move\n\n" \
"Touch all the objects in order."
},
{ // 188. Chicken Crossing the Road
        "Move",                 "Heal2",
"-----","-----","-----","-----","-----","-----",
"Pain", "-----","-----","Heal1","-----","Clear",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Move: Move (while held down)\n" \
"Heal1: Restart chicken (at any time)\n" \
"Heal2: Restart chicken (only while alive)\n" \
"Clear: Clear digits\n" \
"If Move or Pain are held down while your corpse is being crushed, this will increase enemy score.\n" \
"Left digit group are number of road-crossing attempts made so far.\n" \
"Right digit group are score of enemy cars."
},
{ // 189. Enterprise 2 (Elektor)
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Paddle directions: move reticle"
},
{ // 190. Example 2 aka Hamish
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Paddle directions: move dog/ship\n" \
"The default is to move up and right. Hold down and/or left as appropriate."
},
{ // 191. Enterprise 4
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Paddle directions: move ship"
},
{ // 192. Christmas Tree with Stars (28-3)
        "Large",                "-----",
"-----","-----","-----","-----","-----","-----",
"Tree", "-----","Small","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Tree = make Tree appear\n" \
"Large = make Large red stars appear (or subsequently, turn large stars red)\n" \
"Small = make Small stars appear (or subsequently, turn large stars cyan)"
},
{ // 193. 2 Pushbuttons
        "-----",                "Imag1",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","Imag2",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Imag1 = show 1st Image\n" \
"Imag2 = show 2nd Image (while held down) (if 1st image is not already displayed)"
},
{ // 194. Digital Clock
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Slow", "Fast", "-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Slow = advance clock Slowly (while held down)\n" \
"Fast = advance clock Fast (while held down)\n" \
"Time is shown as HH MM and does not advance until after Slow or Fast buttons have been used."
},
{ // 195. AY-3-8500/8550
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Up",   "-----","-----","Up",   "-----",
"-----","Dn",   "-----","-----","Dn",   "-----",
"F2: select game variant\n" \
"F4: toggle short/tall bats\n" \
"F1: start game\n" \
"F3: serve\n" \
"Paddle directions: move primary bat(s)/reticle\n" \
"Up/Dn: move secondary bat(s) (in 3/4-player mode)\n" \
"Fire: shoot rifle"
},
{ // 196. Noise Explosion
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Fire: make noise (while held down)"
},
{ // 197. Laser Battle & Lazarian
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","FirUp","-----","-----","-----","-----",
"FirLt","FirDn","FirRt","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"F1: 1UP\n" \
"F2: 2UP\n" \
"F3: Coin A\n" \
"F4: Coin B\n" \
"F5: Reset\n" \
"FirUp/FirDn/FirLt/FirRt = fire in that direction\n" \
"Paddle directions: move ship\n\n" \
"Official instructions:\n" \
"* Insert coin.\n" \
"* To start game: press 1 or 2 player button.\n" \
"* To move ship: move control stick.\n" \
"* Fire buttons control laser direction.\n" \
"* Phase I: Shoot out the yellow areas to free meteors, move into red centre zone and destroy meteors. Avoid missiles and collisions.\n" \
"* Phase II: Move ship through corridors avoiding obstacles & attacks to dock with sister ship.\n" \
"* Phase III: Cut away monster with upper laser to release eye. Drop back and to side to avoid missile attacks. Four hits are required to destroy free eye."
},
{ // 198. Leapfrog
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"On title screen:\n" \
"SELECT (F2): go to player selection screen\n" \
"Otherwise, demo will commence.\n" \
"On player selection screen:\n" \
"SELECT (F2): toggle 1/2 players\n" \
"START (F1): start game\n" \
"Otherwise, game will reset to title screen.\n" \
"During game:\n" \
"Left paddle: move (for either player)"
},
{ // 199. 07-5-Space Shoot-Out
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","C",    "D",    "E",    "F",
"-----","Up",   "8",    "9",    "A",    "B",
"Lt",   "Fire", "4/Rt", "5",    "6",    "7",
"-----","OK/Dn","0",    "1",    "2",    "3",
"UC (F2): don't press this key!\n" \
"LC (F3): go to options screen\n" \
"On options screen:\n" \
"0..F: enter time limit (2 digits)\n" \
"OK when done\n" \
"Then:\n" \
"1..3: enter difficulty level (1 digit)\n" \
"OK when done\n" \
"In game:\n" \
"Up/Dn/Lt/Rt, or paddle directions: move crosshair Up/Down/Left/Right\n\n" \
"You cannot fire while moving the crosshair.\n" \
"This is an updated version of a game that is used as an example in the book. The object is, quite simply, to shoot down the rocket before your time runs out.\n" \
"It is now an easy matter to modify the time limit and the 'proficiency level': the accuracy required to shoot down the rocket can be varied in three steps from 'beginner's luck' to 'pinpoint'."
},
{ // 200. COWBOY
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"Duck", "-----","-----","Duck", "-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant (1..64)\n" \
"START (F1): start game\n" \
"Duck: duck (only when enemy bullet is present)\n" \
"Paddle directions: move man\n" \
"Paddle up/down: guide bullet (not in all variants)\n" \
"Neither player can fire for a second or two after one of them has regenerated."
},
{ // 201. KNOCKOUT
        "Serve",                "Serve",
"-----","-----","-----","-----","-----","-----",
"-----","-----","Catch","-----","-----","Catch",
"-----","Serve","Catch","-----","Serve","Catch",
"-----","Serve","Catch","-----","Serve","Catch",
"-----","Serve","Catch","-----","Serve","Catch",
"SELECT (F2): choose game variant (1..128)\n" \
"START (F1): start game\n" \
"Catch: Catch ball (while held down) (not in all variants)\n" \
"Paddle directions: move bat (and ball, in certain variants)"
},
{ // 202. default (BINBUG)
        "D",                    "-----",
"-----","-----","-----","-----","-----","-----",
"C",    "-----","E",    "F",    "-----","-----",
"8",    "9",    "A",    "B",    "-----","-----",
"4",    "5",    "6",    "7",    "-----","-----",
"0",    "1",    "2",    "3",    "-----","-----",
""
},
{ // 203. default (Selbstbaucomputer)
        "D",                    "CMD",
"-----","-----","-----","-----","-----","-----",
"C",    "-----","E",    "F",    "-----","FLAG",
"8",    "9",    "A",    "B",    "RUN",  "MON",
"4",    "5",    "6",    "7",    "GOTO", "PC",
"0",    "1",    "2",    "3",    "RST",  "NXT",
""
},
{ // 204. Reaktionstest (Selbstbaucomputer)
        "D",                    "-----",
"-----","-----","-----","-----","-----","-----",
"C",    "-----","E",    "F",    "-----","FLAG",
"8",    "9",    "A",    "B",    "-----","-----",
"4",    "5",    "6",    "7",    "-----","-----",
"0",    "1",    "2",    "3",    "RST",  "-----",
"FLAG: start\n" \
"0..F: respond to prompt\n" \
"RST: prepare for next try"
},
{ // 205. Testprogramm (Selbstbaucomputer)
        "D",                    "CR",
"-----","-----","-----","-----","-----","-----",
"C",    "-----","E",    "F",    "-----","LF",
"8",    "9",    "A",    "B",    "Clear","Right",
"4",    "5",    "6",    "7",    "Left", "Up",
"0",    "1",    "2",    "3",    "RST",  "Home",
"0..F: print number/letter\n" \
"CR (CMD): carriage return\n" \
"LF (FLAG): linefeed (cursor down)\n" \
"Clear (RUN): clear screen\n" \
"Home (NXT): cursor home\n" \
"Left (GOTO): cursor left\n" \
"Right (MON): cursor right\n" \
"Up (PC): cursor up\n" \
"RST: reset"
},
{ // 206. AY-3-8600
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"F2: select game variant\n" \
"F4: select bat size\n" \
"F1: start game\n" \
"Paddle directions: move bat(s)/reticle\n" \
"Fire: serve/shoot (some variants need both players to press serve)"
},
{ // 207. PVI Art
        "WCAS",                 "E",
"-----","-----","-----","-----","-----","-----",
"RCAS", "-----","C",    "D",    "-----","F",
"BP",   "REG",  "8",    "9",    "A",    "B",
"PC",   "MEM",  "4",    "5",    "6",    "7",
"+",    "-",    "0",    "1",    "2",    "3",
"RCAS/WCAS = left/right side vertical bars of background ($1F80..$1FA7)\n" \
"C = horizontal bar extensions of background (HORIZ1..HORIZ5)\n" \
"BP = background and screen colour (BGCOLOUR)\n" \
"REG = score digits (SCORELT/SCORERT)\n" \
"8 = colour of objects 1 and 2 (SPR01COLOURS)\n" \
"PC = sound frequency (PITCH)\n"
"MEM = score format (SCORECTRL)\n" \
"4 = colour of objects 3 and 4 (SPR23COLOURS)\n" \
"0 = size of four objects (SIZES)\n" \
"D..F = shape, horizontal and vertical coord of object 1 ($1F00..$1F0C)\n" \
"9..B = shape, horizontal and vertical coord of object 2 ($1F10..$1F1C)\n" \
"5..7 = shape, horizontal and vertical coord of object 3 ($1F20..$1F2C)\n" \
"1..3 = shape, horizontal and vertical coord of object 4 ($1F40..$1F4C)\n" \
"-/+ = decrease/increase selected value\n" \
"Left paddle left/right: horizontal coordinate of object 1 (SPRITE0AX)\n" \
"Left paddle up/down: vertical coordinate of object 1 (SPRITE0BY)\n" \
"Right paddle left/right: horizontal coordinate of duplicate of object 1 (SPRITE0BX)\n" \
"Right paddle up/down: separation of duplicates of object 1 (SPRITE0BY)"
},
{ // 208. ESS-03-4, ESS-07-6: 4 In a Row (7*6)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Move" ,"-----","Drop" ,"Move", "-----","Drop",
"UC: choose game variant (1-3):\n" \
"1: green (left human) vs. red (right human) with green starting.\n" \
"2: red (right human) vs. green (computer) with red starting.\n" \
"3: red (right human) vs. green (computer) with green starting.\n" \
"START (F1): start game\n" \
"Move = Move cursor right\n" \
"Drop = Drop token\n" \
"This is a fairly well-known game. The object is to get four of your squares in a row (horizontal, vertical or diagonal).\n" \
"As those who have already tried this program will know, the computer is an infuriatingly skilled opponent."
},
{ // 209. default (MIKIT 2650)
        "Read",                 "E",
"-----","-----","-----","-----","-----","-----",
"BLANK","-----","C",    "D",    "-----","F",
"+",    "Go",   "8",    "9",    "A",    "B",
"Halt", "Punch","4",    "5",    "6",    "7",
"Load", "Store","0",    "1",    "2",    "3",
""
},
{ // 210. Program 7 (Ein- und Als-Schaltbare) (MIKIT 2650)
        "Flash",                "Light",
"-----","-----","-----","-----","-----","-----",
"Flash","-----","Light","Flash","-----","Flash",
"Light","Light","Flash","Light","Light","Flash",
"Flash","Flash","Flash","Light","Flash","Light",
"Flash","Light","Flash","Light","Flash","Light",
"Light = Light lamp\n" \
"Flash = Flash lamp (while held)"
},
{ // 211. Program 8 (Elektronischer Würfel) (MIKIT 2650)
        "Roll",                 "Roll",
"-----","-----","-----","-----","-----","-----",
"Roll", "-----","Roll", "Roll", "-----","Roll",
"Roll", "Roll", "Roll", "Roll", "Roll", "Roll",
"Roll", "Roll", "Roll", "Roll", "Roll", "Roll",
"Roll", "Roll", "Roll", "Roll", "Roll", "Roll",
"Roll = Roll die"
},
{ // 212. Program 11 (Stopuhr) (MIKIT 2650)
        "Reset",                "Start",
"-----","-----","-----","-----","-----","-----",
"Reset","-----","Start","Reset","-----","Reset",
"Start","Start","Reset","Start","Start","Reset",
"Reset","Reset","Reset","Start","Reset","Start",
"Reset","Start","Reset","Start","Reset","Start",
"Start = Start\n" \
"Reset = Start/Reset"
},
{ // 213. Program 12 (Reaktionszeittest) (MIKIT 2650)
        "Stop",                 "Start",
"-----","-----","-----","-----","-----","-----",
"Stop", "-----","Start","Stop", "-----","Stop",
"Start","Start","Stop", "Start","Start","Stop",
"Stop", "Stop", "Stop", "Start","Stop", "Start",
"Stop", "Start","Stop", "Start","Stop", "Start",
"Start = Start\n" \
"Stop = Start/Stop"
},
{ // 214. Program 13 (Codiertes Schloß) (MIKIT 2650)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","1",    "2",    "3",
"1, 2, 3: press these quickly in succession"
},
{ // 215. Come-Frutas (Interton)
        "Fast",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): change game variant (1..8) (hold down until display changes)\n" \
"Default variant is 0 (same as 2).\n" \
"Until pressing START (F1), or after game over, you are in practice mode (movement and sound but no collisions).\n" \
"START (F1): start game\n" \
"Paddle directions: move\n" \
"Fast = Faster movement while held down\n" \
"(only in variants 3,4,7,8, and only while there is a blue ghost)\n" \
"Song plays at start of practice mode *or* game.\n" \
"Game over is indicated by a sequence of rapid beeps."
},
{ // 216. Aggressor
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Start","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Left", "Fire", "Right","-----","-----","-----",
"Start: start game\n" \
"Left/right: move ship"
},
{ // 217. Musik1-3
        "$3F"  ,                "$35",
"-----","-----","-----","-----","-----","-----",
"$43"  ,"-----","$3B"  ,"$38"  ,"-----","$32",
"$64"  ,"$5F"  ,"$59"  ,"$54"  ,"$4F"  ,"$4B",
"$97"  ,"$8E"  ,"$86"  ,"$7F"  ,"$77"  ,"$71",
"$E2"  ,"$DC"  ,"$CA"  ,"$BE"  ,"$B3"  ,"$A9",
"$xx: play tone at this pitch (ie. set PITCH to this value)"
},
{ // 218. Musik1-4
        "$17"  ,                "$11",
"-----","-----","-----","-----","-----","-----",
"$1A"  ,"-----","$16"  ,"$13"  ,"-----","$0F",
"$38"  ,"$2F"  ,"$2C"  ,"$27"  ,"$23"  ,"$1F",
"$77"  ,"$6A"  ,"$5F"  ,"$59"  ,"$50"  ,"$46",
"$D6"  ,"$BE"  ,"$B3"  ,"$A0"  ,"$8E"  ,"$86",
"$xx: play tone at this pitch (ie. set PITCH to this value)"
},
{ // 219. Musik1-3x
        "$0D4" ,                "$0B2",
"-----","-----","-----","-----","-----","-----",
"$0E0" ,"-----","$0C8" ,"$0BD" ,"-----","$0A8",
"$150" ,"$13D" ,"$12C" ,"$11B" ,"$10B" ,"$0FC",
"$1F8" ,"$1DC" ,"$1C1" ,"$1A8" ,"$190" ,"$179",
"$2F3" ,"$2C8" ,"$2A0" ,"$27B" ,"$257" ,"$235",
"$xxx: play tone at this pitch (ie. set PITCHB1 to this value)\n" \
"PITCHA1 and PITCHC1 are also set (to other values)."
},
{ // 220. Towers of Hanoi
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Monit","-----","-----","-----","-----","-----",
"-----","Resta","-----","1"    ,"2"    ,"3",
"Monit = return to monitor\n" \
"Resta = restart game (from monitor)\n" \
"1..3: choose starting stack\n" \
"START (F1): start game\n" \
"Each move is:\n" \
"1..3: choose source stack (number will flash), then\n" \
"1..3: choose destination stack"
},
{ // 221. Queen
        "-----",                "Left",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","Beep" ,"Down",
"-----","-----","-----","-----","-----","-----",
"Don't","-----","-----","-----","-----","-----",
"Don't = don't press this key!\n" \
"START (F1): fall down/left"
},
{ // 222. Basar
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Toggl","Start","Varia","-----","-----","-----",
"On title screen:\n" \
"Varia = choose game variant 1..8\n" \
"Start: start game\n" \
"In \"Wurf\" (Cast) mode:\n" \
"Toggl = change to \"Tausch\" mode\n" \
"Varia = change to dice mode\n" \
"In \"Tausch\" (Exchange) mode:\n" \
"Toggl = change to \"Wurf\" mode (if purple thing hasn't moved)\n" \
"Toggl = move purple thing \"backwards\" (if purple thing has moved)\n" \
"Varia = move purple thing \"forwards\"\n" \
"Start = done\n" \
"In dice mode:\n" \
"Start = roll dice and change to \"Kauf\" mode\n" \
"In \"Kauf\" (Bargain) mode:\n" \
"Toggl = change to \"Wurf\" mode (if cyan thing hasn't moved)\n" \
"Varia = move cyan thing \"forwards\" (right)\n" \
"Start = done"
},
{ // 223. Space Battle (Raumschlacht) (Elektor)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Left" ,"-----","Right","Left" ,"-----","Right",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"UC (F2): don't press this key!\n" \
"START (F1): start game\n" \
"Left player is blue, right player is green.\n" \
"Paddle directions: move ship\n" \
"Left/Right = shoot left/right"
},
{ // 224. Calculator (SI50)
        "-----",                "E",
"-----","-----","-----","-----","-----","-----",
"-"    ,"-----","C"    ,"D"    ,"-----","F"    ,
"-----","÷"    ,"8"    ,"9"    ,"A"    ,"B"    ,
"+"    ,"*"    ,"4"    ,"5"    ,"6"    ,"7"    ,
"-----","="    ,"0"    ,"1"    ,"2"    ,"3"    ,
"Enter first number (two decimal digits).\n" \
"Then press key for desired operation (+/-/*/÷).\n" \
"On the display, P means +, - means -, n means *, y means ÷.\n" \
"Then enter second number (two decimal digits).\n" \
"Then press = key (ENT/NXT).\n" \
"RST (F5): do another calculation.\n" \
"MON (F3): return to monitor.\n" \
"Hex digits (A..F) can be entered but should not be."
},
{ // 225. Crap Game (SI50)
        "Start",                "-----",
"-----","-----","-----","-----","-----","-----",
"Start","-----","-----","-----","-----","-----",
"Start","Start","8"    ,"9"    ,"-----","-----",
"Start","Start","4"    ,"5"    ,"6"    ,"7"    ,
"Start","Start","0"    ,"1"    ,"2"    ,"3"    ,
"At \"U BUY\":\n" \
"Enter starting money (1..99).\n" \
"Then press Start.\n" \
"At \"PLACE BET\":\n" \
"Enter bet (1..9).\n" \
"Then press any key except SENS/MON/INT/RST.\n" \
"SENS (F1): stop dice\n" \
"Rules are:\n" \
"First roll (\"PASS 1\" message):\n" \
"2,3,12: lose\n" \
"7,11: win\n" \
"Otherwise, do second roll (\"SHOOT XX\" message):\n" \
"7: lose\n" \
"equal to first roll: win\n" \
"Otherwise, do second roll again"
},
{ // 226. Train (SI50)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Press Ctrl+Shift+` or Amiga-~ to open the DIP switches subwindow.\n" \
"Then choose speed and what your train consists of via parallel I/O toggles:\n" \
"Bits 6..4 control speed (0=stopped, 1=fast..7=slow)\n" \
"Bits 3..0 control what your train consists of (ie. different graphics).\n" \
"Then press INT (F2) and then RST (F4) to start.\n" \
"While train is moving:\n" \
"SENS (F1): train moves backwards (right) while held, otherwise forwards (left)."
},
{ // 227. Beat the Odds (original) (SI50)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Press INT (F2). Flag lamp will light (press Ctrl+W or Amiga-W to see this).\n" \
"Press SENS (F1) to start rolling.\n" \
"Lamps will flicker.\n" \
"Press SENS (F1) to stop rolling.\n" \
"Result will be displayed as digits and lamps.\n" \
"If game not over, press INT (F2) and then SENS (F1) twice for another roll.\n" \
"If game over (\"YOU HOPE\"), press RST (F4)."
},
{ // 228. Beat the Odds (variation) (SI50)
        "Start",                "Start",
"-----","-----","-----","-----","-----","-----",
"Start","-----","Start","Start","-----","Start",
"Start","Start","Start","Start","Start","Start",
"Start","Start","Start","Start","Start","Start",
"Start","Start","Start","Start","Start","Start",
"Press INT (F2). Display will show \"PLACEbET\".\n" \
"Press Start twice to start rolling.\n" \
"Lamps will flicker.\n" \
"Press SENS (F1) to stop rolling.\n" \
"Result will be displayed as digits and lamps.\n" \
"If game not over, press INT (F2) to go back to \"PLACEbet\" screen.\n" \
"If game over (\"YOU HOPE\"), press RST (F4)."
},
{ // 229. Monster Munchers (Interton)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): start game\n" \
"Paddle directions: move\n" \
"Left controller is green muncher. Right controller is yellow/blue monster."
},
{ // 230. Flappy Birds (Maibaum?/Keller?) (Interton)
        "Flap" ,                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Flap: Flap wings"
},
{ // 231. Galaxia
        "Fire",                 "Fire",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"F1: 1UP\n" \
"F2: 2UP\n" \
"F3: Coin A\n" \
"F4: Coin B\n" \
"F5: Reset\n" \
"Fire (space bar) = Fire bullet\n" \
"Paddle left/right: move ship\n\n" \
"Official instructions:\n" \
"* Insert coin(s).\n" \
"* Select one or two player game.\n" \
"* If two player game is selected, players play alternately.\n" \
"* Operate the control buttons to move starship \"Galaxia\" to the left or to the right.\n" \
"* The alien force will send squadrons to attack \"Galaxia\".\n" \
"* Move \"Galaxia\" to avoid missiles from alien squadrons or collisions.\n" \
"* Fire missiles and eliminate alien beings.\n" \
"* Watch your fuel supply.\n" \
"* Hit alien beings when they are near, the score will be higher."
},
{ // 232. Come Come
        "Fast",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): change game variant (1..12) (hold down until display changes)\n" \
"Default variant is 0 (same as 2).\n" \
"Until pressing START (F1), or after game over, you are in practice mode (movement and sound but no collisions).\n" \
"START (F1): start game\n" \
"Paddle directions: move\n" \
"Fast = Faster movement while held down\n" \
"(only in variants 3,4,7,8,11,12, and only while there is a blue ghost)\n" \
"Song plays at start of practice mode *or* game.\n" \
"Game over is indicated by a sequence of rapid beeps."
},
{ // 233. Crazy Crab
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant (1..16)\n" \
"START (F1): start game\n" \
"Paddle directions: move"
},
{ // 234. Munch & Crunch
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant (1..8)\n" \
"START (F1): start game\n" \
"Paddle directions: move"
},
{ // 235. EPROM Programmer aka Table 48 (Elektor)
        "Start",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): end\n" \
"Colours are:\n" \
"Blue: ready (before starting)\n" \
"White: error (memory block not clear)\n" \
"Blue: programming (after starting)\n" \
"Green/red: success/failure"
},
{ // 236. Type-right
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"F1: Class\n" \
"F2: Game\n" \
"F3: Speed\n" \
"F4: Accuracy"
},
{ // 237. Game of Death (SI50)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"If 4 cells in a row are empty: then a new cell is created at the second position from the right. (.... -> ..*.)\n" \
"If a living cell has no neighbours, then it proliferates. (.*. -> *.*)\n" \
"A living cell with a living neighbour at each side dies. (*** -> *.*)\n" \
"Cells do not influence each other across the edge (no connected ends). The edge is regarded as a living cell."
},
{ // 238. Tennis (SI50)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SENS (F1): Serve/volley (left player)\n" \
"INT (F2): Serve/volley (right player)\n"
},
{ // 239. Flappy Birds (Augustin) (Interton)
        "Flap" ,                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose difficulty, which manifests in the upwards movement of the bird which cannot be stopped.\n" \
"START (F1): start game\n" \
"Flap: Flap wings\n\n" \
"The left joystick is used to seed the random number generator on initial startup."
},
{ // 240. 3D Attack (Arcadia)
        "Fire1",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","Selec","-----","-----","-----",
"-----","Fire2","Start","-----","-----","-----",
"-----","Hover","-----","-----","-----","-----",
"Pause","Hover","Unpau","-----","-----","-----",
"START (F1): start normal game\n" \
"A/SELECT (F2): start demo game\n" \
"B/OPTION (F3): start practice game\n" \
"Selec = Scene select (1..5)\n" \
"Start = Start repeat game from current scene\n" \
"Fire1 = Fire first missile\n" \
"Fire2 = Fire second missile\n" \
"Hover = Hover in position (while held)\n" \
"Unpau = Unpause\n" \
"You can have two missiles in the air at a time if you use Fire1 and Fire2 alternately.\n" \
"Fuel depletes more rapidly when firing.\n" \
"Scene number is not shown on screen. Scenes are:\n" \
"1,2,4: runway\n" \
"3: electric fence\n" \
"5: wall\n" \
"End each scene before fuel runs out by flying to the right."
},
{ // 241. Robot Killer (Arcadia)
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"Fire", "-----","Fire", "-----","-----","-----",
"Fire", "Fire", "Fire", "-----","-----","-----",
"Fire", "Fire", "Fire", "-----","-----","-----",
"Fire", "Fire", "Fire", "-----","-----","-----",
"A (F2): choose \"game\" (1..4)\n" \
"B (F3): choose \"option\" (1..4 in game 1, 1..8 in games 2..4)\n" \
"START (F1): start game\n" \
"Paddle directions: move/aim"
},
{ // 242. Rainbow (Arcadia)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"A (F2): turn flag line on (ie. invert colours)\n" \
"B (F3): turn flag line off"
},
{ // 243. Canabalt (Interton)
        "Jump" ,                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): start game"
},
{ // 244. Circledrive (unpatched)
        "Brake",                "-----",
"-----","-----","-----","-----","-----","-----",
"Start","-----","Accel","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"RESET to game (F5): press this first\n" \
"UC (F2): don't press this key!\n" \
"LC (F3): choose game speed (1..9)\n" \
"START (F1) or Start: start game\n" \
"Accel = Accelerate\n" \
"Paddle directions: change lanes\n" \
"Red squares: +2 points. Square turns blue.\n" \
"Blue squares: +1 point. Square stays blue, turns black or disappears.\n" \
"Black squares: -2 points.\n" \
"Left digits: game speed\n" \
"Right digits: score"
},
{ // 245. Christmas Tree with Stars (28-4)
        "Yello",                "-----",
"-----","-----","-----","-----","-----","-----",
"Purpl","-----","Black","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Purpl = Purple tree, black background, yellow stars. Blue stripes in background (while held).\n" \
"Yello = Yellow tree, blue background, yellow stars\n" \
"Black = Black tree, black background" 
},
{ // 246. 07-2-Codebreaker (Elektor)
        "8",                    "8",
"-----","-----","-----","-----","-----","-----",
"7",    "-----","-----","7",    "-----","-----", // 9 is not used
"4",    "5",    "6",    "4",    "5",    "6",
"1",    "2",    "3",    "1",    "2",    "3",
"Clear","Selec","Enter","Clear","Selec","Enter",
"UC (F2): choose game variant\n" \
"START (F1): start game\n" \
"Selec = Select position (ie. move cursor)\n" \
"This is the same principle as Mastermind, but the code consist of digits or shapes instead of colours. Furthermore, it is also possible for two players to play simultaneously (each with a different code!). Each player's score, and the total number of games played, are displayed at the top of the screen. In all, 24 different variations of the game can be played with the same program."
},
{ // 247. Kaboom (Interton)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"SELECT (F2): choose game variant\n" \
"Variants are:\n" \
"1: Big bucket, computer controls bomber, normal mode.\n" \
"2: Big bucket, player 2 controls bomber, normal mode.\n" \
"3: Small bucket, computer controls bomber, normal mode.\n" \
"4: Small bucket, player 2 controls bomber, normal mode.\n" \
"5: Big bucket, computer controls bomber, delta-X mode.\n" \
"6: Big bucket, player 2 controls bomber, delta-X mode.\n" \
"7: Small bucket, computer controls bomber, delta-X mode.\n" \
"8: Small bucket, player 2 controls bomber, delta-X mode.\n" \
"START (F1): start game\n" \
"Paddle left/right: move left/right"
},
{ // 248. Canabalt (Summer 2018 edition) (Interton)
        "Jump" ,                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): start game\n" \
"SELECT (F2): toggle background colour (dark cyan/grey)"
},
{ // 249. Tetris (betas) (Interton)
        "Down",                 "-----",
"-----","-----","-----","-----","-----","-----",
"Anti" ,"-----","Anti" ,"-----","-----","-----",
"Clock","-----","Clock","-----","-----","-----",
"Frwrd","-----","Frwrd","-----","-----","-----",
"Bkwrd","-----","Bkwrd","-----","-----","-----",
"On title screen:\n" \
"SELECT (F2): toggle mode (A/B)\n" \
"START (F1): when done\n" \
"SELECT (F2): adjust difficulty (0..9)\n" \
"START (F1): when done\n" \
"During play:\n" \
"Down = move piece down (while held)\n" \
"Clock = spin piece clockwise\n" \
"Anti = spin piece anticlockwise\n" \
"Frwrd = cycle forwards through pieces\n" \
"Bkwrd = cycle backwards through pieces\n" \
"Paddle left/right/down: move piece left/right/down"
},
{ // 250. Tetris (release version) (Interton)
        "Down",                 "-----",
"-----","-----","-----","-----","-----","-----",
"Anti" ,"-----","Anti" ,"-----","-----","-----",
"Clock","-----","Clock","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"On title screen:\n" \
"Anti: music\n" \
"Clock: sound effects\n" \
"SELECT (F2): toggle mode (A/B)\n" \
"START (F1): when done\n" \
"SELECT (F2): adjust difficulty (0..9)\n" \
"START (F1): when done\n" \
"In mode B only:\n" \
"SELECT (F2): adjust height (0..5)\n" \
"START (F1): when done\n" \
"During play:\n" \
"Down = move piece down (while held)\n" \
"Clock = spin piece clockwise\n" \
"Anti = spin piece anticlockwise\n" \
"Paddle left/right/down: move piece left/right/down\n" \
"START (F1): pause/unpause"
},
{ // 251. Hocosoft Solitaire (Elektor)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","4"    ,"5",    "6",    "7",
"-----","-----","0"    ,"1",    "2",    "3",
"Each move is entered as:\n" \
"X (column) of \"from\" (start) position.\n" \
"Y (row) of \"from\" (start) position.\n" \
"X (column) of \"to\" (end) position.\n" \
"Y (row) of \"to\" (end) position."
},
{ // 252. Griddy & Grid Defender (Interton)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Left paddle: move"
},
{ // 253. Menu (Interton)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Left paddle up/down: move cursor"
},
{ // 254. Hex-Dec-Hex Conversion (Selbst)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"C"    ,"D"    ,"E"    ,"F"    ,"-----","-----",
"8"    ,"9"    ,"A"    ,"B"    ,"+"    ,"-----",
"4"    ,"5"    ,"6"    ,"7"    ,"-"    ,"-----",
"0"    ,"1"    ,"2"    ,"3"    ,"-----","-----",
"For decimal-to-hex conversion:\n" \
"Press D.\n" \
"Press + or - for the sign of the number.\n" \
"Enter 5-digit decimal number.\n" \
"For hex-to-decimal conversion:\n" \
"Press B.\n" \
"Enter 4-digit hex number in two's complement format."
},
{ // 255. Hex-Calculation (Selbst)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"C"    ,"D"    ,"E"    ,"F"    ,"-----","-----",
"8"    ,"9"    ,"A"    ,"B"    ,"+"    ,"-----",
"4"    ,"5"    ,"6"    ,"7"    ,"-"    ,"*"    ,
"0"    ,"1"    ,"2"    ,"3"    ,"-----","/"    , // or use proper division symbol
"Enter 1st 4-digit hex number.\n" \
"Press button for desired arithmetic operation.\n" \
"Enter 2nd 4-digit hex number."
},
{ // 256. Dec-Calculation (Selbst)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"8"    ,"9"    ,"-----","-----","+"    ,"-----",
"4"    ,"5"    ,"6"    ,"7"    ,"-"    ,"*"    ,
"0"    ,"1"    ,"2"    ,"3"    ,"-----","/"    , // or use proper division symbol
"Press + or - for the sign of the 1st number.\n" \
"Enter 1st 5-digit decimal number.\n" \
"Press button for desired arithmetic operation.\n" \
"Press + or - for the sign of the 2nd number.\n" \
"Enter 2nd 5-digit decimal number."
},
{ // 257. Stopwatch (Selbst)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Start","Stop" ,"-----","-----","Reset","-----",
"Start: start stopwatch\n" \
"Stop: stop stopwatch\n" \
"Reset: reset to 00.00.00"
},
{ // 258. Digital Clock (Selbst)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Hours","Mins" ,"Start","-----","-----","-----",
"Hours: increase hours (00..23)\n" \
"Mins: increase minutes (00..59)\n" \
"Start: start clock"
},
{ // 259. Animated 'S' (Elektor)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","1st"  ,"2nd"  ,"3rd"  ,
"-----","-----","-----","-----","-----","-----",
"1st/2nd/3rd: show 1st/2nd/3rd image"
},
{ // 260. Writing to the Screen (10-1d) (Elektor)
        "7"    ,                "Space",
"-----","-----","-----","-----","-----","-----",
"3"    ,"-----","B"    ,"n"    ,"-----","#1"   ,
"2"    ,"6"    ,"A"    ,"I"    ,"="    ,"#2"   ,
"1"    ,"5"    ,"9"    ,"L"    ,"r"    ,"-"    ,
"0"    ,"4"    ,"8"    ,"G"    ,"P"    ,"+"    ,
"#1/#2 = show garbage #1/#2"
},
{ // 261. Exploding Man (10-1e) (Elektor)
        "Show" ,                "Show" ,
"Show" ,"Show" ,"Show" ,"Show" ,"Show" ,"Show",
"Show" ,"Show" ,"Show" ,"Show" ,"Show" ,"Show",
"Show" ,"Show" ,"Show" ,"Show" ,"Show" ,"Show",
"Show" ,"Show" ,"Show" ,"Show" ,"Show" ,"Show",
"Show" ,"Show" ,"Show" ,"Show" ,"Show" ,"Show",
"Show or UC/START/LC/RESET (F1-F4): show \"DEAD\""
},
{ // 262. Arcadian Tree Simulator
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Paddle left/right: rotate tree left/right.\n"\
"Be a tree."
},
{ // 263. Ika nashi no tami muke toon 3
        "Fire" ,                "Fire" ,
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Fire" ,"-----","-----","Fire" ,"-----",
"-----","Fire" ,"-----","-----","Fire" ,"-----",
"-----","Fire" ,"-----","-----","Fire" ,"-----",
"START (F1): start game\n" \
"Paddle directions: move"
},
{ // 264. Arcadia Paint
        "Draw" ,                "Draw" ,
"-----","-----","-----","-----","-----","-----",
"Palet","-----","Canva","Palet","-----","Canva",
"Colou","Up"   ,"Eyedr","Colou","Up"   ,"Eyedr",
"Left" ,"Curso","Right","Left" ,"Curso","Right",
"Erase","Down" ,"Thick","Erase","Down" ,"Erase",
"START (F1): clear screen\n" \
"A (F2): load next preset image\n" \
"Palet = toggle palettes\n" \
"Canva = change canvases\n" \
"Colou = change colours\n" \
"Eyedr = eyedropper\n" \
"Curso = view/hide cursor\n" \
"Erase = eraser\n" \
"Thick = toggle thickness\n" \
"Up/Down/Left/Right = move cursor up/down/left/right by 1\n" \
"Paddle directions: move cursor"
},
{ // 265. Retoran San No Are
        "Forwa",                "-----",
"-----","-----","-----","-----","-----","-----",
"Left" ,"-----","Right","-----","-----","-----",
"Left" ,"Forwa","Right","-----","-----","-----",
"Left" ,"Forwa","Right","-----","-----","-----",
"Left" ,"Forwa","Right","-----","-----","-----",
"START (F1): start game\n" \
"Forwa = move forwards\n" \
"Left/Right = turn left/right"
},
{ // 266. Tetris (Japanese version)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Hold" ,"Up"   ,"Start","-----","-----","-----",
"Left" ,"Down" ,"Right","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Down = proceed to options menu\n" \
"Up/Down = move cursor up/down\n" \
"Start = start game\n" \
"Left/Right = move piece left/right\n" \
"Up = drop piece\n" \
"Paddle left/right: rotate piece left/right"
},
{ // 267. Teaching Arcadia
        "2"    ,                "-----",
"-----","-----","-----","-----","-----","-----",
"1"    ,"-----","3"    ,"-----","-----","-----",
"4"    ,"-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): start game\n" \
"1..4 = choose option 1..4"
},
{ // 268. ESS-07-7: 4 In a Row (8*7 board)
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Move" ,"-----","Drop" ,"Move", "-----","Drop",
"UC: choose game variant (1-3):\n" \
"1: green (left human) vs. red (right human) with green starting.\n" \
"2: red (right human) vs. green (computer) with red starting.\n" \
"3: red (right human) vs. green (computer) with green starting.\n" \
"START (F1): start game\n" \
"Move = Move cursor right\n" \
"Drop = Drop token\n" \
"This is a fairly well-known game. The object is to get four of your squares in a row (horizontal, vertical or diagonal).\n" \
"Skill comes with practice, and a more challenging game becomes desirable. This version uses an 8*7 board, instead of 7*6 as in ESS-07-6. The extra row and column make quite a difference! It is distinctly noticeable that the computer needs much more time to work out its moves - no trick here, it really is a question of the time it needs for its calculations."
},
{ // 269. Journey
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): start game\n" \
"Paddle directions: move\n" \
"RESET (F5): at game over"
},
{ // 270. Space Buster
        "Fire",                 "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"START (F1): start game\n" \
"Paddle left/right: move\n" \
"RESET (F5): at game over"
},
{ // 271. Table A/B
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","Don't","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Don't = Don't press this key!"
},
{ // 272. Joystick aka Table 23
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Left paddle: adjust *($1FCC)\n" \
"Right paddle: adjust *($1FCD)"
},
{ // 273. Disassembler aka Table 34
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"UC (F2): pause\n" \
"START (F1): unpause"
},
{ // 274. Page 91, Figure 26/27
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Lt2"  ,"Rt2"  ,"-----","-----","-----","-----",
"Lt1"  ,"Rt1"  ,"-----","-----","-----","-----",
"Lt1/Rt1 = move 1st paddle left/right\n" \
"Lt2/Rt2 = move 2nd paddle left/right"
},
{ // 275. Frogger 1.3+
        "Up"   ,                "-----",
"-----","-----","-----","-----","-----","-----",
"Left" ,"-----","Right","-----","-----","-----",
"Left" ,"Up"   ,"Right","-----","-----","-----",
"Left" ,"Down" ,"Right","-----","-----","-----",
"Left" ,"Down" ,"Right","-----","-----","-----",
"START (F1): start game\n" \
"Up/Down/Left/Right or paddle: move"
},
{ // 276. Space Zap
        "Fire" ,                "-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","Fire" ,"-----","-----","-----","-----",
"-----","Fire" ,"-----","-----","-----","-----",
"-----","Fire" ,"-----","-----","-----","-----",
"START (F1): start game\n" \
"Hold paddle in desired direction while firing (otherwise shoots upwards)."
},
{ // 277. Akane's Hataagi
        "Both" ,                "-----",
"-----","-----","-----","-----","-----","-----",
"Both" ,"-----","Both" ,"-----","-----","-----",
"Both" ,"Both" ,"Both" ,"-----","-----","-----",
"Both" ,"Both" ,"Both" ,"-----","-----","-----",
"Both" ,"Both" ,"Both" ,"-----","-----","-----",
"Paddle left/right: raise your right/left hand (left/right side)\n" \
"Both: go to next screen or confirm and raise both hands"
},
{ // 278. Akane's Home Run Derby
        "-----",                "-----",
"-----","-----","-----","-----","-----","-----",
"Swing","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"-----","-----","-----","-----","-----","-----",
"Paddle left/right: move left/right\n" \
"Swing: swing bat"
},
};
