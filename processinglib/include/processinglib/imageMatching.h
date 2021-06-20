//
// Created by mohamedkamal on ٦‏/٥‏/٢٠٢١.
//

#ifndef CV_IMAGEMATCHING_H
#define CV_IMAGEMATCHING_H
#include "Image.h"
#include "vector"
std::vector<double> SSDMatching(Image& inputImage_1, Image& inputImage_2);
std::vector<double> normalizedCorrelation(Image& inputImage_1, Image&inputImage_2);

#endif //CV_IMAGEMATCHING_H
