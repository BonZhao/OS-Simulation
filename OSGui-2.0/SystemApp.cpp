/**
 * MyOS\SystemApp.cpp
 *
 * Copyright (c) 2016 LyanQQ
 */

/**
 * Working log:
 * 2016/03/31 20:46		Created				by LyanQQ ;
 * 2016/03/31 22:28		1st time coding		by LyanQQ :
 * 		-Add FileExplorer App(simplified edition).
 * 2016/04/04 18:50		2nd time coding		by CJS&CY :
 * 		-FileExplorer 2.0
 */

#include "ProcessManagement.h"
#include "SystemApp.h"
#include "FileSystem.h"
#include "DiskDriver.h"
#include "Basic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h> 
#include <windows.h>
#include <time.h>


DWORD WINAPI taskManagerInputThread(LPVOID pPararneter);
DWORD WINAPI taskManagerDisplayThread(LPVOID pPararneter);

enum taskOption {CREATE_PROCESS, DELETE_PROCESS, SHOW_PROCESS, EXIT} ;
enum taskInput {LEFT, RIGTH, ENTER, ESC} ;
taskInput taskInputKey ;
const char * taskOptionDesc[4] = {"�½�����", "��������", "��ʾ��������", "�˳�"} ;
int taskOptionNumber = 4 ;

int displayModel = 0 ;
int displayFrequency = 1000 ;
bool taskManagerWorking = false ;
bool taskManagerRequest = false ;
taskOption taskManagerOption ;
int taskManagerPIDInput ;
//char * taskManagerPathInput ;

extern int sectionSize ;
extern char rootDirectoryLabel[] ;
extern HANDLE PCBQueueMutex ;

/* 
void back(char * __path)
{
	int len__ = strlen(__path);
	int i = len__-1;
	
	if (__path[i] == '\\') {
		i--;
	}
	
	for (; i>=0; i--) {
		if (__path[i] == '\\') {
			__path[i] = '\0' ;
			return ;
		}
	}
}

void showFileIndex(FileIndex __fi)
{
	char time[38] ;
	
	printf("�ļ�����%s\n", __fi.name) ;
	showTime(time, __fi.createTime);
	printf("����ʱ�䣺%s\n", time);
	showTime(time, __fi.lastModifyTime);
	printf("�޸�ʱ�䣺%s\n", time);
	showTime(time, __fi.lastOpenTime);
	printf("��ʱ�䣺%s\n", time);
	printf("�ļ����ȣ�%d\n���ݿ�������%d\n���ڵ㣺%d\n", __fi.length, __fi.dataSectionNumber, __fi.fatherNode);
	printf("���ݿ������ţ�") ;
	for (int i=0; i<=__fi.dataSectionNumber-1; i++) {
		printf("%d; ", __fi.dataNode[i]);
	}
	putchar('\n');
}

void fileExplorer()
{
	char path__[200] = "C:" ;
	char div__[2] = "\\" ;
	char name__[38] ;
	int secNum__ ;
	char * sec__ = (char *)malloc(sizeof(char) * 1024) ;
	FolderIndex fo__ ;
	
	initFolderIndex(&fo__) ;
	
	while (1) {
		printf("��ǰ·����%s\n", path__) ;
		
		if (getIndexSection(sec__, &secNum__, path__)) {
			if (sec__[0] == FILE_TYPE) {
				FileIndex fi__ ;
				
				initFileIndex(&fi__) ;
				extractFileIndex(sec__, &fi__) ;
				
				puts("����һ���ļ�����Ϣ���£�");
				
				showFileIndex(fi__);
				
				destroyFileIndex(&fi__);
			}
			else {
				extractFolderIndex(sec__, &fo__) ;
				
				if (fo__.childNodeNumber == 0) {
					puts("���ļ���Ϊ��");
				}
				else {
					for (int i=0; i<=fo__.childNodeNumber-1; i++) {
						readSection(sec__, fo__.childNode[i]);
						
						printf("%s\t\t", &sec__[1]);
						if (sec__[0] == FILE_TYPE) {
							char n__[38], ex__[38] ;
							
							extractFileName(n__, ex__, &sec__[1]);
							printf("%s�ļ�\t\t", ex__);
						}
						else {
							printf("�ļ���\t\t") ;
						}
						
						printf("�����ţ�%d\n", fo__.childNode[i]) ;
					}
					
					printf("����%d������\n", fo__.childNodeNumber) ;
				}
			}
		}
		else {
			puts("·������") ;
		}
		
		printf("��������һ��·��:");
		scanf("%s", name__);
		if (strcmp(name__, "b")==0) {
			back(path__);
		}
		else {
			strcat(path__, div__);
			strcat(path__, name__);
		}
		system("cls");
	}
	
	free(sec__);
} */
bool getAttr(const char * __fileName, char * __result);
bool cutPath(char * __path);
bool openFileDisplay(const char * __path);
void fileListInit(char * __display,const int * __arrow,int * __childNodeNumber,char * __currentPath,FolderIndex * __foi);
void opeListInit(char * __display,int * __arrow);
void newFileFolderInit(char * __display,int * __arrow) ; 
void account(FolderIndex * __fo, int * __fileNum, int * __folderNum, long int * __size, long int * __space) ; 
void deleteOk(char * __display, int * __arrow) ;

