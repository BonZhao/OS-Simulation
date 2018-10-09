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
* 2016/03/17 17:41     ����ҳ��ҳ��
* 2013/03/17 19:10     allocateVM();  showVirtualMemoryDetial();  findVMFrameToMemory();
* 2013/03/20 19:00     �����淶���� VMframeAllocateCount(int __PID)��
* 2013/03/21 20:45     int VMframeNumToSection(int __VMFrameNum);
* 2013/03/23 20:12     isVMFrameNumAvailable(int __VMFrameNum);
* 2016/04/23 10:25     double calVMUtilization()
*  2016/5/10   add workLog
*                      by LiangJie
*/

/**
*  ���������ڴ�ģ�飬����ͨ����������������ʵ���书��
*  �ǵڶ������
*/

#include <stdio.h>
#include <stdlib.h>
#include "VirtualMemory.h"
#include "MemoryManagement.h"
#include "Basic.h"
#include "DiskDriver.h"
#include "WorkLog.h"

/**
* �����ڴ�������Ϣ������
* �����ڴ����ڵ�����λ��
* �����ڴ�Ĵ�С
* ֡�Ĵ�С��ȫ�ֱ�����
*/

int VMLocation;                          /*	����λ�� */
int VMSize;                              /* ����С */
int VMFrameCount;                        /* ����֡���� */
int VMAvailableFrameCount;               /* ����֡���� */

/* �����ڴ������ļ�λ�� */
const char * VMPath = "PC\\Config\\";
extern int frameSize;
extern int sectionSize ;


/* �����ڴ�֡�� */
VMFrame* VMFrameTable;

extern  const char * errorDesc[100];

/**
* �ú�������֡��,��readVMConfig��ſ���ִ��
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
* [readVMConfig ��ȡ�����ڴ��������Ϣ]
* ������frameSize�������ſ���ִ�� (����readMemoryConfig()��ʼִ��)
* ֻ��ִ��һ��
* �����ڸ�ģ��������������ִ��֮ǰִ��
* ���ȡ����λ��
* ���ȡ����С�����ٸ��ֽڣ�
* @return �ɹ���ȡ����true, ����false
*/
bool readVMConfig()
{
    /* �����ļ�·�� */
    char * VMConfigPath__ = (char *)malloc(sizeof(char) * 38);
    sprintf(VMConfigPath__, "%sVMConfig.txt", VMPath);
    int vmEnd__ ;

    char act[] = "����:��ȡ�����ڴ�������Ϣ";


    /* �������ļ���ʧ���򷵻�false */
    FILE * ifp__ = fopen(VMConfigPath__, "r");
    if (ifp__ == NULL) {
        setError(VMEMORY_CONFIG_NOT_EXIST);
        logPrintf(VM_LOG, "%s\t%s\t%s%s\n\n", act, "�������:��ȡʧ��", "ʧ��ԭ��:", errorDesc[VMEMORY_CONFIG_NOT_EXIST]);
        return false;
    }
    if (fscanf(ifp__, "%d %d", &VMLocation, &vmEnd__) != 2) {
        setError(VMEMORY_CONFIG_FORM_ERROR);
        logPrintf(VM_LOG, "%s\t%s\t%s%s\n\n", act, "�������:��ȡʧ��", "ʧ��ԭ��:", errorDesc[VMEMORY_CONFIG_FORM_ERROR]);
        return false;
    }

    fclose(ifp__);
    free(VMConfigPath__);

    if (frameSize != sectionSize) {
        setError(MEMORYMANAGEMENT_CONFIG_NOT_READ);
        logPrintf(VM_LOG, "%s\t%s\t%s%s\n\n", act, "�������:��ȡʧ��", "ʧ��ԭ��:", errorDesc[MEMORYMANAGEMENT_CONFIG_NOT_READ]);
        return false;
    }

    VMFrameCount = vmEnd__ - VMLocation + 1 ;        /* ������frameSize�������ſ���ִ�� */

    VMSize = VMFrameCount * frameSize ;

    VMFrameTable = (VMFrame*)malloc(sizeof(VMFrame) * VMFrameCount);


    char log1[100];
    sprintf(log1, "%s\t%s\t%s%d\n", act, "�������:��ȡ�ɹ�", "���֡����:", VMFrameCount);
    char log2[100];
    sprintf(log2, "%s%d%s\t%s%d\t%s%d\n", "����С:", VMSize, "B",
                                            "�����ʼ������:", VMLocation,
                                            "������������:", vmEnd__);
    logPrintf(VM_LOG, "%s%s\n", log1,log2);
    /* ��ȡ�ɹ� */
    return true;
}

