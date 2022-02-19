#ifndef DATASET_H
#define DATASET_H

#include <stdio.h>

typedef struct DATASET {
    int** DATA;
    int n_nodes;
    int n_edges;
} DATASET;

int read_dataset_from_file(char *file_path, DATASET* dataset);

void destroy(DATASET* dataset);

#endif
