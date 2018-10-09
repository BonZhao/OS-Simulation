/**
 * MyOS\FileSystem.cpp
 *
 * Copyright (c) 2016 LyanQQ
 */

/**
 * Working log:
 * 2016/03/16 21:04		Created 			by LyanQQ ;
 * 2016/03/16 22:27		1st time coding 	by LyanQQ :
 * 		-Finish 4 functions :  compareTimeStamp(); extractTimeStamp(); packTimeStamp(); showTime();
 *								TimeStamp operations are available .
 * 2016/03/17 16:53		2nd time coding		by LyanQQ :
 * 		-Finish FileIndex operation functions : init; destroy; extract; pack .
 *		-Finish debug jobs : connect FileIndex operations and disk operations using "DiskDriver.cpp".
 *							Now we can read/modify/save FileIndex blocks from disk .
 * 2016/03/18 21:52		3rd time coding		by LyanQQ :
 * 		-Finish dataNode and childNode operating functions.
 *		-Finish all FolderIndex functions.
 * 2016/03/20 22:24		4th time coding		by LyanQQ :
 * 		-Finish all enviromental functions and free space management functions.
 * 2016/03/21 22:17		5th time coding		by LyanQQ :
 * 		-Some more lower layer interface finished.
 *		-File system API coding will start using all existing functions and interfaces.
 * 2016/03/22 22:03		6th time coding		by LyanQQ :
 * 		-Finish new folder and new file API.
 * 2016/03/24 22:32		7th time coding		by LyanQQ :
 * 		-Finish delete, rename, move API.
 * 2016/03/25 16:49		8th time coding		by LyanQQ :
 * 		-Finish copyFile API.
 * 2016/03/30 22:11		9th time coding		by LyanQQ :
 * 		-Finish open, close, read, save API.
 * 2016/03/31 20:33		10th time coding		by LyanQQ :
 * 		-Rewrite the APIs which finished yesterday because of the bad design of FileStream.
 *		-All essential APIs completed ;
 * 2016/05/14 21:33		11th time coding		by LyanQQ :
 * 		-Add file\Folder attributes.
 * 2016/05/19 23:49		12th time coding		by LyanQQ :
 * 		-Add more security controls when using file system APIs.
 */

#include "FileSystem.h"
#include "DiskDriver.h"
#include "Basic.h"
#include "ProcessManagement.h"
#include "WorkLog.h"
#include "Account.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * �ļ�ϵͳ�õ���ȫ�ֱ���
 */
const char * bitMapFileName = "bitMap.txt" ;/* λͼ��Ϣ�����ļ��� */
const char * FSConfigFileName = "FSConfig.txt" ;	/* �ļ�ϵͳ������Ϣ�ļ� */
char * bitVector = NULL ;					/* λͼ��ʹ��Ȩ���޸�Ȩ�����ڿ��пռ����ģ�� */

int rootDirectory = 0 ;						/* ���̸�Ŀ¼���ڵ������� */
char rootDirectoryLabel[LENGTH_NAME] ;		/* ��Ŀ¼��Ӧ�ľ�� */
int VMStartSection = 0 ;					/* ���ռ�Ŀ�ʼ���� */
int VMEndSection = 0 ;						/* ���ռ�Ľ������� */
int VMTotalSection = 0 ;					/* ���ռ���������� */

const char fileNameIllegalChar[10] = {9, '/', '\\', ':', '*', '?', '\"', '<', '>', '|'} ;

/**
 * ����ģ��ı���
 */
extern const char * configPath ;			/* ���Ի���ģ�飬���ڶ�ȡ�ļ�ϵͳ�����������Ϣ */
extern int sectionNumber ;						/* ���Դ�������ģ�飬�ڿ��пռ����ģ���õ� */
extern int sectionSize ;
extern const char * errorDesc[] ;
extern AccountLevelType accountLevel ;
extern bool LOG_OUTPUT;

/****************************************
 * ʱ�����������
 ****************************************/
/***************
 ** Debugͨ�� **
 ***************/

int compareTimeStamp(TimeStamp __time_1, TimeStamp __time_2)
{
    /**
     * �Ƚ�����ʱ�����ǰ�߽����򷵻�-1
     * ������ͬ����0�����߽��·���1
     */

    /* ��������ת����10������ */
    int date1 = __time_1.year * 10000 + __time_1.month * 100 + __time_1.day ;
    int date2 = __time_2.year * 10000 + __time_2.month * 100 + __time_2.day ;

    /* ֱ�ӱȽ� */
    if (date2 > date1) {
        return 1 ;
    }
    else if (date1 > date2) {
        return -1 ;
    }

    /* ��ʱ����ת����10������ */
    int time1 = __time_1.hour * 10000 + __time_1.min * 100 + __time_1.sec ;
    int time2 = __time_2.hour * 10000 + __time_2.min * 100 + __time_2.sec ;

    if (time2 > time1) {
        return 1 ;
    }
    else if (time1 > time2) {
        return -1 ;
    }

    /* ��� */
    return 0 ;
}

void extractTimeStamp(char * __buf, TimeStamp * __ts)
{
    /**
     * ��������������ʱ�������__buf��ʼ��7���ֽڣ�
     * ת����TimeStamp��ʽ��ʱ�䣬���ֽ��б����λ
     */
    __ts->year = (unsigned char)__buf[0]*256 + (unsigned char)__buf[1] ;
    __ts->month = (unsigned char)__buf[2] ;
    __ts->day = (unsigned char)__buf[3] ;
    __ts->hour = (unsigned char)__buf[4] ;
    __ts->min = (unsigned char)__buf[5] ;
    __ts->sec =(unsigned char)__buf[6] ;
}

void packTimeStamp(char * __buf, TimeStamp __ts)
{
    /**
     * ��ʱ������浽�����У���__buf��ʼ��7���ֽڣ�
     */
    __buf[0] = __ts.year / 256 ;
    __buf[1] = __ts.year % 256 ;
    __buf[2] = __ts.month ;
    __buf[3] = __ts.day ;
    __buf[4] = __ts.hour ;
    __buf[5] = __ts.min ;
    __buf[6] = __ts.sec ;
}

/****************************************
 * �ļ��������������
 ****************************************/
/***************
 ** Debugͨ�� **
 ***************/

void initFileIndex(FileIndex * __fi)
{
    /**
     * ��ʼ��FileIndex�ṹ����ֹ���ɿصĴ���
     */

    __fi->name = NULL ;
    __fi->dataNode = NULL ;
}

void destroyFileIndex(FileIndex * __fi)
{
    /**
     * �ͷ�FileIndex������Ŀռ�
     * ��initFileIndex()���ʹ��
     */

    free(__fi->name);
    free(__fi->dataNode);
}

void extractFileIndex(char * __sec, FileIndex * __fi)
{
    /**
     * ��ȡ�ļ��������и��ֶ���Ϣ�����浽FileIndex�ṹ��
     */

    /* ��ȡ��� */
    __fi->flag = __sec[FILE_OFFSET_FLAG] ;

    /* ��ȡ�ļ��� */
    /* ��ֹ�ظ�����ռ� */
    if (__fi->name == NULL) {
        __fi->name = (char *)malloc(sizeof(char) * LENGTH_NAME) ;
    }
    /* ��������name�ֶθ��Ƶ�FileIndex��name�� */
    copyString(__fi->name, &__sec[FILE_OFFSET_NAME], LENGTH_NAME) ;

    /* ��ȡʱ��� */
    extractTimeStamp(&__sec[FILE_OFFSET_CREATE_TIME], &(__fi->createTime));
    extractTimeStamp(&__sec[FILE_OFFSET_LAST_MODIFY_TIME], &(__fi->lastModifyTime));
    extractTimeStamp(&__sec[FILE_OFFSET_LAST_OPEN_TIME], &(__fi->lastOpenTime));

    /* ����1B��������ת�������ֽ��зŸ�λ */
    /* 256^3=16777216; 256^2=65536 */
    __fi->length = (unsigned char)__sec[FILE_OFFSET_LENGTH] * 16777216
                + (unsigned char)__sec[FILE_OFFSET_LENGTH + 1] * 65536
                + (unsigned char)__sec[FILE_OFFSET_LENGTH + 2] * 256
                + (unsigned char)__sec[FILE_OFFSET_LENGTH + 3] ;

    __fi->dataSectionNumber = (unsigned char)__sec[FILE_OFFSET_DATASEC_NUMBER] * 256
                            + (unsigned char)__sec[FILE_OFFSET_DATASEC_NUMBER + 1] ;

    __fi->fatherNode = (unsigned char)__sec[FILE_OFFSET_FATHER_NODE] * 256
                    + (unsigned char)__sec[FILE_OFFSET_FATHER_NODE + 1] ;

    /* ���ݿ�ڵ� */
    int dsn__ = __fi->dataSectionNumber ;

    if (dsn__ > 0) {
        __fi->dataNode = (int *)malloc(sizeof(int) * dsn__) ;

        for (int i = 0; i <= dsn__ - 1; i++) {
            (__fi->dataNode)[i] = (unsigned char)__sec[FILE_OFFSET_DATA_NODE + i*2] * 256
                                + (unsigned char)__sec[FILE_OFFSET_DATA_NODE + i*2 + 1] ;
        }
    }

    /* �ļ����� */
    if ((unsigned char)__sec[FILE_OFFSET_ATTRIBUTE] > 0) {
        __fi->attribute.hide = true ;
    }
    else {
        __fi->attribute.hide = false ;
    }
    if ((unsigned char)__sec[FILE_OFFSET_ATTRIBUTE + 1] > 0) {
        __fi->attribute.readOnly = true ;
    }
    else {
        __fi->attribute.readOnly = false ;
    }
}

void packFileIndex(char * __sec, FileIndex __fi)
{
    /**
     * ��FileIndex�ṹ���浽������__sec��
     * ���޸����������е��ֶ�ʱ��Ӧ�õ��ô˺���
     */

    for (int i=0; i<=sectionSize-1; i++) {
        __sec[i] = -1 ;
    }

    /* ������ */
    __sec[FILE_OFFSET_FLAG] = __fi.flag ;
    /* �����ļ��� */
    copyString(&__sec[FILE_OFFSET_NAME], __fi.name, LENGTH_NAME) ;

    /* ����ʱ��� */
    packTimeStamp(&__sec[FILE_OFFSET_CREATE_TIME], __fi.createTime);
    packTimeStamp(&__sec[FILE_OFFSET_LAST_MODIFY_TIME], __fi.lastModifyTime);
    packTimeStamp(&__sec[FILE_OFFSET_LAST_OPEN_TIME], __fi.lastOpenTime);

    /* ������ֵ�ֶ� */
    __sec[FILE_OFFSET_LENGTH] = __fi.length / 16777216 ;
    __sec[FILE_OFFSET_LENGTH + 1] = (__fi.length % 16777216) / 65536 ;
    __sec[FILE_OFFSET_LENGTH + 2] = (__fi.length % 65536) / 256 ;
    __sec[FILE_OFFSET_LENGTH + 3] = __fi.length % 256 ;

    __sec[FILE_OFFSET_DATASEC_NUMBER] = __fi.dataSectionNumber / 256 ;
    __sec[FILE_OFFSET_DATASEC_NUMBER + 1] = __fi.dataSectionNumber % 256 ;

    __sec[FILE_OFFSET_FATHER_NODE] = __fi.fatherNode / 256 ;
    __sec[FILE_OFFSET_FATHER_NODE + 1] = __fi.fatherNode % 256 ;

    /* �������ݽڵ� */
    int dsn__ = __fi.dataSectionNumber ;

    if (dsn__ > 0) {
        for (int i = 0; i <= dsn__ - 1; i++) {
            __sec[FILE_OFFSET_DATA_NODE + i*2] = __fi.dataNode[i] / 256 ;
            __sec[FILE_OFFSET_DATA_NODE + i*2 + 1] = __fi.dataNode[i] % 256 ;
        }
    }

    /* �����ļ����� */
    if (__fi.attribute.hide) {
        __sec[FILE_OFFSET_ATTRIBUTE] = 1 ;
    }
    else {
        __sec[FILE_OFFSET_ATTRIBUTE] = 0 ;
    }
    if (__fi.attribute.readOnly) {
        __sec[FILE_OFFSET_ATTRIBUTE + 1] = 1 ;
    }
    else {
        __sec[FILE_OFFSET_ATTRIBUTE + 1] = 0 ;
    }
}

