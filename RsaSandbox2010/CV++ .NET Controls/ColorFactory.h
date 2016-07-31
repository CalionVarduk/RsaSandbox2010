#pragma once

#include "NumericLimiter.h"

namespace CVNetControls
{
	using namespace System;
	using namespace System::Drawing;

	public ref class ColorFactory abstract
	{
		public:
			static Color Scale(Color color, float scale)
			{
				scale = NumericLimiter<float>::min(scale, 0);

				float maxScaleR = (color.R > 0) ? (255.0f / color.R) : 255.0f;
				float maxScaleG = (color.G > 0) ? (255.0f / color.G) : 255.0f;
				float maxScaleB = (color.B > 0) ? (255.0f / color.B) : 255.0f;

				scale = Math::Min(scale, maxScaleR);
				scale = Math::Min(scale, maxScaleG);
				scale = Math::Min(scale, maxScaleB);

				return Color::FromArgb((Int32)(color.R * scale), (Int32)(color.G * scale), (Int32)(color.B * scale));
			}
	};
}