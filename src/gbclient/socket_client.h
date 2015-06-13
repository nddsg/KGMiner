//
// Created by Baoxu Shi on 6/13/15.
//

#ifndef GBPEDIA_SOCKET_CLIENT_H
#define GBPEDIA_SOCKET_CLIENT_H

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>

namespace local = boost::asio::local;

class socket_client {
  boost::asio::io_service io_service;

public:
  void start(std::string socket_name) {
    local::stream_protocol::endpoint ep(socket_name);
    local::stream_protocol::socket socket(io_service);
    socket.connect(ep);
    boost::array<char, 1024> buf;
    boost::system::error_code error;
    size_t len = boost::asio::read(socket, boost::asio::buffer(buf),error);
    std::cout.write(buf.data(), len);
  }

};

#endif //GBPEDIA_SOCKET_CLIENT_H
