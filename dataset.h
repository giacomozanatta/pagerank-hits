#ifndef DATASET_H
#define DATASET_H

#include <stdio.h>

typedef struct DATASET {
    char** DATA;
    int n_nodes;
    int n_edges;
} DATASET;

int compare_dataset_entries(const void *a, const void *b);

int get_dataset_size(FILE *file, int *n_nodes, int* n_edges);

int get_dataset_entry(FILE *file, int *to_node_id, int *from_node_id);

int get_dataset_entries(FILE *file, int*** DATASET, int n_edges);

int read_file(char *file_path, int*** DATASET, int *n_nodes, int* n_edges);

#endif
