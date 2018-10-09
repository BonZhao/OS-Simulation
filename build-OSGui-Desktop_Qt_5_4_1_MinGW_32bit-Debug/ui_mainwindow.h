/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_system;
    QLabel *label_cal;
    QLabel *label_mem;
    QLabel *label_disk;
    QWidget *page_2;
    QTabWidget *tabWidget;
    QWidget *tab_Mem;
    QLabel *label_5;
    QProgressBar *progressBar_mem;
    QTableWidget *tableWidget_Mem;
    QTextBrowser *textBrowser_memContent;
    QWidget *tab_Vm;
    QLabel *label_6;
    QProgressBar *progressBar_vm;
    QTableWidget *tableWidget_vm;
    QWidget *tab_page;
    QLabel *label_7;
    QTableWidget *tableWidget_pageFault;
    QWidget *page_3;
    QWidget *page_4;
    QTreeWidget *treeWidget_explorer;
    QLabel *path;
    QTextBrowser *showPath;
    QCheckBox *checkBox;
    QPushButton *pushButton_explorerRefresh;
    QWidget *page_5;
    QStackedWidget *stackedWidget_textEditor;
    QWidget *page_6;
    QTextEdit *textEditApp;
    QLabel *label_TextPath;
    QPushButton *pushButton_toHex;
    QPushButton *pushButton_textEditor_save;
    QPushButton *pushButton_textEditor_saveAs;
    QPushButton *pushButton_textEditor_open;
    QPushButton *pushButton_compile;
    QWidget *page_7;
    QLabel *label_8;
    QLineEdit *lineEdit_textPath;
    QPushButton *pushButton_textSave;
    QPushButton *pushButton_textCancel;
    QLabel *label_20;
    QLineEdit *lineEdit_textName;
    QWidget *page_8;
    QLabel *label_9;
    QLineEdit *lineEdit_textPath_open;
    QPushButton *pushButton_textOpen;
    QPushButton *pushButton_textOpenCancel;
    QWidget *page_14;
    QWidget *page_9;
    QStackedWidget *stackedWidget_2;
    QWidget *page_10;
    QPushButton *pushButton_processreturn;
    QPushButton *pushButton_detail;
    QWidget *page_11;
    QTableWidget *tableWidget_pmaintable;
    QPushButton *pushButton_deleteprocess;
    QPushButton *pushButton_newprocess;
    QLabel *label_Prowaitno;
    QLabel *label_Proinusenu;
    QLabel *label_pwn;
    QLabel *label_prn;
    QLabel *label_22;
    QLabel *label_proinrea;
    QLabel *label_prun;
    QLabel *label_Proinusenu_2;
    QWidget *page_12;
    QPushButton *pushButton_return;
    QWidget *formLayoutWidget;
    QFormLayout *formLayout;
    QLabel *label_pids;
    QLabel *label_pid;
    QLabel *label_10;
    QLabel *label_11;
    QWidget *formLayoutWidget_2;
    QFormLayout *formLayout_2;
    QLabel *label_Pronames;
    QLabel *label_pro;
    QLabel *label_propath;
    QLabel *label_12;
    QLabel *label_13;
    QWidget *formLayoutWidget_3;
    QFormLayout *formLayout_3;
    QLabel *label_pc;
    QLabel *label_pcshow;
    QLabel *label_pccpu;
    QLabel *label_pccpus;
    QLabel *label_miu;
    QLabel *label_cpu;
    QLabel *label_miut;
    QLabel *label_cput;
    QLabel *label_18;
    QLabel *label_14;
    QWidget *formLayoutWidget_5;
    QFormLayout *formLayout_5;
    QLabel *label_15;
    QLabel *label_pn;
    QLabel *label_17;
    QLabel *label_isa;
    QLabel *label_21;
    QLabel *label_fra;
    QWidget *formLayoutWidget_4;
    QFormLayout *formLayout_4;
    QLabel *label_pstate;
    QLabel *label_state;
    QLabel *label_16;
    QLabel *label_sppr;
    QLabel *label_pdps;
    QLabel *label_pdp;
    QLabel *label_pcts;
    QLabel *label_pct;
    QLabel *label_pwts;
    QLabel *label_pwt;
    QLabel *label_prts;
    QLabel *label_prt;
    QLabel *label_19;
    QLabel *label_event;
    QLabel *label_33;
    QLabel *label_36;
    QLabel *label_32;
    QLabel *label_28;
    QLabel *label_38;
    QLabel *label_30;
    QLabel *label_34;
    QLabel *label_27;
    QLabel *label_39;
    QLabel *label_40;
    QLabel *label_31;
    QLabel *label_37;
    QLabel *label_43;
    QLabel *label_42;
    QLabel *label_35;
    QLabel *label_29;
    QLabel *label_41;
    QWidget *formLayoutWidget_6;
    QFormLayout *formLayout_6;
    QLabel *label_44;
    QLabel *label_dslen;
    QLabel *label_45;
    QLabel *label_dsuse;
    QLabel *label_46;
    QLabel *label_47;
    QLabel *label_48;
    QWidget *page_13;
    QPushButton *pushButton_poconfirm;
    QPushButton *pushButton_pocancel;
    QLineEdit *lineEdit_processopen;
    QLabel *label_Processopentip;
    QWidget *page_15;
    QLabel *label_23;
    QLabel *label_24;
    QLabel *label_25;
    QCheckBox *checkBox_isLogOut;
    QComboBox *comboBox_PageChangeType;
    QCheckBox *checkBox_isDynamic;
    QPushButton *pushButton_MemM;
    QPushButton *pushButton_2;
    QPushButton *pushButton_explorer;
    QPushButton *pushButton_overview;
    QPushButton *pushButton_powerOff;
    QPushButton *pushButton_textEditor;
    QPushButton *pushButton_Set;
    QLabel *label_26;
    QLabel *label_userName;
    QLabel *label_time;
    QLabel *label_49;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1101, 706);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        stackedWidget = new QStackedWidget(centralWidget);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        stackedWidget->setGeometry(QRect(130, 50, 931, 541));
        page = new QWidget();
        page->setObjectName(QStringLiteral("page"));
        label = new QLabel(page);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(90, 80, 81, 31));
        QFont font;
        font.setPointSize(11);
        label->setFont(font);
        label->setContextMenuPolicy(Qt::DefaultContextMenu);
        label->setLayoutDirection(Qt::LeftToRight);
        label_2 = new QLabel(page);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(90, 140, 81, 31));
        label_2->setFont(font);
        label_2->setContextMenuPolicy(Qt::DefaultContextMenu);
        label_2->setLayoutDirection(Qt::LeftToRight);
        label_3 = new QLabel(page);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(90, 190, 81, 31));
        label_3->setFont(font);
        label_3->setContextMenuPolicy(Qt::DefaultContextMenu);
        label_3->setLayoutDirection(Qt::LeftToRight);
        label_4 = new QLabel(page);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(90, 260, 71, 31));
        label_4->setFont(font);
        label_4->setContextMenuPolicy(Qt::DefaultContextMenu);
        label_4->setLayoutDirection(Qt::LeftToRight);
        label_system = new QLabel(page);
        label_system->setObjectName(QStringLiteral("label_system"));
        label_system->setGeometry(QRect(190, 80, 400, 31));
        label_system->setFont(font);
        label_cal = new QLabel(page);
        label_cal->setObjectName(QStringLiteral("label_cal"));
        label_cal->setGeometry(QRect(190, 140, 400, 31));
        label_cal->setFont(font);
        label_mem = new QLabel(page);
        label_mem->setObjectName(QStringLiteral("label_mem"));
        label_mem->setGeometry(QRect(190, 200, 400, 31));
        label_mem->setFont(font);
        label_disk = new QLabel(page);
        label_disk->setObjectName(QStringLiteral("label_disk"));
        label_disk->setGeometry(QRect(190, 270, 421, 31));
        label_disk->setFont(font);
        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QStringLiteral("page_2"));
        tabWidget = new QTabWidget(page_2);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(10, 10, 531, 411));
        tab_Mem = new QWidget();
        tab_Mem->setObjectName(QStringLiteral("tab_Mem"));
        label_5 = new QLabel(tab_Mem);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 20, 90, 21));
        QFont font1;
        font1.setPointSize(10);
        label_5->setFont(font1);
        progressBar_mem = new QProgressBar(tab_Mem);
        progressBar_mem->setObjectName(QStringLiteral("progressBar_mem"));
        progressBar_mem->setGeometry(QRect(160, 20, 151, 23));
        progressBar_mem->setValue(24);
        tableWidget_Mem = new QTableWidget(tab_Mem);
        tableWidget_Mem->setObjectName(QStringLiteral("tableWidget_Mem"));
        tableWidget_Mem->setGeometry(QRect(20, 60, 491, 161));
        textBrowser_memContent = new QTextBrowser(tab_Mem);
        textBrowser_memContent->setObjectName(QStringLiteral("textBrowser_memContent"));
        textBrowser_memContent->setGeometry(QRect(20, 240, 491, 131));
        tabWidget->addTab(tab_Mem, QString());
        tab_Vm = new QWidget();
        tab_Vm->setObjectName(QStringLiteral("tab_Vm"));
        label_6 = new QLabel(tab_Vm);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(30, 30, 90, 21));
        label_6->setFont(font1);
        progressBar_vm = new QProgressBar(tab_Vm);
        progressBar_vm->setObjectName(QStringLiteral("progressBar_vm"));
        progressBar_vm->setGeometry(QRect(170, 30, 151, 23));
        progressBar_vm->setValue(24);
        tableWidget_vm = new QTableWidget(tab_Vm);
        tableWidget_vm->setObjectName(QStringLiteral("tableWidget_vm"));
        tableWidget_vm->setGeometry(QRect(170, 80, 341, 241));
        tabWidget->addTab(tab_Vm, QString());
        tab_page = new QWidget();
        tab_page->setObjectName(QStringLiteral("tab_page"));
        label_7 = new QLabel(tab_page);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(40, 40, 90, 21));
        label_7->setFont(font1);
        tableWidget_pageFault = new QTableWidget(tab_page);
        tableWidget_pageFault->setObjectName(QStringLiteral("tableWidget_pageFault"));
        tableWidget_pageFault->setGeometry(QRect(120, 40, 381, 281));
        tabWidget->addTab(tab_page, QString());
        stackedWidget->addWidget(page_2);
        page_3 = new QWidget();
        page_3->setObjectName(QStringLiteral("page_3"));
        stackedWidget->addWidget(page_3);
        page_4 = new QWidget();
        page_4->setObjectName(QStringLiteral("page_4"));
        treeWidget_explorer = new QTreeWidget(page_4);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        treeWidget_explorer->setHeaderItem(__qtreewidgetitem);
        treeWidget_explorer->setObjectName(QStringLiteral("treeWidget_explorer"));
        treeWidget_explorer->setGeometry(QRect(30, 70, 511, 341));
        treeWidget_explorer->setMidLineWidth(1);
        path = new QLabel(page_4);
        path->setObjectName(QStringLiteral("path"));
        path->setGeometry(QRect(30, 40, 61, 16));
        showPath = new QTextBrowser(page_4);
        showPath->setObjectName(QStringLiteral("showPath"));
        showPath->setGeometry(QRect(90, 30, 311, 31));
        checkBox = new QCheckBox(page_4);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(430, 30, 171, 31));
        pushButton_explorerRefresh = new QPushButton(page_4);
        pushButton_explorerRefresh->setObjectName(QStringLiteral("pushButton_explorerRefresh"));
        pushButton_explorerRefresh->setGeometry(QRect(450, 450, 75, 23));
        stackedWidget->addWidget(page_4);
        page_5 = new QWidget();
        page_5->setObjectName(QStringLiteral("page_5"));
        stackedWidget_textEditor = new QStackedWidget(page_5);
        stackedWidget_textEditor->setObjectName(QStringLiteral("stackedWidget_textEditor"));
        stackedWidget_textEditor->setGeometry(QRect(40, 20, 501, 441));
        page_6 = new QWidget();
        page_6->setObjectName(QStringLiteral("page_6"));
        textEditApp = new QTextEdit(page_6);
        textEditApp->setObjectName(QStringLiteral("textEditApp"));
        textEditApp->setGeometry(QRect(20, 60, 451, 311));
        label_TextPath = new QLabel(page_6);
        label_TextPath->setObjectName(QStringLiteral("label_TextPath"));
        label_TextPath->setGeometry(QRect(30, 20, 281, 21));
        pushButton_toHex = new QPushButton(page_6);
        pushButton_toHex->setObjectName(QStringLiteral("pushButton_toHex"));
        pushButton_toHex->setGeometry(QRect(370, 20, 101, 21));
        pushButton_textEditor_save = new QPushButton(page_6);
        pushButton_textEditor_save->setObjectName(QStringLiteral("pushButton_textEditor_save"));
        pushButton_textEditor_save->setGeometry(QRect(90, 390, 71, 31));
        pushButton_textEditor_saveAs = new QPushButton(page_6);
        pushButton_textEditor_saveAs->setObjectName(QStringLiteral("pushButton_textEditor_saveAs"));
        pushButton_textEditor_saveAs->setGeometry(QRect(200, 390, 71, 31));
        pushButton_textEditor_open = new QPushButton(page_6);
        pushButton_textEditor_open->setObjectName(QStringLiteral("pushButton_textEditor_open"));
        pushButton_textEditor_open->setGeometry(QRect(300, 390, 71, 31));
        pushButton_compile = new QPushButton(page_6);
        pushButton_compile->setObjectName(QStringLiteral("pushButton_compile"));
        pushButton_compile->setGeometry(QRect(400, 390, 71, 31));
        stackedWidget_textEditor->addWidget(page_6);
        page_7 = new QWidget();
        page_7->setObjectName(QStringLiteral("page_7"));
        label_8 = new QLabel(page_7);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(30, 150, 171, 31));
        QFont font2;
        font2.setPointSize(16);
        label_8->setFont(font2);
        lineEdit_textPath = new QLineEdit(page_7);
        lineEdit_textPath->setObjectName(QStringLiteral("lineEdit_textPath"));
        lineEdit_textPath->setGeometry(QRect(100, 200, 341, 31));
        pushButton_textSave = new QPushButton(page_7);
        pushButton_textSave->setObjectName(QStringLiteral("pushButton_textSave"));
        pushButton_textSave->setGeometry(QRect(264, 270, 71, 31));
        pushButton_textCancel = new QPushButton(page_7);
        pushButton_textCancel->setObjectName(QStringLiteral("pushButton_textCancel"));
        pushButton_textCancel->setGeometry(QRect(370, 270, 71, 31));
        label_20 = new QLabel(page_7);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setGeometry(QRect(30, 50, 171, 31));
        label_20->setFont(font2);
        lineEdit_textName = new QLineEdit(page_7);
        lineEdit_textName->setObjectName(QStringLiteral("lineEdit_textName"));
        lineEdit_textName->setGeometry(QRect(100, 100, 341, 31));
        stackedWidget_textEditor->addWidget(page_7);
        page_8 = new QWidget();
        page_8->setObjectName(QStringLiteral("page_8"));
        label_9 = new QLabel(page_8);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(40, 60, 171, 31));
        label_9->setFont(font2);
        lineEdit_textPath_open = new QLineEdit(page_8);
        lineEdit_textPath_open->setObjectName(QStringLiteral("lineEdit_textPath_open"));
        lineEdit_textPath_open->setGeometry(QRect(100, 120, 341, 31));
        pushButton_textOpen = new QPushButton(page_8);
        pushButton_textOpen->setObjectName(QStringLiteral("pushButton_textOpen"));
        pushButton_textOpen->setGeometry(QRect(274, 270, 71, 31));
        pushButton_textOpenCancel = new QPushButton(page_8);
        pushButton_textOpenCancel->setObjectName(QStringLiteral("pushButton_textOpenCancel"));
        pushButton_textOpenCancel->setGeometry(QRect(380, 270, 71, 31));
        stackedWidget_textEditor->addWidget(page_8);
        page_14 = new QWidget();
        page_14->setObjectName(QStringLiteral("page_14"));
        stackedWidget_textEditor->addWidget(page_14);
        stackedWidget->addWidget(page_5);
        page_9 = new QWidget();
        page_9->setObjectName(QStringLiteral("page_9"));
        stackedWidget_2 = new QStackedWidget(page_9);
        stackedWidget_2->setObjectName(QStringLiteral("stackedWidget_2"));
        stackedWidget_2->setGeometry(QRect(30, 30, 841, 501));
        page_10 = new QWidget();
        page_10->setObjectName(QStringLiteral("page_10"));
        pushButton_processreturn = new QPushButton(page_10);
        pushButton_processreturn->setObjectName(QStringLiteral("pushButton_processreturn"));
        pushButton_processreturn->setGeometry(QRect(310, 230, 93, 28));
        pushButton_detail = new QPushButton(page_10);
        pushButton_detail->setObjectName(QStringLiteral("pushButton_detail"));
        pushButton_detail->setGeometry(QRect(160, 230, 93, 28));
        stackedWidget_2->addWidget(page_10);
        page_11 = new QWidget();
        page_11->setObjectName(QStringLiteral("page_11"));
        tableWidget_pmaintable = new QTableWidget(page_11);
        tableWidget_pmaintable->setObjectName(QStringLiteral("tableWidget_pmaintable"));
        tableWidget_pmaintable->setGeometry(QRect(10, 30, 771, 361));
        pushButton_deleteprocess = new QPushButton(page_11);
        pushButton_deleteprocess->setObjectName(QStringLiteral("pushButton_deleteprocess"));
        pushButton_deleteprocess->setGeometry(QRect(680, 410, 93, 28));
        pushButton_newprocess = new QPushButton(page_11);
        pushButton_newprocess->setObjectName(QStringLiteral("pushButton_newprocess"));
        pushButton_newprocess->setGeometry(QRect(560, 410, 93, 28));
        label_Prowaitno = new QLabel(page_11);
        label_Prowaitno->setObjectName(QStringLiteral("label_Prowaitno"));
        label_Prowaitno->setGeometry(QRect(30, 430, 131, 31));
        label_Proinusenu = new QLabel(page_11);
        label_Proinusenu->setObjectName(QStringLiteral("label_Proinusenu"));
        label_Proinusenu->setGeometry(QRect(30, 405, 141, 31));
        label_pwn = new QLabel(page_11);
        label_pwn->setObjectName(QStringLiteral("label_pwn"));
        label_pwn->setGeometry(QRect(160, 435, 101, 21));
        label_prn = new QLabel(page_11);
        label_prn->setObjectName(QStringLiteral("label_prn"));
        label_prn->setGeometry(QRect(170, 410, 101, 21));
        label_22 = new QLabel(page_11);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setGeometry(QRect(30, 460, 131, 20));
        label_proinrea = new QLabel(page_11);
        label_proinrea->setObjectName(QStringLiteral("label_proinrea"));
        label_proinrea->setGeometry(QRect(160, 460, 47, 13));
        label_prun = new QLabel(page_11);
        label_prun->setObjectName(QStringLiteral("label_prun"));
        label_prun->setGeometry(QRect(170, 0, 55, 16));
        label_Proinusenu_2 = new QLabel(page_11);
        label_Proinusenu_2->setObjectName(QStringLiteral("label_Proinusenu_2"));
        label_Proinusenu_2->setGeometry(QRect(40, 0, 141, 21));
        stackedWidget_2->addWidget(page_11);
        page_12 = new QWidget();
        page_12->setObjectName(QStringLiteral("page_12"));
        pushButton_return = new QPushButton(page_12);
        pushButton_return->setObjectName(QStringLiteral("pushButton_return"));
        pushButton_return->setGeometry(QRect(570, 470, 93, 28));
        formLayoutWidget = new QWidget(page_12);
        formLayoutWidget->setObjectName(QStringLiteral("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(50, 60, 160, 31));
        formLayout = new QFormLayout(formLayoutWidget);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        label_pids = new QLabel(formLayoutWidget);
        label_pids->setObjectName(QStringLiteral("label_pids"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_pids);

        label_pid = new QLabel(formLayoutWidget);
        label_pid->setObjectName(QStringLiteral("label_pid"));

        formLayout->setWidget(0, QFormLayout::FieldRole, label_pid);

        label_10 = new QLabel(page_12);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(50, 20, 121, 21));
        label_11 = new QLabel(page_12);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(50, 100, 121, 21));
        formLayoutWidget_2 = new QWidget(page_12);
        formLayoutWidget_2->setObjectName(QStringLiteral("formLayoutWidget_2"));
        formLayoutWidget_2->setGeometry(QRect(50, 150, 161, 61));
        formLayout_2 = new QFormLayout(formLayoutWidget_2);
        formLayout_2->setSpacing(6);
        formLayout_2->setContentsMargins(11, 11, 11, 11);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        formLayout_2->setContentsMargins(0, 0, 0, 0);
        label_Pronames = new QLabel(formLayoutWidget_2);
        label_Pronames->setObjectName(QStringLiteral("label_Pronames"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_Pronames);

        label_pro = new QLabel(formLayoutWidget_2);
        label_pro->setObjectName(QStringLiteral("label_pro"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, label_pro);

        label_propath = new QLabel(formLayoutWidget_2);
        label_propath->setObjectName(QStringLiteral("label_propath"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, label_propath);

        label_12 = new QLabel(formLayoutWidget_2);
        label_12->setObjectName(QStringLiteral("label_12"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_12);

        label_13 = new QLabel(page_12);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(50, 240, 161, 21));
        formLayoutWidget_3 = new QWidget(page_12);
        formLayoutWidget_3->setObjectName(QStringLiteral("formLayoutWidget_3"));
        formLayoutWidget_3->setGeometry(QRect(50, 270, 160, 101));
        formLayout_3 = new QFormLayout(formLayoutWidget_3);
        formLayout_3->setSpacing(6);
        formLayout_3->setContentsMargins(11, 11, 11, 11);
        formLayout_3->setObjectName(QStringLiteral("formLayout_3"));
        formLayout_3->setContentsMargins(0, 0, 0, 0);
        label_pc = new QLabel(formLayoutWidget_3);
        label_pc->setObjectName(QStringLiteral("label_pc"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_pc);

        label_pcshow = new QLabel(formLayoutWidget_3);
        label_pcshow->setObjectName(QStringLiteral("label_pcshow"));

        formLayout_3->setWidget(0, QFormLayout::FieldRole, label_pcshow);

        label_pccpu = new QLabel(formLayoutWidget_3);
        label_pccpu->setObjectName(QStringLiteral("label_pccpu"));

        formLayout_3->setWidget(1, QFormLayout::FieldRole, label_pccpu);

        label_pccpus = new QLabel(formLayoutWidget_3);
        label_pccpus->setObjectName(QStringLiteral("label_pccpus"));

        formLayout_3->setWidget(1, QFormLayout::LabelRole, label_pccpus);

        label_miu = new QLabel(formLayoutWidget_3);
        label_miu->setObjectName(QStringLiteral("label_miu"));

        formLayout_3->setWidget(2, QFormLayout::FieldRole, label_miu);

        label_cpu = new QLabel(formLayoutWidget_3);
        label_cpu->setObjectName(QStringLiteral("label_cpu"));

        formLayout_3->setWidget(3, QFormLayout::FieldRole, label_cpu);

        label_miut = new QLabel(formLayoutWidget_3);
        label_miut->setObjectName(QStringLiteral("label_miut"));

        formLayout_3->setWidget(2, QFormLayout::LabelRole, label_miut);

        label_cput = new QLabel(formLayoutWidget_3);
        label_cput->setObjectName(QStringLiteral("label_cput"));

        formLayout_3->setWidget(3, QFormLayout::LabelRole, label_cput);

        label_18 = new QLabel(page_12);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setGeometry(QRect(300, 20, 111, 31));
        label_14 = new QLabel(page_12);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(280, 230, 71, 21));
        formLayoutWidget_5 = new QWidget(page_12);
        formLayoutWidget_5->setObjectName(QStringLiteral("formLayoutWidget_5"));
        formLayoutWidget_5->setGeometry(QRect(280, 260, 160, 80));
        formLayout_5 = new QFormLayout(formLayoutWidget_5);
        formLayout_5->setSpacing(6);
        formLayout_5->setContentsMargins(11, 11, 11, 11);
        formLayout_5->setObjectName(QStringLiteral("formLayout_5"));
        formLayout_5->setContentsMargins(0, 0, 0, 0);
        label_15 = new QLabel(formLayoutWidget_5);
        label_15->setObjectName(QStringLiteral("label_15"));

        formLayout_5->setWidget(0, QFormLayout::LabelRole, label_15);

        label_pn = new QLabel(formLayoutWidget_5);
        label_pn->setObjectName(QStringLiteral("label_pn"));

        formLayout_5->setWidget(0, QFormLayout::FieldRole, label_pn);

        label_17 = new QLabel(formLayoutWidget_5);
        label_17->setObjectName(QStringLiteral("label_17"));

        formLayout_5->setWidget(1, QFormLayout::LabelRole, label_17);

        label_isa = new QLabel(formLayoutWidget_5);
        label_isa->setObjectName(QStringLiteral("label_isa"));

        formLayout_5->setWidget(1, QFormLayout::FieldRole, label_isa);

        label_21 = new QLabel(formLayoutWidget_5);
        label_21->setObjectName(QStringLiteral("label_21"));

        formLayout_5->setWidget(2, QFormLayout::LabelRole, label_21);

        label_fra = new QLabel(formLayoutWidget_5);
        label_fra->setObjectName(QStringLiteral("label_fra"));

        formLayout_5->setWidget(2, QFormLayout::FieldRole, label_fra);

        formLayoutWidget_4 = new QWidget(page_12);
        formLayoutWidget_4->setObjectName(QStringLiteral("formLayoutWidget_4"));
        formLayoutWidget_4->setGeometry(QRect(280, 60, 311, 161));
        formLayout_4 = new QFormLayout(formLayoutWidget_4);
        formLayout_4->setSpacing(6);
        formLayout_4->setContentsMargins(11, 11, 11, 11);
        formLayout_4->setObjectName(QStringLiteral("formLayout_4"));
        formLayout_4->setContentsMargins(0, 0, 0, 0);
        label_pstate = new QLabel(formLayoutWidget_4);
        label_pstate->setObjectName(QStringLiteral("label_pstate"));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, label_pstate);

        label_state = new QLabel(formLayoutWidget_4);
        label_state->setObjectName(QStringLiteral("label_state"));

        formLayout_4->setWidget(0, QFormLayout::FieldRole, label_state);

        label_16 = new QLabel(formLayoutWidget_4);
        label_16->setObjectName(QStringLiteral("label_16"));

        formLayout_4->setWidget(1, QFormLayout::LabelRole, label_16);

        label_sppr = new QLabel(formLayoutWidget_4);
        label_sppr->setObjectName(QStringLiteral("label_sppr"));

        formLayout_4->setWidget(1, QFormLayout::FieldRole, label_sppr);

        label_pdps = new QLabel(formLayoutWidget_4);
        label_pdps->setObjectName(QStringLiteral("label_pdps"));

        formLayout_4->setWidget(2, QFormLayout::LabelRole, label_pdps);

        label_pdp = new QLabel(formLayoutWidget_4);
        label_pdp->setObjectName(QStringLiteral("label_pdp"));

        formLayout_4->setWidget(2, QFormLayout::FieldRole, label_pdp);

        label_pcts = new QLabel(formLayoutWidget_4);
        label_pcts->setObjectName(QStringLiteral("label_pcts"));

        formLayout_4->setWidget(3, QFormLayout::LabelRole, label_pcts);

        label_pct = new QLabel(formLayoutWidget_4);
        label_pct->setObjectName(QStringLiteral("label_pct"));

        formLayout_4->setWidget(3, QFormLayout::FieldRole, label_pct);

        label_pwts = new QLabel(formLayoutWidget_4);
        label_pwts->setObjectName(QStringLiteral("label_pwts"));

        formLayout_4->setWidget(4, QFormLayout::LabelRole, label_pwts);

        label_pwt = new QLabel(formLayoutWidget_4);
        label_pwt->setObjectName(QStringLiteral("label_pwt"));

        formLayout_4->setWidget(4, QFormLayout::FieldRole, label_pwt);

        label_prts = new QLabel(formLayoutWidget_4);
        label_prts->setObjectName(QStringLiteral("label_prts"));

        formLayout_4->setWidget(5, QFormLayout::LabelRole, label_prts);

        label_prt = new QLabel(formLayoutWidget_4);
        label_prt->setObjectName(QStringLiteral("label_prt"));

        formLayout_4->setWidget(5, QFormLayout::FieldRole, label_prt);

        label_19 = new QLabel(formLayoutWidget_4);
        label_19->setObjectName(QStringLiteral("label_19"));

        formLayout_4->setWidget(6, QFormLayout::LabelRole, label_19);

        label_event = new QLabel(formLayoutWidget_4);
        label_event->setObjectName(QStringLiteral("label_event"));

        formLayout_4->setWidget(6, QFormLayout::FieldRole, label_event);

        label_33 = new QLabel(page_12);
        label_33->setObjectName(QStringLiteral("label_33"));
        label_33->setGeometry(QRect(60, 440, 480, 16));
        label_33->setStyleSheet(QStringLiteral("background-color: rgb(255, 255, 255);"));
        label_36 = new QLabel(page_12);
        label_36->setObjectName(QStringLiteral("label_36"));
        label_36->setGeometry(QRect(300, 440, 30, 16));
        label_32 = new QLabel(page_12);
        label_32->setObjectName(QStringLiteral("label_32"));
        label_32->setGeometry(QRect(210, 440, 30, 16));
        label_28 = new QLabel(page_12);
        label_28->setObjectName(QStringLiteral("label_28"));
        label_28->setGeometry(QRect(90, 440, 30, 16));
        label_38 = new QLabel(page_12);
        label_38->setObjectName(QStringLiteral("label_38"));
        label_38->setGeometry(QRect(360, 440, 30, 16));
        label_30 = new QLabel(page_12);
        label_30->setObjectName(QStringLiteral("label_30"));
        label_30->setGeometry(QRect(150, 440, 30, 16));
        label_34 = new QLabel(page_12);
        label_34->setObjectName(QStringLiteral("label_34"));
        label_34->setGeometry(QRect(240, 440, 30, 16));
        label_27 = new QLabel(page_12);
        label_27->setObjectName(QStringLiteral("label_27"));
        label_27->setGeometry(QRect(60, 440, 30, 16));
        label_39 = new QLabel(page_12);
        label_39->setObjectName(QStringLiteral("label_39"));
        label_39->setGeometry(QRect(390, 440, 30, 16));
        label_40 = new QLabel(page_12);
        label_40->setObjectName(QStringLiteral("label_40"));
        label_40->setGeometry(QRect(420, 440, 30, 16));
        label_31 = new QLabel(page_12);
        label_31->setObjectName(QStringLiteral("label_31"));
        label_31->setGeometry(QRect(180, 440, 30, 16));
        label_37 = new QLabel(page_12);
        label_37->setObjectName(QStringLiteral("label_37"));
        label_37->setGeometry(QRect(330, 440, 30, 16));
        label_43 = new QLabel(page_12);
        label_43->setObjectName(QStringLiteral("label_43"));
        label_43->setGeometry(QRect(510, 440, 30, 16));
        label_42 = new QLabel(page_12);
        label_42->setObjectName(QStringLiteral("label_42"));
        label_42->setGeometry(QRect(480, 440, 30, 16));
        label_35 = new QLabel(page_12);
        label_35->setObjectName(QStringLiteral("label_35"));
        label_35->setGeometry(QRect(270, 440, 30, 16));
        label_29 = new QLabel(page_12);
        label_29->setObjectName(QStringLiteral("label_29"));
        label_29->setGeometry(QRect(120, 440, 30, 16));
        label_41 = new QLabel(page_12);
        label_41->setObjectName(QStringLiteral("label_41"));
        label_41->setGeometry(QRect(450, 440, 30, 16));
        formLayoutWidget_6 = new QWidget(page_12);
        formLayoutWidget_6->setObjectName(QStringLiteral("formLayoutWidget_6"));
        formLayoutWidget_6->setGeometry(QRect(280, 380, 160, 41));
        formLayout_6 = new QFormLayout(formLayoutWidget_6);
        formLayout_6->setSpacing(6);
        formLayout_6->setContentsMargins(11, 11, 11, 11);
        formLayout_6->setObjectName(QStringLiteral("formLayout_6"));
        formLayout_6->setContentsMargins(0, 0, 0, 0);
        label_44 = new QLabel(formLayoutWidget_6);
        label_44->setObjectName(QStringLiteral("label_44"));

        formLayout_6->setWidget(0, QFormLayout::LabelRole, label_44);

        label_dslen = new QLabel(formLayoutWidget_6);
        label_dslen->setObjectName(QStringLiteral("label_dslen"));

        formLayout_6->setWidget(0, QFormLayout::FieldRole, label_dslen);

        label_45 = new QLabel(formLayoutWidget_6);
        label_45->setObjectName(QStringLiteral("label_45"));

        formLayout_6->setWidget(1, QFormLayout::LabelRole, label_45);

        label_dsuse = new QLabel(formLayoutWidget_6);
        label_dsuse->setObjectName(QStringLiteral("label_dsuse"));

        formLayout_6->setWidget(1, QFormLayout::FieldRole, label_dsuse);

        label_46 = new QLabel(page_12);
        label_46->setObjectName(QStringLiteral("label_46"));
        label_46->setGeometry(QRect(280, 350, 61, 21));
        label_47 = new QLabel(page_12);
        label_47->setObjectName(QStringLiteral("label_47"));
        label_47->setGeometry(QRect(60, 410, 55, 16));
        label_48 = new QLabel(page_12);
        label_48->setObjectName(QStringLiteral("label_48"));
        label_48->setGeometry(QRect(630, 290, 47, 13));
        stackedWidget_2->addWidget(page_12);
        pushButton_return->raise();
        formLayoutWidget->raise();
        label_10->raise();
        label_11->raise();
        formLayoutWidget_2->raise();
        label_13->raise();
        formLayoutWidget_3->raise();
        label_18->raise();
        label_14->raise();
        formLayoutWidget_5->raise();
        formLayoutWidget_4->raise();
        label_33->raise();
        label_36->raise();
        label_32->raise();
        label_28->raise();
        label_38->raise();
        label_34->raise();
        label_27->raise();
        label_39->raise();
        label_40->raise();
        label_31->raise();
        label_37->raise();
        label_43->raise();
        label_42->raise();
        label_35->raise();
        label_29->raise();
        label_41->raise();
        formLayoutWidget_6->raise();
        label_46->raise();
        label_47->raise();
        label_30->raise();
        label_48->raise();
        page_13 = new QWidget();
        page_13->setObjectName(QStringLiteral("page_13"));
        pushButton_poconfirm = new QPushButton(page_13);
        pushButton_poconfirm->setObjectName(QStringLiteral("pushButton_poconfirm"));
        pushButton_poconfirm->setGeometry(QRect(270, 280, 93, 28));
        pushButton_pocancel = new QPushButton(page_13);
        pushButton_pocancel->setObjectName(QStringLiteral("pushButton_pocancel"));
        pushButton_pocancel->setGeometry(QRect(410, 280, 93, 28));
        lineEdit_processopen = new QLineEdit(page_13);
        lineEdit_processopen->setObjectName(QStringLiteral("lineEdit_processopen"));
        lineEdit_processopen->setGeometry(QRect(110, 200, 401, 22));
        label_Processopentip = new QLabel(page_13);
        label_Processopentip->setObjectName(QStringLiteral("label_Processopentip"));
        label_Processopentip->setGeometry(QRect(110, 130, 271, 61));
        stackedWidget_2->addWidget(page_13);
        stackedWidget->addWidget(page_9);
        page_15 = new QWidget();
        page_15->setObjectName(QStringLiteral("page_15"));
        label_23 = new QLabel(page_15);
        label_23->setObjectName(QStringLiteral("label_23"));
        label_23->setGeometry(QRect(110, 110, 231, 41));
        label_23->setFont(font2);
        label_24 = new QLabel(page_15);
        label_24->setObjectName(QStringLiteral("label_24"));
        label_24->setGeometry(QRect(110, 270, 231, 41));
        label_24->setFont(font2);
        label_25 = new QLabel(page_15);
        label_25->setObjectName(QStringLiteral("label_25"));
        label_25->setGeometry(QRect(110, 190, 231, 41));
        label_25->setFont(font2);
        checkBox_isLogOut = new QCheckBox(page_15);
        checkBox_isLogOut->setObjectName(QStringLiteral("checkBox_isLogOut"));
        checkBox_isLogOut->setGeometry(QRect(380, 130, 70, 17));
        checkBox_isLogOut->setChecked(true);
        comboBox_PageChangeType = new QComboBox(page_15);
        comboBox_PageChangeType->setObjectName(QStringLiteral("comboBox_PageChangeType"));
        comboBox_PageChangeType->setGeometry(QRect(380, 280, 69, 22));
        checkBox_isDynamic = new QCheckBox(page_15);
        checkBox_isDynamic->setObjectName(QStringLiteral("checkBox_isDynamic"));
        checkBox_isDynamic->setGeometry(QRect(380, 200, 70, 17));
        checkBox_isDynamic->setChecked(true);
        checkBox_isDynamic->setTristate(false);
        stackedWidget->addWidget(page_15);
        pushButton_MemM = new QPushButton(centralWidget);
        pushButton_MemM->setObjectName(QStringLiteral("pushButton_MemM"));
        pushButton_MemM->setGeometry(QRect(20, 140, 100, 30));
        pushButton_2 = new QPushButton(centralWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(20, 200, 100, 30));
        pushButton_explorer = new QPushButton(centralWidget);
        pushButton_explorer->setObjectName(QStringLiteral("pushButton_explorer"));
        pushButton_explorer->setGeometry(QRect(20, 260, 100, 30));
        pushButton_overview = new QPushButton(centralWidget);
        pushButton_overview->setObjectName(QStringLiteral("pushButton_overview"));
        pushButton_overview->setGeometry(QRect(20, 80, 100, 30));
        pushButton_powerOff = new QPushButton(centralWidget);
        pushButton_powerOff->setObjectName(QStringLiteral("pushButton_powerOff"));
        pushButton_powerOff->setGeometry(QRect(20, 440, 100, 30));
        pushButton_textEditor = new QPushButton(centralWidget);
        pushButton_textEditor->setObjectName(QStringLiteral("pushButton_textEditor"));
        pushButton_textEditor->setGeometry(QRect(20, 320, 100, 30));
        pushButton_Set = new QPushButton(centralWidget);
        pushButton_Set->setObjectName(QStringLiteral("pushButton_Set"));
        pushButton_Set->setGeometry(QRect(20, 380, 100, 30));
        label_26 = new QLabel(centralWidget);
        label_26->setObjectName(QStringLiteral("label_26"));
        label_26->setGeometry(QRect(40, 20, 51, 16));
        label_userName = new QLabel(centralWidget);
        label_userName->setObjectName(QStringLiteral("label_userName"));
        label_userName->setGeometry(QRect(110, 20, 171, 16));
        label_time = new QLabel(centralWidget);
        label_time->setObjectName(QStringLiteral("label_time"));
        label_time->setGeometry(QRect(890, 590, 141, 20));
        label_49 = new QLabel(centralWidget);
        label_49->setObjectName(QStringLiteral("label_49"));
        label_49->setGeometry(QRect(790, 620, 271, 21));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1101, 21));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(5);
        tabWidget->setCurrentIndex(0);
        stackedWidget_textEditor->setCurrentIndex(0);
        stackedWidget_2->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        label->setText(QApplication::translate("MainWindow", "\346\223\215\344\275\234\347\263\273\347\273\237", 0));
        label_2->setText(QApplication::translate("MainWindow", "\345\244\204\347\220\206\345\231\250", 0));
        label_3->setText(QApplication::translate("MainWindow", "\345\206\205\345\255\230", 0));
        label_4->setText(QApplication::translate("MainWindow", "\347\241\254\347\233\230", 0));
        label_system->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_cal->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_mem->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_disk->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_5->setText(QApplication::translate("MainWindow", "\345\206\205\345\255\230\344\275\277\347\224\250\346\203\205\345\206\265\357\274\232", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_Mem), QApplication::translate("MainWindow", "\345\206\205\345\255\230\346\203\205\345\206\265", 0));
        label_6->setText(QApplication::translate("MainWindow", "\350\231\232\345\255\230\344\275\277\347\224\250\346\203\205\345\206\265\357\274\232", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_Vm), QApplication::translate("MainWindow", "\350\231\232\345\255\230\344\275\277\347\224\250\346\203\205\345\206\265", 0));
        label_7->setText(QApplication::translate("MainWindow", "\347\274\272\351\241\265\346\203\205\345\206\265\357\274\232", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_page), QApplication::translate("MainWindow", "\347\274\272\351\241\265\346\203\205\345\206\265", 0));
        path->setText(QApplication::translate("MainWindow", "\350\267\257\345\276\204", 0));
        checkBox->setText(QApplication::translate("MainWindow", "\346\230\276\347\244\272\351\232\220\350\227\217\346\226\207\344\273\266\345\222\214\346\226\207\344\273\266\345\244\271", 0));
        pushButton_explorerRefresh->setText(QApplication::translate("MainWindow", "\345\210\267\346\226\260", 0));
        label_TextPath->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        pushButton_toHex->setText(QApplication::translate("MainWindow", "\345\215\201\345\205\255\350\277\233\345\210\266\346\230\276\347\244\272", 0));
        pushButton_textEditor_save->setText(QApplication::translate("MainWindow", "\344\277\235\345\255\230", 0));
        pushButton_textEditor_saveAs->setText(QApplication::translate("MainWindow", "\345\217\246\345\255\230\344\270\272", 0));
        pushButton_textEditor_open->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200", 0));
        pushButton_compile->setText(QApplication::translate("MainWindow", "\347\274\226\350\257\221", 0));
        label_8->setText(QApplication::translate("MainWindow", "\350\257\267\350\276\223\345\205\245\344\277\235\345\255\230\350\267\257\345\276\204\357\274\232", 0));
        lineEdit_textPath->setText(QApplication::translate("MainWindow", "\350\257\267\350\276\223\345\205\245\344\277\235\345\255\230\350\267\257\345\276\204", 0));
        pushButton_textSave->setText(QApplication::translate("MainWindow", "\347\241\256\345\256\232", 0));
        pushButton_textCancel->setText(QApplication::translate("MainWindow", "\345\217\226\346\266\210", 0));
        label_20->setText(QApplication::translate("MainWindow", "\350\257\267\350\276\223\345\205\245\346\226\207\344\273\266\345\220\215\357\274\232", 0));
        lineEdit_textName->setText(QApplication::translate("MainWindow", "\350\257\267\350\276\223\345\205\245\346\226\207\344\273\266\345\220\215", 0));
        label_9->setText(QApplication::translate("MainWindow", "\350\257\267\350\276\223\345\205\245\346\226\207\344\273\266\350\267\257\345\276\204\357\274\232", 0));
        lineEdit_textPath_open->setText(QApplication::translate("MainWindow", "\350\257\267\350\276\223\345\205\245\346\226\207\344\273\266\350\267\257\345\276\204\357\274\232", 0));
        pushButton_textOpen->setText(QApplication::translate("MainWindow", "\347\241\256\345\256\232", 0));
        pushButton_textOpenCancel->setText(QApplication::translate("MainWindow", "\345\217\226\346\266\210", 0));
        pushButton_processreturn->setText(QApplication::translate("MainWindow", "\350\277\224\345\233\236", 0));
        pushButton_detail->setText(QApplication::translate("MainWindow", "\350\277\233\347\250\213\351\230\237\345\210\227\350\257\246\347\273\206", 0));
        pushButton_deleteprocess->setText(QApplication::translate("MainWindow", "\345\210\240\351\231\244\350\277\233\347\250\213", 0));
        pushButton_newprocess->setText(QApplication::translate("MainWindow", "\346\226\260\345\273\272\350\277\233\347\250\213", 0));
        label_Prowaitno->setText(QApplication::translate("MainWindow", "\347\255\211\345\276\205\351\230\237\345\210\227\344\270\255\347\232\204\350\277\233\347\250\213\346\225\260\357\274\232", 0));
        label_Proinusenu->setText(QApplication::translate("MainWindow", "\346\211\247\350\241\214\351\230\237\345\210\227\344\270\255\347\232\204\350\277\233\347\250\213\346\225\260:", 0));
        label_pwn->setText(QString());
        label_prn->setText(QString());
        label_22->setText(QApplication::translate("MainWindow", "\345\260\261\347\273\252\351\230\237\345\210\227\344\270\255\347\232\204\350\277\233\347\250\213\346\225\260\357\274\232", 0));
        label_proinrea->setText(QString());
        label_prun->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_Proinusenu_2->setText(QApplication::translate("MainWindow", "\347\216\260\345\234\250\346\255\243\345\234\250\350\277\220\350\241\214\347\232\204\350\277\233\347\250\213:", 0));
        pushButton_return->setText(QApplication::translate("MainWindow", "\350\277\224\345\233\236", 0));
        label_pids->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:11pt;\">PID\357\274\232</span></p></body></html>", 0));
        label_pid->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_10->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:10pt;\">\350\277\233\347\250\213\345\206\205\351\203\250\346\240\207\350\257\206\347\254\246</span></p></body></html>", 0));
        label_11->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:10pt;\">\350\277\233\347\250\213\345\244\226\351\203\250\346\240\207\350\257\206\347\254\246</span></p></body></html>", 0));
        label_Pronames->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:11pt;\">\345\220\215\347\247\260\357\274\232</span></p></body></html>", 0));
        label_pro->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_propath->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_12->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:10pt;\">\350\267\257\345\276\204\357\274\232</span></p></body></html>", 0));
        label_13->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:10pt;\">\350\277\233\347\250\213\345\244\204\347\220\206\346\234\272\347\212\266\346\200\201\344\277\241\346\201\257</span></p></body></html>", 0));
        label_pc->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:10pt;\">PC:</span></p></body></html>", 0));
        label_pcshow->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_pccpu->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_pccpus->setText(QApplication::translate("MainWindow", "\344\273\243\347\240\201\345\215\240\347\224\250\345\206\205\345\255\230\357\274\232", 0));
        label_miu->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_cpu->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_miut->setText(QApplication::translate("MainWindow", "\344\275\277\347\224\250\344\270\255\345\206\205\345\255\230\357\274\232", 0));
        label_cput->setText(QApplication::translate("MainWindow", "\357\274\243\357\274\260\357\274\265\357\274\232", 0));
        label_18->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:10pt;\">\350\277\233\347\250\213\350\260\203\345\272\246\344\277\241\346\201\257</span></p></body></html>", 0));
        label_14->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:10pt;\">\350\277\233\347\250\213\351\241\265\350\241\250</span></p></body></html>", 0));
        label_15->setText(QApplication::translate("MainWindow", "\350\277\233\347\250\213\351\241\265\345\217\267\357\274\232", 0));
        label_pn->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_17->setText(QApplication::translate("MainWindow", "\346\234\211\346\225\210\344\275\215\357\274\232", 0));
        label_isa->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_21->setText(QApplication::translate("MainWindow", "\345\215\240\347\224\250\345\270\247\345\217\267", 0));
        label_fra->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_pstate->setText(QApplication::translate("MainWindow", "\350\277\233\347\250\213\347\212\266\346\200\201\357\274\232", 0));
        label_state->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_16->setText(QApplication::translate("MainWindow", "\345\233\272\345\256\232\344\274\230\345\205\210\347\272\247:", 0));
        label_sppr->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_pdps->setText(QApplication::translate("MainWindow", "\345\212\250\346\200\201\344\274\230\345\205\210\347\272\247\357\274\232", 0));
        label_pdp->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_pcts->setText(QApplication::translate("MainWindow", "\350\277\233\347\250\213\345\210\233\345\273\272\346\227\266\351\227\264\357\274\232", 0));
        label_pct->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_pwts->setText(QApplication::translate("MainWindow", "\350\277\233\347\250\213\347\255\211\345\276\205\346\227\266\351\227\264\357\274\232", 0));
        label_pwt->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_prts->setText(QApplication::translate("MainWindow", "\350\277\233\347\250\213\350\277\220\350\241\214\346\227\266\351\227\264\357\274\232", 0));
        label_prt->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_19->setText(QApplication::translate("MainWindow", "\350\277\233\347\250\213\344\272\213\344\273\266\357\274\232", 0));
        label_event->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_33->setText(QString());
        label_36->setText(QString());
        label_32->setText(QString());
        label_28->setText(QString());
        label_38->setText(QString());
        label_30->setText(QString());
        label_34->setText(QString());
        label_27->setText(QString());
        label_39->setText(QString());
        label_40->setText(QString());
        label_31->setText(QString());
        label_37->setText(QString());
        label_43->setText(QString());
        label_42->setText(QString());
        label_35->setText(QString());
        label_29->setText(QString());
        label_41->setText(QString());
        label_44->setText(QApplication::translate("MainWindow", "\346\256\265\351\225\277\345\272\246\357\274\232", 0));
        label_dslen->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_45->setText(QApplication::translate("MainWindow", "\345\267\262\344\275\277\347\224\250\357\274\232", 0));
        label_dsuse->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_46->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:10pt;\">\346\256\265\344\277\241\346\201\257</span></p></body></html>", 0));
        label_47->setText(QApplication::translate("MainWindow", "\346\256\265\350\241\250\347\244\272\357\274\232", 0));
        label_48->setText(QString());
        pushButton_poconfirm->setText(QApplication::translate("MainWindow", "\347\241\256\345\256\232", 0));
        pushButton_pocancel->setText(QApplication::translate("MainWindow", "\345\217\226\346\266\210", 0));
        label_Processopentip->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:12pt;\">\350\257\267\350\276\223\345\205\245\350\246\201\346\211\223\345\274\200\347\232\204\350\277\233\347\250\213\346\226\207\344\273\266\350\267\257\345\276\204\357\274\232</span></p></body></html>", 0));
        label_23->setText(QApplication::translate("MainWindow", "\350\256\276\347\275\256\346\227\245\345\277\227\346\226\207\344\273\266\346\230\257\345\220\246\350\276\223\345\207\272", 0));
        label_24->setText(QApplication::translate("MainWindow", "\350\256\276\347\275\256\345\206\205\345\255\230\350\260\203\351\241\265\347\256\227\346\263\225", 0));
        label_25->setText(QApplication::translate("MainWindow", "\346\230\257\345\220\246\351\207\207\347\224\250\345\212\250\346\200\201\344\274\230\345\205\210\347\272\247", 0));
        checkBox_isLogOut->setText(QApplication::translate("MainWindow", "\346\230\257", 0));
        comboBox_PageChangeType->clear();
        comboBox_PageChangeType->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "FIFO", 0)
         << QApplication::translate("MainWindow", "LRU", 0)
        );
        checkBox_isDynamic->setText(QApplication::translate("MainWindow", "\346\230\257", 0));
        pushButton_MemM->setText(QApplication::translate("MainWindow", "\345\206\205\345\255\230\347\233\221\350\247\206\345\231\250", 0));
        pushButton_2->setText(QApplication::translate("MainWindow", "\344\273\273\345\212\241\347\256\241\347\220\206\345\231\250", 0));
        pushButton_explorer->setText(QApplication::translate("MainWindow", "\346\226\207\344\273\266\350\265\204\346\272\220\347\256\241\347\220\206\345\231\250", 0));
        pushButton_overview->setText(QApplication::translate("MainWindow", "\347\224\265\350\204\221\346\246\202\350\247\210", 0));
        pushButton_powerOff->setText(QApplication::translate("MainWindow", "\345\205\263\346\234\272", 0));
        pushButton_textEditor->setText(QApplication::translate("MainWindow", "\346\226\207\346\234\254\347\274\226\350\276\221\345\231\250", 0));
        pushButton_Set->setText(QApplication::translate("MainWindow", "\350\256\276\347\275\256", 0));
        label_26->setText(QApplication::translate("MainWindow", "\345\275\223\345\211\215\345\270\220\346\210\267:", 0));
        label_userName->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_time->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label_49->setText(QApplication::translate("MainWindow", "Copyright (c) 2016 Group 7, Class 08. All rights reserved", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
