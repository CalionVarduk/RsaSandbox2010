#pragma once

#include "RSAInt.h"
#include "SHA512Password.h"
#include "AES256.h"

namespace RsaSandbox
{
	public ref class RSAKey
	{
		public:
			property bool IsPasswordProtected {
				public: bool get() { return _protected; }
			}

			property bool AreExponentsSet {
				public: bool get() { return (e != BigInteger::Zero); }
			}

			property Int32 BitLength {
				public: Int32 get() { return _bitLength; }
			}

			property Int32 BlockLength {
				public: Int32 get() { return _blockLength; }
			}

			property BigInteger Modulus {
				public: BigInteger get() { return n; }
			}

			property BigInteger PublicExponent {
				public: BigInteger get() { return e; }
			}

			property bool IsFermatSafe {
				public: bool get() { return _fermatSafe; }
			}

			property bool IsPublicSafe {
				public: bool get() { return _publicSafe; }
			}

			property bool IsWienerSafe {
				public: bool get() { return _wienerSafe; }
			}

			RSAKey(BigInteger% p, BigInteger% q);
			RSAKey(RSAKey^ key);

			bool trySetExponents(BigInteger% e);
			void invertExponents();

			void setPassword(String^ password);
			void setPassword(String^ oldPassword, String^ newPassword);
			void removePassword(String^ password);

			bool authenticate(String^ password);

			BigInteger getPrivateExponent();
			BigInteger getPrivateExponent(String^ password);

			void getPrimes(BigInteger% p, BigInteger% q);
			void getPrimes(BigInteger% p, BigInteger% q, String^ password);

			void getPrivateExponents(BigInteger% d, BigInteger% dp, BigInteger% dq);
			void getPrivateExponents(BigInteger% d, BigInteger% dp, BigInteger% dq, String^ password);

			void getCRTDecryptionKey(BigInteger% p, BigInteger% q, BigInteger% dp, BigInteger% dq, BigInteger% qinv);
			void getCRTDecryptionKey(BigInteger% p, BigInteger% q, BigInteger% dp, BigInteger% dq, BigInteger% qinv, String^ password);

			void getAllPrivateComponents(BigInteger% p, BigInteger% q, BigInteger% d, BigInteger% dp, BigInteger% dq, BigInteger% qinv);
			void getAllPrivateComponents(BigInteger% p, BigInteger% q,  BigInteger% d, BigInteger% dp, BigInteger% dq, BigInteger% qinv, String^ password);

		private:
			bool _protected, _fermatSafe, _wienerSafe, _publicSafe;
			Int32 _bitLength, _blockLength;
			BigInteger n, e;
			array<Byte> ^p, ^q, ^d, ^dp, ^dq, ^qinv;
			SHA512Password^ _pswrd;

			RSAKey(array<array<Byte>^>^ nums, Byte flags);

			void _setBlockLength();
			void _setFermatSafeness(BigInteger% p, BigInteger% q);
			void _setPublicSafeness();
			void _setWienerSafeness(BigInteger% d);

			void _openPassword(String^ password);
			void _closePassword();
			void _checkKeyIntegrity(String^ password);

			void _encryptKey(String^ password);
			void _decryptKey(String^ password);
	};
}