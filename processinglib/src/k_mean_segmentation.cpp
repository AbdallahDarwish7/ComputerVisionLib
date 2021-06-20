//
// Created by mohamedkamal on ١٥‏/٥‏/٢٠٢١.
//
#include "k_mean_segmentation.h"
#include "vector"
#include <random>
#include "iostream"

using namespace std;

K_mean::K_mean() {}

K_mean::K_mean(Image *image, int k, int maxIteration) {

    if(image->channels == 3) {
        Image temp = image->RGB2LUV();
        this->image = new Image(&temp);
    } else{
        this->image = new Image(image);
    }

    this->k = k;
    this->clusters = new Image(this->image->width, this->image->height, 1);

    this->maxIterations = maxIteration;
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<std::mt19937::result_type> distForI(0, (this->image->height - 1));
    uniform_int_distribution<std::mt19937::result_type> distForJ(0, (this->image->width - 1));
    for (int loop = 0; loop < k; loop++) {

        int i = (int) distForI(rng);
        int j = (int) distForJ(rng);

        vector<float> temp;
        for (int k_ = 0; k_ < this->image->channels; k_++) {
            temp.push_back((*(this->image))(i, j, k_));
        }
        this->centroids.push_back(temp);
    }

}

void K_mean::clusterPixels() {

    for (int i = 0; i < this->image->height; ++i) {
        for (int j = 0; j < this->image->width; ++j) {
            int closestCluster = this->closestCluster(i, j);
            (*(this->clusters))(i, j) = closestCluster;
        }
    }
}

int K_mean::closestCluster(int i, int j) {
    float closestDistance = 9000;
    int closestClusterIdnex = -1;
    for (int loop = 0; loop < this->k; loop++) {
        float distance_ = distance(i, j, this->centroids[loop]);
        if (distance_ < closestDistance) {
            closestDistance = distance_;
            closestClusterIdnex = loop;
        }
    }
    return closestClusterIdnex;
}

float K_mean::distance(int i, int j, vector<float> centroid) {
    float sum = 0;
    for (int k = 0; k < this->image->channels; k++) {
        float difference = (*(this->image))(i, j, k) - centroid[k];
        sum += (difference * difference);
    }
    float result = sqrt(sum);
    return result;
}

void K_mean::getClustersCentroid() {

    vector<vector<double>> sum = vector<vector<double>>(this->k);
    for (int loop = 0; loop < this->k; loop++) {
        sum[loop] = vector<double>(this->image->channels, 0.0);
    }
    vector<int> count = vector<int>(this->k);
    for (int i = 0; i < this->image->height; ++i) {
        for (int j = 0; j < this->image->width; ++j) {
            int cluster = (*(this->clusters))(i, j);
            for (int k = 0; k < this->image->channels; k++) {
                sum[cluster][k] += (*(this->image))(i, j, k);
            }
            count[cluster] += 1;
        }
    }
    for (int loop = 0; loop < this->k; loop++) {
        for (int k = 0; k < this->image->channels; k++) {
            centroids[loop][k] = (sum[loop][k] / (double) count[loop]);
        }
    }

}

bool K_mean::centroidsChanged(vector<vector<float>> oldCentroids) {

    float sum = 0;
    for (int loop = 0; loop < this->k; loop++) {
        for (int k = 0; k < this->image->channels; k++) {
            float temp = oldCentroids[loop][k] - this->centroids[loop][k];
            if (temp < 0.0) {
                sum += (-temp);
            } else {
                sum += temp;
            }
        }
    }
    if (sum == 0.0) {
        return false;
    } else {
        return true;
    }
}

void K_mean::run() {
    for (int loop = 0; loop < this->maxIterations; loop++) {
        this->clusterPixels();
        vector<vector<float>> oldCentroids = vector<vector<float>>(this->k);
        for (int loop1 = 0; loop1 < this->k; loop1++) {
            oldCentroids[loop1] = vector<float>(this->image->channels);
            for (int loop2 = 0; loop2 < this->image->channels; loop2++) {
                oldCentroids[loop1][loop2] = this->centroids[loop1][loop2];
            }
        }
        this->getClustersCentroid();
        bool changed = this->centroidsChanged(oldCentroids);
        if (changed == false) {
            loop = this->maxIterations;
        }
    }
}

Image K_mean::getOutput() {
    Image output = Image(this->image->width, this->image->height, this->image->channels);
    this->run();
    for (int i = 0; i < this->image->height; i++) {
        for (int j = 0; j < this->image->width; j++) {
            int cluster = (*(this->clusters))(i, j);
            for (int k = 0; k < this->image->channels; k++) {
                output(i, j, k) = centroids[cluster][k];
            }
        }
    }
    return output.LUV2RGB();
}

K_mean::~K_mean() {
    delete image;
    delete clusters;
}

