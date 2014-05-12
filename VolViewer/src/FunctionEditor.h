/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef FUNCEDITOR_H
#define FUNCEDITOR_H

#include <qgl.h>
#include "Vector.h"
#include "BSpline.h"

class FunctionEditor
{
public:

	//(NB) Function space is always 0,0 -> 1,1

	//temp vector for delete_event
	Vector v;
	float x_min;
	float x_max;

	float size;

	//a delete event
	int delete_event;

	//which cp is selected
	int selected;

	//our delete zone size
	float delete_zone;
	//our repultion factor
	float repel_factor;

	//the bspline
	BSpline bs;

	FunctionEditor();
	~FunctionEditor();

	void clear(void);
	void init(float size, int border, int offset);
	void reset();

	void addPoint(int x, int y);
	void deletePoint(void);
	void movePoint(int x, int y);
	int selectPoint(int x, int y);
	
	void constrain_in_bb(void);
	void constrain_in_neighbours(void);
	void quicksortPoints(int lo, int hi);

	void display(void);
	void resize(int w, int h);
	const FunctionEditor& operator= (const FunctionEditor& right);
};

#endif // #define FUNCEDITOR_H
