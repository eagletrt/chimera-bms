#include "livedata.h"
#include "ui_livedata.h"
#include "bms.h"
#include "string.h"
#include <QApplication>
#include <QTableView>
#define NCX 9
#define NCY 12
#define NTX 5
#define NTY 6
using namespace std;
LiveData::LiveData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LiveData)
{
    ui->setupUi(this);
    /* ============ COLORS OF TABS ============= */
/*
    QPalette pal = ui->tabWidget->palette();
    pal.setColor(ui->tabWidget->backgroundRole(), Qt::blue);
    ui->tabWidget->setAutoFillBackground(true);
    ui->tabWidget->setPalette(pal);
    ui->tabWidget->show();
    this->show();

    QPalette pal1 = ui->Voltage->palette();
    pal1.setColor(ui->Voltage->backgroundRole(), Qt::yellow);
    ui->Voltage->setAutoFillBackground(true);
    ui->Voltage->setPalette(pal1);
    ui->Voltage->show();
    this->show();
*/
    /* ============= TAB 1 VOLTAGE ============= */
    ui->TotalPackVoltage->display(85.2);
    ui->TotalPackVoltage->show();
    this->show();

    ui->MinimumVoltage->display(85.2);
    ui->MinimumVoltage->show();
    this->show();

    ui->MaximumVoltage->display(85.2);
    ui->MaximumVoltage->show();
    this->show();

    ui->AverageVoltage->display(78.3);
    ui->AverageVoltage->show();
    this->show();

    ui->OverVoltage->display(4);
    ui->OverVoltage->show();
    this->show();

    ui->UnderVoltage->display(4);
    ui->UnderVoltage->show();
    this->show();

    ui->SOC->setValue(90);
    ui->SOC->show();
    this->show();

    //Table of cells' Voltages
    ui->tableVoltage->setGeometry(370, 50, 696, 440);
    ui->tableVoltage->setColumnCount(NCX);
    ui->tableVoltage->setRowCount(NCY);

    QTableWidgetItem* cellVoltage[NCX][NCY];
    for(int i = 0; i < NCX; i++){
        for(int j = 0; j < NCY; j++){
            float val = 4.26;
            QString vals = QString::fromStdString(to_string(val));
            ui->tableVoltage->setColumnWidth(i, 74);
            ui->tableVoltage->setRowHeight(j, 34);
            cellVoltage[i][j] = new QTableWidgetItem(vals);
            ui->tableVoltage->setItem(j,i,cellVoltage[i][j]);
        }
    }
    ui->tableVoltage->show();
    this->show();

    /* ============= TAB 2 TEMPERATURE ============= */
    ui->TotalPackTemperature->display(85.2);
    ui->TotalPackTemperature->show();
    this->show();

    ui->MaxTemp->display(85.2);
    ui->MaxTemp->show();
    this->show();

    ui->MinTemp->display(85.2);
    ui->MinTemp->show();
    this->show();

    ui->AverageTemp->display(78.3);
    ui->AverageTemp->show();
    this->show();

    ui->OverTemp->display(4);
    ui->OverTemp->show();
    this->show();

    //Table of cells' Temperatures
    ui->tableTemp->setGeometry(370, 50, 696, 440);
    ui->tableTemp->setColumnCount(NCX);
    ui->tableTemp->setRowCount(NCY);

    QTableWidgetItem* cellTemp[NCX][NCY];
    for(int i = 0; i < NCX; i++){
        for(int j = 0; j < NCY; j++){
            float val = 4.26;
            QString vals = QString::fromStdString(to_string(val));
            ui->tableTemp->setColumnWidth(i, 74);
            ui->tableTemp->setRowHeight(j, 34);
            cellTemp[i][j] = new QTableWidgetItem(vals);
            ui->tableTemp->setItem(j,i,cellTemp[i][j]);
        }
    }
    ui->tableVoltage->show();
    this->show();
    /* ============= TAB 3 CURRENT ============= */
    QStringList strings = {"Worst Cell's Temperature", "Time (s)", "Temperature (°C)"};
    //BMS::createLinePlot(ui->plotCurrent, totalCurrX, totalCurrY, strings, 20, 80);
    /* ============= TAB 4 CANBUS ============= */

    // Labels of the columns
    QString labelID = "ID";
    QString labelLength = "Length";
    QString labelByte0 = "Byte0";
    QString labelByte1 = "Byte1";
    QString labelByte2 = "Byte2";
    QString labelByte3 = "Byte3";
    QString labelByte4 = "Byte4";
    QString labelByte5 = "Byte5";
    QString labelByte6 = "Byte6";
    QString labelByte7 = "Byte7";
    QString labelCount = "Count";
    QString labelTimestamp = "Timestamp";
    /* QStringList* canbusLabels = new QStringList();
    canbusLabels->insert(0, labelID);
    canbusLabels->insert(1, labelLength);
    canbusLabels->insert(2, labelByte0);
    canbusLabels->insert(3, labelByte1);
    canbusLabels->insert(4, labelByte2);
    canbusLabels->insert(5, labelByte3);
    canbusLabels->insert(6, labelByte4);
    canbusLabels->insert(7, labelByte5);
    canbusLabels->insert(8, labelByte6);
    canbusLabels->insert(9, labelByte7);
    canbusLabels->insert(10, labelCount);
    canbusLabels->insert(11, labelTimestamp);
    */
    QTableWidgetItem* item0 = new QTableWidgetItem(labelID);
    QTableWidgetItem* item1 = new QTableWidgetItem(labelLength);
    QTableWidgetItem* item2 = new QTableWidgetItem(labelByte0);
    QTableWidgetItem* item3 = new QTableWidgetItem(labelByte1);
    QTableWidgetItem* item4 = new QTableWidgetItem(labelByte2);
    QTableWidgetItem* item5 = new QTableWidgetItem(labelByte3);
    QTableWidgetItem* item6 = new QTableWidgetItem(labelByte4);
    QTableWidgetItem* item7 = new QTableWidgetItem(labelByte5);
    QTableWidgetItem* item8 = new QTableWidgetItem(labelByte6);
    QTableWidgetItem* item9 = new QTableWidgetItem(labelByte7);
    QTableWidgetItem* item10 = new QTableWidgetItem(labelCount);
    QTableWidgetItem* item11 = new QTableWidgetItem(labelTimestamp);
    ui->LiveCanbus->setAlternatingRowColors(true);
    ui->LiveCanbus->setColumnCount(12);

    for(int i = 0; i < 12; i++){
        ui->LiveCanbus->setColumnWidth(i, 89);
    }

    ui->LiveCanbus->setHorizontalHeaderItem(0, item0);
    ui->LiveCanbus->setHorizontalHeaderItem(1, item1);
    ui->LiveCanbus->setHorizontalHeaderItem(2, item2);
    ui->LiveCanbus->setHorizontalHeaderItem(3, item3);
    ui->LiveCanbus->setHorizontalHeaderItem(4, item4);
    ui->LiveCanbus->setHorizontalHeaderItem(5, item5);
    ui->LiveCanbus->setHorizontalHeaderItem(6, item6);
    ui->LiveCanbus->setHorizontalHeaderItem(7, item7);
    ui->LiveCanbus->setHorizontalHeaderItem(8, item8);
    ui->LiveCanbus->setHorizontalHeaderItem(9, item9);
    ui->LiveCanbus->setHorizontalHeaderItem(10, item10);
    ui->LiveCanbus->setHorizontalHeaderItem(11, item11);

    ui->LiveCanbus->show();
    this->show();




}

LiveData::~LiveData()
{
    delete ui;
}

void LiveData::on_LoadTable_clicked()
{
    //ui->LoadTable;
}


