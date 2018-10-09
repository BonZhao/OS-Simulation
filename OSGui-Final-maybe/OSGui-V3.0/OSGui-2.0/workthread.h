#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include <QThread>
#include "mainwindow.h"
class workThread : public QThread
{
    Q_OBJECT
    public:
        explicit workThread(QObject *parent = 0);
        workThread(int );
        ~workThread();
        void run();
        bool stop = false;
    private:
        int type;
    signals:
        void changed(void);
    public slots:

};

#endif // WORKTHREAD_H
