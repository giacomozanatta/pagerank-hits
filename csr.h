#ifndef CSR_H
#define CSR_H
#include "dataset.h"

typedef struct CSR {
    int* val;
    int *col_index;
    int *row_ptr;
    int n_rows;
    int n_cols;
} CSR;

int csr_from_dataset(DATASET dataset, CSR* csr);

int make_stochastic(CSR* csr);
#endif
