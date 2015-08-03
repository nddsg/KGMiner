#!/usr/bin/env python3

import sys
import csv

if len(sys.argv) != 5:
    print("usage: input_csv node_output edge_output edgetype_output")
    exit(233)

input_file = sys.argv[1]
output_node = sys.argv[2]
output_edge = sys.argv[3]
output_edgetype = sys.argv[4]

nodes = {}
nodes_cnt = 1

edges = set()

edgetypes = {}
type_cnt = 1

for edgetype in ["lobby","lobby_for","in","related_to","pay_for","payee"]:
    if edgetype not in edgetypes:
        edgetypes[edgetype] = type_cnt
        type_cnt += 1

with open(input_file) as f:
    dreader = csv.DictReader(f)
    for _line in dreader:
        line = _line
        for key in line:
            if line[key] == "Self":
                line[key] = line["Lobbyist"]
            # Log all unseen nodes
            if key not in ["Date","Amount","Payee"] and line[key] not in nodes and line[key] != "":
                # Skip time and numeric value
                nodes[line[key]] = nodes_cnt
                nodes_cnt += 1
            # Log all unseen payee's
            if key == "Payee" and line[key] not in edgetypes:
                edgetypes[line[key]] = type_cnt
                type_cnt += 1

        # Log all edge types (5 basic and one money usage)
        # `Lobbyist` lobby for `Lobbyist_Firm`
#        edges.add(",".join([str(x) for x in [nodes[line["Lobbyist"]],nodes[line["Lobbyist_Firm"]],edgetypes["lobby_for"]]]))
        # `Lobbyist` lobby `Official`
        edges.add(",".join([str(x) for x in [nodes[line["Lobbyist"]],nodes[line["Official"]],edgetypes[line["Payee"]]]]))
        # `Official` is in `Official Department`
        if line["Official_Department"] != "":
            edges.add(",".join([str(x) for x in [nodes[line["Official"]],nodes[line["Official_Department"]],edgetypes["in"]]]))
        # `SourceOfFunds` related to `Lobbyist_Firm`
        edges.add(",".join([str(x) for x in [nodes[line["SourceOfFunds"]],nodes[line["Lobbyist_Firm"]],edgetypes["related_to"]]]))
        # `SourceOfFunds` pay `Official` for `Payee`[edgetype]
        edges.add(",".join([str(x) for x in [nodes[line["SourceOfFunds"]],nodes[line["Lobbyist"]],edgetypes["pay_for"]]]))
        # `lobbyist_Firm` payee `Payee`
        edges.add(",".join([str(x) for x in [nodes[line["Lobbyist_Firm"]],nodes[line["Official"]],edgetypes["lobby"]]]))


# Write graph

with open(output_node, "w+") as fout:
    for key in nodes:
        fout.write("\t".join([str(nodes[key]), key]).replace(" ","_")+"\n")

with open(output_edgetype, "w+") as fout:
    for key in edgetypes:
        fout.write("\t".join([str(edgetypes[key]), key]).replace(" ","_")+"\n")

with open(output_edge, "w+") as fout:
    for elem in edges:
        fout.write(elem+"\n")
