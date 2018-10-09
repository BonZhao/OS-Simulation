/**
 * MyOS/DiskDriver.c
 * 
 * Copyright (c) 2016 LiNanQinQin
 */

/**
 * DiskDriver.c�����˴������������ṩ�����в������̻��õ��ĺ���
 * ��Щ����ģ����Ӳ���ӿڣ�������ʽ������ȡ/������������
 */

#include "DiskDriver.h"
#include "WorkLog.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <QDebug>
extern bool LOG_OUTPUT;
/**
 * ����������Ϣ�����������������������
 * ����������������С
 */
char * diskDescription = (char *)malloc(sizeof(char) * 200);   	/*   ��������   */
int maxDiskSize = 0 ;											/* ����������� */
int sectionNumber = 0 ;											/*   ��������   */
int sectionSize = 0 ;											/*   ������С   */

/* ����λ�� */
const char * diskPath = "PC\\Disk\\" ;

HANDLE diskMutex = NULL ;

bool readDiskConfig()
{
	/**
	 * ��ȡ���̵�������Ϣ���˲���Ӧ����
	 * ����ϵͳ����ʱ��BIOS�������
	 */
	
	/* �����ļ�·�� */
	char * configPath = (char *)malloc(sizeof(char) * 38) ;
	sprintf(configPath, "%sConfig.txt", diskPath);
	
	/* �������ļ���ʧ���򷵻�false */
	FILE * ifp = fopen(configPath, "r");
	if (ifp == NULL) {
		return false ;
	}
	
	/* ��˳���ȡ���ݣ�����⵽�����������ļ���ʽ�򷵻�false */
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
	
	/* ��ȡ�ɹ� */
	return true ;
}

bool saveDiskConfig()
{
	/**
	 * ������̵�������Ϣ���˲���ֻ���ڴ�������
	 * ��Ϣ���޸�ʱ���ã�������̸�ʽ��ʱ
	 * ����֮��������޸������ļ��е���Ϣ
	 */
	 
	/* �����ļ�·�� */
	char * configPath = (char *)malloc(sizeof(char) * 38) ;
	sprintf(configPath, "%sConfig.txt", diskPath);
	
	/* �������ļ���ʧ���򷵻�false */
	FILE * ofp = fopen(configPath, "w");
	if (ofp == NULL) {
		return false ;
	}
	
	/* ��˳��д������ */
	fprintf(ofp, "%s%d\n%d\n%d\n", diskDescription, maxDiskSize, sectionNumber, sectionSize) ;
	
	fclose(ofp);
	free(configPath);
	
	/* д��ɹ� */
	return true ;
}

