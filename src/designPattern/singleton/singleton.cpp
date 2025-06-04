
struct Singleton_t
{
  // Accessor: the first time you call `instance()`, the static
  // local object is constructed. Subsequent calls return the same instance
  static Singleton_t& instance()
  {
    static Singleton_t _instance;
    return _instance;
  } // instance

private:

  Singleton_t() =default;
  ~Singleton_t()=default;
// 2) (Optional) Private destructor (or defaulted) to prevent deletion

// 3) Delete copy‐constructor and copy‐assignment
Singleton_t(const Singleton_t&) = delete;
Singleton_t& operator=(const Singleton_t&) = delete;

// 4) Delete move‐constructor and move‐assignment (to be extra safe)
Singleton_t(Singleton_t&&) = delete;
Singleton_t& operator=(Singleton_t&&) = delete;

}; // struct singleton_t


int main() {
    // Usage example
    Singleton_t & singletonInstance = Singleton_t::instance();
    
    // The following lines would cause compilation errors due to deleted copy constructor and assignment operator
    // Singleton_t anotherInstance = singletonInstance; // Error
    // Singleton_t yetAnotherInstance;
    // yetAnotherInstance = singletonInstance; // Error
    
    return 0;
}

// g++ -std=c++20 -o singleton.tsk singleton.cpp