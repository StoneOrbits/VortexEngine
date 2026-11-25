#ifndef POV_TEXTURES_H
#define POV_TEXTURES_H

#include <inttypes.h>

// POV textures for the Vortex
//
// When the Vortex spins on its string each arm sweeps through the air like a
// column of 9 pixels, so a texture is a small picture 9 pixels tall and any
// number of columns wide. The columns are shown one after another and the
// motion of the arm spreads them out into an image. All 8 segments (4 arms on
// the front and 4 on the back) show the same column at the same time.
//
// Each texture is 9 strings, one per row, all the same width:
//   row 0 is the tip (the large led at the end of each arm)
//   row 8 is the led closest to the center
//
// Each character is one pixel:
//   '.'      off
//   '1'-'8'  a color 'role', each copy of the texture moves on to the next
//            colors in the colorset so every color gets used (see
//            PovPattern.h), use as many roles as the picture needs
//   '*'      the same as '1'
//   '0'      trim: always the first color of the colorset at full brightness,
//            it never rotates so it frames the texture in one steady color,
//            it blinks with the texture's trimOn / trimOff frames.
//            Row 0 is the four large corner leds so a row of '0' there makes
//            a border around the whole device
//
// After the rows comes the number of blank columns between copies and the
// brightness of each color role (see PovTexture below).
//
// To add a texture: add an entry to this list, add a PATTERN_POV_ id at the
// end of the multi led patterns in Patterns.h (in the same order) and it will
// show up in the pattern list automatically.

#define POV_TEXTURE_HEIGHT 9

struct PovTexture
{
  const char *name;
  const char *rows[POV_TEXTURE_HEIGHT];
  // blank columns between copies of the texture (the default loop gap arg)
  uint8_t gap;
  // brightness of each color role ('1' to '8'), 0 means full brightness.
  // Giving the roles different brightnesses keeps the sections of a texture
  // apart even when the colorset has similar colors
  uint8_t levels[8];
  // the trim ('0') blinks this many frames on then this many off, 0 = solid
  uint8_t trimOn;
  uint8_t trimOff;
};

