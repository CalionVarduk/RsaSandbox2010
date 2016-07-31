#pragma once

#include "IDynamicList.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::Collections::Generic;
	using namespace System::Windows::Forms;

	public ref class DynamicControlCollection : public DynamicControl, public IDynamicList
	{
		public:
		#pragma region Properties
			virtual property bool Enabled {
				public: bool get() override { return true; }
				public: void set(bool value) override
						{ for(Int32 i = 0; i < this->Count; ++i) getAt(i)->Enabled = value; }
			}

			virtual property bool Visible {
				public: bool get() override { return true; }
				public: void set(bool value) override
						{ for(Int32 i = 0; i < this->Count; ++i) getAt(i)->Visible = value; }
			}

			virtual property Int32 Left {
				public: Int32 get() override { return _bounds.X; }
				public: void set(Int32 value) override
						{
							Int32 offset = value - Left;
							_bounds.X += offset;

							for(Int32 i = 0; i < _controls->Count; ++i)
								getAt(i)->Left += offset;
						}
			}

			virtual property Int32 Right {
				public: Int32 get() override { return _bounds.X + _bounds.Width; }
				public: void set(Int32 value) override
						{
							Int32 offset = value - Right;
							_bounds.X += offset;

							for(Int32 i = 0; i < _controls->Count; ++i)
								getAt(i)->Left += offset;
						}
			}

			virtual property Int32 CentreX {
				public: Int32 get() override { return _bounds.X + (_bounds.Width >> 1); }
				public: void set(Int32 value) override
						{
							Int32 offset = value - CentreX;
							_bounds.X += offset;

							for(Int32 i = 0; i < _controls->Count; ++i)
								getAt(i)->Left += offset;
						}
			}

			virtual property Int32 Top {
				public: Int32 get() override { return _bounds.Y; }
				public: void set(Int32 value) override
						{
							Int32 offset = value - Top;
							_bounds.Y += offset;

							for(Int32 i = 0; i < _controls->Count; ++i)
								getAt(i)->Top += offset;
						}
			}

			virtual property Int32 Bottom {
				public: Int32 get() override { return _bounds.Y + _bounds.Height; }
				public: void set(Int32 value) override
						{
							Int32 offset = value - Bottom;
							_bounds.Y += offset;

							for(Int32 i = 0; i < _controls->Count; ++i)
								getAt(i)->Top += offset;
						}
			}

			virtual property Int32 CentreY {
				public: Int32 get() override { return _bounds.Y + (_bounds.Height >> 1); }
				public: void set(Int32 value) override
						{
							Int32 offset = value - CentreY;
							_bounds.Y += offset;

							for(Int32 i = 0; i < _controls->Count; ++i)
								getAt(i)->Top += offset;
						}
			}

			virtual property Point Location {
				public: Point get() override { return Point(_bounds.X, _bounds.Y); }
				public: void set(Point value) override
						{
							Left = value.X;
							Top = value.Y;
						}
			}

			virtual property System::Drawing::Size Size {
				public: System::Drawing::Size get() override { return System::Drawing::Size(Width, Height); }
			}

			virtual property Int32 Width {
				public: Int32 get() override { return _bounds.Width; }
			}

			virtual property Int32 Height {
				public: Int32 get() override { return _bounds.Height; }
			}

			virtual property Rectangle Bounds {
				public: Rectangle get() { return _bounds; }
			}

			virtual property Control^ Parent {
				public: Control^ get() override { return nullptr; }
				public: void set(Control^ value) override
						{
							for(Int32 i = 0; i < _controls->Count; ++i)
								getAt(i)->Parent = value;
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

			property Int32 TitledControls {
				public: Int32 get()
						{
							Int32 count = 0;
							for(Int32 i = 0; i < Count; ++i)
								if(_controls[i]->IsTitled && !_controls[i]->IsList) ++count;
							return count;
						}
			}

			property Int32 SingleControls {
				public: Int32 get()
						{
							Int32 count = 0;
							for(Int32 i = 0; i < Count; ++i)
								if(!_controls[i]->IsTitled && !_controls[i]->IsList) ++count;
							return count;
						}
			}

			property Int32 ControlSingleLists {
				public: Int32 get()
						{
							Int32 count = 0;
							for(Int32 i = 0; i < Count; ++i)
								if(!_controls[i]->IsTitled && _controls[i]->IsList) ++count;
							return count;
						}
			}

			property Int32 ControlTitledLists {
				public: Int32 get()
						{
							Int32 count = 0;
							for(Int32 i = 0; i < Count; ++i)
								if(!_controls[i]->IsTitled && _controls[i]->IsList) ++count;
							return count;
						}
			}
		#pragma endregion

			DynamicControlCollection();

			void add(DynamicControl^ control);
			void insertAt(Int32 i, DynamicControl^ control);
			void removeAt(Int32 iControl);

			DynamicControl^ operator[] (Int32 iControl);
			DynamicControl^ getAt(Int32 iControl);

			template <class T>
			T^ interpretControlAs(Int32 iControl)
			{ return _controls[iControl]->interpretAs<T>(); }

			virtual void clear();
			virtual void updateBounds();
			virtual DynamicControl^ fetchControlAt(Int32 i);
			virtual array<DynamicControl^>^ fetchControlArray();

			virtual void refresh() override;

		private:
			List<DynamicControl^>^ _controls;
			Rectangle _bounds;
	};
}