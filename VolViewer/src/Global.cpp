/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "Global.h"

float divsize = 16;
int count_ = 0;
int progress_iter_steps = 0;
int progress_iter=0;

#define COW 2172
char* moo = "MO ";

//ascii progressbar, 
//takes current value, size, and number of bars
void progress(int i, int size)
{
	//int j;
	//j = COW-moo[i&3],fputs((const char*)&j,stderr);

	/*float prog = ( (float)(i+1)/(float)(size) )*100.0f;
	printf("\r\r Progress = %.2f", prog);*/

	if(i==0)
	{
		divsize = 16;
		count_=0;
		progress_iter_steps = int(((float)size/(float)divsize)+0.5);
		progress_iter=0;
	}

	QString t;
	int k;

	if(progress_iter==i)
	{			
		count_++;
		
		t="[";

		for(k=0; k<(count_); k++)
			t+="*";

		for(k=0; k<divsize-(count_); k++)
			t+=" ";

		t+="]";

//		float numb= ((float)count_/(float)divsize)*100.0f

		printf("\r\r Progress = %s", t.latin1());
		fflush(stdout);

		progress_iter += progress_iter_steps;
	}

	if(i==size)
	{
	/*	t="[";
		for(k=0; k<divsize+2; k++)
			t+="*";
		t+="]";

		printf("\r\r Progress = %s\n", t.latin1());
		fflush(stdout);*/
		printf("\n");
	}

//	float prog = ( (float)(i+1)/(float)(size) )*100.0f;
//	printf("\r\r Progress = %.2f", prog);
}

QString setUnit(float val)
{
	QString numb;
	if(val>9999.0f)
	{
		val /= 10000.0;
		numb.setNum(val);
		numb += " c";
	}
	else if(val>999.0f)
	{
		val /= 1000.0;
		numb.setNum(val);
		numb += " m";
	}
	else
	{
		numb.setNum(val);
		numb += QChar(0x00B5);//QString::fromStdString (L" \u00B5");
	}
	numb += "m";

	return numb;
}