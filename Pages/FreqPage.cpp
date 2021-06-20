//
// Created by abdallah drwesh on 4/4/21.
//

#include "../MainWindow.h"
#include "../ui_MainWindow.h"
#include "processinglib/fourier_transform.h"
#include "processinglib/utilities.h"
#include "processinglib/filters.h"

void MainWindow::on_loadImageFreqBtn_clicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "load image", "../");
    std::string filepathStd = filePath.toStdString();
    auto filename = filepathStd.substr(filepathStd.find_last_of("/") + 1);
    ui->imageFreqName->setText(QString(filename.c_str()));
    loadImage(filepathStd, inputFreqImage);
    auto grayImage = inputFreqImage->toGrayscale();
    displayGrayscaleImage(&grayImage, ui->inputImageFreqLabel);
    std::string imageSize = std::to_string(inputFreqImage->size());
    ui->imageFreqSize->setText(QString(imageSize.c_str()));
    Image fftMagImage = fft2dMag(grayImage).toScale();
//    fftMagImage.saveJPG("fft magnitude");
    displayGrayscaleImage(&fftMagImage, ui->inputImageFTLabel);
}

void MainWindow::on_filterFreqSelect_currentIndexChanged(QString filterName) {
    float * kernel = new float [inputFreqImage->width * inputFreqImage->height];
    Image filterImg{inputFreqImage->width, inputFreqImage->height, 1};
    inputFreqImage->saveJPG("input frequency image");
    if (filterName == "Low Pass Filter") {
        lpfGeneration(kernel, filterImg.width, filterImg.height);
        for (int i = 0; i < filterImg.width; ++i) {
            for (int j = 0; j < filterImg.height; ++j) {
                filterImg(i, j) = kernel[j + i * filterImg.width];
            }
        }
        Image result = lowPassFilter(*inputFreqImage, kernel);
//        result.saveJPG("out low pass filter");
        displayGrayscaleImage(&result, ui->outputImageFreqLabel);
    } else if (filterName == "High Pass Filter") {
        hpfGeneration(kernel, filterImg.width, filterImg.height);
        for (int i = 0; i < filterImg.width; ++i) {
            for (int j = 0; j < filterImg.height; ++j) {
                filterImg(i, j) = kernel[j + i * filterImg.width];
            }
        }
        Image result = highPassFilter(*inputFreqImage, kernel).toScale();
//        result.saveJPG("out high pass filter");
        displayGrayscaleImage(&result, ui->outputImageFreqLabel);
    }
    Image filterScale = filterImg.toScale();
//    filterScale.saveJPG(filterName.toStdString() + "img");
    displayGrayscaleImage(&filterScale, ui->freqFilterLabel);
    delete[] kernel;
}

