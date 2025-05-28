#include "uniquePtr.h"

int main() {
    

    // Example of Constructor get called.
    mynamespace::UniquePtr<int> ptr1(new int(42));

    // move assignment operator
    mynamespace::UniquePtr<int> ptr2 = std::move(ptr1); // Move ptr1 to ptr2


    mynamespace::UniquePtr<int> ptr3(new int(100));
    mynamespace::UniquePtr<int> ptr4(new int(200));
    ptr3 = std::move(ptr4); // Move ptr4 to ptr3

    //mynamespace::UniquePtr<int> ptr5 = ptr3; // This will not compile, copy is deleted
    // ptr2 = ptr3; // This will not compile, copy is deleted

    mynamespace::UniquePtr<int> ptr5(new int(42));
    mynamespace::UniquePtr<int> ptr6(new int(42));
    //using std::swap;
    mynamespace::swap(ptr5, ptr6); // Swap ptr5 and ptr6
}
// g++ -std=c++20 -Wall -I ./ uniquePtr.cpp -o uniquePtr.tsk