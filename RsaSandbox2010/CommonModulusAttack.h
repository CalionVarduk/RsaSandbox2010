#pragma once

#include "Toolbox.h"
#include "RSAKey.h"
#include "SecureRandom.h"
#include "CVUtilities.h"

namespace RsaSandbox
{
	namespace Attacks
	{
		using namespace CVStructures;
		using namespace RsaSandbox;

		ref class CommonModulusAttack;

		public delegate void CommonModulusAttackOutcomeHandler(CommonModulusAttack^ sender, RSAKey^ brokenKey1, RSAKey^ brokenKey2, bool cancelled);

		public ref class CommonModulusAttack : public BackgroundTask
		{
			public:
				event CommonModulusAttackOutcomeHandler^ Outcome;

				CommonModulusAttack();

				void tryToBreak(RSAKey^ key1, RSAKey^ key2);

			protected:
				virtual void _run() override;
				virtual void _done() override;
				virtual void _doneCancelled() override;

			private:
				SecureRandom^ _rng;
				RSAKey^ _key1;
				RSAKey^ _key2;

				void _cleanUp();
		};
	}
}