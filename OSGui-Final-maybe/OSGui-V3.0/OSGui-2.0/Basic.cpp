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
ErrorType errorCode;								/* ������� */

const char * errorDesc[200] = {"���̿ռ䲻��"			/* �������� */
							, "�ļ����ļ��������Ѵ�����"
							, "������ļ�·��"
							, "�ļ������Ѱ���ͬ���ļ����ļ���"
							, "��д���̳���"
							, "����ϵͳ�ڲ�����ͨ���ǲ����˸�����"
							, "�Ƿ����ʻ��޸�ϵͳԤ���Ĵ�������"
							, "���̿ռ����"
							, "�ļ����а����Ƿ��ַ�"
							, "�ļ�������"
							, "�ļ�������Ϊ��"
							, "�����ļ�����"
							, "�ļ���ȡ����"
							, "�ļ�д�ش���"
							, "�ڴ������ļ�������"
							, "�ڴ�������Ϣ��ʽ����������Ϣ���������)"
							, "�ڴ�������Ϣ�ڴ��С��ʽ����"
							, "�ڴ治����"
							, "�ڴ��ַԽ��"
							, "�����ڴ������ļ�������"
							, "�����ڴ������ļ���ʽ����"
							, "���������ڴ�֡������"
							, "�����ڴ治��"
							, "������֡�Ŷ�Ӧ������������"
							, "�ڴ���������ļ�������"
							, "�ڴ���������ļ���ʽ����ȷ"
							, "δ��ȡ�ڹ����������Ϣ"
							, "�����ڴ�֡������"
							, "����֡��Ч"
							, "֡���г�ʼ��ʧ��"
							, "�ڴ�֡��Ч"
							, "�ڴ�ҳ������־�ļ�������"
							, "���ǿ�ִ�е��ļ�"
							, "ִ���ļ�����"
							, "δ֪����"
							, "��Ч�Ľ���"
							, "�����ڴ�֡��ƥ��"
							, "��ַ����Խ��"
							, "IO������Ч"
							, "�ļ��������������д�"
							, "�ļ��������ļ��������������д�"
							, "�����޸�ֻ���ļ�������"
                            , "����ɾ�����ƶ����������ļ���"
                            , "��Ч�ĵ�ַ�ռ����"
                            , "Ŀ��·����Դ·������Ŀ¼"
                            , "Ȩ�ޱ����ƣ��޷����д˲���"
                            , "�˻�ģ��ʧЧ���볢������ϵͳ�޸�"
                            , "�˺Ż��������"
                            , "�ý��̶����ռ��Ѳ���"
                            , "�Ƿ��Ĳ�������"
                            , "������Ƭ����ʧ��"} ;
							
const char * configPath = "PC\\Config\\" ;			/* ����ϵͳ�����ļ�·�� */

void getSystemTime(TimeStamp * __ts)
{
	/**
	 * ��ȡ��ǰϵͳʱ�䣬���浽TimeStamp��
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
	 * ��ʱ���ת���ɿ�����ʾ���ַ���
	 * ������__buf��ʼ���ڴ�ռ��У�__bufӦ������20�ֽڵĿռ�
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
	 * ����from��ʼ��howMuch�����ַ������Ƶ�aim��
	 * �����ַ������ȣ�û��'\0'�ַ��򷵻�-1��
	 * aim��from���뱣֤�㹻�ռ�
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
    result = (char)(value + 48); //48Ϊascii����ġ�0���ַ�����ֵ
  }
  else if(value >= 10 && value <= 15){
    result = (char)(value - 10 + 65); //��ȥ10���ҳ�����16���Ƶ�ƫ������65Ϊascii��'A'���ַ�����ֵ
    }
  return result;

}

/*��һ���ַ���ת����16����*/
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
        *hex++ = valueToHexCh(high); //��д���ֽ�
        *hex++ = valueToHexCh(low); //���д���ֽ�
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
        *hex = valueToHexCh(high); //��д���ֽ�
        hex++;
        *hex = valueToHexCh(low); //���д���ֽ�
        hex++;
        //qDebug()<<*hex;
        ch++;
        i++;
    }
    *hex = '\0';
    return 0;
}
