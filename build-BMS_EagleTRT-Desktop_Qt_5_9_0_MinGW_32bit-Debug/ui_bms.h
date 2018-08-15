/********************************************************************************
** Form generated from reading UI file 'bms.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BMS_H
#define UI_BMS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BMS
{
public:
    QWidget *centralWidget;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QLabel *label;
    QPushButton *pushButton_3;
    QLabel *EagleImage;
    QMenuBar *menuBar;
    QMenu *menuBMS_Eagle_TRT;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *BMS)
    {
        if (BMS->objectName().isEmpty())
            BMS->setObjectName(QStringLiteral("BMS"));
        BMS->resize(1457, 1026);
        centralWidget = new QWidget(BMS);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(100, 800, 191, 91));
        QFont font;
        font.setPointSize(8);
        pushButton->setFont(font);
        pushButton_2 = new QPushButton(centralWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(630, 800, 191, 91));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(300, 800, 191, 91));
        label->setFont(font);
        pushButton_3 = new QPushButton(centralWidget);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setGeometry(QRect(910, 800, 191, 91));
        EagleImage = new QLabel(centralWidget);
        EagleImage->setObjectName(QStringLiteral("EagleImage"));
        EagleImage->setGeometry(QRect(10, 10, 1431, 771));
        BMS->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(BMS);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1457, 26));
        menuBMS_Eagle_TRT = new QMenu(menuBar);
        menuBMS_Eagle_TRT->setObjectName(QStringLiteral("menuBMS_Eagle_TRT"));
        BMS->setMenuBar(menuBar);
        mainToolBar = new QToolBar(BMS);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        BMS->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(BMS);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        BMS->setStatusBar(statusBar);

        menuBar->addAction(menuBMS_Eagle_TRT->menuAction());
        menuBMS_Eagle_TRT->addSeparator();

        retranslateUi(BMS);

        QMetaObject::connectSlotsByName(BMS);
    } // setupUi

    void retranslateUi(QMainWindow *BMS)
    {
        BMS->setWindowTitle(QApplication::translate("BMS", "BMS", Q_NULLPTR));
        pushButton->setText(QApplication::translate("BMS", "Connect The BMS", Q_NULLPTR));
        pushButton_2->setText(QApplication::translate("BMS", "Set a Profile", Q_NULLPTR));
        label->setText(QApplication::translate("BMS", "TextLabel", Q_NULLPTR));
        pushButton_3->setText(QApplication::translate("BMS", "Live Data", Q_NULLPTR));
        EagleImage->setText(QApplication::translate("BMS", "TextLabel", Q_NULLPTR));
        menuBMS_Eagle_TRT->setTitle(QApplication::translate("BMS", "BMS Eagle TRT", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class BMS: public Ui_BMS {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BMS_H
