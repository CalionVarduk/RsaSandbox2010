#pragma once

#include "FermatFactorForm.h"
#include "CommonModForm.h"
#include "HastadForm.h"
#include "WienerForm.h"
#include "EncryptMessageForm.h"
#include "RSACipher.h"

namespace RsaSandbox
{
	using namespace System;
	using namespace System::Text;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Diagnostics;

	using namespace CVNetControls;
	using namespace RsaSandbox::Ciphering;

	public ref class MainForm : public Form
	{
	public:
		MainForm()
		{
			InitializeComponent();
			_initializeUI();

			keyGen = gcnew KeyGenForm(menuButtons[2]);

			LabelButtonList^ attackButtons = ((MenuButton^)menuButtons[3])->MenuItems;
			fermatFactor = gcnew FermatFactorForm(attackButtons[0], keyGen);
			commonMod = gcnew CommonModForm(attackButtons[1], keyGen);
			hastadAttack = gcnew HastadForm(attackButtons[2], keyGen);
			wienerAttack = gcnew WienerForm(attackButtons[3], keyGen);

			key = nullptr;
			cipher = gcnew RSACipher();
			cipher->PaddingMode = RSACipherPaddingMode::OAEP;
			cipher->DecryptionMode = RSACipherDecryptionMode::CRTBlinding;
			cipher->ReportProgress += gcnew TaskReportProgressHandler(this, &MainForm::Event_CipherReportProgress);
			cipher->Outcome += gcnew RSACipherOutcomeHandler(this, &MainForm::Event_CipherOutcome);
			isOutputCipher = false;

			keyGen->setExportKeyEvent(gcnew Func<bool>(this, &MainForm::keyGenExport));
		}

	protected:
		~MainForm()
		{
			if(components)
				delete components;
		}

		virtual void OnPaint(PaintEventArgs^ e) override
		{
			Form::OnPaint(e);
			if(key != nullptr) e->Graphics->DrawLine(Pens::SteelBlue, 20, passwordButtons->Bottom + 10, keyNumbers->Right - 8, passwordButtons->Bottom + 10);
		}

	private:
		RSAKey^ key;
		RSACipher^ cipher;
		KeyGenForm^ keyGen;
		FermatFactorForm^ fermatFactor;
		CommonModForm^ commonMod;
		HastadForm^ hastadAttack;
		WienerForm^ wienerAttack;
		array<Byte>^ cipherOutput;
		bool isOutputCipher;

#pragma region UI Fields
		Panel^ menuPanel;
		LabelButtonList^ menuButtons;

		TitledTextBoxTitledList^ keyNumbers;
		LabelButtonList^ keyButtons;
		LabelButtonList^ passwordButtons;

		TitledCheckBoxTitledList^ cipherSettings;
		LabelButtonList^ encrypt;
		LabelButton^ decrypt;

		LabelButton^ cancel;
		SingleProgressBar^ progressBar;
		TitledLabel^ informant;
		LabelButton^ saveOutcome;

		DynamicControlCollection^ movableControls;

		System::ComponentModel::Container ^components;
#pragma endregion

		void encryptData(array<Byte>^ data)
		{
			cipher->Key = key;
			isOutputCipher = true;

			if(cipherSettings[1]->Checked) data = HuffmanCompression::compress(data);
			cipher->encrypt(data);
		}

		void decryptData(array<Byte>^ data, String^ password)
		{
			cipher->Key = key;
			isOutputCipher = false;

			try { cipher->decrypt(data, password); }
			catch(Exception^ exc) {
				prepareAndUnlockUI(false);
				SimpleBox::display(exc->Message, "Decryption Error");
			}
		}

		void Event_CipherOutcome(RSACipher^ sender, array<Byte>^ output, RSACipherOutcome outcome)
		{
			cipher->Key = nullptr;

			if(outcome == RSACipherOutcome::OK) {
				cipherOutput = output;
				if(!isOutputCipher && cipherSettings[1]->Checked) cipherOutput = HuffmanCompression::decompress(cipherOutput);
				prepareAndUnlockUI(true);
			}
			else {
				prepareAndUnlockUI(false);
				if(outcome == RSACipherOutcome::WrongDecryptionKey)
					SimpleBox::display("The key is incompatible with the provided cipher using the current padding scheme.", "Decryption Error");
			}
		}

		void Event_CipherReportProgress(BackgroundTask^ sender)
		{
			float progress = sender->CurrentStep / (float)sender->StepCount;
			informant->ControlText = sender->CurrentStep.ToString() + " / " + sender->StepCount.ToString() + " (" + ((Int32)(progress * 100)).ToString() + "%)";
			progressBar->ProgressPercentage = progress;
			informant->toLabel()->Refresh();
		}

#pragma region UI Events
		int saveOutcomeToFile(String^ fileName)
		{
			if(AppIO::saveFile(fileName, cipherOutput)) {
				if(cipherOutput != nullptr) Array::Clear(cipherOutput, 0, cipherOutput->Length);
				cipherOutput = nullptr;
				int result = (isOutputCipher) ? 1 : 2;
				isOutputCipher = false;

				SuspendDraw;
				saveOutcome->Visible = false;
				informant->Visible = false;
				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, decrypt->Bottom + 12);
				ResumeDraw;
				return result;
			}
			return 0;
		}

