#include "mainwindow.h"
#include <QApplication>
#include "FileSystem.h"
#include "SystemApp.h"
#include "Basic.h"
#include "MemoryManagement.h"
#include "ProcessManagement.h"
#include "ExeUnit.h"
#include "IOUnit.h"
#include "WorkLog.h"
#include "System.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include<QDebug>
#include <QString>
#include "login.h"
extern OFL * openFileList ;
extern clock_t startTime ;
extern int timeSlice ;
extern PCB * runningProcess ;
QString userName = "";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug()<<"start"  ;

    if (!initSystem()) {
        qDebug() << "initial failed" ;
        return 0 ;
    }
    Login loginDlg;
    if(loginDlg.exec()==QDialog::Accepted) {
        MainWindow w;
        w.show();
        return a.exec();
    }
    else {
        return 0;
    }

}




