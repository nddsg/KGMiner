//
// Created by Baoxu Shi on 6/12/15.
//

#ifndef GBPEDIA_NODE_H
#define GBPEDIA_NODE_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

template <typename T> class node_loader {

public:
  typedef T value_type;

private:
  std::vector<value_type> node_map;
  unsigned int max_id;

public:

  node_loader(std::string node_filepath) : max_id(0), node_map(std::vector<value_type>()) {

    std::fstream fin(node_filepath, std::fstream::in);

    value_type val;
    unsigned int id;

    while(!fin.eof()) {
      fin >> id;
      fin.get(); // remove separator
      fin >> val;

      if (id >= node_map.size()) { /* Enlarge node_map so it can contain certain number of nodes */
        node_map.resize(id + 1u, "");
      }
      node_map[id] = val;

      if (max_id < id) max_id = id;
    }

    fin.close();
  };

  unsigned int getMax_id() const {
    return max_id;
  }

};


#endif //GBPEDIA_NODE_H
