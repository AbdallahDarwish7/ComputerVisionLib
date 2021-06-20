//
// Created by abdulla167 on ٢١‏/٥‏/٢٠٢١.
//

#ifndef CV_AGGLOMERATIVE_CLUSTER_SEGMENTATION_H
#define CV_AGGLOMERATIVE_CLUSTER_SEGMENTATION_H
float euclideanDistance(point p1,point p2);

float clustersDistance(std::vector<point> * cluster1, std::vector<point> * cluster2);

void initialClusters(Image& img, std::vector<std::vector<point>> & clustersList);

int mergeTwoClusters(std::vector<point> & cluster1, std::vector<point> & cluster2);

Image agglomerativeSeg(Image & img, int numOfClusters, unsigned long initialClustersNum);

#endif //CV_AGGLOMERATIVE_CLUSTER_SEGMENTATION_H
