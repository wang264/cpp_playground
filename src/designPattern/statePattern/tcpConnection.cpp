#include <iostream>
#include <memory>
#include <string>

// Forward declaration
class TCPConnection;

// =====================
// Abstract TCPState
// =====================
class TCPState {
public:
    virtual ~TCPState() = default;

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
// Context: TCPConnection
// =====================
class TCPConnection {
    std::unique_ptr<TCPState> state;
public:
    TCPConnection(std::unique_ptr<TCPState> init)
        : state(std::move(init)) {}

    void setState(std::unique_ptr<TCPState> s) {
        state = std::move(s);
    }

    void open()                       { state->open(*this); }
    void close()                      { state->close(*this); }
    void send(const std::string& msg) { state->send(*this, msg); }
    void receive()                    { state->receive(*this); }
    void ack()                        { state->ack(*this); }
    void timeout()                    { state->timeout(*this); }
};

// =====================
// Concrete States
// =====================

class TCPClosed : public TCPState {
public:
    void open(TCPConnection& conn) override;
};

class TCPListening : public TCPState {
public:
    void receive(TCPConnection& conn) override;
    void close(TCPConnection& conn) override;
};

class TCPEstablished : public TCPState {
public:
    void send(TCPConnection&, const std::string& data) override;
    void close(TCPConnection& conn) override;
};

class TCPFinWait : public TCPState {
public:
    void ack(TCPConnection& conn) override;
};

class TCPTimeWait : public TCPState {
public:
    void timeout(TCPConnection& conn) override;
};

// =====================
// State Transitions
// =====================

void TCPClosed::open(TCPConnection& conn) {
    std::cout << "[Closed] -> Opening... transitioning to Listening\n";
    conn.setState(std::make_unique<TCPListening>());
}

void TCPListening::receive(TCPConnection& conn) {
    std::cout << "[Listening] -> Received SYN. Transitioning to Established\n";
    conn.setState(std::make_unique<TCPEstablished>());
}

void TCPListening::close(TCPConnection& conn) {
    std::cout << "[Listening] -> Closing. Transitioning to Closed\n";
    conn.setState(std::make_unique<TCPClosed>());
}

void TCPEstablished::send(TCPConnection&, const std::string& data) {
    std::cout << "[Established] -> Sending data: " << data << "\n";
}

void TCPEstablished::close(TCPConnection& conn) {
    std::cout << "[Established] -> Closing. Transitioning to FinWait\n";
    conn.setState(std::make_unique<TCPFinWait>());
}

void TCPFinWait::ack(TCPConnection& conn) {
    std::cout << "[FinWait] -> ACK received. Transitioning to TimeWait\n";
    conn.setState(std::make_unique<TCPTimeWait>());
}

void TCPTimeWait::timeout(TCPConnection& conn) {
    std::cout << "[TimeWait] -> Timeout expired. Transitioning to Closed\n";
    conn.setState(std::make_unique<TCPClosed>());
}

// =====================
// Main Test
// =====================

int main() {
    TCPConnection conn(std::make_unique<TCPClosed>());

    conn.send("data");         // Illegal
    conn.open();               // Closed → Listening
    conn.receive();            // Listening → Established
    conn.send("Hello");        // Valid
    conn.close();              // Established → FinWait
    conn.ack();                // FinWait → TimeWait
    conn.timeout();            // TimeWait → Closed

    return 0;
}
// g++ -std=c++14 -Wall -o tcp_connection tcpConnection.cpp