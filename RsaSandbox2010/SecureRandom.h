#pragma once

#include "RSAInt.h"

namespace RsaSandbox
{
	using namespace System;
	using namespace System::Numerics;
	using namespace System::Security::Cryptography;

	public ref class SecureRandom
	{
		public:
			SecureRandom();

			BigInteger getBigInteger(Int32 bitCount);
			UInt32 getUInt32(UInt32 min, UInt32 max);
			array<Byte>^ getBytes(Int32 length);

		private:
			RandomNumberGenerator^ _rng;
	};
}