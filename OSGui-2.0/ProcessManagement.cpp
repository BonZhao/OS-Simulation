/**
 * MyOS\ProcessManagement.cpp
 *
 * Copyright (c) 2016 LyanQQ
 */

/**
 * Working log:
 * 2016/04/01 18:12		Created				by LyanQQ ;
 * 2016/04/01 22:11		1st time coding		by LyanQQ :
 * 		-PCB operation and enviroment functions completed.
 * 2016/04/02 21:00		2nd time coding		by LyanQQ :
 * 		-OFL operation and newProcess API completed.
 * 2016/04/03 21:49		3rd time coding		by LyanQQ :
 * 		-deleteProcess, dispatch and process scheduling completed.
 *		-First try of connection of the three parts. Succeed .
 */

#include "ProcessManagement.h"
#include "MemoryManagement.h"
#include "VirtualMemory.h"
#include "FileSystem.h"
#include "DiskDriver.h"
#include "IOUnit.h"
#include "Basic.h"
#include "ExeUnit.h"
#include "WorkLog.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <windows.h>

char * stateDesc[5] = {"新建", "就绪", "运行", "等待", "终止"} ;
char * eventDesc[4] = {"新建立的进程", "时间片耗尽", "等待I/O执行完毕", "I/O执行完毕"} ;
char * FileEx[2] = {"log", "run"} ;
PCBQueue * processQueue = NULL ;			/* 进程队列 */
OFL * openFileList = NULL ;					/* 打开文件列表 */
int totalDegenerationCount = 0 ;			/* 总共执行的退化次数 */
int totalProcessNumber = 0 ;				/* 进程总数 */
int priorityZeroCount = 0 ;

PCB * runningProcess ;						/* 当前正在被执行的进程 */
int timeSlice = 0 ;							/* 当前执行进程的时间片 */
clock_t startTime ;							/* 进程开始执行的时间 */

bool processManagementWorking = false ;

bool isDynamicPriority = true ;

HANDLE PCBQueueMutex ;
HANDLE runningProcessMutex ;

HANDLE 	dispathSemaphore;
HANDLE	runningProcessSemaphore;
HANDLE 	readyProcessSemaphore;
HANDLE	resetSemaphore;

DWORD WINAPI dispatchThread(LPVOID pPararneter);
DWORD WINAPI resetThread(LPVOID pPararneter);

extern int sectionSize ;
extern int processCounter ;
extern const char * errorDesc[] ;
extern ErrorType errorCode ;
extern bool LOG_OUTPUT;

void initPCB(PCB * __pcb)
{
    /**
     * 初始化PCB
     */

    if (__pcb == NULL) {
        return ;
    }

    __pcb->pid = -1 ;

    /* 初始化外部标识符 */
    __pcb->externalInfo.name = NULL ;
    __pcb->externalInfo.path = NULL ;

    /* 初始化执行信息 */
    __pcb->exeInfo.processCounter = 0 ;
    __pcb->exeInfo.codeMemory = 0 ;
    __pcb->exeInfo.memoryUsed = 0 ;
    __pcb->exeInfo.CPUUsed = 0 ;

    /* 初始化调度信息 */
    __pcb->schedInfo.state = NEW ;
    __pcb->schedInfo.priority = 0 ;
    __pcb->schedInfo.dynamicPriority = 0 ;

    TimeStamp ts__ ;
    getSystemTime(&ts__);
    __pcb->schedInfo.time.createTime = ts__ ;
    __pcb->schedInfo.time.lastTime = clock() ;
    __pcb->schedInfo.time.runningTime = 0 ;
    __pcb->schedInfo.time.waitingTime = 0 ;

    __pcb->schedInfo.event = NEW_PROCESS ;

    /* 初始化页表 */
    __pcb->pageTable.number = 0 ;
    __pcb->pageTable.table = NULL ;

    /* 初始化文件打开列表 */
    __pcb->ofl = (ProcessOpenFileList *)malloc(sizeof(ProcessOpenFileList)) ;
    __pcb->ofl->filePath = NULL ;
    __pcb->ofl->nextPtr = NULL ;

    /* 初始化数据段 */
    __pcb->dataSegment.heap = (MyHeap *)malloc(sizeof(MyHeap)) ;
    __pcb->dataSegment.heap->nextPtr = NULL ;
}

void freeProcessOpenFileList(ProcessOpenFileList * __ofl)
{
    /**
     * 摧毁进程文件打开列表
     */

    if (__ofl->nextPtr != NULL) {
        freeProcessOpenFileList(__ofl->nextPtr) ;
    }

    free(__ofl->filePath) ;
    free(__ofl) ;
}

void destroyPCB(PCB * __pcb)
{
    /**
     * 摧毁一个PCB，回收其申请的所有内存空间
     */

    if (__pcb == NULL) {
        return ;
    }

    free(__pcb->externalInfo.name) ;
    free(__pcb->externalInfo.path) ;
    free(__pcb->pageTable.table) ;
    freeProcessOpenFileList(__pcb->ofl) ;

    MyHeap * then = __pcb->dataSegment.heap ;
    MyHeap * tmp ;
    while (then != NULL) {
        tmp = then ;
        then = then->nextPtr ;
        free(tmp) ;
    }
}

void initOpenFile(_OpenFile * __of)
{
    __of->filePath = NULL ;
    __of->counter = 0 ;
}

void destroyOpenFile(_OpenFile * __of)
{
    free(__of->filePath) ;
}

OFL * findOpenFile(const char * __path)
{
    OFL * head__ = openFileList->nextPtr ;

    while (head__ != NULL) {
        if (strcmp(head__->of.filePath, __path) == 0) {
            return head__ ;
        }
        else {
            head__ = head__->nextPtr ;
        }
    }

    return NULL ;
}

