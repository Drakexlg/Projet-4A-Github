#include<opencv2/opencv.hpp>
#include<iostream>
#include<conio.h>           // may have to modify this line if not using Windows
//Attention : le fond de l'image a supperposé ne doit pas etre transparent mais plutot 
//d'une couleur uni qui'nest pas present sur l'image !!
///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
	IplImage *imgOriginal;
	imgOriginal = cvLoadImage("plane.png");          // open image
	IplImage *fond;
	fond = cvLoadImage("background.jpg");
	IplImage *final;
	// show windows
	cvShowImage("GeckoGeek Image PNG & Transparence", imgOriginal);
	IplImage *mask;
	mask = cvCreateImage(cvGetSize(imgOriginal), 8, 1);
	//detection du bleu et creation du mask en noir et blanc
	cvInRangeS(imgOriginal, cvScalar(125.0, 0.0, 0.0), cvScalar(255.0 + 1.0, 130.0, 130.0), mask);

	cvNot(mask, mask);

	//cvShowImage("mask", mask);

	cvSetImageROI(fond, cvRect(300, 30, imgOriginal->width, imgOriginal->height));
	cvCopy(imgOriginal, fond, mask);
	cvResetImageROI(fond);


	cvShowImage("all", fond);

	cv::waitKey(0);                 // hold windows open until user presses a key

	return(0);
}
