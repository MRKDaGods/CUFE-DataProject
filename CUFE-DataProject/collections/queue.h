#pragma once

namespace collections {
	/// <summary>
	/// Abstract queue interface
	/// </summary>
	template<typename T>
	class Queue {
	public:
		/// <summary>
		/// Enqueues an element to the end of queue
		/// </summary>
		virtual void Enqueue(T val) abstract;

		/// <summary>
		/// Attempts to dequeue an element from the queue
		/// </summary>
		virtual bool Dequeue(T* val = 0) abstract;

		/// <summary>
		/// Is the queue empty?
		/// </summary>
		virtual bool IsEmpty() abstract;

		/// <summary>
		/// Length of queue elements
		/// </summary>
		virtual int GetLength() abstract;

		/// <summary>
		/// Attempts to peek at the beginning of the queue
		/// </summary>
		virtual bool Peek(T* val = 0) abstract;

		/// <summary>
		/// Clears the queue
		/// </summary>
		virtual void Clear() abstract;
	};
}