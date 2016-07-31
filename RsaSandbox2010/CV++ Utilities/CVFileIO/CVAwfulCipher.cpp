#include "CVAwfulCipher.h"

using namespace CVFileIO;

CVAwfulCipher::CVAwfulCipher()
{
	_key = gcnew array<Byte>(0);
}

bool CVAwfulCipher::setKey(array<Byte>^ key)
{
	if(key->Length < minBlockLength) {
		_key = gcnew array<Byte>(0);
		return false;
	}

	UInt32 keyLength = (key->Length <= maxBlockLength) ? key->Length : maxBlockLength;
	_key = gcnew array<Byte>(keyLength);
	for(UInt32 i = 0; i < keyLength; ++i)
		_key[i] = key[i];

	return true;
}

array<Byte>^ CVAwfulCipher::getKey()
{
	array<Byte>^ key = gcnew array<Byte>(_key->Length);
	for(Int32 i = 0; i < key->Length; ++i)
		key[i] = _key[i];
	return key;
}

UInt32 CVAwfulCipher::getBlockLength()
{
	return _key->Length;
}

bool CVAwfulCipher::adjustBeforeEncryption(array<Byte>^% input)
{
	if(_key->Length == 0 || input->Length == 0)
		return false;

	input = _eGetAdjustedArray(input);
	return true;
}

bool CVAwfulCipher::adjustAfterDecryption(array<Byte>^% input)
{
	if(_key->Length == 0 || input->Length < (_key->Length << 1))
		return false;

	array<Byte>^ aux = _dGetAdjustedArray(input);
	return (aux->Length > 0) ? input = aux, true : false;
}

array<Byte>^ CVAwfulCipher::encrypt(array<Byte>^ input)
{
	if(_key->Length == 0)
		return gcnew array<Byte>(0);

	array<Byte>^ output = _eGetCorrectArray(input);

	_startEncryption(output);
	return output;
}

array<Byte>^ CVAwfulCipher::decrypt(array<Byte>^ input)
{
	if(_key->Length == 0)
		return gcnew array<Byte>(0);

	array<Byte>^ output = gcnew array<Byte>(input->Length);
	_copyArray(input, output);

	_startDecryption(output);
	return output;
}

array<Byte>^ CVAwfulCipher::_eGetCorrectArray(array<Byte>^ input)
{
	Int32 size = ((input->Length + _key->Length - 1) / _key->Length) * _key->Length;
	array<Byte>^ output = gcnew array<Byte>(size);

	_copyArray(input, output);
	for(Int32 i = input->Length; i < size; ++i)
		output[i] = 0;

	return output;
}

array<Byte>^ CVAwfulCipher::_eGetAdjustedArray(array<Byte>^ input)
{
	Random^ rng = gcnew Random();

	UInt32 aSize = ((input->Length + (_key->Length << 1) + 1) / _key->Length) * _key->Length;
	array<Byte>^ aInput = gcnew array<Byte>(aSize);

	__eAdjustRandomPad(aInput, 0, _key->Length, rng);
	
	UInt16 padLength = (UInt16)((aInput->Length - _key->Length) - (input->Length + 2));
	aInput[_key->Length] = (Byte)(padLength & 255);
	aInput[_key->Length + 1] = (Byte)(padLength >> 8);

	__eAdjustRandomPad(aInput, _key->Length + 2, padLength, rng);

	Int32 iInputBeg = _key->Length + padLength + 2;
	for(Int32 i = 0; i < input->Length; ++i)
		aInput[i + iInputBeg] = input[i];

	return aInput;
}

void CVAwfulCipher::__eAdjustRandomPad(array<Byte>^ aInput, UInt32 iBeg, UInt32 padLength, Random^ rng)
{
	UInt32 iCurrent = iBeg;
	UInt32 iEnd = iBeg + padLength - 1;
	Int32 random32;

	while(iCurrent <= iEnd) {
		random32 = rng->Next();
		for(Byte j = 0; j < 4; ++j) {
			if(iCurrent <= iEnd)
				aInput[iCurrent++] = (Byte)((random32 >> (j << 3)) & 255);
			else break;
		}
	}
}

