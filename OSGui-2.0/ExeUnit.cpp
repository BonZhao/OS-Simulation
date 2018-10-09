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
    "�������" ,
    "��ͨ���" ,
    "�ڴ�����ָ��" ,
    "�ڴ��ͷ�ָ��" ,
    "��ӡָ��" ,
    "IOָ��" ,
    "����ָ��" ,
    "�ӷ�ָ��" ,
    "����ָ��" ,
    "��תָ��" ,
    "������תָ��" ,
    "д��ָ��" ,
    "δ֪���"
} ;

int processCounter = 0 ;			/* ���������PC */

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
     * ����ת�ƣ�PC����offset�����
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
     * ȡ��һ��ִ��ָ��
     */

    if (runningProcess != NULL) {
        ProcessPageTable pt__ = runningProcess->pageTable ;
        int pgn__ = (int)(processCounter / sectionSize) ;
        int offset__ = processCounter - (pgn__ * sectionSize) ;

        if (pgn__ >= 0 && pgn__ <= pt__.number) {
            /* ȱҳ */
            if (pt__.table[pgn__].isAvailable == false) {
                pt__.table[pgn__].frameNumber = VMToMemory(runningProcess->pid, pgn__);

                if (pt__.table[pgn__].frameNumber >= 0) {
                    pt__.table[pgn__].isAvailable = true ;
                }
                else {
                    return false ;
                }
            }

            /* ��ȡָ�� */
            if (readMemoryByChar(runningProcess->pid, pt__.table[pgn__].frameNumber, offset__, __ins)) {


                /* PC��һ */
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

    /* ȡָ�����ֽ� */
    if (getSingleInstruction(__left)) {
        /* ȡָ����ֽ� */
        if (getSingleInstruction(__right)) {

            if (LOG_OUTPUT) {
                logPrintf(EU_LOG, "������ ȡָ��\n����PID�� %d\tPC�� %d\tָ���룺 %d %d\n\n"
                        , runningProcess->pid, processCounter - 2, (*__left), (*__right));
            }

            ReleaseMutex(runningProcessMutex) ;
            return true ;
        }
        else {
            /* ����PC */
            processCounter -- ;
        }
    }
    else {
        ;
    }

    if (LOG_OUTPUT) {
        logPrintf(EU_LOG, "������ ȡָ��\n����PID�� %d\tPC�� %d\tȡָʧ�ܣ��߼���ַԽ��\n\n"
                    , runningProcess->pid, processCounter);
    }

    /* Խ�紦�� */
    illegalAccess(runningProcess) ;

    ReleaseMutex(runningProcessMutex) ;
    return false ;
}

Statement decode(unsigned char __left, unsigned char __right)
{
    /**
     * ָ������
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
        /* ������ƫ���� */
        offsetAll = heap->begin + __offset ;
        /* ���Խ�� */
        if (offsetAll <= heap->end) {
            ProcessPageTable pt__ = __pcb->pageTable ;
            int pgn__ = (int)(offsetAll / sectionSize) ;
            int offsetIn__ = offsetAll - (pgn__ * sectionSize) ;

            if (pgn__ >= 0 && pgn__ <= pt__.number) {
                /* ȱҳ */
                if (pt__.table[pgn__].isAvailable == false) {
                    pt__.table[pgn__].frameNumber = VMToMemory(__pcb->pid, pgn__);

                    if (pt__.table[pgn__].frameNumber >= 0) {
                        pt__.table[pgn__].isAvailable = true ;
                    }
                    else {
                        return false ;
                    }
                }

                /* ��ֵ */
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
        /* ������ƫ���� */
        offsetAll = heap->begin + __offset ;
        /* ���Խ�� */
        if (offsetAll <= heap->end) {
            ProcessPageTable pt__ = __pcb->pageTable ;
            int pgn__ = (int)(offsetAll / sectionSize) ;
            int offsetIn__ = offsetAll - (pgn__ * sectionSize) ;

            if (pgn__ >= 0 && pgn__ <= pt__.number) {
                /* ȱҳ */
                if (pt__.table[pgn__].isAvailable == false) {
                    pt__.table[pgn__].frameNumber = VMToMemory(__pcb->pid, pgn__);

                    if (pt__.table[pgn__].frameNumber >= 0) {
                        pt__.table[pgn__].isAvailable = true ;
                    }
                    else {
                        return false ;
                    }
                }

                /* дֵ */
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
     * �����ִ�в��������뵥�ֽ�ָ��
     * �����ִ�ж�Ӧ�ĳ���
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

    sprintf(buf, "������ ִ��ָ��\n����PID�� %d\tPC�� %d\n\t���ͣ� %s\n"
            , runningProcess->pid, processCounter - 2, insTypeDesc[st__.type]) ;

    setCPURatio(runningProcess, 0);
    switch (st__.type) {
        /* ������� */
        case CONFIG :
            /* ��������ִ�� */
            sprintf(buf, "%s\t����ֵ�� %d", buf, st__.var_1) ;
        break ;

        /* ��ͨ��� */
        case NORMAL :
            sprintf(buf, "%s\tִ��ʱ���� %d ms", buf, st__.var_1) ;

            setCPURatio(runningProcess, st__.var_1 / 3);

            Sleep(st__.var_1) ;
        break ;

        /* �����ڴ���� */
        case MEM_MALLOC :
            sprintf(buf, "%s\t��ַ������ %d\t���ȣ� %d", buf, st__.var_1, st__.var_2);

            if (mallocDataSegment(runningProcess, st__.var_1, st__.var_2)) {
                ;
            }
            else {
                sprintf(buf, "%s\n\t�ڴ����ʧ�ܣ� %s", buf, errorDesc[errorCode]);
            }
        break ;

        /* �ͷ��ڴ���� */
        case MEM_FREE_ :
            sprintf(buf, "%s\t��ַ������ %d", buf, st__.var_1) ;

            freeDataSegment(runningProcess, st__.var_1) ;
        break ;

        /* ��ӡ��� */
        case PRINT :
            sprintf(buf, "%s\t��ַ������ %d", buf, st__.var_1) ;
            if (readMemoryUnit(runningProcess, st__.var_1, 0, &uchar1)) {
                logPrintf(CONSOLE_LOG, "PID: %d\tVALUE: %d\n\n", runningProcess->pid, (char)uchar1) ;

                //printf("pid: %d\tvalue: %d\n", runningProcess->pid, (char)uchar1) ;
                qDebug() << "pid: "<< runningProcess->pid << " value: "<<(int)(char)uchar1;
                sprintf(buf, "%s\tֵ�� %d", buf, (char)uchar1) ;
            }
            else {
                /* �Ƿ����� */
                illegalAddr = st__.var_1 ;
                isIllegal = true ;
            }
        break ;

        /* ����IO��� */
        case IO :
            sprintf(buf, "%s\tIOʱ���� %d ms", buf, st__.var_1) ;

            if (!IORequest(st__.var_1 * 50)) {
                sprintf(buf, "%s\n\t%s", buf, errorDesc[errorCode]) ;
            }
        break ;

        /* ������� */
        case END :
            isEnd = true ;
        break ;

        /* �Ӽ������ */
        case ADD : case SUB :
            sprintf(buf, "%s\t������A�� %d\t������B�� %d", buf, st__.var_1, st__.var_2);

            if ( readMemoryUnit(runningProcess, st__.var_1, 0, &uchar1) ) {
                if ( readMemoryUnit(runningProcess, st__.var_2, 0, &uchar2) ) {
                    sprintf(buf, "%s\n\tֵA�� %d\tֵB�� %d", buf, (char)uchar1, (char)uchar2);

                    if (st__.type == ADD) {
                        uchar1 += uchar2 ;
                    }
                    else {
                        uchar1 -= uchar2 ;
                    }

                    if (writeMemoryUnit(runningProcess, st__.var_1, 0, uchar1)) {
                        sprintf(buf, "%s\n\t�������� %d", buf, (char)uchar1);
                    }
                    else {
                        /* �Ƿ����� */
                        illegalAddr = st__.var_1 ;
                        isIllegal = true ;
                    }
                }
                else {
                    /* �Ƿ����� */
                    illegalAddr = st__.var_2 ;
                    isIllegal = true ;
                }
            }
            else {
                /* �Ƿ����� */
                illegalAddr = st__.var_1 ;
                isIllegal = true ;
            }
        break ;

        /* ��ת��� */
        case JUMP :
            sprintf(buf, "%s\t��ת������� %d", buf, st__.var_1) ;

            gotoMust(st__.var_1) ;
        break ;

        /* ������ת��� */
        case C_JUMP :
            sprintf(buf, "%s\t���������� %d\t��ת������� %d", buf, st__.var_1, st__.var_2) ;

            if (readMemoryUnit(runningProcess, st__.var_1, 0, &uchar1)) {
                sprintf(buf, "%s\n\t��������ֵ�� %d", buf, (char)uchar1) ;

                if (uchar1 != 0) {
                    gotoMust(st__.var_2) ;
                }
            }
            else {
                /* �Ƿ����� */
                illegalAddr = st__.var_1 ;
                isIllegal = true ;
            }
        break ;

        /* д����� */
        case SAVE :
            sprintf(buf, "%s\t��ַ������ %d\t�������� %d", buf, st__.var_1, st__.var_2) ;
            uchar1 = st__.var_2 ;

            if (writeMemoryUnit(runningProcess, st__.var_1, 0, uchar1)) {
                ;
            }
            else {
                /* �Ƿ����� */
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
        sprintf(buf, "%s\n�¼��� ��Ч��ַ�ռ����\t��ַ������ %d", buf, illegalAddr) ;
        logPrintf(CONSOLE_LOG, "PID: %d\t�����쳣��ֹ�� %s\n\n", runningProcess->pid, errorDesc[errorCode]) ;
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
            /* ȡָ�� */
            if (getInstruction(&left, &right)) {
                /* ִ������ָ�� */
                execute(left, right) ;

                /* ʱ��Ƭ�ľ� */
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
