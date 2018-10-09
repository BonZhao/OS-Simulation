/**
* MyOS/MemoryManagement.c
*
* Copyright (c) 2016 LiangJie
*/

/**
* Working log
* 2016/03/17 20:00     Created by Liangjie
*
* 2016/03/21 20:30
* 2016/03/23 19:58    bool initializeMemory()
* 2016/03/24 20:00
* 2016/03/25 14:20    int giveAPage(int __type) void FIFOUse(int __frameNum, bool isFrameAvailable);
                                                bool withDrawFIFO(int __frameNum);
                                                int FIFOPageReplace();
                                                void LRUUse(int __frameNum);
                                                bool withDrawLRU(int __frameNum);
                                                int LRUPageReplace();
                                                void showFrameTableLRU();
                                                void showFrameTableFIFO();
* 2016/04/03 17:20		by	LiangJie
*		-Correct readMemory and pageFault operation.
* 2016/04/23 10:16  countEmptyFrame 计算空闲帧的个数
*                   double calMemUtilization()
*  2016/4/29 16:03		add MemoryLogGui		by LiangJie ;
*  2016/5/10   add workLog
* by LiangJie
*/

/**
* 该模块维护一个帧表
* 可配置帧大小信息
* 分配内存
* 收回内存
* 搬移虚拟内存中的一帧到内存中，同时更改帧表
* 选择牺牲页函数
*
*
* 定义页表
*
*
* 地址变换<------交给进程管理去做
*/

#include <stdio.h>
#include <stdlib.h>
#include "ProcessManagement.h"
#include "MemoryManagement.h"
#include "MemoryDriver.h"
#include "VirtualMemory.h"
#include "DiskDriver.h"
#include "Basic.h"
#include "WorkLog.h"
#include "MemoryLog.h"
extern int memorySize;                                          /* 外部变量 表示内存大小，由readMemoryConfig()读出*/
extern const char * memoryPath;

int frameSize = 1024;                                            /* 帧大小为1024B*/
int frameCount = 2;                                              /*    帧个数    */
Frame * frameTable;
const char * framePath = "PC\\Config\\";

int FIFOCount = 0;
int pageType = 0 ;
extern MemoryFrameFault *MFFhead, *MFFlast;

extern  const char * errorDesc[100];
/**
* [readMFConfig description]  读取帧的大小
* @return [description]
*/
bool readMFConfig()
{
    extern int memorySize;
    /* 配置文件路径 */
    char * MFConfigPath__ = (char *)malloc(sizeof(char) * 38);
    sprintf(MFConfigPath__, "%sMFConfig.txt", framePath);

    char act[] = "操作:读取内存配置信息";
    /* 打开配置文件，失败则返回false */
    FILE * ifp__ = fopen(MFConfigPath__, "r");
    if (ifp__ == NULL) {
        setError(MEMORYMANAGEMENT_CONFIG_NOT_EXIST);
        logPrintf(MEMORY_LOG, "%s\t%s\t%s%s\n\n", act, "操作结果:读取失败", "失败原因:", errorDesc[MEMORYMANAGEMENT_CONFIG_NOT_EXIST]);
        return false;
    }
    if (fscanf(ifp__, "%d", &frameSize) != 1) {
        setError(MEMORYMANAGEMENT_CONFIG_FORM_ERROR);
        logPrintf(MEMORY_LOG, "%s\t%s\t%s%s\n\n", act, "操作结果:读取失败", "失败原因:", errorDesc[MEMORYMANAGEMENT_CONFIG_FORM_ERROR]);
        return false;
    }

    frameCount = memorySize / frameSize;        /* 求出帧的个数 */
    frameTable = (Frame*)malloc(sizeof(Frame) * frameCount);   /* 建立帧表 */


    fclose(ifp__);
    free(MFConfigPath__);


    logPrintf(MEMORY_LOG, "%s\t%s\t%s%d\t%s%d\n\n", act, "操作结果:读取成功", "帧大小为:", frameSize, "帧个数为:", frameCount);
    /* 读取成功 */
    return true;
}
/**
* 一般不推荐使用
* [saveVMConfig 保存内存配置信息，即帧大小]
* @return [description]
*/
bool saveMFConfig() {

    /* 配置文件路径 */
    char * MFConfigPath__ = (char *)malloc(sizeof(char) * 38);
    sprintf(MFConfigPath__, "%sMFConfig.txt", framePath);

    /* 打开配置文件，失败则返回false */
    FILE * ofp__ = fopen(MFConfigPath__, "w");
    if (ofp__ == NULL) {
        setError(MEMORYMANAGEMENT_CONFIG_NOT_EXIST);
        return false;
    }
    fprintf(ofp__, "%d\n", frameSize);
    fclose(ofp__);
    free(MFConfigPath__);

    /* 写入成功 */
    return true;
}

