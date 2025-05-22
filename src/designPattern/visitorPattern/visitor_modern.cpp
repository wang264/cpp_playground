#include <iostream>
#include <variant>

struct ActionAllowed {
    // Define the properties of ActionAllowed
    static constexpr inline char const name[] = "ActionAllowed";
};

struct ActionNotAllowed {
    // Define the properties of ActionNotAllowed
    static constexpr inline char const name[] = "ActionNotAllowed";
};

struct BrokerRejected {
    // Define the properties of BrokerRejected
    static constexpr inline char const name[] = "BrokerRejected";
};

using StateTransitionResult = std::variant<ActionAllowed, ActionNotAllowed, BrokerRejected>;



template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

int main() {
    // Example usage
    StateTransitionResult state_trans_result{ActionAllowed{}};

    std::visit(overloaded{
        [&](ActionAllowed) { std::cout << "Apply logic for ActionAllowed\n" << std::endl; },
        [&](ActionNotAllowed) { std::cout << "Apply logic for ActionNotAllowed\n" << std::endl; },
        [&](BrokerRejected) { std::cout << "Apply logic for BrokerRejected\n" << std::endl; }
    }, state_trans_result);



    StateTransitionResult state_trans_result2{BrokerRejected{}};
    // Can also use it without overload
    std::visit([](const auto& s) {
    using T = std::decay_t<decltype(s)>;

    if constexpr (std::is_same_v<T, ActionAllowed>) {
        std::cout << "Apply logic for ActionAllowed\n";
    } else if constexpr (std::is_same_v<T, ActionNotAllowed>) {
        std::cout << "Apply logic for ActionNotAllowed\n";
    } else if constexpr (std::is_same_v<T, BrokerRejected>) {
        std::cout << "Apply logic for BrokerRejected\n";
    }
    }, state_trans_result2);

    return 0;
}

//  g++ -std=c++20 -Wall visitor_modern.cpp -o visitor_modern.tsk