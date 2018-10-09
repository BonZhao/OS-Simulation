/**
* MyOS/mainWindow.c
*
* Copyright (c) 2016 LiangJie
*/

/**
* Working log
* 2016/04/23 10:03     Created by Liangjie
* 2016/05/14 完善了图形化界面内存部分代码
* 2016/5/14 11:13 合文件系统
*/

/*
 * 增加hidefilefolder.h 定义数据结构  hideFileFolderItem
 * vector<hideFileFolderItem>  hideFileFolderTable; 存隐藏的文件和文件夹
 * 在mainwindow.h中增加变量和函数如下
 * bool showHideFileFolder ; //是否显示隐藏的文件和文件夹 //111111
 *
 * void showFileFolder();  根据hideFileFolderTable数组和showHideFileFolder变量进行显示或隐藏
   void hideFileFolder();

   void changeChildAtt(char *__path,bool __hideOrProtect,int type);
   修改文件夹属性时调用此函数修改儿子的属性 type=0是修改隐藏属性，type=1修改保护属性

   void clay(QTreeWidgetItem *haveChild,QTreeWidgetItem *source_item,bool cut);
   粘贴调用此函数，修改粘贴内容的儿子的新路径，cut代表是否剪切。

   修改删除，重命名，粘贴，属性修改和其中的重命名
 * 修改了showAttrbute.cpp和showFileAtt.cpp
 * 增加了showhidechild.cpp和.h和ui 在showAttrbute.cpp中调用判断是否将更改应用于儿子，并增加取消操作
 * 修改了createfilesystem和createChildFileSystem(增加了根据文件系统初始化hideFileFolderTable数组)
 *
 *修改重命名引起的复制路径和粘贴路径的错误 根据item 重新计算源路径
 *修改删除文件时，将粘贴内容也删除引起的bug
 *

*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "FileSystem.h"
#include "SystemApp.h"
#include "Basic.h"
#include "MemoryManagement.h"
#include "ProcessManagement.h"
#include "VirtualMemory.h"
#include "MemoryLog.h"
#include "System.h"
#include "DiskDriver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include<QDebug>
#include <QContextMenuEvent>
#include "ExeUnit.h"
#include "IOUnit.h"
#include "workthread.h"
#include "IOUnit.h"
#include "WorkLog.h"
#include "MemoryDriver.h"
#include <QMessageBox>
#include "TextEdit.h"
#include <QTextCodec>
#include "createFileFolder.h"
#include "showDelete.h"
#include "showNewName.h"
#include "showAttribute.h"
#include "showFileAtt.h"
#include <conio.h>
#include <QContextMenuEvent>
#include<QTableWidget>
#include<QTableWidgetSelectionRange>
#include "Compile.h"
#include "hidefilefolder.h"
#include <QDateTime>
#include <vector>
extern char readBuff[MAX];
extern char showBuffer1[MAX];
extern char showBuffer2[MAX];
char dataBuff[MAX];
extern char * diskDescription;                  /* 磁盘描述 */
extern char * memoryDescription;                /* 内存描述 */
extern int VMFrameCount;                        /* 虚拟帧个数 */
extern int VMAvailableFrameCount;               /* 空闲帧个数 */
extern int frameCount;                          //帧个数
extern Frame * frameTable;                      //页表
extern VMFrame* VMFrameTable;                   //帧表
extern int frameSize;
extern int sectionSize;
extern int pageType;

extern PCBQueue * processQueue;
extern OFL * openFileList;
extern HANDLE PCBQueueMutex;
extern PCB * runningProcess ;
bool ju;

extern MemoryFrameFault *MFFhead, *MFFlast;     //内存log
extern ErrorType errorCode;
extern const char * errorDesc[100];
char *showAttributePath = (char *)malloc(sizeof(char)*38);
char * writeBuff;
extern bool LOG_OUTPUT;
extern bool isDynamicPriority;
extern AccountLevelType accountLevel ;
extern QString userName;

int bp;

int textLength;
Q_DECLARE_METATYPE(QString);  //从QString转换成char*

using namespace std;

