/********************************************************************************
** Form generated from reading UI file 'showAttribute.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHOWATTRIBUTE_H
#define UI_SHOWATTRIBUTE_H

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

class Ui_showAttribute
{
public:
    QTextBrowser *length;
    QLabel *label;
    QLabel *label_5;
    QLabel *label_2;
    QTextBrowser *type;
    QTextBrowser *place;
    QTextBrowser *occupyPlace;
    QLabel *label_4;
    QTextBrowser *createTime;
    QLabel *label_6;
    QLabel *label_7;
    QLineEdit *name;
    QPushButton *pushButton;
    QCheckBox *protect;
    QCheckBox *hide;
    QLabel *att;
    QLabel *max;
    QTextBrowser *maxSize;

    void setupUi(QDialog *showAttribute)
    {
        if (showAttribute->objectName().isEmpty())
            showAttribute->setObjectName(QStringLiteral("showAttribute"));
        showAttribute->resize(443, 411);
        length = new QTextBrowser(showAttribute);
        length->setObjectName(QStringLiteral("length"));
        length->setGeometry(QRect(130, 190, 181, 31));
        label = new QLabel(showAttribute);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(50, 90, 71, 31));
        label_5 = new QLabel(showAttribute);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(50, 240, 71, 31));
        label_2 = new QLabel(showAttribute);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(50, 140, 71, 31));
        type = new QTextBrowser(showAttribute);
        type->setObjectName(QStringLiteral("type"));
        type->setGeometry(QRect(130, 90, 181, 31));
        place = new QTextBrowser(showAttribute);
        place->setObjectName(QStringLiteral("place"));
        place->setGeometry(QRect(130, 140, 181, 31));
        occupyPlace = new QTextBrowser(showAttribute);
        occupyPlace->setObjectName(QStringLiteral("occupyPlace"));
        occupyPlace->setGeometry(QRect(130, 240, 181, 31));
        label_4 = new QLabel(showAttribute);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(50, 200, 71, 21));
        createTime = new QTextBrowser(showAttribute);
        createTime->setObjectName(QStringLiteral("createTime"));
        createTime->setGeometry(QRect(130, 290, 181, 31));
        label_6 = new QLabel(showAttribute);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(50, 290, 71, 21));
        label_7 = new QLabel(showAttribute);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(50, 40, 71, 31));
        name = new QLineEdit(showAttribute);
        name->setObjectName(QStringLiteral("name"));
        name->setGeometry(QRect(130, 39, 181, 31));
        pushButton = new QPushButton(showAttribute);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(324, 380, 91, 31));
        protect = new QCheckBox(showAttribute);
        protect->setObjectName(QStringLiteral("protect"));
        protect->setGeometry(QRect(130, 340, 81, 21));
        hide = new QCheckBox(showAttribute);
        hide->setObjectName(QStringLiteral("hide"));
        hide->setGeometry(QRect(230, 340, 81, 21));
        att = new QLabel(showAttribute);
        att->setObjectName(QStringLiteral("att"));
        att->setGeometry(QRect(50, 340, 71, 21));
        max = new QLabel(showAttribute);
        max->setObjectName(QStringLiteral("max"));
        max->setGeometry(QRect(50, 340, 71, 21));
        maxSize = new QTextBrowser(showAttribute);
        maxSize->setObjectName(QStringLiteral("maxSize"));
        maxSize->setGeometry(QRect(130, 340, 181, 31));

        retranslateUi(showAttribute);

        QMetaObject::connectSlotsByName(showAttribute);
    } // setupUi

    void retranslateUi(QDialog *showAttribute)
    {
        showAttribute->setWindowTitle(QApplication::translate("showAttribute", "Dialog", 0));
        label->setText(QApplication::translate("showAttribute", "\346\226\207\344\273\266\347\261\273\345\236\213", 0));
        label_5->setText(QApplication::translate("showAttribute", "\345\215\240\347\224\250\347\251\272\351\227\264", 0));
        label_2->setText(QApplication::translate("showAttribute", "\344\275\215\347\275\256", 0));
        label_4->setText(QApplication::translate("showAttribute", "\345\244\247\345\260\217", 0));
        label_6->setText(QApplication::translate("showAttribute", "\345\210\233\345\273\272\346\227\266\351\227\264", 0));
        label_7->setText(QApplication::translate("showAttribute", "\345\220\215\345\255\227", 0));
        pushButton->setText(QApplication::translate("showAttribute", "\347\241\256\345\256\232", 0));
        protect->setText(QApplication::translate("showAttribute", "\344\277\235\346\212\244", 0));
        hide->setText(QApplication::translate("showAttribute", "\351\232\220\350\227\217", 0));
        att->setText(QApplication::translate("showAttribute", "\345\261\236\346\200\247", 0));
        max->setText(QApplication::translate("showAttribute", "\346\234\200\345\244\247\345\256\271\351\207\217", 0));
    } // retranslateUi

};

namespace Ui {
    class showAttribute: public Ui_showAttribute {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHOWATTRIBUTE_H
