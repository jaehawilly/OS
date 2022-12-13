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

/* newBlkNum�� �����ϴ� FAT entry�� value�� 0�� �ƴϸ� -1�� ������.
   lastBlkNum�� �����ϴ� FAT entry�� ���� -1�� �ƴϸ� -1�� ������. */
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

/* firstBlkNum�� �����ϴ� FAT entry�� value�� 0�̰ų�
   logicalBlkNum�� �����ϴ� physical block ��ȣ�� -1�̰ų� 0�� ���, -1�� ������
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

/* firstBlkNum�� �����ϴ� FAT entry�� value�� 0�̰ų�
   startBlkNm�� �����ϴ� FAT entry�� value�� 0�� ���, -1�� ������.*/

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
