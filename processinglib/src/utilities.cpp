//
// Created by abdallah drwesh on 4/3/21.
//

#include "processinglib/utilities.h"
#include "processinglib/histogram.h"
#include <cmath>
#include <algorithm>


void gaussianGeneration(float *kernel, char dim, float sigma, float mean) {
    double r, s = 2.0 * sigma * sigma;
    int t = dim / 2;
    int t2 = dim % 2 == 0 ? t : t + 1;
    float diff_x = 0;
    float diff_y = 0;
    for (int x = -t; x < t2; x++) {
        for (int y = -t; y < t2; y++) {
            diff_x = x - mean;
            diff_y = y - mean;
            r = std::sqrt(diff_x * diff_x + diff_y * diff_y);
            kernel[(x + t) * dim + y + t] = (exp(-(r * r) / s)) / (M_PI * s);
        }
    }
}

void averageGeneration(float *kernel, char dim) {
    int size = dim * dim;
    for (int i = 0; i < size; ++i) {
        kernel[i] = 1.0 / size;
    }
}

float convolution(Image &inputImg, const float *filter, int filterDim, int x, int y, int channelNo) {
    int startX = x - ((filterDim - 1) / 2);
    int startY = y - ((filterDim - 1) / 2);
    float result = 0;
    for (int i = 0; i < filterDim; i++) {
        for (int j = 0; j < filterDim; j++) {
            float dataValue = inputImg(i + startY, j + startX, channelNo);
            float filterValue = filter[(i * 3) + j];
            result += dataValue * filterValue;
        }
    }
    return result;
}

int convSort(Image &inputImg, int filterDim, int x, int y, int channelNo) {
    int startX = x - ((filterDim - 1) / 2);
    int startY = y - ((filterDim - 1) / 2);
    std::vector<int> vals;
    for (int i = 0; i < filterDim; i++) {
        for (int j = 0; j < filterDim; j++) {
            int dataValue = inputImg(i + startY, j + startX, channelNo);
            vals.push_back(dataValue);
        }
    }
    std::sort(vals.begin(), vals.end());
    return vals[vals.size() / 2 + vals.size() % 2];
}

void lpfGeneration(float *kernel, int width, int height, float pWidth, float pHeight) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            kernel[j + i * width] = 0;
        }
    }
    int xi = width / 2 - roundf((pWidth / 2) * width);
    int xf = width / 2 + roundf((pWidth / 2) * width);
    int yi = height / 2 - roundf((pHeight / 2) * height);
    int yf = height / 2 + roundf((pHeight / 2) * height);
    printf("%i, %i, %i, %i\n", xi, xf, yi, yf);
    for (int i = yi; i < yf; ++i) {
        for (int j = xi; j < xf; ++j) {
            kernel[j + i * width] = 1;
        }
    }
}

void hpfGeneration(float *kernel, int width, int height, float pWidth, float pHeight) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            kernel[j + i * width] = 1;
        }
    }
    int xi = width / 2 - roundf((pWidth / 2) * width);
    int xf = width / 2 + roundf((pWidth / 2) * width);
    int yi = height / 2 - roundf((pHeight / 2) * height);
    int yf = height / 2 + roundf((pHeight / 2) * height);
    printf("%i, %i, %i, %i\n", xi, xf, yi, yf);
    for (int i = yi; i < yf; ++i) {
        for (int j = xi; j < xf; ++j) {
            kernel[j + i * width] = 0;
        }
    }
}

Image applyFilter(Image &inputImg, float *filter, int filterDim) {
    Image outputImg{inputImg.width - filterDim + 1, inputImg.height - filterDim + 1, inputImg.channels};
    int offset = filterDim / 2;
    for (int y = offset; y < inputImg.height - offset; y++) {
        for (int x = offset; x < inputImg.width - offset; x++) {
            for (int z = 0; z < inputImg.channels; z++) {
                float result = convolution(inputImg, filter, filterDim, x, y, z);
                outputImg(y - offset, x - offset, z) = result;
            }
        }
    }
    return outputImg;
}

