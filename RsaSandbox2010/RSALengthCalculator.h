#pragma once

#include "RSAKey.h"

namespace RsaSandbox
{
	namespace Ciphering
	{
		using namespace System;
		using namespace RsaSandbox;

		public ref class RSALengthCalculator abstract
		{
			public:
				static Int32 getEncryptBlockCount(RSAKey^ key, Int32 dataLength);
				static Int32 getEncryptCipherLength(RSAKey^ key, Int32 dataLength);
				static void getEncryptLengths(RSAKey^ key, Int32 dataLength, Int32% OUT_blockCount, Int32% OUT_cipherLength);

				static bool isDecryptionPossible(RSAKey^ key, Int32 cipherLength);
				static Int32 getDecryptBlockCount(RSAKey^ key, Int32 cipherLength);
				static Int32 getDecryptDataLength(RSAKey^ key, Int32 cipherLength);
				static bool getDecryptLengths(RSAKey^ key, Int32 cipherLength, Int32% OUT_blockCount, Int32% OUT_dataLength);

				static Int32 getOAEPkLength(RSAKey^ key);
				static Int32 getOAEPMessageLength(RSAKey^ key);
				static void getOAEPLengths(RSAKey^ key, Int32 dataLength, Int32% OUT_kLength, Int32% OUT_msgLength);

				static Int32 getOAEPEncryptBlockCount(RSAKey^ key, Int32 dataLength);
				static Int32 getOAEPEncryptOutputLength(RSAKey^ key, Int32 dataLength);
				static Int32 getOAEPEncryptCipherLength(RSAKey^ key, Int32 dataLength);
				static void getOAEPEncryptLengths(RSAKey^ key, Int32 dataLength, Int32% OUT_blockCount, Int32% OUT_outputLength, Int32% OUT_cipherLength);
				static void getOAEPEncryptLengths(RSAKey^ key, Int32 dataLength, Int32% OUT_kLength, Int32% OUT_msgLength, Int32% OUT_blockCount, Int32% OUT_outputLength, Int32% OUT_cipherLength);

				static Int32 getOAEPDecryptBlockCount(RSAKey^ key, Int32 cipherLength);
				static Int32 getOAEPDecryptOutputLength(RSAKey^ key, Int32 cipherLength);
				static Int32 getOAEPDecryptDataLength(RSAKey^ key, Int32 cipherLength);
				static bool getOAEPDecryptLengths(RSAKey^ key, Int32 dataLength, Int32% OUT_blockCount, Int32% OUT_outputLength, Int32% OUT_dataLength);
				static bool getOAEPDecryptLengths(RSAKey^ key, Int32 dataLength, Int32% OUT_kLength, Int32% OUT_msgLength, Int32% OUT_blockCount, Int32% OUT_outputLength, Int32% OUT_dataLength);
		};
	}
}