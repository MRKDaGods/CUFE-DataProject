#pragma once

namespace collections {
	/// <summary>
	/// Abstract list interface
	/// </summary>
	template<typename T>
	class List {
	public:
		/// <summary>
		/// Adds an element to the list
		/// </summary>
		virtual void Add(T val) abstract;

		/// <summary>
		/// Removes an element from the list
		/// </summary>
		virtual bool Remove(T val) abstract;

		/// <summary>
		/// Is the list empty?
		/// </summary>
		virtual bool IsEmpty() abstract;

		/// <summary>
		/// Returns the number of elements in a list
		/// </summary>
		virtual int GetLength() abstract;

		/// <summary>
		/// Clears the list
		/// </summary>
		virtual void Clear() abstract;

		/// <summary>
		/// Accesses an item using an index
		/// </summary>
		virtual T* operator[](int idx) abstract;
	};
}