/**
* 该函数重置帧表,在readMFConfig后才可以执行
*
*/
void resetFrameTable() {
    int i__;
    for (i__ = 0; i__ < frameCount; i__++) {
        frameTable[i__].isAvailable = false;
        frameTable[i__].FIFOcount = -1;
        frameTable[i__].LRUcount = -1;
        frameTable[i__].isDirty = false;
    }
}

/**
* [initializeMemory 初始化操作系统内存]
* @return [成功返回true]
* 请首先调用此函数
*/
bool initializeMemory() {
    /* 初始化 内存硬件 */
    if (!initializeMemoryHard()) {
        return false;
    }
    /* 初始化内存配置   */
    if (!readMFConfig()) {
        return false;
    }
    /* 初始化虚存      */
    if (!initializeVM()) {
        return false;
    }
    resetFrameTable();                                       /* 重置帧表 */
    return true;
}

/**
* [showFrameTable 展示帧表]
*/
void showFrameTable() {
    int i__;
    printf("%s\n", "                          FRAME TABLE");
    printf("%10s|%15s|%12s|%12s\n", "frame num", "is available", "PID", "page num");
    for (i__ = 0; i__ < frameCount; i__++) {
        printf("%10d|", i__);

        if (frameTable[i__].isAvailable == true) {
            printf("%15s|", "true");
            printf("%12d|", frameTable[i__].PID);
            printf("%12d\n", frameTable[i__].pageNum);

        }
        else {
            printf("%15s|%12s|%12s\n", "false", "null", "null");
        }
    }//for
    printf("\n");
}

void showFrameTableFIFO() {
    int i__;
    printf("%s\n", "                          FRAME TABLE");
    printf("%10s|%15s|%12s|%12s|%12s\n", "frame num", "is available", "PID", "page num", "FIFOCount");
    for (i__ = 0; i__ < frameCount; i__++) {
        printf("%10d|", i__);

        if (frameTable[i__].isAvailable == true) {
            printf("%15s|", "true");
            printf("%12d|", frameTable[i__].PID);
            printf("%12d|", frameTable[i__].pageNum);
            printf("%12d\n", frameTable[i__].FIFOcount);

        }
        else {
            printf("%15s|%12s|%12s|%12s\n", "false", "null", "null", "null");
        }
    }//for
    printf("\n");
}

void showFrameTableLRU() {
    int i__;
    printf("%s\n", "                          FRAME TABLE");
    printf("%10s|%15s|%12s|%12s|%12s\n", "frame num", "is available", "PID", "page num", "LRUCount");
    for (i__ = 0; i__ < frameCount; i__++) {
        printf("%10d|", i__);

        if (frameTable[i__].isAvailable == true) {
            printf("%15s|", "true");
            printf("%12d|", frameTable[i__].PID);
            printf("%12d|", frameTable[i__].pageNum);
            printf("%12d\n", frameTable[i__].LRUcount);

        }
        else {
            printf("%15s|%12s|%12s|%12s\n", "false", "null", "null", "null");
        }
    }//for
    printf("\n");
}

/**
* [showPageTable 显示页表]
* @param __p [被显示的页表]
*/
void showPageTable(PageTable __p, int pageCount__) {
    int i__;
    printf("%s\n", "                          PAGE TABLE");
    printf("%10s|%15s|%12s|%12s\n", "page num", "is available", "PID", "frame num");
    for (i__ = 0; i__ < pageCount__; i__++) {
        printf("%10d|", i__);

        if (__p[i__].isAvailable == true) {
            printf("%15s|", "true");
            printf("%12d|", __p[i__].PID);
            printf("%12d\n", __p[i__].frameNum);

        }
        else {
            printf("%15s|%12s|%12s\n", "false", "null", "null");
        }
    }//for
    printf("\n");
}

/**
* [allocateMemory 分配内存]
* @param  __PID       [进程号]
* @param  __frameNeed [进程申请的内存数]
* @param  __hasUsed   [进程已用的内存数]
* @return             [成功为true]
*/
bool allocateMemory(int __PID, int __frameNeed, int __hasUsed) {
    int maxCanUse = VMframeAllocateCount(__PID);    /* 求出该进程最大可用内存量 */
    if (__frameNeed + __hasUsed < maxCanUse) {
        return true;
    }
    else {
        setError(MEMORY_APPLY_TOO_MUCH);
        return false;
    }

}

