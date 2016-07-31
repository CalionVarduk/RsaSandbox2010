#pragma once

#include "Toolbox.h"
#include "RSAKeyGenerator.h"

namespace RsaSandbox
{
	using namespace System;
	using namespace System::Text;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;
	using namespace System::Diagnostics;

	using namespace CVNetControls;
	using namespace RsaSandbox::Generation;

	public ref class KeyGenForm : public SecondaryForm
	{
		public:
			property RSAKey^ CurrentKey {
				public: RSAKey^ get() { return key; }
			}

			KeyGenForm(LabelButton^ controlButton) : SecondaryForm(controlButton)
			{
				this->Text = L"RSA Sandbox : Key Generator";
				this->Icon = gcnew System::Drawing::Icon(".\\cvrsa.ico");
				_initializeUI();

				keyGen = gcnew RSAKeyGenerator();
				keyGen->ReportProgress += gcnew TaskReportProgressHandler(this, &KeyGenForm::Event_KeyGenReportProgress);
				keyGen->Outcome += gcnew KeyGenOutcomeHandler(this, &KeyGenForm::Event_KeyGenOutcome);
				keyGenState = KeyGenState::StandBy;
			}

			void setExportKeyEvent(Func<bool>^ handler)
			{
				exportKey = handler;
			}

		protected:
			virtual void OnPaint(PaintEventArgs^ e) override
			{
				Form::OnPaint(e);

				Int32 endX = ClientSize.Width - 15;
				Int32 y = modeChoice->Bottom - 1;
				e->Graphics->DrawLine(Pens::SeaGreen, modeChoice[1]->Right, y, endX, y);

				if(keyNumbers[0]->Visible) {
					y = informant->Bottom + 7;
					e->Graphics->DrawLine(Pens::SteelBlue, 15, y, endX, y);
				}

				if(modeChoice[0]->Activated) {
					y = seqAttempts->Bottom + 7;
					e->Graphics->DrawLine(Pens::SteelBlue, 15, y, endX, y);

					endX = seqGenChoice->Right + 10;
					e->Graphics->DrawLine(Pens::SteelBlue, endX, seqAttempts->Top, endX, y);
				}
			}

			virtual void OnResizeEnd(EventArgs^ e) override
			{
				Form::OnResizeEnd(e);
				if(!keyNumbers[2]->Visible && keyNumbers[0]->Visible) {
					if(ClientSize.Height != keyNumbers[1]->Bottom + 12)
						ClientSize = System::Drawing::Size(seqAttempts->Right + 12, keyNumbers[1]->Bottom + 12);
				}
			}

		private:
			RSAKey^ key;
			RSAKeyGenerator^ keyGen;
			KeyGenState^ keyGenState;
			Func<bool>^ exportKey;

#pragma region UI Fields
			Panel^ menuPanel;
			LabelButtonList^ menuButtons;

			LabelButtonTitledList^ modeChoice;

			TitledCheckBoxTitledList^ seqAttempts;
			TitledCheckBoxTitledList^ seqGenChoice;
			TitledCheckBoxTitledList^ dssGenChoice;

			TitledTextBoxList^ maxAttempts;
			TitledTextBoxList^ dssLengthBoxes;
			TitledTextBoxList^ seqLengthBoxes;
			TitledTextBox^ rabinMillerSteps;

			LabelButton^ generate;
			LabelButton^ chooseExp;
			TitledLabel^ informant;
			SingleProgressBar^ progressBar;
		
			TitledTextBoxTitledList^ keyNumbers;
			LabelButtonList^ keyButtons;

			DynamicControlCollection^ movableControls;

			System::ComponentModel::Container ^components;
#pragma endregion

			void generateKey()
			{
				if(modeChoice[0]->Activated) generateKeySeq();
				else generateKeyDss();
			}

			void generateKeySeq()
			{
				Int32 eBits = 0, pBits, qBits;
				if((seqGenChoice[1]->Checked) ? parsePrimeLength(pBits, qBits) : parseAllLength(eBits, pBits, qBits)) {
					if(seqAttempts[2]->Checked) searchSeqInfinite(eBits, pBits, qBits);
					else searchSeqFinite(eBits, pBits, qBits);
				}
			}

			void generateKeyDss()
			{
				Int32 eBits = 0, nBits;
				if((dssGenChoice[1]->Checked) ? parseModLength(nBits) : parseDssLength(eBits, nBits))
					searchDss(eBits, nBits);
			}

			void searchSeqInfinite(Int32 eBits, Int32 pBits, Int32 qBits)
			{
				Int32 mrRounds;
				if(parseMRrounds(mrRounds)) {
					prepareAndLockUI(false);
					if(seqGenChoice[1]->Checked) keyGen->sequential65537(pBits, qBits, mrRounds, seqGenChoice[0]->Checked);
					else keyGen->sequential(eBits, pBits, qBits, mrRounds, seqGenChoice[0]->Checked);
				}
			}

			void searchSeqFinite(Int32 eBits, Int32 pBits, Int32 qBits)
			{
				Int32 pMax, qMax, mrRounds;
				if(parseMRrounds(mrRounds) && parseMaxAmounts(pMax, qMax)) {
					prepareAndLockUI(true);
					if(seqGenChoice[1]->Checked) keyGen->sequential65537(pBits, qBits, mrRounds, pMax, qMax, seqGenChoice[0]->Checked);
					else keyGen->sequential(eBits, pBits, qBits, mrRounds, pMax, qMax, seqGenChoice[0]->Checked);
				}
			}

			void searchDss(Int32 eBits, Int32 nBits)
			{
				Int32 mrRounds;
				if(parseMRrounds(mrRounds)) {
					prepareAndLockUI(true);
					if(dssGenChoice[1]->Checked) {
						if(dssGenChoice[0]->Checked) keyGen->dssWithConditions65537(nBits, mrRounds);
						else keyGen->dss65537(nBits, mrRounds);
					}
					else if(dssGenChoice[0]->Checked) keyGen->dssWithConditions(eBits, nBits, mrRounds);
					else keyGen->dss(eBits, nBits, mrRounds);
				}
			}

			bool chooseDifferentExponent()
			{
				if((modeChoice[0]->Activated) ? chooseDifferentExponentSeq() : chooseDifferentExponentDss()) {
					BigInteger e = key->PublicExponent;
					Toolbox::setTitledControl(keyNumbers[1], "Public Exponent (" + RSAInt::bitCount(e).ToString() + "-bit long):", e.ToString("N0"));
					
					if(keyNumbers[2]->Visible) {
						BigInteger d, dp, dq;
						key->getPrivateExponents(d, dp, dq);

						Toolbox::setTitledControl(keyNumbers[4], "Private Exponent (" + RSAInt::bitCount(d).ToString() + "-bit long):", d.ToString("N0"));
						Toolbox::setTitledControl(keyNumbers[5], "Private CRT p-Exponent (" + RSAInt::bitCount(dp).ToString() + "-bit long):", dp.ToString("N0"));
						Toolbox::setTitledControl(keyNumbers[6], "Private CRT q-Exponent (" + RSAInt::bitCount(dq).ToString() + "-bit long):", dq.ToString("N0"));
						d = dp = dq = BigInteger::Zero;
					}
					e = BigInteger::Zero;
					return true;
				}
				return false;
			}

			bool chooseDifferentExponentSeq()
			{
				if(seqGenChoice[1]->Checked) return key->trySetExponents(BigInteger(65537));

				Int32 eBits;
				if(parseExpLength(eBits) && eBits > 1 && (eBits < key->BitLength)) {
					keyGen->setRandomExponents(key, eBits);
					return true;
				}
				return false;
			}

			bool chooseDifferentExponentDss()
			{
				if(dssGenChoice[1]->Checked) return key->trySetExponents(BigInteger(65537));

				Int32 eBits;
				if(parseDssExpLength(eBits) && eBits > 1 && (eBits < key->BitLength)) {
					keyGen->setRandomExponents(key, eBits);
					return true;
				}
				return false;
			}

			void Event_KeyGenReportProgress(BackgroundTask^ sender)
			{
				checkKeyGenState();

				if(sender->IsStepCountDefined) {
					informant->ControlText = sender->CurrentStep.ToString() + " / " + sender->StepCount.ToString();
					progressBar->ProgressPercentage = sender->CurrentStep / (float)sender->StepCount;
				}
				else informant->ControlText = sender->CurrentStep.ToString();
				informant->toLabel()->Refresh();
			}

			void checkKeyGenState()
			{
				if(keyGenState != keyGen->State) {
					keyGenState = keyGen->State;
					Toolbox::setTitledControl(informant, keyGenState->Info, informant->ControlText);
					informant->Title->Refresh();
				}
			}

			void Event_KeyGenOutcome(RSAKeyGenerator^ sender, RSAKey^ key, KeyGenOutcome outcome)
			{
				this->key = key;

				if(outcome == KeyGenOutcome::OK) {
					chooseExp->Visible = true;
					prepareAndUnlockUI(true);
				}
				else {
					prepareAndUnlockUI(false);
					if(outcome == KeyGenOutcome::NotFound)
						SimpleBox::display("RSA Key hasn't been found.", "Not found");
				}
			}

#pragma region UI Events
			void enableBox(TitledTextBox^ box)
			{
				box->ReadOnly = false;
				box->TitleForeColor = seqAttempts->TitleForeColor;
				box->ControlForeColor = seqAttempts->TitleForeColor;
				box->ControlBackColor = Color::Azure;
			}

			void disableBox(TitledTextBox^ box)
			{
				box->ReadOnly = true;
				box->TitleForeColor = Color::Gray;
				box->ControlForeColor = Color::Gray;
				box->ControlBackColor = Color::Gainsboro;
			}

			void enableBoxList(TitledTextBoxList^ list)
			{
				for(Int32 i = 0; i < list->Count; ++i) enableBox(list[i]);
			}

			void disableBoxList(TitledTextBoxList^ list)
			{
				for(Int32 i = 0; i < list->Count; ++i) disableBox(list[i]);
			}

			int saveRsaKeyToFile(String^ fileName)
			{
				System::Windows::Forms::DialogResult result = SimpleBox::displayWithResult("Do you want to protect the key with a password?", "Save RSA Key to file");

				if(result == System::Windows::Forms::DialogResult::OK) {
					if(Toolbox::setPassword(key)) return (AppIO::saveRsaKey(fileName, key)) ? 3 : 0;
					else return (AppIO::saveRsaKey(fileName, key)) ? 2 : 0;
				}
				return (AppIO::saveRsaKey(fileName, key)) ? 1 : 0;
			}

			RSAKey^ loadRsaKeyFromFile(String^ fileName)
			{
				RSAKey^ newKey;
				return (AppIO::loadRsaKey(fileName, newKey)) ? newKey : nullptr;
			}

			void displaySequentialUI()
			{
				SuspendDraw;
				dssLengthBoxes->Visible = false;
				dssGenChoice->Visible = false;

				maxAttempts->Visible = !seqAttempts[2]->Checked;
				seqAttempts->Visible = true;
				seqGenChoice->Visible = true;

				seqLengthBoxes[0]->Visible = !seqGenChoice[1]->Checked;
				seqLengthBoxes[1]->Visible = true;
				seqLengthBoxes[2]->Visible = true;

				rabinMillerSteps->OffsetX = seqLengthBoxes[0]->ControlLeft - rabinMillerSteps->TitleRight;
				movableControls->Top = seqLengthBoxes->Bottom + 7;
				Int32 bottom = (!keyNumbers[0]->Visible) ? generate->Bottom : (keyNumbers[2]->Visible) ? keyNumbers->Bottom : keyNumbers[1]->Bottom;
				ClientSize = System::Drawing::Size(seqAttempts->Right + 12, bottom + 12);
				ResumeDraw;
			}

			void displayDssUI()
			{
				SuspendDraw;
				dssLengthBoxes[0]->Visible = !dssGenChoice[1]->Checked;
				dssLengthBoxes[1]->Visible = true;
				dssGenChoice->Visible = true;

				maxAttempts->Visible = false;
				seqAttempts->Visible = false;
				seqGenChoice->Visible = false;
				seqLengthBoxes->Visible = false;

				rabinMillerSteps->OffsetX = dssLengthBoxes[0]->ControlLeft - rabinMillerSteps->TitleRight;
				movableControls->Top = dssLengthBoxes->Bottom + 7;
				Int32 bottom = (!keyNumbers[0]->Visible) ? generate->Bottom : (keyNumbers[2]->Visible) ? keyNumbers->Bottom : keyNumbers[1]->Bottom;
				ClientSize = System::Drawing::Size(seqAttempts->Right + 12, bottom + 12);
				ResumeDraw;
			}

			void userDefinedAmountChosen()
			{
				if(!maxAttempts[0]->Visible || maxAttempts[0]->ReadOnly) {
					SuspendDraw;
					enableBoxList(maxAttempts);
					maxAttempts->Visible = true;
					ResumeDraw;
				}
			}

			void updatePntValues(Int32 pBits, Int32 qBits)
			{
				maxAttempts[0]->ControlText = Toolbox::getPntValue(pBits).ToString();
				maxAttempts[1]->ControlText = Toolbox::getPntValue(qBits).ToString();
			}

			void updateHLcValues(Int32 pBits, Int32 qBits)
			{
				maxAttempts[0]->ControlText = Toolbox::getHLcValue(pBits).ToString();
				maxAttempts[1]->ControlText = Toolbox::getHLcValue(qBits).ToString();
			}

			void pntHLcAmountChosen(Int32 pBits, Int32 qBits)
			{
				if(seqGenChoice[0]->Checked) updateHLcValues(pBits, qBits);
				else updatePntValues(pBits, qBits);

				if(!maxAttempts[0]->Visible || !maxAttempts[0]->ReadOnly) {
					SuspendDraw;
					disableBoxList(maxAttempts);
					maxAttempts->Visible = true;
					ResumeDraw;
				}
			}

			void untilFoundAmountChosen()
			{
				if(maxAttempts[0]->Visible) {
					SuspendDraw;
					maxAttempts->Visible = false;
					ResumeDraw;
				}
			}

			void genSafePrimesChosen(Int32 pBits, Int32 qBits)
			{
				if(seqGenChoice[0]->Checked) {
					seqAttempts[1]->TitleText = "Hardy-Littlewood conjecture:";
					if(seqAttempts[1]->Checked) updateHLcValues(pBits, qBits);
				}
				else {
					seqAttempts[1]->TitleText = "Prime number theorem:";
					if(seqAttempts[1]->Checked) updatePntValues(pBits, qBits);
				}
			}

			void prepareAndLockUI(bool progressBarVisible)
			{
				SuspendDraw;
				key = nullptr;
				seqAttempts->Enabled = false;
				seqGenChoice->Enabled = false;
				dssGenChoice->Enabled = false;
				
				disableBoxList(maxAttempts);
				disableBoxList(seqLengthBoxes);
				disableBoxList(dssLengthBoxes);
				disableBox(rabinMillerSteps);

				chooseExp->Visible = false;
				generate->Text = "CANCEL";

				keyButtons[3]->Text = "SHOW MORE";
				keyButtons->Visible = false;
				keyNumbers->Visible = false;
				for(Int32 i = 0; i < keyNumbers->Count; ++i)
					Toolbox::setTitledControl(keyNumbers[i], "", "");

				keyGenState = KeyGenState::GeneratingP;
				Toolbox::setTitledControl(informant, keyGenState->Info, "0");
				informant->Visible = true;

				if(progressBarVisible) {
					progressBar->ProgressPercentage = 0;
					progressBar->Visible = true;
					ClientSize = System::Drawing::Size(seqAttempts->Right + 12, progressBar->Bottom + 12);
				}
				else ClientSize = System::Drawing::Size(seqAttempts->Right + 12, informant->Bottom + 12);
				ResumeDraw;
			}

			void prepareAndUnlockUI(bool ok)
			{
				SuspendDraw;
				keyButtons[0]->Enabled = true;
				seqAttempts->Enabled = true;
				seqGenChoice->Enabled = true;
				dssGenChoice->Enabled = true;

				if(!seqAttempts[1]->Checked) enableBoxList(maxAttempts);
				enableBoxList(seqLengthBoxes);
				enableBoxList(dssLengthBoxes);
				enableBox(rabinMillerSteps);

				generate->Text = "GENERATE RSA KEY";
				progressBar->Visible = false;

				if(ok) showKeyDisplay(false);
				else {
					informant->Visible = false;
					ClientSize = System::Drawing::Size(seqAttempts->Right + 12, generate->Bottom + 12);
				}
				ResumeDraw;
			}

			void showKeyDisplay(bool keyLoadedFromFile)
			{
				if(keyLoadedFromFile) {
					SuspendDraw;
					informant->Visible = false;
					if(keyNumbers[2]->Visible) {
						keyButtons[3]->Text = "SHOW MORE";
						for(Int32 i = 2; i < keyNumbers->Count; ++i) {
							keyNumbers[i]->Visible = false;
							Toolbox::setTitledControl(keyNumbers[i], "", "");
						}
					}
				}
				else Toolbox::setTitledControl(informant, "Time taken:", keyGen->TimeElapsedMs.ToString("N0") + " ms");

				chooseExp->Visible = true;
				keyNumbers->Title->Visible = true;
				keyNumbers[0]->Visible = true;
				keyNumbers[1]->Visible = true;
				keyButtons->Visible = true;

				BigInteger e = key->PublicExponent;
				Toolbox::setTitledControl(keyNumbers[0], "Modulus (" + key->BitLength.ToString() + "-bit long):", key->Modulus.ToString("N0"));
				Toolbox::setTitledControl(keyNumbers[1], "Public Exponent (" + RSAInt::bitCount(e).ToString() + "-bit long):", e.ToString("N0"));
				e = BigInteger::Zero;

				ClientSize = System::Drawing::Size(seqAttempts->Right + 12, keyNumbers[1]->Bottom + 12);
				if(keyLoadedFromFile) ResumeDraw;
			}

			void invertKeyExponents()
			{
				key->invertExponents();

				BigInteger e = key->PublicExponent;
				Toolbox::setTitledControl(keyNumbers[1], "Public Exponent (" + RSAInt::bitCount(e).ToString() + "-bit long):", e.ToString("N0"));

				if(keyNumbers[2]->Visible) {
					BigInteger p, q, d, dp, dq, qinv;
					key->getAllPrivateComponents(p, q, d, dp, dq, qinv);
					Toolbox::setTitledControl(keyNumbers[2], "Prime 'p' (" + RSAInt::bitCount(p).ToString() + "-bit long):", p.ToString("N0"));
					Toolbox::setTitledControl(keyNumbers[3], "Prime 'q' (" + RSAInt::bitCount(q).ToString() + "-bit long):", q.ToString("N0"));
					Toolbox::setTitledControl(keyNumbers[4], "Private Exponent (" + RSAInt::bitCount(d).ToString() + "-bit long):", d.ToString("N0"));
					Toolbox::setTitledControl(keyNumbers[5], "Private CRT p-Exponent (" + RSAInt::bitCount(dp).ToString() + "-bit long):", dp.ToString("N0"));
					Toolbox::setTitledControl(keyNumbers[6], "Private CRT q-Exponent (" + RSAInt::bitCount(dq).ToString() + "-bit long):", dq.ToString("N0"));
					Toolbox::setTitledControl(keyNumbers[7], "Private CRT Inverted 'q' (" + RSAInt::bitCount(qinv).ToString() + "-bit long):", qinv.ToString("N0"));
					p = q = d = dp = dq = qinv = BigInteger::Zero;
				}
			}

			void clearGeneratedKey()
			{
				SuspendDraw;
				key = nullptr;
				informant->Visible = false;
				chooseExp->Visible = false;
				keyButtons[3]->Text = "SHOW MORE";
				keyButtons->Visible = false;
				keyNumbers->Visible = false;
				for(Int32 i = 0; i < keyNumbers->Count; ++i)
					Toolbox::setTitledControl(keyNumbers[i], "", "");

				ClientSize = System::Drawing::Size(seqAttempts->Right + 12, generate->Bottom + 12);
				ResumeDraw;
			}

			void showPrivateComponents()
			{
				SuspendDraw;
				keyButtons[3]->Text = "SHOW LESS";
				for(Int32 i = 2; i < keyNumbers->Count; ++i) keyNumbers[i]->Visible = true;

				BigInteger p, q, d, dp, dq, qinv;
				key->getAllPrivateComponents(p, q, d, dp, dq, qinv);
				Toolbox::setTitledControl(keyNumbers[2], "Prime 'p' (" + RSAInt::bitCount(p).ToString() + "-bit long):", p.ToString("N0"));
				Toolbox::setTitledControl(keyNumbers[3], "Prime 'q' (" + RSAInt::bitCount(q).ToString() + "-bit long):", q.ToString("N0"));
				Toolbox::setTitledControl(keyNumbers[4], "Private Exponent (" + RSAInt::bitCount(d).ToString() + "-bit long):", d.ToString("N0"));
				Toolbox::setTitledControl(keyNumbers[5], "Private CRT p-Exponent (" + RSAInt::bitCount(dp).ToString() + "-bit long):", dp.ToString("N0"));
				Toolbox::setTitledControl(keyNumbers[6], "Private CRT q-Exponent (" + RSAInt::bitCount(dq).ToString() + "-bit long):", dq.ToString("N0"));
				Toolbox::setTitledControl(keyNumbers[7], "Private CRT Inverted 'q' (" + RSAInt::bitCount(qinv).ToString() + "-bit long):", qinv.ToString("N0"));
				p = q = d = dp = dq = qinv = BigInteger::Zero;
				ClientSize = System::Drawing::Size(seqAttempts->Right + 12, keyNumbers->Bottom + 12);
				ResumeDraw;
			}

			void hidePrivateComponents()
			{
				SuspendDraw;
				keyButtons[3]->Text = "SHOW MORE";
				for(Int32 i = 2; i < keyNumbers->Count; ++i) {
					keyNumbers[i]->Visible = false;
					Toolbox::setTitledControl(keyNumbers[i], "", "");
				}
				ClientSize = System::Drawing::Size(seqAttempts->Right + 12, keyNumbers[1]->Bottom + 12);
				ResumeDraw;
			}
#pragma endregion

#pragma region Parsing
			bool parseExpLength(Int32% OUT_e)
			{
				return Int32::TryParse(seqLengthBoxes[0]->ControlText, OUT_e);
			}

			bool parseDssExpLength(Int32% OUT_e)
			{
				return Int32::TryParse(dssLengthBoxes[0]->ControlText, OUT_e);
			}

			bool parsePrimeLength(Int32% OUT_p, Int32% OUT_q)
			{
				if(Int32::TryParse(seqLengthBoxes[1]->ControlText, OUT_p) &&
					Int32::TryParse(seqLengthBoxes[2]->ControlText, OUT_q) &&
					(OUT_p > 63 && OUT_q > 63)) return true;

				SimpleBox::display("Prime bit length must be a natural number greater than 63.", "Parsing Error");
				return false;
			}

			bool parseAllLength(Int32% OUT_e, Int32% OUT_p, Int32% OUT_q)
			{
				if(parsePrimeLength(OUT_p, OUT_q)) {
					if(parseExpLength(OUT_e) && OUT_e > 1 && (OUT_e < OUT_p + OUT_q - 1)) return true;
					SimpleBox::display("Exponent bit length must be a natural number greater than 1\r\n" +
										"and less than the sum of 'p' and 'q' bit lengths minus 1.", "Parsing Error");
				}
				return false;
			}

			bool parseModLength(Int32% OUT_n)
			{
				if(Int32::TryParse(dssLengthBoxes[1]->ControlText, OUT_n) && OUT_n > 127) return true;
				SimpleBox::display("Modulus bit length must be a natural number greater than 127.", "Parsing Error");
				return false;
			}

			bool parseDssLength(Int32% OUT_e, Int32% OUT_n)
			{
				if(parseModLength(OUT_n)) {
					if(parseDssExpLength(OUT_e) && OUT_e > 1 && (OUT_e < OUT_n - 1)) return true;
					SimpleBox::display("Exponent bit length must be a natural number greater than 1\r\n" +
										"and less than the modulus bit length minus 1.", "Parsing Error");
				}
				return false;
			}

			bool parseMRrounds(Int32% OUT_rounds)
			{
				if(Int32::TryParse(rabinMillerSteps->ControlText, OUT_rounds) && OUT_rounds > 0) return true;
				SimpleBox::display("Miller-Rabin rounds count must be a natural number greater than 0.", "Parsing Error");
				return false;
			}

			bool parseMaxAmounts(Int32% OUT_p, Int32% OUT_q)
			{
				if(Int32::TryParse(maxAttempts[0]->ControlText, OUT_p) &&
					Int32::TryParse(maxAttempts[1]->ControlText, OUT_q) &&
					(OUT_p > 0 && OUT_q > 0)) return true;

				SimpleBox::display("Max amounts must be natural numbers greater than 0.", "Parsing Error");
				return false;
			}
#pragma endregion

#pragma region User Events
			void Event_SaveKey(Object^ sender, MouseEventArgs^ e)
			{
				if(key != nullptr) {
					SaveFileDialog^ dialog = gcnew SaveFileDialog();
					dialog->Filter = "RSA Key |*.rsakey";
					dialog->ShowDialog();

					if(!String::IsNullOrEmpty(dialog->FileName)) {
						int result = saveRsaKeyToFile(dialog->FileName);
						if(result == 1) SimpleBox::display("The RSA Key has been successfully saved!", "Save RSA Key to file");
						else if(result == 2) SimpleBox::display("The RSA Key has been successfully saved! (Failed to set a password.)", "Save RSA Key to file");
						else if(result == 3) SimpleBox::display("The RSA Key has been successfully saved! (Password has been set.)", "Save RSA Key to file");
						else SimpleBox::display("RSA Key saving unsuccessful.", "File IO Error");
					}
				}
				else SimpleBox::display("There is no RSA Key to save to file.", "File IO Error");
			}

			void Event_LoadKey(Object^ sender, MouseEventArgs^ e)
			{
				if(!keyGen->IsBusy) {
					OpenFileDialog^ dialog = gcnew OpenFileDialog();
					dialog->Filter = "RSA Key |*.rsakey";
					dialog->ShowDialog();

					if(!String::IsNullOrEmpty(dialog->FileName)) {
						RSAKey^ newKey = loadRsaKeyFromFile(dialog->FileName);

						if(newKey != nullptr) {
							if(newKey->IsPasswordProtected) {
								array<String^>^ msg = gcnew array<String^>(3);
								msg[0] = "The RSA Key has been successfully loaded! However, it is password protected.";
								msg[1] = "The password will have to be removed in order to load the key to the key generator.";
								msg[2] = "Do you want to continue?";
								System::Windows::Forms::DialogResult result = SimpleBox::displayWithResult(msg, "Load RSA Key from file");

								if(result == System::Windows::Forms::DialogResult::OK) {
									if(Toolbox::removePassword(newKey)) {
										key = newKey;
										showKeyDisplay(true);
										SimpleBox::display("The Password has been removed and the RSA Key has been loaded!", "Load RSA Key from file");
									}
									else SimpleBox::display("Failed to remove the password and thus the RSA Key has not been loaded.", "Load RSA Key from file");
								}
								else newKey = nullptr;
							}
							else {
								key = newKey;
								showKeyDisplay(true);
								SimpleBox::display("The RSA Key has been successfully loaded!", "Load RSA Key from file");
							}
						}
						else SimpleBox::display("RSA Key loading unsuccessful.", "File IO Error");
					}
				}
				else SimpleBox::display("Can not load an RSA Key while generating a new one.", "File IO Error");
			}

			void Event_ExportKey(Object^ sender, MouseEventArgs^ e)
			{
				if(key != nullptr) {
					if(exportKey())	clearGeneratedKey();
				}
				else SimpleBox::display("There is no RSA Key to export from the key generator.", "Export Key Error");
			}

			void Event_SeqMode(Object^ sender, MouseEventArgs^ e)
			{
				if(keyGen->IsBusy) {
					if(modeChoice[1]->Activated) {
						modeChoice[0]->deactivate();
						SimpleBox::display("Can not change key generator's mode while generating a key.", "Mode Change Error");
					}
				}
				else {
					bool changeUI = modeChoice[1]->Activated;
					modeChoice[1]->deactivate();
					if(changeUI) displaySequentialUI();
				}
			}

			void Event_DssMode(Object^ sender, MouseEventArgs^ e)
			{
				if(keyGen->IsBusy) {
					if(modeChoice[0]->Activated) {
						modeChoice[1]->deactivate();
						SimpleBox::display("Can not change key generator's mode while generating a key.", "Mode Change Error");
					}
				}
				else {
					bool changeUI = modeChoice[0]->Activated;
					modeChoice[0]->deactivate();
					if(changeUI) displayDssUI();
				}
			}

			void Event_AmountUserDef(Object^ sender, MouseEventArgs^ e)
			{
				userDefinedAmountChosen();
			}

			void Event_AmountPntHLc(Object^ sender, MouseEventArgs^ e)
			{
				Int32 pBits, qBits;
				if(parsePrimeLength(pBits, qBits)) pntHLcAmountChosen(pBits, qBits);
				else seqAttempts->singleSelection((maxAttempts[0]->Visible) ? 0 : 2);
			}

			void Event_AmountUntilFound(Object^ sender, MouseEventArgs^ e)
			{
				untilFoundAmountChosen();
			}

			void Event_GenSafe(Object^ sender, MouseEventArgs^ e)
			{
				Int32 pBits, qBits;
				if(parsePrimeLength(pBits, qBits)) genSafePrimesChosen(pBits, qBits);
				else seqGenChoice[0]->Checked = !seqGenChoice[0]->Checked;
			}

			void Event_Gen65537(Object^ sender, MouseEventArgs^ e)
			{
				seqLengthBoxes[0]->Visible = !seqGenChoice[1]->Checked;
			}

			void Event_Gen65537Dss(Object^ sender, MouseEventArgs^ e)
			{
				dssLengthBoxes[0]->Visible = !dssGenChoice[1]->Checked;
			}

			void Event_GenKey(Object^ sender, MouseEventArgs^ e)
			{
				if(e->Button == System::Windows::Forms::MouseButtons::Left) {
					if(keyGen->IsBusy) keyGen->requestCancel();
					else generateKey();
				}
			}

			void Event_ChooseExp(Object^ sender, MouseEventArgs^ e)
			{
				if(e->Button == System::Windows::Forms::MouseButtons::Left)
					if(!chooseDifferentExponent())
						SimpleBox::display("Exponent bit length must be a natural number greater than 1\r\n" +
											"and less than the RSA modulus' bit length.", "Parsing Error");
			}

			void Event_CheckKey(Object^ sender, MouseEventArgs^ e)
			{
				Toolbox::displayKeySafetyInfo(key);
			}

			void Event_InvertKey(Object^ sender, MouseEventArgs^ e)
			{
				System::Windows::Forms::DialogResult result = SimpleBox::displayWithResult("Are you sure you want to invert the key's exponents?", "Invert Exponents");
				if(result == System::Windows::Forms::DialogResult::OK) invertKeyExponents();
			}

			void Event_ClearKey(Object^ sender, MouseEventArgs^ e)
			{
				System::Windows::Forms::DialogResult result = SimpleBox::displayWithResult("Are you sure you want to clear the current key?", "Clear");
				if(result == System::Windows::Forms::DialogResult::OK) clearGeneratedKey();
			}

			void Event_ShowHide(Object^ sender, MouseEventArgs^ e)
			{
				if(keyNumbers[2]->Visible) hidePrivateComponents();
				else showPrivateComponents();
			}
#pragma endregion

#pragma region Initialization
			void _initializeUI()
			{
				DoubleBuffered = true;
				BackColor = SimpleBox::BackColor;
				FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
				MinimizeBox = true;

				_initializeMenuButtons();
				_initializeModeButtons();
				_initializeMaxAttemptsBoxes();
				_initializeSeqChoiceBoxes();
				_initializeDssChoiceBoxes();
				_initializeSeqLengthBoxes();
				_initializeDssLengthBoxes();
				_initializeMaxAttemptsInputBoxes();
				_initializeRoundsBox();
				_initializeGenerateButtons();
				_initializeProgressBar();
				_initializeKeyButtons();
				_initializeKeyBoxes();
				_initializeEvents();

				_initializeLengthBoxesAlignment();
				_initializeMovableControls();

				ClientSize = System::Drawing::Size(seqAttempts->Right + 12, generate->Bottom + 12);
				menuPanel->Width = ClientSize.Width + 2;
				maxAttempts->Right = seqAttempts->Right - 5;
				dssGenChoice->Right = seqAttempts->Right - 3;
				progressBar->Width = ClientSize.Width - 24;
				generate->Width = (progressBar->Width >> 1) - 3;
				chooseExp->Width = generate->Width;
				chooseExp->Left = generate->Right + 4;
			}

			void _initializeMenuButtons()
			{
				menuPanel = gcnew Panel();
				menuPanel->Height = 19;
				menuPanel->Location = Point(-1, -1);
				menuPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
				menuPanel->BackColor = ColorFactory::Scale(Color::SteelBlue, 1.2f);
				Controls->Add(menuPanel);

				menuButtons = gcnew LabelButtonList(menuPanel);
				menuButtons->IsListHorizontal = true;
				menuButtons->Spacing = Point(10, 0);
				menuButtons->Location = Point(5, 1);

				menuButtons->addButton("menuSave", "SAVE KEY TO FILE");
				menuButtons->addButton("menuLoad", "LOAD KEY FROM FILE");
				menuButtons->addButton("menuExport", "EXPORT KEY");

				LabelButtonScheme::Invert(menuButtons, false, menuPanel->BackColor, Color::WhiteSmoke, 1.1f);
				menuButtons->AutoSize = true;
				menuButtons->BorderStyle = gcnew RectangularBorder();

				menuButtons[0]->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9, FontStyle::Bold);
				for(Int32 i = 1; i < menuButtons->Count; ++i)
					menuButtons[i]->Font = menuButtons[0]->Font;

				menuButtons->performLayout();
			}

