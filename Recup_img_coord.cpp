#include<opencv2/opencv.hpp>
#include <cmath>
#include<iostream>
#include<conio.h>        
#include <iostream>
#include <cmath>
using namespace std; //On utilise un espace de noms ici


using namespace cv; //On utilise un espace de noms ici

					///////////////////////////////////////////////////////////////////////////////////////////////////

Mat remove_noise(Mat img,int a, int b, int c, int d)
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

Mat mask(Mat img, int l_b, int l_g, int l_r, int h_b, int h_g, int h_r)
{
	IplImage * ipl_img;
	IplImage * mask_;
	Mat imag;
	//conversion Mat to Iplimage
	ipl_img = cvCloneImage(&(IplImage)img);
	mask_ = cvCreateImage(cvGetSize(ipl_img), 8, 1);

	//detection de la couleur et creation du mask en noir et blanc     //	(230 50 125)  (255 110  175)
	cvInRangeS(ipl_img, cvScalar(l_b, l_g, l_r), cvScalar(h_b, h_g, h_r), mask_);
	cvNot(mask_, mask_);
	//conversion Iplimage to Mat

	imag = cv::cvarrToMat(mask_);

	return imag;
}

Mat contours_(Mat img, Mat diff)
{
	std::vector<std::vector<cv::Point> > contours;
	//Determine les contours des formes noirs de l'image diff et les placent dans contours
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

		//std::cout << cptx;
		//std::cout << cpty;
		//cptx = 0;
		//cpty = 0;
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
		std::cout << cpt_norm[i - 1]<<"&";
		if (cpt_norm[i - 1] == 3)
		{
			mclasse[0] = mc[i];
		}
		 else if (cpt_norm[i - 1] ==0)
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



	/*for (int i = 1; i == 4; i++)
	{
		circle(diff, mc[i], 5, cv::Scalar(128, 0, 0), -1);  // dessine des cercle gris au niveau des centres d'inerties.
	}*/
	//imshow("test", diff);



	//Découpage et remise en forme de l'image;

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





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{

	//cv::VideoCapture stream1(0);   //0 is the id of video device.0 if you have only one camera.

	//if (!stream1.isOpened()) { //check if video device has been initialised
	//	std::cout << "cannot open camera";
	//}
	cv::Mat cameraFrame;
	Mat mask_;
	Mat recadrage;
	Mat red_color;
	cv::waitKey();

//	while (true) {

		//stream1.read(cameraFrame); 
		//cv::imshow("cam", cameraFrame);

		IplImage * img;
		img = cvLoadImage("Rendu100.jpg");         
		
		cameraFrame = cv::cvarrToMat(img);
		

		mask_ = mask(cameraFrame, 230, 50, 110, 255, 110, 190);  // appel à la fonction mask qui renvoie les parties de l'image dont les couleurs sont dans la plage BGR indiqué
		mask_ = remove_noise(mask_,5,6,5,5);
		cv::imshow("mask", mask_);

		/// Find contours
		std::vector<std::vector<cv::Point> > contours;

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
			//cv::imshow("recadrage", recadrage);
			

			std::cout << recadrage.rows << endl;
			std::cout << recadrage.cols << endl;

			red_color = mask(recadrage, 150, 70, 150, 170, 200, 255);
			red_color = remove_noise(red_color, 3, 3, 5, 5	); ///a modifier pour s'ajuster a la realité.
			cv::imshow("red", red_color);

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

			if (r_contours.size() ==2)
			{
				std::vector<cv::Point2f> mc_(r_contours.size());
				std::vector<cv::Point2f> mcreel_(r_contours.size());
				for (int i = 1; i < 2; i++)
				{
					mc_[i] = cv::Point2f(mu_[i].m10 / mu_[i].m00, mu_[i].m01 / mu_[i].m00);

					mcreel_[i] = cv::Point2f((mc_[i].x * 648) / recadrage.rows, (mc_[i].y * 1152) / recadrage.cols);
					
					std::cout << "  " << endl;
					std::cout << "coordonee recu" << mc_[i].x << endl;
					std::cout << mc_[i].y << endl;
					std::cout << "  " << endl;
					std::cout << "coordonee reelle"<<mcreel_[i].x << endl;
					std::cout << mcreel_[i].y << endl;
				}
				
				
				
			

			}
		
		}

		


		cv::waitKey(0);

		//if (cv::waitKey(30) >= 0) {
//			break;
//		}
	
	return(0);
		//}

}



