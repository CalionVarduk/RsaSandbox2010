#include "CommonModulusAttack.h"

using namespace RsaSandbox::Attacks;

CommonModulusAttack::CommonModulusAttack()
{
	_rng = gcnew SecureRandom();
	_cleanUp();
}

void CommonModulusAttack::tryToBreak(RSAKey^ key1, RSAKey^ key2)
{
	if(!key1->AreExponentsSet || !key2->AreExponentsSet) throw gcnew InvalidOperationException("The keys are incomplete. Exponents must be set before performing an attack.");	
	if(key1->PublicExponent == key2->PublicExponent) throw gcnew ArgumentException("Keys' public exponents must be different.");
	if(key1->Modulus != key2->Modulus) throw gcnew ArgumentException("Keys' modulus' must be the same.");
	
	if(key1->IsPasswordProtected) {
		if(key2->IsPasswordProtected) throw gcnew ArgumentException("Both keys are password protected. In order to perform common modulus attack, at least one key must not be password protected.");
		_key1 = key2;
		_key2 = key1;
	}
	else {
		_key1 = key1;
		_key2 = key2;
	}
	_handleStartUp();
}

void CommonModulusAttack::_run()
{
	BigInteger nm1 = BigInteger::Subtract(_key1->Modulus, 1);
	BigInteger k = BigInteger::Subtract(_key1->getPrivateExponent() * _key1->PublicExponent, 1);

	UInt32 t = 1;
	array<Byte>^ buffer = k.ToByteArray();
	while(!RSAInt::getBit(buffer, t)) ++t;
	if(buffer != nullptr) Array::Clear(buffer, 0, buffer->Length);
	buffer = nullptr;

	BigInteger r = k >> t, g;

	while(true) {
		do {
			g = _rng->getBigInteger(_rng->getUInt32(3, _key1->BitLength - 1));
			if(_rng->getUInt32(0, 2) == 1) --g;
		} while(BigInteger::GreatestCommonDivisor(g, _key1->Modulus) != BigInteger::One);
		g = BigInteger::ModPow(g, r, _key1->Modulus);

		for(UInt32 i = 0; i < t; ++i) {
			if(g != BigInteger::One && g != nm1 && ((g * g) % _key1->Modulus) == BigInteger::One) {
				BigInteger p = BigInteger::GreatestCommonDivisor(BigInteger::Subtract(g, 1), _key1->Modulus);
				BigInteger q = _key1->Modulus / p;
				BigInteger e1 = _key1->PublicExponent;
				BigInteger e2 = _key2->PublicExponent;

				_key1 = gcnew RSAKey(p, q);
				_key2 = gcnew RSAKey(p, q);
				_key1->trySetExponents(e1);
				_key2->trySetExponents(e2);
				return;
			}

			g *= g;
			g %= _key1->Modulus;
			_incrementCurrentStep();
		}

		_handleReportProgress();
		if(_handleCancellation()) return;
	}
}

void CommonModulusAttack::_done()
{
	RSAKey^ brokenKey1 = _key1;
	RSAKey^ brokenKey2 = _key2;
	_cleanUp();
	Outcome(this, brokenKey1, brokenKey2, false);
}

void CommonModulusAttack::_doneCancelled()
{
	_cleanUp();
	Outcome(this, nullptr, nullptr, true);
}

void CommonModulusAttack::_cleanUp()
{
	_key1 = nullptr;
	_key2 = nullptr;
}