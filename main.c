#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STATUS_OK 0
#define STATUS_ERR 1

#define NOT_FOUND 1
#define FOUND 0

typedef struct CSR {
    char* var;
    char *col_index;
    char *row_ptr;
} CSR;

int** DATASET;

int compare_dataset_entries(const void *a, const void *b) {
    const int *_a = *(const int **)a;
    const int *_b = *(const int **)b;

    if(_a[0] == _b[0]) {
        return _a[1] - _b[1];
    } else {
        return _a[0] - _b[0];
    }  
}


int startswith(const char *prefix, const char *string)
{
    return strncmp(prefix, string, strlen(prefix)) == 0;
}

int get_dataset_size(FILE *file, int *n_nodes, int* n_edges) {
    
    char *readed_line = NULL;
    ssize_t n_char_readed = 0;
    size_t len = 0;
    int found_dataset_size = NOT_FOUND;

    while((n_char_readed = getline(&readed_line, &len, file)) != EOF) {
        if (n_char_readed > 0) {
            if (readed_line[0] != '#') {
                return found_dataset_size;
            } else {
                if (startswith("# Nodes:", readed_line)) {
                    if (sscanf( readed_line, "# Nodes: %d Edges: %d", n_nodes, n_edges ) != EOF) {
                        found_dataset_size = FOUND;
                        free(readed_line);
                        readed_line = NULL;
                        len = 0;
                        if ((n_char_readed = getline(&readed_line, &len, file)) == EOF) {
                            return STATUS_ERR;
                        }
                        break;
                    }
                }
            }
        }
        
        n_char_readed = 0;
        free(readed_line);
        readed_line = NULL;
    
    }
    return found_dataset_size;
}

int get_dataset_entry(FILE *file, int *to_node_id, int *from_node_id) {
    char *readed_line = NULL;
    ssize_t n_char_readed = 0;
    size_t len = 0;
    if ((n_char_readed = getline(&readed_line, &len, file) != EOF)) {
        if (sscanf( readed_line, " %d  %d", from_node_id, to_node_id) != EOF) {
            return STATUS_OK;
        } else {
            return STATUS_ERR;
        }
    }   
    return EOF;
}

int get_dataset_entries(FILE *file, int*** DATASET, int n_edges) {
    int to_node_id;
    int from_node_id;
    int status;
    int i;
    int **_DATASET;

    i = 0;
    _DATASET = *DATASET;

    while((status = get_dataset_entry(file, &to_node_id, &from_node_id)) == STATUS_OK || i < n_edges) {

        _DATASET[i] = (int*)malloc(sizeof(int)*2);
        if (_DATASET[i] != NULL) {
            _DATASET[i][0] = to_node_id;
            _DATASET[i][1] = from_node_id;
        } else {
            return STATUS_ERR;
        }
        i++;
    }
    if (n_edges != i -1) {
        printf("[ERR] Number of edges mismatch: Header says %d  but in file there are %d edges\n", n_edges, i);
        return STATUS_ERR;
    }
    if (status != EOF) {
        return STATUS_ERR;
    }
    return STATUS_OK;
}

int read_file(char *file_path, int*** DATASET, int *n_nodes, int* n_edges) {
    FILE *f;

    f = fopen(file_path,"r");

    if (f == NULL) {
        printf("[ERR] Fail opening the file.\n");
        return STATUS_ERR;
    }

    if (get_dataset_size(f, n_nodes, n_edges) == NOT_FOUND) {
        printf("[ERR] Fail getting dataset size.\n");
        return STATUS_ERR;
    }
    // initialize DATASET
    *DATASET = (int**)malloc(sizeof(int*)*(*n_edges));

    get_dataset_entries(f, DATASET, *n_edges);

    if (fclose(f) == EOF) {
        printf("[ERR] Error closing the file.\n");
        return STATUS_ERR;
    }
    return STATUS_OK;
}

int main() {
    int** DATASET;
    int n_edges, n_nodes;
    int status;
    int i;

    status = read_file("data/web-NotreDame.txt", &DATASET, &n_nodes, &n_edges);
    
    if (status == STATUS_ERR) {
        printf("[ERR] Error reading file.\n");
        return STATUS_ERR;
    }

    qsort(DATASET, n_edges, sizeof(DATASET[0]), compare_dataset_entries);
    for (i = 0; i < n_edges; i++) {
        printf("\t%d\t%d\n", DATASET[i][0], DATASET[i][1]);
    }
    
    printf("[OK] Success.\n");
    return 0;
}
