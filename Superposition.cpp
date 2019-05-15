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
Mat fond_hsv, bgnd6_hsv, bgnd5_hsv, bgnd4_hsv, bgnd3_hsv, bgnd2_hsv, rendu_hsv, round_hsv, ninja_hsv, ninja_t_hsv;
Mat menu_main_hsv, menu_settings_hsv, menu_diff_hard_hsv, menu_diff_medium_hsv, menu_diff_easy_hsv, menu_score_hsv;
Mat MaskBgnd6, MaskBgnd5, MaskBgnd4, MaskBgnd3, MaskBgnd2, MaskBgndFond, MaskRound, Maskcarreblack, Maskninja, MaskcroppedD, Maskcroppedtmp, MaskcroppedG, Maskninja_t;
Mat Rendufinal, subrendu;
ninja ninja1, ninja2, ninja3, ninja4, ninja5, ninja6, ninja7, ninja8, ninja9;
int rnd=0, touche = 0, score = 0;
string strscore;

Mat create_HSV(string path);
Mat create_mask(Mat img, int l_b, int l_g, int l_r, int h_b, int h_g, int h_r);
void update_pos(int fct, ninja *ninja_);

Mat remove_noise(Mat img, int a, int b, int c, int d);
Mat contours_(Mat img, Mat diff);
void recup_coords();

cv::VideoCapture stream1(0);   //0 est l'id de la caméra à utiliser 
int laser_x, laser_y;
int nbninja = 0;

Point a(100, 67);  //positionnement de la zone de texte du score en partie
Point b(590, 243);  //positionnement de la zone de texte du score dens le menu des scores
Scalar colortext(125, 255, 0);  //couleur du texte des scores

