//
// Created by mohamedkamal on ٥‏/٥‏/٢٠٢١.
//
#include "../MainWindow.h"
#include "../ui_MainWindow.h"
#include <QFileDialog>
#include "processinglib/harris_corner_detector.h"
#include "processinglib/imageMatching.h"
#include "vector"
#include "iostream"
#include "processinglib/filters.h"
#include <chrono>
#include <thread>

    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds


void MainWindow::on_loadImageMatchBtn_1_clicked() {

    QString filePath = QFileDialog::getOpenFileName(this, "load image", "../");
    std::string filepathStd = filePath.toStdString();
    auto filename = filepathStd.substr(filepathStd.find_last_of("/") + 1);
    QPixmap pixmap{filePath};

    imageMatch_1 = new Image(filepathStd,3);

    QImage qImage(imageMatch_1->width, imageMatch_1->height, QImage::Format_RGB16);
    QRgb rgb;
    for (int j = 0; j < imageMatch_1->width; ++j) {
        for (int i = 0; i < imageMatch_1->height; ++i) {
            rgb = qRgb((*imageMatch_1)(i, j,0), (*imageMatch_1)(i, j,1), (*imageMatch_1)(i, j,2));
            qImage.setPixel(j, i, rgb);

        }
    }
    int width = ui->imageMatch_1->width() ;
    int height = ui->imageMatch_1->height();
    ui->imageMatch_1->setBackground(QPixmap::fromImage(qImage).scaled(width, height,Qt::IgnoreAspectRatio, Qt::FastTransformation));
    ui->imageMatch_1->setAutoFillBackground(true);
    ui->imageMatch_1->replot();


}

void MainWindow::on_loadImageMatchBtn_2_clicked() {

    QString filePath = QFileDialog::getOpenFileName(this, "load image", "../");
    std::string filepathStd = filePath.toStdString();
    auto filename = filepathStd.substr(filepathStd.find_last_of("/") + 1);
    QPixmap pixmap{filePath};

    imageMatch_2 = new Image(filepathStd,3);
//    translate(imageMatch_2);
    int resultHeight = imageMatch_2->height;
    if (imageMatch_1->height > imageMatch_2->height){
        resultHeight = imageMatch_1->height;
    }
    QImage resultImg(imageMatch_1->width + imageMatch_2->width, resultHeight , QImage::Format_RGB16);

    QImage qImage(imageMatch_2->width, imageMatch_2->height, QImage::Format_RGB16);

    QRgb rgb;
    for (int j = 0; j < imageMatch_1->width; ++j) {
        for (int i = 0; i < imageMatch_1->height; ++i) {
            rgb = qRgb((*imageMatch_1)(i, j,0), (*imageMatch_1)(i, j,1), (*imageMatch_1)(i, j,2));
            resultImg.setPixel(j, i, rgb);

        }
    }
    for (int j = 0; j < imageMatch_2->width; ++j) {
        for (int i = 0; i < imageMatch_2->height; ++i) {
            rgb = qRgb((*imageMatch_2)(i, j,0), (*imageMatch_2)(i, j,1), (*imageMatch_2)(i, j,2));
            qImage.setPixel(j, i, rgb);
            rgb = qRgb((*imageMatch_2)(i, j,0), (*imageMatch_2)(i, j,1), (*imageMatch_2)(i, j,2));
            resultImg.setPixel(j+ imageMatch_1->width, i , rgb);
        }
    }
    int width = ui->imageMatch_2->width() ;
    int height = ui->imageMatch_2->height();
    ui->imageMatch_2->setBackground(QPixmap::fromImage(qImage).scaled(width, height,Qt::IgnoreAspectRatio, Qt::FastTransformation));
    ui->imageMatch_2->setAutoFillBackground(true);
    ui->imageMatch_2->replot();

    width = ui->resultMatch-> width();
    height = ui->resultMatch->height();
    ui->resultMatch->setBackground(QPixmap::fromImage(resultImg).scaled(width, height,Qt::IgnoreAspectRatio, Qt::FastTransformation));
    ui->resultMatch->setAutoFillBackground(true);
    ui->resultMatch->replot();



}

