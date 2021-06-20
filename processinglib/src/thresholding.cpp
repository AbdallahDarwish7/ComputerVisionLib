/**
 * Created by abdallah drwesh on 5/18/21.
 */
#include "thresholding.h"
#include "histogram.h"

static double iterativeThresholding(Image &inputImg, double threshold) {
    int objectCount = 0, backgroundCount = 0;
    double objectMean = 0, backgroundMean = 0;
    for (int i = 0; i < inputImg.height; ++i) {
        for (int j = 0; j < inputImg.width; ++j) {
            if (inputImg(i, j) > threshold) {
                objectMean += inputImg(i, j);
                objectCount += 1;
            } else {
                backgroundMean += inputImg(i, j);
                backgroundCount += 1;
            }
        }
    }
    objectMean /= objectCount;
    backgroundMean /= backgroundCount;
    return ((objectMean + backgroundMean) / 2);
}

static double localIterativeThresholding(Image &inputImg, std::pair<int, int> &yRange, std::pair<int, int> &xRange,
                                         double threshold) {
    int objectCount = 0, backgroundCount = 0;
    double objectMean = 0, backgroundMean = 0;
    for (int i = yRange.first; i < yRange.second; ++i) {
        for (int j = xRange.first; j < xRange.second; ++j) {
            if (inputImg(i, j) > threshold) {
                objectMean += inputImg(i, j);
                objectCount += 1;
            } else {
                backgroundMean += inputImg(i, j);
                backgroundCount += 1;
            }
        }
    }
    objectMean /= objectCount;
    backgroundMean /= backgroundCount;
    return ((objectMean + backgroundMean) / 2);
}

int globalOptimalIterativeThresholding(Image &inputImg) {
    double t1 = 0, t2 = 0;
    double objectMean = 0, backgroundMean = 0;
    backgroundMean = (inputImg(0, 0) + inputImg(0, inputImg.width - 1) + inputImg(inputImg.height - 1, 0) +
                      inputImg(inputImg.height - 1, inputImg.width - 1)) / 4;
    for (int i = 1; i < inputImg.height - 1; ++i) {
        for (int j = 1; j < inputImg.width - 1; ++j) {
            objectMean += inputImg(i, j);
        }
    }
    objectMean /= (inputImg.size() - 4);
    t1 = (backgroundMean + objectMean) / 2;
    t2 = iterativeThresholding(inputImg, t1);
    while (std::abs(t1 - t2) < 0.0001) {
        t1 = iterativeThresholding(inputImg, t2);
        t2 = iterativeThresholding(inputImg, t1);
    }
    return (int) t1;
}

Image localOptimalIterativeThresholding(Image &inputImg, int blockDim) {
    int imgWidth = (inputImg.width / blockDim) * blockDim;
    int imgHeight = (inputImg.height / blockDim) * blockDim;
    Image outImage{imgWidth, imgHeight, inputImg.channels};
    double objectBlockMean = 0, backgroundBlockMean = 0;
    double t1 = 0, t2 = 0;
    for (int i = 0; i < outImage.height - blockDim; i += blockDim) {
        for (int j = 0; j < outImage.width - blockDim; j += blockDim) {
            backgroundBlockMean = (inputImg(i, j) + inputImg(i, j + blockDim - 1) + inputImg(i + blockDim - 1, j) +
                                   inputImg(i + blockDim - 1, j + blockDim - 1)) / 4;
            objectBlockMean = 0;
            for (int blockI = i + 1; blockI < i + blockDim - 1; ++blockI) {
                for (int blockJ = j + 1; blockJ < j + blockDim - 1; ++blockJ) {
                    objectBlockMean += inputImg(i, j);
                }
            }
            objectBlockMean /= (blockDim * blockDim - 4);
            t1 = (backgroundBlockMean + objectBlockMean) / 2;
            std::pair<int, int> yRange{i, i + blockDim};
            std::pair<int, int> xRange{j, j + blockDim};
            t2 = localIterativeThresholding(inputImg, yRange, xRange, t1);
            while (std::abs(t1 - t2) < 0.0001) {
                t1 = localIterativeThresholding(inputImg, yRange, xRange, t2);
                t2 = localIterativeThresholding(inputImg, yRange, xRange, t1);
            }
            segmentImg(inputImg, outImage, yRange, xRange, (int) t1);
        }
    }
    return outImage;
}

