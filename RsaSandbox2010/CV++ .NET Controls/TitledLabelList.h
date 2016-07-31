#pragma once

#include "TitledLabel.h"
#include "ControlSingleList.h"

namespace CVNetControls
{
	using namespace System;

	public ref class TitledLabelList : public ControlSingleList<TitledLabel>
	{
		public:
		#pragma region Properties
			property System::Drawing::Font^ TitleFont {
				public: void set(System::Drawing::Font^ value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->TitleFont = value; }
			}

			property System::Drawing::Font^ ControlFont {
				public: void set(System::Drawing::Font^ value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->ControlFont = value; }
			}

			property bool ControlAutoSize {
				public: void set(bool value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->ControlAutoSize = value; }
			}

			property ContentAlignment TextAlign {
				public: void set(ContentAlignment value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->TextAlign = value; }
			}

			property System::Windows::Forms::BorderStyle BorderStyle {
				public: void set(System::Windows::Forms::BorderStyle value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->BorderStyle = value; }
			}

			property System::Windows::Forms::Cursor^ Cursor {
				public: void set(System::Windows::Forms::Cursor^ value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->Cursor = value; }
			}
		#pragma endregion

			TitledLabelList() : ControlSingleList<TitledLabel>() {}
			TitledLabelList(Control^ parent) : ControlSingleList<TitledLabel>(parent) {}

			TitledLabel^ addLabel(String^ name, String^ title, String^ text)
			{
				TitledLabel^ label = gcnew TitledLabel();
				label->Name = name;
				label->TitleText = title;
				label->ControlText = text;
				label->OffsetX = 5;
				return add(label);
			}

			TitledLabel^ insertLabel(Int32 i, String^ name, String^ title, String^ text)
			{
				TitledLabel^ label = gcnew TitledLabel();
				label->Name = name;
				label->TitleText = title;
				label->ControlText = text;
				label->OffsetX = 5;
				return insertAt(i, label);
			}
	};
}