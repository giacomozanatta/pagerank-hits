#include "ranking.h"
#include <math.h>
#include <stdlib.h>
#include "constants.h"

// TODO: allows user to choose the desired error threshold
#define ERROR_THRESHOLD 10e-8

int number_eq_elements(Ranking A, Ranking B, int k) {
    int i, j;
    int count = 0;
    for (i = 0; i < k; i++) {
        for (j = 0; j < k; j++) {
            if (A[i].ID == B[j].ID) {
                count++;
                break;
            }
        }
    }
    return count;
}


double jaccard_score(Ranking A, Ranking B, int k) {
    double eq_elements = (double)number_eq_elements(A,B, k);
    if (eq_elements == 0) {
        return 0.0;
    }
    return (double) eq_elements / (((double) k*2) - eq_elements);
}

int compare_rank_entries(const void *a, const void *b) {
    // sort in reverse order
    const RankEntry *_a = (const RankEntry *)a;
    const RankEntry *_b = (const RankEntry *)b;
    if (_b->value == _a->value) {
        return 0;
    }
    
    if (_b->value > _a->value) {
        return 1;
    }
    
    return -1;
}
/**
 * @brief Calculate the error between 2 ranking. 
 * 
 * @param M - a ranking
 * @param M_next - another ranking
 * @param n - input size
 * @return float - the error
 */
float get_error(Ranking M, Ranking M_next, int n) {
    float error = 0.0;
    int i = 0;
    for (i = 0; i < n; i++) {
        error += fabs(M_next[i].value - M[i].value);
    }
    return error;
}
/**
 * @brief compute the PageRank of the given graph.
 * 
 * @param csr - the input graph.
 * @param P - the Page Rank Ranking.
 * @param n_iter - number of iteration 
 * @return int - STATUS 
 */
int pagerank(CSR csr, Ranking* P, int* n_iter) {
    int i, j, col_bound;
    Ranking P_next;
    double d;
    float error = 0.0;
    *n_iter = 0;
    if (*P != NULL) {
        free(*P);
    }
    *P = (Ranking)malloc(sizeof(RankEntry)*(csr.n_rows-1));
    if (*P == NULL) {
        printf("[ERR] Error allocating file.\n");
        return STATUS_ERR;
    }
    P_next = (Ranking)malloc(sizeof(RankEntry)*(csr.n_rows-1));
    if (P_next == NULL) {
        printf("[ERR] Error allocating P_next.\n");
        free(P);
        return STATUS_ERR;
    }
    // set the dumping factor
    //TODO: allows user to choose the desired d value.
    d = 0.85;
    for (i = 0; i < csr.n_rows-1; i++) {
        (*P)[i].ID = i;
        (*P)[i].value = 1.0 / (csr.n_rows-1);
        P_next[i].ID = i;
        P_next[i].value = 0.0;
    }
    do {
        j = 0;
        col_bound = 0;
        for (i = 0; i < csr.n_rows-1; i++) {
            int n_out_links = csr.row_ptr[i+1] - csr.row_ptr[i];
            col_bound = col_bound + n_out_links;
            while (j < col_bound) {
                P_next[csr.col_index[j]].value += csr.val[j] * (*P)[i].value;
                j++;
            }
        }

        for (i = 0; i < csr.n_rows-1; i++) {
           P_next[i].value = (d * P_next[i].value) + (1.0 - d) / (csr.n_rows-1);
        }

        error = get_error(*P, P_next, csr.n_rows-1);
        for (i = 0; i < csr.n_rows-1; i++) {
           (*P)[i].value = P_next[i].value;
           P_next[i].value = 0.0;
        }
        (*n_iter)++;
    } while(error > ERROR_THRESHOLD);
    free(P_next);
    return STATUS_OK;
}

/**
 * @brief compute the HITS of the graph.
 * 
 * @param csr - the input graph
 * @param csr_traspose - the transposed input graph
 * @param A - the Authority output ranking
 * @param H - the Hub output ranking
 * @param n_iter - number of iterations
 * @return int - STATUS
 */
