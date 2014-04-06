#include "MotionRecog.h"
#include "HandMouse.h"

void main()
{
	MotionRecog motion;
	
	HandMouse HM;

	cvNamedWindow(" ", CV_WINDOW_NORMAL);
    //cvSetWindowProperty(" ", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	WinExec("project.exe",SW_SHOW);
	while(1)
	{
	//	motion.setImage();
	//	cvShowImage(" ", motion.skinDetecting(motion.getImage()));
		
	//	cvShowImage(" ", motion.handDetecting(motion.getImage()));
		cvShowImage(" ", motion.getImg());
		HM.eventHandler(motion.getMotion());
		if(cvWaitKey(1) == 27)
		{
			break;
		}
	}

	
}
