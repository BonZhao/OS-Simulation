#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QMainWindow>
#include <QTableWidget>
#include "workthread.h"
#include <QPushButton>
#include <QTreeWidgetItem>
#include<QTableWidgetItem>
#include"ProcessManagement.h"
#include "Account.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void printErrorGui();
    void createChildFileSystem(char *__path,QTreeWidgetItem *parent);
    void showFileFolder();
    void hideFileFolder();
    void changeChildAtt(char *__path,bool __hideOrProtect,int type);
    void clay(QTreeWidgetItem *haveChild,QTreeWidgetItem *source_item,bool cut);
    void joinChildHide(QTreeWidgetItem *Child);

    void renovateFileAtt(char*); //刷新文件信息
    QTreeWidgetItem* FindItem(QTreeWidgetItem*,char*);
    void renovateFileSystem();
    void showFileWithNoHint(QString __textPath);
    void showTime();
private slots:


    void on_pushButton_MemM_clicked();

    void on_pushButton_overview_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_pushButton_explorer_clicked();

    void on_pushButton_2_clicked();

    void onChanged(void);
    void on_pushButton_powerOff_clicked();
    void showInfo();
    void showInfo2();
    void on_pushButton_textEditor_clicked();

    void on_pushButton_textEditor_save_clicked();

    void on_pushButton_textCancel_clicked();

    void on_pushButton_textSave_clicked();

    void on_pushButton_textEditor_open_clicked();

    void on_pushButton_textOpenCancel_clicked();

    void on_pushButton_textOpen_clicked();

    void openFiley();
    void checkself(QTreeWidgetItem *,int);
    void onFileMenuTriggered(QAction *);
    void onFolderMenuTriggered(QAction *);
    void showMenu(QTreeWidgetItem *item__, int);

    void on_pushButton_toHex_clicked();

    void on_pushButton_detail_clicked();

    void on_pushButton_processreturn_clicked();

    void on_pushButton_newprocess_clicked();

    void on_pushButton_deleteprocess_clicked();

    void on_pushButton_return_clicked();

    void on_pushButton_poconfirm_clicked();

    void on_pushButton_pocancel_clicked();

    void showMenu(QTableWidgetItem*item);

    void onProMenuTriggered(QAction *action);

    void showProtab();

    void showdettab();

    void showfirstpage();

    void infosave(QTableWidgetItem*item);

    void on_pushButton_textEditor_saveAs_clicked();

    void on_pushButton_compile_clicked();
    void on_checkBox_stateChanged(int arg1);

    void on_pushButton_Set_clicked();

    void on_checkBox_isLogOut_clicked();

    void on_comboBox_PageChangeType_currentIndexChanged(int index);

    void on_checkBox_isDynamic_clicked();

    void showdslabel(int num, int label, int len);

    void cleardet();

    void on_pushButton_explorerRefresh_clicked();

private:
    Ui::MainWindow *ui;
    void showSystemInfo();
    void showMemTable();
    void showVMemTable();
    void showPageFault();
    void createItemMenu(); //构造函数中调用此函数
    void createFileSystem();
    void showFile(QString);

        bool isFile;

        QMenu *fileMenu__;
        QMenu *folderMenu__;

        QAction *create__;
        QAction *clay__;
        QAction *shear__;
        QAction *copy__;
        QAction *delete__;
        QAction *rename__;
        QAction *property__;

        QAction *shear__1;
        QAction *copy__1;
        QAction *delete__1;
        QAction *rename__1;
        QAction *property__1;

        QAction *open__1;

        QString fileName;
        QString source_path;  //原路径
        QString dest_path;    //目的路径
        int type;
        int clay_time;        //粘贴次数
        int renovate;        //刷新次数

        QTreeWidgetItem *source_item;
        bool showHideFileFolder ; //是否显示隐藏的文件和文件夹 //111111
        QTreeWidgetItem *itemC;
        //Process table
           QMenu *proMenu;//右键菜单
           QAction *action_detailed;//显示进程详细选项
           QAction *action_killed;//结束进程选项
           ProcessID PID_temp;//PID缓存
           QString filepath;//文件路径

           void createPItemMenu();//右键菜单创建
           void createProtab();//
            void infosave_2(QTableWidgetItem*item);

           //相关关信息记录
          void showUserName();
};



#endif // MAINWINDOW_H
