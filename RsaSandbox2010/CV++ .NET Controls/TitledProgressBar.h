#pragma once

#include "TitledControl.h"
#include "DoubleBufferedPanel.h"
#include "NumericLimiter.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;

	public ref class TitledProgressBar : public TitledControl<DoubleBufferedPanel>
	{
		public:
#pragma region Properties
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

			property float ProgressPercentage {
				public: float get() { return _progressPercentage; }
				public: void set(float value)
						{
							_progressPercentage = NumericLimiter<float>::minMax(value, 0, 1);
							_control->Refresh();
						}
			}
#pragma endregion

			TitledProgressBar() : TitledControl<DoubleBufferedPanel>()
			{ _initialize(); }
			
			TitledProgressBar(Control^ parent) : TitledControl<DoubleBufferedPanel>()
			{
				Parent = parent;
				_initialize();
			}

			TitledProgressBar(Control^ parent, String^ name, String^ title) : TitledControl<DoubleBufferedPanel>()
			{
				Name = name;
				_title->Text = title;
				Parent = parent;
				_initialize();
			}

			Panel^ toPanel()
			{ return _control; }

		private:
			float _progressPercentage;
			SolidBrush^ _brush;
			RectangularBorder^ _border;

			void _initialize()
			{
				Offset = Point(5, 0);
				Location = Point(0, 0);

				_progressPercentage = 0;
				_brush = gcnew SolidBrush(_control->ForeColor);
				_border = gcnew RectangularBorder(Color::Black, 1);
				_border->linkToControl(_control);

				_control->Paint += gcnew PaintEventHandler(this, &TitledProgressBar::Event_OnPaint);
			}

			void Event_OnPaint(Object^ sender, PaintEventArgs^ e)
			{
				Int32 width = _control->Width - (_border->Thickness << 1);
				Int32 height = _control->Height - (_border->Thickness << 1);
				Int32 barWidth = (Int32)(width * _progressPercentage + 0.5f);

				_brush->Color = _control->ForeColor;
				e->Graphics->FillRectangle(_brush, _border->Thickness, _border->Thickness, barWidth, height);
			}
	};
}