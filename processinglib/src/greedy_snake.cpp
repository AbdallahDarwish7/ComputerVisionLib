//
// Created by mohamedkamal on ١٤/٤/٢٠٢١.
//
#include "greedy_snake.h"
#include <cmath>
#include "algorithm"
#include "filters.h"

using namespace std;
int pointsCount = 70;
void currentPrevNextPointIndex(int iteration , int numberOfPoints, int * pointsIndexArray) {
    int prevIndex = iteration - 1;
    int nextIndex = iteration + 1;
    if (prevIndex == -1) {
        prevIndex =  numberOfPoints - 1;
    }

    if (nextIndex > numberOfPoints - 1) {
        nextIndex = 0;
    }
    pointsIndexArray[0] = iteration;
    pointsIndexArray[1] = prevIndex;
    pointsIndexArray[2] = nextIndex;
}

double averageDistance(int* arrayOfPointsX , int* arrayOfPointsY  , int numberOfPOints) {
    double sum = 0;
    for (int i =0; i<numberOfPOints; i++) {
        int nextIndex = i + 1;
        if ( i == (numberOfPOints - 1)){
            nextIndex = 0;
        }
        sum += distance(arrayOfPointsX[i], arrayOfPointsY[i], arrayOfPointsX[nextIndex], arrayOfPointsY[nextIndex]);
    }
    return sum/numberOfPOints;
}

Image imageEnergy(Image &image, char dim,float  mean,float sima) {
    Image gausImage = gaussianFilter(image , dim, mean, sima);
    return sobelEdgeDetector(gausImage);
}

double distance(int x1, int y1, int x2, int y2){
    double temp = pow((double )(x1 - x2),2) + pow((double )(y1 - y2),2);
   return sqrt(temp);
}