array<Byte>^ CVAwfulCipher::_dGetAdjustedArray(array<Byte>^ input)
{
	UInt16 padLength = (UInt16)(input[_key->Length] + (input[_key->Length + 1] << 8));
	
	if(padLength >= _key->Length)
		return gcnew array<Byte>(0);

	Int32 iInputBeg = _key->Length + padLength + 2;
	UInt32 aSize = input->Length - iInputBeg;
	array<Byte>^ aInput = gcnew array<Byte>(aSize);

	for(UInt32 i = 0; i < aSize; ++i)
		aInput[i] = input[i + iInputBeg];
	
	return aInput;
}

void CVAwfulCipher::_startEncryption(array<Byte>^ output)
{
	Int32 iBlockBeg = 0;
	InternalKeyStructure^ key = gcnew InternalKeyStructure();
	key->initForEncryption(_key);
	array<Byte>^ IV = _initIV(key->aKeyE);

	while(iBlockBeg < output->Length) {
		_cXorIV(output, IV, iBlockBeg);

		_cVStep(output, key->vKeyE, iBlockBeg);
		_cPStep(output, key->pKeyE, iBlockBeg, key->blockCopy);
		_eAStep(output, key->aKeyE, iBlockBeg);

		key->nextEncryptionKey(output[iBlockBeg + _key->Length - 1]);
		_nextIV(output, IV, iBlockBeg);

		iBlockBeg += _key->Length;
	}

	InternalKeyStructure::resetArray(IV);
}

void CVAwfulCipher::_startDecryption(array<Byte>^ output)
{
	Int32 iBlockBeg = 0;
	InternalKeyStructure^ key = gcnew InternalKeyStructure();
	key->initForDecryption(_key);
	array<Byte>^ IV = _initIV(key->aKeyE);
	array<Byte>^ tmpIV = gcnew array<Byte>(_key->Length);

	while(iBlockBeg < output->Length) {
		_nextIV(output, tmpIV, iBlockBeg);

		_dAStep(output, key->aKeyD, iBlockBeg);
		_cPStep(output, key->pKeyD, iBlockBeg, key->blockCopy);
		_cVStep(output, key->vKeyD, iBlockBeg);

		_cXorIV(output, IV, iBlockBeg);

		key->nextDecryptionKey(tmpIV[_key->Length - 1]);
		_nextIV(tmpIV, IV, 0);

		iBlockBeg += _key->Length;
	}

	InternalKeyStructure::resetArray(IV);
	InternalKeyStructure::resetArray(tmpIV);
}

array<Byte>^ CVAwfulCipher::_initIV(AffineKey aKey)
{
	array<Byte>^ IV = gcnew array<Byte>(_key->Length);
	for(Int32 i = 0; i < _key->Length; ++i)
		IV[i] = _key[i];

	aKey.getInvert().decryptCBC(IV, 0, _key->Length);
	return IV;
}

void CVAwfulCipher::_nextIV(array<Byte>^ output, array<Byte>^ IV, UInt32 iBlockBeg)
{
	for(Int32 i = 0; i < _key->Length; ++i)
		IV[i] = output[i + iBlockBeg];
}

void CVAwfulCipher::_cXorIV(array<Byte>^ output, array<Byte>^ IV, UInt32 iBlockBeg)
{
	for(Int32 i = 0; i < _key->Length; ++i)
		output[i + iBlockBeg] ^= IV[i];
}

void CVAwfulCipher::_cVStep(array<Byte>^ output, array<Byte>^ vKey, UInt32 iBlockBeg)
{
	for(Int32 i = 0; i < _key->Length; ++i)
		output[i + iBlockBeg] += vKey[i];
}

void CVAwfulCipher::_cPStep(array<Byte>^ output, array<UInt16>^ pKey, UInt32 iBlockBeg, array<Byte>^ blockCopy)
{
	for(Int32 i = 0; i < _key->Length; ++i)
		blockCopy[i] = output[i + iBlockBeg];

	for(Int32 i = 0; i < _key->Length; ++i)
		output[iBlockBeg + pKey[i]] = blockCopy[i];
}

void CVAwfulCipher::_eAStep(array<Byte>^ output, AffineKey aKey, UInt32 iBlockBeg)
{
	aKey.encryptCBC(output, iBlockBeg, _key->Length);
}

void CVAwfulCipher::_dAStep(array<Byte>^ output, AffineKey aKey, UInt32 iBlockBeg)
{
	aKey.decryptCBC(output, iBlockBeg, _key->Length);
}

