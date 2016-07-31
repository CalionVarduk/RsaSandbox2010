#include "AES256.h"

using namespace RsaSandbox;

array<Byte>^ AES256::encrypt(array<Byte>^ data, array<Byte>^ key)
{
	AesCryptoServiceProvider^ aes = _getAES(key, nullptr);
	array<Byte>^ iv = aes->IV;
	ICryptoTransform^ encryptor = aes->CreateEncryptor(aes->Key, aes->IV);

	MemoryStream^ ms = gcnew MemoryStream();
	CryptoStream^ cs = gcnew CryptoStream(ms, encryptor, CryptoStreamMode::Write);
	BinaryWriter^ bw = gcnew BinaryWriter(cs);

	bw->Write(data);
	bw->Close();

	array<Byte>^ cipher = ms->ToArray();

	cs->Close();
	ms->Close();
	
	array<Byte>^ outcome = gcnew array<Byte>(iv->Length + cipher->Length);
	for(Int32 i = 0; i < iv->Length; ++i) outcome[i] = iv[i];
	for(Int32 i = 0; i < cipher->Length; ++i) {
		outcome[i + iv->Length] = cipher[i];
		cipher[i] = 0;
	}

	Array::Clear(iv, 0, iv->Length);
	return outcome;
}

array<Byte>^ AES256::decrypt(array<Byte>^ cipher, array<Byte>^ key)
{
	array<Byte>^ iv = gcnew array<Byte>(16);
	for(Int32 i = 0; i < iv->Length; ++i) iv[i] = cipher[i];

	array<Byte>^ cipherCopy = gcnew array<Byte>(cipher->Length - iv->Length);
	for(Int32 i = 0; i < cipherCopy->Length; ++i)
		cipherCopy[i] = cipher[i + iv->Length];

	AesCryptoServiceProvider^ aes = _getAES(key, iv);
	ICryptoTransform^ decryptor = aes->CreateDecryptor(aes->Key, aes->IV);

	array<Byte>^ data;
	try {
		MemoryStream ms(cipherCopy);
		CryptoStream cs(%ms, decryptor, CryptoStreamMode::Read);
		BinaryReader br(%cs);

		data = br.ReadBytes((Int32)ms.Length);
		Array::Clear(iv, 0, iv->Length);

		br.Close();
		cs.Close();
		ms.Close();
	}
	catch(Exception^) {
		data = gcnew array<Byte>(0);
		Array::Clear(iv, 0, iv->Length);
	}

	return data;
}

AesCryptoServiceProvider^ AES256::_getAES(array<Byte>^ key, array<Byte>^ iv)
{
	AesCryptoServiceProvider^ aes = gcnew AesCryptoServiceProvider();
	aes->BlockSize = 128;
	aes->KeySize = 256;
	aes->Padding = padding;
	aes->Mode = mode;

	if(iv == nullptr) aes->GenerateIV();
	else aes->IV = iv;

	array<Byte>^ aesKey = gcnew array<Byte>(32);
	if(key->Length >= 32) {
		Int32 j = (key->Length > 32) ? key[0] % (key->Length - 32) : 0;
		for(Int32 i = 0; i < 32; ++i, ++j) aesKey[i] = key[j];
	}
	else {
		for(Int32 i = 0; i < key->Length; ++i) aesKey[i] = key[i];
		for(Int32 i = key->Length; i < 32; ++i) aesKey[i] = 0;
	}

	aes->Key = aesKey;
	return aes;
}