//���������� 
void deleteOk(char * __display, int * __arrow)
{
	strcat(__display, "ȷ��ɾ�����ļ�ô��\n") ;
	
	int count = 0 ;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->") ;
	}	
	else
	{
		strcat(__display, "    ") ;
	}
	strcat(__display, "ȷ��") ;
	
	count = 1;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->") ;
	}	
	else
	{
		strcat(__display, "    ") ;
	}
	strcat(__display, "ȡ��") ;
}

//����
void account(FolderIndex * __fo, int * __fileNum, int * __folderNum, long int * __size, long int * __space)
{
	/*
	** ����һ���ļ��е������ӽڵ� 
	*/
	
	int num = __fo->childNodeNumber ;
	int  * child = (int *)malloc(sizeof(int) * __fo->childNodeNumber) ;
	child = __fo->childNode ; 
	
	int i = 0 ;
	char * sec = (char *)malloc(sizeof(char) * sectionSize) ;//��¼�����е����� 
	
	for (i = 0; i<= __fo->childNodeNumber-1; i++ )
	{	
		if (readSection(sec, child[i]))
		{
			if (sec[0] == FILE_TYPE)
			{
				FileIndex fi ;
				initFileIndex(&fi);
				extractFileIndex(sec, &fi) ;				
				(* __size) = (* __size) + fi.length ;
				(* __space) = (*__space) + (fi.dataSectionNumber + 1) * 1024 ;
				(* __fileNum) ++ ;
			}
			else
			{
				(* __folderNum) ++ ;
				FolderIndex fo ; 
				initFolderIndex(&fo);
				extractFolderIndex(sec, &fo) ;
				//���ϸ��ļ���������ռ�õ�һ������ 
				(* __space)=(* __space) + 1024 ;	
				account(&fo, __fileNum, __folderNum, __size, __space) ;
			}
		}
	}
	
	free(sec);
	free(child);
}

/*�޸����Ҽ�����ֲ˵�����bug��
**�����˿����ûس����ļ����У��Ĺ���
**�鿴�ļ��������п�����ʾռ�ÿռ���ļ��д�С 
*/ 

bool getAttr(const char * __fileName, char * __result,char * currentPath)
{
	/*
	** �鿴�ļ������ļ��е����� 
	**/
	 
	char * path = (char *)malloc(sizeof(char) * 1024) ;
	strcpy(path, currentPath);
	
	//filenameΪ��ʱ������������ѯ��Ŀ¼���� 
	if(strcmp(__fileName,""))
	{
		strcat(path, "\\") ;
		strcat(path, __fileName) ;	// �����ַ������õ������ļ����ļ��е�·�� 
	}
	
	
	char * sec = (char *)malloc(sizeof(char) * 1024) ;
	int * secNum = (int *)malloc(sizeof(int));
	
	if (!getIndexSection(sec, secNum, path))
	{
		setError(PATH_NOT_EXIST);
	}
	else
	{
		char * a = (char *)malloc(sizeof(char) * 38) ;
		
		if (sec[0] == FILE_TYPE)
		{
			FileIndex fi ;
			
			initFileIndex(&fi) ;
			extractFileIndex(sec, &fi) ;
			
			strcpy(__result, "�ļ�����:txt�ļ�") ;
			
			strcat(__result, "\nλ��	:") ;
			strcat(__result, currentPath) ;
			
			
			_itoa(fi.length, a, 10) ;//������ת��Ϊ�ַ��� 
			strcat(__result, "\n��С	:") ;
			strcat(__result, a) ;
			strcat(__result,"�ֽ�") ;
			
			_itoa((fi.dataSectionNumber + 1) * 1024, a, 10) ;
			strcat(__result, "\nռ�ÿռ�:") ;
			strcat(__result, a) ;
			strcat(__result,"�ֽ�") ;
			
			showTime(a, fi.createTime) ;
			strcat(__result, "\n����ʱ��:") ;
			strcat(__result, a) ;
			
			showTime(a, fi.lastModifyTime) ;
			strcat(__result, "\n�޸�ʱ��:") ;
			strcat(__result, a ) ;
			
			showTime(a, fi.lastOpenTime) ;
			strcat(__result, "\n����ʱ��:") ;
			strcat(__result, a) ;			
		}
		else
		{
			FolderIndex fo ;
			
			initFolderIndex(&fo) ;
			extractFolderIndex(sec, &fo) ;
			
			strcpy(__result, "�ļ�����:�ļ���") ;
			
			strcat(__result, "\nλ��	:") ;
			strcat(__result, currentPath) ;
			
			/* �����ļ��еĴ�С��ռ�ÿռ䡢������ļ��к��ļ��ĸ��� */
			
			int fin = 0, fon = 0 ;
			long int size = 0, space = 0 ;
			//�ݹ���� 
			account(&fo, &fin, &fon, &size, &space) ;
			
			strcat(__result,"\n����	:");
			_itoa(fin,a,10);
			strcat(__result,a);
			strcat(__result,"���ļ�,");
			_itoa(fon,a,10);
			strcat(__result,a);
			strcat(__result,"���ļ���");
			
			_itoa(size,a,10);
			strcat(__result, "\n��С    :") ;
			strcat(__result,a) ;
			strcat(__result,"�ֽ�") ;
			
			_itoa(space,a,10);
			strcat(__result, "\nռ�ÿռ�:  ") ;
			strcat(__result,a) ;
			strcat(__result,"�ֽ�") ;
						
			showTime(a, fo.createTime) ;
			strcat(__result, "\n����ʱ��:") ;
			strcat(__result, a) ;
		}
		
		free(a);		
	}
	
	free(path);
	free(sec);
	free(secNum);
	
	return true ;
}

