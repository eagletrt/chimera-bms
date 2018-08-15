/********************************************************************************
** Form generated from reading UI file 'livedata.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LIVEDATA_H
#define UI_LIVEDATA_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_LiveData
{
public:
    QLabel *label;
    QTabWidget *tabWidget;
    QWidget *Voltage;
    QLCDNumber *TotalPackVoltage;
    QProgressBar *SOC;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLCDNumber *MinimumVoltage;
    QLCDNumber *MaximumVoltage;
    QLCDNumber *AverageVoltage;
    QLCDNumber *UnderVoltage;
    QLCDNumber *OverVoltage;
    QLabel *label_2;
    QTableWidget *tableVoltage;
    QWidget *Temperature;
    QLCDNumber *MinTemp;
    QLabel *label_10;
    QLabel *label_11;
    QLCDNumber *TotalPackTemperature;
    QLCDNumber *MaxTemp;
    QLabel *label_13;
    QLCDNumber *AverageTemp;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_17;
    QLCDNumber *OverTemp;
    QTableWidget *tableTemp;
    QWidget *Current;
    QCustomPlot *plotCurrent;
    QLabel *label_12;
    QWidget *Canbus;
    QTableWidget *LiveCanbus;
    QWidget *LiveGraph;
    QTextEdit *txtSerialName;
    QLabel *label_14;

    void setupUi(QDialog *LiveData)
    {
        if (LiveData->objectName().isEmpty())
            LiveData->setObjectName(QStringLiteral("LiveData"));
        LiveData->resize(1164, 647);
        label = new QLabel(LiveData);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 20, 211, 41));
        QFont font;
        font.setFamily(QStringLiteral("Myanmar Text"));
        font.setPointSize(20);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        tabWidget = new QTabWidget(LiveData);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(10, 50, 1131, 541));
        Voltage = new QWidget();
        Voltage->setObjectName(QStringLiteral("Voltage"));
        TotalPackVoltage = new QLCDNumber(Voltage);
        TotalPackVoltage->setObjectName(QStringLiteral("TotalPackVoltage"));
        TotalPackVoltage->setGeometry(QRect(190, 50, 131, 41));
        SOC = new QProgressBar(Voltage);
        SOC->setObjectName(QStringLiteral("SOC"));
        SOC->setGeometry(QRect(190, 110, 171, 31));
        SOC->setValue(24);
        label_3 = new QLabel(Voltage);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(40, 50, 151, 41));
        label_4 = new QLabel(Voltage);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(40, 100, 151, 41));
        label_5 = new QLabel(Voltage);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(40, 160, 151, 41));
        label_6 = new QLabel(Voltage);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(40, 220, 151, 41));
        label_7 = new QLabel(Voltage);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(40, 280, 151, 41));
        label_8 = new QLabel(Voltage);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(40, 340, 151, 41));
        label_9 = new QLabel(Voltage);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(40, 400, 151, 41));
        MinimumVoltage = new QLCDNumber(Voltage);
        MinimumVoltage->setObjectName(QStringLiteral("MinimumVoltage"));
        MinimumVoltage->setGeometry(QRect(190, 160, 131, 41));
        MaximumVoltage = new QLCDNumber(Voltage);
        MaximumVoltage->setObjectName(QStringLiteral("MaximumVoltage"));
        MaximumVoltage->setGeometry(QRect(190, 220, 131, 41));
        AverageVoltage = new QLCDNumber(Voltage);
        AverageVoltage->setObjectName(QStringLiteral("AverageVoltage"));
        AverageVoltage->setGeometry(QRect(190, 280, 131, 41));
        AverageVoltage->setSmallDecimalPoint(false);
        AverageVoltage->setProperty("value", QVariant(0));
        AverageVoltage->setProperty("intValue", QVariant(0));
        UnderVoltage = new QLCDNumber(Voltage);
        UnderVoltage->setObjectName(QStringLiteral("UnderVoltage"));
        UnderVoltage->setGeometry(QRect(190, 340, 131, 41));
        OverVoltage = new QLCDNumber(Voltage);
        OverVoltage->setObjectName(QStringLiteral("OverVoltage"));
        OverVoltage->setGeometry(QRect(190, 400, 131, 41));
        label_2 = new QLabel(Voltage);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(370, 10, 131, 31));
        tableVoltage = new QTableWidget(Voltage);
        tableVoltage->setObjectName(QStringLiteral("tableVoltage"));
        tableVoltage->setGeometry(QRect(370, 50, 741, 461));
        tabWidget->addTab(Voltage, QString());
        Temperature = new QWidget();
        Temperature->setObjectName(QStringLiteral("Temperature"));
        MinTemp = new QLCDNumber(Temperature);
        MinTemp->setObjectName(QStringLiteral("MinTemp"));
        MinTemp->setGeometry(QRect(180, 110, 131, 41));
        label_10 = new QLabel(Temperature);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(370, 10, 171, 31));
        label_11 = new QLabel(Temperature);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(30, 50, 151, 41));
        TotalPackTemperature = new QLCDNumber(Temperature);
        TotalPackTemperature->setObjectName(QStringLiteral("TotalPackTemperature"));
        TotalPackTemperature->setGeometry(QRect(180, 50, 131, 41));
        MaxTemp = new QLCDNumber(Temperature);
        MaxTemp->setObjectName(QStringLiteral("MaxTemp"));
        MaxTemp->setGeometry(QRect(180, 170, 131, 41));
        label_13 = new QLabel(Temperature);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(30, 110, 151, 41));
        AverageTemp = new QLCDNumber(Temperature);
        AverageTemp->setObjectName(QStringLiteral("AverageTemp"));
        AverageTemp->setGeometry(QRect(180, 230, 131, 41));
        AverageTemp->setSmallDecimalPoint(false);
        AverageTemp->setDigitCount(5);
        AverageTemp->setMode(QLCDNumber::Dec);
        AverageTemp->setSegmentStyle(QLCDNumber::Filled);
        AverageTemp->setProperty("value", QVariant(0));
        label_15 = new QLabel(Temperature);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(30, 290, 151, 41));
        label_16 = new QLabel(Temperature);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setGeometry(QRect(30, 170, 151, 41));
        label_17 = new QLabel(Temperature);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setGeometry(QRect(30, 230, 151, 41));
        OverTemp = new QLCDNumber(Temperature);
        OverTemp->setObjectName(QStringLiteral("OverTemp"));
        OverTemp->setGeometry(QRect(180, 290, 131, 41));
        tableTemp = new QTableWidget(Temperature);
        tableTemp->setObjectName(QStringLiteral("tableTemp"));
        tableTemp->setGeometry(QRect(340, 40, 741, 461));
        tabWidget->addTab(Temperature, QString());
        Current = new QWidget();
        Current->setObjectName(QStringLiteral("Current"));
        plotCurrent = new QCustomPlot(Current);
        plotCurrent->setObjectName(QStringLiteral("plotCurrent"));
        plotCurrent->setGeometry(QRect(320, 20, 781, 481));
        label_12 = new QLabel(Current);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(40, 70, 221, 71));
        QFont font1;
        font1.setPointSize(14);
        label_12->setFont(font1);
        tabWidget->addTab(Current, QString());
        Canbus = new QWidget();
        Canbus->setObjectName(QStringLiteral("Canbus"));
        LiveCanbus = new QTableWidget(Canbus);
        LiveCanbus->setObjectName(QStringLiteral("LiveCanbus"));
        LiveCanbus->setGeometry(QRect(25, 21, 1070, 471));
        tabWidget->addTab(Canbus, QString());
        LiveGraph = new QWidget();
        LiveGraph->setObjectName(QStringLiteral("LiveGraph"));
        tabWidget->addTab(LiveGraph, QString());
        txtSerialName = new QTextEdit(LiveData);
        txtSerialName->setObjectName(QStringLiteral("txtSerialName"));
        txtSerialName->setGeometry(QRect(580, 10, 171, 31));
        label_14 = new QLabel(LiveData);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(470, 10, 111, 31));

        retranslateUi(LiveData);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(LiveData);
    } // setupUi

    void retranslateUi(QDialog *LiveData)
    {
        LiveData->setWindowTitle(QApplication::translate("LiveData", "Dialog", Q_NULLPTR));
        label->setText(QApplication::translate("LiveData", "Live Data", Q_NULLPTR));
        label_3->setText(QApplication::translate("LiveData", "Total Pack Voltage", Q_NULLPTR));
        label_4->setText(QApplication::translate("LiveData", "State of Charge", Q_NULLPTR));
        label_5->setText(QApplication::translate("LiveData", "Minimum Voltage", Q_NULLPTR));
        label_6->setText(QApplication::translate("LiveData", "Maximum Voltage", Q_NULLPTR));
        label_7->setText(QApplication::translate("LiveData", "Average", Q_NULLPTR));
        label_8->setText(QApplication::translate("LiveData", "N\302\260 Under Cell's Voltage", Q_NULLPTR));
        label_9->setText(QApplication::translate("LiveData", "N\302\260 Over Cell's Voltage", Q_NULLPTR));
        label_2->setText(QApplication::translate("LiveData", "Actual Cells' Voltages", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(Voltage), QApplication::translate("LiveData", "Voltage", Q_NULLPTR));
        label_10->setText(QApplication::translate("LiveData", "Actual Cells' Temperatures", Q_NULLPTR));
        label_11->setText(QApplication::translate("LiveData", "Total Pack Temperature", Q_NULLPTR));
        label_13->setText(QApplication::translate("LiveData", "Minimum Temperature", Q_NULLPTR));
        label_15->setText(QApplication::translate("LiveData", "N\302\260 Over Temperature", Q_NULLPTR));
        label_16->setText(QApplication::translate("LiveData", "Maximum Temperature", Q_NULLPTR));
        label_17->setText(QApplication::translate("LiveData", "Average Temperature", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(Temperature), QApplication::translate("LiveData", "Temperature", Q_NULLPTR));
        label_12->setText(QApplication::translate("LiveData", "Current's plot in time", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(Current), QApplication::translate("LiveData", "Current", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(Canbus), QApplication::translate("LiveData", "Canbus", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(LiveGraph), QApplication::translate("LiveData", "Live Graph", Q_NULLPTR));
        label_14->setText(QApplication::translate("LiveData", "Serial Port", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class LiveData: public Ui_LiveData {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LIVEDATA_H
