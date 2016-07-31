#pragma once

#include "RectangularBorder.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::Collections::Generic;

	public ref class DynamicControl abstract
	{
		public:
			virtual property bool Enabled {
				public: virtual bool get() = 0;
				public: virtual void set(bool value) = 0;
			}

			virtual property bool Visible {
				public: virtual bool get() = 0;
				public: virtual void set(bool value) = 0;
			}

			virtual property Int32 Left {
				public: virtual Int32 get() = 0;
				public: virtual void set(Int32 value) = 0;
			}

			virtual property Int32 Right {
				public: virtual Int32 get() = 0;
				public: virtual void set(Int32 value) = 0;
			}

			virtual property Int32 CentreX {
				public: virtual Int32 get() = 0;
				public: virtual void set(Int32 value) = 0;
			}

			virtual property Int32 Top {
				public: virtual Int32 get() = 0;
				public: virtual void set(Int32 value) = 0;
			}

			virtual property Int32 Bottom {
				public: virtual Int32 get() = 0;
				public: virtual void set(Int32 value) = 0;
			}

			virtual property Int32 CentreY {
				public: virtual Int32 get() = 0;
				public: virtual void set(Int32 value) = 0;
			}

			virtual property Point Location {
				public: virtual Point get() = 0;
				public: virtual void set(Point value) = 0;
			}

			virtual property System::Drawing::Size Size {
				public: virtual System::Drawing::Size get() = 0;
			}

			virtual property Int32 Width {
				public: virtual Int32 get() = 0;
			}

			virtual property Int32 Height {
				public: virtual Int32 get() = 0;
			}

			virtual property Control^ Parent {
				public: virtual Control^ get() = 0;
				public: virtual void set(Control^ value) = 0;
			}

			virtual property bool IsTitled {
				public: virtual bool get() = 0;
			}

			virtual property bool IsList {
				public: virtual bool get() = 0;
			}

			virtual void refresh() = 0;

			template <class T>
			T^ interpretAs()
			{ return (T^)this; }

			Control^ fetchWinControl()
			{
				if(IsList) throw gcnew InvalidOperationException("List-type DynamicControls can not return System::Windows::Fomrs::Control-type objects.");
				return _returnControl();
			}

		protected:
			DynamicControl() {}
			virtual Control^ _returnControl() { return nullptr; }
	};

	public delegate void DynamicControlPostEventHandler(DynamicControl^ sender);
	public delegate void DynamicControlPreEventHandler(DynamicControl^ sender, bool% cancel);
}