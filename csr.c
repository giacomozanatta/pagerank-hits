#include "csr.h"
#include "constants.h"
#include <stdlib.h>
#include <string.h>

#ifdef MMAP
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

int allocate_csr_mmap(CSR* csr, int n_cols, int n_rows, char* file_name_prefix);
int csr_from_dataset_mmap(DATASET dataset, CSR* csr);
#endif

int destroy_csr(CSR* csr) {
    #ifdef MMAP
    if (munmap(csr->val,  sizeof(float)*csr->n_cols) == -1) {
        printf("[ERR] munmap error.\n");
        return STATUS_ERR;
    }

    if (munmap(csr->row_ptr, sizeof(int)*(csr->n_rows+1)) == -1) {
        printf("[ERR] munmap error.\n");
        return STATUS_ERR;
    }
    if (munmap(csr->col_index, sizeof(int)*(csr->n_cols)) == -1) {
        printf("[ERR] munmap error.\n");
        return STATUS_ERR;
    }
    #else
    free(csr->val);
    free(csr->col_index);
    free(csr->row_ptr);
    #endif
    csr->val = NULL;
    csr->col_index = NULL;
    csr->row_ptr = NULL;
    csr->n_cols = 0;
    csr->n_rows = 0;

    return STATUS_OK;
}

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
#ifdef MMAP
int csr_from_dataset_mmap(DATASET dataset, CSR* csr) {
    int** DATA = dataset.DATA;
    int i = 0;
    int n_elem_in_row = 0;
    int cum_n_elem_in_row = 0;
    int current_row = 0;
    float value = 1.0;
    char* val_file_name = malloc(strlen(dataset.name) + 4);
    char* col_index_name = malloc(strlen(dataset.name) + 4);
    char* row_ptr_name = malloc(strlen(dataset.name) + 4);


    FILE* file_row_ptr;
    FILE* file_col_idx;
    FILE* file_val;

    // get file name.
    strcat(val_file_name, dataset.name);
    strcat(val_file_name, "val");
    strcat(col_index_name, dataset.name);
    strcat(col_index_name, "col");
    strcat(row_ptr_name, dataset.name);
    strcat(row_ptr_name, "row");
    printf("%s %s %s", val_file_name, col_index_name, row_ptr_name);
    // open files in write mode.
    file_val = fopen(val_file_name, "w");
    if (file_val == NULL) {
        printf("[ERR] Error opening mmap file");
        return STATUS_ERR;
    }
    file_row_ptr = fopen(row_ptr_name, "w");
    if (file_row_ptr == NULL) {
        fclose(file_val);
        printf("[ERR] Error opening mmap file");
        return STATUS_ERR;
    }
    file_col_idx = fopen(col_index_name, "w");
    if (file_col_idx == NULL) {
        fclose(file_row_ptr);
        fclose(file_val);
        printf("[ERR] Error opening mmap file");
        return STATUS_ERR;
    }

    // writing files.
    fwrite(&i, sizeof(int), 1, file_row_ptr); // csr->row_ptr[0] = 0;
    while ( i < dataset.n_edges) {
        fwrite(&(DATA[i][1]), sizeof(int), 1, file_col_idx);        //csr->col_index[i] = DATA[i][1];
        fwrite(&(value), sizeof(float), 1, file_val);         //csr->val[i] = 1;
        if (current_row < DATA[i][0]) {
            current_row++;
            cum_n_elem_in_row += n_elem_in_row;
            fwrite(&cum_n_elem_in_row, sizeof(int), 1, file_row_ptr); // csr->row_ptr[current_row] = csr->row_ptr[current_row - 1] + n_elem_in_row; 
            n_elem_in_row = 1;
            while (current_row < DATA[i][0]) {
                current_row++;
                fwrite(&cum_n_elem_in_row, sizeof(int), 1, file_row_ptr); //csr->row_ptr[current_row] = csr->row_ptr[current_row - 1]; 
            }
        } else {
            n_elem_in_row++;
        }
        i++;
    }
    current_row++;
    cum_n_elem_in_row += n_elem_in_row;
    fwrite(&cum_n_elem_in_row, sizeof(int), 1, file_row_ptr); //csr->row_ptr[current_row] = csr->row_ptr[current_row - 1] + n_elem_in_row;
    current_row++;
    while (current_row < dataset.n_nodes + 1) {
        fwrite(&cum_n_elem_in_row, sizeof(int), 1, file_row_ptr); //csr->row_ptr[current_row] = csr->row_ptr[current_row - 1];
        current_row++; 
    }
    fflush(file_row_ptr);
    fflush(file_col_idx);
    fflush(file_val);
    // close files.
    fclose(file_row_ptr);
    fclose(file_col_idx);
    fclose(file_val);
    
    // free memory.
    free(val_file_name);
    free(col_index_name);
    free(row_ptr_name);
    // allocate CSR.
    return allocate_csr_mmap(csr, dataset.n_edges, dataset.n_nodes, dataset.name);
}

