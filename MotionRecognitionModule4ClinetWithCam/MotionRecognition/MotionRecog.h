#include <opencv\cv.h>
#include <opencv\highgui.h>
#include "BlobLabeling.h"
#include "HandDetect.h"

#ifndef _MOTIONRECOG_H_
#define _MOTIONRECOG_H_


typedef struct HandMotion{
	double x;
	double y;
	int mot;
}motion;

class MotionRecog : public HandDetect
{
private:
	IplImage *mask;
	CBlobLabeling *blobHand;
	CBlobLabeling *blobFinger;
	CvPoint pos;
	motion handMotion;
	CvRect scr;
	int count;
public:
	MotionRecog();
	void motionRec();
	IplImage *getImg();
	motion getMotion(){return handMotion;}
	bool onMove();
};


#endif