void MainWindow::on_startMatchingBtn_clicked() {

    Image gray_1 = imageMatch_1->toGrayscale();
    Image gray_2 = imageMatch_2->toGrayscale();


    int resultHeight = imageMatch_2->height;
    if (imageMatch_1->height > imageMatch_2->height){
        resultHeight = imageMatch_1->height;
    }
    std::vector<double> result;
    if (ui->matchSelect->currentIndex() == 1){
        result = SSDMatching(*imageMatch_1, *imageMatch_2);
        std::cout<< "ssd " <<  std::endl;
    }else {
        result = normalizedCorrelation(*imageMatch_1, *imageMatch_2);
        std::cout<< "corrl " <<  std::endl;
    }


    xDataMatching_1->clear();
    xDataMatching_1->clear();
    int grapNo = 0;
    QVector<double> *x = new QVector<double>();
    QVector<double> *y = new QVector<double>();
    qDebug()<< "the last "<<resultHeight<<" "<< ui->resultMatch->background().rect().height() << " "<< result.size();

    imageMatchPoints1->setLineStyle((QCPGraph::LineStyle) QCPGraph::lsNone);
    imageMatchPoints1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
    imageMatchPoints1->setPen(QPen(QColor(0, 255, 0), 2));
    imageMatchPoints2->setLineStyle((QCPGraph::LineStyle) QCPGraph::lsNone);
    imageMatchPoints2->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
    imageMatchPoints2->setPen(QPen(QColor(0, 255, 0), 2));
    for(int loop = 0 ; loop < (result.size()/4); loop++) {
        if (result[loop*4 ] != -1 ) {
            double xCoord_1 = result[loop * 4] * (imageMatch_1->width / (double )(imageMatch_1->width + imageMatch_2->width));;
            double yCoord_1 = result[loop * 4 + 1];
            double xCoord_2 = result[loop * 4 + 2] * (imageMatch_2->width / (double )(imageMatch_1->width + imageMatch_2->width));
            double yCoord_2 = result[loop * 4 + 3] ;

            xCoord_1 = xCoord_1 * ui->resultMatch->background().rect().width();
            yCoord_1 = yCoord_1 * (imageMatch_1->height / (double ) resultHeight)* ui->resultMatch->background().rect().height();
            xCoord_2 = (xCoord_2 + (imageMatch_1->width/(double )(imageMatch_1->width + imageMatch_2->width))) * ui->resultMatch->background().rect().width();;
            yCoord_2 = yCoord_2 * (imageMatch_2->height / (double )resultHeight)* ui->resultMatch->background().rect().height();


//
            xDataMatching_1->append(ui->imageMatch_1->xAxis->pixelToCoord(result[loop * 4 ] * ui->imageMatch_1->background().rect().width()));
            yDataMatching_1->append(ui->imageMatch_1->yAxis->pixelToCoord(result[loop * 4 + 1] * ui->imageMatch_1->background().rect().height()));
            xDataMatching_2->append(ui->imageMatch_2->xAxis->pixelToCoord(result[loop * 4 + 2] * ui->imageMatch_2->background().rect().width()));
            yDataMatching_2->append(ui->imageMatch_2->yAxis->pixelToCoord(result[loop * 4 + 3] * ui->imageMatch_2->background().rect().height()));




            QCPItemLine *arrow = new QCPItemLine(ui->resultMatch);
            lines->append(arrow);
            arrow->start->setCoords(ui->resultMatch->xAxis->pixelToCoord(xCoord_2 ), ui->resultMatch->yAxis->pixelToCoord(yCoord_2));
            arrow->end->setCoords(ui->resultMatch->xAxis->pixelToCoord(xCoord_1), ui->resultMatch->yAxis->pixelToCoord(yCoord_1));
            arrow->setPen(QPen(Qt::yellow));
            ui->resultMatch->replot();
            sleep_for(nanoseconds(7));
            sleep_until(system_clock::now() + seconds(1));
            grapNo++;

        }
    }

    imageMatchPoints1->setData(*xDataMatching_1, *yDataMatching_1);
    imageMatchPoints2->setData(*xDataMatching_2, *yDataMatching_2);
    qDebug()<< "number "<<xDataMatching_1->count();
    ui->imageMatch_1->replot();
    ui->imageMatch_2->replot();


}

void MainWindow::on_matchClearBtn_clicked() {

    xDataMatching_1->clear();
    yDataMatching_1->clear();
    xDataMatching_2->clear();
    yDataMatching_2->clear();

    ui->resultMatch->clearGraphs();
    imageMatchPoints1->data()->clear();
    imageMatchPoints2->data()->clear();
    ui->resultMatch->replot();
    ui->imageMatch_1->replot();
    ui->imageMatch_2->replot();
}