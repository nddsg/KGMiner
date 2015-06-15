//
// Created by Baoxu Shi on 6/13/15.
//

#ifndef GBPEDIA_EDGE_LIST_H
#define GBPEDIA_EDGE_LIST_H

#include <vector>
#include <fstream>
#include <algorithm>
#include <utility>
#include <set>

class edge_list {
  std::set< std::pair<uint, uint> > forward; /* Directed edge*/
  std::set< std::pair<uint, uint> > backward; /* Reversed edge for mimicking undirected graph */
public:

  edge_list() : forward(std::set< std::pair<uint, uint> >()),
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

  std::set< std::pair<uint, uint> >& get_forward() noexcept {return forward;}
  std::set< std::pair<uint, uint> >& get_backward() noexcept {return backward;}


};


#endif //GBPEDIA_EDGE_LIST_H
