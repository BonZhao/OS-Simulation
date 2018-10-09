#include "showhidechild.h"
#include "ui_showhidechild.h"


//extern QString hide;
showHideChild::showHideChild(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::showHideChild)
{
    ui->setupUi(this);
    ui->hideChild->setChecked(true);

}

showHideChild::~showHideChild()
{
    delete ui;
}


bool showHideChild::getCancelOperate() const
{
    return cancelOperate;
}

void showHideChild::setCancelOperate(bool value)
{
    cancelOperate = value;
}

bool showHideChild::getHideChild() const
{
    return hideChild;
}

void showHideChild::setHideChild(bool value)
{
    hideChild = value;
}

void showHideChild::on_radioButton_clicked()
{
    this->hideChild = false;
}

void showHideChild::on_hideChild_clicked()
{
    this->hideChild = true;
}


void showHideChild::on_pushButton_clicked()
{
    this->cancelOperate = false;
    close();
}

void showHideChild::on_cancel_clicked()
{
    this->cancelOperate = true;
    close();
}
