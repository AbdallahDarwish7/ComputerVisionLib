//
// Created by mohamedkamal on ١٥‏/٥‏/٢٠٢١.
//

#ifndef CV_K_MEAN_SEGMENTATIN_H
#define CV_K_MEAN_SEGMENTATIN_H
#include "vector"
#include "Image.h"
using namespace std;
class K_mean {
    int k;
    int maxIterations;
    Image * clusters;
    vector<vector<float>> centroids;
    Image * image;

    void clusterPixels();
    int closestCluster(int, int);
    void getClustersCentroid();
    bool centroidsChanged(vector<vector<float>>);
    float distance(int, int, vector<float>);
public:
    void run();
    K_mean();
    K_mean(Image *, int, int);
    Image getOutput();
    ~K_mean();
};
#endif //CV_K_MEAN_SEGMENTATIN_H
