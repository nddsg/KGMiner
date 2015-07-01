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
#include <tuple>
#include <algorithm>
#include <cmath>

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
                  bool is_directed, bool depth_only) {
    if (tmp_path.size() > 0 && tmp_path.size() <= max_depth && tmp_path.back() == dst) {
      if (!depth_only || (depth_only && tmp_path.size() == max_depth)) {
        result.push_back(tmp_path);
      }
      return;
    }

    if (max_depth == depth) return;

    bool dst_visited = false; // on typed network, there may be multiple edges
    // connects to the same target with different edge types,
    // we need to ignore these in an untyped setting. This can be done using visited_set
    // but it can not work with the target node.

    edge_list &edges = edges_ptr->get_edges(src);
    for (auto it = edges.get_forward().cbegin();
         it != edges.get_forward().cend(); ++it) {
      if (visited.find(it->first) == visited.end() || (it->first == dst && !dst_visited)) { // never visited
        tmp_path.push_back(it->first);
        dfs_helper(it->first, dst, depth + 1, max_depth, tmp_path, visited, result, is_directed, depth_only);
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
          dfs_helper(it->first, dst, depth + 1, max_depth, tmp_path, visited, result, is_directed, depth_only);
          tmp_path.pop_back();
          visited.insert(it->first);
        }
      }
    }

  }


  /**
   * Heterogeneous dfs helper
   */
  void dfs_helper(unsigned int src, unsigned int dst,
                  unsigned depth, unsigned max_depth,
                  std::vector<std::pair<unsigned int, unsigned int> > &tmp_path,
                  std::vector<bool> &reverted_rel,
                  std::set<unsigned int> &visited, // only contains nodes in the current path
                  std::vector<std::vector<std::pair<unsigned int, unsigned int> > > &result,
                  std::vector<std::vector<bool> > &rel_result,
                  bool is_directed) {
    if (tmp_path.size() > 0 && tmp_path.size() <= max_depth && tmp_path.back().first == dst) {
      result.push_back(tmp_path);
      rel_result.push_back(reverted_rel);
      return;
    }

    if (max_depth == depth) return;

    edge_list &edges = edges_ptr->get_edges(src);
    for (auto it = edges.get_forward().cbegin();
         it != edges.get_forward().cend(); ++it) {
      if (visited.find(it->first) == visited.end()) {
        tmp_path.push_back(*it);
        reverted_rel.push_back(false);
        visited.insert(it->first);
        dfs_helper(it->first, dst, depth + 1, max_depth, tmp_path, reverted_rel, visited, result, rel_result,
                   is_directed);
        tmp_path.pop_back();
        reverted_rel.pop_back();
        visited.erase(it->first);
      }
    }

    if (!is_directed) {
      for (auto it = edges.get_backward().cbegin();
           it != edges.get_backward().cend(); ++it) {
        if (visited.find(it->first) == visited.end()) {
          tmp_path.push_back(*it);
          reverted_rel.push_back(true);
          visited.insert(it->first);
          dfs_helper(it->first, dst, depth + 1, max_depth, tmp_path, reverted_rel, visited, result, rel_result,
                     is_directed);
          tmp_path.pop_back();
          reverted_rel.pop_back();
          visited.erase(it->first);
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

  const std::set<unsigned int> &get_out_edges(unsigned src) {
    is_node_valid(src);
    return edges_ptr->get_edges(src).get_out_neighbors();

  }

  const std::set<unsigned int> &get_in_edges(unsigned src) {
    is_node_valid(src);
    return edges_ptr->get_edges(src).get_in_neighbors();
  }

  std::vector<std::vector<unsigned int> > homogeneous_dfs(unsigned int src,
                                                          unsigned int dst,
                                                          unsigned int depth = 4,
                                                          bool is_directed = true, bool depth_only = false) {
    is_node_valid(src);
    is_node_valid(dst);

    std::vector<std::vector<unsigned int> > result;

    try {
      std::vector<unsigned int> tmp_path;
      std::set<unsigned int> visited;
      tmp_path.push_back(src);
      visited.insert(src);
      dfs_helper(src, dst, 1u, depth, tmp_path, visited, result, is_directed, depth_only);
      assert(tmp_path.size() == 1); // only source node is in it.
    } catch (std::exception error) {
      std::cerr << "Error occurred when performing dfs, " << error.what() << std::endl;
    }

    return result;

  };

  std::pair<std::vector<std::vector<std::pair<unsigned int, unsigned int> > >, std::vector<std::vector<bool> > > heterogeneous_dfs(
      unsigned int src,
      unsigned int dst,
      unsigned int depth = 4,
      bool is_directed = true) {
    is_node_valid(src);
    is_node_valid(dst);

    std::vector<std::vector<std::pair<unsigned int, unsigned int> > > path_result;
    std::vector<std::vector<bool> > rel_result;

    try {
      std::vector<std::pair<unsigned int, unsigned int> > tmp_path;
      std::vector<bool> tmp_reverted;
      std::set<unsigned int> visited;
      visited.insert(src);
      dfs_helper(src, dst, 1u, depth, tmp_path, tmp_reverted, visited, path_result, rel_result, is_directed);
      assert(tmp_path.size() == 0);
    } catch (std::exception error) {
      std::cerr << "Error occurred when performing heterogeneous dfs, " << error.what() << std::endl;
    }

    assert(path_result.size() == rel_result.size());

    return std::pair<std::vector<std::vector<std::pair<unsigned int, unsigned int> > >, std::vector<std::vector<bool> > >(
        path_result, rel_result);
  }

  node_type get_node_type(unsigned int id) {
    return nodes_ptr->get_value(id);
  }

  edge_type get_edge_type(unsigned int id) {
    return edgetypes_ptr->get_value(id);
  }

  /**
   * [1]	L. A. Adamic and E. Adar,
   *      “Friends and neighbors on the Web,” Social Networks, vol. 25, no. 3, pp. 211–230, Jul. 2003.
   */
  double adamic_adar(unsigned int id1, unsigned id2, unsigned int rel_type = 0) {
    is_node_valid(id1);
    is_node_valid(id2);

    std::vector<unsigned int> common_neighbors = edges_ptr->get_common_neighbor(id1, id2, rel_type, false);

    double result = 0.0;

    for (auto it = common_neighbors.cbegin(); it != common_neighbors.cend(); ++it) {
      size_t degree = edges_ptr->get_edges(*it).get_deg();
      result += 1.0 / log(degree);
    }

    return result;
  }

  /**
   * [1]	G. L. Ciampaglia, P. Shiralkar, L. M. Rocha, J. Bollen, F. Menczer, and A. Flammini,
   *      “Computational fact checking from knowledge networks,” Physical review E, vol. cs.CY. 14-Jan-2015.
   */
  double semantic_proximity(unsigned int src, unsigned dst) {
    std::vector<std::vector<unsigned int> > paths = homogeneous_dfs(src, dst, 4, false);
    // Remove direct connected link
    auto it = paths.begin();
    while (it != paths.end()) {
      if (it->size() == 2) {
        it = paths.erase(it);
      } else {
        it++;
      }
    }

    double result = 0;
    for (auto it = paths.cbegin(); it != paths.cend(); ++it) {
      double denominator = 1;
      for (auto itt = it->cbegin() + 1; itt != it->cend() - 1; ++itt) {
        denominator += log(edges_ptr->get_edges(*itt).get_deg());
      }
      result = result > 1.0 / denominator ? result : 1.0 / denominator;
    }
    return result;
  }

  /**
   * [1]	G. Rossetti, M. Berlingerio, and F. Giannotti,
   *      “Scalable Link Prediction on Multidimensional Networks.,” ICDM Workshops, pp. 979–986, 2011.
   *
   * Multidimensional Adamic Adar * Edge Dimension Connectivity
   */
  double multidimensional_adamic_adar(unsigned int id1, unsigned int id2, unsigned int rel_type) {
    is_node_valid(id1);
    is_node_valid(id2);

    double maa = adamic_adar(id1, id2, rel_type);

    double ndc = double(edges_ptr->get_edge_type_count(rel_type)) / edges_ptr->getNedges();

    return maa * ndc;

  }


  /**
   * Heterogeneous Adamic Adar
   *
   * Instead of calculating common neighbors, we calculate common edge relations.
   * First we find the number of common relations, then we sum up \frac{#rel in the same node}{}
   *
   * sum(#Rel / #Edges)
   *
   */
  double heter_adamic_adar(unsigned int id1, unsigned int id2, unsigned int rel_type) {
    is_node_valid(id1);
    is_node_valid(id2);

    std::vector<std::pair<unsigned int, unsigned int> > common_neighbors = edges_ptr->get_common_neighbor_except_rel(
        id1, id2, rel_type, false);

    double result = 0.0;

    for (auto it = common_neighbors.cbegin(); it != common_neighbors.cend(); ++it) {

      if (it->second == edges_ptr->get_type_rel()) {
        size_t degree = edges_ptr->get_ontology_count(it->first);
        result += 1.0 / log(degree);
      }
    }

    return result;
  }

  /**
   * [1]	T. H. Haveliwala, “Topic-sensitive pagerank,” presented at the WWW, 2002, pp. 517–526.
   *
   * Personalized PageRank
   *
   * TODO: May change to long double for more precise score.
   */
  double personalize_pagerank(unsigned int src, unsigned int dst, double damping = 0.15, double delta = 0.000001,
                              int iter = 1000, bool is_directed = false) {

    is_node_valid(src);
    is_node_valid(dst);

    std::vector<double> ppr_score(nodes_ptr->getMax_id() + 1, 0.0);
    std::vector<double> old_score(nodes_ptr->getMax_id() + 1, 0.0);

    // init score
    ppr_score[src] = 0;
    old_score[src] = 1;

    int cnt = 0;
    while (cnt < iter) { // run at most *iter* iterations
      double changes = 0.0;

      for (int i = 0; i < nodes_ptr->getMax_id() + 1; i++) {
        // get score by adding up all neighbors
        const std::set<uint> &neighbors = is_directed ? edges_ptr->get_edges(i).get_out_neighbors() :
                                          edges_ptr->get_edges(i).get_neighbors();

        double tmp_score = i == src ? damping : 0.0;

        for (auto it = neighbors.cbegin(); it != neighbors.cend(); it++) {
          if ((*it == dst && i == src) || (*it == src && i == dst)) { // skip direct connected edge
            continue;
          }
          //TODO: The degree of src and dst is actually deg - 1 if src and dst are directly connected.
          size_t deg = is_directed ? edges_ptr->get_edges(*it).get_out_deg() : edges_ptr->get_edges(*it).get_deg();
          tmp_score += (1 - damping) * old_score[*it] / deg;
        }
        changes += std::abs(old_score[i] - tmp_score); // changes between old score and new score
        old_score[i] = ppr_score[i];
        ppr_score[i] = tmp_score;
      }

      std::cout << "iteration " << cnt << " complete delta " << changes << "\n";

      if (changes <= delta) break;
      cnt++;
    }

    return ppr_score[dst];

  }

  /**
   * [1]	A. L. Barabasi, H. Jeong, Z. Neda, E. Ravasz, A. Schubert, and T. Vicsek,
   * “Evolution of the social network of scientific collaborations,” Physical review E, vol. cond-mat.soft. 09-Apr-2001.
   *
   */
  double preferential_attachment(unsigned int id1, unsigned int id2) {
    is_node_valid(id1);
    is_node_valid(id2);
    return edges_ptr->get_edges(id1).get_deg() * edges_ptr->get_edges(id2).get_deg();
  }

  /**
   * [1]	L. Katz, “A new status index derived from sociometric analysis,”
   * Psychometrika, vol. 18, no. 1, pp. 39–43, 1953.
   *
   * Default beta is based on
   * [2]	D. Liben-Nowell and J. M. Kleinberg, “The link prediction problem for social networks.,”
   * CIKM, pp. 556–559, 2003.
   */
  double katz(unsigned int id1, unsigned int id2, unsigned int max_length = 5, double beta = 0.05) {
    is_node_valid(id1);
    is_node_valid(id2);
    double score = 0;
    for (int i = 3; i <=
                    max_length; i++) { // we do not start with 1 or 2 because 1 does not have any paths and 2 means directly connected edges
      std::vector<std::vector<unsigned int> > paths = homogeneous_dfs(id1, id2, max_length, false, true);
      double tmp_score = double(paths.size());
      for (int j = 0; j < i; j++) {
        tmp_score *= beta;
      }
      score += tmp_score;
    }
    return score;
  }

  inline std::vector<unsigned int> get_ontology(unsigned int id) {
    is_node_valid(id);
    return edges_ptr->get_ontology(id);
  }

  inline std::vector<std::pair<unsigned int, std::set<unsigned int> > > get_ontology_siblings(unsigned int id) {
    is_node_valid(id);
    return edges_ptr->get_ontology_siblings(id);
  }

  inline std::vector<std::pair<unsigned int, unsigned int> > get_ontology_sibling_count(unsigned int id) {
    is_node_valid(id);
    return edges_ptr->get_ontology_sibling_count(id);
  }

};

#endif //GBPEDIA_GRAPH_H