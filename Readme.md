Code for paper 

Discriminative predicate path mining for fact checking in knowledge graphs, B Shi, T Weninger, Knowledge-Based Systems 104, 123-133, 2016

https://arxiv.org/pdf/1510.05911.pdf

```bash
git clone https://github.com/nddsg/KGMiner.git
cd KGMiner/.
./compile.sh
```

When it’s done, run
```bash
./run_server.sh
```
to start a server instance

when you see the server is started (you should see the following output)

```
Load 4743012 nodes
Load 671 rels
Load 27313477 edges with 671 relations
4218123 out of 4743012 have valid entity label
3313257 out of 4218123 have more than one entity label
Graph constructed
50 threads are created
```

Then open a new command line, run
```bash
./run_test.sh
```

You will see some outputs that like
```
[1] "PREDICATE PATH"
=== 10 Fold Cross Validation ===

=== Summary ===

Correctly Classified Instances         299               99.6667 %
Incorrectly Classified Instances         1                0.3333 %
Kappa statistic                          0.9879
K&B Relative Info Score              28968.5641 %
K&B Information Score                  189.9418 bits      0.6331 bits/instance
Class complexity | order 0             195.016  bits      0.6501 bits/instance
Class complexity | scheme               21.7656 bits      0.0726 bits/instance
Complexity improvement     (Sf)        173.2504 bits      0.5775 bits/instance
Mean absolute error                      0.0081
Root mean squared error                  0.0635
Relative absolute error                  2.9071 %
Root relative squared error             17.0297 %
Total Number of Instances              300

=== Detailed Accuracy By Class ===

                TP Rate  FP Rate  Precision  Recall   F-Measure  MCC      ROC Area  PRC Area  Class
                1.000    0.020    0.996      1.000    0.998      0.988    0.999     1.000     FALSE
                0.980    0.000    1.000      0.980    0.990      0.988    0.999     0.997     TRUE
Weighted Avg.    0.997    0.017    0.997      0.997    0.997      0.988    0.999     0.999

=== Confusion Matrix ===

  a   b   <-- classified as
250   0 |   a = FALSE
  1  49 |   b = TRUE
```