vector<hideFileFolderItem>  hideFileFolderTable;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showSystemInfo();

    //workThread *startExeThread = new workThread(0);
    //ioThread = new workThread(1);
    workThread *showMem = new workThread(2);

    connect(showMem,SIGNAL(changed()),this,SLOT(onChanged()));
    //startExeThread->start();
    showMem->start();

    if(LOG_OUTPUT == true) {
        ui->checkBox_isLogOut->setChecked(true);
    }
    else {
        ui->checkBox_isLogOut->setChecked(false);
    }

    if(isDynamicPriority == true) {
        ui->checkBox_isDynamic->setChecked(true);
    }
    else {
        ui->checkBox_isDynamic->setChecked(false);
    }
    showUserName();
    ui->textBrowser_memContent->hide();
    ui->pushButton_textEditor_save->hide();
    ui->stackedWidget_textEditor->setCurrentIndex(0);
    ui->stackedWidget->setCurrentIndex(0);


    /******************************/

    source_path = "";  //原路径
    dest_path = ""; //目的路径
    this->fileName = "";
    clay_time = 0; //粘贴次数
    type = -1;
    this->source_item = NULL;
    this->showHideFileFolder = false;
    this->itemC = NULL;
    this->renovate = 0;
    createItemMenu();
    createFileSystem();

    //process part
    createProtab();
    createPItemMenu();
    if(accountLevel == GUEST_USER){
        ui->checkBox->setHidden(true);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showTime() {
    QDateTime q = QDateTime::currentDateTime();
    //ui->timeEdit->setTime(q);
    QString temp = q.toString("yyyy-MM-dd hh:mm:ss");
    ui->label_time->setText(temp);

}

void MainWindow::showUserName() {
    if(accountLevel == MANAGER_USER) {
        qDebug()<<userName;
        QString toShow__ = userName + "(管理员)";
        qDebug()<<toShow__;
        ui->label_userName->setText(toShow__);
    }
    else {
        QString toShow__ = "访客";
        ui->label_userName->setText(toShow__);
    }
}

void MainWindow::showSystemInfo() {
    ui->label_system->setText("LCZY 1.0");
    ui->label_cal->setText("LCZY WuKong @ i1 单核");
    ui->label_mem->setText(QString::fromLocal8Bit(memoryDescription));
    ui->label_disk->setText(QString::fromLocal8Bit(diskDescription));

}

void MainWindow::showMemTable() {

    double utiliMem__ = calMemUtilization();
    int utilMul100__ = 100 * utiliMem__;
    ui->progressBar_mem->setValue(utilMul100__);

    ui->tableWidget_Mem->setRowCount(frameCount);     //设置行数
    ui->tableWidget_Mem->setColumnCount(4);            //设置列数
    ui->tableWidget_Mem->verticalHeader()->setVisible(false);   //隐藏列表头
     ui->tableWidget_Mem->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑

    QStringList MemHeader;
    MemHeader<<"frame num"<<"is available"<< "PID" << "page num";

    ui->tableWidget_Mem->setHorizontalHeaderLabels(MemHeader);

    //逐行添加信息
    for(int i__ = 0; i__ < frameCount; i__++) {
        QPushButton *itemFrameNumPush = new QPushButton(QString::number(i__));
        QTableWidgetItem *itemFrameNum = new QTableWidgetItem(QString::number(i__));
        connect(ui->tableWidget_Mem,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(showInfo2()));

       itemFrameNum->setTextAlignment(4);//设置居中
        ui->tableWidget_Mem->setItem(i__,0,itemFrameNum);
       // ui->tableWidget_Mem->setCellWidget(i__,0,itemFrameNumPush);
         //connect(itemFrameNumPush,SIGNAL(clicked()),this,SLOT(showInfo()));
        if (frameTable[i__].isAvailable == true) {
            QTableWidgetItem *itemAvai = new QTableWidgetItem("true");
            QTableWidgetItem *itemPID = new QTableWidgetItem(QString::number(frameTable[i__].PID));
            QTableWidgetItem *itemPageNum = new QTableWidgetItem(QString::number(frameTable[i__].pageNum));

            itemAvai->setTextAlignment(4);
            itemPID->setTextAlignment(4);
            itemPageNum->setTextAlignment(4);

            ui->tableWidget_Mem->setItem(i__,1,itemAvai);
            ui->tableWidget_Mem->setItem(i__,2,itemPID);
            ui->tableWidget_Mem->setItem(i__,3,itemPageNum);

        }
        else {
            QTableWidgetItem *itemAvai = new QTableWidgetItem("false");
            QTableWidgetItem *itemPID = new QTableWidgetItem("null");
            QTableWidgetItem *itemPageNum = new QTableWidgetItem("null");

            itemAvai->setTextAlignment(4);
            itemPID->setTextAlignment(4);
            itemPageNum->setTextAlignment(4);

            ui->tableWidget_Mem->setItem(i__,1,itemAvai);
            ui->tableWidget_Mem->setItem(i__,2,itemPID);
            ui->tableWidget_Mem->setItem(i__,3,itemPageNum);
        }
    }

     ui->tableWidget_Mem->show();
}



//此处1024为frameSize,但frameSize在qt中已定义
void MainWindow:: showInfo(){
    QPushButton *p = (QPushButton *)sender();
    int pframeNum = p->text().toInt();
    if(frameTable[pframeNum].isAvailable == true) {
        char *data__ = (char*)malloc(sizeof(char) * 1024);
        char * hexData__ = (char*)malloc(sizeof(char) * 1024 * 2);
        int offset = pframeNum * 1024;
        if(readMemoryByFrame(offset,data__)) {

            strToHex(data__,hexData__);
            ui->textBrowser_memContent->setText(QString(hexData__));
        }
    }
    ui->textBrowser_memContent->show();


}

void MainWindow:: showInfo2(){

    int pframeNum = ui->tableWidget_Mem->currentRow();
    if(frameTable[pframeNum].isAvailable == true) {
       // char data__[1025];
        //char hexData__[1025*2];
        int offset = pframeNum * 1024;
        if(readMemoryByFrame(offset,dataBuff)) {
           //qDebug() << data__;
            strToHex(dataBuff,showBuffer2,1024);


            ui->textBrowser_memContent->setText(QString(showBuffer2));
        }
    }
    else {
        ui->textBrowser_memContent->setText("");
    }
    ui->textBrowser_memContent->show();


}
void MainWindow::showVMemTable() {

    double utiliVm__ = calVMUtilization();
    int utiliVm100__ = 100 * utiliVm__;
    ui->progressBar_vm->setValue(utiliVm100__);

    ui->tableWidget_vm->setRowCount(VMFrameCount);     //设置行数
    ui->tableWidget_vm->setColumnCount(4);            //设置列数
    ui->tableWidget_vm->verticalHeader()->setVisible(false);   //隐藏列表头
    ui->tableWidget_vm->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑

    QStringList MemHeader;
    MemHeader<<"vframe num"<<"is available"<< "PID" << "page num";

    ui->tableWidget_vm->setHorizontalHeaderLabels(MemHeader);

    //逐行添加信息
    for(int i__ = 0; i__ < VMFrameCount; i__++) {
        QTableWidgetItem *itemFrameNum = new QTableWidgetItem(QString::number(i__));
        itemFrameNum->setTextAlignment(4);//设置居中
        ui->tableWidget_vm->setItem(i__,0,itemFrameNum);
        if (VMFrameTable[i__].isAvailable == true) {
            QTableWidgetItem *itemAvai = new QTableWidgetItem("true");
            QTableWidgetItem *itemPID = new QTableWidgetItem(QString::number(VMFrameTable[i__].PID));
            QTableWidgetItem *itemPageNum = new QTableWidgetItem(QString::number(VMFrameTable[i__].pageNum));

            itemAvai->setTextAlignment(4);
            itemPID->setTextAlignment(4);
            itemPageNum->setTextAlignment(4);

            ui->tableWidget_vm->setItem(i__,1,itemAvai);
            ui->tableWidget_vm->setItem(i__,2,itemPID);
            ui->tableWidget_vm->setItem(i__,3,itemPageNum);
        }
        else {
            QTableWidgetItem *itemAvai = new QTableWidgetItem("false");
            QTableWidgetItem *itemPID = new QTableWidgetItem("null");
            QTableWidgetItem *itemPageNum = new QTableWidgetItem("null");

            itemAvai->setTextAlignment(4);
            itemPID->setTextAlignment(4);
            itemPageNum->setTextAlignment(4);

            ui->tableWidget_vm->setItem(i__,1,itemAvai);
            ui->tableWidget_vm->setItem(i__,2,itemPID);
            ui->tableWidget_vm->setItem(i__,3,itemPageNum);
        }
    }
    ui->tableWidget_vm->show();
}

void MainWindow::showPageFault() {
    ui->tableWidget_pageFault->setRowCount(VMFrameCount);     //设置行数
    ui->tableWidget_pageFault->setColumnCount(5);            //设置列数
    ui->tableWidget_pageFault->verticalHeader()->setVisible(false);   //隐藏列表头
    ui->tableWidget_pageFault->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑

    QStringList MMFHeader;
    MMFHeader<< "frame num"<<"old page"<<"old pid"<< "new page" << "new pid" ;

    ui->tableWidget_pageFault->setHorizontalHeaderLabels(MMFHeader);
     //逐行添加信息
    MemoryFrameFault * work__;
    if(MFFhead == NULL) {

    }
    else {
        work__ = MFFhead->next;
        int i__ = 0;
        while(work__ != NULL) {
            QTableWidgetItem *itemFrameNum = new QTableWidgetItem(QString::number(work__->frameNum));
             QTableWidgetItem *itemPageNew = new QTableWidgetItem(QString::number(work__->pageNew));
            QTableWidgetItem *itemPidNew = new QTableWidgetItem(QString::number(work__->pidNew));
            QTableWidgetItem *itemPageOld;
            QTableWidgetItem *itemPidOld;
            if(work__->pageOld == -1) {
                itemPageOld = new QTableWidgetItem("无");
                itemPidOld = new QTableWidgetItem("无");
            }
            else {
                itemPageOld = new QTableWidgetItem(QString::number(work__->pageOld));
                itemPidOld = new QTableWidgetItem(QString::number(work__->pidOld));
            }

            itemFrameNum->setTextAlignment(4);
            itemPageOld->setTextAlignment(4);
            itemPidOld->setTextAlignment(4);
            itemPageNew->setTextAlignment(4);
            itemPidNew->setTextAlignment(4);

            ui->tableWidget_pageFault->setItem(i__,0,itemFrameNum);
            ui->tableWidget_pageFault->setItem(i__,1,itemPageOld);
            ui->tableWidget_pageFault->setItem(i__,2,itemPidOld);
            ui->tableWidget_pageFault->setItem(i__,3,itemPageNew);
            ui->tableWidget_pageFault->setItem(i__,4,itemPidNew);

            work__ = work__->next;
            i__++;
        }

    }
    ui->tableWidget_pageFault->show();
}

void MainWindow::on_pushButton_MemM_clicked()
{
    showMemTable();
    showVMemTable();
    showPageFault();
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_overview_clicked()
{
    showSystemInfo();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{

    if(index == 0) { //内存情况
        showMemTable();
    }
    else if(index == 1) {//虚存情况
        showVMemTable();
    }
    else if(index == 2) {
        showPageFault();
    }
}

void MainWindow::on_pushButton_explorer_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}


void MainWindow::on_pushButton_2_clicked()
{

    /*for (int i = 0; i <= 5; i++) {
            if (newProcess("C:\\test\\go.run")) {
                puts("success");
            }
            else {
                printError();
            }
        }*/
    ju=false;
    ui->label_prn->setText(QString::number(getQueueLength(RUNNING),10));
    ui->label_pwn->setText(QString::number(getQueueLength(WAITING),10));
    ui->label_proinrea->setText(QString::number(getQueueLength(READY),10));
    ui->stackedWidget_2->setCurrentIndex(1);
    ui->stackedWidget->setCurrentIndex(5);

}

void MainWindow::onChanged(void) {
    this->showMemTable();
    this->showVMemTable();
    this->showPageFault();
    this->showProtab();
    this->showdettab();
    this->showfirstpage();
    this->showTime();
}

void MainWindow::on_pushButton_powerOff_clicked()
{
    destroySystem();

    this->close();
}

void MainWindow::on_pushButton_textEditor_clicked()
{
    ui->label_TextPath->clear();
    ui->label_TextPath->hide();
    ui->pushButton_textEditor_save->hide();
    ui->textEditApp->clear();
    ui->stackedWidget->setCurrentIndex(4);
    ui->stackedWidget_textEditor->setCurrentIndex(0);
}
/*保存按钮*/
void MainWindow::on_pushButton_textEditor_save_clicked()
{
    QString filePath__ = ui->label_TextPath->text();
    std::string str__ = filePath__.toStdString();
    char* path__ =(char *)str__.c_str();

    FileStream * aFile__;
        aFile__ = (FileStream *)malloc(sizeof(FileStream));
        aFile__ = openFile((char *)path__);
        if (aFile__ == NULL) {
            setError(FILE_READ_ERROR);
            QMessageBox::warning(this,"文件保存失败","文件读取错误",QMessageBox::Cancel);
            return;
        }
     QString text__ = ui->textEditApp->toPlainText();

     std::string texts__ = text__.toStdString();
     int length__ = texts__.length();
     //qDebug() << length__;
     writeBuff =(char *)texts__.c_str();
      //qDebug() << writeBuff;
      writeBuff[length__] = '\0';
        bool result__ =  saveWholeFile(writeBuff, length__, aFile__);
        closeFile(aFile__);
        if(!result__) {
            printErrorGui();
        }
        else {
             QMessageBox::information(this,"恭喜","文件保存成功",QMessageBox::Ok);

        }
}

void MainWindow::on_pushButton_textCancel_clicked()
{
    ui->stackedWidget_textEditor->setCurrentIndex(0);
}

void MainWindow::on_pushButton_textEditor_saveAs_clicked()
{
    ui->stackedWidget_textEditor->setCurrentIndex(1);
}

/*点击另存为确定按钮*/
void MainWindow::on_pushButton_textSave_clicked()
{
    QString filePath__ = ui->lineEdit_textPath->text();
    std::string str__ = filePath__.toStdString();
    char* path__ =(char *)str__.c_str();

    QString textName__ = ui->lineEdit_textName->text();
    std::string str2__ = textName__.toStdString();
    char* textNamec__ = (char *)str2__.c_str();

    //qDebug() << textNamec__ << path__;

    if (!newFile(textNamec__, path__)) {
        printErrorGui();
        return;
    }

    std::string fullPath__ = str__+"\\"+str2__;
    const char * fullPathc = fullPath__.c_str();
    QString fullPathQ__ = QString::fromStdString(fullPath__);
    FileStream * aFile__;
        aFile__ = (FileStream *)malloc(sizeof(FileStream));
        aFile__ = openFile((char *)fullPathc);
        if (aFile__ == NULL) {
            //setError(FILE_READ_ERROR);
            this->printErrorGui();
            //QMessageBox::warning(this,"文件保存失败","文件读取错误",QMessageBox::Cancel);
            return;
        }
     QString text__ = ui->textEditApp->toPlainText();
     std::string texts__ = text__.toStdString();
     int length__ = texts__.length();
     writeBuff =(char *)texts__.c_str();
        bool result__ =  saveWholeFile(writeBuff, length__, aFile__);
        closeFile(aFile__);
        if(!result__) {
            printErrorGui();
        }
        else {
             QMessageBox::information(this,"恭喜","文件保存成功",QMessageBox::Ok);
             ui->lineEdit_textName->clear();
             ui->lineEdit_textPath->clear();
             renovateFileSystem();
             on_pushButton_textEditor_clicked();
             showFileWithNoHint(fullPathQ__);
        }
}

/*点击打开按钮*/
void MainWindow::on_pushButton_textEditor_open_clicked()
{
    ui->stackedWidget_textEditor->setCurrentIndex(2);
}

void MainWindow::on_pushButton_textOpenCancel_clicked()
{
     ui->stackedWidget_textEditor->setCurrentIndex(0);
}

void MainWindow::showFileWithNoHint(QString __textPath) {
    ui->textEditApp->clear();
    ui->label_TextPath->clear();
    ui->label_TextPath->hide();
    ui->pushButton_textEditor_save->hide();

    std::string str = __textPath.toStdString();
    const char *path__ = (char *)str.c_str();
    FileStream * aFile__;
        int length__;
        aFile__ = (FileStream *)malloc(sizeof(FileStream));
        aFile__ = openFile(path__);
        if (aFile__ == NULL) {
            setError(FILE_READ_ERROR);
            //QMessageBox::warning(this,"文件打开失败","文件读取错误",QMessageBox::Cancel);
            return;
        }
        //打开成功的话
        length__ = readWholeFile(readBuff, aFile__);
        textLength = length__;
        //qDebug()<<"The Length is"<<length__;

            readBuff[length__] = '\0';

        closeFile(aFile__);
        //QMessageBox::information(this,"成功","文件打开成功",QMessageBox::Ok);


        ui->label_TextPath->setText(__textPath);
        ui->label_TextPath->show();

        std::string toShow__ = readBuff;
        QString textShow__ = QString::fromStdString(toShow__);
        ui->textEditApp->setText(textShow__);

        ui->pushButton_textEditor_save->show();
}

void MainWindow::showFile(QString __textPath) {
    ui->textEditApp->clear();
    ui->label_TextPath->clear();
    ui->label_TextPath->hide();
    ui->pushButton_textEditor_save->hide();

    std::string str = __textPath.toStdString();
    const char *path__ = (char *)str.c_str();
    FileStream * aFile__;
        int length__;
        aFile__ = (FileStream *)malloc(sizeof(FileStream));
        aFile__ = openFile(path__);
        if (aFile__ == NULL) {
            setError(FILE_READ_ERROR);
            QMessageBox::warning(this,"文件打开失败","文件读取错误",QMessageBox::Cancel);
            return;
        }
        //打开成功的话
        length__ = readWholeFile(readBuff, aFile__);
        textLength = length__;
        //qDebug()<<"The Length is"<<length__;

            readBuff[length__] = '\0';

        closeFile(aFile__);
        QMessageBox::information(this,"成功","文件打开成功",QMessageBox::Ok);


        ui->label_TextPath->setText(__textPath);
        ui->label_TextPath->show();

        std::string toShow__ = readBuff;
        QString textShow__ = QString::fromStdString(toShow__);
        ui->textEditApp->setText(textShow__);

        ui->pushButton_textEditor_save->show();
}

/*编译*/
void MainWindow::on_pushButton_compile_clicked()
{
    on_pushButton_textEditor_save_clicked();
    QString text__ = ui->textEditApp->toPlainText();
    std::string str__ = text__.toStdString();
    const char * toCompile__ = str__.c_str();
    unsigned char ucode__[1024];
    char  code__ [1024];
    //qDebug()<<toCompile__;
    int length__ = compiler(toCompile__, ucode__);
    //qDebug()<<"length is "<<length__;
    if(length__ < 0) {
        QMessageBox::warning(this,"Warning","编译失败",QMessageBox::Cancel);
        return;
    }
    for(int i = 0; i < length__; i++) {
        code__[i] = ucode__[i];
    }

    QString filePath__ = ui->label_TextPath->text();

    QString pathFront__ = filePath__.section("\\",0,-2);
    QString fileNameFront__ = filePath__.section("\\",-1,-1).section(".",-2,0);

    //qDebug()<<fileNameFront__;
    //qDebug()<<pathFront__;

    QString fileNameQS__ = fileNameFront__ + ".run";
    std::string strfileName__ = fileNameQS__.toStdString();
    char* fileName__ =(char *) strfileName__.c_str();

    std::string strPath__ = pathFront__.toStdString();
    const char* path__ =strPath__.c_str();

    QString fullPathQS__ = pathFront__ +"\\"+fileNameQS__;
    std::string strPathFull__ = fullPathQS__.toStdString();
    char * pathFull_ = (char *)strPathFull__.c_str();


    if(!fileNotOpen(pathFull_)) {
        QMessageBox::warning(this,"Sorry","进程正在执行，请关闭进程后编译",QMessageBox::Cancel);
        return;
    }

    FileStream * aFile__;
        aFile__ = openFile((char *)pathFull_);
        if (aFile__ == NULL) {
            if (!newFile(fileName__, path__)) {
                //qDebug() << "path is "<<path__;
               // qDebug() << "name is "<<fileName__;
                printErrorGui();
                return;
            }
            aFile__ = openFile((char *)pathFull_);
        }


     bool result__ =  saveWholeFile(code__,length__ , aFile__);
        closeFile(aFile__);
        if(!result__) {
            printErrorGui();
        }
        else {
             QMessageBox::information(this,"恭喜","文件编译成功",QMessageBox::Ok);
             renovateFileSystem();
        }


}


/*确定打开一个文件*/
void MainWindow::on_pushButton_textOpen_clicked()
{

    QString textPath__ = ui->lineEdit_textPath_open->text();
    showFile(textPath__);
    ui->stackedWidget_textEditor->setCurrentIndex(0);
}


void MainWindow::on_pushButton_toHex_clicked()
{
    if(ui->pushButton_toHex->text() == "十六进制显示") {
        ui->pushButton_toHex->setText("正常显示");
        strToHex(readBuff,showBuffer1,textLength);
        std::string toShow__ = showBuffer1;
        QString textShow__ = QString::fromStdString(toShow__);
        ui->textEditApp->setText(textShow__);
        ui->stackedWidget_textEditor->setCurrentIndex(0);
    }
    else {
        ui->pushButton_toHex->setText("十六进制显示");
        std::string toShow__ = readBuff;
        QString textShow__ = QString::fromStdString(toShow__);
        ui->textEditApp->setText(textShow__);
        ui->stackedWidget_textEditor->setCurrentIndex(0);
    }

}
/*****************************************************************/


void MainWindow::createChildFileSystem(char *__path,QTreeWidgetItem *parent){

    char *sec__ = (char *)malloc(sizeof(char)*sectionSize);
    int *secNum__ = (int *)malloc(sizeof(int)*10);
    if(getIndexSection(sec__,secNum__, __path) == false){
        this->printErrorGui();
        free(sec__);
        free(secNum__);
        return;
    }
    QStringList rootTextList;
    rootTextList << "" << ""<<"";

    if(sec__[0] == FOLDER_TYPE || sec__[0] == ROOT_TYPE){
        QTreeWidgetItem *child__ = new QTreeWidgetItem(parent, rootTextList);
        FolderIndex fo1 ;
        initFolderIndex(&fo1);
        extractFolderIndex(sec__, &fo1);

        child__->setText(0,QString(QLatin1String(fo1.name)));
        child__->setText(1,"文件夹");

        if(fo1.attribute.hide){    //vector<hideFileFolderItem>  hideFileFolderTable;    //11111111111
            //int sign = 1;
           // for(int i = 0;i<hideFileFolderTable.size();i++)
               //  if(QString(QLatin1String(hideFileFolderTable[i].path)) == QString(QLatin1String(__path)) ){
                   //     sign = 0;
                //   //     break;
//                 }
           // if(sign == 1) {

               // qDebug()<<QString(QLatin1String(__path));
                hideFileFolderItem item;
                item.itself = child__;
                item.path = __path;
                item.type = FOLDER_TYPE;
                item.hideChild = false;
                hideFileFolderTable.push_back(item);

       //     }

        }

        if(fo1.childNodeNumber <= 0){
            child__->setText(2,"空文件夹");
            free(sec__);
            free(secNum__);
            return;
        }
        for(int  i=0;i<fo1.childNodeNumber;i++) {   //extractFileName(char * __name, char * __ex, const char * __whole);
                char *name__ = (char *)malloc(sizeof(char)*38);
                if(extractName(name__, fo1.childNode[i]) == false){
                    this->printErrorGui();

                    break;
                }
                char *path__ = (char *)malloc(sizeof(char)*90);

                strcpy(path__, __path);

                strcat(path__,"\\") ;
                strcat(path__,name__) ;
             //   qDebug()<<QString(QLatin1String(path__));
                createChildFileSystem(path__,child__);
                free(name__);
                free(path__);
        }
    }
    else if(sec__[0] == FILE_TYPE){
         QTreeWidgetItem *child__ = new QTreeWidgetItem(parent, rootTextList);
         FileIndex file;
         initFileIndex(&file);
         extractFileIndex(sec__,&file);
         char *name__1 = (char *)malloc(sizeof(char)*38);
         char *ex__ = (char *)malloc(sizeof(char)*38);
         extractFileName(name__1,ex__,file.name);
         child__->setText(0,QString(QLatin1String(file.name)));
         if(QString(QLatin1String(ex__)) == "")
              child__->setText(1,"文件");
         else
             child__->setText(1,QString(QLatin1String(ex__))+"文件");

         child__->setText(2,QString::number(file.length,10)+"字节");

         if(file.attribute.hide){    //vector<hideFileFolderItem>  hideFileFolderTable;    //11111111111
            // int sign = 1;
           //  for(int i = 0;i<hideFileFolderTable.size();i++)
                //  if(QString(QLatin1String(hideFileFolderTable[i].path)) == QString(QLatin1String(__path)) ){
                //         sign = 0;
                //         break;
               //   }
            // if(sign == 1) {
                 hideFileFolderItem item;
                 item.itself = child__;
                 item.path = QString(QLatin1String(__path));

                 item.type = FILE_TYPE;
                 item.hideChild = false;
                 hideFileFolderTable.push_back(item);

           //  }

         }

         free(name__1);
         free(ex__);
    }
    else{
        qDebug()<<"error23";
    }
    free(sec__);
    free(secNum__);
}

void MainWindow::createItemMenu(){
        fileMenu__ = new QMenu(ui->treeWidget_explorer);
        folderMenu__ = new QMenu(ui->treeWidget_explorer);

        connect(fileMenu__,SIGNAL(triggered(QAction *)),
                this,SLOT(onFileMenuTriggered(QAction *)));

        connect(folderMenu__,SIGNAL(triggered(QAction *)),
                this,SLOT(onFolderMenuTriggered(QAction *)));

        create__ = new QAction(tr("创建"),this);
        clay__ = new QAction(tr("粘贴"),this);

        shear__ = new QAction(tr("剪切"),this);
        shear__1 = new QAction(tr("剪切"),this);


        copy__ = new QAction(tr("复制"),this);
        copy__1= new QAction(tr("复制"),this);

        delete__ = new QAction(tr("删除"),this);
        delete__1 = new QAction(tr("删除"),this);

        rename__ = new QAction(tr("重命名"),this);
        rename__1 = new QAction(tr("重命名"),this);

        property__ = new QAction(tr("属性"),this);
        property__1 = new QAction(tr("属性"),this);

        open__1 = new QAction(tr("打开"),this);
}

void MainWindow::createFileSystem(){
    ui->treeWidget_explorer->setColumnCount(3);
    ui->treeWidget_explorer->setColumnWidth(0,300);
    QStringList headers;
    headers <<QString("Name")<<QString("Type")<<QString("Size");
    ui->treeWidget_explorer->setHeaderLabels(headers);

    QStringList rootTextList;
    rootTextList << "C:" << "本地磁盘"<<"";
    QTreeWidgetItem *rootC = new QTreeWidgetItem(ui->treeWidget_explorer, rootTextList);
    this->itemC = rootC;
   // ui->treeWidget_explorer->
   // QStringList rootTextList1;
   // rootTextList1 << "D:" << "本地磁盘"<<"";
   // QTreeWidgetItem *rootD = new QTreeWidgetItem(ui->treeWidget_explorer, rootTextList1);
  //  delete rootD;
   // rootD = NULL;
//    if(rootD == NULL){
   //      qDebug()<<"change 123 error";
   // }
    char *sec__ = (char *)malloc(sizeof(char)*sectionSize);
    int *secNum = (int *)malloc(sizeof(int)*10);;
    char *whole_path__ = (char *)malloc(sizeof(char)*38);
    whole_path__[0] = 'C';
    whole_path__[1] = ':';
    whole_path__[2] = '\0';

    if(getIndexSection(sec__,secNum, whole_path__) == false){
        this->printErrorGui();
        free(sec__);
        free(secNum);
        free(whole_path__);
        return;
    }

    if(sec__[0] == FOLDER_TYPE || sec__[0] == ROOT_TYPE){
        FolderIndex fo ;
        initFolderIndex(&fo);
        extractFolderIndex(sec__, &fo);

        for(int  i=0;i<fo.childNodeNumber;i++) {
                char *name__ = (char *)malloc(sizeof(char)*38);

                if(extractName(name__, fo.childNode[i]) == false){
                    this->printErrorGui();
                    break;
                }
                char *path = (char *)malloc(sizeof(char)*90);

                whole_path__[0] = 'C';
                whole_path__[1] = ':';
                whole_path__[2] = '\0';
                strcpy(path, whole_path__);

                strcat(path,"\\") ;
                strcat(path,name__) ;

                createChildFileSystem(path,rootC);

                free(name__);
                free(path);
        }
    }
    else {
        qDebug()<<"  error1";
    }
    free(sec__);
    free(secNum);
    free(whole_path__);

    if(this->renovate == 0){
        connect(ui->treeWidget_explorer,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                this,SLOT(checkself(QTreeWidgetItem* ,int)));
        connect(ui->treeWidget_explorer, SIGNAL(itemPressed(QTreeWidgetItem *, int)),
                this, SLOT(showMenu(QTreeWidgetItem *, int)));
        connect(ui->treeWidget_explorer, SIGNAL(doubleClicked(QModelIndex)),
                this, SLOT(openFiley()));
        this->renovate++;

    }

    if(this->showHideFileFolder){
        showFileFolder();
    }
    else{
        hideFileFolder();
    }
}

void MainWindow::printErrorGui(){
    QString ss = QString::fromLocal8Bit(errorDesc[errorCode]);
    QMessageBox::warning(this,"",ss,QMessageBox::Cancel);
}

void MainWindow::openFiley(){
    if(!(ui->treeWidget_explorer->currentItem()->text(1) == "文件夹"
            || ui->treeWidget_explorer->currentItem()->text(1) == "本地磁盘")) {

        QString textPath__ = ui->showPath->toPlainText();
        QString fileFormat = textPath__.section(".",-1,-1);
        if(fileFormat == "run") {
            if(newProcess(textPath__.toLatin1().data()))
                QMessageBox::information(NULL,"Congratulations!","进程创建成功",QMessageBox::Ok);
             else {
                printErrorGui();
            }

            ui->lineEdit_processopen->setText("");
        }
        else {
            showFile(textPath__);
            ui->stackedWidget_textEditor->setCurrentIndex(0);
            ui->stackedWidget->setCurrentIndex(4);
        }
    }

    else {
           QMessageBox::warning(this,tr(""),tr("can't open"),QMessageBox::Cancel);

    }

}

void MainWindow::checkself(QTreeWidgetItem *item,int )

{

    ui->showPath->clear(); //将showPath内容清空

    /*得到文件路径*/

    QStringList filepath;

    QTreeWidgetItem *itemfile=item; //获取被点击的item
    if(itemfile != NULL){
        fileName = itemfile->text(0); //获取itemfile名称
      //  qDebug()<<itemfile->text(0)<<itemfile->text(1)<<itemfile->text(2);
    }
    while(itemfile!=NULL)

    {

        filepath<<itemfile->text(0); //获取itemfile名称

        itemfile=itemfile->parent(); //将itemfile指向父item

    }

    QString strpath = "";

    //cout<<filepath.size()<<endl;

    for(int i=(filepath.size()-1);i>=0;i--) //QStringlist类filepath反向存着初始item的路径

    { //将filepath反向输出，相应的加入’/‘

        strpath+=filepath.at(i);

        if(i!=0)

            strpath+="\\";

    }

    ui->showPath->insertPlainText(strpath); //将路径显示到plainTextEdit中

}

void MainWindow::clay(QTreeWidgetItem *haveChild,QTreeWidgetItem *source_item,bool cut){
    //qDebug()<<QString::number(source_item);
    int j=0;
    for(j=0;j<source_item->childCount();j++){
        QStringList TextList;
        TextList << source_item->child(j)->text(0) <<
                  source_item->child(j)->text(1)<<
                  source_item->child(j)->text(2);
        QTreeWidgetItem *copy1 = new QTreeWidgetItem(haveChild, TextList);
        haveChild->addChild(copy1);



        for(int i=0;i<hideFileFolderTable.size();i++){
            checkself(hideFileFolderTable[i].itself,0);
            hideFileFolderTable[i].path = ui->showPath->toPlainText();
            checkself(source_item->child(j),0);

            if(hideFileFolderTable[i].path == ui->showPath->toPlainText()){

                hideFileFolderItem item;
                item.itself = copy1;
                checkself(copy1,0);
                item.path = ui->showPath->toPlainText();

                item.type = hideFileFolderTable[i].type;
                item.hideChild = hideFileFolderTable[i].hideChild;
                if(cut){
                    hideFileFolderTable.erase(hideFileFolderTable.begin()+i);
                }

                hideFileFolderTable.push_back(item);
                break;
            }
        }
        clay(copy1,source_item->child(j),cut);
    }
}

void MainWindow::changeChildAtt(char *__path,bool __hideOrProtect,int type){

    char *sec__ = (char *)malloc(sizeof(char)*sectionSize);
    int *secNum__ = (int *)malloc(sizeof(int)*10);
    if(getIndexSection(sec__,secNum__, __path) == false){
        this->printErrorGui();
        free(sec__);
        free(secNum__);
        return;
    }
   // QStringList rootTextList;
   // rootTextList << "" << ""<<"";

    if(sec__[0] == FOLDER_TYPE){
       // QTreeWidgetItem *child__ = new QTreeWidgetItem(parent, rootTextList);
        FolderIndex fo1 ;
        initFolderIndex(&fo1);
        extractFolderIndex(sec__, &fo1);

        if(type == 0){
            FolderAttribute folder;
            folder.protect = fo1.attribute.protect;
            folder.hide = __hideOrProtect;

            if(setFolderAttribute(folder,__path)){
                for(int  i=0;i<fo1.childNodeNumber;i++) {
                        char *name__ = (char *)malloc(sizeof(char)*38);

                        if(extractName(name__, fo1.childNode[i]) == false){
                            this->printErrorGui();
                            break;
                        }
                        char *path = (char *)malloc(sizeof(char)*90);;

                        strcpy(path, __path);

                        strcat(path,"\\") ;
                        strcat(path,name__) ;
                        changeChildAtt(path,__hideOrProtect,type);
                        free(name__);
                        free(path);
                }
            }
            else{
                this->printErrorGui();
            }

        }
        else if(type == 1){
            FolderAttribute folder;
            folder.protect = __hideOrProtect;
            folder.hide = fo1.attribute.hide;

            if(setFolderAttribute(folder,__path)){
                for(int  i=0;i<fo1.childNodeNumber;i++) {
                        char *name__ = (char *)malloc(sizeof(char)*38);

                        if(extractName(name__, fo1.childNode[i]) == false){
                            this->printErrorGui();
                            break;
                        }
                        char *path = (char *)malloc(sizeof(char)*90);;

                        strcpy(path, __path);

                        strcat(path,"\\") ;
                        strcat(path,name__) ;
                        changeChildAtt(path,__hideOrProtect,type);
                        free(name__);
                        free(path);
                }
            }
            else{
                this->printErrorGui();
            }
        }
        else{
            qDebug()<<"change att error";
        }

    }
    else if(sec__[0] == FILE_TYPE){
        if(type == 0){
           // QTreeWidgetItem *child__ = new QTreeWidgetItem(parent, rootTextList);
            FileIndex file;
            initFileIndex(&file);
            extractFileIndex(sec__,&file);

            FileAttribute ff;
            ff.hide = __hideOrProtect;
            ff.readOnly = file.attribute.readOnly;

            if(setFileAttribute(ff,__path)){

            }
            else{
                this->printErrorGui();
            }

        }

    }
    else{
        qDebug()<<"error23";
    }
    free(sec__);
    free(secNum__);
}

void MainWindow::onFolderMenuTriggered(QAction *action){
    if(action == create__){   //创建
        createFileFolder create;
        create.exec();
        if(create.getType() == 0){  //创建文件

            QString str = create.getName(); //QString转char *
            QByteArray ba = str.toLatin1();
            char *Allname__ = ba.data();

            QByteArray ba1 = ui->showPath->toPlainText().toLatin1();
            char *path__ = ba1.data();

           // char *path__ = (char *)malloc(sizeof(char)*90);
          // strcpy(path__, path12);

           // strcat(path__,"\\") ;
          //  strcat(path__,Allname__) ;

            if( newFile(Allname__, path__) ){
                QStringList rootTextList;
                char *__name = (char *)malloc(sizeof(char) * 37);
                char *__ex = (char *)malloc(sizeof(char) * 37);
                extractFileName(__name, __ex,Allname__);
                if(QString(QLatin1String(__ex)) == ""){
                      rootTextList << create.getName() << "文件" <<"0字节";
                }
                else
                      rootTextList << create.getName() << QString(QLatin1String(__ex))+"文件"<<"0字节";
                QTreeWidgetItem *r2 = ui->treeWidget_explorer->currentItem();
                QTreeWidgetItem *r1 = new QTreeWidgetItem(r2, rootTextList);
                r2->addChild(r1);
                QMessageBox::information(this,tr(""),tr("创建成功"));
                free(__name);
                free(__ex);
            }
            else
                this->printErrorGui();
          //  free(path__);
        }
        else if(create.getType() == 1){ //创建文件夹
            QByteArray ba = create.getName().toLatin1();
            char *Allname__ = ba.data();
            QByteArray ba1 = ui->showPath->toPlainText().toLatin1();
            char *path__ = ba1.data();

          //  char *path__ = (char *)malloc(sizeof(char)*90);
          //  strcpy(path__, path123);

           // strcat(path__,"\\") ;
          //  strcat(path__,Allname__) ;
            if(newFolder(Allname__,path__) ){
                QStringList TextList;
                char *__name = (char *)malloc(sizeof(char) * 37);
                char *__ex = (char *)malloc(sizeof(char) * 37);
                extractFileName(__name, __ex,Allname__);
                TextList << create.getName() << "文件夹"<<"空文件夹";
                QTreeWidgetItem *r3 = ui->treeWidget_explorer->currentItem();
                QTreeWidgetItem *r1 = new QTreeWidgetItem(ui->treeWidget_explorer->currentItem(), TextList);
                r3->addChild(r1);
                QMessageBox::information(this,tr(""),tr("创建成功"));
                free(__name);
                free(__ex);
            }

            else
                this->printErrorGui();
            free(path__);
        }
        else if(create.getType() == 2){  //取消创建

            return;
        }
        ui->treeWidget_explorer->currentItem()->setText(2,"");

    }
    else if(action == clay__ ){  //粘贴//bool contains(const QString &str, Qt::CaseSensitivity cs = Qt::CaseSensitive) const;
         if(this->source_item != NULL){

             checkself(this->source_item,0);
             this->source_path = ui->showPath->toPlainText();
             checkself(ui->treeWidget_explorer->currentItem(),0);
         }
         else{
             QMessageBox::information(this,tr(""),tr("源文件或文件夹不存在，无法粘贴"));
             return;
         }

         /*if(ui->showPath->toPlainText()==this->source_path){
             QMessageBox::information(this,tr(""),tr("目标文件夹是源文件夹的子文件夹"));
             return;
         }
         if(ui->showPath->toPlainText().contains(this->source_path+"\\",Qt::CaseSensitive)){
             QMessageBox::information(this,tr(""),tr("目标文件夹是源文件夹的子文件夹"));
             return;
         }*/

         if(this->type == 0){  //代表剪切文件

             if(this->clay_time >  0){
                 QMessageBox::information(this,tr(""),tr("文件已经剪切，无法再粘贴"));
                 return;
             }

             QByteArray ba = this->source_path.toLatin1();
             char *sourcePath = ba.data();
             QByteArray ba1 = ui->showPath->toPlainText().toLatin1();
             char *descPath = ba1.data();

             if(moveFileFolder(descPath,sourcePath)){

                   if(this->source_item != NULL){
                       QStringList TextList;
                       TextList << this->source_item->text(0) <<
                                 this->source_item->text(1)<<
                                 this->source_item->text(2);
                       QTreeWidgetItem *copy1 = new QTreeWidgetItem(ui->treeWidget_explorer->currentItem(), TextList);
                       ui->treeWidget_explorer->currentItem()->addChild(copy1);
                       delete this->source_item;
                       this->source_item = NULL;
                       QMessageBox::information(this,tr(""),tr("剪切文件成功"));
                       for(int i=0;i<hideFileFolderTable.size();i++){
                           checkself(hideFileFolderTable[i].itself,0);
                           hideFileFolderTable[i].path = ui->showPath->toPlainText();
                           checkself(ui->treeWidget_explorer->currentItem(),0);
                           if(hideFileFolderTable[i].path == this->source_path){
                               hideFileFolderTable.erase(hideFileFolderTable.begin()+i);
                               hideFileFolderItem item;
                               item.itself = copy1;
                               item.path = ui->showPath->toPlainText();

                               item.type = FILE_TYPE;
                               item.hideChild = false;
                               hideFileFolderTable.push_back(item);
                               break;
                           }
                       }
                       this->clay_time++;
                   }
                   else
                       QMessageBox::information(this,tr(""),tr("原文件已不存在"));

             }
             else
                   this->printErrorGui();
         }
         else if(this->type == 1){  //代表剪切文件夹
             if(this->clay_time >  0){
                 QMessageBox::information(this,tr(""),tr("文件夹已经剪切，无法再粘贴"));
                 return;
             }

             QByteArray ba = this->source_path.toLatin1();
             char *sourcePath = ba.data();
             QByteArray ba2 = ui->showPath->toPlainText().toLatin1();
             char *descPath = ba2.data();
             if(moveFileFolder(descPath, sourcePath)){
                 //  ui->treeWidget_explorer->currentItem()->addChild(this->source_item);
                   QStringList TextList;

                   if(this->source_item != NULL){
                       TextList << this->source_item->text(0) <<
                               this->source_item->text(1)<<
                               this->source_item->text(2);
                       QTreeWidgetItem *clayRoot = new QTreeWidgetItem(ui->treeWidget_explorer->currentItem(), TextList);
                       ui->treeWidget_explorer->currentItem()->addChild(clayRoot);
                       clay(clayRoot,this->source_item,true); //构建子节点
                       checkself(ui->treeWidget_explorer->currentItem(),0);

                     //  qDebug()<<clayRoot->text(0);
                       delete this->source_item;
                       this->source_item = NULL;
                       QMessageBox::information(this,tr(""),tr("剪切文件夹成功"));
                       this->clay_time++;

                       for(int i=0;i<hideFileFolderTable.size();i++){
                           checkself(hideFileFolderTable[i].itself,0);
                           hideFileFolderTable[i].path = ui->showPath->toPlainText();
                           checkself(ui->treeWidget_explorer->currentItem(),0);

                           if(hideFileFolderTable[i].path == this->source_path){

                               hideFileFolderItem item;
                               item.itself = clayRoot;
                               item.path = ui->showPath->toPlainText();

                               item.type = FOLDER_TYPE;
                               item.hideChild = hideFileFolderTable[i].hideChild;
                               hideFileFolderTable.erase(hideFileFolderTable.begin()+i);
                               hideFileFolderTable.push_back(item);
                               break;
                           }
                       }
                   }
                   else{
                       QMessageBox::information(this,tr(""),tr("原文件夹已不存在"));
                   }

             }
             else
                   this->printErrorGui();
         }
         else if(this->type == 2){  //代表复制文件
             QByteArray ba = this->source_path.toLatin1();
             char *sourcePath = ba.data();
             QByteArray ba1 = ui->showPath->toPlainText().toLatin1();
             char *descPath = ba1.data();
             if(copyFile(descPath, sourcePath)){
                  // this->source_item
                  // ui->treeWidget_explorer->currentItem()->addChild(this->source_item);
                 if(this->source_item != NULL){
                     QStringList TextList;
                     TextList << this->source_item->text(0) <<
                                 this->source_item->text(1)<<
                                 this->source_item->text(2);
                     QTreeWidgetItem *copy = new QTreeWidgetItem(ui->treeWidget_explorer->currentItem(), TextList);
                     ui->treeWidget_explorer->currentItem()->addChild(copy);

                     QMessageBox::information(this,tr(""),tr("复制文件成功"));

                     for(int i=0;i<hideFileFolderTable.size();i++){
                         checkself(hideFileFolderTable[i].itself,0);
                         hideFileFolderTable[i].path = ui->showPath->toPlainText();
                         checkself(ui->treeWidget_explorer->currentItem(),0);
                         if(hideFileFolderTable[i].path == this->source_path){

                             hideFileFolderItem item;
                             item.itself = copy;
                             item.path = ui->showPath->toPlainText();

                             item.type = hideFileFolderTable[i].type;
                             item.hideChild = hideFileFolderTable[i].hideChild;
                             hideFileFolderTable.push_back(item);
                         }
                     }
                 }
                 else{
                     QMessageBox::information(this,tr(""),tr("原文件已不存在"));
                 }

             }
             else
                 this->printErrorGui();
         }
         else if(this->type == 3){  //代表复制文件夹
             QByteArray ba = this->source_path.toLatin1();
             char *sourcePath = ba.data();
             QByteArray ba3 = ui->showPath->toPlainText().toLatin1();
             char *descPath = ba3.data();
             if(copyFolder(descPath, sourcePath)){
                 //  ui->treeWidget_explorer->currentItem()->addChild(this->source_item);
                 if(this->source_item != NULL){
                     QStringList TextList;
                     TextList << this->source_item->text(0) <<
                           this->source_item->text(1)<<
                           this->source_item->text(2);
                     QTreeWidgetItem *clayRoot = new QTreeWidgetItem(ui->treeWidget_explorer->currentItem(), TextList);

                     ui->treeWidget_explorer->currentItem()->addChild(clayRoot);
                     clay(clayRoot,this->source_item,false); //构建子节点
                     checkself(ui->treeWidget_explorer->currentItem(),0);

                     QMessageBox::information(this,tr(""),tr("复制文件夹成功"));

                     for(int i=0;i<hideFileFolderTable.size();i++){
                         checkself(hideFileFolderTable[i].itself,0);
                         hideFileFolderTable[i].path = ui->showPath->toPlainText();
                         checkself(ui->treeWidget_explorer->currentItem(),0);
                         if(hideFileFolderTable[i].path == this->source_path){

                             hideFileFolderItem item;
                             item.itself = clayRoot;
                             item.path = ui->showPath->toPlainText();

                             item.type = hideFileFolderTable[i].type;
                             item.hideChild = hideFileFolderTable[i].hideChild;
                             hideFileFolderTable.push_back(item);
                         }
                     }
                 }
                 else{
                     QMessageBox::information(this,tr(""),tr("原文件夹已不存在"));
                 }

             }
             else
                 this->printErrorGui();
         }
         else{
             QMessageBox::information(this,tr(""),tr("请先选择要粘贴的合法内容"));
             return;
         }
         ui->treeWidget_explorer->currentItem()->setText(2,""); //文件夹不为空

    }
    else if(action == shear__){  //剪切
          this->clay_time = 0;
          this->source_path = ui->showPath->toPlainText();
          this->source_item = ui->treeWidget_explorer->currentItem();
          if(ui->treeWidget_explorer->currentItem()->data(1,0).toString() == QString::fromStdString("本地磁盘") ){
                       QMessageBox::information(this,tr(""),tr("不能剪切本地磁盘"));
                       this->type = 4;
          }
          else if(ui->treeWidget_explorer->currentItem()->data(1,0).toString() == QString::fromStdString("文件夹") )
                       this->type = 1;
          else
                       this->type = 0;
    }
    else if(action == copy__){  //复制
          this->clay_time = 0;
          this->source_path = ui->showPath->toPlainText();
          this->source_item = ui->treeWidget_explorer->currentItem();
          if(ui->treeWidget_explorer->currentItem()->data(1,0).toString() == QString::fromStdString("本地磁盘")){
                       QMessageBox::information(this,tr(""),tr("不能复制本地磁盘"));
                       this->type = 4;
          }
          else if(ui->treeWidget_explorer->currentItem()->data(1,0).toString() == QString::fromStdString("文件夹") )
                       this->type = 3;
          else
                       this->type = 2;
    }
    else if(action == delete__){  //删除
        showDelete __delete ;
        __delete.exec();
         if(__delete.getIsDelete()){

             QByteArray ba = ui->showPath->toPlainText().toLatin1();
             char *sourcePath = ba.data();
             if(deleteFileFolder(sourcePath)){

                 if(this->source_item !=NULL){
                     checkself(this->source_item,0);
                     this->source_path = ui->showPath->toPlainText();
                     checkself(ui->treeWidget_explorer->currentItem(),0);
                     if(this->source_path == ui->showPath->toPlainText()){
                         this->source_item = NULL;    //**************
                     }
                     if(this->source_path.contains(this->source_path+"\\",Qt::CaseSensitive)){
                         this->source_item = NULL;
                     }
                 }

                 for(int i=0;i<hideFileFolderTable.size();i++){
                     checkself(hideFileFolderTable[i].itself,0);
                     hideFileFolderTable[i].path = ui->showPath->toPlainText();
                     checkself(ui->treeWidget_explorer->currentItem(),0);
                     if(hideFileFolderTable[i].path == ui->showPath->toPlainText()){
                         hideFileFolderTable.erase(hideFileFolderTable.begin()+i);
                        // i--;     111111111111111111
                     }
                     if(i>=hideFileFolderTable.size())
                         break;
                     if(hideFileFolderTable[i].path.contains(ui->showPath->toPlainText()+"\\",Qt::CaseSensitive) ){
                         hideFileFolderTable.erase(hideFileFolderTable.begin()+i);
                         i--;
                        //ui->showPath->toPlainText().contains(this->source_path+"\\",Qt::CaseSensitive)
                     }

                 }

                 delete ui->treeWidget_explorer->currentItem();
                 //ui->treeWidget_explorer->currentItem() = NULL;
                 QMessageBox::information(this,tr(""),tr("成功删除"));

                 checkself(ui->treeWidget_explorer->currentItem(),0);
             }


             else{
                 this->printErrorGui();
             }
         }
         else{  //取消删除

         }
    }
    else if(action == rename__){  //重命名
        showNewName name;
        name.exec();
        QString newName__ = name.getNewName();
        if(newName__ != ""){ //ui->treeWidget_explorer->currentItem()->d
            QByteArray ba = ui->showPath->toPlainText().toLatin1();
            char *sourcePath = ba.data();
            QByteArray ba1 = newName__.toLatin1();
            char *name1 = ba1.data();

            if(renameFileFolder(name1,sourcePath)){
                ui->treeWidget_explorer->currentItem()->setText(0,newName__);

                checkself(ui->treeWidget_explorer->currentItem(),0);
            }
            else{
                this->printErrorGui();
            }
        }

    }
    else if(action == property__){  //属性

        strcpy(showAttributePath, "");
        QByteArray ba = ui->showPath->toPlainText().toLatin1();
        showAttributePath = ba.data();

        showAttribute s;
        s.exec();

        if(ui->showPath->toPlainText() != "C:"){
                    if(!s.getCancelOperate()){  //取消操作

                        if(!(s.getOriginalHide() == s.getHide() && s.getOriginalProtect() == s.getProtect())){
                            FolderAttribute folder;
                            folder.hide = s.getHide();
                            folder.protect = s.getProtect();

                           // if(setFileAttribute(FileAttribute __attr, const char * __path))
                            if(setFolderAttribute(folder,showAttributePath)){
                                if(s.getOriginalHide() != s.getHide()){
                                    if(s.getHide()){
                                        hideFileFolderItem item;
                                        item.itself = ui->treeWidget_explorer->currentItem();
                                        item.path = QString(QLatin1String(showAttributePath));
                                        item.type = FOLDER_TYPE;
                                        item.hideChild = s.getChild();
                                        hideFileFolderTable.push_back(item);

                                        if(s.getChild()){
                                            for(int i=0;i<hideFileFolderTable.size();i++){
                                                checkself(hideFileFolderTable[i].itself,0);
                                                hideFileFolderTable[i].path = ui->showPath->toPlainText();
                                                checkself(ui->treeWidget_explorer->currentItem(),0);
                                                if(hideFileFolderTable[i].path.contains(ui->showPath->toPlainText()+"\\",Qt::CaseSensitive)){
                                                    hideFileFolderTable.erase(hideFileFolderTable.begin()+i);
                                                    i--;
                                                }

                                            }
                                            joinChildHide(ui->treeWidget_explorer->currentItem());
                                        }

                                      //  if(ui->showPath->toPlainText().contains(this->source_path+"\\",Qt::CaseSensitive)){
                                         //   QMessageBox::information(this,tr(""),tr("目标文件夹是源文件夹的子文件夹"));
                                       //     return;
                                      //  }
                                    }
                                    else{
                                        for(int i=0;i<hideFileFolderTable.size();i++){
                                            checkself(hideFileFolderTable[i].itself,0);
                                            hideFileFolderTable[i].path = ui->showPath->toPlainText();
                                            checkself(ui->treeWidget_explorer->currentItem(),0);
                                            if(hideFileFolderTable[i].path == ui->showPath->toPlainText()){
                                                hideFileFolderTable.erase(hideFileFolderTable.begin()+i);
                                                i--;
                                            }

                                        }
                                        if(s.getChild()){
                                            for(int i=0;i<hideFileFolderTable.size();i++){
                                                checkself(hideFileFolderTable[i].itself,0);
                                                hideFileFolderTable[i].path = ui->showPath->toPlainText();
                                                checkself(ui->treeWidget_explorer->currentItem(),0);

                                                if(hideFileFolderTable[i].path.contains(ui->showPath->toPlainText()+"\\",Qt::CaseSensitive)){
                                                    hideFileFolderTable.erase(hideFileFolderTable.begin()+i);
                                                    i--;
                                                }

                                            }

                                        }
                                    }//if(s.getHide())
                                }//if(s.getOriginalHide() != s.getHide())


                                if(s.getChild()){
                                        char *sec__ = (char *)malloc(sizeof(char)*sectionSize);
                                        int  *secNum = (int *)malloc(sizeof(int)*10);

                                        if(getIndexSection(sec__,secNum, showAttributePath) == false){
                                            this->printErrorGui();
                                            free(sec__);
                                            free(secNum);
                                            return;
                                        }

                                        if(sec__[0] == FOLDER_TYPE){
                                            FolderIndex fo ;
                                            initFolderIndex(&fo);
                                            extractFolderIndex(sec__, &fo);

                                            for(int  i=0;i<fo.childNodeNumber;i++) {
                                                    char *name__ = (char *)malloc(sizeof(char)*38);

                                                    if(extractName(name__, fo.childNode[i]) == false){
                                                        this->printErrorGui();
                                                        break;
                                                    }
                                                    char *path = (char *)malloc(sizeof(char)*90);;

                                                    strcpy(path, showAttributePath);

                                                    strcat(path,"\\") ;
                                                    strcat(path,name__) ;

                                                    if(s.getOriginalHide() != s.getHide()){
                                                        changeChildAtt(path,s.getHide(),0);
                                                    }
                                                    if(s.getOriginalProtect() != s.getProtect()){
                                                        changeChildAtt(path,s.getProtect(),1);
                                                    }
                                                    changeChildAtt(path,s.getHide(),0);
                                                    free(name__);
                                                    free(path);
                                            }
                                        }
                                        else {
                                            qDebug()<<"  error1";
                                        }//if(sec__[0] == FOLDER_TYPE)
                                        free(sec__);
                                        free(secNum);

                                }//if(s.getChild())

                            }
                            else{
                                this->printErrorGui();
                            }//end of if(setFolderAttribute(folder,showAttributePath))
                        }//if(!(s.getOriginalHide() == s.getHide() && s.getOriginalProtect() == s.getProtect()))

                    }//if(!s.getCancelOperate()){  //取消操作


        }//if(ui->showPath->toPlainText() != "C:")

        QString newName__ = s.getNewName();
        if(newName__ != "" && newName__ != "C"){
            QByteArray ba = ui->showPath->toPlainText().toLatin1();
            char *sourcePath = ba.data();
            QByteArray ba1 = newName__.toLatin1();
            char *name = ba1.data();

            if(renameFileFolder(name,sourcePath)){
                ui->treeWidget_explorer->currentItem()->setText(0,newName__);
                checkself(ui->treeWidget_explorer->currentItem(),0);
            }
            else{
                this->printErrorGui();
            }
           // free(name);
        }
        if(newName__ == "")
            QMessageBox::information(this,tr(""),tr("名字不能为空"));
    }
    if(this->showHideFileFolder){
        showFileFolder();
    }
    else{
        hideFileFolder();
    }
}

void MainWindow::joinChildHide(QTreeWidgetItem *Child){
    for(int i=0;i<Child->childCount();i++){
        hideFileFolderItem item;
        item.itself = Child->child(i);
        item.path = "";
        item.type = FOLDER_TYPE;
        item.hideChild = false;
        hideFileFolderTable.push_back(item);
        joinChildHide(Child->child(i));
    }

}

void MainWindow::onFileMenuTriggered(QAction *action)
{
    if(action == open__1){
       QString textPath__ = ui->showPath->toPlainText();
       QString fileFormat = textPath__.section(".",-1,-1);
       if(fileFormat == "run") {
           if(newProcess(textPath__.toLatin1().data()))
               QMessageBox::information(NULL,"Congratulations!","进程创建成功",QMessageBox::Ok);
            else
               QMessageBox::information(NULL,"Warning!","进程创建失败",QMessageBox::Ok);
           ui->lineEdit_processopen->setText("");
       }
       else {
           showFile(textPath__);
           ui->stackedWidget_textEditor->setCurrentIndex(0);
           ui->stackedWidget->setCurrentIndex(4);
       }
    }
    else if (action == shear__1)  //剪切
    {
         this->clay_time = 0;
         this->source_path = ui->showPath->toPlainText();
         this->source_item = ui->treeWidget_explorer->currentItem();
         this->type = 0;
        //qDebug()<<"jianqi";
    }
    else if (action == copy__1)  //复制
    {
        this->clay_time = 0;
        this->source_path = ui->showPath->toPlainText();
        this->source_item = ui->treeWidget_explorer->currentItem();
        this->type = 2;
    }
    else if(action == delete__1)  //删除
    {
        showDelete __delete;
        __delete.exec();
        if(__delete.getIsDelete()){
            QByteArray ba = ui->showPath->toPlainText().toLatin1();
            char *sourcePath = ba.data();

            if(deleteFileFolder(sourcePath)){
                if(this->source_item != NULL){
                    checkself(this->source_item,0);
                    this->source_path = ui->showPath->toPlainText();
                    checkself(ui->treeWidget_explorer->currentItem(),0);
                    if(this->source_path == ui->showPath->toPlainText()){
                        this->source_item = NULL;
                    }

                }
                for(int i=0;i<hideFileFolderTable.size();i++){
                    checkself(hideFileFolderTable[i].itself,0);
                    hideFileFolderTable[i].path = ui->showPath->toPlainText();
                    checkself(ui->treeWidget_explorer->currentItem(),0);
                    if(hideFileFolderTable[i].path == ui->showPath->toPlainText()){
                        hideFileFolderTable.erase(hideFileFolderTable.begin()+i);
                        i--;
                    }

                }
                delete ui->treeWidget_explorer->currentItem();
                QMessageBox::information(this,tr(""),tr("成功删除"));


                checkself(ui->treeWidget_explorer->currentItem(),0);
            }


            else{
                this->printErrorGui();

            }
        }
        else{  //取消删除

        }
    }
    else if(action == rename__1)  //重命名
    {
        showNewName name1;
        name1.exec();
        QString newName__ = name1.getNewName();
        if(newName__ != ""){ //ui->treeWidget_explorer->currentItem()->d
            QByteArray ba = ui->showPath->toPlainText().toLatin1();
            char *sourcePath = ba.data();
            QByteArray ba1 = newName__.toLatin1();
            char *name = ba1.data();

            if(renameFileFolder(name,sourcePath)){

                ui->treeWidget_explorer->currentItem()->setText(0,newName__);

                if(ui->treeWidget_explorer->currentItem()->text(1) != "文件夹"){

                    char *__name = (char *)malloc(sizeof(char) * 37);
                    char *__ex = (char *)malloc(sizeof(char) * 37);
                    extractFileName(__name, __ex,name);
                    if(__ex == NULL){
                        ui->treeWidget_explorer->currentItem()->setText(1,"文件");
                    }
                    else{
                        ui->treeWidget_explorer->currentItem()->setText(1,QString(QLatin1String(__ex))+"文件");
                    }

                    free(__name);
                    free(__ex);

                }
                checkself(ui->treeWidget_explorer->currentItem(),0);

            }
            else{
                this->printErrorGui();
            }
        }
    }
    else if(action == property__1)  //属性
    {
        strcpy(showAttributePath, "");
        QByteArray ba = ui->showPath->toPlainText().toLatin1();
        showAttributePath = ba.data();

        showFileAtt s;
        s.exec();

        if(!(s.getOriginalHide() == s.getHide() && s.getOriginalReadonly() == s.getReadOnly())){
            FileAttribute fileAtt;
            fileAtt.hide = s.getHide();
            fileAtt.readOnly = s.getReadOnly();

            if(setFileAttribute(fileAtt, showAttributePath)){
                if(s.getOriginalHide() != s.getHide()){
                    if(s.getHide()){
                        hideFileFolderItem item;
                        item.itself = ui->treeWidget_explorer->currentItem();
                        item.path = QString(QLatin1String(showAttributePath));
                        item.type = FILE_TYPE;
                        item.hideChild = false;
                        hideFileFolderTable.push_back(item);
                    }
                    else{
                        for(int i=0;i<hideFileFolderTable.size();i++){
                            checkself(hideFileFolderTable[i].itself,0);
                            hideFileFolderTable[i].path = ui->showPath->toPlainText();
                            checkself(ui->treeWidget_explorer->currentItem(),0);
                            if(hideFileFolderTable[i].path == ui->showPath->toPlainText()){
                                hideFileFolderTable.erase(hideFileFolderTable.begin()+i);
                                i--;
                            }

                        }

                    }
                }
            }
            else
                this->printErrorGui();
        }

        QString newName__ = s.getNewName();
        if(newName__ == "")
            QMessageBox::information(this,tr(""),tr("名字不能为空"));

        if(newName__ != ""){ //ui->treeWidget_explorer->currentItem()->d
            QByteArray ba = ui->showPath->toPlainText().toLatin1();
            char *sourcePath = ba.data();
            QByteArray ba1 = newName__.toLatin1();
            char *name = ba1.data();

            if(renameFileFolder(name,sourcePath)){

                ui->treeWidget_explorer->currentItem()->setText(0,newName__);

                if(ui->treeWidget_explorer->currentItem()->text(1) != "文件夹"){

                    char *__name = (char *)malloc(sizeof(char) * 37);
                    char *__ex = (char *)malloc(sizeof(char) * 37);
                    extractFileName(__name, __ex,name);
                    if(__ex == NULL){
                        ui->treeWidget_explorer->currentItem()->setText(1,"文件");
                    }
                    else{
                        ui->treeWidget_explorer->currentItem()->setText(1,QString(QLatin1String(__ex))+"文件");
                    }

                    free(__name);
                    free(__ex);

                }

                checkself(ui->treeWidget_explorer->currentItem(),0);

            }
            else{
                this->printErrorGui();
            }
            free(name);
        }



    }
    if(this->showHideFileFolder){
        showFileFolder();
    }
    else{
        hideFileFolder();
    }
}

void MainWindow::showMenu(QTreeWidgetItem *item__, int)
{
    if (qApp->mouseButtons() == Qt::LeftButton){ //左键
        return;
       // qApp->doubleClickInterval()
    }

    QTreeWidgetItem *item = item__;
    if(item == NULL)
    {
        //qDebug()<<"空白处";
         //   Menu__->clear();
       //     Menu__->menuAction()->setVisible(false);
        //菜单出现的位置为当前鼠标的位置
     //   Menu__->exec(QCursor::pos());

    }
    else
    {
        checkself(item,0);
       // qDebug()<<ui->treeWidget_explorer->currentItem()->data(1,0).toString();
        if(ui->treeWidget_explorer->currentItem()->childCount() > 0 ||
                ui->treeWidget_explorer->currentItem()->data(1,0).toString() == QString::fromStdString("本地磁盘") ||
                ui->treeWidget_explorer->currentItem()->data(1,0).toString() == QString::fromStdString("文件夹") )
        {
            this->isFile = false;
            if (folderMenu__->isEmpty())
            {
                folderMenu__->addAction(create__);
                folderMenu__->addAction(clay__);
                folderMenu__->addAction(shear__);
                folderMenu__->addAction(copy__);
                folderMenu__->addAction(delete__);
                folderMenu__->addAction(rename__);
                folderMenu__->addAction(property__);
            }
            folderMenu__->exec(QCursor::pos());
        }

        else{
            this->isFile = true;
            if (fileMenu__->isEmpty())
            {

                fileMenu__->addAction(open__1);
                fileMenu__->addAction(shear__1);
                fileMenu__->addAction(copy__1);
                fileMenu__->addAction(delete__1);
                fileMenu__->addAction(rename__1);
                fileMenu__->addAction(property__1);

            }
            fileMenu__->exec(QCursor::pos());
        }

    }

    //event->accept();

}

void showOrHide(QTreeWidgetItem *item,bool hide){
     for(int i=0;i<item->childCount();i++){
         item->child(i)->setHidden(hide);
         showOrHide(item->child(i),hide);
     }
 }

 void MainWindow::showFileFolder(){ //显示文件和文件
     for(int i=0 ;i<hideFileFolderTable.size();i++){
         if(hideFileFolderTable[i].itself != NULL){

             hideFileFolderTable[i].itself->setHidden(false);
         }
     }
 }

 void MainWindow::hideFileFolder(){ //隐藏文件和文件夹
     for(int i=0;i<hideFileFolderTable.size();i++){
         if(hideFileFolderTable[i].itself != NULL){
             hideFileFolderTable[i].itself->setHidden(true);

         }

     }
 }

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if(arg1 == 0){
        hideFileFolder();
        QMessageBox::information(this,tr(""),tr("隐藏文件和文件夹"));
        this->showHideFileFolder = false;
    }
    else{
        showFileFolder();
        QMessageBox::information(this,tr(""),tr("显示文件和文件"));
        this->showHideFileFolder = true;
    }

}

QTreeWidgetItem* MainWindow::FindItem(QTreeWidgetItem * item,char * __path){

            checkself(item,0);
            QString path = ui->showPath->toPlainText();
           // checkself(ui->treeWidget_explorer->currentItem(),0);
            if(QString(QLatin1String(__path)) == path){
                return item;
            }

            QTreeWidgetItem *item__=NULL;
            for(int i=0;i<item->childCount();i++){
                    item__ = FindItem(item->child(i),__path);
                    if(item__ != NULL){
                        return item__;
                    }
            }
            return item__;


}
void MainWindow::renovateFileAtt(char *__path){  //刷新文件信息
   // ui->treeWidget_explorer->
    QTreeWidgetItem *findItem=NULL;
    if(this->itemC != NULL)
        findItem = FindItem(this->itemC,__path);
    else
        return;
    checkself(ui->treeWidget_explorer->currentItem(),0);
    if(findItem == NULL){
        QMessageBox::information(this,tr(""),tr("路径错误"));
        return;
    }
    char *sec__ = (char *)malloc(sizeof(char)*sectionSize);
    int *secNum__ = (int *)malloc(sizeof(int)*10);

    if(!getIndexSection(sec__,secNum__, __path)){
         this->printErrorGui();
         free(sec__);
         free(secNum__);
         return;
    }

    if(sec__[0] == FILE_TYPE){
         FileIndex file;
         initFileIndex(&file);
         extractFileIndex(sec__,&file);

         char *name__3 = (char *)malloc(sizeof(char)*38);
         char *ex__ = (char *)malloc(sizeof(char)*38);

         extractFileName(name__3,ex__,file.name);

         findItem->setText(0,QString(QLatin1String(file.name)));

         if(QString(QLatin1String(ex__)) == ""){
            // ui->type->setText("文件") ;
             findItem->setText(0,"文件");
         }
        else
            findItem->setText(1,QString(QLatin1String(ex__))+"文件");

         int length = file.length;
         findItem->setText(2,QString::number(length, 10)+"字节");

         free(name__3);
         free(ex__);
    }
    else{
        qDebug()<<"error99";
    }
    free(sec__);
    free(secNum__);
}

void MainWindow::renovateFileSystem(){
    hideFileFolderTable.clear();
    if(this->source_item != NULL){
        checkself(this->source_item,0);
        this->source_path = ui->showPath->toPlainText();
        checkself(ui->treeWidget_explorer->currentItem(),0);
    }

    ui->treeWidget_explorer->clear();
    this->itemC = NULL;
    createFileSystem();
   //qDebug()<<this->source_path;
    if(this->source_item != NULL){
        QByteArray ba = this->source_path.toLatin1();
        char *__path = ba.data();

        if(this->itemC != NULL)
            source_item = FindItem(this->itemC,__path);
        else
            return;
        checkself(ui->treeWidget_explorer->currentItem(),0);
    }


}



/*******************************************************************************************************/
void MainWindow::on_pushButton_detail_clicked()
{

  ui->stackedWidget_2->setCurrentIndex(1);
}

void MainWindow::on_pushButton_processreturn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_newprocess_clicked()
{
     ui->lineEdit_processopen->setText("");
     ui->stackedWidget_2->setCurrentIndex(3);
}

void MainWindow::on_pushButton_deleteprocess_clicked()
{
    if(deleteProcess(PID_temp))
    {
        QMessageBox::warning(NULL,"Congratulations!","进程删除成功",QMessageBox::Cancel);
             ui->tableWidget_pmaintable->clear();
    }
        else
        QMessageBox::warning(NULL,"Warning!","进程删除失败",QMessageBox::Cancel);
}

void MainWindow::on_pushButton_return_clicked()
{
    ju=false;
    ui->stackedWidget_2->setCurrentIndex(1);
}

void MainWindow::on_pushButton_poconfirm_clicked()
{
       QString path;
       path=ui->lineEdit_processopen->text();
       if(newProcess(path.toLatin1().data()))
           QMessageBox::warning(NULL,"Congratulations!","进程创建成功",QMessageBox::Cancel);
        else
           QMessageBox::warning(NULL,"Warning!","进程创建失败",QMessageBox::Cancel);
       ui->lineEdit_processopen->setText("");
}

void MainWindow::on_pushButton_pocancel_clicked()
{
   ui->stackedWidget_2->setCurrentIndex(1);
   ui->lineEdit_processopen->setText("");
}

void MainWindow::showMenu(QTableWidgetItem*item)
{
    infosave_2(item);
        if (qApp->mouseButtons() == Qt::LeftButton){ //左键
                return;
            }//
        QTableWidgetItem *item__=item;
        if(item__==NULL)
        {
        }
        else
        {
            if(proMenu->isEmpty())
            {
            proMenu->addAction(action_detailed);
            proMenu->addAction(action_killed);
            }
            proMenu->exec(QCursor::pos());
        }
}

void MainWindow::onProMenuTriggered(QAction *action)
{
    if(action == action_detailed)
       {
           //createdettab();
           ju=true;
           Sleep(500);
           ui->stackedWidget_2->setCurrentIndex(2);
       }
       else if(action == action_killed)
       {
        if(deleteProcess(PID_temp))
        {
            QMessageBox::warning(NULL,"Congratulations!","进程删除成功",QMessageBox::Cancel);
                 ui->tableWidget_pmaintable->clear();
        }
            else
            QMessageBox::warning(NULL,"Warning!","进程删除失败",QMessageBox::Cancel);
       }
}

void MainWindow::createPItemMenu()
{
    proMenu= new QMenu(ui->tableWidget_pmaintable);
        connect(proMenu, SIGNAL(triggered(QAction *)), this, SLOT(onProMenuTriggered(QAction *)));
        action_detailed=new QAction(tr("显示该进程的详细"),this);
        action_killed=new QAction(tr("杀死该进程"),this);
}

void MainWindow::createProtab()
{

    QStringList headers;
            ui->tableWidget_pmaintable->setEditTriggers(QAbstractItemView::NoEditTriggers);
            ui->tableWidget_pmaintable->setRowCount(1000);     //设置行数为500
            ui->tableWidget_pmaintable->setColumnCount(8);    //设置列数为6
            headers<<"NAME"<<"PID"<<"STATE"<<"PATH"<<"MEMORY"<<"CPU"<<"DP"<<"CODE MEM";
            ui->tableWidget_pmaintable->setHorizontalHeaderLabels(headers);
            ui->tableWidget_pmaintable->setColumnWidth(0,70);
            ui->tableWidget_pmaintable->setColumnWidth(1,70);
            ui->tableWidget_pmaintable->setColumnWidth(2,100);
            ui->tableWidget_pmaintable->setColumnWidth(3,200);
            ui->tableWidget_pmaintable->setColumnWidth(4,100);
            ui->tableWidget_pmaintable->setColumnWidth(5,50);
            ui->tableWidget_pmaintable->setColumnWidth(6,50);
             ui->tableWidget_pmaintable->setColumnWidth(7,100);
            ui->tableWidget_pmaintable->setSelectionBehavior(QAbstractItemView::SelectRows);


            /*ui->tableWidget->setItem(0,0,new QTableWidgetItem("1"));
            ui->tableWidget->setItem(0,1,new QTableWidgetItem("2"));
            ui->tableWidget->setItem(0,2,new QTableWidgetItem("3"));
            ui->tableWidget->setItem(0,3,new QTableWidgetItem("1"));
            ui->tableWidget->setItem(0,4,new QTableWidgetItem("1"));
            ui->tableWidget->setItem(0,5,new QTableWidgetItem("1"));*/
        connect(ui->tableWidget_pmaintable, SIGNAL(itemPressed(QTableWidgetItem*)),
                this, SLOT(showMenu(QTableWidgetItem*)));
        connect(ui->tableWidget_pmaintable, SIGNAL(itemClicked(QTableWidgetItem*)),
                this, SLOT(infosave(QTableWidgetItem*)));
}



 void MainWindow::infosave(QTableWidgetItem*item)
 {
        int row;
        QTableWidgetItem * item__=item;
        row=item__->row();
        PID_temp=ui->tableWidget_pmaintable->item(row,1)->text().toInt();
 }

 void MainWindow::infosave_2(QTableWidgetItem*item)
 {
        int row;
        QTableWidgetItem * item__=item;
        row=item__->row();
        PID_temp=ui->tableWidget_pmaintable->item(row,1)->text().toInt();
 }


 void MainWindow::showProtab()
 {
     PCBQueue * header=processQueue->nextPtr;
          QString temp;
          int i=0;
          QStringList headers;
          int sumlen=getQueueLength(READY)+getQueueLength(WAITING)+getQueueLength(RUNNING)+getQueueLength(TERMINATED)+getQueueLength(NEW);
          ui->tableWidget_pmaintable->setRowCount(sumlen);
          headers<<"NAME"<<"PID"<<"STATE"<<"PATH"<<"MEMORY"<<"CPU"<<"DP"<<"CODE MEM";
          ui->tableWidget_pmaintable->setHorizontalHeaderLabels(headers);
          while (header!=NULL) {

              ui->tableWidget_pmaintable->setItem(i,0,new QTableWidgetItem( QString(QLatin1String(header->pcb->externalInfo.name))));
              ui->tableWidget_pmaintable->setItem(i,1,new QTableWidgetItem(QString::number(header->pcb->pid)));
              if(header->pcb->schedInfo.state==NEW)
                  temp="NEW";
              else if(header->pcb->schedInfo.state==READY)
                  temp="READY";
              else if(header->pcb->schedInfo.state==RUNNING)
                  temp="RUNNING";
              else if(header->pcb->schedInfo.state==WAITING)
                  temp="WAITING";
              else if(header->pcb->schedInfo.state==TERMINATED)
                  temp="TERMINATED";
              ui->tableWidget_pmaintable->setItem(i,2,new QTableWidgetItem(temp));
              ui->tableWidget_pmaintable->setItem(i,3,new QTableWidgetItem(QString(QLatin1String(header->pcb->externalInfo.path))));
              ui->tableWidget_pmaintable->setItem(i,4,new QTableWidgetItem(QString::number(header->pcb->exeInfo.memoryUsed)));
              ui->tableWidget_pmaintable->setItem(i,5,new QTableWidgetItem(QString::number(header->pcb->exeInfo.CPUUsed)));
              ui->tableWidget_pmaintable->setItem(i,6,new QTableWidgetItem(QString::number(header->pcb->schedInfo.dynamicPriority)));
              ui->tableWidget_pmaintable->setItem(i,7,new QTableWidgetItem(QString::number(header->pcb->exeInfo.codeMemory)));
              header=header->nextPtr;
              i++;
          }
 }

 void MainWindow::showdettab()
 {
     PCB * PCB__temp;
     PCB__temp = findPCB(PID_temp);

     if (PCB__temp != NULL&&ju == true)
     {
         //内部标识符
         ui->label_pid->setText(QString::number(PCB__temp->pid, 10));
         //外部标识符
         ui->label_pro->setText(QString(QLatin1String(PCB__temp->externalInfo.name)));
         ui->label_propath->setText(QString(QString(QLatin1String(PCB__temp->externalInfo.path))));
         //状态信息
         ui->label_pcshow->setText(QString::number(PCB__temp->exeInfo.processCounter, 10));
         ui->label_pccpu->setText(QString::number(PCB__temp->exeInfo.codeMemory, 10));
         ui->label_cpu->setText(QString::number(PCB__temp->exeInfo.CPUUsed, 10));
         ui->label_miu->setText(QString::number(PCB__temp->exeInfo.memoryUsed, 10));

         QString createt;
         int year__;
         int month__;
         int day__;
         int hour__;
         int sec__;
         int min__;
         year__ = PCB__temp->schedInfo.time.createTime.year;
         month__ = PCB__temp->schedInfo.time.createTime.month;
         day__ = PCB__temp->schedInfo.time.createTime.day;
         hour__ = PCB__temp->schedInfo.time.createTime.hour;
         min__ = PCB__temp->schedInfo.time.createTime.min;
         sec__ = PCB__temp->schedInfo.time.createTime.sec;
         createt = QString::number(year__, 10) + "/" + QString::number(month__, 10) + "/" + QString::number(day__, 10) + " " + QString::number(hour__, 10) + ":" + QString::number(min__, 10) + ":" + QString::number(sec__, 10);
         ui->label_pct->setText(createt);
         ui->label_pwt->setText(QString::number(PCB__temp->schedInfo.time.waitingTime, 10));
         ui->label_prt->setText(QString::number(PCB__temp->schedInfo.time.runningTime, 10));
         ui->label_pdp->setText(QString::number(PCB__temp->schedInfo.dynamicPriority, 10));
         ui->label_sppr->setText(QString::number(PCB__temp->schedInfo.priority, 10));
         if (PCB__temp->schedInfo.state == NEW)
             ui->label_state->setText("新建");
         else if (PCB__temp->schedInfo.state == READY)
             ui->label_state->setText("就绪");
         else if (PCB__temp->schedInfo.state == RUNNING)
             ui->label_state->setText("运行");
         else if (PCB__temp->schedInfo.state == WAITING)
             ui->label_state->setText("等待");
         else if (PCB__temp->schedInfo.state == TERMINATED)
             ui->label_state->setText("完成");

         if (PCB__temp->schedInfo.event == NEW_PROCESS)
             ui->label_event->setText("新建进程");
         else if (PCB__temp->schedInfo.event == RUN_OUT_OF_TIME_SLICE)
             ui->label_event->setText("时间片耗尽");
         else if (PCB__temp->schedInfo.event == WAITING_FOR_IO)
             ui->label_event->setText("等待IO");
         else if (PCB__temp->schedInfo.event == IO_FINISH)
             ui->label_event->setText("IO完成");
         //页表信息
         ui->label_pn->setText(QString::number(PCB__temp->pageTable.number, 10));
         if (PCB__temp->pageTable.table->isAvailable)
             ui->label_isa->setText("有效");
         else
             ui->label_isa->setText("无效");
         //段的显示
         ui->label_fra->setText(QString::number(PCB__temp->pageTable.table->frameNumber, 10));
         int dslen = PCB__temp->dataSegment.end - PCB__temp->dataSegment.begin + 1;
         ui->label_dslen->setText(QString::number(dslen, 10));
         ui->label_dsuse->setText(QString::number(PCB__temp->dataSegment.used, 10));

         MyHeap * dsheap = PCB__temp->dataSegment.heap->nextPtr;

         ui->label_27->setStyleSheet("");
         ui->label_27->setText("");
         ui->label_28->setStyleSheet("");
         ui->label_28->setText("");
         ui->label_29->setStyleSheet("");
         ui->label_29->setText("");
         ui->label_30->setStyleSheet("");
         ui->label_30->setText("");
         ui->label_31->setStyleSheet("");
         ui->label_31->setText("");
         ui->label_32->setStyleSheet("");
         ui->label_32->setText("");
         ui->label_34->setStyleSheet("");
         ui->label_34->setText("");
         ui->label_35->setStyleSheet("");
         ui->label_35->setText("");
         ui->label_36->setStyleSheet("");
         ui->label_36->setText("");
         ui->label_37->setStyleSheet("");
         ui->label_37->setText("");
         ui->label_38->setStyleSheet("");
         ui->label_38->setText("");
         ui->label_39->setStyleSheet("");
         ui->label_39->setText("");
         ui->label_40->setStyleSheet("");
         ui->label_40->setText("");
         ui->label_41->setStyleSheet("");
         ui->label_41->setText("");
         ui->label_42->setStyleSheet("");
         ui->label_42->setText("");
         ui->label_43->setStyleSheet("");
         ui->label_43->setText("");
         ui->label_48->setStyleSheet("");
         ui->label_48->setText("");

         int start;
         int end; \
             int len_ds;
         int len_show;
         int len_temp;
         int len_code = PCB__temp->exeInfo.codeMemory;



         len_temp = (len_code)* 480 / dslen;
         ui->label_48->setStyleSheet("background-color:rgb(0, 0, 0)");
         ui->label_48->setText("");
         ui->label_48->setGeometry(60, 440, len_temp, 16);
         bp = len_temp;

         if (dsheap != NULL)
         {
             start = dsheap->begin;
             end = PCB__temp->exeInfo.codeMemory - 1;

         }

         for (int i = 0; dsheap != NULL; i++)
         {

             if (dsheap->begin != end + 1)
             {
                 len_temp = (dsheap->begin - end - 1) * 480 / dslen;
                 showdslabel(i, -1, len_temp);
                 i++;
             }
             start = dsheap->begin;
             end = dsheap->end; \
                 len_ds = end - start + 1;
             len_show = len_ds * 480 / dslen;
             showdslabel(i, dsheap->label, len_show);
             dsheap = dsheap->nextPtr;
         }


     }
     else if (PCB__temp == NULL)
         cleardet();

 }

 void MainWindow::showfirstpage()
 {
     ui->label_prn->setText(QString::number(getQueueLength(RUNNING),10));
     ui->label_pwn->setText(QString::number(getQueueLength(WAITING),10));
     ui->label_proinrea->setText(QString::number(getQueueLength(READY),10));
     if(runningProcess!=NULL)
     ui->label_prun->setText(QString::number(runningProcess->pid,10));
     else
     ui->label_prun->setText("NULL");
 }
/******************************************************************************************************/


void MainWindow::on_pushButton_Set_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}
//设置日志文件是否输出
void MainWindow::on_checkBox_isLogOut_clicked()
{
    if(ui->checkBox_isLogOut->isChecked()) {
        LOG_OUTPUT = true;
        //qDebug()<<LOG_OUTPUT;
    }
    else {
        LOG_OUTPUT = false;
        //qDebug() << LOG_OUTPUT;
    }
}





