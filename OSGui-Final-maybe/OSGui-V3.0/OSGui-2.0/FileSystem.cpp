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
 * 文件系统用到的全局变量
 */
const char * bitMapFileName = "bitMap.txt" ;/* 位图信息配置文件名 */
const char * FSConfigFileName = "FSConfig.txt" ;	/* 文件系统配置信息文件 */
char * bitVector = NULL ;					/* 位图，使用权和修改权仅限于空闲空间管理模块 */

int rootDirectory = 0 ;						/* 磁盘根目录所在的扇区号 */
char rootDirectoryLabel[LENGTH_NAME] ;		/* 根目录对应的卷标 */
int VMStartSection = 0 ;					/* 虚存空间的开始扇区 */
int VMEndSection = 0 ;						/* 虚存空间的结束扇区 */
int VMTotalSection = 0 ;					/* 虚存空间的总扇区数 */

const char fileNameIllegalChar[10] = {9, '/', '\\', ':', '*', '?', '\"', '<', '>', '|'} ;

/**
 * 其他模块的变量
 */
extern const char * configPath ;			/* 来自基本模块，用于读取文件系统所需的配置信息 */
extern int sectionNumber ;						/* 来自磁盘驱动模块，在空闲空间管理模块用到 */
extern int sectionSize ;
extern const char * errorDesc[] ;
extern AccountLevelType accountLevel ;
extern bool LOG_OUTPUT;

/****************************************
 * 时间戳操作函数
 ****************************************/
/***************
 ** Debug通过 **
 ***************/

int compareTimeStamp(TimeStamp __time_1, TimeStamp __time_2)
{
    /**
     * 比较两个时间戳，前者较新则返回-1
     * 两者相同返回0，后者较新返回1
     */

    /* 将年月日转换成10进制数 */
    int date1 = __time_1.year * 10000 + __time_1.month * 100 + __time_1.day ;
    int date2 = __time_2.year * 10000 + __time_2.month * 100 + __time_2.day ;

    /* 直接比较 */
    if (date2 > date1) {
        return 1 ;
    }
    else if (date1 > date2) {
        return -1 ;
    }

    /* 将时分秒转换成10进制数 */
    int time1 = __time_1.hour * 10000 + __time_1.min * 100 + __time_1.sec ;
    int time2 = __time_2.hour * 10000 + __time_2.min * 100 + __time_2.sec ;

    if (time2 > time1) {
        return 1 ;
    }
    else if (time1 > time2) {
        return -1 ;
    }

    /* 相等 */
    return 0 ;
}

