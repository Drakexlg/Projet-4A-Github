#ifndef NINJA_H
#define NINJA_H

#include <iostream>
#include<conio.h>
#include"opencv2\highgui.hpp"
#include"opencv2\imgproc.hpp"

using namespace std;
using namespace cv;

class ninja
{
private:
	int posx;
	int posy;
	int plan;	
	int etat;
	int dir;
	Mat texture;
	Mat mask;

public:
	ninja(int posx_ = 0, int posy_ = 0, int plan_ = 0, int etat_ = 0, int dir_ = 0, Mat texture_ = Mat(), Mat mask_ = Mat());

	virtual ~ninja();  // destructeur obligatoire

	int getposx();
	int getposy();
	int getplan();
	int getetat();
	int getdir();
	Mat gettexture();
	Mat getmask();

	void setposx(int posx_ = 0);
	void setposy(int posx_ = 0);
	void setcoords(int posx_ = 0, int posy_ = 0);
	void setplan(int posx_ = 0);
	void setetat(int etat_ = 0);
	void setdir(int dir_);
	void settexture(Mat texture_ = Mat());
	void setMask(Mat mask_ = Mat());
};
#endif