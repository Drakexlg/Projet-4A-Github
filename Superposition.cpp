#include<opencv2/opencv.hpp>
#include"opencv2\highgui.hpp"
#include"opencv2\imgproc.hpp"
#include"opencv2\imgcodecs.hpp"
#include<iostream>
#include<conio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include <mutex>
#include "ninja.h"
#include <chrono>
#include <string.h>
#include <cmath>
using namespace std;
using namespace cv;

/*===============================================================================================
=======================================VARIABLES GLOBALES========================================
=================================================================================================*/
Mat fond_hsv, bgnd6_hsv, bgnd5_hsv, bgnd4_hsv, bgnd3_hsv, bgnd2_hsv, rendu_hsv, round_hsv, ninja_hsv;
Mat MaskBgnd6, MaskBgnd5, MaskBgnd4, MaskBgnd3, MaskBgnd2, MaskBgndFond, MaskRound, Maskcarreblack, Maskninja, MaskcroppedD, Maskcroppedtmp, MaskcroppedG;
Mat Rendufinal, subrendu;
ninja ninja1, ninja2, ninja3;
int rnd;


Mat create_HSV(string path);
Mat create_mask(Mat img, int l_b, int l_g, int l_r, int h_b, int h_g, int h_r);
void update_pos(int fct, ninja *ninja_);

Mat remove_noise(Mat img, int a, int b, int c, int d);
Mat contours_(Mat img, Mat diff);


int main() {
	//variables globales :


	
	srand(time(NULL));
	rand();
	rnd = rand() % 3;
	//Création d'objets pouvant contenir une matrice de pixels (une image) et permettant la modification de celle-ci

	/*int posx1 = 95, posy1 = 280;
	int posx2 = 329, posy2 = 271;
	int posx3 = 850, posy3 = 324;*/

	/*======================================================================================================================
	=========================================CREATION DES TEXTURES ET DES MASQUES===========================================
	=======================================================================================================================*/

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

	subrendu = fond_hsv(cvRect(64, 36, 1152, 648));
	RotatedRect rect = RotatedRect(Point2f(23,24 ), Size2f(46, 48), -60);
	Mat M, rotatedD, rotatedG;
	float angle = rect.angle;
	Size rect_size = rect.size;
	M = getRotationMatrix2D(rect.center, angle, 1.0);
	warpAffine(ninja_hsv, rotatedD, M, ninja_hsv.size(), INTER_CUBIC);

	//Création des masques de superposition
	

	//Création des masques en supprimant la couleur rouge (0,255,255 en HSV)
	MaskBgndFond = create_mask(fond_hsv,0,255,255,0,255,255);
	MaskBgnd6 = create_mask(bgnd6_hsv, 0, 255, 255, 0, 255, 255);
	MaskBgnd5 = create_mask(bgnd5_hsv, 0, 255, 255, 0, 255, 255);
	MaskBgnd4 = create_mask(bgnd4_hsv, 0, 255, 255, 0, 255, 255);
	MaskBgnd3 = create_mask(bgnd3_hsv, 0, 255, 255, 0, 255, 255);
	MaskBgnd2 = create_mask(bgnd2_hsv, 0, 255, 255, 0, 255, 255);
	MaskRound = create_mask(round_hsv, 0, 255, 255, 0, 255, 255);
	Maskninja = create_mask(ninja_hsv, 0, 255, 255, 0, 255, 255);

	inRange(rotatedD, Scalar(0, 0, 0), Scalar(0, 0, 0), MaskcroppedD);
	inRange(rotatedD, Scalar(0, 255, 255), Scalar(42, 255, 255), Maskcroppedtmp);
	dilate(Maskcroppedtmp, Maskcroppedtmp, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
	erode(Maskcroppedtmp, Maskcroppedtmp, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(1, 1)));
	MaskcroppedD = MaskcroppedD | Maskcroppedtmp;

	//inversion des masques créés (NOIR >> BLANC && BLANC >> NOIR)
	bitwise_not(MaskcroppedD, MaskcroppedD);
	flip(MaskcroppedD, MaskcroppedG,1);
	flip(rotatedD, rotatedG, 1);

	/*=======================================================================================================================
	====================================FIN DE CREATION DES TEXTURES ET DES MASQUES==========================================
	=======================================================================================================================*/

	//ninja(posx,posy,plan,etat,dir,texture,mask)
	//plan : derrière quel plan est le calque
	//etat : 0 -> rentré     1 -> sorti
	//dir : 0 -> sort à droite   1 -> sort au dessus   2-> sort à gauche
	//texture : rotatedD   ninja_hsv    rotatedG
	//Mask: MaskcroppedD   Maskninja   MaskcroppedG

	ninja1 = ninja(95, 280, 2, 0, 0, rotatedD,MaskcroppedD);
	ninja2 = ninja(329, 271, 3, 0, 1, ninja_hsv,Maskninja);
	ninja3 = ninja(850, 324, 4, 0, 2, rotatedG,MaskcroppedG);
	
	/*Partie code en dehors des initialisations*/


	/*switch (rnd) {
	case 0:
		update_pos(0, &ninja1);
		update_pos(1, &ninja1);
		break;
	case 1:
		update_pos(0, &ninja2);
		update_pos(1, &ninja2);
		break;
	case 2:
		update_pos(0, &ninja3);
		update_pos(1, &ninja3);
		break;
	}

	cout << rnd << endl;
	cv::waitKey(0);   //l'appui sur n'importe quelle touche fera interrompre le programme
	*/
	
	
	
	mutex lock;
	//thread t1(update_pos, 0, &ninja1);
	thread t1([&lock]() {
		lock.lock();
		switch (rnd) {
		case 0:
			update_pos(0, &ninja1);
			//update_pos(1, &ninja1);
			break;
		case 1:
			update_pos(0, &ninja2);
			//update_pos(1, &ninja2);
			break;
		default:
			update_pos(0, &ninja3);
			//update_pos(1, &ninja3);
			break;
		}
	});
	/*thread t2([&lock]() {
		lock.lock();
		clock_t start = time(0);
		clock_t t2 =0;
		while ((t2-start) < 5000) {
			clock_t t2 = time(0);
		}
		lock.unlock();
	});
	thread t3([&lock]() {
		lock.lock();
		switch (rnd) {
		case 0:
			update_pos(1, &ninja1);
			break;
		case 1:
			update_pos(1, &ninja2);
			break;
		case 2:
			update_pos(1, &ninja3);
			break;
		}
		lock.unlock();
	});*/
	t1.join();
	//t2.join();
	//t3.join();
	

	cout << "return 0";

	return 0;
}	




