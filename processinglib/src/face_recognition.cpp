//
// Created by abdulla167 on ٣‏/٦‏/٢٠٢١.
//
#include "iostream"
#include <filesystem>
#include <vector>
#include <Image.h>
#include <fstream>
#include "utilities.h"
#include "face_recognition.h"
#include "math.h"
#include "algorithm"

using std::filesystem::recursive_directory_iterator;
//using std::experimental::filesystem::recursive_directory_iterator;

void loadImgsDatasetOnly(std::string DirPath, std::vector<std::vector<float>>& Dataset, int numImgs = 150) {
    vector<std::string> TempPaths;
    for (const auto &file : recursive_directory_iterator(DirPath)) {
        TempPaths.push_back(file.path());
    }

    std::sort(TempPaths.begin(), TempPaths.end());
    for (int i = 0; i < TempPaths.size(); ++i) {
        Dataset.push_back(Image(TempPaths[i], 1).ImageAsVector());

        }

}
void loadImgsDatasetNames(std::string DirPath, std::vector<std::string>& Dataset, int numImgs = 17) {
    vector<std::string> TempPaths;
    for (const auto &file : recursive_directory_iterator(DirPath)) {
        TempPaths.push_back(file.path());
    }
        std::sort(TempPaths.begin(), TempPaths.end());
        for (int i = 0; i < TempPaths.size(); ++i) {
            Dataset.push_back(TempPaths[i].substr(numImgs,3));
//        if (i > numImgs)
//            break;
        }

}
std::vector<std::string> loadImgsDataset(std::string DirPath, std::vector<std::vector<float>> &Dataset) {
    vector<std::string> TempPaths;
    vector<std::string> filenames;
    for (const auto & file : recursive_directory_iterator(DirPath)){
        TempPaths.push_back(file.path());
    }
    std::sort(TempPaths.begin(), TempPaths.end());
    for (int i = 0; i < TempPaths.size(); ++i) {
        Dataset.push_back(Image(TempPaths[i], 3).toGrayscale().ImageAsVector());
        filenames.push_back(TempPaths[i].substr(2));

    }
    return filenames;
}

std::vector<float> AvgVector(std::vector<std::vector<float>>& TrainingDataset){
    std::vector<float> AverageVect;
    int numOfVectors = TrainingDataset.size();
    for (int i = 0; i < TrainingDataset[0].size(); ++i) {
        float temp = 0;
        for (int j = 0; j < numOfVectors; ++j) {
            temp += TrainingDataset[j][i];
        }
        AverageVect.push_back(temp / numOfVectors);
    }
    return AverageVect;
}

vector<vector<float>>  GetCenteredImgs(vector<vector<float>> TrainingDataset) {
    vector<float> AverageVect = AvgVector(TrainingDataset);
    for (int i = 0; i < TrainingDataset[0].size(); ++i) {
        for (int j = 0; j < TrainingDataset.size(); ++j) {
            TrainingDataset[j][i] = TrainingDataset[j][i] - AverageVect[i];
        }
    }
    return TrainingDataset;
}

vector<vector<float>> GetCovMatrix(vector<vector<float>>& TrainingDataset){
    vector<vector<float>> CorrMat = std::vector<vector<float>>(TrainingDataset.size(),vector<float>(TrainingDataset.size()));
    int M = TrainingDataset[0].size();
    float result = 0;
    for (int i = 0; i < TrainingDataset.size(); ++i) {
        for (int j = 0; j < TrainingDataset.size(); ++j) {
            result = 0;
            for (int k = 0; k < TrainingDataset[0].size(); ++k) {
                result += TrainingDataset[i][k] * TrainingDataset[j][k];
            }
            CorrMat[j][i] = result / (M);
        }
    }
    return CorrMat;
}

vector<vector<float>> GetEigenVectorsOfUpperCorr(vector<vector<float>>& TrainingDataset){
    vector<vector<float>> CovMat = GetCovMatrix(TrainingDataset);
    vector<pair<vector<float>, float>> EigenValuesAndVectors = egienVectorsValues(CovMat);
    vector<vector<float>> EigenFaces;
    float temp = 0;
    for (int eigenValueIndex = 0; eigenValueIndex < EigenValuesAndVectors.size(); ++eigenValueIndex) {
        vector<float> tempEigenFace;
        for (int i = 0; i < TrainingDataset[0].size(); ++i) {
            temp = 0;
            for (int j = 0; j < TrainingDataset.size(); ++j) {
                temp += TrainingDataset[j][i] * EigenValuesAndVectors[eigenValueIndex].first[j];
            }
            tempEigenFace.push_back(temp);
        }
        EigenFaces.push_back(vector<float>(tempEigenFace));
    }
    WriteVectorToFile("../Eigen_Faces_Matrix.txt", EigenFaces);
    return EigenFaces;
}

