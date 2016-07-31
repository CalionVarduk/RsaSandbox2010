#include "RSAInt.h"

using namespace RsaSandbox;

UInt64 RSAInt::toUInt64(BigInteger% n)
{
	array<Byte>^ buffer = n.ToByteArray();
	Byte nBytes = (buffer->Length >= 8) ? 8 : (buffer->Length - 1);
	UInt64 result = buffer[0];
	for(Byte i = 1; i < nBytes; ++i) result |= ((UInt64)buffer[i] << (i << 3));
	return result;
}

Int32 RSAInt::bitCount(BigInteger% n)
{
	array<Byte>^ buffer = n.ToByteArray();
	Int32 nBytes = buffer->Length - 1;
	if(buffer[nBytes] == 0) --nBytes;
	Int32 nBits = (nBytes << 3) + 1;

	while(buffer[nBytes] > 1) {
		buffer[nBytes] >>= 1;
		++nBits;
	}
	return nBits;
}

Int32 RSAInt::byteCount(BigInteger% n)
{
	return (bitCount(n) + 7) >> 3;
}

bool RSAInt::getBit(array<Byte>^ n, UInt32 bit)
{
	return (((n[bit >> 3] >> (bit & 7)) & 1) == 1);
}

BigInteger RSAInt::modularInverse(BigInteger% n, BigInteger% mod)
{
	BigInteger a = mod;
	BigInteger b = n;
	BigInteger r = 2LL, q, t;
	BigInteger u0 = BigInteger::Zero, u1 = BigInteger::One;

	while(r > BigInteger::One) {
		q = BigInteger::DivRem(a, b, r);
		a = b;
		b = r;

		t = BigInteger::Subtract(u0, (q * u1));
		u0 = u1;
		u1 = t;
	}
	return (u1 < BigInteger::Zero) ? BigInteger::Add(u1, mod) : u1;
}

BigInteger RSAInt::intSqrt(BigInteger% n)
{
	if(n < BigInteger::Zero) return -1;

	Int32 bits = bitCount(n);
	BigInteger result = BigInteger::Zero;
	BigInteger bit = BigInteger::One;
	BigInteger aux = n;
		
	bit <<= (bits - 1);
	if((bits & 1) == 0) bit <<= 1;

	while(bit != BigInteger::Zero) {
		BigInteger sum = BigInteger::Add(result, bit);
		if(aux >= sum) {
			aux = BigInteger::Subtract(aux, sum);
			result = BigInteger::Add((result >> 1), bit);
		}
		else result >>= 1;
		bit >>= 2;
	}
	return result;
}

BigInteger RSAInt::nthRoot(BigInteger% a, UInt32 n)
{
	Int32 bits = bitCount(a) / n;
	BigInteger l = (bits > 0) ? (BigInteger::One << (bits - 1)) : BigInteger::One;
	BigInteger r = BigInteger::One << (bits + 1);

	while(l <= r) {
		BigInteger k = BigInteger::Add(l, r) >> 1;
		BigInteger p = BigInteger::Pow(k, n);

		if(p < a) l = BigInteger::Add(k, 1);
		else if(p > a) r = BigInteger::Subtract(k, 1);
		else return k;
	}
	return r;
}