void CVAwfulCipher::_copyArray(array<Byte>^ source, array<Byte>^ dest)
{
	for(Int32 i = 0; i < source->Length; ++i)
		dest[i] = source[i];
}

#pragma region AffineKey Class
CVAwfulCipher::AffineKey CVAwfulCipher::AffineKey::getInvert()
{
	AffineKey invert;
	invert.a = _getInvertA();
	invert.b = (256 - invert.a) * b;
	invert.iv = iv;
	return invert;
}

Byte CVAwfulCipher::AffineKey::_getInvertA()
{
	UInt16 n = 256, aa = a, r, q;
	Byte u0 = 0, u1 = 1, t;

	while(aa > 1) {
		r = n % aa;
		q = n / aa;
		n = aa;
		aa = r;

		t = u0 - (u1 * q);
		u0 = u1;
		u1 = t;
	}

	return u1;
}

void CVAwfulCipher::AffineKey::encryptCBC(array<Byte>^ out_input, UInt32 iBeg, UInt32 length)
{
	UInt32 iEnd = iBeg + length;
	Byte ivcopy = iv;

	while(iBeg < iEnd) {
		out_input[iBeg] ^= ivcopy;
		(out_input[iBeg] *= a) += b;
		ivcopy = out_input[iBeg++];
	}
}

void CVAwfulCipher::AffineKey::decryptCBC(array<Byte>^ out_input, UInt32 iBeg, UInt32 length)
{
	UInt32 iEnd = iBeg + length;
	Byte ivcopy = iv, tmpiv;

	while(iBeg < iEnd) {
		tmpiv = out_input[iBeg];
		(out_input[iBeg] *= a) += b;
		out_input[iBeg++] ^= ivcopy;
		ivcopy = tmpiv;
	}
}
#pragma endregion

#pragma region InternalKeyStructure Class
CVAwfulCipher::InternalKeyStructure::InternalKeyStructure()
	: vKeyE(nullptr), vKeyD(nullptr),
		pKeyE(nullptr), pKeyD(nullptr),
		aKeyE(), aKeyD(), _iAffine(0),
		blockCopy(nullptr)
{}

CVAwfulCipher::InternalKeyStructure::~InternalKeyStructure()
{
	resetArray(vKeyE);
	resetArray(vKeyD);
	resetArray(pKeyE);
	resetArray(pKeyD);
	resetArray(blockCopy);
	aKeyE = AffineKey();
	aKeyD = AffineKey();
	_iAffine = 0;
}

void CVAwfulCipher::InternalKeyStructure::initForEncryption(array<Byte>^ key)
{
	_initVigenereKey(key);
	_initPermutationKey();
	_initAffineKey();
	blockCopy = gcnew array<Byte>(key->Length);
}

void CVAwfulCipher::InternalKeyStructure::initForDecryption(array<Byte>^ key)
{
	initForEncryption(key);
	vKeyD = gcnew array<Byte>(key->Length);
	pKeyD = gcnew array<UInt16>(key->Length);
	_invertKeys();
}

void CVAwfulCipher::InternalKeyStructure::nextEncryptionKey(Byte cLastByte)
{
	_nextVigenereKey(cLastByte);
	_nextPermutationKey();
	_nextAffineKey();
}

void CVAwfulCipher::InternalKeyStructure::nextDecryptionKey(Byte cLastByte)
{
	nextEncryptionKey(cLastByte);
	_invertKeys();
}

void CVAwfulCipher::InternalKeyStructure::_initVigenereKey(array<Byte>^ key)
{
	vKeyE = gcnew array<Byte>(key->Length);
	for(Int32 i = 0; i < key->Length; ++i)
		vKeyE[i] = key[i];
}

void CVAwfulCipher::InternalKeyStructure::_initPermutationKey()
{
	KeyPermutation^ permutation = gcnew KeyPermutation(vKeyE->Length);
	for(Int32 i = 0; i < vKeyE->Length; ++i)
		permutation[i].set(vKeyE[i], i);

	_countingSort(permutation);

	pKeyE = gcnew array<UInt16>(vKeyE->Length);
	for(Int32 i = 0; i < vKeyE->Length; ++i)
		pKeyE[permutation[i].item] = i;
}

void CVAwfulCipher::InternalKeyStructure::_initAffineKey()
{
	_iAffine = 0;
	_nextAffineKey();
}

