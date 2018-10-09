#ifndef CREATEFILEFOLDER_H
#define CREATEFILEFOLDER_H

#include <QDialog>
#include <QString>
namespace Ui {
class createFileFolder;
}

class createFileFolder : public QDialog
{
    Q_OBJECT

public:
    explicit createFileFolder(QWidget *parent = 0);
    ~createFileFolder();

    int getType() const;
    void setType(int value);

    QString getName() const;
    void setName(const QString &value);

private slots:
    void on_file_clicked();

    void on_folder_clicked();

    void on_pushButton_clicked();

    void on_cancel_clicked();

private:
    Ui::createFileFolder *ui;
    int type;
    QString name;
};

#endif // CREATEFILEFOLDER_H
