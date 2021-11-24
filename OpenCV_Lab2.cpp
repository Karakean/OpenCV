#define max_value 255

#include <stdio.h>
using namespace std;

#include <iostream>

// OpenCV includes
#include <opencv2/opencv.hpp>
using namespace cv;

Mat grayImage, binarizedImage;
int threshold_value = 100;

void Threshold(int pos, void* userdata) {
	threshold(grayImage, binarizedImage, threshold_value, max_value, THRESH_BINARY);
	imshow("Binarization", binarizedImage);
}

int main()
{
	// reading source file srcImage
	Mat srcImage;
	srcImage = imread( "Samples/ryba.jpg" );
	if ( !srcImage.data )
	{
		cout << "Error! Cannot read source file. Press ENTER.";
		waitKey();
		return( -1 );
	}
	namedWindow("Mikolaj Nowak");
	moveWindow("Mikolaj Nowak", 0, 0);
	imshow("Mikolaj Nowak", srcImage);

	cvtColor(srcImage, grayImage, COLOR_BGR2GRAY);
	namedWindow("Gray image");
	moveWindow("Gray image", 300, 0);
	imshow("Gray image", grayImage);
	imwrite("Samples/Gray_image.jpg", grayImage);

	Mat resizedImage(100, 100, srcImage.type());
	resize(srcImage, resizedImage, resizedImage.size());
	namedWindow("Resized image");
	moveWindow("Resized image", 600, 0);
	imshow("Resized image", resizedImage);
	imwrite("Samples/Resized_image.jpg", resizedImage);

	Mat blurImage;
	blur(srcImage, blurImage, Size(5, 5));
	namedWindow("Blured image");
	moveWindow("Blured image", 900, 0);
	imshow("Blured image", blurImage);

	Mat CannyImage;
	Canny(srcImage, CannyImage, 70, 70);
	namedWindow("Canny edges");
	moveWindow("Canny edges", 1200, 0);
	imshow("Canny edges", CannyImage);

	Mat LaplacianImage;
	Laplacian(srcImage, LaplacianImage, CV_16S, 3);
	Mat scaledLaplacianImage;
	convertScaleAbs(LaplacianImage, scaledLaplacianImage);
	namedWindow("Laplacian image");
	moveWindow("Laplacian image", 1500, 0);
	imshow("Laplacian image", scaledLaplacianImage);

	Mat brightImage;
	srcImage.copyTo(brightImage);
	for (int i = 0; i < brightImage.rows; i++) {
		for (int j = 0; j < brightImage.cols; j++) {
			Vec3b pixelColor;
			pixelColor = brightImage.at<Vec3b>(Point(i,j)); //to
			for (int k = 0; k < 3; k++) {
				if (pixelColor[k] + 100 > 255)
					pixelColor[k] = 255;
				else
					pixelColor[k] += 100;
			}
			brightImage.at<Vec3b>(Point(i, j)) = pixelColor;
		}
	}
	namedWindow("Bright image");
	moveWindow("Bright image", 900, 300);
	imshow("Bright image", brightImage);
	
	namedWindow("Binarization");
	moveWindow("Binarization", 600, 0);
	createTrackbar("Treshold value", "Binarization", &threshold_value, max_value, Threshold); //to2
	
	waitKey();

	namedWindow("Src video");
	moveWindow("Src video", 300, 600);
	namedWindow("Dst video");
	moveWindow("Dst video", 900, 600);
	Mat srcFrame, dstFrame;
	VideoCapture capture("Samples/Dino.avi");
	capture >> srcFrame;
	VideoWriter writer("Samples/Dino2.avi", -1, 25, srcFrame.size());
	while (waitKey(25) != 27 && !srcFrame.empty()) {
		cvtColor(srcFrame, dstFrame, COLOR_BGR2GRAY);
		writer << dstFrame;
		imshow("Src video", srcFrame);
		imshow("Dst video", dstFrame);
		capture >> srcFrame;
	}

	const int histSize = 256;
	const int hist_w = 256;
	const int hist_h = 256;
	float range[2] = {0, 256};
	const float* histRange = range;
	Mat histImageGray(hist_h, hist_w, CV_8UC3, Scalar (0,0,0));
	Mat histogramGray;
	calcHist(&grayImage,1,0,Mat(),histogramGray,1,&histSize, &histRange);
	normalize(histogramGray, histogramGray, range[0], range[1], NORM_MINMAX);
	for (int i = 0; i < histSize; i++) {
		line(histImageGray, Point(i,hist_h-1), Point(i,hist_h - cvRound(histogramGray.at<float>(i))), Scalar(0, 0, 255), 1);
	}
	namedWindow("Histogram Gray");
	moveWindow("Histogram Gray", 0, 300);
	imshow("Histogram Gray", histImageGray);

	Mat equalizeHistImage;
	equalizeHist(grayImage, equalizeHistImage);
	namedWindow("Equalized Histogram Image");
	moveWindow("Equalized Histogram Image", 300, 300);
	imshow("Equalized Histogram Image", equalizeHistImage);

	Mat histImageEqualized(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
	Mat histogramEqualized;
	calcHist(&equalizeHistImage, 1, 0, Mat(), histogramEqualized, 1, &histSize, &histRange);
	normalize(histogramEqualized, histogramEqualized, range[0], range[1], NORM_MINMAX);
	for (int i = 0; i < histSize; i++) {
		line(histImageEqualized, Point(i, hist_h - 1), Point(i, hist_h - cvRound(histogramEqualized.at<float>(i))), Scalar(0, 255, 0), 1);
	}
	namedWindow("Histogram Equalized");
	moveWindow("Histogram Equalized", 600, 300);
	imshow("Histogram Equalized", histImageEqualized);

	waitKey();
}
