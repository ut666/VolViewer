#include "ColourMap.h"

ColourMap::ColourMap()
{
}
ColourMap::~ColourMap()
{
}
void ColourMap::clear(void)
{
	colourmap_cols.clear();
	colourmap_vals.clear();
}
void ColourMap::add_entry(float val, Vector col)
{
	colourmap_cols.push_back(col);
	colourmap_vals.push_back(val);
}
Vector ColourMap::get_col(int index)
{
	return colourmap_cols[index];
}
float ColourMap::get_val(int index)
{
	return colourmap_vals[index];
}
int ColourMap::getSize(void)
{
	/*if(colourmap_cols.size()!=colourmap_vals.size())
		return -1;*/

	return colourmap_cols.size();
}
void ColourMap::delete_entry(int index)
{
	if(colourmap_cols.empty() || colourmap_vals.empty()) return;

	colourmap_cols.erase(colourmap_cols.begin()+index);
	colourmap_vals.erase(colourmap_vals.begin()+index);
}
Vector ColourMap::compute_colour(float min, float max, float current)
{
	if(colourmap_cols.empty() || colourmap_vals.empty()) return Vector(0,0,0);

	Vector col;
	
	float minvalue, maxvalue;
	int minindex, maxindex;
	float interp = (current-min)/(max-min);

	for(int i=0; i<colourmap_cols.size(); i++)
	{
		if(interp>=colourmap_vals[i])
		{
			minvalue = colourmap_vals[i];
			minindex = i; 
		}
	}

	if(minindex>=colourmap_cols.size()-1) minindex = colourmap_cols.size()-2;
	maxindex = minindex + 1;

	Vector mincol, maxcol;
	Vector rgb;

	mincol = Vector((float)colourmap_cols[minindex].x/255.0, (float)colourmap_cols[minindex].y/255.0, (float)colourmap_cols[minindex].z/255.0);
	maxcol = Vector((float)colourmap_cols[maxindex].x/255.0, (float)colourmap_cols[maxindex].y/255.0, (float)colourmap_cols[maxindex].z/255.0);

	interp = ((float)interp-(float)colourmap_vals[minindex])/ (float)(colourmap_vals[maxindex]-colourmap_vals[minindex]);
	
	if(interp<0.0) interp = 0.0;
	if(interp>1.0) interp = 1.0;

	rgb = ((1.0f-interp)*mincol) + (interp*maxcol);	

	col = Vector((float)rgb.x*255.0f, (float)rgb.y*255.0f, (float)rgb.z*255.0f, 255);

	return col;
}
/*QPixmap QtColourMap::compute_pixmap(void)
{
	QPixmap pixmap_pm;
	pixmap_pm = QPixmap(256, 64);

    QPainter p;
	p.begin(&pixmap_pm);
	p.setBrush(Qt::SolidPattern);
	for (int x=0; x<256; x++)
	{			
		p.setPen(compute_colour(0, 255, x));
		p.drawLine(x, 0, x, 63);
	}
	p.end();

	return pixmap_pm;
}*/
const ColourMap& ColourMap::operator= (const ColourMap& right)
{
	clear();

	if(right.colourmap_cols.size()!=right.colourmap_vals.size())
	{
		printf("ERROR with right hand side object\n");
		return *this;
	}

	colourmap_vals.resize(right.colourmap_vals.size());
	colourmap_cols.resize(right.colourmap_cols.size());

	for(int i=0; i<right.colourmap_cols.size(); i++)
	{
		colourmap_vals[i] = right.colourmap_vals[i];
		colourmap_cols[i] = right.colourmap_cols[i];
	}

	return *this;
}