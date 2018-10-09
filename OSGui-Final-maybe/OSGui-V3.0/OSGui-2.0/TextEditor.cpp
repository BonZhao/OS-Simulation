/**
* MyOS\SystemApp.cpp
*
* Copyright (c) 2016 LiangJie
*/

/**
* Working log:
* 2016/04/04 20:00		Created				by LiangJie ;
* 2016/04/05    runTextEditor()             by LiangJie ;
* 2016/04/07    fix chooseMenu() bug        by LiangJie ;
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include "SystemApp.h"
#include "FileSystem.h"
#include "DiskDriver.h"
#include "Basic.h"
#include "TextEdit.h"
#include "ProcessManagement.h"
#include "SystemApp.h"
#include "MemoryManagement.h"


char showBuffer1[MAX];
char showBuffer2[MAX];
char readBuff[MAX];
char * filePath__;

struct Word{
	char aWord;			//字符
	Word * nextPtr;		//下一个
	Word * lastPtr;     //上一个
	int column;			//列号
	int row;			//行号
};

struct Text{
	Word * wordHead;//文件头节点 
	Word * wordWork;//工作节点
	int totalRow;
	int totalColumn;
	int totalWords;
};
Text myText;

/**
* [destroyText 回收文章内存]
*/
void destroyText() {
	Word * toDestory__, *toWork__;
	toDestory__ = myText.wordHead;
	toWork__ = myText.wordHead;

	while (toDestory__ != NULL) {
		toWork__ = toDestory__->nextPtr;
		free(toDestory__);
		toDestory__ = NULL;
		toDestory__ = toWork__;
	}
	myText.wordHead = NULL;
	myText.totalRow = 0;
	myText.totalColumn = 0;
	myText.totalWords = 0;
}

/**
* [createText 建立一篇文章]
*/
void createText() {
	/* 建立头节点 */
	
		myText.wordHead = (Word *)malloc(sizeof(Word));
		myText.wordHead->nextPtr = NULL;
		myText.wordHead->lastPtr = NULL;
		myText.wordHead->column = -1;
		myText.wordHead->row = 0;

		myText.totalRow = 0;
		myText.totalColumn = 0;
		myText.totalWords = 0;
		/*设定工作节点*/
		myText.wordWork = myText.wordHead;
		

}

/**
* [insertWord 在光标后添加一个单词]
* @param  __toInsert [添加的单词]
* @return            [返回是否成功]
*/
bool insertWord(char __toInsert) {
	Word *temp__;


	if (myText.wordWork == NULL) {
		puts("空指针错");
		return false;
	}
	temp__ = (Word *)malloc(sizeof(Word));
	temp__->nextPtr = myText.wordWork->nextPtr;
	temp__->lastPtr = myText.wordWork;
	temp__->aWord = __toInsert;

	/*如果上一个是换行符，则totalRow + 1*/
	if (myText.wordWork->aWord == '\n') {
		myText.totalRow++;
		temp__->row = myText.wordWork->row + 1; //temp行数加一
		temp__->column = 0;					    //temp列数为0
	}
	else {
		temp__->row = myText.wordWork->row;
		temp__->column = myText.wordWork->column + 1;	//temp列数加1
	}

	//wordWork不是最后一个节点
	if (myText.wordWork->nextPtr != NULL) {
		//下一个节点的last指向temp
		myText.wordWork->nextPtr->lastPtr = temp__;

	}

	//工作节点的next指向temp
	myText.wordWork->nextPtr = temp__;
	//工作节点向后移动一个
	myText.wordWork = temp__;


	Word * toModifyColumn__ = temp__->nextPtr;
	while (toModifyColumn__ != NULL && toModifyColumn__->row == temp__->row) {
		toModifyColumn__->column++;
		toModifyColumn__ = toModifyColumn__->nextPtr;
	}
	
	myText.totalWords++;
	return true;
}

