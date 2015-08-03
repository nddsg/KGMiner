#!/usr/bin/env python3

import sys
import subprocess

if len(sys.argv) < 7:
    print("Usage: node_file rel_file edge_file input_file rel_type output")
    exit(233)

node_file = sys.argv[1]
rel_file = sys.argv[2]
edge_file = sys.argv[3]
input_file = sys.argv[4]
rel_type = sys.argv[5]
output_file = sys.argv[6]

node_map = dict()
rel_map = dict()
edge_map = dict()

with open(node_file) as f:
    for line in f:
        if line.startswith('#'):
            continue
        nid, title = line.rstrip().split('\t')
        node_map[nid] = title

with open(rel_file) as f:
    for line in f:
        if line.startswith('#'):
            continue
        rid, title = line.rstrip().split('\t')
        rel_map[title] = rid

target_rel = rel_map[rel_type]

with open(input_file) as f:
    for line in f:
        p = subprocess.Popen(['socat','-t 3600','-','UNIX-CONNECT:/tmp/gbserver'],stdout=subprocess.PIPE,stderr=subprocess.PIPE,stdin=subprocess.PIPE)
        out, err = p.communicate(" ".join(["neighborwithrel",line.rstrip(),target_rel]))
        print(out)
