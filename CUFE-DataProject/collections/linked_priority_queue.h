#pragma once

#include "queue.h"
#include "linked_list.h"

namespace collections {
	/// <summary>
	/// Pri-Queue implemented using a LinkedList
	/// </summary>
	template<typename T, typename Comp>
	class LinkedPriorityQueue : public Queue<T> {
	protected:
		/// <summary>
		/// The underlying linked list
		/// </summary>
		LinkedList<T> m_LinkedList;

	public:
		/// <summary>
		/// Enqueues an element to the end of queue
		/// </summary>
		void Enqueue(T val) {
			//queue normally if empty
			if (m_LinkedList.IsEmpty()) {
				m_LinkedList.Add(val);
				return;
			}

			Comp c = Comp();
			//highest priority first
			int i;
			for (i = 0; i < m_LinkedList.GetLength(); i++) {
				if (c(val, *m_LinkedList[i])) {
					break;
				}
			}

			m_LinkedList.Insert(i, val);
		}

		/// <summary>
		/// Attempts to dequeue an element from the queue
		/// </summary>
		virtual bool Dequeue(T* val = 0) override {
			//cant dequeue if empty
			if (m_LinkedList.IsEmpty()) return false;

			//get value
			if (val) {
				*val = m_LinkedList.GetHead()->value;
			}

			//delete head
			return m_LinkedList.DeleteNode(m_LinkedList.GetHead());
		}

		/// <summary>
		/// Is the queue empty?
		/// </summary>
		virtual bool IsEmpty() override {
			//we are empty if the linked list is empty
			return m_LinkedList.IsEmpty();
		}

		/// <summary>
		/// Length of queue elements
		/// </summary>
		virtual int GetLength() override {
			return m_LinkedList.GetLength();
		}

		/// <summary>
		/// Attempts to peek at the beginning of the queue
		/// </summary>
		virtual bool Peek(T* val = 0) override {
			//false if empty
			if (m_LinkedList.IsEmpty()) return false;

			//look at head
			if (val) {
				*val = m_LinkedList.GetHead()->value;
			}

			return true;
		}

		/// <summary>
		/// Clears the queue
		/// </summary>
		virtual void Clear() override {
			//clear the linked list
			m_LinkedList.Clear();
		}
	};
}