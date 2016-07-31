#include "Congruence.h"

using namespace RsaSandbox;

Congruence::Congruence()
{
	set(BigInteger::Zero, BigInteger::One);
}

Congruence::Congruence(BigInteger% remainder, BigInteger% modulus)
{
	set(remainder, modulus);
}

void Congruence::set(BigInteger% remainder, BigInteger% modulus)
{
	_modulus = (modulus > BigInteger::One) ? modulus : BigInteger::One;
	_remainder = remainder % modulus;
	if(_remainder < BigInteger::Zero) _remainder = BigInteger::Add(_remainder, _modulus);
}

BigInteger Congruence::getRemainderMultiple(BigInteger multiple)
{
	if(multiple < BigInteger::Zero) multiple = BigInteger::Zero;
	return BigInteger::Add(_remainder, _modulus * multiple);
}

Congruence^ Congruence::solve(Congruence^ c1, Congruence^ c2)
{
	BigInteger r = BigInteger::Subtract(c2->Remainder, c1->Remainder);
	if(r < BigInteger::Zero) {
		r %= c2->Modulus;
		if(r < BigInteger::Zero) r = BigInteger::Add(r, c2->Modulus);
	}
	if(r > BigInteger::Zero) r *= RSAInt::modularInverse(c1->Modulus, c2->Modulus);
	return gcnew Congruence(BigInteger::Add(c1->Remainder, r * c1->Modulus), c1->Modulus * c2->Modulus);
}

String^ Congruence::ToString()
{
	return _remainder.ToString("N0") + " mod " + _modulus.ToString("N0");
}