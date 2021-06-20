//
// Created by abdallah drwesh on 4/13/21.
//

#include "canny.h"
#include "utilities.h"
#include "histogram.h"
#include "filters.h"

float median(Image &image) {
    int histogram[256];
    im_hist(image, histogram, 1);
    int medianVal = 0;
    int size = image.width * image.height;
    int halfIndex = size / 2;
    for (int i = 0; i < 256; ++i) {
        medianVal += histogram[i];
        if (medianVal >= halfIndex) {
            medianVal = i;
            break;
        }
    }
    return medianVal;
}

Image cannyEdgeDetector(Image &image, float sigma, float thHigh, float thLow) {
    float xFilter[9] = {
            -1, 0, 1,
            -2, 0, 2,
            -1, 0, 1};
    float yFilter[9] = {
            1, 2, 1,
            0, 0, 0,
            -1, -2, -1};
    Image outputImg = gaussianFilter(image, 7, 0, sigma);
    Image imgX = applyFilter(outputImg, xFilter, 3);
    Image imgY = applyFilter(outputImg, yFilter, 3);
    Image magnitude = getMagnitude(imgX, imgY);
    Image direction = getDirection(imgX, imgY);
    Image nonMax = cannyNonMaxSuppression(magnitude, direction);
//    float medianVal = median(image);
    return cannyEdgeLink(255. * thHigh, 255 * thLow, nonMax);
}

void dirToCoordinates(float dir, char coordinates[]) {
    switch ((int) dir) {
        case 0:
            coordinates[0] = 0, coordinates[1] = 1;
            break;
        case 45:
            coordinates[0] = -1, coordinates[1] = 1;
            break;
        case 90:
            coordinates[0] = -1, coordinates[1] = 0;
            break;
        case 135:
            coordinates[0] = -1, coordinates[1] = -1;
            break;
    }
}

Image cannyNonMaxSuppression(Image &mag, Image &dir) {
    char i_jCoornidates[2] = {0};
    int postive_i = 0, negative_i = 0;
    int postive_j = 0, negative_j = 0;
    Image outImage{mag.width - 2, mag.height - 2, 1};
    for (int i = 1; i < mag.height - 1; ++i) {
        for (int j = 1; j < mag.width - 1; ++j) {
            outImage(i - 1, j - 1) = mag(i, j);
        }
    }
    for (int i = 1; i < mag.height - 1; ++i) {
        for (int j = 1; j < mag.width - 1; ++j) {
            dirToCoordinates(dir(i, j), i_jCoornidates);
            postive_i = i + i_jCoornidates[0];
            negative_i = i - i_jCoornidates[0];
            postive_j = j + i_jCoornidates[1];
            negative_j = j - i_jCoornidates[1];
            if (outImage(i - 1, j - 1) < mag(postive_i, postive_j) ||
                outImage(i - 1, j - 1) < mag(negative_i, negative_j)) {
                outImage(i - 1, j - 1) = 0;
            }
        }
    }
    return outImage;
}

Image cannyEdgeLink(int tHigh, int tLow, Image &image) {
    Image outImage{image.width - 2, image.height - 2, 1};
    for (int i = 1; i < image.height - 1; ++i) {
        for (int j = 1; j < image.width - 1; ++j) {
            if (image(i, j) > tHigh) {
                outImage(i - 1, j - 1) = 255;
            } else if (image(i, j) < tLow) {
                outImage(i - 1, j - 1) = 0;
            } else {
                int k = -1;
                while (k < 2) {
                    if (image(i + k, j) > tLow ||
                        image(i + k, j - 1) > tLow ||
                        image(i + k, j + 1) > tLow) {
                        break;
                    }
                    k++;
                }
                if (k < 2) {
                    outImage(i - 1, j - 1) = 255;
                } else {
                    outImage(i - 1, j - 1) = 0;
                }
            }
        }
    }
    return outImage;
}