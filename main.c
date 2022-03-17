#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "dataset.h"
#include "constants.h"
#include "csr.h"
#include "ranking.h"

int compute_pagerank(CSR csr, Ranking* page_rank);
int compute_hits(CSR csr, CSR csr_transpose, Ranking* hits_authority, Ranking* hits_hub);
int compute_indegree_rank(CSR csr, Ranking *indegree_rank);
void print_top_k(Ranking R, int k);
void compute_score(Ranking A, Ranking B, int k);

int main(int argc, char *argv[]) {

    Ranking page_rank = NULL;
    Ranking hits_authority_rank = NULL;
    Ranking hits_hub_rank = NULL;
    Ranking indegree_rank = NULL;

    DATASET dataset;
    DATASET dataset_transpose;

    CSR csr;
    CSR csr_transpose;
    CSR csr_transpose_stochastic;

    clock_t clock_start_time;
    clock_t clock_start_time_total = clock();

    char *name;
    char* file_name;
    int k;
    // get the filename and k from argv
    if (argc != 3) {
        printf("[ERR] usage: ./main filepath k\n");
        return STATUS_ERR;
    }
    k = atoi(argv[2]);
    file_name = argv[1];
    // Read from file
    clock_start_time = clock();
    if (read_dataset_from_file(file_name, &dataset, FROM_NODE_ID_FIRST) == STATUS_ERR) {
        printf("[ERR] Error reading file.\n");
        return STATUS_ERR;
    }
    printf("[INFO] Done reading dataset. Time: %f\n", (double)(clock() - clock_start_time) / CLOCKS_PER_SEC);
    printf("[INFO] Reading transposed dataset...\n");

    clock_start_time = clock();
    if (read_dataset_from_file(file_name, &dataset_transpose, TO_NODE_ID_FIRST) == STATUS_ERR) {
        printf("[ERR] Error reading file.\n");
        return STATUS_ERR;
    }

    printf("[INFO] Done reading transpose dataset. Time: %f\n", (double)(clock() - clock_start_time) / CLOCKS_PER_SEC);
    printf("[INFO] Reading dataset...\n");
    clock_start_time = clock();

    printf("[INFO] Generating CSR...\n");
    clock_start_time = clock();
    // generate CSR matrix
    if (csr_from_dataset(dataset, &csr) == STATUS_ERR) {
        printf("[ERR] CSR Creation error.\n");
        return STATUS_ERR;
    }
    printf("[INFO] Done generating CSR. Time: %f\n", (double)(clock() - clock_start_time) / CLOCKS_PER_SEC);
    
    clock_start_time = clock();
    printf("[INFO] Generating transposed CSR...\n");
    if (csr_from_dataset(dataset_transpose, &csr_transpose) == STATUS_ERR) {
        printf("[ERR] CSR Creation error.\n");
        return STATUS_ERR;
    }

    printf("[INFO] Done generating transpose CSR. Time: %f\n", (double)(clock() - clock_start_time) / CLOCKS_PER_SEC);
    clock_start_time = clock();

    clock_start_time = clock();
    printf("[INFO] Generating transposed CSR...\n");
    // change dataset name
    name = (char*)malloc(strlen(dataset_transpose.name) + 2);
    strcpy(name, dataset.name);
    strcat(name, "S");
    strcpy(dataset_transpose.name, name);
    free(name);
    if (csr_from_dataset(dataset_transpose, &csr_transpose_stochastic) == STATUS_ERR) {
        printf("[ERR] CSR Creation error.\n");
        return STATUS_ERR;
    }
    printf("[INFO] Done generating transpose CSR. Time: %f\n", (double)(clock() - clock_start_time) / CLOCKS_PER_SEC);
    printf("[INFO] Stochastization...\n");
    clock_start_time = clock();
    if (make_stochastic(&csr_transpose_stochastic) == STATUS_ERR) {
        printf("[ERR] Matrix Stochastization Error.\n");
        free(&csr);
        return STATUS_ERR;
    }
    printf("[INFO] Done stochastization. Time: %f\n", (double)(clock() - clock_start_time) / CLOCKS_PER_SEC);

    clock_start_time = clock();

    printf("[INFO] Computing Pagerank...\n");
    compute_pagerank(csr_transpose_stochastic, &page_rank);
    printf("[INFO] Done computing Pagerank. Total time spent: %f\n", (double)(clock() - clock_start_time) / CLOCKS_PER_SEC);
    printf("[INFO] Computing HITS...\n");
    clock_start_time = clock();
    compute_hits(csr, csr_transpose, &hits_authority_rank, &hits_hub_rank);
    printf("[INFO] Done computing HITS. Total time spent: %f\n", (double)(clock() - clock_start_time) / CLOCKS_PER_SEC);
    printf("[INFO] Computing INDEGREE...\n");
    clock_start_time = clock();
    compute_indegree_rank(csr_transpose, &indegree_rank);
    printf("[INFO] Done computing INDEGREE. Total time spent: %f\n", (double)(clock() - clock_start_time) / CLOCKS_PER_SEC);
    
    
    printf("\n\n");
    printf("\n***** PAGE RANK TOP %d *****\n", k);
    print_top_k(page_rank, k);

    printf("\n\n***** HITS AUTHORITY TOP %d *****\n", k);
    print_top_k(hits_authority_rank, k);
    printf("\n\n***** HITS HUB TOP %d *****\n", k);
    print_top_k(hits_hub_rank, k);
    printf("\n\n***** INDEGREE HUB TOP %d *****\n", k);
    print_top_k(indegree_rank, k);

    printf("\n\nJACCARD PAGE RANK - HITS AUTH:\n");
    compute_score(page_rank, hits_authority_rank, k);

    printf("\n\nJACCARD PAGE RANK - INDEGREE:\n");
    compute_score(page_rank, indegree_rank, k);

    printf("\n\nJACCARD HITS AUTH - INDEGREE:\n");
    compute_score(hits_authority_rank, indegree_rank, k);

    printf("[INFO] Freeing memory...\n");
    destroy_dataset(&dataset);
    destroy_dataset(&dataset_transpose);
    
    destroy_csr(&csr);
    destroy_csr(&csr_transpose);
    destroy_csr(&csr_transpose_stochastic);
    printf("[INFO] DONE.\n");
    printf("[INFO] Total time: %f\n\n", (double)(clock() - clock_start_time_total) / CLOCKS_PER_SEC);
    return STATUS_OK;
}

