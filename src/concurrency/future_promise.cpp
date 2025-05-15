// my_program.cpp
#include <iostream>
#include <future>
#include <chrono>

int factorial (std::future<int>& f) {
    int res = 1;
    int N = f.get();
    for (int i =N; i>0 ; --i) {
        res*=i;
    }
    std::cout<< "result is:" <<res << std::endl;
    return res;
}

int main() {
    int x;
    std::promise<int>p;
    // std::promise<int> p2 = p; // promise can not be copy. can only be moved
    std::promise<int> p2 = std::move(p);
    std::future<int> f = p.get_future();
    std::future<int> fu = std::async(std::launch::async, factorial, std::ref(f));
    // do something
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    p.set_value(4);
    
    // if you really can not set the future, you break the pormise, you can set the error.
    // p.set_exception(std::make_exception_ptr(std::runtime_error("To err is human"))); 
    
    x = fu.get();
    std::cout << "get from child:" << x << std::endl;
    return 0;
}

//g++ -std=c++20 -Wall future_promise.cpp -o future_promise.tsk