			void _initializeModeButtons()
			{
				modeChoice = gcnew LabelButtonTitledList(this);

				modeChoice->addButton("seqMode", "SEQUENTIAL");
				modeChoice->addButton("dssMode", "DSS");

				LabelButtonScheme::Invert(modeChoice, true, ColorFactory::Scale(Color::LightSeaGreen, 0.9f), Color::Honeydew, 1.15f);
				modeChoice->BorderColor = modeChoice[0]->BackColorNormal;
				modeChoice->TitleText = "Choose RSA key generator mode:";
				modeChoice->TitleFont = gcnew System::Drawing::Font("Microsoft Sans Serif", 10.5f, FontStyle::Bold);
				modeChoice->TitleForeColor = ColorFactory::Scale(Color::LightSeaGreen, 0.725f);
				modeChoice->IsTitleVertical = true;
				modeChoice->IsListHorizontal = true;
				modeChoice->Offset = Point(0, 5);
				modeChoice->Spacing = Point(-2, 0);
				modeChoice->AutoSize = false;
				modeChoice[0]->activate();

				for(Int32 i = 0; i < modeChoice->Count; ++i) {
					modeChoice[i]->Font = modeChoice->TitleFont;
					modeChoice[i]->Size = System::Drawing::Size(155, 25);
				}

				modeChoice->performLayout();
				modeChoice->Location = Point(12, menuPanel->Bottom + 12);
			}