/**
* [deleteWord 删除光标处单词]
* @return [返回是否成功]
*/
bool deleteWord() {
	Word * front__;
	if (myText.wordWork->lastPtr == NULL)
		return false;
	//处理前一个节点
	front__ = myText.wordWork->lastPtr;
	front__->nextPtr = myText.wordWork->nextPtr;


	//处理后一个节点
	if (myText.wordWork->nextPtr != NULL)
		myText.wordWork->nextPtr->lastPtr = front__;

	//如果被删除的是回车，则后面的节点每一个行号都要减一，且总行数减一
	//且原来这行的列数要发生相应改变
	if (myText.wordWork->aWord == '\n') {
		//处理行号
		Word * toModify__ = front__->nextPtr;
		while (toModify__ != NULL) {
			toModify__->row--;
			//如果和front在同一行就需要修改列号
			if (toModify__->row == front__->row) {
				toModify__->column = toModify__->lastPtr->column + 1;
			}
			toModify__ = toModify__->nextPtr;
		}
		myText.totalRow--; //总行数减一
	}
	//如果被删除的不是回车，则修改后面相同行的列号
	else {

		//处理该节点之后节点的列号
		Word * toModifyColumn__ = front__->nextPtr;
		if (toModifyColumn__ != NULL) {
			int nowRow__ = toModifyColumn__->row;
			while (toModifyColumn__ != NULL && toModifyColumn__->row == nowRow__) {
				toModifyColumn__->column--;
				toModifyColumn__ = toModifyColumn__->nextPtr;
			}
		}
	}
	free(myText.wordWork);
	myText.wordWork = NULL;
	myText.wordWork = front__;
	//释放节点
	myText.totalWords--;
	return true;

}

/**
* [showTextFlashWithBuffer 带闪烁显示字符]
* @param bumb [偶数显示_，奇数显示字符]
*/
void showTextFlashWithBuffer(int bumb) {
	//system("CLS");
	Word * toShow__;
	if (myText.wordHead == NULL) {
		return;
	}
	toShow__ = myText.wordHead->nextPtr;
	int it = 0;
	int it2 = 0;
	while (toShow__ != NULL) {
		if (toShow__ != myText.wordWork) {
			showBuffer1[it] = toShow__->aWord;
			showBuffer2[it2] = toShow__->aWord;
			it++;
			it2++;
		}

		else {
			showBuffer1[it] = toShow__->aWord;
			if (toShow__->aWord != '\n') {
				showBuffer2[it2] = '_';
			}
			else {
				showBuffer2[it2++] = '_';
				showBuffer2[it2++] = '|';
				showBuffer2[it2] = '\n';
			}
			it++;
			it2++;
		}

		toShow__ = toShow__->nextPtr;
	}
	showBuffer1[it] = '\0';
	showBuffer2[it2] = '\0';
	if (bumb % 2 == 0) {
		puts(showBuffer1);
	}
	else {
		puts(showBuffer2);
	}
	printf("\n                                                          total_row:%d   row:%d \n", myText.totalRow+1, myText.wordWork->row+1);
}





/**
* [dealUp 处理按上键]
*/
void dealUp() {
	int nowRow__ = myText.wordWork->row;
	int nowColumn = myText.wordWork->column;
	if (nowRow__ != 0) {
		while (myText.wordWork->row != nowRow__ - 1) {
			myText.wordWork = myText.wordWork->lastPtr;
		}
		while (myText.wordWork->column > nowColumn) {
			myText.wordWork = myText.wordWork->lastPtr;
		}
		if (myText.wordWork->aWord == '\n') {
			myText.wordWork = myText.wordWork->lastPtr;
		}
	}
}

