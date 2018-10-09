/**
* MyOS/MemoryDrive.c
*
* Copyright (c) 2016 LiangJie
*/

/**
* MemoryDrive.c�������ڴ����������ṩ�����в����ڴ���õ��ĺ���
* ��Щ����ģ����Ӳ���ӿڣ������ڴ����ã��ڴ�����ڴ�д������ڴ��ַ�Ƿ����
* �������������Щ�����������ڴ��д����Ӳ�����в���
*  2016/5/10   add workLog
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Basic.h"
#include "MemoryDriver.h"
#include <fstream>
#include "WorkLog.h"
extern ErrorType errorCode;
int memorySize = 0;                                              /* �ڴ��С */
char *memoryDescription = (char *)malloc(sizeof(char) * 100);	    /*   �ڴ�����   */

/* �ڴ�λ�� */
const char * memoryPath = "PC\\Memory\\";
/* �ڴ��ļ� */
const char *memoryFileName = "PC\\Memory\\Memory.txt";
extern int frameSize;
extern int frameCount;

bool readMemoryConfig()
{
    /**
    * ��ȡ�ڴ��������Ϣ���˲���Ӧ����
    * ����ϵͳ����ʱ��BIOS�������
    * �����ڴ��С
    */

    /* �����ļ�·�� */
    char * configPath__ = (char *)malloc(sizeof(char) * 38);
    sprintf(configPath__, "%sConfig.txt", memoryPath);

    char * logContent__ = (char *)malloc(sizeof(char) * 200);

    /* �������ļ���ʧ���򷵻�false */
    FILE * ifp__ = fopen(configPath__, "r");
    if (ifp__ == NULL) {
        setError(MEMORY_CONFIG_NOT_EXIST);
        sprintf(logContent__, "%s\t%s\t%s\n\n", "����:��ȡ�ڴ�Ӳ����Ϣ","�������:��ȡʧ��","ʧ��ԭ��:�ڴ������ļ�������");
        logPrintf(MEMORY_DRIVER_LOG, logContent__);

        return false;
    }

    /* ��˳���ȡ���ݣ�����⵽�����������ļ���ʽ�򷵻�false */
    if (fgets(memoryDescription, 100, ifp__) == NULL) {
        setError(MEMORY_CONFIG_DESC_TOO_LONG_OR_SHORT);
        sprintf(logContent__, "%s\t%s\t%s\n\n", "����:��ȡ�ڴ�Ӳ����Ϣ","�������:��ȡʧ��", "ʧ��ԭ��:�ڴ������ļ���ʽ����ȷ");
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return false;
    }
    if (fscanf(ifp__, "%d", &memorySize) != 1) {
        setError(MEMORY_CONFIG_MEMORYSIZE_ERROR);
        sprintf(logContent__, "%s\t%s\t%s\n\n", "����:��ȡ�ڴ�Ӳ����Ϣ","�������:��ȡʧ��", "ʧ��ԭ��:�ڴ������ļ��ڴ��С��ʽ����");
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return false;
    }

    fclose(ifp__);
    free(configPath__);

    sprintf(logContent__, "%s\t%s%s%s%d\n\n", "����:��ȡ�ڴ�Ӳ����Ϣ","�������:��ȡ�ɹ����ڴ�����:", memoryDescription, "�ڴ��С:",memorySize);

    logPrintf(MEMORY_DRIVER_LOG, logContent__);
    /* ��ȡ�ɹ� */
    return true;
}

bool saveMemoryConfig()
{
    /**
    * �����ڴ��������Ϣ���˲���ֻ�����ڴ�����
    * ��Ϣ���޸�ʱ���ã��������ڴ���֮��
    */

    /* �����ļ�·�� */
    char * configPath__ = (char *)malloc(sizeof(char) * 38);
    sprintf(configPath__, "%sConfig.txt", memoryPath);

    /* �������ļ���ʧ���򷵻�false */
    FILE * ofp__ = fopen(configPath__, "w");
    if (ofp__ == NULL) {
        setError(MEMORY_CONFIG_NOT_EXIST);
        return false;
    }
    /* �����ڴ�������Ϣ */
    sprintf(memoryDescription, "%s", "2016����������");

    memorySize = 2048;             /* �����ڴ��С */
    /* ��˳��д������ */
    fprintf(ofp__, "%s\n%d\n", memoryDescription, memorySize);

    fclose(ofp__);
    free(configPath__);

    /* д��ɹ� */
    return true;
}