vector<vector<float>> getImagesCoeff(vector<vector<float>>& TrainingDataset, vector<vector<float>> & EigenVectors){
    vector<vector<float>> CoffMat =  std::vector<vector<float>>(TrainingDataset.size(),vector<float>(EigenVectors.size()));
    float tempCoff = 0;
    for (int imgIndex = 0; imgIndex < TrainingDataset.size(); ++imgIndex) {
        for (int eigenFaceIndex = 0; eigenFaceIndex < EigenVectors.size(); ++eigenFaceIndex) {
            tempCoff = 0;
            for (int i = 0; i < TrainingDataset[0].size(); ++i) {
                tempCoff = tempCoff + (EigenVectors[eigenFaceIndex][i] * TrainingDataset[imgIndex][i]);
            }
            CoffMat[imgIndex][eigenFaceIndex] = tempCoff;
        }
    }
    WriteVectorToFile("../Coefficient_Matrix.txt", CoffMat);
    return CoffMat;
}


vector<float> GetImgCoff(vector<vector<float>> EigenVectors , vector<float> ImgVector){
    vector<float> ImgCoff;
    float tempCoff = 0;
    for (int eigenFaceIndex = 0; eigenFaceIndex < EigenVectors.size(); ++eigenFaceIndex) {
        tempCoff = 0;
        for (int i = 0; i < ImgVector.size(); ++i) {
            tempCoff = tempCoff + (EigenVectors[eigenFaceIndex][i] * ImgVector[i]);
        }
        ImgCoff.push_back(tempCoff);
    }
    return ImgCoff;
}

float GetImgError(vector<float> TestImgCoff, vector<float> TrainImgCoff){
    float error = 0;
    for (int i = 0; i < TestImgCoff.size(); ++i) {
        error = error + abs(TestImgCoff[i] - TrainImgCoff[i]);
    }
    return error;
}

pair<int, float> GetSimilarImg(vector<vector<float>> ImgsCoffMatrix, vector<float> TestImgCoeff)
{
    pair<int, float> MinError{0,INFINITY};
    float TempError = 0;
    for (int i = 0; i < ImgsCoffMatrix.size(); ++i) {
        TempError = GetImgError(TestImgCoeff, ImgsCoffMatrix[i]);
        if (TempError < MinError.second){
            MinError.first = i;
            MinError.second = TempError;
        }
    }
    return MinError;
}