void MainWindow::on_comboBox_PageChangeType_currentIndexChanged(int index)
{
    if(index == 0) {//FIFO
        //qDebug()<<index;
        //qDebug()<< ui->comboBox_PageChangeType->currentIndex();
        pageType = 0;
        //qDebug()<<pageType;
    }
    else if(index == 1) {//LRU
        //qDebug()<<index;
        //qDebug()<< ui->comboBox_PageChangeType->currentIndex();
        pageType = 1;
        //qDebug()<<pageType;
    }
}

void MainWindow::on_checkBox_isDynamic_clicked()
{
    if(ui->checkBox_isDynamic->isChecked()) {
        isDynamicPriority = true;
    }
    else {
       isDynamicPriority = false;
    }
}

void MainWindow::showdslabel(int num, int label, int len)
{
    int x_p;
    switch (num)
    {
    case 0:
        ui->label_27->setGeometry(60 + bp, 440, len, 16);
        if (label != -1)
        {
            ui->label_27->setStyleSheet("background-color:rgb(255, 255, 127)");
            ui->label_27->setText(QString::number(label, 10));
        }
        else
        {
            ui->label_27->setStyleSheet("background-color:rgb(255, 255, 255)");
            ui->label_27->setText("");
        }
        x_p = ui->label_27->x() + len;
        ui->label_28->setGeometry(QRect(x_p, 440, 30, 16));
        break;
    case 1:
        if (label != -1)
        {
            ui->label_28->setStyleSheet("background-color:rgb(255, 0, 0)");
            ui->label_28->setText(QString::number(label, 10));
        }
        else
        {
            ui->label_28->setStyleSheet("background-color:rgb(255, 255, 255)");
            ui->label_28->setText("");
        }
        x_p = ui->label_28->x();
        ui->label_28->setGeometry(QRect(x_p, 440, len, 16));
        x_p = ui->label_28->x() + len;
        ui->label_29->setGeometry(x_p, 440, 30, 16);
        break;
    case 2:
        if (label != -1)
        {

            ui->label_29->setStyleSheet("background-color:rgb(255, 85, 0)");
            ui->label_29->setText(QString::number(label, 10));
        }
        else
        {
            ui->label_29->setStyleSheet("background-color:rgb(255, 255, 255)");
            ui->label_29->setText("");
        }
        x_p = ui->label_29->x();
        ui->label_29->setGeometry(QRect(x_p, 440, len, 16));
        x_p = ui->label_29->x() + len;
        ui->label_30->setGeometry(x_p, 440, 30, 16);
        break;
    case 3:
        if (label != -1)
        {
            ui->label_30->setStyleSheet("background-color:rgb(255, 255, 0)");
            ui->label_30->setText(QString::number(label, 10));
        }
        else
        {
            ui->label_30->setStyleSheet("background-color:rgb(255, 255, 255)");
            ui->label_30->setText("");
        }
        x_p = ui->label_30->x();
        ui->label_30->setGeometry(QRect(x_p, 440, len, 16));
        x_p = ui->label_30->x() + len;
        ui->label_31->setGeometry(x_p, 440, 30, 16);
        break;
    case 4:
        if (label != -1)
        {
            ui->label_31->setStyleSheet("background-color:rgb(0, 255, 0)");
            ui->label_31->setText(QString::number(label, 10));
        }
        else
        {
            ui->label_31->setStyleSheet("background-color:rgb(255, 255, 255)");
            ui->label_31->setText("");
        }
        x_p = ui->label_31->x();
        ui->label_31->setGeometry(QRect(x_p, 440, len, 16));
        x_p = ui->label_31->x() + len;
        ui->label_32->setGeometry(x_p, 440, 30, 16);
        break;
    case 5:
        if (label != -1)
        {
            ui->label_32->setStyleSheet("background-color:rgb(0, 255, 217)");
            ui->label_32->setText(QString::number(label, 10));
        }
        else
        {
            ui->label_32->setStyleSheet("background-color:rgb(255, 255, 255)");
            ui->label_32->setText("");
        }
        x_p = ui->label_32->x();
        ui->label_32->setGeometry(QRect(x_p, 440, len, 16));
        x_p = ui->label_32->x() + len;
        ui->label_34->setGeometry(x_p, 440, 30, 16);
        break;
    case 6:
        if (label != -1)
        {
            ui->label_34->setStyleSheet("background-color:rgb(0, 0, 255)");
            ui->label_34->setText(QString::number(label, 10));
        }
        else
        {
            ui->label_34->setStyleSheet("background-color:rgb(255, 255, 255)");
            ui->label_34->setText("");
        }
        x_p = ui->label_34->x();
        ui->label_34->setGeometry(QRect(x_p, 440, len, 16));
        x_p = ui->label_34->x() + len;
        ui->label_35->setGeometry(x_p, 440, 30, 16);
        break;
    case 7:
        if (label != -1)
        {
            ui->label_35->setStyleSheet("background-color:rgb(170, 0, 255)");
            ui->label_35->setText(QString::number(label, 10));
        }
        else
        {
            ui->label_35->setStyleSheet("background-color:rgb(255, 255, 255)");
            ui->label_35->setText("");
        }
        x_p = ui->label_35->x();
        ui->label_35->setGeometry(QRect(x_p, 440, len, 16));
        x_p = ui->label_35->x() + len;
        ui->label_36->setGeometry(x_p, 440, 30, 16);
        break;
    case 8:
        if (label != -1)
        {
            ui->label_36->setStyleSheet("background-color:rgb(0, 255, 255)");
            ui->label_36->setText(QString::number(label, 10));
        }
        else
        {
            ui->label_36->setStyleSheet("background-color:rgb(255, 255, 255)");
            ui->label_36->setText("");
        }
        x_p = ui->label_36->x();
        ui->label_36->setGeometry(QRect(x_p, 440, len, 16));
        x_p = ui->label_36->x() + len;
        ui->label_37->setGeometry(x_p, 440, 30, 16);
        break;
    case 9:
        if (label != -1)
        {
            ui->label_37->setStyleSheet("background-color:rgb(85, 0, 255)");
            ui->label_37->setText(QString::number(label, 10));
        }
        else
        {
            ui->label_37->setStyleSheet("background-color:rgb(255, 255, 255)");
            ui->label_37->setText("");
        }
        x_p = ui->label_37->x();
        ui->label_37->setGeometry(QRect(x_p, 440, len, 16));
        x_p = ui->label_37->x() + len;
        ui->label_38->setGeometry(x_p, 440, 30, 16);
        break;
    case 10:
        if (label != -1)
        {
            ui->label_38->setStyleSheet("background-color:rgb(255, 0, 255)");
            ui->label_38->setText(QString::number(label, 10));
        }
        else
        {
            ui->label_38->setStyleSheet("background-color:rgb(255, 255, 255)");
            ui->label_38->setText("");
        }
        x_p = ui->label_38->x();
        ui->label_38->setGeometry(QRect(x_p, 440, len, 16));
        x_p = ui->label_38->x() + len;
        ui->label_39->setGeometry(x_p, 440, 30, 16);
        break;
    case 11:
        if (label != -1)
        {
            ui->label_39->setStyleSheet("background-color:rgb(170, 85, 0)");
            ui->label_39->setText(QString::number(label, 10));
        }
        else
        {
            ui->label_39->setStyleSheet("background-color:rgb(255, 255, 255)");
            ui->label_39->setText("");
        }
        x_p = ui->label_39->x();
        ui->label_39->setGeometry(QRect(x_p, 440, len, 16));
        x_p = ui->label_39->x() + len;
        ui->label_40->setGeometry(x_p, 440, 30, 16);
        break;
    case 12:
        if (label != -1)
        {
            ui->label_40->setStyleSheet("background-color:rgb(0, 85, 255)");
            ui->label_40->setText(QString::number(label, 10));
        }
        else
        {
            ui->label_40->setStyleSheet("background-color:rgb(255, 255, 255)");
            ui->label_40->setText("");
        }
        x_p = ui->label_40->x();
        ui->label_40->setGeometry(QRect(x_p, 440, len, 16));
        x_p = ui->label_40->x() + len;
        ui->label_41->setGeometry(x_p, 440, 30, 16);
        break;
    case 13:
        if (label != -1)
        {
            ui->label_41->setStyleSheet("background-color:rgb(0, 0, 127)");
            ui->label_41->setText(QString::number(label, 10));
        }
        else
        {
            ui->label_41->setStyleSheet("background-color:rgb(255, 255, 255)");
            ui->label_41->setText("");
        }
        x_p = ui->label_41->x();
        ui->label_41->setGeometry(QRect(x_p, 440, len, 16));
        x_p = ui->label_41->x() + len;
        ui->label_42->setGeometry(x_p, 440, 30, 16);
        break;
    case 14:
        if (label != -1)
        {
            ui->label_42->setStyleSheet("background-color:rgb(127, 127, 127)");
            ui->label_42->setText(QString::number(label, 10));
        }
        else
        {
            ui->label_42->setStyleSheet("background-color:rgb(255, 255, 255)");
            ui->label_42->setText("");
        }
        x_p = ui->label_42->x();
        ui->label_42->setGeometry(QRect(x_p, 440, len, 16));
        x_p = ui->label_42->x() + len;
        ui->label_43->setGeometry(x_p, 440, 30, 16);
        break;
    case 15:
        if (label != -1)
        {
            ui->label_43->setStyleSheet("background-color:rgb(0, 127, 127)");
            ui->label_43->setText(QString::number(label, 10));
        }
        else
        {
            ui->label_43->setStyleSheet("background-color:rgb(255, 255, 255)");
            ui->label_43->setText("");
        }
        x_p = ui->label_43->x();
        ui->label_43->setGeometry(QRect(x_p, 440, len, 16));
        x_p = ui->label_43->x() + len;
        break;
    default:
        QMessageBox::warning(this, "段显示失败", "段显示错误", QMessageBox::Cancel);
        return;
    }

}

