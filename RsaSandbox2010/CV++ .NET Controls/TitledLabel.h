#pragma once

#include "TitledControl.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;

	public ref class TitledLabel : public TitledControl<Label>
	{
		public:
			property ContentAlignment TextAlign {
				public: ContentAlignment get() { return _control->TextAlign; }
				public: void set(ContentAlignment value) { _control->TextAlign = value; }
			}

			property System::Windows::Forms::BorderStyle BorderStyle {
				public: System::Windows::Forms::BorderStyle get() { return _control->BorderStyle; }
				public: void set(System::Windows::Forms::BorderStyle value) { _control->BorderStyle = value; }
			}

			TitledLabel() : TitledControl<Label>()
			{ _initialize(); }
			
			TitledLabel(Control^ parent) : TitledControl<Label>()
			{
				Parent = parent;
				_initialize();
			}

			TitledLabel(Control^ parent, String^ name, String^ title) : TitledControl<Label>()
			{
				Name = name;
				_title->Text = title;
				Parent = parent;
				_initialize();
			}

			TitledLabel(Control^ parent, String^ name, String^ title, String^ text) : TitledControl<Label>()
			{
				Name = name;
				_title->Text = title;
				_control->Text = text;
				Parent = parent;
				_initialize();
			}

			Label^ toLabel()
			{ return _control; }

		private:
			void _initialize()
			{
				ControlAutoSize = true;
				ControlFont = gcnew Font("Microsoft Sans Serif", 9);
				Offset = Point(5, 0);
				Location = Point(0, 0);
			}
	};
}