#include<opencv2/opencv.hpp>
#include<iostream>
#include<conio.h>           // may have to modify this line if not using Windows

using namespace std;
using namespace cv;
///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
	IplImage *Rendu;
	Rendu = cvLoadImage("Final/bgnd1.png");          // open image
	IplImage *imgBgnd2;
	imgBgnd2 = cvLoadImage("Final/bgnd2.png");   //Obstacle du premier plan
	IplImage *imgBgnd3;
	imgBgnd3 = cvLoadImage("Final/bgnd3.png");
	IplImage *imgBgnd4;
	imgBgnd4 = cvLoadImage("Final/bgnd4.png");          // open image
	IplImage *imgBgnd5;
	imgBgnd5 = cvLoadImage("Final/bgnd5.png");   //Obstacle du premier plan
	IplImage *imgBgnd6;
	imgBgnd6 = cvLoadImage("Final/bgnd6.png");

	//Masques :
	IplImage *maskbgnd2;
	maskbgnd2 = cvCreateImage(cvGetSize(imgBgnd2), 8, 1);
	cvInRangeS(imgBgnd2, cvScalar(0.0, 255.0, 76.0), cvScalar(0.0, 255.0, 76.0), maskbgnd2);//Vert  BGR!!
	cvNot(maskbgnd2, maskbgnd2);


	IplImage *maskbgnd3;
	maskbgnd3 = cvCreateImage(cvGetSize(imgBgnd3), 8, 1);
	cvInRangeS(imgBgnd3, cvScalar(0.0, 255.0, 76.0), cvScalar(0.0, 255.0, 76.0), maskbgnd3);
	cvNot(maskbgnd3, maskbgnd3);


	IplImage *maskbgnd4;
	maskbgnd4 = cvCreateImage(cvGetSize(imgBgnd4), 8, 1);
	cvInRangeS(imgBgnd4, cvScalar(0.0, 255.0, 76.0), cvScalar(0.0, 255.0, 76.0), maskbgnd4);
	cvNot(maskbgnd4, maskbgnd4);


	IplImage *maskbgnd5;
	maskbgnd5 = cvCreateImage(cvGetSize(imgBgnd5), 8, 1);
	cvInRangeS(imgBgnd5, cvScalar(0.0, 255.0, 76.0), cvScalar(0.0, 255.0, 76.0), maskbgnd5);
	cvNot(maskbgnd5, maskbgnd5);


	IplImage *maskbgnd6;
	maskbgnd6 = cvCreateImage(cvGetSize(imgBgnd6), 8, 1);
	cvInRangeS(imgBgnd6, cvScalar(0.0, 255.0, 76.0), cvScalar(0.0, 255.0, 76.0), maskbgnd6);
	cvNot(maskbgnd6, maskbgnd6);
	/* AFFICHAGE DES MASQUES
	cvShowImage("maskbgnd2", maskbgnd2);
	cvShowImage("maskbgnd3", maskbgnd3);
	cvShowImage("maskbgnd4", maskbgnd4);
	cvShowImage("maskbgnd5", maskbgnd5);
	cvShowImage("maskbgnd6", maskbgnd6);*/

		//Reinitialisation de la variable fond
		cvSetImageROI(Rendu, cvRect(0, 0, Rendu->width, Rendu->height));
		//Incrustation de l'image a slider
		cvSetImageROI(Rendu, cvRect(0, 0, imgBgnd6->width, imgBgnd6->height));
		cvCopy(imgBgnd6, Rendu, maskbgnd6);
		cvResetImageROI(Rendu);

		cvSetImageROI(Rendu, cvRect(0, 0, imgBgnd5->width, imgBgnd5->height));
		cvCopy(imgBgnd5, Rendu, maskbgnd5);
		cvResetImageROI(Rendu);

		cvSetImageROI(Rendu, cvRect(0, 0, imgBgnd4->width, imgBgnd4->height));
		cvCopy(imgBgnd4, Rendu, maskbgnd4);
		cvResetImageROI(Rendu);

		cvSetImageROI(Rendu, cvRect(0, 0, imgBgnd3->width, imgBgnd3->height));
		cvCopy(imgBgnd3, Rendu, maskbgnd3);
		cvResetImageROI(Rendu);

		cvSetImageROI(Rendu, cvRect(0,0, imgBgnd2->width, imgBgnd2->height));
		cvCopy(imgBgnd2, Rendu, maskbgnd2);
		cvResetImageROI(Rendu);

		//Affichage
		cvShowImage("all", Rendu);

	cv::waitKey(0);                 // hold windows open until user presses a key

	return(0);
}