void newFileIndex(FileIndex * __fi, char * __name, int __father)
{
    /**
     * �½��ļ������飬��������ֺ͸��ڵ�
     * ����ֵ����ʼ��Ϊ0
     */

    /* ��ʼ��Ϊ�ļ����� */
    __fi->flag = FILE_TYPE ;

    /* ��ʼ���ļ��� */
    /* ��ֹ�ظ�����ռ� */
    if (__fi->name == NULL) {
        __fi->name = (char *)malloc(sizeof(char) * LENGTH_NAME) ;
    }
    copyString(__fi->name, __name, LENGTH_NAME) ;

    /* ��ʼ��ʱ��� */
    TimeStamp ts__ ;

    getSystemTime(&ts__) ;
    __fi->createTime = ts__ ;
    __fi->lastModifyTime = ts__ ;
    __fi->lastOpenTime = ts__ ;

    /* ����ֵ��ʼ��Ϊ0 */
    __fi->length = 0 ;
    __fi->dataSectionNumber = 0 ;

    /* ��ʼ�����ڵ� */
    __fi->fatherNode = __father ;

    /* ��ʼ������ */
    __fi->attribute.hide = false ;
    __fi->attribute.readOnly = false ;
}

void copyIntArray(int * __aim, int * __from, int __long)
{
    for (int i=0; i <= __long - 1; i++) {
        __aim[i] = __from[i] ;
    }
}

bool moreDataNode(FileIndex * __fi, int __dsn, int * __secNum)
{
    /**
     * ��FileIndex�����__dsn�����ݿ�
     */

    int tot__ = __fi->dataSectionNumber + __dsn ;

    /* ����Ƿ񳬹������������� */
    if (tot__ > MAX_DATASEC_NUMBER || __dsn < 0) {
        return false ;
    }

    if (__dsn == 0) {
        return true ;
    }

    /* ���·������������__secNum�����__fi->dataNode��β�� */
    int * newSec__ = (int *)malloc(sizeof(int) * tot__) ;

    copyIntArray(newSec__, __fi->dataNode, __fi->dataSectionNumber) ;
    copyIntArray(&newSec__[__fi->dataSectionNumber], __secNum, __dsn) ;
    __fi->dataSectionNumber = tot__ ;

    free(__fi->dataNode) ;

    __fi->dataNode = newSec__ ;

    return true ;
}

bool releaseDataNode(FileIndex * __fi, int __dsn)
{
    /**
     * ��FileIndex���ͷ�__dsn����������
     * ��dataNodeβ��Ĩ��__dsn������
     */

    int tot__ = __fi->dataSectionNumber - __dsn ;

    /* ���ݺϷ��Լ�� */
    if (__dsn < 0 || tot__ < 0) {
        return false ;
    }

    if (__dsn == 0) {
        return true ;
    }

    if (tot__ == 0) {
        free(__fi->dataNode) ;
        __fi->dataNode = NULL ;
        __fi->dataSectionNumber = 0 ;
        return true ;
    }

    /* ��β��Ĩ��__dsn�����ݿ� */
    int * newSec__ = (int *)malloc(sizeof(int) * tot__) ;

    copyIntArray(newSec__, __fi->dataNode, tot__) ;
    __fi->dataSectionNumber = tot__ ;

    free(__fi->dataNode) ;
    __fi->dataNode = newSec__ ;

    return true ;
}

/****************************************
 * �ļ����������������
 ****************************************/
/***************
 ** Debugͨ�� **
 ***************/

void initFolderIndex(FolderIndex * __foi)
{
    /**
     * ��ʼ��FolderIndex�ṹ����ֹ���ɿصĴ���
     */

    __foi->name = NULL ;
    __foi->childNode = NULL ;
}

void destroyFolderIndex(FolderIndex * __foi)
{
    /**
     * �ͷ�FolderIndex������Ŀռ�
     * ��initFolderIndex()���ʹ��
     */

    free(__foi->name);
    free(__foi->childNode);
}

void extractFolderIndex(char * __sec, FolderIndex * __foi)
{
    /**
     * ��ȡ�ļ��������и��ֶ���Ϣ�����浽FolderIndex�ṹ��
     */

    /* ��ȡ��� */
    __foi->flag = __sec[FOLDER_OFFSET_FLAG] ;

    /* ��ȡ�ļ��� */
    /* ��ֹ�ظ�����ռ� */
    if (__foi->name == NULL) {
        __foi->name = (char *)malloc(sizeof(char) * LENGTH_NAME) ;
    }
    /* ��������name�ֶθ��Ƶ�FolderIndex��name�� */
    copyString(__foi->name, &__sec[FOLDER_OFFSET_NAME], LENGTH_NAME) ;

    /* ��ȡʱ��� */
    extractTimeStamp(&__sec[FOLDER_OFFSET_CREATE_TIME], &(__foi->createTime));

    /* ����1B��������ת�������ֽ��зŸ�λ */
    __foi->childNodeNumber = (unsigned char)__sec[FOLDER_OFFSET_CHILD_NUMBER] * 256
                + (unsigned char)__sec[FOLDER_OFFSET_CHILD_NUMBER + 1] ;

    __foi->fatherNode = (unsigned char)__sec[FOLDER_OFFSET_FATHER_NODE] * 256
            + (unsigned char)__sec[FOLDER_OFFSET_FATHER_NODE + 1] ;

    /* �ӽڵ� */
    int dsn__ = __foi->childNodeNumber ;

    if (dsn__ > 0) {
        __foi->childNode = (int *)malloc(sizeof(int) * dsn__) ;

        for (int i = 0; i <= dsn__ - 1; i++) {
            (__foi->childNode)[i] = (unsigned char)__sec[FOLDER_OFFSET_CHILD_NODE + i*2] * 256
                        + (unsigned char)__sec[FOLDER_OFFSET_CHILD_NODE + i*2 + 1] ;
        }
    }

    /* ��ȡ���� */
    if ((unsigned char)__sec[FOLDER_OFFSET_ATTRIBUTE] > 0) {
        __foi->attribute.hide = true ;
    }
    else {
        __foi->attribute.hide = false ;
    }
    if ((unsigned char)__sec[FOLDER_OFFSET_ATTRIBUTE + 1] > 0) {
        __foi->attribute.protect = true ;
    }
    else {
        __foi->attribute.protect = false ;
    }
}

void packFolderIndex(char * __sec, FolderIndex __foi)
{
    /**
     * ��FolderIndex�ṹ���浽������__sec��
     * ���޸����������е��ֶ�ʱ��Ӧ�õ��ô˺���
     */

    for (int i=0; i<=sectionSize-1; i++) {
        __sec[i] = -1 ;
    }

    /* ������ */
    __sec[FOLDER_OFFSET_FLAG] = __foi.flag ;
    /* �����ļ��� */
    copyString(&__sec[FOLDER_OFFSET_NAME], __foi.name, LENGTH_NAME) ;

    /* ����ʱ��� */
    packTimeStamp(&__sec[FOLDER_OFFSET_CREATE_TIME], __foi.createTime);

    /* ������ֵ�ֶ� */
    __sec[FOLDER_OFFSET_CHILD_NUMBER] = __foi.childNodeNumber / 256 ;
    __sec[FOLDER_OFFSET_CHILD_NUMBER + 1] = __foi.childNodeNumber % 256 ;

    __sec[FOLDER_OFFSET_FATHER_NODE] = __foi.fatherNode / 256 ;
    __sec[FOLDER_OFFSET_FATHER_NODE + 1] = __foi.fatherNode % 256 ;

    /* �������ݽڵ� */
    int dsn__ = __foi.childNodeNumber ;

    if (dsn__ > 0) {
        for (int i = 0; i <= dsn__ - 1; i++) {
            __sec[FOLDER_OFFSET_CHILD_NODE + i*2] = __foi.childNode[i] / 256 ;
            __sec[FOLDER_OFFSET_CHILD_NODE + i*2 + 1] = __foi.childNode[i] % 256 ;
        }
    }

    /* �������� */
    if (__foi.attribute.hide) {
        __sec[FOLDER_OFFSET_ATTRIBUTE] = 1 ;
    }
    else {
        __sec[FOLDER_OFFSET_ATTRIBUTE] = 0 ;
    }
    if (__foi.attribute.protect) {
        __sec[FOLDER_OFFSET_ATTRIBUTE + 1] = 1 ;
    }
    else {
        __sec[FOLDER_OFFSET_ATTRIBUTE + 1] = 0 ;
    }
}

void newFolderIndex(FolderIndex * __foi, char * __name, int __father)
{
    /**
     * �½��ļ��������飬��������ֺ͸��ڵ�
     * ����ֵ����ʼ��Ϊ0
     */

    /* ��ʼ��Ϊ�ļ������� */
    __foi->flag = FOLDER_TYPE ;

    /* ��ʼ���ļ��� */
    /* ��ֹ�ظ�����ռ� */
    if (__foi->name == NULL) {
        __foi->name = (char *)malloc(sizeof(char) * LENGTH_NAME) ;
    }
    copyString(__foi->name, __name, LENGTH_NAME) ;

    /* ��ʼ��ʱ��� */
    TimeStamp ts__ ;

    getSystemTime(&ts__) ;
    __foi->createTime = ts__ ;

    /* �ӽڵ�������ʼ��Ϊ0 */
    __foi->childNodeNumber = 0 ;

    /* ��ʼ�����ڵ� */
    __foi->fatherNode = __father ;

    /* ��ʼ������ */
    __foi->attribute.hide = false ;
    __foi->attribute.protect = false ;
}

bool moreChildNode(FolderIndex * __foi, int __dsn, int * __secNum)
{
    /**
     * ��FolderIndex�����__dsn�����ݿ�
     */

    int tot__ = __foi->childNodeNumber + __dsn ;

    /* ����Ƿ񳬹������������� */
    if (tot__ > MAX_CHILD_NUMBER || __dsn < 0) {
        return false ;
    }

    if (__dsn == 0) {
        return true ;
    }

    /* ���·������������__secNum�����__foi->childNode��β�� */
    int * newSec__ = (int *)malloc(sizeof(int) * tot__) ;

    copyIntArray(newSec__, __foi->childNode, __foi->childNodeNumber) ;
    copyIntArray(&newSec__[__foi->childNodeNumber], __secNum, __dsn) ;
    __foi->childNodeNumber = tot__ ;

    free(__foi->childNode) ;

    __foi->childNode = newSec__ ;

    return true ;
}

bool releaseChildNode(FolderIndex * __foi, int __dsn)
{
    /**
     * ��FolderIndex���ͷ�__dsn����������
     * ��childNodeβ��Ĩ��__dsn������
     */

    int tot__ = __foi->childNodeNumber - __dsn ;

    /* ���ݺϷ��Լ�� */
    if (__dsn < 0 || tot__ < 0) {
        return false ;
    }

    if (__dsn == 0) {
        return true ;
    }

    if (tot__ == 0) {
        free(__foi->childNode) ;
        __foi->childNode = NULL ;
        __foi->childNodeNumber = 0 ;
        return true ;
    }

    /* ��β��Ĩ��__dsn�����ݿ� */
    int * newSec__ = (int *)malloc(sizeof(int) * tot__) ;

    copyIntArray(newSec__, __foi->childNode, tot__) ;
    __foi->childNodeNumber = tot__ ;

    free(__foi->childNode) ;
    __foi->childNode = newSec__ ;

    return true ;
}

bool deleteChildNode(FolderIndex * __foi, int __secNum)
{
    /**
     * ��FolderIndex��ɾ��һ��������Ϊ__secNum���ӽڵ�
     */

    /* ���ݲ��Ϸ� */
    if (__secNum < 0) {
        setError(SYSTEM_ERROR) ;
        return false ;
    }

    int i, j ;
    for (i = 0; i <= __foi->childNodeNumber - 1; i++) {
        /* �ҵ��ýڵ� */
        if (__secNum == __foi->childNode[i]) {
            /* �ӽڵ�����һ */
            __foi->childNodeNumber-- ;

            if (__foi->childNodeNumber == 0) {
                free(__foi->childNode);
                __foi->childNode = NULL ;
                return true ;
            }
            else {
                int * child__ = (int *)malloc(sizeof(int) * __foi->childNodeNumber) ;

                /* �����µ��ӽڵ����� */
                for (j = 0; j <= i-1; j++) {
                    child__[j] = __foi->childNode[j] ;
                }
                for (j = j+1; j <= __foi->childNodeNumber; j++) {
                    child__[j-1] = __foi->childNode[j] ;
                }

                free(__foi->childNode) ;
                __foi->childNode = child__ ;

                return true ;
            }
        }
    }

    setError(SYSTEM_ERROR) ;
    return false ;
}

/****************************************
 * �ļ�ϵͳ��������
 ****************************************/
/***************
 ** Debugͨ�� **
 ***************/

