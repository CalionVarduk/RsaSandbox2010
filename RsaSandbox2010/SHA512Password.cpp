#include "SHA512Password.h"

using namespace RsaSandbox;

SHA512Password::SHA512Password()
{
	_setPassword("");
}

SHA512Password::SHA512Password(String^ password)
{
	_setPassword(password);
}

SHA512Password::SHA512Password(array<Byte>^ salt)
{
	_setPassword("", salt);
}

SHA512Password::SHA512Password(String^ password, array<Byte>^ salt)
{
	_setPassword(password, salt);
}

bool SHA512Password::change(String^ oldPassword, String^ newPassword)
{
	if(authenticate(oldPassword)) {
		_setPassword(newPassword);
		return true;
	}
	return false;
}

bool SHA512Password::change(String^ oldPassword, String^ newPassword, array<Byte>^ newSalt)
{
	if(authenticate(oldPassword)) {
		_setPassword(newPassword, newSalt);
		return true;
	}
	return false;
}

bool SHA512Password::authenticate(String^ password)
{
	array<Byte>^ hash = _getHash(password);

	if(hash->Length == _hash->Length) {
		for(Int32 i = 0; i < _hash->Length; ++i) {
			if(hash[i] != _hash[i]) {
				Array::Clear(hash, 0, hash->Length);
				return false;
			}
		}
		Array::Clear(hash, 0, hash->Length);
		return true;
	}
	Array::Clear(hash, 0, hash->Length);
	return false;
}

void SHA512Password::_generateSalt()
{
	_salt = gcnew array<Byte>(32);
	rng->GetBytes(_salt);
}

void SHA512Password::_setSalt(array<Byte>^ salt)
{
	_salt = gcnew array<Byte>(32);
	if(salt->Length >= _salt->Length)
		for(Int32 i = 0; i < _salt->Length; ++i) _salt[i] = salt[i];
	else {
		for(Int32 i = 0; i < salt->Length; ++i) _salt[i] = salt[i];
		array<Byte>^ moreSalt = gcnew array<Byte>(_salt->Length - salt->Length);
		rng->GetBytes(moreSalt);
		for(Int32 i = 0; i < moreSalt->Length; ++i) _salt[i + salt->Length] = moreSalt[i];
		Array::Clear(moreSalt, 0, moreSalt->Length);
	}
}

array<Byte>^ SHA512Password::_getHash(String^ s)
{
	array<Byte>^ b = Encoding::UTF8->GetBytes(s);
	array<Byte>^ fullPwrd = gcnew array<Byte>(b->Length + _salt->Length);
	for(Int32 i = 0; i < b->Length; ++i) fullPwrd[i] = b[i];
	for(Int32 i = 0; i < _salt->Length; ++i) fullPwrd[i + b->Length] = _salt[i];
	return _getHash(fullPwrd);
}

array<Byte>^ SHA512Password::_getHash(array<Byte>^ b)
{
	SHA512Managed^ sha = gcnew SHA512Managed();
	array<Byte>^ hash = sha->ComputeHash(b);
	Array::Clear(b, 0, b->Length);
	return hash;
}

void SHA512Password::_setPassword(String^ password)
{
	_generateSalt();
	_hash = _getHash(password);
}

void SHA512Password::_setPassword(String^ password, array<Byte>^ salt)
{
	_setSalt(salt);
	_hash = _getHash(password);
}