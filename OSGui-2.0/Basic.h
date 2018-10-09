/**
 * MyOS\Basic.h
 *
 * last modification time : 2016/3/18 16:37		by LyanQQ
 */

/**
 * Working log:
 * 2016/03/18 16:37		Created				by LyanQQ ;
 * 2016/03/18 16:59		1st time coding		by LyanQQ :
 * 		-Finish TimeStamp definition and its operating functions ;
 */

/**
 * Basic.h包含了所有基本的数据结构、常量和函数
 * 这些定义在操作系统的任何一个模块都有可能用到
 * 包括但不限于时间戳，字符串基本操作等
 */ 

#ifndef BASIC_H
#define BASIC_H

/**
 * 错误代码类型
 */
enum ErrorType {  NO_ENOUGH_DISK_SPACE
				, NO_ENOUGH_CHILD_NODE
				, PATH_NOT_EXIST
				, SAME_NAME
				, READ_WRITE_DISK_ERROR
				, SYSTEM_ERROR
				, ILLEGAL_ACCESS_SYSTEM_DISK
				, DISK_SPACE_ERROR
				, ILLEGAL_CHAR_IN_FILE_NAME
				, FILE_NAME_TOO_LONG
				, FILE_NAME_EMPTY
				, FILE_TOO_BIG
				, FILE_READ_ERROR
				, FILE_WRITE_ERROR
				, MEMORY_CONFIG_NOT_EXIST,		                /*内存配置文件不存在*/
				 MEMORY_CONFIG_DESC_TOO_LONG_OR_SHORT,          /*内存配置信息格式错误（描述信息过长或过短)*/
				 MEMORY_CONFIG_MEMORYSIZE_ERROR,                /*内存配置信息内存大小格式错误*/
				 MEMORY_NOT_EXIST,                              /*内存不存在*/
				 MEMORYADDRESS_OUT_OF__BOUND,                   /*内存地址越界*/
				 VMEMORY_CONFIG_NOT_EXIST,                      /*虚拟内存配置文件不存在*/
				 VMEMORY_CONFIG_FORM_ERROR,                     /*虚拟内存配置文件格式错误*/
				 VMEMORY_APPLY_TOO_MUCH,                        /*申请虚拟内存帧数过多*/
				 VMEMORY_NOT_ENOUGH,                            /*虚拟内存不足*/
				 VMEMORY_FRAMENUM_MAP_SECTION_NOT_FOUND,        /*该虚拟帧号对应的扇区不存在*/
				 MEMORYMANAGEMENT_CONFIG_NOT_EXIST,             /*内存管理配置文件不存在*/
				 MEMORYMANAGEMENT_CONFIG_FORM_ERROR,            /*内存管理配置文件格式不正确*/
				 MEMORYMANAGEMENT_CONFIG_NOT_READ,              /*未读取内管理存配置信息*/
				 MEMORY_APPLY_TOO_MUCH,                         /*申请内存帧数过多*/
				 VM_FRAMENUM_NOT_AVAILABLE,                     /*虚拟帧无效*/
				 FRAME_QUEUE_INI_ERROR,                         /*帧队列初始化失败*/
				 FRAME_NOT_AVAILABLE,                           /*内存帧无效*/
				 MEMORYLOGPATH_NOT_EXIST
				, NOT_RUNNABLE_FILE
				, CAN_NOT_RUN_FILE
				, UNKNOW_ERROR
				, UNAVAILABLE_PROCESS
				, PID_NOT_MAP_FRAME
				, ADDRESS_OVERFLOW
				, IO_REQUEST_INVALID
				, FILE_ALREADY_OPEN
				, FOLDER_ALREADY_OPEN
				, CAN_NOT_MODIFY_READ_ONLY_FILE
                , CAN_NOT_DELETE_PROTECT_FOLDER
                , ILLEGAL_ADDRESS_VISIT
                , DEST_FOLDER_IS_SUB_FOLDER
                , PERMISSION_IS_RESTRICTED
                , ACCOUNT_INI_INVALID
                , ACCOUNT_PASSWORD_WRONG
                , NO_ENOUGH_HEAP_SPACE
                , ILLEGAL_PARAMETER
                , DEFRAGMENT_FAILED} ;

/**
 * 时间戳结构
 */
struct TimeStamp {
	int year ;		/* 年，4位数, 2B */
	int month ;		/* 月，2位数, 1B */
	int day ;		/* 日，2位数, 1B */
	int hour ;		/* 时，2位数, 1B */
	int min ;		/* 分，2位数, 1B */
	int sec ;		/* 秒，2位数, 1B */
} ;

void getSystemTime(TimeStamp * __ts);
void showTime(char * __buf, TimeStamp __ts);
char * logTime(char * __buf, TimeStamp __ts);
int copyString(char * aim, const char * from, int howMuch);
void uppercase(char * __str);
void lowercase(char * __str);
int strToHex(char *ch, char *hex);
int strToHex(char *ch, char *hex, int length);

/* 错误处理 */
extern ErrorType errorCode;
void inline setError(ErrorType __e)
{
	errorCode = __e ;
}
void printError();

#endif