//fonction permettant la récupération d'une image et sa conversion en ayant le chemin de l'image à récupérer (argument de fonction nommé path)
Mat create_HSV(string path) {
	Mat tmp;
	tmp = imread(path);
	cvtColor(tmp, tmp, COLOR_BGR2HSV);
	return tmp;
}

// fonction qui permet de créer un masque de type Mat en utilisant une image de type Mat en argument 
Mat create_mask(Mat img, int l_b, int l_g, int l_r, int h_b, int h_g, int h_r) {
	Mat tmp;
	inRange(img, Scalar(l_b, l_g, l_r), Scalar(h_b, h_g, h_r), tmp); //0.255.255   0.255.255
	bitwise_not(tmp, tmp);
	return tmp;
}

void update_pos(int fct, ninja *ninja_) {
	cout << "enter";
	cvNamedWindow("Game", CV_WINDOW_AUTOSIZE);
	moveWindow("Game", 500, -32);
	cout << "enter";
	int i = 0;
	int ninjaplan = ninja_->getplan();
	int ninjadir = ninja_->getdir();
	int ninjaposx = ninja_->getposx();
	int ninjaposy = ninja_->getposy();
	Mat ninjatext = ninja_->gettexture();
	Mat ninjamask = ninja_->getmask();
	if (fct == 0) //0 -> sortir un ninja
	{
		cout << "sortir";
		switch (ninjadir) {
		case 0:  //si le ninja sort à droite   (posx++)
			cout << "droite";
			while (i<30) {
				ninjaposx++;
				ninja_->setposx(ninjaposx);
				Mat fondninja = subrendu(cvRect(ninjaposx, ninjaposy, 46, 48));
				rendu_hsv.copyTo(subrendu);
				bgnd6_hsv.copyTo(subrendu, MaskBgnd6);
				if (ninjaplan == 5) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd5_hsv.copyTo(subrendu, MaskBgnd5);
				if (ninjaplan == 4) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd4_hsv.copyTo(subrendu, MaskBgnd4);
				if (ninjaplan == 3) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd3_hsv.copyTo(subrendu, MaskBgnd3);
				if (ninjaplan == 2) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd2_hsv.copyTo(subrendu, MaskBgnd2);

				round_hsv.copyTo(fond_hsv, MaskRound);
				cvtColor(fond_hsv, Rendufinal, COLOR_HSV2BGR);
				cout << "affichage";
				imshow("Game", Rendufinal);
				cout << "postaffichage";
				waitKey(1);
				i++;
			}
			break;
		case 1: //si le ninja sort en haut   (posy--)
			cout << "haut";
			while (i<30) {
				ninjaposy--;
				ninja_->setposy(ninjaposy);
				Mat fondninja = subrendu(cvRect(ninjaposx, ninjaposy, 46, 48));
				rendu_hsv.copyTo(subrendu);
				bgnd6_hsv.copyTo(subrendu, MaskBgnd6);
				if (ninjaplan == 5) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd5_hsv.copyTo(subrendu, MaskBgnd5);
				if (ninjaplan == 4) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd4_hsv.copyTo(subrendu, MaskBgnd4);
				if (ninjaplan == 3) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd3_hsv.copyTo(subrendu, MaskBgnd3);
				if (ninjaplan == 2) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd2_hsv.copyTo(subrendu, MaskBgnd2);

				round_hsv.copyTo(fond_hsv, MaskRound);
				cvtColor(fond_hsv, Rendufinal, COLOR_HSV2BGR);

				imshow("Game", Rendufinal);
				waitKey(1);
				i++;
			}
			break;
		default: //si le ninja sort à gauche   (posx--)
			cout << "gauche";
			while (i<30) {
				ninjaposx--;
				ninja_->setposx(ninjaposx);
				Mat fondninja = subrendu(cvRect(ninjaposx, ninjaposy, 46, 48));
				rendu_hsv.copyTo(subrendu);
				bgnd6_hsv.copyTo(subrendu, MaskBgnd6);
				if (ninjaplan == 5) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd5_hsv.copyTo(subrendu, MaskBgnd5);
				if (ninjaplan == 4) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd4_hsv.copyTo(subrendu, MaskBgnd4);
				if (ninjaplan == 3) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd3_hsv.copyTo(subrendu, MaskBgnd3);
				if (ninjaplan == 2) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd2_hsv.copyTo(subrendu, MaskBgnd2);

				round_hsv.copyTo(fond_hsv, MaskRound);
				cvtColor(fond_hsv, Rendufinal, COLOR_HSV2BGR);

				imshow("Game", Rendufinal);
				waitKey(1);
				i++;
			}
			break;
		}
	}
	else  //1 -> rentrer un ninja
	{
		cout << "rentrer";
		switch (ninjadir) {
		case 0:  //si le ninja rentre à gauche   (posx--)
			while (i<30) {
				ninjaposx--;
				ninja_->setposx(ninjaposx);
				Mat fondninja = subrendu(cvRect(ninjaposx, ninjaposy, 46, 48));
				rendu_hsv.copyTo(subrendu);
				bgnd6_hsv.copyTo(subrendu, MaskBgnd6);
				if (ninjaplan == 5) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd5_hsv.copyTo(subrendu, MaskBgnd5);
				if (ninjaplan == 4) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd4_hsv.copyTo(subrendu, MaskBgnd4);
				if (ninjaplan == 3) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd3_hsv.copyTo(subrendu, MaskBgnd3);
				if (ninjaplan == 2) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd2_hsv.copyTo(subrendu, MaskBgnd2);

				round_hsv.copyTo(fond_hsv, MaskRound);
				cvtColor(fond_hsv, Rendufinal, COLOR_HSV2BGR);

				imshow("Game", Rendufinal);
				waitKey(1);
				i++;
			}
			break;
		case 1: //si le ninja tentre en bas   (posy++)
			while (i<30) {
				ninjaposy++;
				ninja_->setposy(ninjaposy);
				Mat fondninja = subrendu(cvRect(ninjaposx, ninjaposy, 46, 48));
				rendu_hsv.copyTo(subrendu);
				bgnd6_hsv.copyTo(subrendu, MaskBgnd6);
				if (ninjaplan == 5) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd5_hsv.copyTo(subrendu, MaskBgnd5);
				if (ninjaplan == 4) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd4_hsv.copyTo(subrendu, MaskBgnd4);
				if (ninjaplan == 3) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd3_hsv.copyTo(subrendu, MaskBgnd3);
				if (ninjaplan == 2) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd2_hsv.copyTo(subrendu, MaskBgnd2);

				round_hsv.copyTo(fond_hsv, MaskRound);
				cvtColor(fond_hsv, Rendufinal, COLOR_HSV2BGR);

				imshow("Game", Rendufinal);
				waitKey(1);
				i++;
			}
			break;
		default: //si le ninja rentre à droite   (posx++)
			while (i<30) {
				ninjaposx++;
				ninja_->setposx(ninjaposx);
				Mat fondninja = subrendu(cvRect(ninjaposx, ninjaposy, 46, 48));
				rendu_hsv.copyTo(subrendu);
				bgnd6_hsv.copyTo(subrendu, MaskBgnd6);
				if (ninjaplan == 5) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd5_hsv.copyTo(subrendu, MaskBgnd5);
				if (ninjaplan == 4) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd4_hsv.copyTo(subrendu, MaskBgnd4);
				if (ninjaplan == 3) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd3_hsv.copyTo(subrendu, MaskBgnd3);
				if (ninjaplan == 2) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
				bgnd2_hsv.copyTo(subrendu, MaskBgnd2);

				round_hsv.copyTo(fond_hsv, MaskRound);
				cvtColor(fond_hsv, Rendufinal, COLOR_HSV2BGR);

				imshow("Game", Rendufinal);
				waitKey(1);
				i++;
			}
			break;
		}
	}
	cout << "done";
}


