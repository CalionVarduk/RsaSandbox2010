#include "OAEPPadder.h"

using namespace RsaSandbox::Ciphering;

array<Byte>^ OAEPPadder::encryptPad(RSAKey^ key, array<Byte>^ data)
{
	OAEPContainer^ oaep = OAEPContainer::CreateEncryptor(key, data);
	Int32 blockCountM1 = RSALengthCalculator::getOAEPEncryptBlockCount(key, data->Length) - 1;

	Int32 iByte = 4;
	oaep->parseInputLength();

	for(Int32 i = 0; i < blockCountM1; ++i) {
		oaep->fillNextBlock(iByte);
		oaep->encryptNextBlock();
		iByte = 0;
	}
	oaep->fillLastBlock(iByte);
	oaep->encryptNextBlock();

	array<Byte>^ out = oaep->output;
	oaep->cleanUp();
	return out;
}

array<Byte>^ OAEPPadder::decryptPad(RSAKey^ key, array<Byte>^ data)
{
	OAEPContainer^ oaep = OAEPContainer::CreateDecryptor(key, data);
	Int32 blockCount = data->Length / key->BlockLength;

	for(Int32 i = 0; i < blockCount; ++i)
		oaep->decryptNextBlock();

	oaep->fixDataAfterDecryption();
	array<Byte>^ out = oaep->output;
	oaep->cleanUp();
	return out;
}

OAEPPadder::OAEPContainer^ OAEPPadder::OAEPContainer::CreateEncryptor(RSAKey^ key, array<Byte>^ data)
{
	OAEPContainer^ oaep = gcnew OAEPContainer();

	Int32 k, msgLength;
	RSALengthCalculator::getOAEPLengths(key, data->Length, k, msgLength);
	Int32 outputLength = RSALengthCalculator::getOAEPEncryptOutputLength(key, data->Length);

	oaep->iOutput = 0;
	oaep->input = data;
	oaep->output = gcnew array<Byte>(outputLength);
	oaep->msg = gcnew array<Byte>(msgLength);
	oaep->rand = gcnew array<Byte>(k);
	oaep->X = gcnew array<Byte>(msgLength + k);
	oaep->Y = gcnew array<Byte>(k);
	oaep->shrinker = gcnew SizableHash(gcnew SHA512Managed(), oaep->Y->Length);
	oaep->expander = gcnew SizableHash(oaep->shrinker->Algorithm, oaep->X->Length);
	return oaep;
}

OAEPPadder::OAEPContainer^ OAEPPadder::OAEPContainer::CreateDecryptor(RSAKey^ key, array<Byte>^ data)
{
	OAEPContainer^ oaep = gcnew OAEPContainer();

	Int32 k, msgLength;
	RSALengthCalculator::getOAEPLengths(key, data->Length, k, msgLength);
	Int32 outputLength = (data->Length / key->BlockLength) * msgLength;

	oaep->iOutput = 0;
	oaep->input = data;
	oaep->output = gcnew array<Byte>(outputLength);
	oaep->msg = gcnew array<Byte>(msgLength + k);
	oaep->rand = gcnew array<Byte>(k);
	oaep->X = gcnew array<Byte>(oaep->msg->Length);
	oaep->Y = gcnew array<Byte>(k);
	oaep->shrinker = gcnew SizableHash(gcnew SHA512Managed(), oaep->Y->Length);
	oaep->expander = gcnew SizableHash(oaep->shrinker->Algorithm, oaep->X->Length);
	return oaep;
}

void OAEPPadder::OAEPContainer::fillNextBlock(Int32 iByte)
{
	while(iByte < msg->Length) msg[iByte++] = input[iInput++];
}

void OAEPPadder::OAEPContainer::fillLastBlock(Int32 iByte)
{
	while(iInput < input->Length) msg[iByte++] = input[iInput++];
	while(iByte < msg->Length) msg[iByte++] = 0;
}

void OAEPPadder::OAEPContainer::encryptNextBlock()
{
	_rng->GetBytes(rand);

	expander->computeHash(rand, X);
	for(Int32 i = 0; i < msg->Length; ++i) X[i] ^= msg[i];

	shrinker->computeHash(X, Y);
	for(Int32 i = 0; i < Y->Length; ++i) Y[i] ^= rand[i];

	for(Int32 i = 0; i < X->Length; ++i) output[iOutput++] = X[i];
	for(Int32 i = 0; i < Y->Length; ++i) output[iOutput++] = Y[i];
}

void OAEPPadder::OAEPContainer::decryptNextBlock()
{
	for(Int32 i = 0; i < X->Length; ++i) X[i] = input[iInput++];
	for(Int32 i = 0; i < Y->Length; ++i) Y[i] = input[iInput++];

	shrinker->computeHash(X, rand);
	for(Int32 i = 0; i < rand->Length; ++i) rand[i] ^= Y[i];

	expander->computeHash(rand, msg);
	for(Int32 i = 0; i < msg->Length; ++i) msg[i] ^= X[i];

	Int32 msgLength = X->Length - Y->Length;
	for(Int32 i = 0; i < msgLength; ++i) output[iOutput++] = msg[i];
}

void OAEPPadder::OAEPContainer::parseInputLength()
{
	msg[0] = (Byte)(input->Length & 255);
	msg[1] = (Byte)((input->Length >> 8) & 255);
	msg[2] = (Byte)((input->Length >> 16) & 255);
	msg[3] = (Byte)(input->Length >> 24);
}

void OAEPPadder::OAEPContainer::fixDataAfterDecryption()
{
	Int32 dataLength = output[0];
	dataLength += (Int32)(output[1] << 8);
	dataLength += (Int32)(output[2] << 16);
	dataLength += (Int32)(output[3] << 24);

	if(dataLength >= 0 && dataLength <= output->Length - 4) {
		array<Byte>^ copy = output;
		output = gcnew array<Byte>(dataLength);
		for(Int32 i = 0; i < dataLength; ++i) output[i] = copy[i + 4];
		Array::Clear(copy, 0, copy->Length);
	}
	else output = nullptr;
}

void OAEPPadder::OAEPContainer::cleanUp()
{
	Array::Clear(msg, 0, msg->Length);
	Array::Clear(rand, 0, rand->Length);
	Array::Clear(X, 0, X->Length);
	Array::Clear(Y, 0, Y->Length);
	iInput = iOutput = 0;
	input = output = msg = rand = X = Y = nullptr;
	shrinker = expander = nullptr;
}

OAEPPadder::OAEPContainer::OAEPContainer()
{
	iInput = iOutput = 0;
	input = output = msg = rand = X = Y = nullptr;
	shrinker = expander = nullptr;
}