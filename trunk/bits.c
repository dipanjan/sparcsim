#include <bits.h>



int testBit(unsigned long bitStream, int position)
{
	int bit;
	bit = (bitStream & (1 << position)) >> position;
	return bit;
}



unsigned long setBit(unsigned long bitStream, int position)
{
	unsigned long setBitStream;
	setBitStream |= 1 << position;
	return setBitStream;
}



unsigned long clearBit(unsigned long bitStream, int position)
{
	unsigned long clearedBitStream;
	clearedBitStream &= ~(1 << position);
	return clearedBitStream;
}



unsigned long toggleBit(unsigned long bitStream, int position)
{
	unsigned long toggledBitStream;
	toggledBitStream ^= 1 << position;
	return toggledBitStream;
}



char* showBits(unsigned long bitStream, int startPosition, int endPosition)
{
	// Allocate one extra byte for NULL character
	char* bits = (char*)malloc(endPosition - startPosition + 2);
	int bitIndex;
	for(bitIndex = 0; bitIndex <= endPosition; bitIndex++)
		bits[bitIndex] = (testBit(bitStream, endPosition - bitIndex)) ? '1' : '0';
	bits[bitIndex] = '\0';
	return bits;
}



void main()
{
	printf("%d\n", testBit(0x00001FE1, 5));
	printf("%lx\n", setBit(0x1FE1, 3));
	printf("%s\n", showBits(0x00001FE1, 0, 12));
	printf("%s\n", showBits(setBit(0x00001FE1, 3), 0, 12));
	printf("%s\n", showBits(clearBit(0x00001FE1, 0), 0, 12));
	printf("%s\n", showBits(toggleBit(0x00001FE1, 3), 0, 12));
	printf("%s\n", showBits(toggleBit(0x00001FE1, 0), 0, 12));
}