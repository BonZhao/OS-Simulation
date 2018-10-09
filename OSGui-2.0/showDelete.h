#ifndef SHOWDELETE_H
#define SHOWDELETE_H

#include <QDialog>

namespace Ui {
class showDelete;
}

class showDelete : public QDialog
{
    Q_OBJECT

public:
    explicit showDelete(QWidget *parent = 0);
    ~showDelete();

    bool getIsDelete() const;
    void setIsDelete(bool value);

private slots:
    void on_pushButton_clicked();

    void on_cancel_clicked();

private:
    Ui::showDelete *ui;
    bool isDelete;
};

#endif // SHOWDELETE_H
