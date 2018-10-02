#include "livedata.h"
#include "ui_livedata.h"
#include "bms.h"
#include "string.h"
#include <QApplication>
#include <QTableView>
#include <QTextEdit>
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
void LiveData::addVoltage(int value, QTableWidget *table, int row, int column){
    QTableWidgetItem* cellVoltage;
    double val = value;
    QString vals = QString::fromStdString(to_string(val));
    table->setColumnWidth(row, 74);
    table->setRowHeight(column, 34);
    cellVoltage = new QTableWidgetItem(vals);
    table->setItem(row,column,cellVoltage);

}

void LiveData::addTemperature(int value, QTableWidget *table, int row, int column){

    QTableWidgetItem* cellTemp;
    double val = value;
    QString vals = QString::fromStdString(to_string(val));
    table->setColumnWidth(row, 74);
    table->setRowHeight(column, 34);
    cellTemp = new QTableWidgetItem(vals);
    table->setItem(row,column,cellTemp);
}/*
void LiveData::addNewPoint(int value, QCustomPlot *plot, QVector<double> &x, QVector<double> &y)
{
    //add the point, removing the oldest one first
    moveDataToLeft(x, size);
    moveDataToLeft(y, size);
    y[size - 1] = value;
    x[size - 1] = x[size - 2] + (1.0 / updateFrequency) * updateAfterNValues;
    //update the graph
    plot->xAxis->setRange(x[0], x[size - 1]);
    plot->graph(0)->setData(x, y);
    plot->replot();
}
*/

void LiveData::parseMessage()
{

    while(serial.canReadLine())
    {
        QString received = serial.readLine();

        //log the data
        received.remove("\n");
        received.remove("\r");
        writeToFile(received);

        QStringList valuesReceived =  received.split('\t');

        if(valuesReceived.length() != 13)
            return;

        if(valuesSinceLastUpdate + 1 == updateAfterNValues)
        {
            valuesSinceLastUpdate = 0;

            //qDebug() << received;

            //qDebug() << valuesReceived;

            //add the values to the arrays
           /* addNewPoint(valuesReceived[7].toDouble(), ui->totalCurrPlot, totalCurrX, totalCurrY);
            addNewPoint(valuesReceived[1].toDouble(), ui->worstTempPlot, worstTempX, worstTempY);
            addNewPoint(valuesReceived[8].toDouble() + valuesReceived[9].toDouble() + valuesReceived[10].toDouble(), ui->worstVoltPlot, worstVoltX, worstVoltY);
            addNewPoint(valuesReceived[11].toDouble(), ui->voltControlPlot, voltControlX, voltControlY);
            addNewPoint(valuesReceived[12].toDouble(), ui->endurCurrPlot, endurCurrX, endurCurrY);
            */

            // Load Voltages
            /* ============= TAB 1 VOLTAGE ============= */
            ui->TotalPackVoltage->display(valuesReceived[1].toDouble());
            ui->TotalPackVoltage->show();
            this->show();

            ui->MinimumVoltage->display(valuesReceived[1].toDouble());
            ui->MinimumVoltage->show();
            this->show();

            ui->MaximumVoltage->display(valuesReceived[1].toDouble());
            ui->MaximumVoltage->show();
            this->show();

            ui->AverageVoltage->display(valuesReceived[1].toDouble());
            ui->AverageVoltage->show();
            this->show();
        //NON E' NECESSARIA STA ROBA
            ui->OverVoltage->display(valuesReceived[1].toDouble());
            ui->OverVoltage->show();
            this->show();

            ui->UnderVoltage->display(valuesReceived[1].toDouble());
            ui->UnderVoltage->show();
            this->show();

            ui->SOC->setValue(90);
            ui->SOC->show();
            this->show();
          /*_______________*/
            for(int i = 0; i < NCX; i++){
                for(int j = 0; j < NCY; j++){
                   addVoltage(valuesReceived[i+j].toDouble(), ui->tableVoltage, i, j);
                }
            }

            ui->tableVoltage->show();
            this->show();

            /* ============= TAB 2 TEMPERATURE ============= */
            ui->TotalPackTemperature->display(valuesReceived[1].toDouble());
            ui->TotalPackTemperature->show();
            this->show();

            ui->MaxTemp->display(valuesReceived[1].toDouble());
            ui->MaxTemp->show();
            this->show();

            ui->MinTemp->display(valuesReceived[1].toDouble());
            ui->MinTemp->show();
            this->show();

            ui->AverageTemp->display(valuesReceived[1].toDouble());
            ui->AverageTemp->show();
            this->show();

            ui->OverTemp->display(valuesReceived[1].toDouble());
            ui->OverTemp->show();
            this->show();

            for(int i = 0; i < NCX; i++){
                for(int j = 0; j < NCY; j++){
                   addTemperature(valuesReceived[i+j].toDouble(), ui->tableTemp, i, j);
                }
            }
            ui->tableTemp->show();
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
/*________________________________________________________
            //update the bar plots
            cellTempY.clear();
            cellTempY << valuesReceived[1].toDouble() << valuesReceived[2].toDouble() << valuesReceived[3].toDouble() << valuesReceived[4].toDouble() << valuesReceived[5].toDouble() << valuesReceived[6].toDouble();
            updateBarsPlot(ui->cellTempPlot, cellTempX, cellTempY);

            cellVoltY.clear();
            cellVoltY << valuesReceived[8].toDouble() << valuesReceived[9].toDouble() << valuesReceived[10].toDouble();
            updateBarsPlot(ui->cellVoltPlot, cellVoltX, cellVoltY);

            branchCurrY.clear();
            branchCurrY << valuesReceived[0].toDouble() << valuesReceived[0].toDouble() << valuesReceived[0].toDouble() << valuesReceived[0].toDouble() << valuesReceived[0].toDouble();
            updateBarsPlot(ui->branchCurrPlot, branchCurrX, branchCurrY);
--------------------------------------------------------*/
            //setErrors
            if(valuesReceived[1].toDouble() > 80)
            {
             //   ui->lblTempErr->setStyleSheet("background-color:red;");
            }

            if(valuesReceived[8].toDouble() + valuesReceived[9].toDouble() + valuesReceived[10].toDouble() < 10)
            {
              //  ui->lblVoltErr->setStyleSheet("background-color:red;");
            }

            if(valuesReceived[7].toDouble() > 160)
            {
//                ui->lblCurrErr->setStyleSheet("background-color:red;");
            }

        }
        else
            valuesSinceLastUpdate++;

        //set skipped value error
        if(lastValueReceived != -1)
        {
            //qDebug() << valuesReceived[0].toDouble() << ";" << (lastValueReceived + 1) % 100;
            if(valuesReceived[0].toInt() % 100 != (lastValueReceived + 1) % 100)
            {
                valuesSkipped++;
                //if(valuesSkipped <= 5)
                //    ui->lblSkippedData->setStyleSheet("background-color:orange;");
                //else
//                    ui->lblSkippedData->setStyleSheet("background-color:red;");
               // ui->lblSkippedData->setText("Skip: " + QString::number(valuesSkipped));
            }
        }
        lastValueReceived = valuesReceived[0].toInt();
    }
}

