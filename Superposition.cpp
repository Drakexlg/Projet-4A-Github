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
	ninja = imread("Textures/ninja1.png");

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

	//Création des masques de superposition
	Mat MaskBgnd6_HSV, MaskBgnd5_HSV, MaskBgnd4_HSV, MaskBgnd3_HSV, MaskBgnd2_HSV, MaskBgndFond_HSV, MaskRound_hsv, Maskcarreblack_hsv, Maskninja_hsv;

	//Création des masques en supprimant la couleur rouge (0,255,255 en HSV)
	inRange(fond_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), MaskBgndFond_HSV);
	inRange(bgnd6_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), MaskBgnd6_HSV);
	inRange(bgnd5_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), MaskBgnd5_HSV);
	inRange(bgnd4_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), MaskBgnd4_HSV);
	inRange(bgnd3_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), MaskBgnd3_HSV);
	inRange(bgnd2_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), MaskBgnd2_HSV);
	inRange(round_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), MaskRound_hsv);
	inRange(ninja_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), Maskninja_hsv);

	imshow("ninja", Maskninja_hsv);
	//inRange(carreblack_hsv, Scalar(0, 255, 255), Scalar(0, 255, 255), Maskcarreblack_hsv);

	//inversion des masques créés (NOIR >> BLANC && BLANC >> NOIR)
	bitwise_not(MaskBgnd2_HSV, MaskBgnd2_HSV);
	bitwise_not(MaskBgnd3_HSV, MaskBgnd3_HSV);
	bitwise_not(MaskBgnd4_HSV, MaskBgnd4_HSV);
	bitwise_not(MaskBgnd5_HSV, MaskBgnd5_HSV);
	bitwise_not(MaskBgnd6_HSV, MaskBgnd6_HSV);
	bitwise_not(MaskBgndFond_HSV, MaskBgndFond_HSV);
	bitwise_not(MaskRound_hsv, MaskRound_hsv);
	bitwise_not(Maskninja_hsv, Maskninja_hsv);

	//positionnement dans le rectangle intérieur du fond (équivalent de imgsetROI)
	Mat subrendu = fond_hsv(cvRect(64, 36, 1152, 648));
	int i = 0;
	while (i<40) {
		Mat fondninja = subrendu(cvRect(posx+i, posy, 48, 46));
		rendu_hsv.copyTo(subrendu);
		bgnd6_hsv.copyTo(subrendu, MaskBgnd6_HSV);
		bgnd5_hsv.copyTo(subrendu, MaskBgnd5_HSV);
		bgnd4_hsv.copyTo(subrendu, MaskBgnd4_HSV);
		bgnd3_hsv.copyTo(subrendu, MaskBgnd3_HSV);
		ninja_hsv.copyTo(fondninja,Maskninja_hsv);
		bgnd2_hsv.copyTo(subrendu, MaskBgnd2_HSV);
		
		round_hsv.copyTo(fond_hsv, MaskRound_hsv);
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