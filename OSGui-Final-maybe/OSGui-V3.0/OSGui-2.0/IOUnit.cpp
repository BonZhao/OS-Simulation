/**
 * MyOS\IOUnit.cpp
 *
 * Copyright (c) 2016 LyanQQ
 */

/**
 * Working log:
 * 2016/04/03 21:55		Created				by LyanQQ ;
 * 2016/04/06 22:16		1st time coding		by Burning :
 *		-V1.0 completed
 */

#include "IOUnit.h"
#include "ProcessManagement.h"
#include "WorkLog.h"
#include <time.h>
#include <Windows.h>
#include <stdio.h>

DWORD WINAPI IOExecution(LPVOID pM);

HANDLE IOSemaphore ;
extern bool LOG_OUTPUT;

bool newIO(ProcessID __pid, int __duration)
{
	IOReqInfo * ior__ = (IOReqInfo *)malloc(sizeof(IOReqInfo)) ;
	//printf("new %d %d\n", __pid, __duration);
	ior__->pid = __pid ;
	ior__->duration = __duration ;
	
	HANDLE h ;
	h = CreateThread(NULL, 0, IOExecution, (LPVOID)ior__, 0, NULL);
	if (h) {
		if (LOG_OUTPUT) {
			logPrintf(IO_UNIT_LOG, "新的IO请求\t进程pid：%d\tIO时长：%d ms\n\n", __pid, __duration);
		}
		CloseHandle(h);
		return true ;
	}
	else {
		if (LOG_OUTPUT) {
			logPrintf(IO_UNIT_LOG, "新的IO请求\t进程pid：%d\tIO时长：%d ms\tIO请求失败\n\n", __pid, __duration);
		}
		return false ;
	}
}

DWORD WINAPI IOExecution(LPVOID pM)
{
	WaitForSingleObject(IOSemaphore, INFINITE) ;
	IOReqInfo * ior__ = (IOReqInfo *)pM ;
	
	if (LOG_OUTPUT) {
		logPrintf(IO_UNIT_LOG, "开始处理IO请求\t进程pid：%d\tIO时长：%d ms\n\n", ior__->pid, ior__->duration);
	}
	//printf("start %d\n", ior__->pid);
	Sleep(ior__->duration) ;
	//printf("end %d\n", ior__->pid);
	if (LOG_OUTPUT) {
		logPrintf(IO_UNIT_LOG, "IO请求处理完成\t进程pid：%d\tIO时长：%d ms\n\n", ior__->pid, ior__->duration);
	}
	
	IOFinish(ior__->pid) ;
	free(ior__) ;
	
	ReleaseSemaphore(IOSemaphore, 1, NULL) ;
	return 0 ;
}

bool initIOUnit()
{
    IOSemaphore = CreateSemaphore(NULL, MAX_IO_QUEUE, MAX_IO_QUEUE, NULL);
	if (IOSemaphore) {
		return true ;
	}
	else {
		return false ;
	}
}

void destroyIOUnit()
{
	CloseHandle(IOSemaphore);
}