/**
* [dealDown 处理按下键]
*/
void dealDown() {
	int nowRow__ = myText.wordWork->row;
	int nowColumn = myText.wordWork->column;
	if (nowRow__ != myText.totalRow) {
		while (myText.wordWork->row != nowRow__ + 1) {
			myText.wordWork = myText.wordWork->nextPtr;
		}
		while (myText.wordWork->nextPtr != NULL && myText.wordWork->column != nowColumn && myText.wordWork->row == nowRow__ + 1) {
			myText.wordWork = myText.wordWork->nextPtr;
		}
		if (myText.wordWork != NULL && myText.wordWork->row != nowRow__ + 1) {
			myText.wordWork = myText.wordWork->lastPtr->lastPtr;
		}

	}
}


/**
* [detectDir 用于线程控制按键]
* @param  pM [description]
* @return    [description]
*/
void detectDir(MenuControl *__menu){
	
		int k1 = _getch();
		if (0xe0 == k1) {
			int k2 = _getch();
			switch (k2) {
			case 72://上
				dealUp();
				break;
			case 80://下
				dealDown();
				break;
			case 75://左
				if (myText.wordWork->lastPtr == NULL) {
					break;
				}
				if (myText.wordWork->lastPtr != myText.wordHead) {
					myText.wordWork = myText.wordWork->lastPtr;
				}
				if (myText.wordWork->aWord == '\n') {
					myText.wordWork = myText.wordWork->lastPtr;
				}
				break;
			case 77://右
				if (myText.wordWork->nextPtr != NULL) {
					myText.wordWork = myText.wordWork->nextPtr;
				}
				if (myText.wordWork->nextPtr != NULL && myText.wordWork->aWord == '\n') {
					myText.wordWork = myText.wordWork->nextPtr;
				}
				break;
			default:
				break;
			}
			
		}
		else {
			if (k1 == 13) /*是回车*/{
				insertWord('\n');
			}
			else if (k1 == 8)/*是backspace*/ {
				deleteWord();
				showBoth(__menu, 0);
			}
			else if (k1 == 27) /*是ESC*/{
				__menu->mode = 0;
				showBoth(__menu, 0);
			}
			else {
				char inputc;
				inputc = (char)k1;
				/*读入被修改的字符*/
				insertWord(inputc);
				showBoth(__menu, 0);
			}

		}
		
}

/*DWORD WINAPI modeControl(LPVOID pM) {

}*/



void showControlInfo(MenuControl *__menu) {
	printf("%42s\n", "TEXTEDITOR");
	if (__menu == NULL) {
		puts("NO");
	}
	else if (__menu->menuChooseShow == 0) {
		printf("%15s%20s%20s%15s\n", "->新建文件", "打开文件", "保存文件","退出");
	}
	else if (__menu->menuChooseShow == 1) {
		printf("%15s%20s%20s%15s\n", "新建文件", "->打开文件", "保存文件", "退出");
	}
	else if (__menu->menuChooseShow == 2) {
		printf("%15s%20s%20s%15s\n", "新建文件", "打开文件", "->保存文件", "退出");
	}
	else if (__menu->menuChooseShow == 3) {
		printf("%15s%20s%20s%15s\n", "新建文件", "打开文件", "保存文件", "->退出");
	}
	
}

void showFilePathMenu(char * __filePath, char *__pint) {
	puts(__pint);
	scanf("%s", __filePath);
}
//打开文件并存到链表中
bool openFileToEdit(const char * __path) {
	FileStream * aFile__;
	int length__;
	aFile__ = (FileStream *)malloc(sizeof(FileStream));
	aFile__ = openFile(__path);
	if (aFile__ == NULL) {
		setError(FILE_READ_ERROR);
		return false;
	}
	//打开成功的话
	length__ = readWholeFile(readBuff, aFile__);
	createText();
	for (int i__ = 0; i__ < length__; i__++) {
		insertWord(readBuff[i__]);
	}
	closeFile(aFile__);
	return true;
	
}

bool saveFileToEdit(char * __path) {
	FileStream * aFile__;
	aFile__ = (FileStream *)malloc(sizeof(FileStream));
	aFile__ = openFile(__path);
	if (aFile__ == NULL) {
		setError(FILE_READ_ERROR);
		return false;
	}

	bool result__ =  saveWholeFile(showBuffer1, myText.totalWords, aFile__);
	closeFile(aFile__);
	return result__;
}

