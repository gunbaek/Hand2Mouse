#include <opencv\cv.h>
#include <opencv\highgui.h>
#include "BlobLabeling.h"


#define INIT_TIME 50

///
uchar limit(uchar input, int lower, int upper);
void RGB_2_YUV(uchar R, uchar G, uchar B, uchar &Y, uchar & U, uchar &V);

typedef struct _whitePoints{
	int num;
	int x;
	int y;
}whitePoints;


///


IplImage *image=0;	
IplImage *av=0, *sgm=0;  
IplImage *lower=0, *upper=0, *tmp=0;  
IplImage *dst=0, *msk=0;

CvCapture *capture=0;

///
IplImage *imageCopy=0, *preFrame=0;
IplImage *U, *R, *G, *B;
IplImage *grayImage, *labeled, *labeled2, *labeled3, *hand, *Roi, *Roi2;
IplImage *circle, *circle2;
IplImage *AR, *ARResize, *Map;
char motion[][5] = {"00","01","02","03","04","05"};
///

void main()
{
	capture = cvCaptureFromCAM(0);
	image = cvQueryFrame(capture);
	cvFlip(image, image, 1); 
	CvFont font;

	av = cvCreateImage(cvGetSize(image), IPL_DEPTH_32F,3);
	sgm = cvCreateImage(cvGetSize(image), 32,3);
	lower = cvCreateImage(cvGetSize(image), 32,3);
	upper = cvCreateImage(cvGetSize(image), 32,3);
	tmp = cvCreateImage(cvGetSize(image), 32,3);
	dst = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U,3);
	msk = cvCreateImage(cvGetSize(image), 8,1);
	
	///
	int i, j, h, w, thresh;
	uchar *data_image, *data_pre;
	uchar y, u, v;
	uchar *data; 
	uchar *data_U, *data_R, *data_G, *data_B, *data_sub;
	///

	cvSetZero(av);
	for(i=0; i<INIT_TIME; i++)
	{
		image=cvQueryFrame(capture);
		cvFlip(image, image, 1); 
		cvAcc(image,av);
	}
	cvConvertScale(av, av, 1.0/ INIT_TIME);

	cvSetZero(sgm);
	for(i=0; i<INIT_TIME; i++)
	{
		image=cvQueryFrame(capture);
		cvFlip(image, image, 1); 
		cvConvert(image, tmp);
		cvSub(tmp,av,tmp);
		cvPow(tmp,tmp,2.0);// 제곱
		cvConvertScale(tmp,tmp,2.0);
		cvPow(tmp,tmp,0.5);//루트
		cvAcc(tmp,sgm);
	}
	cvConvertScale(sgm,sgm, 1.0 / INIT_TIME);
	
	//
	bool screenOn=false;
	bool screenSet=false;
	CvPoint scr1;
	CvPoint scr2;
	CBlobLabeling blob;
	CBlobLabeling blob2;
	CBlobLabeling blob3;
//	preFrame = cvCreateImage(cvGetSize(image), 8,3);
	imageCopy = cvCreateImage(cvGetSize(image), 8,3);
	grayImage = cvCreateImage(cvGetSize(image), 8,1);
	labeled = cvCreateImage(cvGetSize(image), 8,3);
	hand = cvCreateImage(cvGetSize(image), 8,1);
