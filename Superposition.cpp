#include<opencv2/opencv.hpp>
#include<iostream>
#include<conio.h>
#include"opencv2\highgui.hpp"
#include"opencv2\imgproc.hpp"
#include <windows.h>

using namespace std;
using namespace cv;
///////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
	//Création d'objets pouvant contenir une matrice de pixels (une image) et permettant la modification de celle-ci
	Mat Rendu, imgBgnd6, imgBgnd5, imgBgnd4, imgBgnd3, imgBgnd2, imgFond, ronds, ninja, Rendufinal;
	Mat fond_hsv, bgnd6_hsv, bgnd5_hsv, bgnd4_hsv, bgnd3_hsv, bgnd2_hsv, rendu_hsv, round_hsv, ninja_hsv;
	int posx = 95, posy = 280;
	cvNamedWindow("Game", CV_WINDOW_AUTOSIZE);
	moveWindow("Game", 500, -32);
	//copie dans les objets de type IplImage le contenu des images situés à l'adresse indiquée
	Rendu = imread("Textures/bgnd1.png");
	imgBgnd2 = imread("Textures/bgnd2.png");
	imgBgnd3 = imread("Textures/bgnd3.png");
	imgBgnd4 = imread("Textures/bgnd4.png");
	imgBgnd5 = imread("Textures/bgnd5.png");
	imgBgnd6 = imread("Textures/bgnd6.png");
	imgFond = imread("Textures/black_bgnd.png");
	ronds = imread("Textures/ronds.png");
	ninja = imread("Textures/ninja.png");

	//conversion de toutes les tesxtures en HSV pour effectuer le traitement des masques
	cvtColor(imgFond, fond_hsv, COLOR_BGR2HSV);
	cvtColor(imgBgnd6, bgnd6_hsv, COLOR_BGR2HSV);
	cvtColor(imgBgnd5, bgnd5_hsv, COLOR_BGR2HSV);
	cvtColor(imgBgnd4, bgnd4_hsv, COLOR_BGR2HSV);
	cvtColor(imgBgnd3, bgnd3_hsv, COLOR_BGR2HSV);
	cvtColor(imgBgnd2, bgnd2_hsv, COLOR_BGR2HSV);
	cvtColor(Rendu, rendu_hsv, COLOR_BGR2HSV);
	cvtColor(ronds, round_hsv, COLOR_BGR2HSV);
	cvtColor(ninja, ninja_hsv, COLOR_BGR2HSV);

	//Mat subrendu = fond_hsv(cvRect(64, 36, 1152, 648));

	Mat subrendu = fond_hsv(cvRect(64, 36, 1152, 648));
	RotatedRect rect = RotatedRect(Point2f(23,24 ), Size2f(46, 48), -60);
	Mat M, rotated, cropped;
	float angle = rect.angle;
	Size rect_size = rect.size;
	/*if (angle < -45.) {
		angle += 90.0;
		swap(rect_size.width, rect_size.height);
	}*/
	M = getRotationMatrix2D(rect.center, angle, 1.0);
	warpAffine(ninja_hsv, rotated, M, ninja_hsv.size(), INTER_CUBIC);
	imshow("rotated", rotated);
	getRectSubPix(rotated, rect_size, rect.center, cropped);
	imshow("cropped", cropped);
	

	//Création des masques de superposition
	Mat MaskBgnd6, MaskBgnd5, MaskBgnd4, MaskBgnd3, MaskBgnd2, MaskBgndFond, MaskRound, Maskcarreblack, Maskninja, Maskcropped,Maskcroppedtmp;

	//Création des masques en supprimant la couleur rouge (0,255,255 en HSV)
	inRange(fond_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), MaskBgndFond);
	inRange(bgnd6_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), MaskBgnd6);
	inRange(bgnd5_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), MaskBgnd5);
	inRange(bgnd4_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), MaskBgnd4);
	inRange(bgnd3_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), MaskBgnd3);
	inRange(bgnd2_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), MaskBgnd2);
	inRange(round_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), MaskRound);
	inRange(ninja_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), Maskninja);
	inRange(rotated, Scalar(0, 0, 0), Scalar(0, 0, 0), Maskcropped);
	imshow("M", Maskcropped);
	inRange(rotated, Scalar(0, 255, 255), Scalar(42, 255, 255), Maskcroppedtmp);
	dilate(Maskcroppedtmp, Maskcroppedtmp, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
	imshow("M1e", Maskcroppedtmp);
	erode(Maskcroppedtmp, Maskcroppedtmp, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(1, 1)));
	imshow("M1", Maskcroppedtmp);
	Maskcropped = Maskcropped | Maskcroppedtmp;
	imshow("Maskcropped2", Maskcropped);

	//Maskcropped = Maskcroppedtmp & Maskcropped;
	//imshow("M3", Maskcropped);

	//inversion des masques créés (NOIR >> BLANC && BLANC >> NOIR)
	bitwise_not(MaskBgnd2, MaskBgnd2);
	bitwise_not(MaskBgnd3, MaskBgnd3);
	bitwise_not(MaskBgnd4, MaskBgnd4);
	bitwise_not(MaskBgnd5, MaskBgnd5);
	bitwise_not(MaskBgnd6, MaskBgnd6);
	bitwise_not(MaskBgndFond, MaskBgndFond);
	bitwise_not(MaskRound, MaskRound);
	bitwise_not(Maskninja, Maskninja);
	bitwise_not(Maskcropped, Maskcropped);

	//positionnement dans le rectangle intérieur du fond (équivalent de imgsetROI)

	int i = 0;
	while (i<30) {
		Mat fondninja = subrendu(cvRect(posx+i, posy, 46, 48));
		rendu_hsv.copyTo(subrendu);
		bgnd6_hsv.copyTo(subrendu, MaskBgnd6);
		bgnd5_hsv.copyTo(subrendu, MaskBgnd5);
		bgnd4_hsv.copyTo(subrendu, MaskBgnd4);
		bgnd3_hsv.copyTo(subrendu, MaskBgnd3);
		cropped.copyTo(fondninja,Maskcropped);
		bgnd2_hsv.copyTo(subrendu, MaskBgnd2);
		
		round_hsv.copyTo(fond_hsv, MaskRound);
		cvtColor(fond_hsv, Rendufinal, COLOR_HSV2BGR);

		imshow("Game", Rendufinal);
		waitKey(1);
		//destroyAllWindows;
		i++;
	}
	
	
	cvtColor(fond_hsv, Rendufinal, COLOR_HSV2BGR);
	
	cv::waitKey(0);   //l'appui sur n'importe quelle touche fera interrompre le programme

	return(0);
}	