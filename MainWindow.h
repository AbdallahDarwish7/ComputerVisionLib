//
// Created by abdallah drwesh on 4/3/21.
//

#ifndef CV_MAINWINDOW_H
#define CV_MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "processinglib/Image.h"
#include <opencv2/imgproc/imgproc.hpp>
#include "qcustomplot.h"
#include "ui_MainWindow.h"
#include "vector"
#include "processinglib/point.h"
using namespace cv;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT
    std::string testImage;
    static const int pointsCount = 70;
    int centerX = 0;
    int centerY = 0 ;
    int raduis = 0;
    Image *inputImage = nullptr;
    Image *outputImage = nullptr;
    Image *noiseImage = nullptr;


    Image *imageA = nullptr;
    Image *imageB = nullptr;

    Image *inputImage_1 = nullptr;
    Image *outputImage_1 = nullptr;

    Image *inputFreqImage = nullptr;

    Image *snakeImage = nullptr;

    Image * imageMatch_1 = nullptr;
    Image * imageMatch_2 = nullptr;
    Image * thresholdingImage = nullptr;
    Image * segmentationImage = nullptr;
    Image * segmentationOutputImage = nullptr;

    QCPItemPixmap *ImageDisplay;
    QCPGraph *points;
    QCPGraph *roc;
    QCPGraph *imageMatchPoints1;
    QCPGraph *imageMatchPoints2;
    QCPGraph *regionGrowing;
    QVector<double> *xData = new QVector<double>();
    QVector<double> *yData = new QVector<double>();

    std::vector<point> DataRG;

    QVector<double> *xDataMatching_1 = new QVector<double>();
    QVector<double> *yDataMatching_1 = new QVector<double>();
    QVector<double> *xDataMatching_2 = new QVector<double>();
    QVector<double> *yDataMatching_2 = new QVector<double>();
    QVector<QCPItemLine*> *lines = new QVector<QCPItemLine*>();
    int arrayOfPointsX [pointsCount] ;
    int arrayOfPointsY [pointsCount] ;


public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private slots:

    void on_loadImageBtn_clicked();

    void on_LoadImageBtn_1_clicked();

    void on_filterSelect_currentIndexChanged(QString);

    void on_filterSelect_2_currentIndexChanged(QString);

    void on_imageSelect_currentIndexChanged(QString);

    void on_imageABtn_clicked();

    void on_imageBBtn_clicked();

    void on_faceRecog_clicked();

    void on_hybridBtn_clicked();

    void on_loadImageFreqBtn_clicked();

    void on_loadSnake_clicked();

    void on_start_clicked();

    void on_clearBtn_clicked();

    void on_loadImageMatchBtn_1_clicked();

    void on_loadImageMatchBtn_2_clicked();

    void on_startMatchingBtn_clicked();

    void on_matchClearBtn_clicked();

    void on_filterFreqSelect_currentIndexChanged(QString);

    void on_clearSegmentBtn_clicked();

    void on_reRunBtn_clicked();

    void on_thHighCannySlider_valueChanged(int);

    void on_sigmaCannySlider_valueChanged(int);

    void on_thLowCannySlider_valueChanged(int);

    void on_segmentImgBtn_clicked();

    void startSegmentation();



protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::MainWindow *ui;

    void initializeCustomPlot(QCustomPlot * plot);

    void loadImage(std::string filepath, Image *&image, int numChannels = 1);

    void displayRGBImage(Image *image, QLabel *label);

    void displayGrayscaleImage(Image *image, QLabel *label);

    void histDisplay(int histogram[], int color, QCustomPlot *plot);

    void CDFDisplay(int histogram[], int color, QCustomPlot *plot);
};

#endif //CV_MAINWINDOW_H
