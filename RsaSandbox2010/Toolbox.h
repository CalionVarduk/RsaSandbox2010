#pragma once

#include "AppIO.h"
#include "PasswordForm.h"

namespace RsaSandbox
{
	using namespace CVNetControls;

	[System::Runtime::InteropServices::DllImport("user32.dll")]
	extern int SendMessage(IntPtr hWnd, Int32 wMsg, bool wParam, Int32 lParam);

	#define SuspendDraw SendMessage(Handle, 0x0B, false, 0)
	#define ResumeDraw SendMessage(Handle, 0x0B, true, 0); Refresh()

	public ref class Toolbox abstract
	{
		public:
			static Toolbox()
			{

			}

			static bool setPassword(RSAKey^ key)
			{
				return passwordForm->setPassword(key);
			}

			static bool removePassword(RSAKey^ key)
			{
				return passwordForm->removePassword(key);
			}

			static bool getKeyPrivateComponents(RSAKey^ key, BigInteger% d, BigInteger% dp, BigInteger% dq)
			{
				return passwordForm->getKeyPrivateComponents(key, d, dp, dq);
			}

			static String^ getPassword(RSAKey^ key)
			{
				return passwordForm->getPassword(key);
			}

			static void displayKeySafetyInfo(RSAKey^ key)
			{
				StringBuilder^ sb = gcnew StringBuilder(200);
			
				sb->Append((key->BitLength < 1024) ? "Small modulus' bit length (At least 1024 bits recommended)." :
															"Modulus' bit length is large enough.");
			
				sb->Append((key->IsFermatSafe) ? "\r\n\r\nIs safe against Fermat's factorization method." :
							"\r\n\r\nIs not safe against Fermat's factorization method (Primes 'p' and 'q' are too close to each other).");

				sb->Append((key->IsWienerSafe) ? "\r\n\r\nIs safe against Wiener's attack." :
										"\r\n\r\nIs not safe against Wiener's attack (Private exponent is too small).");

				sb->Append((key->IsPublicSafe) ? "\r\n\r\nPublic exponent is of correct length." :
										"\r\n\r\nPublic exponent is not of correct length (should be between 16 and 256 bits).");

				SimpleBox::display(sb->ToString(), "RSA key safety check");
			}

			template <class T>
			static void setTitledControl(TitledControl<T>^ control, String^ title, String^ info)
			{
				Point offset = control->Offset;
				control->TitleText = title;
				control->ControlText = info;
				control->Offset = offset;
			}

			static Int32 getPntValue(Int32 bitLength)
			{
				return (Int32)((bitLength * ln2) + 0.5);
			}

			static Int32 getHLcValue(Int32 bitLength)
			{
				static const double twinPrimeConst = 1.320322;
				double hlc = bitLength * ln2;
				hlc *= hlc;
				hlc *= twinPrimeConst;
				return (Int32)(hlc + 0.5);
			}

		private:
			static PasswordForm^ passwordForm = gcnew PasswordForm();

			static const double ln2 = Math::Log(2);
	};
}