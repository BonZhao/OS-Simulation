/**
 * MyOS\SystemApp.cpp
 *
 * Copyright (c) 2016 LyanQQ
 */

/**
 * Working log:
 * 2016/03/31 21:49		Created				by LyanQQ ;
 * 2016/03/31 22:28		1st time coding		by LyanQQ :
 * 		-Add execution unit.
 */

#include "ExeUnit.h"
#include "MemoryManagement.h"
#include "ProcessManagement.h"
#include "WorkLog.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <QDebug>

const char * insTypeDesc[INS_NUMBER + 1] = {
    "配置语句" ,
    "普通语句" ,
    "内存申请指令" ,
    "内存释放指令" ,
    "打印指令" ,
    "IO指令" ,
    "结束指令" ,
    "加法指令" ,
    "减法指令" ,
    "跳转指令" ,
    "条件跳转指令" ,
    "写存指令" ,
    "未知语句"
} ;

int processCounter = 0 ;			/* 程序计数器PC */

bool EURunning = false ;

extern PCB * runningProcess ;
extern int sectionSize ;
extern clock_t startTime ;
extern int timeSlice ;
extern HANDLE PCBQueueMutex ;
extern HANDLE runningProcessMutex ;
extern HANDLE runningProcessSemaphore ;
extern const char * errorDesc[] ;
extern ErrorType errorCode ;
extern bool LOG_OUTPUT;

void gotoMust(int offset)
{
    /**
     * 绝对转移，PC回跳offset个语句
     */

    processCounter -= 2*(offset + 1) ;
    if (processCounter < 0) {
        processCounter = 0 ;
    }
}

int calculateCPURatio(int __time)
{
    return 0 ;
}

bool getSingleInstruction(unsigned char * __ins)
{
    /**
     * 取下一条执行指令
     */

    if (runningProcess != NULL) {
        ProcessPageTable pt__ = runningProcess->pageTable ;
        int pgn__ = (int)(processCounter / sectionSize) ;
        int offset__ = processCounter - (pgn__ * sectionSize) ;

        if (pgn__ >= 0 && pgn__ <= pt__.number) {
            /* 缺页 */
            if (pt__.table[pgn__].isAvailable == false) {
                pt__.table[pgn__].frameNumber = VMToMemory(runningProcess->pid, pgn__);

                if (pt__.table[pgn__].frameNumber >= 0) {
                    pt__.table[pgn__].isAvailable = true ;
                }
                else {
                    return false ;
                }
            }

            /* 读取指令 */
            if (readMemoryByChar(runningProcess->pid, pt__.table[pgn__].frameNumber, offset__, __ins)) {


                /* PC加一 */
                processCounter ++ ;
                return true ;
            }
        }
        else {
            setError(ADDRESS_OVERFLOW);
        }
    }
    else {
        ;
    }

    return false ;
}

bool getInstruction(unsigned char * __left, unsigned char * __right)
{
    WaitForSingleObject(runningProcessMutex, INFINITE);

    /* 取指令首字节 */
    if (getSingleInstruction(__left)) {
        /* 取指令次字节 */
        if (getSingleInstruction(__right)) {

            if (LOG_OUTPUT) {
                logPrintf(EU_LOG, "操作： 取指令\n进程PID： %d\tPC： %d\t指令码： %d %d\n\n"
                        , runningProcess->pid, processCounter - 2, (*__left), (*__right));
            }

            ReleaseMutex(runningProcessMutex) ;
            return true ;
        }
        else {
            /* 回退PC */
            processCounter -- ;
        }
    }
    else {
        ;
    }

    if (LOG_OUTPUT) {
        logPrintf(EU_LOG, "操作： 取指令\n进程PID： %d\tPC： %d\t取指失败，逻辑地址越界\n\n"
                    , runningProcess->pid, processCounter);
    }

    /* 越界处理 */
    illegalAccess(runningProcess) ;

    ReleaseMutex(runningProcessMutex) ;
    return false ;
}

