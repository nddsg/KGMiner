#include "opts.h"
#include "rdf_parser.h"

#include <iostream>

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
    for(size_t i = 0; i < options.getInput_files().size(); i++) {
      std::cout << "resouce map size " << resource_map.size() << " ontology size " << ontology_map.size() << " mapped_id " << mapped_id << std::endl;
      rdf_parser rdfParser(options.getInput_files().at(i));
      rdfParser.read_triplets(resource_map, ontology_map, mapped_id, output_edges);
    }

  }catch (std::invalid_argument error) {
    std::cout << error.what();
  }

  return 0;
}