void MainWindow::cleardet()
{
    ui->label_pid->setText("");
    ui->label_pro->setText("");
    ui->label_propath->setText("");
    ui->label_pcshow->setText("");
    ui->label_pccpu->setText("");
    ui->label_miu->setText("");
    ui->label_cpu->setText("");
    ui->label_dslen->setText("");
    ui->label_dsuse->setText("");
    ui->label_isa->setText("");
    ui->label_fra->setText("");
    ui->label_pn->setText("");
    ui->label_event->setText("");
    ui->label_pct->setText("");
    ui->label_pdp->setText("");
    ui->label_prt->setText("");
    ui->label_state->setText("");
    ui->label_pwt->setText("");
    ui->label_sppr->setText("");

    ui->label_27->setGeometry(60, 440, 30, 16);
    ui->label_28->setGeometry(90, 440, 30, 16);
    ui->label_29->setGeometry(120, 440, 30, 16);
    ui->label_30->setGeometry(150, 440, 30, 16);
    ui->label_31->setGeometry(180, 440, 30, 16);
    ui->label_32->setGeometry(210, 440, 30, 16);
    ui->label_34->setGeometry(240, 440, 30, 16);
    ui->label_35->setGeometry(270, 440, 30, 16);
    ui->label_36->setGeometry(300, 440, 30, 16);
    ui->label_37->setGeometry(330, 440, 30, 16);
    ui->label_38->setGeometry(360, 440, 30, 16);
    ui->label_39->setGeometry(390, 440, 30, 16);
    ui->label_40->setGeometry(420, 440, 30, 16);
    ui->label_41->setGeometry(450, 440, 30, 16);
    ui->label_42->setGeometry(480, 440, 30, 16);
    ui->label_43->setGeometry(510, 440, 30, 16);

    ui->label_27->setStyleSheet("");
    ui->label_27->setText("");
    ui->label_28->setStyleSheet("");
    ui->label_28->setText("");
    ui->label_29->setStyleSheet("");
    ui->label_29->setText("");
    ui->label_30->setStyleSheet("");
    ui->label_30->setText("");
    ui->label_31->setStyleSheet("");
    ui->label_31->setText("");
    ui->label_32->setStyleSheet("");
    ui->label_32->setText("");
    ui->label_34->setStyleSheet("");
    ui->label_34->setText("");
    ui->label_35->setStyleSheet("");
    ui->label_35->setText("");
    ui->label_36->setStyleSheet("");
    ui->label_36->setText("");
    ui->label_37->setStyleSheet("");
    ui->label_37->setText("");
    ui->label_38->setStyleSheet("");
    ui->label_38->setText("");
    ui->label_39->setStyleSheet("");
    ui->label_39->setText("");
    ui->label_40->setStyleSheet("");
    ui->label_40->setText("");
    ui->label_41->setStyleSheet("");
    ui->label_41->setText("");
    ui->label_42->setStyleSheet("");
    ui->label_42->setText("");
    ui->label_43->setStyleSheet("");
    ui->label_43->setText("");
    ui->label_48->setStyleSheet("");
    ui->label_48->setText("");
}



//刷新
void MainWindow::on_pushButton_explorerRefresh_clicked()
{
    renovateFileSystem();
}
