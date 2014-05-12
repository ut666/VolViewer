/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef TFUNCSTATE_H
#define TFUNCSTATE_H

#include <iostream>
#include <fstream>
using namespace std;

#include "FunctionEditor.h"

class TfuncState
{
	public:

		float functioneditor_size;
		int functioneditor_border;
		int functioneditor_offset;
		FunctionEditor functioneditor;

		int brightness;
		int contrast;
		int threshold_min;
		int threshold_max;

		TfuncState();
		~TfuncState();

		void clear();
		void init(float s, int b, int o, int bright, int cont, int min, int max);
		void reset(void);

		void save(ofstream& fout);

		const TfuncState& operator= (const TfuncState& right);
};

#endif // TFUNCSTATE_H
