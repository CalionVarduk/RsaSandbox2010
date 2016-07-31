#include "WienerAttack.h"

using namespace RsaSandbox::Attacks;

WienerAttack::WienerAttack()
{
	_cleanUp();
}

void WienerAttack::tryToBreak(RSAKey^ key)
{
	if(!key->AreExponentsSet) throw gcnew InvalidOperationException("The key is incomplete. Exponents must be set before performing an attack.");
	_key = key;
	_fraction = gcnew BigContinuedFraction(key->PublicExponent, key->Modulus);
	_handleStartUp(_fraction->TermCount - 1);
}

void WienerAttack::_run()
{
	while(CurrentStep < StepCount) {
		BigInteger k, d;
		_fraction->getConvergent(CurrentStep + 1, k, d);
		BigInteger ed = _key->PublicExponent * d;

		if(ed % k == BigInteger::One) {
			BigInteger m = BigInteger::Subtract(ed, 1) / k;

			if(m < _key->Modulus) {
				BigInteger s = BigInteger::Add(BigInteger::Subtract(_key->Modulus, m), 1);
				BigInteger discriminant = BigInteger::Subtract(s * s, _key->Modulus << 2);

				if(discriminant >= BigInteger::Zero) {
					BigInteger discrSqrt = RSAInt::intSqrt(discriminant);

					if((discrSqrt * discrSqrt) == discriminant) {
						BigInteger p = BigInteger::Subtract(s, discrSqrt) >> 1;
						BigInteger q = BigInteger::Add(s, discrSqrt) >> 1;

						if((p > BigInteger::One) && (q > BigInteger::One) && (p * q == _key->Modulus)) {
							BigInteger e = _key->PublicExponent;
							_key = gcnew RSAKey(p, q);
							_key->trySetExponents(e);
							return;
						}
					}
				}
			}
		}
		
		_incrementCurrentStep();
		_handleReportProgress();
		if(_handleCancellation()) return;
	}
	_key = nullptr;
}

void WienerAttack::_done()
{
	RSAKey^ brokenKey = nullptr;
	WienerAttackOutcome outcome;

	if(_key != nullptr) {
		brokenKey = _key;
		outcome = WienerAttackOutcome::OK;
	}
	else outcome = WienerAttackOutcome::Unsuccessful;

	_cleanUp();
	Outcome(this, brokenKey, outcome);
}

void WienerAttack::_doneCancelled()
{
	_cleanUp();
	Outcome(this, nullptr, WienerAttackOutcome::Cancelled);
}
			
void WienerAttack::_cleanUp()
{
	_key = nullptr;
	_fraction = nullptr;
}