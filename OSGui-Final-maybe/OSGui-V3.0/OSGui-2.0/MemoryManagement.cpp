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
* 2016/04/23 10:16  countEmptyFrame �������֡�ĸ���
*                   double calMemUtilization()
*  2016/4/29 16:03		add MemoryLogGui		by LiangJie ;
*  2016/5/10   add workLog
* by LiangJie
*/

/**
* ��ģ��ά��һ��֡��
* ������֡��С��Ϣ
* �����ڴ�
* �ջ��ڴ�
* ���������ڴ��е�һ֡���ڴ��У�ͬʱ����֡��
* ѡ������ҳ����
*
*
* ����ҳ��
*
*
* ��ַ�任<------�������̹���ȥ��
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
extern int memorySize;                                          /* �ⲿ���� ��ʾ�ڴ��С����readMemoryConfig()����*/
extern const char * memoryPath;

int frameSize = 1024;                                            /* ֡��СΪ1024B*/
int frameCount = 2;                                              /*    ֡����    */
Frame * frameTable;
const char * framePath = "PC\\Config\\";

int FIFOCount = 0;
int pageType = 0 ;
extern MemoryFrameFault *MFFhead, *MFFlast;

extern  const char * errorDesc[100];
/**
* [readMFConfig description]  ��ȡ֡�Ĵ�С
* @return [description]
*/
bool readMFConfig()
{
    extern int memorySize;
    /* �����ļ�·�� */
    char * MFConfigPath__ = (char *)malloc(sizeof(char) * 38);
    sprintf(MFConfigPath__, "%sMFConfig.txt", framePath);

    char act[] = "����:��ȡ�ڴ�������Ϣ";
    /* �������ļ���ʧ���򷵻�false */
    FILE * ifp__ = fopen(MFConfigPath__, "r");
    if (ifp__ == NULL) {
        setError(MEMORYMANAGEMENT_CONFIG_NOT_EXIST);
        logPrintf(MEMORY_LOG, "%s\t%s\t%s%s\n\n", act, "�������:��ȡʧ��", "ʧ��ԭ��:", errorDesc[MEMORYMANAGEMENT_CONFIG_NOT_EXIST]);
        return false;
    }
    if (fscanf(ifp__, "%d", &frameSize) != 1) {
        setError(MEMORYMANAGEMENT_CONFIG_FORM_ERROR);
        logPrintf(MEMORY_LOG, "%s\t%s\t%s%s\n\n", act, "�������:��ȡʧ��", "ʧ��ԭ��:", errorDesc[MEMORYMANAGEMENT_CONFIG_FORM_ERROR]);
        return false;
    }

    frameCount = memorySize / frameSize;        /* ���֡�ĸ��� */
    frameTable = (Frame*)malloc(sizeof(Frame) * frameCount);   /* ����֡�� */


    fclose(ifp__);
    free(MFConfigPath__);


    logPrintf(MEMORY_LOG, "%s\t%s\t%s%d\t%s%d\n\n", act, "�������:��ȡ�ɹ�", "֡��СΪ:", frameSize, "֡����Ϊ:", frameCount);
    /* ��ȡ�ɹ� */
    return true;
}
/**
* һ�㲻�Ƽ�ʹ��
* [saveVMConfig �����ڴ�������Ϣ����֡��С]
* @return [description]
*/
bool saveMFConfig() {

    /* �����ļ�·�� */
    char * MFConfigPath__ = (char *)malloc(sizeof(char) * 38);
    sprintf(MFConfigPath__, "%sMFConfig.txt", framePath);

    /* �������ļ���ʧ���򷵻�false */
    FILE * ofp__ = fopen(MFConfigPath__, "w");
    if (ofp__ == NULL) {
        setError(MEMORYMANAGEMENT_CONFIG_NOT_EXIST);
        return false;
    }
    fprintf(ofp__, "%d\n", frameSize);
    fclose(ofp__);
    free(MFConfigPath__);

    /* д��ɹ� */
    return true;
}

/**
* �ú�������֡��,��readMFConfig��ſ���ִ��
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
* [initializeMemory ��ʼ������ϵͳ�ڴ�]
* @return [�ɹ�����true]
* �����ȵ��ô˺���
*/
bool initializeMemory() {
    /* ��ʼ�� �ڴ�Ӳ�� */
    if (!initializeMemoryHard()) {
        return false;
    }
    /* ��ʼ���ڴ�����   */
    if (!readMFConfig()) {
        return false;
    }
    /* ��ʼ�����      */
    if (!initializeVM()) {
        return false;
    }
    resetFrameTable();                                       /* ����֡�� */
    return true;
}

