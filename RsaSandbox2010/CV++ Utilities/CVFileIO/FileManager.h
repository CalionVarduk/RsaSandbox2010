#pragma once

#include "CVAwfulCipher.h"
#include "HuffmanCompression.h"

namespace CVFileIO
{
	using namespace System;
	using namespace System::IO;

	public ref class FileManager
	{
		public:
			FileManager();

			void setEncryptKey(String^ key, bool semiRandomize);
			void setEncryptKey(array<Byte>^ key);
			array<Byte>^ getEncryptKey();

			void setCompressState(bool on);
			bool getCompressState();

			bool eligibleForEncryption();
			bool setEncryptionState(bool on);
			bool getEncryptionState();

			void setSignature(String^ signature);
			void setSignature(array<Byte>^ signature);
			array<Byte>^ getSignature();

			Int32 getSignatureLength();
			Int32 getEncryptKeyLength();

			bool save(String^ path, array<Byte>^ data);
			bool load(String^ path, array<Byte>^% OUT_data);

		private:
			CVAwfulCipher^ _cipher;
			array<Byte>^ _signature;
			bool _encrypt;
			bool _compress;

			bool _isSignatureCorrect(array<Byte>^ signature);
	};
}