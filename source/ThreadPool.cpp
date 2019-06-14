#include "ThreadPool.hpp"

namespace async::threadpool {

LocalInstance threadpool::GlobalInstance::Instance;

LocalInstance::LocalInstance()
{
	_createServices(std::thread::hardware_concurrency());
}

LocalInstance::LocalInstance(uint32_t servicesNumber)
{
	_createServices(servicesNumber);
}

LocalInstance::~LocalInstance()
{
	stop();
}

void LocalInstance::post(CompletionHandler &&task)
{
	std::lock_guard<std::mutex> lck(_guard);
	_tasksQueue.push(std::move(task));
	_condVar.notify_one();
}

uint32_t LocalInstance::activeServices()
{
	uint32_t count = 0;
	for (const auto &it : _services) {
		if (it->active())
			++count;
	}
	return count;
}

void LocalInstance::stop()
{
	for (auto &it : _services) {
		it->setStop();
		_condVar.notify_all();
		it->tryJoin();
	}
	_condVar.notify_all();
}

void LocalInstance::_createServices(uint32_t servicesNumber)
{
	for (unsigned int it = 0; it < servicesNumber; ++it) {
		_services.push_back(std::make_unique<Service>(&_tasksQueue, &_guard, &_condVar));
		_services.back()->launch();
	}
}

}
