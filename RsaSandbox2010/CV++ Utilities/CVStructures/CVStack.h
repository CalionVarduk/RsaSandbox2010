#pragma once

#include "StackNode.h"

namespace CVStructures
{
	using namespace System;

	template <class T>
	public ref class CVStack
	{
		public:
			property Int32 Count {
				public: Int32 get() { return _count; }
			}

			property bool IsEmpty {
				public: bool get() { return (_root == nullptr); }
			}

			CVStack() : _root(nullptr), _count(0) {}
			~CVStack() { clear(); }

			void push(T item)
			{
				++_count;
				_root = gcnew StackNode<T>(item, _root);
			}

			T top()
			{
				return _root->Item;
			}

			T pop()
			{
				--_count;
				T item = _root->Item;
				_root = _root->Next;
				return item;
			}

			T at(Int32 i)
			{
				StackNode<T>^ current = _root;
				for(Int32 j = 0; j < i; ++j) current = current->Next;
				return current->Item;
			}

			void clear()
			{
				while(!IsEmpty) pop();
			}

			array<T>^ toArray()
			{
				StackNode<T>^ current = _root;
				array<T>^ data = gcnew array<T>(_count);

				for(Int32 i = 0; i < _count; ++i) {
					data[i] = current->Item;
					current = current->Next;
				}
				return data;
			}

		private:
			StackNode<T>^ _root;
			Int32 _count;
	};
}