static void
getOptimalThresholdsRec(int intThreshold, double normalizedHist[], int histSize, double globalMean, double classMeans[],
                        double classProb[], int currentNumModes, int numClasses,
                        std::vector<int> &thresholds, std::vector<int> &optimalThreshold, double &maxBetweenVar) {
    if (intThreshold < histSize) {
        // base case
        if (currentNumModes == 2) {
            classProb[1] = 0;
            classMeans[1] = 0;
            double tempProb, tempMean, betweenVar;
            for (int i = intThreshold; i < histSize; ++i) {
                tempProb = 1;
                tempMean = globalMean;
                betweenVar = 0;
                thresholds[thresholds.size() - 1] = i;
                classProb[1] += normalizedHist[i];
                classMeans[1] += i * normalizedHist[i];
                if (classProb[1] != 0) {
                    for (int j = numClasses - 1; j > 0; --j) {
                        tempProb -= classProb[j];
                        tempMean -= classMeans[j];
                    }
                    classProb[0] = tempProb;
                    classMeans[0] = tempMean;
                    for (int j = 0; j < numClasses; ++j) {
                        betweenVar += classProb[j] * (classMeans[j] / classProb[j] - globalMean) *
                                      (classMeans[j] / classProb[j] - globalMean);

                    }
                    if (betweenVar > maxBetweenVar) {
                        maxBetweenVar = betweenVar;
                        for (int j = 0; j < thresholds.size(); ++j) {
                            optimalThreshold[j] = thresholds[j];
                        }
                    }
                }
            }
        } else {
            double tempClassProb = 0;
            double tempClassMean = 0;
            for (int i = intThreshold; i < histSize; ++i) {
                thresholds[thresholds.size() + 1 - currentNumModes] = i;
                tempClassProb += normalizedHist[i];
                tempClassMean += i * normalizedHist[i];
                classProb[currentNumModes - 1] = tempClassProb;
                classMeans[currentNumModes - 1] = tempClassMean;
                if (classProb[currentNumModes - 1] != 0)
                    getOptimalThresholdsRec(i + 1, normalizedHist, histSize, globalMean, classMeans, classProb,
                                            currentNumModes - 1, numClasses, thresholds, optimalThreshold,
                                            maxBetweenVar);

            }
        }
    }
}

static void
getOptimalThresholds(double normalizedHist[], int histSize, double globalMean, double classMeans[],
                     double classProb[], int numModes, std::vector<int> &optimalThreshold) {
    std::vector<int> thresholds(numModes - 1);
    int intThreshold = 0;
    double maxBetweenVar = 0;
    getOptimalThresholdsRec(intThreshold, normalizedHist, histSize, globalMean, classMeans, classProb, numModes,
                            numModes, thresholds, optimalThreshold, maxBetweenVar);
}


std::vector<int> globalAtsu(Image &inputImg, int histSize, int numModes) {
    auto hist = new int[histSize];
    auto normalizedHist = new double[histSize];
    int pixelsNo = inputImg.width * inputImg.height;
    double globalMean = 0;
    auto classProbabilities = new double[numModes];
    auto classMeans = new double[numModes];
    std::vector<int> optimalThreshold(numModes - 1);
    for (int i = 0; i < numModes; ++i) {
        classProbabilities[i] = classMeans[i] = 0;
    }
    im_hist(inputImg, hist, 1);
    getNormalizedHist(hist, normalizedHist, histSize, pixelsNo);
    for (int i = 0; i < histSize; i++) {
        globalMean += i * normalizedHist[i];
    }
    getOptimalThresholds(normalizedHist, histSize, globalMean, classMeans, classProbabilities, numModes,
                         optimalThreshold);
    delete[] hist;
    delete[] normalizedHist;
    delete[] classProbabilities;
    delete[] classMeans;
    return optimalThreshold;
}


Image localAtsu(Image &inputImg, int blockDim, int histSize, int numModes) {
    int imgWidth = (inputImg.width / blockDim) * blockDim;
    int imgHeight = (inputImg.height / blockDim) * blockDim;
    Image outImage{imgWidth, imgHeight, inputImg.channels};
    auto hist = new int[histSize];
    auto normalizedHist = new double[histSize];
    double globalBlockMean = 0;
    auto classProbabilities = new double[numModes];
    auto classMeans = new double[numModes];
    std::vector<int> optimalThreshold(numModes - 1);
    for (int i = 0; i < outImage.height - blockDim; i += blockDim) {
        for (int j = 0; j < outImage.width - blockDim; j += blockDim) {
            globalBlockMean = 0;
            for (int i = 0; i < numModes; ++i) {
                classProbabilities[i] = classMeans[i] = 0;
            }
            for (auto &t: optimalThreshold) {
                t = 0;
            }
            std::pair<int, int> yRange{i, i + blockDim};
            std::pair<int, int> xRange{j, j + blockDim};
            im_hist(inputImg, yRange, xRange, hist, 1);
            getNormalizedHist(hist, normalizedHist, histSize, blockDim * blockDim);
            for (int loop = 0; loop < histSize; loop++) {
                globalBlockMean += loop * normalizedHist[loop];
            }
            getOptimalThresholds(normalizedHist, histSize, globalBlockMean, classMeans, classProbabilities, numModes,
                                 optimalThreshold);
            segmentImg(inputImg, outImage, yRange, xRange, optimalThreshold);
        }
    }
    delete[] hist;
    delete[] normalizedHist;
    delete[] classProbabilities;
    delete[] classMeans;
    return outImage;
}


