#include <queue>
#include <mutex>
#include <utility>

#pragma once

namespace async {

template<typename T, typename Container = std::deque<T>, typename BaseType = std::queue<T>>
class CrossQueue : public std::queue<T> {
public:
	CrossQueue() = default;
	CrossQueue(const CrossQueue&) = default;
	CrossQueue(CrossQueue&&) = default;
	CrossQueue &operator=(const CrossQueue&) = default;
	CrossQueue &operator=(CrossQueue&&) = default;
	virtual ~CrossQueue() noexcept = default;

	void push(const T &data)
	{
		std::unique_lock<std::mutex> lock(_guard);
		static_cast<BaseType*>(this)->push(data);
	}

	void push(T &&data)
	{
		std::unique_lock<std::mutex> lock(_guard);
		static_cast<BaseType*>(this)->push(std::move(data));
	}

	template<typename... Args>
	void emplace(Args&&... args)
	{
		std::unique_lock<std::mutex> lock(_guard);
		static_cast<BaseType*>(this)->emplace(std::forward<Args>(args)...);
	}

	void pop()
	{
		std::unique_lock<std::mutex> lock(_guard);
		static_cast<BaseType*>(this)->pop();
	}

	void swap(CrossQueue<T> &queue) noexcept
	{
		std::unique_lock<std::mutex> lock(_guard);
		static_cast<BaseType*>(this)->swap(queue);
	}

	T &front()
	{
		std::unique_lock<std::mutex> lock(_guard);
		return static_cast<BaseType*>(this)->front();
	}

	const T &front() const
	{
		std::unique_lock<std::mutex> lock(_guard);
		return static_cast<const BaseType*>(this)->front();
	}

	T &back()
	{
		std::unique_lock<std::mutex> lock(_guard);
		return static_cast<BaseType*>(this)->back();
	}

	const T &back() const
	{
		std::unique_lock<std::mutex> lock(_guard);
		return static_cast<const BaseType*>(this)->back();
	}

	bool empty() const
	{
		std::unique_lock<std::mutex> lock(_guard);
		return static_cast<const BaseType*>(this)->empty();
	}

	size_t size() const
	{
		std::unique_lock<std::mutex> lock(_guard);
		return static_cast<const BaseType*>(this)->size();
	}

private:
	mutable std::mutex _guard;
};

}
