#include "showDelete.h"
#include "ui_showDelete.h"
#include <QDebug>
showDelete::showDelete(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::showDelete)
{
    ui->setupUi(this);
    this->isDelete = false;
}

showDelete::~showDelete()
{
    delete ui;
}

void showDelete::on_pushButton_clicked()
{
    this->isDelete = true;

    close();
}

void showDelete::on_cancel_clicked()
{
    this->isDelete = false;
    close();
}
bool showDelete::getIsDelete() const
{
    return isDelete;
}

void showDelete::setIsDelete(bool value)
{
    isDelete = value;
}

