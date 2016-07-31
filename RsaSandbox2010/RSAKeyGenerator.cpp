#include "RSAKeyGenerator.h"

using namespace RsaSandbox::Generation;

RSAKeyGenerator::RSAKeyGenerator()
{
	_primeTester = gcnew PrimalityTester();
	_rng = _primeTester->RNG;
	_cleanUp();
}

void RSAKeyGenerator::sequential(UInt32 eLength, UInt32 pLength, UInt32 qLength, UInt32 mrRounds, UInt32 pSteps, UInt32 qSteps, bool safePrimes)
{
	_mode = (safePrimes) ? GenOperatingMode::SeqFiniteSafe : GenOperatingMode::SeqFinite;
	_eBits = eLength;
	_pBits = pLength;
	_qBits = qLength;
	_rounds = mrRounds;
	_pSteps = pSteps;
	_qSteps = qSteps;
	_handleStartUp();
}

void RSAKeyGenerator::sequential(UInt32 eLength, UInt32 pLength, UInt32 qLength, UInt32 mrRounds, bool safePrimes)
{
	_mode = (safePrimes) ? GenOperatingMode::SeqInfiniteSafe : GenOperatingMode::SeqInfinite;
	_eBits = eLength;
	_pBits = pLength;
	_qBits = qLength;
	_rounds = mrRounds;
	_handleStartUp();
}

void RSAKeyGenerator::sequential65537(UInt32 pLength, UInt32 qLength, UInt32 mrRounds, UInt32 pSteps, UInt32 qSteps, bool safePrimes)
{
	sequential(0, pLength, qLength, mrRounds, pSteps, qSteps, safePrimes);
}

void RSAKeyGenerator::sequential65537(UInt32 pLength, UInt32 qLength, UInt32 mrRounds, bool safePrimes)
{
	sequential(0, pLength, qLength, mrRounds, safePrimes);
}

void RSAKeyGenerator::dss(UInt32 eLength, UInt32 nLength, UInt32 mrRounds)
{
	_mode = GenOperatingMode::DSSRandom;
	_eBits = eLength;
	_nBits = nLength;
	_rounds = mrRounds;
	_handleStartUp();
}

void RSAKeyGenerator::dss65537(UInt32 nLength, UInt32 mrRounds)
{
	dss(0, nLength, mrRounds);
}

void RSAKeyGenerator::dssWithConditions(UInt32 eLength, UInt32 nLength, UInt32 mrRounds)
{
	_mode = GenOperatingMode::DSSConditions;
	_eBits = eLength;
	_nBits = nLength;
	_rounds = mrRounds;
	_handleStartUp();
}

void RSAKeyGenerator::dssWithConditions65537(UInt32 nLength, UInt32 mrRounds)
{
	dssWithConditions(0, nLength, mrRounds);
}

void RSAKeyGenerator::setRandomExponents(RSAKey^ key, UInt32 eLength)
{
	if(eLength >= (UInt32)key->BitLength) --eLength;
	BigInteger e = _rng->getBigInteger(eLength);
	while(!key->trySetExponents(e)) e = BigInteger::Add(e, 2);
}

void RSAKeyGenerator::_run()
{
	if(_mode == GenOperatingMode::SeqFinite) _runSeqFinite();
	else if(_mode == GenOperatingMode::SeqFiniteSafe) _runSeqFiniteSafe();
	else if(_mode == GenOperatingMode::SeqInfinite) _runSeqInfinite();
	else if(_mode == GenOperatingMode::SeqInfiniteSafe) _runSeqInfiniteSafe();
	else if(_mode == GenOperatingMode::DSSRandom) _runDssRandom();
	else _runDssConditions();
}

