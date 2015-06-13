//
// Created by Baoxu Shi on 6/11/15.
//

#ifndef GBPEDIA_OPTS_H
#define GBPEDIA_OPTS_H

#include <boost/program_options.hpp>
  class opts {
    boost::program_options::options_description desc;
    boost::program_options::positional_options_description p;

  private:
    std::string format;
    std::vector<std::string> input_files;
    std::string output_prefix;

  public:
    opts();

    bool parse(int argc, const char* argv[]);

    const std::string &getFormat() const {
      return format;
    }

    const std::vector<std::string> &getInput_files() const {
      return input_files;
    }

    const std::string &getOutput_prefix() const {
      return output_prefix;
    }

  };



#endif //GBPEDIA_OPTS_H
