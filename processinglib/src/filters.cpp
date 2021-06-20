//
// Created by abdallah drwesh on 4/3/21.
//

#include "processinglib/filters.h"
#include "processinglib/utilities.h"
#include <cmath>
#include <iostream>
#include "processinglib/fourier_transform.h"
#include "thresholding.h"

void translate(Image *&inputImg) {
    Image out{inputImg->width, inputImg->height, inputImg->channels};
    for (int i = 0; i < inputImg->height; ++i) {
        for (int j = 60; j < inputImg->width; ++j) {
            for (int k = 0; k < inputImg->channels; ++k) {
                (*inputImg)(i, j - 60, k) = (*inputImg)(i, j, k);
            }
        }
    }

    for (int i = 0; i < inputImg->height; ++i) {
        for (int j = (inputImg->width - 60); j < inputImg->width; ++j) {
            for (int k = 0; k < inputImg->channels; ++k) {
                (*inputImg)(i, j, k) = 0;
            }
        }
    }
}

Image avgFilter(Image &inputImg, char dim) {
    auto *kernel = new float[dim * dim];
    averageGeneration(kernel, dim);
    Image im = applyFilter(inputImg, kernel, dim);
    delete[] kernel;
    return im;
}

Image gaussianFilter(Image &inputImg, char dim, float mean, float sigma) {
    auto *kernel = new float[dim * dim];
    gaussianGeneration(kernel, dim, sigma, mean);
    Image im = applyFilter(inputImg, kernel, dim);
    delete[] kernel;
    return im;
}

Image medianFilter(Image &inputImg, int dim) {
    Image outputImg{inputImg.width - dim + 1, inputImg.height - dim + 1, inputImg.channels};
    int begin = dim / 2;
    for (int i = begin; i < inputImg.height - begin; i++) {
        for (int j = begin; j < inputImg.width - begin; j++) {
            for (int k = 0; k < inputImg.channels; k++) {
                outputImg(i - begin, j - begin, k) = convSort(inputImg, dim, j, i, k);
            }
        }
    }
    return outputImg;
}

Image laplacianFilter(Image &inputImg, int dim) {
    float kernel[] = {
            0, -1, 0,
            -1, 4, -1,
            0, -1, 0
    };
    Image im = applyFilter(inputImg, kernel, dim);
    return im;
}

Image perwitEdgeDetector(Image &inputImg) {
    float xFilter[9] = {
            -1, 0, 1,
            -1, 0, 1,
            -1, 0, 1};
    float yFilter[9] = {
            1, 1, 1,
            0, 0, 0,
            -1, -1, -1};
    Image imgX = applyFilter(inputImg, xFilter, 3);
    Image imgY = applyFilter(inputImg, yFilter, 3);
    return getMagnitude(imgX, imgY);
}

Image sobelEdgeDetector(Image &inputImg) {
    float xFilter[9] = {
            -1, 0, 1,
            -2, 0, 2,
            -1, 0, 1};
    float yFilter[9] = {
            1, 2, 1,
            0, 0, 0,
            -1, -2, -1};
    Image Ix = applyFilter(inputImg, xFilter, 3);
    Image Iy = applyFilter(inputImg, yFilter, 3);
    return getMagnitude(Ix, Iy);
}


Image robertsEdgeDetector(Image &inputImg) {
    Image outputImg{inputImg.width, inputImg.height, inputImg.channels};
    char xFilter[4] = {
            1, 0,
            0, -1};
    char yFilter[4] = {
            0, 1,
            -1, 0};
    float xValue = 0;
    float yValue = 0;
    float result = 0;
    for (int y = 0; y < inputImg.height - 1; y++) {
        for (int x = 0; x < inputImg.width - 1; x++) {
            for (int z = 0; z < inputImg.channels; z++) {
                xValue = inputImg(y, x, z) * xFilter[0] + inputImg(y + 1, x + 1, z) * xFilter[3];
                yValue = inputImg(y, x + 1, z) * yFilter[1] + inputImg(y + 1, x, z) * yFilter[2];
                result = sqrt(pow(xValue, 2) + pow(yValue, 2));
                outputImg(y, x, z) = result;
            }
        }
    }
    return outputImg;
}

Image lowPassFilter(Image &inputImg, float *kernel) {
    auto fftComplex = fft2(inputImg);
    auto fftShift = fft2dShift(fftComplex);
    auto fftMirror = mirror_fft2d(fftShift);
    int width = fftMirror[0].size();
    int height = fftMirror.size();
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fftMirror[i][j] = (double) kernel[j + i * width] * fftMirror[i][j];
        }
    }
    Image outputImg = ifft2(fftMirror);
    return outputImg;
}

Image highPassFilter(Image &inputImg, float *kernel) {
    return lowPassFilter(inputImg, kernel);
}

Image globalThresholding(Image &inputImg, int thresholdVal) {
    if (thresholdVal != 0) {
        return buildSegmentedImg(inputImg, thresholdVal);
    } else {
        auto threshold = globalAtsu(inputImg, 256, 3);
        for (int i = 0; i < threshold.size(); ++i) {
            std::cout << threshold[i] << std::endl;
        }
//        auto threshold = globalOptimalIterativeThresholding(inputImg);
//        std::cout << threshold << std::endl;
        return buildSegmentedImg(inputImg, threshold);
    }
}

Image localThresholding(Image &inputImg, float k, int r, int dim) {
    Image outputImg{inputImg.width, inputImg.height, inputImg.channels};
    int offset = (dim - 1) / 2;
    double mean = 0;
    double std = 0;
    for (int y = offset; y < inputImg.height - offset; y++) {
        for (int x = offset; x < inputImg.width - offset; x++) {
            for (int z = 0; z < inputImg.channels; z++) {
                mean = 0, std = 0;
                sauvolaTechnique(inputImg, x, y, dim, mean, std);
                int threshold = mean + (k * ((std / r) - 1));
                if (inputImg(y, x, z) >= threshold) {
                    outputImg(y, x, z) = 255;
                } else {
                    outputImg(y, x, z) = 0;
                }
            }
        }
    }
    return outputImg;
}