int formatting(int __sectionSize)
{
	/**
	 * �����̸�ʽ���������������
	 * sectionSizeΪ��ʽ�������ɵ�������С
	 * ����ֵΪ��ʽ�����������������������ʽ��ʧ����
	 * ����0��������С������Ҫ�󣩻�ֵ��·������
	 */

	/* sectionSize <= 0 ���ߴ��ڴ����������޷���ʽ��������0 */
	if (__sectionSize <= 0 || __sectionSize > maxDiskSize) {
		return 0 ;
	}

	/* �������� */
	int __sectionNumber = 0 ;

	/* ��������������ȡ�� */
	__sectionNumber = maxDiskSize / __sectionSize ;

	/* ���������ļ� */
	FILE * ofp ;
	int i = 0, j = 0;
	unsigned char data = 255 ;
	char * sectionPath = (char *)malloc(sizeof(char) * 38);

	for (i=0; i <= __sectionNumber - 1; i++) {
		/* ���������ļ�·��������PC\Memory\123.txt����123�������� */
		sprintf(sectionPath, "%s%d.txt", diskPath, i) ;
	
		ofp = fopen(sectionPath, "wb") ;
		/* �ļ�·�����󣬷���-1 */
		if (ofp == NULL) {
			return -1 ;
		}
		
		/* д�����ļ������ݳ�ʼ��Ϊȫ1 */
		for (j=0; j <= __sectionSize - 1; j++) {
			fwrite(&data, sizeof(data), 1, ofp) ;
		}
		
		fclose(ofp) ;
	}
	
	free(sectionPath) ;
	
	/* Ϊ�˴θ�ʽ�������µĴ���λͼ */
	/* if (!newBitVector(__sectionNumber)) {
		return -1 ;
	} */
	
	/* ��ʽ���ɹ����޸Ĳ��������������Ϣ */
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
	 * ��whichSection�����������ݶ�����__section��ʼ���ڴ�������
	 * __section���뱣֤�㹻���Ŀռ�
	 * ����Ϊ__section���볤��ΪsectionSize���ڴ�ռ�
	 */
	
	/* whichSectionָ���������Ų����ڣ��򷵻�false */
	if (whichSection < 0 || whichSection > sectionNumber) {
		
		if (LOG_OUTPUT) {
			logPrintf(DISK_DRIVER_LOG, "��������ȡ����\t�����ţ�%d\t��ȡʧ�ܣ�������Խ��\n\n", whichSection) ;
		}
		
		return false ;
	}
	
	/* ��������·�� */
	char * sectionPath = (char *)malloc(sizeof(char) * 38);
	sprintf(sectionPath, "%s%d.txt", diskPath, whichSection) ;
	
	/* ������̲����� */
	WaitForSingleObject(diskMutex, INFINITE) ;
	
	/* �������ļ� */
	FILE * ifp = fopen(sectionPath, "rb");
	if (ifp == NULL) {
		
		if (LOG_OUTPUT) {
			logPrintf(DISK_DRIVER_LOG, "��������ȡ����\t�����ţ�%d\t��ȡʧ�ܣ������ļ�ʧЧ\n\n", whichSection) ;
		}
		
		ReleaseMutex(diskMutex) ;
		return false ;
	}
	
	/* ��ȡ�������� */
	fread(__section, sizeof(char), sectionSize, ifp);
	if (feof(ifp)) {
		
		if (LOG_OUTPUT) {
			logPrintf(DISK_DRIVER_LOG, "��������ȡ����\t�����ţ�%d\t��ȡʧ�ܣ�������\n\n", whichSection) ;
		}
		
		ReleaseMutex(diskMutex) ;
		return false ;
	}
	
	fclose(ifp);
	free(sectionPath);
	
	ReleaseMutex(diskMutex) ;
	
	if (LOG_OUTPUT) {
		logPrintf(DISK_DRIVER_LOG, "��������ȡ����\t�����ţ�%d\t��ȡ�ɹ�\n\n", whichSection) ;
	}
	
	return true ;
}

bool saveSection(char * __section, int whichSection)
{
	/**
	 * ����__section��ʼ��sectionSize��С�����ݱ��浽whichSection������
	 * __section���뱣֤�㹻���Ŀռ䣬���򷵻�false
	 * ����Ϊ__section���볤��ΪsectionSize���ڴ�ռ�
	 */
	
	/* whichSectionָ���������Ų����ڣ��򷵻�false */
	if (whichSection < 0 || whichSection > sectionNumber) {
		
		if (LOG_OUTPUT) {
			logPrintf(DISK_DRIVER_LOG, "������д������\t�����ţ�%d\tд��ʧ�ܣ�������Խ��\n\n", whichSection) ;
		}
		
		return false ;
	}
	
	/* ��������·�� */
	char * sectionPath = (char *)malloc(sizeof(char) * 38);
	sprintf(sectionPath, "%s%d.txt", diskPath, whichSection) ;
	
	/* ������̲����� */
	WaitForSingleObject(diskMutex, INFINITE) ;
	
	/* �������ļ� */
	FILE * ofp = fopen(sectionPath, "wb");
	if (ofp == NULL) {
		
		if (LOG_OUTPUT) {
			logPrintf(DISK_DRIVER_LOG, "������д������\t�����ţ�%d\tд��ʧ�ܣ������ļ�ʧЧ\n\n", whichSection) ;
		}
		
		ReleaseMutex(diskMutex) ;
		return false ;
	}
	
	/* ������������ */
	fwrite(__section, sizeof(char), sectionSize, ofp);
	
	fclose(ofp);
	free(sectionPath);
	
	ReleaseMutex(diskMutex) ;
	
	if (LOG_OUTPUT) {
		logPrintf(DISK_DRIVER_LOG, "������д������\t�����ţ�%d\tд��ɹ�\n\n", whichSection) ;
	}
	
	return true ;
}
