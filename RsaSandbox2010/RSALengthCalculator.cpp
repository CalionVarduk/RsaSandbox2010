#include "RSALengthCalculator.h"

using namespace RsaSandbox::Ciphering;

Int32 RSALengthCalculator::getEncryptBlockCount(RSAKey^ key, Int32 dataLength)
{
	return (dataLength + key->BlockLength + 3) / key->BlockLength;	//((dataLength + 4) + blockLength - 1) / blockLength = ceil((dataLength + 4) / blockLength)
}

Int32 RSALengthCalculator::getEncryptCipherLength(RSAKey^ key, Int32 dataLength)
{
	return getEncryptBlockCount(key, dataLength) * (key->BlockLength + 1);
}

void RSALengthCalculator::getEncryptLengths(RSAKey^ key, Int32 dataLength, Int32% OUT_blockCount, Int32% OUT_cipherLength)
{
	OUT_blockCount = getEncryptBlockCount(key, dataLength);
	OUT_cipherLength = OUT_blockCount * (key->BlockLength + 1);
}

bool RSALengthCalculator::isDecryptionPossible(RSAKey^ key, Int32 cipherLength)
{
	return ((cipherLength % (key->BlockLength + 1)) == 0);
}

Int32 RSALengthCalculator::getDecryptBlockCount(RSAKey^ key, Int32 cipherLength)
{
	return cipherLength / (key->BlockLength + 1);
}

Int32 RSALengthCalculator::getDecryptDataLength(RSAKey^ key, Int32 cipherLength)
{
	return getDecryptBlockCount(key, cipherLength) * key->BlockLength;
}

bool RSALengthCalculator::getDecryptLengths(RSAKey^ key, Int32 cipherLength, Int32% OUT_blockCount, Int32% OUT_dataLength)
{
	if(isDecryptionPossible(key, cipherLength)) {
		OUT_blockCount = cipherLength / (key->BlockLength + 1);
		OUT_dataLength = OUT_blockCount * key->BlockLength;
		return true;
	}
	OUT_blockCount = OUT_dataLength = -1;
	return false;
}

Int32 RSALengthCalculator::getOAEPkLength(RSAKey^ key)
{
	return key->BlockLength >> 3;
}

Int32 RSALengthCalculator::getOAEPMessageLength(RSAKey^ key)
{
	return key->BlockLength - (getOAEPkLength(key) << 1);
}

void RSALengthCalculator::getOAEPLengths(RSAKey^ key, Int32 dataLength, Int32% OUT_kLength, Int32% OUT_msgLength)
{
	OUT_kLength = getOAEPkLength(key);
	OUT_msgLength = key->BlockLength - (OUT_kLength << 1);
}

Int32 RSALengthCalculator::getOAEPEncryptBlockCount(RSAKey^ key, Int32 dataLength)
{
	Int32 msgLength = getOAEPMessageLength(key);
	return (dataLength + msgLength + 3) / msgLength;
}

Int32 RSALengthCalculator::getOAEPEncryptOutputLength(RSAKey^ key, Int32 dataLength)
{
	return getOAEPEncryptBlockCount(key, dataLength) * key->BlockLength;
}

Int32 RSALengthCalculator::getOAEPEncryptCipherLength(RSAKey^ key, Int32 dataLength)
{
	return getOAEPEncryptBlockCount(key, dataLength) * (key->BlockLength + 1);
}

void RSALengthCalculator::getOAEPEncryptLengths(RSAKey^ key, Int32 dataLength, Int32% OUT_blockCount, Int32% OUT_outputLength, Int32% OUT_cipherLength)
{
	Int32 msgLength = getOAEPMessageLength(key);
	OUT_blockCount = (dataLength + msgLength + 3) / msgLength;
	OUT_outputLength = OUT_blockCount * key->BlockLength;
	OUT_cipherLength = OUT_outputLength + OUT_blockCount;
}

void RSALengthCalculator::getOAEPEncryptLengths(RSAKey^ key, Int32 dataLength, Int32% OUT_kLength, Int32% OUT_msgLength, Int32% OUT_blockCount, Int32% OUT_outputLength, Int32% OUT_cipherLength)
{
	getOAEPLengths(key, dataLength, OUT_kLength, OUT_msgLength);
	OUT_blockCount = (dataLength + OUT_msgLength + 3) / OUT_msgLength;
	OUT_outputLength = OUT_blockCount * key->BlockLength;
	OUT_cipherLength = OUT_outputLength + OUT_blockCount;
}

Int32 RSALengthCalculator::getOAEPDecryptBlockCount(RSAKey^ key, Int32 cipherLength)
{
	return getDecryptBlockCount(key, cipherLength);
}

Int32 RSALengthCalculator::getOAEPDecryptOutputLength(RSAKey^ key, Int32 cipherLength)
{
	return cipherLength - getOAEPDecryptBlockCount(key, cipherLength);
}

Int32 RSALengthCalculator::getOAEPDecryptDataLength(RSAKey^ key, Int32 cipherLength)
{
	return getOAEPDecryptBlockCount(key, cipherLength) * getOAEPMessageLength(key);
}

bool RSALengthCalculator::getOAEPDecryptLengths(RSAKey^ key, Int32 dataLength, Int32% OUT_blockCount, Int32% OUT_outputLength, Int32% OUT_dataLength)
{
	if(isDecryptionPossible(key, dataLength)) {
		Int32 msgLength = getOAEPMessageLength(key);
		OUT_blockCount = dataLength / (key->BlockLength + 1);
		OUT_outputLength = dataLength - OUT_blockCount;
		OUT_dataLength = OUT_blockCount * msgLength;
		return true;
	}
	OUT_blockCount = OUT_outputLength = OUT_dataLength = -1;
	return false;
}

bool RSALengthCalculator::getOAEPDecryptLengths(RSAKey^ key, Int32 dataLength, Int32% OUT_kLength, Int32% OUT_msgLength, Int32% OUT_blockCount, Int32% OUT_outputLength, Int32% OUT_dataLength)
{
	if(isDecryptionPossible(key, dataLength)) {
		getOAEPLengths(key, dataLength, OUT_kLength, OUT_msgLength);
		OUT_blockCount = dataLength / (key->BlockLength + 1);
		OUT_outputLength = dataLength - OUT_blockCount;
		OUT_dataLength = OUT_blockCount * OUT_msgLength;
		return true;
	}
	OUT_kLength = OUT_msgLength = OUT_blockCount = OUT_outputLength = OUT_dataLength = -1;
	return false;
}