/**
* [showFrameTable չʾ֡��]
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
* [showPageTable ��ʾҳ��]
* @param __p [����ʾ��ҳ��]
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
* [allocateMemory �����ڴ�]
* @param  __PID       [���̺�]
* @param  __frameNeed [����������ڴ���]
* @param  __hasUsed   [�������õ��ڴ���]
* @return             [�ɹ�Ϊtrue]
*/
bool allocateMemory(int __PID, int __frameNeed, int __hasUsed) {
    int maxCanUse = VMframeAllocateCount(__PID);    /* ����ý����������ڴ��� */
    if (__frameNeed + __hasUsed < maxCanUse) {
        return true;
    }
    else {
        setError(MEMORY_APPLY_TOO_MUCH);
        return false;
    }

}

/**
* [resetPageTable ��ʼ��ҳ��]
* @param __PID            [���̺�]
* @param __pageApplyCount [����ҳ��]
* @param __pageTable      [ԭҳ��]
*/
void resetPageTable(int __PID, int __pageApplyCount, PageTable __pageTable) {
    int i__ = 0;
    for (i__ = 0; i__ <__pageApplyCount; i__++){           //��ʼ��ҳ��
        __pageTable[i__].isAvailable = false;
        __pageTable[i__].frameNum = -1;
        __pageTable[i__].PID = __PID;
    }
}

/**
* [withdrawMemory �ͷ��ڴ溯��]
* @param __PID [���̺�]
* @param __type[��ҳ����]
*/
void withdrawMemory(int __PID) {
    int i__;
    for (i__ = 0; i__ < frameCount; i__++) {
        /*��֤�˽��̵�֡���ڴ���ſ��ջ�*/
        if (frameTable[i__].PID == __PID && frameTable[i__].isAvailable == true) {
            if (pageType == 0) {
                /* FIFO�ջ�֡ */
                withDrawFIFO(i__);
                logPrintf(MEMORY_LOG, "%s\t%s%d\t%s\t%s\n\n", "����:�ջ��ڴ�", "���̺�:", __PID,"��ҳ����:","FIFO");
                }

            else {
                /* LRU�ջ�֡ */
                withDrawLRU(i__);
                logPrintf(MEMORY_LOG, "%s\t%s%d\t%s\t%s\n\n", "����:�ջ��ڴ�", "���̺�:", __PID, "��ҳ����:", "LRU");
            }
            /* ����Чλ����Ϊ��Ч*/
            frameTable[i__].isAvailable = false;

        }//if
    }
    /*�ջض�Ӧ�����*/
    withdrawVM(__PID);
}

/**
* [memoryAddressMapping Ѱַ����]
* @param  __pageTable  [ҳ��]
* @param  __pageNumber [ҳ��]
* @param  __pageOffset [ҳƫ��]
* @return              [�����ַ����Ч��ַ����-1]
*/
int  memoryAddressMapping(Page* __pageTable, int __pageNumber, int __pageOffset){

    int physicsAddress__ = 0;
    if (__pageTable[__pageNumber].isAvailable == true){   //����ҳ���ҵ�֡�ţ���ת����ַ
        physicsAddress__ = __pageTable[__pageNumber].frameNum * frameSize + __pageOffset;
        return physicsAddress__;   // ���������ַ
    }
    else
        return -1;   //-1����ȱҳ
}


/**
* [SacrificingPageSelection Ѱ�ҵ���ҳ]
* @param  __type [��ʽ]
* @return        [���ص���ҳ]
*/
int giveAPage() {
    /*  ����Ѱ����û�п�֡ */
    int i__;
    for (i__ = 0; i__ < frameCount; i__++) {
        if (frameTable[i__].isAvailable == false) {
            addPageErrorLog(i__, 1);
            return i__;
        }
    }
    int frameToReplace__;

    /*û�ҵ���ҳ����ѡ������ҳ*/
    if (pageType == 0) {
        frameToReplace__ = FIFOPageReplace();
        logPrintf(MEMORY_LOG, "%s\t%s%d\t%s\t%s\n\n", "����:Ѱ�ұ�����֡", "������֡��:", frameToReplace__,"��ҳ����:", "FIFO");
    }
    else {
        frameToReplace__ = LRUPageReplace();
        logPrintf(MEMORY_LOG, "%s\t%s%d\t%s\t%s\n\n", "����:Ѱ�ұ�����֡", "������֡��:", frameToReplace__, "��ҳ����:", "LRU");
    }
    //addPageErrorLog(frameToReplace__, 0);

    return frameToReplace__;
}


