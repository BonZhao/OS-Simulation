/**
* MyOS/VirtualMemory.c
* Copyright (c) 2016 LiangJie
*/

/**
* Working log
* 2016/03/17 15:00     Created by Liangjie
*                      finish some vital definiations
*                      finish readVMConfig();  saveVMConfig(); resetVMFrameTable();
* 2016/03/17 16:50     finish define Page, PageTable;
* 2016/03/17 17:41     撤销页表，页，
* 2013/03/17 19:10     allocateVM();  showVirtualMemoryDetial();  findVMFrameToMemory();
* 2013/03/20 19:00     函数规范化； VMframeAllocateCount(int __PID)；
* 2013/03/21 20:45     int VMframeNumToSection(int __VMFrameNum);
* 2013/03/23 20:12     isVMFrameNumAvailable(int __VMFrameNum);
* 2016/04/23 10:25     double calVMUtilization()
*  2016/5/10   add workLog
*                      by LiangJie
*/

/**
*  这是虚拟内存模块，可以通过调用驱动程序来实现其功能
*  是第二个层次
*/

#include <stdio.h>
#include <stdlib.h>
#include "VirtualMemory.h"
#include "MemoryManagement.h"
#include "Basic.h"
#include "DiskDriver.h"
#include "WorkLog.h"

/**
* 虚拟内存配置信息，包括
* 虚拟内存所在的扇区位置
* 虚拟内存的大小
* 帧的大小（全局变量）
*/

int VMLocation;                          /*	扇区位置 */
int VMSize;                              /* 虚存大小 */
int VMFrameCount;                        /* 虚拟帧个数 */
int VMAvailableFrameCount;               /* 空闲帧个数 */

/* 虚拟内存配置文件位置 */
const char * VMPath = "PC\\Config\\";
extern int frameSize;
extern int sectionSize ;


/* 虚拟内存帧表 */
VMFrame* VMFrameTable;

extern  const char * errorDesc[100];

/**
* 该函数重置帧表,在readVMConfig后才可以执行
*
*/
void resetVMFrameTable() {
    int i__;
    for (i__ = 0; i__ < VMFrameCount; i__++) {
        VMFrameTable[i__].isAvailable = false;
    }
    VMAvailableFrameCount = VMFrameCount;
}

/**
* [readVMConfig 读取虚拟内存的配置信息]
* 必须在frameSize有意义后才可以执行 (即在readMemoryConfig()后开始执行)
* 只能执行一次
* 必须在该模块所有其他函数执行之前执行
* 需读取扇区位置
* 需读取虚存大小（多少个字节）
* @return 成功读取返回true, 否则false
*/
bool readVMConfig()
{
    /* 配置文件路径 */
    char * VMConfigPath__ = (char *)malloc(sizeof(char) * 38);
    sprintf(VMConfigPath__, "%sVMConfig.txt", VMPath);
    int vmEnd__ ;

    char act[] = "操作:读取虚拟内存配置信息";


    /* 打开配置文件，失败则返回false */
    FILE * ifp__ = fopen(VMConfigPath__, "r");
    if (ifp__ == NULL) {
        setError(VMEMORY_CONFIG_NOT_EXIST);
        logPrintf(VM_LOG, "%s\t%s\t%s%s\n\n", act, "操作结果:读取失败", "失败原因:", errorDesc[VMEMORY_CONFIG_NOT_EXIST]);
        return false;
    }
    if (fscanf(ifp__, "%d %d", &VMLocation, &vmEnd__) != 2) {
        setError(VMEMORY_CONFIG_FORM_ERROR);
        logPrintf(VM_LOG, "%s\t%s\t%s%s\n\n", act, "操作结果:读取失败", "失败原因:", errorDesc[VMEMORY_CONFIG_FORM_ERROR]);
        return false;
    }

    fclose(ifp__);
    free(VMConfigPath__);

    if (frameSize != sectionSize) {
        setError(MEMORYMANAGEMENT_CONFIG_NOT_READ);
        logPrintf(VM_LOG, "%s\t%s\t%s%s\n\n", act, "操作结果:读取失败", "失败原因:", errorDesc[MEMORYMANAGEMENT_CONFIG_NOT_READ]);
        return false;
    }

    VMFrameCount = vmEnd__ - VMLocation + 1 ;        /* 必须在frameSize有意义后才可以执行 */

    VMSize = VMFrameCount * frameSize ;

    VMFrameTable = (VMFrame*)malloc(sizeof(VMFrame) * VMFrameCount);


    char log1[100];
    sprintf(log1, "%s\t%s\t%s%d\n", act, "操作结果:读取成功", "虚存帧个数:", VMFrameCount);
    char log2[100];
    sprintf(log2, "%s%d%s\t%s%d\t%s%d\n", "虚存大小:", VMSize, "B",
                                            "虚存起始扇区号:", VMLocation,
                                            "虚存结束扇区号:", vmEnd__);
    logPrintf(VM_LOG, "%s%s\n", log1,log2);
    /* 读取成功 */
    return true;
}