/**
* [initializeVM ��ʼ�����]
* @return [�ɹ�����true]
*/
bool initializeVM() {
    /*��ȡ���λ�ã���С������Ϣ*/
    if (!readVMConfig()) {
        return false;
    }
    /* ���������ڴ�֡�� */
    resetVMFrameTable();
    return true;
}

/**
* [saveVMConfig �޸������ڴ�������Ϣ]
* ���������ڴ�λ�ã���С
* ���ܵ���ʹ�ã�������ļ�ϵͳ���ʹ��
* ���Ƽ�ʹ��
* @return true--�ɹ�
*/
bool saveVMConfig() {

    /* �����ļ�·�� */
    char * VMConfigPath__ = (char *)malloc(sizeof(char) * 38);
    sprintf(VMConfigPath__, "%sVMConfig.txt", VMPath);

    /* �������ļ���ʧ���򷵻�false */
    FILE * ofp__ = fopen(VMConfigPath__, "w");
    if (ofp__ == NULL) {
        setError(VMEMORY_CONFIG_NOT_EXIST);
        return false;
    }
    fprintf(ofp__, "%d %d\n", VMLocation, VMFrameCount + VMLocation - 1);
    fclose(ofp__);
    free(VMConfigPath__);

    /* д��ɹ� */
    return true;
}

/**
* [VMframeAllocateCount ͳ��ĳ������ռ�õ�֡��]
* @param  __PID [���̺�]
* @return       [���طָ�ĳ�����̵�֡��]
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
* [withdrawVM �ͷ�ĳ���̵����]
* @param  _PID [���̺�]
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
* [diskToVM �Ѵ����ļ��������]
* @param  __sectionNumber [����������]
* @param  __VMframeNum    [���֡��]
* @param  __PID           [���̺�]
* @param  __pageNum       [������Ľ��̵�ҳ��]
* @return                 [�ɹ�true]
*/
bool diskToVM(int __sectionNumber, int  __VMFrameNum) {
    char * section__ = (char *)malloc(sizeof(char) * frameSize);   /* ������ */
    int VMsectionNumber__ = VMframeNumToSection(__VMFrameNum);         /* �������֡�ڴ����ϵĵ�ַ */
    //printf("%d\n", VMsectionNumber__);
    char act[] = "����:���������ݵ������";
    if (VMsectionNumber__ < 0) {
        setError(VM_FRAMENUM_NOT_AVAILABLE);
        logPrintf(VM_LOG, "%s\t%s\t%s%s\n\n", act, "����ʧ��", "ʧ��ԭ��:", errorDesc[VM_FRAMENUM_NOT_AVAILABLE]);
        return false;
    }
    /* ���֡ �� ���̵�һ������һ����*/
    /*  �Ӵ����ж��� */
    //printf("%d\n", __sectionNumber);
    if (!readSection(section__, __sectionNumber)) {
        return false;
    }
    if (!saveSection(section__, VMsectionNumber__)) {
        return false;
    }
    logPrintf(VM_LOG, "%s\t%s\t%s%d\t%s%d\n\n", act,"�����ɹ�","���̺�:",__sectionNumber,"����֡��",__VMFrameNum);
    return true;

}

