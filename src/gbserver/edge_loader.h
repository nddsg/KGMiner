//
// Created by Baoxu Shi on 6/12/15.
//

#ifndef GBPEDIA_EDGE_LOADER_H
#define GBPEDIA_EDGE_LOADER_H


#include <vector>
#include <fstream>
#include <algorithm>
#include <utility>
#include <boost/unordered_map.hpp>

class edge_loader {

public:

private:
  
  class edge_type {
    std::set< std::pair<uint, uint> > forward; /* Directed edge*/
    std::set< std::pair<uint, uint> > backward; /* Reversed edge for mimicking undirected graph */
  public:

    edge_type() : forward(std::set< std::pair<uint, uint> >()),
                  backward(std::set< std::pair<uint, uint> >()){}

    inline void connect_to(uint target, uint rel) {
      forward.insert(std::pair<uint, uint>(target, rel));
    }
    inline void connected_by(uint source, uint rel) {
      backward.insert(std::pair<uint, uint>(source, rel));
    }
    inline void disconnect_to(uint target, uint rel) {
      forward.erase(std::pair<uint, uint>(target, rel));
    }
    inline void disconnected_by(uint source, uint rel) {
      backward.erase(std::pair<uint, uint>(source, rel));
    }
  };

  std::vector<edge_type> edge_map;
  uint max_id, max_rel, nedges;

public:

  edge_loader(std::string edge_filepath, bool is_directed = false) : max_id(0), max_rel(0), nedges(0) {
    std::fstream fin(edge_filepath, std::fstream::in);

    uint src, dst, rel;
    char sep;

    while(!fin.eof()) {
      fin >> src >> sep >> dst >> sep >> rel;

      if (std::max(src, dst) >= edge_map.size()) { /* Enlarge edge_map so it can contain certain number of nodes */
        edge_map.resize(std::max(src, dst) + 1u, edge_type());
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
