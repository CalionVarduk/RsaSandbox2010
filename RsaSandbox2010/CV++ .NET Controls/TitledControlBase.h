#pragma once

#include "DynamicControl.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::Drawing;
	using namespace System::Windows::Forms;

	template <class T>
	public ref class TitledControlBase abstract : public DynamicControl
	{
		public:
		#pragma region Properties
			property Label^ Title {
				public: Label^ get() { return _title; }
				public: void set(Label^ value)
						{ if(value != nullptr) _title = value; }
			}

			virtual property bool Enabled {
				public: bool get() override { return _title->Enabled; }
				public: void set(bool value) override
						{
							_title->Enabled = value;
							_control->Enabled = value;
						}
			}

			virtual property bool Visible {
				public: bool get() override { return _title->Visible; }
				public: void set(bool value) override
						{
							_title->Visible = value;
							_control->Visible = value;
						}
			}

			virtual property System::Drawing::Size Size {
				public: System::Drawing::Size get() override { return System::Drawing::Size(Width, Height); }
			}

			virtual property Int32 Width {
				public: Int32 get() override { return Right - Left; }
			}

			virtual property Int32 Height {
				public: Int32 get() override { return Bottom - Top; }
			}

			virtual property Point Location {
				public: Point get() override { return Point(Left, Top); }
				public: void set(Point value) override
						{
							Left = value.X;
							Top = value.Y;
						}
			}

			virtual property Int32 Left {
				public: Int32 get() override { return (_title->Left < _control->Left) ? _title->Left : _control->Left; }
				public: void set(Int32 value) override
						{
							Int32 offset = value - Left;
							_title->Left += offset;
							_control->Left += offset;
						}
			}

			virtual property Int32 Right {
				public: Int32 get() override { return (_title->Right > _control->Right) ? _title->Right : _control->Right; }
				public: void set(Int32 value) override { Left = value - Width; }
			}

			virtual property Int32 CentreX {
				public: Int32 get() override { return Left + (Width >> 1); }
				public: void set(Int32 value) override { Left = value - (Width >> 1); }
			}

			virtual property Int32 Top {
				public: Int32 get() override { return (_title->Top < _control->Top) ? _title->Top : _control->Top; }
				public: void set(Int32 value) override
						{
							Int32 offset = value - Top;
							_title->Top += offset;
							_control->Top += offset;
						}
			}

			virtual property Int32 Bottom {
				public: Int32 get() override { return (_title->Bottom > _control->Bottom) ? _title->Bottom : _control->Bottom; }
				public: void set(Int32 value) override { Top = value - Height; }
			}

			virtual property Int32 CentreY {
				public: Int32 get() override { return Top + (Height >> 1); }
				public: void set(Int32 value) override { Top = value - (Height >> 1); }
			}

			property bool IsTitleHorizontal {
				public: bool get() { return _isTitleHorizontal; }
				public: void set(bool value)
						{
							if(_isTitleHorizontal != value) {
								Point offset = Offset;
								_isTitleHorizontal = value;
								Offset = offset;
							}
						}
			}

			property bool IsTitleVertical {
				public: bool get() { return !_isTitleHorizontal; }
				public: void set(bool value)
						{
							if(_isTitleHorizontal == value) {
								Point offset = Offset;
								_isTitleHorizontal = !value;
								Offset = offset;
							}
						}
			}

			property Int32 OffsetX {
				public: Int32 get() { return _control->Left - ((_isTitleHorizontal) ? _title->Right : _title->Left); }
				public: void set(Int32 value)
						{
							Int32 offset = value - OffsetX;
							_control->Left += offset;
						}
			}

			property Int32 OffsetY {
				public: Int32 get() { return _control->Top - ((_isTitleHorizontal) ? _title->Top : _title->Bottom); }
				public: void set(Int32 value)
						{
							Int32 offset = value - OffsetY;
							_control->Top += offset;
						}
			}

			property Point Offset {
				public: Point get() { return Point(OffsetX, OffsetY); }
				public: void set(Point value)
						{
							OffsetX = value.X;
							OffsetY = value.Y;
						}
			}

			property Int32 MiddleAlignmentX {
				public: Int32 get() { return TitleCentreX - ControlCentreX; }
			}

			property Int32 MiddleAlignmentY {
				public: Int32 get() { return TitleCentreY - ControlCentreY; }
			}

			property Point MiddleAlignment {
				public: Point get() { return Point(MiddleAlignmentX, MiddleAlignmentY); }
			}

			property System::Drawing::Size TitleSize {
				public: System::Drawing::Size get() { return _title->Size; }
			}

			property Int32 TitleWidth {
				public: Int32 get() { return _title->Width; }
			}

			property Int32 TitleHeight {
				public: Int32 get() { return _title->Height; }
			}

			property Point TitleLocation {
				public: Point get() { return _title->Location; }
			}

			property Int32 TitleLeft {
				public: Int32 get() { return _title->Left; }
			}

			property Int32 TitleRight {
				public: Int32 get() { return _title->Right; }
			}

			property Int32 TitleCentreX {
				public: Int32 get() { return _title->Left + (TitleWidth >> 1); }
			}

			property Int32 TitleTop {
				public: Int32 get() { return _title->Top; }
			}

			property Int32 TitleBottom {
				public: Int32 get() { return _title->Bottom; }
			}

			property Int32 TitleCentreY {
				public: Int32 get() { return _title->Top + (TitleHeight >> 1); }
			}

			property String^ TitleText {
				public: String^ get() { return _title->Text; }
				public: void set(String^ value)
						{
							_title->Text = value;
							_title->Size = _title->PreferredSize;
						}
			}

			property System::Drawing::Font^ TitleFont {
				public: System::Drawing::Font^ get() { return _title->Font; }
				public: void set(System::Drawing::Font^ value)
						{
							Point offset = Offset;
							_title->Font = value;
							Offset = offset;
						}
			}

			property System::Windows::Forms::Cursor^ TitleCursor {
				public: System::Windows::Forms::Cursor^ get() { return _title->Cursor; }
				public: void set(System::Windows::Forms::Cursor^ value) { _title->Cursor = value; }
			}

			property Color TitleBackColor {
				public: Color get() { return _title->BackColor; }
				public: void set(Color value) { _title->BackColor = value; }
			}

			property Color TitleForeColor {
				public: Color get() { return _title->ForeColor; }
				public: void set(Color value) { _title->ForeColor = value; }
			}

			property Point ControlLocation {
				public: Point get() { return _control->Location; }
			}

			property Int32 ControlLeft {
				public: Int32 get() { return _control->Left; }
			}

			property Int32 ControlRight {
				public: Int32 get() { return _control->Right; }
			}

			property Int32 ControlCentreX {
				public: Int32 get() { return _control->Left + (_control->Width >> 1); }
			}

			property Int32 ControlTop {
				public: Int32 get() { return _control->Top; }
			}

			property Int32 ControlBottom {
				public: Int32 get() { return _control->Bottom; }
			}

			property Int32 ControlCentreY {
				public: Int32 get() { return _control->Top + (_control->Height >> 1); }
			}

			property IntPtr Handle {
				public: IntPtr get() { return _title->Handle; }
			}

			property bool IsHandleCreated {
				public: bool get() { return _title->IsHandleCreated; }
			}

			virtual property bool IsTitled {
				public: bool get() override { return true; }
			}
		#pragma endregion

			TitledControlBase() : _title(gcnew Label()), _control(gcnew T()), _isTitleHorizontal(true)
			{
				_title->AutoSize = true;
				_title->BackColor = Color::Transparent;
				_title->Font = gcnew Font("Microsoft Sans Serif", 9);
			}

		protected:
			bool _isTitleHorizontal;
			Label^ _title;
			T^ _control;
	};
}