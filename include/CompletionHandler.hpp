#include <memory>
#include <utility>

#pragma once

namespace kpm::async {

class CompletionHandler {
public:
	CompletionHandler() = delete;

	template<typename Type, typename DerefType = typename std::decay<Type>::type>
	CompletionHandler(Type &&f)
		: _deleter([](void *ptr) { delete static_cast<DerefType*>(ptr); }),
		  _invoker([](void *ptr) { return (*static_cast<DerefType*>(ptr))(); } ),
		  _ptr(new DerefType(std::forward<Type>(f)), _deleter)
	{}

	template<typename Type, typename DerefType = typename std::decay<Type>::type>
	CompletionHandler(const Type &f)
		: _deleter([](void *ptr) { delete static_cast<DerefType*>(ptr); }),
		  _invoker([](void *ptr) { return (*static_cast<DerefType*>(ptr))(); } ),
		  _ptr(new DerefType(f), _deleter)
	{}

	CompletionHandler(const CompletionHandler&) = delete;
	CompletionHandler(CompletionHandler&&) = default;
	CompletionHandler &operator=(const CompletionHandler&) = delete;
	CompletionHandler &operator=(CompletionHandler&&) = default;
	~CompletionHandler() = default;

	void operator()()
	{
		(*_invoker)(_ptr.get());
	}

private:
	void (*_deleter)(void*) { nullptr };
	void (*_invoker)(void*) { nullptr };
	std::unique_ptr<void, void(*)(void*)> _ptr;
};

}
