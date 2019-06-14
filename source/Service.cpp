#include "Service.hpp"

#include <iostream>

namespace async {

Service::Service(CrossQueue<CompletionHandler> *q, std::mutex *m, std::condition_variable *c)
	: _q(q), _m(m), _c(c), _isActive(false), _stop(false)
{

}

Service::~Service()
{
	if (_t.joinable())
		_t.join();
}

void Service::_run()
{
	while (!_stop) {
		std::unique_lock<std::mutex> _guard(*_m);
		_c->wait(_guard, [&]() { return !_q->empty() || _stop == true; });
		if (_stop)
			return;
		else if (_q->empty())
			continue;
		CompletionHandler task = std::move(_q->front());
		_q->pop();
		_guard.unlock();

		_isActive = true;
		task();
		_isActive = false;
	}
}

void Service::setStop()
{
	_stop = true;
}

void Service::tryJoin()
{
	if (_t.joinable())
		_t.join();
}

bool Service::active() const
{
	return _isActive;
}

void Service::launch()
{
	_t = std::thread(&Service::_run, this);
}

}
