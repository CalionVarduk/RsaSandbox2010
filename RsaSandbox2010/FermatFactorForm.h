#pragma once

#include "Toolbox.h"
#include "KeyGenForm.h"
#include "RSAFactorization.h"
#include "FaultyKeyGenerator.h"
#include "FermatFaultyKeyForm.h"

namespace RsaSandbox
{
	using namespace System;
	using namespace System::Text;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;
	using namespace System::Diagnostics;

	using namespace CVNetControls;
	using namespace RsaSandbox::Attacks;
	using namespace RsaSandbox::Generation;

	public ref class FermatFactorForm : public SecondaryForm
	{
		public:
			FermatFactorForm(LabelButton^ controlButton, KeyGenForm^ keyGenForm) : SecondaryForm(controlButton)
			{
				this->Text = L"RSA Sandbox : Fermat's Factorization Method";
				this->Icon = gcnew System::Drawing::Icon(".\\cvrsa.ico");
				_initializeUI();

				key = nullptr;
				keyGen = keyGenForm;

				factorization = gcnew RSAFactorization();
				factorization->ReportProgress += gcnew TaskReportProgressHandler(this, &FermatFactorForm::Event_FactorReportProgress);
				factorization->Outcome += gcnew RSAFactorizationOutcomeHandler(this, &FermatFactorForm::Event_FactorOutcome);

				faultyGen = gcnew FaultyKeyGenerator();
				faultyGen->Outcome += gcnew FaultyGenOutcomeHandler(this, &FermatFactorForm::Event_FaultyGenOutcome);
			}

		protected:
			virtual void OnResizeEnd(EventArgs^ e) override
			{
				Form::OnResizeEnd(e);
				if(!brokenKeyNumbers->Visible) {
					if(clearButton->Enabled && performAttack->Visible && ClientSize.Height != performAttack->Bottom + 12)
						ClientSize = System::Drawing::Size(ClientSize.Width, performAttack->Bottom + 12);
				}
				else if(ClientSize.Height != brokenKeyNumbers->Bottom + 12)
					ClientSize = System::Drawing::Size(ClientSize.Width, brokenKeyNumbers->Bottom + 12);
			}

		private:
			RSAKey^ key;
			KeyGenForm^ keyGen;
			RSAFactorization^ factorization;
			FaultyKeyGenerator^ faultyGen;

#pragma region UI Fields
			Panel^ menuPanel;
			LabelButtonList^ menuButtons;

			TitledTextBoxTitledList^ keyNumbers;
			LabelButton^ clearButton;

			LabelButton^ performAttack;
			TitledLabel^ informant;
			TitledTextBoxTitledList^ brokenKeyNumbers;

			System::ComponentModel::Container ^components;
#pragma endregion

			void generateFaultyKey(Int32 primeBitLength, Int32 publicExponentBitLength, Int32 primeDifferenceBitLength)
			{
				hideUI(true);
				faultyGen->fermatUnsafe(primeBitLength, publicExponentBitLength, primeDifferenceBitLength);
			}

			void prepareAndAttack()
			{
				prepareAndLockUI();
				factorization->factor(key);
			}

			void Event_FaultyGenOutcome(FaultyKeyGenerator^ sender, RSAKey^ faultyKey)
			{
				key = faultyKey;
				showUI();
			}

			void Event_FactorOutcome(RSAFactorization^ sender, RSAKey^ brokenKey, bool cancelled)
			{
				if(!cancelled) {
					key = brokenKey;
					prepareAndUnlockUI(true);
				}
				else prepareAndUnlockUI(false);
			}

			void Event_FactorReportProgress(BackgroundTask^ sender)
			{
				if(informant->ControlRight >= keyNumbers->Right) informant->ControlText = ".";
				else informant->ControlText += ".";
				informant->toLabel()->Refresh();
			}

#pragma region UI Events
			bool loadRsaKeyFromFile(String^ fileName)
			{
				if(AppIO::loadRsaKey(fileName, key)) {
					showUI();
					return true;
				}
				return false;
			}

			bool loadRsaKeyFromGenerator()
			{
				if(keyGen->CurrentKey != nullptr) {
					key = gcnew RSAKey(keyGen->CurrentKey);
					showUI();
					return true;
				}
				return false;
			}

			void showUI()
			{
				SuspendDraw;
				keyNumbers->TitleText = "Current RSA Key:";
				keyNumbers[0]->TitleText = "Modulus (" + key->BitLength.ToString() + "-bit long):";
				keyNumbers[0]->ControlText = key->Modulus.ToString("N0");
				keyNumbers[1]->TitleText = "Public Exponent (" + RSAInt::bitCount(key->PublicExponent).ToString() + "-bit long):";
				keyNumbers[1]->ControlText = key->PublicExponent.ToString("N0");
				keyNumbers[0]->Visible = true;
				keyNumbers[1]->Visible = true;

				clearButton->Visible = true;
				performAttack->Visible = true;
				informant->Visible = false;
				hideBrokenKeyComponents();
				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, performAttack->Bottom + 12);
				ResumeDraw;
			}

