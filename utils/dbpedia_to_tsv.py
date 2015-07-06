#!/usr/bin/env python3

import sys

if len(sys.argv) <= 2:
    print("usage: ./dbpedia_to_tsv.py input1 [input2 input3 ...] output")
    exit(233)

outputfile = sys.argv[-1]

fout = open(outputfile, "w+")

for inputfile in sys.argv[1:-1]:
    with open(inputfile) as f:

        for line in f:
            if line.startswith("#"):
                continue

            subj, pred, obj = line.split(" ")[:3]

            if subj.startswith("<http://dbpedia.org/resource/") or subj.startswith("<http://dbpedia.org/ontology"):
                subj = subj[29:-1]
            else:
                continue

            if obj.startswith("<http://dbpedia.org/resource/") or obj.startswith("<http://dbpedia.org/ontology"):
                obj = obj[29:-1]
            else:
                continue

            if pred.startswith("<http://dbpedia.org/ontology"):
                pred = pred[29:-1]
            elif pred != "<http://www.w3.org/1999/02/22-rdf-syntax-ns#type>":
                continue

            fout.write("\t".join([subj,pred,obj])+"\n")