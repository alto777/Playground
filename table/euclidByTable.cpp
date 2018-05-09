# include <stdio.h>

/*
Euclidean Rhythm Lookup Table

Code from Rebel Technology (http://www.rebeltech.org/) Euclidean Sequencer
algorithm from Godfried Toussaint, I assume.

All code published under the Gnu GPL v2 unless otherwise stated.
*/

/* test main should produce identical results to generative program */

unsigned int bits;
unsigned int euclideanRhythm(unsigned char, unsigned char);

int main()
{
    printf("Hello World");

unsigned char nPulses;
unsigned char nLevels;


//for (unsigned int ii = 0; ii < 500000; ii++)
for (nLevels = 0; nLevels <= 16; nLevels++) {
	for (nPulses = 0; nPulses <= nLevels; nPulses++) {
//		compute(nPulses, nLevels);

bits = euclideanRhythm(nPulses, nLevels);

		printf("\n\t0x%04x,\t", bits);
		printf("/* E(%2d, %2d) = 0x%04x  ", nPulses, nLevels, bits);
		
		for (int i = 0; i < nLevels; i++, bits >>= 1)
			printf("%s", (bits & 0x1) ? "X" : ".");

		printf(" */");	
	}
	printf("\n");
}


    return 0;
}


static const unsigned char tableOffset[17] = {
	0, 1, 3, 6,
	10, 15, 21, 28,
	36, 45, 55, 66,
	78, 91, 105, 120,
	136,
};

