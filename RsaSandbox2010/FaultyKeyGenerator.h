#pragma once

#include "RSAKey.h"
#include "CVUtilities.h"
#include "PrimalityTester.h"

namespace RsaSandbox
{
	namespace Generation
	{
		using namespace CVStructures;
		using namespace RsaSandbox;

		ref class FaultyKeyGenerator;

		public delegate void FaultyGenOutcomeHandler(FaultyKeyGenerator^ sender, RSAKey^ faultyKey);

		public ref class FaultyKeyGenerator : public BackgroundTask
		{
			public:
				event FaultyGenOutcomeHandler^ Outcome;

				FaultyKeyGenerator();

				void fermatUnsafe(Int32 pLength, Int32 eLength, Int32 diffLength);
				void wienerUnsafe(Int32 pLength, Int32 dLength);

			protected:
				virtual void _run() override;
				virtual void _done() override;
				virtual void _doneCancelled() override;

			private:
				RSAKey^ _key;
				PrimalityTester^ _primeTester;
				SecureRandom^ _rng;
				Int32 _pLength, _eLength, _diffLength;
				bool _fermatUnsafe;
				
				void _runFermatUnsafe();
				void _runWienerUnsafe();

				void _cleanUp();
		};
	}
}