#pragma once

#include <type_traits>
#include "DynamicControlAligner.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::Drawing;
	using namespace System::Collections::Generic;

	template <class T>
	public ref class ControlSingleList : public DynamicControl, public IDynamicList
	{
		static_assert(std::is_base_of<DynamicControl, T>::value, "T must inherit from CVNetControls::DynamicControl.");

		public:
		#pragma region Properties
			virtual property bool Enabled {
				public: bool get() override { return _controls->Enabled; }
				public: void set(bool value) override { _controls->Enabled = value; }
			}

			virtual property bool Visible {
				public: bool get() override { return _controls->Visible; }
				public: void set(bool value) override { _controls->Visible = value; }
			}

			property bool IsDynamic {
				public: bool get() { return _dynamic; }
				public: void set(bool value)
						{
							if(_dynamic != value) {
								_dynamic = value;
								if(_dynamic) performLayout();
							}
						}
			}

			property bool IsListHorizontal {
				public: bool get() { return _isListHorizontal; }
				public: void set(bool value)
						{
							if(_isListHorizontal != value) {
								_isListHorizontal = value;
								if(_dynamic) performLayout();
							}
						}
			}

			property bool IsListVertical {
				public: bool get() { return !_isListHorizontal; }
				public: void set(bool value)
						{
							if(_isListHorizontal == value) {
								_isListHorizontal = !value;
								if(_dynamic) performLayout();
							}
						}
			}

			property Int32 SpacingX {
				public: Int32 get() { return _spacing.X; }
				public: void set(Int32 value)
						{
							_spacing.X = value;
							if(_dynamic) performLayout();
						}
			}

			property Int32 SpacingY {
				public: Int32 get() { return _spacing.Y; }
				public: void set(Int32 value)
						{
							_spacing.Y = value;
							if(_dynamic) performLayout();
						}
			}

			property Point Spacing {
				public: Point get() { return _spacing; }
				public: void set(Point value)
						{
							_spacing = value;
							if(_dynamic) performLayout();
						}
			}

			virtual property Int32 Left {
				public: Int32 get() override { return _controls->Left; }
				public: void set(Int32 value) override { _controls->Left = value; }
			}

			virtual property Int32 Right {
				public: Int32 get() override { return _controls->Right; }
				public: void set(Int32 value) override { _controls->Right = value; }
			}

			virtual property Int32 CentreX {
				public: Int32 get() override { return _controls->CentreX; }
				public: void set(Int32 value) override { _controls->CentreX = value; }
			}

			virtual property Int32 Top {
				public: Int32 get() override { return _controls->Top; }
				public: void set(Int32 value) override { _controls->Top = value; }
			}

			virtual property Int32 Bottom {
				public: Int32 get() override { return _controls->Bottom; }
				public: void set(Int32 value) override { _controls->Bottom = value; }
			}

			virtual property Int32 CentreY {
				public: Int32 get() override { return _controls->CentreY; }
				public: void set(Int32 value) override { _controls->CentreY = value; }
			}

			virtual property Point Location {
				public: Point get() override { return _controls->Location; }
				public: void set(Point value) override { _controls->Location = value; }
			}

			virtual property System::Drawing::Size Size {
				public: System::Drawing::Size get() override { return _controls->Size; }
			}

			virtual property Int32 Width {
				public: Int32 get() override { return _controls->Width; }
			}

			virtual property Int32 Height {
				public: Int32 get() override { return _controls->Height; }
			}

			virtual property Rectangle Bounds {
				public: Rectangle get() { return _controls->Bounds; }
			}

			virtual property Control^ Parent {
				public: Control^ get() override { return _parent; }
				public: void set(Control^ value) override
						{
							_parent = value;
							_controls->Parent = value;
						}
			}

			virtual property Int32 Count {
				public: Int32 get() { return _controls->Count; }
			}

			virtual property bool IsTitled {
				public: bool get() override { return false; }
			}

			virtual property bool IsList {
				public: bool get() override { return true; }
			}
		#pragma endregion

			ControlSingleList()
				: _controls(gcnew DynamicControlCollection()), _parent(nullptr), _spacing(Point()), _isListHorizontal(false), _dynamic(false)
			{}

			ControlSingleList(Control^ parent)
				: _controls(gcnew DynamicControlCollection()), _parent(parent), _spacing(Point()), _isListHorizontal(false), _dynamic(false)
			{}

			T^ add(T^ control)
			{
				if(control != nullptr) {
					control->Parent = _parent;
					_controls->add(control);

					if(_dynamic) {
						_layoutFrom(_controls->Count - 1);
						updateBounds();
					}
				}
				return control;
			}

			T^ insertAt(Int32 i, T^ control)
			{
				if(control != nullptr) {
					control->Parent = _parent;
					_controls->insertAt(i, control);

					if(_dynamic) {
						_layoutFrom(i);
						updateBounds();
					}
				}
				return control;
			}

			T^ removeAt(Int32 iControl)
			{
				T^ control = getAt(iControl);
				control->Parent = nullptr;
				_controls->removeAt(iControl);

				if(_dynamic) {
					_layoutFrom(iControl);
					updateBounds();
				}
				return control;
			}

			T^ operator[] (Int32 iControl)
			{ return getAt(iControl); }

			T^ getAt(Int32 iControl)
			{ return _controls->interpretControlAs<T>(iControl); }

			void performLayout()
			{
				if(Count > 0) _layoutFrom(0);
				updateBounds();
			}

			virtual void clear()
			{ _controls->clear(); }

			virtual void updateBounds()
			{ _controls->updateBounds(); }

			virtual DynamicControl^ fetchControlAt(Int32 i)
			{ return _controls->fetchControlAt(i); }

			virtual array<DynamicControl^>^ fetchControlArray()
			{ return _controls->fetchControlArray(); }

			bool performHorizontalAlignment()
			{
				if(!_isListHorizontal && Count > 0) {
					DynamicControlAligner^ aligner = gcnew DynamicControlAligner();
					aligner->addControl(_controls);
					aligner->performHorizontalAlignment();
					aligner->clear();
					return true;
				}
				return false;
			}

			virtual void refresh() override
			{ _controls->refresh(); }

		private:
			Control^ _parent;
			DynamicControlCollection^ _controls;
			bool _isListHorizontal;
			bool _dynamic;
			Point _spacing;

			void _layoutFrom(Int32 iInsert)
			{
				T^ prevControl;
				T^ control;

				if(iInsert == 0) {
					control = getAt(iInsert);
					control->Location = Location;
				}
				else control = getAt(--iInsert);

				if(_isListHorizontal) {
					for(Int32 i = iInsert + 1; i < _controls->Count; ++i) {
						prevControl = control;
						control = getAt(i);
						control->Location = Point(prevControl->Right + _spacing.X, prevControl->Top + _spacing.Y);
					}
				}
				else for(Int32 i = iInsert + 1; i < _controls->Count; ++i) {
					prevControl = control;
					control = getAt(i);
					control->Location = Point(prevControl->Left + _spacing.X, prevControl->Bottom + _spacing.Y);
				}
			}
	};
}