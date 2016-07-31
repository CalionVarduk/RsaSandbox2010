#include "PrimalityTester.h"

using namespace RsaSandbox;

PrimalityTester::PrimalityTester()
{
	_rng = gcnew SecureRandom();
}

bool PrimalityTester::isPrime(BigInteger% w, UInt32 mrRounds)
{
	if(w == 2LL) return true;
	if(w < 2LL || w.IsEven) return false;
	//if(w <= 0xfffffffffffffLL) return _smallNumberTest(toUInt64(w));
	if(_basicNumberDivision(w)) return _millerRabinTest(w, mrRounds);
	return false;
}

bool PrimalityTester::isSafePrime(BigInteger% w, UInt32 mrRounds)
{
	if(w == 2LL) return true;
	if(w < 2LL || w.IsEven) return false;

	BigInteger s = BigInteger::Subtract(w, BigInteger::One) >> 1;

	if(_basicNumberDivision(w) && _basicNumberDivision(s)) return _millerRabinSafeTest(w, s, mrRounds);
	return false;
}

bool PrimalityTester::_smallNumberTest(UInt64 w)
{
	UInt64 nsqrt = (UInt64)Math::Sqrt((double)w);
	for(UInt64 i = 3; i <= nsqrt; i += 2)
		if((w % i) == 0) return false;
	return true;
}

bool PrimalityTester::_basicNumberDivision(BigInteger% w)
{
	for(Byte i = 0; i < _firstPrimes->Length; ++i)
		if((w % _firstPrimes[i]) == 0LL) return false;
	return true;
}

bool PrimalityTester::_millerRabinTest(BigInteger% w, UInt32 rounds)
{
	UInt32 wlen = RSAInt::bitCount(w);
	BigInteger ws1 = BigInteger::Subtract(w, BigInteger::One);
	UInt32 a = 1;

	array<Byte>^ buffer = ws1.ToByteArray();
	while(!RSAInt::getBit(buffer, a)) ++a;
	buffer = nullptr;

	BigInteger m = ws1 >> a;

	for(UInt32 i = 0; i < rounds; ++i)
		if(!_singleMillerRabinTest(w, ws1, m, a, wlen))
			return false;

	return true;
}

bool PrimalityTester::_millerRabinSafeTest(BigInteger% w, BigInteger% s, UInt32 rounds)
{
	UInt32 wlen = RSAInt::bitCount(w);
	BigInteger ws1 = BigInteger::Subtract(w, BigInteger::One);
	UInt32 a = 1;

	array<Byte>^ buffer = ws1.ToByteArray();
	while(!RSAInt::getBit(buffer, a)) ++a;

	BigInteger m = ws1 >> a;

	UInt32 slen = RSAInt::bitCount(s);
	BigInteger ss1 = BigInteger::Subtract(s, BigInteger::One);
	UInt32 as = 1;

	buffer = ss1.ToByteArray();
	while(!RSAInt::getBit(buffer, as)) ++as;
	buffer = nullptr;

	BigInteger ms = ss1 >> as;

	for(UInt32 i = 0; i < rounds; ++i)
		if(!_singleMillerRabinTest(s, ss1, ms, as, slen) || !_singleMillerRabinTest(w, ws1, m, a, wlen))
			return false;

	return true;
}

bool PrimalityTester::_singleMillerRabinTest(BigInteger% w, BigInteger% ws1, BigInteger% m, UInt32 a, UInt32 wlen)
{
	BigInteger b;
	do { b = _rng->getBigInteger(wlen); } while(b <= 1LL || b >= ws1);

	BigInteger z = BigInteger::ModPow(b, m, w);
	if(z == BigInteger::One || z == ws1) return true;

	for(UInt32 i = 1; i < a; ++i) {
		z *= z;
		z %= w;
		if(z == ws1) return true;
		if(z == BigInteger::One) return false;
	}
	return false;
}