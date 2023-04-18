#include "CppUnitTest.h"

#include "../CUFE-DataProject/collections/linked_priority_queue.h"

using namespace collections;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(LinkedPriorityQueueTests)
	{
	public:
		TEST_METHOD(Enqueue)
		{
			LinkedPriorityQueue<int, _STD greater<int>> q;
			for (int i = 0; i < 5; i++) {
				q.Enqueue(i);
			}

			//check length
			Assert::AreEqual(q.GetLength(), 5);

			int peeked;
			Assert::IsTrue(q.Peek(&peeked));

			//peek value must be 4
			Assert::AreEqual(peeked, 4);
		}

		TEST_METHOD(Dequeue)
		{
			LinkedPriorityQueue<int, _STD greater<int>> q;
			for (int i = 0; i < 5; i++) {
				q.Enqueue(i);
			}

			int v;
			Assert::IsTrue(q.Dequeue(&v));
			Assert::AreEqual(v, 4);

			Assert::AreEqual(q.GetLength(), 4);

			for (int i = 0; i < 4; i++) {
				q.Dequeue();
			}

			Assert::AreEqual(q.GetLength(), 0);
			Assert::IsFalse(q.Dequeue());
		}

		TEST_METHOD(IsEmpty)
		{
			LinkedPriorityQueue<int, _STD greater<int>> q;

			Assert::IsTrue(q.IsEmpty());

			q.Enqueue(0);

			Assert::IsFalse(q.IsEmpty());

			q.Dequeue();

			Assert::IsTrue(q.IsEmpty());
		}

		TEST_METHOD(GetLength)
		{
			LinkedPriorityQueue<int, _STD greater<int>> q;

			Assert::AreEqual(q.GetLength(), 0);

			q.Enqueue(0);

			Assert::AreEqual(q.GetLength(), 1);

			for (int i = 0; i < 5; i++) {
				q.Dequeue();
			}

			Assert::AreEqual(q.GetLength(), 0);
		}

		TEST_METHOD(Peek)
		{
			LinkedPriorityQueue<int, _STD greater<int>> q;

			Assert::IsFalse(q.Peek());

			q.Enqueue(0);

			int v;
			Assert::IsTrue(q.Peek(&v));
			Assert::AreEqual(v, 0);

			q.Dequeue();
			Assert::IsFalse(q.Peek());
		}

		TEST_METHOD(Clear)
		{
			LinkedPriorityQueue<int, _STD greater<int>> q;

			Assert::AreEqual(q.GetLength(), 0);

			for (int i = 0; i < 10; i++) {
				q.Enqueue(i);
			}

			q.Clear();

			Assert::AreEqual(q.GetLength(), 0);
		}
	};
}
