//
// Created by Baoxu Shi on 6/11/15.
//

#include "opts.h"

#include <iostream>
  opts::opts() : desc("Allowed options") {

    p.add("input-file", -1);

    desc.add_options()
        ("help,h", "display help message")
        ("input-file", boost::program_options::value<std::vector<std::string> >()->required(),
         "RDF files contains infobox property")
        ("format,f", boost::program_options::value<std::string>(),
         "output format, can be graphml or compact, the default is compact");
  }

  bool opts::parse(int argc, const char *argv[]) {
    try {
      boost::program_options::variables_map vm;
      boost::program_options::store(boost::program_options::command_line_parser(argc, argv)
                                        .options(opts::desc).positional(opts::p).run(), vm);

      if (vm.count("help")) {
        desc.print(std::cout);
        return false;
      }

      /* Throw exception if missing important options */
      boost::program_options::notify(vm);

      if (vm.count("input-file")) {
        input_files = vm["input-file"].as<std::vector<std::string> >();
      }
      if (vm.count("format")) {
        format = vm["format"].as<std::string>();
      }
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
