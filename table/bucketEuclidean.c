# include <stdio.h>

// foundEuclidPython.c http://www.computermusicdesign.com/simplest-euclidean-rhythm-algorithm-explained/

char beatMatrix[16];
char beatLocation[16];

unsigned int bits, rrBits;

unsigned int rollReduce(unsigned int bits, unsigned char steps)
{
    if ((steps == 0) || (steps == 1)) return (bits);
    
    unsigned int topBit = 1 << (steps -1);
    unsigned int least = 0xffff;
    
    for (int k = 0; k < steps; k++) {
        if (bits & 0x1) {
            bits >>= 1;
            bits |= topBit;
        }
        else
            bits >>= 1;

        if (bits < least) least = bits;
    }
    return (least);
}

void compute(steps, pulses)
{
    for (int i = 0; i < 16; i++) {
    	beatMatrix[i] = 0;		//empty array which stores the rhythm.
    	beatLocation[i] = 0;	// empty index array
	}

    //the length of the array is equal to the number of steps
    //a value of 1 for each array element indicates a pulse

    int bucket = 0; //out variable to add pulses together for each step

    //fill array with rhythm

	int beatIndex = 0;
    for (int i = 0; i < steps; i++) { 
        bucket += pulses; 
        if (bucket >= steps) {
            bucket -= steps;
            beatMatrix[i] = 1;		//'1' indicates a pulse on this beat
            beatLocation[beatIndex++] = i;	// index it
        } 
/*        else {
            beatMatrix[i] = 0; //'0' indicates no pulse on this beat
        }
*/
    }

	if (beatIndex != pulses)
		printf(" something is wrong ");

    bits = 0;
    for (int k = 0; k < steps; k++) {
        bits <<= 1;
        bits |= beatMatrix[k] ? 1 : 0;
    }
}

int main()
{
    unsigned char nLevels, nPulses;

    printf("Hello World\n\n");
 /*  
    for (nLevels = 0; nLevels <= 4; nLevels++) {
    	for (nPulses = 0; nPulses <= nLevels; nPulses++) {
	    	compute(nLevels, nPulses);
	    	printf("\n\t0x%04x,\t", bits);
	    	printf("// E(%2d, %2d) = 0x%04x  ", nPulses, nLevels, bits);
		
	    	for (int i = 0; i < nLevels; i++, bits >>= 1)
		    	printf("%s", (bits & 0x1) ? "X" : ".");

	    	printf(" ...");
    	}
	}
	printf("\n");

 */
    printf("\n brain damage...\n");
    
    for (nLevels = 0; nLevels <= 16; nLevels++) {
    	for (nPulses = 0; nPulses <= nLevels; nPulses++) {
    
 /* */   
            compute(nLevels, nPulses);
		rrBits = rollReduce(bits, nLevels);

/* this never did - bucket method creates minimum value in bits
		if (rrBits != bits) printf("g");
*/
		printf("\n\t0x%04x,\t", rrBits);
		printf("// E(%2d, %2d) = 0x%04x  ", nPulses, nLevels, rrBits);
		
		for (int i = 0; i < nLevels; i++, rrBits >>= 1)
			printf("%s", (rrBits & 0x1) ? "X" : ".");

    	}
    	printf("\n");
    }

    return 0;
/* */
}









