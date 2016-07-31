#pragma once

namespace CVStructures
{
	using namespace System;

	template <class T>
	public ref class StackNode
	{
		public:
			property bool IsLast {
				public: bool get() { return (Next == nullptr); }
			}

			T Item;
			StackNode<T>^ Next;

			StackNode() : Item(), Next(nullptr) {}
			StackNode(T item) : Item(item), Next(nullptr) {}
			StackNode(T item, StackNode<T>^ next) : Item(item), Next(next) {}

			void setNext(T item)
			{
				Next = gcnew StackNode<T>(item);
			}
	};
}