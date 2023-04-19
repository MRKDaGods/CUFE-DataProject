#pragma once

#include <memory>

#include "list.h"

namespace collections {
	template<typename T>
	class ArrayList : public List<T> {
	private:
		int m_Capacity;
		int m_Count;
		T* m_Buffer;

		void UpdateAllocations(int capacity) {
			if (m_Capacity == capacity || capacity < m_Capacity) return;

			T* buf = new T[capacity];
			memset(buf, 0, sizeof(T) * capacity);

			//copy from old buffer
			if (m_Buffer != 0) {
				memcpy(buf, m_Buffer, sizeof(T) * m_Capacity);

				//delete old buffer
				delete[] m_Buffer;
			}

			m_Buffer = buf;
			m_Capacity = capacity;
		}

		/// <summary>
		/// Increases capacity if needed
		/// </summary>
		void CheckCapacity() {
			//count is equal to capacity, we need more space
			if (m_Count == m_Capacity) {
				//allocate more space
				UpdateAllocations(m_Capacity * 2);
			}
		}

	public:
		ArrayList(int initialCapacity = 64) : m_Capacity(0), m_Count(0), m_Buffer(0) {
			UpdateAllocations(initialCapacity);
		}

		/// <summary>
		/// Adds an element to the list
		/// </summary>
		virtual void Add(T val) override {
			CheckCapacity();

			//add value
			m_Buffer[m_Count++] = val;
		}

		/// <summary>
		/// Inserts an element at the specified position
		/// </summary>
		virtual bool Insert(int pos, T val) override {
			if (pos > m_Count || pos < 0) return false;

			CheckCapacity();

			//if empty, just add
			if (m_Count == 0 || pos == m_Count) {
				Add(val);
				return true;
			}

			for (int i = m_Count; i > pos; i--) {
				m_Buffer[i] = m_Buffer[i - 1];
			}

			m_Buffer[pos] = val;

			m_Count++;

			return true;
		}

		/// <summary>
		/// Removes an element from the list
		/// </summary>
		virtual bool Remove(T val) override {
			int i;
			for (i = 0; i < m_Count; i++) {
				if (m_Buffer[i] == val) {
					//found val
					break;
				}
			}

			//not found
			if (i == m_Count) return false;

			//shift elements to the left
			for (; i < m_Count - 1; i++) {
				m_Buffer[i] = m_Buffer[i + 1];
			}

			//decrement counter
			m_Count--;

			return true;
		}

		/// <summary>
		/// Does the element exist?
		/// </summary>
		virtual bool Contains(T val) override {
			for (int i = 0; i < m_Count; i++) {
				if (m_Buffer[i] == val) return true;
			}

			return false;
		}

		/// <summary>
		/// Is the list empty?
		/// </summary>
		virtual bool IsEmpty() override {
			return m_Count == 0;
		}

		/// <summary>
		/// Returns the number of elements in a list
		/// </summary>
		virtual int GetLength() override {
			return m_Count;
		}

		/// <summary>
		/// Clears the list
		/// </summary>
		virtual void Clear() override {
			//just set count to 0
			m_Count = 0;
		}

		/// <summary>
		/// Accesses an item using an index
		/// </summary>
		virtual T* operator[](int idx) {
			if (idx >= m_Count || idx < 0) return 0;

			return &(m_Buffer[idx]);
		}

		/// <summary>
		/// Reserves memory in the array
		/// </summary>
		void Reserve(int capacity) {
			UpdateAllocations(capacity);
		}
	};
}