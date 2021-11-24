#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>


int main()
{
    cv::Mat srcImage[9];
    std::ifstream file;
    file.open("184865_CSV.csv");
    if (!file) {
        printf("Blad odczytu pliku .csv\n");
        return -1;
    }
    std::string line;
    std::getline(file, line);
    std::string first_half = "Images\\184865_";
    char index = 'A';
    std::string extension = ".jpg";
    for (int i = 0; i < 9; i++) {
        std::string path = first_half + index + extension;
        srcImage[i] = cv::imread(path);
        if (!srcImage[i].data) {
            printf("Blad odczytu pliku .jpg\n");
            return -1;
        }
        std::getline(file, line);
        std::string delimiter = ";";
        std::string token;
        std::size_t j;
        bool is_x = true; //czy jest koordynatem x-owym
        double coordinates[2];
        while ((j = line.find(delimiter)) != std::string::npos) {
            token = line.substr(0, j);
            if (token[token.length() - 2] == ',') //zmiana przecinkow z pliku CSV na kropki
                token[token.length() - 2] = '.';
            if ((is_x) && (token[0] != '"')) { //czy jest koordynatem x-owym oraz czy nie jest pierwszym tokenem linii, tj. sciezka
                coordinates[0] = std::stod(token);
                is_x = false;
            }
            else if (token[0] != '"') {
                coordinates[1] = std::stod(token);
                cv::circle(srcImage[i], cv::Point(coordinates[0], coordinates[1]), 10, cv::Scalar(255, 255, 0), -1);
                is_x = true;
            }
            line.erase(0, j + delimiter.length());
        }
        cv::circle(srcImage[i], cv::Point(coordinates[0], std::stod(line)), 10, cv::Scalar(255, 255, 0), -1); //ostatni punkt
        std::string save = first_half + index + "_OUT" + extension;
        cv::imwrite(save, srcImage[i]);
        index++;
        srcImage[i].release();
    }
    srcImage->release();
    file.close();
    return 0;
}
