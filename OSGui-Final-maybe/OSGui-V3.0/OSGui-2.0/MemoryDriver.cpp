/**
* MyOS/MemoryDrive.c
*
* Copyright (c) 2016 LiangJie
*/

/**
* MemoryDrive.c包含了内存驱动程序提供的所有操作内存会用到的函数
* 这些函数模拟了硬件接口，包括内存重置，内存读，内存写，检测内存地址是否溢出
* 驱动程序调用这些函数来进行内存读写，对硬件进行操作
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
int memorySize = 0;                                              /* 内存大小 */
char *memoryDescription = (char *)malloc(sizeof(char) * 100);	    /*   内存描述   */

/* 内存位置 */
const char * memoryPath = "PC\\Memory\\";
/* 内存文件 */
const char *memoryFileName = "PC\\Memory\\Memory.txt";
extern int frameSize;
extern int frameCount;

bool readMemoryConfig()
{
    /**
    * 读取内存的配置信息，此操作应该在
    * 操作系统启动时由BIOS程序调用
    * 读出内存大小
    */

    /* 配置文件路径 */
    char * configPath__ = (char *)malloc(sizeof(char) * 38);
    sprintf(configPath__, "%sConfig.txt", memoryPath);

    char * logContent__ = (char *)malloc(sizeof(char) * 200);

    /* 打开配置文件，失败则返回false */
    FILE * ifp__ = fopen(configPath__, "r");
    if (ifp__ == NULL) {
        setError(MEMORY_CONFIG_NOT_EXIST);
        sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:读取内存硬件信息","操作结果:读取失败","失败原因:内存配置文件不存在");
        logPrintf(MEMORY_DRIVER_LOG, logContent__);

        return false;
    }

    /* 按顺序读取数据，若检测到不符合配置文件格式则返回false */
    if (fgets(memoryDescription, 100, ifp__) == NULL) {
        setError(MEMORY_CONFIG_DESC_TOO_LONG_OR_SHORT);
        sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:读取内存硬件信息","操作结果:读取失败", "失败原因:内存配置文件格式不正确");
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return false;
    }
    if (fscanf(ifp__, "%d", &memorySize) != 1) {
        setError(MEMORY_CONFIG_MEMORYSIZE_ERROR);
        sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:读取内存硬件信息","操作结果:读取失败", "失败原因:内存配置文件内存大小格式错误");
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return false;
    }

    fclose(ifp__);
    free(configPath__);

    sprintf(logContent__, "%s\t%s%s%s%d\n\n", "操作:读取内存硬件信息","操作结果:读取成功，内存描述:", memoryDescription, "内存大小:",memorySize);

    logPrintf(MEMORY_DRIVER_LOG, logContent__);
    /* 读取成功 */
    return true;
}

bool saveMemoryConfig()
{
    /**
    * 保存内存的配置信息，此操作只能在内存配置
    * 信息被修改时调用，例如插拔内存条之后
    */

    /* 配置文件路径 */
    char * configPath__ = (char *)malloc(sizeof(char) * 38);
    sprintf(configPath__, "%sConfig.txt", memoryPath);

    /* 打开配置文件，失败则返回false */
    FILE * ofp__ = fopen(configPath__, "w");
    if (ofp__ == NULL) {
        setError(MEMORY_CONFIG_NOT_EXIST);
        return false;
    }
    /* 设置内存描述信息 */
    sprintf(memoryDescription, "%s", "2016年联想生产");

    memorySize = 2048;             /* 设置内存大小 */
    /* 按顺序写入数据 */
    fprintf(ofp__, "%s\n%d\n", memoryDescription, memorySize);

    fclose(ofp__);
    free(configPath__);

    /* 写入成功 */
    return true;
}

bool memoryReset() {
    /* 重置内存 */
    FILE *memoryFile__;
    unsigned char data__ = 255;

    /*新建内存文件*/
    if ((memoryFile__ = fopen(memoryFileName, "wb")) == NULL)
    {
        setError(MEMORY_NOT_EXIST);
        return false;
    }

    /*写入数据 “全1” */
    for (int i = 0; i < memorySize; i++) {
        fwrite(&data__, sizeof(data__), 1, memoryFile__);
    }

    fclose(memoryFile__);
    return true;
}

/**
* [initializeMemoryHard 该函数初始化内存硬件]
* @return [description]
*/
bool initializeMemoryHard() {
    /* 读取内存配置信息*/
    if (!readMemoryConfig()) {
        return false;
    }
    /* 重置内存 */
    if (!memoryReset()) {
        return false;
    }
    return true;
}

