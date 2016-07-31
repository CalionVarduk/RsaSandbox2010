#pragma once

#include "SingleControl.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;

	public ref class SingleCheckBox : public SingleControl<PictureBox>
	{
		public:
		#pragma region Properties
			property bool Checked {
				public: bool get() { return _checked; }
				public: void set(bool value)
						 {
							 _checked = value;
							 _control->Refresh();
						 }
			}

			property Int32 CheckOffset {
				public: Int32 get() { return _checkOffset; }
				public: void set(Int32 value)
						 {
							 _checkOffset = (value > 0) ? value : 0;
							 _control->Refresh();
						 }
			}

			property RectangularBorder^ BorderStyle {
				public: RectangularBorder^ get() { return _border; }
				public: void set(RectangularBorder^ value)
						{
							_border->unlinkFromControl(_control);
							_border = (value != nullptr) ? value : gcnew RectangularBorder();
							_border->linkToControl(_control);
							_control->Refresh();
						}
			}

			property Int32 BorderThickness {
				public: Int32 get() { return _border->Thickness; }
				public: void set(Int32 value)
						{
							_border->Thickness = value;
							_control->Refresh();
						}
			}

			property Color BorderColor {
				public: Color get() { return _border->Color; }
				public: void set(Color value)
						{
							_border->Color = value;
							if(Enabled) _control->Refresh();
						}
			}

			property Color BorderColorDisabled {
				public: Color get() { return _border->ColorDisabled; }
				public: void set(Color value)
						{
							_border->ColorDisabled = value;
							if(!Enabled) _control->Refresh();
						}
			}
		#pragma endregion

			SingleCheckBox() : SingleControl<PictureBox>()
			{ _initialize(false); }
			
			SingleCheckBox(Control^ parent) : SingleControl<PictureBox>()
			{
				Parent = parent;
				_initialize(false);
			}

			SingleCheckBox(Control^ parent, String^ name, bool checked) : SingleControl<PictureBox>()
			{
				Name = name;
				Parent = parent;
				_initialize(checked);
			}

			void addMouseDownEvent(MouseEventHandler^ handler)
			{ _control->MouseDown += handler; }

			PictureBox^ toPictureBox()
			{ return _control; }

		private:
			Int32 _checkOffset;
			bool _checked;
			SolidBrush^ _brush;
			RectangularBorder^ _border;

			void Event_MouseClick(Object^ sender, MouseEventArgs^ e)
			{ Checked = !_checked; }

			void Event_OnPaint(Object^ sender, PaintEventArgs^ e)
			{
				Int32 offset = _border->Thickness + _checkOffset;
				Int32 offset2 = offset << 1;
				Rectangle rect = Rectangle(offset, offset, _control->Width - offset2, _control->Height - offset2);

				_brush->Color = (_checked) ? _control->ForeColor : _control->BackColor;
				e->Graphics->FillRectangle(_brush, rect);
			}

			void Event_BackColorChanged(Object^ sender, EventArgs^ e)
			{ _control->Refresh(); }

			void Event_ForeColorChanged(Object^ sender, EventArgs^ e)
			{ _control->Refresh(); }

			void _initialize(bool checked)
			{
				_checked = checked;
				_checkOffset = 1;

				_control->Size = System::Drawing::Size(11, 11);
				_control->BackColor = Color::White;
				_control->ForeColor = Color::DodgerBlue;
				_control->BorderStyle = System::Windows::Forms::BorderStyle::None;
				_control->MouseDown += gcnew MouseEventHandler(this, &SingleCheckBox::Event_MouseClick);
				_control->Paint += gcnew PaintEventHandler(this, &SingleCheckBox::Event_OnPaint);
				_control->BackColorChanged += gcnew EventHandler(this, &SingleCheckBox::Event_BackColorChanged);
				_control->ForeColorChanged += gcnew EventHandler(this, &SingleCheckBox::Event_ForeColorChanged);

				_border = gcnew RectangularBorder(Color::Black, 1);
				_border->linkToControl(_control);

				_brush = gcnew SolidBrush(_control->ForeColor);
				_control->Refresh();

				Location = Point(0, 0);
			}
	};
}