//��������bug 

void explorer()
{
	/*
	**��Դ������ 
	*/ 

	//��ǰĿ¼ 
	char * currentPath = (char *)malloc(sizeof(char)*1024);
	strcpy(currentPath , rootDirectoryLabel);
	strcat(currentPath ,":");

	//�ļ��б�	
	char * display = (char *)malloc(sizeof(char) * 1024);
	strcpy(display,"") ;
	
	int * arrow = (int *)malloc(sizeof(int));
	* arrow = 0;
	
	
	//�ļ������б� 
	char * display1 = (char *)malloc(sizeof(char) * 1024);
	strcpy(display1,"") ;
	
	int * arrow1 = (int *)malloc(sizeof(int));
	* arrow1 = 0;
	
	
	//���������б� 
	char * display2 = (char *)malloc(sizeof(char) * 1024);
	strcpy(display2,"") ;
	
	int * arrow2 = (int *)malloc(sizeof(int));	
	* arrow2 = 0;
	
	
	//ȷ��ɾ������
	char * display3 = (char *)malloc(sizeof(char) * 1024) ;
	strcpy(display3, "") ;
	
	int * arrow3 = (int *)malloc(sizeof(int)) ;
	(* arrow3) = 0 ; 
	
	
	int * childNum = (int *)malloc(sizeof(int));
	char ope;

	FolderIndex foi;
	initFolderIndex(&foi);
	
	fileListInit(display,arrow,childNum,currentPath,&foi);
	puts(display);
	
	//��� 
	int space = 0; 			/* ������ļ������б���ʾʱ����Ӧ���¼� */
	int spaceFlag = 1;		/* ��ǿո�Ĳ��� */
	int opeFlag = 0;		/* ����ļ������б�Ĳ��� */
	int fileFlag = 1;		/* ����ļ��б� */ 
	int newFlag = 0;		/* ��Ǵ������� */ 
	int cutOrCopy = -1 ; 	/* ���ּ��л��Ǹ��ƣ�����0������1������Ϊ-1������ճ�� */ 
	int enterFlag = 0;		/* �س������Ƿ���Ч����Ч0����Ч1�� */
	int deleteFlag = 0 ;	/* ȷ��ɾ����� */ 
	
	int copySecNum = -1;
	
	char * cutSourPath = (char *)malloc(sizeof(char) * 1024) ;	/* ���е�Դ·�� */
	char * copySourPath = (char *)malloc(sizeof(char) * 1024) ;	/* ���Ƶ�Դ·�� */
	
	int escFlag = 0;//�˳���־ 
	int secNum = -1;
	char * name = (char *)malloc(sizeof(char) * LENGTH_NAME);
	char * sec = (char *)malloc(sizeof(char) * 1024);
								
	
	while(!escFlag)
	{
		ope = _getch();
		secNum = foi.childNode[(*arrow) - 3];
		char * root = (char *)malloc(sizeof(char) * LENGTH_NAME);	
				
		switch(ope)
		{
			//�����down 
			case 80:				
				if(fileFlag==1 && deleteFlag == 0 )
				{
					strcpy(display,"") ;
					system("cls");
					(* arrow)++;
				
					if((* arrow) >= (*childNum)+3)
					{
						(* arrow) = (* childNum)+3;
					}
				
					fileListInit(display,arrow,childNum,currentPath,&foi);
					puts(display);
				}
			break;
			
			//�����up 
			case 72:
				if(fileFlag==1 && deleteFlag == 0)
				{
					strcpy(display,"") ;
					system("cls");
					(* arrow)--;
				
					if((* arrow)<=0)
					{
						(* arrow) = 0;
					}
				
					fileListInit(display,arrow,childNum,currentPath,&foi);
					puts(display);
				}
			break;
			
			//�����left 
			case 75:
				if(opeFlag==1 )
				{
					if(newFlag == 0)
					{
						if(deleteFlag == 1)
			 			{	
			 				strcpy(display,"") ;
							strcpy(display1,"") ;
							system("cls");
							fileListInit(display,arrow,childNum,currentPath,&foi);
							puts(display);
							opeListInit(display1,arrow1);
							puts(display1);	
										 			
			 				strcpy(display3,"") ;

			 				(* arrow3)--;
			 		
							if((* arrow3)<=0)
							{
								(* arrow3) = 0;
							}
						
						
			 				deleteOk(display3, arrow3) ;
			 				puts(display3);
			 			}
			 			else
			 			{
							strcpy(display1,"") ;
							system("cls");
							(* arrow1)--;
						
							if((* arrow1) <= 0)
							{
								(* arrow1) = 0;
							}
						
							puts(display);
							opeListInit(display1, arrow1);
					 		puts(display1);
						}
					}
			 		else
			 		{
			 			strcpy(display2,"") ;
						system("cls");
			 			(* arrow2)--;
			 	
						if((* arrow2) <= 0)
						{
							(* arrow2) = 0;
						}
			 	
			 			puts(display);
			 			newFileFolderInit(display2, arrow2) ;
			 			puts(display2);
			 		
			 		}
			 	}
			break;
			
			//�����right
			case 77:
				if(opeFlag==1)
				{
					if(newFlag==0)
					{
						if(deleteFlag == 1)
			 			{
			 				strcpy(display,"") ;
							strcpy(display1,"") ;
							system("cls");
							fileListInit(display,arrow,childNum,currentPath,&foi);
							puts(display);
							opeListInit(display1,arrow1);
							puts(display1);	
									 			
			 				strcpy(display3,"") ;

			 				(* arrow3)++;
			 	
							if((* arrow3)>=1)
							{
								(* arrow3) = 1;
							}
						
						
			 				deleteOk(display3, arrow3) ;
			 				puts(display3);
			 			}
			 			else
			 			{
							strcpy(display1,"") ;
							system("cls");
							(* arrow1)++;
						
							if((* arrow1) >= 5)
							{
								(* arrow1) = 5;
							}
						
							puts(display);
							opeListInit(display1, arrow1);
					 		puts(display1);
					 	}
					}
				
			 		else		 			
					{
						strcpy(display2,"") ;
						system("cls");
						(* arrow2)++;
						
						if((* arrow2) >= 1)
						{
							(* arrow2) = 1;
						}
						
						puts(display);
						newFileFolderInit(display2, arrow2) ;
					 	puts(display2);
					}
				}
			break; 	
					
			//enter:ȷ�ϲ��� 
			case 13:
				spaceFlag = 1;
				fileFlag = 1;
				
				if(space == 1|| !((*arrow)== 0 || (*arrow)== 1 || (* arrow) ==2 || (*arrow) == *childNum + 3))
				{
					if(space == 1)
						space = 0;
					//�ڵ�ǰ·���´����ļ������ļ���
					if (newFlag == 1)
					{
						printf("���������֣�");
						char * name = (char *)malloc(sizeof(char) * LENGTH_NAME);
						gets(name);
						if ((* arrow2) == 0)
						{//�ļ� 
							if(newFile(name,currentPath))
							{
								system("cls");
								strcpy(display,"");
								fileListInit(display,arrow,childNum,currentPath,&foi);
								puts(display);
							}
							else
							{
								printError();
							}
						}
						else
						{
							if(newFolder(name,currentPath))
							{
								system("cls");
								strcpy(display,"");
								fileListInit(display,arrow,childNum,currentPath,&foi);
								puts(display);
							}
							else
							{
								printError();
							}
						}
						free (name) ;
					} 
					//�ļ������б� 
					else if(opeFlag==1 && deleteFlag == 0)
					{	
						opeFlag = 0;
						if(!readSection(sec, secNum))
						{
							printError();
						}
						else
						{	
							int l = copyString(name,&sec[FILE_OFFSET_NAME] , LENGTH_NAME);			
						
						
							char * rePath =(char *)malloc(sizeof(char) * 1024);
							strcpy(rePath,currentPath);
						
							char * reName = (char *)malloc(sizeof(char) * LENGTH_NAME) ;
							strcpy(reName, "") ;
						
							char * result = (char *)malloc(sizeof(char) * 1024) ;
							strcpy(result, "") ;
						
							switch(*arrow1)
							{
								//��
								case 0:
								
									if(sec[0] == FILE_TYPE)
									{
										//���ļ� 
										if(!openFileDisplay(currentPath))
										{
											printError();
										}
										else
										{
											strcpy(display,"");
											system("cls");
											fileListInit(display,arrow,childNum,currentPath,&foi);
											puts(display);		
										}
									}
									else
									{
										strcat(currentPath,"\\");
										strcat(currentPath,name);
										
										//���ļ���
										(* arrow) = 0; 
										fileFlag = 1;
										strcpy(display,"");
										system("cls");
										fileListInit(display,arrow,childNum,currentPath,&foi);
										puts(display);									
									} 							
								break;
						
								//���� 
								case 1:
									cutOrCopy = 0;
									/*��¼�����ļ���Դ·��*/
									strcpy(cutSourPath, currentPath) ; 
									strcat(cutSourPath,"\\");
									strcat(cutSourPath,name);
									copySecNum = secNum;
								
									strcpy(display,"");
									system("cls");
									(*arrow) = 0;
									fileListInit(display,arrow,childNum,currentPath,&foi);
									puts(display);
									
									printf("�Ѹ��Ƶ����а�~~\n") ;						
								break; 
							
								//����
								case 2:
									cutOrCopy = 1;
									/*��¼�����ļ���Դ·��*/
									strcpy(copySourPath, currentPath) ; 
									strcat(copySourPath,"\\");
									strcat(copySourPath,name); 
									copySecNum = secNum;
								
									strcpy(display,"");
									system("cls");
									(*arrow) = 0;
									fileListInit(display,arrow,childNum,currentPath,&foi);
									puts(display);
									
									printf("�Ѹ��Ƶ����а�~~\n") ;
								break; 
								
								//ɾ��
								case 3:	
								(*arrow3) = 0;
								opeFlag = 1;
								deleteFlag = 1;
							//	newFlag = 1;
								strcpy(display3,""); 
								deleteOk(display3, arrow3) ;
								puts(display3);
								break;
							
								//������
								case 4:
									printf("�������µ����֣�") ;
									gets(reName) ;
									
									strcat(rePath,"\\");
									strcat(rePath,name);
									if (renameFileFolder(reName, rePath))
									{
										strcpy(display,"");
										system("cls");
										fileListInit(display,arrow,childNum,currentPath,&foi);
										puts(display);
									}
									else
									{
										printError();
									}
								break;
							
								//�鿴����
								case 5:
									if (getAttr(name, result, currentPath))
									{
										strcpy(display,"");
										system("cls");
										fileListInit(display,arrow,childNum,currentPath,&foi);
										puts(display);
										printf("\n") ;
										puts(result) ;
									}
									else
									{
										printError();
									}
								break;							
							}
							free(rePath) ;
							free(reName) ;
							free(result) ;
						}	
					}
					//enterֱ�Ӵ��ļ�	
					else if((* arrow) != 0 && (* arrow)!= 1 && (* arrow)!= 2 &&deleteFlag == 0)
					{
						if(!readSection(sec, secNum))
						{
							printError();
						}
						else
						{
							int l = copyString(name,&sec[FILE_OFFSET_NAME] , LENGTH_NAME);
							if(sec[0] == FILE_TYPE)
							{
										//���ļ� 
								if(!openFileDisplay(currentPath))
								{
									printError();
								}
								else
								{
									strcpy(display,"");
									system("cls");
									fileListInit(display,arrow,childNum,currentPath,&foi);
									puts(display);		
								}
							}
							else
							{
								strcat(currentPath,"\\");
								strcat(currentPath,name);
								
								//���ļ���
								(* arrow) = 0; 
								fileFlag = 1;
								strcpy(display,"");
								system("cls");
								fileListInit(display,arrow,childNum,currentPath,&foi);
								puts(display);									
							} 	
								
						}
					}
					//ɾ��ȷ�ϲ��� 
					else if(deleteFlag == 1)
					{
						deleteFlag = 0;
						fileFlag = 1;
						opeFlag = 0;
						
						//ȷ��ɾ�� 
						if ((* arrow3)==0)
						{	
							char * dePath =(char *)malloc(sizeof(char) * 1024);
							strcpy(dePath,currentPath);
							deleteFlag =1;		
							strcat(dePath,"\\");
							strcat(dePath,name);
							if(!deleteFileFolder(dePath))
							{
								printError();
							}
							else
							{	
								strcpy(display,"");
								system("cls");
								(*arrow) = 0;
								fileListInit(display,arrow,childNum,currentPath,&foi);
								puts(display);
								puts(display3) ;	
							}
							free(dePath) ;
							fileFlag = 1;
							deleteFlag = 0;
						}
						//��ȡ������ʲôҲ���� 
						strcpy(display,"");
						system("cls");
						fileListInit(display,arrow,childNum,currentPath,&foi);
						puts(display);
					}
				}
				else if (space == 0 && (* arrow) == 2)
				{
					strcpy(root,rootDirectoryLabel);
					strcat(root,":");
						
					if(strcmp(currentPath,root))
					{
						if(cutPath(currentPath))
						{
							strcpy(display,"");
							system("cls"); 
							fileListInit(display,arrow,childNum,currentPath,&foi);
							puts(display);
						}
						else
						{
							printError();
						} 
					}
						
					fileFlag = 1;
				}
				else if((*arrow) == *childNum + 3)
				{
					escFlag = 1;
				}
				
			break;
						
			//space:�򿪲����б� 
			case 32:
				
				(*arrow1) = 0;
				strcpy(display1,"") ;
				strcpy(display2,"") ;
			
			
	 			if( (*arrow) != 1 && (*arrow) != 2)
				{
					fileFlag = 0;
				}
				else
				{
					fileFlag = 1;
				}
				
				if(spaceFlag)
				{
					/*����ͷ���ڴ�������һ��Ŀ¼ʱ������Ӧ�򿪲����б����Ϊ*/ 
					if ((*arrow)!=0 && (*arrow)!=1 && (*arrow)!=2)
					{
						spaceFlag = 0;
						newFlag = 0;
						system("cls");
						if(space==0)
						{
							space = 1;
							puts(display);
							opeListInit(display1,arrow1);
							puts(display1);
							opeFlag = 1; 
						}
					}
					else if((*arrow)==0)
					{//��Ӧ��������
					//	opeFlag = 0;
						newFlag = 1;
						system("cls"); 
						space = 1;
						puts(display);
						newFileFolderInit(display2, arrow2) ;
						puts(display2);
						opeFlag = 1; 
					}
					else if ((* arrow) == 1)
					{//ճ�� 					
						if(cutOrCopy != -1)
				    	{
							if(!readSection(sec, copySecNum))
							{
								printError();
							}
							else
							{	
								if (cutOrCopy == 0)
								{
									//����֮���ճ��
									if(moveFileFolder(currentPath, cutSourPath))
									{
										cutOrCopy = -1 ;
											
										system("cls");
										strcpy(display,"");
										fileListInit(display,arrow,childNum,currentPath,&foi);
										puts(display);
									}
									else
									{
										printError();
									}
								}
								else if(cutOrCopy == 1)
								{
									//����֮���ճ�� 
									if (sec[0] == FILE_TYPE)
									{
										if(copyFile(currentPath, copySourPath))
										{	
											system("cls");
											strcpy(display,"");
											fileListInit(display,arrow,childNum,currentPath,&foi);
											puts(display);
										}	
										else
										{
											printError();
										}
									}	
									else
									{
										if(copyFolder(currentPath, copySourPath))
										{	
											system("cls");
											strcpy(display,"");
											fileListInit(display,arrow,childNum,currentPath,&foi);
											puts(display);
										}
										else
										{
											printError();
										}
									}		
								} 
							}
						}
						else
						{
							space = 0;
							fileFlag = 1;
						}
					}				
					else if((*arrow)==2)
					{//��Ӧ�����ϲ�Ŀ¼����
						//�統ǰĿ¼�Ѿ��Ǹ�Ŀ¼������Ӧ������ 
						strcpy(root,rootDirectoryLabel);
						strcat(root,":");
						
						if(strcmp(currentPath,root))
						{
							if(cutPath(currentPath))
							{
								strcpy(display,"");
								system("cls"); 
								fileListInit(display,arrow,childNum,currentPath,&foi);
								puts(display);
							}
							else
							{
								printError();
							} 
						}
						
						fileFlag = 1;
					} 	
				}
			break;
			
			//esc���˳������б� 
			case 27:
				deleteFlag = 0;
				opeFlag = 0;
				spaceFlag = 1;
				fileFlag = 1;
				newFlag = 0;
				system("cls");
				
				if(space==1)
				{
					space = 0;
					opeFlag = 0;
				}
				
				puts(display);
		}
		free (root) ;		
	}
	free (currentPath) ;
	
	free (display) ;
	free (arrow) ;
	
	free (display1) ;
	free (arrow1) ;
	
	free (display2) ;
	free (arrow2) ;
	
	free (childNum) ;
	
	free (cutSourPath) ;
	free (copySourPath) ;
	
	free (name) ;
	free (sec) ;	
} 




