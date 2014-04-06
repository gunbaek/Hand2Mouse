#include <opencv\cv.h>
#include <opencv\highgui.h>


#ifndef _HANDDETECT_H_
#define _HANDDETECT_H_

#define INIT_TIME 50

double distance(CvPoint a, CvPoint b);
uchar limit(uchar input, int lower, int upper);
void RGB_2_YUV(uchar R, uchar G, uchar B, uchar &Y, uchar & U, uchar &V);

class HandDetect
{
private:
	int numColorBins;
	float max_val;
    IplImage *hand1;
	IplImage *hand2;
    IplImage *image, *hsv, *hue, *msk, *backproject, *backproject1, *backproject2, *gray;
	IplImage *hsvHand1, *hueHand1, *mskHand1, *hsvHand2, *hueHand2, *mskHand2;
	CvHistogram *hist1;
	CvHistogram *hist2;
	CvRect track_window;
	CvConnectedComp track_comp;
	CvBox2D track_box;
	CvCapture *capture;
	int vmin, vmax, smin;
	CvPoint handCen, preCen;
	float rad;
public:
	HandDetect();
	void skinDetect();
	void handDetecting();
	IplImage *getGray();
	IplImage *getImage();
	IplImage *getNoSetImage();
	void setImage();
	CvPoint getHandPos();
	CvPoint getPrePos();
	double getRad();
	void setRad();
	void UsingYCbCr();
};




#endif