//
// Created by abdulla167 on ٢١‏/٥‏/٢٠٢١.
//
#include <Image.h>
#include <vector>
#include "point.h"
#include "math.h"

float euclideanDistance(point p1, point p2){
    return sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2));
}

float clustersDistance(std::vector<point> * cluster1, std::vector<point> * cluster2){
    float cluster1_x = 0;
    float cluster1_y = 0;
    int cluster1_size = cluster1->size();
    float cluster2_x = 0;
    float cluster2_y = 0;
    int cluster2_size = cluster2->size();
    for (int i = 0; i < cluster1_size; i++){
        cluster1_x += (*cluster1)[i].x;
        cluster1_y += (*cluster1)[i].y;
    }
    if (cluster1_size > 0){
        cluster1_x = cluster1_x / cluster1_size;
        cluster1_y = cluster1_y / cluster1_size;
    }
    for (int i = 0; i < cluster2_size; i++){
        cluster2_x += (*cluster2)[i].x;
        cluster2_y += (*cluster2)[i].y;
    }
    if (cluster2_size > 0){
        cluster2_x = cluster2_x / cluster2_size;
        cluster2_y = cluster2_y / cluster2_size;
    }
    return euclideanDistance(point(cluster1_x, cluster1_y), point(cluster2_x, cluster2_y));
}

void initialClusters(Image& img, std::vector<std::vector<point>> & clustersList){
    int clusterIntensityRang = 256 / clustersList.size();
    for (int y = 0; y < img.height; y++){
        for (int x = 0; x < img.width; x++){
            int index = img(y, x) / clusterIntensityRang;
            clustersList[index].push_back(point(x, y));
        }
    }
}

int mergeTwoClusters(std::vector<point> & cluster1, std::vector<point> & cluster2){
    int cluster1_size = cluster1.size();
    int cluster2_size = cluster2.size();
    if (cluster1_size > cluster2_size){
        for (int i = 0; i < cluster1_size; ++i) {
            cluster1.push_back(cluster2[i]);
        }
        return 1;
    } else{
        for (int i = 0; i < cluster2_size; ++i) {
            cluster2.push_back(cluster1[i]);
        }
        return 2;
    }
}

Image agglomerativeSeg(Image & img, int numOfClusters, unsigned long initialClustersNum){
    std::vector<std::vector<point>> clustersList{initialClustersNum};
    int intensity = 0;
    initialClusters(img, clustersList);
    Image outputImg{img.width, img.height, img.channels};
    while (numOfClusters < clustersList.size()){
        int cluster1_index = 0;
        int cluster2_index = 0;
        float min = 0;
        for (int i = 0; i < clustersList.size(); i++){
            for (int j = 0; j < i; ++j) {
                float clusterDis = clustersDistance(&clustersList[j], &clustersList[i]);
                if (clusterDis < min){
                    min = clusterDis;
                    cluster1_index = j;
                    cluster2_index = i;
                }
            }
        }
        int mergeResult = mergeTwoClusters(clustersList[cluster1_index], clustersList[cluster2_index]);
        if (mergeResult == 1){
            clustersList.erase(clustersList.begin() + cluster2_index);
        } else{
            clustersList.erase(clustersList.begin() + cluster1_index);
        }
    }
    for (int i = 0; i < numOfClusters; ++i) {
        intensity += 40;
        for (int j = 0; j < clustersList[i].size(); ++j) {
            int x = clustersList[i][j].x;
            int y = clustersList[i][j].y;
            outputImg(y, x) = intensity;
        }
    }
    return outputImg;
}
