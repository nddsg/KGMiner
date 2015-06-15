//
// Created by Baoxu Shi on 6/13/15.
//

#ifndef GBPEDIA_GRAPH_H
#define GBPEDIA_GRAPH_H

#include "node_loader.h"
#include "edge_loader.h"
#include "type_loader.h"
#include "edge_list.h"

#include <set>

template <typename ND, typename TD>
class graph {

  typedef ND node_type;
  typedef TD edge_type;

  std::shared_ptr< node_loader<node_type> > nodes_ptr;
  std::shared_ptr<edge_loader> edges_ptr;
  std::shared_ptr< type_loader<edge_type> > edgetypes_ptr;

  /**
   * Directed dfs helper
   */
  void dfs_helper(unsigned int src, unsigned int dst,
                  unsigned depth, unsigned max_depth,
                  std::vector<unsigned int>& tmp_path,
                  std::set<unsigned int>& visited,
                  std::vector< std::vector<unsigned int> >& result,
                  bool is_directed) {


    if (tmp_path.size() > 0 && tmp_path.size() <= max_depth && tmp_path.back() == dst) {
      result.push_back(tmp_path);
      return;
    }

    if (max_depth == depth) return;

    edge_list &edges = edges_ptr->get_edges(src);
    for(auto it = edges.get_forward().cbegin();
        it != edges.get_forward().cend(); ++it) {
        if (visited.find(it->first) == visited.end()) { // never visited
          tmp_path.push_back(it->first);
          dfs_helper(it->first, dst, depth + 1, max_depth, tmp_path, visited, result, is_directed);
          tmp_path.pop_back();
          visited.insert(it->first);
        }
    }

    if (!is_directed) {
      for(auto it = edges.get_backward().cend();
          it != edges.get_backward().cend(); ++it) {
        if (visited.find(it->first) == visited.end()) {
          tmp_path.push_back(it->first);
          dfs_helper(it->first, dst, depth + 1, max_depth, tmp_path, visited, result, is_directed);
          tmp_path.pop_back();
          visited.insert(it->first);
        }
      }
    }

  }

public:

  graph(): nodes_ptr(nullptr), edges_ptr(nullptr), edgetypes_ptr(nullptr) {};

  graph(node_loader<node_type> &nodes, edge_loader &edges, type_loader<edge_type> &edgetypes):
      nodes_ptr(&nodes),
      edges_ptr(&edges),
      edgetypes_ptr(&edgetypes) {};

  std::vector< std::vector<unsigned int> > dfs(unsigned int src,
                                               unsigned int dst,
                                               unsigned int depth = 4,
                                               bool is_directed = true){
    if (!nodes_ptr->exists(src) || !nodes_ptr->exists(dst)) {
      throw std::runtime_error("Nodes " + std::to_string(src) + " or " +
                               std::to_string(dst) + " does not exist.");
    }

    std::vector< std::vector<unsigned int> > result;

    try {
      std::vector<unsigned int> tmp_path;
      std::set<unsigned int> visited;
      tmp_path.push_back(src);
      visited.insert(src);
      dfs_helper(src, dst, 1u, depth, tmp_path, visited, result, is_directed);
      assert(tmp_path.size() == 1); // only source node is in it.
    } catch (std::exception error) {
      std::cout << "Error occurred when performing dfs, " << error.what() << std::endl;
    }

    return result;

  };


};

#endif //GBPEDIA_GRAPH_H