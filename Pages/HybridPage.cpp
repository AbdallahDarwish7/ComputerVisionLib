//
// Created by abdallah drwesh on 4/4/21.
//
#include "../MainWindow.h"
#include "../ui_MainWindow.h"
#include <QFileDialog>
#include "processinglib/filters.h"


void MainWindow::on_imageABtn_clicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "load image", "../");
    std::string filepathStd = filePath.toStdString();
    auto filename = filepathStd.substr(filepathStd.find_last_of("/") + 1);
    ui->imageAName->setText(QString(filename.c_str()));
    loadImage(filepathStd, imageA);
    auto grayImage = imageA->toGrayscale();
    displayGrayscaleImage(&grayImage, ui->imageALabel);
    std::string imageSize = std::to_string(imageA->size());
    ui->imageASize->setText(QString(imageSize.c_str()));
}

void MainWindow::on_imageBBtn_clicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "load image", "../");
    std::string filepathStd = filePath.toStdString();
    auto filename = filepathStd.substr(filepathStd.find_last_of("/") + 1);
    ui->imageBName->setText(QString(filename.c_str()));
    loadImage(filepathStd, imageB);
    auto grayImage = imageB->toGrayscale();
    displayGrayscaleImage(&grayImage, ui->imageBLabel);
    std::string imageSize = std::to_string(imageB->size());
    ui->imageBSize->setText(QString(imageSize.c_str()));
}

void MainWindow::on_hybridBtn_clicked() {
    auto grayImage1 = imageA->toGrayscale();
    auto im1 = gaussianFilter(grayImage1);
//    im1.saveJPG("imageA");
    auto grayImage2 = imageB->toGrayscale();
    auto im2 = laplacianFilter(grayImage2);
//    im2.saveJPG("imageB");
    Image hybrid{im1.width, im1.height, 1};
    for (int i = 0; i < im1.height; ++i) {
        for (int j = 0; j < im1.width; ++j) {
            hybrid(i, j) = im1(i, j) + im2(i, j);
        }
    }
    auto displayImg = hybrid.toScale();
//    displayImg.saveJPG("out_hybrid");
    displayGrayscaleImage(&displayImg, ui->hybridImageLabel);
}

