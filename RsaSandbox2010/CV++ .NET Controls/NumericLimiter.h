#pragma once

namespace CVNetControls
{
	using namespace System;

	template <class T>
	public ref class NumericLimiter abstract
	{
		public:
			static T min(T value, T min)
			{ return (value < min) ? min : value; }

			static T max(T value, T max)
			{ return (value > max) ? max : value; }

			static T minMax(T value, T min, T max)
			{
				return (value < min) ? min :
						(value > max) ? max :
										value;
			}
	};
}