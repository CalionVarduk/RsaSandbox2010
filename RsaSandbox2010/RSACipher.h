#pragma once

#include "RSALengthCalculator.h"
#include "OAEPPadder.h"
#include "SecureRandom.h"
#include "CVUtilities.h"

namespace RsaSandbox
{
	namespace Ciphering
	{
		using namespace CVStructures;

		public enum class RSACipherPaddingMode { None, OAEP };
		public enum class RSACipherDecryptionMode { Basic, CRT, BasicBlinding, CRTBlinding };
		public enum class RSACipherOutcome { OK, Cancelled, WrongDecryptionKey };

		ref class RSACipher;

		public delegate void RSACipherOutcomeHandler(RSACipher^ sender, array<Byte>^ output, RSACipherOutcome outcome);

		public ref class RSACipher : public BackgroundTask
		{
			public:
				property RSACipherPaddingMode PaddingMode {
					public: RSACipherPaddingMode get() { return _padding; }
					public: void set(RSACipherPaddingMode value) { _padding = value; }
				}

				property RSACipherDecryptionMode DecryptionMode {
					public: RSACipherDecryptionMode get() { return _decryption; }
					public: void set(RSACipherDecryptionMode value) { _decryption = value; }
				}

				property RSAKey^ Key {
					public: RSAKey^ get() { return _key; }
					public: void set(RSAKey^ value) { _key = value; }
				}

				event RSACipherOutcomeHandler^ Outcome;

				RSACipher();
				RSACipher(RSAKey^ key, RSACipherPaddingMode paddingMode, RSACipherDecryptionMode decryptionMode);

				void encrypt(array<Byte>^ data);
				void decrypt(array<Byte>^ cipher);
				void decrypt(array<Byte>^ cipher, String^ password);

			protected:
				virtual void _run() override;
				virtual void _done() override;
				virtual void _doneCancelled() override;

			private:
				enum class CipherOperatingMode { StandBy, Encrypt, Decrypt, DecryptCRT, DecryptBlind, DecryptCRTBlind };

				SecureRandom^ _rng;
				RSAKey^ _key;

				CipherOperatingMode _mode;
				RSACipherPaddingMode _padding;
				RSACipherDecryptionMode _decryption;
				Int32 _iInput, _iOutput;
				array<Byte> ^_block, ^_input, ^_output;
				Int32 _blockLength, _writeLength, _blockCount, _outputLength;
				BigInteger n, e, d, p, q, dp, dq, qinv;

				void _runEncrypt();
				void _runDecrypt();
				void _runDecryptCRT();
				void _runDecryptBlind();
				void _runDecryptCRTBlind();

				BigInteger _encryptInt(BigInteger% i);
				BigInteger _decryptInt(BigInteger% i);
				BigInteger _crtDecryptInt(BigInteger% i);
				BigInteger _blindDecryptInt(BigInteger% i);
				BigInteger _blindCrtDecryptInt(BigInteger% i);
				BigInteger _getBlindingParams(BigInteger% i, BigInteger% rinv);

				void _initOutputAndBlockBuffers();
				void _readToBlock();
				void _readToBlock(Int32% i);
				void _readLastBlock(Int32% i);
				void _writeToOutput(BigInteger% data);

				void _parseInputLength();
				void _fixDataAfterDecryption();

				void _setupEncryptMode(array<Byte>^ data);
				void _setupDecryptMode(array<Byte>^ data);

				void _cleanUp();
		};
	}
}