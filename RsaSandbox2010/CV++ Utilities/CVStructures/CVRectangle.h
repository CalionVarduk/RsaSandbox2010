#pragma once

#include "CVPoint.h"
#include "CVSize.h"

namespace CVStructures
{
	using namespace System;
	using namespace System::Drawing;

	template <class T>
	public value class CVRectangle
	{
		public:
			static const CVRectangle<T> Empty = CVRectangle<T>(0, 0, 0, 0);

			static CVRectangle<T> FromLTRB(T left, T top, T right, T bottom)
			{
				return CVRectangle<T>(left, top, right - left, bottom - top);
			}

			static CVRectangle<T> Union(const CVRectangle<T>% r1, const CVRectangle<T>% r2)
			{
				T left = (r1.Left < r2.Left) ? r1.Left : r2.Left;
				T right = (r1.Right > r2.Right) ? r1.Right : r2.Right;
				T top = (r1.Top < r2.Top) ? r1.Top : r2.Top;
				T bottom = (r1.Bottom > r2.Bottom) ? r1.Bottom : r2.Bottom;
				return FromLTRB(left, top, right, bottom);
			}

			static CVRectangle<T> Intersect(const CVRectangle<T>% r1, const CVRectangle<T>% r2)
			{
				T width1 = r1.Right - r2.Left;
				T width2 = r2.Right - r1.Left;
				T height1 = r1.Bottom - r2.Top;
				T height2 = r2.Bottom - r1.Top;

				T width = (width1 < width2) ? width1 : width2;
				T height = (height1 < height2) ? height1 : height2;

				if(width >= 0 && height >= 0) {
					T left = (r1.Left > r2.Left) ? r1.Left : r2.Left;
					T top = (r1.Top > r2.Top) ? r1.Top : r2.Top;
					return CVRectangle<T>(left, top, width, height);
				}
				return CVRectangle<T>::Empty;
			}

			property T Left {
				public: T get() { return _left; }
				public: void set(T value) { _left = value; }
			}

			property T Right {
				public: T get() { return _left + _width; }
				public: void set(T value) { _left = value - _width; }
			}

			property T CentreX {
				public: T get() { return _left + (_width / 2); }
				public: void set(T value) { _left = value - (_width / 2); }
			}

			property T Top {
				public: T get() { return _top; }
				public: void set(T value) { _top = value; }
			}

			property T Bottom {
				public: T get() { return _top + _height; }
				public: void set(T value) { _top = value - _height; }
			}

			property T CentreY {
				public: T get() { return _top + (_height / 2); }
				public: void set(T value) { _top = value - (_height / 2); }
			}

			property T Width {
				public: T get() { return _width; }
				public: void set(T value) { _width = value; }
			}

			property T Height {
				public: T get() { return _height; }
				public: void set(T value) { _height = value; }
			}

			property CVPoint<T> Location {
				public: CVPoint<T> get() { return CVPoint<T>(_left, _top); }
				public: void set(CVPoint<T> value)
						{
							Left = value.X;
							Top = value.Y;
						}
			}

			property CVSize<T> Size {
				public: CVSize<T> get() { return CVSize<T>(_width, _height); }
				public: void set(CVSize<T> value)
						{
							Width = value.Width;
							Height = value.Height;
						}
			}

			property bool IsEmpty {
				public: bool get() { return ((_left == 0) && (_top == 0) && (_width == 0) && (_height == 0)); }
			}

			CVRectangle(const CVPoint<T>% location, const CVSize<T>% size)
				: _left(location.X), _top(location.Y), _width(size.Width), _height(size.Height) {}

			CVRectangle(T left, T top, T width, T height)
				: _left(left), _top(top), _width(width), _height(height) {}

			void offset(T dx, T dy)
			{
				_left += dx;
				_top += dy;
			}

			bool contains(const CVRectangle<T>% rect)
			{
				return ((Left <= rect.Left && Right >= rect.Right) &&
						(Top <= rect.Top && Bottom >= rect.Bottom));
			}

			bool contains(const CVPoint<T>% pt)
			{
				return contains(pt.X, pt.Y);
			}

			bool contains(T x, T y)
			{
				return ((Left <= x && Right >= x) && (Top <= y && Bottom >= y));
			}

			bool intersectsWith(const CVRectangle<T>% rect)
			{
				return (Left <= rect.Right && Right >= rect.Left) &&
						(Top <= rect.Bottom && Bottom >= rect.Top);
			}

			void inflate(T width, T height)
			{
				_width += width;
				_height += height;
			}

			static bool operator== (const CVRectangle<T>% r1, const CVRectangle<T>% r2)
			{
				return (r1._left == r2._left && r1._top == r2._top && r1._width == r2._width && r1._height == r2._height);
			}

			static bool operator!= (const CVRectangle<T>% r1, const CVRectangle<T>% r2)
			{
				return (r1._left != r2._left || r1._top != r2._top || r1._width != r2._width || r1._height != r2._height);
			}

			virtual String^ ToString() override
			{
				return "CVRectangle[" + _left.ToString() + ", " + _top.ToString() + ", " + _width.ToString() + ", " + _height.ToString() + "]";
			}

		private:
			T _left;
			T _top;
			T _width;
			T _height;
	};
}