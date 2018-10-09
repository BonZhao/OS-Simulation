/********************************************************************************
** Form generated from reading UI file 'showNewName.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHOWNEWNAME_H
#define UI_SHOWNEWNAME_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_showNewName
{
public:
    QPushButton *cancel;
    QLabel *label;
    QPushButton *pushButton;
    QLineEdit *lineEdit;

    void setupUi(QDialog *showNewName)
    {
        if (showNewName->objectName().isEmpty())
            showNewName->setObjectName(QStringLiteral("showNewName"));
        showNewName->resize(338, 166);
        cancel = new QPushButton(showNewName);
        cancel->setObjectName(QStringLiteral("cancel"));
        cancel->setGeometry(QRect(240, 120, 75, 23));
        label = new QLabel(showNewName);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 40, 101, 31));
        pushButton = new QPushButton(showNewName);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(130, 120, 75, 23));
        lineEdit = new QLineEdit(showNewName);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(130, 40, 181, 31));

        retranslateUi(showNewName);

        QMetaObject::connectSlotsByName(showNewName);
    } // setupUi

    void retranslateUi(QDialog *showNewName)
    {
        showNewName->setWindowTitle(QApplication::translate("showNewName", "Dialog", 0));
        cancel->setText(QApplication::translate("showNewName", "\345\217\226\346\266\210", 0));
        label->setText(QApplication::translate("showNewName", "\350\257\267\350\276\223\345\205\245\346\226\260\345\220\215\345\255\227", 0));
        pushButton->setText(QApplication::translate("showNewName", "\347\241\256\345\256\232", 0));
    } // retranslateUi

};

namespace Ui {
    class showNewName: public Ui_showNewName {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHOWNEWNAME_H