Statement decode(unsigned char __left, unsigned char __right)
{
    /**
     * 指令译码
     */

    Statement stmt ;
    unsigned char x = 16 ;

    int pre = __left / 32 ;

    switch (pre) {
        case 0 :
            stmt.type = CONFIG ;
            stmt.var_1 = __left % 32 ;
        break ;

        case 1 :
            stmt.type = NORMAL ;
            stmt.var_1 = __right ;
        break ;

        case 2 :
            if (__left & x) {
                stmt.type = MEM_FREE_ ;
                stmt.var_1 = __left % 16 ;
            }
            else {
                stmt.type = MEM_MALLOC ;
                stmt.var_1 = __left % 16 ;
                stmt.var_2 = __right ;
            }
        break ;

        case 3 :
            if (__left & x) {
                stmt.type = IO ;
                stmt.var_1 = __right ;
            }
            else {
                stmt.type = PRINT ;
                stmt.var_1 = __left % 16 ;
            }
        break ;

        case 4 :
            stmt.type = END ;
        break ;

        case 5 :
            if (__left & x) {
                stmt.type = SUB ;
                stmt.var_1 = __right / 16 ;
                stmt.var_2 = __right % 16 ;
            }
            else {
                stmt.type = ADD ;
                stmt.var_1 = __right / 16 ;
                stmt.var_2 = __right % 16 ;
            }
        break ;

        case 6 :
            if (__left & x) {
                stmt.type = C_JUMP ;
                stmt.var_1 = __left % 16 ;
                stmt.var_2 = __right ;
            }
            else {
                stmt.type = JUMP ;
                stmt.var_1 = __right ;
            }
        break ;

        case 7 :
            stmt.type = SAVE ;
            stmt.var_1 = __left % 16 ;
            stmt.var_2 = __right ;
        break ;

        default :
            ;
        break ;
    }

    return stmt ;
}

bool readMemoryUnit(PCB * __pcb, int __label, int __offset, unsigned char * __val)
{
    MyHeap * heap ;
    int offsetAll ;

    heap = findDataSegment(__pcb, __label) ;
    if (heap != NULL) {
        /* 计算总偏移量 */
        offsetAll = heap->begin + __offset ;
        /* 检查越界 */
        if (offsetAll <= heap->end) {
            ProcessPageTable pt__ = __pcb->pageTable ;
            int pgn__ = (int)(offsetAll / sectionSize) ;
            int offsetIn__ = offsetAll - (pgn__ * sectionSize) ;

            if (pgn__ >= 0 && pgn__ <= pt__.number) {
                /* 缺页 */
                if (pt__.table[pgn__].isAvailable == false) {
                    pt__.table[pgn__].frameNumber = VMToMemory(__pcb->pid, pgn__);

                    if (pt__.table[pgn__].frameNumber >= 0) {
                        pt__.table[pgn__].isAvailable = true ;
                    }
                    else {
                        return false ;
                    }
                }

                /* 读值 */
                if (readMemoryByChar(__pcb->pid, pt__.table[pgn__].frameNumber, offsetIn__, __val)) {
                    return true ;
                }
                else {
                    ;
                }
            }
            else {
                setError(ILLEGAL_ADDRESS_VISIT) ;
            }
        }
        else {
            setError(ILLEGAL_ADDRESS_VISIT) ;
        }
    }
    else {
        setError(ILLEGAL_ADDRESS_VISIT) ;
    }

    return false ;
}

