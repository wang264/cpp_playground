#include <iostream>
#include <cstring>

class MyStruct{

public:
    union {
        char c;
        char* int_ptr;
    } data;
    bool is_char;
    
    ~MyStruct() {
        if (!is_char && data.int_ptr) {
            delete[] data.int_ptr;
        }
    }

    MyStruct() : data{0}, is_char(true) {}

    MyStruct(char c) {
        data.c = c;
        is_char = true;
    }

    MyStruct(int i){
        data.int_ptr = new char[sizeof(int)];
        std::memcpy(data.int_ptr, &i, sizeof(int));
        is_char = false;
    }

    MyStruct(const MyStruct& other) {
        is_char = other.is_char;
        if (is_char) {
            data.c = other.data.c;
        } else {
            data.int_ptr = new char[4];
            std::memcpy(data.int_ptr, other.data.int_ptr, sizeof(int));
        }
    }

    MyStruct(MyStruct&& other) noexcept {
        is_char = other.is_char;
        if (is_char) {
            data.c = other.data.c;
        } else {
            data.int_ptr = other.data.int_ptr;
            other.data.int_ptr = nullptr;
        }
    }

    MyStruct& operator=(const MyStruct& other) {
        if (this == &other) return *this;
        // Clean up existing data if needed
        if (!is_char && data.int_ptr) {
            delete[] data.int_ptr;
        }
        is_char = other.is_char;
        if (is_char) {
            data.c = other.data.c;
        } else {
            data.int_ptr = new char[sizeof(int)];
            std::memcpy(data.int_ptr, other.data.int_ptr, sizeof(int));
        }
        return *this;
    }

    char get_char() const {
        return data.c;
    }
    int get_int() const {
        int i;
        std::memcpy(&i, data.int_ptr, sizeof(int));
        return i;
    }
};

int main() {
    MyStruct myStruct('A');
    std::cout << "Character: " << myStruct.get_char() << std::endl;

    myStruct = MyStruct(42);
    std::cout << "Integer: " << myStruct.get_int() << std::endl;

    return 0;
}
// g++ -std=c++20 -Wall char_or_int.cpp -o char_or_int.tsk

