//
// Created by Baoxu Shi on 6/11/15.
//

#include "rdf_parser.h"

#include <iostream>
#include <boost/algorithm/string/predicate.hpp>

#define PREFIX_MAGIC 28 // The string prefix length of entity and ontology

bool rdf_parser::is_resource(std::string &str) {
  return boost::istarts_with(str, "http://dbpedia.org/resource/");
}

bool rdf_parser::is_ontology(std::string &str) {
  return boost::istarts_with(str, "http://dbpedia.org/ontology/");
}

bool rdf_parser::is_isa(std::string &str) {
  return boost::istarts_with(str, "http://www.w3.org/1999/02/22-rdf-syntax-ns#type");
}

rdf_parser::rdf_parser(std::string file_path)
    : rdf_reader(file_path, "text/turtle", "UTF-8", "http://dbpedia.org/") {}


unsigned int rdf_parser::get_mapped_id(boost::unordered_map<std::string, unsigned int> &resource_map,
                                       boost::unordered_map<std::string, unsigned int> &ontology_map,
                                       unsigned int &mapped_id, std::string str, bool is_predicate) {

  unsigned int id = 0;

  if (is_predicate) { // Predicate, we only keep DBpedia ontology and is-a relation
    if (is_resource(str)) {
      throw std::domain_error("A predicate can not be a resource type. The URI of predicate is " + str);
    }
    if (is_ontology(str)) {
      str = str.substr(PREFIX_MAGIC);
      if (ontology_map.find(str) == ontology_map.end()) {
        ontology_map[str] = mapped_id;
        id = mapped_id;
        mapped_id++;
      } else {
        id = ontology_map.at(str);
      }
    } else if (is_isa(str)) {
      if (ontology_map.find(str) == ontology_map.end()) {
        ontology_map[str] = mapped_id;
        id = mapped_id;
        mapped_id++;
      } else {
        id = ontology_map.at(str);
      }
    } else { // we ignore other type of edges
      throw std::invalid_argument("Not a valid predicate.");
    }
  } else { // Object or subject. Convert entity and ontology into entity.
    if (is_resource(str) || is_ontology(str)) {
      str = str.substr(PREFIX_MAGIC);
      if (resource_map.find(str) == resource_map.end()) {
        resource_map[str] = mapped_id;
        id = mapped_id;
        mapped_id++;
      } else {
        id = resource_map.at(str);
      }
    } else {
      throw std::invalid_argument("A predicate can not be a resource type. The URI of predicate is " + str);
    }
  }

  return id;
}

void rdf_parser::read_triplets(boost::unordered_map<std::string, unsigned int> &resource_map,
                               boost::unordered_map<std::string, unsigned int> &ontology_map,
                               unsigned int &resource_id, unsigned int &ontology_id,
                               std::vector<std::string> &output_edges) {

  rdf_reader.read_triples([&](std::unique_ptr<rdf::triple> triplet) {

    std::stringstream ss;

    try {
      unsigned int src = get_mapped_id(resource_map, ontology_map, resource_id, triplet->subject->string, false),
          dst = get_mapped_id(resource_map, ontology_map, resource_id, triplet->object->string, false),
          rel = get_mapped_id(resource_map, ontology_map, ontology_id, triplet->predicate->string, true);

      if (src != 0 && dst != 0 && rel != 0)

        ss << src << "," << dst << "," << rel;

      std::cout << ss.str();
      output_edges.push_back(ss.str());
    } catch (std::invalid_argument &err) { }
    catch (std::domain_error &err) {
      std::cout << err.what();
    }

  });
}
