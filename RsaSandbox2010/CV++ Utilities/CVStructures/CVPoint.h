#pragma once

namespace CVStructures
{
	using namespace System;
	using namespace System::Drawing;

	template <class T>
	public value class CVPoint
	{
		public:
			static const CVPoint<T> Empty = CVPoint<T>(0, 0);

			property T X {
				public: T get() { return _x; }
				public: void set(T value) { _x = value; }
			}

			property T Y {
				public: T get() { return _y; }
				public: void set(T value) { _y = value; }
			}

			property bool IsEmpty {
				public: bool get() { return ((_x == 0) && (_y == 0)); }
			}

			CVPoint(T x, T y) : _x(x), _y(y) {}

			void offset(T dx, T dy)
			{
				_x += dx;
				_y += dy;
			}

			static bool operator== (const CVPoint<T>% pt1, const CVPoint<T>% pt2)
			{
				return (pt1._x == pt2._x && pt1._y == pt2._y);
			}

			static bool operator!= (const CVPoint<T>% pt1, const CVPoint<T>% pt2)
			{
				return (pt1._x != pt2._x || pt1._y != pt2._y);
			}

			static CVPoint<T> operator+ (const CVPoint<T>% pt1, const CVPoint<T>% pt2)
			{
				return CVPoint<T>(pt1._x + pt2._x, pt1._y + pt2._y);
			}

			static CVPoint<T> operator- (const CVPoint<T>% pt1, const CVPoint<T>% pt2)
			{
				return CVPoint<T>(pt1._x - pt2._x, pt1._y - pt2._y);
			}

			virtual String^ ToString() override
			{
				return "CVPoint[" + _x.ToString() + ", " + _y.ToString() + "]";
			}

		private:
			T _x;
			T _y;
	};
}