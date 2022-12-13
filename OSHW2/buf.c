#include "buf.h"
#include "Disk.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void BufInit(void) {
    DevCreateDisk();
    DevOpenDisk();
    for (int i = 0; i < HASH_ENTRY_NUM; i++) {
        TAILQ_INIT(&ppBufList[i]);
    }
    TAILQ_INIT(&ppStateListHead[BUF_LIST_DIRTY]);
    TAILQ_INIT(&ppStateListHead[BUF_LIST_CLEAN]);
    TAILQ_INIT(&pLruListHead);
}

void BufRead(int blkno, char *pData) {
    Buf *item = BufGet(blkno);
    Buf *temp;
    if (item != NULL) {
        memcpy(pData, item->pMem, BLOCK_SIZE);
        return;
    } else {
        int cnt = 0;
        for (int i = 0; i < HASH_ENTRY_NUM; i++) {
            TAILQ_FOREACH(temp, &ppBufList[i], blist) { cnt++; }
        }
        if (cnt >= MAX_BUF_NUM) {
            temp = TAILQ_FIRST(&pLruListHead);
            TAILQ_REMOVE(&ppBufList[temp->blkno % HASH_ENTRY_NUM], temp, blist);
            TAILQ_REMOVE(&ppStateListHead[temp->state], temp, slist);
            TAILQ_REMOVE(&pLruListHead, temp, llist);
            if (temp->state == BUF_STATE_DIRTY) {
                DevWriteBlock(temp->blkno, temp->pMem);
                temp->state = BUF_STATE_CLEAN;
            }
            DevReadBlock(blkno, temp->pMem);
            memcpy(pData, temp->pMem, BLOCK_SIZE);
            temp->blkno = blkno;
            TAILQ_INSERT_TAIL(&ppStateListHead[temp->state], temp, slist);
            TAILQ_INSERT_HEAD(&ppBufList[temp->blkno % HASH_ENTRY_NUM], temp,
                              blist);
            TAILQ_INSERT_TAIL(&pLruListHead, temp, llist);
        } else {
            item = malloc(sizeof(Buf));
            item->pMem = malloc(BLOCK_SIZE);
            DevReadBlock(blkno, item->pMem);
            memcpy(pData, item->pMem, BLOCK_SIZE);
            item->state = BUF_STATE_CLEAN;
            item->blkno = blkno;
            TAILQ_INSERT_TAIL(&ppStateListHead[item->state], item, slist);
            TAILQ_INSERT_HEAD(&ppBufList[blkno % HASH_ENTRY_NUM], item, blist);
            TAILQ_INSERT_TAIL(&pLruListHead, item, llist);
        }
    }
}

void BufWrite(int blkno, char *pData) {
    Buf *item = BufGet(blkno);
    Buf *temp;
    if (item != NULL) {
        memcpy(item->pMem, pData, BLOCK_SIZE);
        if (item->state == BUF_LIST_CLEAN) {
            TAILQ_REMOVE(&ppStateListHead[BUF_LIST_CLEAN], item, slist);
            item->state = BUF_STATE_DIRTY;
            TAILQ_INSERT_TAIL(&ppStateListHead[BUF_LIST_DIRTY], item, slist);
            TAILQ_REMOVE(&pLruListHead, item, llist);
            TAILQ_INSERT_TAIL(&pLruListHead, item, llist);
        }
    } else {
        int cnt = 0;
        for (int i = 0; i < HASH_ENTRY_NUM; i++) {
            TAILQ_FOREACH(temp, &ppBufList[i], blist) { cnt++; }
        }
        if (cnt >= MAX_BUF_NUM) {
            temp = TAILQ_FIRST(&pLruListHead);
            TAILQ_REMOVE(&ppBufList[temp->blkno % HASH_ENTRY_NUM], temp, blist);
            TAILQ_REMOVE(&ppStateListHead[temp->state], temp, slist);
            TAILQ_REMOVE(&pLruListHead, temp, llist);
            if (temp->state == BUF_LIST_DIRTY) {
                DevWriteBlock(temp->blkno, temp->pMem);
            }
        }
        temp = malloc(sizeof(Buf));
        temp->pMem = malloc(BLOCK_SIZE);

        memcpy(temp->pMem, pData, BLOCK_SIZE);
        temp->blkno = blkno;
        temp->state = BUF_STATE_DIRTY;
        TAILQ_INSERT_TAIL(&ppStateListHead[BUF_LIST_DIRTY], temp, slist);
        TAILQ_INSERT_HEAD(&ppBufList[blkno % HASH_ENTRY_NUM], temp, blist);
        TAILQ_INSERT_TAIL(&pLruListHead, temp, llist);
    }
}

