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


int main() {
    //DATASET dataset;
    int status;
    int i;
    DATASET dataset;
    status = read_dataset_from_file("data/web-NotreDame.txt", &dataset);
    
    if (status == STATUS_ERR) {
        printf("[ERR] Error reading file.\n");
        return STATUS_ERR;
    }

   // qsort(_DATASET, n_edges, sizeof(_DATASET[0]), compare_dataset_entries);
    for (i = 0; i < dataset.n_edges; i++) {
        printf("\t%d\t%d\n", dataset.DATA[i][0], dataset.DATA[i][1]);
    }
    
    printf("[OK] Success.\n");
    return 0;
}
