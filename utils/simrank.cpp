//
// Written by Lin Yang
// lyang5@nd.edu
//

#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<time.h>
#include<algorithm>
#include<iostream>
#include<math.h>
#include<string>
#include<string.h>
#include<time.h>

using namespace std;


vector<vector<int> > edge;
vector<vector<int> > invedge;
vector<vector<int> > foredge;
double c;
int times, numr;

double singlepair(int sid, int eid, int r) {
  int *u;
  int *v;
  int t, i, j, id, alpha, beta;
  double simrvalue = 0;


  u = (int *) malloc(r * sizeof(int));
  v = (int *) malloc(r * sizeof(int));
  for (i = 0; i < r; i++) {
    u[i] = sid;
    v[i] = eid;
  }


  for (t = 0; t < times; t++) {
    vector<int> inters(r);
    vector<int>::iterator it;
    sort(u, u + r);
    sort(v, v + r);
    vector<int> uu(u, u + r);
    vector<int> uv(v, v + r);
    /*  for (it=uu.begin();it!=uu.end();++it)
    		printf("%d ",*it);
    	printf("\n");
      for (it=uv.begin();it!=uv.end();++it)
    		printf("%d ",*it);
    	printf("\n");
    	printf("\n");*/

    it = unique(uu.begin(), uu.end());
    uu.resize(distance(uu.begin(), it));
    it = unique(uv.begin(), uv.end());
    uv.resize(distance(uv.begin(), it));
    /*for (it=uu.begin();it!=uu.end();++it)
    	printf("%d ",*it);
    printf("\n");
    for (it=uv.begin();it!=uv.end();++it)
    	printf("%d ",*it);
    printf("\n");
    printf("\n");*/


    it = set_intersection(uu.begin(), uu.end(), uv.begin(), uv.end(), inters.begin());

    inters.resize(it - inters.begin());

    //	printf("size:%ld\n",inters.size());
    if (inters.size() > 0) {
      for (it = inters.begin(); it != inters.end(); ++it) {
        id = *it;
        alpha = 0;
        beta = 0;
        for (i = 0; i < r; i++) {
          if (u[i] == id)
            alpha++;
          if (v[i] == id)
            beta++;
        }
        simrvalue = simrvalue + pow(c, t) * (1 - c) * double(alpha) * double(beta) / pow(r, 2);

      }
    }
    for (i = 0; i < r; i++) {
      if (invedge[u[i]].size() > 1)
        u[i] = invedge[u[i]][rand() % (invedge[u[i]].size() - 1) + 1];
      if (invedge[v[i]].size() > 1)
        v[i] = invedge[v[i]][rand() % (invedge[v[i]].size() - 1) + 1];
    }

//			printf("%f \n",simrvalue);
  }
  return simrvalue;
}

int main(int argc, char **argv) {
  FILE *fin, *fout;
  char line[300];
  char file[100], ofile[100];
  int len, topK, tempid, i, j, a, b, numedge = 0, maxvv = 0, newid = 0, start, sid, Maxdis;
  int *trans;
  int *invtrans;
  int *dist, *outputid;
  double *simv, *sortsimv, *topsimv;
  double tempv;
  srand(time(NULL));
  if (argc != 9) {
    printf("Usage: Edge_file Output_file StartId T R c EndID Maxdis\n");
    return 2;
  }

  strcpy(file, argv[1]);
  strcpy(ofile, argv[2]);
  sscanf(argv[3], "%d", &start);
  sscanf(argv[4], "%d", &times);
  sscanf(argv[5], "%d", &numr);
  sscanf(argv[6], "%lf", &c);
  sscanf(argv[7], "%d", &topK);
  sscanf(argv[8], "%d", &Maxdis);

  fin = fopen(file, "r");
  if (!fin) {
    printf("File doesn't exist\n");
    return 1;
  }
  while (fgets(line, 100, fin) != NULL) {
    sscanf(line, "%d %d", &a, &b);
    numedge++;
    if (a > maxvv)
      maxvv = a;
    if (b > maxvv)
      maxvv = b;
    //printf("%d %d\n",a,b);
  }
  maxvv++;
  trans = (int *) malloc(maxvv * sizeof(int));
  for (i = 0; i < maxvv; i++)
    trans[i] = -1;
  fclose(fin);
  a = 0;
  b = 0;
  fin = fopen(file, "r");
  while (fgets(line, 100, fin) != NULL) {
    sscanf(line, "%d %d", &a, &b);
    if (trans[a] == -1) {
      trans[a] = newid;
      newid++;
    }
    if (trans[b] == -1) {
      trans[b] = newid;
      newid++;
    }
  }
  invtrans = (int *) malloc(newid * sizeof(int));
  dist = (int *) malloc(newid * sizeof(int));
  outputid = (int *) malloc(newid * sizeof(int));

  simv = (double *) malloc(newid * sizeof(double));
  sortsimv = (double *) malloc(newid * sizeof(double));
  topsimv = (double *) malloc(newid * sizeof(double));
  for (i = 0; i < maxvv; i++)
    if (trans[i] != -1)
      invtrans[trans[i]] = i;

  for (i = 0; i < newid; i++) {
    vector<int> temprow(1, -1);
    edge.push_back(temprow);
    invedge.push_back(temprow);
    foredge.push_back(temprow);
    simv[i] = 0;
    dist[i] = 1000000000;
    outputid[i] = i;
  }
  fclose(fin);
  fin = fopen(file, "r");
  vector<int>::iterator checkit;
  while (fgets(line, 100, fin) != NULL) {
    sscanf(line, "%d %d", &a, &b);
    checkit = find(edge[trans[a]].begin(), edge[trans[a]].end(), trans[b]);
    if (checkit == edge[trans[a]].end()) {
      edge[trans[a]].push_back(trans[b]);
      edge[trans[b]].push_back(trans[a]);
      invedge[trans[b]].push_back(trans[a]);
      invedge[trans[a]].push_back(trans[b]);
    }
  }
  fclose(fin);

  fout = fopen(ofile, "w+");
  clock_t start_t = clock();
  double simscore = singlepair(trans[start], trans[topK], numr);
  clock_t end_t = clock();
  fprintf(fout, "%d %d %lf %lu\n", start, topK, simscore, (unsigned long) (end_t - start_t) * 1000 / CLOCKS_PER_SEC);
  fclose(fout);
  return 0;
}

