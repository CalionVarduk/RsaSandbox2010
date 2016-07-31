#pragma once

#include "DynamicControl.h"

namespace CVNetControls
{
	public ref class DoubleBufferedPanel : public Panel
	{
		public:
			DoubleBufferedPanel() : Panel()
			{ DoubleBuffered = true; }
	};
}