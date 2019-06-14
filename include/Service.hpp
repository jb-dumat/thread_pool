#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

#include "CrossQueue.hpp"
#include "CompletionHandler.hpp"

#pragma once

namespace async {

/**
 * class: Service
 * The Service class is used by the threadpool but
 * have no consciousness of it. It owns a thread that
 * will perform all tasks got from the queue.
 * 
 * This class is not copiable and because the thread_pool
 * has no reasons to loose its ownership it is also not
 * movable.
 */
class Service {
public:
	/**
	 * Constructors and destructors.
	 */
	Service(CrossQueue<CompletionHandler> * const q, std::mutex * const m, std::condition_variable * const c);
	virtual ~Service();
	
	/**
	 * Public setter: setStop
	 * Set _stop to true
	 */
	void setStop();
	
	/**
	 * Public method: active
	 * Returns the current state of the service:
	 * working = true
	 * iddle = false
	 */
	bool active() const;

	/**
	 * Public method: launch
	 * Launch the thread owned by the
	 * service and calls its 'run' routine.
	 */
	void launch();

	/**
	 * Public method: tryJoin
	 * Try to join the intern thread.
	 * Nothing happens if the thread is
	 * not joinable.
	 */
	void tryJoin();

private:
	/**
	 * Private method: _run
	 * Run the services. This is an infinite loop.
	 * The right way to stop the service is to stop it
	 * directly from the thread_pool.
	 */
	void _run();

	/**
	 * Attribute: _q
	 * A pointer to the queue used to feed services.
	 */
	CrossQueue<CompletionHandler> * const _q;

	/**
	 * Attribute: _m
	 * A pointer to the mutex shared bewteen services.
	 */
	std::mutex * const _m;

	/**
	 * Attribute: _c
	 * A pointer to the condition variable shared between services.
	 */
	std::condition_variable * const _c;

	/**
	 * Attribute: _isActive
	 * A safe boolean used to know about the current service's state.
	 */
	std::atomic<bool> _isActive;
	
	/**
	 * Attribute: _stop
	 * A safe boolean used to stop the service. It is set from the tread_pool.
	 */
	std::atomic<bool> _stop;
	
	/**
	 * Attribute: thread
	 * A thread used to launch the worker.
	 */
	std::thread _t;

	/**
	 * Deleted constructors
	 */
	Service() = delete;
	Service(Service &&s) = delete;
	Service(const Service&) = delete;
	Service &operator=(const Service&) = delete;
	Service &operator=(Service &&s) = delete;
};

}