void addOpenFile(const char * __path)
{
    OFL * ofl__ ;

    /* 查询该文件是否已被其他进程打开 */
    ofl__ = findOpenFile(__path) ;

    /* 若否，则新建一个打开文件节点 */
    if (ofl__ == NULL) {
        OFL * head__ = openFileList ;
        ofl__ = (OFL *)malloc(sizeof(OFL)) ;

        ofl__->of.filePath = (char *)malloc(sizeof(char) * (strlen(__path)+1));

        strcpy(ofl__->of.filePath, __path) ;

        ofl__->of.counter = 1 ;
        ofl__->nextPtr = NULL ;

        while (head__->nextPtr != NULL) {
            head__ = head__->nextPtr ;
        }

        head__->nextPtr = ofl__ ;
    }
    /* 若是，则计数器加一 */
    else {
        ofl__->of.counter++ ;
    }
}

bool closeOpenFile(const char * __path)
{
    OFL * ofl__ ;

    /* 查询该文件是否存在打开记录 */
    ofl__ = findOpenFile(__path) ;

    /* 若否，则返回 */
    if (ofl__ == NULL) {
        setError(SYSTEM_ERROR);
        return false ;
    }
    else {
        /* 计数器自减一 */
        ofl__->of.counter-- ;

        /* 计数值为0，则需删除该节点 */
        if (ofl__->of.counter <= 0) {
            OFL * pre__ = openFileList ;

            while (pre__ != NULL) {
                if (pre__->nextPtr == ofl__) {
                    pre__->nextPtr = ofl__->nextPtr ;

                    destroyOpenFile(&(ofl__->of));
                    free(ofl__);

                    return true ;
                }
                else {
                    pre__ = pre__->nextPtr ;
                }
            }

            /* 未知错误 */
            setError(UNKNOW_ERROR);
            return false ;
        }

        return true ;
    }
}

bool checkOpenFile(const char * __path)
{
    /**
     * 检查打开文件条目，若指定文件没有被打开返回true
     */

    OFL * ofl__ = openFileList->nextPtr ;

    while (ofl__ != NULL) {
        /* 路径相同且计数器大于0 */
        if (strcmp(__path, ofl__->of.filePath) == 0 && ofl__->of.counter > 0) {
            return false ;
        }

        ofl__ = ofl__->nextPtr ;
    }

    return true ;
}

void showOFL()
{
    OFL * ofl__ = openFileList->nextPtr ;

    puts("打开文件列表（Open File List）：\n");

    if (ofl__ == NULL) {
        puts("打开文件列表为空") ;
        return ;
    }

    while (ofl__ != NULL) {
        printf("文件路径:%s\t打开进程数:%d\n", ofl__->of.filePath, ofl__->of.counter);
        ofl__ = ofl__->nextPtr ;
    }
}

void initProcessManagement()
{
    /**
     * 初始化进程管理模块
     * 包括初始化进程队列和文件打开列表
     */

    /* 初始化进程队列，建立头结点 */
    processQueue = (PCBQueue *)malloc(sizeof(PCBQueue)) ;

    //initPCB(processQueue->pcb) ;
    processQueue->pcb = NULL ;
    processQueue->nextPtr = NULL ;

    /* 初始化文件打开列表，建立头结点 */
    openFileList = (OFL *)malloc(sizeof(OFL)) ;

    initOpenFile(&(openFileList->of)) ;
    openFileList->nextPtr = NULL ;

    /* 重置正在执行的进程 */
    runningProcess = NULL ;

    /* 重置退化总次数 */
    totalDegenerationCount = 0 ;

    /* 重置进程计数信息 */
    totalProcessNumber = 0 ;
    priorityZeroCount = 0 ;

    /* PCB列表访问锁 */
    //PCBQueueMutex = CreateMutex(NULL, FALSE, "PCBQueueMutex") ;
    PCBQueueMutex = CreateMutex(NULL, FALSE, NULL) ;

    /* runningProcess访问锁 */
    runningProcessMutex = CreateMutex(NULL, FALSE, NULL) ;

    /* 指派进程的信号量 */
    //dispathSemaphore = CreateSemaphore(NULL, 1, 1, "dispathSemaphore") ;
    dispathSemaphore = CreateSemaphore(NULL, 1, 1, NULL) ;

    /* 执行进程的信号量 */
    //runningProcessSemaphore = CreateSemaphore(NULL, 1, 1, "runningProcessSemaphore") ;
    runningProcessSemaphore = CreateSemaphore(NULL, 0, 1, NULL) ;

    /* 就绪进程计数信号量 */
    //readyProcessSemaphore = CreateSemaphore(NULL, 0, MAX_PROCESS_NUMBER, "readyProcessSemaphore") ;
    readyProcessSemaphore = CreateSemaphore(NULL, 0, MAX_PROCESS_NUMBER, NULL) ;

    /* 重置进程优先级信号量 */
    //resetSemaphore = CreateSemaphore(NULL, 0, 1 , "resetSemaphore") ;
    resetSemaphore = CreateSemaphore(NULL, 0, 1 , NULL) ;

    processManagementWorking = true ;

    HANDLE dispathHandle, resetHandle ;
    dispathHandle = CreateThread(NULL, 0, dispatchThread, NULL, 0, NULL);
    CloseHandle(dispathHandle) ;
    resetHandle = CreateThread(NULL, 0, resetThread, NULL, 0, NULL);
    CloseHandle(resetHandle) ;
}

void destroyProcessManagement()
{
    PCBQueue * pcb_n__ , * pcb_t__ ;
    OFL * ofl_n__, * ofl_t__ ;

    processManagementWorking = false ;

    pcb_n__ = processQueue ;

    while (pcb_n__ != NULL) {
        pcb_t__ = pcb_n__->nextPtr ;

        destroyPCB(pcb_n__->pcb);

        free(pcb_n__);

        pcb_n__ = pcb_t__ ;
    }

    ofl_n__ = openFileList ;

    while (ofl_n__ != NULL) {
        ofl_t__ = ofl_n__->nextPtr ;

        destroyOpenFile(&(ofl_n__->of));

        free(ofl_n__);

        ofl_n__ = ofl_t__ ;
    }

    CloseHandle(PCBQueueMutex) ;

    CloseHandle(dispathSemaphore) ;
    CloseHandle(runningProcessSemaphore);
    CloseHandle(readyProcessSemaphore) ;
    CloseHandle(resetSemaphore) ;
}