bool writeMemoryUnit(PCB * __pcb, int __label, int __offset, unsigned char __val)
{
    MyHeap * heap ;
    int offsetAll ;

    heap = findDataSegment(__pcb, __label) ;
    if (heap != NULL) {
        /* 计算总偏移量 */
        offsetAll = heap->begin + __offset ;
        /* 检查越界 */
        if (offsetAll <= heap->end) {
            ProcessPageTable pt__ = __pcb->pageTable ;
            int pgn__ = (int)(offsetAll / sectionSize) ;
            int offsetIn__ = offsetAll - (pgn__ * sectionSize) ;

            if (pgn__ >= 0 && pgn__ <= pt__.number) {
                /* 缺页 */
                if (pt__.table[pgn__].isAvailable == false) {
                    pt__.table[pgn__].frameNumber = VMToMemory(__pcb->pid, pgn__);

                    if (pt__.table[pgn__].frameNumber >= 0) {
                        pt__.table[pgn__].isAvailable = true ;
                    }
                    else {
                        return false ;
                    }
                }

                /* 写值 */
                if (writeMemoryByChar(__pcb->pid, pt__.table[pgn__].frameNumber, offsetIn__, __val)) {
                    return true ;
                }
                else {
                    ;
                }
            }
            else {
                setError(ILLEGAL_ADDRESS_VISIT) ;
            }
        }
        else {
            setError(ILLEGAL_ADDRESS_VISIT) ;
        }
    }
    else {
        setError(ILLEGAL_ADDRESS_VISIT) ;
    }

    return false ;
}

