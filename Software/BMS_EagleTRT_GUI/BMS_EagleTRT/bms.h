#ifndef BMS_H
#define BMS_H

#include <QMainWindow>
#include "qcustomplot.h"

namespace Ui {
class BMS;
}

class BMS : public QMainWindow
{
    Q_OBJECT

public:
    explicit BMS(QWidget *parent = 0);
    void createLinePlot(QCustomPlot *plot, QVector<double> x, QVector<double> y, QStringList text, double minVal, double maxVal);

    ~BMS();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::BMS *ui;
};

#endif // BMS_H