/**
* [resetPageTable 初始化页表]
* @param __PID            [进程号]
* @param __pageApplyCount [申请页数]
* @param __pageTable      [原页表]
*/
void resetPageTable(int __PID, int __pageApplyCount, PageTable __pageTable) {
    int i__ = 0;
    for (i__ = 0; i__ <__pageApplyCount; i__++){           //初始化页表
        __pageTable[i__].isAvailable = false;
        __pageTable[i__].frameNum = -1;
        __pageTable[i__].PID = __PID;
    }
}

/**
* [withdrawMemory 释放内存函数]
* @param __PID [进程号]
* @param __type[调页类型]
*/
void withdrawMemory(int __PID) {
    int i__;
    for (i__ = 0; i__ < frameCount; i__++) {
        /*保证此进程的帧在内存里才可收回*/
        if (frameTable[i__].PID == __PID && frameTable[i__].isAvailable == true) {
            if (pageType == 0) {
                /* FIFO收回帧 */
                withDrawFIFO(i__);
                logPrintf(MEMORY_LOG, "%s\t%s%d\t%s\t%s\n\n", "操作:收回内存", "进程号:", __PID,"调页类型:","FIFO");
                }

            else {
                /* LRU收回帧 */
                withDrawLRU(i__);
                logPrintf(MEMORY_LOG, "%s\t%s%d\t%s\t%s\n\n", "操作:收回内存", "进程号:", __PID, "调页类型:", "LRU");
            }
            /* 将有效位设置为无效*/
            frameTable[i__].isAvailable = false;

        }//if
    }
    /*收回对应的虚存*/
    withdrawVM(__PID);
}

/**
* [memoryAddressMapping 寻址函数]
* @param  __pageTable  [页号]
* @param  __pageNumber [页数]
* @param  __pageOffset [页偏移]
* @return              [物理地址，无效地址返回-1]
*/
int  memoryAddressMapping(Page* __pageTable, int __pageNumber, int __pageOffset){

    int physicsAddress__ = 0;
    if (__pageTable[__pageNumber].isAvailable == true){   //根据页号找到帧号，并转换地址
        physicsAddress__ = __pageTable[__pageNumber].frameNum * frameSize + __pageOffset;
        return physicsAddress__;   // 返回物理地址
    }
    else
        return -1;   //-1代表缺页
}


/**
* [SacrificingPageSelection 寻找调入页]
* @param  __type [方式]
* @return        [返回调入页]
*/
int giveAPage() {
    /*  首先寻找有没有空帧 */
    int i__;
    for (i__ = 0; i__ < frameCount; i__++) {
        if (frameTable[i__].isAvailable == false) {
            addPageErrorLog(i__, 1);
            return i__;
        }
    }
    int frameToReplace__;

    /*没找到空页，则选择牺牲页*/
    if (pageType == 0) {
        frameToReplace__ = FIFOPageReplace();
        logPrintf(MEMORY_LOG, "%s\t%s%d\t%s\t%s\n\n", "操作:寻找被调入帧", "被调入帧号:", frameToReplace__,"调页类型:", "FIFO");
    }
    else {
        frameToReplace__ = LRUPageReplace();
        logPrintf(MEMORY_LOG, "%s\t%s%d\t%s\t%s\n\n", "操作:寻找被调入帧", "被调入帧号:", frameToReplace__, "调页类型:", "LRU");
    }
    //addPageErrorLog(frameToReplace__, 0);

    return frameToReplace__;
}


