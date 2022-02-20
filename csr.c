#include "csr.h"
#include "constants.h"
#include <stdlib.h>

int allocate_csr(CSR* csr, int n_cols, int n_rows) {
    csr->val = (int*) malloc(sizeof(int)*n_cols);

    if (csr->val == NULL) {
        return STATUS_ERR;
    }
    
    csr->col_index = (int*) malloc(sizeof(int)*n_cols);
    
    if (csr->col_index == NULL) {
        free(csr->val);
        csr->val = NULL;
        return STATUS_ERR;
    }
    
    csr->row_ptr = (int*) malloc(sizeof(int)*(n_rows));
    
    if (csr->row_ptr == NULL) {
        free(csr->val);
        csr->val = NULL;
        free(csr->col_index);
        csr->col_index = NULL;
        return STATUS_ERR;
    }

    csr->n_cols = n_cols;
    csr->n_rows = n_rows;

    return STATUS_OK;
}

int csr_from_dataset(DATASET dataset, CSR* csr) {
    int** DATA = dataset.DATA;
    int i;
    int j;
    int row = 0; 
    int row_idx = 0;
    if (allocate_csr(csr, dataset.n_edges, dataset.n_nodes) != STATUS_OK) {
        return STATUS_ERR;
    }

    csr->row_ptr[0] = 0; // init the first row.

    // populate CSR from dataset.
    for (i = 0; i < csr->n_cols; i++) {
        csr->col_index[i] = DATA[i][1]; // col_index is the second element in data i-th array.
        csr->val[i] = 1; // val is always 1
        if (DATA[i][0] > row_idx) {
            int new_rows = DATA[i][0] - row_idx; // new rows to add to the CSR
            for (j = 1; j <= new_rows; j++) {
                csr->row_ptr[row_idx+j] = i;
            }
            row_idx += new_rows;
        }
    }
    return STATUS_OK;
}

int make_stochastic(CSR* csr) {
    //TODO
    return STATUS_OK;
}