			void hideUI(bool generatingFaultyKey)
			{
				SuspendDraw;
				key = nullptr;

				keyNumbers->TitleText = (generatingFaultyKey) ? "Generating faulty RSA Key..." : "Load the key from a file or import it from the Key Generator.";
				for(Int32 i = 0; i < keyNumbers->Count; ++i) {
					keyNumbers[i]->TitleText = "";
					keyNumbers[i]->ControlText = "";
					keyNumbers[i]->Visible = false;
				}

				clearButton->Visible = false;
				performAttack->Visible = false;
				informant->Visible = false;
				hideBrokenKeyComponents();
				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, keyNumbers->TitleBottom + 12);
				ResumeDraw;
			}

			void showBrokenKeyComponents()
			{
				BigInteger p, q, d, dp, dq, qinv;
				key->getAllPrivateComponents(p, q, d, dp, dq, qinv);
				Toolbox::setTitledControl(brokenKeyNumbers[0], "Prime 'p' (" + RSAInt::bitCount(p).ToString() + "-bit long):", p.ToString("N0"));
				Toolbox::setTitledControl(brokenKeyNumbers[1], "Prime 'q' (" + RSAInt::bitCount(q).ToString() + "-bit long):", q.ToString("N0"));
				Toolbox::setTitledControl(brokenKeyNumbers[2], "Private Exponent (" + RSAInt::bitCount(d).ToString() + "-bit long):", d.ToString("N0"));
				Toolbox::setTitledControl(brokenKeyNumbers[3], "Private CRT p-Exponent (" + RSAInt::bitCount(dp).ToString() + "-bit long):", dp.ToString("N0"));
				Toolbox::setTitledControl(brokenKeyNumbers[4], "Private CRT q-Exponent (" + RSAInt::bitCount(dq).ToString() + "-bit long):", dq.ToString("N0"));
				Toolbox::setTitledControl(brokenKeyNumbers[5], "Private CRT Inverted 'q' (" + RSAInt::bitCount(qinv).ToString() + "-bit long):", qinv.ToString("N0"));

				brokenKeyNumbers->Visible = true;
				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, brokenKeyNumbers->Bottom + 12);
			}

			void hideBrokenKeyComponents()
			{
				for(Int32 i = 0; i < brokenKeyNumbers->Count; ++i) {
					brokenKeyNumbers[i]->TitleText = "";
					brokenKeyNumbers[i]->ControlText = "";
				}
				brokenKeyNumbers->Visible = false;
			}

			void prepareAndLockUI()
			{
				SuspendDraw;
				performAttack->Text = "CANCEL";
				Toolbox::setTitledControl(informant, "Performing factorization (may take a long time)", ".");
				informant->Left = keyNumbers->Left;
				informant->Visible = true;
				clearButton->Enabled = false;
				hideBrokenKeyComponents();

				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, informant->Bottom + 12);
				ResumeDraw;
			}

			void prepareAndUnlockUI(bool ok)
			{
				SuspendDraw;
				performAttack->Text = "PERFORM ATTACK";
				clearButton->Enabled = true;

				if(ok) {
					Toolbox::setTitledControl(informant, "Time taken:", factorization->TimeElapsedMs.ToString("N0") + " ms");
					informant->CentreX = performAttack->CentreX;
					showBrokenKeyComponents();
				}
				else {
					informant->Visible = false;
					ClientSize = System::Drawing::Size(keyNumbers->Right + 12, performAttack->Bottom + 12);
				}
				ResumeDraw;
			}
#pragma endregion