void BufSync(void) {
    Buf *temp;
    for (int i = 0; i < HASH_ENTRY_NUM; i++) {
        TAILQ_FOREACH(temp, &ppBufList[i], blist) {
            if (temp->state == BUF_STATE_DIRTY) {
                DevWriteBlock(temp->blkno, temp->pMem);
                temp->state = BUF_STATE_CLEAN;
                TAILQ_REMOVE(&ppStateListHead[BUF_LIST_DIRTY], temp, slist);
                TAILQ_INSERT_TAIL(&ppStateListHead[BUF_LIST_CLEAN], temp,
                                  slist);
            }
        }
    }
}

Buf *BufGet(int blkno) {
    Buf *item;
    TAILQ_FOREACH(item, &ppBufList[blkno % HASH_ENTRY_NUM], blist) {
        if (item->blkno == blkno) {
            return item;
        }
    }
    return NULL;
}
void BufSyncBlock(int blkno) {
    Buf *temp;
    TAILQ_FOREACH(temp, &ppBufList[blkno % HASH_ENTRY_NUM], blist) {
        if (temp->state == BUF_STATE_DIRTY && temp->blkno == blkno) {
            DevWriteBlock(temp->blkno, temp->pMem);
            temp->state = BUF_STATE_CLEAN;
            TAILQ_REMOVE(&ppStateListHead[BUF_LIST_DIRTY], temp, slist);
            TAILQ_INSERT_TAIL(&ppStateListHead[BUF_LIST_CLEAN], temp, slist);
        }
    }
}

/*
 * GetBufInfoByListNum: Get all buffers in a list specified by listnum.
 *                      This function receives a memory pointer to "ppBufInfo"
 * that can contain the buffers.
 */
void GetBufInfoByListNum(StateList listnum, Buf **ppBufInfo, int *pNumBuf) {
    Buf *temp;
    int i = 0;
    *pNumBuf = 0;
    TAILQ_FOREACH(temp, &ppStateListHead[listnum], slist) {
        ppBufInfo[i++] = temp;
        (*pNumBuf)++;
    }
}
/*
 * GetBufInfoInLruList: Get all buffers in a list specified at the LRU list.
 *                         This function receives a memory pointer to
 * "ppBufInfo" that can contain the buffers.
 */
void GetBufInfoInLruList(Buf **ppBufInfo, int *pNumBuf) {
    Buf *temp;
    int i = 0;
    *pNumBuf = 0;
    TAILQ_FOREACH(temp, &pLruListHead, llist) {
        ppBufInfo[i++] = temp;
        (*pNumBuf)++;
    }
}

/*
 * GetBufInfoInBufferList: Get all buffers in the buffer list.
 *                         This function receives a memory pointer to
 * "ppBufInfo" that can contain the buffers.
 */
void GetBufInfoInBufferList(int hashEntNum, Buf **ppBufInfo, int *pNumBuf) {
    Buf *temp;
    int i = 0;
    *pNumBuf = 0;
    TAILQ_FOREACH(temp, &ppBufList[hashEntNum], blist) {
        ppBufInfo[i++] = temp;
        (*pNumBuf)++;
    }
}
