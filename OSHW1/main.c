#include "fat.h"
#include <stdio.h>
#include <stdlib.h> //이거 있는지 확인

#define ENTRY_PER_BLOCK (BLOCK_SIZE / 4) //이거 있는지 확인
// test
void test(void) {
    int *temp = (int *)malloc(ENTRY_PER_BLOCK * sizeof(int));
    for (int i = 0; i < NUM_FAT_BLOCK; i++) {
        DevReadBlock(i, (char *)temp);
        for (int j = 0; j < ENTRY_PER_BLOCK; j++) {
            printf("%d ", *(temp + j));
        }
        printf("\n");
    }
    free(temp);
}

int main() {
    int blkno;
    FatInit();
    int result2 = FatAdd(-1, 4);
    int result = FatAdd(4, 8);
    int result1 = FatAdd(8, 10);
    int result5 = FatAdd(10, 14);
    int result3 = FatGetBlockNum(4, 3);
    int result6 = FatRemove(4, 10);
    if (result6 != -1)
        test();
    else {
        printf("Error!");
    }
    printf("%d\n", result3);
    return 0;
}
