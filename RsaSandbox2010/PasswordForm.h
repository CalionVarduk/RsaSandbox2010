#pragma once

#include "CVUtilities.h"
#include "CVNetControls.h"
#include "RSAKey.h"

namespace RsaSandbox
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Windows::Forms;
	using namespace System::Text;
	using namespace System::Drawing;

	using namespace CVNetControls;

	public ref class PasswordForm
	{
		public:
			property bool IsShown {
				public: bool get() { return form->Visible; }
			}

			property Int32 MinLength {
				public: Int32 get() { return minLength; }
				public: void set(Int32 value) { minLength = value; }
			}

			PasswordForm()
			{
				form = gcnew Form();
				form->BackColor = SimpleBox::BackColor;
				form->FormBorderStyle = FormBorderStyle::FixedSingle;
				form->MaximizeBox = false;
				form->Text = L"Password Form";
				form->TopMost = true;
				form->Icon = gcnew System::Drawing::Icon(".\\cvrsa.ico");
				minLength = 8;

				TitledTextBoxList^ pswrdBoxes = gcnew TitledTextBoxList(form);

				pswrdBoxes->Spacing = Point(0, 5);

				pswrdBoxes->addTextBox("oldPswrdBox", "Old Password:", "");
				pswrdBoxes->addTextBox("newPswrdBox", "New Password:", "");
				pswrdBoxes->addTextBox("repeatPswrdBox", "Repeat Password:", "");

				pswrdBoxes->TextAlign = HorizontalAlignment::Left;
				pswrdBoxes->MaxLength = 64;
				System::Drawing::Font^ font = gcnew System::Drawing::Font("Microsoft Sans Serif", 8);

				for(Int32 i = 0; i < pswrdBoxes->Count; ++i) {
					pswrdBoxes[i]->Offset = Point(5, -1);
					pswrdBoxes[i]->ControlFont = font;
					pswrdBoxes[i]->ControlSize = System::Drawing::Size(270, 20);
					pswrdBoxes[i]->ControlBackColor = Color::Azure;
					pswrdBoxes[i]->toTextBox()->PasswordChar = '*';
				}

				pswrdBoxes->performLayout();
				pswrdBoxes->performHorizontalAlignment();

				LabelButton^ pswrdConfirm = gcnew LabelButton(form, "pswrdConfirm", "CONFIRM");
				LabelButtonScheme::Invert(pswrdConfirm, true, Color::SteelBlue, Color::Azure, 1.1f);
				pswrdConfirm->Font = SimpleBox::ButtonsFont;
				pswrdConfirm->Location = Point(pswrdBoxes[2]->ControlLeft, pswrdBoxes->Bottom + 10);

				controls = gcnew DynamicControlCollection();
				controls->add(pswrdBoxes[0]);
				controls->add(pswrdBoxes[1]);
				controls->add(pswrdBoxes[2]);
				controls->add(pswrdConfirm);
				controls->updateBounds();

				pswrdConfirm->addMouseDownEvent(gcnew MouseEventHandler(this, &PasswordForm::Event_PswrdButtonClicked));
				form->FormClosing += gcnew FormClosingEventHandler(this, &PasswordForm::Event_FormClosing);
			}

			bool setPassword(RSAKey^ key)
			{
				setClientSize(key->IsPasswordProtected);
				TitledTextBox^ oldPswrdBox = controls->interpretControlAs<TitledTextBox>(0);
				TitledTextBox^ newPswrdBox = controls->interpretControlAs<TitledTextBox>(1);
				TitledTextBox^ repeatPswrdBox = controls->interpretControlAs<TitledTextBox>(2);
				LabelButton^ pswrdConfirm = controls->interpretControlAs<LabelButton>(3);
				bool askForPassword = true;

				while(askForPassword) {
					pswrdConfirm->deactivate();
					form->ShowDialog();

					if(!pswrdConfirm->Activated) askForPassword = false;
					else if(newPswrdBox->ControlText->Equals(repeatPswrdBox->ControlText)) {
						if(newPswrdBox->ControlText->Length < minLength)
							SimpleBox::display("Password must be at least " + minLength.ToString() + " characters long.", "Password Error");
						else try {
							if(key->IsPasswordProtected) key->setPassword(oldPswrdBox->ControlText, newPswrdBox->ControlText);
							else key->setPassword(newPswrdBox->ControlText);
							askForPassword = false;
						}
						catch(Exception^ exc) { SimpleBox::display(exc->Message, "Password Error"); }
					}
					else SimpleBox::display("The password must be repeated correctly.", "Password Error");

					oldPswrdBox->ControlText = "";
					newPswrdBox->ControlText = "";
					repeatPswrdBox->ControlText = "";
				}
				return getOutcome(pswrdConfirm);
			}

			bool removePassword(RSAKey^ key)
			{
				setClientSize();
				TitledTextBox^ oldPswrdBox = controls->interpretControlAs<TitledTextBox>(0);
				LabelButton^ pswrdConfirm = controls->interpretControlAs<LabelButton>(3);
				bool askForPassword = true;

				while(askForPassword) {
					pswrdConfirm->deactivate();
					form->ShowDialog();

					if(!pswrdConfirm->Activated) askForPassword = false;
					else try {
						key->removePassword(oldPswrdBox->ControlText);
						askForPassword = false;
					}
					catch(Exception^ exc) { SimpleBox::display(exc->Message, "Password Error"); }

					oldPswrdBox->ControlText = "";
				}
				return getOutcome(pswrdConfirm);
			}

			bool getKeyPrivateComponents(RSAKey^ key, BigInteger% d, BigInteger% dp, BigInteger% dq)
			{
				setClientSize();
				TitledTextBox^ oldPswrdBox = controls->interpretControlAs<TitledTextBox>(0);
				LabelButton^ pswrdConfirm = controls->interpretControlAs<LabelButton>(3);
				bool askForPassword = true;

				while(askForPassword) {
					pswrdConfirm->deactivate();
					form->ShowDialog();

					if(!pswrdConfirm->Activated) askForPassword = false;
					else try {
						key->getPrivateExponents(d, dp, dq, oldPswrdBox->ControlText);
						askForPassword = false;
					}
					catch(Exception^ exc) { SimpleBox::display(exc->Message, "Password Error"); }

					oldPswrdBox->ControlText = "";
				}
				return getOutcome(pswrdConfirm);
			}

			String^ getPassword(RSAKey^ key)
			{
				setClientSize();
				String^ password = nullptr;
				TitledTextBox^ oldPswrdBox = controls->interpretControlAs<TitledTextBox>(0);
				LabelButton^ pswrdConfirm = controls->interpretControlAs<LabelButton>(3);
				bool askForPassword = true;

				while(askForPassword) {
					pswrdConfirm->deactivate();
					form->ShowDialog();

					if(!pswrdConfirm->Activated) askForPassword = false;
					else if(key->authenticate(oldPswrdBox->ControlText)) {
						password = oldPswrdBox->ControlText;
						askForPassword = false;
					}
					else SimpleBox::display("Wrong password provided.", "Password Error");

					oldPswrdBox->ControlText = "";
				}
				return password;
			}

		private:
			Form^ form;
			DynamicControlCollection^ controls;
			Int32 minLength;
			System::ComponentModel::Container ^components;

			void setClientSize(bool isKeyProtected)
			{
				controls->interpretControlAs<TitledTextBox>(0)->TitleText = "Old Password:";

				if(isKeyProtected) {
					controls[0]->Visible = true;
					controls->Location = Point(12, 12);
				}
				else {
					controls[0]->Visible = false;
					controls->Location = Point(12, 12 - controls[1]->Top + controls[0]->Top);
				}
				controls[1]->Visible = true;
				controls[2]->Visible = true;
				controls[3]->Top = controls[2]->Bottom + 10;

				form->ClientSize = Size(controls->Right + 12, controls->Bottom + 12);
			}

			void setClientSize()
			{
				controls->interpretControlAs<TitledTextBox>(0)->TitleText = "Password:";
				controls[0]->Visible = true;
				controls[1]->Visible = false;
				controls[2]->Visible = false;
				controls->Location = Point(12, 12);
				controls[3]->Top = controls[0]->Bottom + 10;
				form->ClientSize = Size(controls->Right + 12, controls[3]->Bottom + 12);
			}

			bool getOutcome(LabelButton^ confirm)
			{
				if(confirm->Activated) {
					confirm->deactivate();
					return true;
				}
				return false;
			}

			void Event_PswrdButtonClicked(Object^ sender, MouseEventArgs^ e)
			{
				form->Close();
			}

			void Event_FormClosing(Object^ sender, FormClosingEventArgs^ e)
			{
				e->Cancel = true;
				form->Hide();
			}
	};
}