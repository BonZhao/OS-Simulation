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
const char * taskOptionDesc[4] = {"新建进程", "结束进程", "显示进程属性", "退出"} ;
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
	
	printf("文件名：%s\n", __fi.name) ;
	showTime(time, __fi.createTime);
	printf("创建时间：%s\n", time);
	showTime(time, __fi.lastModifyTime);
	printf("修改时间：%s\n", time);
	showTime(time, __fi.lastOpenTime);
	printf("打开时间：%s\n", time);
	printf("文件长度：%d\n数据块数量：%d\n父节点：%d\n", __fi.length, __fi.dataSectionNumber, __fi.fatherNode);
	printf("数据块扇区号：") ;
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
		printf("当前路径：%s\n", path__) ;
		
		if (getIndexSection(sec__, &secNum__, path__)) {
			if (sec__[0] == FILE_TYPE) {
				FileIndex fi__ ;
				
				initFileIndex(&fi__) ;
				extractFileIndex(sec__, &fi__) ;
				
				puts("这是一个文件，信息如下：");
				
				showFileIndex(fi__);
				
				destroyFileIndex(&fi__);
			}
			else {
				extractFolderIndex(sec__, &fo__) ;
				
				if (fo__.childNodeNumber == 0) {
					puts("该文件夹为空");
				}
				else {
					for (int i=0; i<=fo__.childNodeNumber-1; i++) {
						readSection(sec__, fo__.childNode[i]);
						
						printf("%s\t\t", &sec__[1]);
						if (sec__[0] == FILE_TYPE) {
							char n__[38], ex__[38] ;
							
							extractFileName(n__, ex__, &sec__[1]);
							printf("%s文件\t\t", ex__);
						}
						else {
							printf("文件夹\t\t") ;
						}
						
						printf("扇区号：%d\n", fo__.childNode[i]) ;
					}
					
					printf("共计%d个对象\n", fo__.childNodeNumber) ;
				}
			}
		}
		else {
			puts("路径错误") ;
		}
		
		printf("请输入下一级路径:");
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

//新增函数： 
void deleteOk(char * __display, int * __arrow)
{
	strcat(__display, "确认删除该文件么？\n") ;
	
	int count = 0 ;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->") ;
	}	
	else
	{
		strcat(__display, "    ") ;
	}
	strcat(__display, "确认") ;
	
	count = 1;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->") ;
	}	
	else
	{
		strcat(__display, "    ") ;
	}
	strcat(__display, "取消") ;
}

//函数
void account(FolderIndex * __fo, int * __fileNum, int * __folderNum, long int * __size, long int * __space)
{
	/*
	** 遍历一个文件夹的所有子节点 
	*/
	
	int num = __fo->childNodeNumber ;
	int  * child = (int *)malloc(sizeof(int) * __fo->childNodeNumber) ;
	child = __fo->childNode ; 
	
	int i = 0 ;
	char * sec = (char *)malloc(sizeof(char) * sectionSize) ;//记录扇区中的内容 
	
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
				//加上该文件夹索引块占用的一个扇区 
				(* __space)=(* __space) + 1024 ;	
				account(&fo, __fileNum, __folderNum, __size, __space) ;
			}
		}
	}
	
	free(sec);
	free(child);
}

/*修改了右键会出现菜单栏的bug。
**增加了可以用回车打开文件（夹）的功能
**查看文件夹属性中可以显示占用空间和文件夹大小 
*/ 

