#pragma once

#include "TitledControl.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;

	public ref class TitledTextBox : public TitledControl<TextBox>
	{
		public:
		#pragma region Properties
			property HorizontalAlignment TextAlign {
				public: HorizontalAlignment get() { return _control->TextAlign; }
				public: void set(HorizontalAlignment value) { _control->TextAlign = value; }
			}

			property System::Windows::Forms::BorderStyle BorderStyle {
				public: System::Windows::Forms::BorderStyle get() { return _control->BorderStyle; }
				public: void set(System::Windows::Forms::BorderStyle value) { _control->BorderStyle = value; }
			}

			property Int32 MaxLength {
				public: Int32 get() { return _control->MaxLength; }
				public: void set(Int32 value) { _control->MaxLength = value; }
			}

			property bool ReadOnly {
				public: bool get() { return _control->ReadOnly; }
				public: void set(bool value) { _control->ReadOnly = value; }
			}

			property bool Multiline {
				public: bool get() { return _control->Multiline; }
				public: void set(bool value) { _control->Multiline = value; }
			}

			property bool WordWrap {
				public: bool get() { return _control->WordWrap; }
				public: void set(bool value) { _control->WordWrap = value; }
			}

			property System::Windows::Forms::ScrollBars ScrollBars {
				public: System::Windows::Forms::ScrollBars get() { return _control->ScrollBars; }
				public: void set(System::Windows::Forms::ScrollBars value) { _control->ScrollBars = value; }
			}
		#pragma endregion

			TitledTextBox() : TitledControl<TextBox>()
			{ _initialize(); }
			
			TitledTextBox(Control^ parent) : TitledControl<TextBox>()
			{
				Parent = parent;
				_initialize();
			}

			TitledTextBox(Control^ parent, String^ name, String^ title) : TitledControl<TextBox>()
			{
				Name = name;
				_title->Text = title;
				Parent = parent;
				_initialize();
			}

			TitledTextBox(Control^ parent, String^ name, String^ title, String^ text) : TitledControl<TextBox>()
			{
				Name = name;
				_title->Text = title;
				_control->Text = text;
				Parent = parent;
				_initialize();
			}

			TextBox^ toTextBox()
			{ return _control; }

		private:
			void _initialize()
			{
				_control->BackColor = SystemColors::InactiveCaption;
				_control->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
				_control->Font = gcnew Font("Microsoft Sans Serif", 9);
				_control->TextAlign = HorizontalAlignment::Right;
				Offset = Point(5, 0);
				Location = Point(0, 0);
			}
	};
}