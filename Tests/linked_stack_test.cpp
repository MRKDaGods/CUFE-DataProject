#include "CppUnitTest.h"

#include "../CUFE-DataProject/collections/linked_stack.h"

using namespace collections;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(LinkedStackTests)
	{
	public:
		TEST_METHOD(Push)
		{
			LinkedStack<int> s;
			for (int i = 0; i < 5; i++) {
				s.Push(i);
			}

			int len = 0;
			while (s.Pop()) {
				len++;
			}

			Assert::AreEqual(len , 5);
		}

		TEST_METHOD(Pop)
		{
			LinkedStack<int> s;
			for (int i = 0; i < 5; i++) {
				s.Push(i);
			}

			for (int i = 4; i >= 0; i--) {
				int c;
				s.Pop(&c);

				Assert::AreEqual(c, i);
			}
		}

		TEST_METHOD(Peek)
		{
			LinkedStack<int> s;
			for (int i = 0; i < 5; i++) {
				s.Push(i);
			}

			int e;
			Assert::IsTrue(s.Peek(e));
			Assert::AreEqual(e, 4);

			while (s.Pop()) {}

			Assert::IsFalse(s.Peek(e));
		}

		TEST_METHOD(Empty)
		{
			LinkedStack<int> s;
			for (int i = 0; i < 5; i++) {
				s.Push(i);
			}

			Assert::IsFalse(s.IsEmpty());

			while (s.Pop()) { }

			Assert::IsTrue(s.IsEmpty());
		}
	};
}