void newPCB(PCB * __pcb, ProcessID __pid, const char * __name, const char * __path, ProcessPriority __pri, int __pgnum)
{
    /**
     * 新建一个PCB，但不会挂到进程队列中，此时PCB的状态是NEW
     */

    /* 初始化 */
    initPCB(__pcb) ;

    /* 记录PID */
    __pcb->pid = __pid ;

    /* 记录外部标识符 */
    __pcb->externalInfo.name = (char *)malloc(sizeof(char) * (strlen(__name)+1)) ;
    strcpy(__pcb->externalInfo.name, __name);
    __pcb->externalInfo.path = (char *)malloc(sizeof(char) * (strlen(__path)+1)) ;
    strcpy(__pcb->externalInfo.path, __path);

    __pcb->schedInfo.state = NEW ;
    __pcb->schedInfo.event = NEW_PROCESS ;

    /* 记录固定优先级 */
    __pcb->schedInfo.priority = __pri ;
    __pcb->schedInfo.dynamicPriority = __pri ;

    /* 新建页表 */
    __pcb->pageTable.number = __pgnum ;
    __pcb->pageTable.table = (ProcessPage *)malloc(sizeof(ProcessPage) * __pgnum) ;
    for (int i = 0; i <= __pgnum - 1; i++) {
        __pcb->pageTable.table[i].isAvailable = false ;
        __pcb->pageTable.table[i].frameNumber = 0 ;
    }

    /* 记录代码段长度 */
    FileStream * fs__ ;

    fs__ = openFile(__path);
    if (fs__ != NULL) {
        __pcb->exeInfo.codeMemory = getFileLength(fs__) ;
        closeFile(fs__);
    }

    /* 初始化数据段信息 */
    __pcb->dataSegment.begin = __pcb->exeInfo.codeMemory ;
    __pcb->dataSegment.end = __pgnum * sectionSize - 1 ;
    __pcb->dataSegment.used = 0 ;

    /* 进程文件打开列表 */
    /* 缺省 */
}

bool setProcessState(PCB * __pcb, ProcessState __st)
{
    if (__pcb == NULL) {
        return false;
    }

    ProcessState __sn = __pcb->schedInfo.state ;
    bool ret__ ;

    if (__st == TERMINATED) {
        WaitForSingleObject(PCBQueueMutex, INFINITE);

        __pcb->schedInfo.state = TERMINATED ;

        if (__sn == RUNNING) {
            WaitForSingleObject(runningProcessMutex, INFINITE) ;
            setCPURatio(runningProcess, 0);
            runningProcess = NULL ;
            ReleaseMutex(runningProcessMutex) ;

            /* 累加执行时长 */
            __pcb->schedInfo.time.runningTime += clock() - __pcb->schedInfo.time.lastTime ;

            /* 释放执行进程锁，准备派遣新进程 */
            ReleaseSemaphore(dispathSemaphore, 1, NULL);
        }
        else {
            /* 累加等待时长 */
            __pcb->schedInfo.time.waitingTime += clock() - __pcb->schedInfo.time.lastTime ;

            if (__sn == READY) {
                /* 就绪进程减1 */
                WaitForSingleObject(readyProcessSemaphore, INFINITE) ;
            }
        }

        /* 工作日志输出 */
        if (LOG_OUTPUT) {
            char buf[30] ;
            logPrintf(PCB_QUEUE_LOG, "进程PID：%d\n原队列：%s\t转移到队列：%s\n进程已终止：\n\t进程创建时间：%s\n\t总执行时长：%d ms\t总等待时长：%d ms\n\n"
                                ,__pcb->pid, stateDesc[__sn], stateDesc[__st], logTime(buf, __pcb->schedInfo.time.createTime), __pcb->schedInfo.time.runningTime, __pcb->schedInfo.time.waitingTime);
        }

        ReleaseMutex(PCBQueueMutex) ;

        /* 删除进程 */
        deleteProcess(__pcb->pid) ;

        return true ;
    }

    switch (__sn) {
        /* 当前状态为 NEW */
        case NEW :
            if (__st == READY) {
                WaitForSingleObject(PCBQueueMutex, INFINITE);

                __pcb->schedInfo.state = READY ;

                priorityAdjust(__pcb);

                /* 就绪进程加1 */
                ReleaseSemaphore(readyProcessSemaphore, 1, NULL);

                ReleaseMutex(PCBQueueMutex);
                ret__ = true ;
            }
            else {
                ret__ = false ;
            }
        break ;

        /* 当前状态为 READY */
        case READY :
            if (__st == RUNNING) {
                WaitForSingleObject(PCBQueueMutex, INFINITE);

                __pcb->schedInfo.state = RUNNING ;

                /* 累加等待时长 */
                __pcb->schedInfo.time.waitingTime += clock() - __pcb->schedInfo.time.lastTime ;
                __pcb->schedInfo.time.lastTime = clock() ;

                /* 就绪进程减1 */
                WaitForSingleObject(readyProcessSemaphore, INFINITE) ;

                ReleaseMutex(PCBQueueMutex);
                ret__ = true ;
            }
            else {
                ret__ = false ;
            }
        break ;

        /* 当前状态为 RUNNING */
        case RUNNING :
            if (__st == WAITING || __st == READY) {
                WaitForSingleObject(PCBQueueMutex, INFINITE);

                /* 执行进程置空 */
                WaitForSingleObject(runningProcessMutex, INFINITE) ;
                setCPURatio(runningProcess, 0);
                runningProcess = NULL ;
                ReleaseMutex(runningProcessMutex) ;

                __pcb->schedInfo.state = __st ;

                /* 累加执行时长 */
                __pcb->schedInfo.time.runningTime += clock() - __pcb->schedInfo.time.lastTime ;
                __pcb->schedInfo.time.lastTime = clock() ;

                /* 保存程序计数器的值 */
                __pcb->exeInfo.processCounter = processCounter ;

                if (__st == READY) {
                    /* 就绪进程加1 */
                    ReleaseSemaphore(readyProcessSemaphore, 1, NULL);
                }

                /* 释放执行进程锁，准备派遣新进程 */
                ReleaseSemaphore(dispathSemaphore, 1, NULL);

                ReleaseMutex(PCBQueueMutex);
                ret__ = true ;
            }
            else {
                ret__ = false ;
            }
        break ;

        /* 当前状态为 WAITING */
        case WAITING :
            if (__st == READY) {
                WaitForSingleObject(PCBQueueMutex, INFINITE);

                __pcb->schedInfo.state = __st ;

                /* 就绪进程加1 */
                ReleaseSemaphore(readyProcessSemaphore, 1, NULL);

                ReleaseMutex(PCBQueueMutex);
                ret__ = true ;
            }
            else {
                ret__ = false ;
            }
        break ;

        /* 当前状态为 TERMINATED */
        case TERMINATED :
            ret__ = false ;
        break ;

        /* 其他状态 */
        default:
            ret__ = false ;
        break ;
    }

    /* 工作日志输出 */
    if (LOG_OUTPUT) {
        char buf[30] ;
        logPrintf(PCB_QUEUE_LOG, "进程PID：%d\n原队列：%s\t转移到队列：%s\n详情：\n\t进程创建时间：%s\n\t总执行时长：%d ms\t总等待时长：%d ms\t动态优先级：%d\n\n"
                            ,__pcb->pid, stateDesc[__sn], stateDesc[__st], logTime(buf, __pcb->schedInfo.time.createTime), __pcb->schedInfo.time.runningTime, __pcb->schedInfo.time.waitingTime, __pcb->schedInfo.dynamicPriority);
    }

    return ret__ ;
}

