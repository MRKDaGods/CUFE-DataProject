#pragma once

namespace collections 
{
	template<typename T>
	class Stack
	{
	public:
		/// <summary>
		/// Pushes an element to the top of the stack
		/// </summary>
		virtual void Push(T val) abstract;

		/// <summary>
		/// Pops the element at the top of the stack
		/// </summary>
		virtual bool Pop(T* val = 0) abstract;

		/// <summary>
		/// Is the stack empty?
		/// </summary>
		virtual bool IsEmpty() abstract;
	};
}