bool newBitVector(int __sectionNumber)
{
    /**
     * ���̸�ʽ���������µ�����λͼ
     * λͼ��0,1��ʾ��0��ʾ�������ã�1��ʾ������
     */

    /* λͼ�ļ�·�� */
    char * bvPath__ = (char *)malloc(sizeof(char) * 38) ;
    sprintf(bvPath__, "%s%s", configPath, bitMapFileName) ;

    FILE * ofp = fopen(bvPath__, "wb");
    /* λͼ�ļ�·������ */
    if (ofp == NULL) {
        free(bvPath__) ;
        return false ;
    }

    /* д���µ�λͼ����ʼ��Ϊȫ���� */
    char bit = SECTION_AVAILABLE;
    for (int i=0; i <= __sectionNumber -1; i++) {
        fwrite(&bit, sizeof(bit), 1, ofp);
    }

    fclose(ofp) ;
    free(bvPath__) ;

    return true ;
}

bool getBitVector(char * __bitMap)
{
    /**
     * ����ϵͳ����ʱ��λͼ�ļ��л�ȡλͼ��Ϣ
     * λ����������������ϢsectionNumberָ��
     * ���ô˺���ǰ���뱣֤������Ϣ�Ѿ���ȡ�ɹ�
     * __bitMapָ����ڴ���������㹻��
     */

    /* λͼ�ļ�·�� */
    char * bvPath__ = (char *)malloc(sizeof(char) * 38) ;
    sprintf(bvPath__, "%s%s", configPath, bitMapFileName) ;

    FILE * ifp = fopen(bvPath__, "rb");
    /* λͼ�ļ�·������ */
    if (ifp == NULL) {
        free(bvPath__) ;
        return false ;
    }

    /* ��ȡλͼ */
    fread(__bitMap, sizeof(char), sectionNumber, ifp);
    /* ��β��\0�����ڵ��� */
    //__bitMap[sectionNumber] = '\0';

    fclose(ifp) ;
    free(bvPath__) ;

    return true ;
}

bool saveBitVector(char * __bitMap)
{
    /**
     * ��__bitMapλͼ��Ϣ���浽λͼ�ļ���
     * ÿ��λͼ���޸ĺ󶼱�����øú�������֤һ����
     */

    /* λͼ�ļ�·�� */
    char * bvPath__ = (char *)malloc(sizeof(char) * 38) ;
    sprintf(bvPath__, "%s%s", configPath, bitMapFileName) ;

    FILE * ofp = fopen(bvPath__, "wb");
    /* λͼ�ļ�·������ */
    if (ofp == NULL) {
        free(bvPath__) ;
        return false ;
    }

    /* ����λͼ */
    fwrite(__bitMap, sizeof(char), sectionNumber, ofp);

    fclose(ofp) ;
    free(bvPath__) ;

    return true ;
}

bool readFSConfig()
{
    /**
     * ��ȡ���̵�������Ϣ���˲���Ӧ����
     * ����ϵͳ����ʱ��BIOS�������
     */

    /* �����ļ�·�� */
    char * configPath__ = (char *)malloc(sizeof(char) * 38) ;
    sprintf(configPath__, "%s%s", configPath, FSConfigFileName);

    /* �������ļ���ʧ���򷵻�false */
    FILE * ifp = fopen(configPath__, "r");
    if (ifp == NULL) {
        free(configPath__);
        return false ;
    }

    if (fscanf(ifp, "%d %s %d %d", &rootDirectory, rootDirectoryLabel, &VMStartSection, &VMEndSection) != 4) {
        fclose(ifp);
        free(configPath__);
        return false ;
    }

    VMTotalSection = VMEndSection - VMStartSection + 1 ;

    fclose(ifp);
    free(configPath__);

    /* ��ȡ�ɹ� */
    return true ;
}

bool isUserSpace(int __sec)
{
    /**
     * ����Ƿ����û��ռ䣬��ֹԽ��
     */

    if ((__sec == rootDirectory) || (__sec>=VMStartSection && __sec<=VMEndSection) || (__sec < 0) || (__sec >= sectionNumber)) {
        return false ;
    }
    else {
        return true ;
    }
}

bool initFileSystem()
{
    /**
     * ��ʼ���ļ�ϵͳģ��Ļ���
     * ��ȡ����ʼ���ļ�ϵͳ��������Ϣ
     * ��ϵͳ����ʱ����
     */

    /* ��ȡӲ��������Ϣ */
    if (!readDiskConfig()) {
        return false ;
    }

    /* ��ȡλͼ��Ϣ */
    bitVector = (char *)malloc(sizeof(char) * sectionNumber) ;
    if (!getBitVector(bitVector)) {
        return false ;
    }

    /* ��ȡ�ļ�ϵͳ������Ϣ */
    if (!readFSConfig()) {
        return false ;
    }

    return true ;
}

/****************************************
 * �ļ�ϵͳ���пռ������
 ****************************************/
/***************
 ** Debugͨ�� **
 ***************/

int queryFreeSpace()
{
    /**
     * ��ѯλͼ�����ؿ��õĴ�����������
     */

    int remain__ = sectionNumber - VMTotalSection - 1 ;
    int i = 0 ;

    for (i = 0; i <= VMStartSection - 1; i++) {
        if (bitVector[i] == SECTION_TAKEN && i!=rootDirectory) {
            remain__-- ;
        }
    }

    for (i = VMEndSection + 1; i <= sectionNumber - 1; i++) {
        if (bitVector[i] == SECTION_TAKEN && i!=rootDirectory) {
            remain__-- ;
        }
    }

    /* ������־��� */
    if (LOG_OUTPUT) {
        logPrintf(FREE_SPACE_LOG, "���������пռ��ѯ\nʣ����õĴ�����������%d\n\n", remain__) ;
    }

    return remain__ ;
}

bool applyForFreeSpace(int __many, int * __allocated)
{
    /**
     * �����ļ��ռ䣬����ɹ�������������ŷ���__allocated
     * ʧ���򷵻�false
     */

    int i, j = 0 ;

    /* ����������������������false */
    if (__many < 0 || __many > queryFreeSpace()) {

        /* ������־��� */
        if (LOG_OUTPUT) {
            logPrintf(FREE_SPACE_LOG, "���������пռ�����\n�������������%d\t�����������㣬����ʧ��\n\n", __many) ;
        }

        return false ;
    }

    /* ���ҿɷ������������¼��__allocated�� */
    for (i=0; i <= sectionNumber - 1 && j < __many; i++) {
        /* �ܿ���Ŀ¼��������� */
        if ((i!=rootDirectory) && (i<VMStartSection || i>VMEndSection)) {
            if (bitVector[i] == SECTION_AVAILABLE) {
                bitVector[i] = SECTION_TAKEN ;
                __allocated[j] = i ;
                j++ ;
            }
        }
    }

    /* ��û�з����㹻�Ŀռ䣬���ͷ��ѷ���Ŀռ䲢����false */
    if (j != __many) {
        for (i=0; i <= j-1; i++) {
            bitVector[__allocated[i]] = SECTION_AVAILABLE ;
        }

        return false ;
    }
    else {
        /* ����ɹ�������λͼ��Ϣ */
        saveBitVector(bitVector);

        /* ������־��� */
        if (LOG_OUTPUT) {
            char buf[255] = "" ;

            for (i = 0; i <= __many-1; i++) {
                sprintf(buf, "%s %d", buf, __allocated[i]);
            }

            logPrintf(FREE_SPACE_LOG, "���������пռ�����\n�������������%d\t����ɹ�\n����������ţ�%s\n\n", __many, buf) ;
        }

        return true ;
    }
}

bool releaseFreeSpace(int __many, const int * __allocated)
{
    /**
     * �ͷ�__allocated�м�¼�Ĵ�������
     * ʧ���򷵻�false
     */

    int sec__ ;

    /* ���ݺϷ��Լ�� */
    if (__many < 0) {
        return false ;
    }

    for (int i=0; i <= __many - 1; i++) {
        sec__ = __allocated[i] ;

        /* ��ͼ�ͷ������Ŀ¼��ϵͳ�������� */
        if ((sec__>=VMStartSection && sec__<=VMEndSection) || sec__==rootDirectory) {
            return false ;
        }
        else {
            bitVector[sec__] = SECTION_AVAILABLE ;
        }
    }

    /* ����λͼ��Ϣ */
    saveBitVector(bitVector);

    /* ������־��� */
    if (LOG_OUTPUT) {
        char buf[255] = "" ;

        for (int i = 0; i <= __many-1; i++) {
            sprintf(buf, "%s %d", buf, __allocated[i]);
        }

        logPrintf(FREE_SPACE_LOG, "���������пռ��ͷ�\n�ͷŵ���������%d\t�ͷųɹ�\n�ͷŵ������ţ�%s\n\n", __many, buf) ;
    }

    return true ;
}

bool recycleFileSpace(FileIndex __fi, int __secNum)
{
    /**
     * ��ָ���ļ�������ʹ�õĿռ����
     * �贫����ļ���������Ϣ�������������λ��
     */

    /* ���ݿ��������Ϸ� */
    if (__fi.dataSectionNumber < 0) {
        setError(SYSTEM_ERROR) ;
        return false ;
    }

    /* ���ļ������ݿ� */
    if (__fi.dataSectionNumber > 0) {
        if (!releaseFreeSpace(__fi.dataSectionNumber, __fi.dataNode)) {
            return false ;
        }
    }

    /* �ͷ����������� */
    if (releaseFreeSpace(1, &__secNum)) {
        return true ;
    }
    else {
        return false ;
    }
}

bool recycleFolderSpace(FolderIndex __fo, int __secNum)
{
    /**
     * ��ָ���ļ��е�����ʹ�õĿռ���գ������������е��ļ����ļ��У�
     * �贫����ļ���������Ϣ�������������λ��
     */

    /* ���ݿ��������Ϸ� */
    if (__fo.childNodeNumber < 0) {
        setError(SYSTEM_ERROR) ;
        return false ;
    }

    /* ���ļ������ӽڵ� */
    if (__fo.childNodeNumber > 0) {
        int i ;
        char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
        FileIndex fi_child__ ;
        FolderIndex fo_child__ ;

        for (i = 0; i<=__fo.childNodeNumber-1; i++) {
            if (readSection(sec__, __fo.childNode[i])) {
                /* �ӽڵ����ļ� */
                if (sec__[0] == FILE_TYPE) {
                    initFileIndex(&fi_child__) ;
                    extractFileIndex(sec__, &fi_child__) ;

                    /* �����ļ��ռ� */
                    if (!recycleFileSpace(fi_child__, __fo.childNode[i])) {
                        free(sec__) ;
                        destroyFileIndex(&fi_child__) ;
                        return false ;
                    }

                    destroyFileIndex(&fi_child__) ;
                }
                /* �ӽڵ����ļ��� */
                else if (sec__[0] == FOLDER_TYPE) {
                    initFolderIndex(&fo_child__) ;
                    extractFolderIndex(sec__, &fo_child__) ;

                    if (!recycleFolderSpace(fo_child__, __fo.childNode[i])) {
                        free(sec__) ;
                        destroyFolderIndex(&fo_child__);
                        return false ;
                    }

                    destroyFolderIndex(&fo_child__);
                }
                /* �ӽڵ��Ǹ�Ŀ¼���Ƿ����� */
                else {
                    free(sec__) ;
                    setError(ILLEGAL_ACCESS_SYSTEM_DISK);
                    return false ;
                }
            }
            else {
                free(sec__) ;
                setError(READ_WRITE_DISK_ERROR);
                return false ;
            }
        }
    }

    /* �ͷ����������� */
    if (releaseFreeSpace(1, &__secNum)) {
        return true ;
    }
    else {
        return false ;
    }
}

/****************************************
 * �����ײ�ӿ�
 ****************************************/

bool copySection(int __aim, int __from)
{
    /**
     * ���������е����ݣ���__from���Ƶ�__aim
     */

    /* ����Ƿ����û��ռ� */
    if (!(isUserSpace(__aim) && isUserSpace(__from))) {
        return false ;
    }

    if (__aim == __from) {
        return true ;
    }

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;

    /* ��ȡԭ���������浽Ŀ����������ɸ��� */
    if (readSection(sec__, __from)) {
        if (saveSection(sec__, __aim)) {
            free(sec__) ;
            return true ;
        }
    }

    free(sec__) ;
    return false ;
}

