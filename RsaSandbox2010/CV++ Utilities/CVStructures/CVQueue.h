#pragma once

#include "QueueNode.h"

namespace CVStructures
{
	using namespace System;

	template <class T>
	public ref class CVQueue
	{
		public:
			property Int32 Count {
				public: Int32 get() { return _count; }
			}

			property bool IsEmpty {
				public: bool get() { return (_head == nullptr); }
			}

			CVQueue() : _head(nullptr), _tail(nullptr), _count(0) {}
			~CVQueue() { clear(); }

			void enqueue(T item)
			{
				if(_count++) {
					_tail->setNext(item);
					_tail = _tail->Next;
				}
				else _head = _tail = gcnew QueueNode<T>(item);
			}

			T first()
			{
				return _head->Item;
			}

			T last()
			{
				return _tail->Item;
			}

			T dequeue()
			{
				T item = _head->Item;
				if(--_count) {
					_head = _head->Next;
					_head->Prev = nullptr;
				}
				else _head = _tail = nullptr;
				return item;
			}

			T at(Int32 i)
			{
				QueueNode<T>^ current;
				if(i < (_count >> 1)) {
					current = _head;
					for(Int32 j = 0; j < i; ++j) current = current->Next;
				}
				else {
					current = _tail;
					for(Int32 j = _count - 1; j > i; --j) current = current->Prev;
				}
				return current->Item;
			}

			void clear()
			{
				while(!IsEmpty) dequeue();
			}

			array<T>^ toArray()
			{
				QueueNode<T>^ current = _head;
				array<T>^ data = gcnew array<T>(_count);

				for(Int32 i = 0; i < _count; ++i) {
					data[i] = current->Item;
					current = current->Next;
				}
				return data;
			}

		private:
			QueueNode<T>^ _head;
			QueueNode<T>^ _tail;
			Int32 _count;
	};
}