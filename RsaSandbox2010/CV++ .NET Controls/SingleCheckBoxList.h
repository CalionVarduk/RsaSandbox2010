#pragma once

#include "SingleCheckBox.h"
#include "ControlSingleList.h"

namespace CVNetControls
{
	using namespace System;

	public ref class SingleCheckBoxList : public ControlSingleList<SingleCheckBox>
	{
		public:
		#pragma region Properties
			property Int32 CheckOffset {
				public: void set(Int32 value)
						{ for(Int32 i = 0; i < Count; ++i) getAt(i)->CheckOffset = value; }
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

			property bool MultiSelection {
				public: bool get() { return _multiSelect; }
			}

			property bool Locked {
				public: bool get() { return _locked; }
				public: void set(bool value) { _locked = value; }
			}

			property Int32 CheckedIndex {
				public: Int32 get() { return (_multiSelect) ? -1 : _iChecked; }
			}
		#pragma endregion

			SingleCheckBoxList() : ControlSingleList<SingleCheckBox>() { _locked = false; _multiSelect = true; }
			SingleCheckBoxList(Control^ parent) : ControlSingleList<SingleCheckBox>(parent) { _locked = false; _multiSelect = true; }

			SingleCheckBox^ addCheckBox(String^ name, bool checked)
			{
				SingleCheckBox^ box = gcnew SingleCheckBox();
				box->Name = name;
				box->Checked = checked;
				box->addMouseDownEvent(gcnew MouseEventHandler(this, &SingleCheckBoxList::Event_MouseDown));
				return add(box);
			}

			SingleCheckBox^ insertCheckBox(Int32 i, String^ name, bool checked)
			{
				SingleCheckBox^ box = gcnew SingleCheckBox();
				box->Name = name;
				box->Checked = checked;
				box->addMouseDownEvent(gcnew MouseEventHandler(this, &SingleCheckBoxList::Event_MouseDown));
				return insertAt(i, box);
			}

			void disableMultiSelection(Int32 iBox)
			{
				_locked = false;
				_multiSelect = false;
				_iChecked = (iBox >= Count) ? Count - 1 :
							(iBox < 0) ? 0 : iBox;

				for(Int32 i = 0; i < _iChecked; ++i)
					getAt(i)->Checked = false;

				for(Int32 i = _iChecked + 1; i < Count; ++i)
					getAt(i)->Checked = false;

				getAt(_iChecked)->Checked = true;
			}

			bool singleSelection(Int32 iBox)
			{
				if(!_multiSelect) {
					iBox = (iBox >= Count) ? Count - 1 :
							(iBox < 0) ? 0 : iBox;

					if(getAt(iBox)->Checked) getAt(_iChecked)->Checked = false;
					else getAt(iBox)->Checked = true;
					_iChecked = iBox;
					return true;
				}
				return false;
			}

			void enableMultiSelection()
			{ _multiSelect = true; }

		private:
			bool _locked;
			bool _multiSelect;
			Int32 _iChecked;

			void Event_MouseDown(Object^ sender, MouseEventArgs^ e)
			{
				if(!_multiSelect) {
					for(Int32 i = 0; i < Count; ++i) {
						SingleCheckBox^ box = getAt(i);
						if(sender == box->fetchWinControl()) {
							if(_locked) {
								box->Checked = false;
								getAt(_iChecked)->Checked = true;
							}
							else {
								if(box->Checked) getAt(_iChecked)->Checked = false;
								else box->Checked = true;
								_iChecked = i;
							}
							break;
						}
					}
				}
			}
	};
}