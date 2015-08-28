//
// Created by Baoxu Shi on 6/11/15.
//

#include "opts.h"

#include <iostream>

opts::opts() : desc("Allowed options") {

    std::string port_desc = "Port number, default is " + std::to_string(port);

    desc.add_options()
        ("help,h", "display help message")
        ("edge,e", boost::program_options::value<std::string>()->required(),
         "EdgeList filepath")
        ("node,n", boost::program_options::value<std::string>()->required(),
         "NodeList filepath")
        ("type,t", boost::program_options::value<std::string>()->required(),
         "EdgeTypeList filepath")
        ("directed,d", "Directed graph")
        ("port,p", boost::program_options::value<int>(),
         port_desc.c_str())
        ("worker,w", boost::program_options::value<int>(), "Number of workers, default is 10")
        ("ontology,o", boost::program_options::value<unsigned int>(),
         "ontology relation type, the default value is 671");
  }

  bool opts::parse(int argc, const char *argv[]) {
    try {
      boost::program_options::variables_map vm;
      boost::program_options::store(boost::program_options::command_line_parser(argc, argv)
                                        .options(opts::desc).run(), vm);

      if (vm.count("help")) {
        desc.print(std::cout);
        return false;
      }

      /* Throw exception if missing important options */
      boost::program_options::notify(vm);

      if (vm.count("edge")) {
        edge_filepath = vm["edge"].as<std::string>();
      }
      if (vm.count("node")) {
        node_filepath = vm["node"].as<std::string>();
      }
      if (vm.count("type")) {
        type_filepath = vm["type"].as<std::string>();
      }
      if (vm.count("port")) {
        port = vm["port"].as<int>();
      }
      if (vm.count("worker")) {
        nworker = vm["worker"].as<int>();
      }
      if (vm.count("ontology")) {
        rel_type = vm["ontology"].as < unsigned
        int > ();
      }
      is_directed = vm.count("directed") == 1;
    } catch (std::exception& err) {
      std::cout << err.what() << std::endl;
      return false;
    }
    catch(...) {
      std::cout << "Unknown error" << std::endl;
      return false;
    }

    return true;
  }
