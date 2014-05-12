/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "Timer.h"

Timer::Timer()
{	
	fps_count = 0;  
	fps_time = 0; 				
	fps = 0;
}
void Timer::startFPS(void)
{
	fps_start = QTime::currentTime();
}
float Timer::endFPS(void)
{
	float deltaT;
	fps_end = QTime::currentTime();
	deltaT = fps_start.msecsTo(fps_end);
	fps_time += deltaT;
	fps_count++;

	if (fps_count >= 10)
	{	
		fps = (fps_count / fps_time) * 1000.0f;				
		fps_time  = 0.0;
		fps_count = 0;
	}

	return fps;
}
void Timer::startTimer(void)
{
	start = QTime::currentTime();
}
float Timer::endTimer(QString msg)
{
	end = QTime::currentTime();

	float elapsed_time = start.msecsTo(end); 

	printf("%s - elapsed_time = %f (ms)\n", msg.latin1(), elapsed_time);

	return elapsed_time;
}