bool checkFileName(char * __fn)
{
    /**
     * �ļ����Ϸ��Լ�⣬��������
     * 1.���ܳ���37��Ӣ���ַ���������׺����
     * 2.���ܺ��������κ��ַ��� / \ : * ? " < > |
     * 3.�ļ�����ͷ�����ǿո�
     */

    int len__ ;
    int i, j ;
    char * name__ = (char *)malloc(sizeof(char) * LENGTH_NAME);
    char * ex__ = (char *)malloc(sizeof(char) * LENGTH_NAME);

    len__ = strlen(__fn) ;
    /* ��������ǰ��Ŀո� */
    i = 0;
    while (__fn[i]==' ') {
        i++ ;
    }
    for (j=i; j<=len__; j++) {
        __fn[j-i] = __fn[j] ;
    }

    len__ = strlen(__fn) ;
    /* �������ƺ���Ŀո� */
    i = len__-1;
    while (__fn[i]==' ' || __fn[i]=='.') {
        i-- ;
    }
    __fn[i+1] = '\0' ;

    len__ = strlen(__fn) ;
    /* ���ȳ��� */
    if (len__ + 1 > LENGTH_NAME) {
        setError(FILE_NAME_TOO_LONG);
        free(name__);
        free(ex__);
        return false ;
    }

    extractFileName(name__, ex__, __fn);
    /* �ļ�������Ϊ�� */
    if (strlen(name__) == 0) {
        setError(FILE_NAME_EMPTY);
        free(name__);
        free(ex__);
        return false ;
    }

    /* �������ַ� */
    for (i=0; i<=len__-1; i++) {
        for (j=1; j<=fileNameIllegalChar[0]; j++) {
            if (__fn[i] == fileNameIllegalChar[j]) {
                setError(ILLEGAL_CHAR_IN_FILE_NAME);
                free(name__);
                free(ex__);
                return false ;
            }
        }
    }

    free(name__);
    free(ex__);
    return true ;
}

void extractFileName(char * __name, char * __ex, const char * __whole)
{
    /**
     * �������ļ�������ȡ�����ֺͺ�׺
     * �ֱ�浽__name��__ex��
     */
    int len_w = strlen(__whole) ;
    int i = len_w -1 ;

    while (__whole[i]!='.' && i>=0) {
        i-- ;
    }

    /* ��û��С���㣬�����С�������ַ������ */
    if (i<0) {
        i = len_w ;
    }

    /* �������� */
    copyString(__name, __whole, i);
    __name[i] = '\0';

    if (i>=len_w -1) {
        /* û�к�׺����Ϊ�� */
        __ex[0] = '\0';
    }
    else {
        /* ���ƺ�׺�� */
        copyString(__ex, &__whole[i+1], len_w - i -1) ;
        __ex[len_w - i -1] = '\0';
    }
}

bool extractFilePath(const char * __whole_path, char * __path, int __which)
{
    /**
     * �������ļ�·��__whole_path����ȡ��__which��·��
     * ����__path�У���Ŀ¼�ǵ�0�㣬��ȡʧ���򷵻�false
     */

    if (__which < 0 || __whole_path == NULL) {
        return false ;
    }

    int len_wp = strlen(__whole_path) ;
    int i ;

    /* ��ȡ��Ŀ¼��� */
    if (__which == 0) {
        for (i = 0; i<=len_wp-1; i++) {
            if (__whole_path[i] == ':' && (__whole_path[i+1] == '\\' || __whole_path[i+1] == '\0')) {
                copyString(__path, __whole_path, i) ;
                __path[i] = '\0' ;
                return true ;
            }
        }

        return false ;
    }
    else {
        int count = 0 , j = 0;

        /* ��ȡ��__which��·�� */
        for (i = 0; i<=len_wp ; i++) {
            if (__which == count) {
                if (__whole_path[i] == '\\' || __whole_path[i] == '\0') {
                    __path[j] = '\0' ;

                    if (j!=0) {
                        return true ;
                    }
                    else {
                        return false ;
                    }
                }
                else {
                    __path[j] = __whole_path[i] ;
                    j++ ;
                }
            }
            else if (__whole_path[i] == '\\') {
                count ++ ;
            }
        }

        return false ;
    }
}

bool createRootDirectory()
{
    /**
     * �ļ�ϵͳ�״γ�ʼ��ʱ�贴����Ŀ¼
     */

    FolderIndex root__ ;
    char name__[38] = "���ش���" ;
    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;

    for (int i=0; i<=sectionSize-1; i++) {
        sec__[i] = -1 ;
    }

    /* �½���Ŀ¼������ */
    initFolderIndex(&root__);

    newFolderIndex(&root__, name__, NULL_POINTER) ;
    root__.flag = ROOT_TYPE ;

    packFolderIndex(sec__, root__) ;

    /* ���浽��Ŀ¼���� */
    if (!saveSection(sec__, rootDirectory)) {
        destroyFolderIndex(&root__);
        free(sec__);
        return false ;
    }

    destroyFolderIndex(&root__);
    free(sec__) ;

    return true ;
}

bool extractName(char * __name, int __secNum)
{
    /**
     * ֻ����������ȡ�������ֶΣ���������
     */

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;

    if (!readSection(sec__, __secNum)) {
        free(sec__);
        return false ;
    }

    copyString(__name, &sec__[FOLDER_OFFSET_NAME], LENGTH_NAME) ;

    free(sec__) ;
    return true ;
}

bool searchChildNode(int * __secNum, FolderIndex __fo, char * __name)
{
    /**
     * �����ļ���������������Ϊ__name���ӽڵ�
     * ���У����ӽڵ�ɽ����д��__secNum
     */

    /* �ļ�����û���ӽڵ� */
    if (__fo.childNodeNumber == 0) {
        return false ;
    }

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    char * name__ = (char *)malloc(sizeof(char) * LENGTH_NAME) ;

    for (int i = 0; i <= __fo.childNodeNumber -1; i++) {
        /* ��ȡ�ӽڵ������ */
        if (!extractName(name__, __fo.childNode[i])) {
            free(sec__) ;
            free(name__) ;
            return false ;
        }
        /* ��������ȣ���������д��__secNum */
        else if (strcmp(name__, __name) == 0) {
            (*__secNum) = __fo.childNode[i] ;

            free(sec__) ;
            free(name__) ;
            return true ;
        }
    }

    free(sec__) ;
    free(name__) ;
    /* δ�ҵ�������ͬ���ӽڵ� */
    return false ;
}

bool getIndexSection(char * __sec, int * __secNum, const char * __whole_path)
{
    /**
     * ��������·�����ļ�ϵͳ���������ļ����ļ���
     * �������鲢д��__sec��������������Ŵ���__secNum
     * ���ļ������ļ��У�·����Ч�򷵻�false
     */

    char path__[LENGTH_NAME] ;
    FolderIndex fo__ ;
    int ns__ ;
    int level__ ;

    /* ��λ��Ŀ¼ */
    if (!extractFilePath(__whole_path, path__, 0)) {
        return false ;
    }
    /* ����Ŀ¼��� */
    if (strcmp(rootDirectoryLabel, path__) == 0) {
        /* ��ȡ��Ŀ¼���� */
        if (!readSection(__sec, rootDirectory)) {
            return false ;
        }

        (*__secNum) = rootDirectory ;
    }

    level__ = 1 ;
    while (extractFilePath(__whole_path, path__, level__) && (__sec[0] == FOLDER_TYPE || __sec[0] == ROOT_TYPE)) {
        /*  */
        if (__sec[0] == FILE_TYPE) {
            return false ;
        }
        /* ��ȡ��������Ϣ */
        initFolderIndex(&fo__);
        extractFolderIndex(__sec, &fo__) ;

        /* ���ҵ���һ��Ŀ¼ */
        if (searchChildNode(&ns__, fo__, path__)) {
            /* ��ȡ��һ�������� */
            if (!readSection(__sec, ns__)) {
                return false ;
            }

            /* ��¼����λ�� */
            (*__secNum) = ns__ ;

            /* ׼����ȡ��һ�� */
            level__++ ;

            destroyFolderIndex(&fo__);
        }
        else {
            destroyFolderIndex(&fo__);
            return false ;
        }
    }

    return true ;
}

bool fileNotOpen(const char * __path)
{
    /**
     * ����ļ��Ƿ�ĳ�����̴�
     */

    return checkOpenFile(__path) ;
}

bool folderNotOpen(const char * __path)
{
    /**
     * ����ļ��µ�ĳ���ļ��Ƿ�ĳ�����̴�
     */

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    int secNum__ ;

    if (getIndexSection(sec__, &secNum__, __path)) {
        FolderIndex fo__ ;

        initFolderIndex(&fo__);
        extractFolderIndex(sec__, &fo__);

        char newPath__[255] = "" ;

        for (int i=0; i<=fo__.childNodeNumber-1; i++) {;

            if (readSection(sec__, fo__.childNode[i])) {

                /* �ӽڵ����ļ� */
                if (sec__[FILE_OFFSET_FLAG] == FILE_TYPE) {
                    sprintf(newPath__, "%s\\%s", __path, &sec__[FILE_OFFSET_NAME]);

                    if (!fileNotOpen(newPath__)) {
                        destroyFolderIndex(&fo__);
                        free(sec__) ;
                        return false ;
                    }
                }
                /* �ӽڵ����ļ��� */
                else {
                    sprintf(newPath__, "%s\\%s", __path, &sec__[FOLDER_OFFSET_NAME]);

                    if (!folderNotOpen(newPath__)) {
                        destroyFolderIndex(&fo__);
                        free(sec__) ;
                        return false ;
                    }
                }
            }
            else {
                setError(READ_WRITE_DISK_ERROR);
            }
        }

        destroyFolderIndex(&fo__);
        free(sec__) ;
        return true ;
    }
    else {
        ;
    }

    free(sec__) ;
    return true ;
}

bool isSubFolder(const char * __dest, const char * __source)
{
    int lenS = strlen(__source) ;
    char dname[40], sname[40] ;

    if (lenS > strlen(__dest)) {
        return false ;
    }
    else {
        char dname[40], sname[40] ;
        int i = 0 ;

        while (extractFilePath(__source, sname, i)) {
            if (extractFilePath(__dest, dname, i)) {
                if (strcmp(sname, dname) != 0) {
                    return false ;
                }
            }
            else {
                return false ;
            }

            i++ ;
        }

        return true ;
    }
}


/****************************************
 * �ļ�ϵͳAPI
 ****************************************/

bool newFolder(char * __name, const char * __path)
{
    /**
     * ��ָ��·��__path�½�һ������Ϊ__name�Ŀ��ļ���
     */

    /* ������ֺϷ��� */
    if (!checkFileName(__name)) {

        /* ������־��� */
        if (LOG_OUTPUT) {
            logPrintf(FILE_API_LOG, "�������½��ļ���\n�ļ������ƣ�%s\n·����%s\n�½�ʧ�ܣ�ԭ��%s\n\n", __name, __path, errorDesc[errorCode]) ;
        }

        return false ;
    }

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    FolderIndex fo_father__, fo_child__ ;
    int sn_father__ ;
    int sn__ ;

    if (getIndexSection(sec__, &sn_father__, __path)) {
        /* ָ����·�����ļ��У�������½� */
        if (sec__[0] != FILE_TYPE) {
            /* ��ȡĿ��·���ļ�����Ϣ */
            initFolderIndex(&fo_father__) ;
            extractFolderIndex(sec__, &fo_father__) ;

            /* ����Ƿ�ﵽ�½��ļ����� */
            if (fo_father__.childNodeNumber + 1 <= MAX_CHILD_NUMBER)
            {
                /* û��ͬ���ļ����ļ��� */
                if (!searchChildNode(&sn__, fo_father__, __name)) {
                    int allo__ ;
                    /* ����һ�������������ռ� */
                    if (applyForFreeSpace(1, &allo__)) {
                        /* �½��ļ��������� */
                        initFolderIndex(&fo_child__) ;
                        newFolderIndex(&fo_child__, __name, sn_father__);
                        packFolderIndex(sec__, fo_child__) ;
                        destroyFolderIndex(&fo_child__) ;

                        /* �����ļ��������������䵽������ */
                        if (saveSection(sec__, allo__)) {
                            /* ΪĿ���ļ������һ���ӽڵ� */
                            moreChildNode(&fo_father__, 1, &allo__);

                            packFolderIndex(sec__, fo_father__);
                            if (saveSection(sec__, sn_father__)) {
                                destroyFolderIndex(&fo_father__) ;
                                free(sec__);

                                if (LOG_OUTPUT) {
                                    logPrintf(FILE_API_LOG, "�������½��ļ���\n�ļ������ƣ�%s\n·����%s\n�½��ɹ�\n\n"
                                                    , __name, __path) ;
                                }

                                return true ;
                            }
                            else {
                                setError(READ_WRITE_DISK_ERROR);
                            }
                        }
                        else {
                            setError(READ_WRITE_DISK_ERROR);
                        }
                    }
                    else {
                        setError(NO_ENOUGH_DISK_SPACE);
                    }
                }
                else {
                    setError(SAME_NAME);
                }
            }
            else {
                setError(NO_ENOUGH_CHILD_NODE);
            }

            destroyFolderIndex(&fo_father__) ;
        }
        else {
            setError(PATH_NOT_EXIST);
        }
    }
    else {
        setError(PATH_NOT_EXIST);
    }

    free(sec__) ;

    /* ������־��� */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "�������½��ļ���\n�ļ������ƣ�%s\n·����%s\n�½�ʧ�ܣ�ԭ��%s\n\n", __name, __path, errorDesc[errorCode]) ;
    }

    return false ;
}