void execute(unsigned char __left, unsigned char __right)
{
    /**
     * 译码和执行部件，传入单字节指令
     * 译码后执行对应的程序
     */

    Statement st__ ;
    unsigned char uchar1, uchar2 ;
    bool isIllegal = false , isEnd = false ;
    int illegalAddr ;
    char buf[1024] ;

    st__ = decode(__left, __right) ;

    WaitForSingleObject(runningProcessMutex, INFINITE);
    //printf("type:%d\tvar1:%d\tvar2:%d\n", st__.type, st__.var_1, st__.var_2);

    if (runningProcess == NULL) {
        ReleaseMutex(runningProcessMutex) ;
        return ;
    }

    sprintf(buf, "操作： 执行指令\n进程PID： %d\tPC： %d\n\t类型： %s\n"
            , runningProcess->pid, processCounter - 2, insTypeDesc[st__.type]) ;

    setCPURatio(runningProcess, 0);
    switch (st__.type) {
        /* 配置语句 */
        case CONFIG :
            /* 跳过，不执行 */
            sprintf(buf, "%s\t配置值： %d", buf, st__.var_1) ;
        break ;

        /* 普通语句 */
        case NORMAL :
            sprintf(buf, "%s\t执行时长： %d ms", buf, st__.var_1) ;

            setCPURatio(runningProcess, st__.var_1 / 3);

            Sleep(st__.var_1) ;
        break ;

        /* 申请内存语句 */
        case MEM_MALLOC :
            sprintf(buf, "%s\t地址变量： %d\t长度： %d", buf, st__.var_1, st__.var_2);

            if (mallocDataSegment(runningProcess, st__.var_1, st__.var_2)) {
                ;
            }
            else {
                sprintf(buf, "%s\n\t内存分配失败： %s", buf, errorDesc[errorCode]);
            }
        break ;

        /* 释放内存语句 */
        case MEM_FREE_ :
            sprintf(buf, "%s\t地址变量： %d", buf, st__.var_1) ;

            freeDataSegment(runningProcess, st__.var_1) ;
        break ;

        /* 打印语句 */
        case PRINT :
            sprintf(buf, "%s\t地址变量： %d", buf, st__.var_1) ;
            if (readMemoryUnit(runningProcess, st__.var_1, 0, &uchar1)) {
                logPrintf(CONSOLE_LOG, "PID: %d\tVALUE: %d\n\n", runningProcess->pid, (char)uchar1) ;

                //printf("pid: %d\tvalue: %d\n", runningProcess->pid, (char)uchar1) ;
                qDebug() << "pid: "<< runningProcess->pid << " value: "<<(int)(char)uchar1;
                sprintf(buf, "%s\t值： %d", buf, (char)uchar1) ;
            }
            else {
                /* 非法访问 */
                illegalAddr = st__.var_1 ;
                isIllegal = true ;
            }
        break ;

        /* 其他IO语句 */
        case IO :
            sprintf(buf, "%s\tIO时长： %d ms", buf, st__.var_1) ;

            if (!IORequest(st__.var_1 * 50)) {
                sprintf(buf, "%s\n\t%s", buf, errorDesc[errorCode]) ;
            }
        break ;

        /* 结束语句 */
        case END :
            isEnd = true ;
        break ;

        /* 加减法语句 */
        case ADD : case SUB :
            sprintf(buf, "%s\t操作数A： %d\t操作数B： %d", buf, st__.var_1, st__.var_2);

            if ( readMemoryUnit(runningProcess, st__.var_1, 0, &uchar1) ) {
                if ( readMemoryUnit(runningProcess, st__.var_2, 0, &uchar2) ) {
                    sprintf(buf, "%s\n\t值A： %d\t值B： %d", buf, (char)uchar1, (char)uchar2);

                    if (st__.type == ADD) {
                        uchar1 += uchar2 ;
                    }
                    else {
                        uchar1 -= uchar2 ;
                    }

                    if (writeMemoryUnit(runningProcess, st__.var_1, 0, uchar1)) {
                        sprintf(buf, "%s\n\t运算结果： %d", buf, (char)uchar1);
                    }
                    else {
                        /* 非法访问 */
                        illegalAddr = st__.var_1 ;
                        isIllegal = true ;
                    }
                }
                else {
                    /* 非法访问 */
                    illegalAddr = st__.var_2 ;
                    isIllegal = true ;
                }
            }
            else {
                /* 非法访问 */
                illegalAddr = st__.var_1 ;
                isIllegal = true ;
            }
        break ;

        /* 跳转语句 */
        case JUMP :
            sprintf(buf, "%s\t跳转语句数： %d", buf, st__.var_1) ;

            gotoMust(st__.var_1) ;
        break ;

        /* 条件跳转语句 */
        case C_JUMP :
            sprintf(buf, "%s\t条件变量： %d\t跳转语句数： %d", buf, st__.var_1, st__.var_2) ;

            if (readMemoryUnit(runningProcess, st__.var_1, 0, &uchar1)) {
                sprintf(buf, "%s\n\t条件变量值： %d", buf, (char)uchar1) ;

                if (uchar1 != 0) {
                    gotoMust(st__.var_2) ;
                }
            }
            else {
                /* 非法访问 */
                illegalAddr = st__.var_1 ;
                isIllegal = true ;
            }
        break ;

        /* 写存语句 */
        case SAVE :
            sprintf(buf, "%s\t地址变量： %d\t立即数： %d", buf, st__.var_1, st__.var_2) ;
            uchar1 = st__.var_2 ;

            if (writeMemoryUnit(runningProcess, st__.var_1, 0, uchar1)) {
                ;
            }
            else {
                /* 非法访问 */
                illegalAddr = st__.var_1 ;
                isIllegal = true ;
            }
        break ;

        default :
            ;
        break ;
    }

    ReleaseMutex(runningProcessMutex) ;

    if (isEnd) {
        setProcessState(runningProcess, TERMINATED) ;
    }

    if (isIllegal) {
        sprintf(buf, "%s\n事件： 无效地址空间访问\t地址变量： %d", buf, illegalAddr) ;
        logPrintf(CONSOLE_LOG, "PID: %d\t程序异常终止： %s\n\n", runningProcess->pid, errorDesc[errorCode]) ;
        qDebug() << "pid: "<< runningProcess->pid << "\t Process crashed" ;
        illegalAccess(runningProcess) ;
    }

    logPrintf(EU_LOG, "%s\n\n", buf) ;
}

void CPU()
{
    unsigned char left, right ;

    while (EURunning) {
        WaitForSingleObject(runningProcessSemaphore, INFINITE) ;

        while (runningProcess != NULL) {
            /* 取指令 */
            if (getInstruction(&left, &right)) {
                /* 执行这条指令 */
                execute(left, right) ;

                /* 时间片耗尽 */
                if (clock()-startTime > timeSlice) {
                    finishTimeSlice() ;
                }
            }
        }
    }
}

void startExeUnit()
{
    EURunning = true ;

    CPU() ;
}

void shutDownExeUnit()
{
    EURunning = false ;
}
