#include<opencv2/opencv.hpp>
#include<iostream>
#include<conio.h>

using namespace std;
using namespace cv;
///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
	//Création d'objets pouvant contenir une matrice de pixels (une image) et permettant la modification de celle-ci
	IplImage *Rendu,*imgBgnd2,*imgBgnd3,*imgBgnd4,*imgBgnd5,*imgBgnd6;
  //copie dans les objets de type IplImage le contenu des images situés à l'adresse indiquée
	Rendu = cvLoadImage("Final/bgnd1.png");
	imgBgnd2 = cvLoadImage("Final/bgnd2.png");
	imgBgnd3 = cvLoadImage("Final/bgnd3.png");
	imgBgnd4 = cvLoadImage("Final/bgnd4.png");
	imgBgnd5 = cvLoadImage("Final/bgnd5.png");
	imgBgnd6 = cvLoadImage("Final/bgnd6.png");

	//Masques :
	IplImage *maskbgnd2;  //création d'un objet de type image (le masque est une image)

	/*cvCreateImage : transforme le type IplImageen un "réceptacle" pour une image particulière
	Arguments de cvCreateImage : (size,color_depth,channels)
	Size : largeur et longueur de l'image (en nombre de pixels)
	Color_depth : nombre de valeurs que peut prendre une couleur ex: pour 8 bits le rouge,le vert et le bleu seront codés sur 256 valeurs
	channels : "http://images.math.cnrs.fr/le-traitement-numerique-des-images.html*   	*/
	
	maskbgnd2 = cvCreateImage(cvGetSize(imgBgnd2), 8, 1);  //transformation du masque
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