void setCPURatio(PCB * __pcb, int __ra)
{
    if (__pcb == NULL || __ra < 0 || __ra > 100) {
        return ;
    }

    __pcb->exeInfo.CPUUsed = __ra ;
}

void setMemoryUsed(PCB * __pcb, int __ra)
{
    if (__pcb == NULL) {
        return ;
    }

    if (__pcb->exeInfo.memoryUsed < -__ra) {
        __pcb->exeInfo.memoryUsed = 0 ;
    }
    else {
        __pcb->exeInfo.memoryUsed += __ra ;
    }
}

void addToQueue(PCB * __pcb)
{
    /**
     * 将新建的PCB挂起到进程队列中
     */

    /* 申请进程队列访问锁 */
    WaitForSingleObject(PCBQueueMutex, INFINITE);

    PCBQueue * pq__ = (PCBQueue *)malloc(sizeof(PCBQueue)) ;
    PCBQueue * head__ = processQueue ;

    /* 移动到尾节点 */
    while (head__->nextPtr != NULL) {
        head__ = head__->nextPtr ;
    }

    pq__->pcb = __pcb ;
    pq__->nextPtr = NULL ;

    head__->nextPtr = pq__ ;

    /* 进程总数加1 */
    totalProcessNumber++ ;

    /* 释放锁 */
    ReleaseMutex(PCBQueueMutex);
}

bool deleteFromQueue(ProcessID __pid)
{
    /* 申请进程队列访问锁 */
    WaitForSingleObject(PCBQueueMutex, INFINITE);

    PCBQueue * pre__ = processQueue ;
    PCBQueue * then__ = pre__->nextPtr ;

    while (then__ != NULL) {
        if (then__->pcb->pid == __pid) {
            pre__->nextPtr = then__->nextPtr ;
            destroyPCB(then__->pcb);
        //	free(then__);

            /* 进程总数减1 */
            totalProcessNumber-- ;

            /* 释放锁 */
            ReleaseMutex(PCBQueueMutex);
            return true ;
        }
        else {
            pre__ = then__ ;
            then__ = then__->nextPtr ;
        }
    }

    /* 释放锁 */
    ReleaseMutex(PCBQueueMutex);
    return false ;
}

int getQueueLength(ProcessState __state)
{
    /**
     * 返回某个进程队列的长度
     */

    /* 申请进程队列访问锁 */
    WaitForSingleObject(PCBQueueMutex, INFINITE);

    PCBQueue * pq__ = processQueue->nextPtr ;
    int count__ = 0 ;

    while (pq__ != NULL) {
        if (pq__->pcb->schedInfo.state == __state) {
            count__ ++ ;
        }

        pq__ = pq__->nextPtr ;
    }

    ReleaseMutex(PCBQueueMutex);
    return count__ ;
}

PCB * findPCB(ProcessID __pid)
{
    /**
     * 在所有进程队列中找到某个pid的进程并返回
     */

    WaitForSingleObject(PCBQueueMutex, INFINITE);

    PCBQueue * head__ = processQueue ;
    head__ = head__->nextPtr ;

    while (head__ != NULL) {
        if (head__->pcb->pid == __pid) {
            ReleaseMutex(PCBQueueMutex);
            return head__->pcb ;
        }
        else {
            head__ = head__->nextPtr ;
        }
    }

    ReleaseMutex(PCBQueueMutex);
    return NULL ;
}

PCB * findPCB(ProcessState __state, ProcessID __pid)
{
    /**
     * 在__state队列中找到某个pid的进程并返回
     */

    WaitForSingleObject(PCBQueueMutex, INFINITE);

    PCBQueue * head__ = processQueue ;
    head__ = head__->nextPtr ;

    while (head__ != NULL) {
        if (head__->pcb->pid == __pid && head__->pcb->schedInfo.state == __state) {
            ReleaseMutex(PCBQueueMutex);
            return head__->pcb ;
        }
        else {
            head__ = head__->nextPtr ;
        }
    }

    ReleaseMutex(PCBQueueMutex);
    return NULL ;
}

