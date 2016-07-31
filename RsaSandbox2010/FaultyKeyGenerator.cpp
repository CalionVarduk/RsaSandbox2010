#include "FaultyKeyGenerator.h"

using namespace RsaSandbox::Generation;

FaultyKeyGenerator::FaultyKeyGenerator()
{
	_primeTester = gcnew PrimalityTester();
	_rng = _primeTester->RNG;
	_cleanUp();
}

void FaultyKeyGenerator::fermatUnsafe(Int32 pLength, Int32 eLength, Int32 diffLength)
{
	_fermatUnsafe = true;
	_pLength = pLength;
	_eLength = eLength;
	_diffLength = diffLength;
	_handleStartUp();
}

void FaultyKeyGenerator::wienerUnsafe(Int32 pLength, Int32 dLength)
{
	_fermatUnsafe = false;
	_pLength = pLength;
	_eLength = dLength;
	_handleStartUp();
}

void FaultyKeyGenerator::_run()
{
	if(_fermatUnsafe) _runFermatUnsafe();
	else _runWienerUnsafe();
}

void FaultyKeyGenerator::_done()
{
	RSAKey^ key = _key;
	_cleanUp();
	Outcome(this, key);
}

void FaultyKeyGenerator::_doneCancelled() {}
				
void FaultyKeyGenerator::_runFermatUnsafe()
{
	BigInteger p = _rng->getBigInteger(_pLength);
	while(!_primeTester->isPrime(p, 20))
		p = BigInteger::Add(p, 2);

	BigInteger q = BigInteger::Add(p, BigInteger::Add(_rng->getBigInteger(_diffLength), 1));
	while(!_primeTester->isPrime(q, 20))
		q = BigInteger::Add(q, 2);

	_key = gcnew RSAKey(p, q);
	BigInteger e = _rng->getBigInteger(_eLength);
	while(!_key->trySetExponents(e))
		e = BigInteger::Add(e, 2);
}

void FaultyKeyGenerator::_runWienerUnsafe()
{
	BigInteger p = _rng->getBigInteger(_pLength);
	while(!_primeTester->isPrime(p, 20))
		p = BigInteger::Add(p, 2);

	BigInteger q = _rng->getBigInteger(_pLength);
	while(!_primeTester->isPrime(q, 20) || p == q)
		q = BigInteger::Add(q, 2);

	_key = gcnew RSAKey(p, q);
	BigInteger e = _rng->getBigInteger(_eLength);
	while(!_key->trySetExponents(e))
		e = BigInteger::Add(e, 2);

	_key->invertExponents();
}

void FaultyKeyGenerator::_cleanUp()
{
	_pLength = _eLength = _diffLength = 0;
	_fermatUnsafe = true;
	_key = nullptr;
}