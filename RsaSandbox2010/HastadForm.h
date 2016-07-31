#pragma once

#include "Toolbox.h"
#include "KeyGenForm.h"
#include "HastadExponentForm.h"
#include "RSACipher.h"
#include "EncryptMessageForm.h"
#include "HastadAttack.h"

namespace RsaSandbox
{
	using namespace System;
	using namespace System::Text;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;
	using namespace System::Diagnostics;

	using namespace CVNetControls;
	using namespace RsaSandbox::Ciphering;
	using namespace RsaSandbox::Attacks;

	public ref class HastadForm : public SecondaryForm
	{
		public:
			HastadForm(LabelButton^ controlButton, KeyGenForm^ keyGenForm) : SecondaryForm(controlButton)
			{
				this->Text = L"RSA Sandbox : Håstad's Attack";
				this->Icon = gcnew System::Drawing::Icon(".\\cvrsa.ico");
				_initializeUI();
				
				keys = gcnew List<RSAKey^>();
				ciphers = gcnew List<array<Byte>^>();
				keyGen = keyGenForm;

				keyGenerator = gcnew RSAKeyGenerator();
				keyGenerator->Outcome += gcnew KeyGenOutcomeHandler(this, &HastadForm::Event_KeyGenOutcome);
				keyCount = desiredKeyCount = bitCount = 0;

				cipher = gcnew RSACipher();
				cipher->PaddingMode = RSACipherPaddingMode::None;
				cipher->Outcome += gcnew RSACipherOutcomeHandler(this, &HastadForm::Event_CipherOutcome);
				message = nullptr;

				hastad = gcnew HastadAttack();
				hastad->ReportProgress += gcnew TaskReportProgressHandler(this, &HastadForm::Event_AttackReportProgress);
				hastad->Outcome += gcnew HastadAttackOutcomeHandler(this, &HastadForm::Event_AttackOutcome);
			}

		protected:
			virtual void OnResizeEnd(EventArgs^ e) override
			{
				Form::OnResizeEnd(e);
				if(!performAttack->Visible) {
					if(clearButton->Enabled && encrypt[0]->Visible && ClientSize.Height != encrypt->Bottom + 12)
						ClientSize = System::Drawing::Size(ClientSize.Width, encrypt->Bottom + 12);
				}
				else if(!attackInformant->Visible && ClientSize.Height != performAttack->Bottom + 12)
					ClientSize = System::Drawing::Size(ClientSize.Width, performAttack->Bottom + 12);
			}

		private:
			List<RSAKey^>^ keys;
			List<array<Byte>^>^ ciphers;
			array<Byte>^ message;
			KeyGenForm^ keyGen;
			RSAKeyGenerator^ keyGenerator;
			RSACipher^ cipher;
			HastadAttack^ hastad;
			Int32 keyCount, desiredKeyCount, bitCount;

#pragma region UI Fields
			Panel^ menuPanel;
			LabelButtonList^ menuButtons;

			TitledTextBoxTitledList^ keyNumbers;
			TitledTextBox^ boxKeyId;
			LabelButton^ confirmId;
			LabelButton^ clearButton;

			TitledLabel^ keyGenInformant;
			SingleProgressBar^ keyGenBar;
			LabelButton^ keyGenCancel;

			LabelButtonList^ encrypt;
			TitledLabel^ cipherInformant;
			SingleProgressBar^ cipherBar;
			LabelButton^ cipherCancel;

			LabelButton^ performAttack;
			TitledLabel^ attackInformant;
			SingleProgressBar^ attackBar;

			LabelButton^ saveOutcome;

			System::ComponentModel::Container ^components;
#pragma endregion

			void generateAdditionalKeys(Int32 count, Int32 modBitCount)
			{
				keyCount = (keys->Count == 1) ? 2 : 1;
				desiredKeyCount = (keys->Count == 1) ? count + 1 : count;
				bitCount = modBitCount;

				prepareAndLockKeyGenUI();

				keyGenerator->dss(10, modBitCount, 20);
			}

			void encryptData()
			{
				ciphers->Clear();
				ciphers->TrimExcess();

				keyCount = 0;
				desiredKeyCount = (Int32)keys[0]->PublicExponent;

				prepareAndLockEncryptUI();

				cipher->Key = keys[0];
				cipher->encrypt(message);
			}

			void prepareAndAttack()
			{
				prepareAndLockAttackUI();
				hastad->performAttack(keys, ciphers);
			}

			bool isKeyPairwiseCoprime(RSAKey^ key)
			{
				for(Int32 i = 0; i < keys->Count; ++i)
					if(BigInteger::GreatestCommonDivisor(key->Modulus, keys[i]->Modulus) != BigInteger::One)
						return false;
				return true;
			}

			void Event_KeyGenOutcome(RSAKeyGenerator^ sender, RSAKey^ key, KeyGenOutcome outcome)
			{
				if(outcome != KeyGenOutcome::Cancelled) {
					if(outcome == KeyGenOutcome::OK && key->trySetExponents(BigInteger(desiredKeyCount))) {
						if(isKeyPairwiseCoprime(key)) {
							keys->Add(key);
							if(++keyCount <= desiredKeyCount) {
								updateProgress(keyGenInformant, keyGenBar);
								keyGenerator->dss(10, bitCount, 20);
							}
							else prepareAndUnlockDisplayUI();
						}
						else keyGenerator->dss(10, bitCount, 20);
					}
					else keyGenerator->dss(10, bitCount, 20);
				}
				else hideUI();
			}

			void Event_CipherOutcome(RSACipher^ sender, array<Byte>^ output, RSACipherOutcome outcome)
			{
				if(outcome == RSACipherOutcome::OK) {
					ciphers->Add(output);
					if(++keyCount < desiredKeyCount) {
						updateProgress(cipherInformant, cipherBar);
						cipher->Key = keys[keyCount];
						cipher->encrypt(message);
					}
					else prepareAndUnlockAttackUI();
				}
				else prepareAndUnlockCancelEncryptUI();
			}

			void Event_AttackOutcome(HastadAttack^ sender, array<Byte>^ message, bool cancelled)
			{
				if(!cancelled) {
					this->message = HuffmanCompression::decompress(message);
					prepareAndUnlockDecryptedUI();
				}
				else prepareAndUnlockCancelAttackUI();
			}

			void Event_AttackReportProgress(BackgroundTask^ sender)
			{
				float progress = sender->CurrentStep / (float)sender->StepCount;
				attackInformant->ControlText = sender->CurrentStep.ToString() + " / " + sender->StepCount.ToString() + " (" + ((Int32)(progress * 100)).ToString() + "%)";
				attackInformant->toLabel()->Refresh();
				attackBar->ProgressPercentage = progress;
			}

#pragma region UI Events
			bool saveOutcomeToFile(String^ fileName)
			{
				if(AppIO::saveFile(fileName, message)) {
					if(message != nullptr) Array::Clear(message, 0, message->Length);
					message = nullptr;
					ciphers->Clear();
					ciphers->TrimExcess();

					SuspendDraw;
					saveOutcome->Visible = false;
					performAttack->Visible = false;
					attackInformant->Visible = false;
					ClientSize = System::Drawing::Size(keyNumbers->Right + 12, encrypt->Bottom + 12);
					ResumeDraw;
					return true;
				}
				return false;
			}

			Int32 loadRsaKeyFromFile(String^ fileName)
			{
				RSAKey^ key;
				if(AppIO::loadRsaKey(fileName, key)) {
					if(key->PublicExponent > BigInteger(HastadExponentForm::MaxPublicExponent)) return 2;
					keys->Clear();
					keys->TrimExcess();
					keys->Add(key);
					return 0;
				}
				return 1;
			}

			Int32 loadRsaKeyFromGenerator()
			{
				if(keyGen->CurrentKey != nullptr) {
					if(keyGen->CurrentKey->PublicExponent > BigInteger(HastadExponentForm::MaxPublicExponent)) return 2;
					keys->Clear();
					keys->TrimExcess();
					keys->Add(gcnew RSAKey(keyGen->CurrentKey));
					return 0;
				}
				return 1;
			}

			void updateProgress(TitledLabel^ informant, SingleProgressBar^ bar)
			{
				float progress = keyCount / (float)desiredKeyCount;
				informant->ControlText = keyCount.ToString() + " / " + desiredKeyCount.ToString() + " (" + ((Int32)(progress * 100)).ToString() + "%)";
				informant->toLabel()->Refresh();
				bar->ProgressPercentage = progress;
			}

			void prepareAndLockKeyGenUI()
			{
				SuspendDraw;
				keyNumbers->Visible = false;
				boxKeyId->Visible = false;
				confirmId->Visible = false;
				clearButton->Visible = false;
				encrypt->Visible = false;
				performAttack->Visible = false;
				attackInformant->Visible = false;
				saveOutcome->Visible = false;
				
				float progress = keyCount / (float)desiredKeyCount;
				Toolbox::setTitledControl(keyGenInformant, "Generating key:", keyCount.ToString() + " / " + desiredKeyCount.ToString() + " (" + ((Int32)(progress * 100)).ToString() + "%)");
				keyGenBar->ProgressPercentage = progress;
				keyGenInformant->Visible = true;
				keyGenBar->Visible = true;
				keyGenCancel->Visible = true;
				
				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, keyGenBar->Bottom + 12);
				ResumeDraw;
			}

