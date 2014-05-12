/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtThreadedSliceLoader.h"

QtThreadedSliceLoader::QtThreadedSliceLoader(VolumeObject* vo, QString filename, QString e, int x, int y, int z, int m)
{
	volobj = vo;

	fn = filename;
	ext = e;
	x2=x;
	y2=y;
	z2=z;
	mode=m;
}

QtThreadedSliceLoader::~QtThreadedSliceLoader()
{

}
void QtThreadedSliceLoader::run()
{
	/*printf("fn: %s\n",fn.latin1());
	printf("ext: %s\n",ext.latin1());
	printf("x2: %d\n",x2);
	printf("y2: %d\n",y2);
	printf("z2: %d\n",z2);
	printf("mode: %d\n",mode);*/

	QImage img;
	img.load(fn, ext);
	int index_small;
	int r,g,b;

	for(float x=0	; x<volobj->width-1; x+=volobj->x_resize)
	{
		for(float y=0; y<volobj->height-1; y+=volobj->y_resize)
		{
			//make sure we round up to nearest int for x,y
			QRgb pixel = img.pixel(int(x+0.5),int(y+0.5));
		
			if(x2>=volobj->resized_texwidth)  x2=volobj->resized_texwidth-1;
			if(y2>=volobj->resized_texheight) y2=volobj->resized_texheight-1;
			if(z2>=volobj->resized_texdepth)  z2=volobj->resized_texdepth-1;

			index_small = (((((z2)*volobj->resized_texheight)+(y2))*volobj->resized_texwidth)+(x2));

			r = qRed(pixel);
			g = qGreen(pixel);
			b = qBlue(pixel);

			if(r!=0 || g!=0 || b!=0)
			{
				if(mode==4)
				{
					volobj->texture3d[3*index_small+0] = int(r);
					volobj->texture3d[3*index_small+1] = int(g); 
					volobj->texture3d[3*index_small+2] = int(b); 
					volobj->histogram[(3*int(r))+0]++;
					volobj->histogram[(3*int(g))+1]++;
					volobj->histogram[(3*int(b))+2]++;
				}
				else if(mode==1)
				{
					volobj->texture3d[3*index_small+0] = int(r);
					volobj->histogram[(3*int(r))+0]++;
				}
				else if(mode==2)
				{
					volobj->texture3d[3*index_small+1] = int(g); 
					volobj->histogram[(3*int(g))+1]++;
				}
				else if(mode==3)
				{
					volobj->texture3d[3*index_small+2] = int(b); 
					volobj->histogram[(3*int(b))+2]++;
				}					
			}
			y2++;
		}
		x2++;
		y2=0;
	}
}