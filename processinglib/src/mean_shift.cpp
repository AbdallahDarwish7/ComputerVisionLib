//
// Created by mohamedkamal on ١٥‏/٥‏/٢٠٢١.
//
#define MS_MAX_NUM_CONVERGENCE_STEPS	5										// up to 10 steps are for convergence
#define MS_MEAN_SHIFT_TOL_COLOR			0.3
#include "mean_shift.h"
#include "vector"
#include <random>
#include "iostream"

using namespace std;

MeanShift::MeanShift() {}

MeanShift::MeanShift(Image *image, float hs, float hr) {
    this->hs = hs;
    this->hr = hr;
    if(image->channels == 3) {
        Image temp = image->RGB2LUV();
        this->image = new Image(&temp);
    }else {
        this->image = new Image(image);
    }
}


float MeanShift::distance(int i1, int j1, int i2, int j2) {

    float sum = 0;
    for (int channel = 0; channel < this->image->channels; channel++) {
        float difference = (*(this->image))(i1, j1, channel) - (*(this->image))(i2, j2, channel);
        sum += (difference * difference);
    }
    float result = sqrt(sum);
    return result;
}

float MeanShift::distance(vector<float> pt1, vector<float> pt2) {

    float sum = 0;
    for (int channel = 0; channel < this->image->channels; channel++) {
        float difference = (pt1[channel] - pt2[channel]);
        sum += (difference * difference);
    }
    float result = sqrt(sum);
    return result;
}

Image MeanShift::run() {

    Image output(this->image);
    vector<float> currentPint = vector<float>(this->image->channels, 0.0);
    vector<float> prevPoint = vector<float>(this->image->channels, 0.0);
    int NumPts = 0;
    int step = 0;

    for (int i = 0; i < this->image->height; i++) {
        for (int j = 0; j < this->image->width; j++) {
            int Left = (j - hs) > 0 ? (j - hs) : 0;
            int Right = (j + hs) < this->image->width ? (j + hs) : this->image->width;
            int Top = (i - hs) > 0 ? (i - hs) : 0;
            int Bottom = (i + hs) < this->image->height ? (i + hs) : this->image->height;
            for (int channel = 0; channel < this->image->channels; channel++) {
                currentPint[channel] = (*(this->image))(i, j, channel);
            }
            step = 0;
            vector<float> sum = vector<float>(this->image->channels, 0.0);

            do {
                for (int channel = 0; channel < this->image->channels; channel++) {
                    prevPoint[channel] = currentPint[channel];
                }
                NumPts = 0;
                for (int hx = Left; hx < Right; hx++) {
                    for (int hy = Top; hy < Bottom; hy++) {

                        float distance = this->distance(i, j, hy, hx);
                        if (distance < hr) {
                            for (int channel = 0; channel < this->image->channels; channel++) {
                                sum[channel] = sum[channel] + (*(this->image))(hy, hx, channel);
                            }
                            NumPts++;
                        }
                    }
                }
                for (int channel = 0; channel < this->image->channels; channel++) {
                    if(NumPts == 0){
                        cout<< "hellp" << endl;
                    }
                    sum[channel] = sum[channel] / ((float) NumPts );
                    currentPint[channel] = sum[channel];
                }

                step++;
            } while ((this->distance(currentPint, prevPoint) > MS_MEAN_SHIFT_TOL_COLOR) &&
                     (step < MS_MAX_NUM_CONVERGENCE_STEPS));

            for (int channel = 0; channel < this->image->channels; channel++) {
                output(i, j, channel) = currentPint[channel];
            }
        }

    }
    return output.LUV2RGB();
}
MeanShift::~MeanShift()  {
        delete image;
    }



