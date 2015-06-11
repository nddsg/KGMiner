//
// Created by Baoxu Shi on 6/11/15.
//

#ifndef GBPEDIA_OPTS_H
#define GBPEDIA_OPTS_H

#include <boost/program_options.hpp>
  class opts {
    boost::program_options::options_description desc;
    boost::program_options::positional_options_description p;

    std::string format;
    std::vector<std::string> input_files;

  public:
    opts();

    bool parse(int argc, const char* argv[]);

  };



#endif //GBPEDIA_OPTS_H
