/********************************************************************************
** Form generated from reading UI file 'configure.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGURE_H
#define UI_CONFIGURE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Configure
{
public:
    QTabWidget *MaximumVoltageChargingSet;
    QWidget *tab_2;
    QWidget *tab;
    QDoubleSpinBox *MaximumVoltageSet;
    QDoubleSpinBox *MinimumVoltageSet;
    QDoubleSpinBox *doubleSpinBox_8;
    QDoubleSpinBox *doubleSpinBox_10;
    QDoubleSpinBox *StartBalOver;
    QDoubleSpinBox *MinimumVoltageSet_2;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_12;
    QWidget *tab_6;
    QWidget *tab_4;
    QWidget *tab_3;
    QDoubleSpinBox *MinimumVoltageSet_3;
    QLabel *label_13;
    QLabel *label_14;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_17;
    QLabel *label_18;
    QLabel *label_19;
    QDoubleSpinBox *MinimumVoltageSet_4;
    QLabel *label_20;
    QLabel *label_21;
    QLabel *label_22;
    QLabel *label_23;
    QDoubleSpinBox *doubleSpinBox_11;
    QDoubleSpinBox *doubleSpinBox_9;
    QLabel *label_24;
    QDoubleSpinBox *StartBalOver_2;
    QDoubleSpinBox *MaximumVoltageSet_2;
    QWidget *tab_5;

    void setupUi(QWidget *Configure)
    {
        if (Configure->objectName().isEmpty())
            Configure->setObjectName(QStringLiteral("Configure"));
        Configure->resize(1292, 660);
        MaximumVoltageChargingSet = new QTabWidget(Configure);
        MaximumVoltageChargingSet->setObjectName(QStringLiteral("MaximumVoltageChargingSet"));
        MaximumVoltageChargingSet->setGeometry(QRect(40, 10, 1221, 631));
        MaximumVoltageChargingSet->setLayoutDirection(Qt::LeftToRight);
        MaximumVoltageChargingSet->setUsesScrollButtons(true);
        MaximumVoltageChargingSet->setDocumentMode(false);
        MaximumVoltageChargingSet->setTabsClosable(false);
        MaximumVoltageChargingSet->setMovable(true);
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        MaximumVoltageChargingSet->addTab(tab_2, QString());
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        MaximumVoltageSet = new QDoubleSpinBox(tab);
        MaximumVoltageSet->setObjectName(QStringLiteral("MaximumVoltageSet"));
        MaximumVoltageSet->setGeometry(QRect(350, 140, 151, 61));
        QFont font;
        font.setPointSize(20);
        MaximumVoltageSet->setFont(font);
        MaximumVoltageSet->setMaximum(15);
        MinimumVoltageSet = new QDoubleSpinBox(tab);
        MinimumVoltageSet->setObjectName(QStringLiteral("MinimumVoltageSet"));
        MinimumVoltageSet->setGeometry(QRect(350, 250, 151, 61));
        MinimumVoltageSet->setFont(font);
        doubleSpinBox_8 = new QDoubleSpinBox(tab);
        doubleSpinBox_8->setObjectName(QStringLiteral("doubleSpinBox_8"));
        doubleSpinBox_8->setGeometry(QRect(350, 360, 151, 61));
        doubleSpinBox_8->setFont(font);
        doubleSpinBox_10 = new QDoubleSpinBox(tab);
        doubleSpinBox_10->setObjectName(QStringLiteral("doubleSpinBox_10"));
        doubleSpinBox_10->setGeometry(QRect(960, 360, 151, 61));
        doubleSpinBox_10->setFont(font);
        StartBalOver = new QDoubleSpinBox(tab);
        StartBalOver->setObjectName(QStringLiteral("StartBalOver"));
        StartBalOver->setGeometry(QRect(960, 140, 151, 61));
        StartBalOver->setFont(font);
        MinimumVoltageSet_2 = new QDoubleSpinBox(tab);
        MinimumVoltageSet_2->setObjectName(QStringLiteral("MinimumVoltageSet_2"));
        MinimumVoltageSet_2->setGeometry(QRect(960, 250, 151, 61));
        MinimumVoltageSet_2->setFont(font);
        label = new QLabel(tab);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 130, 321, 71));
        QFont font1;
        font1.setPointSize(18);
        label->setFont(font1);
        label_2 = new QLabel(tab);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(30, 240, 321, 71));
        label_2->setFont(font1);
        label_3 = new QLabel(tab);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(30, 350, 321, 71));
        label_3->setFont(font1);
        label_4 = new QLabel(tab);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(600, 240, 361, 71));
        QFont font2;
        font2.setPointSize(14);
        label_4->setFont(font2);
        label_5 = new QLabel(tab);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(600, 350, 361, 71));
        label_5->setFont(font1);
        label_6 = new QLabel(tab);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(600, 130, 361, 71));
        label_6->setFont(font1);
        label_7 = new QLabel(tab);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(520, 140, 51, 41));
        label_7->setFont(font);
        label_8 = new QLabel(tab);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(520, 260, 51, 41));
        label_8->setFont(font);
        label_9 = new QLabel(tab);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(520, 370, 51, 41));
        label_9->setFont(font);
        label_10 = new QLabel(tab);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(1130, 160, 51, 41));
        label_10->setFont(font);
        label_11 = new QLabel(tab);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(1130, 270, 51, 41));
        label_11->setFont(font);
        label_12 = new QLabel(tab);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(1130, 380, 51, 41));
        label_12->setFont(font);
        MaximumVoltageChargingSet->addTab(tab, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QStringLiteral("tab_6"));
        MaximumVoltageChargingSet->addTab(tab_6, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QStringLiteral("tab_4"));
        MaximumVoltageChargingSet->addTab(tab_4, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        MinimumVoltageSet_3 = new QDoubleSpinBox(tab_3);
        MinimumVoltageSet_3->setObjectName(QStringLiteral("MinimumVoltageSet_3"));
        MinimumVoltageSet_3->setGeometry(QRect(350, 270, 151, 61));
        MinimumVoltageSet_3->setFont(font);
        label_13 = new QLabel(tab_3);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(1130, 400, 51, 41));
        label_13->setFont(font);
        label_14 = new QLabel(tab_3);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(30, 150, 321, 71));
        label_14->setFont(font1);
        label_15 = new QLabel(tab_3);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(1130, 290, 51, 41));
        label_15->setFont(font);
        label_16 = new QLabel(tab_3);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setGeometry(QRect(30, 260, 321, 71));
        label_16->setFont(font1);
        label_17 = new QLabel(tab_3);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setGeometry(QRect(1130, 180, 51, 41));
        label_17->setFont(font);
        label_18 = new QLabel(tab_3);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setGeometry(QRect(520, 280, 51, 41));
        label_18->setFont(font);
        label_19 = new QLabel(tab_3);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setGeometry(QRect(520, 390, 51, 41));
        label_19->setFont(font);
        MinimumVoltageSet_4 = new QDoubleSpinBox(tab_3);
        MinimumVoltageSet_4->setObjectName(QStringLiteral("MinimumVoltageSet_4"));
        MinimumVoltageSet_4->setGeometry(QRect(960, 270, 151, 61));
        MinimumVoltageSet_4->setFont(font);
        label_20 = new QLabel(tab_3);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setGeometry(QRect(600, 260, 361, 71));
        label_20->setFont(font2);
        label_21 = new QLabel(tab_3);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setGeometry(QRect(30, 370, 321, 71));
        label_21->setFont(font1);
        label_22 = new QLabel(tab_3);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setGeometry(QRect(520, 160, 61, 51));
        label_22->setFont(font);
        label_23 = new QLabel(tab_3);
        label_23->setObjectName(QStringLiteral("label_23"));
        label_23->setGeometry(QRect(600, 370, 361, 71));
        label_23->setFont(font1);
        doubleSpinBox_11 = new QDoubleSpinBox(tab_3);
        doubleSpinBox_11->setObjectName(QStringLiteral("doubleSpinBox_11"));
        doubleSpinBox_11->setGeometry(QRect(960, 380, 151, 61));
        doubleSpinBox_11->setFont(font);
        doubleSpinBox_9 = new QDoubleSpinBox(tab_3);
        doubleSpinBox_9->setObjectName(QStringLiteral("doubleSpinBox_9"));
        doubleSpinBox_9->setGeometry(QRect(350, 380, 151, 61));
        doubleSpinBox_9->setFont(font);
        label_24 = new QLabel(tab_3);
        label_24->setObjectName(QStringLiteral("label_24"));
        label_24->setGeometry(QRect(600, 150, 361, 71));
        label_24->setFont(font1);
        StartBalOver_2 = new QDoubleSpinBox(tab_3);
        StartBalOver_2->setObjectName(QStringLiteral("StartBalOver_2"));
        StartBalOver_2->setGeometry(QRect(960, 160, 151, 61));
        StartBalOver_2->setFont(font);
        MaximumVoltageSet_2 = new QDoubleSpinBox(tab_3);
        MaximumVoltageSet_2->setObjectName(QStringLiteral("MaximumVoltageSet_2"));
        MaximumVoltageSet_2->setGeometry(QRect(350, 160, 151, 61));
        MaximumVoltageSet_2->setFont(font);
        MaximumVoltageSet_2->setMaximum(15);
        MaximumVoltageChargingSet->addTab(tab_3, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QStringLiteral("tab_5"));
        MaximumVoltageChargingSet->addTab(tab_5, QString());

        retranslateUi(Configure);

        MaximumVoltageChargingSet->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Configure);
    } // setupUi

    void retranslateUi(QWidget *Configure)
    {
        Configure->setWindowTitle(QApplication::translate("Configure", "Form", Q_NULLPTR));
        MaximumVoltageChargingSet->setTabText(MaximumVoltageChargingSet->indexOf(tab_2), QApplication::translate("Configure", "Battery Profile", Q_NULLPTR));
        MaximumVoltageSet->setSpecialValueText(QString());
        label->setText(QApplication::translate("Configure", "Maximum Voltage", Q_NULLPTR));
        label_2->setText(QApplication::translate("Configure", "Minimium Voltage", Q_NULLPTR));
        label_3->setText(QApplication::translate("Configure", "Max Voltage in Charge", Q_NULLPTR));
        label_4->setText(QApplication::translate("Configure", "Stop Balancing when are within", Q_NULLPTR));
        label_5->setText(QApplication::translate("Configure", "Never Balancing Below", Q_NULLPTR));
        label_6->setText(QApplication::translate("Configure", "Start Balancing Over", Q_NULLPTR));
        label_7->setText(QApplication::translate("Configure", "[V]", Q_NULLPTR));
        label_8->setText(QApplication::translate("Configure", "[V]", Q_NULLPTR));
        label_9->setText(QApplication::translate("Configure", "[V]", Q_NULLPTR));
        label_10->setText(QApplication::translate("Configure", "[V]", Q_NULLPTR));
        label_11->setText(QApplication::translate("Configure", "[V]", Q_NULLPTR));
        label_12->setText(QApplication::translate("Configure", "[V]", Q_NULLPTR));
        MaximumVoltageChargingSet->setTabText(MaximumVoltageChargingSet->indexOf(tab), QApplication::translate("Configure", "Cells Settings", Q_NULLPTR));
        MaximumVoltageChargingSet->setTabText(MaximumVoltageChargingSet->indexOf(tab_6), QApplication::translate("Configure", "SOC Settings", Q_NULLPTR));
        MaximumVoltageChargingSet->setTabText(MaximumVoltageChargingSet->indexOf(tab_4), QApplication::translate("Configure", "Relays", Q_NULLPTR));
        label_13->setText(QApplication::translate("Configure", "[V]", Q_NULLPTR));
        label_14->setText(QApplication::translate("Configure", "Maximum Temperature", Q_NULLPTR));
        label_15->setText(QApplication::translate("Configure", "[V]", Q_NULLPTR));
        label_16->setText(QApplication::translate("Configure", "Minimium Voltage", Q_NULLPTR));
        label_17->setText(QApplication::translate("Configure", "[V]", Q_NULLPTR));
        label_18->setText(QApplication::translate("Configure", "[V]", Q_NULLPTR));
        label_19->setText(QApplication::translate("Configure", "[V]", Q_NULLPTR));
        label_20->setText(QApplication::translate("Configure", "Stop Balancing when are within", Q_NULLPTR));
        label_21->setText(QApplication::translate("Configure", "Max Voltage in Charge", Q_NULLPTR));
        label_22->setText(QApplication::translate("Configure", "[C\302\260]", Q_NULLPTR));
        label_23->setText(QApplication::translate("Configure", "Never Balancing Below", Q_NULLPTR));
        label_24->setText(QApplication::translate("Configure", "Start Balancing Over", Q_NULLPTR));
        MaximumVoltageSet_2->setSpecialValueText(QString());
        MaximumVoltageChargingSet->setTabText(MaximumVoltageChargingSet->indexOf(tab_3), QApplication::translate("Configure", "Thermal Settings", Q_NULLPTR));
        MaximumVoltageChargingSet->setTabText(MaximumVoltageChargingSet->indexOf(tab_5), QApplication::translate("Configure", "Charger", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Configure: public Ui_Configure {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGURE_H