bool newFile(char * __name, const char * __path)
{
    /**
     * ��ָ��·��__path�½�һ������Ϊ__name�Ŀ��ļ�
     */

    /* ������ֺϷ��� */
    if (!checkFileName(__name)) {

        /* ������־��� */
        if (LOG_OUTPUT) {
            logPrintf(FILE_API_LOG, "�������½��ļ�\n�ļ����ƣ�%s\n·����%s\n�½�ʧ�ܣ�ԭ��%s\n\n", __name, __path, errorDesc[errorCode]) ;
        }

        return false ;
    }

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    FolderIndex fo_father__ ;
    FileIndex fi_child__ ;
    int sn_father__ ;
    int sn__ ;

    if (getIndexSection(sec__, &sn_father__, __path)) {
        /* ָ����·�����ļ��У�������½� */
        if (sec__[0] != FILE_TYPE) {
            /* ��ȡĿ��·���ļ�����Ϣ */
            initFolderIndex(&fo_father__) ;
            extractFolderIndex(sec__, &fo_father__) ;

            /* ����Ƿ�ﵽ�½��ļ����� */
            if (fo_father__.childNodeNumber + 1 <= MAX_CHILD_NUMBER)
            {
                /* û��ͬ���ļ����ļ��� */
                if (!searchChildNode(&sn__, fo_father__, __name)) {
                    int allo__ ;
                    /* ����һ�������������ռ� */
                    if (applyForFreeSpace(1, &allo__)) {
                        /* �½��ļ������� */
                        initFileIndex(&fi_child__) ;
                        newFileIndex(&fi_child__, __name, sn_father__);
                        packFileIndex(sec__, fi_child__) ;
                        destroyFileIndex(&fi_child__) ;

                        /* �����ļ������������䵽������ */
                        if (saveSection(sec__, allo__)) {
                            /* ΪĿ���ļ������һ���ӽڵ� */
                            moreChildNode(&fo_father__, 1, &allo__);

                            packFolderIndex(sec__, fo_father__);
                            if (saveSection(sec__, sn_father__)) {
                                destroyFolderIndex(&fo_father__) ;
                                free(sec__);

                                if (LOG_OUTPUT) {
                                    logPrintf(FILE_API_LOG, "�������½��ļ�\n�ļ����ƣ�%s\n·����%s\n�½��ɹ�\n\n"
                                                    , __name, __path) ;
                                }

                                return true ;
                            }
                            else {
                                setError(READ_WRITE_DISK_ERROR);
                            }
                        }
                        else {
                            setError(READ_WRITE_DISK_ERROR);
                        }
                    }
                    else {
                        setError(NO_ENOUGH_DISK_SPACE);
                    }
                }
                else {
                    setError(SAME_NAME);
                }
            }
            else {
                setError(NO_ENOUGH_CHILD_NODE);
            }

            destroyFolderIndex(&fo_father__) ;
        }
        else {
            setError(PATH_NOT_EXIST);
        }
    }
    else {
        setError(PATH_NOT_EXIST);
    }

    free(sec__) ;

    /* ������־��� */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "�������½��ļ�\n�ļ����ƣ�%s\n·����%s\n�½�ʧ�ܣ�ԭ��%s\n\n", __name, __path, errorDesc[errorCode]) ;
    }

    return false ;
}

bool deleteFileFolder(const char * __path)
{
    /**
     * ɾ��ָ��·�����ļ����ļ���
     */

    /* Ȩ�޼�� */
    if (accountLevel == GUEST_USER) {
        setError(PERMISSION_IS_RESTRICTED);
        return false ;
    }

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    int secNum__ ;
    FileIndex fi_child__ ;
    FolderIndex fo_child__, fo_father__ ;

    initFileIndex(&fi_child__);
    initFolderIndex(&fo_child__);
    initFolderIndex(&fo_father__);

    if (getIndexSection(sec__, &secNum__, __path)) {
        if (sec__[0] == FILE_TYPE) {
            /* ����ļ��Ƿ��ѱ��������̴� */
            if (!fileNotOpen(__path)) {
                setError(FILE_ALREADY_OPEN);

                destroyFileIndex(&fi_child__);
                destroyFolderIndex(&fo_child__);
                destroyFolderIndex(&fo_father__);
                free(sec__);

                /* ������־��� */
                if (LOG_OUTPUT) {
                    logPrintf(FILE_API_LOG, "������ɾ���ļ����У�\n�ļ����У�·����%s\nɾ��ʧ�ܣ�ԭ��%s\n\n", __path, errorDesc[errorCode]) ;
                }

                return false ;
            }

            extractFileIndex(sec__, &fi_child__) ;

            if (recycleFileSpace(fi_child__, secNum__)) {
                /* ��ȡ���ڵ� */
                if (readSection(sec__, fi_child__.fatherNode)) {
                    extractFolderIndex(sec__, &fo_father__) ;

                    /* ɾ�����ڵ��и��ӽڵ���Ϣ */
                    if (deleteChildNode(&fo_father__, secNum__)) {
                        packFolderIndex(sec__, fo_father__) ;

                        /* ���游�ڵ� */
                        if (saveSection(sec__, fi_child__.fatherNode)) {
                            destroyFileIndex(&fi_child__) ;
                            destroyFolderIndex(&fo_father__) ;
                            free(sec__);

                            /* ������־��� */
                            if (LOG_OUTPUT) {
                                logPrintf(FILE_API_LOG, "������ɾ���ļ����У�\n�ļ����У�·����%s\nɾ���ɹ�\n\n"
                                                , __path) ;
                            }

                            return true ;
                        }
                        else {
                            setError(READ_WRITE_DISK_ERROR) ;
                        }
                    }
                    else {
                        setError(SYSTEM_ERROR) ;
                    }
                }
                else {
                    setError(READ_WRITE_DISK_ERROR) ;
                }
            }
            else {
                setError(DISK_SPACE_ERROR) ;
            }
        }
        else if (sec__[0] == FOLDER_TYPE) {
            /* ����ļ������Ƿ����ļ����������̴� */
            if (!folderNotOpen(__path)) {
                setError(FOLDER_ALREADY_OPEN);

                destroyFileIndex(&fi_child__);
                destroyFolderIndex(&fo_child__);
                destroyFolderIndex(&fo_father__);
                free(sec__);

                /* ������־��� */
                if (LOG_OUTPUT) {
                    logPrintf(FILE_API_LOG, "������ɾ���ļ����У�\n�ļ����У�·����%s\nɾ��ʧ�ܣ�ԭ��%s\n\n", __path, errorDesc[errorCode]) ;
                }

                return false ;
            }

            extractFolderIndex(sec__, &fo_child__) ;

            if (fo_child__.attribute.protect) {
                destroyFolderIndex(&fo_child__) ;
                free(sec__) ;
                setError(CAN_NOT_DELETE_PROTECT_FOLDER) ;

                /* ������־��� */
                if (LOG_OUTPUT) {
                    logPrintf(FILE_API_LOG, "������ɾ���ļ����У�\n�ļ����У�·����%s\nɾ��ʧ�ܣ�ԭ��%s\n\n", __path, errorDesc[errorCode]) ;
                }

                return false ;
            }

            if (recycleFolderSpace(fo_child__, secNum__)) {
                /* ��ȡ���ڵ� */
                if (readSection(sec__, fo_child__.fatherNode)) {
                    extractFolderIndex(sec__, &fo_father__) ;

                    /* ɾ�����ڵ��и��ӽڵ���Ϣ */
                    if (deleteChildNode(&fo_father__, secNum__)) {
                        packFolderIndex(sec__, fo_father__) ;

                        /* ���游�ڵ� */
                        if (saveSection(sec__, fo_child__.fatherNode)) {
                            destroyFolderIndex(&fo_child__) ;
                            destroyFolderIndex(&fo_father__) ;
                            free(sec__);

                            /* ������־��� */
                            if (LOG_OUTPUT) {
                                logPrintf(FILE_API_LOG, "������ɾ���ļ����У�\n�ļ����У�·����%s\nɾ���ɹ�\n\n"
                                                , __path) ;
                            }

                            return true ;
                        }
                        else {
                            setError(READ_WRITE_DISK_ERROR) ;
                        }
                    }
                    else {
                        setError(SYSTEM_ERROR) ;
                    }
                }
                else {
                    setError(READ_WRITE_DISK_ERROR) ;
                }
            }
            else {
                setError(DISK_SPACE_ERROR) ;
            }
        }
        else {
            setError(ILLEGAL_ACCESS_SYSTEM_DISK) ;
        }
    }
    else {
        setError(PATH_NOT_EXIST) ;
    }

    destroyFileIndex(&fi_child__);
    destroyFolderIndex(&fo_child__);
    destroyFolderIndex(&fo_father__);
    free(sec__);

    /* ������־��� */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "������ɾ���ļ����У�\n�ļ����У�·����%s\nɾ��ʧ�ܣ�ԭ��%s\n\n", __path, errorDesc[errorCode]) ;
    }

    return false ;
}

bool renameFileFolder(char * __new_name, const char * __path)
{
    /**
     * ��ָ��·�����ļ����ļ���������
     */

    char * newName__ = (char *)malloc(sizeof(char) * (strlen(__new_name)+1));
    strcpy(newName__, __new_name);

    /* ������ֺϷ��� */
    if (!checkFileName(newName__)) {
        free(newName__);

        /* ������־��� */
        if (LOG_OUTPUT) {
            logPrintf(FILE_API_LOG, "�������������ļ����У�\n�����ƣ�%s\nԴ�ļ����У�·����%s\n������ʧ�ܣ�ԭ��%s\n\n", newName__, __path, errorDesc[errorCode]) ;
        }

        return false ;
    }

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    int secNum__ ;

    if (getIndexSection(sec__, &secNum__, __path)) {
        /* ��·�����ܸ��� */
        if (sec__[0] != ROOT_TYPE) {

            /* ����ļ��Ƿ��ѱ��� */
            if (sec__[0] == FILE_TYPE) {
                if (!fileNotOpen(__path)) {
                    setError(FILE_ALREADY_OPEN);

                    /* ������־��� */
                    if (LOG_OUTPUT) {
                        logPrintf(FILE_API_LOG, "�������������ļ����У�\n�����ƣ�%s\nԴ�ļ����У�·����%s\n������ʧ�ܣ�ԭ��%s\n\n", newName__, __path, errorDesc[errorCode]) ;
                    }

                    free(newName__);
                    free(sec__);
                    return false ;
                }
            }
            else {
                if (!folderNotOpen(__path)) {
                    setError(FOLDER_ALREADY_OPEN);

                    /* ������־��� */
                    if (LOG_OUTPUT) {
                        logPrintf(FILE_API_LOG, "�������������ļ����У�\n�����ƣ�%s\nԴ�ļ����У�·����%s\n������ʧ�ܣ�ԭ��%s\n\n", newName__, __path, errorDesc[errorCode]) ;
                    }

                    free(newName__);
                    free(sec__);
                    return false ;
                }
            }

            FileIndex fi_child__ ;
            FolderIndex fo_child__ ;

            initFileIndex(&fi_child__);
            initFolderIndex(&fo_child__);

            if (sec__[0] == FILE_TYPE) {
                extractFileIndex(sec__, &fi_child__);

                strcpy(fi_child__.name, newName__);

                packFileIndex(sec__, fi_child__);
            }
            else if (sec__[0] == FOLDER_TYPE) {
                extractFolderIndex(sec__, &fo_child__);

                strcpy(fo_child__.name, newName__);

                packFolderIndex(sec__, fo_child__);
            }

            destroyFileIndex(&fi_child__);
            destroyFolderIndex(&fo_child__);

            if (saveSection(sec__, secNum__)) {

                /* ������־��� */
                if (LOG_OUTPUT) {
                    logPrintf(FILE_API_LOG, "�������������ļ����У�\n�����ƣ�%s\nԴ�ļ����У�·����%s\n�������ɹ�\n\n", newName__, __path) ;
                }

                free(newName__);
                free(sec__);
                return true;
            }
            else {
                setError(READ_WRITE_DISK_ERROR);
            }
        }
        else {
            setError(ILLEGAL_ACCESS_SYSTEM_DISK);
        }
    }
    else {
        setError(PATH_NOT_EXIST);
    }

    /* ������־��� */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "�������������ļ����У�\n�����ƣ�%s\nԴ�ļ����У�·����%s\n������ʧ�ܣ�ԭ��%s\n\n", newName__, __path, errorDesc[errorCode]) ;
    }

    free(newName__);
    free(sec__);

    return false ;
}

