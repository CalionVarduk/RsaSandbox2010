#pragma once

#include "TreeNode.h"

namespace CVStructures
{
	using namespace System;
	using namespace System::Collections::Generic;

	template <class T>
	public ref class BSTree
	{
		public:
#pragma region Properties
			property Int32 Count {
				public: Int32 get() { return _count; }
			}

			property bool IsEmpty {
				public: bool get() { return (_root == nullptr); }
			}

			property T Root {
				public: T get() { return _root->Item; }
			}

			property T Min {
				public: T get()
						{
							TreeNode<T>^ current = _root;
							while(current->HasLeftChild) current = current->Left;
							return current->Item;
						}
			}

			property T Max {
				public: T get()
						{
							TreeNode<T>^ current = _root;
							while(current->HasRightChild) current = current->Right;
							return current->Item;
						}
			}

			property Int32 Height {
				public: Int32 get() { return _levelOrder(_root, true)->Count - 1; }
			}

			property array<T>^ PreOrder {
				public: array<T>^ get()
						{
							Int32 i = 0;
							array<T>^ data = gcnew array<T>(_count);
							_recursivePreOrder(data, i, _root);
							return data;
						}
			}

			property array<T>^ InOrder {
				public: array<T>^ get()
						{
							Int32 i = 0;
							array<T>^ data = gcnew array<T>(_count);
							_recursiveInOrder(data, i, _root);
							return data;
						}
			}

			property array<T>^ PostOrder {
				public: array<T>^ get()
						{
							Int32 i = 0;
							array<T>^ data = gcnew array<T>(_count);
							_recursivePostOrder(data, i, _root);
							return data;
						}
			}

			property array<T>^ PreOrderInverse {
				public: array<T>^ get()
						{
							Int32 i = 0;
							array<T>^ data = gcnew array<T>(_count);
							_recursivePreOrderInverse(data, i, _root);
							return data;
						}
			}

			property array<T>^ InOrderInverse {
				public: array<T>^ get()
						{
							Int32 i = 0;
							array<T>^ data = gcnew array<T>(_count);
							_recursiveInOrderInverse(data, i, _root);
							return data;
						}
			}

			property array<T>^ PostOrderInverse {
				public: array<T>^ get()
						{
							Int32 i = 0;
							array<T>^ data = gcnew array<T>(_count);
							_recursivePostOrderInverse(data, i, _root);
							return data;
						}
			}

			property array<T>^ LevelOrderLeftToRight {
				public: array<T>^ get()
						{ return _convertLevelOrderOutcome(_levelOrder(_root, true)); }
			}

			property array<T>^ LevelOrderRightToLeft {
				public: array<T>^ get()
						{ return _convertLevelOrderOutcome(_levelOrder(_root, false)); }
			}
#pragma endregion

			BSTree() : _root(nullptr), _count(0) {}
			~BSTree() { clear(); }

			BSTree<T>^ cloneSubTree(T rootItem);

			bool insert(T item);
			bool remove(T item);
			bool replace(T oldItem, T newItem);
			bool exists(T item);

			List<T>^ getRange(T min, T max);
			List<T>^ getRange(T item, Int32% nPredecessors, Int32% nSuccessors);

			T getPredecessor(T item);
			T getSuccessor(T item);

			Int32 getDepth(T item);

			void clear();

		protected:
			TreeNode<T>^ _root;
			Int32 _count;

			void _insertToEmpty(T item);
			bool _insertToNonEmpty(T item);
			bool _findAndRemove(T item);
			
			void _insertAsLeftChild(T item, TreeNode<T>^ parent);
			void _insertAsRightChild(T item, TreeNode<T>^ parent);

			void _removeNode(TreeNode<T>^ node, bool lastSearchMoveRight);
			void _removeNodeWithTwoChildren(TreeNode<T>^ node);
			void _removeNodeWithOneChildLeft(TreeNode<T>^ node, bool isLastSearchMoveRight);
			void _removeNodeWithOneChildRight(TreeNode<T>^ node, bool isLastSearchMoveRight);
			void _removeNodeWithNoChildren(TreeNode<T>^ node, bool isLastSearchMoveRight);

			bool _searchForItemsParent(T item, TreeNode<T>^% OUT_node, bool% OUT_lastSearchMoveRight);
			bool _searchForItem(T item, TreeNode<T>^% OUT_node, bool% OUT_lastSearchMoveRight);

			TreeNode<T>^ _findNode(T item);
			TreeNode<T>^ _findPredecessor(T item);
			TreeNode<T>^ _findSuccessor(T item);

			TreeNode<T>^ _findPredecessorOfNode(TreeNode<T>^ node);
			TreeNode<T>^ _findPredecessorWithinChildren(TreeNode<T>^ node);
			TreeNode<T>^ _findPredecessorWithinParents(TreeNode<T>^ node);

			TreeNode<T>^ _findSuccessorOfNode(TreeNode<T>^ node);
			TreeNode<T>^ _findSuccessorWithinChildren(TreeNode<T>^ node);
			TreeNode<T>^ _findSuccessorWithinParents(TreeNode<T>^ node);

			void _recursivePreOrder(array<T>^% OUT_data, Int32% iData, TreeNode<T>^ current);
			void _recursiveInOrder(array<T>^% OUT_data, Int32% iData, TreeNode<T>^ current);
			void _recursivePostOrder(array<T>^% OUT_data, Int32% iData, TreeNode<T>^ current);
			void _recursivePreOrderInverse(array<T>^% OUT_data, Int32% iData, TreeNode<T>^ current);
			void _recursiveInOrderInverse(array<T>^% OUT_data, Int32% iData, TreeNode<T>^ current);
			void _recursivePostOrderInverse(array<T>^% OUT_data, Int32% iData, TreeNode<T>^ current);

			List<List<TreeNode<T>^>^>^ _levelOrder(TreeNode<T>^ current, bool leftToRight);
			void _levelOrderLeftToRight(List<List<TreeNode<T>^>^>^ orderData);
			void _levelOrderRightToLeft(List<List<TreeNode<T>^>^>^ orderData);
			static array<T>^ _convertLevelOrderOutcome(List<List<TreeNode<T>^>^>^ orderData);
	};