int allocate_csr_mmap(CSR* csr, int n_cols, int n_rows, char* file_name_prefix) {

    char* val_file_name = malloc(strlen(file_name_prefix) + 4);
    char* col_index_name = malloc(strlen(file_name_prefix) + 4);
    char* row_ptr_name = malloc(strlen(file_name_prefix) + 4);


    int fstream_row_ptr;
    int fstream_col_idx;
    int fstream_val;

    // get file name.
    strcat(val_file_name, file_name_prefix);
    strcat(val_file_name, "val");
    strcat(col_index_name, file_name_prefix);
    strcat(col_index_name, "col");
    strcat(row_ptr_name, file_name_prefix);
    strcat(row_ptr_name, "row");

    csr->n_cols = n_cols;
    csr->n_rows = n_rows + 1;
    // open and mmap val
    fstream_val = open(val_file_name, O_RDWR);
    if (fstream_val == -1) {
        printf("[ERR] Error opening mmap file\n");
        return STATUS_ERR;
    }
    csr->val = (float*) mmap(0, sizeof(float)*(n_cols), PROT_READ | PROT_WRITE, MAP_SHARED, fstream_val, 0);
    if (csr->val == MAP_FAILED) {
        close(fstream_val);
        printf("[ERR] MMAP error\n");
        return STATUS_ERR;
    }
    close(fstream_val);

    // open and mmap col_index
    fstream_col_idx = open(col_index_name, O_RDWR);
    if (fstream_col_idx == -1) {
        printf("[ERR] Error opening mmap file");
        return STATUS_ERR;
    }
    csr->col_index = (int*) mmap(0, sizeof(int)*(n_cols), PROT_READ, MAP_SHARED, fstream_col_idx, 0);
    if (csr->col_index == MAP_FAILED) {
        close(fstream_col_idx);
        printf("[ERR] MMAP error");
        return STATUS_ERR;
    }
    close(fstream_col_idx);

    // open and mmap row_ptr
    fstream_row_ptr = open(row_ptr_name, O_RDWR);
    if (fstream_row_ptr == -1) {
        printf("[ERR] Error opening mmap file");
        return STATUS_ERR;
    }
    csr->row_ptr = (int*) mmap(0, sizeof(int)*(n_rows + 1), PROT_READ, MAP_SHARED, fstream_row_ptr, 0);
    if (csr->row_ptr == MAP_FAILED) {
        close(fstream_row_ptr);
        printf("[ERR] MMAP error");
        return STATUS_ERR;
    }
    close(fstream_row_ptr);

    // free memory.
    free(val_file_name);
    free(col_index_name);
    free(row_ptr_name);
    
    return STATUS_OK;
}
#endif

int csr_from_dataset(DATASET dataset, CSR* csr) {
    int** DATA = dataset.DATA;
    int i = 0;
    int n_elem_in_row = 0;
    int current_row = 0;
    #ifdef MMAP
    return csr_from_dataset_mmap(dataset, csr);
    #else
    if (allocate_csr(csr, dataset.n_edges, dataset.n_nodes) != STATUS_OK) {
        return STATUS_ERR;
    }
    #endif
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