			void _initializeMaxAttemptsBoxes()
			{
				seqAttempts = gcnew TitledCheckBoxTitledList(this);
			
				seqAttempts->addCheckBox("attemptsUser", "User defined:", false);
				seqAttempts->addCheckBox("attemptsPnt", "Prime number theorem:", false);
				seqAttempts->addCheckBox("attemptsInf", "Until primes are found:", false);

				seqAttempts->TitleText = "Max amount of numbers tested for primality:";
				seqAttempts->TitleFont = gcnew System::Drawing::Font(modeChoice->TitleFont, FontStyle::Regular);
				seqAttempts->TitleForeColor = ColorFactory::Scale(Color::DeepSkyBlue, 0.6f);
				seqAttempts->IsTitleVertical = true;
				seqAttempts->IsListVertical = true;
				seqAttempts->Offset = Point(15, 5);
				seqAttempts->Spacing = Point(0, 0);
				seqAttempts->disableMultiSelection(0);
				seqAttempts->BorderColor = ColorFactory::Scale(Color::DeepSkyBlue, 0.85f);
			
				for(Int32 i = 0; i < seqAttempts->Count; ++i) {
					seqAttempts[i]->TitleFont = seqAttempts->TitleFont;
					seqAttempts[i]->TitleForeColor = seqAttempts->TitleForeColor;
					seqAttempts[i]->ControlForeColor = seqAttempts[0]->BorderColor;
					seqAttempts[i]->ControlBackColor = Color::Azure;
					seqAttempts[i]->ControlSize = System::Drawing::Size(12, 12);
					seqAttempts[i]->OffsetX = 35;
					seqAttempts[i]->OffsetY += seqAttempts[i]->MiddleAlignmentY + 2;
				}

				seqAttempts->performLayout();
				seqAttempts->performHorizontalAlignment();
				seqAttempts->Top = modeChoice->Bottom + 12;
			}

