#include "Disk.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int fd;
int diskReadCount = 0;
int diskWriteCount = 0;

void DevResetDiskAccessCount(void) { diskReadCount = diskWriteCount = 0; }

int DevGetDiskReadCount(void) { return diskReadCount; }

int DevGetDiskWriteCount(void) { return diskWriteCount; }

void DevCreateDisk(void) {
    fd = open("MY_DISK", O_RDWR | O_CREAT | O_TRUNC, 0644);
}

void DevOpenDisk(void) { fd = open("MY_DISK", O_RDWR); }

void __DevMoveBlock(int blkno) {
    lseek(fd, (off_t) + (BLOCK_SIZE * blkno), SEEK_SET);
}

void DevReadBlock(int blkno, char *pBuf) {
    __DevMoveBlock(blkno);
    read(fd, pBuf, BLOCK_SIZE);
    diskReadCount++;
}

void DevWriteBlock(int blkno, char *pBuf) {
    __DevMoveBlock(blkno);
    write(fd, pBuf, BLOCK_SIZE);
    diskWriteCount++;
}
