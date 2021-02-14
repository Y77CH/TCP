#include "byte_stream.hh"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <stdexcept>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) : _capacity(capacity) {
    // DUMMY_CODE(capacity);
}

size_t ByteStream::write(const string &data) {
    size_t len = data.length();
    if (len > _capacity - _buffer.size()) {
        //_error=true;
        // throw runtime_error("ByteStream: write size greater than remaining capacity.");
        len = _capacity - _buffer.size();
    }
    _write_count += len;
    for (size_t i = 0; i < len; i++) {
        _buffer.push_back(data[i]);
    }
    return len;
    // DUMMY_CODE(data);
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t length = len;
    if (length > _buffer.size()) {
        //_error=true;
        //throw runtime_error("ByteStream: peek size greater than remaining capacity.");
        length = _buffer.size();
    }
    return string().assign(_buffer.begin(), _buffer.begin() + length);
    // DUMMY_CODE(len);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t length = len;
    if (length > _buffer.size()) {
        //throw runtime_error("ByteStream: pop size greater than remaining capacity.");
        //_error=true;
        length = _buffer.size();
    }
    _read_count += length;
    while (length--) {
        _buffer.pop_front();
    }
    return;
    // DUMMY_CODE(len);
}

void ByteStream::end_input() { _input_ended_flag = true; }

bool ByteStream::input_ended() const { return _input_ended_flag; }

size_t ByteStream::buffer_size() const { return _buffer.size(); }

bool ByteStream::buffer_empty() const { return _buffer.size() == 0; }

bool ByteStream::eof() const { return buffer_empty() && input_ended(); }

size_t ByteStream::bytes_written() const { return _write_count; }

size_t ByteStream::bytes_read() const { return _read_count; }

size_t ByteStream::remaining_capacity() const { return _capacity - _buffer.size(); }