/**
* [VMToMemory 把一个虚拟帧搬到内存帧中]
* @param  __type   [调页方式]
* @param  __PID        [进程号]
* @param  __pageNum	[进程对应的页号]
* @return              [帧号]
*/
int VMToMemory(int __PID, int __pageNum) {
    int frameNum__ = giveAPage();

    int VMFrameNum__ = findVMFrameToMemory(__PID, __pageNum);
    //printf("%d %d\n", frameNum__, VMFrameNum__);
    /* 保证虚存的帧为有效的才可进行 */
    if (!isVMFrameNumAvailable(VMFrameNum__)) {
        setError(VM_FRAMENUM_NOT_AVAILABLE);
        return -1;
    }

    if(frameTable[frameNum__].isDirty == true) {
        MemoryToVM(frameNum__);
    }

    char * section__ = (char *)malloc(sizeof(char) * frameSize);   /* 缓冲区 */
    int sectionNumber__ = VMframeNumToSection(VMFrameNum__);         /* 求出虚拟帧在磁盘上的地址 */
    //printf("sectionNumber is %d\n", sectionNumber__);
    if (sectionNumber__ < 0) {
        setError(VMEMORY_FRAMENUM_MAP_SECTION_NOT_FOUND);
        return false;
    }
    /* 虚存帧 和 磁盘的一个扇区一样大*/
    /*  从磁盘中读出 */
    if (!readSection(section__, sectionNumber__)) {
        return -1;
    }
    /* 写入内存 */
    if (!writeMemoryByFrame(frameNum__ * frameSize, section__)) {
        return -1;
    }

    char log0[] = "操作:虚存帧调入内存";
    char log1__[100];
    sprintf(log1__, "%s%d\t%s%d", "虚存帧号:", VMFrameNum__,"帧号:", frameNum__);

    char log2__[100];
    sprintf(log2__, "%s%d\t%s%d", "新进程PID:", __PID, "占用帧的新进程的页号", __pageNum);

    /*更改被占用帧 的进程页表的信息*/
    if (frameTable[frameNum__].isAvailable == true) {
        int oldPid__ = frameTable[frameNum__].PID;
        int oldPage__ = frameTable[frameNum__].pageNum;
        pageUnvailable(oldPid__, oldPage__);

        char log3__[100];
        sprintf(log3__, "%s%d\t%s%d", "被抢占的进程PID:", oldPid__, "被抢占帧的进程的页号:", oldPage__);

        logPrintf(MEMORY_LOG, "%s\t%s\t%s\t%s\n\n",log0,log1__,log3__,log2__);

        /*OSGui中记录*/
        if(MFFhead == NULL) {
            iniMFF();
        }
        addMFF(oldPage__, oldPid__, __pageNum, __PID, frameNum__);
    }
    else {
        logPrintf(MEMORY_LOG, "%s\t%s\t%s\t\n\n",log0, log1__, log2__);
        if(MFFhead == NULL) {
            iniMFF();
        }
        addMFF(-1, -1, __pageNum, __PID, frameNum__);
    }


    /* 更改帧表的信息 */
    FIFOUse(frameNum__, frameTable[frameNum__].isAvailable);
    LRUUse(frameNum__);

    frameTable[frameNum__].isAvailable = true;

    frameTable[frameNum__].PID = __PID;
    frameTable[frameNum__].pageNum = __pageNum;
    frameTable[frameNum__].isDirty = false;

    return frameNum__;
}

/**
 * [MemoryToVM 把被改变的内存帧存回到虚存里]
 * @param  __frameNum [帧号]
 * @return            [是否成功]
 */
bool MemoryToVM(int __frameNum) {
    int pid__ = frameTable[__frameNum].PID;
    int pageNum__ = frameTable[__frameNum].pageNum;
    int VMFrameNum__ = findVMFrameToMemory(pid__,pageNum__);
    int offset = __frameNum * frameSize;

    if (!isVMFrameNumAvailable(VMFrameNum__)) {
        setError(VM_FRAMENUM_NOT_AVAILABLE);
        return -1;
    }
    char * section__ = (char *)malloc(sizeof(char) * frameSize);   /* 缓冲区 */
    int sectionNumber__ = VMframeNumToSection(VMFrameNum__);         /* 求出虚拟帧在磁盘上的地址 */
    if (sectionNumber__ < 0) {
        setError(VMEMORY_FRAMENUM_MAP_SECTION_NOT_FOUND);
        return false;
    }
    if(!readMemoryByFrame(offset,section__)) {
        return false;
    }
    if(!saveSection(section__, sectionNumber__)) {
        return false;
    }
    frameTable[__frameNum].isDirty = false;
    return true;

}

