#pragma once

namespace collections {
	template<typename T>
	struct BinaryTreeNode {
		BinaryTreeNode<T>* left;
		BinaryTreeNode<T>* right;

		T value;
	};

	// This tree holds only unique values
	template<typename T>
	class BinaryTree {
	private:
		BinaryTreeNode<T>* m_Root;

		BinaryTreeNode<T>* FindNodeWithValue(BinaryTreeNode<T>* node, T& value) {
			if (node == 0) return 0;

			if (node->value == value) return node;

			BinaryTreeNode<T>* n = FindNodeWithValue(node->left, value);
			if (n) return n;

			return FindNodeWithValue(node->right, value);
		}

		BinaryTreeNode<T>* CreateNode(T value) {
			return new BinaryTreeNode<T> {
				0,
				0,
				value
			};
		}

	public:
		BinaryTree() : m_Root(0) {
		}

		~BinaryTree() {
			//since this BT is only used for forking, each Node will be a root and should delete itself
			if (m_Root) {
				delete m_Root;
			}
		}

		BinaryTreeNode<T>* GetRoot() {
			return m_Root;
		}

		// Inserts a value into the BinaryTree for the specified parent
		void Insert(T* parent, T value) {
			BinaryTreeNode<T>* node = CreateNode(value);
			if (!InsertForeign(parent, node)) {
				delete node;
			}
		}

		// Inserts a foreign node into this binary tree
		// The foreignNode may be the root of another binary tree
		bool InsertForeign(T* parent, BinaryTreeNode<T>* foreignNode, BinaryTreeNode<T>*** foreginAddress = 0) {
			if (foreignNode == 0) return false;

			if (parent == 0) {
				if (m_Root == 0) {
					m_Root = foreignNode;

					if (foreginAddress) {
						*foreginAddress = &m_Root;
					}

					return true;
				}

				return false;
			}

			BinaryTreeNode<T>* node = FindNodeWithValue(m_Root, *parent);
			if (node == 0 || (node->left && node->right)) return false;

			//insert anywhere.
			if (node->left == 0) {
				node->left = foreignNode;

				if (foreginAddress) {
					*foreginAddress = &node->left;
				}
			}
			else {
				node->right = foreignNode;

				if (foreginAddress) {
					*foreginAddress = &node->right;
				}
			}

			return true;
		}
	};
}