void RSAKeyGenerator::_done()
{
	RSAKey^ key = nullptr;
	KeyGenOutcome outcome;

	if(_pPrime != BigInteger::Zero && _qPrime != BigInteger::Zero) {
		if(BigInteger::GreatestCommonDivisor(_pPrime, _qPrime) == BigInteger::One) {
			outcome = KeyGenOutcome::OK;
			key = gcnew RSAKey(_pPrime, _qPrime);
			if(_publExp != BigInteger::Zero) key->trySetExponents(_publExp);
			if(_eBits > 0) setRandomExponents(key, _eBits);
			else key->trySetExponents(BigInteger(65537));
		}
		else outcome = KeyGenOutcome::NotFound;
	}
	else outcome = KeyGenOutcome::NotFound;

	_cleanUp();
	Outcome(this, key, outcome);
}

void RSAKeyGenerator::_doneCancelled()
{
	_cleanUp();
	Outcome(this, nullptr, KeyGenOutcome::Cancelled);
}

void RSAKeyGenerator::_runSeqFinite()
{
	_genState = KeyGenState::GeneratingP;
	if(_sequentialPrimeSearch(_pPrime, _pBits, _rounds, _pSteps)) {
		_genState = KeyGenState::GeneratingQ;
		_sequentialPrimeSearch(_qPrime, _qBits, _rounds, _qSteps);
	}
}

void RSAKeyGenerator::_runSeqInfinite()
{
	_genState = KeyGenState::GeneratingP;
	if(_sequentialPrimeSearch(_pPrime, _pBits, _rounds)) {
		_genState = KeyGenState::GeneratingQ;
		_sequentialPrimeSearch(_qPrime, _qBits, _rounds);
	}
}

void RSAKeyGenerator::_runSeqFiniteSafe()
{
	_genState = KeyGenState::GeneratingP;
	if(_sequentialSafePrimeSearch(_pPrime, _pBits, _rounds, _pSteps)) {
		_genState = KeyGenState::GeneratingQ;
		_sequentialSafePrimeSearch(_qPrime, _qBits, _rounds, _qSteps);
	}
}

void RSAKeyGenerator::_runSeqInfiniteSafe()
{
	_genState = KeyGenState::GeneratingP;
	if(_sequentialSafePrimeSearch(_pPrime, _pBits, _rounds)) {
		_genState = KeyGenState::GeneratingQ;
		_sequentialSafePrimeSearch(_qPrime, _qBits, _rounds);
	}
}

void RSAKeyGenerator::_runDssRandom()
{
	BigInteger lowerBound, fermatBound;
	_prepareDssConstants(lowerBound, fermatBound);

	_genState = KeyGenState::GeneratingP;
	if(_dssFirstPrime(lowerBound)) {
		_genState = KeyGenState::GeneratingQ;
		_dssSecondPrime(lowerBound, fermatBound);
	}
}

void RSAKeyGenerator::_runDssConditions()
{
	UInt32 auxMinBits, auxMaxBits;
	BigInteger lowerBound, fermatBound, x1, x2, xp, xq;
	_prepareDssConstants(lowerBound, fermatBound);
	_prepareDssConditional(auxMinBits, auxMaxBits);

	UInt32 x1bits = _rng->getUInt32(auxMinBits, auxMaxBits);
	UInt32 x2bits = _rng->getUInt32(auxMinBits, auxMaxBits);

	_genState = KeyGenState::GeneratingAux;
	if(_sequentialPrimeSearch(x1, x1bits, _rounds) && _sequentialPrimeSearch(x2, x2bits, _rounds)) {
		_genState = KeyGenState::GeneratingP;
		if(!_dssConditionalPrime(_pPrime, xp, x1, x2, lowerBound)) return;

		do {
			x1bits = _rng->getUInt32(auxMinBits, auxMaxBits);
			x2bits = _rng->getUInt32(auxMinBits, auxMaxBits);

			_genState = KeyGenState::GeneratingAux;
			if(_sequentialPrimeSearch(x1, x1bits, _rounds) && _sequentialPrimeSearch(x2, x2bits, _rounds)) {
				_genState = KeyGenState::GeneratingQ;
				if(!_dssConditionalPrime(_qPrime, xq, x1, x2, lowerBound)) return;
			}

			if(_handleCancellation()) return;
		}
		while(BigInteger::Abs(BigInteger::Subtract(xp, xq)) <= fermatBound || BigInteger::Abs(BigInteger::Subtract(_pPrime, _qPrime)) <= fermatBound);
	}
}