/**
* [readMemoryByChar 读取字符]
* @param  __pid      [进程号]
* @param  __frameNum [帧号]
* @param  __offsetIn [帧内偏移量]
* @return            [错误返回255]
*/
bool readMemoryByChar(int __pid, int __frameNum, int __offsetIn, unsigned char * __toRead) {
    int offset__;
    //假如该帧没被占用
    char log0[] = "操作:读内存单元";
    if (frameTable[__frameNum].isAvailable == false) {
        setError(FRAME_NOT_AVAILABLE);
        logPrintf(MEMORY_LOG, "%s\t%s\t\n\n", log0,"操作失败","无效帧");
        return false;
    }
    //加入该帧不是被该进程占用
    if (frameTable[__frameNum].PID != __pid) {
        setError(PID_NOT_MAP_FRAME);
        logPrintf(MEMORY_LOG, "%s\t%s\t\n\n", log0, "操作失败", errorDesc[PID_NOT_MAP_FRAME]);
        return false;
    }
    offset__ = __frameNum * frameSize + __offsetIn;
    char log1[150];
    sprintf(log1, "%s\t%s%d\t%s%d\t%s%d\n\n", log0, "进程号:", __pid, "帧号:", __frameNum, "地址:", __offsetIn);
    logPrintf(MEMORY_LOG, "%s",log1);
    bool result__ = readMemoryUnit(offset__, __toRead);
    return result__;

}



/**
 * [writeMemoryByChar 写入字符]
* @param  __pid      [进程号]
 * @param  __frameNum [帧号]
 * @param  __offsetIn [帧内偏移量]
 * @return            [错误返回false]
 */
bool writeMemoryByChar(int __pid, int __frameNum, int __offsetIn, const unsigned char __toSave) {
    int offset__;
    //假如该帧没被占用
    char log0[] = "操作:写内存单元";
    if (frameTable[__frameNum].isAvailable == false) {
        setError(FRAME_NOT_AVAILABLE);
        logPrintf(MEMORY_LOG, "%s\t%s\t\n\n", log0, "操作失败", "无效帧");
        return false;
    }
    //加入该帧不是被该进程占用
    if (frameTable[__frameNum].PID != __pid) {
        setError(PID_NOT_MAP_FRAME);
        logPrintf(MEMORY_LOG, "%s\t%s\t\n\n", log0, "操作失败", errorDesc[PID_NOT_MAP_FRAME]);
        return false;
    }
    offset__ = __frameNum * frameSize + __offsetIn;
    bool result = writeMemoryUnit(offset__, __toSave);
    if(result) {
        frameTable[__frameNum].isDirty = true;
    }
    return result;
}



/**
* [FIFOUse 使用一个帧时对应FIFO处理]
* @param __frameNum [使用的帧]
*/
void FIFOUse(int __frameNum, bool isFrameAvailable) {
    int i__;
    if (!isFrameAvailable) {
        frameTable[__frameNum].FIFOcount = FIFOCount;
        FIFOCount++;
    }
    else {
        for (i__ = 0; i__ < frameCount; i__++) {
            if (frameTable[i__].isAvailable == true ) {
                frameTable[i__].FIFOcount--;
            }
        }
        frameTable[__frameNum].FIFOcount = FIFOCount - 1;
    }
}

/**
* [withDrawFIFO 收回一个帧是FIFO处理]
* @param __frameNum [收回的帧]
*/
bool withDrawFIFO(int __frameNum) {
    /*先检查被收回帧是否是已用帧*/
    if (!frameTable[__frameNum].isAvailable) {
        /*返回 收回未被使用帧错误*/
        setError(FRAME_NOT_AVAILABLE);
        return false;
    }
    int i__;
    for (i__ = 0; i__ < frameCount; i__++) {
        if (frameTable[i__].isAvailable == true && frameTable[i__].FIFOcount > frameTable[__frameNum].FIFOcount) {
            frameTable[i__].FIFOcount--;
        }
    }
    FIFOCount--;
    frameTable[__frameNum].FIFOcount = 0;
    return true;
}
/**
* [FIFOPageReplace 利用FIFO找到调入页]
* @return [返回调入页(是-1发生有空页未被使用错误)]
*/
int FIFOPageReplace() {
    int i__;
    int frameToReplace__ = -1;
    for (i__ = 0; i__ < frameCount; i__++) {
        if (frameTable[i__].isAvailable == true && frameTable[i__].FIFOcount == 0) {
            frameToReplace__ = i__;
            return frameToReplace__;
        }
    }
    return frameToReplace__;
}

/**
* [LRUUse 使用一个帧时对应LRU处理]
* @param __frameNum [使用的帧]
*/
void LRUUse(int __frameNum) {
    int i__;
    /*未被使用的帧LRUcount均加1*/
    for (i__ = 0; i__ < frameCount; i__++) {
        /*在i__帧被使用的情况下*/
        if (frameTable[i__].isAvailable == true) {
            frameTable[i__].LRUcount = frameTable[i__].LRUcount + 1;
        }

    }
    /*被使用的帧LRUcount为0*/
    frameTable[__frameNum].LRUcount = 0;
}