void arrayToScale(double *data, int size) {
    int max = 0;
    for (int i = 0; i < size; ++i) {
        data[i] = std::abs(data[i]);
        if (data[i] > max)
            max = data[i];
    }
    for (int i = 0; i < size; ++i) {
        data[i] = (data[i] / max) * 255;
    }
}

void arrayToScale(float *data, int size) {
    int max = 0;
    for (int i = 0; i < size; ++i) {
        data[i] = std::abs(data[i]);
        if (data[i] > max)
            max = data[i];
    }
    for (int i = 0; i < size; ++i) {
        data[i] = (data[i] / max) * 255;
    }
}

Image saltPepper(Image &image, int count, int sizePixel) {
    Image out = image;
    int xPosition;
    int yPosition;
    int saltPepperNoise;
    while (count) {
        xPosition = rand() % (image.width - sizePixel + 1);
        yPosition = rand() % (image.height - sizePixel + 1);
        saltPepperNoise = rand() % 2;
        for (int i = 0; i < sizePixel; i++) {
            for (int j = 0; j < sizePixel; j++) {
                for (int k = 0; k < image.channels; ++k) {
                    out(i + yPosition, j + xPosition, k) = saltPepperNoise * 255;
                }
            }
        }
        count--;
    }
    return out;
}

Image minMaxNormalize(Image &image) {
    Image normalizedImage{image.width, image.height, image.channels};
    float *max = new float[image.channels];
    float *min = new float[image.channels];
    for (int k = 0; k < image.channels; ++k) {
        max[k] = 0;
        min[k] = 0;
    }

    for (int i = 0; i < image.height; ++i) {
        for (int j = 0; j < image.width; ++j) {
            for (int k = 0; k < image.channels; ++k) {
                if (image(i, j, k) > max[k])
                    max[k] = image(i, j, k);
                if (image(i, j, k) < min[k])
                    min[k] = image(i, j, k);
            }
        }
    }
    for (int k = 0; k < image.channels; ++k) {
        printf("%f, %f -", min[k], max[k]);
    }
    for (int i = 0; i < image.height; ++i) {
        for (int j = 0; j < image.width; ++j) {
            for (int k = 0; k < image.channels; ++k) {
                normalizedImage(i, j, k) = (image(i, j, k) - min[k]) / (max[k] - min[k]);
            }
        }
    }
    return normalizedImage;
}

double imageMax(Image &img) {
    double max = 0;
    for (int i = 0; i < img.height; ++i) {
        for (int j = 0; j < img.width; ++j) {
            if (img(i, j) > max)
                max = img(i, j);
        }
    }
    return max;
}

int approximateAngle(float angle) {
    if (angle > -22.5 && angle <= 22.5) {
        return 0;
    } else if (angle > 22.5 && angle <= 67.5) {
        return 45;
    } else if ((angle > 67.5 && angle <= 90) || (angle > -90 && angle <= -67.5)) {
        return 90;
    } else if (angle > -67.5 && angle <= -22.5) {
        return 135;
    }
}


Image getMagnitude(Image &imgX, Image &imgY) {
    Image outImage{imgX.width, imgX.height, 1};
    for (int i = 0; i < imgX.height; ++i) {
        for (int j = 0; j < imgX.width; ++j) {
            outImage(i, j) = sqrt(imgX(i, j) * imgX(i, j) + imgY(i, j) * imgY(i, j));
        }
    }
    return outImage;
}

