
#include <stdio.h>
#include <cmath>
#include <string>
using namespace std;

#include <iostream>

// OpenCV includes
#include <opencv2/opencv.hpp>
#include "opencv2/core/types_c.h"
#include "opencv2/calib3d/calib3d_c.h"
#include "Typedefs.h"


int main()
{

	Mat srcImage;
	const int scale = 4;
	srcImage = imread( "184865_D.jpg" );
	if ( !srcImage.data )
	{
		cout << "Error! Cannot read source file. Press ENTER.";
		waitKey();
		return( -1 );
	}
	Mat resizedImage;
	resize(srcImage, resizedImage, Size(srcImage.cols / scale, srcImage.rows / scale));
	namedWindow("Mikolaj Nowak");
	moveWindow("Mikolaj Nowak", 0, 0);
	//imshow("Mikolaj Nowak", resizedImage);
	Mat grayImage;
	cvtColor(resizedImage, grayImage, COLOR_BGR2GRAY);
	Size patternsize(CheckerboardInnerWidth[0], CheckerboardInnerHeight[0]);
	vector<Point2f> corners;
	for (int i = 0; i < 4; i++) {
		Point2f tmp(CheckerboardInnerWidth[i], CheckerboardInnerHeight[i]);
		corners.push_back(tmp);
	}
	bool result = findChessboardCorners(grayImage, patternsize, corners);
	if (!result) {
		cout << "Error! Press ENTER.";
		waitKey();
		return(-1);
	}
	cornerSubPix(grayImage, corners, patternsize, Size(CheckerboardInnerWidth[3], CheckerboardInnerHeight[3]), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
	drawChessboardCorners(grayImage,patternsize,corners,true);
	//imshow("Mikolaj Nowak", grayImage);
	//imwrite("184865_D_TopChessboardFound.jpg", grayImage);
	double roznicax = corners[1].x - corners[0].x;
	double roznicay = corners[1].y - corners[0].y;
	double odleglosc = sqrt(pow(roznicax,2) + pow(roznicay,2));
	cout << odleglosc << endl;
	for (int i = 0; i < 39; i++) {
		putText(grayImage, to_string(i), corners[i], 1, 1, Scalar(255,0,0), 1,8,false);
	}
	imwrite("184865_D_TopChessboardFound.jpg", grayImage);
	//imshow("Mikolaj Nowak", grayImage);
	float reverseScale = 100.0f;
	vector<Point2f>templateCorners;
	for (int i = 0; i < patternsize.height; i++) {
		for (int j = 0; j < patternsize.width; j++) {
			Point2f coord;
			coord.x = CheckerboardLTCoordinatesWithMargin[0].x + (j+1) * CHECKERBOARD_FIELD_SIZE;
			coord.y = CheckerboardLTCoordinatesWithMargin[0].y + (i+1) * CHECKERBOARD_FIELD_SIZE;
			templateCorners.push_back(coord * reverseScale);
		}
	}
	Mat homography = findHomography(corners, templateCorners, CV_RANSAC);
	Mat dewarpedImage(srcImage.size(), srcImage.type());
	warpPerspective(resizedImage, dewarpedImage, homography, srcImage.size());
	imwrite("184865_D_Dewarped.jpg", dewarpedImage);
	
	Mat dewarpedROI = dewarpedImage(Rect(reverseScale * TemplateLT, reverseScale * TemplateRB));
	Mat debugImage = dewarpedROI.clone();
	std::vector<Point2f>dewarpedCorners;
	perspectiveTransform(corners, dewarpedCorners, homography);
	for (int i = 0; i < 39; i++) {
		circle(debugImage, dewarpedCorners[i], 12, Scalar(255, 0, 0), 2);
	}
	imshow("Mikolaj Nowak", debugImage);
	imwrite("184865_D_Dewarped_ROI.jpg", debugImage);
	waitKey();
}
