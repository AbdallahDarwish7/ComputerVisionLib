//
// Created by abdallah drwesh on 4/3/21.
//

#ifndef CV_FOURIER_TRANSFORM_H
#define CV_FOURIER_TRANSFORM_H

#include <vector>
#include <complex>
#include "Image.h"

std::vector<std::vector<std::complex<double>>> fft2(Image &inputImg);

std::vector<std::vector<std::complex<double>>> fft2dShift(std::vector<std::vector<std::complex<double>>> &fftComplex);

void fft2dShift(double * data, int width, int height);

std::vector<std::vector<std::complex<double>>> mirror_fft2d(std::vector<std::vector<std::complex<double>>> &fftComplex);

Image fft2dMag(Image &inputImg);


Image ifft2(std::vector<std::vector<std::complex<double>>>& fftComplex);

Image image_of_fft(double * fftData);
#endif //CV_FOURIER_TRANSFORM_H
