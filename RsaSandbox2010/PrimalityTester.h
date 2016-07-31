#pragma once

#include "SecureRandom.h"

namespace RsaSandbox
{
	using namespace System::ComponentModel;

	public ref class PrimalityTester
	{
		public:
			property SecureRandom^ RNG {
				public: SecureRandom^ get() { return _rng; }
			}

			PrimalityTester();

			bool isPrime(BigInteger% w, UInt32 mrRounds);
			bool isSafePrime(BigInteger% w, UInt32 mrRounds);

		private:
			static array<const Byte>^ _firstPrimes = gcnew array<const Byte>{ 3, 5, 7, 11, 13, 17, 19 };

			SecureRandom^ _rng;

			bool _smallNumberTest(UInt64 w);
			bool _basicNumberDivision(BigInteger% w);
			bool _millerRabinTest(BigInteger% w, UInt32 rounds);
			bool _millerRabinSafeTest(BigInteger% w, BigInteger% s, UInt32 rounds);
			bool _singleMillerRabinTest(BigInteger% w, BigInteger% ws1, BigInteger% m, UInt32 a, UInt32 wlen);
	};
}