void chooseMenu(MenuControl *__menu) {
	if (__menu == NULL) {
		puts("NO");
	}
	int k1 = _getch();
	if (0xe0 == k1) {
		int k2 = _getch();
		switch (k2) {
		case 72://上
			break;
		case 80://下
			break;
		case 75://左
			if (__menu->menuChooseShow != 0) {
				(__menu->menuChooseShow)--;
				showBoth(__menu, 0);
			}
			break;
		case 77://右
			if (__menu->menuChooseShow != 3) {
				(__menu->menuChooseShow)++;
				showBoth(__menu, 0);
			}
			break;
		default:
			break;
		}
	}
	else if (k1 == 13) {
		__menu->menuChoose = __menu->menuChooseShow;
		
		if (__menu->menuChoose == 0) {//新建文件
			if (filePath__ != NULL) {
				filePath__ = (char *)malloc(sizeof(char) * MAX_FILE_PATH_LEN);
				free(filePath__);
				filePath__ = NULL;
			}
			destroyText();
			createText();
			__menu->mode = 1;
		}
		else if (__menu->menuChoose == 1) {//打开文件
			filePath__ = (char *)malloc(sizeof(char) * MAX_FILE_PATH_LEN);
			showFilePathMenu(filePath__, "请输入要打开的文件路径");
			if (!openFileToEdit(filePath__)) {
				printError();
			}
			else {
				puts("打开成功");
				__menu->mode = 1;
			}
			
		}
		else if (__menu->menuChoose == 2) {//保存文件
			if (myText.wordHead == NULL) {
				puts("无文件!");
			}
			else if (myText.wordHead->nextPtr == NULL) {
				puts("文件为空!");
			}
			else {
				//检查是否有路径
				if (filePath__ != NULL) {
					//保存文件
					if (saveFileToEdit(filePath__)) {
						puts("保存成功");
						destroyText();
						free(filePath__);
						filePath__ = NULL;
						
					}
					else {
						printError();
						destroyText();
					}
				}
				else {
					filePath__ = (char *)malloc(sizeof(char) * MAX_FILE_PATH_LEN);
					showFilePathMenu(filePath__, "请输入要保存的文件路径");
					puts("请输入文件名:");
					char *fileName__ = (char *)malloc(sizeof(char) * MAX_FILE_PATH_LEN);
					scanf("%s", fileName__);
					if (!newFile(fileName__, filePath__)) {
						printError();
						return;
					}
					
					sprintf(filePath__, "%s%s", filePath__, fileName__);

					if (saveFileToEdit(filePath__)) {
						puts("保存成功");
					}
					else {
						printError();
					}
					destroyText();
					free(fileName__);
					fileName__ = NULL;
					free(filePath__);
					filePath__ = NULL;
					//showBoth(__menu, 0);
				}
				
			}

		}
		else if (__menu->menuChoose == 3) {
			destroyText();
		}
	}
}

void showBoth(MenuControl *__menu, int bumb) {
	system("CLS");
	showControlInfo(__menu);
	printf("_______________________________________________________________________________\n");
	if (__menu->mode == 1) {
		printf("____________________________按ESC退出编辑模式__________________________________\n");
		printf("_______________________________________________________________________________\n");
	}
	showTextFlashWithBuffer(bumb);
}

/*DWORD WINAPI showControlFun(LPVOID pM)
{
	MenuControl *__menu = (MenuControl *)pM;
	while (__menu->menuChoose != 3) {
		showBoth(__menu, 0);
		Sleep(2000);
		while (__menu->mode != 0) {
			Sleep(1000);
		}
	}
	return 0;
}*/

DWORD WINAPI chooseModeFun(LPVOID pM)
{
	MenuControl *__menu = (MenuControl *)pM;
	while (__menu->menuChoose != 3) {
		while (__menu->mode != 0) {
			Sleep(1);
		}
		chooseMenu(__menu);
		Sleep(50);
		while (__menu->mode != 0) {
			Sleep(1000);
		}
	}
	return 0;
}


