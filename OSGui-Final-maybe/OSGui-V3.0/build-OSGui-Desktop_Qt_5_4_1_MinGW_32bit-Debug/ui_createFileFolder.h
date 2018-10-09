/********************************************************************************
** Form generated from reading UI file 'createFileFolder.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CREATEFILEFOLDER_H
#define UI_CREATEFILEFOLDER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_createFileFolder
{
public:
    QPushButton *cancel;
    QTextEdit *name;
    QRadioButton *file;
    QRadioButton *folder;
    QPushButton *pushButton;
    QLabel *label;

    void setupUi(QDialog *createFileFolder)
    {
        if (createFileFolder->objectName().isEmpty())
            createFileFolder->setObjectName(QStringLiteral("createFileFolder"));
        createFileFolder->resize(391, 230);
        cancel = new QPushButton(createFileFolder);
        cancel->setObjectName(QStringLiteral("cancel"));
        cancel->setGeometry(QRect(200, 170, 75, 23));
        name = new QTextEdit(createFileFolder);
        name->setObjectName(QStringLiteral("name"));
        name->setGeometry(QRect(140, 80, 141, 31));
        file = new QRadioButton(createFileFolder);
        file->setObjectName(QStringLiteral("file"));
        file->setGeometry(QRect(70, 20, 91, 31));
        folder = new QRadioButton(createFileFolder);
        folder->setObjectName(QStringLiteral("folder"));
        folder->setGeometry(QRect(200, 20, 91, 31));
        pushButton = new QPushButton(createFileFolder);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(70, 170, 75, 23));
        label = new QLabel(createFileFolder);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(70, 90, 51, 21));

        retranslateUi(createFileFolder);

        QMetaObject::connectSlotsByName(createFileFolder);
    } // setupUi

    void retranslateUi(QDialog *createFileFolder)
    {
        createFileFolder->setWindowTitle(QApplication::translate("createFileFolder", "Dialog", 0));
        cancel->setText(QApplication::translate("createFileFolder", "\345\217\226\346\266\210", 0));
        file->setText(QApplication::translate("createFileFolder", "\346\226\207\344\273\266", 0));
        folder->setText(QApplication::translate("createFileFolder", "\346\226\207\344\273\266\345\244\271", 0));
        pushButton->setText(QApplication::translate("createFileFolder", "\347\241\256\345\256\232", 0));
        label->setText(QApplication::translate("createFileFolder", "\350\276\223\345\205\245\345\220\215\345\255\227", 0));
    } // retranslateUi

};

namespace Ui {
    class createFileFolder: public Ui_createFileFolder {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CREATEFILEFOLDER_H
