#include <thread>
#include <future>
int factorial(int N) {
    int res = 1;
    for (int i =N; i>0 ; --i) {
        res*=i;
    }
    return res;
}

int main() {
    /* thread*/
    std::thread t1(factorial, 6);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // let the thread sleep until a time point
    std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now(); + std::chrono::milliseconds(200);
    std::this_thread::sleep_until(tp);

    /* Mutex*/
    std::timed_mutex mu;
    std::timed_mutex mu2;
    std::lock_guard<std::timed_mutex> locker(mu);
    std::unique_lock<std::timed_mutex> unique_locker(mu2); // lock and unlock multiple time, also can transfer ownership
    unique_locker.try_lock(); // try to lock, if not lock, return false
    unique_locker.unlock(); // unlock the mutex
    unique_locker.try_lock_for(std::chrono::milliseconds(200)); // try to lock for a time period
    unique_locker.try_lock_until(tp); // try to lock until a time point
    //std::lock(mu, std::defer_lock); // lock the mutex in a defer way, do not lock it immediately


    /* condition variable*/
    std::mutex mu3;
    std::unique_lock<std::mutex> unique_locker2(mu3); 
    std::condition_variable cond_var;
    cond_var.wait_for(unique_locker2, std::chrono::milliseconds(200)); // wait for a time period
    cond_var.wait_until(unique_locker2, tp); // wait until a time point

    /*Future and promise*/
    std::promise<int> p;
    std::future<int> f = p.get_future();
    int result = f.get();
    (void) result;
    f.wait(); // wait for the future to be ready
    f.wait_for(std::chrono::milliseconds(200)); // wait for a time period
    f.wait_until(tp); // wait until a time point

    /* async()*/
    std::future<int> fu = std::async(std::launch::async, factorial, 6);

    /* packaged task*/
    std::packaged_task<int(int)> pt(factorial); //is a class template that wraps a callable object and returns a future
    std::future<int> fu2 = pt.get_future();
    
    pt(6);
}
// g++ -std=c++20 -Wall review_timeconstrain.cpp -o review_timeconstrain.tsk