#ifndef LIVEDATA_H
#define LIVEDATA_H

#include <QDialog>

namespace Ui {
class LiveData;
}

class LiveData : public QDialog
{
    Q_OBJECT

public:
    int updateFrequency = 50; //Hz
    int updateAfterNValues = 5; //1 = update each time, 2 = update once every 2 values received etc.
    int valuesSinceLastUpdate = 0;

    int size = 600;
    QVector<double> worstTempX, worstTempY;
    QVector<double> worstVoltX, worstVoltY;
    QVector<double> totalCurrX, totalCurrY;
    QVector<double> voltControlX, voltControlY;
    QVector<double> endurCurrX, endurCurrY;

    QVector<double> cellTempX, cellTempY;
    QVector<double> cellVoltX, cellVoltY;
    QVector<double> branchCurrX, branchCurrY;

    QString filename = "log.txt";

    int lastValueReceived = -1;
    int valuesSkipped = 0;

    explicit LiveData(QWidget *parent = 0);
    ~LiveData();

private slots:
    void on_LoadTable_clicked();

private:
    Ui::LiveData *ui;

};

#endif // LIVEDATA_H