//	cvCopy(image, preFrame,0);
	whitePoints cp;
	whitePoints cp2;
	CvPoint finger;
	CvPoint finger2;
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0);
//	cvNamedWindow("image", CV_WINDOW_NORMAL);
//   cvSetWindowProperty("image", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
//	cvNamedWindow("Map", CV_WINDOW_NORMAL);
//    cvSetWindowProperty("Map", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	AR = cvLoadImage("world.jpg", CV_LOAD_IMAGE_COLOR);
	Map = cvCreateImage(cvGetSize(AR), 8,3);
	double mapRate;

	
	//





	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    while(1)
    {

		image = cvQueryFrame(capture);
		cvFlip(image, image, 1); 

		double B_PARAM = 0.02;// 1.0 / 30.0;
		double T_PARAM = 0.02;//1.0 / 100.0;
		double zeta = 10.0;
	
		
		msk = cvCreateImage(cvGetSize(image), 8,1);

		cvConvert(image, tmp);

		cvSub(av, sgm, lower);
		cvSubS(lower, cvScalarAll(zeta),lower);
		cvAdd(av, sgm, upper);
		cvAddS(upper, cvScalarAll(zeta),upper);
		cvInRange(tmp, lower, upper, msk);//마스크 영상 만들기

		cvSub(tmp, av, tmp);
		cvPow(tmp, tmp, 2.0);//제곱
		//cvConvertScale(tmp,tmp,2.0);
		cvPow(tmp, tmp, 0.5);//루트
		cvRunningAvg(image, av, B_PARAM, msk);	
		cvRunningAvg(tmp, sgm, B_PARAM, msk);

		cvNot(msk, msk);
		cvRunningAvg(tmp, sgm, T_PARAM, msk);

		cvErode(msk,msk,0,1);
		cvErode(msk,msk,0,1);
		cvDilate(msk,msk,0,1);
		cvDilate(msk,msk,0,1);

		///
		cvCopy(image, imageCopy, 0);
	//	cvCvtColor(image, image, CV_BGR2HSV);
		cvCopy(AR, Map, 0);
		data_image = (uchar*)imageCopy->imageData;
//		data_pre = (uchar*)preFrame->imageData;
	
		h=imageCopy->height;
		w=imageCopy->width;
	
		U = cvCreateImage(cvGetSize(image),8,1);
		R = cvCreateImage(cvGetSize(image),8,1);
		G = cvCreateImage(cvGetSize(image),8,1);
		B = cvCreateImage(cvGetSize(image),8,1);

		data_U = (uchar*)U->imageData; 
		data_R = (uchar*)R->imageData; 
		data_G = (uchar*)G->imageData; 
		data_B = (uchar*)B->imageData;

		

		for(j=0; j<h;j++)
				for(i=0; i<w*3; i+=3)
				{
					data_R[j*w+i/3]=data_image[j*w*3+i+2];
					data_G[j*w+i/3]=data_image[j*w*3+i+1];
					data_B[j*w+i/3]=data_image[j*w*3+i];

					RGB_2_YUV(data_image[j*w*3+i+2], data_image[j*w*3+i+1], data_image[j*w*3+i], y, u, v);

					data_U[j*w+i/3]=u;
				
				
					if(data_R[j*w+i/3]<10 || data_R[j*w+i/3]-5 < data_G[j*w+i/3] ||
						data_G[j*w+i/3] +15 < data_B[j*w+i/3] ||
						(data_R[j*w+i/3]-data_G[j*w+i/3]) + 20 < (data_G[j*w+i/3]-data_B[j*w+i/3]))
					{
						data_image[j*w*3+i+2]=0;//data_pre[j*w*3+i+2];
							data_image[j*w*3+i+1]=0;//data_pre[j*w*3+i+1];
							data_image[j*w*3+i]=0;//data_pre[j*w*3+i];
					}
					else
					{
						if(data_U[j*w+i/3]>=130 && data_U[j*w+i/3]<=180)
						{
							data_image[j*w*3+i+2]=0;//data_pre[j*w*3+i+2];
							data_image[j*w*3+i+1]=0;//data_pre[j*w*3+i+1];
							data_image[j*w*3+i]=0;//data_pre[j*w*3+i];
						}
						else
						{
							data_image[j*w*3+i+2]=255;//data_image[j*w*3+i+2];
							data_image[j*w*3+i+1]=255;//data_image[j*w*3+i+1];
							data_image[j*w*3+i]=255;//data_image[j*w*3+i];
						}
					}


					



				
				
					if(screenOn)
					{
						if(i/3<scr1.x-50 || i/3>scr2.x+50 || j<scr1.y-50 || j>scr2.y+50)// && j<min(scr1.y,scr2.y))// || i/3>max(scr1.x,scr2.x) || j>max(scr1.y,scr2.y))
						{
							data_image[j*w*3+i+2]=0;//data_pre[j*w*3+i+2];
							data_image[j*w*3+i+1]=0;//data_pre[j*w*3+i+1];
							data_image[j*w*3+i]=0;//data_pre[j*w*3+i];
						}
					}
	/*				if(msk->imageData[j*w+i/3]==0)
					{
						data_image[j*w*3+i+2]=0;//data_pre[j*w*3+i+2];
						data_image[j*w*3+i+1]=0;//data_pre[j*w*3+i+1];
						data_image[j*w*3+i]=0;//data_pre[j*w*3+i];
					}*/


				}
		cvCvtColor(imageCopy, grayImage, CV_RGB2GRAY);
	
		/*cvErode(grayImage,grayImage,0,1);
		cvErode(grayImage,grayImage,0,1);
		cvDilate(grayImage,grayImage,0,1);
		cvDilate(grayImage,grayImage,0,1);*/

		cvAnd(msk, grayImage, grayImage,0);
		
		cvDilate(grayImage,grayImage,0,1);
		cvErode(grayImage,grayImage,0,1);
				

		cvCvtColor(grayImage, labeled, CV_GRAY2BGR);
		
		blob.SetParam(grayImage, 100);
		blob.DoLabeling();
		
		blob.BlobBigSizeConstraint(300, 300);
		blob.BlobSmallSizeConstraint(30, 30);
		

	/*	for(i=0; i<1; i++)
		{
			CvPoint pt1= cvPoint(blob.m_recBlobs[i].x, blob.m_recBlobs[i].y);
			CvPoint pt2= cvPoint(pt1.x+blob.m_recBlobs[i].width, pt1.y+blob.m_recBlobs[i].height);

			CvScalar color = cvScalar(0,0,255);

			cvDrawRect(image, pt1, pt2, color, 2);
		}*/

		if(blob.m_nBlobs!=0)
		{
			cvSetImageROI(grayImage, cvRect(blob.m_recBlobs[0].x, blob.m_recBlobs[0].y, blob.m_recBlobs[0].width, blob.m_recBlobs[0].height));
			Roi = cvCreateImage(cvSize(blob.m_recBlobs[0].width, blob.m_recBlobs[0].height),8,1);
			cvCopy(grayImage, Roi,0);
			cvResetImageROI(grayImage);
			
		
	//		cvDilate(Roi, Roi, 0, 1);
	//		cvErode(Roi, Roi, 0, 1);
			
			int sub_w = Roi->width;
			int sub_h = Roi->height;
			int sub_ws = Roi->widthStep;
			data_sub = (uchar*)Roi->imageData;
			cp.x=0;
			cp.y=0;
			cp.num=0;

			
			for(j=0; j<sub_h; j++)
				for(i=0; i<sub_w; i++)
				{
					if(data_sub[j*sub_ws+i]!=0)
					{
						cp.x+=i;
						cp.y+=j;
						cp.num++;
					}
				}
			int maxFinger=0;
			for(j=0; j<sub_h; j++)
				for(i=0; i<sub_w; i++)
				{
					if(data_sub[j*sub_ws+i]!=0)
						if(maxFinger<(int)sqrt((double)(((int)(cp.x/cp.num) - i)*((int)(cp.x/cp.num) - i)+((int)(cp.y/cp.num) - j)*((int)(cp.y/cp.num) - j))))
						{
							maxFinger=(int)sqrt((double)(((int)(cp.x/cp.num) - i)*((int)(cp.x/cp.num) - i)+((int)(cp.y/cp.num) - j)*((int)(cp.y/cp.num) - j)));
							finger.x=i;
							finger.y=j;
						}
				}
			if(screenOn)
				cvCircle(Map, cvPoint((int)((blob.m_recBlobs[0].x-scr1.x+(int)(cp.x/cp.num))*mapRate), (int)((blob.m_recBlobs[0].y-scr1.y+(int)(cp.y/cp.num))*mapRate)), 5, CV_RGB(255, 0, 0), 3);
			else
				cvCircle(image, cvPoint(blob.m_recBlobs[0].x+(int)(cp.x/cp.num), blob.m_recBlobs[0].y+(int)(cp.y/cp.num)), 5, CV_RGB(0, 255, 0), 3);
		//	cvCircle(image, cvPoint(blob.m_recBlobs[0].x+(int)(cp.y/cp.num), blob.m_recBlobs[0].x+(int)(cp.x/cp.num)), 5, CV_RGB(0, 255, 0), 3);
		//	cvCircle(Roi, cvPoint(blob.m_recBlobs[0].x+(int)(cp.y/cp.num), blob.m_recBlobs[0].x+(int)(cp.x/cp.num)), 5, CV_RGB(0, 255, 0), 3);

			int rad = (int)sqrt((double)(((int)(cp.x/cp.num) - finger.x)*((int)(cp.x/cp.num) - finger.x)+
										 ((int)(cp.y/cp.num) - finger.y)*((int)(cp.y/cp.num) - finger.y)));

		//	cvCircle(image, cvPoint(blob.m_recBlobs[0].x+(int)(cp.x/cp.num), blob.m_recBlobs[0].y+(int)(cp.y/cp.num)), (int)(rad/1.5), CV_RGB(255, 0, 0), 1);
		//	cvLine(image, cvPoint(blob.m_recBlobs[0].x+(int)(cp.x/cp.num), blob.m_recBlobs[0].y+(int)(cp.y/cp.num)),
		//		cvPoint(blob.m_recBlobs[0].x+finger.x, blob.m_recBlobs[0].y+finger.y), CV_RGB(255,255,0), 1);

			circle = cvCreateImage(cvGetSize(Roi), 8, 1);
			cvSetZero(circle);
			cvCircle(circle, cvPoint((int)(cp.x/cp.num), (int)(cp.y/cp.num)), (int)(rad/1.5), CV_RGB(255, 255, 255), 3);
			cvAnd(Roi, circle, circle, 0);

			labeled2 = cvCreateImage(cvGetSize(circle), 8,3);
			cvCvtColor(circle, labeled2, CV_GRAY2BGR);
		
			blob2.SetParam(circle, 20);
			blob2.DoLabeling();
		
			//blob2.BlobBigSizeConstraint(30,30);
			//blob.BlobSmallSizeConstraint(40, 40);
			
			

			for(i=0; i<blob2.m_nBlobs; i++)
			{
				
				CvPoint pt1= cvPoint(blob2.m_recBlobs[i].x, blob2.m_recBlobs[i].y);
				CvPoint pt2= cvPoint(pt1.x+blob2.m_recBlobs[i].width, pt1.y+blob2.m_recBlobs[i].height);

				CvScalar color = cvScalar(0,0,255);

				cvDrawRect(labeled2, pt1, pt2, color, 2);
				cvPutText(image, motion[blob2.m_nBlobs-1], cvPoint(50,50), &font, cvScalar(0,0,255));
				if(screenOn)
					cvPutText(Map, motion[blob2.m_nBlobs-1], cvPoint(50,50), &font, cvScalar(0,0,255));
			}
			




			if(blob.m_nBlobs>1)
			{
				cvSetImageROI(grayImage, cvRect(blob.m_recBlobs[1].x, blob.m_recBlobs[1].y, blob.m_recBlobs[1].width, blob.m_recBlobs[1].height));
				Roi2 = cvCreateImage(cvSize(blob.m_recBlobs[1].width, blob.m_recBlobs[1].height),8,1);
				cvCopy(grayImage, Roi2,0);
				cvResetImageROI(grayImage);

				int sub_w = Roi2->width;
				int sub_h = Roi2->height;
				int sub_ws = Roi2->widthStep;
				data_sub = (uchar*)Roi2->imageData;
				cp2.x=0;
				cp2.y=0;
				cp2.num=0;

				int k=0;
				for(j=0; j<sub_h; j++)
					for(i=0; i<sub_w; i++)
					{
						if(data_sub[j*sub_ws+i]!=0)
						{
							cp2.x+=i;
							cp2.y+=j;
							cp2.num++;
							
						}
					}
				
				maxFinger=0;
				for(j=0; j<sub_h; j++)
					for(i=0; i<sub_w; i++)
					{
						if(data_sub[j*sub_ws+i]!=0)
							if(maxFinger<(int)sqrt((double)(((int)(cp2.x/cp2.num) - i)*((int)(cp2.x/cp2.num) - i)+((int)(cp2.y/cp2.num) - j)*((int)(cp2.y/cp2.num) - j))))
							{
								maxFinger=(int)sqrt((double)(((int)(cp2.x/cp2.num) - i)*((int)(cp2.x/cp2.num) - i)+((int)(cp2.y/cp2.num) - j)*((int)(cp2.y/cp2.num) - j)));
								finger2.x=i;
								finger2.y=j;
							}
					}
				if(screenOn)
					cvCircle(Map, cvPoint((int)((blob.m_recBlobs[1].x-scr1.x+(int)(cp2.x/cp2.num))*mapRate), (int)((blob.m_recBlobs[1].y-scr1.y+(int)(cp2.y/cp2.num))*mapRate)), 5, CV_RGB(255, 0, 0), 3);
				else
					cvCircle(image, cvPoint(blob.m_recBlobs[1].x+(int)(cp2.x/cp2.num), blob.m_recBlobs[1].y+(int)(cp2.y/cp2.num)), 5, CV_RGB(0, 255, 0), 3);
			//	cvCircle(image, cvPoint(blob.m_recBlobs[0].x+(int)(cp.y/cp.num), blob.m_recBlobs[0].x+(int)(cp.x/cp.num)), 5, CV_RGB(0, 255, 0), 3);
				//cvCircle(Roi, cvPoint(blob.m_recBlobs[0].x+(int)(cp.y/cp.num), blob.m_recBlobs[0].x+(int)(cp.x/cp.num)), 5, CV_RGB(0, 255, 0), 3);

				int rad = (int)sqrt((double)(((int)(cp2.x/cp2.num) - finger2.x)*((int)(cp2.x/cp2.num) - finger2.x)+
											 ((int)(cp2.y/cp2.num) - finger2.y)*((int)(cp2.y/cp2.num) - finger2.y)));

			//	cvCircle(image, cvPoint(blob.m_recBlobs[1].x+(int)(cp2.x/cp2.num), blob.m_recBlobs[1].y+(int)(cp2.y/cp2.num)), (int)(rad/1.5), CV_RGB(255, 0, 0), 1);
			//	cvLine(image, cvPoint(blob.m_recBlobs[1].x+(int)(cp2.x/cp2.num), blob.m_recBlobs[1].y+(int)(cp2.y/cp2.num)),
			//		cvPoint(blob.m_recBlobs[1].x+finger2.x, blob.m_recBlobs[1].y+finger2.y), CV_RGB(255,255,0), 1);

				circle2 = cvCreateImage(cvGetSize(Roi2), 8, 1);
				cvSetZero(circle2);
				cvCircle(circle2, cvPoint((int)(cp2.x/cp2.num), (int)(cp2.y/cp2.num)), (int)(rad/1.5), CV_RGB(255, 255, 255), 3);
				cvAnd(Roi2, circle2, circle2, 0);

				labeled3 = cvCreateImage(cvGetSize(circle2), 8,3);
				cvCvtColor(circle2, labeled3, CV_GRAY2BGR);
		
				blob3.SetParam(circle2, 20);
				blob3.DoLabeling();
		
				//blob.BlobBigSizeConstraint(circle->width, circle->height);
				//blob.BlobSmallSizeConstraint(40, 40);
				
				int imageW = image->width;
				int imageH = image->height;
				cvPutText(image, motion[blob3.m_nBlobs-1], cvPoint(imageW-50,50), &font, cvScalar(0,0,255));

				for(i=0; i<blob3.m_nBlobs; i++)
				{
				
					CvPoint pt1= cvPoint(blob3.m_recBlobs[i].x, blob3.m_recBlobs[i].y);
					CvPoint pt2= cvPoint(pt1.x+blob3.m_recBlobs[i].width, pt1.y+blob3.m_recBlobs[i].height);

					CvScalar color = cvScalar(0,0,255);

					cvDrawRect(labeled3, pt1, pt2, color, 2);
					if(screenOn)
						cvPutText(Map, motion[blob3.m_nBlobs-1], cvPoint(imageW-50,50), &font, cvScalar(0,0,255));
				}
				if(blob2.m_nBlobs>=5&&blob3.m_nBlobs>=5)
					screenSet=true;

				if(screenSet&&!screenOn)
				{
					
					double rate;
				
					if(blob.m_recBlobs[0].x+(int)(cp.x/cp.num)<blob.m_recBlobs[1].x+(int)(cp2.x/cp2.num))
					{
						rate=((double)(blob.m_recBlobs[1].x+(int)(cp2.x/cp2.num)-blob.m_recBlobs[0].x+(int)(cp.x/cp.num)))/Map->width;
						scr1=cvPoint(blob.m_recBlobs[0].x+(int)(cp.x/cp.num), blob.m_recBlobs[0].y+(int)(cp.y/cp.num)-(int)(Map->height*rate/2));
						scr2=cvPoint(blob.m_recBlobs[1].x+(int)(cp2.x/cp2.num), blob.m_recBlobs[0].y+(int)(cp.y/cp.num)+(int)(Map->height*rate/2));
					}else
					{
						rate=((double)(blob.m_recBlobs[0].x+(int)(cp.x/cp.num)-blob.m_recBlobs[1].x+(int)(cp2.x/cp2.num)))/Map->width;
						scr1=cvPoint(blob.m_recBlobs[1].x+(int)(cp2.x/cp2.num), blob.m_recBlobs[1].y+(int)(cp2.y/cp2.num)-(int)(Map->height*rate/2));
						scr2=cvPoint(blob.m_recBlobs[0].x+(int)(cp.x/cp.num), blob.m_recBlobs[1].y+(int)(cp2.y/cp2.num)+(int)(Map->height*rate/2));
					}

			//		cvDrawRect(image, scr1,scr2, CV_RGB(255,0,255), 3);
					ARResize = cvCreateImage(cvSize(scr2.x-scr1.x, scr2.y-scr1.y), 8, 3);
					cvResize(AR, ARResize, CV_INTER_LINEAR);
			//		cvShowImage("resize",ARResize);
					for(int ii=0; ii<ARResize->height; ii++)
						for(int jj=0; jj<ARResize->widthStep; jj++)
						{
							if((ii+scr1.y)>=0&&(jj+scr1.x*3)>=0&&(ii+scr1.y)<image->height&&(jj+scr1.x*3)<image->width*3)
								image->imageData[(ii+scr1.y)*image->widthStep+(jj+scr1.x*3)]=ARResize->imageData[ii*ARResize->widthStep+jj];
						}
					if(blob2.m_nBlobs<=2&&blob3.m_nBlobs<=2){
						screenOn=true;
						screenSet=false;
						mapRate = Map->width/((double)(scr2.x-scr1.x));
					}
				}
			}

			if(screenOn&&blob2.m_nBlobs<=2&&sqrt((double)(((int)((blob.m_recBlobs[0].x-scr1.x+(int)(cp.x/cp.num))*mapRate)-(AR->width-100))*((int)((blob.m_recBlobs[0].x-scr1.x+(int)(cp.x/cp.num))*mapRate)-(AR->width-100))
				+((int)((blob.m_recBlobs[0].y-scr1.y+(int)(cp.y/cp.num))*mapRate)-(AR->height-100))*((int)((blob.m_recBlobs[0].y-scr1.y+(int)(cp.y/cp.num))*mapRate)-(AR->height-100))))<=50)
				break;
		}
		////
		if(screenOn){
			cvDrawRect(image, scr1,scr2, CV_RGB(255,0,255), 2);
			cvCircle(Map, cvPoint(Map->width-100, Map->height-100), 50, CV_RGB(255, 0, 0), 3);
		}

//		cvShowImage("Map",grayImage);
//		if(!screenOn)
//			cvShowImage("image",image);
	//	else
//			cvShowImage("image",Map);
//		cvShowImage("skin",imageCopy);
//		cvShowImage("circle", labeled2);
//		cvShowImage("hand", Roi);
		//cvShowImage("hand", Roi2);
		cvShowImage("msk",msk);

		//
//		cvReleaseImage(&preFrame);
		cvReleaseImage(&U);
		cvReleaseImage(&B);
		cvReleaseImage(&G);
		cvReleaseImage(&R);
		cvReleaseImage(&Roi);
		cvReleaseImage(&circle);
		cvReleaseImage(&circle2);
		cvReleaseImage(&labeled2);
		cvReleaseImage(&labeled3);
		cvReleaseImage(&ARResize);
		cvReleaseImage(&msk);
	//	delete faces;
	//	preFrame = cvCreateImage(cvGetSize(image), 8,3);
	//	cvCopy(image, preFrame, 0);
		
		//
		if(cvWaitKey(1) == 27)
		{
			break;
		}
	}

	cvReleaseImage(&grayImage);
	cvReleaseImage(&labeled);
	cvReleaseImage(&hand);
//	cvReleaseImage(&preFrame);
	cvReleaseImage(&av);
	cvReleaseImage(&sgm);
	cvReleaseImage(&lower);
	cvReleaseImage(&upper);
	cvReleaseImage(&tmp);
	cvReleaseImage(&dst);
	cvReleaseImage(&msk);

	cvReleaseCapture(&capture);

	cvDestroyAllWindows();
}

///

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