static const unsigned int euclideanBits[153] = {
	0x0000,	/* E( 0,  0) = 0x0000   */

	0x0000,	/* E( 0,  1) = 0x0000  . */
	0x0001,	/* E( 1,  1) = 0x0001  X */

	0x0000,	/* E( 0,  2) = 0x0000  .. */
	0x0002,	/* E( 1,  2) = 0x0002  .X */
	0x0003,	/* E( 2,  2) = 0x0003  XX */

	0x0000,	/* E( 0,  3) = 0x0000  ... */
	0x0004,	/* E( 1,  3) = 0x0004  ..X */
	0x0003,	/* E( 2,  3) = 0x0003  XX. */
	0x0007,	/* E( 3,  3) = 0x0007  XXX */

	0x0000,	/* E( 0,  4) = 0x0000  .... */
	0x0008,	/* E( 1,  4) = 0x0008  ...X */
	0x000a,	/* E( 2,  4) = 0x000a  .X.X */
	0x0007,	/* E( 3,  4) = 0x0007  XXX. */
	0x000f,	/* E( 4,  4) = 0x000f  XXXX */

	0x0000,	/* E( 0,  5) = 0x0000  ..... */
	0x0010,	/* E( 1,  5) = 0x0010  ....X */
	0x000a,	/* E( 2,  5) = 0x000a  .X.X. */
	0x0015,	/* E( 3,  5) = 0x0015  X.X.X */
	0x000f,	/* E( 4,  5) = 0x000f  XXXX. */
	0x001f,	/* E( 5,  5) = 0x001f  XXXXX */

	0x0000,	/* E( 0,  6) = 0x0000  ...... */
	0x0020,	/* E( 1,  6) = 0x0020  .....X */
	0x0024,	/* E( 2,  6) = 0x0024  ..X..X */
	0x002a,	/* E( 3,  6) = 0x002a  .X.X.X */
	0x001b,	/* E( 4,  6) = 0x001b  XX.XX. */
	0x001f,	/* E( 5,  6) = 0x001f  XXXXX. */
	0x003f,	/* E( 6,  6) = 0x003f  XXXXXX */

	0x0000,	/* E( 0,  7) = 0x0000  ....... */
	0x0040,	/* E( 1,  7) = 0x0040  ......X */
	0x0024,	/* E( 2,  7) = 0x0024  ..X..X. */
	0x002a,	/* E( 3,  7) = 0x002a  .X.X.X. */
	0x0055,	/* E( 4,  7) = 0x0055  X.X.X.X */
	0x005b,	/* E( 5,  7) = 0x005b  XX.XX.X */
	0x003f,	/* E( 6,  7) = 0x003f  XXXXXX. */
	0x007f,	/* E( 7,  7) = 0x007f  XXXXXXX */

	0x0000,	/* E( 0,  8) = 0x0000  ........ */
	0x0080,	/* E( 1,  8) = 0x0080  .......X */
	0x0088,	/* E( 2,  8) = 0x0088  ...X...X */
	0x0092,	/* E( 3,  8) = 0x0092  .X..X..X */
	0x00aa,	/* E( 4,  8) = 0x00aa  .X.X.X.X */
	0x006d,	/* E( 5,  8) = 0x006d  X.XX.XX. */
	0x0077,	/* E( 6,  8) = 0x0077  XXX.XXX. */
	0x007f,	/* E( 7,  8) = 0x007f  XXXXXXX. */
	0x00ff,	/* E( 8,  8) = 0x00ff  XXXXXXXX */

	0x0000,	/* E( 0,  9) = 0x0000  ......... */
	0x0100,	/* E( 1,  9) = 0x0100  ........X */
	0x0088,	/* E( 2,  9) = 0x0088  ...X...X. */
	0x0124,	/* E( 3,  9) = 0x0124  ..X..X..X */
	0x00aa,	/* E( 4,  9) = 0x00aa  .X.X.X.X. */
	0x0155,	/* E( 5,  9) = 0x0155  X.X.X.X.X */
	0x00db,	/* E( 6,  9) = 0x00db  XX.XX.XX. */
	0x0177,	/* E( 7,  9) = 0x0177  XXX.XXX.X */
	0x00ff,	/* E( 8,  9) = 0x00ff  XXXXXXXX. */
	0x01ff,	/* E( 9,  9) = 0x01ff  XXXXXXXXX */

	0x0000,	/* E( 0, 10) = 0x0000  .......... */
	0x0200,	/* E( 1, 10) = 0x0200  .........X */
	0x0210,	/* E( 2, 10) = 0x0210  ....X....X */
	0x0124,	/* E( 3, 10) = 0x0124  ..X..X..X. */
	0x014a,	/* E( 4, 10) = 0x014a  .X.X..X.X. */
	0x02aa,	/* E( 5, 10) = 0x02aa  .X.X.X.X.X */
	0x02b5,	/* E( 6, 10) = 0x02b5  X.X.XX.X.X */
	0x02db,	/* E( 7, 10) = 0x02db  XX.XX.XX.X */
	0x01ef,	/* E( 8, 10) = 0x01ef  XXXX.XXXX. */
	0x01ff,	/* E( 9, 10) = 0x01ff  XXXXXXXXX. */
	0x03ff,	/* E(10, 10) = 0x03ff  XXXXXXXXXX */

	0x0000,	/* E( 0, 11) = 0x0000  ........... */
	0x0400,	/* E( 1, 11) = 0x0400  ..........X */
	0x0210,	/* E( 2, 11) = 0x0210  ....X....X. */
	0x0444,	/* E( 3, 11) = 0x0444  ..X...X...X */
	0x0492,	/* E( 4, 11) = 0x0492  .X..X..X..X */
	0x02aa,	/* E( 5, 11) = 0x02aa  .X.X.X.X.X. */
	0x0555,	/* E( 6, 11) = 0x0555  X.X.X.X.X.X */
	0x036d,	/* E( 7, 11) = 0x036d  X.XX.XX.XX. */
	0x03bb,	/* E( 8, 11) = 0x03bb  XX.XXX.XXX. */
	0x05ef,	/* E( 9, 11) = 0x05ef  XXXX.XXXX.X */
	0x03ff,	/* E(10, 11) = 0x03ff  XXXXXXXXXX. */
	0x07ff,	/* E(11, 11) = 0x07ff  XXXXXXXXXXX */

	0x0000,	/* E( 0, 12) = 0x0000  ............ */
	0x0800,	/* E( 1, 12) = 0x0800  ...........X */
	0x0820,	/* E( 2, 12) = 0x0820  .....X.....X */
	0x0888,	/* E( 3, 12) = 0x0888  ...X...X...X */
	0x0924,	/* E( 4, 12) = 0x0924  ..X..X..X..X */
	0x094a,	/* E( 5, 12) = 0x094a  .X.X..X.X..X */
	0x0aaa,	/* E( 6, 12) = 0x0aaa  .X.X.X.X.X.X */
	0x06b5,	/* E( 7, 12) = 0x06b5  X.X.XX.X.XX. */
	0x06db,	/* E( 8, 12) = 0x06db  XX.XX.XX.XX. */
	0x0777,	/* E( 9, 12) = 0x0777  XXX.XXX.XXX. */
	0x07df,	/* E(10, 12) = 0x07df  XXXXX.XXXXX. */
	0x07ff,	/* E(11, 12) = 0x07ff  XXXXXXXXXXX. */
	0x0fff,	/* E(12, 12) = 0x0fff  XXXXXXXXXXXX */

	0x0000,	/* E( 0, 13) = 0x0000  ............. */
	0x1000,	/* E( 1, 13) = 0x1000  ............X */
	0x0820,	/* E( 2, 13) = 0x0820  .....X.....X. */
	0x0888,	/* E( 3, 13) = 0x0888  ...X...X...X. */
	0x0924,	/* E( 4, 13) = 0x0924  ..X..X..X..X. */
	0x0a52,	/* E( 5, 13) = 0x0a52  .X..X.X..X.X. */
	0x0aaa,	/* E( 6, 13) = 0x0aaa  .X.X.X.X.X.X. */
	0x1555,	/* E( 7, 13) = 0x1555  X.X.X.X.X.X.X */
	0x15ad,	/* E( 8, 13) = 0x15ad  X.XX.X.XX.X.X */
	0x16db,	/* E( 9, 13) = 0x16db  XX.XX.XX.XX.X */
	0x1777,	/* E(10, 13) = 0x1777  XXX.XXX.XXX.X */
	0x17df,	/* E(11, 13) = 0x17df  XXXXX.XXXXX.X */
	0x0fff,	/* E(12, 13) = 0x0fff  XXXXXXXXXXXX. */
	0x1fff,	/* E(13, 13) = 0x1fff  XXXXXXXXXXXXX */

	0x0000,	/* E( 0, 14) = 0x0000  .............. */
	0x2000,	/* E( 1, 14) = 0x2000  .............X */
	0x2040,	/* E( 2, 14) = 0x2040  ......X......X */
	0x2108,	/* E( 3, 14) = 0x2108  ...X....X....X */
	0x1224,	/* E( 4, 14) = 0x1224  ..X..X...X..X. */
	0x2492,	/* E( 5, 14) = 0x2492  .X..X..X..X..X */
	0x152a,	/* E( 6, 14) = 0x152a  .X.X.X..X.X.X. */
	0x2aaa,	/* E( 7, 14) = 0x2aaa  .X.X.X.X.X.X.X */
	0x2ad5,	/* E( 8, 14) = 0x2ad5  X.X.X.XX.X.X.X */
	0x1b6d,	/* E( 9, 14) = 0x1b6d  X.XX.XX.XX.XX. */
	0x2ddb,	/* E(10, 14) = 0x2ddb  XX.XX.XXX.XX.X */
	0x1ef7,	/* E(11, 14) = 0x1ef7  XXX.XXXX.XXXX. */
	0x1fbf,	/* E(12, 14) = 0x1fbf  XXXXXX.XXXXXX. */
	0x1fff,	/* E(13, 14) = 0x1fff  XXXXXXXXXXXXX. */
	0x3fff,	/* E(14, 14) = 0x3fff  XXXXXXXXXXXXXX */

	0x0000,	/* E( 0, 15) = 0x0000  ............... */
	0x4000,	/* E( 1, 15) = 0x4000  ..............X */
	0x2040,	/* E( 2, 15) = 0x2040  ......X......X. */
	0x4210,	/* E( 3, 15) = 0x4210  ....X....X....X */
	0x4444,	/* E( 4, 15) = 0x4444  ..X...X...X...X */
	0x4924,	/* E( 5, 15) = 0x4924  ..X..X..X..X..X */
	0x294a,	/* E( 6, 15) = 0x294a  .X.X..X.X..X.X. */
	0x2aaa,	/* E( 7, 15) = 0x2aaa  .X.X.X.X.X.X.X. */
	0x5555,	/* E( 8, 15) = 0x5555  X.X.X.X.X.X.X.X */
	0x56b5,	/* E( 9, 15) = 0x56b5  X.X.XX.X.XX.X.X */
	0x36db,	/* E(10, 15) = 0x36db  XX.XX.XX.XX.XX. */
	0x3bbb,	/* E(11, 15) = 0x3bbb  XX.XXX.XXX.XXX. */
	0x3def,	/* E(12, 15) = 0x3def  XXXX.XXXX.XXXX. */
	0x5fbf,	/* E(13, 15) = 0x5fbf  XXXXXX.XXXXXX.X */
	0x3fff,	/* E(14, 15) = 0x3fff  XXXXXXXXXXXXXX. */
	0x7fff,	/* E(15, 15) = 0x7fff  XXXXXXXXXXXXXXX */

	0x0000,	/* E( 0, 16) = 0x0000  ................ */
	0x8000,	/* E( 1, 16) = 0x8000  ...............X */
	0x8080,	/* E( 2, 16) = 0x8080  .......X.......X */
	0x4210,	/* E( 3, 16) = 0x4210  ....X....X....X. */
	0x8888,	/* E( 4, 16) = 0x8888  ...X...X...X...X */
	0x4924,	/* E( 5, 16) = 0x4924  ..X..X..X..X..X. */
	0x9292,	/* E( 6, 16) = 0x9292  .X..X..X.X..X..X */
	0x952a,	/* E( 7, 16) = 0x952a  .X.X.X..X.X.X..X */
	0xaaaa,	/* E( 8, 16) = 0xaaaa  .X.X.X.X.X.X.X.X */
	0x6ad5,	/* E( 9, 16) = 0x6ad5  X.X.X.XX.X.X.XX. */
	0x6d6d,	/* E(10, 16) = 0x6d6d  X.XX.XX.X.XX.XX. */
	0xb6db,	/* E(11, 16) = 0xb6db  XX.XX.XX.XX.XX.X */
	0x7777,	/* E(12, 16) = 0x7777  XXX.XXX.XXX.XXX. */
	0xbdef,	/* E(13, 16) = 0xbdef  XXXX.XXXX.XXXX.X */
	0x7f7f,	/* E(14, 16) = 0x7f7f  XXXXXXX.XXXXXXX. */
	0x7fff,	/* E(15, 16) = 0x7fff  XXXXXXXXXXXXXXX. */
	0xffff,	/* E(16, 16) = 0xffff  XXXXXXXXXXXXXXXX */
};

unsigned int euclideanRhythm(unsigned char pulses, unsigned char slots)
{
	return  euclideanBits[tableOffset[slots] + pulses];
}