void extractTimeStamp(char * __buf, TimeStamp * __ts)
{
    /**
     * 将保存于扇区的时间戳（从__buf开始的7个字节）
     * 转换成TimeStamp格式的时间，低字节中保存高位
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
     * 将时间戳保存到扇区中（从__buf开始的7个字节）
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
 * 文件索引块操作函数
 ****************************************/
/***************
 ** Debug通过 **
 ***************/

void initFileIndex(FileIndex * __fi)
{
    /**
     * 初始化FileIndex结构，防止不可控的错误
     */

    __fi->name = NULL ;
    __fi->dataNode = NULL ;
}

void destroyFileIndex(FileIndex * __fi)
{
    /**
     * 释放FileIndex中申请的空间
     * 与initFileIndex()配对使用
     */

    free(__fi->name);
    free(__fi->dataNode);
}

void extractFileIndex(char * __sec, FileIndex * __fi)
{
    /**
     * 提取文件索引块中各字段信息，保存到FileIndex结构中
     */

    /* 提取标记 */
    __fi->flag = __sec[FILE_OFFSET_FLAG] ;

    /* 提取文件名 */
    /* 防止重复申请空间 */
    if (__fi->name == NULL) {
        __fi->name = (char *)malloc(sizeof(char) * LENGTH_NAME) ;
    }
    /* 将扇区中name字段复制到FileIndex的name中 */
    copyString(__fi->name, &__sec[FILE_OFFSET_NAME], LENGTH_NAME) ;

    /* 提取时间戳 */
    extractTimeStamp(&__sec[FILE_OFFSET_CREATE_TIME], &(__fi->createTime));
    extractTimeStamp(&__sec[FILE_OFFSET_LAST_MODIFY_TIME], &(__fi->lastModifyTime));
    extractTimeStamp(&__sec[FILE_OFFSET_LAST_OPEN_TIME], &(__fi->lastOpenTime));

    /* 超过1B的数进行转换，低字节中放高位 */
    /* 256^3=16777216; 256^2=65536 */
    __fi->length = (unsigned char)__sec[FILE_OFFSET_LENGTH] * 16777216
                + (unsigned char)__sec[FILE_OFFSET_LENGTH + 1] * 65536
                + (unsigned char)__sec[FILE_OFFSET_LENGTH + 2] * 256
                + (unsigned char)__sec[FILE_OFFSET_LENGTH + 3] ;

    __fi->dataSectionNumber = (unsigned char)__sec[FILE_OFFSET_DATASEC_NUMBER] * 256
                            + (unsigned char)__sec[FILE_OFFSET_DATASEC_NUMBER + 1] ;

    __fi->fatherNode = (unsigned char)__sec[FILE_OFFSET_FATHER_NODE] * 256
                    + (unsigned char)__sec[FILE_OFFSET_FATHER_NODE + 1] ;

    /* 数据块节点 */
    int dsn__ = __fi->dataSectionNumber ;

    if (dsn__ > 0) {
        __fi->dataNode = (int *)malloc(sizeof(int) * dsn__) ;

        for (int i = 0; i <= dsn__ - 1; i++) {
            (__fi->dataNode)[i] = (unsigned char)__sec[FILE_OFFSET_DATA_NODE + i*2] * 256
                                + (unsigned char)__sec[FILE_OFFSET_DATA_NODE + i*2 + 1] ;
        }
    }

    /* 文件属性 */
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
     * 将FileIndex结构保存到索引块__sec中
     * 当修改了索引块中的字段时，应该调用此函数
     */

    for (int i=0; i<=sectionSize-1; i++) {
        __sec[i] = -1 ;
    }

    /* 保存标记 */
    __sec[FILE_OFFSET_FLAG] = __fi.flag ;
    /* 保存文件名 */
    copyString(&__sec[FILE_OFFSET_NAME], __fi.name, LENGTH_NAME) ;

    /* 保存时间戳 */
    packTimeStamp(&__sec[FILE_OFFSET_CREATE_TIME], __fi.createTime);
    packTimeStamp(&__sec[FILE_OFFSET_LAST_MODIFY_TIME], __fi.lastModifyTime);
    packTimeStamp(&__sec[FILE_OFFSET_LAST_OPEN_TIME], __fi.lastOpenTime);

    /* 保存数值字段 */
    __sec[FILE_OFFSET_LENGTH] = __fi.length / 16777216 ;
    __sec[FILE_OFFSET_LENGTH + 1] = (__fi.length % 16777216) / 65536 ;
    __sec[FILE_OFFSET_LENGTH + 2] = (__fi.length % 65536) / 256 ;
    __sec[FILE_OFFSET_LENGTH + 3] = __fi.length % 256 ;

    __sec[FILE_OFFSET_DATASEC_NUMBER] = __fi.dataSectionNumber / 256 ;
    __sec[FILE_OFFSET_DATASEC_NUMBER + 1] = __fi.dataSectionNumber % 256 ;

    __sec[FILE_OFFSET_FATHER_NODE] = __fi.fatherNode / 256 ;
    __sec[FILE_OFFSET_FATHER_NODE + 1] = __fi.fatherNode % 256 ;

    /* 保存数据节点 */
    int dsn__ = __fi.dataSectionNumber ;

    if (dsn__ > 0) {
        for (int i = 0; i <= dsn__ - 1; i++) {
            __sec[FILE_OFFSET_DATA_NODE + i*2] = __fi.dataNode[i] / 256 ;
            __sec[FILE_OFFSET_DATA_NODE + i*2 + 1] = __fi.dataNode[i] % 256 ;
        }
    }

    /* 保存文件属性 */
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
     * 新建文件索引块，需给出名字和父节点
     * 其他值均初始化为0
     */

    /* 初始化为文件类型 */
    __fi->flag = FILE_TYPE ;

    /* 初始化文件名 */
    /* 防止重复申请空间 */
    if (__fi->name == NULL) {
        __fi->name = (char *)malloc(sizeof(char) * LENGTH_NAME) ;
    }
    copyString(__fi->name, __name, LENGTH_NAME) ;

    /* 初始化时间戳 */
    TimeStamp ts__ ;

    getSystemTime(&ts__) ;
    __fi->createTime = ts__ ;
    __fi->lastModifyTime = ts__ ;
    __fi->lastOpenTime = ts__ ;

    /* 其他值初始化为0 */
    __fi->length = 0 ;
    __fi->dataSectionNumber = 0 ;

    /* 初始化父节点 */
    __fi->fatherNode = __father ;

    /* 初始化属性 */
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
     * 向FileIndex中添加__dsn个数据块
     */

    int tot__ = __fi->dataSectionNumber + __dsn ;

    /* 检查是否超过扇区数量限制 */
    if (tot__ > MAX_DATASEC_NUMBER || __dsn < 0) {
        return false ;
    }

    if (__dsn == 0) {
        return true ;
    }

    /* 将新分配的数据扇区__secNum添加至__fi->dataNode的尾部 */
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
     * 从FileIndex中释放__dsn个数据扇区
     * 从dataNode尾部抹掉__dsn个扇区
     */

    int tot__ = __fi->dataSectionNumber - __dsn ;

    /* 数据合法性检查 */
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

    /* 从尾部抹掉__dsn个数据块 */
    int * newSec__ = (int *)malloc(sizeof(int) * tot__) ;

    copyIntArray(newSec__, __fi->dataNode, tot__) ;
    __fi->dataSectionNumber = tot__ ;

    free(__fi->dataNode) ;
    __fi->dataNode = newSec__ ;

    return true ;
}

/****************************************
 * 文件夹索引块操作函数
 ****************************************/
/***************
 ** Debug通过 **
 ***************/

void initFolderIndex(FolderIndex * __foi)
{
    /**
     * 初始化FolderIndex结构，防止不可控的错误
     */

    __foi->name = NULL ;
    __foi->childNode = NULL ;
}

void destroyFolderIndex(FolderIndex * __foi)
{
    /**
     * 释放FolderIndex中申请的空间
     * 与initFolderIndex()配对使用
     */

    free(__foi->name);
    free(__foi->childNode);
}

void extractFolderIndex(char * __sec, FolderIndex * __foi)
{
    /**
     * 提取文件索引块中各字段信息，保存到FolderIndex结构中
     */

    /* 提取标记 */
    __foi->flag = __sec[FOLDER_OFFSET_FLAG] ;

    /* 提取文件名 */
    /* 防止重复申请空间 */
    if (__foi->name == NULL) {
        __foi->name = (char *)malloc(sizeof(char) * LENGTH_NAME) ;
    }
    /* 将扇区中name字段复制到FolderIndex的name中 */
    copyString(__foi->name, &__sec[FOLDER_OFFSET_NAME], LENGTH_NAME) ;

    /* 提取时间戳 */
    extractTimeStamp(&__sec[FOLDER_OFFSET_CREATE_TIME], &(__foi->createTime));

    /* 超过1B的数进行转换，低字节中放高位 */
    __foi->childNodeNumber = (unsigned char)__sec[FOLDER_OFFSET_CHILD_NUMBER] * 256
                + (unsigned char)__sec[FOLDER_OFFSET_CHILD_NUMBER + 1] ;

    __foi->fatherNode = (unsigned char)__sec[FOLDER_OFFSET_FATHER_NODE] * 256
            + (unsigned char)__sec[FOLDER_OFFSET_FATHER_NODE + 1] ;

    /* 子节点 */
    int dsn__ = __foi->childNodeNumber ;

    if (dsn__ > 0) {
        __foi->childNode = (int *)malloc(sizeof(int) * dsn__) ;

        for (int i = 0; i <= dsn__ - 1; i++) {
            (__foi->childNode)[i] = (unsigned char)__sec[FOLDER_OFFSET_CHILD_NODE + i*2] * 256
                        + (unsigned char)__sec[FOLDER_OFFSET_CHILD_NODE + i*2 + 1] ;
        }
    }

    /* 提取属性 */
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
     * 将FolderIndex结构保存到索引块__sec中
     * 当修改了索引块中的字段时，应该调用此函数
     */

    for (int i=0; i<=sectionSize-1; i++) {
        __sec[i] = -1 ;
    }

    /* 保存标记 */
    __sec[FOLDER_OFFSET_FLAG] = __foi.flag ;
    /* 保存文件名 */
    copyString(&__sec[FOLDER_OFFSET_NAME], __foi.name, LENGTH_NAME) ;

    /* 保存时间戳 */
    packTimeStamp(&__sec[FOLDER_OFFSET_CREATE_TIME], __foi.createTime);

    /* 保存数值字段 */
    __sec[FOLDER_OFFSET_CHILD_NUMBER] = __foi.childNodeNumber / 256 ;
    __sec[FOLDER_OFFSET_CHILD_NUMBER + 1] = __foi.childNodeNumber % 256 ;

    __sec[FOLDER_OFFSET_FATHER_NODE] = __foi.fatherNode / 256 ;
    __sec[FOLDER_OFFSET_FATHER_NODE + 1] = __foi.fatherNode % 256 ;

    /* 保存数据节点 */
    int dsn__ = __foi.childNodeNumber ;

    if (dsn__ > 0) {
        for (int i = 0; i <= dsn__ - 1; i++) {
            __sec[FOLDER_OFFSET_CHILD_NODE + i*2] = __foi.childNode[i] / 256 ;
            __sec[FOLDER_OFFSET_CHILD_NODE + i*2 + 1] = __foi.childNode[i] % 256 ;
        }
    }

    /* 保存属性 */
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
     * 新建文件夹索引块，需给出名字和父节点
     * 其他值均初始化为0
     */

    /* 初始化为文件夹类型 */
    __foi->flag = FOLDER_TYPE ;

    /* 初始化文件名 */
    /* 防止重复申请空间 */
    if (__foi->name == NULL) {
        __foi->name = (char *)malloc(sizeof(char) * LENGTH_NAME) ;
    }
    copyString(__foi->name, __name, LENGTH_NAME) ;

    /* 初始化时间戳 */
    TimeStamp ts__ ;

    getSystemTime(&ts__) ;
    __foi->createTime = ts__ ;

    /* 子节点数量初始化为0 */
    __foi->childNodeNumber = 0 ;

    /* 初始化父节点 */
    __foi->fatherNode = __father ;

    /* 初始化属性 */
    __foi->attribute.hide = false ;
    __foi->attribute.protect = false ;
}

bool moreChildNode(FolderIndex * __foi, int __dsn, int * __secNum)
{
    /**
     * 向FolderIndex中添加__dsn个数据块
     */

    int tot__ = __foi->childNodeNumber + __dsn ;

    /* 检查是否超过扇区数量限制 */
    if (tot__ > MAX_CHILD_NUMBER || __dsn < 0) {
        return false ;
    }

    if (__dsn == 0) {
        return true ;
    }

    /* 将新分配的数据扇区__secNum添加至__foi->childNode的尾部 */
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
     * 从FolderIndex中释放__dsn个数据扇区
     * 从childNode尾部抹掉__dsn个扇区
     */

    int tot__ = __foi->childNodeNumber - __dsn ;

    /* 数据合法性检查 */
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

    /* 从尾部抹掉__dsn个数据块 */
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
     * 从FolderIndex中删除一个扇区号为__secNum的子节点
     */

    /* 数据不合法 */
    if (__secNum < 0) {
        setError(SYSTEM_ERROR) ;
        return false ;
    }

    int i, j ;
    for (i = 0; i <= __foi->childNodeNumber - 1; i++) {
        /* 找到该节点 */
        if (__secNum == __foi->childNode[i]) {
            /* 子节点数减一 */
            __foi->childNodeNumber-- ;

            if (__foi->childNodeNumber == 0) {
                free(__foi->childNode);
                __foi->childNode = NULL ;
                return true ;
            }
            else {
                int * child__ = (int *)malloc(sizeof(int) * __foi->childNodeNumber) ;

                /* 建立新的子节点数组 */
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
 * 文件系统环境函数
 ****************************************/
/***************
 ** Debug通过 **
 ***************/

bool newBitVector(int __sectionNumber)
{
    /**
     * 磁盘格式化后，生成新的扇区位图
     * 位图用0,1表示，0表示扇区可用，1表示不可用
     */

    /* 位图文件路径 */
    char * bvPath__ = (char *)malloc(sizeof(char) * 38) ;
    sprintf(bvPath__, "%s%s", configPath, bitMapFileName) ;

    FILE * ofp = fopen(bvPath__, "wb");
    /* 位图文件路径有误 */
    if (ofp == NULL) {
        free(bvPath__) ;
        return false ;
    }

    /* 写入新的位图，初始化为全可用 */
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
     * 操作系统启动时从位图文件中获取位图信息
     * 位向量长度由配置信息sectionNumber指定
     * 调用此函数前必须保证配置信息已经读取成功
     * __bitMap指向的内存区域必须足够长
     */

    /* 位图文件路径 */
    char * bvPath__ = (char *)malloc(sizeof(char) * 38) ;
    sprintf(bvPath__, "%s%s", configPath, bitMapFileName) ;

    FILE * ifp = fopen(bvPath__, "rb");
    /* 位图文件路径有误 */
    if (ifp == NULL) {
        free(bvPath__) ;
        return false ;
    }

    /* 读取位图 */
    fread(__bitMap, sizeof(char), sectionNumber, ifp);
    /* 结尾补\0，用于调试 */
    //__bitMap[sectionNumber] = '\0';

    fclose(ifp) ;
    free(bvPath__) ;

    return true ;
}

bool saveBitVector(char * __bitMap)
{
    /**
     * 将__bitMap位图信息保存到位图文件中
     * 每当位图被修改后都必须调用该函数，保证一致性
     */

    /* 位图文件路径 */
    char * bvPath__ = (char *)malloc(sizeof(char) * 38) ;
    sprintf(bvPath__, "%s%s", configPath, bitMapFileName) ;

    FILE * ofp = fopen(bvPath__, "wb");
    /* 位图文件路径有误 */
    if (ofp == NULL) {
        free(bvPath__) ;
        return false ;
    }

    /* 保存位图 */
    fwrite(__bitMap, sizeof(char), sectionNumber, ofp);

    fclose(ofp) ;
    free(bvPath__) ;

    return true ;
}

bool readFSConfig()
{
    /**
     * 读取磁盘的配置信息，此操作应该在
     * 操作系统启动时由BIOS程序调用
     */

    /* 配置文件路径 */
    char * configPath__ = (char *)malloc(sizeof(char) * 38) ;
    sprintf(configPath__, "%s%s", configPath, FSConfigFileName);

    /* 打开配置文件，失败则返回false */
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

    /* 读取成功 */
    return true ;
}

bool isUserSpace(int __sec)
{
    /**
     * 检查是否是用户空间，防止越界
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
     * 初始化文件系统模块的环境
     * 读取并初始化文件系统的配置信息
     * 在系统启动时调用
     */

    /* 读取硬件配置信息 */
    if (!readDiskConfig()) {
        return false ;
    }

    /* 读取位图信息 */
    bitVector = (char *)malloc(sizeof(char) * sectionNumber) ;
    if (!getBitVector(bitVector)) {
        return false ;
    }

    /* 读取文件系统配置信息 */
    if (!readFSConfig()) {
        return false ;
    }

    return true ;
}

/****************************************
 * 文件系统空闲空间管理函数
 ****************************************/
/***************
 ** Debug通过 **
 ***************/

int queryFreeSpace()
{
    /**
     * 查询位图，返回可用的磁盘扇区数量
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

    /* 工作日志输出 */
    if (LOG_OUTPUT) {
        logPrintf(FREE_SPACE_LOG, "操作：空闲空间查询\n剩余可用的磁盘扇区数：%d\n\n", remain__) ;
    }

    return remain__ ;
}

bool applyForFreeSpace(int __many, int * __allocated)
{
    /**
     * 申请文件空间，申请成功则将申请的扇区号放入__allocated
     * 失败则返回false
     */

    int i, j = 0 ;

    /* 分配数超过可用数，返回false */
    if (__many < 0 || __many > queryFreeSpace()) {

        /* 工作日志输出 */
        if (LOG_OUTPUT) {
            logPrintf(FREE_SPACE_LOG, "操作：空闲空间申请\n申请的扇区数：%d\t可用扇区不足，申请失败\n\n", __many) ;
        }

        return false ;
    }

    /* 需找可分配的扇区并记录在__allocated中 */
    for (i=0; i <= sectionNumber - 1 && j < __many; i++) {
        /* 避开根目录个虚存区域 */
        if ((i!=rootDirectory) && (i<VMStartSection || i>VMEndSection)) {
            if (bitVector[i] == SECTION_AVAILABLE) {
                bitVector[i] = SECTION_TAKEN ;
                __allocated[j] = i ;
                j++ ;
            }
        }
    }

    /* 若没有分配足够的空间，则释放已分配的空间并返回false */
    if (j != __many) {
        for (i=0; i <= j-1; i++) {
            bitVector[__allocated[i]] = SECTION_AVAILABLE ;
        }

        return false ;
    }
    else {
        /* 申请成功，保存位图信息 */
        saveBitVector(bitVector);

        /* 工作日志输出 */
        if (LOG_OUTPUT) {
            char buf[255] = "" ;

            for (i = 0; i <= __many-1; i++) {
                sprintf(buf, "%s %d", buf, __allocated[i]);
            }

            logPrintf(FREE_SPACE_LOG, "操作：空闲空间申请\n申请的扇区数：%d\t申请成功\n申请的扇区号：%s\n\n", __many, buf) ;
        }

        return true ;
    }
}

bool releaseFreeSpace(int __many, const int * __allocated)
{
    /**
     * 释放__allocated中记录的磁盘扇区
     * 失败则返回false
     */

    int sec__ ;

    /* 数据合法性检测 */
    if (__many < 0) {
        return false ;
    }

    for (int i=0; i <= __many - 1; i++) {
        sec__ = __allocated[i] ;

        /* 企图释放虚存或根目录等系统保留区域 */
        if ((sec__>=VMStartSection && sec__<=VMEndSection) || sec__==rootDirectory) {
            return false ;
        }
        else {
            bitVector[sec__] = SECTION_AVAILABLE ;
        }
    }

    /* 保存位图信息 */
    saveBitVector(bitVector);

    /* 工作日志输出 */
    if (LOG_OUTPUT) {
        char buf[255] = "" ;

        for (int i = 0; i <= __many-1; i++) {
            sprintf(buf, "%s %d", buf, __allocated[i]);
        }

        logPrintf(FREE_SPACE_LOG, "操作：空闲空间释放\n释放的扇区数：%d\t释放成功\n释放的扇区号：%s\n\n", __many, buf) ;
    }

    return true ;
}

bool recycleFileSpace(FileIndex __fi, int __secNum)
{
    /**
     * 将指定文件的所有使用的空间回收
     * 需传入该文件索引块信息和索引块的扇区位置
     */

    /* 数据块数量不合法 */
    if (__fi.dataSectionNumber < 0) {
        setError(SYSTEM_ERROR) ;
        return false ;
    }

    /* 该文件有数据块 */
    if (__fi.dataSectionNumber > 0) {
        if (!releaseFreeSpace(__fi.dataSectionNumber, __fi.dataNode)) {
            return false ;
        }
    }

    /* 释放索引块扇区 */
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
     * 将指定文件夹的所有使用的空间回收（包括其下所有的文件和文件夹）
     * 需传入该文件索引块信息和索引块的扇区位置
     */

    /* 数据块数量不合法 */
    if (__fo.childNodeNumber < 0) {
        setError(SYSTEM_ERROR) ;
        return false ;
    }

    /* 该文件夹有子节点 */
    if (__fo.childNodeNumber > 0) {
        int i ;
        char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
        FileIndex fi_child__ ;
        FolderIndex fo_child__ ;

        for (i = 0; i<=__fo.childNodeNumber-1; i++) {
            if (readSection(sec__, __fo.childNode[i])) {
                /* 子节点是文件 */
                if (sec__[0] == FILE_TYPE) {
                    initFileIndex(&fi_child__) ;
                    extractFileIndex(sec__, &fi_child__) ;

                    /* 回收文件空间 */
                    if (!recycleFileSpace(fi_child__, __fo.childNode[i])) {
                        free(sec__) ;
                        destroyFileIndex(&fi_child__) ;
                        return false ;
                    }

                    destroyFileIndex(&fi_child__) ;
                }
                /* 子节点是文件夹 */
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
                /* 子节点是根目录，非法访问 */
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

    /* 释放索引块扇区 */
    if (releaseFreeSpace(1, &__secNum)) {
        return true ;
    }
    else {
        return false ;
    }
}

/****************************************
 * 其他底层接口
 ****************************************/

bool copySection(int __aim, int __from)
{
    /**
     * 拷贝扇区中的内容，从__from复制到__aim
     */

    /* 检查是否是用户空间 */
    if (!(isUserSpace(__aim) && isUserSpace(__from))) {
        return false ;
    }

    if (__aim == __from) {
        return true ;
    }

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;

    /* 读取原扇区，保存到目的扇区，完成复制 */
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
     * 文件名合法性检测，命名规则：
     * 1.不能超过37个英文字符（包括后缀名）
     * 2.不能含有以下任何字符： / \ : * ? " < > |
     * 3.文件名开头不能是空格
     */

    int len__ ;
    int i, j ;
    char * name__ = (char *)malloc(sizeof(char) * LENGTH_NAME);
    char * ex__ = (char *)malloc(sizeof(char) * LENGTH_NAME);

    len__ = strlen(__fn) ;
    /* 消除名称前面的空格 */
    i = 0;
    while (__fn[i]==' ') {
        i++ ;
    }
    for (j=i; j<=len__; j++) {
        __fn[j-i] = __fn[j] ;
    }

    len__ = strlen(__fn) ;
    /* 消除名称后面的空格 */
    i = len__-1;
    while (__fn[i]==' ' || __fn[i]=='.') {
        i-- ;
    }
    __fn[i+1] = '\0' ;

    len__ = strlen(__fn) ;
    /* 长度超限 */
    if (len__ + 1 > LENGTH_NAME) {
        setError(FILE_NAME_TOO_LONG);
        free(name__);
        free(ex__);
        return false ;
    }

    extractFileName(name__, ex__, __fn);
    /* 文件名不能为空 */
    if (strlen(name__) == 0) {
        setError(FILE_NAME_EMPTY);
        free(name__);
        free(ex__);
        return false ;
    }

    /* 逐个检查字符 */
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
     * 从完整文件名中提取出名字和后缀
     * 分别存到__name和__ex中
     */
    int len_w = strlen(__whole) ;
    int i = len_w -1 ;

    while (__whole[i]!='.' && i>=0) {
        i-- ;
    }

    /* 若没有小数点，则假设小数点在字符串最后 */
    if (i<0) {
        i = len_w ;
    }

    /* 复制名字 */
    copyString(__name, __whole, i);
    __name[i] = '\0';

    if (i>=len_w -1) {
        /* 没有后缀，即为空 */
        __ex[0] = '\0';
    }
    else {
        /* 复制后缀名 */
        copyString(__ex, &__whole[i+1], len_w - i -1) ;
        __ex[len_w - i -1] = '\0';
    }
}

bool extractFilePath(const char * __whole_path, char * __path, int __which)
{
    /**
     * 从完整文件路径__whole_path中提取第__which层路径
     * 存入__path中，根目录是第0层，提取失败则返回false
     */

    if (__which < 0 || __whole_path == NULL) {
        return false ;
    }

    int len_wp = strlen(__whole_path) ;
    int i ;

    /* 提取根目录卷标 */
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

        /* 提取第__which层路径 */
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
     * 文件系统首次初始化时需创建根目录
     */

    FolderIndex root__ ;
    char name__[38] = "本地磁盘" ;
    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;

    for (int i=0; i<=sectionSize-1; i++) {
        sec__[i] = -1 ;
    }

    /* 新建根目录索引块 */
    initFolderIndex(&root__);

    newFolderIndex(&root__, name__, NULL_POINTER) ;
    root__.flag = ROOT_TYPE ;

    packFolderIndex(sec__, root__) ;

    /* 保存到根目录扇区 */
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
     * 只从扇区中提取出名字字段，用于搜索
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
     * 搜索文件夹索引块中名字为__name的子节点
     * 若有，则将子节点山区号写入__secNum
     */

    /* 文件夹下没有子节点 */
    if (__fo.childNodeNumber == 0) {
        return false ;
    }

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    char * name__ = (char *)malloc(sizeof(char) * LENGTH_NAME) ;

    for (int i = 0; i <= __fo.childNodeNumber -1; i++) {
        /* 提取子节点的名字 */
        if (!extractName(name__, __fo.childNode[i])) {
            free(sec__) ;
            free(name__) ;
            return false ;
        }
        /* 名字若相等，则将扇区号写入__secNum */
        else if (strcmp(name__, __name) == 0) {
            (*__secNum) = __fo.childNode[i] ;

            free(sec__) ;
            free(name__) ;
            return true ;
        }
    }

    free(sec__) ;
    free(name__) ;
    /* 未找到名字相同的子节点 */
    return false ;
}

bool getIndexSection(char * __sec, int * __secNum, const char * __whole_path)
{
    /**
     * 根据完整路径在文件系统中搜索该文件或文件夹
     * 的索引块并写入__sec，索引块的扇区号存入__secNum
     * 是文件还是文件夹，路径无效则返回false
     */

    char path__[LENGTH_NAME] ;
    FolderIndex fo__ ;
    int ns__ ;
    int level__ ;

    /* 定位根目录 */
    if (!extractFilePath(__whole_path, path__, 0)) {
        return false ;
    }
    /* 检查根目录卷标 */
    if (strcmp(rootDirectoryLabel, path__) == 0) {
        /* 读取根目录扇区 */
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
        /* 提取索引块信息 */
        initFolderIndex(&fo__);
        extractFolderIndex(__sec, &fo__) ;

        /* 若找到下一级目录 */
        if (searchChildNode(&ns__, fo__, path__)) {
            /* 读取下一级索引块 */
            if (!readSection(__sec, ns__)) {
                return false ;
            }

            /* 记录扇区位置 */
            (*__secNum) = ns__ ;

            /* 准备读取下一级 */
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
     * 检查文件是否被某个进程打开
     */

    return checkOpenFile(__path) ;
}

bool folderNotOpen(const char * __path)
{
    /**
     * 检查文件下的某个文件是否被某个进程打开
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

                /* 子节点是文件 */
                if (sec__[FILE_OFFSET_FLAG] == FILE_TYPE) {
                    sprintf(newPath__, "%s\\%s", __path, &sec__[FILE_OFFSET_NAME]);

                    if (!fileNotOpen(newPath__)) {
                        destroyFolderIndex(&fo__);
                        free(sec__) ;
                        return false ;
                    }
                }
                /* 子节点是文件夹 */
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
 * 文件系统API
 ****************************************/

bool newFolder(char * __name, const char * __path)
{
    /**
     * 在指定路径__path新建一个名字为__name的空文件夹
     */

    /* 检查名字合法性 */
    if (!checkFileName(__name)) {

        /* 工作日志输出 */
        if (LOG_OUTPUT) {
            logPrintf(FILE_API_LOG, "操作：新建文件夹\n文件夹名称：%s\n路径：%s\n新建失败，原因：%s\n\n", __name, __path, errorDesc[errorCode]) ;
        }

        return false ;
    }

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    FolderIndex fo_father__, fo_child__ ;
    int sn_father__ ;
    int sn__ ;

    if (getIndexSection(sec__, &sn_father__, __path)) {
        /* 指定的路径是文件夹，则可以新建 */
        if (sec__[0] != FILE_TYPE) {
            /* 提取目标路径文件夹信息 */
            initFolderIndex(&fo_father__) ;
            extractFolderIndex(sec__, &fo_father__) ;

            /* 检查是否达到新建文件上限 */
            if (fo_father__.childNodeNumber + 1 <= MAX_CHILD_NUMBER)
            {
                /* 没有同名文件或文件夹 */
                if (!searchChildNode(&sn__, fo_father__, __name)) {
                    int allo__ ;
                    /* 申请一个索引块扇区空间 */
                    if (applyForFreeSpace(1, &allo__)) {
                        /* 新建文件夹索引块 */
                        initFolderIndex(&fo_child__) ;
                        newFolderIndex(&fo_child__, __name, sn_father__);
                        packFolderIndex(sec__, fo_child__) ;
                        destroyFolderIndex(&fo_child__) ;

                        /* 将新文件夹索引块存入分配到的扇区 */
                        if (saveSection(sec__, allo__)) {
                            /* 为目标文件夹添加一个子节点 */
                            moreChildNode(&fo_father__, 1, &allo__);

                            packFolderIndex(sec__, fo_father__);
                            if (saveSection(sec__, sn_father__)) {
                                destroyFolderIndex(&fo_father__) ;
                                free(sec__);

                                if (LOG_OUTPUT) {
                                    logPrintf(FILE_API_LOG, "操作：新建文件夹\n文件夹名称：%s\n路径：%s\n新建成功\n\n"
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

    /* 工作日志输出 */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "操作：新建文件夹\n文件夹名称：%s\n路径：%s\n新建失败，原因：%s\n\n", __name, __path, errorDesc[errorCode]) ;
    }

    return false ;
}

bool newFile(char * __name, const char * __path)
{
    /**
     * 在指定路径__path新建一个名字为__name的空文件
     */

    /* 检查名字合法性 */
    if (!checkFileName(__name)) {

        /* 工作日志输出 */
        if (LOG_OUTPUT) {
            logPrintf(FILE_API_LOG, "操作：新建文件\n文件名称：%s\n路径：%s\n新建失败，原因：%s\n\n", __name, __path, errorDesc[errorCode]) ;
        }

        return false ;
    }

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    FolderIndex fo_father__ ;
    FileIndex fi_child__ ;
    int sn_father__ ;
    int sn__ ;

    if (getIndexSection(sec__, &sn_father__, __path)) {
        /* 指定的路径是文件夹，则可以新建 */
        if (sec__[0] != FILE_TYPE) {
            /* 提取目标路径文件夹信息 */
            initFolderIndex(&fo_father__) ;
            extractFolderIndex(sec__, &fo_father__) ;

            /* 检查是否达到新建文件上限 */
            if (fo_father__.childNodeNumber + 1 <= MAX_CHILD_NUMBER)
            {
                /* 没有同名文件或文件夹 */
                if (!searchChildNode(&sn__, fo_father__, __name)) {
                    int allo__ ;
                    /* 申请一个索引块扇区空间 */
                    if (applyForFreeSpace(1, &allo__)) {
                        /* 新建文件索引块 */
                        initFileIndex(&fi_child__) ;
                        newFileIndex(&fi_child__, __name, sn_father__);
                        packFileIndex(sec__, fi_child__) ;
                        destroyFileIndex(&fi_child__) ;

                        /* 将新文件索引块存入分配到的扇区 */
                        if (saveSection(sec__, allo__)) {
                            /* 为目标文件夹添加一个子节点 */
                            moreChildNode(&fo_father__, 1, &allo__);

                            packFolderIndex(sec__, fo_father__);
                            if (saveSection(sec__, sn_father__)) {
                                destroyFolderIndex(&fo_father__) ;
                                free(sec__);

                                if (LOG_OUTPUT) {
                                    logPrintf(FILE_API_LOG, "操作：新建文件\n文件名称：%s\n路径：%s\n新建成功\n\n"
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

    /* 工作日志输出 */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "操作：新建文件\n文件名称：%s\n路径：%s\n新建失败，原因：%s\n\n", __name, __path, errorDesc[errorCode]) ;
    }

    return false ;
}

bool deleteFileFolder(const char * __path)
{
    /**
     * 删除指定路径的文件或文件夹
     */

    /* 权限检测 */
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
            /* 检查文件是否已被其他进程打开 */
            if (!fileNotOpen(__path)) {
                setError(FILE_ALREADY_OPEN);

                destroyFileIndex(&fi_child__);
                destroyFolderIndex(&fo_child__);
                destroyFolderIndex(&fo_father__);
                free(sec__);

                /* 工作日志输出 */
                if (LOG_OUTPUT) {
                    logPrintf(FILE_API_LOG, "操作：删除文件（夹）\n文件（夹）路径：%s\n删除失败，原因：%s\n\n", __path, errorDesc[errorCode]) ;
                }

                return false ;
            }

            extractFileIndex(sec__, &fi_child__) ;

            if (recycleFileSpace(fi_child__, secNum__)) {
                /* 读取父节点 */
                if (readSection(sec__, fi_child__.fatherNode)) {
                    extractFolderIndex(sec__, &fo_father__) ;

                    /* 删除父节点中该子节点信息 */
                    if (deleteChildNode(&fo_father__, secNum__)) {
                        packFolderIndex(sec__, fo_father__) ;

                        /* 保存父节点 */
                        if (saveSection(sec__, fi_child__.fatherNode)) {
                            destroyFileIndex(&fi_child__) ;
                            destroyFolderIndex(&fo_father__) ;
                            free(sec__);

                            /* 工作日志输出 */
                            if (LOG_OUTPUT) {
                                logPrintf(FILE_API_LOG, "操作：删除文件（夹）\n文件（夹）路径：%s\n删除成功\n\n"
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
            /* 检查文件夹下是否有文件被其他进程打开 */
            if (!folderNotOpen(__path)) {
                setError(FOLDER_ALREADY_OPEN);

                destroyFileIndex(&fi_child__);
                destroyFolderIndex(&fo_child__);
                destroyFolderIndex(&fo_father__);
                free(sec__);

                /* 工作日志输出 */
                if (LOG_OUTPUT) {
                    logPrintf(FILE_API_LOG, "操作：删除文件（夹）\n文件（夹）路径：%s\n删除失败，原因：%s\n\n", __path, errorDesc[errorCode]) ;
                }

                return false ;
            }

            extractFolderIndex(sec__, &fo_child__) ;

            if (fo_child__.attribute.protect) {
                destroyFolderIndex(&fo_child__) ;
                free(sec__) ;
                setError(CAN_NOT_DELETE_PROTECT_FOLDER) ;

                /* 工作日志输出 */
                if (LOG_OUTPUT) {
                    logPrintf(FILE_API_LOG, "操作：删除文件（夹）\n文件（夹）路径：%s\n删除失败，原因：%s\n\n", __path, errorDesc[errorCode]) ;
                }

                return false ;
            }

            if (recycleFolderSpace(fo_child__, secNum__)) {
                /* 读取父节点 */
                if (readSection(sec__, fo_child__.fatherNode)) {
                    extractFolderIndex(sec__, &fo_father__) ;

                    /* 删除父节点中该子节点信息 */
                    if (deleteChildNode(&fo_father__, secNum__)) {
                        packFolderIndex(sec__, fo_father__) ;

                        /* 保存父节点 */
                        if (saveSection(sec__, fo_child__.fatherNode)) {
                            destroyFolderIndex(&fo_child__) ;
                            destroyFolderIndex(&fo_father__) ;
                            free(sec__);

                            /* 工作日志输出 */
                            if (LOG_OUTPUT) {
                                logPrintf(FILE_API_LOG, "操作：删除文件（夹）\n文件（夹）路径：%s\n删除成功\n\n"
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

    /* 工作日志输出 */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "操作：删除文件（夹）\n文件（夹）路径：%s\n删除失败，原因：%s\n\n", __path, errorDesc[errorCode]) ;
    }

    return false ;
}

bool renameFileFolder(char * __new_name, const char * __path)
{
    /**
     * 将指定路径的文件或文件夹重命名
     */

    char * newName__ = (char *)malloc(sizeof(char) * (strlen(__new_name)+1));
    strcpy(newName__, __new_name);

    /* 检查名字合法性 */
    if (!checkFileName(newName__)) {
        free(newName__);

        /* 工作日志输出 */
        if (LOG_OUTPUT) {
            logPrintf(FILE_API_LOG, "操作：重命名文件（夹）\n新名称：%s\n源文件（夹）路径：%s\n重命名失败，原因：%s\n\n", newName__, __path, errorDesc[errorCode]) ;
        }

        return false ;
    }

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    int secNum__ ;

    if (getIndexSection(sec__, &secNum__, __path)) {
        /* 根路径不能改名 */
        if (sec__[0] != ROOT_TYPE) {

            /* 检查文件是否已被打开 */
            if (sec__[0] == FILE_TYPE) {
                if (!fileNotOpen(__path)) {
                    setError(FILE_ALREADY_OPEN);

                    /* 工作日志输出 */
                    if (LOG_OUTPUT) {
                        logPrintf(FILE_API_LOG, "操作：重命名文件（夹）\n新名称：%s\n源文件（夹）路径：%s\n重命名失败，原因：%s\n\n", newName__, __path, errorDesc[errorCode]) ;
                    }

                    free(newName__);
                    free(sec__);
                    return false ;
                }
            }
            else {
                if (!folderNotOpen(__path)) {
                    setError(FOLDER_ALREADY_OPEN);

                    /* 工作日志输出 */
                    if (LOG_OUTPUT) {
                        logPrintf(FILE_API_LOG, "操作：重命名文件（夹）\n新名称：%s\n源文件（夹）路径：%s\n重命名失败，原因：%s\n\n", newName__, __path, errorDesc[errorCode]) ;
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

                /* 工作日志输出 */
                if (LOG_OUTPUT) {
                    logPrintf(FILE_API_LOG, "操作：重命名文件（夹）\n新名称：%s\n源文件（夹）路径：%s\n重命名成功\n\n", newName__, __path) ;
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

    /* 工作日志输出 */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "操作：重命名文件（夹）\n新名称：%s\n源文件（夹）路径：%s\n重命名失败，原因：%s\n\n", newName__, __path, errorDesc[errorCode]) ;
    }

    free(newName__);
    free(sec__);

    return false ;
}

bool moveFileFolder(const char * __dest_path, const char * __source_path)
{
    /**
     * 将源路径的文件或文件夹移动到目的路径的文件夹下
     */

    /* 权限检测 */
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

    /* 读取目的路径 */
    if (getIndexSection(sec__, &dest__, __dest_path)) {
        /* 目的路径必须是文件夹 */
        if (sec__[0] != FILE_TYPE) {
            /* 检查目的路径是否是源路径的子目录 */
            if (isSubFolder(__dest_path, __source_path)) {
                setError(DEST_FOLDER_IS_SUB_FOLDER) ;
                free(sec__) ;
                return false ;
            }

            FolderIndex dest_fo__ ;

            /* 提取目的文件夹索引块 */
            initFolderIndex(&dest_fo__) ;
            extractFolderIndex(sec__, &dest_fo__) ;

            /* 检查子节点数是否超过最大值 */
            if (dest_fo__.childNodeNumber + 1 <= MAX_CHILD_NUMBER) {
                /* 读取源路径 */
                if (getIndexSection(sec__, &source__, __source_path)) {
                    /* 不能移动根目录 */
                    if (sec__[0] != ROOT_TYPE) {

                        /* 检查文件是否已被打开 */
                        if (sec__[0] == FILE_TYPE) {
                            if (!fileNotOpen(__source_path)) {
                                setError(FILE_ALREADY_OPEN);

                                free(sec__);

                                /* 工作日志输出 */
                                if (LOG_OUTPUT) {
                                    logPrintf(FILE_API_LOG, "操作：移动文件（夹）\n目的路径：%s\n源路径：%s\n移动失败，原因：%s\n\n"
                                                    , __dest_path, __source_path, errorDesc[errorCode]) ;
                                }

                                return false ;
                            }
                        }
                        else {
                            if (!folderNotOpen(__source_path)) {
                                setError(FOLDER_ALREADY_OPEN);

                                free(sec__);

                                /* 工作日志输出 */
                                if (LOG_OUTPUT) {
                                    logPrintf(FILE_API_LOG, "操作：移动文件（夹）\n目的路径：%s\n源路径：%s\n移动失败，原因：%s\n\n"
                                                    , __dest_path, __source_path, errorDesc[errorCode]) ;
                                }

                                return false ;
                            }
                        }

                        FolderIndex source_father_fo__ ;
                        FolderIndex source_fo__ ;
                        FileIndex source_fi__ ;

                        /* 检查重名 */
                        if (searchChildNode(&sn__, dest_fo__, &sec__[FILE_OFFSET_NAME])) {
                            free(sec__);
                            destroyFolderIndex(&dest_fo__) ;
                            setError(SAME_NAME);

                            /* 工作日志输出 */
                            if (LOG_OUTPUT) {
                                logPrintf(FILE_API_LOG, "操作：移动文件（夹）\n目的路径：%s\n源路径：%s\n移动失败，原因：%s\n\n"
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
                            /* 修改源路径文件的父节点 */
                            source_fi__.fatherNode = dest__ ;

                            /* 打包 */
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

                                /* 工作日志输出 */
                                if (LOG_OUTPUT) {
                                    logPrintf(FILE_API_LOG, "操作：移动文件（夹）\n文件（夹）路径：%s\n删除失败，原因：%s\n\n", __source_path, errorDesc[errorCode]) ;
                                }

                                return false ;
                            }


                            source__father__ = source_fo__.fatherNode ;
                            /* 修改源路径文件夹的父节点 */
                            source_fo__.fatherNode = dest__ ;

                            /* 打包 */
                            packFolderIndex(sec__, source_fo__);
                            destroyFolderIndex(&source_fo__);
                        }

                        /* 将源文件存回原扇区 */
                        if (!saveSection(sec__, source__)) {
                            free(sec__);
                            setError(READ_WRITE_DISK_ERROR);

                            /* 工作日志输出 */
                            if (LOG_OUTPUT) {
                                logPrintf(FILE_API_LOG, "操作：移动文件（夹）\n目的路径：%s\n源路径：%s\n移动失败，原因：%s\n\n"
                                                , __dest_path, __source_path, errorDesc[errorCode]) ;
                            }

                            return false ;
                        }

                        /* 读取源路径文件的父节点文件夹的索引块 */
                        if (!readSection(sec__, source__father__)) {
                            free(sec__);
                            setError(READ_WRITE_DISK_ERROR);

                            /* 工作日志输出 */
                            if (LOG_OUTPUT) {
                                logPrintf(FILE_API_LOG, "操作：移动文件（夹）\n目的路径：%s\n源路径：%s\n移动失败，原因：%s\n\n"
                                                , __dest_path, __source_path, errorDesc[errorCode]) ;
                            }

                            return false ;
                        }

                        /* 提取父节点索引块信息 */
                        extractFolderIndex(sec__, &source_father_fo__) ;

                        /* 在源父节点中删除源文件节点 */
                        deleteChildNode(&source_father_fo__, source__) ;

                        /* 在目的节点中添加源文件节点 */
                        moreChildNode(&dest_fo__, 1, &source__) ;

                        /* 将目的父节点打包并存回 */
                        packFolderIndex(sec__, dest_fo__);
                        destroyFolderIndex(&dest_fo__);
                        if (!saveSection(sec__, dest__)) {
                            free(sec__);
                            setError(READ_WRITE_DISK_ERROR);

                            /* 工作日志输出 */
                            if (LOG_OUTPUT) {
                                logPrintf(FILE_API_LOG, "操作：移动文件（夹）\n目的路径：%s\n源路径：%s\n移动失败，原因：%s\n\n"
                                                , __dest_path, __source_path, errorDesc[errorCode]) ;
                            }

                            return false ;
                        }

                        /* 将源父节点打包并存回 */
                        packFolderIndex(sec__, source_father_fo__);
                        destroyFolderIndex(&source_father_fo__);
                        if (!saveSection(sec__, source__father__)) {
                            free(sec__);
                            setError(READ_WRITE_DISK_ERROR);

                            /* 工作日志输出 */
                            if (LOG_OUTPUT) {
                                logPrintf(FILE_API_LOG, "操作：移动文件（夹）\n目的路径：%s\n源路径：%s\n移动失败，原因：%s\n\n"
                                                , __dest_path, __source_path, errorDesc[errorCode]) ;
                            }

                            return false ;
                        }

                        /* 移动文件成功 */
                        free(sec__) ;

                        /* 工作日志输出 */
                        if (LOG_OUTPUT) {
                            logPrintf(FILE_API_LOG, "操作：移动文件（夹）\n目的路径：%s\n源路径：%s\n移动成功\n\n"
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

    /* 工作日志输出 */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "操作：移动文件（夹）\n目的路径：%s\n源路径：%s\n移动失败，原因：%s\n\n", __dest_path, __source_path, errorDesc[errorCode]) ;
    }

    return false ;
}

bool copyFile(const char * __dest_path, const char * __source_path)
{
    /**
     * 将源路径的文件复制到目的路径的文件夹下
     */

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    int source__, dest__father__, sn__ ;

    /* 读取目的路径 */
    if (getIndexSection(sec__, &dest__father__, __dest_path)) {
        /* 目的路径必须是文件夹 */
        if (sec__[0] != FILE_TYPE) {
            FolderIndex dest_father_fo__ ;

            /* 提取目的文件夹索引块 */
            initFolderIndex(&dest_father_fo__) ;
            extractFolderIndex(sec__, &dest_father_fo__) ;

            /* 超过了文件夹最大子节点数，则无法复制 */
            if (dest_father_fo__.childNodeNumber + 1 > MAX_CHILD_NUMBER) {
                destroyFolderIndex(&dest_father_fo__);
                free(sec__);
                setError(NO_ENOUGH_CHILD_NODE);

                /* 工作日志输出 */
                if (LOG_OUTPUT) {
                    logPrintf(FILE_API_LOG, "操作：复制文件\n目的路径：%s\n源路径：%s\n复制失败，原因：%s\n\n"
                                    , __dest_path, __source_path, errorDesc[errorCode]) ;
                }

                return false ;
            }

            /* 读取源路径 */
            if (getIndexSection(sec__, &source__, __source_path)) {
                /* 只能复制文件 */
                if (sec__[0] == FILE_TYPE) {
                    /* 重名检查 */
                    if (searchChildNode(&sn__, dest_father_fo__, &sec__[FILE_OFFSET_NAME])) {
                        free(sec__) ;
                        destroyFolderIndex(&dest_father_fo__);
                        setError(SAME_NAME);

                        /* 工作日志输出 */
                        if (LOG_OUTPUT) {
                            logPrintf(FILE_API_LOG, "操作：复制文件\n目的路径：%s\n源路径：%s\n复制失败，原因：%s\n\n"
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

                    /* 保存源数据扇区信息 */
                    source_data__ = dest_fi__.dataNode ;
                    dsn__ = dest_fi__.dataSectionNumber ;
                    /* 清空数据扇区信息 */
                    dest_fi__.dataSectionNumber = 0 ;
                    dest_fi__.dataNode = NULL ;

                    /* 修改父节点 */
                    dest_fi__.fatherNode = dest__father__ ;

                    /* 申请磁盘空间，数据扇区加一个索引块扇区 */
                    allo__ = (int *)malloc(sizeof(int) * (dsn__ + 1)) ;
                    if (applyForFreeSpace(dsn__ + 1, allo__)) {
                        /* 复制数据块扇区 */
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

                        /* 添加新申请的数据块节点并将索引块存回 */
                        if (moreDataNode(&dest_fi__, dsn__, &allo__[1])) {

                            packFileIndex(sec__, dest_fi__) ;

                            if (saveSection(sec__, allo__[0])) {
                                /* 为父节点增加一个子节点，并保存 */
                                if (moreChildNode(&dest_father_fo__, 1, allo__)) {

                                    packFolderIndex(sec__, dest_father_fo__) ;

                                    /* 复制完成 */
                                    if (saveSection(sec__, dest__father__)) {
                                        free(sec__);
                                        free(allo__);
                                        free(source_data__) ;
                                        destroyFolderIndex(&dest_father_fo__);
                                        destroyFileIndex(&dest_fi__) ;

                                        /* 工作日志输出 */
                                        if (LOG_OUTPUT) {
                                            logPrintf(FILE_API_LOG, "操作：复制文件\n目的路径：%s\n源路径：%s\n复制成功\n\n"
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

    /* 工作日志输出 */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "操作：复制文件\n目的路径：%s\n源路径：%s\n复制失败，原因：%s\n\n", __dest_path, __source_path, errorDesc[errorCode]) ;
    }

    return false ;
}

bool copyFolder(char * __dest_path, char * __source_path)
{
    /**
     * 将源路径的文件夹及其目录下所有
     * 文件（夹）复制到目的路径的文件夹下
     */

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    int source__, dest__father__, sn__ ;

    /* 读取目的路径 */
    if (getIndexSection(sec__, &dest__father__, __dest_path)) {
        /* 目的路径必须是文件夹 */
        if (sec__[0] != FILE_TYPE) {
            /* 检查目的路径是否是源路径的子目录 */
            if (isSubFolder(__dest_path, __source_path)) {
                setError(DEST_FOLDER_IS_SUB_FOLDER) ;
                free(sec__) ;
                return false ;
            }

            FolderIndex dest_father_fo__ ;

            /* 提取目标路径索引块信息 */
            initFolderIndex(&dest_father_fo__);
            extractFolderIndex(sec__, &dest_father_fo__);

            /* 超过了文件夹最大子节点数，则无法复制 */
            if (dest_father_fo__.childNodeNumber + 1 > MAX_CHILD_NUMBER) {
                destroyFolderIndex(&dest_father_fo__);
                free(sec__);
                setError(NO_ENOUGH_CHILD_NODE);

                /* 工作日志输出 */
                if (LOG_OUTPUT) {
                    logPrintf(FILE_API_LOG, "操作：复制文件夹\n目的路径：%s\n源路径：%s\n复制失败，原因：%s\n\n"
                                    , __dest_path, __source_path, errorDesc[errorCode]) ;
                }

                return false ;
            }

            /* 读取源路径 */
            if (getIndexSection(sec__, &source__, __source_path)) {
                /* 只能复制文件夹 */
                if (sec__[0] == FOLDER_TYPE) {
                    /* 重名检查 */
                    if (searchChildNode(&sn__, dest_father_fo__, &sec__[FILE_OFFSET_NAME])) {
                        free(sec__) ;
                        destroyFolderIndex(&dest_father_fo__);
                        setError(SAME_NAME);

                        /* 工作日志输出 */
                        if (LOG_OUTPUT) {
                            logPrintf(FILE_API_LOG, "操作：复制文件夹\n目的路径：%s\n源路径：%s\n复制失败，原因：%s\n\n"
                                            , __dest_path, __source_path, errorDesc[errorCode]) ;
                        }

                        return false ;
                    }

                    int allo__ ;

                    /* 申请一个索引块扇区节点 */
                    if (applyForFreeSpace(1, &allo__)) {
                        FolderIndex dest_child_fo__ ;
                        int dsn__ ;
                        int * source_data__ ;

                        /* 将源文件夹的索引块信息提取到目的文件索引块中 */
                        initFolderIndex(&dest_child_fo__);
                        extractFolderIndex(sec__, &dest_child_fo__);

                        /* 修改父节点 */
                        dest_child_fo__.fatherNode = dest__father__ ;

                        /* 记录源文件夹的子节点信息 */
                        dsn__ = dest_child_fo__.childNodeNumber ;
                        source_data__ = dest_child_fo__.childNode ;

                        /* 清空源文件夹子节点 */
                        dest_child_fo__.childNodeNumber = 0 ;
                        dest_child_fo__.childNode = NULL ;

                        /* 在父节点中增加该子节点 */
                        moreChildNode(&dest_father_fo__, 1, &allo__);

                        packFolderIndex(sec__, dest_father_fo__);
                        if (saveSection(sec__, dest__father__)) {
                            packFolderIndex(sec__, dest_child_fo__);

                            if (saveSection(sec__, allo__)) {
                                /* 用递归的方法复制原文件夹下的子目录 */
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

                                /* 工作日志输出 */
                                if (LOG_OUTPUT) {
                                    logPrintf(FILE_API_LOG, "操作：复制文件夹\n目的路径：%s\n源路径：%s\n复制成功\n\n"
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

    /* 工作日志输出 */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "操作：复制文件夹\n目的路径：%s\n源路径：%s\n复制失败，原因：%s\n\n", __dest_path, __source_path, errorDesc[errorCode]) ;
    }

    return false ;
}

bool setFileAttribute(FileAttribute __attr, const char * __path)
{
    /* 权限检测 */
    if (accountLevel == GUEST_USER) {
        setError(PERMISSION_IS_RESTRICTED);
        return false ;
    }

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    int secNum__ ;

    /* 读取目的路径 */
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

                /* 工作日志输出 */
                if (LOG_OUTPUT) {
                    char * buf[2] = {"否", "是"} ;
                    logPrintf(FILE_API_LOG, "操作：设置文件属性\n文件路径：%s\n隐藏：%s\t只读：%s\n\n", __path, buf[__attr.hide], buf[__attr.readOnly]) ;
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

    /* 工作日志输出 */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "操作：设置文件属性\n文件路径：%s\n设置失败，原因：%s\n\n", __path, errorDesc[errorCode]) ;
    }

    return false ;
}

bool setFolderAttribute(FolderAttribute __attr, const char * __path)
{
    /* 权限检测 */
    if (accountLevel == GUEST_USER) {
        setError(PERMISSION_IS_RESTRICTED);
        return false ;
    }

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;
    int secNum__ ;

    /* 读取目的路径 */
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

                /* 工作日志输出 */
                if (LOG_OUTPUT) {
                    char * buf[2] = {"否", "是"} ;
                    logPrintf(FILE_API_LOG, "操作：设置文件夹属性\n文件夹路径：%s\n隐藏：%s\t保护：%s\n\n", __path, buf[__attr.hide], buf[__attr.protect]) ;
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

    /* 工作日志输出 */
    if (LOG_OUTPUT) {
        logPrintf(FILE_API_LOG, "操作：设置文件夹属性\n文件夹路径：%s\n设置失败，原因：%s\n\n", __path, errorDesc[errorCode]) ;
    }

    return false ;
}

FileStream * openFile(const char * __path)
{
    /**
     * 打开指定路径的文件，返回该文件对应的文件流
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

            /* 添加到打开文件列表 */
            addOpenFile(__path);

            free(sec__) ;

            /* 工作日志输出 */
            if (LOG_OUTPUT) {
                logPrintf(FILE_STREAM_LOG, "操作：打开文件\n文件路径：%s\n打开成功，文件流信息：\n索引块扇区号：%d\n读取指针:%d\n写入指针:%d\n\n"
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

    /* 工作日志输出 */
    if (LOG_OUTPUT) {
        logPrintf(FILE_STREAM_LOG, "操作：打开文件\n文件路径：%s\n打开失败，原因：%s\n\n", __path, errorDesc[errorCode]) ;
    }

    return NULL ;
}

void closeFile(FileStream * __fs)
{
    /**
     * 关闭文件，释放文件流，可以再次打开其他文件
     */

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;

    if (__fs != NULL) {
        /* 从文件打开列表中删除 */
        closeOpenFile(__fs->path);

        /* 工作日志输出 */
        if (LOG_OUTPUT) {
            logPrintf(FILE_STREAM_LOG, "操作：关闭文件流\n文件路径：%s\n文件索引块扇区号：%d\n关闭成功\n\n", __fs->path, __fs->indexSectionNumber) ;
        }

        free(__fs->path);
        free(__fs) ;

        __fs = NULL ;
    }
}

int getFileLength(FileStream * __fs)
{
    /**
     * 返回文件流所对应的文件的长度
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

            /* 工作日志输出 */
            if (LOG_OUTPUT) {
                logPrintf(FILE_STREAM_LOG, "操作：获取文件长度\n文件路径：%s\n文件索引块扇区号：%d\n获取成功，文件长度：%d\n\n" , __fs->path, __fs->indexSectionNumber, len__) ;
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

    /* 工作日志输出 */
    if (LOG_OUTPUT) {
        logPrintf(FILE_STREAM_LOG, "操作：获取文件长度\n文件路径：%s\n文件索引块扇区号：%d\n获取失败，原因：%s\n\n" , __fs->path, __fs->indexSectionNumber, errorDesc[errorCode]) ;
    }

    return 0 ;
}

int readWholeFile(char * __buf, FileStream * __fs)
{
    /**
     * 将文件流中的数据读入buf中
     * 读取文件的全部数据
     */

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;

    if (readSection(sec__, __fs->indexSectionNumber)) {
        if (sec__[0] == FILE_TYPE) {
            FileIndex fi__ ;
            /* 剩余的未读取的字符数，已经读取的字符数，准备读取的扇区序列号 */
            int left__, have__ = 0, whsec = 0 ;

            initFileIndex(&fi__) ;
            extractFileIndex(sec__, &fi__) ;

            left__ = fi__.length ;

            /* 读取数据 */
            while (left__ > 0 && whsec <= fi__.dataSectionNumber-1) {
                /* 直接读取整个扇区 */
                if (left__ >= sectionSize) {
                    if (readSection(&__buf[have__], fi__.dataNode[whsec])) {
                        have__ += sectionSize ;
                        left__ -= sectionSize ;
                    }
                    else {
                        setError(READ_WRITE_DISK_ERROR) ;
                        free(sec__) ;

                        /* 工作日志输出 */
                        if (LOG_OUTPUT) {
                            logPrintf(FILE_STREAM_LOG, "操作：读取全部文件内容\n文件路径：%s\n文件索引块扇区号：%d\n读取失败，原因：%s\n读取字节数：%d\n\n"
                                            , __fs->path, __fs->indexSectionNumber, errorDesc[errorCode], have__) ;
                        }

                        return have__ ;
                    }
                }
                /* 先读取整个扇区，再将扇区前left__个拷贝到buf中 */
                else {
                    if (readSection(sec__, fi__.dataNode[whsec])) {
                        copyString(&__buf[have__], sec__, left__) ;
                        have__ += left__ ;
                        left__ -= left__ ;
                    }
                    else {
                        setError(READ_WRITE_DISK_ERROR) ;
                        free(sec__) ;

                        /* 工作日志输出 */
                        if (LOG_OUTPUT) {
                            logPrintf(FILE_STREAM_LOG, "操作：读取全部文件内容\n文件路径：%s\n文件索引块扇区号：%d\n读取失败，原因：%s\n读取字节数：%d\n\n"
                                            , __fs->path, __fs->indexSectionNumber, errorDesc[errorCode], have__) ;
                        }

                        return have__ ;
                    }
                }

                /* 下一个扇区 */
                whsec ++ ;
            }

            /* 修改读取指针 */
            __fs->readPtr = have__ ;

            free(sec__) ;

            /* 工作日志输出 */
            if (LOG_OUTPUT) {
                char * buf = (char *)malloc(sizeof(char) * (have__+1));

                copyString(buf, __buf, have__) ;
                buf[have__] = '\0' ;

                logPrintf(FILE_STREAM_LOG, "操作：读取全部文件内容\n文件路径：%s\n文件索引块扇区号：%d\n读取成功，文件内容：\n%s\n文件长度：%d\n\n" , __fs->path, __fs->indexSectionNumber, buf, have__) ;

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

    /* 工作日志输出 */
    if (LOG_OUTPUT) {
        logPrintf(FILE_STREAM_LOG, "操作：读取全部文件内容\n文件路径：%s\n文件索引块扇区号：%d\n读取失败，原因：%s\n读取字节数：%d\n\n" , __fs->path, __fs->indexSectionNumber, errorDesc[errorCode], 0) ;
    }

    return 0 ;
}

bool saveWholeFile(char * __buf, int __count, FileStream * __fs)
{
    /**
     * 将buf中最多count个字符写入到文件流fs中
     * 会覆盖以前的数据
     */

    char * sec__ = (char *)malloc(sizeof(char) * sectionSize) ;

    if (readSection(sec__, __fs->indexSectionNumber)) {
        if (sec__[0] == FILE_TYPE) {
            FileIndex fi__ ;
            int now_need__, past_need__ ;

            initFileIndex(&fi__) ;
            extractFileIndex(sec__, &fi__) ;

            /* 不能修改只读文件内容 */
            if (fi__.attribute.readOnly) {
                destroyFileIndex(&fi__) ;
                free(sec__) ;
                setError(CAN_NOT_MODIFY_READ_ONLY_FILE);

                /* 工作日志输出 */
                if (LOG_OUTPUT) {
                    logPrintf(FILE_STREAM_LOG, "操作：保存文件内容\n文件路径：%s\n文件索引块扇区号：%d\n保存失败，原因：%s\n\n" , __fs->path, __fs->indexSectionNumber, errorDesc[errorCode]) ;
                }

                return false ;
            }

            /* 记录已使用的数据扇区数 */
            past_need__ = fi__.dataSectionNumber ;
            /* 计算保存新的数据需要的数据扇区数 */
            if (__count == 0) {
                now_need__ = 0 ;
            }
            else {
                now_need__ = (__count / sectionSize) + 1 ;
            }

            /* 需要更多的数据扇区 */
            if (now_need__ > past_need__) {
                int more__ = now_need__ - past_need__ ;
                int * allo__ = (int *)malloc(sizeof(int) * more__) ;

                if (applyForFreeSpace(more__, allo__)) {
                    if (moreDataNode(&fi__, more__, allo__)) {
                        ;
                    }
                    else {
                        setError(FILE_TOO_BIG);
                        /* 应将申请的扇区释放 */
                        releaseFreeSpace(more__, allo__);
                        free(allo__);
                        free(sec__);

                        /* 工作日志输出 */
                        if (LOG_OUTPUT) {
                            logPrintf(FILE_STREAM_LOG, "操作：保存文件内容\n文件路径：%s\n文件索引块扇区号：%d\n保存失败，原因：%s\n\n" , __fs->path, __fs->indexSectionNumber, errorDesc[errorCode]) ;
                        }

                        return false ;
                    }
                }
                else {
                    setError(NO_ENOUGH_DISK_SPACE);
                    free(allo__);
                    free(sec__);

                    /* 工作日志输出 */
                    if (LOG_OUTPUT) {
                        logPrintf(FILE_STREAM_LOG, "操作：保存文件内容\n文件路径：%s\n文件索引块扇区号：%d\n保存失败，原因：%s\n\n" , __fs->path, __fs->indexSectionNumber, errorDesc[errorCode]) ;
                    }

                    return false ;
                }
            }
            /* 所需数据扇区减少，应释放掉多余的扇区 */
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

            /* 清空文件长度 */
            fi__.length = 0 ;

            /* 保存数据 */
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

            /* 更新上次修改时间 */
            TimeStamp ts__ ;
            getSystemTime(&ts__) ;
            fi__.lastModifyTime = ts__ ;

            packFileIndex(sec__, fi__) ;
            destroyFileIndex(&fi__) ;

            /* 保存索引块信息 */
            if (saveSection(sec__, __fs->indexSectionNumber)) {
                __fs->writePtr = have__ ;

                free(sec__);

                /* 工作日志输出 */
                if (LOG_OUTPUT) {
                    char * buf = (char *)malloc(sizeof(char) * (__count+1));

                    copyString(buf, __buf, __count) ;
                    buf[__count] = '\0' ;

                    logPrintf(FILE_STREAM_LOG, "操作：保存文件内容\n文件路径：%s\n文件索引块扇区号：%d\n保存成功，文件内容：\n%s\n文件长度：%d\n\n" , __fs->path, __fs->indexSectionNumber, buf, __count) ;

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

    /* 工作日志输出 */
    if (LOG_OUTPUT) {
        logPrintf(FILE_STREAM_LOG, "操作：保存文件内容\n文件路径：%s\n文件索引块扇区号：%d\n保存失败，原因：%s\n\n" , __fs->path, __fs->indexSectionNumber, errorDesc[errorCode]) ;
    }

    return false ;
}
