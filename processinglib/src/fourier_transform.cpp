//
// Created by abdallah drwesh on 4/3/21.
//

#include "processinglib/fourier_transform.h"
#include "processinglib/utilities.h"
#include <fftw3.h>
#include <cmath>
#include <complex>

#define REAL 0
#define IMAG 1

double magnitude(double x, double y) {
    double val = x * x + y * y;
    return std::sqrt(val);
}

void multiply(fftw_complex val1, fftw_complex val2, fftw_complex reslult) {
    double real = val1[REAL] * val2[REAL] - val1[IMAG] * val2[IMAG];
    double img = val1[REAL] * val2[IMAG] + val1[IMAG] * val2[REAL];
    reslult[REAL] = real;
    reslult[IMAG] = img;
}

std::vector<std::vector<std::complex<double>>> fft2(Image &inputImg) {
    auto data = inputImg.copyData<double>();
    int nComplexCols = inputImg.width / 2 + 1;
    int fftDateSize = nComplexCols * inputImg.height;
    fftw_complex *fftData = new fftw_complex[fftDateSize];
    fftw_plan plan = fftw_plan_dft_r2c_2d(inputImg.height, inputImg.width, data, fftData, FFTW_ESTIMATE);
    fftw_execute(plan);


    std::vector<std::vector<std::complex<double>>> vecComplex;
    for (int i = 0; i < inputImg.height; ++i) {
        vecComplex.push_back(std::vector<std::complex<double>>{});
        for (int j = 0; j < nComplexCols; ++j) {
            vecComplex.back().push_back(
                    std::complex<double>{fftData[j + nComplexCols * i][REAL], fftData[j + nComplexCols * i][IMAG]});
        }
    }
    fftw_destroy_plan(plan);
    fftw_cleanup();
    delete[] data;
    delete[] fftData;
    return vecComplex;
}


std::vector<std::vector<std::complex<double>>> fft2dShift(std::vector<std::vector<std::complex<double>>> &fftComplex) {
    int width = fftComplex[0].size();
    int height = fftComplex.size();
    std::complex<double> temp;
    int hHeight = height / 2;
    std::vector<std::vector<std::complex<double>>> vecComplex;
    for (int i = 0; i < height; ++i) {
        vecComplex.push_back(std::vector<std::complex<double>>{});
        for (int j = 0; j < width; ++j) {
            vecComplex.back().push_back(fftComplex[i][width - 1 - j]);
        }
    }

    for (int i = 0; i < hHeight / 2; ++i) {
        for (int j = 0; j < width; ++j) {
            temp = vecComplex[i][j];
            vecComplex[i][j] = vecComplex[(hHeight - 1 - i)][j];
            vecComplex[(hHeight - 1 - i)][j] = temp;
        }
    }
    int h3_4 = hHeight + hHeight / 2;
    for (int i = hHeight, k = 0; i < h3_4; ++i, ++k) {
        for (int j = 0; j < width; ++j) {
            temp = vecComplex[i][j];
            vecComplex[i][j] = vecComplex[(height - 1 - k)][j];
            vecComplex[(height - 1 - k)][j] = temp;
        }
    }
    return vecComplex;
}

void fft2dShift(double *data, int width, int height) {
    double temp = 0;
    int hWidth = width / 2;
    int hHeight = height / 2;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < hWidth; ++j) {
            temp = data[j + i * width];
            data[j + i * width] = data[width - 1 - j + i * width];
            data[width - 1 - j + i * width] = temp;
        }
    }

    for (int i = 0; i < hHeight / 2; ++i) {
        for (int j = 0; j < width; ++j) {
            temp = data[j + i * width];
            data[j + i * width] = data[j + (hHeight - 1 - i) * width];
            data[j + (hHeight - 1 - i) * width] = temp;
        }
    }
    int h3_4 = hHeight + hHeight / 2;
    for (int i = hHeight, k = 0; i < h3_4; ++i, ++k) {
        for (int j = 0; j < width; ++j) {
            temp = data[j + i * width];
            data[j + i * width] = data[j + (height - 1 - k) * width];
            data[j + (height - 1 - k) * width] = temp;
        }
    }
}

std::vector<std::vector<std::complex<double>>>
mirror_fft2d(std::vector<std::vector<std::complex<double>>> &fftComplex) {
    int width = fftComplex[0].size();
    int height = fftComplex.size();
    if (width % 2 != 0)
        width -= 2;
    std::vector<std::vector<std::complex<double>>> vecComplex = fftComplex;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            vecComplex[i].push_back(fftComplex[i][width - 1 - j]);
        }
    }
    return vecComplex;
}

Image fft2dMag(Image &inputImg) {
    auto fftComplex = fft2(inputImg);
    auto fftShift = fft2dShift(fftComplex);
    auto fftMirror = mirror_fft2d(fftShift);
    int width = fftMirror[0].size();
    int height = fftMirror.size();
    Image im{width, height, 1};
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            im(i,j) = std::log10(1 + abs(fftMirror[i][j]));
        }
    }
    return im;
}

void reverse(std::vector<std::vector<std::complex<double>>> &fftComplex) {
    int fullwidth = fftComplex[0].size();
    int height = fftComplex.size();
    int width = fullwidth / 2 + 1;
    std::complex<double> temp;
    int hWidth = width / 2;
    int hHeight = height / 2;

    for (int i = 0; i < hHeight / 2; ++i) {
        for (int j = 0; j < width; ++j) {
            temp = fftComplex[i][j];
            fftComplex[i][j] = fftComplex[(hHeight - 1 - i)][j];
            fftComplex[(hHeight - 1 - i)][j] = temp;
        }
    }
    int h3_4 = hHeight + hHeight / 2;
    for (int i = hHeight, k = 0; i < h3_4; ++i, ++k) {
        for (int j = 0; j < width; ++j) {
            temp = fftComplex[i][j];
            fftComplex[i][j] = fftComplex[(height - 1 - k)][j];
            fftComplex[(height - 1 - k)][j] = temp;
        }
    }

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < hWidth; ++j) {
            temp = fftComplex[i][j];
            fftComplex[i][j] = fftComplex[i][width - 1 - j];
            fftComplex[i][width - 1 - j] = temp;
        }
    }
}

Image ifft2(std::vector<std::vector<std::complex<double>>> &fftComplex) {
    int width = fftComplex[0].size();
    int height = fftComplex.size();
    int nComplexCols = width / 2 + 1;
    int fftDateSize = nComplexCols * height;
    fftw_complex *fftData = new fftw_complex[fftDateSize];
    reverse(fftComplex);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < nComplexCols; ++j) {
            fftData[j + i * nComplexCols][REAL] = fftComplex[i][j].real();
            fftData[j + i * nComplexCols][IMAG] = fftComplex[i][j].imag();
        }
    }

    double *ifftData = new double[height * width];

    fftw_plan iplan = fftw_plan_dft_c2r_2d(height, width, fftData, ifftData, FFTW_ESTIMATE);
    fftw_execute(iplan);
    Image outImg{width, height, 1};

    for (int i = 0; i < outImg.height; ++i) {
        for (int j = 0; j < outImg.width; ++j) {
            outImg(i, j) = (ifftData[j + i * outImg.width] / (outImg.height * outImg.width));
        }
    }
    fftw_destroy_plan(iplan);
    fftw_cleanup();
    delete[] fftData;
    delete[] ifftData;
    return outImg;
}

