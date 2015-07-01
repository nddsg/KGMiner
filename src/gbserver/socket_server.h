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

bool is_true(const std::string &str) {
  return str.compare("true") == 0 ||
         str.compare("TRUE") == 0 ||
         str.compare("T") == 0;
}

bool is_false(const std::string &str) {
  return str.compare("false") == 0 ||
         str.compare("FALSE") == 0 ||
         str.compare("F") == 0;
}

void worker(local::stream_protocol::socket *socket, graph<std::string, std::string> &g) {
  boost::array<char, 1024> buf;
  boost::system::error_code error;
  size_t len = boost::asio::read(*socket, boost::asio::buffer(buf), error);

  std::vector<std::string> commands;
  std::stringbuf strbuf;
  for (int i = 0; i < len; i++) {
    if (buf.elems[i] != ',' && buf.elems[i] != ' ' && buf.elems[i] != '\n') {
      strbuf.sputc(buf.elems[i]);
    } else {
      commands.push_back(strbuf.str());
      strbuf.str("");
    }
  }
  //commands.push_back(strbuf.str());

  std::string return_string = "";
  try {
    if (commands.size() == 0) {
      return_string = "No command provided\n";
    } else if (commands.at(0).compare("path") == 0) {
      std::ostringstream oss;
      std::vector<std::vector<unsigned int> > paths = g.homogeneous_dfs((unsigned int) stoi(commands.at(1)),
                                                                        (unsigned int) stoi(commands.at(2)),
                                                                        (unsigned int) stoi(commands.at(3)),
                                                                        commands.size() == 4 ||
                                                                        is_true(commands.at(4)));

      oss << "find " << paths.size() << " paths\n";
      if (commands.size() > 5 && is_true(commands.at(5))) {
        for (auto it = paths.cbegin(); it != paths.cend(); ++it) {
          for (auto itt = it->cbegin(); itt != it->cend(); ++itt) {
            oss << g.get_node_type(*itt) << "--";
          }
          oss << "\n";
        }
      } else {
        for (auto it = paths.cbegin(); it != paths.cend(); ++it) {
          for (auto itt = it->cbegin(); itt != it->cend(); ++itt) {
            oss << *itt << "--";
          }
          oss << "\n";
        }
      }
      return_string = oss.str();

    } else if (commands.at(0).compare("hpath") == 0) {
      std::ostringstream oss;
      std::pair<std::vector<std::vector<std::pair<unsigned int, unsigned int> > >, std::vector<std::vector<bool> > > hpaths = g.heterogeneous_dfs(
          (unsigned int) stoi(commands.at(1)),
          (unsigned int) stoi(commands.at(2)),
          (unsigned int) stoi(commands.at(3)),
          commands.size() == 4 || is_true(commands.at(4)));

      std::vector<std::vector<std::pair<unsigned int, unsigned int> > > &paths = hpaths.first;
      std::vector<std::vector<bool> > &rel_paths = hpaths.second;

      oss << "find " << paths.size() << " paths\n";

      if (commands.size() > 5 && is_true(commands.at(5))) {
        size_t path_id = 0, path_pos = 0;
        for (auto it = paths.cbegin(); it != paths.cend(); ++it) {
          oss << g.get_node_type((unsigned int) stoi(commands.at(1))) << "-";
          path_pos = 0;
          for (auto itt = it->cbegin(); itt != it->cend(); ++itt) {
            oss << (!rel_paths.at(path_id).at(path_pos) ? "(" : "(-1)(") << g.get_edge_type(itt->second) << ")-" <<
            g.get_node_type(itt->first) << "-";
            path_pos++;
          }
          oss << std::endl;
          path_id++;
        }
      } else if (is_false(commands.at(5))) {
        size_t path_id = 0, path_pos = 0;
        for (auto it = paths.cbegin(); it != paths.cend(); ++it) {
          oss << commands.at(1) << "-";
          path_pos = 0;
          for (auto itt = it->cbegin(); itt != it->cend(); ++itt) {
            oss << (!rel_paths.at(path_id).at(path_pos) ? "(" : "(-1)(") << itt->second << ")-" << itt->first << "-";
            path_pos++;
          }
          path_id++;
          oss << std::endl;
        }
      } else {
        size_t path_id = 0, path_pos = 0;
        for (auto it = paths.cbegin(); it != paths.cend(); ++it) {
          path_pos = 0;
          for (auto itt = it->cbegin(); itt != it->cend(); ++itt) {
            oss << (!rel_paths.at(path_id).at(path_pos) ? 1 : -1) * long(itt->second) << ",";
            path_pos++;
          }
          path_id++;
          oss << std::endl;
        }
      }

      return_string = oss.str();

    } else if (commands.at(0) == "in_neighbor") {
      std::ostringstream oss;
      const std::set<unsigned int> &neighbors = g.get_in_edges((unsigned int) stoi(commands.at(1)));
      for (auto it = neighbors.cbegin(); it != neighbors.cend(); ++it) {
        oss << *it << ",";
      }
      oss << "\n";
      return_string = oss.str();
    } else if (commands.at(0) == "out_neighbor") {
      std::ostringstream oss;
      const std::set<unsigned int> &neighbors = g.get_out_edges((unsigned int) stoi(commands.at(1)));
      for (auto it = neighbors.cbegin(); it != neighbors.cend(); ++it) {
        oss << *it << ",";
      }
      oss << "\n";
      return_string = oss.str();
    } else if (commands.at(0) == "ontology") {
      std::vector<unsigned int> ontology = g.get_ontology((unsigned int) stoi(commands.at(1)));
      std::ostringstream oss;
      if (commands.size() >= 3 && is_true(commands.at(2))) {
        for (auto it = ontology.begin(); it != ontology.end(); ++it) {
          oss << g.get_node_type(*it) << ",";
        }
      } else {
        for (auto it = ontology.begin(); it != ontology.end(); ++it) {
          oss << *it << ",";
        }
      }

      oss << "\n";
      return_string = oss.str();
    } else if (commands.at(0) == "siblings") {
      std::ostringstream oss;
      std::vector<std::pair<unsigned int, std::set<unsigned int> > > siblings =
          g.get_ontology_siblings((unsigned int) stoi(commands.at(1)));
      if (commands.size() >= 3 && is_true(commands.at(2))) {
        for (auto it = siblings.begin(); it != siblings.end(); ++it) {
          oss << "[" << it->first << "]" << " ";
          for (auto itt = it->second.begin(); itt != it->second.end(); ++it) {
            oss << g.get_node_type(*itt) << ",";
          }
          oss << "\n";
        }
      } else {
        for (auto it = siblings.begin(); it != siblings.end(); ++it) {
          oss << "[" << it->first << "]" << " ";
          for (auto itt = it->second.begin(); itt != it->second.end(); ++it) {
            oss << *itt << ",";
          }
          oss << "\n";
        }
      }
      oss << "\n";
      return_string = oss.str();
    } else if (commands.at(0) == "nsiblings") {
      std::ostringstream oss;
      std::vector<std::pair<unsigned int, unsigned int> > res = g.get_ontology_sibling_count(
          (unsigned int) stoi(commands.at(1)));

      if (commands.size() >= 3 && is_true(commands.at(2))) {
        for (auto it = res.begin(); it != res.end(); ++it) {
          oss << g.get_node_type(it->first) << "," << it->second << ";";
        }
      } else {
        for (auto it = res.begin(); it != res.end(); ++it) {
          oss << it->first << "," << it->second << ";";
        }
      }

      oss << "\n";
      return_string = oss.str();

    } else if (commands.at(0) == "aa") {
      return_string = std::to_string(
          g.adamic_adar((unsigned int) stoi(commands.at(1)), (unsigned int) stoi(commands.at(2))));
    } else if (commands.at(0) == "haa") {
      return_string = std::to_string(
          g.heter_adamic_adar((unsigned int) stoi(commands.at(1)), (unsigned int) stoi(commands.at(2)),
                              (unsigned int) stoi(commands.at(3))));
    } else if (commands.at(0) == "sp") {
      return_string = std::to_string(
          g.semantic_proximity((unsigned int) stoi(commands.at(1)), (unsigned int) stoi(commands.at(2))));
    } else if (commands.at(0) == "maa") {
      return_string = std::to_string(
          g.multidimensional_adamic_adar((unsigned int) stoi(commands.at(1)), (unsigned int) stoi(commands.at(2)),
                                         (unsigned int) stoi(commands.at(3))));

    } else if (commands.at(0) == "ppr") {
      return_string = std::to_string(
          g.personalize_pagerank((unsigned int) stoi(commands.at(1)), (unsigned int) stoi(commands.at(2)),
                                 0.15, 0.00001, 20, false));
    } else if (commands.at(0) == "pa") {
      return_string = std::to_string(g.preferential_attachment((unsigned int) stoi(commands.at(1)),
                                                               (unsigned int) stoi(commands.at(2))));
    } else if (commands.at(0) == "katz") {
      return_string = std::to_string(
          g.katz((unsigned int) stoi(commands.at(1)),
                 (unsigned int) stoi(commands.at(2))));
    } else {
      return_string = "Unsupported command\n";
    }
  } catch (std::exception error) {
    std::cerr << "Error occurred when executing command " << buf.elems << ". Error is " << error.what() << std::endl;
    return_string = error.what();
  }

  try {
    len = boost::asio::write(*socket, boost::asio::buffer(return_string, return_string.size()),
                             boost::asio::transfer_all());

    if (len != return_string.size()) {
      std::cerr << "write error, wrote " << len << " bytes, " <<
      return_string.size() << " expected.\n";
    } else {
      std::cerr << "wrote " << len << " bytes to client\n";
    }
    socket->close();
    delete socket;
  } catch (std::exception &exception) {
    std::cerr << "write error, " << exception.what() <<
    ". Please enlarge timeout time when trying to get very long path\n";
  }
}


class socket_server {

  boost::asio::io_service worker_io_service;
  boost::thread_group threadpool;
  boost::asio::io_service::work *work;

public:

  socket_server(int nworkers = 10) {
    work = new boost::asio::io_service::work(worker_io_service);
    for (int i = 0; i < nworkers; i++) {
      threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &worker_io_service));
    }

    std::cout << threadpool.size() << " threads are created\n";
  }

  void start(std::string socket_name, graph<std::string, std::string> &g) {
    boost::asio::io_service socket_io_service;
    ::unlink(socket_name.c_str());
    local::stream_protocol::endpoint ep(socket_name);
    local::stream_protocol::acceptor acceptor(socket_io_service, ep);

    while (true) { //TODO: Throw Broken pipe error when all workers are busy and a lot of requests are coming.
      local::stream_protocol::socket *socket = new local::stream_protocol::socket(socket_io_service);
      acceptor.accept(*socket);
      worker_io_service.post(boost::bind(worker, socket, g));
    }
  }

  void cleanup() {
    worker_io_service.stop();
    threadpool.join_all();
  }


};

#endif //GBPEDIA_SOCKET_SERVER_H