ProcessPriority getPri(int __num)
{
    ProcessPriority pri__ = (ProcessPriority)((__num + 1) * 3.375) ;

    if (pri__ > MY_MAX_PRIORITY) {
        pri__ = MY_MAX_PRIORITY ;
    }

    return pri__ ;
}

int getTimeSlice(ProcessPriority __pri)
{
    return 2 * __pri + 20 ;
}

ProcessID newPID()
{
    ProcessID pid__ ;

    srand( (unsigned)time( NULL ) );

    pid__ = rand()%MAX_PID + 1 ;
    while (findPCB(pid__)!=NULL) {
        pid__ = rand()%MAX_PID + 1 ;
    }

    return pid__ ;
}

bool newProcess(const char * __path)
{
    /**
     * 打开指定路径的可执行文件并新建该进程
     */

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    int secNum__ ;

    /* 读取目标路径 */
    if (getIndexSection(sec__, &secNum__, __path)) {
        /* 检查是否是文件类型 */
        if (sec__[0] == FILE_TYPE) {
            FileIndex fi__ ;
            char * name__ = (char *)malloc(sizeof(char) * LENGTH_NAME) ;
            char * ex__ = (char *)malloc(sizeof(char) * LENGTH_NAME) ;

            /* 提取文件信息 */
            initFileIndex(&fi__) ;
            extractFileIndex(sec__, &fi__) ;

            /* 提取后缀名，并检查是否为可执行文件 */
            extractFileName(name__, ex__, fi__.name) ;
            if (strcmp(ex__, FileEx[RUN_FILE]) == 0) {
                /* 数据扇区为空，则无法执行 */
                if (fi__.dataSectionNumber > 0) {
                    /* 读取第一个数据扇区 */
                    if (readSection(sec__, fi__.dataNode[0])) {
                        /* 提取第一句配置语句 */
                        int first3 = sec__[0] / 32 ;
                        int last5 = sec__[0] % 32 ;

                        /* 检测第一句是否为配置语句，否则可执行文件格式有误，无法执行 */
                        if (first3 == 0) {
                            /* 计算为进程分配的优先级 */
                            ProcessPriority pri__ = getPri(last5) ;
                            /* 获取一个全局唯一的PID */
                            ProcessID pid__ = newPID() ;

                            /* 为进程分配虚存，并将执行代码调入虚存 */
                            if (allocateVM(pid__, fi__.dataSectionNumber, fi__.dataNode)) {
                                PCB * pcb__ = (PCB *)malloc(sizeof(PCB)) ;
                                //FileStream * fs__ ;


                                /* 新建PCB */
                                newPCB(pcb__, pid__, fi__.name, __path, pri__, fi__.dataSectionNumber);

                                /* fs__ = openFile(__path);
                                if (fs__ != NULL) {
                                    pcb__->exeInfo.codeMemory = getFileLength(fs__) ;
                                    closeFile(fs__);
                                } */

                                /* 添加打开文件到列表中 */
                                addOpenFile(__path);

                                //pcb__->schedInfo.event = NEW_PROCESS ;

                                /* 将该进程挂入队列中 */
                                addToQueue(pcb__);

                                /* 转移到就绪队列 */
                                setProcessState(pcb__, READY);

                                //setProcessState(pcb__, NEW);

                                /* 新建进程成功 */
                                return true ;
                            }
                            else {
                                ;
                            }
                        }
                        else {
                            setError(CAN_NOT_RUN_FILE);
                        }
                    }
                    else {
                        setError(CAN_NOT_RUN_FILE);
                    }
                }
                else {
                    setError(CAN_NOT_RUN_FILE);
                }
            }
            else {
                setError(NOT_RUNNABLE_FILE);
            }

            free(name__);
            free(ex__);
        }
        else {
            setError(PATH_NOT_EXIST);
        }
    }
    else {
        setError(PATH_NOT_EXIST);
    }

    free(sec__);
    return false ;
}

bool deleteProcess(ProcessID __pid)
{
    /**
     * 终止指定PID的进程
     */

    PCB * pcb__ ;
    pcb__ = findPCB(__pid) ;

    /* 进程不存在 */
    if (pcb__ != NULL) {
        /* 如果该进程正在被执行，则需清空执行进程，便于下次调度 */
        /* if (pcb__->schedInfo.state == RUNNING) {
            runningProcess = NULL ;
        } */

        if (pcb__->schedInfo.dynamicPriority == 0) {
            priorityZeroCount-- ;
        }

        /* 释放该进程的虚存空间 */
        withdrawMemory(__pid) ;

        /* 在文件打开列表中关闭该文件 */
        closeOpenFile(pcb__->externalInfo.path);

        /* 从队列中删除该进程 */
        if (deleteFromQueue(__pid)) {
            return true ;
        }
        else {
            setError(UNKNOW_ERROR);
            return false ;
        }
    }
    else {
        setError(UNAVAILABLE_PROCESS) ;
        return false ;
    }
}

