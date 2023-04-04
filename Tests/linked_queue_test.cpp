#include "CppUnitTest.h"

#include "../CUFE-DataProject/collections/linked_queue.h"

using namespace collections;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(LinkedQueueTests)
	{
	public:
		TEST_METHOD(Enqueue)
		{
			LinkedQueue<int> q;
			for (int i = 0; i < 5; i++) {
				q.Enqueue(i);
			}

			//check length
			Assert::AreEqual(q.GetLength(), 5);

			int peeked;
			Assert::IsTrue(q.Peek(&peeked));

			//peek value must be 0
			Assert::AreEqual(peeked, 0);
		}

		TEST_METHOD(Dequeue)
		{
			LinkedQueue<int> q;
			for (int i = 0; i < 5; i++) {
				q.Enqueue(i);
			}

			int v;
			Assert::IsTrue(q.Dequeue(&v));
			Assert::AreEqual(v, 0);

			Assert::AreEqual(q.GetLength(), 4);

			for (int i = 0; i < 4; i++) {
				q.Dequeue();
			}

			Assert::AreEqual(q.GetLength(), 0);
			Assert::IsFalse(q.Dequeue());
		}

		TEST_METHOD(IsEmpty)
		{
			LinkedQueue<int> q;

			Assert::IsTrue(q.IsEmpty());

			q.Enqueue(0);

			Assert::IsFalse(q.IsEmpty());

			q.Dequeue();

			Assert::IsTrue(q.IsEmpty());
		}

		TEST_METHOD(GetLength)
		{
			LinkedQueue<int> q;

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
			LinkedQueue<int> q;

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
			LinkedQueue<int> q;

			Assert::AreEqual(q.GetLength(), 0);

			for (int i = 0; i < 10; i++) {
				q.Enqueue(i);
			}

			q.Clear();

			Assert::AreEqual(q.GetLength(), 0);
		}
	};
}
