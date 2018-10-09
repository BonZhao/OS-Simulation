/**
 * MyOS\IOUnit.h
 *
 * Copyright (c) 2016 LyanQQ
 */

/**
 * Working log:
 * 2016/04/03 21:55		Created				by LyanQQ ;
 */

#ifndef IOUNIT_H
#define IOUNIT_H
 
#include "ProcessManagement.h"
 
#define MAX_IO_QUEUE 50		/* 最大可处理进程数 */

struct IOReqInfo {
	ProcessID pid ;
	int duration ;
} ;

bool newIO(ProcessID __pid, int __duration);
bool initIOUnit();
void destroyIOUnit();

#endif