bool showPCB(ProcessID __pid)
{
    PCB * pcb__ ;
    pcb__ = findPCB(__pid) ;
    char buf[40] ;
    char * tf[2] = {"False", "True"} ;

    if (pcb__ != NULL) {
        printf("进程ID：%d\n\n", pcb__->pid);

        puts("外部标识符：");
        printf("\t进程名称：%s\t路径：%s\n", pcb__->externalInfo.name, pcb__->externalInfo.path);

        puts("执行信息：");
        printf("\tPC：%d\n\tCPU使用：%d%%\t内存使用：%d B\n"
            , pcb__->exeInfo.processCounter, pcb__->exeInfo.CPUUsed, pcb__->exeInfo.memoryUsed + pcb__->exeInfo.codeMemory);

        puts("调度信息：");
        printf("\t状态：%s\n\t固定优先级：%d\t动态优先级：%d\n"
            , stateDesc[pcb__->schedInfo.state], pcb__->schedInfo.priority, pcb__->schedInfo.dynamicPriority);
        showTime(buf, pcb__->schedInfo.time.createTime);
        printf("\t进程创建时间：%s\n\t执行时长：%d ms\t等待时长：%d ms\n"
            , buf, pcb__->schedInfo.time.runningTime, pcb__->schedInfo.time.waitingTime);

        printf("\t事件：");
        if (pcb__->schedInfo.state != RUNNING) {
            puts(eventDesc[pcb__->schedInfo.event]);
        }
        else {
            putchar('\n');
        }

        puts("页表：\n\t页号\t有效位\t帧号");
        for (int i=0; i <= pcb__->pageTable.number-1; i++) {
            printf("\t%d\t%s\t%d\n", i, tf[pcb__->pageTable.table[i].isAvailable], pcb__->pageTable.table[i].frameNumber);
        }

        return true ;
    }
    else {
        setError(UNAVAILABLE_PROCESS) ;
        return false ;
    }
}

void showPCBQueue()
{
    WaitForSingleObject(PCBQueueMutex, INFINITE);

    PCBQueue * head__ = processQueue->nextPtr ;
    PCB * pcb ;

    puts("进程列表：\n");

    if (head__ == NULL) {
        puts("进程列表为空");

        ReleaseMutex(PCBQueueMutex);
        return ;
    }

    while (head__ != NULL) {
        pcb = head__->pcb ;
        printf("PID:%d\tName:%s\tState:%s\tPri:%d\n"
                , pcb->pid, pcb->externalInfo.name, stateDesc[pcb->schedInfo.state], pcb->schedInfo.dynamicPriority);

        //printf("runTime:%d\twaitTime:%d\n", pcb->schedInfo.time.runningTime, pcb->schedInfo.time.waitingTime);

        head__ = head__->nextPtr ;
    }

    printf("\n总进程数：%d\n优先级降为0进程数：%d\n\n", totalProcessNumber, priorityZeroCount) ;

    ReleaseMutex(PCBQueueMutex);
}

void degeneration(PCB * __pcb)
{
    /**
     * 动态优先级的退化函数
     */

    if (__pcb == NULL) {
        return ;
    }

    ProcessPriority pri__ = __pcb->schedInfo.dynamicPriority;

    if (pri__ > 0) {
        /* 总退化次数加一 */
        totalDegenerationCount++ ;

        if (isDynamicPriority) {
            __pcb->schedInfo.dynamicPriority = (ProcessPriority)(sqrt(pri__) - 1) ;
        }
        else {
            __pcb->schedInfo.dynamicPriority = 0 ;
        }

        if (__pcb->schedInfo.dynamicPriority == 0) {
            priorityZeroCount++ ;
            if (priorityZeroCount == totalProcessNumber) {
                ReleaseSemaphore(resetSemaphore, 1, NULL) ;
            }
        }
    }
    else {
        return ;
    }
}

void priorityAdjust(PCB * __pcb)
{
    /**
     * 动态优先级调整，使得新调入等待队列的进程在退化次数上
     * 与队列平均值一致
     */

    int count ;

    if (totalProcessNumber == 0) {
        return ;
    }
    else {
        count = (int)((double)totalDegenerationCount / totalProcessNumber + 0.5) ;
    }

    for (int i = 0; i <= count-1; i++) {
        degeneration(__pcb);
    }
}

/**
 * [dispatchProcess 选择一个进程执行（就绪队列中动态优先级最高的）]
 * @return [返回进程的PCB指针，没有则返回NULL]
 */
PCB * dispatchProcess() {

    WaitForSingleObject(PCBQueueMutex, INFINITE);

    PCBQueue * pcb_temp__ = NULL, *pcb_ans__ = NULL;
    pcb_temp__ = processQueue ;

    pcb_temp__ = pcb_temp__->nextPtr ;

    /*遍历整个链表*/
    while (pcb_temp__ != NULL) {

        /*如果pcb_ans__为空*/
        if(pcb_ans__ == NULL) {
            if(pcb_temp__->pcb->schedInfo.state == READY ) {
                pcb_ans__ = pcb_temp__;
            }
        }
        /*如果pcb_ans__不为空*/
        else {
            if(pcb_temp__->pcb->schedInfo.state == READY &&
                pcb_temp__->pcb->schedInfo.dynamicPriority >
                 pcb_ans__->pcb->schedInfo.dynamicPriority) {
                pcb_ans__ = pcb_temp__;
            }
        }
        pcb_temp__ = pcb_temp__->nextPtr ;
    }
    if(pcb_ans__ == NULL) {
        ReleaseMutex(PCBQueueMutex);
        return NULL;
    }

    ReleaseMutex(PCBQueueMutex);
    return pcb_ans__->pcb;
}

void finishTimeSlice()
{
    WaitForSingleObject(runningProcessMutex, INFINITE) ;
    if (runningProcess != NULL) {
        /* 保存PC的值 */
        runningProcess->exeInfo.processCounter = processCounter ;

        /* 对其动态优先级进行一次退化 */
        degeneration(runningProcess) ;

        /* 设置阻塞事件 */
        runningProcess->schedInfo.event = RUN_OUT_OF_TIME_SLICE ;

        ReleaseMutex(runningProcessMutex) ;
        /* 将该进程调入就绪队列 */
        setProcessState(runningProcess, READY);
    }

    ReleaseMutex(runningProcessMutex) ;
}

void pageUnvailable(ProcessID __pid, int __pgn)
{
    PCB * pcb__ ;

    pcb__ = findPCB(__pid) ;

    if (pcb__ != NULL) {
        pcb__->pageTable.table[__pgn].isAvailable = false ;
    }
}

