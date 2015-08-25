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
  std::map<unsigned int, std::pair<std::set<unsigned int>, unsigned int> > node_type_count;
  uint max_id, max_rel, nedges;
    int entity_with_entology;

  const unsigned int type_rel;

public:

  //TODO: Automatically detect id of rel_type rdf-syntax-ns#type
  edge_loader(std::string edge_filepath, bool is_directed = false, unsigned int type_rel = 671) :
      max_id(0), max_rel(0), nedges(0), type_rel(type_rel), entity_with_entology(-1) {
    std::fstream fin(edge_filepath, std::fstream::in);

    uint src, dst, rel;
    char sep;

    while(!fin.eof()) {
      fin >> src >> sep >> dst >> sep >> rel;

      if (std::max(src, dst) >= edge_map.size()) { /* Enlarge edge_map so it can contain certain number of nodes */
        edge_map.resize(std::max(src, dst) + 1u, edge_list());
      }

      /* Log edge count regarding to edge type */
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

      /* Log type count when edge is A - type - B */
      if (rel == type_rel) {
        if (node_type_count.find(dst) == node_type_count.end()) {
          node_type_count[dst] = std::pair<std::set<unsigned int>, unsigned int>();
        }
        node_type_count[dst].second++;
        node_type_count[dst].first.insert(src);
      }

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
    return rel_type == 0 ? getMax_id() : edge_type_count.at(rel_type);
  }

  std::set<std::pair<unsigned int, unsigned int> > get_neighbors_except_rel(unsigned int id, unsigned int rel_type = 0,
                                                                            bool is_directed = false) {
    std::set<std::pair<unsigned int, unsigned int> > neighbors;
    for (auto it = get_edges(id).get_forward().cbegin(); it != get_edges(id).get_forward().cend(); ++it) {
      if (rel_type != it->second) {
        neighbors.insert(*it);
      }
    }
    if (!is_directed) {
      for (auto it = get_edges(id).get_backward().cbegin(); it != get_edges(id).get_backward().cend(); ++it) {
        if (rel_type != it->second) {
          neighbors.insert(*it);
        }
      }
    }
    return neighbors;
  }

  /**
   * Return neighbors of `id` if they are not connected by `discard_rel`.
   */
  std::set<unsigned int> get_neighbors(unsigned int id, unsigned int discard_rel = 0, bool is_directed = false) {
    std::set<unsigned int> neighbors;
    for (auto it = get_edges(id).get_forward().cbegin(); it != get_edges(id).get_forward().cend(); ++it) {
      if (discard_rel == 0 || discard_rel != it->second) {
        neighbors.insert(it->first);
      }
    }
    if (!is_directed) {
      for (auto it = get_edges(id).get_backward().cbegin(); it != get_edges(id).get_backward().cend(); ++it) {
        if (discard_rel == 0 || discard_rel != it->second) {
          neighbors.insert(it->first);
        }
      }
    }
    return neighbors;
  }

  std::set<unsigned int> get_neighbors_by_rel(unsigned int id, unsigned int rel, bool is_directed = false) {
    std::set<unsigned int> neighbors;
    for (auto it = get_edges(id).get_forward().cbegin(); it != get_edges(id).get_forward().cend(); ++it) {
      if (rel == it->second) {
        neighbors.insert(it->first);
      }
    }
    if (!is_directed) {
      for (auto it = get_edges(id).get_backward().cbegin(); it != get_edges(id).get_backward().cend(); ++it) {
        if (rel == it->second) {
          neighbors.insert(it->first);
        }
      }
    }
    return neighbors;
  }

  unsigned int get_neighbor_count_by_rel(unsigned int id, unsigned int rel, bool is_directed = false) {
    unsigned int res = 0;
    for (auto it = get_edges(id).get_forward().cbegin(); it != get_edges(id).get_forward().cend(); ++it) {
      if (rel == it->second) {
        res++;
      }
    }
    if (!is_directed) {
      for (auto it = get_edges(id).get_backward().cbegin(); it != get_edges(id).get_backward().cend(); ++it) {
        if (rel == it->second) {
          res++;
        }
      }
    }

    return res;
  }

  std::vector<std::pair<unsigned int, unsigned int> > get_common_neighbor_except_rel(unsigned int id1, unsigned int id2,
                                                                                     unsigned int rel_type = 0,
                                                                                     bool is_directed = false) {
    std::set<std::pair<unsigned int, unsigned int> > neighbors_1 = get_neighbors_except_rel(id1, rel_type, is_directed);
    std::set<std::pair<unsigned int, unsigned int> > neighbors_2 = get_neighbors_except_rel(id2, rel_type, is_directed);

    std::vector<std::pair<unsigned int, unsigned int> > common_neighbors;

    for (auto it = neighbors_1.cbegin(); it != neighbors_1.cend(); ++it) {
      if (neighbors_2.find(*it) != neighbors_2.end()) {
        common_neighbors.push_back(*it);
      }
    }

    return common_neighbors;
  }

  std::vector<unsigned int> get_common_neighbor(unsigned int id1, unsigned int id2, unsigned int discard_rel = 0,
                                                bool is_directed = false) {
    std::set<unsigned int> neighbors_1 = get_neighbors(id1, discard_rel, is_directed);
    std::set<unsigned int> neighbors_2 = get_neighbors(id2, discard_rel, is_directed);

    std::vector<unsigned int> common_neighbors;

    for (auto it = neighbors_1.cbegin(); it != neighbors_1.cend(); ++it) {
      if (neighbors_2.find(*it) != neighbors_2.end()) {
        common_neighbors.push_back(*it);
      }
    }

    return common_neighbors;
  }

  std::vector<unsigned int> get_ontology(unsigned int id) {
    std::vector<unsigned int> types;
    std::set<std::pair<unsigned int, unsigned int> > &edges = get_edges(id).get_forward();
    for (auto it = edges.begin(); it != edges.end(); ++it) {
      if (it->second == type_rel) { // this is an ontology edge
        types.push_back(it->first);
      }
    }
    sort(types.begin(), types.end());
    return types;
  }

  std::set<unsigned int> get_ontology_set(unsigned int id) {
    std::set<unsigned int> types;
    std::set<std::pair<unsigned int, unsigned int> > &edges = get_edges(id).get_forward();
    for (auto it = edges.begin(); it != edges.end(); ++it) {
      if (it->second == type_rel) { // this is an ontology edge
        types.insert(it->first);
      }
    }

    return types;
  }

  /**
   * Return vertices sharing the same ontology set
   *
   * //TODO: pass const reference instead of value copy to save time and memory
   */
  std::vector<std::pair<unsigned int, std::set<unsigned int> > > get_ontology_siblings(unsigned int id) {

    std::vector<std::pair<unsigned int, std::set<unsigned int> > > res;

    std::vector<unsigned int> node_ontology = get_ontology(id);
    for (int i = 0; i < node_ontology.size(); i++) {
      res.push_back(std::pair<unsigned int, std::set<unsigned int> >(node_ontology[i],
                                                                     node_type_count[node_ontology[i]].first));
    }

    return res;
  }

  //TODO: Implement tol based filtering
  std::set<unsigned int> get_ontology_siblings(unsigned int id, double tol) {

    std::set<unsigned int> res;

    std::set<unsigned int> ontology = get_ontology_set(id);

    for (auto it_ontology = ontology.cbegin(); it_ontology != ontology.cend(); ++it_ontology) {
      for (auto it_entity = node_type_count[*it_ontology].first.cbegin();
           it_entity != node_type_count[*it_ontology].first.cend(); ++it_entity) {
        if (ontology == get_ontology_set(*it_entity)) {
          res.insert(*it_entity);
        }
      }
    }

    return res;
  }

  std::vector<std::pair<unsigned int, unsigned int> > get_ontology_sibling_count(unsigned int id) {
    std::vector<std::pair<unsigned int, unsigned int> > res;

    std::vector<unsigned int> node_ontology = get_ontology(id);
    for (int i = 0; i < node_ontology.size(); i++) {
      res.push_back(std::pair<unsigned int, unsigned int>(node_ontology[i],
                                                          node_type_count[node_ontology[i]].second));
    }

    return res;
  }

    int get_nentity_with_ontology() {
      if (entity_with_entology == -1) {
        entity_with_entology = 0;

        for (int id = 0; id < max_id; id++) {
          std::set<std::pair<unsigned int, unsigned int> > &edges = get_edges(id).get_forward();
          for (auto it = edges.begin(); it != edges.end(); ++it) {
            if (it->second == type_rel) { // this is an ontology edge
              entity_with_entology++;
              break;
            }
          }
        }
      }

      return entity_with_entology;
    }

  unsigned int get_ontology_count(unsigned int id) {
    return node_type_count[id].second;
  }

  inline unsigned int get_type_rel() { return type_rel; }

};


#endif //GBPEDIA_EDGE_LOADER_H