	template <class T>
	BSTree<T>^ BSTree<T>::cloneSubTree(T rootItem)
	{
		BSTree<T>^ tree = gcnew BSTree();
		TreeNode<T>^ node = _findNode(rootItem);

		if(node != nullptr) {
			List<List<TreeNode<T>^>^>^ order = _levelOrder(node, true);
			for(Int32 i = 0; i < order->Count; ++i) {
				List<TreeNode<T>^>^ level = order[i];
				for(Int32 j = 0; j < level->Count; ++j)
					tree->insert(level[j]->Item);
			}
		}
		return tree;
	}

	template <class T>
	bool BSTree<T>::insert(T item)
	{
		if(IsEmpty) {
			_insertToEmpty(item);
			return true;
		}
		return _insertToNonEmpty(item);
	}

	template <class T>
	bool BSTree<T>::remove(T item)
	{
		return (IsEmpty) ? false : _findAndRemove(item);
	}

	template <class T>
	bool BSTree<T>::replace(T oldItem, T newItem)
	{
		if(remove(oldItem)) {
			insert(newItem);
			return true;
		}
		return false;
	}

	template <class T>
	bool BSTree<T>::exists(T item)
	{
		return (_findNode(item) != nullptr);
	}

	template <class T>
	List<T>^ BSTree<T>::getRange(T min, T max)
	{
		if(min <= max && !IsEmpty) {
			bool islastSearchMoveRight;
			TreeNode<T>^ node = _root;
			if(_searchForItemsParent(min, node, islastSearchMoveRight) && islastSearchMoveRight)
				node = _findSuccessorOfNode(node);

			List<T>^ range = gcnew List<T>();
			while(node != nullptr) {
				if(node->Item <= max) range->Add(node->Item);
				else break;
				node = _findSuccessorOfNode(node);
			}
			return range;
		}
		return gcnew List<T>();
	}