void illegalAccess(PCB * __pcb)
{
    /* logPrintf */
    /* ... */

    setProcessState(__pcb, TERMINATED);
}

bool IORequest (int __time)
{
    WaitForSingleObject(runningProcessMutex, INFINITE) ;
    if (runningProcess == NULL) {
        ReleaseMutex(runningProcessMutex) ;
        setError(UNAVAILABLE_PROCESS) ;
        return false ;
    }
    /* 发送IO请求 */
    if (newIO(runningProcess->pid, __time)) {
        /* 设置等待事件 */
        runningProcess->schedInfo.event = WAITING_FOR_IO ;

        /* 保存PC的值 */
        runningProcess->exeInfo.processCounter = processCounter ;

        ReleaseMutex(runningProcessMutex) ;
        /* 将进程移到等待队列 */
        setProcessState(runningProcess, WAITING) ;

        return true ;
    }
    else {
        setError(IO_REQUEST_INVALID) ;
    }

    ReleaseMutex(runningProcessMutex) ;
    return false ;
}

void IOFinish(ProcessID __pid)
{
    PCB * pcb__ ;

    pcb__ = findPCB(WAITING, __pid);
    if (pcb__ != NULL) {
        /* 设置等待事件 */
        pcb__->schedInfo.event = IO_FINISH ;

        /* 优先级调整 */
        degeneration(pcb__);

        /* 将进程移到就绪队列 */
        setProcessState(pcb__, READY);
    }
}

void dispath()
{
    WaitForSingleObject(dispathSemaphore, INFINITE) ;

    WaitForSingleObject(readyProcessSemaphore, INFINITE) ;

    ReleaseSemaphore(readyProcessSemaphore, 1, NULL) ;

    PCB * pcb__ ;

    pcb__ = dispatchProcess() ;

    if (pcb__ != NULL) {
        /* 计算时间片长度 */
        timeSlice = /*20 **/ getTimeSlice(pcb__->schedInfo.priority);

        /* 更新PC */
        processCounter = pcb__->exeInfo.processCounter ;

        /* 将该进程调入执行 */
        setProcessState(pcb__, RUNNING);
        WaitForSingleObject(runningProcessMutex, INFINITE) ;
        runningProcess = pcb__ ;
        ReleaseMutex(runningProcessMutex) ;

        /* 释放执行进程信号量 */
        ReleaseSemaphore(runningProcessSemaphore, 1, NULL) ;

        /* 执行计时开始 */
        startTime = clock() ;
    }
    else {
        //runningProcess = NULL ;

        /* 等待10ms */
        Sleep(10);

        /* 释放锁 */
        ReleaseSemaphore(dispathSemaphore, 1, NULL) ;
    }
}

void resetQueue()
{
    WaitForSingleObject(resetSemaphore, INFINITE) ;

    WaitForSingleObject(PCBQueueMutex, INFINITE);

    PCBQueue * pq__ = processQueue->nextPtr ;

    /* 重置动态优先级 */
    while (pq__ != NULL) {
        pq__->pcb->schedInfo.dynamicPriority = pq__->pcb->schedInfo.priority ;

        pq__ = pq__->nextPtr ;
    }

    /* 重置总退化次数 */
    totalDegenerationCount = 0 ;
    priorityZeroCount = 0 ;

    ReleaseMutex(PCBQueueMutex);
}

DWORD WINAPI dispatchThread(LPVOID pPararneter)
{
    /**
     * 负责指派进程
     */
    while (processManagementWorking)
    {
        dispath();
    }
    return 0;
}

DWORD WINAPI resetThread(LPVOID pPararneter)
{
    /**
     * 负责重置进程队列
     */
    while (processManagementWorking)
    {
        resetQueue();
    }
    return 0;
}

MyHeap * findDataSegment(PCB * __pcb, int __label)
{
    if (__pcb == NULL) {
        return NULL ;
    }

    MyHeap * tmp = __pcb->dataSegment.heap->nextPtr ;
    while (tmp != NULL) {
        if (tmp->label == __label) {
            return tmp ;
        }

        tmp = tmp->nextPtr ;
    }

    return NULL ;
}

bool freeDataSegment(PCB * __pcb, int __label)
{
    if (__pcb == NULL) {
        return false ;
    }

    MyHeap * tmp = __pcb->dataSegment.heap ;
    MyHeap * then = tmp->nextPtr ;

    while (then != NULL) {
        if (then->label == __label) {
            /* 释放使用空间 */
            __pcb->dataSegment.used -= then->end - then->begin + 1 ;
            if (__pcb->dataSegment.used < 0) {
                __pcb->dataSegment.used = 0 ;
            }

            logPrintf(HEAP_LOG, "操作： 释放内存\n进程PID： %d\n\t地址变量： %d\t起始地址： %d\t结束地址： %d\n\n"
                            , __pcb->pid, then->label, then->begin, then->end) ;

            setMemoryUsed(__pcb, then->begin - then->end - 1);

            /* 释放该节点 */
            tmp->nextPtr = then->nextPtr ;
            free(then) ;

            return true ;
        }

        tmp = then ;
        then = then->nextPtr ;
    }

    logPrintf(HEAP_LOG, "操作： 释放内存\n进程PID： %d\n\t地址变量： %d\t没有匹配的堆空间\n\n"
                    , __pcb->pid, __label) ;
    return NULL ;
}

