#include "RSAKey.h"

using namespace RsaSandbox;

RSAKey::RSAKey(BigInteger% p, BigInteger% q)
{
	_pswrd = gcnew SHA512Password();
	this->p = p.ToByteArray();
	this->q = q.ToByteArray();
	n = p * q;
	e = BigInteger::Zero;
	qinv = RSAInt::modularInverse(q, p).ToByteArray();
	d = dp = dq = gcnew array<Byte>(0);

	_protected = _wienerSafe = _publicSafe = false;
	_setBlockLength();
	_setFermatSafeness(p, q);
}

RSAKey::RSAKey(RSAKey^ key)
{
	_pswrd = gcnew SHA512Password(key->_pswrd->Salt);
	p = (array<Byte>^)key->p->Clone();
	q = (array<Byte>^)key->q->Clone();
	n = key->n;
	e = key->e;
	d = (array<Byte>^)key->d->Clone();
	dp = (array<Byte>^)key->dp->Clone();
	dq = (array<Byte>^)key->dq->Clone();
	qinv = (array<Byte>^)key->qinv->Clone();
	_protected = key->_protected;
	_fermatSafe = key->_fermatSafe;
	_wienerSafe = key->_wienerSafe;
	_publicSafe = key->_publicSafe;
	_bitLength = key->_bitLength;
	_blockLength = key->_blockLength;
}

bool RSAKey::trySetExponents(BigInteger% e)
{
	if(_protected) throw gcnew InvalidOperationException("The key is password protected and, as such, can not be modified.");

	BigInteger P = BigInteger(p);
	BigInteger Q = BigInteger(q);
	BigInteger m = BigInteger::Add(BigInteger::Subtract(BigInteger::Subtract(n, P), Q), 1);

	if((m > BigInteger::One) && (e < m)) {
		if(BigInteger::GreatestCommonDivisor(e, m) == BigInteger::One) {
			this->e = e;
			BigInteger D = RSAInt::modularInverse(e, m);

			d = D.ToByteArray();
			dp = (D % BigInteger::Subtract(P, 1)).ToByteArray();
			dq = (D % BigInteger::Subtract(Q, 1)).ToByteArray();
			
			_setPublicSafeness();
			_setWienerSafeness(D);
			P = Q = m = D = BigInteger::Zero;
			return true;
		}
	}
	P = Q = m = BigInteger::Zero;
	return false;
}

