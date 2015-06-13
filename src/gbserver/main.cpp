//
// Created by Baoxu Shi on 6/12/15.
//

#include "opts.h"
#include "node_loader.h"
#include "edge_loader.h"

#include <iostream>


int main(int argc, const char *argv[]) {

  opts options;
  options.parse(argc, argv);

  node_loader<std::string> nodes(options.getNode_filepath());

  std::cout << "Load " << nodes.getMax_id() << " nodes\n";

  edge_loader edges(options.getEdge_filepath(), options.getIs_directed());

  std::cout << "Load " << edges.getNedges() << " edges with " << edges.getMax_rel() <<" relations\n";

  if (nodes.getMax_id() != edges.getMax_id()) {
    std::cerr << "Warning: #Node does not match in nodelist (" << nodes.getMax_id() << ")"
              << " and edgelist (" << edges.getMax_id() <<")";
  }

  return 0;
}
