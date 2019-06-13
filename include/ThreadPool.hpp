#include <future>
#include <mutex>
#include <memory>

#include "CrossQueue.hpp"
#include "Service.hpp"

#pragma once

namespace kpm::async::threadpool {

class LocalInstance {
public:
	LocalInstance();
	explicit LocalInstance(uint32_t servicesNumber);
	virtual ~LocalInstance();

	void post(CompletionHandler &&task);
	uint32_t activeServices();
	void stop();

private:
	void _createServices(uint32_t servicesNumber);

	std::vector<std::unique_ptr<Service>> _services;
	CrossQueue<CompletionHandler> _tasksQueue;
	std::condition_variable _condVar;
	std::mutex _guard;

	LocalInstance(const LocalInstance&) = delete;
	LocalInstance(LocalInstance&&) = default;
	LocalInstance &operator=(const LocalInstance&) = delete;
	LocalInstance &operator=(LocalInstance&&) = default;
};

}

namespace kpm::async::threadpool {
class GlobalInstance {
public:
	~GlobalInstance() {
		Instance.stop();
	}

	static void post(CompletionHandler &&task)
	{
		Instance.post(std::move(task));
	}

	static uint32_t activeServices()
	{
		return Instance.activeServices();
	}

	static void stop()
	{
		Instance.stop();
	}

private:
	static LocalInstance Instance;

	GlobalInstance() {}
};
}
