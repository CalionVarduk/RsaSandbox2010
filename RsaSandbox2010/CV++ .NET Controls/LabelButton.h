#pragma once

#include "SingleControl.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::Drawing;
	using namespace System::Windows::Forms;

	public ref class LabelButton : public SingleControl<Label>
	{
		public:
		#pragma region Properties
			property ContentAlignment TextAlign {
				public: ContentAlignment get() { return _control->TextAlign; }
				public: void set(ContentAlignment value) { _control->TextAlign = value; }
			}

			property bool IsMouseDown { 
				public: bool get() { return _mouseDown; }
			}

			property bool IsMouseMoving { 
				public: bool get() { return _mouseMove; }
			}

			property bool IsMouseWithinBounds { 
				public: bool get() { return _mouseWithinBounds; }
			}

			property bool IsMouseHovering { 
				public: bool get() { return _mouseHover; }
			}

			property bool Activated { 
				public: bool get() { return _isActive; }
			}

			property bool ActivatedAndColorable { 
				public: bool get() { return (ColorableActivation && Activated); }
			}

			property bool MousedOverAndColorable { 
				public: bool get() { return (ColorableMouseOver && IsMouseWithinBounds); }
			}

			property bool ActivatedOnClick { 
				public: bool get() { return _activeOnClick; }
				public: void set(bool value) { _activeOnClick = value; }
			}

			property bool ActivatedOnHold { 
				public: bool get() { return !_activeOnClick; }
				public: void set(bool value) { _activeOnClick = !value; }
			}

			property bool ColorableMouseOver { 
				public: bool get() { return _colorableMouseOver; }
				public: void set(bool value) { _colorableMouseOver = value; }
			}

			property bool ColorableActivation { 
				public: bool get() { return _colorableActivation; }
				public: void set(bool value) { _colorableActivation = value; }
			}

			property bool OnClickDeactivatable { 
				public: bool get() { return _onClickDeactivatable; }
				public: void set(bool value) { _onClickDeactivatable = value; }
			}

			property Color BackColorNormal {
				public: Color get() { return _backColorNormal; }
				public: void set(Color value)
						{
							_backColorNormal = value;
							if(Enabled && !ActivatedAndColorable && !MousedOverAndColorable)
								BackColor = _backColorNormal;
						}
			}

			property Color ForeColorNormal {
				public: Color get() { return _foreColorNormal; }
				public: void set(Color value)
						{
							_foreColorNormal = value;
							if(Enabled && !ActivatedAndColorable && !MousedOverAndColorable)
								ForeColor = _foreColorNormal;
						}
			}

			property Color BackColorMouseOver {
				public: Color get() { return _backColorMouseOver; }
				public: void set(Color value)
						{
							_backColorMouseOver = value;
							if(Enabled && !ActivatedAndColorable && MousedOverAndColorable)
								BackColor = _backColorMouseOver;
						}
			}

			property Color ForeColorMouseOver {
				public: Color get() { return _foreColorMouseOver; }
				public: void set(Color value)
						{
							_foreColorMouseOver = value;
							if(Enabled && !ActivatedAndColorable && MousedOverAndColorable)
								ForeColor = _foreColorMouseOver;
						}
			}

			property Color BackColorActive {
				public: Color get() { return _backColorActive; }
				public: void set(Color value)
						{
							_backColorActive = value;
							if(Enabled && ActivatedAndColorable && !MousedOverAndColorable)
								BackColor = _backColorActive;
						}
			}

			property Color ForeColorActive {
				public: Color get() { return _foreColorActive; }
				public: void set(Color value)
						{
							_foreColorActive = value;
							if(Enabled && ActivatedAndColorable && !MousedOverAndColorable)
								ForeColor = _foreColorActive;
						}
			}

			property Color BackColorActiveMouseOver {
				public: Color get() { return _backColorActiveMouseOver; }
				public: void set(Color value)
						{
							_backColorActiveMouseOver = value;
							if(Enabled && ActivatedAndColorable && MousedOverAndColorable)
								BackColor = _backColorActiveMouseOver;
						}
			}

			property Color ForeColorActiveMouseOver {
				public: Color get() { return _foreColorActiveMouseOver; }
				public: void set(Color value)
						{
							_foreColorActiveMouseOver = value;
							if(Enabled && ActivatedAndColorable && MousedOverAndColorable)
								ForeColor = _foreColorActiveMouseOver;
						}
			}

			property Color BackColorDisabled {
				public: Color get() { return _backColorDisabled; }
				public: void set(Color value)
						{
							_backColorDisabled = value;
							if(!Enabled) BackColor = _backColorDisabled;
						}
			}

			property Color ForeColorDisabled {
				public: Color get() { return _foreColorDisabled; }
				public: void set(Color value)
						{
							_foreColorDisabled = value;
							if(!Enabled) ForeColor = _foreColorDisabled;
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

			LabelButton();
			LabelButton(Control^ parent);
			LabelButton(Control^ parent, String^ name, String^ text);

			void activate();
			void deactivate();

			void addMouseDownEvent(MouseEventHandler^ handler);
			void addMouseUpEvent(MouseEventHandler^ handler);
			void addMouseMoveEvent(MouseEventHandler^ handler);
			void addMouseEnterEvent(EventHandler^ handler);
			void addMouseLeaveEvent(EventHandler^ handler);
			void addMouseHoverEvent(EventHandler^ handler);

			Label^ toLabel();

		private:
		#pragma region Properties Variables
			bool _mouseDown;
			bool _mouseMove;
			bool _mouseWithinBounds;
			bool _mouseHover;
			bool _isActive;
			bool _activeOnClick;
			bool _colorableMouseOver;
			bool _colorableActivation;
			bool _onClickDeactivatable;
			Color _backColorNormal;
			Color _foreColorNormal;
			Color _backColorMouseOver;
			Color _foreColorMouseOver;
			Color _backColorActive;
			Color _foreColorActive;
			Color _backColorActiveMouseOver;
			Color _foreColorActiveMouseOver;
			Color _backColorDisabled;
			Color _foreColorDisabled;
			RectangularBorder^ _border;
		#pragma endregion

			void Event_MouseDown(Object^ sender, MouseEventArgs^ e);
			void Event_MouseUp(Object^ sender, MouseEventArgs^ e);
			void Event_MouseMove(Object^ sender, MouseEventArgs^ e);
			void Event_MouseEnter(Object^ sender, EventArgs^ e);
			void Event_MouseLeave(Object^ sender, EventArgs^ e);
			void Event_MouseHover(Object^ sender, EventArgs^ e);

			void Event_EnabledChanged(Object^ sender, EventArgs^ e);

			void _activateColorChange();
			void _mouseEnterColorChange();
			void _mouseLeaveColorChange();

			void _initialize();
			void _initializeColors();
			void _initializeEvents();
	};
}