int firstFit(PCB * __pcb, int __label, int __len)
{
    /**
     * 首次适应算法，返回找到的足够长的数据段的起始地址
     * 适应失败则返回负值
     */

    if (__pcb == NULL) {
        return -3 ;
    }

    int tot = __pcb->dataSegment.end - __pcb->dataSegment.begin + 1 ;
    /* 没有足够空间 */
    if (__len > tot - __pcb->dataSegment.used) {
        return -2 ;
    }

    MyHeap * tmp = __pcb->dataSegment.heap->nextPtr ;

    /* 没有已分配的数据段，直接返回起始地址 */
    if (tmp == NULL) {
        __pcb->dataSegment.used += __len ;

        MyHeap * newh = (MyHeap *)malloc(sizeof(MyHeap)) ;
        newh->label = __label ;
        newh->begin = __pcb->dataSegment.begin ;
        newh->end = newh->begin + __len - 1 ;
        newh->nextPtr = tmp ;
        __pcb->dataSegment.heap->nextPtr = newh ;

        return newh->begin ;
    }

    /* 分配位置在已分配的第一个数据段之前 */
    if (__len <= tmp->begin - __pcb->dataSegment.begin) {
        __pcb->dataSegment.used += __len ;

        MyHeap * newh = (MyHeap *)malloc(sizeof(MyHeap)) ;
        newh->label = __label ;
        newh->begin = __pcb->dataSegment.begin ;
        newh->end = newh->begin + __len - 1 ;
        newh->nextPtr = tmp ;
        __pcb->dataSegment.heap->nextPtr = newh ;

        return newh->begin ;
    }

    MyHeap * then = tmp->nextPtr ;
    while (then != NULL) {
        if (__len <= then->begin - tmp->end - 1) {
            __pcb->dataSegment.used += __len ;

            MyHeap * newh = (MyHeap *)malloc(sizeof(MyHeap)) ;
            newh->label = __label ;
            newh->begin = tmp->end + 1 ;
            newh->end = newh->begin + __len - 1 ;
            newh->nextPtr = then ;
            tmp->nextPtr = newh ;

            return newh->begin ;
        }

        tmp = then ;
        then = then->nextPtr ;
    }

    /* 分配位置在已分配的最后一个数据段之后 */
    if (__len <= __pcb->dataSegment.end - tmp->end) {
        __pcb->dataSegment.used += __len ;

        MyHeap * newh = (MyHeap *)malloc(sizeof(MyHeap)) ;
        newh->label = __label ;
        newh->begin = tmp->end + 1 ;
        newh->end = newh->begin + __len - 1 ;
        newh->nextPtr = NULL ;
        tmp->nextPtr = newh ;

        return newh->begin ;
    }

    /* 没有匹配的块 */
    return -1 ;
}

void defragment(PCB * __pcb)
{
    int moveto, movefrom, except ;
    unsigned char uchar ;
    char bufb[768] = "" ;
    char bufa[768] = "" ;

    moveto = __pcb->dataSegment.begin ;
    MyHeap * heap = __pcb->dataSegment.heap->nextPtr ;

    while (heap != NULL) {
        sprintf(bufb, "%s\t地址： %2d\t起： %4d\t止： %4d\n", bufb, heap->label, heap->begin, heap->end) ;

        movefrom = heap->begin ;
        if (movefrom > moveto) {
            except = movefrom - moveto ;
            //printf("label: %d\n", heap->label);
            heap->begin = moveto ;
            __pcb->dataSegment.used += except ;
            /* while (movefrom <= heap->end) {
                //printf("%d->%d  ", movefrom, moveto) ;
                readMemoryUnit(__pcb, heap->label, movefrom - heap->begin, &uchar);
                writeMemoryUnit(__pcb, heap->label, moveto - heap->begin, &uchar);
                moveto++ ;
                movefrom++ ;
            } */
            readMemoryUnit(__pcb, heap->label, movefrom - heap->begin, &uchar);
            writeMemoryUnit(__pcb, heap->label, moveto - heap->begin, uchar);

            heap->end -= except ;
            __pcb->dataSegment.used -= except ;
            //printf("\nafter move begin: %d  end: %d\n", heap->begin, heap->end) ;
        }

        sprintf(bufa, "%s\t地址： %2d\t起： %4d\t止： %4d\n", bufa, heap->label, heap->begin, heap->end) ;

        moveto = heap->end + 1 ;
        heap = heap->nextPtr ;
    }

    logPrintf(HEAP_LOG, "操作： 碎片整理\n进程PID： %d\n整理前：\n%s整理后：\n%s\n\n", __pcb->pid, bufb, bufa) ;
}

bool mallocDataSegment(PCB * __pcb, int __label, int __len)
{
    if (__pcb == NULL || __len <= 0) {
        setError(ILLEGAL_PARAMETER);
        return false ;
    }

    /* 释放原有label空间 */
    freeDataSegment(__pcb, __label) ;

    bool ret__ ;
    int allo ;
    allo = firstFit(__pcb, __label, __len) ;

    if (allo >= 0) {
        /* 分配成功 */

        ret__ = true ;
    }
    else {
        /* 没有足够空间 */
        if (allo == -2) {
            setError(NO_ENOUGH_HEAP_SPACE);
            ret__ = false ;
        }
        /* 适应失败，没有匹配的块 */
        else if (allo == -1) {
            /* 启用碎片整理算法 */
            defragment(__pcb) ;

            /* 再次尝试分配 */
            allo = firstFit(__pcb, __label, __len) ;
            if (allo >= 0) {
                ret__ = true ;
            }
            else {
                setError(DEFRAGMENT_FAILED);
                ret__ = false ;
            }
        }
    }

    if (ret__) {
        setMemoryUsed(__pcb, __len);
        logPrintf(HEAP_LOG, "操作： 分配内存\t\t成功\n\tPID： %d\t地址变量： %d\n\t起始地址： %d\t结束地址： %d\t长度： %d\n\n"
                        , __pcb->pid, __label, allo, allo+__len-1, __len) ;
    }
    else {
        logPrintf(HEAP_LOG, "操作： 分配内存\t\t失败： %s\n\tPID： %d\t地址变量： %d\t长度： %d\n\n"
                        , errorDesc[errorCode], __pcb->pid, __label, __len) ;
    }

    return ret__ ;
}
