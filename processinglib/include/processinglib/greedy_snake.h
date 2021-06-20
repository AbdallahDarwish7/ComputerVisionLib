//
// Created by mohamedkamal on ١٤‏/٤‏/٢٠٢١.
//

#ifndef CV_GREEDY_SNAKE_H
#define CV_GREEDY_SNAKE_H

#endif //CV_GREEDY_SNAKE_H
#include "Image.h"

void currentPrevNextPointIndex(int iteration , int numberOfPoints, int * pointsIndexArray);
double averageDistance(int* arrayOfPointsX , int* arrayOfPointsY , int numberOfPOints);
Image imageEnergy(Image &image, char dim,float  mean =0,float sima=3);
double distance(int x1, int y1, int x2, int y2);
void greedySnake(Image&image,int iteration, int numberOfPoints, int* arrayOfPointsX, int* arrayOfPointsY, float sigma, float alpha, float beta, float gamma);