bool getAttr(const char * __fileName, char * __result,char * currentPath)
{
	/*
	** 查看文件或者文件夹的属性 
	**/
	 
	char * path = (char *)malloc(sizeof(char) * 1024) ;
	strcpy(path, currentPath);
	
	//filename为空时的特例，即查询根目录属性 
	if(strcmp(__fileName,""))
	{
		strcat(path, "\\") ;
		strcat(path, __fileName) ;	// 连接字符串，得到最终文件或文件夹的路径 
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
			
			strcpy(__result, "文件类型:txt文件") ;
			
			strcat(__result, "\n位置	:") ;
			strcat(__result, currentPath) ;
			
			
			_itoa(fi.length, a, 10) ;//将数字转换为字符串 
			strcat(__result, "\n大小	:") ;
			strcat(__result, a) ;
			strcat(__result,"字节") ;
			
			_itoa((fi.dataSectionNumber + 1) * 1024, a, 10) ;
			strcat(__result, "\n占用空间:") ;
			strcat(__result, a) ;
			strcat(__result,"字节") ;
			
			showTime(a, fi.createTime) ;
			strcat(__result, "\n创建时间:") ;
			strcat(__result, a) ;
			
			showTime(a, fi.lastModifyTime) ;
			strcat(__result, "\n修改时间:") ;
			strcat(__result, a ) ;
			
			showTime(a, fi.lastOpenTime) ;
			strcat(__result, "\n访问时间:") ;
			strcat(__result, a) ;			
		}
		else
		{
			FolderIndex fo ;
			
			initFolderIndex(&fo) ;
			extractFolderIndex(sec, &fo) ;
			
			strcpy(__result, "文件类型:文件夹") ;
			
			strcat(__result, "\n位置	:") ;
			strcat(__result, currentPath) ;
			
			/* 计算文件夹的大小、占用空间、下面的文件夹和文件的个数 */
			
			int fin = 0, fon = 0 ;
			long int size = 0, space = 0 ;
			//递归调用 
			account(&fo, &fin, &fon, &size, &space) ;
			
			strcat(__result,"\n包含	:");
			_itoa(fin,a,10);
			strcat(__result,a);
			strcat(__result,"个文件,");
			_itoa(fon,a,10);
			strcat(__result,a);
			strcat(__result,"个文件夹");
			
			_itoa(size,a,10);
			strcat(__result, "\n大小    :") ;
			strcat(__result,a) ;
			strcat(__result,"字节") ;
			
			_itoa(space,a,10);
			strcat(__result, "\n占用空间:  ") ;
			strcat(__result,a) ;
			strcat(__result,"字节") ;
						
			showTime(a, fo.createTime) ;
			strcat(__result, "\n创建时间:") ;
			strcat(__result, a) ;
		}
		
		free(a);		
	}
	
	free(path);
	free(sec);
	free(secNum);
	
	return true ;
}

//修正部分bug 

