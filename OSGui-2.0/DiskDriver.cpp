/**
 * MyOS/DiskDriver.c
 * 
 * Copyright (c) 2016 LiNanQinQin
 */

/**
 * DiskDriver.c包含了磁盘驱动程序提供的所有操作磁盘会用到的函数
 * 这些函数模拟了硬件接口，包括格式化、读取/保存扇区数据
 */

#include "DiskDriver.h"
#include "WorkLog.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <QDebug>
extern bool LOG_OUTPUT;
/**
 * 磁盘配置信息，包括磁盘描述，最大容量
 * 扇区数量和扇区大小
 */
char * diskDescription = (char *)malloc(sizeof(char) * 200);   	/*   磁盘描述   */
int maxDiskSize = 0 ;											/* 磁盘最大容量 */
int sectionNumber = 0 ;											/*   扇区数量   */
int sectionSize = 0 ;											/*   扇区大小   */

/* 磁盘位置 */
const char * diskPath = "PC\\Disk\\" ;

HANDLE diskMutex = NULL ;

bool readDiskConfig()
{
	/**
	 * 读取磁盘的配置信息，此操作应该在
	 * 操作系统启动时由BIOS程序调用
	 */
	
	/* 配置文件路径 */
	char * configPath = (char *)malloc(sizeof(char) * 38) ;
	sprintf(configPath, "%sConfig.txt", diskPath);
	
	/* 打开配置文件，失败则返回false */
	FILE * ifp = fopen(configPath, "r");
	if (ifp == NULL) {
		return false ;
	}
	
	/* 按顺序读取数据，若检测到不符合配置文件格式则返回false */
    if ( fgets(diskDescription, 200, ifp) == NULL ) {
		return false ;
	}
    //qDebug() << diskDescription ;
	if (fscanf(ifp, "%d %d %d", &maxDiskSize, &sectionNumber, &sectionSize) != 3) {
		return false ;
	}
	
	fclose(ifp);
	free(configPath);
	
	diskMutex = CreateMutex(NULL, FALSE, NULL) ;
	
	/* 读取成功 */
	return true ;
}

bool saveDiskConfig()
{
	/**
	 * 保存磁盘的配置信息，此操作只能在磁盘配置
	 * 信息被修改时调用，例如磁盘格式化时
	 * 除此之外均不能修改配置文件中的信息
	 */
	 
	/* 配置文件路径 */
	char * configPath = (char *)malloc(sizeof(char) * 38) ;
	sprintf(configPath, "%sConfig.txt", diskPath);
	
	/* 打开配置文件，失败则返回false */
	FILE * ofp = fopen(configPath, "w");
	if (ofp == NULL) {
		return false ;
	}
	
	/* 按顺序写入数据 */
	fprintf(ofp, "%s%d\n%d\n%d\n", diskDescription, maxDiskSize, sectionNumber, sectionSize) ;
	
	fclose(ofp);
	free(configPath);
	
	/* 写入成功 */
	return true ;
}

int formatting(int __sectionSize)
{
	/**
	 * 将磁盘格式化，构建多个扇区
	 * sectionSize为格式化后生成的扇区大小
	 * 返回值为格式化后可用扇区的数量，若格式化失败则
	 * 返回0（扇区大小不符合要求）或负值（路径有误）
	 */

	/* sectionSize <= 0 或者大于磁盘容量则无法格式化，返回0 */
	if (__sectionSize <= 0 || __sectionSize > maxDiskSize) {
		return 0 ;
	}

	/* 扇区数量 */
	int __sectionNumber = 0 ;

	/* 计算扇区数量，取整 */
	__sectionNumber = maxDiskSize / __sectionSize ;

	/* 生成扇区文件 */
	FILE * ofp ;
	int i = 0, j = 0;
	unsigned char data = 255 ;
	char * sectionPath = (char *)malloc(sizeof(char) * 38);

	for (i=0; i <= __sectionNumber - 1; i++) {
		/* 生成扇区文件路径，形如PC\Memory\123.txt（第123号扇区） */
		sprintf(sectionPath, "%s%d.txt", diskPath, i) ;
	
		ofp = fopen(sectionPath, "wb") ;
		/* 文件路径有误，返回-1 */
		if (ofp == NULL) {
			return -1 ;
		}
		
		/* 写扇区文件，数据初始化为全1 */
		for (j=0; j <= __sectionSize - 1; j++) {
			fwrite(&data, sizeof(data), 1, ofp) ;
		}
		
		fclose(ofp) ;
	}
	
	free(sectionPath) ;
	
	/* 为此次格式化生成新的磁盘位图 */
	/* if (!newBitVector(__sectionNumber)) {
		return -1 ;
	} */
	
	/* 格式化成功，修改并保存磁盘配置信息 */
	sectionNumber = __sectionNumber;
	sectionSize = __sectionSize;
	if ( !saveDiskConfig() ) {
		return -2 ;
	}
	
	return __sectionNumber ;
}

