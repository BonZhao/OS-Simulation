#include "login.h"
#include "ui_login.h"
#include <QDebug>
extern QString userName;

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    ui->lineEdit_loginPassword->setEchoMode(QLineEdit::Password);
}

Login::~Login()
{
    delete ui;
}

void Login::on_pushButton_clicked()
{

    if(checkAccount(ui->lineEdit_loginName->text().toStdString().c_str(),ui->lineEdit_loginPassword->text().toStdString().c_str())) {
        userName = ui->lineEdit_loginName->text();
        //qDebug()<<userName;
        accept();
    }
    else {
        QMessageBox::warning(this,tr("警告"),tr("用户名或密码错误!"),QMessageBox::Yes);
        ui->lineEdit_loginName->clear();
        ui->lineEdit_loginPassword->clear();
        ui->lineEdit_loginName->setFocus();
    }
}

void Login::on_pushButton_2_clicked()
{
    this->close();
}
