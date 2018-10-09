#include "showNewName.h"
#include "ui_showNewName.h"

showNewName::showNewName(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::showNewName)
{
    ui->setupUi(this);
    this->newName = "";
}

showNewName::~showNewName()
{
    delete ui;
}

void showNewName::on_pushButton_clicked()
{
    this->newName = ui->lineEdit->text();
    close();
}

void showNewName::on_cancel_clicked()
{
    this->newName = "";
    close();
}
QString showNewName::getNewName() const
{
    return newName;
}

void showNewName::setNewName(const QString &value)
{
    newName = value;
}