static const PovTexture povTextures[] = {
  // zigzag: a line bouncing between the centre and the tips
  { "zigzag", {
    "1...............",
    ".1.............1",
    "..1...........1.",
    "...1.........1..",
    "....1.......1...",
    ".....1.....1....",
    "......1...1.....",
    ".......1.1......",
    "........1.......",
  }, 2 },
  // sine: a smooth wave
  { "sine", {
    ".................111....",
    "...............11...11..",
    "..............1.......1.",
    ".............1.........1",
    "1...........1...........",
    ".1.........1............",
    "..1.......1.............",
    "...11...11..............",
    ".....111................",
  }, 2 },
  // helix: two waves crossing like a double helix
  { "helix", {
    ".....222.........111....",
    "...22...22.....11...11..",
    "..2.......2...1.......1.",
    ".2.........2.1.........1",
    "2...........2...........",
    ".1.........1.2.........2",
    "..1.......1...2.......2.",
    "...11...11.....22...22..",
    ".....111.........222....",
  }, 2 },
  // checker: a checkerboard of 3x3 blocks
  { "checker", {
    "111222111222",
    "111222111222",
    "111222111222",
    "222111222111",
    "222111222111",
    "222111222111",
    "111222111222",
    "111222111222",
    "111222111222",
  }, 2 },
  // diamonds: a chain of outlined diamonds
  { "diamonds", {
    "....1.....",
    "...1.1....",
    "..1...1...",
    ".1.....1..",
    "1...2...1.",
    ".1.....1..",
    "..1...1...",
    "...1.1....",
    "....1.....",
  }, 2 },
  // chevrons: arrows pointing along the spin
  { "chevrons", {
    "11....22....33....",
    ".11....22....33...",
    "..11....22....33..",
    "...11....22....33.",
    "....11....22....33",
    "...11....22....33.",
    "..11....22....33..",
    ".11....22....33...",
    "11....22....33....",
  }, 2 },
  // hearts: hearts, each heart is the next color
  { "hearts", {
    ".**...**....",
    "****.****...",
    "*********...",
    "*********...",
    ".*******....",
    "..*****.....",
    "...***......",
    "....*.......",
    "............",
  }, 2 },
  // stars: five pointed stars, each star is the next color
  { "stars", {
    "....*.......",
    "....*.......",
    "...***......",
    "*********...",
    ".*******....",
    "..*****.....",
    "..**.**.....",
    ".**...**....",
    ".*.....*....",
  }, 2 },
  // bricks: an offset brick wall
  { "bricks", {
    "1111111.2222222.",
    "1111111.2222222.",
    "................",
    "222.1111111.2222",
    "222.1111111.2222",
    "................",
    "1111111.2222222.",
    "1111111.2222222.",
    "................",
  }, 2 },
  // diagonals: diagonal stripes in three colors
  { "diagonals", {
    "11....22....33....",
    "1....22....33....1",
    "....22....33....11",
    "...22....33....11.",
    "..22....33....11..",
    ".22....33....11...",
    "22....33....11....",
    "2....33....11....2",
    "....33....11....22",
  }, 2 },
  // equalizer: bars rising from the centre like a music meter
  { "equalizer", {
    "............3...........3.......",
    "....3.......3.....3.....3.......",
    "....2.....2.2.....2.....2.2.....",
    "....2.2...2.2...2.2.....2.2...2.",
    "..2.2.2...2.2...2.2...2.2.2...2.",
    "..1.1.1...1.1.1.1.1...1.1.1...1.",
    "..1.1.1.1.1.1.1.1.1.1.1.1.1...1.",
    "1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.",
    "1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.1.",
  }, 2 },
  // dots: scattered dots
  { "dots", {
    ".................3..",
    ".1..................",
    "........3...........",
    "...........1........",
    "..............2.....",
    "....2...............",
    ".3..................",
    "..........1.........",
    "..................1.",
  }, 2 },
  // text: the word VORTEX, each copy is the next color
  { "text", {
    "..........................................",
    "*...*..***..****..*****.*****.*...*.......",
    "*...*.*...*.*...*...*...*.....*...*.......",
    "*...*.*...*.*...*...*...*......*.*........",
    "*...*.*...*.****....*...****....*.........",
    "*...*.*...*.*.*.....*...*......*.*........",
    ".*.*..*...*.*..*....*...*.....*...*.......",
    "..*....***..*...*...*...*****.*...*.......",
    "..........................................",
  }, 2 },
  // ladder: two rails joined by rungs
  { "ladder", {
    "........",
    "11111111",
    "2.......",
    "2.......",
    "2.......",
    "2.......",
    "2.......",
    "11111111",
    "........",
  }, 2 },
  // stairs: a filled staircase climbing from the centre to the tips
  { "stairs", {
    "................22..",
    "..............2222..",
    "............222222..",
    "..........22222222..",
    "........2222222222..",
    "......111111111111..",
    "....11111111111111..",
    "..1111111111111111..",
    "111111111111111111..",
  }, 2 },
  // bunting: triangles hanging from the tips between triangles rising from the centre
  { "bunting", {
    "1111111111111111",
    ".111111..111111.",
    "..1111....1111..",
    "...11......11...",
    "................",
    "2......22......2",
    "22....2222....22",
    "222..222222..222",
    "2222222222222222",
  }, 2 },
  // scales: overlapping fish scales in three rows
  { "scales", {
    "..11....11..",
    ".1..1..1..1.",
    "1....11....1",
    "2....22....2",
    ".2..2..2..2.",
    "..22....22..",
    "..33....33..",
    ".3..3..3..3.",
    "3....33....3",
  }, 2 },
  // crosses: plus signs in two colors
  { "crosses", {
    "............",
    "............",
    "..1.....2...",
    "..1.....2...",
    "11111.22222.",
    "..1.....2...",
    "..1.....2...",
    "............",
    "............",
  }, 2 },

  // ======== native american inspired, greek inspired and tribal inspired ========
  // row 0 of these is the trim on the four corner leds (see '0' above)
  // navajo: stepped diamonds between two borders like a woven rug band, trim blinks 7 on 1 off
  { "navajo", {
    "00000000000000000000",
    "........1111........",
    "......11122111......",
    "....111222222111....",
    "..1112222222222111..",
    "....111222222111....",
    "......11122111......",
    "........1111........",
    "33333333333333333333",
  }, 0, { 255, 110, 35 }, 7, 1 },
  // dazzler: concentric diamonds like an eye dazzler blanket, trim blinks 7 on 3 off
  { "dazzler", {
    "00000000000000000",
    ".11.33.222.33.11.",
    "11.33.22.22.33.11",
    "1.33.22.1.22.33.1",
    ".33.22.111.22.33.",
    "1.33.22.1.22.33.1",
    "11.33.22.22.33.11",
    ".11.33.222.33.11.",
    "2.11.33.2.33.11.2",
  }, 0, { 255, 110, 35 }, 7, 3 },
  // arrows: arrowheads on a long shaft, trim blinks 7 on 2 off
  { "arrows", {
    "00000000000000",
    "..1...........",
    "..11..........",
    "..111.........",
    "22111122222222",
    "..111.........",
    "..11..........",
    "..1...........",
    "..............",
  }, 0, { 255, 90, 0 }, 7, 2 },
  // mesa: stepped pyramids rising from the center, trim blinks 4 on 2 off
  { "mesa", {
    "0000000000000000",
    "......111.......",
    "......111.......",
    "....2211122.....",
    "....1111111.....",
    "..22221112222...",
    "..11111111111...",
    "222222111222222.",
    "111111111111111.",
  }, 0, { 255, 110, 35 }, 4, 2 },
  // lightning: lightning strikes with a star in between, trim blinks 10 on 1 off
  { "lightning", {
    "0000000000000000000000000000",
    ".1111........1111...........",
    "..1111......1111............",
    "...1111....1111.....2.......",
    "....1111..1111.....222......",
    ".....11111111.......2.......",
    "......111111................",
    ".......1111.................",
    "........11..................",
  }, 0, { 255, 70, 0 }, 10, 1 },
  // feathers: feathers, trim blinks 4 on 1 off
  { "feathers", {
    "0000000000000",
    "....131......",
    "...12122.....",
    "..1121122....",
    "..1121122....",
    "..1121122....",
    "...12122.....",
    "....121......",
    ".....2.......",
  }, 0, { 255, 100, 30 }, 4, 1 },
  // greekkey: the greek key meander under a border, trim blinks 5 on 1 off
  { "greekkey", {
    "00000000",
    "........",
    "1111111.",
    "1.....1.",
    "1.111.1.",
    "1.1.1.1.",
    "1.11111.",
    "1.......",
    "11111111",
  }, 0, { 255, 60, 0 }, 5, 1 },
  // dentils: a cornice of tooth blocks between two bands, trim blinks 8 on 3 off
  { "dentils", {
    "000",
    "111",
    "...",
    "22.",
    "22.",
    "22.",
    "...",
    "333",
    "333",
  }, 0, { 255, 110, 35 }, 8, 3 },
  // eggdart: egg and dart molding, trim blinks 6 on 1 off
  { "eggdart", {
    "0000000000",
    "..........",
    "..111.....",
    ".1...1..2.",
    "1.222.1.2.",
    "1.222.1.2.",
    ".1...1.222",
    "..111...2.",
    "3333333333",
  }, 0, { 255, 110, 35 }, 6, 1 },
  // laurel: a laurel branch, trim blinks 11 on 1 off
  { "laurel", {
    "00000000000",
    "..1.....1..",
    ".11....11..",
    "11....11...",
    "22222222222",
    "11....11...",
    ".11....11..",
    "..1.....1..",
    "...........",
  }, 0, { 255, 90, 0 }, 11, 1 },
  // columns: fluted greek columns, trim blinks 5 on 2 off
  { "columns", {
    "000000000",
    ".11111...",
    ".1.1.1...",
    ".1.1.1...",
    ".1.1.1...",
    ".1.1.1...",
    ".1.1.1...",
    ".11111...",
    "2222222..",
  }, 0, { 255, 90, 0 }, 5, 2 },
  // teeth: interlocking wolf teeth triangles, trim blinks 3 on 2 off
  { "teeth", {
    "0000000000000000",
    ".111111..111111.",
    "..1111....1111..",
    "...11......11...",
    "3333333333333333",
    "2......22......2",
    "22....2222....22",
    "222..222222..222",
    "2222222222222222",
  }, 0, { 255, 110, 35 }, 3, 2 },
  // spirals: spirals like a koru, trim blinks 1 on 1 off
  { "spirals", {
    "000000000000",
    "..111111....",
    ".1......1...",
    "1..2222..1..",
    "1.2....2.1..",
    "1.2.33.2.1..",
    "1.2..3.2.1..",
    ".1....2..1..",
    "..11111.1...",
  }, 0, { 255, 110, 35 }, 1, 1 },
  // kente: woven kente cloth blocks, trim blinks 6 on 2 off
  { "kente", {
    "000000000000",
    "111111322223",
    "333333322223",
    "111111322223",
    "111111322223",
    "333333322223",
    "111111322223",
    "111111322223",
    "3333333....3",
  }, 0, { 255, 110, 35 }, 6, 2 },
  // mudcloth: mudcloth rows of dots, lines and crosses, trim blinks 12 on 2 off
  { "mudcloth", {
    "000000000000",
    "............",
    "222222222222",
    "............",
    ".3...3...3..",
    "333.333.333.",
    ".3...3...3..",
    "............",
    "222222222222",
  }, 0, { 255, 110, 35 }, 12, 2 },
  // shield: oval shields with a boss in the middle, trim blinks 2 on 2 off
  { "shield", {
    "00000000000000",
    "..12222221....",
    ".1211111121...",
    ".1213333121...",
    ".2113333112...",
    ".1213333121...",
    ".1211111121...",
    "..12222221....",
    "....1111......",
  }, 0, { 255, 110, 35 }, 2, 2 },
  // tattoo: a tattoo band of diamonds and dots, trim blinks 6 on 3 off
  { "tattoo", {
    "00000000000000000000",
    "....11........11....",
    "...1..1......1..1...",
    "..1....1....1....1..",
    ".1..22..1..1..22..1.",
    "..1....1....1....1..",
    "...1..1......1..1...",
    "....11........11....",
    "3.3.3.3.3.3.3.3.3.3.",
  }, 0, { 255, 110, 35 }, 6, 3 },
  // crossfire: stepped crosses like a rug, trim blinks 9 on 3 off
  { "crossfire", {
    "000000000000",
    "....1111....",
    "....1221....",
    ".1111221111.",
    "312222222213",
    "312222222213",
    ".1111221111.",
    "....1221....",
    "....1111....",
  }, 0, { 255, 90, 30 }, 9, 3 },
  // hourglass: pueblo hourglass triangles, trim blinks 12 on 1 off
  { "hourglass", {
    "0000000000",
    ".11111111.",
    "..111111..",
    "...1111...",
    "3...11...3",
    "3...22...3",
    "...2222...",
    "..222222..",
    ".22222222.",
  }, 0, { 255, 100, 40 }, 12, 1 },
  // beadwork: plains beadwork triangles between stripes, trim blinks 3 on 1 off
  { "beadwork", {
    "000000000000",
    "333333333333",
    "............",
    ".....11.....",
    "....1221....",
    "...122221...",
    "..12222221..",
    ".1111111111.",
    "333333333333",
  }, 0, { 255, 100, 35 }, 3, 1 },
  // tipis: tipis with crossed poles and a door, trim blinks 10 on 2 off
  { "tipis", {
    "000000000000",
    "....2.2.....",
    ".....2......",
    ".....1......",
    "....111.....",
    "...11111....",
    "..1113111...",
    ".111333111..",
    "11113331111.",
  }, 0, { 255, 120, 40 }, 10, 2 },
  // rattlesnake: a diamondback snake band, trim blinks 4 on 3 off
  { "rattlesnake", {
    "00000000",
    "........",
    "...11...",
    "..1221..",
    ".122221.",
    "12233221",
    ".122221.",
    "..1221..",
    "...11...",
  }, 0, { 255, 100, 30 }, 4, 3 },
  // starquilt: eight pointed stars like a star quilt, trim blinks 10 on 3 off
  { "starquilt", {
    "000000000000",
    ".....1......",
    "...1.1.1....",
    "...11211....",
    ".111222111..",
    ".111222111..",
    "...11211....",
    "...1.1.1....",
    ".....1......",
  }, 0, { 255, 110, 0 }, 10, 3 },
  // amphora: amphora vases with handles, trim blinks 11 on 2 off
  { "amphora", {
    "00000000000",
    "..111111...",
    "...1111....",
    ".2.1111.2..",
    ".21111112..",
    ".11333311..",
    "..111111...",
    "...1111....",
    "..111111...",
  }, 0, { 255, 90, 40 }, 11, 2 },
  // palmette: palmette fans on a base, trim blinks 9 on 1 off
  { "palmette", {
    "00000000000",
    "..1..1..1..",
    "...1.1.1...",
    "1..1.1.1..1",
    ".1..111..1.",
    "...11111...",
    "....111....",
    "....222....",
    "...22222...",
  }, 0, { 255, 90, 0 }, 9, 1 },
  // battlement: battlements over a dotted frieze, trim blinks 2 on 1 off
  { "battlement", {
    "000000",
    "111...",
    "111...",
    "111111",
    "......",
    ".2..2.",
    "......",
    "333333",
    "...333",
  }, 0, { 255, 100, 40 }, 2, 1 },
  // guilloche: two interwoven bands with dots in the eyes, trim blinks 11 on 3 off
  { "guilloche", {
    "000000000000",
    "..222...111.",
    "..2.2...1.1.",
    ".2...2.1...1",
    "1..3..1..3..",
    "1..3..1..3..",
    ".1...1.2...2",
    "..1.1...2.2.",
    "..111...222.",
  }, 0, { 255, 110, 40 }, 11, 3 },
  // sharkteeth: rows of shark teeth triangles, trim blinks 5 on 3 off
  { "sharkteeth", {
    "00000000",
    "111.111.",
    ".1...1..",
    "2.222.22",
    "...2...2",
    "111.111.",
    ".1...1..",
    "3.333.33",
    "...3...3",
  }, 0, { 255, 110, 40 }, 5, 3 },
  // sun: a tribal sun with rays, trim blinks 8 on 1 off
  { "sun", {
    "000000000000",
    "..2..2..2...",
    "..22.2.22...",
    "...11111....",
    "22.13331.22.",
    "22.13331.22.",
    "...11111....",
    "..22.2.22...",
    "..2..2..2...",
  }, 0, { 255, 100, 40 }, 8, 1 },
  // chain: a chain of interlocking rings, trim blinks 8 on 2 off
  { "chain", {
    "00000000",
    ".111.222",
    "11.112.2",
    "1..211.1",
    "...2.1.1",
    "...2.1.1",
    "1..211.1",
    "11.112.2",
    ".111.222",
  }, 0, { 255, 110, 0 }, 8, 2 },
  // basket: an over and under basket weave, trim blinks 9 on 2 off
  { "basket", {
    "00000000",
    "11112.2.",
    "....2.2.",
    "11112.2.",
    "....2.2.",
    "2.2.1111",
    "2.2.....",
    "2.2.1111",
    "2.2.....",
  }, 0, { 255, 110, 0 }, 9, 2 },
};

#define NUM_POV_TEXTURES (sizeof(povTextures) / sizeof(povTextures[0]))

#endif
