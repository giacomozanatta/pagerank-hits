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

   // csr->row_ptr[0] = 0; // init the first row.

    // populate CSR from dataset.
    /*for (i = 0; i < csr->n_cols; i++) {
        csr->col_index[i] = DATA[i][1]; // col_index is the second element in data i-th array.
        csr->val[i] = 1; // val is always 1
        if (DATA[i][0] > row_idx) {
            int new_rows = DATA[i][0] - row_idx; // new rows to add to the CSR
            row_idx++;
            csr->row_ptr[row_idx] = csr->row_ptr[row_idx-1] + n_elem;

            for (j = 1; j <= new_rows - 1; j++) {
                csr->row_ptr[row_idx+j] = csr->row_ptr[row_idx];
               // printf("row_ptr[%d]  = %d\n", row_idx+j, i);
            }
             n_elem = 0;
            row_idx += new_rows - 1;
        }
        n_elem++;
    }
    // last position
    for (i = row_idx+1; i < csr->n_rows; i++) {
        csr->row_ptr[i] = csr->n_cols;
    }*/
    csr->row_ptr[0] = 0;
    while ( i < csr->n_cols) {

        csr->col_index[i] = DATA[i][1];
        csr->val[i] = 1;
       // printf("i=%d, current_row is %d, dataset row is %d. n_elem_in_row is %d\n", i, current_row, DATA[i][0], n_elem_in_row);
        if (current_row < DATA[i][0]) {
          //  printf("change current_row.\n");
            current_row++;
            //printf("\ti=%d, current_row is %d, dataset row is %d. n_elem_in_row is %d\n", i, current_row, DATA[i][0], n_elem_in_row);
            csr->row_ptr[current_row] = csr->row_ptr[current_row - 1] + n_elem_in_row; 

            n_elem_in_row = 1; // just advance to the next row -> we saw one element
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
    //csr->row_ptr[csr->n_rows-1] = csr->n_cols;
     printf("VAL: [ ");
    for (int i = 0; i < csr->n_cols; i++) {
        printf("%f ", csr->val[i]);
    }
    printf("]\n");
    printf("COL_IDX: [ ");
    for (int i = 0; i < csr->n_cols; i++) {
        printf("%d ", csr->col_index[i]);
    }
    printf("]\n");
    printf("ROW PTR: [ ");
    for (int i = 0; i < csr->n_rows; i++) {
        printf("%d ", csr->row_ptr[i]);
    }
    printf("]\n");
   // printf("row_ptr[%d]  = %d\n", csr->n_rows-1, csr->n_cols);
    return STATUS_OK;
}

int make_stochastic(CSR* csr) {
    int i, j;
    int col_bound = 0;
    j = 0;
    printf("***** STOCAZZI *****\n\n");
    for (i = 0; i < csr->n_rows; i++) {
        int n_out_links = csr->row_ptr[i+1] - csr->row_ptr[i];
        col_bound = col_bound + n_out_links;
        while (j < col_bound) {
           //printf("csr->val[%d] = %f - n_out_links -> %d\n", j, csr->val[j], n_out_links);
            csr->val[j] = csr->val[j] / n_out_links;
           //printf("csr->val[%d] = %f - n_out_links -> %d\n", j, csr->val[j], n_out_links);
            j++;
        }
    }
    return STATUS_OK;
}