int main() {
	strscore = to_string(score);
	cvNamedWindow("Game", CV_WINDOW_AUTOSIZE);
	moveWindow("Game", 1900, -27);  //permet de mettre l'écran au centre de l'image (changer la variable 500 en fonction de la résolution)  //-32
	//variables globales :
	laser_x = 400;
	laser_y = 400;
	bool partie = true; //variable de boucle du programme entier (false que si la caméra n'est pas branchée)
	bool jeu = false; //variable de boucle du jeu
	bool laser = false; //variable de boucle de récupération des coordonnées laser
	bool menu = true; //variable de boucle du menu
	bool settings = false; //variable de boucle des réglages
	bool menu_score = false; //variable de menu des scores
	int ninja_actif = 0;
	int tempo_out = 0, tempo_max = 100; //temporisation courante et temporisation max de sortie
	int nbninjamax = 20; //nombre de ninjas courants et nombre maximum de ninjas qui apparaitront au cours d'une partie
	int level = 1; //niveau de difficulté : 0=easy   1=medium   2=hard

	srand(time(NULL));
	rand();

	/*======================================================================================================================
	=========================================CREATION DES TEXTURES ET DES MASQUES===========================================
	=======================================================================================================================*/

	//copie dans les objets de type IplImage le contenu des images situés à l'adresse indiquée et les convertit en hsv
	rendu_hsv = create_HSV("Textures/bgnd1u.png");
	fond_hsv = create_HSV("Textures/black_bgnd.png");
	bgnd2_hsv = create_HSV("Textures/bgnd2.png");
	bgnd3_hsv = create_HSV("Textures/bgnd3.png");
	bgnd4_hsv = create_HSV("Textures/bgnd4.png");
	bgnd5_hsv = create_HSV("Textures/bgnd5u.png");
	bgnd6_hsv = create_HSV("Textures/bgnd6u.png");
	round_hsv = create_HSV("Textures/ronds.png");
	ninja_hsv = create_HSV("Textures/ninja.png");
	ninja_t_hsv = create_HSV("Textures/ninja_t.png");
	menu_main_hsv = create_HSV("Textures/menu1.png");
	menu_diff_hard_hsv = create_HSV("Textures/menu_diff_hard1.png");
	menu_diff_easy_hsv = create_HSV("Textures/menu_diff_easy1.png");
	menu_diff_medium_hsv = create_HSV("Textures/menu_diff_medium1.png");
	menu_score_hsv = create_HSV("Textures/menu_score1.png");

	subrendu = fond_hsv(cvRect(64, 36, 1152, 648));  //représente la partie centrale de l'écran sans les bords noirs

	//création des textures des ninjas inclinés
	RotatedRect rect = RotatedRect(Point2f(23, 24), Size2f(46, 48), -60);
	Mat M, rotatedD, rotatedG;
	float angle = rect.angle;
	Size rect_size = rect.size;
	M = getRotationMatrix2D(rect.center, angle, 1.0);
	warpAffine(ninja_hsv, rotatedD, M, ninja_hsv.size(), INTER_CUBIC);

	//Création des masques de superposition en supprimant la couleur rouge (0,255,255 en HSV)
	MaskBgndFond = create_mask(fond_hsv, 0, 255, 255, 0, 255, 255);
	MaskBgnd6 = create_mask(bgnd6_hsv, 0, 255, 255, 0, 255, 255);
	MaskBgnd5 = create_mask(bgnd5_hsv, 0, 255, 255, 0, 255, 255);
	MaskBgnd4 = create_mask(bgnd4_hsv, 0, 255, 255, 0, 255, 255);
	MaskBgnd3 = create_mask(bgnd3_hsv, 0, 255, 255, 0, 255, 255);
	MaskBgnd2 = create_mask(bgnd2_hsv, 0, 255, 255, 0, 255, 255);
	MaskRound = create_mask(round_hsv, 0, 255, 255, 0, 255, 255);
	Maskninja = create_mask(ninja_hsv, 0, 255, 255, 0, 255, 255);
	Maskninja_t = create_mask(ninja_t_hsv, 0, 255, 255, 0, 255, 255);

	inRange(rotatedD, Scalar(0, 0, 0), Scalar(0, 0, 0), MaskcroppedD);
	inRange(rotatedD, Scalar(0, 255, 255), Scalar(42, 255, 255), Maskcroppedtmp);
	dilate(Maskcroppedtmp, Maskcroppedtmp, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
	erode(Maskcroppedtmp, Maskcroppedtmp, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(1, 1)));
	MaskcroppedD = MaskcroppedD | Maskcroppedtmp;

	//inversion des masques créés (NOIR >> BLANC && BLANC >> NOIR)
	bitwise_not(MaskcroppedD, MaskcroppedD);
	flip(MaskcroppedD, MaskcroppedG, 1);
	flip(rotatedD, rotatedG, 1);

	/*=======================================================================================================================
	====================================FIN DE CREATION DES TEXTURES ET DES MASQUES==========================================
	=======================================================================================================================*/


	/*=======================================================================================================================
	==================================================CREATION DES NINJAS====================================================
	=======================================================================================================================*/

	//ninja(posx,posy,plan,etat,dir,texture,mask)
	//plan : derrière quel plan est le calque
	//etat : 0 -> rentré     1 -> sorti
	//dir : 0 -> sort à droite   1 -> sort au dessus   2-> sort à gauche
	//texture : rotatedD   ninja_hsv    rotatedG
	//Mask: MaskcroppedD   Maskninja   MaskcroppedG

	ninja1 = ninja(95, 280, 2, 0, 0, rotatedD, MaskcroppedD);
	ninja2 = ninja(329, 271, 3, 0, 1, ninja_hsv, Maskninja);
	ninja3 = ninja(850, 324, 4, 0, 2, rotatedG, MaskcroppedG);
	ninja4 = ninja(493, 347, 5, 0, 0, rotatedD, MaskcroppedD);
	ninja5 = ninja(840, 513, 2, 0, 1, ninja_hsv, Maskninja);
	ninja6 = ninja(595, 79, 5, 0, 1, ninja_hsv, Maskninja);
	ninja7 = ninja(232, 527, 2, 0, 1, ninja_hsv, Maskninja);
	ninja8 = ninja(270, 232, 5, 0, 1, ninja_hsv, Maskninja);
	ninja9 = ninja(810, 254, 6, 0, 2, rotatedG, MaskcroppedG);

	/*=======================================================================================================================
	=============================================FIN DE CREATION DES NINJAS==================================================
	=======================================================================================================================*/

	if (!stream1.isOpened()) { //vérifie si la caméra est bien branchée et initialisée
		std::cout << "cannot open camera";
		partie = false;
	}

	///Partie code en dehors des initialisations

	while(partie){
		while (menu) {
			//affichage du menu principal
			menu_main_hsv.copyTo(subrendu);
			round_hsv.copyTo(fond_hsv, MaskRound);
			cvtColor(fond_hsv, Rendufinal, COLOR_HSV2BGR);
			imshow("Game", Rendufinal);
			recup_coords();
			if (laser_x >= 339 && laser_x <= 812 && laser_y >= 196 && laser_y <= 288) {  //bouton jouer
				menu = false;
				jeu = true;
			}
			if (laser_x >= 339 && laser_x <= 812 && laser_y >= 343 && laser_y <= 435) {  //bouton paramètres
				menu = false;
				settings = true;
			}
			cout << "pm";
			waitKey(1);
		}

		while (settings) {
			recup_coords();
			if (laser_x >= 60 && laser_x <= 242 && laser_y >= 50 && laser_y <= 125) {  //si on touche la touche de retour au menu principal
				settings = false;
				menu = true;

			}
			else if (laser_x >= 224 && laser_x <= 319 && laser_y >= 269 && laser_y <= 361) {  //si on touche la touche difficulté -
				if (level > 0) {  //possible de baisser le niveau que si il est supérieur à facile 
					level--;
				}
			}
			else if (laser_x >= 850 && laser_x <= 945 && laser_y >= 269 && laser_y <= 361) {  //si on touche la touche difficulté +
				if (level < 2) {  //possible d'augmenter le niveau que si il est inférieur à difficile
					level++;
				}
			}
			

			switch (level) {
				case 0:  //difficulté facile 
					tempo_max = 100;  //temporisation de sortie des ninjas
					nbninjamax = 20;  //nombre maximum de ninjas
					//affichage de la variante facile du menu de difficulté
					menu_diff_easy_hsv.copyTo(subrendu);
					round_hsv.copyTo(fond_hsv, MaskRound);
					cvtColor(fond_hsv, Rendufinal, COLOR_HSV2BGR);
					imshow("Game", Rendufinal);
					break;
				case 1:  //difficulté Normal
					tempo_max = 50;  //temporisation de sortie des ninjas
					nbninjamax = 30;  //nombre maximum de ninjas
					//affichage de la variante normale du menu de difficulté
					menu_diff_medium_hsv.copyTo(subrendu);
					round_hsv.copyTo(fond_hsv, MaskRound);
					cvtColor(fond_hsv, Rendufinal, COLOR_HSV2BGR);
					imshow("Game", Rendufinal);
					break;
				case 2:  //difficulté Difficile
					tempo_max = 25;  //temporisation de sortie des ninjas
					nbninjamax = 40; //nombre maximum de ninjas
					//affichage de la variante difficile du menu de difficulté
					menu_diff_hard_hsv.copyTo(subrendu);
					round_hsv.copyTo(fond_hsv, MaskRound);
					cvtColor(fond_hsv, Rendufinal, COLOR_HSV2BGR);
					imshow("Game", Rendufinal);
					break;
				default:  //difficulté par défaut (normale)
					tempo_max = 50; //temporisation de sortie des ninjas par défaut
					nbninjamax = 30;  //nombre maximum de ninjas par défaut
					menu_diff_medium_hsv.copyTo(subrendu);
					round_hsv.copyTo(fond_hsv, MaskRound);
					cvtColor(fond_hsv, Rendufinal, COLOR_HSV2BGR);
					imshow("Game", Rendufinal);
					break;
				}
			waitKey(1);
		}

		//boucle du jeu qui tourne constamment jusqu'à ce que la variable jeu soit passée à false
		while (jeu) {
			laser = true;

			//randomisation du ninja à afficher sur l'écran
			rnd = rand() % 9;

			//affichage du ninja correspondant à la valeur randomisée

			switch (rnd) {
			case 0:
				update_pos(0, &ninja1);
				ninja_actif = 0;
				break;
			case 1:
				update_pos(0, &ninja2);
				ninja_actif = 1;
				break;
			case 2:
				update_pos(0, &ninja3);
				ninja_actif = 2;
				break;
			case 3:
				update_pos(0, &ninja4);
				ninja_actif = 3;
				break;
			case 4:
				update_pos(0, &ninja5);
				ninja_actif = 4;
				break;
			case 5:
				update_pos(0, &ninja6);
				ninja_actif = 5;
				break;
			case 6:
				update_pos(0, &ninja7);
				ninja_actif = 6;
				break;
			case 7:
				update_pos(0, &ninja8);
				ninja_actif = 7;
				break;
			case 8:
				update_pos(0, &ninja9);
				ninja_actif = 8;
				break;
			default:
				update_pos(0, &ninja9);
				ninja_actif = 8;
				break;
			}

			//boucle du jeu (coeur du jeu)
			while (laser) {
				recup_coords();
				tempo_out++;

				//vérifie si le laser a touché le ninja et gère la rentrée avec clignotement si c'est le cas
				switch (ninja_actif) {
				case 0:
					if (laser_x >= ninja1.getposx() && laser_x <= (ninja1.getposx() + 46) && laser_y >= ninja1.getposy() && laser_y <= (ninja1.getposy() + 48)) {
						score += 100;
						strscore = to_string(score);
						touche = 1;
						cout << "====================TOUCHE===================";
						update_pos(1, &ninja1);
						tempo_out = 0;
						touche = 0;
						laser = false;
					}
					break;
				case 1:
					if (laser_x >= ninja2.getposx() && laser_x <= (ninja2.getposx() + 46) && laser_y >= ninja2.getposy() && laser_y <= (ninja2.getposy() + 48)) {
						score += 100;
						strscore = to_string(score);
						touche = 1;
						cout << "====================TOUCHE===================";
						update_pos(1, &ninja2);
						tempo_out = 0;
						touche = 0;
						laser = false;
					}
					break;
				case 2:
					if (laser_x >= ninja3.getposx() && laser_x <= (ninja3.getposx() + 46) && laser_y >= ninja3.getposy() && laser_y <= (ninja3.getposy() + 48)) {
						score += 100;
						strscore = to_string(score);
						touche = 1;
						cout << "====================TOUCHE===================";
						update_pos(1, &ninja3);
						touche = 0;
						tempo_out = 0;
						laser = false;
					}
					break;
				case 3:
					if (laser_x >= ninja4.getposx() && laser_x <= (ninja4.getposx() + 46) && laser_y >= ninja4.getposy() && laser_y <= (ninja4.getposy() + 48)) {
						score += 100;
						strscore = to_string(score);
						touche = 1;
						cout << "====================TOUCHE===================";
						update_pos(1, &ninja4);
						touche = 0;
						tempo_out = 0;
						laser = false;
					}
					break;
				case 4:
					if (laser_x >= ninja5.getposx() && laser_x <= (ninja5.getposx() + 46) && laser_y >= ninja5.getposy() && laser_y <= (ninja5.getposy() + 48)) {
						score += 100;
						strscore = to_string(score);
						touche = 1;
						cout << "====================TOUCHE===================";
						update_pos(1, &ninja5);
						touche = 0;
						tempo_out = 0;
						laser = false;
					}
					break;
				case 5:
					if (laser_x >= ninja6.getposx() && laser_x <= (ninja6.getposx() + 46) && laser_y >= ninja6.getposy() && laser_y <= (ninja6.getposy() + 48)) {
						score += 100;
						strscore = to_string(score);
						touche = 1;
						cout << "====================TOUCHE===================";
						update_pos(1, &ninja6);
						touche = 0;
						tempo_out = 0;
						laser = false;
					}
					break;
				case 6:
					if (laser_x >= ninja7.getposx() && laser_x <= (ninja7.getposx() + 46) && laser_y >= ninja7.getposy() && laser_y <= (ninja7.getposy() + 48)) {
						score += 100;
						strscore = to_string(score);
						touche = 1;
						cout << "====================TOUCHE===================";
						update_pos(1, &ninja7);
						touche = 0;
						tempo_out = 0;
						laser = false;
					}
					break;
				case 7:
					if (laser_x >= ninja8.getposx() && laser_x <= (ninja8.getposx() + 46) && laser_y >= ninja8.getposy() && laser_y <= (ninja8.getposy() + 48)) {
						score += 100;
						strscore = to_string(score);
						touche = 1;
						cout << "====================TOUCHE===================";
						update_pos(1, &ninja8);
						touche = 0;
						tempo_out = 0;
						laser = false;
					}
					break;
				case 8:
					if (laser_x >= ninja9.getposx() && laser_x <= (ninja9.getposx() + 46) && laser_y >= ninja9.getposy() && laser_y <= (ninja9.getposy() + 48)) {
						score += 100;
						strscore = to_string(score);
						touche = 1;
						cout << "====================TOUCHE===================";
						update_pos(1, &ninja9);
						touche = 0;
						tempo_out = 0;
						laser = false;
					}
					break;
				}

				//si le laser n'a pas touché, la temporisation définit la durée au bout de laquelle le ninja rentrera
				if (tempo_out == tempo_max) {
					switch (ninja_actif) {
					case 0:
						update_pos(1, &ninja1);
						break;
					case 1:
						update_pos(1, &ninja2);
						break;
					case 2:
						update_pos(1, &ninja3);
						break;
					case 3:
						update_pos(1, &ninja4);
						break;
					case 4:
						update_pos(1, &ninja5);
						break;
					case 5:
						update_pos(1, &ninja6);
						break;
					case 6:
						update_pos(1, &ninja7);
						break;
					case 7:
						update_pos(1, &ninja8);
						break;
					case 8:
						update_pos(1, &ninja9);
						break;
					default:
						update_pos(1, &ninja9);
						break;
					}
					tempo_out = 0;
					laser = false;
				}
			}
			if (nbninja == (nbninjamax+1)) { //si le nombre de ninja maximum est atteint, on arrête la partie 
				menu_score = true;
				laser = false;
				jeu = false;
			}
			cout << nbninja;
		}
		
		while (menu_score) {
			recup_coords();
			menu_score_hsv.copyTo(subrendu);
			round_hsv.copyTo(fond_hsv, MaskRound);
			cvtColor(fond_hsv, Rendufinal, COLOR_HSV2BGR);
			putText(Rendufinal, strscore, b, FONT_HERSHEY_SIMPLEX, 0.9, colortext, 2, 8, false);
			imshow("Game", Rendufinal);
			waitKey(1);

			if (laser_x >= 438 && laser_x <= 713 && laser_y >= 365 && laser_y <= 439) { //si le laser touche le bouton Rejouer
				menu_score = false;
				score = 0; //réinitialisation du score
				nbninja = 0; //réinitialisation du nombre de ninjas sortis
				jeu = true;
			}
			else if (laser_x >= 438 && laser_x <= 713 && laser_y >= 476 && laser_y <= 550) { //si le laser touche le bouton Menu Principak
				menu_score = false;
				score = 0; //réinitialisation du score
				nbninja = 0; //réinitialisation du nombre de ninjas sortis
				menu = true;
			}
		}
	}
	cout << "return 0";

	return 0;
}

