#include "opts.h"

#include <iostream>
#include <rdf++.h>

using namespace std;

int main(int argc, const char* argv[]) {

  opts options;

  if (!options.parse(argc, argv)) {
    return -1;
  }

//  try{
//    rdf::reader rdfReader("./data/mappingbased_properties_en.100.ttl", "text/turtle", "UTF-8", "http://dbpedia.org/");
//    rdfReader.read_triples([](std::unique_ptr<rdf::triple> triplet){
//      std::cout << "subject " << triplet->subject->string << " " << triplet->subject->size() << std::endl;
//      std::cout << "object " << triplet->object->string << " " << triplet->object->size() << std::endl;
//      std::cout << "predicate " << triplet->predicate->string << " " << triplet->predicate->size() << std::endl;
//    });
//  }catch (std::invalid_argument error) {
//    std::cout << error.what();
//  }

  return 0;
}