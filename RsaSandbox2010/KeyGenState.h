#pragma once

namespace RsaSandbox
{
	namespace Generation
	{
		using namespace System;

		public ref class KeyGenState
		{
			public:
				static KeyGenState^ StandBy = gcnew KeyGenState("Chilling out...");
				static KeyGenState^ GeneratingP = gcnew KeyGenState("Searching for 'p'...");
				static KeyGenState^ GeneratingQ = gcnew KeyGenState("'p' has been found! Searching for 'q'...");
				static KeyGenState^ GeneratingAux = gcnew KeyGenState("Searching for auxiliary primes...");

				property String^ Info {
					public: String^ get() { return _info; }
				}

			private:
				KeyGenState(String^ info) { _info = info; }

				String^ _info;
		};
	}
}