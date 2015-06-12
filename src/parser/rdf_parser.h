//
// Created by Baoxu Shi on 6/11/15.
//

#ifndef GBPEDIA_RDF_PARSER_H
#define GBPEDIA_RDF_PARSER_H

#include <string>
#include <boost/unordered_map.hpp>

#include <rdf++.h>


class rdf_parser {

  rdf::reader rdf_reader;

  static inline bool is_resource(std::string &str);

  static inline bool is_ontology(std::string &str);

  static inline bool is_isa(std::string &str);

  unsigned int get_mapped_id(boost::unordered_map<std::string, unsigned int> &resource_map,
                             boost::unordered_map<std::string, unsigned int> &ontology_map,
                             unsigned int &mapped_id, std::string str, bool is_predicate = false);

public:

  rdf_parser(std::string file_path);

  void read_triplets(boost::unordered_map<std::string, unsigned int> &resource_map,
                     boost::unordered_map<std::string, unsigned int> &ontology_map,
                     unsigned int &resource_id, unsigned int &ontology_id,
                     std::vector<std::string> &output_edges);

};


#endif //GBPEDIA_RDF_PARSER_H
