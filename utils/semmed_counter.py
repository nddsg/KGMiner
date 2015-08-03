#!/usr/bin/env python3

import sys

input_file = sys.argv[1]
output_file = sys.argv[2]

records = dict()

with open(input_file) as f:
    for line in f:
        key = line.rstrip()
        if key not in records:
            records[key] = 1
        else:
            records[key] += 1

with open(output_file, "w+") as fout:
    for key in records:
        fout.write(key)
        fout.write("\t")
        fout.write(str(records[key]))
        fout.write("\n")
