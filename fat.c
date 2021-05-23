#include "fat.h"
#include "Disk.h"
#include <stdio.h>
#include <stdlib.h>
#define ENTRY_PER_BLOCK (BLOCK_SIZE / 4)

void FatInit(void) {
    int *pMem = (int *)calloc(ENTRY_PER_BLOCK, sizeof(int));
    DevCreateDisk();
    DevOpenDisk();
    for (int i = 0; i < NUM_FAT_BLOCK; i++) {
        DevWriteBlock(i, (char *)pMem);
    }
    free(pMem);
}

/* newBlkNum이 지정하는 FAT entry의 value가 0이 아니면 -1을 리턴함.
   lastBlkNum이 지정하는 FAT entry의 값이 -1이 아니면 -1을 리턴함. */
int FatAdd(int lastBlkNum, int newBlkNum) {
    int *pMem = (int *)malloc(ENTRY_PER_BLOCK * sizeof(int));
    DevReadBlock(newBlkNum / ENTRY_PER_BLOCK, (char *)pMem);
    if (*(pMem + newBlkNum % ENTRY_PER_BLOCK) != 0) {
        free(pMem);
        return -1;
    }

    if (lastBlkNum == -1) {
        int *tempMem = (int *)calloc(ENTRY_PER_BLOCK, sizeof(int));
        int blockNum = newBlkNum / ENTRY_PER_BLOCK;
        DevReadBlock(blockNum, (char *)tempMem);
        *(tempMem + newBlkNum % ENTRY_PER_BLOCK) = -1;
        DevWriteBlock(blockNum, (char *)tempMem);
        free(tempMem);
        return 0;
    } else {
        DevReadBlock(lastBlkNum / ENTRY_PER_BLOCK, (char *)pMem);
        if (*(pMem + lastBlkNum % ENTRY_PER_BLOCK) != -1) {
            free(pMem);
            return -1;
        }
        DevReadBlock(lastBlkNum / ENTRY_PER_BLOCK, (char *)pMem);
        int v = *(pMem + lastBlkNum % ENTRY_PER_BLOCK);
        *(pMem + lastBlkNum % ENTRY_PER_BLOCK) = newBlkNum;
        DevWriteBlock(lastBlkNum / ENTRY_PER_BLOCK, (char *)pMem);

        DevReadBlock(newBlkNum / ENTRY_PER_BLOCK, (char *)pMem);
        *(pMem + newBlkNum % ENTRY_PER_BLOCK) = -1;
        DevWriteBlock(newBlkNum / ENTRY_PER_BLOCK, (char *)pMem);
        return 0;
    }
}

/* firstBlkNum이 지정하는 FAT entry의 value가 0이거나
   logicalBlkNum에 대응하는 physical block 번호가 -1이거나 0인 경우, -1을 리턴함
 */
int FatGetBlockNum(int firstBlkNum, int logicalBlkNum) {
    int *tempMem = (int *)calloc(ENTRY_PER_BLOCK, sizeof(int));
    int EntryValue = 0;
    int blockNum = 0;
    for (int i = 0; i < logicalBlkNum; i++) {
        if (i == 0) {
            EntryValue = firstBlkNum;
            blockNum = firstBlkNum / ENTRY_PER_BLOCK;
            DevReadBlock(EntryValue / ENTRY_PER_BLOCK, (char *)tempMem);
            EntryValue = *(tempMem + EntryValue % ENTRY_PER_BLOCK);
            if (EntryValue == 0)
                return -1;
        } else {
            DevReadBlock(EntryValue / ENTRY_PER_BLOCK, (char *)tempMem);
            EntryValue = *(tempMem + EntryValue % ENTRY_PER_BLOCK);
            if (EntryValue == -1 || EntryValue == 0)
                return -1;
        }
    }
    return EntryValue;
}

/* firstBlkNum이 지정하는 FAT entry의 value가 0이거나
   startBlkNm이 지정하는 FAT entry의 value가 0인 경우, -1을 리턴함.*/

int FatRemove(int firstBlkNum, int startBlkNum) {
    int *tempMem = (int *)calloc(ENTRY_PER_BLOCK, sizeof(int));
    int EntryValue = firstBlkNum;
    DevReadBlock(firstBlkNum / ENTRY_PER_BLOCK, (char *)tempMem);
    if (*(tempMem + firstBlkNum % ENTRY_PER_BLOCK) == 0)
        return -1;
    DevReadBlock(startBlkNum / ENTRY_PER_BLOCK, (char *)tempMem);
    if (*(tempMem + startBlkNum % ENTRY_PER_BLOCK) == 0) {
        return -1;
    }
    while (1) {
        DevReadBlock(EntryValue / ENTRY_PER_BLOCK, (char *)tempMem);
        int v = *(tempMem + EntryValue % ENTRY_PER_BLOCK);
        if (v == startBlkNum) {
            *(tempMem + EntryValue % ENTRY_PER_BLOCK) = -1;
            DevWriteBlock(EntryValue / ENTRY_PER_BLOCK, (char *)tempMem);
            EntryValue = v;
            break;
        } else {
            EntryValue = v;
        }
    }
    int nextEntry = EntryValue;
    while (1) {
        EntryValue = nextEntry;
        DevReadBlock(EntryValue / ENTRY_PER_BLOCK, (char *)tempMem);
        nextEntry = *(tempMem + EntryValue % ENTRY_PER_BLOCK);
        *(tempMem + EntryValue % ENTRY_PER_BLOCK) = 0;
        DevWriteBlock(EntryValue / ENTRY_PER_BLOCK, (char *)tempMem);
        if (nextEntry == -1) {
            break;
        }
    }
    return 0;
}
