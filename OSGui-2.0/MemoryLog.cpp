/**
* MyOS\MemoryLog.cpp
*
* last modification time : 2016/4/28 17:03		by LiangJie
*/

/**
* Working log:
* 2016/4/28 17:03		Created				by LiangJie ;
* 2016/4/29 16:03		iniMFF()		addMFF()		by LiangJie ;
*/

/*
    添加一个日志
    哪个进程的第几页替换了几号帧上的哪个进程的第几页
*/
#include "MemoryLog.h"
#include <stdio.h>
#include <stdlib.h>


MemoryFrameFault *MFFhead, *MFFlast;
int MemLogCount;

void iniMFF() {
    MFFhead = (MemoryFrameFault *)malloc(sizeof(MemoryFrameFault));
    MFFlast = MFFhead;
    MemLogCount = 0;
}

void addMFF(int __pageOld, int __pidOld, int __pageNew, int __pidNew, int __frameNum) {
    MemoryFrameFault *tempPtr;
    tempPtr = (MemoryFrameFault *)malloc(sizeof(MemoryFrameFault));
    tempPtr->pageOld = __pageOld;
    tempPtr->pidOld = __pidOld;
    tempPtr->pageNew = __pageNew;
    tempPtr->pidNew = __pidNew;
    tempPtr->frameNum = __frameNum;

    tempPtr->next = NULL;
    MFFlast->next = tempPtr;
    MFFlast = tempPtr;
    MemLogCount++;
    if(MemLogCount >= MEM_LOG_COUNT) {
        MemoryFrameFault *temp = MFFhead;
        MFFhead = MFFhead->next;
        free(temp);
    }

}