bool RSAKeyGenerator::_sequentialPrimeSearch(BigInteger% OUT_prime, UInt32 bitLength, UInt32 rounds, UInt32 steps)
{
	StepCount = steps;
	_resetCurrentStep();
	OUT_prime = _rng->getBigInteger(bitLength);

	while(CurrentStep < StepCount) {
		if(_primeTester->isPrime(OUT_prime, rounds)) return true;
		OUT_prime = BigInteger::Add(OUT_prime, 2);
		_increaseCurrentStep(2);

		_handleReportProgress();
		if(_handleCancellation()) return false;
	}
	OUT_prime = BigInteger::Zero;
	return false;
}

bool RSAKeyGenerator::_sequentialPrimeSearch(BigInteger% OUT_prime, UInt32 bitLength, UInt32 rounds)
{
	StepCount = 0;
	_resetCurrentStep();
	OUT_prime = _rng->getBigInteger(bitLength);

	while(!_primeTester->isPrime(OUT_prime, rounds)) {
		OUT_prime = BigInteger::Add(OUT_prime, 2);
		_increaseCurrentStep(2);

		_handleReportProgress();
		if(_handleCancellation()) return false;
	}
	return true;
}

bool RSAKeyGenerator::_sequentialSafePrimeSearch(BigInteger% OUT_prime, UInt32 bitLength, UInt32 rounds, UInt32 steps)
{
	StepCount = steps;
	_resetCurrentStep();
	OUT_prime = _rng->getBigInteger(bitLength);

	while(CurrentStep < StepCount) {
		if(_primeTester->isSafePrime(OUT_prime, rounds)) return true;
		OUT_prime = BigInteger::Add(OUT_prime, 2);
		_increaseCurrentStep(2);

		_handleReportProgress();
		if(_handleCancellation()) return false;
	}
	OUT_prime = BigInteger::Zero;
	return false;
}

bool RSAKeyGenerator::_sequentialSafePrimeSearch(BigInteger% OUT_prime, UInt32 bitLength, UInt32 rounds)
{
	StepCount = 0;
	_resetCurrentStep();
	OUT_prime = _rng->getBigInteger(bitLength);

	while(!_primeTester->isSafePrime(OUT_prime, rounds)) {
		OUT_prime = BigInteger::Add(OUT_prime, 2);
		_increaseCurrentStep(2);

		_handleReportProgress();
		if(_handleCancellation()) return false;
	}
	return true;
}

bool RSAKeyGenerator::_dssFirstPrime(BigInteger% lowerBound)
{
	UInt32 nBitsHalf = _nBits >> 1;
	StepCount = 5 * nBitsHalf;

	for(_resetCurrentStep(); CurrentStep < StepCount; _incrementCurrentStep()) {
		do { _pPrime = _rng->getBigInteger(nBitsHalf); } while(_pPrime < lowerBound);
		if(BigInteger::GreatestCommonDivisor(BigInteger::Subtract(_pPrime, 1), _publExp) == BigInteger::One)
			if(_primeTester->isPrime(_pPrime, _rounds)) return true;

		_handleReportProgress();
		if(_handleCancellation()) return false;
	}
	_pPrime = BigInteger::Zero;
	return false;
}

