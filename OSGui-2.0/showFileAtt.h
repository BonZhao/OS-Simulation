#ifndef SHOWFILEATT_H
#define SHOWFILEATT_H

#include <QDialog>

namespace Ui {
class showFileAtt;
}

class showFileAtt : public QDialog
{
    Q_OBJECT

public:
    explicit showFileAtt(QWidget *parent = 0);
    ~showFileAtt();

    QString getNewName() const;
    void setNewName(const QString &value);

    bool getOriginalReadonly() const;
    void setOriginalReadonly(bool value);

    bool getOriginalHide() const;
    void setOriginalHide(bool value);

    bool getReadOnly() const;
    void setReadOnly(bool value);

    bool getHide() const;
    void setHide(bool value);

private slots:
    void on_pushButton_clicked();

    void on_readOnly_stateChanged(int arg1);

    void on_hide_stateChanged(int arg1);

private:
    Ui::showFileAtt *ui;
    char *path;
    QString newName;

    bool originalReadonly; //文件原始状态
    bool originalHide;

    bool readOnly;  //文件最后的状态
    bool hide;
};

#endif // SHOWFILEATT_H
