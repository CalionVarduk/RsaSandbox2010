#pragma once

#include "Toolbox.h"
#include "RSALengthCalculator.h"
#include "Congruence.h"
#include "CVUtilities.h"

namespace RsaSandbox
{
	namespace Attacks
	{
		using namespace CVStructures;
		using namespace RsaSandbox;
		using namespace RsaSandbox::Ciphering;

		ref class HastadAttack;

		public delegate void HastadAttackOutcomeHandler(HastadAttack^ sender, array<Byte>^ message, bool cancelled);

		public ref class HastadAttack : public BackgroundTask
		{
			public:
				event HastadAttackOutcomeHandler^ Outcome;

				HastadAttack();

				void performAttack(List<RSAKey^>^ keys, List<array<Byte>^>^ ciphers);

			protected:
				virtual void _run() override;
				virtual void _done() override;
				virtual void _doneCancelled() override;

			private:
				array<Byte>^ _readBuffer;
				
				array<Byte>^ _message;
				List<array<Byte>^>^ _ciphers;
				List<Congruence^>^ _congruences;
				Int32 e, _blockCount, _blockLength, _writeLength, _iMsg;

				void _prepareData(List<RSAKey^>^ keys, List<array<Byte>^>^ ciphers);

				bool _solveNextBlock();
				void _readCipherBlocks(Int32 iBlock);
				void _writeSolutionToMessage(BigInteger% solution);
				void _fixMessageAfterAttack();

				void _cleanUp();
		};
	}
}