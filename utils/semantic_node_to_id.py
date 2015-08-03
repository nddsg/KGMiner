#!/usr/bin/env python3

import sys
import html

if len(sys.argv) < 4:
    print("Usage: node_file input_file output_file")
    exit(233)

node_file = sys.argv[1]
input_file = sys.argv[2]
output_file = sys.argv[3]
separator = '\t'

node_map = dict()
with open(node_file) as f:
    for line in f:
        if line.startswith('#'):
            continue
        nid, title = line.split('\t')
        node_map[title.rstrip()] = nid

fout = open(output_file, "w+")
with open(input_file) as f:
    for line in f:
        if line.startswith('#'):
            continue
        titles = html.unescape(line.rstrip()).split(separator)
        try:
            fout.write(separator.join([node_map[x] for x in titles])+"\n")
        except KeyError:
            print("key error, "+" ".join(titles))

fout.close()
