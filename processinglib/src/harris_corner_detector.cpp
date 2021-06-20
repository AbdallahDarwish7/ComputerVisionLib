//
// Created by abdulla167 on ٣/٥/٢٠٢١.
//
#include "Image.h"
#include "filters.h"
#include <cmath>
#include <iostream>
#include <utilities.h>
#include "hough.h"
#include "vector"
#include "harris_corner_detector.h"

void getHarrisCorner(Image &pointsStrength, std::vector<_Point> &cornerPoints, double threshold) {
    for (int y = 4; y < pointsStrength.height - 4; ++y) {
        for (int x = 4; x < pointsStrength.width - 4; ++x) {
            bool cornerPoint = true;
            if (pointsStrength(y, x) < threshold)
                continue;
            for (int i = -4; i < 5; ++i) {
                for (int j = -4; j < 5; ++j) {
                    if (pointsStrength(y + i, x + j) > pointsStrength(y, x)) {
                        i = 5;
                        j = 5;
                        cornerPoint = false;
                    }
                }
            }
            if (cornerPoint)
                cornerPoints.push_back({x + 3, y + 3});
        }
    }
}

std::vector<_Point> cornerHarris(Image &grayImg, double threshold, int patchDim) {
    double det, trace, strength;
    float xFilter[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    float yFilter[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
    float kernel[9 * 9] = {0};
    gaussianGeneration(kernel, 9, 0.5, 1);
    std::vector<_Point> cornerPoints;
    Image bluredImg = gaussianFilter(grayImg);
    Image Ix = applyFilter(bluredImg, xFilter, 3);
    Image Iy = applyFilter(bluredImg, yFilter, 3);
    Image pointsStrength{Ix.width - (patchDim - 1), Ix.height - (patchDim - 1), 1};
    int patchOffset = (patchDim - 1) / 2;
    for (int y = patchOffset; y < Ix.height - patchOffset; ++y) {
        for (int x = patchOffset; x < Ix.width - patchOffset; ++x) {
            double Ixp2 = 0;
            double Ixyp = 0;
            double Iyp2 = 0;
            for (int i = -patchOffset; i < (patchOffset + 1); ++i) {
                for (int j = -patchOffset; j < (patchOffset + 1); ++j) {
                    Ixp2 += Ix(y + i, x + j) * Ix(y + i, x + j) * kernel[(j + 5) + ((i + 5) * 9)];
                    Iyp2 += Iy(y + i, x + j) * Iy(y + i, x + j) * kernel[(j + 5) + ((i + 5) * 9)];
                    Ixyp += Ix(y + i, x + j) * Iy(y + i, x + j) * kernel[(j + 5) + ((i + 5) * 9)];
                }
            }
            det = (Ixp2 * Iyp2) - pow(Ixyp, 2);
            trace = Ixp2 + Iyp2;
            strength = det - (0.1 * trace * trace);
            pointsStrength(y - patchOffset, x - patchOffset) = strength;
        }
    }
    double max = imageMax(pointsStrength);
    std::cout << max;
    getHarrisCorner(pointsStrength, cornerPoints, max * threshold);
    return cornerPoints;
}

void drawCornerPoints(Image &img, std::vector<_Point> &cornerPoints) {
    int x, y;
    for (int pointNum = 0; pointNum < cornerPoints.size(); ++pointNum) {
        x = cornerPoints[pointNum].x;
        y = cornerPoints[pointNum].y;
        for (int j = -2; j < 3; ++j) {
            for (int i = -2; i < 3; ++i) {
                if (x != 0 && y != 0) {
                    img(y + j, x + i, 1) = 255;
                }
            }
        }
    }
}

void featureHistogram(Image &dir, Image &magnitude, double mainOrientation, std::pair<int, int> iRange,
                      std::pair<int, int> jRange,
                      std::vector<double> &v) {
    double hist[8] = {0};
    double step = 180 / 4;
    int index = 0;
    double absoluteDir = 0, relativeDir = 0;
    for (int i = iRange.first; i < iRange.second; ++i) {
        for (int j = jRange.first; j < jRange.second; ++j) {
            absoluteDir = dir(i, j);
            relativeDir = absoluteDir >= mainOrientation ? absoluteDir - mainOrientation : 360 - (mainOrientation -absoluteDir);
            index = relativeDir / step;
            hist[index] += 1 * magnitude(i, j);
        }
    }
    for (double &i : hist) {
        v.push_back(i);
    }
}

void normalize(std::vector<double> &vector) {
    double sumSquare = 0;
    for (auto &val: vector) {
        sumSquare += val * val;
    }
    sumSquare = sqrt(sumSquare);
    if (sumSquare != 0) {
        for (auto &val: vector) {
            val = val / sumSquare;
        }
    }

}

std::vector<double>
getMainOrientation(Image &dir, Image &magnitude, std::pair<int, int> iRange, std::pair<int, int> jRange) {
    std::vector<double> orientations;
    double orientationHistogram[36] = {0};
    float step = 360 / 36;
    int index = 0;
    for (int i = iRange.first; i < iRange.second; ++i) {
        for (int j = jRange.first; j < jRange.second; ++j) {
            index = dir(i, j) / step;
            orientationHistogram[index] += 1 * magnitude(i, j);
        }
    }
    double max = 0;
    for (double &val: orientationHistogram) {
        if (val > max) {
            max = val;
        }
    }
    for (int i = 0; i < 36; i++) {
        if (orientationHistogram[i] > 0.8 * max) {
            orientations.push_back(i * step);
        }
    }
    return orientations;
}

std::vector<std::pair<std::vector<double>, _Point>> getSIFTDescriptor(Image &inputImg, float threshold) {
    std::vector<std::pair<std::vector<double>, _Point>> features;
    auto cornerPoints = cornerHarris(inputImg, threshold);
    float xFilter[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    float yFilter[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
    float kernel16[16 * 16] = {0};
    float kernel5[5 * 5] = {0};
    gaussianGeneration(kernel16, 16, 1.5, 0);
    gaussianGeneration(kernel5, 5, 1.5, 0);
    Image Ix = applyFilter(inputImg, xFilter, 3);
    Image Iy = applyFilter(inputImg, yFilter, 3);
    auto magnitude = getMagnitude(Ix, Iy);
    auto directions = getDirection(Ix, Iy, false);
    for (auto &point: cornerPoints) {
        if (!(point.x - 8 < 0 || point.x + 7 > magnitude.width || point.y - 8 < 0 || point.y + 7 > magnitude.height)) {
            // multiply by Gaussian
            for (int i = -2; i < 3; i++) {
                for (int j = -2; j < 3; j++) {
                    magnitude(point.y + i, point.x + j) =
                            magnitude(point.y + i, point.x + j) * kernel5[(j + 2) + (i + 2) * 5];
                }
            }
            auto mainOrientations = getMainOrientation(directions, magnitude, {point.y - 2, point.y + 3},
                                                       {point.x - 2, point.x + 3});
            // remove guessing multiplication
            for (int i = -2; i < 3; i++) {
                for (int j = -2; j < 3; j++) {
                    magnitude(point.y + i, point.x + j) =
                            magnitude(point.y + i, point.x + j) / kernel5[(j + 2) + (i + 2) * 5];
                }
            }
            for (auto &mainOrientation: mainOrientations) {
                printf("mainOrientation: %f, _Point(%i, %i)\n", mainOrientation, point.x, point.y);
                features.emplace_back(std::vector<double>(), point);
                // multiply by Gaussian
                for (int i = -8; i < 8; i++) {
                    for (int j = -8; j < 8; j++) {
                        magnitude(point.y + i, point.x + j) =
                                magnitude(point.y + i, point.x + j) * kernel16[(j + 8) + (i + 8) * 16];
                    }
                }
                for (int i = -8; i < 8; i += 4) {
                    for (int j = -8; j < 8; j += 4) {
                        featureHistogram(directions, magnitude, mainOrientation, {point.y + i, point.y + i + 4},
                                         {point.x + j, point.x + j + 4},
                                         features.back().first);
                    }
                }
                // remove guessing multiplication
                for (int i = -8; i < 8; i++) {
                    for (int j = -8; j < 8; j++) {
                        magnitude(point.y + i, point.x + j) =
                                magnitude(point.y + i, point.x + j) / kernel16[(j + 8) + (i + 8) * 16];
                    }
                }
            }
        }
    }
    for (auto &featureV: features) {
        normalize(featureV.first);
    }
    for (auto &featureV: features) {
        for (double &val : featureV.first) {
            if (val > 0.2) {
                val = 0;
            }
        }
    }
    for (auto &featureV: features) {
        normalize(featureV.first);
    }
    return features;
}