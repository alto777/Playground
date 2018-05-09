# include <stdio.h>

void build(char);
unsigned int compute(unsigned char, unsigned char);

unsigned int bits;

int main()
{
    printf("Hello World");

unsigned char nPulses;
unsigned char nLevels;


//for (unsigned int ii = 0; ii < 500000; ii++)
for (nLevels = 0; nLevels <= 16; nLevels++) {
	for (nPulses = 0; nPulses <= nLevels; nPulses++) {
		compute(nPulses, nLevels);
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

unsigned char count[32];
unsigned char remainder[32];
unsigned char pos = 0;

void build(char level)
{
	if(level == -1) { // !    pos++;
		bits &= ~(1UL << pos++);
    } 
    else if (level == -2) {
		bits |= 1UL << pos++;
	} 
	else { 
		for(unsigned char i = 0; i < count[level]; i++)
			build(level-1); 
		if(remainder[level] != 0)
			build(level-2); 
	}
}

unsigned int compute(unsigned char pulses, unsigned char slots)
{
	unsigned char  divisor = slots - pulses;
	char level = 0;
	unsigned char cycleLength = 1; 
	unsigned char remLength = 1;

	bits = 0UL;
	pos = 0;

	if (!pulses)
		return bits;

    /* Figure 11 */
    remainder[0] = pulses; 
	do { 
		count[level] = divisor / remainder[level]; 
		remainder[level + 1] = divisor % remainder[level]; 
		divisor = remainder[level]; 
		unsigned char newLength = (cycleLength * count[level]) + remLength; 
		remLength = cycleLength; 
		cycleLength = newLength; 
		level = level + 1;
	} while (remainder[level] > 1);

	count[level] = divisor; 
	if (remainder[level] > 0)
		cycleLength = (cycleLength * count[level]) + remLength;
	build(level);
	return bits;
}

