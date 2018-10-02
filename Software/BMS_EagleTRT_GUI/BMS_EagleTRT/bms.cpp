#include "bms.h"
#include "ui_bms.h"
#include "configure.h"
#include "livedata.h"
#include <QFileDialog>
BMS::BMS(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BMS)
{
    ui->setupUi(this);

   // ui->EagleImage->
/*
     QString fileName = QFileDialog::getOpenFileName(this, "alba", QDir::currentPath());
     QImage image(fileName);
     QGraphicsScene* scene = new QGraphicsScene();
     QGraphicsView* view = new QGraphicsView();
     QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
     scene->addItem(item);
     ui->EagleImage->show();*/
    QString filename = "../Chimera.jpg";
    QImage image(filename);
    ui->EagleImage->setPixmap(QPixmap::fromImage(image));

}
void BMS::createLinePlot(QCustomPlot *plot, QVector<double> x, QVector<double> y, QStringList text, double minVal, double maxVal)
{
    plot->addGraph();
    plot->graph(0)->setData(x, y);
    plot->xAxis->setRange(0, 60);
    plot->yAxis->setRange(minVal, maxVal);
    //title
    plot->plotLayout()->insertRow(0);
    plot->plotLayout()->addElement(0, 0, new QCPTextElement(plot, text[0], QFont("sans", 12, QFont::Bold)));
    //labels
    plot->xAxis->setLabel(text[1]);
    plot->yAxis->setLabel(text[2]);

    plot->replot();
}
/*


*/

BMS::~BMS()
{
    delete ui;
}

void BMS::on_pushButton_clicked()
{
    ui->label->setText("BMS CONNECTED");
}

void BMS::on_pushButton_2_clicked()
{
    Configure secdialog;
    secdialog.setModal(true);
    secdialog.exec();
}

void BMS::on_pushButton_3_clicked()
{
    LiveData tirdialog;
    tirdialog.setModal(true);
    tirdialog.exec();
}
