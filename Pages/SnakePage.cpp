//
// Created by mohamedkamal on ١٣‏/٤‏/٢٠٢١.
//

#include "../MainWindow.h"
#include "../ui_MainWindow.h"
#include <QFileDialog>
#include <iostream>
# include "processinglib/greedy_snake.h"

using namespace std;
using namespace cv;
void MainWindow::on_start_clicked(){


   if (snakeImage != nullptr && !xData->empty()) {
        qDebug() << "started";
        int blurred = ui->checkBlurr->isChecked();

        for (int count = 0; count < (8 - blurred * 7); count++) {
            for (int k = 0; k < 4 - 2*blurred; k++) {
                for (int j = 0; j < 5 - k ; j++) {
                    float alpha = 1.5, beta = 1.7, gamma = 2.5, sigma = 9 - 2 * j - 2 * k - 2 * blurred, maxIt = 30;

                    for (int i = 0; i < 5 - j - k - blurred; i++) {
                        Image image = snakeImage->toGrayscale();
                        greedySnake(image, maxIt, pointsCount, arrayOfPointsX, arrayOfPointsY, sigma, alpha, beta,
                                    gamma);

                        sigma = sigma - 2;

                    }
                }
            }
        }
        xData->clear(), yData->clear();
        for (int i = 0; i < pointsCount; i++) {
            int xCoordinate = ceil(
                    (arrayOfPointsX[i] / (float) snakeImage->width) * ui->snake->background().rect().width());
            int yCoordinate = ceil(
                    (arrayOfPointsY[i] / (float) snakeImage->height) * ui->snake->background().rect().height());

            xData->append(ui->snake->xAxis->pixelToCoord(xCoordinate));
            yData->append(ui->snake->yAxis->pixelToCoord(yCoordinate));
        }
        points->setData(*xData, *yData);
        ui->snake->replot();
    }

}
void MainWindow::on_loadSnake_clicked() {

    QString filePath = QFileDialog::getOpenFileName(this, "load image", "../");
    std::string filepathStd = filePath.toStdString();
    auto filename = filepathStd.substr(filepathStd.find_last_of("/") + 1);
    QPixmap pixmap{filePath};
    snakeImage = new Image(filepathStd,3);

    QImage qImage(snakeImage->width, snakeImage->height, QImage::Format_RGB16);
    QRgb rgb;
    for (int j = 0; j < snakeImage->width; ++j) {
        for (int i = 0; i < snakeImage->height; ++i) {
            rgb = qRgb((*snakeImage)(i, j,0), (*snakeImage)(i, j,1), (*snakeImage)(i, j,2));
            qImage.setPixel(j, i, rgb);
        }
    }
    int width = ui->snake->width() ;
    int height = ui->snake->height();
    ui->snake->setBackground(QPixmap::fromImage(qImage).scaled(width, height,Qt::IgnoreAspectRatio, Qt::FastTransformation));
    ui->snake->setAutoFillBackground(true);
    ui->snake->replot();



}

void  MainWindow::on_clearBtn_clicked() {
    xData->clear();
    yData->clear();
    points->data()->clear();
    ui->snake->replot();
    centerX = 0 , centerY = 0, raduis = 0;
}

