// You are onboarding a new third-party library from a company call 4K
// * They sell secrete properitary market data, which can come from many difference sources, all off which
//    is accessed through a common API - class Read4K
// * The interface is specified in the code section below. This provides access to the secrete data
//   (the underlying data for this source could be a database, a file, a socket, etc- that is abstracted away from you)

// * YOur job is to onboard this new library into your team's tech stack.
// The raed4K interface , as defined below can only read in 4096 bytes page size
// * Assuming the data comes in as HEADER-MESSAGE-HEADER-MESSAGE-...-HEADER-MESSAGE, write a function/class
//   what ever you think it appropriate, to allow you team to easily obtain message from the 4k data stream. See
//   code section for more details on reauirements.

// NB: this is both a design and coding question. Explain your choices. 


#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <span>

struct Buffer4096 {
    static constexpr size_t BUFFER_SIZE = 4096;
    std::array<uint8_t, BUFFER_SIZE> data{};
    size_t pos = 0; // Current write position

    // Adds data to the buffer. Flushes when full.
    void addData(const uint8_t* input, size_t len) {
        size_t inputPos = 0;
        while (inputPos < len) {
            size_t spaceLeft = BUFFER_SIZE - pos;
            size_t toCopy = std::min(spaceLeft, len - inputPos);

            std::memcpy(data.data() + pos, input + inputPos, toCopy);
            pos += toCopy;
            inputPos += toCopy;

            if (pos == BUFFER_SIZE) {
                flush();
                clear();
            }
        }
    }

    // Flush: send the buffer (here just prints for demo)
    void flush() {
        // Replace with actual output logic, e.g., write to disk or network
        std::cout << "Flushing buffer with " << pos << " bytes\n";
        // For demonstration: print as hex
        // for (size_t i = 0; i < pos; ++i) std::cout << std::hex << (int)data[i] << ' ';
        // std::cout << std::dec << '\n';
    }

    // Clears the buffer
    void clear() { pos = 0; }

    // Query: Get current size
    size_t size() const { return pos; }

    // Query: Is buffer full?
    bool isFull() const { return pos == BUFFER_SIZE; }

    // Query: Get const span/view of current data
    std::span<const uint8_t> view() const { return std::span<const uint8_t>(data.data(), pos); }
};

int main() {
    Buffer4096 buffer;
    uint8_t test[2000] = {1,2,3,4};
    buffer.addData(test, 2000);
    std::cout << "Buffer size after 2000 bytes: " << buffer.size() << "\n";
    buffer.addData(test, 3000); // triggers flush and clear
    std::cout << "Buffer size after 3000 more bytes: " << buffer.size() << "\n";
    // Query
    std::cout << "Is buffer full? " << (buffer.isFull() ? "yes" : "no") << "\n";
}

// g++ -std=c++20 -o buffer.tsk buffer.cpp