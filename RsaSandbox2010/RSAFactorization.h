#pragma once

#include "RSAKey.h"
#include "CVUtilities.h"

namespace RsaSandbox
{
	namespace Attacks
	{
		using namespace CVStructures;
		using namespace RsaSandbox;

		public enum class RSAFactorizationMode { Naive, Fermat };

		ref class RSAFactorization;

		public delegate void RSAFactorizationOutcomeHandler(RSAFactorization^ sender, RSAKey^ brokenKey, bool cancelled);

		public ref class RSAFactorization : public BackgroundTask
		{
			public:
				property RSAFactorizationMode Mode {
					public: RSAFactorizationMode get() { return _mode; }
					public: void set(RSAFactorizationMode value) { _mode = value; }
				}

				event RSAFactorizationOutcomeHandler^ Outcome;

				RSAFactorization();

				void factor(RSAKey^ key);

			protected:
				virtual void _run() override;
				virtual void _done() override;
				virtual void _doneCancelled() override;

			private:
				RSAKey^ _key;
				RSAFactorizationMode _mode;
				
				void _runNaive();
				void _runFermat();

				void _cleanUp();
		};
	}
}