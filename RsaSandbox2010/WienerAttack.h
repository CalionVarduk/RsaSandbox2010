#pragma once

#include "RSAKey.h"
#include "CVUtilities.h"

namespace RsaSandbox
{
	namespace Attacks
	{
		using namespace CVStructures;
		using namespace RsaSandbox;

		public enum class WienerAttackOutcome { OK, Cancelled, Unsuccessful };

		ref class WienerAttack;

		public delegate void WienerAttackOutcomeHandler(WienerAttack^ sender, RSAKey^ brokenKey, WienerAttackOutcome outcome);

		public ref class WienerAttack : public BackgroundTask
		{
			public:
				event WienerAttackOutcomeHandler^ Outcome;

				WienerAttack();

				void tryToBreak(RSAKey^ key);

			protected:
				virtual void _run() override;
				virtual void _done() override;
				virtual void _doneCancelled() override;

			private:
				RSAKey^ _key;
				BigContinuedFraction^ _fraction;

				void _cleanUp();
		};
	}
}