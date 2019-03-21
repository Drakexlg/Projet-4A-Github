#include "ninja.h"
#include <iostream>
#include"opencv2\highgui.hpp"
#include"opencv2\imgproc.hpp"

using namespace std;
using namespace cv;

ninja::ninja(int posx_ , int posy_ , int plan_ , int etat_  ,int dir_ , Mat texture_,Mat mask_  ) {
	posx = posx_;
	posy = posy_;
	plan = plan_;
	etat = etat_;
	dir = dir_;
	texture = texture_;
	mask = mask_;
}

ninja::~ninja() {}

int ninja::getposx() {
	return posx;
}

int ninja::getposy() {
	return posy;
}

int ninja::getplan() {
	return plan;
}

int ninja::getetat() {
	return etat;
}

int ninja::getdir() {
	return dir;
}

Mat ninja::gettexture() {
	return texture;
}

Mat ninja::getmask() {
	return mask;
}

void ninja::setposx(int posx_) {
	posx = posx_;
}

void ninja::setposy(int posy_) {
	posy = posy_;
}

void ninja::setcoords(int posx_, int posy_) {
	posx = posx_;
	posy = posy_;
}

void ninja::setplan(int plan_) {
	plan = plan_;
}

void ninja::setetat(int etat_) {
	etat = etat_;
}

void ninja::setdir(int dir_) {
	dir = dir_;
}

void ninja::settexture(Mat texture_) {
	texture = texture_;
}

void ninja::setMask(Mat mask_) {
	mask = mask_;
}
