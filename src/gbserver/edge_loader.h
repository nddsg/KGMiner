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
  std::vector<unsigned int> edge_type_count;
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

      /* Log node count and edge count regarding to edge type */
      if (rel >= edge_type_count.size()) {
        edge_type_count.resize(rel + 1u, 0);
      }
      edge_type_count[rel]++;

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

  unsigned int get_edge_type_count(unsigned int rel_type) const {
    return edge_type_count.at(rel_type);
  }

  std::set<unsigned int> get_neighbors(unsigned int id, unsigned int rel_type = 0, bool is_directed = false) {
    std::set<unsigned int> neighbors;
    for (auto it = get_edges(id).get_forward().cbegin(); it != get_edges(id).get_forward().cend(); ++it) {
      if (rel_type == 0 || rel_type == it->second) {
        neighbors.insert(it->first);
      }
    }
    if (!is_directed) {
      for (auto it = get_edges(id).get_backward().cbegin(); it != get_edges(id).get_backward().cend(); ++it) {
        if (rel_type == 0 || rel_type == it->second) {
          neighbors.insert(it->first);
        }
      }
    }
    return neighbors;
  }

  std::vector<unsigned int> get_common_neighbor(unsigned int id1, unsigned int id2, unsigned int rel_type = 0,
                                                bool is_directed = false) {
    std::set<unsigned int> neighbors_1 = get_neighbors(id1, rel_type, is_directed);
    std::set<unsigned int> neighbors_2 = get_neighbors(id2, rel_type, is_directed);

    std::vector<unsigned int> common_neighbors;

    for (auto it = neighbors_1.cbegin(); it != neighbors_1.cend(); ++it) {
      if (neighbors_2.find(*it) != neighbors_2.end()) {
        common_neighbors.push_back(*it);
      }
    }

    return common_neighbors;
  }

};


#endif //GBPEDIA_EDGE_LOADER_H
