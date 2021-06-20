//
// Created by abdallah drwesh on 4/4/21.
//
#include "../MainWindow.h"
#include "../ui_MainWindow.h"
#include <QFileDialog>
#include "processinglib/filters.h"
#include "processinglib/utilities.h"
#include "processinglib/canny.h"
#include "processinglib/hough.h"
#include <iostream>
#include "processinglib/harris_corner_detector.h"
#include "processinglib/thresholding.h"
#include "processinglib/face_detection_opencv.h"
#include "processinglib/face_recognition.h"


void MainWindow::on_loadImageBtn_clicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "load image", "../");
    std::string filepathStd = filePath.toStdString();
    auto filename = filepathStd.substr(filepathStd.find_last_of("/") + 1);
    ui->imageName->setText(QString(filename.c_str()));
    loadImage(filepathStd, inputImage, 3);
    if (inputImage->channels > 1){
        displayRGBImage(inputImage, ui->inputImageLabel);
    } else {
        displayGrayscaleImage(inputImage, ui->inputImageLabel);
    }
    std::string imageSize = std::to_string(inputImage->size());
    ui->imageSize->setText(QString(imageSize.c_str()));
}

void MainWindow::on_filterSelect_currentIndexChanged(QString filterName) {
    std::cout << filterName.toStdString() << std::endl;
    if (filterName == "Average") {
        auto grayImage = noiseImage->toGrayscale();
        auto im = avgFilter(grayImage, 7);
        displayGrayscaleImage(&im, ui->outputImageLabel);
//        im.saveJPG("out_average");
    } else if (filterName == "Gaussian") {
        auto grayImage = noiseImage->toGrayscale();
        auto im = gaussianFilter(grayImage, 7);
        displayGrayscaleImage(&im, ui->outputImageLabel);
//        im.saveJPG("out_gaussian");
    } else if (filterName == "Median") {
        auto grayImage = noiseImage->toGrayscale();
        auto im = medianFilter(grayImage, 7);
        displayGrayscaleImage(&im, ui->outputImageLabel);
//        im.saveJPG("out_median");
    } else if (filterName == "Prewitt") {
        auto grayImage = inputImage->toGrayscale();
        displayGrayscaleImage(&grayImage, ui->inputImageLabel);
        auto im = perwitEdgeDetector(grayImage).toScale();
        displayGrayscaleImage(&im, ui->outputImageLabel);
//        im.saveJPG("out_prewitt");
    } else if (filterName == "Roberts") {
        auto grayImage = inputImage->toGrayscale();
        displayGrayscaleImage(&grayImage, ui->inputImageLabel);
//        auto im = robertsEdgeDetector(grayImage).toScale();
//        auto im = translate(grayImage);
//        im.saveJPG("out_robot");
//        displayGrayscaleImage(&im, ui->outputImageLabel);
    } else if (filterName == "Sobel") {
        auto grayImage = inputImage->toGrayscale();
        displayGrayscaleImage(&grayImage, ui->inputImageLabel);
        auto im = sobelEdgeDetector(grayImage).toScale();
        displayGrayscaleImage(&im, ui->outputImageLabel);
//        im.saveJPG("out_sobel");
    } else if (filterName == "Salt and Pepper Noise") {
        delete noiseImage;
        noiseImage = new Image(inputImage->width, inputImage->height, inputImage->channels);
        auto grayImage = inputImage->toGrayscale();
        auto im = saltPepper(grayImage, 100, 2);
        for (int i = 0; i < im.height; ++i) {
            for (int j = 0; j < im.width; ++j) {
                for (int k = 0; k < im.channels; ++k) {
                    (*noiseImage)(i, j, k) = im(i, j, k);
                }
            }
        }
        displayGrayscaleImage(&im, ui->inputImageLabel);
//        im.saveJPG("out_salt and pepper");
    } else if (filterName == "Normalize") {
        auto grayImage = inputImage->toGrayscale();
//        displayGrayscaleImage(&grayImage, ui->inputImageLabel);
//        auto im = minMaxNormalize(grayImage).toScale();
//        displayGrayscaleImage(&im, ui->outputImageLabel);
//        displayRGBImage(&im, ui->outputImageLabel);
//        im.saveJPG("out_normalize");

//        auto im2 = detectFace(grayImage);
//        displayGrayscaleImage(&im2, ui->outputImageLabel);
        auto im2 = detectFace(*inputImage);
        displayRGBImage(&im2, ui->outputImageLabel);
    } else if (filterName == "Global Thresholding") {
        auto grayImage = inputImage->toGrayscale();
        displayGrayscaleImage(&grayImage, ui->inputImageLabel);
        auto im = globalThresholding(grayImage, 0);
        displayGrayscaleImage(&im, ui->outputImageLabel);
//        im.saveJPG("out_global_thresholding");
    } else if (filterName == "Local Thresholding") {
        auto grayImage = inputImage->toGrayscale();
        displayGrayscaleImage(&grayImage, ui->inputImageLabel);
        auto im = localThresholding(grayImage, 0, 128, 15);
        displayGrayscaleImage(&im, ui->outputImageLabel);
//        im.saveJPG("out_local_thresholding");
    } else if (filterName == "Canny") {
        auto grayImage = inputImage->toGrayscale();
        auto input = Image(inputImage);
        auto cannyImage = cannyEdgeDetector(grayImage);
        displayGrayscaleImage(&cannyImage, ui->outputImageLabel);

        ui->sigmaCannySlider->setValue(30);
        ui->thLowCannySlider->setValue(0.05 * 255 + 1);
        ui->thHighCannySlider->setValue(0.5 * 255 + 1);
    } else if (filterName == "Hough Lines") {
        auto grayImage = inputImage->toGrayscale();
        auto input = Image(inputImage);
        auto cannyImage = cannyEdgeDetector(grayImage, 3, 0.5, 0.1);
        // house -> thLow= 0.1, maxGap =7
        // forest -> thLow=0.16, sigma = 2.7, maxGap=1
        auto houghData = houghLineTransform(cannyImage, 1, 1);
        auto ps = linePeaks(houghData.houghImage, 200);
        auto lines = houghLines(cannyImage, ps, houghData.theta, houghData.roh, 7);
        auto image = drawLines(lines, input, cannyImage);
        displayRGBImage(&input, ui->outputImageLabel);
    } else if (filterName == "Hough Circles") {
        auto grayImage = inputImage->toGrayscale();
        auto input = Image(inputImage);
        auto cannyImage = cannyEdgeDetector(grayImage, 3, 0.5, 0.1);
        // circles-image -> thLow = 0.10
        auto range = std::pair<int, int>(20, 100);
        auto circles = houghCircles(cannyImage, range, 50);
//        auto houghImages = houghCircleTransform(cannyImage, range);
//        auto peaks = circlePeaks(houghImages, 1);
//        auto circles = houghCircles(input, cannyImage, peaks,range);
        auto image = drawCircles(circles, input, cannyImage);
        displayRGBImage(&image, ui->outputImageLabel);
//        delete houghImages;
    } else if (filterName == "Harris Corner") {
        auto grayImage = inputImage->toGrayscale();
        Image colorImg{grayImage.width, grayImage.height, 3};
        for (int y = 0; y < grayImage.height; y++) {
            for (int x = 0; x < grayImage.width; x++) {
                for (int z = 0; z < 3; z++) {
                    colorImg(y, x, z) = grayImage(y, x);
                }
            }
        }
        std::vector<_Point> corners = cornerHarris(grayImage, 0.01, 3);
        auto des = getSIFTDescriptor(grayImage, 0.01);
        std::vector<_Point> ps;
        for (auto &vec_point: des) {
            ps.push_back(vec_point.second);
        }
        drawCornerPoints(colorImg, ps);
        displayRGBImage(&colorImg, ui->outputImageLabel);
//        for (auto &featureV: des) {
//            for (auto val: featureV.first) {
//                std::cout << val << ", ";
//            }
//            std::cout << std::endl;
//        }
    } else if (filterName == "GlobalAtsu") {
        auto grayImage = inputImage->toGrayscale();
        displayGrayscaleImage(&grayImage, ui->inputImageLabel);
        auto thresolds = globalAtsu(grayImage, 256, 3);
        auto im = buildSegmentedImg(grayImage, thresolds);
        displayGrayscaleImage(&im, ui->outputImageLabel);
    } else if (filterName == "LocalAtsu") {
        auto grayImage = inputImage->toGrayscale();
        displayGrayscaleImage(&grayImage, ui->inputImageLabel);
        auto im = localAtsu(grayImage, 5, 256, 3);
        displayGrayscaleImage(&im, ui->outputImageLabel);
    } else if (filterName == "GlobalOptimalIterative") {
        auto grayImage = inputImage->toGrayscale();
        displayGrayscaleImage(&grayImage, ui->inputImageLabel);
        auto thresolds = globalOptimalIterativeThresholding(grayImage);
        auto im = buildSegmentedImg(grayImage, thresolds);
        displayGrayscaleImage(&im, ui->outputImageLabel);
    } else if (filterName == "LocalOptimalIterative") {
        auto grayImage = inputImage->toGrayscale();
        displayGrayscaleImage(&grayImage, ui->inputImageLabel);
        auto im = localOptimalIterativeThresholding(grayImage, 7);
        displayGrayscaleImage(&im, ui->outputImageLabel);
    } else if (filterName == "Face Detection"){
        auto im = detectFace(*inputImage);
        if (im.channels > 1){
            displayRGBImage(&im, ui->outputImageLabel);
        } else {
            displayGrayscaleImage(&im, ui->outputImageLabel);
        }
    } else if(filterName == "FaceRecognition"){
        std::vector<std::vector<float>> TrainingDataset;
        auto filenames = loadImgsDataset("../Dataset/train", TrainingDataset);
        std::cout << "Done Loading" << std::endl;
        /*training*/
//        std::vector<std::vector<float>> CenteredImgs = GetCenteredImgs(TrainingDataset);
//        std::cout << "Done Zero Avg" << std::endl;
//        vector<vector<float>> EigenVectorsUpper = GetEigenVectorsOfUpperCorr(CenteredImgs);
//        vector<vector<float>> CoffMat = getImagesCoeff(TrainingDataset, EigenVectorsUpper);

        /*testing*/
        Image TestIm = Image(inputImage).toGrayscale();
        displayGrayscaleImage(&TestIm, ui->inputImageLabel);
        vector<vector<float>> CoffMat;
        vector<vector<float>> EigenVectorsUpper;
        ReadFileToVector("../Coefficient_Matrix.txt", CoffMat);
        ReadFileToVector("../Eigen_Vectors_Matrix.txt", EigenVectorsUpper);
        std::pair<int, float> result = PredictImg(TestIm.ImageAsVector(), EigenVectorsUpper, CoffMat);
        Image SameImage {TrainingDataset[result.first],  92, 112, 1};
        QString filenameQ{filenames[result.first].c_str()};
        ui->recognizedFaceLabel->clear();
        ui->recognizedFaceLabel->setText(filenameQ);
        displayGrayscaleImage(&SameImage, ui->outputImageLabel);
    }
}