bool readSection(char * __section, int whichSection)
{
	/**
	 * 将whichSection号扇区的内容读到从__section开始的内存区域中
	 * __section必须保证足够长的空间
	 * 建议为__section申请长度为sectionSize的内存空间
	 */
	
	/* whichSection指定的扇区号不存在，则返回false */
	if (whichSection < 0 || whichSection > sectionNumber) {
		
		if (LOG_OUTPUT) {
			logPrintf(DISK_DRIVER_LOG, "操作：读取扇区\t扇区号：%d\t读取失败，扇区号越界\n\n", whichSection) ;
		}
		
		return false ;
	}
	
	/* 计算扇区路径 */
	char * sectionPath = (char *)malloc(sizeof(char) * 38);
	sprintf(sectionPath, "%s%d.txt", diskPath, whichSection) ;
	
	/* 申请磁盘操作锁 */
	WaitForSingleObject(diskMutex, INFINITE) ;
	
	/* 打开扇区文件 */
	FILE * ifp = fopen(sectionPath, "rb");
	if (ifp == NULL) {
		
		if (LOG_OUTPUT) {
			logPrintf(DISK_DRIVER_LOG, "操作：读取扇区\t扇区号：%d\t读取失败，扇区文件失效\n\n", whichSection) ;
		}
		
		ReleaseMutex(diskMutex) ;
		return false ;
	}
	
	/* 读取扇区数据 */
	fread(__section, sizeof(char), sectionSize, ifp);
	if (feof(ifp)) {
		
		if (LOG_OUTPUT) {
			logPrintf(DISK_DRIVER_LOG, "操作：读取扇区\t扇区号：%d\t读取失败，扇区损坏\n\n", whichSection) ;
		}
		
		ReleaseMutex(diskMutex) ;
		return false ;
	}
	
	fclose(ifp);
	free(sectionPath);
	
	ReleaseMutex(diskMutex) ;
	
	if (LOG_OUTPUT) {
		logPrintf(DISK_DRIVER_LOG, "操作：读取扇区\t扇区号：%d\t读取成功\n\n", whichSection) ;
	}
	
	return true ;
}

bool saveSection(char * __section, int whichSection)
{
	/**
	 * 将从__section开始的sectionSize大小的内容保存到whichSection号扇区
	 * __section必须保证足够长的空间，否则返回false
	 * 建议为__section申请长度为sectionSize的内存空间
	 */
	
	/* whichSection指定的扇区号不存在，则返回false */
	if (whichSection < 0 || whichSection > sectionNumber) {
		
		if (LOG_OUTPUT) {
			logPrintf(DISK_DRIVER_LOG, "操作：写入扇区\t扇区号：%d\t写入失败，扇区号越界\n\n", whichSection) ;
		}
		
		return false ;
	}
	
	/* 计算扇区路径 */
	char * sectionPath = (char *)malloc(sizeof(char) * 38);
	sprintf(sectionPath, "%s%d.txt", diskPath, whichSection) ;
	
	/* 申请磁盘操作锁 */
	WaitForSingleObject(diskMutex, INFINITE) ;
	
	/* 打开扇区文件 */
	FILE * ofp = fopen(sectionPath, "wb");
	if (ofp == NULL) {
		
		if (LOG_OUTPUT) {
			logPrintf(DISK_DRIVER_LOG, "操作：写入扇区\t扇区号：%d\t写入失败，扇区文件失效\n\n", whichSection) ;
		}
		
		ReleaseMutex(diskMutex) ;
		return false ;
	}
	
	/* 保存扇区数据 */
	fwrite(__section, sizeof(char), sectionSize, ofp);
	
	fclose(ofp);
	free(sectionPath);
	
	ReleaseMutex(diskMutex) ;
	
	if (LOG_OUTPUT) {
		logPrintf(DISK_DRIVER_LOG, "操作：写入扇区\t扇区号：%d\t写入成功\n\n", whichSection) ;
	}
	
	return true ;
}
