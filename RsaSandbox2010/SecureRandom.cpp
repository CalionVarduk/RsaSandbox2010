#include "SecureRandom.h"

using namespace RsaSandbox;

SecureRandom::SecureRandom()
{
	_rng = gcnew RNGCryptoServiceProvider();
}

BigInteger SecureRandom::getBigInteger(Int32 bitCount)
{
	Int32 nBytes = (bitCount + 7) >> 3;
	array<Byte>^ buffer = gcnew array<Byte>((nBytes--) + 1);
	_rng->GetBytes(buffer);
							
	Byte lastBits = bitCount - (nBytes << 3);		// makes sure the number is actually bitCount long
	buffer[nBytes] |= (lastBits > 0) ? (1 << (lastBits - 1)) : 0x80;

	for(Byte i = lastBits; i < 8; ++i)				// clears out trailing bits
		buffer[nBytes] &= ~(1 << i);

	buffer[nBytes + 1] = 0;							// makes sure that BigInteger constructor interprets the data as a positive integer
	buffer[0] |= 1;									// makes sure the integer is odd

	BigInteger n = BigInteger(buffer);
	Array::Clear(buffer, 0, buffer->Length);
	return n;
}

UInt32 SecureRandom::getUInt32(UInt32 min, UInt32 max)
{
	array<Byte>^ buffer = gcnew array<Byte>(4);
	_rng->GetBytes(buffer);

	UInt32 rand = buffer[0];
	for(Byte i = 1; i < 4; ++i) rand |= ((UInt32)buffer[i] << (i << 3));
	Array::Clear(buffer, 0, buffer->Length);

	return (rand % (max - min)) + min;
}

array<Byte>^ SecureRandom::getBytes(Int32 length)
{
	array<Byte>^ bytes = gcnew array<Byte>(length);
	_rng->GetBytes(bytes);
	return bytes;
}