			void _initializeSeqChoiceBoxes()
			{
				seqGenChoice = gcnew TitledCheckBoxTitledList(this);
			
				seqGenChoice->addCheckBox("genSafe", "Generate safe primes:", false);
				seqGenChoice->addCheckBox("gen65537", "Use 65537 as public exponent:", false);

				seqGenChoice->TitleText = "Key generation settings:";
				seqGenChoice->TitleFont = seqAttempts->TitleFont;
				seqGenChoice->TitleForeColor = seqAttempts->TitleForeColor;
				seqGenChoice->IsTitleVertical = true;
				seqGenChoice->IsListVertical = true;
				seqGenChoice->Offset = Point(5, 5);
				seqGenChoice->Spacing = Point(0, 0);
				seqGenChoice->BorderColor = seqAttempts[0]->BorderColor;
			
				for(Int32 i = 0; i < seqGenChoice->Count; ++i) {
					seqGenChoice[i]->TitleFont = seqGenChoice->TitleFont;
					seqGenChoice[i]->TitleForeColor = seqGenChoice->TitleForeColor;
					seqGenChoice[i]->ControlForeColor = seqGenChoice[0]->BorderColor;
					seqGenChoice[i]->ControlBackColor = Color::Azure;
					seqGenChoice[i]->ControlSize = System::Drawing::Size(12, 12);
					seqGenChoice[i]->OffsetX = 10;
					seqGenChoice[i]->OffsetY += seqGenChoice[i]->MiddleAlignmentY + 2;
				}

				seqGenChoice->performLayout();
				seqGenChoice->performHorizontalAlignment();
				seqGenChoice->Location = Point(modeChoice->Left, seqAttempts->Top);
				seqAttempts->Left = seqGenChoice->Right + 20;
			}

