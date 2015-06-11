#include "opts.h"

#include <iostream>
#include <boost/unordered_map.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <vector>
#include <utility>
#include <rdf++.h>

using namespace std;

int main(int argc, const char* argv[]) {

  opts options;

  if (!options.parse(argc, argv)) {
    return -1;
  }

  boost::unordered_map<std::string, unsigned int> resource_map = boost::unordered_map<std::string, unsigned int>(); // contains all elements under dbpedia.org/resource/
  boost::unordered_map<std::string, unsigned int> ontology_map = boost::unordered_map<std::string, unsigned int>(); // all others
  unsigned int mapped_id = 0;

  std::vector<std::string> output_edges;

  try{
    rdf::reader rdfReader(options.getInput_files().front(), "text/turtle", "UTF-8", "http://dbpedia.org/");
    rdfReader.read_triples([&resource_map, &ontology_map, &mapped_id, &output_edges](std::unique_ptr<rdf::triple> triplet){

      std::function<bool(std::string&)> is_resource = [](std::string& str) -> bool {
        return boost::istarts_with(str, "http://dbpedia.org/resource");
      };

      std::stringstream ss;

      unsigned int src, dst, rel;

      if (is_resource(triplet->subject->string)) {
        if (resource_map.find(triplet->subject->string) == resource_map.end()) {
          resource_map[triplet->subject->string] = mapped_id;
          src = mapped_id;
          mapped_id++;
        } else {
          src = resource_map.at(triplet->subject->string);
        }
      } else {
        if (ontology_map.find(triplet->subject->string) == ontology_map.end()) {
          ontology_map[triplet->subject->string] = mapped_id;
          src = mapped_id;
          mapped_id++;
        } else {
          src = ontology_map.at(triplet->subject->string);
        }
      }

      if (is_resource(triplet->object->string)) {
        if (resource_map.find(triplet->object->string) == resource_map.end()) {
          resource_map[triplet->object->string] = mapped_id;
          dst = mapped_id;
          mapped_id++;
        } else {
          dst = resource_map.at(triplet->object->string);
        }
      } else {
        if (ontology_map.find(triplet->object->string) == ontology_map.end()) {
          ontology_map[triplet->object->string] = mapped_id;
          dst = mapped_id;
          mapped_id++;
        } else {
          dst = ontology_map.at(triplet->object->string);
        }
      }

      if (is_resource(triplet->predicate->string)) {
        if (resource_map.find(triplet->predicate->string) == resource_map.end()) {
          resource_map[triplet->predicate->string] = mapped_id;
          rel = mapped_id;
          mapped_id++;
        } else {
          rel = resource_map.at(triplet->predicate->string);
        }
      } else {
        if (ontology_map.find(triplet->predicate->string) == ontology_map.end()) {
          ontology_map[triplet->predicate->string] = mapped_id;
          rel = mapped_id;
          mapped_id++;
        } else {
          rel = ontology_map.at(triplet->predicate->string);
        }
      }

      ss << src << "," << dst << "," << rel << "\n";

      output_edges.push_back(ss.str());

      std::cout << "subject " << src  << " object " << dst << " predicate " << rel << std::endl;
    });
  }catch (std::invalid_argument error) {
    std::cout << error.what();
  }

  return 0;
}