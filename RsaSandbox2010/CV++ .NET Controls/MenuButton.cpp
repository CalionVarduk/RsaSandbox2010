#include "MenuButton.h"

using namespace CVNetControls;

MenuButton::MenuButton() : LabelButton()
{ _initializeMenu(); }

MenuButton::MenuButton(Control^ parent) : LabelButton(parent)
{ _initializeMenu(); }

MenuButton::MenuButton(Control^ parent, String^ name, String^ text) : LabelButton(parent, name, text)
{ _initializeMenu(); }

void MenuButton::addButton(LabelButton^ button)
{
	Int32 marginRight = MenuMarginRight;
	Int32 marginBottom = MenuMarginBottom;

	_menuItems->add(button);
	_menuForm->ClientSize = System::Drawing::Size(_menuItems->Right + marginRight, _menuItems->Bottom + marginBottom);
}

void MenuButton::addButtons(ControlSingleList<LabelButton>^ buttons)
{
	Int32 marginRight = MenuMarginRight;
	Int32 marginBottom = MenuMarginBottom;

	for(Int32 i = 0; i < buttons->Count; ++i) _menuItems->add(buttons[i]);
	_menuForm->ClientSize = System::Drawing::Size(_menuItems->Right + marginRight, _menuItems->Bottom + marginBottom);
}

LabelButton^ MenuButton::getButtonAt(Int32 i)
{ return _menuItems[i]; }

void MenuButton::Event_Tick(Object^ sender, EventArgs^ e)
{
	if(IsMouseWithinBounds || _mouseWithinForm) _hideTimer = 0;
	else {
		for(Int32 i = 0; i < _menuItems->Count; ++i) {
			if(_menuItems[i]->IsMouseWithinBounds) {
				_hideTimer = 0;
				return;
			}
		}

		if(++_hideTimer >= _hideDelay) {
			_hideTimer = 0;
			deactivate();
			_menuForm->Hide();
			_timer->Stop();
		}
	}
}

void MenuButton::Event_FormMouseEnter(Object^ sender, EventArgs^ e)
{
	_mouseWithinForm = true;
}

void MenuButton::Event_FormMouseLeave(Object^ sender, EventArgs^ e)
{
	_mouseWithinForm = false;
}

void MenuButton::Event_MouseClick(Object^ sender, MouseEventArgs^ e)
{
	if(!Activated) {
		if(MenuVisible) {
			_menuForm->Hide();
			_timer->Stop();
		}
	}
	else {
		Point location = _control->PointToScreen(Point::Empty);
		_menuForm->DesktopLocation =
			(_alignedLeft) ?
			Point(location.X + _menuOffset.X, location.Y + _menuOffset.Y + _control->Height - 1) :
			Point(location.X  + _menuOffset.X - _menuForm->ClientSize.Width + Width, location.Y + _menuOffset.Y + _control->Height - 1);

		_menuForm->Show();
		_control->FindForm()->Focus();
		_timer->Start();
	}
}

void MenuButton::_initializeMenu()
{
	ActivatedOnClick = true;

	_menuForm = gcnew Form();
	_menuForm->Name = Name + "_MENU";
	_menuForm->FormBorderStyle = FormBorderStyle::None;
	_menuForm->ControlBox = false;
	_menuForm->ShowInTaskbar = false;
	_menuForm->ShowIcon = false;
	_menuForm->MinimizeBox = false;
	_menuForm->MaximizeBox = false;
	_menuForm->TopMost = true;
	_menuForm->MouseEnter += gcnew EventHandler(this, &MenuButton::Event_FormMouseEnter);
	_menuForm->MouseLeave += gcnew EventHandler(this, &MenuButton::Event_FormMouseLeave);
	_menuForm->Show();
	_menuForm->Hide();
	_menuForm->ClientSize = System::Drawing::Size(20, 20);

	_timer = gcnew Timer();
	_timer->Interval = 15;
	_timer->Tick += gcnew EventHandler(this, &MenuButton::Event_Tick);

	_menuItems = gcnew LabelButtonList(_menuForm);
	_menuItems->Spacing = Point(0, 5);
	_menuItems->Location = Point(10, 10);
	_menuItems->IsListVertical = true;
	_menuItems->IsDynamic = true;

	_menuBorder = gcnew RectangularBorder(Color::Black, 1);
	_menuBorder->linkToControl(_menuForm);

	_menuOffset = Point(0, 0);
	_alignedLeft = true;
	_mouseWithinForm = false;
	_hideDelay = 7;
	_hideTimer = 0;

	_control->MouseDown += gcnew MouseEventHandler(this, &MenuButton::Event_MouseClick);
}