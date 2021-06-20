//
// Created by abdulla167 on ٤‏/٥‏/٢٠٢١.
//

#ifndef CV_HARRIS_CORNER_DETECTOR_H
#define CV_HARRIS_CORNER_DETECTOR_H
#include "vector"
#include "hough.h"

void getHarrisCorner(Image& pointsStrength, std::vector<_Point>& cornerPoints, int threshold);

std::vector<_Point> cornerHarris(Image& grayImg, double threshold, int patchDim = 3);

void drawCornerPoints(Image& img, std::vector<_Point>& cornerPoints);

std::vector<std::pair<std::vector<double>, _Point>> getSIFTDescriptor(Image &inputImg, float threshold = 0.01);
#endif //CV_HARRIS_CORNER_DETECTOR_H