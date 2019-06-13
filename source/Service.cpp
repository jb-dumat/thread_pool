#include "Service.hpp"

#include <iostream>

namespace async {

Service::Service(CrossQueue<CompletionHandler> *q, std::mutex *m, std::condition_variable *c)
	: _q(q), _m(m), _c(c), _isActive(false), _stop(false)
{

}

Service::Service(Service &&s)
	: _q(std::move(s._q)), _m(std::move(s._m)), _c(std::move(s._c)),
	  _isActive(false), _stop(false),
	  _t(std::move(s._t))
{

}

Service &Service::operator=(Service &&s)
{
	_q = std::move(s._q);
	_m = std::move(s._m);
	_c = std::move(s._c);
	_t = std::move(s._t);
	_isActive = false;
	_stop = false;
	return *this;
}

Service::~Service()
{
	if (_t.joinable())
		_t.join();
}

void Service::run()
{
	while (!_stop) {
		{
			std::unique_lock<std::mutex> _guard(*_m);
			_c->wait(_guard, [&]() { return !_q->empty() || _stop == true; });
			if (_stop)
				return;
			else if (_q->empty())
				continue;
		}

		{
		std::unique_lock<std::mutex> _l(*_m);
		CompletionHandler task = std::move(_q->front());
		_q->pop();
		_isActive = true;
		task();
		_isActive = false;
		}
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
	_t = std::thread(&Service::run, this);
}

}