			void _initializeDssChoiceBoxes()
			{
				dssGenChoice = gcnew TitledCheckBoxTitledList(this);
			
				dssGenChoice->addCheckBox("dssGenConditions", "Primes with conditions:", false);
				dssGenChoice->addCheckBox("dssGen65537", seqGenChoice[1]->TitleText, false);

				dssGenChoice->TitleText = seqGenChoice->TitleText;
				dssGenChoice->TitleFont = seqGenChoice->TitleFont;
				dssGenChoice->TitleForeColor = seqGenChoice->TitleForeColor;
				dssGenChoice->IsTitleVertical = true;
				dssGenChoice->IsListVertical = true;
				dssGenChoice->Offset = Point(5, 5);
				dssGenChoice->Spacing = Point(0, 0);
				dssGenChoice->BorderColor = seqAttempts[0]->BorderColor;
			
				for(Int32 i = 0; i < dssGenChoice->Count; ++i) {
					dssGenChoice[i]->TitleFont = dssGenChoice->TitleFont;
					dssGenChoice[i]->TitleForeColor = dssGenChoice->TitleForeColor;
					dssGenChoice[i]->ControlForeColor = dssGenChoice[0]->BorderColor;
					dssGenChoice[i]->ControlBackColor = Color::Azure;
					dssGenChoice[i]->ControlSize = System::Drawing::Size(12, 12);
					dssGenChoice[i]->OffsetX = 7;
					dssGenChoice[i]->OffsetY += dssGenChoice[i]->MiddleAlignmentY + 2;
				}

				dssGenChoice->performLayout();
				dssGenChoice->performHorizontalAlignment();
				dssGenChoice->Top = seqAttempts->Top;
				dssGenChoice->Visible = false;
			}