			void prepareAndUnlockDisplayUI()
			{
				SuspendDraw;
				keyGenInformant->Visible = false;
				keyGenBar->Visible = false;
				keyGenCancel->Visible = false;
				keyNumbers->Title->Visible = true;

				displayKey(0);
				keyNumbers[0]->Visible = true;
				keyNumbers[1]->Visible = true;

				clearButton->Visible = true;
				boxKeyId->Visible = true;
				confirmId->Visible = true;
				encrypt->Visible = true;
				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, encrypt->Bottom + 12);
				ResumeDraw;
			}

			void prepareAndLockEncryptUI()
			{
				SuspendDraw;
				encrypt->Enabled = false;
				clearButton->Enabled = false;
				confirmId->Enabled = false;
				performAttack->Visible = false;
				attackInformant->Visible = false;
				saveOutcome->Visible = false;

				float progress = keyCount / (float)desiredKeyCount;
				Toolbox::setTitledControl(cipherInformant, "Encrypting:", keyCount.ToString() + " / " + desiredKeyCount.ToString() + " (" + ((Int32)(progress * 100)).ToString() + "%)");
				cipherBar->ProgressPercentage = progress;
				cipherInformant->Visible = true;
				cipherBar->Visible = true;
				cipherCancel->Visible = true;

				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, cipherBar->Bottom + 12);
				ResumeDraw;
			}

			void prepareAndUnlockCancelEncryptUI()
			{
				SuspendDraw;
				message = nullptr;
				cipherInformant->Visible = false;
				cipherBar->Visible = false;
				cipherCancel->Visible = false;

				encrypt->Enabled = true;
				clearButton->Enabled = true;
				confirmId->Enabled = true;

				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, encrypt->Bottom + 12);
				ResumeDraw;
			}

			void prepareAndUnlockAttackUI()
			{
				SuspendDraw;
				message = nullptr;
				cipherInformant->Visible = false;
				cipherBar->Visible = false;
				cipherCancel->Visible = false;

				encrypt->Enabled = true;
				clearButton->Enabled = true;
				confirmId->Enabled = true;
				performAttack->Visible = true;

				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, performAttack->Bottom + 12);
				ResumeDraw;
			}

			void prepareAndLockAttackUI()
			{
				SuspendDraw;
				encrypt->Enabled = false;
				clearButton->Enabled = false;
				confirmId->Enabled = false;
				saveOutcome->Visible = false;

				performAttack->Text = "CANCEL";
				Toolbox::setTitledControl(attackInformant, L"Performing Håstad's attack:", "0%");
				keyGenBar->ProgressPercentage = 0;
				attackInformant->Visible = true;
				attackBar->Visible = true;
				
				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, attackBar->Bottom + 12);
				ResumeDraw;
			}

			void prepareAndUnlockCancelAttackUI()
			{
				SuspendDraw;
				performAttack->Text = "PERFORM ATTACK";
				attackInformant->Visible = false;
				attackBar->Visible = false;

				encrypt->Enabled = true;
				clearButton->Enabled = true;
				confirmId->Enabled = true;

				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, performAttack->Bottom + 12);
				ResumeDraw;
			}

			void prepareAndUnlockDecryptedUI()
			{
				SuspendDraw;
				performAttack->Text = "PERFORM ATTACK";
				Toolbox::setTitledControl(attackInformant, "Time taken:", hastad->TimeElapsedMs.ToString("N0") + " ms");
				attackBar->Visible = false;

				encrypt->Enabled = true;
				clearButton->Enabled = true;
				confirmId->Enabled = true;

				saveOutcome->Visible = true;
				ResumeDraw;
			}

			void displayKey(Int32 i)
			{
				keyNumbers->TitleText = "RSA Key no. [" + (i + 1).ToString() + "]:";
				keyNumbers[0]->TitleText = "Modulus (" + keys[i]->BitLength.ToString() + "-bit long):";
				keyNumbers[0]->ControlText = keys[i]->Modulus.ToString("N0");
				keyNumbers[1]->TitleText = "Public Exponent (" + RSAInt::bitCount(keys[i]->PublicExponent).ToString() + "-bit long):";
				keyNumbers[1]->ControlText = keys[i]->PublicExponent.ToString("N0");
			}

			void hideUI()
			{
				SuspendDraw;
				keyGenInformant->Visible = false;
				keyGenBar->Visible = false;
				keyGenCancel->Visible = false;
				keyNumbers->Title->Visible = true;

				keys->Clear();
				keys->TrimExcess();
				ciphers->Clear();
				ciphers->TrimExcess();
				message = nullptr;

				keyNumbers->TitleText = "Load the template key from a file or import it from the Key Generator" + Environment::NewLine + "or set the public exponent explicitly.";
				for(Int32 i = 0; i < keyNumbers->Count; ++i) {
					keyNumbers[i]->TitleText = "";
					keyNumbers[i]->ControlText = "";
					keyNumbers[i]->Visible = false;
				}

				clearButton->Visible = false;
				boxKeyId->Visible = false;
				confirmId->Visible = false;
				encrypt->Visible = false;
				performAttack->Visible = false;
				attackInformant->Visible = false;
				saveOutcome->Visible = false;
				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, keyNumbers->TitleBottom + 12);
				ResumeDraw;
			}

			bool prepareToEncryptFile(String^ fileName)
			{
				array<Byte>^ data;
				if(AppIO::openFile(fileName, data)) {
					message = HuffmanCompression::compress(data);
					encryptData();
					return true;
				}
				return false;
			}

			void prepareAndEncryptMsg(String^ msg)
			{
				message = HuffmanCompression::compress(Encoding::UTF8->GetBytes(msg));
				encryptData();
			}
