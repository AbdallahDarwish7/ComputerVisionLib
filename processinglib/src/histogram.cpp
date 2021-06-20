//
// Created by abdallah drwesh on 4/3/21.
//

#include "processinglib/histogram.h"

#include <iostream>

using namespace std;

#include <cmath>

void im_hist(Image &image, int histogram[], int channel, int initIndex,int histSize) {

    // initialize all intensity values to 0
    for (int i = initIndex; i < histSize + initIndex; i++) {
        histogram[i] = 0;
    }

    // calculate the histgram
    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            histogram[(int) image(y, x, channel - 1) + initIndex]++;
        }
    }
}

void im_hist(Image &image, std::pair<int, int> yRange, std::pair<int, int> xRange,int histogram[], int channel, int initIndex,int histSize) {

    // initialize all intensity values to 0
    for (int i = initIndex; i < histSize + initIndex; i++) {
        histogram[i] = 0;
    }

    // calculate the histgram
    for (int y = yRange.first; y < yRange.second; y++) {
        for (int x = xRange.first; x < xRange.second; x++) {
            histogram[(int) image(y, x, channel - 1) + initIndex]++;
        }
    }
}

void smooth_hist(Image &image, int histogram[],double smoothedHist[],int windowSmooth, int initIndex,int smoothedHistSize) {
    double sum;
    for (int i = initIndex; i < smoothedHistSize + initIndex; ++i) {
        sum = 0;
        for (int j = -windowSmooth; j < windowSmooth; ++j) {
            sum += histogram[j + windowSmooth + i];
        }
        smoothedHist[i] = sum / (2 * windowSmooth + 1);
    }
}

void cumulative_hist(int histogram[], int cumulative_histogram[]) {
    cumulative_histogram[0] = histogram[0];

    for (int i = 1; i < 256; i++) {
        cumulative_histogram[i] = histogram[i] + cumulative_histogram[i - 1];
    }
}

void hist_eq_formula(Image &image, int cumhistogram[], int histogram_equalization_formula[]) {
    // Caluculate the size of image
    int image_size = image.height * image.width;

    float alpha = (255.0 / (image_size));
    // Scale the histogram
    for (int i = 0; i < 256; i++) {
        histogram_equalization_formula[i] = (unsigned char) round(((double) cumhistogram[i]) * alpha);
    }

}


void equalized_image(Image &inImage, int histogram_equalization_formula[]) {
    cout << inImage(0, 0, 0);
    for (int y = 0; y < inImage.height; y++) {
        for (int x = 0; x < inImage.width; x++) {
            inImage(y, x) = (unsigned char) histogram_equalization_formula[(int) inImage(y, x)];
        }
    }
}

void getNormalizedHist(int *hist, double *normalizedHist, int histSize, int pixelsNo) {
    for (int i = 0; i < histSize; i++) {
        normalizedHist[i] = hist[i] / (double) pixelsNo;
    }
}