		bool saveRsaKeyToFile(String^ fileName)
		{
			return AppIO::saveRsaKey(fileName, key);
		}

		bool loadRsaKeyFromFile(String^ fileName)
		{
			if(AppIO::loadRsaKey(fileName, key)) {
				showUI();
				return true;
			}
			return false;
		}

		void showUI()
		{
			SuspendDraw;
			hidePrivateKeyComponents(false);
			keyNumbers->TitleText = "Current RSA Key" + ((key->IsPasswordProtected) ? " (protected):" : ":");
			keyNumbers[0]->TitleText = "Modulus (" + key->BitLength.ToString() + "-bit long):";
			keyNumbers[0]->ControlText = key->Modulus.ToString("N0");
			keyNumbers[1]->TitleText = "Public Exponent (" + RSAInt::bitCount(key->PublicExponent).ToString() + "-bit long):";
			keyNumbers[1]->ControlText = key->PublicExponent.ToString("N0");
			keyNumbers[0]->Visible = true;
			keyNumbers[1]->Visible = true;

			keyButtons->Visible = true;
			passwordButtons->Visible = true;
			cipherSettings->Visible = true;
			encrypt->Visible = true;
			decrypt->Visible = true;
			ClientSize = System::Drawing::Size(keyNumbers->Right + 12, ((informant->Visible) ? saveOutcome->Bottom : decrypt->Bottom) + 12);
			ResumeDraw;
		}

		void hideUI()
		{
			SuspendDraw;
			key = nullptr;
			isOutputCipher = false;
			if(cipherOutput != nullptr) Array::Clear(cipherOutput, 0, cipherOutput->Length);
			cipherOutput = nullptr;

			keyNumbers->TitleText = "Load the key from a file or export it from the Key Generator.";
			for(Int32 i = 0; i < keyNumbers->Count; ++i) {
				keyNumbers[i]->TitleText = "";
				keyNumbers[i]->ControlText = "";
				keyNumbers[i]->Visible = false;
			}

			keyButtons->Visible = false;
			movableControls->Visible = false;
			ClientSize = System::Drawing::Size(keyNumbers->Right + 12, keyNumbers->TitleBottom + 12);
			ResumeDraw;
		}

		void showPrivateKeyComponents(BigInteger% d, BigInteger% dp, BigInteger% dq)
		{
			SuspendDraw;
			keyNumbers[2]->TitleText = "Private Exponent (" + RSAInt::bitCount(d).ToString() + "-bit long):";
			keyNumbers[2]->ControlText = d.ToString("N0");
			keyNumbers[3]->TitleText = "Private CRT p-Exponent (" + RSAInt::bitCount(dp).ToString() + "-bit long):";
			keyNumbers[3]->ControlText = dp.ToString("N0");
			keyNumbers[4]->TitleText = "Private CRT q-Exponent (" + RSAInt::bitCount(dq).ToString() + "-bit long):";
			keyNumbers[4]->ControlText = dq.ToString("N0");

			for(Int32 i = 2; i < keyNumbers->Count; ++i) keyNumbers[i]->Visible = true;
			passwordButtons[0]->Text = "HIDE PRIVATE EXP";
			movableControls->Top = keyNumbers->Bottom + 15;
			ClientSize = System::Drawing::Size(keyNumbers->Right + 12, ((informant->Visible) ? saveOutcome->Bottom : decrypt->Bottom) + 12);
			ResumeDraw;
		}

