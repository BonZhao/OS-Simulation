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

char * stateDesc[5] = {"�½�", "����", "����", "�ȴ�", "��ֹ"} ;
char * eventDesc[4] = {"�½����Ľ���", "ʱ��Ƭ�ľ�", "�ȴ�I/Oִ�����", "I/Oִ�����"} ;
char * FileEx[2] = {"log", "run"} ;
PCBQueue * processQueue = NULL ;			/* ���̶��� */
OFL * openFileList = NULL ;					/* ���ļ��б� */
int totalDegenerationCount = 0 ;			/* �ܹ�ִ�е��˻����� */
int totalProcessNumber = 0 ;				/* �������� */
int priorityZeroCount = 0 ;

PCB * runningProcess ;						/* ��ǰ���ڱ�ִ�еĽ��� */
int timeSlice = 0 ;							/* ��ǰִ�н��̵�ʱ��Ƭ */
clock_t startTime ;							/* ���̿�ʼִ�е�ʱ�� */

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
     * ��ʼ��PCB
     */

    if (__pcb == NULL) {
        return ;
    }

    __pcb->pid = -1 ;

    /* ��ʼ���ⲿ��ʶ�� */
    __pcb->externalInfo.name = NULL ;
    __pcb->externalInfo.path = NULL ;

    /* ��ʼ��ִ����Ϣ */
    __pcb->exeInfo.processCounter = 0 ;
    __pcb->exeInfo.codeMemory = 0 ;
    __pcb->exeInfo.memoryUsed = 0 ;
    __pcb->exeInfo.CPUUsed = 0 ;

    /* ��ʼ��������Ϣ */
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

    /* ��ʼ��ҳ�� */
    __pcb->pageTable.number = 0 ;
    __pcb->pageTable.table = NULL ;

    /* ��ʼ���ļ����б� */
    __pcb->ofl = (ProcessOpenFileList *)malloc(sizeof(ProcessOpenFileList)) ;
    __pcb->ofl->filePath = NULL ;
    __pcb->ofl->nextPtr = NULL ;

    /* ��ʼ�����ݶ� */
    __pcb->dataSegment.heap = (MyHeap *)malloc(sizeof(MyHeap)) ;
    __pcb->dataSegment.heap->nextPtr = NULL ;
}

void freeProcessOpenFileList(ProcessOpenFileList * __ofl)
{
    /**
     * �ݻٽ����ļ����б�
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
     * �ݻ�һ��PCB������������������ڴ�ռ�
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

    /* ��ѯ���ļ��Ƿ��ѱ��������̴� */
    ofl__ = findOpenFile(__path) ;

    /* �������½�һ�����ļ��ڵ� */
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
    /* ���ǣ����������һ */
    else {
        ofl__->of.counter++ ;
    }
}

bool closeOpenFile(const char * __path)
{
    OFL * ofl__ ;

    /* ��ѯ���ļ��Ƿ���ڴ򿪼�¼ */
    ofl__ = findOpenFile(__path) ;

    /* �����򷵻� */
    if (ofl__ == NULL) {
        setError(SYSTEM_ERROR);
        return false ;
    }
    else {
        /* �������Լ�һ */
        ofl__->of.counter-- ;

        /* ����ֵΪ0������ɾ���ýڵ� */
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

            /* δ֪���� */
            setError(UNKNOW_ERROR);
            return false ;
        }

        return true ;
    }
}

