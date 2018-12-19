#include<opencv2/opencv.hpp>
#include <cmath>
#include<iostream>
#include<conio.h>        
#include <iostream>
#include <cmath>
using namespace std; //On utilise un espace de noms ici


using namespace cv; //On utilise un espace de noms ici

					///////////////////////////////////////////////////////////////////////////////////////////////////

Mat remove_noise(Mat img)
{
	Mat difference;
	threshold(img, difference, 200, 255, CV_8UC1);
	//Eliminer le bruit
	erode(difference, difference, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
	dilate(difference, difference, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(6, 6)));


	dilate(difference, difference, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
	erode(difference, difference, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
	//cv::imshow("diff0", difference);

	// Floodfill from point (0, 0) Remplir les figures
	//cv::Mat im_floodfill = difference.clone();
	//floodFill(im_floodfill, cv::Point(0, 0), cv::Scalar(255));

	// Invert floodfilled image
	//	cv::Mat im_floodfill_inv;
	//	bitwise_not(im_floodfill, im_floodfill_inv);
	// Combine the two images to get the foreground.
	//	cv::Mat imag;
	//	imag=(difference | im_floodfill_inv);

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

	return remove_noise(imag);
}

Mat contours_(Mat img, Mat diff)
{
	std::vector<std::vector<cv::Point> > contours;

	cv::findContours(diff, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	/// Get the moments
	std::vector<cv::Moments> mu(contours.size());

	for (int i = 1; i < contours.size(); i++)
	{
		if (arcLength(contours[i], true) > 30)
		{
			mu[i] = moments(contours[i], false);
		}
	}

	///  Get the mass centers:

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

	int cpt_norm[4];
	cv::Mat little_frame;
	double norm[4];
	for (int i = 1; i <= 4; i = i + 1)
	{
		norm[i - 1] = sqrt(mc[i].x*mc[i].x + mc[i].y*mc[i].y);
		//std::cout << norm[i-1];
	}
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



	for (int i = 1; i == 4; i++)
	{
		circle(diff, mc[i], 5, cv::Scalar(128, 0, 0), -1);
	}
	//imshow("test", diff);
	double norm1 = cv::norm(mclasse[0] - mclasse[1]);
	double norm2 = cv::norm(mclasse[2] - mclasse[3]);
	double norm3 = cv::norm(mclasse[0] - mclasse[2]);
	double norm4 = cv::norm(mclasse[1] - mclasse[3]);
	cout << "a";
	//if (fabs(norm1 - norm2) < 170) {
	//std::cout << "Pas de laser ok";
	inputQuad[0] = mclasse[3];
	inputQuad[1] = mclasse[2];
	inputQuad[2] = mclasse[0];
	inputQuad[3] = mclasse[1];
	cout << "g";
	cv::Mat M;
	M = getPerspectiveTransform(inputQuad, outputQuad);

	warpPerspective(img, little_frame, M, little_frame.size());
	return little_frame;
	//	}


}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{

	cv::VideoCapture stream1(1);   //0 is the id of video device.0 if you have only one camera.

	if (!stream1.isOpened()) { //check if video device has been initialised
		std::cout << "cannot open camera";
	}
	cv::Mat cameraFrame;
	Mat mask_;
	Mat recadrage;
	Mat red_color;
	cv::waitKey();

	while (true) {

		stream1.read(cameraFrame);
		cv::imshow("cam", cameraFrame);


		mask_ = mask(cameraFrame, 230, 50, 110, 255, 110, 190);
		cv::imshow("mask", mask_);

		/// Find contours
		std::vector<std::vector<cv::Point> > contours;

		cv::findContours(mask_, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
		/// Get the moments
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


		//mask_ = mask(cameraFrame, 230, 50, 125, 255, 110, 175);
		std::cout << cpt;

		if (cpt == 4) {

			recadrage = contours_(cameraFrame, mask_);
			cv::imshow("recadrage", recadrage);
		}

		//red_color = mask(recadrage, 170, 70, 150, 150, 200, 255);

		//cv::imshow("red", red_color);
		if (cv::waitKey(30) >= 0) {
			break;
		}
	}
	return(0);
}




//
////Dimension du flux camera	

//cv::Mat M2;

/*cv::Point2f inputQuad[4];  //Coordonnees selectionnée dans l'image recu par la webcam


cv::Point2f outputQuad[4];//Dimension de la video de sortie
outputQuad[0] = cv::Point2f(0, 0);
outputQuad[1] = cv::Point2f(Matimg.cols, 0);
outputQuad[2] = cv::Point2f(Matimg.cols, Matimg.rows);
outputQuad[3] = cv::Point2f(0, Matimg.rows);

cv::Point2f inputQuad2[4];  //Coordonnees selectionnée dans l'image recu par la webcam
inputQuad2[0] = cv::Point2f(0, 0);
inputQuad2[1] = cv::Point2f(little_frame.cols, 0);
inputQuad2[2] = cv::Point2f(little_frame.cols, little_frame.rows);
inputQuad2[3] = cv::Point2f(0, little_frame.rows);


cv::Point2f outputQuad2[4];//Dimension de la video de sortie
outputQuad2[0] = cv::Point2f(0, 0);
outputQuad2[1] = cv::Point2f(485, 0);
outputQuad2[2] = cv::Point2f(485, 250);
outputQuad2[3] = cv::Point2f(0, 250);
*/



//Decoupage

//M2 = getPerspectiveTransform(inputQuad2, outputQuad2);

//warpPerspective(little_frame, little_frame2, M2, little_frame2.size());

//cv::imshow("Partie de video2", little_frame2);

//convertion de type mat to ipl



//		mat_decoup= cvCloneImage(&(IplImage)little_frame);
//		
//		maskred = cvCreateImage(cvGetSize(mat_decoup), 8, 1);
//		//detection du rouge et creation du maskgreen en noir et blanc
//		cvInRangeS(mat_decoup, cvScalar(110, 70, 150), cvScalar(150 + 1.0, 200, 255.0), maskred);
//		cvNot(maskred, maskred);
//		

//		//conversion Iplimage to Mat
//		 Matmask = cv::cvarrToMat(maskred);
//		cv::imshow("mat_de", Matmask);
//		 Matimg = cv::cvarrToMat(mat_decoup);
//		//cv::imshow("Matimg", Matimg);

//		threshold(Matmask, difference, 200, 255, CV_8UC1);
//		cv::imshow("maskred", Matmask);
//		//Eliminer le bruit
//		erode(difference, difference, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(12, 12)));
//		dilate(difference, difference, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(12, 12)));


//		dilate(difference, difference, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
//		erode(difference, difference, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
//		cv::imshow("diff0", difference);

//		// Floodfill from point (0, 0) Remplir les figures
//		cv::Mat im_floodfill = difference.clone();
//		floodFill(im_floodfill, cv::Point(0, 0), cv::Scalar(255));

//		// Invert floodfilled image
//		cv::Mat im_floodfill_inv;
//		bitwise_not(im_floodfill, im_floodfill_inv);

//		// Combine the two images to get the foreground.
//		cv::Mat im_out = (difference | im_floodfill_inv);

//		cv::imshow("diff", difference);
//	}
//}

//else {
//	//std::cout << "probleme";
//}








