//
// Created by Baoxu Shi on 6/12/15.
//

#ifndef GBPEDIA_TYPE_LOADER_H
#define GBPEDIA_TYPE_LOADER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

template<typename T>
class type_loader {

public:
  typedef T value_type;

private:
  std::vector<value_type> type_map;
  unsigned int max_rel;

public:

  type_loader(std::string type_filepath) noexcept : max_rel(0), type_map(std::vector<value_type>()) {

    std::fstream fin(type_filepath, std::fstream::in);

    value_type val;
    unsigned int id;

    while (!fin.eof()) {
      fin >> id;
      fin.get(); // remove separator
      fin >> val;

      if (id >= type_map.size()) { /* Enlarge node_map so it can contain certain number of nodes */
        type_map.resize(id + 1u, "");
      }
      type_map[id] = val;

      if (max_rel < id) max_rel = id;
    }

    fin.close();
  };

  unsigned int getMax_rel() const noexcept {
    return max_rel;
  }

  value_type get_value(unsigned int id) const {
    if (id > getMax_rel()) {
      throw std::runtime_error(std::to_string(id) + " is larger than max id in typelist.");
    }
    return type_map.at(id);
  }

};

#endif //GBPEDIA_TYPE_LOADER_H
