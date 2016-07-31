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

	public ref class HastadExponentForm : public Form
	{
		public:
			property Int32 PublicExponent {
				public: Int32 get() { return Int32::Parse(boxes[0]->ControlText); }
			}

			property Int32 ModulusBitLength {
				public: Int32 get() { return Int32::Parse(boxes[1]->ControlText); }
			}

			static property Int32 MaxPublicExponent {
				public: Int32 get() { return 65537; }
			}

			HastadExponentForm() : Form()
			{
				InitializeComponent();

				boxes = gcnew TitledTextBoxList(this);

				boxes->addTextBox("expBox", "Public exponent:", "");
				boxes->addTextBox("bitBox", "Modulus' bit length:", "");

				boxes->IsListVertical = true;
				boxes->Spacing = Point(0, -1);
				boxes->TextAlign = HorizontalAlignment::Left;
				boxes[0]->MaxLength = 5;
				boxes[1]->MaxLength = 4;

				System::Drawing::Font^ font = gcnew System::Drawing::Font("Microsoft Sans Serif", 10.5f);
				for(Int32 i = 0; i < boxes->Count; ++i) {
					boxes[i]->TitleFont = font;
					boxes[i]->ControlFont = font;
					boxes[i]->IsTitleVertical = false;
					boxes[i]->Offset = Point(5, -1);
					boxes[i]->ControlBackColor = Color::Azure;
					boxes[i]->ControlSize = System::Drawing::Size(60, 21);
				}

				boxes->performLayout();
				boxes->performHorizontalAlignment();
				boxes->Location = Point(12, 12);

				confirm = gcnew LabelButton(this, "confirmButton", "CONFIRM");
				LabelButtonScheme::Invert(confirm, false, Color::SteelBlue, Color::Azure, 1.1f);
				confirm->Font = SimpleBox::ButtonsFont;
				confirm->Location = Point(boxes->Left, boxes->Bottom + 5);

				confirm->addMouseDownEvent(gcnew MouseEventHandler(this, &HastadExponentForm::Event_ConfirmButtonClicked));
				ClientSize = System::Drawing::Size(boxes->Right + 12, confirm->Bottom + 12);
				DialogResult = System::Windows::Forms::DialogResult::Abort;
			}

		private:
			TitledTextBoxList^ boxes;
			LabelButton^ confirm;
			System::ComponentModel::Container ^components;

			void Event_ConfirmButtonClicked(Object^ sender, MouseEventArgs^ e)
			{
				try {
					Int32 exp = PublicExponent;
					if(exp < 3 || exp > MaxPublicExponent || (exp & 1) == 0) throw gcnew Exception();

					Int32 bits = ModulusBitLength;
					if(bits < 128) throw gcnew Exception();

					DialogResult = System::Windows::Forms::DialogResult::OK;
					Close();
				}
				catch(Exception^) {
					SimpleBox::display("Public Exponent must be a natural odd number greater than 2\r\n" +
										"and less than or equal to " + MaxPublicExponent.ToString("N0") + "\r\n" +
										"and Modulus' Bit Length must be a natural number greater than 127.", "Parsing Error");
				}
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
				this->Name = L"HastadExponentForm";
				this->Text = L"Provide Public Exponent";
				this->TopMost = true;
				this->Icon = gcnew System::Drawing::Icon(".\\cvrsa.ico");
				this->ResumeLayout(false);
			}
#pragma endregion
	};
}