void greedySnake(Image&image,int iteration, int numberOfPoints, int* arrayOfPointsX, int* arrayOfPointsY, float sigma, float alpha, float beta, float gamma) {
    int neighbourDim = 5;
    float curvatureThreshold = 0.3;
    float imgEnergyThreshold = 250;
    int counter = 0;
    float arrayOfAlpha[pointsCount] ;
    float arrayOfBeta[pointsCount];
    float arrayOfgamma[pointsCount] ;
    float energyCountinuty[25];
    float energyCurvature[25];
    float energyImage[25];
    float pointsCurvature[pointsCount];
    float pointsImageEnergy[pointsCount];
    Image energyOfImage = imageEnergy(image,sigma);

    // average siatance between each two points
    double avgDist = averageDistance(arrayOfPointsX, arrayOfPointsY, numberOfPoints);
    // number of points in each vicinity
    int neighpourCount = pow((double )neighbourDim,2);

    // the offsets needed when examining the vicinity
    int neighbourCoordinate[25][2] = {
            {-2,-2},{-1,-2},{0,-2},{1,-2},{2,-2},
            {-2,-1},{-1,-1},{0,-1},{1,-1},{2,-1},
            {-2,0},{-1,0},{0,0},{1,0},{2,0},
            {-2,1},{-1,1},{0,1},{1,1},{2,1},
            {-2,2},{-1,2},{0,2},{1,2},{2,2}
    };
    for (int i = 0; i<numberOfPoints;i++){
        arrayOfAlpha[i] = alpha;

            arrayOfBeta[i] = beta;

        arrayOfgamma[i] = gamma;

    }

    bool loop = true;
    // while loop
    while (loop) {
        int countMovedPoints = 0;

        for ( int i =0; i<numberOfPoints; i++){
            int pointIndexArray[3];
            currentPrevNextPointIndex(i,numberOfPoints,pointIndexArray);

            for (int j =0; j<neighpourCount;j++){
                int x = 0;
                int y = 0;
                double temp = 0;
                x = arrayOfPointsX[i] + neighbourCoordinate[j][0];
                y = arrayOfPointsY[i] + neighbourCoordinate[j][1];
                temp = distance(x, y, arrayOfPointsX[pointIndexArray[1]], arrayOfPointsY[pointIndexArray[1]]);
                energyCountinuty[j] = abs(avgDist - temp);
                temp = distance(arrayOfPointsX[pointIndexArray[1]] + arrayOfPointsX[pointIndexArray[2]],
                                arrayOfPointsY[pointIndexArray[1]] + arrayOfPointsY[pointIndexArray[2]],
                                2 * x, 2 * y);
                energyCurvature[j] = pow(temp,2);
                energyImage[j] = energyOfImage(y,x );
            }
            float maxCurv = *std::max_element(energyCurvature, energyCurvature + neighpourCount);
            float maxcountinuty = *std::max_element(energyCountinuty, energyCountinuty + neighpourCount);
            float maxImage = *std::max_element(energyImage, energyImage + neighpourCount);
            float minImage = *std::min_element(energyImage, energyImage + neighpourCount);
            if ((maxImage - minImage) < 5){
                minImage = maxImage -5;
            }

            float eSnake[neighpourCount];
            int maxIndex = 0;
            float minSnake = 0;
            for (int k =0; k<neighpourCount; k++){
                energyCurvature[k] = energyCurvature[k]/ maxCurv;
                energyCountinuty[k] = energyCountinuty[k]/maxcountinuty;
                energyImage[k] = (minImage - energyImage[k])/(maxImage - minImage);
                float beta = arrayOfBeta[i];

                eSnake[k] = energyCountinuty[k]*arrayOfAlpha[i] + energyCurvature[k]*beta + energyImage[k]*arrayOfgamma[i] ;
                if (k>0) {
                    if (eSnake[k] < minSnake){
                        maxIndex = k;
                        minSnake = eSnake[k];
                    }
                } else if (k ==0){
                    minSnake = eSnake[0];
                }
            }
            if(maxIndex != 12
            ) {
                countMovedPoints++;
                arrayOfPointsX[i] = arrayOfPointsX[i] + neighbourCoordinate[maxIndex][0];
                arrayOfPointsY[i] = arrayOfPointsY[i] + neighbourCoordinate[maxIndex][1];
            }


            pointsImageEnergy[i] = energyImage[maxIndex];
        }
        // iterate over all snake points to find curvature
        for (int i=0; i<numberOfPoints; i++) {
            int pointIndexArray[3];
            currentPrevNextPointIndex(i,numberOfPoints,pointIndexArray);
            int uX = arrayOfPointsX[i] - arrayOfPointsX[pointIndexArray[1]];
            int uY = arrayOfPointsY[i] - arrayOfPointsY[pointIndexArray[1]];
            int uX1 = arrayOfPointsX[pointIndexArray[2]] - arrayOfPointsX[i];
            int uY1 = arrayOfPointsY[pointIndexArray[2]] - arrayOfPointsY[i] ;

            float norm = sqrt(pow((double )uX,2)+ pow((double)uY,2));
            float norm1 = sqrt(pow((double )uX1,2)+ pow((double)uY1,2));
            pointsCurvature[i] = pow(distance(uX / norm, uY / norm, uX1 / norm1, uY1 / norm1), 2);
        }
        // iterate over snake points to find where to relax beta
        for (int i=0; i<numberOfPoints; i++) {
            int pointIndexArray[3];
            currentPrevNextPointIndex(i,numberOfPoints,pointIndexArray);
            float curvature = pointsCurvature[i];
            if(curvature> pointsCurvature[pointIndexArray[1]] &&
                curvature> pointsCurvature[pointIndexArray[2]] &&
                curvature>curvatureThreshold &&
                pointsImageEnergy[i]> imgEnergyThreshold &&
                arrayOfBeta[i]!=0){
                arrayOfBeta[i] = 0;
            }

        }
        counter++;
        if (counter == iteration || countMovedPoints <3)
        { loop = false; }
        avgDist = averageDistance(arrayOfPointsX,arrayOfPointsY,numberOfPoints);
    }
}