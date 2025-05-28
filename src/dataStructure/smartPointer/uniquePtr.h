
#ifndef UNIQUE_PTR
#define UNIQUE_PTR

#include <iostream>
#include <utility>   // for std::exchange, std::swap

namespace mynamespace {
template <typename U>
struct DefaultDeleter {
    void operator()(U* ptr) const {
        std::cout << "DefaultDeleter called" << std::endl;
        delete ptr;
    }
};

template <typename T, typename Deleter = DefaultDeleter<T>>
class UniquePtr {
    public:
        // Constructor
        explicit UniquePtr(T* ptr = nullptr, Deleter d = Deleter{}) : _ptr(ptr), _deleter(d) {
            std::cout<< "UniquePtr Ctor"<< "["<< *_ptr << "]"<< std::endl;
        }
        // Destructor
        ~UniquePtr() {
            std::cout<< "UniquePtr Dtor"<<std::endl;
            _deleter(_ptr);
        }
        // copy constructor (deleted)
        UniquePtr(const UniquePtr& other) = delete;
        // assignment operator (deleted)
        UniquePtr& operator=(const UniquePtr& other) = delete;
        // move constructor
        UniquePtr(UniquePtr&& other) noexcept : _ptr(other._ptr), _deleter(std::move(other._deleter)) {
            std::cout<< "UniquePtr Move Ctor"<<std::endl;
            other._ptr = nullptr; // Transfer ownership
        }
        // move assignment operator
        UniquePtr& operator=(UniquePtr&& other) noexcept {
            std::cout<< "UniquePtr Move Assignment"<<std::endl;
            // if (this != &other) {
            //     _deleter(_ptr); // Clean up existing resource
            //     _ptr = other._ptr; // Transfer ownership
            //     _deleter = std::move(other._deleter); // Move deleter
            //     other._ptr = nullptr; // Leave other in a valid state
            // }
            // return *this;
            swap(other);
            return *this;
        }
    // Accessor
    T* get() const noexcept {
        std::cout<< "UniquePtr Get"<<std::endl;
        return _ptr;
    }
    Deleter get_deleter() const noexcept {
        std::cout<< "UniquePtr Get Deleter"<<std::endl;
        return _deleter;
    }
    // Dereference operator
    T& operator*() const {
        std::cout<< "UniquePtr Dereference"<<std::endl;
        return *_ptr;
    }
    // Arrow operator
    T* operator->() const {
        std::cout<< "UniquePtr Arrow"<<std::endl;
        return _ptr;
    }

    // bool conversion operator
    explicit operator bool() const {
        return _ptr != nullptr;
    }
    // release ownership without deleting
    T* release() noexcept {
        T* temp = _ptr;
        _ptr = nullptr;
        return temp;
    }
    // replace the managed object
    void reset(T* p = nullptr) noexcept {
        _deleter(_ptr);
        _ptr = p;
    }
    friend void swap(UniquePtr<T,Deleter>& a,
                                        UniquePtr<T,Deleter>& b) noexcept;
    private:
        T* _ptr;
        Deleter _deleter;
        void swap(UniquePtr& other) noexcept {
            std::cout<< "UniquePtr Swap Member function"<<std::endl;
            std::swap(_ptr,      other._ptr);
            std::swap(_deleter,  other._deleter);
        }
    
};

// Free-function swap
template<typename T, typename D>
void swap(UniquePtr<T,D>& a, UniquePtr<T,D>& b) noexcept {
    std::cout<< "UniquePtr Swap Free function"<<std::endl;
    a.swap(b);
}

} // namespace mynamespace
#endif // UNIQUE_PTR