		void hidePrivateKeyComponents(bool updateSize)
		{
			if(updateSize) SuspendDraw;
			for(Int32 i = 2; i < keyNumbers->Count; ++i) {
				keyNumbers[i]->TitleText = "";
				keyNumbers[i]->ControlText = "";
				keyNumbers[i]->Visible = false;
			}
			passwordButtons[0]->Text = "SHOW PRIVATE EXP";
			movableControls->Top = keyNumbers[1]->Bottom + 15;
			if(updateSize) {
				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, ((informant->Visible) ? saveOutcome->Bottom : decrypt->Bottom) + 12);
				ResumeDraw;
			}
		}

		void prepareAndLockUI(bool encrypting)
		{
			SuspendDraw;
			keyButtons[0]->Enabled = false;
			cipherSettings->Enabled = false;
			encrypt->Enabled = false;
			decrypt->Enabled = false;

			Toolbox::setTitledControl(informant, (encrypting) ? "Encrypting:" : "Decrypting:", "0%");
			informant->Visible = true;

			progressBar->ProgressPercentage = 0;
			progressBar->Visible = true;
			cancel->Visible = true;
			ClientSize = System::Drawing::Size(keyNumbers->Right + 12, saveOutcome->Bottom + 12);
			ResumeDraw;
		}

		void prepareAndUnlockUI(bool ok)
		{
			SuspendDraw;
			keyButtons[0]->Enabled = true;
			cipherSettings->Enabled = true;
			encrypt->Enabled = true;
			decrypt->Enabled = true;

			progressBar->Visible = false;
			cancel->Visible = false;

			if(ok) {
				Toolbox::setTitledControl(informant, ((isOutputCipher) ? "Encryption" : "Decryption") + " time taken:", cipher->TimeElapsedMs.ToString("N0") + " ms");
				saveOutcome->Visible = true;
			}
			else {
				isOutputCipher = false;
				informant->Visible = false;
				ClientSize = System::Drawing::Size(keyNumbers->Right + 12, decrypt->Bottom + 12);
			}
			ResumeDraw;
		}

		bool prepareToEncryptFile(String^ fileName)
		{
			array<Byte>^ data;
			if(AppIO::openFile(fileName, data)) {
				prepareAndLockUI(true);
				encryptData(data);
				return true;
			}
			return false;
		}

		void prepareAndEncryptMsg(String^ message)
		{
			prepareAndLockUI(true);
			encryptData(Encoding::UTF8->GetBytes(message));
		}

		bool prepareAndDecryptFile(String^ fileName, String^ password)
		{
			array<Byte>^ data;
			if(AppIO::openFile(fileName, data)) {
				prepareAndLockUI(false);
				decryptData(data, password);
				return true;
			}
			return false;
		}
#pragma endregion

