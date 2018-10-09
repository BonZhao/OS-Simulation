#include "showAttribute.h"
#include "ui_showAttribute.h"
#include "FileSystem.h"
#include "SystemApp.h"
#include "Basic.h"
#include "MemoryManagement.h"
#include "ProcessManagement.h"
#include "VirtualMemory.h"
#include "MemoryLog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "ExeUnit.h"
#include "IOUnit.h"
#include "workthread.h"
#include <QContextMenuEvent>
#include "createFileFolder.h"
#include "showDelete.h"
#include "showNewName.h"
#include <QMessageBox>
#include <QTextCodec>
#include <QDebug>
#include "showhidechild.h"

extern int maxDiskSize;

extern char rootDirectoryLabel[LENGTH_NAME] ;
extern int frameSize;
extern int sectionSize;

extern char *showAttributePath;

showAttribute::showAttribute(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::showAttribute)
{
    ui->setupUi(this);

    this->newName = "";
    this->path = showAttributePath;
    char *sec__ = (char *)malloc(sizeof(char)*sectionSize);
    int *secNum__ = (int *)malloc(sizeof(int)*10);

    if(!getIndexSection(sec__,secNum__, this->path)){
         MainWindow w;
         w.printErrorGui();
         free(sec__);
         free(secNum__);
         close();
    }


    if(sec__[0] == FOLDER_TYPE || sec__[0] == ROOT_TYPE){
        FolderIndex fo1 ;
        initFolderIndex(&fo1);
        extractFolderIndex(sec__, &fo1);

        this->originalHide = fo1.attribute.hide;
        this->hide = this->originalHide;
        this->originalProtect = fo1.attribute.protect;
        this->protect = this->originalProtect;

        if(sec__[0] == ROOT_TYPE){
            ui->name->setText(QString(QLatin1String(rootDirectoryLabel)));
            ui->type->setText("本地磁盘");
            ui->place->setText("C:");
            ui->hide->setHidden(true);
            ui->protect->setHidden(true);
            ui->att->setHidden(true);

            ui->maxSize->setText(QString::number(maxDiskSize,10)+"字节");

        }
        else{
            ui->name->setText(QString(QLatin1String(fo1.name)));
            ui->type->setText("文件夹");
            ui->place->setText(QString(QLatin1String(this->path)));
            ui->protect->setChecked(fo1.attribute.protect);
            ui->hide->setChecked(fo1.attribute.hide);

            ui->maxSize->setHidden(true);
            ui->max->setHidden(true);
        }




        int length = computeLength(this->path);
        int occupyPlace = computeOccupyLength(this->path)*sectionSize;
        ui->length->setText(QString::number(length, 10)+"字节");
        ui->occupyPlace->setText(QString::number(occupyPlace, 10) + "字节");

        char * time__ = (char *)malloc(sizeof(char)*30);
        showTime(time__, fo1.createTime);
        ui->createTime->setText(QString(QLatin1String(time__)));
        free(time__);

    }
    else{
        qDebug()<<"error77";
    }
    free(sec__);
    free(secNum__);


}

int showAttribute::computeLength(char *__path){


    char *sec__1 = (char *)malloc(sizeof(char)*sectionSize);
    int *secNum__ = (int *)malloc(sizeof(int)*10);

    getIndexSection(sec__1,secNum__, __path);

    if(sec__1[0] == FOLDER_TYPE || sec__1[0] == ROOT_TYPE){
        FolderIndex fo1 ;
        initFolderIndex(&fo1);
        extractFolderIndex(sec__1, &fo1);

        int length=0;
        if(fo1.childNodeNumber == 0){
            return 0;
        }
        for(int  i=0;i<fo1.childNodeNumber;i++) {   //extractFileName(char * __name, char * __ex, const char * __whole);
                char *name__ = (char *)malloc(sizeof(char)*38);
                extractName(name__, fo1.childNode[i]);
                char *path__ = (char *)malloc(sizeof(char)*38);;
                strcpy(path__, __path);

                strcat(path__,"\\") ;
                strcat(path__,name__) ;
             //   qDebug()<<QString(QLatin1String(path__));
                length += computeLength(path__);
                free(name__);
                free(path__);
        }
        return length;
    }
    else if(sec__1[0] == FILE_TYPE){
         FileIndex file;
         initFileIndex(&file);
         extractFileIndex(sec__1,&file);

         return file.length;


    }
    else{
        qDebug()<<"error2";
    }

    free(sec__1);
    free(secNum__);
    return -1;
}

