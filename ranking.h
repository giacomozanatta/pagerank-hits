#ifndef RANKING
#define RANKING

#include "csr.h"
typedef struct RankEntry{
    int ID;
    float value;
} RankEntry;

typedef RankEntry* Ranking;

int pagerank(CSR csr, Ranking* P, int* n_iter);
int hits(CSR csr, CSR csr_traspose, Ranking* A, Ranking* H, int *n_iter);
int indegree(CSR csr, Ranking* I);
void sort_ranking(Ranking R, int n);
double jaccard_score(Ranking A, Ranking B, int k);
#endif
