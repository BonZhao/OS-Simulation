#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#define MAX 999999
#define MAX_FILE_PATH_LEN 25

struct MenuControl {
	int mode;          //mode == 0 �˵�ģʽ       mode == 1  �༭ģʽ
	int menuChooseShow;    //
	int menuChoose;
};

void showBoth(MenuControl *__menu, int bumb);
void showFilePathMenu(char * __filePath, char *__pint);
void runTextEditor();
#endif
