#pragma once

#include "RSAInt.h"

namespace RsaSandbox
{
	using namespace System;
	using namespace System::Numerics;

	public ref class Congruence
	{
		public:
			static Congruence^ solve(Congruence^ c1, Congruence^ c2);

			property BigInteger Remainder {
				public: BigInteger get() { return _remainder; }
				public: void set(BigInteger value) { this->set(value, _modulus); }
			}

			property BigInteger Modulus {
				public: BigInteger get() { return _modulus; }
				public: void set(BigInteger value) { this->set(_remainder, value); }
			}

			property BigInteger Opposite {
				public: BigInteger get() { return (_remainder > BigInteger::Zero) ? BigInteger::Subtract(_modulus, _remainder) : BigInteger::Zero; }
			}

			property BigInteger Reciprocal {
				public: BigInteger get() { return (IsInvertible) ? RSAInt::modularInverse(_remainder, _modulus) : BigInteger::Zero; }
			}

			property bool IsInvertible {
				public: bool get() { return (BigInteger::GreatestCommonDivisor(_remainder, _modulus) == BigInteger::One); }
			}

			Congruence();
			Congruence(BigInteger% remainder, BigInteger% modulus);

			void set(BigInteger% remainder, BigInteger% modulus);

			BigInteger getRemainderMultiple(BigInteger multiple);

			virtual String^ ToString() override;

		private:
			BigInteger _remainder;
			BigInteger _modulus;
	};
}