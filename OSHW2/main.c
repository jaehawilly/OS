// #include "Disk.h"
// #include "buf.h"
// #include <stdio.h>
// #include <stdlib.h>

// void printLRUList() {
//     printf("\n------------[LRU list]---------------\n");
//     Buf *item;
//     TAILQ_FOREACH(item, &pLruListHead, llist) {
//         printf("blkno[%d]: %s\n", item->blkno, (char *)item->pMem);
//     }
//     printf("\n");
// }

// void printStateList() {
//     printf("\n------------[CLEAN list]---------------\n");
//     Buf *item;
//     TAILQ_FOREACH(item, &ppStateListHead[BUF_LIST_CLEAN], slist) {
//         printf("blkno[%d]: %s\n", item->blkno, (char *)item->pMem);
//     }
//     printf("\n");
//     printf("\n------------[DIRTY list]---------------\n");
//     TAILQ_FOREACH(item, &ppStateListHead[BUF_LIST_DIRTY], slist) {
//         printf("blkno[%d]: %s\n", item->blkno, (char *)item->pMem);
//     }
// }

// void printBufList() {
//     printf("\n-------------[BUF list]----------------\n");
//     Buf *item;
//     for (int i = 0; i < HASH_ENTRY_NUM; i++) {
//         printf("Hash[%d] -> ", i);
//         TAILQ_FOREACH(item, &ppBufList[i], blist) {
//             printf("blkno[%d]: %s\n", item->blkno, (char *)item->pMem);
//         }
//         printf("\n");
//     }
//     printf("\n");
// }

// void printDisk() {
//     printf("\n---------------[DISK]------------------\n");

//     for (int i = 0; i < 10; i++) {
//         char *pMem = malloc(BLOCK_SIZE);
//         DevReadBlock(i, pMem);
//         printf("blkno[%d]: %s\n", i, pMem);
//     }
//     printf("\n");
// }

// int main(void) {
//     DevCreateDisk();
//     DevOpenDisk();

//     char pData[BLOCK_SIZE] = "v";
//     BufInit();
//     BufWrite(0, pData);
//     BufWrite(1, pData);
//     BufWrite(2, pData);
//     BufWrite(3, pData);
//     BufWrite(4, pData);
//     BufWrite(5, pData);
//     BufWrite(6, pData);
//     printLRUList();
//     printStateList();
//     printBufList();
//     printDisk();

//     printf("\n------------------[AFTER]-------------------\n");

//     BufRead(0, pData);
//     BufRead(1, pData);
//     BufRead(2, pData);
//     printLRUList();
//     printStateList();
//     printBufList();
//     printDisk();
//     return 0;
// }