pair<int, float> PredictImg(vector<float> testImg, vector<vector<float>> EigenVectors, vector<vector<float>> ImgsCoffMat){
    vector<float> testImgCoeff = GetImgCoff(EigenVectors, testImg);
    pair<int, float> MinError = GetSimilarImg(ImgsCoffMat, testImgCoeff);
    return MinError;
}
pair<int, pair<int,vector<float>>> GetSimilarImg_1(vector<vector<float>> CoffMatrix, vector<float> ImgCoff)
{
    vector<std::string > classesNames {"s1-", "s2-", "s3-", "s4-", "s5-", "s6-", "s7-", "s8-", "s9-", "s10", "s11", "s12", "s13", "s14", "s15", "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23", "s24", "s25",
                                       "s26", "s27", "s28", "s29", "s30", "s31", "s32", "s33", "s34", "s35", "s36", "s37", "s38", "s39", "s40"};
    int classesNumber = classesNames.size();
    pair<int, pair<int,vector<float>>> result{0,pair<int,vector<float>>{0,vector<float>(classesNumber)}};

    float TempError = 0;

    vector<double> distances = vector<double>(classesNumber,0);
    vector<double> classCount = vector<double>(classesNumber,0);
    vector<std::string> trainImagesNames ;
    loadImgsDatasetNames("../Dataset/train", trainImagesNames,17);
    std::cout << trainImagesNames.size() << ", " << CoffMatrix.size() << "\n";
    for (int i = 0; i < CoffMatrix.size(); ++i) {
        TempError = GetImgError(ImgCoff, CoffMatrix[i]);
        std::string imageName = trainImagesNames[i];
        for(int class_ = 0; class_ < classesNumber; class_++){
            if(classesNames[class_] == imageName){
                distances[class_] += TempError;
                classCount[class_] += 1;

            }
        }
    }

    for(int class_ = 0; class_ < classesNumber; class_++) {
        distances[class_] = log10(distances[class_] / 8);
    }
    double sum = 0;
    for(int class_ = 0; class_ < classesNumber; class_++) {
        sum += distances[class_];
    }
    vector<float> similarity = vector<float>(classesNumber,0);
    for(int class_ = 0; class_ < classesNumber; class_++){


             similarity[class_] = distances[class_] ;
//             similarity[class_]  = std::exp( -1* similarity[class_]);
             double sumDistanceOthes = (sum - distances[class_])/ 39;
             similarity[class_] =  1-(similarity[class_] / (sumDistanceOthes + similarity[class_]));
    }
    result.second.second = similarity;
    float largest = 0;
    int smalcl = 0;
    for(int class_ = 0; class_ < classesNumber; class_++){

            if (similarity[class_] > largest){
                largest = similarity[class_];
                result.second.first = class_;
                smalcl = class_;
            }
    }
    std::cout<< " ki" <<smalcl<< "\n";
    pair<int, float> MinError {0, INFINITY};
    for (int i = 0; i < CoffMatrix.size(); ++i) {
        if(trainImagesNames[i] == classesNames[result.second.first]){
            TempError = GetImgError(ImgCoff, CoffMatrix[i]);
            std::string imageName = trainImagesNames[i];
            if (TempError < MinError.second){
                MinError.second = TempError;
                MinError.first = i;
            }
        }
    }
    result.first = MinError.first;

    return result;
}

pair<int, pair<int,vector<float>>> TestImg_1(vector<float> testImg, vector<vector<float>> EigenFaces,  vector<vector<float>> CoffMat){
    vector<float> imgCoff = GetImgCoff(EigenFaces, testImg);
    pair<int, pair<int,vector<float>>> similarity = GetSimilarImg_1(CoffMat, imgCoff);
    return similarity;
}

void WriteVectorToFile(std::string filename, vector<vector<float>> Mat){
    std::ofstream outFile(filename);
    for (int i = 0; i < Mat.size(); ++i) {
        for (int j = 0; j < Mat[0].size(); ++j) {
            outFile << Mat[i][j] << " ";
        }
        outFile << std::endl;
    }
}

void ReadFileToVector(std::string filename, vector<vector<float>>& Mat){
    std::string str;
    std::ifstream in(filename);
    while (std::getline(in, str))
    {
        if (str.size() > 0){
            std::stringstream ss(str);
            std::string word;
            vector<float> temp;
            while (ss >> word) {
                temp.push_back(std::stof(word));
            }
            Mat.push_back(temp);
        }
    }
}

float TestImages() {
    vector<std::string > classesNames {"s1-", "s2-", "s3-", "s4-", "s5-", "s6-", "s7-", "s8-", "s9-", "s10", "s11", "s12", "s13", "s14", "s15", "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23", "s24", "s25",
                                       "s26", "s27", "s28", "s29", "s30", "s31", "s32", "s33", "s34", "s35", "s36", "s37", "s38", "s39", "s40"};


    vector<vector<float>> testData;
     loadImgsDatasetOnly("../Dataset/test", testData, -1);
     vector<std::string> names;
     loadImgsDatasetNames("../Dataset/test", names, 16);
    vector<vector<float>> CoffMat;
    vector<vector<float>> EigenFaces;

    ReadFileToVector("../Coefficient_Matrix.txt", CoffMat);
    ReadFileToVector("../Eigen_Faces_Matrix.txt", EigenFaces);
    float sum = 0;
    std::ofstream outfile("../roc-data.txt");
    for(int img = 0; img < testData.size(); img++){

        std::pair<int, std::pair<int, vector<float>>> result = TestImg_1(testData[img], EigenFaces, CoffMat);

        for (int j = 0; j < result.second.second.size(); j++)
                outfile  <<  std::to_string(result.second.second[j]) << "\t" ;
        for(int i = 0; i <classesNames.size(); i++){
//            std::cout<< classesNames[i] <<"=="<< names[img]<<"\n";
            if(classesNames[i] == names[img]){
                if(i == result.second.first)
                sum++;
                outfile << i << "\n";
                break;

            }
        }
    }
    return sum/ testData.size();
}