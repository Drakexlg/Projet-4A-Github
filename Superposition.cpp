#include<opencv2/opencv.hpp>
#include<iostream>
#include<conio.h>
#include"opencv2\highgui.hpp"
#include"opencv2\imgproc.hpp"
#include <windows.h>

using namespace std;
using namespace cv;
///////////////////////////////////////////////////////////////////////////////////////////////////

//fonction permettant la récupération d'une image et sa conversion en ayant le chemin de l'image à récupérer (argument de fonction nommé path)
Mat create_HSV(string path) {
	Mat tmp;
	tmp = imread(path);
	cvtColor(tmp, tmp, COLOR_BGR2HSV);
	return tmp;
}

// fonction qui permet de créer un masque de type Mat en utilisant une image de type Mat en argument
Mat create_mask(Mat img) {
	Mat tmp;
	inRange(img, Scalar(0, 255, 255), Scalar(0, 255, 255),tmp);
	bitwise_not(tmp, tmp);
	return tmp;
}

int main() {
	//Création d'objets pouvant contenir une matrice de pixels (une image) et permettant la modification de celle-ci
	Mat Rendu, imgBgnd6, imgBgnd5, imgBgnd4, imgBgnd3, imgBgnd2, imgFond, ronds, ninja, Rendufinal;
	Mat fond_hsv, bgnd6_hsv, bgnd5_hsv, bgnd4_hsv, bgnd3_hsv, bgnd2_hsv, rendu_hsv, round_hsv, ninja_hsv;
	int posx = 95, posy = 280;
	cvNamedWindow("Game", CV_WINDOW_AUTOSIZE);
	moveWindow("Game", 500, -32);  //décalalge de l'image pour l'ajuster parfaitement au vidéoprojecteur (type : 1920,-32)
	//copie dans les objets de type IplImage le contenu des images situés à l'adresse indiquée
	rendu_hsv = create_HSV("Textures/bgnd1.png");
	fond_hsv = create_HSV("Textures/black_bgnd.png");
	bgnd2_hsv = create_HSV("Textures/bgnd2.png");
	bgnd3_hsv = create_HSV("Textures/bgnd3.png");
	bgnd4_hsv = create_HSV("Textures/bgnd4.png");
	bgnd5_hsv = create_HSV("Textures/bgnd5.png");
	bgnd6_hsv = create_HSV("Textures/bgnd6.png");
	round_hsv = create_HSV("Textures/ronds.png");
	ninja_hsv = create_HSV("Textures/ninja.png");

	Mat subrendu = fond_hsv(cvRect(64, 36, 1152, 648));
	RotatedRect rect = RotatedRect(Point2f(23,24 ), Size2f(46, 48), -60);
	Mat M, rotated;
	float angle = rect.angle;
	Size rect_size = rect.size;
	M = getRotationMatrix2D(rect.center, angle, 1.0);
	warpAffine(ninja_hsv, rotated, M, ninja_hsv.size(), INTER_CUBIC);

	//Création des masques de superposition
	Mat MaskBgnd6, MaskBgnd5, MaskBgnd4, MaskBgnd3, MaskBgnd2, MaskBgndFond, MaskRound, Maskcarreblack, Maskninja, Maskcropped,Maskcroppedtmp;

	//Création des masques en supprimant la couleur rouge (0,255,255 en HSV)
	MaskBgndFond = create_mask(fond_hsv);
	MaskBgnd6 = create_mask(bgnd6_hsv);
	MaskBgnd5 = create_mask(bgnd5_hsv);
	MaskBgnd4 = create_mask(bgnd4_hsv);
	MaskBgnd3 = create_mask(bgnd3_hsv);
	MaskBgnd2 = create_mask(bgnd2_hsv);
	MaskRound = create_mask(round_hsv);
	Maskninja = create_mask(ninja_hsv);

	inRange(rotated, Scalar(0, 0, 0), Scalar(0, 0, 0), Maskcropped);
	inRange(rotated, Scalar(0, 255, 255), Scalar(42, 255, 255), Maskcroppedtmp);
	dilate(Maskcroppedtmp, Maskcroppedtmp, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
	erode(Maskcroppedtmp, Maskcroppedtmp, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(1, 1)));
	Maskcropped = Maskcropped | Maskcroppedtmp;

	//inversion des masques créés (NOIR >> BLANC && BLANC >> NOIR)
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
		rotated.copyTo(fondninja,Maskcropped);
		bgnd2_hsv.copyTo(subrendu, MaskBgnd2);

		round_hsv.copyTo(fond_hsv, MaskRound);
		cvtColor(fond_hsv, Rendufinal, COLOR_HSV2BGR);

		imshow("Game", Rendufinal);
		waitKey(1);
		//destroyAllWindows;
		i++;
	}


	//cvtColor(fond_hsv, Rendufinal, COLOR_HSV2BGR);

	cv::waitKey(0);   //l'appui sur n'importe quelle touche fera interrompre le programme

	return(0);
}
