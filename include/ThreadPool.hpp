#include <mutex>
#include <memory>
#include <atomic>
#include <condition_variable>

#include "CrossQueue.hpp"
#include "CompletionHandler.hpp"
#include "Service.hpp"

#pragma once

namespace async::threadpool {

/**
 * class: LocalInstance
 * 
 * The class LocalInstances provides a local thread_pool to be used anywhere in your code.
 * It follows the RAII principle.
 * This thread_pool is also generic and support any type of tasks signatures. it means that your
 * functors (works/tasks) don't need to inherit from any interface. Just pass them and the
 * thread_pool will do the job.
 * Keeps in mind you shouldn't use a threadpool to execute tasks that never ends...
 */
class LocalInstance {
public:	
	/**
	 * Constructs a local threadpool. You must specify the number of
	 * services to build. You can use std::thread::hardware_concurrency
	 * if you are not sure of how many threads your systems can supports.
	 */
	explicit LocalInstance(uint32_t servicesNumber);

	/**
	 * Constructors
	 */
	LocalInstance(LocalInstance&&) = default;
	LocalInstance &operator=(LocalInstance&&) = default;
	
	/**
	 * Destructor
	 */
	virtual ~LocalInstance();

	/**
	 * Public method: makeServices
	 * Make a new service and adds it the the services table.
	 */
	void makeServices(uint32_t servicesNumber);
	
	/**
	 * Public method: post
	 * Post a new task into the queue that will feed the services.
	 */
	void post(CompletionHandler &&task);
	
	/**
	 * Public method: activeServices
	 * Returns how many services are currently iddles.
	 */
	uint32_t activeServices();
	
	/**
	 * Public method: stop
	 * Stops the thread_pool and waits for tasks to be done.
	 */
	void stop();

private:
	/**
	 * Private method: _createServices
	 */
	void _createServices(uint32_t servicesNumber);

	/**
	 * Attribute: _services
	 * Vector containing a unique_ptr of each services.
	 * Services are stored on the heap because of the fact they
	 * are not copiable. This also follows RAII principle.
	 */
	std::vector<std::unique_ptr<Service>> _services;

	/**
	 * Attribute: _tasksQueue
	 * The crossQueue used to feed services. It
	 * is protected with mutex to avoind concurrency
	 * issues.
	 */
	CrossQueue<CompletionHandler> _tasksQueue;

	/**
	 * Attribute: _condVar
	 * Condition variable used to organize access
	 * to the tasks queue and to stop services when needed.
	 */
	std::condition_variable _condVar;
	
	/**
	 * Attribute: _guard
	 * Mutex shared between all services
	 * and the thread_pool
	 */
	mutable std::mutex _guard;

	/**
	 * Deleted constructors
	 * ThreadPool is not intended (and cannot) be copied.
	 */
	LocalInstance(const LocalInstance&) = delete;
	LocalInstance &operator=(const LocalInstance&) = delete;
};

}

namespace async::threadpool {

/**
 * class GlobalInstance
 * The GlobalInstance provides a global thread_pool
 * described as a signleton pattern and can be used anywhere
 * in your application.
 */
class GlobalInstance {
public:
	/**
	 * Destructor
	 */
	~GlobalInstance() {
		Instance.stop();
	}

	/**
	 * Public method: post
	 * Call the LocalInstance 'post' method.
	 */
	static void post(CompletionHandler &&task)
	{
		Instance.post(std::move(task));
	}

	/**
	 * Public method: activeServices
	 * Call the LocalInstance 'activeServices' method.
	 */
	static uint32_t activeServices()
	{
		return Instance.activeServices();
	}

	/**
	 * Public method: stop
	 * Call the LocalInstance 'stop' method.
	 */
	static void stop()
	{
		Instance.stop();
	}

private:
	/**
	 * Attribute: Instance
	 * The thread_pool instance.
	 */
	static LocalInstance Instance;

	/**
	 * Constructor is private. Use LocalInstance if
	 * you need to build a local thread_pool.
	 */
	GlobalInstance() = default;
};
}
