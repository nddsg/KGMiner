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

template<typename ND, typename TD>
class graph {

  typedef ND node_type;
  typedef TD edge_type;

  std::shared_ptr<node_loader<node_type> > nodes_ptr;
  std::shared_ptr<edge_loader> edges_ptr;
  std::shared_ptr<type_loader<edge_type> > edgetypes_ptr;

  inline void is_node_valid(unsigned int id) {
    if (!nodes_ptr->exists(id)) {
      throw std::runtime_error("Nodes " + std::to_string(id) + " does not exist.");
    }
  }

  inline bool is_loop(std::vector<std::pair<unsigned int, unsigned int> > &path, unsigned int id) noexcept {
    for (auto it = path.cbegin(); it != path.cend(); ++it) {
      if (it->first == id) return true;
    }
    return false;
  }


  /**
   * Homogeneous dfs helper
   */
  void dfs_helper(unsigned int src, unsigned int dst,
                  unsigned depth, unsigned max_depth,
                  std::vector<unsigned int> &tmp_path,
                  std::set<unsigned int> &visited,
                  std::vector<std::vector<unsigned int> > &result,
                  bool is_directed) {


    if (tmp_path.size() > 0 && tmp_path.size() <= max_depth && tmp_path.back() == dst) {
      result.push_back(tmp_path);
      return;
    }

    if (max_depth == depth) return;

    bool dst_visited = false; // when forward edges can reach destination,
    // we need to ignore those edges in backward mode so we do not introduce duplicated edges

    edge_list &edges = edges_ptr->get_edges(src);
    for (auto it = edges.get_forward().cbegin();
         it != edges.get_forward().cend(); ++it) {
      if (visited.find(it->first) == visited.end() || it->first == dst) { // never visited
        tmp_path.push_back(it->first);
        dfs_helper(it->first, dst, depth + 1, max_depth, tmp_path, visited, result, is_directed);
        tmp_path.pop_back();
        visited.insert(it->first);
        if (it->first == dst) {
          dst_visited = true;
        }
      }
    }

    if (!is_directed) {
      for (auto it = edges.get_backward().cbegin();
           it != edges.get_backward().cend(); ++it) {
        if (visited.find(it->first) == visited.end() || (it->first == dst && !dst_visited)) {
          tmp_path.push_back(it->first);
          dfs_helper(it->first, dst, depth + 1, max_depth, tmp_path, visited, result, is_directed);
          tmp_path.pop_back();
          visited.insert(it->first);
        }
      }
    }

  }


  /**
   * Heterogeneous dfs helper
   */
  //TODO: When working on reversed path, mark the rel as negative?
  void dfs_helper(unsigned int src, unsigned int dst,
                  unsigned depth, unsigned max_depth,
                  std::vector<std::pair<unsigned int, unsigned int> > &tmp_path,
                  std::vector<std::vector<std::pair<unsigned int, unsigned int> > > &result,
                  bool is_directed) {
    if (tmp_path.size() > 0 && tmp_path.size() <= max_depth && tmp_path.back().first == dst) {
      result.push_back(tmp_path);
      return;
    }

    if (max_depth == depth) return;

    edge_list &edges = edges_ptr->get_edges(src);
    for (auto it = edges.get_forward().cbegin();
         it != edges.get_forward().cend(); ++it) {
      if (!is_loop(tmp_path, it->first)) {
        tmp_path.push_back(*it);
        dfs_helper(it->first, dst, depth + 1, max_depth, tmp_path, result, is_directed);
        tmp_path.pop_back();
      }
    }

    if (!is_directed) {
      for (auto it = edges.get_backward().cbegin();
           it != edges.get_backward().cend(); ++it) {
        if (!is_loop(tmp_path, it->first)) {
          tmp_path.push_back(*it);
          dfs_helper(it->first, dst, depth + 1, max_depth, tmp_path, result, is_directed);
          tmp_path.pop_back();
        }
      }
    }

  }

public:

  graph() : nodes_ptr(nullptr), edges_ptr(nullptr), edgetypes_ptr(nullptr) { };

  graph(node_loader<node_type> &nodes, edge_loader &edges, type_loader<edge_type> &edgetypes) :
      nodes_ptr(&nodes),
      edges_ptr(&edges),
      edgetypes_ptr(&edgetypes) { };

  std::vector<unsigned int> get_out_edges(unsigned src) {
    is_node_valid(src);
    std::vector<unsigned int> result;
    edge_list &edges = edges_ptr->get_edges(src);
    for (auto it = edges.get_forward().cbegin(); it != edges.get_forward().cend(); ++it) {
      result.push_back(it->first);
    }
    return (result);

  }

  std::vector<unsigned int> get_in_edges(unsigned src) {
    is_node_valid(src);
    std::vector<unsigned int> result;
    edge_list &edges = edges_ptr->get_edges(src);
    for (auto it = edges.get_backward().cbegin(); it != edges.get_backward().cend(); ++it) {
      result.push_back(it->first);
    }
    return (result);
  }

  //TODO: Remove duplicate result, for example A->B and A<-B in undirected graph
  std::vector<std::vector<unsigned int> > homogeneous_dfs(unsigned int src,
                                                          unsigned int dst,
                                                          unsigned int depth = 4,
                                                          bool is_directed = true) {
    is_node_valid(src);
    is_node_valid(dst);

    std::vector<std::vector<unsigned int> > result;

    try {
      std::vector<unsigned int> tmp_path;
      std::set<unsigned int> visited;
      tmp_path.push_back(src);
      visited.insert(src);
      dfs_helper(src, dst, 1u, depth, tmp_path, visited, result, is_directed);
      assert(tmp_path.size() == 1); // only source node is in it.
    } catch (std::exception error) {
      std::cerr << "Error occurred when performing dfs, " << error.what() << std::endl;
    }

    return result;

  };

  std::vector<std::vector<std::pair<unsigned int, unsigned int> > > heterogeneous_dfs(unsigned int src,
                                                                                      unsigned int dst,
                                                                                      unsigned int depth = 4,
                                                                                      bool is_directed = true) {
    is_node_valid(src);
    is_node_valid(dst);

    std::vector<std::vector<std::pair<unsigned int, unsigned int> > > result;

    try {
      std::vector<std::pair<unsigned int, unsigned int> > tmp_path;
      // We do not keep a visited list in heterogeneous path finding because
      //every path has its own edge/node type sequence and keeping a visited node set
      //will result in only returning the first meta-path that touches the node.
      dfs_helper(src, dst, 1u, depth, tmp_path, result, is_directed);
          assert(tmp_path.size() == 0);
    } catch (std::exception error) {
      std::cerr << "Error occurred when performing heterogeneous dfs, " << error.what() << std::endl;
    }

    return result;
  }

  node_type get_node_type(unsigned int id) {
    return nodes_ptr->get_value(id);
  }

  edge_type get_edge_type(unsigned int id) {
    return edgetypes_ptr->get_value(id);
  }

};

#endif //GBPEDIA_GRAPH_H