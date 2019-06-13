#include <mutex>
#include <future>
#include <thread>

#include "CrossQueue.hpp"
#include "CompletionHandler.hpp"

#pragma once

namespace async {

class Service {

public:
	Service(CrossQueue<CompletionHandler> *q, std::mutex *m, std::condition_variable *c);
	Service(Service &&s);
	Service &operator=(Service &&s);
	virtual ~Service();

	void run();
	void setStop();
	bool active() const;
	void launch();
	void tryJoin();

private:
	CrossQueue<CompletionHandler> *_q;
	std::mutex *_m;
	std::condition_variable *_c;
	std::atomic<bool> _isActive;
	std::atomic<bool> _stop;
	std::thread _t;

	Service() = delete;
	Service(const Service&) = delete;
	Service &operator=(const Service&) = delete;
};

}
