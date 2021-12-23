#include <iostream>
#include <cmath>
#include <string>
// OpenCV includes
#include <opencv2/opencv.hpp>
#include "opencv2/core/types_c.h"
#include "opencv2/calib3d/calib3d_c.h"
#include "Typedefs.h"
using namespace std;


int main()
{
	Mat srcImage;
	const int scale = 4;
	srcImage = imread( "184865_D.jpg" );
	if ( !srcImage.data )
	{
		cout << "Error! Cannot read source file. Press ENTER.\n";
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

	Size patternsize[4];
	vector<Point2f> corners[4];
	bool result[4];
	int test = 0;
	float roznica[2][4];
	double odleglosc[4];

	float reverseScale = 100.0f;
	vector<Point2f>templateCorners[4];
	Mat homography[4];
	Mat dewarpedImage(srcImage.size(), srcImage.type());
	Mat dewarpedROI;
	Mat debugImage;
	std::vector<Point2f>dewarpedCorners[4];

	for (int i = 0; i < 4; i++) {
		patternsize[i] = Size(CheckerboardInnerWidth[i], CheckerboardInnerHeight[i]);
		result[i] = findChessboardCorners(grayImage, patternsize[i], corners[i]);
		if (!result[i]) {
			cout <<"Chessboard wasn't found. Press ENTER.\n";
			waitKey();
			return(-1);
		}
		cornerSubPix(grayImage, corners[i], Size(5, 5), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
		drawChessboardCorners(grayImage, patternsize[i], corners[i], true);
		for (int j = 0; j < corners[i].size(); j++) {
			putText(grayImage, to_string(j), corners[i][j], 1, 1, Scalar(255, 0, 0), 1, 8, false);
		}
		roznica[0][i] = corners[i][1].x - corners[i][0].x;
		roznica[1][i] = corners[i][1].y - corners[i][0].y;
		odleglosc[i] = sqrt(pow(roznica[0][i], 2) + pow(roznica[1][i], 2));
		cout << odleglosc[i] << endl;

		for (int j = 0; j < patternsize[i].height; j++) {
			for (int k = 0; k < patternsize[i].width; k++) {
				Point2f coord;
				coord.x = CheckerboardLTCoordinatesWithMargin[i].x + (k + 1) * CHECKERBOARD_FIELD_SIZE;
				coord.y = CheckerboardLTCoordinatesWithMargin[i].y + (j + 1) * CHECKERBOARD_FIELD_SIZE;
				templateCorners[i].push_back(coord * reverseScale);
			}
		}

		homography[i] = findHomography(corners[i], templateCorners[i], CV_RANSAC);
		warpPerspective(resizedImage, dewarpedImage, homography[i], srcImage.size());

		dewarpedROI = dewarpedImage(Rect(reverseScale * TemplateLT, reverseScale * TemplateRB));
		debugImage = dewarpedROI.clone();

		perspectiveTransform(corners[i], dewarpedCorners[i], homography[i]);
		for (int j = 0; j < dewarpedCorners[i].size(); j++) {
			circle(debugImage, dewarpedCorners[i][j], 10, Scalar(255, 0, 0), 5);
		}
		string filename = "184865_D_Dewarped_ROI" + to_string(i + 1) + ".jpg";
		imwrite(filename, debugImage);
	}
	waitKey();
}
