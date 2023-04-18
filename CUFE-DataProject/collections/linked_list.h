#pragma once

#include "list.h"

namespace collections {
	/// <summary>
	/// Linked list node
	/// </summary>
	template<typename T>
	struct LinkedListNode {
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

		LinkedListNode(T& val) {
			prev = next = 0;

			value = val;
		}
	};

	template<typename T>
	class LinkedList : public List<T> {
	private:
		int m_Count;

	protected:
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

		LinkedListNode<T>* GetNodeAtIndex(int idx) {
			if (idx >= m_Count || idx < 0) return 0;

			int len = 0;
			for (LinkedListNode<T>* node = m_Head; node; node = node->next) {
				if (len++ == idx) return node;
			}

			return 0;
		}

	public:
		/// <summary>
		/// Constructor, initialize head to 0
		/// </summary>
		LinkedList() : m_Count(0), m_Head(0), m_Tail(0) {
		}

		~LinkedList() {
			//clear list content upon deletion
			Clear();
		}

		/// <summary>
		/// Returns the head of the linked list
		/// </summary>
		LinkedListNode<T>* GetHead() {
			return m_Head;
		}

		/// <summary>
		/// Returns the tail of the linked list
		/// </summary>
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

			//decrement counter
			m_Count--;

			return true;
		}

		/// <summary>
		/// Adds an element to the list
		/// </summary>
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

			//increment counter
			m_Count++;
		}

		/// <summary>
		/// Inserts an element at the specified position
		/// </summary>
		virtual bool Insert(int pos, T val) override {
			if (pos > m_Count || pos < 0) return false;

			//if empty, just add
			if (m_Head == 0 || pos == m_Count) {
				Add(val);
				return true;
			}

			LinkedListNode<T>* node = new LinkedListNode<T>(val);
			LinkedListNode<T>* oldNode = GetNodeAtIndex(pos);

			node->prev = oldNode->prev;
			if (oldNode->prev != 0) {
				oldNode->prev->next = node;
			}

			oldNode->prev = node;
			node->next = oldNode;

			if (oldNode == m_Head) {
				m_Head = node;
			}

			m_Count++;

			return true;
		}

		/// <summary>
		/// Removes an element from the list
		/// </summary>
		virtual bool Remove(T val) override {
			//return false if empty
			if (m_Head == 0) return false;

			//find node with value
			auto node = GetNodeWithValue(val);

			//return false if node not found
			if (!node) return false;

			return DeleteNode(node);
		}

		/// <summary>
		/// Does the element exist?
		/// </summary>
		virtual bool Contains(T val) override {
			return GetNodeWithValue(val) != 0;
		}

		/// <summary>
		/// Is the list empty?
		/// </summary>
		virtual bool IsEmpty() override {
			return m_Head == 0;
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
			if (m_Head == 0) return;

			//keep deleting tail
			while (m_Tail) {
				DeleteNode(m_Tail);
			}
		}

		/// <summary>
		/// Accesses an item using an index
		/// </summary>
		virtual T* operator[](int idx) override {
			if (idx >= m_Count || idx < 0) return 0;

			LinkedListNode<T>* node = GetNodeAtIndex(idx);
			if (node != 0) {
				return &node->value;
			}

			return 0;
		}
	};
}