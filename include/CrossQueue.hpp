#include <queue>
#include <mutex>
#include <utility>

#pragma once

namespace async {

/**
 * A standard queue protected with mutex. 
 * Example:
 * 		Crossqueue<int> _crossQueue; // Constructs a crossQueue
 * 		_crossQueue.push(12); // The queue contains { 12 }
 * 		_crossQueue.front(); // returns 12
 * 		_crossQueue.pop(); // the queue is now empty.
 * 
 * The crossQueue inherits from std::queue. This allows to perform a low cost
 * convesion at compile time on the pointer instead of storing in class a std::queue.
 * Destructor is declared as virtual which permits inheritance.
 */
template<typename T, typename Container = std::deque<T>, typename Queue = std::queue<T>>
class CrossQueue : public std::queue<T> {
public:
	CrossQueue() = default;
	CrossQueue(const CrossQueue&) = default;
	CrossQueue(CrossQueue&&) = default;
	CrossQueue &operator=(const CrossQueue&) = default;
	CrossQueue &operator=(CrossQueue&&) = default;
	virtual ~CrossQueue() noexcept = default;

	/**
	 * Push a referenced variable into the queue.
	 * Example:
	 * 		CrossQueue<int> _crossQueue;
	 * 		std::string s("Hello World!");
	 * 		_crossQueue.push(s);
	 */
	void push(const T &data)
	{
		std::unique_lock<std::mutex> lock(_guard);
		static_cast<Queue*>(this)->push(data);
	}

	/**
	 * Push a moved variable into the queue.
	 * This might be usefull if you want to store a variable that hasn't an available
	 * copy constructor / operator such as std::packaged_task.
	 * Example:
	 * 		CrossQueue<std::string> _crossQueue;
	 * 		std::string s("Hello World!");
	 * 		_crossQueue.push(std::move(s));
	 */
	void push(T &&data)
	{
		std::lock_guard<std::mutex> lock(_guard);
		static_cast<Queue*>(this)->push(std::move(data));
	}

	/**
	 * Construct the object and push it into the queue.
	 * Example:
	 * 		struct vectorInt2D {
	 * 			A(int x, int y) _x(x), _y(y) {}
	 * 			int x;
	 * 			int y;
	 * 		};
	 * 		...
	 * 		CrossQueue<vectorInt2D> _crossQueue;
	 * 		_crossQueue.emplace(42, 84);
	 */
	template<typename... Args>
	void emplace(Args&&... args)
	{
		std::lock_guard<std::mutex> lock(_guard);
		static_cast<Queue*>(this)->emplace(std::forward<Args>(args)...);
	}

	/**
	 * Pop the oldest element of the queue following the FIFO principle.
	 * Example:
	 * 		CrossQueue<int> _crossQueue;
	 * 		_crossQueue.push(1);
	 * 		_crossQueue.push(2);
	 * 		// _crossQueue contains { 1, 2 }
	 * 		_crossQueue.pop();
	 * 		// _crossQueue now contains { 2 }
	 */
	void pop()
	{
		std::lock_guard<std::mutex> lock(_guard);
		static_cast<Queue*>(this)->pop();
	}

	/**
	 * Swap the content of two CrossQueues.
	 */
	void swap(CrossQueue<T> &queue) noexcept
	{
		std::lock_guard<std::mutex> lock(_guard);
		static_cast<Queue*>(this)->swap(queue);
	}

	/**
	 * Access the first element of the queue.
	 */
	T &front()
	{
		std::lock_guard<std::mutex> lock(_guard);
		return static_cast<Queue*>(this)->front();
	}

	/**
	 * Access the first element of the queue with const qualifiers.
	 */
	const T &front() const
	{
		std::lock_guard<std::mutex> lock(_guard);
		return static_cast<const Queue*>(this)->front();
	}

	/**
	 * Access the last element of the queue.
	 */
	T &back()
	{
		std::lock_guard<std::mutex> lock(_guard);
		return static_cast<Queue*>(this)->back();
	}

	/**
	 * Access the last element of the queue with const qualifiers.
	 */
	const T &back() const
	{
		std::lock_guard<std::mutex> lock(_guard);
		return static_cast<const Queue*>(this)->back();
	}

	/**
	 * Returns true if the queue is empty otherwise returns false.
	 */
	bool empty() const
	{
		std::lock_guard<std::mutex> lock(_guard);
		return static_cast<const Queue*>(this)->empty();
	}

	/**
	 * Returns the current size of the queue.
	 */
	size_t size() const
	{
		std::lock_guard<std::mutex> lock(_guard);
		return static_cast<const Queue*>(this)->size();
	}

private:
	/**
	 * The mutex used to protect the queue from concurrency.
	 */
	mutable std::mutex _guard;
};

}
