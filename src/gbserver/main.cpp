//
// Created by Baoxu Shi on 6/12/15.
//

#include "opts.h"
#include "node_loader.h"
#include "type_loader.h"
#include "edge_loader.h"
#include "socket_server.h"


int main(int argc, const char *argv[]) {

  opts options;
  if (!options.parse(argc, argv)) {
    return -1;
  }

  node_loader<std::string> nodes(options.getNode_filepath());

  std::cout << "Load " << nodes.getMax_id() << " nodes\n";

  type_loader<std::string> edgetypes(options.getType_filepath());

  std::cout << "Load " << edgetypes.getMax_rel() << " rels\n";

  edge_loader edges(options.getEdge_filepath(), options.getIs_directed());

  std::cout << "Load " << edges.getNedges() << " edges with " << edges.getMax_rel() <<" relations\n";

  std::cout << edges.get_nentity_with_ontology() << " out of " << nodes.getMax_id() <<
  " have valid ontology information\n";

  if (nodes.getMax_id() != edges.getMax_id()) {
    std::cerr << "Warning: #Node does not match in NodeList (" << nodes.getMax_id() << ")"
              << " and EdgeList (" << edges.getMax_id() <<")";
  }

  if (edgetypes.getMax_rel() != edges.getMax_rel()) {
    std::cerr << "Warning: #EdgeTypes does not match in TypeList (" << edgetypes.getMax_rel() << ")"
    << " and edgelist (" << edges.getMax_rel() <<")";
  }

  graph<std::string, std::string> g(nodes, edges, edgetypes);

  std::cout << "Graph constructed\n";

  socket_server server(options.getNworker());
  server.start("/tmp/gbserver", g);

  //TODO: Die gracefully.

  return 0;
}