			void _initializeSeqLengthBoxes()
			{
				seqLengthBoxes = gcnew TitledTextBoxList(this);
			
				seqLengthBoxes->addTextBox("publExpLength", "Public exponent's (e) bit length:", "17");
				seqLengthBoxes->addTextBox("pLength", "First prime number's (p) bit length:", "512");
				seqLengthBoxes->addTextBox("qLength", "Second prime number's (q) bit length:", "512");

				seqLengthBoxes->IsListVertical = true;
				seqLengthBoxes->Spacing = Point(0, -1);
				seqLengthBoxes->ControlAutoSize = false;
				seqLengthBoxes->MaxLength = 4;
			
				for(Int32 i = 0; i < seqLengthBoxes->Count; ++i) {
					seqLengthBoxes[i]->TitleFont = seqAttempts->TitleFont;
					seqLengthBoxes[i]->ControlFont = seqAttempts->TitleFont;
					seqLengthBoxes[i]->TitleForeColor = seqAttempts->TitleForeColor;
					seqLengthBoxes[i]->ControlForeColor = seqAttempts->TitleForeColor;
					seqLengthBoxes[i]->ControlBackColor = Color::Azure;
					seqLengthBoxes[i]->Offset = Point(8, 0);
					seqLengthBoxes[i]->ControlSize = System::Drawing::Size(45, 21);
				}

				seqLengthBoxes->performLayout();
				seqLengthBoxes->Location = Point(modeChoice->Left, seqAttempts->Bottom + 15);
			}

