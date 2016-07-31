#pragma once

#include "SizableHash.h"
#include "RSALengthCalculator.h"

namespace RsaSandbox
{
	namespace Ciphering
	{
		using namespace System;
		using namespace System::Security::Cryptography;
		using namespace RsaSandbox;

		public ref class OAEPPadder abstract
		{
			public:
				static array<Byte>^ encryptPad(RSAKey^ key, array<Byte>^ data);
				static array<Byte>^ decryptPad(RSAKey^ key, array<Byte>^ data);

			private:
				ref class OAEPContainer
				{
					public:
						array<Byte> ^output;

						static OAEPContainer^ CreateEncryptor(RSAKey^ key, array<Byte>^ data);
						static OAEPContainer^ CreateDecryptor(RSAKey^ key, array<Byte>^ data);

						void fillNextBlock(Int32 iByte);
						void fillLastBlock(Int32 iByte);

						void encryptNextBlock();
						void decryptNextBlock();

						void parseInputLength();
						void fixDataAfterDecryption();
						void cleanUp();

					private:
						static RandomNumberGenerator^ _rng = gcnew RNGCryptoServiceProvider();
						Int32 iOutput, iInput;
						array<Byte> ^input, ^msg, ^rand, ^X, ^Y;
						SizableHash ^shrinker, ^expander;

						OAEPContainer();
				};
		};
	}
}