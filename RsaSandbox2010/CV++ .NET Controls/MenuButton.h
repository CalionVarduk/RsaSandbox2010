#pragma once

#include "LabelButtonList.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::Drawing;
	using namespace System::Windows::Forms;

	public ref class MenuButton : public LabelButton
	{
		public:
		#pragma region Properties
			property Int32 MenuMarginLeft {
				public: Int32 get() { return _menuItems->Left; }
				public: void set(Int32 value)
						{
							Int32 marginRight = MenuMarginRight;
							_menuItems->Left = value;
							_menuForm->ClientSize = System::Drawing::Size(_menuItems->Right + marginRight, _menuForm->ClientSize.Height);
						}
			}

			property Int32 MenuMarginRight {
				public: Int32 get() { return (_menuForm->ClientSize.Width - _menuItems->Right); }
				public: void set(Int32 value)
						{
							Int32 offset = value - MenuMarginRight;
							_menuForm->ClientSize = System::Drawing::Size(_menuForm->ClientSize.Width + offset, _menuForm->ClientSize.Height);
						}
			}

			property Int32 MenuMarginTop {
				public: Int32 get() { return _menuItems->Top; }
				public: void set(Int32 value)
						{
							Int32 marginBottom = MenuMarginBottom;
							_menuItems->Top = value;
							_menuForm->ClientSize = System::Drawing::Size(_menuForm->ClientSize.Width, _menuItems->Bottom + marginBottom);
						}
			}

			property Int32 MenuMarginBottom {
				public: Int32 get() { return (_menuForm->ClientSize.Height - _menuItems->Bottom); }
				public: void set(Int32 value)
						{
							Int32 offset = value - MenuMarginBottom;
							_menuForm->ClientSize = System::Drawing::Size(_menuForm->ClientSize.Width, _menuForm->ClientSize.Height + offset);
						}
			}

			property Int32 MenuHideDelay {
				public: Int32 get() { return (_hideDelay * _timer->Interval); }
				public: void set(Int32 value) { _hideDelay = (Int32)((((value > 0) ? value : 0) / (float)_timer->Interval) + 0.5f); }
			}

			property Int32 MenuItemSpacing {
				public: Int32 get() { return _menuItems->SpacingY; }
				public: void set(Int32 value)
						{
							Int32 marginBottom = MenuMarginBottom;
							_menuItems->SpacingY = value;
							_menuForm->ClientSize = System::Drawing::Size(_menuForm->ClientSize.Width, _menuItems->Bottom + marginBottom);
						}
			}
			
			property bool MenuVisible {
				public: bool get() { return _menuForm->Visible; }
			}

			property bool MenuAlignedLeft {
				public: bool get() { return _alignedLeft; }
				public: void set(bool value) { _alignedLeft = value; }
			}

			property bool MenuAlignedRight {
				public: bool get() { return !_alignedLeft; }
				public: void set(bool value) { _alignedLeft = !value; }
			}

			property System::Drawing::Size MenuSize {
				public: System::Drawing::Size get() { return _menuForm->ClientSize; }
			}

			property Int32 MenuWidth {
				public: Int32 get() { return _menuForm->ClientSize.Width; }
			}

			property Int32 MenuHeight {
				public: Int32 get() { return _menuForm->ClientSize.Height; }
			}

			property Point MenuOffset {
				public: Point get() { return _menuOffset; }
				public: void set(Point value) { _menuOffset = value; }
			}

			property Int32 MenuOffsetX {
				public: Int32 get() { return _menuOffset.X; }
				public: void set(Int32 value) { _menuOffset.X = value; }
			}

			property Int32 MenuOffsetY {
				public: Int32 get() { return _menuOffset.Y; }
				public: void set(Int32 value) { _menuOffset.Y = value; }
			}

			property Color MenuBackColor {
				public: Color get() { return _menuForm->BackColor; }
				public: void set(Color value) { _menuForm->BackColor = value; }
			}

			property RectangularBorder^ MenuBorderStyle {
				public: RectangularBorder^ get() { return _menuBorder; }
				public: void set(RectangularBorder^ value)
						{
							_menuBorder->unlinkFromControl(_menuForm);
							_menuBorder = (value != nullptr) ? value : gcnew RectangularBorder();
							_menuBorder->linkToControl(_menuForm);
							if(MenuVisible) _menuForm->Refresh();
						}
			}

			property Int32 MenuBorderThickness {
				public: Int32 get() { return _menuBorder->Thickness; }
				public: void set(Int32 value)
						{
							_menuBorder->Thickness = value;
							if(MenuVisible) _menuForm->Refresh();
						}
			}

			property Color MenuBorderColor {
				public: Color get() { return _menuBorder->Color; }
				public: void set(Color value)
						{
							_menuBorder->Color = value;
							if(MenuVisible) _menuForm->Refresh();
						}
			}

			property Color MenuBorderColorDisabled {
				public: Color get() { return _menuBorder->ColorDisabled; }
				public: void set(Color value)
						{
							_menuBorder->ColorDisabled = value;
							if(MenuVisible) _menuForm->Refresh();
						}
			}

			property LabelButtonList^ MenuItems {
				public: LabelButtonList^ get() { return _menuItems; }
			}
		#pragma endregion

			MenuButton();
			MenuButton(Control^ parent);
			MenuButton(Control^ parent, String^ name, String^ text);

			void addButton(LabelButton^ button);
			void addButtons(ControlSingleList<LabelButton>^ buttons);

			LabelButton^ getButtonAt(Int32 i);

		private:
			Form^ _menuForm;
			Timer^ _timer;
			LabelButtonList^ _menuItems;
			RectangularBorder^ _menuBorder;
			Point _menuOffset;
			Int32 _hideDelay;
			Int32 _hideTimer;
			bool _mouseWithinForm;
			bool _alignedLeft;

			void Event_Tick(Object^ sender, EventArgs^ e);
			void Event_FormMouseEnter(Object^ sender, EventArgs^ e);
			void Event_FormMouseLeave(Object^ sender, EventArgs^ e);
			void Event_MouseClick(Object^ sender, MouseEventArgs^ e);

			void _initializeMenu();
	};
}