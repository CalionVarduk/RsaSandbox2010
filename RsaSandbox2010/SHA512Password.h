#pragma once

namespace RsaSandbox
{
	using namespace System;
	using namespace System::Text;
	using namespace System::Security::Cryptography;

	public ref class SHA512Password
	{
		public:
			property array<Byte>^ Salt {
				public: array<Byte>^ get() { return (array<Byte>^)_salt->Clone(); }
			}

			property array<Byte>^ Hash {
				public: array<Byte>^ get() { return (array<Byte>^)_hash->Clone(); }
			}

			SHA512Password();
			SHA512Password(String^ password);
			SHA512Password(array<Byte>^ salt);
			SHA512Password(String^ password, array<Byte>^ salt);

			bool change(String^ oldPassword, String^ newPassword);
			bool change(String^ oldPassword, String^ newPassword, array<Byte>^ newSalt);
			bool authenticate(String^ password);

		private:
			static RandomNumberGenerator^ rng = gcnew RNGCryptoServiceProvider();
			array<Byte>^ _hash;
			array<Byte>^ _salt;

			void _generateSalt();
			void _setSalt(array<Byte>^ salt);

			array<Byte>^ _getHash(String^ s);
			array<Byte>^ _getHash(array<Byte>^ b);

			void _setPassword(String^ password);
			void _setPassword(String^ password, array<Byte>^ salt);
	};
}