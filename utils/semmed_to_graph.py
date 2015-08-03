#!/usr/bin/env python3

import sys

if len(sys.argv) != 6:
    print("usage: input_file node_file edge_file edgetype_file unique[T|F]")
    exit(233)

input_file = sys.argv[1]
node_file = sys.argv[2]
edge_file = sys.argv[3]
edgetype_file = sys.argv[4]
is_unique = sys.argv[5]

node_map = {}
node_cnt = 1

edge_list = None
if is_unique == 'T':
    edge_list = set()
else:
    edge_list = []

edgetype_map = {}
edgetype_cnt = 1

with open(input_file) as f:
    for line in f:
        sub,pred,obj = line.rstrip().split("\t")

        if sub not in node_map:
            node_map[sub] = node_cnt
            node_cnt += 1
        sub_id = node_map[sub]

        if pred not in edgetype_map:
            edgetype_map[pred] = edgetype_cnt
            edgetype_cnt += 1
        pred_id = edgetype_map[pred]

        if obj not in node_map:
            node_map[obj] = node_cnt
            node_cnt += 1
        obj_id = node_map[obj]

        if is_unique == 'T':
            edge_list.add(",".join([str(x) for x in [sub_id,obj_id,pred_id]]))
        else:
            edge_list.append(",".join([str(x) for x in [sub_id,obj_id,pred_id]]))


with open(node_file, "w+") as fout:
    for key in node_map:
        fout.write("\t".join([str(x) for x in [node_map[key], key]]))
        fout.write("\n")

with open(edge_file, "w+") as fout:
    for elem in edge_list:
        fout.write(elem)
        fout.write("\n")

with open(edgetype_file, "w+") as fout:
    for key in edgetype_map:
        fout.write("\t".join([str(x) for x in [edgetype_map[key], key]]))
        fout.write("\n")
