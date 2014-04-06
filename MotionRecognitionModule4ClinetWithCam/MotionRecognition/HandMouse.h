#include "MotionRecog.h"
#include <windows.h>

#ifndef _HANDMOUSE_H_
#define _HANDMOUSE_H_



class HandMouse{
private:
	int dragOnCount;
	int dragOffCount;
	int clickCount;
	int wait;
public:
	HandMouse(){dragOnCount=0;clickCount=0;}
	void eventHandler(motion curs);
	
};

#endif