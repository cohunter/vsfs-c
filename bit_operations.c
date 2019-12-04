#include "constants.h"
#include "structures.h"
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>

void setBit(bitMap *myMap, uint16_t bitNumber) {
	// Each char is at least 8 bits. We have 8*BLOCK_SIZE bits in total.

	// The index of the character element we want.
	uint32_t offset = bitNumber / CHAR_BIT;

	// The bit within the character element that we are addressing.
	// Bitwise operations on signed integers are implementation-defined.
	// Always use unsigned to ensure consistency and portability.
	uint32_t bit = 1 << (bitNumber % CHAR_BIT);

	if ( offset >= BLOCK_CHAR_COUNT ) {
		printf("ERROR: out of bounds write request in setBit(map, bitnumber=%d", bitNumber);
		exit(-1);
	}

	if ( (myMap->bits[offset] & bit) == bit ) {
		printf("WARN: Bit set, but already set. Bit number: %d\n", bitNumber);
	}

	myMap->bits[offset] |= bit;
}

bool getBit(bitMap *myMap, int bitNumber) {
	uint32_t offset = bitNumber / CHAR_BIT;
	uint32_t bit = 1 << (bitNumber % CHAR_BIT);

	return (myMap->bits[offset] & bit);
}