#pragma region User Events
		bool authenticateKeyBeforeDecryption(String^% password)
		{
			if(key->IsPasswordProtected) {
				password = Toolbox::getPassword(key);
				if(password == nullptr)	return false;
			}
			return true;
		}

		void Event_SaveKey(Object^ sender, MouseEventArgs^ e)
		{
			if(key != nullptr) {
				SaveFileDialog^ dialog = gcnew SaveFileDialog();
				dialog->Filter = "RSA Key |*.rsakey";
				dialog->ShowDialog();

				if(!String::IsNullOrEmpty(dialog->FileName)) {
					if(saveRsaKeyToFile(dialog->FileName))
						SimpleBox::display("The RSA Key has been successfully saved!", "Save RSA Key to file");
					else SimpleBox::display("RSA Key saving unsuccessful.", "File IO Error");
				}
			}
			else SimpleBox::display("RSA Key is not set.", "File IO Error");
		}

		void Event_LoadKey(Object^ sender, MouseEventArgs^ e)
		{
			if(!cipher->IsBusy) {
				OpenFileDialog^ dialog = gcnew OpenFileDialog();
				dialog->Filter = "RSA Key |*.rsakey";
				dialog->ShowDialog();

				if(!String::IsNullOrEmpty(dialog->FileName)) {
					if(loadRsaKeyFromFile(dialog->FileName))
						SimpleBox::display("The RSA Key has been successfully loaded!", "Load RSA Key from file");
					else SimpleBox::display("RSA Key loading unsuccessful.", "File IO Error");
				}
			}
			else SimpleBox::display("Can not load a new RSA Key while encrypting or decrypting.", "File IO Error");
		}

		bool keyGenExport()
		{
			if(!cipher->IsBusy) {
				key = keyGen->CurrentKey;
				System::Windows::Forms::DialogResult result = SimpleBox::displayWithResult("Do you want to protect the key with a password?", "Export Key");

				if(result == System::Windows::Forms::DialogResult::OK) {
					bool passwordSet = Toolbox::setPassword(key);
					showUI();
					if(passwordSet) SimpleBox::display("Key exported successfully (Password has been set).", "Export Key");
					else SimpleBox::display("Key exported successfully (Failed to set a password).", "Export Key");
				}
				else {
					showUI();
					SimpleBox::display("Key exported successfully.", "Export Key");
				}
				return true;
			}
			SimpleBox::display("Can not load a new RSA Key while encrypting or decrypting.", "Export Key Error");
			return false;
		}

		void Event_ClearKey(Object^ sender, MouseEventArgs^ e)
		{
			System::Windows::Forms::DialogResult result = SimpleBox::displayWithResult("Are you sure you want to clear the current key?", "Clear");
			if(result == System::Windows::Forms::DialogResult::OK) hideUI();
		}

		void Event_CheckKey(Object^ sender, MouseEventArgs^ e)
		{
			Toolbox::displayKeySafetyInfo(key);
		}

		void Event_ShowHidePrivate(Object^ sender, MouseEventArgs^ e)
		{
			if(!keyNumbers[2]->Visible) {
				BigInteger d, dp, dq;
				bool greenSignal = true;

				if(key->IsPasswordProtected) {
					if(!Toolbox::getKeyPrivateComponents(key, d, dp, dq)) greenSignal = false;
				}
				else key->getPrivateExponents(d, dp, dq);

				if(greenSignal) showPrivateKeyComponents(d, dp, dq);
				else SimpleBox::display("Failed to show current key's private exponents.", "Show private exponents");

				d = dp = dq = BigInteger::Zero;
			}
			else hidePrivateKeyComponents(true);
		}

		void Event_RemovePswrd(Object^ sender, MouseEventArgs^ e)
		{
			if(key->IsPasswordProtected) {
				if(Toolbox::removePassword(key)) SimpleBox::display("Password has been removed.", "Remove password");
				else SimpleBox::display("Failed to remove the password.", "Remove password");
			}
			else SimpleBox::display("The Key is not password protected.", "Remove password");
		}

		void Event_SetPswrd(Object^ sender, MouseEventArgs^ e)
		{
			if(keyNumbers[2]->Visible) hidePrivateKeyComponents(true);
			if(Toolbox::setPassword(key)) SimpleBox::display("New password has been set.", "Set password");
			else SimpleBox::display("Failed to set a new password.", "Set password");
		}

		void Event_OaepSetting(Object^ sender, MouseEventArgs^ e)
		{
			cipher->PaddingMode = (cipherSettings[0]->Checked) ? RSACipherPaddingMode::OAEP : RSACipherPaddingMode::None;
		}

		void Event_CrtSetting(Object^ sender, MouseEventArgs^ e)
		{
			bool blinding = (cipher->DecryptionMode == RSACipherDecryptionMode::BasicBlinding || cipher->DecryptionMode == RSACipherDecryptionMode::CRTBlinding);
			if(cipherSettings[2]->Checked)
				cipher->DecryptionMode = (blinding) ? RSACipherDecryptionMode::CRTBlinding : RSACipherDecryptionMode::CRT;
			else cipher->DecryptionMode = (blinding) ? RSACipherDecryptionMode::BasicBlinding : RSACipherDecryptionMode::Basic;
		}

		void Event_BlindingSetting(Object^ sender, MouseEventArgs^ e)
		{
			bool crt = (cipher->DecryptionMode == RSACipherDecryptionMode::CRT || cipher->DecryptionMode == RSACipherDecryptionMode::CRTBlinding);
			if(cipherSettings[3]->Checked)
				cipher->DecryptionMode = (crt) ? RSACipherDecryptionMode::CRTBlinding : RSACipherDecryptionMode::BasicBlinding;
			else cipher->DecryptionMode = (crt) ? RSACipherDecryptionMode::CRT : RSACipherDecryptionMode::Basic;
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

		void Event_Decrypt(Object^ sender, MouseEventArgs^ e)
		{
			String^ password;
			if(authenticateKeyBeforeDecryption(password)) {
				OpenFileDialog^ dialog = gcnew OpenFileDialog();
				dialog->Filter = "RSA Cipher |*.rsacipher";
				dialog->ShowDialog();

				if(!String::IsNullOrEmpty(dialog->FileName))
					if(!prepareAndDecryptFile(dialog->FileName, password))
						SimpleBox::display("Could not open the file.", "File IO Error");
			}
			else SimpleBox::display("Failed to authenticate the key for decryption.", "Authenticate key");
			password = "";
		}

		void Event_Cancel(Object^ sender, MouseEventArgs^ e)
		{
			cipher->requestCancel();
		}

		void Event_SaveOutcome(Object^ sender, MouseEventArgs^ e)
		{
			SaveFileDialog^ dialog = gcnew SaveFileDialog();
			if(isOutputCipher) dialog->Filter = "RSA Cipher |*.rsacipher";
			dialog->ShowDialog();

			if(!String::IsNullOrEmpty(dialog->FileName)) {
				int result = saveOutcomeToFile(dialog->FileName);
				if(result == 1) SimpleBox::display("The RSA Cipher has been successfully saved!", "Save outcome to file");
				else if(result == 2) SimpleBox::display("The decrypted file has been successfully saved!", "Save outcome to file");
				else SimpleBox::display("File saving unsuccessful.", "File IO Error");
			}
		}
#pragma endregion

#pragma region Initialization
		void _initializeUI()
		{
			_initializeMenuButtons();
			_initializeKeyDisplay();
			_initializeKeyButtons();
			_initializePasswordButtons();
			_initializeCipherSettings();
			_initializeEncryptButtons();
			_initializeDecryptButton();
			_initializeProgressAndOutcome();
			_initializeEvents();
			
			DoubleBuffered = true;
			ClientSize = System::Drawing::Size(keyNumbers->Right + 12, keyNumbers->TitleBottom + 12);
			menuPanel->Width = ClientSize.Width + 2;
			cipherSettings->CentreX = encrypt->CentreX - 20;
			progressBar->Width = ClientSize.Width - 24;
			cancel->Right = progressBar->Right;

			_initializeMovableControls();
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
			menuButtons->addButton("menuKeyGen", "KEY GENERATOR");
			menuButtons->add(gcnew MenuButton(this, "menuAttackSim", "ATTACK SIMULATION"));

			LabelButtonScheme::Invert(menuButtons, true, menuPanel->BackColor, Color::WhiteSmoke, 1.1f);
			menuButtons->AutoSize = true;
			menuButtons->BorderStyle = gcnew RectangularBorder();
			menuButtons[0]->ActivatedOnHold = true;
			menuButtons[1]->ActivatedOnHold = true;

			menuButtons[0]->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9, FontStyle::Bold);
			for(Int32 i = 1; i < menuButtons->Count; ++i)
				menuButtons[i]->Font = menuButtons[0]->Font;

			menuButtons->performLayout();
			_initializeAttacksMenuButton((MenuButton^)menuButtons[3]);
		}

		void _initializeAttacksMenuButton(MenuButton^ button)
		{
			button->MenuAlignedRight = true;
			button->MenuBackColor = button->BackColorActive;
			button->MenuBorderColor = button->BackColorNormal;
			button->MenuItemSpacing = 3;
			button->MenuHideDelay = 30;
			button->MenuOffsetY = 1;
			button->OnClickDeactivatable = true;

			LabelButtonList^ attackButtons = gcnew LabelButtonList();

			attackButtons->addButton("attackFermat", "FERMAT'S FACTORIZATION METHOD");
			attackButtons->addButton("attackCommonMod", "COMMON MODULUS ATTACK");
			attackButtons->addButton("attackHastad", L"HÅSTAD'S ATTACK");
			attackButtons->addButton("attackWiener", "WIENER'S ATTACK");

			LabelButtonScheme::Invert(attackButtons, true, button->MenuBackColor, menuPanel->BackColor, 0.96f);
			attackButtons->AutoSize = true;
			attackButtons->BorderStyle = gcnew RectangularBorder();
			attackButtons->TextAlign = ContentAlignment::MiddleRight;

			for(Int32 i = 0; i < attackButtons->Count; ++i)
				attackButtons[i]->Font = menuButtons[0]->Font;

			button->addButtons(attackButtons);

			attackButtons->AutoSize = false;
			for(Int32 i = 0; i < attackButtons->Count; ++i)
				attackButtons[i]->Size = System::Drawing::Size(button->MenuItems->Width, attackButtons[i]->PreferredSize.Height);
		}

		void _initializeKeyDisplay()
		{
			keyNumbers = gcnew TitledTextBoxTitledList(this);

			keyNumbers->addTextBox("boxN", "Modulus:", "");
			keyNumbers->addTextBox("boxE", "Public exponent:", "");
			keyNumbers->addTextBox("boxD", "Private exponent:", "");
			keyNumbers->addTextBox("boxDP", "Private CRT p-exponent:", "");
			keyNumbers->addTextBox("boxDQ", "Private CRT q-exponent:", "");

			keyNumbers->TitleText = "Load the key from a file or export it from the Key Generator.";
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

		void _initializeKeyButtons()
		{
			keyButtons = gcnew LabelButtonList(this);

			keyButtons->addButton("bClearKey", "CLEAR");
			keyButtons->addButton("bCheckSafety", "CHECK KEY'S SAFETY");
			
			LabelButtonScheme::Invert(keyButtons, false, ColorFactory::Scale(Color::DeepSkyBlue, 0.85f), Color::Azure, 1.1f);
			keyButtons->BorderColor = keyButtons[0]->BackColorNormal;
			keyButtons->IsListHorizontal = true;
			keyButtons->Spacing = Point(3, 0);
			keyButtons->AutoSize = false;

			for(Int32 i = 0; i < keyButtons->Count; ++i) {
				keyButtons[i]->Font = menuButtons[0]->Font;
				keyButtons[i]->Size = System::Drawing::Size(165, 21);
			}

			keyButtons->performLayout();
			keyButtons->Right = keyNumbers->Right;
			keyButtons->Top = keyNumbers->Top;
			keyButtons->Visible = false;
		}

		void _initializePasswordButtons()
		{
			passwordButtons = gcnew LabelButtonList(this);

			passwordButtons->addButton("bShowPrivate", "SHOW PRIVATE EXP");
			passwordButtons->addButton("bRemovePswrd", "REMOVE PASSWORD");
			passwordButtons->addButton("bSetPswrd", "SET PASSWORD");
			
			LabelButtonScheme::Invert(passwordButtons, false, ColorFactory::Scale(Color::DeepSkyBlue, 0.7f), Color::Azure, 1.15f);
			passwordButtons->BorderColor = passwordButtons[0]->BackColorNormal;
			passwordButtons->IsListHorizontal = true;
			passwordButtons->Spacing = Point(3, 0);
			passwordButtons->AutoSize = false;

			for(Int32 i = 0; i < passwordButtons->Count; ++i) {
				passwordButtons[i]->Font = menuButtons[0]->Font;
				passwordButtons[i]->Size = System::Drawing::Size(192, 21);
			}

			passwordButtons->performLayout();
			passwordButtons->Left = keyNumbers->Left;
		}

		void _initializeCipherSettings()
		{
			cipherSettings = gcnew TitledCheckBoxTitledList(this);
			cipherSettings->TitleText = "Cipher Settings:";
			cipherSettings->TitleFont = keyNumbers[0]->ControlFont;
			cipherSettings->TitleForeColor = keyNumbers->TitleForeColor;
			cipherSettings->IsTitleVertical = true;
			cipherSettings->IsListVertical = true;
			cipherSettings->Offset = Point(20, 5);
			cipherSettings->Spacing = Point(0, 0);
			
			cipherSettings->addCheckBox("settingsOAEP", "Use OAEP scheme:", true);
			cipherSettings->addCheckBox("settingsCompress", "Use data compression:", true);
			cipherSettings->addCheckBox("settingsCRT", "When decrypting, use CRT mode:", true);
			cipherSettings->addCheckBox("settingsBlind", "When decrypting, use blinding:", true);
			cipherSettings->BorderColor = ColorFactory::Scale(Color::DeepSkyBlue, 0.85f);
			
			for(Int32 i = 0; i < cipherSettings->Count; ++i) {
				cipherSettings[i]->TitleFont = cipherSettings->TitleFont;
				cipherSettings[i]->TitleForeColor = cipherSettings->TitleForeColor;
				cipherSettings[i]->ControlForeColor = Color::LimeGreen;
				cipherSettings[i]->ControlBackColor = Color::Azure;
				cipherSettings[i]->ControlSize = System::Drawing::Size(12, 12);
				cipherSettings[i]->OffsetX = 15;
				cipherSettings[i]->OffsetY += cipherSettings[i]->MiddleAlignmentY + 2;
			}

			cipherSettings->performLayout();
			cipherSettings->performHorizontalAlignment();
			cipherSettings->Top = passwordButtons->Bottom + 15;
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
			encrypt->CentreX = keyNumbers->CentreX;
			encrypt->Top = cipherSettings->Bottom + 15;
		}

		void _initializeDecryptButton()
		{
			decrypt = gcnew LabelButton(this, "decrypt", "DECRYPT FILE");
			
			LabelButtonScheme::Invert(decrypt, false, ColorFactory::Scale(Color::ForestGreen, 1.1f), Color::Honeydew, 1.1f);
			decrypt->BorderColor = decrypt->BackColorNormal;
			decrypt->AutoSize = false;
			decrypt->Font = keyNumbers->TitleFont;
			decrypt->Size = System::Drawing::Size(180, 25);
			
			decrypt->CentreX = keyNumbers->CentreX;
			decrypt->Top = encrypt->Bottom + 12;
		}

		void _initializeProgressAndOutcome()
		{
			informant = gcnew TitledLabel(this, "informant", "");
			informant->ControlAutoSize = true;
			informant->TitleFont = keyNumbers[0]->TitleFont;
			informant->ControlFont = keyNumbers[0]->TitleFont;
			informant->TitleForeColor = keyNumbers[0]->TitleForeColor;
			informant->ControlForeColor = keyNumbers[0]->TitleForeColor;
			informant->Offset = Point(3, 0);
			informant->Left = keyNumbers->Left;
			informant->Top = decrypt->Bottom + 5;

			progressBar = gcnew SingleProgressBar(this, "progressBar");
			progressBar->BorderThickness = 2;
			progressBar->Height = 21;
			progressBar->BorderColor = keyButtons[0]->BackColorNormal;
			progressBar->BackColor = Color::Azure;
			progressBar->ForeColor = keyButtons[0]->BackColorNormal;
			progressBar->Top = informant->Bottom + 5;
			progressBar->Left = keyNumbers->Left;

			saveOutcome = gcnew LabelButton(this, "saveOutcome", "SAVE OUTCOME TO FILE");
			LabelButtonScheme::Invert(saveOutcome, false, ColorFactory::Scale(Color::DeepSkyBlue, 0.85f), Color::Azure, 1.1f);
			saveOutcome->AutoSize = false;
			saveOutcome->BorderColor = saveOutcome->BackColorNormal;
			saveOutcome->Font = menuButtons[0]->Font;
			saveOutcome->Size = System::Drawing::Size(200, 21);
			saveOutcome->Left = keyNumbers->Left;
			saveOutcome->Top = progressBar->Top;

			cancel = gcnew LabelButton(this, "cancel", "CANCEL");
			LabelButtonScheme::Invert(cancel, false, ColorFactory::Scale(Color::DeepSkyBlue, 0.85f), Color::Azure, 1.1f);
			cancel->AutoSize = false;
			cancel->BorderColor = cancel->BackColorNormal;
			cancel->Font = menuButtons[0]->Font;
			cancel->Size = System::Drawing::Size(120, 21);
			cancel->Bottom = progressBar->Top + 1;
		}

		void _initializeMovableControls()
		{
			movableControls = gcnew DynamicControlCollection();
			movableControls->add(passwordButtons);
			movableControls->add(cipherSettings);
			movableControls->add(encrypt);
			movableControls->add(decrypt);
			movableControls->add(informant);
			movableControls->add(cancel);
			movableControls->add(progressBar);
			movableControls->add(saveOutcome);
			movableControls->updateBounds();
			movableControls->Visible = false;
		}

		void _initializeEvents()
		{
			menuButtons[0]->addMouseDownEvent(gcnew MouseEventHandler(this, &MainForm::Event_SaveKey));
			menuButtons[1]->addMouseDownEvent(gcnew MouseEventHandler(this, &MainForm::Event_LoadKey));

			keyButtons[0]->addMouseDownEvent(gcnew MouseEventHandler(this, &MainForm::Event_ClearKey));
			keyButtons[1]->addMouseDownEvent(gcnew MouseEventHandler(this, &MainForm::Event_CheckKey));

			passwordButtons[0]->addMouseDownEvent(gcnew MouseEventHandler(this, &MainForm::Event_ShowHidePrivate));
			passwordButtons[1]->addMouseDownEvent(gcnew MouseEventHandler(this, &MainForm::Event_RemovePswrd));
			passwordButtons[2]->addMouseDownEvent(gcnew MouseEventHandler(this, &MainForm::Event_SetPswrd));

			cipherSettings[0]->addMouseDownEvent(gcnew MouseEventHandler(this, &MainForm::Event_OaepSetting));
			cipherSettings[2]->addMouseDownEvent(gcnew MouseEventHandler(this, &MainForm::Event_CrtSetting));
			cipherSettings[3]->addMouseDownEvent(gcnew MouseEventHandler(this, &MainForm::Event_BlindingSetting));

			encrypt[0]->addMouseDownEvent(gcnew MouseEventHandler(this, &MainForm::Event_EncryptFile));
			encrypt[1]->addMouseDownEvent(gcnew MouseEventHandler(this, &MainForm::Event_EncryptMsg));
			decrypt->addMouseDownEvent(gcnew MouseEventHandler(this, &MainForm::Event_Decrypt));

			saveOutcome->addMouseDownEvent(gcnew MouseEventHandler(this, &MainForm::Event_SaveOutcome));
			cancel->addMouseDownEvent(gcnew MouseEventHandler(this, &MainForm::Event_Cancel));
		}
#pragma endregion

#pragma region Windows Form Designer generated code
		void InitializeComponent()
		{
			this->SuspendLayout();
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(680, 353);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->BackColor = SimpleBox::BackColor;
			this->MaximizeBox = false;
			this->Name = L"MainForm";
			this->Text = L"RSA Sandbox";
			this->Icon = gcnew System::Drawing::Icon(".\\cvrsa.ico");
			this->ResumeLayout(false);
		}
#pragma endregion
	};
}
