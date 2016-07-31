#pragma once

#include "Toolbox.h"
#include "KeyGenForm.h"
#include "CommonModulusAttack.h"

namespace RsaSandbox
{
	using namespace System;
	using namespace System::Text;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;
	using namespace System::Diagnostics;

	using namespace CVNetControls;
	using namespace RsaSandbox::Attacks;

	public ref class CommonModForm : public SecondaryForm
	{
		public:
			CommonModForm(LabelButton^ controlButton, KeyGenForm^ keyGenForm) : SecondaryForm(controlButton)
			{
				this->Text = L"RSA Sandbox : Common Modulus Attack";
				this->Icon = gcnew System::Drawing::Icon(".\\cvrsa.ico");
				_initializeUI();

				key1 = key2 = nullptr;
				keyGen = keyGenForm;
				
				commonModulus = gcnew CommonModulusAttack();
				commonModulus->ReportProgress += gcnew TaskReportProgressHandler(this, &CommonModForm::Event_AttackReportProgress);
				commonModulus->Outcome += gcnew CommonModulusAttackOutcomeHandler(this, &CommonModForm::Event_AttackOutcome);
			}

		protected:
			virtual void OnResizeEnd(EventArgs^ e) override
			{
				Form::OnResizeEnd(e);
				if(brokenKeyNumbers->Visible && ClientSize.Height != brokenKeyNumbers->Bottom + 12)
					ClientSize = System::Drawing::Size(ClientSize.Width, brokenKeyNumbers->Bottom + 12);
			}

		private:
			RSAKey^ key1;
			RSAKey^ key2;
			KeyGenForm^ keyGen;
			CommonModulusAttack^ commonModulus;

#pragma region UI Fields
			Panel^ menuPanel;
			LabelButtonList^ menuButtons;

			TitledTextBoxTitledList^ keyNumbers;
			LabelButton^ clearButton;
			TitledCheckBoxTitledList^ keyChoice;
			Label^ load2ndKey;

			LabelButton^ performAttack;
			TitledLabel^ informant;
			TitledTextBoxTitledList^ brokenKeyNumbers;

			System::ComponentModel::Container ^components;
#pragma endregion

			void prepareAndAttack()
			{
				prepareAndLockUI();
				commonModulus->tryToBreak(key1, key2);
			}

			void Event_AttackOutcome(CommonModulusAttack^ sender, RSAKey^ brokenKey1, RSAKey^ brokenKey2, bool cancelled)
			{
				if(!cancelled) {
					key1 = brokenKey1;
					key2 = brokenKey2;
					prepareAndUnlockUI(true);
				}
				else prepareAndUnlockUI(false);
			}

			void Event_AttackReportProgress(BackgroundTask^ sender)
			{
				informant->ControlText = sender->CurrentStep.ToString();
				informant->toLabel()->Refresh();
			}

#pragma region UI Events
			Int32 loadRsaKeyFromFile(String^ fileName, bool first)
			{
				RSAKey^ key;
				if(AppIO::loadRsaKey(fileName, key)) {
					return loadExternalKey(key, first);
				}
				return 1;
			}

			Int32 loadRsaKeyFromGenerator(bool first)
			{
				if(keyGen->CurrentKey != nullptr) {
					return loadExternalKey(gcnew RSAKey(keyGen->CurrentKey), first);
				}
				return 1;
			}

			Int32 loadExternalKey(RSAKey^ key, bool first)
			{
				if(!first) {
					if(key2 == nullptr) {
						if(key->Modulus != key1->Modulus) return 2;
						else if(key->PublicExponent == key1->PublicExponent) return 3;
						else key2 = key;
						loadSecondKey();
					}
					else if(keyChoice[0]->Checked) {
						if(key->Modulus != key2->Modulus) return 2;
						else if(key->PublicExponent == key2->PublicExponent) return 3;
						else if (key->IsPasswordProtected && key2->IsPasswordProtected) return 4;
						else {
							key1 = key;
							updateCurrentKey(key1);
						}
					}
					else {
						if(key->Modulus != key1->Modulus) return 2;
						else if(key->PublicExponent == key1->PublicExponent) return 3;
						else if (key->IsPasswordProtected && key1->IsPasswordProtected) return 4;
						else {
							key2 = key;
							updateCurrentKey(key2);
						}
					}
				}
				else {
					key1 = key;
					showUI();
				}
				return 0;
			}

			void showUI()
			{
				SuspendDraw;
				menuButtons[0]->Text = "LOAD 2ND KEY FROM FILE";
				menuButtons[1]->Text = "IMPORT 2ND KEY FROM GENERATOR";
				menuButtons->performLayout();

				displayKey(key1);
				keyNumbers[0]->Visible = true;
				keyNumbers[1]->Visible = true;

				clearButton->Visible = true;
				load2ndKey->Visible = true;
				informant->Visible = false;
				performAttack->Visible = false;
				hideBrokenKeyComponents();
				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, load2ndKey->Bottom + 12);
				ResumeDraw;
			}

