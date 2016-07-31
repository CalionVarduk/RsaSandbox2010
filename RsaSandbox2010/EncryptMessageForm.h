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

	public ref class EncryptMessageForm : public Form
	{
		public:
			property String^ Message {
				public: String^ get() { return box->ControlText; }
			}

			EncryptMessageForm() : Form()
			{
				InitializeComponent();

				box = gcnew TitledTextBox(this, "msgBox", "Message to encrypt:");

				box->TextAlign = HorizontalAlignment::Left;
				box->WordWrap = false;
				box->Multiline = true;
				box->ScrollBars = ScrollBars::Both;

				box->TitleFont = gcnew System::Drawing::Font("Microsoft Sans Serif", 8);
				box->ControlFont = box->TitleFont;
				box->IsTitleVertical = true;
				box->Offset = Point(0, 5);
				box->ControlBackColor = Color::Azure;
				box->ControlSize = System::Drawing::Size(300, 200);
				box->Location = Point(12, 12);

				confirm = gcnew LabelButton(this, "confirmButton", "CONFIRM");
				LabelButtonScheme::Invert(confirm, true, Color::SteelBlue, Color::Azure, 1.1f);
				confirm->Font = SimpleBox::ButtonsFont;
				confirm->Location = Point(box->Left, box->Bottom + 12);

				confirm->addMouseDownEvent(gcnew MouseEventHandler(this, &EncryptMessageForm::Event_ConfirmButtonClicked));
				ClientSize = System::Drawing::Size(box->Right + 12, confirm->Bottom + 12);
				DialogResult = System::Windows::Forms::DialogResult::Abort;
			}

		private:
			TitledTextBox^ box;
			LabelButton^ confirm;
			System::ComponentModel::Container ^components;

			void Event_ConfirmButtonClicked(Object^ sender, MouseEventArgs^ e)
			{
				DialogResult = System::Windows::Forms::DialogResult::OK;
				Close();
			}

#pragma region Windows Form Designer generated code
			void InitializeComponent()
			{
				this->SuspendLayout();
				// 
				// EncryptMessageForm
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->ClientSize = System::Drawing::Size(300, 350);
				this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
				this->BackColor = SimpleBox::BackColor;
				this->MaximizeBox = false;
				this->Name = L"EncryptMessageForm";
				this->Text = L"Provide Message";
				this->TopMost = true;
				this->Icon = gcnew System::Drawing::Icon(".\\cvrsa.ico");
				this->ResumeLayout(false);
			}
#pragma endregion
	};
}