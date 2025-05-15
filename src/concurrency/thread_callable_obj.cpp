#include <iostream>
#include <thread>

class A {
    public:
        void f(int x, char c) {
            std::cout << "A::f(int, char)" << std::endl;
        }
        long g(double d) {
            std::cout << "A::g(double)" << std::endl;
            return 0;
        }
        void operator() (int N) {
            std::cout << "A::operator()" <<N << std::endl;
        }
};

void foo (int x) {
    std::cout << "foo(int)" << std::endl;
}

int main() {
    A a;
    std::thread t1(a, 6); // copy of a
    std::thread t2(std::ref(a), 6); // a() in a different thread
    std::thread t3(A(), 6); // temp A, also a copy
    std::thread t4([] (int x) {std::cout<< x*x << std::endl;},6); // lambda
    std::thread t5(foo, 6); // function pointer
    std::thread t6(&A::f, a, 6, 'a'); // member function pointer, make a copy of a in a different thread
    std::thread t7(&A::f, &a, 6, 'a'); // member function pointer, do not make a copy of a in a different thread
    
    std::thread t8(std::move(a),6); // you can also move. 

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    // std::thread t7(&A::g, a, 6.0); // error: no matching function for call to ‘A::g(int)’
    return 0;
}


//g++ -std=c++20 -Wall thread_callable_obj.cpp -o thread_callable_obj.tsk
