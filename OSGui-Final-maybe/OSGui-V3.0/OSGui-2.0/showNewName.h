#ifndef SHOWNEWNAME_H
#define SHOWNEWNAME_H

#include <QDialog>

namespace Ui {
class showNewName;
}

class showNewName : public QDialog
{
    Q_OBJECT

public:
    explicit showNewName(QWidget *parent = 0);
    ~showNewName();

    QString getNewName() const;
    void setNewName(const QString &value);

private slots:
    void on_pushButton_clicked();

    void on_cancel_clicked();

private:
    Ui::showNewName *ui;
    QString newName;
};

#endif // SHOWNEWNAME_H
