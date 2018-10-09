/**
 * MyOS\WorkLog.h
 *
 * Copyright (c) 2016 LyanQQ
 */

/**
 * Working log:
 * 2016/05/10 10:55		Created				by LyanQQ ;
 */

#ifndef WORKLOG_H
#define WORKLOG_H


#define LOG_NUMBER 12			/* 工作日志数量 */

enum LogType {
                FREE_SPACE_LOG ,
                FILE_API_LOG ,
                FILE_STREAM_LOG ,
                DISK_DRIVER_LOG ,
                IO_UNIT_LOG ,
                EU_LOG ,
                PCB_QUEUE_LOG ,
                MEMORY_DRIVER_LOG ,
                MEMORY_LOG ,
                VM_LOG ,
                CONSOLE_LOG ,
                HEAP_LOG

                /* 在此处添加工作日志类型 */
            } ;

bool initWorkLog();
void destroyWorkLog();
int __cdecl logPrintf(LogType __lt, const char *fmt, ...);

#endif
