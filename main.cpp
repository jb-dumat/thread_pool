#include <iostream>
#include <string>
#include <functional>

#include "ThreadPool.hpp"

static std::mutex IO_MUTEX;

template<typename... Args>
static void COUT(Args&&... args)
{
    std::unique_lock<std::mutex> lock(IO_MUTEX);
    (void)std::initializer_list<int> { ((std::cout << args << " "), 0)... };
    std::cout << std::endl;
}

static void SLEEP(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

//#define DEBUG
#ifdef DEBUG

#else

static void v()
{
    COUT("no return fct");
}

static int f() {
    return 84;
}

class A {
public:
    A() = default;
    ~A() = default;

    void foo(const std::string &s) {
        COUT("public method with arg:", s); 
    }
};

int main(int, char *[])
{
    // Using local instance of the threadpool
    async::threadpool::LocalInstance pool;

    // Posting a lambda
    pool.post([]() { COUT("lambda: Hello World!"); });

    // Posting a packaged_task and get future
    std::packaged_task<int()> pt(f);
    std::future<int> promise = pt.get_future();

    pool.post(std::move(pt));
    promise.wait();

    COUT("function and future:", promise.get());

    // Posting binded public method with arg(s)
    A a;
    std::string s("bar");
    pool.post(std::bind(&A::foo, a, s));

    pool.post(v);

    // Using global instance of the threadpool
    async::threadpool::GlobalInstance::post([]() { COUT("lambda using global instance"); });
    async::threadpool::GlobalInstance::post(v);
    async::threadpool::GlobalInstance::post(f);

    pool.stop();
    async::threadpool::GlobalInstance::stop();

    return 0;
}

#endif