#pragma endregion

#pragma region User Events
			void Event_LoadKey(Object^ sender, MouseEventArgs^ e)
			{
				if(!keyGenerator->IsBusy && !cipher->IsBusy && !hastad->IsBusy) {
					OpenFileDialog^ dialog = gcnew OpenFileDialog();
					dialog->Filter = "RSA Key |*.rsakey";
					dialog->ShowDialog();

					if(!String::IsNullOrEmpty(dialog->FileName)) {
						Int32 result = loadRsaKeyFromFile(dialog->FileName);
						if(result == 0) {
							Int32 additionalCount = (Int32)keys[0]->PublicExponent - 1;
							array<String^>^ msg = gcnew array<String^>(3);
							msg[0] = "The RSA Key has been successfully loaded! The Key's public exponent equals " + keys[0]->PublicExponent.ToString("N0") + ".";
							msg[1] = "In order to perform Hastad's attack, the attack simulator will have to generate additional " + additionalCount.ToString("N0") + " keys.";
							msg[2] = "Do you want to continue?";

							System::Windows::Forms::DialogResult result = SimpleBox::displayWithResult(msg, "Load RSA Key from file");
							if(result == System::Windows::Forms::DialogResult::OK) generateAdditionalKeys(additionalCount, keys[0]->BitLength);
							else keys->Clear();
						}
						else if(result == 1) SimpleBox::display("RSA Key loading unsuccessful.", "File IO Error");
						else SimpleBox::display("Loaded Key's public exponent must be less than or equal to " + HastadExponentForm::MaxPublicExponent.ToString("N0") + ".", "Load Error");
					}
				}
				else SimpleBox::display("Can not load a new RSA Key while performing an action.", "File IO Error");
			}

			void Event_ImportKey(Object^ sender, MouseEventArgs^ e)
			{
				if(!keyGenerator->IsBusy && !cipher->IsBusy && !hastad->IsBusy) {
					Int32 result = loadRsaKeyFromGenerator();
					if(result == 0) {
						Int32 additionalCount = (Int32)keys[0]->PublicExponent - 1;
						array<String^>^ msg = gcnew array<String^>(3);
						msg[0] = "The RSA Key has been successfully imported! The Key's public exponent equals " + keys[0]->PublicExponent.ToString("N0") + ".";
						msg[1] = "In order to perform Hastad's attack, the attack simulator will have to generate additional " + additionalCount.ToString("N0") + " keys.";
						msg[2] = "Do you want to continue?";

						System::Windows::Forms::DialogResult result = SimpleBox::displayWithResult(msg, "Import RSA Key from Generator");
						if(result == System::Windows::Forms::DialogResult::OK) generateAdditionalKeys(additionalCount, keys[0]->BitLength);
						else keys->Clear();
					}
					else if(result == 1) SimpleBox::display("There is no RSA Key to import from the key generator.", "Import Error");
					else SimpleBox::display("Imported Key's public exponent must be less than or equal to " + HastadExponentForm::MaxPublicExponent.ToString("N0") + ".", "Import Error");
				}
				else SimpleBox::display("Can not load a new RSA Key while performing an action.", "Import Error");
			}

			void Event_SetExponent(Object^ sender, MouseEventArgs^ e)
			{
				if(!keyGenerator->IsBusy && !cipher->IsBusy && !hastad->IsBusy) {
					HastadExponentForm^ form = gcnew HastadExponentForm();
					System::Windows::Forms::DialogResult result = form->ShowDialog();
					if(result == System::Windows::Forms::DialogResult::OK) {
						keys->Clear();
						keys->TrimExcess();
						generateAdditionalKeys(form->PublicExponent, form->ModulusBitLength);
					}
				}
				else SimpleBox::display("Can not set a new Public Exponent while performing an action.", "Set Exponent Error");
			}

			void Event_CancelKeyGen(Object^ sender, MouseEventArgs^ e)
			{
				keyGenerator->requestCancel();
			}

			void Event_ConfirmId(Object^ sender, MouseEventArgs^ e)
			{
				Int32 id;
				if(Int32::TryParse(boxKeyId->ControlText, id) && id > 0 && id <= keys->Count)
					displayKey(id - 1);
				else SimpleBox::display("Key's no. must be a natural number greater than 0\r\n" +
										"and less than or equal to the shared public exponent (" + keys->Count.ToString("N0") + ").", "Parsing Error");
			}

			void Event_ClearKeys(Object^ sender, MouseEventArgs^ e)
			{
				System::Windows::Forms::DialogResult result = SimpleBox::displayWithResult("Are you sure you want to clear all keys?", "Clear");
				if(result == System::Windows::Forms::DialogResult::OK) hideUI();
			}

			void Event_EncryptFile(Object^ sender, MouseEventArgs^ e)
			{
				OpenFileDialog^ dialog = gcnew OpenFileDialog();
				dialog->ShowDialog();

				if(!String::IsNullOrEmpty(dialog->FileName))
					if(!prepareToEncryptFile(dialog->FileName))
						SimpleBox::display("Could not open the file.", "File IO Error");
			}

			void Event_EncryptMsg(Object^ sender, MouseEventArgs^ e)
			{
				EncryptMessageForm^ form = gcnew EncryptMessageForm();
				form->ShowDialog();

				if(form->DialogResult == System::Windows::Forms::DialogResult::OK) {
					if(!String::IsNullOrEmpty(form->Message)) prepareAndEncryptMsg(form->Message);
					else SimpleBox::display("Message can not be empty.", "Encryption Error");
				}
			}

			void Event_CancelCipher(Object^ sender, MouseEventArgs^ e)
			{
				cipher->requestCancel();
			}

			void Event_PerformAttack(Object^ sender, MouseEventArgs^ e)
			{
				if(!hastad->IsBusy) prepareAndAttack();
				else hastad->requestCancel();
			}

			void Event_SaveOutcome(Object^ sender, MouseEventArgs^ e)
			{
				SaveFileDialog^ dialog = gcnew SaveFileDialog();
				dialog->ShowDialog();

				if(!String::IsNullOrEmpty(dialog->FileName)) {
					if(saveOutcomeToFile(dialog->FileName))
						SimpleBox::display("The decrypted data has been successfully saved!", "Save data to file");
					else SimpleBox::display("File saving unsuccessful.", "File IO Error");
				}
			}
