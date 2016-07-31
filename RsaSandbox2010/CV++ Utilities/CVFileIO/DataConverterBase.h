#pragma once

namespace CVFileIO
{
	using namespace System;

	public ref class DataConverterBase abstract
	{
		public:
			property Object^ DataObject {
				public: void set(Object^ value) { _data = value; }
			}

			property array<Byte>^ ByteArray {
				public: void set(array<Byte>^ value) { _bytes = value; }
			}

			property bool IsDataLoaded {
				public: bool get() { return (_data != nullptr); }
			}

			property bool AreBytesLoaded {
				public: bool get() { return (_bytes != nullptr); }
			}

			DataConverterBase() : _data(nullptr), _bytes(nullptr), _index(0) {}

			bool dataToBytes(array<Byte>^% OUT_bytes)
			{
				if(IsDataLoaded) {
					_setBuffers();
					if(writeDataToBytes()) {
						OUT_bytes = _bytes;
						_cleanUp();
						return true;
					}
					_cleanUp();
				}
				return false;
			}

			template <class T>
			bool bytesToData(T% OUT_data)
			{
				if(AreBytesLoaded) {
					if(readDataFromBytes()) {
						OUT_data = interpretDataAs<T>();
						_cleanUp();
						return true;
					}
					_cleanUp();
				}
				return false;
			}

		protected:
			property Int32 BytesLength {
				protected: Int32 get() { return _bytes->Length; }
			}

			property Int32 Index {
				protected: Int32 get() { return _index; }
			}

			template <class T>
			T interpretDataAs()
			{
				return (T)_data;
			}

			virtual bool writeDataToBytes() = 0;
			virtual bool readDataFromBytes() = 0;
			virtual Int32 dataBinaryLength() = 0;

			void writeInt(Int32 n)
			{
				writeUInt((UInt32)n);
			}

			Int32 readInt()
			{
				return (Int32)readUInt();
			}

			void writeUInt(UInt32 n)
			{
				_bytes[_index++] = (Byte)(n & 255);
				_bytes[_index++] = (Byte)((n >> 8) & 255);
				_bytes[_index++] = (Byte)((n >> 16) & 255);
				_bytes[_index++] = (Byte)(n >> 24);
			}

			UInt32 readUInt()
			{
				UInt32 value = _bytes[_index++];
				value |= (UInt32)_bytes[_index++] << 8;
				value |= (UInt32)_bytes[_index++] << 16;
				value |= (UInt32)_bytes[_index++] << 24;
				return value;
			}

			void writeShort(Int16 n)
			{
				writeUShort((UInt16)n);
			}

			Int16 readShort()
			{
				return (Int16)readUShort();
			}

			void writeUShort(UInt16 n)
			{
				_bytes[_index++] = (Byte)(n & 255);
				_bytes[_index++] = (Byte)(n >> 8);
			}

			UInt16 readUShort()
			{
				UInt16 value = _bytes[_index++];
				value |= (UInt16)_bytes[_index++] << 8;
				return value;
			}

			void writeChar(SByte n)
			{
				writeUChar((Byte)n);
			}

			SByte readChar()
			{
				return (SByte)readUChar();
			}

			void writeUChar(Byte n)
			{
				_bytes[_index++] = n;
			}

			Byte readUChar()
			{
				return _bytes[_index++];
			}

			void writeLong(Int64 n)
			{
				writeULong((UInt64)n);
			}

			Int64 readLong()
			{
				return (Int64)readULong();
			}

			void writeULong(UInt64 n)
			{
				writeUInt((UInt32)(n & 0xffffffff));
				writeUInt((UInt32)(n >> 32));
			}

			UInt64 readULong()
			{
				UInt64 value = readUInt();
				value |= (UInt64)readUInt() << 32;
				return value;
			}

			void writeDouble(double n)
			{
				array<Byte>^ buffer = BitConverter::GetBytes(n);
				for (int j = 0; j < buffer->Length; ++j, ++_index)
					_bytes[_index] = buffer[j];
			}

			double readDouble()
			{
				double value = BitConverter::ToDouble(_bytes, _index);
				_index += 8;
				return value;
			}

			void writeFloat(float n)
			{
				array<Byte>^ buffer = BitConverter::GetBytes(n);
				for (int j = 0; j < buffer->Length; ++j, ++_index)
					_bytes[_index] = buffer[j];
			}

			float readFloat()
			{
				float value = BitConverter::ToSingle(_bytes, _index);
				_index += 4;
				return value;
			}

		private:
			Object^ _data;
			array<Byte>^ _bytes;
			Int32 _index;

			void _setBuffers()
			{
				_bytes = gcnew array<Byte>(dataBinaryLength());
				_index = 0;
			}

			void _cleanUp()
			{
				_data = nullptr;
				_bytes = nullptr;
				_index = 0;
			}
	};
}