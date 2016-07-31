#pragma once

namespace CVStructures
{
	using namespace System;

	template <class K, class T>
	public value class KeyedPair
	{
		public:
			K key;
			T item;

			void set(K _key, T _item);
			void get(K% out_key, T% out_item);
		
			static bool operator== (KeyedPair<K, T> lhs, KeyedPair<K, T> rhs);
			static bool operator!= (KeyedPair<K, T> lhs, KeyedPair<K, T> rhs);
			static bool operator>= (KeyedPair<K, T> lhs, KeyedPair<K, T> rhs);
			static bool operator<= (KeyedPair<K, T> lhs, KeyedPair<K, T> rhs);
			static bool operator> (KeyedPair<K, T> lhs, KeyedPair<K, T> rhs);
			static bool operator< (KeyedPair<K, T> lhs, KeyedPair<K, T> rhs);

			static bool operator== (KeyedPair<K, T> lhs, K rhs);
			static bool operator!= (KeyedPair<K, T> lhs, K rhs);
			static bool operator>= (KeyedPair<K, T> lhs, K rhs);
			static bool operator<= (KeyedPair<K, T> lhs, K rhs);
			static bool operator> (KeyedPair<K, T> lhs, K rhs);
			static bool operator< (KeyedPair<K, T> lhs, K rhs);

			static bool operator== (K lhs, KeyedPair<K, T> rhs);
			static bool operator!= (K lhs, KeyedPair<K, T> rhs);
			static bool operator>= (K lhs, KeyedPair<K, T> rhs);
			static bool operator<= (K lhs, KeyedPair<K, T> rhs);
			static bool operator> (K lhs, KeyedPair<K, T> rhs);
			static bool operator< (K lhs, KeyedPair<K, T> rhs);
	};

	template <class K, class T>
	void KeyedPair<K, T>::set(K _key, T _item)
	{
		key = _key;
		item = _item;
	}

	template <class K, class T>
	void KeyedPair<K, T>::get(K% out_key, T% out_item)
	{
		out_key = key;
		out_item = item;
	}

	template <class K, class T>
	bool KeyedPair<K, T>::operator== (KeyedPair<K, T> lhs, KeyedPair<K, T> rhs)
	{ return (lhs.key == rhs.key); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator!= (KeyedPair<K, T> lhs, KeyedPair<K, T> rhs)
	{ return (lhs.key != rhs.key); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator>= (KeyedPair<K, T> lhs, KeyedPair<K, T> rhs)
	{ return (lhs.key >= rhs.key); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator<= (KeyedPair<K, T> lhs, KeyedPair<K, T> rhs)
	{ return (lhs.key <= rhs.key); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator> (KeyedPair<K, T> lhs, KeyedPair<K, T> rhs)
	{ return (lhs.key > rhs.key); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator< (KeyedPair<K, T> lhs, KeyedPair<K, T> rhs)
	{ return (lhs.key < rhs.key); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator== (KeyedPair<K, T> lhs, K rhs)
	{ return (lhs.key == rhs); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator!= (KeyedPair<K, T> lhs, K rhs)
	{ return (lhs.key != rhs); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator>= (KeyedPair<K, T> lhs, K rhs)
	{ return (lhs.key >= rhs); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator<= (KeyedPair<K, T> lhs, K rhs)
	{ return (lhs.key <= rhs); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator> (KeyedPair<K, T> lhs, K rhs)
	{ return (lhs.key > rhs); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator< (KeyedPair<K, T> lhs, K rhs)
	{ return (lhs.key < rhs); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator== (K lhs, KeyedPair<K, T> rhs)
	{ return (lhs == rhs.key); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator!= (K lhs, KeyedPair<K, T> rhs)
	{ return (lhs != rhs.key); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator>= (K lhs, KeyedPair<K, T> rhs)
	{ return (lhs >= rhs.key); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator<= (K lhs, KeyedPair<K, T> rhs)
	{ return (lhs <= rhs.key); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator> (K lhs, KeyedPair<K, T> rhs)
	{ return (lhs > rhs.key); }

	template <class K, class T>
	bool KeyedPair<K, T>::operator< (K lhs, KeyedPair<K, T> rhs)
	{ return (lhs < rhs.key); }
}