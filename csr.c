#include "csr.h"
#include "constants.h"
#include <stdlib.h>

int allocate_csr(CSR* csr, int n_cols, int n_rows) {
    csr->val = (float*) malloc(sizeof(float)*n_cols);

    if (csr->val == NULL) {
        return STATUS_ERR;
    }
    
    csr->col_index = (int*) malloc(sizeof(int)*n_cols);
    
    if (csr->col_index == NULL) {
        free(csr->val);
        csr->val = NULL;
        return STATUS_ERR;
    }
    
    csr->row_ptr = (int*) malloc(sizeof(int)*(n_rows+1));
    
    if (csr->row_ptr == NULL) {
        free(csr->val);
        csr->val = NULL;
        free(csr->col_index);
        csr->col_index = NULL;
        return STATUS_ERR;
    }

    csr->n_cols = n_cols;
    csr->n_rows = n_rows + 1;

    return STATUS_OK;
}

int csr_from_dataset(DATASET dataset, CSR* csr) {
    int** DATA = dataset.DATA;
    int i = 0;
    int j;
    int row = 0; 
    int row_idx = 0;
    int n_elem_in_row = 0;
    int current_row = 0;
    if (allocate_csr(csr, dataset.n_edges, dataset.n_nodes) != STATUS_OK) {
        return STATUS_ERR;
    }

    csr->row_ptr[0] = 0;
    while ( i < csr->n_cols) {

        csr->col_index[i] = DATA[i][1];
        csr->val[i] = 1;
        if (current_row < DATA[i][0]) {
            current_row++;
            csr->row_ptr[current_row] = csr->row_ptr[current_row - 1] + n_elem_in_row; 

            n_elem_in_row = 1;
            while (current_row < DATA[i][0]) {
                current_row++;
                csr->row_ptr[current_row] = csr->row_ptr[current_row - 1]; 
            }
        } else {
            n_elem_in_row++;
        }
        i++;
    }
    current_row++;
    csr->row_ptr[current_row] = csr->row_ptr[current_row - 1] + n_elem_in_row;
    current_row++;
    while (current_row < csr->n_rows) {
        csr->row_ptr[current_row] = csr->row_ptr[current_row - 1];
        current_row++; 
    }
    return STATUS_OK;
}

int make_stochastic(CSR* csr) {
    int i, j;
    int col_bound = 0;
    j = 0;
    for (i = 0; i < csr->n_rows; i++) {
        int n_out_links = csr->row_ptr[i+1] - csr->row_ptr[i];
        col_bound = col_bound + n_out_links;
        while (j < col_bound) {
            csr->val[j] = csr->val[j] / n_out_links;
            j++;
        }
    }
    return STATUS_OK;
}