	template <class T>
	List<T>^ BSTree<T>::getRange(T item, Int32% nPredecessors, Int32% nSuccessors)
	{
		if(!IsEmpty) {
			Int32 count = nPredecessors + nSuccessors;
			List<T>^ range = gcnew List<T>(((count < 0) ? count : 0) + 1);

			bool islastSearchMoveRight;
			TreeNode<T>^ node = _root;
			TreeNode<T>^ midNode;
			if(_searchForItemsParent(item, node, islastSearchMoveRight) && islastSearchMoveRight) {
				midNode = node;
				for(Int32 i = 0; i < nPredecessors; ++i) {
					range->Add(node->Item);
					node = _findPredecessorOfNode(node);
					if(node == nullptr) break;
				}
				nPredecessors = range->Count;
				range->Reverse();
				node = midNode;
				for(Int32 i = 0; i < nSuccessors; ++i) {
					node = _findSuccessorOfNode(node);
					if(node == nullptr) break;
					range->Add(node->Item);
				}
				nSuccessors = range->Count - nPredecessors;
			}
			else {
				if(node->Item != item) {
					midNode = node;
					for(Int32 i = 0; i < nPredecessors; ++i) {
						node = _findPredecessorOfNode(node);
						if(node == nullptr) break;
						range->Add(node->Item);
					}
					nPredecessors = range->Count;
					range->Reverse();
					node = midNode;
					for(Int32 i = 0; i < nSuccessors; ++i) {
						range->Add(node->Item);
						node = _findSuccessorOfNode(node);
						if(node == nullptr) break;
					}
					nSuccessors = range->Count - nPredecessors;
				}
				else {
					midNode = node;
					for(Int32 i = 0; i < nPredecessors; ++i) {
						node = _findPredecessorOfNode(node);
						if(node == nullptr) break;
						range->Add(node->Item);
					}
					nPredecessors = range->Count;
					range->Reverse();
					range->Add(midNode->Item);
					node = midNode;
					for(Int32 i = 0; i < nSuccessors; ++i) {
						node = _findSuccessorOfNode(node);
						if(node == nullptr) break;
						range->Add(node->Item);
					}
					nSuccessors = range->Count - nPredecessors - 1;
				}
			}
			return range;
		}
		nPredecessors = nSuccessors = 0;
		return gcnew List<T>();
	}

	template <class T>
	T BSTree<T>::getPredecessor(T item)
	{
		TreeNode<T>^ predecessor = _findPredecessor(item);
		return (predecessor != nullptr) ? predecessor->Item : item;
	}

	template <class T>
	T BSTree<T>::getSuccessor(T item)
	{
		TreeNode<T>^ successor = _findSuccessor(item);
		return (successor != nullptr) ? successor->Item : item;
	}

	template <class T>
	Int32 BSTree<T>::getDepth(T item)
	{
		TreeNode<T>^ current = _root;
		Int32 depth = 0;
		while(current != nullptr) {
			if(item < current->Item)  current = current->Left;
			else if(item > current->Item)  current = current->Right;
			else return depth;
			++depth;
		}
		return -1;
	}

	template <class T>
	void BSTree<T>::clear()
	{
		while(!IsEmpty) _removeNode(_root, false);
	}

	template <class T>
	void BSTree<T>::_insertToEmpty(T item)
	{
		++_count;
		_root = gcnew TreeNode<T>(item);
	}

	template <class T>
	bool BSTree<T>::_insertToNonEmpty(T item)
	{
		bool isLastSearchMoveRight;
		TreeNode<T>^ current = _root;

		if(_searchForItemsParent(item, current, isLastSearchMoveRight)) {
			if(isLastSearchMoveRight) _insertAsRightChild(item, current);
			else _insertAsLeftChild(item, current);
			return true;
		}
		return false;
	}