bool memoryReset() {
    /* �����ڴ� */
    FILE *memoryFile__;
    unsigned char data__ = 255;

    /*�½��ڴ��ļ�*/
    if ((memoryFile__ = fopen(memoryFileName, "wb")) == NULL)
    {
        setError(MEMORY_NOT_EXIST);
        return false;
    }

    /*д������ ��ȫ1�� */
    for (int i = 0; i < memorySize; i++) {
        fwrite(&data__, sizeof(data__), 1, memoryFile__);
    }

    fclose(memoryFile__);
    return true;
}

/**
* [initializeMemoryHard �ú�����ʼ���ڴ�Ӳ��]
* @return [description]
*/
bool initializeMemoryHard() {
    /* ��ȡ�ڴ�������Ϣ*/
    if (!readMemoryConfig()) {
        return false;
    }
    /* �����ڴ� */
    if (!memoryReset()) {
        return false;
    }
    return true;
}

bool isOverflow(int __offset) {
    /**
    *����Ƿ�Խ��
    *Խ�緵��false�����򷵻�true
    */
    FILE *memoryFile__;

    /*����ڴ��ļ��Ƿ���ڣ������ڷ���false*/
    if ((memoryFile__ = fopen(memoryFileName, "rb")) == NULL) {
        setError(MEMORY_NOT_EXIST);
        return false;
    }
    else {
        fclose(memoryFile__);
        /*����Ƿ�Խ��*/
        if (__offset < memorySize && __offset >= 0) {
            return true;
        }
        else {
            setError(MEMORYADDRESS_OUT_OF__BOUND);
            return false;
        }

    }

}
bool readMemoryUnit(int __offset, unsigned char *__toRead) {
    /**
    *���ڴ��offsetλ�ö�ȡһ���ֽڵ�����
    */
    FILE *memoryFile__;

    char * logContent__ = (char *)malloc(sizeof(char) * 200);

    /*���ڴ��ļ�*/
    memoryFile__ = fopen(memoryFileName, "rb");
    if (memoryFile__ == NULL) {
        /*�ļ������ڷ��ء�ȫ1��*/
        setError(MEMORY_NOT_EXIST);
        sprintf(logContent__, "%s\t%s\t%s\n\n", "����:���ڴ浥Ԫ","�������:��ȡʧ��", "ʧ��ԭ��:�ڴ治����");
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return false;
    }
    else {
        /*����Ƿ����*/
        if (!isOverflow(__offset)) {
            sprintf(logContent__, "%s\t%s\t%s\n\n", "����:���ڴ浥Ԫ", "�������:��ȡʧ��", "ʧ��ԭ��:�ڴ��ַ���");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;

        }

        /*��λ*/
        int result__ = fseek(memoryFile__, __offset, SEEK_SET);
        if (result__ < 0) {
            /*δ�ҵ����ء�ȫ1��*/
            setError(MEMORYADDRESS_OUT_OF__BOUND);
            sprintf(logContent__, "%s\t%s\t%s\n\n", "����:���ڴ浥Ԫ","�������:��ȡʧ��", "ʧ��ԭ��:�ڴ��ַ����ȷ");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }

        /*�����ݵ�data*/
        fread(__toRead, sizeof(unsigned char), 1, memoryFile__);
        fclose(memoryFile__);
        memoryFile__ = NULL;

        sprintf(logContent__, "%s\t%s\t%s%d\t%s%02X\n\n", "����:���ڴ浥Ԫ","�������:��ȡ�ɹ�", "��ȡλ��:", __offset, "��ȡ����:", *__toRead);

        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return true;
    }
}

bool writeMemoryUnit(int __offset, const unsigned char __toSave) {
    /*
    * ���ڴ��offsetλ��дһ���ֽڵ�����
    */
    FILE *memoryFile__;

    char * logContent__ = (char *)malloc(sizeof(char) * 200);

    /*���ڴ��ļ�*/
    if ((memoryFile__ = fopen(memoryFileName, "rb+")) == NULL) {
        /*�ļ�������*/
        setError(MEMORY_NOT_EXIST);
        sprintf(logContent__, "%s\t%s\t%s\n\n", "����:д�ڴ浥Ԫ","�������:д��ʧ��", "ʧ��ԭ��:�ڴ治����");
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return false;
    }
    else{
        /*����Ƿ����*/
        if (!isOverflow(__offset)) {
            sprintf(logContent__, "%s\t%s\t%s\n\n", "����:д�ڴ浥Ԫ", "�������:д��ʧ��", "ʧ��ԭ��:�ڴ��ַ���");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }

        /*��λ*/
        int result__ = fseek(memoryFile__, __offset, SEEK_SET);
        if (result__ < 0) {
            /*δ�ɹ�����false*/
            setError(MEMORYADDRESS_OUT_OF__BOUND);
            sprintf(logContent__, "%s\t%s\t%s\n\n", "����:д�ڴ浥Ԫ","�������:д��ʧ��", "ʧ��ԭ��:�ڴ��ַ����");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }

        fwrite(&__toSave, sizeof(__toSave), 1, memoryFile__);
        fclose(memoryFile__);

        sprintf(logContent__, "%s\t%s\t%s%d\t%s%02X\n\n", "����:д�ڴ浥Ԫ","�������:д��ɹ�", "д��λ��:", __offset, "д������:", __toSave);
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return true;
    }
}

