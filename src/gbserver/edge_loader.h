//
// Created by Baoxu Shi on 6/12/15.
//

#ifndef GBPEDIA_EDGE_LOADER_H
#define GBPEDIA_EDGE_LOADER_H

#include "edge_list.h"

#include <vector>
#include <fstream>
#include <algorithm>
#include <utility>

class edge_loader {

private:

  std::vector<edge_list> edge_map;
  uint max_id, max_rel, nedges;

public:

  edge_loader(std::string edge_filepath, bool is_directed = false) : max_id(0), max_rel(0), nedges(0) {
    std::fstream fin(edge_filepath, std::fstream::in);

    uint src, dst, rel;
    char sep;

    while(!fin.eof()) {
      fin >> src >> sep >> dst >> sep >> rel;

      if (std::max(src, dst) >= edge_map.size()) { /* Enlarge edge_map so it can contain certain number of nodes */
        edge_map.resize(std::max(src, dst) + 1u, edge_list());
      }
      
      /* Log forward edge */
      edge_map[src].connect_to(dst, rel);

      /* Log backward edge */
      if (!is_directed) edge_map[dst].connected_by(src, rel);

      if (max_id < std::max(src, dst)) max_id = std::max(src, dst);
      if (max_rel < rel) max_rel = rel;

      nedges++;
    }

    fin.close();
  }

  edge_list& get_edges(unsigned int src) {
    return edge_map.at(src);
  }

  unsigned int getMax_id() const {
    return max_id;
  }

  unsigned int getMax_rel() const {
    return max_rel;
  }

  unsigned int getNedges() const {
    return nedges;
  }

};


#endif //GBPEDIA_EDGE_LOADER_H
