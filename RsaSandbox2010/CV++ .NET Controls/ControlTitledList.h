#pragma once

#include "ControlSingleList.h"
#include "TitledControlBase.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::Drawing;
	using namespace System::Collections::Generic;

	template <class T>
	public ref class ControlTitledList : public TitledControlBase<ControlSingleList<T>>, public IDynamicList
	{
		static_assert(std::is_base_of<DynamicControl, T>::value, "T must inherit from CVNetControls::DynamicControl.");

		public:
		#pragma region Properties
			property System::Drawing::Size ControlSize {
				public: System::Drawing::Size get() { return _control->Size; }
			}

			property Int32 ControlWidth {
				public: Int32 get() { return _control->Width; }
			}

			property Int32 ControlHeight {
				public: Int32 get() { return _control->Height; }
			}

			property String^ TitleName {
				public: String^ get() { return _title->Name; }
				public: void set(String^ value) { _title->Name = value; }
			}

			property bool IsDynamic {
				public: bool get() { return _control->IsDynamic; }
				public: void set(bool value) { _control->IsDynamic = value; }
			}

			property bool IsListHorizontal {
				public: bool get() { return _control->IsListHorizontal; }
				public: void set(bool value) { _control->IsListHorizontal = value; }
			}

			property bool IsListVertical {
				public: bool get() { return _control->IsListVertical; }
				public: void set(bool value) { _control->IsListVertical = value; }
			}

			property Int32 SpacingX {
				public: Int32 get() { return _control->SpacingX; }
				public: void set(Int32 value) { _control->SpacingX = value; }
			}

			property Int32 SpacingY {
				public: Int32 get() { return _control->SpacingY; }
				public: void set(Int32 value) { _control->SpacingY = value; }
			}

			property Point Spacing {
				public: Point get() { return _control->Spacing; }
				public: void set(Point value) { _control->Spacing = value; }
			}

			virtual property Int32 Count {
				public: Int32 get() { return _control->Count; }
			}

			virtual property Rectangle Bounds {
				public: Rectangle get() { return _control->Bounds; }
			}

			virtual property Control^ Parent {
				public: Control^ get() override { return _title->Parent; }
				public: void set(Control^ value) override
						{
							if(_title->Parent != nullptr)
								_title->Parent->Controls->Remove(_title);
							if(value != nullptr)
								value->Controls->Add(_title);
							_control->Parent = value;
						}
			}

			virtual property bool IsList {
				public: bool get() override { return true; }
			}
		#pragma endregion

			ControlTitledList() : TitledControlBase<ControlSingleList<T>>() {}
			ControlTitledList(Control^ parent) : TitledControlBase<ControlSingleList<T>>()
			{ Parent = parent; }

			T^ add(T^ control)
			{ return _control->add(control); }

			T^ insertAt(Int32 i, T^ control)
			{ return _control->insertAt(i, control); }

			T^ removeAt(Int32 iControl)
			{ return _control->removeAt(iControl); }

			T^ operator[] (Int32 iControl)
			{ return getAt(iControl); }

			T^ getAt(Int32 iControl)
			{ return _control->getAt(iControl); }

			virtual DynamicControl^ fetchControlAt(Int32 i)
			{ return _control->fetchControlAt(i); }

			void performLayout()
			{ _control->performLayout(); }

			virtual void clear()
			{ _control->clear(); }

			virtual void updateBounds()
			{ _control->updateBounds(); }

			void performHorizontalAlignment()
			{ _control->performHorizontalAlignment(); }

			virtual array<DynamicControl^>^ fetchControlArray()
			{ return _control->fetchControlArray(); }

			ControlSingleList<T>^ toSingleList()
			{ return _control; }

			virtual void refresh() override
			{
				_title->Refresh();
				_control->refresh();
			}
	};
}