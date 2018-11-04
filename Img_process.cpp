#include<opencv2/opencv.hpp>
#include <cmath>
#include<iostream>
#include<conio.h>        

///////////////////////////////////////////////////////////////////////////////////////////////////





int main() {
	IplImage * img;
	cv::VideoCapture stream1(1);   //0 is the id of video device.0 if you have only one camera.

	if (!stream1.isOpened()) { //check if video device has been initialised
		std::cout << "cannot open camera";
	}
	cv::waitKey();
	int cpt = 0;
	while (true) {
		cv::Mat cameraFrame;
		stream1.read(cameraFrame);
		cv::imshow("cam", cameraFrame);
		
		//conversion Mat to Iplimage
		img = cvCloneImage(&(IplImage)cameraFrame);
		
		// show windows
		cvShowImage("Mat_Flux_entree", img);
		IplImage * mask;
		mask = cvCreateImage(cvGetSize(img), 8, 1);

		//detection du rouge et creation du mask en noir et blanc
		cvInRangeS(img, cvScalar(40.0, 40.0, 140.0), cvScalar(100.0, 100.0, 255.0 + 1.0), mask);

		cvNot(mask, mask);

		//cvShowImage("mask", mask);

		//conversion Iplimage to Mat
		cv::Mat Matmask = cv::cvarrToMat(mask);
		cv::imshow("Matmask", Matmask);

		cv::Mat Matimg = cv::cvarrToMat(img);
		//cv::imshow("Matimg", Matimg);

		cv::Mat difference;
		threshold(Matmask, difference, 200, 255, CV_8UC1);

		erode(difference, difference, cv::getStructuringElement(cv:: MORPH_ELLIPSE, cv::Size(5, 5)));
		dilate(difference, difference, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

		//morphological closing (fill small holes in the foreground)
		dilate(difference, difference, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
		erode(difference, difference, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
		cv::imshow("diff0", difference);

		// Floodfill from point (0, 0)
		cv::Mat im_floodfill = difference.clone();
		floodFill(im_floodfill, cv::Point(0, 0), cv::Scalar(255));

		// Invert floodfilled image
		cv::Mat im_floodfill_inv;
		bitwise_not(im_floodfill, im_floodfill_inv);

		// Combine the two images to get the foreground.
		cv::Mat im_out = (difference | im_floodfill_inv);

		cv::imshow("diff",difference);

		/// Find contours
		std::vector<std::vector<cv::Point> > contours;
		cv::Mat contourOutput = difference.clone();
		cv::findContours(contourOutput, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));


		cpt = 0;
		/// Get the moments
		std::vector<cv::Moments> mu(contours.size());
		for (int i = 1; i < contours.size(); i++)
		{
			if (arcLength(contours[i], true) > 30)
			{
				cpt = cpt + 1;
				mu[cpt] = moments(contours[cpt], false);
				
			}
			
		}

		///  Get the mass centers:

		std::vector<cv::Point2f> mc(contours.size());
		
			for (int i = 1; i < cpt+1; i++)
			{
				mc[i] = cv::Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
			}

		imshow("Centres", difference);
		

		cv::Mat little_frame;
		//Dimension du flux camera	

	
			cv::Mat M;

			cv::Point2f inputQuad[4];  //Coordonnees selectionnée dans l'image recu par la webcam
	

		cv::Point2f outputQuad[4];//Dimension de la video de sortie
		outputQuad[0] = cv::Point2f(0, 0);
		outputQuad[1] = cv::Point2f(Matimg.cols, 0);
		outputQuad[2] = cv::Point2f(Matimg.cols, Matimg.rows);
		outputQuad[3] = cv::Point2f(0, Matimg.rows);
		
		
		std::vector<cv::Point2f> mclasse(cpt);
		std::cout << cpt;
		int cptx = 0, cpty = 0;
		if (cpt == 4) {
		

			std::cout << "allo";

			for (int i = 1; i <= 4; i=i+1)
			{
				std::cout << "bjr";
				cptx = 0;
				cpty = 0;
				for (int j = 1; j <= 4; j++)
				{
					if (mc[i].x > mc[j].x)
					{
						cptx++;
					}
					if (mc[i].y > mc[j].y)
					{
						cpty++;
					}
					
				}
				std::cout << cptx;
				std::cout << cpty;
				if ((cptx >= 2) && (cpty >= 2))
				{
					mclasse[0] = mc[i];
				}
				else if ((cptx < 2) && (cpty >= 2))
				{
					mclasse[2] = mc[i];
				}
				else if ((cptx < 2) && (cpty < 2))
				{
					mclasse[3] = mc[i];
				}
				else if ((cptx >= 2) && (cpty < 2))
				{
					mclasse[1] = mc[i];
				}

			}
			//mclasse[2] = mc[1];
			std::cout << mclasse[0];
			std::cout << mclasse[1];
			std::cout << mclasse[2];
			std::cout << mclasse[3];
			for (int i = 1; i == 4; i++)
			{
				circle(difference, mc[i], 5, cv::Scalar(128, 0, 0), -1);
			}

			imshow("test", difference);
			double norm1 = cv::norm(mclasse[0] - mclasse[1]);
			double norm2 = cv::norm(mclasse[2] - mclasse[3]);
			if (fabs(norm1 - norm2) < 70) {
					//std::cout << "Pas de laser ok";
					inputQuad[0] = mclasse[3];
					inputQuad[1] = mclasse[1];
					inputQuad[2] = mclasse[0];
					inputQuad[3] = mclasse[2];

					
					//Decoupage
					M = getPerspectiveTransform(inputQuad, outputQuad);

					warpPerspective(Matimg, little_frame, M, little_frame.size());
					//Affichage
					cv::imshow("Video de base", Matimg);
					cv::imshow("Partie de video", little_frame);
				}
			}

			else {
				//std::cout << "probleme";
			}
		


		if (cv::waitKey(30) >= 0)
			break;
	}
	return(0);
}