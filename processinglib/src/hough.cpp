//
// Created by abdallah on 4/15/21.
//

#include "hough.h"
#include <algorithm>
#include <cmath>
#include <map>
#include "utilities.h"

HoughLineTransformData houghLineTransform(Image &bw, float thetaStep, float rohStep) {
    int width = (89 + 90) / thetaStep + 1;
    float rohMax = std::sqrt(bw.width * bw.width + bw.height * bw.height);
    int halfHeight = rohMax / rohStep;
    Image houghImg{width, halfHeight * 2, 1};
    float roh = 0, theta;
    std::vector<double> thetaV;
    thetaV.reserve(width);
    std::vector<double> rohV;
    rohV.reserve(halfHeight * 2);
    for (int iTheta = 0; iTheta < houghImg.width; ++iTheta) {
        theta = (-90 + iTheta * thetaStep) * M_PI / 180;
        thetaV.push_back(theta);
    }
    for (int iR = 0; iR < houghImg.height; ++iR) {
        rohV.push_back((iR - halfHeight) * rohStep);
    }
    for (int i = 0; i < bw.height; ++i) {
        for (int j = 0; j < bw.width; ++j) {
            if ((int) bw(i, j) == 255) {
                for (int iTheta = 0; iTheta < houghImg.width; ++iTheta) {
                    roh = j * cos(thetaV[iTheta]) + i * sin(thetaV[iTheta]);
                    houghImg(halfHeight + roh / rohStep, iTheta) += 1;
                }
            }
        }
    }
    return HoughLineTransformData(houghImg, thetaV, rohV);
}

std::vector<_Point> linePeaks(Image &houghImage, int peaksNum) {
    std::vector<std::pair<int, _Point>> accs(houghImage.width * houghImage.height);
    std::vector<_Point> peaks;
    for (int i = 0; i < houghImage.height; ++i) {
        for (int j = 0; j < houghImage.width; ++j) {
            accs[j + i * houghImage.width].first = houghImage(i, j);
            accs[j + i * houghImage.width].second.y = i;
            accs[j + i * houghImage.width].second.x = j;
        }
    }
    std::sort(accs.begin(), accs.end(), [](std::pair<int, _Point> &p1, std::pair<int, _Point> &p2) {
        return p1.first < p2.first;
    });
    for (int i = accs.size(); i > accs.size() - peaksNum; --i) {
        peaks.push_back(accs[i - 1].second);
    }
    return peaks;
}


void partialLine(Image &bw, std::vector<std::vector<_Point>> &fullLines,
                 std::vector<std::vector<_Point>> &partialLines, int gap) {
    int i = 0;
    bool lastPush = false;
    int gapLength = 0;
    int lastPushIndex = 0;
    while (i < fullLines.back().size()) {
        auto &point = fullLines.back()[i];
        if (bw(point.y, point.x) == 255) {
            if (!lastPush && gapLength < gap) {
                for (int j = lastPushIndex; j < lastPushIndex + gapLength; ++j) {
                    if (j < fullLines.back().size())
                        partialLines.back().push_back(fullLines.back()[j]);
                }
            }
            partialLines.back().push_back(point);
            lastPush = true;
            gapLength = 0;

        } else {
            if (lastPush) {
                lastPushIndex = i;
            }
            gapLength++;
            lastPush = false;
        }
        i++;
    }
}


std::vector<std::vector<_Point>>
houghLines(Image &bw, std::vector<_Point> &peaks, std::vector<double> &thetaV, std::vector<double> &rohV,
           int maxGap) {
    double theta = 0, roh = 0;
    std::vector<std::vector<_Point>> fullLines;
    std::vector<std::vector<_Point>> partialLines;
    for (auto &peak: peaks) {
        theta = thetaV[peak.x];
        roh = rohV[peak.y];
        fullLines.emplace_back(std::vector<_Point>{});
        for (int i = 0; i < bw.height; ++i) {
            for (int j = 0; j < bw.width; ++j) {
                if ((int) roh == (int) ((j) * cos(theta) + (i) * sin(theta))) {
                    fullLines.back().emplace_back(j, i);
                }
            }
        }
        partialLines.emplace_back(std::vector<_Point>{});
        partialLine(bw, fullLines, partialLines, maxGap);
    }

    return partialLines;
}


Image drawLines(std::vector<std::vector<_Point>> &lines, Image &origin, Image &bw) {
    int iInit = (origin.height - bw.height) / 2;
    int jInit = (origin.width - bw.width) / 2;
    Image out{origin.width, origin.height, 3};
    for (int i = iInit; i < origin.height - iInit; ++i) {
        for (int j = jInit; j < origin.width - jInit; ++j) {
            for (int k = 0; k < out.channels; ++k) {
                out(i, j, k) = origin(i, j, origin.channels > 1 ? k : 0) / 2;
            }
        }
    }
    for (auto &line: lines) {
        for (auto &point: line) {
            out(point.y + iInit, point.x + jInit, 0) = 255;
            out(point.y + iInit, point.x + jInit, 1) = 0;
            out(point.y + iInit, point.x + jInit, 2) = 0;
        }
    }
    return out;
}


