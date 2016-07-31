#pragma once

#include "RSAKey.h"
#include "KeyGenState.h"
#include "PrimalityTester.h"
#include "CVUtilities.h"

namespace RsaSandbox
{
	namespace Generation
	{
		using namespace RsaSandbox;
		using namespace CVStructures;

		public enum class KeyGenOutcome { OK, Cancelled, NotFound };
		ref class RSAKeyGenerator;

		public delegate void KeyGenOutcomeHandler(RSAKeyGenerator^ sender, RSAKey^ key, KeyGenOutcome outcome);

		public ref class RSAKeyGenerator : public BackgroundTask
		{
			public:
				property KeyGenState^ State {
					public: KeyGenState^ get() { return _genState; }
				}

				event KeyGenOutcomeHandler^ Outcome;

				RSAKeyGenerator();

				void sequential(UInt32 eLength, UInt32 pLength, UInt32 qLength, UInt32 mrRounds, UInt32 pSteps, UInt32 qSteps, bool safePrimes);
				void sequential(UInt32 eLength, UInt32 pLength, UInt32 qLength, UInt32 mrRounds, bool safePrimes);

				void sequential65537(UInt32 pLength, UInt32 qLength, UInt32 mrRounds, UInt32 pSteps, UInt32 qSteps, bool safePrimes);
				void sequential65537(UInt32 pLength, UInt32 qLength, UInt32 mrRounds, bool safePrimes);

				void dss(UInt32 eLength, UInt32 nLength, UInt32 mrRounds);
				void dss65537(UInt32 nLength, UInt32 mrRounds);

				void dssWithConditions(UInt32 eLength, UInt32 nLength, UInt32 mrRounds);
				void dssWithConditions65537(UInt32 nLength, UInt32 mrRounds);

				void setRandomExponents(RSAKey^ key, UInt32 eLength);

			protected:
				virtual void _run() override;
				virtual void _done() override;
				virtual void _doneCancelled() override;

			private:
				enum class GenOperatingMode { StandBy, SeqFinite, SeqInfinite, SeqFiniteSafe, SeqInfiniteSafe, DSSRandom, DSSConditions };

				KeyGenState^ _genState;
				GenOperatingMode _mode;
				PrimalityTester^ _primeTester;
				SecureRandom^ _rng;

				UInt32 _eBits, _pBits, _qBits, _nBits, _rounds, _pSteps, _qSteps;
				BigInteger _publExp, _pPrime, _qPrime;

				void _runSeqFinite();
				void _runSeqInfinite();
				void _runSeqFiniteSafe();
				void _runSeqInfiniteSafe();
				void _runDssRandom();
				void _runDssConditions();

				bool _sequentialPrimeSearch(BigInteger% OUT_prime, UInt32 bitLength, UInt32 rounds, UInt32 steps);
				bool _sequentialPrimeSearch(BigInteger% OUT_prime, UInt32 bitLength, UInt32 rounds);
				bool _sequentialSafePrimeSearch(BigInteger% OUT_prime, UInt32 bitLength, UInt32 rounds, UInt32 steps);
				bool _sequentialSafePrimeSearch(BigInteger% OUT_prime, UInt32 bitLength, UInt32 rounds);

				bool _dssFirstPrime(BigInteger% lowerBound);
				bool _dssSecondPrime(BigInteger% lowerBound, BigInteger% fermatBound);
				bool _dssConditionalPrime(BigInteger% OUT_prime, BigInteger% OUT_x, BigInteger% x1, BigInteger% x2, BigInteger% lowerBound);
				void _prepareDssConstants(BigInteger% OUT_lowerBound, BigInteger% OUT_fermatBound);
				void _prepareDssConditional(UInt32% auxMinLength, UInt32% auxMaxLength);

				void _cleanUp();
		};
	}
}