	template <class T>
	bool BSTree<T>::_findAndRemove(T item)
	{
		bool isLastSearchMoveRight;
		TreeNode<T>^ current = _root;

		if(_searchForItem(item, current, isLastSearchMoveRight)) {
			_removeNode(current, isLastSearchMoveRight);
			return true;
		}
		return false;
	}

	template <class T>
	void BSTree<T>::_insertAsLeftChild(T item, TreeNode<T>^ parent)
	{
		++_count;
		parent->setLeftChild(item);
	}

	template <class T>
	void BSTree<T>::_insertAsRightChild(T item, TreeNode<T>^ parent)
	{
		++_count;
		parent->setRightChild(item);
	}

	template <class T>
	void BSTree<T>::_removeNode(TreeNode<T>^ node, bool lastSearchMoveRight)
	{
		--_count;
		if(node->HasLeftChild) {
			if(node->HasRightChild) _removeNodeWithTwoChildren(node);
			else _removeNodeWithOneChildLeft(node, lastSearchMoveRight);
		}
		else if(node->HasRightChild) _removeNodeWithOneChildRight(node, lastSearchMoveRight);
		else _removeNodeWithNoChildren(node, lastSearchMoveRight);
	}

	template <class T>
	void BSTree<T>::_removeNodeWithTwoChildren(TreeNode<T>^ node)
	{
		TreeNode<T>^ successor = _findSuccessorOfNode(node);
		node->Item = successor->Item;

		if(successor->HasRightChild) {
			successor->Right->Parent = successor->Parent;
			if(successor->Parent->Left == successor)
				successor->Parent->Left = successor->Right;
			else successor->Parent->Right = successor->Right;
		}
		else if(successor->Parent->Left == successor)
			successor->Parent->Left = nullptr;
		else successor->Parent->Right = nullptr;
	}

	template <class T>
	void BSTree<T>::_removeNodeWithOneChildLeft(TreeNode<T>^ node, bool isLastSearchMoveRight)
	{
		node->Left->Parent = node->Parent;
		if(node->HasParent)
			((isLastSearchMoveRight) ? node->Parent->Right : node->Parent->Left) = node->Left;
		else _root = node->Left;
	}

	template <class T>
	void BSTree<T>::_removeNodeWithOneChildRight(TreeNode<T>^ node, bool isLastSearchMoveRight)
	{
		node->Right->Parent = node->Parent;
		if(node->HasParent)
			((isLastSearchMoveRight) ? node->Parent->Right : node->Parent->Left) = node->Right;
		else _root = node->Right;
	}

	template <class T>
	void BSTree<T>::_removeNodeWithNoChildren(TreeNode<T>^ node, bool isLastSearchMoveRight)
	{
		if(node->HasParent) {
			TreeNode<T>::goUp(node);
			((isLastSearchMoveRight) ? node->Right : node->Left) = nullptr;
		}
		else _root = nullptr;
	}

	template <class T>
	bool BSTree<T>::_searchForItemsParent(T item, TreeNode<T>^% OUT_node, bool% OUT_lastSearchMoveRight)
	{
		while(true) {
			if(item < OUT_node->Item) {
				if(OUT_node->HasLeftChild) OUT_node = OUT_node->Left;
				else {
					OUT_lastSearchMoveRight = false;
					return true;
				}
			}
			else if(item > OUT_node->Item) {
				if(OUT_node->HasRightChild) OUT_node = OUT_node->Right;
				else {
					OUT_lastSearchMoveRight = true;
					return true;
				}
			}
			else return false;
		}
	}

	template <class T>
	bool BSTree<T>::_searchForItem(T item, TreeNode<T>^% OUT_node, bool% OUT_lastSearchMoveRight)
	{
		while(OUT_node != nullptr) {
			if(item < OUT_node->Item) {
				OUT_lastSearchMoveRight = false;
				TreeNode<T>::goLeft(OUT_node);
			}
			else if(item > OUT_node->Item) {
				OUT_lastSearchMoveRight = true;
				TreeNode<T>::goRight(OUT_node);
			}
			else return true;
		}
		return false;
	}
			