bool readMemoryByFrame(int __offset, char* __data) {
    /**
    *���ڴ��offsetλ�ö�ȡһ��֡������
    */
    FILE *memoryFile__;

    char * logContent__ = (char *)malloc(sizeof(char) * 200);
    /*���ڴ��ļ�*/
    if ((memoryFile__ = fopen(memoryFileName, "rb")) == NULL) {
        /*�ļ������ڷ��ء�ȫ1��*/
        setError(MEMORY_NOT_EXIST);
        sprintf(logContent__, "%s\t%s\t%s\n\n", "����:���ڴ�֡", "�������:��ȡʧ��", "ʧ��ԭ��:�ڴ治����");
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return false;
    }
    else {
        /*����½��Ƿ����*/
        if (!isOverflow(__offset)) {
            sprintf(logContent__, "%s\t%s\t%s\n\n", "����:���ڴ�֡", "�������:��ȡʧ��", "ʧ��ԭ��:�ڴ��ַ����");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }
        /*����Ͻ��Ƿ����*/
        if (!isOverflow(__offset + frameSize - 1)) {
            sprintf(logContent__, "%s\t%s\t%s\n\n", "����:���ڴ�֡", "�������:��ȡʧ��", "ʧ��ԭ��:�ڴ��ַ����");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }


        /*��λ*/
        int result__ = fseek(memoryFile__, __offset, SEEK_SET);
        if (result__ < 0) {
            /*δ�ҵ����ء�ȫ1��*/
            setError(MEMORYADDRESS_OUT_OF__BOUND);
            sprintf(logContent__, "%s\t%s\t%s\n\n", "����:���ڴ�֡", "�������:��ȡʧ��", "ʧ��ԭ��:�ڴ��ַ����ȷ");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }

        /*�����ݵ�data*/
        fread(__data, sizeof(char), frameSize, memoryFile__);
        fclose(memoryFile__);


         char * hexData__ = (char*)malloc(sizeof(char) * frameSize * 2);
         strToHex(__data,hexData__);

        sprintf(logContent__, "%s\t%s\t%s%s\n\n", "����:���ڴ�֡", "�������:��ȡ�ɹ�", "֡����:",hexData__);
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return true;
    }
}

bool writeMemoryByFrame(int __offset, char *__toSave) {
    /*
    * ���ڴ��offsetλ��дһ��֡������
    */
    FILE *memoryFile__;

    char * logContent__ = (char *)malloc(sizeof(char) * 200);

    /*���ڴ��ļ�*/
    if ((memoryFile__ = fopen(memoryFileName, "rb+")) == NULL) {
        /*�ļ�������*/
        setError(MEMORY_NOT_EXIST);
        sprintf(logContent__, "%s\t%s\t%s\n\n", "����:д�ڴ�֡", "�������:д��ʧ��", "ʧ��ԭ��:�ڴ治����");
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return false;
    }
    else{
        /*����½��Ƿ����*/
        if (!isOverflow(__offset)) {
            sprintf(logContent__, "%s\t%s\t%s\n\n", "����:д�ڴ�֡", "�������:д��ʧ��", "ʧ��ԭ��:�ڴ��ַ�½����");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }

        /*����Ͻ��Ƿ����*/
        if (!isOverflow(__offset + frameSize - 1)) {
            sprintf(logContent__, "%s\t%s\t%s\n\n", "����:д�ڴ�֡", "�������:д��ʧ��", "ʧ��ԭ��:�ڴ��ַ�Ͻ����");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }


        /*��λ*/
        int result__ = fseek(memoryFile__, __offset, SEEK_SET);
        if (result__ < 0) {
            /*δ�ɹ�����false*/
            setError(MEMORYADDRESS_OUT_OF__BOUND);
            sprintf(logContent__, "%s\t%s\t%s\n\n", "����:д�ڴ�֡", "�������:д��ʧ��", "ʧ��ԭ��:�ڴ��ַԽ��");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }

        fwrite(__toSave, sizeof(char), frameSize, memoryFile__);
        fclose(memoryFile__);

        char * hexData__ = (char*)malloc(sizeof(char) * frameSize * 2);
        strToHex(__toSave,hexData__);

        sprintf(logContent__, "%s\t%s\t%s%s\n\n", "����:д�ڴ�֡", "�������:д��ɹ�", "д������:\n",hexData__);
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return true;
    }
}