/**
* [showTextFun 用于线程的显示]
* @param  pM [description]
* @return    [description]
*/
DWORD WINAPI showTextFun(LPVOID pM)
{
	MenuControl *__menu = (MenuControl *)pM;
	int bumb = 2;
	while (__menu->menuChoose != 3) {
		//这一行以后再修改
		while (__menu->mode == 0) {
			Sleep(1000);
		}
		showBoth(__menu, bumb);
		bumb--;
		if (bumb == 0) {
			bumb = 2;
		}
		Sleep(500);
	}
	return 0;
}

DWORD WINAPI textEdit(LPVOID pM)
{
	MenuControl *__menu = (MenuControl *)pM;
	while (__menu->menuChoose != 3) {
		while (__menu->mode == 0) {
			Sleep(1000);
		}
		detectDir(__menu);
		Sleep(50);

	}
	
	return 0;
}

void runTextEditor() {
	MenuControl *menu__;
	menu__ = (MenuControl *)malloc(sizeof(MenuControl));

	menu__->menuChooseShow = 0;
	menu__->menuChoose = -1;
	menu__->mode = 0;     //选择菜单模式
	
	//展示选择菜单的线程
	/*HANDLE  showControlMenu = CreateThread(NULL, 0, showControlFun, menu__, 0, NULL);
	CloseHandle(showControlMenu);*/

	//选择菜单的线程
	HANDLE  handleChooseMenu = CreateThread(NULL, 0, chooseModeFun, menu__, 0, NULL);
	CloseHandle(handleChooseMenu);
	//显示输入内容的线程
	HANDLE  handleShowText = CreateThread(NULL, 0, showTextFun, menu__, 0, NULL);
	CloseHandle(handleShowText);
	//编辑输入的线程
	HANDLE  handleTextEdit = CreateThread(NULL, 0, textEdit, menu__, 0, NULL);
	CloseHandle(handleTextEdit);
	showBoth(menu__, 0);
	while (menu__->menuChoose != 3) {
		Sleep(1);
	}
}

void runTextEditor(char *path__) {
	MenuControl *menu__;
	menu__ = (MenuControl *)malloc(sizeof(MenuControl));

	menu__->menuChooseShow = 0;
	menu__->menuChoose = 1;
	menu__->mode = 0;     //选择菜单模式

	if (!openFileToEdit(path__)) {
		printError();
	}
	else {
		puts("打开成功");
		menu__->mode = 1;
		filePath__ = (char *)malloc(sizeof(char) * MAX_FILE_PATH_LEN);
		strcpy(filePath__, path__);
		system("pause");
	}
	
	//选择菜单的线程
	HANDLE  handleChooseMenu = CreateThread(NULL, 0, chooseModeFun, menu__, 0, NULL);
	CloseHandle(handleChooseMenu);
	//显示输入内容的线程
	HANDLE  handleShowText = CreateThread(NULL, 0, showTextFun, menu__, 0, NULL);
	CloseHandle(handleShowText);
	//编辑输入的线程
	HANDLE  handleTextEdit = CreateThread(NULL, 0, textEdit, menu__, 0, NULL);
	CloseHandle(handleTextEdit);
	//showBoth(menu__, 0);
	//showTextFlashWithBuffer(1);
	while (menu__->menuChoose != 3) {
		Sleep(1);
	}
}

/*int main(int argc, char const *argv[])
{
	
	if (!initFileSystem()) {
		printf("文件系统配置文件损坏，请检查\n");
	}

	if (!initializeMemory()) {
		printf("内存管理模块配置文件损坏，请检查\n");
	}
	initProcessManagement();
	//runTextEditor("C:\\2.c");
	//explorer();
	runTextEditor();
	destroyProcessManagement();
	system("pause");
	return 0;
}*/
