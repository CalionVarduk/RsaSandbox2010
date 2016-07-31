#include "SizableHash.h"

using namespace RsaSandbox;

SizableHash::SizableHash(HashAlgorithm^ algorithm, Int32 byteSize)
{
	_algorithm = algorithm;
	ByteSize = byteSize;
}

array<Byte>^ SizableHash::computeHash(array<Byte>^ data)
{
	array<Byte>^ output = gcnew array<Byte>(_byteSize);
	computeHash(data, output);
	return output;
}

void SizableHash::computeHash(array<Byte>^ data, array<Byte>^ dest)
{
	array<Byte>^ hash = _algorithm->ComputeHash(data);

	if(_blockCount == 1)
		for(Int32 i = 0; i < _lastBlockSize; ++i) dest[i] = hash[i];
	else {
		Int32 iByte = 0;
		for(Int32 i = 0; i < 64; ++i) dest[iByte++] = hash[i];
		for(Int32 h = 2; h < _blockCount; ++h) {
			hash = _algorithm->ComputeHash(hash);
			for(Int32 i = 0; i < 64; ++i) dest[iByte++] = hash[i];
		}
		hash = _algorithm->ComputeHash(hash);
		for(Int32 i = 0; i < _lastBlockSize; ++i) dest[iByte++] = hash[i];
	}
	Array::Clear(hash, 0, hash->Length);
}

void SizableHash::_computeBlockCount()
{
	Int32 hashBytes = _algorithm->HashSize >> 3;
	_blockCount = (_byteSize + hashBytes - 1) / hashBytes;
	_lastBlockSize = _byteSize - ((_blockCount - 1) * hashBytes);
}