#include <memory>
#include <utility>

#pragma once

namespace async {

/**
 * Class: CompletionHandler
 * 
 * Wraps a functor (function object). The completion handler supports
 * any signature type, rvalue and references.
 * The CompletionHandler doesn't support copy and is not intended to be copied.
 * 
 * The class works as follow:
 * Type is deduced implicitly in the constructor, then references/pointers are removed and
 * a new object is built into a void unique_ptr.
 * The operator() calls an invoker wich execute the functor.
 * The CompletionHandler supports any kind of function or functions objects.
 * Because it is intended to be used by the thread_pool, it doesn't support arguments or placeholders
 * so you'll have to use std::bind and pass it to the CompletionHandler.
 */
class CompletionHandler {
public:

	/**
	 * Construct a completion handler which wraps the functor.
	 * Constructor allows implicit conversion so you don't need to explicitly call it.
	 * If you do so, keep in mind that the class isn't copiable and must be passed by using std::move.
	 */
	template<typename Type, typename DerefType = typename std::decay<Type>::type>
	CompletionHandler(Type &&f)
		: _deleter([](void *ptr) { delete static_cast<DerefType*>(ptr); }),
		  _invoker([](void *ptr) { (*static_cast<DerefType*>(ptr))(); } ),
		  _ptr(new DerefType(std::forward<Type>(f)), _deleter)
	{}

	/**
	 * Default constructors and destructor
	 */
	CompletionHandler(CompletionHandler&& c) = default;
	CompletionHandler &operator=(CompletionHandler&& c) = default;
	~CompletionHandler() = default;

	/**
	 * Operator: operator()()
	 * The completion handler wraps a functor.
	 * To call it you simply have to use the operator()()
	 */
	void operator()()
	{
		(*_invoker)(_ptr.get());
	}

private:
	/**
	 * Attribute: _deleter
	 * Function pointer passed as deleter to unique_ptr
	 */
	void (*_deleter)(void*) { nullptr };
	
	/**
	 * Attribute: _invoker
	 * Function pointer used to invoke the functor
	 */
	void (*_invoker)(void*) { nullptr };

	/**
	 * Attribute: _ptr
	 * Unique_ptr used to store the functor on the heap.
	 */
	std::unique_ptr<void, void(*)(void*)> _ptr;

	/**
	 * Deleted constructors.
	 */
	CompletionHandler() = delete;
	CompletionHandler(const CompletionHandler&) = delete;
	CompletionHandler &operator=(const CompletionHandler&) = delete;
};

}
