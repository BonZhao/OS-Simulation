#ifndef SHOWATTRIBUTE_H
#define SHOWATTRIBUTE_H

#include <QDialog>

namespace Ui {
class showAttribute;
}

class showAttribute : public QDialog
{
    Q_OBJECT

public:
    explicit showAttribute(QWidget *parent = 0);
    ~showAttribute();
    int computeLength(char *__path);
    int computeOccupyLength(char *__path);

    QString getNewName() const;
    void setNewName(const QString &value);

    bool getOriginalHide() const;
    void setOriginalHide(bool value);

    bool getOriginalProtect() const;
    void setOriginalProtect(bool value);

    bool getProtect() const;
    void setProtect(bool value);

    bool getHide() const;
    void setHide(bool value);

    bool getChild() const;
    void setChild(bool value);

    bool getCancelOperate() const;
    void setCancelOperate(bool value);

private slots:
    void on_pushButton_clicked();

    void on_protect_stateChanged(int arg1);

    void on_hide_stateChanged(int arg1);

private:
    Ui::showAttribute *ui;
    char *path;
    QString newName;

    bool originalProtect; //文件原始状态
    bool originalHide;

    bool protect;  //文件最后的状态
    bool hide;

    bool child;

    bool cancelOperate; //取消操作

};

#endif // SHOWATTRIBUTE_H
