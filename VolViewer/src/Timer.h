/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef TIMER_H
#define TIMER_H

#include <stdio.h>
#include <iostream>
#include <QTime>
using namespace std;

class Timer
{
	public:

		QTime start;
		QTime end;
		float deltaTime;

		QTime fps_start;
		QTime fps_end;
		float fps;
		int   fps_count;  				// Frames per second count 
		float fps_time; 				// Frames per second time
			
		//-------------------------------------------------------------------		
		Timer();
		
		void startFPS(void);
		float endFPS(void);

		void startTimer(void);
		float endTimer(QString msg);
};

#endif //TIMER
