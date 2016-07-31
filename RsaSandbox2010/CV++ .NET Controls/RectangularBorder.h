#pragma once

namespace CVNetControls
{
	using namespace System;
	using namespace System::Drawing;
	using namespace System::Windows::Forms;

	public ref class RectangularBorder
	{
		public:
			property bool IsNone {
				public: bool get() { return (_pen->Width == 0); }
			}

			property bool IsSimple {
				public: bool get() { return (_pen->Width > 0); }
			}

			property Int32 Thickness {
				public: Int32 get() { return (Int32)_pen->Width; }
				public: void set(Int32 value) { if(IsSimple && value > 0) _pen->Width = (float)value; }
			}

			property System::Drawing::Color Color {
				public: System::Drawing::Color get() { return _color; }
				public: void set(System::Drawing::Color value) { _color = value; }
			}

			property System::Drawing::Color ColorDisabled {
				public: System::Drawing::Color get() { return _colorDisabled; }
				public: void set(System::Drawing::Color value) { _colorDisabled = value; }
			}

			RectangularBorder();
			RectangularBorder(System::Drawing::Color color);
			RectangularBorder(System::Drawing::Color color, Int32 thickness);

			void linkToControl(Control^ control);
			void unlinkFromControl(Control^ control);

			void setNone();
			void setSimple();
			void setSimple(System::Drawing::Color color);
			void setSimple(System::Drawing::Color color, Int32 thickness);

		private:
			Pen^ _pen;
			System::Drawing::Color _color;
			System::Drawing::Color _colorDisabled;

			void Event_OnPaint(Object^ sender, PaintEventArgs^ e);
	};
}