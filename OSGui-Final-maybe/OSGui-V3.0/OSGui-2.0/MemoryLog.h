#ifndef MEMORYLOG_H
#define MEMORYLOG_H
/**
* MyOS\MemoryLog.h
*
* last modification time : 2016/4/28 17:03		by LiangJie
*/

/**
* Working log:
* 22016/4/28 17:03		Created				by LiangJie ;
*/

/*
    添加一个日志
    哪个进程的第几页替换了几号帧上的哪个进程的第几页
*/
#define MEM_LOG_COUNT 100

typedef struct MemoryFrameFault{
    int pageOld;
    int pidOld;
    int pageNew;
    int pidNew;
    int frameNum;
    struct MemoryFrameFault *next;
}MemoryFrameFault;

void iniMFF();
void addMFF(int __pageOld, int __pidOld, int __pageNew, int __pidNew, int __frameNum);
















#endif // MEMORYLOG_H

