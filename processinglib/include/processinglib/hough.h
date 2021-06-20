//
// Created by abdallah on 4/15/21.
//

#ifndef CV_HOUGH_H
#define CV_HOUGH_H

#include "Image.h"
#include <utility>
#include <vector>

struct HoughLineTransformData {
    Image houghImage;
    std::vector<double> theta;
    std::vector<double> roh;

    HoughLineTransformData(const Image &houghImage, std::vector<double> theta, std::vector<double> roh) : houghImage(
            houghImage),
                                                                                                          theta(std::move(
                                                                                                                  theta)),
                                                                                                          roh(std::move(
                                                                                                                  roh)) {}
};

struct _Point {
    int x, y;

    _Point() {}
    _Point(const _Point & point): _Point(point.x, point.y){}
    _Point(int x, int y) : x{x}, y{y} {}
};


struct Circle {
    int radius;
    int xCenter, yCenter;

    Circle() {}

    Circle(int radius, int xCenter, int yCenter) : radius{radius}, xCenter{xCenter}, yCenter{yCenter} {}


    bool operator<(const Circle &circle) {
        return radius < circle.radius;
    }

    bool operator<(const Circle &circle) const {
        return radius < circle.radius;
    }
};

HoughLineTransformData houghLineTransform(Image &bw, float thetaStep, float rohStep);


std::vector<_Point> linePeaks(Image &houghImage, int peaksNum = 1);


std::vector<std::vector<_Point>>
houghLines(Image &bw, std::vector<_Point> &peaks, std::vector<double> &thetaV, std::vector<double> &rohV,
           int maxGap);

Image drawLines(std::vector<std::vector<_Point>> &lines, Image &origin, Image &bw);

Image *houghCircleTransform(Image &bw, std::pair<int, int> rohRange);

std::vector<Circle> houghCircles(Image &bw, std::pair<int, int> rohRange, int threshold);

std::vector<std::vector<int>> circlePeaks(Image *&houghImage, int peaksNum = 1);

std::vector<Circle>
houghCircles(Image &origin, Image &bw, std::vector<std::vector<int>> peaks, std::pair<int, int> rohRange);

std::vector<Circle> filterCircles(Image &bw, std::vector<Circle> &circles);

Image drawCircles(std::vector<Circle> circles, Image &origin, Image &bw);

#endif //CV_HOUGH_H