/**
* [initializeVM 初始化虚存]
* @return [成功返回true]
*/
bool initializeVM() {
    /*读取虚存位置，大小配置信息*/
    if (!readVMConfig()) {
        return false;
    }
    /* 重置虚拟内存帧表 */
    resetVMFrameTable();
    return true;
}

/**
* [saveVMConfig 修改虚拟内存配置信息]
* 包括虚拟内存位置，大小
* 不能单独使用，必须和文件系统配合使用
* 不推荐使用
* @return true--成功
*/
bool saveVMConfig() {

    /* 配置文件路径 */
    char * VMConfigPath__ = (char *)malloc(sizeof(char) * 38);
    sprintf(VMConfigPath__, "%sVMConfig.txt", VMPath);

    /* 打开配置文件，失败则返回false */
    FILE * ofp__ = fopen(VMConfigPath__, "w");
    if (ofp__ == NULL) {
        setError(VMEMORY_CONFIG_NOT_EXIST);
        return false;
    }
    fprintf(ofp__, "%d %d\n", VMLocation, VMFrameCount + VMLocation - 1);
    fclose(ofp__);
    free(VMConfigPath__);

    /* 写入成功 */
    return true;
}

/**
* [VMframeAllocateCount 统计某个进程占用的帧数]
* @param  __PID [进程号]
* @return       [返回分给某个进程的帧数]
*/
int VMframeAllocateCount(int __PID) {
    int i__;
    int count__ = 0;
    for (i__ = 0; i__ < VMFrameCount; i__++) {
        if (VMFrameTable[i__].PID == __PID && VMFrameTable[i__].isAvailable == true) {
            count__++;
        }
    }
    return count__;
}

/**
* [withdrawVM 释放某进程的虚存]
* @param  _PID [进程号]
*/
void withdrawVM(int __PID) {
    int i__;
    for (i__ = 0; i__ < VMFrameCount; i__++) {
        if (VMFrameTable[i__].PID == __PID) {
            VMFrameTable[i__].isAvailable = false;
            VMAvailableFrameCount++;
        }
    }
}


/**
* [diskToVM 把磁盘文件调入虚存]
* @param  __sectionNumber [磁盘扇区号]
* @param  __VMframeNum    [虚存帧号]
* @param  __PID           [进程号]
* @param  __pageNum       [被调入的进程的页号]
* @return                 [成功true]
*/
bool diskToVM(int __sectionNumber, int  __VMFrameNum) {
    char * section__ = (char *)malloc(sizeof(char) * frameSize);   /* 缓冲区 */
    int VMsectionNumber__ = VMframeNumToSection(__VMFrameNum);         /* 求出虚拟帧在磁盘上的地址 */
    //printf("%d\n", VMsectionNumber__);
    char act[] = "操作:将磁盘内容调入虚存";
    if (VMsectionNumber__ < 0) {
        setError(VM_FRAMENUM_NOT_AVAILABLE);
        logPrintf(VM_LOG, "%s\t%s\t%s%s\n\n", act, "操作失败", "失败原因:", errorDesc[VM_FRAMENUM_NOT_AVAILABLE]);
        return false;
    }
    /* 虚存帧 和 磁盘的一个扇区一样大*/
    /*  从磁盘中读出 */
    //printf("%d\n", __sectionNumber);
    if (!readSection(section__, __sectionNumber)) {
        return false;
    }
    if (!saveSection(section__, VMsectionNumber__)) {
        return false;
    }
    logPrintf(VM_LOG, "%s\t%s\t%s%d\t%s%d\n\n", act,"操作成功","磁盘号:",__sectionNumber,"虚拟帧号",__VMFrameNum);
    return true;

}

