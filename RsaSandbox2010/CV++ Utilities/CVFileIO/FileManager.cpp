#include "FileManager.h"

using namespace CVFileIO;

FileManager::FileManager()
	: _cipher(gcnew CVAwfulCipher()), _signature(gcnew array<Byte>(0)),
		_encrypt(false), _compress(false)
{}

void FileManager::setEncryptKey(String^ key, bool semiRandomize)
{
	array<Byte>^ keyArr = gcnew array<Byte>(key->Length);

	if(semiRandomize) {
		for(Int32 i = 0; i < key->Length; ++i)
			keyArr[i] = (Byte)(key[i] + (3 * i));
	}
	else for(Int32 i = 0; i < key->Length; ++i)
		keyArr[i] = (Byte)(key[i]);

	setEncryptKey(keyArr);
}

inline void FileManager::setEncryptKey(array<Byte>^ key)
{
	_cipher->setKey(key);
	if(_encrypt && !eligibleForEncryption())
		_encrypt = false;
}

inline array<Byte>^ FileManager::getEncryptKey()
{ return _cipher->getKey(); }

inline void FileManager::setCompressState(bool on)
{ _compress = on; }

inline bool FileManager::getCompressState()
{ return _compress; }

inline bool FileManager::eligibleForEncryption()
{ return (getEncryptKeyLength() > 0); }

inline bool FileManager::setEncryptionState(bool on)
{
	if(eligibleForEncryption()) {
		_encrypt = on;
		return true;
	}
	return false;
}

inline bool FileManager::getEncryptionState()
{ return _encrypt; }

void FileManager::setSignature(String^ signature)
{
	array<Byte>^ sigArr = gcnew array<Byte>(signature->Length);
	for(Int32 i = 0; i < signature->Length; ++i)
		sigArr[i] = (Byte)(signature[i]);
	setSignature(sigArr);
}

inline void FileManager::setSignature(array<Byte>^ signature)
{ _signature = signature; }

inline array<Byte>^ FileManager::getSignature()
{ return _signature; }

inline Int32 FileManager::getSignatureLength()
{ return _signature->Length; }

inline Int32 FileManager::getEncryptKeyLength()
{ return _cipher->getBlockLength(); }

bool FileManager::save(String^ path, array<Byte>^ data)
{
	try {
		if(_compress)
			data = HuffmanCompression::compress(data);
		if(_encrypt) {
			_cipher->adjustBeforeEncryption(data);
			data = _cipher->encrypt(data);
		}

		FileStream^ _stream = gcnew FileStream(path, FileMode::Create);
		BinaryWriter^ bWriter = gcnew BinaryWriter(_stream);

		if(_signature->Length > 0)
			bWriter->Write(_signature);
		bWriter->Write(data);
		bWriter->Close();
	}
	catch(Exception^ e)
	{ e; return false; }
	return true;
}

bool FileManager::load(String^ path, array<Byte>^% OUT_data)
{
	try {
		FileStream^ _stream = gcnew FileStream(path, FileMode::Open);
		BinaryReader^ bReader = gcnew BinaryReader(_stream);
	
		if(bReader->BaseStream->Length >= _signature->Length) {
			array<Byte>^ signature = bReader->ReadBytes(_signature->Length);

			if(_isSignatureCorrect(signature)) {
				OUT_data = bReader->ReadBytes((Int32)bReader->BaseStream->Length - _signature->Length);
				bReader->Close();

				if(_encrypt) {
					OUT_data = _cipher->decrypt(OUT_data);
					_cipher->adjustAfterDecryption(OUT_data);
				}
				if(_compress)
					OUT_data = HuffmanCompression::decompress(OUT_data);
				return true;
			}
			else bReader->Close();
		}
	}
	catch(Exception^ e)
	{ e; return false; }
	return false;
}

inline bool FileManager::_isSignatureCorrect(array<Byte>^ signature)
{
	if(_signature->Length == signature->Length) {
		for(Int32 i = 0; i < _signature->Length; ++i)
			if(_signature[i] != signature[i]) return false;
		return true;
	}
	return false;
}