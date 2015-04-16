#include "gamma.h"

uint encode(uint* output, uint pos, uint value) {
	value++;
	uint l = logb2(value);
	uint i;	
	for(i = 0; i < l; i++) {
		mybitclean(output,pos);
		pos++;
	}
	mybitset(output,pos);
	pos++;
	for(i = 0; i < l; i++) {
		if(value & (1 << i))
			mybitset(output,pos);
		else
			mybitclean(output,pos);
		pos++;
	}
	
	return 2*l+1;
}

uint decode(uint* input, uint pos, uint* value) {
	uint nbits = 0;
	uint lValue = 0;
	register uint i;
	while(!mybitget(input,pos)) {
		nbits++;
		pos++;
	}
	pos++;
	for(i = 0; i < nbits; i++) {
		if(mybitget(input,pos))
			lValue += 1 << i;
		pos++;
	}
	lValue |= (1 << nbits);

	*value = lValue - 1;
	return 2*nbits+1;	
}