			void _initializeDssLengthBoxes()
			{
				dssLengthBoxes = gcnew TitledTextBoxList(this);
			
				dssLengthBoxes->addTextBox("dssExpLength", seqLengthBoxes[0]->TitleText, "17");
				dssLengthBoxes->addTextBox("nLength", "Key's modulus' (n) bit length:", "1024");

				dssLengthBoxes->IsListVertical = true;
				dssLengthBoxes->Spacing = Point(0, -1);
				dssLengthBoxes->ControlAutoSize = false;
				dssLengthBoxes->MaxLength = 4;
			
				for(Int32 i = 0; i < dssLengthBoxes->Count; ++i) {
					dssLengthBoxes[i]->TitleFont = seqAttempts->TitleFont;
					dssLengthBoxes[i]->ControlFont = seqAttempts->TitleFont;
					dssLengthBoxes[i]->TitleForeColor = seqAttempts->TitleForeColor;
					dssLengthBoxes[i]->ControlForeColor = seqAttempts->TitleForeColor;
					dssLengthBoxes[i]->ControlBackColor = Color::Azure;
					dssLengthBoxes[i]->Offset = Point(8, 0);
					dssLengthBoxes[i]->ControlSize = System::Drawing::Size(45, 21);
				}

				dssLengthBoxes->performLayout();
				dssLengthBoxes->Location = Point(modeChoice->Left, seqAttempts->Top);
				dssLengthBoxes->Visible = false;
			}

			void _initializeMaxAttemptsInputBoxes()
			{
				maxAttempts = gcnew TitledTextBoxList(this);
			
				maxAttempts->addTextBox("pAttempts", L"→ max attempts:", "500");
				maxAttempts->addTextBox("qAttempts", maxAttempts[0]->TitleText, "500");

				maxAttempts->IsListVertical = true;
				maxAttempts->Spacing = Point(0, -1);
				maxAttempts->ControlAutoSize = false;
				maxAttempts->MaxLength = 10;
			
				for(Int32 i = 0; i < maxAttempts->Count; ++i) {
					maxAttempts[i]->TitleFont = seqAttempts->TitleFont;
					maxAttempts[i]->ControlFont = seqAttempts->TitleFont;
					maxAttempts[i]->TitleForeColor = seqAttempts->TitleForeColor;
					maxAttempts[i]->ControlForeColor = seqAttempts->TitleForeColor;
					maxAttempts[i]->ControlBackColor = Color::Azure;
					maxAttempts[i]->Offset = Point(15, 0);
					maxAttempts[i]->ControlSize = System::Drawing::Size(90, 21);
				}

				maxAttempts->performLayout();
				maxAttempts->performHorizontalAlignment();
				maxAttempts->Bottom = seqLengthBoxes->Bottom;
			}

			void _initializeRoundsBox()
			{
				rabinMillerSteps = gcnew TitledTextBox(this, "rabinMillerSteps", "Miller-Rabin primality test's rounds:", "20");

				rabinMillerSteps->ControlAutoSize = false;
				rabinMillerSteps->MaxLength = 2;
				rabinMillerSteps->TitleFont = seqAttempts->TitleFont;
				rabinMillerSteps->ControlFont = seqAttempts->TitleFont;
				rabinMillerSteps->TitleForeColor = seqAttempts->TitleForeColor;
				rabinMillerSteps->ControlForeColor = seqAttempts->TitleForeColor;
				rabinMillerSteps->ControlBackColor = Color::Azure;
				rabinMillerSteps->Offset = Point(8, 0);
				rabinMillerSteps->ControlSize = System::Drawing::Size(30, 21);

				rabinMillerSteps->Location = Point(modeChoice->Left, seqLengthBoxes->Bottom + 7);
			}

