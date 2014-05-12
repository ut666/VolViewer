#ifndef COLOURMAP_H
#define COLOURMAP_H

#include <vector>
using namespace std;

#include "Vector.h"

class ColourMap
{
public:

	vector< Vector > colourmap_cols;
	vector< float > colourmap_vals;

	ColourMap();
	~ColourMap();

	void clear(void);
	void add_entry(float val, Vector col);
	void delete_entry(int index);
	Vector get_col(int index);
	float get_val(int index);
	int getSize(void);

	Vector compute_colour(float min, float max, float current);

	const ColourMap& operator= (const ColourMap& right);
};

#endif // QTCOLOURMAP_H