/*==========================FONCTIONS CODE GUILLAUME=========================*/


//--------------------------------Suppression du bruit------------------------------------------------------
Mat remove_noise(Mat img, int a, int b, int c, int d)
{
	Mat difference;
	threshold(img, difference, 200, 255, CV_8UC1);
	//erode : permet de changer la couleur (noir ou blanc) d'un pixel en fonction de la couleur des pixels voisins (si majorité de pixel noir -> le pixel devient noir; si majorité de pixel blanc -> le pixel devient blanc)
	// l'effet de la zone peut etre changer avec la valeur du cv::Size
	erode(difference, difference, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(a, a)));  //supprime les points/petite zone de pixels qui sont des faux positifs et réduit la taille des zones à reperer 
																								  //dilate : augmente la taille des pixels noirs 
	dilate(difference, difference, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(b, b))); //reaugmente la taille des zones a reccuperer

																								  //permet d'éviter que 1 zone soit découper en 2 sous zones
	dilate(difference, difference, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(c, c)));//augmente la taille des zones pour n'en former qu'une.
	erode(difference, difference, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(d, d)));//diminue la taille pour revenir approximativement à la tille d'origine
																								//cv::imshow("diff0", difference);

	return difference;
}



//--------------------------------Detection ds contours des formes puis des coordonnées du centre de gravité de la forme------------------------------------------------------
Mat contours_(Mat img, Mat diff)
{
	std::vector<std::vector<cv::Point> > contours;
	//Determine les contours des formes noires de l'image diff et les place dans contours
	cv::findContours(diff, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	/// Get the moments
	std::vector<cv::Moments> mu(contours.size());
	//calcul du moments pour chaque "point"
	for (int i = 1; i < contours.size(); i++)
	{
		if (arcLength(contours[i], true) > 30)
		{
			mu[i] = moments(contours[i], false);
		}
	}

	///  Obtention du centre d'inertie (milieu du cercle) pour chaque "point"
	std::vector<cv::Point2f> mc(contours.size());

	for (int i = 1; i < 5; i++)
	{
		mc[i] = cv::Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);

	}

	cv::Point2f inputQuad[4];  //Coordonnees selectionnée dans l'image recu par la webcam

	cv::Point2f outputQuad[4];//Dimension de la video de sortie
	outputQuad[0] = cv::Point2f(0, 0);
	outputQuad[1] = cv::Point2f(img.cols, 0);
	outputQuad[2] = cv::Point2f(img.cols, img.rows);
	outputQuad[3] = cv::Point2f(0, img.rows);
	std::vector<cv::Point2f> mclasse(4);

	//determination de l'emplacement de chaque point
	int cpt_norm[4];
	cv::Mat little_frame;
	double norm[4];
	for (int i = 1; i <= 4; i = i + 1)
	{
		norm[i - 1] = sqrt(mc[i].x*mc[i].x + mc[i].y*mc[i].y); //calcul de la norme du vecteur entre chaque point et l'origine de l'image. 
	}

	//classement des points en fonction de la dimention des normes;
	for (int i = 1; i <= 4; i = i + 1)
	{
		cpt_norm[0] = 0;
		cpt_norm[1] = 0;
		cpt_norm[2] = 0;
		cpt_norm[3] = 0;
		for (int j = 1; j <= 4; j++)
		{
			if (norm[i - 1] > norm[j - 1])
			{
				cpt_norm[i - 1] = cpt_norm[i - 1] + 1;
			}
		}
		std::cout << cpt_norm[i - 1] << "&";
		if (cpt_norm[i - 1] == 3)
		{
			mclasse[0] = mc[i];
		}
		else if (cpt_norm[i - 1] == 0)
		{
			mclasse[3] = mc[i];
		}
		else if (cpt_norm[i - 1] == 1)
		{
			mclasse[1] = mc[i];
		}
		else if (cpt_norm[i - 1] == 2)
		{
			mclasse[2] = mc[i];
		}
	}

	//-------------------Découpage et remise en forme de l'image;

	//Repartition des centres d'inertie pour les adapter à la fonctions
	inputQuad[0] = mclasse[3];
	inputQuad[1] = mclasse[2];
	inputQuad[2] = mclasse[0];
	inputQuad[3] = mclasse[1];

	cv::Mat M;
	//Transforme l'image délimité par les points de l'inputquad pour les faire correspondre à ceux de l'outputquad
	M = getPerspectiveTransform(inputQuad, outputQuad);

	warpPerspective(img, little_frame, M, little_frame.size());
	return little_frame;
}