void fileListInit(char * __display,const int * __arrow,int * __childNodeNumber,char * __currentPath,FolderIndex * __foi)
{
	/*�������Ϊ��Դ��������ʾ���ַ�����
	**�˴����г�ʼ����
	**��ɶԵ�ǰĿ¼�������ļ����ļ��е���ʾ 
	*/ 	 
	
	strcat(__display,"��ǰĿ¼��");
	strcat(__display,__currentPath);
	strcat(__display,"\n");
	
	//�������λ��
	int count = 0;	
	if(count ==( * __arrow))
	{
		strcat(__display,"->  ");
	}
	else
	{
		strcat(__display,"    ");
	}
	strcat(__display,"����\n");	
	
	
	count = 1;
	if(count ==( * __arrow))
	{
		strcat(__display,"->  ");
	}
	else
	{
		strcat(__display,"    ");
	}
	strcat(__display,"ճ��\n");
	
	
	count = 2;
	if(count ==( * __arrow))
	{
		strcat(__display,"->  ");
	}
	else
	{
		strcat(__display,"    ");
	}
	strcat(__display,"..(�ϲ�Ŀ¼)\n");
	
	//��������ǰĿ¼�����������ݴ���sec�������Ŵ���secNum 
	char * sec = (char *)malloc(sizeof(char)*1024);
	int * secNum = (int *)malloc(sizeof(int));
	
	if(getIndexSection(sec,secNum,__currentPath))
	{		
		extractFolderIndex(sec, __foi);
		
		*__childNodeNumber = __foi->childNodeNumber;
		int i;
		for(i=0;i<=__foi->childNodeNumber - 1;i++)
		{
			count ++;
			char * sec1 = (char *)malloc(sizeof(char)*1024);
			if(!readSection(sec1, __foi->childNode[i]))
			{
				setError(ILLEGAL_ACCESS_SYSTEM_DISK);
			}
			else
			{
				char * name = (char *)malloc(sizeof(char)*LENGTH_NAME);
				
				if(count ==(* __arrow))
				{
					strcat(__display,"->  ");
				}
				else
				{
					strcat(__display,"    ");
				}
				
				//��ȡ���ļ����У����ƺ����ͣ�������� 
				if(extractName(name,__foi->childNode[i]))
				{
					strcat(__display,name);
					//Ϊ��������ۣ������ֲ���38λ 
					int len=strlen(name);
					
					for(int j = 0;j<=LENGTH_NAME-len-1;j++)
					{
						strcat(__display," ");
					}
				
					if(sec1[0]==FILE_TYPE)
					{
						char * shortName =(char *) malloc(sizeof(char)*LENGTH_NAME);
						char * shortEx = (char *) malloc(sizeof(char)*LENGTH_NAME);
						
						extractFileName(shortName, shortEx,name);
						strcat(__display,"\t");
						strcat(__display,shortEx);
						strcat(__display,"�ļ�");
						
						free (shortName) ;
						free (shortEx) ; 	
					}
					else
					{
						strcat(__display,"\t�ļ���");	
					}
				}
				free (name) ;				
			}			
			strcat(__display,"\n");	
			free (sec1) ;
		}		
	}
	
	count ++;
	if(count ==( * __arrow))
	{
		strcat(__display,"->  ");
	}
	else
	{
		strcat(__display,"    ");
	}
	strcat(__display,"�˳�\n");
	
	
	free (sec) ;
	free (secNum) ;	
} 