/**
* [allocate 分配虚拟内存给某进程]
*
* @param PID             该进程对应的PID(进程还未定义，不知道怎么用)
* @param  FrameCountApply  该进程申请的帧号
* @param  sectionNum[]   该进程对应的磁盘文件地址，共有__frameCountApply个
* @return                 申请成功，返回true，否则返回false
*/
bool allocateVM(int __PID, int __frameCountApply, int sectionNum[]) {
    int i__ = 0;
    int successApplyCount__ = 0;
    int remainApplyCount__ = __frameCountApply;

    char act[150];
    sprintf(act, "%s\t%s%d\t%s%d\t%s%d", "操作:分配虚拟内存给某进程", "进程号:", __PID, "申请帧数:", __frameCountApply, "空闲帧数:", VMAvailableFrameCount);
    /* 如果空闲帧少于申请的帧数，则申请失败*/
    //printf("%d %d\n", VMAvailableFrameCount, __frameCountApply);
    if (__frameCountApply > VMAvailableFrameCount) {
        setError(VMEMORY_APPLY_TOO_MUCH);
        //logPrintf(VM_LOG, "%s\t%s%d\t%s%d\t%s\t%s\t%s\n\n", act, "进程号:",__PID, "申请帧数",__frameCountApply,"操作失败", "失败原因:", errorDesc[VMEMORY_APPLY_TOO_MUCH]);
        logPrintf(VM_LOG, "%s\n%s\t%s\t%s\n\n", act, "操作失败", "失败原因:", errorDesc[VMEMORY_APPLY_TOO_MUCH]);
        return false;
    }
    else {

        /* 遍历搜寻空闲帧（扇区），改变虚拟内存帧的帧表 */
        for (i__ = 0; i__ < VMFrameCount && successApplyCount__ < __frameCountApply; i__++) {
            if (VMFrameTable[i__].isAvailable == false) {
                VMFrameTable[i__].isAvailable = true;    /* 该帧设置为被占用 */
                VMFrameTable[i__].PID = __PID;
                VMFrameTable[i__].pageNum = successApplyCount__;

                VMAvailableFrameCount--;			    /* 可用帧数减一 */
                remainApplyCount__--;                     /* 剩余还需要申请帧数减一 */
                successApplyCount__++;                    /* 成功申请帧数加一 */
                /**
                * 下面的判断是为了防止多进程竞争虚拟内存时，导致虚拟内存分配了一部分时发现
                * 虚拟内存不足的情况，此时要把分配出去的部分收回来
                */
                if (remainApplyCount__ > VMAvailableFrameCount) {
                    withdrawVM(__PID);                   /* 收回虚存 */
                    setError(VMEMORY_NOT_ENOUGH);
                    logPrintf(VM_LOG, "%s\n%s\t%s\t%s\n\n", act, "操作失败", "失败原因:", errorDesc[VMEMORY_NOT_ENOUGH]);
                    return false;
                }

            }
        }//for
        /* 把磁盘文件调入虚存*/
        //showVirtualMemoryDetial();
        int secInNum = 0;//带调入的磁盘的下标
        for (i__ = 0; i__ < VMFrameCount && secInNum < __frameCountApply; i__++) {
            if (VMFrameTable[i__].PID == __PID) {
                /*若未成功移入虚存，则需收回已分配的虚存*/
                if (!diskToVM(sectionNum[secInNum], i__)) {
                    withdrawVM(__PID);                   /* 收回虚存 */
                    return false;
                }
                else {
                    /*成功分配*/
                    secInNum++;
                }
            }//if(pid)
        }//for(i__)
    }//else
    /* 申请成功 */
    logPrintf(VM_LOG, "%s\n%s\t%s%d\n\n", act, "操作成功","分配的帧数:",__frameCountApply);
    return true;
}

/**
* [showVirtualMemoryDetial 显示虚存分配情况]
*/
void showVirtualMemoryDetial() {
    int i__;
    printf("%s: %d\n", "VMAvailableFrameCount is", VMAvailableFrameCount);
    printf("%10s|%15s|%20s|%12s\n", "frame num", "is available", "PID", "page num");
    for (i__ = 0; i__ < VMFrameCount; i__++) {
        printf("%10d|", i__);

        if (VMFrameTable[i__].isAvailable == true) {
            printf("%15s|", "true");
            printf("%20d|", VMFrameTable[i__].PID);
            printf("%12d\n", VMFrameTable[i__].pageNum);
        }
        else {
            printf("%15s|%20s|%12s\n", "false", "null", "null");
        }
    }//for
    printf("\n");
}

/**
* [findVMFrameToMemory 找到调入帧]
* @param  _PID     [进程ID]
* @param  _pageNum [进程需要调入的页号]
* @return          [返回虚存的对应帧号 未找到返回-1]
*/
int findVMFrameToMemory(int __PID, int __pageNum) {
    int i__;
    for (i__ = 0; i__ < VMFrameCount; i__++) {
        if (VMFrameTable[i__].PID == __PID && VMFrameTable[i__].pageNum == __pageNum) {
            return i__;
        }
    }
    return -1;
}

/**
* [VMframeNumToSection 返回某个虚拟帧对应的扇区]
* @param  __VMFrameNum [虚拟帧号]
* @return              [扇区号(没有返回-1)]
*/
int VMframeNumToSection(int __VMFrameNum) {
    if (__VMFrameNum < 0 || __VMFrameNum > VMFrameCount) {
        setError(VMEMORY_FRAMENUM_MAP_SECTION_NOT_FOUND);
        return -1;
    }
    else {
        return (VMLocation + __VMFrameNum);
    }
}

/**
* [isVMFrameNumAvailable 判断某个虚拟帧是否有效]
* @param  __VMFrameNum   [虚拟帧号]
* @return                [返回是否有效]
*/
bool isVMFrameNumAvailable(int __VMFrameNum) {
    return VMFrameTable[__VMFrameNum].isAvailable;
}

/**
 * [calVMUtilization 计算虚拟内存利用率]
 * @return [利用率]
 */
double calVMUtilization() {
    double utili = (double)VMAvailableFrameCount/(double)VMFrameCount;
    return (1 - utili);
}