bool isOverflow(int __offset) {
    /**
    *检查是否越界
    *越界返回false，否则返回true
    */
    FILE *memoryFile__;

    /*检测内存文件是否存在，不存在返回false*/
    if ((memoryFile__ = fopen(memoryFileName, "rb")) == NULL) {
        setError(MEMORY_NOT_EXIST);
        return false;
    }
    else {
        fclose(memoryFile__);
        /*检查是否越界*/
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
    *从内存的offset位置读取一个字节的数据
    */
    FILE *memoryFile__;

    char * logContent__ = (char *)malloc(sizeof(char) * 200);

    /*打开内存文件*/
    memoryFile__ = fopen(memoryFileName, "rb");
    if (memoryFile__ == NULL) {
        /*文件不存在返回“全1”*/
        setError(MEMORY_NOT_EXIST);
        sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:读内存单元","操作结果:读取失败", "失败原因:内存不存在");
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return false;
    }
    else {
        /*检查是否溢出*/
        if (!isOverflow(__offset)) {
            sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:读内存单元", "操作结果:读取失败", "失败原因:内存地址溢出");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;

        }

        /*定位*/
        int result__ = fseek(memoryFile__, __offset, SEEK_SET);
        if (result__ < 0) {
            /*未找到返回“全1”*/
            setError(MEMORYADDRESS_OUT_OF__BOUND);
            sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:读内存单元","操作结果:读取失败", "失败原因:内存地址不正确");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }

        /*读数据到data*/
        fread(__toRead, sizeof(unsigned char), 1, memoryFile__);
        fclose(memoryFile__);
        memoryFile__ = NULL;

        sprintf(logContent__, "%s\t%s\t%s%d\t%s%02X\n\n", "操作:读内存单元","操作结果:读取成功", "读取位置:", __offset, "读取内容:", *__toRead);

        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return true;
    }
}

bool writeMemoryUnit(int __offset, const unsigned char __toSave) {
    /*
    * 从内存的offset位置写一个字节的数据
    */
    FILE *memoryFile__;

    char * logContent__ = (char *)malloc(sizeof(char) * 200);

    /*打开内存文件*/
    if ((memoryFile__ = fopen(memoryFileName, "rb+")) == NULL) {
        /*文件不存在*/
        setError(MEMORY_NOT_EXIST);
        sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:写内存单元","操作结果:写入失败", "失败原因:内存不存在");
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return false;
    }
    else{
        /*检查是否溢出*/
        if (!isOverflow(__offset)) {
            sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:写内存单元", "操作结果:写入失败", "失败原因:内存地址溢出");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }

        /*定位*/
        int result__ = fseek(memoryFile__, __offset, SEEK_SET);
        if (result__ < 0) {
            /*未成功返回false*/
            setError(MEMORYADDRESS_OUT_OF__BOUND);
            sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:写内存单元","操作结果:写入失败", "失败原因:内存地址错误");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }

        fwrite(&__toSave, sizeof(__toSave), 1, memoryFile__);
        fclose(memoryFile__);

        sprintf(logContent__, "%s\t%s\t%s%d\t%s%02X\n\n", "操作:写内存单元","操作结果:写入成功", "写入位置:", __offset, "写入内容:", __toSave);
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return true;
    }
}

bool readMemoryByFrame(int __offset, char* __data) {
    /**
    *从内存的offset位置读取一个帧的数据
    */
    FILE *memoryFile__;

    char * logContent__ = (char *)malloc(sizeof(char) * 200);
    /*打开内存文件*/
    if ((memoryFile__ = fopen(memoryFileName, "rb")) == NULL) {
        /*文件不存在返回“全1”*/
        setError(MEMORY_NOT_EXIST);
        sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:读内存帧", "操作结果:读取失败", "失败原因:内存不存在");
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return false;
    }
    else {
        /*检查下界是否溢出*/
        if (!isOverflow(__offset)) {
            sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:读内存帧", "操作结果:读取失败", "失败原因:内存地址下溢");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }
        /*检查上界是否溢出*/
        if (!isOverflow(__offset + frameSize - 1)) {
            sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:读内存帧", "操作结果:读取失败", "失败原因:内存地址上溢");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }


        /*定位*/
        int result__ = fseek(memoryFile__, __offset, SEEK_SET);
        if (result__ < 0) {
            /*未找到返回“全1”*/
            setError(MEMORYADDRESS_OUT_OF__BOUND);
            sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:读内存帧", "操作结果:读取失败", "失败原因:内存地址不正确");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }

        /*读数据到data*/
        fread(__data, sizeof(char), frameSize, memoryFile__);
        fclose(memoryFile__);


         char * hexData__ = (char*)malloc(sizeof(char) * frameSize * 2);
         strToHex(__data,hexData__);

        sprintf(logContent__, "%s\t%s\t%s%s\n\n", "操作:读内存帧", "操作结果:读取成功", "帧内容:",hexData__);
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return true;
    }
}

bool writeMemoryByFrame(int __offset, char *__toSave) {
    /*
    * 从内存的offset位置写一个帧的数据
    */
    FILE *memoryFile__;

    char * logContent__ = (char *)malloc(sizeof(char) * 200);

    /*打开内存文件*/
    if ((memoryFile__ = fopen(memoryFileName, "rb+")) == NULL) {
        /*文件不存在*/
        setError(MEMORY_NOT_EXIST);
        sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:写内存帧", "操作结果:写入失败", "失败原因:内存不存在");
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return false;
    }
    else{
        /*检查下界是否溢出*/
        if (!isOverflow(__offset)) {
            sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:写内存帧", "操作结果:写入失败", "失败原因:内存地址下界溢出");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }

        /*检查上界是否溢出*/
        if (!isOverflow(__offset + frameSize - 1)) {
            sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:写内存帧", "操作结果:写入失败", "失败原因:内存地址上界溢出");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }


        /*定位*/
        int result__ = fseek(memoryFile__, __offset, SEEK_SET);
        if (result__ < 0) {
            /*未成功返回false*/
            setError(MEMORYADDRESS_OUT_OF__BOUND);
            sprintf(logContent__, "%s\t%s\t%s\n\n", "操作:写内存帧", "操作结果:写入失败", "失败原因:内存地址越界");
            logPrintf(MEMORY_DRIVER_LOG, logContent__);
            return false;
        }

        fwrite(__toSave, sizeof(char), frameSize, memoryFile__);
        fclose(memoryFile__);

        char * hexData__ = (char*)malloc(sizeof(char) * frameSize * 2);
        strToHex(__toSave,hexData__);

        sprintf(logContent__, "%s\t%s\t%s%s\n\n", "操作:写内存帧", "操作结果:写入成功", "写入内容:\n",hexData__);
        logPrintf(MEMORY_DRIVER_LOG, logContent__);
        return true;
    }
}

