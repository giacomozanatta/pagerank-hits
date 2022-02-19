#ifndef DATASET_H
#define DATASET_H

#define FROM_NODE_ID_FIRST 0
#define TO_NODE_ID_FIRST 1

#include <stdio.h>

typedef struct DATASET {
    int** DATA;
    int n_nodes;
    int n_edges;
} DATASET;

int read_dataset_from_file(char *file_path, DATASET* dataset, int order);
void sort_dataset(DATASET *dataset);
void destroy_dataset(DATASET* dataset);

#endif