/**
* [allocate ���������ڴ��ĳ����]
*
* @param PID             �ý��̶�Ӧ��PID(���̻�δ���壬��֪����ô��)
* @param  FrameCountApply  �ý��������֡��
* @param  sectionNum[]   �ý��̶�Ӧ�Ĵ����ļ���ַ������__frameCountApply��
* @return                 ����ɹ�������true�����򷵻�false
*/
bool allocateVM(int __PID, int __frameCountApply, int sectionNum[]) {
    int i__ = 0;
    int successApplyCount__ = 0;
    int remainApplyCount__ = __frameCountApply;

    char act[150];
    sprintf(act, "%s\t%s%d\t%s%d\t%s%d", "����:���������ڴ��ĳ����", "���̺�:", __PID, "����֡��:", __frameCountApply, "����֡��:", VMAvailableFrameCount);
    /* �������֡���������֡����������ʧ��*/
    //printf("%d %d\n", VMAvailableFrameCount, __frameCountApply);
    if (__frameCountApply > VMAvailableFrameCount) {
        setError(VMEMORY_APPLY_TOO_MUCH);
        //logPrintf(VM_LOG, "%s\t%s%d\t%s%d\t%s\t%s\t%s\n\n", act, "���̺�:",__PID, "����֡��",__frameCountApply,"����ʧ��", "ʧ��ԭ��:", errorDesc[VMEMORY_APPLY_TOO_MUCH]);
        logPrintf(VM_LOG, "%s\n%s\t%s\t%s\n\n", act, "����ʧ��", "ʧ��ԭ��:", errorDesc[VMEMORY_APPLY_TOO_MUCH]);
        return false;
    }
    else {

        /* ������Ѱ����֡�����������ı������ڴ�֡��֡�� */
        for (i__ = 0; i__ < VMFrameCount && successApplyCount__ < __frameCountApply; i__++) {
            if (VMFrameTable[i__].isAvailable == false) {
                VMFrameTable[i__].isAvailable = true;    /* ��֡����Ϊ��ռ�� */
                VMFrameTable[i__].PID = __PID;
                VMFrameTable[i__].pageNum = successApplyCount__;

                VMAvailableFrameCount--;			    /* ����֡����һ */
                remainApplyCount__--;                     /* ʣ�໹��Ҫ����֡����һ */
                successApplyCount__++;                    /* �ɹ�����֡����һ */
                /**
                * ������ж���Ϊ�˷�ֹ����̾��������ڴ�ʱ�����������ڴ������һ����ʱ����
                * �����ڴ治����������ʱҪ�ѷ����ȥ�Ĳ����ջ���
                */
                if (remainApplyCount__ > VMAvailableFrameCount) {
                    withdrawVM(__PID);                   /* �ջ���� */
                    setError(VMEMORY_NOT_ENOUGH);
                    logPrintf(VM_LOG, "%s\n%s\t%s\t%s\n\n", act, "����ʧ��", "ʧ��ԭ��:", errorDesc[VMEMORY_NOT_ENOUGH]);
                    return false;
                }

            }
        }//for
        /* �Ѵ����ļ��������*/
        //showVirtualMemoryDetial();
        int secInNum = 0;//������Ĵ��̵��±�
        for (i__ = 0; i__ < VMFrameCount && secInNum < __frameCountApply; i__++) {
            if (VMFrameTable[i__].PID == __PID) {
                /*��δ�ɹ�������棬�����ջ��ѷ�������*/
                if (!diskToVM(sectionNum[secInNum], i__)) {
                    withdrawVM(__PID);                   /* �ջ���� */
                    return false;
                }
                else {
                    /*�ɹ�����*/
                    secInNum++;
                }
            }//if(pid)
        }//for(i__)
    }//else
    /* ����ɹ� */
    logPrintf(VM_LOG, "%s\n%s\t%s%d\n\n", act, "�����ɹ�","�����֡��:",__frameCountApply);
    return true;
}

/**
* [showVirtualMemoryDetial ��ʾ���������]
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
* [findVMFrameToMemory �ҵ�����֡]
* @param  _PID     [����ID]
* @param  _pageNum [������Ҫ�����ҳ��]
* @return          [�������Ķ�Ӧ֡�� δ�ҵ�����-1]
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
* [VMframeNumToSection ����ĳ������֡��Ӧ������]
* @param  __VMFrameNum [����֡��]
* @return              [������(û�з���-1)]
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
* [isVMFrameNumAvailable �ж�ĳ������֡�Ƿ���Ч]
* @param  __VMFrameNum   [����֡��]
* @return                [�����Ƿ���Ч]
*/
bool isVMFrameNumAvailable(int __VMFrameNum) {
    return VMFrameTable[__VMFrameNum].isAvailable;
}

/**
 * [calVMUtilization ���������ڴ�������]
 * @return [������]
 */
double calVMUtilization() {
    double utili = (double)VMAvailableFrameCount/(double)VMFrameCount;
    return (1 - utili);
}
