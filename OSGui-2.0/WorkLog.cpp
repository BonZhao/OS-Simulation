/**
 * MyOS\IOUnit.h
 *
 * Copyright (c) 2016 LyanQQ
 */

/**
 * Working log:
 * 2016/05/10 10:55		Created				by LyanQQ ;
 * 2016/05/10 10:55		1st time coding		by LyanQQ ;
 *		-v1.0 complete
 */

#include "WorkLog.h"
#include "Basic.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <Windows.h>

HANDLE * logSemaphore = NULL ;
bool LOG_OUTPUT = true;

const char * logPath[LOG_NUMBER] = {

    "log\\free space\\" ,
    "log\\file system api\\" ,
    "log\\file stream\\" ,
    "log\\disk driver\\" ,
    "log\\IO unit\\" ,
    "log\\exe unit\\" ,
    "log\\pcb queue\\" ,
    "log\\memory driver\\" ,
    "log\\memory\\" ,
    "log\\virtual memory\\" ,
    "log\\console output\\" ,
    "log\\heap\\"
    /* 在此处添加工作日志的路径，与LogType对应 */

};

const bool logSwitch[LOG_NUMBER] = {

    true, 		/* 空闲空间 */
    true, 		/* 文件系统API */
    true, 		/* 文件流 */
    true, 		/* 磁盘驱动 */
    true, 		/* IO部件 */
    true, 		/* 执行部件 */
    true, 		/* 进程队列 */
    true, 		/* 内存驱动 */
    true, 		/* 内存 */
    true, 		/* 虚存 */
    true,		/* 控制台输出 */
    true		/* 堆 */

} ;

extern TimeStamp systemStartTime ;			/* 系统启动时间 */

bool initWorkLog()
{
    logSemaphore = (HANDLE *)malloc(sizeof(HANDLE) * LOG_NUMBER) ;

    for (int i=0; i<=LOG_NUMBER-1; i++) {
        logSemaphore[i] = CreateSemaphore(NULL, 1, 1, NULL) ;

        if (logSemaphore[i]) {
            ;
        }
        else {
            return false ;
        }
    }

    return true ;
}

void destroyWorkLog()
{
    for (int i=0; i<=LOG_NUMBER-1; i++) {
        CloseHandle(logSemaphore[i]);
    }

    free(logSemaphore);
}

int __cdecl logPrintf(LogType __lt, const char *fmt, ...)
{
    if (!(LOG_OUTPUT && logSwitch[__lt])) {
        return 0 ;
    }

    FILE * file_out ;
    char sys_time[40] ;
    char path__[255] ;

    WaitForSingleObject(logSemaphore[__lt], INFINITE) ;

    sprintf(path__, "%s%s.txt", logPath[__lt], logTime(sys_time, systemStartTime));

    file_out = fopen(path__, "a");
    if (file_out == NULL) {
        ReleaseSemaphore(logSemaphore[__lt], 1, NULL);
        return 0 ;
    }

    char printf_buf[1024];
    va_list args;
    int printed, i;

    va_start(args, fmt);
    printed = vsprintf(printf_buf, fmt, args);
    va_end(args);

    fprintf(file_out, "系统运行时间： %ld ms\n", clock());
    fprintf(file_out, printf_buf) ;
    fclose(file_out);

    ReleaseSemaphore(logSemaphore[__lt], 1, NULL);

    return printed ;
}
