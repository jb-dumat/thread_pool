#include <queue>
#include <mutex>

#pragma once

namespace kpm::async {

template<typename T, typename Container = std::deque<T>, typename BaseType = std::queue<T>>
class CrossQueue : public std::queue<T> {
public:
	CrossQueue() = default;
	CrossQueue(const CrossQueue&) = default;
	CrossQueue(CrossQueue&&) = default;
	CrossQueue &operator=(const CrossQueue&) = default;
	CrossQueue &operator=(CrossQueue&&) = default;
	virtual ~CrossQueue() = default;

	void push(T &&data)
	{
		std::unique_lock<std::mutex> lock(_guard);
		static_cast<BaseType*>(this)->push(std::move(data));
	}

	void pop()
	{
		std::unique_lock<std::mutex> lock(_guard);
		static_cast<BaseType*>(this)->pop();
	}

	T &front()
	{
		std::unique_lock<std::mutex> lock(_guard);
		return static_cast<BaseType*>(this)->front();
	}

	bool empty()
	{
		std::unique_lock<std::mutex> lock(_guard);
		return static_cast<BaseType*>(this)->empty();
	}

	size_t size()
	{
		std::unique_lock<std::mutex> lock(_guard);
		return static_cast<BaseType*>(this)->size();
	}

private:
	std::mutex _guard;
};

}
