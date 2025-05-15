#include <iostream>
#include <future>
#include <chrono>

int factorial (std::string name, std::shared_future<int> sf) {
    int res = 1;
    int N = sf.get();
    for (int i =N; i>0 ; --i) {
        res*=i;
    }
    std::cout<<"From:"<< name<< " result is:" <<res << std::endl;
    return res;
}

int main() {
    std::promise<int>p;
    // std::future<int> f = p.get_future();
    // std::shared_future<int> sf = f.share();
    std::shared_future<int> sf = p.get_future();
    
    // for shared future, you can pass by value or by reference
    std::future<int> fu1 = std::async(std::launch::async, factorial, std::string("1"), sf);
    std::future<int> fu2 = std::async(std::launch::async, factorial, std::string("2"), sf);
    std::future<int> fu3 = std::async(std::launch::async, factorial, std::string("3"), sf);
    
    // do something
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    p.set_value(4);
    
    // if you really can not set the future, you break the pormise, you can set the error.
    // p.set_exception(std::make_exception_ptr(std::runtime_error("To err is human"))); 
    
    int x1 = fu1.get();
    int x2 = fu2.get();
    int x3 = fu3.get();
    std::cout << "get from child:" << x1 << std::endl;
    std::cout << "get from child:" << x2 << std::endl;
    std::cout << "get from child:" << x3 << std::endl;
    return 0;
}

//g++ -std=c++20 -Wall future_promise2.cpp -o future_promise2.tsk
