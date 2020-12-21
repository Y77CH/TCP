#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "byte_stream.hh"

#include <cstdint>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iostream>

//! \brief A class that assembles a series of excerpts from a byte stream (possibly out of order,
//! possibly overlapping) into an in-order byte stream.
class StreamReassembler {
  private:
    // Your code here -- add private members as necessary.
    struct block_node{
      size_t begin;
      size_t length;
    };
    std::list<block_node> _blocks={};
    std::vector<char> _buffer={};
    size_t _unassembled_byte=0;
    size_t _head_index=0;
    ByteStream _output;  //!< The reassembled in-order byte stream
    size_t _capacity;    //!< The maximum number of bytes
    
    long block_intersection_size(const block_node nx,const block_node ny){
      block_node x=nx;
      block_node y=ny;
      try{
        if(x.begin>y.begin){
          std::swap(x,y);
        }
        if(x.begin+x.length<y.begin){
          return -1;
        }
        else if(x.begin+x.length>=y.begin+y.length){
          return y.length;
        }
        else {
          return x.begin+x.length-y.begin;
        }
      }
      catch(std::exception& e){
        std::cout<<"block_intersection_size catch excption";
        throw e;
      }
    }

    block_node block_union(block_node x,block_node y){
      if(x.begin>y.begin){
        std::swap(x,y);
      }
      if(x.begin+x.length<y.begin){
        throw std::runtime_error("StreamReassembler: empty intersect.");
      }
      else if(x.begin+x.length<y.begin+y.length){
        x.length=y.begin+y.length-x.begin;
      }
      return x;
    }

  public:
    //! \brief Construct a `StreamReassembler` that will store up to `capacity` bytes.
    //! \note This capacity limits both the bytes that have been reassembled,
    //! and those that have not yet been reassembled.
    StreamReassembler(const size_t capacity);

    //! \brief Receives a substring and writes any newly contiguous bytes into the stream.
    //!
    //! If accepting all the data would overflow the `capacity` of this
    //! `StreamReassembler`, then only the part of the data that fits will be
    //! accepted. If the substring is only partially accepted, then the `eof`
    //! will be disregarded.
    //!
    //! \param data the string being added
    //! \param index the index of the first byte in `data`
    //! \param eof whether or not this segment ends with the end of the stream
    void push_substring(const std::string &data, const uint64_t index, const bool eof);

    //! \name Access the reassembled byte stream
    //!@{
    const ByteStream &stream_out() const { return _output; }
    ByteStream &stream_out() { return _output; }
    //!@}

    //! The number of bytes in the substrings stored but not yet reassembled
    //!
    //! \note If the byte at a particular index has been submitted twice, it
    //! should only be counted once for the purpose of this function.
    size_t unassembled_bytes() const;

    //! \brief Is the internal state empty (other than the output stream)?
    //! \returns `true` if no substrings are waiting to be assembled
    bool empty() const;
};

#endif  // SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
