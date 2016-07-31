#include "HastadAttack.h"

using namespace RsaSandbox::Attacks;

HastadAttack::HastadAttack()
{
	_cleanUp();
}

void HastadAttack::performAttack(List<RSAKey^>^ keys, List<array<Byte>^>^ ciphers)
{
	for(Int32 i = 0; i < keys->Count; ++i)
		if(!keys[i]->AreExponentsSet)
			throw gcnew InvalidOperationException("At least one of the keys is incomplete. Exponents must be set before performing an attack.");

	for(Int32 i = 1; i < keys->Count; ++i)
		if(keys[i - 1]->PublicExponent != keys[i]->PublicExponent)
			throw gcnew ArgumentException("All keys must have the same public exponent.");

	if(BigInteger(keys->Count) != keys[0]->PublicExponent) throw gcnew ArgumentException("The amount of keys must be equal to the public exponent.");
	if(keys->Count != ciphers->Count) throw gcnew ArgumentException("The amount of keys must be equal to the amount of ciphers.");

	_prepareData(keys, ciphers);
	_handleStartUp((keys->Count - 1) * _blockCount);
}

void HastadAttack::_run()
{
	for(Int32 k = 0; k < _blockCount; ++k) {
		_readCipherBlocks(k);
		if(!_solveNextBlock()) return;
	}
	_fixMessageAfterAttack();
}

void HastadAttack::_done()
{
	array<Byte>^ message = _message;
	_cleanUp();
	Outcome(this, message, false);
}

void HastadAttack::_doneCancelled()
{
	_cleanUp();
	Outcome(this, nullptr, true);
}

void HastadAttack::_prepareData(List<RSAKey^>^ keys, List<array<Byte>^>^ ciphers)
{
	_ciphers = ciphers;
	_iMsg = 0;
	_blockLength = (_writeLength = keys[0]->BlockLength) + 1;
	_readBuffer = gcnew array<Byte>(_blockLength + 1);
	_readBuffer[_blockLength] = 0;

	e = (Int32)keys[0]->PublicExponent;
	_blockCount = RSALengthCalculator::getDecryptBlockCount(keys[0], ciphers[0]->Length);
	_message = gcnew array<Byte>(RSALengthCalculator::getDecryptDataLength(keys[0], ciphers[0]->Length));

	_congruences = gcnew List<Congruence^>(keys->Count + 1);
	for(Int32 i = 0; i < keys->Count; ++i) _congruences->Add(gcnew Congruence(BigInteger::Zero, keys[i]->Modulus));
	_congruences->TrimExcess();
}

bool HastadAttack::_solveNextBlock()
{
	Congruence^ result = _congruences[0];

	for(Int32 i = 1; i < _ciphers->Count; ++i, _incrementCurrentStep()) {
		result = Congruence::solve(result, _congruences[i]);

		_handleReportProgress();
		if(_handleCancellation()) return false;
	}

	_forceReportProgress();
	_writeSolutionToMessage(RSAInt::nthRoot(result->Remainder, e));
	return true;
}

void HastadAttack::_readCipherBlocks(Int32 iBlock)
{
	Int32 blockStart = iBlock * _blockLength;

	for(Int32 i = 0; i < _ciphers->Count; ++i) {
		array<Byte>^ cipher = _ciphers[i];
		for(Int32 j = 0, k = blockStart; j < _blockLength; ++j, ++k) _readBuffer[j] = cipher[k];
		_congruences[i]->Remainder = BigInteger(_readBuffer);
	}
}

void HastadAttack::_writeSolutionToMessage(BigInteger% solution)
{
	Int32 i;
	array<Byte>^ bytes = solution.ToByteArray();

	if(bytes->Length < _writeLength) {
		for(i = 0; i < bytes->Length; ++i)
			_message[_iMsg++] = bytes[i];
		while(i < _writeLength) { _message[_iMsg++] = 0; ++i; }
	}
	else for(i = 0; i < _writeLength; ++i)
		_message[_iMsg++] = bytes[i];

	Array::Clear(bytes, 0, bytes->Length);
}

void HastadAttack::_fixMessageAfterAttack()
{
	Int32 dataLength = _message[0];
	dataLength += (Int32)(_message[1] << 8);
	dataLength += (Int32)(_message[2] << 16);
	dataLength += (Int32)(_message[3] << 24);

	if(dataLength >= 0 && dataLength <= _message->Length - 4) {
		array<Byte>^ copy = _message;
		_message = gcnew array<Byte>(dataLength);
		for(Int32 i = 0; i < dataLength; ++i) _message[i] = copy[i + 4];
		Array::Clear(copy, 0, copy->Length);
	}
	else _message = nullptr;
}

void HastadAttack::_cleanUp()
{
	_blockCount = _blockLength = _writeLength = e = _iMsg = 0;
	_congruences = nullptr;
	_ciphers = nullptr;
	_message = nullptr;
}