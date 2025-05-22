#include <iostream>
#include <memory>
#include <string>

// Forward declaration for context
class TCPConnection;

// =====================
// Abstract State Interface
// =====================
class TCPState {
public:
    virtual ~TCPState() = default;

    virtual void onEnter(TCPConnection&) {}
    virtual void onExit(TCPConnection&) {}

    virtual void open(TCPConnection&)                          { illegal("open"); }
    virtual void close(TCPConnection&)                         { illegal("close"); }
    virtual void send(TCPConnection&, const std::string&)      { illegal("send"); }
    virtual void receive(TCPConnection&)                       { illegal("receive"); }
    virtual void ack(TCPConnection&)                           { illegal("ack"); }
    virtual void timeout(TCPConnection&)                       { illegal("timeout"); }

protected:
    void illegal(const std::string& action) {
        std::cout << "[!] Illegal action: " << action << " in current state.\n";
    }
};

// =====================
// Concrete States
// =====================
class TCPClosed : public TCPState {
public:
    void onEnter(TCPConnection&) override {
        std::cout << "[Enter] TCPClosed\n";
    }
    void onExit(TCPConnection&) override {
        std::cout << "[Exit] TCPClosed\n";
    }
    void open(TCPConnection& conn) override;
};

class TCPListening : public TCPState {
public:
    void onEnter(TCPConnection&) override {
        std::cout << "[Enter] TCPListening\n";
    }
    void onExit(TCPConnection&) override {
        std::cout << "[Exit] TCPListening\n";
    }
    void receive(TCPConnection& conn) override;
    void close(TCPConnection& conn) override;
};

class TCPEstablished : public TCPState {
public:
    void onEnter(TCPConnection&) override {
        std::cout << "[Enter] TCPEstablished\n";
    }
    void onExit(TCPConnection&) override {
        std::cout << "[Exit] TCPEstablished\n";
    }
    void send(TCPConnection&, const std::string& data) override;
    void close(TCPConnection& conn) override;
};

class TCPFinWait : public TCPState {
public:
    void onEnter(TCPConnection&) override {
        std::cout << "[Enter] TCPFinWait\n";
    }
    void onExit(TCPConnection&) override {
        std::cout << "[Exit] TCPFinWait\n";
    }
    void ack(TCPConnection& conn) override;
};

class TCPTimeWait : public TCPState {
public:
    void onEnter(TCPConnection&) override {
        std::cout << "[Enter] TCPTimeWait\n";
    }
    void onExit(TCPConnection&) override {
        std::cout << "[Exit] TCPTimeWait\n";
    }
    void timeout(TCPConnection& conn) override;
};

// =====================
// Context: TCPConnection
// =====================
class TCPConnection {
    std::unique_ptr<TCPState> state;

public:
    TCPConnection(std::unique_ptr<TCPState> init)
        : state(std::move(init)) {
        state->onEnter(*this);
    }

    void setState(std::unique_ptr<TCPState> s) {
        state->onExit(*this);
        state = std::move(s);
        state->onEnter(*this);
    }

    void open()                       { state->open(*this); }
    void close()                      { state->close(*this); }
    void send(const std::string& msg) { state->send(*this, msg); }
    void receive()                    { state->receive(*this); }
    void ack()                        { state->ack(*this); }
    void timeout()                    { state->timeout(*this); }
};

// =====================
// State Transitions (implementations)
// =====================

void TCPClosed::open(TCPConnection& conn) {
    std::cout << "[Closed] open() called. Checking guard...\n";
    bool guardOK = true; // Simulated guard
    if (guardOK) {
        std::cout << "[Guard Passed] Transitioning to Listening\n";
        conn.setState(std::make_unique<TCPListening>());
    } else {
        std::cout << "[Guard Failed] Cannot open connection.\n";
    }
}

void TCPListening::receive(TCPConnection& conn) {
    std::cout << "[Listening] receive() called. SYN detected.\n";
    conn.setState(std::make_unique<TCPEstablished>());
}

void TCPListening::close(TCPConnection& conn) {
    std::cout << "[Listening] close() called.\n";
    conn.setState(std::make_unique<TCPClosed>());
}

void TCPEstablished::send(TCPConnection&, const std::string& data) {
    std::cout << "[Established] Sending: " << data << "\n";
}

void TCPEstablished::close(TCPConnection& conn) {
    std::cout << "[Established] Closing. Moving to FinWait\n";
    conn.setState(std::make_unique<TCPFinWait>());
}

void TCPFinWait::ack(TCPConnection& conn) {
    std::cout << "[FinWait] ACK received. Transitioning to TimeWait\n";
    conn.setState(std::make_unique<TCPTimeWait>());
}

void TCPTimeWait::timeout(TCPConnection& conn) {
    std::cout << "[TimeWait] Timeout expired. Transitioning to Closed\n";
    conn.setState(std::make_unique<TCPClosed>());
}

// =====================
// Main Test
// =====================
int main() {
    TCPConnection conn(std::make_unique<TCPClosed>());

    conn.send("Hello");        // Illegal
    conn.open();               // Closed → Listening
    conn.receive();            // Listening → Established
    conn.send("GET /index");   // Valid
    conn.close();              // Established → FinWait
    conn.ack();                // FinWait → TimeWait
    conn.timeout();            // TimeWait → Closed

    return 0;
}

// g++ -std=c++20 -Wall tcpConnection2.cpp -o tcpConnection2.tsk