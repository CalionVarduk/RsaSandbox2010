#pragma once

namespace RsaSandbox
{
	using namespace System;
	using namespace System::Security::Cryptography;

	public ref class SizableHash
	{
		public:
			property HashAlgorithm^ Algorithm {
				public: HashAlgorithm^ get() { return _algorithm; }
				public: void set(HashAlgorithm^ value)
						{
							_algorithm = value;
							_computeBlockCount();
						}
			}

			property Int32 ByteSize {
				public: Int32 get() { return _byteSize; }
				public: void set(Int32 value)
						{
							_byteSize = value;
							_computeBlockCount();
						}
			}

			property Int32 BitSize {
				public: Int32 get() { return _byteSize << 3; }
			}

			property Int32 HashBlockCount {
				public: Int32 get() { return _blockCount; }
			}

			SizableHash(HashAlgorithm^ algorithm, Int32 byteSize);

			array<Byte>^ computeHash(array<Byte>^ data);
			void computeHash(array<Byte>^ data, array<Byte>^ dest);

		private:
			HashAlgorithm^ _algorithm;
			Int32 _byteSize, _blockCount, _lastBlockSize;

			void _computeBlockCount();
	};
}