#pragma once

#include "DynamicControl.h"

namespace CVNetControls
{

	public interface class IDynamicList
	{
		public:
			property Int32 Count {
				public: Int32 get();
			}

			property Rectangle Bounds {
				public: Rectangle get();
			}

			DynamicControl^ fetchControlAt(Int32 i);
			void clear();
			void updateBounds();
			array<DynamicControl^>^ fetchControlArray();
	};
}