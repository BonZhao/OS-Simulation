/**
 * MyOS\System.cpp
 *
 * Copyright (c) 2016 LyanQQ
 */

#include "System.h"
#include "FileSystem.h"
#include "SystemApp.h"
#include "Basic.h"
#include "MemoryManagement.h"
#include "ProcessManagement.h"
#include "ExeUnit.h"
#include "IOUnit.h"
#include "Account.h"
#include "WorkLog.h"
#include <stdio.h>
#include <Windows.h>
#include <QDebug>

TimeStamp systemStartTime ;

HANDLE systemClosing ;

DWORD WINAPI ExeUnitThread(LPVOID pPararneter);
//DWORD WINAPI SystemThread(LPVOID pPararneter);

void OSRunning(void);

bool initSystem()
{
	getSystemTime(&systemStartTime);
	
    qDebug() << "loading..." ;
	
	if (!initWorkLog()) {
        qDebug() << "worklog failed" ;
        return false;
    }

	if (!initFileSystem()) {
        qDebug() << "file system failed" ;
        return false;
    }

	if (!initializeMemory()) {
        qDebug() << "memory failed" ;
        return false;
    }

	if (!initIOUnit()) {
        qDebug() << "IO unit failed" ;
        return false;
    }

    initProcessManagement();

    if (!initAccount()) {
        qDebug() << "account failed" ;
        return false ;
    }

    qDebug() << "completed" ;

    OSRunning();

    return true ;
}

void destroySystem()
{
	printf("���ڹػ�...\n");

	destroyProcessManagement();
	destroyIOUnit();
	destroyWorkLog();
    shutDownExeUnit();
	
	printf("Bye\n");
}

void OSRunning(void)
{
	HANDLE EUHandle, systemAppHandle ;
	
	/* ϵͳ�ػ��� */
    systemClosing =  CreateSemaphore(NULL, 0, 1, NULL) ;
	if (!systemClosing) {
		puts("ִ�в�������ʧ�ܣ�������");
		return ;
    }
    //qDebug()<<"here"  ;
	
	/* EU��Ԫ�߳� */
	EUHandle = CreateThread (NULL,0,ExeUnitThread,NULL,0,NULL);
	if (EUHandle) {
		CloseHandle (EUHandle);
	}
	else {
		puts("ִ�в�������ʧ�ܣ�������");
		return ;
	}
    //qDebug()<<"here"  ;
	
    //for (int i=0; i<=199 && newProcess("C:\\example\\defragment.run"); i++) ;
    //for (int i=0; i<=199 && newProcess("C:\\example\\illegal.run"); i++) ;
   for (int i=0; i<=999 && newProcess("C:\\example\\accumulation.run"); i++) ;
    //for (int i=0; i<=19 && newProcess("C:\\example\\normal.run"); i++) ;
    //for (int i=0; i<=99 && newProcess("C:\\example\\IOdemo.run"); i++) ;

    //qDebug()<<"here"  ;
	/* ϵͳӦ���߳� */
/*	systemAppHandle = CreateThread (NULL,0,SystemThread,NULL,0,NULL);
	if (EUHandle) {
		CloseHandle (systemAppHandle);
	}
	else {
		puts("ϵͳӦ�ü���ʧ�ܣ�������");
		return ;
    }*/
	
    //WaitForSingleObject(systemClosing, INFINITE);
	
	CloseHandle(systemClosing);
}

DWORD WINAPI ExeUnitThread(LPVOID pPararneter)
{	
	startExeUnit();
	
	return 0;
}
/*
DWORD WINAPI SystemThread(LPVOID pPararneter)
{	
	desktop();
	
	return 0;
}*/
