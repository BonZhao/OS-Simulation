/********************************************************************************
** Form generated from reading UI file 'showDelete.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHOWDELETE_H
#define UI_SHOWDELETE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_showDelete
{
public:
    QPushButton *pushButton;
    QPushButton *cancel;
    QLabel *label;

    void setupUi(QDialog *showDelete)
    {
        if (showDelete->objectName().isEmpty())
            showDelete->setObjectName(QStringLiteral("showDelete"));
        showDelete->resize(324, 151);
        pushButton = new QPushButton(showDelete);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(70, 100, 75, 23));
        cancel = new QPushButton(showDelete);
        cancel->setObjectName(QStringLiteral("cancel"));
        cancel->setGeometry(QRect(180, 100, 75, 23));
        label = new QLabel(showDelete);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(70, 20, 181, 61));

        retranslateUi(showDelete);

        QMetaObject::connectSlotsByName(showDelete);
    } // setupUi

    void retranslateUi(QDialog *showDelete)
    {
        showDelete->setWindowTitle(QApplication::translate("showDelete", "Dialog", 0));
        pushButton->setText(QApplication::translate("showDelete", "\347\241\256\345\256\232", 0));
        cancel->setText(QApplication::translate("showDelete", "\345\217\226\346\266\210", 0));
        label->setText(QApplication::translate("showDelete", "\346\230\257\345\220\246\347\241\256\350\256\244\345\210\240\351\231\244\357\274\237", 0));
    } // retranslateUi

};

namespace Ui {
    class showDelete: public Ui_showDelete {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHOWDELETE_H
