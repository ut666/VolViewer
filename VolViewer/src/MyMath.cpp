/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "MyMath.h"

float roundup(float number, int decplaces)
{
	float dp=1;

	for(int i=0; i<decplaces; i++)
	{
		dp *= 10.0;
	}
	
	int numb = int((number*dp)+0.5);
	float result = (float)(numb)/(float)(dp);

	return result;
}
float randomFloat(float min, float max)
{
	float r = (float)rand() / (float)RAND_MAX;
	return min + r * (max - min);
}

// ***************************************************************************
// some fast and utility maths funcs
// ***************************************************************************
/*
__forceinline static
__declspec(naked)
float __fastcall
fsin(float a) 
{
	__asm {
		fld		DWORD PTR [esp+4] 
		fsin
		ret 4
	}
}

__forceinline static
__declspec(naked) 
float __fastcall
fcos(float a) 
{
	__asm {
		fld		DWORD PTR [esp+4]
		fcos
		ret 4
	}
}*/

