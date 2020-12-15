#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity){
    _buffer.resize(capacity);
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    bool inserted=false;
    block_node elm{index,data.length()};
    _unassembled_byte+=elm.length;
    if(eof){
        _capacity=elm.begin+elm.length;
    }
    for(size_t i=0;i<data.length();i++){
        _buffer[i+index]=data[i];
    }
    for(auto iter=_blocks.begin();iter!=_blocks.end();iter++){
        if(block_intersection_size(elm,*iter)>=0){
            _unassembled_byte-=block_intersection_size(elm,*iter);
            elm=block_union(elm,*iter);
            _blocks.erase(iter);
        }
        else if(iter->begin>elm.begin){
            _blocks.insert(iter,elm);
            inserted=true;
        }
    }
    if(!inserted){
        _blocks.push_back(elm);
    }
    if(_unassembled_byte==_capacity){
        //for(size_t i=0;i<_capacity;i++){
            string str;
            str.assign(_buffer.begin(),_buffer.end());
            _output.write(str);
    }
    //DUMMY_CODE(data, index, eof);
}

size_t StreamReassembler::unassembled_bytes() const { return _unassembled_byte; }

bool StreamReassembler::empty() const { return _unassembled_byte==0; }
