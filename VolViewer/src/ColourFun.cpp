/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "ColourFun.h"

/*
HSV SPACE
---------
Hue				0 to 360 degrees around the cone 
Saturation		0 to 1, distance from the centre of the cone 
Value			0 to 1, vertical position in the cone where 0 = black and 1 = white 
*/

void HSVtoRGB(Vector hsv, Vector* rgb) 
{ 
    int i; 
    float f, p, q, t,hTemp; 
  
    if( hsv.y == 0.0 || hsv.x == -1.0) // s==0? Totally unsaturated = grey so R,G and B all equal value 
    { 
      rgb->x = rgb->y = rgb->z = hsv.z; 
      return; 
    }
	
    hTemp = hsv.x/60.0f; 
    i = (int)floor( hTemp );                 // which sector 
    f = hTemp - i;                      // how far through sector 
    p = hsv.z * ( 1 - hsv.y ); 
    q = hsv.z * ( 1 - hsv.y * f ); 
    t = hsv.z * ( 1 - hsv.y * ( 1 - f ) ); 
  
    switch( i )  
    { 
    case 0:{rgb->x = hsv.z;	rgb->y = t;		rgb->z = p;break;} 
    case 1:{rgb->x = q;		rgb->y = hsv.z;	rgb->z = p;break;} 
    case 2:{rgb->x = p;		rgb->y = hsv.z;	rgb->z = t;break;} 
    case 3:{rgb->x = p;		rgb->y = q;		rgb->z = hsv.z;break;}  
    case 4:{rgb->x = t;		rgb->y = p;		rgb->z = hsv.z;break;} 
    case 5:{rgb->x = hsv.z;	rgb->y = p;		rgb->z = q;break;} 
    } 
} 

void RGBtoHSV(Vector rgb, Vector* hsv)
{ 

    float mn=rgb.x,mx=rgb.x; 
    int maxVal=0; 
  
    if (rgb.y > mx){ mx=rgb.y;maxVal=1;} 
    if (rgb.z > mx){ mx=rgb.z;maxVal=2;}  
    if (rgb.y < mn)	 mn=rgb.y; 
    if (rgb.z < mn)  mn=rgb.z;  

    float  delta = mx - mn; 
  
    hsv->z = mx;  
    if( mx != 0 ) 
      hsv->y = delta / mx;  
    else  
    { 
      hsv->y = 0; 
      hsv->x = 0; 
      return; 
    } 
    if (hsv->y==0.0f) 
    { 
      hsv->x=-1; 
      return; 
    } 
    else 
    {  
      switch (maxVal) 
      { 
      case 0:{hsv->x = ( rgb.y - rgb.z ) / delta;break;}         // yel < h < mag 
      case 1:{hsv->x = 2 + ( rgb.z - rgb.x ) / delta;break;}     // cyan < h < yel 
      case 2:{hsv->x = 4 + ( rgb.x - rgb.y ) / delta;break;}     // mag < h < cyan 
      } 
    } 
  
    hsv->x *= 60; 
    if( hsv->x < 0 ) hsv->x += 360; 
}

void CompHSV(Vector hsv, Vector* comphsv)
{
	if(hsv.x>=180.0f) comphsv->x = hsv.x - 180.0f;
	else comphsv->x = hsv.x + 180.0f;

	comphsv->y = hsv.y;
	comphsv->z = hsv.z;
}
 