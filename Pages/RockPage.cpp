//
// Created by mohamedkamal on ٥‏/٦‏/٢٠٢١.
//
#include "../MainWindow.h"
#include "processinglib/roc.h"
void MainWindow::on_faceRecog_clicked() {
    roc->data().clear();
    int classIndex = ui->class_Name->value();
    if (classIndex == 0)
        return;
    std::ifstream inFile("../results.txt");
    float TP;
    float FP;
    QVector<double> xD ;
    QVector<double> yD ;
    while(!inFile.eof()) {
        for(int loop = 0; loop < classIndex; loop++){
            inFile  >>  TP >> FP;
        }
        xD.push_back(TP);
        yD.push_back(FP);

        if (!inFile) break;
    }
    
    roc->setData(xD, yD);
//    roc->setLineStyle((QCPGraph::LineStyle) QCPGraph::lsNone);
//    roc->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 3));
    roc->setPen(QPen(QColor(0, 0, 255), 2));
    ui->widget_5->replot();

}