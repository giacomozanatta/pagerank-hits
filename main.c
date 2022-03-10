#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataset.h"
#include "constants.h"
#include "csr.h"
#include "pagerank.h"

int main() {

    int status;
    //int i;
    DATASET dataset;
    CSR csr;
    status = read_dataset_from_file("data/web-NotreDame.txt", &dataset, FROM_NODE_ID_FIRST);
    if (status == STATUS_ERR) {
        printf("[ERR] Error reading file.\n");
        return STATUS_ERR;
    }
    /*for (i = 0; i < dataset.n_edges; i++) {
        printf("\t%d\t%d\n", dataset.DATA[i][0], dataset.DATA[i][1]);
    }*/
   // printf("n_rows: %d n_cols:%d\n", dataset.n_nodes, dataset.n_edges);
   print_dataset(dataset);
    status = csr_from_dataset(dataset, &csr);
    if (status == STATUS_ERR) {
        printf("[ERR] Error creating CSR.\n");
        return STATUS_ERR;
    }
    status = make_stochastic(&csr);
    pagerank(csr);

    //hits_iterate();
    //in_degree_ranking();
    //jaccard();
   /* printf("***** NUM_COLS: %d\n", csr.n_cols);
    for (int i = 0; i < csr.n_cols; i++) {
        printf("%d\n", csr.col_index[i]);
    }
    
    printf("****** NUM_ROWS: %d\n", csr.n_rows);
    for (i = 0; i < csr.n_rows; i++) {
        printf("%d\n", csr.row_ptr[i]);
    }*/
    printf("\n\n\n\n");
    
    destroy_dataset(&dataset);
    printf("[OK] Success.\n");
    return STATUS_OK;
}