int hits(CSR csr, CSR csr_traspose, Ranking* A, Ranking* H, int *n_iter) {
    Ranking A_next = NULL;
    Ranking H_next = NULL;
    float error_a, error_h;
    int i, j_a, j_h;
    int col_bound_a = 0;
    int col_bound_h = 0;
    if (*A != NULL) {
        free(*A);
    }
    *A = (Ranking)malloc(sizeof(RankEntry)*(csr.n_rows-1));
    if (*A == NULL) {
        printf("[ERR] Error allocating file.\n");
        return STATUS_ERR;
    }
    A_next = (Ranking)malloc(sizeof(RankEntry)*(csr.n_rows-1));
    if (A_next == NULL) {
        printf("[ERR] Error allocating P_next.\n");
        free(*A);
        return STATUS_ERR;
    }
    if (*H != NULL) {
        free(*H);
    }
    *H = (Ranking)malloc(sizeof(RankEntry)*(csr.n_rows-1));
    if (*H == NULL) {
        printf("[ERR] Error allocating file.\n");
        free(A_next);
        free(*A);
        return STATUS_ERR;
    }
    H_next = (Ranking)malloc(sizeof(RankEntry)*(csr.n_rows-1));
    if (H_next == NULL) {
        printf("[ERR] Error allocating H_next.\n");
        free(*A);
        free(A_next);
        free(*H);
        return STATUS_ERR;
    }

    for (i = 0; i < csr.n_rows-1; i++) {
        (*A)[i].ID = i;
        (*A)[i].value = 1.0;
        (*H)[i].ID = i;
        (*H)[i].value = 1.0;
        A_next[i].ID = i;
        A_next[i].value = 0.0;
        H_next[i].ID = i;
        H_next[i].value = 0.0;
    }

    *n_iter = 0;
    
    do {
        float sum_a = 0.0;
        float sum_h = 0.0;
        j_a = 0;
        j_h = 0;
        col_bound_a = 0;
        col_bound_h = 0;
        for (i = 0; i < csr.n_rows-1; i++) {
            int n_out_links_a = csr_traspose.row_ptr[i+1] - csr_traspose.row_ptr[i];
            int n_out_links_h = csr.row_ptr[i+1] - csr.row_ptr[i];
            col_bound_a = col_bound_a + n_out_links_a;
            col_bound_h = col_bound_h + n_out_links_h;
            while (j_a < col_bound_a) {
                A_next[i].value += (csr_traspose.val[j_a]*(*H)[csr_traspose.col_index[j_a]].value);
                j_a++;
            }
            while (j_h < col_bound_h) {
                H_next[i].value += (csr.val[j_h]*(*A)[csr.col_index[j_h]].value);
                j_h++;
            }
        }

        for (i = 0; i < csr.n_rows-1; i++) {
            sum_a += A_next[i].value;
            sum_h += H_next[i].value;
        }

        for (i = 0; i < csr.n_rows-1; i++) {
            H_next[i].value = H_next[i].value / sum_h;
            A_next[i].value = A_next[i].value / sum_a;
        }
        // normalization
        error_a = get_error(*A, A_next, csr.n_rows-1);
        error_h = get_error(*H, H_next, csr.n_rows-1);
        for (i = 0; i < csr.n_rows-1; i++) {
            (*H)[i].value = H_next[i].value;
            H_next[i].value = 0.0;
            (*A)[i].value = A_next[i].value;
            A_next[i].value = 0.0;
        }

        (*n_iter)++;

    } while(error_a > ERROR_THRESHOLD && error_h > ERROR_THRESHOLD);
    free(H_next);
    free(A_next);
    return STATUS_OK;
}

/**
 * @brief compute the indegree ranking
 * 
 * @param csr - the input graph
 * @param I - the output ranking
 * @return int - STATUS
 */
int indegree(CSR csr, Ranking* I) {
    int i;
    if (*I != NULL) {
        free(*I);
    }
    *I = (Ranking)malloc(sizeof(RankEntry)*(csr.n_rows-1));
    if (*I == NULL) {
        printf("[ERR] Error allocating Ranking.\n");
        return STATUS_ERR;
    }
    for (i = 0; i < csr.n_rows-1; i++) {
        int n_out_links = csr.row_ptr[i+1] - csr.row_ptr[i];
        (*I)[i].ID = i;
        (*I)[i].value = (double) n_out_links / (double) (csr.n_rows-1);
    }
    return STATUS_OK;
}

void sort_ranking(Ranking R, int n) {
    qsort(R, n, sizeof(*R), compare_rank_entries);
}
