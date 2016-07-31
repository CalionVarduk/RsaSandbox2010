#pragma once

namespace RsaSandbox
{
	using namespace System;
	using namespace System::IO;
	using namespace System::Security::Cryptography;

	public ref class AES256 abstract
	{
		public:
			static property PaddingMode Padding {
				public: PaddingMode get() { return padding; }
				public: void set(PaddingMode value) { padding = value; }
			}

			static property CipherMode Mode {
				public: CipherMode get() { return mode; }
				public: void set(CipherMode value) { mode = value; }
			}

			static array<Byte>^ encrypt(array<Byte>^ data, array<Byte>^ key);
			static array<Byte>^ decrypt(array<Byte>^ cipher, array<Byte>^ key);

		private:
			static PaddingMode padding = PaddingMode::PKCS7;
			static CipherMode mode = CipherMode::CBC;

			static AesCryptoServiceProvider^ _getAES(array<Byte>^ key, array<Byte>^ iv);
	};
}