#pragma once

namespace CVStructures
{
	using namespace System;

	template <class T>
	public ref class QueueNode
	{
		public:
			property bool IsLast {
				public: bool get() { return (Next == nullptr); }
			}

			property bool IsFirst {
				public: bool get() { return (Prev == nullptr); }
			}

			T Item;
			QueueNode<T>^ Next;
			QueueNode<T>^ Prev;

			QueueNode() : Item(), Next(nullptr), Prev(nullptr) {}
			QueueNode(T item) : Item(item), Next(nullptr), Prev(nullptr) {}
			QueueNode(T item, QueueNode<T>^ next, QueueNode<T>^ prev) : Item(item), Next(next), Prev(prev) {}

			void setNext(T item)
			{
				Next = gcnew QueueNode<T>(item, nullptr, this);
			}

			void setPrev(T item)
			{
				Prev = gcnew QueueNode<T>(item, this, nullptr);
			}
	};
}