#include "HandDetect.h"

HandDetect::HandDetect()
{
	numColorBins = 256;
	max_val = 0.f;
	hand1 = cvLoadImage("hand.png");
	hand2 = cvLoadImage("hand2.png");
	hist1 = cvCreateHist(1, &numColorBins, CV_HIST_ARRAY);
	hist2 = cvCreateHist(1, &numColorBins, CV_HIST_ARRAY);
	
	rad=0;
	vmin=10, vmax=256, smin=30;

	capture = cvCaptureFromCAM(0);
	setImage();
	backproject = cvCreateImage(cvGetSize(image), 8, 1);
	gray = cvCreateImage(cvGetSize(image), 8, 1);
	track_window = cvRect((int)image->width/2, (int)image->height/2, 1, 1);
	track_box.center.x=-1;
	track_box.center.y=-1;

	hsvHand1 = cvCreateImage(cvGetSize(hand1), 8, 3);
	mskHand1 = cvCreateImage(cvGetSize(hand1), 8, 1);
	hueHand1 = cvCreateImage(cvGetSize(hand1), 8, 1);

	hsvHand2 = cvCreateImage(cvGetSize(hand2), 8, 3);
	mskHand2 = cvCreateImage(cvGetSize(hand2), 8, 1);
	hueHand2 = cvCreateImage(cvGetSize(hand2), 8, 1);	

	cvCvtColor(hand1, hsvHand1, CV_RGB2HSV);
	cvInRangeS(hsvHand1, cvScalar(0, smin, MIN(vmin, vmax), 0), cvScalar(180, 256, MAX(vmin, vmax), 0), mskHand1);
	cvSplit(hsvHand1, hueHand1, 0, 0, 0);

	cvCalcHist(&hueHand1, hist1, 0, mskHand1);
	cvGetMinMaxHistValue(hist1, 0, &max_val, 0, 0);
	cvConvertScale(hist1->bins, hist1->bins, max_val ? 255. / max_val : 0., 0);


	cvCvtColor(hand2, hsvHand2, CV_RGB2HSV);
	cvInRangeS(hsvHand2, cvScalar(0, smin, MIN(vmin, vmax), 0), cvScalar(180, 256, MAX(vmin, vmax), 0), mskHand2);
	cvSplit(hsvHand2, hueHand2, 0, 0, 0);

	cvCalcHist(&hueHand2, hist2, 0, mskHand2);
	cvGetMinMaxHistValue(hist2, 0, &max_val, 0, 0);
	cvConvertScale(hist2->bins, hist2->bins, max_val ? 255. / max_val : 0., 0);
}

