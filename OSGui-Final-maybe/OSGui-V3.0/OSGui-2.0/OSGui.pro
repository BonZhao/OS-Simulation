#-------------------------------------------------
#
# Project created by QtCreator 2016-04-23T08:24:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OSGui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Basic.cpp \
    Compile.cpp \
    DiskDriver.cpp \
    ExeUnit.cpp \
    FileSystem.cpp \
    IOUnit.cpp \
    MemoryDriver.cpp \
    MemoryManagement.cpp \
    ProcessManagement.cpp \
    SystemApp.cpp \
    TextEditor.cpp \
    VirtualMemory.cpp \
    MemoryLog.cpp \
    workthread.cpp \
    WorkLog.cpp \
    createFileFolder.cpp \
    showAttribute.cpp \
    showDelete.cpp \
    showFileAtt.cpp \
    showNewName.cpp \
    System.cpp \
    showhidechild.cpp \
    login.cpp \
    Account.cpp

HEADERS  += mainwindow.h \
    Basic.h \
    Compile.h \
    DiskDriver.h \
    ExeUnit.h \
    FileSystem.h \
    IOUnit.h \
    MemoryDriver.h \
    MemoryManagement.h \
    ProcessManagement.h \
    SystemApp.h \
    TextEdit.h \
    VirtualMemory.h \
    MemoryLog.h \
    workthread.h \
    WorkLog.h \
    createFileFolder.h \
    showAttribute.h \
    showDelete.h \
    showFileAtt.h \
    showNewName.h \
    System.h \
    hidefilefolder.h \
    showhidechild.h \
    login.h \
    Account.h

FORMS    += mainwindow.ui \
    createFileFolder.ui \
    showAttribute.ui \
    showDelete.ui \
    showFileAtt.ui \
    showNewName.ui \
    showhidechild.ui \
    login.ui
