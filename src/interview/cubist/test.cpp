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


#include <bits/stdc++.h>

using namespace std;

#include <cassert>
#include <cstring>
#include <array>
#include <iostream>
#include <variant>
#include <optional>
#include <vector>

// ----------------------------
// API from thirdparty/read5k.h
// ----------------------------
class Read4k
{
public:
    // --------------------------------------------------------------------------------
    // If there is >= 4096 bytes left in the underlying stream, 4096 bytes
    // is copied into destination, and 4096 is returned
    // Else: the remaining bytes [0-4095] bytes are copied into destination, and the number of bytes that was copied is returned.
    // All subsequent calls to Read4k::read will copy zero bytes, and return zero (indicating EOF)
    // NB: You will NOT implement the Read4k::read function, but rather you will USE this function. It's from a thirdparty, and cannot be changed.
    // Perhaps a simple child class of Read4k could be implemented so we can compile the test
    virtual std::size_t read(char *destination) = 0;
};
// --------------------
enum class MsgType
{
    Quote = 0,
    Trade,
    Snapshot,
    MsgTypeCount
};
struct Header
{
    MsgType type;
};
struct Quote
{
    int32_t symbol_id;
    double price;
    char side;
};
struct Trade
{
    int32_t symbol_id;
    double price;
    int32_t quantity;
    char side;
};
struct Snapshot
{
    std::array<char, 100000> payload;
};

// The data that comes from Read4k will be HEADER-MESSAGE-HEADER-MESSAGE-...-HEADER-MESSAGE -- but only 4k bytes at a time.
// The specific message that follows the header is specified in Header::MsgType.
//
// Your job is to on-board this third-party library into your team's tech stack.
// You want your teammates to be able to programatically get individual messages, one message (Quote|Trade|Snapshot) at a time.
// You want your teammates to be able to do this easily, without having to care about any underlying page sizes, or how all this was done.
// Read4k could be a socket, which has an never-ending stream of messages; it could be a file, which has a finite number of messages.

// -------------------------------------------------------------------
// This is where you will write your code

// -------------------
// Your additions here
// -------------------
class FeedHandler
{
public:
    explicit FeedHandler(Read4k &reader)
        : _reader(reader), _eof(false)
    {
    }

    // Attempts to parse and print exactly one message.
    // Returns true if a message was printed, false on EOF.
    bool consume_and_print_next(std::ostream &os)
    {
        // 1) Make sure we have at least a header's worth of bytes
        if (!fillBuffer(sizeof(Header)))
        {
            return false; // no header → EOF
        }

        // 2) Peek at the header (safe even if unaligned)
        Header hdr;
        std::memcpy(&hdr, _buf.data(), sizeof(Header));

        // 3) Decide how much more we need for this full message
        size_t payloadSize = 0;
        const char *typeName = nullptr;
        switch (hdr.type)
        {
        case MsgType::Quote:
            payloadSize = sizeof(Quote);
            typeName = "Quote";
            break;
        case MsgType::Trade:
            payloadSize = sizeof(Trade);
            typeName = "Trade";
            break;
        case MsgType::Snapshot:
            payloadSize = sizeof(Snapshot);
            typeName = "Snapshot";
            break;
        default:
            // Unknown type → protocol violation
            throw std::runtime_error("Unknown message type");
        }

        size_t totalSize = sizeof(Header) + payloadSize;

        // 4) Make sure we have the full message
        if (!fillBuffer(totalSize))
        {
            return false; // truncated message at EOF
        }

        // 5) "Consume" it: print the type, then drop those bytes
        os << typeName << "\n";
        _buf.erase(_buf.begin(), _buf.begin() + totalSize);
        return true;
    }

private:
    Read4k &_reader;
    std::vector<char> _buf;
    bool _eof;

    // Keeps calling reader.read(...) until _buf.size() >= want
    // or until read() returns 0 (EOF).  Returns true iff we
    // ended up with at least want bytes.
    bool fillBuffer(size_t want)
    {
        while (_buf.size() < want && !_eof)
        {
            char page[4096];
            size_t n = _reader.read(page);
            if (n == 0)
            {
                _eof = true;
                break;
            }
            _buf.insert(_buf.end(), page, page + n);
        }
        return _buf.size() >= want;
    }
};

class RoundRobinReader : public Read4k
{
public:
    std::size_t read(char *destination) override
    {
        auto sz = std::min(std::size_t(4096), m_bytes.size());
        std::memcpy(destination, m_bytes.data(), sz);
        std::vector<char> buf;
        buf.resize(m_bytes.size() - sz);
        memcpy(buf.data(), m_bytes.data() + sz, m_bytes.size() - sz);
        m_bytes = buf;
        return sz;
    }
    void add(Quote msg)
    {
        add(msg, MsgType::Quote);
    }
    void add(Trade msg)
    {
        add(msg, MsgType::Trade);
    }
    void add(Snapshot msg)
    {
        add(msg, MsgType::Snapshot);
    }
    std::vector<char> m_bytes;

private:
    template <class T>
    void add(T msg, MsgType type)
    {
        size_t offset = m_bytes.size();
        m_bytes.resize(m_bytes.size() + sizeof(Header) + sizeof(T));
        auto *hdr = reinterpret_cast<Header *>(m_bytes.data() + offset);
        auto *msg_ptr = reinterpret_cast<T *>(m_bytes.data() + offset + sizeof(Header));
        hdr->type = type;
        *msg_ptr = msg;
    }
};

int main()
{
    ofstream fout(getenv("OUTPUT_PATH"));

    string messages;
    getline(cin, messages);
    RoundRobinReader reader;
    for (char message : messages)
    {
        if (message == 'q')
        {
            reader.add(Quote{1, 0, 'b'});
        }
        if (message == 't')
        {
            reader.add(Trade{1, 0, 'b'});
        }
        if (message == 's')
        {
            reader.add(Snapshot{1, 0, 'b'});
        }
    }

    FeedHandler handler(reader);
    while (handler.consume_and_print_next(fout))
    {
    };

    // fout << result << "\n";
    fout.close();

    return 0;
}