Image getDirection(Image &imgX, Image &imgY, bool approximateFour) {
    Image outImage{imgX.width, imgX.height, 1};
    float angel = 0;
    for (int i = 0; i < imgX.height; ++i) {
        for (int j = 0; j < imgX.width; ++j) {
            if (imgX(i, j) == 0) {
                if (imgY(i, j) == 0) {
                    angel = 0;
                } else {
                    if (!approximateFour) {
                        angel = imgY(i, j) > 0 ? 90 : 270;
                    } else {
                        angel = imgY(i, j) > 0 ? 90 : -90;
                    }
                }

            } else {
                angel = atan(imgY(i, j) / imgX(i, j)) * (180 / 3.14159265);
                if (!approximateFour) {
                    if (angel > 0) {
                        if (imgY(i, j) < 0) {
                            angel = angel + 180;
                        }
                    } else {
                        if (imgY(i, j) < 0) {
                            angel = 360 + angel;
                        } else {
                            angel = 180 + angel;
                        }
                    }
                }

            }
            outImage(i, j) = angel;
        }
    }
    return outImage;
}


void sauvolaTechnique(Image &inputImg, int x, int y, int filterDim, double &mean, double &std) {
    int offset = (filterDim - 1) / 2;
    int startX = x - offset;
    int startY = y - offset;
    int N = filterDim * filterDim;
    for (int i = 0; i < filterDim; i++) {
        for (int j = 0; j < filterDim; j++) {
            mean += inputImg(i + startY, j + startX);
        }
    }
    mean = mean / N;
    for (int i = 0; i < filterDim; i++) {
        for (int j = 0; j < filterDim; j++) {
            std += pow(inputImg(i + startY, j + startX) - mean, 2);
        }
    }
    std = sqrt(std / (N - 1));
}

std::vector<std::vector<float>> covarMatrix(const std::vector<std::vector<float>> & mat)
{
    const int rows = mat.size();
    const int cols = mat[0].size();
    const int nsamples = cols;
    std::vector<float> mean = std::vector<float>(rows, (float)0);
    std::vector<std::vector<float>> covar = std::vector<std::vector<float>>(rows);

    for (int i = 0; i < cols; ++i)
        covar[i].resize(rows, (float)0);


    for (int h = 0; h < rows; ++h) {
        for (int w = 0; w < cols; ++w) {
            mean[h] += mat[h][w];
        }
    }

    for (auto& value : mean) {
        value = (1. / cols)* value;
    }

    for (int i = 0; i < rows; ++i) {
        std::vector<float> row_buf(cols, (float)0);
        for (int k = 0; k < cols; ++k)
            row_buf[k] = mat[i][k] - mean[i];

        for (int j = 0; j < rows; ++j) {
            double s0 = 0;
            for (int k = 0; k < cols; ++k) {
                s0 += (row_buf[k] * (mat[j][k] - mean[j]));
            }
            covar[i][j] = (float)s0/ (nsamples - 1) ;
        }
    }

    return covar;
}


bool comparePairs(std::pair<std::vector<float>, float> p1, std::pair<std::vector<float>, float> p2){
    return (p1.second > p2.second);
}

std::vector<std::pair<std::vector<float>, float>> egienVectorsValues(const std::vector<std::vector<float>> &CovarMatrix) {
    Eigen::EigenSolver<Eigen::MatrixXf>  eigensolver;
    Eigen::MatrixXf covarMatrix (CovarMatrix.size(), CovarMatrix.size())  ;
    for (int i = 0; i < CovarMatrix.size();i++ ){
        for (int j = 0; j < CovarMatrix.size(); j++){
            covarMatrix(i,j) = CovarMatrix[i][j];
        }
    }
    eigensolver.compute(covarMatrix);
    Eigen::VectorXf eigen_values = eigensolver.eigenvalues().real();
    Eigen::MatrixXf eigen_vectors = eigensolver.eigenvectors().real();
    std::vector<std::pair<std::vector<float>, float>> result;
    for(int i=0; i<eigen_values.size(); i++){
        std::pair<std::vector<float>, float> tempPair;
        std::vector<float> tempVector;
        for(float element : eigen_vectors.col(i)){
            tempVector.push_back(element);
        }
        tempPair.second = eigen_values(i);
        tempPair.first = tempVector;
        result.push_back(tempPair);
    }
    return result;
}