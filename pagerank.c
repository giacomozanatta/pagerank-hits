#include "pagerank.h"
#include <math.h>
#include <stdlib.h>

float get_error(float* P, float* P_next, int n) {
    float error = 0.0;
    int i = 0;
    for (i = 0; i < n; i++) {
        //printf("%f  -   %f\n", P_next[i], P[i]);
        error += fabs(P_next[i] - P[i]);
        //printf("%f\n", error);
    }
    return error;
}

void pagerank(CSR csr) {
    int i, j, col_bound, k;
    float *P;   
    float *P_next;
    double d;
    float error = 0.0;
    P = (float*)malloc(sizeof(float)*(csr.n_rows-1));
    if (P == NULL) {
        printf("[ERR] Error allocating file.\n");
        //return STATUS_ERR;
        return;
    }
    // TODO: check allocation
    P_next = (float*)malloc(sizeof(float)*(csr.n_rows-1));
    if (P_next == NULL) {
        printf("[ERR] Error allocating P_next.\n");
        free(P);
        //return STATUS_ERR;
        return;
    }
    d = 0.85;
    k = 0;
    for (i = 0; i < csr.n_rows-1; i++) {
        P[i] = 1.0 / (csr.n_rows-1);
        P_next[i] = 0.0;
    }
    do {
        j = 0;
        col_bound = 0;
        for (i = 0; i < csr.n_rows-1; i++) {
            int n_out_links = csr.row_ptr[i+1] - csr.row_ptr[i];
            col_bound = col_bound + n_out_links;
            //printf("col bound = %d\n", col_bound);
            //printf("n_out_links = %d\n", n_out_links);
            while (j < col_bound) {
                //printf("%d\n",j);
                P_next[csr.col_index[j]] += csr.val[j] * P[i];
                j++;
            }
        }

        for (i = 0; i < csr.n_rows-1; i++) {
           P_next[i] = (d * P_next[i]) + (1.0 - d) / (csr.n_rows-1);
        }

        error = get_error(P, P_next, csr.n_rows-1);
        printf("error: %f\n", error);
        for (i = 0; i < csr.n_rows-1; i++) {
           P[i] = P_next[i];
           P_next[i] = 0.0;
        }
        k++;
        for (i = 0; i < csr.n_rows-1; i++) {
           // printf("%f\n", P[i]);
        }
    } while(error > 0.000001);
    // TODO: free and return
    printf("number of iterations: %d\n", k);
}