//
// Created by mohamedkamal on ٤‏/٦‏/٢٠٢١.
//

#ifndef CV_ROC_H
#define CV_ROC_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <math.h>

class ROC {
public:

    ROC(std::string, int);
    ROC(std::vector<std::pair<std::vector<float>,int>>  data, int);
    ~ROC();

    std::vector<std::vector<float>> TP; //roc true positives
    std::vector<std::vector<float>> FP; //roc false positive

    void writeToFile(std::string);
    std::vector<float> getAreaUnderCurve(void);

private:
    std::vector<std::pair<std::vector<float>,int>>  data;
    int classes;
    void getROCFromData(std::vector<std::pair<std::vector<float>,int>>);
};

#endif //CV_ROC_H
