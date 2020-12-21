#include "stream_reassembler.hh"
#include <iostream>
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
    string ndata;
    if(index+data.length()<_head_index){
        return;
    }
    else if(index<_head_index){
        size_t offset=_head_index-index;
        ndata.assign(data.begin()+offset,data.end());
    }
    else {
        ndata=data;
    }
    bool inserted=false;
    block_node elm{index,ndata.length()};
    _unassembled_byte+=elm.length;
    if(eof){
        _capacity=elm.begin+elm.length;
    }
    for(size_t i=0;i<ndata.length();i++){
        _buffer[i+index]=ndata[i];
    }
    if(!_blocks.empty()){
        //cout<<"blocks size: "<<_blocks.size()<<"\n";
        for(auto iter=_blocks.begin();iter!=_blocks.end();){
            //cout<<"iter ++ "<<_blocks.size()<<"\n";
            block_node pivot=*iter;
            long inter_size=block_intersection_size(elm,pivot);
            if(inter_size>=0){
                _unassembled_byte-=block_intersection_size(elm,*iter);
                elm=block_union(elm,*iter);
                auto a_iter=iter;
                iter++;
                _blocks.erase(a_iter);
            }
            else if(iter->begin>elm.begin){
                _blocks.insert(iter,elm);
                inserted=true;
                break;
            }
            else {
                iter++;
            }
        }
    }
    if(!inserted){
        _blocks.push_back(elm);
    }
    if(!_blocks.empty()&&_blocks.begin()->begin==_head_index){
        block_node head_block=_blocks.front();
        string str;
        str.assign(_buffer.begin()+head_block.begin,_buffer.begin()+head_block.begin+head_block.length);
        _output.write(str);
        _head_index=head_block.begin+head_block.length;
        _unassembled_byte-=head_block.length;
        _blocks.pop_front();
    }
    if(_head_index>=_capacity){
        _output.end_input();
    }
    /*if(_unassembled_byte==_capacity){
        //for(size_t i=0;i<_capacity;i++){
            string str;
            str.assign(_buffer.begin(),_buffer.end());
            _output.write(str);
    }*/
    //DUMMY_CODE(data, index, eof);
}

size_t StreamReassembler::unassembled_bytes() const { return _unassembled_byte; }

bool StreamReassembler::empty() const { return _unassembled_byte==0; }
