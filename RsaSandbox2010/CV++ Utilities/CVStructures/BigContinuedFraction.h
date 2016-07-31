#pragma once

namespace CVStructures
{
	using namespace System;
	using namespace System::Numerics;
	using namespace System::Collections::Generic;

	public ref class BigContinuedFraction
	{
		public:
			property BigInteger Numerator {
				public: BigInteger get() { return _numerator; }
				public: void set(BigInteger value) { this->set(value, _denominator); }
			}

			property BigInteger Denominator {
				public: BigInteger get() { return _denominator; }
				public: void set(BigInteger value) { this->set(_numerator, value); }
			}

			property Int32 TermCount {
				public: Int32 get() { return _terms->Count; }
			}

			BigContinuedFraction()
			{
				_terms = gcnew List<BigInteger>();
				set(BigInteger::Zero, BigInteger::One);
			}

			BigContinuedFraction(BigInteger% numerator, BigInteger% denominator)
			{
				_terms = gcnew List<BigInteger>();
				set(numerator, denominator);
			}

			void set(BigInteger% numerator, BigInteger% denominator)
			{
				if(denominator == BigInteger::Zero) throw gcnew DivideByZeroException();

				if(denominator < BigInteger::Zero) {
					_denominator = -denominator;
					_numerator = -numerator;
				}
				else {
					_denominator = denominator;
					_numerator = numerator;
				}

				_shorten();
				_generateTerms();
			}

			BigInteger getTerm(Int32 i)
			{
				return _terms[i];
			}

			void getConvergent(Int32 i, BigInteger% numerator, BigInteger% denominator)
			{
				numerator = BigInteger::One;
				denominator = _terms[i];
				BigInteger t;

				while(i > 0) {
					t = denominator;
					denominator = BigInteger::Add(numerator, _terms[--i] * denominator);
					numerator = t;
				}

				t = denominator;
				denominator = numerator;
				numerator = t;
			}

			Tuple<BigInteger, BigInteger>^ getConvergent(Int32 i)
			{
				BigInteger n, d;
				getConvergent(i, n, d);
				return Tuple::Create(n, d);
			}

			List<Tuple<BigInteger, BigInteger>^>^ getConvergents()
			{
				List<Tuple<BigInteger, BigInteger>^>^ result = gcnew List<Tuple<BigInteger, BigInteger>^>();
				for(Int32 i = 0; i < _terms->Count; ++i)
					result->Add(getConvergent(i));
				return result;
			}

		private:
			BigInteger _numerator;
			BigInteger _denominator;
			List<BigInteger>^ _terms;

			void _generateTerms()
			{
				_terms->Clear();
				_terms->TrimExcess();

				BigInteger a = BigInteger::Abs(_numerator);
				BigInteger b = _denominator;
				BigInteger r;

				while(b > BigInteger::One) {
					_terms->Add(BigInteger::DivRem(a, b, r));
					a = b;
					b = r;
				}
				_terms->Add(a);
				_terms->TrimExcess();
				if(_numerator < BigInteger::Zero) _terms[0] = -_terms[0];
			}

			void _shorten()
			{
				BigInteger a = BigInteger::Abs(_numerator);
				BigInteger b = _denominator;
				BigInteger r;

				while(b > BigInteger::Zero) {
					r = a % b;
					a = b;
					b = r;
				}
				_numerator /= a;
				_denominator /= a;
			}
	};
}