//fonction de récupération des coordonnées du laser dans les variables laser_x et laser_y
void recup_coords() {
	cv::Mat cameraFrame;
	Mat mask_;
	Mat recadrage;
	Mat red_color;
	stream1.read(cameraFrame);
	imshow("webcam", cameraFrame);
	mask_ = create_mask(cameraFrame, 200, 0, 200, 255, 180, 255);  // appel à la fonction mask qui renvoie les parties de l'image dont les couleurs sont dans la plage BGR indiqué
	mask_ = remove_noise(mask_, 5, 6, 5, 5);
	std::vector<std::vector<cv::Point> > contours;
	imshow("mask", mask_);
	cv::findContours(mask_, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	/// Get the moments
	//verification de la presence de 4 points distincts dans la vidéo capturé
	std::vector<cv::Moments> mu(contours.size());
	int cpt = 0;
	for (int i = 1; i < contours.size(); i++)
	{
		if (arcLength(contours[i], true) > 30)
		{
			cpt = cpt + 1;
			mu[cpt] = moments(contours[cpt], false);
		}
	}
	if (cpt == 4) {
		// utilisation de la fontion contours_ qui renvoi la partie de l'image correspondant à l'écran de jeu
		recadrage = contours_(cameraFrame, mask_);
		//imshow("recadrage", recadrage);


		//red_color = create_mask(recadrage, 100, 100, 165, 150, 150, 255);
		red_color = create_mask(recadrage, 40, 130, 165, 100, 170, 255);
		red_color = remove_noise(red_color, 5, 1, 1, 5); ///a modifier pour s'ajuster a la realité.
	//	red_color = remove_noise(red_color, 7, 2, 7, 5); ///a modifier pour s'ajuster a la realité.
	//	dilate(red_color, red_color, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7))); //reaugmente la taille des zones a reccuperer

		//erode(red_color, red_color, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));
		imshow("red", red_color);

		std::vector<std::vector<cv::Point> > r_contours;
		cv::findContours(red_color, r_contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
		/// Get the moments
		std::vector<cv::Moments> mu_(r_contours.size());

		for (int i = 1; i < r_contours.size(); i++)
		{
			if (arcLength(r_contours[i], true) > 1)   // mettre un chiffre etit ici --> contours petit!!
			{
				mu_[i] = moments(r_contours[i], false);
			}
		}

		if (r_contours.size() == 2)
		{
			std::vector<cv::Point2f> mc_(r_contours.size());
			std::vector<cv::Point2f> mcreel_(r_contours.size());
			mc_[1] = cv::Point2f(mu_[1].m10 / mu_[1].m00, mu_[1].m01 / mu_[1].m00);
			//mcreel_[1] = cv::Point2f((mc_[1].x * 648) / recadrage.rows, (mc_[1].y * 1152) / recadrage.cols);
			mcreel_[1] = cv::Point2f((mc_[1].x * 1152) / recadrage.cols, (mc_[1].y * 648) / recadrage.rows);
			laser_y = (int)mcreel_[1].y;
			laser_x = (int)mcreel_[1].x;
		}
	}
	else {
		laser_y = 0;
		laser_x = 0;
	}
//	cv::imshow("red", red_color);
	cout << "\n X = " << laser_x;
	cout << "  Y = " << laser_y;
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

//fonction d'animation et de modification de la position des ninjas 
void update_pos(int fct, ninja *ninja_) {
	int i = 0;
	int ninjaplan = ninja_->getplan();
	int ninjadir = ninja_->getdir();
	int ninjaposx = ninja_->getposx();
	int ninjaposy = ninja_->getposy();
	Mat ninjatext = ninja_->gettexture(); //texture utilisée pour l'affichage (sera donc modifiée pour faire le clignotement)
	Mat ninjatexttmp = ninja_->gettexture(); //sauvegarde de la texture du ninja de base, ne sera jamais changée
	Mat ninjamask = ninja_->getmask();
	Mat ninjamasktmp = ninja_->getmask();

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
				if (ninjaplan == 6) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
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
				putText(Rendufinal, strscore, a, FONT_HERSHEY_SIMPLEX, 0.9, colortext, 2, 8, false);
				imshow("Game", Rendufinal);
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
				if (ninjaplan == 6) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
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
				putText(Rendufinal, strscore, a, FONT_HERSHEY_SIMPLEX, 0.9, colortext, 2, 8, false);
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
				if (ninjaplan == 6) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
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
				putText(Rendufinal, strscore, a, FONT_HERSHEY_SIMPLEX, 0.9, colortext, 2, 8, false);
				imshow("Game", Rendufinal);
				waitKey(1);
				i++;
			}
			break;
		}
	}
	else  //1 -> rentrer un ninja
	{
		nbninja++;
		cout << "rentrer";
		switch (ninjadir) {
		case 0:  //si le ninja rentre à gauche   (posx--)
			while (i<30) {
				if (touche == 1 && (i<6 || (i >= 12 && i < 18) || (i >= 24))) {  //en cours : effectuer le clignotement
					ninjatext = ninja_t_hsv;
					ninjamask = Maskninja_t;
				}
				else if (touche == 1 && i >= 6 && i < 12) {
					ninjatext = ninjatexttmp;		//modification de la texture du ninja à afficher en lui appliquant une texture transparente
					ninjamask = ninjamasktmp;
				}
				else if (touche == 1 && i > 18 && i <24) {
					ninjatext = ninjatexttmp;
					ninjamask = ninjamasktmp;
				}
				ninjaposx--;
				ninja_->setposx(ninjaposx);
				Mat fondninja = subrendu(cvRect(ninjaposx, ninjaposy, 46, 48));
				rendu_hsv.copyTo(subrendu);
				if (ninjaplan == 6) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
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
				//rectangle(Rendufinal, b, c, colorrect, CV_FILLED, 8, 0);
				putText(Rendufinal, strscore, a, FONT_HERSHEY_SIMPLEX, 0.9, colortext, 2, 8, false);
				imshow("Game", Rendufinal);
				waitKey(1);
				i++;
			}
			break;
		case 1: //si le ninja tentre en bas   (posy++)
			while (i<30) {
				if (touche == 1 && (i<6 || (i >= 12 && i < 18) || (i >= 24))) {  //en cours : effectuer le clignotement
					ninjatext = ninja_t_hsv;
					ninjamask = Maskninja_t;
				}
				else if (touche == 1 && i >= 6 && i < 12) {
					ninjatext = ninjatexttmp;		//modification de la texture du ninja à afficher en lui appliquant une texture transparente
					ninjamask = ninjamasktmp;
				}
				else if (touche == 1 && i > 18 && i <24) {
					ninjatext = ninjatexttmp;
					ninjamask = ninjamasktmp;
				}
				ninjaposy++;
				ninja_->setposy(ninjaposy);
				Mat fondninja = subrendu(cvRect(ninjaposx, ninjaposy, 46, 48));
				rendu_hsv.copyTo(subrendu);
				if (ninjaplan == 6) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
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
				putText(Rendufinal, strscore, a, FONT_HERSHEY_SIMPLEX, 0.9, colortext, 2, 8, false);
				imshow("Game", Rendufinal);
				waitKey(1);
				i++;
			}
			break;
		default: //si le ninja rentre à droite   (posx++)
			while (i<30) {
				if (touche == 1 && (i<6 || (i >= 12 && i < 18) || (i >= 24))) {  //en cours : effectuer le clignotement
					ninjatext = ninja_t_hsv;
					ninjamask = Maskninja_t;
				}
				else if (touche == 1 && i >= 6 && i < 12) {
					ninjatext = ninjatexttmp;		//modification de la texture du ninja à afficher en lui appliquant une texture transparente
					ninjamask = ninjamasktmp;
				}
				else if (touche == 1 && i > 18 && i <24) {
					ninjatext = ninjatexttmp;
					ninjamask = ninjamasktmp;
				}
				ninjaposx++;
				ninja_->setposx(ninjaposx);
				Mat fondninja = subrendu(cvRect(ninjaposx, ninjaposy, 46, 48));
				rendu_hsv.copyTo(subrendu);
				if (ninjaplan == 6) {
					ninjatext.copyTo(fondninja, ninjamask);
				}
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
				putText(Rendufinal, strscore, a, FONT_HERSHEY_SIMPLEX, 0.9, colortext, 2, 8, false);
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