void CVAwfulCipher::InternalKeyStructure::_nextVigenereKey(Byte cLastByte)
{
	for(Int32 i = 0; i < vKeyE->Length; ++i)
		++(vKeyE[i] ^= cLastByte);

	aKeyE.encryptCBC(vKeyE, 0, vKeyE->Length);
}

void CVAwfulCipher::InternalKeyStructure::_nextPermutationKey()
{
	Int32 iSuffixBeg = vKeyE->Length - 1;

	while((iSuffixBeg > 0) && (pKeyE[iSuffixBeg - 1] >= pKeyE[iSuffixBeg]))
		--iSuffixBeg;

	if(iSuffixBeg == 0) {
		for(Int32 i = 0; i < vKeyE->Length; ++i)
			pKeyE[i] = i;
		return;
	}

	Int32 iPivot = iSuffixBeg - 1;
	Int32 iSmallestBigger = vKeyE->Length - 1;
	while(pKeyE[iSmallestBigger] <= pKeyE[iPivot])
		--iSmallestBigger;

	UInt16 tmp = pKeyE[iPivot];
	pKeyE[iPivot] = pKeyE[iSmallestBigger];
	pKeyE[iSmallestBigger] = tmp;

	Int32 i = iSuffixBeg, j = vKeyE->Length - 1;
	while(i < j) {
		tmp = pKeyE[i];
		pKeyE[i++] = pKeyE[j];
		pKeyE[j--] = tmp;
	}
}

void CVAwfulCipher::InternalKeyStructure::_nextAffineKey()
{
	bool keyFound = true;
	Int32 iPrevAffine = _iAffine;

	if(!__foundNextAffineKey(vKeyE->Length)) {
		_iAffine = 0;
		keyFound = __foundNextAffineKey(iPrevAffine);
	}

	aKeyE.a = (keyFound) ? vKeyE[_iAffine] : 23;
	aKeyE.b = (_iAffine < vKeyE->Length - 1) ? vKeyE[++_iAffine] : vKeyE[(_iAffine = 0)];
	aKeyE.iv = (_iAffine > 1) ? vKeyE[_iAffine - 2] : vKeyE[vKeyE->Length - 1];
}

bool CVAwfulCipher::InternalKeyStructure::__foundNextAffineKey(Int32 upperBound)
{
	while((_iAffine < upperBound) && (((vKeyE[_iAffine] & 1) == 0) || (vKeyE[_iAffine] == 1) || (vKeyE[_iAffine] == 255)))
		++_iAffine;

	return (_iAffine < upperBound);
}

void CVAwfulCipher::InternalKeyStructure::_invertKeys()
{
	__invertVigenereKey();
	__invertPermutationKey();
	__invertAffineKey();
}

void CVAwfulCipher::InternalKeyStructure::__invertVigenereKey()
{
	for(Int32 i = 0; i < vKeyE->Length; ++i)
		vKeyD[i] = 256 - vKeyE[i];
}

void CVAwfulCipher::InternalKeyStructure::__invertPermutationKey()
{
	for(Int32 i = 0; i < vKeyE->Length; ++i)
		pKeyD[pKeyE[i]] = i;
}

void CVAwfulCipher::InternalKeyStructure::__invertAffineKey()
{
	aKeyD = aKeyE.getInvert();
}

void CVAwfulCipher::InternalKeyStructure::_countingSort(KeyPermutation^ permutation)
{
	array<UInt32>^ bytesCount = gcnew array<UInt32>(256);
	for(Int32 i = 0; i < bytesCount->Length; ++i)
		bytesCount[i] = 0;

	for(Int32 i = 0; i < permutation->Length; ++i)
		++bytesCount[permutation[i].key];

	for(Int32 i = 1; i < bytesCount->Length; ++i)
		bytesCount[i] += bytesCount[i - 1];

	KeyPermutation^ permutationCopy = gcnew KeyPermutation(permutation->Length);
	for(Int32 i = 0; i < permutation->Length; ++i)
		permutationCopy[i].set(permutation[i].key, permutation[i].item);

	for(Int32 i = permutation->Length - 1; i >= 0; --i)
		permutation[--bytesCount[permutationCopy[i].key]].set(permutationCopy[i].key, permutationCopy[i].item);
}
#pragma endregion
