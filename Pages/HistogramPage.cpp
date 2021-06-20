//
// Created by abdallah drwesh on 4/4/21.
//
#include "../MainWindow.h"
#include "../ui_MainWindow.h"
#include <QFileDialog>
#include "processinglib/histogram.h"

using namespace cv;


void MainWindow::on_LoadImageBtn_1_clicked() {

    QString filePath = QFileDialog::getOpenFileName(this, "load image", "../");
    std::string filepathStd = filePath.toStdString();
    auto filename = filepathStd.substr(filepathStd.find_last_of("/") + 1);
    ui->imageName->setText(QString(filename.c_str()));
    QPixmap pixmap{filePath};
    ui->InputImagLabel_1->setPixmap(pixmap);
    loadImage(filepathStd, inputImage_1);
    std::string imageSize = std::to_string(inputImage_1->size());
    ui->imageSize->setText(QString(imageSize.c_str()));
    if (inputImage_1->channels == 1) {
        ui->imageSelect->setCurrentIndex(1);
        ui->imageSelect->setCurrentIndex(0);
    } else {
        ui->imageSelect->setCurrentIndex(0);
        ui->imageSelect->setCurrentIndex(1);
    }
}

void MainWindow::on_imageSelect_currentIndexChanged(QString filterName) {
    ui->histPLot->clearGraphs();
    ui->CDFPlot->clearGraphs();
    ui->eqHistPlot->clearGraphs();
    ui->eqCDFPLot->clearGraphs();
    if (filterName == "Equalize" || inputImage_1->channels == 1) {
        ui->outpuImageLabel_1->clear();
        ui->eqHistPlot->setVisible(true);
        ui->eqCDFPLot->setVisible(true);
        ui->outpuImageLabel_1->setVisible(true);
        ui->label_9->setVisible(true);
        ui->label_14->setVisible(true);
        ui->label_15->setVisible(true);

        int histogram[256];
        int cumHist[256];
        Image grayIm = inputImage_1->toGrayscale();
        im_hist(grayIm, histogram, 1);
        cumulative_hist(histogram, cumHist);
        histDisplay(histogram, 0, ui->histPLot);
        CDFDisplay(cumHist, 0, ui->CDFPlot);

        displayGrayscaleImage(&grayIm, ui->InputImagLabel_1);

        int hist_eq_form[256];
        int eq_hist[256];
        hist_eq_formula(grayIm, cumHist, hist_eq_form);
        equalized_image(grayIm, hist_eq_form);
        im_hist(grayIm, eq_hist, 1);
        histDisplay(eq_hist, 0, ui->eqHistPlot);
        int cumEqHist[256];
        cumulative_hist(eq_hist, cumEqHist);
        CDFDisplay(cumEqHist, 0, ui->eqCDFPLot);
        QImage qImage_1(grayIm.width, grayIm.height, QImage::Format_RGB16);
        displayGrayscaleImage(&grayIm, ui->outpuImageLabel_1);

    } else if (filterName == "Original" & inputImage_1->channels == 3) {
        ui->outpuImageLabel_1->clear();
        Image grayIm = inputImage_1->toGrayscale();
        displayRGBImage(inputImage_1, ui->InputImagLabel_1);
        displayGrayscaleImage(&grayIm, ui->outpuImageLabel_1);
        int histogram[256];
        int cumHist[256];
        im_hist(grayIm, histogram, 1);
        cumulative_hist(histogram, cumHist);
        histDisplay(histogram, 0, ui->eqHistPlot);
        CDFDisplay(cumHist, 0,ui->eqCDFPLot);


        int hist_1[256];
        int cumHist_1[256];
        Image *image = new Image(*inputImage_1);
        im_hist(*image, hist_1, 1);
        cumulative_hist(hist_1, cumHist_1);
        histDisplay(hist_1, 1, ui->histPLot);
        CDFDisplay(cumHist_1, 1, ui->CDFPlot);

        int hist_2[256];
        int cumHist_2[256];
        im_hist(*image, hist_2, 2);
        cumulative_hist(hist_2, cumHist_2);
        histDisplay(hist_2, 2, ui->histPLot);
        CDFDisplay(cumHist_2, 2, ui->CDFPlot);

        int hist_3[256];
        int cumHist_3[256];
        im_hist(*image, hist_3, 3);
        cumulative_hist(hist_3, cumHist_3);
        histDisplay(hist_3, 3, ui->histPLot);
        CDFDisplay(cumHist_3, 3, ui->CDFPlot);

    }
}


void MainWindow::histDisplay(int histogram[], int color, QCustomPlot *plot) {
    int max = histogram[0];
    QVector<double> x(256), y(256);
    for (int i = 0; i < 256; i++) {
        if (max < histogram[i]) {
            max = histogram[i];
        }
        x[i] = i;
        y[i] = histogram[i];
    }

    QColor colorArray[4] = {QColor(0, 0, 0), QColor(255, 0, 0), QColor(0, 255, 0), QColor(0, 0, 255)};

    plot->addGraph();
    if (color != 0) {
        plot->graph(color - 1)->setData(x, y);
        plot->graph(color - 1)->setPen(QPen(colorArray[color]));
    } else {
        plot->graph(color)->setData(x, y);
        plot->graph(color)->setPen(QPen(colorArray[color]));
    }

    plot->xAxis->setRange(0, 256);
    plot->yAxis->setRange(0, max);
    plot->replot();
}

void MainWindow::CDFDisplay(int histogram[], int color, QCustomPlot *plot) {
    QVector<double> x(256), y(256);
    for (int i = 0; i < 256; i++) {
        x[i] = i;
        y[i] = histogram[i]/(double )histogram[255];
    }

    QColor colorArray[4] = {QColor(0, 0, 0), QColor(255, 0, 0), QColor(0, 255, 0), QColor(0, 0, 255)};

    plot->addGraph();
    if (color != 0) {
        plot->graph(color - 1)->setData(x, y);
        plot->graph(color - 1)->setPen(QPen(colorArray[color]));
    } else {
        plot->graph(color)->setData(x, y);
        plot->graph(color)->setPen(QPen(colorArray[color]));
    }
    plot->xAxis->setRange(0, 256);
    plot->yAxis->setRange(0, 1);
    plot->replot();
}