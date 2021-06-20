/**
 * Created by abdallah drwesh on 5/18/21.
 */

#ifndef CV_THRESHOLDING_H
#define CV_THRESHOLDING_H
#include "Image.h"
#include <vector>

int globalOptimalIterativeThresholding(Image &inputImg);

Image localOptimalIterativeThresholding(Image &inputImg, int blockDim);

std::vector<int> globalAtsu(Image &inputImg, int histSize = 256, int numModes = 2);

Image localAtsu(Image &inputImg, int blockDim, int histSize = 256, int numModes = 2);

Image buildSegmentedImg(Image &inputImg, std::vector<int> &thresholds);

Image buildSegmentedImg(Image &colorImage, std::vector<std::vector<int>> thresholds);

Image buildSegmentedImg(Image &inputImg, int thresholdVal);

void segmentImg(Image &inputImg, Image & outputImg, std::pair<int, int> &yRange, std::pair<int, int> &xRange , int thresholdVal);

void segmentImg(Image &inputImg, Image & outputImg, std::pair<int, int> &yRange, std::pair<int, int> &xRange , std::vector<int> &thresholds);

#endif //CV_THRESHOLDING_H
