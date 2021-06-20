//
// Created by abdallah drwesh on 4/3/21.
//

#ifndef CV_HISTOGRAM_H
#define CV_HISTOGRAM_H

#include "Image.h"

void im_hist(Image &image, int histogram[], int channel, int initIndex = 0, int histSize = 256);

void im_hist(Image &image, std::pair<int, int> yRange, std::pair<int, int> xRange, int histogram[], int channel,
             int initIndex = 0, int histSize = 256);

void smooth_hist(Image &image, int histogram[], double smoothedHist[], int windowSmooth, int initIndex,
                 int smoothedHistSize);

void cumulative_hist(int histogram[], int cumulative_histogram[]);

void hist_eq_formula(Image &image, int cumhistogram[], int histogram_equalization_formula[]);

void equalized_image(Image &inImage, int histogram_equalization_formula[]);

void getNormalizedHist(int *hist, double *normalizedHist, int histSize, int pixelsNo);

#endif //CV_HISTOGRAM_H
