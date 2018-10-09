#include "showFileAtt.h"
#include "ui_showFileAtt.h"
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

extern int frameSize;
extern int sectionSize;

extern char *showAttributePath;

showFileAtt::showFileAtt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::showFileAtt)
{
    ui->setupUi(this);
    this->newName = "";
    this->path = showAttributePath;
    char *sec__ = (char *)malloc(sizeof(char)*sectionSize);
    int *secNum__ = (int *)malloc(sizeof(int)*10);


    if(!getIndexSection(sec__,secNum__, this->path)){
        // this->printErrorGui();
         MainWindow w;
         w.printErrorGui();
         free(sec__);
         free(secNum__);
         close();
    }
    if(sec__[0] == FILE_TYPE){
         FileIndex file;
         initFileIndex(&file);
         extractFileIndex(sec__,&file);

         this->originalReadonly = file.attribute.readOnly;
         this->originalHide = file.attribute.hide;
         //ui->checkBox->setChecked(false);
         this->hide = file.attribute.hide;
         this->readOnly = file.attribute.readOnly;
         ui->hide->setChecked(file.attribute.hide);
         ui->readOnly->setChecked(file.attribute.readOnly);

         char *name__3 = (char *)malloc(sizeof(char)*38);
         char *ex__ = (char *)malloc(sizeof(char)*38);

         extractFileName(name__3,ex__,file.name);

         ui->name->setText(QString(QLatin1String(file.name)));

         if(QString(QLatin1String(ex__)) == ""){
             ui->type->setText("文件") ;
         }
        else
            ui->type->setText(QString(QLatin1String(ex__))+"文件");

         ui->place->setText(QString(QLatin1String(this->path)));
         int length = file.length;
         int occupyPlace = (1+file.dataSectionNumber) * sectionSize;
         ui->length->setText(QString::number(length, 10)+"字节");
         ui->occupyPlace->setText(QString::number(occupyPlace, 10) + "字节");

         char * time__ = (char *)malloc(sizeof(char)*30);
         showTime(time__, file.createTime);
         ui->createTime->setText(QString(QLatin1String(time__)));

        // //lastModifyTime ;	/*  7B	文件上次修改时间 */
           //  TimeStamp lastOpenTime ;	/*  7B	文件上次打开时间 */

         strcpy(time__, "");
         showTime(time__, file.lastModifyTime);
         ui->changeTime->setText(QString(QLatin1String(time__)));

         strcpy(time__, "");
         showTime(time__, file.lastOpenTime);
         ui->visitTime->setText(QString(QLatin1String(time__)));


         free(time__);
         free(name__3);
         free(ex__);
    }
    else{
        qDebug()<<"error77";
    }
    free(sec__);
    free(secNum__);
}



showFileAtt::~showFileAtt()
{
    delete ui;
}

void showFileAtt::on_pushButton_clicked()
{
    this->newName = ui->name->text();
    close();
}
bool showFileAtt::getHide() const
{
    return hide;
}

void showFileAtt::setHide(bool value)
{
    hide = value;
}

bool showFileAtt::getReadOnly() const
{
    return readOnly;
}

void showFileAtt::setReadOnly(bool value)
{
    readOnly = value;
}

bool showFileAtt::getOriginalHide() const
{
    return originalHide;
}

void showFileAtt::setOriginalHide(bool value)
{
    originalHide = value;
}

bool showFileAtt::getOriginalReadonly() const
{
    return originalReadonly;
}

void showFileAtt::setOriginalReadonly(bool value)
{
    originalReadonly = value;
}

QString showFileAtt::getNewName() const
{
    return newName;
}

void showFileAtt::setNewName(const QString &value)
{
    newName = value;
}


void showFileAtt::on_readOnly_stateChanged(int arg1)
{
    if(arg1 == 0){
        this->readOnly = false;
    }
    else{
        this->readOnly = true;
    }
}

void showFileAtt::on_hide_stateChanged(int arg1)
{
    if(arg1 == 0){
        this->hide = false;
    }
    else{
        this->hide = true;
    }
}
