/**
 * Created by abdallah drwesh on 6/4/21.
 */

#ifndef CV_FACE_DETECTION_OPENCV_H
#define CV_FACE_DETECTION_OPENCV_H
#include "Image.h"
#include <opencv2/objdetect/objdetect.hpp>

Image detectFace(Image& inputImage);

Image drawFaces(cv::Mat img, std::vector<cv::Rect> faces);

Image cropImageToTheDetectedFace(cv::Mat img, cv::Rect face);

#endif //CV_FACE_DETECTION_OPENCV_H
