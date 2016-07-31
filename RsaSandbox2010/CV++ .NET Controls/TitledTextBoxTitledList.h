#pragma once

#include "TitledTextBoxList.h"
#include "ControlTitledList.h"

namespace CVNetControls
{
	using namespace System;

	public ref class TitledTextBoxTitledList : public ControlTitledList<TitledTextBox>
	{
		public:
		#pragma region Properties
			property bool ControlAutoSize {
				public: void set(bool value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->ControlAutoSize = value; }
			}

			property HorizontalAlignment TextAlign {
				public: void set(HorizontalAlignment value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->TextAlign = value; }
			}

			property System::Windows::Forms::BorderStyle BorderStyle {
				public: void set(System::Windows::Forms::BorderStyle value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->BorderStyle = value; }
			}

			property Int32 MaxLength {
				public: void set(Int32 value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->MaxLength = value; }
			}

			property bool ReadOnly {
				public: void set(bool value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->ReadOnly = value; }
			}

			property bool Multiline {
				public: void set(bool value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->Multiline = value; }
			}

			property bool WordWrap {
				public: void set(bool value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->WordWrap = value; }
			}

			property System::Windows::Forms::ScrollBars ScrollBars {
				public: void set(System::Windows::Forms::ScrollBars value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->ScrollBars = value; }
			}

			property System::Windows::Forms::Cursor^ Cursor {
				public: void set(System::Windows::Forms::Cursor^ value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->Cursor = value; }
			}
		#pragma endregion

			TitledTextBoxTitledList() : ControlTitledList<TitledTextBox>() {}
			TitledTextBoxTitledList(Control^ parent) : ControlTitledList<TitledTextBox>(parent) {}

			TitledTextBox^ addTextBox(String^ name, String^ title, String^ text)
			{
				TitledTextBox^ box = gcnew TitledTextBox();
				box->Name = name;
				box->TitleText = title;
				box->ControlText = text;
				box->OffsetX = 5;
				return add(box);
			}

			TitledTextBox^ insertTextBox(Int32 i, String^ name, String^ title, String^ text)
			{
				TitledTextBox^ box = gcnew TitledTextBox();
				box->Name = name;
				box->TitleText = title;
				box->ControlText = text;
				box->OffsetX = 5;
				return insertAt(i, box);
			}
	};
}