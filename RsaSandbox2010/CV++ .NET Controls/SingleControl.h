#pragma once

#include <type_traits>
#include "DynamicControl.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::Drawing;
	using namespace System::Windows::Forms;

	template <class T>
	public ref class SingleControl : public DynamicControl
	{
		static_assert(std::is_base_of<Control, T>::value, "T must inherit from System::Windows::Forms::Control.");

		public:
		#pragma region Properties
			virtual property bool Enabled {
				public: bool get() override { return _control->Enabled; }
				public: void set(bool value) override { _control->Enabled = value; }
			}

			virtual property bool Visible {
				public: bool get() override { return _control->Visible; }
				public: void set(bool value) override { _control->Visible = value; }
			}

			virtual property System::Drawing::Size Size {
				public: System::Drawing::Size get() override { return _control->Size; }
				public: void set(System::Drawing::Size value) { _control->Size = value; }
			}

			property System::Drawing::Size PreferredSize {
				public: System::Drawing::Size get() { return _control->PreferredSize; }
			}

			virtual property Int32 Width {
				public: Int32 get() override { return _control->Width; }
				public: void set(Int32 value) { _control->Width = value; }
			}

			virtual property Int32 Height {
				public: Int32 get() override { return _control->Height; }
				public: void set(Int32 value) { _control->Height = value; }
			}

			virtual property Point Location {
				public: Point get() override { return _control->Location; }
				public: void set(Point value) override { _control->Location = value; }
			}

			virtual property Int32 Left {
				public: Int32 get() override { return _control->Left; }
				public: void set(Int32 value) override { _control->Left = value; }
			}

			virtual property Int32 Right {
				public: Int32 get() override { return _control->Right; }
				public: void set(Int32 value) override { _control->Left = value - Width; }
			}

			virtual property Int32 CentreX {
				public: Int32 get() override { return _control->Left + (Width >> 1); }
				public: void set(Int32 value) override { _control->Left = value - (Width >> 1); }
			}

			virtual property Int32 Top {
				public: Int32 get() override { return _control->Top; }
				public: void set(Int32 value) override { _control->Top = value; }
			}

			virtual property Int32 Bottom {
				public: Int32 get() override { return _control->Bottom; }
				public: void set(Int32 value) override { _control->Top = value - Height; }
			}

			virtual property Int32 CentreY {
				public: Int32 get() override { return _control->Top + (Height >> 1); }
				public: void set(Int32 value) override { _control->Top = value - (Height >> 1); }
			}

			property String^ Name {
				public: String^ get() { return _control->Name; }
				public: void set(String^ value) { _control->Name = value; }
			}

			property String^ Text {
				public: String^ get() { return _control->Text; }
				public: void set(String^ value) { _control->Text = value; }
			}

			property System::Drawing::Font^ Font {
				public: System::Drawing::Font^ get() { return _control->Font; }
				public: void set(System::Drawing::Font^ value) { _control->Font = value; }
			}

			virtual property Control^ Parent {
				public: Control^ get() override { return _control->Parent; }
				public: void set(Control^ value) override
						{
							if(_control->Parent != nullptr)
								_control->Parent->Controls->Remove(_control);
							if(value != nullptr)
								value->Controls->Add(_control);
						}
			}

			property IntPtr Handle {
				public: IntPtr get() { return _control->Handle; }
			}

			property bool IsHandleCreated {
				public: bool get() { return _control->IsHandleCreated; }
			}

			property bool HasChildren {
				public: bool get() { return _control->HasChildren; }
			}

			property bool AutoSize {
				public: bool get() { return _control->AutoSize; }
				public: void set(bool value) { _control->AutoSize = value; }
			}

			property System::Windows::Forms::Cursor^ Cursor {
				public: System::Windows::Forms::Cursor^ get() { return _control->Cursor; }
				public: void set(System::Windows::Forms::Cursor^ value) { _control->Cursor = value; }
			}

			property Color BackColor {
				public: Color get() { return _control->BackColor; }
				public: void set(Color value) { _control->BackColor = value; }
			}

			property Color ForeColor {
				public: Color get() { return _control->ForeColor; }
				public: void set(Color value) { _control->ForeColor = value; }
			}

			virtual property bool IsTitled {
				public: bool get() override { return false; }
			}

			virtual property bool IsList {
				public: bool get() override { return false; }
			}
		#pragma endregion

			SingleControl() : _control(gcnew T()) {}

			virtual void refresh() override
			{ _control->Refresh(); }

			void setWinControl(T^ control)
			{ _control = control; }

		protected:
			T^ _control;
			virtual Control^ _returnControl() override { return (Control^)_control; }
	};
}