void explorer()
{
	/*
	**资源管理器 
	*/ 

	//当前目录 
	char * currentPath = (char *)malloc(sizeof(char)*1024);
	strcpy(currentPath , rootDirectoryLabel);
	strcat(currentPath ,":");

	//文件列表	
	char * display = (char *)malloc(sizeof(char) * 1024);
	strcpy(display,"") ;
	
	int * arrow = (int *)malloc(sizeof(int));
	* arrow = 0;
	
	
	//文件操作列表 
	char * display1 = (char *)malloc(sizeof(char) * 1024);
	strcpy(display1,"") ;
	
	int * arrow1 = (int *)malloc(sizeof(int));
	* arrow1 = 0;
	
	
	//创建操作列表 
	char * display2 = (char *)malloc(sizeof(char) * 1024);
	strcpy(display2,"") ;
	
	int * arrow2 = (int *)malloc(sizeof(int));	
	* arrow2 = 0;
	
	
	//确认删除界面
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
	
	//标记 
	int space = 0; 			/* 标记在文件操作列表显示时不响应上下键 */
	int spaceFlag = 1;		/* 标记空格的操作 */
	int opeFlag = 0;		/* 标记文件操作列表的操作 */
	int fileFlag = 1;		/* 标记文件列表 */ 
	int newFlag = 0;		/* 标记创建操作 */ 
	int cutOrCopy = -1 ; 	/* 区分剪切还是复制（剪切0，复制1），若为-1则不允许粘贴 */ 
	int enterFlag = 0;		/* 回车操作是否有效（无效0，有效1） */
	int deleteFlag = 0 ;	/* 确认删除标记 */ 
	
	int copySecNum = -1;
	
	char * cutSourPath = (char *)malloc(sizeof(char) * 1024) ;	/* 剪切的源路径 */
	char * copySourPath = (char *)malloc(sizeof(char) * 1024) ;	/* 复制的源路径 */
	
	int escFlag = 0;//退出标志 
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
			//方向键down 
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
			
			//方向键up 
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
			
			//方向键left 
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
			
			//方向键right
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
					
			//enter:确认操作 
			case 13:
				spaceFlag = 1;
				fileFlag = 1;
				
				if(space == 1|| !((*arrow)== 0 || (*arrow)== 1 || (* arrow) ==2 || (*arrow) == *childNum + 3))
				{
					if(space == 1)
						space = 0;
					//在当前路径下创建文件或者文件夹
					if (newFlag == 1)
					{
						printf("请输入名字：");
						char * name = (char *)malloc(sizeof(char) * LENGTH_NAME);
						gets(name);
						if ((* arrow2) == 0)
						{//文件 
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
					//文件操作列表 
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
								//打开
								case 0:
								
									if(sec[0] == FILE_TYPE)
									{
										//打开文件 
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
										
										//打开文件夹
										(* arrow) = 0; 
										fileFlag = 1;
										strcpy(display,"");
										system("cls");
										fileListInit(display,arrow,childNum,currentPath,&foi);
										puts(display);									
									} 							
								break;
						
								//剪切 
								case 1:
									cutOrCopy = 0;
									/*记录剪切文件的源路径*/
									strcpy(cutSourPath, currentPath) ; 
									strcat(cutSourPath,"\\");
									strcat(cutSourPath,name);
									copySecNum = secNum;
								
									strcpy(display,"");
									system("cls");
									(*arrow) = 0;
									fileListInit(display,arrow,childNum,currentPath,&foi);
									puts(display);
									
									printf("已复制到剪切板~~\n") ;						
								break; 
							
								//复制
								case 2:
									cutOrCopy = 1;
									/*记录复制文件的源路径*/
									strcpy(copySourPath, currentPath) ; 
									strcat(copySourPath,"\\");
									strcat(copySourPath,name); 
									copySecNum = secNum;
								
									strcpy(display,"");
									system("cls");
									(*arrow) = 0;
									fileListInit(display,arrow,childNum,currentPath,&foi);
									puts(display);
									
									printf("已复制到剪切板~~\n") ;
								break; 
								
								//删除
								case 3:	
								(*arrow3) = 0;
								opeFlag = 1;
								deleteFlag = 1;
							//	newFlag = 1;
								strcpy(display3,""); 
								deleteOk(display3, arrow3) ;
								puts(display3);
								break;
							
								//重命名
								case 4:
									printf("请输入新的名字：") ;
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
							
								//查看属性
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
					//enter直接打开文件	
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
										//打开文件 
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
								
								//打开文件夹
								(* arrow) = 0; 
								fileFlag = 1;
								strcpy(display,"");
								system("cls");
								fileListInit(display,arrow,childNum,currentPath,&foi);
								puts(display);									
							} 	
								
						}
					}
					//删除确认操作 
					else if(deleteFlag == 1)
					{
						deleteFlag = 0;
						fileFlag = 1;
						opeFlag = 0;
						
						//确定删除 
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
						//若取消，则什么也不做 
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
						
			//space:打开操作列表 
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
					/*当箭头处于创建和上一级目录时，不响应打开操作列表的行为*/ 
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
					{//响应创建操作
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
					{//粘贴 					
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
									//剪切之后的粘贴
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
									//复制之后的粘贴 
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
					{//响应返回上层目录请求
						//如当前目录已经是根目录，则不响应此请求 
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
			
			//esc：退出操作列表 
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
	/*传入参数为资源管理器显示的字符串，
	**此处进行初始化，
	**完成对当前目录下所有文件和文件夹的显示 
	*/ 	 
	
	strcat(__display,"当前目录：");
	strcat(__display,__currentPath);
	strcat(__display,"\n");
	
	//留出光标位置
	int count = 0;	
	if(count ==( * __arrow))
	{
		strcat(__display,"->  ");
	}
	else
	{
		strcat(__display,"    ");
	}
	strcat(__display,"创建\n");	
	
	
	count = 1;
	if(count ==( * __arrow))
	{
		strcat(__display,"->  ");
	}
	else
	{
		strcat(__display,"    ");
	}
	strcat(__display,"粘贴\n");
	
	
	count = 2;
	if(count ==( * __arrow))
	{
		strcat(__display,"->  ");
	}
	else
	{
		strcat(__display,"    ");
	}
	strcat(__display,"..(上层目录)\n");
	
	//搜索到当前目录的引导块内容存入sec，扇区号存入secNum 
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
				
				//提取出文件（夹）名称和类型，用于输出 
				if(extractName(name,__foi->childNode[i]))
				{
					strcat(__display,name);
					//为了输出美观，将名字补齐38位 
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
						strcat(__display,"文件");
						
						free (shortName) ;
						free (shortEx) ; 	
					}
					else
					{
						strcat(__display,"\t文件夹");	
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
	strcat(__display,"退出\n");
	
	
	free (sec) ;
	free (secNum) ;	
} 


void opeListInit(char * __display,int * __arrow)
{	
	strcat(__display, "操作列表：\n");
	
	/*共有7种对文件操作*/
	int count = 0;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->");
	}
	else
	{
		strcat(__display, "    ");
	} 
	strcat(__display, "打开");
	
	count = 1;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->");
	}
	else
	{
		strcat(__display, "    ");
	} 
	strcat(__display, "剪切");
	
	count = 2;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->");
	}
	else
	{
		strcat(__display, "    ");
	}
	strcat(__display, "复制"); 
	
	
	count = 3;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->");
	}
	else
	{
		strcat(__display, "    ");
	}
	strcat(__display, "删除");
	
	count = 4;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->");
	}
	else
	{
		strcat(__display, "    ");
	}
	strcat(__display, "重命名");
	
	count = 5;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->");
	}
	else
	{
		strcat(__display, "    ");
	}
	strcat(__display, "属性");	
}