bool checkOpenFile(const char * __path)
{
    /**
     * �����ļ���Ŀ����ָ���ļ�û�б��򿪷���true
     */

    OFL * ofl__ = openFileList->nextPtr ;

    while (ofl__ != NULL) {
        /* ·����ͬ�Ҽ���������0 */
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

    puts("���ļ��б�Open File List����\n");

    if (ofl__ == NULL) {
        puts("���ļ��б�Ϊ��") ;
        return ;
    }

    while (ofl__ != NULL) {
        printf("�ļ�·��:%s\t�򿪽�����:%d\n", ofl__->of.filePath, ofl__->of.counter);
        ofl__ = ofl__->nextPtr ;
    }
}

void initProcessManagement()
{
    /**
     * ��ʼ�����̹���ģ��
     * ������ʼ�����̶��к��ļ����б�
     */

    /* ��ʼ�����̶��У�����ͷ��� */
    processQueue = (PCBQueue *)malloc(sizeof(PCBQueue)) ;

    //initPCB(processQueue->pcb) ;
    processQueue->pcb = NULL ;
    processQueue->nextPtr = NULL ;

    /* ��ʼ���ļ����б�����ͷ��� */
    openFileList = (OFL *)malloc(sizeof(OFL)) ;

    initOpenFile(&(openFileList->of)) ;
    openFileList->nextPtr = NULL ;

    /* ��������ִ�еĽ��� */
    runningProcess = NULL ;

    /* �����˻��ܴ��� */
    totalDegenerationCount = 0 ;

    /* ���ý��̼�����Ϣ */
    totalProcessNumber = 0 ;
    priorityZeroCount = 0 ;

    /* PCB�б������ */
    //PCBQueueMutex = CreateMutex(NULL, FALSE, "PCBQueueMutex") ;
    PCBQueueMutex = CreateMutex(NULL, FALSE, NULL) ;

    /* runningProcess������ */
    runningProcessMutex = CreateMutex(NULL, FALSE, NULL) ;

    /* ָ�ɽ��̵��ź��� */
    //dispathSemaphore = CreateSemaphore(NULL, 1, 1, "dispathSemaphore") ;
    dispathSemaphore = CreateSemaphore(NULL, 1, 1, NULL) ;

    /* ִ�н��̵��ź��� */
    //runningProcessSemaphore = CreateSemaphore(NULL, 1, 1, "runningProcessSemaphore") ;
    runningProcessSemaphore = CreateSemaphore(NULL, 0, 1, NULL) ;

    /* �������̼����ź��� */
    //readyProcessSemaphore = CreateSemaphore(NULL, 0, MAX_PROCESS_NUMBER, "readyProcessSemaphore") ;
    readyProcessSemaphore = CreateSemaphore(NULL, 0, MAX_PROCESS_NUMBER, NULL) ;

    /* ���ý������ȼ��ź��� */
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
     * �½�һ��PCB��������ҵ����̶����У���ʱPCB��״̬��NEW
     */

    /* ��ʼ�� */
    initPCB(__pcb) ;

    /* ��¼PID */
    __pcb->pid = __pid ;

    /* ��¼�ⲿ��ʶ�� */
    __pcb->externalInfo.name = (char *)malloc(sizeof(char) * (strlen(__name)+1)) ;
    strcpy(__pcb->externalInfo.name, __name);
    __pcb->externalInfo.path = (char *)malloc(sizeof(char) * (strlen(__path)+1)) ;
    strcpy(__pcb->externalInfo.path, __path);

    __pcb->schedInfo.state = NEW ;
    __pcb->schedInfo.event = NEW_PROCESS ;

    /* ��¼�̶����ȼ� */
    __pcb->schedInfo.priority = __pri ;
    __pcb->schedInfo.dynamicPriority = __pri ;

    /* �½�ҳ�� */
    __pcb->pageTable.number = __pgnum ;
    __pcb->pageTable.table = (ProcessPage *)malloc(sizeof(ProcessPage) * __pgnum) ;
    for (int i = 0; i <= __pgnum - 1; i++) {
        __pcb->pageTable.table[i].isAvailable = false ;
        __pcb->pageTable.table[i].frameNumber = 0 ;
    }

    /* ��¼����γ��� */
    FileStream * fs__ ;

    fs__ = openFile(__path);
    if (fs__ != NULL) {
        __pcb->exeInfo.codeMemory = getFileLength(fs__) ;
        closeFile(fs__);
    }

    /* ��ʼ�����ݶ���Ϣ */
    __pcb->dataSegment.begin = __pcb->exeInfo.codeMemory ;
    __pcb->dataSegment.end = __pgnum * sectionSize - 1 ;
    __pcb->dataSegment.used = 0 ;

    /* �����ļ����б� */
    /* ȱʡ */
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

            /* �ۼ�ִ��ʱ�� */
            __pcb->schedInfo.time.runningTime += clock() - __pcb->schedInfo.time.lastTime ;

            /* �ͷ�ִ�н�������׼����ǲ�½��� */
            ReleaseSemaphore(dispathSemaphore, 1, NULL);
        }
        else {
            /* �ۼӵȴ�ʱ�� */
            __pcb->schedInfo.time.waitingTime += clock() - __pcb->schedInfo.time.lastTime ;

            if (__sn == READY) {
                /* �������̼�1 */
                WaitForSingleObject(readyProcessSemaphore, INFINITE) ;
            }
        }

        /* ������־��� */
        if (LOG_OUTPUT) {
            char buf[30] ;
            logPrintf(PCB_QUEUE_LOG, "����PID��%d\nԭ���У�%s\tת�Ƶ����У�%s\n��������ֹ��\n\t���̴���ʱ�䣺%s\n\t��ִ��ʱ����%d ms\t�ܵȴ�ʱ����%d ms\n\n"
                                ,__pcb->pid, stateDesc[__sn], stateDesc[__st], logTime(buf, __pcb->schedInfo.time.createTime), __pcb->schedInfo.time.runningTime, __pcb->schedInfo.time.waitingTime);
        }

        ReleaseMutex(PCBQueueMutex) ;

        /* ɾ������ */
        deleteProcess(__pcb->pid) ;

        return true ;
    }

    switch (__sn) {
        /* ��ǰ״̬Ϊ NEW */
        case NEW :
            if (__st == READY) {
                WaitForSingleObject(PCBQueueMutex, INFINITE);

                __pcb->schedInfo.state = READY ;

                priorityAdjust(__pcb);

                /* �������̼�1 */
                ReleaseSemaphore(readyProcessSemaphore, 1, NULL);

                ReleaseMutex(PCBQueueMutex);
                ret__ = true ;
            }
            else {
                ret__ = false ;
            }
        break ;

        /* ��ǰ״̬Ϊ READY */
        case READY :
            if (__st == RUNNING) {
                WaitForSingleObject(PCBQueueMutex, INFINITE);

                __pcb->schedInfo.state = RUNNING ;

                /* �ۼӵȴ�ʱ�� */
                __pcb->schedInfo.time.waitingTime += clock() - __pcb->schedInfo.time.lastTime ;
                __pcb->schedInfo.time.lastTime = clock() ;

                /* �������̼�1 */
                WaitForSingleObject(readyProcessSemaphore, INFINITE) ;

                ReleaseMutex(PCBQueueMutex);
                ret__ = true ;
            }
            else {
                ret__ = false ;
            }
        break ;

        /* ��ǰ״̬Ϊ RUNNING */
        case RUNNING :
            if (__st == WAITING || __st == READY) {
                WaitForSingleObject(PCBQueueMutex, INFINITE);

                /* ִ�н����ÿ� */
                WaitForSingleObject(runningProcessMutex, INFINITE) ;
                setCPURatio(runningProcess, 0);
                runningProcess = NULL ;
                ReleaseMutex(runningProcessMutex) ;

                __pcb->schedInfo.state = __st ;

                /* �ۼ�ִ��ʱ�� */
                __pcb->schedInfo.time.runningTime += clock() - __pcb->schedInfo.time.lastTime ;
                __pcb->schedInfo.time.lastTime = clock() ;

                /* ��������������ֵ */
                __pcb->exeInfo.processCounter = processCounter ;

                if (__st == READY) {
                    /* �������̼�1 */
                    ReleaseSemaphore(readyProcessSemaphore, 1, NULL);
                }

                /* �ͷ�ִ�н�������׼����ǲ�½��� */
                ReleaseSemaphore(dispathSemaphore, 1, NULL);

                ReleaseMutex(PCBQueueMutex);
                ret__ = true ;
            }
            else {
                ret__ = false ;
            }
        break ;

        /* ��ǰ״̬Ϊ WAITING */
        case WAITING :
            if (__st == READY) {
                WaitForSingleObject(PCBQueueMutex, INFINITE);

                __pcb->schedInfo.state = __st ;

                /* �������̼�1 */
                ReleaseSemaphore(readyProcessSemaphore, 1, NULL);

                ReleaseMutex(PCBQueueMutex);
                ret__ = true ;
            }
            else {
                ret__ = false ;
            }
        break ;

        /* ��ǰ״̬Ϊ TERMINATED */
        case TERMINATED :
            ret__ = false ;
        break ;

        /* ����״̬ */
        default:
            ret__ = false ;
        break ;
    }

    /* ������־��� */
    if (LOG_OUTPUT) {
        char buf[30] ;
        logPrintf(PCB_QUEUE_LOG, "����PID��%d\nԭ���У�%s\tת�Ƶ����У�%s\n���飺\n\t���̴���ʱ�䣺%s\n\t��ִ��ʱ����%d ms\t�ܵȴ�ʱ����%d ms\t��̬���ȼ���%d\n\n"
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
     * ���½���PCB���𵽽��̶�����
     */

    /* ������̶��з����� */
    WaitForSingleObject(PCBQueueMutex, INFINITE);

    PCBQueue * pq__ = (PCBQueue *)malloc(sizeof(PCBQueue)) ;
    PCBQueue * head__ = processQueue ;

    /* �ƶ���β�ڵ� */
    while (head__->nextPtr != NULL) {
        head__ = head__->nextPtr ;
    }

    pq__->pcb = __pcb ;
    pq__->nextPtr = NULL ;

    head__->nextPtr = pq__ ;

    /* ����������1 */
    totalProcessNumber++ ;

    /* �ͷ��� */
    ReleaseMutex(PCBQueueMutex);
}

bool deleteFromQueue(ProcessID __pid)
{
    /* ������̶��з����� */
    WaitForSingleObject(PCBQueueMutex, INFINITE);

    PCBQueue * pre__ = processQueue ;
    PCBQueue * then__ = pre__->nextPtr ;

    while (then__ != NULL) {
        if (then__->pcb->pid == __pid) {
            pre__->nextPtr = then__->nextPtr ;
            destroyPCB(then__->pcb);
        //	free(then__);

            /* ����������1 */
            totalProcessNumber-- ;

            /* �ͷ��� */
            ReleaseMutex(PCBQueueMutex);
            return true ;
        }
        else {
            pre__ = then__ ;
            then__ = then__->nextPtr ;
        }
    }

    /* �ͷ��� */
    ReleaseMutex(PCBQueueMutex);
    return false ;
}

int getQueueLength(ProcessState __state)
{
    /**
     * ����ĳ�����̶��еĳ���
     */

    /* ������̶��з����� */
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
     * �����н��̶������ҵ�ĳ��pid�Ľ��̲�����
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
     * ��__state�������ҵ�ĳ��pid�Ľ��̲�����
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
     * ��ָ��·���Ŀ�ִ���ļ����½��ý���
     */

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    int secNum__ ;

    /* ��ȡĿ��·�� */
    if (getIndexSection(sec__, &secNum__, __path)) {
        /* ����Ƿ����ļ����� */
        if (sec__[0] == FILE_TYPE) {
            FileIndex fi__ ;
            char * name__ = (char *)malloc(sizeof(char) * LENGTH_NAME) ;
            char * ex__ = (char *)malloc(sizeof(char) * LENGTH_NAME) ;

            /* ��ȡ�ļ���Ϣ */
            initFileIndex(&fi__) ;
            extractFileIndex(sec__, &fi__) ;

            /* ��ȡ��׺����������Ƿ�Ϊ��ִ���ļ� */
            extractFileName(name__, ex__, fi__.name) ;
            if (strcmp(ex__, FileEx[RUN_FILE]) == 0) {
                /* ��������Ϊ�գ����޷�ִ�� */
                if (fi__.dataSectionNumber > 0) {
                    /* ��ȡ��һ���������� */
                    if (readSection(sec__, fi__.dataNode[0])) {
                        /* ��ȡ��һ��������� */
                        int first3 = sec__[0] / 32 ;
                        int last5 = sec__[0] % 32 ;

                        /* ����һ���Ƿ�Ϊ������䣬�����ִ���ļ���ʽ�����޷�ִ�� */
                        if (first3 == 0) {
                            /* ����Ϊ���̷�������ȼ� */
                            ProcessPriority pri__ = getPri(last5) ;
                            /* ��ȡһ��ȫ��Ψһ��PID */
                            ProcessID pid__ = newPID() ;

                            /* Ϊ���̷�����棬����ִ�д��������� */
                            if (allocateVM(pid__, fi__.dataSectionNumber, fi__.dataNode)) {
                                PCB * pcb__ = (PCB *)malloc(sizeof(PCB)) ;
                                //FileStream * fs__ ;


                                /* �½�PCB */
                                newPCB(pcb__, pid__, fi__.name, __path, pri__, fi__.dataSectionNumber);

                                /* fs__ = openFile(__path);
                                if (fs__ != NULL) {
                                    pcb__->exeInfo.codeMemory = getFileLength(fs__) ;
                                    closeFile(fs__);
                                } */

                                /* ��Ӵ��ļ����б��� */
                                addOpenFile(__path);

                                //pcb__->schedInfo.event = NEW_PROCESS ;

                                /* ���ý��̹�������� */
                                addToQueue(pcb__);

                                /* ת�Ƶ��������� */
                                setProcessState(pcb__, READY);

                                //setProcessState(pcb__, NEW);

                                /* �½����̳ɹ� */
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
     * ��ָֹ��PID�Ľ���
     */

    PCB * pcb__ ;
    pcb__ = findPCB(__pid) ;

    /* ���̲����� */
    if (pcb__ != NULL) {
        /* ����ý������ڱ�ִ�У��������ִ�н��̣������´ε��� */
        /* if (pcb__->schedInfo.state == RUNNING) {
            runningProcess = NULL ;
        } */

        if (pcb__->schedInfo.dynamicPriority == 0) {
            priorityZeroCount-- ;
        }

        /* �ͷŸý��̵����ռ� */
        withdrawMemory(__pid) ;

        /* ���ļ����б��йرո��ļ� */
        closeOpenFile(pcb__->externalInfo.path);

        /* �Ӷ�����ɾ���ý��� */
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
        printf("����ID��%d\n\n", pcb__->pid);

        puts("�ⲿ��ʶ����");
        printf("\t�������ƣ�%s\t·����%s\n", pcb__->externalInfo.name, pcb__->externalInfo.path);

        puts("ִ����Ϣ��");
        printf("\tPC��%d\n\tCPUʹ�ã�%d%%\t�ڴ�ʹ�ã�%d B\n"
            , pcb__->exeInfo.processCounter, pcb__->exeInfo.CPUUsed, pcb__->exeInfo.memoryUsed + pcb__->exeInfo.codeMemory);

        puts("������Ϣ��");
        printf("\t״̬��%s\n\t�̶����ȼ���%d\t��̬���ȼ���%d\n"
            , stateDesc[pcb__->schedInfo.state], pcb__->schedInfo.priority, pcb__->schedInfo.dynamicPriority);
        showTime(buf, pcb__->schedInfo.time.createTime);
        printf("\t���̴���ʱ�䣺%s\n\tִ��ʱ����%d ms\t�ȴ�ʱ����%d ms\n"
            , buf, pcb__->schedInfo.time.runningTime, pcb__->schedInfo.time.waitingTime);

        printf("\t�¼���");
        if (pcb__->schedInfo.state != RUNNING) {
            puts(eventDesc[pcb__->schedInfo.event]);
        }
        else {
            putchar('\n');
        }

        puts("ҳ��\n\tҳ��\t��Чλ\t֡��");
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

    puts("�����б�\n");

    if (head__ == NULL) {
        puts("�����б�Ϊ��");

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

    printf("\n�ܽ�������%d\n���ȼ���Ϊ0��������%d\n\n", totalProcessNumber, priorityZeroCount) ;

    ReleaseMutex(PCBQueueMutex);
}

void degeneration(PCB * __pcb)
{
    /**
     * ��̬���ȼ����˻�����
     */

    if (__pcb == NULL) {
        return ;
    }

    ProcessPriority pri__ = __pcb->schedInfo.dynamicPriority;

    if (pri__ > 0) {
        /* ���˻�������һ */
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
     * ��̬���ȼ�������ʹ���µ���ȴ����еĽ������˻�������
     * �����ƽ��ֵһ��
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
 * [dispatchProcess ѡ��һ������ִ�У����������ж�̬���ȼ���ߵģ�]
 * @return [���ؽ��̵�PCBָ�룬û���򷵻�NULL]
 */
PCB * dispatchProcess() {

    WaitForSingleObject(PCBQueueMutex, INFINITE);

    PCBQueue * pcb_temp__ = NULL, *pcb_ans__ = NULL;
    pcb_temp__ = processQueue ;

    pcb_temp__ = pcb_temp__->nextPtr ;

    /*������������*/
    while (pcb_temp__ != NULL) {

        /*���pcb_ans__Ϊ��*/
        if(pcb_ans__ == NULL) {
            if(pcb_temp__->pcb->schedInfo.state == READY ) {
                pcb_ans__ = pcb_temp__;
            }
        }
        /*���pcb_ans__��Ϊ��*/
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
        /* ����PC��ֵ */
        runningProcess->exeInfo.processCounter = processCounter ;

        /* ���䶯̬���ȼ�����һ���˻� */
        degeneration(runningProcess) ;

        /* ���������¼� */
        runningProcess->schedInfo.event = RUN_OUT_OF_TIME_SLICE ;

        ReleaseMutex(runningProcessMutex) ;
        /* ���ý��̵���������� */
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
    /* ����IO���� */
    if (newIO(runningProcess->pid, __time)) {
        /* ���õȴ��¼� */
        runningProcess->schedInfo.event = WAITING_FOR_IO ;

        /* ����PC��ֵ */
        runningProcess->exeInfo.processCounter = processCounter ;

        ReleaseMutex(runningProcessMutex) ;
        /* �������Ƶ��ȴ����� */
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
        /* ���õȴ��¼� */
        pcb__->schedInfo.event = IO_FINISH ;

        /* ���ȼ����� */
        degeneration(pcb__);

        /* �������Ƶ��������� */
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
        /* ����ʱ��Ƭ���� */
        timeSlice = /*20 **/ getTimeSlice(pcb__->schedInfo.priority);

        /* ����PC */
        processCounter = pcb__->exeInfo.processCounter ;

        /* ���ý��̵���ִ�� */
        setProcessState(pcb__, RUNNING);
        WaitForSingleObject(runningProcessMutex, INFINITE) ;
        runningProcess = pcb__ ;
        ReleaseMutex(runningProcessMutex) ;

        /* �ͷ�ִ�н����ź��� */
        ReleaseSemaphore(runningProcessSemaphore, 1, NULL) ;

        /* ִ�м�ʱ��ʼ */
        startTime = clock() ;
    }
    else {
        //runningProcess = NULL ;

        /* �ȴ�10ms */
        Sleep(10);

        /* �ͷ��� */
        ReleaseSemaphore(dispathSemaphore, 1, NULL) ;
    }
}

void resetQueue()
{
    WaitForSingleObject(resetSemaphore, INFINITE) ;

    WaitForSingleObject(PCBQueueMutex, INFINITE);

    PCBQueue * pq__ = processQueue->nextPtr ;

    /* ���ö�̬���ȼ� */
    while (pq__ != NULL) {
        pq__->pcb->schedInfo.dynamicPriority = pq__->pcb->schedInfo.priority ;

        pq__ = pq__->nextPtr ;
    }

    /* �������˻����� */
    totalDegenerationCount = 0 ;
    priorityZeroCount = 0 ;

    ReleaseMutex(PCBQueueMutex);
}

DWORD WINAPI dispatchThread(LPVOID pPararneter)
{
    /**
     * ����ָ�ɽ���
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
     * �������ý��̶���
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
            /* �ͷ�ʹ�ÿռ� */
            __pcb->dataSegment.used -= then->end - then->begin + 1 ;
            if (__pcb->dataSegment.used < 0) {
                __pcb->dataSegment.used = 0 ;
            }

            logPrintf(HEAP_LOG, "������ �ͷ��ڴ�\n����PID�� %d\n\t��ַ������ %d\t��ʼ��ַ�� %d\t������ַ�� %d\n\n"
                            , __pcb->pid, then->label, then->begin, then->end) ;

            setMemoryUsed(__pcb, then->begin - then->end - 1);

            /* �ͷŸýڵ� */
            tmp->nextPtr = then->nextPtr ;
            free(then) ;

            return true ;
        }

        tmp = then ;
        then = then->nextPtr ;
    }

    logPrintf(HEAP_LOG, "������ �ͷ��ڴ�\n����PID�� %d\n\t��ַ������ %d\tû��ƥ��Ķѿռ�\n\n"
                    , __pcb->pid, __label) ;
    return NULL ;
}

int firstFit(PCB * __pcb, int __label, int __len)
{
    /**
     * �״���Ӧ�㷨�������ҵ����㹻�������ݶε���ʼ��ַ
     * ��Ӧʧ���򷵻ظ�ֵ
     */

    if (__pcb == NULL) {
        return -3 ;
    }

    int tot = __pcb->dataSegment.end - __pcb->dataSegment.begin + 1 ;
    /* û���㹻�ռ� */
    if (__len > tot - __pcb->dataSegment.used) {
        return -2 ;
    }

    MyHeap * tmp = __pcb->dataSegment.heap->nextPtr ;

    /* û���ѷ�������ݶΣ�ֱ�ӷ�����ʼ��ַ */
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

    /* ����λ�����ѷ���ĵ�һ�����ݶ�֮ǰ */
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

    /* ����λ�����ѷ�������һ�����ݶ�֮�� */
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

    /* û��ƥ��Ŀ� */
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
        sprintf(bufb, "%s\t��ַ�� %2d\t�� %4d\tֹ�� %4d\n", bufb, heap->label, heap->begin, heap->end) ;

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

        sprintf(bufa, "%s\t��ַ�� %2d\t�� %4d\tֹ�� %4d\n", bufa, heap->label, heap->begin, heap->end) ;

        moveto = heap->end + 1 ;
        heap = heap->nextPtr ;
    }

    logPrintf(HEAP_LOG, "������ ��Ƭ����\n����PID�� %d\n����ǰ��\n%s�����\n%s\n\n", __pcb->pid, bufb, bufa) ;
}

bool mallocDataSegment(PCB * __pcb, int __label, int __len)
{
    if (__pcb == NULL || __len <= 0) {
        setError(ILLEGAL_PARAMETER);
        return false ;
    }

    /* �ͷ�ԭ��label�ռ� */
    freeDataSegment(__pcb, __label) ;

    bool ret__ ;
    int allo ;
    allo = firstFit(__pcb, __label, __len) ;

    if (allo >= 0) {
        /* ����ɹ� */

        ret__ = true ;
    }
    else {
        /* û���㹻�ռ� */
        if (allo == -2) {
            setError(NO_ENOUGH_HEAP_SPACE);
            ret__ = false ;
        }
        /* ��Ӧʧ�ܣ�û��ƥ��Ŀ� */
        else if (allo == -1) {
            /* ������Ƭ�����㷨 */
            defragment(__pcb) ;

            /* �ٴγ��Է��� */
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
        logPrintf(HEAP_LOG, "������ �����ڴ�\t\t�ɹ�\n\tPID�� %d\t��ַ������ %d\n\t��ʼ��ַ�� %d\t������ַ�� %d\t���ȣ� %d\n\n"
                        , __pcb->pid, __label, allo, allo+__len-1, __len) ;
    }
    else {
        logPrintf(HEAP_LOG, "������ �����ڴ�\t\tʧ�ܣ� %s\n\tPID�� %d\t��ַ������ %d\t���ȣ� %d\n\n"
                        , errorDesc[errorCode], __pcb->pid, __label, __len) ;
    }

    return ret__ ;
}
