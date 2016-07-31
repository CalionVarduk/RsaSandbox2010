#include "RSAFactorization.h"

using namespace RsaSandbox::Attacks;

RSAFactorization::RSAFactorization()
{
	ReportProgressInterval = 1000;
	_mode = RSAFactorizationMode::Fermat;
	_cleanUp();
}

void RSAFactorization::factor(RSAKey^ key)
{
	if(!key->AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. Exponents must be set before factorization.");
	_key = key;
	_handleStartUp();
}

void RSAFactorization::_run()
{
	if(_mode == RSAFactorizationMode::Naive) _runNaive();
	else _runFermat();
}

void RSAFactorization::_done()
{
	RSAKey^ brokenKey = _key;
	_cleanUp();
	Outcome(this, brokenKey, false);
}

void RSAFactorization::_doneCancelled()
{
	_cleanUp();
	Outcome(this, nullptr, true);
}
				
void RSAFactorization::_runNaive()
{
	Int32 c = 0;
	BigInteger e = _key->PublicExponent;
	BigInteger n = _key->Modulus;
	BigInteger sqrtn = RSAInt::intSqrt(n);
	BigInteger k = 3;

	while(k <= sqrtn) {
		if((n % k) == BigInteger::Zero) {
			BigInteger p = k;
			BigInteger q = n / k;
			_key = gcnew RSAKey(p, q);
			_key->trySetExponents(e);
			return;
		}
		k = BigInteger::Add(k, 2);

		if(++c >= 50) {
			c = 0;
			_handleReportProgress();
			if(_handleCancellation()) return;
		}
	}
}

void RSAFactorization::_runFermat()
{
	Int32 c = 0;
	BigInteger e = _key->PublicExponent;
	BigInteger n = _key->Modulus;

	BigInteger a = RSAInt::intSqrt(n);
	if(a * a < n) ++a;

	BigInteger a2n = BigInteger::Subtract(a * a, n);
	BigInteger b = RSAInt::intSqrt(a2n);

	while(b * b != a2n) {
		++a;
		a2n = BigInteger::Subtract(a * a, n);
		b = RSAInt::intSqrt(a2n);

		if(++c >= 50) {
			c = 0;
			_handleReportProgress();
			if(_handleCancellation()) return;
		}
	}

	BigInteger p = BigInteger::Subtract(a, b);
	BigInteger q = BigInteger::Add(a, b);
	_key = gcnew RSAKey(p, q);
	_key->trySetExponents(e);
}

void RSAFactorization::_cleanUp()
{
	_key = nullptr;
}