	template <class T>
	TreeNode<T>^ BSTree<T>::_findNode(T item)
	{
		TreeNode<T>^ current = _root;
		while(current != nullptr) {
			if(item < current->Item) current = current->Left;
			else if(item > current->Item) current = current->Right;
			else return current;
		}
		return current;
	}

	template <class T>
	TreeNode<T>^ BSTree<T>::_findPredecessor(T item)
	{
		bool isLastSearchMoveRight;
		TreeNode<T>^ current = _root;
		return (_searchForItemsParent(item, current, isLastSearchMoveRight) && isLastSearchMoveRight) ?
															current : _findPredecessorOfNode(current);
	}

	template <class T>
	TreeNode<T>^ BSTree<T>::_findSuccessor(T item)
	{
		bool isLastSearchMoveRight;
		TreeNode<T>^ current = _root;
		return (_searchForItemsParent(item, current, isLastSearchMoveRight) && !isLastSearchMoveRight) ?
															current : _findSuccessorOfNode(current);
	}

	template <class T>
	TreeNode<T>^ BSTree<T>::_findPredecessorOfNode(TreeNode<T>^ node)
	{
		return (node->HasLeftChild) ? _findPredecessorWithinChildren(node) : _findPredecessorWithinParents(node);
	}

	template <class T>
	TreeNode<T>^ BSTree<T>::_findPredecessorWithinChildren(TreeNode<T>^ node)
	{
		node = node->Left;
		while(node->HasRightChild) node = node->Right;
		return node;
	}

	template <class T>
	TreeNode<T>^ BSTree<T>::_findPredecessorWithinParents(TreeNode<T>^ node)
	{
		TreeNode<T>^ predecessor = node->Parent;
		while(predecessor != nullptr && predecessor->Item > node->Item)
			predecessor = predecessor->Parent;
		return predecessor;
	}

	template <class T>
	TreeNode<T>^ BSTree<T>::_findSuccessorOfNode(TreeNode<T>^ node)
	{
		return (node->HasRightChild) ? _findSuccessorWithinChildren(node) : _findSuccessorWithinParents(node);
	}

	template <class T>
	TreeNode<T>^ BSTree<T>::_findSuccessorWithinChildren(TreeNode<T>^ node)
	{
		node = node->Right;
		while(node->HasLeftChild) node = node->Left;
		return node;
	}

	template <class T>
	TreeNode<T>^ BSTree<T>::_findSuccessorWithinParents(TreeNode<T>^ node)
	{
		TreeNode<T>^ successor = node->Parent;
		while(successor != nullptr && successor->Item < node->Item)
			successor = successor->Parent;
		return successor;
	}

	template <class T>
	void BSTree<T>::_recursivePreOrder(array<T>^% OUT_data, Int32% iData, TreeNode<T>^ current)
	{
		if(current == nullptr) return;
		OUT_data[iData++] = current->Item;
		_recursivePreOrder(OUT_data, iData, current->Left);
		_recursivePreOrder(OUT_data, iData, current->Right);
	}

	template <class T>
	void BSTree<T>::_recursiveInOrder(array<T>^% OUT_data, Int32% iData, TreeNode<T>^ current)
	{
		if(current == nullptr) return;
		_recursiveInOrder(OUT_data, iData, current->Left);
		OUT_data[iData++] = current->Item;
		_recursiveInOrder(OUT_data, iData, current->Right);
	}

	template <class T>
	void BSTree<T>::_recursivePostOrder(array<T>^% OUT_data, Int32% iData, TreeNode<T>^ current)
	{
		if(current == nullptr) return;
		_recursivePostOrder(OUT_data, iData, current->Left);
		_recursivePostOrder(OUT_data, iData, current->Right);
		OUT_data[iData++] = current->Item;
	}

	template <class T>
	void BSTree<T>::_recursivePreOrderInverse(array<T>^% OUT_data, Int32% iData, TreeNode<T>^ current)
	{
		if(current == nullptr) return;
		OUT_data[iData++] = current->Item;
		_recursivePreOrder(OUT_data, iData, current->Right);
		_recursivePreOrder(OUT_data, iData, current->Left);
	}

