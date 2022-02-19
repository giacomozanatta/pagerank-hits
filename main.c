#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataset.h"
#include "constants.h"



int main() {

    int status;
    int i;
    DATASET dataset;
    status = read_dataset_from_file("data/web-NotreDame.txt", &dataset, TO_NODE_ID_FIRST);
    if (status == STATUS_ERR) {
        printf("[ERR] Error reading file.\n");
        return STATUS_ERR;
    }
    for (i = 0; i < dataset.n_edges; i++) {
        printf("\t%d\t%d\n", dataset.DATA[i][0], dataset.DATA[i][1]);
    }
    destroy_dataset(&dataset);
    printf("[OK] Success.\n");
    return 0;
}
