/********************************************************************************
** Form generated from reading UI file 'showhidechild.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHOWHIDECHILD_H
#define UI_SHOWHIDECHILD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_showHideChild
{
public:
    QLabel *label_3;
    QLabel *label_4;
    QRadioButton *radioButton;
    QRadioButton *hideChild;
    QPushButton *pushButton;
    QPushButton *cancel;

    void setupUi(QDialog *showHideChild)
    {
        if (showHideChild->objectName().isEmpty())
            showHideChild->setObjectName(QStringLiteral("showHideChild"));
        showHideChild->resize(360, 212);
        label_3 = new QLabel(showHideChild);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 10, 391, 21));
        label_4 = new QLabel(showHideChild);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 40, 371, 16));
        radioButton = new QRadioButton(showHideChild);
        radioButton->setObjectName(QStringLiteral("radioButton"));
        radioButton->setGeometry(QRect(50, 80, 161, 17));
        hideChild = new QRadioButton(showHideChild);
        hideChild->setObjectName(QStringLiteral("hideChild"));
        hideChild->setGeometry(QRect(50, 110, 301, 17));
        pushButton = new QPushButton(showHideChild);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(180, 180, 75, 23));
        cancel = new QPushButton(showHideChild);
        cancel->setObjectName(QStringLiteral("cancel"));
        cancel->setGeometry(QRect(270, 180, 75, 23));

        retranslateUi(showHideChild);

        QMetaObject::connectSlotsByName(showHideChild);
    } // setupUi

    void retranslateUi(QDialog *showHideChild)
    {
        showHideChild->setWindowTitle(QApplication::translate("showHideChild", "Dialog", 0));
        label_3->setText(QApplication::translate("showHideChild", "\346\230\257\345\220\246\350\246\201\345\260\206\346\255\244\346\233\264\346\224\271\344\273\205\345\272\224\347\224\250\344\272\216\346\255\244\346\226\207\344\273\266\345\244\271\357\274\214\346\210\226\350\200\205\346\230\257\345\220\246\350\246\201\345\260\206\345\256\203\345\272\224\347\224\250\344\272\216\346\211\200\346\234\211\345\255\220", 0));
        label_4->setText(QApplication::translate("showHideChild", "\346\226\207\344\273\266\345\244\271\345\222\214\346\226\207\344\273\266\357\274\237", 0));
        radioButton->setText(QApplication::translate("showHideChild", "\344\273\205\345\260\206\346\233\264\346\224\271\345\272\224\347\224\250\344\272\216\346\255\244\346\226\207\344\273\266\345\244\271", 0));
        hideChild->setText(QApplication::translate("showHideChild", "\345\260\206\346\233\264\346\224\271\345\272\224\347\224\250\344\272\216\346\211\200\346\234\211\346\255\244\346\226\207\344\273\266\345\244\271\357\274\214\345\255\220\346\226\207\344\273\266\345\244\271\345\222\214\346\226\207\344\273\266", 0));
        pushButton->setText(QApplication::translate("showHideChild", "\347\241\256\345\256\232", 0));
        cancel->setText(QApplication::translate("showHideChild", "\345\217\226\346\266\210", 0));
    } // retranslateUi

};

namespace Ui {
    class showHideChild: public Ui_showHideChild {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHOWHIDECHILD_H
