#include "CppUnitTest.h"

#include "../CUFE-DataProject/collections/linked_list.h"

using namespace collections;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(LinkedListTests)
	{
	public:
		TEST_METHOD(Add)
		{
			LinkedList<int> ll;
			for (int i = 0; i < 5; i++) {
				ll.Add(i);
			}

			Assert::AreEqual(ll.GetLength(), 5);
		}

		TEST_METHOD(Remove)
		{
			LinkedList<int> ll;
			for (int i = 0; i < 5; i++) {
				ll.Add(i);
			}

			ll.Remove(1);
			ll.Remove(0);
			ll.Remove(1);
			ll.Remove(99999);

			Assert::AreEqual(ll.GetLength(), 3);
		}

		TEST_METHOD(Empty)
		{
			LinkedList<int> ll;
			for (int i = 0; i < 5; i++) {
				ll.Add(i);
			}

			for (int i = 0; i < 5; i++) {
				ll.Remove(i);
			}

			Assert::AreEqual(ll.IsEmpty(), true);
		}

		TEST_METHOD(GetItem)
		{
			LinkedList<int> ll;
			for (int i = 0; i < 5; i++) {
				ll.Add(i);
			}

			Assert::AreEqual(*ll[0], 0);
			Assert::AreEqual(*ll[4], 4);
			Assert::IsNull(ll[5]);
		}
	};
}