Image buildSegmentedImg(Image &inputImg, std::vector<int> &thresholds) {
    Image segmentedImg{inputImg.width, inputImg.height, inputImg.channels};
    if (thresholds.size() == 1)
        return buildSegmentedImg(inputImg, thresholds[0]);
    int step = 255 / thresholds.size();
    for (int y = 0; y < inputImg.height; y++) {
        for (int x = 0; x < inputImg.width; x++) {
            if (inputImg(y, x) < thresholds[0]) {
                segmentedImg(y, x) = 0;
            } else if (inputImg(y, x) > thresholds[thresholds.size() - 1]) {
                segmentedImg(y, x) = 255;
            } else {
                for (int i = 1; i < thresholds.size(); ++i) {
                    if (inputImg(y, x) >= thresholds[i - 1] && inputImg(y, x) < thresholds[i]) {
                        segmentedImg(y, x) = i * step;
                    }
                }
            }
        }
    }
    return segmentedImg;
}

void segmentImg(Image &inputImg, Image &outputImg, std::pair<int, int> &yRange, std::pair<int, int> &xRange,
                std::vector<int> &thresholds) {
    if (thresholds.size() == 1)
        return segmentImg(inputImg, outputImg, yRange, xRange, thresholds[0]);
    int step = 255 / thresholds.size();
    for (int y = yRange.first; y < yRange.second; y++) {
        for (int x = xRange.first; x < xRange.second; x++) {
            if (inputImg(y, x) < thresholds[0]) {
                outputImg(y, x) = 0;
            } else if (inputImg(y, x) > thresholds[thresholds.size() - 1]) {
                outputImg(y, x) = 255;
            } else {
                for (int i = 1; i < thresholds.size(); ++i) {
                    if (inputImg(y, x) >= thresholds[i - 1] && inputImg(y, x) < thresholds[i]) {
                        outputImg(y, x) = i * step;
                    }
                }
            }
        }
    }
}

Image buildSegmentedImg(Image &colorImage, std::vector<std::vector<int>> thresholds) {
    Image segmentedImg{colorImage.width, colorImage.height, colorImage.channels};
    for (int k = 0; k < colorImage.channels; ++k) {
        int step = 255 / thresholds[k].size();
        for (int y = 0; y < colorImage.height - 1; y++) {
            for (int x = 0; x < colorImage.width - 1; x++) {
                if (colorImage(y, x, k) < thresholds[k][0]) {
                    segmentedImg(y, x, k) = 0;
                } else if (colorImage(y, x, k) > thresholds[k][thresholds[k].size() - 1]) {
                    segmentedImg(y, x, k) = 255;
                } else {
                    for (int i = 1; i < thresholds[k].size(); ++i) {
                        if (colorImage(y, x, k) >= thresholds[k][i - 1] && colorImage(y, x, k) < thresholds[k][i]) {
                            segmentedImg(y, x, k) = i * step;
                        }
                    }
                }
            }
        }
    }
    return segmentedImg;
}

Image buildSegmentedImg(Image &inputImg, int thresholdVal) {
    Image segmentedImg{inputImg.width, inputImg.height, inputImg.channels};
    for (int y = 0; y < inputImg.height; y++) {
        for (int x = 0; x < inputImg.width; x++) {
            if (inputImg(y, x) > thresholdVal) {
                segmentedImg(y, x) = 255;
            } else {
                segmentedImg(y, x) = 0;
            }
        }
    }
    return segmentedImg;
}

void segmentImg(Image &inputImg, Image &outputImg, std::pair<int, int> &yRange, std::pair<int, int> &xRange,
                int thresholdVal) {
    for (int y = yRange.first; y < yRange.second; y++) {
        for (int x = xRange.first; x < xRange.second; x++) {
            if (inputImg(y, x) > thresholdVal) {
                outputImg(y, x) = 255;
            } else {
                outputImg(y, x) = 0;
            }
        }
    }
}