			void loadSecondKey()
			{
				SuspendDraw;
				menuButtons[0]->Text = "LOAD CURRENT KEY FROM FILE";
				menuButtons[1]->Text = "IMPORT CURRENT KEY FROM GENERATOR";
				menuButtons->performLayout();	

				keyChoice[0]->Checked = true;
				keyChoice[1]->Checked = false;
				keyChoice->Visible = true;
				performAttack->Visible = true;
				load2ndKey->Visible = false;
				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, performAttack->Bottom + 12);
				ResumeDraw;
			}

			void updateCurrentKey(RSAKey^ key)
			{
				SuspendDraw;
				hideBrokenKeyComponents();
				displayKey(key);

				informant->Visible = false;
				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, performAttack->Bottom + 12);
				ResumeDraw;
			}

			void hideUI()
			{
				SuspendDraw;
				menuButtons[0]->Text = "LOAD 1ST KEY FROM FILE";
				menuButtons[1]->Text = "IMPORT 1ST KEY FROM GENERATOR";
				menuButtons->performLayout();
				key1 = nullptr;

				keyNumbers->TitleText = "Load the 1st key from a file or import it from the Key Generator.";
				for(Int32 i = 0; i < keyNumbers->Count; ++i) {
					keyNumbers[i]->TitleText = "";
					keyNumbers[i]->ControlText = "";
					keyNumbers[i]->Visible = false;
				}

				clearButton->Visible = false;
				load2ndKey->Visible = false;
				informant->Visible = false;
				performAttack->Visible = false;
				hideBrokenKeyComponents();
				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, keyNumbers->TitleBottom + 12);
				ResumeDraw;
			}

			void removeFirstKey()
			{
				key1 = key2;
				removeSecondKey();
			}

			void removeSecondKey()
			{
				SuspendDraw;
				menuButtons[0]->Text = "LOAD 2ND KEY FROM FILE";
				menuButtons[1]->Text = "IMPORT 2ND KEY FROM GENERATOR";
				menuButtons->performLayout();
				key2 = nullptr;

				displayKey(key1);

				keyChoice[0]->Checked = true;
				keyChoice[1]->Checked = false;
				keyChoice->Visible = false;
				performAttack->Visible = false;
				informant->Visible = false;
				load2ndKey->Visible = true;
				hideBrokenKeyComponents();
				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, load2ndKey->Bottom + 12);
				ResumeDraw;
			}

			void displayKey(RSAKey^ key)
			{
				keyNumbers->TitleText = "Current RSA Key" + ((key->IsPasswordProtected) ? " (protected):" : ":");
				keyNumbers[0]->TitleText = "Modulus (" + key->BitLength.ToString() + "-bit long):";
				keyNumbers[0]->ControlText = key->Modulus.ToString("N0");
				keyNumbers[1]->TitleText = "Public Exponent (" + RSAInt::bitCount(key->PublicExponent).ToString() + "-bit long):";
				keyNumbers[1]->ControlText = key->PublicExponent.ToString("N0");

				if(brokenKeyNumbers->Visible) {
					BigInteger p, q, d, dp, dq, qinv;
					key->getAllPrivateComponents(p, q, d, dp, dq, qinv);
					Toolbox::setTitledControl(brokenKeyNumbers[0], "Prime 'p' (" + RSAInt::bitCount(p).ToString() + "-bit long):", p.ToString("N0"));
					Toolbox::setTitledControl(brokenKeyNumbers[1], "Prime 'q' (" + RSAInt::bitCount(q).ToString() + "-bit long):", q.ToString("N0"));
					Toolbox::setTitledControl(brokenKeyNumbers[2], "Private Exponent (" + RSAInt::bitCount(d).ToString() + "-bit long):", d.ToString("N0"));
					Toolbox::setTitledControl(brokenKeyNumbers[3], "Private CRT p-Exponent (" + RSAInt::bitCount(dp).ToString() + "-bit long):", dp.ToString("N0"));
					Toolbox::setTitledControl(brokenKeyNumbers[4], "Private CRT q-Exponent (" + RSAInt::bitCount(dq).ToString() + "-bit long):", dq.ToString("N0"));
					Toolbox::setTitledControl(brokenKeyNumbers[5], "Private CRT Inverted 'q' (" + RSAInt::bitCount(qinv).ToString() + "-bit long):", qinv.ToString("N0"));
				}
			}

			void showBrokenKeyComponents(RSAKey^ key)
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
				Toolbox::setTitledControl(informant, "Performing Common Modulus attack:", "0");
				informant->Left = keyNumbers->Left;
				informant->Visible = true;
				clearButton->Enabled = false;
				keyChoice->Enabled = false;
				hideBrokenKeyComponents();

				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, informant->Bottom + 12);
				ResumeDraw;
			}

			void prepareAndUnlockUI(bool ok)
			{
				SuspendDraw;
				performAttack->Text = "PERFORM ATTACK";
				clearButton->Enabled = true;
				keyChoice->Enabled = true;

				if(ok) {
					Toolbox::setTitledControl(informant, "Time taken:", commonModulus->TimeElapsedMs.ToString("N0") + " ms");
					informant->CentreX = performAttack->CentreX;
					RSAKey^ active = (keyChoice[0]->Checked) ? key1 : key2;
					displayKey(active);
					showBrokenKeyComponents(active);
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
				if(!commonModulus->IsBusy) {
					OpenFileDialog^ dialog = gcnew OpenFileDialog();
					dialog->Filter = "RSA Key |*.rsakey";
					dialog->ShowDialog();

					if(!String::IsNullOrEmpty(dialog->FileName)) {
						Int32 result = loadRsaKeyFromFile(dialog->FileName, key1 == nullptr);
						if(result == 0) SimpleBox::display("The RSA Key has been successfully loaded!", "Load RSA Key from file");
						else if(result == 1) SimpleBox::display("RSA Key loading unsuccessful.", "File IO Error");
						else if(result == 2) SimpleBox::display("Keys' modulus' must be equal.", "Load Error");
						else if(result == 3) SimpleBox::display("Key's public exponents must not be equal.", "Load Error");
						else SimpleBox::display("At least one of the keys must not be password protected.", "Load Error");
					}
				}
				else SimpleBox::display("Can not load a new RSA Key while performing an attack.", "File IO Error");
			}

			void Event_ImportKey(Object^ sender, MouseEventArgs^ e)
			{
				if(!commonModulus->IsBusy) {
					Int32 result = loadRsaKeyFromGenerator(key1 == nullptr);
					if(result == 0) SimpleBox::display("The RSA Key has been successfully imported!", "Import RSA Key from Generator");
					else if(result == 1) SimpleBox::display("There is no RSA Key to import from the key generator.", "Import Error");
					else if(result == 2) SimpleBox::display("Keys' modulus' must be equal.", "Import Error");
					else if(result == 3) SimpleBox::display("Key's public exponents must not be equal.", "Import Error");
					else SimpleBox::display("At least one of the keys must not be password protected.", "Import Error");
				}
				else SimpleBox::display("Can not load a new RSA Key while performing an attack.", "Import Error");
			}

			void Event_ClearKey(Object^ sender, MouseEventArgs^ e)
			{
				System::Windows::Forms::DialogResult result = SimpleBox::displayWithResult("Are you sure you want to clear current key?", "Clear");
				if(result == System::Windows::Forms::DialogResult::OK) {
					if(!keyChoice->Visible) hideUI();
					else if(keyChoice[0]->Checked) removeFirstKey();
					else removeSecondKey();
				}
			}

			void Event_FirstKeyChosen(Object^ sender, MouseEventArgs^ e)
			{
				keyChoice[1]->Checked = false;
				if(keyChoice[0]->Checked) {
					displayKey(key1);
				}
				else keyChoice[0]->Checked = true;
			}

			void Event_SecondKeyChosen(Object^ sender, MouseEventArgs^ e)
			{
				keyChoice[0]->Checked = false;
				if(keyChoice[1]->Checked) {
					displayKey(key2);
				}
				else keyChoice[1]->Checked = true;
			}

			void Event_PerformAttack(Object^ sender, MouseEventArgs^ e)
			{
				if(!commonModulus->IsBusy) prepareAndAttack();
				else commonModulus->requestCancel();
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
				_initializeKeyChoiceBoxes();
				_initialize2ndKeyLabel();
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

				menuButtons->addButton("menuLoad1", "LOAD 1ST KEY FROM FILE");
				menuButtons->addButton("menuImport1", "IMPORT 1ST KEY FROM GENERATOR");

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

				keyNumbers->TitleText = "Load the 1st key from a file or import it from the Key Generator.";
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
					keyNumbers[i]->ControlWidth = keyNumbers->TitleRight + 90;
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

			void _initializeKeyChoiceBoxes()
			{
				keyChoice = gcnew TitledCheckBoxTitledList(this);
			
				keyChoice->addCheckBox("choiceKey1", "First:", false);
				keyChoice->addCheckBox("choiceKey2", "Second:", false);

				keyChoice->TitleText = "Displayed key:";
				keyChoice->TitleFont = gcnew System::Drawing::Font(clearButton->Font, FontStyle::Regular);
				keyChoice->TitleForeColor = ColorFactory::Scale(Color::DeepSkyBlue, 0.6f);
				keyChoice->IsTitleVertical = true;
				keyChoice->IsListVertical = true;
				keyChoice->Offset = Point(0, -1);
				keyChoice->Spacing = Point(0, -4);
				keyChoice->BorderColor = ColorFactory::Scale(Color::DeepSkyBlue, 0.85f);
			
				for(Int32 i = 0; i < keyChoice->Count; ++i) {
					keyChoice[i]->TitleFont = keyChoice->TitleFont;
					keyChoice[i]->TitleForeColor = keyChoice->TitleForeColor;
					keyChoice[i]->ControlForeColor = keyChoice[0]->BorderColor;
					keyChoice[i]->ControlBackColor = Color::Azure;
					keyChoice[i]->ControlSize = System::Drawing::Size(12, 12);
					keyChoice[i]->OffsetX = 10;
					keyChoice[i]->OffsetY += keyChoice[i]->MiddleAlignmentY + 2;
				}

				keyChoice->performLayout();
				keyChoice->performHorizontalAlignment();
				keyChoice->Right = clearButton->Left - 10;
				keyChoice->Top = keyNumbers->Top - 5;
				keyChoice->Visible = false;
			}

			void _initialize2ndKeyLabel()
			{
				load2ndKey = gcnew Label();
				load2ndKey->BackColor = Color::Transparent;
				load2ndKey->ForeColor = keyNumbers->TitleForeColor;
				load2ndKey->Font = keyNumbers->TitleFont;
				load2ndKey->AutoSize = true;
				load2ndKey->Text = "Load the 2nd key from a file or import it from the Key Generator.";
				Controls->Add(load2ndKey);
				load2ndKey->Location = Point(keyNumbers->Left, keyNumbers->Bottom + 12);
				load2ndKey->Visible = false;
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
				menuButtons[0]->addMouseDownEvent(gcnew MouseEventHandler(this, &CommonModForm::Event_LoadKey));
				menuButtons[1]->addMouseDownEvent(gcnew MouseEventHandler(this, &CommonModForm::Event_ImportKey));

				keyChoice[0]->addMouseDownEvent(gcnew MouseEventHandler(this, &CommonModForm::Event_FirstKeyChosen));
				keyChoice[1]->addMouseDownEvent(gcnew MouseEventHandler(this, &CommonModForm::Event_SecondKeyChosen));

				clearButton->addMouseDownEvent(gcnew MouseEventHandler(this, &CommonModForm::Event_ClearKey));
				performAttack->addMouseDownEvent(gcnew MouseEventHandler(this, &CommonModForm::Event_PerformAttack));
			}
#pragma endregion
	};
}