#pragma region User Events
			void Event_LoadKey(Object^ sender, MouseEventArgs^ e)
			{
				if(!factorization->IsBusy && !faultyGen->IsBusy) {
					OpenFileDialog^ dialog = gcnew OpenFileDialog();
					dialog->Filter = "RSA Key |*.rsakey";
					dialog->ShowDialog();

					if(!String::IsNullOrEmpty(dialog->FileName)) {
						if(loadRsaKeyFromFile(dialog->FileName))
							SimpleBox::display("The RSA Key has been successfully loaded!", "Load RSA Key from file");
						else SimpleBox::display("RSA Key loading unsuccessful.", "File IO Error");
					}
				}
				else SimpleBox::display("Can not load a new RSA Key while performing an action.", "File IO Error");
			}

			void Event_ImportKey(Object^ sender, MouseEventArgs^ e)
			{
				if(!factorization->IsBusy && !faultyGen->IsBusy) {
					if(loadRsaKeyFromGenerator())
						SimpleBox::display("The RSA Key has been successfully imported!", "Import RSA Key from Generator");
					else SimpleBox::display("There is no RSA Key to import from the key generator.", "Import Error");
				}
				else SimpleBox::display("Can not import a new RSA Key while performing an action.", "Import Error");
			}

			void Event_GenFaultyKey(Object^ sender, MouseEventArgs^ e)
			{
				if(!factorization->IsBusy && !faultyGen->IsBusy) {
					FermatFaultyKeyForm^ form = gcnew FermatFaultyKeyForm();
					System::Windows::Forms::DialogResult result = form->ShowDialog();

					if(result == System::Windows::Forms::DialogResult::OK) {
						generateFaultyKey(form->ModulusBitLength >> 1, form->PublicExponentBitLength, form->PrimeDifferenceBitLength);
					}
				}
				else SimpleBox::display("Can not generate a new RSA Key while performing an action.", "Generate Error");
			}

			void Event_ClearKey(Object^ sender, MouseEventArgs^ e)
			{
				System::Windows::Forms::DialogResult result = SimpleBox::displayWithResult("Are you sure you want to clear the current key?", "Clear");
				if(result == System::Windows::Forms::DialogResult::OK) hideUI(false);
			}

			void Event_PerformAttack(Object^ sender, MouseEventArgs^ e)
			{
				if(!factorization->IsBusy) {
					bool perform = true;

					if(key->IsFermatSafe) {
						array<String^>^ msg = gcnew array<String^>(2);
						msg[0] = "The RSA Key is flagged as safe against Fermat's factorization method.";
						msg[1] = "Do you want to continue?";
						System::Windows::Forms::DialogResult result = SimpleBox::displayWithResult(msg, "Perform Attack");
						if(result != System::Windows::Forms::DialogResult::OK) perform = false;
					}

					if(perform) prepareAndAttack();
				}
				else factorization->requestCancel();
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
				_initializeAttackButton();
				_initializeInformant();
				_initializeBrokenKeyDisplay();
				_initializeEvents();

				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, keyNumbers->TitleBottom + 12);
				menuPanel->Width = ClientSize.Width + 2;
				performAttack->CentreX = ClientSize.Width >> 1;
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

				menuButtons->addButton("menuLoad", "LOAD KEY FROM FILE");
				menuButtons->addButton("menuImport", "IMPORT KEY FROM GENERATOR");
				menuButtons->addButton("menuFaultyGen", "GENERATE FAULTY KEY");

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

				keyNumbers->TitleText = "Load the key from a file or import it from the Key Generator.";
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
				keyNumbers->Location = Point(12, menuPanel->Bottom + 12);
			}

			void _initializeClearButton()
			{
				clearButton = gcnew LabelButton(this, "bClearKey", "CLEAR");
			
				LabelButtonScheme::Invert(clearButton, false, ColorFactory::Scale(Color::DeepSkyBlue, 0.85f), Color::Azure, 1.1f);
				clearButton->BorderColor = clearButton->BackColorNormal;
				clearButton->AutoSize = false;
				clearButton->Font = menuButtons[0]->Font;
				clearButton->Size = System::Drawing::Size(165, 21);

				clearButton->Right = keyNumbers->Right;
				clearButton->Top = keyNumbers->Top;
				clearButton->Visible = false;
			}

			void _initializeAttackButton()
			{
				performAttack = gcnew LabelButton(this, "bAttack", "PERFORM ATTACK");
			
				LabelButtonScheme::Invert(performAttack, false, ColorFactory::Scale(Color::LightCoral, 0.85f), Color::WhiteSmoke, 1.1f);
				performAttack->BorderColor = performAttack->BackColorNormal;
				performAttack->AutoSize = false;
				performAttack->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 10.5f, FontStyle::Bold);
				performAttack->Size = System::Drawing::Size(250, 25);

				performAttack->Top = keyNumbers->Bottom + 15;
				performAttack->Visible = false;
			}

			void _initializeInformant()
			{
				informant = gcnew TitledLabel(this, "informant", "informant", "");
				informant->ControlAutoSize = true;
				informant->TitleFont = gcnew System::Drawing::Font("Microsoft Sans Serif", 10.5f);
				informant->ControlFont = informant->TitleFont;
				informant->TitleForeColor = keyNumbers->TitleForeColor;
				informant->ControlForeColor = keyNumbers->TitleForeColor;
				informant->Offset = Point(0, 0);
				informant->Location = Point(keyNumbers->Left, performAttack->Bottom + 5);
				informant->Visible = false;
			}

			void _initializeBrokenKeyDisplay()
			{
				brokenKeyNumbers = gcnew TitledTextBoxTitledList(this);

				brokenKeyNumbers->addTextBox("boxP", "Prime 'p':", "");
				brokenKeyNumbers->addTextBox("boxQ", "Prime 'q':", "");
				brokenKeyNumbers->addTextBox("boxD", "Private exponent:", "");
				brokenKeyNumbers->addTextBox("boxDP", "Private CRT p-exponent:", "");
				brokenKeyNumbers->addTextBox("boxDQ", "Private CRT q-exponent:", "");
				brokenKeyNumbers->addTextBox("boxQINV", "Private CRT inverted q:", "");

				brokenKeyNumbers->TitleText = "The RSA Key has been successfully broken:";
				brokenKeyNumbers->TitleForeColor = ColorFactory::Scale(Color::DeepSkyBlue, 0.6f);
				brokenKeyNumbers->TitleFont = keyNumbers->TitleFont;
				brokenKeyNumbers->IsListVertical = true;
				brokenKeyNumbers->IsTitleVertical = true;
				brokenKeyNumbers->Offset = Point(0, 10);
				brokenKeyNumbers->Spacing = Point(0, 5);
				brokenKeyNumbers->ReadOnly = true;
				brokenKeyNumbers->Multiline = true;
				brokenKeyNumbers->TextAlign = HorizontalAlignment::Left;
				brokenKeyNumbers->WordWrap = false;
				brokenKeyNumbers->ScrollBars = ScrollBars::Horizontal;
				brokenKeyNumbers->ControlAutoSize = false;

				for(Int32 i = 0; i < brokenKeyNumbers->Count; ++i) {
					brokenKeyNumbers[i]->IsTitleVertical = true;
					brokenKeyNumbers[i]->Offset = Point(0, 2);
					brokenKeyNumbers[i]->TitleFont = keyNumbers[0]->TitleFont;
					brokenKeyNumbers[i]->ControlFont = keyNumbers[0]->TitleFont;
					brokenKeyNumbers[i]->TitleForeColor = brokenKeyNumbers->TitleForeColor;
					brokenKeyNumbers[i]->ControlForeColor = brokenKeyNumbers->TitleForeColor;
					brokenKeyNumbers[i]->ControlBackColor = Color::Azure;
					brokenKeyNumbers[i]->ControlHeight = 38;
					brokenKeyNumbers[i]->ControlWidth = keyNumbers[0]->ControlWidth;
					brokenKeyNumbers[i]->Visible = false;
				}

				brokenKeyNumbers->performLayout();
				brokenKeyNumbers->performHorizontalAlignment();
				brokenKeyNumbers->Location = Point(keyNumbers->Left, informant->Bottom + 10);
				brokenKeyNumbers->Visible = false;
			}

			void _initializeEvents()
			{
				menuButtons[0]->addMouseDownEvent(gcnew MouseEventHandler(this, &FermatFactorForm::Event_LoadKey));
				menuButtons[1]->addMouseDownEvent(gcnew MouseEventHandler(this, &FermatFactorForm::Event_ImportKey));
				menuButtons[2]->addMouseDownEvent(gcnew MouseEventHandler(this, &FermatFactorForm::Event_GenFaultyKey));

				clearButton->addMouseDownEvent(gcnew MouseEventHandler(this, &FermatFactorForm::Event_ClearKey));
				performAttack->addMouseDownEvent(gcnew MouseEventHandler(this, &FermatFactorForm::Event_PerformAttack));
			}
#pragma endregion
	};
}