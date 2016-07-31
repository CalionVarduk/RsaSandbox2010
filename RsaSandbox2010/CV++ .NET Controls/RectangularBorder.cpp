#include "RectangularBorder.h"

using namespace CVNetControls;

RectangularBorder::RectangularBorder()
	: _pen(gcnew Pen(System::Drawing::Color::Black, 0)), _color(System::Drawing::Color::Black), _colorDisabled(System::Drawing::Color::DarkGray)
{}

RectangularBorder::RectangularBorder(System::Drawing::Color color)
	: _pen(gcnew Pen(System::Drawing::Color::Black, 1)), _color(color), _colorDisabled(System::Drawing::Color::DarkGray)
{}

RectangularBorder::RectangularBorder(System::Drawing::Color color, Int32 thickness)
	: _pen(gcnew Pen(System::Drawing::Color::Black, (float)((thickness > 0) ? thickness : 0))), _color(color), _colorDisabled(System::Drawing::Color::DarkGray)
{}

inline void RectangularBorder::linkToControl(Control^ control)
{ control->Paint += gcnew PaintEventHandler(this, &RectangularBorder::Event_OnPaint); }

inline void RectangularBorder::unlinkFromControl(Control^ control)
{ control->Paint -= gcnew PaintEventHandler(this, &RectangularBorder::Event_OnPaint); }

inline void RectangularBorder::setNone()
{ _pen->Width = 0; }

inline void RectangularBorder::setSimple()
{ if(IsNone) _pen->Width = 1; }

inline void RectangularBorder::setSimple(System::Drawing::Color color)
{
	setSimple();
	_color = color;
}

inline void RectangularBorder::setSimple(System::Drawing::Color color, Int32 thickness)
{
	_pen->Width = (float)((thickness > 0) ? thickness : 0);
	_color = color;
}

inline void RectangularBorder::Event_OnPaint(Object^ sender, PaintEventArgs^ e)
{
	if(!IsNone) {
		Control^ control = (Control^)sender;
		Int32 halfThickness = (Int32)_pen->Width >> 1;
		_pen->Color = (control->Enabled) ? _color : _colorDisabled;
		e->Graphics->DrawRectangle(_pen, halfThickness, halfThickness, control->Width - (Int32)_pen->Width, control->Height - (Int32)_pen->Width);
	}
}