void MainWindow::on_thHighCannySlider_valueChanged(int val) {
    ui->thHighCannyValLabel->setNum((int) (val / 255. * 100));
    auto grayImage = Image(inputImage);
    auto im = cannyEdgeDetector(grayImage, ui->sigmaCannySlider->value() / 10., val / 255.,
                                ui->thLowCannySlider->value() / 255.);
    displayGrayscaleImage(&im, ui->outputImageLabel);
}

void MainWindow::on_thLowCannySlider_valueChanged(int val) {
    ui->thLowCannyValLabel->setNum((int) (val / 255. * 100));
    auto grayImage = Image(inputImage);
    auto im = cannyEdgeDetector(grayImage, ui->sigmaCannySlider->value() / 10., ui->thHighCannySlider->value() / 255.,
                                val / 255.);
    displayGrayscaleImage(&im, ui->outputImageLabel);
}

void MainWindow::on_sigmaCannySlider_valueChanged(int val) {
    ui->sigmaCannyValLabel->setNum(val / 10.);
    auto grayImage = Image(inputImage);
    auto im = cannyEdgeDetector(grayImage, val / 10, ui->thHighCannySlider->value() / 255.,
                                ui->thLowCannySlider->value() / 255.);
    displayGrayscaleImage(&im, ui->outputImageLabel);
}