void RSAKey::invertExponents()
{
	if(!AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. Exponents must be set before inverting them.");
	if(_protected) throw gcnew InvalidOperationException("The key is password protected and, as such, can not be modified.");

	BigInteger D = BigInteger(d);
	d = e.ToByteArray();
	e = D;

	D = BigInteger(d);
	dp = (D % BigInteger::Subtract(BigInteger(p), 1)).ToByteArray();
	dq = (D % BigInteger::Subtract(BigInteger(q), 1)).ToByteArray();

	_setPublicSafeness();
	_setWienerSafeness(D);
	D = BigInteger::Zero;
}

void RSAKey::setPassword(String^ password)
{
	if(!AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. Exponents must be set before applying a password.");
	if(_protected) throw gcnew UnauthorizedAccessException("The key is password protected. In order to change its password, an old password must be provided as well.");
	_encryptKey(password);
}

void RSAKey::setPassword(String^ oldPassword, String^ newPassword)
{
	if(!AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. Exponents must be set before applying a password.");
	if(_protected) _decryptKey(oldPassword);
	_encryptKey(newPassword);
}

void RSAKey::removePassword(String^ password)
{
	if(_protected) _decryptKey(password);
}

bool RSAKey::authenticate(String^ password)
{
	if(!AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. Exponents must be set before authentication attempt.");
	if(_protected) {
		try {
			_checkKeyIntegrity(password);
			_closePassword();
		}
		catch(UnauthorizedAccessException^) { return false; }
	}
	return true;
}

BigInteger RSAKey::getPrivateExponent()
{
	if(!AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. Private exponent is not set yet.");
	if(_protected) throw gcnew UnauthorizedAccessException("The key is password protected. In order to obtain its private exponent, a password must be provided.");
	return BigInteger(d);
}

BigInteger RSAKey::getPrivateExponent(String^ password)
{
	if(!AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. Private exponent is not set yet.");

	if(_protected) {
		_checkKeyIntegrity(password);
		BigInteger D = BigInteger(AES256::decrypt(d, _pswrd->Hash));
		_closePassword();
		return D;
	}
	return BigInteger(d);
}

void RSAKey::getPrimes(BigInteger% p, BigInteger% q)
{
	if(_protected) throw gcnew UnauthorizedAccessException("The key is password protected. In order to obtain its primes, a password must be provided.");
	p = BigInteger(this->p);
	q = BigInteger(this->q);
}

void RSAKey::getPrimes(BigInteger% p, BigInteger% q, String^ password)
{
	if(_protected) {
		_checkKeyIntegrity(password);
		p = BigInteger(AES256::decrypt(this->p, _pswrd->Hash));
		q = BigInteger(AES256::decrypt(this->q, _pswrd->Hash));
		_closePassword();
	}
	else {
		p = BigInteger(this->p);
		q = BigInteger(this->q);
	}
}

void RSAKey::getPrivateExponents(BigInteger% d, BigInteger% dp, BigInteger% dq)
{
	if(!AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. Private exponents are not set yet.");
	if(_protected) throw gcnew UnauthorizedAccessException("The key is password protected. In order to obtain its private exponents, a password must be provided.");
	d = BigInteger(this->d);
	dp = BigInteger(this->dp);
	dq = BigInteger(this->dq);
}

void RSAKey::getPrivateExponents(BigInteger% d, BigInteger% dp, BigInteger% dq, String^ password)
{
	if(!AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. Private exponents are not set yet.");
	if(_protected) {
		_checkKeyIntegrity(password);
		d = BigInteger(AES256::decrypt(this->d, _pswrd->Hash));
		dp = BigInteger(AES256::decrypt(this->dp, _pswrd->Hash));
		dq = BigInteger(AES256::decrypt(this->dq, _pswrd->Hash));
		_closePassword();
	}
	else {
		d = BigInteger(this->d);
		dp = BigInteger(this->dp);
		dq = BigInteger(this->dq);
	}
}

void RSAKey::getCRTDecryptionKey(BigInteger% p, BigInteger% q, BigInteger% dp, BigInteger% dq, BigInteger% qinv)
{
	if(!AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. CRT decryption key is not set yet.");
	if(_protected) throw gcnew UnauthorizedAccessException("The key is password protected. In order to obtain its CRT decryption key, a password must be provided.");
	p = BigInteger(this->p);
	q = BigInteger(this->q);
	dp = BigInteger(this->dp);
	dq = BigInteger(this->dq);
	qinv = BigInteger(this->qinv);
}

void RSAKey::getCRTDecryptionKey(BigInteger% p, BigInteger% q, BigInteger% dp, BigInteger% dq, BigInteger% qinv, String^ password)
{
	if(!AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. CRT decryption key is not set yet.");

	if(_protected) {
		_checkKeyIntegrity(password);
		p = BigInteger(AES256::decrypt(this->p, _pswrd->Hash));
		q = BigInteger(AES256::decrypt(this->q, _pswrd->Hash));
		dp = BigInteger(AES256::decrypt(this->dp, _pswrd->Hash));
		dq = BigInteger(AES256::decrypt(this->dq, _pswrd->Hash));
		qinv = BigInteger(AES256::decrypt(this->qinv, _pswrd->Hash));
		_closePassword();
	}
	else {
		p = BigInteger(this->p);
		q = BigInteger(this->q);
		dp = BigInteger(this->dp);
		dq = BigInteger(this->dq);
		qinv = BigInteger(this->qinv);
	}
}

void RSAKey::getAllPrivateComponents(BigInteger% p, BigInteger% q, BigInteger% d, BigInteger% dp, BigInteger% dq, BigInteger% qinv)
{
	if(!AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. Private components are not set yet.");
	if(_protected) throw gcnew UnauthorizedAccessException("The key is password protected. In order to obtain its private components, a password must be provided.");
	p = BigInteger(this->p);
	q = BigInteger(this->q);
	d = BigInteger(this->d);
	dp = BigInteger(this->dp);
	dq = BigInteger(this->dq);
	qinv = BigInteger(this->qinv);
}

void RSAKey::getAllPrivateComponents(BigInteger% p, BigInteger% q,  BigInteger% d, BigInteger% dp, BigInteger% dq, BigInteger% qinv, String^ password)
{
	if(!AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. Private components are not set yet.");
	if(_protected) {
		_checkKeyIntegrity(password);
		p = BigInteger(AES256::decrypt(this->p, _pswrd->Hash));
		q = BigInteger(AES256::decrypt(this->q, _pswrd->Hash));
		d = BigInteger(AES256::decrypt(this->d, _pswrd->Hash));
		dp = BigInteger(AES256::decrypt(this->dp, _pswrd->Hash));
		dq = BigInteger(AES256::decrypt(this->dq, _pswrd->Hash));
		qinv = BigInteger(AES256::decrypt(this->qinv, _pswrd->Hash));
		_closePassword();
	}
	else {
		p = BigInteger(this->p);
		q = BigInteger(this->q);
		d = BigInteger(this->d);
		dp = BigInteger(this->dp);
		dq = BigInteger(this->dq);
		qinv = BigInteger(this->qinv);
	}
}

RSAKey::RSAKey(array<array<Byte>^>^ nums, Byte flags)
{
	p = (array<Byte>^)nums[4]->Clone();
	q = (array<Byte>^)nums[5]->Clone();
	n = BigInteger(nums[1]);
	e = BigInteger(nums[2]);
	d = (array<Byte>^)nums[3]->Clone();
	dp = (array<Byte>^)nums[6]->Clone();
	dq = (array<Byte>^)nums[7]->Clone();
	qinv = (array<Byte>^)nums[8]->Clone();
	_pswrd = ((_protected = (nums[0]->Length > 0))) ? gcnew SHA512Password("", nums[0]) : gcnew SHA512Password();

	_setPublicSafeness();
	_fermatSafe = (flags & 1) == 1;
	_wienerSafe = (flags > 1);
	_setBlockLength();
}

void RSAKey::_setBlockLength()
{
	_bitLength = RSAInt::bitCount(n);
	_blockLength = ((_bitLength + 7) >> 3) - 1;
}

void RSAKey::_setFermatSafeness(BigInteger% p, BigInteger% q)
{
	Int32 absBitCount = RSAInt::bitCount(BigInteger::Abs(BigInteger::Subtract(p, q)));
	Int32 fermatBitCount = (_bitLength >> 1) - 100;
	_fermatSafe = (fermatBitCount > 0 && (absBitCount > fermatBitCount));
}

void RSAKey::_setPublicSafeness()
{
	UInt32 eLength = RSAInt::bitCount(e);
	_publicSafe = (eLength > 16 && eLength < 256);
}

void RSAKey::_setWienerSafeness(BigInteger% d)
{
	_wienerSafe = (RSAInt::bitCount(d) > (_bitLength >> 1));
}

void RSAKey::_openPassword(String^ password)
{
	_pswrd = gcnew SHA512Password(password, _pswrd->Salt);
}

void RSAKey::_closePassword()
{
	_pswrd = gcnew SHA512Password(_pswrd->Salt);
}

void RSAKey::_checkKeyIntegrity(String^ password)
{
	_openPassword(password);

	BigInteger P = BigInteger(AES256::decrypt(p, _pswrd->Hash));
	BigInteger Q = BigInteger(AES256::decrypt(q, _pswrd->Hash));

	if(P * Q != n) {
		_closePassword();
		throw gcnew UnauthorizedAccessException("Wrong password provided.");
	}
	P = Q = BigInteger::Zero;
}

void RSAKey::_encryptKey(String^ password)
{
	_pswrd = gcnew SHA512Password(password);

	p = AES256::encrypt(p, _pswrd->Hash);
	q = AES256::encrypt(q, _pswrd->Hash);
	d = AES256::encrypt(d, _pswrd->Hash);
	dp = AES256::encrypt(dp, _pswrd->Hash);
	dq = AES256::encrypt(dq, _pswrd->Hash);
	qinv = AES256::encrypt(qinv, _pswrd->Hash);

	_closePassword();
	_protected = true;
}

void RSAKey::_decryptKey(String^ password)
{
	_checkKeyIntegrity(password);

	p = AES256::decrypt(p, _pswrd->Hash);
	q = AES256::decrypt(q, _pswrd->Hash);
	d = AES256::decrypt(d, _pswrd->Hash);
	dp = AES256::decrypt(dp, _pswrd->Hash);
	dq = AES256::decrypt(dq, _pswrd->Hash);
	qinv = AES256::decrypt(qinv, _pswrd->Hash);

	_closePassword();
	_protected = false;
}