#pragma endregion
			
#pragma region Initialization
			void _initializeUI()
			{
				BackColor = SimpleBox::BackColor;
				FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
				MinimizeBox = true;
				DoubleBuffered = true;

				_initializeMenuButtons();
				_initializeKeyDisplay();
				_initializeClearButton();
				_initializeIdBoxAndButton();
				_initializeKeyGenInfo();
				_initializeEncryptButtons();
				_initializeCipherInfo();
				_initializeAttackButton();
				_initializeAttackInfo();
				_initializeSaveOutcomeButton();
				_initializeEvents();

				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, keyNumbers->TitleBottom + 12);
				menuPanel->Width = ClientSize.Width + 2;
				keyGenBar->Width = ClientSize.Width - 24;
				keyGenCancel->Right = keyGenBar->Right;
				encrypt->CentreX = ClientSize.Width >> 1;
				cipherBar->Width = keyGenBar->Width;
				cipherCancel->Right = cipherBar->Right;
				performAttack->CentreX = encrypt->CentreX;
				attackBar->Width = keyGenBar->Width;
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

				menuButtons->addButton("menuLoad", "LOAD TEMPLATE KEY FROM FILE");
				menuButtons->addButton("menuImport", "IMPORT TEMPLATE KEY FROM GENERATOR");
				menuButtons->addButton("menuSetExp", "SET PUBLIC EXPONENT");

				LabelButtonScheme::Invert(menuButtons, false, menuPanel->BackColor, Color::WhiteSmoke, 1.1f);
				menuButtons->AutoSize = true;
				menuButtons->BorderStyle = gcnew RectangularBorder();

				menuButtons[0]->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9, FontStyle::Bold);
				for(Int32 i = 1; i < menuButtons->Count; ++i)
					menuButtons[i]->Font = menuButtons[0]->Font;

				menuButtons->performLayout();
			}

			void _initializeKeyDisplay()
			{
				keyNumbers = gcnew TitledTextBoxTitledList(this);

				keyNumbers->addTextBox("boxN", "Modulus:", "");
				keyNumbers->addTextBox("boxE", "Public exponent:", "");

				keyNumbers->TitleText = "Load the template key from a file or import it from the Key Generator";
				keyNumbers->TitleForeColor = ColorFactory::Scale(Color::DeepSkyBlue, 0.6f);
				keyNumbers->TitleFont = gcnew System::Drawing::Font("Microsoft Sans Serif", 10.5f, FontStyle::Bold);
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

				System::Drawing::Font^ font = gcnew System::Drawing::Font(keyNumbers->TitleFont, FontStyle::Regular);
				for(Int32 i = 0; i < keyNumbers->Count; ++i) {
					keyNumbers[i]->IsTitleVertical = true;
					keyNumbers[i]->Offset = Point(0, 2);
					keyNumbers[i]->TitleFont = font;
					keyNumbers[i]->ControlFont = font;
					keyNumbers[i]->TitleForeColor = keyNumbers->TitleForeColor;
					keyNumbers[i]->ControlForeColor = keyNumbers->TitleForeColor;
					keyNumbers[i]->ControlBackColor = Color::Azure;
					keyNumbers[i]->ControlHeight = 38;
					keyNumbers[i]->ControlWidth = menuButtons->Width + 12;
					keyNumbers[i]->Visible = false;
				}

				keyNumbers->performLayout();
				keyNumbers->performHorizontalAlignment();
				keyNumbers->TitleText += Environment::NewLine + "or set the public exponent explicitly.";
				keyNumbers->Location = Point(12, menuPanel->Bottom + 12);
			}

			void _initializeClearButton()
			{
				clearButton = gcnew LabelButton(this, "bClearKeys", "CLEAR ALL");
			
				LabelButtonScheme::Invert(clearButton, false, ColorFactory::Scale(Color::DeepSkyBlue, 0.85f), Color::Azure, 1.1f);
				clearButton->BorderColor = clearButton->BackColorNormal;
				clearButton->AutoSize = false;
				clearButton->Font = menuButtons[0]->Font;
				clearButton->Size = System::Drawing::Size(165, 21);

				clearButton->Right = keyNumbers->Right;
				clearButton->Top = keyNumbers->Top;
				clearButton->Visible = false;
			}

			void _initializeIdBoxAndButton()
			{
				confirmId = gcnew LabelButton(this, "bConfirmId", "CONFIRM");
			
				LabelButtonScheme::Invert(confirmId, false, ColorFactory::Scale(Color::DeepSkyBlue, 0.7f), Color::Azure, 1.15f);
				confirmId->BorderColor = confirmId->BackColorNormal;
				confirmId->AutoSize = false;
				confirmId->Font = menuButtons[0]->Font;
				confirmId->Size = System::Drawing::Size(100, 21);

				confirmId->Right = clearButton->Left - 15;
				confirmId->Top = keyNumbers->Top;
				confirmId->Visible = false;

				boxKeyId = gcnew TitledTextBox(this, "boxKeyId", "DISPLAY KEY NO.", "1");

				boxKeyId->ControlAutoSize = false;
				boxKeyId->MaxLength = 5;
				boxKeyId->TitleFont = gcnew System::Drawing::Font("Microsoft Sans Serif", 9);
				boxKeyId->ControlFont = boxKeyId->TitleFont;
				boxKeyId->TitleForeColor = keyNumbers->TitleForeColor;
				boxKeyId->ControlForeColor = keyNumbers->TitleForeColor;
				boxKeyId->ControlBackColor = Color::Azure;
				boxKeyId->Offset = Point(1, -1);
				boxKeyId->ControlSize = System::Drawing::Size(45, 19);

				boxKeyId->Right = confirmId->Left - 1;
				boxKeyId->CentreY = confirmId->CentreY;
				boxKeyId->Visible = false;
			}

			void _initializeKeyGenInfo()
			{
				keyGenInformant = gcnew TitledLabel(this, "keyGenInfo", "informant", "");
				keyGenInformant->ControlAutoSize = true;
				keyGenInformant->TitleFont = gcnew System::Drawing::Font("Microsoft Sans Serif", 10.5f);
				keyGenInformant->ControlFont = keyGenInformant->TitleFont;
				keyGenInformant->TitleForeColor = keyNumbers->TitleForeColor;
				keyGenInformant->ControlForeColor = keyNumbers->TitleForeColor;
				keyGenInformant->Offset = Point(3, 0);
				keyGenInformant->Location = Point(keyNumbers->Left, keyNumbers->Top);
				keyGenInformant->Visible = false;

				keyGenBar = gcnew SingleProgressBar(this, "keyGenBar");
				keyGenBar->BorderThickness = 2;
				keyGenBar->Height = 21;
				keyGenBar->BorderColor = clearButton->BackColorNormal;
				keyGenBar->BackColor = Color::Azure;
				keyGenBar->ForeColor = clearButton->BackColorNormal;
				keyGenBar->Top = keyGenInformant->Bottom + 5;
				keyGenBar->Left = keyGenInformant->Left;
				keyGenBar->Visible = false;

				keyGenCancel = gcnew LabelButton(this, "keyGenCancel", "CANCEL");
				LabelButtonScheme::Invert(keyGenCancel, false, ColorFactory::Scale(Color::DeepSkyBlue, 0.85f), Color::Azure, 1.1f);
				keyGenCancel->AutoSize = false;
				keyGenCancel->BorderColor = keyGenCancel->BackColorNormal;
				keyGenCancel->Font = menuButtons[0]->Font;
				keyGenCancel->Size = System::Drawing::Size(120, 21);
				keyGenCancel->Bottom = keyGenBar->Top + 1;
				keyGenCancel->Visible = false;
			}

			void _initializeEncryptButtons()
			{
				encrypt = gcnew LabelButtonList(this);

				encrypt->addButton("encryptFile", "ENCRYPT FILE");
				encrypt->addButton("encryptMsg", "ENCRYPT MESSAGE");
			
				LabelButtonScheme::Invert(encrypt, false, ColorFactory::Scale(Color::LightGreen, 0.85f), Color::Honeydew, 1.1f);
				encrypt->BorderColor = encrypt[0]->BackColorNormal;
				encrypt->IsListHorizontal = true;
				encrypt->Spacing = Point(5, 0);
				encrypt->AutoSize = false;

				for(Int32 i = 0; i < encrypt->Count; ++i) {
					encrypt[i]->Font = keyNumbers->TitleFont;
					encrypt[i]->Size = System::Drawing::Size(180, 25);
				}

				encrypt->performLayout();
				encrypt->Top = keyNumbers->Bottom + 15;
				encrypt->Visible = false;
			}

			void _initializeCipherInfo()
			{
				cipherInformant = gcnew TitledLabel(this, "cipherInfo", "informant", "");
				cipherInformant->ControlAutoSize = true;
				cipherInformant->TitleFont = keyGenInformant->TitleFont;
				cipherInformant->ControlFont = cipherInformant->TitleFont;
				cipherInformant->TitleForeColor = keyNumbers->TitleForeColor;
				cipherInformant->ControlForeColor = keyNumbers->TitleForeColor;
				cipherInformant->Offset = Point(3, 0);
				cipherInformant->Location = Point(keyNumbers->Left, encrypt->Bottom + 5);
				cipherInformant->Visible = false;

				cipherBar = gcnew SingleProgressBar(this, "cipherBar");
				cipherBar->BorderThickness = 2;
				cipherBar->Height = 21;
				cipherBar->BorderColor = clearButton->BackColorNormal;
				cipherBar->BackColor = Color::Azure;
				cipherBar->ForeColor = clearButton->BackColorNormal;
				cipherBar->Top = cipherInformant->Bottom + 5;
				cipherBar->Left = cipherInformant->Left;
				cipherBar->Visible = false;

				cipherCancel = gcnew LabelButton(this, "cipherCancel", "CANCEL");
				LabelButtonScheme::Invert(cipherCancel, false, ColorFactory::Scale(Color::DeepSkyBlue, 0.85f), Color::Azure, 1.1f);
				cipherCancel->AutoSize = false;
				cipherCancel->BorderColor = cipherCancel->BackColorNormal;
				cipherCancel->Font = menuButtons[0]->Font;
				cipherCancel->Size = System::Drawing::Size(120, 21);
				cipherCancel->Bottom = cipherBar->Top + 1;
				cipherCancel->Visible = false;
			}

			void _initializeAttackButton()
			{
				performAttack = gcnew LabelButton(this, "bAttack", "PERFORM ATTACK");
			
				LabelButtonScheme::Invert(performAttack, false, ColorFactory::Scale(Color::LightCoral, 0.85f), Color::WhiteSmoke, 1.1f);
				performAttack->BorderColor = performAttack->BackColorNormal;
				performAttack->AutoSize = false;
				performAttack->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 10.5f, FontStyle::Bold);
				performAttack->Size = System::Drawing::Size(250, 25);

				performAttack->Top = encrypt->Bottom + 15;
				performAttack->Visible = false;
			}

			void _initializeAttackInfo()
			{
				attackInformant = gcnew TitledLabel(this, "attackInfo", "informant", "");
				attackInformant->ControlAutoSize = true;
				attackInformant->TitleFont = keyGenInformant->TitleFont;
				attackInformant->ControlFont = cipherInformant->TitleFont;
				attackInformant->TitleForeColor = keyNumbers->TitleForeColor;
				attackInformant->ControlForeColor = keyNumbers->TitleForeColor;
				attackInformant->Offset = Point(3, 0);
				attackInformant->Location = Point(keyNumbers->Left, performAttack->Bottom + 5);
				attackInformant->Visible = false;

				attackBar = gcnew SingleProgressBar(this, "attackBar");
				attackBar->BorderThickness = 2;
				attackBar->Height = 21;
				attackBar->BorderColor = clearButton->BackColorNormal;
				attackBar->BackColor = Color::Azure;
				attackBar->ForeColor = clearButton->BackColorNormal;
				attackBar->Top = attackInformant->Bottom + 5;
				attackBar->Left = attackInformant->Left;
				attackBar->Visible = false;
			}

			void _initializeSaveOutcomeButton()
			{
				saveOutcome = gcnew LabelButton(this, "saveOutcome", "SAVE DECRYPTED DATA TO FILE");
				LabelButtonScheme::Invert(saveOutcome, false, ColorFactory::Scale(Color::DeepSkyBlue, 0.85f), Color::Azure, 1.1f);
				saveOutcome->AutoSize = false;
				saveOutcome->BorderColor = saveOutcome->BackColorNormal;
				saveOutcome->Font = menuButtons[0]->Font;
				saveOutcome->Size = System::Drawing::Size(270, 21);
				saveOutcome->Left = keyNumbers->Left;
				saveOutcome->Top = attackBar->Top;
				saveOutcome->Visible = false;
			}

			void _initializeEvents()
			{
				menuButtons[0]->addMouseDownEvent(gcnew MouseEventHandler(this, &HastadForm::Event_LoadKey));
				menuButtons[1]->addMouseDownEvent(gcnew MouseEventHandler(this, &HastadForm::Event_ImportKey));
				menuButtons[2]->addMouseDownEvent(gcnew MouseEventHandler(this, &HastadForm::Event_SetExponent));

				keyGenCancel->addMouseDownEvent(gcnew MouseEventHandler(this, &HastadForm::Event_CancelKeyGen));

				confirmId->addMouseDownEvent(gcnew MouseEventHandler(this, &HastadForm::Event_ConfirmId));
				clearButton->addMouseDownEvent(gcnew MouseEventHandler(this, &HastadForm::Event_ClearKeys));

				encrypt[0]->addMouseDownEvent(gcnew MouseEventHandler(this, &HastadForm::Event_EncryptFile));
				encrypt[1]->addMouseDownEvent(gcnew MouseEventHandler(this, &HastadForm::Event_EncryptMsg));

				cipherCancel->addMouseDownEvent(gcnew MouseEventHandler(this, &HastadForm::Event_CancelCipher));

				performAttack->addMouseDownEvent(gcnew MouseEventHandler(this, &HastadForm::Event_PerformAttack));

				saveOutcome->addMouseDownEvent(gcnew MouseEventHandler(this, &HastadForm::Event_SaveOutcome));
			}
	};
}