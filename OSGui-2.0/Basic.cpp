/**
 * MyOS\Basic.h
 *
 * Copyright (c) 2016 LyanQQ
 */

/**
 * Working log:
 * 2016/03/18 16:37		Created				by LyanQQ ;
 * 2016/03/18 16:59		1st time coding		by LyanQQ :
 * 		-Finish TimeStamp definition and its operating functions.
 * 2016/03/23 21:11		2nd time coding		by LyanQQ :
 * 		-Add error handling mechanism.
 */ 

#include "Basic.h"
#include <time.h>
#include <stdio.h>
#include <QDebug>
ErrorType errorCode;								/* 错误代码 */

const char * errorDesc[200] = {"磁盘空间不足"			/* 错误描述 */
							, "文件和文件夹数量已达上限"
							, "错误的文件路径"
							, "文件夹中已包含同名文件或文件夹"
							, "读写磁盘出错"
							, "操作系统内部错误（通常是产生了负数）"
							, "非法访问或修改系统预留的磁盘区域"
							, "磁盘空间出错"
							, "文件名中包含非法字符"
							, "文件名过长"
							, "文件名不能为空"
							, "单个文件过大"
							, "文件读取错误"
							, "文件写回错误"
							, "内存配置文件不存在"
							, "内存配置信息格式错误（描述信息过长或过短)"
							, "内存配置信息内存大小格式错误"
							, "内存不存在"
							, "内存地址越界"
							, "虚拟内存配置文件不存在"
							, "虚拟内存配置文件格式错误"
							, "申请虚拟内存帧数过多"
							, "虚拟内存不足"
							, "该虚拟帧号对应的扇区不存在"
							, "内存管理配置文件不存在"
							, "内存管理配置文件格式不正确"
							, "未读取内管理存配置信息"
							, "申请内存帧数过多"
							, "虚拟帧无效"
							, "帧队列初始化失败"
							, "内存帧无效"
							, "内存页错误日志文件不存在"
							, "不是可执行的文件"
							, "执行文件错误"
							, "未知错误"
							, "无效的进程"
							, "进程内存帧不匹配"
							, "地址访问越界"
							, "IO请求无效"
							, "文件已在其他进程中打开"
							, "文件夹中有文件已在其他进程中打开"
							, "不能修改只读文件的内容"
                            , "不能删除、移动被保护的文件夹"
                            , "无效的地址空间访问"
                            , "目的路径是源路径的子目录"
                            , "权限被限制，无法进行此操作"
                            , "账户模块失效，请尝试重启系统修复"
                            , "账号或密码错误"
                            , "该进程堆区空间已不足"
                            , "非法的操作参数"
                            , "堆区碎片整理失败"} ;
							
const char * configPath = "PC\\Config\\" ;			/* 操作系统配置文件路径 */

void getSystemTime(TimeStamp * __ts)
{
	/**
	 * 获取当前系统时间，保存到TimeStamp中
	 */
	
	time_t st__ ;
	struct tm * ti__ ;
	
	time(&st__) ;
	ti__ = localtime(&st__) ;
	
	__ts->year = ti__->tm_year + 1900 ;
	__ts->month = ti__->tm_mon + 1 ;
	__ts->day = ti__->tm_mday ;
	__ts->hour = ti__->tm_hour ;
	__ts->min = ti__->tm_min ;
	__ts->sec = ti__->tm_sec ;
}

void showTime(char * __buf, TimeStamp __ts)
{
	/**
	 * 将时间戳转换成可以显示的字符串
	 * 保存在__buf开始的内存空间中，__buf应至少有20字节的空间
	 */
	sprintf(__buf, "%d/%d/%d %02d:%02d:%02d", __ts.year, __ts.month, __ts.day, __ts.hour, __ts.min, __ts.sec) ;
}

char * logTime(char * __buf, TimeStamp __ts)
{
	sprintf(__buf, "%d_%d_%d-%02d_%02d_%02d", __ts.year, __ts.month, __ts.day, __ts.hour, __ts.min, __ts.sec) ;
	
	return __buf ;
}

int copyString(char * aim, const char * from, int howMuch)
{
	/**
	 * 将从from开始的howMuch长的字符串复制到aim中
	 * 返回字符串长度（没有'\0'字符则返回-1）
	 * aim和from必须保证足够空间
	 */
	
	int i, where = -1 ;
	
	for (i = howMuch - 1; i >= 0; i--) {
		if (from[i] == '\0') {
			where = i ;
		}
		
		aim[i] = from[i] ;
	}
	
	return where ;
}

void uppercase(char * __str)
{
	int i = 0 ;
	
	while (__str[i]!='\0') {
		if (__str[i] >= 97 && __str[i] <= 122) {
			__str[i] -= 32 ;
		}
		
		i++ ;
	}
}

void lowercase(char * __str)
{
	int i = 0 ;
	
	while (__str[i]!='\0') {
		if (__str[i] >= 65 && __str[i] <= 90) {
			__str[i] += 32 ;
		}
		
		i++ ;
	}
}

void printError()
{
	puts(errorDesc[errorCode]) ;
}

char valueToHexCh(const int value)

{

  char result = '\0';
  if(value >= 0 && value <= 9){
    result = (char)(value + 48); //48为ascii编码的‘0’字符编码值
  }
  else if(value >= 10 && value <= 15){
    result = (char)(value - 10 + 65); //减去10则找出其在16进制的偏移量，65为ascii的'A'的字符编码值
    }
  return result;

}

/*把一个字符串转换成16进制*/
int strToHex(char *ch, char *hex)

{

    int high,low;
    int tmp = 0;
    if(ch == NULL || hex == NULL){
        return -1;
    }
    while(*ch){
        tmp = (int)*ch;
        high = tmp >> 4;
        low = tmp & 15;
        *hex++ = valueToHexCh(high); //先写高字节
        *hex++ = valueToHexCh(low); //其次写低字节
        ch++;
    }
    *hex = '\0';
    return 0;
}
int strToHex(char *ch, char *hex,int length)

{

    int high,low;
    int tmp = 0;
    if(ch == NULL || hex == NULL){
        return -1;
    }
    int i = 0;
    while(i < length){
        tmp = (int)(unsigned char)*ch;
        high = tmp >> 4;
        low = tmp & 15;
        *hex = valueToHexCh(high); //先写高字节
        hex++;
        *hex = valueToHexCh(low); //其次写低字节
        hex++;
        //qDebug()<<*hex;
        ch++;
        i++;
    }
    *hex = '\0';
    return 0;
}
