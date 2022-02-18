#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataset.h"
#include "constants.h"

typedef struct CSR {
    char* var;
    char *col_index;
    char *row_ptr;
} CSR;

int** _DATASET;

int main() {
    //DATASET dataset;
    int n_edges, n_nodes;
    int status;
    int i;

    status = read_file("data/web-NotreDame.txt", &_DATASET, &n_nodes, &n_edges);
    
    if (status == STATUS_ERR) {
        printf("[ERR] Error reading file.\n");
        return STATUS_ERR;
    }

    qsort(_DATASET, n_edges, sizeof(_DATASET[0]), compare_dataset_entries);
    /*for (i = 0; i < n_edges; i++) {
        printf("\t%d\t%d\n", _DATASET[i][0], _DATASET[i][1]);
    }*/
    
    printf("[OK] Success.\n");
    return 0;
}
