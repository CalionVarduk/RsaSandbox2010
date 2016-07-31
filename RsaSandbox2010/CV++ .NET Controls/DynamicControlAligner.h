#pragma once

#include "SingleControl.h"
#include "DynamicControlCollection.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::Collections::Generic;
	using namespace System::Windows::Forms;

	public ref class DynamicControlAligner
	{
		public:
			property Int32 Count {
				public: Int32 get() { return _controls->Count; }
			}

			property Int32 TitledControls {
				public: Int32 get() { return _controls->TitledControls; }
			}

			property Int32 SingleControls {
				public: Int32 get() { return _controls->SingleControls; }
			}

			property Int32 ControlSingleLists {
				public: Int32 get() { return _controls->ControlSingleLists; }
			}

			property Int32 ControlTitledLists {
				public: Int32 get() { return _controls->ControlTitledLists; }
			}

			DynamicControlAligner() : _controls(gcnew DynamicControlCollection()), _lists(gcnew List<IDynamicList^>()) {}

			void addControl(Control^ control)
			{
				SingleControl<Control>^ ctrl = gcnew SingleControl<Control>();
				ctrl->setWinControl(control);
				_addSingleControl(ctrl);
			}

			void addControl(DynamicControl^ control)
			{
				if(control->IsList) {
					_lists->Add((IDynamicList^)control);
					array<DynamicControl^>^ list = ((IDynamicList^)control)->fetchControlArray();
					for(Int32 i = 0; i < list->Length; ++i) {
						if(list[i]->IsTitled) addControl(list[i]->fetchWinControl());
						else _addSingleControl(list[i]);
					}
				}
				else if(control->IsTitled) addControl(control->fetchWinControl());
				else _addSingleControl(control);
			}

			void performHorizontalAlignment()
			{
				if(Count > 0) {
					Int32 controlX = _controls[0]->Left;

					for(Int32 i = 1; i < Count; ++i)
						if(controlX < _controls[i]->Left)
							controlX = _controls[i]->Left;

					for(Int32 i = 0; i < Count; ++i)
						_controls[i]->Left = controlX;

					for(Int32 i = 0; i < _lists->Count; ++i)
						_lists[i]->updateBounds();
				}
			}

			void clear()
			{
				_controls->clear();
				_lists->Clear();
			}

		private:
			DynamicControlCollection^ _controls;
			List<IDynamicList^>^ _lists;

			void _addSingleControl(DynamicControl^ control)
			{
				_controls->add(control);
			}
	};
}