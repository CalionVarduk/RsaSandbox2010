#pragma once

namespace CVStructures
{
	using namespace System;
	using namespace System::Drawing;

	template <class T>
	public value class CVSize
	{
		public:
			static const CVSize<T> Empty = CVSize<T>(0, 0);

			property T Width {
				public: T get() { return _width; }
				public: void set(T value) { _width = value; }
			}

			property T Height {
				public: T get() { return _height; }
				public: void set(T value) { _height = value; }
			}

			property bool IsEmpty {
				public: bool get() { return ((_width == 0) && (_height == 0)); }
			}

			CVSize(T width, T height) : _width(width), _height(height) {}

			static bool operator== (const CVSize<T>% sz1, const CVSize<T>% sz2)
			{
				return (sz1._width == sz2._width && sz1._height == sz2._height);
			}

			static bool operator!= (const CVSize<T>% sz1, const CVSize<T>% sz2)
			{
				return (sz1._width != sz2._width || sz1._height != sz2._height);
			}

			static CVSize<T> operator+ (const CVSize<T>% sz1, const CVSize<T>% sz2)
			{
				return CVSize<T>(sz1._width + sz2._width, sz1._height + sz2._height);
			}

			static CVSize<T> operator- (const CVSize<T>% sz1, const CVSize<T>% sz2)
			{
				return CVSize<T>(sz1._width - sz2._width, sz1._height - sz2._height);
			}

			virtual String^ ToString() override
			{
				return "CVSize[" + _width.ToString() + ", " + _height.ToString() + "]";
			}

		private:
			T _width;
			T _height;
	};
}