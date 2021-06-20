//
// Created by abdulla167 on ٣‏/٦‏/٢٠٢١.
//

#ifndef CV_FACE_RECOGNITION_H
#define CV_FACE_RECOGNITION_H
#include "vector"

using std::vector; using std::pair;


void loadImgsDatasetOnly(std::string DirPath, std::vector<std::vector<float>>& TrainingDataset, int numImgs);

void loadImgsDatasetNames(std::string DirPath, std::vector<std::string>& Dataset, int numImgs);

std::vector<std::string> loadImgsDataset(std::string DirPath, std::vector<std::vector<float>> &Dataset);

std::vector<float> AvgVector(std::vector<std::vector<float>>& TrainingDataset);

vector<vector<float>>  GetCenteredImgs(vector<vector<float>> TrainingDataset);

float VectorsDotProduct(std::vector<float> v1, std::vector<float> v2);

std::vector<std::vector<float>> GetCovMatrix(std::vector<std::vector<float>>& TrainingDataset);

vector<vector<float>> GetEigenVectorsOfUpperCorr(vector<vector<float>>& TrainingDataset);

vector<vector<float>> getImagesCoeff(vector<vector<float>>& TrainingDataset, vector<vector<float>> & EigenVectors);

pair<int, pair<int,vector<float>>> TestImg_1(vector<float> testImg, vector<vector<float>> EigenFaces,  vector<vector<float>> CoffMat);

pair<int, float> PredictImg(vector<float> testImg, vector<vector<float>> EigenVectors, vector<vector<float>> ImgsCoffMat);

void WriteVectorToFile(std::string filename, vector<vector<float>> Mat);

void ReadFileToVector(std::string filename, vector<vector<float>>& Mat);

float TestImages();
#endif //CV_FACE_RECOGNITION_H
