#include "HandMouse.h"

void HandMouse::eventHandler(motion curs)
{
	SetCursorPos((int)(curs.x*GetSystemMetrics(SM_CXSCREEN)), (int)(curs.y*GetSystemMetrics(SM_CYSCREEN)));
	/*if(curs.mot<=2)
		if(dragOnCount==2)
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
		}
		else{
			dragOnCount++;
			dragOffCount=0;
			clickCount=0;
		}
	if(curs.mot==5)
		if(dragOffCount==3)
			mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
		else{
			dragOnCount=0;
			dragOffCount++;
			clickCount=0;
		}*/
	if(wait>0)
		wait--;
	else
	{
		
		if(curs.mot<=2)
			if(clickCount==2)
			{
				mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
				mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
				clickCount=0;
				wait=10;
			}else{
				dragOnCount=0;
				clickCount++;
			}
		else if(curs.mot==4)
			if(clickCount==2)
			{
				mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
				dragOnCount=0;
				wait=10;
			}else{
				dragOnCount++;
				clickCount=0;
			}
		else if(curs.mot==6)
		{
			mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
			dragOnCount=0;
			clickCount=0;
		}else if(curs.mot==5)
		{
			dragOnCount=0;
			clickCount=0;
		}
	}
}