void opeListInit(char * __display,int * __arrow)
{	
	strcat(__display, "�����б�\n");
	
	/*����7�ֶ��ļ�����*/
	int count = 0;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->");
	}
	else
	{
		strcat(__display, "    ");
	} 
	strcat(__display, "��");
	
	count = 1;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->");
	}
	else
	{
		strcat(__display, "    ");
	} 
	strcat(__display, "����");
	
	count = 2;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->");
	}
	else
	{
		strcat(__display, "    ");
	}
	strcat(__display, "����"); 
	
	
	count = 3;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->");
	}
	else
	{
		strcat(__display, "    ");
	}
	strcat(__display, "ɾ��");
	
	count = 4;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->");
	}
	else
	{
		strcat(__display, "    ");
	}
	strcat(__display, "������");
	
	count = 5;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->");
	}
	else
	{
		strcat(__display, "    ");
	}
	strcat(__display, "����");	
}

void newFileFolderInit(char * __display,int * __arrow)
{
	strcat(__display, "�ļ������б�\n") ;
	
	int count = 0;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->") ;
	}
	else
	{
		strcat(__display, "    ") ;
	}
	
	strcat(__display, "�ļ�");
	
	count = 1;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->") ;
	}
	else
	{
		strcat(__display, "    ") ;
	}
	strcat(__display, "�ļ���") ;
}


