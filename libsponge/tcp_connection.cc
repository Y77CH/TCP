#include "tcp_connection.hh"

#include <iostream>

// Dummy implementation of a TCP connection

// For Lab 4, please replace with a real implementation that passes the
// automated checks run by `make check`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

size_t TCPConnection::remaining_outbound_capacity() const { return _sender.stream_in().remaining_capacity(); }

size_t TCPConnection::bytes_in_flight() const { return _sender.bytes_in_flight(); }

size_t TCPConnection::unassembled_bytes() const { return _receiver.unassembled_bytes(); }

size_t TCPConnection::time_since_last_segment_received() const { return _time_since_last_segment_received; }

void TCPConnection::segment_received(const TCPSegment &seg) { 
    if(!_active) return;
    _time_since_last_segment_received=0;
    if(seg.header().rst){
        unclean_shutdown();
        return;
    }
    if(seg.header().ack){
        _sender.ack_received(seg.header().ackno,seg.header().win);
    }
    _receiver.segment_received(seg);
    if(seg.header().syn||seg.header().fin||seg.payload().size()!=0){
        _need_send_ackno=true;
    }
    push_segments_out();
    clean_shutdown();
 }

bool TCPConnection::active() const { return _active; }

size_t TCPConnection::write(const string &data) {
    _sender.fill_window();
    push_segments_out();
    return _sender.stream_in().write(data);
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) { 
    if(!_active) return;
    _time_since_last_segment_received+=ms_since_last_tick;
    clean_shutdown();
    _sender.tick(ms_since_last_tick);
    if(_sender.consecutive_retransmissions()>TCPConfig::MAX_RETX_ATTEMPTS){
        unclean_shutdown();
        TCPSegment seg;
        seg.header().rst=true;
        _sender.fill_window();
    }
    push_segments_out();
 }

void TCPConnection::end_input_stream() {
    _sender.stream_in().end_input();
    _sender.fill_window();
    push_segments_out();
}

void TCPConnection::connect() {
    // when initital, fill_window auto send a SYN
    _sender.fill_window();
    push_segments_out();
}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";
        unclean_shutdown();
        TCPSegment seg;
        seg.header().rst=true;
        _sender.fill_window();
        push_segments_out();

            // Your code here: need to send a RST segment to the peer
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}

bool TCPConnection::push_segments_out(){
    TCPSegment seg;
    if((_need_send_ackno||!_sender.segments_out().empty())&&_receiver.ackno().has_value()){
        if(_sender.segments_out().empty()){
            _sender.send_empty_segment();
        }
        seg=_sender.segments_out().front();
        _sender.segments_out().pop();
        seg.header().ack=true;
        seg.header().ackno=_receiver.ackno().value();
        seg.header().win=_receiver.window_size();
        _segments_out.push(seg);
        _need_send_ackno=false;
    }
    while(!_sender.segments_out().empty()){
        seg=_sender.segments_out().front();
        _sender.segments_out().pop();
        _segments_out.push(seg);
    }
    return true;
}


void TCPConnection::unclean_shutdown(){
    _receiver.stream_out().set_error();
    _sender.stream_in().set_error();
    _active=false;
}

bool TCPConnection::clean_shutdown(){
    if(_receiver.stream_out().input_ended()&&!(_sender.stream_in().eof())&&_sender.bytes_in_flight()==0){
        _linger_after_streams_finish=false;
    }
    if(_sender.stream_in().eof()&&_sender.bytes_in_flight()==0&&_receiver.stream_out().input_ended()){
        if(!_linger_after_streams_finish||time_since_last_segment_received()>=10*_cfg.rt_timeout){
            _active=false;
        }
    }
    return !_active;
}