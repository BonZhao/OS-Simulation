#include "createFileFolder.h"
#include "ui_createFileFolder.h"

createFileFolder::createFileFolder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::createFileFolder)
{
    ui->setupUi(this);
    this->type = 0;
    this->name = "";
    ui->file->setChecked(true);
}

createFileFolder::~createFileFolder()
{
    delete ui;
}

void createFileFolder::on_file_clicked()
{
    type = 0;
}

void createFileFolder::on_folder_clicked()
{
    type = 1;
}

void createFileFolder::on_pushButton_clicked()
{
    this->name = ui->name->toPlainText();
    close();
}

void createFileFolder::on_cancel_clicked()
{
    this->name = "";
    this->type = 2;
    close();
}
QString createFileFolder::getName() const
{
    return name;
}

void createFileFolder::setName(const QString &value)
{
    name = value;
}

int createFileFolder::getType() const
{
    return type;
}

void createFileFolder::setType(int value)
{
    type = value;
}