int showAttribute::computeOccupyLength(char *__path){
    char * sec__ = (char *)malloc(sizeof(char)*sectionSize);
    int *secNum__ = (int *)malloc(sizeof(int)*10);

    getIndexSection(sec__,secNum__, __path);

    if(sec__[0] == FOLDER_TYPE || sec__[0] == ROOT_TYPE){
        FolderIndex fo1 ;
        initFolderIndex(&fo1);
        extractFolderIndex(sec__, &fo1);

        int block=0;
        if(fo1.childNodeNumber == 0){
            return 1;
        }
        for(int  i=0;i<fo1.childNodeNumber;i++) {   //extractFileName(char * __name, char * __ex, const char * __whole);
                char *name__ = (char *)malloc(sizeof(char)*38);
                extractName(name__, fo1.childNode[i]);
                char *path__ = (char *)malloc(sizeof(char)*38);;
                strcpy(path__, __path);

                strcat(path__,"\\") ;
                strcat(path__,name__) ;
             //   qDebug()<<QString(QLatin1String(path__));
                block += computeOccupyLength(path__);
                free(name__);
                free(path__);
        }
        return block+1;
    }
    else if(sec__[0] == FILE_TYPE){
         FileIndex file;
         initFileIndex(&file);
         extractFileIndex(sec__,&file);

         return file.dataSectionNumber + 1;


    }
    else{
        qDebug()<<"error2";
    }

    free(sec__);
    free(secNum__);
    return -1;
}

showAttribute::~showAttribute()
{
    delete ui;
}

void showAttribute::on_pushButton_clicked()
{
    this->newName = ui->name->text();
    if(!(this->hide == this->originalHide && this->originalProtect == this->protect)){
        showHideChild h;
        h.exec();
        if(h.getCancelOperate()){
            this->cancelOperate = true;
            close();
        }
        else{
            this->child = h.getHideChild();
        }
    }
    close();
}
bool showAttribute::getCancelOperate() const
{
    return cancelOperate;
}

void showAttribute::setCancelOperate(bool value)
{
    cancelOperate = value;
}

bool showAttribute::getChild() const
{
    return child;
}

void showAttribute::setChild(bool value)
{
    child = value;
}

bool showAttribute::getHide() const
{
    return hide;
}

void showAttribute::setHide(bool value)
{
    hide = value;
}

bool showAttribute::getProtect() const
{
    return protect;
}

void showAttribute::setProtect(bool value)
{
    protect = value;
}

bool showAttribute::getOriginalProtect() const
{
    return originalProtect;
}

void showAttribute::setOriginalProtect(bool value)
{
    originalProtect = value;
}

bool showAttribute::getOriginalHide() const
{
    return originalHide;
}

void showAttribute::setOriginalHide(bool value)
{
    originalHide = value;
}

QString showAttribute::getNewName() const
{
    return newName;
}

void showAttribute::setNewName(const QString &value)
{
    newName = value;
}


void showAttribute::on_protect_stateChanged(int arg1)
{
    if(arg1 == 0){
        this->protect = false;
    }
    else
        this->protect = true;
}

void showAttribute::on_hide_stateChanged(int arg1)
{
    if(arg1 == 0){
        this->hide = false;
       // hide = "不隐藏";

    }
    else{
        this->hide = true;
       // hide = "隐藏";

    }
}
