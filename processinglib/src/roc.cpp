//
// Created by mohamedkamal on ٤‏/٦‏/٢٠٢١.
//

#include "roc.h"
#include <string>
#include <iostream>
ROC::ROC(std::string filename,int classes){
    this->classes = classes;

    float score;
    int label;
    int actualLabel;
    this->FP = std::vector<std::vector<float>>(this->classes);
    this->TP = std::vector<std::vector<float>>(this->classes);
    std::ifstream inFile(filename.c_str());

    if(!inFile) {
        std::cerr << "File "<< filename << " not found.";
    }

    // read the data from the file
    std::pair<std::vector<float>,int> temp = std::pair{std::vector<float>(classes, 0),0};
    while(!inFile.eof()) {
        for(int loop = 0; loop < classes; loop++){
            inFile  >>  score;
            std:: cout << score << " , ";
            temp.first[loop] = score;
        }
        inFile >> label;
        std::cout << label << "\n";
        temp.second = label;
        this->data.push_back(temp);
        if (!inFile) break;



    }
    getROCFromData(this->data);
}

ROC::ROC(std::vector<std::pair<std::vector<float>,int>> data_, int classes) {
    this->classes = classes;
    this->FP = std::vector<std::vector<float>>(this->classes);
    this->TP = std::vector<std::vector<float>>(this->classes);
    this->data = data_;
    getROCFromData(this->data);
}
ROC::~ROC() {
}

void ROC::writeToFile(std::string filename){
    std::ofstream outfile(filename.c_str());
    for (int i = 0; i < this->TP[0].size(); i++) {
        for(int j =0; j < this->TP.size(); j++)
            outfile  <<  std::to_string(FP[j][i]) << "\t" << std::to_string(TP[j][i]) << "\t";
            outfile << std::endl;

    }
    std::cout <<  "ROC curve points saved to file " << filename << std::endl;
}


std::vector<float> ROC::getAreaUnderCurve(){
    // TODO - Implement this
    std::vector<float> areas  = std::vector<float>(this->classes, 0);
    for (int class_ = 0; class_ < this->classes; class_++) {
        int size = TP[class_].size();
        float q1, q2, p1, p2;
        q1 = FP[class_][0];
        q2 = TP[class_][0];
        float area = 0.0;
        for (int i = 1; i < size; ++i) {
            p1 = FP[class_][i];
            p2 = TP[class_][i];
            area += sqrt(pow(((1 - q1) + (1 - p1)) / 2 * (q2 - p2), 2));
            q1 = p1;
            q2 = p2;
        }
        areas[class_] = area;
    }
    return areas;
}

bool compare(const std::pair<float,int>&i, const std::pair<float,int>&j){
    return i.first > j.first;
}

void ROC::getROCFromData(std::vector<std::pair<std::vector<float>,int>>  data){
    //sort the data by classification score
//    sort(data.begin(),data.end(),compare);
    int L = data.size();
    std::vector<float> thresholds ;
    for (int loop =0; loop < 50; loop++) {
        thresholds.push_back(loop*0.02);
    }




    float similarity;
    int label;
    //init FP TP counters
    std::vector<std::vector<int>> TP = std::vector<std::vector<int>>(this->classes,std::vector<int>(thresholds.size(),0));
    std::vector<std::vector<int>> FP = std::vector<std::vector<int>>(this->classes, std::vector<int>(thresholds.size(),0));
    std::vector<std::vector<int>> TN = std::vector<std::vector<int>>(this->classes,std::vector<int>(thresholds.size(),0));
    std::vector<std::vector<int>> FN = std::vector<std::vector<int>>(this->classes, std::vector<int>(thresholds.size(),0));
    double f_prev = -std::numeric_limits<double>::infinity();
    std::vector<std::pair<float,float> > R;
    //loop through all data
    for(int class_ = 0; class_ < this->classes; class_++){

        for(int threshold = 0; threshold < thresholds.size(); threshold++){
            for (int i = 0; i < data.size(); i++) {

                similarity = data[i].first[class_];
                label = data[i].second;

            if (similarity >= thresholds[threshold] ){
                if (class_ == label){
                    TP[class_][threshold] += 1;
                } else {
                    FP[class_][threshold] +=1;
                }
            } else {
                if (class_ == label){
                    FN[class_][threshold] += 1;
                } else {
                    TN[class_][threshold] +=1;
                }
            }
                std::cout << "threshold : " << thresholds[threshold] << "similarity " << similarity << " : "<<TP[class_][threshold] << " , " << FP[class_][threshold] << " , " << TN[class_][threshold] << " , " << FN[class_][threshold] << "\n";


            }

            std::cout << "threshold : " << thresholds[threshold] << " : "<<TP[class_][threshold] << " , " << FP[class_][threshold] << " , " << TN[class_][threshold] << " , " << FN[class_][threshold] << "\n";
            this->TP[class_].push_back((float)TP[class_][threshold] /(float)(TP[class_][threshold] + FN[class_][threshold]));
            this->FP[class_].push_back((float)FP[class_][threshold] / (float)(TN[class_][threshold] + FP[class_][threshold]));
        }
        std::cout << "jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj \n";
    }

}


