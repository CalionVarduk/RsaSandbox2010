#pragma once

namespace RsaSandbox
{
	using namespace System;
	using namespace System::Numerics;

	public ref class RSAInt abstract
	{
		public:
			static UInt64 toUInt64(BigInteger% n);
			static Int32 bitCount(BigInteger% n);
			static Int32 byteCount(BigInteger% n);

			static bool getBit(array<Byte>^ n, UInt32 bit);

			static BigInteger modularInverse(BigInteger% n, BigInteger% mod);
			static BigInteger intSqrt(BigInteger% n);
			static BigInteger nthRoot(BigInteger% a, UInt32 n);
	};
}