int accumulate(int yCenter, int xCenter, int roh, Image &bw, std::vector<std::pair<int, Circle>> &accV) {
    int iStart = yCenter - roh, iEnd = yCenter + roh + 1;
    int jStart = xCenter - roh, jEnd = xCenter + roh + 1;
    if (iStart < 0) {
        iStart = 0;
    }
    if (iEnd > (bw.height - 1)) {
        iEnd = bw.height - 1;
    }
    if (jStart < 0) {
        jStart = 0;
    }
    if (jEnd > (bw.width - 1)) {
        jEnd = bw.width - 1;
    }
    for (int i = iStart; i < iEnd; ++i) {
        for (int j = jStart; j < jEnd; ++j) {
            if ((roh * roh) > ((j - xCenter) * (xCenter - j) + (yCenter - i) * (yCenter - i)) &&
                ((roh-1) * (roh-1)) < ((xCenter - j) * (xCenter - j) + (yCenter - i) * (yCenter - i))) {
                accV[j + i * bw.width].first++;
                accV[j + i * bw.width].second.radius = roh;
                accV[j + i * bw.width].second.xCenter = j;
                accV[j + i * bw.width].second.yCenter = i;
            }
        }
    }
    return 0;
}


std::vector<Circle> houghCircles(Image &bw, std::pair<int, int> rohRange, int threshold) {
    std::vector<Circle> filteredCircles;
    for (int iRoh = rohRange.first; iRoh < (rohRange.second + 1); ++iRoh) {
        std::vector<std::pair<int, Circle>> acc(bw.width * bw.height);
        for (int i = 0; i < bw.height; ++i) {
            for (int j = 0; j < bw.width; ++j) {
                if (bw(i, j) == 255) {
                    accumulate(i, j, iRoh, bw, acc);
                }
            }
        }
        for (auto &a: acc) {
            if (a.first > threshold) {
                filteredCircles.push_back(a.second);
            }
        }
    }
    return filterCircles(bw, filteredCircles);
}

Circle getDifferentCircle(std::vector<Circle> &circles, Circle &c) {
    std::vector<Circle> circleD;
    for (int i = 0; i < circles.size(); ++i) {
        if (std::abs((c.xCenter - circles[i].xCenter)) < 15 && std::abs((c.yCenter - circles[i].yCenter)) < 15) {
            circleD.push_back(circles[i]);
        }
    }
    if (circleD.empty())
        return c;
    return circleD[circleD.size() / 2];
}

std::vector<Circle> filterCircles(Image &bw, std::vector<Circle> &circles) {
    int roh, x, y;
    std::vector<Circle> filteredCircles1;
    std::vector<Circle> filteredCircles2;
    for (auto &circle: circles) {
        roh = circle.radius;
        x = circle.xCenter;
        y = circle.yCenter;
        Circle c{roh, x, y};
        int acc = 0;
        int iStart = y - roh < 0 ? 0 : y - roh - 20;
        int iEnd = y + roh > bw.height ? bw.height : y + roh + 1 + 20;
        int jStart = x - roh < 0 ? 0 : x - roh - 20;
        int jEnd = x + roh > bw.width ? bw.width : x + roh + 1 + 20;
        for (int i = iStart; i < iEnd; ++i) {
            for (int j = jStart; j < jEnd; ++j) {
                if (((int) bw(i, j)) == 255) {
                    if (((roh + 2.5) * (roh + 2.5)) >= ((j - x) * (j - x) + (i - y) * (i - y)) &&
                        ((roh - 2.5) * (roh - 2.5)) <= ((j - x) * (j - x) + (i - y) * (i - y))) {
                        acc++;
                    }
                }
            }
        }
        if (acc > (5 * roh)) {
//            if (isDifferent(c, filteredCircles1))
            filteredCircles1.push_back(c);
        }
    }

    for (auto &c: filteredCircles1) {
        filteredCircles2.push_back(getDifferentCircle(filteredCircles1, c));
    }
    return filteredCircles2;
}


