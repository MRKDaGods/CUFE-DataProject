#pragma once

#include "list.h"

namespace collections {
	/// <summary>
	/// Linked list node
	/// </summary>
	template<typename T>
	struct LinkedListNode {
		LinkedListNode(T& val) {
			prev = next = 0;

			value = val;
		}

		/// <summary>
		/// Previous node
		/// </summary>
		LinkedListNode<T>* prev;

		/// <summary>
		/// Next node
		/// </summary>
		LinkedListNode<T>* next;

		/// <summary>
		/// Node value
		/// </summary>
		T value;
	};

	template<typename T>
	class LinkedList : public List<T> {
	private:
		/// <summary>
		/// Linked list head
		/// </summary>
		LinkedListNode<T>* m_Head;

		/// <summary>
		/// Linked list tail
		/// </summary>
		LinkedListNode<T>* m_Tail;

		/// <summary>
		/// Returns a node if exists with the given value
		/// </summary>
		LinkedListNode<T>* GetNodeWithValue(T& val) {
			if (m_Head == 0) return 0;
			
			//quick check
			if (m_Head->value == val) return m_Head;
			if (m_Tail->value == val) return m_Tail;

			//not head, not tail, start traversal
			LinkedListNode<T>* node = m_Head;
			while (node = node->next) {
				if (node->value == val) {
					return node;
				}
			}

			return 0;
		}

	public:
		/// <summary>
		/// Constructor, initialize head to 0
		/// </summary>
		LinkedList() : m_Head(0), m_Tail(0) {
		}

		~LinkedList() {
			//clear list content upon deletion
			Clear();
		}

		LinkedListNode<T>* GetTail() {
			return m_Tail;
		}

		/// <summary>
		/// Deletes a linked node
		/// </summary>
		bool DeleteNode(const LinkedListNode<T>* node) {
			if (node == 0 || m_Head == 0) return false;

			//update prev
			if (node->next) {
				node->next->prev = node->prev;
			}

			//update next
			if (node->prev) {
				node->prev->next = node->next;
			}

			//update tail incase of deletion
			if (node == m_Tail) {
				m_Tail = node->prev;

				if (m_Tail) {
					m_Tail->next = 0;
				}
			}

			//update head incase of deletion
			if (node == m_Head) {
				m_Head = node->next;
			}

			//delete ptr
			delete node;

			return true;
		}

		virtual void Add(T val) override {
			auto node = new LinkedListNode<T>(val);

			//if head is null
			if (m_Head == 0) {
				//set both head and tail to the new node
				m_Head = node;
				m_Tail = node;
			}
			else {
				//set prev to current tail
				node->prev = m_Tail;

				//set next to the new node
				m_Tail->next = node;

				//new node is now the tail
				m_Tail = node;
			}
		}

		virtual bool Remove(T val) override {
			//return false if empty
			if (m_Head == 0) return false;

			//find node with value
			auto node = GetNodeWithValue(val);

			//return false if node not found
			if (!node) return false;

			return DeleteNode(node);
		}

		virtual bool IsEmpty() override {
			return m_Head == 0;
		}

		virtual int GetLength() override {
			if (m_Head == 0) return 0;

			int len = 0;
			for (LinkedListNode<T>* node = m_Head; node; node = node->next) {
				len++;
			}

			return len;
		}

		virtual void Clear() override {
			if (m_Head == 0) return;

			//keep deleting tail
			while (m_Tail) {
				DeleteNode(m_Tail);
			}
		}

		virtual T* operator[](int idx) override {
			//no checks, user should make sure that idx < length and >= 0
			int len = 0;
			for (LinkedListNode<T>* node = m_Head; node; node = node->next) {
				if (len == idx) return &node->value;

				len++;
			}

			return 0;
		}
	};
}