bool cutPath(char * __path)
{//��Դ�������з�����һ��ʱ��ȡ��ǰ·���ĺ��� 
	int len=strlen(__path);
	int i=len-1;
	
	while(__path[i]!='\\'&& i> 0)
	{
		i--;
	}
	
	if(i >= 0)
	{
		__path[i]='\0';
		return true;
	}
	else
	{
		return false;
	}
}

bool openFileDisplay(const char * __path)
{
	/*
	**��Դ�������д��ļ��Ľӿں��� 
	**path�а����˸��ļ�������
	*/
	
	return true ;
} 

/**************
 * ��������� *
 **************/

void moveOption(int __move)
{
	taskOption to[4] = {CREATE_PROCESS, DELETE_PROCESS, SHOW_PROCESS, EXIT} ;
	
	while (__move < 0) {
		__move += taskOptionNumber ;
	}
	
	taskManagerOption = to[(taskManagerOption + __move) % taskOptionNumber] ;
} 

void taskManagerInput()
{
	char ch ;
	
	while (taskManagerWorking) {
		if (!taskManagerRequest) {
			ch = _getch() ;
			
			switch (ch) {
				/* ������� */
				case 75 :
					taskInputKey = LEFT ;
					taskManagerRequest = true ;
				break ;
				
				/* ������� */
				case 77 :
					taskInputKey = RIGTH ;
					taskManagerRequest = true ;
				break ;
				
				/* �س� */
				case 13 :
					taskInputKey = ENTER ;
					taskManagerRequest = true ;
				break ;
				
				/* ESC */
				case 27 :
					taskInputKey = ESC ;
					taskManagerRequest = true ;
				break ;
				
				default :
					;
				break;
			}
		}
		else {
			Sleep(100) ;
		}
	}
}