void accumulate(Image *&rImage, int yCenter, int xCenter, int rohIndex, int roh) {
    int iStart = yCenter - roh, iEnd = yCenter + roh + 1;
    int jStart = xCenter - roh, jEnd = xCenter + roh + 1;
    if (iStart < 0) {
        iStart = 0;
    }
    if (iEnd > (rImage->height - 1)) {
        iEnd = rImage->height - 1;
    }
    if (jStart < 0) {
        jStart = 0;
    }
    if (jEnd > (rImage->width - 1)) {
        jEnd = rImage->width - 1;
    }
    for (int i = iStart; i < iEnd; ++i) {
        for (int j = jStart; j < jEnd; ++j) {
            if ((roh * roh) == ((xCenter - j) * (xCenter - j) + (yCenter - i) * (yCenter - i))) {
                (*rImage)(i, j, rohIndex)++;
            }
        }
    }
}

Image *houghCircleTransform(Image &bw, std::pair<int, int> rohRange) {
    Image *houghImage = new Image(bw.width, bw.height, rohRange.second - rohRange.first + 1);
    for (int iRoh = rohRange.first; iRoh < (rohRange.second + 1); ++iRoh) {
        for (int i = 0; i < bw.height; ++i) {
            for (int j = 0; j < bw.width; ++j) {
                if (bw(i, j) == 255) {
                    accumulate(houghImage, i, j, iRoh - rohRange.first, iRoh);
                }
            }
        }
    }
    return houghImage;
}


std::vector<std::vector<int>> circlePeaks(Image *&houghImage, int peaksNum) {
    std::vector<std::vector<std::pair<int, std::vector<int>>>> accs(houghImage->channels);
    std::vector<std::vector<int>> peaks;
    for (int i = 0; i < houghImage->height; ++i) {
        for (int j = 0; j < houghImage->width; ++j) {
            for (int k = 0; k < houghImage->channels; ++k) {
                accs[k].push_back(std::pair<int, std::vector<int>>((*houghImage)(i, j, k), std::vector<int>{}));
                auto &v = accs[k].back().second;
                v.push_back(i);
                v.push_back(j);
                v.push_back(k);
            }
        }
    }
    for (auto &v: accs) {
        std::sort(v.begin(), v.end());
    }
    for (auto &v: accs) {
        for (int i = v.size(); i > v.size() - peaksNum; --i) {
            peaks.push_back(v[i - 1].second);
        }
    }
    return peaks;
}

std::vector<Circle>
houghCircles(Image &origin, Image &bw, std::vector<std::vector<int>> peaks, std::pair<int, int> rohRange) {
    int minRoh = rohRange.first;
    int roh, x, y;
    std::map<Circle, int> map;
    std::vector<Circle> circles;
    for (auto &peak: peaks) {
        roh = peak[2] + minRoh;
        x = peak[1];
        y = peak[0];
        Circle c{roh, x, y};
        int acc = 0;
        int iStart = y - roh < 0 ? 0 : y - roh - 20;
        int iEnd = y + roh > bw.height ? bw.height : y + roh + 1 + 20;
        int jStart = x - roh < 0 ? 0 : x - roh - 20;
        int jEnd = x + roh > bw.width ? bw.width : x + roh + 1 + 20;
        for (int i = iStart; i < iEnd; ++i) {
            for (int j = jStart; j < jEnd; ++j) {
                if (((int) bw(i, j)) == 255) {
                    if (((roh + 2) * (roh + 2)) > ((j - x) * (j - x) + (i - y) * (i - y)) &&
                        ((roh - 2) * (roh - 2)) < ((j - x) * (j - x) + (i - y) * (i - y))) {
                        acc++;
                    }
                }
            }
        }
        if (acc > (3 * roh))
            circles.push_back(c);
    }
    return circles;
}


Image drawCircles(std::vector<Circle> circles, Image &origin, Image &bw) {
    int iInit = (origin.height - bw.height) / 2;
    int jInit = (origin.width - bw.width) / 2;
    Image out{origin.width, origin.height, 3};
    for (int i = iInit; i < origin.height - iInit; ++i) {
        for (int j = jInit; j < origin.width - jInit; ++j) {
            for (int k = 0; k < out.channels; ++k) {
                out(i, j, k) = origin(i, j, origin.channels > 1 ? k : 0) / 2;
            }
        }
    }
    int r, x, y;
    for (auto &circle: circles) {
        r = circle.radius;
        x = circle.xCenter;
        y = circle.yCenter;
        for (int i = 0; i < bw.height; ++i) {
            for (int j = 0; j < bw.width; ++j) {
                if ((r + 1) * (r + 1) > ((j - x) * (j - x) + (i - y) * (i - y)) &&
                    ((r - 1) * (r - 1)) < ((j - x) * (j - x) + (i - y) * (i - y))) {
                    out(i + iInit, j + jInit, 0) = 255;
                    out(i + iInit, j + jInit, 1) = 0;
                    out(i + iInit, j + jInit, 1) = 0;
                }
            }
        }
    }
    return out;
}