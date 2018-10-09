#ifndef SHOWHIDECHILD_H
#define SHOWHIDECHILD_H

#include <QDialog>

namespace Ui {
class showHideChild;
}

class showHideChild : public QDialog
{
    Q_OBJECT

public:
    explicit showHideChild(QWidget *parent = 0);
    ~showHideChild();

    bool getHideChild() const;
    void setHideChild(bool value);

    bool getCancelOperate() const;
    void setCancelOperate(bool value);

private slots:
    void on_radioButton_clicked();

    void on_hideChild_clicked();

    void on_pushButton_clicked();

    void on_cancel_clicked();

private:
    Ui::showHideChild *ui;

    bool hideChild; //是否隐藏子文件夹和子文件

    bool cancelOperate; //取消操作
};

#endif // SHOWHIDECHILD_H