/**
* [VMToMemory ��һ������֡�ᵽ�ڴ�֡��]
* @param  __type   [��ҳ��ʽ]
* @param  __PID        [���̺�]
* @param  __pageNum	[���̶�Ӧ��ҳ��]
* @return              [֡��]
*/
int VMToMemory(int __PID, int __pageNum) {
    int frameNum__ = giveAPage();

    int VMFrameNum__ = findVMFrameToMemory(__PID, __pageNum);
    //printf("%d %d\n", frameNum__, VMFrameNum__);
    /* ��֤����֡Ϊ��Ч�Ĳſɽ��� */
    if (!isVMFrameNumAvailable(VMFrameNum__)) {
        setError(VM_FRAMENUM_NOT_AVAILABLE);
        return -1;
    }

    if(frameTable[frameNum__].isDirty == true) {
        MemoryToVM(frameNum__);
    }

    char * section__ = (char *)malloc(sizeof(char) * frameSize);   /* ������ */
    int sectionNumber__ = VMframeNumToSection(VMFrameNum__);         /* �������֡�ڴ����ϵĵ�ַ */
    //printf("sectionNumber is %d\n", sectionNumber__);
    if (sectionNumber__ < 0) {
        setError(VMEMORY_FRAMENUM_MAP_SECTION_NOT_FOUND);
        return false;
    }
    /* ���֡ �� ���̵�һ������һ����*/
    /*  �Ӵ����ж��� */
    if (!readSection(section__, sectionNumber__)) {
        return -1;
    }
    /* д���ڴ� */
    if (!writeMemoryByFrame(frameNum__ * frameSize, section__)) {
        return -1;
    }

    char log0[] = "����:���֡�����ڴ�";
    char log1__[100];
    sprintf(log1__, "%s%d\t%s%d", "���֡��:", VMFrameNum__,"֡��:", frameNum__);

    char log2__[100];
    sprintf(log2__, "%s%d\t%s%d", "�½���PID:", __PID, "ռ��֡���½��̵�ҳ��", __pageNum);

    /*���ı�ռ��֡ �Ľ���ҳ�����Ϣ*/
    if (frameTable[frameNum__].isAvailable == true) {
        int oldPid__ = frameTable[frameNum__].PID;
        int oldPage__ = frameTable[frameNum__].pageNum;
        pageUnvailable(oldPid__, oldPage__);

        char log3__[100];
        sprintf(log3__, "%s%d\t%s%d", "����ռ�Ľ���PID:", oldPid__, "����ռ֡�Ľ��̵�ҳ��:", oldPage__);

        logPrintf(MEMORY_LOG, "%s\t%s\t%s\t%s\n\n",log0,log1__,log3__,log2__);

        /*OSGui�м�¼*/
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


    /* ����֡�����Ϣ */
    FIFOUse(frameNum__, frameTable[frameNum__].isAvailable);
    LRUUse(frameNum__);

    frameTable[frameNum__].isAvailable = true;

    frameTable[frameNum__].PID = __PID;
    frameTable[frameNum__].pageNum = __pageNum;
    frameTable[frameNum__].isDirty = false;

    return frameNum__;
}

/**
 * [MemoryToVM �ѱ��ı���ڴ�֡��ص������]
 * @param  __frameNum [֡��]
 * @return            [�Ƿ�ɹ�]
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
    char * section__ = (char *)malloc(sizeof(char) * frameSize);   /* ������ */
    int sectionNumber__ = VMframeNumToSection(VMFrameNum__);         /* �������֡�ڴ����ϵĵ�ַ */
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
* [readMemoryByChar ��ȡ�ַ�]
* @param  __pid      [���̺�]
* @param  __frameNum [֡��]
* @param  __offsetIn [֡��ƫ����]
* @return            [���󷵻�255]
*/
bool readMemoryByChar(int __pid, int __frameNum, int __offsetIn, unsigned char * __toRead) {
    int offset__;
    //�����֡û��ռ��
    char log0[] = "����:���ڴ浥Ԫ";
    if (frameTable[__frameNum].isAvailable == false) {
        setError(FRAME_NOT_AVAILABLE);
        logPrintf(MEMORY_LOG, "%s\t%s\t\n\n", log0,"����ʧ��","��Ч֡");
        return false;
    }
    //�����֡���Ǳ��ý���ռ��
    if (frameTable[__frameNum].PID != __pid) {
        setError(PID_NOT_MAP_FRAME);
        logPrintf(MEMORY_LOG, "%s\t%s\t\n\n", log0, "����ʧ��", errorDesc[PID_NOT_MAP_FRAME]);
        return false;
    }
    offset__ = __frameNum * frameSize + __offsetIn;
    char log1[150];
    sprintf(log1, "%s\t%s%d\t%s%d\t%s%d\n\n", log0, "���̺�:", __pid, "֡��:", __frameNum, "��ַ:", __offsetIn);
    logPrintf(MEMORY_LOG, "%s",log1);
    bool result__ = readMemoryUnit(offset__, __toRead);
    return result__;

}



/**
 * [writeMemoryByChar д���ַ�]
* @param  __pid      [���̺�]
 * @param  __frameNum [֡��]
 * @param  __offsetIn [֡��ƫ����]
 * @return            [���󷵻�false]
 */
bool writeMemoryByChar(int __pid, int __frameNum, int __offsetIn, const unsigned char __toSave) {
    int offset__;
    //�����֡û��ռ��
    char log0[] = "����:д�ڴ浥Ԫ";
    if (frameTable[__frameNum].isAvailable == false) {
        setError(FRAME_NOT_AVAILABLE);
        logPrintf(MEMORY_LOG, "%s\t%s\t\n\n", log0, "����ʧ��", "��Ч֡");
        return false;
    }
    //�����֡���Ǳ��ý���ռ��
    if (frameTable[__frameNum].PID != __pid) {
        setError(PID_NOT_MAP_FRAME);
        logPrintf(MEMORY_LOG, "%s\t%s\t\n\n", log0, "����ʧ��", errorDesc[PID_NOT_MAP_FRAME]);
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
* [FIFOUse ʹ��һ��֡ʱ��ӦFIFO����]
* @param __frameNum [ʹ�õ�֡]
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
* [withDrawFIFO �ջ�һ��֡��FIFO����]
* @param __frameNum [�ջص�֡]
*/
bool withDrawFIFO(int __frameNum) {
    /*�ȼ�鱻�ջ�֡�Ƿ�������֡*/
    if (!frameTable[__frameNum].isAvailable) {
        /*���� �ջ�δ��ʹ��֡����*/
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
* [FIFOPageReplace ����FIFO�ҵ�����ҳ]
* @return [���ص���ҳ(��-1�����п�ҳδ��ʹ�ô���)]
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
* [LRUUse ʹ��һ��֡ʱ��ӦLRU����]
* @param __frameNum [ʹ�õ�֡]
*/
void LRUUse(int __frameNum) {
    int i__;
    /*δ��ʹ�õ�֡LRUcount����1*/
    for (i__ = 0; i__ < frameCount; i__++) {
        /*��i__֡��ʹ�õ������*/
        if (frameTable[i__].isAvailable == true) {
            frameTable[i__].LRUcount = frameTable[i__].LRUcount + 1;
        }

    }
    /*��ʹ�õ�֡LRUcountΪ0*/
    frameTable[__frameNum].LRUcount = 0;
}

int LRUPageReplace() {
    int i__;
    int frameToReplace__, maxCount;
    frameToReplace__ = -1;
    maxCount = -1;

    /*�����ҳ�LRUcount����һ��֡Ϊ���û�֡*/
    for (i__ = 0; i__ < frameCount; i__++) {
        if (frameTable[i__].isAvailable == true && frameTable[i__].LRUcount > maxCount) {
            frameToReplace__ = i__;
            maxCount = frameTable[i__].LRUcount;
        }
    }
    return frameToReplace__;
}

/**
* [withDrawLRU �ջ�һ��֡��LRU����]
* @param __frameNum [�ջص�֡]
*/
bool withDrawLRU(int __frameNum) {

    /*�ȼ�鱻�ջ�֡�Ƿ�������֡*/
    if (!frameTable[__frameNum].isAvailable) {
        /*���� �ջ�δ��ʹ��֡����*/
        setError(FRAME_NOT_AVAILABLE);
        return false;
    }

    int i__;
    for (i__ = 0; i__ < frameCount; i__++) {
        /*���ӵĲ�����������������*/
        /*���ջ�֡LRUcount���֡��LRUcount  ��ȥ  �ջ�֡LRUcount */
        if (frameTable[i__].isAvailable == true && frameTable[i__].LRUcount > frameTable[__frameNum].LRUcount) {
            frameTable[i__].LRUcount--;
        }
    }
    frameTable[__frameNum].LRUcount = -1;
    return false;
}


bool addPageErrorLog(int  __frameNum, int __type) {

    /* �����ļ�·�� */
    char * MemoryLogPath__ = (char *)malloc(sizeof(char) * 38);
    sprintf(MemoryLogPath__, "%sLog.txt", memoryPath);

    /* �������ļ���ʧ���򷵻�false */
    FILE * ofp__ = fopen(MemoryLogPath__, "a+");
    if (ofp__ == NULL) {
        setError(MEMORYLOGPATH_NOT_EXIST);
        return false;
    }
    if (__type == 0)
        fprintf(ofp__, "�ڴ�֡�������滻���� %d ֡  ",__frameNum);
    else {
        fprintf(ofp__, "ҳ�����ڴ��У����뵽�� %d ֡  ", __frameNum);
    }
    TimeStamp time__;
    getSystemTime(&time__);
    char buff__[40];
    showTime(buff__, time__);
    fprintf(ofp__, "%s\n", buff__);

    fclose(ofp__);
    free(MemoryLogPath__);
/* д��ɹ� */
    return true;
}

/**
 * [countusedFrame �������֡�ĸ���]
 * @return [����]
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
 * [calMemUtilization �����ڴ�������]
 * @return [������]
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
