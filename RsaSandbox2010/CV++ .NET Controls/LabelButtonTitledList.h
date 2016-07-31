#pragma once

#include "LabelButtonList.h"
#include "ControlTitledList.h"

namespace CVNetControls
{
	using namespace System;

	public ref class LabelButtonTitledList : public ControlTitledList<LabelButton>
	{
		public:
		#pragma region Properties
			property System::Drawing::Font^ Font {
				public: void set(System::Drawing::Font^ value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->Font = value; }
			}

			property bool AutoSize {
				public: void set(bool value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->AutoSize = value; }
			}

			property ContentAlignment TextAlign {
				public: void set(ContentAlignment value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->TextAlign = value; }
			}

			property bool ActivatedOnClick { 
				public: void set(bool value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->ActivatedOnClick = value; }
			}

			property bool ActivatedOnHold { 
				public: void set(bool value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->ActivatedOnHold = value; }
			}

			property bool ColorableMouseOver { 
				public: void set(bool value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->ColorableMouseOver = value; }
			}

			property bool ColorableActivation { 
				public: void set(bool value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->ColorableActivation = value; }
			}

			property bool OnClickDeactivatable { 
				public: void set(bool value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->OnClickDeactivatable = value; }
			}

			property Color BackColorNormal {
				public: void set(Color value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->BackColorNormal = value; }
			}

			property Color ForeColorNormal {
				public: void set(Color value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->ForeColorNormal = value; }
			}

			property Color BackColorMouseOver {
				public: void set(Color value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->BackColorMouseOver = value; }
			}

			property Color ForeColorMouseOver {
				public: void set(Color value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->ForeColorMouseOver = value; }
			}

			property Color BackColorActive {
				public: void set(Color value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->BackColorActive = value; }
			}

			property Color ForeColorActive {
				public: void set(Color value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->ForeColorActive = value; }
			}

			property Color BackColorActiveMouseOver {
				public: void set(Color value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->BackColorActiveMouseOver = value; }
			}

			property Color ForeColorActiveMouseOver {
				public: void set(Color value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->ForeColorActiveMouseOver = value; }
			}

			property Color BackColorDisabled {
				public: void set(Color value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->BackColorDisabled = value; }
			}

			property Color ForeColorDisabled {
				public: void set(Color value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->ForeColorDisabled = value; }
			}

			property System::Windows::Forms::Cursor^ Cursor {
				public: void set(System::Windows::Forms::Cursor^ value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->Cursor = value; }
			}

			property RectangularBorder^ BorderStyle {
				public: void set(RectangularBorder^ value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->BorderStyle = value; }
			}

			property Int32 BorderThickness {
				public: void set(Int32 value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->BorderThickness = value; }
			}

			property Color BorderColor {
				public: void set(Color value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->BorderColor = value; }
			}

			property Color BorderColorDisabled {
				public: void set(Color value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->BorderColorDisabled = value; }
			}
		#pragma endregion

			LabelButtonTitledList() : ControlTitledList<LabelButton>() {}
			LabelButtonTitledList(Control^ parent) : ControlTitledList<LabelButton>(parent) {}

			LabelButton^ addButton(String^ name, String^ text)
			{
				LabelButton^ button = gcnew LabelButton();
				button->Name = name;
				button->Text = text;
				return add(button);
			}

			LabelButton^ insertButton(Int32 i, String^ name, String^ text)
			{
				LabelButton^ button = gcnew LabelButton();
				button->Name = name;
				button->Text = text;
				return insertAt(i, button);
			}
	};
}