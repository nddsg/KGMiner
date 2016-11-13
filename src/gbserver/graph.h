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
#include <cstdlib>
#include <cmath>

template<typename ND, typename TD>
class graph
{

  typedef ND node_type;
  typedef TD edge_type;

  std::shared_ptr<node_loader<node_type> > nodes_ptr;
  std::shared_ptr<edge_loader> edges_ptr;
  std::shared_ptr<type_loader<edge_type> > edgetypes_ptr;

  inline void is_node_valid(unsigned int id)
  {
    if (!nodes_ptr->exists(id)) {
      throw std::runtime_error("Nodes " + std::to_string(id) + " does not exist.");
    }
  }

  inline bool is_loop(std::vector<std::pair<unsigned int, unsigned int> > &path, unsigned int id) noexcept {
    for (auto it = path.cbegin(); it != path.cend(); ++it)
    {
      if (it->first == id) return true;
    }
    return false;
  }


  /**
   * Homogeneous dfs helper
   */
  void dfs_helper(unsigned int src, unsigned int dst, unsigned int discard_rel, unsigned max_depth,
                  std::vector<unsigned int> &tmp_path, std::set<unsigned int> &visited,
                  std::vector<std::vector<unsigned int> > &result, bool is_directed, bool depth_only, unsigned depth)
  {
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

      if (it->second == discard_rel) continue; // discard certain rel_type

      if (visited.find(it->first) == visited.end() || (it->first == dst && !dst_visited)) { // never visited
        tmp_path.push_back(it->first);
        dfs_helper(it->first, dst, discard_rel, max_depth, tmp_path, visited, result, is_directed, depth_only,
                   depth + 1);
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

        if (it->second == discard_rel) continue; // discard certain rel_type

        if (visited.find(it->first) == visited.end() || (it->first == dst && !dst_visited)) {
          tmp_path.push_back(it->first);
          dfs_helper(it->first, dst, discard_rel, max_depth, tmp_path, visited, result, is_directed, depth_only,
                     depth + 1);
          tmp_path.pop_back();
          visited.insert(it->first);
        }
      }
    }

  }


  /**
   * Heterogeneous dfs helper
   */
  void dfs_helper(unsigned int src, unsigned int dst, unsigned int discard_rel, unsigned max_depth,
                  std::vector<std::pair<unsigned int, unsigned int> > &tmp_path, std::vector<bool> &reverted_rel,
                  std::set<unsigned int> &visited,
                  std::vector<std::vector<std::pair<unsigned int, unsigned int> > > &result,
                  std::vector<std::vector<bool> > &rel_result, bool is_directed, unsigned depth)
  {
    if (tmp_path.size() > 0 && tmp_path.size() <= max_depth && tmp_path.back().first == dst) {
      result.push_back(tmp_path);
      rel_result.push_back(reverted_rel);
      return;
    }

    if (max_depth == depth) return;

    edge_list &edges = edges_ptr->get_edges(src);
    for (auto it = edges.get_forward().cbegin();
         it != edges.get_forward().cend(); ++it) {
      if (it->second == discard_rel) continue;
      if (visited.find(it->first) == visited.end()) {
        tmp_path.push_back(*it);
        reverted_rel.push_back(false);
        visited.insert(it->first);
        dfs_helper(it->first, dst, discard_rel, max_depth, tmp_path, reverted_rel, visited, result, rel_result,
                   is_directed,
                   depth + 1);
        tmp_path.pop_back();
        reverted_rel.pop_back();
        visited.erase(it->first);
      }
    }

    if (!is_directed) {
      for (auto it = edges.get_backward().cbegin();
           it != edges.get_backward().cend(); ++it) {
        if (it->second == discard_rel) continue;
        if (visited.find(it->first) == visited.end()) {
          tmp_path.push_back(*it);
          reverted_rel.push_back(true);
          visited.insert(it->first);
          dfs_helper(it->first, dst, discard_rel, max_depth, tmp_path, reverted_rel, visited, result, rel_result,
                     is_directed,
                     depth + 1);
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

  const std::set<unsigned int> &get_out_edges(unsigned src)
  {
    is_node_valid(src);
    return edges_ptr->get_edges(src).get_out_neighbors();

  }

  const std::set<unsigned int> &get_in_edges(unsigned src)
  {
    is_node_valid(src);
    return edges_ptr->get_edges(src).get_in_neighbors();
  }

  std::vector<std::vector<unsigned int> > homogeneous_dfs(unsigned int src, unsigned int dst, unsigned int discard_rel,
      unsigned int depth, bool depth_only, bool is_directed)
  {
    is_node_valid(src);
    is_node_valid(dst);

    std::vector<std::vector<unsigned int> > result;

    try {
      std::vector<unsigned int> tmp_path;
      std::set<unsigned int> visited;
      tmp_path.push_back(src);
      visited.insert(src);
      dfs_helper(src, dst, discard_rel, depth, tmp_path, visited, result, is_directed, depth_only, 1u);
      assert(tmp_path.size() == 1); // only source node is in it.
    } catch (std::exception error) {
      std::cerr << "Error occurred when performing dfs, " << error.what() << std::endl;
    }

    return result;

  };

  std::pair<std::vector<std::vector<std::pair<unsigned int, unsigned int> > >, std::vector<std::vector<bool> > > heterogeneous_dfs(
    unsigned int src, unsigned int dst, unsigned int discard_rel, bool is_directed, unsigned int depth)
  {
    is_node_valid(src);
    is_node_valid(dst);

    std::vector<std::vector<std::pair<unsigned int, unsigned int> > > path_result;
    std::vector<std::vector<bool> > rel_result;

    try {
      std::vector<std::pair<unsigned int, unsigned int> > tmp_path;
      std::vector<bool> tmp_reverted;
      std::set<unsigned int> visited;
      visited.insert(src);
      dfs_helper(src, dst, discard_rel, depth, tmp_path, tmp_reverted, visited, path_result, rel_result, is_directed,
                 1u);
      assert(tmp_path.size() == 0);
    } catch (std::exception error) {
      std::cerr << "Error occurred when performing heterogeneous dfs, " << error.what() << std::endl;
    }

    assert(path_result.size() == rel_result.size());

    return std::pair<std::vector<std::vector<std::pair<unsigned int, unsigned int> > >, std::vector<std::vector<bool> > >(
             path_result, rel_result);
  }

  node_type get_node_type(unsigned int id)
  {
    return nodes_ptr->get_value(id);
  }

  edge_type get_edge_type(unsigned int id)
  {
    return edgetypes_ptr->get_value(id);
  }

  /**
   * [1]	L. A. Adamic and E. Adar,
   *      “Friends and neighbors on the Web,” Social Networks, vol. 25, no. 3, pp. 211–230, Jul. 2003.
   */
  double adamic_adar(unsigned int id1, unsigned id2, unsigned int discard_rel = 0)
  {
    is_node_valid(id1);
    is_node_valid(id2);

    std::vector<unsigned int> common_neighbors = edges_ptr->get_common_neighbor(id1, id2, discard_rel, false);

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
  double semantic_proximity(unsigned int src, unsigned dst, unsigned int discard_rel = 0)
  {
    std::vector<std::vector<unsigned int> > paths = homogeneous_dfs(src, dst, discard_rel, 3, false, false);
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
  double multidimensional_adamic_adar(unsigned int id1, unsigned int id2, unsigned int rel_type)
  {
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
  double heter_adamic_adar(unsigned int id1, unsigned int id2, unsigned int rel_type)
  {
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
   * [1]	N. Lao and W. W. Cohen, “Relational retrieval using a combination of path-constrained random walks.,”
   *      ML, vol. 81, no. 1, pp. 53–67, 2010.
   *
   *      In the original paper they did not mention how to generate paths,
   *      due to the fact that 600+ rel_types exist in DBpedia, we can not afford generating all combinations of paths,
   *      which have 600^l different paths. Hence we generate paths based on starting point
   */
  double path_constrained_random_walk(unsigned int src, unsigned int dst, std::vector<unsigned int> metapath,
                                      bool is_directed = true)
  {

    is_node_valid(src);
    is_node_valid(dst);

    if (metapath.size() == 0) return 0.0;

    std::vector<bool> is_visited(nodes_ptr->getMax_id() + 1, false);
    std::vector<double> score(nodes_ptr->getMax_id() + 1, 0.0);

    std::vector<unsigned int> rel_count(nodes_ptr->getMax_id() + 1, 0);
    // for path length = 0, set initial scores for each reachable node

    score[src] = 1.0;

    // for path length >= 1, calculate scores.

    for (auto it = metapath.cbegin(); it != metapath.cend(); ++it) {
      for (unsigned int i = 0; i <= nodes_ptr->getMax_id(); i++) {
        rel_count[i] = edges_ptr->get_neighbor_count_by_rel(i, *it, is_directed);
      }
      // calculate scores
      for (unsigned int i = 0; i <= nodes_ptr->getMax_id(); i++) {
        auto back_edges = edges_ptr->get_edges(i).get_backward();
        for (auto prev = back_edges.cbegin(); prev != back_edges.cend(); ++prev) {
          if (prev->second == *it) {
            score[i] +=
              score[prev->first] / double(rel_count[prev->first]);
          }
        }
      }
    }

    return score[dst];

  }

  /**
   * [1]	T. H. Haveliwala, “Topic-sensitive pagerank,” presented at the WWW, 2002, pp. 517–526.
   *
   * Personalized PageRank
   *
   * TODO: May change to long double for more precise score.
   */
  double personalized_pagerank(unsigned int src, unsigned int dst, unsigned int discard_rel, double delta, int iter,
                               bool is_directed, double damping)
  {

    is_node_valid(src);
    is_node_valid(dst);

    std::cout << src <<", and " << dst << " are valid nodes\n";

    std::vector<double> ppr_score(nodes_ptr->getMax_id() + 1, 0.0);
    std::vector<double> old_score(nodes_ptr->getMax_id() + 1, 0.0);

    // init score
    ppr_score[src] = 0;
    old_score[src] = 1;

    int cnt = 0;
    while (cnt < iter) { // run at most *iter* iterations
      double changes = 0.0;

      for (int i = 0; i <= nodes_ptr->getMax_id(); i++) {
        // get score by adding up all neighbors

        std::set<uint> neighbors;

        const std::set<std::pair<uint, uint> > &in_neighbors = edges_ptr->get_edges(
              i).get_backward(); // all in-coming edges


        for (auto it = in_neighbors.cbegin(); it != in_neighbors.cend(); ++it) {
          if (it->second != discard_rel) neighbors.insert(it->first);
        }

        if (!is_directed) {
          const std::set<std::pair<uint, uint> > &out_neighbors = edges_ptr->get_edges(
                i).get_forward(); // all out-going edges

          for (auto it = out_neighbors.cbegin(); it != out_neighbors.cend(); ++it) {
            if (it->second != discard_rel) neighbors.insert(it->first);
          }
        }

        double tmp_score = i == src ? damping : 0.0;

        for (auto it = neighbors.cbegin(); it != neighbors.cend(); it++) {
          //TODO: The degree of src and dst is actually deg - 1 if src and dst are directly connected.
          size_t deg = is_directed ? edges_ptr->get_edges(*it).get_out_deg() : edges_ptr->get_edges(*it).get_deg();
          if (deg != 0)
            tmp_score += (1 - damping) * old_score[*it] / deg;

          changes += std::abs(old_score[i] - tmp_score); // changes between old score and new score
          old_score[i] = ppr_score[i];
          ppr_score[i] = tmp_score;
        }
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
  double preferential_attachment(unsigned int id1, unsigned int id2)
  {
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
  double katz(unsigned int id1, unsigned int id2, unsigned int discard_rel, unsigned int max_length = 3,
              double beta = 0.05)
  {
    is_node_valid(id1);
    is_node_valid(id2);
    double score = 0;
    for (int i = 3; i <=
         max_length; i++) { // we do not start with 1 or 2 because 1 does not have any paths and 2 means directly connected edges
      std::vector<std::vector<unsigned int> > paths = homogeneous_dfs(id1, id2, discard_rel, max_length, true, false);
      double tmp_score = double(paths.size());
      for (int j = 0; j < i; j++) {
        tmp_score *= beta;
      }
      score += tmp_score;
    }
    return score;
  }

  inline std::vector<unsigned int> get_ontology(unsigned int id)
  {
    is_node_valid(id);
    return edges_ptr->get_ontology(id);
  }

  inline std::vector<std::pair<unsigned int, std::set<unsigned int> > > get_ontology_siblings(unsigned int id)
  {
    is_node_valid(id);
    return edges_ptr->get_ontology_siblings(id);
  }

  inline std::set<unsigned int> get_ontology_siblings(unsigned int id, double tol)
  {
    is_node_valid(id);
    return edges_ptr->get_ontology_siblings(id, tol);
  }

  inline std::vector<std::pair<unsigned int, unsigned int> > get_ontology_sibling_count(unsigned int id)
  {
    is_node_valid(id);
    return edges_ptr->get_ontology_sibling_count(id);
  }

  bool connected_by_helper(unsigned int src, unsigned int dst, unsigned int pos, std::vector<unsigned int> &link_type,
                           bool is_directed)
  {
    edge_list &edges = edges_ptr->get_edges(src);

    for (auto it = edges.get_forward().cbegin(); it != edges.get_forward().cend(); ++it) {
      if (it->second == link_type[pos]) { // match link type
        if (pos == link_type.size() - 1) { // reach the end
          if (it->first == dst) {
            return true;
          }
        } else { // not reach the end
          if (connected_by_helper(it->first, dst, pos + 1, link_type, is_directed)) { // if find it
            return true;
          }
        }
      }
    }

    if (!is_directed) {
      for (auto it = edges.get_backward().cbegin(); it != edges.get_backward().cend(); ++it) {
        if (it->second == link_type[pos]) { // match link type
          if (pos == link_type.size() - 1) { // reach the end
            if (it->first == dst) {
              return true;
            }
          } else { // not reach the end
            if (connected_by_helper(it->first, dst, pos + 1, link_type, is_directed)) { // if find it
              return true;
            }
          }
        }
      }
    }

    return false;
  }

  bool connected_by(unsigned int src, unsigned int dst, std::vector<unsigned int> link_type, bool is_directed = false)
  {
    is_node_valid(src);
    is_node_valid(dst);

    return (connected_by_helper(src, dst, 0, link_type, is_directed));

  }

  bool connected_by(unsigned int src, unsigned int dst, unsigned int link_type, bool is_directed = false)
  {
    is_node_valid(src);
    is_node_valid(dst);

    edge_list &edges = edges_ptr->get_edges(src);

    for (auto it = edges.get_forward().cbegin(); it != edges.get_forward().cend(); ++it) {
      if (it->first == dst && it->second == link_type) return true;
    }

    if (!is_directed) {
      for (auto it = edges.get_backward().cbegin(); it != edges.get_backward().cend(); ++it) {
        if (it->first == src && it->second == link_type) return true;
      }
    }

    return false;
  }

  //TODO: This function iterates through entire edge list to find qualified entity pairs, try cache this
  std::vector<std::pair<unsigned int, unsigned int> > get_entity_pairs_by_rel(unsigned int rel_type,
      double sample_rate = 0.1)
  {

    srand(233);

    std::vector<std::pair<unsigned int, unsigned int> > res;
    for (unsigned int i = nodes_ptr->getMin_id(); i <= nodes_ptr->getMax_id(); i++) {
      edge_list &edges = edges_ptr->get_edges(i);
      for (auto it = edges.get_forward().cbegin(); it != edges.get_forward().cend(); ++it) {
        if (it->second == rel_type && ((double) rand() / RAND_MAX <= sample_rate)) {
          res.push_back(std::pair<unsigned int, unsigned int>(i, it->first));
        }
      }
    }

    return res;
  }

  inline std::set<std::pair<unsigned int, unsigned int> > get_entity_pairs_by_triple(unsigned int src, unsigned int dst,
      unsigned int rel_type,
      double sample_rate = 0.1)
  {
    return get_entity_pairs_by_triple_helper(src, dst, rel_type, sample_rate, false);
  }

  inline std::set<std::pair<unsigned int, unsigned int> > get_entity_pairs_without_rel(unsigned int src,
      unsigned int dst,
      unsigned int rel_type,
      double sample_rate = 0.1)
  {
    return get_entity_pairs_by_triple_helper(src, dst, rel_type, sample_rate, true);
  }

  std::set<std::pair<unsigned int, unsigned int> > get_entity_pairs_by_triple_helper(unsigned int src, unsigned int dst,
      unsigned int rel_type,
      double sample_rate = 0.1,
      bool exclude_rel = false)
  {
    is_node_valid(src);
    is_node_valid(dst);
    srand(233);

    // Step 1: Get src_set and dst_set that matches src and dst's ontology

    std::set<unsigned int> src_set = get_ontology_siblings(src, 0.0);
    std::set<unsigned int> dst_set = get_ontology_siblings(dst, 0.0);

    // Step 2: Get true labeled node pairs from previous sets

    std::set<std::pair<unsigned int, unsigned int> > entity_pairs;
    std::set<std::pair<unsigned int, unsigned int> > entity_pairs_with_rel;

    for (auto it = src_set.cbegin(); it != src_set.cend(); ++it) {
      auto edges = edges_ptr->get_edges(*it).get_forward();
      for (auto p = edges.cbegin(); p != edges.cend(); ++p) {
        if (!exclude_rel) { // get all entity pairs with rel_type
          if (p->second == rel_type && dst_set.find(p->first) != dst_set.end()) { // rel match and dst is in the set
            entity_pairs_with_rel.insert(std::pair<unsigned int, unsigned int>(*it, p->first));
          }
        } else { // get all entity pairs without rel_type
          if (p->second != rel_type && dst_set.find(p->first) != dst_set.end()) {
            entity_pairs.insert(std::pair<unsigned int, unsigned int>(*it, p->first));
          }
        }
      }
    }

    for (auto it = dst_set.cbegin(); it != dst_set.cend(); ++it) {
      auto edges = edges_ptr->get_edges(*it).get_backward();
      for (auto p = edges.cbegin(); p != edges.cend(); ++p) {
        if (!exclude_rel) {
          if (p->second == rel_type && src_set.find(p->first) != src_set.end()) { // rel match and src is in the set
            entity_pairs_with_rel.insert(std::pair<unsigned int, unsigned int>(p->first, *it));
          }
        } else { // get all entity pairs that matching the given ontology but no rel_type
          if (p->second != rel_type && src_set.find(p->first) != src_set.end()) {
            entity_pairs.insert(std::pair<unsigned int, unsigned int>(p->first, *it));
          }
        }

      }
    }

    if (!exclude_rel) return entity_pairs_with_rel;

    for (auto it = entity_pairs_with_rel.cbegin(); it != entity_pairs_with_rel.cend(); ++it) {
      auto p = entity_pairs.find(*it);
      if (p != entity_pairs.end()) {
        entity_pairs.erase(p);
      }
    }

    return entity_pairs;

  }

  inline unsigned int get_edge_type_count(unsigned int rel_type)
  {
    return edges_ptr->get_edge_type_count(rel_type);
  }

  std::set<unsigned int> get_neighbor_by_rel(unsigned int src, unsigned int rel_type, bool is_directed = false)
  {
    is_node_valid(src);

    edge_list &edges = edges_ptr->get_edges(src);

    std::set<unsigned int> res;

    for (auto it = edges.get_forward().cbegin(); it != edges.get_forward().cend(); ++it) {
      if (it->second == rel_type) {
        res.insert(it->first);
      }
    }

    if (!is_directed) {
      for (auto it = edges.get_backward().cbegin(); it != edges.get_backward().cend(); ++it) {
        if (it->second == rel_type) {
          res.insert(it->first);
        }
      }
    }

    return res;

  }

  unsigned int get_nontology()
  {
    return edges_ptr->get_nontology();
  }

};

#endif //GBPEDIA_GRAPH_H