bool RSAKeyGenerator::_dssSecondPrime(BigInteger% lowerBound, BigInteger% fermatBound)
{
	UInt32 nBitsHalf = _nBits >> 1;
	StepCount = 5 * nBitsHalf;

	for(_resetCurrentStep(); CurrentStep < StepCount; _incrementCurrentStep()) {
		do { _qPrime = _rng->getBigInteger(nBitsHalf); }
		while((BigInteger::Abs(BigInteger::Subtract(_pPrime, _qPrime)) <= fermatBound) || (_qPrime < lowerBound));

		if(BigInteger::GreatestCommonDivisor(BigInteger::Subtract(_qPrime, 1), _publExp) == BigInteger::One)
			if(_primeTester->isPrime(_qPrime, _rounds)) return true;

		_handleReportProgress();
		if(_handleCancellation()) return false;
	}
	_qPrime = BigInteger::Zero;
	return false;
}

bool RSAKeyGenerator::_dssConditionalPrime(BigInteger% OUT_prime, BigInteger% OUT_x, BigInteger% x1, BigInteger% x2, BigInteger% lowerBound)
{
	x1 <<= 1;
	if(BigInteger::GreatestCommonDivisor(x1, x2) != BigInteger::One) return false;

	UInt32 nBitsHalf = _nBits >> 1;
	BigInteger x1x2 = x1 * x2;
	BigInteger x2inv = RSAInt::modularInverse(x2, x1);
	BigInteger x1inv = RSAInt::modularInverse(x1, x2);
	BigInteger R = BigInteger::Subtract(x2inv * x2, x1inv * x1);
	BigInteger pow2 = BigInteger::One << nBitsHalf;
		
	_resetCurrentStep();
	StepCount = 5 * nBitsHalf;
	BigInteger Rx;

	while(CurrentStep < StepCount) {
		do { OUT_x = _rng->getBigInteger(nBitsHalf); } while(OUT_x < lowerBound);

		Rx = BigInteger::Subtract(R, OUT_x) % x1x2;
		if(Rx < BigInteger::Zero) Rx = BigInteger::Add(Rx, x1x2);

		OUT_prime = BigInteger::Add(OUT_x, Rx);

		while(CurrentStep < StepCount && OUT_prime < pow2) {
			if(BigInteger::GreatestCommonDivisor(BigInteger::Subtract(OUT_prime, 1), _publExp) == BigInteger::One)
				if(_primeTester->isPrime(OUT_prime, _rounds)) return true;

			_handleReportProgress();
			if(_handleCancellation()) return false;

			_incrementCurrentStep();
			OUT_prime = BigInteger::Add(OUT_prime, x1x2);
		}
	}
	OUT_prime = BigInteger::Zero;
	return false;
}

void RSAKeyGenerator::_prepareDssConstants(BigInteger% OUT_lowerBound, BigInteger% OUT_fermatBound)
{
	UInt32 nBitsHalf = _nBits >> 1;
	_publExp = (_eBits > 0) ? _rng->getBigInteger(_eBits) : 65537LL;
	OUT_fermatBound = BigInteger::One << (nBitsHalf - 100);
	OUT_lowerBound = BigInteger::One << (nBitsHalf - 1);
	OUT_lowerBound = BigInteger::Add(OUT_lowerBound, (OUT_lowerBound * 215LL) >> 9);	// 215 / 512 --> near sqrt(2) - 1 ~= 0.42
}

void RSAKeyGenerator::_prepareDssConditional(UInt32% auxMinLength, UInt32% auxMaxLength)
{
	UInt32 nBitsHalf = _nBits >> 1;
	UInt32 lgHalf = (UInt32)Math::Log((double)nBitsHalf, 2);

	auxMinLength = _nBits / (lgHalf + 1);
	auxMaxLength = (nBitsHalf - lgHalf - 6) >> 1;
}

void RSAKeyGenerator::_cleanUp()
{
	_genState = KeyGenState::StandBy;
	_mode = GenOperatingMode::StandBy;
	_eBits = _pBits = _qBits = _nBits = _rounds = _pSteps = _qSteps = 0;
	_publExp = _pPrime = _qPrime = BigInteger::Zero;
}