void newFileFolderInit(char * __display,int * __arrow)
{
	strcat(__display, "文件操作列表：\n") ;
	
	int count = 0;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->") ;
	}
	else
	{
		strcat(__display, "    ") ;
	}
	
	strcat(__display, "文件");
	
	count = 1;
	if (count == (* __arrow))
	{
		strcat(__display, "  ->") ;
	}
	else
	{
		strcat(__display, "    ") ;
	}
	strcat(__display, "文件夹") ;
}


bool cutPath(char * __path)
{//资源管理器中返回上一层时获取当前路径的函数 
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
	**资源管理器中打开文件的接口函数 
	**path中包含了该文件的名字
	*/
	
	return true ;
} 

/**************
 * 任务管理器 *
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
				/* 方向键左 */
				case 75 :
					taskInputKey = LEFT ;
					taskManagerRequest = true ;
				break ;
				
				/* 方向键右 */
				case 77 :
					taskInputKey = RIGTH ;
					taskManagerRequest = true ;
				break ;
				
				/* 回车 */
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
	char dis__[200] = "\n菜单：\n\t" ;
	
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
			printf("请输入可执行文件路径：");
			scanf("%s", path__);
			
			if (newProcess(path__)) {
				printf("进程已成功创建");
			}
			else {
				printError();
			}
			
		break ;
		
		case DELETE_PROCESS :
		{
			PCB * pcb__ ;
		
			printf("请输入进程PID：");
			scanf("%d", &pid__);
			pcb__ = findPCB(pid__);
			
			if (pcb__ != NULL) {
				setProcessState(pcb__, TERMINATED);
				printf("进程已成功结束");
			}
			else {
				printError();
			}
		}
		break ;
		
		case SHOW_PROCESS :
			printf("请输入进程PID：");
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

DWORD WINAPI taskManagerInputThread(LPVOID pPararneter)//打印线程
{	
//	while (1)
	{
	//	WaitForSingleObject(hM,INFINITE);//使线程互斥
		taskManagerInput();
	//	ReleaseMutex(hM);
	}
	return 0;
}

DWORD WINAPI taskManagerDisplayThread(LPVOID pPararneter)//打印线程
{	
//	while (1)
	{
	//	WaitForSingleObject(hM,INFINITE);//使线程互斥
		taskDisplay();
	//	ReleaseMutex(hM);
	}
	return 0;
}