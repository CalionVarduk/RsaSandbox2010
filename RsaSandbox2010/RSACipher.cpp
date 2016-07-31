#include "RSACipher.h"

using namespace RsaSandbox::Ciphering;

RSACipher::RSACipher()
{
	_rng = gcnew SecureRandom();
	_key = nullptr;
	_padding = RSACipherPaddingMode::None;
	_decryption = RSACipherDecryptionMode::Basic;
	_cleanUp();
}

RSACipher::RSACipher(RSAKey^ key, RSACipherPaddingMode paddingMode, RSACipherDecryptionMode decryptionMode)
{
	_rng = gcnew SecureRandom();
	_key = key;
	_padding = paddingMode;
	_decryption = decryptionMode;
	_cleanUp();
}

void RSACipher::encrypt(array<Byte>^ data)
{
	if(!_key->AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. Exponents must be set before encryption.");
	_setupEncryptMode(data);

	n = _key->Modulus;
	e = _key->PublicExponent;

	_handleStartUp(_blockCount - 1);
}

void RSACipher::decrypt(array<Byte>^ cipher)
{
	if(!_key->AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. Exponents must be set before decryption.");
	_setupDecryptMode(cipher);

	try {
		bool crt = (DecryptionMode == RSACipherDecryptionMode::CRT || DecryptionMode == RSACipherDecryptionMode::CRTBlinding);
		if(crt) _key->getCRTDecryptionKey(p, q, dp, dq, qinv);
		else d = _key->getPrivateExponent();
		n = _key->Modulus;
		e = _key->PublicExponent;
	}
	catch(UnauthorizedAccessException^ exc) {
		_cleanUp();
		throw exc;
	}

	_handleStartUp(_blockCount);
}

void RSACipher::decrypt(array<Byte>^ cipher, String^ password)
{
	if(!_key->AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. Exponents must be set before decryption.");
	_setupDecryptMode(cipher);

	try {
		bool crt = (DecryptionMode == RSACipherDecryptionMode::CRT || DecryptionMode == RSACipherDecryptionMode::CRTBlinding);
		if(crt) _key->getCRTDecryptionKey(p, q, dp, dq, qinv, password);
		else d = _key->getPrivateExponent(password);
		n = _key->Modulus;
		e = _key->PublicExponent;
	}
	catch(UnauthorizedAccessException^ exc) {
		_cleanUp();
		throw exc;
	}

	_handleStartUp(_blockCount);
}

void RSACipher::_run()
{
	if(_mode == CipherOperatingMode::Encrypt) _runEncrypt();
	else if(_mode == CipherOperatingMode::Decrypt) _runDecrypt();
	else if(_mode == CipherOperatingMode::DecryptCRT) _runDecryptCRT();
	else if(_mode == CipherOperatingMode::DecryptBlind) _runDecryptBlind();
	else _runDecryptCRTBlind();
}

void RSACipher::_done()
{
	array<Byte>^ output = nullptr;
	RSACipherOutcome outcome;

	if(_mode == CipherOperatingMode::Encrypt) {
		outcome = RSACipherOutcome::OK;
		output = _output;
	}
	else if(_output != nullptr) {
		if(_padding == RSACipherPaddingMode::OAEP) {
			output = OAEPPadder::decryptPad(_key, _output);
			outcome = (output != nullptr) ? RSACipherOutcome::OK : RSACipherOutcome::WrongDecryptionKey;
		}
		else {
			outcome = RSACipherOutcome::OK;
			output = _output;
		}
	}
	else outcome = RSACipherOutcome::WrongDecryptionKey;

	_cleanUp();
	Outcome(this, output, outcome);
}

void RSACipher::_doneCancelled()
{
	_cleanUp();
	Outcome(this, nullptr, RSACipherOutcome::Cancelled);
}

void RSACipher::_runEncrypt()
{
	_initOutputAndBlockBuffers();

	Int32 iByte;
	if(_padding != RSACipherPaddingMode::OAEP) {
		_parseInputLength();
		iByte = 4;
	}
	else iByte = 0;

	for(_resetCurrentStep(); CurrentStep < StepCount; _incrementCurrentStep()) {
		_readToBlock(iByte);
		_writeToOutput(_encryptInt(BigInteger(_block)));
		iByte = 0;

		_handleReportProgress();
		if(_handleCancellation()) return;
	}
	_readLastBlock(iByte);
	_writeToOutput(_encryptInt(BigInteger(_block)));
}

void RSACipher::_runDecrypt()
{
	_initOutputAndBlockBuffers();

	for(_resetCurrentStep(); CurrentStep < StepCount; _incrementCurrentStep()) {
		_readToBlock();
		_writeToOutput(_decryptInt(BigInteger(_block)));

		_handleReportProgress();
		if(_handleCancellation()) return;
	}
	if(_padding != RSACipherPaddingMode::OAEP) _fixDataAfterDecryption();
}

void RSACipher::_runDecryptCRT()
{
	_initOutputAndBlockBuffers();

	for(_resetCurrentStep(); CurrentStep < StepCount; _incrementCurrentStep()) {
		_readToBlock();
		_writeToOutput(_crtDecryptInt(BigInteger(_block)));

		_handleReportProgress();
		if(_handleCancellation()) return;
	}
	if(_padding != RSACipherPaddingMode::OAEP) _fixDataAfterDecryption();
}

void RSACipher::_runDecryptBlind()
{
	_initOutputAndBlockBuffers();

	for(_resetCurrentStep(); CurrentStep < StepCount; _incrementCurrentStep()) {
		_readToBlock();
		_writeToOutput(_blindDecryptInt(BigInteger(_block)));

		_handleReportProgress();
		if(_handleCancellation()) return;
	}
	if(_padding != RSACipherPaddingMode::OAEP) _fixDataAfterDecryption();
}

void RSACipher::_runDecryptCRTBlind()
{
	_initOutputAndBlockBuffers();

	for(_resetCurrentStep(); CurrentStep < StepCount; _incrementCurrentStep()) {
		_readToBlock();
		_writeToOutput(_blindCrtDecryptInt(BigInteger(_block)));

		_handleReportProgress();
		if(_handleCancellation()) return;
	}
	if(_padding != RSACipherPaddingMode::OAEP) _fixDataAfterDecryption();
}

BigInteger RSACipher::_encryptInt(BigInteger% i)
{
	return BigInteger::ModPow(i, e, n);
}

BigInteger RSACipher::_decryptInt(BigInteger% i)
{
	return BigInteger::ModPow(i, d, n);
}

BigInteger RSACipher::_crtDecryptInt(BigInteger% i)
{
	BigInteger t1 = BigInteger::ModPow(i, dq, q);
	BigInteger t2 = BigInteger::ModPow(i, dp, p);
	BigInteger h = (qinv * BigInteger::Subtract(t2, t1)) % p;
	if(h < BigInteger::Zero) h = BigInteger::Add(h, p);
	return BigInteger::Add(t1, (h * q));
}

BigInteger RSACipher::_blindDecryptInt(BigInteger% i)
{
	BigInteger rinv;
	BigInteger i2 = _getBlindingParams(i, rinv);
	return (_decryptInt(i2) * rinv) % n;
}

BigInteger RSACipher::_blindCrtDecryptInt(BigInteger% i)
{
	BigInteger rinv;
	BigInteger i2 = _getBlindingParams(i, rinv);
	return (_crtDecryptInt(i2) * rinv) % n;
}

BigInteger RSACipher::_getBlindingParams(BigInteger% i, BigInteger% rinv)
{
	BigInteger r;
	do { r = _rng->getBigInteger(_key->BitLength - 1); } while(BigInteger::GreatestCommonDivisor(r, n) != BigInteger::One);
	rinv = RSAInt::modularInverse(r, n);
	return (i * BigInteger::ModPow(r, e, n)) % n;
}

void RSACipher::_initOutputAndBlockBuffers()
{
	_output = gcnew array<Byte>(_outputLength);
	_block = gcnew array<Byte>(_blockLength + 1);
	_block[_blockLength] = 0;
}

void RSACipher::_readToBlock()
{
	for(Int32 i = 0; i < _blockLength; ++i) _block[i] = _input[_iInput++];
}

void RSACipher::_readToBlock(Int32% i)
{
	while(i < _blockLength) _block[i++] = _input[_iInput++];
}

void RSACipher::_readLastBlock(Int32% i)
{
	while(_iInput < _input->Length) _block[i++] = _input[_iInput++];
	while(i < _blockLength) _block[i++] = 0;
}

void RSACipher::_writeToOutput(BigInteger% data)
{
	Int32 i;
	array<Byte>^ bytes = data.ToByteArray();

	if(bytes->Length < _writeLength) {
		for(i = 0; i < bytes->Length; ++i)
			_output[_iOutput++] = bytes[i];
		while(i < _writeLength) { _output[_iOutput++] = 0; ++i; }
	}
	else for(i = 0; i < _writeLength; ++i)
		_output[_iOutput++] = bytes[i];

	Array::Clear(bytes, 0, bytes->Length);
}

void RSACipher::_parseInputLength()
{
	_block[0] = (Byte)(_input->Length & 255);
	_block[1] = (Byte)((_input->Length >> 8) & 255);
	_block[2] = (Byte)((_input->Length >> 16) & 255);
	_block[3] = (Byte)(_input->Length >> 24);
}

void RSACipher::_fixDataAfterDecryption()
{
	Int32 dataLength = _output[0];
	dataLength += (Int32)(_output[1] << 8);
	dataLength += (Int32)(_output[2] << 16);
	dataLength += (Int32)(_output[3] << 24);

	if(dataLength >= 0 && dataLength <= _output->Length - 4) {
		array<Byte>^ copy = _output;
		_output = gcnew array<Byte>(dataLength);
		for(Int32 i = 0; i < dataLength; ++i) _output[i] = copy[i + 4];
		Array::Clear(copy, 0, copy->Length);
	}
	else _output = nullptr;
}

void RSACipher::_setupEncryptMode(array<Byte>^ data)
{
	_writeLength = (_blockLength = _key->BlockLength) + 1;
	_mode = CipherOperatingMode::Encrypt;

	if(_padding == RSACipherPaddingMode::OAEP) {
		_blockCount = RSALengthCalculator::getOAEPEncryptBlockCount(_key, data->Length);
		_outputLength = RSALengthCalculator::getOAEPEncryptCipherLength(_key, data->Length);
		_input = OAEPPadder::encryptPad(_key, data);
	}
	else {
		RSALengthCalculator::getEncryptLengths(_key, data->Length, _blockCount, _outputLength);
		_input = data;
	}
}

void RSACipher::_setupDecryptMode(array<Byte>^ data)
{
	_input = data;
	_blockLength = (_writeLength = _key->BlockLength) + 1;
	_mode = (_decryption == RSACipherDecryptionMode::Basic) ? CipherOperatingMode::Decrypt :
			(_decryption == RSACipherDecryptionMode::BasicBlinding) ? CipherOperatingMode::DecryptBlind :
			(_decryption == RSACipherDecryptionMode::CRT) ? CipherOperatingMode::DecryptCRT :
															CipherOperatingMode::DecryptCRTBlind;

	if(_padding == RSACipherPaddingMode::OAEP) {
		if(RSALengthCalculator::isDecryptionPossible(_key, data->Length)) {
			_blockCount = RSALengthCalculator::getOAEPDecryptBlockCount(_key, data->Length);
			_outputLength = RSALengthCalculator::getOAEPDecryptOutputLength(_key, data->Length);
		}
		else {
			_cleanUp();
			throw gcnew InvalidOperationException("The key is incompatible with the provided cipher using the current padding scheme.");
		}
	}
	else if(!RSALengthCalculator::getDecryptLengths(_key, data->Length, _blockCount, _outputLength)) {
		_cleanUp();
		throw gcnew InvalidOperationException("The key is incompatible with the provided cipher using the current padding scheme.");
	}
}

void RSACipher::_cleanUp()
{
	_mode = CipherOperatingMode::StandBy;
	_iInput = _iOutput = 0;
	if(_block != nullptr) Array::Clear(_block, 0, _block->Length);
	_block = _input = _output = nullptr;
	n = e = d = p = q = dp = dq = qinv = BigInteger::Zero;
	_writeLength = _blockLength = _blockCount = _outputLength = 0;
}