void LiveData::moveDataToLeft(QVector<double> &arr, int size)
{
    for(int i = 0; i < size - 1; i++){
        arr[i] = arr[i+1];
    }
    arr[size - 1] = 0; //useless value
}


void LiveData::setNewFileName()
{
    filename =  "log_" + QDate::currentDate().toString().replace(" ", "_") + "_" + QTime::currentTime().toString().replace(" ", "_").replace(":", "_") + ".txt";
    qDebug() << filename;

    //see if it works
    QFile file(filename);
    if ( file.open( QIODevice::WriteOnly | QIODevice::Append) )
    {/*
        ui->lblLog->setStyleSheet("background-color:green;");
        ui->lblFile->setText("Saving to: " + filename);
        ui->btnLogRetry->setEnabled(false);
        */
        file.close();
    }
    else
    {/*
        ui->lblLog->setStyleSheet("background-color:red;");
        ui->btnLogRetry->setEnabled(true);
    */
    }
}


void LiveData::writeToFile(QString stringToWrite)
{
    QFile file(filename);
    if ( file.open( QIODevice::WriteOnly | QIODevice::Append) )
    {
        QTextStream stream( &file );
        stream << stringToWrite << endl;
        file.close();
    }
}

void LiveData::openSerial()
{
    qDebug() << "Trying to open serial";

    SERIAL_PORT = ui->txtSerialName->toPlainText();
    QFileInfo file_info(SERIAL_PORT);

    if (file_info.isSymLink()) {
        // This file is a SYM Link
        // thus the serial needs to be be targeted correctly
        serial.setPortName(file_info.symLinkTarget());
    } else {
        serial.setPortName(file_info.absoluteFilePath());
    }

    qDebug() << serial.portName();

    serial.setBaudRate(115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    // Open the serial
    bool serialIsOpen = serial.open(QIODevice::ReadWrite);

    if (serialIsOpen) {
        /*
        qDebug() << "Serial Communication: OK";
        ui->lblSerial->setStyleSheet("background-color:green;");
        ui->btnSerialRetry->setEnabled(false);
        ui->btnStart->setEnabled(true);
        */
    } else {
        qDebug() << "Error: " << serial.error();
        /*
        qDebug() << "Serial Communication: NO";
        ui->lblSerial->setStyleSheet("background-color:red;");
        ui->btnSerialRetry->setEnabled(true);
        ui->btnStart->setEnabled(false);*/
    }

}
LiveData::~LiveData()
{
    delete ui;
}

void LiveData::on_LoadTable_clicked()
{
    //ui->LoadTable;
}


