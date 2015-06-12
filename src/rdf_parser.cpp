//
// Created by Baoxu Shi on 6/11/15.
//

#include "rdf_parser.h"

#include <iostream>
#include <boost/algorithm/string/predicate.hpp>

bool rdf_parser::is_resource(std::string &str) {
  return boost::istarts_with(str, "http://dbpedia.org/resource");
}

rdf_parser::rdf_parser(std::string file_path)
    : rdf_reader(file_path, "text/turtle", "UTF-8", "http://dbpedia.org/") {}

void rdf_parser::read_triplets(boost::unordered_map<std::string, unsigned int> &resource_map,
                               boost::unordered_map<std::string, unsigned int> &ontology_map, unsigned int &mapped_id,
                               std::vector<std::string> output_edges) {

  rdf_reader.read_triples([&resource_map, &ontology_map, &mapped_id, &output_edges](std::unique_ptr<rdf::triple> triplet){

    std::function<unsigned int(std::string&)> get_id = [&resource_map, &ontology_map, &mapped_id](std::string& str) {
      unsigned int id;
      if (is_resource(str)){
        if (resource_map.find(str) == resource_map.end()) {
          resource_map[str] = mapped_id;
          id = mapped_id;
          mapped_id++;
        } else {
          id = resource_map.at(str);
        }
      } else {
        if (ontology_map.find(str) == ontology_map.end()) {
          ontology_map[str] = mapped_id;
          id = mapped_id;
          mapped_id++;
        } else {
          id = ontology_map.at(str);
        }
      }
      return id;
    };

    std::stringstream ss;

    unsigned int src = get_id(triplet->subject->string),
        dst = get_id(triplet->object->string),
        rel = get_id(triplet->predicate->string);

    ss << src << "," << dst << "," << rel << "\n";

    std::cout << ss.str();
    output_edges.push_back(ss.str());

  });
}
