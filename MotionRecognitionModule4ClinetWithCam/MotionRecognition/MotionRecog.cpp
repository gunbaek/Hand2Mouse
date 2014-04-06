#include "MotionRecog.h"

MotionRecog::MotionRecog()
{
	HandDetect();
	blobHand = new CBlobLabeling();
	blobFinger = new CBlobLabeling();
	handMotion.mot=0;
	handMotion.x=0;
	handMotion.y=0;
	IplImage *tmp = getNoSetImage();
	scr = cvRect((int)(tmp->width*0.15), (int)(tmp->height*0.15), (int)(tmp->width*0.7), (int)(tmp->height*0.6));
	pos=getHandPos();
	count=0;
}

void MotionRecog::motionRec()
{
//	if(handMotion.x>=scr.x&&handMotion.x<=scr.x+scr.width&&handMotion.y>=scr.y&&handMotion.y<=scr.y+scr.height)
//	{
		IplImage *tmp = getGray();
		mask = cvCreateImage(cvGetSize(tmp),8,1);
		cvZero(mask);

		cvCircle(mask, getHandPos(), getRad()*0.65, CV_RGB(255, 255, 255), 3);
		cvAnd(tmp, mask, mask);
		cvErode(mask, mask, 0, 1);
		cvDilate(mask, mask, 0, 3);
	

		blobHand->SetParam(mask, 20);
		blobHand->DoLabeling();

		blobHand->BlobSmallSizeConstraint(5, 5);

//		handMotion.x=(double)(getHandPos().x-scr.x)/(double)scr.width;
//		handMotion.y=(double)(getHandPos().y-scr.y)/(double)scr.height;
		if(blobHand->m_nBlobs!=0&&!onMove())
		{
			pos=getPrePos();
			if(count<3)
			{
				count++;
			}else{
				if(blobHand->m_nBlobs>=6)
					handMotion.mot=6;
				else
					handMotion.mot=blobHand->m_nBlobs;
				count=0;
			}
			
		//	cvShowImage("3", tmp);
		}
		if(onMove())
		{
			pos=getHandPos();
			if(handMotion.mot>2)
				handMotion.mot=5;
		}
		char motion[][5] = {"00","01","02","03","04","05"};
		CvFont font;
		cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0);
		cvPutText(getNoSetImage(), motion[handMotion.mot-1], cvPoint(50,50), &font, cvScalar(0,0,255));

		cvReleaseImage(&mask);
	if(pos.x>=scr.x&&pos.x<=scr.x+scr.width&&pos.y>=scr.y&&pos.y<=scr.y+scr.height)
	{
		handMotion.x=(double)(pos.x-scr.x)/(double)scr.width;
		handMotion.y=(double)(pos.y-scr.y)/(double)scr.height;
	}else{
		handMotion.x=0;
		handMotion.y=0;
		handMotion.mot=6;
	}
	cvDrawRect(getNoSetImage(),cvPoint(scr.x, scr.y), cvPoint(scr.x+scr.width, scr.y+scr.height), CV_RGB(255, 0, 255), 2);
}

IplImage *MotionRecog::getImg()
{
	motionRec();
	return getNoSetImage();
}

bool MotionRecog::onMove()
{
	if(distance(getPrePos(), getHandPos())>5)
		return true;
	else
		return false;
}