void showOption()
{
	char dis__[200] = "\n�˵���\n\t" ;
	
	for (int i=0; i<=taskOptionNumber - 1; i++) {
		if (i == taskManagerOption) {
			strcat(dis__, " -> ");
		}
		else {
			strcat(dis__, "    ");
		}
		
		strcat(dis__, taskOptionDesc[i]);
	}
	
	puts(dis__) ;
}

void taskDetailDisplay(ProcessID __pid)
{
	//clock_t start = clock() - displayFrequency ;
	
	/* while (1) {
		
		if (taskManagerRequest) {
			if (taskInputKey == ESC) {
				taskManagerRequest = false ;
				return ;
			}
			else {
				taskManagerRequest = false ;
			}
		} */
		
		//if (clock()-start >= displayFrequency) {
		//	system("cls");
			if (showPCB(__pid)) {
				;
			}
			else {
				printError();
			}
			
		//	start = clock() ;
		//}
	//}
}

void doEnter()
{
	int pid__ ;
	char path__[200] ;
	
	switch (taskManagerOption) {
		case CREATE_PROCESS :
			printf("�������ִ���ļ�·����");
			scanf("%s", path__);
			
			if (newProcess(path__)) {
				printf("�����ѳɹ�����");
			}
			else {
				printError();
			}
			
		break ;
		
		case DELETE_PROCESS :
		{
			PCB * pcb__ ;
		
			printf("���������PID��");
			scanf("%d", &pid__);
			pcb__ = findPCB(pid__);
			
			if (pcb__ != NULL) {
				setProcessState(pcb__, TERMINATED);
				printf("�����ѳɹ�����");
			}
			else {
				printError();
			}
		}
		break ;
		
		case SHOW_PROCESS :
			printf("���������PID��");
			scanf("%d", &taskManagerPIDInput);
			
			displayModel = 1 ;
		break ;
		
		case EXIT :
			if (displayModel == 0) {
				taskManagerWorking = false ;
			}
			else if (displayModel == 1) {
				displayModel = 0 ;
			}
		break ;
		
		default :
		
		break;
	}
}

