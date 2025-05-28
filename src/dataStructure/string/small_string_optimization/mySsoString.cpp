
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <iostream>


class SmallString {
public:
    // Default Constructors
    SmallString() noexcept : size_(0), is_small_(true) {
        storage_.small[0] = '\0';
    }

    SmallString(const char* s) {
        size_t len = std::strlen(s);
        if (len <= small_capacity) {
            init_small(s, len);
        } else {
            init_large(s, len);
        }
    }

    SmallString(const SmallString& other) {
        size_ = other.size_;
        is_small_ = other.is_small_;
        if (other.is_small_) {
            std::memcpy(storage_.small, other.storage_.small, size_ + 1);
        } else {
            storage_.large.capacity = other.storage_.large.capacity;
            storage_.large.data = new char[storage_.large.capacity + 1];
            std::memcpy(storage_.large.data, other.storage_.large.data, size_ + 1);
        }
    }

    SmallString(SmallString&& other) noexcept {
        size_ = other.size_;
        is_small_ = other.is_small_;
        if (other.is_small_) {
            std::memcpy(storage_.small, other.storage_.small, size_ + 1);
        } else {
            storage_.large.data = other.storage_.large.data;
            storage_.large.capacity = other.storage_.large.capacity;
            other.storage_.large.data = nullptr;
            other.storage_.large.capacity = 0;
        }
        other.size_ = 0;
        other.is_small_ = true;
        other.storage_.small[0] = '\0';
    }

    SmallString& operator=(const SmallString& other) {
        if (this == &other) return *this;
        this->~SmallString();
        new (this) SmallString(other);
        return *this;
    }

    SmallString& operator=(SmallString&& other) noexcept {
        if (this == &other) return *this;
        this->~SmallString();
        new (this) SmallString(std::move(other));
        return *this;
    }

    ~SmallString() {
        if (!is_small_ && storage_.large.data) {
            delete[] storage_.large.data;
        }
    }

    // Element access
    char& operator[](size_t pos) {
        if (pos >= size_) throw std::out_of_range("Index out of range");
        return is_small_ ? storage_.small[pos] : storage_.large.data[pos];
    }

    const char& operator[](size_t pos) const {
        if (pos >= size_) throw std::out_of_range("Index out of range");
        return is_small_ ? storage_.small[pos] : storage_.large.data[pos];
    }

    const char* c_str() const noexcept {
        return is_small_ ? storage_.small : storage_.large.data;
    }

    // Capacity and size
    size_t size() const noexcept { return size_; }

    size_t capacity() const noexcept {
        return is_small_ ? small_capacity : storage_.large.capacity;
    }

private:
    static constexpr size_t small_capacity = 15;
    size_t size_;
    bool is_small_;
    union Storage {
        char small[small_capacity + 1];
        struct {
            char* data;
            size_t capacity;
        } large;
        Storage() {}
        ~Storage() {}
    } storage_;

    void init_small(const char* s, size_t len) noexcept {
        size_ = len;
        is_small_ = true;
        std::memcpy(storage_.small, s, len + 1);
    }

    void init_large(const char* s, size_t len) {
        size_ = len;
        is_small_ = false;
        storage_.large.capacity = len;
        storage_.large.data = new char[len + 1];
        std::memcpy(storage_.large.data, s, len + 1);
    }
};



int main() {
    // Example usage of MySsoString
    SmallString str("Hello");
    std::cout << str.c_str() << " (capacity: " << str.capacity() << ")\n";

    // Example usage:
    SmallString s("Hello, loooooooooooooooooooog String!x");
    std::cout << s.c_str() << " (capacity: " << s.capacity() << ")\n";
}

// g++ -std=c++20 -Wall mySsoString.cpp -o mySsoString.tsk