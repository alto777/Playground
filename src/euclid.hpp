// euclide.hpp lookup table for E(k, n)
// in canonical form
// no error checking!
//

// vector table for n offset into

static const unsigned char tableOffset[17] = {
	0, 1, 3, 6,
	10, 15, 21, 28,
	36, 45, 55, 66,
	78, 91, 105, 120,
	136,
};

// table of all E(k, n)

static const unsigned int euclideanBits[153] = {
	0x0000,	// E( 0,  0) = 0x0000

	0x0000,	// E( 0,  1) = 0x0000  .
	0x0001,	// E( 1,  1) = 0x0001  X

	0x0000,	// E( 0,  2) = 0x0000  ..
	0x0001,	// E( 1,  2) = 0x0001  X.
	0x0003,	// E( 2,  2) = 0x0003  XX

	0x0000,	// E( 0,  3) = 0x0000  ...
	0x0001,	// E( 1,  3) = 0x0001  X..
	0x0003,	// E( 2,  3) = 0x0003  XX.
	0x0007,	// E( 3,  3) = 0x0007  XXX

	0x0000,	// E( 0,  4) = 0x0000  ....
	0x0001,	// E( 1,  4) = 0x0001  X...
	0x0005,	// E( 2,  4) = 0x0005  X.X.
	0x0007,	// E( 3,  4) = 0x0007  XXX.
	0x000f,	// E( 4,  4) = 0x000f  XXXX

	0x0000,	// E( 0,  5) = 0x0000  .....
	0x0001,	// E( 1,  5) = 0x0001  X....
	0x0005,	// E( 2,  5) = 0x0005  X.X..
	0x000b,	// E( 3,  5) = 0x000b  XX.X.
	0x000f,	// E( 4,  5) = 0x000f  XXXX.
	0x001f,	// E( 5,  5) = 0x001f  XXXXX

	0x0000,	// E( 0,  6) = 0x0000  ......
	0x0001,	// E( 1,  6) = 0x0001  X.....
	0x0009,	// E( 2,  6) = 0x0009  X..X..
	0x0015,	// E( 3,  6) = 0x0015  X.X.X.
	0x001b,	// E( 4,  6) = 0x001b  XX.XX.
	0x001f,	// E( 5,  6) = 0x001f  XXXXX.
	0x003f,	// E( 6,  6) = 0x003f  XXXXXX

	0x0000,	// E( 0,  7) = 0x0000  .......
	0x0001,	// E( 1,  7) = 0x0001  X......
	0x0009,	// E( 2,  7) = 0x0009  X..X...
	0x0015,	// E( 3,  7) = 0x0015  X.X.X..
	0x002b,	// E( 4,  7) = 0x002b  XX.X.X.
	0x0037,	// E( 5,  7) = 0x0037  XXX.XX.
	0x003f,	// E( 6,  7) = 0x003f  XXXXXX.
	0x007f,	// E( 7,  7) = 0x007f  XXXXXXX

	0x0000,	// E( 0,  8) = 0x0000  ........
	0x0001,	// E( 1,  8) = 0x0001  X.......
	0x0011,	// E( 2,  8) = 0x0011  X...X...
	0x0025,	// E( 3,  8) = 0x0025  X.X..X..
	0x0055,	// E( 4,  8) = 0x0055  X.X.X.X.
	0x005b,	// E( 5,  8) = 0x005b  XX.XX.X.
	0x0077,	// E( 6,  8) = 0x0077  XXX.XXX.
	0x007f,	// E( 7,  8) = 0x007f  XXXXXXX.
	0x00ff,	// E( 8,  8) = 0x00ff  XXXXXXXX

	0x0000,	// E( 0,  9) = 0x0000  .........
	0x0001,	// E( 1,  9) = 0x0001  X........
	0x0011,	// E( 2,  9) = 0x0011  X...X....
	0x0049,	// E( 3,  9) = 0x0049  X..X..X..
	0x0055,	// E( 4,  9) = 0x0055  X.X.X.X..
	0x00ab,	// E( 5,  9) = 0x00ab  XX.X.X.X.
	0x00db,	// E( 6,  9) = 0x00db  XX.XX.XX.
	0x00ef,	// E( 7,  9) = 0x00ef  XXXX.XXX.
	0x00ff,	// E( 8,  9) = 0x00ff  XXXXXXXX.
	0x01ff,	// E( 9,  9) = 0x01ff  XXXXXXXXX

	0x0000,	// E( 0, 10) = 0x0000  ..........
	0x0001,	// E( 1, 10) = 0x0001  X.........
	0x0021,	// E( 2, 10) = 0x0021  X....X....
	0x0049,	// E( 3, 10) = 0x0049  X..X..X...
	0x00a5,	// E( 4, 10) = 0x00a5  X.X..X.X..
	0x0155,	// E( 5, 10) = 0x0155  X.X.X.X.X.
	0x016b,	// E( 6, 10) = 0x016b  XX.X.XX.X.
	0x01b7,	// E( 7, 10) = 0x01b7  XXX.XX.XX.
	0x01ef,	// E( 8, 10) = 0x01ef  XXXX.XXXX.
	0x01ff,	// E( 9, 10) = 0x01ff  XXXXXXXXX.
	0x03ff,	// E(10, 10) = 0x03ff  XXXXXXXXXX

	0x0000,	// E( 0, 11) = 0x0000  ...........
	0x0001,	// E( 1, 11) = 0x0001  X..........
	0x0021,	// E( 2, 11) = 0x0021  X....X.....
	0x0089,	// E( 3, 11) = 0x0089  X..X...X...
	0x0125,	// E( 4, 11) = 0x0125  X.X..X..X..
	0x0155,	// E( 5, 11) = 0x0155  X.X.X.X.X..
	0x02ab,	// E( 6, 11) = 0x02ab  XX.X.X.X.X.
	0x02db,	// E( 7, 11) = 0x02db  XX.XX.XX.X.
	0x0377,	// E( 8, 11) = 0x0377  XXX.XXX.XX.
	0x03df,	// E( 9, 11) = 0x03df  XXXXX.XXXX.
	0x03ff,	// E(10, 11) = 0x03ff  XXXXXXXXXX.
	0x07ff,	// E(11, 11) = 0x07ff  XXXXXXXXXXX

	0x0000,	// E( 0, 12) = 0x0000  ............
	0x0001,	// E( 1, 12) = 0x0001  X...........
	0x0041,	// E( 2, 12) = 0x0041  X.....X.....
	0x0111,	// E( 3, 12) = 0x0111  X...X...X...
	0x0249,	// E( 4, 12) = 0x0249  X..X..X..X..
	0x0295,	// E( 5, 12) = 0x0295  X.X.X..X.X..
	0x0555,	// E( 6, 12) = 0x0555  X.X.X.X.X.X.
	0x056b,	// E( 7, 12) = 0x056b  XX.X.XX.X.X.
	0x06db,	// E( 8, 12) = 0x06db  XX.XX.XX.XX.
	0x0777,	// E( 9, 12) = 0x0777  XXX.XXX.XXX.
	0x07df,	// E(10, 12) = 0x07df  XXXXX.XXXXX.
	0x07ff,	// E(11, 12) = 0x07ff  XXXXXXXXXXX.
	0x0fff,	// E(12, 12) = 0x0fff  XXXXXXXXXXXX

	0x0000,	// E( 0, 13) = 0x0000  .............
	0x0001,	// E( 1, 13) = 0x0001  X............
	0x0041,	// E( 2, 13) = 0x0041  X.....X......
	0x0111,	// E( 3, 13) = 0x0111  X...X...X....
	0x0249,	// E( 4, 13) = 0x0249  X..X..X..X...
	0x04a5,	// E( 5, 13) = 0x04a5  X.X..X.X..X..
	0x0555,	// E( 6, 13) = 0x0555  X.X.X.X.X.X..
	0x0aab,	// E( 7, 13) = 0x0aab  XX.X.X.X.X.X.
	0x0b5b,	// E( 8, 13) = 0x0b5b  XX.XX.X.XX.X.
	0x0db7,	// E( 9, 13) = 0x0db7  XXX.XX.XX.XX.
	0x0eef,	// E(10, 13) = 0x0eef  XXXX.XXX.XXX.
	0x0fbf,	// E(11, 13) = 0x0fbf  XXXXXX.XXXXX.
	0x0fff,	// E(12, 13) = 0x0fff  XXXXXXXXXXXX.
	0x1fff,	// E(13, 13) = 0x1fff  XXXXXXXXXXXXX

	0x0000,	// E( 0, 14) = 0x0000  ..............
	0x0001,	// E( 1, 14) = 0x0001  X.............
	0x0081,	// E( 2, 14) = 0x0081  X......X......
	0x0211,	// E( 3, 14) = 0x0211  X...X....X....
	0x0489,	// E( 4, 14) = 0x0489  X..X...X..X...
	0x0925,	// E( 5, 14) = 0x0925  X.X..X..X..X..
	0x0a95,	// E( 6, 14) = 0x0a95  X.X.X..X.X.X..
	0x1555,	// E( 7, 14) = 0x1555  X.X.X.X.X.X.X.
	0x15ab,	// E( 8, 14) = 0x15ab  XX.X.X.XX.X.X.
	0x16db,	// E( 9, 14) = 0x16db  XX.XX.XX.XX.X.
	0x1bb7,	// E(10, 14) = 0x1bb7  XXX.XX.XXX.XX.
	0x1def,	// E(11, 14) = 0x1def  XXXX.XXXX.XXX.
	0x1fbf,	// E(12, 14) = 0x1fbf  XXXXXX.XXXXXX.
	0x1fff,	// E(13, 14) = 0x1fff  XXXXXXXXXXXXX.
	0x3fff,	// E(14, 14) = 0x3fff  XXXXXXXXXXXXXX

	0x0000,	// E( 0, 15) = 0x0000  ...............
	0x0001,	// E( 1, 15) = 0x0001  X..............
	0x0081,	// E( 2, 15) = 0x0081  X......X.......
	0x0421,	// E( 3, 15) = 0x0421  X....X....X....
	0x0889,	// E( 4, 15) = 0x0889  X..X...X...X...
	0x1249,	// E( 5, 15) = 0x1249  X..X..X..X..X..
	0x14a5,	// E( 6, 15) = 0x14a5  X.X..X.X..X.X..
	0x1555,	// E( 7, 15) = 0x1555  X.X.X.X.X.X.X..
	0x2aab,	// E( 8, 15) = 0x2aab  XX.X.X.X.X.X.X.
	0x2d6b,	// E( 9, 15) = 0x2d6b  XX.X.XX.X.XX.X.
	0x36db,	// E(10, 15) = 0x36db  XX.XX.XX.XX.XX.
	0x3777,	// E(11, 15) = 0x3777  XXX.XXX.XXX.XX.
	0x3def,	// E(12, 15) = 0x3def  XXXX.XXXX.XXXX.
	0x3f7f,	// E(13, 15) = 0x3f7f  XXXXXXX.XXXXXX.
	0x3fff,	// E(14, 15) = 0x3fff  XXXXXXXXXXXXXX.
	0x7fff,	// E(15, 15) = 0x7fff  XXXXXXXXXXXXXXX

	0x0000,	// E( 0, 16) = 0x0000  ................
	0x0001,	// E( 1, 16) = 0x0001  X...............
	0x0101,	// E( 2, 16) = 0x0101  X.......X.......
	0x0421,	// E( 3, 16) = 0x0421  X....X....X.....
	0x1111,	// E( 4, 16) = 0x1111  X...X...X...X...
	0x1249,	// E( 5, 16) = 0x1249  X..X..X..X..X...
	0x2525,	// E( 6, 16) = 0x2525  X.X..X..X.X..X..
	0x2a55,	// E( 7, 16) = 0x2a55  X.X.X.X..X.X.X..
	0x5555,	// E( 8, 16) = 0x5555  X.X.X.X.X.X.X.X.
	0x55ab,	// E( 9, 16) = 0x55ab  XX.X.X.XX.X.X.X.
	0x5b5b,	// E(10, 16) = 0x5b5b  XX.XX.X.XX.XX.X.
	0x6db7,	// E(11, 16) = 0x6db7  XXX.XX.XX.XX.XX.
	0x7777,	// E(12, 16) = 0x7777  XXX.XXX.XXX.XXX.
	0x7bdf,	// E(13, 16) = 0x7bdf  XXXXX.XXXX.XXXX.
	0x7f7f,	// E(14, 16) = 0x7f7f  XXXXXXX.XXXXXXX.
	0x7fff,	// E(15, 16) = 0x7fff  XXXXXXXXXXXXXXX.
	0xffff,	// E(16, 16) = 0xffff  XXXXXXXXXXXXXXXX
};

unsigned int euclideanRhythm(unsigned char k, unsigned char n)
{
	return  euclideanBits[tableOffset[n] + k];
}

