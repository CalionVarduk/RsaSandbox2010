#include "LabelButton.h"

using namespace CVNetControls;

LabelButton::LabelButton() : SingleControl<Label>()
{ _initialize(); }

LabelButton::LabelButton(Control^ parent) : SingleControl<Label>()
{
	_initialize();
	Parent = parent;
}

LabelButton::LabelButton(Control^ parent, String^ name, String^ text) : SingleControl<Label>()
{
	_initialize();
	Name = name;
	Text = text;
	Parent = parent;
}

inline void LabelButton::activate()
{
	if(Enabled && ActivatedOnClick) {
		_isActive = true;
		_activateColorChange();
	}
}

inline void LabelButton::deactivate()
{
	if(Enabled && ActivatedOnClick) {
		_isActive = false;
		_activateColorChange();
	}
}

inline void LabelButton::addMouseDownEvent(MouseEventHandler^ handler)
{ _control->MouseDown += handler; }

inline void LabelButton::addMouseUpEvent(MouseEventHandler^ handler)
{ _control->MouseUp += handler; }

inline void LabelButton::addMouseMoveEvent(MouseEventHandler^ handler)
{ _control->MouseMove += handler; }

inline void LabelButton::addMouseEnterEvent(EventHandler^ handler)
{ _control->MouseEnter += handler; }

inline void LabelButton::addMouseLeaveEvent(EventHandler^ handler)
{ _control->MouseLeave += handler; }

inline void LabelButton::addMouseHoverEvent(EventHandler^ handler)
{ _control->MouseHover += handler; }

inline Label^ LabelButton::toLabel()
{ return _control; }

inline void LabelButton::Event_MouseDown(Object^ sender, MouseEventArgs^ e)
{
	_mouseDown = true;

	if(ActivatedOnClick) {
		if(_onClickDeactivatable || !_isActive) {
			_isActive = !_isActive;
			_activateColorChange();
		}
	}
	else {
		_isActive = true;
		_activateColorChange();
	}
}

inline void LabelButton::Event_MouseUp(Object^ sender, MouseEventArgs^ e)
{
	_mouseDown = false;

	if(ActivatedOnHold) {
		_isActive = false;
		_activateColorChange();
	}
}

inline void LabelButton::Event_MouseMove(Object^ sender, MouseEventArgs^ e)
{
	_mouseMove = true;
	_mouseHover = false;
}

inline void LabelButton::Event_MouseEnter(Object^ sender, EventArgs^ e)
{
	_mouseWithinBounds = true;
	_mouseEnterColorChange();
}

inline void LabelButton::Event_MouseLeave(Object^ sender, EventArgs^ e)
{
	_mouseWithinBounds = false;
	_mouseMove = false;
	_mouseHover = false;
	_mouseDown = false;

	if(ActivatedOnHold)
		_isActive = false;

	_mouseLeaveColorChange();
}

inline void LabelButton::Event_MouseHover(Object^ sender, EventArgs^ e)
{
	_mouseMove = false;
	_mouseHover = true;
}

inline void LabelButton::Event_EnabledChanged(Object^ sender, EventArgs^ e)
{
	if(Enabled) {
		BackColor = (_isActive) ? BackColorActive : BackColorNormal;
		ForeColor = (_isActive) ? ForeColorActive : ForeColorNormal;
	}
	else {
		_mouseDown = false;
		BackColor = BackColorDisabled;
		ForeColor = ForeColorDisabled;
	}
}

void LabelButton::_activateColorChange()
{
	if(_colorableActivation) {
		if(_isActive) {
			if(MousedOverAndColorable) {
				BackColor = BackColorActiveMouseOver;
				ForeColor = ForeColorActiveMouseOver;
			}
			else {
				BackColor = BackColorActive;
				ForeColor = ForeColorActive;
			}
		}
		else if(MousedOverAndColorable) {
			BackColor = BackColorMouseOver;
			ForeColor = ForeColorMouseOver;
		}
		else {
			BackColor = BackColorNormal;
			ForeColor = ForeColorNormal;
		}
	}
}

void LabelButton::_mouseEnterColorChange()
{
	if(_colorableMouseOver) {
		if(ActivatedAndColorable) {
			BackColor = BackColorActiveMouseOver;
			ForeColor = ForeColorActiveMouseOver;
		}
		else {
			BackColor = BackColorMouseOver;
			ForeColor = ForeColorMouseOver;
		}
	}
}

void LabelButton::_mouseLeaveColorChange()
{
	if(_colorableMouseOver) {
		if(ActivatedAndColorable) {
			BackColor = BackColorActive;
			ForeColor = ForeColorActive;
		}
		else {
			BackColor = BackColorNormal;
			ForeColor = ForeColorNormal;
		}
	}
}

void LabelButton::_initialize()
{
	_mouseDown = false;
	_mouseMove = false;
	_mouseWithinBounds = false;
	_mouseHover = false;
	_isActive = false;
	_activeOnClick = true;
	_colorableMouseOver = true;
	_colorableActivation = true;
	_onClickDeactivatable = true;

	Font = gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, FontStyle::Regular, GraphicsUnit::Point, (Byte)238);
	_border = gcnew RectangularBorder(Color::Black, 1);
	_border->linkToControl(_control);
	TextAlign = System::Drawing::ContentAlignment::MiddleCenter;

	_initializeColors();
	_initializeEvents();
}

void LabelButton::_initializeColors()
{
	BackColor = Color::White;
	ForeColor = Color::Black;
	_backColorNormal = _backColorMouseOver = _backColorActive = _backColorActiveMouseOver = Color::White;
	_foreColorNormal = _foreColorMouseOver = _foreColorActive = _foreColorActiveMouseOver = Color::Black;
	_backColorDisabled = Color::LightGray;
	_foreColorDisabled = Color::DarkGray;
}

void LabelButton::_initializeEvents()
{
	_control->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &LabelButton::Event_MouseDown);
	_control->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &LabelButton::Event_MouseUp);
	_control->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &LabelButton::Event_MouseMove);
	_control->MouseEnter += gcnew System::EventHandler(this, &LabelButton::Event_MouseEnter);
	_control->MouseLeave += gcnew System::EventHandler(this, &LabelButton::Event_MouseLeave);
	_control->MouseHover += gcnew System::EventHandler(this, &LabelButton::Event_MouseHover);
	_control->EnabledChanged += gcnew System::EventHandler(this, &LabelButton::Event_EnabledChanged);
}