int compute_pagerank(CSR csr, Ranking* page_rank) {
    clock_t clock_start_time;
    int n_iter;
    printf("[INFO] PAGERANK: Computing... \n");
    clock_start_time = clock();
    if (pagerank(csr, page_rank, &n_iter) == STATUS_ERR) {
        printf("[ERR] PageRank calculation error.\n");
        free(&csr);
        free(page_rank);
        return STATUS_ERR;
    }
    printf("[INFO] PAGERANK: Done computing. Time: %f\n", (double)(clock() - clock_start_time) / CLOCKS_PER_SEC);
    printf("[INFO] PAGERANK: No. of iterations: %d\n", n_iter);
    printf("[INFO] PAGERANK: sorting rank entries...\n");
    clock_start_time = clock();
    sort_ranking(*page_rank, csr.n_rows-1);
    printf("[INFO] PAGERANK: done sorting rank entries. Time: %f\n", (double)(clock() - clock_start_time) / CLOCKS_PER_SEC);
    return STATUS_OK;
}

int compute_hits(CSR csr, CSR csr_transpose, Ranking* hits_authority, Ranking* hits_hub) {
    int n_iter;
    clock_t clock_start_time;
    printf("[INFO] HITS: Computing... \n");
    clock_start_time = clock();
    if (hits(csr, csr_transpose, hits_authority, hits_hub, &n_iter) == STATUS_ERR) {
        printf("[ERR] HITS calculation error.\n");
        return STATUS_ERR;
    }
    printf("[INFO] HITS: Done computing. Time: %f\n", (double)(clock() - clock_start_time) / CLOCKS_PER_SEC);
    printf("[INFO] HITS: No. of iterations: %d\n", n_iter);
    printf("[INFO] HITS: sorting rank entries...");
    clock_start_time = clock();
    sort_ranking(*hits_authority, csr.n_rows-1);
    sort_ranking(*hits_hub, csr.n_rows-1);
    printf("[INFO] HITS: done sorting rank entries. Time: %f\n", (double)(clock() - clock_start_time) / CLOCKS_PER_SEC);
    return STATUS_OK;
}

int compute_indegree_rank(CSR csr, Ranking *indegree_rank) {
    clock_t clock_start_time;
    printf("[INFO] INDEGREE: Computing...\n");
    clock_start_time = clock();
    if (indegree(csr, indegree_rank) == STATUS_ERR) {
        printf("[ERR] InDegree calculation error.\n");
        free(&csr);
        return STATUS_ERR;
    }
    printf("[INFO] INDEGREE: Done computing. Time: %f\n", (double)(clock() - clock_start_time) / CLOCKS_PER_SEC);
    clock_start_time = clock();
    printf("[INFO] INDEGREE: sorting rank entries...\n");
    sort_ranking(*indegree_rank, csr.n_rows-1);
    printf("[INFO] INDEGREE: done sorting rank entries. Time: %f\n", (double)(clock() - clock_start_time) / CLOCKS_PER_SEC);

    return STATUS_OK;
}

void print_top_k(Ranking R, int k) {
    int i;
    for (i = 0; i < k; i++) {
        printf("%d) ID: %d RANK: %f\n", i+1, R[i].ID, R[i].value);
    }
}

void compute_score(Ranking A, Ranking B, int k) {
    int i = k;
    printf("K\tVALUE\n\n");
    //for (i = ; i < k; i+=100) {
    printf("%d\t%f\n", i, jaccard_score(A,B,i));
    //}
}