void taskRefresh()
{
	WaitForSingleObject(PCBQueueMutex, INFINITE);
	
	system("cls");
	if (displayModel == 0) {
		showPCBQueue();
		showOFL();
		showOption();
	}
	else if (displayModel == 1) {
		taskDetailDisplay(taskManagerPIDInput) ;
	}
	
	ReleaseMutex(PCBQueueMutex);
}

void taskDisplay()
{
	clock_t start = clock();
	
	while (taskManagerWorking) {
		
		if (clock()-start >= displayFrequency) {
			taskRefresh();
			start = clock() ;
		}
		
		if (taskManagerRequest) {
			switch (taskInputKey) {
				case LEFT :
					moveOption(-1);
					taskRefresh();
					taskManagerRequest = false ;
				break ;
				
				case RIGTH :
					moveOption(1);
					taskRefresh();
					taskManagerRequest = false ;
				break ;
				
				case ENTER :
					doEnter() ;
					taskRefresh();
					taskManagerRequest = false ;
				break ;
				
				case ESC :
					if (displayModel == 1) {
						displayModel = 0 ;
					}
					taskRefresh();
					taskManagerRequest = false ;
				break ;
				
				default :
					taskManagerRequest = false ;
				break;
			}
		}
		else {
			Sleep(100) ;
		}
	}
}

void taskManager(void)
{
	taskManagerWorking = true ;
	char ch ;
	
	HANDLE h1, h2 ;

	//hM=CreateMutex(NULL,FALSE,NULL);
	
	h1=CreateThread (NULL,0,taskManagerInputThread,NULL,0,NULL);
	h2=CreateThread (NULL,0,taskManagerDisplayThread,NULL,0,NULL);
	CloseHandle (h1);
	CloseHandle (h2);

	while (taskManagerWorking)
	{
		Sleep(2000);
		//ch = _getch() ;
		
		/* if (taskManagerRequest) {
			h2=CreateThread (NULL,0,taskManagerDisplayThread,NULL,0,NULL);
			CloseHandle (h2);
		} */
		
		//h1=CreateThread (NULL,0,taskManagerInput,NULL,0,NULL);
		//Sleep(1000);
	}
}

DWORD WINAPI taskManagerInputThread(LPVOID pPararneter)//��ӡ�߳�
{	
//	while (1)
	{
	//	WaitForSingleObject(hM,INFINITE);//ʹ�̻߳���
		taskManagerInput();
	//	ReleaseMutex(hM);
	}
	return 0;
}

DWORD WINAPI taskManagerDisplayThread(LPVOID pPararneter)//��ӡ�߳�
{	
//	while (1)
	{
	//	WaitForSingleObject(hM,INFINITE);//ʹ�̻߳���
		taskDisplay();
	//	ReleaseMutex(hM);
	}
	return 0;
}