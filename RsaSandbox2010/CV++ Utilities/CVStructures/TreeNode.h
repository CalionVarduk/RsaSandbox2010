#pragma once

namespace CVStructures
{
	using namespace System;

	template <class T>
	public ref class TreeNode
	{
		public:
			property bool HasLeftChild {
				public: bool get() { return (Left != nullptr); }
			}

			property bool HasRightChild {
				public: bool get() { return (Right != nullptr); }
			}

			property bool HasParent {
				public: bool get() { return (Parent != nullptr); }
			}

			T Item;
			TreeNode<T>^ Left;
			TreeNode<T>^ Right;
			TreeNode<T>^ Parent;

			TreeNode() : Item(), Left(nullptr), Right(nullptr), Parent(nullptr) {}
			TreeNode(T item) : Item(item), Left(nullptr), Right(nullptr), Parent(nullptr) {}
			TreeNode(T item, TreeNode<T>^ left, TreeNode<T>^ right, TreeNode<T>^ parent)
				: Item(item), Left(left), Right(right), Parent(parent) {}

			void setLeftChild(T item)
			{
				Left = gcnew TreeNode<T>(item, nullptr, nullptr, this);
			}

			void setRightChild(T item)
			{
				Right = gcnew TreeNode<T>(item, nullptr, nullptr, this);
			}
	};
}