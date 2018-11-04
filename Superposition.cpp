#include<opencv2/opencv.hpp>
#include<iostream>
#include<conio.h>           // may have to modify this line if not using Windows

///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
	IplImage *imgOriginal;
	imgOriginal = cvLoadImage("plane.png");          // open image
	IplImage *cache;
	cache = cvLoadImage("Cache.png");   //Obstacle du premier plan
	IplImage *fond;
	fond = cvLoadImage("background.jpg"); 
	
	
	//Creation des masks
		//Mask objet a slider
	IplImage *maskav;
		maskav = cvCreateImage(cvGetSize(imgOriginal), 8, 1);
		//detection du bleu et creation du mask en noir et blanc
		cvInRangeS(imgOriginal, cvScalar(0.0, 255.0, 76.0), cvScalar(0.0, 255.0, 76.0), maskav); //Vert  BGR!!
		cvNot(maskav, maskav);	
		
		//Mask objet de devant
		IplImage *mask;
		mask = cvCreateImage(cvGetSize(cache), 8, 1);
		//detection du bleu et creation du mask en noir et blanc
		cvInRangeS(cache, cvScalar(0.0, 255.0, 76.0), cvScalar(0.0, 255.0, 76.0), mask);
		cvNot(mask, mask);
	//Declaration rendu final
	IplImage *Rendu;
	Rendu = cvLoadImage("background.jpg");
	int i;
	//Pour faire changer la valeur de i : rester appuyer sur une touche 
	for (i = 0; i < 1000; i++) {
		
		//Reinitialisation de la variable fond
		cvSetImageROI(Rendu, cvRect(0,0, fond->width, fond->height));
		cvCopy(fond, Rendu);
		cvResetImageROI(Rendu);
		//Incrustation de l'image a slider
		cvSetImageROI(Rendu, cvRect(300+i, 30+i, imgOriginal->width, imgOriginal->height));
		cvCopy(imgOriginal, Rendu, maskav);
		cvResetImageROI(Rendu);
		//Supperposition de l'obstacle visuel
		cvSetImageROI(Rendu, cvRect(300, 30, cache->width, cache->height));
		cvCopy(cache, Rendu, mask);
		cvResetImageROI(Rendu);
		//Affichage
		cvShowImage("all", Rendu);
		cvWaitKey(0);
	}

	

	

	cv::waitKey(0);                 // hold windows open until user presses a key

	return(0);
}