			void _initializeGenerateButtons()
			{
				generate = gcnew LabelButton(this, "generate", "GENERATE RSA KEY");

				LabelButtonScheme::Invert(generate, false, ColorFactory::Scale(Color::DeepSkyBlue, 0.85f), Color::Azure, 1.1f);
				generate->BorderColor = generate->BackColorNormal;
				generate->AutoSize = false;
				generate->Font = modeChoice->TitleFont;
				generate->Height = 25;
				generate->Location = Point(modeChoice->Left, rabinMillerSteps->Bottom + 10);

				chooseExp = gcnew LabelButton(this, "chooseExp", "CHOOSE DIFFERENT PUBLIC EXP");
				
				LabelButtonScheme::Invert(chooseExp, false, ColorFactory::Scale(Color::DeepSkyBlue, 0.85f), Color::Azure, 1.1f);
				chooseExp->BorderColor = chooseExp->BackColorNormal;
				chooseExp->AutoSize = false;
				chooseExp->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9, FontStyle::Bold);
				chooseExp->Height = generate->Height;
				chooseExp->Top = generate->Top;
				chooseExp->Visible = false;
			}

			void _initializeProgressBar()
			{
				informant = gcnew TitledLabel(this, "informant", "");
				informant->ControlAutoSize = true;
				informant->TitleFont = seqAttempts->TitleFont;
				informant->ControlFont = seqAttempts->TitleFont;
				informant->TitleForeColor = seqAttempts->TitleForeColor;
				informant->ControlForeColor = seqAttempts->TitleForeColor;
				informant->Offset = Point(3, 0);
				informant->Location = Point(modeChoice->Left, generate->Bottom + 5);

				progressBar = gcnew SingleProgressBar(this, "progressBar");
				progressBar->BorderThickness = 2;
				progressBar->Height = 20;
				progressBar->BorderColor = generate->BackColorNormal;
				progressBar->BackColor = Color::Azure;
				progressBar->ForeColor = generate->BackColorNormal;
				progressBar->Location = Point(modeChoice->Left, informant->Bottom + 5);

				informant->Visible = false;
				progressBar->Visible = false;
			}

			void _initializeKeyButtons()
			{
				keyButtons = gcnew LabelButtonList(this);

				keyButtons->addButton("bCheckSafety", "CHECK KEY'S SAFETY");
				keyButtons->addButton("bInvertKey", "INVERT EXP");
				keyButtons->addButton("bClearKey", "CLEAR");
				keyButtons->addButton("bShowHide", "SHOW MORE");
			
				LabelButtonScheme::Invert(keyButtons, false, ColorFactory::Scale(Color::DeepSkyBlue, 0.85f), Color::Azure, 1.1f);
				keyButtons->BorderColor = keyButtons[0]->BackColorNormal;
				keyButtons->IsListHorizontal = true;
				keyButtons->Spacing = Point(4, 0);
				keyButtons->AutoSize = false;

				for(Int32 i = 0; i < keyButtons->Count; ++i) {
					keyButtons[i]->Font = menuButtons[0]->Font;
					keyButtons[i]->Size = System::Drawing::Size(174, 21);
				}

				keyButtons->performLayout();
				keyButtons->Location = Point(modeChoice->Left, informant->Bottom + 15);
				keyButtons[3]->Location = Point(keyButtons[2]->Left, keyButtons->Bottom + 4);
				keyButtons->updateBounds();
				keyButtons->Visible = false;
			}

			void _initializeKeyBoxes()
			{
				keyNumbers = gcnew TitledTextBoxTitledList(this);

				keyNumbers->addTextBox("boxN", "Modulus:", "");
				keyNumbers->addTextBox("boxE", "Public exponent:", "");
				keyNumbers->addTextBox("boxP", "Prime 'p':", "");
				keyNumbers->addTextBox("boxQ", "Prime 'q':", "");
				keyNumbers->addTextBox("boxD", "Private exponent:", "");
				keyNumbers->addTextBox("boxDP", "Private CRT p-exponent:", "");
				keyNumbers->addTextBox("boxDQ", "Private CRT q-exponent:", "");
				keyNumbers->addTextBox("boxQINV", "Private CRT inverted q:", "");

				keyNumbers->TitleText = "Current RSA Key:";
				keyNumbers->TitleForeColor = ColorFactory::Scale(Color::DeepSkyBlue, 0.6f);
				keyNumbers->TitleFont = modeChoice->TitleFont;
				keyNumbers->IsListVertical = true;
				keyNumbers->IsTitleVertical = true;
				keyNumbers->Offset = Point(0, 10);
				keyNumbers->Spacing = Point(0, 5);
				keyNumbers->ReadOnly = true;
				keyNumbers->Multiline = true;
				keyNumbers->TextAlign = HorizontalAlignment::Left;
				keyNumbers->WordWrap = false;
				keyNumbers->ScrollBars = ScrollBars::Horizontal;
				keyNumbers->ControlAutoSize = false;

				for(Int32 i = 0; i < keyNumbers->Count; ++i) {
					keyNumbers[i]->IsTitleVertical = true;
					keyNumbers[i]->Offset = Point(0, 2);
					keyNumbers[i]->TitleFont = seqAttempts->TitleFont;
					keyNumbers[i]->ControlFont = seqAttempts->TitleFont;
					keyNumbers[i]->TitleForeColor = keyNumbers->TitleForeColor;
					keyNumbers[i]->ControlForeColor = keyNumbers->TitleForeColor;
					keyNumbers[i]->ControlBackColor = Color::Azure;
					keyNumbers[i]->ControlHeight = 38;
					keyNumbers[i]->ControlWidth = seqAttempts->Right - 14;
				}

				keyNumbers->performLayout();
				keyNumbers->performHorizontalAlignment();
				keyNumbers->Location = Point(modeChoice->Left, keyButtons[0]->Bottom + 5);
				keyNumbers->Visible = false;
			}

			void _initializeLengthBoxesAlignment()
			{
				DynamicControlAligner^ aligner = gcnew DynamicControlAligner();
				aligner->addControl(dssLengthBoxes);
				aligner->addControl(rabinMillerSteps);
				aligner->performHorizontalAlignment();
				aligner->clear();

				aligner->addControl(seqLengthBoxes);
				aligner->addControl(rabinMillerSteps);
				aligner->performHorizontalAlignment();
				aligner->clear();
			}

			void _initializeMovableControls()
			{
				movableControls = gcnew DynamicControlCollection();
				movableControls->add(rabinMillerSteps);
				movableControls->add(generate);
				movableControls->add(chooseExp);
				movableControls->add(informant);
				movableControls->add(progressBar);
				movableControls->add(keyButtons);
				movableControls->add(keyNumbers);
				movableControls->updateBounds();
			}

			void _initializeEvents()
			{
				menuButtons[0]->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_SaveKey));
				menuButtons[1]->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_LoadKey));
				menuButtons[2]->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_ExportKey));

				modeChoice[0]->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_SeqMode));
				modeChoice[1]->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_DssMode));

				seqAttempts[0]->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_AmountUserDef));
				seqAttempts[1]->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_AmountPntHLc));
				seqAttempts[2]->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_AmountUntilFound));

				seqGenChoice[0]->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_GenSafe));
				seqGenChoice[1]->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_Gen65537));

				dssGenChoice[1]->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_Gen65537Dss));

				generate->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_GenKey));
				chooseExp->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_ChooseExp));

				keyButtons[0]->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_CheckKey));
				keyButtons[1]->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_InvertKey));
				keyButtons[2]->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_ClearKey));
				keyButtons[3]->addMouseDownEvent(gcnew MouseEventHandler(this, &KeyGenForm::Event_ShowHide));
			}
#pragma endregion
	};
}