void HandDetect::skinDetect()
{
	setImage();
	cvFlip(image, image, 1);

	hsv = cvCreateImage(cvGetSize(image), 8, 3);
	msk = cvCreateImage(cvGetSize(image), 8, 1);
	hue = cvCreateImage(cvGetSize(image), 8, 1);
	
	backproject1 = cvCreateImage(cvGetSize(image), 8, 1);
	backproject2 = cvCreateImage(cvGetSize(image), 8, 1);

	cvCvtColor(image, hsv, CV_RGB2HSV);
	cvInRangeS(hsv, cvScalar(0, smin, MIN(vmin, vmax), 0), cvScalar(180, 256, MAX(vmin, vmax), 0), msk);
	cvSplit(hsv, hue, 0, 0, 0);
		
	cvCalcBackProject(&hue, backproject1, hist1);
	cvCalcBackProject(&hue, backproject2, hist2);
		
	cvThreshold(backproject1, backproject1, 50, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	cvThreshold(backproject2, backproject2, 50, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	cvOr(backproject1, backproject2, backproject, 0);

	cvErode(backproject, backproject, 0, 1);
	cvDilate(backproject, backproject, 0, 1);
	cvAnd(backproject, msk, backproject, 0);


	if(track_box.center.x!=-1&&track_box.center.y!=-1)
		preCen=cvPoint(handCen.x, handCen.y);
	else
		preCen=cvPoint(0,0);

	cvCamShift(backproject, track_window, cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1), &track_comp, &track_box);
	if(track_comp.rect.height>0&&track_comp.rect.width>0)
		track_window = track_comp.rect;
	else
	{
		track_box.center.x=-1;
		track_box.center.y=-1;
	}
	

	cvReleaseImage(&hsv);
	cvReleaseImage(&msk);
	cvReleaseImage(&hue);
	cvReleaseImage(&backproject1);
	cvReleaseImage(&backproject2);
}

void HandDetect::handDetecting()
{
	skinDetect();
	IplImage *tmp = cvCreateImage(cvGetSize(backproject), 8, 1);
	cvZero(tmp);

	if(track_comp.rect.height>0&&track_comp.rect.width>0)
	{
		cvCircle(tmp, handCen, track_box.size.width, CV_RGB(255, 255, 255), -1);
		cvDrawRect(tmp, cvPoint(track_window.x-(int)(track_box.size.width*0.2), track_window.y-(int)(track_box.size.height*0.2)), 
			cvPoint(track_window.x+(int)(track_box.size.width*1.2), track_window.y+track_box.size.height), CV_RGB(255, 255, 255), -1);
		
	}
	cvAnd(backproject, tmp, backproject, 0);
	cvDilate(backproject, backproject, 0, 1);
	cvErode(backproject, backproject, 0, 1);
	
	UsingYCbCr();
	cvAnd(gray, tmp, gray, 0);
	cvErode(gray, gray, 0, 1);
	cvDilate(gray, gray, 0, 1);
//	cvShowImage("52", gray);
	cvReleaseImage(&tmp);

	cvOr(gray, backproject, backproject, 0);
	
	handCen=cvPoint(track_box.center.x, track_box.center.y);
	
	setRad();
//	cvDrawRect(image, cvPoint(track_window.x, track_window.y), cvPoint(track_window.x+track_window.width, track_window.y+track_window.height), CV_RGB(255, 0, 0));
	cvCircle(image, handCen, 2, CV_RGB(255, 0, 0), 2);

}

IplImage *HandDetect::getGray(){
	handDetecting();

	return backproject;
}

IplImage *HandDetect::getImage(){
	handDetecting();
	return image;
}

IplImage *HandDetect::getNoSetImage(){return image;}

void HandDetect::setImage()
{
	image = cvQueryFrame(capture);
}

void HandDetect::setRad()
{
	rad=0;
	double tmp;
	for(int j=0; j<backproject->height; j++)
		for(int i=0; i<backproject->width; i++)
			if(backproject->imageData[j*backproject->width+i]!=0)
			{
				tmp=distance(handCen, cvPoint(i, j));
				if(tmp>rad)
					rad=tmp;
			}
}

CvPoint HandDetect::getHandPos(){return handCen;}
CvPoint HandDetect::getPrePos(){return preCen;}
double HandDetect::getRad(){return rad;}

void HandDetect::UsingYCbCr()
{
	IplImage *U = cvCreateImage(cvGetSize(image),8,1);
	IplImage *R = cvCreateImage(cvGetSize(image),8,1);
	IplImage *G = cvCreateImage(cvGetSize(image),8,1);
	IplImage *B = cvCreateImage(cvGetSize(image),8,1);

	uchar *data_image = (uchar*)image->imageData;
	uchar *data_U = (uchar*)U->imageData; 
	uchar *data_R = (uchar*)R->imageData; 
	uchar *data_G = (uchar*)G->imageData; 
	uchar *data_B = (uchar*)B->imageData;
	uchar y, u, v;

	for(int j=0; j<image->height;j++)
		for(int i=0; i<image->width*3; i+=3)
		{
			data_R[j*image->width+i/3]=data_image[j*image->width*3+i+2];
			data_G[j*image->width+i/3]=data_image[j*image->width*3+i+1];
			data_B[j*image->width+i/3]=data_image[j*image->width*3+i];

			RGB_2_YUV(data_image[j*image->width*3+i+2], data_image[j*image->width*3+i+1], data_image[j*image->width*3+i], y, u, v);

			data_U[j*image->width+i/3]=u;
				
				
			if(data_R[j*image->width+i/3]<10 || data_R[j*image->width+i/3]-5 < data_G[j*image->width+i/3] || data_G[j*image->width+i/3] +15 < data_B[j*image->width+i/3] ||
				(data_R[j*image->width+i/3]-data_G[j*image->width+i/3]) + 20 < (data_G[j*image->width+i/3]-data_B[j*image->width+i/3]))
			{
				gray->imageData[j*image->width+i/3]=0;
			}
			else
			{
				if(data_U[j*image->width+i/3]>=130 && data_U[j*image->width+i/3]<=180)
				{
					gray->imageData[j*image->width+i/3]=0;
				}
				else
				{
					gray->imageData[j*image->width+i/3]=255;
				}
			}
		}

	cvReleaseImage(&U);
	cvReleaseImage(&B);
	cvReleaseImage(&G);
	cvReleaseImage(&R);
}

double distance(CvPoint a, CvPoint b)
{
	return (double)sqrt((double)((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)));
}

uchar limit(uchar input, int lower, int upper)
{
	uchar tmp;
	tmp = ((input > upper) ? upper : input);
	return ((input < lower) ? lower : input);
}


void RGB_2_YUV(uchar R, uchar G, uchar B, uchar &Y, uchar & U, uchar &V)
{
	Y = (uchar)limit(0.299*R + 0.587*G + 0.114*B +0.5, 0, 255);
	U = (uchar)limit(-0.169*R - 0.331*G + 0.500*B+128+0.5 , 0, 255);
	V = (uchar)limit(0.500*R - 0.419*G - 0.081*B +128+0.5, 0, 255);
}
