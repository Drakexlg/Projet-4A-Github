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
	channels : RGB = 3 channels, or le masque est une image en échelle de gris donc un seul canal est nécessaire  donc canal = 1
	"http://images.math.cnrs.fr/le-traitement-numerique-des-images.html*   	*/
	maskbgnd2 = cvCreateImage(cvGetSize(imgBgnd2), 8, 1);  //créé un "receptacle" de la taille de l'image bgnd2, de couleur 8 bits en échelle de gris

	/*cvInRangeS : créé un masque à partir d'une image
	arguments de cvInRangeS : (InputArray source, InputArray lowerb, InputArray upperb, OutputArray destination)
	InputArray Source : image en couleur qui va être échantillonnée pour créer le masque
	InputArray lowerb : écriture scalaire de la couleur RGB "basse"  ATTENTION la syntaxe n'est pas RGB mais BGR dans opencv
  InputArray lowerb : écriture scalaire de la couleur RGB "haute"  ATTENTION la syntaxe n'est pas RGB mais BGR dans opencv
	toutes les couleurs comprises dans l'intervalle [lowerb,upperb] seront transcrites en blanc dans le masque, les autres en noir
	OutputArray destination : objet de sortie qui comprendra le masque
	*/
	cvInRangeS(imgBgnd2, cvScalar(0.0, 255.0, 76.0), cvScalar(0.0, 255.0, 76.0), maskbgnd2);//détection du fond vert et création du masque à partir de celui-ci
	cvNot(maskbgnd2, maskbgnd2); //inversion du blanc et du noir du masque


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

		/*cvSetImageROI : definit la région d'intêret d'une image (ROI = Region Of Interest): permet de ne se concentrer que sur une certaine partie de l'image
		arguments de cvSetImageROI : (IplImage image, cvRect rect)
		IplImage image : représente l'image dont on veut changer le cvSetImageROI
		cvRect rect : Rectangle composé d'un point d'origine, d'une longueur et d'une largeur
		*/
		cvSetImageROI(Rendu, cvRect(0, 0, Rendu->width, Rendu->height)); // on définit le ROI du rendu final sur toute l'image (on souhaite voir toute l'image)

		//Incrustation de l'image a slider
		cvSetImageROI(Rendu, cvRect(0, 0, imgBgnd6->width, imgBgnd6->height)); //on définit le ROI du rendu final sur le ROI de l'image qu'on va copier dessus afin que si
		//il y a un écart de résolution entre l'image à copier et le rendu, le programme ne plante pas
		cvCopy(imgBgnd6, Rendu, maskbgnd6); //on copie l'image imgBgnd6 sur le rendu final en utilisant le masque maskbgnd6 créé précédemment pour ne pas copier le vert dessus
		cvResetImageROI(Rendu); //on reset le ROI du rendu final

		//on continue de superposer les images sur le rendu final en faisant attention à copier les calques d'arrière plan en premier et en remontant de cette manière
		//vers le premier plan
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

		//Affichage du rendu final
		cvShowImage("all", Rendu);

	cv::waitKey(0);     //l'appui sur n'importe quelle touche fera interrompre le programme

	return(0);
}
