#pragma once

#include <type_traits>
#include "TitledControlBase.h"
#include "SingleControl.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::Drawing;
	using namespace System::Windows::Forms;

	template <class T>
	public ref class TitledControl : public TitledControlBase<T>
	{
		static_assert(std::is_base_of<Control, T>::value, "T must inherit from System::Windows::Forms::Control.");

		public:
		#pragma region Properties
			property System::Drawing::Size ControlSize {
				public: System::Drawing::Size get() { return _control->Size; }
				public: void set(System::Drawing::Size value) { _control->Size = value; }
			}

			property Int32 ControlWidth {
				public: Int32 get() { return _control->Width; }
				public: void set(Int32 value) { _control->Width = value; }
			}

			property Int32 ControlHeight {
				public: Int32 get() { return _control->Height; }
				public: void set(Int32 value) { _control->Height = value; }
			}

			virtual property Control^ Parent {
				public: Control^ get() override { return _title->Parent; }
				public: void set(Control^ value) override
						{
							if(_title->Parent != nullptr) {
								_title->Parent->Controls->Remove(_title);
								_control->Parent->Controls->Remove(_control);
							}
							if(value != nullptr) {
								value->Controls->Add(_title);
								value->Controls->Add(_control);
							}
						}
			}

			property String^ Name {
				public: String^ get() { return _title->Name->Substring(0, _title->Name->Length - 5); }
				public: void set(String^ value)
						{
							_title->Name = value + "Title";
							_control->Name = value + "Control";
						}
			}

			property String^ TitleName {
				public: String^ get() { return _title->Name; }
			}

			property String^ ControlName {
				public: String^ get() { return _control->Name; }
			}

			property System::Drawing::Size ControlPreferredSize {
				public: System::Drawing::Size get() { return _control->PreferredSize; }
			}

			property String^ ControlText {
				public: String^ get() { return _control->Text; }
				public: void set(String^ value) { _control->Text = value; }
			}

			property System::Drawing::Font^ ControlFont {
				public: System::Drawing::Font^ get() { return _control->Font; }
				public: void set(System::Drawing::Font^ value) { _control->Font = value; }
			}

			property Color ControlBackColor {
				public: Color get() { return _control->BackColor; }
				public: void set(Color value) { _control->BackColor = value; }
			}

			property Color ControlForeColor {
				public: Color get() { return _control->ForeColor; }
				public: void set(Color value) { _control->ForeColor = value; }
			}

			property bool ControlAutoSize {
				public: bool get() { return _control->AutoSize; }
				public: void set(bool value) { _control->AutoSize = value; }
			}

			property bool HasChildren {
				public: bool get() { return _control->HasChildren; }
			}

			property System::Windows::Forms::Cursor^ Cursor {
				public: System::Windows::Forms::Cursor^ get() { return _control->Cursor; }
				public: void set(System::Windows::Forms::Cursor^ value) { _control->Cursor = value; }
			}

			virtual property bool IsList {
				public: bool get() override { return false; }
			}
		#pragma endregion

			TitledControl() : TitledControlBase<T>() {}

			void setWinControl(T^ control)
			{ _control = control; }

			virtual void refresh() override
			{
				_title->Refresh();
				_control->Refresh();
			}

		protected:
			virtual Control^ _returnControl() override { return _control; }
	};
}