bool moveFileFolder(const char * __dest_path, const char * __source_path)
{
    /**
     * ��Դ·�����ļ����ļ����ƶ���Ŀ��·�����ļ�����
     */

    /* Ȩ�޼�� */
    if (accountLevel == GUEST_USER) {
        setError(PERMISSION_IS_RESTRICTED);
        return false ;
    }

    if (isSubFolder(__dest_path, __source_path)) {
        setError(DEST_FOLDER_IS_SUB_FOLDER) ;
        return false ;
    }

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    int source__father__, source__, dest__, sn__ ;

    /* ��ȡĿ��·�� */
    if (getIndexSection(sec__, &dest__, __dest_path)) {
        /* Ŀ��·���������ļ��� */
        if (sec__[0] != FILE_TYPE) {
            /* ���Ŀ��·���Ƿ���Դ·������Ŀ¼ */
            if (isSubFolder(__dest_path, __source_path)) {
                setError(DEST_FOLDER_IS_SUB_FOLDER) ;
                free(sec__) ;
                return false ;
            }

            FolderIndex dest_fo__ ;

            /* ��ȡĿ���ļ��������� */
            initFolderIndex(&dest_fo__) ;
            extractFolderIndex(sec__, &dest_fo__) ;

            /* ����ӽڵ����Ƿ񳬹����ֵ */
            if (dest_fo__.childNodeNumber + 1 <= MAX_CHILD_NUMBER) {
                /* ��ȡԴ·�� */
                if (getIndexSection(sec__, &source__, __source_path)) {
                    /* �����ƶ���Ŀ¼ */
                    if (sec__[0] != ROOT_TYPE) {

                        /* ����ļ��Ƿ��ѱ��� */
                        if (sec__[0] == FILE_TYPE) {
                            if (!fileNotOpen(__source_path)) {
                                setError(FILE_ALREADY_OPEN);

                                free(sec__);

                                /* ������־��� */
                                if (LOG_OUTPUT) {
                                    logPrintf(FILE_API_LOG, "�������ƶ��ļ����У�\nĿ��·����%s\nԴ·����%s\n�ƶ�ʧ�ܣ�ԭ��%s\n\n"
                                                    , __dest_path, __source_path, errorDesc[errorCode]) ;
                                }

                                return false ;
                            }
                        }
                        else {
                            if (!folderNotOpen(__source_path)) {
                                setError(FOLDER_ALREADY_OPEN);

                                free(sec__);

                                /* ������־��� */
                                if (LOG_OUTPUT) {
                                    logPrintf(FILE_API_LOG, "�������ƶ��ļ����У�\nĿ��·����%s\nԴ·����%s\n�ƶ�ʧ�ܣ�ԭ��%s\n\n"
                                                    , __dest_path, __source_path, errorDesc[errorCode]) ;
                                }

                                return false ;
                            }
                        }

                        FolderIndex source_father_fo__ ;
                        FolderIndex source_fo__ ;
                        FileIndex source_fi__ ;

                        /* ������� */
                        if (searchChildNode(&sn__, dest_fo__, &sec__[FILE_OFFSET_NAME])) {
                            free(sec__);
                            destroyFolderIndex(&dest_fo__) ;
                            setError(SAME_NAME);

                            /* ������־��� */
                            if (LOG_OUTPUT) {
                                logPrintf(FILE_API_LOG, "�������ƶ��ļ����У�\nĿ��·����%s\nԴ·����%s\n�ƶ�ʧ�ܣ�ԭ��%s\n\n"
                                                , __dest_path, __source_path, errorDesc[errorCode]) ;
                            }

                            return false ;
                        }

                        initFolderIndex(&source_father_fo__);
                        initFolderIndex(&source_fo__);
                        initFileIndex(&source_fi__);

                        if (sec__[0] == FILE_TYPE) {
                            extractFileIndex(sec__, &source_fi__);

                            source__father__ = source_fi__.fatherNode ;
                            /* �޸�Դ·���ļ��ĸ��ڵ� */
                            source_fi__.fatherNode = dest__ ;

                            /* ��� */
                            packFileIndex(sec__, source_fi__);
                            destroyFileIndex(&source_fi__);
                        }
                        else if (sec__[0] == FOLDER_TYPE) {
                            extractFolderIndex(sec__, &source_fo__);

                            if (source_fo__.attribute.protect) {
                                destroyFolderIndex(&source_fo__) ;
                                destroyFolderIndex(&source_father_fo__) ;
                                destroyFileIndex(&source_fi__) ;
                                destroyFolderIndex(&dest_fo__) ;
                                free(sec__) ;
                                setError(CAN_NOT_DELETE_PROTECT_FOLDER) ;

                                /* ������־��� */
                                if (LOG_OUTPUT) {
                                    logPrintf(FILE_API_LOG, "�������ƶ��ļ����У�\n�ļ����У�·����%s\nɾ��ʧ�ܣ�ԭ��%s\n\n", __source_path, errorDesc[errorCode]) ;
                                }

                                return false ;
                            }


                            source__father__ = source_fo__.fatherNode ;
                            /* �޸�Դ·���ļ��еĸ��ڵ� */
                            source_fo__.fatherNode = dest__ ;

                            /* ��� */
                            packFolderIndex(sec__, source_fo__);
                            destroyFolderIndex(&source_fo__);
                        }

                        /* ��Դ�ļ����ԭ���� */
                        if (!saveSection(sec__, source__)) {
                            free(sec__);
                            setError(READ_WRITE_DISK_ERROR);

                            /* ������־��� */
                            if (LOG_OUTPUT) {
                                logPrintf(FILE_API_LOG, "�������ƶ��ļ����У�\nĿ��·����%s\nԴ·����%s\n�ƶ�ʧ�ܣ�ԭ��%s\n\n"
                                                , __dest_path, __source_path, errorDesc[errorCode]) ;
                            }

                            return false ;
                        }

                        /* ��ȡԴ·���ļ��ĸ��ڵ��ļ��е������� */
                        if (!readSection(sec__, source__father__)) {
                            free(sec__);
                            setError(READ_WRITE_DISK_ERROR);

                            /* ������־��� */
                            if (LOG_OUTPUT) {
                                logPrintf(FILE_API_LOG, "�������ƶ��ļ����У�\nĿ��·����%s\nԴ·����%s\n�ƶ�ʧ�ܣ�ԭ��%s\n\n"
                                                , __dest_path, __source_path, errorDesc[errorCode]) ;
                            }

                            return false ;
                        }

                        /* ��ȡ���ڵ���������Ϣ */
                        extractFolderIndex(sec__, &source_father_fo__) ;

                        /* ��Դ���ڵ���ɾ��Դ�ļ��ڵ� */
                        deleteChildNode(&source_father_fo__, source__) ;

                        /* ��Ŀ�Ľڵ������Դ�ļ��ڵ� */
                        moreChildNode(&dest_fo__, 1, &source__) ;

                        /* ��Ŀ�ĸ��ڵ�������� */
                        packFolderIndex(sec__, dest_fo__);
                        destroyFolderIndex(&dest_fo__);
                        if (!saveSection(sec__, dest__)) {
                            free(sec__);
                            setError(READ_WRITE_DISK_ERROR);

                            /* ������־��� */
                            if (LOG_OUTPUT) {
                                logPrintf(FILE_API_LOG, "�������ƶ��ļ����У�\nĿ��·����%s\nԴ·����%s\n�ƶ�ʧ�ܣ�ԭ��%s\n\n"
                                                , __dest_path, __source_path, errorDesc[errorCode]) ;
                            }

                            return false ;
                        }

                        /* ��Դ���ڵ�������� */
                        packFolderIndex(sec__, source_father_fo__);
                        destroyFolderIndex(&source_father_fo__);
                        if (!saveSection(sec__, source__father__)) {
                            free(sec__);
                            setError(READ_WRITE_DISK_ERROR);

                            /* ������־��� */
                            if (LOG_OUTPUT) {
                                logPrintf(FILE_API_LOG, "�������ƶ��ļ����У�\nĿ��·����%s\nԴ·����%s\n�ƶ�ʧ�ܣ�ԭ��%s\n\n"
                                                , __dest_path, __source_path, errorDesc[errorCode]) ;
                            }

                            return false ;
                        }

                        /* �ƶ��ļ��ɹ� */
                        free(sec__) ;

                        /* ������־��� */
                        if (LOG_OUTPUT) {
                            logPrintf(FILE_API_LOG, "�������ƶ��ļ����У�\nĿ��·����%s\nԴ·����%s\n�ƶ��ɹ�\n\n"
                                            , __dest_path, __source_path) ;
                        }

                        return true ;
                    }
                    else {
                        setError(ILLEGAL_ACCESS_SYSTEM_DISK);
                    }
                }
                else {
                    setError(PATH_NOT_EXIST);
                }
            }
            else {
                setError(NO_ENOUGH_CHILD_NODE);
            }
        }
        else {
            setError(PATH_NOT_EXIST);
        }
    }
    else {
        setError(PATH_NOT_EXIST);
    }

    free(sec__) ;

    /* ������־��� */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "�������ƶ��ļ����У�\nĿ��·����%s\nԴ·����%s\n�ƶ�ʧ�ܣ�ԭ��%s\n\n", __dest_path, __source_path, errorDesc[errorCode]) ;
    }

    return false ;
}

bool copyFile(const char * __dest_path, const char * __source_path)
{
    /**
     * ��Դ·�����ļ����Ƶ�Ŀ��·�����ļ�����
     */

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    int source__, dest__father__, sn__ ;

    /* ��ȡĿ��·�� */
    if (getIndexSection(sec__, &dest__father__, __dest_path)) {
        /* Ŀ��·���������ļ��� */
        if (sec__[0] != FILE_TYPE) {
            FolderIndex dest_father_fo__ ;

            /* ��ȡĿ���ļ��������� */
            initFolderIndex(&dest_father_fo__) ;
            extractFolderIndex(sec__, &dest_father_fo__) ;

            /* �������ļ�������ӽڵ��������޷����� */
            if (dest_father_fo__.childNodeNumber + 1 > MAX_CHILD_NUMBER) {
                destroyFolderIndex(&dest_father_fo__);
                free(sec__);
                setError(NO_ENOUGH_CHILD_NODE);

                /* ������־��� */
                if (LOG_OUTPUT) {
                    logPrintf(FILE_API_LOG, "�����������ļ�\nĿ��·����%s\nԴ·����%s\n����ʧ�ܣ�ԭ��%s\n\n"
                                    , __dest_path, __source_path, errorDesc[errorCode]) ;
                }

                return false ;
            }

            /* ��ȡԴ·�� */
            if (getIndexSection(sec__, &source__, __source_path)) {
                /* ֻ�ܸ����ļ� */
                if (sec__[0] == FILE_TYPE) {
                    /* ������� */
                    if (searchChildNode(&sn__, dest_father_fo__, &sec__[FILE_OFFSET_NAME])) {
                        free(sec__) ;
                        destroyFolderIndex(&dest_father_fo__);
                        setError(SAME_NAME);

                        /* ������־��� */
                        if (LOG_OUTPUT) {
                            logPrintf(FILE_API_LOG, "�����������ļ�\nĿ��·����%s\nԴ·����%s\n����ʧ�ܣ�ԭ��%s\n\n"
                                            , __dest_path, __source_path, errorDesc[errorCode]) ;
                        }

                        return false ;
                    }

                    FileIndex dest_fi__ ;
                    int * allo__ ;
                    int * source_data__ ;
                    int dsn__ ;

                    initFileIndex(&dest_fi__) ;

                    extractFileIndex(sec__, &dest_fi__) ;

                    /* ����Դ����������Ϣ */
                    source_data__ = dest_fi__.dataNode ;
                    dsn__ = dest_fi__.dataSectionNumber ;
                    /* �������������Ϣ */
                    dest_fi__.dataSectionNumber = 0 ;
                    dest_fi__.dataNode = NULL ;

                    /* �޸ĸ��ڵ� */
                    dest_fi__.fatherNode = dest__father__ ;

                    /* ������̿ռ䣬����������һ������������ */
                    allo__ = (int *)malloc(sizeof(int) * (dsn__ + 1)) ;
                    if (applyForFreeSpace(dsn__ + 1, allo__)) {
                        /* �������ݿ����� */
                        for (int i=1; i<=dsn__; i++) {
                            if (!copySection(allo__[i], source_data__[i-1])) {
                                free(sec__);
                                free(allo__);
                                free(source_data__) ;
                                destroyFolderIndex(&dest_father_fo__);
                                destroyFileIndex(&dest_fi__) ;
                                return false ;
                            }
                        }

                        /* �������������ݿ�ڵ㲢���������� */
                        if (moreDataNode(&dest_fi__, dsn__, &allo__[1])) {

                            packFileIndex(sec__, dest_fi__) ;

                            if (saveSection(sec__, allo__[0])) {
                                /* Ϊ���ڵ�����һ���ӽڵ㣬������ */
                                if (moreChildNode(&dest_father_fo__, 1, allo__)) {

                                    packFolderIndex(sec__, dest_father_fo__) ;

                                    /* ������� */
                                    if (saveSection(sec__, dest__father__)) {
                                        free(sec__);
                                        free(allo__);
                                        free(source_data__) ;
                                        destroyFolderIndex(&dest_father_fo__);
                                        destroyFileIndex(&dest_fi__) ;

                                        /* ������־��� */
                                        if (LOG_OUTPUT) {
                                            logPrintf(FILE_API_LOG, "�����������ļ�\nĿ��·����%s\nԴ·����%s\n���Ƴɹ�\n\n"
                                                            , __dest_path, __source_path) ;
                                        }

                                        return true ;
                                    }
                                    else {
                                        setError(READ_WRITE_DISK_ERROR) ;
                                    }
                                }
                                else {
                                    setError(NO_ENOUGH_CHILD_NODE);
                                }
                            }
                            else {
                                setError(READ_WRITE_DISK_ERROR) ;
                            }
                        }
                        else {
                            setError(FILE_TOO_BIG);
                        }
                    }
                    else {
                        setError(NO_ENOUGH_DISK_SPACE);
                    }

                    free(allo__) ;
                    free(source_data__) ;
                    destroyFileIndex(&dest_fi__) ;
                    destroyFolderIndex(&dest_father_fo__) ;
                }
                else {
                    setError(SYSTEM_ERROR);
                }
            }
            else {
                setError(PATH_NOT_EXIST);
            }
        }
        else {
            setError(PATH_NOT_EXIST);
        }
    }
    else {
        setError(PATH_NOT_EXIST);
    }

    free(sec__) ;

    /* ������־��� */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "�����������ļ�\nĿ��·����%s\nԴ·����%s\n����ʧ�ܣ�ԭ��%s\n\n", __dest_path, __source_path, errorDesc[errorCode]) ;
    }

    return false ;
}

