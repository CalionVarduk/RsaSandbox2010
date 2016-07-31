#pragma once

namespace CVStructures
{
	using namespace System;
	using namespace System::Collections::Generic;

	template <class T>
	public ref class CVHeap
	{
		public:
			property Int32 Count {
				public: Int32 get() { return _items->Count; }
			}

			property bool IsEmpty {
				public: bool get() { return (Count == 0); }
			}

			property bool IsInMinMode {
				public: bool get() { return _inMinMode; }
			}

			property bool IsInMaxMode {
				public: bool get() { return !_inMinMode; }
			}

			CVHeap() : _items(gcnew List<T>()), _inMinMode(true) {}

			CVHeap(Int32 capacity) : _items(gcnew List<T>(capacity)), _inMinMode(true) {}

			void setMinMode()
			{
				if(IsInMaxMode) {
					_inMinMode = true;
					build(toArray());
				}
			}

			void setMaxMode()
			{
				if(IsInMinMode) {
					_inMinMode = false;
					build(toArray());
				}
			}

			void build(array<T>^ items)
			{
				clear();
				_items->AddRange(items);

				if(_inMinMode)
					for(Int32 i = (items->Length - 1) >> 1; i >= 0; --i)
						_fixDownMin(i);
				else
					for(Int32 i = (items->Length - 1) >> 1; i >= 0; --i)
						_fixDownMax(i);

			}

			void insert(T item)
			{
				_items->Add(item);
				(_inMinMode) ? _fixUpMin(Count - 1) : _fixUpMax(Count - 1);
			}

			T extract()
			{
				T item = _items[0];
				_items[0] = _items[Count - 1];
				_items->RemoveAt(Count - 1);
				(_inMinMode) ? _fixDownMin(0) : _fixDownMax(0);
				return item;
			}

			T peek()
			{
				return _items[0];
			}

			void clear()
			{
				_items->Clear();
			}

			array<T>^ toArray()
			{
				return _items->ToArray();
			}

		private:
			List<T>^ _items;
			bool _inMinMode;

			void _fixUpMax(Int32 i)
			{
				Int32 p = (i - 1) >> 1;
				while((i > 0) && (_items[i] > _items[p])) {
					_swap(i, p);
					i = p;
					p = (i - 1) >> 1;
				}
			}

			void _fixUpMin(Int32 i)
			{
				Int32 p = (i - 1) >> 1;
				while((i > 0) && (_items[i] < _items[p])) {
					_swap(i, p);
					i = p;
					p = (i - 1) >> 1;
				}
			}

			void _fixDownMax(Int32 i)
			{
				Int32 m, r, l = (i << 1) + 1;
				while(l < Count) {
					r = l + 1;
					m = (_items[l] > _items[i]) ? l : i;

					if(r < Count && (_items[r] > _items[m]))
						m = r;

					if(m != i) {
						_swap(i, m);
						i = m;
						l = (i << 1) + 1;
					}
					else break;
				}
			}

			void _fixDownMin(Int32 i)
			{
				Int32 m, r, l = (i << 1) + 1;
				while(l < Count) {
					r = l + 1;
					m = (_items[l] < _items[i]) ? l : i;

					if(r < Count && (_items[r] < _items[m]))
						m = r;

					if(m != i) {
						_swap(i, m);
						i = m;
						l = (i << 1) + 1;
					}
					else break;
				}
			}

			void _swap(Int32 i, Int32 j)
			{
				T tmp = _items[i];
				_items[i] = _items[j];
				_items[j] = tmp;
			}
	};
}