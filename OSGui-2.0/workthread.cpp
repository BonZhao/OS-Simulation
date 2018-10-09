#include "workthread.h"
#include "ExeUnit.h"
#include "IOUnit.h"
#include "SystemApp.h"
#include<QDebug>
#include<QMutex>
workThread::workThread(QObject *parent) :
    QThread(parent)
{

}

workThread::workThread(int type) {
    this->type = type;
}

void workThread::run()
{

    if(type == 0) {
       startExeUnit();
     }
    /*else if(type == 1) {
        startIOUnit();
    }*/
    else if(type == 2) {
        while(!(this ->stop))
        {
            QMutex mutex;
            mutex.lock();

           emit changed();

            mutex.unlock();
            this->msleep(1000);
        }
    }

}
workThread::~workThread() {

}