bool copyFolder(char * __dest_path, char * __source_path)
{
    /**
     * ��Դ·�����ļ��м���Ŀ¼������
     * �ļ����У����Ƶ�Ŀ��·�����ļ�����
     */

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    int source__, dest__father__, sn__ ;

    /* ��ȡĿ��·�� */
    if (getIndexSection(sec__, &dest__father__, __dest_path)) {
        /* Ŀ��·���������ļ��� */
        if (sec__[0] != FILE_TYPE) {
            /* ���Ŀ��·���Ƿ���Դ·������Ŀ¼ */
            if (isSubFolder(__dest_path, __source_path)) {
                setError(DEST_FOLDER_IS_SUB_FOLDER) ;
                free(sec__) ;
                return false ;
            }

            FolderIndex dest_father_fo__ ;

            /* ��ȡĿ��·����������Ϣ */
            initFolderIndex(&dest_father_fo__);
            extractFolderIndex(sec__, &dest_father_fo__);

            /* �������ļ�������ӽڵ��������޷����� */
            if (dest_father_fo__.childNodeNumber + 1 > MAX_CHILD_NUMBER) {
                destroyFolderIndex(&dest_father_fo__);
                free(sec__);
                setError(NO_ENOUGH_CHILD_NODE);

                /* ������־��� */
                if (LOG_OUTPUT) {
                    logPrintf(FILE_API_LOG, "�����������ļ���\nĿ��·����%s\nԴ·����%s\n����ʧ�ܣ�ԭ��%s\n\n"
                                    , __dest_path, __source_path, errorDesc[errorCode]) ;
                }

                return false ;
            }

            /* ��ȡԴ·�� */
            if (getIndexSection(sec__, &source__, __source_path)) {
                /* ֻ�ܸ����ļ��� */
                if (sec__[0] == FOLDER_TYPE) {
                    /* ������� */
                    if (searchChildNode(&sn__, dest_father_fo__, &sec__[FILE_OFFSET_NAME])) {
                        free(sec__) ;
                        destroyFolderIndex(&dest_father_fo__);
                        setError(SAME_NAME);

                        /* ������־��� */
                        if (LOG_OUTPUT) {
                            logPrintf(FILE_API_LOG, "�����������ļ���\nĿ��·����%s\nԴ·����%s\n����ʧ�ܣ�ԭ��%s\n\n"
                                            , __dest_path, __source_path, errorDesc[errorCode]) ;
                        }

                        return false ;
                    }

                    int allo__ ;

                    /* ����һ�������������ڵ� */
                    if (applyForFreeSpace(1, &allo__)) {
                        FolderIndex dest_child_fo__ ;
                        int dsn__ ;
                        int * source_data__ ;

                        /* ��Դ�ļ��е���������Ϣ��ȡ��Ŀ���ļ��������� */
                        initFolderIndex(&dest_child_fo__);
                        extractFolderIndex(sec__, &dest_child_fo__);

                        /* �޸ĸ��ڵ� */
                        dest_child_fo__.fatherNode = dest__father__ ;

                        /* ��¼Դ�ļ��е��ӽڵ���Ϣ */
                        dsn__ = dest_child_fo__.childNodeNumber ;
                        source_data__ = dest_child_fo__.childNode ;

                        /* ���Դ�ļ����ӽڵ� */
                        dest_child_fo__.childNodeNumber = 0 ;
                        dest_child_fo__.childNode = NULL ;

                        /* �ڸ��ڵ������Ӹ��ӽڵ� */
                        moreChildNode(&dest_father_fo__, 1, &allo__);

                        packFolderIndex(sec__, dest_father_fo__);
                        if (saveSection(sec__, dest__father__)) {
                            packFolderIndex(sec__, dest_child_fo__);

                            if (saveSection(sec__, allo__)) {
                                /* �õݹ�ķ�������ԭ�ļ����µ���Ŀ¼ */
                                char dest_new__[255] = "" ;
                                char source_new__[255] = "" ;
                                int len_dest__ = strlen(__dest_path);
                                int len_source__ = strlen(__source_path);

                                strcat(dest_new__, __dest_path) ;
                                if (dest_new__[len_dest__-1] != '\\') {
                                    strcat(dest_new__, "\\");
                                    len_dest__++ ;
                                }

                                strcat(source_new__, __source_path) ;
                                if (source_new__[len_dest__-1] != '\\') {
                                    strcat(source_new__, "\\");
                                    len_source__++ ;
                                }

                                for (int i=0; i<=dsn__-1; i++) {
                                    if (readSection(sec__, source_data__[i])) {
                                        strcat(dest_new__, dest_child_fo__.name);
                                        strcat(source_new__, &sec__[FILE_OFFSET_NAME]);
                                        //puts("Here I am!");
                                        //puts(dest_new__);
                                        //puts(source_new__);
                                        if (sec__[0] == FILE_TYPE) {
                                            copyFile(dest_new__, source_new__);
                                        }
                                        else if (sec__[0] == FOLDER_TYPE) {
                                            copyFolder(dest_new__, source_new__);
                                        }

                                        dest_new__[len_dest__] = '\0' ;
                                        source_new__[len_source__] = '\0' ;
                                    }
                                }

                                free(sec__);
                                free(source_data__);
                                destroyFolderIndex(&dest_father_fo__);
                                destroyFolderIndex(&dest_child_fo__);

                                /* ������־��� */
                                if (LOG_OUTPUT) {
                                    logPrintf(FILE_API_LOG, "�����������ļ���\nĿ��·����%s\nԴ·����%s\n���Ƴɹ�\n\n"
                                                    , __dest_path, __source_path) ;
                                }

                                return true ;
                            }
                            else {
                                setError(READ_WRITE_DISK_ERROR) ;
                            }
                        }
                        else {
                            setError(READ_WRITE_DISK_ERROR) ;
                        }

                        free(source_data__);
                        destroyFolderIndex(&dest_child_fo__);
                    }
                    else {
                        setError(NO_ENOUGH_DISK_SPACE) ;
                    }
                }
                else {
                    setError(SYSTEM_ERROR);
                }
            }
            else {
                setError(PATH_NOT_EXIST);
            }

            destroyFolderIndex(&dest_father_fo__);
        }
        else {
            setError(PATH_NOT_EXIST);
        }
    }
    else {
        setError(PATH_NOT_EXIST);
    }

    free(sec__);

    /* ������־��� */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "�����������ļ���\nĿ��·����%s\nԴ·����%s\n����ʧ�ܣ�ԭ��%s\n\n", __dest_path, __source_path, errorDesc[errorCode]) ;
    }

    return false ;
}

bool setFileAttribute(FileAttribute __attr, const char * __path)
{
    /* Ȩ�޼�� */
    if (accountLevel == GUEST_USER) {
        setError(PERMISSION_IS_RESTRICTED);
        return false ;
    }

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    int secNum__ ;

    /* ��ȡĿ��·�� */
    if (getIndexSection(sec__, &secNum__, __path)) {
        if (sec__[FILE_OFFSET_FLAG] == FILE_TYPE) {
            FileIndex fi__ ;

            initFileIndex(&fi__);
            extractFileIndex(sec__, &fi__);

            fi__.attribute = __attr ;

            packFileIndex(sec__, fi__);
            destroyFileIndex(&fi__);

            if (saveSection(sec__, secNum__)) {
                free(sec__);

                /* ������־��� */
                if (LOG_OUTPUT) {
                    char * buf[2] = {"��", "��"} ;
                    logPrintf(FILE_API_LOG, "�����������ļ�����\n�ļ�·����%s\n���أ�%s\tֻ����%s\n\n", __path, buf[__attr.hide], buf[__attr.readOnly]) ;
                }

                return true ;
            }
            else {
                setError(READ_WRITE_DISK_ERROR);
            }
        }
        else {
            setError(PATH_NOT_EXIST);
        }
    }
    else {
        setError(PATH_NOT_EXIST);
    }

    free(sec__);

    /* ������־��� */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "�����������ļ�����\n�ļ�·����%s\n����ʧ�ܣ�ԭ��%s\n\n", __path, errorDesc[errorCode]) ;
    }

    return false ;
}

bool setFolderAttribute(FolderAttribute __attr, const char * __path)
{
    /* Ȩ�޼�� */
    if (accountLevel == GUEST_USER) {
        setError(PERMISSION_IS_RESTRICTED);
        return false ;
    }

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    int secNum__ ;

    /* ��ȡĿ��·�� */
    if (getIndexSection(sec__, &secNum__, __path)) {
        if (sec__[FOLDER_OFFSET_FLAG] == FOLDER_TYPE) {
            FolderIndex fi__ ;

            initFolderIndex(&fi__);
            extractFolderIndex(sec__, &fi__);

            fi__.attribute = __attr ;

            packFolderIndex(sec__, fi__);
            destroyFolderIndex(&fi__);

            if (saveSection(sec__, secNum__)) {
                free(sec__);

                /* ������־��� */
                if (LOG_OUTPUT) {
                    char * buf[2] = {"��", "��"} ;
                    logPrintf(FILE_API_LOG, "�����������ļ�������\n�ļ���·����%s\n���أ�%s\t������%s\n\n", __path, buf[__attr.hide], buf[__attr.protect]) ;
                }

                return true ;
            }
            else {
                setError(READ_WRITE_DISK_ERROR);
            }
        }
        else {
            setError(PATH_NOT_EXIST);
        }
    }
    else {
        setError(PATH_NOT_EXIST);
    }

    free(sec__);

    /* ������־��� */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "�����������ļ�������\n�ļ���·����%s\n����ʧ�ܣ�ԭ��%s\n\n", __path, errorDesc[errorCode]) ;
    }

    return false ;
}

FileStream * openFile(const char * __path)
{
    /**
     * ��ָ��·�����ļ������ظ��ļ���Ӧ���ļ���
     */

    FileStream * fs__ ;
    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    int secNum__ ;

    if (getIndexSection(sec__, &secNum__, __path)) {
        if (sec__[0] == FILE_TYPE) {
            FileIndex fi__ ;

            initFileIndex(&fi__) ;
            extractFileIndex(sec__, &fi__) ;

            fs__ = (FileStream *)malloc(sizeof(FileStream)) ;

            fs__->path = (char *)malloc(sizeof(char) * (strlen(__path)+1));
            strcpy(fs__->path, __path);
            fs__->indexSectionNumber = secNum__ ;
            fs__->readPtr = 0 ;
            fs__->writePtr = 0 ;

            destroyFileIndex(&fi__) ;

            /* ��ӵ����ļ��б� */
            addOpenFile(__path);

            free(sec__) ;

            /* ������־��� */
            if (LOG_OUTPUT) {
                logPrintf(FILE_STREAM_LOG, "���������ļ�\n�ļ�·����%s\n�򿪳ɹ����ļ�����Ϣ��\n�����������ţ�%d\n��ȡָ��:%d\nд��ָ��:%d\n\n"
                                , __path, fs__->indexSectionNumber, fs__->readPtr, fs__->writePtr) ;
            }

            return fs__ ;
        }
        else {
            setError(PATH_NOT_EXIST) ;
        }
    }
    else {
        setError(PATH_NOT_EXIST) ;
    }

    free(sec__) ;

    /* ������־��� */
    if (LOG_OUTPUT) {
        logPrintf(FILE_STREAM_LOG, "���������ļ�\n�ļ�·����%s\n��ʧ�ܣ�ԭ��%s\n\n", __path, errorDesc[errorCode]) ;
    }

    return NULL ;
}

