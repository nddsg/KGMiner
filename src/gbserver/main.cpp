//
// Created by Baoxu Shi on 6/12/15.
//

#include "opts.h"
#include "node_loader.h"

#include <iostream>


int main(int argc, const char *argv[]) {

  opts options;
  options.parse(argc, argv);

  node_loader<std::string> nodes(options.getNode_filepath());

  std::cout << "Load " << nodes.getMax_id() << " nodes\n";

  return 0;
}
