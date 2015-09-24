#!/usr/bin/env python3
import sys

# Generate commands for parallel SimRank computation

if len(sys.argv) != 5:
    print("usage: ./command_generator.py input_file input_graph command_file output_dir")
    exit(233)

input_file = sys.argv[1]
input_graph = sys.argv[2]
command_file = sys.argv[3]
output_dir = sys.argv[4]

def constructCommand(output_file, src, dst):
    return " ".join([str(x) for x in ["./simrank",input_graph,output_file, src, 100, 100000, 0.8, dst, 20]])

fout = open(command_file, "w+")

with open(input_file) as f:
    for line in f:
        if line.startswith("\"") or line.startswith("#"):
            continue
        dat = line.rstrip().split(",")
        fout.write(constructCommand(output_dir+"_".join(dat[0:2]), dat[0], dat[1])+"\n")