void closeFile(FileStream * __fs)
{
    /**
     * �ر��ļ����ͷ��ļ����������ٴδ������ļ�
     */

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;

    if (__fs != NULL) {
        /* ���ļ����б���ɾ�� */
        closeOpenFile(__fs->path);

        /* ������־��� */
        if (LOG_OUTPUT) {
            logPrintf(FILE_STREAM_LOG, "�������ر��ļ���\n�ļ�·����%s\n�ļ������������ţ�%d\n�رճɹ�\n\n", __fs->path, __fs->indexSectionNumber) ;
        }

        free(__fs->path);
        free(__fs) ;

        __fs = NULL ;
    }
}

int getFileLength(FileStream * __fs)
{
    /**
     * �����ļ�������Ӧ���ļ��ĳ���
     */

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;

    if (readSection(sec__, __fs->indexSectionNumber)) {
        if (sec__[0] == FILE_TYPE) {
            FileIndex fi__ ;
            int len__ ;

            initFileIndex(&fi__) ;
            extractFileIndex(sec__, &fi__) ;

            len__ = fi__.length ;

            destroyFileIndex(&fi__) ;

            free(sec__) ;

            /* ������־��� */
            if (LOG_OUTPUT) {
                logPrintf(FILE_STREAM_LOG, "��������ȡ�ļ�����\n�ļ�·����%s\n�ļ������������ţ�%d\n��ȡ�ɹ����ļ����ȣ�%d\n\n" , __fs->path, __fs->indexSectionNumber, len__) ;
            }

            return len__ ;
        }
        else {
            setError(FILE_READ_ERROR) ;
        }
    }
    else {
        setError(READ_WRITE_DISK_ERROR) ;
    }

    free(sec__) ;

    /* ������־��� */
    if (LOG_OUTPUT) {
        logPrintf(FILE_STREAM_LOG, "��������ȡ�ļ�����\n�ļ�·����%s\n�ļ������������ţ�%d\n��ȡʧ�ܣ�ԭ��%s\n\n" , __fs->path, __fs->indexSectionNumber, errorDesc[errorCode]) ;
    }

    return 0 ;
}

int readWholeFile(char * __buf, FileStream * __fs)
{
    /**
     * ���ļ����е����ݶ���buf��
     * ��ȡ�ļ���ȫ������
     */

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;

    if (readSection(sec__, __fs->indexSectionNumber)) {
        if (sec__[0] == FILE_TYPE) {
            FileIndex fi__ ;
            /* ʣ���δ��ȡ���ַ������Ѿ���ȡ���ַ�����׼����ȡ���������к� */
            int left__, have__ = 0, whsec = 0 ;

            initFileIndex(&fi__) ;
            extractFileIndex(sec__, &fi__) ;

            left__ = fi__.length ;

            /* ��ȡ���� */
            while (left__ > 0 && whsec <= fi__.dataSectionNumber-1) {
                /* ֱ�Ӷ�ȡ�������� */
                if (left__ >= sectionSize) {
                    if (readSection(&__buf[have__], fi__.dataNode[whsec])) {
                        have__ += sectionSize ;
                        left__ -= sectionSize ;
                    }
                    else {
                        setError(READ_WRITE_DISK_ERROR) ;
                        free(sec__) ;

                        /* ������־��� */
                        if (LOG_OUTPUT) {
                            logPrintf(FILE_STREAM_LOG, "��������ȡȫ���ļ�����\n�ļ�·����%s\n�ļ������������ţ�%d\n��ȡʧ�ܣ�ԭ��%s\n��ȡ�ֽ�����%d\n\n"
                                            , __fs->path, __fs->indexSectionNumber, errorDesc[errorCode], have__) ;
                        }

                        return have__ ;
                    }
                }
                /* �ȶ�ȡ�����������ٽ�����ǰleft__��������buf�� */
                else {
                    if (readSection(sec__, fi__.dataNode[whsec])) {
                        copyString(&__buf[have__], sec__, left__) ;
                        have__ += left__ ;
                        left__ -= left__ ;
                    }
                    else {
                        setError(READ_WRITE_DISK_ERROR) ;
                        free(sec__) ;

                        /* ������־��� */
                        if (LOG_OUTPUT) {
                            logPrintf(FILE_STREAM_LOG, "��������ȡȫ���ļ�����\n�ļ�·����%s\n�ļ������������ţ�%d\n��ȡʧ�ܣ�ԭ��%s\n��ȡ�ֽ�����%d\n\n"
                                            , __fs->path, __fs->indexSectionNumber, errorDesc[errorCode], have__) ;
                        }

                        return have__ ;
                    }
                }

                /* ��һ������ */
                whsec ++ ;
            }

            /* �޸Ķ�ȡָ�� */
            __fs->readPtr = have__ ;

            free(sec__) ;

            /* ������־��� */
            if (LOG_OUTPUT) {
                char * buf = (char *)malloc(sizeof(char) * (have__+1));

                copyString(buf, __buf, have__) ;
                buf[have__] = '\0' ;

                logPrintf(FILE_STREAM_LOG, "��������ȡȫ���ļ�����\n�ļ�·����%s\n�ļ������������ţ�%d\n��ȡ�ɹ����ļ����ݣ�\n%s\n�ļ����ȣ�%d\n\n" , __fs->path, __fs->indexSectionNumber, buf, have__) ;

                free(buf);
            }

            return have__ ;
        }
        else {
            setError(FILE_READ_ERROR) ;
        }
    }
    else {
        setError(READ_WRITE_DISK_ERROR) ;
    }

    free(sec__) ;

    /* ������־��� */
    if (LOG_OUTPUT) {
        logPrintf(FILE_STREAM_LOG, "��������ȡȫ���ļ�����\n�ļ�·����%s\n�ļ������������ţ�%d\n��ȡʧ�ܣ�ԭ��%s\n��ȡ�ֽ�����%d\n\n" , __fs->path, __fs->indexSectionNumber, errorDesc[errorCode], 0) ;
    }

    return 0 ;
}

bool saveWholeFile(char * __buf, int __count, FileStream * __fs)
{
    /**
     * ��buf�����count���ַ�д�뵽�ļ���fs��
     * �Ḳ����ǰ������
     */

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;

    if (readSection(sec__, __fs->indexSectionNumber)) {
        if (sec__[0] == FILE_TYPE) {
            FileIndex fi__ ;
            int now_need__, past_need__ ;

            initFileIndex(&fi__) ;
            extractFileIndex(sec__, &fi__) ;

            /* �����޸�ֻ���ļ����� */
            if (fi__.attribute.readOnly) {
                destroyFileIndex(&fi__) ;
                free(sec__) ;
                setError(CAN_NOT_MODIFY_READ_ONLY_FILE);

                /* ������־��� */
                if (LOG_OUTPUT) {
                    logPrintf(FILE_STREAM_LOG, "�����������ļ�����\n�ļ�·����%s\n�ļ������������ţ�%d\n����ʧ�ܣ�ԭ��%s\n\n" , __fs->path, __fs->indexSectionNumber, errorDesc[errorCode]) ;
                }

                return false ;
            }

            /* ��¼��ʹ�õ����������� */
            past_need__ = fi__.dataSectionNumber ;
            /* ���㱣���µ�������Ҫ������������ */
            if (__count == 0) {
                now_need__ = 0 ;
            }
            else {
                now_need__ = (__count / sectionSize) + 1 ;
            }

            /* ��Ҫ������������� */
            if (now_need__ > past_need__) {
                int more__ = now_need__ - past_need__ ;
                int * allo__ = (int *)malloc(sizeof(int) * more__) ;

                if (applyForFreeSpace(more__, allo__)) {
                    if (moreDataNode(&fi__, more__, allo__)) {
                        ;
                    }
                    else {
                        setError(FILE_TOO_BIG);
                        /* Ӧ������������ͷ� */
                        releaseFreeSpace(more__, allo__);
                        free(allo__);
                        free(sec__);

                        /* ������־��� */
                        if (LOG_OUTPUT) {
                            logPrintf(FILE_STREAM_LOG, "�����������ļ�����\n�ļ�·����%s\n�ļ������������ţ�%d\n����ʧ�ܣ�ԭ��%s\n\n" , __fs->path, __fs->indexSectionNumber, errorDesc[errorCode]) ;
                        }

                        return false ;
                    }
                }
                else {
                    setError(NO_ENOUGH_DISK_SPACE);
                    free(allo__);
                    free(sec__);

                    /* ������־��� */
                    if (LOG_OUTPUT) {
                        logPrintf(FILE_STREAM_LOG, "�����������ļ�����\n�ļ�·����%s\n�ļ������������ţ�%d\n����ʧ�ܣ�ԭ��%s\n\n" , __fs->path, __fs->indexSectionNumber, errorDesc[errorCode]) ;
                    }

                    return false ;
                }
            }
            /* ���������������٣�Ӧ�ͷŵ���������� */
            else if (now_need__ < past_need__) {
                int less__ = past_need__ - now_need__ ;

                if (releaseFreeSpace(less__, &(fi__.dataNode[now_need__]))) {
                    if (releaseDataNode(&fi__, less__)) {
                        ;
                    }
                    else {
                        setError(SYSTEM_ERROR);
                        free(sec__);
                        return false ;
                    }
                }
                else {
                    setError(SYSTEM_ERROR);
                    free(sec__);
                    return false ;
                }
            }


            int left__ = __count ;
            int have__ = 0 ;
            int whsec__ = 0 ;

            /* ����ļ����� */
            fi__.length = 0 ;

            /* �������� */
            while (left__ > 0 && whsec__ <= fi__.dataSectionNumber - 1) {
                if (left__ >= sectionSize) {
                    if (saveSection(&__buf[have__], fi__.dataNode[whsec__])) {
                        left__ -= sectionSize ;
                        have__ += sectionSize ;
                        fi__.length += sectionSize ;
                    }
                    else {
                        setError(READ_WRITE_DISK_ERROR);
                        free(sec__);
                        return false ;
                    }
                }
                else {
                    copyString(sec__, &__buf[have__], left__) ;

                    if (saveSection(sec__, fi__.dataNode[whsec__])) {
                        have__ += left__ ;
                        fi__.length += left__ ;
                        left__ -= left__ ;
                    }
                    else {
                        setError(READ_WRITE_DISK_ERROR);
                        free(sec__);
                        return false ;
                    }
                }

                whsec__ ++ ;
            }

            /* �����ϴ��޸�ʱ�� */
            TimeStamp ts__ ;
            getSystemTime(&ts__) ;
            fi__.lastModifyTime = ts__ ;

            packFileIndex(sec__, fi__) ;
            destroyFileIndex(&fi__) ;

            /* ������������Ϣ */
            if (saveSection(sec__, __fs->indexSectionNumber)) {
                __fs->writePtr = have__ ;

                free(sec__);

                /* ������־��� */
                if (LOG_OUTPUT) {
                    char * buf = (char *)malloc(sizeof(char) * (__count+1));

                    copyString(buf, __buf, __count) ;
                    buf[__count] = '\0' ;

                    logPrintf(FILE_STREAM_LOG, "�����������ļ�����\n�ļ�·����%s\n�ļ������������ţ�%d\n����ɹ����ļ����ݣ�\n%s\n�ļ����ȣ�%d\n\n" , __fs->path, __fs->indexSectionNumber, buf, __count) ;

                    free(buf);
                }

                return true ;
            }
            else {
                setError(READ_WRITE_DISK_ERROR) ;
            }
        }
        else {
            setError(FILE_WRITE_ERROR) ;
        }
    }
    else {
        setError(READ_WRITE_DISK_ERROR) ;
    }

    free(sec__) ;

    /* ������־��� */
    if (LOG_OUTPUT) {
        logPrintf(FILE_STREAM_LOG, "�����������ļ�����\n�ļ�·����%s\n�ļ������������ţ�%d\n����ʧ�ܣ�ԭ��%s\n\n" , __fs->path, __fs->indexSectionNumber, errorDesc[errorCode]) ;
    }

    return false ;
}
