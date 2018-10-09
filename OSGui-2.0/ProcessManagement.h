/**
 * MyOS\ProcessManagement.h
 *
 * Copyright (c) 2016 LyanQQ
 */

/**
 * Working log:
 * 2016/04/01 16:33		Created				by LyanQQ ;
 * 2016/04/01 18:11		1st time coding		by LyanQQ :
 * 		-PCB data structure completed.
 */

#ifndef PROCESSMANAGEMENT_H
#define PROCESSMANAGEMENT_H

#include "Basic.h"

#define MAX_PID 100000
#define MY_MAX_PRIORITY 108

#define MAX_PROCESS_NUMBER 512

/**
 * PCB数据结构
 */
/* 进程内部标识符 */
typedef int ProcessID ;

/* 进程外部标识符 */
struct ProcessExternalInfo {
    char * name ;					/* 进程名称，通常为exe文件名 */
    char * path ;					/* 进程文件路径 */
} ;

/* 进程处理机状态信息 */
struct ProcessExecutionInfo {
    int processCounter ;			/* 程序计数器 */
    int codeMemory ;				/* 代码段占用的内存 */
    int memoryUsed ;				/* 使用中的内存 */
    int CPUUsed ;					/* CPU使用率 */
} ;

/* 进程状态 */
enum ProcessState {NEW				/* 新建 */
                , READY				/* 就绪 */
                , RUNNING			/* 运行 */
                , WAITING			/* 等待 */
                , TERMINATED} ;		/* 终止 */

/* 进程优先级 */
typedef int ProcessPriority ;

/* 进程运行时间记录 */
struct ProcessTime {
    TimeStamp createTime ;			/* 进程创建时间 */
    int lastTime ;
    int runningTime ;				/* 进程已经执行的总时长（毫秒） */
    int waitingTime ;				/* 进程等待执行的总时长（毫秒） */
} ;

/* 事件，进程进入阻塞（等待、就绪）队列的原因 */
enum ProcessEvent { NEW_PROCESS				/* 新建进程 */
                , RUN_OUT_OF_TIME_SLICE		/* 时间片耗尽 */
                , WAITING_FOR_IO			/* 等待IO */
                , IO_FINISH} ;

/* 进程调度信息 */
struct ProcessSchedulingInfo {
    ProcessState state ;				/* 进程状态 */
    ProcessPriority priority ;			/* 进程固定优先级 */
    ProcessPriority dynamicPriority ;	/* 进程动态优先级 */
    ProcessTime time ;					/* 进程时间记录 */
    ProcessEvent event ;				/* 进程事件 */
} ;

/* 进程页 */
struct ProcessPage {
    bool isAvailable ;				/* 该进程页是否有效 */
    int frameNumber ;				/* 该进程页占用的帧号 */
} ;

/* 进程页表 */
struct ProcessPageTable {
    int number ;
    ProcessPage * table ;
} ;

/* 进程打开文件列表 */
struct ProcessOpenFileList {
    char * filePath ;				/* 打开文件的路径 */
    ProcessOpenFileList * nextPtr ;
} ;

struct MyHeap {
    int label ;
    int begin ;
    int end ;
    MyHeap * nextPtr ;
} ;

/* 进程数据段 */
struct ProcessDataSegment {
    int begin ;					/* 数据段起始 */
    int end ;					/* 数据段结束 */
    int used ;					/* 已分配的数据段 */
    MyHeap * heap ;				/* 堆信息 */
} ;

/* PCB */
struct ProcessControlBlock {
    ProcessID pid ;							/* 进程标识符 */
    ProcessExternalInfo externalInfo ;		/* 外部标识符 */
    ProcessExecutionInfo exeInfo ;			/* 执行信息 */
    ProcessSchedulingInfo schedInfo ;		/* 调度信息 */
    ProcessPageTable pageTable ;			/* 进程页表 */
    ProcessOpenFileList * ofl ;				/* 文件打开列表 */
    ProcessDataSegment dataSegment ;		/* 数据段 */
} ;
typedef ProcessControlBlock PCB ;

/* 进程PCB队列 */
struct PCBQueue {
    PCB * pcb ;
    PCBQueue * nextPtr ;
} ;

/* 操作系统维护的进程打开列表 */
struct _OpenFile {
    char * filePath ;
    int counter ;
} ;

struct OpenFileList {
    _OpenFile of ;
    OpenFileList * nextPtr ;
} ;
typedef OpenFileList OFL ;

/* 文件类型及其后缀 */
enum FileType {LOG_FILE, RUN_FILE} ;

/* 环境函数 */
void initProcessManagement();
void destroyProcessManagement();

/* PCB操作 */
void initPCB(PCB * __pcb);
void destroyPCB(PCB * __pcb);
void newPCB(PCB * __pcb, ProcessID __pid, const char * __name, const char * __path, ProcessPriority __pri, int __pgnum) ;
PCB * findPCB(ProcessID __pid);
PCB * findPCB(ProcessState __state, ProcessID __pid);
bool setProcessState(PCB * __pcb, ProcessState __state);
void setCPURatio(PCB * __pcb, int __ra);
void setMemoryUsed(PCB * __pcb, int __ra);
void addToQueue(PCB * __pcb);
bool deleteFromQueue(ProcessID __pid);
int getQueueLength(ProcessState __state);
bool showPCB(ProcessID __pid);
void showPCBQueue();

/* 文件打开列表操作 */
//void initOpenFile(OpenFile * __of);
//void destroyOpenFile(OpenFile * __of);
void addOpenFile(const char * __path);
bool closeOpenFile(const char * __path);
bool checkOpenFile(const char * __path);
void showOFL();

/* 页表操作 */
void pageUnvailable(ProcessID __pid, int __pgn);

/* 调度函数 */
//ProcessPriority degeneration(ProcessPriority __pri);
void finishTimeSlice();
PCB * dispatchProcess();
void dispatch();
void priorityAdjust(PCB * __pcb);

/* EU单元函数 */
bool IORequest (int __time);
void IOFinish(ProcessID __pid);
void illegalAccess(PCB * __pcb);

/* 数据段 */
MyHeap * findDataSegment(PCB * __pcb, int __label) ;
bool freeDataSegment(PCB * __pcb, int __label) ;
int firstFit(PCB * __pcb, int __label, int __len) ;
void defragment(PCB * __pcb) ;
bool mallocDataSegment(PCB * __pcb, int __label, int __len) ;

/* 进程API */
bool newProcess(const char * __path);
bool deleteProcess(ProcessID __pid);

#endif
