#pragma once

#include "../CVStructures/KeyedPair.h"

namespace CVFileIO
{
	using namespace System;
	using namespace CVStructures;

	public ref class CVAwfulCipher
	{
		public:
			static const UInt32 minBlockLength = 10;
			static const UInt32 maxBlockLength = 65535;

			CVAwfulCipher();

			bool setKey(array<Byte>^ key);
			array<Byte>^ getKey();

			UInt32 getBlockLength();

			bool adjustBeforeEncryption(array<Byte>^% input);
			bool adjustAfterDecryption(array<Byte>^% input);

			array<Byte>^ encrypt(array<Byte>^ input);
			array<Byte>^ decrypt(array<Byte>^ input);

		private:
			value class AffineKey {
				public:
					Byte a, b, iv;

					AffineKey getInvert();
					void encryptCBC(array<Byte>^ out_input, UInt32 iBeg, UInt32 length);
					void decryptCBC(array<Byte>^ out_input, UInt32 iBeg, UInt32 length);

				private:
					Byte _getInvertA();
			};

			ref class InternalKeyStructure {
				public:
					typedef array<KeyedPair<Byte, UInt16>> KeyPermutation;

					array<Byte>^ vKeyE;
					array<Byte>^ vKeyD;
					array<UInt16>^ pKeyE;
					array<UInt16>^ pKeyD;
					AffineKey aKeyE;
					AffineKey aKeyD;
					array<Byte>^ blockCopy;

					InternalKeyStructure();
					~InternalKeyStructure();

					void initForEncryption(array<Byte>^ key);
					void initForDecryption(array<Byte>^ key);

					void nextEncryptionKey(Byte cLastByte);
					void nextDecryptionKey(Byte cLastByte);

					template <class T>
					static void resetArray(array<T>^ input)
					{ for(Int32 i = 0; i < input->Length; ++i) input[i] = T(); }

				private:
					Int32 _iAffine;

					void _initVigenereKey(array<Byte>^ key);
					void _initPermutationKey();
					void _initAffineKey();

					void _nextVigenereKey(Byte cLastByte);
					void _nextPermutationKey();
					void _nextAffineKey();
					bool __foundNextAffineKey(Int32 upperBound);

					void _invertKeys();
					void __invertVigenereKey();
					void __invertPermutationKey();
					void __invertAffineKey();

					void _countingSort(KeyPermutation^ permutation);
			};

			array<Byte>^ _key;

			array<Byte>^ _eGetCorrectArray(array<Byte>^ input);
			void _startEncryption(array<Byte>^ output);
			void _startDecryption(array<Byte>^ output);

			array<Byte>^ _initIV(AffineKey aKey);
			void _nextIV(array<Byte>^ output, array<Byte>^ IV, UInt32 iBlockBeg);
		
			void _cXorIV(array<Byte>^ output, array<Byte>^ IV, UInt32 iBlockBeg);
			void _cVStep(array<Byte>^ output, array<Byte>^ vKey, UInt32 iBlockBeg);
			void _cPStep(array<Byte>^ output, array<UInt16>^ pKey,  UInt32 iBlockBeg, array<Byte>^ blockCopy);
			void _eAStep(array<Byte>^ output, AffineKey aKey, UInt32 iBlockBeg);
			void _dAStep(array<Byte>^ output, AffineKey aKey, UInt32 iBlockBeg);

			array<Byte>^ _eGetAdjustedArray(array<Byte>^ input);
			void __eAdjustRandomPad(array<Byte>^ aInput, UInt32 iBeg, UInt32 padLength, Random^ rng);

			array<Byte>^ _dGetAdjustedArray(array<Byte>^ input);

			void _copyArray(array<Byte>^ source, array<Byte>^ dest);
	};
}