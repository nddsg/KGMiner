#!/usr/bin/env python3

import sys

if len(sys.argv) != 4:
    print("usage: edge_file edgetype_file output_file")
    exit(233)

edge_file = sys.argv[1]
edgetype_file = sys.argv[2]
output_file = sys.argv[3]

edgetypes = {}

with open(edgetype_file) as f:
    for line in f:
        type_id,name = line.rstrip().split("\t")
        edgetypes[type_id] = name

fout = open(output_file, "w+")

with open(edge_file) as f:
    for line in f:
        src,dst,rel = line.rstrip().split(",")
        fout.write("\t".join([src,edgetypes[rel],dst])+"\n")
