#include "dataset.h"

#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "utils.h"

void destroy_on_error(DATASET* dataset, int allocated_pointers) {
    int i = 0;
    for (i = 0; i < allocated_pointers; i++) {
        free(dataset->DATA[i]);
    }
    free(dataset->DATA);
    dataset->DATA = NULL;
    dataset->n_edges = 0;
    dataset->n_nodes = 0;
}

void destroy_dataset(DATASET* dataset) {
    int i = 0;
    for (i = 0; i < dataset->n_edges; i++) {
        free(dataset->DATA[i]);
    }
    free(dataset->DATA);
    dataset->DATA = NULL;
    dataset->n_edges = 0;
    dataset->n_nodes = 0;
}


int compare_dataset_entries(const void *a, const void *b) {
    const int *_a = *(const int **)a;
    const int *_b = *(const int **)b;
    if(_a[0] == _b[0]) {
        return _a[1] - _b[1];
    } else {
        return _a[0] - _b[0];
    }

}

void sort_dataset(DATASET* dataset) {
    qsort(dataset->DATA, dataset->n_edges, sizeof(*(dataset->DATA)), compare_dataset_entries);
}

int get_dataset_size(FILE *file, DATASET* dataset) {
    
    char *readed_line = NULL;
    ssize_t n_char_readed = 0;
    size_t len = 0;
    int status = STATUS_ERR;

    while((n_char_readed = getline(&readed_line, &len, file)) != EOF) {
        if (n_char_readed > 0) {
            if (readed_line[0] != '#') {
                break;
            } else {
                if (starts_with("# Nodes:", readed_line)) {
                    if (sscanf(readed_line, "# Nodes: %d Edges: %d", &(dataset->n_nodes), &(dataset->n_edges)) != EOF) {
                        free(readed_line);
                        readed_line = NULL;
                        len = 0;
                        if ((n_char_readed = getline(&readed_line, &len, file)) != EOF) {
                            status = STATUS_OK;
                        }
                        break;
                    }
                }
            }
        }
        free(readed_line);
        readed_line = NULL;
        len = 0;
    }
    return status;
}

int get_dataset_entry(FILE *file, int *to_node_id, int *from_node_id) {
    char *readed_line = NULL;
    ssize_t n_char_readed = 0;
    size_t len = 0;
    if ((n_char_readed = getline(&readed_line, &len, file) != EOF)) {
        if (sscanf( readed_line, "%d  %d", from_node_id, to_node_id) != EOF) {
            return STATUS_OK;
        } else {
            return STATUS_ERR;
        }
    }   
    return EOF;
}

int get_dataset_entries(FILE *file, DATASET* dataset, int order) {
    int to_node_id;
    int from_node_id;
    int status;
    int i;
    int **DATA;

    i = 0;
    DATA = dataset->DATA;

    while((status = get_dataset_entry(file, &(to_node_id), &from_node_id)) == STATUS_OK || i < dataset->n_edges) {

        DATA[i] = (int*)malloc(sizeof(int)*2);
        if (DATA[i] != NULL) {
            if (order == TO_NODE_ID_FIRST) {
                DATA[i][0] = to_node_id;
                DATA[i][1] = from_node_id;
            } else {
                DATA[i][0] = from_node_id;
                DATA[i][1] = to_node_id;
            }
        } else {
            destroy_on_error(dataset, i+1);
            return STATUS_ERR;
        }
        i++;
    }
    if (dataset->n_edges != i) {
        printf("[ERR] Number of edges mismatch: Header says %d  but in file there are %d edges\n", dataset->n_edges, i);
        return STATUS_ERR;
    }
    if (status != EOF) {
        return STATUS_ERR;
    }
    if (order == TO_NODE_ID_FIRST) {
        sort_dataset(dataset);
    }
    return STATUS_OK;
}

int read_dataset_from_file(char *file_path, DATASET* dataset, int order) {
    FILE *f;
    int status;
    f = fopen(file_path,"r");

    if (f == NULL) {
        printf("[ERR] Fail opening the file.\n");
        return STATUS_ERR;
    }

    if (get_dataset_size(f, dataset) == NOT_FOUND) {
        printf("[ERR] Fail getting dataset size.\n");
        return STATUS_ERR;
    }
    // initialize DATASET
    dataset->DATA = (int**)malloc(sizeof(int*)*dataset->n_edges);
    //TODO: check error
    if ((status = get_dataset_entries(f, dataset, order)) == STATUS_ERR) {
        printf("[ERR] ");
    }
    if (fclose(f) == EOF) {
        printf("[ERR] Error closing the file.\n");
        return STATUS_ERR;
    }
    return STATUS_OK;
}

void print_dataset(DATASET dataset) {
    int i;
    printf("***** DATASET *****\n");
    printf("N_NODES: %d\tN_EDGES=%d\n", dataset.n_nodes, dataset.n_edges);
    for (i = 0; i < dataset.n_edges; i++) {
        printf("%d %d\n", dataset.DATA[i][0], dataset.DATA[i][1]);
    }
}