	template <class T>
	void BSTree<T>::_recursiveInOrderInverse(array<T>^% OUT_data, Int32% iData, TreeNode<T>^ current)
	{
		if(current == nullptr) return;
		_recursiveInOrder(OUT_data, iData, current->Right);
		OUT_data[iData++] = current->Item;
		_recursiveInOrder(OUT_data, iData, current->Left);
	}

	template <class T>
	void BSTree<T>::_recursivePostOrderInverse(array<T>^% OUT_data, Int32% iData, TreeNode<T>^ current)
	{
		if(current == nullptr) return;
		_recursivePostOrder(OUT_data, iData, current->Right);
		_recursivePostOrder(OUT_data, iData, current->Left);
		OUT_data[iData++] = current->Item;
	}

	template <class T>
	List<List<TreeNode<T>^>^>^ BSTree<T>::_levelOrder(TreeNode<T>^ current, bool leftToRight)
	{
		List<List<TreeNode<T>^>^>^ orderData = gcnew List<List<TreeNode<T>^>^>();
		if(current != nullptr) {
			orderData->Add(gcnew List<TreeNode<T>^>());
			orderData[0]->Add(current);
			if(leftToRight) _levelOrderLeftToRight(orderData);
			else _levelOrderRightToLeft(orderData);
		}
		return orderData;
	}

	template <class T>
	void BSTree<T>::_levelOrderLeftToRight(List<List<TreeNode<T>^>^>^ orderData)
	{
		Int32 prevReadCount = 0;
		Int32 readCount = 1;
		Int32 iLevel = 0;
		List<TreeNode<T>^>^ prevLevel = orderData[iLevel];
		List<TreeNode<T>^>^ level;

		while(prevReadCount != readCount) {
			Int32 iNode = 0;
			prevReadCount = readCount;
			orderData->Add(gcnew List<TreeNode<T>^>());
			level = orderData[++iLevel];

			while(iNode < prevLevel->Count) {
				if(prevLevel[iNode]->HasLeftChild)
					level->Add(prevLevel[iNode]->Left);

				if(prevLevel[iNode]->HasRightChild)
					level->Add(prevLevel[iNode]->Right);

				++iNode;
			}
			readCount += level->Count;
			prevLevel = level;
		}
	}

	template <class T>
	void BSTree<T>::_levelOrderRightToLeft(List<List<TreeNode<T>^>^>^ orderData)
	{
		Int32 prevReadCount = 0;
		Int32 readCount = 1;
		Int32 iLevel = 0;
		List<TreeNode<T>^>^ prevLevel = orderData[iLevel];
		List<TreeNode<T>^>^ level;

		while(prevReadCount != readCount) {
			Int32 iNode = 0;
			prevReadCount = readCount;
			orderData->Add(gcnew List<TreeNode<T>^>());
			level = orderData[++iLevel];

			while(iNode < prevLevel->Count) {
				if(prevLevel[iNode]->HasRightChild)
					level->Add(prevLevel[iNode]->Right);

				if(prevLevel[iNode]->HasLeftChild)
					level->Add(prevLevel[iNode]->Left);

				++iNode;
			}
			readCount += level->Count;
			prevLevel = level;
		}
	}

	template <class T>
	array<T>^ BSTree<T>::_convertLevelOrderOutcome(List<List<TreeNode<T>^>^>^ orderData)
	{
		Int32 nodeCount = 0;
		for(Int32 i = 0; i < orderData->Count; ++i)
			nodeCount += orderData[i]->Count;

		array<T>^ data = gcnew array<T>(nodeCount);
		for(Int32 i = 0, k = 0; i < orderData->Count; ++i) {
			List<TreeNode<T>^>^ levelData = orderData[i];
			for(Int32 j = 0; j < levelData->Count; ++j, ++k)
				data[k] = levelData[j]->Item;
		}
		return data;
	}
}