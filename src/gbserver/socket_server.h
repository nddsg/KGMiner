//
// Created by Baoxu Shi on 6/12/15.
//

#ifndef GBPEDIA_SOCKET_SERVER_H
#define GBPEDIA_SOCKET_SERVER_H

#include "graph.h"

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include <string>
#include <sstream>
#include <vector>

namespace local = boost::asio::local;

void worker(local::stream_protocol::socket *socket, graph<std::string, std::string>& g) {
  boost::array<char, 1024> buf;
  boost::system::error_code error;
  size_t len = boost::asio::read(*socket, boost::asio::buffer(buf), error);

  std::vector<std::string> commands;
  std::stringbuf strbuf;
  for(int i = 0; i < len; i++) {
    if (buf.elems[i] != ',' && buf.elems[i] != ' ') {
      strbuf.sputc(buf.elems[i]);
    } else {
      commands.push_back(strbuf.str());
      strbuf.str("");
    }
  }
  commands.push_back(strbuf.str());

  std::string return_string = "";

  if (commands.size() == 0) {
    return_string = "No command provided\n";
  }else if (commands.at(0) == "path") {
    try{
      std::ostringstream oss;
      std::vector< std::vector<unsigned int> > paths = g.dfs((unsigned int)stoi(commands.at(1)),
                                                             (unsigned int)stoi(commands.at(2)),
                                                             (unsigned int)stoi(commands.at(3)),
                                                             commands.size() == 4 || (commands.at(4) == "true" ||
                                                                 commands.at(4) == "TRUE" || commands.at(4) == "T"));
      oss << "find " << paths.size() << " paths\n";
      for(auto it = paths.cbegin(); it != paths.cend(); ++it) {
        for(auto itt = it->cbegin(); itt != it->cend(); ++itt) {
          oss << *itt << "--";
        }
        oss << "\n";
      }
      return_string = oss.str();
    } catch(std::exception error) {
      return_string = error.what();
    }

  } else if (commands.at(0) == "in_neighbor") {
    try {
      std::ostringstream oss;
      std::vector<unsigned int> neighbors = g.get_in_edges((unsigned int)stoi(commands.at(1)));
      for(auto it = neighbors.cbegin(); it != neighbors.cend(); ++it) {
        oss << *it << ",";
      }
      oss << "\n";
      return_string = oss.str();
    } catch(std::exception error) {
      return_string = error.what();
    }
  } else if (commands.at(0) == "out_neighbor") {
    try {
      std::ostringstream oss;
      std::vector<unsigned int> neighbors = g.get_out_edges((unsigned int)stoi(commands.at(1)));
      for(auto it = neighbors.cbegin(); it != neighbors.cend(); ++it) {
        oss << *it << ",";
      }
      oss << "\n";
      return_string = oss.str();
    } catch(std::exception error) {
      return_string = error.what();
    }
  } else {
    return_string = "Unsupported command\n";
  }


  try{
    len = boost::asio::write(*socket, boost::asio::buffer(return_string));
    if (len !=return_string.size()) {
      std::cerr << "write error, wrote " << len << " characters, " << return_string.size() << " expected.\n";
    }
  } catch (std::exception &exception) {
    std::cerr << exception.what();
  }

  socket->close();
  delete socket;
}


class socket_server {

  boost::asio::io_service worker_io_service;
  boost::thread_group threadpool;
  boost::asio::io_service::work *work;

public:

  socket_server(int nworkers = 10) {
    work = new boost::asio::io_service::work(worker_io_service);
    for(int i = 0 ; i < nworkers; i++) {
      threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &worker_io_service));
    }

    std::cout << threadpool.size() << " threads are created\n";
  }

  void start(std::string socket_name, graph<std::string, std::string>& g) {
    boost::asio::io_service socket_io_service;
    ::unlink(socket_name.c_str());
    local::stream_protocol::endpoint ep(socket_name);
    local::stream_protocol::acceptor acceptor(socket_io_service, ep);

    while(true) { //TODO: Throw Broken pipe error when all workers are busy and a lot of requests are coming.
      local::stream_protocol::socket *socket = new local::stream_protocol::socket(socket_io_service);
      acceptor.accept(*socket);
      worker_io_service.post(boost::bind(worker, socket, g));
    }
  }

  void cleanup(){
    worker_io_service.stop();
    threadpool.join_all();
  }


};

#endif //GBPEDIA_SOCKET_SERVER_H