int LRUPageReplace() {
    int i__;
    int frameToReplace__, maxCount;
    frameToReplace__ = -1;
    maxCount = -1;

    /*首先找出LRUcount最大的一个帧为被置换帧*/
    for (i__ = 0; i__ < frameCount; i__++) {
        if (frameTable[i__].isAvailable == true && frameTable[i__].LRUcount > maxCount) {
            frameToReplace__ = i__;
            maxCount = frameTable[i__].LRUcount;
        }
    }
    return frameToReplace__;
}

/**
* [withDrawLRU 收回一个帧是LRU处理]
* @param __frameNum [收回的帧]
*/
bool withDrawLRU(int __frameNum) {

    /*先检查被收回帧是否是已用帧*/
    if (!frameTable[__frameNum].isAvailable) {
        /*返回 收回未被使用帧错误*/
        setError(FRAME_NOT_AVAILABLE);
        return false;
    }

    int i__;
    for (i__ = 0; i__ < frameCount; i__++) {
        /*附加的操作，减少溢出的情况*/
        /*比收回帧LRUcount多的帧的LRUcount  减去  收回帧LRUcount */
        if (frameTable[i__].isAvailable == true && frameTable[i__].LRUcount > frameTable[__frameNum].LRUcount) {
            frameTable[i__].LRUcount--;
        }
    }
    frameTable[__frameNum].LRUcount = -1;
    return false;
}


bool addPageErrorLog(int  __frameNum, int __type) {

    /* 配置文件路径 */
    char * MemoryLogPath__ = (char *)malloc(sizeof(char) * 38);
    sprintf(MemoryLogPath__, "%sLog.txt", memoryPath);

    /* 打开配置文件，失败则返回false */
    FILE * ofp__ = fopen(MemoryLogPath__, "a+");
    if (ofp__ == NULL) {
        setError(MEMORYLOGPATH_NOT_EXIST);
        return false;
    }
    if (__type == 0)
        fprintf(ofp__, "内存帧已满，替换掉第 %d 帧  ",__frameNum);
    else {
        fprintf(ofp__, "页不再内存中，调入到第 %d 帧  ", __frameNum);
    }
    TimeStamp time__;
    getSystemTime(&time__);
    char buff__[40];
    showTime(buff__, time__);
    fprintf(ofp__, "%s\n", buff__);

    fclose(ofp__);
    free(MemoryLogPath__);
/* 写入成功 */
    return true;
}

/**
 * [countusedFrame 计算空闲帧的个数]
 * @return [个数]
 */
int countusedFrame() {
    int i__;
    int usedCount__ = 0;
    for (i__ = 0; i__ < frameCount; i__++) {
        if (frameTable[i__].isAvailable == true) {
            usedCount__++;
        }
    }
    return usedCount__;
}

/**
 * [calMemUtilization 计算内存利用率]
 * @return [利用率]
 */
double calMemUtilization() {
    int usedFrame = countusedFrame();
    double utilization = (double)usedFrame / (double)frameCount;
    return utilization;
}


/* void test() {
    if (!readDiskConfig()) {
        return;
    }
    if (initializeMemory()) {
        puts("YES");
    }
    else {
        printError();
        return;
    }
    PageTable p1 = (PageTable)malloc(sizeof(Page) * 3);
    int needDisk[] = { 5, 20, 25 };
    if (!allocateVM(0, 3, needDisk)) {
        printError();
        return;
    }
    resetPageTable(0, 3, p1);


    PageTable p2 = (PageTable)malloc(sizeof(Page) * 5);
    int needDisk2[] = { 7,8,9,23,50};
    if (!allocateVM(1, 5, needDisk2)) {
        printError();
        return;
    }
    resetPageTable(1, 5, p2);

    int type = 1;
    showVirtualMemoryDetial();

    for (int i = 0; i < 5; i++) {
        if (!VMToMemory(type, 1, i, p2)) {
            printError();

        }
        showFrameTableLRU();
        showPageTable(p2,5);
    }
    for (i = 0; i < 3; i++) {
        if (!VMToMemory(type, 0, i, p1)) {
            printError();

        }
        showFrameTableLRU();
        showPageTable(p1, 3);
    }
    showVirtualMemoryDetial();
} */

/* int main(int argc, char const *argv[])
{
    test();
    system("pause");
    return 0;
} */
