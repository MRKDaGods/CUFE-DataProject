#pragma once

#include "stack.h"
#include "linked_list.h"

namespace collections {
	template<typename T>
	class LinkedStack : public Stack<T> {
	private:
		/// <summary>
		/// The underlying linked list
		/// </summary>
		LinkedList<T> m_LinkedList;

	public:
		virtual void Push(T val) override {
			//add value to linked list
			m_LinkedList.Add(val);
		}

		virtual bool Pop(T* val = 0) override {
			if (m_LinkedList.IsEmpty()) return false;

			//tail is never null
			LinkedListNode<T>* tail = m_LinkedList.GetTail();

			if (val) {
				*val = tail->value;
			}

			return m_LinkedList.DeleteNode(tail);
		}

		virtual bool Peek(T& val) override {
			if (m_LinkedList.IsEmpty()) return false;

			//get value of tail
			val = m_LinkedList.GetTail()->value;
			return true;
		}

		virtual bool IsEmpty() override {
			return m_LinkedList.IsEmpty();
		}
	};
}