/********************************************************************************
** Form generated from reading UI file 'showFileAtt.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHOWFILEATT_H
#define UI_SHOWFILEATT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_showFileAtt
{
public:
    QPushButton *pushButton;
    QLabel *label;
    QLabel *label_7;
    QLabel *label_4;
    QLabel *label_8;
    QTextBrowser *place;
    QLabel *label_2;
    QLineEdit *name;
    QTextBrowser *type;
    QTextBrowser *occupyPlace;
    QTextBrowser *length;
    QTextBrowser *createTime;
    QLabel *label_9;
    QTextBrowser *changeTime;
    QLabel *label_10;
    QTextBrowser *visitTime;
    QLabel *label_11;
    QLabel *label_12;
    QCheckBox *readOnly;
    QCheckBox *hide;

    void setupUi(QDialog *showFileAtt)
    {
        if (showFileAtt->objectName().isEmpty())
            showFileAtt->setObjectName(QStringLiteral("showFileAtt"));
        showFileAtt->resize(448, 483);
        pushButton = new QPushButton(showFileAtt);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(310, 430, 91, 31));
        label = new QLabel(showFileAtt);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 71, 71, 31));
        label_7 = new QLabel(showFileAtt);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(30, 21, 71, 31));
        label_4 = new QLabel(showFileAtt);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(30, 181, 71, 21));
        label_8 = new QLabel(showFileAtt);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(30, 221, 71, 21));
        place = new QTextBrowser(showFileAtt);
        place->setObjectName(QStringLiteral("place"));
        place->setGeometry(QRect(110, 121, 181, 31));
        label_2 = new QLabel(showFileAtt);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(30, 121, 71, 31));
        name = new QLineEdit(showFileAtt);
        name->setObjectName(QStringLiteral("name"));
        name->setGeometry(QRect(110, 20, 181, 31));
        type = new QTextBrowser(showFileAtt);
        type->setObjectName(QStringLiteral("type"));
        type->setGeometry(QRect(110, 71, 181, 31));
        occupyPlace = new QTextBrowser(showFileAtt);
        occupyPlace->setObjectName(QStringLiteral("occupyPlace"));
        occupyPlace->setGeometry(QRect(110, 221, 181, 31));
        length = new QTextBrowser(showFileAtt);
        length->setObjectName(QStringLiteral("length"));
        length->setGeometry(QRect(110, 171, 181, 31));
        createTime = new QTextBrowser(showFileAtt);
        createTime->setObjectName(QStringLiteral("createTime"));
        createTime->setGeometry(QRect(110, 271, 181, 31));
        label_9 = new QLabel(showFileAtt);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(30, 271, 71, 21));
        changeTime = new QTextBrowser(showFileAtt);
        changeTime->setObjectName(QStringLiteral("changeTime"));
        changeTime->setGeometry(QRect(110, 320, 181, 31));
        label_10 = new QLabel(showFileAtt);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(30, 320, 71, 21));
        visitTime = new QTextBrowser(showFileAtt);
        visitTime->setObjectName(QStringLiteral("visitTime"));
        visitTime->setGeometry(QRect(110, 370, 181, 31));
        label_11 = new QLabel(showFileAtt);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(30, 370, 71, 21));
        label_12 = new QLabel(showFileAtt);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(30, 420, 71, 21));
        readOnly = new QCheckBox(showFileAtt);
        readOnly->setObjectName(QStringLiteral("readOnly"));
        readOnly->setGeometry(QRect(110, 420, 81, 21));
        hide = new QCheckBox(showFileAtt);
        hide->setObjectName(QStringLiteral("hide"));
        hide->setGeometry(QRect(210, 420, 81, 21));

        retranslateUi(showFileAtt);

        QMetaObject::connectSlotsByName(showFileAtt);
    } // setupUi

    void retranslateUi(QDialog *showFileAtt)
    {
        showFileAtt->setWindowTitle(QApplication::translate("showFileAtt", "Dialog", 0));
        pushButton->setText(QApplication::translate("showFileAtt", "\347\241\256\345\256\232", 0));
        label->setText(QApplication::translate("showFileAtt", "\346\226\207\344\273\266\347\261\273\345\236\213", 0));
        label_7->setText(QApplication::translate("showFileAtt", "\345\220\215\345\255\227", 0));
        label_4->setText(QApplication::translate("showFileAtt", "\345\244\247\345\260\217", 0));
        label_8->setText(QApplication::translate("showFileAtt", "\345\215\240\347\224\250\347\251\272\351\227\264", 0));
        label_2->setText(QApplication::translate("showFileAtt", "\344\275\215\347\275\256", 0));
        label_9->setText(QApplication::translate("showFileAtt", "\345\210\233\345\273\272\346\227\266\351\227\264", 0));
        label_10->setText(QApplication::translate("showFileAtt", "\344\277\256\346\224\271\346\227\266\351\227\264", 0));
        label_11->setText(QApplication::translate("showFileAtt", "\350\256\277\351\227\256\346\227\266\351\227\264", 0));
        label_12->setText(QApplication::translate("showFileAtt", "\345\261\236\346\200\247", 0));
        readOnly->setText(QApplication::translate("showFileAtt", "\345\217\252\350\257\273", 0));
        hide->setText(QApplication::translate("showFileAtt", "\351\232\220\350\227\217", 0));
    } // retranslateUi

};

namespace Ui {
    class showFileAtt: public Ui_showFileAtt {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHOWFILEATT_H
