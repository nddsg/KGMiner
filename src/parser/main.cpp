#include "opts.h"
#include "rdf_parser.h"

#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

using namespace std;

namespace fs = boost::filesystem;

void write_map(boost::unordered_map<std::string, unsigned int> &map, std::string file_path, std::string sep = ",") {
  std::fstream fs(file_path, std::fstream::out);
  for (boost::unordered_map<std::string, unsigned int>::const_iterator it = map.cbegin();
       it != map.cend(); it++) {
    fs << it->second << sep << it->first << std::endl;
  }
  fs.close();
}

void write_vector(std::vector<std::string> &vec, std::string file_path) {
  std::fstream fs(file_path, std::fstream::out);
  for (std::vector<std::string>::const_iterator it = vec.cbegin();
       it != vec.cend(); it++) {
    fs << *it << std::endl;
  }
}

int main(int argc, const char* argv[]) {

  opts options;

  if (!options.parse(argc, argv)) {
    return -1;
  }

  unsigned int resource_id = 1, ontology_id = 1;
  boost::unordered_map<std::string, unsigned int> resource_map = boost::unordered_map<std::string, unsigned int>(); // contains all elements under dbpedia.org/resource/
  boost::unordered_map<std::string, unsigned int> ontology_map = boost::unordered_map<std::string, unsigned int>(); // all others
  std::vector<std::string> output_edges;

  try{
    for(size_t i = 0; i < options.getInput_files().size(); i++) {
      const std::string &path = options.getInput_files().at(i);
      if (fs::is_regular_file(path)) { // read if path is a regular file
        rdf_parser rdfParser(path);
        rdfParser.read_triplets(resource_map, ontology_map, resource_id, ontology_id, output_edges);
        std::cout << path << " processed.\n";
      } else if (fs::is_directory(path)) { // iterate through the folder and read every file in it
        for (fs::directory_iterator it(path); it != fs::directory_iterator(); it++) {
          if (fs::is_regular_file(it->status())) {
            rdf_parser rdfParser(it->path().string());
            rdfParser.read_triplets(resource_map, ontology_map, resource_id, ontology_id, output_edges);
            std::cout << it->path().string() << " processed.\n";
          } else {
            std::cout << "skip " << it->path().string() << " because it is not a valid file\n";
          }
        }
      }

    }

  }catch (std::invalid_argument error) {
    std::cout << error.what();
    return -1;
  } catch (std::exception &error) {
    std::cout << error.what();
    return -1;
  }

  std::cout << "writing " << options.getOutput_prefix() << ".nodes ...\n";
  write_map(resource_map, options.getOutput_prefix() + ".nodes", "\t");

  std::cout << "writing " << options.getOutput_prefix() << ".edgetypes ...\n";
  write_map(ontology_map, options.getOutput_prefix() + ".edgetypes", "\t");

  // Deduplicate edges
  std::cout << "dedpulicating edges, this may take a while ...\n";
  sort(output_edges.begin(), output_edges.end());
  output_edges.erase(unique(output_edges.begin(), output_edges.end()), output_edges.end());

  std::cout << "writing " << options.getOutput_prefix() << ".edgelist ...\n";
  write_vector(output_edges, options.getOutput_prefix() + ".edgelist");

  std::cout << "Job done!\nStatistics:\n"
  << "\t#Nodes: " << resource_id << std::endl
  << "\t#EdgeTypes: " << ontology_id << std::endl
  << "\t#Edges: " << output_edges.size() << std::endl;

  return 0;
}