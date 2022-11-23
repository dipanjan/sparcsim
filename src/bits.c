#include <bits.h>



// Returns a single bit at a specified position of a 32-bit bitStream
int getBit(unsigned long bitStream, int position)
{
	int bit;
	bit = (bitStream & (1 << position)) >> position;
	return bit;
}



// Sets a single bit at a specified position of a 32-bit bitStream
unsigned long setBit(unsigned long bitStream, int position)
{
	unsigned long setBitStream = bitStream;
	setBitStream |= 1 << position;
	return setBitStream;
}



// Clears a single bit at a specified position of a 32-bit bitStream
unsigned long clearBit(unsigned long bitStream, int position)
{
	unsigned long clearedBitStream = bitStream;
	clearedBitStream &= ~(1 << position);
	return clearedBitStream;
}



// Toggles a single bit at a specified position of a 32-bit bitStream
unsigned long toggleBit(unsigned long bitStream, int position)
{
	unsigned long toggledBitStream = bitStream;
	toggledBitStream ^= 1 << position;
	return toggledBitStream;
}



// Returns a pointer to a string containing bytes within startPosition and endPosition extracted from 32-bit bitStream
char* showBits(unsigned long bitStream, int startPosition, int endPosition)
{
	// Allocate one extra byte for NULL character
	char* bits = (char*)malloc(endPosition - startPosition + 2);
	int bitIndex;
	for(bitIndex = 0; bitIndex <= endPosition; bitIndex++)
		bits[bitIndex] = (getBit(bitStream, endPosition - bitIndex)) ? '1' : '0';
	bits[bitIndex] = '\0';
	return bits;
}
